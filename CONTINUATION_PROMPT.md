# TAGBOUND — Current Continuation Checkpoint

> **Authoritative checkpoint — 2026-11-25.** The owner's `TAGBOUND_Prompt_v2.docx` is the governing specification. This section supersedes conflicting historical notes below. Keep implementing the existing game; do not restart the architecture or return a plan instead of work.

## Verified repository and CI state

- Repository: https://github.com/villanuevayanmar/tagbound
- Development branch: `development/complete-game`
- Last exact-head source commit that passed full CI: `d3722be3dbcf1b008bfbbe7a49e7f3dba40ce088`
- GitHub Actions [run #475](https://github.com/villanuevayanmar/tagbound/actions/runs/45594604642), run ID `45594604642`, passed both `build` and `browser-smoke` on that exact source commit.
- The run passed Emscripten/WebAssembly build, C++ gameplay regression tests, PWA build-output checks, desktop and mobile-landscape Chromium, build identity checks, gameplay input/save, current class-skill/map UI checks, offline cache/reload, portrait guidance, and forced WebGL context-loss recovery.
- Desktop canvas: 12 samples, minimum non-black pixel ratio 0.9999. Mobile landscape: 12 samples, minimum ratio 1.0000. These short checks do **not** satisfy M1's three-minute stability gate.
- The `tagbound-visual-review` artifact from run #475 (SHA-256 `cccd374cf75ff6528eebf79321ac18931974ca9f6f351bbe3327510e3471ca4b`) contains 14 full-resolution screenshots: desktop/mobile intro and gameplay plus five-city desktop/mobile boss-gate views. All 14 were inspected. Current city art is still repetitive low-poly blockout geometry; boss-gate views are not complete boss-fight or city-completion evidence.
- The M0 CI defect was an incorrect comparison between the build source SHA and GitHub's synthetic PR integration SHA. The browser test now checks source identity against `TAGBOUND_SOURCE_SHA` and integration identity against `GITHUB_SHA`; the test was not weakened.
- The runtime diagnostics report includes source/integration build identity, WASM hash, save schema, frame rate/time, WebGL context counters, WebGL version, GPU renderer where exposed, memory values when available, canvas viewport, and recent JavaScript errors. The Copy action does not include save contents.
- PR #17 remains open, draft, targets `main`, and is unmerged. PRs #18 and #19 are closed without merging.
- **Documentation commits after the tested code head:** the current branch head is newer than `d3722be3` because `DEVELOPMENT_STATUS.md` and `README.md` were updated after run #475. Those documentation changes still require one exact-head full CI run; verify the current branch SHA and its run before calling the documentation head green.

## PR, main, and release state

- `main` remains `a18e39e9eca221a15d75c4070de9d06ef94abc3e`. Do not merge PR #17 or change `main` without explicit owner authorization.
- Production `https://tagbound.vercel.app` was not touched. Last observed production deployment: `dpl_GELuDKX6B4NragFQ6DSWjSNwFHAJ`, runtime bundle `9115f13b4021f9052a95eea66ceaa1000f134978`.
- No new Preview is available. The prior recorded Preview `https://tagbound-5v46rpzby-yanmar.vercel.app` was built from an older artifact and must not be presented as current.
- Vercel project remains on Hobby/free with deployment protection/SSO enabled. The read-only usage lookup did not return current quota data; free-tier Preview capacity is unverified. Do not deploy until it is verified. Do not change Vercel settings or disable SSO.
- Do not use real player saves in tests or commit them. No paid services, billing changes, upgrades, or quota workarounds.

## Approved campaign decisions — do not change without owner approval

- Campaign order: Port Town → Merchant Tier → Clockwork Foundry → Imperial Capital → High Spire.
- City layout is hybrid: interiors, streets, rooftops, vertical traversal, and defensive walls.
- Floor count is flexible by city and must follow actual geometry/gameplay.
- Stamina and Mana are separate.
- Each level grants one stat choice. Skill milestones grant one mutually exclusive choice: unlock a new skill or upgrade an owned skill; ranks are bounded.
- Each boss guarantees its city key. Randomized item quality depends on measurable combat performance.
- Keep C++17, raylib 5.5, Emscripten, WebAssembly, WebGL, JS/HTML/CSS, and the current PWA. Do not migrate to another engine.
- User works from Android through ChatGPT/cloud tools and has no laptop. Do not require local terminal, PowerShell, VS Code, local builds, or manual Git.
- Budget stays ₱0 / $0.

## Next milestone — M1: diagnose rendering instability

**The reported black/flickering-frame root cause is not known and is not fixed.**

Inspection of the current renderer found:
- One Emscripten frame driver in `src/main.cpp`, one balanced `BeginDrawing` / `EndDrawing` pair, and a balanced `BeginMode3D` / `EndMode3D` pass.
- No competing application `requestAnimationFrame` loop in the inspected web shell.
- No application textures, models, shaders, or render targets created/disposed every frame in the inspected renderer.
- The context-loss reload in `web/index.html` is recovery behavior. The forced context-loss test passing does not identify the natural flicker cause.
- Short Chromium canvas samples showed no fully black frames and no natural unexpected context loss. No three-minute run or sustained frame-time/memory baseline has been collected.

Continue M1 in this order:
1. Inspect full renderer/platform lifecycle and instrument actual WebGL errors, context losses/restores, frame timing, and memory without changing gameplay or masking errors.
2. Reproduce the issue or add a regression scenario that can fail for the condition.
3. Only then make the smallest root-cause correction and test it.
4. Add a scripted run of at least three minutes covering movement, combat, abilities, enemy encounters, a city transition, a boss fight, and a background/return cycle where automation supports it. Sampled gameplay frames must never be fully black/blank, with no uncaught errors, WebGL errors, or unexpected context loss.
5. Record the root cause and measured frame-time/memory baseline in `DEVELOPMENT_STATUS.md`.
6. Keep anything not physically verified on the owner's Android phone marked unverified.

## Remaining milestones — follow the owner's prompt in order

- M2: all five bosses contained by visible/collision arena boundaries through pursuit, navigation, knockback, stun, charge, enrage, recovery, and grounding; automated checks and one scripted fight per boss.
- M3: responsive camera-relative movement, facing, camera follow/collision, parkour, and multi-touch (two or more touches simultaneously); remove accidental page scroll/zoom/pull-to-refresh.
- M4: verify all three class skills, costs/cooldowns/readiness, Arrow Rain targeting/cancel/telegraph/waves/damage, stat/skill milestone choice rules, caps, save persistence, and visible build.
- M5: test legacy and schema v1/v2/v3 saves, malformed/partial fixtures, pending choices, exploration, city progression, idempotent migration, death/restart/ending.
- M6: verify the five discovery layers against real navigable geometry in every city; explore-only reveal, floor separation, landmarks/objectives, persistence, and inspected per-city screenshots.
- M7: guarantee keys, grade-bound randomized loot, coherent inventory/equipment/stat effects, persistence without loss/duplication, and end-to-end campaign unlock.
- M8: finish the five distinct fortified cities in campaign order with real floors/zones, supported elevated surfaces, collision that matches visible geometry, fair enemy archetypes/telegraphs/counterplay, and inspected city screenshots.

Do not mark a milestone complete from source presence alone. Pass its exit gate on the exact commit, update the docs, and continue to the next milestone. Stop only at the owner's explicit authorization gate, a verified Preview ready for phone testing, a genuine blocker after completing all unblocked work, or full production completion with explicit deployment authorization.

## Repository and architecture
- Repository: https://github.com/villanuevayanmar/tagbound
- Production: https://tagbound.vercel.app
- Stack: C++17, Raylib 5.5, Emscripten, WebAssembly/WebGL, JavaScript, HTML/CSS, PWA, CMake, GitHub Actions, Vercel.
- Keep the existing engine and project structure. The user develops from ChatGPT on Android and has no laptop; do not require local terminal, build, or Git steps.
- Cost must remain ₱0/$0. No paid APIs/services/upgrades, billing changes, purchases, or quota bypasses.
- Preserve user data, save progression, Git history, and working features. Never claim unverified tests, deployments, or physical-device results.

## Non-negotiable game requirements
- Exactly three classes with fixed loadouts: Vanguard → Sword, Skirmisher → Bow, Warden → Shield. No weapon switching; Bow action only for Skirmisher.
- Five campaign cities in order: Harbor, Gilded, Foundry, Imperial, Spire.
- Preserve each city's rooftop routes, objectives, enemies, bosses, checkpoints, boss progression, ending, restart, and saves.
- Preserve parkour, combat, health/stamina, pursuits, boss telegraphs, XP/skills, touch controls, real pause/resume, sound, and PWA offline behavior.
- Action controls and pause actions are icon-only, with accessible aria-labels but no visible button words or hover labels.
- PWA installation appears only on Home and pause/settings, never during active gameplay.
- The user has not yet physically tested TAGBOUND on Android.

## Critical Vercel deployment hold
The user reported a Vercel deployment limit on November 23, 2026, at about 3:35 PM Philippine time, requiring at least 24 hours before another deployment attempt.
- Do not deploy or manually trigger a deployment.
- Do not merge a PR if merge could trigger a Vercel deployment.
- Do not bypass the limit or change billing.
- Earliest possible retry: after November 24, 2026 at approximately 3:35 PM Philippine time. This is not a guaranteed reset.
- Even after that time, deployment requires the user's explicit authorization.
- Continue on GitHub development branches and use GitHub Actions only for builds/tests.
- Production was checked read-only and is READY at deployment `dpl_GELuDKX6B4NragFQ6DSWjSNwFHAJ`, URL `https://tagbound-3ly5acjad-yanmar.vercel.app`, alias `https://tagbound.vercel.app`, generated runtime commit `9115f13b4021f9052a95eea66ceaa1000f134978`.

## Current GitHub checkpoint (verify again before acting)
- Main at session start: `a18e39e9eca221a15d75c4070de9d06ef94abc3e`.
- PR #13: `https://github.com/villanuevayanmar/tagbound/pull/13`, open/unmerged, branch `feat/icon-only-game-controls`, head `6b7bd74fa832ffef50225307d3ed2db419222243`. Matching CI run #268 passed. It implements icon-only game and pause controls; its branch is separate from PR #14.
- PR #14: `https://github.com/villanuevayanmar/tagbound/pull/14`, open/draft/unmerged, branch `feat/campaign-imperial-before-spire`. Previous head `fe7cc9182deb` passed run #271.
- Review found a real regression risk in PR #14: it swaps the identities at campaign indices 3 and 4, while the existing local save had no schema version. Leaving numeric save values unchanged makes an old Spire save restore as Imperial, and vice versa.
- The current checkpoint adds migration from unversioned saves: index 3 → 4 and 4 → 3, then writes `schemaVersion: 1`; future loads must not remap again. Regression tests cover both directions and repeated loading.
- The new migration commit has just been pushed and its CI must be rechecked against the exact latest PR #14 head. Do not assume run #271 covers it.
- PR #13 and #14 both modify the CI workflow and may overlap. Do not merge either while deployment hold is active.

## Development workflow
1. Inspect repository tree, README, DEVELOPMENT_STATUS, this file, main/PR branches, open PRs, latest commits, latest exact-SHA Actions runs/logs, source, tests, PWA, CMake, workflows, and deployment state (read-only).
2. Compare docs to actual code; docs can be stale.
3. Find a concrete implementation gap or reproducible bug.
4. Make the smallest reliable fix on a development branch.
5. Run the relevant WebAssembly/C++ build, gameplay regression tests, browser/desktop/mobile/PWA checks in GitHub Actions.
6. Fix actual causes; never weaken or remove assertions to get green CI.
7. Commit logical changes and keep DEVELOPMENT_STATUS.md and this file accurate.
8. Move to the next unfinished requirement without asking for "continue" after every task.
9. Never merge or deploy during the Vercel hold, or later without explicit deployment authorization.

## Exact next action
Inspect the newest PR #14 commit and the matching Actions run generated by the legacy-save migration commit. Confirm the C++ build, gameplay regression suite, and browser smoke all pass; fix failures from logs. Then continue development on the branch. Keep PRs open and production unchanged.


## Verified CI checkpoint — legacy save migration
- PR #14 migration commit `a8d16889816f6681f2163646497f3fa236dd7a3c` passed run #272 (`45133035082`) exactly: WebAssembly/C++ build, gameplay regressions, PWA validation, and Chromium browser smoke.
- Legacy unversioned city indices are migrated once (3→4, 4→3) and persisted with `schemaVersion: 1`. Repeat reloads do not remap the save again.
- PR #13 remains open/unmerged; PR #14 remains draft/open/unmerged. Production remains at the previously verified READY deployment; no deployment has been triggered.

## New feature checkpoint — survival-pursuit objective
- Implementation commit: `e498d73408385aaf71c5055c57c25bae676275a9`.
- Adds survival encounters in Gilded quest 2 (18s), Foundry quest 2 (20s), and Spire quest 1 (22s). Timer advances independent of beacon proximity; player defeat/quest change/city rebuild reset it. It has no beacon marker or objective compass; HUD displays `SURVIVE THE PURSUIT` and remaining time/tagger count. Hold-beacon logic remains unchanged.
- C++ unit tests were added in this implementation commit. This exact commit needs CI verification.
- A subsequent commit adds browser assertions for all three objective restore states and updates README/status/continuation documentation. Recheck the final exact PR #14 head and matching workflow run before claiming verification.
- Continue investigating gameplay balance/edge cases after CI passes. Do not merge or deploy while the user-reported Vercel limit is active. Even after the earliest 24-hour window, deployment requires explicit user authorization.


## Latest verified feature CI — survival-pursuit objectives
- Exact PR #14 head `095fb05665f6b100fb3c9f5db0b5d18d05ca6d82` passed run #274 (`45133786317`): WebAssembly/C++ build, gameplay regression tests, PWA validation, Chromium browser smoke, including saved-state checks for survival objectives in Gilded, Foundry, and Spire.
- Legacy save migration was independently verified by run #272 (`45133035082`) on commit `a8d1688...`.
- PR #13/#14 remain open and unmerged. The Vercel deployment limit remains active; production is unchanged and deployment still requires explicit authorization.

## Latest implementation — rooftop pursuit routing test
- Added a C++ regression that places a wall between a pursuer and player, simulates pursuit, and asserts the enemy navigates around the obstacle without embedding into it.
- Immediate next action: inspect the newest exact-head PR #14 CI run. If this navigation assertion fails, inspect the C++ gameplay regression output and fix the pathfinding root cause; do not weaken the assertion. If it passes, use the next remaining verified gameplay gap.


## Latest checkpoint override — 2026-11-23

Use this section as the authoritative state; earlier next-action text in this file predates the latest commits.

- PR #13 remains open/unmerged: `feat/icon-only-game-controls`, head `6b7bd74fa832ffef50225307d3ed2db419222243`, exact matching run #268 passed.
- PR #14 remains open, draft, and unmerged: `feat/campaign-imperial-before-spire`.
- PR #14 commit `c1837d212db35c224389534268f61fc3b21a254f` added a rooftop pursuit pathing regression. Its exact run #275 (`45134277135`) failed at `tests/gameplay.cpp:712` after the WebAssembly compile passed.
- Failure root cause: pursuit LOS did not account for the pursuer's physical radius, so direct pursuit resumed while the body still collided with the obstacle.
- Fix: `15938b998823fe9bc7a9e4ca8ae57954e6ac2a9c` updates `src/systems.cpp` so direct pursuit line-of-sight uses body clearance (0.42 normal pursuer / 0.62 boss), while corner-waypoint LOS keeps its proven narrow ray margin and radius-offset corners. Attack LOS keeps its prior default.
- Documentation commit: `0204f9cfc206d98a94038527c6f872d4c3d59729`.
- Recheck the latest branch HEAD and its exact SHA-matched CI run; it may include this documentation commit. Inspect all job steps and logs. Do not claim the fix passes until the regression job actually passes.
- The existing navigation test must remain intact and pass both distance and no-wall-overlap assertions.

### Deployment hold

Production remains READY at deployment `dpl_GELuDKX6B4NragFQ6DSWjSNwFHAJ`, runtime commit `9115f13b4021f9052a95eea66ceaa1000f134978`, alias `https://tagbound.vercel.app`. User-reported Vercel limit hold remains active; no merges, manual deploys, quota workarounds, billing changes, or paid services. Even after the 24-hour window, deployment requires explicit user authorization.

### Next development task

After the exact-head CI result is known, continue with a test-backed review of parkour ledge/vault/wall-run collision transitions, then combat/boss counterplay. Preserve existing behavior and user saves. Keep `DEVELOPMENT_STATUS.md` and this prompt current after meaningful changes. Physical Android testing is still pending and must not be represented as completed.


- Follow-up run #277 (`45137349773`) found a second regression at `tests/gameplay.cpp:558`: body-radius expansion on corner-waypoint LOS rejected a valid open-side route. Commit `1c48ec1806a2d8593aa61b184471eface72ae5aa` keeps body-radius clearance for direct pursuit but restores the established waypoint ray tests. Verify the latest exact head in CI before claiming the routing fix passes.


## Authoritative checkpoint override — 2026-11-23

### Latest verified branch and tests
- PR #13: open, unmerged, branch `feat/icon-only-game-controls`, head `6b7bd74fa832ffef50225307d3ed2db419222243`; exact run #268 passed.
- PR #14: open, draft, unmerged, branch `feat/campaign-imperial-before-spire`, latest gameplay/test commit `37bffda9a20139393b9074d4e7e43fa002a62c3f`.
- Exact run #287 (`45141318209`) passed for `37bffda...`: Emscripten/WebAssembly build, C++ regression tests, PWA artifact validation, desktop/mobile Chromium, service-worker cache and offline reload. The pull-request `publish-dist` job was skipped.
- Run #286 (`45140870041`) passed the PLAY AGAIN restart browser test. It verifies progression resets on a reload while Skirmisher/Bow selection and the stored sound preference remain intact.
- Run #285 (`45139427079`) passed the Skirmisher base-speed regression. Run #284 (`45138923137`) passed wall-run stamina and wall-jump regressions. Run #283 (`45138485082`) passed the body-aware and sequential rooftop pursuit routing tests.
- Sprint stamina behavior is now explicit: holding Sprint without enough stamina does not regenerate it; release Sprint to regenerate; wall-running drains stamina. README touch-control guidance was updated to match.

### Deployment hold and Vercel integration
- Production remains READY at `dpl_GELuDKX6B4NragFQ6DSWjSNwFHAJ`, alias `https://tagbound.vercel.app`, runtime commit `9115f13b4021f9052a95eea66ceaa1000f134978`.
- No PR merge or production deployment was performed. User-reported deployment limit hold remains active; deployment still requires explicit user authorization after the stated 24-hour window.
- `vercel.json` contains the existing ignored-build guard `git diff --quiet HEAD^ HEAD -- dist/`. Read-only inspection of PR #14 commit `a8d1688...` showed Vercel canceled the preview attempt because this command returned exit code 0. Keep all PR #14 changes away from `dist/` during the hold and do not bypass or weaken the guard.
- Vercel history showed READY preview builds from older PR #13 commits created at 13:59 and 15:10 PHT on Nov 23, before the user's 15:35 PHT hold. A PR #14 source-only attempt at 15:42 PHT was canceled by the ignored-build step. Production stayed on its original READY deployment.
- PR #13 bumps the service-worker cache to `tagbound-v27`; PR #14's current workflow still expects `tagbound-v25`. Their shared `.github/workflows/ci.yml` and docs require careful integration: preserve PR #13's icon-only assertions and PR #14's survival-objective, save-migration, pursuit, and restart coverage, and reconcile the service-worker version. Neither PR may be merged while the hold is active.

### Remaining scope and next action
- Physical Android testing is still pending; do not claim hosted mobile emulation is physical-device verification.
- Continue with a code/test review of boss telegraph shapes and strike boundaries, then other demonstrable gameplay/balance gaps. Inspect actual implementation first and add boundary tests; do not invent mechanics.
- Keep PR #14 source/test changes off `dist/`, preserve saves and history, run exact-head Actions, and update these documents after each meaningful checkpoint.


## Latest verified combat checkpoint — 2026-11-23

- Latest PR #14 code/test SHA: `81465b29c2943c48c45516054f8ad77696b599a9`.
- Exact run #290 (`45142530269`) passed: WebAssembly build, C++ gameplay tests, PWA validation, desktop/mobile Chromium, service-worker and offline reload. The PR-only publish job was skipped.
- Boss hitbox/warning mismatch fixed: Gilded Charger and Spire Sentinel had a 0.9-unit damage half-lane but their warning cylinders were narrower/tapered. Shared `BossStrikeHalfWidth` now controls both collision and rendering. Harbor's cleave wedge also uses the helper for its boundary.
- Boundary regressions verify 0.89 units is hit and 0.91 units is safe for both Charger and Sentinel; existing boss damage, dodge, radial range, and enrage tests remain.
- Run #289 (`0fe17ffb9796df41e5dd5f9d126aeb074fcc3fcb`) passed the geometry correction; run #290 verified the follow-up cleanup.

## Authoritative deployment / branch state
- PR #13 is open/unmerged at `6b7bd74fa832ffef50225307d3ed2db419222243` (run #268 passed).
- PR #14 is open/draft/unmerged at `81465b29c2943c48c45516054f8ad77696b599a9`.
- Main: `a18e39e9eca221a15d75c4070de9d06ef94abc3e`.
- Production: READY deployment `dpl_GELuDKX6B4NragFQ6DSWjSNwFHAJ`, alias `https://tagbound.vercel.app`, runtime `9115f13b4021f9052a95eea66ceaa1000f134978`.
- Vercel deployment hold remains active. Never merge, deploy, change billing, or bypass quotas. Keep PR #14 changes away from `dist/` and preserve the existing ignored-build guard. No physical Android testing has been completed.
- PR integration still needs reconciliation: PR #13 uses service-worker cache `tagbound-v27`; PR #14's current CI asserts `tagbound-v25`. Preserve icon-only checks from #13 and survival, migration, pursuit, restart, and boss geometry tests from #14 during later integration.

### Next task
Review actual rooftop route reachability for Harbor, Gilded, Foundry, Imperial, and Spire. Use the movement/jump physics in `src/systems.cpp` and route coordinates from `src/game.cpp`; add a deterministic simulation test for consecutive objective platforms and the final boss gate. Only adjust route positions if the test proves a gap is unreachable. Never modify `dist/`, merge, or deploy during the hold.


## Verified five-city route checkpoint — 2026-11-23

- PR #14 branch head `48569424b036950fe4b477f0307f38e25eae9ca7`.
- Exact Actions run #296 (`45144472169`) passed on this SHA: WebAssembly compile, C++ gameplay tests, PWA validation, Chromium browser smoke, offline reload. `publish-dist` was intentionally skipped for the pull request.
- Added 20 deterministic rooftop transfer simulations: each city's spawn roof to objective 1, then objective 1→2, 2→3, and objective 3→boss gate.
- Initial route simulation failure was an input-sequencing omission in the test: when the jump caught a ledge, the test did not press jump again to climb. The corrected simulation sends the documented ledge-climb input. All 20 transitions now pass; no gameplay route/movement tuning was needed.
- Do not characterize the initial Imperial failure as an inaccessible route. The debug trace showed the player entered a valid ledge-hang state, and the missing second jump was the cause.
- PR #13 and PR #14 remain open/unmerged; production remains on the last READY main deployment. Vercel deployment hold remains active. Keep source/test changes away from `dist/`; no merge or deploy without authorization.
- Integration still must reconcile PR #13's `tagbound-v27` service-worker cache with PR #14's `tagbound-v25` CI assertion.

### Next exact task
Extend the existing five-city transfer simulation from the third objective platform into each boss arena, based on actual boss placement/spawn geometry and normal movement. Then inspect checkpoint/save restoration at boss unlock, death, and restart. Keep all save migration and preference-preservation checks; do not change gameplay mechanics without a reproducible failure.


## Latest verified checkpoint — boss-gate entry in all five cities (2026-11-23)

- PR #14 latest source/test commit: `8f6aa99e53b69f8da116ab6ac38e4954be347721`.
- Exact run #298 (`45148968483`) passed all jobs: WebAssembly build, C++ gameplay suite, PWA validation, desktop/mobile Chromium smoke, service-worker and offline reload. The pull-request `publish-dist` job was skipped.
- The 20-transition route simulation now creates each city's actual boss before the final approach. It verifies the boss is unlocked, exactly one live boss exists, its `variant` matches the city, and its spawn position matches the gate target. The approach then lands on the boss gate using normal Skirmisher jump/sprint and ledge-climb controls.
- No runtime mechanics or route geometry changed in this test-only extension. All five routes and boss gate approaches pass.
- Production is still READY on main runtime `9115f13b4021f9052a95eea66ceaa1000f134978`; PR #13 and #14 remain open/unmerged. Deployment hold still applies. Do not merge, deploy, or modify `dist/`.

### Next exact task
Audit local save restoration at each city/quest checkpoint. Verify the saved campaign index, unlock progression, quest, level/XP, selected class and fixed weapon are preserved through reload; confirm boss-gate restoration rebuilds the correct city-specific boss and player checkpoint. Reuse the existing browser and C++ tests, adding only missing assertions. Retain legacy stage 3/4 one-time migration and restart preference preservation. Keep all edits on PR #14 and outside `dist/`.


## Latest verified save checkpoint — 2026-11-23

- PR #14 head: `39c128f65e0ffc722b44f850aee2f32ab34d615e`.
- Exact run #300 (`45149650762`) passed: WebAssembly/C++ build, gameplay regressions, PWA artifact validation, Chromium smoke, and offline reload. Pull-request publishing was skipped.
- Legacy stage 3/4 migration browser checks now verify city/unlocked, quest, level 2, XP 15, Skirmisher/Bow, finished false and schema v1 after both the first migration reload and the second idempotent reload.
- Boss-gate reload now verifies the saved city/unlocked, quest 3, level/XP, class/weapon, finished state and schema version alongside one active reconstructed boss.
- Runtime save schema and game behavior were not changed; only CI assertions were strengthened.
- PR #13 and #14 remain open/unmerged. Production remains READY on main runtime `9115f13b4021f9052a95eea66ceaa1000f134978`; Vercel deployment hold remains active. Keep `dist/` untouched.

### Next task
Inspect existing city rendering before changing it: city-specific landmarks, tint/materials, lighting, ground grids and procedural draw calls. Identify concrete visual duplication or high per-frame cost. Prefer a small city-specific render correction or a measurable draw-call reduction over adding new models/dependencies. Validate through existing browser screenshots/CI; physical Android rendering and frame-time testing remain unverified. Do not merge or deploy during the hold.


## Latest verified render checkpoint — enemy model orientation (2026-11-23)

- Latest PR #14 code/test SHA: `a93a7e4327570630cbc4cc884ea2982eea90aa57`.
- Exact GitHub Actions run #302 (`45150660649`) passed: WebAssembly build, complete C++ gameplay regression tests, PWA validation, Chromium desktop/mobile browser smoke, service-worker cache and offline reload. Pull-request publishing was skipped.
- Enemy render models now orient toward the player's horizontal position. Bosses keep their locked `strikeDirection` during Windup/Strike, then resume tracking the player in other phases. A deterministic fallback handles coincident positions.
- Tests cover normal target-facing, normalized locked strike direction, recovery tracking and fallback. No gameplay mechanics, hitboxes, or damage changed.
- PR #13/#14 remain open/unmerged. Production remains READY on main runtime `9115f13b4021f9052a95eea66ceaa1000f134978`; deployment hold remains active. Do not merge/deploy or edit `dist/`.

### Next exact task
Review the renderer for proven redundant per-frame geometry/draw work and any concrete five-city visual inconsistencies. Inspect actual screenshots and source; make no speculative performance or quality changes. Use existing CI/browser smoke for verification, but state clearly that actual Android frame time has not been measured. Keep all source/test work on PR #14 and avoid the deployable bundle during the hold.


## Latest verified render checkpoint — floor/grid de-duplication (2026-11-23)

- PR #14 exact code/test SHA: `a3e7e66b8c16712171b5abaf808d7faf98f01efe`.
- Exact Actions run #304 (`45151623835`) passed: WebAssembly build, C++ gameplay regressions, PWA validation, Chromium desktop/mobile smoke, service-worker cache and offline reload. The PR publishing job was skipped.
- Renderer no longer draws the 120×120 ground collision slab as a cube/wireframe on every frame. It still exists in `GameState` for collision; its visual surface is provided by one ground plane at height 0.0. Ground grid was moved slightly above the plane so it is visible rather than occluded inside the slab.
- Inspected generated desktop and mobile screenshots from run #304: the ground grid is visible, and player/rooftop heights remain aligned.
- No gameplay collision or movement behavior changed. Removed two redundant world-floor drawing calls plus the slab geometry; no actual FPS/frame-time measurement has been performed on Android.

## Current repo/deployment constraints
- PR #13 remains open/unmerged at `6b7bd74fa832ffef50225307d3ed2db419222243`; PR #14 remains open/draft/unmerged at `a3e7e66b8c16712171b5abaf808d7faf98f01efe`.
- Production remains READY on main runtime `9115f13b4021f9052a95eea66ceaa1000f134978`. No PR merge, deployment, or `dist/` change was made.
- Vercel deployment hold remains active; deployment still requires explicit authorization later. Keep code edits away from `dist/`.
- Cache version is aligned at `tagbound-v27` across `web/index.html`, `web/sw.js`, and PR #14 CI. Keep PR #13 separate and unmerged.

### Latest verified checkpoint — 2026-11-23
- Latest verified gameplay/source code head: `a3e7e66b8c16712171b5abaf808d7faf98f01efe` (run #310 SUCCESS). Documentation-only follow-up head `616b0c5bc12be063fe4712bded81f9c2c2549662` passed run #311 SUCCESS.
- CI run #310: SUCCESS on that exact commit. C++ gameplay tests, Emscripten build, PWA validation, Chromium desktop/mobile browser smoke, pause/audio/touch reset, save restoration, and offline reload passed.
- Run #310 artifact `tagbound-visual-review` was retrieved and all ten boss windup screenshots were individually inspected at full resolution. All five boss warnings are visible on desktop and mobile; no render change was justified.
- Icon-only touch and pause controls are implemented with accessible labels; Bow is visible only for Skirmisher. The CI test checks the accessible icon-button state and saved sound preference.
- Cache version is aligned at `tagbound-v27`. No `dist/` changes, merges, or deployments occurred.
- Run #309 had an obsolete visible-text assertion for the Settings sound icon; this was corrected, and run #310 passed.
- Physical Android FPS/touch testing remains unverified.

### Next exact task
Continue auditing the agreed gameplay/PWA scope for concrete uncovered edge cases. Do not add speculative features. Keep the zero-cost constraint, preserve existing save/progression behavior, and do not merge or deploy without explicit authorization.


## Remaining-work clarification — 2026-11-23

- The README's current playable-scope statement and `DEVELOPMENT_STATUS.md` remaining-work section now reflect the implemented five-city campaign: reach, hold and survival objectives are all present; the next work is evidence-based balance/polish, not adding unrequested objective types.
- Production still serves main runtime `9115f13b4021f9052a95eea66ceaa1000f134978`; the current PR #14 code is not deployed. Do not ask for Android testing of this branch until the user authorizes a deployment after the reported Vercel hold.
- Physical Android frame-time, touch and audio behavior remain unverified. Hosted Chromium mobile tests do not establish Android performance.


## Latest verified checkpoint — 2026-11-23, browser lifecycle recovery

- Source/test head: `29fee596ac204be433ff18d25266e722b18bd249`; exact GitHub Actions run #315 (`45184017300`) passed.
- Run #314 (`45182902026`) passed input cleanup tests for window blur and document visibility changes.
- Run #315 added and passed audio lifecycle tests: ambient music pauses on browser blur and when the document is hidden; music resumes when focus/visibility returns only while active gameplay is visible. Settings and the victory ending remain paused.
- Existing desktop/mobile Chromium, C++ gameplay, WebAssembly, save restoration, service-worker cache, and offline reload checks passed. PR-only `publish-dist` remained skipped.
- README, development status, and this prompt now record the background lifecycle behavior.
- PR #13 and PR #14 remain open/unmerged; PR #14 remains draft. Main is still `a18e39e9eca221a15d75c4070de9d06ef94abc3e`. No `dist/` change, merge, deployment, or billing modification occurred.
- Physical Android performance/touch/audio checks remain unverified. Do not merge/deploy or attempt a quota workaround; deployment still needs explicit user authorization.

### Next exact task
Continue the remaining source/test audit for concrete issues. Prioritize gameplay defects with reproducible regressions over speculative additions or balance changes without playtest evidence. Keep all work outside `dist/` during the deployment hold.


## Latest active continuation — 2026-11-24

- Current development branch: fix/boss-arena-floor-bounds; draft PR #15 is temporarily based on main so CI can validate the accumulated source changes. Do not merge it.
- PR #15 contains the tested boss-arena containment work: arena geometry is taken from the actual gate platform; simulation movement, knockback and charge cannot take the boss body beyond its valid floor area; boss Y is kept on the floor; five-city deterministic regressions and a read-only browser containment check are included.
- Exact-head Actions run #320 (45269952163) passed on f97682834a3de0a98b539b2b75bac2426f218c4c, including C++ tests, WebAssembly build, PWA validation, Chromium browser/offline tests and desktop/mobile boss screenshots.
- A WebGL context-loss recovery path and a forced Chromium context-loss test have since been added. They are not yet verified: wait for an exact-head run on the current branch head and fix failures before claiming success.
- The black-flicker root cause remains unknown. No actual WebGL loss has been observed in available production/browser logs; context-loss recovery is a robustness measure, not proof that this is the cause.
- Production and Vercel billing remain untouched; there are no dist/ changes. PRs #13/#14/#15 remain open and unmerged. Physical Android testing remains pending.

### Next task
Run exact-head CI for the context-loss recovery, inspect logs/artifacts, then restore PR #15's base to feat/campaign-imperial-before-spire. Continue investigating flicker with evidence; follow with the camera/movement/grounding audit. Keep the ₱0/$0 constraint and do not merge/deploy without explicit authorization.


## Verified continuation checkpoint — 2026-11-24, TAGBOUND arena and graphics recovery

- Current feature branch: fix/boss-arena-floor-bounds. Draft PR #15 is temporarily based on main for exact-head CI; restore it to feat/campaign-imperial-before-spire after the documentation-validation run.
- Boss arenas now derive from actual gate-floor geometry. The simulated boss body stays within collision-radius-adjusted bounds and on the floor through pursuit, knockback and Gilded Charger charge movement. All five boss variants and boundary cases have C++ regression coverage.
- WebGL context-loss recovery is implemented in web/index.html. The lost event is cancelled, SaveNow flushes current local progress, and the page reloads after restoration or an 8-second fallback. A 30-second session guard prevents automatic reload loops.
- CI deliberately triggers WEBGL_lose_context, restores it, checks that the actual in-memory save overwrites a deliberately corrupted test fixture, then verifies reload and gameplay restart.
- Run #321 failed because Module.calledRun was not present in the generated Emscripten runtime, so the save-flush guard skipped SaveNow. That guard was removed. Run #322 (45273216472) passed on exact head 53fcaf598804963c1602c3acf852e7bcaf23482b: C++ gameplay tests, WebAssembly build, PWA validation, Chromium desktop/mobile/offline smoke and forced context-loss/save/reload/restart.
- The intermittent black-flicker root cause remains unconfirmed. Context-loss handling is a verified resilience feature, not proof the symptom is caused by context loss. No Android device testing has occurred.
- Production remains READY on the existing main deployment; no merge, dist/ change, release deployment, quota bypass or billing change has occurred. PRs #13/#14/#15 remain open/unmerged. Zero-cost constraints remain mandatory.

### Next exact task
Restore PR #15's base to PR #14 after the full current-head CI. Then continue the P1 camera follow/orientation fix: preserve the existing camera-relative control scheme, add a predictable camera-follow rule with manual-look override, reset camera state for a new run, and write tests for yaw, facing and movement. Follow with movement acceleration/deceleration, grounding and parkour edge cases. Continue investigating flicker with evidence rather than assuming the new recovery path is its root cause.


## Latest verified continuation — 2026-11-24, camera orientation

- Current camera branch: fix/camera-facing-follow, draft PR #16, temporarily targeting main for the repository's pull_request CI. After its final exact-head CI, retarget it to fix/boss-arena-floor-bounds (PR #15). PR #15 is draft and stacked on PR #14.
- Camera-relative movement remains the control model. When movement stops and the player is not aiming, camera yaw smoothly follows the character's last facing direction after the 1.2-second manual-look override. Touch/mouse look remains responsive and takes priority during the override.
- ResetCamera initializes yaw from the restored spawn/checkpoint facing, resets pitch and clears manual-look state. StartGame applies this after restoring the player position/facing.
- Regression tests cover reset, strafe facing, camera-relative controls, idle follow, manual-look override and follow resumption. Existing rooftop route tests now derive joystick input from the live camera basis rather than assuming a fixed world-aligned camera.
- Exact-head run #327 (45276625519) passed on 015c6013c5db740729f33d96789a7ed2eccb5c7f: C++ gameplay tests, WebAssembly build, PWA validation and Chromium desktop/mobile/offline smoke. Visual artifacts for all five boss arenas were reviewed at desktop resolution; mobile-landscape gameplay and Spire boss captures were also reviewed.
- PR #15's boss containment and WebGL context-loss save/reload recovery passed exact-head run #322 (45273216472), with docs head revalidated by run #323 (45274447641). Black-flicker cause remains unconfirmed; no Android test has occurred.
- Production is unchanged and READY on the older main runtime. No dist output, merge, deploy, Vercel billing change or quota workaround was made. All services remain zero-cost.

### Next task
After the documentation/control README update passes exact-head CI, stack PR #16 on PR #15 and check the clean stacked diff. Continue with movement acceleration/deceleration, air control, jump/landing and parkour grounding/collision; preserve existing routes and add deterministic regressions. Keep physical Android limitations and the still-unconfirmed black flicker cause explicit.


## Latest continuation checkpoint — 2026-11-24

Continue from the current repository; do not restart or create a replacement project.

### Live branch structure
- Main: `a18e39e9eca221a15d75c4070de9d06ef94abc3e`.
- PR #14 / `feat/campaign-imperial-before-spire`: head `814d2e86c0c7cb5afc7b794e0d26d054eed87222`, base `main`; exact-head run #317 (`45185250016`) passed.
- PR #15 / `fix/boss-arena-floor-bounds`: head `c92136f54d38df42aa239342dc803e784182ac73`, base PR #14; exact-head run #323 (`45274447641`) passed.
- PR #16 / `fix/camera-facing-follow`: head `9a83d69b2862b1329467952e4cafa020e25d75fd`, base PR #15; compare is 13 commits ahead, zero behind. All PRs remain open drafts, not merged.
- Do not flatten the stack, force-push, merge, edit `dist/` or deploy without explicit authorization.

### Latest implementation on PR #16
- Camera follow, manual-look override, camera reset and camera-relative traversal regressions remain in place.
- Added transient `Player::moveVelocity` for horizontal player locomotion, separate from physics impulses/vertical velocity.
- Ground acceleration/braking: 34 / 28 units/s². Air steering/braking: 16 / 3.5 units/s². Existing class top-speed targets are preserved.
- While dash is active, acceleration is 100 units/s² so the existing 2.7× burst can be reached during its 0.24-second window.
- Wall collision cancels the corresponding locomotion and impulse axis. City rebuild, ledge/climb transitions and death/checkpoint reset the transient movement velocity. It is intentionally not serialized.
- Added regressions for acceleration ramp, top speed, release braking, weaker air steering, dash burst and the class speed distinction.

### Exact test evidence
- GitHub Actions run #334 (`45329538797`) passed on code/test head `9a83d69b2862b1329467952e4cafa020e25d75fd`.
- Build: C++ gameplay regression suite, Emscripten/WebAssembly, PWA assembly/validation passed.
- Browser smoke: desktop and mobile-landscape Chromium, gameplay start/input, save migration/restoration, offline cache, all five boss-arena visual captures, and forced WebGL context-loss/save-flush/reload recovery passed.
- Visual artifact `tagbound-visual-review` was uploaded and inspected. PR-only `publish-dist` was skipped.
- Earlier run #332 failed only the class-speed assertion because its synthetic platform had a top surface at Y=0 while the player spawn surface is Y=0.3; test fixtures were corrected. The corrected exact head is run #334 and passed.

### Still unresolved
- Intermittent black flickering has no confirmed root cause. The source audit found one frame drawing pair and one 3D pass per frame, with no repeated per-frame model/texture allocations identified. WebGL context-loss recovery is resilience, not a proven flicker fix.
- Physical Android GPU/browser, touch-feel and sustained performance testing has not been performed. Chromium is not a substitute.
- Production alias still points to older READY runtime SHA `9115f13b4021f9052a95eea66ceaa1000f134978`; no deployment or merge was performed. Maintain ₱0/$0 and do not work around quotas.

### Next work order
1. Investigate black-frame flicker with a repeatable multi-frame canvas check in hosted Chromium. Check for an actually black framebuffer rather than treating a dark scene as failure; preserve current screenshot/visual assertions. Report this as browser evidence only, not Android validation.
2. Add focused deterministic tests for jump/landing, grounding and rooftop transitions; then review ledge/vault/climb collision and enemy navigation.
3. Once the movement/rendering foundation is stable, use the master specification to implement the remaining systems in focused increments: separate stamina and mana, a stat choice at each level, skill-vs-upgrade choices at milestones, fortified vertical city/floor progression, enemy archetypes, fog-of-war exploration, guaranteed boss keys and performance-banded equipment loot.
4. Keep `DEVELOPMENT_STATUS.md` and this file synchronized with exact PR heads, bases and GitHub Actions run IDs. Check exact tested SHA; a started run is not a passing run.



## Newest pending diagnostic — 2026-11-24

- Added repeatable WebGL canvas-frame sampling to `.github/workflows/ci.yml` in commit `e5da108ec84281c2e08eb1bff3dca4244b7055d5`.
- During desktop and mobile-landscape gameplay, Playwright captures 12 frames from the actual `#canvas` element per device. `pngjs` checks that each frame has at least 97% pixels with an RGB channel above 12. The threshold allows dark scene colors while flagging a nearly all-black canvas. Failure reports the sample index and ratio.
- This test is intended to catch obvious intermittent black canvas frames in hosted Chromium. It can miss shorter events between samples and cannot prove the real Android symptom is gone. Do not describe this as a flicker fix.
- Run #335 (`45330722930`) passed on exact workflow/source-tree SHA `e5da108ec84281c2e08eb1bff3dca4244b7055d5`: build, C++ gameplay tests, Emscripten/WebAssembly, PWA validation and Chromium browser-smoke succeeded; `publish-dist` was skipped. Logs explicitly report desktop 12/12 and mobile-landscape 12/12 canvas samples passed, with minimum non-black pixel ratio `1.0000` on both.
- Previous gameplay verification remains run #334 (`45329538797`) on code/test SHA `9a83d69b2862b1329467952e4cafa020e25d75fd`; both build and browser-smoke succeeded, and `publish-dist` was skipped.
- The 14-image visual-review artifact from run #335 (`14892037925`) was inspected. No sampled black canvas frame appeared, but 12 samples per device can miss short events and do not prove the real-world flicker is fixed.
- A test-only raised-platform landing regression was added after run #335 in commit `1753b20e0ac2c6e796ddfbbd58601fdaaa6e476c`. It checks that falling onto a rooftop whose actual top is Y=2.3 snaps to and remains at Y=2.3, resets vertical speed, and begins landing recovery. This test is included in run #336.
- PR #16 has been restored to base PR #15; the draft stack remains #14 → #15 → #16. No merge, deployment or `dist/` edit.

### Next action for the next developer turn
1. Continue movement-foundation review with focused grounding, raised-floor, vault/ledge collision and enemy-navigation regressions. Avoid duplicating jump/landing tests already present.
2. After movement/rendering stability, implement the master specification's remaining systems incrementally: separate stamina and mana, level-up stat choices, skill-versus-upgrade milestone choices, vertical city/floor progression, enemy archetypes, fog-of-war exploration, guaranteed boss keys and performance-banded equipment.
3. Keep the PR stack and release hold unchanged. The intermittent black-flicker root cause and physical Android validation remain open.



## Latest exact-head run — 2026-11-24

- Run #336 (`45332306488`) passed on application source/test head `c0334c0d71bafda76e9a67a72dd5d9aebceede3c`.
- Build job `138236093166` and browser-smoke job `138236454555` both passed. Coverage includes the C++ regression suite with raised-platform landing, Emscripten/WebAssembly, PWA validation, Chromium desktop/mobile runtime, offline cache, boss-arena captures and forced context-loss recovery. `publish-dist` was skipped.
- Desktop and mobile-landscape browser logs both report all 12 canvas samples passed; the minimum non-black pixel ratio was `1.0000` for each device.
- Visual-review artifact `14892717266` uploaded 14 screenshots and was downloaded; the desktop gameplay capture was inspected. Finite Chromium sampling is not proof that the flicker is fixed on real Android hardware.
- PR #16 is stacked on PR #15, which is stacked on PR #14. All remain open/unmerged. Keep the release hold and zero-cost constraint.
- Branch commits after tested application source/test SHA `c0334c0d...` are documentation-only.



## Active implementation checkpoint — fortified five-city perimeter (2026-11-24)

- Added a 12-unit-high, 2-unit-thick perimeter to each city using the existing solid platform/collision representation. The west, east and south sides are continuous; the north side is split around a gate opening aligned with that city's existing final boss-route platform.
- Added gate towers and a lintel while preserving player clearance. Existing skyline pieces that would obstruct the gate approach are moved beyond the playable perimeter rather than removed.
- Added deterministic C++ regression coverage for all five cities: expected wall geometry, collision against all four sides, and passage through the aligned gate. Existing platform-count assertions were updated for the eight new wall/gate structures.
- Implementation commits: `cca3a538340e058bc0068ffa873e3b0197a07ae7` (walls), `fd61e6afc1194230c621defe9ce22dd82be854a8` (regressions), and `bdd3e22b78b21f8725114412612e65d02238d89b` (gate sightline clearance).
- Exact-head CI is pending. Do not mark the enclosure complete until the C++ gameplay suite, Emscripten build and Chromium smoke tests pass on this code/test head.
- PR #16 is temporarily targeted at `main` for the repository's required pull-request CI filter. After exact-head verification, restore it to PR #15 (`fix/boss-arena-floor-bounds`). Keep the original PR #14 → #15 → #16 stack, draft state, and release hold.
- No merge, production deployment, `dist/` change, billing change, or quota workaround was performed.

 
## Active implementation checkpoint — explored-area map (2026-11-24)

Development branch: `development/complete-game`.

Implemented in this branch:
- A 20-column × 16-row explored-area grid aligned to city world bounds (5 world units per cell).
- Nearby tiles are revealed around the player's actual world position; unknown cells remain hidden.
- The map draws platform/building footprints only where the corresponding cells are explored.
- Player position/facing is shown. The active objective marker appears only if its cell has already been explored.
- Keyboard M and an icon-only touch button toggle the map. Simulation pauses while the map is open.
- On touch screens, gameplay controls are hidden while the map is open except for the map button used to close it.
- Each city stores a fixed 320-character reveal state in its own localStorage key `tagbound-exploration-v1-<city>`, separate from the existing `tagbound-save` campaign record.
- Added C++ exploration boundary/reveal tests, browser keyboard/touch map tests, updated mobile control-count assertions, and service-worker cache version `tagbound-v28`.

Important release constraints:
- Do not merge to `main` or deploy to production without explicit authorization.
- Do not mark the map complete until the exact integration commit passes C++/Emscripten and desktop/mobile Chromium workflows.
- User must still confirm actual rendering and touch usability on Android after an authorized Preview is accessible.
- The root cause of the reported Android black flickering remains unknown; do not claim this map feature fixes it.

Next:
1. Create/inspect a draft PR targeting `main` so the repository CI runs on the exact development head.
2. Fix any compile, gameplay, browser, UI, or persistence failures without removing assertions.
3. Verify the artifact contains the new web UI/service-worker and WASM with map feature.
4. Continue with the next substantial incomplete gameplay feature: separate Energy/Mana and actual multi-skill progression, based on the specification once located or reconstructed from project documents. Do not invent a design that conflicts with the master spec.

 
## Boss arena enclosure update (2026-11-24)

- Each city now builds three 2.8-unit-high collision-bearing stone walls around its 8×8 raised boss deck. The south entrance remains open and aligned with the rooftop route.
- Boss arena half-extents are derived from the inner wall faces; boss body radius is accounted for by the existing clamp used during pursuit, knockback and the Gilded Charger strike.
- C++ tests verify wall geometry, boss containment and the open approach. This code still requires exact-head CI and later physical Android review; do not claim the user's original boss issue is proven fixed until tested in the new preview/device.

 
## Floor-aware exploration map update — 2026-11-24

- Exploration now contains five independent 20 × 16 layers per city (ground, three objective rooftop elevations, boss deck).
- Discovery updates only while the player is grounded. Each 5 × 5 world-unit cell is revealed near the player; the floor layers never reveal one another.
- The map displays only the selected discovered floor, with a player marker only on the player's current floor and an objective marker only when its floor/cell is discovered.
- Floor navigation uses icon-only previous/next controls and keyboard `[` / `]`; undiscovered floors are skipped and cannot be selected.
- The new localStorage key is `tagbound-exploration-v2-<city>` with 1,600 characters. Earlier v1 320-cell data is read into the ground layer; campaign key `tagbound-save` is unchanged.
- Service-worker cache version: `tagbound-v29`.
- Added C++ tests for height mapping, airborne reveal prevention, isolated floor state, and floor selection. CI browser checks verify the UI, mobile tap behavior, 1,600-cell save format, and reload persistence.
- This latest implementation and the boss-arena walls are not yet verified by exact-head CI; never inherit the earlier run #347 result as proof of the new code.

 
## Character resources and stat progression update — 2026-11-24

- Player now has separate Health, Stamina, and Mana. Class Mana bases are Vanguard 70, Skirmisher 90, Warden 80. Mana regenerates at 3.5/sec. Stamina continues to fund sprint, dash, vault/climb and block.
- Each level-up adds exactly one pending stat choice; movement/combat simulation pauses until it is selected.
- Bounded stats (rank 0–10): Vitality +8 max HP, Endurance +8 max Stamina, Focus +12 max Mana, Power +4% melee damage, Precision +4% bow damage. Capped choices remain unspent; one successful selection consumes one pending choice.
- Save schema is now v2, storing Mana, stat ranks and pending choices. Schema v1 saves preserve campaign progress and receive pending choices for previously completed levels not already represented by stat ranks.
- C++ tests cover derived resource limits, actual melee/bow damage scaling, caps, and Mana regeneration. Browser CI now tests one saved level-up selection and v1 migration.
- Service-worker cache is `tagbound-v30`.
- Mana is not yet consumed by active class skills in this checkpoint. The next gameplay batch must add the class skill selection at level milestones and connect each activated skill to Mana cost/cooldown, including the specified Skirmisher Arrow Rain targeting interaction.


## Class skill system update — 2026-11-24

- Class skills are actual combat systems: Vanguard has Cleaving Arc/Breaker Strike; Skirmisher has Arrow Rain/Piercing Shot; Warden has Shield Bash/Aegis Guard.
- Level 3 and level 5 each issue one pending skill choice. A milestone offers either a new class skill in an empty slot or an upgrade to an owned skill; ranks cap at 3. Two skill slots are available; one is equipped as the active technique action in Settings.
- Mana costs and cooldowns are separate from Stamina. Current costs: Cleaving Arc 24, Breaker Strike 30, Arrow Rain 38, Piercing Shot 28, Shield Bash 22, Aegis Guard 30. Arrow Rain is charged only on confirm; it has target validation, visible area/warning, then three damage waves. Cancel does not consume Mana. Aegis reduces direct pursuer/boss damage during its timer.
- Save schema v3 persists both skill slots, ranks, active slot, pending milestone choices, milestone mask, stats, Mana and campaign state. Older save versions get unclaimed level 3/5 milestones once.
- Added C++ skill regressions and Playwright browser workflows for skill-choice UI, equip/save, and Arrow Rain activation/confirmation. Exact-head CI for this current integration is pending.
- Web cache version is tagbound-v31.
