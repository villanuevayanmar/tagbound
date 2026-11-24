#include "game.hpp"
#include "raymath.h"
#include <algorithm>
#include <cmath>
#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#define KEEP EMSCRIPTEN_KEEPALIVE
EM_JS(void, saveLocal, (int city,int level,int xp,int unlocked,int klass,int weapon,int finished,int quest), {
  try { localStorage.setItem("tagbound-save",JSON.stringify({schemaVersion:1,city,level,xp,unlocked,klass,weapon,finished,quest})); } catch(e) {}
});
EM_JS(int, loadSaveVersion, (), {
  try { const s=JSON.parse(localStorage.getItem("tagbound-save")||"{}"); return Number.isInteger(s.schemaVersion)?s.schemaVersion:0; } catch(e) { return 0; }
});
EM_JS(int, loadLocal, (int field), {
  try { const s=JSON.parse(localStorage.getItem("tagbound-save")||"{}"); const k=["city","level","xp","unlocked","klass","weapon","finished","quest"]; return Number.isFinite(s[k[field]])?s[k[field]]:-1; } catch(e) { return -1; }
});
EM_JS(void, saveExploration, (int city,const char* data), {
  try { localStorage.setItem("tagbound-exploration-v2-"+city,UTF8ToString(data)); } catch(e) {}
});
EM_JS(void, loadExploration, (int city,char* output,int capacity), {
  if(capacity<=0) return;
  try {
    const current=localStorage.getItem("tagbound-exploration-v2-"+city);
    const legacy=localStorage.getItem("tagbound-exploration-v1-"+city);
    stringToUTF8(current||legacy||"",output,capacity);
  } catch(e) { HEAPU8[output]=0; }
});
EM_JS(void, setMapControlsState, (int open,int floor,int canPrevious,int canNext), {
  document.getElementById("touch-controls")?.classList.toggle("map-open",!!open);
  document.querySelector(".action.map")?.setAttribute("aria-label",open?"Close explored map":"Open explored map");
  const panel=document.getElementById("map-floor-controls");
  panel?.classList.toggle("hidden",!open);
  const previous=document.getElementById("map-floor-prev");
  const next=document.getElementById("map-floor-next");
  const label=document.getElementById("map-floor-label");
  if(previous) previous.disabled=!canPrevious;
  if(next) next.disabled=!canNext;
  if(label) label.textContent=["GROUND","ROOFTOP 1","ROOFTOP 2","ROOFTOP 3","BOSS DECK"][Math.max(0,Math.min(4,floor))];
});
EM_JS(void, hideIntro, (), { document.getElementById("intro")?.classList.add("hidden"); });
EM_JS(void, showEnding, (), { window.TagboundAudio?.pauseMusic?.(); document.getElementById("ending")?.classList.remove("hidden"); });
EM_JS(void, initGameAudio, (), { try { window.TagboundAudio?.init?.(); } catch (e) {} });
EM_JS(void, playGameAudio, (int kind), { try { window.TagboundAudio?.play?.(kind); } catch (e) {} });
EM_JS(int, isSettingsOpen, (), { return document.getElementById("settings")?.classList.contains("hidden") ? 0 : 1; });
#else
#define KEEP
static void saveLocal(int,int,int,int,int,int,int,int) {}
static int loadSaveVersion() { return 0; }
static int loadLocal(int) { return -1; }
static void saveExploration(int,const char*) {}
static void loadExploration(int,char*,int) {}
static void setMapControlsHidden(int) {}
static void hideIntro() {}
static void showEnding() {}
static void initGameAudio() {}
static void playGameAudio(int) {}
static int isSettingsOpen() { return 0; }
#endif

static GameState game;
static Camera3D camera{};
static int selectedClass=0, touchButtons=0;
static float touchX=0,touchY=0,lookX=0,lookY=0,attackCooldown=0,saveCooldown=0;
static bool skillWasDown=false,jumpWasDown=false,classChosen=false;
static int savedCity=-1;

static void Save() {
    saveLocal(game.city,game.player.level,game.player.xp,game.unlockedCity,(int)game.player.playerClass,(int)game.player.weapon,game.finished?1:0,game.quest);
}
static void SaveExploration() {
    std::array<char,ExplorationMapTotalCells+1> encoded{};
    for(int i=0;i<ExplorationMapTotalCells;i++) encoded[i]=game.explored[i]?'1':'0';
    saveExploration(game.city,encoded.data());
}
static void LoadExploration() {
    std::array<char,ExplorationMapTotalCells+1> encoded{};
    game.explored.fill(0);
    game.currentFloor=0;
    game.mapFloor=0;
    loadExploration(game.city,encoded.data(),(int)encoded.size());
    for(int i=0;i<ExplorationMapTotalCells&&encoded[i];i++) game.explored[i]=encoded[i]=='1'?1:0;
    // A v1 map had no floor dimension. Its existing discovery is retained on ground.
}
extern "C" {
KEEP void ChooseClass(int c) { selectedClass=std::clamp(c,0,2); classChosen=true; }
KEEP void StartGame() {
    game.player.playerClass=(PlayerClass)selectedClass;
    game.player.health=ClassMaxHealth(game.player.playerClass);
    game.player.stamina=ClassMaxStamina(game.player.playerClass);
    game.player.weapon=ClassWeapon(game.player.playerClass);
    int saveVersion=loadSaveVersion();
    int c=loadLocal(0), l=loadLocal(1), x=loadLocal(2), u=loadLocal(3), cl=loadLocal(4), finished=loadLocal(6), quest=loadLocal(7);
    if(c>=0) {
        if(saveVersion<1) {
            c=MigrateLegacyCityIndex(c);
            u=MigrateLegacyCityIndex(u);
        }
        game.city=std::clamp(c,0,4); game.unlockedCity=std::clamp(u,0,4);
        game.player.level=std::max(1,l); game.player.xp=std::max(0,x);
        if(!classChosen) {
            game.player.playerClass=(PlayerClass)std::clamp(cl,0,2);
            game.player.health=ClassMaxHealth(game.player.playerClass);
            game.player.stamina=ClassMaxStamina(game.player.playerClass);
        }
        game.player.escapeSkill=game.player.level>=3; game.player.combatSkill=game.player.level>=5;
    }
    game.player.weapon=ClassWeapon(game.player.playerClass);
    game.started=true; game.finished=finished>0; game.mapOpen=false; SyncMapControls(); BuildCity(game);
    if(game.finished) game.quest=std::clamp(quest<0?0:quest,0,3);
    else {
        SetQuestProgress(game,quest<0?0:quest);
        game.player.position=QuestCheckpoint(game.city,game.quest);
        game.player.facing=QuestCheckpointFacing(game.city,game.quest);
    }
    LoadExploration();
    UpdateExploration(game,game.player.position);
    SaveExploration();
    ResetCamera(game.player,camera);
    savedCity=game.city; initGameAudio(); hideIntro(); Save();
    if(game.finished) showEnding();
}
KEEP void SaveNow() { Save(); }
KEEP void SetTouch(float x,float y,int buttons) { touchX=std::clamp(x,-1.0f,1.0f); touchY=std::clamp(y,-1.0f,1.0f); touchButtons=buttons; }
KEEP void SetLook(float x,float y) { lookX+=x; lookY+=y; }
KEEP void ResetLook() { lookX=lookY=0; }
KEEP int IsInputStateClear() {
    return touchX==0.0f && touchY==0.0f && touchButtons==0 && lookX==0.0f && lookY==0.0f ? 1 : 0;
}
KEEP int GetQuestProgress() { return game.quest; }
KEEP int IsCurrentQuestHoldObjective() { return QuestRequiresHold(game.city,game.quest)?1:0; }
KEEP float GetBeaconHoldProgress() { return game.beaconHoldTime; }
KEEP float GetCurrentQuestHoldDuration() { return QuestHoldDuration(game.city,game.quest); }
KEEP int IsCurrentQuestSurvivalObjective() { return QuestRequiresSurvival(game.city,game.quest)?1:0; }
KEEP float GetCurrentQuestSurvivalDuration() { return QuestSurvivalDuration(game.city,game.quest); }
KEEP int IsGameUpdating() { return game.started && !game.finished && !isSettingsOpen() ? 1 : 0; }
static void SyncMapControls() {
    setMapControlsState(game.mapOpen?1:0,game.mapFloor,
        CanChangeExplorationFloor(game,-1)?1:0,
        CanChangeExplorationFloor(game,1)?1:0);
}
KEEP void ToggleMap() {
    if(!game.started||game.finished||isSettingsOpen()) return;
    game.mapOpen=!game.mapOpen;
    if(game.mapOpen) game.mapFloor=game.currentFloor;
    SyncMapControls();
}
KEEP void ChangeMapFloor(int direction) {
    if(!game.mapOpen) return;
    if(ChangeExplorationFloor(game,direction)) SyncMapControls();
}
KEEP int IsMapOpen() { return game.mapOpen?1:0; }
KEEP int GetMapFloor() { return game.mapFloor; }
KEEP int CanChangeMapFloor(int direction) {
    return CanChangeExplorationFloor(game,direction)?1:0;
}
KEEP int GetExploredCount() {
    const int start=game.mapFloor*ExplorationMapCells;
    return (int)std::count(game.explored.begin()+start,
        game.explored.begin()+start+ExplorationMapCells,(unsigned char)1);
}
KEEP int GetExploredCell(int index) {
    if(index<0||index>=ExplorationMapCells) return 0;
    return game.explored[game.mapFloor*ExplorationMapCells+index];
}
KEEP int GetActiveBossCount() {
    return (int)std::count_if(game.enemies.begin(),game.enemies.end(),[](const Enemy& e){return e.alive&&e.boss;});
}
KEEP int IsBossArenaContained() {
    int count=0;
    for(const Enemy& e:game.enemies) if(e.alive&&e.boss) {
        ++count;
        if(!BossArenaContains(game,e)) return 0;
    }
    return count==1?1:0;
}
KEEP void ResetProgress() {
    game.city=0; game.unlockedCity=0; game.quest=0; game.bossUnlocked=false;
    game.player.level=1; game.player.xp=0;
    game.player.escapeSkill=0; game.player.combatSkill=0; game.finished=false; game.mapOpen=false; setMapControlsHidden(0); savedCity=0; Save();
}
}

static Controls ReadControls() {
    Controls c{};
    c.move.x=(float)(IsKeyDown(KEY_D)||IsKeyDown(KEY_RIGHT))-(float)(IsKeyDown(KEY_A)||IsKeyDown(KEY_LEFT))+touchX;
    c.move.y=(float)(IsKeyDown(KEY_W)||IsKeyDown(KEY_UP))-(float)(IsKeyDown(KEY_S)||IsKeyDown(KEY_DOWN))-touchY;
    c.sprint=IsKeyDown(KEY_LEFT_SHIFT)||IsKeyDown(KEY_RIGHT_SHIFT)||(touchButtons&64);
    c.slide=IsKeyDown(KEY_LEFT_CONTROL)||IsKeyDown(KEY_C)||(touchButtons&2);
    bool jumpDown=IsKeyDown(KEY_SPACE)||(touchButtons&1);
    c.jump=jumpDown&&!jumpWasDown;
    jumpWasDown=jumpDown;
    c.block=IsMouseButtonDown(MOUSE_BUTTON_RIGHT)||(touchButtons&8);
    c.aim=(touchButtons&16)||(IsMouseButtonDown(MOUSE_BUTTON_RIGHT)&&game.player.weapon==Weapon::Bow);
    bool skillDown=IsKeyDown(KEY_E)||(touchButtons&32);
    c.skill=skillDown&&!skillWasDown;
    skillWasDown=skillDown;
    c.lookX=lookX; c.lookY=lookY; lookX=lookY=0;
    if(IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) {
        Vector2 d=GetMouseDelta(); c.lookX+=d.x*0.7f; c.lookY+=d.y*0.7f;
    }
    attackCooldown=std::max(0.0f,attackCooldown-GetFrameTime());
    bool attack=IsMouseButtonPressed(MOUSE_BUTTON_LEFT)||(touchButtons&4);
    if(attack&&attackCooldown<=0) { c.attack=true; attackCooldown=game.player.weapon==Weapon::Bow?0.5f:0.42f; }
    return c;
}

static unsigned int BossWindupMask() {
    unsigned int mask=0;
    for(const Enemy& e:game.enemies)
        if(e.alive&&e.boss&&e.phase==BossPhase::Windup)
            mask|=1u<<std::clamp(e.variant,0,4);
    return mask;
}

static int TaggerWindupCount() {
    return (int)std::count_if(game.enemies.begin(),game.enemies.end(),
        [](const Enemy& e){return e.alive&&!e.boss&&e.attackWindup>0;});
}

extern "C" {
KEEP unsigned int GetBossWindupMask() { return BossWindupMask(); }
}

static void Frame() {
    float dt=std::min(GetFrameTime(),0.04f);
    if(game.started && !game.finished && !isSettingsOpen()) {
        Controls controls=ReadControls();
        if(!game.mapOpen) {
        game.player.blocking=controls.block;
        float previousY=game.player.position.y;
        bool wasDashing=game.player.dashTime>0;
        float previousHealth=game.player.health;
        int previousDefeated=game.defeated;
        unsigned int previousWindups=BossWindupMask();
        int previousTaggerWindups=TaggerWindupCount();
        if(controls.attack) playGameAudio(game.player.weapon==Weapon::Bow?1:0);
        UpdateMovement(game,controls,camera,dt);
        if(UpdateExploration(game,game.player.position)) SaveExploration();
        if(controls.jump&&game.player.position.y>previousY+0.01f) playGameAudio(3);
        if(!wasDashing&&game.player.dashTime>0) playGameAudio(2);
        float staminaBeforeCombat=game.player.stamina;
        UpdatePursuit(game,dt);
        if(TaggerWindupCount()>previousTaggerWindups) playGameAudio(7);
        if((BossWindupMask()&~previousWindups)!=0) playGameAudio(5);
        UpdateCombat(game,controls,dt);
        if(game.player.stamina<staminaBeforeCombat-0.01f) playGameAudio(8);
        if(game.player.health<previousHealth) playGameAudio(4);
        if(game.defeated>previousDefeated) playGameAudio(6);
        if(game.city!=savedCity) {
            savedCity=game.city;
            LoadExploration();
            UpdateExploration(game,game.player.position);
            SaveExploration();
            Save();
        }
        game.questTime+=dt;
        if(game.quest<3) {
            if(UpdateQuestObjective(game,dt)) {
                game.quest++;
                UpdateProgression(game,65+game.city*15+game.quest*10);
                game.questTime=0;
                game.questLimit=100.0f-game.city*8.0f-game.quest*9.0f;
                if(game.quest==3) SetQuestProgress(game,game.quest);
                Save();
            }
            if(game.questTime>game.questLimit) {
                Vector3 spawn{};
                if(FindReinforcementSpawn(game,game.player.position,spawn)) {
                    game.questTime=0;
                    float hp=42.0f+game.city*14.0f;
                    game.enemies.push_back({spawn,hp,hp,2.7f+game.city*0.3f,0,false,true});
                    game.player.stamina=std::max(10.0f,game.player.stamina-18);
                } else {
                    game.questTime=std::max(0.0f,game.questLimit-0.75f);
                }
            }
        }
        saveCooldown+=dt;
        if(saveCooldown>4.0f) { Save(); saveCooldown=0; }
        if(game.finished) { Save(); showEnding(); }
        }
    }
    BeginDrawing();
    DrawGame(game,camera);
    if(!game.started) {
        DrawRectangle(0,0,GetScreenWidth(),GetScreenHeight(),Fade(BLACK,0.4f));
        DrawText("A FUGITIVE. FIVE CITIES. NO WAY BACK.",GetScreenWidth()/2-245,GetScreenHeight()/2-12,22,GOLD);
    }
    EndDrawing();
}

int main() {
    SetConfigFlags(FLAG_WINDOW_RESIZABLE|FLAG_VSYNC_HINT);
    InitWindow(1280,720,"TAGBOUND - The Five Cities");
    SetTargetFPS(60);
    camera.position={0,4,-8}; camera.target={0,1,0}; camera.up={0,1,0};
    camera.fovy=60; camera.projection=CAMERA_PERSPECTIVE;
    BuildCity(game);
#ifdef __EMSCRIPTEN__
    emscripten_set_main_loop(Frame,0,1);
#else
    while(!WindowShouldClose()) Frame();
    CloseWindow();
#endif
    return 0;
}
