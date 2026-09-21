#!/usr/bin/env bash
# VORHER-Abzug an derselben Stelle: die vier Cut-7-Dateien des AUSLIEFERUNGSSTANDES
# (aus build/r19c/alt_MASKS, byte-genau gegen den Stand vor dieser Runde geprueft,
# protokoll/bau_alt.sh) kurz einsetzen, den Abzug fahren, danach den neuen Stand
# zurueckschreiben — mit Hash-Beleg vor und nach dem Tausch.
set -euo pipefail
cd "$(dirname "$0")/../../../.."
M=re15_port/shared_assets/PSX/MASKS
S=build/r19c/alt_MASKS
N=build/r19c/neu_MASKS
echo "--- Hashes NEU (Stand dieser Runde), vor dem Tausch"
md5sum $M/ROOM10D0.MSK $M/ROOM10D0_PRI07.TIM
for f in ROOM10D0.MSK ROOM10D0_PRI07.TIM ROOM10D0_PRI07.PBM ROOM10D0_PRI07.STAND; do
    cp "$S/$f" "$M/$f"
done
echo "--- Hashes ALT (Auslieferungsstand), eingesetzt"
md5sum $M/ROOM10D0.MSK $M/ROOM10D0_PRI07.TIM
bash analysis/befunde_2026-09-21/pri-runde19c-saum/protokoll/abzug_F3843.sh \
     build/r19c/abzug_vorher "${1:-90}" 120 200 260 20 || true
for f in ROOM10D0.MSK ROOM10D0_PRI07.TIM ROOM10D0_PRI07.PBM ROOM10D0_PRI07.STAND; do
    cp "$N/$f" "$M/$f"
done
echo "--- Hashes NEU, zurueckgeschrieben (muss dem ersten Block gleichen)"
md5sum $M/ROOM10D0.MSK $M/ROOM10D0_PRI07.TIM $M/ROOM10D0_PRI07.PBM $M/ROOM10D0_PRI07.STAND
