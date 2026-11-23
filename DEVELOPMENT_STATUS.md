# TAGBOUND Development Status

Last updated: 2026-11-23

## Project state

**Active development — not release-ready.**

Repository: https://github.com/villanuevayanmar/tagbound  
Production: https://tagbound.vercel.app  
Current main commit: `ad40f8968b8bde81367ab928d4cab3d9d81a0d1f`  
Latest tested generated WebAssembly bundle: `4c194fd0b55342ed093530086ef7765ef92619bd`  
Current production deployment: `dpl_EiD7wy3TYXzWUD71ZhNV5fcGA7QD`  
Current production commit: `a15a767daaa76e7abe7d053d76c62c4d4522f7b0`

The production URL is not yet serving the latest runtime bundle. Do not ask the user to test the release until Vercel reports a READY production deployment for the latest runtime-changing main commit and the production alias points to it.

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

- Added original ambient music synthesized locally using Web Audio oscillators and filters.
- No downloaded music assets, external audio API or paid dependency.
- Score stays stopped on Home, starts after Start, pauses in Settings, resumes with the run, follows the shared sound toggle and stops on the victory ending.
- Hosted desktop/mobile tests verify generated note count, pause, no notes while paused, resumed notes and saved-victory silence.

### Class-specific BOW control — PR #5

- BOW touch action is hidden for Vanguard and Warden and shown only for Skirmisher.
- Fixed loadouts remain Vanguard → Sword, Skirmisher → Bow, Warden → Shield.
- Hosted desktop/mobile tests verify action visibility and saved weapons for each class.

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

- Replaced the prior Axe-only boss-transition test with a full five-city progression loop for Vanguard, Skirmisher and Warden.
- Verifies the fixed Sword, Bow and Shield loadouts defeat each city's boss.
- Skirmisher's test uses actual Bow projectile travel and impact.
- Verifies city unlock progression and the fifth-city ending for all three classes.

## Verified cloud testing

- Settings feature: run `45073264491` passed.
- HUD/render fix and Settings layout: run `45074803692` passed.
- Pause input reset: run `45075281567` passed.
- Ambient score: run `45076329628` passed.
- Class-specific BOW control: run `45077008462` passed.
- Hold-beacon PR checks: run `45077905618` passed.
- Main after hold-beacon work: run `45078141314` passed.
- Death-frame fix: run `45078544251` passed.
- Main after PR #7 and generated output sync: run `45078768095` passed.
- PR #8 all-class five-city campaign regression: run `45104171971` passed.
- Main after PR #8: run `45104378957` passed, including Emscripten build, C++ gameplay regressions, PWA validation, desktop/mobile Chromium and publish-dist.
- These hosted checks do not replace playtesting on a physical Android device.

## Deployment status — Vercel

The latest runtime bundle is committed to GitHub, but production is still on commit `a15a767`.

- The latest production alias remains `tagbound.vercel.app` → deployment `dpl_EiD7wy3TYXzWUD71ZhNV5fcGA7QD`.
- The attempted Vercel deployment for PR #8's test-only commit `ad40f896` was canceled by the configured Ignored Build Step. Vercel ran `git diff --quiet HEAD^ HEAD -- dist/`; exit code 0 means that commit did not change `dist/`, so Vercel intentionally skipped a deployment. This is expected for a test-only PR.
- There is no production deployment record for the latest runtime bundle commit `4c194fd`.
- Earlier manual Vercel deployment attempts hit the free daily deployment API allowance, and the Git integration reported `build-rate-limit`.
- Do not upgrade plans, add payment details, bypass limits or use paid build services.
- A current green PR Vercel status does not mean the production alias has been updated.

## Remaining development work

### Gameplay and campaign

- Add explicit counterplay tests for the radial strikes used by Foundry Colossus and Imperial Regent.
- Tune city difficulty, reinforcement pacing, boss health/damage, stamina costs and encounter duration based on actual play sessions.
- Review all boss telegraphs, hitboxes and dodge/block counterplay.
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
- Test Web Audio unlock/resume behavior on real Android Chrome after deployment.

### Release readiness

- Obtain a READY production deployment for the latest runtime-changing main commit without paid upgrades or quota bypasses.
- Confirm `tagbound.vercel.app` points to that exact deployment.
- Run manual live-production Chromium only after deployment READY.
- Only then ask the user to test on Android; record device bugs, fix through GitHub, redeploy and repeat.

## Testing and development constraints

- The user does not have a laptop. Never require local commands or ask them to test incomplete builds.
- Use GitHub-hosted CI and authorized cloud workflows for development verification.
- Actual Android playtesting begins only after a READY production deployment and verified current alias.
- Keep cost at ₱0/$0. No paid API, paid subscription, paid hosting upgrade, billing change or usage-billed bypass without explicit approval.
- Never claim a test, commit, merge, deployment or device test that has not actually occurred.

## Exact next task

1. Add focused C++ regression tests for the Foundry Colossus and Imperial Regent radial attack boundaries: inside their radius should take the attack, while a player outside the telegraphed radius should evade it.
2. Run Emscripten build, C++ regression, and desktop/mobile Chromium checks; merge only after they pass.
3. Keep reviewing boss counterplay without changing balance values until a real Android playtest can inform tuning.
4. Recheck Vercel deployment eligibility later. Do not create paid usage or attempt to bypass free-tier limits.
