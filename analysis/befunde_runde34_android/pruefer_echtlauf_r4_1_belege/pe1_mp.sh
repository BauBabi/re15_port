#!/usr/bin/env bash
# Pruefer echtlauf R4-1: EIN Lauf von release/make_package.sh, ECHT und unveraendert im Arbeitsbaum (kein Shim, PATH
# unveraendert). Nur die git-Schreibzugriffe (git add / git rm --cached der Split-Volumes) gehen in einen WEGWERF-Index
# + WEGWERF-Objektspeicher (vorhandene Objekte ueber ALTERNATES lesbar) - im selben Baum arbeitet ein zweiter Pruefer,
# dessen Commits sonst ~550 MB vorgemerkte Paketdateien mitnehmen koennten.
# Aufruf: pe1_mp.sh <tag> <make_package-Argumente...>  -> Log build/r34a/pruefer_e1/logs/mp/<tag>.log (letzte Zeile EXIT=)
set -u
BAUM=C:/workspace/git/reAi_v2/.claude/worktrees/r34a_android
W=$BAUM/build/r34a/pruefer_e1
G=$W/git_iso
TAG="$1"; shift
mkdir -p "$W/logs/mp"
cd "$BAUM" || exit 99
rm -rf "$G"; mkdir -p "$G/objects"
cp "$(git rev-parse --git-path index)" "$G/index" || exit 98
LOG="$W/logs/mp/$TAG.log"
{
    echo "START $(date '+%F %T') make_package.sh $*"
    GIT_INDEX_FILE="$G/index" GIT_OBJECT_DIRECTORY="$G/objects" \
    GIT_ALTERNATE_OBJECT_DIRECTORIES="C:/workspace/git/reAi_v2/.git/objects" \
        bash release/make_package.sh "$@"
    rc=$?
    echo "ENDE $(date '+%F %T')"
    echo "EXIT=$rc"
} > "$LOG" 2>&1
tail -1 "$LOG"
