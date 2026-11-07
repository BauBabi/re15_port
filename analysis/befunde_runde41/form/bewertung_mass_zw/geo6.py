#!/usr/bin/env python3
"""Biegeprobe fein (Rumpf 5-30 Grad, Beine 60 Grad), Raum-Probe gegen SCA, Schwerpunkt-Details."""
import json, math, os, struct, sys
import numpy as np
import cv2
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import geo as G
import geo2 as G2
import geo4 as G4
ZW = os.path.join(G.FORM, 'bewertung_mass_zw')
ROOT = 'C:/workspace/git/reAi_v2/.claude/worktrees/r41_form'
NAMES = ["collision", "camera", "zone", "light", "md1ptr", "floor", "block", "message", "mainScd", "subScd", "extraScd", "effect", "x50", "espTim", "modelTim", "anim"]
RUMPF = {'a': ['rumpf_mitte', 'rumpf'], 'b': ['rumpf_m', 'rumpf'], 'c': ['rumpf_m', 'rumpf_v']}
BEINGELENKE = {
 'a': ['huefte_r', 'fr_ober', 'fr_unter', 'fr_fuss', 'huefte_l', 'fl_ober', 'fl_unter', 'fl_fuss', 'nl_ober', 'nl_unter', 'nl_hand', 'nr_ober', 'nr_unter', 'nr_hand', 'hals', 'kopf'],
 'b': ['fr_ober', 'fr_unter', 'fr_fuss', 'fl_ober', 'fl_unter', 'fl_fuss', 'nl_ober', 'nl_unter', 'nl_faust', 'nr_ober', 'nr_unter', 'nr_faust', 'maul_l', 'maul_r'],
 'c': ['fr_huefte', 'fr_bein', 'fr_fuss', 'fl_huefte', 'fl_bein', 'fl_fuss', 'nl_ober', 'nl_kugel_o', 'nl_kugel_u', 'nr_ober', 'nr_unter', 'nr_hand', 'hals', 'vorsprung'],
}


def sca(rid):
    p = '%s/re15_port/shared_assets/PSX/STAGE%d/ROOM%04X.RDT' % (ROOT, rid >> 12, rid)
    d = open(p, 'rb').read()
    offs = {nm: struct.unpack_from('<I', d, 0x20 + 4 * i)[0] for i, nm in enumerate(NAMES)}
    s = offs['collision']
    n = sum(struct.unpack_from('<5I', d, s + 4))
    z = []
    for i in range(n):
        w, dep, x, zz = struct.unpack_from('<HHhh', d, s + 24 + 12 * i)
        typ, u0, u1, flr = struct.unpack_from('<BBBB', d, s + 24 + 12 * i + 8)
        z.append((x, zz, w, dep, flr, typ, u0))
    return z


def raum(k):
    f, _ = G.lade(k)
    aus = {}
    for n, ci, rid in ((1, 0, 0x5080), (2, 1, 0x5090)):
        Y, lage = G2.welt_kamera(k, n)
        V, Fa, Bi = G2.dreiecke_welt(f, ci, range(len(f['knochen'])))
        W = V @ Y.T + lage
        boden = W[-V[:, 1] < 900]   # Teile unter 900 Hoehe
        c = sca(rid)
        rechte = [x for x in c if (x[5] & 0x0F) == 1 and (x[6] & 1)]
        z = []
        tref = np.zeros(len(W), bool)
        for x, zz, w, dep, flr, typ, u0 in rechte:
            m = (W[:, 0] >= x) & (W[:, 0] <= x + w) & (W[:, 2] >= zz) & (W[:, 2] <= zz + dep)
            if m.any():
                # Eindringtiefe
                t = np.minimum.reduce([W[m, 0] - x, x + w - W[m, 0], W[m, 2] - zz, zz + dep - W[m, 2]])
                z.append(dict(zelle=(x, zz, w, dep, flr, typ, u0), ecken=int(m.sum()), tief=round(float(t.max())), hoehe=(round(float((-V[m, 1]).min())), round(float((-V[m, 1]).max())))))
            tref |= m
        aus[rid] = dict(ecken=len(W), in_zelle=int(tref.sum()), bbox=[[round(float(W[:, 0].min())), round(float(W[:, 0].max()))], [round(float(W[:, 2].min())), round(float(W[:, 2].max()))]], zellen=z[:8], rechte=len(rechte), alle=len(c))
    return aus


def biege_liste(f, k, namen, winkel, achsen=((1, 0, 0), (0, 0, 1))):
    N = G4.Netz(f)
    nm = G.NAMEN[k]
    R0, T0 = G4.pose_frei(f, 2)
    V, _, _ = N.alles(R0, T0)
    m = (V.max(0) + V.min(0)) / 2
    s = G4.GROSS * 0.46 / float(np.linalg.norm(V - m, axis=1).max())
    g0, z0, _, _ = G4.rundblick(N, R0, T0, s, m)
    aus = {}
    for n in namen:
        b = nm.index(n)
        z = []
        for w in winkel:
            sch, zer = 0, 0
            for ax in achsen:
                for sg in (1, -1):
                    R, T = G4.pose_frei(f, 2, {b: G4.rot(ax, sg * w)})
                    g, zr, _, _ = G4.rundblick(N, R, T, s, m)
                    sch = max(sch, g - g0)
                    zer = max(zer, len(zr))
            z.append((w, sch, zer))
        aus[n] = z
    return aus


if __name__ == '__main__':
    was = sys.argv[1] if len(sys.argv) > 1 else 'raum'
    for k in 'abc':
        f, _ = G.lade(k)
        if was == 'raum':
            print('\n====', k.upper())
            r = raum(k)
            for rid, x in r.items():
                print(hex(rid), x)
        if was == 'rumpf':
            print('\n====', k.upper(), 'Rumpf, (Grad, neue Lochpixel, zerfallene Richtungen)')
            for n, z in biege_liste(f, k, RUMPF[k], (5, 10, 15, 20, 30)).items():
                print(' ', n, z)
        if was == 'beine':
            print('\n====', k.upper(), 'Beine, 60 Grad')
            for n, z in biege_liste(f, k, BEINGELENKE[k], (60,)).items():
                print('  %-11s neue Lochpixel %5d  Zerfall %d' % (n, z[0][1], z[0][2]))
