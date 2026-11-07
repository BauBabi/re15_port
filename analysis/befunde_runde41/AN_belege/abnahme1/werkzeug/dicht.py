"""Dichte Kontaktleiste mit Zeitstempel: python dicht.py <video> <t0> <t1> <schritt> <ziel.png> [spalten]"""
import subprocess, sys
import numpy as np
from PIL import Image, ImageDraw

FF = r"C:/ProgramData/chocolatey/bin/ffmpeg.exe"
v, t0, t1, st, ziel = sys.argv[1], float(sys.argv[2]), float(sys.argv[3]), float(sys.argv[4]), sys.argv[5]
sp = int(sys.argv[6]) if len(sys.argv) > 6 else 8
W, H = 320, 240
fps = 1.0 / st
n = int(round((t1 - t0) / st))
raw = subprocess.run([FF, '-hide_banner', '-loglevel', 'error', '-ss', str(t0), '-t', str(t1 - t0), '-i', v,
                      '-vf', 'fps=%f,scale=%d:%d' % (fps, W, H), '-f', 'rawvideo', '-pix_fmt', 'rgb24', '-'],
                     capture_output=True, check=True).stdout
a = np.frombuffer(raw, dtype=np.uint8).reshape(-1, H, W, 3)
z = (len(a) + sp - 1) // sp
blatt = Image.new('RGB', (W * sp, H * z))
for i, f in enumerate(a):
    im = Image.fromarray(f)
    d = ImageDraw.Draw(im)
    d.rectangle((0, 0, 70, 14), fill=(0, 0, 0))
    d.text((3, 2), '%.2f' % (t0 + i * st), fill=(255, 255, 0))
    blatt.paste(im, ((i % sp) * W, (i // sp) * H))
blatt.save(ziel)
print(len(a), 'Bilder')
