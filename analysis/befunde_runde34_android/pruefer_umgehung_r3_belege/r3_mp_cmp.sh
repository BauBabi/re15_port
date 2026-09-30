#!/usr/bin/env bash
# Gegenpruefung R3: cmp-Gates in make_package.sh check_tree (TORSE.VBS, RE2/DOOR/*.DO2, RE15DOOR/*.DO2) im ECHTEN
# Lauf (Sandbox sb_mp, --zip-only nimmt den vorhandenen Paketordner). Je Fall 1 Byte in der PAKET-Kopie (Linkzahl 1
# geprueft), danach zurueck. Rueckgabe je Lauf selbst abgefangen.
cd C:/workspace/git/reAi_v2/.claude/worktrees/r34a_android || exit 9
W=build/r34a/pruefer_r3; S=$W/sb_mp; L=$W/logs/mp4; mkdir -p "$L"; PY=/c/Python310/python
PK="$S/release/pkg-linux/re15_port_v0.8.19"
cmp -s release/apk_asset_gate.py "$S/release/apk_asset_gate.py" || { echo "Sandbox-Gate != Arbeitsbaum"; exit 1; }
fall() {    # $1 = Tag, $2 = Pfad im Paket
    local p="$PK/$2" rc=0
    [[ "$(stat -c %h "$p")" == 1 ]] || { echo "$p ist ein Link - Abbruch"; exit 1; }
    cp -p "$p" "$L/$1.orig"
    "$PY" -c "import sys; p=sys.argv[1]; d=bytearray(open(p,'rb').read()); d[-1]^=0x01; open(p,'wb').write(d)" "$p"
    bash "$S/release/make_package.sh" --version v0.8.19 --only linux --zip-only > "$L/$1.log" 2>&1 || rc=$?
    cp -p "$L/$1.orig" "$p"
    printf '%-22s EXIT=%s | %s\n' "$1" "$rc" "$(grep -a -m1 '^ABBRUCH' "$L/$1.log" | tr -d '\r' | cut -c1-150)"
}
fall torse_vbs   shared_assets/RE2/TORSE.VBS
fall re2_door04  shared_assets/RE2/DOOR/DOOR04.DO2
fall re15_p07g   shared_assets/RE15DOOR/P07G.DO2
echo FERTIG
