#include "game.hpp"
#include "raymath.h"
#include <algorithm>
#include <cmath>
#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#define KEEP EMSCRIPTEN_KEEPALIVE
EM_JS(void, saveLocal, (int city,int level,int xp,int unlocked,int klass,int weapon,int finished,int quest,
                               int mana,int vitality,int endurance,int focus,int power,int precision,int pending,
                               int skill0,int rank0,int skill1,int rank1,int activeSlot,int pendingSkills,int milestoneMask), {
  try {
    localStorage.setItem("tagbound-save",JSON.stringify({
      schemaVersion:3,city,level,xp,unlocked,klass,weapon,finished,quest,
      mana,vitality,endurance,focus,power,precision,pendingStatChoices:pending,
      skill0,rank0,skill1,rank1,activeSkillSlot:activeSlot,
      pendingSkillChoices:pendingSkills,skillMilestoneMask:milestoneMask
    }));
  } catch(e) {}
});
EM_JS(int, loadSaveVersion, (), {
  try { const s=JSON.parse(localStorage.getItem("tagbound-save")||"{}"); return Number.isInteger(s.schemaVersion)?s.schemaVersion:0; } catch(e) { return 0; }
});
EM_JS(int, loadLocal, (int field), {
  try {
    const s=JSON.parse(localStorage.getItem("tagbound-save")||"{}");
    const k=["city","level","xp","unlocked","klass","weapon","finished","quest",
             "mana","vitality","endurance","focus","power","precision","pendingStatChoices",
             "skill0","rank0","skill1","rank1","activeSkillSlot","pendingSkillChoices","skillMilestoneMask"];
    return Number.isFinite(s[k[field]])?s[k[field]]:-1;
  } catch(e) { return -1; }
});
EM_JS(void, saveExploration, (int city,const char* data), {
  try { localStorage.setItem("tagbound-exploration-v2-"+city,UTF8ToString(data)); } catch(e) {}
});
EM_JS(void, loadExploration, (int city,char* output,int capacity), {
  if(capacity<=0) return;
  try {
    const current=localStorage.getItem("tagbound-exploration-v2-"+city);
    const legacy=localStorage.getItem("tagbound-exploration-v1-"+city);
    const source=current&&current.length===1600?current:(legacy&&legacy.length===320?legacy:"");
    stringToUTF8(source,output,capacity);
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
EM_JS(void, setProgressionChoiceState, (int pending,int vitality,int endurance,int focus,int power,int precision), {
  const panel=document.getElementById("level-up");
  const open=pending>0;
  const settings=document.getElementById("settings");
  const intro=document.getElementById("intro");
  const ending=document.getElementById("ending");
  const gameReady=intro?.classList.contains("hidden")&&ending?.classList.contains("hidden");
  const settingsVisible=!settings?.classList.contains("hidden");
  panel?.classList.toggle("hidden",!open);
  document.getElementById("touch-controls")?.classList.toggle("hidden",open||settingsVisible);
  document.getElementById("settings-open")?.classList.toggle("hidden",open||!gameReady||settingsVisible);
  const count=document.getElementById("level-up-count");
  const countText="STAT CHOICES REMAINING: "+pending;
  if(count&&count.textContent!==countText) count.textContent=countText;
  const ranks=[vitality,endurance,focus,power,precision];
  document.querySelectorAll("[data-stat-choice]").forEach(button=>{
    const stat=Number(button.dataset.statChoice);
    const rank=ranks[stat]||0;
    const disabled=rank>=10;
    if(button.disabled!==disabled) button.disabled=disabled;
    const value=button.querySelector("[data-stat-rank]");
    const rankText="RANK "+rank+"/10";
    if(value&&value.textContent!==rankText) value.textContent=rankText;
  });
});

EM_JS(void, setSkillChoiceState, (int pending,int pendingStats,int playerClass,int option0,int option1,
                                  int skill0,int rank0,int skill1,int rank1), {
  const panel=document.getElementById("skill-choice");
  const open=pending>0&&pendingStats===0;
  const settings=document.getElementById("settings");
  const gameReady=document.getElementById("intro")?.classList.contains("hidden")&&
    document.getElementById("ending")?.classList.contains("hidden");
  const settingsVisible=!settings?.classList.contains("hidden");
  panel?.classList.toggle("hidden",!open);
  document.getElementById("touch-controls")?.classList.toggle("hidden",open||settingsVisible);
  document.getElementById("settings-open")?.classList.toggle("hidden",open||!gameReady||settingsVisible);
  const names=["","CLEAVING ARC","BREAKER STRIKE","ARROW RAIN","PIERCING SHOT","SHIELD BASH","AEGIS GUARD"];
  const descriptions=["","Frontal sword sweep that can strike several nearby foes.","Committed heavy strike that staggers and hits hard.",
    "Mark a visible area, then rain three waves of arrows into it.","Piercing arrow that can hit several enemies in a line.",
    "Close-range shield impact that knocks an enemy back.","Reduce incoming damage for a short defensive window."];
  const slots=[{id:skill0,rank:rank0},{id:skill1,rank:rank1}];
  const options=[];
  for(const id of [option0,option1]) {
    const owned=slots.findIndex(slot=>slot.id===id);
    if(owned>=0) {
      if(slots[owned].rank<3) options.push({id,upgrade:1,title:"UPGRADE "+names[id],
        detail:"Rank "+slots[owned].rank+" → "+(slots[owned].rank+1)+": "+descriptions[id]});
    } else if(slots.some(slot=>slot.id===0)) {
      options.push({id,upgrade:0,title:"UNLOCK "+names[id],detail:descriptions[id]});
    }
  }
  const count=document.getElementById("skill-choice-count");
  const countText="SKILL MILESTONE — CHOOSE ONE ("+pending+" REMAINING)";
  if(count&&count.textContent!==countText) count.textContent=countText;
  for(let index=0;index<2;index++) {
    const button=document.getElementById("skill-option-"+index);
    if(!button) continue;
    const option=options[index];
    const hidden=!option;
    if(button.classList.contains("hidden")!==hidden) button.classList.toggle("hidden",hidden);
    if(button.disabled===!!option) button.disabled=!option;
    if(option) {
      const skillId=String(option.id),upgrade=String(option.upgrade);
      if(button.dataset.skillId!==skillId) button.dataset.skillId=skillId;
      if(button.dataset.upgrade!==upgrade) button.dataset.upgrade=upgrade;
      const title=button.querySelector("[data-skill-title]");
      const detail=button.querySelector("[data-skill-detail]");
      if(title&&title.textContent!==option.title) title.textContent=option.title;
      if(detail&&detail.textContent!==option.detail) detail.textContent=option.detail;
    }
  }
});
EM_JS(void, setSkillLoadoutState, (int skill0,int rank0,int skill1,int rank1,int activeSlot,int playerClass,int mana,int cooldownTenths), {
  const names=["NO SKILL","CLEAVING ARC","BREAKER STRIKE","ARROW RAIN","PIERCING SHOT","SHIELD BASH","AEGIS GUARD"];
  for(let slot=0;slot<2;slot++) {
    const button=document.getElementById("skill-slot-"+slot);
    const label=document.getElementById("skill-slot-"+slot+"-name");
    const skill=slot===0?skill0:skill1,rank=slot===0?rank0:rank1;
    if(button) {
      const disabled=skill===0;
      if(button.disabled!==disabled) button.disabled=disabled;
      const selected=activeSlot===slot&&skill!==0;
      if(button.classList.contains("selected")!==selected) button.classList.toggle("selected",selected);
      const ariaLabel=skill===0?"Empty skill slot "+(slot+1):"Equip "+names[skill]+" in skill slot "+(slot+1);
      if(button.getAttribute("aria-label")!==ariaLabel) button.setAttribute("aria-label",ariaLabel);
    }
    const slotLabel=skill===0?"EMPTY":names[skill]+" · R"+rank;
    if(label&&label.textContent!==slotLabel) label.textContent=slotLabel;
  }
  const skill=activeSlot===0?skill0:skill1;
  const rank=activeSlot===0?rank0:rank1;
  const name=names[skill]||"NO SKILL";
  const costs=[0,24,30,38,28,22,30];
  const icons=[
    "",
    '<path d="M4 20 19 5M13 5h6v6M7 4l13 13M4 7l4-4"/>',
    '<path d="M12 2v13m-5-5 5 5 5-5M4 20h16"/>',
    '<path d="M18 3c-8 4-8 14 0 18M18 3v18M3 12h15m-4-4 4 4-4 4"/>',
    '<path d="M3 12h15m-5-5 5 5-5 5M18 3 6 21"/>',
    '<path d="M12 2.5 20 5.5v5.2c0 4.7-3.1 8.1-8 10.8-4.9-2.7-8-6.1-8-10.8V5.5z"/><path d="M4 12h16"/>',
    '<path d="M12 2.5 20 5.5v5.2c0 4.7-3.1 8.1-8 10.8-4.9-2.7-8-6.1-8-10.8V5.5z"/><path d="m8 12 2.5 2.5L16 9"/>'
  ];
  const technique=document.querySelector(".action.technique");
  const cost=costs[skill]||0;
  const cooldown=cooldownTenths/10;
  if(technique) {
    const ready=skill!==0&&mana>=cost&&cooldown<=0;
    const disabled=skill===0||!ready;
    if(technique.disabled!==disabled) technique.disabled=disabled;
    const costLabel=cost?cost+"M":"";
    const cooldownLabel=cooldown>0?cooldown.toFixed(1)+"s":"";
    if(technique.dataset.cost!==costLabel) technique.dataset.cost=costLabel;
    if(technique.dataset.cooldown!==cooldownLabel) technique.dataset.cooldown=cooldownLabel;
    const noMana=skill!==0&&mana<cost&&cooldown<=0;
    if(technique.classList.contains("no-mana")!==noMana) technique.classList.toggle("no-mana",noMana);
    const cooling=cooldown>0;
    if(technique.classList.contains("cooling")!==cooling) technique.classList.toggle("cooling",cooling);
    const ariaLabel=skill===0?"No equipped technique":
      "Use "+name+" rank "+rank+", costs "+cost+" Mana, "+(cooldown>0?"cooldown "+cooldown.toFixed(1)+" seconds":mana<cost?"not enough Mana":"ready");
    if(technique.getAttribute("aria-label")!==ariaLabel) technique.setAttribute("aria-label",ariaLabel);
    const svg=technique.querySelector("svg");
    if(svg&&skill>0&&svg.innerHTML!==icons[skill]) svg.innerHTML=icons[skill];
  }
});
EM_JS(void, setSkillTargetState, (int active,int valid), {
  const panel=document.getElementById("skill-target");
  panel?.classList.toggle("hidden",!active);
  const controls=document.getElementById("touch-controls");
  controls?.classList.toggle("skill-targeting",!!active);
  const settings=document.getElementById("settings-open");
  if(settings) settings.classList.toggle("hidden",!!active||
    !document.getElementById("intro")?.classList.contains("hidden")||
    !document.getElementById("ending")?.classList.contains("hidden"));
  const confirm=document.getElementById("skill-target-confirm");
  if(confirm&&confirm.disabled===!!valid) confirm.disabled=!valid;
  const label=document.getElementById("skill-target-status");
  const status=valid?"TARGET VALID":"MOVE THE MARKER TO A CLEAR AREA";
  if(label&&label.textContent!==status) label.textContent=status;
});

EM_JS(void, hideIntro, (), { document.getElementById("intro")?.classList.add("hidden"); });
EM_JS(void, showEnding, (), { window.TagboundAudio?.pauseMusic?.(); document.getElementById("ending")?.classList.remove("hidden"); });
EM_JS(void, initGameAudio, (), { try { window.TagboundAudio?.init?.(); } catch (e) {} });
EM_JS(void, playGameAudio, (int kind), { try { window.TagboundAudio?.play?.(kind); } catch (e) {} });
EM_JS(int, isSettingsOpen, (), { return document.getElementById("settings")?.classList.contains("hidden") ? 0 : 1; });
#else
#define KEEP
static void saveLocal(int,int,int,int,int,int,int,int,int,int,int,int,int,int,int,int,int,int,int,int,int,int) {}
static int loadSaveVersion() { return 0; }
static int loadLocal(int) { return -1; }
static void saveExploration(int,const char*) {}
static void loadExploration(int,char*,int) {}
static void setMapControlsState(int,int,int,int) {}
static void setProgressionChoiceState(int,int,int,int,int,int) {}
static void setSkillChoiceState(int,int,int,int,int,int,int,int,int) {}
static void setSkillLoadoutState(int,int,int,int,int,int,int,int) {}
static void setSkillTargetState(int,int) {}
static void hideIntro() {}
static void showEnding() {}
static void initGameAudio() {}
static void playGameAudio(int) {}
static int isSettingsOpen() { return 0; }
#endif

static void SyncMapControls();
static void SyncProgressionUI();
static void SyncSkillUI();
static void SyncSkillTargetUI();
static GameState game;
static Camera3D camera{};
static int selectedClass=0, touchButtons=0;
static float touchX=0,touchY=0,lookX=0,lookY=0,attackCooldown=0,saveCooldown=0;
static bool skillWasDown=false,jumpWasDown=false,classChosen=false;
static int savedCity=-1;

static void Save() {
    const Player& p=game.player;
    saveLocal(game.city,p.level,p.xp,game.unlockedCity,(int)p.playerClass,(int)p.weapon,
        game.finished?1:0,game.quest,(int)std::lround(p.mana),
        p.vitality,p.endurance,p.focus,p.power,p.precision,p.pendingStatChoices,
        p.skills[0],p.skillRanks[0],p.skills[1],p.skillRanks[1],p.activeSkillSlot,
        p.pendingSkillChoices,p.skillMilestoneMask);
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
    game.player.weapon=ClassWeapon(game.player.playerClass);
    int saveVersion=loadSaveVersion();
    int c=loadLocal(0), l=loadLocal(1), x=loadLocal(2), u=loadLocal(3), cl=loadLocal(4);
    int finished=loadLocal(6), quest=loadLocal(7), mana=loadLocal(8);
    int vitality=loadLocal(9), endurance=loadLocal(10), focus=loadLocal(11);
    int power=loadLocal(12), precision=loadLocal(13), pending=loadLocal(14);
    int skill0=loadLocal(15),rank0=loadLocal(16),skill1=loadLocal(17),rank1=loadLocal(18);
    int activeSlot=loadLocal(19),pendingSkills=loadLocal(20),milestoneMask=loadLocal(21);
    if(c>=0) {
        if(saveVersion<1) {
            c=MigrateLegacyCityIndex(c);
            u=MigrateLegacyCityIndex(u);
        }
        game.city=std::clamp(c,0,4);
        game.unlockedCity=std::clamp(u,0,4);
        game.player.level=std::max(1,l);
        game.player.xp=std::max(0,x);
        if(!classChosen) game.player.playerClass=(PlayerClass)std::clamp(cl,0,2);
        game.player.vitality=std::clamp(vitality,0,MaxStatRank);
        game.player.endurance=std::clamp(endurance,0,MaxStatRank);
        game.player.focus=std::clamp(focus,0,MaxStatRank);
        game.player.power=std::clamp(power,0,MaxStatRank);
        game.player.precision=std::clamp(precision,0,MaxStatRank);
        const int earnedStats=game.player.vitality+game.player.endurance+
            game.player.focus+game.player.power+game.player.precision;
        game.player.pendingStatChoices=std::clamp(pending<0?0:pending,0,99);
        if(saveVersion<2) {
            game.player.pendingStatChoices=std::max(game.player.pendingStatChoices,
                std::max(0,game.player.level-1-earnedStats));
        }
        game.player.skills[0]=skill0;
        game.player.skillRanks[0]=std::clamp(rank0,0,3);
        game.player.skills[1]=skill1;
        game.player.skillRanks[1]=std::clamp(rank1,0,3);
        game.player.activeSkillSlot=std::clamp(activeSlot<0?0:activeSlot,0,1);
        game.player.pendingSkillChoices=std::clamp(pendingSkills<0?0:pendingSkills,0,99);
        game.player.skillMilestoneMask=std::clamp(milestoneMask<0?0:milestoneMask,0,3);
        if(saveVersion<3) {
            for(int milestoneLevel:{3,5}) if(game.player.level>=milestoneLevel) {
                const int bit=milestoneLevel==3?1:2;
                if(!(game.player.skillMilestoneMask&bit)) {
                    game.player.skillMilestoneMask|=bit;
                    game.player.pendingSkillChoices++;
                }
            }
        }
        for(int slot=0;slot<2;slot++) {
            const int id=game.player.skills[slot];
            if(id<0||id>(int)ClassSkill::AegisGuard||
               (id!=(int)ClassSkill::None&&!((ClassSkill)id==ClassSkillOption(game.player.playerClass,0)||
                 (ClassSkill)id==ClassSkillOption(game.player.playerClass,1)))) {
                game.player.skills[slot]=0;
                game.player.skillRanks[slot]=0;
            }
        }
        if(game.player.skills[0]==game.player.skills[1]) {
            game.player.skills[1]=0;
            game.player.skillRanks[1]=0;
        }
        if(game.player.skills[game.player.activeSkillSlot]==0&&game.player.skills[1-game.player.activeSkillSlot]!=0)
            game.player.activeSkillSlot=1-game.player.activeSkillSlot;
        game.player.escapeSkill=game.player.level>=3;
        game.player.combatSkill=game.player.level>=5;
    }
    game.player.weapon=ClassWeapon(game.player.playerClass);
    game.player.health=PlayerMaxHealth(game.player);
    game.player.stamina=PlayerMaxStamina(game.player);
    game.player.mana=mana>=0?std::clamp((float)mana,0.0f,PlayerMaxMana(game.player)):PlayerMaxMana(game.player);
    game.started=true; game.finished=finished>0; game.mapOpen=false; BuildCity(game);
    if(game.finished) game.quest=std::clamp(quest<0?0:quest,0,3);
    else {
        SetQuestProgress(game,quest<0?0:quest);
        game.player.position=QuestCheckpoint(game.city,game.quest);
        game.player.facing=QuestCheckpointFacing(game.city,game.quest);
    }
    LoadExploration();
    UpdateExploration(game,game.player.position);
    SaveExploration();
    SyncMapControls();
    SyncProgressionUI();
    SyncSkillUI();
    SyncSkillTargetUI();
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
KEEP int IsGameUpdating() { return game.started && !game.finished && !isSettingsOpen() && game.player.pendingStatChoices==0 && game.player.pendingSkillChoices==0 ? 1 : 0; }
KEEP void ToggleMap() {
    if(!game.started||game.finished||isSettingsOpen()||game.player.pendingStatChoices>0||game.player.pendingSkillChoices>0) return;
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
KEEP int GetPendingStatChoices() { return game.player.pendingStatChoices; }
KEEP void ChooseStat(int stat) {
    if(!game.started||isSettingsOpen()||stat<0||stat>4) return;
    if(ChooseStatUpgrade(game,(StatUpgrade)stat)) {
        Save();
        SyncProgressionUI();
    }
}
KEEP int GetPendingSkillChoices() { return game.player.pendingSkillChoices; }
KEEP int IsSkillTargeting() { return game.skillTargeting?1:0; }
KEEP int GetSkillOption(int option) { return (int)ClassSkillOption(game.player.playerClass,option); }
KEEP int GetSkillSlot(int slot) { return slot>=0&&slot<2?game.player.skills[slot]:0; }
KEEP int GetSkillSlotRank(int slot) { return slot>=0&&slot<2?game.player.skillRanks[slot]:0; }
KEEP int GetActiveSkillSlot() { return game.player.activeSkillSlot; }
KEEP int GetSkillEffectCount() { return (int)game.skillEffects.size(); }
KEEP int GetPlayerMana() { return (int)std::lround(game.player.mana); }
KEEP int GetTechniqueCooldownTenths() { return (int)std::lround(game.player.techniqueCooldown*10.0f); }
KEEP void ChooseSkill(int skillId,int upgrade) {
    if(!game.started||isSettingsOpen()) return;
    if(ChooseSkillOption(game,skillId,upgrade!=0)) {
        Save(); SyncSkillUI();
    }
}
KEEP void EquipSkillSlot(int slot) {
    if(EquipSkill(game,slot)) { Save(); SyncSkillUI(); }
}
KEEP void UseClassSkill() {
    if(!game.started||game.mapOpen||game.skillTargeting||isSettingsOpen()||
       game.player.pendingStatChoices>0||game.player.pendingSkillChoices>0) return;
    if(ActivateClassSkill(game)) {
        if(!game.skillTargeting) Save();
        SyncSkillUI();
        SyncSkillTargetUI();
    }
}
KEEP int SetClassSkillTargetScreen(float x,float y) {
    if(!game.skillTargeting) return 0;
    Ray ray=GetScreenToWorldRay({x,y},camera);
    if(std::fabs(ray.direction.y)<0.0001f) return 0;
    const float distance=(game.player.position.y-ray.position.y)/ray.direction.y;
    if(distance<0.0f) return 0;
    Vector3 target=Vector3Add(ray.position,Vector3Scale(ray.direction,distance));
    const int valid=SetClassSkillTarget(game,target)?1:0;
    SyncSkillTargetUI();
    return valid;
}
KEEP void ConfirmSkillTarget() {
    if(ConfirmClassSkillTarget(game)) { Save(); SyncSkillUI(); SyncSkillTargetUI(); }
}
KEEP void CancelSkillTarget() {
    CancelClassSkillTarget(game); SyncSkillTargetUI();
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
    game.player.vitality=game.player.endurance=game.player.focus=0;
    game.player.power=game.player.precision=game.player.pendingStatChoices=0;
    game.player.skills={0,0}; game.player.skillRanks={0,0}; game.player.activeSkillSlot=0;
    game.player.pendingSkillChoices=0; game.player.skillMilestoneMask=0;
    game.player.techniqueCooldown=game.player.aegisTime=0;
    game.skillEffects.clear(); game.skillTargeting=false;
    game.player.health=PlayerMaxHealth(game.player);
    game.player.stamina=PlayerMaxStamina(game.player);
    game.player.mana=PlayerMaxMana(game.player);
    game.player.escapeSkill=0; game.player.combatSkill=0;
    game.finished=false; game.mapOpen=false;
    SyncMapControls(); SyncProgressionUI(); savedCity=0; Save();
}
}

static void SyncMapControls() {
    setMapControlsState(game.mapOpen?1:0,game.mapFloor,
        CanChangeExplorationFloor(game,-1)?1:0,
        CanChangeExplorationFloor(game,1)?1:0);
}

static void SyncProgressionUI() {
    const Player& p=game.player;
    setProgressionChoiceState(p.pendingStatChoices,p.vitality,p.endurance,p.focus,p.power,p.precision);
}
static void SyncSkillUI() {
    const Player& p=game.player;
    setSkillChoiceState(p.pendingSkillChoices,p.pendingStatChoices,(int)p.playerClass,
        (int)ClassSkillOption(p.playerClass,0),(int)ClassSkillOption(p.playerClass,1),
        p.skills[0],p.skillRanks[0],p.skills[1],p.skillRanks[1]);
    setSkillLoadoutState(p.skills[0],p.skillRanks[0],p.skills[1],p.skillRanks[1],p.activeSkillSlot,
        (int)p.playerClass,(int)std::lround(p.mana),(int)std::lround(p.techniqueCooldown*10.0f));
}
static void SyncSkillTargetUI() {
    setSkillTargetState(game.skillTargeting?1:0,IsClassSkillTargetValid(game)?1:0);
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
        if(!game.mapOpen && game.player.pendingStatChoices==0 && game.player.pendingSkillChoices==0) {
        game.player.blocking=controls.block;
        const int previousLevel=game.player.level;
        float previousY=game.player.position.y;
        bool wasDashing=game.player.dashTime>0;
        float previousHealth=game.player.health;
        int previousDefeated=game.defeated;
        unsigned int previousWindups=BossWindupMask();
        int previousTaggerWindups=TaggerWindupCount();
        if(controls.attack) playGameAudio(game.player.weapon==Weapon::Bow?1:0);
        if(game.skillTargeting) {
            controls.move={0,0}; controls.jump=false; controls.slide=false; controls.sprint=false;
            controls.attack=false; controls.block=false; controls.aim=false; controls.skill=false;
            controls.lookX=controls.lookY=0;
        }
        UpdateMovement(game,controls,camera,dt);
        RegenerateMana(game.player,dt);
        UpdateClassSkills(game,dt);
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
        if(game.player.level!=previousLevel) Save();
        saveCooldown+=dt;
        if(saveCooldown>4.0f) { Save(); saveCooldown=0; }
        if(game.finished) { Save(); showEnding(); }
        }
    }
    SyncProgressionUI();
    SyncSkillUI();
    SyncSkillTargetUI();
    BeginDrawing();
    DrawGame(game,camera);
    if(!game.started) {
        DrawRectangle(0,0,GetScreenWidth(),GetScreenHeight(),Fade(BLACK,0.4f));
        DrawText("A FUGITIVE. FIVE CITIES. NO WAY BACK.",GetScreenWidth()/2-245,GetScreenHeight()/2-12,22,GOLD);
    }
    EndDrawing();
}

KEEP int GetFrameRate() { return GetFPS(); }
KEEP float GetFrameTimeMs() { return GetFrameTime()*1000.0f; }
KEEP int GetSaveSchemaVersion() { return 3; }

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
