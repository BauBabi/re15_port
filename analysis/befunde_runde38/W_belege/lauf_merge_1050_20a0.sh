#!/usr/bin/env bash
# Runde 38 Spur W, Fortsetzung 2026-10-17 (Dossier §9.4): Messung NACH dem Merge von r38/cursor an der exe.
#   m1050_cut5   ROOM1050 Rolltor obj 0 (Obj_model_set @0x00C36) in Cut 5 — zweiter Original-Fall (§4b):
#                Original verwirft alle 15 Vierecke an NCLIP (`bgez` @0x800257c0) -> Messschiene muss
#                "cut=5 obj=0 gezeichnet=0 nclip=15 otz=0 gpu=0" melden. Spieler an der Original-Messstelle
#                (16050, 0, -16200) (orig1050_log.txt "Zone 4->5"), Kamera Cut 5 erzwungen.
#   m20a0_cut2   ROOM20A0 Cut 2 (Hinweis Spur A §4.4: Rolllaeden obj 4/5 Typ 3, sub01 je Cut) — Messschiene je Objekt.
# Aufruf: bash lauf_merge_1050_20a0.sh   (Laeufer lauf_w.sh, Messkopie re15_pc_w38m.exe)
set -u
HIER="$(cd "$(dirname "$0")" && pwd)"
export W38_SUFFIX=m
export W38_SP="${W38_SP:-C:/Users/MJOEDI~1/AppData/Local/Temp/claude/c--workspace-git-reAi-v2/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/W38b}"
bash "$HIER/lauf_w.sh" m1050_cut5 - "RE15_DEBUG_JUMP=1050@250" "RE15_PLAYER_POS=16050,-16200,0,0" \
     "RE15_FORCE_CUT=5" "RE15_FRAMEDUMP=400:fd_000400.ppm" "RE15_EXIT_AT=402#1050"
bash "$HIER/lauf_w.sh" m20a0_cut2 - "RE15_DEBUG_JUMP=20A0@250" "RE15_FORCE_CUT=2" \
     "RE15_FRAMEDUMP=400:fd_000400.ppm" "RE15_EXIT_AT=402#20A0"
