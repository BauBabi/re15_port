"""SF V7 linke Hand (Bauer, 2026-11-08): Krallen der (W)-linken Hand (Mesh 12) - Finger rot, Daumen grau, jede Kralle auf Foto-Laenge.
Basis = EM30_V6.EMD (sha256 9e0e58ef); alles ausser den vier Krallen-Doppelpyramiden von Mesh 12 und der neuen CLUT-Zeile 492 bleibt
bytegleich V6 (491 nur um die Daumen-Indizes ergaenzt, die V6 nicht hatte).

    python hand_bau.py [--basis EMD] [--aus DIR] [--json PROT] [--name NAME]
        -> DIR/EM30_V7.EMD (+ EM30_V4.EMD darauf, wenn --name fehlt)

Nutzer 2026-11-08 (woertlich): siehe hand_gemein. Kein RE-Befund: die Sichel-Form gibt es auf keiner Disc; keine Zahl hier ist eine
@0x-Konstante. Masse = Bildmessung (hand_gemein.foto_mass, Bilder SF_belege/roh/alle f1475-f1477 und f1482-f1488) und am Modell.

1. Zuordnung (hand_gemein.krallen): Daumenkralle T (Spitze 20), Fingerkrallen A (47), B (29), C (42); je 6 Dreiecke.
2. Farbe (nur CLUT-Feld der Krallenflaechen + Zeilen):
   * T (6 Flaechen) -> Zeile 491 (V6-Grau). Daumen-Indizes, die 491 noch nicht umgefaerbt hat, bekommen dieselbe Regel wie V6
     (drei_bau.kralle_zeile, Verschiebung 25,664 aus SF_belege/v4/v6drei/drei_bau_protokoll.json); die V6-Eintraege bleiben bytegleich.
   * A, B, C (je 6 Flaechen) -> neue Zeile 492 = Kopie 490 (Klauenhand = Referenz-2-Zeile 482, rot); jeder Index der Finger-Krallentexel
     bekommt den naechsten (dE76) roten Eintrag der Referenz-2-Palette (EM30_HANGAR Zeilen 480-485, Paletten-a* >= ROT_A) zum Ziel
     (Paletten-L* + Verschiebung, a*, b* des 490-Eintrags); Verschiebung per Halbierung so, dass die Finger-Krallenpixel in P1 (Videoraum)
     im Mittel die Foto-L* der roten Streifen treffen (foto_mass finger_L).
   * Kappe v16 des Kastenfingers (in V6 auf 491, ist keine Kralle) -> 490 wie die Hand.
3. Laenge: je Kralle nur die Spitzenecke entlang der Achse Ring-Mitte -> Spitze verschoben (Ring, obere Spitze, Flaechen, UV unveraendert;
   Spitze bleibt spitz, die Doppelpyramide bleibt geschlossen). Faktor so, dass die projizierte Laenge Ring-Mitte -> Spitze in P1 (Foto 1)
   die Foto-Laenge trifft: Finger = finger_H (rote Streifen f1482-f1488), Daumen = daumen_H (grauer Keil f1475-f1477); 4 Iterationen
   s <- s * Soll / Ist. Nie kuerzer als V6.
4. Normalen: jede Normale, die eine untere Krallenflaeche nutzt (alle exklusiv in der Doppelpyramide), = Mittel der Einheits-Flaechen-
   normalen ihrer Flaechen (Wicklung negiert), Laenge 4096. Diese Regel trifft die Original-Normalen von V5k auf Median 0,7 / max 1,5 Grad
   (flaechengewichtet 7,7 / 40,6, winkelgewichtet 5,3 / 42,1; gemessen sfv7/p15.py).
"""
import os, sys, json, argparse, hashlib
import numpy as np
HIER = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HIER)
import hand_gemein as H           # noqa: E402
import kopf_orig_bau as KB        # noqa: E402
import koerper_bau as KBAU        # noqa: E402
import drei_bau as DB             # noqa: E402

BASIS = os.environ.get('RE15_V7_BASIS', os.path.join(KB.AUS, 'EM30_V6.EMD'))     # sha256 9e0e58ef (V6)
AUS = KB.AUS
V6_VERSCHIEBUNG = 25.664          # SF_belege/v4/v6drei/drei_bau_protokoll.json kralle.verschiebung
ROT_A = 10.0                      # Messblatt 4: Rot = a* >= 10
FOTO = os.path.join(H.G.BAUM, 'analysis', 'befunde_runde41', 'SF_belege', 'v4', 'v7hand', 'foto_hand.json')


def sha(b):
    return hashlib.sha256(b).hexdigest()


def foto():
    if os.path.exists(FOTO):
        return json.load(open(FOTO))
    f = H.foto_mass()
    os.makedirs(os.path.dirname(FOTO), exist_ok=True)
    json.dump(f, open(FOTO, 'w'), indent=1)
    return f


def texel_indizes(m, t, flaechen):
    mk = np.zeros(t['pix'].shape, bool)
    for k, uv in KBAU.flaechen_schluessel(m):
        if k in flaechen:
            mk |= KB.raster(uv, 3 if len(uv) == 9 else 4, (uv[5] & 0xF) * 128)
    return sorted(set(int(x) for x in t['pix'][mk])), int(mk.sum())


def rot_kandidaten():
    ref2 = open(KB.REF2, 'rb').read()
    tr = KB.tim_teile(KB.teile(ref2)[3])
    werte = sorted(set(int(v) & 0x7FFF for z in tr['clut'] for v in z if int(v) & 0x7FFF))
    lab_ = KBAU.lab15(np.array(werte))
    ok = lab_[:, 1] >= ROT_A
    return np.array(werte)[ok], lab_[ok]


def finger_zeile(z490, idx, shift, kand):
    werte, klab = kand
    neu = z490.copy()
    lab_ = KBAU.lab15(z490[idx] & 0x7FFF)
    for ix, l in zip(idx, lab_):
        ziel = np.array([l[0] + shift, l[1], l[2]])
        j = int(np.argmin(np.linalg.norm(klab - ziel, axis=1)))
        neu[ix] = (int(werte[j]) & 0x7FFF) | (int(z490[ix]) & 0x8000)
    return neu


def normalen_neu(m, kr):
    P = np.array([v[:3] for v in m['v']], float)
    fl = [('d%d' % i, f) for i, f in enumerate(m['tf'])] + [('v%d' % i, f) for i, f in enumerate(m['qf'])]
    nutz = {}
    for k, f in fl:
        for n in f[0::2]:
            nutz.setdefault(n, set()).add(k)
    unten = {k: f for k, f in fl if k in kr['unten']}
    neu = {}
    for n in sorted({n for f in unten.values() for n in f[0::2]}):
        assert nutz[n] <= set(kr['flaechen']), 'Normale %d nicht exklusiv' % n
        s = np.zeros(3)
        for k, f in fl:
            if k in kr['flaechen'] and n in f[0::2]:
                vi = f[1::2]
                c = -np.cross(P[vi[1]] - P[vi[0]], P[vi[2]] - P[vi[0]])
                s += c / np.linalg.norm(c)
        s = s / np.linalg.norm(s) * 4096
        alt = list(m['n'][n])
        m['n'][n] = [int(round(x)) for x in s] + [alt[3]]
        neu[n] = dict(alt=alt[:3], neu=m['n'][n][:3])
    return neu


def bauen(basis=BASIS):
    blob = open(basis, 'rb').read()
    d, _do, ms, tim = KB.teile(blob)
    t = KB.tim_teile(tim)
    assert t['clut'].shape[0] == 12, 'Basis hat %d CLUT-Zeilen (erwartet 12 = V6)' % t['clut'].shape[0]
    F = foto()
    prot = dict(basis=basis, basis_sha256=sha(blob), foto={k: v for k, v in F.items() if k not in ('rot_bilder', 'daumen_bilder')})
    m = ms[12]
    K = {k['name']: k for k in H.krallen(m)}
    prot['krallen'] = {n: {kk: vv for kk, vv in k.items()} for n, k in K.items()}
    finger = sorted(f for n in 'ABC' for f in K[n]['flaechen'])
    # ---- 2. Farbe
    pose = H.V.posen()['P1']
    for k, uv in KBAU.flaechen_schluessel(m):
        if k in K['T']['flaechen']:
            KBAU.setze_zeile(uv, H.Z_DAUMEN)
        elif k in finger:
            KBAU.setze_zeile(uv, H.Z_FINGER)
        elif k == 'v16':
            KBAU.setze_zeile(uv, H.Z_HAND)
    z490 = t['clut'][H.Z_HAND - 480].copy()
    z491 = t['clut'][H.Z_DAUMEN - 480].copy()
    t_idx, t_tex = texel_indizes(m, t, K['T']['flaechen'])
    fehlend = [i for i in t_idx if z491[i] == z490[i]]
    z491_neu = DB.kralle_zeile(z490, fehlend, V6_VERSCHIEBUNG, DB.kralle_kandidaten()) if fehlend else z490.copy()
    for i in fehlend:
        z491[i] = z491_neu[i]
    t['clut'][H.Z_DAUMEN - 480] = z491
    f_idx, f_tex = texel_indizes(m, t, finger)
    kand = rot_kandidaten()
    clut0 = t['clut'].copy()
    prot['farbe'] = dict(daumen_texel=t_tex, daumen_indizes=len(t_idx), daumen_491_ergaenzt=fehlend, finger_texel=f_tex,
                         finger_indizes=len(f_idx), rot_kandidaten=int(len(kand[0])))
    # ---- 3. Laenge (vor der Farbsuche, damit die Farbe auf den langen Krallen gemessen wird)
    mo_pack = lambda: KB.packen(blob, d, ms, KB.tim_packen(dict(t, clut=np.vstack([clut0, z490[None]]))))
    soll = {n: (F['daumen_H'] if n == 'T' else F['finger_H']) for n in K}
    P0 = np.array([v[:3] for v in m['v']], float)
    laenge = {}
    s = {n: 1.0 for n in K}
    Hp = None
    for it in range(4):
        mo = H.V.modell(mo_pack())
        for n, k in K.items():
            px, h, Hp = H.laenge_bild(mo, pose, k, Hp)
            laenge.setdefault(n, []).append(round(h, 4))
            s[n] = max(1.0, s[n] * soll[n] / h)
            c = P0[k['ring']].mean(0)
            m['v'][k['spitze']][:3] = [int(round(x)) for x in c + s[n] * (P0[k['spitze']] - c)]
    mo = H.V.modell(mo_pack())
    prot['laenge'] = {}
    for n, k in K.items():
        px, h, Hp = H.laenge_bild(mo, pose, k, Hp)
        c = P0[k['ring']].mean(0)
        prot['laenge'][n] = dict(soll_H=soll[n], verlauf_H=laenge[n], ist_H=round(h, 4), ist_px=round(px, 1), faktor=round(s[n], 4),
                                 spitze_alt=P0[k['spitze']].astype(int).tolist(), spitze_neu=m['v'][k['spitze']][:3],
                                 lok_alt=round(float(np.linalg.norm(P0[k['spitze']] - c)), 1),
                                 lok_neu=round(float(np.linalg.norm(np.array(m['v'][k['spitze']][:3], float) - c)), 1), H_port=int(Hp))
    # ---- 4. Normalen
    prot['normalen'] = {n: normalen_neu(m, k) for n, k in K.items()}
    # ---- 2b. Finger-Zeile 492 per Halbierung auf Foto-L*
    lo, hi = -20.0, 30.0
    verlauf = []
    for _ in range(12):
        sh = (lo + hi) / 2
        t['clut'] = np.vstack([clut0, finger_zeile(z490, f_idx, sh, kand)[None]])
        mo = H.V.modell(KB.packen(blob, d, ms, KB.tim_packen(t)))
        sel, Lm, _A = H.pixel(mo, pose, finger)
        lab_ = Lm[sel].mean(0)
        verlauf.append(dict(verschiebung=round(sh, 3), lab=[round(float(x), 2) for x in lab_], px=int(sel.sum())))
        if lab_[0] < F['finger_L']:
            lo = sh
        else:
            hi = sh
    sh = min(verlauf, key=lambda v: abs(v['lab'][0] - F['finger_L']))['verschiebung']
    z492 = finger_zeile(z490, f_idx, sh, kand)
    t['clut'] = np.vstack([clut0, z492[None]])
    prot['farbe'].update(verschiebung=sh, verlauf=verlauf,
                         pal_490_p50=[round(float(x), 1) for x in np.median(KBAU.lab15(z490[f_idx] & 0x7FFF), 0)],
                         pal_492_p50=[round(float(x), 1) for x in np.median(KBAU.lab15(z492[f_idx] & 0x7FFF), 0)])
    neu = KB.packen(blob, d, ms, KB.tim_packen(t))
    prot['tim'] = dict(clut_zeilen=int(t['clut'].shape[0]), port_texturhoehe=int(t['ih'] * t['clut'].shape[0]))
    prot['emd'] = dict(bytes=len(neu), sha256=sha(neu))
    return neu, prot


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--basis', default=BASIS); ap.add_argument('--aus', default=AUS); ap.add_argument('--json'); ap.add_argument('--name')
    a = ap.parse_args()
    neu, prot = bauen(a.basis)
    name = a.name or 'EM30_V7.EMD'
    os.makedirs(a.aus, exist_ok=True)
    open(os.path.join(a.aus, name), 'wb').write(neu)
    if not a.name:
        open(os.path.join(a.aus, 'EM30_V4.EMD'), 'wb').write(neu)
    print('geschrieben %s (%d Byte, sha256 %s)' % (os.path.join(a.aus, name), len(neu), prot['emd']['sha256']))
    print('TIM', prot['tim'])
    print('foto', prot['foto'])
    for n, v in prot['laenge'].items():
        print('laenge', n, {k: v[k] for k in ('soll_H', 'ist_H', 'ist_px', 'faktor', 'lok_alt', 'lok_neu')})
    print('farbe', {k: v for k, v in prot['farbe'].items() if k != 'verlauf'})
    if a.json:
        json.dump(prot, open(a.json, 'w'), indent=1, default=lambda o: o.tolist() if hasattr(o, 'tolist') else str(o))


if __name__ == '__main__':
    main()
