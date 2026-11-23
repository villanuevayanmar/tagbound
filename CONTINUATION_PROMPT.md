# TAGBOUND Continuation Prompt

You are the dedicated cloud-development team for **TAGBOUND: The Five Cities**.

Continue the actual existing game until its agreed five-city scope is complete, polished, tested, deployed, and release-ready. Do not stop at a plan, prototype, or merely playable build. Do not ask whether to continue after each task. Work on actionable tasks in the current session, verify them, checkpoint them, and move to the next unfinished requirement.

## Project
- Repository: https://github.com/villanuevayanmar/tagbound
- Production: https://tagbound.vercel.app
- Stack: C++17, raylib 5.5, Emscripten, WebAssembly, WebGL, JavaScript, HTML/CSS, PWA, CMake, GitHub Actions, Vercel.
- The C++ simulation and renderer use raylib. The browser layer provides the interface, touch controls, PWA installation, caching, and story.
- Three.js is a reference for studying 3D techniques. Do not replace raylib or add a second renderer without a justified reason.

## Hard constraints
- The user no longer has a laptop and uses ChatGPT on Android. Never require local commands, terminals, PowerShell, local compilers, Emscripten, CMake, npm, or local tests. Use authorized GitHub, GitHub Actions, hosted Chromium, and Vercel tools.
- Cost must remain ₱0 / $0. No paid services, APIs, subscriptions, upgrades, billing changes, purchases, or quota bypasses.
- Preserve repository history, working features, and save behavior. Never force-push or perform destructive production changes.
- Never claim a test, merge, deployment, or physical-device test happened unless verified.
- The assistant cannot keep running in the background or detect an internet disconnection after it occurs. Commit and update these continuity documents frequently so the next session can resume from GitHub.

## Non-negotiable game requirements
- Exactly three classes and fixed weapons: Vanguard → Sword; Skirmisher → Bow; Warden → Shield. No weapon switching. The Bow touch control appears only for Skirmisher.
- PWA installation is available only on Home and inside the in-game pause menu, never during active gameplay.
- The in-game pause menu pauses the actual C++ simulation, preserves the run, clears held joystick/touch/look inputs, supports resume, shares the sound preference, and provides installation.
- Campaign: Harbor, Gilded, Foundry, Imperial, and Spire. Preserve each city's routes, objectives, and boss.
- Preserve parkour, combat, stamina/health, pursuers, boss telegraphs, progression, ending, local saves, and offline PWA behavior.
- Mix immediate reach-beacon and timed hold-beacon objectives. Expand variety without breaking existing reset rules.

## Icon-only in-game interface rule
Visible action controls and the in-game pause interface must use game-themed icons, not written action labels. This includes the vault/climb, slide/drop, equipped-weapon action, guard, bow, escape skill, sprint, and pause controls. Pause-menu sound, install, and resume actions should also be icon-only. Provide accessible `aria-label` text for assistive technology, but do not render those labels as visible button text or hover titles. Remove the word-based touch-help strip. Keep useful story, class, objective, boss, health, and combat-status information when text is necessary to understand the game; the icon-only rule is for controls and pause actions, not a requirement to erase useful game information.

## Session start checklist
1. Inspect the actual current `main` branch, repository tree, recent commits, pull requests, GitHub Actions, and production deployment.
2. Read `README.md`, `DEVELOPMENT_STATUS.md`, and this file. Compare documentation with the real source and test evidence.
3. Inspect source, browser integration, PWA/service worker, CMake, tests, workflows, Vercel config, and generated `dist/` files as relevant.
4. Identify the exact next unfinished task. Do not repeat work already completed unless fixing a verified regression.
5. Implement through cloud tools, test, fix failures, checkpoint, and continue.

## Testing and delivery
Use hosted CI for WebAssembly builds, C++ regressions, desktop/mobile Chromium, touch controls, class loadouts, HUD visibility, pause/resume, audio, save restoration, PWA install placement, service-worker cache, and offline reload. Update the service-worker cache version when changing cached assets. Keep `web/` and the production `dist/` copies synchronized; Vercel serves `dist/`.
Only deploy after relevant checks pass. Confirm the actual production deployment and run live smoke against that deployment. Physical Android testing remains separate: ask the user to test the ready deployed PWA and report concrete findings; never claim it was tested by the assistant.

## Continuity rules
Maintain:
- `DEVELOPMENT_STATUS.md`: verified project state, completed work, tests, current deployment, known issues, remaining work, current phase, exact next task.
- `CONTINUATION_PROMPT.md`: this ready-to-copy recovery prompt, updated whenever requirements, branch state, tested work, deployment, or next task changes.

Commit meaningful work before risky or interruption-prone operations and after verified milestones. Do not store credentials or secrets in either document. After each substantial session, leave the repository with accurate status and a current next action.

## Baseline known at 2026-11-23
- Main commit observed before the icon-only controls work: `a18e39e9eca221a15d75c4070de9d06ef94abc3e`.
- Latest generated WebAssembly runtime bundle observed: `9115f13b4021f9052a95eea66ceaa1000f134978`.
- The latest recorded main CI run #247 and live production smoke #9 passed. The earlier live smoke #8 failed because it ran against the older deployment before the runtime bundle had updated; the later live smoke #9 passed.
- PRs #1–#12 were observed merged, including the Settings/pause system, HUD rendering, input reset, ambient score, class-specific Bow control, hold-beacon objectives, death-frame attack prevention, all-class campaign progression, boss warning-range alignment, live-smoke automation, stamina HUD fix, and production smoke rerun.
- Physical Android testing has not been recorded. The user can test the deployed game on Android; no laptop or local commands may be requested.
- The existing project is still described by its README as an early playable foundation with substantial gameplay, visual, animation, quest, balancing, and physical-device verification work remaining.

## Latest verified continuation checkpoint — 2026-11-23

- PR #13 remains open and unmerged: https://github.com/villanuevayanmar/tagbound/pull/13
- Run #254 (45121695420) for commit 313c9f97aba3e7144c0f53fbe4e15ff7e6290474 completed with failure. The WebAssembly/C++ build job passed; the browser-smoke job failed at the class-specific Bow visibility assertion because .action{display:grid} overrode the browser's default [hidden] display behavior. Vanguard incorrectly displayed the Bow action even though its saved weapon mapping was still Sword.
- Root cause fixed without weakening tests: added .action[hidden]{display:none} to web/styles.css and synchronized dist/styles.css. Commits: 65909d7bea9c2820bb89665ad63d806e9dbd6c14 and 0206ec4534b856e9955967ec204f5e60211e974b.
- DEVELOPMENT_STATUS.md was updated with the exact failure evidence and correction.
- Immediate next action: inspect the latest PR head and its matching newest Actions run after the CSS fix and documentation commits. Confirm that the fixed CSS exists identically in web/styles.css and dist/styles.css. Wait for and inspect all build/browser jobs. Do not merge until the latest exact-head run passes. If another failure occurs, inspect its job logs and fix the underlying cause.
- PR #13 is not merged; icon-only controls are not in production. Existing production remains at the previously verified runtime until a successful merge and automatic deployment.

## Immediate continuation at the last checkpoint

Checkpoint note: the user explicitly asked to stop development for now and create a continuation prompt. No merge or production release of PR #13 has been authorized or performed. Resume only when the user starts another development session.
The icon-only interface work is on branch `feat/icon-only-game-controls` and open pull request [#13](https://github.com/villanuevayanmar/tagbound/pull/13). It updates `web/index.html`, `web/styles.css`, `web/pwa.js`, `web/sw.js`, matching committed `dist/` copies, normal CI assertions, live-smoke assertions, README guidance, and this continuity documentation. The service-worker cache was bumped from `tagbound-v25` to `tagbound-v26`. The pull request also updates `DEVELOPMENT_STATUS.md`.

At the last run inspection, PR #13 was open. Runs #248 (`45121351189`) and #249 (`45121459290`) were cancelled after later commits superseded their tested heads. Run #250 (`45121515646`) was still in progress for an earlier head. Run #251 (`45121627995`) was pending for PR head `533157582ed3bbe18963536ade08b3dcc8516d73`. Continuity-document commits were made afterward and may have triggered newer runs, so these run numbers are historical checkpoint information only. At the start of the next session, fetch the current PR head and the newest Actions run matching it; do not assume any of the listed runs tests the current branch tip.

Current production deployment was inspected through Vercel and is **READY**:
- Deployment ID: `dpl_GELuDKX6B4NragFQ6DSWjSNwFHAJ`
- URL: https://tagbound-3ly5acjad-yanmar.vercel.app
- Production alias: https://tagbound.vercel.app
- Runtime bundle commit: `9115f13b4021f9052a95eea66ceaa1000f134978`

**Next action:** the user paused development after asking for this continuation prompt. Do not make further implementation changes until the next session. On resumption, inspect PR #13 and the newest Actions run matching its current head; historical run #251 (`45121627995`) may no longer test the latest commit. Fix every failed assertion or build step and rerun the checks. The icon-only browser assertions must verify that action and pause controls contain SVG icons, retain accessible labels, and render no visible button text. Preserve the fixed class weapon mapping and Bow visibility rules. Once all required CI checks pass, merge PR #13 using the normal GitHub workflow. Then verify that Vercel automatically deploys the updated committed `dist/` files, inspect the new READY deployment and exact commit, and manually dispatch the live production smoke workflow against that deployment. Do not call the UI change finished or deployed until those steps pass. Update both continuity documents with exact verified commits, run IDs, and deployment details. Then resume the broader remaining gameplay, visual, animation, performance, audio, and release-readiness work.

**Final objective:** finish the agreed TAGBOUND game scope. Keep costs at zero, keep all work cloud-based, preserve the existing project, and never misrepresent unverified work as complete.

## Latest checkpoint — Bow control default visibility fix (2026-11-23)

- CI run #257 (45123933188) failed browser-smoke after its build passed. Vanguard's Bow button DOM `hidden` property was false.
- Corrected the initial markup by adding the native `hidden` attribute to the Bow button in `web/index.html` and `dist/index.html`; the existing JavaScript reveals it only for Skirmisher. CSS `.action[hidden]{display:none}` remains in both stylesheets.
- Commits: c763d3a0058001b77053a2d9caff41b45ea4b86c and 34966f568602b272b5878ade4ec5ce0699ac4f4a. Development status update commit: 0c31620f08ec58c71f9df57992230528d1391c59.
- Immediate next task: inspect the newest PR #13 Actions run for the latest branch head. Confirm Bow hidden property is false only for Skirmisher and all desktop/mobile checks pass. Do not merge until latest exact-head CI passes. Then verify deployment and continue remaining game scope.