#!/usr/bin/env bash
# NB3 Bauer: Auswertung der finalen exe-Laeufe (final_runs/) mit den C-Auswertern obenlog/haenglog (= Riegel) und den Messer-Skripten oben3.py/haeng3.py (Gegenprobe).
SP="C:/workspace/git/reAi_v2/.claude/worktrees/r41_gatorfix"
S="C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/g2nb3"
T="$SP/re15_port/build/tests/unit/test_r41_gatorfix.exe"
W="$S/werkzeug"
AUS="$SP/analysis/befunde_runde41/G2_belege/nb3/bau/exe_final.txt"
export PATH="/c/msys64/mingw64/bin:$PATH" SDL_ASSERT=always_ignore
{
echo "# NB3 Bauer - exe-Laeufe mit der finalen exe (md5 je Lauf), Auswertung $(date +%F' '%T)"
echo -e "# Laeufe: final_bau.sh (Eingaben = exe-Riegel test_r41_gatorfix.cmake rampe / abn2_wand_elza / rampe_oben; *_alt = exe a661eb8aa d872c8e6 mit derselben Eingabe);
# kante2_* = kante2_bau.sh (Cut-2-Lage west4_leon 4200,-18800,3072,2 mit der rampe-Eingabe, Leon ohne Kanister) fuer die Bildleiste nb3_bau_suedkante_cut2_leon"
for r in oben_leon_ohne oben_leon_neu oben_elza_ohne oben_elza_neu oben_leon_ohne_alt; do
  d="$S/final_runs/runs/$r"
  echo; echo "== $r: $(cat "$d/pid.txt" | tr '\n' ' ') exe $(cut -c1-8 "$d/exe_md5.txt"); Kanister: $(grep -m1 -o 'KANISTER angehaengt.*' "$d/gator_boss.log" | sed 's/lokal=([-0-9,]*) //')"
  grep -c "sub=9" "$d/climb.log" | sed 's/^/   climb.log sub=9 Zeilen: /'
  m=""; case "$r" in *_neu) m=mit;; esac
  "$T" obenlog "$d" 2>/dev/null | grep -E "^obenlog|^  oben|^    Treffer" | cut -c1-400
  echo "   -- oben3.py (Messer-Skript, ohne Kanister-Regel):"; python -I "$W/oben3.py" "$d" | grep -E "^  oben" | cut -c1-200
done
for r in rampe_leon_ohne rampe_leon_neu rampe_elza_ohne wand_elza_ohne wand_elza_neu rampe_leon_ohne_alt kante2_leon_ohne kante2_leon_ohne_alt; do
  d="$S/final_runs/runs/$r"
  echo; echo "== $r: $(cat "$d/pid.txt" | tr '\n' ' ') exe $(cut -c1-8 "$d/exe_md5.txt")"
  m=""; case "$r" in *_neu) m=mit;; esac
  "$T" haenglog "$d" $m 2>/dev/null | grep -E "^haenglog|Haenge-Fenster F" | cut -c1-400
  echo "   -- haeng3.py (Messer-Skript):"; python -I "$W/haeng3.py" "$d" | grep -E "laengste Zeit|Haenge-Fenster \(" | cut -c1-200
  echo "   -- Treffer (hp<100 Flanken) im Rueckkehr-Besuch: $(python -I "$W/haeng3.py" "$d" | grep -o 'Treffer (hp<100) [0-9]*')"
done
} | tee "$AUS"
echo "geschrieben $AUS"
