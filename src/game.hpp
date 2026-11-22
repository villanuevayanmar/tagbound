#pragma once
#include "raylib.h"
#include <vector>
#include <string>

enum class Weapon { Sword, Axe, Shield, Bow, Spear };
enum class PlayerClass { Vanguard, Skirmisher, Warden };
enum class BossPhase { Dormant, Hunt, Windup, Strike, Recover, Defeated };

struct Platform { Vector3 position; Vector3 size; Color color; };
struct Projectile { Vector3 position; Vector3 velocity; float life; };
struct Enemy {
    Vector3 position{};
    float hp=40, maxHp=40, speed=2.5f, cooldown=0;
    bool boss=false, alive=true;
    BossPhase phase=BossPhase::Dormant;
    float phaseTime=0, hitTime=0, stunTime=0;
    int variant=0;
    Vector3 knockback{};
};
struct Player {
    Vector3 position{0,1,0}, velocity{0,0,0}, facing{0,0,1};
    float health=100, stamina=100, invulnerable=0;
    float dashTime=0, dashCooldown=0, vaultTime=0, climbTime=0, landingTime=0, attackTime=0;
    bool landingRoll=false;
    bool wallRunning=false, vaulting=false, ledgeGrabbing=false, climbing=false;
    Vector3 ledgeNormal{}, ledgeHangPosition{}, ledgeTargetPosition{};
    int level=1, xp=0;
    PlayerClass playerClass=PlayerClass::Vanguard;
    Weapon weapon=Weapon::Sword;
    bool grounded=true, blocking=false, bowAiming=false;
    int escapeSkill=0, combatSkill=0;
};
struct Controls {
    Vector2 move{0,0};
    float lookX=0, lookY=0;
    bool jump=false, slide=false, sprint=false, attack=false, block=false, aim=false, skill=false;
};
struct GameState {
    Player player;
    std::vector<Platform> platforms;
    std::vector<Enemy> enemies;
    std::vector<Projectile> projectiles;
    int city=0, unlockedCity=0, quest=0, totalQuests=3, defeated=0;
    float danger=0, questTime=0, questLimit=110;
    bool started=false, finished=false, bossUnlocked=false;
};

void UpdateMovement(GameState& game, const Controls& controls, Camera3D& camera, float dt);
void UpdatePursuit(GameState& game, float dt);
void UpdateCombat(GameState& game, const Controls& controls, float dt);
void UpdateProgression(GameState& game, int amount);
void BuildCity(GameState& game);
void SetQuestProgress(GameState& game,int quest);
void DrawGame(const GameState& game, const Camera3D& camera);
const char* CityName(int city);
const char* BossName(int city);
const char* WeaponName(Weapon weapon);
Weapon ClassWeapon(PlayerClass playerClass);
float ClassMaxHealth(PlayerClass playerClass);
bool FindReinforcementSpawn(const GameState& game,Vector3 origin,Vector3& spawn);
inline float ClassMaxStamina(PlayerClass playerClass) {
    return playerClass==PlayerClass::Vanguard?95.0f:100.0f;
}
