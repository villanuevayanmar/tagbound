#include "game.hpp"
#include "raymath.h"
#include <algorithm>
#include <cmath>

static void DrawCityLandmarks(int city) {
    switch (city) {
        case 0: // Port town: dock cranes and stacked cargo.
            for (int i=0;i<3;i++) {
                float x=-25.0f+i*9.0f;
                DrawCylinder({x,2.5f,-15},0.16f,0.16f,5.0f,8,BROWN);
                DrawCubeV({x+1.8f,4.8f,-15},{3.8f,0.16f,0.16f},BROWN);
                DrawCubeV({x+3.2f,3.9f,-15},{0.14f,1.8f,0.14f},DARKBROWN);
                DrawCubeV({x+3.2f,2.9f,-13.8f},{2.2f,1.6f,2.2f},{116,76,48,255});
            }
            break;
        case 1: // Merchant tier: colorful market awnings.
            for (int i=0;i<4;i++) {
                float x=-27.0f+i*7.0f;
                Color cloth=i%2?Color{153,76,59,255}:Color{190,145,78,255};
                DrawCubeV({x,2.4f,-14},{4.2f,0.24f,3.2f},cloth);
                for (int side=-1;side<=1;side+=2)
                    DrawCylinder({x+side*1.75f,1.2f,-14},0.08f,0.08f,2.4f,6,BROWN);
                DrawCubeV({x,0.55f,-14},{3.5f,1.0f,2.5f},{91,67,51,255});
            }
            break;
        case 2: // Foundry: furnaces, chimneys and glowing vents.
            for (int i=0;i<3;i++) {
                float x=-25.0f+i*9.0f;
                DrawCubeV({x,3.8f,-15},{4.0f,7.6f,4.0f},{65,72,79,255});
                DrawCylinder({x,8.3f,-15},0.8f,1.0f,2.2f,8,{93,92,91,255});
                DrawCubeV({x,0.35f,-12.85f},{1.4f,0.18f,0.12f},ORANGE);
            }
            break;
        case 3: // High spire: narrow towers and pointed roofs.
            for (int i=0;i<4;i++) {
                float x=-27.0f+i*8.0f, h=12.0f+(i%2)*5.0f;
                DrawCylinder({x,h*0.5f,-16},1.15f,1.45f,h,8,{92,88,124,255});
                DrawCylinder({x,h+1.1f,-16},0.0f,1.5f,2.2f,8,{173,143,103,255});
            }
            break;
        case 4: // Imperial capital: curtain wall and fortified towers.
            DrawCubeV({-12,3.0f,-17},{27.0f,6.0f,1.4f},{112,83,78,255});
            for (int i=0;i<4;i++) {
                float x=-24.0f+i*8.0f;
                DrawCylinder({x,5.2f,-17},1.45f,1.7f,10.4f,8,{133,91,80,255});
                DrawCylinder({x,11.0f,-17},0.0f,1.7f,2.0f,8,{189,148,104,255});
            }
            break;
    }
}

void DrawGame(const GameState& g,const Camera3D& camera) {
    static const Color sky[5]={{32,52,68,255},{53,52,69,255},{33,43,54,255},{55,49,77,255},{51,36,46,255}};
    ClearBackground(sky[g.city]);
    BeginMode3D(camera);
    DrawPlane({0,-0.32f,0},{120,120},{45,50,57,255});
    for(const auto& p:g.platforms) {
        DrawCubeV(p.position,p.size,p.color);
        DrawCubeWiresV(p.position,p.size,{28,29,36,255});
    }
    for(int i=0;i<32;i++) {
        float a=i*0.19635f, r=47.0f+(i%4)*2.0f;
        Vector3 p={std::cos(a)*r,3.5f+(i%6)*1.5f,std::sin(a)*r};
        DrawCubeV(p,{5.0f,7.0f+(i%6)*3.0f,5.0f},{25,31,43,255});
    }
    const Vector3 points[3]={{4,1.7f,6},{10,2.45f,13},{16,3.2f,20}};
    Vector3 marker=g.quest<3?Vector3Add(points[g.quest],{0,1.2f,0}):Vector3{22,5.4f,27};
    DrawCylinder(marker,0.38f,0.08f,2.1f,8,g.quest<3?GOLD:RED);
    DrawSphere(Vector3Add(marker,{0,1.2f,0}),0.5f,g.quest<3?GOLD:MAROON);

    const Player& player=g.player;
    Color tunic=player.playerClass==PlayerClass::Vanguard?Color{139,67,58,255}:(player.playerClass==PlayerClass::Skirmisher?Color{58,112,89,255}:Color{78,89,116,255});
    Color flash=player.invulnerable>0?WHITE:tunic;
    DrawCubeV({player.position.x,player.position.y+0.78f,player.position.z},{0.7f,1.55f,0.48f},flash);
    DrawSphere({player.position.x,player.position.y+1.7f,player.position.z},0.31f,{194,158,126,255});
    Vector3 forward=Vector3Normalize(player.facing), right={forward.z,0,-forward.x};
    Vector3 hand=Vector3Add(player.position,{forward.x*0.48f,1.05f,forward.z*0.48f});
    Vector3 tip=Vector3Add(hand,Vector3Scale(forward,0.95f));
    switch(player.weapon) {
        case Weapon::Sword:
            DrawCylinderEx(Vector3Subtract(hand,Vector3Scale(forward,0.22f)),hand,0.065f,0.055f,6,BROWN);
            DrawCylinderEx(hand,tip,0.055f,0.018f,7,LIGHTGRAY);
            break;
        case Weapon::Axe: {
            Vector3 shaft=Vector3Add(hand,Vector3Scale(forward,0.62f));
            DrawCylinderEx(hand,shaft,0.055f,0.04f,7,BROWN);
            DrawCubeV(Vector3Add(shaft,Vector3Scale(right,0.13f)),{0.28f,0.34f,0.14f},LIGHTGRAY);
            break;
        }
        case Weapon::Shield:
            DrawCubeV(Vector3Add(hand,Vector3Scale(forward,0.12f)),{0.42f,0.55f,0.16f},GRAY);
            break;
        case Weapon::Bow: {
            Vector3 a=Vector3Add(hand,Vector3Scale(right,0.22f));
            Vector3 b=Vector3Add(hand,Vector3Scale(right,0.08f));
            Vector3 c=Vector3Subtract(hand,Vector3Scale(right,0.08f));
            Vector3 d=Vector3Subtract(hand,Vector3Scale(right,0.22f));
            DrawCylinderEx(a,b,0.035f,0.025f,7,BROWN);
            DrawCylinderEx(b,c,0.025f,0.025f,7,BROWN);
            DrawCylinderEx(c,d,0.025f,0.035f,7,BROWN);
            DrawLine3D(a,d,LIGHTGRAY);
            break;
        }
        case Weapon::Spear: {
            Vector3 spearTip=Vector3Add(hand,Vector3Scale(forward,1.35f));
            DrawCylinderEx(hand,spearTip,0.045f,0.025f,7,BROWN);
            DrawCylinderEx(spearTip,Vector3Add(spearTip,Vector3Scale(forward,0.2f)),0.07f,0.0f,7,LIGHTGRAY);
            break;
        }
    }

    for(const auto& e:g.enemies) {
        if(!e.alive) continue;
        Color enemyColor=e.boss?(e.phase==BossPhase::Windup?ORANGE:(e.phase==BossPhase::Strike?RED:MAROON)):Color{124,54,49,255};
        float scale=e.boss?1.25f:0.72f;
        DrawCubeV({e.position.x,e.position.y+0.76f,e.position.z},{scale,e.boss?1.9f:1.5f,e.boss?1.0f:0.48f},enemyColor);
        DrawSphere({e.position.x,e.position.y+(e.boss?1.78f:1.7f),e.position.z},e.boss?0.42f:0.29f,BEIGE);
        float width=e.boss?2.0f:1.0f;
        DrawCubeV({e.position.x,e.position.y+2.3f,e.position.z},{width,0.09f,0.1f},DARKGRAY);
        DrawCubeV({e.position.x-width*0.5f*(1-e.hp/e.maxHp),e.position.y+2.3f,e.position.z},{width*(e.hp/e.maxHp),0.1f,0.12f},RED);
    }
    for(const auto& s:g.projectiles) DrawSphere(s.position,0.12f,GOLD);
    DrawCityLandmarks(g.city);
    DrawGrid(80,1.0f);
    EndMode3D();

    int w=GetScreenWidth(),h=GetScreenHeight();
    DrawRectangle(12,12,250,75,Fade(BLACK,0.65f));
    DrawText(TextFormat("HEALTH  %d",(int)g.player.health),24,20,16,WHITE);
    DrawRectangle(24,42,215,9,DARKGRAY);
    DrawRectangle(24,42,(int)(215*std::max(0.0f,g.player.health/125.0f)),9,RED);
    DrawText(TextFormat("STAMINA  %d",(int)g.player.stamina),24,56,14,WHITE);
    DrawRectangle(112,60,127,7,DARKGRAY);
    DrawRectangle(112,60,(int)(127*g.player.stamina/100.0f),7,GREEN);
    if(g.player.escapeSkill) DrawText(g.player.dashCooldown<=0?"DASH READY":TextFormat("DASH %.1fs",g.player.dashCooldown),24,73,10,GOLD);
    else DrawText("DASH UNLOCKS AT LVL 3",24,73,10,GRAY);
    if(g.player.wallRunning) DrawText("WALL RUN",180,73,10,ORANGE);
    DrawRectangle(w-245,12,233,76,Fade(BLACK,0.65f));
    DrawText(TextFormat("LEVEL %d   XP %d/%d",g.player.level,g.player.xp,g.player.level*100),w-233,20,15,WHITE);
    DrawText(TextFormat("TAG DANGER   %d%%",(int)g.danger),w-233,44,15,g.danger>70?RED:ORANGE);
    DrawRectangle(w-233,67,205,8,DARKGRAY);
    DrawRectangle(w-233,67,(int)(205*g.danger/100),8,ORANGE);
    DrawRectangle(12,h-78,390,65,Fade(BLACK,0.7f));
    DrawText(TextFormat("%s  |  %s",CityName(g.city),WeaponName(g.player.weapon)),24,h-69,16,GOLD);
    DrawText(g.quest<3?TextFormat("QUEST %d/3  -  REACH THE BEACON",g.quest+1):"BOSS GATE OPEN  -  DEFEAT THE CITY WARDEN",24,h-44,14,WHITE);
    DrawText(TextFormat("TAGGERS %d",(int)std::count_if(g.enemies.begin(),g.enemies.end(),[](const Enemy& e){return e.alive&&!e.boss;})),24,h-24,12,LIGHTGRAY);
    if(g.finished) {
        DrawRectangle(0,0,w,h,Fade(BLACK,0.7f));
        DrawText("THE FIVE CITIES ARE FREE",w/2-190,h/2-18,28,GOLD);
    }
}
