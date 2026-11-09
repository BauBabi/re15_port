"""Zeichenhilfen fuer die Nutzer-Tafeln (PIL, TTF-Schrift mit Umlauten)."""
import numpy as np, cv2
from PIL import Image, ImageDraw, ImageFont

FONT = 'C:/Windows/Fonts/arial.ttf'
FONTB = 'C:/Windows/Fonts/arialbd.ttf'
_f = {}


def schrift(gr, fett=False):
    k = (gr, fett)
    if k not in _f:
        _f[k] = ImageFont.truetype(FONTB if fett else FONT, gr)
    return _f[k]


# gut unterscheidbare Falschfarben (Glasbey-aehnlich), RGB
PALETTE = [(230, 25, 75), (60, 180, 75), (255, 205, 0), (0, 130, 200), (245, 130, 48), (145, 30, 180), (70, 220, 230), (240, 50, 230),
           (170, 220, 40), (250, 160, 190), (0, 128, 128), (190, 150, 255), (170, 110, 40), (120, 0, 0), (100, 255, 160), (128, 128, 0),
           (255, 190, 140), (0, 0, 160), (255, 90, 90), (40, 90, 40), (200, 0, 120), (90, 160, 255), (220, 220, 110), (100, 60, 160),
           (0, 200, 120), (255, 120, 0), (150, 40, 60), (40, 200, 200), (180, 80, 255), (120, 200, 60), (255, 60, 160), (60, 60, 60),
           (140, 230, 255), (200, 120, 80), (90, 120, 0), (255, 230, 180), (0, 90, 150), (210, 40, 40), (160, 160, 255), (80, 40, 0)]


def farbe(i):
    return PALETTE[i % len(PALETTE)]


def auf_weiss(rgba_bgr):
    a = rgba_bgr[..., 3:4] / 255.0
    return (rgba_bgr[..., :3][..., ::-1] * a + 255 * (1 - a)).astype(np.uint8)


def modell_grau(rgb, maske, hell=0.55):
    """Modellrender als ruhiges Grau (aufgehellt), Hintergrund weiss."""
    g = cv2.cvtColor(rgb, cv2.COLOR_RGB2GRAY).astype(np.float32)
    g = np.clip(110 + g * 1.6, 0, 235)
    out = np.full(rgb.shape, 255, np.uint8)
    out[maske] = np.repeat(g[..., None], 3, 2)[maske].astype(np.uint8)
    return out


def einfaerben(bild, maske, rgb, alpha=0.72):
    if maske.any():
        bild[maske] = (bild[maske] * (1 - alpha) + np.array(rgb) * alpha).astype(np.uint8)


def kontur(bild, maske, rgb, dicke=2):
    cnt, _ = cv2.findContours(maske.astype(np.uint8), cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_NONE)
    cv2.drawContours(bild, cnt, -1, tuple(int(c) for c in rgb), dicke, lineType=cv2.LINE_AA)


def gestrichelt(bild, maske, rgb, dicke=2, l=6):
    cnt, _ = cv2.findContours(maske.astype(np.uint8), cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_NONE)
    for c in cnt:
        c = c[:, 0, :]
        for i in range(0, len(c) - 1, 2 * l):
            seg = c[i:i + l]
            if len(seg) > 1:
                cv2.polylines(bild, [seg.reshape(-1, 1, 2)], False, tuple(int(x) for x in rgb), dicke, lineType=cv2.LINE_AA)


def schraffur(bild, maske, rgb, abstand=7):
    H, W = maske.shape
    yy, xx = np.mgrid[0:H, 0:W]
    m = maske & (((xx + yy) % abstand) < 2)
    bild[m] = rgb


def marken(img_pil, punkte, gr=20):
    """punkte: [(x, y, text, rgb)] -> Kreis mit Nummer."""
    d = ImageDraw.Draw(img_pil)
    for x, y, t, rgb in punkte:
        f = schrift(gr, True)
        w = d.textlength(t, font=f)
        r = max(gr * 0.75, w / 2 + 5)
        d.ellipse([x - r, y - r, x + r, y + r], fill=(255, 255, 255), outline=tuple(rgb), width=3)
        d.text((x - w / 2, y - gr * 0.58), t, fill=(0, 0, 0), font=f)


def text(img_pil, xy, t, gr=22, fett=False, rgb=(0, 0, 0)):
    ImageDraw.Draw(img_pil).text(xy, t, fill=rgb, font=schrift(gr, fett))


def umbruch(t, gr, breite, fett=False):
    f = schrift(gr, fett); d = ImageDraw.Draw(Image.new('RGB', (10, 10)))
    zeilen = []
    for absatz in t.split('\n'):
        z = ''
        for w in absatz.split(' '):
            v = (z + ' ' + w).strip()
            if d.textlength(v, font=f) <= breite:
                z = v
            else:
                if z:
                    zeilen.append(z)
                z = w
        zeilen.append(z)
    return zeilen


def tabelle(breite, spalten, zeilen, gr=21, kopf_gr=22, farben=None, markierung=None):
    """spalten: [(titel, anteil)], zeilen: [[str,...]], farben: [rgb] je Zeile (Farbfeld vorn), markierung: [str] je Zeile (rechts, rot)."""
    pad = 10; feld = 34
    ws = [int((breite - feld - 2 * pad) * a) for _, a in spalten]
    # Hoehe bestimmen
    def zh(z):
        return max(len(umbruch(c, gr, w - 12)) for c, w in zip(z, ws)) * int(gr * 1.28) + 12
    hk = max(len(umbruch(t, kopf_gr, w - 12, True)) for (t, _), w in zip(spalten, ws)) * int(kopf_gr * 1.3) + 14
    H = hk + sum(zh(z) for z in zeilen) + 2 * pad
    im = Image.new('RGB', (breite, H), (255, 255, 255)); d = ImageDraw.Draw(im)
    x = pad + feld; y = pad
    d.rectangle([pad, y, breite - pad, y + hk], fill=(235, 235, 235))
    for (t, _), w in zip(spalten, ws):
        for k, l in enumerate(umbruch(t, kopf_gr, w - 12, True)):
            d.text((x + 6, y + 7 + k * int(kopf_gr * 1.3)), l, fill=(0, 0, 0), font=schrift(kopf_gr, True))
        x += w
    y += hk
    for i, z in enumerate(zeilen):
        h = zh(z)
        if i % 2:
            d.rectangle([pad, y, breite - pad, y + h], fill=(248, 248, 248))
        if farben and farben[i] is not None:
            d.rectangle([pad + 4, y + 7, pad + feld - 6, y + 7 + feld - 12], fill=tuple(farben[i]), outline=(0, 0, 0))
        x = pad + feld
        for j, (c, w) in enumerate(zip(z, ws)):
            rgb = (0, 0, 0)
            if markierung and markierung[i] and j == len(z) - 1:
                rgb = (190, 0, 0)
            for k, l in enumerate(umbruch(c, gr, w - 12)):
                d.text((x + 6, y + 6 + k * int(gr * 1.28)), l, fill=rgb, font=schrift(gr))
            x += w
        d.line([pad, y + h, breite - pad, y + h], fill=(210, 210, 210))
        y += h
    return im


def nebeneinander(bilder, titel, gr=26, abstand=24, grund=(255, 255, 255)):
    hs = max(b.shape[0] for b in bilder); kopf = int(gr * 1.9)
    W = sum(b.shape[1] for b in bilder) + abstand * (len(bilder) + 1)
    im = Image.new('RGB', (W, hs + kopf + abstand), grund); x = abstand
    d = ImageDraw.Draw(im)
    for b, t in zip(bilder, titel):
        im.paste(Image.fromarray(b), (x, kopf))
        for k, l in enumerate(umbruch(t, gr, b.shape[1], True)[:2]):
            d.text((x, 6 + k * int(gr * 0.95)), l, fill=(0, 0, 0), font=schrift(gr if k == 0 else int(gr * 0.8), k == 0))
        d.rectangle([x - 1, kopf - 1, x + b.shape[1], kopf + b.shape[0]], outline=(180, 180, 180))
        x += b.shape[1] + abstand
    return im


def stapeln(teile, grund=(255, 255, 255), abstand=16):
    W = max(t.size[0] for t in teile); H = sum(t.size[1] for t in teile) + abstand * (len(teile) - 1)
    im = Image.new('RGB', (W, H), grund); y = 0
    for t in teile:
        im.paste(t, (0, y)); y += t.size[1] + abstand
    return im


def titelzeile(breite, t, unter=None, gr=34):
    zeilen_u = umbruch(unter, 22, breite - 40) if unter else []
    H = int(gr * 1.5) + len(zeilen_u) * 28 + 16
    im = Image.new('RGB', (breite, H), (255, 255, 255)); d = ImageDraw.Draw(im)
    d.text((20, 10), t, fill=(0, 0, 0), font=schrift(gr, True))
    for k, l in enumerate(zeilen_u):
        d.text((20, int(gr * 1.5) + 6 + k * 28), l, fill=(60, 60, 60), font=schrift(22))
    return im


def marken_seitlich(bild, punkte, gr=20, rand=None, abstand=None):
    """Nummern in Randspalten links/rechts mit Fuehrungslinie zum Ankerpunkt (ruhiger als Marken auf der Figur)."""
    H, W = bild.shape[:2]
    rand = rand or int(gr * 3.2); abstand = abstand or int(gr * 2.2)
    out = np.full((H, W + 2 * rand, 3), 255, np.uint8); out[:, rand:rand + W] = bild
    im = Image.fromarray(out); d = ImageDraw.Draw(im)
    mitte = W / 2.0
    for seite in (0, 1):
        ps = sorted([p for p in punkte if (p[0] >= mitte) == bool(seite)], key=lambda p: p[1])
        ys = []
        for p in ps:
            y = max(p[1], (ys[-1] + abstand) if ys else gr)
            ys.append(y)
        # nach oben schieben, falls unten ueberlaeuft
        for k in range(len(ys) - 1, -1, -1):
            grenze = H - gr if k == len(ys) - 1 else ys[k + 1] - abstand
            ys[k] = min(ys[k], grenze)
        for p, y in zip(ps, ys):
            mx = rand / 2 if seite == 0 else rand + W + rand / 2
            ax = p[0] + rand
            d.line([(mx, y), (ax, p[1])], fill=tuple(int(c) for c in p[3]), width=3)
            d.ellipse([ax - 4, p[1] - 4, ax + 4, p[1] + 4], fill=(0, 0, 0))
        marken(im, [(rand / 2 if seite == 0 else rand + W + rand / 2, y, p[2], p[3]) for p, y in zip(ps, ys)], gr)
    return np.array(im)
