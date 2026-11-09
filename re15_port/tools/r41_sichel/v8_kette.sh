#!/usr/bin/env bash
# Runde 41 Spur SF V8 (Koerperbau nach Nutzer-Skizze, 2026-11-08): baut die Sichel-Form V8 (Spiel-Asset EMD/EM30_HANGAR.EMD) aus
# committeten Eingaben neu und prueft die sha256 gegen release/em30_hangar.sha256 (Exit 1 bei Abweichung).
#
#   bash re15_port/tools/r41_sichel/v8_kette.sh [ZIELORDNER]      (Standard: Scratchpad .../sfv8/kette)
#   Umgebung: KETTE_COMMIT (Stand der Werkzeuge, Standard HEAD), KETTE_REF2_COMMIT (wie v7_kette.sh),
#             V7_SOLL (sha256 der Stufe 6, Standard 319bbd17 = V7, Spiel-Asset v0.8.30)
#
# Nutzer 2026-11-08 abends (woertlich): "Das Gesicht vom 2 beinigen Birkin ist jetzt schon gut, aber mit Schwanz und sowas bin ich immer
# noch 0 zufrieden.... Es ist doch eigentlich KLAR erkennbar. Siehe Darstellung.png und Darstellung_Vorlage.png. Man sieht ganz klar den
# Koerper Aufbau...." und "Uebernehme die die Textur Sachen die du dafuer brauchst vom originalen Birkin von Resident Evil 1.5".
# Kein RE-Befund: die Sichel-Form gibt es auf keiner Disc (Dossier analysis/befunde_runde41/SF_sichel.md, Abschnitt "V8 Koerperbau nach
# Nutzer-Skizze (2026-11-08)"; Messblatt SF_MESSBLATT_V8.md).
#
# Schritte:
#   1-6 v7_kette.sh (Stufe 6 = V7, sha256 319bbd17; dessen Pin-Pruefung wird hier durch V7_SOLL ersetzt, weil der Pin jetzt V8 ist)
#   7   v8/v8_bau.py --stufen srwa   + v8/kette_v8.json, v8/ruecken_v8.json, v8/adern_v8.json, Original-EM30 (CDEMD0.EMS)
#       (Runde 4: Stufe w = wurzel_r4.py statt e, Texturen adern_r4.py modus original_r4;
#        Runde 5: Geometrie wie Runde 4, Texturen adern_r5.py modus original_r5 - Brust auf Original-Zeile 481, fast weisse Texel
#        ersetzt, blass_r5)
#                                    -> EM30_V8.EMD = Pin (V8)
set -u
HIER="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BAUM="$(git -C "$HIER" rev-parse --show-toplevel)" || exit 2
ZIEL="${1:-C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/sfv8/kette}"
V7_SOLL="${V7_SOLL:-319bbd17f10ba0cdd703861c6d844a00fb414d82b513e77057bf0ff80fda65f6}"
PIN="$BAUM/release/em30_hangar.sha256"
PIN_SOLL="$(awk 'NR==1{print $1}' "$PIN")"
[ -n "$PIN_SOLL" ] || { echo "FEHLER: Pin $PIN leer"; exit 2; }
mkdir -p "$ZIEL"

echo "== V8-Kette: Stufen 1-6 (v7_kette.sh), Ziel $ZIEL"
# v7_kette.sh vergleicht Stufe 6 mit dem Pin; der Pin ist seit dem Einbau V8 die V8-Summe, sein Exit-Code zaehlt hier deshalb nicht -
# Stufe 6 wird gegen V7_SOLL geprueft
bash "$HIER/v7_kette.sh" "$ZIEL" > "$ZIEL/v7_kette.txt" 2>&1
v7="$(sha256sum "$ZIEL/emd/EM30_V7.EMD" 2>/dev/null | awk '{print $1}')"
if [ "$v7" = "$V7_SOLL" ]; then echo "   6 V7: EM30_V7.EMD sha256 $v7 OK"
else echo "   6 V7: EM30_V7.EMD sha256 ${v7:-fehlt} ABWEICHUNG (Soll $V7_SOLL)"; tail -12 "$ZIEL/v7_kette.txt"; echo "V8-KETTE-ABWEICHUNG"; exit 1; fi
grep -E "^   [1-5] " "$ZIEL/v7_kette.txt"

T="$ZIEL/baum/re15_port/tools/r41_sichel/v8"
[ -f "$T/v8_bau.py" ] || { echo "FEHLER: $T/v8_bau.py fehlt im Schattenbaum"; exit 2; }
echo "-- 7_v8: python v8_bau.py --basis EM30_V7.EMD --stufen srwa"
( cd "$T" && python v8_bau.py --basis "$ZIEL/emd/EM30_V7.EMD" --aus "$ZIEL/emd" --name EM30_V8.EMD --stufen srwa \
      --json "$ZIEL/prot/7_v8.json" ) > "$ZIEL/prot/7_v8.txt" 2>&1 || { echo "FEHLER: Schritt 7_v8 (Protokoll $ZIEL/prot/7_v8.txt)"; tail -5 "$ZIEL/prot/7_v8.txt"; exit 1; }
rc=0
v8="$(sha256sum "$ZIEL/emd/EM30_V8.EMD" | awk '{print $1}')"
if [ "$v8" = "$PIN_SOLL" ]; then echo "   7 V8: EM30_V8.EMD sha256 $v8 OK (= Pin release/em30_hangar.sha256)"
else echo "   7 V8: EM30_V8.EMD sha256 $v8 ABWEICHUNG (Pin $PIN_SOLL)"; rc=1; fi
asset="$(sha256sum "$BAUM/re15_port/shared_assets/PSX/EMD/EM30_HANGAR.EMD" | awk '{print $1}')"
if [ "$asset" = "$PIN_SOLL" ]; then echo "   Spiel-Asset EM30_HANGAR.EMD = Pin ($asset)"
else echo "   Spiel-Asset EM30_HANGAR.EMD $asset != Pin $PIN_SOLL"; rc=1; fi
if cmp -s "$ZIEL/emd/EM30_V8.EMD" "$BAUM/re15_port/shared_assets/PSX/EMD/EM30_HANGAR.EMD"; then echo "   EM30_V8.EMD bytegleich Spiel-Asset"
else echo "   EM30_V8.EMD weicht vom Spiel-Asset ab"; rc=1; fi
if [ "$rc" = 0 ]; then echo "V8-KETTE-OK sha256 $PIN_SOLL"; else echo "V8-KETTE-ABWEICHUNG"; fi
exit "$rc"
