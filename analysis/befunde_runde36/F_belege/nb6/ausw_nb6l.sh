#!/bin/bash
# ausw_nb6g.sh <variante> : 14 Stellen (ausw5 + Summen), Schuss-Laeufe, cmp gegen Abnahme 5 (abn5f_neu) und Vorgaenger (nb6b)
S="C:/Users/MJOEDI~1/AppData/Local/Temp/claude/c--workspace-git-reAi-v2/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad"
W="C:/workspace/git/reAi_v2/.claude/worktrees/r36_zombieki"
v="$1"; cd "$S/F_$v" || exit 1
P="-7000,12500 -14000,2000 6000,-9000 -24000,10000 7000,4000 9000,10000 -14746,-4140 7200,-4130 -14000,4000 -14500,6000 -24452,11938 -22288,11938 -26500,15500 -20000,14500"
echo "## 14 Stellen ($v), Auswerter ausw5.py der Abnahme 5"
for p in $P; do python -I "$W/analysis/befunde_runde36/F_belege/nb6/ausw5.py" "affen_${p}_$v"; done
echo "## Summen 14 Stellen"
python -I - "$W/analysis/befunde_runde36/F_belege/nb6/ausw5.py" "$v" $P <<'PY'
import sys, importlib.util, os
spec=importlib.util.spec_from_file_location("a5", sys.argv[1]); a5=importlib.util.module_from_spec(spec); spec.loader.exec_module(a5)
v=sys.argv[2]; fest=fern=0; rows=[]
for p in sys.argv[3:]:
    r,f0,f1=a5.ausw(os.path.join("affen_%s_%s"%(p,v),"gy.log"))
    s2,s3=r[2],r[3]; fest+=s2["fest"]+s3["fest"]; fern+=s2["fern"]+s3["fern"]
    rows.append("| (%s) | %s / %s | %d / %d (FERN %d / %d) |"%(p,"F%d"%s2["erst"] if s2["erst"] else "nie","F%d"%s3["erst"] if s3["erst"] else "nie",s2["nah"],s3["nah"],s2["fern"],s3["fern"]))
print("| Leon | erster Angriff S2 / S3 | NAH fest S2 / S3 (FERN) |"); print("|---|---|---|")
for r in rows: print(r)
print("Summe fest %d (FERN %d / NAH %d)"%(fest,fern,fest-fern))
PY
echo "## Schuss-Laeufe + Tuerwege ($v)"
for t in nw12_schuss nw4_schuss nw11_schuss nw13_schuss stelle3_schuss t11b0kampf_schuss t11b0szene_schuss t11b0kampf t11b0szene szene11c0 flucht_west; do python -I "$W/analysis/befunde_runde36/F_belege/nb6/ausw5.py" "${t}_$v"; done
echo "## cmp gegen Abnahme 5 (abn5f_neu) / Stand NB6c (nb6h)"
for t in szene11c0 t11b0szene t11b0kampf flucht_west p3lache r1040w r10b0 tuer1010 tuer1040 t1220a t1220c "affen_-24000,10000" "affen_-24452,11938" "affen_-22288,11938" "affen_-26500,15500" "affen_-7000,12500" "affen_-14000,2000" "affen_7000,4000" "affen_9000,10000" "affen_-14746,-4140" "affen_-14000,4000" "affen_-14500,6000" "affen_-20000,14500" "affen_6000,-9000" "affen_7200,-4130" nw12_schuss nw4_schuss nw11_schuss nw13_schuss stelle3_schuss t11b0kampf_schuss t11b0szene_schuss; do
  for f in gy.log st.log; do
    [ -f "${t}_$v/$f" ] || continue
    a="?"; b="?"
    [ -f "$S/F_abn5/${t}_abn5f_neu/$f" ] && { cmp -s "${t}_$v/$f" "$S/F_abn5/${t}_abn5f_neu/$f" && a="GLEICH" || a="anders"; }
    [ -f "$S/F_nb6h/${t}_nb6h/$f" ] && { cmp -s "${t}_$v/$f" "$S/F_nb6h/${t}_nb6h/$f" && b="GLEICH" || b="anders"; }
    echo "$t $f: Abnahme5 $a | nb6h $b"
  done
done
echo "## p3lache Framedumps gegen Abnahme 5"
n=0; g=0; for f in "p3lache_$v"/fd_*.ppm; do [ -f "$f" ] || continue; n=$((n+1)); cmp -s "$f" "$S/F_abn5/p3lache_abn5f_neu/$(basename "$f")" && g=$((g+1)); done; echo "Framedumps $n, bitgleich $g"
echo "## r1010a R1010-Teil gegen Abnahme 5"
R="$2"; [ -n "$R" ] && [ -f "$R/gy.log" ] && { grep " R1010 " "$R/gy.log" > /tmp/r1010_$v.txt; grep " R1010 " "$S/F_abn5/r1010a_abn5f_neu/gy.log" > /tmp/r1010_abn5.txt; echo "r1010a aus $R: R1010-Zeilen $(wc -l < /tmp/r1010_$v.txt) (Abnahme 5: $(wc -l < /tmp/r1010_abn5.txt))"; cmp -s /tmp/r1010_$v.txt /tmp/r1010_abn5.txt && echo "R1010-Teil GLEICH" || echo "R1010-Teil anders"; }
