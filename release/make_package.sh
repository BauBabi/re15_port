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
RE2="$REPO/re15_port/shared_assets/RE2"
SYNCHRO="$REPO/synchro"
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

check_binary_fresh() {   # $1 = Binary, $2 = Label
    # ⛔ v0.3.9-UNFALL (2026-08-21): Das Skript BAUT NICHT, es KOPIERT aus win_out/
    # bzw. linux_out/. Der Windows-Build lief nach release/wbuild/, win_out/ blieb
    # auf dem Stand von v0.3.8 — das ausgelieferte Paket enthielt KEINEN der fuenf
    # Fix-Commits (Beleg: 're15_climb' 0x im Paket-Binary, 10x im echten Build),
    # und der Nutzer hat vier bereits behobene Fehler erneut gemeldet.
    # Gate: das Binary muss NEUER sein als der letzte Commit, der Port-Code aendert.
    local bin="$1" label="$2" bin_t src_t
    command -v git >/dev/null 2>&1 || return
    src_t="$(git -C "$HERE/.." log -1 --format=%ct -- re15_port/engine re15_port/platform re15_port/include 2>/dev/null)"
    [[ -n "$src_t" ]] || return
    bin_t="$(stat -c %Y "$bin" 2>/dev/null || stat -f %m "$bin" 2>/dev/null)"
    [[ -n "$bin_t" ]] || return
    if (( bin_t < src_t )); then
        die "$label ist VERALTET: $bin
        stammt von $(date -d "@$bin_t" '+%F %T' 2>/dev/null || date -r "$bin_t" '+%F %T'),
        der letzte Port-Code-Commit von $(date -d "@$src_t" '+%F %T' 2>/dev/null || date -r "$src_t" '+%F %T').
        Dieses Skript baut NICHT — es kopiert nur. Erst neu bauen und das Ergebnis
        nach $(dirname "$bin")/ kopieren, dann das Paket erzeugen."
    fi
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

check_lf() {             # $1 = fertiger Paketordner — nur fuer Linux-Pakete sinnvoll
    # Ein Shell-Skript mit CRLF ist auf dem Deck TOT: die Shell liest das \r als Teil des
    # Interpreter-Pfads ("/usr/bin/env bash\r: nicht gefunden") bzw. haengt es an jedes
    # Kommando. Gemeldet 2026-08-25 vom Nutzer, nachdem run.sh genau so ausgeliefert wurde.
    # Der Paketbau kopiert aus dem ARBEITSBAUM, wo `.gitattributes` (eol=lf) nicht greift —
    # deshalb hier am FERTIGEN Paket pruefen, nicht an der Quelle.
    local out="$1" bad=0 f
    while IFS= read -r f; do
        if LC_ALL=C grep -qU $'\r' "$f"; then
            echo "   CRLF in $f" >&2
            bad=1
        fi
    done < <(find "$out" -maxdepth 1 -type f \( -name '*.sh' -o -name '*.desktop' \))
    [[ $bad -eq 0 ]] || die "Shell-/Desktop-Datei mit CRLF im Linux-Paket — auf dem Deck
        scheitert das schon am Shebang. Quelle normalisieren (tr -d '\\r') und neu packen."
    echo "   Zeilenenden-Gate: alle Skripte im Paket sind LF"
}

check_tree() {           # $1 = fertiger Paketordner
    local out="$1"
    [[ -d "$out/shared_assets/PSX/STAGE1" ]] || die "Asset-Baum unvollstaendig in $out"
    for f in "${FX_REQUIRED[@]}"; do
        [[ -f "$out/shared_assets/extracted_fx/$f" ]] \
            || die "Effekt-Textur fehlt im Paket: shared_assets/extracted_fx/$f"
    done
    # Seit v0.2: OPTIONS->AI=RE2 laedt Gegner-Modelle/-Sounds aus shared_assets/RE2/
    # (platform/pc/main.c pc_re2_cdemd, audio_pc.c read_re2_enemse_vbs — beide ueber
    # re15_pc_read_re2(): env RE15_RE2_ASSET_ROOT, sonst <shared>/RE2/). Fehlen die
    # Dateien, faellt die Option still auf RE1.5 zurueck -> Gate statt Stille.
    for f in CDEMD0.EMS ENEMSE.VBS; do
        [[ -s "$out/shared_assets/RE2/$f" ]] \
            || die "RE2-Asset fehlt/leer im Paket: shared_assets/RE2/$f (RE2-AI-Option waere still tot)"
    done
    # Voiceover: der Port laedt NICHT aus shared_assets/PSX/VOICE, sondern aus
    # synchro/STAGE<n>/room<id>/main<nn>.wav (audio_pc.c re15_voice_load_clip).
    # Seit 2026-08-24 ueber die BASIS-Wurzelliste (asset_root_pc.c): synchro/ muss
    # neben der exe liegen. Bis v0.3.16 fehlte synchro/ in JEDEM Paket — auf dem
    # Dev-Rechner traf die alte ../../..-Probe das Repo, im ausgelieferten Paket
    # (Deck) blieb das Voiceover stumm.
    [[ -s "$out/synchro/STAGE1/room1170/main00.wav" ]] \
        || die "Voiceover fehlt im Paket: synchro/STAGE1/room1170/main00.wav"
    local want got
    want="$(find "$SYNCHRO/STAGE1" -name '*.wav' | wc -l)"
    got="$(find "$out/synchro/STAGE1" -name '*.wav' 2>/dev/null | wc -l)"
    (( got == want )) || die "Voiceover unvollstaendig im Paket: $got/$want WAVs unter synchro/STAGE1"
}

# =============================================================================
# check_runtime_assets — DAS LAUFZEIT-GATE (2026-08-24)
# =============================================================================
# $1 = fertiger Paketordner, $2 = Name der Binary darin ("re15_pc.exe"/"re15_pc")
#
# WARUM ES DAS GIBT — der 0.3.19-Fehler ging an ALLEN bisherigen Gates vorbei:
# check_tree prueft, ob Dateien im Paketordner LIEGEN. Sie lagen alle korrekt da.
# Der Fehler war, dass die exe sie zur LAUFZEIT nicht FAND: sie suchte am
# einkompilierten Pfad, der im Docker-Cross-Build auf den Container zeigt
# ("/src/re15_port/shared_assets/PSX") und beim Nutzer nicht existiert. Ergebnis
# beim Doppelklick: keine Spielschrift -> keine CONFIG-Labels, keine Untertitel,
# keine Dialoge, keine Item-Namen; dazu stumme Musik und fehlende Effekte.
# "Liegt da" und "wird gefunden" sind also ZWEI verschiedene Fragen — bisher hat
# das Gate nur die erste gestellt. Dieses hier stellt die zweite.
#
# ES REICHT NICHT, NUR "GEFUNDEN?" ZU FRAGEN. Auf der Entwickler-Maschine zeigt
# der einkompilierte Default INS REPO und rettet jedes noch so kaputte Paket.
# GEMESSEN 2026-08-24: Paketordner komplett ohne shared_assets/ -> die exe meldet
# trotzdem "RESULT ok=26 missing=0", weil alle 26 Treffer aus
# C:/workspace/git/reAi_v2/re15_port/shared_assets/ kamen. Ein Gate, das nur den
# Rueckgabewert prueft, waere dabei GRUEN gewesen.
# DESHALB ist die HERKUNFT die eigentliche Pruefung: jeder Treffer muss aus dem
# PAKETORDNER stammen. Genau das meldet der Selbsttest hinter "<-".
#
# ZWEI LAEUFE, weil es zwei verschiedene Nutzer-Situationen sind:
#   (1) cwd = Paketordner   -> der Doppelklick im Explorer
#   (2) cwd = fremdes Verzeichnis -> Verknuepfung, Steam, "Ausfuehren in ..."
# (2) ist der schaerfere Test: nur eine exe-relative Aufloesung besteht ihn.
#
# Die Umgebung wird GELEERT (env -u ...): das Paket muss aus eigener Kraft
# laufen. Sonst wuerde ausgerechnet die Variable, die den Fehler verdeckt hat,
# den Test bestehen lassen.
#
# Linux-Pakete werden nur geprueft, wenn das Binary hier ueberhaupt laufen kann
# (der Paketbau laeuft unter Windows/Git-Bash) — sonst uebersprungen mit Hinweis.
# =============================================================================
check_runtime_assets() {
    local out="$1" bin="$2"
    local exe="$out/$bin"
    [[ -x "$exe" || -f "$exe" ]] || die "Binary fehlt im Paket: $exe"

    # Kann diese Maschine das Binary starten? .exe nur unter Windows/MSYS/Cygwin,
    # ELF nur unter Linux. Kein Wunschdenken: lieber ehrlich ueberspringen.
    case "$(uname -s)" in
        MINGW*|MSYS*|CYGWIN*) [[ "$bin" == *.exe ]] || {
            echo "   (Laufzeit-Gate uebersprungen: $bin ist kein Windows-Binary)"; return; } ;;
        Linux)                [[ "$bin" != *.exe ]] || {
            echo "   (Laufzeit-Gate uebersprungen: $bin laeuft nicht unter Linux)"; return; } ;;
        *) echo "   (Laufzeit-Gate uebersprungen: unbekanntes Host-System)"; return ;;
    esac

    # Der Paketordner als absoluter, normalisierter Pfad — damit ist er mit den
    # Pfaden vergleichbar, die der Selbsttest meldet (der schreibt immer '/').
    local pkg_abs
    pkg_abs="$(cd "$out" && pwd)"
    case "$(uname -s)" in
        MINGW*|MSYS*|CYGWIN*) pkg_abs="$(cd "$out" && pwd -W 2>/dev/null || echo "$pkg_abs")" ;;
    esac
    pkg_abs="${pkg_abs%/}"

    local scratch run_dir log rc
    scratch="$(mktemp -d)"
    trap 'rm -rf "$scratch"' RETURN

    local pass
    for pass in in_pkg foreign_cwd; do
        if [[ "$pass" == in_pkg ]]; then
            run_dir="$out"                       # (1) Doppelklick: cwd = Paketordner
        else
            run_dir="$scratch/cwd"; mkdir -p "$run_dir"   # (2) Verknuepfung/Steam
        fi
        log="$run_dir/debug.log"
        rm -f "$log"

        # Der Selbsttest schreibt seinen Bericht nach stderr; stderr geht im Port
        # in debug.log im AKTUELLEN Arbeitsverzeichnis. Deshalb wird der Bericht
        # von dort gelesen und nicht von stdout (eine GUI-exe hat unter Git-Bash
        # oft gar keine Konsole, an die sie sich haengen koennte).
        #
        # MIT ZEITGRENZE UND AUFRAEUMEN, und zwar aus Erfahrung: kennt das Binary
        # den Schalter NICHT (alter Build), startet es statt des Selbsttests das
        # ganze SPIEL — ein Fenster, das nie von allein zurueckkehrt. GEMESSEN
        # 2026-08-24 mit dem 0.3.19-Binary: der Aufruf kehrte zurueck, der Prozess
        # lief WEITER. Ein haengender re15_pc blockiert danach den Linker.
        # Deshalb: im Hintergrund starten, auf den Selbsttest warten (er braucht
        # ~50 ms), und die PID am Ende in jedem Fall abraeumen.
        rc=0
        ( cd "$run_dir" && env -u RE15_ASSET_ROOT -u RE15_CD_ROOT -u RE15_RE2_ASSET_ROOT \
              RE15_ASSET_SELFTEST=1 "$(cd "$out" && pwd)/$bin" ) >/dev/null 2>&1 &
        local probe_pid=$! waited=0
        while (( waited < 300 )); do                       # 300 x 0.1 s = 30 s Deckel
            kill -0 "$probe_pid" 2>/dev/null || break
            grep -q '^\[selftest\] RESULT' "$log" 2>/dev/null && break
            sleep 0.1; waited=$((waited + 1))
        done
        if kill -0 "$probe_pid" 2>/dev/null; then
            kill -9 "$probe_pid" 2>/dev/null || true
            rc=124
        else
            wait "$probe_pid" 2>/dev/null || rc=$?
        fi
        # Sicherheitsnetz: der GUI-Prozess kann den Shell-Job ueberleben (genau so
        # geschehen). Was dieser Lauf gestartet hat, wird hier zuverlaessig beendet.
        case "$(uname -s)" in
            MINGW*|MSYS*|CYGWIN*) taskkill //F //IM "$bin" >/dev/null 2>&1 || true ;;
        esac

        [[ -f "$log" ]] || die "Laufzeit-Gate ($pass): das Binary hat keine debug.log
        geschrieben. Entweder ist es sofort abgestuerzt, oder es kennt
        RE15_ASSET_SELFTEST nicht — dann stammt es aus einem Build VOR dem
        2026-08-24-Fix und gehoert nicht in dieses Paket."

        local result
        result="$(grep -m1 '^\[selftest\] RESULT' "$log" || true)"
        [[ -n "$result" ]] || die "Laufzeit-Gate ($pass): keine '[selftest] RESULT'-Zeile in
        $log. Das Binary kennt RE15_ASSET_SELFTEST nicht -> es ist ein alter Build."

        # (a) Wurde alles gefunden?
        if [[ "$result" != *"missing=0"* ]]; then
            echo "--- fehlende Assets ($pass) ---" >&2
            grep '^\[selftest\] MISSING' "$log" >&2 || true
            die "Laufzeit-Gate ($pass): $result
        Das Paket startet beim Nutzer, findet aber die oben gelisteten Assets
        NICHT. Genau das war der 0.3.19-Fehler."
        fi

        # (b) DIE EIGENTLICHE PRUEFUNG: kam auch alles aus dem PAKET?
        #     Ein Treffer aus dem Repo bedeutet, dass das Paket beim Nutzer —
        #     der dieses Repo nicht hat — an derselben Stelle scheitern wird.
        local strays
        strays="$(grep '^\[selftest\] OK' "$log" | sed 's/.*<- //' \
                  | grep -v -F "$pkg_abs/" || true)"
        if [[ -n "$strays" ]]; then
            echo "--- Treffer AUSSERHALB des Pakets ($pass) ---" >&2
            echo "$strays" | sort -u >&2
            die "Laufzeit-Gate ($pass): das Paket laedt Assets von ausserhalb
        ($pkg_abs). Auf DIESER Maschine faellt das nicht auf — beim Nutzer
        gibt es diese Pfade nicht. Das ist exakt die Taeuschung, die den
        0.3.19-Fehler bis zur Auslieferung durchgelassen hat."
        fi

        (( rc == 0 )) || die "Laufzeit-Gate ($pass): Selbsttest-Exitcode $rc trotz $result"

        local n
        n="$(grep -c '^\[selftest\] OK' "$log")"
        echo "   Laufzeit-Gate $pass: $n/$n Assets, alle aus dem Paket (cwd=$(basename "$run_dir"))"
        rm -f "$log"
    done
}

# --- Gemeinsames Einsammeln --------------------------------------------------
[[ -d "$ASSETS" ]] || die "Asset-Baum fehlt: $ASSETS"
[[ -d "$FX"     ]] || die "Effekt-Texturen fehlen: $FX"
[[ -s "$RE2/CDEMD0.EMS" && -s "$RE2/ENEMSE.VBS" ]] || die "RE2-Assets fehlen: $RE2"
[[ -s "$SYNCHRO/STAGE1/room1170/main00.wav" ]] || die "Voiceover-Quelle fehlt: $SYNCHRO/STAGE1"

copy_common() {          # $1 = Paketordner
    local out="$1"
    echo "   Assets kopieren (shared_assets/PSX, ~283 MB) ..."
    mkdir -p "$out/shared_assets"
    cp -r "$ASSETS" "$out/shared_assets/PSX"
    echo "   Effekt-Texturen kopieren (shared_assets/extracted_fx) ..."
    cp -r "$FX" "$out/shared_assets/extracted_fx"
    echo "   RE2-Assets kopieren (shared_assets/RE2, ~18 MB, fuer OPTIONS->AI=RE2) ..."
    cp -r "$RE2" "$out/shared_assets/RE2"
    echo "   Voiceover kopieren (synchro/STAGE1, ~17 MB) ..."
    # Paket-Wurzel, NICHT unter shared_assets: re15_pc_read_base() sucht
    # "<Basis-Wurzel>/synchro/..." und die Basis-Wurzel ist das exe-Verzeichnis.
    # synchro/unused/ (24 MB) bleibt draussen — kein Codepfad liest es.
    mkdir -p "$out/synchro"
    cp -r "$SYNCHRO/STAGE1" "$out/synchro/STAGE1"
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
    check_binary_fresh "$LINUX_BIN" "Linux-Binary"
    check_glibc        "$LINUX_BIN"

    OUT="$HERE/pkg-linux/$NAME"
    if [[ $ZIP_ONLY -eq 0 ]]; then
        rm -rf "$HERE/pkg-linux"; mkdir -p "$OUT"
        install -m 755 "$LINUX_BIN"            "$OUT/re15_pc"
        # ⛔ ZEILENENDEN NORMALISIEREN (2026-08-25, Nutzer-Report "du baust das shell Skript
        # run.sh in crlf statt in lf"): .gitattributes hat zwar `*.sh text eol=lf`, das
        # normalisiert aber nur, was git SPEICHERT — eine vom Editor/Werkzeug mit CRLF
        # geschriebene Datei bleibt im ARBEITSBAUM CRLF, und genau von dort kopiert dieses
        # Skript. Auf dem Deck scheitert so ein run.sh schon am Shebang
        # ("/usr/bin/env bash\r: Datei oder Verzeichnis nicht gefunden"). Deshalb hier beim
        # Kopieren hart auf LF ziehen statt sich auf den Arbeitsbaum zu verlassen.
        tr -d '\r' < "$HERE/pkg_files/linux/run.sh" > "$OUT/run.sh"
        chmod 755 "$OUT/run.sh"
        render_readme "$HERE/pkg_files/linux/README.txt.in" "$OUT/README_${VERSION#v}.txt"
        copy_common "$OUT"
    fi
    check_tree  "$OUT"
    check_lf    "$OUT"
    check_runtime_assets "$OUT" "re15_pc"
    LINUX_FILES=$(find "$OUT" -type f | wc -l)
    echo "   OK: $(du -sh "$OUT" | cut -f1), $LINUX_FILES Dateien"
fi

# --- Windows -----------------------------------------------------------------
if [[ "$ONLY" == "both" || "$ONLY" == "win" ]]; then
    [[ -f "$WIN_BIN" ]] || die "Windows-Binary fehlt: $WIN_BIN"
    echo "== Windows-Paket: $NAME =="
    check_binary_paths "$WIN_BIN"
    check_binary_fresh "$WIN_BIN" "Windows-Binary"

    OUT="$HERE/pkg-win/$NAME"
    if [[ $ZIP_ONLY -eq 0 ]]; then
        rm -rf "$HERE/pkg-win"; mkdir -p "$OUT"
        install -m 755 "$WIN_BIN"                          "$OUT/re15_pc.exe"
        install -m 644 "$HERE/pkg_files/win/Start_RE15_Port.bat" "$OUT/Start_RE15_Port.bat"
        # Diagnose-Starter: setzt die Trace-Haken und sichert das Log unter eigenem Namen.
        # Damit kann der Nutzer einen gemeldeten Fehler selbst belegen, statt dass ich
        # seinen Spielzustand raten muss.
        install -m 644 "$HERE/pkg_files/win/Diagnose_ROOM1090.bat" "$OUT/Diagnose_ROOM1090.bat"
        render_readme "$HERE/pkg_files/win/README.txt.in"   "$OUT/README_${VERSION#v}.txt"
        copy_common "$OUT"
    fi
    check_tree  "$OUT"
    check_runtime_assets "$OUT" "re15_pc.exe"
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

# --- Git: NUR die aktuelle Version im Repo halten ----------------------------
# Nutzer-Vorgabe 2026-08-22: "zukuenftig bitte immer das neuste Package mit
# hochladen, und alte Packages vom Repo loeschen."
# Hintergrund: bis v0.3.8 wurde JEDE Version eingecheckt und keine je entfernt —
# 86 Paketdateien mit 5,9 GB steckten in der Historie und blaehten das Repo auf
# 8,6 GB auf. Die Historie wurde einmalig bereinigt (git filter-repo, 2,4 GB);
# damit das nicht zurueckkehrt, macht dieses Skript den Austausch selbst.
if command -v git >/dev/null 2>&1 && git -C "$HERE/.." rev-parse --git-dir >/dev/null 2>&1; then
    echo "== Git: alte Pakete austauschen =="
    alt=0
    while IFS= read -r f; do
        [[ -n "$f" ]] || continue
        case "$(basename "$f")" in
            "${NAME}"_*) continue ;;          # die AKTUELLE Version bleibt
        esac
        git -C "$HERE/.." rm --cached --quiet -- "$f" 2>/dev/null && alt=$((alt+1))
        rm -f "$HERE/../$f"                   # auch lokal weg, sonst waechst release/ endlos
    done < <(git -C "$HERE/.." ls-files -- 'release/re15_port_v0*')
    # Auch UNGETRACKTE Altpakete entfernen — sonst waechst release/ lokal endlos
    # weiter (die Schleife oben sieht nur, was Git kennt).
    for f in "$HERE"/re15_port_v0*.z*; do
        [[ -f "$f" ]] || continue
        case "$(basename "$f")" in
            "${NAME}"_*) continue ;;
        esac
        rm -f "$f" && alt=$((alt+1))
    done
    neu=0
    for f in "$HERE/${NAME}"_*.z*; do
        [[ -f "$f" ]] || continue
        git -C "$HERE/.." add -- "release/$(basename "$f")" && neu=$((neu+1))
    done
    echo "   $alt alte Paketdatei(en) aus dem Repo entfernt, $neu neue vorgemerkt"
    echo "   (noch nicht committet — das macht der Release-Commit)"
fi

echo "== Fertig =="
