"""Diagnose Runde 2: Lichtfaktor je Kopfpixel, Texel-(u,v)-Karte des Kopfes im Bild, Haelften, Auge/Zaehne unter Licht."""
import os, sys, json
import numpy as np, cv2
T = r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel/re15_port/tools'
for p in (T, T + '/r41_sichel/v4', T + '/r41_sichel/re2kopf', T + '/r41_sichel/v2'):
    sys.path.insert(0, p)
import v4_gemein as G, b2, kopf_re2_bau as KB, kopf_re2_mess as KM
OUT = os.path.dirname(os.path.abspath(__file__))
SP = KB.SP
emd = sys.argv[1] if len(sys.argv) > 1 else os.path.join(SP, 'sfv4', 'emd', 'EM30_V4re2.EMD')
mo = b2.modell(emd); ref = b2.modell(G.REF2)
P = G.posen()

def uv_karte(mo, pose):
    """Pass A/B: Texel der G2-Region durch u+1 / v+1 ersetzen, LUT Zeile 486 -> Graustufe i; raw Render liefert u, v je Pixel."""
    o = KB.G2_ORT; x0 = 128 * (o['page'] & 0xF) + o['u0']; z = KB.ZEILE_G2 - mo.clut_y
    tex0, lut0 = mo.tex, mo.lut
    res = []
    for pas in (0, 1):
        tex = tex0.copy(); lut = lut0.copy()
        sub = np.zeros((90, 126), np.uint8)
        if pas == 0:
            sub[:] = (np.arange(126)[None, :] + 1).astype(np.uint8)
        else:
            sub[:] = (np.arange(90)[:, None] + 1).astype(np.uint8)
        tex[o['v0']:o['v0'] + 90, x0:x0 + 126] = sub
        lut[z, :, 0] = np.arange(256); lut[z, :, 1] = np.arange(256); lut[z, :, 2] = np.arange(256); lut[z, :, 3] = 255
        lut[z, 0, 3] = 0
        mo.tex, mo.lut = tex, lut
        try:
            img, ids = G.zeichne(mo, pose, licht=False)
        finally:
            mo.tex, mo.lut = tex0, lut0
        res.append((img[..., 0].astype(int) - 1, ids))
    return res[0][0], res[1][0], res[0][1]

out = {}
for h in ('135', '85'):
    pose = P[h]
    il, ids, iv = G.zeichne(mo, pose, voll=True)
    ir, _ = G.zeichne(mo, pose, licht=False)
    Ll = G.lab(il.astype(float))[..., 0]; Lr = G.lab(ir.astype(float))[..., 0]
    k = ids == 9; r8 = ids == 8
    kb = G.kopfbox(ids); x0, y0, x1, y1 = kb; xm = 0.5 * (x0 + x1)
    fak = np.where(Lr > 1, Ll / np.maximum(Lr, 1), np.nan)
    U, Vv, ids2 = uv_karte(mo, pose)
    r_ids_diff = int((ids2 != ids).sum())
    X = np.arange(640)[None, :].repeat(480, 0)
    li, re_ = k & (X < xm), k & (X >= xm)
    r = dict(ids_diff_uvpass=r_ids_diff, kopfbox=kb, H=float(G.h_port(iv)[2]),
             kopf_L_lit=float(np.median(Ll[k])), kopf_L_raw=float(np.median(Lr[k])), rumpf_L_lit=float(np.median(Ll[r8])), rumpf_L_raw=float(np.median(Lr[r8])),
             fak_kopf_p50=float(np.nanmedian(fak[k])), fak_rumpf_p50=float(np.nanmedian(fak[r8])),
             haelften_lit=(float(np.median(Ll[li])), float(np.median(Ll[re_]))), haelften_raw=(float(np.median(Lr[li])), float(np.median(Lr[re_]))))
    # Texel-Belegung je Haelfte: Histogramm der u (G2-Spalten) links/rechts
    g2 = k & (U >= 0) & (U < 126)
    for nm, m in (('links', li & g2), ('rechts', re_ & g2)):
        us = U[m]; vs = Vv[m]
        r['u_' + nm] = dict(n=int(m.sum()), u_p10_p50_p90=[float(np.percentile(us, q)) for q in (10, 50, 90)], v_p50=float(np.median(vs)),
                            u_hist=np.histogram(us, bins=[0, 20, 41, 61, 80, 101, 126])[0].tolist())
    r['g3_px'] = int((k & ~g2).sum())
    # Lichtfaktor nach Bildspalte (Kopf), fuenf Streifen
    strf = []
    for i in range(5):
        a = x0 + (x1 - x0 + 1) * i / 5; b = x0 + (x1 - x0 + 1) * (i + 1) / 5
        m = k & (X >= a) & (X < b)
        strf.append([float(np.nanmedian(fak[m])) if m.any() else None, float(np.median(Ll[m])) if m.any() else None, float(np.median(Lr[m])) if m.any() else None])
    r['streifen_fak_Llit_Lraw'] = strf
    # Auge / Zaehne
    am, zm = KM.auge_texel(mo), KM.zahn_texel(mo)
    ma = KM.marker_render(mo, pose, am, KB.ZEILE_G2); mz = KM.marker_render(mo, pose, zm, KB.ZEILE_G3)
    for nm, m in (('auge', ma), ('zaehne', mz)):
        if m.any():
            r[nm] = dict(px=int(m.sum()), L_lit=float(np.median(Ll[m])), L_raw=float(np.median(Lr[m])), fak=float(np.nanmedian(fak[m])),
                         sp=[float(np.nonzero(m)[1].mean()), float(np.nonzero(m)[0].mean())])
    # Ref2
    il2, ids_r, iv2 = G.zeichne(ref, pose, voll=True); ir2, _ = G.zeichne(ref, pose, licht=False)
    L2l = G.lab(il2.astype(float))[..., 0]; L2r = G.lab(ir2.astype(float))[..., 0]
    kr = ids_r == 9; r8r = ids_r == 8
    r['ref2'] = dict(kopf_L_lit=float(np.median(L2l[kr])), kopf_L_raw=float(np.median(L2r[kr])), rumpf_L_lit=float(np.median(L2l[r8r])), rumpf_L_raw=float(np.median(L2r[r8r])),
                     fak_kopf=float(np.nanmedian((L2l / np.maximum(L2r, 1))[kr])))
    out[h] = r
    # Bilder: lit | raw | U-Karte | Faktor
    kbr = (max(x0 - 10, 0), max(y0 - 10, 0), min(x1 + 10, 639), min(y1 + 10, 479))
    def crop(a): return a[kbr[1]:kbr[3] + 1, kbr[0]:kbr[2] + 1]
    f = 4
    tiles = [cv2.resize(crop(il), None, fx=f, fy=f, interpolation=cv2.INTER_NEAREST), cv2.resize(crop(ir), None, fx=f, fy=f, interpolation=cv2.INTER_NEAREST)]
    um = np.zeros((480, 640, 3), np.uint8); um[..., 0] = np.clip(U * 2, 0, 255); um[..., 1] = np.clip(Vv * 2, 0, 255); um[~k] = 0
    fm = np.zeros((480, 640, 3), np.uint8); fm[..., :] = np.clip(np.nan_to_num(fak) * 200, 0, 255)[..., None].astype(np.uint8); fm[~k] = 0
    fm[ma] = (255, 255, 0); fm[mz] = (0, 255, 255)
    tiles += [cv2.resize(crop(um), None, fx=f, fy=f, interpolation=cv2.INTER_NEAREST), cv2.resize(crop(fm), None, fx=f, fy=f, interpolation=cv2.INTER_NEAREST)]
    cv2.imwrite(os.path.join(OUT, 'diag1_%s.png' % h), cv2.cvtColor(np.hstack(tiles), cv2.COLOR_RGB2BGR))
json.dump(out, open(os.path.join(OUT, 'diag1.json'), 'w'), indent=1)
print(json.dumps(out, indent=1))
