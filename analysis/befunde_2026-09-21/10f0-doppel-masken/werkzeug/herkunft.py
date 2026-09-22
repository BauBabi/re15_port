# -*- coding: utf-8 -*-
"""Bauweg-Nachstellung ROOM10F0 C4/C5: WOHER kommen die Doppel?

Stellt die beiden Zweige aus re15_port/tools/maske/raum.py nach:
  Zweig S ("szene", raum.py:545-553): je Lasso-KOMPONENTE eine Region
        _comp | (_dunkel_label == ki), mit quader = _sperr[ki]
  Zweig Q ("quader"+nur_kunst, raum.py:401-431): _trq_ki
        & ((_kunst_label == ki) | (_dunkel_label == ki))

_dunkel_label kommt in BEIDEN Zweigen aus derselben Quelle und ist in beiden
auf die Silhouette DERSELBEN Zelle beschraenkt (raum.py:344/356). Der einzig
moegliche Unterschied ist also  _comp & ~_trq_ki  — und den misst dieses
Skript OHNE den Hintergrund (der fuer _dunkel_label noetig waere).
"""
import json
import os
import struct
import sys
import numpy as np
from scipy import ndimage as nd

sys.path.insert(0, "re15_port/tools/maske")
import geom
import maske_aus_png

CD = "re15_port/shared_assets/PSX"
AUS = json.load(open("analysis/esp_masken_2026-09-03/auswahl.json", encoding="utf-8"))
rdt = open(os.path.join(CD, "STAGE1", "ROOM10F0.RDT"), "rb").read()
cam_off = struct.unpack_from("<I", rdt, 0x24)[0]
sperr = geom.sca_sperrzellen(rdt, 0) or []
print("SCA-Sperrzellen (Band 0): %d" % len(sperr))
moebel = [(i, z) for i, z in enumerate(sperr) if z[4] == 3]
print("davon Typ 3 (Moebel): %d -> %s" % (len(moebel), [(i, z[0], z[1]) for i, z in moebel]))
print()

for cut in (4, 5):
    e = AUS["ROOM10F0"][str(cut)]
    v = geom.cut_view(rdt, cam_off, cut)
    sil = {}
    for ki, (zx, zz, zw, zd, typ) in enumerate(sperr):
        if typ != 3:
            continue
        vzq, trq = geom.quader_tiefe(v[0], v[1], v[2], zx, zx + zw, zz, zz + zd, -1950)
        sil[ki] = (vzq, trq)

    kunst = np.full((240, 320), -1, int)
    teile = {}
    for oi, o in enumerate(e["objekte"]):
        if "png" not in o:
            continue
        r = maske_aus_png.setze(o["png"], o["x"], o["y"], o.get("massstab", 1))
        lab, n = nd.label(r)
        lst = []
        for k in range(1, n + 1):
            comp = lab == k
            npx = int(comp.sum())
            if npx < 20:
                continue
            bf, bki = 0.0, None
            for ki, (vzq, trq) in sil.items():
                f = float((trq & comp).sum()) / npx
                if f > bf:
                    bf, bki = f, ki
            if bki is None:
                continue
            kunst[comp] = bki
            lst.append((bki, comp, npx, bf))
        teile[oi] = lst

    print("=== ROOM10F0 C%d" % cut)
    zellen_S = {}
    for oi, lst in teile.items():
        for (ki, comp, npx, bf) in lst:
            zx, zz = sperr[ki][0], sperr[ki][1]
            print("  Zweig S: Objekt %d (%s) Komponente -> Zelle %2d (x%d z%d), %d px, Jaccard %.3f"
                  % (oi, e["objekte"][oi]["png"], ki, zx, zz, npx, bf))
            zellen_S.setdefault(ki, []).append(comp)

    print()
    for oi, o in enumerate(e["objekte"]):
        if "quader" not in o:
            continue
        q = [int(x) for x in o["quader"]]
        eig = None
        for ki, (zx, zz, zw, zd, typ) in enumerate(sperr):
            if typ == 3 and zx == q[0] and zz == q[1]:
                eig = ki
                break
        mark = ""
        if eig in zellen_S:
            trq = sil[eig][1]
            comp = np.zeros((240, 320), bool)
            for c in zellen_S[eig]:
                comp |= c
            nur_S = int((comp & ~trq).sum())
            quader_region = trq & (kunst == eig)
            szene_region = comp
            sym = int((quader_region ^ szene_region).sum())
            mark = ("  <<< DOPPELT zu Zweig S: |S|=%d |Q|=%d  S\\Q=%d  Q\\S=%d"
                    % (int(szene_region.sum()), int(quader_region.sum()),
                       int((szene_region & ~quader_region).sum()),
                       int((quader_region & ~szene_region).sum())))
        print("  Zweig Q: Objekt %2d %-26s -> Zelle %s%s"
              % (oi, o["name"], eig, mark))
    print()
