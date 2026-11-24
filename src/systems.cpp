#include "game.hpp"
#include "raymath.h"
#include <algorithm>
#include <cmath>

static float cameraYaw=0.0f, cameraPitch=-0.18f, cameraManualLookTime=0.0f;

static void SetCameraPose(Vector3 player,Camera3D& camera) {
    Vector3 forward={std::sin(cameraYaw),0,std::cos(cameraYaw)};
    Vector3 right={forward.z,0,-forward.x};
    camera.target={player.x+forward.x*4.0f,player.y+1.2f+cameraPitch*5.0f,player.z+forward.z*4.0f};
    camera.position={player.x-forward.x*8.0f+right.x*1.6f,player.y+4.0f,player.z-forward.z*8.0f+right.z*1.6f};
}

void ResetCamera(const Player& p,Camera3D& camera) {
    Vector3 facing=p.facing; facing.y=0;
    if(Vector3Length(facing)<0.001f) facing={0,0,1};
    else facing=Vector3Normalize(facing);
    cameraYaw=std::atan2(facing.x,facing.z);
    cameraPitch=-0.18f;
    cameraManualLookTime=0.0f;
    SetCameraPose(p.position,camera);
}

static bool Inside(Vector3 p,const Platform& b) {
    return std::abs(p.x-b.position.x)<=b.size.x*0.5f &&
           std::abs(p.z-b.position.z)<=b.size.z*0.5f;
}
static float Distance(Vector3 a,Vector3 b) { return Vector3Distance(a,b); }

static bool FacesAttacker(const Player& player,Vector3 attacker) {
    Vector3 forward=player.facing; forward.y=0;
    Vector3 toward=Vector3Subtract(attacker,player.position); toward.y=0;
    float forwardLength=Vector3Length(forward),towardLength=Vector3Length(toward);
    if(towardLength<0.001f) return true;
    if(forwardLength<0.001f) return false;
    return Vector3DotProduct(Vector3Scale(forward,1.0f/forwardLength),
                             Vector3Scale(toward,1.0f/towardLength))>=0.25f;
}

static bool SegmentSphere(Vector3 from,Vector3 to,Vector3 center,float radius,float& hitTime) {
    Vector3 delta=Vector3Subtract(to,from);
    Vector3 offset=Vector3Subtract(from,center);
    float a=Vector3DotProduct(delta,delta);
    if(a<0.000001f) return false;
    float b=2.0f*Vector3DotProduct(offset,delta);
    float c=Vector3DotProduct(offset,offset)-radius*radius;
    if(c<=0.0f) { hitTime=0.0f; return true; }
    float discriminant=b*b-4.0f*a*c;
    if(discriminant<0.0f) return false;
    float t=(-b-std::sqrt(discriminant))/(2.0f*a);
    if(t<0.0f) t=(-b+std::sqrt(discriminant))/(2.0f*a);
    if(t<0.0f||t>1.0f) return false;
    hitTime=t;
    return true;
}
static bool SegmentPlatform(Vector3 from,Vector3 to,const Platform& p,float& hitTime,float clearance=0.12f) {
    Vector3 low=Vector3Subtract(p.position,Vector3Scale(p.size,0.5f));
    Vector3 high=Vector3Add(p.position,Vector3Scale(p.size,0.5f));
    Vector3 delta=Vector3Subtract(to,from);
    float enter=0.0f,exit=1.0f;
    const float start[3]={from.x,from.y,from.z};
    const float step[3]={delta.x,delta.y,delta.z};
    const float minimum[3]={low.x-clearance,low.y-clearance,low.z-clearance};
    const float maximum[3]={high.x+clearance,high.y+clearance,high.z+clearance};
    for(int axis=0;axis<3;axis++) {
        if(std::abs(step[axis])<0.000001f) {
            if(start[axis]<minimum[axis]||start[axis]>maximum[axis]) return false;
            continue;
        }
        float a=(minimum[axis]-start[axis])/step[axis];
        float b=(maximum[axis]-start[axis])/step[axis];
        if(a>b) std::swap(a,b);
        enter=std::max(enter,a);
        exit=std::min(exit,b);
        if(enter>exit) return false;
    }
    if(enter<0.0f||enter>1.0f) return false;
    hitTime=enter;
    return true;
}
static bool HasLineOfSight(const GameState& g,Vector3 from,Vector3 to,float clearance=0.12f) {
    from.y+=1.0f;
    to.y+=1.0f;
    for(const auto& platform:g.platforms) {
        float hitTime=0.0f;
        if(SegmentPlatform(from,to,platform,hitTime,clearance)) return false;
    }
    return true;
}

static bool FindPursuitWaypoint(const GameState& g,const Enemy& e,Vector3 target,Vector3& waypoint) {
    const float bodyRadius=e.boss?0.62f:0.42f;
    Vector3 from=e.position; from.y+=1.0f;
    Vector3 to=target; to.y+=1.0f;
    const Platform* blocker=nullptr;
    float nearest=1.0f;
    for(const auto& platform:g.platforms) {
        float hit=0.0f;
        if(SegmentPlatform(from,to,platform,hit,bodyRadius)&&hit<nearest) {
            nearest=hit;
            blocker=&platform;
        }
    }
    if(!blocker) return false;

    float clearance=bodyRadius+0.72f;
    float minX=blocker->position.x-blocker->size.x*0.5f-clearance;
    float maxX=blocker->position.x+blocker->size.x*0.5f+clearance;
    float minZ=blocker->position.z-blocker->size.z*0.5f-clearance;
    float maxZ=blocker->position.z+blocker->size.z*0.5f+clearance;
    Vector3 corners[4]={
        {minX,e.position.y,minZ},{maxX,e.position.y,minZ},
        {minX,e.position.y,maxZ},{maxX,e.position.y,maxZ}
    };
    float best=1000000.0f;
    bool found=false;
    for(Vector3 corner:corners) {
        if(!HasLineOfSight(g,e.position,corner)) continue;
        Vector3 cornerRay=corner; cornerRay.y+=1.0f;
        float blockedByCurrent=0.0f;
        if(SegmentPlatform(cornerRay,to,*blocker,blockedByCurrent)) continue;
        float route=Distance(e.position,corner)+Distance(corner,target);
        if(Distance(e.position,corner)<0.65f) continue;
        if(route<best) { best=route; waypoint=corner; found=true; }
    }
    return found;
}
static bool OverlapsWall(Vector3 feet,const Platform& b,float radius=0.34f,float height=1.72f) {
    bool x=feet.x+radius>b.position.x-b.size.x*0.5f && feet.x-radius<b.position.x+b.size.x*0.5f;
    bool z=feet.z+radius>b.position.z-b.size.z*0.5f && feet.z-radius<b.position.z+b.size.z*0.5f;
    bool y=feet.y+height>b.position.y-b.size.y*0.5f+0.05f && feet.y<b.position.y+b.size.y*0.5f-0.05f;
    return x&&z&&y;
}
static void ConstrainBossToArena(const GameState& g,Enemy& e) {
    if(!e.boss||!g.bossArena.active) return;
    constexpr float radius=0.62f;
    const BossArena& arena=g.bossArena;
    const float minX=arena.center.x-arena.halfExtents.x+radius;
    const float maxX=arena.center.x+arena.halfExtents.x-radius;
    const float minZ=arena.center.z-arena.halfExtents.y+radius;
    const float maxZ=arena.center.z+arena.halfExtents.y-radius;
    const float x=std::clamp(e.position.x,minX,maxX);
    const float z=std::clamp(e.position.z,minZ,maxZ);
    if(x!=e.position.x||z!=e.position.z||e.position.y!=arena.floorY)
        e.hasPursuitWaypoint=false;
    e.position.x=x;
    e.position.y=arena.floorY;
    e.position.z=z;
}

static void MoveEnemy(GameState& g,Enemy& e,Vector3 delta) {
    const float radius=e.boss?0.62f:0.42f;
    const float height=e.boss?2.2f:1.72f;
    float oldX=e.position.x;
    e.position.x+=delta.x;
    for(const auto& platform:g.platforms) if(OverlapsWall(e.position,platform,radius,height)) {
        e.position.x=oldX;
        break;
    }
    float oldZ=e.position.z;
    e.position.z+=delta.z;
    for(const auto& platform:g.platforms) if(OverlapsWall(e.position,platform,radius,height)) {
        e.position.z=oldZ;
        break;
    }
    ConstrainBossToArena(g,e);
}
static bool NearWall(Vector3 p,const Platform& b,Vector3& away) {
    float dx=std::abs(std::abs(p.x-b.position.x)-b.size.x*0.5f);
    float dz=std::abs(std::abs(p.z-b.position.z)-b.size.z*0.5f);
    bool zSpan=std::abs(p.z-b.position.z)<b.size.z*0.5f+0.45f;
    bool xSpan=std::abs(p.x-b.position.x)<b.size.x*0.5f+0.45f;
    if(dx<0.55f&&zSpan) { away={p.x>=b.position.x?1.0f:-1.0f,0,0}; return true; }
    if(dz<0.55f&&xSpan) { away={0,0,p.z>=b.position.z?1.0f:-1.0f}; return true; }
    return false;
}

static bool FindLedge(const GameState& g,Vector3 p,Vector3 facing,
                     Vector3& normal,Vector3& hang,Vector3& target) {
    for(const auto& b:g.platforms) {
        Vector3 away{};
        if(!NearWall(p,b,away)) continue;
        if(Vector3DotProduct(facing,Vector3Scale(away,-1.0f))<0.65f) continue;
        float top=b.position.y+b.size.y*0.5f;
        float handGap=top-p.y;
        if(handGap<0.85f||handGap>1.8f) continue;

        normal=away;
        hang=p;
        hang.y=top-1.55f;
        target=hang;
        target.y=top;
        target.x-=away.x*0.75f;
        target.z-=away.z*0.75f;
        target.x=std::clamp(target.x,b.position.x-b.size.x*0.5f+0.55f,
                                  b.position.x+b.size.x*0.5f-0.55f);
        target.z=std::clamp(target.z,b.position.z-b.size.z*0.5f+0.55f,
                                  b.position.z+b.size.z*0.5f-0.55f);
        return true;
    }
    return false;
}

void UpdateMovement(GameState& g,const Controls& c,Camera3D& camera,float dt) {
    Player& p=g.player;
    bool wasGrounded=p.grounded;
    p.landingTime=std::max(0.0f,p.landingTime-dt);
    if(p.landingTime<=0) p.landingRoll=false;
    p.invulnerable=std::max(0.0f,p.invulnerable-dt);
    p.dashCooldown=std::max(0.0f,p.dashCooldown-dt);
    p.dashTime=std::max(0.0f,p.dashTime-dt);
    p.vaultTime=std::max(0.0f,p.vaultTime-dt);
    p.vaulting=p.vaultTime>0;

    const bool manualLook=std::abs(c.lookX)>0.001f||std::abs(c.lookY)>0.001f;
    if(manualLook) cameraManualLookTime=1.2f;
    else cameraManualLookTime=std::max(0.0f,cameraManualLookTime-dt);
    cameraYaw-=c.lookX*0.008f;
    cameraPitch=std::clamp(cameraPitch-c.lookY*0.008f,-0.48f,0.42f);
    Vector3 forward={std::sin(cameraYaw),0,std::cos(cameraYaw)};
    Vector3 right={forward.z,0,-forward.x};
    Vector3 direction=Vector3Add(Vector3Scale(right,c.move.x),Vector3Scale(forward,c.move.y));
    if(Vector3Length(direction)>1.0f) direction=Vector3Normalize(direction);
    const bool moving=Vector3Length(direction)>0.05f;
    if(moving) p.facing=Vector3Normalize(direction);
    else if(c.aim) p.facing=forward;
    else if(cameraManualLookTime<=0.0f&&!p.ledgeGrabbing&&!p.climbing) {
        Vector3 facing=p.facing; facing.y=0;
        if(Vector3Length(facing)>0.001f) {
            const float targetYaw=std::atan2(facing.x,facing.z);
            const float yawDelta=std::atan2(std::sin(targetYaw-cameraYaw),std::cos(targetYaw-cameraYaw));
            cameraYaw+=yawDelta*(1.0f-std::exp(-4.0f*dt));
            forward={std::sin(cameraYaw),0,std::cos(cameraYaw)};
            right={forward.z,0,-forward.x};
        }
    }
    auto updateCamera=[&]() { SetCameraPose(p.position,camera); };

    if(p.ledgeGrabbing) {
        p.facing=Vector3Scale(p.ledgeNormal,-1.0f);
        if(c.jump) {
            p.ledgeGrabbing=false;
            p.climbing=true;
            p.climbTime=0.0f;
        } else if(c.slide||c.move.y<-0.55f) {
            p.ledgeGrabbing=false;
            p.position=Vector3Add(p.position,Vector3Scale(p.ledgeNormal,0.42f));
            p.velocity=Vector3Add(Vector3Scale(p.ledgeNormal,2.0f),Vector3{0,-1.5f,0});
            p.grounded=false;
            p.ledgeNormal={};
            p.ledgeHangPosition={};
            p.ledgeTargetPosition={};
        } else {
            p.position=p.ledgeHangPosition;
            p.velocity={0,0,0};
            p.moveVelocity={0,0};
            p.grounded=false;
            p.wallRunning=false;
            p.vaulting=false;
            updateCamera();
            return;
        }
    }

    if(p.climbing) {
        constexpr float climbDuration=0.42f;
        p.climbTime=std::min(climbDuration,p.climbTime+dt);
        float t=p.climbTime/climbDuration;
        float eased=t*t*(3.0f-2.0f*t);
        p.position=Vector3Add(p.ledgeHangPosition,
            Vector3Scale(Vector3Subtract(p.ledgeTargetPosition,p.ledgeHangPosition),eased));
        p.facing=Vector3Scale(p.ledgeNormal,-1.0f);
        p.velocity={0,0,0};
        p.moveVelocity={0,0};
        p.grounded=false;
        p.vaulting=true;
        if(t>=1.0f) {
            p.position=p.ledgeTargetPosition;
            p.climbing=false;
            p.climbTime=0.0f;
            p.grounded=true;
            p.vaulting=false;
            p.ledgeNormal={};
            p.ledgeHangPosition={};
            p.ledgeTargetPosition={};
        }
        updateCamera();
        return;
    }

    float speed=p.playerClass==PlayerClass::Skirmisher?7.5f:6.0f;
    float sprintCost=18.0f*dt;
    if(c.sprint&&p.stamina>=sprintCost) { speed*=1.5f; p.stamina-=sprintCost; }
    else if(!c.sprint) p.stamina=std::min(ClassMaxStamina(p.playerClass),p.stamina+13*dt);
    if(c.slide&&p.grounded) speed*=1.22f;
    if(c.skill&&p.escapeSkill>0&&p.stamina>=20&&p.dashCooldown<=0) {
        p.dashTime=0.24f; p.dashCooldown=1.8f; p.stamina-=20;
        if(Vector3Length(direction)<0.05f) direction=p.facing;
    }
    if(p.dashTime>0) speed*=2.7f;

    // Keep player-driven locomotion separate from physics impulses such as vaults
    // and knockback. Ground movement accelerates and brakes responsively; air
    // steering is weaker and preserves more horizontal momentum.
    const Vector2 targetMoveVelocity={direction.x*speed,direction.z*speed};
    const float targetSpeed=std::sqrt(targetMoveVelocity.x*targetMoveVelocity.x+
                                      targetMoveVelocity.y*targetMoveVelocity.y);
    const float currentDeltaX=targetMoveVelocity.x-p.moveVelocity.x;
    const float currentDeltaZ=targetMoveVelocity.y-p.moveVelocity.y;
    const float deltaLength=std::sqrt(currentDeltaX*currentDeltaX+currentDeltaZ*currentDeltaZ);
    const float rate=targetSpeed>0.05f
        ? (p.grounded?34.0f:16.0f)
        : (p.grounded?28.0f:3.5f);
    const float maxChange=rate*dt;
    if(deltaLength<=maxChange) p.moveVelocity=targetMoveVelocity;
    else if(deltaLength>0.0001f) {
        const float scale=maxChange/deltaLength;
        p.moveVelocity.x+=currentDeltaX*scale;
        p.moveVelocity.y+=currentDeltaZ*scale;
    }

    auto moveAxis=[&](float amount,bool xAxis) {
        if(std::abs(amount)<0.000001f) return false;
        int steps=std::max(1,(int)std::ceil(std::abs(amount)/0.12f));
        float step=amount/steps;
        for(int i=0;i<steps;i++) {
            if(xAxis) p.position.x+=step;
            else p.position.z+=step;
            bool blocked=false;
            for(const auto& b:g.platforms) if(OverlapsWall(p.position,b)) { blocked=true; break; }
            if(blocked) {
                if(xAxis) { p.position.x-=step; p.moveVelocity.x=0; p.velocity.x=0; }
                else { p.position.z-=step; p.moveVelocity.y=0; p.velocity.z=0; }
                return true;
            }
        }
        return false;
    };
    bool wallContact=moveAxis((p.moveVelocity.x+p.velocity.x)*dt,true);
    wallContact=moveAxis((p.moveVelocity.y+p.velocity.z)*dt,false)||wallContact;
    p.velocity.x*=std::max(0.0f,1.0f-9.0f*dt);
    p.velocity.z*=std::max(0.0f,1.0f-9.0f*dt);

    if(wallContact&&!p.grounded&&!p.vaulting&&!p.ledgeGrabbing&&!p.climbing) {
        Vector3 normal{},hang{},target{};
        if(FindLedge(g,p.position,p.facing,normal,hang,target)) {
            p.ledgeNormal=normal;
            p.ledgeHangPosition=hang;
            p.ledgeTargetPosition=target;
            p.position=hang;
            p.velocity={0,0,0};
            p.moveVelocity={0,0};
            p.grounded=false;
            p.ledgeGrabbing=true;
            p.vaulting=false;
            p.vaultTime=0;
            p.wallRunning=false;
            updateCamera();
            return;
        }
    }

    Vector3 wallAway{};
    p.wallRunning=false;
    if(!p.grounded&&c.sprint&&p.stamina>0) {
        for(const auto& b:g.platforms) {
            if(NearWall(p.position,b,wallAway)&&p.position.y<b.position.y+b.size.y*0.5f+2.0f) {
                p.wallRunning=true; break;
            }
        }
    }
    if(p.wallRunning) {
        p.velocity.y=std::max(p.velocity.y,-2.8f);
        p.stamina=std::max(0.0f,p.stamina-8.0f*dt);
    }
    if(c.jump&&p.wallRunning) {
        p.velocity.y=9.5f;
        p.velocity.x=wallAway.x*5.5f; p.velocity.z=wallAway.z*5.5f;
        p.grounded=false; p.wallRunning=false;
    } else if(c.jump&&p.grounded&&p.stamina>=8) {
        bool vaulted=false;
        Vector3 wallAway{};
        for(const auto& b:g.platforms) {
            float rise=b.position.y+b.size.y*0.5f-p.position.y;
            bool facingWall=NearWall(p.position,b,wallAway)&&
                Vector3DotProduct(p.facing,Vector3Scale(wallAway,-1.0f))>0.25f;
            if(facingWall&&rise>0.25f&&rise<=1.55f) {
                p.velocity.y=13.0f;
                p.velocity.x=p.facing.x*6.0f;
                p.velocity.z=p.facing.z*6.0f;
                p.vaultTime=0.46f;
                p.vaulting=true;
                p.stamina-=10;
                vaulted=true;
                break;
            }
        }
        if(!vaulted) {
            p.velocity.y=p.playerClass==PlayerClass::Skirmisher?10.0f:8.8f;
            p.stamina-=8;
            for(const auto& b:g.platforms) {
                if(NearWall(p.position,b,wallAway)&&Vector3DotProduct(p.facing,Vector3Scale(wallAway,-1.0f))>0.25f) {
                    p.velocity.x=p.facing.x*5.0f; p.velocity.z=p.facing.z*5.0f;
                    break;
                }
            }
        }
        p.grounded=false;
    }

    p.velocity.y-=22.0f*dt;
    float previousY=p.position.y;
    p.position.y+=p.velocity.y*dt;
    float floor=0.0f;
    for(const auto& b:g.platforms) {
        float top=b.position.y+b.size.y*0.5f;
        if(Inside(p.position,b)&&previousY>=top-0.08f&&p.position.y<=top&&p.velocity.y<=0)
            floor=std::max(floor,top);
    }
    if(p.position.y<floor) {
        p.position.y=floor;
        if(!wasGrounded&&p.velocity.y<-4.0f) {
            p.landingTime=0.28f;
            p.landingRoll=c.slide&&p.velocity.y<-7.0f;
            if(p.landingRoll) {
                p.velocity.x+=p.facing.x*2.8f;
                p.velocity.z+=p.facing.z*2.8f;
            }
        }
        p.velocity.y=0;
        p.grounded=true;
    } else p.grounded=false;

    p.position.x=std::clamp(p.position.x,-49.0f,49.0f);
    p.position.z=std::clamp(p.position.z,-35.0f,42.0f);
    updateCamera();
}

void UpdatePursuit(GameState& g,float dt) {
    Player& p=g.player;
    int nearby=0;
    for(auto& e:g.enemies) {
        if(!e.alive) continue;
        ConstrainBossToArena(g,e);
        float d=Distance(e.position,p.position);
        e.hitTime=std::max(0.0f,e.hitTime-dt);
        e.stunTime=std::max(0.0f,e.stunTime-dt);
        e.cooldown=std::max(0.0f,e.cooldown-dt);
        if(d<25) nearby++;
        if(!e.boss&&(d>1.45f||e.stunTime>0)) e.attackWindup=0;
        if(e.stunTime>0) {
            e.hasPursuitWaypoint=false;
            e.attackWindup=0;
            MoveEnemy(g,e,Vector3Scale(e.knockback,dt));
            e.knockback=Vector3Scale(e.knockback,std::max(0.0f,1.0f-8.0f*dt));
        } else if(d<24) {
            Vector3 to=Vector3Subtract(p.position,e.position); to.y=0;
            float horizontal=Vector3Length(to);
            if(d>1.45f&&horizontal>0.001f&&(!e.boss||e.phase==BossPhase::Dormant||e.phase==BossPhase::Hunt)) {
                const float bodyRadius=e.boss?0.62f:0.42f;
                if(HasLineOfSight(g,e.position,p.position,bodyRadius)) e.hasPursuitWaypoint=false;
                else if(!e.hasPursuitWaypoint) {
                    Vector3 waypoint{};
                    if(FindPursuitWaypoint(g,e,p.position,waypoint)) {
                        e.pursuitWaypoint=waypoint;
                        e.hasPursuitWaypoint=true;
                    }
                }
                Vector3 destination=e.hasPursuitWaypoint?e.pursuitWaypoint:p.position;
                Vector3 route=Vector3Subtract(destination,e.position); route.y=0;
                if(Vector3Length(route)>0.001f)
                    MoveEnemy(g,e,Vector3Scale(Vector3Normalize(route),e.speed*dt));
                if(e.hasPursuitWaypoint&&Distance(e.position,e.pursuitWaypoint)<0.45f)
                    e.hasPursuitWaypoint=false;
            } else if(!e.boss) {
                bool canStrike=d<=1.45f&&HasLineOfSight(g,e.position,p.position);
                if(!canStrike||e.cooldown>0) e.attackWindup=0;
                else if(e.attackWindup<=0) e.attackWindup=0.42f;
                else {
                    e.attackWindup=std::max(0.0f,e.attackWindup-dt);
                    if(e.attackWindup<=0) {
                        e.cooldown=1.0f;
                        float guardCost=p.playerClass==PlayerClass::Warden?5.0f:8.0f;
                        if(p.blocking&&FacesAttacker(p,e.position)&&p.stamina>=guardCost) p.stamina-=guardCost;
                        else if(p.invulnerable<=0) { p.health-=11; p.invulnerable=0.48f; }
                    }
                }
            }
        }
        if(e.boss) {
            static const float strike[5]={0.35f,0.32f,0.38f,0.48f,0.22f};
            static const float recover[5]={0.8f,0.9f,1.0f,0.65f,0.72f};
            static const float damage[5]={24,30,32,28,18};
            int v=std::clamp(e.variant,0,4);
            const float attackRange=BossStrikeRange(v);
            bool enraged=e.hp<=e.maxHp*0.5f;
            float strikeDuration=strike[v]*(enraged?0.85f:1.0f);
            float recoverDuration=recover[v]*(enraged?0.85f:1.0f);
            e.phaseTime+=dt;
            if(e.phase==BossPhase::Dormant&&d<20) e.phase=BossPhase::Hunt;
            else if(e.phase==BossPhase::Hunt&&d<attackRange*1.15f) {
                Vector3 aim=Vector3Subtract(p.position,e.position); aim.y=0;
                if(Vector3Length(aim)>0.001f) {
                    e.strikeDirection=Vector3Normalize(aim);
                    e.strikeOrigin=e.position;
                }
                e.phase=BossPhase::Windup; e.phaseTime=0;
            }
            else if(e.phase==BossPhase::Windup&&e.phaseTime>BossWindupDuration(v,enraged)) { e.phase=BossPhase::Strike; e.phaseTime=0; }
            else if(e.phase==BossPhase::Strike) {
                if(v==1&&e.phaseTime<=strikeDuration&&Vector3Length(e.strikeDirection)>0.001f)
                    MoveEnemy(g,e,Vector3Scale(e.strikeDirection,e.speed*3.0f*(enraged?1.15f:1.0f)*dt));
                d=Distance(e.position,p.position);
                if(e.phaseTime>strikeDuration) {
                    bool inRange=d<attackRange;
                    Vector3 attackOrigin=e.position;
                    if(v==0||v==1||v==4) {
                        attackOrigin=e.strikeOrigin;
                        Vector3 toPlayer=Vector3Subtract(p.position,attackOrigin); toPlayer.y=0;
                        float forward=Vector3DotProduct(toPlayer,e.strikeDirection);
                        float lateralSquared=std::max(0.0f,Vector3LengthSqr(toPlayer)-forward*forward);
                        const float halfWidth=BossStrikeHalfWidth(v,forward);
                        inRange=forward>0.0f&&forward<attackRange&&lateralSquared<=halfWidth*halfWidth;
                    }
                    if(inRange&&HasLineOfSight(g,attackOrigin,p.position)&&p.invulnerable<=0) {
                        float guardCost=p.playerClass==PlayerClass::Warden?12.0f:18.0f;
                        if(p.blocking&&FacesAttacker(p,attackOrigin)&&p.stamina>=guardCost) p.stamina-=guardCost;
                        else { p.health-=damage[v]*(enraged?1.1f:1.0f); p.invulnerable=0.65f; }
                    }
                    e.phase=BossPhase::Recover; e.phaseTime=0;
                }
            }
            else if(e.phase==BossPhase::Recover&&e.phaseTime>recoverDuration) { e.phase=BossPhase::Hunt; e.phaseTime=0; }
        }
    }
    g.danger=std::clamp(g.danger+(nearby?18.0f:-12.0f)*dt,0.0f,100.0f);
}

void UpdateCombat(GameState& g,const Controls& c,float dt) {
    Player& p=g.player;
    const bool aliveAtStart=p.health>0.0f;
    p.attackTime=std::max(0.0f,p.attackTime-dt);
    p.blocking=c.block; p.bowAiming=c.aim;
    if(c.attack&&aliveAtStart) p.attackTime=0.22f;
    bool bossDefeated=false;
    if(c.attack&&aliveAtStart&&p.weapon==Weapon::Bow) {
        g.projectiles.push_back({Vector3Add(p.position,{0,1.35f,0}),Vector3Scale(p.facing,22),2.0f});
    } else if(c.attack&&aliveAtStart) {
        const float range=p.weapon==Weapon::Spear?3.1f:2.2f;
        float nearest=range+0.0001f;
        Enemy* target=nullptr;
        for(auto& e:g.enemies) {
            if(!e.alive) continue;
            Vector3 to=Vector3Subtract(e.position,p.position);
            float d=Vector3Length(to);
            if(d>0.001f&&d<=range&&d<nearest&&Vector3DotProduct(p.facing,Vector3Normalize(to))>0.1f) {
                if(HasLineOfSight(g,p.position,e.position)) {
                    nearest=d;
                    target=&e;
                }
            }
        }
        if(target) {
            float damage=p.weapon==Weapon::Axe?43:(p.weapon==Weapon::Spear?32:(p.weapon==Weapon::Shield?14:(p.weapon==Weapon::Sword?30:26)));
            if(p.weapon==Weapon::Shield&&p.combatSkill) damage=29;
            target->hp-=damage;
            target->hitTime=0.16f;
            target->stunTime=target->boss?0.08f:0.18f;
            Vector3 knock=Vector3Subtract(target->position,p.position); knock.y=0;
            if(Vector3Length(knock)>0.001f)
                target->knockback=Vector3Scale(Vector3Normalize(knock),target->boss?1.1f:3.0f);
            if(target->hp<=0&&target->alive) {
                target->alive=false; g.defeated++; UpdateProgression(g,target->boss?240:45);
                if(target->boss) bossDefeated=true;
            }
        }
    }
    for(auto& s:g.projectiles) {
        if(s.life<=0) continue;
        Vector3 from=s.position;
        Vector3 to=Vector3Add(from,Vector3Scale(s.velocity,dt));
        float firstHit=2.0f;
        Enemy* target=nullptr;
        for(auto& e:g.enemies) if(e.alive) {
            float hitTime=0.0f;
            if(SegmentSphere(from,to,Vector3Add(e.position,{0,1,0}),1.1f,hitTime)&&hitTime<firstHit) {
                firstHit=hitTime;
                target=&e;
            }
        }
        bool blocked=false;
        for(const auto& platform:g.platforms) {
            float hitTime=0.0f;
            if(SegmentPlatform(from,to,platform,hitTime)&&hitTime<firstHit) {
                firstHit=hitTime;
                target=nullptr;
                blocked=true;
            }
        }
        s.position=Vector3Lerp(from,to,std::min(firstHit,1.0f));
        s.life-=dt;
        if(target) {
            target->hp-=45;
            target->hitTime=0.16f;
            target->stunTime=target->boss?0.08f:0.16f;
            Vector3 knock=Vector3Subtract(target->position,p.position); knock.y=0;
            if(Vector3Length(knock)>0.001f)
                target->knockback=Vector3Scale(Vector3Normalize(knock),target->boss?0.9f:2.4f);
            if(target->hp<=0&&target->alive) {
                target->alive=false; g.defeated++; UpdateProgression(g,target->boss?240:45);
                if(target->boss) bossDefeated=true;
            }
            s.life=0;
        } else if(blocked) s.life=0;
    }
    g.projectiles.erase(std::remove_if(g.projectiles.begin(),g.projectiles.end(),[](const Projectile& s){return s.life<=0;}),g.projectiles.end());
    if(bossDefeated) {
        g.unlockedCity=std::max(g.unlockedCity,std::min(4,g.city+1));
        if(g.city==4) g.finished=true;
        else { g.city++; BuildCity(g); }
    }
    if(p.health<=0) {
        Vector3 checkpoint=QuestCheckpoint(g.city,g.quest);
        p.facing=QuestCheckpointFacing(g.city,g.quest);
        p.health=ClassMaxHealth(p.playerClass);
        p.stamina=ClassMaxStamina(p.playerClass);
        p.position=checkpoint;
        p.velocity={0,0,0};
        p.moveVelocity={0,0};
        p.grounded=true;
        p.wallRunning=false;
        p.dashTime=0;
        p.dashCooldown=0;
        p.vaulting=false; p.vaultTime=0;
        p.ledgeGrabbing=false; p.climbing=false; p.climbTime=0;
        p.ledgeNormal={}; p.ledgeHangPosition={}; p.ledgeTargetPosition={};
        p.landingTime=0; p.landingRoll=false; p.attackTime=0;
        p.blocking=false; p.bowAiming=false;
        p.invulnerable=1.2f;
        g.questTime=0;
        g.beaconHoldTime=0;
        g.danger=15;
    }
}

void UpdateProgression(GameState& g,int amount) {
    Player& p=g.player;
    p.xp=std::max(0,p.xp+amount);
    while(p.xp>=p.level*100) {
        p.xp-=p.level*100; p.level++;
        p.health=std::min(ClassMaxHealth(p.playerClass),p.health+12); p.stamina=ClassMaxStamina(p.playerClass);
    }
    if(p.level>=3) p.escapeSkill=1;
    if(p.level>=5) p.combatSkill=1;
}
