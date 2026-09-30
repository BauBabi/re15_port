#!/usr/bin/env bash
# =============================================================================
# release/python_finden.sh — ein ECHTES Python >= 3.8 finden (Runde 34a, 2026-09-29)
# =============================================================================
# WARUM: Unter Git-Bash zeigt "python3" auf der Bau-Maschine ZUERST auf den App-Alias
#     C:/Users/<name>/AppData/Local/Microsoft/WindowsApps/python3(.exe)
# (Python Install Manager). Ein "python3"-Aufruf aus make_package.sh (verify_split) hat in
# v0.8.17 ungefragt Python 3.14 installiert; v0.8.19 lief nur mit einem Shim im PATH.
# Schon das STARTEN eines solchen Alias kann den Installer anwerfen - deshalb wird ein
# WindowsApps-Pfad hier am PFAD erkannt und verworfen, NIE ausgefuehrt.
#
# AUFRUF
#   source "$HERE/python_finden.sh" || die "..."   setzt PY (Pfad) und PY_VERSION (x.y.z)
#   bash release/python_finden.sh                   Diagnose: druckt den Fund, Rueckgabe 0/1
#
# KANDIDATEN (Reihenfolge; der erste, der die Pruefung besteht, gewinnt)
#   1. $RE15_PYTHON, falls gesetzt - dann NUR dieser (kein stiller Rueckfall)
#   2. die PATH-Ordner der Reihe nach, in JEDEM Ordner erst "python3", dann "python"
#      (leere PATH-Eintraege = aktuelles Verzeichnis werden uebergangen)
#   3. nur Windows (Git-Bash/MSYS/Cygwin): /c/Python3*/python.exe und
#      "/c/Program Files"/Python3*/python.exe - abschaltbar mit RE15_PYTHON_NUR_PATH=1
#   Nachbesserung R1 (Gegenpruefung echtlauf B1): bis dahin kamen erst ALLE "python3" des PATH
#   und danach alle "python". Auf der Bau-Maschine gewann so /c/msys64/mingw64/bin/python3
#   (MSYS2 3.14.x, Abhaengigkeit von gdb, wandert mit jedem pacman -Syu) gegen /c/Python310, das
#   an Stelle 12 des PATH steht (msys64 an 43) und auch "python" im cmd-Fenster ist.
# PRUEFUNG je Kandidat
#   - Pfad, Langname (cygpath -l, falls da: 8.3-Kurznamen wie .../MICROS~1/WINDOW~1/) oder
#     Link-Ziel unter */WindowsApps/*, Gross/klein egal                 -> verworfen, NICHT gestartet
#   - ein Link, dessen Ziel sich nicht pruefen laesst (kein readlink)   -> verworfen, NICHT gestartet
#   - fehlt oder leere Datei (App-Execution-Aliase sind 0 Byte)       -> verworfen, NICHT gestartet
#   - '<kand> -c "import sys, zipfile, hashlib; ..."' mit stdin=/dev/null (+ timeout 30, falls
#     vorhanden) muss Python >= 3.8 melden                               -> sonst verworfen
# Nichts gefunden -> Meldung auf stderr, Rueckgabe 1 (fail closed).
# Ausser dem Kandidaten selbst (und optional timeout/readlink/cygpath) nur Bash-Bordmittel: laeuft
# damit auch mit einem PATH, der NUR den WindowsApps-Ordner enthaelt - unter Lang- UND 8.3-Namen
# (Nachbesserung R2, Gegenpruefung B7: bis dahin stimmte das nur fuer den Langnamen; ohne readlink
# wurde die Linkregel uebersprungen und der Alias ueber den 8.3-Pfad gestartet).
# Aufrufer: release/build_android.sh (APK-Asset-Gate), release/make_package.sh (verify_split,
# zip_exec_bit.py). Beide laufen mit "set -euo pipefail": hier daher nur ${VAR:-}-Zugriffe.
# =============================================================================

re15_python_finden() {
    local kand low schluessel ziel lang ver rc name p d
    local -a kands=() ordner=()
    local -A gesehen=()
    local probe='import sys, zipfile, hashlib; print("%d.%d.%d" % tuple(sys.version_info[:3])); sys.exit(0 if sys.version_info >= (3, 8) else 3)'
    PY=""
    PY_VERSION=""

    if [[ -n "${RE15_PYTHON:-}" ]]; then
        kands=("$RE15_PYTHON")
    else
        IFS=: read -r -a ordner <<<"${PATH:-}"
        for d in ${ordner[@]+"${ordner[@]}"}; do
            [[ -n "$d" ]] || continue
            for name in python3 python; do
                if [[ -f "$d/$name" || -f "$d/$name.exe" ]]; then
                    kands+=("$d/$name")
                fi
            done
        done
        case "${OSTYPE:-}" in
            msys*|cygwin*|win32*)
                if [[ "${RE15_PYTHON_NUR_PATH:-0}" != 1 ]]; then
                    for p in /c/Python3*/python.exe "/c/Program Files"/Python3*/python.exe; do
                        [[ -f "$p" ]] && kands+=("$p")
                    done
                fi ;;
        esac
    fi

    for kand in ${kands[@]+"${kands[@]}"}; do
        low="${kand,,}"
        low="${low//\\//}"
        schluessel="${low%.exe}"
        [[ -n "${gesehen[$schluessel]:-}" ]] && continue
        gesehen[$schluessel]=1
        case "$low" in
            */windowsapps/*)
                echo "   Python-Kandidat verworfen (WindowsApps-Alias, NICHT gestartet): $kand" >&2
                continue ;;
        esac
        # 8.3-Kurzname (Nachbesserung R2, Gegenpruefung B7): .../MICROS~1/WINDOW~1/python3 entgeht der
        # Pfadregel oben; cygpath -l liefert den Langnamen (Git-Bash/MSYS/Cygwin).
        if type -P cygpath >/dev/null 2>&1; then
            lang="$(cygpath -m -l -- "$kand" 2>/dev/null || true)"
            case "${lang,,}" in
                */windowsapps/*)
                    echo "   Python-Kandidat verworfen (Langname $lang = WindowsApps-Alias, NICHT gestartet): $kand" >&2
                    continue ;;
            esac
        fi
        if [[ -L "$kand" ]]; then
            # Ein Link, dessen Ziel sich nicht pruefen laesst, wird VERWORFEN (Nachbesserung R2, B7): bis dahin
            # wurde die Linkregel ohne readlink uebersprungen - mit dem 8.3-Pfad kam so der Alias zum Start.
            if ! type -P readlink >/dev/null 2>&1; then
                echo "   Python-Kandidat verworfen (Link, Ziel ohne readlink nicht pruefbar - NICHT gestartet): $kand" >&2
                continue
            fi
            ziel="$(readlink -f -- "$kand" 2>/dev/null || true)"
            case "${ziel,,}" in
                */windowsapps/*|"")
                    echo "   Python-Kandidat verworfen (Link auf WindowsApps oder Ziel unlesbar: '${ziel}', NICHT gestartet): $kand" >&2
                    continue ;;
            esac
        fi
        if [[ ! -s "$kand" && ! -s "$kand.exe" ]]; then
            echo "   Python-Kandidat verworfen (fehlt oder 0 Byte - App-Alias?, NICHT gestartet): $kand" >&2
            continue
        fi
        if type -P timeout >/dev/null 2>&1; then
            echo "   SONDE: WUERDE STARTEN (mit timeout): $kand" >&2; ver=""; rc=99
        else
            echo "   SONDE: WUERDE STARTEN (ohne timeout): $kand" >&2; ver=""; rc=99
        fi
        ver="${ver%%$'\r'*}"
        if [[ $rc -ne 0 || ! "$ver" =~ ^[0-9]+\.[0-9]+\.[0-9]+$ ]]; then
            echo "   Python-Kandidat verworfen (Pruefung rc=$rc, Version '${ver}' - braucht >= 3.8 mit zipfile/hashlib): $kand" >&2
            continue
        fi
        PY="$kand"
        PY_VERSION="$ver"
        echo "   Python: $PY ($PY_VERSION)" >&2
        return 0
    done

    echo "FEHLER: kein echtes Python >= 3.8 gefunden (geprueft: ${#gesehen[@]} Kandidat(en);" \
         "WindowsApps-Aliase werden nie gestartet). Abhilfe: RE15_PYTHON=<pfad zu python.exe> setzen," \
         "z.B. RE15_PYTHON=/c/Python310/python.exe" >&2
    return 1
}

if [[ "${BASH_SOURCE[0]}" == "${0}" ]]; then
    if re15_python_finden; then
        echo "PY=$PY"
        echo "PY_VERSION=$PY_VERSION"
        exit 0
    fi
    exit 1
fi
re15_python_finden
