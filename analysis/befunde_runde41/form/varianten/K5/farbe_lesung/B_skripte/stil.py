"""STIL T1-T8 an den Texeln: EM30_HANGAR (Haut-Meshes), K5c (Kopf 22, Kiefer 23, Koerper J), K4 heute."""
import json, math, sys
import numpy as np, cv2
import fl, texel, zonen as Z

EM30_JEANS = (0, 1, 3, 4, 6)   # Meshes mit Jeans/Schuhen (Mittel b* -5,9 bis -9,9, Atlas x 128-254 y 124-254), keine Haut


def kmeans_lab(L, k, seed=0):
    Zf = L.astype(np.float32)
    cv2.setRNGSeed(seed)
    crit = (cv2.TERM_CRITERIA_EPS + cv2.TERM_CRITERIA_MAX_ITER, 60, 0.1)
    _, lab_, cen = cv2.kmeans(Zf, min(k, len(Zf)), None, crit, 5, cv2.KMEANS_PP_CENTERS)
    lab_ = lab_.ravel()
    ant = np.bincount(lab_, minlength=len(cen)) / len(lab_)
    o = np.argsort(-ant)
    return cen[o].astype(float), ant[o]


def zaehle(L, k=8, merge=10.0):
    if len(L) < 40:
        return None
    cen, ant = kmeans_lab(L, k)
    grp = list(range(len(cen)))

    def f(i):
        while grp[i] != i:
            i = grp[i]
        return i
    for i in range(len(cen)):
        for j in range(i + 1, len(cen)):
            if np.linalg.norm(cen[i] - cen[j]) < merge:
                grp[f(j)] = f(i)
    s = {}
    for i, a in enumerate(ant):
        s[f(i)] = s.get(f(i), 0) + a
    return sum(1 for a in s.values() if a >= 0.05)


def hf_anteil(mo, per, feld, meshes, grenze=0.25):
    """Energie bei Frequenzen > 0,25 Zyklen/Texel (Periode < 4 Texel) gegen gesamt ohne DC, Luminanz L*, je Atlasfeld (Zusammenhangskomponente
    der Texel eines Meshes), Hann-Fenster, Flaechenmittel."""
    h, w = mo.tex.shape
    num = den = 0.0
    for b in meshes:
        if b not in per:
            continue
        d = per[b]
        img = np.zeros((h, w), np.float32)
        m = np.zeros((h, w), np.uint8)
        img[d['xy'][:, 1], d['xy'][:, 0]] = fl.lab(d['rgb'])[:, 0]
        m[d['xy'][:, 1], d['xy'][:, 0]] = 1
        n, lab_, st, _ = cv2.connectedComponentsWithStats(m, connectivity=8)
        for i in range(1, n):
            x, y, ww, hh, a = st[i]
            if ww < 12 or hh < 12 or a < 144:
                continue
            sub = img[y:y + hh, x:x + ww].copy()
            sm = lab_[y:y + hh, x:x + ww] == i
            mu = sub[sm].mean()
            sub[~sm] = mu
            sub -= mu
            win = np.outer(np.hanning(hh), np.hanning(ww)).astype(np.float32)
            P = np.abs(np.fft.fft2(sub * win)) ** 2
            fy = np.fft.fftfreq(hh)[:, None]
            fx = np.fft.fftfreq(ww)[None, :]
            fr = np.hypot(fx, fy)
            tot = P[fr > 0].sum()
            if tot <= 0:
                continue
            num += a * P[fr > grenze].sum() / tot
            den += a
    return num / den if den else None


def messen(mo, per, tri, feld, meshes, ausnahme=None, name=''):
    rgb = []
    for b in meshes:
        if b not in per:
            continue
        d = per[b]
        keep = np.ones(len(d['rgb']), bool)
        if ausnahme is not None:
            keep = ~ausnahme(d)
        rgb.append(d['rgb'][keep])
    rgb = np.concatenate(rgb)
    L = fl.lab(rgb)
    cen, ant = kmeans_lab(L, 8)
    C = np.hypot(L[:, 1], L[:, 2])
    farben = [zaehle(fl.lab(per[b]['rgb'][~ausnahme(per[b])] if ausnahme else per[b]['rgb'])) for b in meshes if b in per and len(per[b]['rgb']) >= 200]
    farben = [f for f in farben if f is not None]
    dens = [math.sqrt(uvA / A3) for (b, uvA, A3) in tri if b in meshes and A3 > 1 and uvA > 0]
    hue = np.degrees(np.arctan2(L[:, 2], L[:, 1]))
    rot = (L[:, 1] >= 15) & (L[:, 0] >= 20) & (hue > -30) & (hue < 70)
    rot_n = zaehle(L[rot], k=6) if rot.sum() >= 40 else 0
    r = dict(name=name, texel=int(len(L)),
             T1_palette=[dict(lab=[round(float(x), 1) for x in c], anteil=round(float(a), 3)) for c, a in zip(cen, ant)],
             T2_farben_je_teil=round(float(np.mean(farben)), 2) if farben else None, T2_meshes=len(farben),
             T3_texeldichte_mittel=round(float(np.mean(dens)), 4), T3_texeldichte_median=round(float(np.median(dens)), 4),
             T4_glanz_L85_proz=round(100 * float((L[:, 0] > 85).mean()), 2),
             T5_hf=round(float(hf_anteil(mo, per, feld, meshes)), 4),
             T6_chroma_mittel=round(float(C.mean()), 1), T6_chroma_streu=round(float(C.std()), 1),
             L_mittel=round(float(L[:, 0].mean()), 1), L_p5_p95=[round(float(np.percentile(L[:, 0], 5)), 1), round(float(np.percentile(L[:, 0], 95)), 1)],
             T7_rot_anteil_proz=round(100 * float(rot.mean()), 1), T7_rot_cluster=rot_n,
             T7_rot_L_p5_p95=[round(float(np.percentile(L[rot, 0], 5)), 1), round(float(np.percentile(L[rot, 0], 95)), 1)] if rot.sum() > 20 else None,
             T7_rot_mittel_lab=[round(float(x), 1) for x in L[rot].mean(0)] if rot.sum() > 20 else None,
             klassen=None)
    kk = Z.klassen(L, np.zeros(len(L), bool))
    r['klassen'] = {Z.KL[i][0]: round(float((kk == i).mean()), 3) for i in range(5)}
    return r


def zahn_auge(d):
    return (d['xy'][:, 0] >= 384) & (d['xy'][:, 1] >= 175)    # K5-Feld Zaehne (Dreieck x 385-407, y 180-192) und Auge (x 384-408, y 197-203)


if __name__ == '__main__':
    aus = {}
    em = fl.lade_em30()
    per, tri, feld = texel.sammeln(em)
    haut = [b for b in per if b not in EM30_JEANS]
    aus['EM30_haut'] = messen(em, per, tri, feld, haut, name='EM30_HANGAR Haut (Meshes ohne Jeans)')
    aus['EM30_alle'] = messen(em, per, tri, feld, list(per), name='EM30_HANGAR alle Meshes')
    for nm in ('K5c', 'K4_heute'):
        mo = fl.lade_var(fl.K5 + '/' + nm)
        per, tri, feld = texel.sammeln(mo)
        koerper = [b for b in per if b not in (21, 22, 23)]
        aus[nm + '_gesamt'] = messen(mo, per, tri, feld, list(per), ausnahme=zahn_auge if nm == 'K5c' else None, name=nm + ' gesamt ohne Zaehne/Augen')
        aus[nm + '_koerperJ'] = messen(mo, per, tri, feld, koerper, name=nm + ' Koerper J (ohne 21-23)')
        aus[nm + '_kopf22'] = messen(mo, per, tri, feld, [21, 22], ausnahme=zahn_auge if nm == 'K5c' else None, name=nm + ' Hals+Kopf (21,22)')
        aus[nm + '_kiefer23'] = messen(mo, per, tri, feld, [23], ausnahme=zahn_auge if nm == 'K5c' else None, name=nm + ' Unterkiefer (23)')
        if nm == 'K5c':
            za = []
            for b in (22, 23):
                d = per[b]
                s = zahn_auge(d)
                za.append(d['rgb'][s])
            za = np.concatenate(za)
            L = fl.lab(za)
            cen, ant = kmeans_lab(L, 4)
            aus['K5c_zaehne_augen'] = [dict(lab=[round(float(x), 1) for x in c], anteil=round(float(a), 3)) for c, a in zip(cen, ant)]
    json.dump(aus, open(fl.S + '/stil.json', 'w'), indent=1)
    for k, v in aus.items():
        print('==', k)
        if isinstance(v, dict):
            for kk, vv in v.items():
                if kk != 'T1_palette':
                    print('   ', kk, vv)
            print('    T1', [(p['lab'], p['anteil']) for p in v['T1_palette']])
        else:
            print('   ', v)
