#!/usr/bin/env python3
"""Einbau v0.8.30 (Nutzerentscheid E30): CLUT-Zeilen einer Variante auf hoechstens ZIEL (16) bringen, ohne sichtbare Aenderung.

    python kf_zeilen16.py <Variante> <aus-Ordner> [--ziel 16] [--json aus.json]

WARUM 16: der PC-Port legt je CLUT-Zeile eine Kopie der Textur untereinander (re15_port/platform/pc/src/render_pc.c
re15_render_pc_upload_tim_slot: n_cluts = clut_entries / 256 bei 8 bpp, tex_h = tim->height * n_cluts, keine Maximalpruefung;
SDL_CreateTexture gibt ueber dem Renderer-Maximum NULL zurueck, GLES2-Geraete melden oft GL_MAX_TEXTURE_SIZE 4096, der Fehler wird
dort nicht geloggt). 17 x 256 = 4352 > 4096. Der Port haelt anderswo 16 Zeilen = 4096 als Grenze (re2fx_pc.c:35-45,
fx_plattform_pc.h:106-110, gator_fx_r41_pc.c:79).

VERFAHREN (Auftrag Einbau Runde 1):
1. Zaehlen, welche CLUT-Zeile jede Flaeche (Dreieck/Viereck, alle Meshes) ueber ihr clut-Wort liest: Zeile = ((clut >> 6) & 0x1ff) - clut_y
   (render_pc.c re15_render_pc_push_tri: clut_idx = clut_y - s_tim_clut_base_y).
2. Unreferenzierte Zeilen entfallen (Flaechen-Zeilen werden nachgezogen) - keine Farbe aendert sich, kein Texel wird anders aufgeloest.
3. Reicht das nicht, werden die zwei Zeilen mit dem kleinsten Farbabstand (max dE76 je Eintrag ueber alle 256 Eintraege, 15-Bit-Farben als
   sRGB -> Lab) zusammengelegt: die Flaechen der Zeile mit weniger Flaechen wandern auf die andere. Der Bericht nennt max dE und die Zahl der
   betroffenen Flaechen/Texel. (In Runde 5 nicht noetig: Zeile 2 ist unreferenziert.)
Geometrie, Knochen, Clips, UV, TPage und Pixel bleiben byte-gleich (Rundlauf form_emd lese/schreibe ist byte-gleich, geprueft).
"""
import argparse
import json
import os
import shutil
import sys

import numpy as np

HIER = os.path.dirname(os.path.abspath(__file__))
WURZEL = os.path.abspath(os.path.join(HIER, '../../../../../..'))
sys.path.insert(0, os.path.join(WURZEL, 're15_port/tools'))
import form_emd as FE  # noqa: E402


def srgb_lab(rgb):
    c = np.asarray(rgb, float) / 255.0
    c = np.where(c <= 0.04045, c / 12.92, ((c + 0.055) / 1.055) ** 2.4)
    M = np.array([[0.4124564, 0.3575761, 0.1804375], [0.2126729, 0.7151522, 0.0721750], [0.0193339, 0.1191920, 0.9503041]])
    xyz = c @ M.T / np.array([0.95047, 1.0, 1.08883])
    f = np.where(xyz > 0.008856, np.cbrt(xyz), 7.787 * xyz + 16.0 / 116.0)
    return np.stack([116 * f[..., 1] - 16, 500 * (f[..., 0] - f[..., 1]), 200 * (f[..., 1] - f[..., 2])], -1)


def clut_rgb(zeile):
    z = np.asarray(zeile, np.uint32)
    r, g, b = z & 31, (z >> 5) & 31, (z >> 10) & 31
    return np.stack([r, g, b], -1) * 255.0 / 31.0


def flaechen(form):
    for mi, m in enumerate(form['meshes']):
        for art in ('dreiecke', 'vierecke'):
            for f in m[art]:
                yield mi, art, f


def zeile_von(f, clut_y):
    return ((int(f['clut']) >> 6) & 0x1ff) - clut_y


def zaehlen(form, tim):
    n = len(tim['cluts'])
    cnt = [0] * n
    meshes = [set() for _ in range(n)]
    for mi, art, f in flaechen(form):
        z = zeile_von(f, tim['clut_y'])
        if not 0 <= z < n:
            raise SystemExit('Flaeche in Mesh %d liest Zeile %d ausserhalb 0..%d' % (mi, z, n - 1))
        cnt[z] += 1
        meshes[z].add(mi)
    return cnt, [sorted(m) for m in meshes]


def texel_der_zeile(form, tim, z):
    """Zahl der Texel (Atlas-Positionen), die Flaechen der Zeile z lesen (Bbox-Fuellung je Flaeche, Vereinigung)."""
    h, w = tim['pixel'].shape
    maske = np.zeros((h, w), bool)
    for mi, art, f in flaechen(form):
        if zeile_von(f, tim['clut_y']) != z:
            continue
        xs = [u[0] + (int(f['page']) & 0xF) * 128 for u in f['uv']]
        ys = [u[1] for u in f['uv']]
        maske[max(0, min(ys)):min(h, max(ys) + 1), max(0, min(xs)):min(w, max(xs) + 1)] = True
    return int(maske.sum())


def umhaengen(form, tim, abbildung):
    """abbildung: alte Zeile -> neue Zeile. Setzt das clut-Wort jeder Flaeche neu (clut_x-Bits bleiben)."""
    n_fl = 0
    for mi, art, f in flaechen(form):
        alt = zeile_von(f, tim['clut_y'])
        neu = abbildung[alt]
        if neu != alt:
            n_fl += 1
        f['clut'] = ((tim['clut_y'] + neu) << 6) | (int(f['clut']) & 0x3f)
    return n_fl


def main():
    a = argparse.ArgumentParser()
    a.add_argument('variante')
    a.add_argument('aus')
    a.add_argument('--ziel', type=int, default=16)
    a.add_argument('--json', default=None)
    g = a.parse_args()
    emd = open(os.path.join(g.variante, 'FORM.EMD'), 'rb').read()
    timb = open(os.path.join(g.variante, 'FORM.TIM'), 'rb').read()
    form = FE.lese_emd(emd)
    tim = FE.tim_lesen(timb)
    if FE.schreibe_emd(form) != emd:
        raise SystemExit('Rundlauf EMD nicht byte-gleich')
    n0 = len(tim['cluts'])
    cnt, meshes = zaehlen(form, tim)
    ber = dict(variante=os.path.abspath(g.variante), zeilen_vorher=n0, ziel=g.ziel,
               referenzen=[dict(zeile=z, flaechen=cnt[z], meshes=meshes[z]) for z in range(n0)],
               entfernt=[], zusammengelegt=[])
    print('Zeilen %d, Flaechen je Zeile: %s' % (n0, ' '.join('%d:%d' % (z, cnt[z]) for z in range(n0))))

    # 1. unreferenzierte Zeilen entfernen
    behalten = [z for z in range(n0) if cnt[z] > 0]
    weg = [z for z in range(n0) if cnt[z] == 0]
    if weg:
        print('unreferenziert: %s -> entfallen' % weg)
    ber['entfernt'] = weg
    # 2. wenn noetig zusammenlegen
    lab = srgb_lab(np.array([clut_rgb(z) for z in tim['cluts']]))     # n x 256 x 3
    ziel_von = {z: z for z in behalten}
    while len(behalten) > g.ziel:
        best = None
        for i in range(len(behalten)):
            for j in range(i + 1, len(behalten)):
                zi, zj = behalten[i], behalten[j]
                d = float(np.sqrt(((lab[zi] - lab[zj]) ** 2).sum(-1)).max())
                if best is None or d < best[0]:
                    best = (d, zi, zj)
        d, zi, zj = best
        quelle, ziel = (zi, zj) if cnt[zi] <= cnt[zj] else (zj, zi)
        n_tex = texel_der_zeile(form, tim, quelle)
        print('zusammenlegen: Zeile %d -> %d, max dE76 %.2f, %d Flaechen, %d Texel' % (quelle, ziel, d, cnt[quelle], n_tex))
        ber['zusammengelegt'].append(dict(von=quelle, nach=ziel, max_dE76=round(d, 3), flaechen=cnt[quelle], texel=n_tex))
        for k, v in ziel_von.items():
            if v == quelle:
                ziel_von[k] = ziel
        cnt[ziel] += cnt[quelle]
        cnt[quelle] = 0
        behalten.remove(quelle)
    neu_index = {z: i for i, z in enumerate(behalten)}
    abbildung = {z: neu_index[ziel_von[z]] for z in ziel_von}
    for z in weg:
        abbildung[z] = -1                      # nie gelesen
    n_fl = umhaengen(form, tim, {z: (abbildung[z] if abbildung[z] >= 0 else 0) for z in range(n0)})
    cluts = [tim['cluts'][z] for z in behalten]
    ber.update(zeilen_nachher=len(cluts), abbildung={str(k): v for k, v in sorted(abbildung.items())}, flaechen_umgehaengt=n_fl,
               port_texturhoehe=tim['pixel'].shape[0] * len(cluts))
    os.makedirs(g.aus, exist_ok=True)
    emd2 = FE.schreibe_emd(form)
    tim2 = FE.tim_schreiben(tim['pixel'], cluts, tim['clut_x'], tim['clut_y'], tim['bild_x'], tim['bild_y'])
    open(os.path.join(g.aus, 'FORM.EMD'), 'wb').write(emd2)
    open(os.path.join(g.aus, 'FORM.TIM'), 'wb').write(tim2)
    fj = os.path.join(g.variante, 'form.json')
    if os.path.exists(fj):
        j = json.load(open(fj, encoding='utf-8'))
        j['name'] = os.path.basename(os.path.normpath(g.aus))
        json.dump(j, open(os.path.join(g.aus, 'form.json'), 'w', encoding='utf-8'), indent=1)
    # Gegenprobe: ausser den clut-Worten ist nichts anders
    f0 = FE.lese_emd(emd)
    gleich = (f0['knochen'] == form['knochen'] and f0['pool'] == form['pool'] and f0['clips'] == form['clips'] and
              all(m0['ecken'] == m1['ecken'] and m0['normalen'] == m1['normalen'] for m0, m1 in zip(f0['meshes'], form['meshes'])) and
              all([(x['e'], x['n'], x['uv'], x['page']) for x in m0[art]] == [(x['e'], x['n'], x['uv'], x['page']) for x in m1[art]]
                  for m0, m1 in zip(f0['meshes'], form['meshes']) for art in ('dreiecke', 'vierecke')))
    ber['rest_bytegleich'] = bool(gleich and (FE.tim_lesen(tim2)['pixel'] == tim['pixel']).all())
    print('Zeilen nachher %d, Port-Texturhoehe %d, Flaechen umgehaengt %d, Rest gleich: %s' % (
        len(cluts), ber['port_texturhoehe'], n_fl, ber['rest_bytegleich']))
    with open(g.json or os.path.join(g.aus, 'zeilen.json'), 'w', encoding='utf-8') as fh:
        json.dump(ber, fh, indent=1)
    print(os.path.join(g.aus, 'FORM.EMD'), os.path.join(g.aus, 'FORM.TIM'))


if __name__ == '__main__':
    main()
