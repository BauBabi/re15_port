"""SF V6 drei Punkte (Bauer): Leisten 30-34 (roh, Nearest-Neighbor-Vergroesserung, keine Filter).

    python leisten_v6drei.py --neu EM30_V6.EMD --alt EM30_V5f.EMD --aus DIR [--lauf LAUFORDNER]

30_foto2_vergleich.jpg  Pose P2 (Foto 2): Foto 2 roh | Beschreibung2 (Nutzer) | V5f | neu | 50 % Foto/neu mit Nutzerpunkten (gelb), Foto-
                        Schwanzmaske (gruen), Kette neu (magenta), Spitze (Apex, cyan), Umriss (weiss); ganze Figur x2.
31_foto1_vergleich.jpg  Foto 1 (Vorderansicht): Foto 1 roh | Beschreibung1 | V5f | neu | 50 % (entblendet); Zeile 1 Pose P1, Zeile 2 Pose 135.
32_kopf_nah.jpg         Kopf x4: Zeile Foto 1 (Foto roh | V5f P1 | neu P1 | V5f 135 | neu 135), Zeile Foto 2 (Foto roh | V5f P2 | neu P2).
33_kralle_x8.png        Hand (W)-links x8: Foto n1476 (heller Keil, Messblatt 6) | Foto 1 entblendet | V5f P1 | neu P1.
34_spiel.jpg            Spielbild (RE15_FRAMEDUMP, registriert wie riegel_v4.spielbild / nb2neu c6.port) h135 / h85 gegen das Foto.
"""
import os, sys, argparse
import numpy as np, cv2
HIER = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HIER)
import drei_gemein as DG          # noqa: E402
import leisten_v5f as L5          # noqa: E402
import f_schwanz as FS            # noqa: E402

G, V, B2, F = DG.G, DG.V, DG.b2, DG.F
BOX, BOX_KOPF = L5.BOX, L5.BOX_KOPF
KRALLE_BOX = (466, 330, 516, 380)
VID = os.path.join(G.BAUM, 'analysis', 'befunde_runde41', 'SF_belege', 'roh', 'alle', 'f%04d.png')


def ueberblendung2(foto, mo, img, ids):
    o = L5.ueberblendung(foto, img, ids, 2, F.kette_bild(mo, V.posen()['P2']))
    ap = DG.apex_bild(mo, V.posen()['P2'])
    cv2.drawMarker(o, (int(round(ap[0])), int(round(ap[1]))), (0, 255, 255), cv2.MARKER_TILTED_CROSS, 7, 1)
    return o


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--neu', required=True); ap.add_argument('--alt', required=True); ap.add_argument('--aus', required=True)
    ap.add_argument('--lauf')
    a = ap.parse_args()
    os.makedirs(a.aus, exist_ok=True)
    mn, ma = V.modell(a.neu), V.modell(a.alt)
    P = V.posen()
    nn, na = os.path.basename(a.neu), os.path.basename(a.alt)
    f = 2
    # ---------------- 30 Foto 2
    imn, idn = G.zeichne(mn, P['P2']); ima, ida = G.zeichne(ma, P['P2'])
    f2 = L5.foto_roh(2)
    bx = BOX[2]
    zeile = [B2.tafel(f2, 'Foto 2 roh (n1435)', ['Kamera 50A0 Cut 12'], f, bx),
             L5.beschreibung(2, (bx[3] - bx[1]) * f),
             B2.tafel(ima, 'vorher %s (P2)' % na, ['Clip 12 Bild 48 Gier 861'], f, bx),
             B2.tafel(imn, 'neu %s (P2)' % nn, ['Clip 12 Bild 48 Gier 861'], f, bx),
             B2.tafel(ueberblendung2(f2, mn, imn, idn), '50 %: Foto 2 / neu', ['gelb Nutzer, gruen Foto-Schwanz, magenta Kette, cyan Spitze'], f, bx),
             B2.tafel(ueberblendung2(f2, ma, ima, ida), '50 %: Foto 2 / vorher', ['wie links, Stand %s' % na], f, bx)]
    B2.leiste([zeile], ['SF V6 drei Punkte: 30 Foto 2 (Seitenansicht) - roh, x2',
                        'Haar grau, Schwanz bis zur Nutzer-Spitze (Ende 295/343), Kralle (W)-links hell'],
              os.path.join(a.aus, '30_foto2_vergleich.jpg'))
    # ---------------- 31 Foto 1
    f1r, f1e = L5.foto_roh(1), G.vorlage(1).astype(np.uint8)
    imn1, idn1 = G.zeichne(mn, P['P1']); ima1, _ = G.zeichne(ma, P['P1'])
    imn3, idn3 = G.zeichne(mn, P['135']); ima3, _ = G.zeichne(ma, P['135'])
    bx = BOX[1]
    z1 = [B2.tafel(f1r, 'Foto 1 roh (n1485)', ['Kamera 50A0 Cut 12'], f, bx),
          L5.beschreibung(1, (bx[3] - bx[1]) * f),
          B2.tafel(ima1, 'vorher %s (P1)' % na, ['Clip 20 Bild 139 Gier 1422'], f, bx),
          B2.tafel(imn1, 'neu %s (P1)' % nn, ['Clip 20 Bild 139 Gier 1422'], f, bx),
          B2.tafel(L5.ueberblendung(f1e, imn1, idn1, 1), '50 %: Foto 1 entblendet / neu', ['gelb Nutzer, weiss Umriss'], f, bx)]
    z2 = [B2.tafel(f1r, 'Foto 1 roh', ['Pose 135 = Kopf von vorn'], f, bx),
          L5.beschreibung(1, (bx[3] - bx[1]) * f),
          B2.tafel(ima3, 'vorher (135)', ['Clip 0 Bild 9 Gier 1737'], f, bx), B2.tafel(imn3, 'neu (135)', ['Clip 0 Bild 9 Gier 1737'], f, bx),
          B2.tafel(L5.ueberblendung(f1e, imn3, idn3, 1), '50 %: Foto 1 / neu (135)', ['gelb Nutzer, weiss Umriss'], f, bx)]
    B2.leiste([z1, z2], ['SF V6 drei Punkte: 31 Foto 1 (Vorderansicht) - roh, x2', 'Zeile 1 Pose P1 (Messblatt 2), Zeile 2 Pose 135 (Kopf von vorn)'],
              os.path.join(a.aus, '31_foto1_vergleich.jpg'))
    # ---------------- 32 Kopf nah
    f4 = 4
    def kbox(ids, g=40):
        ys, xs = np.nonzero(ids == 9)
        cx, cy = int((xs.min() + xs.max()) / 2), int((ys.min() + ys.max()) / 2)
        return (cx - g, cy - g, cx + g, cy + g)
    k1 = [B2.tafel(f1r, 'Foto 1 roh', ['Box %s' % (BOX_KOPF[1],)], f4, BOX_KOPF[1]),
          B2.tafel(ima1, 'vorher P1', ['Kopfmitte des Modells'], f4, kbox(idn1)),
          B2.tafel(imn1, 'neu P1', ['Kopfmitte des Modells'], f4, kbox(idn1)),
          B2.tafel(ima3, 'vorher 135', ['Kopf von vorn'], f4, kbox(idn3)),
          B2.tafel(imn3, 'neu 135', ['Kopf von vorn'], f4, kbox(idn3))]
    k2 = [B2.tafel(f2, 'Foto 2 roh', ['Box %s' % (BOX_KOPF[2],)], f4, BOX_KOPF[2]),
          B2.tafel(ima, 'vorher P2', ['Kopfmitte des Modells'], f4, kbox(idn)),
          B2.tafel(imn, 'neu P2', ['Kopfmitte des Modells'], f4, kbox(idn))]
    B2.leiste([k1, k2], ['SF V6 drei Punkte: 32 Kopf nah (x4, roh) - Haar ergraut (Fleck-Eintraege gleichen Helligkeitsrangs)', ''],
              os.path.join(a.aus, '32_kopf_nah.jpg'))
    # ---------------- 33 Kralle x8
    f8 = 8
    v1476 = cv2.cvtColor(cv2.imread(VID % 1476), cv2.COLOR_BGR2RGB)
    kz = [B2.tafel(v1476, 'Foto n1476 (heller Keil)', ['Messblatt 6: L* 30,3'], f8, KRALLE_BOX),
          B2.tafel(f1e, 'Foto 1 entblendet', ['n1485'], f8, KRALLE_BOX),
          B2.tafel(ima1, 'vorher P1', ['%s' % na], f8, KRALLE_BOX),
          B2.tafel(imn1, 'neu P1', ['%s' % nn], f8, KRALLE_BOX)]
    B2.leiste([kz], ['SF V6 drei Punkte: 33 Kralle (W)-links x8 (roh)', 'Krallenflaechen auf Zeile 491 (Ref2-/Original-Grau), Hand und Sichel unveraendert'],
              os.path.join(a.aus, '33_kralle_x8.png'))
    # ---------------- 34 Spielbild
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
            fo = L5.foto_roh(b) if b == 2 else f1e
            bx = BOX[b]
            zs.append([B2.tafel(fo, 'Foto %d%s' % (b, ' entblendet' if b == 1 else ' roh'), ['n%d' % (1350 + h)], f, bx),
                       B2.tafel(fd, 'Spiel h%d (RE15_FRAMEDUMP)' % h, ['Lauf %s' % os.path.basename(os.path.abspath(a.lauf))], f, bx)])
        if zs:
            B2.leiste(zs, ['SF V6 drei Punkte: 34 Spielbild (RE15_FRAMEDUMP, registriert wie riegel_v4.spielbild)', 'Zeile 1 h135 = Foto 1, Zeile 2 h85 = Foto 2'],
                      os.path.join(a.aus, '34_spiel.jpg'))
    print('Leisten geschrieben nach', a.aus)


if __name__ == '__main__':
    main()
