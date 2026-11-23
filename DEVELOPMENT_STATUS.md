# TAGBOUND Development Status

Last updated: 2026-11-23

## Project state

**Playable production build available — continued development in progress.**

Repository: https://github.com/villanuevayanmar/tagbound  
Production: https://tagbound.vercel.app  
Current main commit: `a18e39e9eca221a15d75c4070de9d06ef94abc3e`  
Latest tested runtime bundle commit: `9115f13b4021f9052a95eea66ceaa1000f134978`  
Production deployment: `dpl_GELuDKX6B4NragFQ6DSWjSNwFHAJ`  
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

### Stamina HUD overlap fix — PR #11

- Moved the stamina bar so it no longer covers the final digit of the stamina value.
- Added desktop and landscape-mobile screenshot-pixel checks for the digits and overlap area.
- Main CI and generated WebAssembly bundle synchronization completed.

### Live production smoke rerun — PR #12

- Added an explicit production base-URL diagnostic to the live-smoke workflow.
- Re-ran production smoke against the READY deployment serving runtime bundle `9115f13`.
- Latest main CI run #247 and live production smoke #9 both passed.

### Icon-only gameplay and pause controls — PR #13 (in progress)

- Replaced visible touch action labels with game-themed SVG icons.
- Replaced the in-game pause control and pause overlay actions with icons.
- Kept accessible labels for assistive technology without visible button text or hover titles.
- Removed the word-based touch-help strip while retaining necessary story, objective, and status text.
- Synchronized `web/` with the committed `dist/` files and bumped the service-worker cache to `tagbound-v26`.
- Added browser assertions in the normal CI and live-smoke workflows.
- Added `CONTINUATION_PROMPT.md` for recovery across interrupted conversations.
- PR #13 CI run #254 (`45121695420`) failed in the browser-smoke job after the WebAssembly build passed. The actual failure was the existing class-specific Bow visibility assertion: `.action` had `display:grid`, which overrode the browser's default hidden-attribute display behavior, so the Bow button remained visible for Vanguard. This is a real CSS regression introduced by the icon-button layout, not a gameplay loadout failure.
- Fixed the underlying cause by adding `.action[hidden]{display:none}` to `web/styles.css` and synchronizing the exact same CSS into `dist/styles.css` in commits `65909d7bea9c2820bb89665ad63d806e9dbd6c14` and `0206ec4534b856e9955967ec204f5e60211e974b`. This preserves the existing JavaScript class-visibility contract and does not weaken the test.
- The CSS fix has been committed to `feat/icon-only-game-controls`. A new run for the latest branch head must be inspected; no passing result is claimed yet. PR #13 remains open, unmerged, and not deployed.

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
- Latest main CI run #247: `45107305759` passed.
- Latest live production smoke #9: `45107305802` passed against the READY production alias and runtime bundle `9115f13`.
- PR #13 run #254 (`45121695420`) on commit `313c9f97aba3e7144c0f53fbe4e15ff7e6290474`: WebAssembly/C++ build job passed; browser-smoke job failed at the class-specific Bow visibility assertion; publishing the generated `dist/` artifact was skipped. Log evidence: `CI class BOW control visibility mismatch` for Vanguard (`klass:0`, expected `bowVisible:false`, observed `true`). The root cause was CSS `[hidden]` handling overridden by `.action{display:grid}`. The correction `.action[hidden]{display:none}` is committed to both `web/styles.css` and `dist/styles.css`. The latest branch head now needs a fresh hosted CI run; do not merge or deploy until the latest exact-head run passes.
- Live smoke confirmed WebAssembly/WebGL startup, gameplay, class selection, save restoration, Settings pause/resume, touch/look input reset, ambient music pause/resume, HUD visibility, install placement, service-worker control/cache, offline reload, mobile controls and portrait rotation guidance.
- These hosted checks do not replace physical Android playtesting.

## Deployment

<badge>READY</badge>

- Production deployment ID: `dpl_GELuDKX6B4NragFQ6DSWjSNwFHAJ`
- Deployed runtime bundle commit: `9115f13b4021f9052a95eea66ceaa1000f134978`
- Production alias: `tagbound.vercel.app`
- Vercel deployment metadata and alias lookup both confirmed the production alias points to this READY deployment.
- Current main SHA: `a18e39e9eca221a15d75c4070de9d06ef94abc3e`. The READY production deployment serves runtime bundle `9115f13b4021f9052a95eea66ceaa1000f134978`.
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

1. Resume by checking PR #13's current head and the newest Actions run for that exact SHA. Run #254 (`45121695420`) failed because the icon CSS overrode the `hidden` attribute on the Bow action. The fix `.action[hidden]{display:none}` is committed to `web/styles.css` and `dist/styles.css`; verify the latest branch contents and run.
2. Merge PR #13 only after the hosted WebAssembly, C++ regression, browser, and PWA checks pass.
3. Verify Vercel automatically deploys the updated committed `dist/` files, then run the live production smoke workflow against that exact READY deployment.
4. Update this document and `CONTINUATION_PROMPT.md` with exact merge, runtime bundle, CI, live-smoke, and deployment identifiers.
5. Have the user perform a physical Android playtest on the updated build; this has not yet been recorded.
6. Continue gameplay balancing, boss/AI/pathfinding review, objective variety, visual/animation work, audio tuning, performance, and release verification. Do not stop after the icon-controls task.


## Follow-up fix — default Bow control visibility

- CI run #257 (`45123933188`) for commit `ab279200a3f5dbd70fe07ee06371763404f26d30` passed the build job but failed browser-smoke at the class-specific Bow visibility assertion: for Vanguard, the element's `hidden` property was false after selecting class 0.
- The prior CSS correction only affects rendering when the `hidden` attribute is present; it cannot set the DOM property itself. The Bow button markup had no initial `hidden` attribute, leaving a brief/initial visibility risk before JavaScript synchronization.
- Added the native `hidden` attribute directly to the Bow button in both `web/index.html` and `dist/index.html`. Existing `syncClassControls()` removes it only when Skirmisher is selected and restores it for Vanguard/Warden. The CSS `[hidden]` rule remains to preserve layout behavior.
- Commits: `c763d3a0058001b77053a2d9caff41b45ea4b86c` (`web/index.html`) and `34966f568602b272b5878ade4ec5ce0699ac4f4a` (`dist/index.html`).
- This correction is committed but not yet CI-verified. PR #13 remains open and not deployed. Next: inspect the newest exact-head CI run, verify the Bow property is false for Vanguard/Warden and true only for Skirmisher, then proceed through all remaining tests.


## Latest continuation checkpoint — 2026-11-23 (PR #13 PWA initialization)

- PR #13 remains open and unmerged on `feat/icon-only-game-controls`. The production deployment remains the previously verified READY deployment for runtime commit `9115f13b4021f9052a95eea66ceaa1000f134978`; no merge or production deployment was started.
- PR CI run #261 (ID `45130001937`) tested head `8b47a3558393577c2fea6310d6c5de4a424d878f`. The WebAssembly build and C++ gameplay regression suite passed. Browser smoke timed out while waiting for the per-class test page to initialize.
- Exact source inspection found a real runtime bug: `web/pwa.js` and `dist/pwa.js` registered button listeners with undefined `soundButtons.forEach(...)`, despite only declaring `soundToggle` and `settingsSoundToggle`. This ReferenceError aborted the remainder of the PWA IIFE, preventing the class-selection, Start, touch, and pause event handlers from attaching.
- Fixed both browser scripts to attach the sound listener to `[soundToggle,settingsSoundToggle].filter(Boolean)`. This restores shared sound toggling and allows the rest of the script initialization to execute.
- Since cached assets changed after the previous v26 checkpoint, both service-worker files now use `tagbound-v27`; the HTML application-version metadata is also synchronized to v27. Normal CI and live-smoke expectations have been updated to v27.
- These changes are committed on the PR branch but are not yet verified by a new hosted run. Do not merge or deploy until the new exact-head CI checks pass and the user explicitly approves any production-triggering merge/deploy.


## Latest CI verification — PR #13 (2026-11-23)

- Run #267 (ID `45130586984`) tested the exact source head `ea2b29b988c8b515012eb974163bb10cd1bbbc6c` and completed successfully.
- The WebAssembly compile, C++ gameplay regression suite, static PWA assembly/validation, and Chromium browser-smoke workflow all passed. The browser tests now complete after the PWA initialization fix; they cover class weapon/Bow visibility, saved progress, pause/resume, sound, service-worker cache v27, offline reload, mobile landscape controls and portrait guidance.
- PR #13 remains open and unmerged. The production deployment remains unchanged; no merge or production deploy was initiated. Physical Android testing remains unverified.
