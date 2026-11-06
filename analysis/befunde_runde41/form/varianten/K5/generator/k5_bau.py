#!/usr/bin/env python3
"""Variante K5, Bau einer Variante (offline, NICHT ins Spiel-Asset): J (FORM.EMD/TIM, Textur H2) + K5-Kopf/Kiefer (k5_kopf) + Krallen wie K4
(k_bau.krallen) + Gerippe/Posen aus dem Fit (k5_fit: Halslaenge, Kiefergelenk, Clip 1 Hals/Kopf/Kiefer/Arme/Rumpf, Clip 0 Kopf) + Fusspose Bild 1
(k5_fuss, AE9/R9) + Gelenkkoerper mit den Faktoren von K4 (K4_kroko/naht.json) + Textur Seite 3 wie K4 (Farben von K4, Kiefer-Eichung) mit
dunkel rotvioletten Augen (AE10) + Fusstextur ohne tuerkise Bodenfarben.

    python k5_bau.py ../_lauf/K5a/fit.json ../K5a [--fuss ../_lauf/fuss.json] [--ohne-pruefung]

Kein RE-Wert: Gestaltung eines neuen Modells (PORT-WAHL)."""
import argparse
import copy
import json
import os
import subprocess
import sys

import cv2
import numpy as np

import k5_kern as C
import k5_kopf as K5

KK, FE = C.KK, C.FE
import k_bau as KB  # noqa: E402  (K_alligator/generator)
import k_naht as KN  # noqa: E402

K4_DIR = os.path.join(KK.K_DIR, 'K4_kroko')
YK = np.array([.299, .587, .114])
AUGE_ZIEL = np.array([82.0, 36.0, 52.0])      # FORMBLATT_V2 1.2 Nr 17: (280,275) RGB (82,36,52), Y 47 - dunkles Rotviolett, kein Leuchtrot
FUSS_RECTS = {'fuss_r': 'unterschenkel_r', 'fuss_l': 'unterschenkel_l', 'zeh0_r': 'unterschenkel_r'}


def form_mit_kopf(fit, fuss=None):
    """form_emd-Struktur von J mit K5-Kopf, Hals, Krallen, Posen. Rueckgabe (form, p, quelle vor den Gelenkkoerpern)."""
    form, mo, fj = KK.lade()
    p = dict(K5.VORGABE)
    p.update({k: v for k, v in fit['kopf'].items() if k in K5.VORGABE})
    KB.neu_netz(form, 22, K5.kopf(p))
    KB.neu_netz(form, 23, K5.kiefer(p))
    hl = float(fit['halslaenge'])
    if abs(hl - 503.0) > 0.5:
        m = form['meshes'][21]
        V = np.array(m['ecken'], float)
        V[:, 0] = np.where(V[:, 0] > 0, V[:, 0] * hl / 503.0, V[:, 0])
        m['ecken'] = [[int(round(x)) for x in q] for q in V]
    form['knochen'][22]['versatz'] = [int(round(hl)), 0, 0]
    form['knochen'][23]['versatz'] = [int(round(p['gx'])), int(round(p['gy'])), 0]
    kb = KB.krallen(form)
    pool = form['pool']
    for clip, key in ((1, 'bild2'), (0, 'bild1')):
        kf = form['clips'][clip]['bilder'][0]['kf']
        for b, w in fit[key]['winkel'].items():
            pool[kf]['winkel'][int(b)] = [int(round(x)) for x in w]
    if fuss:
        kf = form['clips'][0]['bilder'][0]['kf']
        for b, w in fuss['winkel_bild1'].items():
            pool[kf]['winkel'][int(b)] = [int(x) for x in w]
    kf = form['clips'][2]['bilder'][0]['kf']
    pool[kf]['winkel'][22] = list(KB.RUHE_KOPF)
    pool[kf]['winkel'][23] = list(KB.RUHE_KIEFER)
    return form, p, kb


# Gelenkkoerper am Kopf: K4 hatte Kugel/Muffe Kopf - Hals mit Radius 426 (Schaedel 764 breit) und Mundwinkel 268/335 (K4_kroko/bau.json naht).
# Der K5-Schaedel ist 1,6-2,6-mal so breit; mit den Faktoren von K4 waere die Muffe 1100 lang und ragte oben aus dem Schaedel (Probebau v1-K5a:
# Ecken bei y -774 ueber dem Schaedeldach -339). Darum Kopfkoerper auf die absoluten Masse von K4 begrenzt.
KOPF_KOERPER_MAX = {'Kopf - Hals': (426.0, 426.0), 'Muffe Kopf - Hals': (426.0, 681.0), 'Kiefer - Kopf': (268.0, 335.0)}


def naht_k4(quelle=None):
    g = json.load(open(os.path.join(K4_DIR, 'naht.json'), encoding='utf-8'))['gelenke']
    out = {k: tuple(v) if v[3] is None else (v[0], v[1], v[2], tuple(v[3])) for k, v in g.items()}
    if quelle is not None:
        for nm, (q_max, n_max) in KOPF_KOERPER_MAX.items():
            c, fq, fn, ax = out[nm]
            _a, r, _n = KN.gelenk_mass(quelle, c, ax)
            out[nm] = (c, min(fq, q_max / r), min(fn, n_max / r), ax)
    return out


def ohne_augen(form):
    """Kopie, in der Knochen 22 keine Augen-Flaechen hat: die Gelenkkoerper nehmen UV von der naechsten Flaeche des Eltern/Kinds; Augenfelder
    gaeben ihnen die Augenfarbe (Probebau v1-K5a: zwei Muffen-Vierecke mit Augen-UV)."""
    q = copy.deepcopy(form)
    ax, ay, aw, ah = K5.ATLAS['auge']
    for art in ('dreiecke', 'vierecke'):
        q['meshes'][22][art] = [f for f in q['meshes'][22][art]
                                if not ('tex' in f and all(ax <= u <= ax + aw and ay <= v <= ay + ah for u, v in f['tex']))]
    return q


def fuss_textur(px, tx, bau):
    """Fuss-/Zehenfelder (CLUT-Zeile 2) ohne die tuerkisen Bodenfarben: Farbton und mittlere Helligkeit des Unterschenkels derselben Seite,
    Helligkeitsstruktur des Fusses bleibt; Index = naechste Farbe der unveraenderten Zeile 2 (andere Teile unberuehrt)."""
    A = {a['name']: a for a in bau['atlas']}
    lut = tx.lut[2][:, :3].astype(np.float32)
    ber = {}
    for fn, sn in FUSS_RECTS.items():
        x, y, w, h = A[fn]['rechteck']
        sx, sy, sw, sh = A[sn]['rechteck']
        s = tx.lut[A[sn]['zeile']][px[sy:sy + sh, sx:sx + sw]][..., :3].astype(np.float32).reshape(-1, 3)
        ziel_mittel = s.mean(0)
        f = lut[px[y:y + h, x:x + w]]
        Y = f @ YK
        k = (Y / max(float(Y.mean()), 1.0))[..., None]
        k = 1.0 + 0.6 * (k - 1.0)                         # Struktur gedaempft (Fuss lag im Bodenlicht, Kontrast hoeher als am Bein)
        neu = np.clip(k * ziel_mittel, 0, 255)
        d = ((neu.reshape(-1, 1, 3) - lut[None, 1:]) ** 2).sum(2)
        idx = d.argmin(1) + 1
        px[y:y + h, x:x + w] = idx.reshape(h, w).astype(np.uint8)
        ber[fn] = dict(vorher=[round(float(v)) for v in f.reshape(-1, 3).mean(0)], nachher=[round(float(v)) for v in lut[idx].mean(0)],
                       ziel=[round(float(v)) for v in ziel_mittel])
    return ber


def tim_k5(seite3, auge_rgb):
    """TIM: Seiten 0-2 von J mit Fusstextur ohne Bodenfarben, Seite 3 neu (Kopf/Kiefer/Zaehne/Krallen wie K4, Augenfeld in Augenfarbe)."""
    j_tim = open(os.path.join(KK.J_DIR, 'FORM.TIM'), 'rb').read()
    import form_buehne as FB
    tx = FB.Textur(j_tim)
    bau = json.load(open(os.path.join(KK.J_DIR, 'bau.json')))
    t = FE.tim_lesen(j_tim)
    px = np.zeros((256, 512), np.uint8)
    px[:, :384] = t['pixel']
    fb = fuss_textur(px, tx, bau)
    S = seite3.copy()
    ax, ay, aw, ah = K5.ATLAS['auge']
    ax -= 384
    st = S[ay:ay + ah, ax:ax + aw]
    Y = st @ YK
    k = (Y / max(float(Y.mean()), 1.0))[..., None]
    S[ay:ay + ah, ax:ax + aw] = np.clip((1.0 + 0.25 * (k - 1.0)) * np.asarray(auge_rgb, np.float32), 8, 255)      # kaum Struktur, kein Glanz
    zent, lab = KB.kmeans255(S)
    px[:, 384:] = (lab + 1).reshape(256, 128).astype(np.uint8)
    cl = [list(c) for c in t['cluts']] + [FE.clut_aus_rgb([(0, 0, 0)] + [tuple(int(c) << 3 for c in f) for f in zent], transparent0=True)]
    return FE.tim_schreiben(px, cl), fb, S


def schreiben(form, aus, name, p, fit, seite3, auge_rgb, meta):
    os.makedirs(aus, exist_ok=True)
    open(os.path.join(aus, 'FORM.EMD'), 'wb').write(FE.schreibe_emd(form))
    tim, fb, S = tim_k5(seite3, auge_rgb)
    open(os.path.join(aus, 'FORM.TIM'), 'wb').write(tim)
    cv2.imwrite(os.path.join(aus, 'seite3.png'), S[:, :, ::-1].astype(np.uint8))
    fj = json.load(open(os.path.join(KK.J_DIR, 'form.json'), encoding='utf-8'))
    fj['name'] = name
    json.dump(fj, open(os.path.join(aus, 'form.json'), 'w', encoding='utf-8'), indent=1)
    lm = K5.landmarken(p)
    json.dump(dict(kopf=p, landmarken={str(b): d for b, d in lm.items()}, halslaenge=fit['halslaenge'], f4=fit['f4'], f5=fit['f5'],
                   schnauze_e=round(K5.schnauze_e(p), 3), winkel=dict(bild2=fit['bild2']['winkel'], bild1=fit['bild1']['winkel'])),
              open(os.path.join(aus, 'kopf.json'), 'w', encoding='utf-8'), indent=1)
    meta['fuss_textur'] = fb
    json.dump(meta, open(os.path.join(aus, 'bau.json'), 'w', encoding='utf-8'), indent=1, default=str)


def auge_messen(aus):
    """Mittlere Farbe der sichtbaren Augenpixel im sauberen Render Bild 2 (form_pruefung.Kandidat, Licht wie die Pruefung)."""
    import form_pruefung as P
    import k5_mass as M
    k = P.Kandidat(os.path.join(aus, 'form.json'))
    sauber, _alb, _m, _ = k.rendern(2)
    idb = M.kennung(aus, 2)
    z = np.isin(idb, (C.AUGE_L, C.AUGE_R))
    if z.sum() < 3:
        return None, int(z.sum())
    # Vergleich mit dem Lichtpunkt des Originals (+30 ueber dem Umfeld, FORMBLATT_V2 1.2 Nr 1/17): hellstes Drittel der Augenpixel, nicht das Mittel
    # (Probebau K5a: Mittel ueber den ganzen Augapfel inkl. Schattenseite trieb die Textur bis an die Grenze 255/173/255)
    px = sauber[z].reshape(-1, 3).astype(np.float32)
    y = px @ YK
    return px[y >= np.percentile(y, 67)].mean(0), int(z.sum())


def bauen(fit_json, aus, fuss_json=None, pruefen=True):
    fit = json.load(open(fit_json, encoding='utf-8'))
    name = os.path.basename(os.path.normpath(aus))
    fuss = json.load(open(fuss_json, encoding='utf-8')) if fuss_json else None
    form, p, kb = form_mit_kopf(fit, fuss)
    quelle = ohne_augen(form)
    form, nb = KN.schliessen(form, naht_k4(quelle), quelle=quelle)
    FE.orientiere(form)
    # Seite 3 wie K4: Farben (Lichtmessung/Kiefer-Eichung von K4) als Start
    k4 = json.load(open(os.path.join(K4_DIR, 'bau.json'), encoding='utf-8'))
    fb = {k: v for k, v in k4['farben'].items() if isinstance(v, list)}
    import form_buehne as FB
    tx = FB.Textur(open(os.path.join(KK.J_DIR, 'FORM.TIM'), 'rb').read())
    bau = json.load(open(os.path.join(KK.J_DIR, 'bau.json')))
    hand = np.mean([KB._rgb_teil(tx, a['rechteck'], a['zeile']).reshape(-1, 3).mean(0) for a in bau['atlas'] if a['name'] in ('hand_r', 'hand_l')], 0)
    KB.FLEISCH_STRUKTUR, KB.FLECKEN = 0.4, False
    auge = AUGE_ZIEL.copy()
    meta = dict(name=name, basis='J_dicker + K5-Kopf', fit=fit_json, kopf=p, halslaenge=fit['halslaenge'], krallen=kb, naht=nb, farben=fb,
                flaechen=sum(len(m['dreiecke']) + len(m['vierecke']) for m in form['meshes']))
    eich = []
    for runde in range(5):
        S = KB.malen(tx, bau, fb, hand)
        schreiben(form, aus, name, p, fit, S, auge, meta)
        q, n = KB.kiefer_eichen(aus)
        am, an = auge_messen(aus)
        eich.append(dict(runde=runde, kiefer_soll_durch_ist=round(q, 3), kiefer_px=n, auge_render=None if am is None else [round(float(v)) for v in am],
                         auge_px=an, auge_textur=[round(float(v)) for v in auge]))
        print('  Eichung %d: Kiefer Soll/Ist %.3f (%d px), Auge %s (%d px)' % (runde, q, n, None if am is None else np.round(am).tolist(), an), flush=True)
        fertig = abs(q - 1) < 0.03 and (am is None or np.abs(am - AUGE_ZIEL).max() < 8)
        if fertig:
            break
        for k in ('fleisch', 'unterseite', 'hell'):
            fb[k] = [round(float(v * q), 3) for v in fb[k]]
        if am is not None:
            auge = np.clip(auge * AUGE_ZIEL / np.maximum(am, 4.0), 8, 200)
    meta['eichung'] = eich
    S = KB.malen(tx, bau, fb, hand)
    schreiben(form, aus, name, p, fit, S, auge, meta)
    print('%s: %d Flaechen' % (name, meta['flaechen']))
    if pruefen:
        env = dict(os.environ, PYTHONIOENCODING='utf-8')
        r = subprocess.run([sys.executable, os.path.join(KK.TOOLS, 'form_pruefung.py'), 'pruefe', os.path.join(aus, 'form.json'), '--aus',
                            os.path.join(aus, 'pruefung')], capture_output=True, text=True, env=env, encoding='utf-8', errors='replace')
        if r.returncode:
            print(r.stdout[-2000:], r.stderr[-2000:])
    return meta


if __name__ == '__main__':
    a = argparse.ArgumentParser()
    a.add_argument('fit')
    a.add_argument('aus')
    a.add_argument('--fuss', default=None)
    a.add_argument('--ohne-pruefung', action='store_true')
    x = a.parse_args()
    bauen(x.fit, x.aus, x.fuss, not x.ohne_pruefung)
