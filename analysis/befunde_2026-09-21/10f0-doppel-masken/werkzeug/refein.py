# -*- coding: utf-8 -*-
"""Was macht die Entdopplung MIT DER VERDECKUNG?

Regionen kommen aus der geschriebenen Maske (Atlas-Deckung je Objektblock).
Tiefenkarten werden aus dem RDT neu gerechnet — woertlich die beiden Zweige aus
geom.depth_map_objekt (szene: geom.py:909-940, quader: geom.py:966-985).
Eichung: die so nachgerechneten Kacheltiefen muessen die GESCHRIEBENEN Tiefen
ergeben (Statistik np.max, anwenden.py:496-497).
"""
import os
import struct
import sys
import numpy as np
sys.path.insert(0, "re15_port/tools/maske")
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import geom
from zensus import container
from paare import lies_tim_bytes
from bloecke import deckung
from bloecke2 import bloecke
from gewinn import waehle, KANTEN, BUDGET

MASKS = "re15_port/shared_assets/PSX/MASKS"
CD = "re15_port/shared_assets/PSX"
INF = 1 << 30


def szene_tiefe(rdt, R, t, H, region):
    best = np.full((240, 320), np.inf)
    for (x, z, w, d, typ) in (geom.sca_sperrzellen(rdt, 0) or ()):
        if typ == 3:
            vzq, trq = geom.quader_tiefe(R, t, H, x, x + w, z, z + d, -1950)
            m = trq & (vzq < best)
            best[m] = vzq[m]
        else:
            kt = geom.kollisionstiefe_schnell(R, t, H, [(x, z, w, d)]) * 64.0
            m = (kt > 0) & (kt < best)
            best[m] = kt[m]
    dep = np.zeros((240, 320), np.int32)
    msk = np.asarray(region, bool)
    hit = msk & np.isfinite(best)
    dep[hit] = np.clip(np.ceil(best[hit] / 64.0).astype(np.int32), 1, 1023)
    fehlt = msk & ~hit
    if fehlt.any() and hit.any():
        from scipy import ndimage as nd
        _, (iy, ix) = nd.distance_transform_edt(~hit, return_indices=True)
        dep[fehlt] = dep[iy[fehlt], ix[fehlt]]
    return dep


def quader_tiefe_map(R, t, H, q, region):
    X, Z, W, D, h = [int(v) for v in q]
    vz, tr = geom.quader_tiefe(R, t, H, X, X + W, Z, Z + D, h)
    dep = np.zeros((240, 320), np.int32)
    msk = np.asarray(region, bool) & tr
    dep[msk] = np.clip(np.ceil(vz[msk] / 64.0).astype(np.int32), 1, 1023)
    return dep


def kacheln(region, dep, k):
    out = []
    for (x, y, w, h) in geom.gitter_mit_kante(region, k):
        win = dep[y:y + h, x:x + w]
        win = win[win > 0]
        if len(win) == 0:
            continue
        out.append((x, y, w, h, int(np.max(win))))
    return out


def feld(objekte, idx_regionen):
    """min-Tiefe je Bildpunkt ueber die Kacheln, MASKIERT mit der Region."""
    T = np.full((240, 320), INF, np.int64)
    for (reg, ks) in objekte:
        for (x, y, w, h, d) in ks:
            sub = np.zeros((240, 320), bool)
            sub[y:y + h, x:x + w] = True
            sub &= reg
            T = np.where(sub, np.minimum(T, d), T)
    return T


def lauf(room, cut, doppel, quader_je_obj, kasten, eimer):
    rid = int(room[4:], 16)
    rdt = open(os.path.join(CD, "STAGE1", "%s.RDT" % room), "rb").read()
    cam = struct.unpack_from("<I", rdt, 0x24)[0]
    R, t, H = geom.cut_view(rdt, cam, cut)

    res, _ = container(os.path.join(MASKS, "%s.MSK" % room))
    masks = res[cut][0]
    idx, _c = lies_tim_bytes(open(os.path.join(MASKS, "%s_PRI%02d.TIM" % (room, cut)), "rb").read())
    bl = bloecke(masks)
    regs, ist_kante, ist_tiefen = [], [], []
    for (s, e, k, ax, ay) in bl:
        d = np.zeros((240, 320), bool)
        for m in masks[s:e]:
            d |= deckung(idx, m)
        regs.append(d)
        ist_kante.append(k)
        ist_tiefen.append(sorted((m[2], m[3], m[5], m[6], m[4]) for m in masks[s:e]))

    deps = []
    for i, reg in enumerate(regs):
        q = quader_je_obj.get(i)
        deps.append(quader_tiefe_map(R, t, H, q, reg) if q else szene_tiefe(rdt, R, t, H, reg))

    print("=== %s C%d — Eichung der nachgerechneten Tiefen gegen die geschriebenen" % (room, cut))
    ok_ges = 0
    for i, reg in enumerate(regs):
        ks = sorted(kacheln(reg, deps[i], ist_kante[i]))
        soll = ist_tiefen[i]
        gleich = (len(ks) == len(soll)) and all(
            (a[0], a[1], a[2], a[3]) == (b[0], b[1], b[2], b[3]) and a[4] == b[4]
            for a, b in zip(ks, soll))
        n_t = sum(1 for a, b in zip(ks, soll) if a[4] == b[4]) if len(ks) == len(soll) else 0
        ok_ges += n_t
        print("  Obj %2d: Kante %2d  %2d Kacheln  Tiefen %s (%d/%d gleich)"
              % (i, ist_kante[i], len(ks), "IDENTISCH" if gleich else "abweichend",
                 n_t, len(soll)))
    print("  Summe: %d von %d Kacheltiefen nachgerechnet == geschrieben" % (ok_ges, len(masks)))

    # Verdeckung im Figurkasten: IST vs. entdoppelt-und-verfeinert
    behalten = [i for i in range(len(regs)) if i not in doppel]
    kosten = [geom.gitter_kosten(regs[i], KANTEN) for i in behalten]
    komb2, n2, f2, nn2 = waehle(kosten, BUDGET)
    objekte_ist = [(regs[i], kacheln(regs[i], deps[i], ist_kante[i])) for i in range(len(regs))]
    objekte_neu = [(regs[i], kacheln(regs[i], deps[i], komb2[a])) for a, i in enumerate(behalten)]
    Ta = feld(objekte_ist, None)
    Tb = feld(objekte_neu, None)
    x0, x1, y0, y1 = kasten
    print("  Figurkasten x%d..%d y%d..%d (%d Punkte)" % (x0, x1, y0, y1, (x1 - x0 + 1) * (y1 - y0 + 1)))
    for name, e_ in eimer:
        va = int((Ta[y0:y1 + 1, x0:x1 + 1] < e_).sum())
        vb = int((Tb[y0:y1 + 1, x0:x1 + 1] < e_).sum())
        print("    Eimer %3d (%-5s): IST %4d verdeckt  ->  entdoppelt+verfeinert %4d  (%+d)"
              % (e_, name, va, vb, vb - va))
    print("  Kanten neu: %s -> %d Masken" % (list(komb2), n2))
    print("  Bildweit verdeckte Flaeche (Tiefe < 173): IST %d  neu %d"
          % (int((Ta < 173).sum()), int((Tb < 173).sum())))
    d = Ta != Tb
    print("  Tiefenfeld IST vs. entdoppelt+verfeinert: %d Punkte verschieden" % int(d.sum()))
    if d.any():
        print("    Ta-Tb Verteilung: naeher(neu kleiner) %d, ferner %d, groesster Betrag %d"
              % (int((Tb < Ta).sum()), int((Tb > Ta).sum()),
                 int(np.abs(np.where(d, Ta - Tb, 0)).max())))
        for e_ in (120, 140, 160, 165, 169, 173, 200, 300):
            print("      Schwelle %3d: IST %6d verdeckte Punkte, neu %6d"
                  % (e_, int((Ta < e_).sum()), int((Tb < e_).sum())))


if __name__ == "__main__":
    A = json_q = {
        3: [6600, 7500, 1500, 1500, -1950], 4: [6700, 10300, 1500, 1500, -1950],
        5: [4200, 11800, 1500, 1500, -1950], 6: [1550, 11650, 1500, 1500, -1950],
        7: [-1600, 12200, 1500, 1500, -1950], 8: [-1800, 8200, 1500, 1500, -1950],
        9: [-1900, 4300, 1500, 1500, -1950], 10: [1600, 5700, 1500, 1500, -1950]}
    lauf("ROOM10F0", 4, {7, 8, 9}, A, (99, 121, 81, 118),
         [("Fuss", 173), ("Mitte", 169), ("Kopf", 165)])
