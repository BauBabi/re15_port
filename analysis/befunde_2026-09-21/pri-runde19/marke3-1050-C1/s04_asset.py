"""Messung 2: was das GESCHRIEBENE Asset wirklich deckt und mit welcher Tiefe.
Quelle sind die Dateien, die die Engine liest: ROOM1050.MSK (Sektion) + ROOM1050_PRI01.TIM
(Atlas, Palettenindex != 0 = opak). Kein Nachbau des Modells.
"""
import os
import sys
import numpy as np

D = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.abspath(os.path.join(D, "..", "..", "..", ".."))
sys.path.insert(0, os.path.join(ROOT, "re15_port/tools/maske"))
os.chdir(ROOT)
import maskenbild
import abnahme
import geometrie

MASKS = "re15_port/shared_assets/PSX/MASKS"
blob = open(os.path.join(MASKS, "ROOM1050.MSK"), "rb").read()
ms = maskenbild.masken(blob, 1)
idx = maskenbild.lies_tim(os.path.join(MASKS, "ROOM1050_PRI01.TIM"))[0]
print("Rechtecke in der Datei: %d" % len(ms))
deck, tief = abnahme.deckung_und_tiefe(ms, idx)
print("Deckung (Texel): %d Punkte" % deck.sum())


def lies_pbm(p):
    b = open(p, "rb").read()
    i = b.index(b"\n", 3) + 1
    k = b[3:i].strip().split()
    w, h = int(k[0]), int(k[1])
    return np.unpackbits(np.frombuffer(b[i:], np.uint8).reshape(h, -1), axis=1)[:, :w].astype(bool)


soll = lies_pbm(os.path.join(MASKS, "ROOM1050_PRI01.PBM"))
print("Soll (PBM): %d | Deckung ohne Soll %d | Soll ohne Deckung %d"
      % (soll.sum(), int((deck & ~soll).sum()), int((soll & ~deck).sum())))

# --- der Faecher: Rechteck-Kasten GEGEN eigene Texel ------------------------
print()
print("Der 'Faecher' — Kasten gegen TEXEL (Bbox ist nicht die Kunst):")
print("  Nr  Kasten            t    Texel  Texelzeilen y   Kastenzeilen y")
for i, (sx, sy, X, Y, w, h, dep) in enumerate(ms):
    if not (180 <= X <= 232 and h >= 30):
        continue
    sub = idx[sy:sy + h, sx:sx + w]
    op = sub != 0
    ys = np.nonzero(op.any(1))[0]
    print("  %3d (%3d,%3d)%3dx%-3d t=%-4d %5d   y%3d..%3d (%2d Zeilen)  y%3d..%3d"
          % (i, X, Y, w, h, dep, op.sum(), Y + ys.min(), Y + ys.max(), len(ys), Y, Y + h - 1))

# --- Tiefe je Bildpunkt, Spielerkasten -------------------------------------
PVZ = 5650      # Fuss   (befund.log F239)
PVZ_H = 5465    # Huefte
PVZ_K = 5280    # Kopf
print()
print("Spieler F239: vz Fuss %d / Huefte %d / Kopf %d -> bucket %d / %d / %d"
      % (PVZ, PVZ_H, PVZ_K, geometrie.bucket(PVZ), geometrie.bucket(PVZ_H), geometrie.bucket(PVZ_K)))
kx0, kx1, ky0, ky1 = 181, 219, 122, 190
box = deck[ky0:ky1 + 1, kx0:kx1 + 1]
tb = tief[ky0:ky1 + 1, kx0:kx1 + 1]
print("Im Spielerkasten x%d..%d y%d..%d: %d von %d Punkten mit Maskentexel"
      % (kx0, kx1, ky0, ky1, box.sum(), box.size))
# verdeckt bei Fusstiefe / Kopftiefe
for nm, vz in (("Fuss", PVZ), ("Huefte", PVZ_H), ("Kopf", PVZ_K)):
    v = box & (tb < geometrie.bucket(vz))
    print("  bei %-6s vz=%d: %d Texel verdecken" % (nm, vz, v.sum()))

np.save(os.path.join(D, "deck.npy"), deck)
np.save(os.path.join(D, "tief.npy"), tief)
np.save(os.path.join(D, "soll.npy"), soll)
