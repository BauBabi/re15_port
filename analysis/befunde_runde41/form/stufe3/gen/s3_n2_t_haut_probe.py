"""Nachbesserung 2, Textur-Spur: Probe haut_stoff gegen k_bau.stoff (Seite 3). Aufruf aus analysis/befunde_runde41/form:
    python stufe3/gen/s3_n2_t_haut_probe.py   -> Kennzahlen, stufe3/roh/n2_textur/haut_stoff_vergleich.png (alt | neu | neu 2 x gekachelt)"""
import sys, os, json, time
sys.path.insert(0, r'stufe3/gen'); sys.path.insert(0, r'varianten/K_alligator/generator')
import s3_n2_textur as T, numpy as np
import form_buehne as FB
import k_bau as KB
from PIL import Image
J = 'varianten/J_dicker'
tx = FB.Textur(open(J + '/FORM.TIM', 'rb').read())
bau = json.load(open(J + '/bau.json'))
t0 = time.time()
st, basis = T.haut_stoff(tx, bau, (128, 256))
print('kandidaten', T.haut_stoff.kandidaten); print('haut_stoff', st.shape, 'basis', basis, 'mittel', st.reshape(-1, 3).mean(0), 'sek', round(time.time() - t0, 1))
alt, b_alt = KB.stoff(tx, bau, (128, 256))
Y = lambda a: a @ np.array([.299, .587, .114])
for nm, a, b in (('stoff (alt)', alt, b_alt), ('haut_stoff', st, basis)):
    c = a * b
    y = Y(c)
    med = np.median(y)
    print(nm, 'Y p50 %.1f p99 %.1f max %.1f, Anteil > Median+40: %.4f, Buntheit p99 %.1f' % (med, np.percentile(y, 99), y.max(), (y > med + 40).mean(), np.percentile(c.max(-1) - c.min(-1), 99)))
    # Naht am Umlauf: Sprung zwischen letzter und erster Spalte gegen Nachbarspalten
    d_um = np.abs(c[:, -1] - c[:, 0]).mean(); d_in = np.abs(c[:, 1:] - c[:, :-1]).mean()
    print('   Umlaufsprung %.1f gegen Nachbarspalten %.1f' % (d_um, d_in))
def bild(a, b):
    return np.clip(a * b * 2.2, 0, 255).astype(np.uint8)
out = np.hstack([bild(alt, b_alt), np.zeros((256, 8, 3), np.uint8), bild(st, basis), np.zeros((256, 8, 3), np.uint8), np.tile(bild(st, basis), (1, 2, 1))])
Image.fromarray(out).resize((out.shape[1] * 3, out.shape[0] * 3), Image.NEAREST).save('stufe3/roh/n2_textur/haut_stoff_vergleich.png')
