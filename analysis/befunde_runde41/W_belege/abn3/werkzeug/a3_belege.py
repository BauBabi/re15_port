# Abnahme 3 Spur W: Bildbelege aus den RE15_FRAMEDUMP-Serien (320x240) nach W_belege/abn3/.
import os, sys, numpy as np
from PIL import Image, ImageDraw
M = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
R = os.path.join(M, 'runs')
ZIEL = sys.argv[1]
def ppm(lauf, name):
    d = open(os.path.join(R, lauf, name), 'rb').read(); parts = d.split(b'\n', 3); w, h = map(int, parts[1].split())
    return np.frombuffer(parts[3][:w*h*3], dtype=np.uint8).reshape(h, w, 3)
def text(img, xy, t, farbe=(255, 255, 0)):
    ImageDraw.Draw(img).text(xy, t, fill=farbe)
def ausschnitt_reihe(paare, praefix, bilder, kasten, z, datei, titel):
    """paare: [(lauf, beschriftung)], bilder: Bildnummern, kasten: (x0,y0,x1,y1) in 320x240, z: Vergroesserung."""
    x0, y0, x1, y1 = kasten; w = (x1 - x0) * z; h = (y1 - y0) * z
    rand = 4; kopf = 14; links = 150
    out = Image.new('RGB', (links + len(bilder) * (w + rand), kopf + len(paare) * (h + kopf + rand)), (20, 20, 20))
    text(out, (4, 2), titel)
    for j, f in enumerate(bilder):
        text(out, (links + j * (w + rand), kopf - 12 + 12), 'F%d' % f, (200, 200, 200))
    for i, (lauf, besch) in enumerate(paare):
        y = kopf + 12 + i * (h + kopf + rand)
        text(out, (4, y + h // 2 - 5), besch[:24], (255, 255, 255))
        for j, f in enumerate(bilder):
            a = ppm(lauf, '%s%06d.ppm' % (praefix, f))[y0:y1, x0:x1]
            im = Image.fromarray(a).resize((w, h), Image.NEAREST)
            out.paste(im, (links + j * (w + rand), y))
    out.save(os.path.join(ZIEL, datei))
def voll_paar(paare, datei, titel, markiere=None):
    ims = []
    for lauf, name, besch in paare:
        im = Image.fromarray(ppm(lauf, name)).resize((640, 480), Image.NEAREST)
        dr = ImageDraw.Draw(im)
        if markiere:
            mx, my = markiere; dr.ellipse((mx*2 - 14, my*2 - 14, mx*2 + 14, my*2 + 14), outline=(255, 0, 0), width=2)
        dr.rectangle((0, 462, 640, 480), fill=(0, 0, 0)); dr.text((4, 465), besch, fill=(255, 255, 0))
        ims.append(im)
    out = Image.new('RGB', (640 * len(ims) + 6 * (len(ims) - 1), 496), (20, 20, 20))
    for i, im in enumerate(ims): out.paste(im, (i * 646, 16))
    ImageDraw.Draw(out).text((4, 2), titel, fill=(255, 255, 255))
    out.save(os.path.join(ZIEL, datei))
B = list(range(544, 568, 2))
ausschnitt_reihe([('L2_marke', 'Leon 2000 Abnahme-exe'), ('K_L2_marke', 'Leon Gegenprobe sub07'), ('E2_marke_b', 'Elza 2001 Abnahme-exe'), ('K_E2_marke_b', 'Elza Gegenprobe sub08')],
                 'a_', B, (68, 0, 100, 12), 8, 'abn3_cut2_rand_leon_elza.png',
                 'Cut 2 (Marke), oberer Rand x 68..99 y 0..11, 8-fach. Gegenprobe = dieselbe exe mit RE15_SUBSTART (Mod-Verhalten): Punkt 7 (83,1) ab F546')
ausschnitt_reihe([('L5_cut', 'Leon 2000 Cut 5 Abnahme'), ('K_L5_cut', 'Leon Cut 5 Gegenprobe'), ('E5_cut', 'Elza 2001 Cut 5 Abnahme')],
                 'c_', B, (178, 0, 210, 12), 8, 'abn3_cut5_rand.png', 'Cut 5 (FORCE_CUT), x 178..209 y 0..11, 8-fach: Tropfen-Punkt (193..194,1..2) nur in der Gegenprobe')
ausschnitt_reihe([('L7_cut', 'Leon 2000 Cut 7 Abnahme'), ('K_L7_cut', 'Leon Cut 7 Gegenprobe'), ('E7_cut', 'Elza 2001 Cut 7 Abnahme')],
                 'c_', B, (154, 6, 186, 24), 8, 'abn3_cut7_rand.png', 'Cut 7 (FORCE_CUT), x 154..185 y 6..23, 8-fach: Tropfen-Punkt (169,15) nur in der Gegenprobe')
voll_paar([('L2_marke', 'a_000600.ppm', 'Leon ROOM2000 Cut 2 F600 - Abnahme-exe (kein Tropfen)'),
           ('K_L2_marke', 'a_000600.ppm', 'Leon ROOM2000 Cut 2 F600 - Gegenprobe RE15_SUBSTART=7')], 'abn3_cut2_voll_leon_f600.png',
          'Cut 2 Marke, F600, roter Kreis = Punkt 7 (83,1)', markiere=(83, 1))
voll_paar([('E2_marke_b', 'a_000600.ppm', 'Elza ROOM2001 Cut 2 F600 - Abnahme-exe (kein Tropfen)'),
           ('K_E2_marke_b', 'a_000600.ppm', 'Elza ROOM2001 Cut 2 F600 - Gegenprobe RE15_SUBSTART=8')], 'abn3_cut2_voll_elza_f600.png',
          'Cut 2 Marke, F600, roter Kreis = Punkt 7 (83,1)', markiere=(83, 1))
voll_paar([('LD_L', 'l_000600.ppm', 'Leon nach CONTINUE (Spielstand ROOM2000) Cut 2 F600'),
           ('LD_E', 'l_000600.ppm', 'Elza nach CONTINUE (Spielstand ROOM2001) Cut 2 F600')], 'abn3_laden_cut2_f600.png',
          'Laden eines Spielstands: kein Tropfen am Rand (roter Kreis = Lage von Punkt 7)', markiere=(83, 1))
voll_paar([('RW_L', 'r_000600.ppm', 'Leon 2000 -> 2010 -> 2000, zweiter Eintritt Cut 14 F600'),
           ('RW_E', 'r_000600.ppm', 'Elza 2001 -> 2011 -> 2001, zweiter Eintritt Cut 14 F600')], 'abn3_rueckweg_cut14_f600.png',
          'Raumwechsel hin und zurueck (Tuer Slot 2 / Rueckweg 2010 Slot 0), zweiter Eintritt')
voll_paar([('L10_cut', 'w_000300.ppm', 'Leon 2000 Cut 10 F300 (Wasserlauf + Gitterstrahlen)'),
           ('L10_cut', 'w_000315.ppm', 'Leon 2000 Cut 10 F315'),
           ('E10_cut', 'w_000315.ppm', 'Elza 2001 Cut 10 F315')], 'abn3_cut10_wasserlauf.png',
          'Cut 10 Leiter: Wasserlauf der Spur N und die drei Gitterstrahlen 0x0B laufen weiter')
# Wechselzahl-Karten (|d| > 16 je Bildpaar) ueber dem Medianbild
def karte_bild(lauf, besch):
    k = np.load(os.path.join(R, lauf, 'karte.npy')); med = np.load(os.path.join(R, lauf, 'median.npy'))
    im = (med.astype(float) * 0.5).astype(np.uint8)
    m = k >= 6
    im[m] = [255, 40, 40]
    im[(k > 0) & ~m] = [255, 200, 0]
    p = Image.fromarray(im).resize((640, 480), Image.NEAREST)
    dr = ImageDraw.Draw(p); dr.rectangle((0, 462, 640, 480), fill=(0, 0, 0)); dr.text((4, 465), besch, fill=(255, 255, 0))
    return p
ims = [karte_bild('L2_marke', 'Leon 2000 Cut 2 F520..F1300 Abnahme'), karte_bild('K_L2_marke', 'Leon Gegenprobe F520..F720'),
       karte_bild('E2_marke', 'Elza 2001 Cut 2 F520..F1300 Abnahme'), karte_bild('LD_L', 'Leon nach CONTINUE F520..F720')]
out = Image.new('RGB', (1286, 1000), (20, 20, 20))
for i, im in enumerate(ims): out.paste(im, ((i % 2) * 646, 20 + (i // 2) * 490))
ImageDraw.Draw(out).text((4, 2), 'Wechselzahl-Karten (rot >= 6 Wechsel |d|>16, gelb 1..5) ueber dem halbdunklen Medianbild: Abnahme nur die Figur, Gegenprobe zusaetzlich (83,1)', fill=(255, 255, 255))
out.save(os.path.join(ZIEL, 'abn3_wechselkarten_cut2.png'))
print('ok', sorted(os.listdir(ZIEL)))
