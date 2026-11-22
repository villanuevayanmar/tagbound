#include "game.hpp"
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

    // Respawning restores the selected class's actual health maximum.
    const float classHealth[3]={125.0f,95.0f,115.0f};
    for(int klass=0;klass<3;klass++) {
        game.enemies.clear();
        game.player.playerClass=(PlayerClass)klass;
        game.player.health=0;
        Controls idle{};
        UpdateCombat(game,idle,0.016f);
        assert(near(game.player.health,classHealth[klass]));
    }

    std::cout << "PASS: city setup, names, XP/skills, movement, jump, dash, melee combat, and boss progression.\\n";
    return 0;
}
