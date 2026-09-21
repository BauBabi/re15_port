"""WO aendert sich die Tiefe — und warum bis zu 24 Eimer? Zerlegt die 1112 geaenderten
Punkte nach Zugehoerigkeit (Stuhl / Tischplattenecke / Holztisch-Quader) und zeigt die
Differenzen je Menge. Die Frage dahinter: der Saum darf den Stuhl um EINEN Eimer
verschieben (ein Standpunkt, geometrie.py:125-127), aber nicht fremde Gegenstaende
umdeuten, ohne dass ich es benenne.
"""
import os
import struct
import sys

import numpy as np

sys.path.insert(0, os.path.abspath("re15_port/tools/maske"))
import abnahme                                              # noqa: E402
import geom                                                 # noqa: E402
import maske_aus_png                                        # noqa: E402
import maskenbild                                           # noqa: E402

CD = "re15_port/shared_assets/PSX"
ROOM, CUT = "ROOM10D0", 7
rdt, _ = geom.load_rdt(CD, ROOM)
cam = struct.unpack_from("<I", rdt, 0x24)[0]
R, t, H = geom.cut_view(rdt, cam, CUT)


def lies(d):
    blob = open(os.path.join(d, "%s.MSK" % ROOM), "rb").read()
    ms = maskenbild.masken(blob, CUT) or []
    idx = maskenbild.lies_tim_bytes(open(os.path.join(d, "%s_PRI%02d.TIM" % (ROOM, CUT)), "rb").read())[0]
    return (ms,) + abnahme.deckung_und_tiefe(ms, idx)


msA, dA, tA = lies(os.path.join(CD, "MASKS"))
msB, dB, tB = lies("build/r19c/neu_MASKS")
alt = np.load("build/r19c/alt.npy")
neu = np.load("build/r19c/neu.npy")
saum = neu & ~alt
platte = maske_aus_png.setze("pri/STAGE1/10D0/07_02.png", 0, 146, 1, alpha_schwelle=110)
quader = dA & ~alt & ~platte                     # der Rest der Deckung = Holztisch-Quader

gl = dA & dB
for name, m in (("Stuhl alt (2677)", alt & gl), ("Tischplattenecke", platte & gl),
                ("Holztisch-Quader (Rest)", quader & gl)):
    if m.sum() == 0:
        continue
    dd = (tB[m].astype(int) - tA[m].astype(int))
    werte, anz = np.unique(dd, return_counts=True)
    print("%-26s n=%5d  geaendert %5d  Differenzen: %s"
          % (name, int(m.sum()), int((dd != 0).sum()),
             ", ".join("%+d:%d" % (w, c) for w, c in zip(werte, anz) if w != 0) or "keine"))
print("Saum: Tiefen nachher %d..%d" % (int(tB[saum & dB].min()), int(tB[saum & dB].max())))
print("   Saumpunkte insgesamt %d, davon vorher schon von einem ANDEREN Objekt gedeckt %d, "
      "vorher ganz ungedeckt %d" % (int(saum.sum()), int((saum & dA).sum()), int((saum & ~dA).sum())))

print("\n=== Zeilenmodell des Stuhls (Riegel-Eigenschaften B/C/D), NACHHER ===")
mehr = rueck = 0
letzte = -1
for y in range(240):
    xs = np.nonzero(neu[y] & dB[y])[0]
    if len(xs) == 0:
        continue
    tv = np.unique(tB[y, xs])
    if len(tv) > 1:
        mehr += 1
    if letzte >= 0 and tv.min() < letzte:
        rueck += 1
    letzte = int(tv.max())
print("   Bildzeilen mit MEHR als einer Tiefe: %d ; Monotonie-Rueckschritte: %d" % (mehr, rueck))

spr = 0
mx = 0
for y in range(1, 239):
    for x in range(1, 319):
        if not (neu[y, x] and dB[y, x]):
            continue
        for dy in (-1, 0, 1):
            for dx in (-1, 0, 1):
                if dy == 0 and dx == 0:
                    continue
                if neu[y + dy, x + dx] and dB[y + dy, x + dx]:
                    spr += 1
                    mx = max(mx, abs(int(tB[y, x]) - int(tB[y + dy, x + dx])))
print("   Nachbarpaare %d, groesster Tiefensprung an der Naht %d" % (spr, mx))
