#!/usr/bin/env python3
"""Variante K (E20), Bau: J (FORM.EMD/TIM, Textur H2) + Alligator-Kopf/Kiefer (k_form, Pose aus k_kopf_fit) + Krallen (Farbe/Haken) + Gelenkkugeln (k_naht).

    python k_bau.py --kopf ../_lauf/K2/kopf.json --aus ../K2_breit --name K2_breit [--naht abgestimmt.json] [--ohne-pruefung]

Textur: Seiten 0-2 (TIM-x 0-383) und CLUT-Zeilen 0-2 bytegleich wie J; neue Seite 3 (TIM-x 384-511) mit eigener CLUT-Zeile 3 fuer Kopf, Kiefer,
Zaehne, Krallen. Farben der neuen Stellen aus Screenshot 2 (farbe_bild2_echt.png, farbe.md) durch das Port-Licht an derselben Stelle geteilt und mit
demselben Faktor wie der Rumpf von J auf Texturwerte gebracht (zwei Durchgaenge: erst Licht messen, dann malen). Stoff = H2-Haut von J (Brust),
nur umgefaerbt. Kein RE-Wert."""
import argparse
import copy
import json
import math
import os
import subprocess
import sys

import cv2
import numpy as np

import k_kern as KK
import k_form as KF
import k_naht as KN

FE = KK.FE
YK = np.array([.299, .587, .114])
KRALLEN = {14: 'klaue_l1', 15: 'klaue_l2', 28: 'klaue_l3', 30: 'klaue_l4', 19: 'klaue_r1', 20: 'klaue_r2', 29: 'klaue_r3', 31: 'klaue_r4'}
KRALLE_HAND = {13: (246, 222, 30, 17), 18: (146, 222, 38, 21)}      # Atlas-Rechteck der fuenften Kralle in J (bau.json kralle_l / kralle_r)
KRALLE_AB = 0.55          # Anteil der Fingerlaenge, ab dem die Kralle beginnt (rf_plan.finger: Ring L*0,52 ist der letzte volle Fingerring)
ZAEHNE = 7               # je Reihe: 6 helle Flecken auf 109 px Band, Abstand 16 px (K.md Abschnitt 1), auf die volle Kieferlaenge 7
HAKEN = 0.0               # keine Hakenkruemmung: im Bild nicht messbar (K.md Abschnitt 2); mit 0,18 lagen die Spitzen 2 px weiter von den Handschnitten (14,6 gegen 12,7 px)
FLACH = 1.0               # Querschnitt wie J: Breite der Krallen im Bild 10-32 px, keine Aussage zur Abflachung


def _tex_aus_uv(f):
    pg = f['page'] & 0xF
    return [[u + pg * 128, v] for u, v in f['uv']]


def neu_netz(form, b, netz, zeile=KF.ZEILE_NEU):
    form['meshes'][b] = dict(ecken=[], normalen=[], dreiecke=[], vierecke=[])
    P, F, T, N = netz.arrays()
    FE.netz(form, b, P, F, T, zeile, normalen=N * 4096)


def krallen(form, kralle_rect=KF.ATLAS['kralle']):
    """Fingerspitzen als Krallen: Ecken jenseits KRALLE_AB der Fingerlaenge werden seitlich flacher und zum Boden gekruemmt, ihre Flaechen
    bekommen UV im Krallenfeld der Seite 3 (v = Lage zwischen Krallenwurzel und Spitze). Fingeransatz bleibt wie J."""
    x0, y0, w, h = kralle_rect
    ber = {}
    ziele = [(b, None) for b in KRALLEN] + [(b, r) for b, r in KRALLE_HAND.items()]
    for b, rect in ziele:
        m = form['meshes'][b]
        V = np.array(m['ecken'], float)
        if rect is None:
            idx = np.arange(len(V))
            flaechen = m['dreiecke'] + m['vierecke']
        else:
            flaechen = [f for f in m['dreiecke'] + m['vierecke'] if (f['clut'] >> 6) - 480 == 1 and
                        rect[0] <= min(_tex_aus_uv(f), key=lambda t: t[0])[0] < rect[0] + rect[2] and rect[1] <= min(t[1] for t in _tex_aus_uv(f)) < rect[1] + rect[3]]
            idx = np.array(sorted(set(i for f in flaechen for i in f['e'])), int)
        Q = V[idx]
        c = Q.mean(0)
        u, s_, vt = np.linalg.svd(Q - c)
        ax = vt[0]
        t = (Q - c) @ ax
        if abs(t.max()) < abs(t.min()):
            ax, t = -ax, -t
        # Wurzel = Ende zum Knochenursprung (Finger) bzw. zur Hand (Kralle an der Hand)
        t0, t1 = t.min(), t.max()
        tk = t0 + KRALLE_AB * (t1 - t0)
        quer = np.cross(ax, [0, 1.0, 0])
        quer /= max(np.linalg.norm(quer), 1e-9)
        neu = V.copy()
        for k, i in enumerate(idx):
            if t[k] > tk:
                a = (t[k] - tk) / (t1 - tk)
                p = V[i] - c - t[k] * ax
                z = p @ quer
                p = p - (1 - FLACH) * z * quer
                neu[i] = c + t[k] * ax + p + np.array([0, 1.0, 0]) * HAKEN * (t1 - tk) * a * a
        m['ecken'] = [[int(round(x)) for x in q] for q in neu]
        lage = {i: (t[k] - tk) / (t1 - tk) for k, i in enumerate(idx)}
        winkel = {i: (math.atan2((V[i] - c) @ np.cross(ax, quer), (V[i] - c) @ quer) / (2 * math.pi)) % 1.0 for i in idx}
        n = 0
        for f in flaechen:
            a = [lage.get(i, 0.0) for i in f['e']]
            if max(a) <= 0:
                continue
            wu = [winkel[i] for i in f['e']]
            if max(wu) - min(wu) > 0.5:
                wu = [x + 1.0 if x < 0.5 else x for x in wu]
            tex = [[int(round(x0 + 1 + (w - 3) * (min(x, 1.999) / 2.0))), int(round(y0 + 1 + (h - 3) * min(max(v, 0), 1)))] for x, v in zip(wu, a)]
            f.pop('uv', None)
            f.pop('clut', None)
            f.pop('page', None)
            f['tex'] = tex
            f['zeile'] = KF.ZEILE_NEU
            n += 1
        ber[b] = dict(flaechen_kralle=n, laenge_kralle=round(float(t1 - tk)), haken=round(HAKEN * float(t1 - tk)))
    return ber


RUHE_KOPF = [0, 0, 150]       # Ruhe (Clip 2): Kopf gerade aus dem Hals, 13 Grad nach unten; Kiefer 60 = 5 Grad offen. Frei gesetzt, nur fuer Rundumblatt
RUHE_KIEFER = [0, 0, 60]


def posen_setzen(form, kopf):
    """Kopfwinkel (21, 22, 23) in die Keyframes der Clips 0 (Bild 1) und 1 (Bild 2) aus k_kopf_fit; Ruhe (Clip 2): Hals wie J, Kopf RUHE_KOPF."""
    pool = form['pool']
    for clip, key in ((0, 'bild1'), (1, 'bild2')):
        kf = form['clips'][clip]['bilder'][0]['kf']
        for b, w in kopf[key]['winkel'].items():
            pool[kf]['winkel'][int(b)] = [int(x) for x in w]
    kf = form['clips'][2]['bilder'][0]['kf']
    pool[kf]['winkel'][22] = list(RUHE_KOPF)
    pool[kf]['winkel'][23] = list(RUHE_KIEFER)


# ─────────────────────────────── Textur
def _rgb_teil(tx, rect, zeile):
    x, y, w, h = rect
    return tx.lut[zeile][tx.px[y:y + h, x:x + w]][..., :3].astype(np.float32)


def stoff(tx, bau, ziel_wh):
    """H2-Haut von J (Brust, CLUT-Zeile 0), auf die Zielgroesse gespiegelt gekachelt, Mittel 1 je Kanal (nur Struktur)."""
    a = {x['name']: x for x in bau['atlas']}['brust']
    s = _rgb_teil(tx, a['rechteck'], a['zeile'])
    W, H = ziel_wh
    reihe = np.concatenate([s, s[:, ::-1]], 1)
    blk = np.concatenate([reihe, reihe[::-1]], 0)
    rep = np.tile(blk, (H // blk.shape[0] + 1, W // blk.shape[1] + 1, 1))[:H, :W]
    return rep / np.maximum(rep.reshape(-1, 3).mean(0), 1.0), s.reshape(-1, 3).mean(0)


def farben_standard():
    """Erster Durchgang (ohne Lichtmessung): Verhaeltnis der Rueckrechnung zum Rumpf in Bild 2 (K.md Abschnitt 4), Basis = Mittel der Brust-Textur."""
    # haut: Oberkiefer (28,9,8) gegen Rumpf (27,12,12), Y 24 gegen 25 -> gleiche Haut wie der Rumpf, Abweichung unter dem Filterrauschen (farbe.md MAE 15-21)
    # kralle: Ton der Hand von J, zur Spitze heller um das gemessene Y-Verhaeltnis Spitze/Hand 1,2-2,0 (Mittel 1,6; K.md Abschnitt 2), ohne eigenen Farbton:
    # das Verhaeltnis der Farbkanaele (1,2 / 2,1 / 2,8) auf die graue Hand von J gelegt ergaebe Blau, das der Screenshot nicht zeigt (E14).
    return dict(haut=[1.0, 1.0, 1.0], fleisch=[2.2, 2.0, 1.75], unterseite=[2.2, 2.0, 1.75], hell=[3.4, 3.4, 3.4], kralle_wurzel=[1.0, 1.0, 1.0],
                kralle_spitze=[1.6, 1.6, 1.6])


def malen(tx, bau, farben, hand_mittel):
    """Seite 3 (128 x 256) als RGB. farben: Faktoren je Material relativ zur Brust-Textur von J (Haut, Fleisch, Hell) bzw. zur Hand (Kralle)."""
    S = np.zeros((256, 128, 3), np.float32)
    st, basis = stoff(tx, bau, (128, 256))

    def feld(rect, f):
        x, y, w, h = rect
        x -= KF.SEITE_X
        S[y:y + h, x:x + w] = st[y:y + h, x:x + w] * basis * np.array(f, np.float32)
    kx, ky, kw, kh = KF.ATLAS['kopf']
    feld(KF.ATLAS['kopf'], farben['haut'])
    # Gaumen: Ecken 0-1 und 9-10 (u < 0,1 und > 0,9) = Maulinneres oben
    gx = kx - KF.SEITE_X
    for u0, u1 in ((0.0, 0.09), (0.91, 1.0)):
        a, b = gx + int(u0 * kw), gx + int(math.ceil(u1 * kw))
        S[ky:ky + kh, a:b] = st[ky:ky + kh, a:b] * basis * np.array(farben['fleisch'], np.float32)
    jx, jy, jw, jh = KF.ATLAS['kiefer']
    gx = jx - KF.SEITE_X
    # Kiefer aussen (Ecken 0-2, 8-10) = Unterseite, Ecken 3-7 (u 0,3-0,7) = Rand und Maulboden (Fleisch). Screenshot 2 zeigt das ganze Band hell
    # (Unterkiefer gefiltert Y 94 gegen Rumpf 25), darum ist auch die Unterseite hell (wie die blasse Unterseite eines Krokodilkiefers).
    feld(KF.ATLAS['kiefer'], farben['unterseite'])
    a, b = gx + int(0.3 * jw), gx + int(math.ceil(0.7 * jw))
    S[jy:jy + jh, a:b] = st[jy:jy + jh, a:b] * basis * np.array(farben['fleisch'], np.float32)
    # helle Flecken am Rand: 6 Flecken im Band, Abstand 16 px = rund 190 Einheiten (K.md Abschnitt 1)
    for u in (0.35, 0.65):
        c = gx + int(u * jw)
        for r in range(jy + 3, jy + jh - 3, 9):
            S[r:r + 4, c - 2:c + 3] = basis * np.array(farben['hell'], np.float32) * st[r:r + 4, c - 2:c + 3].mean((0, 1))
    feld(KF.ATLAS['zahn'], farben['hell'])
    feld(KF.ATLAS['auge'], farben['haut'])
    x, y, w, h = KF.ATLAS['kralle']
    x -= KF.SEITE_X
    for r in range(h):
        a = r / max(h - 1, 1)
        f = (1 - a) * np.array(farben['kralle_wurzel']) + a * np.array(farben['kralle_spitze'])
        S[y + r, x:x + w] = st[y + r, x:x + w] * hand_mittel * f
    # Rest der Seite: Haut (kein Texel bleibt leer, sonst Loch-Index 0)
    leer = S.sum(2) == 0
    S[leer] = (st * basis * np.array(farben['haut'], np.float32))[leer]
    return np.clip(S, 8, 255)


def kmeans255(rgb):
    q = np.clip(np.round(rgb.reshape(-1, 3) / 8.0), 0, 31)
    code = (q[:, 0] + 32 * q[:, 1] + 1024 * q[:, 2]).astype(np.int64)
    u, inv, cnt = np.unique(code, return_inverse=True, return_counts=True)
    fb = np.stack([u % 32, (u // 32) % 32, u // 1024], 1).astype(np.float32)
    if len(u) <= 255:
        return fb, inv
    crit = (cv2.TERM_CRITERIA_EPS + cv2.TERM_CRITERIA_MAX_ITER, 60, 0.01)
    wdh = np.repeat(fb, np.maximum(1, np.round(np.sqrt(cnt))).astype(int), axis=0)
    cv2.setRNGSeed(7)
    _, _, zent = cv2.kmeans(wdh, 255, None, crit, 3, cv2.KMEANS_PP_CENTERS)
    zent = np.clip(np.round(zent), 0, 31)
    d = ((fb[:, None, :] - zent[None]) ** 2).sum(2)
    return zent, d.argmin(1)[inv]


def tim_k(j_tim, seite3):
    t = FE.tim_lesen(j_tim)
    px = np.zeros((256, 512), np.uint8)
    px[:, :384] = t['pixel']
    zent, lab = kmeans255(seite3)
    px[:, 384:] = (lab + 1).reshape(256, 128).astype(np.uint8)
    cl = [list(c) for c in t['cluts']] + [FE.clut_aus_rgb([(0, 0, 0)] + [tuple(int(c) << 3 for c in f) for f in zent], transparent0=True)]
    return FE.tim_schreiben(px, cl), len(zent)


def lichtmessung(aus):
    """Zweiter Durchgang: Rueckrechnung (farbe_bild2_echt) geteilt durch das Port-Licht je Pixel (sauber / Albedo aus form_pruefung.Kandidat.rendern),
    Faktor wie beim Rumpf: Albedo J / (echt / Licht) im Rumpf. Rueckgabe Zielfarben je Material in Texturwerten."""
    import form_pruefung as P
    k = P.Kandidat(os.path.join(aus, 'form.json'))
    sauber, alb, m, _ = k.rendern(2)
    hg = P.lade_rgb(P.daten('hintergrund_bild2.png')).astype(np.float32)
    echt = cv2.imread(os.path.join(KK.FORM, 'farbe_bild2_echt.png'))[:, :, ::-1].astype(np.float32)
    orig = P.original(2).astype(np.float32)
    s, a = sauber.astype(np.float32), alb.astype(np.float32)
    licht = np.where(a > 4, s / np.maximum(a, 1), 0)
    from k_kopf_fit import Buehne
    form, mo, fj = KK.lade(aus)
    tief, idb = KK.raster(KK.kamera(2), *KK.tris_welt(form, *KK.welt_pose(*mo.pose(KK.kf_von(mo, 1)), fj['bild2']['lage'], fj['bild2']['gier'])))
    logo = P.logo_maske()
    gut = m & ~logo & (licht.min(2) > 0.15)
    face = KK.maske('face_and_upper_body') & ~KK.maske('oberkiefer')
    rumpf = gut & face & np.isin(idb, (0, 7, 10, 11, 16)) & (a.sum(2) > 30)
    q = lambda mm: (echt[mm] / licht[mm]).mean(0)
    k_rumpf = a[rumpf].mean(0) / np.maximum(q(rumpf), 1e-3)
    Y = orig @ YK
    uk = KK.maske('unterkiefer')
    ok = KK.maske('oberkiefer')
    zon = dict(haut=gut & ok & (idb == 22), fleisch=gut & uk & (idb == 23) & (Y < np.percentile(Y[uk], 50)),
               hell=gut & uk & (idb == 23) & (Y > np.percentile(Y[uk], 85)))
    ziel = {n: (q(mm) * k_rumpf).tolist() if mm.sum() > 20 else None for n, mm in zon.items()}
    return dict(k_rumpf=k_rumpf.tolist(), rumpf_px=int(rumpf.sum()), zonen_px={n: int(mm.sum()) for n, mm in zon.items()}, ziel=ziel)


def bauen(kopf_json, aus, name, naht=None, farben=None, pruefen=True):
    kopf = json.load(open(kopf_json, encoding='utf-8'))
    form, mo, fj = KK.lade()
    j_tim = open(os.path.join(KK.J_DIR, 'FORM.TIM'), 'rb').read()
    p = dict(KF.VORGABE)
    p.update(kopf['form'])
    p['zaehne'] = ZAEHNE
    neu_netz(form, 22, KF.kopf(p))
    neu_netz(form, 23, KF.kiefer(p))
    kb = krallen(form)
    posen_setzen(form, kopf)
    quelle = copy.deepcopy(form)          # Masse und UV der Gelenkkoerper vom neuen Kopf, nicht vom alten Kopf von J
    nb = {}
    if naht:
        form, nb = KN.schliessen(form, KN.laden(naht), quelle=quelle)
    FE.orientiere(form)
    os.makedirs(aus, exist_ok=True)
    open(os.path.join(aus, 'FORM.EMD'), 'wb').write(FE.schreibe_emd(form))
    import form_buehne as FB
    tx = FB.Textur(j_tim)
    bau = json.load(open(os.path.join(KK.J_DIR, 'bau.json')))
    hand = np.mean([_rgb_teil(tx, a['rechteck'], a['zeile']).reshape(-1, 3).mean(0) for a in bau['atlas'] if a['name'] in ('hand_r', 'hand_l')], 0)
    _st, brust = stoff(tx, bau, (8, 8))
    fb = farben or farben_standard()
    S = malen(tx, bau, fb, hand)
    tim, nf = tim_k(j_tim, S)
    open(os.path.join(aus, 'FORM.TIM'), 'wb').write(tim)
    cv2.imwrite(os.path.join(aus, 'seite3.png'), S[:, :, ::-1].astype(np.uint8))
    j = dict(fj)
    j['name'] = name
    json.dump(j, open(os.path.join(aus, 'form.json'), 'w', encoding='utf-8'), indent=1)
    fl = sum(len(m['dreiecke']) + len(m['vierecke']) for m in form['meshes'])
    meta = dict(name=name, basis='J_dicker (FORM.EMD/TIM, Textur H2)', kopf=kopf_json, kopfform=p, kopf_mass=dict(bild2=kopf['bild2']['mass'], bild1=kopf['bild1']['mass']),
                kiefer_offen_grad=kopf.get('kiefer_offen_grad'), krallen=kb, naht=nb, flaechen=fl, farben=fb, brust_mittel=brust.tolist(), hand_mittel=hand.tolist(),
                farben_zeile3=nf)
    json.dump(meta, open(os.path.join(aus, 'bau.json'), 'w', encoding='utf-8'), indent=1)
    print('%s: %d Flaechen, Zeile 3 %d Farben' % (name, fl, nf))
    if pruefen:
        env = dict(os.environ, PYTHONIOENCODING='utf-8')
        r = subprocess.run([sys.executable, os.path.join(KK.TOOLS, 'form_pruefung.py'), 'pruefe', os.path.join(aus, 'form.json'), '--aus', os.path.join(aus, 'pruefung')],
                           capture_output=True, text=True, env=env, encoding='utf-8', errors='replace')
        if r.returncode:
            print(r.stdout[-2000:], r.stderr[-2000:])
    return meta


def kiefer_eichen(aus):
    """Helligkeit des Unterkiefers wie Screenshot 2: Render durch video_filter gegen das Video, mittleres Y im Unterkiefer (Nutzer-Teil, sichtbarer Knochen 23,
    ohne Logo). Rueckgabe Soll/Ist."""
    import form_pruefung as P
    import form_video as FV
    k = P.Kandidat(os.path.join(aus, 'form.json'))
    sauber, _alb, m, _ = k.rendern(2)
    fil = FV.video_filter(sauber, bild=2, entzerren=False, rauschen=False).astype(np.float32)
    form, mo, fj = KK.lade(aus)
    _t, idb = KK.raster(KK.kamera(2), *KK.tris_welt(form, *KK.welt_pose(*mo.pose(KK.kf_von(mo, 1)), fj['bild2']['lage'], fj['bild2']['gier'])))
    z = KK.maske('unterkiefer') & (idb == 23) & ~P.logo_maske()
    orig = P.original(2).astype(np.float32)
    return float((orig[z] @ YK).mean() / max((fil[z] @ YK).mean(), 1.0)), int(z.sum())


def alles(kopf, aus, name, naht=None, pruefen=True):
    """Durchgang 1 Standardfarben, Lichtmessung, Durchgang 2 Farben aus Licht, Eichung Kiefer-Helligkeit (2 Runden), Enddurchgang mit Pruefung."""
    bauen(kopf, aus, name, naht, None, pruefen=False)
    L = lichtmessung(aus)
    import form_buehne as FB
    tx = FB.Textur(open(os.path.join(KK.J_DIR, 'FORM.TIM'), 'rb').read())
    _st, brust = stoff(tx, json.load(open(os.path.join(KK.J_DIR, 'bau.json'))), (8, 8))
    fb = farben_standard()
    if L['ziel']['fleisch'] is not None:
        fb['fleisch'] = [round(float(z / max(b, 1.0)), 3) for z, b in zip(L['ziel']['fleisch'], brust)]
    fb['unterseite'] = list(fb['fleisch'])
    # Zaehne: hellster Wert im Band. Die Rueckrechnung ergibt Y > 255 und einen Gruenstich aus dem Kanalfaktor des Rumpfs (k_rumpf), den das Video nicht zeigt
    # (E14). Darum Ton des Fleisches, auf Y 230 gehoben und halb mit Neutralweiss gemischt.
    f = np.array(fb['fleisch']) * brust
    yf = float(f @ YK)
    zahn = 0.5 * np.clip(f * 230.0 / max(yf, 1.0), 0, 255) + 0.5 * 230.0
    fb['hell'] = [round(float(z / max(b, 1.0)), 3) for z, b in zip(zahn, brust)]
    eich = []
    for runde in range(2):
        bauen(kopf, aus, name, naht, fb, pruefen=False)
        q, px = kiefer_eichen(aus)
        eich.append(dict(runde=runde, soll_durch_ist=round(q, 3), px=px))
        print('  Kiefer-Eichung %d: Soll/Ist %.3f (%d px)' % (runde, q, px))
        if abs(q - 1) < 0.03:
            break
        for n in ('fleisch', 'unterseite'):
            fb[n] = [round(float(v * q), 3) for v in fb[n]]
    fb['lichtmessung'] = L
    fb['kiefer_eichung'] = eich
    return bauen(kopf, aus, name, naht, fb, pruefen=pruefen)


if __name__ == '__main__':
    a = argparse.ArgumentParser()
    a.add_argument('--kopf', required=True)
    a.add_argument('--aus', required=True)
    a.add_argument('--name', required=True)
    a.add_argument('--naht', default=None)
    a.add_argument('--ohne-pruefung', action='store_true')
    a.add_argument('--schnell', action='store_true', help='nur ein Durchgang mit Standardfarben (fuer Abstimmung)')
    x = a.parse_args()
    if x.schnell:
        bauen(x.kopf, x.aus, x.name, x.naht, None, pruefen=not x.ohne_pruefung)
    else:
        alles(x.kopf, x.aus, x.name, x.naht, pruefen=not x.ohne_pruefung)
