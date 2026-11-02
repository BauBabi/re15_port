#!/usr/bin/env bash
# sichel_spiel.sh — Hangar-/Labor-Birkin (EM30_HANGAR.EMD) im ECHTEN Spiel zeigen und das echte Fenster abgreifen.
# Muster: tools/form_spiel.sh der Bahnsteig-Spur (Zweig r41/form), hier ohne Posen-Haken: der Raum laeuft normal.
#
#   bash re15_port/tools/sichel_spiel.sh --raum 50A0 --aus bild.png [--bilder 6] [--abstand 1]
#
# Ablauf: eigene exe-Kopie mit eigenem Namen NEBEN der gebauten exe (Asset-Wurzel = exe-Verzeichnis, wie die
# Integrationstests), Laufverzeichnis build/sichel_spiel/<id>, Debug-Sprung in den Raum, RE15_ENEMY_DBG=1 meldet in
# enemy_dbg.log, sobald ein 0x30 gezeichnet wird ("ZEICHNE Typ 0x30: <Teile> Teile (Bones .., Meshes ..)"), dann
# Fensterbilder: gdigrab per Fenster-Handle (Skill re15-port-visual-verify) oder, in einer getrennten Sitzung,
# form_fensterbild.ps1 (PrintWindow). Kein AUTOSHOT, kein SOFTWARE_RENDER. Beendet nur die eigene PID.
#
# Optionen:
#   --raum HEX            Raum, Standard 50A0
#   --spieler x,z,rot,b   RE15_PLAYER_POS nach dem Sprung (leer = Raumvorgabe)
#   --aus DATEI.png       Ziel (Pflicht); bei --bilder N > 1 werden es _1.png .. _N.png
#   --bilder N            Anzahl Fensterbilder, Standard 1
#   --abstand S           Sekunden zwischen den Bildern, Standard 1
#   --nach S              Wartezeit nach dem ersten gezeichneten 0x30, Standard 2
#   --zeit S              Hoechstdauer, Standard 150
#   --env "A=1 B=2"       weitere Umgebungsvariablen
#   --warte S             nicht auf die ZEICHNE-Zeile warten, sondern S Sekunden nach dem Erscheinen des Fensters abgreifen
# Ausgabe: Bildpfade, die ZEICHNE-Zeile und die Ersatzmodell-Zeile. Rueckgabe 0 nur mit Bild und gezeichnetem 0x30.
set -u

BAUM="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")/../.." && pwd -W)"
PS=/c/Windows/System32/WindowsPowerShell/v1.0/powershell.exe
FFMPEG=/c/ProgramData/chocolatey/bin/ffmpeg
EXE_DIR="$BAUM/re15_port/build/platform/pc"
EXE="$EXE_DIR/re15_pc.exe"

RAUM=50A0; SPIELER=""; AUS=""; BILDER=1; ABSTAND=1; NACH=2; ZEIT=150; ZUSATZ=""; WARTE=""
while [ $# -gt 0 ]; do
    case "$1" in
        --raum) RAUM="$2"; shift 2 ;;
        --spieler) SPIELER="$2"; shift 2 ;;
        --aus) AUS="$2"; shift 2 ;;
        --bilder) BILDER="$2"; shift 2 ;;
        --abstand) ABSTAND="$2"; shift 2 ;;
        --nach) NACH="$2"; shift 2 ;;
        --zeit) ZEIT="$2"; shift 2 ;;
        --env) ZUSATZ="$2"; shift 2 ;;
        --warte) WARTE="$2"; shift 2 ;;
        *) echo "unbekannte Option $1" >&2; exit 2 ;;
    esac
done
[ -n "$AUS" ] || { echo "--aus ist Pflicht" >&2; exit 2; }
[ -f "$EXE" ] || { echo "exe fehlt: $EXE (bash re15_port/tools/local_build.sh build)" >&2; exit 2; }
python -c "import PIL, numpy" 2>/dev/null || { echo "python ohne PIL/numpy: $(command -v python)" >&2; exit 2; }

ID="$$_$(date +%H%M%S)"
LAUF="$BAUM/re15_port/build/sichel_spiel/$ID"
NAME="re15_pc_sichel_$ID"
mkdir -p "$LAUF"
cp "$EXE" "$EXE_DIR/$NAME.exe" || exit 2

export SDL_ASSERT=always_ignore RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_WINDOW_SCALE=2
export RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2
export RE15_DEBUG_JUMP="$RAUM@250" RE15_ENEMY_DBG=1 RE15_SPAWN_DIAG=1
[ -n "$SPIELER" ] && export RE15_PLAYER_POS="$SPIELER"
export RE15_EXIT_AT="9000#$RAUM"                         # Selbstabbruch, falls dieses Skript stirbt
for kv in $ZUSATZ; do export "$kv"; done

cd "$LAUF" || exit 2
timeout "$ZEIT" "$EXE_DIR/$NAME.exe" >exe_stdout.log 2>exe_stderr.log &
GP=$!

hellstes() { python -c "import sys; from PIL import Image; import numpy as n; print(int(n.array(Image.open(sys.argv[1]).convert('RGB')).max()))" "$1" 2>/dev/null || echo 0; }
hole_proz() {
    "$PS" -NoProfile -Command "Get-Process $NAME -ErrorAction SilentlyContinue | Select-Object -First 1 | ForEach-Object { '{0} {1}' -f \$_.Id, \$_.MainWindowHandle }" 2>/dev/null | tr -d '\r'
}

ENDE=$(( $(date +%s) + ZEIT - 10 ))
WPID=""; HWND=""
while [ "$(date +%s)" -lt "$ENDE" ]; do
    set -- $(hole_proz)
    WPID="${1:-}"; HWND="${2:-0}"
    [ -n "$WPID" ] && [ "$HWND" != "0" ] && break
    kill -0 "$GP" 2>/dev/null || break
    sleep 1
done
echo "exe $NAME PID ${WPID:-?} Fenster ${HWND:-?}"

ANWESEND=0
if [ -n "$WARTE" ]; then sleep "$WARTE"; ANWESEND=1; fi
while [ "$ANWESEND" = 0 ] && [ "$(date +%s)" -lt "$ENDE" ]; do
    if grep -q "ZEICHNE Typ 0x30" enemy_dbg.log 2>/dev/null; then ANWESEND=1; break; fi
    kill -0 "$GP" 2>/dev/null || break
    sleep 1
done
[ "$ANWESEND" = 1 ] && sleep "$NACH"

RC=1
if [ "$ANWESEND" = 1 ] && [ -n "$HWND" ] && [ "$HWND" != "0" ]; then
    set -- $(hole_proz); HWND="${2:-$HWND}"
    "$PS" -NoProfile -Command "Add-Type -Name F -Namespace R -MemberDefinition '[DllImport(\"user32.dll\")] public static extern bool SetWindowPos(System.IntPtr h, System.IntPtr a, int x, int y, int w, int g, uint f);'; [void][R.F]::SetWindowPos([System.IntPtr]$HWND, [System.IntPtr](-1), 20, 20, 0, 0, 0x41)" >/dev/null 2>&1
    sleep 1
    HX=$(printf '0x%x' "$HWND")
    mkdir -p "$(dirname -- "$AUS")"
    if "$FFMPEG" -hide_banner -loglevel quiet -f gdigrab -framerate 1 -i desktop -frames:v 1 -f null - 2>/dev/null; then WEG=gdigrab
    else WEG=PrintWindow; fi
    i=1; RC=0
    while [ "$i" -le "$BILDER" ]; do
        ZIEL="$AUS"; [ "$BILDER" -gt 1 ] && ZIEL="${AUS%.png}_$i.png"
        GUT=0
        for versuch in 1 2 3 4; do
            if [ "$WEG" = gdigrab ]; then
                "$FFMPEG" -hide_banner -loglevel error -y -f gdigrab -framerate 5 -draw_mouse 0 -i "hwnd=$HX" -frames:v 2 -update 1 "$ZIEL" 2>/dev/null
            else
                "$PS" -NoProfile -ExecutionPolicy Bypass -File "$(cygpath -w "$BAUM/re15_port/tools/form_fensterbild.ps1")" -Hwnd "$HWND" -Aus "$(cygpath -w "$ZIEL")" | tr -d '\r'
            fi
            if [ "$(hellstes "$ZIEL")" -ge 8 ]; then GUT=1; break; fi
            sleep 1
        done
        if [ "$GUT" = 1 ]; then echo "Verfahren: $WEG  Bild: $ZIEL"; else echo "kein Bild: schwarz ($ZIEL)" >&2; RC=1; fi
        i=$((i + 1))
        [ "$i" -le "$BILDER" ] && sleep "$ABSTAND"
    done
else
    echo "kein Bild: kein 0x30 gezeichnet (ANWESEND=$ANWESEND) oder kein Fenster" >&2
fi

if [ -n "$WPID" ]; then
    taskkill //PID "$WPID" >/dev/null 2>&1
    for i in 1 2 3 4 5 6; do kill -0 "$GP" 2>/dev/null || break; sleep 1; done
    kill -0 "$GP" 2>/dev/null && taskkill //PID "$WPID" //T //F >/dev/null 2>&1
fi
wait "$GP" 2>/dev/null
rm -f "$EXE_DIR/$NAME.exe"
grep -a "ZEICHNE Typ 0x30" enemy_dbg.log 2>/dev/null | head -1
grep -a "Ersatzmodell" exe_stderr.log 2>/dev/null | tail -1
echo "Laufverzeichnis: $LAUF"
exit $RC
