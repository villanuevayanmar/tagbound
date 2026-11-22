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

    std::cout << "PASS: city setup, names, XP/skills, movement, jump, dash, melee combat, and boss progression.\\n";
    return 0;
}
