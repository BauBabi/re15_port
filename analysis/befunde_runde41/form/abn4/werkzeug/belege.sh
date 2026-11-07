#!/usr/bin/env bash
# Abnahme 4 (eigenes Werkzeug): Belege aus dem Scratchpad in abn4/ uebernehmen - je Lauf die kleinen Logs (deck.log, kam.log, env.txt,
# exe_sha.txt, lauf_ende.txt, mass_*.txt, stderr.log, re2se.log, birkin_dbg.log), Kontaktblaetter als JPG (die *.png-Regel der
# form/.gitignore), Hashes der exe/Assets/RDTs, Bauprotokolle. Idempotent.
set -uo pipefail
S=/c/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/formnb4
A="$(cd "$(dirname "$0")/.." && pwd)"
W="$(cd "$A/../../../.." && pwd)"
for d in "$S"/abn4e/laeufe/*/; do
  n=$(basename "$d"); mkdir -p "$A/laeufe/$n"
  for f in deck.log kam.log env.txt exe_sha.txt lauf_ende.txt stderr.log re2se.log birkin_dbg.log pid.txt; do [ -f "$d/$f" ] && cp -p "$d/$f" "$A/laeufe/$n/"; done
  cp -p "$d"/mass_*.txt "$A/laeufe/$n/" 2>/dev/null
done
/c/Python310/python.exe - "$A/blatt" "$S/abn4e/blatt_png" <<'PY'
import sys, os, glob
from PIL import Image
src, keep = sys.argv[1], sys.argv[2]
os.makedirs(keep, exist_ok=True)
for p in sorted(glob.glob(os.path.join(src, '*.png'))):
    j = p[:-4] + '.jpg'
    Image.open(p).convert('RGB').save(j, quality=82, optimize=True)
    os.replace(p, os.path.join(keep, os.path.basename(p)))
    print(os.path.basename(j), os.path.getsize(j))
PY
{ echo "# Abnahme 4: Hashes (sha256), selbst gerechnet"; sha256sum "$S/abn4e/exe/re15_pc_formabn3.exe" "$S/abn4e/exe/re15_pc_formabn3_vorher.exe" "$W/re15_port/build/platform/pc/re15_pc.exe" \
  "$W/re15_port/shared_assets/RE2/BAHNSTEIG_FORM.EMD" "$W/re15_port/shared_assets/RE2/BAHNSTEIG_FORM.TIM" "$S/abn4e/re2priv/BAHNSTEIG_FORM.EMD" "$S/abn4e/re2priv/BAHNSTEIG_FORM.TIM" \
  "$W/re15_port/shared_assets/PSX/STAGE5/ROOM5090.RDT" "$W/re15_port/shared_assets/PSX/STAGE5/ROOM5091.RDT" "$W/info/Re1.5/PSX/STAGE5/ROOM5090.RDT" "$W/info/Re1.5/PSX/STAGE5/ROOM5091.RDT" 2>/dev/null; echo "# exe je Lauf:"; for d in "$S"/abn4e/laeufe/*/; do echo "$(basename "$d") $(cut -c1-16 "$d/exe_sha.txt")"; done; } > "$A/hashes.txt"
cp -p "$S/build_configure.txt" "$S/build_build.txt" "$S/exe_vor_bau.txt" "$A/" 2>/dev/null
echo "belege ok: $(ls "$A/laeufe" | wc -l) Eintraege, $(ls "$A/blatt" | wc -l) Blaetter"
