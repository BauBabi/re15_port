#!/usr/bin/env bash
# =============================================================================
# RE1.5 Port — Startskript (Linux x64 / Steam Deck)
# =============================================================================
# DIESE Datei wird gestartet (bzw. auf dem Deck als Nicht-Steam-Spiel
# eingetragen) — NICHT das Binary direkt. Sie erledigt vier Dinge, ohne die
# das Paket auf einem Linux-System nachweislich falsch laeuft:
#
#   1. ASSET-WURZEL setzen (RE15_ASSET_ROOT / RE15_CD_ROOT).
#      Ohne diese beiden greift im Port nur die cwd-relative Notkette
#      (platform/pc/main.c, pc_read_shared) — die findet zwar
#      shared_assets/PSX, aber NICHT die Geschwister-Texturen unter
#      shared_assets/extracted_fx (dort gesucht als "$RE15_CD_ROOT/../
#      extracted_fx/..."). Folge ohne Export: Blut, Muendungsfeuer, Rauch
#      und Huelsen rendern gar nicht.
#   2. SDL-Render-Backend auf opengles2 zwingen.
#      Der Port zeichnet Fades-to-black, Cutscene-Balken und Schatten mit
#      einem SUBTRAKTIVEN Custom-Blend (SDL_BLENDOPERATION_REV_SUBTRACT).
#      Auf dem Deck (Mesa) kann das per Default gewaehlte "opengl"-Backend
#      diesen Blend NICHT: SDL_SetRenderDrawBlendMode() schlaegt still fehl,
#      der vorherige BLEND-Modus bleibt stehen und die helle Graustufen-
#      Quelle wird DECKEND gezeichnet -> Fades/Balken erscheinen WEISS.
#      Auf echter Deck-Hardware nachgemessen (re15_port/tools/sdl_blend_probe.c,
#      2026-08-11): opengles2 rc=0; opengl/opengles/vulkan/software rc=-1
#      ("That operation is not supported").
#      Wer selbst ein Backend gesetzt hat, behaelt seine Wahl.
#   3. Ein BESCHREIBBARES Arbeitsverzeichnis waehlen. Der Port legt
#      Spielstand (re15_card.mcr), debug.log und Screenshots im
#      Arbeitsverzeichnis an. Liegt der Paketordner beschreibbar (Normalfall,
#      z.B. entpackt im Home), bleibt alles wie bisher NEBEN dem Binary —
#      bestehende Spielstaende wandern also nicht weg. Nur wenn der Ordner
#      schreibgeschuetzt ist (z.B. read-only gemountete Karte), wird nach
#      ~/.local/share/re15 ausgewichen.
#   4. Ausfuehrbarkeit sicherstellen — manche Entpacker (Ark, Windows-Tools)
#      verwerfen das x-Bit.
#
# Start:  ./run.sh
# =============================================================================
set -euo pipefail

HERE="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BIN="$HERE/re15_pc"
ASSETS="$HERE/shared_assets/PSX"
FX="$HERE/shared_assets/extracted_fx"

[[ -f "$BIN" ]] || { echo "FEHLER: re15_pc fehlt neben diesem Skript ($HERE)." >&2; exit 1; }
[[ -x "$BIN" ]] || chmod +x "$BIN" 2>/dev/null || true
[[ -x "$BIN" ]] || { echo "FEHLER: re15_pc ist nicht ausfuehrbar: chmod +x re15_pc" >&2; exit 1; }

[[ -d "$ASSETS" ]] || {
    echo "FEHLER: Asset-Baum fehlt: $ASSETS" >&2
    echo "        Das Paket ist unvollstaendig entpackt (Split-Zip: BEIDE Volumes" >&2
    echo "        .z01 UND .zip in denselben Ordner legen, dann die .zip oeffnen)." >&2
    exit 1
}
[[ -d "$FX" ]] || echo "WARNUNG: $FX fehlt — Treffer-/Schusseffekte rendern nicht." >&2

export RE15_ASSET_ROOT="$ASSETS"
export RE15_CD_ROOT="$ASSETS"
export SDL_RENDER_DRIVER="${SDL_RENDER_DRIVER:-opengles2}"

if [[ -w "$HERE" ]]; then
    WORK="$HERE"
else
    WORK="${XDG_DATA_HOME:-$HOME/.local/share}/re15"
    mkdir -p "$WORK"
fi
cd "$WORK"

echo "RE1.5 Port"
echo "  Binary : $BIN"
echo "  Assets : $ASSETS"
echo "  Daten  : $WORK   (Spielstand re15_card.mcr, debug.log, shots/)"

exec "$BIN" "$@"
