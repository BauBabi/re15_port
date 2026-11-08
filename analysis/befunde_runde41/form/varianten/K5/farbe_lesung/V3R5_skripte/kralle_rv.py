"""Runde 5: Anteil der Krallenpixel an der RV-Messregion Bild 2 / Bild 1 und RV1/RV2 getrennt nach Kralle / Rest,
fuer Vorlage (Rundlauf-Kette), Runde 3, Runde 4, Runde 5. Krallenmaske per Kennungs-Render: Kopie der Variante, Krallenflaechen auf
eine zusaetzliche CLUT-Zeile mit Markerfarbe (0,255,0); Albedo == Marker -> Kralle.
    python kralle_rv.py <aus.json>
"""
import json, os, sys, shutil, tempfile
import numpy as np
K5 = os.path.normpath(os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', '..'))
RR = os.path.join(K5, 'farbe_lesung', 'V3R4_skripte')
sys.path.insert(0, RR)
sys.path.insert(0, os.path.join(K5, 'generator'))
import riegel_rot as R  # noqa
import kf_g4 as KG  # noqa
FE = KG.FE
PZ = R.PZ
KLAUEN = {14, 15, 28, 30, 19, 20, 29, 31}
MARKER = (0, 255, 0)


def kennung(vdir, tmp):
    os.makedirs(tmp, exist_ok=True)
    emd = open(os.path.join(vdir, 'FORM.EMD'), 'rb').read()
    tim = open(os.path.join(vdir, 'FORM.TIM'), 'rb').read()
    t = FE.tim_lesen(tim)
    cl = [list(z) for z in t['cluts']]
    zm = len(cl)
    cl.append([0] + [KG.wort(MARKER)] * 255)
    form = FE.lese_emd(emd)
    for mi in KLAUEN:
        for teil in ('dreiecke', 'vierecke'):
            for f in form['meshes'][mi][teil]:
                f['clut'] = ((t['clut_y'] + zm) << 6) | 0
    open(os.path.join(tmp, 'FORM.EMD'), 'wb').write(FE.schreibe_emd(form, pruefen=True))
    open(os.path.join(tmp, 'FORM.TIM'), 'wb').write(FE.tim_schreiben(t['pixel'], cl, clut_x=t['clut_x'], clut_y=t['clut_y']))
    fj = json.load(open(os.path.join(vdir, 'form.json'), encoding='utf-8'))
    json.dump(fj, open(os.path.join(tmp, 'form.json'), 'w', encoding='utf-8'))
    k = PZ.P.Kandidat(os.path.join(tmp, 'form.json'))
    out = {}
    for n in (1, 2):
        _s, alb, m, _k = k.rendern(n)
        out[n] = (alb[..., 0] == 0) & (alb[..., 1] >= 240) & (alb[..., 2] == 0) & m
    return out


def messe(Ls, rm, A0):
    ms = [R.masse(L, rm, A0) for L in Ls]
    o = {}
    for m in ('rot_eng', 'h_rot', 'dL', 'gross_eng_p'):
        v = [q[m] for q in ms if q[m] is not None]
        o[m] = round(float(np.mean(v)), 2) if v else None
    o['px'] = int(rm.sum())
    return o


def main():
    M = np.load(os.path.join(RR, 'rot_masken.npz'))
    E = json.load(open(os.path.join(RR, 'mess.json'), encoding='utf-8'))
    tmp = tempfile.mkdtemp()
    kr = kennung(os.path.join(K5, 'farbe', 'K5c_farbe_v3'), os.path.join(tmp, 'id'))
    staende = [('Vorlage', None), ('Runde 3', 'K5c_farbe_v3r3'), ('Runde 4', 'K5c_farbe_v3r4'), ('Runde 5', 'K5c_farbe_v3')]
    res = {}
    for n in (1, 2):
        basis0 = np.unpackbits(M['basis%d' % n])[:480 * 640].reshape(480, 640).astype(bool)
        reg = M['reg%d' % n].astype(int)
        A0 = E['bilder'][str(n)]['A0']['T']
        for nm, d in staende:
            if d is None:
                To, _ = PZ.rueck(PZ.P.original(n), n)
                sauber = np.clip(To * PZ.LICHT[n], 0, 255).astype(np.uint8)
                basis = basis0
            else:
                k = PZ.P.Kandidat(os.path.join(K5, 'farbe', d, 'form.json'))
                sauber, alb, m, _k = k.rendern(n)
                basis = basis0 & m & (alb.max(2) > 0)
            Ls = [R.lab(PZ.rueck(np.asarray(PZ.FV.video_filter(sauber, bild=n, entzerren=False, rauschen=True, saat=s), np.uint8), n)[0])
                  for s in R.SAATEN]
            for rn in ('Vorderbeine', 'Figur'):
                rm = basis & ((reg == R.REGIONEN.index(rn)) if rn != 'Figur' else (reg >= 0))
                res['Bild %d|%s|%s|alle' % (n, rn, nm)] = messe(Ls, rm, A0)
                res['Bild %d|%s|%s|Kralle' % (n, rn, nm)] = messe(Ls, rm & kr[n], A0)
                res['Bild %d|%s|%s|ohne Kralle' % (n, rn, nm)] = messe(Ls, rm & ~kr[n], A0)
    json.dump(res, open(sys.argv[1], 'w', encoding='utf-8'), indent=1, ensure_ascii=False)
    for k_, v in res.items():
        print('%-48s %s' % (k_, v))
    shutil.rmtree(tmp, ignore_errors=True)


if __name__ == '__main__':
    main()
