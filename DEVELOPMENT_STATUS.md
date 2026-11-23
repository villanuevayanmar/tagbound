# TAGBOUND Development Status

Last updated: 2026-11-23

## Project state

**Playable production build available — continued development in progress.**

Repository: https://github.com/villanuevayanmar/tagbound  
Production: https://tagbound.vercel.app  
Current main commit: `f778777accc3ba26cd876a338d180a9fb999187c`  
Latest tested runtime bundle commit: `679a6d6ae5d7b3b338e91d9cb000d47b9e2615ac`  
Production deployment: `dpl_XKCpxqGJeALKmogMdui1HLboqJqo`  
Production deployment state: **READY**  
Production alias: `tagbound.vercel.app` → `dpl_XKCpxqGJeALKmogMdui1HLboqJqo`

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

- Production deployment ID: `dpl_XKCpxqGJeALKmogMdui1HLboqJqo`
- Deployed runtime bundle commit: `679a6d6ae5d7b3b338e91d9cb000d47b9e2615ac`
- Production alias: `tagbound.vercel.app`
- Vercel deployment metadata and alias lookup both confirmed the production alias points to this READY deployment.
- The current main SHA `f778777...` contains only the live-smoke workflow trigger change after the deployed runtime bundle; no game code was changed after the deployed bundle.
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
