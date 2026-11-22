#include "game.hpp"
#include <algorithm>
#include <cassert>
#include <cmath>
#include <iostream>

static bool near(float a, float b, float epsilon=0.02f) {
    return std::fabs(a-b) <= epsilon;
}

int main() {
    GameState game;
    BuildCity(game);
    assert(game.city == 0);
    assert(game.platforms.size() == 34);
    assert(game.enemies.size() == 2);
    assert(std::string(CityName(0)) == "PORT TOWN");
    assert(std::string(CityName(4)) == "IMPERIAL CAPITAL");
    assert(std::string(BossName(0)) == "HARBOR WARDEN");
    assert(std::string(BossName(4)) == "IMPERIAL REGENT");
    assert(std::string(WeaponName(Weapon::Spear)) == "SPEAR");

    // Each class has exactly one assigned weapon.
    assert(ClassWeapon(PlayerClass::Vanguard) == Weapon::Sword);
    assert(ClassWeapon(PlayerClass::Skirmisher) == Weapon::Bow);
    assert(ClassWeapon(PlayerClass::Warden) == Weapon::Shield);

    // Each campaign city builds its expected pursuer count and full scene.
    for (int city = 0; city < 5; ++city) {
        game.city = city;
        BuildCity(game);
        assert(game.platforms.size() == 34);
        assert(game.enemies.size() == static_cast<size_t>(2 + city * 2));
        assert(game.quest == 0);
        assert(!game.bossUnlocked);
    }
    game.city = 0;
    BuildCity(game);

    // A saved beacon index restores the current objective and exact city boss once.
    game.city=2;
    BuildCity(game);
    SetQuestProgress(game,2);
    assert(game.quest==2);
    assert(near(game.questLimit,66.0f));
    assert(!game.bossUnlocked);
    assert(std::count_if(game.enemies.begin(),game.enemies.end(),[](const Enemy& e){return e.boss;})==0);
    SetQuestProgress(game,3);
    assert(game.quest==3);
    assert(game.bossUnlocked);
    assert(std::count_if(game.enemies.begin(),game.enemies.end(),[](const Enemy& e){return e.boss;})==1);
    const Enemy& restoredBoss=game.enemies.back();
    assert(restoredBoss.boss && restoredBoss.variant==2);
    assert(near(restoredBoss.hp,520.0f));
    SetQuestProgress(game,3);
    assert(std::count_if(game.enemies.begin(),game.enemies.end(),[](const Enemy& e){return e.boss;})==1);
    game.city=0;
    BuildCity(game);

    UpdateProgression(game, 100);
    assert(game.player.level == 2);
    UpdateProgression(game, 200);
    assert(game.player.level == 3);
    assert(game.player.escapeSkill == 1);
    UpdateProgression(game, 700);
    assert(game.player.level >= 5);
    assert(game.player.combatSkill == 1);

    BuildCity(game);
    Camera3D camera{};
    Controls move{};
    move.move.y = 1.0f;
    UpdateMovement(game, move, camera, 0.1f);
    assert(game.player.position.z > 0.4f);

    Controls jump{};
    jump.jump = true;
    UpdateMovement(game, jump, camera, 0.01f);
    assert(!game.player.grounded);
    assert(game.player.position.y > 0.3f);

    // A grounded jump into a reachable rooftop ledge performs a forward vault.
    BuildCity(game);
    game.player.position = {1.15f,0.3f,6.0f};
    game.player.facing = {1,0,0};
    game.player.stamina = 100;
    Controls vault{};
    vault.move.x = 1.0f;
    vault.jump = true;
    UpdateMovement(game,vault,camera,0.01f);
    assert(game.player.vaulting);
    assert(game.player.vaultTime > 0.4f);
    assert(game.player.velocity.y > 12.0f);
    assert(game.player.stamina < 100.0f);
    vault.jump = false;
    UpdateMovement(game,vault,camera,0.2f);
    assert(game.player.position.y > 1.7f);
    assert(game.player.vaultTime > 0.0f);
    UpdateMovement(game,vault,camera,0.2f);
    assert(game.player.position.x > 1.5f);

    // Airborne contact with a reachable roof edge enters a stable ledge hang.
    BuildCity(game);
    game.player.position = {6.51f,0.3f,6.0f};
    game.player.facing = {-1,0,0};
    game.player.grounded = false;
    Controls grab{};
    grab.move.x = -1.0f;
    UpdateMovement(game,grab,camera,0.01f);
    assert(game.player.ledgeGrabbing);
    assert(near(game.player.velocity.x,0.0f));
    assert(near(game.player.velocity.y,0.0f));
    Vector3 hangPosition = game.player.position;
    UpdateMovement(game,Controls{},camera,0.1f);
    assert(game.player.ledgeGrabbing);
    assert(near(game.player.position.x,hangPosition.x));
    assert(near(game.player.position.y,hangPosition.y));

    // Jump transitions from hanging to a smooth climb onto the roof.
    Controls climb{};
    climb.jump = true;
    UpdateMovement(game,climb,camera,0.21f);
    assert(game.player.climbing);
    assert(game.player.position.y > hangPosition.y);
    climb.jump = false;
    UpdateMovement(game,climb,camera,0.22f);
    assert(!game.player.climbing);
    assert(!game.player.ledgeGrabbing);
    assert(game.player.grounded);
    assert(near(game.player.position.y,1.7f));
    assert(game.player.position.x < hangPosition.x);

    // A downward/slide input safely releases a hanging player.
    game.player.position = {6.51f,0.3f,6.0f};
    game.player.facing = {-1,0,0};
    game.player.grounded = false;
    grab.move.x = -1.0f;
    UpdateMovement(game,grab,camera,0.01f);
    assert(game.player.ledgeGrabbing);
    Controls drop{};
    drop.slide = true;
    UpdateMovement(game,drop,camera,0.016f);
    assert(!game.player.ledgeGrabbing);
    assert(!game.player.grounded);

    assert(game.player.position.x > 6.8f);
    UpdateMovement(game,Controls{},camera,0.016f);
    assert(!game.player.ledgeGrabbing);

    // Pushing the mobile joystick down also releases the ledge.
    game.player.position = {6.51f,0.3f,6.0f};
    game.player.facing = {-1,0,0};
    game.player.grounded = false;
    grab.move.x = -1.0f;
    UpdateMovement(game,grab,camera,0.01f);
    assert(game.player.ledgeGrabbing);
    Controls joystickDrop{};
    joystickDrop.move.y = -1.0f;
    UpdateMovement(game,joystickDrop,camera,0.016f);
    assert(!game.player.ledgeGrabbing);
    assert(game.player.position.x > 6.8f);

    // A hard fall triggers a short landing roll when slide is held.
    game.platforms.clear();
    game.platforms.push_back({{0,-0.3f,0},{120,0.6f,120},{44,49,59,255}});
    game.player.position = {0,1.0f,0};
    game.player.velocity = {0,-10.0f,0};
    game.player.facing = {0,0,1};
    game.player.grounded = false;
    Controls roll{};
    roll.slide = true;
    UpdateMovement(game,roll,camera,0.1f);
    assert(game.player.grounded);
    assert(game.player.landingTime > 0.25f);
    assert(game.player.landingRoll);
    assert(game.player.velocity.z > 2.0f);

    // A normal landing gets recovery pose without the forward roll impulse.
    game.player.position = {0,0.8f,0};
    game.player.velocity = {0,-3.0f,0};
    game.player.grounded = false;
    Controls land{};
    UpdateMovement(game,land,camera,0.1f);
    assert(!game.player.grounded);
    UpdateMovement(game,land,camera,0.1f);
    assert(game.player.grounded);
    assert(game.player.landingTime > 0.25f);
    assert(!game.player.landingRoll);

    game.player.escapeSkill = 1;
    Controls dash{};
    dash.skill = true;
    UpdateMovement(game, dash, camera, 0.01f);
    assert(game.player.dashCooldown > 1.7f);
    assert(game.player.stamina < 100.0f);

    // Every melee weapon must apply its intended damage.
    const Weapon weapons[] = {Weapon::Sword, Weapon::Axe, Weapon::Shield, Weapon::Spear};
    const float expectedDamage[] = {26, 43, 14, 32};
    for (int i = 0; i < 4; ++i) {
        game.enemies.clear();
        game.player.position = {0,0.3f,0};
        game.player.facing = {0,0,1};
        game.player.weapon = weapons[i];
        game.player.combatSkill = 0;
        game.enemies.push_back({{0,0.3f,1.5f},100,100,0,0,false,true});
        Controls hit{};
        hit.attack = true;
        UpdateCombat(game, hit, 0.016f);
        assert(near(game.enemies.front().hp, 100.0f-expectedDamage[i]));
        assert(game.player.attackTime>0.2f);
        assert(game.enemies.front().hitTime>0.1f);
        assert(game.enemies.front().stunTime>0.1f);
        const Vector3 knockback=game.enemies.front().knockback;
        assert(std::sqrt(knockback.x*knockback.x+knockback.y*knockback.y+knockback.z*knockback.z)>0.5f);
    }

    // A melee swing hits only the nearest target, even when enemies overlap the attack arc.
    game.enemies.clear();
    game.player.position = {0,0.3f,0};
    game.player.facing = {0,0,1};
    game.player.weapon = Weapon::Sword;
    game.enemies.push_back({{0,0.3f,1.8f},100,100,0,0,false,true});
    game.enemies.push_back({{0,0.3f,1.0f},100,100,0,0,false,true});
    Controls singleHit{};
    singleHit.attack = true;
    UpdateCombat(game,singleHit,0.016f);
    assert(near(game.enemies[0].hp,100.0f));
    assert(near(game.enemies[1].hp,74.0f));

    // Solid rooftop cover blocks melee attacks against enemies behind it.
    game.enemies.clear();
    game.platforms.clear();
    game.platforms.push_back({{0,1.5f,1.0f},{4,3.0f,0.4f},GRAY});
    game.player.position = {0,0.3f,0};
    game.player.facing = {0,0,1};
    game.player.weapon = Weapon::Sword;
    game.enemies.push_back({{0,0.3f,2.0f},100,100,0,0,false,true});
    Controls blockedMelee{};
    blockedMelee.attack = true;
    UpdateCombat(game,blockedMelee,0.016f);
    assert(near(game.enemies.front().hp,100.0f));

    // An enemy before the same cover remains a valid melee target.
    game.enemies.clear();
    game.enemies.push_back({{0,0.3f,0.5f},100,100,0,0,false,true});
    UpdateCombat(game,blockedMelee,0.016f);
    assert(near(game.enemies.front().hp,74.0f));
    game.platforms.clear();

    // Bow projectiles travel and damage a target in front of the player.
    game.enemies.clear();
    game.player.position = {0,0.3f,0};
    game.player.facing = {0,0,1};
    game.player.weapon = Weapon::Bow;
    game.enemies.push_back({{0,0.3f,1.5f},100,100,0,0,false,true});
    Controls bow{};
    bow.attack = true;
    UpdateCombat(game, bow, 0.016f);
    bow.attack = false;
    UpdateCombat(game, bow, 0.05f);
    assert(near(game.enemies.front().hp, 55.0f));
    assert(game.enemies.front().hitTime>0.1f);
    assert(game.enemies.front().stunTime>0.1f);

    // Fast arrows hit the first target along their path and cannot pass through cover.
    game.projectiles.clear();
    game.enemies.clear();
    game.platforms.clear();
    game.platforms.push_back({{0,1.5f,2.0f},{4,3.0f,0.4f},GRAY});
    game.enemies.push_back({{0,0.3f,3.0f},100,100,0,0,false,true});
    Controls fastBow{};
    fastBow.attack=true;
    UpdateCombat(game,fastBow,0.1f);
    assert(near(game.enemies.front().hp,100.0f));
    assert(game.projectiles.empty());

    // A target in front of cover is hit before the arrow reaches the wall.
    game.enemies.clear();
    game.enemies.push_back({{0,0.3f,1.0f},100,100,0,0,false,true});
    game.projectiles.clear();
    UpdateCombat(game,fastBow,0.1f);
    assert(near(game.enemies.front().hp,55.0f));
    assert(game.projectiles.empty());

    game.platforms.clear();
    // Hit-stunned enemies are pushed back before resuming pursuit.
    game.enemies.clear();
    game.player.position = {0,0.3f,0};
    game.player.facing = {0,0,1};
    game.player.weapon = Weapon::Sword;
    game.enemies.push_back({{0,0.3f,1.8f},100,100,2.5f,0,false,true});
    Controls stunHit{};
    stunHit.attack = true;
    UpdateCombat(game,stunHit,0.016f);
    float hitPosition=game.enemies.front().position.z;
    UpdatePursuit(game,0.05f);
    assert(game.enemies.front().position.z>hitPosition);
    assert(game.enemies.front().stunTime>0);

    // Pursuers stop at solid rooftop walls instead of walking through them.
    game.platforms.clear();
    game.platforms.push_back({{0,1.5f,2.0f},{4,3.0f,0.4f},GRAY});
    game.enemies.clear();
    game.player.position={0,0.3f,6.0f};
    game.enemies.push_back({{0,0.3f,0},100,100,3.0f,0,false,true});
    for(int i=0;i<12;i++) UpdatePursuit(game,0.1f);
    assert(game.enemies.front().position.z<1.6f);

    // Axis-separated collision lets a pursuer move around the wall's open side.
    game.enemies.clear();
    game.player.position={4.0f,0.3f,5.0f};
    game.enemies.push_back({{0,0.3f,0},100,100,3.0f,0,false,true});
    for(int i=0;i<120;i++) UpdatePursuit(game,0.05f);
    assert(game.enemies.front().position.x>2.2f);
    assert(game.enemies.front().position.z>2.2f);
    game.platforms.clear();

    // Blocking spends stamina instead of health; an exhausted block does not.
    game.enemies.clear();
    game.player.health = 100;
    game.player.stamina = 100;
    game.player.blocking = true;
    game.enemies.push_back({{0,0.3f,0.8f},40,40,0,0,false,true});
    UpdatePursuit(game, 0.016f);
    assert(near(game.player.health, 100.0f));
    assert(near(game.player.stamina, 92.0f));
    game.enemies.front().cooldown = 0;
    game.player.stamina = 0;
    UpdatePursuit(game, 0.016f);
    assert(near(game.player.health, 89.0f));
    game.player.blocking = false;

    // A pursuer cannot hit a player through a solid rooftop wall.
    game.enemies.clear();
    game.platforms.clear();
    game.platforms.push_back({{0,1.5f,0.5f},{4,3.0f,0.4f},GRAY});
    game.player.position={0,0.3f,0};
    game.player.health=100;
    game.player.invulnerable=0;
    game.enemies.push_back({{0,0.3f,1.0f},40,40,0,0,false,true});
    UpdatePursuit(game,0.016f);
    assert(near(game.player.health,100.0f));

    // The same pursuer hits normally once the obstructing wall is removed.
    game.platforms.clear();
    UpdatePursuit(game,0.016f);
    assert(near(game.player.health,89.0f));

    game.enemies.clear();
    game.player.position = {0,0.3f,0};
    game.player.facing = {0,0,1};
    game.player.weapon = Weapon::Sword;
    game.enemies.push_back({{0,0.3f,1.5f},20,20,0,0,false,true});
    Controls attack{};
    attack.attack = true;
    UpdateCombat(game, attack, 0.016f);
    assert(!game.enemies.front().alive);
    assert(game.defeated == 1);

    game.enemies.clear();
    game.player.position = {0,0.3f,0};
    game.player.facing = {0,0,1};
    game.enemies.push_back({{0,0.3f,1.5f},10,10,0,0,true,true});
    UpdateCombat(game, attack, 0.016f);
    assert(game.city == 1);
    assert(game.unlockedCity == 1);

    // Bosses must begin their telegraphed attacks at their own effective ranges.
    const float bossWindupRanges[5]={4.0f,2.3f,5.5f,8.0f,4.5f};
    for(int variant=0;variant<5;variant++) {
        game.enemies.clear();
        game.player.position={0,0.3f,0};
        Enemy boss{{0,0.3f,bossWindupRanges[variant]*0.9f},100,100,2.5f,0,true,true};
        boss.variant=variant;
        boss.phase=BossPhase::Hunt;
        game.enemies.push_back(boss);
        UpdatePursuit(game,0.016f);
        assert(game.enemies.front().phase==BossPhase::Windup);
    }

    // Each city boss has a distinct attack pattern, range and damage profile.
    const float bossDistances[5]={3.5f,2.2f,5.0f,7.0f,4.0f};
    const float bossDamage[5]={24,30,32,18,28};
    for(int variant=0;variant<5;variant++) {
        game.enemies.clear();
        game.player.position={0,0.3f,0};
        game.player.health=100;
        game.player.invulnerable=0;
        game.player.blocking=false;
        game.player.stamina=100;
        Enemy boss{{0,0.3f,bossDistances[variant]},100,100,2.5f,0,true,true};
        boss.variant=variant;
        boss.phase=BossPhase::Strike;
        boss.phaseTime=1.0f;
        game.enemies.push_back(boss);
        UpdatePursuit(game,0.05f);
        assert(near(game.player.health,100.0f-bossDamage[variant]));
        assert(game.enemies.front().phase==BossPhase::Recover);
    }

    // The Gilded Charger advances rapidly during its strike before impact.
    game.enemies.clear();
    game.player.position={0,0.3f,0};
    game.player.health=100;
    game.player.invulnerable=0;
    Enemy charger{{0,0.3f,4.0f},100,100,3.0f,0,true,true};
    charger.variant=1;
    charger.phase=BossPhase::Strike;
    game.enemies.push_back(charger);
    UpdatePursuit(game,0.2f);
    assert(game.enemies.front().position.z<2.0f);
    assert(near(game.player.health,100.0f));
    UpdatePursuit(game,0.15f);
    assert(near(game.player.health,70.0f));

    // A Gilded Charger cannot charge through a solid rooftop wall.
    game.platforms.clear();
    game.platforms.push_back({{0,1.5f,2.0f},{4,3.0f,0.4f},GRAY});
    game.enemies.clear();
    game.player.position={0,0.3f,6.0f};
    Enemy coveredCharger{{0,0.3f,0},100,100,3.0f,0,true,true};
    coveredCharger.variant=1;
    coveredCharger.phase=BossPhase::Strike;
    game.enemies.push_back(coveredCharger);
    for(int i=0;i<4;i++) UpdatePursuit(game,0.05f);
    assert(game.enemies.front().position.z<1.3f);
    game.platforms.clear();

    // Boss strikes cannot damage a player through a solid rooftop wall.
    game.platforms.clear();
    game.platforms.push_back({{0,1.5f,1.5f},{4,3.0f,0.4f},GRAY});
    game.player.position={0,0.3f,0};
    game.player.health=100;
    game.player.invulnerable=0;
    game.player.blocking=false;
    Enemy coveredBoss{{0,0.3f,3.5f},100,100,0,0,true,true};
    coveredBoss.variant=0;
    coveredBoss.phase=BossPhase::Strike;
    coveredBoss.phaseTime=1.0f;
    game.enemies.clear();
    game.enemies.push_back(coveredBoss);
    UpdatePursuit(game,0.05f);
    assert(near(game.player.health,100.0f));

    // The same boss strike still damages a player when no cover separates them.
    game.player.health=100;
    game.player.invulnerable=0;
    Enemy exposedBoss{{0,0.3f,0.7f},100,100,0,0,true,true};
    exposedBoss.variant=0;
    exposedBoss.phase=BossPhase::Strike;
    exposedBoss.phaseTime=1.0f;
    game.enemies.clear();
    game.enemies.push_back(exposedBoss);
    UpdatePursuit(game,0.05f);
    assert(near(game.player.health,76.0f));
    game.platforms.clear();

    // Defeating each city boss advances the campaign; the fifth ends it.
    game.unlockedCity = 0;
    game.finished = false;
    for (int city = 0; city < 5; ++city) {
        game.city = city;
        BuildCity(game);
        game.enemies.clear();
        game.player.position = {0,0.3f,0};
        game.player.facing = {0,0,1};
        game.player.weapon = Weapon::Axe;
        game.enemies.push_back({{0,0.3f,1.5f},10,10,0,0,true,true});
        Controls bossHit{};
        bossHit.attack = true;
        UpdateCombat(game, bossHit, 0.016f);
        if (city < 4) {
            assert(game.city == city + 1);
            assert(game.unlockedCity >= city + 1);
        } else {
            assert(game.city == 4);
            assert(game.finished);
        }
    }

    // Stamina caps, regeneration, and level-up refills match each class.
    const float classStamina[3]={95.0f,100.0f,100.0f};
    for(int klass=0;klass<3;klass++) {
        game.player.playerClass=(PlayerClass)klass;
        assert(near(ClassMaxStamina(game.player.playerClass),classStamina[klass]));
        game.player.stamina=classStamina[klass]-1.0f;
        UpdateMovement(game,Controls{},camera,0.1f);
        assert(near(game.player.stamina,classStamina[klass]));
        game.player.stamina=1.0f;
        game.player.xp=game.player.level*100-1;
        UpdateProgression(game,1);
        assert(near(game.player.stamina,classStamina[klass]));
    }

    // Each class's maximum health remains consistent when leveling up.
    const float classMaximums[3]={125.0f,95.0f,115.0f};
    for(int klass=0;klass<3;klass++) {
        game.player.playerClass=(PlayerClass)klass;
        assert(near(ClassMaxHealth(game.player.playerClass),classMaximums[klass]));
        game.player.health=classMaximums[klass]-5.0f;
        game.player.level=1;
        game.player.xp=99;
        UpdateProgression(game,1);
        assert(near(game.player.health,classMaximums[klass]));
        game.player.xp=99;
        UpdateProgression(game,1);
        assert(near(game.player.health,classMaximums[klass]));
    }


    const float classHealth[3]={125.0f,95.0f,115.0f};
    for(int klass=0;klass<3;klass++) {
        game.enemies.clear();
        game.player.playerClass=(PlayerClass)klass;
        game.player.health=0;
        game.player.stamina=0;
        Controls idle{};
        UpdateCombat(game,idle,0.016f);
        assert(near(game.player.health,classHealth[klass]));
        assert(near(game.player.stamina,classStamina[klass]));
    }

    std::cout << "PASS: city setup, names, XP/skills, movement, jump, dash, melee combat, and boss progression.\\n";
    return 0;
}
