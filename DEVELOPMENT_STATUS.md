# TAGBOUND Development Status

Last updated: 2026-11-23

## Project state

**Active development — not release-ready.**

Repository: https://github.com/villanuevayanmar/tagbound  
Production: https://tagbound.vercel.app  
Current main commit: `ea8e576d32fdedd6f80172150c65569770fa9ee3`  
Current production deployment: `dpl_EiD7wy3TYXzWUD71ZhNV5fcGA7QD`  
Current production commit: `a15a767daaa76e7abe7d053d76c62c4d4522f7b0`

The production URL is still serving the older deployment. Do not send the user to test the new release until Vercel reports a READY deployment for latest main and the production alias points to it.

## Recently completed and merged

### Timed hold-beacon objectives — PR #6

Merged in commit `805e96887ac4f61e17b1c6dd161c6fe026ab9026`.

- Mixed reach and timed hold objectives across the five cities.
- Hold stages require 2.5–4.0 seconds inside the beacon radius while pursuers remain active.
- Leaving the radius resets the capture timer; death, quest changes and city rebuilds also reset it.
- The objective HUD identifies “HOLD THE BEACON” and shows live capture time; the 3D marker changes color for hold stages.
- C++ regression tests cover stage pattern, duration, accumulation, leave-radius, quest/city and death resets.
- Hosted Chromium restores a saved hold-beacon checkpoint and verifies objective mode, zero initial capture time and duration.

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

### Class-specific BOW control — PR #5

- BOW touch action is hidden for Vanguard and Warden and shown only for Skirmisher.
- The fixed loadouts remain Vanguard → Sword, Skirmisher → Bow, Warden → Shield.
- Hosted desktop browser tests start each class in an isolated session and verify the saved weapon enum.
- Hosted mobile checks cycle through all three classes, verify visible action counts/BOW visibility, and verify the saved Skirmisher weapon is Bow.

### Original ambient score — PR #4

- Added an original ambient note sequence synthesized locally using Web Audio oscillators and filters.
- No downloaded music assets, external audio API or paid dependency.
- Score remains stopped on Home, starts after Start, pauses while Settings is open, resumes with the run, follows the shared sound toggle and stops on the victory ending.
- Added desktop/mobile tests for generated note count, pause, no new notes while paused, resumed notes, and saved-victory silence.

### Death-frame attack prevention — PR #7

Merged in commit `ea8e576d32fdedd6f80172150c65569770fa9ee3`.

- Combat resolution captures whether the player is alive at the start of the frame.
- If pursuit damage has already reduced HP to zero, the player cannot begin a new melee hit or spawn a new bow projectile later in that frame.
- Already-fired projectiles remain in flight and resolve normally.
- C++ regression tests cover a zero-HP Vanguard melee attempt and a zero-HP Skirmisher bow attempt; both respawn without creating the new attack.
- PR hosted Emscripten build, C++ regressions, PWA validation and desktop/mobile Chromium all passed in run `45078544251`.

## Verified cloud testing

- Settings PR checks passed in run `45073264491`.
- HUD/render PR checks passed in run `45074803692`.
- Pause input-reset PR checks passed in run `45075281567`.
- Ambient score PR checks passed in run `45076329628`.
- Class-specific BOW/loadout PR checks passed in run `45077008462`.
- Main after ambient-score merge passed WebAssembly compilation, C++ gameplay regressions, generated PWA validation, desktop/mobile Chromium, and publish-dist in run `45076558880`.
- Main after class-specific BOW merge passed in run `45077290312`: Emscripten build, C++ gameplay regression tests, static PWA validation, desktop/mobile Chromium, and publish-dist.
- PR #6 held-beacon checks passed in pull-request workflow `45077905618`.
- Main after PR #6 passed Emscripten build, C++ regressions, PWA validation, desktop/mobile Chromium and publish-dist in run `45078141314`.
- PR #7 death-frame fix passed Emscripten build, C++ regressions, PWA validation and desktop/mobile Chromium in run `45078544251`.
- Main-branch CI and generated `dist/` sync after PR #7 are pending; record their terminal result and output SHA once available.
- Latest main commit `e629ccbecbafed645e593b7040f3ea01f7667dd5` is the tested generated `dist/` sync.
- Hosted browser tests are not physical Android playtesting.

## Current work — fix/no-attack-after-death

Implemented on a feature branch; awaiting hosted tests and PR review:

- Prevent a player whose HP has already reached zero during pursuit from starting a melee hit or spawning a new bow projectile in the same combat frame.
- Preserve already-fired projectiles, which remain valid in-flight attacks.
- Add C++ regression tests for both a zero-HP melee attacker and a zero-HP Skirmisher bow user; each test verifies no attack/projectile is created and the correct class health is restored by respawn.

## Current state

The class-specific BOW feature is merged in PR #5; no weapon swapping or secondary weapon slot was introduced. Latest tested generated bundle from the prior main workflow was `e629ccbecbafed645e593b7040f3ea01f7667dd5`; main's documentation-only commit after that was `5479bd3613937f7fc76af346d84fe76f25f846f9`.

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

1. Finish hosted tests for `feat/varied-beacon-objectives`.
2. Fix any Emscripten, C++ regression or Chromium failures, then merge only after the checks pass.
3. Check main CI and ensure the generated `dist/` bundle is synchronized.
4. Continue reviewing checkpoint/respawn and boss encounters while monitoring Vercel's free deployment quota. Android testing remains pending until the latest production build is live.
