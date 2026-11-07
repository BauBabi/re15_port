"""Pruefer V3: schwarz-rote Anteile je Nutzer-Region, Texel T = e / Licht. Zwei Lichter nebeneinander:
(a) Konstante Figurlicht farbe_palette.json (wie Leser B und p_zonen.rueck), (b) Licht je Pixel aus dem K5c-Szenenrender (eigen).
Dazu die G4-Atlas-Anteile derselben Klassen (Soll-Familie fuer die Uebertragung)."""
from szene import *
import json
sys.path.insert(0, K5 + '/farbe_lesung/P_skripte')
import p_zonen as PZ

FP = json.load(open(FORM + '/farbe_palette.json', encoding='utf-8'))
LK = {1: np.array(FP['licht']['bild1']['faktor_figur']), 2: np.array(FP['licht']['bild2']['faktor'])}


def klassen(T):
    L = lab(T.astype(np.float32))
    C = np.hypot(L[:, 1], L[:, 2]); h = np.degrees(np.arctan2(L[:, 2], L[:, 1]))
    dunkel = L[:, 0] < 20
    rot = (L[:, 1] >= 15) & (L[:, 0] >= 20) & (h > -30) & (h < 70)
    hell = (L[:, 0] >= 45) & (L[:, 1] < 10)
    r = dict(n=int(len(T)), dunkel=round(100 * float(dunkel.mean()), 1), dunkelrot=round(100 * float((dunkel & (L[:, 1] >= 8)).mean()), 1),
             rot=round(100 * float(rot.mean()), 1), hell=round(100 * float(hell.mean()), 1),
             T_dunkel=[round(float(x)) for x in T[dunkel].mean(0)] if dunkel.any() else None,
             lab_dunkel=[round(float(x), 1) for x in L[dunkel].mean(0)] if dunkel.any() else None,
             lab_rot=[round(float(x), 1) for x in L[rot].mean(0)] if rot.any() else None,
             ton_rot=round(float(np.degrees(np.arctan2(L[rot, 2].mean(), L[rot, 1].mean()))), 1) if rot.any() else None,
             L_p5_50_95=[round(float(np.percentile(L[:, 0], p)), 1) for p in (5, 50, 95)])
    return r


R = {}
for n in (1, 2):
    V = P.original(n); e = np.clip(FV.video_umkehr(V, n), 0, 255)
    e = cv2.GaussianBlur(e.astype(np.float32), (0, 0), 1.0)
    Lp = np.load('L%d.npy' % n); m = np.load('m%d.npy' % n)
    clip = V.min(2) >= 200
    fig = np.zeros((480, 640), bool)
    for nm, rg in PZ.regionen(n):
        q = rg & ~clip
        qa = q
        qb = q & m & (Lp.min(2) > 0.05)
        R['bild%d|%s' % (n, nm)] = dict(a_figurlicht=klassen(np.clip(e[qa] / LK[n], 0, 255)), b_licht_je_pixel=klassen(np.clip(e[qb] / Lp[qb], 0, 255)),
                                        licht_je_pixel_median=[round(float(x), 3) for x in np.median(Lp[qb], 0)])
        fig |= q
    qb = fig & m & (Lp.min(2) > 0.05)
    R['bild%d|Figur' % n] = dict(a_figurlicht=klassen(np.clip(e[fig] / LK[n], 0, 255)), b_licht_je_pixel=klassen(np.clip(e[qb] / Lp[qb], 0, 255)),
                                 licht_je_pixel_median=[round(float(x), 3) for x in np.median(Lp[qb], 0)])
# G4-Atlas, genutzte Texel (eigene UV-Rasterung)
sys.path.insert(0, r'C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/formfarbe/A')
from lade import modelle
G4, tim, _ = modelle()['G4']
tx = FB.Textur(tim)
used = np.zeros((tx.h, tx.w), bool); rgb = np.zeros((tx.h, tx.w, 3), np.uint8)
for mm in G4.meshes:
    for faces, uvs, q in ((mm.tf, mm.tuv, 0), (mm.qf, mm.quv, 1)):
        for uv in uvs:
            po = (uv[5] & 0xF) * 128; z = ((uv[2] >> 6) & 0x1FF) - tx.clut_y
            pts = [(uv[0] + po, uv[1]), (uv[3] + po, uv[4]), (uv[6] + po, uv[7])] + ([(uv[9] + po, uv[10])] if q else [])
            if q:
                pts = [pts[0], pts[1], pts[3], pts[2]]
            mk_ = np.zeros((tx.h, tx.w), np.uint8)
            cv2.fillPoly(mk_, [np.array(pts, np.int32)], 1)
            mk_ = mk_.astype(bool) & ~used
            rgb[mk_] = tx.lut[z][tx.px[mk_]][:, :3]
            used |= mk_
R['G4_atlas_genutzt'] = klassen(rgb[used].astype(np.float32))
json.dump(R, open('schwarzrot.json', 'w', encoding='utf-8'), indent=1, ensure_ascii=False)
for k, v in R.items():
    if k.startswith('G4'):
        print(k, v); continue
    print(k, 'Licht', v['licht_je_pixel_median'])
    for a in ('a_figurlicht', 'b_licht_je_pixel'):
        x = v[a]
        print('   ', a, 'dunkel', x['dunkel'], 'dunkelrot', x['dunkelrot'], 'rot', x['rot'], 'hell', x['hell'], 'T_dunkel', x['T_dunkel'], 'lab_rot', x['lab_rot'], 'ton', x['ton_rot'], 'L', x['L_p5_50_95'])
