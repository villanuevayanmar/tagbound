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

- Added an in-game Settings entry point and overlay, with Resume.
- Pauses the actual C++ simulation while Settings is open.
- Home and Settings share the saved sound preference.
- PWA installation appears only on Home and Settings, never during active gameplay.
- Bumped the service-worker cache to `tagbound-v25`.

### Gameplay HUD rendering and Settings layout — PR #2

- Added the missing Raylib `BeginDrawing()` / `EndDrawing()` frame boundaries.
- Confirmed the health, stamina, level, danger, objective and city/quest HUD render in hosted Chromium screenshots.
- Moved Settings into the top HUD gap; narrow landscape screens use a compact gear control.
- Added screenshot-pixel validation and rectangle-based checks that Settings does not overlap HUD regions.

### Pause input reset — PR #3

- Opening Settings clears joystick movement, touch action bits, camera-look deltas, active-button styling and tracked pointer captures.
- Hosted desktop/mobile browser tests prove all touch/look state is cleared.

## Current work — feat/procedural-background-score

Implemented on a feature branch; awaiting hosted CI and PR review:

- Added an original ambient score using Web Audio oscillators and filters. It is generated locally; no downloaded music assets, external audio API, or paid dependency is added.
- The score remains stopped on Home and starts only after the user taps Start.
- Opening Settings pauses the score; Resume continues the same motif.
- Toggling the shared sound setting OFF and ON while paused does not restart the score before Resume.
- The score stops when the five-city victory ending appears and does not start for an already-completed saved run.
- Extended hosted desktop and mobile tests to check Home stopped → Start playing → Settings paused → sound toggle remains paused → Resume playing.
- The same lifecycle checks are included in the manual live-production smoke workflow.

## Verified cloud testing

- Settings feature passed WebAssembly build, C++ gameplay regression and desktop/mobile Chromium checks in run `45073264491`.
- HUD rendering fix and Settings layout passed screenshot-pixel/no-overlap checks in run `45074803692`.
- Pause input reset passed desktop/mobile input-state assertions in run `45075281567`.
- The procedural-score branch is awaiting hosted CI. Record the run ID/result after it completes.
- Hosted browser checks are not physical Android playtesting.

## Deployment blocker — Vercel

The current production deployment is still the old SHA `a15a767`.

- The Vercel deployment API rejected manual deployment creation with HTTP 402, resource `api-deployments-free-per-day`; the free deployment API allowance exceeded its 24-hour request cap.
- Vercel Git checks report `build-rate-limit`.
- A minimal temporary Vercel Sandbox was used only to check for an existing CLI login. No Vercel CLI/token was configured; the Sandbox was stopped without deploying.
- No paid plan, payment method, usage override or quota bypass was enabled.
- Latest source and generated `dist/` files are in GitHub. That does not mean they are deployed.

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

- Tune ambient score melody/spacing, loudness and its balance against combat effects.
- Test Web Audio unlock/resume behavior on a real Android device after deployment.
- Confirm sound mute preference, Settings pause/resume and victory-ending stop behavior remain correct.

### Release readiness

- Merge the procedural score only after the hosted WebAssembly build, C++ regression tests and desktop/mobile browser lifecycle tests pass.
- Check main CI after merge.
- When free Vercel deployment limits permit, obtain a READY deployment for latest main without upgrading or paying.
- Confirm `tagbound.vercel.app` points to that exact deployment.
- Run manual live-production Chromium only after deployment READY.
- Only then ask the user to test on Android; record device bugs, fix through GitHub, redeploy, and repeat.

## Testing and development constraints

- The user does not have a laptop. Never ask them to run local commands or test incomplete builds.
- Use GitHub-hosted CI and authorized cloud workflows for development verification.
- Android playtesting only begins after a READY production deployment and verified current alias.
- Keep project cost at ₱0/$0. No paid API, paid subscription, paid hosting upgrade, billing change, or usage-billed bypass without explicit approval.
- Never claim a test, commit, merge, deployment or device test that has not actually occurred.

## Exact next task

1. Open a PR from `feat/procedural-background-score` to `main`.
2. Inspect the GitHub Actions results and fix any JS/audio timing or browser-policy failures.
3. Require desktop and landscape-mobile tests for Home stopped → Start playing → Settings paused → sound OFF/ON while still paused → Resume playing.
4. Require the victory-ending check to confirm the ambient score is stopped.
5. Merge only after all hosted build and browser tests pass.
6. Recheck Vercel later, but do not exceed its free quota. Android testing stays pending until latest main is deployed READY.
