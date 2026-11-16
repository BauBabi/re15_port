"""Runde 3, Befund 11: Von wo aus ist jede Flaeche von Kopf 22 / Unterkiefer 23 zu sehen? Nur Kopf + Kiefer (+ optional Hals 21),
Tiefenpuffer, orthografisch, 162 Blickrichtungen gleichmaessig auf der Kugel, im Knochenraum des Kopfes (x = Schnauze, y = unten, z = Seite).
Je Flaeche: in wie vielen Richtungen sichtbar (>= 3 Bildpunkte), mittlere Betrachter-Richtung, Anteil 'Betrachter unten' (y > 0,5),
Anteil 'Betrachter vorn' (x > 0,5). Klasse:
  'Gelenkspalt' = (zuerst gepruefte Regel) Mittelstrahl trifft das Gegenstueck nach < 60 Einheiten;
  'Maulhoehle'  = Flaechenmitte im Zahn-x-Bereich (+-150), Aussen-Normale zeigt zum Gegenstueck (Kopf 22: nach unten, n_y > 0,3;
                  Kiefer 23: nach oben, n_y < -0,3) UND ihr Mittelstrahl trifft das Gegenstueck (22 -> 23 bzw. 23 -> 22) nach < 900 Einheiten
                  UND von unten (Betrachter-y > 0,5) in weniger als 10 Richtungen zu sehen;
  'aussen'      = sonst, sichtbar aus mindestens einer Richtung;
  'verdeckt'    = sonst, aus keiner Richtung sichtbar.
Aufruf: python r3_kugel.py <clip> <bild> <name>"""
import json
import sys
from s_basis import *  # noqa: F401,F403
import r3_lage as RL
import r3_kopfbild as KB
import material_lesen as ML


def kugel(n=162):
    i = np.arange(n) + 0.5
    y = 1 - 2 * i / n; r = np.sqrt(1 - y * y); phi = i * np.pi * (3 - np.sqrt(5))
    return np.stack([r * np.cos(phi), y, r * np.sin(phi)], 1)


if __name__ == '__main__':
    clip, bild, name = int(sys.argv[1]), int(sys.argv[2]), sys.argv[3]
    k = kand(ASSET); mo = k.mo; kf = k.kf(clip, bild)
    mat = ML.material_je_flaeche()
    K, teil, schl = KB.kopfraum(mo, kf, (22, 23))
    alle = sorted(set(schl.tolist()))
    sicht = {f: [] for f in alle}
    for d in kugel():
        blick = -d                       # Betrachter bei +d, blickt nach -d
        oben = (0, -1, 0) if abs(d[1]) < 0.9 else (1, 0, 0)
        img, idb, _xy, _a = KB.ansicht(K, teil, schl, blick, oben, B=240, rand=10)
        ids, cnt = np.unique(idb[idb >= 0], return_counts=True)
        z = {}
        for i, c in zip(ids, cnt):
            z[schl[i]] = z.get(schl[i], 0) + int(c)
        for f, c in z.items():
            if c >= 3:
                sicht[f].append(d.tolist())
    # Mittelstrahl der Normale gegen das Gegenstueck
    F, tris, tteil, tschl = RL.flaechen(mo, kf)
    R, T = mo.pose(kf)
    Rk = R[22].astype(float) / 4096.0; Tk = T[22].astype(float)
    zahn_x = KB.json.load(open(ZIEL + '/messung/r3_kopf_griff.json', encoding='utf-8'))['lage_kopfraum']['zaehne_xyz_bereich'][0]
    O = np.array([F[f][2] + 2 * F[f][1] for f in alle]); D = np.array([F[f][1] for f in alle])
    erg = {}
    for gegen_teil in (22, 23):
        sel = tteil == (23 if gegen_teil == 22 else 22)
        t, j = RL.strahlen(O, D, tris[sel])
        for n_, f in enumerate(alle):
            if int(f.split('|')[0]) != gegen_teil:
                continue
            treff = bool(np.isfinite(t[n_]))
            ck = (F[f][2] - Tk) @ Rk
            s = np.array(sicht[f]) if sicht[f] else np.zeros((0, 3))
            nk = F[f][1] @ Rk
            im_zahnbereich = zahn_x[0] - 150 <= ck[0] <= zahn_x[1] + 150
            zum_gegen = (nk[1] > 0.3) if gegen_teil == 22 else (nk[1] < -0.3)
            if treff and t[n_] < 60:
                kl = 'Gelenkspalt'
            elif treff and t[n_] < 900 and im_zahnbereich and zum_gegen and int((s[:, 1] > 0.5).sum() if len(s) else 0) < 10:
                kl = 'Maulhoehle'
            elif len(s) == 0:
                kl = 'verdeckt'
            else:
                kl = 'aussen'
            erg[f] = dict(klasse=kl, richtungen=len(s), betrachter_unten=float((s[:, 1] > 0.5).mean()) if len(s) else 0.0,
                          betrachter_vorn=float((s[:, 0] > 0.5).mean()) if len(s) else 0.0,
                          mittel_richtung=[round(float(x), 2) for x in s.mean(0)] if len(s) else None,
                          gegenstueck_getroffen=treff, abstand=round(float(t[n_]), 1) if treff else None,
                          mitte_kopfraum=[int(round(x)) for x in ck], normale_kopfraum=[round(float(x), 2) for x in nk], material=mat.get(f, '?'),
                          zonen=RL.ZONE_VON.get(f, []))
    from collections import Counter
    zus = {'klasse_je_material': {}, 'zone30': {f: erg[f] for f in RL.Z30}}
    for f, e in erg.items():
        zus['klasse_je_material'].setdefault(e['material'], Counter())[e['klasse']] += 1
    zus['klasse_je_material'] = {m: dict(c) for m, c in zus['klasse_je_material'].items()}
    zus['maulhoehle_flaechen'] = {f: (e['material'], e['zonen']) for f, e in erg.items() if e['klasse'] == 'Maulhoehle'}
    zus['maulinneres_nicht_maulhoehle'] = {f: e['klasse'] for f, e in erg.items() if e['material'] == 'Maulinneres' and e['klasse'] != 'Maulhoehle'}
    zus['pose'] = dict(clip=clip, bild=bild, kieferwinkel=round(RL.kieferwinkel(mo, kf), 2))
    json.dump(dict(zusammenfassung=zus, je_flaeche=erg), open(ZIEL + '/messung/r3_kugel_%s.json' % name, 'w', encoding='utf-8'),
              ensure_ascii=False, indent=1)
    print(json.dumps(zus['klasse_je_material'], ensure_ascii=False, indent=1))
    print('Maulhoehle:', len(zus['maulhoehle_flaechen']), Counter(v[0] for v in zus['maulhoehle_flaechen'].values()))
    for f in RL.Z30:
        e = erg[f]
        print(f, e['klasse'], e['richtungen'], 'unten %.2f vorn %.2f' % (e['betrachter_unten'], e['betrachter_vorn']), e['mittel_richtung'],
              e['gegenstueck_getroffen'], e['abstand'], e['mitte_kopfraum'], e['normale_kopfraum'])
