"""Messung 8: ein Riegel, der NICHT sein eigenes Modell prueft.

Behauptung, die aus dem RDT allein folgt: liegt der Gegenstand einer Maske ganz in der
SCA-Sperrzelle x16100..17200 z1500..5900 (Hoehe gemessen -1575), dann hat KEIN Punkt des
Gegenstands ein Kamera-z groesser als der fernste Eckpunkt dieses Kastens. Eine
Maskentiefe, die darueber liegt, behauptet der Gegenstand stehe HINTER sich selbst —
das ist ohne jedes Tiefenmodell falsch.

Der Riegel ist damit bewusst LOCKER (die fernste Ecke ist die schwaechste Schranke, die
noch beweisbar ist) — er darf nicht strenger sein als der Befund.

Ausgegeben wird die ABDECKUNG: wie viele der geschriebenen Cuts der Riegel ueberhaupt
sieht, und wie viele Objekte im Projekt ihre Tiefe aus einer Zelle nehmen.
"""
import glob
import io
import json
import os
import struct
import sys
import numpy as np

D = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.abspath(os.path.join(D, "..", "..", "..", ".."))
sys.path.insert(0, os.path.join(ROOT, "re15_port/tools/maske"))
os.chdir(ROOT)
import geom
import geometrie
import maskenbild
import abnahme
import maske_aus_png

rdt, st = geom.load_rdt("re15_port/shared_assets/PSX", "ROOM1050")
cam = struct.unpack_from("<I", rdt, 0x24)[0]
R, t, H = geom.cut_view(rdt, cam, 1)
ZELLE = (16100, 1500, 1100, 4400)
HOEHE = -1575
X0, X1, Z0, Z1 = ZELLE[0], ZELLE[0] + ZELLE[2], ZELLE[1], ZELLE[1] + ZELLE[3]

vzmax = -1e9
ecke = None
for x in (X0, X1):
    for y in (0, HOEHE):
        for z in (Z0, Z1):
            vz = (x * R[6] + y * R[7] + z * R[8]) / 4096.0 + t[2]
            if vz > vzmax:
                vzmax, ecke = vz, (x, y, z)
print("Zelle x%d..%d z%d..%d Hoehe %d" % (X0, X1, Z0, Z1, HOEHE))
print("fernste Ecke %s: Kamera-z %.0f -> OT-Eimer %d"
      % (ecke, vzmax, geometrie.bucket(vzmax)))
GRENZE = int(geometrie.bucket(vzmax))
print("Riegel: jede Maskentiefe dieses Cuts muss <= %d sein." % GRENZE)
print()

SP = ("C:/Users/MJOEDI~1/AppData/Local/Temp/claude/c--workspace-git-reAi-v2/"
      "d917ef01-c258-42fe-ac63-7a1e284c7456/scratchpad/vorher/")
for name, msk, tim in (("VORHER", SP + "ROOM1050.MSK.vorher", SP + "ROOM1050_PRI01.TIM.vorher"),
                       ("NACHHER", "re15_port/shared_assets/PSX/MASKS/ROOM1050.MSK",
                        "re15_port/shared_assets/PSX/MASKS/ROOM1050_PRI01.TIM")):
    ms = maskenbild.masken(open(msk, "rb").read(), 1)
    idx = maskenbild.lies_tim(tim)[0]
    deck, tief = abnahme.deckung_und_tiefe(ms, idx)
    schlecht = deck & (tief > GRENZE)
    tiefen = [m[6] for m in ms]
    print("%-8s %d Rechtecke, Tiefen %d..%d | Texel ueber der Grenze: %d von %d"
          % (name, len(ms), min(tiefen), max(tiefen), int(schlecht.sum()), int(deck.sum())))
    if schlecht.any():
        ys, xs = np.nonzero(schlecht)
        print("         Kasten x%d..%d y%d..%d, Tiefen %d..%d, die %d Rechtecke:"
              % (xs.min(), xs.max(), ys.min(), ys.max(), tief[schlecht].min(), tief[schlecht].max(),
                 sum(1 for m in ms if m[6] > GRENZE)))
        print("         betroffene Rechtecke: %s"
              % ", ".join("#%d t=%d" % (i, m[6]) for i, m in enumerate(ms) if m[6] > GRENZE))

# --- ABDECKUNG -------------------------------------------------------------
print()
aus = json.load(io.open("analysis/esp_masken_2026-09-03/auswahl.json", encoding="utf-8"))
objekte = 0
mit_zelle = 0
cuts_mit_maske = len(glob.glob("re15_port/shared_assets/PSX/MASKS/*_PRI*.PBM"))
for room, cuts in aus.items():
    if not room.startswith("ROOM"):
        continue
    for cut, e in cuts.items():
        if not isinstance(e, dict):
            continue
        for o in e.get("objekte") or []:
            objekte += 1
            if o.get("zelle"):
                mit_zelle += 1
print("ABDECKUNG des Riegels:")
print("  geschriebene Cuts im Projekt:              %d" % cuts_mit_maske)
print("  Objekte in auswahl.json:                   %d" % objekte)
print("  Objekte mit ausdruecklicher Zelle:         %d" % mit_zelle)
print("  vom Riegel geprueft:                       1 Cut (ROOM1050 C1)")
print("  von den 4 Nutzer-Marken gesehen:           1 von 4 (Marke 3)")
