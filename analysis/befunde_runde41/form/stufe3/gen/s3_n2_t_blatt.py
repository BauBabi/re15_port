"""Nachbesserung 2, Textur-Spur: Blaetter fuer den Nutzer.

    python s3_n2_t_blatt.py echt AUS.png SATZ ...          echtes Fenster (re15_port/build_n2/spiel/tex_<SATZ>_<raum>_g<gier>, letztes Bild),
                                                          je Ansicht eine Zeile, je Satz eine Spalte, 2-fach vergroessert auf die Figur
    python s3_n2_t_blatt.py heat AUS.png NAME=EMD,TIM[,naht] ...
                                                          Buehne (Spielkamera, Raumlicht): je Ansicht eine Zeile, je Satz Render | Texelbloecke |
                                                          Kantenspruenge an Gelenkkoerper-Grenzen
"""
import os, sys, glob, json
import numpy as np
from PIL import Image, ImageDraw
HIER = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HIER)
import s3_rig as S          # noqa: E402
import s3_n2_textur as T    # noqa: E402
import form_emd as FE       # noqa: E402

SPIEL = os.path.join(S.WURZEL, 're15_port', 'build_n2', 'spiel')
ECHT = [('5080', 0, 'seitlich rechts'), ('5080', 2048, 'seitlich links'), ('5080', 3072, 'vorn nah'), ('5090', 3874, 'Pose/Lage Bild 2'),
        ('5090', 802, 'seitlich')]
# Ausschnitte (x0, y0, x1, y1) im 640 x 480-Fenster, so gewaehlt, dass die Form ganz drin ist
BOX = {'5080': (0, 60, 560, 440), '5090': (100, 120, 520, 440)}


def letztes(name):
    b = sorted(glob.glob(os.path.join(SPIEL, name, 'bild', 'b_*.png')))
    return Image.open(b[-1]).convert('RGB') if b else None


def zeichne(tafeln, titel, aus, spalten):
    pad = 8
    W = max(sum(i.size[0] for i in ims) + pad * (len(ims) - 1) for _t, ims in tafeln)
    H = 52 + sum(ims[0].size[1] + 26 for _t, ims in tafeln)
    out = Image.new('RGB', (W, H), (18, 18, 22))
    d = ImageDraw.Draw(out)
    d.text((6, 6), titel, fill=(240, 200, 120))
    d.text((6, 26), 'Spalten: ' + ' | '.join(spalten), fill=(210, 210, 210))
    y = 52
    for text, ims in tafeln:
        d.text((6, y + 4), text, fill=(220, 220, 220))
        x = 0
        for i in ims:
            out.paste(i, (x, y + 22))
            x += i.size[0] + pad
        y += ims[0].size[1] + 26
    out.save(aus)
    print(aus, out.size)


def echt(aus, saetze):
    tafeln = []
    for raum, gier, wort in ECHT:
        ims = [letztes('tex_%s_%s_g%d' % (s, raum, gier)) for s in saetze]
        if any(i is None for i in ims):
            print('fehlt', raum, gier)
            continue
        ims = [i.crop(BOX[raum]) for i in ims]
        ims = [i.resize((i.size[0] * 2, i.size[1] * 2), Image.NEAREST) for i in ims]
        tafeln.append(('%s %s (Gier %d), Spielkamera %s' % (raum, wort, gier, '5080 Schnitt 4, Ruhe' if raum == '5080' else '5090 Schnitt 11, Drohen Bild 45'), ims))
    zeichne(tafeln, 'Nachbesserung 2, Textur: echtes Fenster (eigene exe-Kopie, gdigrab, Renderer des Nutzers), 2-fach. Asset per RE15_RE2_ASSET_ROOT.',
            aus, saetze)


def heat(aus, specs):
    from scipy import ndimage
    namen, saetze = [], []
    for sp in specs:
        n, emd, tim, naht = T.lade_satz(sp)
        namen.append(n)
        form = FE.lese_emd(emd)
        saetze.append((emd, tim, T.teil_karte(form, naht)))
    tafeln = []
    for an in (T.ANSICHTEN[0], T.ANSICHTEN[2], T.ANSICHTEN[3], T.ANSICHTEN[4], T.ANSICHTEN[5]):
        reihe, figs = [], []
        for emd, tim, tk in saetze:
            ren, alb, kenn, texel = T.ansicht(emd, tim, *an)
            _sp, _gr, bl = T.block_bild(alb, kenn, texel)
            fig = kenn >= 0
            blk = np.zeros_like(ren)
            blk[fig] = T.farbskala(ndimage.grey_dilation(bl, size=(3, 3))[fig], 120)
            # Gelenkkoerper-Grenzen: Sprung der oertlichen Mittel (2 px je Seite), rot > 25, blau <= 25
            gk = np.isin(kenn, [k for k, t in tk.items() if t == 'gelenk'])
            gr = (alb.astype(float) * 0.55).astype(np.uint8)
            A = alb.astype(float)
            for axis in (0, 1):
                def sl(a, o):
                    return a[o:a.shape[0] - 3 + o] if axis == 0 else a[:, o:a.shape[1] - 3 + o]
                k1, k0, l0, l1 = sl(kenn, 0), sl(kenn, 1), sl(kenn, 2), sl(kenn, 3)
                ok = (k1 >= 0) & (l1 >= 0) & (k1 == k0) & (l0 == l1) & (k0 != l0)
                d = np.abs((sl(A, 0) + sl(A, 1)) / 2 - (sl(A, 2) + sl(A, 3)) / 2).mean(-1)
                o = sl(gr, 1)
                g = sl(gk, 1) != sl(gk, 2)
                o[ok & ~g & (d > 25)] = (255, 220, 60)
                o[ok & g & (d > 25)] = (255, 60, 60)
                o[ok & g & (d <= 25)] = (60, 170, 255)
            reihe.append((ren, blk, gr))
            figs.append(fig)
        f = np.logical_or.reduce(figs)
        ys, xs = np.nonzero(f)
        y0, y1, x0, x1 = max(ys.min() - 6, 0), min(ys.max() + 6, 480), max(xs.min() - 6, 0), min(xs.max() + 6, 640)
        ims = []
        for ren, blk, gr in reihe:
            for a in (ren, blk, gr):
                ims.append(Image.fromarray(np.ascontiguousarray(a[y0:y1, x0:x1])))
        tafeln.append(('%s Schnitt %d, Clip %d Bild %d, Gier %d' % (an[0], an[1], an[3], an[4], an[5]), ims))
    sp = []
    for n in namen:
        sp += ['%s Render' % n, '%s Texelbloecke' % n, '%s Kanten' % n]
    zeichne(tafeln, 'Nachbesserung 2, Textur-Heatmap (Buehne = Port-Zeichenweg, Spielkamera, Raumlicht). Texelbloecke: Sprung an Texelgrenzen x '
            'Texelgroesse in px (schwarz-blau-gelb-weiss bis 120). Kanten (Albedo): rot = Gelenkkoerper-Grenze > 25, blau <= 25, gelb = '
            'sonstige Polygonkante > 25', aus, sp)


def main():
    was, aus = sys.argv[1], sys.argv[2]
    if was == 'echt':
        echt(aus, sys.argv[3:])
    else:
        heat(aus, sys.argv[3:])


if __name__ == '__main__':
    main()
