#!/usr/bin/env bash
# Runde 41 Spur H: Bezugsbild p0 einer 30-Bilder/s-Aufnahme (lauf.sh mit GD_FPS=30 GD_VF=scale=320:240) = Aufnahmebild
# des Videobilds h0. Erst das erste Cut-12-Bild (finde12.py, Kantenkorrelation gegen ein Videobild), dann die erste
# Bewegung im Spider-Man-Feld (x 0..120) = Absprung h2 (Zeitplan BF_H_SPRUNG_AB) -> p0 = dieses Bild - 2.
# Aufruf: p0find.sh <laufordner> <videobild-ordner mit f_00150.png>   (Windows-Pfade, z.B. C:/...)
set -u
HIER=$(cd "$(dirname "$0")" && pwd)
st=$(python "$HIER/finde12.py" "$1/gd_*.png" "$2/f_00150.png" | head -1 | grep -o "gd_[0-9]*" | head -1 | cut -c4-)
python -c "
import numpy as np, sys
from PIL import Image
d=sys.argv[1]; s=int(sys.argv[2]); prev=None
for i in range(s, s+25):
    a=np.asarray(Image.open(d+'/gd_%04d.png'%i).convert('L'),dtype=np.float32)
    if prev is not None and np.abs(a-prev)[60:220,0:120].mean()>5: print(i-2); break
    prev=a
" "$1" "$st"
