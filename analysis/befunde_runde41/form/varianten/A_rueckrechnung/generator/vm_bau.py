#!/usr/bin/env python3
"""Varianten, Bau: Geometrie (fb_bau, optional vm_form: Schwanz E9, weniger Flaechen) -> gleichmaessiger Atlas -> vm_mal (RE2-Stil, Farbe A/B/C)
-> TIM mit drei CLUT-Zeilen wie RE2 -> FORM.EMD, FORM.TIM, form.json im Variantenordner -> form_pruefung pruefe.

    python vm_bau.py --farbe A --aus ../A_rueckrechnung [--schwanz e9] [--flaechen wenig] [--ohne-pruefung]

Gemessen an RE2 (VARIANTEN.md Abschnitt 1): je Mesh eine CLUT-Zeile, 2-4 Zeilen je Modell, je Zeile rund 250 Farben. Atlas: Gewicht 1 fuer alle Teile
(form_bemalt hatte 0,55-1,35 je Teil, Dichte p10/p90 0,0157/0,0373). Kein RE-Wert."""
import argparse
import json
import math
import os
import subprocess
import sys
import time

import cv2
import numpy as np

from rf_kern import HIER, TOOLS
import form_emd as FE
import fb_bau
import rf_plan

TEX_W, TEX_H = 384, 256


def gruppe(name):
    """CLUT-Zeile je Teil wie RE2 (je Mesh eine Zeile): 0 Rumpf und Kopf, 1 Arme und Haende, 2 Beine und Schwanz."""
    if name in ('huefte', 'wirbel', 'brust', 'hals', 'kopf', 'kiefer') or name.startswith(('fang', 'wange')):
        return 0
    if name.startswith(('oberarm', 'unterarm', 'hand', 'klaue', 'kralle')):
        return 1
    return 2


def zeilen_setzen(form, teile):
    zk = {}
    for p in teile:
        zk.setdefault(p.knochen, gruppe(p.name))
    for b, mm in enumerate(form['meshes']):
        z = zk.get(b, 0)
        for f in mm['dreiecke'] + mm['vierecke']:
            f['zeile'] = z
    return zk


def kmeans_zeile(farben):
    """farben [N,3] 8 Bit -> (Zentren 5 Bit [K,3], Index je Farbe). K <= 255."""
    q = np.clip(np.round(farben.astype(np.float32) / 8.0), 0, 31)
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


def tim_zeilen(rgb, zeile_je_texel, n_zeilen=3):
    px = np.zeros(rgb.shape[:2], np.uint8)
    cluts, info = [], []
    for z in range(n_zeilen):
        s = zeile_je_texel == z
        if not s.any():
            cluts.append(FE.clut_aus_rgb([(0, 0, 0)], transparent0=True))
            info.append(0)
            continue
        zent, lab = kmeans_zeile(rgb[s].reshape(-1, 3))
        px[s] = (lab + 1).astype(np.uint8)
        cluts.append(FE.clut_aus_rgb([(0, 0, 0)] + [tuple(int(c) << 3 for c in f) for f in zent], transparent0=True))
        info.append(len(zent))
    return px, cluts, info


def eichen(aus, schreib_tim, gain, gs, runden=6):
    """Helligkeit der Haut am Standbild: Render durch video_filter (ohne Korn) gegen das Standbild, mittleres Y in der Figurmaske
    (ziel2 weiss, ohne Logo). Koerper: Bild 1 ohne Schwanz und Bild 2 (geometrisches Mittel), Schwanz: Nutzerteil tail in Bild 1.
    So unterscheiden sich die Varianten nur in Malstil und Farbton, nicht in der Helligkeit."""
    import form_pruefung as P
    import form_video as FV
    mk = {n: (P.ziel_laden(n)[1] == 255) & ~P.logo_maske() for n in (1, 2)}
    sw = P.teil_maske('tail') & mk[1]
    rest1 = mk[1] & ~sw
    Yo = {n: P.original(n).astype(np.float32) @ P.YK for n in (1, 2)}
    log = []
    for r in range(runden):
        schreib_tim(gain, gs)
        k = P.Kandidat(os.path.join(aus, 'form.json'))
        Yr = {}
        for n in (1, 2):
            sauber, _alb, _m, _ = k.rendern(n)
            Yr[n] = FV.video_filter(sauber, bild=n, entzerren=False, rauschen=False).astype(np.float32) @ P.YK
        q1 = float(Yo[1][rest1].mean() / Yr[1][rest1].mean())
        q2 = float(Yo[2][mk[2]].mean() / Yr[2][mk[2]].mean())
        qs = float(Yo[1][sw].mean() / Yr[1][sw].mean())
        log.append(dict(runde=r, gain=round(gain, 4), gain_schwanz=round(gs, 4), q_bild1=round(q1, 3), q_bild2=round(q2, 3), q_schwanz=round(qs, 3)))
        print('  Eichung %d: Haut %.3f Schwanz %.3f  Soll/Ist Bild1 %.3f Bild2 %.3f Schwanz %.3f' % (r, gain, gs, q1, q2, qs))
        if max(abs(math.log(q1 * q2) / 2), abs(math.log(qs))) < 0.01:
            break
        gain *= math.sqrt(q1 * q2) ** 1.2
        gs *= qs ** 1.2
    return gain, gs, log


def baue(args):
    t0 = time.time()
    g, teile = rf_plan.plan()
    stand = json.load(open(os.path.join(HIER, 'stand_final.json')))
    fb_bau.delta_laden(args.delta)
    fb_bau.dichte_gewicht = lambda n: 1.0            # gleichmaessige Texelgroesse ueber alle Teile (E8)
    if args.schwanz == 'e9' or args.flaechen != 'voll':
        import vm_form
        form, lage, dichte, L, C = vm_form.baue_form(g, teile, stand, schwanz=args.schwanz, flaechen=args.flaechen)
    else:
        form, lage, dichte, L, C = fb_bau.baue_form(g, teile, stand)
    zk = zeilen_setzen(form, teile)
    import vm_mal
    ml = vm_mal.Maler(form, g, teile, lage, stand, L, C)
    # Zeile je Texel: Knochen -> Zeile; leere Texel nach dem Atlas-Rechteck
    zt = np.zeros((TEX_H, TEX_W), np.int32)
    for p, r in zip(teile, lage):
        x, y, w, h = r
        zt[y:y + h, x:x + w] = gruppe(p.name)
    for b, z in zk.items():
        zt[ml.kn == b] = z
    aus = os.path.abspath(args.aus)
    os.makedirs(aus, exist_ok=True)
    open(os.path.join(aus, 'FORM.EMD'), 'wb').write(FE.schreibe_emd(form))
    j = dict(name=args.name or os.path.basename(aus), emd='FORM.EMD', tim='FORM.TIM', licht='mess', ruhe=dict(clip=2, bild=0), teile=fb_bau.teile_json(g))
    for n in (1, 2):
        p = stand['posen'][str(n)]
        j['bild%d' % n] = dict(lage=[int(round(p['lage'][0])), 0, int(round(p['lage'][2]))], gier=int(round(p['gier'])) % 4096, clip=n - 1, bild=0)
    json.dump(j, open(os.path.join(aus, 'form.json'), 'w', encoding='utf-8'), indent=1)

    def schreib_tim(g_, gs_):
        rgb_ = ml.farbe(args.farbe, g_, gs_)
        px_, cl_, nf_ = tim_zeilen(rgb_, zt)
        open(os.path.join(aus, 'FORM.TIM'), 'wb').write(FE.tim_schreiben(px_, cl_))
        return rgb_, nf_
    gain, gs, eich = args.gain, args.gain_schwanz, []
    if args.aus_bild:
        # D: Farben aus den Screenshots (vm_bild); vorlaeufige TIM nur fuer die Indizes des Weissbilds
        import vm_bild
        schreib_tim(gain, gs)
        nf_ = {}

        def schreib_rgb(rgb_):
            px_, cl_, nf_['n'] = tim_zeilen(rgb_, zt)
            open(os.path.join(aus, 'FORM.TIM'), 'wb').write(FE.tim_schreiben(px_, cl_))
        rgb, eich = vm_bild.wunden_eichen(ml, aus, os.path.join(aus, 'form.json'), schreib_rgb, sp=args.ms_sp, sr=args.ms_sr, schaerfe=args.schaerfe,
                                          form_staerke=args.form_staerke)
        nf = nf_['n']
    else:
        if args.eichen:
            gain, gs, eich = eichen(aus, schreib_tim, gain, gs)
        rgb, nf = schreib_tim(gain, gs)
    cv2.imwrite(os.path.join(aus, 'textur.png'), rgb[:, :, ::-1])
    fl = sum(len(m_['dreiecke']) + len(m_['vierecke']) for m_ in form['meshes'])
    meta = dict(farbe=args.farbe, schwanz=args.schwanz, flaechen_art=args.flaechen, flaechen=fl, dichte=round(dichte, 4), farben_je_zeile=nf,
                gain=round(gain, 4), gain_schwanz=round(gs, 4), eichung=eich,
                atlas=[dict(name=p.name, knochen=p.knochen, rechteck=list(r), zeile=gruppe(p.name)) for p, r in zip(teile, lage)])
    json.dump(meta, open(os.path.join(aus, 'bau.json'), 'w', encoding='utf-8'), indent=0)
    print('Bau %s: %d Flaechen, Dichte %.4f, Farben je Zeile %s, Helligkeit Haut %.3f Schwanz %.3f, %.0f s' % (args.farbe, fl, dichte, nf, gain, gs, time.time() - t0))
    if not args.ohne_pruefung:
        d = os.path.join(aus, 'pruefung')
        env = dict(os.environ, PYTHONIOENCODING='utf-8')
        r = subprocess.run([sys.executable, os.path.join(TOOLS, 'form_pruefung.py'), 'pruefe', os.path.join(aus, 'form.json'), '--aus', d],
                           capture_output=True, text=True, env=env, encoding='utf-8', errors='replace')
        if r.returncode:
            print(r.stdout[-3000:], r.stderr[-3000:])
        print('Pruefung %s, %.0f s' % (d, time.time() - t0))
    return aus


if __name__ == '__main__':
    a = argparse.ArgumentParser()
    a.add_argument('--farbe', default='A', choices=('A', 'B', 'C'))
    a.add_argument('--aus', required=True)
    a.add_argument('--name', default=None)
    a.add_argument('--schwanz', default='alt', choices=('alt', 'e9'))
    a.add_argument('--flaechen', default='voll', choices=('voll', 'wenig'))
    a.add_argument('--delta', default=None)
    a.add_argument('--ohne-pruefung', action='store_true')
    a.add_argument('--eichen', action='store_true')
    a.add_argument('--aus-bild', action='store_true', help='D: Farben aus den Screenshots (vm_bild)')
    a.add_argument('--ms-sp', type=int, default=3)
    a.add_argument('--ms-sr', type=int, default=14)
    a.add_argument('--schaerfe', type=float, default=0.6)
    a.add_argument('--form-staerke', type=float, default=0.12)
    a.add_argument('--gain', type=float, default=1.0)
    a.add_argument('--gain-schwanz', type=float, default=1.0)
    baue(a.parse_args())
