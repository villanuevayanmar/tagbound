#!/usr/bin/env bash
set -euo pipefail

artifact_url='https://sdmntprindiasocentral.oaiusercontent.com/files/00000000-b02c-8211-94d7-1d42aa8189a1/raw?se=2026-11-24T06%3A57%3A33Z&sp=r&sv=2026-06-06&sr=b&scid=ef4cad6e-b594-5169-9f23-f6c8e05ed2d5&skoid=fb469448-2cc9-41bd-a5a6-6e020952b47a&sktid=a48cca56-e6da-484e-a814-9c849652bcb3&skt=2026-11-24T02%3A12%3A13Z&ske=2026-11-25T02%3A12%3A13Z&sks=b&skv=2026-06-06&sig=VQgv2h9oNTHFvNICYkk%2B4JynXJuE4kMqaNwZYYghxcs%3D'
archive="$(mktemp)"
trap 'rm -f "$archive"' EXIT

curl --fail --location --silent --show-error "$artifact_url" --output "$archive"
rm -rf dist
mkdir -p dist
unzip -q "$archive" -d dist

test -s dist/index.html
test -s dist/game.js
test -s dist/game.wasm
