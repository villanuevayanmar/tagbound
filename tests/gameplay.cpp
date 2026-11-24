#include "game.hpp"
#include "raymath.h"
#include <algorithm>
#include <cassert>
#include <cmath>
#include <iostream>

static bool near(float a, float b, float epsilon=0.02f) {
    return std::fabs(a-b) <= epsilon;
}

static float cameraTargetYaw(const Camera3D& camera,Vector3 player) {
    return std::atan2(camera.target.x-player.x,camera.target.z-player.z);
}
static float angleDistance(float a,float b) {
    return std::atan2(std::sin(a-b),std::cos(a-b));
}

int main() {
    GameState game;
    BuildCity(game);
    assert(game.city == 0);
    assert(game.platforms.size() == 34);
    assert(game.enemies.size() == 2);
    assert(std::string(CityName(0)) == "PORT TOWN");
    assert(std::string(CityName(3)) == "IMPERIAL CAPITAL");
    assert(std::string(CityName(4)) == "HIGH SPIRE");
    assert(std::string(BossName(0)) == "HARBOR WARDEN");
    assert(std::string(BossName(3)) == "IMPERIAL REGENT");
    assert(std::string(BossName(4)) == "SPIRE SENTINEL");
    assert(MigrateLegacyCityIndex(0)==0);
    assert(MigrateLegacyCityIndex(1)==1);
    assert(MigrateLegacyCityIndex(2)==2);
    assert(MigrateLegacyCityIndex(3)==4);
    assert(MigrateLegacyCityIndex(4)==3);
    assert(MigrateLegacyCityIndex(-1)==-1);
    assert(std::string(WeaponName(Weapon::Spear)) == "SPEAR");

    // Enemy models face their target; boss windup/strike poses retain the locked warning direction.
    Enemy facingEnemy{{2,0.3f,3},100,100,2.5f,0,false,true};
    Vector3 facing=EnemyFacing(facingEnemy,{5,0.3f,7});
    assert(near(facing.x,0.6f)&&near(facing.z,0.8f));
    Enemy facingBoss=facingEnemy;
    facingBoss.boss=true;
    facingBoss.phase=BossPhase::Windup;
    facingBoss.strikeDirection={0,0,4};
    facing=EnemyFacing(facingBoss,{-10,0.3f,3});
    assert(near(facing.x,0.0f)&&near(facing.z,1.0f));
    facingBoss.phase=BossPhase::Recover;
    facing=EnemyFacing(facingBoss,{-2,0.3f,3});
    assert(near(facing.x,-1.0f)&&near(facing.z,0.0f));
    facing=EnemyFacing(facingEnemy,facingEnemy.position);
    assert(near(facing.x,0.0f)&&near(facing.z,-1.0f));


    // Each city has its own rooftop route with consistent climb spacing.
    const Vector3 expectedRoutes[5][4]={
        {{4,1.7f,6},{10,2.45f,13},{14,3.2f,19},{18,3.95f,25}},
        {{-4,1.7f,6},{-10,2.45f,13},{-14,3.2f,19},{-18,3.95f,25}},
        {{5,1.7f,3},{12,2.45f,8},{10,3.2f,16},{5,3.95f,24}},
        {{0,1.7f,7},{7,2.45f,14},{0,3.2f,21},{0,3.95f,29}},
        {{-5,1.7f,3},{-12,2.45f,8},{-10,3.2f,16},{-5,3.95f,24}}
    };
    for(int city=0;city<5;city++) {
        for(int quest=0;quest<4;quest++) {
            Vector3 actual=QuestTarget(city,quest),expected=expectedRoutes[city][quest];
            assert(near(actual.x,expected.x)&&near(actual.y,expected.y)&&near(actual.z,expected.z));
            assert(std::abs(actual.x)<49&&actual.z>-35&&actual.z<42);
            if(quest>0) {
                Vector3 previous=QuestTarget(city,quest-1);
                float dx=actual.x-previous.x,dy=actual.y-previous.y,dz=actual.z-previous.z;
                float spacing=std::sqrt(dx*dx+dy*dy+dz*dz);
                assert(spacing>7.0f&&spacing<11.0f);
            }
        }
    }

    // Saved quest progress maps back to the latest cleared beacon checkpoint.
    for(int city=0;city<5;city++) for(int quest=0;quest<4;quest++) {
        Vector3 checkpoint=QuestCheckpoint(city,quest);
        Vector3 expected=quest>0?QuestTarget(city,quest-1):Vector3{0,0.3f,0};
        assert(near(checkpoint.x,expected.x)&&near(checkpoint.y,expected.y)&&near(checkpoint.z,expected.z));
        Vector3 target=QuestTarget(city,quest);
        Vector3 facing=QuestCheckpointFacing(city,quest);
        float dx=target.x-checkpoint.x, dz=target.z-checkpoint.z;
        float length=std::sqrt(dx*dx+dz*dz);
        assert(length<0.001f || (near(facing.x,dx/length)&&near(facing.z,dz/length)));
    }

    // Each city has a deliberate mix of reach and hold-beacon objectives.
    const bool holdStages[5][3]={
        {false,true,false},
        {true,false,false},
        {false,false,true},
        {true,false,true},
        {false,true,false}
    };
    const bool survivalStages[5][3]={
        {false,false,false},
        {false,true,false},
        {false,true,false},
        {false,false,false},
        {true,false,false}
    };
    for(int city=0;city<5;city++) for(int quest=0;quest<3;quest++) {
        assert(QuestRequiresHold(city,quest)==holdStages[city][quest]);
        assert(QuestRequiresSurvival(city,quest)==survivalStages[city][quest]);
        assert(!(holdStages[city][quest]&&survivalStages[city][quest]));
        if(holdStages[city][quest]) {
            float duration=QuestHoldDuration(city,quest);
            assert(duration>=2.5f&&duration<=4.0f);
        } else assert(near(QuestHoldDuration(city,quest),0.0f));
        if(survivalStages[city][quest]) {
            float duration=QuestSurvivalDuration(city,quest);
            assert(duration>=18.0f&&duration<=22.0f);
        } else assert(near(QuestSurvivalDuration(city,quest),0.0f));
    }

    // Reach objectives complete as soon as the beacon radius is entered.
    GameState reachObjective;
    BuildCity(reachObjective);
    reachObjective.player.position=QuestTarget(0,0);
    assert(!QuestRequiresHold(0,0));
    assert(UpdateQuestObjective(reachObjective,0.016f));
    assert(near(reachObjective.beaconHoldTime,0.0f));

    // Hold objectives accumulate only while the player remains inside the beacon.
    GameState holdObjective;
    BuildCity(holdObjective);
    SetQuestProgress(holdObjective,1);
    const Vector3 holdTarget=QuestTarget(0,1);
    const float holdDuration=QuestHoldDuration(0,1);
    holdObjective.player.position=holdTarget;
    assert(!UpdateQuestObjective(holdObjective,holdDuration*0.4f));
    assert(near(holdObjective.beaconHoldTime,holdDuration*0.4f));
    holdObjective.player.position.x+=5.0f;
    assert(!UpdateQuestObjective(holdObjective,0.1f));
    assert(near(holdObjective.beaconHoldTime,0.0f));
    holdObjective.player.position=holdTarget;
    assert(!UpdateQuestObjective(holdObjective,holdDuration*0.5f));
    assert(UpdateQuestObjective(holdObjective,holdDuration*0.5f+0.01f));
    assert(near(holdObjective.beaconHoldTime,0.0f));

    // Survival objectives progress away from the beacon and finish only after their timer.
    const int survivalCities[3]={1,2,4};
    const int survivalQuests[3]={1,1,0};
    for(int i=0;i<3;i++) {
        GameState survivalObjective;
        survivalObjective.city=survivalCities[i];
        BuildCity(survivalObjective);
        SetQuestProgress(survivalObjective,survivalQuests[i]);
        const float duration=QuestSurvivalDuration(survivalObjective.city,survivalObjective.quest);
        assert(QuestRequiresSurvival(survivalObjective.city,survivalObjective.quest));
        assert(!QuestRequiresHold(survivalObjective.city,survivalObjective.quest));
        assert(near(QuestHoldDuration(survivalObjective.city,survivalObjective.quest),0.0f));
        survivalObjective.player.position={-30,0.3f,30};
        assert(!UpdateQuestObjective(survivalObjective,duration*0.4f));
        assert(near(survivalObjective.beaconHoldTime,duration*0.4f));
        assert(!UpdateQuestObjective(survivalObjective,duration*0.5f));
        assert(UpdateQuestObjective(survivalObjective,duration*0.1f+0.01f));
        assert(near(survivalObjective.beaconHoldTime,0.0f));
    }


    // Changing the quest or rebuilding the city never carries old capture time forward.
    holdObjective.beaconHoldTime=0.9f;
    SetQuestProgress(holdObjective,2);
    assert(near(holdObjective.beaconHoldTime,0.0f));
    holdObjective.beaconHoldTime=0.9f;
    BuildCity(holdObjective);
    assert(near(holdObjective.beaconHoldTime,0.0f));

    // Boss warning countdowns and strike radii use shared combat/render values.
    const float bossWindups[5]={0.65f,0.45f,0.9f,0.5f,0.7f};
    const float bossRanges[5]={4.0f,2.3f,5.5f,4.5f,8.0f};
    for(int variant=0;variant<5;variant++) {
        assert(near(BossWindupDuration(variant),bossWindups[variant]));
        assert(near(BossWindupDuration(variant,true),bossWindups[variant]*0.85f));
        assert(near(BossStrikeRange(variant),bossRanges[variant]));
    }

    // Boss arena collision bounds are derived from the visible final gate floor.
    // Pursuit, knockback, charge movement and attack phases must keep every boss grounded.
    for(int city=0;city<5;city++) {
        GameState arenaGame;
        arenaGame.city=city;
        BuildCity(arenaGame);
        assert(!arenaGame.bossArena.active);
        arenaGame.enemies.clear();
        SetQuestProgress(arenaGame,3);
        assert(arenaGame.bossArena.active);
        const Vector3 gate=QuestTarget(city,3);
        assert(near(arenaGame.bossArena.center.x,gate.x));
        assert(near(arenaGame.bossArena.center.y,gate.y));
        assert(near(arenaGame.bossArena.center.z,gate.z));
        assert(near(arenaGame.bossArena.halfExtents.x,4.0f));
        assert(near(arenaGame.bossArena.halfExtents.y,4.0f));
        assert(near(arenaGame.bossArena.floorY,gate.y));
        assert(arenaGame.enemies.size()==1&&arenaGame.enemies.front().boss);
        Enemy* arenaBoss=&arenaGame.enemies.front();
        assert(BossArenaContains(arenaGame,*arenaBoss));
        arenaGame.player.position={gate.x+30.0f,gate.y,gate.z+30.0f};
        const Vector3 pushes[4]={{1,0,0},{-1,0,0},{0,0,1},{0,0,-1}};
        for(const Vector3 push:pushes) {
            arenaBoss->position={
                gate.x+(push.x>0?3.30f:(push.x<0?-3.30f:0.0f)),
                gate.y+3.0f,
                gate.z+(push.z>0?3.30f:(push.z<0?-3.30f:0.0f))
            };
            arenaBoss->stunTime=1.0f;
            arenaBoss->knockback=Vector3Scale(push,100.0f);
            UpdatePursuit(arenaGame,0.1f);
            assert(BossArenaContains(arenaGame,*arenaBoss));
        }
        // A boss pursuing a player outside the platform cannot walk off its floor.
        arenaBoss->stunTime=0.0f;
        arenaBoss->phase=BossPhase::Hunt;
        arenaBoss->position={gate.x+3.30f,gate.y,gate.z};
        arenaGame.player.position={gate.x+18.0f,gate.y,gate.z};
        UpdatePursuit(arenaGame,0.1f);
        assert(BossArenaContains(arenaGame,*arenaBoss));

        // The Gilded Charger is the only boss with forward strike movement.
        if(city==1) {
            arenaBoss->variant=1;
            arenaBoss->phase=BossPhase::Strike;
            arenaBoss->phaseTime=0.0f;
            arenaBoss->strikeDirection={1,0,0};
            arenaBoss->strikeOrigin=arenaBoss->position;
            arenaBoss->position={gate.x+3.30f,gate.y,gate.z};
            UpdatePursuit(arenaGame,0.1f);
            assert(BossArenaContains(arenaGame,*arenaBoss));
        }
        // Resolve each city's attack phase at the boundary, then verify recovery is still grounded.
        arenaBoss->variant=city;
        arenaBoss->phase=BossPhase::Strike;
        arenaBoss->phaseTime=2.0f;
        arenaBoss->position={gate.x+3.30f,gate.y+2.0f,gate.z};
        arenaGame.player.position={gate.x+30.0f,gate.y,gate.z+30.0f};
        UpdatePursuit(arenaGame,0.05f);
        assert(arenaBoss->phase==BossPhase::Recover);
        assert(BossArenaContains(arenaGame,*arenaBoss));
        SetQuestProgress(arenaGame,2);
        assert(!arenaGame.bossArena.active);
    }

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
        for(int quest=0;quest<4;quest++) {
            Vector3 target=QuestTarget(city,quest);
            const Platform& routePlatform=game.platforms[2+quest];
            assert(near(routePlatform.position.x,target.x));
            assert(near(routePlatform.position.y+0.25f,target.y));
            assert(near(routePlatform.position.z,target.z));
        }
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
    Vector3 restoredGate=QuestTarget(2,3);
    assert(near(restoredBoss.position.x,restoredGate.x)&&near(restoredBoss.position.y,restoredGate.y)&&near(restoredBoss.position.z,restoredGate.z));
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
    ResetCamera(game.player,camera);
    Controls move{};
    move.move.y = 1.0f;
    for(int frame=0;frame<12;frame++) UpdateMovement(game,move,camera,1.0f/60.0f);
    assert(game.player.position.z > 0.4f);

    // Ground movement ramps to the existing top speed, then brakes when released.
    GameState movementResponse;
    movementResponse.platforms.push_back({{0,-0.3f,0},{120,0.6f,120},GRAY});
    movementResponse.player.position={0,0.3f,0};
    Camera3D movementCamera{};
    ResetCamera(movementResponse.player,movementCamera);
    Controls forwardMove{};
    forwardMove.move.y=1.0f;
    for(int frame=0;frame<6;frame++)
        UpdateMovement(movementResponse,forwardMove,movementCamera,1.0f/60.0f);
    assert(movementResponse.player.moveVelocity.y>1.0f);
    assert(movementResponse.player.moveVelocity.y<6.0f);
    for(int frame=0;frame<30;frame++)
        UpdateMovement(movementResponse,forwardMove,movementCamera,1.0f/60.0f);
    assert(near(movementResponse.player.moveVelocity.y,6.0f,0.08f));
    const float speedBeforeBrake=movementResponse.player.moveVelocity.y;
    UpdateMovement(movementResponse,Controls{},movementCamera,1.0f/60.0f);
    assert(movementResponse.player.moveVelocity.y<speedBeforeBrake);
    assert(movementResponse.player.moveVelocity.y>0.0f);
    for(int frame=0;frame<30;frame++)
        UpdateMovement(movementResponse,Controls{},movementCamera,1.0f/60.0f);
    assert(std::abs(movementResponse.player.moveVelocity.y)<0.05f);

    // The existing dash burst still reaches its burst speed during its active window.
    GameState dashResponse;
    dashResponse.platforms.push_back({{0,-0.3f,0},{120,0.6f,120},GRAY});
    dashResponse.player.position={0,0.3f,0};
    dashResponse.player.escapeSkill=1;
    Camera3D dashCamera{};
    ResetCamera(dashResponse.player,dashCamera);
    Controls dashMove{};
    dashMove.move.y=1.0f;
    dashMove.skill=true;
    for(int frame=0;frame<12;frame++)
        UpdateMovement(dashResponse,dashMove,dashCamera,1.0f/60.0f);
    assert(dashResponse.player.dashTime>0.0f);
    assert(dashResponse.player.moveVelocity.y>15.5f);

    // Air steering has less authority than grounded acceleration.
    GameState airControl;
    airControl.platforms.push_back({{0,-0.3f,0},{120,0.6f,120},GRAY});
    airControl.player.position={0,4.0f,0};
    airControl.player.grounded=false;
    Camera3D airCamera{};
    ResetCamera(airControl.player,airCamera);
    Controls airStrafe{};
    airStrafe.move.x=1.0f;
    UpdateMovement(airControl,airStrafe,airCamera,1.0f/60.0f);
    assert(airControl.player.moveVelocity.x>0.0f);
    assert(airControl.player.moveVelocity.x<0.4f);

    // The camera-relative control scheme keeps its heading while moving, then follows
    // the direction the character last faced after movement and any manual-look override end.
    GameState cameraFollow;
    cameraFollow.platforms.push_back({{0,0,0},{120,0.6f,120},GRAY});
    cameraFollow.player.position={0,0.3f,0};
    cameraFollow.player.facing={0,0,1};
    Camera3D followCamera{};
    ResetCamera(cameraFollow.player,followCamera);
    assert(near(cameraTargetYaw(followCamera,cameraFollow.player.position),0.0f));
    assert(near(followCamera.target.y-cameraFollow.player.position.y,0.3f));
    Controls strafe{};
    strafe.move.x=1.0f;
    UpdateMovement(cameraFollow,strafe,followCamera,0.1f);
    assert(cameraFollow.player.facing.x>0.99f&&std::abs(cameraFollow.player.facing.z)<0.02f);
    assert(near(cameraTargetYaw(followCamera,cameraFollow.player.position),0.0f));
    for(int frame=0;frame<30;frame++) UpdateMovement(cameraFollow,Controls{},followCamera,0.1f);
    assert(std::abs(angleDistance(cameraTargetYaw(followCamera,cameraFollow.player.position),PI/2.0f))<0.05f);

    // A manual swipe overrides automatic follow briefly while preserving camera-relative input.
    cameraFollow.player.position={0,0.3f,0};
    cameraFollow.player.facing={0,0,1};
    ResetCamera(cameraFollow.player,followCamera);
    Controls manualStrafe{};
    manualStrafe.move.x=1.0f;
    manualStrafe.lookX=60.0f;
    UpdateMovement(cameraFollow,manualStrafe,followCamera,0.1f);
    const float manuallySetYaw=cameraTargetYaw(followCamera,cameraFollow.player.position);
    assert(near(manuallySetYaw,-0.48f));
    for(int frame=0;frame<4;frame++) UpdateMovement(cameraFollow,Controls{},followCamera,0.1f);
    assert(std::abs(angleDistance(cameraTargetYaw(followCamera,cameraFollow.player.position),manuallySetYaw))<0.01f);
    for(int frame=0;frame<30;frame++) UpdateMovement(cameraFollow,Controls{},followCamera,0.1f);
    const float strafeFacingYaw=std::atan2(cameraFollow.player.facing.x,cameraFollow.player.facing.z);
    assert(std::abs(angleDistance(cameraTargetYaw(followCamera,cameraFollow.player.position),strafeFacingYaw))<0.05f);

    // Starting a new run resets yaw, pitch and manual-look override to the new spawn facing.
    cameraFollow.player.facing={-1,0,0};
    ResetCamera(cameraFollow.player,followCamera);
    assert(std::abs(angleDistance(cameraTargetYaw(followCamera,cameraFollow.player.position),-PI/2.0f))<0.001f);
    assert(near(followCamera.target.y-cameraFollow.player.position.y,0.3f));
    UpdateMovement(cameraFollow,Controls{},followCamera,0.1f);
    assert(std::abs(angleDistance(cameraTargetYaw(followCamera,cameraFollow.player.position),-PI/2.0f))<0.001f);
    Player defaultCameraPlayer;
    ResetCamera(defaultCameraPlayer,camera);

    // Skirmisher keeps the fastest base movement speed without sprinting.
    GameState vanguardMovement;
    vanguardMovement.platforms.push_back({{0,-0.3f,0},{120,0.6f,120},GRAY});
    vanguardMovement.player.position={0,0.3f,0};
    vanguardMovement.player.playerClass=PlayerClass::Vanguard;
    GameState skirmisherMovement;
    skirmisherMovement.platforms.push_back({{0,-0.3f,0},{120,0.6f,120},GRAY});
    skirmisherMovement.player.position={0,0.3f,0};
    skirmisherMovement.player.playerClass=PlayerClass::Skirmisher;
    Camera3D vanguardCamera{},skirmisherCamera{};
    ResetCamera(vanguardMovement.player,vanguardCamera);
    ResetCamera(skirmisherMovement.player,skirmisherCamera);
    for(int frame=0;frame<20;frame++) {
        UpdateMovement(vanguardMovement,move,vanguardCamera,1.0f/60.0f);
        UpdateMovement(skirmisherMovement,move,skirmisherCamera,1.0f/60.0f);
    }
    assert(skirmisherMovement.player.position.z>vanguardMovement.player.position.z+0.1f);

    // Simulate every raised rooftop transfer from each city's spawn roof to its boss gate.
    for(int city=0;city<5;city++) {
        for(int segment=0;segment<4;segment++) {
            GameState routeGame;
            routeGame.city=city;
            BuildCity(routeGame);
            routeGame.player.playerClass=PlayerClass::Skirmisher;
            routeGame.player.weapon=Weapon::Bow;
            routeGame.player.stamina=100.0f;

            const Vector3 startCenter=segment==0?Vector3{0,0.3f,0}:QuestTarget(city,segment-1);
            const Vector3 target=QuestTarget(city,segment);
            Vector3 direction={target.x-startCenter.x,0,target.z-startCenter.z};
            const float distance=Vector3Length(direction);
            assert(distance>0.001f);
            direction=Vector3Scale(direction,1.0f/distance);

            const float startHalf=segment==0?4.0f:2.5f;
            const float edgeX=std::fabs(direction.x)>0.001f?startHalf/std::fabs(direction.x):1000.0f;
            const float edgeZ=std::fabs(direction.z)>0.001f?startHalf/std::fabs(direction.z):1000.0f;
            const float takeoffDistance=std::min(edgeX,edgeZ)-0.25f;
            routeGame.player.position={
                startCenter.x+direction.x*takeoffDistance,
                startCenter.y,
                startCenter.z+direction.z*takeoffDistance
            };
            routeGame.player.velocity={0,0,0};
            routeGame.player.grounded=true;
            routeGame.player.stamina=100.0f;

            if(segment==3) {
                // Starting the final approach must instantiate this city's actual boss at its gate.
                SetQuestProgress(routeGame,3);
                assert(routeGame.bossUnlocked);
                assert(std::count_if(routeGame.enemies.begin(),routeGame.enemies.end(),
                    [](const Enemy& enemy){return enemy.boss&&enemy.alive;})==1);
                const Enemy& boss=*std::find_if(routeGame.enemies.begin(),routeGame.enemies.end(),
                    [](const Enemy& enemy){return enemy.boss&&enemy.alive;});
                assert(boss.variant==city);
                assert(near(boss.position.x,target.x)&&near(boss.position.y,target.y)&&near(boss.position.z,target.z));
            }

            // Keep BuildCity's grounded spawn state and reset the shared camera for this route.
            Camera3D routeCamera=camera;
            ResetCamera(routeGame.player,routeCamera);

            const float targetHalf=segment==3?4.0f:2.5f;
            bool airborne=false;
            bool landed=false;
            for(int frame=0;frame<180;frame++) {
                Vector3 toTarget={target.x-routeGame.player.position.x,0,target.z-routeGame.player.position.z};
                const float remaining=Vector3Length(toTarget);
                Controls input{};
                if(remaining>0.25f) {
                    const Vector3 desired=Vector3Scale(toTarget,1.0f/remaining);
                    // Controls are camera-relative, so convert the world-space route direction
                    // into this frame's camera basis before applying the movement input.
                    const float yaw=cameraTargetYaw(routeCamera,routeGame.player.position);
                    const Vector3 cameraForward={std::sin(yaw),0,std::cos(yaw)};
                    const Vector3 cameraRight={cameraForward.z,0,-cameraForward.x};
                    input.move.x=Vector3DotProduct(desired,cameraRight);
                    input.move.y=Vector3DotProduct(desired,cameraForward);
                    input.sprint=true;
                }
                // If the jump reaches a ledge, use the game's required jump-to-climb action.
                input.jump=frame==0||routeGame.player.ledgeGrabbing;
                UpdateMovement(routeGame,input,routeCamera,1.0f/60.0f);
                if(!routeGame.player.grounded) airborne=true;
                const bool insideTarget=std::fabs(routeGame.player.position.x-target.x)<targetHalf-0.2f &&
                                        std::fabs(routeGame.player.position.z-target.z)<targetHalf-0.2f;
                if(airborne&&routeGame.player.grounded&&insideTarget&&
                   routeGame.player.position.y>=target.y-0.05f) {
                    landed=true;
                    break;
                }
            }
            if(segment==3) {
                assert(routeGame.bossUnlocked);
                assert(std::count_if(routeGame.enemies.begin(),routeGame.enemies.end(),
                    [](const Enemy& enemy){return enemy.boss&&enemy.alive;})==1);
            }
            if(!landed) {
                std::cerr << "Rooftop route transfer failed: city=" << city
                          << " segment=" << segment
                          << " player=(" << routeGame.player.position.x << ","
                          << routeGame.player.position.y << ","
                          << routeGame.player.position.z << ")" << std::endl;
                assert(landed);
            }
        }
    }
    Player defaultRouteCameraPlayer;
    ResetCamera(defaultRouteCameraPlayer,camera);

    // A high-speed Skirmisher dash cannot tunnel through a thin wall.
    GameState dashCollision;
    dashCollision.platforms.push_back({{0,-0.3f,0},{120,0.6f,120},GRAY});
    dashCollision.platforms.push_back({{1.0f,1.5f,0},{0.2f,3.0f,4.0f},GRAY});
    dashCollision.player.playerClass=PlayerClass::Skirmisher;
    dashCollision.player.position={0.3f,0.3f,0};
    dashCollision.player.stamina=100;
    dashCollision.player.escapeSkill=1;
    Controls fastDash{};
    fastDash.move.x=1.0f;
    fastDash.sprint=true;
    fastDash.skill=true;
    UpdateMovement(dashCollision,fastDash,camera,0.04f);
    assert(dashCollision.player.position.x<0.56f);

    // Wall-running consumes stamina and a wall-jump launches away from the surface.
    GameState wallRun;
    wallRun.platforms.push_back({{0,-0.3f,0},{120,0.6f,120},GRAY});
    wallRun.platforms.push_back({{0,1.5f,0},{1,3,8},GRAY});
    wallRun.player.position={0.95f,2.0f,0};
    wallRun.player.velocity={0,-1.0f,0};
    wallRun.player.grounded=false;
    Controls wallRunInput{};
    wallRunInput.sprint=true;
    UpdateMovement(wallRun,wallRunInput,camera,0.04f);
    assert(wallRun.player.wallRunning);
    assert(wallRun.player.velocity.y>-2.5f);
    assert(near(wallRun.player.stamina,98.96f));
    Controls wallJumpInput=wallRunInput;
    wallJumpInput.jump=true;
    UpdateMovement(wallRun,wallJumpInput,camera,0.04f);
    assert(!wallRun.player.wallRunning);
    assert(wallRun.player.velocity.x>5.0f);
    assert(wallRun.player.velocity.y>8.5f);

    // Holding sprint cannot regenerate stamina while a wall-run is exhausting it.
    GameState exhaustedWallRun;
    exhaustedWallRun.platforms.push_back({{0,-0.3f,0},{120,0.6f,120},GRAY});
    exhaustedWallRun.platforms.push_back({{0,1.5f,0},{1,3,8},GRAY});
    exhaustedWallRun.player.position={0.95f,2.0f,0};
    exhaustedWallRun.player.velocity={0,-1.0f,0};
    exhaustedWallRun.player.grounded=false;
    exhaustedWallRun.player.stamina=0.2f;
    for(int frame=0;frame<2;frame++) UpdateMovement(exhaustedWallRun,wallRunInput,camera,0.04f);
    assert(near(exhaustedWallRun.player.stamina,0.0f));
    assert(!exhaustedWallRun.player.wallRunning);

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

    // A player killed by pursuit before combat resolution cannot start a final melee hit.
    GameState deadMelee;
    BuildCity(deadMelee);
    deadMelee.enemies.clear();
    deadMelee.player.position={0,0.3f,0};
    deadMelee.player.facing={0,0,1};
    deadMelee.player.weapon=Weapon::Sword;
    deadMelee.player.health=0;
    deadMelee.enemies.push_back({{0,0.3f,1.5f},100,100,0,0,false,true});
    Controls finalMelee{};
    finalMelee.attack=true;
    UpdateCombat(deadMelee,finalMelee,0.016f);
    assert(near(deadMelee.enemies.front().hp,100.0f));
    assert(near(deadMelee.player.attackTime,0.0f));
    assert(near(deadMelee.player.health,ClassMaxHealth(deadMelee.player.playerClass)));

    // A player killed before combat resolution cannot create a new bow projectile.
    GameState deadBow;
    BuildCity(deadBow);
    deadBow.enemies.clear();
    deadBow.player.position={0,0.3f,0};
    deadBow.player.facing={0,0,1};
    deadBow.player.weapon=Weapon::Bow;
    deadBow.player.playerClass=PlayerClass::Skirmisher;
    deadBow.player.health=0;
    Controls finalBow{};
    finalBow.attack=true;
    UpdateCombat(deadBow,finalBow,0.016f);
    assert(deadBow.projectiles.empty());
    assert(near(deadBow.player.attackTime,0.0f));
    assert(near(deadBow.player.health,ClassMaxHealth(deadBow.player.playerClass)));

    // Every melee weapon must apply its intended damage.
    const Weapon weapons[] = {Weapon::Sword, Weapon::Axe, Weapon::Shield, Weapon::Spear};
    const float expectedDamage[] = {30, 43, 14, 32};
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
    assert(near(game.enemies[1].hp,70.0f));

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
    assert(near(game.enemies.front().hp,70.0f));
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
    // Hitting a pursuer during its warning interrupts the pending strike.
    game.enemies.clear();
    game.platforms.clear();
    game.player.position={0,0.3f,0};
    game.player.facing={0,0,1};
    game.player.weapon=Weapon::Sword;
    game.player.health=100;
    game.player.invulnerable=0;
    game.enemies.push_back({{0,0.3f,0.8f},100,100,0,0,false,true});
    UpdatePursuit(game,0.016f);
    assert(game.enemies.front().attackWindup>0.35f);
    Controls interruptAttack{};
    interruptAttack.attack=true;
    UpdateCombat(game,interruptAttack,0.016f);
    assert(game.enemies.front().stunTime>0);
    UpdatePursuit(game,0.016f);
    assert(near(game.enemies.front().attackWindup,0.0f));
    assert(near(game.player.health,100.0f));

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

    // Pursuers route around solid rooftop walls without overlapping them.
    game.platforms.clear();
    game.platforms.push_back({{0,1.5f,2.0f},{4,3.0f,0.4f},GRAY});
    game.enemies.clear();
    game.player.position={0,0.3f,6.0f};
    game.enemies.push_back({{0,0.3f,0},100,100,3.0f,0,false,true});
    float wallStartDistance=Vector3Distance(game.enemies.front().position,game.player.position);
    for(int i=0;i<12;i++) UpdatePursuit(game,0.1f);
    const Vector3 routed=game.enemies.front().position;
    assert(!(std::abs(routed.x)<2.42f&&routed.z>1.38f&&routed.z<2.62f));
    assert(Vector3Distance(routed,game.player.position)<wallStartDistance);

    // Axis-separated collision lets a pursuer move around the wall's open side.
    game.enemies.clear();
    game.player.position={4.0f,0.3f,5.0f};
    game.enemies.push_back({{0,0.3f,0},100,100,3.0f,0,false,true});
    for(int i=0;i<120;i++) UpdatePursuit(game,0.05f);
    assert(game.enemies.front().position.x>2.2f);
    assert(game.enemies.front().position.z>2.2f);
    game.platforms.clear();

    // Sequential walls are routed one at a time instead of blocking all candidate corners.
    game.enemies.clear();
    game.platforms.clear();
    game.platforms.push_back({{0,-0.3f,0},{120,0.6f,120},GRAY});
    game.platforms.push_back({{0,1.5f,2.0f},{4,3.0f,0.4f},GRAY});
    game.platforms.push_back({{0,1.5f,6.0f},{4,3.0f,0.4f},GRAY});
    game.player.position={0,0.3f,10.0f};
    game.enemies.push_back({{0,0.3f,0},100,100,3.0f,0,false,true});
    const float twoWallStart=Vector3Distance(game.enemies.front().position,game.player.position);
    for(int i=0;i<140;i++) {
        UpdatePursuit(game,0.05f);
        const Vector3 position=game.enemies.front().position;
        assert(!(std::abs(position.x)<2.42f&&position.z>1.38f&&position.z<2.62f));
        assert(!(std::abs(position.x)<2.42f&&position.z>5.38f&&position.z<6.62f));
    }
    assert(game.enemies.front().position.z>6.62f);
    assert(Vector3Distance(game.enemies.front().position,game.player.position)<twoWallStart);
    game.platforms.clear();

    // A pursuer aligned directly behind a wall finds an open side instead of stalling.
    game.enemies.clear();
    game.platforms.push_back({{0,1.5f,2.0f},{4,3.0f,0.4f},GRAY});
    game.player.position={0,0.3f,6.0f};
    game.enemies.push_back({{0,0.3f,0},100,100,3.0f,0,false,true});
    float initialDistance=Vector3Distance(game.enemies.front().position,game.player.position);
    for(int i=0;i<40;i++) UpdatePursuit(game,0.05f);
    assert(game.enemies.front().position.z>2.2f);
    assert(Vector3Distance(game.enemies.front().position,game.player.position)<initialDistance);
    game.platforms.clear();

    // Vanguard uses the standard guard stamina cost.
    game.player.playerClass=PlayerClass::Vanguard;
    // Blocking spends stamina instead of health; an exhausted block does not.
    game.enemies.clear();
    game.player.position={0,0.3f,0};
    game.player.facing={0,0,1};
    game.player.invulnerable=0;
    game.player.health = 100;
    game.player.stamina = 100;
    game.player.blocking = true;
    game.enemies.push_back({{0,0.3f,0.8f},40,40,0,0,false,true});
    UpdatePursuit(game,0.016f);
    assert(near(game.player.health,100.0f));
    assert(near(game.player.stamina,100.0f));
    assert(game.enemies.front().attackWindup>0.35f);
    UpdatePursuit(game,0.45f);
    assert(near(game.player.health,100.0f));
    assert(near(game.player.stamina,92.0f));
    assert(near(game.enemies.front().cooldown,1.0f));
    game.player.stamina=0;
    UpdatePursuit(game,1.0f);
    assert(near(game.enemies.front().cooldown,0.0f));
    assert(near(game.player.health,100.0f));
    UpdatePursuit(game,0.45f);
    assert(near(game.player.health,89.0f));
    game.player.blocking = false;

    // A shield cannot block an attack that arrives from behind.
    game.enemies.clear();
    game.player.playerClass=PlayerClass::Warden;
    game.player.position={0,0.3f,0};
    game.player.facing={0,0,1};
    game.player.health=100;
    game.player.stamina=100;
    game.player.invulnerable=0;
    game.player.blocking=true;
    game.enemies.push_back({{0,0.3f,-0.8f},40,40,0,0,false,true});
    UpdatePursuit(game,0.016f);
    assert(game.enemies.front().attackWindup>0.35f);
    UpdatePursuit(game,0.45f);
    assert(near(game.player.health,89.0f));
    assert(near(game.player.stamina,100.0f));
    game.player.blocking=false;

    // Warden's shield reduces guard stamina costs against both taggers and bosses.
    game.enemies.clear();
    game.player.playerClass=PlayerClass::Warden;
    game.player.position={0,0.3f,0};
    game.player.facing={0,0,1};
    game.player.health=100;
    game.player.stamina=100;
    game.player.invulnerable=0;
    game.player.blocking=true;
    game.enemies.push_back({{0,0.3f,0.8f},40,40,0,0,false,true});
    UpdatePursuit(game,0.016f);
    assert(near(game.player.health,100.0f));
    assert(near(game.player.stamina,100.0f));
    UpdatePursuit(game,0.45f);
    assert(near(game.player.health,100.0f));
    assert(near(game.player.stamina,95.0f));

    game.enemies.clear();
    game.player.position={0,0.3f,0};
    game.player.facing={0,0,1};
    game.player.health=100;
    game.player.stamina=100;
    game.player.invulnerable=0;
    Enemy guardedBoss{{0,0.3f,0.7f},100,100,0,0,true,true};
    guardedBoss.variant=0;
    guardedBoss.phase=BossPhase::Strike;
    guardedBoss.phaseTime=1.0f;
    guardedBoss.strikeDirection={0,0,-1};
    guardedBoss.strikeOrigin=guardedBoss.position;
    game.player.blocking=true;
    game.enemies.push_back(guardedBoss);
    UpdatePursuit(game,0.05f);
    assert(near(game.player.health,100.0f));
    assert(near(game.player.stamina,88.0f));

    // A boss strike from behind bypasses a frontal guard.
    game.enemies.clear();
    game.player.position={0,0.3f,0};
    game.player.facing={0,0,1};
    game.player.health=100;
    game.player.stamina=100;
    game.player.invulnerable=0;
    game.player.blocking=true;
    Enemy rearBoss{{0,0.3f,-3.5f},100,100,0,0,true,true};
    rearBoss.variant=0;
    rearBoss.phase=BossPhase::Strike;
    rearBoss.phaseTime=1.0f;
    rearBoss.strikeDirection={0,0,1};
    rearBoss.strikeOrigin=rearBoss.position;
    game.enemies.push_back(rearBoss);
    UpdatePursuit(game,0.05f);
    assert(near(game.player.health,76.0f));
    assert(near(game.player.stamina,100.0f));

    // A Warden with insufficient stamina still takes the unblocked hit.
    game.enemies.clear();
    game.player.position={0,0.3f,0};
    game.player.health=100;
    game.player.stamina=4;
    game.player.invulnerable=0;
    game.player.blocking=true;
    game.enemies.push_back({{0,0.3f,0.8f},40,40,0,0,false,true});
    UpdatePursuit(game,0.016f);
    assert(near(game.player.health,100.0f));
    assert(near(game.player.stamina,4.0f));
    UpdatePursuit(game,0.45f);
    assert(near(game.player.health,89.0f));
    assert(near(game.player.stamina,4.0f));
    game.player.blocking=false;

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
    assert(near(game.enemies.front().attackWindup,0.0f));

    // Breaking line of sight cancels an active warning and requires a fresh windup.
    game.platforms.clear();
    UpdatePursuit(game,0.016f);
    assert(game.enemies.front().attackWindup>0.35f);
    game.platforms.push_back({{0,1.5f,0.5f},{4,3.0f,0.4f},GRAY});
    UpdatePursuit(game,0.05f);
    assert(near(game.enemies.front().attackWindup,0.0f));
    game.platforms.clear();
    UpdatePursuit(game,0.016f);
    assert(game.enemies.front().attackWindup>0.35f);
    UpdatePursuit(game,0.45f);
    assert(near(game.player.health,89.0f));

    // A pursuer must route around a solid rooftop wall instead of stalling at it.
    GameState pathing;
    pathing.platforms.push_back({{0,-0.3f,0},{120,0.6f,120},GRAY});
    pathing.platforms.push_back({{0,1.5f,0},{0.4f,3.0f,3.0f},GRAY});
    pathing.player.position={0,0.3f,-6.0f};
    pathing.player.invulnerable=1000.0f;
    pathing.enemies.push_back({{0,0.3f,6.0f},40,40,2.4f,0,false,true});
    const float blockedRouteDistance=Vector3Distance(pathing.player.position,pathing.enemies.front().position);
    for(int frame=0;frame<240;frame++) UpdatePursuit(pathing,0.05f);
    const Enemy& routedPursuer=pathing.enemies.front();
    const float routedDistance=Vector3Distance(pathing.player.position,routedPursuer.position);
    assert(routedDistance<3.0f);
    assert(routedDistance<blockedRouteDistance-7.0f);
    const Platform& rooftopWall=pathing.platforms[1];
    const bool overlapsX=routedPursuer.position.x+0.42f>rooftopWall.position.x-rooftopWall.size.x*0.5f &&
                         routedPursuer.position.x-0.42f<rooftopWall.position.x+rooftopWall.size.x*0.5f;
    const bool overlapsY=routedPursuer.position.y+1.72f>rooftopWall.position.y-rooftopWall.size.y*0.5f+0.05f &&
                         routedPursuer.position.y<rooftopWall.position.y+rooftopWall.size.y*0.5f-0.05f;
    const bool overlapsZ=routedPursuer.position.z+0.42f>rooftopWall.position.z-rooftopWall.size.z*0.5f &&
                         routedPursuer.position.z-0.42f<rooftopWall.position.z+rooftopWall.size.z*0.5f;
    assert(!(overlapsX&&overlapsY&&overlapsZ));

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
    for(int variant=0;variant<5;variant++) {
        game.enemies.clear();
        game.player.position={0,0.3f,0};
        Enemy boss{{0,0.3f,BossStrikeRange(variant)*0.9f},100,100,2.5f,0,true,true};
        boss.variant=variant;
        boss.phase=BossPhase::Hunt;
        game.enemies.push_back(boss);
        UpdatePursuit(game,0.016f);
        assert(game.enemies.front().phase==BossPhase::Windup);
    }

    // The Harbor Warden cleaves a frontal wedge; a lateral dodge leaves the danger zone.
    game.platforms.clear();
    game.enemies.clear();
    game.player.position={0,0.3f,3.5f};
    game.player.health=100;
    game.player.invulnerable=0;
    Enemy harborCleave{{0,0.3f,0},100,100,0,0,true,true};
    harborCleave.variant=0;
    harborCleave.phase=BossPhase::Strike;
    harborCleave.phaseTime=1.0f;
    harborCleave.strikeDirection={0,0,1};
    harborCleave.strikeOrigin=harborCleave.position;
    game.enemies.push_back(harborCleave);
    UpdatePursuit(game,0.01f);
    assert(near(game.player.health,76.0f));
    assert(game.enemies.front().phase==BossPhase::Recover);

    game.enemies.clear();
    game.player.position={1.5f,0.3f,3.5f};
    game.player.health=100;
    game.player.invulnerable=0;
    Enemy dodgedHarbor{{0,0.3f,0},100,100,0,0,true,true};
    dodgedHarbor.variant=0;
    dodgedHarbor.phase=BossPhase::Strike;
    dodgedHarbor.phaseTime=1.0f;
    dodgedHarbor.strikeDirection={0,0,1};
    dodgedHarbor.strikeOrigin=dodgedHarbor.position;
    game.enemies.push_back(dodgedHarbor);
    UpdatePursuit(game,0.01f);
    assert(near(game.player.health,100.0f));
    assert(game.enemies.front().phase==BossPhase::Recover);

    // The Spire Sentinel locks a narrow attack lane when its windup begins.
    game.platforms.clear();
    game.enemies.clear();
    game.player.position={0,0.3f,7.0f};
    Enemy spireWindup{{0,0.3f,0},100,100,2.5f,0,true,true};
    spireWindup.variant=4;
    spireWindup.phase=BossPhase::Hunt;
    game.enemies.push_back(spireWindup);
    UpdatePursuit(game,0.016f);
    assert(game.enemies.front().phase==BossPhase::Windup);
    assert(near(game.enemies.front().strikeDirection.x,0.0f));
    assert(near(game.enemies.front().strikeDirection.z,1.0f));
    Vector3 spireHold=game.enemies.front().position;
    game.player.position={1.1f,0.3f,7.0f};
    UpdatePursuit(game,0.05f);
    assert(near(game.enemies.front().position.x,spireHold.x));
    assert(near(game.enemies.front().position.z,spireHold.z));
    assert(near(game.enemies.front().strikeDirection.x,0.0f));
    assert(near(game.enemies.front().strikeDirection.z,1.0f));

    // The Gilded Charger locks its charge direction and pauses during its warning.
    game.enemies.clear();
    game.player.position={0,0.3f,2.5f};
    Enemy chargerWindup{{0,0.3f,0},100,100,3.0f,0,true,true};
    chargerWindup.variant=1;
    chargerWindup.phase=BossPhase::Hunt;
    game.enemies.push_back(chargerWindup);
    UpdatePursuit(game,0.016f);
    assert(game.enemies.front().phase==BossPhase::Windup);
    assert(near(game.enemies.front().strikeDirection.z,1.0f));
    Vector3 chargerHold=game.enemies.front().position;
    game.player.position={1.1f,0.3f,2.5f};
    UpdatePursuit(game,0.05f);
    assert(near(game.enemies.front().position.x,chargerHold.x));
    assert(near(game.enemies.front().position.z,chargerHold.z));
    assert(near(game.enemies.front().strikeDirection.z,1.0f));

    // A Gilded Charger can be sidestepped after its direction-locked warning.
    game.enemies.clear();
    game.player.position={1.1f,0.3f,2.0f};
    game.player.health=100;
    game.player.invulnerable=0;
    Enemy dodgedCharger{{0,0.3f,0},100,100,0,0,true,true};
    dodgedCharger.variant=1;
    dodgedCharger.phase=BossPhase::Strike;
    dodgedCharger.phaseTime=0.33f;
    dodgedCharger.strikeDirection={0,0,1};
    dodgedCharger.strikeOrigin=dodgedCharger.position;
    game.enemies.push_back(dodgedCharger);
    UpdatePursuit(game,0.01f);
    assert(near(game.player.health,100.0f));
    assert(game.enemies.front().phase==BossPhase::Recover);

    // An aligned Spire Sentinel thrust hits, but a lateral dodge escapes its lane.
    game.enemies.clear();
    game.player.position={0,0.3f,7.0f};
    game.player.health=100;
    game.player.invulnerable=0;
    game.player.blocking=false;
    game.player.stamina=100;
    Enemy alignedSpire{{0,0.3f,0},100,100,0,0,true,true};
    alignedSpire.variant=4;
    alignedSpire.phase=BossPhase::Strike;
    alignedSpire.phaseTime=0.23f;
    alignedSpire.strikeDirection={0,0,1};
    alignedSpire.strikeOrigin=alignedSpire.position;
    game.enemies.push_back(alignedSpire);
    UpdatePursuit(game,0.01f);
    assert(near(game.player.health,82.0f));
    assert(game.enemies.front().phase==BossPhase::Recover);

    game.enemies.clear();
    game.player.position={1.1f,0.3f,7.0f};
    game.player.health=100;
    game.player.invulnerable=0;
    Enemy dodgedSpire{{0,0.3f,0},100,100,0,0,true,true};
    dodgedSpire.variant=4;
    dodgedSpire.phase=BossPhase::Strike;
    dodgedSpire.phaseTime=0.23f;
    dodgedSpire.strikeDirection={0,0,1};
    dodgedSpire.strikeOrigin=dodgedSpire.position;
    game.enemies.push_back(dodgedSpire);
    UpdatePursuit(game,0.01f);
    assert(near(game.player.health,100.0f));
    assert(game.enemies.front().phase==BossPhase::Recover);

    // Telegraph and damage share the same half-width for lane-based boss strikes.
    assert(near(BossStrikeHalfWidth(0,8.0f),3.04f));
    assert(near(BossStrikeHalfWidth(1,8.0f),0.9f));
    assert(near(BossStrikeHalfWidth(4,8.0f),0.9f));
    assert(near(BossStrikeHalfWidth(2,8.0f),0.0f));
    const int laneVariants[2]={1,4};
    for(int variant : laneVariants) {
        const float expectedDamage=variant==1?30.0f:18.0f;
        for(int edge=0;edge<2;edge++) {
            game.platforms.clear();
            game.enemies.clear();
            game.player.position={edge==0?0.89f:0.91f,0.3f,2.0f};
            game.player.health=100;
            game.player.invulnerable=0;
            game.player.blocking=false;
            Enemy laneBoss{{0,0.3f,0},100,100,0,0,true,true};
            laneBoss.variant=variant;
            laneBoss.phase=BossPhase::Strike;
            laneBoss.phaseTime=1.0f;
            laneBoss.strikeDirection={0,0,1};
            laneBoss.strikeOrigin=laneBoss.position;
            game.enemies.push_back(laneBoss);
            UpdatePursuit(game,0.01f);
            assert(near(game.player.health,edge==0?100.0f-expectedDamage:100.0f));
            assert(game.enemies.front().phase==BossPhase::Recover);
        }
    }

    // Each city boss has a distinct attack pattern, range and damage profile.
    const float bossDistances[5]={3.5f,2.2f,5.0f,4.0f,7.0f};
    const float bossDamage[5]={24,30,32,28,18};
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
        boss.strikeDirection={0,0,-1};
        boss.strikeOrigin=boss.position;
        game.enemies.push_back(boss);
        UpdatePursuit(game,0.05f);
        assert(near(game.player.health,100.0f-bossDamage[variant]));
        assert(game.enemies.front().phase==BossPhase::Recover);
    }

    // Foundry Colossus and Imperial Regent use radial strikes that can be dodged beyond their warning ring.
    const int radialBossVariants[2]={2,3};
    const float radialBossDamage[2]={32.0f,28.0f};
    for(int i=0;i<2;i++) {
        const int variant=radialBossVariants[i];
        const float radius=BossStrikeRange(variant);
        for(int evade=0;evade<2;evade++) {
            game.platforms.clear();
            game.enemies.clear();
            game.player.position={0,0.3f,radius*1.05f};
            game.player.health=100;
            game.player.stamina=100;
            game.player.invulnerable=0;
            game.player.blocking=false;
            Enemy radialBoss{{0,0.3f,0},100,100,0,0,true,true};
            radialBoss.variant=variant;
            radialBoss.phase=BossPhase::Hunt;
            game.enemies.push_back(radialBoss);

            // Enter the warning just outside the blast ring, then step into or out of it.
            UpdatePursuit(game,0.016f);
            assert(game.enemies.front().phase==BossPhase::Windup);
            game.player.position.z=evade==0?radius*0.85f:radius+0.1f;
            UpdatePursuit(game,1.0f);
            assert(game.enemies.front().phase==BossPhase::Strike);
            UpdatePursuit(game,0.5f);
            assert(game.enemies.front().phase==BossPhase::Recover);
            if(evade==0) assert(near(game.player.health,100.0f-radialBossDamage[i]));
            else assert(near(game.player.health,100.0f));
        }
    }

    // Bosses enter an enrage phase below half health and deal increased strike damage.
    const float enragedBossDamage[5]={26.4f,33.0f,35.2f,30.8f,19.8f};
    for(int variant=0;variant<5;variant++) {
        game.enemies.clear();
        game.player.position={0,0.3f,0};
        game.player.health=100;
        game.player.invulnerable=0;
        game.player.blocking=false;
        game.player.stamina=100;
        Enemy boss{{0,0.3f,bossDistances[variant]},50,100,2.5f,0,true,true};
        boss.variant=variant;
        boss.phase=BossPhase::Strike;
        boss.phaseTime=1.0f;
        boss.strikeDirection={0,0,-1};
        boss.strikeOrigin=boss.position;
        game.enemies.push_back(boss);
        UpdatePursuit(game,0.05f);
        assert(near(game.player.health,100.0f-enragedBossDamage[variant]));
        assert(game.enemies.front().phase==BossPhase::Recover);
    }

    // An enraged boss completes its windup sooner than the same boss at full health.
    game.enemies.clear();
    game.player.position={0,0.3f,0};
    Enemy enragedWindup{{0,0.3f,3.5f},50,100,2.5f,0,true,true};
    enragedWindup.variant=0;
    enragedWindup.phase=BossPhase::Windup;
    enragedWindup.phaseTime=BossWindupDuration(0,true)-0.01f;
    game.enemies.push_back(enragedWindup);
    UpdatePursuit(game,0.02f);
    assert(game.enemies.front().phase==BossPhase::Strike);

    // The Gilded Charger advances rapidly during its strike before impact.
    game.enemies.clear();
    game.player.position={0,0.3f,0};
    game.player.health=100;
    game.player.invulnerable=0;
    Enemy charger{{0,0.3f,2.2f},100,100,3.0f,0,true,true};
    charger.variant=1;
    charger.phase=BossPhase::Strike;
    charger.strikeDirection={0,0,-1};
    charger.strikeOrigin=charger.position;
    game.enemies.push_back(charger);
    for(int i=0;i<7;i++) UpdatePursuit(game,0.04f);
    assert(game.enemies.front().position.z<0.0f);
    assert(near(game.player.health,100.0f));
    UpdatePursuit(game,0.05f);
    assert(near(game.player.health,70.0f));

    // A Gilded Charger cannot charge through a solid rooftop wall.
    game.platforms.clear();
    game.platforms.push_back({{0,1.5f,2.0f},{4,3.0f,0.4f},GRAY});
    game.enemies.clear();
    game.player.position={0,0.3f,6.0f};
    Enemy coveredCharger{{0,0.3f,0},100,100,3.0f,0,true,true};
    coveredCharger.variant=1;
    coveredCharger.phase=BossPhase::Strike;
    coveredCharger.strikeDirection={0,0,1};
    coveredCharger.strikeOrigin=coveredCharger.position;
    game.enemies.push_back(coveredCharger);
    for(int i=0;i<4;i++) UpdatePursuit(game,0.05f);
    assert(game.enemies.front().position.z<1.5f);
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
    coveredBoss.strikeDirection={0,0,-1};
    coveredBoss.strikeOrigin=coveredBoss.position;
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
    exposedBoss.strikeDirection={0,0,-1};
    exposedBoss.strikeOrigin=exposedBoss.position;
    game.enemies.clear();
    game.enemies.push_back(exposedBoss);
    UpdatePursuit(game,0.05f);
    assert(near(game.player.health,76.0f));
    game.platforms.clear();

    // Every playable class can defeat all five bosses with its one fixed weapon.
    const PlayerClass campaignClasses[3]={
        PlayerClass::Vanguard,PlayerClass::Skirmisher,PlayerClass::Warden
    };
    const Weapon campaignWeapons[3]={
        Weapon::Sword,Weapon::Bow,Weapon::Shield
    };
    for(int klass=0;klass<3;klass++) {
        GameState campaign;
        campaign.unlockedCity=0;
        campaign.finished=false;
        campaign.player.playerClass=campaignClasses[klass];
        campaign.player.weapon=ClassWeapon(campaignClasses[klass]);
        campaign.player.health=ClassMaxHealth(campaignClasses[klass]);

        assert(campaign.player.weapon==campaignWeapons[klass]);
        for(int city=0;city<5;city++) {
            campaign.city=city;
            BuildCity(campaign);
            campaign.platforms.clear();
            campaign.enemies.clear();
            campaign.player.playerClass=campaignClasses[klass];
            campaign.player.weapon=ClassWeapon(campaignClasses[klass]);
            campaign.player.health=ClassMaxHealth(campaignClasses[klass]);
            campaign.player.position=QuestTarget(city,3);
            campaign.player.position.z-=1.5f;
            campaign.player.facing={0,0,1};

            SetQuestProgress(campaign,3);
            assert(campaign.bossUnlocked);
            assert(campaign.enemies.size()==1);
            assert(campaign.enemies.front().boss);
            assert(campaign.enemies.front().variant==city);
            campaign.enemies.front().hp=10.0f;
            campaign.enemies.front().maxHp=10.0f;

            Controls bossHit{};
            bossHit.attack=true;
            UpdateCombat(campaign,bossHit,0.016f);
            if(campaignWeapons[klass]==Weapon::Bow) {
                assert(campaign.projectiles.size()==1);
                UpdateCombat(campaign,Controls{},0.1f);
            }

            assert(campaign.player.weapon==campaignWeapons[klass]);
            assert(std::count_if(campaign.enemies.begin(),campaign.enemies.end(),
                [](const Enemy& enemy){return enemy.boss&&enemy.alive;})==0);
            if(city<4) {
                assert(campaign.city==city+1);
                assert(campaign.unlockedCity>=city+1);
                assert(campaign.quest==0);
                assert(!campaign.finished);
            } else {
                assert(campaign.city==4);
                assert(campaign.unlockedCity==4);
                assert(campaign.finished);
            }
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
    GameState checkpointGame;
    Controls idle{};
    for(int city=0;city<5;city++) {
        checkpointGame.city=city;
        BuildCity(checkpointGame);
        for(int quest=0;quest<4;quest++) {
            SetQuestProgress(checkpointGame,quest);
            for(int klass=0;klass<3;klass++) {
                Player& player=checkpointGame.player;
                player.playerClass=(PlayerClass)klass;
                player.health=0;
                player.stamina=0;
                player.position={-30,12,30};
                player.grounded=false;
                player.wallRunning=true;
                player.dashTime=0.2f;
                player.vaulting=true;
                player.ledgeGrabbing=true;
                player.climbing=true;
                player.blocking=true;
                player.bowAiming=true;
                checkpointGame.beaconHoldTime=1.0f;
                int xpBefore=player.xp, levelBefore=player.level;
                int defeatedBefore=checkpointGame.defeated;
                UpdateCombat(checkpointGame,idle,0.016f);
                Vector3 expected=quest>0?QuestTarget(city,quest-1):Vector3{0,0.3f,0};
                assert(near(player.health,classHealth[klass]));
                assert(near(player.stamina,classStamina[klass]));
                assert(near(player.position.x,expected.x)&&near(player.position.y,expected.y)&&near(player.position.z,expected.z));
                assert(player.grounded&&!player.wallRunning&&!player.vaulting&&!player.ledgeGrabbing&&!player.climbing);
                assert(!player.blocking&&!player.bowAiming&&near(player.dashTime,0));
                assert(near(player.invulnerable,1.2f));
                assert(near(checkpointGame.beaconHoldTime,0.0f));
                assert(checkpointGame.city==city&&checkpointGame.quest==quest);
                assert(player.xp==xpBefore&&player.level==levelBefore&&checkpointGame.defeated==defeatedBefore);
                assert(near(checkpointGame.questTime,0));
                Vector3 target=QuestTarget(city,quest);
                float fx=target.x-expected.x,fz=target.z-expected.z;
                float fl=std::sqrt(fx*fx+fz*fz);
                assert(fl<0.001f || (std::abs(player.facing.x-fx/fl)<0.02f&&std::abs(player.facing.z-fz/fl)<0.02f));
                if(quest==3) assert(std::count_if(checkpointGame.enemies.begin(),checkpointGame.enemies.end(),[](const Enemy& e){return e.alive&&e.boss;})==1);
            }
        }
    }

    // Reinforcement spawns stay in bounds and out of solid geometry.
    GameState spawnGame;
    spawnGame.platforms.push_back({{0,-0.3f,0},{120,0.6f,120},GRAY});
    Vector3 spawn{};
    assert(FindReinforcementSpawn(spawnGame,{0,0.3f,0},spawn));
    assert(near(std::sqrt(spawn.x*spawn.x+spawn.z*spawn.z),8.0f));
    Vector3 firstSpawn=spawn;
    spawnGame.platforms.push_back({{firstSpawn.x,3.0f,firstSpawn.z},{5,6,5},GRAY});
    assert(FindReinforcementSpawn(spawnGame,{0,0.3f,0},spawn));
    assert(std::sqrt((spawn.x-firstSpawn.x)*(spawn.x-firstSpawn.x)+(spawn.z-firstSpawn.z)*(spawn.z-firstSpawn.z))>2.0f);

    // Existing enemies are not overlapped by a new reinforcement.
    GameState crowdedSpawn=spawnGame;
    crowdedSpawn.enemies.push_back({spawn,40,40,2.5f,0,false,true});
    Vector3 nextSpawn{};
    assert(FindReinforcementSpawn(crowdedSpawn,{0,0.3f,0},nextSpawn));
    assert(std::sqrt((nextSpawn.x-spawn.x)*(nextSpawn.x-spawn.x)+(nextSpawn.z-spawn.z)*(nextSpawn.z-spawn.z))>=1.7f);

    // Edge-of-map players still receive a clear, in-bounds spawn.
    Vector3 edgeSpawn{};
    assert(FindReinforcementSpawn(spawnGame,{48.0f,0.3f,40.0f},edgeSpawn));
    assert(edgeSpawn.x>=-48.5f && edgeSpawn.x<=48.5f);
    assert(edgeSpawn.z>=-34.5f && edgeSpawn.z<=41.5f);

    // If every candidate is obstructed, the helper declines to spawn an enemy.
    GameState closedSpawn;
    closedSpawn.platforms.push_back({{0,-0.3f,0},{120,0.6f,120},GRAY});
    closedSpawn.platforms.push_back({{0,3.0f,0},{120,6.0f,120},GRAY});
    assert(!FindReinforcementSpawn(closedSpawn,{0,0.3f,0},spawn));

    std::cout << "PASS: city setup, mixed beacon objectives, hold resets, XP/skills, movement, combat, and boss progression.\\n";
    return 0;
}
