#!/usr/bin/env python3
"""Zahlen-Pruefer Einbau Runde 1 — EIGENER EMD/TIM-Leser (nicht form_emd.py, nicht die Riegel des Bauers).

Formatquellen (Port-Code, byte-genau nachgebaut):
  TIM: re15_port/engine/src/tim_common.c:40-80 (CLUT-Block u32 size, u16 x,y,w,h; Pixelblock; width = data_w*2 bei 8 bpp)
  EMD (RE2-Format): re15_port/engine/src/re2_ems.c:93-170 (u32 dir_off, u32 dir_cnt == 8; dir[7] = MD1, dir[2] = EMR-Struktur,
       dir[1]/[3]/[5] = EDD, dir[4]/[6] = EMR-Pool)
  MD1: re15_port/engine/src/md1_common.c (Header 12, Mesh-Header 56, Offsets relativ zu 12; Tri 12 B, Quad 16 B,
       Tri-UV 12 B {u0,v0,clut,u1,v1,page,u2,v2,pad}, Quad-UV 16 B {u0,v0,clut,u1,v1,page,u2,v2,pad,u3,v3,pad})
  EMR: re15_port/engine/src/emd_common.c:136-200 (u16 bones_table_off, kf_off, bone_count, kf_size; rel-Pos s16[3] ab +8;
       Knochentabelle {u16 child_count, u16 child_off}, Kinderliste relativ zu bones_table_off)
  CLUT-Zeile: render_pc.c:2773-2775  clut_idx = ((clut >> 6) & 0x1ff) - clut_base_y
  Texturhoehe: render_pc.c:2560-2569  n_cluts = clut_entries / 256 (8 bpp), tex_h = height * n_cluts
  U-Basis: platform/pc/main.c:2288  pxo = (page & 0xF) * 128
"""
import hashlib
import json
import struct
import sys

import numpy as np
from PIL import Image

W = 'C:/workspace/git/reAi_v2/.claude/worktrees/r41_form'
ASSET_EMD = W + '/re15_port/shared_assets/RE2/BAHNSTEIG_FORM.EMD'
ASSET_TIM = W + '/re15_port/shared_assets/RE2/BAHNSTEIG_FORM.TIM'
VAR_EMD = W + '/analysis/befunde_runde41/form/varianten/K5/farbe/K5c_farbe_v3/FORM.EMD'
VAR_TIM = W + '/analysis/befunde_runde41/form/varianten/K5/farbe/K5c_farbe_v3/FORM.TIM'
BASIS_EMD = W + '/analysis/befunde_runde41/form/stufe3/basis_k5c/FORM.EMD'
OUT = sys.argv[1] if len(sys.argv) > 1 else '.'

u16 = lambda b, o: struct.unpack_from('<H', b, o)[0]
u32 = lambda b, o: struct.unpack_from('<I', b, o)[0]
s16 = lambda b, o: struct.unpack_from('<h', b, o)[0]


def sha(p):
    return hashlib.sha256(open(p, 'rb').read()).hexdigest()


# ---------------- TIM ----------------
def tim_lesen(b):
    assert u32(b, 0) == 0x10, 'kein TIM'
    flags = u32(b, 4)
    bpp = {0: 4, 1: 8, 2: 16}[flags & 7]
    has_clut = bool(flags & 8)
    off = 8
    t = {'bpp': bpp, 'has_clut': has_clut}
    if has_clut:
        csize = u32(b, off); cx, cy, cw, ch = u16(b, off + 4), u16(b, off + 6), u16(b, off + 8), u16(b, off + 10)
        t.update(clut_x=cx, clut_y=cy, clut_w=cw, clut_h=ch, clut_entries=cw * ch)
        t['clut'] = np.frombuffer(b, '<u2', cw * ch, off + 12).copy()
        off += csize
    dsize = u32(b, off); dx, dy, dw, dh = u16(b, off + 4), u16(b, off + 6), u16(b, off + 8), u16(b, off + 10)
    width = dw * 2 if bpp == 8 else dw * 4 if bpp == 4 else dw
    t.update(data_x=dx, data_y=dy, width=width, height=dh)
    if bpp == 8:
        t['pix'] = np.frombuffer(b, 'u1', width * dh, off + 12).reshape(dh, width).copy()
    # render_pc.c:2560-2569
    row_width = 16 if bpp == 4 else 256
    n_cluts = max(1, t['clut_entries'] // row_width) if has_clut else 1
    t['n_cluts'] = n_cluts
    t['tex_h'] = dh * n_cluts
    return t


def psx_rgb(w):
    w = np.asarray(w, np.uint32)
    r, g, b = w & 31, (w >> 5) & 31, (w >> 10) & 31
    return np.stack([r, g, b], -1).astype(np.float64) * 255.0 / 31.0


def lab(rgb):
    c = np.asarray(rgb, float) / 255.0
    c = np.where(c <= 0.04045, c / 12.92, ((c + 0.055) / 1.055) ** 2.4)
    M = np.array([[0.4124564, 0.3575761, 0.1804375], [0.2126729, 0.7151522, 0.0721750], [0.0193339, 0.1191920, 0.9503041]])
    xyz = c @ M.T / np.array([0.95047, 1.0, 1.08883])
    f = np.where(xyz > 0.008856, np.cbrt(xyz), 7.787 * xyz + 16.0 / 116.0)
    return np.stack([116 * f[..., 1] - 16, 500 * (f[..., 0] - f[..., 1]), 200 * (f[..., 1] - f[..., 2])], -1)


def de76(rgb_a, rgb_b):
    return np.sqrt(((lab(rgb_a) - lab(rgb_b)) ** 2).sum(-1))


# ---------------- EMD / MD1 / EMR ----------------
def emd_lesen(b):
    dir_off, dir_cnt = u32(b, 0), u32(b, 4)
    assert dir_cnt == 8, 'dir_cnt != 8'
    D = [u32(b, dir_off + 4 * i) for i in range(8)]
    return D


def md1_lesen(b, off):
    n_obj = u32(b, off + 8)
    base = off + 12
    meshes = []
    for i in range(n_obj // 2):
        h = off + 12 + i * 56
        f = struct.unpack_from('<14I', b, h)
        tv_off, tv_cnt, tn_off, tn_cnt, tf_off, tf_cnt, tu_off, qv_off, qv_cnt, qn_off, qn_cnt, qf_off, qf_cnt, qu_off = f
        m = {'i': i}
        m['tv'] = np.frombuffer(b, '<i2', tv_cnt * 4, base + tv_off).reshape(tv_cnt, 4)[:, :3].copy()
        m['tri'] = [struct.unpack_from('<6H', b, base + tf_off + 12 * k) for k in range(tf_cnt)]  # n0,v0,n1,v1,n2,v2
        m['tri_uv'] = []
        for k in range(tf_cnt):
            o = base + tu_off + 12 * k
            m['tri_uv'].append({'o': o, 'u': (b[o], b[o + 4], b[o + 8]), 'v': (b[o + 1], b[o + 5], b[o + 9]),
                                'clut': u16(b, o + 2), 'page': u16(b, o + 6), 'clut_o': o + 2})
        m['qv'] = np.frombuffer(b, '<i2', qv_cnt * 4, base + qv_off).reshape(qv_cnt, 4)[:, :3].copy()
        m['quad'] = [struct.unpack_from('<8H', b, base + qf_off + 16 * k) for k in range(qf_cnt)]
        m['quad_uv'] = []
        for k in range(qf_cnt):
            o = base + qu_off + 16 * k
            m['quad_uv'].append({'o': o, 'u': (b[o], b[o + 4], b[o + 8], b[o + 12]), 'v': (b[o + 1], b[o + 5], b[o + 9], b[o + 13]),
                                 'clut': u16(b, o + 2), 'page': u16(b, o + 6), 'clut_o': o + 2})
        meshes.append(m)
    return meshes


def emr_lesen(b, off):
    bt_off, kf_off, nb, kf_size = u16(b, off), u16(b, off + 2), u16(b, off + 4), u16(b, off + 6)
    rel = np.array([[s16(b, off + 8 + k * 6 + j) for j in (0, 2, 4)] for k in range(nb)])
    parent = [-1] * nb
    for p in range(nb):
        cc, co = u16(b, off + bt_off + p * 4), u16(b, off + bt_off + p * 4 + 2)
        for c in range(cc):
            ch = b[off + bt_off + co + c]
            if ch < nb and ch != p:
                parent[ch] = p
    return {'nb': nb, 'rel': rel, 'parent': parent, 'kf_off': kf_off, 'kf_size': kf_size}


def edd_clips(b, off):
    ct = u16(b, off + 2)
    return ct // 4


def welt_pos(emr):
    nb = emr['nb']
    pos = np.zeros((nb, 3))
    done = [False] * nb
    for _ in range(nb):
        for k in range(nb):
            if done[k]:
                continue
            p = emr['parent'][k]
            if p < 0:
                pos[k] = emr['rel'][k]; done[k] = True
            elif done[p]:
                pos[k] = pos[p] + emr['rel'][k]; done[k] = True
    return pos


# ---------------- Renderer (eigen, orthographisch, z-Puffer, nearest Texel) ----------------
def textur_rgba(t):
    """Texturstapel wie render_pc.c:2572-2578: je CLUT-Zeile eine Kopie, untereinander. RGBA; Texel 0x0000 = transparent."""
    h, w = t['pix'].shape
    stack = np.zeros((t['n_cluts'] * h, w, 4), np.uint8)
    for c in range(t['n_cluts']):
        row = t['clut'][c * 256:(c + 1) * 256]
        raw = row[t['pix']]
        rgb = psx_rgb(raw)
        a = np.where(raw == 0, 0, 255)
        stack[c * h:(c + 1) * h, :, :3] = np.round(rgb).astype(np.uint8)
        stack[c * h:(c + 1) * h, :, 3] = a
    return stack


def rendern(meshes, emr, tex, t, kamera, groesse=(640, 480)):
    """kamera: 3x3 Rotationsmatrix Welt->Kamera. Orthographisch, auf Bbox eingepasst (gemeinsam fuer beide Dateien ueber `kamera` + fester Massstab)."""
    Wd, Ht = groesse
    pos = welt_pos(emr)
    R, massstab, mitte = kamera
    img = np.zeros((Ht, Wd, 3), np.uint8)
    zbuf = np.full((Ht, Wd), np.inf)
    th, tw = tex.shape[:2]
    one_h = t['height']

    def flaeche(P, U, V, clut, page):
        # P: 3x3 Kamera-Koordinaten (x,y,z), z = Tiefe
        xs = (P[:, 0] - mitte[0]) * massstab + Wd / 2
        ys = (P[:, 1] - mitte[1]) * massstab + Ht / 2
        zs = P[:, 2]
        x0, x1 = int(max(0, np.floor(xs.min()))), int(min(Wd - 1, np.ceil(xs.max())))
        y0, y1 = int(max(0, np.floor(ys.min()))), int(min(Ht - 1, np.ceil(ys.max())))
        if x1 < x0 or y1 < y0:
            return
        gx, gy = np.meshgrid(np.arange(x0, x1 + 1) + 0.5, np.arange(y0, y1 + 1) + 0.5)
        d = (xs[1] - xs[0]) * (ys[2] - ys[0]) - (xs[2] - xs[0]) * (ys[1] - ys[0])
        if abs(d) < 1e-9:
            return
        l1 = ((gx - xs[0]) * (ys[2] - ys[0]) - (xs[2] - xs[0]) * (gy - ys[0])) / d
        l2 = ((xs[1] - xs[0]) * (gy - ys[0]) - (gx - xs[0]) * (ys[1] - ys[0])) / d
        l0 = 1 - l1 - l2
        innen = (l0 >= -1e-6) & (l1 >= -1e-6) & (l2 >= -1e-6)
        if not innen.any():
            return
        z = l0 * zs[0] + l1 * zs[1] + l2 * zs[2]
        pxo = (page & 0xF) * 128
        clut_idx = ((clut >> 6) & 0x1FF) - t['clut_y']
        if clut_idx < 0 or clut_idx >= t['n_cluts']:
            clut_idx = 0   # render_pc.c:2775
        u = l0 * (U[0] + pxo) + l1 * (U[1] + pxo) + l2 * (U[2] + pxo)
        v = l0 * V[0] + l1 * V[1] + l2 * V[2] + clut_idx * one_h
        ui = np.clip(np.floor(u).astype(int), 0, tw - 1)
        vi = np.clip(np.floor(v).astype(int), 0, th - 1)
        texel = tex[vi, ui]
        sicht = innen & (texel[..., 3] > 0)
        sub_z = zbuf[y0:y1 + 1, x0:x1 + 1]
        naeher = sicht & (z < sub_z)
        sub_z[naeher] = z[naeher]
        sub_i = img[y0:y1 + 1, x0:x1 + 1]
        sub_i[naeher] = texel[..., :3][naeher]

    for m in meshes:
        mi = m['i']
        base = pos[mi] if mi < len(pos) else np.zeros(3)
        for k, tr in enumerate(m['tri']):
            vidx = (tr[1], tr[3], tr[5])
            P = (m['tv'][list(vidx)] + base) @ R.T
            uv = m['tri_uv'][k]
            flaeche(P, uv['u'], uv['v'], uv['clut'], uv['page'])
        for k, q in enumerate(m['quad']):
            vidx = (q[1], q[3], q[5], q[7])
            P4 = (m['qv'][list(vidx)] + base) @ R.T
            uv = m['quad_uv'][k]
            for a, bb, c in ((0, 1, 2), (1, 3, 2)):
                flaeche(P4[[a, bb, c]], (uv['u'][a], uv['u'][bb], uv['u'][c]), (uv['v'][a], uv['v'][bb], uv['v'][c]), uv['clut'], uv['page'])
    return img


def rot_y(g):
    c, s = np.cos(g), np.sin(g); return np.array([[c, 0, s], [0, 1, 0], [-s, 0, c]])


def rot_x(g):
    c, s = np.cos(g), np.sin(g); return np.array([[1, 0, 0], [0, c, -s], [0, s, c]])


def main():
    bericht = {'dateien': {}, 'punkte': {}}
    A = open(ASSET_EMD, 'rb').read(); AT = open(ASSET_TIM, 'rb').read()
    V = open(VAR_EMD, 'rb').read(); VT = open(VAR_TIM, 'rb').read()
    B = open(BASIS_EMD, 'rb').read()
    for n, p in (('asset_emd', ASSET_EMD), ('asset_tim', ASSET_TIM), ('var_emd', VAR_EMD), ('var_tim', VAR_TIM), ('basis_emd', BASIS_EMD)):
        bericht['dateien'][n] = {'pfad': p, 'sha256': sha(p), 'bytes': len(open(p, 'rb').read())}

    ta, tv = tim_lesen(AT), tim_lesen(VT)
    bericht['tim_asset'] = {k: int(v) for k, v in ta.items() if k not in ('clut', 'pix')}
    bericht['tim_variante'] = {k: int(v) for k, v in tv.items() if k not in ('clut', 'pix')}

    # (1) Zeilen <= 16, tex_h <= 4096
    p1 = {'zeilen': ta['n_cluts'], 'clut_h': ta['clut_h'], 'tex_h': ta['tex_h'], 'ok': ta['n_cluts'] <= 16 and ta['tex_h'] <= 4096,
          'variante_zeilen': tv['n_cluts'], 'variante_tex_h': tv['tex_h']}
    bericht['punkte']['P1_zeilen'] = p1

    # Meshes/Flaechen + CLUT-Zeilen
    DA, DV, DB = emd_lesen(A), emd_lesen(V), emd_lesen(B)
    bericht['dir_asset'] = DA; bericht['dir_variante'] = DV; bericht['dir_basis'] = DB
    MA, MV = md1_lesen(A, DA[7]), md1_lesen(V, DV[7])
    EA, EV = emr_lesen(A, DA[2]), emr_lesen(V, DV[2])
    clipsA = [edd_clips(A, DA[i]) for i in (1, 3, 5)]
    clipsV = [edd_clips(V, DV[i]) for i in (1, 3, 5)]
    clipsB = [edd_clips(B, DB[i]) for i in (1, 3, 5)]
    bericht['bank'] = {'asset': {'meshes': len(MA), 'knochen': EA['nb'], 'clips_paare': clipsA},
                       'variante': {'meshes': len(MV), 'knochen': EV['nb'], 'clips_paare': clipsV},
                       'basis_k5c': {'clips_paare': clipsB}}

    def zeilen_zaehlen(M, t):
        z = {}; n = 0; ungueltig = []
        for m in M:
            for art in ('tri_uv', 'quad_uv'):
                for uv in m[art]:
                    r = ((uv['clut'] >> 6) & 0x1FF) - t['clut_y']
                    z[r] = z.get(r, 0) + 1; n += 1
                    if r < 0 or r >= t['n_cluts']:
                        ungueltig.append((m['i'], art, uv['o'], uv['clut'], r))
                    # CLUT-x: Bits 0..5 * 16 muss clut_x sein
                    if ((uv['clut'] & 0x3F) * 16) != t['clut_x']:
                        ungueltig.append((m['i'], art, uv['o'], uv['clut'], 'clut_x'))
        return dict(sorted(z.items())), n, ungueltig

    zA, nA, badA = zeilen_zaehlen(MA, ta)
    zV, nV, badV = zeilen_zaehlen(MV, tv)
    bericht['punkte']['P1_uv_zeilen_asset'] = {'flaechen': nA, 'je_zeile': zA, 'ungueltig': len(badA), 'beispiele': badA[:5], 'ok': not badA}
    bericht['punkte']['P1_uv_zeilen_variante'] = {'flaechen': nV, 'je_zeile': zV, 'ungueltig': len(badV)}
    # Meshes je Zeile
    mz = {}
    for m in MA:
        for art in ('tri_uv', 'quad_uv'):
            for uv in m[art]:
                r = ((uv['clut'] >> 6) & 0x1FF) - ta['clut_y']; mz.setdefault(r, set()).add(m['i'])
    bericht['punkte']['P1_meshes_je_zeile'] = {k: sorted(v) for k, v in sorted(mz.items())}

    # (2) Byte-Vergleich EMD: alles ausser clut-Feldern gleich
    p2 = {'laenge_gleich': len(A) == len(V), 'dir_gleich': DA == DV}
    clut_offs_A = set()
    for m in MA:
        for art in ('tri_uv', 'quad_uv'):
            for uv in m[art]:
                clut_offs_A.add(uv['clut_o']); clut_offs_A.add(uv['clut_o'] + 1)
    if len(A) == len(V):
        a = np.frombuffer(A, 'u1'); v = np.frombuffer(V, 'u1')
        diff = np.nonzero(a != v)[0]
        ausser = [int(i) for i in diff if int(i) not in clut_offs_A]
        p2['diff_bytes'] = int(len(diff)); p2['diff_ausserhalb_clut'] = len(ausser); p2['beispiele_ausserhalb'] = ausser[:10]
        # geaenderte clut-Worte zaehlen
        worte = 0; abb = {}
        for m, mv in zip(MA, MV):
            for art in ('tri_uv', 'quad_uv'):
                for ua, uvv in zip(m[art], mv[art]):
                    if ua['clut'] != uvv['clut']:
                        worte += 1
                        ra = ((ua['clut'] >> 6) & 0x1FF) - ta['clut_y']; rv = ((uvv['clut'] >> 6) & 0x1FF) - tv['clut_y']
                        abb[(rv, ra)] = abb.get((rv, ra), 0) + 1
                    else:
                        ra = ((ua['clut'] >> 6) & 0x1FF) - ta['clut_y']; rv = ((uvv['clut'] >> 6) & 0x1FF) - tv['clut_y']
                        abb[(rv, ra)] = abb.get((rv, ra), 0) + 1
        p2['clut_worte_geaendert'] = worte
        p2['abbildung_variante_zu_asset'] = {f'{k[0]}->{k[1]}': n for k, n in sorted(abb.items())}
        # Abbildung eindeutig? jede Variantenzeile -> genau eine Assetzeile
        quelle = {}
        for (rv, ra) in abb:
            quelle.setdefault(rv, set()).add(ra)
        p2['abbildung_eindeutig'] = all(len(s) == 1 for s in quelle.values())
        p2['ok'] = p2['dir_gleich'] and len(ausser) == 0 and p2['abbildung_eindeutig']
        abbildung = {rv: next(iter(s)) for rv, s in quelle.items()}
    else:
        p2['ok'] = False; abbildung = {}
    # Sektionen einzeln (Info): EDD/EMR byte-gleich?
    sek = {}
    grenzenA = sorted(set(DA)) + [len(A)]
    for i in range(8):
        s, e = DA[i], min(x for x in grenzenA if x > DA[i])
        sek[f'dir[{i}]'] = {'off': DA[i], 'len': e - s, 'gleich': A[s:e] == V[DV[i]:DV[i] + (e - s)]}
    p2['sektionen'] = sek
    bericht['punkte']['P2_emd_bytegleich'] = p2

    # (2b) TIM: Pixel gleich, Zeilen des Assets = Quellzeilen der Variante (dE76 je Eintrag), Zusammenlegung?
    p2b = {'pixel_gleich': bool(np.array_equal(ta['pix'], tv['pix'])), 'masse_gleich': (ta['width'], ta['height']) == (tv['width'], tv['height']),
           'clut_x_y_gleich': (ta['clut_x'], ta['clut_y']) == (tv['clut_x'], tv['clut_y'])}
    zeilen_de = {}
    for rv, ra in sorted(abbildung.items()):
        za = ta['clut'][ra * 256:(ra + 1) * 256]; zv = tv['clut'][rv * 256:(rv + 1) * 256]
        d = de76(psx_rgb(za), psx_rgb(zv))
        zeilen_de[f'{rv}->{ra}'] = {'max_dE76': float(d.max()), 'eintraege_ungleich': int((za != zv).sum()), 'bytegleich': bool(np.array_equal(za, zv))}
    p2b['zeilen_dE'] = zeilen_de
    # Welche Variantenzeile fehlt im Asset (entfernt)?
    entfernt = sorted(set(range(tv['n_cluts'])) - set(abbildung.keys()))
    p2b['variante_zeilen_ohne_ziel'] = entfernt
    p2b['entfernte_zeilen_referenziert'] = {r: zV.get(r, 0) for r in entfernt}
    # Zusammenlegung = zwei Variantenzeilen auf dieselbe Assetzeile
    ziele = {}
    for rv, ra in abbildung.items():
        ziele.setdefault(ra, []).append(rv)
    zusammen = {ra: rvs for ra, rvs in ziele.items() if len(rvs) > 1}
    p2b['zusammenlegungen'] = {str(k): v for k, v in zusammen.items()}
    for ra, rvs in zusammen.items():
        za = ta['clut'][ra * 256:(ra + 1) * 256]
        for rv in rvs:
            zv = tv['clut'][rv * 256:(rv + 1) * 256]
            p2b['zeilen_dE'][f'{rv}->{ra}']['zusammenlegung'] = True
    # Info: dE Variantenzeile 2 gegen alle anderen (nur Dokumentation)
    if entfernt:
        r = entfernt[0]; zv = tv['clut'][r * 256:(r + 1) * 256]
        p2b['info_dE_entfernte_zeile_gegen_andere'] = {k: float(de76(psx_rgb(zv), psx_rgb(tv['clut'][k * 256:(k + 1) * 256])).max()) for k in range(tv['n_cluts']) if k != r}
    p2b['ok'] = p2b['pixel_gleich'] and p2b['masse_gleich'] and p2b['clut_x_y_gleich'] and all(z['bytegleich'] for z in zeilen_de.values()) \
        and all(zV.get(r, 0) == 0 for r in entfernt) and len(abbildung) == tv['n_cluts'] - len(entfernt)
    bericht['punkte']['P2_tim_zeilen'] = p2b

    # Basis-EMD (3 Clips) gegen Asset: MD1 bytegleich?
    MB = md1_lesen(B, DB[7])
    md1_len_A = min(x for x in grenzenA if x > DA[7]) - DA[7]
    grenzenB = sorted(set(DB)) + [len(B)]
    md1_len_B = min(x for x in grenzenB if x > DB[7]) - DB[7]
    bericht['punkte']['P2_basis_md1'] = {'md1_len_asset': md1_len_A, 'md1_len_basis': md1_len_B,
                                         'bytegleich': A[DA[7]:DA[7] + md1_len_A] == B[DB[7]:DB[7] + md1_len_B]}

    # (2c) Render-Diff zwei Szenen Variante gegen Asset (eigener Renderer, Bind-Pose)
    texA, texV = textur_rgba(ta), textur_rgba(tv)
    pos = welt_pos(EA)
    alle = np.concatenate([m['tv'] + pos[m['i']] for m in MA if len(m['tv'])] + [m['qv'] + pos[m['i']] for m in MA if len(m['qv'])])
    szenen = {'S1_seite': rot_y(np.pi / 2), 'S2_schraeg_oben': rot_x(-0.5) @ rot_y(0.7)}
    rd = {}
    for name, R in szenen.items():
        pc = alle @ R.T
        mitte = (pc.min(0) + pc.max(0)) / 2
        ausdehnung = (pc.max(0) - pc.min(0))[:2]
        massstab = 0.9 * min(640 / max(ausdehnung[0], 1), 480 / max(ausdehnung[1], 1))
        kam = (R, massstab, mitte)
        ia = rendern(MA, EA, texA, ta, kam)
        iv = rendern(MV, EV, texV, tv, kam)
        Image.fromarray(ia).save(f'{OUT}/render_{name}_asset.png'); Image.fromarray(iv).save(f'{OUT}/render_{name}_variante.png')
        dmask = (ia != iv).any(-1)
        n = int(dmask.sum())
        maxde = float(de76(ia[dmask].astype(float), iv[dmask].astype(float)).max()) if n else 0.0
        belegt = int((ia.sum(-1) > 0).sum())
        # Empfindlichkeit: Asset mit vertauschten Zeilen 0<->5 rendern
        ta2 = dict(ta); c2 = ta['clut'].copy(); c2[0:256], c2[5 * 256:6 * 256] = ta['clut'][5 * 256:6 * 256].copy(), ta['clut'][0:256].copy(); ta2['clut'] = c2
        ie = rendern(MA, EA, textur_rgba(ta2), ta2, kam)
        emask = (ie != ia).any(-1); ne = int(emask.sum())
        maxde_e = float(de76(ie[emask].astype(float), ia[emask].astype(float)).max()) if ne else 0.0
        rd[name] = {'pixel_belegt_asset': belegt, 'pixel_diff': n, 'max_dE76': maxde,
                    'empfindlichkeit_zeilen_0_5_vertauscht': {'pixel_diff': ne, 'max_dE76': maxde_e}, 'ok': n == 0 and ne > 0}
        Image.fromarray(np.concatenate([iv, ia, ie], 1)).save(f'{OUT}/render_{name}_tafel.png')
    bericht['punkte']['P2_render_diff'] = rd

    gesamt = all(bericht['punkte'][k].get('ok', True) for k in ('P1_zeilen', 'P1_uv_zeilen_asset', 'P2_emd_bytegleich', 'P2_tim_zeilen')) \
        and bericht['punkte']['P2_basis_md1']['bytegleich'] and all(r['ok'] for r in rd.values())
    bericht['gesamt_ok'] = gesamt
    json.dump(bericht, open(f'{OUT}/pruef_einbau.json', 'w'), indent=1, default=str)
    print(json.dumps(bericht, indent=1, default=str))


if __name__ == '__main__':
    main()
