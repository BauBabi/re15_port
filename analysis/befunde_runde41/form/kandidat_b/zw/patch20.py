s = open('b_fein.py', encoding='utf-8').read()
s = s.replace("""            if x['id'] == 'V6':""", """            if 'treffer' in x and n == 1:
                z = cv2.imread(FORM + '/ziel_bild1.png', 0) == 0
                inn = EDT(z)
                sg = cv2.GaussianBlur(inn.astype(np.float32), (0, 0), 1.5)
                gx = cv2.Sobel(sg, cv2.CV_32F, 1, 0, ksize=3) / 8.0
                gy = cv2.Sobel(sg, cv2.CV_32F, 0, 1, ksize=3) / 8.0
                zone = P.poly_maske(P.zone_b1(x['id']))
                for px, x0, y0, x1, y1 in B['schwarz_treffer']:
                    if not zone[y0:y1 + 1, x0:x1 + 1].any() or px > 60:
                        continue
                    c = np.array([(x0 + x1) / 2.0, (y0 + y1) / 2.0])
                    xi, yi = int(c[0]), int(c[1])
                    g = np.array([gx[yi, xi], gy[yi, xi]], float)
                    g /= max(np.linalg.norm(g), 1e-3)
                    tief = min(float(inn[y0:y1 + 1, x0:x1 + 1].max()), 3.0)
                    ziehe(teile, s, c, g * -1.0 * (tief + 1.5) if False else -g * (tief + 1.5), sigma=45.0, radius=10.0)
                    n_ += 1
                continue
            if x['id'] == 'V6':""")
open('b_fein.py', 'w', encoding='utf-8').write(s)
s = open('b_alles.py', encoding='utf-8').read()
s = s.replace("""    teile = SN.stuecke(z + '/quad')
    for st in teile:
        st['V'] = np.array(V[st['name']], float)
    form = None
""", """    teile = SN.stuecke(z + '/quad')
    if ab == 'fein' and os.path.exists(z + '/snap_fein.pkl'):
        V, w2, fit['lage2'], fit['gier2'] = pickle.load(open(z + '/snap_fein.pkl', 'rb'))
    for st in teile:
        st['V'] = np.array(V[st['name']], float)
    form = None
""")
s = s.replace("""    pickle.dump((V, w2, fit['lage2'], fit['gier2']), open(z + '/snap_fein.pkl', 'wb'))
    return form
""", """        pickle.dump((V, w2, fit['lage2'], fit['gier2']), open(z + '/snap_fein.pkl', 'wb'))
    return form
""")
s = s.replace("""        form = L.lauf(z + '/fit.pt', z + '/quad', z + '/v', malen=malen if r_ == runden - 1 or True else None, snap=V, leise=r_ < runden - 1, ruhe_w=ruhe_w)
        n = FN.runde(teile, alle_sichten(fit, teile, w2), z + '/v')
        M2.faktor_nachziehen(z + '/v')
        print('Feinrunde', r_, 'offen', n, flush=True)
        if n == 0 and r_ >= 2:
            break
""", """        form = L.lauf(z + '/fit.pt', z + '/quad', z + '/v', malen=malen if r_ == runden - 1 or True else None, snap=V, leise=r_ < runden - 1, ruhe_w=ruhe_w)
        pickle.dump((V, w2, fit['lage2'], fit['gier2']), open(z + '/snap_fein.pkl', 'wb'))
        n = FN.runde(teile, alle_sichten(fit, teile, w2), z + '/v')
        M2.faktor_nachziehen(z + '/v')
        print('Feinrunde', r_, 'offen', n, flush=True)
        if n == 0 and r_ >= 1:
            break
""")
open('b_alles.py', 'w', encoding='utf-8').write(s)
