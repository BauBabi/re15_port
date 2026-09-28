#!/usr/bin/env python3
"""Baut ein RE15_INPUT_SCRIPT aus (Tasten, Bilder)-Paaren in BILDERN statt Sekunden.
Der Parser (platform/pc/src/input_pc.c script_parse_once) rechnet n = (int)(sek*fps + 0.5)
mit fps 30; hier wird sek = bilder/30 mit 4 Nachkommastellen geschrieben, das ergibt exakt
`bilder` Ticks.  Aufruf: skript_bau.py "XU:96" "W:15" "A:3" ... -> eine Zeile auf stdout.
Wiederholung: "14*W:39,A:3" wiederholt die Gruppe 14 mal."""
import sys
teile = []
for arg in sys.argv[1:]:
    rep = 1
    if "*" in arg:
        n, arg = arg.split("*", 1)
        rep = int(n)
    gruppe = []
    for tok in arg.split(","):
        k, b = tok.split(":")
        gruppe.append(f"{k}{int(b) / 30:.4f}")
    teile += gruppe * rep
print(",".join(teile))
