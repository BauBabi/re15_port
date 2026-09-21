"""Der EINE neue HINTER-Rest-Platz (-1700, 24950): wo steht er im Bild, welche Spalten
tragen dort eine Maske, und warum verdeckt sie ihn nicht? Gegenprobe mit dem bereits
vorher offenen Nachbarn (-1500, 24950).
"""
import os
import struct
import sys

import numpy as np

sys.path.insert(0, os.path.abspath("re15_port/tools/maske"))
import abnahme                                              # noqa: E402
import geom                                                 # noqa: E402
import geometrie                                            # noqa: E402
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
    deck, tief = abnahme.deckung_und_tiefe(ms, idx)
    p = os.path.join(d, "%s_PRI%02d.STAND" % (ROOM, CUT))
    zeil = [l for l in open(p).read().splitlines() if not l.startswith("#")]
    sv = np.array([float(v) for v in zeil[0].split()]); sv[sv < 0] = np.nan
    y0 = np.array([float(v) for v in zeil[1].split()])
    return deck, tief, sv, y0


for nm, d in (("VORHER", os.path.join(CD, "MASKS")), ("NACHHER", "build/r19c/neu_MASKS")):
    deck, tief, sv, sy0 = lies(d)
    print("=== %s ===" % nm)
    for (wx, wz) in ((-1700, 24950), (-1500, 24950), (-900, 25750)):
        pf = abnahme.proj(R, t, H, wx, 0, wz)
        pk = abnahme.proj(R, t, H, wx, -abnahme.KOPF, wz)
        if pf is None or pk is None:
            print("   (%6d,%6d) nicht projizierbar" % (wx, wz)); continue
        fsx, fsy, fvz = pf; ksx, ksy, kvz = pk
        hw = abnahme.HALB * H / fvz
        x0 = int(max(0, fsx - hw)); x1 = int(min(320, fsx + hw))
        y0 = int(max(0, min(ksy, fsy))); y1 = int(min(240, max(ksy, fsy)))
        box = deck[y0:y1, x0:x1]
        ys = np.arange(y0, y1)
        vzs, ok = geometrie.profil_spalte(R, t, H, wx, wz, ys)
        vzs = np.where(ok, vzs, fvz)
        verd = box & geometrie.verdeckt(tief[y0:y1, x0:x1], vzs[:, None])
        basis = (wx * R[6] + wz * R[8]) / 4096.0 + t[2]
        ref = basis + sy0[x0:x1] * R[7] / 4096.0
        colmask = box.any(0) & np.isfinite(sv[x0:x1])
        hin = colmask & (ref > sv[x0:x1] + geometrie.BUCKET)
        vor = colmask & (ref < sv[x0:x1] - geometrie.BUCKET)
        q = verd[:, hin].sum() / max(1, box[:, hin].sum()) if hin.any() else float("nan")
        print("   (%6d,%6d) scr=(%3d,%3d) kasten x%d..%d y%d..%d  Kamera-z %d"
              % (wx, wz, int(fsx), int(fsy), x0, x1, y0, y1, fvz))
        print("        Spalten mit Maske %d, davon HINTER %d / VOR %d ; Verdeckungsquote HINTER %.2f"
              % (int(colmask.sum()), int(hin.sum()), int(vor.sum()), q))
        if hin.any():
            cs = np.nonzero(hin)[0] + x0
            print("        HINTER-Spalten %d..%d ; Standlinie dort %s ; Referenz %d"
                  % (cs.min(), cs.max(),
                     "%.0f..%.0f" % (np.nanmin(sv[cs]), np.nanmax(sv[cs])), int(ref[hin][0])))
            tt = tief[y0:y1, x0:x1][:, hin]
            bb = np.floor(1023.0 * vzs / 65536.0)[:, None] * np.ones((1, int(hin.sum())))
            m = box[:, hin]
            print("        Maskentiefen dort %d..%d ; Figur-Eimer %d..%d -> verdeckt %d von %d"
                  % (int(tt[m].min()), int(tt[m].max()), int(bb.min()), int(bb.max()),
                     int((m & (tt < bb)).sum()), int(m.sum())))
    print()
