#!/usr/bin/env python3
"""Variante D (E10 und Nachtrag des Nutzers): Farben der Haut und der Wunden aus den Screenshots, nicht gemalt. Kein RE-Wert.

Weg je Texel:
 1. Lage im Bild: rf_textur.Bake (Pose und Kamera von Bild 1 und Bild 2, Tiefentest), Bildpunkt q und Sichtbarkeit.
 2. Schirmfarbe: farbe_bild{1,2}_echt.png (form_video.umkehr: Helligkeitskurve, Streulicht, Kanalbalance zurueckgerechnet), bilinear an q.
 3. Licht des Ports an derselben Stelle: dasselbe Modell mit weisser Textur durch form_pruefung.Kandidat.rendern (gleiche Kamera, gleiches Licht 'mess').
    Textur = Schirmfarbe / Licht. Dann zeigt der Port an dieser Stelle die Schirmfarbe, und video_filter macht daraus wieder das Video
    (Hin und zurueck MAE 6,6 / 10,4, farbe.md).
 4. Gewicht: Licht (dunkle Stellen sind unsicher), Abstand zum Umriss (am Rand mischt die Unschaerfe den Hintergrund ein), Logo aus, Bild 1 vor Bild 2.
    Bild 2 hat Feuerschein, den das Port-Licht nicht kennt. Ein Kanalfaktor aus Texeln, die beide Bilder sehen, faerbte die Vorderseite tuerkis (D1);
    darum ohne Ausgleich: was nur Bild 2 zeigt, behaelt die Farbe von Bild 2 (Nutzer: Farben aus dem Bild uebernehmen).
 5. Unsichtbare Texel: aus demselben Teil, zuerst die Gegenseite (Spiegel im Streifen U -> 1-U und U -> U+1/2), dann Fortsetzung laengs und rundum
    (Ausfuellen je Teil), Teile ohne sichtbares Texel nehmen das Gegenstueck (links/rechts) oder den Median ihrer Gruppe.
 6. PSX-Form: Farben in Bereiche zusammenfassen (Mean-Shift je Teil), Kanten schaerfen, die gemalte Form (Furchen, Fasern, Korn von vm_mal) leise
    darueber, damit Texelkanten hart sind und keine Flaeche einfarbig ist. Danach CLUT-Zeilen wie RE2 (vm_bau.tim_zeilen).
"""
import json
import os

import cv2
import numpy as np

from rf_kern import FORM, HIER
import form_emd as FE
import form_pruefung as P
import vm_mal

YK = np.array([.299, .587, .114], np.float32)
BILD_GEWICHT = {1: 1.0, 2: 0.5}       # Nutzer: "vor allem aus Bild 1 (Rueckansicht)"
RAND_PX = {1: 5.0, 2: 2.5}                         # Abstand zum Renderumriss, ab dem die Schirmfarbe voll zaehlt: Optik-Gauss 2 px und Chroma 6 x 3 px (farbe.md);
                                       # Bild 1 mit 2,5 trugen die Unterschenkel den tuerkisen Boden (Lauf _probe/D1); Bild 2 hat dunklen Grund,
                                       # dort nahm 5 px die duennen hellen Zuege Q und G weg (D2)
BILD2_AUSGLEICH = False                # Lauf D1: Faktor aus 1773 gemeinsamen Texeln (0,62/1,15/1,12) faerbte die Vorderseite tuerkis, Bild 2 dE 2,2 -> 7,3
LICHT_MIN = 0.12                       # darunter zaehlt ein Texel nicht (Teilen durch fast 0)


def licht_bilder(aus, form_json):
    """Port-Licht je Bildpunkt: Modell mit weisser Textur (alle CLUT-Eintraege 0x7FFF) durch Kandidat.rendern. -> {n: (Licht HxWx3 0..1, Maske)}"""
    j = json.load(open(form_json, encoding='utf-8'))
    t = FE.tim_lesen(open(os.path.join(aus, j['tim']), 'rb').read())
    px = np.maximum(t['pixel'], 1)
    weiss = [[0] + [0x7FFF] * 255 for _ in t['cluts']]
    open(os.path.join(aus, '_WEISS.TIM'), 'wb').write(FE.tim_schreiben(px, weiss))
    jw = dict(j, tim='_WEISS.TIM', name=j.get('name', 'D') + '_weiss')
    pj = os.path.join(aus, '_weiss.json')
    json.dump(jw, open(pj, 'w', encoding='utf-8'))
    k = P.Kandidat(pj)
    aus_ = {}
    for n in (1, 2):
        lage, gier, clip, bild = k.stand(n)
        hg = np.zeros((P.HOEHE, P.BREITE, 3), np.uint8)
        import form_buehne as FB
        kam = P.kamera(n)
        kf = k.kf(clip, bild)
        licht, _ = P.licht_fuer(n, k.licht)
        m = FB.rastern(FB.dreiecke(k.mo, kf, lage, gier, kam, licht), k.tex, hg, kam.abbildung)
        aus_[n] = (hg.astype(np.float32) / 248.0, P.geschlossen(m))
    for f in ('_WEISS.TIM', '_weiss.json'):
        os.remove(os.path.join(aus, f))
    return aus_


def _abtasten(bild, q, sel):
    x = np.nan_to_num(q[..., 0][sel], nan=-10).astype(np.float32).reshape(-1, 1)
    y = np.nan_to_num(q[..., 1][sel], nan=-10).astype(np.float32).reshape(-1, 1)
    return cv2.remap(bild, x, y, cv2.INTER_LINEAR, borderMode=cv2.BORDER_REPLICATE).reshape(-1, bild.shape[2] if bild.ndim == 3 else 1)


def aus_bild(ml, licht, ausgabe=None):
    """-> rgb (HxWx3 float, Textur aus dem Bild, unsichtbar = NaN), Gewicht, Bericht."""
    bk, m = ml.bk, ml.m
    H, W = m.shape
    logo = P.logo_maske()
    summe = np.zeros((H, W, 3), np.float64)
    gew = np.zeros((H, W), np.float64)
    je = {}
    for n in (1, 2):
        echt = cv2.imread(os.path.join(FORM, 'farbe_bild%d_echt.png' % n))[:, :, ::-1].astype(np.float32)
        L, rm = licht[n]
        innen = cv2.distanceTransform((rm & ~logo).astype(np.uint8), cv2.DIST_L2, 3).astype(np.float32)
        q = bk.q[n]
        sel = bk.vis[n] & m
        S = _abtasten(echt, q, sel)
        Lt = _abtasten(L, q, sel)
        d = _abtasten(innen[..., None], q, sel)[:, 0]
        ly = Lt @ YK
        w = np.clip((ly - LICHT_MIN) / 0.25, 0, 1) * np.clip(d / RAND_PX[n], 0, 1) * BILD_GEWICHT[n]
        t = S / np.maximum(Lt, 0.08)
        tex = np.full((H, W, 3), np.nan, np.float32)
        ww = np.zeros((H, W), np.float32)
        tex[sel] = t
        ww[sel] = w
        je[n] = (tex, ww)
    # Feuerschein Bild 2: Kanalfaktor aus gemeinsam sichtbaren Texeln
    t1, w1 = je[1]
    t2, w2 = je[2]
    beide = (w1 > 0.5) & (w2 > 0.25)
    if BILD2_AUSGLEICH and beide.sum() > 30:
        f2 = np.median(t1[beide] + 1, axis=0) / np.median(t2[beide] + 1, axis=0)
    else:
        f2 = np.ones(3)
    f2 = np.clip(f2, 0.5, 2.0)
    for n, (t, w) in je.items():
        if n == 2:
            t = t * f2[None, None]
        ok = w > 0
        summe[ok] += t[ok] * w[ok][:, None]
        gew[ok] += w[ok]
    rgb = np.full((H, W, 3), np.nan, np.float32)
    ok = gew > 0.15
    rgb[ok] = (summe[ok] / gew[ok][:, None]).astype(np.float32)
    rgb = np.clip(rgb, 0, 255)
    ber = dict(texel=int(m.sum()), aus_bild=int((ok & m).sum()), gemeinsam=int(beide.sum()), faktor_bild2=[round(float(x), 3) for x in f2])
    return rgb, ok & m, ber


def ergaenzen(ml, rgb, bekannt):
    """Unsichtbare Texel je Teil: Gegenseite (U -> 1-U, U -> U+1/2 im Streifen), dann Ausfuellen je Teil (Telea), Teile ohne Bild vom Gegenstueck."""
    teile, lage, tid, m = ml.teile, ml.lage, ml.tid, ml.m
    out = rgb.copy()
    hat = bekannt.copy()
    namen = [p.name for p in teile]
    for i, p in enumerate(teile):
        x, y, w, h = lage[i]
        sub_t = tid[y:y + h, x:x + w] == i
        if not sub_t.any():
            continue
        R = out[y:y + h, x:x + w]
        K = hat[y:y + h, x:x + w] & sub_t
        for spiegel in (lambda c: w - 1 - c, lambda c: (c + w // 2) % w):
            cols = np.arange(w)
            src = spiegel(cols)
            fehlt = sub_t & ~K
            if not fehlt.any():
                break
            ys, xs = np.nonzero(fehlt)
            sx = src[xs]
            okq = K[ys, sx]
            R[ys[okq], xs[okq]] = R[ys[okq], sx[okq]]
            K[ys[okq], xs[okq]] = True
        hat[y:y + h, x:x + w] |= K
    # Ausfuellen je Teil (Fortsetzung laengs und rundum)
    for i, p in enumerate(teile):
        x, y, w, h = lage[i]
        sub_t = tid[y:y + h, x:x + w] == i
        if not sub_t.any():
            continue
        K = hat[y:y + h, x:x + w] & sub_t
        fehlt = sub_t & ~K
        if not fehlt.any() or K.sum() < 4:
            continue
        R = out[y:y + h, x:x + w].copy()
        R[~K] = 0
        r8 = np.clip(R, 0, 255).astype(np.uint8)
        maske = (~K).astype(np.uint8)
        f = cv2.inpaint(r8, maske, 4, cv2.INPAINT_TELEA).astype(np.float32)
        out[y:y + h, x:x + w][fehlt] = f[fehlt]
        hat[y:y + h, x:x + w] |= fehlt
    # Teile ganz ohne Bild: Gegenstueck links/rechts, sonst Median aller bekannten Haut
    med = np.median(out[hat & m], axis=0) if (hat & m).any() else np.array([40, 25, 35], np.float32)
    for i, p in enumerate(teile):
        x, y, w, h = lage[i]
        sub_t = tid[y:y + h, x:x + w] == i
        if not sub_t.any() or (hat[y:y + h, x:x + w] & sub_t).any():
            continue
        gegen = None
        gn = p.name.replace('_l', '_r') if '_l' in p.name else p.name.replace('_r', '_l') if '_r' in p.name else None
        if gn in namen and gn != p.name:
            gegen = namen.index(gn)
        if gegen is not None:
            gx, gy, gw, gh = lage[gegen]
            q = cv2.resize(out[gy:gy + gh, gx:gx + gw], (w, h), interpolation=cv2.INTER_NEAREST)
            qm = cv2.resize((hat[gy:gy + gh, gx:gx + gw] & (tid[gy:gy + gh, gx:gx + gw] == gegen)).astype(np.uint8), (w, h), interpolation=cv2.INTER_NEAREST) > 0
            if qm.any():
                tgt = sub_t & qm
                out[y:y + h, x:x + w][tgt] = q[tgt]
                rest = sub_t & ~qm
                out[y:y + h, x:x + w][rest] = np.median(q[qm], axis=0)
                hat[y:y + h, x:x + w] |= sub_t
                continue
        out[y:y + h, x:x + w][sub_t] = med
        hat[y:y + h, x:x + w] |= sub_t
    rest = m & (~hat | np.isnan(out).any(-1))
    out[rest] = med
    hat |= rest
    return out, hat


def psx_form(ml, rgb, sp=3, sr=14, schaerfe=0.6, form_staerke=0.12):
    """Bereiche zusammenfassen (Mean-Shift je Teil-Rechteck), Kanten schaerfen, gemalte Form leise darueber."""
    teile, lage, tid, m = ml.teile, ml.lage, ml.tid, ml.m
    out = rgb.copy()
    for i, p in enumerate(teile):
        x, y, w, h = lage[i]
        sub_t = tid[y:y + h, x:x + w] == i
        if not sub_t.any():
            continue
        R = np.clip(np.nan_to_num(out[y:y + h, x:x + w]), 0, 255).astype(np.uint8)
        # Rand des Teils mit der naechsten Teilfarbe auffuellen, damit der Mean-Shift nicht in Leerflaechen laeuft
        if (~sub_t).any():
            from scipy.ndimage import distance_transform_edt
            _, (iy, ix) = distance_transform_edt(~sub_t, return_indices=True)
            R = R[iy, ix]
        # im Wurzelraum zusammenfassen: gleiche Farbschwelle gilt dann relativ, dunkle Haut behaelt ihre Unterschiede (D3: Ruecken/Schwanz einfarbig)
        Rw = np.clip(np.sqrt(R.astype(np.float32) / 255.0) * 255, 0, 255).astype(np.uint8)
        msw = cv2.pyrMeanShiftFiltering(np.ascontiguousarray(Rw[:, :, ::-1]), sp, sr)[:, :, ::-1].astype(np.float32)
        ms = (msw / 255.0) ** 2 * 255
        bl = cv2.GaussianBlur(ms, (0, 0), 1.0)
        sh = np.clip(ms + schaerfe * (ms - bl), 0, 255)
        out[y:y + h, x:x + w][sub_t] = sh[sub_t]
    # gemalte Form: Furchen dunkel, Fasern und Korn, Woelbung (vm_mal, Werte je Texel in Reihenfolge m)
    mod = np.ones(m.shape, np.float32)
    fur = ml.furche * np.where(ml.ist_schwanz[m], 0.0, 1.0)
    mod[m] = (1 + form_staerke * (0.9 * ml.faser + 0.5 * ml.korn + 0.8 * (ml.woelb - 0.5)) - 0.2 * fur).astype(np.float32)   # 0,45 las als Risslinien (D2)
    out = out * np.clip(mod, 0.4, 1.4)[..., None]
    return np.clip(out, 0, 255)


WUNDE_TIEF = (70, 12, 20)       # dunkelrote Tiefe (Handwerk RE2-Birkin: Kern rot, Tiefen fast schwarzrot, gen/../D_fleisch/handwerk/re2_fleisch.json)
WUNDE_GLANZ = (238, 222, 232)   # feuchtes Glanzlicht
WUNDE_ENTZUENDET = (92, 26, 36)


def wunden(ml, rgb, ohne=None):
    """Struktur in den Markenflaechen (Lage aus den Bildmasken, Farbe aus dem Bild): im Mittel bleibt die Bildfarbe, darin Fasern, rote Tiefen,
    feuchte Glanzpunkte (oben im Licht), gerissener Rand (heller mit dunklen Kerben), entzuendeter Ring aussen. Hart, ohne Weichzeichnung."""
    m = ml.m
    H, W = m.shape
    alle = np.zeros((H, W), bool)
    ring = np.zeros((H, W), bool)
    for i, nm, grund, fak, s, r in ml.marken:
        alle |= s
        if r is not None:
            ring |= r
    ring &= ~alle
    if not alle.any():
        return rgb
    tief = cv2.distanceTransform(alle.astype(np.uint8), cv2.DIST_L2, 3)
    pos = np.zeros((H, W, 3), np.float32)
    pos[m] = ml.pb.astype(np.float32)
    n1 = vm_mal.rauschen(pos / 28.0, 71)
    n2 = vm_mal.rauschen(pos / 14.0, 73)
    fa = np.zeros((H, W), np.float32)
    fa[m] = ml.faser
    out = rgb.copy()
    frei_ = np.ones((H, W), bool) if ohne is None else ~ohne
    innen = alle & (tief >= 1.5)                 # Fleisch, Tiefen, Glanz auch in Z5 (sonst Einfarbfeld, Lauf D_fleisch v2)
    rand = alle & (tief < 1.5) & frei_           # Z5-Rand bleibt in der Spanne: daran haengen kappe_spitze/kappe_basis
    # Fasern im Fleisch, leise; in Z5 nur abdunkelnd (Gegenprobe N2 unter Z5, Lauf D_fleisch v3/v4)
    z5i = innen & ~frei_
    out[innen & frei_] *= (1 + 0.16 * fa[innen & frei_])[:, None]
    out[z5i] *= (1 - 0.16 * np.abs(fa[z5i]))[:, None]
    # Tiefen
    t = innen & (n1 < -0.35)
    out[t] = out[t] + (np.array(WUNDE_TIEF, np.float32) - out[t]) * 0.65
    # Glanzpunkte oben im Licht
    g = alle & frei_ & (n2 > 0.55) & (ml.lichtk > 0.1)      # kein Glanz in Z5: verletzte die Gegenprobe N2 (Lauf D_fleisch v3)
    out[g] = out[g] + (np.array(WUNDE_GLANZ, np.float32) - out[g]) * 0.7
    # gerissener Rand: heller, mit dunklen Kerben
    kerbe = rand & (n2 < -0.3)
    out[rand & ~kerbe] *= 1.12
    out[kerbe] *= 0.5
    # entzuendeter Ring aussen
    out[ring] = out[ring] + (np.array(WUNDE_ENTZUENDET, np.float32) - out[ring]) * 0.3
    return np.clip(out, 0, 255)


def naht(ml, rgb):
    """Nahtspalte je Streifen: fb_bau.teil_mesh legt U = 1 auf Spalte x + w, eine Spalte rechts neben dem Teilrechteck (keinem Teil zugeordnet).
    Sie zeigt denselben Winkel wie U = 0, darum die Farbe der Spalte x. (D3: dunkle Linie am Schwanz.)"""
    out = rgb.copy()
    for i, p in enumerate(ml.teile):
        x, y, w, h = ml.lage[i]
        c = x + w
        if c >= ml.m.shape[1]:
            continue
        zeilen = np.arange(y, min(y + h, ml.m.shape[0]))
        sel = ml.m[zeilen, c] & (ml.tid[zeilen, c] < 0) & (ml.kn[zeilen, c] == p.knochen)
        out[zeilen[sel], c] = out[zeilen[sel], x]
    return out


def backen(ml, aus, form_json, ausgabe=None, faktoren=None, cache=None, **kw):
    """faktoren: je Marke (rf_marken-Id) ein Helligkeitsfaktor auf ihre Texel (Wunden-Eichung in vm_bau). cache: dict, haelt Licht und Grundtextur
    zwischen den Eichrunden (Licht, Projektion, Ergaenzen und Zusammenfassen haengen nicht von den Faktoren ab)."""
    cache = {} if cache is None else cache
    if 'psx' not in cache:
        licht = licht_bilder(aus, form_json)
        rgb, bekannt, ber = aus_bild(ml, licht)
        voll, hat = ergaenzen(ml, rgb, bekannt)
        ber['ergaenzt'] = int((hat & ml.m & ~bekannt).sum())
        cache.update(psx=psx_form(ml, voll, **kw), ber=ber, roh=rgb, bekannt=bekannt)
    psx = cache['psx'].copy()
    ber = dict(cache['ber'])
    m = ml.m
    # Wunden: Farbton aus dem Bild, Helligkeit je Marke geeicht (die Unschaerfe des Videos nimmt kleinen hellen Stellen die Spitze)
    if faktoren:
        for i, nm, grund, fak, s_, r in ml.marken:
            k = faktoren.get(i.replace('_z5', ''), 1.0)
            if k != 1.0 and s_.any():
                psx[s_] *= k
        ber['marken_faktoren'] = {k: round(float(v), 3) for k, v in faktoren.items()}
    # Z5 traegt die Pruefpunkte b1.kappe_spitze/kappe_basis (Material helle_baender). Diese Spanne ist aus demselben Bildbereich zurueckgerechnet
    # (farbe_palette.json helle_baender, von/bis): Z5 dorthin legen, Helligkeit bleibt; ohne das fielen beide Punkte (Laeufe D_fleisch v1, D5).
    z5 = np.zeros(m.shape, bool)
    for i, nm, grund, fak, s_, r in ml.marken:
        if i == 'Z5' or i.endswith('_z5'):
            z5 |= s_
    if z5.any():
        von = np.array(vm_mal._PAL['helle_baender']['oberflaeche_von'], np.float32) + 2
        bis = np.array(vm_mal._PAL['helle_baender']['oberflaeche_bis'], np.float32) - 2
        q = np.clip(psx[z5], 1, 255)
        y0 = q @ YK
        q = np.clip(q, von, bis)
        q *= np.clip(y0 / np.maximum(q @ YK, 1), 0.9, 1.1)[:, None]
        psx[z5] = np.clip(q, von, bis)
    psx = wunden(ml, np.clip(psx, 0, 255), ohne=z5)
    for s_ in ml.n12:
        Y = psx @ YK
        zu = s_ & (Y > vm_mal.Y_N)
        psx[zu] *= (vm_mal.Y_N / Y[zu])[:, None]
    psx = naht(ml, psx)
    iy, ix = ml.naechst
    psx[~m] = psx[iy[~m], ix[~m]]
    if ausgabe is not None:
        ausgabe.update(dict(roh=cache['roh'], bekannt=cache['bekannt'], bericht=ber))
    return np.clip(np.round(np.nan_to_num(psx)), 0, 255).astype(np.uint8), ber


def wunden_eichen(ml, aus, form_json, schreib, runden=5, **kw):
    """Je Marke: mittleres Y im Bild (Maske aus rf_marken) im Screenshot gegen das Render durch video_filter (ohne Korn); Faktor auf die Markentexel,
    bis beide gleich sind. schreib(rgb) schreibt die TIM. -> rgb, Bericht mit Verlauf."""
    import fb_mal as FM
    import form_video as FV
    cache, fak, verlauf = {}, {}, []
    liste = [(i, n, mk) for i, n, mk, _f in FM.liste()]
    Yo = {n: P.original(n).astype(np.float32) @ YK for n in (1, 2)}
    logo = P.logo_maske()
    for r in range(runden):
        rgb, ber = backen(ml, aus, form_json, faktoren=fak, cache=cache, **kw)
        schreib(rgb)
        k = P.Kandidat(form_json)
        Yr = {}
        for n in (1, 2):
            sauber, _a, _m, _ = k.rendern(n)
            Yr[n] = FV.video_filter(sauber, bild=n, entzerren=False, rauschen=False).astype(np.float32) @ YK
        zeile = {}
        for i, n, mk in liste:
            q = (mk > 0) & ~logo
            if q.sum() < 4:
                continue
            ys, yr = float(Yo[n][q].mean()), float(Yr[n][q].mean())
            zeile[i] = (round(ys), round(yr))
            fak[i] = float(np.clip(fak.get(i, 1.0) * (ys / max(yr, 1.0)) ** 1.3, 0.5, 4.0))
        verlauf.append(zeile)
    rgb, ber = backen(ml, aus, form_json, faktoren=fak, cache=cache, **kw)
    schreib(rgb)
    ber['wunden_y_screenshot_render'] = verlauf[-1]
    return rgb, ber
