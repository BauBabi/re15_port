"""Nachbesserung 3: Ausschnitt zweier Kartenabzuege (vorher | nachher) nebeneinander, Nearest-Neighbour vergroessert.
bild_x6.py <vorher.bmp> <nachher.bmp> <x0> <y0> <x1> <y1> <ausgabe.png> [faktor=6]"""
import sys
from PIL import Image
a, b = Image.open(sys.argv[1]).convert('RGB'), Image.open(sys.argv[2]).convert('RGB')
x0, y0, x1, y1 = map(int, sys.argv[3:7])
f = int(sys.argv[8]) if len(sys.argv) > 8 else 6
ka = a.crop((x0, y0, x1 + 1, y1 + 1)).resize(((x1 - x0 + 1) * f, (y1 - y0 + 1) * f), Image.NEAREST)
kb = b.crop((x0, y0, x1 + 1, y1 + 1)).resize(((x1 - x0 + 1) * f, (y1 - y0 + 1) * f), Image.NEAREST)
out = Image.new('RGB', (ka.width * 2 + 2 * f, ka.height), (255, 255, 255))
out.paste(ka, (0, 0)); out.paste(kb, (ka.width + 2 * f, 0))
out.save(sys.argv[7])
print(sys.argv[7], out.size)
