#include "game.hpp"
#include <algorithm>
#include <cmath>

static const char* cityNames[5]={"PORT TOWN","MERCHANT TIER","CLOCKWORK FOUNDRY","IMPERIAL CAPITAL","HIGH SPIRE"};
const char* CityName(int c) { return cityNames[std::clamp(c,0,4)]; }
int MigrateLegacyCityIndex(int city) {
    if(city==3) return 4; // Legacy stage 3 was High Spire.
    if(city==4) return 3; // Legacy stage 4 was Imperial Capital.
    return city;
}
static const char* bossNames[5]={"HARBOR WARDEN","GILDED CHARGER","FOUNDRY COLOSSUS","IMPERIAL REGENT","SPIRE SENTINEL"};
const char* BossName(int c) { return bossNames[std::clamp(c,0,4)]; }
Vector3 QuestTarget(int city,int quest) {
    static const Vector3 routes[5][4]={
        {{4,1.7f,6},{10,2.45f,13},{14,3.2f,19},{18,3.95f,25}},
        {{-4,1.7f,6},{-10,2.45f,13},{-14,3.2f,19},{-18,3.95f,25}},
        {{5,1.7f,3},{12,2.45f,8},{10,3.2f,16},{5,3.95f,24}},
        {{0,1.7f,7},{7,2.45f,14},{0,3.2f,21},{0,3.95f,29}},
        {{-5,1.7f,3},{-12,2.45f,8},{-10,3.2f,16},{-5,3.95f,24}}
    };
    return routes[std::clamp(city,0,4)][std::clamp(quest,0,3)];
}

bool ReachedQuestBeacon(Vector3 position,int city,int quest) {
    if(quest<0||quest>=3) return false;
    Vector3 target=QuestTarget(city,quest);
    float dx=position.x-target.x, dz=position.z-target.z;
    return dx*dx+dz*dz<=3.4f*3.4f && std::abs(position.y-target.y)<=0.45f;
}
bool QuestRequiresHold(int city,int quest) {
    static const bool holds[5][3]={
        {false,true,false},
        {true,false,false},
        {false,false,true},
        {true,false,true},
        {false,true,false}
    };
    return city>=0&&city<5&&quest>=0&&quest<3?holds[city][quest]:false;
}
float QuestHoldDuration(int city,int quest) {
    static const float durations[5][3]={
        {0.0f,2.5f,0.0f},
        {2.75f,0.0f,0.0f},
        {0.0f,0.0f,3.25f},
        {3.75f,0.0f,4.0f},
        {0.0f,3.5f,0.0f}
    };
    return city>=0&&city<5&&quest>=0&&quest<3?durations[city][quest]:0.0f;
}
bool QuestRequiresSurvival(int city,int quest) {
    static const bool survival[5][3]={
        {false,false,false},
        {false,true,false},
        {false,true,false},
        {false,false,false},
        {true,false,false}
    };
    return city>=0&&city<5&&quest>=0&&quest<3?survival[city][quest]:false;
}
float QuestSurvivalDuration(int city,int quest) {
    static const float durations[5][3]={
        {0.0f,0.0f,0.0f},
        {0.0f,18.0f,0.0f},
        {0.0f,20.0f,0.0f},
        {0.0f,0.0f,0.0f},
        {22.0f,0.0f,0.0f}
    };
    return city>=0&&city<5&&quest>=0&&quest<3?durations[city][quest]:0.0f;
}
bool UpdateQuestObjective(GameState& g,float dt) {
    if(g.quest<0||g.quest>=3) {
        g.beaconHoldTime=0.0f;
        return false;
    }
    if(QuestRequiresSurvival(g.city,g.quest)) {
        const float duration=QuestSurvivalDuration(g.city,g.quest);
        g.beaconHoldTime=std::min(duration,g.beaconHoldTime+std::max(0.0f,dt));
        if(g.beaconHoldTime+0.0001f>=duration) {
            g.beaconHoldTime=0.0f;
            return true;
        }
        return false;
    }
    if(!QuestRequiresHold(g.city,g.quest)) {
        g.beaconHoldTime=0.0f;
        return ReachedQuestBeacon(g.player.position,g.city,g.quest);
    }
    if(!ReachedQuestBeacon(g.player.position,g.city,g.quest)) {
        g.beaconHoldTime=0.0f;
        return false;
    }
    const float duration=QuestHoldDuration(g.city,g.quest);
    g.beaconHoldTime=std::min(duration,g.beaconHoldTime+std::max(0.0f,dt));
    if(g.beaconHoldTime+0.0001f>=duration) {
        g.beaconHoldTime=0.0f;
        return true;
    }
    return false;
}
int ExplorationCellForPosition(Vector3 position) {
    const int column=std::clamp((int)std::floor((position.x+50.0f)/5.0f),0,ExplorationMapColumns-1);
    const int row=std::clamp((int)std::floor((position.z+36.0f)/5.0f),0,ExplorationMapRows-1);
    return row*ExplorationMapColumns+column;
}
int ExplorationFloorForPosition(float height) {
    static constexpr float floorHeights[ExplorationMapFloors]={0.3f,1.7f,2.45f,3.2f,3.95f};
    int nearest=0;
    float closest=std::fabs(height-floorHeights[0]);
    for(int floor=1;floor<ExplorationMapFloors;floor++) {
        const float distance=std::fabs(height-floorHeights[floor]);
        if(distance<closest) { closest=distance; nearest=floor; }
    }
    return nearest;
}
bool UpdateExploration(GameState& g,Vector3 position) {
    if(!g.player.grounded) return false;
    constexpr float revealRadius=7.5f;
    constexpr float minX=-50.0f,minZ=-36.0f,cellSize=5.0f;
    const int floor=ExplorationFloorForPosition(position.y);
    g.currentFloor=floor;
    if(g.mapFloor!=floor) g.mapFloor=floor;
    const int floorOffset=floor*ExplorationMapCells;
    const int playerCell=floorOffset+ExplorationCellForPosition(position);
    bool changed=false;
    if(!g.explored[playerCell]) { g.explored[playerCell]=1; changed=true; }
    for(int row=0;row<ExplorationMapRows;row++) {
        const float z=minZ+(row+0.5f)*cellSize;
        for(int column=0;column<ExplorationMapColumns;column++) {
            const float x=minX+(column+0.5f)*cellSize;
            const float dx=x-position.x,dz=z-position.z;
            if(dx*dx+dz*dz>revealRadius*revealRadius) continue;
            const int index=floorOffset+row*ExplorationMapColumns+column;
            if(!g.explored[index]) { g.explored[index]=1; changed=true; }
        }
    }
    return changed;
}
bool CanChangeExplorationFloor(const GameState& g,int direction) {
    if(direction==0) return false;
    const int step=direction>0?1:-1;
    for(int floor=g.mapFloor+step;floor>=0&&floor<ExplorationMapFloors;floor+=step) {
        const int start=floor*ExplorationMapCells;
        if(std::find(g.explored.begin()+start,g.explored.begin()+start+ExplorationMapCells,
                     (unsigned char)1)!=g.explored.begin()+start+ExplorationMapCells) return true;
    }
    return false;
}
bool ChangeExplorationFloor(GameState& g,int direction) {
    if(!CanChangeExplorationFloor(g,direction)) return false;
    const int step=direction>0?1:-1;
    for(int floor=g.mapFloor+step;floor>=0&&floor<ExplorationMapFloors;floor+=step) {
        const int start=floor*ExplorationMapCells;
        if(std::find(g.explored.begin()+start,g.explored.begin()+start+ExplorationMapCells,
                     (unsigned char)1)!=g.explored.begin()+start+ExplorationMapCells) {
            g.mapFloor=floor;
            return true;
        }
    }
    return false;
}

Vector3 QuestCheckpoint(int city,int quest) {
    return quest>0?QuestTarget(city,quest-1):Vector3{0,0.3f,0};
}
Vector3 QuestCheckpointFacing(int city,int quest) {
    Vector3 from=QuestCheckpoint(city,quest);
    Vector3 to=QuestTarget(city,quest);
    float dx=to.x-from.x, dz=to.z-from.z;
    float length=std::sqrt(dx*dx+dz*dz);
    return length>0.001f?Vector3{dx/length,0,dz/length}:Vector3{0,0,1};
}

float BossWindupDuration(int variant,bool enraged) {
    static const float duration[5]={0.65f,0.45f,0.9f,0.5f,0.7f};
    return duration[std::clamp(variant,0,4)]*(enraged?0.85f:1.0f);
}
float BossStrikeRange(int variant) {
    static const float range[5]={4.0f,2.3f,5.5f,4.5f,8.0f};
    return range[std::clamp(variant,0,4)];
}
bool BossArenaContains(const GameState& g,const Enemy& e) {
    if(!e.boss||!g.bossArena.active) return false;
    constexpr float radius=0.62f;
    const BossArena& arena=g.bossArena;
    return e.position.x>=arena.center.x-arena.halfExtents.x+radius-0.001f&&
           e.position.x<=arena.center.x+arena.halfExtents.x-radius+0.001f&&
           e.position.z>=arena.center.z-arena.halfExtents.y+radius-0.001f&&
           e.position.z<=arena.center.z+arena.halfExtents.y-radius+0.001f&&
           std::abs(e.position.y-arena.floorY)<=0.025f;
}
float BossStrikeHalfWidth(int variant,float forward) {
    switch(std::clamp(variant,0,4)) {
        case 0: return std::max(0.0f,forward)*0.38f;
        case 1:
        case 4: return 0.9f;
        default: return 0.0f;
    }
}
Vector3 EnemyFacing(const Enemy& enemy,Vector3 target) {
    Vector3 direction{};
    if(enemy.boss&&(enemy.phase==BossPhase::Windup||enemy.phase==BossPhase::Strike)&&
       enemy.strikeDirection.x*enemy.strikeDirection.x+
       enemy.strikeDirection.z*enemy.strikeDirection.z>0.000001f) {
        direction=enemy.strikeDirection;
    } else {
        direction={target.x-enemy.position.x,0,target.z-enemy.position.z};
    }
    const float length=std::sqrt(direction.x*direction.x+direction.z*direction.z);
    if(length<=0.001f) return {0,0,-1};
    return {direction.x/length,0,direction.z/length};
}
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
    g.quest=0; g.bossUnlocked=false; g.questTime=0; g.beaconHoldTime=0; g.danger=0;
    g.questLimit=110.0f-g.city*7.0f;
    Color tint[5]={{73,132,135,255},{167,123,75,255},{91,119,132,255},{143,76,69,255},{111,103,151,255}};
    g.platforms.push_back({{0,-0.3f,0},{120,0.6f,120},{44,49,59,255}});
    g.platforms.push_back({{0,0.0f,0},{8,0.5f,8},{106,84,68,255}});
    for(int quest=0;quest<3;quest++) {
        Vector3 target=QuestTarget(g.city,quest);
        g.platforms.push_back({{target.x,target.y-0.25f,target.z},{5,0.5f,5},tint[g.city]});
    }
    Vector3 gate=QuestTarget(g.city,3);
    g.platforms.push_back({{gate.x,gate.y-0.25f,gate.z},{8,0.5f,8},{160,139,96,255}});
    const Platform& gateFloor=g.platforms.back();
    const Vector3 gateFloorTop={
        gateFloor.position.x,
        gateFloor.position.y+gateFloor.size.y*0.5f,
        gateFloor.position.z
    };
    const float arenaHalfWidth=gateFloor.size.x*0.5f;
    const float arenaHalfDepth=gateFloor.size.z*0.5f;
    constexpr float arenaWallThickness=0.6f;
    constexpr float arenaWallHeight=2.8f;
    g.bossArena={
        gateFloorTop,
        {arenaHalfWidth-arenaWallThickness,arenaHalfDepth-arenaWallThickness},
        gateFloorTop.y,
        false
    };
    // Keep skyline buildings outside the central rooftop routes.
    for(int i=0;i<28;i++) {
        float a=i*0.2244f+g.city*0.08f, r=38.0f+(i%3)*1.0f;
        float h=4.0f+(i*3%7)*1.45f+g.city*0.55f;
        Vector3 p={std::cos(a)*r,h*0.5f,std::sin(a)*r-3.0f};
        // Keep the approach behind the gate clear; move only obstructing skyline
        // pieces beyond the playable perimeter instead of dropping city geometry.
        if(p.z>30.0f&&std::abs(p.x-gate.x)<7.5f) p.z=46.0f;
        g.platforms.push_back({p,{4.5f,h,4.5f},Color{(unsigned char)std::min(235,(int)tint[g.city].r+(i%3)*8),(unsigned char)std::min(235,(int)tint[g.city].g+(i%4)*6),(unsigned char)std::min(235,(int)tint[g.city].b+(i%2)*9),255}});
    }
    // Fortify each city with a continuous stone perimeter. The north gate opening
    // is aligned with this city's boss platform so the route and architecture agree.
    const Color wallStone={78,74,67,255};
    const Color gateStone={128,105,73,255};
    const float wallX=44.0f, wallMinZ=-31.0f, wallMaxZ=38.0f;
    const float wallHeight=12.0f, wallThickness=2.0f;
    const float wallY=wallHeight*0.5f;
    const float gateHalfWidth=5.0f;
    const float gateLeft=gate.x-gateHalfWidth;
    const float gateRight=gate.x+gateHalfWidth;
    g.platforms.push_back({{-wallX,wallY,(wallMinZ+wallMaxZ)*0.5f},
                           {wallThickness,wallHeight,wallMaxZ-wallMinZ},wallStone});
    g.platforms.push_back({{wallX,wallY,(wallMinZ+wallMaxZ)*0.5f},
                           {wallThickness,wallHeight,wallMaxZ-wallMinZ},wallStone});
    g.platforms.push_back({{0,wallY,wallMinZ},{wallX*2,wallHeight,wallThickness},wallStone});
    g.platforms.push_back({{(-wallX+gateLeft)*0.5f,wallY,wallMaxZ},
                           {gateLeft+wallX,wallHeight,wallThickness},wallStone});
    g.platforms.push_back({{(gateRight+wallX)*0.5f,wallY,wallMaxZ},
                           {wallX-gateRight,wallHeight,wallThickness},wallStone});
    // Gate towers and lintel make the opening legible while leaving ample player clearance.
    const float gatePillarOffset=gateHalfWidth+0.6f;
    g.platforms.push_back({{gate.x-gatePillarOffset,wallY,wallMaxZ},
                           {1.2f,wallHeight,3.0f},gateStone});
    g.platforms.push_back({{gate.x+gatePillarOffset,wallY,wallMaxZ},
                           {1.2f,wallHeight,3.0f},gateStone});
    g.platforms.push_back({{gate.x,wallHeight+0.8f,wallMaxZ},
                           {gateHalfWidth*2+2.0f,1.6f,3.0f},gateStone});
    // Support every elevated route deck with four grounded stone piers.
    // The open center keeps the jump/landing approach clear while the piers
    // physically connect each deck to the city floor.
    const float supportSidesX[4]={-1.0f,-1.0f,1.0f,1.0f};
    const float supportSidesZ[4]={-1.0f,1.0f,-1.0f,1.0f};
    for(int route=0;route<4;route++) {
        const int deckIndex=route<3?2+route:5;
        const Platform deck=g.platforms[deckIndex];
        const float pillarSize=route==3?0.85f:0.55f;
        const float offset=deck.size.x*0.5f-pillarSize*0.5f-0.25f;
        const float deckBottom=deck.position.y-deck.size.y*0.5f;
        const Color supportColor=route==3?Color{91,78,59,255}:Color{
            (unsigned char)(tint[g.city].r*0.68f),
            (unsigned char)(tint[g.city].g*0.68f),
            (unsigned char)(tint[g.city].b*0.68f),255};
        for(int corner=0;corner<4;corner++) {
            g.platforms.push_back({
                {deck.position.x+supportSidesX[corner]*offset,deckBottom*0.5f,
                 deck.position.z+supportSidesZ[corner]*offset},
                {pillarSize,deckBottom,pillarSize},supportColor});
        }
    }
    // Three stone walls enclose the raised boss platform while keeping its south
    // entrance open for the final rooftop route. Their inner faces match the
    // boss collision bounds so the model cannot clip through the visible arena.
    const float arenaWallOffsetX=arenaHalfWidth-arenaWallThickness*0.5f;
    const float arenaWallOffsetZ=arenaHalfDepth-arenaWallThickness*0.5f;
    const float arenaWallY=gateFloorTop.y+arenaWallHeight*0.5f;
    g.platforms.push_back({
        {gate.x-arenaWallOffsetX,arenaWallY,gate.z},
        {arenaWallThickness,arenaWallHeight,arenaHalfDepth*2.0f-arenaWallThickness},gateStone});
    g.platforms.push_back({
        {gate.x+arenaWallOffsetX,arenaWallY,gate.z},
        {arenaWallThickness,arenaWallHeight,arenaHalfDepth*2.0f-arenaWallThickness},gateStone});
    g.platforms.push_back({
        {gate.x,arenaWallY,gate.z+arenaWallOffsetZ},
        {arenaHalfWidth*2.0f,arenaWallHeight,arenaWallThickness},gateStone});
    for(int i=0;i<2+g.city*2;i++) {
        float a=6.28318f*i/(2+g.city*2);
        Vector3 p={std::cos(a)*(12.0f+g.city),0.0f,std::sin(a)*(12.0f+g.city)+3.0f};
        float toughness=42.0f+g.city*14.0f;
        g.enemies.push_back({p,toughness,toughness,2.4f+g.city*0.32f,0,false,true});
    }
    g.player.position={0,0.3f,0}; g.player.velocity={0,0,0}; g.player.moveVelocity={0,0};
    g.player.grounded=true; g.player.wallRunning=false; g.player.dashTime=0;
    g.player.vaulting=false; g.player.vaultTime=0; g.player.ledgeGrabbing=false; g.player.climbing=false;
    g.player.landingTime=0; g.player.landingRoll=false; g.player.attackTime=0;
    g.player.climbTime=0; g.player.ledgeNormal={}; g.player.ledgeHangPosition={}; g.player.ledgeTargetPosition={};
}

void SetQuestProgress(GameState& g,int quest) {
    const int nextQuest=std::clamp(quest,0,3);
    if(nextQuest!=g.quest) g.beaconHoldTime=0.0f;
    g.quest=nextQuest;
    if(g.quest<3) {
        g.bossArena.active=false;
        g.bossUnlocked=false;
        if(g.quest>0) g.questLimit=100.0f-g.city*8.0f-g.quest*9.0f;
        return;
    }
    g.bossArena.active=true;
    for(const auto& e:g.enemies) if(e.boss) {
        g.bossUnlocked=true;
        return;
    }
    float hp=280.0f+g.city*120.0f;
    g.enemies.push_back({QuestTarget(g.city,3),hp,hp,2.4f+g.city*0.38f,0,true,true});
    g.enemies.back().variant=g.city;
    g.bossUnlocked=true;
}
