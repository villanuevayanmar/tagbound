#include "game.hpp"
#include "raymath.h"
#include <algorithm>
#include <cmath>

static void DrawCityLandmarks(int city) {
    switch (city) {
        case 0: // Port town: dock cranes and stacked cargo.
            for (int i=0;i<3;i++) {
                float x=-25.0f+i*9.0f;
                DrawCylinder({x,2.5f,38},0.16f,0.16f,5.0f,8,BROWN);
                DrawCubeV({x+1.8f,4.8f,38},{3.8f,0.16f,0.16f},BROWN);
                DrawCubeV({x+3.2f,3.9f,38},{0.14f,1.8f,0.14f},DARKBROWN);
                DrawCubeV({x+3.2f,2.9f,39.2f},{2.2f,1.6f,2.2f},{116,76,48,255});
            }
            break;
        case 1: // Merchant tier: colorful market awnings.
            for (int i=0;i<4;i++) {
                float x=-27.0f+i*7.0f;
                Color cloth=i%2?Color{153,76,59,255}:Color{190,145,78,255};
                DrawCubeV({x,2.4f,39},{4.2f,0.24f,3.2f},cloth);
                for (int side=-1;side<=1;side+=2)
                    DrawCylinder({x+side*1.75f,1.2f,39},0.08f,0.08f,2.4f,6,BROWN);
                DrawCubeV({x,0.55f,39},{3.5f,1.0f,2.5f},{91,67,51,255});
            }
            break;
        case 2: // Foundry: furnaces, chimneys and glowing vents.
            for (int i=0;i<3;i++) {
                float x=-25.0f+i*9.0f;
                DrawCubeV({x,3.8f,38},{4.0f,7.6f,4.0f},{65,72,79,255});
                DrawCylinder({x,8.3f,38},0.8f,1.0f,2.2f,8,{93,92,91,255});
                DrawCubeV({x,0.35f,40.15f},{1.4f,0.18f,0.12f},ORANGE);
            }
            break;
        case 3: // Imperial capital: curtain wall and fortified towers.
            DrawCubeV({-12,3.0f,36},{27.0f,6.0f,1.4f},{112,83,78,255});
            for (int i=0;i<4;i++) {
                float x=-24.0f+i*8.0f;
                DrawCylinder({x,5.2f,36},1.45f,1.7f,10.4f,8,{133,91,80,255});
                DrawCylinder({x,11.0f,36},0.0f,1.7f,2.0f,8,{189,148,104,255});
            }
            break;
        case 4: // High spire: narrow towers and pointed roofs.
            for (int i=0;i<4;i++) {
                float x=-27.0f+i*8.0f, h=12.0f+(i%2)*5.0f;
                DrawCylinder({x,h*0.5f,37},1.15f,1.45f,h,8,{92,88,124,255});
                DrawCylinder({x,h+1.1f,37},0.0f,1.5f,2.2f,8,{173,143,103,255});
            }
            break;
    }
}


static void DrawBuildingDetails(const Platform& p,int city,Vector3 viewer) {
    if(p.size.x<4.0f||p.size.y<2.0f||p.size.z<4.0f) return;
    float dx=p.position.x-viewer.x, dz=p.position.z-viewer.z;
    if(dx*dx+dz*dz>1156.0f) return;

    static const Color glass[5]={{211,166,91,255},{231,190,112,255},{255,117,55,255},{239,177,119,255},{157,190,204,255}};
    static const Color trim[5]={{105,145,143,255},{183,133,81,255},{118,132,143,255},{171,98,88,255},{143,133,178,255}};
    int style=std::clamp(city,0,4);
    float roof=p.position.y+p.size.y*0.5f;
    float base=p.position.y-p.size.y*0.5f;

    DrawCubeV({p.position.x,roof+0.07f,p.position.z},{p.size.x+0.22f,0.14f,p.size.z+0.22f},trim[style]);
    DrawCubeV({p.position.x,base+0.16f,p.position.z},{p.size.x+0.12f,0.22f,p.size.z+0.12f},{32,35,43,255});

    int floors=std::clamp((int)(p.size.y/1.55f),1,3);
    for(int floor=0;floor<floors;floor++) {
        float y=base+0.92f+floor*1.38f;
        if(y>roof-0.28f) break;
        for(int side=-1;side<=1;side+=2) {
            DrawCubeV({p.position.x+side*0.92f,y,p.position.z+p.size.z*0.505f},
                      {0.48f,0.58f,0.065f},glass[style]);
            DrawCubeV({p.position.x+p.size.x*0.505f,y,p.position.z+side*0.92f},
                      {0.065f,0.58f,0.48f},glass[style]);
        }
    }

    Vector3 roofCenter={p.position.x,roof+0.24f,p.position.z};
    if(style==0) {
        DrawCubeV({roofCenter.x-0.72f,roofCenter.y,roofCenter.z},{0.9f,0.42f,0.75f},{74,79,83,255});
        DrawCylinder({roofCenter.x+0.72f,roofCenter.y+0.12f,roofCenter.z},0.38f,0.38f,0.62f,8,{111,126,132,255});
    } else if(style==1) {
        DrawCubeV({roofCenter.x,roofCenter.y+0.12f,roofCenter.z},{1.55f,0.12f,1.2f},trim[style]);
        DrawCylinder({roofCenter.x-0.55f,roofCenter.y-0.04f,roofCenter.z},0.06f,0.06f,0.42f,6,BROWN);
        DrawCylinder({roofCenter.x+0.55f,roofCenter.y-0.04f,roofCenter.z},0.06f,0.06f,0.42f,6,BROWN);
    } else if(style==2) {
        DrawCubeV(roofCenter,{0.9f,0.38f,0.9f},{65,72,79,255});
        DrawCylinder({roofCenter.x,roofCenter.y+0.35f,roofCenter.z},0.22f,0.30f,0.7f,8,{112,116,120,255});
        DrawCubeV({roofCenter.x,roofCenter.y+0.72f,roofCenter.z},{0.55f,0.10f,0.55f},ORANGE);
    } else if(style==3) {
        for(int side=-1;side<=1;side+=2)
            DrawCubeV({roofCenter.x+side*0.72f,roofCenter.y+0.12f,roofCenter.z},
                      {0.42f,0.38f,0.72f},trim[style]);
        DrawCubeV({roofCenter.x,roofCenter.y+0.12f,roofCenter.z},
                  {0.42f,0.38f,0.72f},trim[style]);
    } else {
        DrawCubeV(roofCenter,{0.72f,0.28f,0.72f},{76,73,103,255});
        DrawCylinder({roofCenter.x,roofCenter.y+0.72f,roofCenter.z},0.025f,0.11f,1.0f,6,trim[style]);
    }
}

static void DrawRooftopProps(const Platform& p,int city) {
    if(p.size.x<4.0f||p.size.x>8.5f||p.size.y>0.65f||p.position.y<0.0f) return;
    float top=p.position.y+p.size.y*0.5f;
    float x=p.position.x-p.size.x*0.31f;
    float z=p.position.z-p.size.z*0.28f;
    Color metal=city==2?Color{91,101,108,255}:Color{70,76,82,255};
    DrawCubeV({x,top+0.16f,z},{0.72f,0.30f,0.62f},metal);
    DrawCubeV({x,top+0.33f,z},{0.48f,0.06f,0.40f},{133,139,143,255});
    if(city==0) DrawCylinder({x+0.62f,top+0.34f,z+0.18f},0.18f,0.18f,0.55f,8,{105,123,128,255});
    else if(city==2) DrawCylinder({x+0.62f,top+0.38f,z+0.18f},0.12f,0.16f,0.72f,8,{112,116,120,255});
    else if(city==4) DrawCylinder({x+0.62f,top+0.58f,z+0.18f},0.025f,0.09f,0.8f,6,{143,133,178,255});
}

static void DrawGroundGrid(Vector3 center,int city) {
    static const Color tint[5]={{91,126,132,255},{151,112,82,255},{111,123,132,255},{151,99,91,255},{132,119,163,255}};
    Color minor=Fade(tint[std::clamp(city,0,4)],0.22f);
    Color major=Fade(tint[std::clamp(city,0,4)],0.42f);
    constexpr float half=40.0f;
    constexpr float spacing=2.0f;
    for(int i=-20;i<=20;i++) {
        float offset=i*spacing;
        Color line=(i%5==0)?major:minor;
        DrawLine3D({center.x-half,0.012f,center.z+offset},
                   {center.x+half,0.012f,center.z+offset},line);
        DrawLine3D({center.x+offset,0.012f,center.z-half},
                   {center.x+offset,0.012f,center.z+half},line);
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
    float impact=std::clamp(p.landingTime/0.28f,0.0f,1.0f);
    float crouch=impact*(p.landingRoll?0.16f:0.23f);
    float breath=bob-crouch;

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

    float legLift=(p.vaulting||p.ledgeGrabbing||p.climbing)?0.26f:(p.grounded?0.0f:0.12f)-impact*0.12f;
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
    float attackPhase=p.attackTime>0?std::sin((1.0f-p.attackTime/0.22f)*PI):0.0f;
    float strikeReach=attackPhase*0.72f;
    float armSwing=p.blocking?0.25f:(p.landingRoll?0.42f:-swing*0.30f);
    Vector3 leftElbow=BodyPoint(p.position,right,forward,-0.39f,(p.vaulting||p.ledgeGrabbing||p.climbing)?1.48f:(1.06f+breath),(p.vaulting||p.ledgeGrabbing||p.climbing)?0.22f:(p.blocking?0.34f:armSwing)+strikeReach*0.2f);
    Vector3 leftHand=BodyPoint(p.position,right,forward,-0.43f,(p.vaulting||p.ledgeGrabbing||p.climbing)?1.68f:(0.91f+breath),(p.vaulting||p.ledgeGrabbing||p.climbing)?0.34f:(p.blocking?0.52f:armSwing*1.35f)+strikeReach*0.25f);
    Vector3 rightElbow=BodyPoint(p.position,right,forward,0.39f,(p.vaulting||p.ledgeGrabbing||p.climbing)?1.48f:(1.08f+breath),(p.vaulting||p.ledgeGrabbing||p.climbing)?0.22f:(p.bowAiming?0.28f:(p.blocking?0.34f:0.12f+swing*0.16f))+strikeReach*0.45f);
    Vector3 rightHand=BodyPoint(p.position,right,forward,0.43f,(p.vaulting||p.ledgeGrabbing||p.climbing)?1.68f:(0.94f+breath),(p.vaulting||p.ledgeGrabbing||p.climbing)?0.34f:(p.bowAiming?0.48f:(p.blocking?0.52f:0.31f+swing*0.18f))+strikeReach);
    Color armColor=cloth;
    DrawLimb(leftShoulder,leftElbow,0.13f,0.095f,armColor);
    DrawLimb(leftElbow,leftHand,0.095f,0.065f,armColor);
    DrawLimb(rightShoulder,rightElbow,0.13f,0.095f,armColor);
    DrawLimb(rightElbow,rightHand,0.095f,0.065f,armColor);
    DrawSphere(leftShoulder,0.16f,p.playerClass==PlayerClass::Skirmisher?cloth:metal);
    DrawSphere(rightShoulder,0.16f,p.playerClass==PlayerClass::Skirmisher?cloth:metal);
    DrawSphere(leftHand,0.075f,{194,158,126,255});
    DrawSphere(rightHand,0.075f,{194,158,126,255});

    // Class equipment is placed on the back so each silhouette remains readable
    // from the normal follow-camera angle without changing movement or hitboxes.
    if (p.playerClass==PlayerClass::Vanguard) {
        for (int side=-1;side<=1;side+=2) {
            DrawCubeV(BodyPoint(p.position,right,forward,side*0.31f,1.36f+breath,-0.08f),
                      {0.34f,0.20f,0.34f},{105,82,69,255});
            DrawCubeV(BodyPoint(p.position,right,forward,side*0.31f,1.43f+breath,-0.08f),
                      {0.26f,0.06f,0.28f},{163,132,91,255});
        }
        DrawCubeV(BodyPoint(p.position,right,forward,0,1.02f+breath,-0.22f),
                  {0.48f,0.68f,0.10f},{104,48,43,255});
        DrawCubeV(BodyPoint(p.position,right,forward,0,1.05f+breath,-0.28f),
                  {0.07f,0.48f,0.025f},{190,145,91,255});
        DrawCubeV(BodyPoint(p.position,right,forward,0,2.04f+breath,-0.015f),
                  {0.10f,0.22f,0.25f},{190,145,91,255});
    } else if (p.playerClass==PlayerClass::Skirmisher) {
        Vector3 quiver=BodyPoint(p.position,right,forward,-0.23f,1.18f+breath,-0.23f);
        DrawCylinder(quiver,0.11f,0.15f,0.76f,8,{91,61,43,255});
        DrawCylinder(BodyPoint(p.position,right,forward,-0.23f,1.57f+breath,-0.23f),
                     0.15f,0.15f,0.08f,8,{150,111,67,255});
        DrawCubeV(BodyPoint(p.position,right,forward,0,1.30f+breath,-0.20f),
                  {0.48f,0.08f,0.055f},{121,91,59,255});
        for (int i=-1;i<=1;i++) {
            Vector3 arrow=BodyPoint(p.position,right,forward,-0.23f+i*0.055f,1.53f+breath,-0.24f);
            Vector3 arrowTip=Vector3Add(arrow,Vector3{0,0.20f,0});
            DrawCylinderEx(arrow,arrowTip,0.018f,0.009f,5,{190,177,145,255});
            DrawCubeV(Vector3Add(arrow,{0,-0.035f,-0.015f}),{0.045f,0.07f,0.025f},{164,91,65,255});
        }
        DrawCubeV(BodyPoint(p.position,right,forward,0.30f,1.19f+breath,-0.12f),
                  {0.12f,0.42f,0.10f},{45,76,61,255});
    } else {
        DrawCubeV(BodyPoint(p.position,right,forward,0,1.08f+breath,-0.22f),
                  {0.54f,0.78f,0.14f},{43,53,75,255});
        DrawCubeV(BodyPoint(p.position,right,forward,0,1.08f+breath,-0.30f),
                  {0.40f,0.64f,0.035f},{75,88,111,255});
        DrawCubeV(BodyPoint(p.position,right,forward,0,1.08f+breath,-0.325f),
                  {0.07f,0.53f,0.025f},{190,157,99,255});
        DrawCubeV(BodyPoint(p.position,right,forward,0,1.08f+breath,-0.325f),
                  {0.34f,0.07f,0.025f},{190,157,99,255});
        DrawCubeV(BodyPoint(p.position,right,forward,0,1.47f+breath,-0.10f),
                  {0.42f,0.12f,0.12f},{103,116,139,255});
    }
    return rightHand;
}

static void DrawEnemyModel(const Enemy& e,Vector3 playerPosition,float time) {
    Vector3 forward=EnemyFacing(e,playerPosition), right={forward.z,0,-forward.x};
    static const Color bossHide[5]={{82,45,53,255},{91,66,49,255},{58,68,83,255},{105,46,48,255},{50,77,78,255}};
    static const Color bossArmor[5]={{91,91,105,255},{145,111,67,255},{105,119,133,255},{153,79,72,255},{66,113,111,255}};
    int variant=std::clamp(e.variant,0,4);
    Color hide=e.boss?bossHide[variant]:Color{105,62,52,255};
    Color armor=e.boss?bossArmor[variant]:Color{72,76,78,255};
    Color skin=e.boss?Color{160,111,87,255}:Color{169,128,103,255};
    if(e.hitTime>0) { hide=Color{255,196,120,255}; armor=Color{255,224,174,255}; skin=WHITE; }
    float scale=e.boss?1.22f:0.82f;
    float swing=std::sin(time*5.0f+e.position.x)*0.18f;
    if(!e.boss&&e.attackWindup>0) swing=0.55f;
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
    if(e.hitTime>0) DrawSphere(BodyPoint(origin,right,forward,0,1.25f*scale,0.28f*scale),0.13f*scale,GOLD);
    if(e.boss) {
        if(variant==1) {
            for(int side=-1;side<=1;side+=2)
                DrawCylinderEx(BodyPoint(origin,right,forward,side*0.17f*scale,1.78f*scale,0),
                               BodyPoint(origin,right,forward,side*0.28f*scale,2.08f*scale,0.02f),0.09f*scale,0.015f,6,GOLD);
        } else if(variant==2) {
            DrawCubeV(BodyPoint(origin,right,forward,0,1.22f*scale,-0.18f),{0.78f*scale,0.84f*scale,0.18f*scale},armor);
        } else if(variant==4) {
            DrawCubeV(BodyPoint(origin,right,forward,0,0.96f*scale,-0.22f),{0.62f*scale,1.15f*scale,0.12f*scale},hide);
        } else if(variant==3) {
            for(int side=-1;side<=1;side++)
                DrawCylinderEx(BodyPoint(origin,right,forward,side*0.16f*scale,1.94f*scale,0),
                               BodyPoint(origin,right,forward,side*0.16f*scale,2.2f*scale,0),0.07f*scale,0.0f,6,GOLD);
        }
        // Give each city's boss a readable weapon or silhouette at gameplay distance.
        if(variant==0) {
            Vector3 grip=BodyPoint(origin,right,forward,-0.48f*scale,0.76f*scale,0.12f);
            Vector3 anchor=Vector3Add(grip,{0.0f,-0.72f*scale,0.0f});
            DrawCylinderEx(grip,anchor,0.075f*scale,0.06f*scale,7,BROWN);
            DrawCubeV(Vector3Add(anchor,{0.0f,-0.18f*scale,0.0f}),{0.48f*scale,0.30f*scale,0.18f*scale},LIGHTGRAY);
            DrawCubeV(Vector3Add(anchor,{-0.22f*scale,0.02f*scale,0.0f}),{0.12f*scale,0.38f*scale,0.18f*scale},LIGHTGRAY);
        } else if(variant==2) {
            Vector3 grip=BodyPoint(origin,right,forward,0.48f*scale,0.78f*scale,0.08f);
            Vector3 head=Vector3Add(grip,{0.0f,0.82f*scale,0.0f});
            DrawCylinderEx(grip,head,0.075f*scale,0.06f*scale,7,{76,61,49,255});
            DrawCubeV(head,{0.58f*scale,0.52f*scale,0.48f*scale},{112,124,132,255});
            DrawCubeV(Vector3Add(head,{0.0f,0.0f,0.25f*scale}),{0.28f*scale,0.28f*scale,0.10f*scale},ORANGE);
        } else if(variant==4) {
            for(int side=-1;side<=1;side+=2) {
                DrawCubeV(BodyPoint(origin,right,forward,side*0.38f*scale,1.35f*scale,-0.30f*scale),
                          {0.34f*scale,0.92f*scale,0.16f*scale},armor);
                DrawCylinderEx(BodyPoint(origin,right,forward,side*0.42f*scale,1.72f*scale,-0.28f*scale),
                               BodyPoint(origin,right,forward,side*0.58f*scale,2.12f*scale,-0.32f*scale),
                               0.075f*scale,0.0f,6,GOLD);
            }
        } else if(variant==3) {
            DrawSphere(BodyPoint(origin,right,forward,0,1.18f*scale,0.28f*scale),0.14f*scale,GOLD);
            DrawCubeV(BodyPoint(origin,right,forward,0,0.98f*scale,0.23f*scale),
                      {0.40f*scale,0.08f*scale,0.08f*scale},GOLD);
        }
        DrawSphere(BodyPoint(origin,right,forward,0,1.95f*scale,0),0.10f*scale,e.phase==BossPhase::Windup?ORANGE:RED);
    } else {
        DrawSphere(BodyPoint(origin,right,forward,0,1.38f*scale,0.25f*scale),0.055f*scale,RED);
    }
}

void DrawGame(const GameState& g,const Camera3D& camera) {
    static const Color sky[5]={{32,52,68,255},{53,52,69,255},{33,43,54,255},{51,36,46,255},{55,49,77,255}};
    static const Color horizon[5]={{91,119,126,255},{119,93,87,255},{91,102,107,255},{125,77,77,255},{112,103,137,255}};
    int screenW=GetScreenWidth(),screenH=GetScreenHeight();
    ClearBackground(sky[g.city]);
    DrawRectangleGradientV(0,0,screenW,screenH,sky[g.city],horizon[g.city]);
    Color sunTint=g.city==2?Color{255,142,78,255}:Color{255,205,143,255};
    DrawCircleGradient((int)(screenW*0.79f),(int)(screenH*0.19f),66,Fade(sunTint,0.34f),Fade(sunTint,0.0f));
    BeginMode3D(camera);
    DrawPlane({0,0,0},{120,120},{45,50,57,255});
    for(const auto& p:g.platforms) {
        // The world-floor collision slab is represented visually by DrawPlane above.
        // Keep the slab in GameState for collision, but avoid drawing its duplicate cube/wireframe.
        const bool worldFloor=p.size.x>=100.0f&&p.size.z>=100.0f&&p.size.y<=1.0f&&
                             std::fabs(p.position.y+p.size.y*0.5f)<0.01f;
        if(worldFloor) continue;
        DrawCubeV(p.position,p.size,p.color);
        DrawCubeWiresV(p.position,p.size,{28,29,36,255});
        DrawBuildingDetails(p,g.city,g.player.position);
        DrawRooftopProps(p,g.city);
    }
    static const Color skylineColor[5]={{34,57,64,255},{70,52,48,255},{36,48,58,255},{65,39,45,255},{56,48,76,255}};
    for(int i=0;i<32;i++) {
        float a=i*0.19635f, r=47.0f+(i%4)*2.0f;
        float width=3.8f+(i%3)*0.75f;
        float height=7.0f+(i%6)*3.0f+g.city*0.65f;
        Vector3 p={std::cos(a)*r,height*0.5f-0.5f,std::sin(a)*r};
        DrawCubeV(p,{width,height,width},skylineColor[g.city]);
        float roof=p.y+height*0.5f;
        if(g.city==0) {
            DrawCubeV({p.x,roof+0.12f,p.z},{width+0.18f,0.24f,width+0.18f},{75,111,116,255});
        } else if(g.city==1) {
            DrawCubeV({p.x,roof+0.18f,p.z},{width+0.5f,0.16f,width+0.5f},{157,103,68,255});
        } else if(g.city==2) {
            DrawCylinder({p.x,roof+0.8f,p.z},0.34f,0.48f,1.6f,7,{81,91,99,255});
        } else if(g.city==4) {
            DrawCylinder({p.x,roof+0.7f,p.z},0.0f,width*0.34f,1.4f,7,{128,112,159,255});
        } else {
            for(int side=-1;side<=1;side+=2)
                DrawCubeV({p.x+side*width*0.28f,roof+0.18f,p.z},
                          {0.42f,0.36f,width*0.7f},{132,83,77,255});
        }
    }
    const Vector3 objectiveTarget=QuestTarget(g.city,g.quest);
    const bool survivalObjective=g.quest<3&&QuestRequiresSurvival(g.city,g.quest);
    if(!survivalObjective) {
        Vector3 marker=Vector3Add(objectiveTarget,{0,g.quest<3?1.2f:1.45f,0});
        const Color objectiveColor=g.quest<3&&QuestRequiresHold(g.city,g.quest)?Color{86,190,211,255}:(g.quest<3?GOLD:RED);
        DrawCylinder(marker,0.38f,0.08f,2.1f,8,objectiveColor);
        DrawSphere(Vector3Add(marker,{0,1.2f,0}),0.5f,g.quest<3?objectiveColor:MAROON);
    }

    if(g.skillTargeting) {
        Vector3 marker=Vector3Add(g.skillTarget,{0,0.06f,0});
        DrawCylinder(marker,3.7f,3.7f,0.08f,32,Fade(ORANGE,0.24f));
        DrawCylinderWires(marker,3.7f,3.7f,0.08f,32,Color{255,190,92,255});
        DrawLine3D(Vector3Add(g.player.position,{0,0.25f,0}),
                   Vector3Add(g.skillTarget,{0,0.1f,0}),Color{255,213,130,220});
    }
    const float skillVisualTime=(float)GetTime();
    for(const SkillEffect& effect:g.skillEffects) {
        const Color warning=effect.delay>0.0f?Color{255,161,73,255}:Color{255,205,107,255};
        Vector3 ring=Vector3Add(effect.position,{0,0.045f,0});
        DrawCylinder(ring,effect.radius,effect.radius,0.08f,28,Fade(warning,effect.delay>0.0f?0.18f:0.22f));
        DrawCylinderWires(ring,effect.radius,effect.radius,0.08f,28,Fade(warning,0.85f));
        if(effect.delay<=0.0f) {
            for(int arrow=0;arrow<8;arrow++) {
                const float angle=arrow*0.785398163f+skillVisualTime*0.55f;
                const float radius=effect.radius*0.72f;
                const float x=effect.position.x+std::cos(angle)*radius;
                const float z=effect.position.z+std::sin(angle)*radius;
                const float fall=std::fmod(skillVisualTime*5.5f+arrow*0.31f,2.3f);
                Vector3 tip{x,effect.position.y+2.5f-fall,z};
                Vector3 bottom{x,effect.position.y+0.25f,z};
                DrawCylinderEx(tip,bottom,0.035f,0.012f,5,GOLD);
            }
        }
    }

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
    DrawCylinder({player.position.x,player.position.y+0.025f,player.position.z},0.38f,0.38f,0.025f,12,Fade(BLACK,0.24f));
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
        if(!e.boss&&e.attackWindup>0) {
            float pulse=0.9f+0.1f*std::sin(animationClock*8.0f);
            DrawCircle3D({e.position.x,e.position.y+0.05f,e.position.z},1.45f*pulse,
                         {1,0,0},48,Color{255,150,55,85});
        }
        if(e.boss&&e.hp<=e.maxHp*0.5f) {
            float pulse=0.9f+0.1f*std::sin(animationClock*5.0f);
            DrawCircle3D({e.position.x,e.position.y+0.05f,e.position.z},0.88f*pulse,
                         {1,0,0},64,Color{255,82,38,155});
        }
        if(e.boss&&(e.phase==BossPhase::Windup||e.phase==BossPhase::Strike)) {
            Color warning=e.phase==BossPhase::Windup?Color{255,166,56,65}:Color{238,62,52,80};
            int variant=std::clamp(e.variant,0,4);
            const float warningRadius=BossStrikeRange(variant);
            if(variant==0||variant==1||variant==4) {
                Vector3 direction=e.strikeDirection; direction.y=0;
                float length=Vector3Length(direction);
                direction=length>0.001f?Vector3Scale(direction,1.0f/length):Vector3{0,0,1};
                Vector3 origin=e.strikeOrigin;
                Vector3 start={origin.x,origin.y+0.06f,origin.z};
                if(variant==0) {
                    Vector3 right={direction.z,0,-direction.x};
                    Vector3 end=Vector3Add(start,Vector3Scale(direction,warningRadius));
                    Vector3 side=Vector3Scale(right,BossStrikeHalfWidth(variant,warningRadius));
                    DrawCylinderEx(start,end,0.13f,0.13f,8,warning);
                    DrawCylinderEx(start,Vector3Add(end,side),0.055f,0.055f,6,warning);
                    DrawCylinderEx(start,Vector3Subtract(end,side),0.055f,0.055f,6,warning);
                } else {
                    Vector3 end=Vector3Add(start,Vector3Scale(direction,warningRadius));
                    float width=BossStrikeHalfWidth(variant,warningRadius);
                    DrawCylinderEx(start,end,width,width,8,warning);
                }
            } else {
                DrawCircle3D({e.position.x,e.position.y+0.04f,e.position.z},warningRadius,{1,0,0},90,warning);
            }
        }
        DrawCylinder({e.position.x,e.position.y+0.025f,e.position.z},e.boss?0.62f:0.42f,e.boss?0.62f:0.42f,0.025f,12,Fade(BLACK,0.24f));
        DrawEnemyModel(e,player.position,animationClock);
        float width=e.boss?2.0f:1.0f;
        DrawCubeV({e.position.x,e.position.y+2.3f,e.position.z},{width,0.09f,0.1f},DARKGRAY);
        DrawCubeV({e.position.x-width*0.5f*(1-e.hp/e.maxHp),e.position.y+2.3f,e.position.z},{width*(e.hp/e.maxHp),0.1f,0.12f},RED);
    }
    for(const auto& s:g.projectiles) DrawSphere(s.position,0.12f,GOLD);
    DrawCityLandmarks(g.city);
    DrawGroundGrid(player.position,g.city);
    EndMode3D();

    int w=GetScreenWidth(),h=GetScreenHeight();
    DrawRectangle(12,12,270,98,Fade(BLACK,0.65f));
    DrawText(TextFormat("HEALTH  %d",(int)g.player.health),24,19,14,WHITE);
    DrawRectangle(128,23,131,7,DARKGRAY);
    DrawRectangle(128,23,(int)(131*std::clamp(g.player.health/PlayerMaxHealth(g.player),0.0f,1.0f)),7,RED);
    DrawText(TextFormat("STAMINA  %d",(int)g.player.stamina),24,39,13,WHITE);
    DrawRectangle(128,43,131,7,DARKGRAY);
    DrawRectangle(128,43,(int)(131*std::clamp(g.player.stamina/PlayerMaxStamina(g.player),0.0f,1.0f)),7,GREEN);
    DrawText(TextFormat("MANA  %d",(int)g.player.mana),24,58,13,WHITE);
    DrawRectangle(128,62,131,7,DARKGRAY);
    DrawRectangle(128,62,(int)(131*std::clamp(g.player.mana/PlayerMaxMana(g.player),0.0f,1.0f)),7,Color{89,137,218,255});
    if(g.player.escapeSkill) DrawText(g.player.dashCooldown<=0?"DASH READY":TextFormat("DASH %.1fs",g.player.dashCooldown),24,79,10,GOLD);
    else DrawText("DASH UNLOCKS AT LVL 3",24,79,10,GRAY);
    if(g.player.wallRunning) DrawText("WALL RUN",180,79,10,ORANGE);
    const ClassSkill equippedSkill=(ClassSkill)g.player.skills[std::clamp(g.player.activeSkillSlot,0,1)];
    if(equippedSkill==ClassSkill::None)
        DrawText("TECHNIQUE: CHOOSE AT LVL 3",24,96,9,GRAY);
    else if(g.player.techniqueCooldown>0.0f)
        DrawText(TextFormat("%s  %.1fs",ClassSkillName(equippedSkill),g.player.techniqueCooldown),24,96,9,ORANGE);
    else
        DrawText(TextFormat("%s  READY",ClassSkillName(equippedSkill)),24,96,9,GOLD);
    if(g.player.aegisTime>0.0f) DrawText(TextFormat("AEGIS %.1fs",g.player.aegisTime),185,96,9,Color{133,197,235,255});
    if(!g.finished&&!survivalObjective) {
        Vector3 toward=Vector3Subtract(objectiveTarget,g.player.position);
        toward.y=0;
        Vector3 facing=g.player.facing;
        facing.y=0;
        if(Vector3Length(facing)<0.001f) facing={0,0,1};
        facing=Vector3Normalize(facing);
        if(Vector3Length(toward)>0.001f) {
            toward=Vector3Normalize(toward);
            Vector3 right={facing.z,0,-facing.x};
            float angle=std::atan2(Vector3DotProduct(toward,right),Vector3DotProduct(toward,facing));
            float s=std::sin(angle),c=std::cos(angle);
            int cx=w/2,cy=43;
            DrawRectangle(cx-56,12,112,52,Fade(BLACK,0.68f));
            DrawText("OBJECTIVE",cx-34,17,10,LIGHTGRAY);
            Vector2 tip={(float)cx+s*15.0f,(float)cy-c*15.0f};
            Vector2 back={(float)cx-s*8.0f,(float)cy+c*8.0f};
            Vector2 side={c*6.0f,s*6.0f};
            DrawCircleLines(cx,cy,16,Fade(GOLD,0.48f));
            DrawTriangle(tip,{back.x+side.x,back.y+side.y},{back.x-side.x,back.y-side.y},GOLD);
        }
    }
    const auto activeBoss=std::find_if(g.enemies.begin(),g.enemies.end(),[](const Enemy& e) {
        return e.alive&&e.boss;
    });
    if(activeBoss!=g.enemies.end()) {
        bool enraged=activeBoss->hp<=activeBoss->maxHp*0.5f;
        if(activeBoss->phase==BossPhase::Windup) {
            float remaining=std::max(0.0f,BossWindupDuration(activeBoss->variant,enraged)-activeBoss->phaseTime);
            DrawRectangle(w/2-96,67,192,23,Fade(enraged?MAROON:BLACK,0.82f));
            DrawText(TextFormat("%s  STRIKE IN %.1fs",BossName(activeBoss->variant),remaining),
                     w/2-85,73,11,remaining<0.25f?RED:ORANGE);
        } else if(enraged) {
            DrawRectangle(w/2-96,67,192,23,Fade(MAROON,0.82f));
            DrawText(TextFormat("%s  ENRAGED",BossName(activeBoss->variant)),w/2-60,73,11,ORANGE);
        }
    }
    if(g.player.ledgeGrabbing) DrawText("HANGING - JUMP: CLIMB / DOWN: DROP",w/2-155,28,14,GOLD);
    else if(g.player.climbing) DrawText("CLIMBING...",w/2-55,28,14,GOLD);
    DrawRectangle(w-245,12,233,76,Fade(BLACK,0.65f));
    DrawText(TextFormat("LEVEL %d   XP %d/%d",g.player.level,g.player.xp,g.player.level*100),w-233,20,15,WHITE);
    DrawText(TextFormat("TAG DANGER   %d%%",(int)g.danger),w-233,44,15,g.danger>70?RED:ORANGE);
    DrawRectangle(w-233,67,205,8,DARKGRAY);
    DrawRectangle(w-233,67,(int)(205*g.danger/100),8,ORANGE);
    int objectiveHudY=h<=440?116:h-78;
    DrawRectangle(12,objectiveHudY,390,65,Fade(BLACK,0.7f));
    DrawText(TextFormat("%s  |  %s",CityName(g.city),WeaponName(g.player.weapon)),24,objectiveHudY+9,16,GOLD);
    const bool holdObjective=g.quest<3&&QuestRequiresHold(g.city,g.quest);
    DrawText(g.quest<3?
        TextFormat("QUEST %d/3  -  %s",g.quest+1,holdObjective?"HOLD THE BEACON":(survivalObjective?"SURVIVE THE PURSUIT":"REACH THE BEACON")):
        TextFormat("BOSS GATE OPEN  -  %s",BossName(g.city)),24,objectiveHudY+34,14,WHITE);
    int taggers=(int)std::count_if(g.enemies.begin(),g.enemies.end(),[](const Enemy& e){return e.alive&&!e.boss;});
    int targetDistance=(int)std::round(Vector3Distance(g.player.position,objectiveTarget));
    if(holdObjective) {
        DrawText(TextFormat("CAPTURE %.1f/%.1fs  |  TAGGERS %d",g.beaconHoldTime,QuestHoldDuration(g.city,g.quest),taggers),
                 24,objectiveHudY+54,12,Color{126,215,231,255});
    } else if(survivalObjective) {
        const float remaining=std::max(0.0f,QuestSurvivalDuration(g.city,g.quest)-g.beaconHoldTime);
        DrawText(TextFormat("SURVIVE %.1fs  |  TAGGERS %d",remaining,taggers),
                 24,objectiveHudY+54,12,Color{126,215,231,255});
    } else {
        DrawText(g.quest<3?TextFormat("BEACON %dm  |  TAGGERS %d  |  WAVE %ds",targetDistance,taggers,std::max(0,(int)std::ceil(g.questLimit-g.questTime))):TextFormat("BOSS %dm  |  TAGGERS %d",targetDistance,taggers),
                 24,objectiveHudY+54,12,LIGHTGRAY);
    }
    if(g.finished) {
        DrawRectangle(0,0,w,h,Fade(BLACK,0.7f));
        DrawText("THE FIVE CITIES ARE FREE",w/2-190,h/2-18,28,GOLD);
    }
    if(g.mapOpen) {
        const int mapWidth=std::min(340,w-32);
        const int mapHeight=std::min(296,h-24);
        const int panelX=(w-mapWidth)/2,panelY=(h-mapHeight)/2;
        const int floorOffset=g.mapFloor*ExplorationMapCells;
        DrawRectangle(0,0,w,h,Fade(BLACK,0.78f));
        DrawRectangle(panelX,panelY,mapWidth,mapHeight,Color{20,27,35,248});
        DrawRectangleLines(panelX,panelY,mapWidth,mapHeight,Color{177,140,92,230});
        DrawText(TextFormat("%s  /  %s",CityName(g.city),ExplorationFloorName(g.mapFloor)),
                 panelX+12,panelY+9,14,GOLD);
        const int mapX=panelX+12,mapY=panelY+34;
        const int mapW=mapWidth-24,mapH=mapHeight-58;
        const float cellW=(float)mapW/ExplorationMapColumns;
        const float cellH=(float)mapH/ExplorationMapRows;
        DrawRectangle(mapX,mapY,mapW,mapH,Color{8,13,19,255});
        for(int row=0;row<ExplorationMapRows;row++) {
            for(int column=0;column<ExplorationMapColumns;column++) {
                const int index=floorOffset+row*ExplorationMapColumns+column;
                if(!g.explored[index]) continue;
                const int x=mapX+(int)std::floor(column*cellW);
                const int y=mapY+(int)std::floor(row*cellH);
                const int right=mapX+(int)std::ceil((column+1)*cellW);
                const int bottom=mapY+(int)std::ceil((row+1)*cellH);
                DrawRectangle(x+1,y+1,std::max(1,right-x-2),std::max(1,bottom-y-2),
                              Color{83,112,112,255});
            }
        }
        for(const Platform& platform:g.platforms) {
            const bool worldFloor=platform.size.x>=100.0f&&platform.size.z>=100.0f&&platform.size.y<=1.0f;
            if(worldFloor|| (platform.size.x<1.1f&&platform.size.z<1.1f)) continue;
            const float platformTop=platform.position.y+platform.size.y*0.5f;
            if(platform.size.y<8.0f&&ExplorationFloorForPosition(platformTop)!=g.mapFloor) continue;
            const float left=platform.position.x-platform.size.x*0.5f;
            const float right=platform.position.x+platform.size.x*0.5f;
            const float top=platform.position.z-platform.size.z*0.5f;
            const float bottom=platform.position.z+platform.size.z*0.5f;
            for(int row=0;row<ExplorationMapRows;row++) {
                const float cellTop=-36.0f+row*5.0f,cellBottom=cellTop+5.0f;
                if(bottom<=cellTop||top>=cellBottom) continue;
                for(int column=0;column<ExplorationMapColumns;column++) {
                    const int index=floorOffset+row*ExplorationMapColumns+column;
                    if(!g.explored[index]) continue;
                    const float cellLeft=-50.0f+column*5.0f,cellRight=cellLeft+5.0f;
                    if(right<=cellLeft||left>=cellRight) continue;
                    const float clippedLeft=std::max(left,cellLeft);
                    const float clippedRight=std::min(right,cellRight);
                    const float clippedTop=std::max(top,cellTop);
                    const float clippedBottom=std::min(bottom,cellBottom);
                    const int x=mapX+(int)((clippedLeft+50.0f)/100.0f*mapW);
                    const int y=mapY+(int)((clippedTop+36.0f)/80.0f*mapH);
                    const int width=std::max(1,(int)std::ceil((clippedRight-clippedLeft)/100.0f*mapW));
                    const int height=std::max(1,(int)std::ceil((clippedBottom-clippedTop)/80.0f*mapH));
                    DrawRectangle(x+1,y+1,width-1,height-1,Color{116,137,133,255});
                }
            }
        }
        DrawRectangleLines(mapX,mapY,mapW,mapH,Color{135,151,151,180});
        const Vector3 target=QuestTarget(g.city,g.quest);
        const int objectiveCell=floorOffset+ExplorationCellForPosition(target);
        if(ExplorationFloorForPosition(target.y)==g.mapFloor&&g.explored[objectiveCell]) {
            const float markerX=mapX+(target.x+50.0f)/100.0f*mapW;
            const float markerY=mapY+(target.z+36.0f)/80.0f*mapH;
            DrawCircle((int)markerX,(int)markerY,4.0f,GOLD);
        }
        if(g.currentFloor==g.mapFloor) {
            const float playerX=mapX+(g.player.position.x+50.0f)/100.0f*mapW;
            const float playerY=mapY+(g.player.position.z+36.0f)/80.0f*mapH;
            DrawCircle((int)playerX,(int)playerY,5.0f,WHITE);
            Vector3 facing=g.player.facing;
            facing.y=0;
            if(Vector3Length(facing)>0.001f) {
                facing=Vector3Normalize(facing);
                DrawLineEx({playerX,playerY},
                           {playerX+facing.x*11.0f,playerY+facing.z*11.0f},2.0f,GOLD);
            }
        }
        DrawText("WHITE: YOU    GOLD: OBJECTIVE    M / MAP: CLOSE",
                 panelX+12,panelY+mapHeight-18,9,Color{210,205,190,255});
    }

}
