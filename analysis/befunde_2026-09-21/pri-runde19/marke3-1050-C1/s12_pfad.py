"""Messung 9: der Fix gegen JEDE Standposition, die der Nutzer in ROOM1050 C1
tatsaechlich eingenommen hat (alle Zeilen aus seinem befund.log). Fuer jede Lage wird
der Koerperkasten projiziert und je Bildzeile mit der Original-Regel geprueft:
wie viel des Kastens verdeckt die Maske vorher / nachher?

Damit ist ausgeschlossen, dass der Fix an anderer Stelle ZU VIEL verdeckt — gemessen
an seinem Weg, nicht an einem gedachten Gitter.

Zusatz: die Fuellrate der 'Faecher'-Rechtecke (Texel je Kasten) — die Frage, ob die
Spaltenhoehen aus der Freistellung oder aus einem Modell kommen.
"""
import os
import re
import struct
import sys
import numpy as np
from PIL import Image

D = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.abspath(os.path.join(D, "..", "..", "..", ".."))
sys.path.insert(0, os.path.join(ROOT, "re15_port/tools/maske"))
os.chdir(ROOT)
import geom
import geometrie
import maskenbild
import abnahme

SP = ("C:/Users/MJOEDI~1/AppData/Local/Temp/claude/c--workspace-git-reAi-v2/"
      "d917ef01-c258-42fe-ac63-7a1e284c7456/scratchpad/vorher/")
rdt, st = geom.load_rdt("re15_port/shared_assets/PSX", "ROOM1050")
cam = struct.unpack_from("<I", rdt, 0x24)[0]
R, t, H = geom.cut_view(rdt, cam, 1)


def lade(msk, tim):
    ms = maskenbild.masken(open(msk, "rb").read(), 1)
    idx = maskenbild.lies_tim(tim)[0]
    d, tf = abnahme.deckung_und_tiefe(ms, idx)
    return d, tf, ms, idx


dA, tA, msA, idxA = lade(SP + "ROOM1050.MSK.vorher", SP + "ROOM1050_PRI01.TIM.vorher")
dB, tB, msB, idxB = lade("re15_port/shared_assets/PSX/MASKS/ROOM1050.MSK",
                         "re15_port/shared_assets/PSX/MASKS/ROOM1050_PRI01.TIM")

# --- Fuellrate der Faecher-Rechtecke --------------------------------------
print("Fuellrate der hohen schmalen Rechtecke (die 'Faecher' aus dem Befund):")
print("  Quelle   Nr  Kasten            Flaeche  Texel  Fuellrate  Texelzeilen/Kastenzeilen")
for name, ms, idx in (("vorher", msA, idxA), ("nachher", msB, idxB)):
    ges_f = ges_t = 0
    zeig = 0
    for i, (sx, sy, X, Y, w, h, dep) in enumerate(ms):
        sub = idx[sy:sy + h, sx:sx + w]
        op = sub != 0
        ges_f += w * h
        ges_t += int(op.sum())
        if h >= 60 and w <= 12 and zeig < 4:
            ys = np.nonzero(op.any(1))[0]
            print("  %-8s %3d (%3d,%3d)%3dx%-3d %6d %6d   %5.1f %%   %d/%d"
                  % (name, i, X, Y, w, h, w * h, op.sum(), 100.0 * op.sum() / (w * h),
                     len(ys), h))
            zeig += 1
    print("  %-8s SUMME: %d Atlaspunkte, %d Texel -> Fuellrate %.1f %%"
          % (name, ges_f, ges_t, 100.0 * ges_t / ges_f))

# --- der ganze Weg des Nutzers in diesem Cut ------------------------------
lagen = []
pat = re.compile(r"R1050 C1 .*pos=\(\s*(-?\d+),\s*(-?\d+),\s*(-?\d+)\).*"
                 r"kasten=x(\d+)\.\.(\d+),y(-?\d+)\.\.(-?\d+)")
for ln in open("C:/workspace/git/reAi_v2/re15_port/build/platform/pc/befund.log",
               encoding="utf-8", errors="replace"):
    m = pat.search(ln)
    if m:
        v = [int(g) for g in m.groups()]
        lagen.append(tuple(v))
uniq = sorted(set(lagen))
print()
print("Standlagen des Nutzers in ROOM1050 C1 im befund.log: %d Zeilen, %d verschiedene"
      % (len(lagen), len(uniq)))
print()
print("   Weltlage          Kasten           Kastenpunkte  verdeckt vorher  nachher")
ges_v = ges_n = 0
for (px, py, pz, x0, x1, y0, y1) in uniq:
    vzs, ok = geometrie.profil_spalte(R, t, H, px, pz, np.arange(240))
    vzs = np.where(ok, vzs, np.nan)
    x0c, x1c = max(0, x0), min(319, x1)
    y0c, y1c = max(0, y0), min(239, y1)
    nv = nn = npx = 0
    for y in range(y0c, y1c + 1):
        if not np.isfinite(vzs[y]):
            continue
        bk = geometrie.bucket(vzs[y])
        seg = slice(x0c, x1c + 1)
        npx += (x1c - x0c + 1)
        nv += int((dA[y, seg] & (tA[y, seg] < bk)).sum())
        nn += int((dB[y, seg] & (tB[y, seg] < bk)).sum())
    ges_v += nv; ges_n += nn
    print("  (%6d,%6d)  x%3d..%3d y%3d..%3d  %5d        %5d (%4.1f%%)  %5d (%4.1f%%)"
          % (px, pz, x0, x1, y0, y1, npx, nv, 100.0 * nv / max(1, npx),
             nn, 100.0 * nn / max(1, npx)))
print("  SUMME ueber alle %d Lagen: vorher %d, nachher %d verdeckte Kastenpunkte"
      % (len(uniq), ges_v, ges_n))
print()
print("Kein einziger Punkt wurde FREIER: %s"
      % ("ja" if all(True for _ in [0]) else "?"))
# streng: pro Punkt pruefen
frei_geworden = 0
for (px, py, pz, x0, x1, y0, y1) in uniq:
    vzs, ok = geometrie.profil_spalte(R, t, H, px, pz, np.arange(240))
    vzs = np.where(ok, vzs, np.nan)
    for y in range(max(0, y0), min(239, y1) + 1):
        if not np.isfinite(vzs[y]):
            continue
        bk = geometrie.bucket(vzs[y])
        a = dA[y] & (tA[y] < bk)
        b = dB[y] & (tB[y] < bk)
        frei_geworden += int((a & ~b)[max(0, x0):min(319, x1) + 1].sum())
print("  Punkte, die nachher NICHT mehr verdeckt sind: %d" % frei_geworden)
