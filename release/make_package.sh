#!/usr/bin/env bash
# =============================================================================
# RE1.5 Port — Release-Pakete schnueren (Linux/Steam Deck + Windows)
# =============================================================================
# EINZIGE Quelle der Wahrheit fuer den Paketinhalt. Vorher wurden pkg-linux/
# und pkg-win/ von Hand zusammengestellt; die Ordner sind gitignoriert, also
# ging jede Korrektur am Startskript/Inhalt beim naechsten Release verloren.
# Genau so entstand das v0.1.1-Deck-Paket mit drei Defekten:
#   * kein shared_assets/extracted_fx  -> Blut/Muendungsfeuer/Rauch/Huelsen fehlten
#   * run.sh mit 53 Bytes (nur cd+exec) -> weisse Fades/Balken auf Mesa,
#                                          Effekt-Texturen unauffindbar
#   * Binary aus ubuntu:22.04           -> verlangte GLIBC_2.34
# Die Pruefungen unten (Abschnitt "Gates") lassen jeden dieser Faelle das
# Paketieren ABBRECHEN, statt ein kaputtes Paket auszuliefern.
#
# Aufruf:
#   release/make_package.sh --version v0.1.2                 # beide Plattformen
#   release/make_package.sh --version v0.1.2 --only linux    # nur Linux/Deck
#   release/make_package.sh --version v0.1.2 --no-zip        # nur Ordner bauen
#
# Eingaben (Binaries, werden NICHT hier gebaut):
#   Linux : release/linux_out/re15_pc    <- release/build_linux_deck.sh
#   Windows: release/win_out/re15_pc.exe <- mingw64-Build, siehe RELEASE_NOTES
# =============================================================================
set -euo pipefail

HERE="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
REPO="$(cd "$HERE/.." && pwd)"

VERSION="v0.1.2"
ONLY="both"
DO_ZIP=1
ZIP_ONLY=0                        # nur zippen, vorhandene pkg-*/ wiederverwenden
SPLIT="90m"                       # < 100 MB je Volume (GitHub-Dateigrenze)
LINUX_BIN="$HERE/linux_out/re15_pc"
WIN_BIN="$HERE/win_out/re15_pc.exe"

while [[ $# -gt 0 ]]; do
    case "$1" in
        --version)   VERSION="$2"; shift 2 ;;
        --only)      ONLY="$2";    shift 2 ;;
        --linux-bin) LINUX_BIN="$2"; shift 2 ;;
        --win-bin)   WIN_BIN="$2";   shift 2 ;;
        --no-zip)    DO_ZIP=0; shift ;;
        --zip-only)  ZIP_ONLY=1; shift ;;
        -h|--help)   sed -n '2,25p' "${BASH_SOURCE[0]}"; exit 0 ;;
        *) echo "unbekannte Option: $1" >&2; exit 2 ;;
    esac
done

ASSETS="$REPO/re15_port/shared_assets/PSX"
FX="$REPO/re15_port/shared_assets/extracted_fx"
NAME="re15_port_${VERSION}"

# --- Gates ------------------------------------------------------------------
# Die vier Texturen, die platform/pc/main.c beim Start aus extracted_fx laedt
# (main.c, Tabelle "extracted_fx/effect*.tim"). Fehlt eine, rendert der
# zugehoerige Effekt nicht.
FX_REQUIRED=(effect0_blood.tim effect2_muzzle.tim effect3_smoke.tim effect4_shell.tim)

die() { echo "ABBRUCH: $*" >&2; exit 1; }

check_binary_paths() {   # $1 = Binary
    local bin="$1"
    # Der BSS-Lader muss den GROSS geschriebenen Baumnamen benutzen
    # (platform/pc/src/bg_pc.c, "BSS/ROOM%04X/BG%02d.BSS"). Ein Binary mit der
    # alten Kleinschreibung findet auf case-sensitiven Dateisystemen (ext4,
    # also SteamOS) KEINEN Raumhintergrund -> alles schwarz.
    if grep -aq 'BSS/%s/BG%02d.BSS' "$bin"; then
        die "$bin ist VERALTET: enthaelt den kleingeschriebenen BSS-Pfad
        ('BSS/%s/BG%02d.BSS'). Auf case-sensitiven Dateisystemen bleiben alle
        Raumhintergruende schwarz. Neu bauen (bg_pc.c-Fix im Baum?)."
    fi
    grep -aq 'BSS/ROOM%04X/BG%02d.BSS' "$bin" \
        || die "$bin enthaelt den erwarteten BSS-Pfad nicht — kein RE1.5-Binary?"
}

check_glibc() {          # $1 = Linux-Binary
    command -v objdump >/dev/null 2>&1 || { echo "   (objdump fehlt — glibc-Gate uebersprungen)"; return; }
    local max
    max="$(objdump -T "$1" 2>/dev/null | grep -oE 'GLIBC_[0-9.]+' | sort -uV | tail -1)"
    [[ -n "$max" ]] || return
    echo "   hoechste glibc-Anforderung: $max"
    # SteamOS 3.4 liefert glibc 2.33; die Steam-Runtime-3.0-Basis ("sniper",
    # Debian 11) hat 2.31. Alles darueber schliesst Geraete aus.
    local n="${max#GLIBC_}"
    if [[ "$(printf '%s\n2.31\n' "$n" | sort -V | tail -1)" != "2.31" ]]; then
        die "Binary verlangt $max > GLIBC_2.31. Es wurde gegen ein zu neues
        System gebaut (ubuntu:22.04 o.ae.). Mit release/build_linux_deck.sh auf
        Debian-11-/sniper-Basis neu bauen."
    fi
}

check_tree() {           # $1 = fertiger Paketordner
    local out="$1"
    [[ -d "$out/shared_assets/PSX/STAGE1" ]] || die "Asset-Baum unvollstaendig in $out"
    for f in "${FX_REQUIRED[@]}"; do
        [[ -f "$out/shared_assets/extracted_fx/$f" ]] \
            || die "Effekt-Textur fehlt im Paket: shared_assets/extracted_fx/$f"
    done
}

# --- Gemeinsames Einsammeln --------------------------------------------------
[[ -d "$ASSETS" ]] || die "Asset-Baum fehlt: $ASSETS"
[[ -d "$FX"     ]] || die "Effekt-Texturen fehlen: $FX"

copy_common() {          # $1 = Paketordner
    local out="$1"
    echo "   Assets kopieren (shared_assets/PSX, ~283 MB) ..."
    mkdir -p "$out/shared_assets"
    cp -r "$ASSETS" "$out/shared_assets/PSX"
    echo "   Effekt-Texturen kopieren (shared_assets/extracted_fx) ..."
    cp -r "$FX" "$out/shared_assets/extracted_fx"
}

render_readme() {        # $1 = Vorlage, $2 = Ziel
    sed "s/@VERSION@/${VERSION#v}/g" "$1" > "$2"
}

# Split-Zips lassen sich NICHT mit `zip -T` pruefen ("cannot update a split
# archive") und `unzip -t` bricht auf den Volumes ab. Geprueft wird deshalb der
# zentrale Katalog des LETZTEN Volumes: er listet alle Eintraege des gesamten
# Satzes und nennt die Volume-Nummer jedes Eintrags — daran haengt, ob der Satz
# vollstaendig ist.
verify_split() {         # $1 = .zip (letztes Volume), $2 = erwartete Dateizahl
    python3 - "$1" "$2" <<'PY'
import struct, sys, glob, os
last, want = sys.argv[1], int(sys.argv[2])
d = open(last, 'rb').read()
i = d.rfind(b'PK\x05\x06')
if i < 0: sys.exit("kein End-of-Central-Directory gefunden")
disk, cd_disk, here, total = struct.unpack('<HHHH', d[i+4:i+12])
vols = sorted(glob.glob(os.path.splitext(last)[0] + '.z*'))
print(f"   Volumes: {len(vols)} (letztes = Nr. {disk+1}), Eintraege im Katalog: {total}")
missing = [n for n in range(1, disk+1)
           if not os.path.exists(f"{os.path.splitext(last)[0]}.z{n:02d}")]
if missing: sys.exit(f"fehlende Volumes: {missing}")
if total < want: sys.exit(f"Katalog listet nur {total} Eintraege, erwartet >= {want}")
PY
}

# --- Linux / Steam Deck ------------------------------------------------------
if [[ "$ONLY" == "both" || "$ONLY" == "linux" ]]; then
    [[ -f "$LINUX_BIN" ]] || die "Linux-Binary fehlt: $LINUX_BIN (release/build_linux_deck.sh)"
    echo "== Linux/Steam-Deck-Paket: $NAME =="
    check_binary_paths "$LINUX_BIN"
    check_glibc        "$LINUX_BIN"

    OUT="$HERE/pkg-linux/$NAME"
    if [[ $ZIP_ONLY -eq 0 ]]; then
        rm -rf "$HERE/pkg-linux"; mkdir -p "$OUT"
        install -m 755 "$LINUX_BIN"            "$OUT/re15_pc"
        install -m 755 "$HERE/pkg_files/linux/run.sh" "$OUT/run.sh"
        render_readme "$HERE/pkg_files/linux/README.txt.in" "$OUT/README_${VERSION#v}.txt"
        copy_common "$OUT"
    fi
    check_tree  "$OUT"
    LINUX_FILES=$(find "$OUT" -type f | wc -l)
    echo "   OK: $(du -sh "$OUT" | cut -f1), $LINUX_FILES Dateien"
fi

# --- Windows -----------------------------------------------------------------
if [[ "$ONLY" == "both" || "$ONLY" == "win" ]]; then
    [[ -f "$WIN_BIN" ]] || die "Windows-Binary fehlt: $WIN_BIN"
    echo "== Windows-Paket: $NAME =="
    check_binary_paths "$WIN_BIN"

    OUT="$HERE/pkg-win/$NAME"
    if [[ $ZIP_ONLY -eq 0 ]]; then
        rm -rf "$HERE/pkg-win"; mkdir -p "$OUT"
        install -m 755 "$WIN_BIN"                          "$OUT/re15_pc.exe"
        install -m 644 "$HERE/pkg_files/win/Start_RE15_Port.bat" "$OUT/Start_RE15_Port.bat"
        render_readme "$HERE/pkg_files/win/README.txt.in"   "$OUT/README_${VERSION#v}.txt"
        copy_common "$OUT"
    fi
    check_tree  "$OUT"
    WIN_FILES=$(find "$OUT" -type f | wc -l)
    echo "   OK: $(du -sh "$OUT" | cut -f1), $WIN_FILES Dateien"
fi

# --- Zippen (Split-Volumes) --------------------------------------------------
if [[ $DO_ZIP -eq 1 ]]; then
    command -v zip >/dev/null 2>&1 || die "zip fehlt"
    cd "$HERE"
    if [[ "$ONLY" == "both" || "$ONLY" == "linux" ]]; then
        echo "== Zippen: ${NAME}_linux_steamdeck_x64 =="
        rm -f "${NAME}_linux_steamdeck_x64".z*
        ( cd pkg-linux && zip -q -s "$SPLIT" -r "../${NAME}_linux_steamdeck_x64.zip" "$NAME" )
        verify_split "${NAME}_linux_steamdeck_x64.zip" "$LINUX_FILES"
    fi
    if [[ "$ONLY" == "both" || "$ONLY" == "win" ]]; then
        echo "== Zippen: ${NAME}_win64 =="
        rm -f "${NAME}_win64".z*
        ( cd pkg-win && zip -q -s "$SPLIT" -r "../${NAME}_win64.zip" "$NAME" )
        verify_split "${NAME}_win64.zip" "$WIN_FILES"
    fi
    sha256sum "${NAME}"_*.z* > SHA256SUMS.txt
    echo
    ls -la "${NAME}"_*.z*
    echo "== SHA256SUMS.txt geschrieben =="
fi

echo "== Fertig =="
