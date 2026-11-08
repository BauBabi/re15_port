#!/usr/bin/env bash
# Runde 41 Spur SF, Einbau v0.8.30: baut die Sichel-Form V7 (Spiel-Asset EMD/EM30_HANGAR.EMD) aus committeten Eingaben neu
# und prueft die sha256 gegen release/em30_hangar.sha256 (Exit 1 bei Abweichung).
#
#   bash re15_port/tools/r41_sichel/v7_kette.sh [ZIELORDNER]      (Standard: Scratchpad .../laufc/kette)
#   Umgebung: KETTE_COMMIT (Stand der Werkzeuge, Standard HEAD), KETTE_REF2_COMMIT (Commit mit Referenz 2 cfd1ac42,
#             Standard ed254a7fe = letzter Stand vor dem Einbau)
#
# Nutzerfreigabe 2026-11-08 (woertlich): "Naja, gehen wir so erst mal ins rennnen." (Vorlage V7). Kein RE-Befund: die
# Sichel-Form gibt es auf keiner Disc; die Schritte und ihre Parameter sind Gestaltung (Dossier
# analysis/befunde_runde41/SF_sichel.md, Abschnitte V4 Runde 3 (ot3), V5 Runde 4, V6 Kopf, V5 wie Foto, V6 drei Punkte,
# V7 linke Hand, Einbau v0.8.30).
#
# Warum ein Schattenbaum: alle Bau-Werkzeuge lesen "Referenz 2" aus <Baum>/re15_port/shared_assets/PSX/EMD/EM30_HANGAR.EMD
# (kopf_orig_bau.REF2, v4_gemein.REF2, re2_kopf.REF2). Seit dem Einbau liegt dort V7 selbst; die Kette braucht aber den
# Grundstand Referenz 2 (sha256 cfd1ac42, git show KETTE_REF2_COMMIT). Darum: Werkzeuge per "git archive KETTE_COMMIT" in
# ZIEL/baum, EMD-Ordner aus git, darin EM30_HANGAR.EMD = Referenz 2; die grossen Lese-Ordner (PSX-Unterordner ausser EMD,
# shared_assets/RE2, engine, analysis, info) als Verzeichnis-Verbindungen (mklink /J) auf diesen Arbeitsbaum. Das Spiel-Asset
# dieses Arbeitsbaums wird nie beschrieben (birkin_formen.py laeuft nur mit --variante --aus ZIEL/emd im Schattenbaum).
#
# Schritte (Soll-sha256 = Dossier):
#   1 birkin_formen.py --variante v4        CDEMD0.EMS + Referenz 2 + v4/*.npz         -> EM30_V4ot3.EMD a1bc9309 (V4 Runde 3 ot3)
#   2 re2kopf/kopf_re2_bau.py (PARAM_R4)    + shared_assets/RE2/CDEMD0.EMS (G2/G3)     -> EM30_V5r4.EMD  38562ced (V5 Runde 4)
#   3 v6kopf/kopf_orig_bau.py (Stufe 1)     + Original-Kopf EM30, Referenz 2           -> EM30_V5k.EMD   99703bbd (V6 Kopf)
#   4 v5koerper/koerper_bau.py (abc)        + schwanz_param_v5f.json                   -> EM30_V5f.EMD   784c8f05 (V5 wie Foto)
#   5 v6drei/drei_bau.py (hsk)              + schwanz_laengen_v6.json / _radien_v6.json -> EM30_V6.EMD   9e0e58ef (V6 drei Punkte)
#   6 v7hand/hand_bau.py                    + SF_belege/v4/v7hand/foto_hand.json       -> EM30_V7.EMD    = Pin (V7 linke Hand)
set -u
HIER="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BAUM="$(git -C "$HIER" rev-parse --show-toplevel)" || exit 2
ZIEL="${1:-C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/laufc/kette}"
COMMIT="${KETTE_COMMIT:-HEAD}"
REF2_COMMIT="${KETTE_REF2_COMMIT:-ed254a7fe}"
REF2_SOLL=cfd1ac42729e56365847db42b352337d7f0c06fdcc1ef3413d0aee32d4a97a95
PIN="$BAUM/release/em30_hangar.sha256"
PIN_SOLL="$(awk 'NR==1{print $1}' "$PIN")"
[ -n "$PIN_SOLL" ] || { echo "FEHLER: Pin $PIN leer"; exit 2; }

verbindung() {   # $1 Verbindung, $2 Ziel (Verzeichnis-Verbindung, keine Kopie)
    MSYS_NO_PATHCONV=1 cmd /c mklink /J "$(cygpath -w "$1")" "$(cygpath -w "$2")" > /dev/null || { echo "FEHLER: mklink $1"; exit 2; }
}
verbindungen_weg() {   # Verbindungen nur loesen (rmdir ohne /s), nie in die Ziele hinein loeschen
    local v
    for v in "$ZIEL"/baum/re15_port/shared_assets/PSX/* "$ZIEL"/baum/re15_port/shared_assets/RE2 "$ZIEL"/baum/re15_port/engine \
             "$ZIEL"/baum/analysis "$ZIEL"/baum/info; do
        if [ -L "$v" ]; then MSYS_NO_PATHCONV=1 cmd /c rmdir "$(cygpath -w "$v")" > /dev/null; fi
    done
    return 0
}

mkdir -p "$ZIEL"
verbindungen_weg
rm -rf "$ZIEL/baum" "$ZIEL/emd" "$ZIEL/prot"
mkdir -p "$ZIEL/baum/re15_port/shared_assets/PSX" "$ZIEL/emd" "$ZIEL/prot"
trap verbindungen_weg EXIT

echo "== V7-Kette: Werkzeuge $(git -C "$BAUM" rev-parse --short "$COMMIT"), Referenz 2 aus $REF2_COMMIT, Ziel $ZIEL"
git -C "$BAUM" archive "$COMMIT" re15_port/tools re15_port/shared_assets/PSX/EMD | tar -x -C "$ZIEL/baum" || exit 2
git -C "$BAUM" show "$REF2_COMMIT:re15_port/shared_assets/PSX/EMD/EM30_HANGAR.EMD" \
    > "$ZIEL/baum/re15_port/shared_assets/PSX/EMD/EM30_HANGAR.EMD" || exit 2
r2="$(sha256sum "$ZIEL/baum/re15_port/shared_assets/PSX/EMD/EM30_HANGAR.EMD" | awk '{print $1}')"
[ "$r2" = "$REF2_SOLL" ] || { echo "FEHLER: Referenz 2 aus $REF2_COMMIT hat sha256 $r2 statt $REF2_SOLL"; exit 1; }
echo "   Referenz 2 (Schattenbaum) sha256 $r2"
for s in "$BAUM"/re15_port/shared_assets/PSX/*; do
    n="$(basename "$s")"
    [ "$n" = EMD ] || verbindung "$ZIEL/baum/re15_port/shared_assets/PSX/$n" "$s"
done
verbindung "$ZIEL/baum/re15_port/shared_assets/RE2" "$BAUM/re15_port/shared_assets/RE2"
verbindung "$ZIEL/baum/re15_port/engine" "$BAUM/re15_port/engine"
verbindung "$ZIEL/baum/analysis" "$BAUM/analysis"
verbindung "$ZIEL/baum/info" "$BAUM/info"

T="$ZIEL/baum/re15_port/tools"; E="$ZIEL/emd"; P="$ZIEL/prot"
rc=0
pruefe() {   # $1 Datei, $2 Soll-sha256, $3 Name
    local ist
    ist="$(sha256sum "$1" 2>/dev/null | awk '{print $1}')"
    if [ "$ist" = "$2" ]; then echo "   $3: $(basename "$1") sha256 $ist OK"
    else echo "   $3: $(basename "$1") sha256 ${ist:-fehlt} ABWEICHUNG (Soll $2)"; rc=1; fi
}
schritt() {   # $1 Name, $2 Arbeitsordner, Rest = Befehl (Python ohne -I: die Werkzeuge importieren Nachbarmodule)
    local n="$1" w="$2"
    shift 2
    echo "-- $n: $*"
    ( cd "$w" && "$@" ) > "$P/$n.txt" 2>&1 || { echo "FEHLER: Schritt $n (Protokoll $P/$n.txt)"; tail -5 "$P/$n.txt"; exit 1; }
}
schritt 1_v4ot3 "$T" python birkin_formen.py --variante v4 --aus "$E/v4"
mv -f "$E/v4/EM30_V4.EMD" "$E/EM30_V4ot3.EMD"
pruefe "$E/EM30_V4ot3.EMD" a1bc9309e5dd9ce8a948371ec4fef8595bd86310e3ea5e01dc3326552d5f8998 "1 V4 Runde 3 (ot3)"
schritt 2_v5r4 "$T/r41_sichel" python re2kopf/kopf_re2_bau.py --basis "$E/EM30_V4ot3.EMD" --aus "$E/r4" --name EM30_V5r4.EMD --json "$P/2_v5r4.json"
mv -f "$E/r4/EM30_V5r4.EMD" "$E/EM30_V5r4.EMD"
pruefe "$E/EM30_V5r4.EMD" 38562ced41061258f435be0d6598697b10ed241b09ba1a948007bc5bdf5993d5 "2 V5 Runde 4"
schritt 3_v5k "$T/r41_sichel" python v6kopf/kopf_orig_bau.py --basis "$E/EM30_V5r4.EMD" --aus "$E" --name EM30_V5k.EMD --json "$P/3_v5k.json"
pruefe "$E/EM30_V5k.EMD" 99703bbda76ebda1317555c0e7c5e5669938c32df408a8d9a2042db570bd679e "3 V6 Kopf (V5k)"
schritt 4_v5f "$T/r41_sichel" python v5koerper/koerper_bau.py --basis "$E/EM30_V5k.EMD" --aus "$E" --name EM30_V5f.EMD --json "$P/4_v5f.json"
pruefe "$E/EM30_V5f.EMD" 784c8f05dfc4ac57dfdf3ac52127ac2a4e900603043aafa0c6bada897a2d3019 "4 V5 wie Foto (V5f)"
schritt 5_v6 "$T/r41_sichel" python v6drei/drei_bau.py --basis "$E/EM30_V5f.EMD" --aus "$E" --name EM30_V6.EMD --json "$P/5_v6.json"
pruefe "$E/EM30_V6.EMD" 9e0e58ef09322b51213eaa4e55f6d04d980478cce4fe940f0f9b841c9e78c626 "5 V6 drei Punkte"
schritt 6_v7 "$T/r41_sichel" python v7hand/hand_bau.py --basis "$E/EM30_V6.EMD" --aus "$E" --name EM30_V7.EMD --json "$P/6_v7.json"
pruefe "$E/EM30_V7.EMD" "$PIN_SOLL" "6 V7 linke Hand = Pin release/em30_hangar.sha256"
asset="$(sha256sum "$BAUM/re15_port/shared_assets/PSX/EMD/EM30_HANGAR.EMD" | awk '{print $1}')"
if [ "$asset" = "$PIN_SOLL" ]; then echo "   Spiel-Asset EM30_HANGAR.EMD = Pin ($asset)"
else echo "   Spiel-Asset EM30_HANGAR.EMD $asset != Pin $PIN_SOLL"; rc=1; fi
if cmp -s "$E/EM30_V7.EMD" "$BAUM/re15_port/shared_assets/PSX/EMD/EM30_HANGAR.EMD"; then echo "   EM30_V7.EMD bytegleich Spiel-Asset"
else echo "   EM30_V7.EMD weicht vom Spiel-Asset ab"; rc=1; fi
if [ "$rc" = 0 ]; then echo "V7-KETTE-OK sha256 $PIN_SOLL"; else echo "V7-KETTE-ABWEICHUNG"; fi
exit "$rc"
