#!/usr/bin/env bash
# tests.sh - Abnahme 5 Spur AN (zweiter Abnehmer): eigene ctest-Laeufe im Baum r41_annette (Build = 99bbe97ba), nacheinander.
#   Git-Bash, PATH=/c/msys64/mingw64/bin:$PATH, SDL_ASSERT=always_ignore. Ausgabe je Gruppe nach belege/ctest_<gruppe>.txt
set -uo pipefail
export PATH=/c/msys64/mingw64/bin:$PATH SDL_ASSERT=always_ignore
B=/c/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/annb6/abn5/mein/belege
BD="C:/workspace/git/reAi_v2/.claude/worktrees/r41_annette/re15_port/build"
run() { local name="$1"; shift; echo "== $name start $(date '+%F %T')" | tee -a "$B/ctest_zeiten.txt"; ctest --test-dir "$BD" "$@" > "$B/ctest_$name.txt" 2>&1; echo "EXIT=$?" >> "$B/ctest_$name.txt"; echo "== $name ende $(date '+%F %T') $(grep -h 'tests passed' "$B/ctest_$name.txt")" | tee -a "$B/ctest_zeiten.txt"; }
case "${1:-alle}" in
  annette) run unit_annette -R '^unit_r41_annette' -V ;;
  suite)   run suite -R '^unit_|^prop_|^probe_' -E 'releaskript|releaseskript|titelversion|android|paket|r37_masken36_ausschluss' -j 2 --timeout 600 ;;
  integ1)  run integ1 -R '^integration_r35_zgirl_5090$|^integration_r36_story_leon$|^integration_r36_story_elza$|^integration_r41_annette$|^integration_r41_begleiter_schusslinie$|^integration_r41_kollision_npc3070$|^integration_r41_kollision_npc3001$|^integration_r41_birkin_pistole$' -j 1 --timeout 1200 ;;
  integ2)  run integ2 -R '^integration_r32_tor_hell$|^integration_r35_zgirl$|^integration_r36_elza_|^integration_r37_elzaems_|^integration_r37_sperre1050$|^integration_r37_szene11c0$|^integration_r38_elzatoene_|^integration_r38_tagebuch_|^integration_r41_wasser20b0$|^integration_r41_wasser20b0_npc$' -j 1 --timeout 1200 ;;
  integ3)  run integ3 -R '^integration_r35_granate$|^integration_r41_masken36_zellen3d$|^integration_r41_masken36_teile_c1$|^integration_r41_masken36_teile_c2$|^integration_r41_masken36_teile_c3$|^integration_r41_masken36_teile_d2$|^integration_r41_g5_westen$|^integration_r41_g5_tod$' -j 1 --timeout 1500 ;;
  integ3rot) run integ3rot -R '^integration_r41_masken36_teile_c2$|^integration_r41_masken36_teile_c3$|^integration_r41_masken36_teile_d2$|^integration_r41_g5_westen$|^integration_r41_g5_tod$' -j 1 --timeout 1500 ;;
  integ3rest) run integ3rest -R '^integration_r35_granate$|^integration_r41_masken36_zellen3d$|^integration_r41_masken36_teile_c1$' -j 1 --timeout 1500 ;;
  *) echo "gruppe?"; exit 2 ;;
esac
