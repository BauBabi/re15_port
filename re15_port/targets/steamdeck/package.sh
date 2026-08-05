#!/usr/bin/env bash
# =============================================================================
# RE1.5-Port — Paket fuer das Steam Deck schnueren
# =============================================================================
# Legt einen in sich geschlossenen Ordner an, der auf das Deck kopiert wird:
#
#   re15-deck/
#     re15_pc          das Linux-Binary (SDL2 statisch eingebettet)
#     re15-deck.sh     Startskript -> DAS wird in Steam eingetragen
#     assets/          der Asset-Baum (~283 MB)
#     README.txt       Kurzanleitung fuer das Geraet
#
# Aufruf (nach build.sh):
#     ./re15_port/targets/steamdeck/package.sh [Zielordner]
# =============================================================================
set -euo pipefail

HERE="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
REPO="$(cd "$HERE/../../.." && pwd)"
OUT="${1:-$HERE/dist/re15-deck}"

BIN="$HERE/build/platform/pc/re15_pc"
ASSETS="$REPO/re15_port/shared_assets/PSX"

[[ -x "$BIN"    ]] || { echo "FEHLER: $BIN fehlt — zuerst build.sh ausfuehren." >&2; exit 1; }
[[ -d "$ASSETS" ]] || { echo "FEHLER: Asset-Baum $ASSETS fehlt." >&2; exit 1; }

echo "== Paket wird erstellt: $OUT =="
rm -rf "$OUT"
mkdir -p "$OUT"

cp "$BIN"               "$OUT/re15_pc"
cp "$HERE/re15-deck.sh" "$OUT/re15-deck.sh"
chmod +x "$OUT/re15_pc" "$OUT/re15-deck.sh"

echo "   Assets kopieren (~283 MB) ..."
cp -r "$ASSETS" "$OUT/assets"

cat > "$OUT/README.txt" <<'EOF'
RE1.5 Rebuilt — Steam Deck
==========================

Installieren
------------
1. Diesen Ordner auf das Deck kopieren, z. B. nach
      /home/deck/Games/re15-deck
   (SD-Karte geht auch; der Spielstand landet trotzdem im Home-Verzeichnis.)
2. Desktop-Modus -> Steam -> Bibliothek -> "Nicht-Steam-Spiel hinzufuegen"
3. "Durchsuchen" -> re15-deck.sh auswaehlen  (NICHT re15_pc direkt!)
4. Zurueck in den Spielmodus — der Eintrag erscheint in der Bibliothek.

Steuerung
---------
Das Spiel liest ein Gamepad ueber SDL2; Steam Input mappt die Deck-Tasten
automatisch. Bestaetigen ist die X-Taste (links), Abbrechen die A-Taste
(unten) — RE1.5 bestaetigt game-weit mit Quadrat, nicht mit Kreuz.
Wer lieber Kreis bestaetigt: im Spiel OPTIONS -> Tastenbelegung Typ B/C.

Wo liegt was
------------
Spielstand, Log und Screenshots:  ~/.local/share/re15/
  re15_card.mcr   Speicherkarten-Abbild (echtes PSX-Format)
  debug.log       Diagnoseausgabe
  shots/          Screenshots

Laeuft es nicht?
----------------
Im Desktop-Modus ein Terminal oeffnen und ./re15-deck.sh starten — das Skript
meldet fehlende Assets oder ein fehlendes Binary im Klartext, danach steht die
Ursache in debug.log.
EOF

SIZE=$(du -sh "$OUT" | cut -f1)
echo
echo "== Fertig: $OUT  ($SIZE) =="
echo "   Auf das Deck kopieren und dort re15-deck.sh in Steam eintragen."
