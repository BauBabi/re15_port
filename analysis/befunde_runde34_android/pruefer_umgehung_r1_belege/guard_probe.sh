#!/usr/bin/env bash
# Wertet die drei Schutzregeln von release/python_finden.sh (Zeilen 60-81, woertlich uebernommen) fuer
# gegebene Kandidatenpfade aus - OHNE einen Kandidaten zu starten. Zweck: pruefen, ob ein Alias-Pfad in
# anderer Schreibweise (8.3, Backslash, Grossbuchstaben) VOR dem Start verworfen wuerde.
# Aufruf: bash guard_probe.sh <pfad> [<pfad> ...]
for kand in "$@"; do
    low="${kand,,}"
    low="${low//\\//}"
    g1=nein; g2=nein; g3=nein; ziel=""
    case "$low" in
        */windowsapps/*) g1=JA ;;
    esac
    if [[ -L "$kand" ]] && type -P readlink >/dev/null 2>&1; then
        ziel="$(readlink -f -- "$kand" 2>/dev/null || true)"
        case "${ziel,,}" in
            */windowsapps/*) g2=JA ;;
        esac
    fi
    if [[ ! -s "$kand" && ! -s "$kand.exe" ]]; then g3=JA; fi
    if [[ $g1 == JA || $g2 == JA || $g3 == JA ]]; then urteil="verworfen OHNE Start"; else urteil="WUERDE GESTARTET"; fi
    printf '%s\n   pfad-guard=%s  link-guard=%s (%s)  leer-guard=%s  -> %s\n' "$kand" "$g1" "$g2" "${ziel:-kein Link}" "$g3" "$urteil"
done
