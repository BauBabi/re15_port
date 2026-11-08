"""SF V5 wie Foto - Koerper (Bauer): Leisten 20-24 (roh, Nearest-Neighbor-Vergroesserung, keine Filter).

    python leisten_v5f.py --neu EM30_V5f.EMD --alt EM30_V5k.EMD --aus DIR [--lauf LAUFORDNER]

20_foto2_vergleich.jpg  Pose P2 (Foto 2): Foto 2 roh | Beschreibung2 (Nutzer) | Stand V5k | neu | Ueberblendung 50 % Foto/neu mit
                        Nutzerpunkten (gelb, f_tafeln.NUTZER), Schwanzmaske Foto (gruen) und Modellumriss (weiss); ganze Figur x2.
21_foto1_vergleich.jpg  Pose P1 (Foto 1): Foto 1 roh | Foto 1 entblendet (I' = 17 + (I - 17) / 0,766) | Beschreibung1 | Stand V5k | neu |
                        Ueberblendung (entblendet) mit Nutzerpunkten; dazu Zeile Pose 135 (Kopf von vorn); ganze Figur x2.
22_kopf_nah_beide.jpg   Kopf x4: Zeile Foto 1 (roh | entblendet | neu P1 | neu 135), Zeile Foto 2 (roh | neu P2).
23_spiel.jpg            Spielbild (RE15_FRAMEDUMP, registriert wie riegel_v4.spielbild / nb2neu c6.port) h135 und h85: Foto | neu (Lauf).
24_rot_karte.jpg        Rot-Karte (a* >= 10, Messblatt 4): Foto 1 / Foto 2 (Figur sicher) gegen neu (P1 / P2), rot ueber grau.
"""
import os, sys, argparse
import numpy as np, cv2
from PIL import Image
HIER = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HIER)
import v5f_gemein as F
import f_schwanz as FS
G, B2, V = F.G, F.b2, F.V
NUTZER = {1: {'Auge': (382, 164), 'Haare': (404, 160), 'Nase': (395, 177)},
          2: {'Auge': (460, 234), 'Haare': (468, 241), 'Spitze': (295, 343), 'Beginn': (357, 243), 'Knick': (290, 308)}}   # = f_tafeln.NUTZER
BOX = {1: (260, 120, 540, 460), 2: (240, 160, 530, 450)}
BOX_KOPF = {1: (350, 125, 430, 205), 2: (405, 180, 485, 260)}


def foto_roh(b):
    return np.asarray(Image.open(G.V80 + 'Beschreibung%d_Vorlage.png' % b).convert('RGB'))


def beschreibung(b, h):
    im = np.asarray(Image.open(G.V80 + 'Beschreibung%d.png' % b).convert('RGB'))
    s = h / im.shape[0]
    t = cv2.resize(im, None, fx=s, fy=s, interpolation=cv2.INTER_AREA)
    t = np.ascontiguousarray(t)
    B2.schrift(t, 'Beschreibung%d.png (Nutzer, ganz, auf Hoehe skaliert)' % b, 6, 20, (255, 255, 0), 0.5)
    return t


def ueberblendung(foto, img, ids, b, kp=None):
    o = (0.5 * foto.astype(float) + 0.5 * img.astype(float)).astype(np.uint8).copy()
    c, _ = cv2.findContours((ids >= 0).astype(np.uint8), cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_NONE)
    cv2.drawContours(o, c, -1, (255, 255, 255), 1)
    if b == 2:
        c, _ = cv2.findContours(F.foto_maske().astype(np.uint8), cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_NONE)
        cv2.drawContours(o, c, -1, (0, 255, 0), 1)
        lin = np.array(FS.NUTZER, np.int32)
        cv2.polylines(o, [lin], False, (255, 255, 0), 1)
        if kp is not None:
            cv2.polylines(o, [np.round(np.asarray(kp[1:])).astype(np.int32)], False, (255, 0, 255), 1)
    for n, p in NUTZER[b].items():
        cv2.drawMarker(o, p, (255, 255, 0), cv2.MARKER_CROSS, 7, 1)
        B2.schrift(o, n, p[0] + 4, p[1] - 3, (255, 255, 0), 0.32)
    return o


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--neu', required=True); ap.add_argument('--alt', required=True); ap.add_argument('--aus', required=True)
    ap.add_argument('--lauf')
    a = ap.parse_args()
    mn, ma = V.modell(a.neu), V.modell(a.alt)
    P = V.posen()
    nn, na = os.path.basename(a.neu), os.path.basename(a.alt)
    # ---------------- 20 Foto 2
    imn, idn = G.zeichne(mn, P['P2']); ima, ida = G.zeichne(ma, P['P2'])
    f2 = foto_roh(2)
    kp = F.kette_bild(mn, P['P2'])
    bx = BOX[2]; f = 2
    zeile = [B2.tafel(f2, 'Foto 2 roh (n1435)', ['Kamera 50A0 Cut 12'], f, bx),
             beschreibung(2, (bx[3] - bx[1]) * f),
             B2.tafel(ima, 'Stand %s (P2)' % na, ['Clip 12 Bild 48 Gier 861'], f, bx),
             B2.tafel(imn, 'neu %s (P2)' % nn, ['Clip 12 Bild 48 Gier 861'], f, bx),
             B2.tafel(ueberblendung(f2, imn, idn, 2, kp), '50 %: Foto 2 / neu', ['gelb Nutzer, gruen Foto-Schwanz, magenta Kette, weiss Umriss'], f, bx)]
    B2.leiste([zeile], ['SF V5 wie Foto - Koerper: 20 Foto 2 (Seitenansicht) - roh, x2',
                        'Nutzer 1/3: Schwanz dick, dunkel gruenlich-grau, ein Stueck; Nutzer 5/8: Rot nur Brust und oberer Arm'],
              os.path.join(a.aus, '20_foto2_vergleich.jpg'))
    # ---------------- 21 Foto 1
    f1r, f1e = foto_roh(1), G.vorlage(1).astype(np.uint8)
    imn1, idn1 = G.zeichne(mn, P['P1']); ima1, ida1 = G.zeichne(ma, P['P1'])
    imn3, idn3 = G.zeichne(mn, P['135']); ima3, ida3 = G.zeichne(ma, P['135'])
    bx = BOX[1]
    z1 = [B2.tafel(f1r, 'Foto 1 roh (n1485)', ['Kamera 50A0 Cut 12'], f, bx),
          B2.tafel(f1e, 'Foto 1 entblendet', ["I' = 17 + (I - 17) / 0,766"], f, bx),
          beschreibung(1, (bx[3] - bx[1]) * f),
          B2.tafel(ima1, 'Stand %s (P1)' % na, ['Clip 20 Bild 139 Gier 1422'], f, bx),
          B2.tafel(imn1, 'neu %s (P1)' % nn, ['Clip 20 Bild 139 Gier 1422'], f, bx),
          B2.tafel(ueberblendung(f1e, imn1, idn1, 1), '50 %: Foto 1 entblendet / neu', ['gelb Nutzer, weiss Umriss'], f, bx)]
    z2 = [B2.tafel(f1r, 'Foto 1 roh', ['Pose 135 = Kopf von vorn'], f, bx), B2.tafel(f1e, 'Foto 1 entblendet', [''], f, bx),
          B2.tafel(ima3, 'Stand (135)', ['Clip 0 Bild 9 Gier 1737'], f, bx), B2.tafel(imn3, 'neu (135)', ['Clip 0 Bild 9 Gier 1737'], f, bx),
          B2.tafel(ueberblendung(f1e, imn3, idn3, 1), '50 %: Foto 1 / neu (135)', ['gelb Nutzer, weiss Umriss'], f, bx)]
    B2.leiste([z1, z2], ['SF V5 wie Foto - Koerper: 21 Foto 1 (Vorderansicht) - roh, x2',
                         'Zeile 1 Pose P1 (Messblatt 2), Zeile 2 Pose 135 (Kopf von vorn)'], os.path.join(a.aus, '21_foto1_vergleich.jpg'))
    # ---------------- 22 Kopf nah
    f4 = 4
    def kbox(ids, g=40):
        ys, xs = np.nonzero(ids == 9)
        cx, cy = int((xs.min() + xs.max()) / 2), int((ys.min() + ys.max()) / 2)
        return (cx - g, cy - g, cx + g, cy + g)
    k1 = [B2.tafel(f1r, 'Foto 1 roh', ['Box %s' % (BOX_KOPF[1],)], f4, BOX_KOPF[1]),
          B2.tafel(f1e, 'Foto 1 entblendet', [''], f4, BOX_KOPF[1]),
          B2.tafel(imn1, 'neu P1', ['Kopfmitte des Modells'], f4, kbox(idn1)),
          B2.tafel(imn3, 'neu 135', ['Kopfmitte des Modells'], f4, kbox(idn3))]
    k2 = [B2.tafel(f2, 'Foto 2 roh', ['Box %s' % (BOX_KOPF[2],)], f4, BOX_KOPF[2]),
          B2.tafel(imn, 'neu P2', ['Kopfmitte des Modells'], f4, kbox(idn))]
    B2.leiste([k1, k2], ['SF V5 wie Foto - Koerper: 22 Kopf nah (x4, roh) - Kopf unveraendert V6 (Original RE1.5-Kopf)', ''],
              os.path.join(a.aus, '22_kopf_nah_beide.jpg'))
    # ---------------- 24 Rot-Karte
    zr = []
    for b, pn in ((1, 'P1'), (2, 'P2')):
        r = F.rot_karte(mn, b)
        fo = foto_roh(b) if b == 2 else f1e
        g1 = cv2.cvtColor(cv2.cvtColor(fo, cv2.COLOR_RGB2GRAY), cv2.COLOR_GRAY2RGB) // 2
        g1[r['rot_foto']] = (255, 40, 40)
        g2 = cv2.cvtColor(cv2.cvtColor(r['img'], cv2.COLOR_RGB2GRAY), cv2.COLOR_GRAY2RGB) // 2
        g2[r['rot_modell']] = (255, 40, 40)
        bx = BOX[b]
        zr.append([B2.tafel(g1, 'Foto %d: a* >= 10 (Figur sicher)' % b, ['Anteil %.3f' % (r['rot_foto'].sum() / max((F.trimap(b) == 1).sum(), 1))], f, bx),
                   B2.tafel(g2, 'neu %s: a* >= 10' % pn, ['Anteil %.3f' % (r['rot_modell'].sum() / max((r['bone'] >= 0).sum(), 1))], f, bx)])
    B2.leiste(zr, ['SF V5 wie Foto - Koerper: 24 Rot-Karte (Messblatt 4: a* >= 10, Videoraum)', 'Zeile 1 Foto 1 / P1, Zeile 2 Foto 2 / P2'],
              os.path.join(a.aus, '24_rot_karte.jpg'))
    # ---------------- 23 Spielbild
    if a.lauf:
        sys.path.insert(0, os.path.join(os.path.dirname(HIER), 'nb2neu'))
        os.environ['NB6_RUNS'] = os.path.dirname(os.path.abspath(a.lauf))
        import c6
        zs = []
        for h, b in ((135, 1), (85, 2)):
            fd = c6.port(os.path.abspath(a.lauf), h)
            if fd is None:
                continue
            fd = cv2.cvtColor(fd, cv2.COLOR_BGR2RGB)
            fo = foto_roh(b) if b == 2 else f1e
            bx = BOX[b]
            zs.append([B2.tafel(fo, 'Foto %d%s' % (b, ' entblendet' if b == 1 else ' roh'), ['n%d' % (1350 + h)], f, bx),
                       B2.tafel(fd, 'Spiel h%d (RE15_FRAMEDUMP)' % h, ['Lauf %s' % os.path.basename(os.path.abspath(a.lauf))], f, bx)])
        if zs:
            B2.leiste(zs, ['SF V5 wie Foto - Koerper: 23 Spielbild (RE15_FRAMEDUMP, registriert wie riegel_v4.spielbild)', 'Zeile 1 h135 = Foto 1, Zeile 2 h85 = Foto 2'],
                      os.path.join(a.aus, '23_spiel.jpg'))
    print('Leisten geschrieben nach', a.aus)


if __name__ == '__main__':
    main()
