# Messbild-Tafel: Liste von (bild, crop, titel) -> PNG mit roter Kopfzeile "Messbild - kein Ergebnis".
import sys
from PIL import Image, ImageDraw, ImageFont
f = ImageFont.truetype('C:/Windows/Fonts/consola.ttf', 15); fb = ImageFont.truetype('C:/Windows/Fonts/consolab.ttf', 20)
def tafel(teile, aus, titel, sk=2, sp=3, marks=None):
    ims = []
    for t in teile:
        im = Image.open(t[0]).convert('RGB')
        if len(t) > 1 and t[1]: im = im.crop(t[1])
        im = im.resize((im.width * sk, im.height * sk), Image.NEAREST)
        d = ImageDraw.Draw(im)
        for (x, y, c, lab) in (t[3] if len(t) > 3 else []):
            ox = t[1][0] if len(t) > 1 and t[1] else 0; oy = t[1][1] if len(t) > 1 and t[1] else 0
            X, Y = (x - ox) * sk + sk // 2, (y - oy) * sk + sk // 2
            d.ellipse([X - 4, Y - 4, X + 4, Y + 4], outline=c, width=2)
            if lab: d.text((X + 6, Y - 8), lab, fill=c, font=f)
        ims.append((im, t[2]))
    w = max(i.width for i, _ in ims); h = max(i.height for i, _ in ims)
    n = len(ims); rows = (n + sp - 1) // sp
    out = Image.new('RGB', (sp * (w + 10) + 10, 44 + rows * (h + 28)), (10, 10, 10)); d = ImageDraw.Draw(out)
    d.rectangle([0, 0, out.width, 36], fill=(160, 0, 0)); d.text((8, 7), 'Messbild - kein Ergebnis   ' + titel, fill=(255, 255, 255), font=fb)
    for k, (im, lab) in enumerate(ims):
        x = 10 + (k % sp) * (w + 10); y = 44 + (k // sp) * (h + 28)
        d.text((x, y), lab, fill=(255, 255, 160), font=f); out.paste(im, (x, y + 20))
    out.save(aus)
if __name__ == '__main__':
    pass
