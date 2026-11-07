#!/usr/bin/env python3
"""Variante K5, Nachtrag Rueckbild-Pose (Nutzer 2026-11-06 zu Zeile 1 der rundum_K5*.png: "Koerper irgendwie so komisch zusammengefaltet ...
nicht wie die Position aus dem Rueckbild"). Nur Bilder, keine Modell-/Clip-/Asset-Aenderung.

    python k5_rueckbild.py tafel AUS.png NAME=DIR [K4=DIR]     Tafel: Zeile 1 Pose Bild 1 (Clip 0), Zeile 2 Stehen des Spiel-Assets (Clip 2, Bild 0),
                                                              Zeile 3 Bild 1 Original | Variante Bild-1-Kamera (roh / aufgehellt) | K4 heute
    python k5_rueckbild.py diff DIR_A DIR_B                    Vertex-Diff Welt (Pose Bild 1) je Mesh, ohne Hals/Kopf/Kiefer (21-23)

Zeile 2: die Winkel und die Wurzel von Clip 2 ("stehen", 60 Bilder) Bild 0 aus stufe3/asset/BAHNSTEIG_FORM.EMD (Spiel-Asset, K4) werden in eine
Kopie des Varianten-EMD (nur _lauf/, nicht versioniert) als Ruhe-Keyframe geschrieben; Knochenversatz (Halslaenge, Kiefergelenk) bleibt der der Variante.
Licht und Gamma wie k5_blatt.blatt_rundum (Blattlicht, Gamma 0,5)."""
import json
import os
import sys

import cv2
import numpy as np

import k5_kern as C
import k5_blatt as B

KK, FE = C.KK, C.FE
P = B.P
ASSET = os.path.join(C.FORM, 'stufe3', 'asset', 'BAHNSTEIG_FORM.EMD')
STEHEN = 2           # stufe3/asset/clips.json: Clip 2 "stehen", 60 Bilder
ANSICHTEN = (('vorn', 90, 0), ('3/4 vorn rechts', 45, 15), ('rechts (W)', 0, 0), ('hinten', -90, 0), ('links (W)', 180, 0),
             ('3/4 vorn links oben', 135, 30))


def stehen_kandidat(d, name):
    """Kopie von d mit Clip 2 = Spiel-Asset Clip 2 Bild 0 (Winkel + Wurzel). Rueckgabe Kandidat."""
    a = FE.lese_emd(open(ASSET, 'rb').read())
    kf_a = a['clips'][STEHEN]['bilder'][0]['kf']
    f = FE.lese_emd(open(os.path.join(d, 'FORM.EMD'), 'rb').read())
    kf = f['clips'][2]['bilder'][0]['kf']
    f['pool'][kf]['winkel'] = [list(w) for w in a['pool'][kf_a]['winkel']]
    f['pool'][kf]['wurzel'] = list(a['pool'][kf_a]['wurzel'])
    aus = os.path.join(C.K5_DIR, '_lauf', 'stehen_' + name)
    os.makedirs(aus, exist_ok=True)
    open(os.path.join(aus, 'FORM.EMD'), 'wb').write(FE.schreibe_emd(f))
    open(os.path.join(aus, 'FORM.TIM'), 'wb').write(open(os.path.join(d, 'FORM.TIM'), 'rb').read())
    fj = json.load(open(os.path.join(d, 'form.json'), encoding='utf-8'))
    json.dump(fj, open(os.path.join(aus, 'form.json'), 'w', encoding='utf-8'))
    return P.Kandidat(os.path.join(aus, 'form.json')), kf_a


def reihe_rundum(k, clip, titel, gross=420):
    kf = k.kf(clip, 0)
    alle = P.pose_punkte(k.mo, kf)
    mitte = (alle.max(0) + alle.min(0)) / 2
    s = gross * 0.46 / float(np.linalg.norm(alle - mitte, axis=1).max())
    ks = []
    for t, az, el in ANSICHTEN:
        a, e = np.radians(az), np.radians(el)
        vor = np.array([-np.sin(a) * np.cos(e), np.sin(e), np.cos(a) * np.cos(e)])
        ks.append(B.blick_bild(k, kf, B.blick_matrix(vor, [0, -1, 0]), mitte, s, gross))
        B.schrift(ks[-1], '%s: %s' % (titel, t), 8, 22, 0.48)
    return B.reihe(ks)


def bild1_kacheln(k, nm, h=420):
    sauber, _alb, m, _ = k.rendern(1)
    s1 = sauber[:, :, ::-1].copy()
    hell = s1.copy()
    hell[m] = np.clip(np.power(s1[m] / 255.0, 0.6) * 255, 0, 255).astype(np.uint8)
    t1, _s = B.ausschnitt(s1, B.B1, h)
    B.schrift(t1, '%s Bild-1-Kamera, roh' % nm, 8, 22, 0.5)
    t2, _s = B.ausschnitt(hell, B.B1, h)
    B.schrift(t2, '%s Bild-1-Kamera, aufgehellt (Gamma 0,6)' % nm, 8, 22, 0.5)
    return t1, t2


def beschriftung(nm, d):
    kj = os.path.join(d, 'kopf.json')
    j = json.load(open(kj, encoding='utf-8')) if os.path.exists(kj) else {}
    if 'f4' in j:
        return '%s: F4 %s, F5 %s, Hals %d, Schnauze %.2f S, Augenabstand %d' % (nm, j['f4'], j['f5'], round(j['halslaenge']), j['schnauze_e'],
                                                                              round(j['kopf']['S']))
    return '%s: K4 heute (Spiel-Asset-Kopf), Hals 503, Schnauze 2,9 Augenabstaende (Augenhoecker 397 auseinander)' % nm


def tafel(aus, spec, k4spec=None):
    nm, d = spec.split('=', 1)
    k = P.Kandidat(os.path.join(d, 'form.json'))
    ks, kf_a = stehen_kandidat(d, nm)
    z1 = reihe_rundum(k, 0, 'Pose Bild 1 (Clip 0)')
    z2 = reihe_rundum(ks, 2, 'Stehen (Spiel-Clip %d, Bild 0)' % STEHEN)
    o1 = P.original(1)[:, :, ::-1].copy()
    t0, _s = B.ausschnitt(o1, B.B1, 420)
    B.schrift(t0, 'Bild 1 Original (Rueckbild Bahnsteig)', 8, 22, 0.5)
    t1, t2 = bild1_kacheln(k, nm)
    kach = [t0, t1, t2]
    if k4spec:
        n4, d4 = k4spec.split('=', 1)
        _r, h4 = bild1_kacheln(P.Kandidat(os.path.join(d4, 'form.json')), n4)
        kach.append(h4)
    z3 = B.reihe(kach)
    breite = max(z1.shape[1], z3.shape[1])
    kopf = B.kopfzeile('%s | Zeile 1 Pose Bild 1 (Clip 0), Zeile 2 Stehen = Spiel-Asset Clip %d Bild 0 (Winkel des Spiels, Halslaenge der Variante), '
                       'Licht nur fuers Blatt, Gamma 0,5 | Zeile 3 Bild-1-Kamera' % (beschriftung(nm, d), STEHEN), breite, g=0.55)
    cv2.imwrite(aus, B.stapel([kopf, z1, z2, z3]))


def diff(da, db):
    """Max. und mittlerer Abstand der Weltecken in Pose Bild 1 je Mesh (gleiche Eckenzahl vorausgesetzt), ohne 21-23."""
    out = {}
    import k5_mass as M
    fa, Ra, Ta = M.welt(da, 1)
    fb, Rb, Tb = M.welt(db, 1)
    for b in range(32):
        if b in (21, 22, 23):
            continue
        va = np.array(fa['meshes'][b]['ecken'], float)
        vb = np.array(fb['meshes'][b]['ecken'], float)
        if va.shape != vb.shape:
            out[b] = dict(ecken=[len(va), len(vb)], gleich=False)
            continue
        wa = va @ Ra[b].T + Ta[b]
        wb = vb @ Rb[b].T + Tb[b]
        dd = np.linalg.norm(wa - wb, axis=1)
        lok = float(np.abs(va - vb).max()) if len(va) else 0.0
        out[b] = dict(ecken=len(va), lokal_max=round(lok, 1), welt_max=round(float(dd.max()), 1) if len(dd) else 0.0,
                      welt_mittel=round(float(dd.mean()), 1) if len(dd) else 0.0)
    return out


if __name__ == '__main__':
    if sys.argv[1] == 'tafel':
        tafel(sys.argv[2], sys.argv[3], sys.argv[4] if len(sys.argv) > 4 else None)
    elif sys.argv[1] == 'diff':
        r = diff(sys.argv[2], sys.argv[3])
        print(json.dumps(r))
