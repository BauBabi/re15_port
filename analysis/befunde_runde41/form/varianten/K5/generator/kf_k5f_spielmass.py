#!/usr/bin/env python3
"""K5f: Spielmasse aus den Framedumps (Messblatt K5F 1.1, 4.1, 4.4 HU3).

    python kf_k5f_spielmass.py --schluessel <Variante> <ziel-praefix>   -> <praefix>_normal/ und <praefix>_key/ (BAHNSTEIG_FORM.EMD/.TIM)
    python kf_k5f_spielmass.py <Variante> <name> <aus.json>              -> Masse P/Q (5080 bindend, 5090 Info) und HU3
    python kf_k5f_spielmass.py --s <par.json> <lo> <hi> <schritte>      -> haut.s per Bisektion im Spiel (Z4/Z8), schreibt par.json

Figurmaske = Pixel, die sich zwischen Normal- und Schluessel-Lauf unterscheiden (eigenes Paar der Variante; die Laeufe sind deterministisch,
Messblatt 1.1: Normal gegen Normal 0 Pixel). P = Spielpixel, Q = normal/key x 255 (nur Pixel mit key-Mittel >= 24). Masse = k5f/skripte/fam.alles
(Lab OpenCV float, Figur um 2 px erodiert). Bilder mit < 1200 Maskenpixeln zaehlen nicht (wie spiel_fam2.py).
HU3: 5090 Pose Clip 27 Bild 20; Orte = Zeile-0-Pixel des K5e_f1 (Schluessel nur auf Zeile 0: Laeufe hp1_form_k5e_f1_normal/_key0 des Messers);
warm-rot = a* > 6 und Ton > -10 Grad. Gezaehlt fuer die Variante und als Gegenprobe fuer K5e_f1 selbst.
Die json traegt sha256 des TIM der Variante und des gemessenen Normal-TIM: der Riegel nimmt die Werte nur bei Gleichheit an.
"""
import glob
import hashlib
import json
import os
import shutil
import struct
import sys

import cv2
import numpy as np

HIER = os.path.dirname(os.path.abspath(__file__))
K5 = os.path.dirname(HIER)
sys.path.insert(0, os.path.join(K5, 'k5f', 'skripte'))
S = r'C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/k5f'
SP = S + '/spiel'
KENN = ('L_mittel', 'L_std', 'L_cv', 'lokal5', 'lokal5_rel', 'C_mittel', 'C_std', 'dunkel_pct', 'hell_pct', 'S5', 'F_fremd', 'spanne90',
        'block_ton_streu')


def sha(p):
    return hashlib.sha256(open(p, 'rb').read()).hexdigest()


def tim_key(b, o=0):
    """wie k5f/skripte/schluessel.tim_key: jede CLUT-Farbe != 0 -> 0x7FFF, STP-Bit bleibt."""
    assert struct.unpack_from('<I', b, o)[0] == 0x10
    cs = struct.unpack_from('<I', b, o + 8)[0]
    k = 0
    for i in range((cs - 12) // 2):
        p = o + 20 + 2 * i
        w = struct.unpack_from('<H', b, p)[0]
        if w != 0:
            struct.pack_into('<H', b, p, 0x7FFF | (w & 0x8000))
            k += 1
    return k


def schluessel(v, praefix):
    for art in ('normal', 'key'):
        d = praefix + '_' + art
        os.makedirs(d, exist_ok=True)
        t = bytearray(open(os.path.join(v, 'BAHNSTEIG_FORM.TIM'), 'rb').read())
        k = tim_key(t) if art == 'key' else 0
        open(os.path.join(d, 'BAHNSTEIG_FORM.TIM'), 'wb').write(bytes(t))
        shutil.copy(os.path.join(v, 'BAHNSTEIG_FORM.EMD'), os.path.join(d, 'BAHNSTEIG_FORM.EMD'))
        print('Schluessel', art, d, 'CLUT-Woerter geaendert', k)


def rgb(p):
    return cv2.cvtColor(cv2.imread(p), cv2.COLOR_BGR2RGB)


def lab(img):
    return cv2.cvtColor(img.astype(np.float32) / 255.0, cv2.COLOR_RGB2Lab).astype(np.float64)


def paar(n_ordner, k_ordner):
    import fam
    W = []
    for f in sorted(glob.glob('%s/%s/dump_*.ppm' % (SP, n_ordner))):
        b = os.path.basename(f)
        fk = '%s/%s/%s' % (SP, k_ordner, b)
        if not os.path.exists(fk):
            continue
        a, ky = rgb(f), rgb(fk)
        m = (a != ky).any(2)
        if m.sum() < 1200:
            continue
        d = fam.alles(a, m)
        d['bild'] = b
        d['maske_px'] = int(m.sum())
        kf = ky.astype(float)
        mq = m & (kf.mean(2) >= 24)
        q = np.clip(a.astype(float) / np.maximum(kf, 1) * 255.0, 0, 255).astype(np.uint8)
        dq = fam.alles(q, mq)
        for kk in KENN:
            d['Q_' + kk] = dq[kk]
        d['licht'] = round(float(kf[m].mean()), 1)
        W.append(d)
    if not W:
        return None
    mit = {kk: round(float(np.mean([w[kk] for w in W])), 3) for kk in W[0] if isinstance(W[0][kk], (int, float))}
    return dict(mittel=mit, bilder=W, n=len(W))


def hu3(ordner):
    """Zeile-0-Orte des K5e_f1 (Schluessel nur Zeile 0) in 5090 Pose Clip 27 Bild 20; warm-rot (a* > 6, Ton > -10) im Bild `ordner`."""
    b = 'dump_000440.ppm'
    n = rgb('%s/hp1_form_k5e_f1_normal/%s' % (SP, b))
    k = rgb('%s/hp1_form_k5e_f1_key0/%s' % (SP, b))
    m = (n != k).any(2)
    v = rgb('%s/%s/%s' % (SP, ordner, b))
    L = lab(v)
    h = np.degrees(np.arctan2(L[..., 2], L[..., 1]))
    rot = m & (L[..., 1] > 6) & (h > -10)
    return dict(orte_px=int(m.sum()), warm_rot_px=int(rot.sum()), bild=b, ordner=ordner,
                lab_mittel=[round(float(x), 2) for x in L[m].mean(0)] if m.any() else None)


def s_bisektion(pp, lo, hi, schritte):
    """haut.s: kleinste Buntheit, mit der die Form im Spiel (5080 Bild-1-Lage, eigenes Normal-/Schluessel-Paar) Z4 P-C* >= 4,41 und Z8 Q-C* >= 12,09
    erreicht (Messblatt K5F 3.1). Je Schritt: Bau (kf_k5f), Schluessel, 2 Spiellaeufe (k5f/skripte/lauf.sh), Masse paar(). Ergebnis + Protokoll in par."""
    import subprocess
    sys.path.insert(0, HIER)
    import kf_k5f as KF
    par = json.load(open(pp, encoding='utf-8'))
    lauf = os.path.join(K5, 'k5f', 'skripte', 'lauf.sh')      # Git-Bash ausdruecklich (CreateProcess faende sonst System32/bash.exe = WSL)
    proto = []

    def mess(sv):
        p = json.loads(json.dumps(par))
        p['haut']['s'] = sv
        n = 'sbis_%s' % ('%.4f' % sv).replace('.', '')
        v = os.path.join(S, 'bis', n)
        if os.path.isdir(v):
            shutil.rmtree(v)
        KF.bauen(p, v)
        schluessel(v, '%s/ueber/form_%s' % (S, n))
        for art in ('normal', 'key'):
            for d in glob.glob('%s/f80_%s_%s' % (SP, n, art)):
                shutil.rmtree(d)
            subprocess.run([shutil.which('bash') or 'bash', lauf, 'f80_%s_%s' % (n, art), '5080', '360-440/20', 'RE15_BIRKIN_FORM=bahnsteig',
                            'RE15_BB_POSE=-20711,-15984,756,0,11,1', 'RE15_PLAYER_POS=500,-9175,2048,0',
                            'RE15_RE2_ASSET_ROOT=%s/ueber/form_%s_%s' % (S, n, art)], cwd=S, check=False, stdout=subprocess.DEVNULL)
        m = paar('f80_%s_normal' % n, 'f80_%s_key' % n)['mittel']
        proto.append([sv, m['C_mittel'], m['Q_C_mittel'], m['C_std'], m['Q_C_std']])
        print('  s %.4f -> P C* %.2f Q C* %.2f | P C-std %.2f Q C-std %.2f' % tuple(proto[-1]), flush=True)
        return m['C_mittel'] >= 4.41 and m['Q_C_mittel'] >= 12.09
    for _ in range(schritte):
        sv = round(0.5 * (lo + hi), 4)
        if mess(sv):
            hi = sv
        else:
            lo = sv
    par['haut']['s'] = round(hi, 4)
    par['haut']['s_quelle'] = ('kf_k5f_spielmass.py --s: kleinste Buntheit (Bisektion im echten Spiel, 5080 Bild-1-Lage, Bilder 360-440, eigenes Normal-/'
                               'Schluessel-Paar) mit Z4 P-C* >= 4,41 und Z8 Q-C* >= 12,09 (Messblatt K5F 3.1: "kleinste Buntheit, mit der Z4 und Z8 im Spiel '
                               'erreicht werden"; Vorschau v7 mit naechster 15-Bit-Farbe: s 0,52). Protokoll [s, P C*, Q C*, P C-std, Q C-std]: %s' % proto)
    json.dump(par, open(pp, 'w', encoding='utf-8'), indent=1, ensure_ascii=False)
    print('fertig s', par['haut']['s'])


def main():
    if sys.argv[1] == '--s':
        s_bisektion(sys.argv[2], float(sys.argv[3]), float(sys.argv[4]), int(sys.argv[5]))
        return
    if sys.argv[1] == '--schluessel':
        schluessel(sys.argv[2], sys.argv[3])
        return
    v, name, aus = sys.argv[1], sys.argv[2], sys.argv[3]
    r = dict(variante=os.path.basename(os.path.normpath(v)), name=name, tim_sha256=sha(os.path.join(v, 'BAHNSTEIG_FORM.TIM')),
             emd_sha256=sha(os.path.join(v, 'BAHNSTEIG_FORM.EMD')),
             gemessen_tim_sha256=sha('%s/ueber/form_%s_normal/BAHNSTEIG_FORM.TIM' % (S, name)),
             gemessen_emd_sha256=sha('%s/ueber/form_%s_normal/BAHNSTEIG_FORM.EMD' % (S, name)),
             verfahren='kf_k5f_spiel.sh + kf_k5f_spielmass.py (Messblatt K5F 1.1): eigenes Normal-/Schluessel-Paar, echter Renderer, Framedump')
    for sz in ('f80', 'f90'):
        r[sz] = paar('%s_%s_normal' % (sz, name), '%s_%s_key' % (sz, name))
        if r[sz]:
            m = r[sz]['mittel']
            print(sz, 'n', r[sz]['n'], 'px', m['maske_px'], '| P L', m['L_mittel'], 'cv', m['L_cv'], 'lok', m['lokal5_rel'], 'C', m['C_mittel'], m['C_std'],
                  '| Q cv', m['Q_L_cv'], 'lok', m['Q_lokal5_rel'], 'C', m['Q_C_mittel'], m['Q_C_std'], '| R', m['S5'], m['F_fremd'], m['spanne90'],
                  m['block_ton_streu'])
    hp = 'hp1_%s_normal' % name
    if not os.path.isdir('%s/%s' % (SP, hp)):
        hp = 'hp1_form_%s_normal' % name      # Laeufe des Messers (K5e_f1)
    r['hu3'] = hu3(hp) if os.path.isdir('%s/%s' % (SP, hp)) else None
    r['hu3_gegenprobe_k5e_f1'] = hu3('hp1_form_k5e_f1_normal')
    print('HU3', r['hu3'], '| Gegenprobe K5e_f1', r['hu3_gegenprobe_k5e_f1'])
    json.dump(r, open(aus, 'w', encoding='utf-8'), indent=1, ensure_ascii=False)


if __name__ == '__main__':
    main()
