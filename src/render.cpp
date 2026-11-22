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

static Vector3 BodyPoint(Vector3 origin,Vector3 right,Vector3 forward,float side,float height,float depth) {
    return Vector3Add(origin,Vector3Add(Vector3Scale(right,side),Vector3Add(Vector3Scale(forward,depth),Vector3{0,height,0})));
}

static void DrawLimb(Vector3 start,Vector3 end,float startRadius,float endRadius,Color color) {
    if (Vector3Distance(start,end)>0.01f)
        DrawCylinderEx(start,end,startRadius,endRadius,7,color);
    DrawSphere(start,startRadius,color);
    DrawSphere(end,endRadius,color);
}

static Vector3 DrawHero(const Player& p,float swing,float bob) {
    Vector3 forward=Vector3Length(p.facing)>0.01f?Vector3Normalize(p.facing):Vector3{0,0,1};
    Vector3 right={forward.z,0,-forward.x};
    Color tunic=p.playerClass==PlayerClass::Vanguard?Color{139,67,58,255}:
                (p.playerClass==PlayerClass::Skirmisher?Color{58,112,89,255}:Color{78,89,116,255});
    Color cloth=p.invulnerable>0?WHITE:tunic;
    Color pants={43,45,53,255}, leather={77,54,39,255}, metal={139,145,151,255};
    float breath=bob;

    Vector3 waist=BodyPoint(p.position,right,forward,0,0.72f+breath,0);
    Vector3 chest=BodyPoint(p.position,right,forward,0,1.30f+breath,0);
    Vector3 neck=BodyPoint(p.position,right,forward,0,1.48f+breath,0);
    DrawCylinderEx(waist,chest,0.22f,0.29f,8,cloth);
    DrawCylinderEx(chest,neck,0.29f,0.13f,8,cloth);
    DrawCylinder(BodyPoint(p.position,right,forward,0,0.78f+breath,0),0.245f,0.245f,0.09f,8,leather);
    DrawCylinderEx(BodyPoint(p.position,right,forward,-0.19f,1.18f+breath,0.12f),
                   BodyPoint(p.position,right,forward,0.19f,0.88f+breath,0.13f),0.035f,0.035f,6,leather);

    Vector3 head=BodyPoint(p.position,right,forward,0,1.73f+breath,0.015f);
    DrawCylinderEx(BodyPoint(p.position,right,forward,0,1.42f+breath,0),
                   BodyPoint(p.position,right,forward,0,1.56f+breath,0),0.105f,0.11f,7,BEIGE);
    DrawSphere(head,0.285f,{194,158,126,255});
    Color headgear=p.playerClass==PlayerClass::Vanguard?Color{85,91,99,255}:
                   (p.playerClass==PlayerClass::Skirmisher?Color{35,48,43,255}:Color{48,57,77,255});
    DrawSphere(BodyPoint(p.position,right,forward,0,1.91f+breath,-0.015f),0.235f,headgear);
    DrawSphere(BodyPoint(p.position,right,forward,-0.105f,1.77f+breath,0.244f),0.035f,{35,31,29,255});
    DrawSphere(BodyPoint(p.position,right,forward,0.105f,1.77f+breath,0.244f),0.035f,{35,31,29,255});
    DrawSphere(BodyPoint(p.position,right,forward,0,1.68f+breath,0.278f),0.045f,{167,119,91,255});

    float legLift=(p.vaulting||p.ledgeGrabbing||p.climbing)?0.26f:(p.grounded?0.0f:0.12f);
    float leftDepth=swing*0.36f, rightDepth=-swing*0.36f;
    Vector3 leftHip=BodyPoint(p.position,right,forward,-0.145f,0.69f+breath,0);
    Vector3 rightHip=BodyPoint(p.position,right,forward,0.145f,0.69f+breath,0);
    Vector3 leftKnee=BodyPoint(p.position,right,forward,-0.18f,0.38f+legLift,leftDepth);
    Vector3 rightKnee=BodyPoint(p.position,right,forward,0.18f,0.38f+legLift,rightDepth);
    Vector3 leftAnkle=BodyPoint(p.position,right,forward,-0.18f,0.13f+legLift,leftDepth*0.65f);
    Vector3 rightAnkle=BodyPoint(p.position,right,forward,0.18f,0.13f+legLift,rightDepth*0.65f);
    DrawLimb(leftHip,leftKnee,0.145f,0.105f,pants);
    DrawLimb(rightHip,rightKnee,0.145f,0.105f,pants);
    DrawLimb(leftKnee,leftAnkle,0.105f,0.075f,pants);
    DrawLimb(rightKnee,rightAnkle,0.105f,0.075f,pants);
    DrawSphere(BodyPoint(p.position,right,forward,-0.18f,0.10f+legLift,leftDepth*0.65f+0.08f),0.12f,leather);
    DrawSphere(BodyPoint(p.position,right,forward,0.18f,0.10f+legLift,rightDepth*0.65f+0.08f),0.12f,leather);

    Vector3 leftShoulder=BodyPoint(p.position,right,forward,-0.30f,1.35f+breath,0);
    Vector3 rightShoulder=BodyPoint(p.position,right,forward,0.30f,1.35f+breath,0);
    float armSwing=p.blocking?0.25f:-swing*0.30f;
    Vector3 leftElbow=BodyPoint(p.position,right,forward,-0.39f,(p.vaulting||p.ledgeGrabbing||p.climbing)?1.48f:(1.06f+breath),(p.vaulting||p.ledgeGrabbing||p.climbing)?0.22f:(p.blocking?0.34f:armSwing));
    Vector3 leftHand=BodyPoint(p.position,right,forward,-0.43f,(p.vaulting||p.ledgeGrabbing||p.climbing)?1.68f:(0.91f+breath),(p.vaulting||p.ledgeGrabbing||p.climbing)?0.34f:(p.blocking?0.52f:armSwing*1.35f));
    Vector3 rightElbow=BodyPoint(p.position,right,forward,0.39f,(p.vaulting||p.ledgeGrabbing||p.climbing)?1.48f:(1.08f+breath),(p.vaulting||p.ledgeGrabbing||p.climbing)?0.22f:(p.bowAiming?0.28f:(p.blocking?0.34f:0.12f+swing*0.16f)));
    Vector3 rightHand=BodyPoint(p.position,right,forward,0.43f,(p.vaulting||p.ledgeGrabbing||p.climbing)?1.68f:(0.94f+breath),(p.vaulting||p.ledgeGrabbing||p.climbing)?0.34f:(p.bowAiming?0.48f:(p.blocking?0.52f:0.31f+swing*0.18f)));
    Color armColor=cloth;
    DrawLimb(leftShoulder,leftElbow,0.13f,0.095f,armColor);
    DrawLimb(leftElbow,leftHand,0.095f,0.065f,armColor);
    DrawLimb(rightShoulder,rightElbow,0.13f,0.095f,armColor);
    DrawLimb(rightElbow,rightHand,0.095f,0.065f,armColor);
    DrawSphere(leftShoulder,0.16f,p.playerClass==PlayerClass::Skirmisher?cloth:metal);
    DrawSphere(rightShoulder,0.16f,p.playerClass==PlayerClass::Skirmisher?cloth:metal);
    DrawSphere(leftHand,0.075f,{194,158,126,255});
    DrawSphere(rightHand,0.075f,{194,158,126,255});

    if (p.playerClass==PlayerClass::Warden) {
        DrawCubeV(BodyPoint(p.position,right,forward,0,1.08f,-0.19f),{0.48f,0.78f,0.10f},{43,53,75,255});
    }
    return rightHand;
}

static void DrawEnemyModel(const Enemy& e,float time) {
    Vector3 forward={0,0,-1}, right={1,0,0};
    Color hide=e.boss?Color{82,45,53,255}:Color{105,62,52,255};
    Color armor=e.boss?Color{91,91,105,255}:Color{72,76,78,255};
    Color skin=e.boss?Color{160,111,87,255}:Color{169,128,103,255};
    float scale=e.boss?1.22f:0.82f;
    float swing=std::sin(time*5.0f+e.position.x)*0.18f;
    if(e.phase==BossPhase::Windup) swing=0.55f;
    if(e.phase==BossPhase::Strike) swing=-0.42f;
    Vector3 origin=e.position;
    Vector3 hip=BodyPoint(origin,right,forward,0,0.66f*scale,0);
    Vector3 chest=BodyPoint(origin,right,forward,0,1.18f*scale,0);
    Vector3 head=BodyPoint(origin,right,forward,0,1.58f*scale,0);
    DrawCylinderEx(hip,chest,0.22f*scale,0.29f*scale,8,hide);
    DrawSphere(head,0.25f*scale,skin);
    DrawSphere(BodyPoint(origin,right,forward,0,1.76f*scale,-0.02f),0.22f*scale,armor);
    DrawCubeV(BodyPoint(origin,right,forward,0,1.18f*scale,0.15f),{0.52f*scale,0.72f*scale,0.12f*scale},armor);
    DrawCubeV(BodyPoint(origin,right,forward,0,0.72f*scale,0.15f),{0.48f*scale,0.10f*scale,0.12f*scale},BROWN);
    for(int side=-1;side<=1;side+=2) {
        float x=side*0.14f*scale;
        Vector3 thigh=BodyPoint(origin,right,forward,x,0.36f*scale,swing*side);
        Vector3 foot=BodyPoint(origin,right,forward,x,0.08f*scale,swing*side*0.7f);
        DrawLimb(BodyPoint(origin,right,forward,x,0.65f*scale,0),thigh,0.13f*scale,0.095f*scale,hide);
        DrawLimb(thigh,foot,0.095f*scale,0.07f*scale,armor);
        Vector3 shoulder=BodyPoint(origin,right,forward,side*0.31f*scale,1.28f*scale,0);
        Vector3 elbow=BodyPoint(origin,right,forward,side*0.39f*scale,0.95f*scale,swing*side);
        Vector3 hand=BodyPoint(origin,right,forward,side*0.42f*scale,0.76f*scale,swing*side*1.2f);
        DrawLimb(shoulder,elbow,0.13f*scale,0.09f*scale,armor);
        DrawLimb(elbow,hand,0.09f*scale,0.065f*scale,hide);
        DrawSphere(shoulder,0.15f*scale,e.boss?MAROON:armor);
    }
    if(e.boss) {
        DrawSphere(BodyPoint(origin,right,forward,0,1.95f*scale,0),0.10f*scale,e.phase==BossPhase::Windup?ORANGE:RED);
    } else {
        DrawSphere(BodyPoint(origin,right,forward,0,1.38f*scale,0.25f*scale),0.055f*scale,RED);
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
    static float animationClock=0.0f;
    static Vector3 previousPosition=player.position;
    static bool hasPreviousPosition=false;
    float frameDt=std::clamp(GetFrameTime(),0.001f,0.1f);
    float dx=player.position.x-previousPosition.x, dz=player.position.z-previousPosition.z;
    float distance=std::sqrt(dx*dx+dz*dz);
    float moveSpeed=distance/frameDt;
    if(!hasPreviousPosition||distance>3.0f) moveSpeed=0.0f;
    previousPosition=player.position;
    hasPreviousPosition=true;
    animationClock+=frameDt;
    float stride=std::clamp(moveSpeed/6.0f,0.0f,1.35f);
    float gait=std::sin(animationClock*(moveSpeed>7.0f?13.0f:9.0f))*0.48f*stride;
    float bodyBob=std::sin(animationClock*2.2f)*0.018f*(1.0f-stride);
    Vector3 forward=Vector3Length(player.facing)>0.01f?Vector3Normalize(player.facing):Vector3{0,0,1};
    Vector3 right={forward.z,0,-forward.x};
    Vector3 hand=DrawHero(player,gait,bodyBob);
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
            if(player.bowAiming) DrawLine3D(hand,Vector3Add(hand,Vector3Scale(forward,0.75f)),GOLD);
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
        DrawEnemyModel(e,animationClock);
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
    if(g.player.ledgeGrabbing) DrawText("HANGING - JUMP: CLIMB / DOWN: DROP",w/2-155,28,14,GOLD);
    else if(g.player.climbing) DrawText("CLIMBING...",w/2-55,28,14,GOLD);
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
