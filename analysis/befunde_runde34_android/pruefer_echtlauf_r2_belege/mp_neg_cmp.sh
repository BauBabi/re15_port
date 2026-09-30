#!/usr/bin/env bash
# Pruefer echtlauf r2: die neuen cmp-Gates (check_tree: RE2/DOOR je Datei + TORSE.VBS) im ECHTEN Fluss.
# Nach dem gruenen Vollauf liegen release/pkg-linux + pkg-win. Je Fall EIN Byte (gleiche Groesse) in einer
# Paketdatei kippen, make_package.sh --zip-only (verwendet die Paketordner, kopiert nicht neu) -> muss mit
# der passenden Meldung abbrechen; danach die Datei aus dem Quellbaum zurueck (cmp gleich).
set -u
BAUM=C:/workspace/git/reAi_v2/.claude/worktrees/r34a_android
BEL=$BAUM/analysis/befunde_runde34_android/pruefer_echtlauf_r2_belege
OUTD=$BAUM/build/r34a/pruefer_echtlauf_r2
cd "$BAUM" || exit 99
kippen() { /c/Python310/python - "$1" <<'PY'
import sys
p = sys.argv[1]
with open(p, "r+b") as f:
    f.seek(100); b = f.read(1); f.seek(100); f.write(bytes([b[0] ^ 0x01]))
print("gekippt: Byte 100 in", p)
PY
}
fall() {   # $1 = Kennung, $2 = Paketdatei (relativ zu release/), $3 = Quelle, $4 = erwartete Meldung
    local id="$1" ziel="release/$2" quelle="$3" soll="$4" rc
    kippen "$ziel"
    cmp -s "$quelle" "$ziel" && { echo "$id: Kippen wirkungslos?!"; return 1; }
    LOG=$OUTD/mp_neg_$id.log bash "$BEL/mp_isoliert.sh" --version v0.8.19 --zip-only; rc=$?
    cp -f "$quelle" "$ziel"
    cmp -s "$quelle" "$ziel" && wieder=gleich || wieder=UNGLEICH
    if grep -qF "$soll" "$OUTD/mp_neg_$id.log"; then treffer=ja; else treffer=NEIN; fi
    echo "$id: EXIT=$rc  Meldung-gefunden=$treffer  zurueck=$wieder  ($(grep -m1 'ABBRUCH' "$OUTD/mp_neg_$id.log" | cut -d' ' -f2- | cut -c1-150))"
}
R=re15_port/shared_assets/RE2
fall N1_linux_door04 pkg-linux/re15_port_v0.8.19/shared_assets/RE2/DOOR/DOOR04.DO2 $R/DOOR/DOOR04.DO2 \
     "RE2-Tuerarchiv im Paket weicht vom Quellbaum ab: shared_assets/RE2/DOOR/DOOR04.DO2"
fall N2_linux_torse pkg-linux/re15_port_v0.8.19/shared_assets/RE2/TORSE.VBS $R/TORSE.VBS \
     "RE2-Asset im Paket weicht vom Quellbaum ab: shared_assets/RE2/TORSE.VBS"
fall N3_win_door36 pkg-win/re15_port_v0.8.19/shared_assets/RE2/DOOR/DOOR36.DO2 $R/DOOR/DOOR36.DO2 \
     "RE2-Tuerarchiv im Paket weicht vom Quellbaum ab: shared_assets/RE2/DOOR/DOOR36.DO2"
fall N4_win_torse pkg-win/re15_port_v0.8.19/shared_assets/RE2/TORSE.VBS $R/TORSE.VBS \
     "RE2-Asset im Paket weicht vom Quellbaum ab: shared_assets/RE2/TORSE.VBS"
