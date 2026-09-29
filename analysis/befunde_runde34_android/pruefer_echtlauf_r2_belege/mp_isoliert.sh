#!/usr/bin/env bash
# Pruefer echtlauf r2 (Vorbild r1/nb): make_package.sh im Arbeitsbaum ECHT und UNVERAENDERT laufen lassen;
# nur die git-SCHREIBzugriffe am Ende (git add der neuen Split-Volumes, git rm --cached alter) gehen in einen
# WEGWERF-Index (Kopie des Baum-Index) + WEGWERF-Objektspeicher; vorhandene Objekte ueber ALTERNATES lesbar.
# Grund: ~480 MB Paket-Blobs sollen nicht im gemeinsamen .git/objects landen, und der ECHTE Index dieses
# Baums wird auch vom Gegenpruefer Umgehung R2 zum Committen benutzt (vorgemerkte Volumes gingen sonst in
# dessen Commit). Aufruf: LOG=<log> mp_isoliert.sh <make_package-Argumente>  (Rueckgabe = make_package.sh)
set -u
BAUM=C:/workspace/git/reAi_v2/.claude/worktrees/r34a_android
G=$BAUM/build/r34a/pruefer_echtlauf_r2/git_iso
cd "$BAUM" || exit 99
rm -rf "$G"; mkdir -p "$G/objects"
cp "$(git rev-parse --git-path index)" "$G/index" || exit 98
export GIT_INDEX_FILE="$G/index"
export GIT_OBJECT_DIRECTORY="$G/objects"
export GIT_ALTERNATE_OBJECT_DIRECTORIES="C:/workspace/git/reAi_v2/.git/objects"
exec bash "$BAUM/analysis/befunde_runde34_android/pruefer_echtlauf_r2_belege/lauf_mit_zeit.sh" bash release/make_package.sh "$@"
