"""Nachtrag Rot (Messer): Ketten-Vorhersage im Speicher (kein Bau). Die roten Texel der Variante V3 r3 (Texel-a* >= 8, Schwelle Bild 1 T)
werden im Szenenrender auf Farbton H und Buntheit x c gesetzt (L* bleibt), Licht je Pixel = sauber / Albedo bleibt; dann video_filter (Saat 15)
und dieselben Masse wie mess.py. Zweck: welche Texelwerte die Ziele der Kette treffen (Spielraum fuer den Bauer), nicht Teil der Ziele."""
from basis import *
import itertools
from mess_hilfe import masse

E = json.load(open('mess.json', encoding='utf-8'))
A_TEX = E['bilder']['1']['A0']['T']
OUT = {}
for n in (1, 2):
    d = np.load('szene%d.npz' % n)
    sauber, alb, basis, reg = d['sauber'].astype(np.float32), d['alb'].astype(np.float32), d['basis'], d['reg']
    A0 = E['bilder'][str(n)]['A0']['T']
    fig = alb.max(2) > 0
    licht = np.where(fig[..., None], sauber / np.maximum(alb, 1.0), 0)
    La = lab(alb)
    rotx = fig & (La[..., 1] >= A_TEX)
    C0 = np.hypot(La[..., 1], La[..., 2])
    for H, c in itertools.product(((-30, -20, -10, 'G4') if n == 1 else (-20, -10, 0, 'G4')), (1.0, 0.6)):
        L2 = La.copy()
        if H != 'G4':
            L2[rotx, 1] = c * C0[rotx] * math.cos(math.radians(H))
            L2[rotx, 2] = c * C0[rotx] * math.sin(math.radians(H))
        else:
            L2[rotx, 1:] *= c
        a2 = rgb_von_lab(L2)
        s2 = np.where(fig[..., None], np.clip(a2 * licht, 0, 255), sauber).astype(np.uint8)
        vid = np.asarray(PZ.FV.video_filter(s2, bild=n, entzerren=False, rauschen=True, saat=15), np.uint8)
        T = lab(PZ.rueck(vid, n)[0].astype(np.float32))
        r = {}
        for rn in REGIONEN + ['Figur']:
            rm = basis & ((reg == RID[rn]) if rn != 'Figur' else (reg >= 0))
            if rm.sum() >= 150:
                m = masse(T, rm, A0)
                r[rn] = dict(rot_eng=m['rot_eng'], h_rot=m['h_rot'], C_rot=m['C_rot'], dL=m['dL'], gross_p=m['gross_p'], gross_eng_p=m['gross_eng_p'])
        OUT['bild%d|H%s|c%.1f' % (n, H, c)] = r
        print(n, H, c, 'Figur', r['Figur'], flush=True)
json.dump(OUT, open('probe.json', 'w', encoding='utf-8'), indent=1, ensure_ascii=False)
