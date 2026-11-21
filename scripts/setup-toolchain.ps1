$ErrorActionPreference = "Stop"
python -m pip install --user cmake ninja
if ($LASTEXITCODE -ne 0) { throw "CMake/Ninja installation failed." }
$sdk = Join-Path $env:USERPROFILE "emsdk"
if (!(Test-Path $sdk)) {
 git clone --depth 1 https://github.com/emscripten-core/emsdk.git $sdk
 if ($LASTEXITCODE -ne 0) { throw "Could not clone Emscripten SDK." }
}
Push-Location $sdk
try {
 .\emsdk.bat install latest
 if ($LASTEXITCODE -ne 0) { throw "Emscripten install failed." }
 .\emsdk.bat activate latest
 if ($LASTEXITCODE -ne 0) { throw "Emscripten activation failed." }
} finally { Pop-Location }
Write-Host "Toolchain installed. Open a new terminal before building."
