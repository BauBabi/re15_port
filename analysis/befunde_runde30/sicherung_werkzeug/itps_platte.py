#!/usr/bin/env python3
"""HINTERGRUNDPLATTE der Item-Bilder (Fortsetzungs-Agent).

Frage: Was liegt UNTER dem Gegenstand? Ein Ersatzbild (Weg 2) braucht den Hintergrund auch
dort, wo im ausgelieferten Bild 0x40 der alte Gegenstand liegt. Antwort aus den Daten selbst:
alle Item-Bilder teilen sich eine Platte (Rahmen + blauer Verlauf). Je Bildpunkt wird das
haeufigste Farbwort ueber alle Bloecke genommen, die denselben Kopf tragen wie Block 0x40.

Ausgabe: build/r30_sicherung/platte_worte.npy (72x112 u16), platte_stuetze.npy (Anzahl der
Bloecke, die das Wort tragen), platte_4x.png, platte_stuetze.png

    python analysis/befunde_runde30/sicherung_werkzeug/itps_platte.py
"""
import os
import struct

import numpy as np
from PIL import Image

REPO = os.path.abspath(os.path.join(os.path.dirname(__file__), "..", "..", ".."))
PSX = os.path.join(REPO, "re15_port", "shared_assets", "PSX")
ZIEL = os.path.join(REPO, "build", "r30_sicherung")
S, W, H = 0x3000, 112, 72


def rgb(w):
    return np.stack([(w & 31) << 3, ((w >> 5) & 31) << 3, ((w >> 10) & 31) << 3], -1).astype(np.uint8)


def main():
    d = open(os.path.join(PSX, "ITEM", "ITPS.ITP"), "rb").read()
    n = len(d) // S
    kopf40 = d[0x40 * S:0x40 * S + 0x14]
    print("Kopf Block 0x40 (20 B @Datei 0x%X): %s" % (0x40 * S, kopf40.hex(" ")))
    bilder, wer = [], []
    for k in range(n):
        b = d[k * S:(k + 1) * S]
        if b[:0x14] != kopf40:
            continue
        clut = np.array(struct.unpack_from("<256H", b, 0x14), np.uint16)
        idx = np.frombuffer(b, np.uint8, W * H, 0x220).reshape(H, W)
        bilder.append(clut[idx])
        wer.append(k)
    a = np.stack(bilder)                       # (n, H, W)
    print("%d von %d Bloecken tragen denselben Kopf: %s"
          % (len(wer), n, " ".join("%02X" % k for k in wer)))
    platte = np.zeros((H, W), np.uint16)
    stuetze = np.zeros((H, W), np.int32)
    for y in range(H):
        for x in range(W):
            w_, z_ = np.unique(a[:, y, x], return_counts=True)
            i = int(z_.argmax())
            platte[y, x] = w_[i]
            stuetze[y, x] = z_[i]
    np.save(os.path.join(ZIEL, "platte_worte.npy"), platte)
    np.save(os.path.join(ZIEL, "platte_stuetze.npy"), stuetze)
    Image.fromarray(rgb(platte)).resize((W * 4, H * 4), Image.NEAREST).save(
        os.path.join(ZIEL, "platte_4x.png"))
    st = (stuetze * 255 // len(wer)).astype(np.uint8)
    Image.fromarray(st).resize((W * 4, H * 4), Image.NEAREST).save(
        os.path.join(ZIEL, "platte_stuetze.png"))
    print("Stuetze je Bildpunkt: min %d, Median %d, max %d (von %d Bloecken)"
          % (stuetze.min(), int(np.median(stuetze)), stuetze.max(), len(wer)))
    for s in (2, 3, 5, 10):
        print("  Bildpunkte mit Stuetze < %2d: %d" % (s, int((stuetze < s).sum())))
    worte, zahl = np.unique(platte, return_counts=True)
    o = np.argsort(-zahl)
    print("Plattenworte: %d verschiedene; die haeufigsten: %s"
          % (len(worte), ", ".join("0x%04X x%d" % (worte[i], zahl[i]) for i in o[:8])))
    # Zeilenprofil der Innenflaeche (ohne Rahmen)
    print("Zeilenprofil Spalte 8 / 56 / 104 (Wort):")
    for y in (4, 12, 24, 36, 48, 60, 68):
        print("   y=%2d  0x%04X  0x%04X  0x%04X   Stuetze %d %d %d"
              % (y, platte[y, 8], platte[y, 56], platte[y, 104],
                 stuetze[y, 8], stuetze[y, 56], stuetze[y, 104]))


if __name__ == "__main__":
    main()
