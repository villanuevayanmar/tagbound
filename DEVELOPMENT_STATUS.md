# TAGBOUND Development Status

Last updated: 2026-11-23

## Project state

**Active development — not release-ready.**

Repository: https://github.com/villanuevayanmar/tagbound  
Production: https://tagbound.vercel.app  
Current main commit: `5476f40dd7e897984ef702bbdafc57eec9804b6e`  
Current production deployment: `dpl_EiD7wy3TYXzWUD71ZhNV5fcGA7QD`  
Current production commit: `a15a767daaa76e7abe7d053d76c62c4d4522f7b0`

The production URL is still serving the older deployment. Do not send the user to test the new release until Vercel reports a READY deployment for the latest main build and the production alias points to it.

## Recently completed and merged

### Settings and pause system

Merged in PR #1, commit `954c8fe77a8aa1667e1bdec94116bc3816765c59`.

- Added an in-game Settings entry point and overlay.
- Added Resume without restarting the run.
- Pauses the actual C++ simulation while Settings is open.
- Home and Settings share the saved sound preference.
- PWA install is available only on Home and Settings, never during active gameplay.
- Reset touch controls while opening the Settings overlay.
- Bumped the service-worker cache to `tagbound-v25`.

### Gameplay HUD rendering and Settings layout

Merged in PR #2, commit `5476f40dd7e897984ef702bbdafc57eec9804b6e`.

- Added the missing Raylib `BeginDrawing()` / `EndDrawing()` boundaries around every screen-rendering frame.
- Confirmed the health, stamina, level, danger, objective and city/quest HUD now render in hosted Chromium screenshots.
- Moved the Settings control into the top HUD gap.
- Added a compact gear-style control for narrower landscape screens.
- Added rectangle-based assertions to verify Settings does not overlap the health, level/danger, compass, boss-warning or mobile objective HUD areas.
- Added screenshot-pixel validation for actual HUD rendering.

## Verified cloud testing

- The Settings feature passed its PR WebAssembly build, C++ gameplay regression tests, and desktop/mobile Chromium checks in run `45073264491`.
- The HUD rendering fix passed Emscripten compilation and the C++ gameplay regression suite in run `45074237401`.
- The HUD fix and top-row Settings layout passed desktop/mobile Chromium, including screenshot pixel and no-overlap assertions, in run `45074803692`.
- These are hosted browser checks. They do not replace physical Android playtesting.
- Main-branch CI for the merged HUD fix: run `45075051487` (check its terminal status before the next release step).

## Current deployment blocker — Vercel

The new production deployment has not been completed.

- The Vercel deployment API rejected a manual deployment request with HTTP 402, resource `api-deployments-free-per-day`, because the free deployment API allowance had exceeded 100 requests in the current 24-hour window.
- Vercel's Git check also reported `build-rate-limit` and links to an upgrade prompt.
- A temporary, small Vercel Sandbox was created only to check for an existing CLI login. It had no Vercel CLI or Vercel token available, so it was stopped immediately. No deployment was made through it.
- Do not upgrade plans, add payment details, bypass free-tier limits, or use a paid build service.
- The existing Vercel production alias remains on commit `a15a767`. The latest source and generated `dist/` files are committed to GitHub, but that does not mean they are deployed.

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

- Sound effects are synthesized in-browser.
- Background music is not implemented. Any future music must be original or openly licensed and bundled without paid services or runtime downloads.
- Tune sound balance and confirm the saved mute preference remains synchronized between Home and Settings.

### Release readiness

- Obtain a READY Vercel deployment for the latest main commit without exceeding free limits.
- Confirm `tagbound.vercel.app` points to that exact deployment.
- Run the manual live-production Chromium suite after deployment; it covers startup, WebAssembly/WebGL, class selection, gameplay, save restoration, Settings/pause/resume, sound synchronization, install-button placement, visible HUD, PWA cache and offline reload, mobile controls and portrait guidance.
- Only after the live suite passes, ask the user to test on Android.
- Record physical-device issues, fix them through GitHub, redeploy and repeat until release criteria are met.

## Testing and development constraints

- The user does not have a laptop. Do not ask them to run local commands or test incomplete builds.
- Use GitHub-hosted CI and authorized cloud workflows for development verification.
- Actual Android testing happens only after the production deployment is READY and its public URL has been verified.
- Keep the project zero-cost. No paid APIs, subscriptions, paid hosting changes, usage-billed services or payment methods without explicit approval.
- Preserve existing project files and data. Do not claim a test, commit, merge, deployment or device test that has not actually happened.

## Exact next task

1. Check the terminal status of main-branch CI run `45075051487`.
2. Recheck Vercel deployment quota/build status. Do not attempt repeated deployment creation while its quota is exhausted.
3. When the free deployment allowance is available, deploy or verify the automatic Git deployment for the latest main commit.
4. Verify the production alias points to the new READY deployment.
5. Rerun the failed live-production smoke job `45073565905` only after the new deployment is live.
6. Once live tests pass, tell the user the production URL is ready for Android testing.
