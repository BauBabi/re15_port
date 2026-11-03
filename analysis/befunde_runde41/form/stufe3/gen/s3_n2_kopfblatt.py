"""Nachbesserung 2 (E22), Kopf: Belegblaetter fuer den Nutzer (alt K3 | neu K4).

    python s3_n2_kopfblatt.py maske AUS.png                 Screenshot 2 mit Nutzer-Teilen Ober-/Unterkiefer, je Satz sichtbarer Kopf (22) und
                                                             Kiefer (23) aus dem Tiefenpuffer der gebauten Variante (Pose Bild 2), dazu die Referenz
                                                             "nur Maske" (Fit ohne Krokodil-Grenzen, nicht gebaut)
    python s3_n2_kopfblatt.py rundum AUS.png                Offline-Buehne mit Spiel-Licht (ROOM5090 Schnitt 11), Form an der Lage von Bild 2,
                                                             Kamera kreist um den Kopf (8 Richtungen), je Satz Ruhe (Maul zu) und Pose Bild 2
    python s3_n2_kopfblatt.py spiel AUS.png                 echtes Fenster (re15_port/build_n2/spiel/kopf_<satz>_<raum>_g<gier>, letztes Bild),
                                                             Ausschnitt um den Kopf, je Ansicht eine Zeile, Spalten alt | neu
Kein RE-Wert."""
import os, sys, json, math, glob
import numpy as np
from PIL import Image, ImageDraw
import cv2
HIER = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HIER)
import s3_rig as S          # noqa: E402
import form_buehne as FB    # noqa: E402
import form_emd as FE       # noqa: E402
K = os.path.join(S.FORM_DIR, 'varianten', 'K_alligator')
sys.path.insert(0, os.path.join(K, 'generator'))
import k_kern as KK         # noqa: E402
import k_mass as KM         # noqa: E402

# Basis der Spiel-Assets (Geometrie = Variante, Textur wie im Spiel): NB1 = K3 + Texturfix NB1, NB2 = K4 + Textur-Bau NB2
SAETZE = [('alt K3 (NB1)', os.path.join(S.FORM_DIR, 'stufe3', 'basis_n1')), ('neu K4 Krokodil (NB2)', os.path.join(S.FORM_DIR, 'stufe3', 'basis_n2'))]
REFERENZ = os.path.join(S.FORM_DIR, 'stufe3', 'nachbesserung2', 'kopf', 'fits', 'Kfrei_kopf.json')   # Kopie von varianten/K_alligator/_lauf/Kfrei/kopf.json
SPIEL = os.path.join(S.WURZEL, 're15_port', 'build_n2', 'spiel')
ANS = [('5080', 3072, 'frontal'), ('5080', 2560, '3/4 von vorn'), ('5080', 2048, 'seitlich'), ('5090', 3874, 'Lage/Pose Bild 2, 3/4 von vorn'),
       ('5090', 0, 'frontal'), ('5090', 802, 'seitlich')]


def _kontur(img, m, col, s):
    m3 = cv2.resize(m.astype(np.uint8) * 255, None, fx=s, fy=s, interpolation=cv2.INTER_NEAREST)
    cs, _ = cv2.findContours(m3, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_NONE)
    cv2.drawContours(img, cs, -1, col, 2)


def maske(aus):
    ok, uk = KK.maske('oberkiefer'), KK.maske('unterkiefer')
    bild = cv2.imread(os.path.join(KK.FORM, 'bild2.png'))
    x0, y0, x1, y1, s = 150, 190, 440, 440, 3
    kacheln = []
    for name, d in SAETZE:
        idb = KM.id2(d)
        kacheln.append((name, idb))
    if os.path.exists(REFERENZ):
        import k_kopf_fit as F
        import k_kopf_fit2 as F2
        import k_form as KF
        j = json.load(open(REFERENZ, encoding='utf-8'))
        form, mo, fj = KK.lade()
        b2 = F.Buehne(2, form, mo, fj)
        p = dict(KF.VORGABE)
        p.update(j['form'])
        w = j['bild2']['winkel']
        _t, idb = b2.rendern(F2.geometrie(p, b2.hals, b2.hals_f), w['21'], w['22'], w['23'])
        kacheln.append(('Referenz: Fit ohne Krokodil-Grenzen (nicht gebaut)', idb))
    spalten = []
    for name, idb in kacheln:
        a = cv2.resize(bild[y0:y1, x0:x1], None, fx=s, fy=s, interpolation=cv2.INTER_NEAREST)
        hell = a.copy()
        k22 = cv2.resize((idb[y0:y1, x0:x1] == 22).astype(np.uint8), None, fx=s, fy=s, interpolation=cv2.INTER_NEAREST) > 0
        k23 = cv2.resize((idb[y0:y1, x0:x1] == 23).astype(np.uint8), None, fx=s, fy=s, interpolation=cv2.INTER_NEAREST) > 0
        hell[k22] = (0.45 * hell[k22] + 0.55 * np.array([0, 220, 255])).astype(np.uint8)
        hell[k23] = (0.45 * hell[k23] + 0.55 * np.array([220, 0, 220])).astype(np.uint8)
        _kontur(hell, ok[y0:y1, x0:x1], (0, 140, 255), s)
        _kontur(hell, uk[y0:y1, x0:x1], (255, 120, 200), s)
        iou_ok = KK.iou(idb == 22, ok)
        iou_uk = KK.iou(idb == 23, uk)
        spalten.append((name, np.vstack([a, hell]), iou_ok, iou_uk))
    W = sum(c[1].shape[1] for c in spalten) + 10 * (len(spalten) - 1)
    H = spalten[0][1].shape[0] + 60
    out = Image.new('RGB', (W, H), (18, 18, 22))
    d = ImageDraw.Draw(out)
    d.text((6, 6), 'Screenshot 2, Nutzer-Teile Oberkiefer (orange) / Unterkiefer (rosa); sichtbarer Kopf 22 gelb, Kiefer 23 magenta '
           '(Tiefenpuffer, Pose Bild 2, mit Gelenkkoerpern). Oben Screenshot, unten Overlay.', fill=(240, 200, 120))
    x = 0
    for name, a, io, iu in spalten:
        d.text((x + 4, 24), '%s: IoU Oberkiefer %.3f, Unterkiefer %.3f' % (name, io, iu), fill=(220, 220, 220))
        out.paste(Image.fromarray(a[:, :, ::-1]), (x, 44))
        x += a.shape[1] + 10
    out.save(aus)
    print(aus, [(c[0], round(c[2], 3), round(c[3], 3)) for c in spalten])


def _kopf_welt(d, clip, bild, lage, gier):
    emd = open(os.path.join(d, 'FORM.EMD'), 'rb').read()
    mo = FE.modell(emd, open(os.path.join(d, 'FORM.TIM'), 'rb').read())
    R, T = mo.pose(FB.bild_von_clip(mo, clip, bild))
    Rw, Tw = KK.welt_pose(R, T, lage, gier)
    return Tw[22] + Rw[22] @ np.array([700.0, 150.0, 0.0])


def rundum(aus):
    R = FB.Raum('5090')
    licht = R.licht(11)
    lage, gier = (941, 0, -23776), 3874
    zeilen = []
    for name, d in SAETZE:
        emd = open(os.path.join(d, 'FORM.EMD'), 'rb').read()
        tim = open(os.path.join(d, 'FORM.TIM'), 'rb').read()
        for clip, wort in ((2, 'Ruhe, Maul zu'), (1, 'Pose Bild 2, Maul offen')):
            mitte = _kopf_welt(d, clip, 0, lage, gier)
            reihe = []
            for k in (-90, -60, -30, 0, 30, 60, 90):
                a = math.radians(gier * 360.0 / 4096.0 + k)
                pos = (mitte[0] + 4200 * math.cos(a), mitte[1] - 1500, mitte[2] - 4200 * math.sin(a))
                kam = FB.Kamera.aus_rid(pos, mitte, fov=26684)
                r, _m = FB.buehne(emd, tim, clip, 0, lage, gier, kam, licht, None)
                reihe.append(r[60:420, 120:520])
            zeilen.append(('%s, %s' % (name, wort), reihe))
    tw, th = zeilen[0][1][0].shape[1], zeilen[0][1][0].shape[0]
    out = Image.new('RGB', (7 * (tw + 6), 30 + len(zeilen) * (th + 24)), (18, 18, 22))
    dr = ImageDraw.Draw(out)
    dr.text((6, 6), 'Rundum offline mit Spiel-Licht (ROOM5090 Schnitt 11, Lage von Bild 2), Kamera vor dem Kopf von -90 bis +90 Grad in 30-Grad-Schritten (Mitte = von vorn), leicht von oben',
            fill=(240, 200, 120))
    y = 30
    for text, reihe in zeilen:
        dr.text((6, y + 4), text, fill=(220, 220, 220))
        for i, r in enumerate(reihe):
            out.paste(Image.fromarray(r), (i * (tw + 6), y + 22))
        y += th + 24
    out.save(aus)
    print(aus, out.size)


def letztes(name):
    b = sorted(glob.glob(os.path.join(SPIEL, name, 'bild', 'b_*.png')))
    return Image.open(b[-1]).convert('RGB') if b else None


BOX = {'5080': (60, 0, 620, 440), '5090': (120, 100, 520, 440)}


def spiel(aus):
    tafeln = []
    for raum, gier, wort in ANS:
        ims = [letztes('kopf_%s_%s_g%d' % (t, raum, gier)) for t in ('K3', 'K4')]
        if any(i is None for i in ims):
            print('fehlt', raum, gier)
            continue
        ims = [i.crop(BOX[raum]) for i in ims]
        ims = [i.resize((i.size[0] * 3 // 2, i.size[1] * 3 // 2), Image.NEAREST) for i in ims]
        tafeln.append(('%s %s (Gier %d), Spielkamera %s, echtes Fenster: links alt K3, rechts neu K4' % (
            raum, wort, gier, '5080 Schnitt 4' if raum == '5080' else '5090 Schnitt 11'), ims))
    W = max(sum(i.size[0] for i in ims) + 8 for _t, ims in tafeln)
    H = 30 + sum(ims[0].size[1] + 24 for _t, ims in tafeln)
    out = Image.new('RGB', (W, H), (18, 18, 22))
    d = ImageDraw.Draw(out)
    d.text((6, 8), 'Kopf aus den Spielkameras (eigene exe-Kopie, gdigrab, Renderer des Nutzers): alt K3 | neu K4 Krokodil', fill=(240, 200, 120))
    y = 30
    for text, ims in tafeln:
        d.text((6, y + 4), text, fill=(220, 220, 220))
        x = 0
        for i in ims:
            out.paste(i, (x, y + 22))
            x += i.size[0] + 8
        y += ims[0].size[1] + 24
    out.save(aus)
    print(aus, out.size)


if __name__ == '__main__':
    {'maske': maske, 'rundum': rundum, 'spiel': spiel}[sys.argv[1]](sys.argv[2])
