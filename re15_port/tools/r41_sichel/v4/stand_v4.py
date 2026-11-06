"""SF V4 (Bauer): Zwischenstand bauen (birkin_formen --variante v4 nach sfv4/emd), Riegel ausgeben, Ausschnitte nach sfv4/bau/<name>_*.png."""
import os, sys, subprocess
import numpy as np, cv2
HIER = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HIER)
import v4_gemein as G, b2
name = sys.argv[1] if len(sys.argv) > 1 else 'stand'
BAUM = G.BAUM
subprocess.run([sys.executable, os.path.join(BAUM, 're15_port', 'tools', 'birkin_formen.py'), '--variante', 'v4', '--aus',
                os.path.join(G.SFV4, 'emd')], check=True, stdout=subprocess.DEVNULL)
r = subprocess.run([sys.executable, os.path.join(HIER, 'riegel_v4.py'), G.EMD_V4, '--kein-exit'], capture_output=True, text=True)
for z in r.stdout.splitlines():
    if not z.startswith('OK   G1') and not z.startswith('OK   M'):
        print(z[:240])
P = G.posen(); mo = b2.modell(G.EMD_V4)
for h, b, box in (('135', 1, (300, 120, 460, 260)), ('85', 2, (260, 170, 490, 360))):
    img, ids = G.zeichne(mo, P[h]); v = G.vorlage(b).astype(np.uint8)
    x0, y0, x1, y1 = box
    t = np.hstack([v[y0:y1, x0:x1], img[y0:y1, x0:x1]])
    cv2.imwrite(os.path.join(G.SFV4, 'bau', '%s_%s.png' % (name, h)),
                cv2.cvtColor(cv2.resize(t, None, fx=3, fy=3, interpolation=cv2.INTER_NEAREST), cv2.COLOR_RGB2BGR))
