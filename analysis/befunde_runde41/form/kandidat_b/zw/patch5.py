s = open('b_snap.py', encoding='utf-8').read()
a = s.index('def schritt(sichten, teile, ziele')
b = s.index('def baue_ziele(')
neu = '''def schritt(sichten, teile, ziele, rate=0.6, sigma=110.0, max_px=9.0, deckel=170.0):
    """Weiche Verschiebung: Umrissecken geben eine Bildebenen-Verschiebung vor, das ganze Teil folgt per Gauss-Gewicht ueber den 3D-Abstand. Kein Dorn, Deckel je Ecke."""
    bericht = {}
    for s in sichten:
        m = s.maske(teile)
        rand = m & ~cv2.erode(m.astype(np.uint8), np.ones((3, 3), np.uint8)).astype(bool)
        dr = EDT(~rand)
        Z = ziele[s.n]
        fehl = []
        for ti, st in enumerate(teile):
            kon, w = s.konturecken(st)
            q = s.proj(w)
            ix = np.clip(np.round(q[:, 0]).astype(int), 1, 638)
            iy = np.clip(np.round(q[:, 1]).astype(int), 1, 478)
            tiefe = FK.tiefe(s.kam, w)
            idx = []
            d = []
            for i in np.nonzero(kon & (dr[iy, ix] <= 1.5))[0]:
                dx, dy = Z(ix[i], iy[i])
                if dx is None:
                    continue
                idx.append(i)
                d.append((dx, dy))
                fehl.append(math.hypot(dx, dy))
            if not idx:
                continue
            idx = np.array(idx)
            d = np.clip(np.array(d, float), -max_px, max_px)
            r, u = s.kam['R'][0], s.kam['R'][1]
            dw = (d[:, 0, None] * r[None] * (tiefe[idx] / s.f[0])[:, None]) + (d[:, 1, None] * u[None] * (tiefe[idx] / s.f[1])[:, None])
            b = st['bone']
            dl = (dw @ s.Yg) @ s.R[b]
            P0 = st['V'][idx]
            d2 = ((st['V'][:, None, :] - P0[None]) ** 2).sum(-1)
            G = np.exp(-d2 / (2 * sigma ** 2))
            nen = G.sum(1)
            feld = (G @ dl) / np.maximum(nen, 1e-6)[:, None]
            feld *= np.minimum(nen, 1.0)[:, None]               # weit weg vom Umriss bleibt alles stehen
            st['V'] += rate * feld
            weg = st['V'] - st['V0']
            lg = np.linalg.norm(weg, axis=1, keepdims=True)
            st['V'] = st['V0'] + weg * np.minimum(1.0, deckel / np.maximum(lg, 1e-6))
        bericht[s.n] = (len(fehl), float(np.mean(fehl)) if fehl else 0.0, float(np.max(fehl)) if fehl else 0.0)
    return bericht


'''
s = s[:a] + neu + s[b:]
open('b_snap.py', 'w', encoding='utf-8').write(s)
s = open('b_alles.py', encoding='utf-8').read()
s = s.replace("        b = SN.schritt([s1, s2], teile, ziele, rate=0.7 if it < schritte - 6 else 0.4)\n", "        b = SN.schritt([s1], teile, ziele, rate=0.6 if it < schritte - 3 else 0.35)\n")
s = s.replace("print('Snap %2d: Bild 1 %d Ecken, Mittel %.1f, max %.1f px | Bild 2 %d, %.1f, %.1f' % (it, *b[1], *b[2]), flush=True)", "print('Snap %2d: Bild 1 %d Ecken, Mittel %.1f, max %.1f px' % (it, *b[1]), flush=True)")
s = s.replace("def durchgang(fit_pfad, tag, ab='mesh', schritte=24, malen=None):", "def durchgang(fit_pfad, tag, ab='mesh', schritte=10, malen=None):")
s = s.replace("ap.add_argument('--snap', type=int, default=24)", "ap.add_argument('--snap', type=int, default=10)")
s = s.replace("BUDGET = {'kuppel': 660, 'k01': 56, 'k02': 56, 'k03': 34, 'k04': 26, 'k05': 118, 'k06': 62, 'k07': 40, 'k08': 34, 'k11': 72, 'k12': 60, 'k13': 88, 'k14': 54, 'k15': 54, 'k16': 66, 'k17': 60, 'k18': 88, 'k19': 44, 'k20': 58}",
              "BUDGET = {'kuppel': 560, 'k01': 46, 'k02': 46, 'k03': 28, 'k04': 22, 'k05': 104, 'k06': 56, 'k07': 34, 'k08': 28, 'k11': 66, 'k12': 54, 'k13': 80, 'k14': 50, 'k15': 50, 'k16': 58, 'k17': 56, 'k18': 80, 'k19': 40, 'k20': 54}")
open('b_alles.py', 'w', encoding='utf-8').write(s)
