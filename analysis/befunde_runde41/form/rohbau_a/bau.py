#!/usr/bin/env python3
"""Baut FORM.EMD, FORM.TIM, form.json aus Entwurf, Korrektur (korr.json), Posen (posen.json) und Atlas (atlas.py).
python bau.py [--aus DIR]"""
import argparse, json, math, os, sys
import numpy as np
from kn import knochen, lade_posen, HIER, FORM, TEILE, INDEX, ry, E
import form_emd as FE
import entwurf, atlas, ruhe
import netz as NZ


def korr_laden(pfad=None):
    p = pfad or os.path.join(HIER, 'korr.json')
    if os.path.exists(p):
        return {k: np.array(v) for k, v in json.load(open(p, encoding='utf-8')).items()}
    return None


def normalen_je_ecke(V, F, grenze_grad):
    """Flaechennormalen nach aussen, je Ecke gemittelt ueber Flaechen innerhalb der Grenze. -> Liste je Flaeche: Normale je Ecke."""
    fn = []
    for f in F:
        p = V[list(f)]
        n = np.zeros(3)
        for i in range(1, len(f) - 1):
            n += np.cross(p[i] - p[0], p[i + 1] - p[0])      # netz.py richtet die Ringe so aus, dass dies nach aussen zeigt
        fn.append(n)
    fn = np.array(fn)
    fl = np.linalg.norm(fn, axis=1, keepdims=True)
    eh = fn / np.maximum(fl, 1e-9)
    an = {}
    for i, f in enumerate(F):
        for v in f:
            an.setdefault(v, []).append(i)
    c = math.cos(math.radians(grenze_grad))
    aus = []
    for i, f in enumerate(F):
        nn = []
        for v in f:
            s = np.zeros(3)
            for j in an[v]:
                if eh[i] @ eh[j] >= c:
                    s += fn[j]
            nn.append(s if s @ s > 0 else fn[i])
        aus.append(nn)
    return aus


def baue_form(N, ks, posen, karte):
    form = FE.neu()
    for k in ks:
        FE.knochen_dazu(form, k['name'], k['eltern'], [int(round(c)) for c in k['versatz']])
    for m in N:
        me = form['meshes'][m['knochen']]
        V = m['V']
        grenze = m['teil'].kante if m['teil'].kante is not None else 48.0
        NN = normalen_je_ecke(V, m['F'], grenze)
        v0 = len(me['ecken'])
        for p in V:
            FE.ecke(me, p)
        nmap = {}
        for f, nn, uvf in zip(m['F'], NN, karte[m['name']]):
            ni = []
            for n in nn:
                e = tuple(int(x) for x in np.round(n / max(np.linalg.norm(n), 1e-9) * 4096.0))
                if e not in nmap:
                    nmap[e] = FE.normale(me, e)
                ni.append(nmap[e])
            if len(f) == 3:
                FE.dreieck(me, [v0 + i for i in f], ni, uvf)
            else:
                # Ring a b c d -> Z-Reihenfolge a b d c: a-b und a-d werden Kanten, c liegt a gegenueber
                z = (0, 1, 3, 2)
                FE.viereck(me, [v0 + f[i] for i in z], [ni[i] for i in z], [uvf[i] for i in z])

    def b(p):
        return FE.bild(form, wurzel=[int(round(c)) for c in p['wurzel']], winkel=[[int(round(c)) for c in a] for a in p['winkel']])
    FE.clip_dazu(form, 'pose1', [b(posen['bild1'])])
    FE.clip_dazu(form, 'pose2', [b(posen['bild2'])])
    FE.clip_dazu(form, 'ruhe', [b(posen['ruhe'])])
    FE.orientiere(form)
    FE.boden_nachfuehren(form)
    return form


def main():
    a = argparse.ArgumentParser()
    a.add_argument('--aus', default=HIER)
    a.add_argument('--ohne-zeichnung', action='store_true')
    a = a.parse_args()
    ks = knochen()
    korr = korr_laden()
    teile = entwurf.alle()
    N = NZ.netze(teile, korr)
    posen = lade_posen()
    for n in ('bild1', 'bild2'):
        # Wurzelversatz x, z in die Aktorlage: Das Spiel haelt die Wurzel je Clip im Aktor, Clipwechsel springen sonst
        p = posen[n]
        Y = ry(p['gier'] * E)
        w = np.array([p['wurzel'][0], 0.0, p['wurzel'][2]])
        p['lage'] = [int(round(c)) for c in (np.array(p['lage'], float) + Y @ w)]
        p['wurzel'] = [0, p['wurzel'][1], 0]
    posen['ruhe'] = ruhe.pose(ks, posen)
    karte, streifen = atlas.karte(N)
    form = baue_form(N, ks, posen, karte)
    F, W = FE.pruefe(form)
    for x in F:
        print('FEHLER', x)
    for x in W:
        print('Warnung', x)
    px, clut = atlas.textur(N, ks, form, posen, streifen, karte, ohne_zeichnung=a.ohne_zeichnung)
    emd = FE.schreibe_emd(form)
    tim = FE.tim_schreiben(px, [clut])
    open(os.path.join(a.aus, 'FORM.EMD'), 'wb').write(emd)
    open(os.path.join(a.aus, 'FORM.TIM'), 'wb').write(tim)
    FE.speichern(form, os.path.join(a.aus, 'form_zwischen.json'))
    ix = {k['name']: i for i, k in enumerate(ks)}
    j = dict(name='A', emd='FORM.EMD', tim='FORM.TIM',
             bild1=dict(lage=[int(c) for c in posen['bild1']['lage']], gier=int(posen['bild1']['gier'])),
             bild2=dict(lage=[int(c) for c in posen['bild2']['lage']], gier=int(posen['bild2']['gier'])),
             ruhe=dict(clip=2, bild=0), teile={t: [ix[b] for b in bb] for t, bb in TEILE.items()})
    json.dump(j, open(os.path.join(a.aus, 'form.json'), 'w', encoding='utf-8'), indent=1)
    nt = sum(len(m['dreiecke']) for m in form['meshes'])
    nq = sum(len(m['vierecke']) for m in form['meshes'])
    print('Flaechen %d (Dreiecke %d, Vierecke %d), Knochen %d, Meshes %d, Textur %dx%d, EMD %d Byte' % (nt + nq, nt, nq, len(form['knochen']), len(form['meshes']), px.shape[1], px.shape[0], len(emd)))
    for c in form['clips']:
        print(c['name'], 'wurzel', c['bilder'][0]['wurzel'])


if __name__ == '__main__':
    main()
