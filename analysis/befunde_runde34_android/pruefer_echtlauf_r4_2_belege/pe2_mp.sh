#!/usr/bin/env bash
# pe2_mp.sh <tag> <make_package-Argumente...>
# make_package.sh ECHT und unveraendert im Arbeitsbaum (kein Shim, PATH unveraendert). Nur git schreibt in einen
# Wegwerf-Index (Kopie des echten Index) + Wegwerf-Objektspeicher (Alternates = echter Objektspeicher): im Baum
# arbeitet parallel ein zweiter Pruefer, dessen Commits sonst die Paket-Volumes mitnaehmen koennten.
set -u
T=C:/workspace/git/reAi_v2/.claude/worktrees/r34a_android
W=$T/build/r34a/pruefer_e2
TAG="$1"; shift
mkdir -p "$W/logs/mp" "$W/git"
GD="$(git -C "$T" rev-parse --absolute-git-dir)"
CD="$(cd "$T" && git rev-parse --path-format=absolute --git-common-dir)"
cp "$GD/index" "$W/git/index_$TAG"
rm -rf "$W/git/objects_$TAG"; mkdir -p "$W/git/objects_$TAG"
export GIT_INDEX_FILE="$W/git/index_$TAG"
export GIT_OBJECT_DIRECTORY="$W/git/objects_$TAG"
export GIT_ALTERNATE_OBJECT_DIRECTORIES="$CD/objects"
L="$W/logs/mp/$TAG.log"
t0=$(date +%s)
( cd "$T" && echo "== pe2_mp $TAG: make_package.sh $* ($(date '+%F %T'))" && bash release/make_package.sh "$@" ) > "$L" 2>&1
rc=$?
echo "EXIT=$rc ($(( $(date +%s) - t0 )) s)" >> "$L"
echo "$TAG EXIT=$rc ($(( $(date +%s) - t0 )) s)"
