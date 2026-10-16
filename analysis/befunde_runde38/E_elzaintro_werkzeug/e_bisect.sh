#!/usr/bin/env bash
# Runde 38 Spur E: Bisect-Bau eines alten Stands OHNE den Arbeitsbaum umzuschalten.
#   e_bisect.sh <commit> <marke>
# Quelle: git archive <commit> re15_port (ohne shared_assets) nach re15_port/build_bisect/src_<marke>/ (ignoriert,
# .gitignore re15_port/build_*/), Bau mit dem local_build.sh DIESES Stands nach re15_port/build_bisect/b_<marke>
# (Tests AUS), Assets ueber RE15_ASSETS_PATH aus dem Arbeitsbaum. Seit v0.8.23 aenderten sich in shared_assets/PSX
# nur MASKS fuer ROOM10F0/1100/1221/2091 (git diff --stat 3ec5b1f8 HEAD -- re15_port/shared_assets/PSX) - keiner der
# gemessenen Raeume (1240/1241/1031/1170).
set -uo pipefail
ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/../../.." && pwd)"
C="${1:?commit}"; M="${2:?marke}"
SRC="$ROOT/re15_port/build_bisect/src_$M"
rm -rf "$SRC"; mkdir -p "$SRC"
git -C "$ROOT" archive "$C" re15_port ':(exclude)re15_port/shared_assets' | tar -x -C "$SRC" || exit 2
export RE15_ASSETS_PATH="$(cygpath -m "$ROOT/re15_port/shared_assets/PSX")"
export RE15_BUILD_DIR="$ROOT/re15_port/build_bisect/b_$M" RE15_TESTS=OFF
bash "$SRC/re15_port/tools/local_build.sh" configure && bash "$SRC/re15_port/tools/local_build.sh" build
echo "[e_bisect] $C -> $RE15_BUILD_DIR/platform/pc/re15_pc.exe rc=$?"
