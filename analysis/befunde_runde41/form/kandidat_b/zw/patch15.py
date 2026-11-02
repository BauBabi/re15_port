s = open('b_mal2.py', encoding='utf-8').read()
s = s.replace("S = np.array([[14, 6, 10], [27, 11, 18], [38, 15, 23], [48, 20, 30], [62, 30, 41], [78, 42, 56], [96, 57, 76]], float)",
              "S = np.array([[14, 7, 11], [26, 13, 19], [36, 18, 25], [46, 23, 31], [60, 33, 42], [76, 44, 56], [94, 58, 75]], float)")
# Striche: vier Stufen
a = s.index('def strich_aus_bild(rgb, sicht, Fak, innen, schwellen):')
b = s.index('def stellen(rgb, sichten, Fak1):')
neu = '''def albedo_bild1(sicht, Fak):
    xy, si = sicht['xy'], sicht['sicht']
    e = spielbild(1)
    ok = si & np.isfinite(xy[..., 0])
    x = np.clip(np.round(np.nan_to_num(xy[..., 0])).astype(int), 0, 639)
    y = np.clip(np.round(np.nan_to_num(xy[..., 1])).astype(int), 0, 479)
    A = e[y, x] / np.maximum(Fak, 0.2)
    return A, ok, x, y


def strich_aus_bild(rgb, sicht, Fak, innen, schwellen):
    """Helle Stellen aus Bild 1 in harte Palettenstufen. Albedo = Spielbild / Licht. Warm = Rotbraun/Rosa, kuehl = Linie/Band."""
    A, ok, x, y = albedo_bild1(sicht, Fak)
    drin = ok & innen[y, x]
    Ya = A @ np.array([.299, .587, .114], np.float32)
    rr = A[..., 0] / np.maximum(A[..., 1], 1)
    warm = rr > 1.18
    s0, s1, s2, s3 = schwellen
    m = drin & (Ya >= s0) & (Ya < s1)
    rgb[m & warm] = S[5]
    rgb[m & ~warm] = S[4]
    m = drin & (Ya >= s1) & (Ya < s2)
    rgb[m & warm] = ROT[0]
    rgb[m & ~warm] = LINIE[0]
    m = drin & (Ya >= s2) & (Ya < s3)
    rgb[m & warm] = ROT[2]
    rgb[m & ~warm] = LINIE[1]
    m = drin & (Ya >= s3)
    rgb[m & warm] = ROSA[0]
    rgb[m & ~warm] = BAND[1]
    return rgb


'''
s = s[:a] + neu + s[b:]
# Stellen mit Faktor aus dem letzten Bericht
s = s.replace("""        yv = Yb[s['bild']][y, x]
        lo, hi = np.percentile(yv[tr], [15, 85])
        w = np.clip((yv - lo) / max(hi - lo, 1), 0, 0.999)
        mat = s['material']
""", """        if s['id'] == 'Q4':                 # duennes Band am Rand, 2 px breiter malen, sonst verliert es sich im Filter
            k2 = cv2.dilate(k, np.ones((5, 5), np.uint8))
            tr = ok & (k2[y, x] > 0)
        yv = Yb[s['bild']][y, x]
        lo, hi = np.percentile(yv[tr], [15, 85])
        w = np.clip((yv - lo) / max(hi - lo, 1), 0, 0.999)
        w = np.clip(w + FAKTOR.get(s['id'], 0.0), 0, 0.999)
        mat = s['material']
""")
s = s.replace("def stellen(rgb, sichten, Fak1):", "FAKTOR = {}\n\n\ndef stellen(rgb, sichten, Fak1):")
# Zusatz: Ton aus dem Bild statt fest
s = s.replace("""        for i, t in d.items():
            k = np.zeros((480, 640), np.uint8)
            cv2.fillPoly(k, [np.round(np.array(Mk[i]['polygon'], float)).astype(np.int32)], 1)
            rgb[ok & (k[y, x] > 0)] = TON[t]
""", """        for i, t in d.items():
            k = np.zeros((480, 640), np.uint8)
            cv2.fillPoly(k, [np.round(np.array(Mk[i]['polygon'], float)).astype(np.int32)], 1)
            tr = ok & (k[y, x] > 0)
            if n == 1 and Fak1 is not None:
                A = spielbild(1)[y, x] / np.maximum(Fak1, 0.2)
                Ya = A @ np.array([.299, .587, .114], np.float32)
                tab = np.array([S[4], S[5], ROT[0], LINIE[0], LINIE[1], BAND[1]])
                i_ = np.clip(((Ya - 60.0) / 36.0).astype(int), 0, 5)
                rgb[tr] = tab[i_[tr]]
            else:
                rgb[tr] = TON[t]
""")
s = s.replace("def zusatz(rgb, sichten):", "def zusatz(rgb, sichten, Fak1=None):")
# Hakeninneres vor dem Rand
s = s.replace("""    v = sichten[2]
    xy, si = v['xy'], v['sicht']
    ok = si & np.isfinite(xy[..., 0])
    x = np.clip(np.round(np.nan_to_num(xy[..., 0])).astype(int), 0, 639)
    y = np.clip(np.round(np.nan_to_num(xy[..., 1])).astype(int), 0, 479)
    k = np.zeros((480, 640), np.uint8)
    cv2.polylines(k, [np.array(ZUG_Q, np.int32)], False, 1, 4)
""", """    v = sichten[2]
    xy, si = v['xy'], v['sicht']
    ok = si & np.isfinite(xy[..., 0])
    x = np.clip(np.round(np.nan_to_num(xy[..., 0])).astype(int), 0, 639)
    y = np.clip(np.round(np.nan_to_num(xy[..., 1])).astype(int), 0, 479)
    k = np.zeros((480, 640), np.uint8)
    cv2.fillPoly(k, [np.round(np.array(P.teil_b2('H')['polygon'], float)).astype(np.int32)], 1)
    k[:, :] = k * (1 - cv2.dilate(cv2.fillPoly(np.zeros((480, 640), np.uint8), [np.round(np.array(Mk['B08']['polygon'], float)).astype(np.int32), np.round(np.array(Mk['B09']['polygon'], float)).astype(np.int32)], 1), np.ones((3, 3), np.uint8)))
    rgb[ok & (k[y, x] > 0)] = ROT[1]        # FORMBLATT 2.1: Hakeninneres rotbraun
    k = np.zeros((480, 640), np.uint8)
    cv2.polylines(k, [np.array(ZUG_Q, np.int32)], False, 1, 4)
""")
s = s.replace("rgb = strich_aus_bild(rgb, sichten[1], F1, innen_maske(), (125.0, 175.0, 245.0))", "rgb = strich_aus_bild(rgb, sichten[1], F1, innen_maske(), (78.0, 118.0, 170.0, 245.0))")
s = s.replace("    rgb = zusatz(rgb, sichten)\n", "    rgb = zusatz(rgb, sichten, F1)\n")
s = s.replace("""def malen(pos, nrm, bone, belegt, sichten, netze, ctx):
    fit, J, s1 = ctx['fit'], ctx['J'], ctx['s1']
""", """def lese_faktor(pfad='zw/faktor.json'):
    FAKTOR.clear()
    if os.path.exists(pfad):
        FAKTOR.update(json.load(open(pfad)))


def faktor_nachziehen(aus, pfad='zw/faktor.json', schritt=0.22):
    \"\"\"Helligkeit je Stelle am Bericht nachziehen: Stufe hoch, wenn der gefilterte Render dunkler ist als das Video.\"\"\"
    R = json.load(open(os.path.join(aus, 'bericht.json'), encoding='utf-8'))
    for n in (1, 2):
        for z in R['bild%d' % n]['zeichnung']:
            q = z['y_video'] / max(z['y_render'], 1.0)
            if q > 1.12:
                FAKTOR[z['id']] = min(FAKTOR.get(z['id'], 0.0) + schritt, 0.9)
            elif q < 0.88:
                FAKTOR[z['id']] = max(FAKTOR.get(z['id'], 0.0) - schritt, -0.9)
    json.dump(FAKTOR, open(pfad, 'w'))


def malen(pos, nrm, bone, belegt, sichten, netze, ctx):
    fit, J, s1 = ctx['fit'], ctx['J'], ctx['s1']
    lese_faktor()
""")
open('b_mal2.py', 'w', encoding='utf-8').write(s)

s = open('b_fein.py', encoding='utf-8').read()
s = s.replace("""        for x in B['flaechen']:
            if not x['zaehlt'] or x['ok']:
                continue
            if x['id'] == 'V6':""", """        for x in B['flaechen']:
            if not x['zaehlt'] or x['ok']:
                continue
            if 'treffer' in x and n == 1:
                z = cv2.imread(FORM + '/ziel_bild1.png', 0) == 0
                m = s.maske(teile)
                hit = (cv2.dilate(m.astype(np.uint8), np.ones((3, 3), np.uint8)) > 0) & z
                k, lab, st, ce = cv2.connectedComponentsWithStats(hit.astype(np.uint8), connectivity=8)
                d_ = EDT(~z)
                inn = EDT(z)
                sg = cv2.GaussianBlur(inn.astype(np.float32), (0, 0), 2.0)
                gx = cv2.Sobel(sg, cv2.CV_32F, 1, 0, ksize=3) / 8.0
                gy = cv2.Sobel(sg, cv2.CV_32F, 0, 1, ksize=3) / 8.0
                for i in range(1, k):
                    if st[i, 4] < 4:
                        continue
                    c = ce[i]
                    xi, yi = int(c[0]), int(c[1])
                    g = np.array([gx[yi, xi], gy[yi, xi]], float)
                    g /= max(np.linalg.norm(g), 1e-3)
                    tief = float(inn[lab == i].max())
                    ziehe(teile, s, c, -g * (tief + 2.5), sigma=60.0, radius=18.0)
                    n_ += 1
                continue
            if x['id'] == 'V6':""")
open('b_fein.py', 'w', encoding='utf-8').write(s)

s = open('b_alles.py', encoding='utf-8').read()
s = s.replace("""        n = FN.runde(teile, alle_sichten(fit, teile, w2), z + '/v')
        print('Feinrunde', r_, 'offen', n, flush=True)
        if n == 0:
            break
""", """        n = FN.runde(teile, alle_sichten(fit, teile, w2), z + '/v')
        M2.faktor_nachziehen(z + '/v')
        print('Feinrunde', r_, 'offen', n, flush=True)
        if n == 0 and r_ >= 2:
            break
""")
open('b_alles.py', 'w', encoding='utf-8').write(s)
