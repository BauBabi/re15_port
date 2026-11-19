# Messbild: Kartenabzuege nebeneinander, 3x, mit Kopfzeile "Messbild - kein Ergebnis". Aufruf: mess.py aus.png titel bmp=label ...
import sys
from PIL import Image, ImageDraw, ImageFont
aus, titel = sys.argv[1], sys.argv[2]
teile = [a.split('=', 1) for a in sys.argv[3:]]
SC = int(__import__('os').environ.get('SC', '2'))
ims = [Image.open(p).convert('RGB') for p, _ in teile]
w = max(i.size[0] for i in ims) * SC; h = max(i.size[1] for i in ims) * SC
cols = min(len(ims), int(__import__('os').environ.get('COLS', '3'))); rows = (len(ims) + cols - 1) // cols
out = Image.new('RGB', (cols * (w + 8), 34 + rows * (h + 26)), (20, 20, 20)); d = ImageDraw.Draw(out)
fb = ImageFont.truetype('C:/Windows/Fonts/consolab.ttf', 20); f = ImageFont.truetype('C:/Windows/Fonts/consola.ttf', 16)
d.rectangle([0, 0, out.size[0], 30], fill=(160, 0, 0)); d.text((8, 4), 'Messbild - kein Ergebnis   ' + titel, font=fb, fill=(255, 255, 255))
for k, (im, (_, lab)) in enumerate(zip(ims, teile)):
    x = (k % cols) * (w + 8); y = 34 + (k // cols) * (h + 26)
    d.text((x + 4, y + 2), lab, font=f, fill=(255, 255, 0))
    out.paste(im.resize((im.size[0] * SC, im.size[1] * SC), Image.NEAREST), (x, y + 24))
out.save(aus); print('ok', aus)
