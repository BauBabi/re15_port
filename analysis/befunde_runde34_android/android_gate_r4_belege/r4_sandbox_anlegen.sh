#!/usr/bin/env bash
# Runde 4 (Kette): Sandbox-Repo fuer echte Laeufe von make_package.sh / build_android.sh --gate-only, OHNE
# release/ oder den git-Index des Arbeitsbaums anzufassen. Vorbild pruefer_umgehung_r3_belege/r3_sandbox_anlegen.sh
# (dieselbe Logik, eigener Ordner):
#   <S>/release/        Kopien der AKTUELLEN Release-Skripte des Arbeitsbaums (+ pkg_files, apk_signer.sha256),
#                       per cmp gegen den Arbeitsbaum geprueft
#   <S>/re15_port/shared_assets/{PSX,extracted_fx,RE2,RE15DOOR}, <S>/synchro/STAGE*   HARDLINKS (cp -al) - nur
#                       gelesen; wer eine Datei aendert, ersetzt vorher den Link durch eine Kopie
#   <S>/re15_port/{include/re15_door_seq.h, engine/src/gen/*.inc, platform/android/app/build.gradle}  Kopien
#   eigenes git-Repo mit EINEM Commit (Datum 2026-09-29 12:00: die Frische-Gates nehmen die v0.8.19-Binaries
#   von 19:34 und die Referenz-APK ohne touch an)
# Aufruf: r4_sandbox_anlegen.sh <S>
set -euo pipefail
S="$1"
cd C:/workspace/git/reAi_v2/.claude/worktrees/r34a_android
[[ ! -e "$S" ]] || { echo "Sandbox $S existiert schon"; exit 1; }
mkdir -p "$S/release" "$S/re15_port/shared_assets" "$S/synchro" "$S/re15_port/include" \
         "$S/re15_port/engine/src/gen" "$S/re15_port/platform/android/app"
SKRIPTE=(make_package.sh apk_pruefen.sh python_finden.sh apk_asset_gate.py zip_exec_bit.py apk_signer.sha256 build_android.sh)
for f in "${SKRIPTE[@]}" .gitattributes; do cp -p "release/$f" "$S/release/$f"; done
cp -rp release/pkg_files "$S/release/pkg_files"
for t in PSX extracted_fx RE2 RE15DOOR; do cp -al "re15_port/shared_assets/$t" "$S/re15_port/shared_assets/$t"; done
for d in synchro/STAGE*; do cp -al "$d" "$S/synchro/$(basename "$d")"; done
cp -p re15_port/include/re15_door_seq.h "$S/re15_port/include/"
cp -p re15_port/engine/src/gen/re15_tuer_eigen.inc re15_port/engine/src/gen/re2_tuer_tabelle.inc \
      re15_port/engine/src/gen/tuer_zuordnung.inc "$S/re15_port/engine/src/gen/"
cp -p re15_port/platform/android/app/build.gradle "$S/re15_port/platform/android/app/"
for f in "${SKRIPTE[@]}"; do cmp -s "release/$f" "$S/release/$f" || { echo "Kopie weicht ab: $f"; exit 1; }; done
(
  cd "$S"
  git init -q
  git config user.email r4-kette@sandbox.invalid; git config user.name "R4 Kette Sandbox"
  git config core.autocrlf false
  printf 're15_port/shared_assets/\nsynchro/\nrelease/pkg-*/\nrelease/linux_out/\nrelease/win_out/\n*.apk\n' > .gitignore
  git add .gitignore re15_port/include re15_port/engine re15_port/platform release/*.sh release/*.py release/apk_signer.sha256
  GIT_AUTHOR_DATE="2026-09-29T12:00:00+0200" GIT_COMMITTER_DATE="2026-09-29T12:00:00+0200" \
      git commit -q -m "Sandbox R4 Kette (Quellbaum per Hardlink)"
  git log -1 --format='Sandbox-Commit %h %ci'
)
echo "Skripte = Arbeitsbaum: ${SKRIPTE[*]}"
echo "Dateien: PSX $(find "$S/re15_port/shared_assets/PSX" -type f | wc -l), fx $(find "$S/re15_port/shared_assets/extracted_fx" -type f | wc -l)," \
     "RE2 $(find "$S/re15_port/shared_assets/RE2" -type f | wc -l), RE15DOOR $(find "$S/re15_port/shared_assets/RE15DOOR" -type f | wc -l)," \
     "synchro $(find "$S/synchro" -type f | wc -l)"
