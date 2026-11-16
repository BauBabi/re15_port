p = r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel_v8/re15_port/tools/r41_sichel/v8/riegel_r7c.py'
s = open(p, encoding='utf-8').read()


def rep(a, b):
    global s
    assert s.count(a) == 1, (s.count(a), a[:90])
    s = s.replace(a, b)


rep("""          In jeder Ansicht verboten: helle Zacke mit dunkler V-Kerbe direkt neben dem Kopf in zwei oder mehr Bildern in Folge (E9). Mass:
          Kerben-Stueck rechts neben dem Kopf ueber dem Soll, dessen Mesh-8-Wand jenseits des Stuecks (4 px) im echten Bild mittlere Helligkeit
          L >= L_HELL hat; Folge = zwei verschiedene aufeinanderfolgende Bilder (gleiche Doppelbilder zaehlen einmal).""",
    """          In jeder Ansicht verboten: helle Zacke mit dunkler V-Kerbe direkt neben dem Kopf in zwei oder mehr Bildern in Folge (E9). Mass:
          Kerben-Stueck rechts neben dem Kopf mit mehr als 21 Bildpunkten UND groesster Tiefe ueber 3,2 px (man sieht hinein: luecke_r5
          SOLL_SUMME / SOLL_MAXTIEFE), dessen Mesh-8-Wand jenseits des Stuecks (4 px) im echten Bild mittlere Helligkeit L >= L_HELL hat;
          Folge = zwei aufeinanderfolgende Framedumps.""")
rep("""        if not im_soll(m) and lw >= L_HELL:""", """        import luecke_r5 as L
        if m['px'] > L.SOLL_SUMME and m['max_tiefe'] > L.SOLL_MAXTIEFE and lw >= L_HELL:""")
rep('''def artefakt(kk, k7):
    """E13: Silhouette im Kopffenster (Kopf-Rahmen +- NAH) bildpunktgleich zu V7."""
    H = kk >= 3
    if not H.any():
        return False
    ys, xs = np.nonzero(H)
    sl = (slice(max(0, ys.min() - NAH), ys.max() + NAH + 1), slice(max(0, xs.min() - NAH), xs.max() + NAH + 1))
    return bool(((kk[sl] >= 0) == (k7[sl] >= 0)).all())''', '''def artefakt(kk, k7):
    """E13: die Silhouette um die Kerben-Bildpunkte des Prueflings (Rahmen der Kerbe + 3 px) ist bildpunktgleich zu V7 - die Kerbe zaehlt nur
    ueber die Klassen der Randpunkte (P5-Pruefer R3 zu F340: "Spaltenprofil Kopf-Oberkante / Mesh-8-Unterkante identisch")."""
    G = _kerben(kk)[4]
    if not G.any():
        return False
    ys, xs = np.nonzero(G)
    sl = (slice(max(0, ys.min() - 3), ys.max() + 4), slice(max(0, xs.min() - 3), xs.max() + 4))
    return bool(((kk[sl] >= 0) == (k7[sl] >= 0)).all())''')
rep("""    z = []
    letzt, n_folge, folgen = None, 0, []
    for f in range(F_LUECKE[0], F_LUECKE[1] + 1):
        if f not in Kk or VORDERGRUND[0] <= f <= VORDERGRUND[1]:
            continue
        kk = klein(Kk[f])
        if letzt is not None and (kk == letzt).all():
            continue                                              # Doppelbild: zaehlt einmal
        letzt = kk
        p = ppm(plain_dir, f)
        st = zacke(kk, klein(p).astype(float)) if p is not None else []
        if st:
            z.append(('F%d' % f, st))
            n_folge += 1
            if n_folge >= 2:
                folgen.append('F%d' % f)
        else:
            n_folge = 0""", """    z, folgen, vorher = [], [], False
    for f in range(F_LUECKE[0], F_LUECKE[1] + 1):
        if f not in Kk or VORDERGRUND[0] <= f <= VORDERGRUND[1]:
            vorher = False
            continue
        p = ppm(plain_dir, f)
        st = zacke(klein(Kk[f]), klein(p).astype(float)) if p is not None else []
        if st:
            z.append(('F%d' % f, st))
            if vorher:
                folgen.append('F%d' % f)
        vorher = bool(st)""")
rep("""    p7 = os.path.join(V.SFV8, 'riegel_s16_%s.EMD' % V.sha(ref_blob)[:12])
    open(p7, 'wb').write(ref_blob)
    z7 = L.zusammen(L.messen_parallel(p7, 'alle', 7, 12))""", """    z7 = luecke_offline(ref_blob)""")
rep("""def e9_offline(ueber_kand, ueber_v7, cos):""", '''def luecke_offline(blob):
    """luecke_r5.messen_parallel (177 Ansichten, 7 Varianten) mit Zwischenspeicher je sha256 unter SFV8."""
    import json
    import luecke_r5 as L
    sha = V.sha(blob)
    pj = os.path.join(V.SFV8, 'riegel_r7c_luecke_%s.json' % sha[:16])
    if os.path.exists(pj):
        return json.load(open(pj))
    pe = os.path.join(V.SFV8, 'riegel_s16_%s.EMD' % sha[:12])
    os.makedirs(V.SFV8, exist_ok=True)
    open(pe, 'wb').write(blob)
    z = L.zusammen(L.messen_parallel(pe, 'alle', 7, 12))
    json.dump(z, open(pj, 'w'))
    return z


def e9_offline(ueber_kand, ueber_v7, cos):''')
rep("""if __name__ == '__main__':
    if sys.argv[1] == 'lm':""", '''def teil(blob, ref_blob, r4, a):
    """Nur die Runde-7c-Punkte (fuer Gegenproben): die noetigen Teile von riegel_v8.alles (S3, S9, S11, S16 a und verdeckt) und einbauen."""
    import riegel_v8 as RV
    import riegel_r6 as R6
    mo, A = RV.render(blob)
    out = dict(sha256=V.sha(blob))
    _s2, out['S3'], _om = RV.s2_s3(mo, A['bone'][:480])
    out['S3']['f_huegel_rot'] = RV.huegel_rot(A)
    out['S3']['ok'] = out['S3']['ok'] and out['S3']['f_huegel_rot'] >= RV.S3F_MIN
    out['S9'] = RV.s9(blob, ref_blob)
    out['S9']['b_schlitz'] = RV.s9b(blob, ref_blob)
    out['S9']['ok'] = out['S9']['ok'] and out['S9']['b_schlitz']['ok']
    out['S11'] = RV.s11(blob)
    z = luecke_offline(blob)
    out['S16'] = dict(ueber_soll=z['ueber_soll'], ok=z['ok'], verdeckt=R6.s16_verdeckt(blob, ref_blob))
    out['S18'] = dict(ok=False)
    return einbauen(out, blob, ref_blob, r4, a)


if __name__ == '__main__':
    if sys.argv[1] == 'teil':          # python riegel_r7c.py teil <emd> <aus.txt> [--r4 R4.EMD] [--r7c-* wie riegel_v8.py]
        import argparse, json
        import riegel_r6 as R6
        ap = argparse.ArgumentParser()
        ap.add_argument('was'); ap.add_argument('emd'); ap.add_argument('aus'); ap.add_argument('--r4'); ap.add_argument('--v7')
        for k in ('lm', 'plain', 'lm-v7', 'plain-v7', 'lm-ein', 'plain-ein', 'plain-r4', 'ein'):
            ap.add_argument('--r7c-' + k)
        a = ap.parse_args()
        ref = open(a.v7 or os.path.join(V.SFV8, 'emd', 'EM30_V7.EMD'), 'rb').read()
        assert V.sha(ref) == V.V7_SHA
        blob = open(a.emd, 'rb').read()
        out = teil(blob, ref, R6.r4_blob(a.r4), a)
        z = ['Riegel Runde 7c (Teil, Gegenprobe) fuer %s' % a.emd, 'sha256 %s' % V.sha(blob)] + zeilen(out)
        st = {k: out[k].get('status', 'GRUEN' if out[k]['ok'] else 'ROT') for k in ('S3', 'S9', 'S16', 'S18', 'S21')}
        z.append('URTEILE Runde 7c: %s' % st)
        open(a.aus, 'w', encoding='utf-8').write('\\n'.join(z) + '\\n')
        json.dump(out.get('R7c'), open(a.aus.replace('.txt', '.json'), 'w'), indent=1, default=lambda o: o.tolist() if hasattr(o, 'tolist') else str(o))
        print('\\n'.join(z))
    elif sys.argv[1] == 'lm':''')
open(p, 'w', encoding='utf-8', newline='\n').write(s)
print('ok')
