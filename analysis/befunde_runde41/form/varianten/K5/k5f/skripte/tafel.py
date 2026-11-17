"""Tafel familie_spiellicht.png (roh, kein Gamma): je Zeile Spielbild-Ausschnitt (Framedump, echter Renderer) | Albedo-Quotient normal/key |
weisses Licht (Tafel-07-Licht, 3/4 vorn) + Zahlen. Figur per Schluessel-Maske; ausserhalb abgedunkelt (x0,35)."""
import sys, os, json, glob
sys.path.insert(0, r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_form/analysis/befunde_runde41/form/varianten/K5/ruhe/skripte')
import numpy as np, cv2, rl
S = sys.argv[1]; AUS = sys.argv[2]; SP = S + '/spiel'
J = {}
for f in ('spiel_a.json', 'spiel_b.json', 'spiel_c.json', 'spiel_d.json', 'spiel_e.json'):
    if os.path.exists(S + '/' + f): J.update({k: v for k, v in json.load(open(S + '/' + f)).items() if v})
W = json.load(open(S + '/weiss_fam.json'))
WV = {}
for f in glob.glob(S + '/weiss_var_*.json'): WV.update(json.load(open(f)))
H = 210
def text(img, zeilen, y0=14, x0=4, s=0.42):
    for i, z in enumerate(zeilen):
        cv2.putText(img, z, (x0, y0 + 15 * i), cv2.FONT_HERSHEY_SIMPLEX, s, (235, 235, 235), 1, cv2.LINE_AA)
def crop(a, m, pad=12):
    ys, xs = np.nonzero(m)
    y0, y1, x0, x1 = max(ys.min() - pad, 0), min(ys.max() + pad, a.shape[0]), max(xs.min() - pad, 0), min(xs.max() + pad, a.shape[1])
    c = a[y0:y1, x0:x1]; sc = H / c.shape[0]
    c = cv2.resize(c, (max(int(c.shape[1] * sc), 1), H), interpolation=cv2.INTER_NEAREST)
    return c[:, :int(H * 1.6)] if c.shape[1] > H * 1.6 else c
def spielzelle(name, n, k, b, mk=None):
    a = cv2.imread('%s/%s/%s' % (SP, n, b))
    if mk:
        mn, mkk = mk
        m = (cv2.imread('%s/%s/%s' % (SP, mn, b)) != cv2.imread('%s/%s/%s' % (SP, mkk, b))).any(2); ky = cv2.imread('%s/%s/%s' % (SP, mkk, b))
    else:
        ky = cv2.imread('%s/%s/%s' % (SP, k, b)); m = (a != ky).any(2)
    o = a.copy(); o[~m] = (o[~m] * 0.35).astype(np.uint8)
    q = np.clip(a.astype(float) / np.maximum(ky.astype(float), 1) * 255, 0, 255).astype(np.uint8); q[~m] = 40
    return crop(o, m), crop(q, m)
def pad(c, w):
    out = np.full((H, w, 3), 24, np.uint8); out[:, :min(c.shape[1], w)] = c[:, :w]; return out
REIHEN = [('Zombie (RE1.5 EM10) 1140', 'zombie_n', 'zombie_k', 'dump_000600.ppm', None, 'Zombie 1140', 'Zombie'),
          ('Gorilla (RE1.5 EM27) 11C0', 'gorilla_normal', 'gorilla_key', None, None, 'Gorilla 11C0', 'Gorilla'),
          ('EM30 Hangar-Birkin 50A0 Schnitt 4', 'hc4_normal', 'hc4_key', 'dump_000600.ppm', None, 'EM30 50A0 c4', 'EM30'),
          ('Tyrant/EM2B 50A0 Schnitt 4', 'hc4_normal', 'hc4_key2b', 'dump_000600.ppm', None, 'Tyrant 50A0 c4', 'Tyrant'),
          ('K5c (vorher) 5080 Bild-1-Lage', 'f80_k5c_normal', 'f80_k5c_key', 'dump_000440.ppm', None, 'K5c 5080', 'K5c'),
          ('K5e_f1 (Spiel-Asset heute) 5080', 'f80_k5e_f1_normal', 'f80_k5e_f1_key', 'dump_000440.ppm', None, 'K5e_f1 5080', 'K5e_f1'),
          ('K5f-Vorschau v7 (nur scratch) 5080', 'f80_v7', None, 'dump_000440.ppm', ('f80_k5e_f1_normal', 'f80_k5e_f1_key'), 'K5f v7 5080', 'v7'),
          ('K5c (vorher) 5090 Bild-2-Pose', 'f90_k5c_normal', 'f90_k5c_key', 'dump_000440.ppm', None, 'K5c 5090', None),
          ('K5e_f1 (heute) 5090', 'f90_k5e_f1_normal', 'f90_k5e_f1_key', 'dump_000440.ppm', None, 'K5e_f1 5090', None),
          ('K5f-Vorschau v7 5090', 'f90_v7', None, 'dump_000440.ppm', ('f90_k5e_f1_normal', 'f90_k5e_f1_key'), 'K5f v7 5090', None)]
mods = {'Zombie': lambda: rl.PL.re15typ(0x10), 'Gorilla': lambda: rl.PL.re15typ(0x27), 'EM30': lambda: rl.fl.lade_em30(), 'Tyrant': lambda: rl.PL.re15typ(0x2B),
        'K5c': lambda: rl.fl.lade_var(rl.BASIS), 'K5e_f1': lambda: rl.fl.lade_var(rl.K5 + '/ruhe/K5e_f1'), 'v7': lambda: rl.fl.lade_var(S + '/vorschau/v7'),
        'G4': lambda: rl.PL.re2(0x34), 'G5': lambda: rl.PL.re2(0x36)}
def weisszelle(key):
    mo = mods[key]()
    if not hasattr(mo, 'T'): mo.T = rl._Tex(mo)
    clip = 2 if key in ('K5c', 'K5e_f1', 'v7') else 0
    img, K = rl.weiss_id(mo, clip, 45, 15, H)
    return cv2.cvtColor(img, cv2.COLOR_RGB2BGR)
zeilen = []
for titel, n, k, b, mk, jn, wk in REIHEN:
    if b is None:
        b = J[jn]['bilder'][0]['bild']
    p, q = spielzelle(titel, n, k, b, mk)
    m = J[jn]['mittel']
    info = np.full((H, 330, 3), 24, np.uint8)
    text(info, [titel, 'Spiel %s, %d Bilder' % (b[:-4], J[jn]['n']),
                'P (Spielpixel): L* %.1f  L-cv %.2f  lokal %.2f' % (m['L_mittel'], m['L_cv'], m['lokal5_rel']),
                '   C* %.2f  C-std %.2f  dunkel %.0f%%' % (m['C_mittel'], m['C_std'], m['dunkel_pct']),
                'Q (Albedo normal/key): L* %.1f  L-cv %.2f' % (m['Q_L_mittel'], m['Q_L_cv']),
                '   lokal %.2f  C* %.2f  C-std %.2f' % (m['Q_lokal5_rel'], m['Q_C_mittel'], m['Q_C_std']),
                'Ruhe P: S5 %.1f Spanne %.0f Block %.1f' % (m['S5'], m['spanne90'], m['block_ton_streu'])])
    wz = weisszelle(wk) if wk else np.full((H, H, 3), 24, np.uint8)
    if wk:
        wm = W.get(wk, {}).get('mittel') or WV.get(wk) or WV.get('v7')
        if wm: text(wz, ['weiss: L-cv %.2f lok %.2f' % (wm['L_cv'], wm['lokal5_rel']), 'C* %.1f std %.1f' % (wm['C_mittel'], wm['C_std']),
                          'Spanne %.0f Block %.1f' % (wm['spanne90'], wm['block_ton_streu'])], y0=H - 40, s=0.36)
    zeilen.append(np.hstack([info, pad(p, 340), pad(q, 340), wz]))
for wk, titel in (('G4', 'RE2-G4 Original (nicht im Spiel)'), ('G5', 'RE2-G5 EM36 (im Spiel nicht im Bild)')):
    info = np.full((H, 330, 3), 24, np.uint8); wm = W[wk]['mittel']
    text(info, [titel, 'nur weisses Licht (Tafel 07)', 'L-cv %.2f lokal %.2f' % (wm['L_cv'], wm['lokal5_rel']), 'C* %.2f C-std %.2f' % (wm['C_mittel'], wm['C_std']),
                'S5 %.1f Spanne %.0f Block %.1f' % (wm['S5'], wm['spanne90'], wm['block_ton_streu'])])
    zeilen.append(np.hstack([info, np.full((H, 680, 3), 24, np.uint8), weisszelle(wk)]))
kopf = np.full((40, zeilen[0].shape[1], 3), 10, np.uint8)
text(kopf, ['K5f Messung: Familie im Spiellicht. Spalten: Zahlen | Spielbild (Framedump echter Renderer, Figur per Schluessel-CLUT) | Albedo-Quotient normal/key | weisses Licht 3/4 vorn. Roh, kein Gamma.'], s=0.45)
os.makedirs(os.path.dirname(AUS), exist_ok=True)
cv2.imwrite(AUS, np.vstack([kopf] + zeilen)); print('geschrieben', AUS)
