"""Spielbilder: Figurmaske = Pixel, die sich zwischen Normal- und Schluessel-Lauf (CLUT weiss) unterscheiden (Laeufe deterministisch:
Normal gegen Normal 0 Pixel). Je Bild: (P) Pixelwerte, wie der Spieler sie sieht; (Q) Albedo-Quotient normal/key (Raumlicht herausgerechnet,
nur Pixel mit key >= 24 je Kanal-Mittel)."""
import sys, json, glob, os
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import numpy as np, cv2, fam
S = sys.argv[1]; SP = S + '/spiel'
PAARE = [x.split(',') for x in sys.argv[3:]]
aus = {}
for p in PAARE:
    name, n, k = p[0], p[1], p[2]; mk = p[3] if len(p) > 3 else None   # mk: Ordner, dessen Normal/Key-Paar die Maske liefert
    W = []
    for f in sorted(glob.glob('%s/%s/dump_*.ppm' % (SP, n))):
        b = os.path.basename(f)
        a = cv2.cvtColor(cv2.imread(f), cv2.COLOR_BGR2RGB)
        if mk:
            mn, mkk = mk.split('+')
            if not os.path.exists('%s/%s/%s' % (SP, mkk, b)): continue
            m = (cv2.imread('%s/%s/%s' % (SP, mn, b)) != cv2.imread('%s/%s/%s' % (SP, mkk, b))).any(2)
            ky = cv2.cvtColor(cv2.imread('%s/%s/%s' % (SP, mkk, b)), cv2.COLOR_BGR2RGB)
        else:
            fk = '%s/%s/%s' % (SP, k, b)
            if not os.path.exists(fk): continue
            ky = cv2.cvtColor(cv2.imread(fk), cv2.COLOR_BGR2RGB); m = (a != ky).any(2)
        if m.sum() < 1200: continue
        d = fam.alles(a, m); d['bild'] = b; d['maske_px'] = int(m.sum())
        kf = ky.astype(float); mq = m & (kf.mean(2) >= 24)
        q = np.clip(a.astype(float) / np.maximum(kf, 1) * 255.0, 0, 255).astype(np.uint8)
        dq = fam.alles(q, mq)
        for kk in ('L_mittel', 'L_std', 'L_cv', 'lokal5', 'lokal5_rel', 'C_mittel', 'C_std', 'dunkel_pct', 'hell_pct', 'S5', 'F_fremd', 'spanne90', 'block_ton_streu'):
            d['Q_' + kk] = dq[kk]
        d['licht'] = round(float(kf[m].mean()), 1)
        W.append(d)
        if b.endswith('000440.ppm') or b.endswith('000600.ppm'):
            o = a.copy(); o[~m] = (o[~m] * 0.35).astype(np.uint8)
            cv2.imwrite('%s/maske_%s_%s.png' % (S, name.replace(' ', '_'), b[:-4]), cv2.cvtColor(np.hstack([a, o, q]), cv2.COLOR_RGB2BGR))
    if not W:
        print(name, 'keine Bilder mit Figur'); aus[name] = None; continue
    mit = {kk: round(float(np.mean([w[kk] for w in W])), 3) for kk in W[0] if isinstance(W[0][kk], (int, float))}
    aus[name] = dict(mittel=mit, bilder=W, n=len(W))
    print(name, 'n', len(W), 'px', mit['maske_px'], 'licht', mit['licht'], '| P L', mit['L_mittel'], 'std', mit['L_std'], 'cv', mit['L_cv'], 'lok', mit['lokal5'], mit['lokal5_rel'],
          'C', mit['C_mittel'], mit['C_std'], 'd/h%', mit['dunkel_pct'], mit['hell_pct'], 'R', mit['S5'], mit['F_fremd'], mit['spanne90'], mit['block_ton_streu'],
          '| Q L', mit['Q_L_mittel'], 'std', mit['Q_L_std'], 'cv', mit['Q_L_cv'], 'lok', mit['Q_lokal5_rel'], 'C', mit['Q_C_mittel'], mit['Q_C_std'], 'R', mit['Q_S5'], mit['Q_spanne90'], mit['Q_block_ton_streu'], flush=True)
json.dump(aus, open(S + '/' + sys.argv[2], 'w'), indent=1)
