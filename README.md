# TAGBOUND: The Five Cities

A mobile-first, low-poly fantasy rooftop chase. The game simulation and 3D renderer are C++17 compiled to WebAssembly with Emscripten and raylib. The browser layer supplies touch controls, PWA installation, offline caching and the opening story.

## Current playable scope

- Three selectable classes: Vanguard, Skirmisher and Warden. Each class has one fixed weapon; players cannot switch weapons.
- Five city environments with distinct low-poly landmarks and increasing pursuer toughness and speed.
- Procedural articulated player, pursuer, and boss models with animated movement poses.
- Procedurally synthesized sound effects for jumps, melee, bow shots, dashes, damage and boss telegraphs; no audio downloads required.
- A Home-screen sound toggle with a saved mute preference.
- Forward mantle vaults and ledge grabbing, with a stable hanging state, jump-to-climb transition and safe drop input.
- Landing recovery poses and slide-assisted forward rolls after hard falls.
- Timed melee strike poses, enemy impact flashes, short hit-stun and knockback feedback.
- Rooftop traversal with running, sprinting, jumping, sliding, solid building-side collision, wall-running and wall-jumping.
- Pursuit pressure, health, stamina and class-exclusive weapons: Vanguard sword, Skirmisher bow and Warden shield.
- Three beacon quests per city, followed by five uniquely named bosses with different strike patterns, attack ranges, visual designs and telegraphed warning zones.
- Shared beacon and boss-gate coordinates drive gameplay triggers, 3D markers and the live HUD distance readout.
- A rotating objective compass points toward the active beacon or boss gate relative to the player's facing direction.
- XP, levels, two skill unlock milestones and local progress saving.
- Landscape-first mobile controls and keyboard/mouse support.
- PWA manifest, phone icons, service worker and offline asset cache.

This is an early playable foundation, not a finished commercial game. Character models, animation, detailed city layouts, advanced parkour traversal, sound design, quest variety and complete boss encounter tuning remain development work. The current renderer uses procedural low-poly geometry. No online accounts or game server are required.

## Requirements

- Windows 10/11, macOS or Linux
- Git
- Python 3.10+ (only needed for icon generation / local static server)
- CMake 3.20+
- Emscripten SDK (latest stable)
- Internet connection for the first build: CMake fetches raylib 5.5 from GitHub

The Emscripten SDK includes its own compiler and Node.js. CMake can be installed with: python -m pip install --user cmake ninja.

## Windows setup

From the project root in PowerShell:

    Set-ExecutionPolicy -Scope Process Bypass
    .\scripts\setup-toolchain.ps1

Open a new PowerShell terminal after setup, then run:

    .\scripts\build-web.ps1

The script configures raylib for WebGL, compiles game.js and game.wasm, and copies the PWA files into dist/. The generated dist/ folder is committed so Vercel can serve a static build without running a C++ toolchain.

## macOS / Linux setup

Install Git, CMake, Python and Ninja, then install Emscripten following the official SDK instructions: https://emscripten.org/docs/getting_started/downloads.html. Activate the SDK in your terminal:

    git clone https://github.com/emscripten-core/emsdk.git
    cd emsdk
    ./emsdk install latest
    ./emsdk activate latest
    source ./emsdk_env.sh
    cd /path/to/tagbound
    emcmake cmake -S . -B build-web -DPLATFORM=Web -DCMAKE_BUILD_TYPE=Release
    cmake --build build-web --config Release --parallel
    cp -R web/. dist/

The CMake target emits the generated JavaScript and WebAssembly files in dist/.

## Automated cloud checks

Every push to `main` and pull request runs the GitHub Actions workflow in `.github/workflows/ci.yml`. It configures Emscripten, compiles the WebAssembly game, runs the C++ gameplay regression test executable, assembles and validates the static PWA, and runs Chromium checks for the browser runtime, start flow, saved class/progress, mobile landscape controls, service-worker precache and offline reload. These checks run in hosted CI; they do not replace playtesting on a physical Android device.

The C++ regression tests are in `tests/gameplay.cpp`. To run them locally with an active Emscripten SDK and a configured build directory:

    em++ -std=c++17 -O2 -Isrc -Ibuild-web/_deps/raylib-src/src \
      src/game.cpp src/systems.cpp tests/gameplay.cpp \
      -sENVIRONMENT=node -sEXIT_RUNTIME=1 -o gameplay-tests.js
    node gameplay-tests.js

Check the Actions tab for the result of each commit: https://github.com/villanuevayanmar/tagbound/actions.

## Test locally

Serve dist/ over HTTP (do not open index.html as a file):

    python -m http.server 8000 --directory dist

Open http://localhost:8000. WebAssembly, service workers and PWA install prompts require a secure context; localhost is treated as secure by browsers.

## GitHub push

Create an empty repository named tagbound under your GitHub account. From the project root:

    git init
    git add .
    git commit -m "Build TAGBOUND WebAssembly PWA"
    git branch -M main
    git remote add origin https://github.com/YOUR_USERNAME/tagbound.git
    git push -u origin main

If the Git repository already exists, inspect its remote and branch before pushing; do not replace its history with --force.

## Deploy to Vercel

1. Sign in to Vercel and choose Add New → Project.
2. Import the GitHub repository.
3. Keep the project root at the repository root.
4. Set Output Directory to dist.
5. Leave Build Command empty because the generated static build is committed.
6. Deploy. Later pushes to main trigger new deployments when Git integration is enabled.

The included vercel.json declares the static output directory, WebAssembly MIME type, and cache rules for the service worker and generated assets.

## Install on a phone

### iPhone / iPad

1. Open the deployed HTTPS URL in Safari.
2. On TAGBOUND's Home screen, tap INSTALL ON PHONE, then follow Safari's Share → Add to Home Screen instructions.
3. Confirm the name and tap Add.
4. Launch TAGBOUND from the Home Screen. Turn the device sideways.

iOS Safari does not consistently support JavaScript orientation locking. The app uses a landscape-first layout and shows a portrait rotation notice instead of claiming to force the device to rotate.

### Android

1. Open the deployed HTTPS URL in Chrome.
2. On TAGBOUND's Home screen, tap INSTALL ON PHONE and confirm the browser's install prompt. If it is unavailable, use Chrome's menu and choose Install app / Add to Home screen.
3. Launch TAGBOUND from its new home-screen icon.

The first visit must download the game assets. The service worker caches the HTML, JS, WASM, stylesheet, manifest and icons for later offline launches. Clear the browser's site data to remove the offline cache.

## Controls

### Touch

- Left joystick: move
- Right side of the screen: swipe to look
- Vault: jump; near a reachable wall, face it and jump to mantle over the edge; when hanging, jump to climb onto the roof
- Slide: short speed boost while grounded; hold it through a hard landing to roll forward; while hanging, slide or push down on the joystick to drop
- Attack: use equipped weapon
- Block: defend against incoming attacks
- Bow: aim with the Skirmisher's bow; use Attack to fire
- Skill: one-tap escape dash after level 3 (stamina cost and cooldown)
- Sprint: hold; sprint beside a wall while airborne to wall-run, then tap Vault to wall-jump

### Keyboard / mouse

- WASD or arrow keys: move
- Mouse right-drag: look / block
- Space: jump
- Shift: sprint
- Ctrl or C: slide
- Left mouse: attack
- E: escape skill after unlock

## Save data

Progress is saved to browser localStorage on the device. Saves are local to each browser profile and do not automatically synchronize across devices.

## Zero-cost notes

The project has no paid engine, API, database or server requirement. GitHub and Vercel plan limits still apply. This is a static PWA, not an authoritative multiplayer game server.