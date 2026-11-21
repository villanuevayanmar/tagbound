$ErrorActionPreference = "Stop"
$root = Split-Path -Parent $PSScriptRoot
$sdk = Join-Path $env:USERPROFILE "emsdk"
$emscripten = Join-Path $sdk "upstream\emscripten"
if (!(Test-Path (Join-Path $emscripten "emcc.exe"))) { throw "Emscripten is not installed. Run scripts\setup-toolchain.ps1 first." }
$env:EMSDK = $sdk
$env:PATH = "$emscripten;$sdk\upstream\bin;$env:PATH"
$env:EMSDK_NODE = (Get-ChildItem (Join-Path $sdk "node") -Filter node.exe -Recurse | Select-Object -First 1 -ExpandProperty FullName)
$env:EMSDK_PYTHON = (Get-ChildItem (Join-Path $sdk "python") -Filter python.exe -Recurse -ErrorAction SilentlyContinue | Select-Object -First 1 -ExpandProperty FullName)
$pythonScripts = python -c "import sysconfig; print(sysconfig.get_path('scripts', scheme='nt_user'))"
$env:PATH = "$pythonScripts;$env:PATH"
if (!$env:EMSDK_NODE) { throw "Emscripten bundled Node.js not found." }
if (!$env:EMSDK_PYTHON) { throw "Emscripten bundled Python not found." }
Set-Location $root
New-Item -ItemType Directory -Force -Path (Join-Path $root "dist") | Out-Null
python (Join-Path $root "scripts\generate_icons.py")
if ($LASTEXITCODE -ne 0) { throw "PWA icon generation failed." }
emcmake cmake -S . -B build-web -G Ninja -DPLATFORM=Web -DCMAKE_BUILD_TYPE=Release -DBUILD_EXAMPLES=OFF
if ($LASTEXITCODE -ne 0) { throw "CMake configure failed." }
cmake --build build-web --config Release --parallel 2
if ($LASTEXITCODE -ne 0) { throw "WebAssembly compilation failed." }
Copy-Item (Join-Path $root "web\*") (Join-Path $root "dist") -Recurse -Force
Write-Host "Web build ready in dist\"
