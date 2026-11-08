"""Leser A: Kopf-Nahaufnahmen (nur Kopf-Mesh) der RE2-Kinds 0x30/0x31/0x33 und Referenz 2 (EM30_HANGAR Mesh 9).
    python re15_port/tools/r41_sichel/re2kopf/re2_kopf.py OUTDIR
Je Quelle: vorn (EA.V_FRONT, Kamera +X) und rechts (EA.V_RIGHT, Kamera -Z = rechte Flanke des Wesens, wie Beschreibung2),
Kopfbox auf KH Pixel Hoehe normiert, Licht = EA.render kamerarelativ (alle gleich); zusaetzlich flach (shade 1) fuer Texelfarben.
Zonen (Anteile der Kopfbox): Kuppe y 0-0.25; links-Mitte x 0-0.5 / y 0.25-0.65; rechts-Mitte x 0.5-1 / y 0.25-0.65; Maul y 0.65-1.
Augen: rote Flecken (R - (G+B)/2 > 60) und dunkle Flecken (L < 0.35 x Median) im flachen Vorn-Render, je Komponente Schwerpunkt.
"""
import os, sys, json
import numpy as np, cv2
from PIL import Image
from skimage.color import rgb2lab
HIER = os.path.dirname(os.path.abspath(__file__)); sys.path.insert(0, HIER)
import re2_leser as RL, emd_ansichtsblatt as EA
sys.path.insert(0, os.path.dirname(HIER)); sys.path.insert(0, os.path.join(os.path.dirname(HIER), 'v2'))
import birkin_buehne as BB

OUT = sys.argv[1]; os.makedirs(OUT, exist_ok=True)
REF2 = os.path.join(RL.BAUM, 're15_port', 'shared_assets', 'PSX', 'EMD', 'EM30_HANGAR.EMD')
KH = 240          # Kopfbox-Hoehe in Pixel
TILE = 300
ZONEN = dict(kuppe=(0, 0, 1, .25), links_mitte=(0, .25, .5, .65), rechts_mitte=(.5, .25, 1, .65), maul=(0, .65, 1, 1))


def lab_mittel(rgb, m):
    px = rgb[m]
    if len(px) == 0:
        return None
    L = rgb2lab(px.reshape(-1, 1, 3) / 255.0).reshape(-1, 3)
    return [round(float(x), 1) for x in L.mean(0)] + [[int(round(x)) for x in px.mean(0)]]


def kopf_render(mo, bone, view, flach=False, kf=None):
    kf0 = mo.frames[mo.clips[0][0]] & 0xfff if kf is None else kf
    V, N, F = mo.geometrie(kf0)
    Fk = [f for f in F if f['bone'] == bone]
    vi = sorted({v for f in Fk for v in f['v']})
    right, down, depth = (np.asarray(v, float) for v in view)
    if flach:
        licht = right * 0.42 - down * 0.52 + depth * 0.75
        licht /= np.linalg.norm(licht)
        N = np.tile(licht, (len(N), 1))
    KV = V[vi]
    ys = KV @ down
    xs = KV @ right
    h = ys.max() - ys.min()
    w = xs.max() - xs.min()
    SS = 2
    sc = KH * SS / h
    W = H = TILE * SS
    cx = W / 2 - (xs.min() + xs.max()) / 2 * sc
    cy = H / 2 - (ys.min() + ys.max()) / 2 * sc
    col, alp = EA.render(mo, V, N, Fk, right, down, depth, W, H, sc, cx, cy)
    rgb = np.clip(col * 1.62, 0, 1) ** (1 / 1.18) if not flach else np.clip(col, 0, 1)
    img = (rgb * 255).astype(np.uint8)
    a = alp > 0.5
    img = cv2.resize(img, (TILE, TILE), interpolation=cv2.INTER_AREA)
    a = cv2.resize(a.astype(np.uint8), (TILE, TILE), interpolation=cv2.INTER_NEAREST) > 0
    yy, xx = np.nonzero(a)
    box = (int(xx.min()), int(yy.min()), int(xx.max()), int(yy.max()))
    return img, a, box, dict(hoehe_einh=float(h), breite_einh=float(w), kopfbox_px=box)


def zonen(img, a, box):
    x0, y0, x1, y1 = box
    w, h = x1 - x0 + 1, y1 - y0 + 1
    out = {}
    for k, (u0, v0, u1, v1) in ZONEN.items():
        m = np.zeros_like(a)
        m[int(y0 + v0 * h):int(y0 + v1 * h), int(x0 + u0 * w):int(x0 + u1 * w)] = True
        out[k] = lab_mittel(img.astype(float), m & a)
    out['gesamt'] = lab_mittel(img.astype(float), a)
    return out


def augen(img, a, box):
    x0, y0, x1, y1 = box
    w, h = x1 - x0 + 1, y1 - y0 + 1
    f = img.astype(float)
    rot = (f[..., 0] - (f[..., 1] + f[..., 2]) / 2 > 60) & a
    L = f @ np.array([.299, .587, .114])
    med = np.median(L[a]) if a.any() else 1
    dunkel = (L < 0.35 * med) & a
    res = {}
    for nm, m in (('rot', rot), ('dunkel', dunkel)):
        n, lab_, st, cen = cv2.connectedComponentsWithStats(m.astype(np.uint8), connectivity=8)
        fl = []
        for i in range(1, n):
            if st[i, cv2.CC_STAT_AREA] >= 12:
                fl.append(dict(px=int(st[i, cv2.CC_STAT_AREA]), x_anteil=round((cen[i][0] - x0) / w, 3),
                               y_anteil=round((cen[i][1] - y0) / h, 3), rgb=[int(v) for v in f[lab_ == i].mean(0)]))
        res[nm] = sorted(fl, key=lambda d: -d['px'])[:6]
    return res


def texel(mo, bone):
    kf0 = mo.frames[mo.clips[0][0]] & 0xfff
    V, N, F = mo.geometrie(kf0)
    tm = np.zeros((mo.texh, mo.texw), np.int16) - 1
    zeilen = set()
    for f in F:
        if f['bone'] != bone:
            continue
        ub = mo.page_ubase(f['page'])
        row = mo.clut_row(f['clut'])
        zeilen.add((f['page'] & 0xF, row))
        (u0, v0), (u1, v1), (u2, v2) = f['uv']
        m = np.zeros((mo.texh, mo.texw), np.uint8)
        cv2.fillPoly(m, [np.array([[u0 + ub, v0], [u1 + ub, v1], [u2 + ub, v2]], np.int32)], 1)
        tm[m > 0] = row
    ys, xs = np.nonzero(tm >= 0)
    arr = np.array([mo.pal[tm[y, x]][mo.tex[y, x]] for y, x in zip(ys, xs)], float)
    return tm, dict(n=int(len(xs)), u=(int(xs.min()), int(xs.max())), v=(int(ys.min()), int(ys.max())),
                    page_clut=sorted(zeilen), lab=lab_mittel(arr, np.ones(len(arr), bool)))


quellen = []
for kind in (0x30, 0x31, 0x33):
    quellen.append(('em%02x' % kind, RL.modell(kind), 3))
ref = BB.modell(open(REF2, 'rb').read())
print('Ref2 knochen', ref.skel.bone_count, 'parents', ref.skel.parent)
print('Ref2 rel', ref.skel.rel)
quellen.append(('ref2', ref, 9))
res = {}
for nm, mo, bone in quellen:
    r = dict(bone=bone)
    for vn, view in (('vorn', EA.V_FRONT), ('rechts', EA.V_RIGHT)):
        img, a, box, info = kopf_render(mo, bone, view)
        imgf, af, boxf, _ = kopf_render(mo, bone, view, flach=True)
        bg = np.full_like(img, 40); bg[a] = img[a]
        Image.fromarray(bg).save(os.path.join(OUT, '%s_kopf_%s.png' % (nm, vn)))
        bgf = np.full_like(imgf, 40); bgf[af] = imgf[af]
        Image.fromarray(bgf).save(os.path.join(OUT, '%s_kopf_%s_flach.png' % (nm, vn)))
        np.save(os.path.join(OUT, '%s_kopf_%s_maske.npy' % (nm, vn)), a)
        r[vn] = dict(info, zonen_licht=zonen(img, a, box), zonen_flach=zonen(imgf, af, boxf), augen_flach=augen(imgf, af, boxf),
                     seitenverhaeltnis_w_h=round((box[2] - box[0] + 1) / (box[3] - box[1] + 1), 3))
    tm, tx = texel(mo, bone)
    r['texel'] = tx
    u0, u1 = tx['u']; v0, v1 = tx['v']
    sub = tm[v0:v1 + 1, u0:u1 + 1]
    crop = np.full((sub.shape[0], sub.shape[1], 3), (40, 40, 44), np.uint8)
    for row in np.unique(sub[sub >= 0]):
        mm = sub == row
        crop[mm] = mo.pal[row][mo.tex[v0:v1 + 1, u0:u1 + 1][mm]]
    Image.fromarray(crop).resize((crop.shape[1] * 3, crop.shape[0] * 3), Image.NEAREST).save(os.path.join(OUT, '%s_kopf_textur.png' % nm))
    res[nm] = r
    print(nm, 'bone', bone, 'vorn h/b', round(r['vorn']['hoehe_einh']), round(r['vorn']['breite_einh']), 'w/h', r['vorn']['seitenverhaeltnis_w_h'],
          'rechts b', round(r['rechts']['breite_einh']), 'texel', tx['n'], tx['u'], tx['v'], tx['page_clut'], tx['lab'])
    print('   augen rot', r['vorn']['augen_flach']['rot'])
    print('   augen dunkel', r['vorn']['augen_flach']['dunkel'])
    print('   zonen flach', r['vorn']['zonen_flach'])
json.dump(res, open(os.path.join(OUT, 'koepfe.json'), 'w'), indent=1, default=str)
