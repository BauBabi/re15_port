#!/usr/bin/env bash
# w_cuts.sh - Runde 41 Spur W: je Cut c (RE15_FORCE_CUT) ein Lauf bis Bild 200 in Raum $RAUM mit RE15_RAUMLICHT_LOG,
# zaehlt die Glanzpunkt-Vierecke, die der Raumlicht-Pfad im Bild projiziert (ok=1 und Mitte im 320x240-Schirm).
# Aufruf: w_cuts.sh <exe> <ausgabe> <raum_hex> <figur leon|elza> <cut...>
EXE="$1"; OUT="$2"; RAUM="$3"; FIG="$4"; shift 4
if [ "$FIG" = elza ]; then FENV=(RE15_TITLE_CONFIRM_MS=1500 RE15_PSELECT_AUTO=1 RE15_PSELECT_AUTO_SWITCH=1); ZIEL="${RAUM%?}1"; else FENV=(RE15_TITLE_CONFIRM_MS=1500 RE15_PSELECT_AUTO=1); ZIEL="$RAUM"; fi
for c in "$@"; do
  d="$OUT/c$c"; mkdir -p "$d"
  ( cd "$d" && env SDL_ASSERT=always_ignore RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_WINDOW_SCALE=1 "${FENV[@]}" RE15_DEBUG_JUMP=${RAUM}@250 \
      "RE15_PLAYER_POS=-7000,-25659,1976,1" RE15_FORCE_CUT=$c RE15_RAUMLICHT_LOG=1 "RE15_EXIT_AT=200#${ZIEL}" timeout 120 "$EXE" >/dev/null 2>&1 ) &
done
wait
for c in "$@"; do
  d="$OUT/c$c"
  python - "$d/debug.log" "$c" <<'PY'
import re, sys
n_an = n_sicht = 0
for l in open(sys.argv[1], errors="replace"):
    m = re.match(r"\[raumlicht\] F\d+ R\w+ #\d+ ok=(\d) \((-?\d+),(-?\d+)\)\((-?\d+),(-?\d+)\)\((-?\d+),(-?\d+)\)\((-?\d+),(-?\d+)\)", l)
    if not m: continue
    n_an += 1
    v = list(map(int, m.groups()[1:]))
    cx = sum(v[0::2]) / 4; cy = sum(v[1::2]) / 4
    if m.group(1) == "1" and 0 <= cx < 320 and 0 <= cy < 240: n_sicht += 1
ex = "EXIT" if "EXIT_AT" in open(sys.argv[1], errors="replace").read() else "kein EXIT"
print("Cut %s: Glanz-Vierecke %d, davon im Bild %d (%s)" % (sys.argv[2], n_an, n_sicht, ex))
PY
done
