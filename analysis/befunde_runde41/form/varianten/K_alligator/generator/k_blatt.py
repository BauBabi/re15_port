#!/usr/bin/env python3
"""Variante K: Kontaktblaetter fuer den Nutzer (deutsch beschriftet, ohne Umlaute wie im Repo).

    python k_blatt.py bild ../K_alligator/blatt_bild2.png 2 "J=../../J_dicker" "K1=../K1_lang" ...
    python k_blatt.py nah  AUS.png "J=..." "K1=..."         Maul und Krallen nah, Screenshot 2 gegen Render
    python k_blatt.py rundum AUS.png "J=..." "K1=..."       sechs Winkel, Ruhepose, und Kopf nah
    python k_blatt.py loecher AUS.png VORHER.EMD VORHER.json NACHHER.EMD NACHHER.json [N]   schlimmste Nahtloecher vorher/nachher, gleiche Pose/Ansicht

Render: form_pruefung.Kandidat.rendern (Buehne wie die Pruefung: Kamera, Licht, Hintergrund), gefiltert mit form_video.video_filter wie im Pruefblatt."""
import json
import math
import os
import sys

import cv2
import numpy as np

import k_kern as KK
import form_pruefung as P
import form_buehne as FB
import form_video as FV

GRAU = (92, 92, 92)


def schrift(a, t, x, y, g=0.5, f=(255, 255, 255), d=1):
    cv2.putText(a, t, (x, y), cv2.FONT_HERSHEY_SIMPLEX, g, (0, 0, 0), d + 2, cv2.LINE_AA)
    cv2.putText(a, t, (x, y), cv2.FONT_HERSHEY_SIMPLEX, g, f, d, cv2.LINE_AA)


def kachel(a, titel, w, h):
    b = cv2.resize(a, (w, h), interpolation=cv2.INTER_AREA if a.shape[1] > w else cv2.INTER_NEAREST)
    b = np.ascontiguousarray(b)
    schrift(b, titel, 6, 18, 0.5)
    return b


def gitter(zeilen, kopf, rand=6):
    """zeilen: Liste von (Zeilentitel, [Kacheln BGR gleicher Hoehe])."""
    h = zeilen[0][1][0].shape[0]
    breite = max(sum(k.shape[1] + rand for k in z[1]) for z in zeilen) + 150
    out = np.full((40 + len(zeilen) * (h + rand) + rand, breite, 3), 24, np.uint8)
    schrift(out, kopf, 8, 26, 0.6, (230, 230, 120))
    y = 40
    for t, ks in zeilen:
        for i, line in enumerate(t.split('|')):
            schrift(out, line, 6, y + 22 + 20 * i, 0.5, (240, 200, 120))
        x = 150
        for k in ks:
            out[y:y + h, x:x + k.shape[1]] = k
            x += k.shape[1] + rand
        y += h + rand
    return out


def lade_var(spec):
    nm, d = spec.split('=', 1)
    return nm, P.Kandidat(os.path.join(d, 'form.json')), d


def id_karte(d, n):
    form, mo, fj = KK.lade(d)
    s = fj['bild%d' % n]
    R, T = mo.pose(KK.kf_von(mo, s['clip'], s['bild']))
    return KK.raster(KK.kamera(n), *KK.tris_welt(form, *KK.welt_pose(R, T, s['lage'], s['gier'])))[1]


def umrisse(a, maske, farbe, s=1):
    cs, _ = cv2.findContours(maske.astype(np.uint8), cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_NONE)
    if s != 1:
        cs = [np.round(c * s).astype(np.int32) for c in cs]
    cv2.drawContours(a, cs, -1, farbe, 1)


def blatt_bild(aus, n, specs):
    orig = P.original(n)[:, :, ::-1].copy()
    zeilen = []
    for sp in specs:
        nm, k, d = lade_var(sp)
        sauber, alb, m, _ = k.rendern(n)
        fil = FV.video_filter(sauber, bild=n, entzerren=False, rauschen=False)
        idb = id_karte(d, n)
        ov = (orig * 0.75).astype(np.uint8)
        umrisse(ov, m, (0, 255, 0))
        umrisse(ov, idb == 22, (0, 220, 255))
        umrisse(ov, idb == 23, (255, 0, 255))
        if n == 2:
            for t, f in (('oberkiefer', (0, 140, 255)), ('unterkiefer', (200, 80, 255))):
                umrisse(ov, KK.maske(t), f)
        w, h = 480, 360
        zeilen.append((nm, [kachel(orig, 'Screenshot %d' % n, w, h), kachel(ov, 'Overlay', w, h), kachel(sauber[:, :, ::-1], 'Render sauber', w, h),
                            kachel(fil[:, :, ::-1], 'Render durch Videofilter', w, h)]))
    kopf = ('Bild %d: Screenshot | Overlay (gruen Modellumriss, gelb Kopf 22, magenta Kiefer 23%s) | Render mit der Kamera von Bild %d'
            % (n, '; orange/violett Nutzer-Teile Ober-/Unterkiefer' if n == 2 else '', n))
    cv2.imwrite(aus, gitter(zeilen, kopf))


def ausschnitt(a, box, s):
    x0, y0, x1, y1 = box
    return cv2.resize(np.ascontiguousarray(a[y0:y1, x0:x1]), None, fx=s, fy=s, interpolation=cv2.INTER_NEAREST)


MAUL = (190, 250, 370, 420)
HAND_R = (130, 330, 300, 480)
HAND_L = (350, 250, 490, 430)


def blatt_nah(aus, specs):
    """Je Variante eine Zeile: Maul, Hand rechts, Hand links; je Stelle Screenshot | Render durch Videofilter | Render sauber (1,5-fach)."""
    orig = P.original(2)[:, :, ::-1].copy()
    zeilen = []
    for sp in specs:
        nm, k, d = lade_var(sp)
        sauber, alb, m, _ = k.rendern(2)
        fil = FV.video_filter(sauber, bild=2, entzerren=False, rauschen=False)[:, :, ::-1].copy()
        s = sauber[:, :, ::-1].copy()
        o = orig.copy()
        for t, f in (('oberkiefer', (0, 140, 255)), ('unterkiefer', (200, 80, 255))):
            umrisse(o, KK.maske(t), f)
            umrisse(fil, KK.maske(t), f)
        ks = []
        for box, titel in ((MAUL, 'Maul'), (HAND_R, 'Hand rechts'), (HAND_L, 'Hand links')):
            for bild, art in ((o, 'Screenshot'), (fil, 'gefiltert'), (s, 'sauber')):
                a = ausschnitt(bild, box, 1.5)
                schrift(a, '%s %s' % (titel, art), 4, 14, 0.4)
                ks.append(a)
        zeilen.append((nm, ks))
    h = max(k.shape[0] for z in zeilen for k in z[1])
    zeilen = [(t, [cv2.copyMakeBorder(k, 0, h - k.shape[0], 0, 0, cv2.BORDER_CONSTANT, value=(24, 24, 24)) for k in ks]) for t, ks in zeilen]
    cv2.imwrite(aus, gitter(zeilen, 'Nahansicht Bild 2 (1,5-fach): Maul und Krallen, je Stelle Screenshot | Render durch Videofilter | Render sauber; orange/violett = Nutzer-Teile Ober-/Unterkiefer'))


ANSICHTEN6 = (('vorn', 90, 0), ('3/4 vorn rechts', 45, 15), ('rechts', 0, 0), ('hinten', -90, 0), ('links', 180, 0), ('3/4 vorn links oben', 135, 30))


def rundum_bild(k, kf, az, el, gross, knochen_mitte=None, zoom=1.0):
    tris = P.rundum_dreiecke(k.mo, kf, az, el, gross)
    a = np.full((gross, gross, 3), 92, np.uint8)
    ab = dict(ax=1.0, bx=0.0, ay=1.0, by=0.0)
    if knochen_mitte is not None:
        # Kopf nah: gleiche Projektion, verschoben und vergroessert auf den Kopf
        R, T = k.mo.pose(kf)
        alle = P.pose_punkte(k.mo, kf)
        mitte = (alle.max(0) + alle.min(0)) / 2
        rad = float(np.linalg.norm(alle - mitte, axis=1).max())
        s = gross * 0.46 / rad
        A = P_blick(az, el)
        q = (np.asarray(T[knochen_mitte], float) - mitte) @ A.T
        cx, cy = gross / 2 + q[0] * s, gross / 2 + q[1] * s
        ab = dict(ax=zoom, bx=gross / 2 - cx * zoom, ay=zoom, by=gross / 2 - cy * zoom)
    m = FB.rastern(tris, k.tex, a, ab)
    a[m] = np.clip(np.power(a[m] / 255.0, 0.5) * 255, 0, 255).astype(np.uint8)
    return a[:, :, ::-1].copy()


def P_blick(az, el):
    a, e = math.radians(az), math.radians(el)
    vor = np.array([-math.sin(a) * math.cos(e), math.sin(e), math.cos(a) * math.cos(e)])
    rechts = np.array([math.cos(a), 0.0, math.sin(a)])
    unten = np.cross(vor, rechts)
    if unten[1] < 0 and abs(el) < 80:
        unten = -unten
    return np.stack([rechts, unten, vor])


def blatt_rundum(aus, specs, clip=2):
    zeilen = []
    for sp in specs:
        nm, k, d = lade_var(sp)
        kf = k.kf(clip, 0)
        zeilen.append((nm + '|ganz', [kachel(rundum_bild(k, kf, az, el, 400), t, 300, 300) for t, az, el in ANSICHTEN6]))
        zeilen.append((nm + '|Kopf nah', [kachel(rundum_bild(k, kf, az, el, 400, knochen_mitte=22, zoom=2.6), t, 300, 300) for t, az, el in ANSICHTEN6]))
    cv2.imwrite(aus, gitter(zeilen, 'Rundum, Ruhepose (Clip 2), Licht nur fuers Blatt, aufgehellt (Gamma 0,5); je Variante ganze Figur und Kopf 2,6-fach'))


def blatt_loecher(aus, vorher_emd, vorher_js, nachher_emd, nachher_js, n=8, tim=None):
    """Schlimmste Nahtloecher von VORHER (Pose, Ansicht) neben derselben Pose/Ansicht NACHHER. Textur-Render wie das Rundumblatt (rundum_dreiecke),
    Loecher aus k_loecher in derselben Projektion, rot umrandet und gefuellt."""
    import k_loecher as KL
    R0 = json.load(open(vorher_js, encoding='utf-8'))
    R1 = json.load(open(nachher_js, encoding='utf-8'))
    L = []
    for c in R0['clips']:
        for b in c['bilder']:
            px = sum(l['px'] for l in b['loecher'] if l['naht'])
            if px:
                L.append((px, c['clip'], b['bild'], b['ansicht']))
    L.sort(reverse=True)
    gesehen, wahl = set(), []
    for x in L:
        if (x[1], x[2]) not in gesehen:
            gesehen.add((x[1], x[2]))
            wahl.append(x)
        if len(wahl) >= n:
            break
    namen = {c['clip']: c['name'] for c in json.load(open(os.path.join(KK.FORM, 'stufe3', 'asset', 'clips.json'), encoding='utf-8'))}
    V = {a[0]: a for a in KL.ANSICHTEN}
    tim = tim or os.path.join(KK.J_DIR, 'FORM.TIM')
    zeilen = []
    for px, c, b, an in wahl:
        ks = []
        for emd, t, R in ((vorher_emd, os.path.join(KK.J_DIR, 'FORM.TIM'), R0), (nachher_emd, tim, R1)):
            mo = KK.FE.modell(open(emd, 'rb').read(), open(t, 'rb').read())
            tex = FB.Textur(open(t, 'rb').read())
            fig = KL.Figur(emd)
            kf = fig.kf(c, b)
            _, az, el = V[an]
            gross = 420
            alle = P.pose_punkte(mo, kf)
            mitte = (alle.max(0) + alle.min(0)) / 2
            s = gross * 0.46 / float(np.linalg.norm(alle - mitte, axis=1).max())
            tris = P.rundum_dreiecke(mo, kf, az, el, gross)
            a = np.full((gross, gross, 3), 92, np.uint8)
            m = FB.rastern(tris, tex, a, dict(ax=1.0, bx=0.0, ay=1.0, by=0.0))
            a[m] = np.clip(np.power(a[m] / 255.0, 0.5) * 255, 0, 255).astype(np.uint8)
            a = a[:, :, ::-1].copy()
            idb, piv, _ = KL.rendern(fig, kf, KL.blick(az, el), mitte=mitte, gross=gross, s=s)
            lo = KL.loecher(idb, piv, s=s)
            npx = 0
            for l in lo:
                if not l['naht']:
                    continue
                x, y, w, h = l['box']
                sub = idb[y:y + h, x:x + w] < 0
                a[y:y + h, x:x + w][sub] = (0, 0, 255)
                cv2.rectangle(a, (x - 3, y - 3), (x + w + 3, y + h + 3), (0, 0, 255), 1)
                npx += l['px']
            ks.append(kachel(a, '%s: Naht %d px (Massstab %.3f px/E.)' % ('vorher' if emd == vorher_emd else 'nachher', npx, s), 420, 420))
        zeilen.append(('Clip %d %s|Bild %d|%s' % (c, namen.get(c, ''), b, an), ks))
    cv2.imwrite(aus, gitter(zeilen, 'Nahtloecher, schlimmste Posen vorher (J, Stufe-3-Asset) und nachher (gleiche Pose und Ansicht); rot = Loch an einer Naht'))


if __name__ == '__main__':
    art = sys.argv[1]
    if art == 'bild':
        blatt_bild(sys.argv[2], int(sys.argv[3]), sys.argv[4:])
    elif art == 'nah':
        blatt_nah(sys.argv[2], sys.argv[3:])
    elif art == 'rundum':
        blatt_rundum(sys.argv[2], sys.argv[3:])
    elif art == 'loecher':
        blatt_loecher(sys.argv[2], sys.argv[3], sys.argv[4], sys.argv[5], sys.argv[6], int(sys.argv[7]) if len(sys.argv) > 7 else 8,
                      sys.argv[8] if len(sys.argv) > 8 else None)
