"""SF V6 drei Punkte (Haar, Schwanzspitze, Kralle) - Riegel. Exit 1 bei Rot.

    python riegel_v6drei.py --emd EM30_V6.EMD [--basis EM30_V5f.EMD] [--aus DIR --name NAME] [--n-spalten 50]

Kein RE-Befund (die Sichel-Form gibt es auf keiner Disc). Posen v6_gemein.posen: P1 = Foto 1, P2 = Foto 2, 135 = Kopf von vorn.
Pflicht (Auftrag Runde 41 SF "V6 drei Punkte", Nutzer 2026-11-08):
  T-K1   Ausnahmeliste gegen die Basis V5f: (a) Sektionen 0, 1, 3-6 bytegleich, EMR (2) bytegleich bis auf die y-Felder von relpos 20/21/22
         (Keyframes, Kinderlisten, Bindversatz 16 bytegleich); (b) Texel nur im Haarfeld (drei_bau.haar_maske auf der Basis, Kopf-exklusiv),
         neue Indizes nur aus dem grauen Fleck der Original-Textur; (c) Meshes 0-15 bytegleich bis auf das CLUT-Feld der Krallenflaechen von
         Mesh 12 (490 -> 491); Meshes 16-22: alte Flaechen/UV/Normalen als Anfang der Listen bytegleich, dazu nur Deckel (11 Dreiecke je
         Deckel, Zeile 487) und je Deckel eine Ecke/Normale; (d) CLUT 480-490 = Basis, 491 = 490 bis auf die Krallen-Indizes, deren Werte
         aus der Referenz-2-/Original-Palette (grau/knochenfarben, drei_bau.kralle_kandidaten); 12 Zeilen <= 16, Port-Texturhoehe <= 4096.
  T-H7   Haar grau: Kopfpixel (Knochen 9, Videoraum, RDT-Licht) in P1/P2/135 ohne die roten Original-Merkmale (Texel mit Paletten-a* >= 15:
         rotes (W)-linkes Auge, aufgerissene Wange, Lippen - V6 unveraendert): b* P90 <= 6 und C* P90 <= 10; dazu jedes Haartexel mit
         Paletten-b* <= 10.
  T-F9   Schwanz in P2 (nur Schwanz gerendert, Spielkamera 50A0/12): Spitze (Apex Mesh 22) <= 8 px vom Nutzerpunkt Ende (295,343);
         Verlauf: Nutzerpunkte Beginn (357,243) und Knick (290,308) <= 10 px vom Schwanz-Umriss (0 = auf dem Schwanz; die Nutzerpunkte
         liegen am Rand: Foto-Maskenmitte f 0 = (354,224), Messblatt 5), Biegung (295,252) ebenso. Ersetzt V4 T1-T4/TA (Pose 85).
  T-F5d  Steigungsknick der Mantellinie <= 15 Grad (keine Huckel).
  T-F9s  Spalten: Stichprobe 50 Keyframes x 4 Ansichten, sichtbare Innenwand nach Oeffnung 3 x 3 = 0 px (spalten.py).
  T-F10  Kralle (W)-links in P1 hell: Krallenpixel Mittel-L* = Foto 30,3 +- 6 (Messblatt 6); Sichel-Unterarm (Mesh 14) und alles ausser
         Kopf, Schwanz und Krallenflaechen unveraendert: in P1 und P2 jedes Pixel, das in Basis und neu derselbe Knochen ist, bytegleich.
  aus riegel_v5f (Stand V5f gruen): T-F1, T-F5a/b/c/e, T-F8, T-F4, Kopf T-H1i/ii, T-H2, T-H4, T-H5, T-H6.
INFO: riegel_v5f T-K1 (gegen V5k), T-F7 (in V5f rot, offen beim Nutzer; hier gegen V5f verglichen), V4 Pose 85 (T1-T5, TA, TK: abgeloest
durch T-F9 in P2).
"""
import os, sys, json, argparse, hashlib, struct
import numpy as np
HIER = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HIER)
import drei_gemein as DG          # noqa: E402
import drei_bau as DB             # noqa: E402
import spalten as SP              # noqa: E402
import kopf_orig_bau as KB        # noqa: E402
import koerper_bau as KBAU        # noqa: E402
import texel_erweitern as TE      # noqa: E402
import schwanz_form as SF         # noqa: E402

G, V, b2 = DG.G, DG.V, DG.b2
V5K = os.path.join(KB.AUS, 'EM30_V5k.EMD')


def tf9(mo):
    P2 = V.posen()['P2']
    _img, ids = G.zeichne(mo, P2, licht=False, knochen=set(DG.SCHWANZ))
    m = ids >= 0
    ap = DG.apex_bild(mo, P2)
    e = float(np.hypot(*(ap - np.array(DG.NUTZER2['Ende']))))
    d = {k: DG.punkt_zu_maske(DG.NUTZER2[k], m) for k in ('Beginn', 'Biegung', 'Knick')}
    kp = b2.projiziere(P2, DG.kette_punkte(mo, DG.BB.kf_von_clip(mo, P2['clip'], P2['bild'])))
    ys, xs = np.nonzero(m)
    return dict(spitze=[round(float(x), 1) for x in ap], spitze_abstand=round(e, 1), abstand=d, kette=np.round(kp, 1).tolist(),
                tiefster=[int(xs[np.argmax(ys)]), int(ys.max())])


def boden(mo):
    """Je verschiedenem Keyframe (912): tiefster Schwanzpunkt (Modell-y, + = unten) gegen die tiefste Koerperecke (Meshes 0-15 ohne Kopf =
    Sohle). Rueckgabe Zahl der Keyframes mit Schwanz unter der Sohle, groesster Ueberstand, je Clip (Zahl, Laenge)."""
    erst = {}
    for c, (_f, k) in enumerate(mo.clips):
        for b in range(k):
            erst.setdefault(int(DG.BB.kf_von_clip(mo, c, b)), (c, b))
    unter, mx, je = 0, -1e9, {}
    for kf, (c, b) in erst.items():
        R, T = mo.pose(kf)
        def ymax(bs):
            m = -1e9
            for bb in bs:
                P = np.array([v[:3] for v in mo.meshes[bb].tv], float)
                m = max(m, float((P @ (R[bb].astype(float) / 4096).T + T[bb].astype(float))[:, 1].max()))
            return m
        d = ymax(DG.SCHWANZ) - ymax([x for x in range(16) if x != 9])
        mx = max(mx, d)
        if d > 0:
            unter += 1; je[c] = je.get(c, 0) + 1
    return dict(keyframes=len(erst), unter_sohle=unter, max_ueberstand=round(mx), je_clip=je)


def th7(blob):
    mo = V.modell(blob)
    _d, _, ms, tim = KB.teile(blob)
    t = KB.tim_teile(tim)
    lab486 = TE.pal_lab(t['clut'][DB.Z_KOPF - 480])
    out = {}
    for pn in ('P1', 'P2', '135'):
        img, ids = G.zeichne(mo, V.posen()[pn])
        L = G.lab(G.videoraum(img)); k = ids == 9
        U, Vv, _Z = V.uv_karte(mo, V.posen()[pn])
        rot = lab486[t['pix'][Vv[k], U[k]]][:, 1] >= 15
        X = L[k]; C = np.hypot(X[:, 1], X[:, 2])
        out[pn] = dict(px=int(k.sum()), rot_merkmale=int(rot.sum()), b_p90=round(float(np.percentile(X[~rot, 2], 90)), 2),
                       c_p90=round(float(np.percentile(C[~rot], 90)), 2), b_p90_alle=round(float(np.percentile(X[:, 2], 90)), 2),
                       c_p90_alle=round(float(np.percentile(C, 90)), 2), l_p50=round(float(np.median(X[:, 0])), 1))
    return out


def pruefen(emd, basis, n_spalten=50):
    blob = open(emd, 'rb').read(); bas = open(basis, 'rb').read()
    d, _, ms, tim = KB.teile(blob); db, _, mb, timb = KB.teile(bas)
    t, tb = KB.tim_teile(tim), KB.tim_teile(timb)
    R = dict(emd=emd, emd_sha256=hashlib.sha256(blob).hexdigest(), basis=basis, basis_sha256=hashlib.sha256(bas).hexdigest(), zeilen=[])

    def zeile_(name, ok, soll, ist, info=False):
        R['zeilen'].append(dict(name=name, ok=bool(ok), soll=soll, ist=ist, info=info))

    # ------------------------------------------------------------------ T-K1a
    sek = [i for i in (0, 1, 3, 4, 5, 6) if blob[d[i]:d[i + 1]] != bas[db[i]:db[i + 1]]]
    e_n, e_b = bytearray(blob[d[2]:d[3]]), bytearray(bas[db[2]:db[3]])
    erlaubt = set()
    for kn in (20, 21, 22):
        erlaubt.update((8 + kn * 6 + 2, 8 + kn * 6 + 3))
    emr_diff = [i for i in range(max(len(e_n), len(e_b))) if (e_n[i:i + 1] != e_b[i:i + 1])]
    emr_fremd = [i for i in emr_diff if i not in erlaubt] + (['Laenge'] if len(e_n) != len(e_b) else [])
    rel = {kn: (struct.unpack_from('<3h', e_b, 8 + kn * 6), struct.unpack_from('<3h', e_n, 8 + kn * 6)) for kn in (20, 21, 22)}
    zeile_('T-K1a Sektionen 0/1/3-6 bytegleich, EMR nur relpos-y 20/21/22', not sek and not emr_fremd,
           'Sektionen bytegleich; EMR-Abweichung nur in den y-Feldern von relpos 20/21/22',
           'Sektionen abweichend %s; EMR fremde Bytes %s; relpos %s' % (sek, emr_fremd[:8], {k: '%s -> %s' % v for k, v in rel.items()}))
    # ------------------------------------------------------------------ T-K1b
    maske = DB.haar_maske(tb['pix'], TE.pal_lab(tb['clut'][DB.Z_KOPF - 480]), DB.kopf_frei(mb), tb['pix'] != DB.original_pix())
    po = DB.original_pix(); lab0 = TE.pal_lab(tb['clut'][DB.Z_KOPF - 480])
    x0, y0, x1, y1 = TE.PARAM['fleck_box']
    fb = np.zeros(po.shape, bool); fb[y0:y1 + 1, x0:x1 + 1] = True
    fleck_idx = set(int(i) for i in po[fb & (lab0[po][..., 0] <= TE.PARAM['fleck_l']) & (lab0[po][..., 2] <= TE.PARAM['haar_b']) & DB.kopf_frei(mb)])
    dif = t['pix'] != tb['pix']
    ausser = int((dif & ~maske).sum())
    fremd_idx = sorted(set(int(i) for i in t['pix'][dif]) - fleck_idx)
    zeile_('T-K1b Texel nur im Haarfeld, Indizes aus dem Fleck', ausser == 0 and not fremd_idx and dif.any(),
           'Abweichung nur im Haarfeld (Kopf-exklusiv), neue Indizes aus dem grauen Fleck der Original-Textur',
           '%d Texel geaendert, ausserhalb des Haarfelds %d, fremde Indizes %s (Haarfeld %d Texel)' % (int(dif.sum()), ausser, fremd_idx[:8], int(maske.sum())))
    # ------------------------------------------------------------------ T-K1c
    fehler = []
    kl = set(DG.krallen_flaechen(mb[12]))
    for i in range(16):
        a, b_ = ms[i], mb[i]
        if a['v'] != b_['v'] or a['n'] != b_['n'] or a['tf'] != b_['tf'] or a['qf'] != b_['qf']:
            fehler.append('%d: Geometrie' % i); continue
        for (k, ua), (_k, ub) in zip(KBAU.flaechen_schluessel(a), KBAU.flaechen_schluessel(b_)):
            if ua == ub:
                continue
            nur_clut = [x for j, x in enumerate(ua) if j != 2] == [x for j, x in enumerate(ub) if j != 2] and (ua[2] & 0x3F) == (ub[2] & 0x3F)
            if not (i == 12 and k in kl and nur_clut and ub[2] >> 6 == DB.Z_HAND and ua[2] >> 6 == DB.Z_KRALLE):
                fehler.append('%d/%s' % (i, k))
    kl_neu = sum(1 for k, uv in KBAU.flaechen_schluessel(ms[12]) if uv[2] >> 6 == DB.Z_KRALLE)
    deckel = 0
    for i in DG.SCHWANZ:
        a, b_ = ms[i], mb[i]
        nt, nv = len(b_['tf']), len(b_['v'])
        if a['tf'][:nt] != b_['tf'] or a['tuv'][:nt] != b_['tuv'] or a['qf'] != b_['qf'] or a['quv'] != b_['quv'] or a['n'][:nv] != b_['n']:
            fehler.append('%d: alte Flaechen/UV/Normalen' % i); continue
        neu_f = a['tf'][nt:]
        if len(neu_f) % SF.S_ or len(a['v']) - nv != len(neu_f) // SF.S_ or len(a['n']) - nv != len(neu_f) // SF.S_:
            fehler.append('%d: Deckel-Zahl' % i)
        if any(uv[2] >> 6 != KBAU.Z_SCHWANZ for uv in a['tuv'][nt:]):
            fehler.append('%d: Deckel-Zeile' % i)
        deckel += len(neu_f) // SF.S_
    zeile_('T-K1c Meshes: nur Krallen-CLUT-Feld (Mesh 12) und Schwanz (Ecken + Deckel)', not fehler and kl_neu == len(kl),
           'Meshes 0-15 bytegleich bis auf CLUT-Feld der Krallenflaechen 490 -> 491; 16-22 alte Flaechen/UV/Normalen gleich, nur Deckel dazu',
           'Fehler %s; Krallenflaechen auf 491: %d von %d; Deckel %d' % (fehler[:6], kl_neu, len(kl), deckel))
    # ------------------------------------------------------------------ T-K1d
    c, cb = t['clut'], tb['clut']
    nz = c.shape[0]
    mk = np.zeros(t['pix'].shape, bool)
    for k, uv in KBAU.flaechen_schluessel(mb[12]):
        if k in kl:
            mk |= KB.raster(uv, 3 if len(uv) == 9 else 4, (uv[5] & 0xF) * 128)
    kidx = set(int(x) for x in t['pix'][mk])
    kand = set(int(v) for v in DB.kralle_kandidaten()[0])
    ok_z = nz == 12 and t['ih'] * nz <= 4096 and np.array_equal(c[:11], cb)
    z491 = c[11] if nz >= 12 else None
    rest_gleich = z491 is not None and all(z491[i] == cb[10][i] for i in range(256) if i not in kidx)
    kand_ok = z491 is not None and all((int(z491[i]) & 0x7FFF) in kand for i in kidx)
    zeile_('T-K1d CLUT: 480-490 = Basis, 491 = 490 bis auf Krallen-Indizes (Referenz-2-/Original-Palette)', ok_z and rest_gleich and kand_ok,
           '12 Zeilen (<= 16), Hoehe <= 4096; 480-490 bytegleich; 491 sonst = 490; Krallen-Indizes aus grau/knochenfarbenen Ref2-/Original-Eintraegen',
           '%d Zeilen, Hoehe %d; 480-490 gleich %s; 491 Rest gleich %s; %d Krallen-Indizes aus Kandidaten %s' % (
               nz, t['ih'] * nz, bool(np.array_equal(c[:11], cb)) if nz >= 11 else False, rest_gleich, len(kidx), kand_ok))
    # ------------------------------------------------------------------ T-H7
    h = th7(blob); hb = th7(bas)
    haar_b = TE.pal_lab(c[DB.Z_KOPF - 480])[t['pix'][maske]][:, 2]
    ok7 = all(v['b_p90'] <= 6 and v['c_p90'] <= 10 for v in h.values()) and float(haar_b.max()) <= 10
    zeile_('T-H7 Haar grau (P1/P2/135)', ok7, 'ohne rote Original-Merkmale: b* P90 <= 6, C* P90 <= 10; Haartexel Paletten-b* <= 10',
           '; '.join('%s b* %.1f C* %.1f (alle %.1f/%.1f, %d px, L* %.1f)' % (k, v['b_p90'], v['c_p90'], v['b_p90_alle'], v['c_p90_alle'], v['px'], v['l_p50'])
                     for k, v in h.items()) + '; Haartexel b* max %.1f' % float(haar_b.max()))
    zeile_('INFO T-H7 Basis', True, '-', '; '.join('%s b* %.1f C* %.1f' % (k, v['b_p90'], v['c_p90']) for k, v in hb.items()), info=True)
    R['th7'] = dict(neu=h, basis=hb)
    # ------------------------------------------------------------------ T-F9
    mo = V.modell(blob)
    f9 = tf9(mo); f9b = tf9(V.modell(bas))
    ok9 = f9['spitze_abstand'] <= 8 and all(v <= 10 for v in f9['abstand'].values())
    zeile_('T-F9 Schwanz P2: Spitze am Nutzerpunkt, Verlauf Beginn/Biegung/Knick', ok9,
           'Spitze <= 8 px von (295,343); Beginn/Biegung/Knick <= 10 px vom Schwanz-Umriss',
           'Spitze %s, %.1f px; Beginn %.1f, Biegung %.1f, Knick %.1f px; tiefster Schwanzpixel %s; Kette %s' % (
               f9['spitze'], f9['spitze_abstand'], f9['abstand']['Beginn'], f9['abstand']['Biegung'], f9['abstand']['Knick'], f9['tiefster'],
               ' '.join('(%.0f,%.0f)' % tuple(p) for p in f9['kette'])))
    zeile_('INFO T-F9 Basis', True, '-', 'Spitze %s, %.1f px; Beginn %.1f, Biegung %.1f, Knick %.1f px' % (
        f9b['spitze'], f9b['spitze_abstand'], f9b['abstand']['Beginn'], f9b['abstand']['Biegung'], f9b['abstand']['Knick']), info=True)
    R['tf9'] = dict(neu=f9, basis=f9b)
    bo, bob = boden(mo), boden(V.modell(bas))
    R['boden'] = dict(neu=bo, basis=bob)
    zeile_('INFO T-F9b Schwanz unter der Sohle (alle Keyframes)', True, 'Beobachtung, kein Auftragsmass',
           'neu: %d von %d Keyframes, max %d E. unter der tiefsten Koerperecke, je Clip %s; Basis: %d, max %d E.' % (
               bo['unter_sohle'], bo['keyframes'], bo['max_ueberstand'], bo['je_clip'], bob['unter_sohle'], bob['max_ueberstand']), info=True)
    kn = SF.profil_knicke_geo(ms)
    zeile_('T-F5d Steigungsknick der Mantellinie <= 15 Grad', max(k['knick'] for k in kn) <= 15.0, '<= 15 Grad je Gelenk (Knochenraum)',
           '; '.join('%s %.1f' % (k['zwischen'], k['knick']) for k in kn))
    sp = SP.pruefen(blob, n_spalten)
    R['spalten'] = sp
    zeile_('T-F9s Spalten (Stichprobe Keyframes x 4 Ansichten)', sp['luecke_px'] == 0 and sp['stichprobe'] >= min(n_spalten, 50),
           'sichtbare Innenwand nach Oeffnung 3 x 3 = 0 px', '%d Keyframes (von %d) x %d Ansichten: %d px in %d Bildern (roh %d px Saum)' % (
               sp['stichprobe'], sp['keyframes_gesamt'], sp['ansichten'], sp['luecke_px'], sp['bilder_mit_luecke'], sp.get('roh_px', 0)))
    # ------------------------------------------------------------------ T-F10
    lab_k, npx, _sel, _L, A = DB.kralle_pixel(blob)
    lab_kb, _n, _s, _L2, Ab = DB.kralle_pixel(bas)
    ok10 = abs(lab_k[0] - DB.KRALLE_FOTO[0]) <= 6
    zeile_('T-F10a Kralle (W)-links hell (P1)', ok10, 'Krallenpixel L* = Foto 30,3 +- 6 (Messblatt 6, Lab 30,3/4,9/1,7)',
           'neu %s (%d px); Basis %s' % ('/'.join('%.1f' % x for x in lab_k), npx, '/'.join('%.1f' % x for x in lab_kb)))
    import f_rast as FR
    gleich, info10 = True, []
    kk = set(DG.krallen_flaechen(ms[12]))
    for pn in ('P1', 'P2'):
        An = A if pn == 'P1' else FR.mit_bg(mo, V.posen()['P2'])
        Ab_ = Ab if pn == 'P1' else FR.mit_bg(V.modell(bas), V.posen()['P2'])
        bn, bb = An['bone'][:480], Ab_['bone'][:480]
        krall = (bn == 12) & np.vectorize(lambda s: s in kk)(An['flaeche'][:480])
        vgl = (bn == bb) & (bn >= 0) & ~np.isin(bn, (9,) + DG.SCHWANZ) & ~krall
        dn = (An['img'][:480] != Ab_['img'][:480]).any(-1) & vgl
        s14 = vgl & (bn == 14)
        gleich &= not dn.any()
        info10.append('%s: verglichen %d px (Mesh 14: %d), abweichend %d (Mesh 14: %d)' % (pn, int(vgl.sum()), int(s14.sum()), int(dn.sum()), int((dn & s14).sum())))
    zeile_('T-F10b Sichel-Unterarm und Koerper unveraendert (P1/P2)', gleich, 'jedes Pixel gleichen Knochens ausser Kopf/Schwanz/Krallen bytegleich zur Basis',
           '; '.join(info10))
    # ------------------------------------------------------------------ riegel_v5f
    import riegel_v5f as RV5
    R5 = RV5.pruefen(emd, V5K, mit_kopf=True)
    for z in R5['zeilen']:
        nm = z['name']
        if nm.startswith('T-K1') or nm.startswith('INFO') or z['info']:
            zeile_('INFO v5f ' + nm, True, z['soll'], z['ist'] + ('' if z['ok'] else ' [gegen V5k, hier durch T-K1 ersetzt]' if nm.startswith('T-K1') else ''), info=True)
        elif nm.startswith('T-F7'):
            zeile_('INFO v5f ' + nm, True, z['soll'], z['ist'] + (' [ROT wie in V5f, offen beim Nutzer]' if not z['ok'] else ''), info=True)
        elif nm.startswith('V4 '):
            zeile_('INFO v5f ' + nm, True, z['soll'], z['ist'] + (' [Pose 85 abgeloest durch T-F9 in P2]' if not z['ok'] else ''), info=True)
        else:
            zeile_('v5f ' + nm, z['ok'], z['soll'], z['ist'])
    R['rot'] = [z['name'] for z in R['zeilen'] if not z['ok'] and not z['info']]
    return R


def bericht(R):
    z = ['Riegel SF V6 drei Punkte (riegel_v6drei.py)', 'EMD %s sha256 %s' % (R['emd'], R['emd_sha256']),
         'Basis %s sha256 %s' % (R['basis'], R['basis_sha256']), '']
    for e in R['zeilen']:
        z.append('%-5s %s | Soll: %s | Ist: %s' % ('INFO' if e['info'] else ('OK' if e['ok'] else 'ROT'), e['name'], e['soll'], e['ist']))
    z.append('')
    z.append('Ergebnis: %d rot von %d (%s)' % (len(R['rot']), sum(1 for e in R['zeilen'] if not e['info']), ', '.join(R['rot']) or '-'))
    return '\n'.join(z)


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--emd', required=True); ap.add_argument('--basis', default=DB.BASIS)
    ap.add_argument('--aus'); ap.add_argument('--name', default='riegel_v6drei'); ap.add_argument('--n-spalten', type=int, default=50)
    a = ap.parse_args()
    R = pruefen(a.emd, a.basis, a.n_spalten)
    txt = bericht(R)
    print(txt)
    if a.aus:
        os.makedirs(a.aus, exist_ok=True)
        open(os.path.join(a.aus, a.name + '.txt'), 'w', encoding='utf-8').write(txt + '\n')
        json.dump(R, open(os.path.join(a.aus, a.name + '.json'), 'w'), indent=1, default=lambda o: o.tolist() if hasattr(o, 'tolist') else str(o))
    sys.exit(1 if R['rot'] else 0)


if __name__ == '__main__':
    main()
