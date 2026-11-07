# Vergroesserter Bildausschnitt aus FRAMEDUMP-Bildern (eigenes Werkzeug der Abnahme 2).
#   python ausschnitt.py <laufdir> <aus.png> <x0,y0,x1,y1> <faktor> <bilder,..> [titel]
import sys, os
from PIL import Image, ImageDraw, ImageFont
d, out, box, fak, sel = sys.argv[1], sys.argv[2], [int(v) for v in sys.argv[3].split(',')], float(sys.argv[4]), sys.argv[5]
titel = sys.argv[6] if len(sys.argv) > 6 else ''
frames = [int(x) for x in sel.split(',')]
w, h = int((box[2]-box[0])*fak), int((box[3]-box[1])*fak)
cols = min(len(frames), 6); rows = (len(frames)+cols-1)//cols
sheet = Image.new('RGB', (cols*w, rows*(h+16)+24), (20,20,20)); dr = ImageDraw.Draw(sheet)
try: font = ImageFont.truetype('C:/Windows/Fonts/consola.ttf', 12)
except Exception: font = ImageFont.load_default()
dr.text((4,4), titel, fill=(255,255,255), font=font)
for i,f in enumerate(frames):
    p = os.path.join(d,'fd','f%06d.ppm'%f)
    if not os.path.exists(p): continue
    im = Image.open(p).convert('RGB').crop(tuple(box)).resize((w,h), Image.NEAREST)
    x, y = (i%cols)*w, 24+(i//cols)*(h+16)
    sheet.paste(im,(x,y)); dr.text((x+2,y+h+1),'F%d'%f,fill=(255,230,120),font=font)
sheet.save(out, optimize=True); print(out, sheet.size)
