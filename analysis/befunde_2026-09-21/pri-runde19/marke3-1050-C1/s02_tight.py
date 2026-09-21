import os
from PIL import Image, ImageDraw
D = os.path.dirname(os.path.abspath(__file__))
small = Image.open(os.path.join(D, "00_voll_320x240.png")).convert("RGB")

def z(name, x0, y0, x1, y1, f=8, grid=0):
    c = small.crop((x0, y0, x1, y1)).resize(((x1 - x0) * f, (y1 - y0) * f), Image.NEAREST)
    if grid:
        d = ImageDraw.Draw(c)
        for x in range(x0, x1, grid):
            d.line([((x - x0) * f, 0), ((x - x0) * f, c.size[1])], fill=(255, 0, 0), width=1)
            d.text(((x - x0) * f + 1, 1), str(x), fill=(255, 255, 0))
        for y in range(y0, y1, grid):
            d.line([(0, (y - y0) * f), (c.size[0], (y - y0) * f)], fill=(255, 0, 0), width=1)
            d.text((1, (y - y0) * f + 1), str(y), fill=(255, 255, 0))
    c.save(os.path.join(D, name))
    print(name, c.size)

# Spielerkasten x181..219 y122..190 + Faecher x180..230
z("02_kasten_x175-235_y115-200.png", 175, 115, 235, 200, f=10, grid=10)
z("03_beine_x175-230_y150-200.png", 175, 150, 230, 200, f=12, grid=5)
z("04_oberkoerper_x175-230_y115-160.png", 175, 115, 230, 160, f=12, grid=5)
