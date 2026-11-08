# Leser B: Vorlagebild -> Bildnummer n im Original-Video (Bildsuche, MSE)
import sys, glob, os, numpy as np
from PIL import Image
S = sys.argv[1]; V = sys.argv[2]
fr = sorted(glob.glob(os.path.join(S, 'allf', 'f_*.png')))
A = [np.asarray(Image.open(f).convert('RGB'), dtype=np.float32) for f in fr]
for name in ['Beschreibung1_Vorlage.png', 'Beschreibung2_Vorlage.png']:
    v = np.asarray(Image.open(os.path.join(V, name)).convert('RGB'), dtype=np.float32)
    m = [float(((a - v) ** 2).mean()) for a in A]
    o = np.argsort(m)[:5]
    print(name, [(1200 + int(i), round(m[i], 2)) for i in o])
