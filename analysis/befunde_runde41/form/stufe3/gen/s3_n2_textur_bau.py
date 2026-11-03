"""Nachbesserung 2, Textur-Spur, Bau: Nachfolger von s3_n1_textur.py bauen, fuer jede K-Variante.

    python s3_n2_textur.py bauen K_DIR AUS_DIR

Liest K_DIR/FORM.EMD, FORM.TIM, naht.json, form.json; schreibt AUS_DIR/FORM.EMD, FORM.TIM, form.json, naht.json, textur_n2.json.

Ursache (gemessen, s3_n2_textur.py netz/bild, Dossier nachbesserung2_textur.md): die Gelenkkoerper tragen in den Spielansichten rund 30 %
der sichtbaren Figur (Schulter, Ruecken, Hals, Knie), und ihre Haut ist seit NB1 ein Stueck von 12-32 Texeln, auf ein Ellipsoid von bis zu
5800 Einheiten Umfang gezogen: rund 97 Einheiten je Texel gegen 26-49 an Rumpf und Gliedern (Blockmass 7,2 gegen 2,3-3,7). Im Bild sind
das grosse, flache Texelbloecke mit Spruengen an jeder Polygonkante (Kante 16 gegen Innen 1,8) - das Mosaik.

Behebung (Gestaltung, kein RE-Wert):
- Je Gelenkkoerper eine periodische Hautkachel P x P (P = 64 Texel) der CLUT-Zeile seiner Umgebung (Zeile der 12 naechsten
  Originalflaechen desselben Meshes), durch Quilting auf dem Torus (s3_n2_textur.quilt_torus) aus den sauberen Hauttexeln DIESER Zeile
  (ohne Hell-/Wundflecken, Bloecke mit Helligkeit nahe der Umgebung): nur Indizes, die es schon gibt. Danach Mittelfarbe auf den Median
  der Umgebung (ton_angleichen: jede Texelfarbe skaliert und auf den naechsten VORHANDENEN Eintrag derselben CLUT-Zeile gesetzt): keine
  neue Farbe, keine Spiegelung. Koerper gleicher Zeile mit Ton +-7 teilen eine Kachel.
- Ablage: neue Seiten rechts der TIM (x ab 512, Seite 4, 5, ...; Port und form_emd lesen bis 15*128+255), je Kachel ein Feld
  (P + F) x (P + F) = 128 x 128 (Kachel 2 x 2), damit jede Flaeche bis 64 Texel zusammenhaengend darin liegt; zwei Felder je Seite.
- UV je Koerper: mittabstandstreue Azimutalabbildung um den Pol 0 (+n, bei den Kugeln die Aussenseite) mit der Texeldichte der Umgebung
  (Median der 12 naechsten Originalflaechen, 22-50 Einheiten je Texel); jede Flaeche um Vielfache von P ins Feld verschoben (Kachel
  periodisch = gleicher Inhalt). Stetig ausser am Gegenpol (im Glied). Vergleich 'laenge' (Laengen-/Breitengrad, N2_ABBILDUNG=laenge):
  Pol-Stern auf der sichtbaren Beule, verworfen.
- Ausnahme Ellbogen r: wie NB1 'projektion' (er traegt in Bild 1 das weisse Dreieck Z5, E1/E3/E14).
- Kappen von Kopf/Kiefer (kappen_uv): Faecher der Rohr-Enden (k_form.rohr) zeigten eine einzige Texelzeile als Streifen; jetzt planar.
- Flaechen auf den alten Seiten 0-3, deren Texel rechts ueber x 511 hinausreichen, auf x 511 geklemmt (vorher lagen sie hinter der TIM,
  jetzt laegen sie sonst auf Seite 4).
"""
import os, sys, json, copy, shutil, math
import numpy as np

HIER = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HIER)
import s3_n2_textur as T        # noqa: E402
import form_emd as FE           # noqa: E402

P, F = 64, 64                   # Kachelperiode, Rand (Texel): jede Flaeche bis 64 Texel liegt zusammenhaengend im Feld
FELD = P + F
SCHRITT, UEBER = 16, 4          # Quilting-Raster (P / 4) und Ueberlappung
TON_TOL = 7.0
NAECHSTE = 12
DICHTE_MIN, DICHTE_MAX = 22.0, 50.0
PROJEKTION = ('Ellbogen r',)
SEED = 11
ABBILDUNG = os.environ.get('N2_ABBILDUNG', 'azimutal')     # 'azimutal' (gewaehlt) oder 'laenge' (Laengen-/Breitengrad, Vergleich)


def _quelle_zeile(form, tex, F_, z, alte_breite):
    """RGB-Abbild der Zeile z und Maske ihrer Quelltexel (alle Originalflaechen dieser Zeile, keine Gelenkkoerper, nur alte Seiten)."""
    H, W = tex.h, alte_breite
    msk = np.zeros((H, W), bool)
    for x in F_:
        f = x['f']
        if x['koerper'] is not None or tex.zeile(f['clut']) != z:
            continue
        po = (int(f['page']) & 0xF) * 128
        for (u, v) in T.texel_menge(f):
            if 0 <= u + po < W and 0 <= v < H:
                msk[v, u + po] = True
    idx = tex.px[:, :W]
    rgb = tex.lut[z][idx]
    msk &= idx != 0
    return idx, rgb, msk


def ton_angleichen(ti, lut, ziel):
    """Kachel auf die Mittelfarbe `ziel` der Umgebung bringen, Struktur behalten: jede Texelfarbe je Kanal mit ziel / Mittel skalieren und
    auf den naechsten VORHANDENEN Eintrag derselben CLUT-Zeile setzen (Index 1-255; keine neue Farbe). Zwei Runden (Quantisierung)."""
    pal = lut[1:].astype(float)
    for _ in range(2):
        cur = lut[ti].astype(float)
        m = cur.reshape(-1, 3).mean(0)
        want = cur * (np.asarray(ziel, float) / np.maximum(m, 1.0))
        d = ((want.reshape(-1, 1, 3) - pal[None]) ** 2).sum(-1)
        ti = (d.argmin(1) + 1).reshape(ti.shape)
    return ti


def _koerper_plan(form, tex, F_, naht):
    """Je Gelenkkoerper: Mesh, Ecken, Flaechen, Zeile, Ton, Dichte (aus den 12 naechsten Originalflaechen desselben Meshes)."""
    kn = T.koerper_namen(form, naht)
    plan = []
    for b, namen in kn.items():
        m = form['meshes'][b]
        V = np.array(m['ecken'], float)
        nk = len(namen)
        ne = nk * T.JE_KOERPER['ecken']
        e0 = len(V) - ne
        org = [x for x in F_ if x['b'] == b and x['koerper'] is None]
        om = np.array([V[x['f']['e']].mean(0) for x in org])
        for i, nm in enumerate(namen):
            ecken = np.arange(e0 + i * T.JE_KOERPER['ecken'], e0 + (i + 1) * T.JE_KOERPER['ecken'])
            mitte = V[ecken].mean(0)
            nah = np.argsort(np.linalg.norm(om - mitte, axis=1))[:NAECHSTE]
            zeilen = {}
            for j in nah:
                zeilen.setdefault(tex.zeile(org[j]['f']['clut']), []).append(j)
            z, js = max(zeilen.items(), key=lambda kv: len(kv[1]))
            Y, dichte, RGB = [], [], []
            for j in js:
                f = org[j]['f']
                _xy, c, ok = T.flaeche_texel(f, tex)
                Y += list(c[ok] @ T.YK)
                RGB += list(c[ok])
                dichte.append(math.sqrt(T.welt_flaeche(V, f) / max(T.uv_flaeche(f), 0.25)))
            fl = [x for x in F_ if x['b'] == b and x['koerper'] == nm]
            plan.append(dict(name=nm, b=b, ecken=ecken, flaechen=fl, zeile=z, clut=org[js[0]]['f']['clut'],
                             page_hoch=int(org[js[0]]['f']['page']) & 0xF0, ton=float(np.median(Y)) if Y else 50.0,
                             farbe=np.median(np.array(RGB), 0) if RGB else None,
                             dichte=float(min(max(np.median(dichte), DICHTE_MIN), DICHTE_MAX))))
    return plan


def _uv_koerper(V, ecken, dichte):
    """Laengen-/Breitengrad-Koordinaten (u, v) in Texeln je Ecke. Ecken-Reihenfolge k_naht.ellipsoid_netz: (V-1) Ringe zu U, dann Pol 0
    (Breite 0), dann Pol V. Rueckgabe dict Ecke -> (u oder None am Pol, v), Umfang in Texeln (Vielfaches von P)."""
    U, Vn = T.KN.KUGEL_U, T.KN.KUGEL_V
    Q = V[ecken]
    ring = Q[:(Vn - 1) * U].reshape(Vn - 1, U, 3)
    p0, p1 = Q[(Vn - 1) * U], Q[(Vn - 1) * U + 1]
    # Bogen vom Pol 0 je Ring (Mittel ueber die Laengen)
    bog = [0.0]
    prev = np.repeat(p0[None], U, 0)
    for r in range(Vn - 1):
        bog.append(bog[-1] + float(np.linalg.norm(ring[r] - prev, axis=1).mean()))
        prev = ring[r]
    bog.append(bog[-1] + float(np.linalg.norm(p1[None] - prev, axis=1).mean()))
    umf = max(float(np.linalg.norm(np.roll(ring[r], -1, 0) - ring[r], axis=1).sum()) for r in range(Vn - 1))
    k = max(1, int(math.ceil(umf / (dichte * P * 1.15))))      # nie groeber als 1,15 x die Umgebung
    Uc = k * P
    out = {}
    for r in range(Vn - 1):
        for j in range(U):
            out[int(ecken[r * U + j])] = (Uc * j / float(U), bog[r + 1] / dichte)
    out[int(ecken[(Vn - 1) * U])] = (None, 0.0)
    out[int(ecken[(Vn - 1) * U + 1])] = (None, bog[-1] / dichte)
    return out, Uc, umf


def _uv_azimutal(V, ecken, dichte):
    """Mittabstandstreue Azimutalabbildung um den Pol 0 (+n: bei den Kugeln die Aussenseite = die sichtbare Beule): je Ecke (Radius in
    Texeln = Bogen vom Pol 0 / Dichte, Winkel um die Achse). Stetig ueberall ausser am Gegenpol (steckt im Eltern- bzw. Kindglied);
    keine Umlaufnaht, kein Pol-Stern auf der Aussenseite. Rueckgabe dict Ecke -> (r, phi oder None am Gegenpol), Umfang (Einheiten)."""
    U, Vn = T.KN.KUGEL_U, T.KN.KUGEL_V
    Q = V[ecken]
    ring = Q[:(Vn - 1) * U].reshape(Vn - 1, U, 3)
    p0, p1 = Q[(Vn - 1) * U], Q[(Vn - 1) * U + 1]
    out = {int(ecken[(Vn - 1) * U]): (0.0, 0.0)}
    prev = np.repeat(p0[None], U, 0)
    bog = np.zeros(U)
    for r in range(Vn - 1):
        bog = bog + np.linalg.norm(ring[r] - prev, axis=1)
        prev = ring[r]
        for j in range(U):
            out[int(ecken[r * U + j])] = (float(bog[j]) / dichte, 2 * math.pi * j / float(U))
    out[int(ecken[(Vn - 1) * U + 1])] = (float((bog + np.linalg.norm(p1[None] - prev, axis=1)).mean()) / dichte, None)
    umf = max(float(np.linalg.norm(np.roll(ring[r], -1, 0) - ring[r], axis=1).sum()) for r in range(Vn - 1))
    return out, umf


def _flaeche_uv_az(f, kuv):
    rs = [kuv[e][0] for e in f['e']]
    ph = [kuv[e][1] for e in f['e']]
    da = [x for x in ph if x is not None]
    pm = math.atan2(sum(math.sin(x) for x in da), sum(math.cos(x) for x in da))
    ph = [pm if x is None else x for x in ph]
    us = [r * math.cos(a) for r, a in zip(rs, ph)]
    vs = [r * math.sin(a) for r, a in zip(rs, ph)]
    su = math.floor(min(us) / P) * P
    sv = math.floor(min(vs) / P) * P
    uu = [u - su for u in us]
    vv = [v - sv for v in vs]
    ueber = max(max(uu), max(vv)) > FELD - 1
    uu = [min(max(u, 0.0), FELD - 1.0) for u in uu]
    vv = [min(max(v, 0.0), FELD - 1.0) for v in vv]
    return uu, vv, ueber


def _flaeche_uv(f, kuv, Uc):
    """UV der Ecken einer Koerperflaeche im Feld: Umlauf entwirren, Pol-u = Mittel der anderen, um Vielfache von P verschieben."""
    us = [kuv[e][0] for e in f['e']]
    vs = [kuv[e][1] for e in f['e']]
    da = [u for u in us if u is not None]
    if max(da) - min(da) > Uc / 2.0:
        da = [u + Uc if u < Uc / 2.0 else u for u in da]
        us = [None if u is None else (u + Uc if u < Uc / 2.0 else u) for u in us]
    um = sum(da) / len(da)
    us = [um if u is None else u for u in us]
    su = math.floor(min(us) / P) * P
    sv = math.floor(min(vs) / P) * P
    uu = [u - su for u in us]
    vv = [v - sv for v in vs]
    ueber = max(max(uu), max(vv)) > FELD - 1
    uu = [min(max(u, 0.0), FELD - 1.0) for u in uu]
    vv = [min(max(v, 0.0), FELD - 1.0) for v in vv]
    return uu, vv, ueber


KAPPEN_MESHES = (22, 23)     # Kopf, Kiefer (k_form.rohr: Faecherkappen an Schnauze/Hinterkopf/Kinn)


def kappen_uv(form, naht, meshes):
    """Faecherkappen der Rohre (k_form.rohr, spitze0/1) bilden ihre UV auf EINE Texelzeile ab (Mitte und Ring liegen in der ersten/letzten
    Zeile): die Kappe zeigt eine einzige, ueber die ganze Flaeche gezogene Texelreihe als konzentrische Streifen. Neu: jede Kappe planar
    (Ebene senkrecht zur Achse Ringmitte -> Spitze) mit der Texeldichte des Meshes in dessen Hautfeld, Mitte unter der UV-Mitte des Rings,
    um den Kappenradius ins Feld hinein verschoben. Erkennung: Dreiecke mit UV-Flaeche <= 1 Texel und Weltflaeche > 2000."""
    F_ = T.flaechen(form, naht)
    ber = {}
    for b in meshes:
        m = form['meshes'][b]
        if not m['ecken']:
            continue
        V = np.array(m['ecken'], float)
        org = [x for x in F_ if x['b'] == b and x['koerper'] is None]
        deg = [x for x in org if x['art'] == 0 and T.uv_flaeche(x['f']) <= 1.0 and T.welt_flaeche(V, x['f']) > 2000]
        gut = [x for x in org if x not in deg]
        if not deg or not gut:
            continue
        # Faecher = Gruppen mit gemeinsamer Spitze (haeufigste Ecke)
        from collections import Counter
        reste = list(deg)
        n_k = 0
        while reste:
            c = Counter(e for x in reste for e in x['f']['e'])
            spitze, _n = c.most_common(1)[0]
            fan = [x for x in reste if spitze in x['f']['e']]
            reste = [x for x in reste if spitze not in x['f']['e']]
            if len(fan) < 3:
                continue
            f0 = fan[0]['f']
            seite, clut = f0['page'], f0['clut']
            # Insel des Rohrs: von den Nachbarflaechen des Rings ueber gemeinsame Ecken (Zaehne/Augenhoecker haben eigene Ecken)
            ring0 = set(e for x in fan for e in x['f']['e'] if e != spitze)
            insel = [x for x in gut if ring0 & set(x['f']['e'])]
            ecken_i = set(e for x in insel for e in x['f']['e'])
            while True:
                dazu = [x for x in gut if x not in insel and ecken_i & set(x['f']['e'])]
                if not dazu:
                    break
                insel += dazu
                for x in dazu:
                    ecken_i |= set(x['f']['e'])
            ref = [x['f'] for x in insel if x['f']['page'] == seite and x['f']['clut'] == clut]
            if not ref:
                continue
            ts = float(np.median([math.sqrt(T.welt_flaeche(V, f) / max(T.uv_flaeche(f), 0.25)) for f in ref]))
            UV = np.array([q for f in ref for q in f['uv'][:len(f['e'])]], float)
            u_lo, v_lo = UV.min(0)
            u_hi, v_hi = UV.max(0)
            ring = sorted(set(e for x in fan for e in x['f']['e'] if e != spitze))
            rc = V[ring].mean(0)
            ax = V[spitze] - rc
            ax = ax / max(np.linalg.norm(ax), 1e-9)
            a = np.cross(ax, [0.0, 1.0, 0.0])
            if np.linalg.norm(a) < 1e-3:
                a = np.cross(ax, [1.0, 0.0, 0.0])
            a = a / np.linalg.norm(a)
            bb = np.cross(ax, a)
            R = max(float(np.linalg.norm((V[e] - rc) - ((V[e] - rc) @ ax) * ax)) for e in ring + [spitze])
            r_tex = R / ts
            halb = 0.5 * min(u_hi - u_lo, v_hi - v_lo) - 1.0
            if r_tex > halb:
                ts *= r_tex / halb
                r_tex = halb
            ruv = np.array([q for x in fan for i, q in zip(x['f']['e'], x['f']['uv']) if i != spitze], float)
            uc = float(np.clip(ruv[:, 0].mean(), u_lo + r_tex + 1, u_hi - r_tex - 1))
            v0 = float(ruv[:, 1].mean())
            vc = v0 + r_tex + 1 if v0 < 0.5 * (v_lo + v_hi) else v0 - r_tex - 1
            vc = float(np.clip(vc, v_lo + r_tex + 1, v_hi - r_tex - 1))
            for x in fan:
                f = x['f']
                f['uv'] = [[int(round(uc + ((V[e] - rc) @ a) / ts)), int(round(vc + ((V[e] - rc) @ bb) / ts))] for e in f['e']]
            n_k += 1
        ber[str(b)] = dict(kappen=n_k, flaechen=len(deg))
    return ber


def bauen(k_dir, aus, ausgabe=True):
    emd = open(os.path.join(k_dir, 'FORM.EMD'), 'rb').read()
    tim = open(os.path.join(k_dir, 'FORM.TIM'), 'rb').read()
    naht = json.load(open(os.path.join(k_dir, 'naht.json'), encoding='utf-8'))
    fj = json.load(open(os.path.join(k_dir, 'form.json'), encoding='utf-8'))
    form = FE.lese_emd(emd)
    tex = T.Tex(tim)
    t = FE.tim_lesen(tim)
    alte_breite = min(t['pixel'].shape[1], 512)
    F_ = T.flaechen(form, naht)
    plan = _koerper_plan(form, tex, F_, naht)
    # Kacheln: je (Zeile, Ton +- TON_TOL) eine
    kacheln = []
    for k in plan:
        if k['name'] in PROJEKTION:
            continue
        for kk in kacheln:
            if kk['zeile'] == k['zeile'] and abs(kk['ton'] - k['ton']) <= TON_TOL:
                kk['koerper'].append(k['name'])
                k['kachel'] = kk['nr']
                break
        else:
            kacheln.append(dict(nr=len(kacheln), zeile=k['zeile'], ton=k['ton'], koerper=[k['name']]))
            k['kachel'] = len(kacheln) - 1
    je_seite = (128 // FELD) * (256 // FELD)
    seiten = (len(kacheln) + je_seite - 1) // je_seite
    neue_breite = 512 + 128 * seiten
    px = np.zeros((tex.h, neue_breite), np.uint8)
    px[:, :t['pixel'].shape[1]] = t['pixel'][:, :min(t['pixel'].shape[1], neue_breite)]
    rng = np.random.RandomState(SEED)
    quellen = {}
    for kk in kacheln:
        z = kk['zeile']
        if z not in quellen:
            idx, rgb, msk = _quelle_zeile(form, tex, F_, z, alte_breite)
            quellen[z] = (idx, rgb, T.saubere_quelle(rgb, msk))
        idx, rgb, sauber = quellen[z]
        ys, xs = T.kandidaten(rgb, sauber, SCHRITT + UEBER, ton=kk['ton'], tol=TON_TOL, mindest=40)
        if ys is None or not len(ys):
            raise SystemExit('keine sauberen Quellbloecke fuer Zeile %d' % z)
        ti, tc = T.quilt_torus(idx, rgb, ys, xs, P, P, SCHRITT, UEBER, rng)
        ziel = np.mean([k['farbe'] for k in plan if k.get('kachel') == kk['nr'] and k['farbe'] is not None], 0)
        ton_vor = float((tc @ T.YK).mean())
        ti = ton_angleichen(ti, tex.lut[z], ziel)
        tc = tex.lut[z][ti]
        feld = np.tile(ti, (2, 2))[:FELD, :FELD]
        s = kk['nr'] // je_seite
        r = kk['nr'] % je_seite
        x0 = 512 + 128 * s + FELD * (r % (128 // FELD))
        y0 = FELD * (r // (128 // FELD))
        px[y0:y0 + FELD, x0:x0 + FELD] = feld.astype(np.uint8)
        kk.update(x0=x0, y0=y0, quellbloecke=int(len(ys)), ton_vor_angleich=round(ton_vor, 1), ton_kachel=round(float((tc @ T.YK).mean()), 1),
                  farbe_ziel=[round(float(x), 1) for x in ziel], farbe_kachel=[round(float(x), 1) for x in tc.reshape(-1, 3).mean(0)],
                  koernung_kachel=round(float((tc @ T.YK).std()), 1))
    neu = copy.deepcopy(form)
    bericht = dict(kacheln=[{k: v for k, v in kk.items()} for kk in kacheln], koerper={})
    # Ellbogen r & Co.: NB1-Projektion
    if any(k['name'] in PROJEKTION for k in plan):
        import s3_n1_textur as T1
        p_form, _b = T1.neu_uv(form, naht, T1.Tex(tim), 'projektion')
        for k in plan:
            if k['name'] not in PROJEKTION:
                continue
            for x in k['flaechen']:
                src = p_form['meshes'][x['b']]['dreiecke' if x['art'] == 0 else 'vierecke'][x['i']]
                dst = neu['meshes'][x['b']]['dreiecke' if x['art'] == 0 else 'vierecke'][x['i']]
                dst['uv'], dst['page'], dst['clut'] = [list(q) for q in src['uv']], src['page'], src['clut']
            bericht['koerper'][k['name']] = dict(art='projektion (NB1)', zeile=k['zeile'])
    for k in plan:
        if k['name'] in PROJEKTION:
            continue
        kk = kacheln[k['kachel']]
        V = np.array(neu['meshes'][k['b']]['ecken'], float)
        if ABBILDUNG == 'azimutal':
            kuv, umf = _uv_azimutal(V, k['ecken'], k['dichte'])
            Uc = None
        else:
            kuv, Uc, umf = _uv_koerper(V, k['ecken'], k['dichte'])
        seite = kk['x0'] // 128
        u_basis = kk['x0'] - seite * 128
        ueber = 0
        for x in k['flaechen']:
            dst = neu['meshes'][x['b']]['dreiecke' if x['art'] == 0 else 'vierecke'][x['i']]
            uu, vv, ue = _flaeche_uv_az(dst, kuv) if ABBILDUNG == 'azimutal' else _flaeche_uv(dst, kuv, Uc)
            ueber += int(ue)
            if ue:
                bericht.setdefault('ueber_feld_flaechen', []).append([x['b'], x['art'], x['i']])
            dst['uv'] = [[int(round(u_basis + u)), int(round(kk['y0'] + v))] for u, v in zip(uu, vv)]
            dst['page'] = k['page_hoch'] | seite
            dst['clut'] = k['clut']
        bericht['koerper'][k['name']] = dict(art='kachel', kachel=k['kachel'], zeile=k['zeile'], ton_umgebung=round(k['ton'], 1),
                                             dichte=round(k['dichte'], 1), umfang=round(umf), abbildung=ABBILDUNG,
                                             dichte_ist=round(umf / Uc, 1) if Uc else round(k['dichte'], 1), flaechen_ueber_feld=ueber)
    bericht['kappen'] = kappen_uv(neu, naht, KAPPEN_MESHES)
    # alte Seiten: nichts rechts ueber x 511 (vorher hinter der TIM, jetzt laege es auf Seite 4)
    geklemmt = 0
    for m in neu['meshes']:
        for f in m['dreiecke'] + m['vierecke']:
            pg = int(f['page']) & 0xF
            if pg <= 3:
                rand = 511 - pg * 128
                for q in f['uv']:
                    if q[0] > rand:
                        q[0] = rand
                        geklemmt += 1
    bericht['uv_geklemmt'] = geklemmt
    bericht['tim'] = dict(breite=neue_breite, hoehe=int(tex.h), seiten_neu=seiten, feld=FELD, periode=P)
    if not ausgabe:
        return neu, px, t, bericht
    os.makedirs(aus, exist_ok=True)
    open(os.path.join(aus, 'FORM.EMD'), 'wb').write(FE.schreibe_emd(neu))
    open(os.path.join(aus, 'FORM.TIM'), 'wb').write(FE.tim_schreiben(px, t['cluts'], t['clut_x'], t['clut_y'], t['bild_x'], t['bild_y']))
    fj2 = dict(fj)
    fj2['name'] = os.path.basename(os.path.normpath(aus))
    json.dump(fj2, open(os.path.join(aus, 'form.json'), 'w', encoding='utf-8'), indent=1)
    shutil.copy(os.path.join(k_dir, 'naht.json'), os.path.join(aus, 'naht.json'))
    bericht['quelle'] = os.path.relpath(k_dir, aus)
    json.dump(bericht, open(os.path.join(aus, 'textur_n2.json'), 'w', encoding='utf-8'), indent=1, default=lambda o: int(o) if isinstance(o, np.integer) else float(o))
    print('Kacheln', len(kacheln), 'TIM', neue_breite, 'x', tex.h, 'geklemmt', geklemmt)
    for nm, b in bericht['koerper'].items():
        print('  %-24s %s' % (nm, b))
    return bericht
