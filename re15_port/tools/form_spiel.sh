#!/usr/bin/env bash
# form_spiel.sh — Kandidat der Bahnsteig-Form im ECHTEN Spiel zeigen und das echte Fenster abgreifen.
#
#   bash re15_port/tools/form_spiel.sh --emd X.EMD --tim X.TIM --pose x,z,gier,clip,bild,schnitt --aus bild.png
#
# Ablauf: eigene exe-Kopie mit eigenem Namen unter re15_port/build/form_spiel/<id>/, Kandidat als
# BAHNSTEIG_G4.EMD/.TIM in ein privates Verzeichnis (RE15_RE2_ASSET_ROOT, alle anderen RE2-Dateien fallen auf
# shared_assets/RE2 zurueck), Debug-Sprung in den Raum, Haken RE15_BB_POSE haelt Lage, Clip und Schnitt
# (engine/src/birkin_bahnsteig.c), gdigrab per Fenster-Handle (Skill re15-port-visual-verify), Ende nur der eigenen PID.
# Kein AUTOSHOT, kein SOFTWARE_RENDER. Das Skript beendet nie fremde Prozesse.
# In einer getrennten Sitzung (qwinsta Status Getr.) liefert gdigrab nur Schwarz: dann holt form_fensterbild.ps1 den Inhalt des
# echten D3D11-Fensters von aussen ueber PrintWindow(PW_RENDERFULLCONTENT). Die Ausgabezeile "Verfahren:" nennt den Weg.
#
# Optionen:
#   --emd DATEI --tim DATEI   Kandidat. Ohne beide laeuft das Asset aus shared_assets/RE2.
#   --pose WERTE              x,z,gier,clip,bild,schnitt (Pflicht). bild < 0 spielt den Clip ab.
#   --aus DATEI.png           Ziel des Fensterbilds (Pflicht)
#   --raum HEX                Raum, Standard 5090
#   --spieler x,z,rot,band    Spieler nach dem Sprung, Standard 500,-9175,2048,0 (aus integration_r41_birkin_pistole)
#   --fenster N               RE15_WINDOW_SCALE, Standard 2 (640x480)
#   --nach SEKUNDEN           Wartezeit zwischen Pose und Bild, Standard 3
#   --zeit SEKUNDEN           Hoechstdauer des Laufs, Standard 150
#   --bilder N                N Fensterbilder im Sekundenabstand (--aus wird zu _1.png .. _N.png), Standard 1
#   --einsetzen               Kandidat zusaetzlich nach shared_assets/RE2 kopieren (im Arbeitsbaum, versioniert)
#   --zuruecksetzen           BAHNSTEIG_G4.EMD/.TIM aus git wiederherstellen und enden
#   --env "A=1 B=2"           weitere Umgebungsvariablen fuer die exe
#
# Ausgabe: Pfad des Bilds, Zeile "[enemy] Bahnsteig-Form: ..." aus debug.log, Lagezeile aus bb_pose.log.
# Rueckgabe 0 nur, wenn die Form geladen ist, der 0x30 im Raum steht und das Bild geschrieben wurde.
set -u

BAUM="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")/../.." && pwd -W)"
PS=/c/Windows/System32/WindowsPowerShell/v1.0/powershell.exe
FFMPEG=/c/ProgramData/chocolatey/bin/ffmpeg
EXE="$BAUM/re15_port/build/platform/pc/re15_pc.exe"
RE2="$BAUM/re15_port/shared_assets/RE2"

EMD=""; TIM=""; POSE=""; AUS=""; RAUM=5090; SPIELER="500,-9175,2048,0"; FENSTER=2; NACH=3; ZEIT=150; BILDER=1
EINSETZEN=0; ZUSATZ=""
while [ $# -gt 0 ]; do
    case "$1" in
        --emd) EMD="$2"; shift 2 ;;
        --tim) TIM="$2"; shift 2 ;;
        --pose) POSE="$2"; shift 2 ;;
        --aus) AUS="$2"; shift 2 ;;
        --raum) RAUM="$2"; shift 2 ;;
        --spieler) SPIELER="$2"; shift 2 ;;
        --fenster) FENSTER="$2"; shift 2 ;;
        --nach) NACH="$2"; shift 2 ;;
        --zeit) ZEIT="$2"; shift 2 ;;
        --bilder) BILDER="$2"; shift 2 ;;
        --einsetzen) EINSETZEN=1; shift ;;
        --env) ZUSATZ="$2"; shift 2 ;;
        --zuruecksetzen)
            git -C "$BAUM" checkout -- re15_port/shared_assets/RE2/BAHNSTEIG_G4.EMD re15_port/shared_assets/RE2/BAHNSTEIG_G4.TIM
            echo "shared_assets/RE2/BAHNSTEIG_G4.EMD/.TIM aus git wiederhergestellt"; exit $? ;;
        *) echo "unbekannte Option $1" >&2; exit 2 ;;
    esac
done
[ -n "$POSE" ] && [ -n "$AUS" ] || { echo "--pose und --aus sind Pflicht" >&2; exit 2; }
[ -f "$EXE" ] || { echo "exe fehlt: $EXE (bash re15_port/tools/local_build.sh build)" >&2; exit 2; }
if [ -n "$EMD$TIM" ]; then
    [ -f "$EMD" ] && [ -f "$TIM" ] || { echo "Kandidat unvollstaendig: '$EMD' '$TIM'" >&2; exit 2; }
fi

ID="$$_$(date +%H%M%S)"
LAUF="$BAUM/re15_port/build/form_spiel/$ID"
NAME="re15_pc_form_$ID"
mkdir -p "$LAUF/re2"
cp "$EXE" "$LAUF/$NAME.exe" || exit 2
if [ -n "$EMD" ]; then
    cp "$EMD" "$LAUF/re2/BAHNSTEIG_G4.EMD"; cp "$TIM" "$LAUF/re2/BAHNSTEIG_G4.TIM"
    if [ "$EINSETZEN" = 1 ]; then
        cp "$EMD" "$RE2/BAHNSTEIG_G4.EMD"; cp "$TIM" "$RE2/BAHNSTEIG_G4.TIM"
        echo "eingesetzt: $RE2/BAHNSTEIG_G4.EMD/.TIM (zuruecksetzen: --zuruecksetzen)"
    fi
fi

# Umgebung wie integration_r41_birkin_pistole, aber sichtbares Fenster und echter Renderer
export RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_WINDOW_SCALE="$FENSTER"
export RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2
export RE15_DEBUG_JUMP="$RAUM@250" RE15_PLAYER_POS="$SPIELER"
export RE15_BIRKIN_FORM=bahnsteig RE15_BB_POSE="$POSE" RE15_SPAWN_DIAG=1
export RE15_EXIT_AT="9000#$RAUM"                        # Selbstabbruch, falls dieses Skript stirbt
[ -n "$EMD" ] && [ "$EINSETZEN" = 0 ] && export RE15_RE2_ASSET_ROOT="$LAUF/re2"
for kv in $ZUSATZ; do export "$kv"; done
export PATH="/c/msys64/mingw64/bin:$PATH"               # DLLs der exe

cd "$LAUF" || exit 2
timeout "$ZEIT" "./$NAME.exe" >exe_stdout.log 2>exe_stderr.log &
GP=$!

hellstes() {    # hoechster Kanalwert des Bilds, 0 wenn es fehlt
    python -c "import sys; from PIL import Image; import numpy as n; print(int(n.array(Image.open(sys.argv[1]).convert('RGB')).max()))" "$1" 2>/dev/null || echo 0
}

hole_proz() {   # Windows-PID + Fenster-Handle der EIGENEN exe (eindeutiger Name)
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
while [ "$(date +%s)" -lt "$ENDE" ]; do
    if grep -q "typ 0x30" bb_pose.log 2>/dev/null; then ANWESEND=1; break; fi
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
    i=1; RC=0
    while [ "$i" -le "$BILDER" ]; do
        ZIEL="$AUS"; [ "$BILDER" -gt 1 ] && ZIEL="${AUS%.png}_$i.png"
        VERFAHREN=""
        for versuch in 1 2 3 4 5 6; do
            # drei Bilder, das letzte zaehlt: das erste gdigrab-Bild kann noch vom Fensterwechsel stammen
            "$FFMPEG" -hide_banner -loglevel error -y -f gdigrab -framerate 5 -draw_mouse 0 -i "hwnd=$HX" -frames:v 3 -update 1 "$ZIEL" 2>/dev/null
            if [ "$(hellstes "$ZIEL")" -ge 8 ]; then VERFAHREN=gdigrab; break; fi
            # Getrennte Sitzung (qwinsta Status Getr.): gdigrab liefert schwarz -> Fensterverwaltung fragen (form_fensterbild.ps1)
            [ "$versuch" = 1 ] && echo "gdigrab: leeres Bild, Sitzung: $(/c/Windows/System32/qwinsta.exe 2>/dev/null | grep -a '^>' | tr -s ' ')"
            "$PS" -NoProfile -ExecutionPolicy Bypass -File "$(cygpath -w "$BAUM/re15_port/tools/form_fensterbild.ps1")" -Hwnd "$HWND" -Aus "$(cygpath -w "$ZIEL")" | tr -d ''
            if [ "$(hellstes "$ZIEL")" -ge 8 ]; then VERFAHREN=PrintWindow; break; fi
            sleep 2                     # Abblende oder noch kein Bild: gleich nochmal
        done
        if [ -n "$VERFAHREN" ]; then echo "Verfahren: $VERFAHREN (Versuch $versuch)"; echo "Bild: $ZIEL"
        else echo "kein Bild: alle Versuche schwarz" >&2; RC=1; fi
        i=$((i + 1))
        [ "$i" -le "$BILDER" ] && sleep 1
    done
else
    echo "kein Bild: Form nicht im Raum (ANWESEND=$ANWESEND) oder kein Fenster" >&2
fi

# Nur die eigene PID: erst freundlich, dann hart. Fremde exe bleiben unberuehrt.
if [ -n "$WPID" ]; then
    taskkill //PID "$WPID" >/dev/null 2>&1
    for i in 1 2 3 4 5 6; do
        kill -0 "$GP" 2>/dev/null || break
        sleep 1
    done
    kill -0 "$GP" 2>/dev/null && taskkill //PID "$WPID" //T //F >/dev/null 2>&1
fi
wait "$GP" 2>/dev/null
echo "exe beendet"

grep -a "Bahnsteig-Form" debug.log 2>/dev/null | tail -2
grep -a "typ 0x30" bb_pose.log 2>/dev/null | tail -1
grep -a -q "Bahnsteig-Form: " debug.log 2>/dev/null || RC=1
echo "Laufverzeichnis: $LAUF"
exit $RC
