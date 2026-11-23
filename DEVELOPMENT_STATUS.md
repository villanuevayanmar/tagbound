# TAGBOUND Development Status

Last updated: 2026-11-23

## Project state

**Playable production build available — continued development in progress.**

Repository: https://github.com/villanuevayanmar/tagbound  
Production: https://tagbound.vercel.app  
Current main commit (verified at session start): `a18e39e9eca221a15d75c4070de9d06ef94abc3e`  
Latest tested runtime bundle commit: `9115f13b4021f9052a95eea66ceaa1000f134978`  
Production deployment: `dpl_GELuDKX6B4NragFQ6DSWjSNwFHAJ`  
Production deployment state: **READY**  
Production alias: `tagbound.vercel.app` → `dpl_GELuDKX6B4NragFQ6DSWjSNwFHAJ`

The latest generated WebAssembly bundle is deployed. The live production Chromium suite passed against the public production alias. Physical Android testing has not yet been performed by the assistant; the user can now test the deployed game on Android.

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

- User has no laptop. Do not request local commands or local development testing.
- The public production build is now READY and has passed live hosted Chromium tests.
- The next useful verification is a real Android play session on the user's phone: install/open the PWA, choose each class, test touch movement and attacks, open Settings while moving/holding buttons, Resume, sound/music pause, and complete several beacon and boss encounters.
- Record real-device findings and reproduce/fix them through GitHub/cloud workflows.

### Gameplay and campaign

- Tune city difficulty, reinforcement pacing, boss health/damage, stamina costs and encounter duration based on actual play sessions.
- Review all boss telegraphs, hitboxes and dodge/block counterplay, especially Foundry Colossus and Imperial Regent radial attacks.
- Expand quest variety beyond reach and timed hold beacons.
- Validate city transitions, boss defeat progression, ending/restart and save restoration at every checkpoint.
- Continue reviewing enemy pathfinding in dense rooftop geometry and reinforcement spawn edge cases.

### Visuals and animation

- Improve procedural player/enemy models, equipment silhouettes and class readability.
- Improve animation blending and movement/combat poses.
- Further distinguish the five city environments and rooftop routes.
- Review camera behavior, HUD legibility, visual effects and performance on mid-range Android hardware.

### Audio

- Tune ambient score melody/spacing, loudness and balance against combat effects.
- Confirm Web Audio unlock/resume behavior and audio balance on a physical Android device.

### Release readiness

- Continue code improvements from actual Android play feedback.
- Fix critical/high-severity device bugs through GitHub, rebuild and verify via hosted CI, let Vercel deploy the generated `dist/` bundle, then re-run live production smoke checks.
- Keep project cost at ₱0/$0; no paid API, subscription or usage-billed service.

## Testing and development constraints

- The user does not have a laptop. Never require local commands or local build/test steps.
- Use GitHub-hosted CI and authorized cloud workflows for development verification.
- Ask for Android testing only when the deployed production alias has been verified READY and the live smoke suite has passed.
- Keep cost at ₱0/$0. No paid API, paid subscription, paid hosting upgrade, billing change or quota bypass without explicit approval.
- Never claim a test, commit, merge, deployment or device test that has not actually occurred.

## Exact next task

1. Have the user perform the first production Android playtest now that live smoke has passed.
2. Collect the first concrete device findings (loading/performance, controls, class weapons, Settings/pause, audio, beacon captures, boss warnings, save restore, install/offline).
3. Prioritize and reproduce the most serious issue through GitHub-hosted code and tests.
4. Preserve the current READY deployment until the next runtime change has passed hosted CI and a new READY production deployment is available.


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
