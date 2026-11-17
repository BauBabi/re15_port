# Runde 7e Analyse 1: Flaechen von Mesh 11 / 12 (und 14, 13) - CLUT-Zeile, Seite, UV-Rechteck, Farbe (Paletten-Lab-Median der Texel)
import os, sys, struct, json
import numpy as np, cv2
W = r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel_v8/re15_port/tools/r41_sichel/v8'
sys.path.insert(0, W)
import v8_gemein as V
import kopf_orig_bau as KB
import birkin_formen as BF

def lab15(c):
    r = (c & 31) / 31.0; g = ((c >> 5) & 31) / 31.0; b = ((c >> 10) & 31) / 31.0
    a = np.array([[[b, g, r]]], np.float32)
    L = cv2.cvtColor(a, cv2.COLOR_BGR2Lab)[0, 0]
    return L

def info(blob, name, meshes=(11, 12)):
    d, _o, ms, tim = KB.teile(blob)
    t = KB.tim_teile(tim)
    cy = t['cy']
    print('==', name, 'CLUT ab y', cy, 'Zeilen', t['clut'].shape[0], 'pix', t['pix'].shape)
    for mi in meshes:
        m = ms[mi]
        rows = {}
        for kind, F, UV, n in (('d', m['tf'], m['tuv'], 3), ('v', m['qf'], m['quv'], 4)):
            for k, (f, uv) in enumerate(zip(F, UV)):
                clut = uv[2]; row = clut >> 6; page = uv[5] & 0xF
                us = [uv[3 * j] for j in range(n)]; vs = [uv[3 * j + 1] for j in range(n)]
                mk = KB.raster(uv, n, page * 128, t['pix'].shape)
                idx = t['pix'][mk]
                zr = row - cy
                if 0 <= zr < t['clut'].shape[0]:
                    cols = t['clut'][zr][idx]
                    labs = np.array([lab15(int(c)) for c in cols]) if len(cols) else np.zeros((0, 3))
                    med = np.median(labs, 0) if len(labs) else [0, 0, 0]
                else:
                    med = [-1, -1, -1]
                ys = [m['v'][vi][1] for vi in f[1::2]]
                rows.setdefault(row, []).append('%s%d' % (kind, k))
                print('  M%d %s%-2d zeile %d seite %d u %3d-%3d v %3d-%3d texel %4d Lab %5.1f %5.1f %5.1f  ecken %s y %d..%d' % (
                    mi, kind, k, row, page, min(us), max(us), min(vs), max(vs), int(mk.sum()), med[0], med[1], med[2],
                    list(f[1::2]), min(ys), max(ys)))
        print('  M%d Zeilen:' % mi, {r: len(v) for r, v in sorted(rows.items())})

asset = open(V.ASSET, 'rb').read()
print('asset sha', V.sha(asset))
info(asset, 'ASSET 5c902d49', (11, 12, 14))
o = BF.lade_em30()
print('orig sha', V.sha(o))
info(o, 'ORIGINAL EM30', (11, 12, 14))
