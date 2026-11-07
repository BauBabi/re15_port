"""Riegel B10-B17 (MESSBLATT_FARBE_V2 Abschnitt 2, Binnenzeichnung) an einer Variante und an der Familie.
Masse aus n2_mass.py (Definitionen dort). Grenzen = Familienspanne EM30-Haut / RE2-G4 / RE2-G5 (python n2_riegel.py --familie),
Toleranz nach Messblatt V1 Abschnitt 6 (K1/K2: 0,85-fach unten, 1,2-fach oben); Kohaerenz (B17) ohne Faktor, Grenze = kleinster Familienwert.

    python n2_riegel.py <Variantenordner> [--json aus.json]    (Ordner mit FORM.EMD/FORM.TIM; Koerper = Atlas x < 384, Kopf (384,0,128,104))
    python n2_riegel.py --familie                               (EM30, G4, G5, dazu r3 und K4 heute)
Exit 1, wenn ein Riegel der Variante ROT ist.
"""
import os, sys, json, math
import numpy as np, cv2
HIER = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HIER)
import n2_mass as N
from scipy import ndimage as ND

GRENZEN = dict(
    B10=(8.1, 23.2),     # Atlas Fleisch dunkle Linien %: Familie 9,50 (G4) .. 19,32 (G5); 0,85 x 9,50 / 1,2 x 19,32
    B11=(3.3, 9.9),      # Atlas Haut dunkle Linien %: Familie 3,89 (G4) .. 8,29 (G5)
    B12L=4.6,            # Atlas Kopf dunkle Linien %: EM30-Kopf 5,39, G4-Kopf 46,45 -> 0,85 x 5,39
    B12C=25.2,           # Atlas Kopf Canny-Dichte: EM30-Kopf 29,62, G4-Kopf 30,07 -> 0,85 x 29,62
    B13=(25.1, 36.2),    # Atlas Fleisch Canny-Dichte 6/12: Familie 29,55 (G4) .. 30,17 (G5)
    B14R=8.2,            # Render Rotflaechen L*-Streuung gepoolt: Familie 9,6 (EM30) .. 11,2 (G5) -> 0,85 x 9,6
    B14A=9.3,            # Atlas dasselbe: Familie 10,9 (EM30) .. 14,5 (G5) -> 0,85 x 10,9
    B15=4.25,            # Render Rotflaechen a*-Streuung gepoolt: Familie 5,0 (EM30) .. 8,7 (G5) -> 0,85 x 5,0
    B16=50.7,            # Render Rotpixel in Rotflaechen mit >= 2 Rotstufen %: Familie 59,6 (EM30) .. 81,1 (G4) -> 0,85 x 59,6
    B17R=0.60,           # Render lokale Kohaerenz an Kanten, flaechengleich, Mittel 3 Winkel: kleinster Einzelwinkel der Familie 0,601 (EM30 3/4 vorn)
    B17A=0.576,          # Atlas lokale Kohaerenz (kantengewichtet je Feld): kleinster Familienwert EM30 0,576
)
ZIEL_PX = 30000          # Figurbildpunkte je Winkel fuer B17 (Familie bei 380 px: 24 000-36 700, K5c 15 900; flaechengleich statt umkugelgleich)


def render_koh(mo, clip):
    import render as R
    px = np.mean([R.blick(mo, R.kf(mo, clip), az, el, 380, None)[1].sum() for _wt, az, el in R.WINKEL])
    g = int(round(380 * math.sqrt(ZIEL_PX / px)))
    ko = []
    for _wt, az, el in R.WINKEL:
        img, msk = R.blick(mo, R.kf(mo, clip), az, el, g, None)
        valid = cv2.erode(msk.astype(np.uint8), np.ones((3, 3), np.uint8)).astype(bool)
        L = np.zeros(img.shape, np.float32)
        L[msk] = N.PL.lab(img[msk].astype(np.float32))
        e = N.kanten(L[..., 0], valid, N.SCHW['6/12'])
        _gx, _gy, coh = N.tensor(L[..., 0])
        ko.append(float(coh[e].mean()))
    return g, ko


def messen(mo, clip, zkoerper, zkopf):
    a = N.atlas_mass(mo, zkoerper, 'koerper')
    k = N.atlas_mass(mo, zkopf, 'kopf') if zkopf is not None else None
    r = N.render_mass(mo, clip, 'render')
    g, ko = render_koh(mo, clip)
    return dict(atlas=a, kopf=k, render=r, koh_gross=g, koh=ko)


def riegel(m):
    G = GRENZEN
    a, k, r = m['atlas'], m['kopf'], m['render']['rot_alle']
    out = {}
    v = a['linien_fleisch']['anteil_p']
    out['B10 Atlas Fleisch dunkle Linien %'] = (v, G['B10'][0] <= v <= G['B10'][1])
    v = a['linien_haut']['anteil_p']
    out['B11 Atlas Haut dunkle Linien %'] = (v, G['B11'][0] <= v <= G['B11'][1])
    if k is not None:
        vl, vc = k['linien_zone']['anteil_p'], k['kanten_6/12']['zone']['dichte']
        out['B12 Atlas Kopf Linien % / Canny-Dichte'] = ((vl, vc), vl >= G['B12L'] and vc >= G['B12C'])
    v = a['kanten_6/12']['fleisch']['dichte']
    out['B13 Atlas Fleisch Canny-Dichte'] = (v, G['B13'][0] <= v <= G['B13'][1])
    vr, va = r.get('sdL_pool', 0.0), a['rot'].get('sdL_pool', 0.0)
    out['B14 Rotflaechen L*-Streuung Render / Atlas'] = ((vr, va), vr >= G['B14R'] and va >= G['B14A'])
    v = r.get('sda_pool', 0.0)
    out['B15 Render Rotflaechen a*-Streuung'] = (v, v >= G['B15'])
    v = r.get('stufen_ge2_gew_p', 0.0)
    out['B16 Render Rotpixel in Flaechen mit >= 2 Stufen %'] = (v, v >= G['B16'])
    vr = round(float(np.mean(m['koh'])), 3)
    va = a.get('koh_lokal_gew', 0.0)
    out['B17 Kohaerenz Render (flaechengleich, Mittel) / Atlas'] = ((vr, [round(x, 3) for x in m['koh']], va), vr >= G['B17R'] and va >= G['B17A'])
    return out


def drucken(name, res):
    print('== %s' % name)
    for kk, (v, ok) in res.items():
        print('  %-52s %-44s %s' % (kk, v, 'GRUEN' if ok else 'ROT'))
    g = sum(1 for _v, ok in res.values() if ok)
    print('  -> %d/%d GRUEN' % (g, len(res)))
    return g


if __name__ == '__main__':
    import fl
    Z = N.zonen()
    if '--familie' in sys.argv:
        import kf_tafeln as KT
        alle = {}
        for nm, mo, clip, zk, zh in (('EM30', fl.lade_em30(), 0, Z['em30'], Z['em30kopf']), ('G4', KT.re2_mo(0x34), 0, Z['g4'], Z['g4kopf']),
                                     ('G5', KT.re2_mo(0x36), 0, Z['g5'], None),
                                     ('r3', fl.lade_var(N.K5 + '/farbe/K5c_farbe_r3'), 2, Z['k5'], Z['k5kopf']),
                                     ('K4', fl.lade_var(N.K5 + '/K4_heute'), 2, Z['k5'], Z['k5kopf'])):
            r = riegel(messen(mo, clip, zk, zh))
            drucken(nm, r)
            alle[nm] = {k_: dict(wert=str(v), ok=bool(ok)) for k_, (v, ok) in r.items()}
        json.dump(alle, open(os.path.join(N.FL, 'N2_riegel_familie.json'), 'w', encoding='utf-8'), indent=1, ensure_ascii=False)
        sys.exit(0)
    vd = sys.argv[1]
    r = riegel(messen(fl.lade_var(vd), 2, Z['k5'], Z['k5kopf']))
    g = drucken(vd, r)
    if '--json' in sys.argv:
        json.dump({k_: dict(wert=str(v), ok=bool(ok)) for k_, (v, ok) in r.items()},
                  open(sys.argv[sys.argv.index('--json') + 1], 'w', encoding='utf-8'), indent=1, ensure_ascii=False)
    sys.exit(0 if g == len(r) else 1)
