"""Nachbau der ausgelieferten ROOM10F0-Sektionen C4/C5 mit der ALTEN Kette.

⛔ EICHUNG: ohne den Nachweis, dass dieser Weg die ausgelieferte Sektion BITGENAU
   reproduziert, ist jede Messung einer Aenderung wertlos. Der Nachbau ruft genau die
   Funktionen, die raum.main() fuer rid >= 0x2000 ruft (objekt_regionen ->
   anwenden.bau_objektweise) - fuer STAGE1 sperrt main() das ueber main_p2/
   P2_UNANGETASTET, die Sektionen SELBST stammen aber aus diesem Weg.
"""
import json
import os
import struct
import sys

import numpy as np

HIER = os.path.dirname(os.path.abspath(__file__))
WURZEL = os.path.abspath(os.path.join(HIER, "..", "..", "..", ".."))
sys.path.insert(0, os.path.join(WURZEL, "re15_port", "tools", "maske"))
os.chdir(WURZEL)

import raum
import anwenden
import geom
import maskenbild
from geom import load_bg, load_rdt

CD = "re15_port/shared_assets/PSX"
PPM = "build/bg_ppm"
BLATT = "build/blaetter"


def sektion_aus_container(pfad, cut):
    b = open(pfad, "rb").read()
    assert b[:4] == b"R15M"
    _, nc = struct.unpack_from("<II", b, 4)
    offs = struct.unpack_from("<%dI" % nc, b, 12)
    ends = sorted([o for o in offs if o] + [len(b)])
    if not offs[cut]:
        return None
    return b[offs[cut]:min(x for x in ends if x > offs[cut])]


def bau(room, cut, eintrag_json, out_dir):
    rdt, _ = load_rdt(CD, room)
    cam = struct.unpack_from("<I", rdt, 0x24)[0]
    e = raum.eintrag(eintrag_json)
    bg = load_bg(PPM, int(room[4:], 16), cut)
    objekte = raum.objekt_regionen(room, cut, e, PPM, BLATT)
    os.makedirs(out_dir, exist_ok=True)
    res = anwenden.bau_objektweise(rdt, cam, cut, objekte, bg, out_dir, room)
    flaeche = np.zeros((240, 320), bool)
    for e_ in objekte:
        flaeche |= e_[1]
    return res[0], res[1], flaeche, objekte, rdt, cam, bg


def deckung(sec, rdt1, cut, timpfad):
    blob = geom.pack_container({cut: sec}, rdt1)
    ms = maskenbild.masken(blob, cut) or []
    t = maskenbild.lies_tim(timpfad)
    idx = t[0]
    deck = np.zeros((240, 320), bool)
    tief = np.full((240, 320), np.iinfo(np.int32).max, np.int64)
    for (sx, sy, X, Y, w, h, dep) in ms:
        x0, x1 = max(0, X), min(320, X + w)
        y0, y1 = max(0, Y), min(240, Y + h)
        if x1 <= x0 or y1 <= y0:
            continue
        sub = idx[sy + (y0 - Y):sy + (y1 - Y), sx + (x0 - X):sx + (x1 - X)]
        if sub.shape != (y1 - y0, x1 - x0):
            continue
        m = sub != 0
        deck[y0:y1, x0:x1] |= m
        blk = tief[y0:y1, x0:x1]
        np.copyto(blk, np.minimum(blk, dep), where=m)
        tief[y0:y1, x0:x1] = blk
    return deck, tief, ms


if __name__ == "__main__":
    aus = json.load(open(raum.AUSWAHL, encoding="utf-8"))
    for cut in (4, 5):
        sec, n, flaeche, objekte, rdt, cam, bg = bau(
            "ROOM10F0", cut, aus["ROOM10F0"][str(cut)], "build/r22/nachbau")
        alt = sektion_aus_container(os.path.join(CD, "MASKS", "ROOM10F0.MSK"), cut)
        altim = open(os.path.join(CD, "MASKS", "ROOM10F0_PRI%02d.TIM" % cut), "rb").read()
        neuim = open(os.path.join("build/r22/nachbau", "ROOM10F0_PRI%02d.TIM" % cut), "rb").read()
        print("CUT %d: Rechtecke %d | Sollflaeche %d px" % (cut, n, int(flaeche.sum())))
        print("   Sektion  ausgeliefert %d B, nachgebaut %d B, BITGLEICH=%s"
              % (len(alt), len(sec), alt == sec))
        print("   TIM      ausgeliefert %d B, nachgebaut %d B, BITGLEICH=%s"
              % (len(altim), len(neuim), altim == neuim))
