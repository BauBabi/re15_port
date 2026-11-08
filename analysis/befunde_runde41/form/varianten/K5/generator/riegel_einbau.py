#!/usr/bin/env python3
"""Riegel Einbau v0.8.30 (Nutzerentscheid E30): 16-Zeilen-Basis gegen die 17-Zeilen-Variante und gegen das Spiel-Asset. Exit 1 bei Rot.

    python riegel_einbau.py <Basis16> --variante17 <K5c_farbe_v3> [--asset <BAHNSTEIG_FORM.EMD> <BAHNSTEIG_FORM.TIM>]
                            [--spiel A.png B.png ...] [--json aus.json] [--tafel aus.png]

Zeilen (bindend):
  E-Z1  CLUT-Zeilen <= 16 und Port-Texturhoehe <= 4096. Port-Formel re15_port/platform/pc/src/render_pc.c re15_render_pc_upload_tim_slot:
        n_cluts = clut_entries / 256 (8 bpp; clut_entries = clut_w * clut_h, engine/src/tim_common.c:59), tex_h = tim->height * n_cluts.
        Grenze 4096 = GL_MAX_TEXTURE_SIZE vieler GLES2-Geraete; der Port haelt 16 Zeilen = 4096 als Grenze (re2fx_pc.c:35-45,
        fx_plattform_pc.h:106-110, gator_fx_r41_pc.c:79). Gegenprobe: die 17-Zeilen-Variante muss hier ROT sein.
  E-Z2  Jede Flaeche liest eine gueltige CLUT-Zeile: 0 <= ((clut >> 6) & 0x1ff) - clut_y < n_cluts (render_pc.c re15_render_pc_push_tri;
        ausserhalb faellt der Port still auf Zeile 0).
  E-Z3  Render-Gegenprobe 17-Zeilen-Variante gegen Basis16 in den Tafel-Szenen (gleiche Kette, roh, kein Gamma):
        01/02 = form_pruefung.Kandidat.rendern(n) (Disc-Licht x k, Szene Bild 1/Bild 2, wie kf_tafeln.szene Z.1),
        05    = render.blick 6 Winkel (kf_tafeln.WINKEL6) x Clip 0 und Clip 2 (wie kf_tafeln.rundum),
        07    = echtes Spielfenster: Bildpaare aus --spiel (form_spiel.sh, gleiche Pose, Variante ueber RE15_RE2_ASSET_ROOT gegen Basis16).
        Bindend: Pixel-Diff 0 oder max dE76 <= 1 (Zahl und Ort werden genannt).
  E-Z4  (mit --asset) Spiel-Asset: MD1-Sektion byte-gleich Basis16, TIM byte-gleich Basis16, 32 Meshes / 32 Knochen / 28 Clips
        (test_birkin_bahnsteig.c test_asset; Tick-Clips birkin_bahnsteig.c BB_C_*).
"""
import argparse
import json
import os
import struct
import sys

import cv2
import numpy as np

HIER = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HIER)
import kf_tafeln as KT  # noqa: E402  (Pfade farbe_lesung/B_skripte + P_skripte, render, fl, p_zonen)
import kf_zeilen16 as KZ  # noqa: E402

FE = KZ.FE
R, fl, PZ = KT.R, KT.fl, KT.PZ
MAX_TEX = 4096     # re2fx_pc.c:35-45, fx_plattform_pc.h:106-110, gator_fx_r41_pc.c:79 (16 Zeilen x 256)
MAX_ZEILEN = 16


def tim_kopf(timb):
    """Wie engine/src/tim_common.c: Flag, CLUT-Breite/-Hoehe, Bildhoehe."""
    _id, flag = struct.unpack_from('<II', timb, 0)
    cs, cx, cy, cw, ch = struct.unpack_from('<I4H', timb, 8)
    o = 8 + cs
    _ds, bx, by, bw, bh = struct.unpack_from('<I4H', timb, o)
    bpp = {8: 4, 9: 8, 2: 16}.get(flag, 0)
    breite = bw * 2 if bpp == 8 else (bw * 4 if bpp == 4 else bw)
    return dict(bpp=bpp, clut_x=cx, clut_y=cy, clut_w=cw, clut_h=ch, clut_entries=cw * ch, width=breite, height=bh)


def port_texturhoehe(timb):
    k = tim_kopf(timb)
    row = 16 if k['bpp'] == 4 else 256
    n = max(1, k['clut_entries'] // row) if k['clut_entries'] else 1
    return k['height'] * n, n, k


def ez1(timb):
    h, n, k = port_texturhoehe(timb)
    ok = n <= MAX_ZEILEN and h <= MAX_TEX
    return ok, 'Zeilen %d (<= %d), Port-Texturhoehe %d x %d = %d (<= %d)' % (n, MAX_ZEILEN, k['height'], n, h, MAX_TEX)


def ez2(emdb, timb):
    form = FE.lese_emd(emdb)
    k = tim_kopf(timb)
    n = max(1, k['clut_entries'] // 256)
    schlecht = []
    cnt = [0] * n
    for mi, art, f in KZ.flaechen(form):
        z = ((int(f['clut']) >> 6) & 0x1ff) - k['clut_y']
        if 0 <= z < n:
            cnt[z] += 1
        else:
            schlecht.append((mi, art, z))
    return not schlecht, '%d Flaechen, Zeilen %s%s' % (sum(cnt), ' '.join('%d:%d' % (i, c) for i, c in enumerate(cnt)),
                                                      (', UNGUELTIG %s' % schlecht[:5]) if schlecht else '')


def de76_bild(a, b):
    """Pixel-Diff und max dE76 zweier RGB-Bilder gleicher Groesse; Ort des Maximums."""
    if a.shape != b.shape:
        return dict(pixel_diff=-1, max_de=999.0, ort=None, text='Groesse %s gegen %s' % (a.shape, b.shape))
    d = np.any(a != b, -1)
    n = int(d.sum())
    if n == 0:
        return dict(pixel_diff=0, max_de=0.0, ort=None)
    la, lb = KZ.srgb_lab(a[d]), KZ.srgb_lab(b[d])
    de = np.sqrt(((la - lb) ** 2).sum(-1))
    i = int(de.argmax())
    ys, xs = np.nonzero(d)
    return dict(pixel_diff=n, max_de=float(de.max()), ort=[int(xs[i]), int(ys[i])])


def ez3_offline(var17, basis16):
    erg = {}
    bilder = {}
    kA = PZ.P.Kandidat(os.path.join(var17, 'form.json'))
    kB = PZ.P.Kandidat(os.path.join(basis16, 'form.json'))
    for n in (1, 2):
        sA = np.asarray(kA.rendern(n)[0], np.uint8)
        sB = np.asarray(kB.rendern(n)[0], np.uint8)
        erg['0%d Szene Bild %d' % (n, n)] = de76_bild(sA, sB)
        bilder['0%d' % n] = (sA, sB)
    moA, moB = fl.lade_var(var17), fl.lade_var(basis16)
    for nm, c in (('Clip 0', 0), ('Clip 2', 2)):
        for wt, az, el in KT.WINKEL6:
            iA, _ = R.blick(moA, R.kf(moA, c), az, el, 300, None)
            iB, _ = R.blick(moB, R.kf(moB, c), az, el, 300, None)
            iA, iB = np.asarray(iA, np.uint8), np.asarray(iB, np.uint8)
            erg['05 rundum %s %s' % (nm, wt)] = de76_bild(iA, iB)
            bilder.setdefault('05', []).append((iA, iB))
    return erg, bilder


def ez3_spiel(paare):
    erg = {}
    for i in range(0, len(paare) - 1, 2):
        a = cv2.imread(paare[i]); b = cv2.imread(paare[i + 1])
        if a is None or b is None:
            erg['07 Spiel %s' % os.path.basename(paare[i])] = dict(pixel_diff=-1, max_de=999.0, ort=None, text='Bild fehlt')
            continue
        erg['07 Spiel %s | %s' % (os.path.basename(paare[i]), os.path.basename(paare[i + 1]))] = de76_bild(
            cv2.cvtColor(a, cv2.COLOR_BGR2RGB), cv2.cvtColor(b, cv2.COLOR_BGR2RGB))
    return erg


def ez4(asset_emd, asset_tim, basis16):
    ae = open(asset_emd, 'rb').read(); at = open(asset_tim, 'rb').read()
    be = open(os.path.join(basis16, 'FORM.EMD'), 'rb').read(); bt = open(os.path.join(basis16, 'FORM.TIM'), 'rb').read()
    md1_gleich = FE.sektionen(ae)['md1'] == FE.sektionen(be)['md1']
    tim_gleich = at == bt
    fa = FE.lese_emd(ae)
    n_k, n_m, n_c = len(fa['knochen']), len(fa['meshes']), len(fa['clips'])
    ok = md1_gleich and tim_gleich and (n_k, n_m, n_c) == (32, 32, 28)
    return ok, 'MD1 byte-gleich %s, TIM byte-gleich %s, Knochen/Meshes/Clips %d/%d/%d (Soll 32/32/28)' % (
        md1_gleich, tim_gleich, n_k, n_m, n_c)


def tafel(bilder, aus):
    """Rohe Gegenueberstellung 17 | 16 | Diff x 32 je Szene (nur Beleg)."""
    zeilen = []
    for key in ('01', '02'):
        a, b = bilder[key]
        d = np.clip(np.abs(a.astype(int) - b.astype(int)).sum(-1) * 32, 0, 255).astype(np.uint8)
        zeilen.append(np.concatenate([a, b, np.stack([d] * 3, -1)], 1))
    reihe = []
    for a, b in bilder.get('05', []):
        d = np.clip(np.abs(a.astype(int) - b.astype(int)).sum(-1) * 32, 0, 255).astype(np.uint8)
        reihe.append(np.concatenate([a, b, np.stack([d] * 3, -1)], 0))
    if reihe:
        zeilen.append(np.concatenate(reihe, 1))
    w = max(z.shape[1] for z in zeilen)
    out = np.concatenate([np.pad(z, ((0, 0), (0, w - z.shape[1]), (0, 0))) for z in zeilen], 0)
    cv2.imwrite(aus, cv2.cvtColor(out, cv2.COLOR_RGB2BGR))


def main():
    a = argparse.ArgumentParser()
    a.add_argument('basis16')
    a.add_argument('--variante17', required=True)
    a.add_argument('--asset', nargs=2, default=None)
    a.add_argument('--spiel', nargs='*', default=[])
    a.add_argument('--json', default=None)
    a.add_argument('--tafel', default=None)
    a.add_argument('--ohne-render', action='store_true')
    g = a.parse_args()
    be = open(os.path.join(g.basis16, 'FORM.EMD'), 'rb').read(); bt = open(os.path.join(g.basis16, 'FORM.TIM'), 'rb').read()
    ve = open(os.path.join(g.variante17, 'FORM.EMD'), 'rb').read(); vt = open(os.path.join(g.variante17, 'FORM.TIM'), 'rb').read()
    res = {}
    rot = []

    def zeile(name, ok, text, bindend=True):
        res[name] = dict(ok=bool(ok), text=text)
        print('%-28s %s  %s' % (name, 'GRUEN' if ok else 'ROT', text))
        if bindend and not ok:
            rot.append(name)

    ok, t = ez1(bt); zeile('E-Z1 Basis16', ok, t)
    ok17, t17 = ez1(vt); zeile('E-Z1 Gegenprobe Variante17', not ok17, t17 + ' -> muss ROT sein: %s' % ('ROT' if not ok17 else 'GRUEN'))
    ok, t = ez2(be, bt); zeile('E-Z2 Basis16', ok, t)
    ok, t = ez2(ve, vt); zeile('E-Z2 Variante17 (Info)', ok, t, bindend=False)
    if not g.ohne_render:
        erg, bilder = ez3_offline(g.variante17, g.basis16)
        for k, v in erg.items():
            ok = v['pixel_diff'] == 0 or (0 <= v['pixel_diff'] and v['max_de'] <= 1.0)
            zeile('E-Z3 ' + k, ok, 'Pixel-Diff %d, max dE76 %.3f%s' % (v['pixel_diff'], v['max_de'],
                                                                     (', Ort x/y %s' % v['ort']) if v['ort'] else ''))
        if g.tafel:
            tafel(bilder, g.tafel)
    if g.spiel:
        for k, v in ez3_spiel(g.spiel).items():
            ok = v['pixel_diff'] == 0 or (0 <= v['pixel_diff'] and v['max_de'] <= 1.0)
            zeile('E-Z3 ' + k, ok, 'Pixel-Diff %d, max dE76 %.3f%s' % (v['pixel_diff'], v['max_de'],
                                                                     (', Ort x/y %s' % v['ort']) if v['ort'] else ''))
    if g.asset:
        ok, t = ez4(g.asset[0], g.asset[1], g.basis16); zeile('E-Z4 Spiel-Asset', ok, t)
    n_ok = sum(1 for v in res.values() if v['ok'])
    print('Riegel Einbau: %d/%d gruen, rot: %s' % (n_ok, len(res), rot or 'keine'))
    if g.json:
        with open(g.json, 'w', encoding='utf-8') as fh:
            json.dump(dict(basis16=os.path.abspath(g.basis16), variante17=os.path.abspath(g.variante17), zeilen=res, rot=rot), fh, indent=1)
    sys.exit(1 if rot else 0)


if __name__ == '__main__':
    main()
