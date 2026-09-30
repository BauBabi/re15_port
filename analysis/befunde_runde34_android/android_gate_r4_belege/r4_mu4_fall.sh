#!/usr/bin/env bash
# Runde 4 (Kette B2): faengt der FALL 226 (Manifest 64 MiB + 1 B ohne Pruefhaken) MU4 auch allein? Im Mutanten MU4
# endet der Selbsttest schon an den inneren Proben (_manifest_grenze). Hier: MU4 + innere Proben stillgelegt
# ('return falsch' am Ende von _innere_proben -> 'return []') - dann laufen die Faelle, und 226 muss rot werden.
# Kontrolle: nur die inneren Proben stillgelegt (ohne MU4) -> Selbsttest OK. Nur /c/Python310/python.
cd C:/workspace/git/reAi_v2/.claude/worktrees/r34a_android || exit 9
W=build/r34a/r4; PY=/c/Python310/python; M=$W/mutanten; L=$W/logs; mkdir -p "$M/MU4_ohne_innen" "$M/K_ohne_innen"
"$PY" - release/apk_asset_gate.py "$M" <<'PY'
import sys, os
t = open(sys.argv[1], encoding="utf-8", newline="").read()
innen_alt = "            os.environ[\"RE15_GATE_MANIFEST_MAX\"] = haken_vorher\n    return falsch\n"
innen_neu = "            os.environ[\"RE15_GATE_MANIFEST_MAX\"] = haken_vorher\n    return []\n"
mu4_alt, mu4_neu = "MANIFEST_MAX = 64 << 20", "MANIFEST_MAX = 64 << 30"
assert t.count(innen_alt) == 1 and t.count(mu4_alt) == 1
k = t.replace(innen_alt, innen_neu)
open(os.path.join(sys.argv[2], "K_ohne_innen", "apk_asset_gate.py"), "w", encoding="utf-8", newline="").write(k)
open(os.path.join(sys.argv[2], "MU4_ohne_innen", "apk_asset_gate.py"), "w", encoding="utf-8", newline="").write(k.replace(mu4_alt, mu4_neu))
print("geschrieben: K_ohne_innen, MU4_ohne_innen")
PY
for n in K_ohne_innen MU4_ohne_innen; do
    rc=0; "$PY" "$M/$n/apk_asset_gate.py" --selbsttest > "$L/selbsttest_$n.log" 2>&1 || rc=$?
    echo "$n rc=$rc | $(grep -a -E '^== SELBSTTEST' "$L/selbsttest_$n.log" | tr -d '\r')"
    grep -a -E '^   \[FEHLER\]' "$L/selbsttest_$n.log" | tr -d '\r' | cut -c1-150 | sed 's/^/   /'
done
echo FERTIG
