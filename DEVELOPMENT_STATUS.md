# TAGBOUND Development Status

Last updated: 2026-11-23

## Project state

**Active development — not release-ready.**

Repository: https://github.com/villanuevayanmar/tagbound  
Production: https://tagbound.vercel.app  
Current production deployment: `dpl_EiD7wy3TYXzWUD71ZhNV5fcGA7QD`  
Production deployment commit: `a15a767daaa76e7abe7d053d76c62c4d4522f7b0`  
Latest main commit at the start of this work: `afcf1543d47d6253c4079481aa2ec4d924d58dc9`

## Verified before this feature branch

- GitHub Actions WebAssembly build and C++ gameplay regression suite passed on main.
- Hosted Chromium smoke tests passed for WebAssembly/WebGL startup, class selection, gameplay start, local save restoration, PWA precache, offline reload, mobile landscape controls, and portrait rotation guidance.
- Live production smoke test passed against `https://tagbound.vercel.app`.
- Vercel production deployment was READY and its production alias pointed to the deployment above.
- These checks do not replace full playtesting on a physical Android device.

## Current work — feature/settings-pause-screen

Implemented on the feature branch; awaiting pull-request CI and live deployment verification:

- Added an in-game Settings entry point.
- Added a Settings overlay with Resume, shared sound preference, and PWA installation.
- The C++ game simulation pauses while Settings is open and resumes without rebuilding the run.
- PWA installation is offered only on Home and in Settings; neither install button is visible during active gameplay.
- Bumped the service-worker cache to `tagbound-v25`.
- Added desktop and mobile browser assertions for install-button placement, sound synchronization, pause/resume behavior, and Settings visibility.
- Added a player-position getter used by browser tests to verify that simulation time stops while paused.

## Remaining development work

### Gameplay and campaign

- Complete full five-city playthroughs with all three classes.
- Tune city difficulty, reinforcement pacing, boss health, boss damage, stamina costs, and encounter lengths from observed play sessions.
- Review every boss attack pattern for readable telegraphs, fair hitboxes, and reliable dodge/block counterplay.
- Expand quest variety beyond beacon traversal and boss gates; ensure objectives remain meaningful and distinct in every city.
- Validate city transitions, defeated-boss progression, ending/restart behavior, and save restoration across every checkpoint.
- Review enemy pathfinding in dense rooftop geometry and edge cases where enemies can stall or spawn poorly.

### Visuals and animation

- Improve the procedural low-poly player and enemy models, equipment silhouettes, and class readability.
- Improve animation blending and action-specific movement/combat poses.
- Increase the visual distinction and navigational identity of the five city environments and their rooftop routes.
- Review camera behavior, HUD legibility, visual effects, and frame pacing on mid-range Android hardware.

### Audio

- Current sound effects are synthesized in-browser.
- Background music is not implemented; assess a small original or openly licensed soundtrack only if it can be bundled without adding paid services or external runtime dependencies.
- Tune sound balance and confirm mute preference remains consistent across Home and Settings.

### Release readiness

- Complete the Settings feature's pull-request build and browser tests.
- Merge only after the branch checks pass.
- Wait for the tested WebAssembly artifact to publish and Vercel production to become READY.
- Run the live production browser suite against the deployed production URL.
- Only then ask for Android device testing.
- Record physical-device bugs, fix them through GitHub, redeploy, and repeat until release criteria are met.
- Confirm the project remains zero-cost and uses no paid APIs, services, or usage-billed dependencies.

## Testing policy

The user does not have a laptop. Do not ask them to run local commands or test incomplete builds. Use GitHub-hosted CI and Vercel/cloud workflows for development verification. Physical Android testing happens only after a production deployment is READY and its public URL has been checked.

Never report a check as passed unless its workflow result is successful. Never claim a commit, merge, deployment, or device test that has not actually occurred.

## Exact next task

Open a pull request from `feature/settings-pause-screen` to `main`, inspect the GitHub Actions results, fix any failures, and merge only after the WebAssembly build and desktop/mobile browser tests pass. Then wait for the production deployment, run the live production suite, and notify the user when the Android test build is ready.
