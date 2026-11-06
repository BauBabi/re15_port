#!/usr/bin/env python3
"""Riegel MESSBLATT_FARBE_V1 Abschnitt 6, alle 24 Zeilen, dazu ab Runde 2 die Befund-Riegel B1-B5 (Pruefung r1) (A1-A12 Atlas, K1-K10 Kette, N1-N2 neutraler Render gegen EM30) an einer Variante.
Atlas und Kette: Pruefer-Skript farbe_lesung/P_skripte/p_riegel.py (atlas_riegel, kette_riegel; dieselbe Definition, unveraendert).
N1/N2: Leser-B-Werkzeug farbe_lesung/B_skripte/render.py (form_buehne.rastern, Licht 0,55 + 0,45 max(0, N.L), weiss, Umkugel 0,46, 3 Winkel, roh,
EM30 Clip 0 Bild 0, Variante Clip 2 Ruhe Bild 0, Kachel 380 px wie lauf_render.py). Grenzen N1 >= 0,75 x EM30 (live gerechnet), N2 <= 0,10 %.

    python riegel_farbe.py <Variantenordner> [--gegenprobe <K5c heute>] [--gegenprobe-r1 <Runde 1>] [--json <aus.json>]
    python riegel_farbe.py --familie   (B1-B3 an EM30/G5/G4)

Exit 1, wenn ein Riegel der Variante ROT ist, oder wenn die Gegenprobe (z. B. K5c heute) NICHT rot ist (dann misst der Riegel nichts)."""
import argparse
import json
import os
import sys

HIER = os.path.dirname(os.path.abspath(__file__))
FL = os.path.normpath(os.path.join(HIER, '..', 'farbe_lesung'))
sys.path.insert(0, os.path.join(FL, 'P_skripte'))
sys.path.insert(0, os.path.join(FL, 'B_skripte'))
import p_riegel as PR  # noqa: E402
from p_lade import var  # noqa: E402


def n_riegel(vdir):
    import fl
    import render as R
    em = fl.lade_em30()
    mo = fl.lade_var(vdir)
    out = {}
    c_em, c_v, h_v = [], [], []
    for wt, az, el in R.WINKEL:
        ie, me = R.blick(em, R.kf(em, 0), az, el, 380, None)
        iv, mv = R.blick(mo, R.kf(mo, 2), az, el, 380, None)
        a, b = R.masse(ie, me), R.masse(iv, mv)
        c_em.append(a['C'])
        c_v.append(b['C'])
        h_v.append(b['hell85'])
    out['N1 neutral C* je Winkel (Variante / EM30)'] = ((tuple(c_v), tuple(c_em)), all(v >= 0.75 * e for v, e in zip(c_v, c_em)))
    out['N2 neutral L*>85 % je Winkel'] = (tuple(h_v), all(h <= 0.10 for h in h_v))
    return out


# ---------------------------------------------------------------- Runde 2: Riegel B1-B6 zu den Befunden der Pruefung r1 (FARBE_PRUEFUNG_r1_sicht.md P1/P2/P4,
# FARBE_PRUEFUNG_r1_zahlen.md 6.1/6.6). Familienwerte (EM30 / G5 / G4) mit demselben Code: python riegel_farbe.py --familie.
K5C_HEUTE = os.path.normpath(os.path.join(HIER, '..', 'K5c'))


def _struktur(img, msk):
    import cv2
    import numpy as np
    import fl
    L = np.zeros(img.shape, np.float32)
    L[msk] = fl.lab(img[msk])
    C = np.hypot(L[..., 1], L[..., 2])
    rot = msk & (L[..., 1] > 15) & (C > 20)                       # Rotpixel wie Sicht-Pruefer r1 P1 (= Riegel A3 ohne Farbtonfenster)
    er = cv2.erode(rot.astype(np.uint8), np.ones((3, 3), np.uint8)).astype(bool)
    n, _lab, st, _c = cv2.connectedComponentsWithStats(rot.astype(np.uint8), connectivity=8)
    gross = int(st[1:, cv2.CC_STAT_AREA].max()) if n > 1 else 0
    blau = msk & (L[..., 2] < -6) & (C > 8)                       # Blau-Klasse Sicht-Pruefer r1 P2
    return dict(rot=int(rot.sum()), ero=int(er.sum()), gross=gross, px=int(msk.sum()), blau=int(blau.sum()))


def struktur_render(mo, clip):
    import render as R
    out = []
    for wt, az, el in R.WINKEL:
        img, msk = R.blick(mo, R.kf(mo, clip), az, el, 380, None)
        out.append(_struktur(img, msk))
    return out


def b_riegel(vdir):
    import numpy as np
    import fl
    import p_lade as PL
    from p_messen import atlas as p_atlas
    mo = fl.lade_var(vdir)
    st = struktur_render(mo, 2)                                    # Ruhe (Clip 2) wie N1/N2 und Tafel 03
    out = {}
    ero = 100.0 * sum(x['ero'] for x in st) / max(sum(x['rot'] for x in st), 1)
    # B1: Rot als Flaeche: erosionsfester Anteil der Rotpixel (3x3), alle drei Winkel zusammen. Grenze = Familienminimum derselben Messung:
    # EM30 32,1 % (891+892+527 von 2299+2726+2168), G4 41,4 %, G5 52,4 % (--familie, je Winkel EM30 24-39, G4 34-53, G5 48-61).
    out['B1 Rot erosionsfest % (3 Winkel zus.)'] = (round(ero, 1), ero >= 32.1)
    # B2: groesste zusammenhaengende Rotflaeche / Figurpixel in den Vorderansichten (3/4 vorn, Seite), kleinerer Wert. Grenze = Familienminimum
    # derselben Messung 3,2 % (G4 Seite 1258/39249; EM30 3,59, G5 11,27; Sicht-Pruefer r1 schlug >= 1000 px absolut vor);
    # relativ, weil K5c in derselben Umkugel nur halb so viele Figurpixel deckt wie G4 (Sicht-Pruefer r1 3, Fairness). Die Rueckansicht regelt K4
    # (Bild 1 ist eine Rueckansicht mit 5,2 % Rot im Original).
    g = [100.0 * x['gross'] / x['px'] for x in st[:2]]
    out['B2 groesste Rotflaeche % Figur (3/4 vorn, Seite)'] = (tuple(round(v, 2) for v in g), min(g) >= 3.2)
    # B3: Blau-Klasse im neutralen Render je Winkel. Familie G4/G5 0,0 % (EM30 nur Jeans); Grenze 0,10 % (= N2-Toleranz, Kantenpixel).
    b = [100.0 * x['blau'] / x['px'] for x in st]
    out['B3 Blau-Klasse % je Winkel'] = (tuple(round(v, 2) for v in b), max(b) <= 0.10)
    # B4/B5 am Atlas: ehemalige Glanzkeime von K5c heute (Koerper, L* > 85, C* < 10) und Gaumen-/Zahnklasse im Kopffeld.
    mv = PL.var(vdir)
    per, _ = PL.sammeln(mv)
    rgb, mid = p_atlas(mv, per)
    L = PL.lab(rgb.reshape(-1, 3).astype(np.float32)).reshape(rgb.shape)
    m0 = PL.var(K5C_HEUTE)
    per0, _ = PL.sammeln(m0)
    rgb0, mid0 = p_atlas(m0, per0)
    L0 = PL.lab(rgb0.reshape(-1, 3).astype(np.float32)).reshape(rgb0.shape)
    koerper = (mid >= 0) & (np.arange(L.shape[1])[None, :] < 384)
    keim = koerper & (mid0 >= 0) & (L0[..., 0] > 85) & (np.hypot(L0[..., 1], L0[..., 2]) < 10)
    p90 = float(np.percentile(L[..., 0][keim], 90))
    # B4: Glanzrest = L*-p90 der ehemaligen Glanzkeime. Grenze 58 = hellstes Band im Original (Messblatt 2: helle Baender T L* 51-58): heller
    # als jede Stelle des Screenshots ist ein Rest der Rueckprojektion. Runde 1: Lab (72,9/7,9/1,5), p90 80,2 (gestaucht statt entfernt).
    out['B4 Glanzrest L* p90 ehemalige Keime'] = (round(p90, 1), p90 <= 58.0)
    kopf = (mid >= 0)
    kopf[:, :384] = False
    kopf[104:] = False                                             # Kopffeld (384,0,128,104), k_form.py:11
    z = kopf & (L[..., 0] > 60) & (np.hypot(L[..., 1], L[..., 2]) < 35)
    zk = 100.0 * z.sum() / kopf.sum()
    # B5: Zahn-/Gaumenklasse im Kopffeld (Messblatt 3: L* > 60, C* < 35). Der Kopf hat ein eigenes Zahnfeld; Grenze 2,0 % = Familie ohne Zahnreihe
    # (G5-Maul 1,8 %, Hund-Kopf 0,1 %, Messblatt 3). K5c heute 15,1 % (Gaumenstreifen), Runde 1 beiger Streifen x 500-511 (Zahlen-Pruefer 6.6).
    out['B5 Kopffeld Zahn-/Gaumenklasse %'] = (round(zk, 2), zk <= 2.0)
    return out


def familie():
    import fl
    sys.path.insert(0, HIER)
    import kf_tafeln as KT
    for nm, mo, c in (('EM30', fl.lade_em30(), 0), ('G5', KT.re2_mo(0x36), 0), ('G4', KT.re2_mo(0x34), 0)):
        st = struktur_render(mo, c)
        print(nm, [dict(x, ero_p=round(100 * x['ero'] / max(x['rot'], 1), 1), gross_p=round(100 * x['gross'] / x['px'], 2),
                        blau_p=round(100 * x['blau'] / x['px'], 2)) for x in st])


def messen(vdir):
    res = {}
    for k, (v, ok) in PR.atlas_riegel(var(vdir), os.path.basename(os.path.normpath(vdir))).items():
        res[k] = (v, bool(ok))
    for k, (v, ok) in PR.kette_riegel(vdir).items():
        res[k] = (v, bool(ok))
    for k, (v, ok) in n_riegel(vdir).items():
        res[k] = (v, bool(ok))
    for k, (v, ok) in b_riegel(vdir).items():
        res[k] = (v, bool(ok))
    return res


def drucken(name, res):
    print('== %s' % name)
    for k, (v, ok) in res.items():
        print('  %-44s %-70s %s' % (k, v, 'GRUEN' if ok else 'ROT'))
    g = sum(1 for _v, ok in res.values() if ok)
    print('  -> %d/%d GRUEN' % (g, len(res)))
    return g


if __name__ == '__main__':
    if '--familie' in sys.argv:
        familie()
        sys.exit(0)
    a = argparse.ArgumentParser()
    a.add_argument('variante')
    a.add_argument('--gegenprobe', default=None, help='K5c heute: jeder Farbriegel muss dort ROT sein (Halteriegel A10-A12 ausgenommen)')
    a.add_argument('--gegenprobe-r1', default=None, help='Runde 1 (farbe/K5c_farbe_r1): die Befund-Riegel B1-B5 muessen dort ROT sein')
    a.add_argument('--json', default=None)
    x = a.parse_args()
    r = messen(x.variante)
    g = drucken(x.variante, r)
    exitcode = 0 if g == len(r) else 1
    out = dict(variante={k: dict(wert=str(v), ok=ok) for k, (v, ok) in r.items()})
    if x.gegenprobe:
        rg = messen(x.gegenprobe)
        gg = drucken('Gegenprobe ' + x.gegenprobe, rg)
        out['gegenprobe'] = {k: dict(wert=str(v), ok=ok) for k, (v, ok) in rg.items()}
        # Ausnahmen laut Messblatt 6 (K5c heute GRUEN): A10 Texeldichte, A11 Schwanz, A12 Farbkorn = Halteriegel.
        halte = ('A10', 'A11', 'A12')
        falsch = [k for k, (_v, ok) in rg.items() if ok and not k.startswith(halte)]
        print('  Gegenprobe K5c heute: %d/%d GRUEN; Farbriegel, die dort NICHT rot sind: %s' % (gg, len(rg), falsch or 'keine'))
        out['gegenprobe_nicht_rot'] = falsch
        if falsch:
            exitcode = 1
    if x.gegenprobe_r1:
        r1 = {k: (v, bool(ok)) for k, (v, ok) in b_riegel(x.gegenprobe_r1).items()}
        print('== Gegenprobe Runde 1 (nur B-Riegel) ' + x.gegenprobe_r1)
        for k, (v, ok) in r1.items():
            print('  %-44s %-70s %s' % (k, v, 'GRUEN' if ok else 'ROT'))
        falsch1 = [k for k, (_v, ok) in r1.items() if ok]
        print('  Gegenprobe Runde 1: B-Riegel, die dort NICHT rot sind: %s' % (falsch1 or 'keine'))
        out['gegenprobe_r1'] = {k: dict(wert=str(v), ok=ok) for k, (v, ok) in r1.items()}
        out['gegenprobe_r1_nicht_rot'] = falsch1
        if falsch1:
            exitcode = 1
    print('EXIT %d' % exitcode)
    if x.json:
        json.dump(out, open(x.json, 'w', encoding='utf-8'), indent=1, ensure_ascii=False)
    sys.exit(exitcode)
