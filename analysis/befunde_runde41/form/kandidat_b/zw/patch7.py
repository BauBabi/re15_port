s = open('b_teile.py', encoding='utf-8').read()
s = s.replace("n('nl_s', (560, -1560, -760), 480)", "n('nl_s', (134, -1720, -520), 430)")
s = s.replace("n('nr_s', (560, -1560, 520), 460)", "n('nr_s', (-100, -1720, 454), 400)")
open('b_teile.py', 'w', encoding='utf-8').write(s)

s = open('b_fit.py', encoding='utf-8').read()
s = s.replace("        l_kup = (dp[i_d] ** 2).sum(1).mean() / (170.0 ** 2) + ((mod.logr[i_d] - torch.log(mod.rad0[i_d])) ** 2).mean() / (0.12 ** 2)\n",
              "        l_kup = (dp[i_d] ** 2).sum(1).mean() / (170.0 ** 2) + ((mod.logr[i_d] - torch.log(mod.rad0[i_d])) ** 2).mean() / (0.12 ** 2)\n"
              "        l_schulter = 0\n"
              "        for k_, gr_ in (('nr_s', 1130.0), ('nl_s', 1260.0)):\n"
              "            i_ = r_i[k_]\n"
              "            l_schulter = l_schulter + torch.relu(torch.norm(p[i_] - c0) + r[i_] - gr_) ** 2 / (60.0 ** 2)\n")
s = s.replace("    i_d = [i for k, i in mod.ni.items() if k.startswith('d')]\n", "    i_d = [i for k, i in mod.ni.items() if k.startswith('d')]\n    c0 = ablage_torch(T.KUPPEL_MITTE)\n")
s = s.replace("+ 0.03 * l_kup", "+ 0.03 * l_kup + 0.05 * l_schulter")
open('b_fit.py', 'w', encoding='utf-8').write(s)

s = open('b_blender.py', encoding='utf-8').read()
s = s.replace("        m.ratio = min(1.0, 1.7 * ziel / max(len(o.data.polygons), 1))\n", "        dreiecke = sum(len(p.vertices) - 2 for p in o.data.polygons)\n        m.ratio = min(1.0, 1.55 * ziel / max(dreiecke, 1))\n")
s = s.replace("for saat in (7, 3, 11, 19, 23, 31, 42, 57):", "for saat in (7, 3, 11, 19, 23, 31, 42, 57, 64, 71, 83, 97):")
s = s.replace("ok = 'FINISHED' in r and n > ziel * 0.55 and n < ziel * 1.3", "ok = 'FINISHED' in r and n > ziel * 0.6 and n < ziel * 1.25")
open('b_blender.py', 'w', encoding='utf-8').write(s)

s = open('b_alles.py', encoding='utf-8').read()
a = s.index('def snap_lauf(fit, quad, schritte):')
b = s.index('def durchgang(')
neu = '''def snap_lauf(fit, quad, schritte):
    import b_lage2 as LG
    J = np.asarray(fit['J'], float)
    w2 = np.asarray(fit['w2'], float).copy()
    w2[[T.IDX['rumpf_m'], T.IDX['rumpf'], T.IDX['maul_l'], T.IDX['maul_r']]] = 0
    n = len(T.KNOCHEN)
    teile = SN.stuecke(quad)
    R1, T1 = BB.fk(np.zeros((n, 3)), J, T.ELTERN, 0.0)
    R2, T2 = BB.fk(w2, J, T.ELTERN, fit['dy2'])
    ziele = SN.baue_ziele()
    lage2, gier2 = np.array(fit['lage2'], float), float(fit['gier2'])

    def sichten():
        d1 = boden_dy(teile, R1, T1, J)
        d2 = boden_dy(teile, R2, T2, J)
        T1b = [t + [0, d1, 0] for t in T1]
        T2b = [t + [0, d2, 0] for t in T2]
        return SN.Sicht(1, K1, O1, G1, R1, T1b, J), SN.Sicht(2, K2, lage2, gier2, R2, T2b, J), T2b
    for it in range(schritte):
        s1, s2, _ = sichten()
        b = SN.schritt([s1], teile, ziele, rate=0.6)
        if it % 4 == 0 or it == schritte - 1:
            print('Snap B1 %2d: %d Ecken, Mittel %.1f, max %.1f px' % (it, *b[1]), flush=True)
    s1, s2, T2b = sichten()
    lage2, gier2 = LG.suche(teile, R2, T2b, J, lage2, gier2)
    kuppel = [st for st in teile if st['name'] == 'kuppel']
    for it in range(6):
        s1, s2, _ = sichten()
        b = SN.schritt([s2], kuppel, ziele, rate=0.5, sigma=170.0)
        s1, s2, _ = sichten()
        c = SN.schritt([s1], teile, ziele, rate=0.45)
        print('Snap B2 %2d: %d Ecken, Mittel %.1f, max %.1f px | B1 %d, %.1f, %.1f' % (it, *b.get(2, (0, 0, 0)), *c[1]), flush=True)
    s1, s2, T2b = sichten()
    lage2, gier2 = LG.suche(teile, R2, T2b, J, lage2, gier2, schritte=(30, 15), g_schritte=(6, 3))
    return {st['name']: st['V'] for st in teile}, w2, lage2, gier2


'''
s = s[:a] + neu + s[b:]
s = s.replace("""        V, w2 = snap_lauf(fit, z + '/quad', schritte)
        pickle.dump((V, w2), open(z + '/snap.pkl', 'wb'))
    V, w2 = pickle.load(open(z + '/snap.pkl', 'rb'))
    fit['w2'] = w2
""", """        pickle.dump(snap_lauf(fit, z + '/quad', schritte), open(z + '/snap.pkl', 'wb'))
    V, w2, fit['lage2'], fit['gier2'] = pickle.load(open(z + '/snap.pkl', 'rb'))
    fit['w2'] = w2
""")
a = s.index("    # Lage Bild 2 am fertigen Netz")
b = s.index("    torch.save(fit, z + '/fit.pt')")
s = s[:a] + s[b:]
open('b_alles.py', 'w', encoding='utf-8').write(s)
