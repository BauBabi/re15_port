"""Leser A: Vollfigur-Render (roh, EA-Rasterer, Kamera-Licht) + Kopf-Kennzahlen je RE2-Kind.
    python re15_port/tools/r41_sichel/re2kopf/re2_analyse.py OUTDIR
"""
import os, sys, json
import numpy as np
from PIL import Image
HIER = os.path.dirname(os.path.abspath(__file__)); sys.path.insert(0, HIER)
import re2_leser as RL, emd_ansichtsblatt as EA

OUT = sys.argv[1]; os.makedirs(OUT, exist_ok=True)
KOPF = 3
res = {}
for kind in (0x30, 0x31, 0x33):
    mo = RL.modell(kind)
    kf0 = mo.frames[mo.clips[0][0]] & 0xfff
    V, N, F = mo.geometrie(kf0)
    R, T = mo.pose(kf0)
    lo, hi = V.min(0), V.max(0); ext = hi - lo
    r = dict(kind='0x%02x' % kind, kf0=int(kf0), hoehe=float(ext[1]), breite_z=float(ext[2]), tiefe_x=float(ext[0]),
             y_min=float(lo[1]), y_max=float(hi[1]), clips=len(mo.clips), knochen=mo.skel.bone_count, meshes=len(mo.meshes),
             tex=(mo.texw, mo.texh), clut_rows=int(mo.pal.shape[0]), clut_y=int(mo.clut_y))
    # je Mesh: bone-lokale bbox, Flaechen, UV-Belegung (page, clut-Zeile, u/v-Bereich)
    per = []
    for b, m in enumerate(mo.meshes):
        vs = np.array([v[:3] for v in m.tv + m.qv], float) if (m.tv or m.qv) else np.zeros((0, 3))
        uvs = {}
        for uv in m.tuv:
            for k in (0, 3, 6):
                key = (uv[5] & 0xF, mo.clut_row(uv[2]))
                uvs.setdefault(key, []).append((uv[k], uv[k + 1]))
        for uv in m.quv:
            for k in (0, 3, 6, 9):
                key = (uv[5] & 0xF, mo.clut_row(uv[2]))
                uvs.setdefault(key, []).append((uv[k], uv[k + 1]))
        belegung = {('page%d_clut%d' % k): dict(u=(int(min(p[0] for p in v)), int(max(p[0] for p in v))),
                                               v=(int(min(p[1] for p in v)), int(max(p[1] for p in v))), n=len(v)) for k, v in uvs.items()}
        d = dict(mesh=b, tris=len(m.tf), quads=len(m.qf), verts=len(vs),
                 bbox_lokal=(vs.min(0).tolist(), vs.max(0).tolist()) if len(vs) else None,
                 welt_T=T[b].tolist() if b < mo.skel.bone_count else None, parent=mo.skel.parent[b] if b < mo.skel.bone_count else None,
                 uv=belegung)
        per.append(d)
    r['meshes_detail'] = per
    # Weltpositionen Kopf-Mesh (Knochen 3) in der Pose
    idx = [i for i, f in enumerate(F) if f['bone'] == KOPF]
    vi = sorted({v for i in idx for v in F[i]['v']})
    KV = V[vi]
    r['kopf_welt_bbox'] = (KV.min(0).tolist(), KV.max(0).tolist())
    r['kopf_welt_ext'] = (KV.max(0) - KV.min(0)).tolist()
    r['kopf_anteil_H'] = dict(hoehe=float((KV.max(0) - KV.min(0))[1] / ext[1]), breite=float((KV.max(0) - KV.min(0))[2] / ext[1]))
    res['0x%02x' % kind] = r
    # Renders Vollfigur vorn / rechts (roh)
    SS = 2; TILE = 600
    SC = (TILE * SS * 0.9) / ext.max(); ctr = (lo + hi) / 2
    for nm, view in (('vorn', EA.V_FRONT), ('rechts', EA.V_RIGHT), ('links', EA.V_LEFT)):
        right, down, depth = view
        w = h = TILE * SS
        cx = w / 2 - float(np.dot(ctr, right)) * SC; cy = h / 2 - float(np.dot(ctr, down)) * SC
        col, alp = EA.render(mo, V, N, F, right, down, depth, w, h, SC, cx, cy)
        rgb = np.clip(col * 1.62, 0, 1) ** (1 / 1.18)
        img = (rgb * 255).astype(np.uint8); img[alp < 0.5] = (40, 40, 44)
        Image.fromarray(img).resize((TILE, TILE), Image.LANCZOS).save(os.path.join(OUT, 'em%02x_voll_%s.png' % (kind, nm)))
    # Texturblatt mit je Seite ihrer CLUT (mehrheitlich genutzte Zeile je Page)
    pz = {}
    for f in F:
        pz.setdefault(f['page'] & 0xF, []).append(mo.clut_row(f['clut']))
    rows = {p: int(np.bincount(v).argmax()) for p, v in pz.items()}
    r['page_clut_mehrheit'] = rows
    sheet = np.zeros((mo.texh, mo.texw, 3), np.uint8)
    for p in range(mo.texw // 128):
        sheet[:, p * 128:(p + 1) * 128] = mo.pal[rows.get(p, 0)][mo.tex[:, p * 128:(p + 1) * 128]]
    Image.fromarray(sheet).resize((mo.texw * 2, mo.texh * 2), Image.NEAREST).save(os.path.join(OUT, 'em%02x_textur.png' % kind))
    print(kind, 'H', round(ext[1]), 'kopf ext', [round(x) for x in r['kopf_welt_ext']], 'kopf uv', per[KOPF]['uv'], 'rows', rows)
json.dump(res, open(os.path.join(OUT, 'analyse.json'), 'w'), indent=1)
