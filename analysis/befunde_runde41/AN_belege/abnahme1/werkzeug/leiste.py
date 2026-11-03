"""Bildleiste aus Videos (ffmpeg holt je Zeitpunkt ein Bild). Nur lesen.
   python leiste.py <ziel.jpg> <spalten> <video>|<sek>|<text> ...   (Video "-" = wie davor)"""
import subprocess, sys, os, tempfile
from PIL import Image, ImageDraw

FF = r"C:/ProgramData/chocolatey/bin/ffmpeg.exe"
ziel, sp = sys.argv[1], int(sys.argv[2])
TW = int(os.environ.get("TW", "640")); TH = TW * 3 // 4
bilder, vid = [], None
tmp = tempfile.mkdtemp()
for k, arg in enumerate(sys.argv[3:]):
    v, t, txt = arg.split('|', 2)
    if v != '-':
        vid = v
    out = os.path.join(tmp, '%03d.png' % k)
    subprocess.run([FF, '-hide_banner', '-loglevel', 'error', '-ss', t, '-i', vid, '-frames:v', '1', '-y', out], check=True)
    im = Image.open(out).convert('RGB').resize((TW, TH))
    d = ImageDraw.Draw(im)
    d.rectangle((0, 0, TW, 22), fill=(0, 0, 0))
    d.text((6, 5), txt, fill=(255, 220, 80))
    bilder.append(im)
z = (len(bilder) + sp - 1) // sp
blatt = Image.new('RGB', (TW * sp, TH * z), (20, 20, 20))
for i, b in enumerate(bilder):
    blatt.paste(b, ((i % sp) * TW, (i // sp) * TH))
blatt.save(ziel, quality=85)
print(ziel, blatt.size)
