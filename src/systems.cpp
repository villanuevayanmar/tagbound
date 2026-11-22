#include "game.hpp"
#include "raymath.h"
#include <algorithm>
#include <cmath>

static float cameraYaw=0.0f, cameraPitch=-0.18f;

static bool Inside(Vector3 p,const Platform& b) {
    return std::abs(p.x-b.position.x)<=b.size.x*0.5f &&
           std::abs(p.z-b.position.z)<=b.size.z*0.5f;
}
static float Distance(Vector3 a,Vector3 b) { return Vector3Distance(a,b); }
static bool OverlapsWall(Vector3 feet,const Platform& b) {
    const float radius=0.34f, height=1.72f;
    bool x=feet.x+radius>b.position.x-b.size.x*0.5f && feet.x-radius<b.position.x+b.size.x*0.5f;
    bool z=feet.z+radius>b.position.z-b.size.z*0.5f && feet.z-radius<b.position.z+b.size.z*0.5f;
    bool y=feet.y+height>b.position.y-b.size.y*0.5f+0.05f && feet.y<b.position.y+b.size.y*0.5f-0.05f;
    return x&&z&&y;
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

void UpdateMovement(GameState& g,const Controls& c,Camera3D& camera,float dt) {
    Player& p=g.player;
    p.invulnerable=std::max(0.0f,p.invulnerable-dt);
    p.dashCooldown=std::max(0.0f,p.dashCooldown-dt);
    p.dashTime=std::max(0.0f,p.dashTime-dt);
    p.vaultTime=std::max(0.0f,p.vaultTime-dt);
    p.vaulting=p.vaultTime>0;

    cameraYaw-=c.lookX*0.008f;
    cameraPitch=std::clamp(cameraPitch-c.lookY*0.008f,-0.48f,0.42f);
    Vector3 forward={std::sin(cameraYaw),0,std::cos(cameraYaw)};
    Vector3 right={forward.z,0,-forward.x};
    Vector3 direction=Vector3Add(Vector3Scale(right,c.move.x),Vector3Scale(forward,c.move.y));
    if(Vector3Length(direction)>1.0f) direction=Vector3Normalize(direction);
    if(Vector3Length(direction)>0.05f) p.facing=Vector3Normalize(direction);
    else if(c.aim) p.facing=forward;

    float speed=p.playerClass==PlayerClass::Skirmisher?7.5f:6.0f;
    float sprintCost=18.0f*dt;
    if(c.sprint&&p.stamina>=sprintCost) { speed*=1.5f; p.stamina-=sprintCost; }
    else p.stamina=std::min(100.0f,p.stamina+13*dt);
    if(c.slide&&p.grounded) speed*=1.22f;
    if(c.skill&&p.escapeSkill>0&&p.stamina>=20&&p.dashCooldown<=0) {
        p.dashTime=0.24f; p.dashCooldown=1.8f; p.stamina-=20;
        if(Vector3Length(direction)<0.05f) direction=p.facing;
    }
    if(p.dashTime>0) speed*=2.7f;

    float oldX=p.position.x,oldZ=p.position.z;
    p.position.x+=(direction.x*speed+p.velocity.x)*dt;
    for(const auto& b:g.platforms) if(OverlapsWall(p.position,b)) { p.position.x=oldX; p.velocity.x=0; break; }
    p.position.z+=(direction.z*speed+p.velocity.z)*dt;
    for(const auto& b:g.platforms) if(OverlapsWall(p.position,b)) { p.position.z=oldZ; p.velocity.z=0; break; }
    p.velocity.x*=std::max(0.0f,1.0f-9.0f*dt);
    p.velocity.z*=std::max(0.0f,1.0f-9.0f*dt);

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
    if(p.position.y<floor) { p.position.y=floor; p.velocity.y=0; p.grounded=true; }
    else p.grounded=false;

    p.position.x=std::clamp(p.position.x,-49.0f,49.0f);
    p.position.z=std::clamp(p.position.z,-35.0f,42.0f);
    camera.target={p.position.x+forward.x*4.0f,p.position.y+1.2f+cameraPitch*5.0f,p.position.z+forward.z*4.0f};
    camera.position={p.position.x-forward.x*8.0f+right.x*1.6f,p.position.y+4.0f,p.position.z-forward.z*8.0f+right.z*1.6f};
}

void UpdatePursuit(GameState& g,float dt) {
    Player& p=g.player;
    int nearby=0;
    for(auto& e:g.enemies) {
        if(!e.alive) continue;
        float d=Distance(e.position,p.position);
        if(d<25) nearby++;
        if(d<24) {
            Vector3 to=Vector3Subtract(p.position,e.position); to.y=0;
            float horizontal=Vector3Length(to);
            if(d>1.45f&&horizontal>0.001f) e.position=Vector3Add(e.position,Vector3Scale(Vector3Normalize(to),e.speed*dt));
            else if(d<=1.45f&&e.cooldown<=0&&!e.boss) {
                e.cooldown=1.0f;
                if(p.blocking&&p.stamina>=8) p.stamina-=8;
                else if(p.invulnerable<=0) { p.health-=11; p.invulnerable=0.48f; }
            }
        }
        e.cooldown=std::max(0.0f,e.cooldown-dt);
        if(e.boss) {
            e.phaseTime+=dt;
            if(e.phase==BossPhase::Dormant&&d<20) e.phase=BossPhase::Hunt;
            else if(e.phase==BossPhase::Hunt&&d<3.5f) { e.phase=BossPhase::Windup; e.phaseTime=0; }
            else if(e.phase==BossPhase::Windup&&e.phaseTime>0.65f) { e.phase=BossPhase::Strike; e.phaseTime=0; }
            else if(e.phase==BossPhase::Strike&&e.phaseTime>0.35f) {
                if(d<4.0f&&p.invulnerable<=0) {
                    if(p.blocking&&p.stamina>=18) p.stamina-=18;
                    else { p.health-=24; p.invulnerable=0.65f; }
                }
                e.phase=BossPhase::Recover; e.phaseTime=0;
            }
            else if(e.phase==BossPhase::Recover&&e.phaseTime>0.8f) { e.phase=BossPhase::Hunt; e.phaseTime=0; }
        }
    }
    g.danger=std::clamp(g.danger+(nearby?18.0f:-12.0f)*dt,0.0f,100.0f);
}

void UpdateCombat(GameState& g,const Controls& c,float dt) {
    Player& p=g.player;
    p.blocking=c.block; p.bowAiming=c.aim;
    bool bossDefeated=false;
    if(c.attack&&p.weapon==Weapon::Bow) {
        g.projectiles.push_back({Vector3Add(p.position,{0,1.35f,0}),Vector3Scale(p.facing,22),2.0f});
    } else if(c.attack) {
        for(auto& e:g.enemies) {
            if(!e.alive) continue;
            Vector3 to=Vector3Subtract(e.position,p.position);
            float d=Vector3Length(to);
            if(d>0.001f&&d<=(p.weapon==Weapon::Spear?3.1f:2.2f)&&Vector3DotProduct(p.facing,Vector3Normalize(to))>0.1f) {
                float damage=p.weapon==Weapon::Axe?43:(p.weapon==Weapon::Spear?32:(p.weapon==Weapon::Shield?14:26));
                if(p.weapon==Weapon::Shield&&p.combatSkill) damage=29;
                e.hp-=damage;
                if(e.hp<=0&&e.alive) {
                    e.alive=false; g.defeated++; UpdateProgression(g,e.boss?240:45);
                    if(e.boss) bossDefeated=true;
                }
            }
        }
    }
    for(auto& s:g.projectiles) {
        if(s.life<=0) continue;
        s.position=Vector3Add(s.position,Vector3Scale(s.velocity,dt)); s.life-=dt;
        for(auto& e:g.enemies) if(e.alive&&Distance(s.position,Vector3Add(e.position,{0,1,0}))<1.1f) {
            e.hp-=45; s.life=0;
            if(e.hp<=0&&e.alive) {
                e.alive=false; g.defeated++; UpdateProgression(g,e.boss?240:45);
                if(e.boss) bossDefeated=true;
            }
            break;
        }
    }
    g.projectiles.erase(std::remove_if(g.projectiles.begin(),g.projectiles.end(),[](const Projectile& s){return s.life<=0;}),g.projectiles.end());
    if(bossDefeated) {
        g.unlockedCity=std::max(g.unlockedCity,std::min(4,g.city+1));
        if(g.city==4) g.finished=true;
        else { g.city++; BuildCity(g); }
    }
    if(p.health<=0) {
        p.health=100; p.position={0,0.3f,0}; p.velocity={0,0,0};
        p.stamina=100; p.invulnerable=1.2f; g.danger=15;
    }
}

void UpdateProgression(GameState& g,int amount) {
    Player& p=g.player;
    p.xp=std::max(0,p.xp+amount);
    while(p.xp>=p.level*100) {
        p.xp-=p.level*100; p.level++;
        p.health=std::min(125.0f,p.health+12); p.stamina=100;
    }
    if(p.level>=3) p.escapeSkill=1;
    if(p.level>=5) p.combatSkill=1;
}
