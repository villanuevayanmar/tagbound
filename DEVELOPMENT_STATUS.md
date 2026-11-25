# TAGBOUND Development Status

Last updated: 2026-11-25

## Current verified checkpoint — M0 baseline — 2026-11-25

**Verified source commit:** `1c51d4be821caf1688fa913131b990f4a3c20ccb`

**GitHub Actions run:** [#432](https://github.com/villanuevayanmar/tagbound/actions/runs/45536564152), workflow run ID `45536564152`, tied to the exact source commit above.

- Build job passed: Emscripten WebAssembly configuration and compile, C++ gameplay regression suite, static PWA assembly, and generated-output validation.
- Browser job passed: Chromium desktop and mobile landscape, gameplay startup/input/save, class-skill UI, Arrow Rain confirmation, map/floor UI, mobile Settings controls, service-worker cache and offline reload, portrait guidance, and forced WebGL context-loss recovery.
- Desktop canvas sampler: 12 samples, minimum non-black pixel ratio 0.9999. Mobile landscape sampler: 12 samples, minimum non-black pixel ratio 1.0000. These are short smoke samples, not the M1 three-minute run.
- Browser log ended with the PASS line for WebAssembly runtime, WebGL, context-loss recovery, gameplay input/save, service-worker cache, offline reload, mobile landscape controls, and portrait rotation.
- The visual artifact from run #432 (`tagbound-visual-review`, SHA-256 `d7893740230c06e45bdbd31230ef38ac05ca9f9fe05e785b1b93ffc2e2b997d8`) was downloaded and inspected: desktop/mobile intro, desktop/mobile gameplay, and mobile boss-gate screenshots for all five cities.
- The mobile Settings control was moved out of the HUD's top-left overlap. The mobile stamina screenshot assertion was corrected to sample the renderer's actual Stamina text band at the same coordinates as desktop; its pixel threshold and green-overlap rule were preserved.

### M0 root causes and workflow

1. The Cleaving Arc browser test expected the invalid SVG substring `M19 5`; the rendered icon path begins `M4 20 19 5`. The test now checks the actual move-to/line-to sequence. The icon was not changed.
2. At 844×390, the Settings button overlapped the left HUD by 12 pixels. Landscape CSS now places it between the objective and level panels; narrow landscape screens use a separate lower-center position.
3. The mobile stamina test inspected x=108–126, y=56–69 (the Mana row/blank area), despite the renderer drawing Stamina at x=24, y=39. It now samples x=70–125, y=37–53, matching the desktop check and visible glyph band.
4. The earlier PR #19 failure was from its temporary, older CI-only branch: it required touch controls while the intro screen was still visible. The current development workflow checks the intro and gameplay controls in their respective states; no production/gameplay behavior was changed to hide that test failure.
5. CI triggers include pushes to `main` and `development/complete-game`, plus pull requests targeting `main`. The `publish-dist` job only runs on a push to `main`, preventing development CI from publishing generated files to the protected branch. Visual-review artifacts upload even when browser assertions fail.

### Renderer / M1 investigation snapshot — not complete

- `src/main.cpp` has one Emscripten main loop and one balanced `BeginDrawing` / `EndDrawing` pair. `src/render.cpp` balances `BeginMode3D` / `EndMode3D`.
- No competing browser `requestAnimationFrame` driver was found in the inspected web shell. The inspected C++ and web sources do not create or dispose application textures, models, shaders, or render targets every frame.
- The WebGL context-loss path in `web/index.html` saves and reloads the page on loss/restore. It passes the forced context-loss smoke test, but that is recovery behavior, not proof that the intermittent black flicker is fixed or that the underlying cause is known.
- No root cause for the reported intermittent black/flickering frames has been established. No natural unexpected context loss was observed in the short CI samples. Baseline three-minute frame-time and memory measurements have not been collected.
- M1 remains open until a scripted run of at least three minutes covers movement, combat, abilities, enemy encounters, a city transition, and a boss fight with sampled frames never black/blank and no uncaught errors or unexpected context loss.

### PR, release, and owner-device state

- PR #17 remains open and draft. PR #19 was closed without merging after the exact-head M0 run passed.
- `main` remains `a18e39e9eca221a15d75c4070de9d06ef94abc3e`. Production was not touched; last observed production deployment is `dpl_GELuDKX6B4NragFQ6DSWjSNwFHAJ`, with runtime bundle `9115f13b4021f9052a95eea66ceaa1000f134978`.
- No new Preview was deployed. Previously recorded Preview `https://tagbound-5v46rpzby-yanmar.vercel.app` is stale. The available read-only Vercel usage endpoint returned no cost data, so free-tier capacity remains unverified and deployment is blocked.
- No phone testing has occurred in this session. Chromium emulation is not physical Android verification.
- No real player saves were accessed or committed; no paid services or quota workarounds were used.

### Ordered next work

1. Complete M1: instrument the in-game diagnostics panel and root-cause the intermittent WebGL black-frame behavior; then build the three-minute scripted stability scenario and capture baseline frame time/memory.
2. M2: test containment and scripted fights for all five bosses, including every movement/attack state and arena collision.
3. M3–M8: follow the owner-approved milestone sequence and exit gates. Do not claim a system complete merely because the source contains a partial implementation.
4. After M0 documentation changes, verify the final documentation head with CI; then continue M1. Keep PR #17 draft/unmerged and do not deploy production without explicit approval.

Last updated: 2026-11-24

## Class skill system implementation checkpoint — 2026-11-24

- Added two class-bound skills per class: Vanguard (Cleaving Arc, Breaker Strike), Skirmisher (Arrow Rain, Piercing Shot), and Warden (Shield Bash, Aegis Guard).
- Level 3 and level 5 each award one class-skill choice. Each choice offers a new skill in a free slot or an upgrade to an owned skill; skill rank is capped at 3. One of two owned slots is equipped as the active technique through Settings.
- Techniques use Mana and cooldowns. Arrow Rain uses a validated target marker, same-floor/range/line-of-sight/boss-arena checks, a warning delay, then three separated area waves. The target can be moved during aiming; the target is confirmed or cancelled; enemies can leave the area between waves. Aegis Guard reduces incoming pursuer and boss damage during its active window.
- Added class-skill milestone/loadout/target state to campaign save schema v3. Earlier saves receive unclaimed level 3/5 skill choices during one-time migration. Exploration data remains separate.
- Added C++ coverage for class mapping, milestone awards, unlock/upgrade, equip slots, Arrow Rain targeting/waves, invalid target rejection, and Aegis defensive window. CI browser checks now cover skill selection, saved loadout and Arrow Rain Mana/target behavior.
- Exact-head CI verification for the complete skill integration is pending. Do not reuse the previous map/stat CI runs as proof of this newer code.

## Character resources and stat progression checkpoint — 2026-11-24

- Added a separate Mana resource with class bases (Vanguard 70, Skirmisher 90, Warden 80) and slow 3.5-per-second regeneration. Stamina remains the physical traversal/guarding resource.
- Added five bounded stats (rank 0–10): Vitality +8 max health, Endurance +8 max stamina, Focus +12 max mana, Power +4% melee damage, Precision +4% bow damage.
- Each level gained adds one pending stat choice. The game pauses until the player selects exactly one upgrade through the level-up overlay. Capped stats cannot consume a choice.
- Stat choices, Mana, and pending choices are stored in campaign save schema v2. Existing schema v1 saves keep their city/level/XP/class; pre-existing levels receive unspent stat choices on migration. Exploration storage remains separate.
- Added C++ tests for max resource calculations, one-choice behavior, stat caps, Mana regeneration, and real melee/bow damage scaling. CI browser checks cover schema migration and the stat-selection flow.
- Exact-head CI verification for this new resource/stat batch is pending. The feature is not complete until the skill system uses Mana and users can choose new skills or upgrades at skill milestones.

## Boss arena enclosure checkpoint — 2026-11-24

- Added three raised collision-bearing stone walls around each city's final boss deck, leaving the south entrance open for the rooftop approach.
- The simulated boss center bounds now align with the inner faces of the visible walls (3.4-unit arena half-extents minus the boss body radius).
- Updated C++ regression expectations and added geometry checks for the west/east/north arena walls and open approach.
- This addresses the visual lack of enclosure as well as the existing movement/knockback/charge clamp. Physical Android verification is still required.

## Active development checkpoint — floor-aware exploration map

- Development branch: `development/complete-game`; PR #17 remains draft and unmerged. Production and `main` remain unchanged.
- The map uses a 20 × 16 grid per floor, with 5 × 5 world-unit cells and five elevation layers corresponding to the current ground, three objective rooftop platforms, and boss deck.
- Only grounded movement reveals the current elevation. Airborne motion cannot reveal an upper floor. Each city's layers are saved independently in `tagbound-exploration-v2-<city>` (1,600 cells per city).
- Legacy `tagbound-exploration-v1-<city>` data is read as the ground layer on first load; the existing `tagbound-save` campaign record remains separate and unchanged.
- Previous/next floor controls and keyboard `[` / `]` navigate only to floors that already contain discovered cells. The map shows the player only on their current floor and gates the objective marker by both floor and explored cell.
- The service-worker cache is now `tagbound-v29`. C++ tests cover floor heights, no airborne reveal, separate floor discovery, and discovered-floor navigation. Browser tests cover UI state, mobile touch control, the 1,600-cell format, and reload persistence.
- The floor-aware implementation and the boss-arena wall additions require a fresh exact-head CI run. The earlier single-layer map passed run #347, but that does not verify these subsequent changes.
- Physical Android testing remains outstanding; the original black-flicker root cause is still unconfirmed.

## Project state

**Playable production build available — continued development in progress.**

Repository: https://github.com/villanuevayanmar/tagbound  
Production: https://tagbound.vercel.app  
Current main commit (verified at session start): `a18e39e9eca221a15d75c4070de9d06ef94abc3e`  
Latest tested runtime bundle commit: `9115f13b4021f9052a95eea66ceaa1000f134978`  
Production deployment: `dpl_GELuDKX6B4NragFQ6DSWjSNwFHAJ`  
Production deployment state: **READY**  
Production alias: `tagbound.vercel.app` → `dpl_GELuDKX6B4NragFQ6DSWjSNwFHAJ`

The public production alias still serves the previously verified main-branch runtime `9115f13b4021f9052a95eea66ceaa1000f134978`; PR #14 changes are not deployed. The production Chromium suite passed against that deployed runtime. Physical Android verification of the current PR #14 code is pending because the deployment hold remains active.

## Recently completed and merged

### Settings and pause system — PR #1

- Added an in-game Settings overlay with Resume.
- Pauses the actual C++ simulation while Settings is open.
- Home and Settings share the saved sound preference.
- PWA installation appears only on Home and Settings, never during active gameplay.
- Service-worker cache version: `tagbound-v25`.

### HUD rendering and Settings layout — PR #2

- Added the missing Raylib `BeginDrawing()` / `EndDrawing()` frame boundaries.
- Confirmed health, stamina, level, danger, objective and city/quest HUD render in hosted Chromium screenshots.
- Moved Settings into the top HUD gap; narrow landscape screens use a compact gear control.
- Added screenshot-pixel validation and rectangle checks to prevent HUD overlap.

### Pause input reset — PR #3

- Opening Settings clears joystick movement, touch action bits, camera-look deltas, active-button styling and tracked pointer captures.
- Hosted desktop/mobile checks verify all touch/look state is cleared.

### Original ambient score — PR #4

- Added locally synthesized original ambient music using Web Audio oscillators and filters.
- No downloaded music assets, external audio API or paid dependency.
- Score remains stopped on Home, starts after Start, pauses in Settings, resumes with the run, follows the shared sound toggle and stops on the victory ending.
- Hosted desktop/mobile checks verify generated note count, pause, no notes while paused, resumed notes and saved-victory silence.

### Class-specific BOW control — PR #5

- BOW touch action is hidden for Vanguard and Warden and shown only for Skirmisher.
- Fixed loadouts remain Vanguard → Sword, Skirmisher → Bow, Warden → Shield.
- Hosted tests verify action visibility and saved weapons for each class.

### Timed hold-beacon objectives — PR #6

- Mixed reach and timed hold objectives across all five cities.
- Hold stages require 2.5–4.0 seconds inside the capture radius while pursuers remain active.
- Leaving the radius, death, quest changes and city rebuilds reset capture.
- Objective HUD shows hold progress; C++ and hosted browser tests cover objective behavior.

### Death-frame attack prevention — PR #7

- A player whose health reaches zero during pursuit cannot begin a new melee attack or spawn a new bow projectile later in the same frame.
- Already-fired projectiles remain in flight.
- C++ regressions cover dead Vanguard melee and dead Skirmisher bow attempts.

### Five-city campaign across all classes — PR #8

- Verifies Vanguard/Sword, Skirmisher/Bow and Warden/Shield can each defeat all five city bosses.
- Skirmisher's regression uses actual Bow projectile travel and impact.
- Verifies boss defeat, city unlock progression, fixed loadouts and the fifth-city ending for all three classes.

### Shared boss warning and strike ranges — PR #9

- Added a single `BossStrikeRange()` source used by both combat hit detection and the visible 3D boss warning rings.
- Removed duplicate combat and renderer range arrays, preventing warning visuals from drifting away from the actual hit radius.
- Added radial inside/outside evasion regressions for Foundry Colossus and Imperial Regent.

### Automatic live-production smoke trigger — PR #10

- Kept manual live-smoke dispatch available.
- Added a push trigger limited to updates of `.github/workflows/live-smoke.yml`, so the live production suite can be run and verified without a manual workflow-dispatch action.
- This trigger does not deploy game code and does not run on ordinary source/documentation changes.

## Verified cloud testing

- Settings and pause feature: run `45073264491` passed.
- HUD rendering fix and Settings layout: run `45074803692` passed.
- Pause input reset: run `45075281567` passed.
- Original ambient score: run `45076329628` passed.
- Class-specific BOW controls: run `45077008462` passed.
- Timed hold-beacon objectives: run `45077905618` passed.
- Death-frame attack fix and generated output: run `45078768095` passed.
- All-class five-city campaign PR: run `45104171971` passed.
- Main after the campaign and runtime bundle sync: run `45104378957` passed.
- Shared boss range/radial evasion PR: run `45104855315` passed.
- Main after shared boss range merge and generated output sync: run `45105067345` passed.
- PR #10 workflow change: run `45105430992` passed.
- Live production suite: run `45105714358` passed against `https://tagbound.vercel.app`.
- Main after PR #10 workflow merge: run `45105714359` passed, including WebAssembly build, C++ regressions, desktop/mobile browser checks and PWA artifact validation.
- Live smoke confirmed WebAssembly/WebGL startup, gameplay, class selection, save restoration, Settings pause/resume, touch/look input reset, ambient music pause/resume, HUD visibility, install placement, service-worker control/cache, offline reload, mobile controls and portrait rotation guidance.
- These hosted checks do not replace physical Android playtesting.

## Deployment

<badge>READY</badge>

- Production deployment ID: `dpl_GELuDKX6B4NragFQ6DSWjSNwFHAJ`
- Deployed runtime bundle commit: `679a6d6ae5d7b3b338e91d9cb000d47b9e2615ac`
- Production alias: `tagbound.vercel.app`
- Vercel deployment metadata and alias lookup both confirmed the production alias points to this READY deployment.
- The current main SHA `a18e39e...` adds a live-smoke target diagnostic after the generated runtime bundle commit `9115f13...`; the verified READY production deployment runs that generated bundle.
- Do not create a manual deployment unless the automatic Git deployment path stops working. Keep Vercel at the free tier; no paid plan or billing changes were made.

## Remaining development work

### Physical-device verification

- GitHub Actions Chromium desktop/mobile workflows pass, but emulation is not a substitute for a real Android device.
- The production alias still runs main runtime `9115f13b4021f9052a95eea66ceaa1000f134978`, not the current PR #14 branch.
- The user reported a Vercel deployment limit on 2026-11-23. Do not merge, manually deploy, bypass the limit, or change billing. The earliest reported retry is after 2026-11-24 at approximately 3:35 PM Philippine time; that is not a guarantee of reset. Any deployment still requires explicit user authorization.
- After an authorized release of the current code, test PWA installation, landscape/orientation, touch movement and actions, class loadouts, pause/input reset, audio, objective encounters, boss telegraphs, save restoration, offline launch, and actual frame-time on the user's Android device.
- No real-device FPS or performance improvement has been measured.

### Gameplay and campaign

- The agreed campaign systems are implemented and regression-tested: five-city order and progression; legacy-save migration; Vanguard/Sword, Skirmisher/Bow, Warden/Shield; rooftop routes, jumping, ledge climbing, wall-running, wall-jumping and dash; pursuit enemies; reach, timed hold and timed survival objectives; five boss variants and their attack geometry; boss defeat progression, ending/restart and save restoration.
- Run #310 also verified the browser workflows for pause/resume, audio preference persistence, touch reset, save restoration, and offline reload.
- Difficulty, stamina costs, boss health/damage, reinforcement pacing, and encounter duration still need real-play feedback. Do not invent extra quest types: reach, hold and survival are the agreed objective set.

### Visuals and animation

- The renderer intentionally uses procedural low-poly geometry.
- Run #310's ten full-resolution boss windup screenshots were reviewed across all five cities at desktop and mobile-landscape sizes. Variant equipment/silhouettes and warning geometries were distinguishable; radial warnings are subtler but visible. No render correction was justified.
- Continue visual changes only when a reproducible issue appears. Physical Android readability and performance remain unverified.

### Audio

- Chromium tests verify audio unlock, shared sound preference, music pause/resume, and stopping music on the victory ending.
- Physical Android audio balance and resume behavior remain unverified.

### Release readiness

- PR #13 and PR #14 remain open and unmerged. PR #14 remains draft.
- Keep all changes outside `dist/` during the deployment hold. Do not merge, deploy, upgrade, or bypass quotas.
- Maintain the ₱0/$0 constraint. No paid APIs, subscriptions, hosting upgrades or usage billing.

## Testing and development constraints

- The user does not have a laptop. Never require local commands or local build/test steps.
- Use GitHub-hosted CI and authorized cloud workflows for development verification.
- Ask for Android testing only when the deployed production alias has been verified READY and the live smoke suite has passed.
- Keep cost at ₱0/$0. No paid API, paid subscription, paid hosting upgrade, billing change or quota bypass without explicit approval.
- Never claim a test, commit, merge, deployment or device test that has not actually occurred.

## Exact next task

1. Continue auditing the agreed gameplay/PWA scope for concrete uncovered edge cases using the existing source and hosted regression tests.
2. Keep PR #13 and PR #14 open/unmerged and production unchanged.
3. Do not request physical Android testing of PR #14 until a release is explicitly authorized and deployed after the Vercel hold.
4. After authorized device access, collect actual Android touch, orientation, audio, and frame-time findings; reproduce any serious issue through GitHub-hosted tests.


## Active checkpoint — campaign order and legacy-save preservation (2026-11-23)

- Production was checked read-only: deployment `dpl_GELuDKX6B4NragFQ6DSWjSNwFHAJ` is **READY**, with alias `https://tagbound.vercel.app`; its generated runtime commit is `9115f13b4021f9052a95eea66ceaa1000f134978`.
- The user reported that the Vercel deployment limit is active. Do not merge a PR that may trigger deployment, manually deploy, or attempt to bypass the limit. The earliest retry is after November 24, 2026 at approximately 3:35 PM Philippine time, and deployment still requires the user's explicit authorization.
- PR #13 remains open at `6b7bd74fa832ffef50225307d3ed2db419222243`; matching run #268 passed. It is separate from PR #14 and is not in production.
- PR #14 remains an open draft: `https://github.com/villanuevayanmar/tagbound/pull/14`, branch `feat/campaign-imperial-before-spire`. Its parent checkpoint `fe7cc9182deb` passed run #271 (WebAssembly/C++ build and browser smoke).
- Review found that PR #14 changes the meaning of campaign indices 3 and 4 while the old local save contains no schema version. Its original browser test incorrectly expected raw indices to remain unchanged, which would change an existing save's city identity.
- This checkpoint adds a one-time legacy-save migration: old stage 3 (High Spire) maps to new stage 4, and old stage 4 (Imperial Capital) maps to new stage 3. Saves are then written with `schemaVersion: 1`; later loads do not remap them again. The C++ regression suite and hosted browser smoke test are extended to check the mapping and repeat-load behavior.
- The new migration commit's CI is triggered by this PR update. Recheck the latest run against the exact branch head before treating it as verified. PR #14 remains draft/unmerged; PR #13 remains open/unmerged; production must remain unchanged.
- Physical Android testing is still unverified.

### Exact next task

1. Inspect the latest PR #14 head and matching GitHub Actions run after the legacy-save migration commit. Read logs and fix any failures without weakening assertions.
2. Verify PR #13 and PR #14 remain open and compare overlapping `.github/workflows/ci.yml` changes before any later merge planning.
3. Continue the next verified gameplay, campaign, visual, performance, or audio gap on a development branch. Do not merge or deploy while the user-reported Vercel limit is active or without explicit authorization.


### Survival-pursuit objectives — PR #14 development branch

- Added a third objective type alongside immediate reach and timed hold: survive an active pursuit away from the beacon for 18–22 seconds.
- Gilded (quest 2), Foundry (quest 2), and Spire (quest 1) use survival encounters; other quest patterns retain their reach/hold behavior.
- Survival objectives do not draw a beacon marker or objective compass. The HUD names the survival task and shows remaining time plus active tagger count.
- Survival timers advance independent of player position and reset on defeat, quest change, or city rebuild. Hold-beacon radius/reset rules are unchanged.
- Added C++ tests for each survival duration, out-of-radius completion, type exclusivity, and existing defeat-reset behavior. Added hosted Chromium checks for the exported objective state for all three selected stages.
- Legacy save migration commit `a8d16889816f6681f2163646497f3fa236dd7a3c` passed CI run #272 (`45133035082`): WebAssembly compile, C++ gameplay tests, PWA validation, and browser smoke all succeeded.
- Survival feature commit `e498d73408385aaf71c5055c57c25bae676275a9` contains implementation and C++ regressions; browser workflow assertions are being added in the following commit and need a new exact-head CI run.
- No PR merge or Vercel deployment was performed; the user's deployment hold remains active.


### Rooftop pursuit navigation regression — PR #14 development branch

- Added a C++ simulation regression with a solid wall blocking direct line of sight. It requires a pursuer to reach the player by following a waypoint around the wall and verifies it does not finish embedded in the obstacle.
- This fills a documented test gap; existing tests already covered line-of-sight attack prevention but not successful waypoint traversal.
- Survival-objective implementation and browser assertions passed run #274 (`45133786317`) for exact head `095fb05665f6b100fb3c9f5db0b5d18d05ca6d82`.
- The navigation regression commit below has a new CI run pending. If it fails, inspect the actual C++ test output and correct the navigation behavior at its source.
- PR #13 and #14 remain open/unmerged. Production remains on the previously verified READY deployment; no Vercel deploy has been attempted.


## Latest checkpoint — body-aware rooftop pursuit routing (2026-11-23)

- PR #14 branch head before this fix: `c1837d212db35c224389534268f61fc3b21a254f`.
- Exact-head run #275 (`45134277135`) built WebAssembly successfully but failed the C++ gameplay regression at `tests/gameplay.cpp:712`: a pursuer could not route around a rooftop wall and reach the player.
- Root cause: pursuit line-of-sight used a small ray margin while movement collision used the enemy's full body radius. The pursuer therefore resumed direct movement before its body could clear the wall and stalled against it.
- Fix committed as `15938b998823fe9bc7a9e4ca8ae57954e6ac2a9c`: obstacle segment tests now accept a clearance margin; direct pursuit line-of-sight uses the enemy body radius (0.42 for pursuers, 0.62 for bosses), preventing early direct movement. Corner-waypoint visibility checks retain their existing narrow ray margin; waypoint offsets already include the enemy radius plus clearance.
- The existing regression remains unchanged: it still requires the pursuer to get within 3 units and not overlap the wall. No assertion was weakened.
- CI for `15938b9...` must be checked against that exact SHA. Do not infer success from the earlier run #274.
- Current PR #14 remains open/draft/unmerged. PR #13 remains open/unmerged at `6b7bd74fa832ffef50225307d3ed2db419222243`; its matching run #268 passed.
- Vercel production was checked read-only and remains READY at `dpl_GELuDKX6B4NragFQ6DSWjSNwFHAJ`, deployed runtime `9115f13b4021f9052a95eea66ceaa1000f134978`. The user-reported deployment hold remains in force. No merge or deployment has been attempted.

### Exact next task

1. Verify the latest PR #14 branch head and matching GitHub Actions run after `15938b9...` (including the new documentation commits); inspect job steps/logs.
2. If the routing regression still fails, use its actual coordinates/trace to correct path selection without weakening the assertion.
3. If it passes, continue with another concrete gameplay gap, prioritizing parkour collision/ledge transitions and boss counterplay; add reproducible tests before changing behavior.
4. Keep both PRs open and production unchanged. Physical Android testing remains unverified.


- Follow-up run #277 (`45137349773`) showed that applying the body radius to corner-waypoint LOS broke the existing open-side wall-routing regression at `tests/gameplay.cpp:558`. The follow-up commit `1c48ec1806a2d8593aa61b184471eface72ae5aa` limits body-radius clearance to direct pursuit LOS and restores the prior waypoint ray checks. Re-run all tests against the newest exact branch head; neither routing fix is verified until CI passes.


## Verified checkpoint — parkour stamina, class speed, and campaign restart (2026-11-23)

### Current branch and CI
- PR #14 remains open and draft: `feat/campaign-imperial-before-spire`.
- Latest gameplay/test commit: `37bffda9a20139393b9074d4e7e43fa002a62c3f`.
- Exact Actions run #287 (`45141318209`) passed on that SHA. WebAssembly compile, C++ gameplay tests, PWA validation, desktop/mobile Chromium smoke, service-worker cache checks, and offline reload all passed. The PR-only `publish-dist` job was skipped.
- Run #286 (`45140870041`) also passed on `73c5fb35f2430345d9eb5b44a031f8088fe80ed1` with the campaign-restart browser check.
- Earlier exact-head runs #283 (`eb85bab387f12e8c05bd285eeb55a829820a1e0c`), #284 (`ecaff196d843d78b5b17ff69fb3c59023df1da1b`), and #285 (`262dd881edba4651769ec6d9f679bf200079f80a`) passed as well.

### Gameplay corrections and regression coverage
- Pursuit pathfinding now accounts for pursuer body clearance on direct line-of-sight checks and retains waypoints while navigating around sequential rooftop obstacles. Existing wall-routing assertions were kept intact; run #283 passed the full C++ suite.
- Sprint stamina correction: holding Sprint when stamina is too low to pay the sprint cost no longer regenerates stamina. Stamina regenerates after Sprint is released; wall-running continues to drain it and stops when exhausted. C++ tests cover normal wall-run drain, wall-jump launch direction, and low-stamina behavior. Run #284 passed.
- Added a direct movement comparison proving Skirmisher's unsprinted base movement exceeds Vanguard's under identical starting conditions and input. Run #285 passed.
- Added a browser regression for PLAY AGAIN from the fifth-city victory screen. Run #286 confirmed city, unlocked stage, quest, level, XP, and finished state reset on reload.
- Extended that restart check to verify the selected Skirmisher class, Bow loadout, and sound preference survive the reset. Exact run #287 passed.

### Vercel safety and deployment state
- Production remains **READY** at `dpl_GELuDKX6B4NragFQ6DSWjSNwFHAJ`, alias `https://tagbound.vercel.app`, runtime commit `9115f13b4021f9052a95eea66ceaa1000f134978`. This was checked read-only after run #287.
- The user's deployment hold remains in force. No PR has been merged and no production deployment has been initiated.
- Read-only Vercel event inspection confirmed the existing `vercel.json` ignored-build command (`git diff --quiet HEAD^ HEAD -- dist/`) canceled a source-only PR #14 preview attempt before deployment because the command returned exit code 0. Keep PR #14 commits away from `dist/`; do not change this guard or manually deploy during the hold.
- Vercel's history also contains older READY preview deployments from PR #13 commits at 13:59 and 15:10 Philippine time on November 23, before the user's reported 15:35 hold. The source-only PR #14 attempt at 15:42 was canceled by the ignored-build step; production's READY deployment did not change.
- PR #13 remains open/unmerged at `6b7bd74fa832ffef50225307d3ed2db419222243`; exact run #268 passed.
- PR #14 remains open/draft/unmerged at the commit recorded above.
- Integration note: PR #13 bumps the service-worker cache to `tagbound-v27`, while this PR #14 branch still validates `tagbound-v25`. The combined CI workflow must reconcile the cache-version assertion and preserve both PRs' icon-only, survival-objective, legacy-save, and restart browser checks before integration. The PRs also both edit `.github/workflows/ci.yml` and the project documentation. Do not merge during the deployment hold.

### Remaining verification
- Physical Android playtesting remains unverified. Hosted Chromium mobile emulation is not a substitute.
- Remaining work includes review of boss telegraph/counterplay consistency, parkour behavior on more complex rooftop geometry, balance across the five cities, and any concrete findings from the user's Android playtest.

### Exact next task
Review each boss variant's telegraph geometry against its actual strike damage region and facing/line-of-sight rules. Add targeted C++ boundary tests before changing any combat behavior. Keep source/test changes on PR #14; do not edit `dist/`, merge, or deploy.


## Verified checkpoint — boss telegraph and strike-lane alignment (2026-11-23)

- PR #14 branch: `feat/campaign-imperial-before-spire`; latest source/test commit `81465b29c2943c48c45516054f8ad77696b599a9`.
- Exact GitHub Actions run #290 (`45142530269`) passed on that SHA: Emscripten/WebAssembly compile, C++ gameplay regression suite, static PWA validation, Chromium browser smoke, and service-worker/offline checks. The PR-only `publish-dist` job was skipped.
- Root cause: Gilded Charger's and Spire Sentinel's damage lanes accepted player centres up to 0.9 units from the strike centreline, while their warning cylinders were drawn with smaller, tapered radii (0.38 and 0.45). This made the visible warning narrower than the actual damage area.
- Added shared `BossStrikeHalfWidth(variant, forward)` geometry in `src/game.cpp` / `src/game.hpp`; both combat hit testing and warning rendering now use it. The Charger and Sentinel warning cylinders use a 0.9-unit radius; Harbor's widening cleave uses the same helper for its wedge boundary.
- C++ regressions assert the shared widths and check both bosses at 0.89 (hit) and 0.91 (miss) units from the lane centreline. Existing damage and dodge tests remain in place.
- Run #289 (`0fe17ffb9796df41e5dd5f9d126aeb074fcc3fcb`) passed the substantive boss geometry fix; run #290 passed the cleanup commit and full suite.

### Repository / deployment state
- PR #13 remains open/unmerged at `6b7bd74fa832ffef50225307d3ed2db419222243`; exact run #268 passed.
- PR #14 remains open/draft/unmerged at `81465b29c2943c48c45516054f8ad77696b599a9`.
- Main remains `a18e39e9eca221a15d75c4070de9d06ef94abc3e`.
- Production remains **READY** at `dpl_GELuDKX6B4NragFQ6DSWjSNwFHAJ`, alias `https://tagbound.vercel.app`, runtime commit `9115f13b4021f9052a95eea66ceaa1000f134978`. Read-only Vercel lookup after run #290 confirmed the production deployment did not change.
- The deployment hold remains active. No PR merge, production deployment, or `dist/` change was made.
- Integration warning remains: PR #13's service-worker cache is `tagbound-v27`, while PR #14's CI currently expects `tagbound-v25`. When combining branches, reconcile cache assertions and preserve both PRs' browser tests. Do not merge during the hold.
- Physical Android testing remains unverified.

### Exact next task
Review rooftop-route geometry and jump envelopes for all five cities. Compare each pair of consecutive objective platforms and the boss gate against the actual movement/jump physics; add a simulation regression for any route gap that cannot be traversed with the supported controls. Do not change gameplay parameters without a reproducible reachability failure.


## Verified checkpoint — five-city rooftop route simulation (2026-11-23)

- PR #14 head `48569424b036950fe4b477f0307f38e25eae9ca7` passed exact Actions run #296 (`45144472169`): WebAssembly build, complete C++ gameplay regression suite, static PWA validation, desktop/mobile Chromium, and service-worker/offline smoke. The PR-only `publish-dist` job was skipped.
- Added a deterministic movement simulation that checks all four transitions in each of the five cities: spawn roof → objective 1 → objective 2 → objective 3 → final boss gate (20 transfers total).
- The initial simulation exposed a test-harness omission: when the jump reaches a roof edge and enters `ledgeGrabbing`, the actual control contract requires a second Vault/jump press to climb. The final simulation now supplies that input and all 20 transitions pass. No route geometry or movement tuning was changed.
- The earlier failed attempts (#292–#295) were test-development runs. Their diagnostics showed the Imperial opening jump reached a valid ledge hang; the simulation had not sent the climb input. This was corrected in the test, not by weakening the reachability assertion.
- The test models Skirmisher movement/sprint and the existing jump/ledge-climb controls, and tests each route segment from the prior objective platform. Physical Android timing/handling still requires device testing.

### Current branch/deployment state
- PR #13 remains open/unmerged at `6b7bd74fa832ffef50225307d3ed2db419222243`; exact run #268 passed.
- PR #14 remains open/draft/unmerged at `48569424b036950fe4b477f0307f38e25eae9ca7`.
- Production remains READY at `dpl_GELuDKX6B4NragFQ6DSWjSNwFHAJ`, alias `https://tagbound.vercel.app`, runtime commit `9115f13b4021f9052a95eea66ceaa1000f134978`. No merge or deployment was performed.
- Keep PR #14 edits outside `dist/` during the Vercel hold. PR #13 uses service-worker cache `tagbound-v27`, while PR #14 still validates `tagbound-v25`; reconcile that CI/service-worker overlap when preparing integration, while preserving both branches' browser assertions.
- Android physical-device playtest remains pending.

### Next task
Inspect how boss progression is unlocked after the third objective on each city and whether every boss arena is actually reachable from the final objective using normal movement. Extend the route simulation to cover objective 3 → boss arena entry. Then audit save/checkpoint restoration at boss entry and after defeat, preserving the legacy-save migration and existing player preferences. Do not modify `dist/`, merge, or deploy during the hold.


## Verified checkpoint — boss-gate spawn and final approach (2026-11-23)

- PR #14 source/test head `8f6aa99e53b69f8da116ab6ac38e4954be347721` passed exact Actions run #298 (`45148968483`): Emscripten/WebAssembly build, full C++ gameplay regressions, static PWA validation, Chromium browser smoke, service-worker cache and offline reload. The PR-only `publish-dist` job was skipped.
- Extended the five-city rooftop simulation so the final transition (objective 3 → boss gate) instantiates the actual boss first using `SetQuestProgress(routeGame, 3)`.
- For every city, the test verifies boss unlock, exactly one live boss, the expected city-specific boss variant, and exact spawn coordinates at that city's gate before simulating the final approach.
- All 20 rooftop transfers still pass. The run also retains the existing boss-gate browser restore check and the C++ checkpoint-after-defeat tests.
- No gameplay or route geometry was changed in this checkpoint; only regression coverage was extended.

### Current repository and deployment status
- PR #13 remains open/unmerged at `6b7bd74fa832ffef50225307d3ed2db419222243`; exact run #268 passed.
- PR #14 remains open/draft/unmerged at `8f6aa99e53b69f8da116ab6ac38e4954be347721`.
- Production is still **READY** at `dpl_GELuDKX6B4NragFQ6DSWjSNwFHAJ`, alias `https://tagbound.vercel.app`, runtime commit `9115f13b4021f9052a95eea66ceaa1000f134978`. A read-only production lookup after run #298 confirmed it is unchanged.
- No merge, deployment, or `dist/` edit occurred. The user's deployment hold remains active.
- Integration still needs PR #13's `tagbound-v27` service-worker cache reconciled with PR #14's `tagbound-v25` CI assertion. Preserve both PRs' icon-only, survival, migration, restart, boss and route checks when integrating.
- Physical Android playtesting remains unverified. Hosted Chromium mobile emulation is not a substitute.

### Next task
Review campaign save/restore behavior at every city and objective checkpoint. Confirm city, unlocked-city, quest, XP/level, class/weapon and schema version survive reload; verify boss-gate reload reconstructs the correct city-specific boss and checkpoint location. Add only missing targeted assertions, keeping the existing legacy-stage remap and class/sound preservation tests intact. Do not edit `dist/`, merge or deploy during the hold.


## Verified checkpoint — full save-field reload assertions (2026-11-23)

- PR #14 commit `39c128f65e0ffc722b44f850aee2f32ab34d615e` passed exact GitHub Actions run #300 (`45149650762`): WebAssembly compile, C++ gameplay suite, PWA validation, Chromium desktop/mobile smoke, service-worker cache and offline reload. The PR-only `publish-dist` job was skipped.
- Strengthened legacy stage 3/4 migration tests to confirm level 2 and 15 XP, selected Skirmisher class, Bow loadout, quest, completion flag, and `schemaVersion: 1` survive both the initial migration reload and a second reload. The one-time remap remains idempotent.
- Strengthened the browser boss-gate restoration check to confirm the complete saved campaign fields (city/unlocked, quest, level/XP, class/weapon, finished flag, schema version) are preserved while the boss is reconstructed.
- No save-format or runtime logic change was required. The new assertions passed against the existing save implementation.

### Current repository/deployment
- PR #13 remains open/unmerged at `6b7bd74fa832ffef50225307d3ed2db419222243`; exact run #268 passed.
- PR #14 remains open/draft/unmerged at `39c128f65e0ffc722b44f850aee2f32ab34d615e`.
- Production remains **READY** at `dpl_GELuDKX6B4NragFQ6DSWjSNwFHAJ`, alias `https://tagbound.vercel.app`, runtime commit `9115f13b4021f9052a95eea66ceaa1000f134978`. Read-only Vercel lookup after run #300 confirmed production was unchanged.
- No merge, deployment or `dist/` edit occurred. The user-reported deployment hold still applies.
- The PR integration issue remains: PR #13 updates the service-worker cache to `tagbound-v27`, whereas PR #14's PWA workflow currently expects `tagbound-v25`. Resolve this when preparing a combined candidate; preserve both PRs' CI assertions.
- Physical Android testing remains outstanding.

### Next task
Review the five city render paths for visible differentiation and mobile rendering cost. Inspect existing city-specific meshes, landmarks, lighting and per-frame draw calls first. Avoid adding high-cost geometry or new dependencies; make only changes that can be checked with the existing screenshot/browser smoke workflow. Keep source changes out of `dist/`, with no merge or deploy during the hold.


## Verified checkpoint — enemy and boss model facing (2026-11-23)

- PR #14 source/test head `a93a7e4327570630cbc4cc884ea2982eea90aa57` passed exact Actions run #302 (`45150660649`): Emscripten/WebAssembly compile, full C++ gameplay regression suite, PWA assembly/validation, Chromium browser smoke, and service-worker/offline checks. The PR-only `publish-dist` job was skipped.
- Screenshot/source review found that all enemy and boss models used a fixed local forward vector `{0,0,-1}`, regardless of player approach direction or a boss's locked attack direction.
- Added pure `EnemyFacing(enemy,target)` logic and supplied the player's position to the enemy renderer. Pursuers and bosses in Hunt/Recover face the player's horizontal direction. Bosses in Windup/Strike retain their normalized, locked `strikeDirection`, keeping their pose consistent with the warning/attack lane. Coincident positions use a deterministic fallback.
- C++ tests cover target-facing normalization, locked boss windup direction despite target relocation, returning to target tracking after recovery, and the zero-length fallback. No movement, pursuit, hitbox, damage or animation timing changed.
- Run #302 passed both CI jobs. Browser log explicitly reported the WebAssembly/WebGL/gameplay/save/PWA/offline/mobile smoke PASS marker.

### Current branch and deployment state
- PR #13 remains open/unmerged at `6b7bd74fa832ffef50225307d3ed2db419222243`; exact run #268 passed.
- PR #14 remains open/draft/unmerged at `a93a7e4327570630cbc4cc884ea2982eea90aa57`.
- Production remains **READY** at `dpl_GELuDKX6B4NragFQ6DSWjSNwFHAJ`, alias `https://tagbound.vercel.app`, deployed runtime `9115f13b4021f9052a95eea66ceaa1000f134978`. This render-only branch change was not published.
- Vercel deployment hold is still in force. No merge, production deployment, or `dist/` edit was performed.
- Integration still needs PR #13's `tagbound-v27` service-worker cache reconciled with PR #14's `tagbound-v25` CI expectation.
- Physical Android performance/orientation testing remains unverified; browser emulation is not an Android device measurement.

### Next task
Review the remaining low-poly rendering path for redundant or duplicated per-frame geometry. Keep the review evidence-based: profile/draw-call counts where the source permits, inspect generated screenshots, and avoid changing geometry or quality settings without a reproducible visual/performance defect. Prefer small renderer-only changes and keep all changes out of `dist/` during the deployment hold.


## Verified checkpoint — world-floor render de-duplication (2026-11-23)

- PR #14 code/test commit `a3e7e66b8c16712171b5abaf808d7faf98f01efe` passed exact Actions run #304 (`45151623835`): Emscripten/WebAssembly build, full C++ gameplay tests, PWA assembly/validation, Chromium desktop/mobile smoke, service-worker cache and offline reload. The PR-only `publish-dist` job was skipped.
- Found duplicate floor rendering in `DrawGame`: the 120×120 collision-floor platform was drawn as a cube plus wireframe every frame, while a separate ground plane was also drawn below it. The grid was positioned inside the collision slab and therefore occluded.
- Kept the collision platform in `GameState` unchanged. Renderer now recognizes only the large thin world-floor slab and skips its cube/wireframe, drawing one plane at the collision top instead. Ground-grid lines are placed just above that plane.
- Inspected the run #304 desktop and mobile gameplay screenshot artifacts: the ground grid is visible on both sizes; player and rooftop-platform heights remain visually aligned. No movement, collision, route or gameplay behavior was changed.
- This removes the duplicate world-floor cube and wireframe render calls; actual frame-time improvement has not been measured on Android or desktop, so no FPS claim is made.

### Current branch and deployment state
- PR #13 remains open/unmerged at `6b7bd74fa832ffef50225307d3ed2db419222243`; exact run #268 passed.
- PR #14 remains open/draft/unmerged at `a3e7e66b8c16712171b5abaf808d7faf98f01efe`.
- Main remains `a18e39e9eca221a15d75c4070de9d06ef94abc3e`.
- Production remains **READY** at `dpl_GELuDKX6B4NragFQ6DSWjSNwFHAJ`, alias `https://tagbound.vercel.app`, deployed runtime `9115f13b4021f9052a95eea66ceaa1000f134978`. Read-only Vercel verification after run #304 confirmed it was unchanged.
- No merge, production deployment, or `dist/` edit occurred. The user-reported deployment hold remains in force.
- Service-worker cache version is aligned at `tagbound-v27` in `web/index.html`, `web/sw.js`, and the CI assertion. PR #13 remains separate and unmerged.
- Physical Android frame-time, touch, and orientation testing remains pending.

### Next task
Continue the source/test audit for any remaining gameplay or PWA edge cases in the agreed scope. Prioritize concrete uncovered failures over speculative features. Physical Android touch/performance profiling remains pending and must not be claimed from Chromium emulation. Keep changes out of `dist/`; do not merge/deploy during the hold.


## Verified checkpoint — 2026-11-23, CI run #310

- PR #14 branch `feat/campaign-imperial-before-spire` head: `a3e7e66b8c16712171b5abaf808d7faf98f01efe`. PR #14 remains open/draft/unmerged; PR #13 remains open/unmerged.
- Run #310 tested that exact commit and passed: WebAssembly compilation, C++ gameplay regressions, static PWA validation, Chromium desktop/mobile browser smoke, pause/resume, audio preference persistence, touch-input reset, save restoration, service-worker cache, and offline reload. PR-only `publish-dist` was skipped.
- Visual artifact `tagbound-visual-review` from run #310 contains 14 screenshots, including ten boss-windup captures (five cities × desktop/mobile). All five telegraph geometries and countdowns were visible at both resolutions. Radial warnings are subtler but remain discernible; no render correction was justified.
- Boss silhouettes retain city-specific distinctions: Harbor cleaver, Gilded horns/charge profile, Foundry heavy armor/hammer, Imperial crown/core, and Spire mantle/spines.
- Ported icon-only touch and pause controls into PR #14. Accessible labels and pressed state remain; the Bow action is hidden unless Skirmisher is selected. Visible touch-help text was removed. Browser assertions now test the icon-only accessible contract rather than obsolete visible sound-button text.
- Added a read-only WASM diagnostic `GetBossWindupMask` so CI can wait for each real boss's windup before screenshot capture.
- `web/index.html`, `web/sw.js`, and CI agree on `tagbound-v27`. No `dist/` files changed.
- Run #309's first browser attempt failed only because an old assertion expected visible `SOUND: OFF` text on the now icon-only pause button. The assertion was corrected to verify accessible label, `aria-pressed`, muted class, and persisted preference; run #310 passed.
- No merge, production deployment, billing change, or Vercel deployment attempt occurred. Production remains on the existing main deployment; the deployment hold remains active.
- Physical Android FPS and device-specific touch behavior remain unmeasured.


## Verified checkpoint — browser lifecycle and input recovery (2026-11-23)

- PR #14 head before this documentation update: `29fee596ac204be433ff18d25266e722b18bd249`.
- Run #314 (`45182902026`) passed on exact commit `a418bc44638cef1fc54bc0ca0f7002a43d29bfe5`: Emscripten/WebAssembly build, C++ gameplay regression suite, PWA assembly/static validation, Chromium desktop/mobile smoke and offline reload. The PR-only `publish-dist` job was skipped.
- Run #314 added browser checks and source handling for `window.blur` and `document.visibilitychange`: clear held joystick/action/look input, active-button styling and tracked pointer IDs.
- Run #315 (`45184017300`) passed on exact commit `29fee596ac204be433ff18d25266e722b18bd249`: all build, gameplay, PWA, browser, audio, save, and offline checks passed. `publish-dist` was skipped.
- Run #315 verifies ambient music pauses on browser focus loss and document backgrounding, then resumes on focus/visibility return only when gameplay remains active. Settings and the ending continue to suppress automatic resume.
- No C++ gameplay mechanics, campaign ordering, save schema, or progression behavior changed in these two fixes. No `dist/` files changed.
- PR #13 remains open/unmerged; PR #14 remains open/draft/unmerged. Main remains `a18e39e9eca221a15d75c4070de9d06ef94abc3e`.
- No merge, production deployment, Vercel deployment attempt, quota bypass, or billing change occurred. Physical Android touch and frame-time measurements remain outstanding.


## Active development checkpoint — 2026-11-24

### Boss arena containment — PR #15

- Draft PR #15: https://github.com/villanuevayanmar/tagbound/pull/15, branch fix/boss-arena-floor-bounds, currently targeted at main to run the repository's pull-request CI. It is not merged.
- The boss arena is derived from the final gate platform's actual center, top surface and dimensions. The simulated boss body is constrained by its collision radius during normal pursuit, knockback and the Gilded Charger's forward strike; boundary corrections also clear stale pursuit waypoints.
- Deterministic C++ regressions cover all five city bosses, every cardinal edge, forced vertical knockback, pursuit toward a player outside the floor, the Gilded Charger strike, and attack recovery.
- Exact-head run #320 (45269952163) passed on f97682834a3de0a98b539b2b75bac2426f218c4c: Emscripten/WebAssembly build, C++ gameplay suite, PWA validation and Chromium desktop/mobile/offline smoke. Its visual-review artifact was inspected at full resolution for all five boss arenas at desktop and mobile-landscape sizes.

### Black flicker / WebGL resilience — investigation status

- Source inspection found one application frame loop and one BeginDrawing/EndDrawing pair, with one BeginMode3D/EndMode3D pair. No per-frame model/texture loading or offscreen-render-target churn was found.
- The repository had no explicit canvas webglcontextlost / webglcontextrestored handling. A recovery path is now present in web/index.html: it prevents the browser's default loss handling, flushes current local progress through SaveNow, displays a recovery message, and reloads after context restoration (or after an 8-second timeout). A 30-second session guard prevents repeated automatic reload loops.
- A Chromium regression now deliberately loses/restores WebGL and checks that the game reloads and can start again. That new test and recovery code are not yet verified by exact-head CI as of this checkpoint.
- WebGL context loss has not been observed in the existing production/Chromium logs. The recovery is a resilience improvement, not a confirmed root-cause fix for the user's intermittent black flicker. Physical Android reproduction and graphics diagnostics are still required to identify the cause.

### Release and device constraints

- PR #13 and PR #14 remain open/unmerged; PR #14 remains draft. PR #15 remains draft and must remain stacked on PR #14 after its exact-head CI validation.
- Production remains the existing READY deployment; no merge, dist/ change, production deployment, quota bypass or Vercel billing change occurred during this work.
- Physical Android touch, frame-time and flicker verification remain unperformed. Hosted Chromium is not a substitute for the user's Android device.
- The project remains on the zero-cost path.

### Next exact task

1. Run and inspect full exact-head CI for the WebGL recovery change, including the forced context-loss Chromium test.
2. Fix any failure before marking the recovery test complete.
3. After passing, restore PR #15's base to feat/campaign-imperial-before-spire and update this checkpoint with the exact verified SHA/run.
4. Continue the black-flicker investigation without assuming context loss is the cause; then proceed to camera orientation and movement/grounding defects.


## Verified checkpoint — 2026-11-24, boss containment and WebGL recovery

### Boss arenas

- Draft PR #15: https://github.com/villanuevayanmar/tagbound/pull/15, branch fix/boss-arena-floor-bounds. During this exact-head verification pass, PR #15 is temporarily targeted at main so the repository's pull_request workflow runs; it will be restored to feat/campaign-imperial-before-spire after validation.
- Each arena derives its center, floor height and half-extents from the visible final gate platform. Boss simulation position is constrained by its body radius during pursuit, knockback and Gilded Charger strike movement. Boundary correction also clears stale path waypoints.
- Deterministic C++ regression coverage checks all five city bosses, all four boundary directions, vertical knockback, player pursuit outside the platform, Gilded Charger charge and attack recovery.

### WebGL context-loss recovery

- Added canvas webglcontextlost / webglcontextrestored handling in web/index.html. The loss event is cancelled so the browser may restore the context; TAGBOUND flushes the current in-memory save through the exported SaveNow function, displays a recovery message and reloads after restoration. An 8-second fallback reload handles a context that does not restore; a 30-second session guard prevents repeated reload loops.
- Chromium now forces a real WebGL loss through WEBGL_lose_context, triggers restoration, verifies the page reloads, and starts gameplay again. Before loss, the test deliberately changes only its localStorage fixture to city/unlocked 4; after recovery it verifies SaveNow restored the actual in-memory city/unlocked values (0), schemaVersion 1, and gameplay can start.
- Run #321 (45271918529) exposed a real defect: the first recovery implementation gated SaveNow on Module.calledRun, which is not defined by the generated Emscripten runtime. The page reloaded but the sentinel save remained. The guard was removed; the call now uses the exported Module.ccall function when available.
- Exact-head run #322 (45273216472) passed on 53fcaf598804963c1602c3acf852e7bcaf23482b. Build job: Emscripten/WebAssembly compile, C++ gameplay regressions and PWA validation all passed. Browser-smoke job: Chromium runtime, desktop/mobile checks, offline cache and the forced WebGL-loss/save-flush/reload/restart test all passed. PR-only publish-dist was skipped.

### Flicker diagnosis remains open

- Source inspection found a single application frame loop and one BeginDrawing/EndDrawing pair per frame, with one BeginMode3D/EndMode3D pair. No per-frame model/texture creation or offscreen render-target churn was found.
- Context loss is now handled and tested, but no context-loss event has been observed in the previously available production/Chromium logs. This is a recovery improvement, not proof that context loss caused the user's intermittent black flicker. The actual Android symptom still needs physical-device diagnostics.

### Release and device state

- PR #13 and PR #14 remain open/unmerged; PR #14 is draft. PR #15 remains draft and is being stacked on PR #14 after this exact-head CI pass.
- No dist/ file changed. The PR-only publish-dist step was skipped. Vercel shows no deployment for the development branch.
- Production alias tagbound.vercel.app remains READY on deployment dpl_GELuDKX6B4NragFQ6DSWjSNwFHAJ, runtime commit 9115f13b4021f9052a95eea66ceaa1000f134978. No merge, release, deployment, quota bypass or billing change occurred.
- Physical Android input, performance and black-flicker behavior remain unverified. The project remains on the zero-cost path.

### Next exact task

1. Restore PR #15's base to feat/campaign-imperial-before-spire and confirm the stacked diff/mergeability.
2. Continue P1 camera orientation: current movement is camera-relative, but camera yaw is changed by look input and is not automatically aligned to the character's movement-facing direction. Add a deliberate follow/override model and deterministic tests before tuning movement acceleration and grounding.
3. Continue the flicker investigation without assuming the context-loss path is the root cause; collect physical Android evidence only after an authorized release.


## Verified checkpoint — 2026-11-24, camera follow and arena/graphics recovery

### Third-person camera — PR #16

- Draft PR #16: https://github.com/villanuevayanmar/tagbound/pull/16, branch fix/camera-facing-follow. Its current CI base is main; after exact-head verification it will be retargeted to PR #15 (fix/boss-arena-floor-bounds), which is itself stacked on PR #14.
- Kept camera-relative controls: joystick movement is resolved against camera forward/right. Character facing changes with intended movement/aim direction.
- When movement stops and there is no aim or active manual-look override, camera yaw eases toward the character's last facing direction using shortest-angle interpolation. Manual mouse/touch look gets a 1.2-second priority window so automatic follow does not fight a swipe.
- ResetCamera derives the initial camera yaw from the restored player's spawn facing, restores the default pitch and clears the manual-look timer. StartGame calls it after the player spawn/checkpoint is restored.
- C++ regression tests cover initial camera alignment, camera-relative strafe, idle follow after release, manual-look override, follow resumption and reset. The rooftop route-transfer tests now compute their movement input in the camera's current basis, preserving the declared camera-relative control scheme.
- Exact-head run #327 (45276625519) passed on 015c6013c5db740729f33d96789a7ed2eccb5c7f: Emscripten/WebAssembly compile, full C++ gameplay regression suite, PWA validation, Chromium desktop/mobile smoke and offline-cache checks. Full-resolution browser visual artifacts were inspected for all five boss arenas at desktop and for gameplay plus Spire boss at mobile landscape.

### Previously verified foundations

- PR #15 boss-arena containment and WebGL context-loss recovery remain in draft, stacked on PR #14. Exact-head run #322 (45273216472) passed the simulated WebGL loss/restore, save flush, reload and gameplay restart; PR #15's docs head was revalidated by run #323 (45274447641).
- The black-flicker root cause remains unknown. WebGL context-loss recovery is tested resilience, not proof that context loss caused the reported symptom.
- Production alias remains READY on deployment dpl_GELuDKX6B4NragFQ6DSWjSNwFHAJ, runtime commit 9115f13b4021f9052a95eea66ceaa1000f134978. No dist/ changes, merge, deployment, billing change or quota workaround occurred.
- Physical Android camera, flicker, touch and performance checks remain unverified. Chromium visual artifacts are not device testing.

### Next exact task

1. Run exact-head CI after these documentation/control-description updates, then stack PR #16 on PR #15 and verify both PR diffs/mergeability.
2. Continue P1 movement and parkour grounding: acceleration/deceleration, turn response, sprint/air control, jump and landing consistency, ledge/vault/climb transitions and collision/visual alignment. Add deterministic tests before tuning.
3. Keep investigating flicker with evidence and obtain physical Android observations only after an authorized release. Continue toward the remaining approved scope (medieval-fantasy city layout, floor/zone encounters, separate stamina/mana, stat/skill milestones, fog of war, guaranteed boss keys and performance-based loot) in testable increments.


## Verified checkpoint — 2026-11-24: movement response and stacked PR verification

### Exact branch / PR state
- Main remains `a18e39e9eca221a15d75c4070de9d06ef94abc3e`; production has not been changed.
- PR #14: `feat/campaign-imperial-before-spire` at `814d2e86c0c7cb5afc7b794e0d26d054eed87222`, based on `main`; exact-head run #317 (`45185250016`) passed.
- PR #15: `fix/boss-arena-floor-bounds` at `c92136f54d38df42aa239342dc803e784182ac73`, based on PR #14; exact-head run #323 (`45274447641`) passed.
- PR #16: `fix/camera-facing-follow` at `9a83d69b2862b1329467952e4cafa020e25d75fd`, based on PR #15. GitHub compare confirms 13 commits ahead and zero behind. All three PRs remain open drafts; none is merged.

### Movement response implementation — PR #16
- Added transient horizontal locomotion velocity to `Player`, separate from the existing physics velocity used for vertical motion and impulses.
- Ground acceleration is 34 units/s² and ground braking is 28 units/s². Air steering acceleration is 16 units/s² and air braking is 3.5 units/s², retaining more horizontal momentum while airborne.
- Existing class top speeds and input direction calculations are retained. Dash uses a faster 100 units/s² acceleration while active so the existing 2.7× burst remains reachable inside its 0.24-second window.
- Horizontal collision stops the locomotion and impulse components on the blocked axis. City rebuild, ledge/climb transitions and death/checkpoint reset the transient movement velocity; it is not persisted as save data.
- Added deterministic tests for the initial movement ramp, normal top speed, release braking, weak air steering, the dash burst and the Vanguard/Skirmisher speed distinction.
- The first new CI attempt exposed a test-fixture issue: its synthetic floor top was at Y=0 while the player spawn feet are at Y=0.3, leaving the test player airborne. Fixtures were corrected to use the actual spawn-floor height; the production movement code was not changed to accommodate the bad fixture.

### Exact-head verification
- Run #334 (`45329538797`) passed on exact source head `9a83d69b2862b1329467952e4cafa020e25d75fd`.
- Build job passed: C++ gameplay regressions, Emscripten/WebAssembly compilation, static PWA assembly and validation.
- Browser-smoke job passed: Chromium runtime, desktop/mobile-landscape behavior, save restoration, offline cache and forced WebGL context-loss/recovery checks.
- The `tagbound-visual-review` artifact was uploaded and inspected: desktop/mobile gameplay and all five boss-gate scenes rendered. The browser test confirmed boss containment during the captures.
- PR-only `publish-dist` was skipped. No generated files were committed to `dist/`.

### Unresolved issues and release state
- Intermittent black flickering remains unresolved. Source audit still finds one `BeginDrawing/EndDrawing` pair and one `BeginMode3D/EndMode3D` pass per frame, with no repeated per-frame model/texture allocation identified. WebGL context-loss recovery is a tested resilience path, not proof of the flicker cause.
- Chromium sampling and screenshots do not replace physical Android testing. Device GPU/browser behavior, real touch feel and sustained frame pacing remain unverified.
- Production alias `https://tagbound.vercel.app` remains on the older READY runtime commit `9115f13b4021f9052a95eea66ceaa1000f134978`. No merge, production deployment, Vercel upgrade, billing change or quota workaround was performed. Cost remains ₱0/$0.

### Next exact task
1. Continue the black-frame investigation with a repeatable multi-frame canvas check in hosted Chromium; distinguish a fully black framebuffer from normal dark scene content and preserve existing visual assertions. Do not call this an Android fix.
2. Continue movement foundation regressions for jump/landing, grounding and rooftop transitions, then review ledge/vault/climb collision and enemy navigation.
3. After the foundation is stable, implement the master specification's remaining systems incrementally: separate stamina/mana, stat choice on every level, skill-choice/upgrade milestones, vertical city/floor progression, enemy archetypes, fog-of-war discovery, guaranteed boss keys and performance-banded equipment rewards. Keep save migration, deterministic tests and mobile UI coverage in scope.



## Latest diagnostic update — 2026-11-24: repeated canvas-frame sampling

- Added a browser-smoke diagnostic in `.github/workflows/ci.yml` on source tree head `e5da108ec84281c2e08eb1bff3dca4244b7055d5`. It samples the WebGL canvas 12 times during desktop gameplay and 12 times during mobile-landscape gameplay.
- Each captured canvas frame is decoded with the existing `pngjs` dependency. The check requires at least 97% of pixels to have one RGB channel above 12; this intentionally permits the normal dark palette but fails a near-total black canvas. A failure includes the sample index and visible-pixel ratio.
- This is a repeatable browser diagnostic for fully/overwhelmingly black frames, not a proven root-cause fix and not evidence about physical Android GPUs. Sampling can still miss very brief events between captures.
- Exact workflow run #335 (`45330722930`) passed on workflow/source tree head `e5da108ec84281c2e08eb1bff3dca4244b7055d5`. Build/gameplay tests, WebAssembly, PWA validation and Chromium browser-smoke all passed; `publish-dist` was skipped. The browser log explicitly reports 12/12 desktop and 12/12 mobile-landscape samples passed with a minimum non-black pixel ratio of `1.0000` on both.
- PR #16 is restored to base `fix/boss-arena-floor-bounds`, still open/draft; PR #14 → PR #15 → PR #16 remains the intended stack. No merge, deployment or `dist/` change was made.
- The 14-image visual-review artifact (`14892037925`) was inspected. Desktop/mobile gameplay and five boss-arena scenes rendered. The canvas sampling did not detect a black frame, but this is a short Chromium sample window and does not prove the intermittent report is fixed.
- Added a test-only raised-rooftop landing regression in `tests/gameplay.cpp` at commit `1753b20e0ac2c6e796ddfbbd58601fdaaa6e476c`: falling onto a platform with top height Y=2.3 must snap and remain at Y=2.3, with vertical velocity reset and landing recovery started. It was included in run #336.



## Latest exact-head verification — 2026-11-24

- Run #336 (`45332306488`) passed on PR #16 application source/test head `c0334c0d71bafda76e9a67a72dd5d9aebceede3c`, including the raised-rooftop landing regression and canvas-frame diagnostic.
- Build job `138236093166` passed: C++ gameplay regression tests, Emscripten/WebAssembly compilation, PWA assembly and PWA validation.
- Browser-smoke job `138236454555` passed: Chromium desktop/mobile runtime, save restoration/migration, offline cache, five-city boss-arena visual captures, forced WebGL context-loss recovery and screenshots.
- Browser logs explicitly report 12/12 desktop and 12/12 mobile-landscape canvas samples passed, with minimum non-black pixel ratio `1.0000` on both.
- Visual-review artifact `14892717266` was uploaded (14 screenshots). The desktop gameplay capture was inspected. The CI canvas sampling found no overwhelmingly black frames during the sampled intervals; it does not prove the intermittent black flicker is permanently fixed or establish Android GPU behavior.
- PR-only `publish-dist` was skipped. The workflow was started while PR #16 temporarily targeted `main` to satisfy the repository's current PR trigger filter, then the PR was restored to base `fix/boss-arena-floor-bounds`.
- Current application source/test head verified is `c0334c0d71bafda76e9a67a72dd5d9aebceede3c`. Later changes to this branch tip are documentation-only. PR #14 → PR #15 → PR #16 remains intact; no merge, deployment or `dist/` change was made.



## Active implementation checkpoint — fortified city perimeters (2026-11-24)

- Added five connected wall sections per city (west, east, south, and two north sections) using the existing solid-platform collision system. Each wall is 12 units high and 2 units thick.
- The north opening is centered on the city's existing boss-gate route. Two gate towers and a lintel define the entrance; skyline pieces that obstruct the approach are relocated beyond the playable boundary.
- Added deterministic C++ coverage for all five city layouts, wall dimensions and gate alignment, player collision against all four sides, and passage through the gate.
- Source/test commits: `cca3a538340e058bc0068ffa873e3b0197a07ae7`, `fd61e6afc1194230c621defe9ce22dd82be854a8`, `bdd3e22b78b21f8725114412612e65d02238d89b`. Checkpoint documentation head: `eaf84617467a6b3830ce36cf5528dd333c5d08c4`.
- Exact-head CI is **pending**. The previously passing run #336 predates these changes and does not verify them. Do not mark the wall feature complete until the updated gameplay suite, Emscripten build, PWA validation and browser smoke pass.
- PR #16 has been restored to its original draft base PR #15. PR #15 remains based on PR #14. No PR was merged, no production deployment was attempted, and no generated `dist/` files were changed.
- Production remains on the existing READY deployment. The ₱0/$0 restriction and release hold remain in force.

### Next steps

1. Run the CI workflow on the exact current PR #16 source/test head and fix any failures before calling the walls verified.
2. Review the visual layout and collision in the generated five-city scenes; pay particular attention to the gate approach and skyline clearance.
3. Continue the approved remaining systems in dependency order: floor/zone progression, separate Mana alongside the existing stamina/energy resource, level-up stat and milestone choices, enemy archetypes/skills, fog-of-war exploration, and boss key/performance-based equipment rewards.
4. Keep PR #16 stacked on PR #15, avoid `dist/` changes, and do not merge or deploy without the required release authorization and verified zero-cost deployment capacity.
