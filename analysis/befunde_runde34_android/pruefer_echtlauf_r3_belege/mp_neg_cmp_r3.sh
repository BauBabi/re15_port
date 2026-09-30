#!/usr/bin/env bash
# Pruefer echtlauf r3: die cmp-Gates fuer RE2/DOOR und TORSE.VBS im ECHTEN Fluss (make_package.sh --zip-only auf die
# Paketordner des positiven Laufs, git-Schreibzugriffe isoliert wie mp_isoliert_r3.sh). Je Fall EIN Byte in einer
# Paketdatei gekippt (Groesse gleich), danach aus dem Quellbaum zurueck (cmp gleich). Andere Dateien als in r2 (dort
# DOOR04/DOOR36): hier RE2/DOOR/DOOR13.DO2 (Linux) und RE2/TORSE.VBS (Windows).
set -u
BAUM=C:/workspace/git/reAi_v2/.claude/worktrees/r34a_android
W=$BAUM/build/r34a/pruefer_echtlauf_r3
B=$BAUM/analysis/befunde_runde34_android/pruefer_echtlauf_r3_belege
cd "$BAUM" || exit 99
kippen() { /c/Python310/python -c "import sys; p=sys.argv[1]; b=bytearray(open(p,'rb').read()); b[int(sys.argv[2])]^=0x01; open(p,'wb').write(b)" "$1" "$2"; }
fall() {   # $1 = Kennung, $2 = Plattform (linux|win), $3 = Paketdatei relativ zum Paketordner, $4 = Offset
    local kenn="$1" pl="$2" rel="$3" off="$4" pkg
    if [[ $pl == linux ]]; then pkg=release/pkg-linux/re15_port_v0.8.19; else pkg=release/pkg-win/re15_port_v0.8.19; fi
    local q="re15_port/$rel"; [[ $rel == synchro/* ]] && q="$rel"
    cmp -s "$q" "$pkg/$rel" || { echo "$kenn: Ausgangslage nicht gleich" ; return 1; }
    kippen "$pkg/$rel" "$off"
    echo "$kenn: gekippt $pkg/$rel @ $off -> cmp $(cmp -s "$q" "$pkg/$rel"; echo $?) ; Groesse $(stat -c %s "$pkg/$rel")/$(stat -c %s "$q")"
    LOG=$W/mp_neg_${kenn}.log bash $B/mp_isoliert_r3.sh --version v0.8.19 --zip-only --only "$pl"
    echo "$kenn: EXIT=$? ; Abbruchzeile: $(grep -m1 'ABBRUCH' $W/mp_neg_${kenn}.log | cut -d' ' -f2- )"
    echo "$kenn: gezippt? $(grep -c '== Zippen' $W/mp_neg_${kenn}.log) Zeilen '== Zippen'; APK-Kette vorher: $(grep -c 'APK-PRUEFUNG-OK' $W/mp_neg_${kenn}.log) x OK"
    cp -p "$q" "$pkg/$rel"
    echo "$kenn: zurueck -> cmp $(cmp -s "$q" "$pkg/$rel"; echo $?)"
}
fall N1_linux_door13 linux shared_assets/RE2/DOOR/DOOR13.DO2 1500
fall N2_win_torse    win   shared_assets/RE2/TORSE.VBS 9000
