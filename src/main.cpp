#include "game.hpp"
#include "raymath.h"
#include <algorithm>
#include <cmath>
#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#define KEEP EMSCRIPTEN_KEEPALIVE
EM_JS(void, saveLocal, (int city,int level,int xp,int unlocked,int klass,int weapon,int finished,int quest), {
  try { localStorage.setItem("tagbound-save",JSON.stringify({city,level,xp,unlocked,klass,weapon,finished,quest})); } catch(e) {}
});
EM_JS(int, loadLocal, (int field), {
  try { const s=JSON.parse(localStorage.getItem("tagbound-save")||"{}"); const k=["city","level","xp","unlocked","klass","weapon","finished","quest"]; return Number.isFinite(s[k[field]])?s[k[field]]:-1; } catch(e) { return -1; }
});
EM_JS(void, hideIntro, (), { document.getElementById("intro")?.classList.add("hidden"); });
EM_JS(void, showEnding, (), { document.getElementById("ending")?.classList.remove("hidden"); });
#else
#define KEEP
static void saveLocal(int,int,int,int,int,int,int,int) {}
static int loadLocal(int) { return -1; }
static void hideIntro() {}
static void showEnding() {}
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
extern "C" {
KEEP void ChooseClass(int c) { selectedClass=std::clamp(c,0,2); classChosen=true; }
KEEP void StartGame() {
    game.player.playerClass=(PlayerClass)selectedClass;
    if(selectedClass==0) { game.player.health=125; game.player.stamina=95; game.player.weapon=ClassWeapon(PlayerClass::Vanguard); }
    if(selectedClass==1) { game.player.health=95; game.player.stamina=100; game.player.weapon=ClassWeapon(PlayerClass::Skirmisher); }
    if(selectedClass==2) { game.player.health=115; game.player.stamina=100; game.player.weapon=ClassWeapon(PlayerClass::Warden); }
    int c=loadLocal(0), l=loadLocal(1), x=loadLocal(2), u=loadLocal(3), cl=loadLocal(4), finished=loadLocal(6), quest=loadLocal(7);
    if(c>=0) {
        game.city=std::clamp(c,0,4); game.unlockedCity=std::clamp(u,0,4);
        game.player.level=std::max(1,l); game.player.xp=std::max(0,x);
        if(!classChosen) {
            game.player.playerClass=(PlayerClass)std::clamp(cl,0,2);
            if(cl==0) { game.player.health=125; game.player.stamina=95; }
            if(cl==1) { game.player.health=95; game.player.stamina=100; }
            if(cl==2) { game.player.health=115; game.player.stamina=100; }
        }
        game.player.escapeSkill=game.player.level>=3; game.player.combatSkill=game.player.level>=5;
    }
    game.player.weapon=ClassWeapon(game.player.playerClass);
    game.started=true; game.finished=finished>0; BuildCity(game);
    if(game.finished) game.quest=std::clamp(quest<0?0:quest,0,3);
    else SetQuestProgress(game,quest<0?0:quest);
    savedCity=game.city; hideIntro(); Save();
    if(game.finished) showEnding();
}
KEEP void SetTouch(float x,float y,int buttons) { touchX=std::clamp(x,-1.0f,1.0f); touchY=std::clamp(y,-1.0f,1.0f); touchButtons=buttons; }
KEEP void SetLook(float x,float y) { lookX+=x; lookY+=y; }
KEEP int GetQuestProgress() { return game.quest; }
KEEP int GetActiveBossCount() {
    return (int)std::count_if(game.enemies.begin(),game.enemies.end(),[](const Enemy& e){return e.alive&&e.boss;});
}
KEEP void ResetProgress() {
    game.city=0; game.unlockedCity=0; game.quest=0; game.bossUnlocked=false;
    game.player.level=1; game.player.xp=0;
    game.player.escapeSkill=0; game.player.combatSkill=0; game.finished=false; savedCity=0; Save();
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

static void Frame() {
    float dt=std::min(GetFrameTime(),0.04f);
    if(game.started && !game.finished) {
        Controls controls=ReadControls();
        game.player.blocking=controls.block;
        UpdateMovement(game,controls,camera,dt);
        UpdatePursuit(game,dt);
        UpdateCombat(game,controls,dt);
        if(game.city!=savedCity) { savedCity=game.city; Save(); }
        game.questTime+=dt;
        if(game.quest<3) {
            Vector3 p=game.player.position;
            const Vector3 targets[3]={{4,1.7f,6},{10,2.45f,13},{16,3.2f,20}};
            if(Vector3Distance(p,targets[game.quest])<3.0f) {
                game.quest++;
                UpdateProgression(game,65+game.city*15+game.quest*10);
                game.questTime=0;
                game.questLimit=100.0f-game.city*8.0f-game.quest*9.0f;
                if(game.quest==3) SetQuestProgress(game,game.quest);
                Save();
            }
            if(game.questTime>game.questLimit) {
                game.questTime=0;
                float hp=42.0f+game.city*14.0f;
                Vector3 spawn={game.player.position.x+8,game.player.position.y,game.player.position.z+8};
                game.enemies.push_back({spawn,hp,hp,2.7f+game.city*0.3f,0,false,true});
                game.player.stamina=std::max(10.0f,game.player.stamina-18);
            }
        }
        saveCooldown+=dt;
        if(saveCooldown>4.0f) { Save(); saveCooldown=0; }
        if(game.finished) { Save(); showEnding(); }
    }
    DrawGame(game,camera);
    if(!game.started) {
        DrawRectangle(0,0,GetScreenWidth(),GetScreenHeight(),Fade(BLACK,0.4f));
        DrawText("A FUGITIVE. FIVE CITIES. NO WAY BACK.",GetScreenWidth()/2-245,GetScreenHeight()/2-12,22,GOLD);
    }
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
