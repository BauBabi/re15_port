"""Sicht-Pruefer Einbau Runde 1: eigene Spielbilder gegen Vorlage-Spielbilder (Tafel 05), gegen die 17-Zeilen-Variante gleicher Kette und gegen K4.
Roh, 640x480, keine Gamma. Pixel-Diff (max 8-Bit-Kanalabstand) fuer 5080 (deterministisch); fuer 5090 (Feuer) Minimum ueber 3 Bilder je Lauf
im Kreatur-Kasten mit Feuermaske (R>140, R>G+40, G>B+20)."""
import glob, os, sys
import numpy as np
from PIL import Image, ImageDraw, ImageFont
K = os.path.dirname(os.path.abspath(__file__))                    # .../K5/einbau/sicht
E = os.path.join(K, '..', 'spiel'); V = os.path.join(K, '..', '..', 'farbe', 'spiel_v3')
def ld(p): return np.asarray(Image.open(p).convert('RGB')).astype(int)
def feuer(A):
    R, G, B = A[..., 0], A[..., 1], A[..., 2]
    return (R > 140) & (R > G + 40) & (G > B + 20)
def pdiff(a, b, name):
    A, B = ld(a), ld(b); d = np.abs(A - B).max(axis=2); n = int((d > 0).sum())
    ys, xs = np.nonzero(d)
    print(f'{name}: Pixel-Diff {n}, max 8bit {int(d.max())}', list(zip(xs.tolist()[:6], ys.tolist()[:6])) if 0 < n <= 12 else '')
def mdiff(a, b, name, box):
    A, B = ld(a), ld(b); m = ~(feuer(A) | feuer(B)); x0, y0, x1, y1 = box
    mm = np.zeros_like(m); mm[y0:y1, x0:x1] = True; m &= mm
    d = np.abs(A - B).max(axis=2) * m; tot = int(m.sum())
    print(f'{name}: Maske {tot}px, >8: {int((d>8).sum())} ({100*(d>8).sum()/tot:.1f}%), >32: {int((d>32).sum())} ({100*(d>32).sum()/tot:.1f}%)')
def minbild(muster):
    ims = [ld(p) for p in sorted(glob.glob(muster))]
    assert ims, muster
    return np.min(np.stack(ims), axis=0), len(ims)
def save(arr, p): Image.fromarray(arr.astype(np.uint8)).save(p); return p
print('--- 5080 (deterministisch)')
pdiff(f'{K}/sicht_nachst_r5080.png', f'{E}/einbau_pose1_nachstellung_r5080.png', 'eigener Lauf Nachstellung vs Bauer-Lauf (Wiederholbarkeit)')
pdiff(f'{K}/sicht_nachst_r5080.png', f'{V}/spiel_pose1_nachstellung_r5080.png', 'eigener Lauf Nachstellung vs Vorlage-Spielbild Tafel 05 Z1')
pdiff(f'{K}/sicht_pose1_r5080_s1.png', f'{V}/spiel_pose1_r5080_s1.png', 'Asset Clip0 Bild11 (Lage form.json) vs Vorlage Tafel 05 Z2 (Variante Clip0)')
pdiff(f'{K}/sicht_pose1_r5080_s1.png', f'{V}/heute_spiel_pose1_r5080_s1.png', 'Asset vs K4 heute (muss gross)')
pdiff(f'{K}/sicht_ruhe_r5080_s1.png', f'{V}/spiel_ruhe_r5080_s1.png', 'Asset Ruhe Clip2 Bild0 vs Vorlage Tafel 05 Z3 (Variante eigene Ruhe)')
pdiff(f'{K}/sicht_ruhe_r5080_s1.png', f'{K}/var17_ruhe_r5080_s1.png', 'Asset Ruhe vs 17-Zeilen-Variante Ruhe, gleiche Kette heute')
pdiff(f'{K}/var17_ruhe_r5080_s1.png', f'{V}/spiel_ruhe_r5080_s1.png', '17-Zeilen-Variante Ruhe heute vs Vorlage-Spielbild (Wiederholbarkeit Kette)')
pdiff(f'{K}/sicht_ruhe_r5080_s1.png', f'{V}/heute_spiel_ruhe_r5080_s1.png', 'Asset Ruhe vs K4 heute Ruhe (muss gross)')
print('--- 5090 (Feuer): Minimum ueber 3 Bilder je Lauf, Kreatur-Kasten')
mp, n1 = minbild(f'{K}/sicht_pose2_r5090_s11_*.png'); mv, n2 = minbild(f'{K}/var17_pose2_r5090_s11_*.png')
mr, n3 = minbild(f'{K}/sicht_ruhe_r5090_s11_*.png'); mrv, n4 = minbild(f'{K}/var17_ruhe_r5090_s11_*.png')
print('Bilder je Lauf', n1, n2, n3, n4)
save(mp, f'{K}/min_asset_pose2_r5090.png'); save(mv, f'{K}/min_var17_pose2_r5090.png'); save(mr, f'{K}/min_asset_ruhe_r5090.png'); save(mrv, f'{K}/min_var17_ruhe_r5090.png')
box = (190, 140, 560, 430)
mdiff(f'{K}/min_asset_pose2_r5090.png', f'{K}/min_var17_pose2_r5090.png', 'Pose2 Asset(min3) vs Variante17(min3)', box)
mdiff(f'{K}/min_asset_pose2_r5090.png', f'{V}/spiel_pose2_r5090_s11.png', 'Pose2 Asset(min3) vs Vorlage-Spielbild (1 Bild)', box)
mdiff(f'{K}/min_asset_pose2_r5090.png', f'{V}/heute_spiel_pose2_r5090_s11.png', 'Pose2 Asset(min3) vs K4 heute (muss gross)', box)
mdiff(f'{K}/min_asset_ruhe_r5090.png', f'{K}/min_var17_ruhe_r5090.png', 'Ruhe Asset(min3) vs Variante17 Ruhe(min3)', box)
mdiff(f'{K}/min_asset_ruhe_r5090.png', f'{V}/heute_spiel_ruhe_r5090_s11.png', 'Ruhe Asset(min3) vs K4 heute (muss gross)', box)
# Rauschboden: Einzelbilder desselben Laufs
a = sorted(glob.glob(f'{K}/sicht_pose2_r5090_s11_*.png'))
mdiff(a[0], a[1], 'Rauschboden Pose2 Bild1 vs Bild2 (gleicher Lauf)', box)
