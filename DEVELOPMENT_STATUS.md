# TAGBOUND Development Status

Last updated: 2026-11-23

## Project state

**Active development — not release-ready.**

Repository: https://github.com/villanuevayanmar/tagbound  
Production: https://tagbound.vercel.app  
Main at start of this branch: `c373139c25f3bc3ab3ff867e137a51f6cf0f89c6`  
Current production deployment: `dpl_EiD7wy3TYXzWUD71ZhNV5fcGA7QD`  
Current production commit: `a15a767daaa76e7abe7d053d76c62c4d4522f7b0`

The production URL is still serving the older deployment. Do not send the user to test the new release until Vercel reports a READY deployment for latest main and the production alias points to it.

## Recently completed and merged

### Settings and pause system — PR #1

- Added an in-game Settings entry point and overlay.
- Added Resume without restarting the run.
- Pauses the actual C++ simulation while Settings is open.
- Home and Settings share the saved sound preference.
- PWA installation appears only on Home and Settings, never during active gameplay.
- Updated the service-worker cache to `tagbound-v25`.

### Gameplay HUD rendering and Settings layout — PR #2

- Added the missing Raylib `BeginDrawing()` / `EndDrawing()` frame boundaries.
- Confirmed the health, stamina, level, danger, objective and city/quest HUD render in hosted Chromium screenshots.
- Moved Settings into the top HUD gap; narrow landscape screens use a compact gear control.
- Added screenshot-pixel validation and rectangle-based checks that Settings does not overlap HUD regions.

### Pause input reset — PR #3

- Opening Settings clears joystick movement, touch action bits, camera-look deltas, active-button styling and tracked pointer captures.
- Added hosted C++ state assertions and desktop/mobile browser checks proving all touch/look state is cleared.

## Current work — feat/procedural-background-score

Implemented on a feature branch; awaiting hosted CI and PR review:

- Added an original ambient score synthesized locally in the browser through the Web Audio API.
- No downloaded music assets, external audio API, or paid dependency.
- Music starts only after the player taps Start.
- Music pauses immediately when Settings opens and resumes after Resume.
- The existing saved sound toggle controls both effects and music.
- Turning sound OFF during a pause does not restart music while the Settings overlay remains open.
- The score stops when the five-city victory ending appears.
- Extended hosted desktop and mobile browser tests to verify Home stopped, Start playing, Settings paused, sound toggle remains paused, and Resume playing.

## Verified cloud testing

- Settings feature passed WebAssembly build, C++ gameplay regression and desktop/mobile Chromium checks in run `45073264491`.
- HUD drawing fix and Settings layout passed browser screenshot pixel/no-overlap checks in run `45074803692`.
- Pause input reset passed desktop/mobile touch/look assertions in run `45075281567`.
- Previous main CI for the merged HUD fix passed in run `45075096367`.
- A new CI run for the procedural score will be recorded after the PR branch workflow completes.
- Hosted CI is not physical Android playtesting.

## Deployment blocker — Vercel

The current production deployment is still the old SHA `a15a767`.

- The Vercel deployment API rejected manual deployment creation with HTTP 402, resource `api-deployments-free-per-day`; the free deployment API allowance exceeded its 24-hour request cap.
- Vercel Git checks report `build-rate-limit`.
- A minimal, temporary Vercel Sandbox was used only to check for an existing CLI login. No Vercel CLI/token was configured; the Sandbox was stopped without deploying.
- No paid plan, payment method, usage override, or quota bypass was enabled.
- The latest source and PWA bundle are in GitHub. That does not mean they are deployed.

## Remaining development work

### Gameplay and campaign

- Complete full five-city playthroughs with all three classes.
- Tune city difficulty, reinforcement pacing, boss health/damage, stamina costs and encounter duration from observed play sessions.
- Review boss telegraphs, hitboxes and dodge/block counterplay.
- Expand quest variety beyond beacon traversal and boss gates.
- Validate city transitions, boss defeat progression, ending/restart behavior and save restoration at every checkpoint.
- Review enemy pathfinding in dense rooftop geometry and reinforcement spawn edge cases.

### Visuals and animation

- Improve procedural player/enemy models, equipment silhouettes and class readability.
- Improve animation blending and movement/combat poses.
- Further distinguish the five city environments and rooftop routes.
- Review camera behavior, HUD legibility, visual effects and performance on mid-range Android hardware.

### Audio

- Tune ambient score note progression, loudness and how it sits behind gameplay effects.
- Test autoplay/unlock behavior on real Android Chrome after deployment.
- Ensure mute preference, Settings pause/resume and victory ending continue to behave correctly.

### Release readiness

- Merge procedural music only after hosted WebAssembly and desktop/mobile browser tests pass.
- Check current main CI after merge.
- When free Vercel deployment limits permit, obtain a READY deployment for latest main without upgrading or paying.
- Confirm `tagbound.vercel.app` points to that exact deployment.
- Run the manual live-production Chromium suite only after deployment is READY.
- Only then ask the user to test on Android.
- Record device bugs, fix through GitHub, redeploy and repeat until release criteria are met.

## Testing and development constraints

- The user does not have a laptop. Never ask them to run local commands or test incomplete builds.
- Use GitHub-hosted CI and authorized cloud workflows for development verification.
- Android playtesting only begins after a READY production deployment and verified current alias.
- Keep project cost at ₱0/$0. No paid API, paid subscription, paid hosting upgrade, billing change, or usage-billed bypass without explicit approval.
- Never claim a test, commit, merge, deployment or device test that has not actually occurred.

## Exact next task

1. Open a PR from `feat/procedural-background-score` to `main`.
2. Inspect GitHub Actions results. Fix any TypeScript/JS, timing, or mobile audio-policy issues.
3. Verify score transitions on both desktop and mobile: stopped Home → playing on Start → paused in Settings → stays paused if muted/unmuted → playing on Resume; victory ending stops it.
4. Merge only after the WebAssembly build, C++ regression and desktop/mobile Chromium checks pass.
5. Check Vercel deployment status but do not exceed free quotas. Android testing remains pending until latest main is live.
