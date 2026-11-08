"""SF Messblatt Fotovorlage, Schritt 7: Overlay-Tafeln (roh; Foto 1 entblendet wie Messblatt V4, Foto 2 roh).

Je Foto eine Leiste: Foto | Modell V5r4 in der gefundenen Pose | 50 %-Ueberblendung mit Foto-Umriss (gruen = Figur sicher,
rot = unbekannt), Nutzerpunkten (gelbe Kreuze = Linienenden Beschreibung1/2) und Befund-Markern (Legende in der Kachel).
Zusatz: Rot-Karte Foto | Modell; Pose alt (posen_v4) | neu.
"""
import os, sys, json, numpy as np, cv2
sys.path.insert(0, os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', 'v4'))
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import v4_gemein as G, b2
S = os.path.join(G.SFV4, 'foto')
AUS = os.path.join(G.BAUM, 'analysis', 'befunde_runde41', 'SF_belege', 'foto')
BOX = {1: (250, 120, 530, 460), 2: (250, 160, 530, 420)}
NUTZER = {1: {'Auge': (382, 164), 'Haare': (404, 160), 'Nase': (395, 177)},
          2: {'Auge': (460, 234), 'Haare': (468, 241), 'Spitze': (295, 343), 'Beginn': (357, 243), 'Knick': (290, 308)}}
F = 3


def u8(a):
    return np.clip(a, 0, 255).astype(np.uint8)


def kontur(bild, maske, box, col, dick=1):
    x0, y0, x1, y1 = box
    m = cv2.resize(maske[y0:y1, x0:x1].astype(np.uint8), None, fx=F, fy=F, interpolation=cv2.INTER_NEAREST)
    cs, _ = cv2.findContours(m, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_NONE)
    cv2.drawContours(bild, cs, -1, col, dick)


def pkt(bild, p, box, col, r=6, art='x'):
    x0, y0 = box[:2]
    X, Y = int((p[0] - x0 + 0.5) * F), int((p[1] - y0 + 0.5) * F)
    if art == 'x':
        cv2.line(bild, (X - r, Y - r), (X + r, Y + r), col, 2); cv2.line(bild, (X - r, Y + r), (X + r, Y - r), col, 2)
    else:
        cv2.circle(bild, (X, Y), r, col, 2)
    return X, Y


def kachel(rgb, box):
    x0, y0, x1, y1 = box
    return np.ascontiguousarray(cv2.resize(u8(rgb)[y0:y1, x0:x1], None, fx=F, fy=F, interpolation=cv2.INTER_NEAREST))


def main():
    A = json.load(open(os.path.join(S, 'analyse.json')))
    SW = json.load(open(os.path.join(S, 'schwanz.json')))
    KR = json.load(open(os.path.join(S, 'krallen.json')))
    KF = json.load(open(os.path.join(S, 'kopf_foto.json')))
    mo = b2.modell(G.EMD_V4)
    os.makedirs(AUS, exist_ok=True)
    for b in (1, 2):
        a = A[str(b)]; box = BOX[b]
        pose = a['pose']
        v = G.vorlage(b)
        img, ids = G.zeichne(mo, pose)
        T = np.load(os.path.join(S, 'trimap_%d.npy' % b))
        t1 = kachel(v, box); t2 = kachel(img, box)
        t3 = kachel(0.5 * v + 0.5 * img.astype(float), box)
        kontur(t3, T == 1, box, (0, 255, 0), 1); kontur(t3, T == 2, box, (255, 0, 0), 1)
        kontur(t3, ids >= 0, box, (255, 255, 255), 1)
        ab = np.load(os.path.join(S, 'rot_ab_%d.npy' % b))
        kontur(t3, cv2.morphologyEx(ab.astype(np.uint8), cv2.MORPH_CLOSE, np.ones((3, 3), np.uint8)) > 0, box, (255, 0, 255), 1)
        for n, p in NUTZER[b].items():
            X, Y = pkt(t3, p, box, (255, 255, 0)); b2.schrift(t3, n, X + 8, Y - 4, (255, 255, 0), 0.45)
        # Augen: Foto-Fleck (cyan Kreis), Modell (W)-rechts / (W)-links Projektion (orange / blau)
        au = KF['auge'][str(b)]['schwerpunkt']; pkt(t3, au, box, (0, 255, 255), 9, 'o')
        sp = a['kopf_seiten_projektion']
        X, Y = pkt(t3, sp['W_rechts'], box, (255, 140, 0), 5); b2.schrift(t3, 'W-re', X + 6, Y + 14, (255, 140, 0), 0.4)
        X, Y = pkt(t3, sp['W_links'], box, (80, 160, 255), 5); b2.schrift(t3, 'W-li', X + 6, Y + 14, (80, 160, 255), 0.4)
        # Maul: Modell-Fussabdruck Zeile 487 (orange Kontur), Foto-Maulband (Foto 1) cyan Rechteck
        kontur(t3, np.load(os.path.join(S, 'maul_fuss_%d.npy' % b)), box, (255, 140, 0), 1)
        if b == 1:
            x0, y0, x1, y1 = a['maul_band_foto_im_modell']['band']
            cv2.rectangle(t3, ((x0 - box[0]) * F, (y0 - box[1]) * F), ((x1 + 1 - box[0]) * F, (y1 + 1 - box[1]) * F), (0, 255, 255), 1)
        # Huckel: Spalten, in denen die Modell-Oberkante >= 4 px ueber der Foto-Kontur liegt (rote Striche oben)
        for xs_, k in a['obere_kontur'].items():
            if k['d'] is not None and k['d'] >= 4:
                X = int((int(xs_) - box[0] + 0.5) * F); Y = int((k['modell'] - box[1]) * F)
                cv2.line(t3, (X, Y), (X, Y + int(k['d'] * F)), (255, 60, 60), 1)
        if b == 2:
            for g in SW['modell']['gliedgrenzen']:
                X, Y = pkt(t3, g['punkt'], box, (255, 0, 255), 4, 'o')
                b2.schrift(t3, g['grenze'], X + 5, Y, (255, 0, 255), 0.35)
            kp = np.array(SW['modell']['kette_bild'])
            cv2.polylines(t3, [((kp - box[:2] + 0.5) * F).astype(np.int32)], False, (255, 0, 255), 1)
            nu = np.array([NUTZER[2][k] for k in ('Beginn',)] + [(295, 252)] + [NUTZER[2][k] for k in ('Knick', 'Spitze')], float)
            cv2.polylines(t3, [((nu - box[:2] + 0.5) * F).astype(np.int32)], False, (255, 255, 0), 1)
        else:
            kb = KR['foto']['1476']['rot_box']
            if kb:
                cv2.rectangle(t3, ((kb[0] - box[0]) * F, (kb[1] - box[1]) * F), ((kb[2] + 1 - box[0]) * F, (KR['foto']['1476']['hand_unterkante'] + 1 - box[1]) * F), (0, 255, 255), 1)
            for kk in KR['modell']['krallen']:
                pkt(t3, kk['spitze_bild'], box, (255, 0, 255), 4, 'o')
        T1 = 'Foto %d %s' % (b, 'n1485 entblendet' if b == 1 else 'n1435 roh')
        T2 = 'V5r4 Pose clip %d bild %d gier %d (%.0f Grad) x %d z %d' % (pose['clip'], pose['bild'], pose['gier'], pose['gier'] * 360 / 4096, pose['x'], pose['z'])
        T3 = '50%%: gruen Foto-Figur, rot unbekannt, weiss Modell, magenta Rot-Abw., gelb Nutzer'
        unten3 = ['cyan Kreis = Foto-Auge; orange/blau x = Modell-Kopf (W)-rechts/(W)-links; orange Kontur = Modell-Maul (Zeile 487)',
                  'rote Striche = Modell-Oberkante >= 4 px ueber Foto-Kontur (Huckel)' + ('; magenta Kreise = Gliedgrenzen 16/17..21/22, Kette' if b == 2 else '; magenta Kreise = Modell-Krallenspitzen, cyan Box = Foto-Hand n1476')]
        import f_pose as FP
        FP._init(b); An = FP.anker(b)
        bn = FP.bewerte(FP.umriss_ids(mo, pose), FP._T, An); ba = FP.bewerte(FP.umriss_ids(mo, G.posen()['135' if b == 1 else '85']), FP._T, An)
        p = dict(iou_alt=ba[0])
        unten2 = ['IoU %.3f (alt posen_v4 %.3f); Anker neu %s' % (bn[0], ba[0], {k: round(v, 2) for k, v in bn[1].items()}),
                  'Anker alt %s' % {k: round(v, 2) for k, v in ba[1].items()}]
        G_ = b2.leiste([[b2.tafel(t1, T1, f=1), b2.tafel(t2, T2, unten2, f=1), b2.tafel(t3, T3, unten3, f=1)]],
                       ['SF Messblatt Fotovorlage - Foto %d gegen V5r4 in der gefundenen Pose (Kamera 50A0/12), x%d' % (b, F)],
                       os.path.join(AUS, '0%d_overlay_foto%d.jpg' % (b, b)))
        # Rot-Karte
        rf = np.load(os.path.join(S, 'rot_foto_%d.npy' % b)); rm = np.load(os.path.join(S, 'rot_modell_%d.npy' % b))
        k1 = kachel(v * 0.45, box); k2 = kachel(img.astype(float) * 0.45, box)
        for kk, m in ((k1, rf), (k2, rm)):
            mm = cv2.resize(m[box[1]:box[3], box[0]:box[2]].astype(np.uint8), None, fx=F, fy=F, interpolation=cv2.INTER_NEAREST) > 0
            kk[mm] = (255, 40, 40)
        kontur(k2, ab, box, (255, 0, 255), 1)
        ra = a['rot_anteil']
        b2.leiste([[b2.tafel(k1, 'Foto %d: a* >= 10 (rot), Figur sicher' % b, ['Anteil %.3f' % ra['foto']], f=1),
                    b2.tafel(k2, 'Modell V5r4 (Videoraum): a* >= 10 (rot); magenta = rot im Modell, Foto a* < 6', ['Anteil %.3f' % ra['modell']], f=1)]],
                  ['SF Messblatt Fotovorlage - Rot-Karte Foto %d' % b], os.path.join(AUS, '1%d_rotkarte_foto%d.jpg' % (b, b)))
        # Pose alt | neu
        alt = G.posen()['135' if b == 1 else '85']
        ia, _ = G.zeichne(mo, alt)
        b2.leiste([[b2.tafel(kachel(v, box), T1, f=1), b2.tafel(kachel(ia, box), 'alt posen_v4 "%s" (IoU %.3f)' % ('135' if b == 1 else '85', p['iou_alt']), f=1),
                    b2.tafel(t2, 'neu: ' + T2, unten2, f=1)]],
                  ['SF Messblatt Fotovorlage - Vergleichsansicht Foto %d: Pose alt gegen neu' % b], os.path.join(AUS, '2%d_pose_alt_neu_foto%d.jpg' % (b, b)))
    print('ok')


if __name__ == '__main__':
    main()
