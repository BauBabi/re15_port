"""Runde 3, Pruefer-Befund 17: Die roten Hals-Streifen ueber ALLE Bilder und ueber die Asset-Staende messen (nicht nur die Keile).
Je Stand:
 (a) Textur: die Flaechen von Kopf 22 an der Stelle der 6 Streifen-Flaechen (Flaechenmitte im Knochenraum <= 40 Einheiten von der
     K5e_f1-Flaeche; bei gleicher Gliederung dieselbe Nummer) - Texturzeile und Anteil warm-roter Texel (Rot-Regel wie s_hals.py).
 (b) Je Bild (alle Clips, alle Bilder): rote Bildpunkte auf Hals 21 / Kopf 22 / Kiefer 23 im Tiefenpuffer (= vorderste Flaeche, also
     wirklich zu sehen), ganze Figur gerastert, Texel ohne Licht, 640 x 480, zwei Ansichten:
       'Spiel 5090' = Kamera 5090 Schnitt 11, Lage 941/0/-23776, Gier 3874 (Bild-2-Stand);
       'von unten'  = Kamera 40 Grad unter der Figur, von vorn (Gier 0, Blick-Azimut 0), wie hals.json.
     Davon die Bildpunkte, die von den Streifen-Flaechen kommen.
Aufruf: python r3_rot_alle.py <name> <ordner> [schritt]   (schritt = jedes n-te Bild, Standard 1 = alle)"""
import json
import sys
import time
from multiprocessing import Pool
from s_basis import *  # noqa: F401,F403
import s_hals as SH

REF = {'22|1|1': (14, 210, 586), '22|1|8': (14, 210, -586), '22|1|9': (14, 240, -257), '22|1|194': (412, 303, 126),
       '22|1|201': (412, 303, -126), '22|1|209': (0, 374, -155)}
_K = {}


def streifen_flaechen(k):
    """Flaechen von Teil 22 an den Stellen der 6 (Knochenraum-Mitte <= 40 Einheiten)."""
    m = k.mo.meshes[22]
    out = {}
    for viereck, faces, verts in ((0, m.tf, m.tv), (1, m.qf, m.qv)):
        for fi, g in enumerate(faces):
            c = np.array([verts[i][:3] for i in g[1::2]], float).mean(0)
            for r, rc in REF.items():
                if np.linalg.norm(c - np.array(rc)) <= 40:
                    out['22|%d|%d' % (viereck, fi)] = r
    return out


def kameras():
    R5090 = FB.Raum('5090'); s = R5090.schnitt(11)
    return {'Spiel 5090': (FB.Kamera.aus_rid(s['pos'], s['ziel'], fov=s['fov']), (941, 0, -23776), 3874),
            'von unten': (SH.kamera_blick((0, -1200, 0), 0, 40), (0, 0, 0), 0)}


def eins(arg):
    ordner, clip, bild = arg
    if ordner not in _K:
        k = kand(ordner)
        _K[ordner] = (k, kameras(), set(streifen_flaechen(k)))
    k, kams, sf = _K[ordner]
    kf = k.kf(clip, bild)
    r = dict(clip=clip, bild=bild)
    for nm, (kam, lage, gier) in kams.items():
        tris = dreiecke_3d(k.mo, kf, lage, gier, kam)
        # nur Dreiecke, deren Bildschirm-Huellrechteck das von Hals/Kopf/Kiefer beruehrt (Verdeckung dort bleibt vollstaendig)
        hk = [t for t in tris if t[4][0] in (21, 22, 23)]
        if hk:
            xs = [q[0] for t in hk for q in t[1]]; ys = [q[1] for t in hk for q in t[1]]
            bx0, bx1, by0, by1 = min(xs), max(xs), min(ys), max(ys)
            tris = [t for t in tris if not (max(q[0] for q in t[1]) < bx0 or min(q[0] for q in t[1]) > bx1 or
                                            max(q[1] for q in t[1]) < by0 or min(q[1] for q in t[1]) > by1)]
        R = rastern_beide(tris, k.tex, 480, 640, np.diag([2.0, 2.0, 1.0]))
        tm = tri_mesh(tris)
        ip = R['puffer_i']
        mp = np.where(ip >= 0, tm[np.maximum(ip, 0)], -1)
        rp = SH.rot(R['rgb_puffer']) & (ip >= 0) & np.isin(mp, (21, 22, 23))
        tags = [SH.schluessel(t[4]) for t in tris]
        aus_sf = np.zeros(ip.shape, bool)
        idx = [i for i, t in enumerate(tags) if t in sf]
        if idx:
            aus_sf = np.isin(ip, idx)
        r[nm] = dict(rot_px=int(rp.sum()), rot_px_streifen=int((rp & aus_sf).sum()), streifen_px=int(aus_sf.sum()))
    return r


if __name__ == '__main__':
    name, ordner = sys.argv[1], sys.argv[2]
    k = kand(ordner)
    frot = SH.flaechen_rot(k)
    sf = streifen_flaechen(k)
    textur = {f: dict(an_stelle_von=r, zeile=int(frot[f][1]), rotanteil=round(frot[f][0], 3)) for f, r in sorted(sf.items())}
    jobs = [(ordner, c, b) for c, (f0, n) in enumerate(k.mo.clips) for b in range(n)]
    t0 = time.time()
    schritt = int(sys.argv[3]) if len(sys.argv) > 3 else 1
    jobs = jobs[::schritt]
    with Pool(6) as p:
        res = p.map(eins, jobs, chunksize=4)
    zus = {}
    for nm in ('Spiel 5090', 'von unten'):
        a = np.array([r[nm]['rot_px_streifen'] for r in res]); b = np.array([r[nm]['rot_px'] for r in res])
        zus[nm] = dict(bilder=len(res), bilder_mit_rot_aus_streifen=int((a > 0).sum()), median_rot_streifen=float(np.median(a)),
                       max_rot_streifen=int(a.max()), bilder_mit_rot_hals_kopf_kiefer=int((b > 0).sum()), median_rot_alle=float(np.median(b)))
        je_clip = {}
        for c in sorted(set(r['clip'] for r in res)):
            v = np.array([r[nm]['rot_px_streifen'] for r in res if r['clip'] == c])
            je_clip[c] = [int((v > 0).sum()), len(v), int(v.min()), int(np.median(v)), int(v.max())]
        zus[nm]['je_clip_bilder_mit_rot_von_n_min_median_max'] = je_clip
    erg = dict(stand=name, ordner=ordner, schritt=schritt, clips=len(k.mo.clips), textur_streifen_flaechen=textur, zusammenfassung=zus,
               je_bild=res, sekunden=round(time.time() - t0))
    json.dump(erg, open(ZIEL + '/messung/r3_rot_alle_%s.json' % name, 'w', encoding='utf-8'), ensure_ascii=False, indent=0)
    print(name, json.dumps(dict(textur=textur, zus={n: {k_: v for k_, v in z.items() if not k_.startswith('je_clip')} for n, z in zus.items()}),
                           ensure_ascii=False, indent=1), '%.0f s' % (time.time() - t0))
