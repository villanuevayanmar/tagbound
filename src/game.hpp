#pragma once
#include "raylib.h"
#include <vector>
#include <array>
#include <string>

enum class Weapon { Sword, Axe, Shield, Bow, Spear };
enum class PlayerClass { Vanguard, Skirmisher, Warden };
enum class BossPhase { Dormant, Hunt, Windup, Strike, Recover, Defeated };
enum class StatUpgrade { Vitality, Endurance, Focus, Power, Precision };
enum class ClassSkill : int {
    None, CleavingArc, BreakerStrike, ArrowRain, PiercingShot, ShieldBash, AegisGuard
};
constexpr int MaxStatRank=10;

struct Platform { Vector3 position; Vector3 size; Color color; };
struct Projectile { Vector3 position; Vector3 velocity; float life; };
struct SkillEffect {
    ClassSkill skill=ClassSkill::None;
    Vector3 position{};
    float radius=0.0f, damage=0.0f, delay=0.0f, waveTimer=0.0f;
    int wavesRemaining=0;
};
struct BossArena {
    Vector3 center{};
    Vector2 halfExtents{};
    float floorY=0;
    bool active=false;
};
struct Enemy {
    Vector3 position{};
    float hp=40, maxHp=40, speed=2.5f, cooldown=0;
    bool boss=false, alive=true;
    BossPhase phase=BossPhase::Dormant;
    float phaseTime=0, hitTime=0, stunTime=0;
    int variant=0;
    Vector3 knockback{};
    Vector3 strikeDirection{};
    Vector3 strikeOrigin{};
    float attackWindup=0;
    Vector3 pursuitWaypoint{};
    bool hasPursuitWaypoint=false;
};
struct Player {
    Vector3 position{0,1,0}, velocity{0,0,0}, facing{0,0,1};
    Vector2 moveVelocity{0,0};
    float health=100, stamina=100, mana=70, invulnerable=0;
    float dashTime=0, dashCooldown=0, vaultTime=0, climbTime=0, landingTime=0, attackTime=0;
    bool landingRoll=false;
    bool wallRunning=false, vaulting=false, ledgeGrabbing=false, climbing=false;
    Vector3 ledgeNormal{}, ledgeHangPosition{}, ledgeTargetPosition{};
    int level=1, xp=0, vitality=0, endurance=0, focus=0, power=0, precision=0;
    int pendingStatChoices=0, pendingSkillChoices=0, skillMilestoneMask=0;
    std::array<int,2> skills{{0,0}}, skillRanks{{0,0}};
    int activeSkillSlot=0;
    float techniqueCooldown=0, aegisTime=0;
    PlayerClass playerClass=PlayerClass::Vanguard;
    Weapon weapon=Weapon::Sword;
    bool grounded=true, blocking=false, bowAiming=false;
    int escapeSkill=0, combatSkill=0;
};
constexpr int ExplorationMapColumns=20;
constexpr int ExplorationMapRows=16;
constexpr int ExplorationMapCells=ExplorationMapColumns*ExplorationMapRows;
constexpr int ExplorationMapFloors=5;
constexpr int ExplorationMapTotalCells=ExplorationMapCells*ExplorationMapFloors;

struct Controls {
    Vector2 move{0,0};
    float lookX=0, lookY=0;
    bool jump=false, slide=false, sprint=false, attack=false, block=false, aim=false, skill=false, technique=false;
};
struct GameState {
    Player player;
    std::vector<Platform> platforms;
    std::vector<Enemy> enemies;
    std::vector<Projectile> projectiles;
    std::vector<struct SkillEffect> skillEffects;
    BossArena bossArena;
    bool skillTargeting=false;
    Vector3 skillTarget{};
    std::array<unsigned char,ExplorationMapTotalCells> explored{};
    int currentFloor=0, mapFloor=0;
    bool mapOpen=false;
    int city=0, unlockedCity=0, quest=0, totalQuests=3, defeated=0;
    float danger=0, questTime=0, questLimit=110, beaconHoldTime=0;
    bool started=false, finished=false, bossUnlocked=false;
};

void ResetCamera(const Player& player, Camera3D& camera);
void UpdateMovement(GameState& game, const Controls& controls, Camera3D& camera, float dt);
void UpdatePursuit(GameState& game, float dt);
void UpdateCombat(GameState& game, const Controls& controls, float dt);
void UpdateProgression(GameState& game, int amount);
void BuildCity(GameState& game);
void SetQuestProgress(GameState& game,int quest);
void DrawGame(const GameState& game, const Camera3D& camera);
const char* CityName(int city);
int MigrateLegacyCityIndex(int city);
const char* BossName(int city);
Vector3 QuestTarget(int city,int quest);
Vector3 QuestCheckpoint(int city,int quest);
Vector3 QuestCheckpointFacing(int city,int quest);
bool ReachedQuestBeacon(Vector3 position,int city,int quest);
bool QuestRequiresHold(int city,int quest);
float QuestHoldDuration(int city,int quest);
bool QuestRequiresSurvival(int city,int quest);
float QuestSurvivalDuration(int city,int quest);
bool UpdateQuestObjective(GameState& game,float dt);
int ExplorationCellForPosition(Vector3 position);
int ExplorationFloorForPosition(float height);
const char* ExplorationFloorName(int floor);
bool UpdateExploration(GameState& game,Vector3 position);
bool CanChangeExplorationFloor(const GameState& game,int direction);
bool ChangeExplorationFloor(GameState& game,int direction);
float BossWindupDuration(int variant,bool enraged=false);
float BossStrikeRange(int variant);
bool BossArenaContains(const GameState& game,const Enemy& boss);
float BossStrikeHalfWidth(int variant,float forward);
Vector3 EnemyFacing(const Enemy& enemy,Vector3 target);
const char* WeaponName(Weapon weapon);
Weapon ClassWeapon(PlayerClass playerClass);
float ClassMaxHealth(PlayerClass playerClass);
float ClassMaxMana(PlayerClass playerClass);
float PlayerMaxHealth(const Player& player);
float PlayerMaxStamina(const Player& player);
float PlayerMaxMana(const Player& player);
float PlayerMeleeDamageMultiplier(const Player& player);
float PlayerRangedDamageMultiplier(const Player& player);
bool ChooseStatUpgrade(GameState& game,StatUpgrade stat);
ClassSkill ClassSkillOption(PlayerClass playerClass,int option);
const char* ClassSkillName(ClassSkill skill);
bool ChooseSkillOption(GameState& game,int skillId,bool upgrade);
bool EquipSkill(GameState& game,int slot);
bool ActivateClassSkill(GameState& game);
bool SetClassSkillTarget(GameState& game,Vector3 target);
bool ConfirmClassSkillTarget(GameState& game);
void CancelClassSkillTarget(GameState& game);
void UpdateClassSkills(GameState& game,float dt);
void RegenerateMana(Player& player,float dt);
bool FindReinforcementSpawn(const GameState& game,Vector3 origin,Vector3& spawn);
inline float ClassMaxStamina(PlayerClass playerClass) {
    return playerClass==PlayerClass::Vanguard?95.0f:100.0f;
}
