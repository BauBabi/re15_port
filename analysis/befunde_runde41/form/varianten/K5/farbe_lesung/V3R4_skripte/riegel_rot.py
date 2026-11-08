#!/usr/bin/env python3
"""Riegel RV1-RV5 "Rot wie die Vorlage" (MESSBLATT_FARBE_V3_NACHTRAG_ROT.md, Abschnitt 4) an einem Variantenordner (form.json, FORM.EMD, FORM.TIM).

Kette wie p_riegel.kette_riegel: Kandidat.rendern(n) -> form_video.video_filter (Saaten 15 / 12345 / 7) -> p_zonen.rueck -> Lab.
Messmaske und Regionen fest (rot_masken.npz, aus mess.py): Figur des Nutzers x K5c-Geometrie, ohne geclippte Pixel und ohne Leuchtstellen
(F1 Bild 1, Band + Zaehne Bild 2, je 2 px Saum). Ziele und Fenster aus ziele.json (Vorlage O, 2 sigma Block-Bootstrap + Rundlauf-Verschiebung).

    python riegel_rot.py <Variantenordner> [--rundlauf] [--json aus.json]
    --rundlauf: Gegenprobe Vorlage (Original-T x Figurlicht durch dieselbe Kette) statt einer Variante; muss alle bindenden Zeilen GRUEN haben.
Exit 1, wenn eine bindende Zeile ROT ist.

Runde 5 (Krallen wieder mit der Runde-3-Farbe; Orchestrator 2026-11-07: "sonst die Krallen-Texel aus der RV-Messregion ausnehmen und das im
Nachtrag mit Zahl belegen"): Bild 2 "Vorderbeine" wird Info; bindend ist "Vorderbeine ohne Krallen" = dieselbe Region ohne die Krallenpixel
samt 2 px Saum (feste Maske ../V3R5_skripte/krallen_bild2.npz, Kennungs-Render), fuer Vorlage UND Variante, Ziel nach demselben Verfahren
(../V3R5_skripte/ziel_ohne_krallen.json, ziel_ohne_krallen.py). Gemessen: mit Krallen Runde 5 77,32 % gegen Fenster bis 74,45; die Krallen
allein Vorlage 51,0 %, Runde 3/5 78,1 %, Runde 4 51,6 % (V3R5_skripte/kralle_rv.py)."""
import argparse, json, math, os, sys
import numpy as np, cv2

HIER = os.path.dirname(os.path.abspath(__file__))
WT = os.path.normpath(os.path.join(HIER, *(['..'] * 7)))
K5 = os.path.join(WT, 'analysis', 'befunde_runde41', 'form', 'varianten', 'K5')
for p in (os.path.join(WT, 're15_port', 'tools'), os.path.join(K5, 'farbe_lesung', 'P_skripte')):
    sys.path.insert(0, p)
import p_zonen as PZ  # noqa: E402

REGIONEN = ['Ruecken', 'Flanke', 'Brust/Bauch', 'Vorderbeine', 'Hinterbeine', 'Hals', 'Kopf', 'Schwanz']
SAATEN = (15, 12345, 7)
H_ROT = (-30.0, 70.0)          # Farbton-Fenster "rot" (MESSBLATT_FARBE_V3 4, Leser B 4)
RIEGEL = dict(rot_eng='RV1 Rot eng %', h_rot='RV2 Farbton a*-Klasse Grad', dL='RV3 dL Rot - Haut', gross_eng_p='RV4 groesste Rotflaeche eng % (Obergrenze)',
              C_rot='RV5 C* Rotklasse [Info]')
R5 = os.path.join(HIER, '..', 'V3R5_skripte')


def lab(x):
    a = np.asarray(x, np.float32)
    return cv2.cvtColor(a.reshape(1, -1, 3) / 255.0, cv2.COLOR_RGB2LAB)[0].reshape(a.shape)


def masse(Lf, reg, A0):
    L = Lf[reg]
    r = L[:, 1] >= A0
    h = np.degrees(np.arctan2(L[:, 2], L[:, 1]))
    eng = r & (h >= H_ROT[0]) & (h < H_ROT[1])
    d = dict(rot_eng=100.0 * eng.mean(), h_rot=None, C_rot=None, dL=None)
    if r.sum() >= 5:
        d['h_rot'] = math.degrees(math.atan2(L[r, 2].mean(), L[r, 1].mean()))
        d['C_rot'] = float(np.hypot(L[r, 1], L[r, 2]).mean())
        if (~r).sum():
            d['dL'] = float(L[r, 0].mean() - L[~r, 0].mean())
    em = np.zeros(reg.shape, bool); em[reg] = eng
    n, _l, st, _c = cv2.connectedComponentsWithStats(em.astype(np.uint8), connectivity=8)
    d['gross_eng_p'] = 100.0 * (int(st[1:, cv2.CC_STAT_AREA].max()) if n > 1 else 0) / len(L)
    return d


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('variante', nargs='?')
    ap.add_argument('--rundlauf', action='store_true')
    ap.add_argument('--json')
    a = ap.parse_args()
    M = np.load(os.path.join(HIER, 'rot_masken.npz'))
    Z = json.load(open(os.path.join(HIER, 'ziele.json'), encoding='utf-8'))
    E = json.load(open(os.path.join(HIER, 'mess.json'), encoding='utf-8'))
    k = None if a.rundlauf else PZ.P.Kandidat(os.path.join(a.variante, 'form.json'))
    # Runde 5: Bild 2 Vorderbeine -> Info, neu bindend "Vorderbeine ohne Krallen"
    zr5 = json.load(open(os.path.join(R5, 'ziel_ohne_krallen.json'), encoding='utf-8'))
    Z = dict(Z)
    Z['bild2|Vorderbeine|T'] = {m: dict(x, bindend=False) for m, x in Z['bild2|Vorderbeine|T'].items()}
    Z['bild2|Vorderbeine ohne Krallen|T'] = zr5['ziel']
    kz = np.load(os.path.join(R5, 'krallen_bild2.npz'))
    kralle2 = np.unpackbits(kz['kralle'])[:480 * 640].reshape(480, 640).astype(bool)
    res, rot = {}, 0
    print('== Riegel Rot wie die Vorlage:', 'Rundlauf der Vorlage' if a.rundlauf else a.variante)
    for n in (1, 2):
        basis = np.unpackbits(M['basis%d' % n])[:480 * 640].reshape(480, 640).astype(bool)
        reg = M['reg%d' % n].astype(int)
        A0 = E['bilder'][str(n)]['A0']['T']
        if a.rundlauf:
            To, _ = PZ.rueck(PZ.P.original(n), n)
            sauber = np.clip(To * PZ.LICHT[n], 0, 255).astype(np.uint8)
        else:
            sauber, alb, m, _k = k.rendern(n)
            basis = basis & m & (alb.max(2) > 0)
        Ls = [lab(PZ.rueck(np.asarray(PZ.FV.video_filter(sauber, bild=n, entzerren=False, rauschen=True, saat=s), np.uint8), n)[0]) for s in SAATEN]
        for rn in REGIONEN + ['Vorderbeine ohne Krallen', 'Figur']:
            z = Z.get('bild%d|%s|T' % (n, rn))
            if not z:
                continue
            if rn == 'Vorderbeine ohne Krallen':
                rm = basis & (reg == REGIONEN.index('Vorderbeine')) & ~kralle2
            else:
                rm = basis & ((reg == REGIONEN.index(rn)) if rn != 'Figur' else (reg >= 0))
            ms = [masse(L, rm, A0) for L in Ls]
            for m, name in RIEGEL.items():
                x = z.get(m)
                if not x or not x.get('ziel'):
                    continue
                v = [q[m] for q in ms if q[m] is not None]
                if not v:
                    continue
                w = float(np.mean(v)); lo, hi = x['ziel']
                ok = (w <= hi) if m == 'gross_eng_p' else (lo <= w <= hi)
                bind = x['bindend']
                rot += int(bind and not ok)
                res['Bild %d %s %s' % (n, rn, name)] = dict(wert=round(w, 2), fenster=[lo, hi], ok=bool(ok), bindend=bool(bind))
                print('  Bild %d %-24s %-44s %7.2f  [%7.2f .. %7.2f]  %s%s' % (n, rn, name, w, lo, hi, 'GRUEN' if ok else 'ROT', '' if bind else ' (Info)'))
    t = sum(1 for v in res.values() if v['bindend'])
    print('  -> %d/%d bindende Zeilen GRUEN' % (t - rot, t))
    if a.json:
        json.dump(res, open(a.json, 'w', encoding='utf-8'), indent=1, ensure_ascii=False)
    sys.exit(1 if rot else 0)


if __name__ == '__main__':
    main()
