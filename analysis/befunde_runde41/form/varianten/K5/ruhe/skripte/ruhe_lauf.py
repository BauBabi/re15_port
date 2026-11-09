#!/usr/bin/env python3
"""Messblatt RUHE Teil 1: vier Modelle unter weissem Licht (4 Winkel, Ruhe-Clip) und in den Szenenketten Bild 1/2 (Disc-Licht x k ohne Filter,
mit Videofilter Saaten 15/12345/7); dazu die Vorlage (Video-Standbild in den Nutzer-Teilmasken, nur Info). Schreibt messung/ruhe_mass.json und
die Kacheln r1-r6.   python ruhe_lauf.py"""
import json
import math
import os

import cv2
import numpy as np

import rl
import ruhe_mass as M

SCHRIFT = cv2.FONT_HERSHEY_SIMPLEX


def text(im, t, x, y, s=0.42, c=(235, 235, 235)):
    cv2.putText(im, t, (x, y), SCHRIFT, s, (0, 0, 0), 3, cv2.LINE_AA)
    cv2.putText(im, t, (x, y), SCHRIFT, s, c, 1, cv2.LINE_AA)


def kachel(img, titel, zeilen=()):
    k = np.full((22 + 18 * len(zeilen), img.shape[1], 3), 24, np.uint8)
    text(k, titel, 4, 15)
    for i, z in enumerate(zeilen):
        text(k, z, 4, 33 + 18 * i, 0.4, (230, 230, 140))
    return np.vstack([k, img])


def reihe(ks, rand=4):
    h = max(k.shape[0] for k in ks)
    o = []
    for k in ks:
        if k.shape[0] < h:
            k = np.vstack([k, np.full((h - k.shape[0], k.shape[1], 3), 24, np.uint8)])
        o += [k, np.full((h, rand, 3), 24, np.uint8)]
    return np.hstack(o[:-1])


def stapel(rs, kopf):
    w = max(r.shape[1] for r in rs)
    k = np.full((28, w, 3), 24, np.uint8)
    text(k, kopf, 4, 19, 0.5)
    o = [k]
    for r in rs:
        if r.shape[1] < w:
            r = np.hstack([r, np.full((r.shape[0], w - r.shape[1], 3), 24, np.uint8)])
        o += [r, np.full((6, w, 3), 24, np.uint8)]
    return np.vstack(o)


def speichern(name, rgb):
    p = os.path.join(rl.OUT, name)
    cv2.imwrite(p, cv2.cvtColor(rgb, cv2.COLOR_RGB2BGR))
    return p


def fleck_bild(img, karte):
    g = cv2.cvtColor(cv2.cvtColor(img, cv2.COLOR_RGB2GRAY), cv2.COLOR_GRAY2RGB)
    g = (g * 0.6).astype(np.uint8)
    for wert, farbe in ((1, (0, 220, 255)), (2, (255, 0, 255)), (3, (255, 230, 0)), (4, (255, 60, 30))):
        g[karte == wert] = farbe
    return g


def ton_bild(img, v):
    L = M.lab(img)
    C = np.hypot(L[..., 1], L[..., 2])
    h = np.mod(np.degrees(np.arctan2(L[..., 2], L[..., 1])), 360.0)
    sek = (np.floor(h / 15.0) * 15.0 + 7.5)
    o = np.full(img.shape, 40, np.uint8)
    lab_k = np.stack([np.full(h.shape, 60.0), 45 * np.cos(np.radians(sek)), 45 * np.sin(np.radians(sek))], -1).astype(np.float32)
    rgb = np.clip(cv2.cvtColor(lab_k, cv2.COLOR_LAB2RGB) * 255, 0, 255).astype(np.uint8)
    bunt = v & (C >= 6)
    o[bunt] = rgb[bunt]
    o[v & ~bunt] = 150
    return o


def zeilen_text(m):
    return ['S5 %d  F %.0f  fremd %.1f/10k  C*sd %.1f  Spanne %.0f  Block %.0f' % (m['S5'], m['F'], m['F_fremd'], m['C_std'], m['spanne90'],
                                                                                 m['block_ton_streu'])]


def main():
    mod = rl.modelle()
    erg = {'weiss': {}, 'szene': {}, 'vorlage': {}}
    k_rund, k_fleck, k_ton = [], [], []
    for wn, az, el in rl.WINKEL4:
        r1, r2, r3 = [], [], []
        for key, titel, mo, clip, _o in mod:
            img, K = rl.weiss_id(mo, clip, az, el)
            msk = rl.haut(key, K)
            m, karte, v = M.masse(img, msk, 2, True)
            erg['weiss'].setdefault(key, {})[wn] = m
            if key == 'EM30':
                erg.setdefault('weiss_em30_mit_kleidung', {})[wn] = M.masse(img, K['maske'], 2)
            r1.append(kachel(img, '%s, %s %d/%d' % (titel, wn, az, el), zeilen_text(m)))
            r2.append(kachel(fleck_bild(img, karte), '%s, %s: Flecken cyan L*, magenta Ton, gelb beides, ROT farbfremd' % (key, wn)))
            r3.append(kachel(ton_bild(img, v), '%s, %s: Farbtonsektor je Pixel' % (key, wn)))
        k_rund.append(reihe(r1))
        k_fleck.append(reihe(r2))
        k_ton.append(reihe(r3))
        print('weiss', wn, flush=True)
    for key in list(erg['weiss']):
        ws = [erg['weiss'][key][w[0]] for w in rl.WINKEL4]
        erg['weiss'][key]['MITTEL'] = M.mittel(ws)
    speichern('r1_weiss_rundum.png', stapel(k_rund, 'R1 Weisses Licht wie Familien-Tafel 07 (0,55 + 0,45 N.L, Umkugel 0,46, 380 px), roh, kein Gamma: '
                                            'RE2-G4 | EM30 | K5c | K5d, je 4 Winkel, Ruhe-Clip. Zahlen: Ruhe-Masse dieser Kachel.'))
    speichern('r2_weiss_flecken.png', stapel(k_fleck, 'R2 Flecken-Karte (Mass b): Komponenten gleicher Klasse 6 px..5 %, Ring 4 px, '
                                             'cyan = L* weicht >= 8 ab, magenta = Farbton >= 15 Grad, gelb = beides; Figur grau x 0,6.'))
    speichern('r3_weiss_farbtonkarte.png', stapel(k_ton, 'R3 Farbtonsektor je Figurpixel (15-Grad-Sektoren, Kartenfarbe L 60 / C 45), '
                                                  'grau = unbunt (C* < 6). Ruhig = wenige Sektoren in grossen Flaechen.'))
    ausschnitt, nah = {}, {}
    for n in (1, 2):
        vor = rl.v4f.original(n)
        um = np.zeros((480, 640), bool)
        for _a, mm in rl.v4f.PZ.regionen(n):
            um |= mm
        um[428:, :262] = False
        erg['vorlage']['bild%d' % n] = M.masse(vor, um, 3)
        zs = [kachel(vor, 'Vorlage Bild %d (Video-Standbild)' % n, zeilen_text(erg['vorlage']['bild%d' % n]))]
        zv = [kachel(vor, 'Vorlage Bild %d (Video-Standbild)' % n, ['Nutzer-Teilmasken ohne Logo, nur Info'])]
        for key, titel, mo, clip, ordner in mod:
            s, K = rl.szene_k(mo, n, ordner)
            msk = rl.haut(key, K)
            ms = M.masse(s, msk, 2)
            vids = [rl.v4f.video(s, n, sa) for sa in rl.v4f.SAATEN]
            mv = M.mittel([M.masse(vv, msk, 3) for vv in vids])
            erg['szene'].setdefault(key, {})['bild%d' % n] = dict(ohne_filter=ms, video=mv)
            zs.append(kachel(s, '%s: Disc-Licht x k ohne Filter' % key, zeilen_text(ms)))
            zv.append(kachel(vids[0], '%s: + Videofilter Saat 15' % key,
                             ['3 Saaten: S5 %.1f  F %.0f  fremd %.1f  Spanne %.0f  Block %.0f' % (mv['S5'], mv['F'], mv['F_fremd'], mv['spanne90'],
                                                                                             mv['block_ton_streu'])]))
            ausschnitt.setdefault(n, []).append(msk)
            nah.setdefault(n, []).append((key, s, vids[0]))
            print('szene', n, key, flush=True)
        speichern('r%d_szene_bild%d.png' % (3 + n, n), stapel(
            [reihe(zs), reihe(zv)],
            'R%d Szenenkette Bild %d (%s): Vorlage | RE2-G4 Clip 0 | EM30 Clip 0 (beide an Lage/Gier der K5c) | K5c | K5d. Zeile 1 ohne Filter, '
            'Zeile 2 Videofilter. Roh.' % (3 + n, n, '5080' if n == 1 else '5090 Schnitt 11')))
    # Nahkacheln der Szenen (x2 nearest, Ausschnitt = Huellrechteck aller Figuren + 8 px), roh
    for n in (1, 2):
        u = np.any(np.stack(ausschnitt[n]), 0)
        ys, xs = np.nonzero(u)
        x0, x1, y0, y1 = max(xs.min() - 8, 0), min(xs.max() + 8, 640), max(ys.min() - 8, 0), min(ys.max() + 8, 480)
        f = 2 if (x1 - x0) <= 330 else 1
        gr = lambda im: cv2.resize(im[y0:y1, x0:x1], ((x1 - x0) * f, (y1 - y0) * f), interpolation=cv2.INTER_NEAREST)
        z1 = [kachel(gr(rl.v4f.original(n)), 'Vorlage Bild %d' % n)] + [kachel(gr(s_), '%s ohne Filter' % k_) for k_, s_, _v in nah[n]]
        z2 = [kachel(gr(rl.v4f.original(n)), 'Vorlage Bild %d' % n)] + [kachel(gr(v_), '%s Videofilter Saat 15' % k_) for k_, _s, v_ in nah[n]]
        speichern('r%d_szene_bild%d_nah.png' % (6 + n, n), stapel([reihe(z1), reihe(z2)], 'R%d Szene Bild %d nah (x%d nearest, roh): Vorlage | G4 | EM30 | '
                                                                  'K5c | K5d' % (6 + n, n, f)))
    kh = []
    for key, titel, *_r in mod:
        im = np.full((200, 24 * 14 + 20, 3), 24, np.uint8)
        acc = np.zeros(24)
        for wn in [w[0] for w in rl.WINKEL4]:
            d = erg['weiss'][key][wn]
            for s, p in d['sektoren'].items():
                acc[int(s) // 15] += p * d['px'] * d['bunt_pct'] / 1e4
        acc = acc / max(acc.sum(), 1e-9)
        erg['weiss'][key]['hist_gepoolt'] = {str(i * 15): round(100 * float(acc[i]), 1) for i in range(24) if acc[i] >= 0.005}
        erg['weiss'][key]['S5_gepoolt'] = int((acc >= 0.05).sum())
        top = max(acc.max(), 1e-9)
        for i in range(24):
            hh = int(150 * acc[i] / top)
            hm = i * 15 + 7.5
            c = np.clip(cv2.cvtColor(np.array([[[60, 45 * math.cos(math.radians(hm)), 45 * math.sin(math.radians(hm))]]], np.float32),
                                     cv2.COLOR_LAB2RGB)[0, 0] * 255, 0, 255).astype(int).tolist()
            cv2.rectangle(im, (10 + i * 14, 170 - hh), (10 + i * 14 + 11, 170), c, -1)
            if acc[i] >= 0.05:
                cv2.rectangle(im, (10 + i * 14, 170 - hh), (10 + i * 14 + 11, 170), (255, 255, 255), 1)
        y5 = 170 - int(150 * 0.05 / top)
        cv2.line(im, (8, y5), (24 * 14 + 12, y5), (120, 120, 120), 1)
        text(im, '0', 8, 188, 0.35)
        text(im, '180', 10 + 12 * 14, 188, 0.35)
        text(im, '345', 10 + 23 * 14 - 6, 188, 0.35)
        kh.append(kachel(im, '%s: Sektoren >= 5 %%: %d' % (key, erg['weiss'][key]['S5_gepoolt'])))
    speichern('r6_farbton_histogramm.png', stapel([reihe(kh)], 'R6 Farbtonhaeufigkeit (weisses Licht, 4 Winkel gepoolt, C* >= 6) je 15-Grad-Sektor; '
                                                  'graue Linie = 5 %.'))
    json.dump(erg, open(os.path.join(rl.OUT, 'ruhe_mass.json'), 'w', encoding='utf-8'), indent=1, ensure_ascii=False)
    print('fertig')


if __name__ == '__main__':
    main()
