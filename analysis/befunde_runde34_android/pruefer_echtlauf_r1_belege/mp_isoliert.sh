#!/usr/bin/env bash
# Pruefer echtlauf r1: make_package.sh im Arbeitsbaum ECHT laufen lassen, aber git-Schreibzugriffe
# (git add der neuen Split-Volumes, git rm --cached alter) in einen WEGWERF-Index + WEGWERF-Objektspeicher
# lenken (GIT_INDEX_FILE = Kopie des Baum-Index, GIT_OBJECT_DIRECTORY = leerer Ordner, bestehende Objekte
# ueber GIT_ALTERNATE_OBJECT_DIRECTORIES lesbar). Grund: ~500 MB Paket-Blobs sollen nicht als Muell im
# gemeinsamen .git/objects des Hauptbaums landen. Alles andere (Gates, Kopieren, Zippen, git log fuer
# check_binary_fresh) laeuft unveraendert. Aufruf: LOG=<log> mp_isoliert.sh <make_package-Argumente>
set -u
BAUM=C:/workspace/git/reAi_v2/.claude/worktrees/r34a_android
G=$BAUM/build/r34a/pruefer_echtlauf_r1/git_iso
export GIT_INDEX_FILE="$G/index"
export GIT_OBJECT_DIRECTORY="$G/objects"
export GIT_ALTERNATE_OBJECT_DIRECTORIES="C:/workspace/git/reAi_v2/.git/objects"
cd "$BAUM" || exit 99
exec bash "$BAUM/analysis/befunde_runde34_android/pruefer_echtlauf_r1_belege/lauf_mit_zeit.sh" bash release/make_package.sh "$@"
