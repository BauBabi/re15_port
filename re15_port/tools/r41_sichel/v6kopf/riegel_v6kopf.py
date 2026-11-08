"""SF V6 Kopf (Bauer): Riegel "Kopf aus dem Original RE1.5-Birkin". Exit 1 bei Rot.

    python riegel_v6kopf.py --emd EM30_V5k.EMD [--basis EM30_V5r4.EMD] [--aus DIR --name NAME]

Pruefungen (Auftrag Runde 41 SF, Nutzer 2026-11-08; Posen v6_gemein.posen: P1 = Foto 1, P2 = Foto 2, 135 = Kopf von vorn):
  T-K1  Koerper/Schwanz unveraendert gegen die Basis (V5 Runde 4): Meshes 0-8 / 10-22 und Sektionen 0-6 bytegleich; CLUT-Zeilen 480-485
        bytegleich Referenz 2; Zeilen <= 16, Port-Texturhoehe <= 4096; Texel: jede Abweichung von Referenz 2 liegt im Schwanzfeld
        (TPage 130 x 320-383, gleich der Basis) oder auf Texeln, die NUR Mesh 9 nutzt (Ausnahmeliste: Zahl, Box, Zonen).
  T-H6  Kopfgeometrie = Original: Mesh 9 Normalen, Flaechen, UV-Koordinaten bytegleich EM30 (CDEMD0.EMS Blob 16) Mesh 9; nur das
        CLUT-Feld = Zeile 486 (Spalte wie das Original). Ecken bytegleich bis auf die Glaettung (kopf_orig_bau.glaetten): hoechstens
        4 Ecken, jede auf der Geraden zum Schwerpunkt der Original-Ecken (abseits <= 1,5 E.), hoechstens 10 % des Wegs.
  T-H5  keine neue Farbe: Zeile 486 = Original-Zeile 0 bytegleich; alle Kopftexel (Raster der Mesh-9-Flaechen) tragen nur Indizes, die
        die Original-Kopftexel schon tragen.
  T-H1  Mund geschlossen: Maul = Kern der Zone B (texel_erweitern.PARAM zone_b, 2 Texel eingezogen; bei V5 = Flaechen der Zeile 487).
        (i) Texel: Anteil dunkel (Paletten-L* < 30) <= 2 %, rot (Paletten-a* >= 15) <= 2 %, Median Paletten-L* >= 55 (Hellgrau der
        Mitte: Stirn L* 55-84). (ii) je Pose 135 / P1 / P2 sichtbare Maulpixel: dunkel/rot nach Palette <= 5 %, beleuchtet a* >= 10
        (Videoraum) <= 10 %.
  T-H2  grauer Fleck erweitert: Gesichtshaelfte bis zur Nase = Kopftexel u 58-86, v 186-254 ohne Haar (Original-b* > 10), ohne Auge
        (u 70-88 v 186-200), ohne Zone B. Grau = Fleck-Klasse (Paletten-L* <= 50 und C* <= 20 = Spanne der Original-Fleck-Texel).
        Anteil je Pose (>= 20 Pixel) gegen Foto 1 (Box x 368-395 y 150-191 ohne Augenfleck, entblendet, gleiche Klasse): neu >= Foto - 0,10 und neu >= Original
        + 0,10; dazu der Texel-Anteil (posenunabhaengig).
  T-H4  obere Kontur Ruecken -> Kopf ohne Buckel (Pose P2): von der Buckelkuppe bis zur Kopf-Vorderkante faellt die Kontur zum Kopf hin;
        Wiederanstieg (spaeterer Punkt hoeher als ein frueherer) <= 2 px. INFO: Abstand Modell-Kontur ueber Foto-Kontur (kopf_foto.json).
"""
import os, sys, json, argparse, hashlib
import numpy as np, cv2
HIER = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HIER)
import v6_gemein as V
import kopf_orig_bau as KB
import texel_erweitern as TE
import emd_ansichtsblatt as EA

KOPF = 9
FOTO1_HALB = (368, 150, 395, 191)
FOTO1_AUGE = (373, 162, 381, 172)          # Augenfleck Messblatt 3.3: Schwerpunkt (376,9/167,2), 8 x 10 px
H_BOX = (58, 186, 86, 254)
AUGE_BOX = (70, 186, 88, 200)
AUGE_WEISS = (74, 194, 84, 198)
# Fleck-Klasse = Spanne der Original-Fleck-Texel (Box u 58-74 v 186-226, b* <= 10, L* <= 50: 351 Texel, C* P50/P90/max 7,6 / 12,0 / 19,9)
GRAU_L, GRAU_C = 50.0, 20.0
# Glaettung (kopf_orig_bau.glaetten): hoechstens 4 Ecken, je auf der Geraden zum Schwerpunkt des Original-Meshes, hoechstens 10 % des Wegs
GLATT_N, GLATT_S, GLATT_AB = 4, 0.10, 1.5


def pal_lab_zeilen(mo):
    from skimage.color import rgb2lab
    return rgb2lab(mo.lut[..., :3].astype(float) / 255.0)


def mund_kern(shape):
    m = TE._poly_maske(TE.PARAM['zone_b'], shape).astype(np.uint8)
    return cv2.erode(m, np.ones((5, 5), np.uint8)).astype(bool)


def haelfte(shape, orig_tex, orig_pal_lab):
    H = np.zeros(shape, bool)
    x0, y0, x1, y1 = H_BOX; H[y0:y1 + 1, x0:x1 + 1] = True
    H &= orig_pal_lab[orig_tex][..., 2] <= 10.0
    x0, y0, x1, y1 = AUGE_BOX; H[y0:y1 + 1, x0:x1 + 1] = False
    H &= ~TE._poly_maske(TE.PARAM['zone_b'], shape)
    return H


def pruefen(emd, basis, mit_posen=True):
    blob = open(emd, 'rb').read(); bas = open(basis, 'rb').read(); ref2 = open(KB.REF2, 'rb').read()
    o, m9o, to = KB.original_kopf()
    d, _, ms, tim = KB.teile(blob); db, _, mb, timb = KB.teile(bas); _, _, mr, timr = KB.teile(ref2)
    t, tb, tr = KB.tim_teile(tim), KB.tim_teile(timb), KB.tim_teile(timr)
    R = dict(emd=emd, emd_sha256=hashlib.sha256(blob).hexdigest(), basis=basis, basis_sha256=hashlib.sha256(bas).hexdigest(),
             ref2_sha256=hashlib.sha256(ref2).hexdigest(), original_sha256=hashlib.sha256(o).hexdigest(), zeilen=[])

    def zeile_(name, ok, soll, ist, info=False):
        R['zeilen'].append(dict(name=name, ok=bool(ok), soll=soll, ist=ist, info=info))

    # ---------------- T-K1
    ungleich = [i for i in range(len(mb)) if i != KOPF and (i >= len(ms) or ms[i] != mb[i])]
    zeile_('T-K1a Meshes 0-8/10-22 = Basis', not ungleich and len(ms) == len(mb), 'bytegleich', 'abweichend %s, Meshes %d/%d' % (ungleich, len(ms), len(mb)))
    sek = [i for i in range(7) if blob[d[i]:d[i + 1]] != bas[db[i]:db[i + 1]]]
    zeile_('T-K1b Sektionen 0-6 (EDD, EMR, ...) = Basis', not sek, 'bytegleich', 'abweichend %s' % sek)
    nz = t['clut'].shape[0]
    z_ok = nz >= 6 and np.array_equal(t['clut'][:6], tr['clut'][:6])
    zeile_('T-K1c CLUT 480-485 = Referenz 2; Zeilen <= 16; Hoehe <= 4096', z_ok and nz <= 16 and t['ih'] * nz <= 4096,
           '480-485 bytegleich, <= 16, <= 4096', '%s, %d Zeilen, %d' % ('gleich' if z_ok else 'ABWEICHEND', nz, t['ih'] * nz))
    nutz = KB.texel_nutzung(ms)
    fremd = np.zeros_like(nutz[KOPF])
    for i, mk in nutz.items():
        if i != KOPF:
            fremd |= mk
    diff = t['pix'] != tr['pix']
    schwanz = np.zeros_like(diff); schwanz[:, 320:384] = True
    schwanz_ok = np.array_equal(t['pix'][:, 320:384], tb['pix'][:, 320:384])
    fremd_diff = diff & ~schwanz & fremd
    kopf_diff = diff & ~schwanz & ~fremd
    yk, xk = np.nonzero(kopf_diff)
    R['ausnahmeliste'] = dict(kopf_texel_geaendert=int(kopf_diff.sum()),
                              box=[int(xk.min()), int(yk.min()), int(xk.max()), int(yk.max())] if len(xk) else None)
    zeile_('T-K1d Texel: Koerper = Referenz 2, Abweichung nur Schwanzfeld (= Basis) und Kopf-exklusiv', not fremd_diff.any() and schwanz_ok,
           'Koerpertexel 0 abweichend', 'Koerpertexel abweichend %d; Schwanzfeld = Basis %s; Kopf-exklusiv geaendert %d (Box %s)' % (
               int(fremd_diff.sum()), schwanz_ok, int(kopf_diff.sum()), R['ausnahmeliste']['box']))
    # ---------------- T-H6
    m9 = ms[KOPF]
    geo = dict(n=m9['n'] == m9o['n'], tf=m9['tf'] == m9o['tf'], qf=m9['qf'] == m9o['qf'], ecken_zahl=len(m9['v']) == len(m9o['v']))
    glatt, glatt_ok = [], geo['ecken_zahl']
    if geo['ecken_zahl']:
        Po = np.array([v[:3] for v in m9o['v']], float); c = Po.mean(0)
        for i, (a_, b_) in enumerate(zip(m9['v'], m9o['v'])):
            if list(a_) != list(b_):
                pn_, po_ = np.array(a_[:3], float), Po[i]
                r_ = c - po_; sc = float((pn_ - po_) @ r_ / (r_ @ r_)); ab = float(np.linalg.norm(pn_ - (po_ + sc * r_)))
                ok_ = a_[3] == b_[3] and 0 < sc <= GLATT_S and ab <= GLATT_AB
                glatt.append(dict(ecke=i, alt=list(b_[:3]), neu=list(a_[:3]), zum_schwerpunkt=round(sc, 4), abseits=round(ab, 2), ok=ok_))
                glatt_ok &= ok_
        glatt_ok &= len(glatt) <= GLATT_N
    R['glaettung'] = glatt
    uv_ok = len(m9['tuv']) == len(m9o['tuv']) and len(m9['quv']) == len(m9o['quv'])
    uv_abw = clut_abw = 0
    if uv_ok:
        for a, b in zip(m9['tuv'] + m9['quv'], m9o['tuv'] + m9o['quv']):
            if [x for k, x in enumerate(a) if k != 2] != [x for k, x in enumerate(b) if k != 2]:
                uv_abw += 1
            if a[2] >> 6 != KB.ZEILE_KOPF or (a[2] & 0x3F) != (b[2] & 0x3F):
                clut_abw += 1
    ok6 = all(geo.values()) and glatt_ok and uv_ok and uv_abw == 0 and clut_abw == 0
    zeile_('T-H6 Kopfgeometrie = Original (EM30 Mesh 9) bis auf die gelistete Glaettung', ok6,
           'Normalen/Flaechen/UV bytegleich, CLUT-Feld = 486; Ecken gleich ausser <= %d Glaettungsecken (zum Schwerpunkt <= %.2f, abseits <= %.1f E.)' % (
               GLATT_N, GLATT_S, GLATT_AB),
           'Ecken %d/%d, Normalen %d/%d, Dreiecke %d/%d, Vierecke %d/%d; gleich %s; Glaettung %s; UV abweichend %s; CLUT-Feld abweichend %s' % (
               len(m9['v']), len(m9o['v']), len(m9['n']), len(m9o['n']), len(m9['tf']), len(m9o['tf']), len(m9['qf']), len(m9o['qf']),
               {k: bool(v) for k, v in geo.items()}, ['%d: %s -> %s (%.3f, %.1f E.)' % (g['ecke'], g['alt'], g['neu'], g['zum_schwerpunkt'], g['abseits']) for g in glatt][:6]
               if len(glatt) <= 6 else '%d Ecken' % len(glatt), uv_abw if uv_ok else 'Anzahl', clut_abw if uv_ok else 'Anzahl'))
    # ---------------- T-H5
    zeilen_kopf = sorted({uv[2] >> 6 for uv in m9['tuv'] + m9['quv']})
    z486 = nz > 6 and np.array_equal(t['clut'][6], to['clut'][0])
    mk9o = KB.texel_nutzung([m9o])[0]
    idx_o = set(int(i) for i in to['pix'][mk9o])
    idx_n = set(int(i) for i in t['pix'][nutz[KOPF]])
    neu_idx = sorted(idx_n - idx_o)
    ok5 = zeilen_kopf == [KB.ZEILE_KOPF] and z486 and not neu_idx
    zeile_('T-H5 keine neue Farbe', ok5, 'Kopf nur Zeile 486 = Original-Zeile 0; Indizes aus den Original-Kopftexeln',
           'Zeilen %s; 486 = Original-Zeile 0: %s; Kopftexel %d, Indizes %d, nicht im Original-Kopf %s' % (
               zeilen_kopf, z486, int(nutz[KOPF].sum()), len(idx_n), neu_idx[:20]))
    # ---------------- T-H1 Texel
    mo = V.modell(blob)
    pl = pal_lab_zeilen(mo)
    if 487 in zeilen_kopf:                        # V5: Maul = Flaechen der Zeile 487 (G3-Zahnkachel)
        mk = np.zeros(t['pix'].shape, bool)
        for uv in m9['tuv']:
            if uv[2] >> 6 == 487:
                mk |= KB.raster(uv, 3, (uv[5] & 0xF) * 128)
        for uv in m9['quv']:
            if uv[2] >> 6 == 487:
                mk |= KB.raster(uv, 4, (uv[5] & 0xF) * 128)
        maul_tex, maul_zeile = mk, 487
    else:
        maul_tex, maul_zeile = mund_kern(t['pix'].shape) & nutz[KOPF], 486
    lm = pl[maul_zeile - 480][t['pix'][maul_tex]] if maul_zeile - 480 < pl.shape[0] else np.zeros((0, 3))
    dk, rt, l50 = float((lm[:, 0] < 30).mean()), float((lm[:, 1] >= 15).mean()), float(np.median(lm[:, 0]))
    zeile_('T-H1i Mund geschlossen (Texel)', dk <= 0.02 and rt <= 0.02 and l50 >= 55,
           'dunkel (L* < 30) <= 2 %, rot (a* >= 15) <= 2 %, Median L* >= 55',
           'Maultexel %d (Zeile %d): dunkel %.1f %%, rot %.1f %%, Median L* %.1f' % (len(lm), maul_zeile, 100 * dk, 100 * rt, l50))
    R['posen'] = {}
    if mit_posen:
        # Original-Kopf (Stufe 0) zum Vergleich fuer T-H2
        orig_blob, _ = KB.bauen(0, basis)
        mo0 = V.modell(orig_blob)
        H = haelfte(t['pix'].shape, to['pix'], pl[6] if pl.shape[0] > 6 else pl[0])
        def fleck_klasse(lab_):
            return (lab_[..., 0] <= GRAU_L) & (np.hypot(lab_[..., 1], lab_[..., 2]) <= GRAU_C)
        # Texel-Anteil (posenunabhaengig)
        if pl.shape[0] > 6 and zeilen_kopf == [KB.ZEILE_KOPF]:
            R['th2_texel'] = dict(neu=float(fleck_klasse(pl[6][t['pix'][H & nutz[KOPF]]]).mean()),
                                  original=float(fleck_klasse(pl[6][to['pix'][H & nutz[KOPF]]]).mean()), texel=int((H & nutz[KOPF]).sum()))
        # Foto 1
        f1 = V.G.lab(V.G.vorlage(1))
        x0, y0, x1, y1 = FOTO1_HALB
        fm = np.zeros(f1.shape[:2], bool); fm[y0:y1 + 1, x0:x1 + 1] = True
        ax0, ay0, ax1, ay1 = FOTO1_AUGE; fm[ay0:ay1 + 1, ax0:ax1 + 1] = False
        fl = f1[fm]
        foto_anteil = float(((fl[:, 0] <= GRAU_L) & (np.hypot(fl[:, 1], fl[:, 2]) <= GRAU_C)).mean())
        R['foto1_haelfte'] = dict(box=FOTO1_HALB, ohne_auge=FOTO1_AUGE, px=int(fm.sum()), grau=foto_anteil,
                                  lab_p50=[round(float(v), 1) for v in np.median(fl, 0)])
        th1_ok, th2_ok, th2_n = True, True, 0
        for pn, pose in V.posen().items():
            img, ids = V.render(mo, pose)
            labv = V.G.lab(V.G.videoraum(img))
            U, Vv, Z = V.uv_karte(mo, pose)
            kopf = ids == 9
            Uc, Vc = np.clip(U, 0, 383), np.clip(Vv, 0, 255)
            pal_px = pl[np.clip(Z - 480, 0, pl.shape[0] - 1), t['pix'][Vc, Uc]]
            if maul_zeile == 487:
                maul = kopf & (Z == 487)
            else:
                maul = kopf & (Z == 486) & maul_tex[Vc, Uc]
            e = dict(kopf_px=int(kopf.sum()), maul_px=int(maul.sum()))
            if maul.any():
                pm = pal_px[maul]; lv = labv[maul]
                e.update(maul_pal_dunkel=float((pm[:, 0] < 30).mean()), maul_pal_rot=float((pm[:, 1] >= 15).mean()),
                         maul_licht_rot=float((lv[:, 1] >= 10).mean()), maul_licht_lab_p50=[round(float(v), 1) for v in np.median(lv, 0)])
                ok = e['maul_pal_dunkel'] <= 0.05 and e['maul_pal_rot'] <= 0.05 and e['maul_licht_rot'] <= 0.10
                e['th1_ok'] = ok; th1_ok &= ok
            if zeilen_kopf == [KB.ZEILE_KOPF]:
                hm = kopf & (Z == 486) & H[Vc, Uc]
                e['haelfte_px'] = int(hm.sum())
                if hm.any():
                    U0, V0, Z0 = V.uv_karte(mo0, pose)
                    pl0 = pal_lab_zeilen(mo0)
                    hm0 = (Z0 == 486) & H[np.clip(V0, 0, 255), np.clip(U0, 0, 383)]
                    a_neu = float(fleck_klasse(pal_px[hm]).mean())
                    a_alt = float(fleck_klasse(pl0[6][mo0.tex[np.clip(V0, 0, 255), np.clip(U0, 0, 383)]][hm0]).mean()) if hm0.any() else 0.0
                    e.update(haelfte_grau_neu=a_neu, haelfte_grau_original=a_alt,
                             haelfte_licht_lab_p50=[round(float(v), 1) for v in np.median(labv[hm], 0)])
                    if hm.sum() >= 20:
                        ok = a_neu >= foto_anteil - 0.10 and a_neu >= a_alt + 0.10
                        e['th2_ok'] = ok; th2_ok &= ok; th2_n += 1
            if zeilen_kopf == [KB.ZEILE_KOPF]:
                # (d) Auge (W)-rechts = Texturbild links: Augenweiss u 74-84 v 194-198 (Original L* 60-64), nur INFO
                am = kopf & (Z == 486) & (U >= AUGE_WEISS[0]) & (U <= AUGE_WEISS[2]) & (Vv >= AUGE_WEISS[1]) & (Vv <= AUGE_WEISS[3])
                e['auge_px'] = int(am.sum())
                if am.any():
                    ys_, xs_ = np.nonzero(am)
                    e['auge'] = dict(mitte=[round(float(xs_.mean()), 1), round(float(ys_.mean()), 1)],
                                     licht_lab_p50=[round(float(v), 1) for v in np.median(labv[am], 0)],
                                     licht_l_max=round(float(labv[am][:, 0].max()), 1))
            if pn == 'P2':
                e['kontur'] = V.kontur(ids)
            R['posen'][pn] = e
        zeile_('T-H1ii Mund geschlossen (sichtbar, Posen 135/P1/P2)', th1_ok, 'dunkel/rot nach Palette <= 5 %, beleuchtet a* >= 10 <= 10 %',
               '; '.join('%s: %d px, dunkel %.0f %%, rot %.0f %%, Licht-rot %.0f %%, Lab %s' % (
                   pn, e['maul_px'], 100 * e.get('maul_pal_dunkel', 0), 100 * e.get('maul_pal_rot', 0), 100 * e.get('maul_licht_rot', 0),
                   e.get('maul_licht_lab_p50')) for pn, e in R['posen'].items()))
        if zeilen_kopf == [KB.ZEILE_KOPF]:
            zeile_('T-H2 grauer Fleck erweitert (Gesichtshaelfte bis zur Nase)', th2_ok and th2_n > 0,
                   'je Pose (>= 20 px): neu >= Foto 1 - 0,10 und >= Original + 0,10',
                   'Foto 1 %.2f; ' % foto_anteil + '; '.join('%s: %d px neu %.2f / Original %.2f' % (
                       pn, e.get('haelfte_px', 0), e.get('haelfte_grau_neu', 0), e.get('haelfte_grau_original', 0))
                       for pn, e in R['posen'].items()) + ('; Texel %d: neu %.2f / Original %.2f' % (
                           R['th2_texel']['texel'], R['th2_texel']['neu'], R['th2_texel']['original']) if 'th2_texel' in R else ''))
        else:
            zeile_('T-H2 grauer Fleck erweitert', False, 'Kopf = Original-Kopf (Zeile 486)', 'Kopf nicht der Original-Kopf (Zeilen %s)' % zeilen_kopf)
        zeile_('INFO (d) Auge (W)-rechts = Texturbild links (Augenweiss u 74-84 v 194-198), unveraendert', True,
               'Foto 1 Augenfleck (376,9/167,2) L* 47,0; Foto 2 (452,5/237,2) L* 48,5 (Messblatt 3.3)',
               '; '.join('%s: %d px%s' % (pn, e.get('auge_px', 0), (' bei %s, Lab %s, L* max %.1f' % (e['auge']['mitte'], e['auge']['licht_lab_p50'],
                                                                                                  e['auge']['licht_l_max'])) if 'auge' in e else '')
                         for pn, e in R['posen'].items()), info=True)
        k = R['posen']['P2']['kontur']
        zeile_('T-H4 obere Kontur Ruecken -> Kopf ohne Buckel (P2)', k['wiederanstieg'] <= 2, 'Wiederanstieg <= 2 px',
               'Kuppe x %d y %d, Kopf-Vorderkante x %d; Wiederanstieg %d px (x %d y %d -> x %d y %d)' % (
                   k['kuppe'][0], k['kuppe'][1], k['x_vorn'], k['wiederanstieg'], *k['stelle']))
        zeile_('INFO T-H4 Modell-Kontur ueber Foto-2-Kontur', True, '-', 'max %d px bei x %d (Knochen %d); Kopf-Oberkante Modell y %d / Foto y %s bei x %d' % (
            k['ueber_foto'][0], k['ueber_foto'][1], k['ueber_foto'][2], k['kopf_oben'][1], k['kopf_oben'][2], k['kopf_oben'][0]), info=True)
    R['rot'] = [z['name'] for z in R['zeilen'] if not z['ok'] and not z['info']]
    return R


def bericht(R):
    z = ['Riegel V6 Kopf aus dem Original RE1.5-Birkin (riegel_v6kopf.py)', 'EMD %s sha256 %s' % (R['emd'], R['emd_sha256']),
         'Basis %s sha256 %s; Referenz 2 %s; Original EM30 %s' % (R['basis'], R['basis_sha256'][:16], R['ref2_sha256'][:16], R['original_sha256'][:16]), '']
    for e in R['zeilen']:
        z.append('%-5s %s | Soll: %s | Ist: %s' % ('INFO' if e['info'] else ('OK' if e['ok'] else 'ROT'), e['name'], e['soll'], e['ist']))
    z.append('')
    z.append('Ergebnis: %d rot von %d (%s)' % (len(R['rot']), sum(1 for e in R['zeilen'] if not e['info']), ', '.join(R['rot']) or '-'))
    return '\n'.join(z)


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--emd', required=True); ap.add_argument('--basis', default=KB.BASIS)
    ap.add_argument('--aus'); ap.add_argument('--name', default='riegel_v6kopf')
    a = ap.parse_args()
    R = pruefen(a.emd, a.basis)
    txt = bericht(R)
    print(txt)
    if a.aus:
        os.makedirs(a.aus, exist_ok=True)
        open(os.path.join(a.aus, a.name + '.txt'), 'w', encoding='utf-8').write(txt + '\n')
        json.dump(R, open(os.path.join(a.aus, a.name + '.json'), 'w'), indent=1, default=lambda o: o.tolist() if hasattr(o, 'tolist') else str(o))
    sys.exit(1 if R['rot'] else 0)


if __name__ == '__main__':
    main()
