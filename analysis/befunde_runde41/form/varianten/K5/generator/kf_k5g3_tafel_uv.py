#!/usr/bin/env python3
"""Tafel 11 der K5g3-Geometrie Runde 2 (roh): UV der Spalt-Kinder - vorher (B4-UV, Runde 1) und nachher (Stufe uvfein, Runde 2).

Textur = die der K5g3-Abgabe (k5g/K5g3/BAHNSTEIG_FORM.TIM, kontrastreich, dort fielen die Streifen auf). Die Flaechen kommen aus dem
Pruef-Stand der Textur-Stufe (k5g/K5g3/pruef = K5e_f1-Flaechen mit K5g3-Textur, ungeteilt) und werden wie in kf_k5g3.py mit
kf_k5g2_geo.mulde_auf_form geteilt: einmal mit der B4-Tabelle (ergibt byte-gleich die K5g3-Abgabe), einmal mit geo/spalt_uv_tabelle.json.
Spalten: ungeteilt (Pruef-Stand) | Runde 1 | Runde 1, Linien-Dreiecke magenta | Runde 2 | Runde 2, Linien-Dreiecke magenta.
Zeilen: Kopf nah unter weissem Licht (rl.weiss_id, Clip 1 0/-60 = Ansicht der Sicht-Pruefung, Clip 2 0/10, Clip 0 180/20, Clip 2 90/-45).

  python kf_k5g3_tafel_uv.py [<ziel.png>]    -> K5/k5g/K5g3/tafeln/11_uv_kinder.png, Zahlen auf stdout
"""
import hashlib
import os
import shutil
import sys
import tempfile

sys.dont_write_bytecode = True
import numpy as np  # noqa: E402
import cv2  # noqa: E402

HIER = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HIER)
import kf_k5g3_geo as K  # noqa: E402
import kf_k5g3_uv as UVF  # noqa: E402

K5 = K.K5
sys.path.insert(0, os.path.join(K5, 'ruhe', 'skripte'))
import rl  # noqa: E402

G, FE = K.G, K.FE
ABGABE = os.path.join(K5, 'k5g', 'K5g3')
PRUEF = os.path.join(ABGABE, 'pruef')
ZIEL = os.path.join(ABGABE, 'tafeln', '11_uv_kinder.png')
ANSICHTEN = ((1, 0, -60), (2, 0, 10), (0, 180, 20), (2, 90, -45))
GRAU = 20


def variante(tabelle, ordner):
    pe = open(os.path.join(PRUEF, 'BAHNSTEIG_FORM.EMD'), 'rb').read()
    form = FE.lese_emd(pe)
    G.mulde_auf_form(form, tabelle=tabelle)
    bahn = G.emd_mit_md1(pe, form)
    os.makedirs(ordner, exist_ok=True)
    open(os.path.join(ordner, 'BAHNSTEIG_FORM.EMD'), 'wb').write(bahn)
    open(os.path.join(ordner, 'FORM.EMD'), 'wb').write(K.form_emd_bytes(bahn))
    for n in ('BAHNSTEIG_FORM.TIM', 'FORM.TIM', 'form.json'):
        shutil.copy(os.path.join(ABGABE, n), os.path.join(ordner, n))
    return hashlib.sha256(bahn).hexdigest()


def linien(ordner):
    f = FE.lese_emd(open(os.path.join(ordner, 'FORM.EMD'), 'rb').read())
    s = set()
    for t in K.SPALT_TEILE:
        for a in (0, 1):
            for i, fl in enumerate(f['meshes'][t][UVF.ART[a]]):
                if any(UVF.linie(tr) for tr in UVF.render_dreiecke(fl, a)):
                    s.add((t, a, i))
    return s


def titel(img, t, h=20, gross=0.45):
    b = max(img.shape[1], cv2.getTextSize(t, cv2.FONT_HERSHEY_SIMPLEX, gross, 1)[0][0] + 10)
    o = np.full((img.shape[0] + h, b, 3), GRAU, np.uint8)
    o[h:, :img.shape[1]] = img
    cv2.putText(o, t, (4, h - 6), cv2.FONT_HERSHEY_SIMPLEX, gross, (255, 255, 255), 1, cv2.LINE_AA)
    return o


def reihe(bl):
    h = max(b.shape[0] for b in bl)
    aus = []
    for b in bl:
        aus += [np.concatenate([b, np.full((h - b.shape[0], b.shape[1], 3), GRAU, np.uint8)], 0), np.full((h, 6, 3), 255, np.uint8)]
    return np.concatenate(aus[:-1], 1)


def main(ziel=ZIEL):
    tmp = tempfile.mkdtemp(prefix='k5g3_uv_')
    try:
        v1, v2 = os.path.join(tmp, 'r1'), os.path.join(tmp, 'r2')
        s1, s2 = variante(K.GEO3_SPALT, v1), variante(K.GEO3_SPALT_UV, v2)
        abg = hashlib.sha256(open(os.path.join(ABGABE, 'BAHNSTEIG_FORM.EMD'), 'rb').read()).hexdigest()
        print('Runde-1-Variante == K5g3-Abgabe: %s (%s / %s); Runde 2 sha %s' % (s1 == abg, s1[:16], abg[:16], s2[:16]))
        mods = dict(p=rl.fl.lade_var(PRUEF), r1=rl.fl.lade_var(v1), r2=rl.fl.lade_var(v2))
        lin = dict(p=linien(PRUEF), r1=linien(v1), r2=linien(v2))
        zeilen, summe = [], {k: [0, 0] for k in mods}
        for clip, az, el in ANSICHTEN:
            R = {k: rl.weiss_id(mo, clip, az, el, 760) for k, mo in mods.items()}
            ip, Kp = R['p']
            m = np.isin(Kp['mesh'], (21, 22, 23))
            ys, xs = np.nonzero(m)
            y0, y1, x0, x1 = max(ys.min() - 10, 0), ys.max() + 11, max(xs.min() - 10, 0), xs.max() + 11
            f = max(1, min(3, 440 // max(y1 - y0, x1 - x0)))
            kach = []
            for k in ('p', 'r1', 'r2'):
                img, Kk = R[k]
                me, fl = Kk['mesh'], Kk['flaeche']
                netz = np.isin(me, K.SPALT_TEILE)
                mk = np.zeros(me.shape, bool)
                for t, a, i in lin[k]:
                    mk |= (me == t) & (fl == i)
                n = int((mk & netz).sum())
                summe[k][0] += n
                summe[k][1] += int(netz.sum())
                z = lambda b: cv2.resize(b[y0:y1, x0:x1], ((x1 - x0) * f, (y1 - y0) * f), interpolation=cv2.INTER_NEAREST)
                name = {'p': 'ungeteilt (Pruef-Stand)', 'r1': 'Runde 1 (B4-UV)', 'r2': 'Runde 2 (uvfein)'}[k]
                kach.append(titel(z(img), '%s Clip %d %d/%d' % (name, clip, az, el)))
                if k != 'p':
                    ov = img.copy()
                    ov[mk] = (255, 0, 255)
                    kach.append(titel(z(ov), '%s: Linien-Dreiecke %d px' % (name, n)))
            zeilen.append(reihe(kach))
        w = max(z.shape[1] for z in zeilen)
        kopf = titel(np.full((4, w, 3), GRAU, np.uint8), 'K5g3 Geometrie Runde 2: UV der Spalt-Kinder, Textur K5g3, weisses Licht, roh. Magenta = Dreieck, '
                     'dessen UV auf eine Linie faellt (Textur quer gestreckt).', 26, 0.5)
        img = np.concatenate([kopf[:, :w]] + [np.concatenate([z, np.full((z.shape[0], w - z.shape[1], 3), GRAU, np.uint8)], 1) for z in zeilen], 0)
        os.makedirs(os.path.dirname(ziel), exist_ok=True)
        cv2.imwrite(ziel, cv2.cvtColor(img, cv2.COLOR_RGB2BGR))
        print('Tafel', ziel.replace(chr(92), '/'), img.shape[1], 'x', img.shape[0])
        for k, (n, a) in summe.items():
            print('  %s: Linien-Dreieck-Pixel %d von %d Pixeln der Netze 10/16/21/22 (vier Ansichten)' % (k, n, a))
    finally:
        shutil.rmtree(tmp, ignore_errors=True)


if __name__ == '__main__':
    main(sys.argv[1] if len(sys.argv) > 1 else ZIEL)
