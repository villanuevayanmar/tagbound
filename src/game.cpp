#include "game.hpp"
#include <algorithm>
#include <cmath>

static const char* cityNames[5]={"PORT TOWN","MERCHANT TIER","CLOCKWORK FOUNDRY","HIGH SPIRE","IMPERIAL CAPITAL"};
const char* CityName(int c) { return cityNames[std::clamp(c,0,4)]; }
static const char* bossNames[5]={"HARBOR WARDEN","GILDED CHARGER","FOUNDRY COLOSSUS","SPIRE SENTINEL","IMPERIAL REGENT"};
const char* BossName(int c) { return bossNames[std::clamp(c,0,4)]; }
const char* WeaponName(Weapon w) {
    static const char* names[5]={"SWORD","AXE","SHIELD","BOW","SPEAR"};
    return names[std::clamp((int)w,0,4)];
}
Weapon ClassWeapon(PlayerClass c) {
    static const Weapon loadouts[3]={Weapon::Sword,Weapon::Bow,Weapon::Shield};
    return loadouts[std::clamp((int)c,0,2)];
}
float ClassMaxHealth(PlayerClass c) {
    static const float health[3]={125.0f,95.0f,115.0f};
    return health[std::clamp((int)c,0,2)];
}

bool FindReinforcementSpawn(const GameState& g,Vector3 origin,Vector3& spawn) {
    static const Vector2 directions[8]={
        {1.0f,0.0f},{0.70710678f,0.70710678f},{0.0f,1.0f},
        {-0.70710678f,0.70710678f},{-1.0f,0.0f},
        {-0.70710678f,-0.70710678f},{0.0f,-1.0f},
        {0.70710678f,-0.70710678f}
    };
    static const float rings[3]={8.0f,10.5f,13.0f};
    constexpr float radius=0.55f;
    constexpr float height=1.72f;
    for(float distance:rings) for(const Vector2 direction:directions) {
        Vector3 candidate={origin.x+direction.x*distance,0.3f,origin.z+direction.y*distance};
        if(candidate.x < -48.5f || candidate.x > 48.5f ||
           candidate.z < -34.5f || candidate.z > 41.5f) continue;
        bool blocked=false;
        for(const Platform& p:g.platforms) {
            float minX=p.position.x-p.size.x*0.5f, maxX=p.position.x+p.size.x*0.5f;
            float minY=p.position.y-p.size.y*0.5f, maxY=p.position.y+p.size.y*0.5f;
            float minZ=p.position.z-p.size.z*0.5f, maxZ=p.position.z+p.size.z*0.5f;
            bool x=candidate.x+radius>minX && candidate.x-radius<maxX;
            bool y=candidate.y+height>minY+0.05f && candidate.y<maxY-0.05f;
            bool z=candidate.z+radius>minZ && candidate.z-radius<maxZ;
            if(x&&y&&z) { blocked=true; break; }
        }
        if(blocked) continue;
        for(const Enemy& e:g.enemies) if(e.alive) {
            float dx=candidate.x-e.position.x, dz=candidate.z-e.position.z;
            if(dx*dx+dz*dz<1.7f*1.7f) { blocked=true; break; }
        }
        if(!blocked) { spawn=candidate; return true; }
    }
    return false;
}
void BuildCity(GameState& g) {
    g.platforms.clear(); g.enemies.clear(); g.projectiles.clear();
    g.quest=0; g.bossUnlocked=false; g.questTime=0; g.danger=0;
    g.questLimit=110.0f-g.city*7.0f;
    Color tint[5]={{73,132,135,255},{167,123,75,255},{91,119,132,255},{111,103,151,255},{143,76,69,255}};
    g.platforms.push_back({{0,-0.3f,0},{120,0.6f,120},{44,49,59,255}});
    g.platforms.push_back({{0,0.0f,0},{8,0.5f,8},{106,84,68,255}});
    g.platforms.push_back({{4,1.45f,6},{5,0.5f,5},tint[g.city]});
    g.platforms.push_back({{10,2.2f,13},{5,0.5f,5},tint[g.city]});
    g.platforms.push_back({{16,2.95f,20},{5,0.5f,5},tint[g.city]});
    g.platforms.push_back({{22,3.7f,27},{8,0.5f,8},{160,139,96,255}});
    // Decorative low-poly buildings, placed away from the quest route.
    for(int i=0;i<28;i++) {
        float a=i*0.2244f, r=18.0f+(i%5)*4.2f;
        float h=4.0f+(i*3%7)*1.45f+g.city*0.55f;
        Vector3 p={std::cos(a)*r,h*0.5f,std::sin(a)*r-3.0f};
        g.platforms.push_back({p,{4.5f,h,4.5f},Color{(unsigned char)std::min(235,(int)tint[g.city].r+(i%3)*8),(unsigned char)std::min(235,(int)tint[g.city].g+(i%4)*6),(unsigned char)std::min(235,(int)tint[g.city].b+(i%2)*9),255}});
    }
    for(int i=0;i<2+g.city*2;i++) {
        float a=6.28318f*i/(2+g.city*2);
        Vector3 p={std::cos(a)*(12.0f+g.city),0.0f,std::sin(a)*(12.0f+g.city)+3.0f};
        float toughness=42.0f+g.city*14.0f;
        g.enemies.push_back({p,toughness,toughness,2.4f+g.city*0.32f,0,false,true});
    }
    g.player.position={0,0.3f,0}; g.player.velocity={0,0,0};
    g.player.grounded=true; g.player.wallRunning=false; g.player.dashTime=0;
    g.player.vaulting=false; g.player.vaultTime=0; g.player.ledgeGrabbing=false; g.player.climbing=false;
    g.player.landingTime=0; g.player.landingRoll=false; g.player.attackTime=0;
    g.player.climbTime=0; g.player.ledgeNormal={}; g.player.ledgeHangPosition={}; g.player.ledgeTargetPosition={};
}

void SetQuestProgress(GameState& g,int quest) {
    g.quest=std::clamp(quest,0,3);
    if(g.quest<3) {
        g.bossUnlocked=false;
        if(g.quest>0) g.questLimit=100.0f-g.city*8.0f-g.quest*9.0f;
        return;
    }
    for(const auto& e:g.enemies) if(e.boss) {
        g.bossUnlocked=true;
        return;
    }
    float hp=280.0f+g.city*120.0f;
    g.enemies.push_back({{22,3.95f,27},hp,hp,2.4f+g.city*0.38f,0,true,true});
    g.enemies.back().variant=g.city;
    g.bossUnlocked=true;
}
