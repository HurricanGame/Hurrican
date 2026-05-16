#!/usr/bin/env sh
set -eu
root=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)
png2icns "$root/darwin/Hurrican.icns" "$root/data/textures/hurrican_rund.png"
