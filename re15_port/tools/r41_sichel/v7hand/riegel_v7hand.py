"""SF V7 linke Hand - Riegel. Exit 1 bei Rot.

    python riegel_v7hand.py --emd EM30_V7.EMD [--basis EM30_V6.EMD] [--aus DIR --name NAME]

Kein RE-Befund (die Sichel-Form gibt es auf keiner Disc). Foto-Werte = hand_gemein.foto_mass (SF_belege/v4/v7hand/foto_hand.json).
Pflicht (Auftrag Runde 41 SF "V7 linke Hand", Nutzer 2026-11-08):
  T-V7a  Krallenzahl: Mesh 12 hat genau 4 Krallen-Doppelpyramiden (Daumen T + Finger A, B, C; hand_gemein.krallen), jede geschlossen
         (jede Kante genau 2 Flaechen). Foto: in jedem der Bilder f1482-f1488 mindestens ein roter Streifen und in f1475-f1477 ein grauer
         Keil (die Zahl der Fingerkrallen loest das Video nicht auf: je Bild ein Streifen).
  T-V7b  Daumen grau: Pixel der Daumenkralle in P1 (Videoraum, RDT-Licht) Mittel-a* <= 6 (Foto-Keil a* 4,0).
  T-V7c  Finger rot: Pixel der Fingerkrallen A/B/C in P1 Mittel-a* >= Foto-a* der roten Streifen - 5.
  T-V7d  Laenge je Kralle: projizierte Laenge Ring-Mitte -> Spitze in P1 / H_port = Foto +- 15 % (Finger: rote Streifen; Daumen: Keil).
  T-V7e  Rest bytegleich zur Basis V6: Sektionen 0-6 und TIM-Texel bytegleich; CLUT 480-490 bytegleich, 491 = V6 bis auf Daumen-Indizes,
         die V6 nicht umgefaerbt hatte (dort Eintraege der V6-Regel), 492 = 490 bis auf Finger-Krallen-Indizes (dort nur rote
         Referenz-2-Eintraege a* >= 10); Meshes ausser 12 bytegleich; Mesh 12: Ecken bis auf die 4 Krallenspitzen gleich, Normalen bis auf
         die der unteren Krallenflaechen gleich, Flaechen-Indizes gleich, UV gleich bis auf das CLUT-Feld (Daumen -> 491, Finger -> 492,
         v16 -> 490).
  T-V7f  CLUT-Zeilen <= 16, Port-Texturhoehe <= 4096.
  T-V7g  Boden: in keinem der 912 Keyframes liegt eine Krallenspitze unter der tiefsten Koerperecke (Meshes 0-15 ohne Kopf 9, Unterarm 11,
         Hand 12).
INFO:  T-V7h Koerper: Keyframes, in denen ein Punkt der Krallenachse (11 Punkte Ring-Mitte -> Spitze) im Mesh eines anderen Knochens
         liegt (Strahltest, 3 Richtungen, Mehrheit; vorab Huellen-Test), Basis gegen neu. Alle Werte auch in P2 und 135.
"""
import os, sys, json, argparse
import numpy as np
from scipy.spatial import Delaunay
HIER = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HIER)
import hand_gemein as H           # noqa: E402
import hand_bau as HB             # noqa: E402
import kopf_orig_bau as KB        # noqa: E402
import koerper_bau as KBAU        # noqa: E402
import drei_bau as DB             # noqa: E402

G, V = H.G, H.V
ROT_TOL, LAENGE_TOL, DAUMEN_A = 5.0, 0.15, 6.0


def geschlossen(m, flaechen):
    kanten = {}
    for k, f in [('d%d' % i, f) for i, f in enumerate(m['tf'])]:
        if k in flaechen:
            v = f[1::2]
            for a, b in ((v[0], v[1]), (v[1], v[2]), (v[2], v[0])):
                kanten[tuple(sorted((a, b)))] = kanten.get(tuple(sorted((a, b))), 0) + 1
    return all(n == 2 for n in kanten.values())


def _tris(m):
    out = [f[1::2] for f in m.tf]
    for f in m.qf:
        v = f[1::2]; out += [[v[0], v[1], v[3]], [v[0], v[3], v[2]]]
    return np.array(out)


def _innen(p, Q, T):
    hits = 0
    A, B, C = Q[T[:, 0]], Q[T[:, 1]], Q[T[:, 2]]
    e1, e2 = B - A, C - A
    for d in (np.array([1, 0.013, 0.007]), np.array([0.011, 1, 0.017]), np.array([0.019, 0.003, 1])):
        h = np.cross(d, e2); a = (e1 * h).sum(1); ok = np.abs(a) > 1e-9
        f = np.where(ok, 1 / np.where(ok, a, 1), 0); s = p - A; u = f * (s * h).sum(1); q = np.cross(s, e1)
        v = f * (q @ d); t = f * (e2 * q).sum(1)
        hits += int(((u >= 0) & (v >= 0) & (u + v <= 1) & (t > 0) & ok).sum()) % 2
    return hits >= 2


def keyframes(blob):
    mo = V.modell(blob)
    _d, _, ms, _t = KB.teile(blob)
    K = H.krallen(ms[12])
    erst = {}
    for c, (_f, k) in enumerate(mo.clips):
        for b in range(k):
            erst.setdefault(int(H.BB.kf_von_clip(mo, c, b)), (c, b))
    P12 = np.array([v[:3] for v in mo.meshes[12].tv], float)
    roh = {bb: (np.array([v[:3] for v in mo.meshes[bb].tv], float), _tris(mo.meshes[bb])) for bb in range(len(mo.meshes))}
    unter, mx, koerper = 0, -1e9, {}
    for kf, (c, b) in erst.items():
        R, T = mo.pose(kf)
        w = lambda bb, P: P @ (R[bb].astype(float) / 4096).T + T[bb].astype(float)
        sohle = max(w(bb, roh[bb][0])[:, 1].max() for bb in range(16) if bb not in (9, 11, 12))
        tips = w(12, P12[[k['spitze'] for k in K]])
        d = tips[:, 1] - sohle
        mx = max(mx, float(d.max())); unter += int((d > 0).any())
        achse = {k['name']: w(12, np.array([P12[k['ring']].mean(0) + (P12[k['spitze']] - P12[k['ring']].mean(0)) * t
                                            for t in np.linspace(0, 1, 11)])) for k in K}
        for bb in range(len(mo.meshes)):
            if bb in (11, 12) or len(roh[bb][0]) < 5:
                continue
            Q = w(bb, roh[bb][0])
            try:
                hull = Delaunay(Q)
            except Exception:
                continue
            for n, pts in achse.items():
                kand = [i for i, p in enumerate(pts) if hull.find_simplex(p) >= 0]
                tief = [i for i in kand if _innen(pts[i], Q, roh[bb][1])]
                if tief:
                    koerper.setdefault(n, []).append(dict(clip=c, bild=b, knochen=bb, punkte=len(tief), ab=min(tief) / 10))
    kfs = {(e['clip'], e['bild']) for v in koerper.values() for e in v}
    return dict(keyframes=len(erst), unter_sohle=unter, max_ueber_sohle=round(mx), koerper_keyframes=len(kfs),
                koerper_je_kralle={n: len(v) for n, v in koerper.items()},
                koerper_clips=sorted({e['clip'] for v in koerper.values() for e in v}),
                koerper_knochen=sorted({e['knochen'] for v in koerper.values() for e in v}),
                koerper_tiefste=max((1 - e['ab'] for v in koerper.values() for e in v), default=0.0), liste=koerper)


def pixelwerte(mo, K, pose):
    fd = K['T']['flaechen']; ff = sorted(f for n in 'ABC' for f in K[n]['flaechen'])
    out = {}
    for name, fl in (('daumen', fd), ('finger', ff)):
        sel, Lm, _A = H.pixel(mo, pose, fl)
        out[name] = dict(px=int(sel.sum()), lab=[round(float(x), 2) for x in Lm[sel].mean(0)] if sel.any() else None)
    return out


def ausnahme(basis, neu):
    """T-V7e: Liste der Abweichungen (leer = gruen)."""
    fehler = []
    d0, _, m0, t0b = KB.teile(basis); d1, _, m1, t1b = KB.teile(neu)
    for i in range(7):
        if basis[d0[i]:d0[i + 1]] != neu[d1[i]:d1[i + 1]]:
            fehler.append('Sektion %d' % i)
    t0, t1 = KB.tim_teile(t0b), KB.tim_teile(t1b)
    if not np.array_equal(t0['pix'], t1['pix']):
        fehler.append('Texel')
    if not np.array_equal(t0['clut'][:11], t1['clut'][:11]):
        fehler.append('CLUT 480-490')
    K = {k['name']: k for k in H.krallen(m1[12])}
    td, _ = HB.texel_indizes(m1[12], t1, K['T']['flaechen'])
    z490 = t0['clut'][10]
    ergaenzt = [i for i in td if t0['clut'][11][i] == z490[i]]
    soll491 = t0['clut'][11].copy()
    if ergaenzt:
        r = DB.kralle_zeile(z490, ergaenzt, HB.V6_VERSCHIEBUNG, DB.kralle_kandidaten())
        soll491[ergaenzt] = r[ergaenzt]
    if not np.array_equal(t1['clut'][11], soll491):
        fehler.append('CLUT 491')
    ff = sorted(f for n in 'ABC' for f in K[n]['flaechen'])
    fi, _ = HB.texel_indizes(m1[12], t1, ff)
    if t1['clut'].shape[0] < 13:
        fehler.append('Zeile 492 fehlt')
        z492 = np.zeros(0, np.uint16)
    else:
        z492 = t1['clut'][12]
    rotw = set(int(v) for v in HB.rot_kandidaten()[0])
    for i in range(len(z492)):
        if i in fi:
            if (int(z492[i]) & 0x7FFF) not in rotw or (int(z492[i]) & 0x8000) != (int(z490[i]) & 0x8000):
                fehler.append('492[%d] kein roter Ref2-Eintrag' % i)
        elif z492[i] != z490[i]:
            fehler.append('492[%d] != 490' % i)
    for i in range(len(m0)):
        if i == 12:
            continue
        if m0[i] != m1[i]:
            fehler.append('Mesh %d' % i)
    a, b = m0[12], m1[12]
    spitzen = {k['spitze'] for k in K.values()}
    for j, (x, y) in enumerate(zip(a['v'], b['v'])):
        if x != y and j not in spitzen:
            fehler.append('Mesh 12 Ecke %d' % j)
    fl_neu = [('d%d' % i, f) for i, f in enumerate(b['tf'])] + [('v%d' % i, f) for i, f in enumerate(b['qf'])]
    unten_n = {n for k in K.values() for key, f in fl_neu if key in k['unten'] for n in f[0::2]}
    for j, (x, y) in enumerate(zip(a['n'], b['n'])):
        if x != y and j not in unten_n:
            fehler.append('Mesh 12 Normale %d' % j)
    if a['tf'] != b['tf'] or a['qf'] != b['qf'] or len(a['v']) != len(b['v']) or len(a['n']) != len(b['n']):
        fehler.append('Mesh 12 Flaechen/Zahl')
    ziel = {}
    for k in K['T']['flaechen']:
        ziel[k] = H.Z_DAUMEN
    for k in ff:
        ziel[k] = H.Z_FINGER
    ziel['v16'] = H.Z_HAND
    for (k, u0), (_k, u1) in zip(KBAU.flaechen_schluessel(a), KBAU.flaechen_schluessel(b)):
        z = ziel.get(k)
        uu0 = list(u0)
        if z is not None:
            KBAU.setze_zeile(uu0, z)
        if uu0 != list(u1):
            fehler.append('Mesh 12 UV %s' % k)
    return fehler, dict(daumen_491_ergaenzt=ergaenzt, finger_indizes=len(fi))


def pruefen(emd, basis):
    neu, alt = open(emd, 'rb').read(), open(basis, 'rb').read()
    F = HB.foto()
    mo = V.modell(neu)
    _d, _, ms, tim = KB.teile(neu)
    K = {k['name']: k for k in H.krallen(ms[12])}
    P = V.posen()
    erg = []

    def zeile(name, ok, text, info=False):
        erg.append(dict(pruefung=name, ergebnis='INFO' if info else ('gruen' if ok else 'ROT'), text=text))
    # a
    ok_a = len(K) == 4 and set(K) == {'T', 'A', 'B', 'C'} and all(geschlossen(ms[12], k['flaechen']) for k in K.values())
    foto_ok = all(n >= 1 for n in F['rot_streifen_je_bild']) and all(F['daumen_bilder'][str(n) if str(n) in F['daumen_bilder'] else n]
                                                                      for n in H.DAUMEN_BILDER)
    zeile('T-V7a Krallenzahl', ok_a and foto_ok, 'Modell %d Doppelpyramiden %s, geschlossen %s; Foto rote Streifen je Bild %s, Keil in %d/%d Bildern'
          % (len(K), sorted(K), all(geschlossen(ms[12], k['flaechen']) for k in K.values()), F['rot_streifen_je_bild'],
             sum(1 for v in F['daumen_bilder'].values() if v), len(F['daumen_bilder'])))
    # b, c
    pw = {pn: pixelwerte(mo, K, P[pn]) for pn in ('P1', 'P2', '135')}
    d1, f1 = pw['P1']['daumen'], pw['P1']['finger']
    zeile('T-V7b Daumen grau (P1)', d1['lab'] is not None and d1['lab'][1] <= DAUMEN_A,
          'a* %s (<= %.1f), Lab %s, %d px; Foto-Keil %s' % (d1['lab'][1] if d1['lab'] else None, DAUMEN_A, d1['lab'], d1['px'], F['daumen_lab']))
    zeile('T-V7c Finger rot (P1)', f1['lab'] is not None and f1['lab'][1] >= F['finger_a'] - ROT_TOL,
          'a* %s (>= %.2f = Foto %.2f - 5), Lab %s, %d px; Foto-Streifen L*/a*/b* %.2f / %.2f / %.2f'
          % (f1['lab'][1] if f1['lab'] else None, F['finger_a'] - ROT_TOL, F['finger_a'], f1['lab'], f1['px'], F['finger_L'], F['finger_a'], F['finger_b']))
    zeile('T-V7bc P2 / 135', True, 'P2 Daumen %s Finger %s; 135 Daumen %s Finger %s' % (pw['P2']['daumen'], pw['P2']['finger'],
                                                                                       pw['135']['daumen'], pw['135']['finger']), info=True)
    # d
    Hp = None; texte = []; ok_d = True
    for n in 'TABC':
        px, h, Hp = H.laenge_bild(mo, P['P1'], K[n], Hp)
        soll = F['daumen_H'] if n == 'T' else F['finger_H']
        ok = abs(h / soll - 1) <= LAENGE_TOL
        ok_d &= ok
        texte.append('%s %.4f H (%.1f px) / Soll %.4f = %+.1f %%' % (n, h, px, soll, 100 * (h / soll - 1)))
    zeile('T-V7d Laenge je Kralle (P1)', ok_d, '; '.join(texte))
    # e, f
    fehler, info_e = ausnahme(alt, neu)
    zeile('T-V7e Rest bytegleich V6', not fehler, ('Abweichungen: %s' % fehler[:12]) if fehler else
          'nur Krallen von Mesh 12; 491 ergaenzt um Indizes %s, 492 mit %d Finger-Indizes' % (info_e['daumen_491_ergaenzt'], info_e['finger_indizes']))
    t = KB.tim_teile(tim)
    zeile('T-V7f Zeilen', t['clut'].shape[0] <= 16 and t['ih'] * t['clut'].shape[0] <= 4096,
          '%d Zeilen, Port-Texturhoehe %d' % (t['clut'].shape[0], t['ih'] * t['clut'].shape[0]))
    # g, h
    kn, ka = keyframes(neu), keyframes(alt)
    zeile('T-V7g Boden (912 Keyframes)', kn['unter_sohle'] == 0,
          '%d von %d Keyframes mit Krallenspitze unter der Sohle; naechste Spitze %d E. ueber der Sohle (Basis %d / %d)'
          % (kn['unter_sohle'], kn['keyframes'], -kn['max_ueber_sohle'], ka['unter_sohle'], -ka['max_ueber_sohle']))
    zeile('T-V7h Koerper (912 Keyframes)', True,
          'neu %d Keyframes (je Kralle %s; Clips %s, Knochen %s; tiefster Anteil der Achse %.1f), Basis %d (%s; Clips %s, Knochen %s; %.1f)'
          % (kn['koerper_keyframes'], kn['koerper_je_kralle'], kn['koerper_clips'], kn['koerper_knochen'], kn['koerper_tiefste'],
             ka['koerper_keyframes'], ka['koerper_je_kralle'], ka['koerper_clips'], ka['koerper_knochen'], ka['koerper_tiefste']), info=True)
    return erg, dict(pixel=pw, keyframes_neu={k: v for k, v in kn.items() if k != 'liste'}, keyframes_basis={k: v for k, v in ka.items() if k != 'liste'},
                     koerper_liste_neu=kn['liste'], koerper_liste_basis=ka['liste'], foto={k: v for k, v in F.items() if k not in ('rot_bilder',)})


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--emd', required=True); ap.add_argument('--basis', default=HB.BASIS)
    ap.add_argument('--aus'); ap.add_argument('--name', default='V7')
    a = ap.parse_args()
    erg, daten = pruefen(a.emd, a.basis)
    rot = [e for e in erg if e['ergebnis'] == 'ROT']
    pflicht = [e for e in erg if e['ergebnis'] != 'INFO']
    zeilen = ['SF V7 linke Hand - Riegel %s (%s gegen Basis %s)' % (a.name, os.path.basename(a.emd), os.path.basename(a.basis))]
    zeilen += ['%-32s %-6s %s' % (e['pruefung'], e['ergebnis'], e['text']) for e in erg]
    zeilen.append('ERGEBNIS: %d rot von %d Pflichtpruefungen -> Exit %d' % (len(rot), len(pflicht), 1 if rot else 0))
    txt = '\n'.join(zeilen)
    print(txt)
    if a.aus:
        os.makedirs(a.aus, exist_ok=True)
        open(os.path.join(a.aus, 'riegel_v7hand_%s.txt' % a.name), 'w', encoding='utf-8').write(txt + '\n')
        json.dump(dict(ergebnisse=erg, daten=daten), open(os.path.join(a.aus, 'riegel_v7hand_%s.json' % a.name), 'w'), indent=1,
                  default=lambda o: o.tolist() if hasattr(o, 'tolist') else str(o))
    sys.exit(1 if rot else 0)


if __name__ == '__main__':
    main()
