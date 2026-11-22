# TAGBOUND Development Status

Last updated: 2026-11-23

## Project state

**Active development — not release-ready.**

Repository: https://github.com/villanuevayanmar/tagbound  
Production: https://tagbound.vercel.app  
Current main commit: `15da20d50789098d94d83a4dea5a5343e8284cef`  
Current production deployment: `dpl_EiD7wy3TYXzWUD71ZhNV5fcGA7QD`  
Current production commit: `a15a767daaa76e7abe7d053d76c62c4d4522f7b0`

The production URL is still serving the older deployment. Do not send the user to test the new release until Vercel reports a READY deployment for latest main and the production alias points to it.

## Recently completed and merged

### Settings and pause system — PR #1

- Added an in-game Settings overlay with Resume.
- Pauses the actual C++ simulation while Settings is open.
- Home and Settings share the saved sound preference.
- PWA installation appears only on Home and Settings, never during active gameplay.
- Service-worker cache version: `tagbound-v25`.

### Gameplay HUD rendering and Settings layout — PR #2

- Added the missing Raylib `BeginDrawing()` / `EndDrawing()` frame boundaries.
- Confirmed health, stamina, level, danger, objective and city/quest HUD render in hosted Chromium screenshots.
- Moved Settings into the top HUD gap; narrow landscape screens use a compact gear control.
- Added screenshot-pixel validation and rectangle checks to prevent HUD overlap.

### Pause input reset — PR #3

- Opening Settings clears joystick movement, touch action bits, camera-look deltas, active-button styling and tracked pointer captures.
- Hosted desktop/mobile checks verify all touch/look state is cleared.

### Original ambient score — PR #4

- Added an original ambient note sequence synthesized locally using Web Audio oscillators and filters.
- No downloaded music assets, external audio API or paid dependency.
- Score remains stopped on Home, starts after Start, pauses while Settings is open, resumes with the run, follows the shared sound toggle and stops on the victory ending.
- Added desktop/mobile tests for generated note count, pause, no new notes while paused, resumed notes, and saved-victory silence.

## Verified cloud testing

- Settings PR checks passed in run `45073264491`.
- HUD/render PR checks passed in run `45074803692`.
- Pause input-reset PR checks passed in run `45075281567`.
- Ambient score PR checks passed in run `45076329628`.
- Main after ambient-score merge passed WebAssembly compilation, C++ gameplay regressions, generated PWA validation, desktop/mobile Chromium, and publish-dist in run `45076558880`.
- Latest main commit `15da20d` is the generated build-output sync from that successful run.
- Hosted browser tests are not physical Android playtesting.

## Current work — fix/class-specific-bow-control

Implemented on a feature branch; awaiting CI and PR review:

- The BOW touch action currently appears for every class even though only Skirmisher has a bow.
- Hide BOW for Vanguard and Warden; show it only when Skirmisher is selected.
- Verify all three class choices persist the correct fixed weapon in the saved player state: Vanguard → Sword, Skirmisher → Bow, Warden → Shield.
- Extend desktop and mobile browser tests to switch through all classes and assert the corresponding touch-action visibility and saved weapon mapping.
- Keep the one-weapon-per-class rule: no weapon swapping or secondary weapon slot.

## Deployment blocker — Vercel

Production still serves commit `a15a767`, not the latest main.

- Vercel manual deployment API returned HTTP 402, resource `api-deployments-free-per-day`; the free daily deployment API allowance had been exhausted.
- Vercel Git checks report `build-rate-limit`.
- A small temporary Vercel Sandbox was used only to check for an existing CLI login. No CLI/token was configured; it was stopped without deploying.
- No paid plan, payment method, quota override or bypass was enabled.
- GitHub having a new source/dist commit does not mean the public production URL is updated.

## Remaining development work

### Gameplay and campaign

- Complete full five-city playthroughs with all three classes.
- Tune city difficulty, reinforcement pacing, boss health/damage, stamina costs and encounter duration from observed play sessions.
- Review boss telegraphs, hitboxes and dodge/block counterplay.
- Expand quest variety beyond beacon traversal and boss gates.
- Validate city transitions, boss defeat progression, ending/restart and save restoration across every checkpoint.
- Review enemy pathfinding in dense rooftop geometry and reinforcement spawn edge cases.

### Visuals and animation

- Improve procedural player/enemy models, equipment silhouettes and class readability.
- Improve animation blending and movement/combat poses.
- Further distinguish city environments and rooftop routes.
- Review camera behavior, HUD legibility, visual effects and performance on mid-range Android hardware.

### Audio

- Tune ambient score melody/spacing, loudness and balance against effects.
- Test Web Audio unlock/resume behavior on real Android Chrome after deployment.

### Release readiness

- Continue merging tested features without deploying while Vercel's free build/deployment quota is blocked.
- When the free deployment allowance permits, obtain a READY deployment for latest main without upgrading or paying.
- Confirm `tagbound.vercel.app` points to that exact deployment.
- Run manual live-production Chromium only after deployment READY.
- Only then ask the user to test on Android; record device bugs, fix through GitHub, redeploy and repeat.

## Testing and development constraints

- The user does not have a laptop. Never require local commands or ask them to test incomplete builds.
- Use GitHub-hosted CI and authorized cloud workflows for development verification.
- Actual Android playtesting only begins after a READY production deployment and verified current alias.
- Keep cost at ₱0/$0. No paid API, paid subscription, paid hosting upgrade, billing change or usage-billed bypass without explicit approval.
- Never claim a test, commit, merge, deployment or device test that has not actually happened.

## Exact next task

1. Finish `fix/class-specific-bow-control`.
2. Add browser checks for the BOW button visibility across all three classes and fixed saved weapon assignment.
3. Require Emscripten build, C++ gameplay regression and desktop/mobile Chromium tests to pass.
4. Merge only after hosted CI is green.
5. Continue with campaign playthrough validation and quest variety, while monitoring Vercel's free deployment quota. Android testing remains pending until the latest production build is live.
