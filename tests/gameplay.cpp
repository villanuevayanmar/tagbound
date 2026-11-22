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

    std::cout << "PASS: city setup, names, XP/skills, movement, jump, dash, melee combat, and boss progression.\\n";
    return 0;
}
