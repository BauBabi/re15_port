#!/usr/bin/env python3
"""tuer_uv_sonde.py - Runde 33: WO liest ein Tuerarchiv welche Texelstelle? (Werkzeug fuer die Textur-Rezepte)

    python re15_port/tools/tueren/tuer_uv_sonde.py 27 0 0 [--bild 60]  -> build/r33_tueren/uvsonde_DOOR27_v0.png

Rastert die Objekte eines RE2-Archivs (Variante, Bild) mit dem Katalog-Simulator + Rasterer von
tor_helligkeit.py (dieselbe Geometrie/Bewegung, gegen die die Port-Maschine Bild fuer Bild geprueft ist) mit
einer KOORDINATEN-Textur: rot = u*2, gruen = v, blau = Mesh*60; weisse Linien bei u, v = 0 mod 16, Zahlen
alle 32. So ist im Bild ablesbar, welcher Texturteil wohin kommt (z. B. DOOR27: der linke Teil liest
v 0..100 fuer die obere UND die untere Haelfte).
"""
import argparse
import os
import sys

import numpy as np
from PIL import Image, ImageDraw

HIER = os.path.dirname(os.path.abspath(__file__))
PORT = os.path.abspath(os.path.join(HIER, "..", ".."))
REPO = os.path.abspath(os.path.join(PORT, ".."))
sys.path.insert(0, os.path.join(PORT, "tools", "tor"))
import tor_helligkeit as th   # noqa: E402


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("archiv")
    ap.add_argument("variante", type=int)
    ap.add_argument("x", nargs="?", default="0")
    ap.add_argument("--bild", type=int, default=1)
    a = ap.parse_args()
    name = a.archiv.upper()
    md1, _, _ = th.archiv(name)
    jetzt, vor = th.vm_saetze(name, a.variante, a.bild)
    tris = []
    for k, sz in sorted(jetzt.items()):
        if sz["mesh"] >= len(md1.meshes):
            continue
        for t in th.dreiecke(md1, None, sz, None, vor.get(k)):
            t["mesh"] = sz["mesh"]
            tris.append(t)
    uu, vv = np.meshgrid(np.arange(128), np.arange(256))
    tex = np.stack([uu * 2, vv, np.zeros_like(uu)], -1).astype(np.float32)
    ok = np.ones((256, 128), bool)
    R = th.rastern(tris, tex, ok, rand=0)
    T = R["T"]
    ident = R["id"]
    img = np.zeros(T.shape, np.uint8)
    img[..., 0] = np.clip(T[..., 0], 0, 255)
    img[..., 1] = np.clip(T[..., 1], 0, 255)
    mesh_bild = np.full(ident.shape, -1)
    for k, t in enumerate(tris):
        mesh_bild[ident == k] = t["mesh"]
    img[..., 2] = np.where(mesh_bild >= 0, 40 + 60 * np.maximum(mesh_bild, 0), 0)
    u = T[..., 0] / 2.0
    v = T[..., 1]
    drin = ident >= 0
    gitter = drin & ((np.abs(u - np.round(u / 16) * 16) < 0.35) | (np.abs(v - np.round(v / 16) * 16) < 0.35))
    img[gitter] = 255
    im = Image.fromarray(img)
    d = ImageDraw.Draw(im)
    H, W = ident.shape
    for y in range(0, H, 36):
        for x in range(0, W, 48):
            if drin[y, x]:
                d.text((x, y), "%d,%d" % (u[y, x], v[y, x]), fill=(255, 255, 0))
    aus = os.path.join(REPO, "build", "r33_tueren", "uvsonde_DOOR%s_v%d_b%d.png" % (name, a.variante, a.bild))
    os.makedirs(os.path.dirname(aus), exist_ok=True)
    im.save(aus)
    print(aus, "Dreiecke", len(tris))
    return 0


if __name__ == "__main__":
    sys.exit(main())
