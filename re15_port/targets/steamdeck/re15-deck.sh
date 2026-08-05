#!/usr/bin/env bash
# =============================================================================
# RE1.5-Port — Startskript fuer das Steam Deck
# =============================================================================
# Genau DIESE Datei wird in Steam als "Nicht-Steam-Spiel" eingetragen (nicht
# das Binary direkt). Sie erledigt drei Dinge, die das Binary selbst nicht tut:
#
#   1. ASSET-WURZEL setzen. Der Port sucht Assets sonst relativ zum
#      Arbeitsverzeichnis; hier wird sie explizit auf den mitgelieferten Baum
#      gezeigt (RE15_ASSET_ROOT, ausgewertet in platform/pc/src/asset_pc.c).
#   2. In ein BESCHREIBBARES Verzeichnis wechseln. Der Port legt Spielstand
#      (re15_card.mcr), debug.log und Screenshots im Arbeitsverzeichnis an —
#      auf SteamOS ist das Wurzeldateisystem schreibgeschuetzt, deshalb
#      ~/.local/share/re15.
#   3. Die Spielstaende ueberleben lassen: liegt das Paket auf einer SD-Karte,
#      bleibt der Spielstand trotzdem im Home-Verzeichnis.
#
# Aufruf (Desktop-Modus zum Testen):
#     ./re15-deck.sh
# In Steam:  Rechtsklick Bibliothek -> Nicht-Steam-Spiel hinzufuegen -> diese Datei
# =============================================================================
set -euo pipefail

HERE="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

# --- Binary finden: erst neben dem Skript (Paket), dann im Bauordner ---------
if   [[ -x "$HERE/re15_pc" ]];                                  then BIN="$HERE/re15_pc"
elif [[ -x "$HERE/build/platform/pc/re15_pc" ]];                then BIN="$HERE/build/platform/pc/re15_pc"
else
    echo "FEHLER: re15_pc nicht gefunden (weder neben diesem Skript noch in build/)." >&2
    echo "        Zuerst bauen: targets/steamdeck/build.sh" >&2
    exit 1
fi

# --- Assets finden: erst neben dem Skript (Paket), dann im Repo --------------
if   [[ -d "$HERE/assets" ]];                                   then ASSETS="$HERE/assets"
elif [[ -d "$HERE/../../shared_assets/PSX" ]];                  then ASSETS="$(cd "$HERE/../../shared_assets/PSX" && pwd)"
else
    echo "FEHLER: Asset-Baum nicht gefunden (assets/ neben dem Skript oder" >&2
    echo "        re15_port/shared_assets/PSX im Repo)." >&2
    exit 1
fi

export RE15_ASSET_ROOT="$ASSETS"
export RE15_CD_ROOT="$ASSETS"

# --- Beschreibbares Arbeitsverzeichnis (Spielstand, Log, Screenshots) -------
DATA="${XDG_DATA_HOME:-$HOME/.local/share}/re15"
mkdir -p "$DATA"
cd "$DATA"

echo "RE1.5 Rebuilt"
echo "  Binary : $BIN"
echo "  Assets : $ASSETS"
echo "  Daten  : $DATA   (Spielstand re15_card.mcr, debug.log, shots/)"

exec "$BIN" "$@"
