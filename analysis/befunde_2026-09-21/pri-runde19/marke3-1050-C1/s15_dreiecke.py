"""Messung 11 — OHNE Naeherung. Der Zeichner entscheidet je DREIECK, nicht je Bildzeile
(render_pc.c:939/968: eine Maske wird vor dem Dreieck gemalt, solange
re15_pri_mask_camera_z(depth) > tri_depth; bei kleiner-gleich liegt sie darueber).

Die Marke selbst nennt die Spanne der gezeichneten Dreiecke:
  'Gezeichnete Dreiecke: Kamera-z 4704..5665 (Maskentiefe 73..88)'  (befund.log F239)

Damit laesst sich die Frage ohne jedes Tiefenprofil entscheiden:
  * eine Maske verdeckt JEDES Dreieck Leons, wenn depth <= bucket(4704)-1 = 72
  * eine Maske verdeckt KEIN Dreieck Leons, wenn depth >  bucket(5665)-1 = 87
Dazwischen verdeckt sie einen Teil.
"""
import os
import sys
import numpy as np

D = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.abspath(os.path.join(D, "..", "..", "..", ".."))
sys.path.insert(0, os.path.join(ROOT, "re15_port/tools/maske"))
os.chdir(ROOT)
import geometrie
import maskenbild
import abnahme

SP = ("C:/Users/MJOEDI~1/AppData/Local/Temp/claude/c--workspace-git-reAi-v2/"
      "d917ef01-c258-42fe-ac63-7a1e284c7456/scratchpad/vorher/")
TRI_NAH, TRI_FERN = 4704, 5665
b_nah, b_fern = geometrie.bucket(TRI_NAH), geometrie.bucket(TRI_FERN)
print("Dreiecke der Marke: Kamera-z %d..%d -> OT-Eimer %d..%d" % (TRI_NAH, TRI_FERN, b_nah, b_fern))
print("  Maske verdeckt ALLE Dreiecke, wenn Tiefe <= %d" % (b_nah - 1))
print("  Maske verdeckt KEIN Dreieck,  wenn Tiefe >= %d" % b_fern)
print()


def lade(msk, tim):
    ms = maskenbild.masken(open(msk, "rb").read(), 1)
    idx = maskenbild.lies_tim(tim)[0]
    return abnahme.deckung_und_tiefe(ms, idx)


dA, tA = lade(SP + "ROOM1050.MSK.vorher", SP + "ROOM1050_PRI01.TIM.vorher")
dB, tB = lade("re15_port/shared_assets/PSX/MASKS/ROOM1050.MSK",
              "re15_port/shared_assets/PSX/MASKS/ROOM1050_PRI01.TIM")
gemalt = np.load(os.path.join(D, "gemalt.npy"))

KX0, KX1, KY0, KY1 = 181, 219, 122, 190
print("Im Spielerkasten x%d..%d y%d..%d, je Bildpunkt die NAECHSTE Maskentiefe:" % (KX0, KX1, KY0, KY1))
print("  Quelle   Texel  verdeckt ALLE (<=%d)  verdeckt TEILS  verdeckt KEIN (>=%d)"
      % (b_nah - 1, b_fern))
for name, d, tf in (("vorher", dA, tA), ("nachher", dB, tB)):
    box = d[KY0:KY1 + 1, KX0:KX1 + 1]
    tb = tf[KY0:KY1 + 1, KX0:KX1 + 1]
    alle = int((box & (tb <= b_nah - 1)).sum())
    kein = int((box & (tb >= b_fern)).sum())
    teils = int(box.sum()) - alle - kein
    print("  %-8s %5d %14d %16d %16d" % (name, int(box.sum()), alle, teils, kein))

print()
print("Nur die SICHTBAREN Figurpunkte (Screenshot != Hintergrund) im Kasten:")
for name, d, tf in (("vorher", dA, tA), ("nachher", dB, tB)):
    m = np.zeros((240, 320), bool)
    m[KY0:KY1 + 1, KX0:KX1 + 1] = True
    f = gemalt & m
    alle = int((f & d & (tf <= b_nah - 1)).sum())
    kein = int((f & d & (tf >= b_fern)).sum())
    ohne = int((f & ~d).sum())
    teils = int((f & d).sum()) - alle - kein
    print("  %-8s Figurpunkte %d | mit Maske %d -> ALLE %d, TEILS %d, KEIN %d | ohne Maske %d"
          % (name, int(f.sum()), int((f & d).sum()), alle, teils, kein, ohne))

print()
print("Je Spalte die NAECHSTE Tiefe im Kasten und das Urteil ohne Naeherung:")
print("  x   vorher  Urteil          | nachher  Urteil")
for x in range(180, 222, 2):
    col = slice(KY0, KY1 + 1)
    out = []
    for d, tf in ((dA, tA), (dB, tB)):
        t = tf[col, x][d[col, x]]
        if t.size == 0:
            out.append(("-", "keine Maske"))
            continue
        mn = int(t.min())
        u = ("ALLE Dreiecke" if mn <= b_nah - 1
             else ("KEIN Dreieck" if mn >= b_fern else "nur die fernen"))
        out.append((str(mn), u))
    print("  %3d  %6s  %-15s | %6s  %s" % (x, out[0][0], out[0][1], out[1][0], out[1][1]))
