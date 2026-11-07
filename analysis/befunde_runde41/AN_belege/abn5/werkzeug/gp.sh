#!/usr/bin/env bash
# gp.sh - Abnahme 5 (zweiter Abnehmer): Gegenproben zum Riegel test_r41_annette AUSSERHALB des Arbeitsbaums.
# basis = Objekte des Baums (99bbe97ba); gp1 = re15_damage.c aus cefd57402 (case 0x4d 350/1350); gp2 = annette_r41.c ohne re2_segment-Aufrufe.
set -uo pipefail
export PATH=/c/msys64/mingw64/bin:/usr/bin:$PATH SDL_ASSERT=always_ignore
G="${1:?scratch-ordner}"; mkdir -p "$G"; cd "$G" || exit 2
WT=C:/workspace/git/reAi_v2/.claude/worktrees/r41_annette; BD=$WT/re15_port/build
git -C "$WT" show cefd57402:re15_port/engine/src/re15_damage.c > re15_damage_cefd57402.c
git -C "$WT" show HEAD:re15_port/engine/src/annette_r41.c > annette_r41_head.c
sed 's/^\(\s*\)re2_segment(a);\(.*\)$/\1\/* GP2 Abnahme 5b: re2_segment entfernt *\/\2/' annette_r41_head.c > annette_r41_ohne_umschaltung.c
diff annette_r41_head.c annette_r41_ohne_umschaltung.c > gp2.diff
grep -c "case 0x4d: r = 350" re15_damage_cefd57402.c
DEF="-DRE15_ASSETS_PATH=$WT/re15_port/shared_assets/PSX -DRE15_PLATFORM_PC=1 -std=c11 -I$WT/re15_port/include -I$WT/re15_port/engine/src"
gcc $DEF -c re15_damage_cefd57402.c -o dmg_gp1.o || exit 3
gcc $DEF -c annette_r41_ohne_umschaltung.c -o an_gp2.o || exit 3
LIBS="$BD/engine/libre15_engine.a $BD/tests/libre15_test_support.a -lm -lkernel32 -luser32 -lgdi32 -lwinspool -lshell32 -lole32 -loleaut32 -luuid -lcomdlg32 -ladvapi32"
T=$BD/tests/unit/CMakeFiles/test_r41_annette.dir/test_r41_annette.c.obj
gcc -o test_basis.exe $T $LIBS && gcc -o test_gp1.exe $T dmg_gp1.o $LIBS && gcc -o test_gp2.exe $T an_gp2.o $LIBS || exit 4
for t in basis gp1 gp2; do timeout 300 ./test_$t.exe > out_$t.txt 2>&1; echo "$t exit=$?"; done
