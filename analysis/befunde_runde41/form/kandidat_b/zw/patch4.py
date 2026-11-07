s = open('b_bau.py', encoding='utf-8').read()
s = s.replace('def lade_netze(ordner, kuppel_teilen=None):', 'def lade_netze(ordner, kuppel_teilen=None, snap=None):')
s = s.replace("""        v, vt, f = lese_obj(p)
        n, um = vertexnormalen(v, f)
        mi = np.array([v[[a for a, _ in e]].mean(0) for e in f])""", """        v, vt, f = lese_obj(p)
        if snap and 'kuppel' in snap:
            v = snap['kuppel']
        n, um = vertexnormalen(v, f)
        mi = np.array([v[[a for a, _ in e]].mean(0) for e in f])""")
s = s.replace("""        v, vt, f = lese_obj(p)
        n, um = vertexnormalen(v, f)
        netze.append(dict(name=nm, bone=i, v=v, vt=vt, f=f, n=n, umdrehen=um))""", """        v, vt, f = lese_obj(p)
        k = 'k%02d' % i
        if snap and k in snap:
            v = snap[k]
        n, um = vertexnormalen(v, f)
        netze.append(dict(name=nm, bone=i, v=v, vt=vt, f=f, n=n, umdrehen=um))""")
open('b_bau.py', 'w', encoding='utf-8').write(s)
s = open('b_lauf.py', encoding='utf-8').read()
s = s.replace("def lauf(fit_pfad, quad, aus, malen=None, pruefen=True, rand=2, dichte=None, ruhe_w=None, theta=THETA, lage1=None, gier1=None):",
              "def lauf(fit_pfad, quad, aus, malen=None, pruefen=True, rand=2, dichte=None, ruhe_w=None, theta=None, lage1=None, gier1=None, snap=None):")
s = s.replace("""    netze = BB.lade_netze(quad, teiler(c, theta))
""", """    if theta is None:
        d = (fit['naht']['P'][2:9] - c).mean(0) if 'naht' in fit else np.array([math.cos(math.radians(THETA)), 0, math.sin(math.radians(THETA))])
        theta = math.degrees(math.atan2(d[2], d[0]))
    print('Nahtazimut %.1f Grad' % theta)
    netze = BB.lade_netze(quad, teiler(c, theta), snap)
""")
open('b_lauf.py', 'w', encoding='utf-8').write(s)
s = open('b_snap.py', encoding='utf-8').read()
s = s.replace("""            e = s_b[y, x] - 1.5
            return -(-e) * gxb[y, x] * -1, -(-e) * gyb[y, x] * -1""", """            e = -s_b[y, x] + 1.5
            return e * gxb[y, x], e * gyb[y, x]""")
open('b_snap.py', 'w', encoding='utf-8').write(s)
