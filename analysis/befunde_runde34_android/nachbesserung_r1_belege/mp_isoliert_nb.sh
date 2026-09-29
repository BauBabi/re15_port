#!/usr/bin/env bash
# Nachbesserung R1 (nach dem Vorbild pruefer_echtlauf_r1_belege/mp_isoliert.sh): make_package.sh im
# Arbeitsbaum ECHT laufen lassen, git-Schreibzugriffe (git add/rm --cached der Split-Volumes) aber in
# einen WEGWERF-Index + WEGWERF-Objektspeicher lenken; vorhandene Objekte ueber ALTERNATES lesbar.
# Aufruf: LOG=<log> mp_isoliert_nb.sh <make_package-Argumente>   (Rueckgabe = die von make_package.sh)
set -u
BAUM=C:/workspace/git/reAi_v2/.claude/worktrees/r34a_android
G=$BAUM/build/r34a/nb/git_iso
cd "$BAUM" || exit 99
rm -rf "$G"; mkdir -p "$G/objects"
cp "$(git rev-parse --git-path index)" "$G/index" || exit 98
export GIT_INDEX_FILE="$G/index"
export GIT_OBJECT_DIRECTORY="$G/objects"
export GIT_ALTERNATE_OBJECT_DIRECTORIES="C:/workspace/git/reAi_v2/.git/objects"
exec bash "$BAUM/analysis/befunde_runde34_android/pruefer_echtlauf_r1_belege/lauf_mit_zeit.sh" bash release/make_package.sh "$@"
