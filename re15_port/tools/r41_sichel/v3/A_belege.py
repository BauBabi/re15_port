# Leser A V3: Belegbilder nach analysis/befunde_runde41/SF_belege/v3_lesung/A_*.png
# A_1_augen_x8.png      Augen-Ausschnitte (KI Kopf vorn/Seite, KI Schulter, V1 Kopf/Schulter) mit Masken
# A_2_schwanz_masken.png Schwanzmasken + Mittellinie + Schnitt + Dickenmarken (KI Bogen h83/h85/h92, KI roter Schwanz h69/h70, V1 h85)
# A_3_farbfelder.png    Farbfelder (Mittel der erodierten Masken) KI-Haut / KI-Bogen / KI-roter Schwanz / Original / V1, mit Lab
# A_4_original_masken.png Original n1433/n1435/n1442 (Schwarzwert 17 ab, x2) mit Schwanz- und Hautmaske, daneben KI
# A_5_maul.png          Maul-Ausschnitte KI (Zahnflecken gelb, Maulhuelle magenta) und V1
import sys, json
sys.path.insert(0, r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel/re15_port/tools/r41_sichel/v3')
from A_common import *
from A_augen import KOPF, SCHULTER, auge, maul
from A_original import voll, SCHWARZ
from scipy import ndimage as ndi
from PIL import ImageDraw
import cv2

FONT_BG = (0, 0, 0)


def titel(im, txt, zeile2=None):
    d = ImageDraw.Draw(im)
    d.rectangle([0, 0, im.width, 26 if zeile2 else 14], fill=FONT_BG)
    d.text((3, 1), txt, fill=(255, 255, 0))
    if zeile2:
        d.text((3, 13), zeile2, fill=(255, 255, 255))
    return im


def kontur(arr, mask, col, dicke=1):
    cs, _ = cv2.findContours(mask.astype(np.uint8), cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_NONE)
    cv2.drawContours(arr, cs, -1, col, dicke)


def reihe(tiles, gap=6, bg=(255, 255, 255)):
    W = sum(t.width for t in tiles) + gap * (len(tiles) - 1)
    Hh = max(t.height for t in tiles)
    s = Image.new('RGB', (W, Hh), bg)
    x = 0
    for t in tiles:
        s.paste(t, (x, 0))
        x += t.width + gap
    return s


def stapel(rows, gap=6, bg=(255, 255, 255)):
    W = max(r.width for r in rows)
    Hh = sum(r.height for r in rows) + gap * (len(rows) - 1)
    s = Image.new('RGB', (W, Hh), bg)
    y = 0
    for r in rows:
        s.paste(r, (0, y))
        y += r.height + gap
    return s


def augen_kachel(k, F=8):
    nn = int(k[1:])
    c = (KOPF if k[0] == 'k' else SCHULTER)[nn]
    rgb, al = load(nn)
    r, mk = auge(nn, c, k)
    cx, cy = c
    X0, Y0, X1, Y1 = cx - 15, cy - 10, cx + 16, cy + 11
    crop = rgb[Y0:Y1, X0:X1].copy()
    big = cv2.resize(crop, None, fx=F, fy=F, interpolation=cv2.INTER_NEAREST)
    x0 = r['bbox'][0] - int(np.nonzero(mk['eye'])[1].min())
    y0 = r['bbox'][1] - int(np.nonzero(mk['eye'])[0].min())
    for m_, col in ((mk['eye'], (0, 255, 0)), (mk['kern'], (0, 255, 255)), (mk['pup'], (255, 0, 0))):
        M = np.zeros(rgb.shape[:2], bool)
        ys, xs = np.nonzero(m_)
        M[ys + y0, xs + x0] = True
        Mb = cv2.resize(M[Y0:Y1, X0:X1].astype(np.uint8), None, fx=F, fy=F, interpolation=cv2.INTER_NEAREST)
        kontur(big, Mb, col, 2)
    t = Image.fromarray(big)
    return titel(t, '%s birkin%02d h%d Ausschnitt x%d-%d y%d-%d x%d' % ('Kopfauge' if k[0] == 'k' else 'Schulterauge', nn, MAP[nn][1], X0, X1 - 1, Y0, Y1 - 1, F),
                 '%dx%d px, weiss(L*>80) %d px, L Auge/Haut %.2f' % (r['breite'], r['hoehe'], r['weiss_L80'], r['L_auge_durch_haut']))


def v1_auge(c, name):
    im = cv2.cvtColor(cv2.imread(SCR + 'A_v1_kopfnah.png'), cv2.COLOR_BGR2RGB)
    crop = im[c[1] - 60:c[1] + 61, max(c[0] - 90, 0):c[0] + 91]
    big = cv2.resize(crop, None, fx=1.37, fy=1.37, interpolation=cv2.INTER_NEAREST)
    return titel(Image.fromarray(big), 'V1 %s (Kopf vorn nah, neutrales Licht, 960er Render)' % name, 'Ausschnitt um (%d,%d)' % c)


def bild1():
    r1 = reihe([augen_kachel('k19'), augen_kachel('k20'), augen_kachel('k22'), augen_kachel('k12')])
    r2 = reihe([augen_kachel('s19'), augen_kachel('s20'), v1_auge((432, 438), 'Kopfauge'), v1_auge((88, 485), 'Schulterauge')])
    s = stapel([r1, r2])
    s = titel_kopf(s, 'A1-A6 Augen: KI-Freisteller (gruen = Augenmaske, cyan = heller Kern L*>Haut, rot = Pupille L*<=Lmin+25%) | Port V1')
    s.save(BEL + 'A_1_augen_x8.png')


def titel_kopf(s, txt):
    t = Image.new('RGB', (s.width, s.height + 18), (0, 0, 0))
    t.paste(s, (0, 18))
    ImageDraw.Draw(t).text((4, 3), txt, fill=(0, 255, 255))
    return t


def schwanz_kachel(nn, f=1.0):
    rgb, al = load(nn)
    m = np.load(SCR + 'A_schwanz_m%02d.npy' % nn)
    P = np.load(SCR + 'A_schwanz_P%02d.npy' % nn)
    J = json.load(open(SCR + 'A_schwanz_ki.json'))['ki%02d' % nn]
    a = rgb.copy()
    a[~al] = (60, 110, 60)
    kontur(a, m, (255, 0, 255), 1)
    cv2.polylines(a, [P.astype(np.int32).reshape(-1, 1, 2)], False, (255, 255, 0), 1)
    if 'kerbe' in J['info']:
        cv2.line(a, tuple(J['info']['kerbe']), tuple(J['info']['naht']), (0, 255, 255), 1)
    L = len(P)
    tang = np.gradient(P, axis=0)
    for fr in (0.1, 0.2, 0.3, 0.5, 0.7, 0.9):
        i = int(fr * (L - 1))
        d = tang[i] / np.linalg.norm(tang[i])
        n = np.array([-d[1], d[0]])
        w = J['form']['dicke_px'][str(fr)] / 2
        p0 = (P[i] - n * w).astype(int)
        p1 = (P[i] + n * w).astype(int)
        cv2.line(a, tuple(p0), tuple(p1), (0, 255, 0), 1)
    for pos in J['form']['knick_pos']:
        i = int(pos * (L - 1))
        cv2.circle(a, tuple(P[i].astype(int)), 5, (255, 80, 0), 1)
    big = cv2.resize(a, None, fx=f, fy=f, interpolation=cv2.INTER_NEAREST)
    fo = J['form']
    return titel(Image.fromarray(big), 'KI birkin%02d h%d H=%d px: Laenge %.2f H, Knicke %d (orange)' % (nn, J['h'], J['H'], fo['laenge_H'], fo['knicke_gt20']),
                 'Dicke 0,1..0,9: ' + '/'.join('%.3f' % fo['dicke_H'][k] for k in ('0.1', '0.2', '0.3', '0.5', '0.7', '0.9')) + ' H')


def v1_schwanz_kachel(h):
    rgb = cv2.cvtColor(cv2.imread(SCR + 'A_v1_seite_h%d.png' % h), cv2.COLOR_BGR2RGB)
    ids = np.load(SCR + 'A_v1_seite_h%d_ids.npy' % h)
    P = np.load(SCR + 'A_v1_schwanz_P%d.npy' % h)
    J = json.load(open(SCR + 'A_port_v1.json'))['v1_seite_h%d' % h]
    m = (ids >= 16) & (ids <= 22)
    ys, xs = np.nonzero(ids >= 0)
    X0, Y0, X1, Y1 = max(xs.min() - 10, 0), max(ys.min() - 10, 0), min(xs.max() + 10, 640), min(ys.max() + 10, 480)
    a = rgb.copy()
    kontur(a, m, (255, 0, 255), 1)
    cv2.polylines(a, [P.astype(np.int32).reshape(-1, 1, 2)], False, (255, 255, 0), 1)
    L = len(P)
    for pos in J['form']['knick_pos']:
        cv2.circle(a, tuple(P[int(pos * (L - 1))].astype(int)), 3, (255, 80, 0), 1)
    crop = a[Y0:Y1, X0:X1]
    big = cv2.resize(crop, None, fx=2, fy=2, interpolation=cv2.INTER_NEAREST)
    fo = J['form']
    return titel(Image.fromarray(big), 'Port V1 h%d (Spielkamera, Knochen 16-22) H=%d px (640er): Laenge %.2f H, Knicke %d' % (h, J['H'], fo['laenge_H'], fo['knicke_gt20']),
                 'Dicke: ' + '/'.join('%.3f' % fo['dicke_H'][k] for k in ('0.1', '0.2', '0.3', '0.5', '0.7', '0.9')) + ' H (sichtbare Pixel)')


def bild2():
    r1 = reihe([schwanz_kachel(11), schwanz_kachel(12), schwanz_kachel(13)])
    r2 = reihe([schwanz_kachel(6), schwanz_kachel(9), v1_schwanz_kachel(85)])
    s = titel_kopf(stapel([r1, r2]), 'S3-S6 Schwanz: magenta = Maske, gelb = Mittellinie, cyan = Wurzelschnitt Kerbe->Naht, gruen = Dicke bei 0,1/0,2/0,3/0,5/0,7/0,9, orange = Knick (>20 Grad auf 0,05 H)')
    s.save(BEL + 'A_2_schwanz_masken.png')


def feld(rgb, lab_, txt1, txt2, w=170, h=110):
    t = Image.new('RGB', (w, h + 30), tuple(int(round(v)) for v in rgb))
    d = ImageDraw.Draw(t)
    d.rectangle([0, h, w, h + 30], fill=(255, 255, 255))
    d.text((3, h + 2), txt1, fill=(0, 0, 0))
    d.text((3, h + 15), 'L %.1f a %.1f b %.1f' % tuple(lab_) + ' ' + txt2, fill=(0, 0, 0))
    return t


def bild3():
    K = json.load(open(SCR + 'A_schwanz_ki.json'))
    O = json.load(open(SCR + 'A_original.json'))
    V = json.load(open(SCR + 'A_port_v1.json'))
    A = json.load(open(SCR + 'A_augen.json'))
    f = []
    k = K['ki12']['farbe']
    f.append(feld(k['haut']['rgb'], k['haut']['lab'], 'KI-Haut birkin12 h85', ''))
    f.append(feld(k['schwanz']['rgb'], k['schwanz']['lab'], 'KI-Bogen birkin12 h85', 'dE00 %.1f' % K['ki12']['dE00_schwanz_haut']))
    k9 = K['ki09']['farbe']
    f.append(feld(k9['schwanz']['rgb'], k9['schwanz']['lab'], 'KI-roter Schwanz birkin09 h70', 'dE00 %.1f' % K['ki09']['dE00_schwanz_haut']))
    o = O['1435']
    f.append(feld(o['haut']['rgb'], o['haut']['lab'], 'Original-Haut n1435 (-17)', ''))
    f.append(feld(o['schwanz']['rgb'], o['schwanz']['lab'], 'Original-Bogen n1435 (-17)', 'dE00 %.1f' % o['dE00']))
    v = V['v1_seite_h85']['farbe']
    f.append(feld(v['haut']['rgb'], v['haut']['lab'], 'V1-Haut h85 (Spielkamera)', ''))
    f.append(feld(v['schwanz']['rgb'], v['schwanz']['lab'], 'V1-Schwanz h85', 'dE00 %.1f' % V['v1_seite_h85']['dE00_schwanz_haut']))
    r1 = reihe(f)
    g = []
    a = A['k19']
    g.append(feld(a['haut']['rgb'], a['haut']['lab'], 'KI Haut um Kopfauge h118', ''))
    g.append(feld(a['lidrand']['rgb'], a['lidrand']['lab'], 'KI Lidrand (1 px Ring)', ''))
    g.append(feld(a['zwischen']['rgb'], a['zwischen']['lab'], 'KI Auge Mittelton', ''))
    g.append(feld(a['pupille']['rgb'], a['pupille']['lab'], 'KI Pupille/dunkler Kern', ''))
    g.append(feld(a['kern']['rgb'], a['kern']['lab'], 'KI heller Kern (3x3 px)', ''))
    va = V['v1_auge_kopf']
    g.append(feld(va['hell_ring']['rgb'], va['hell_ring']['lab'], 'V1 Augenring (hell)', ''))
    g.append(feld(va['dunkel_innen']['rgb'], va['dunkel_innen']['lab'], 'V1 Augenmitte (dunkel)', ''))
    r2 = reihe(g)
    s = titel_kopf(stapel([r1, r2]), 'S1 / A4 Farbfelder: Mittel der 2 px erodierten Masken (RGB-Mittel als Flaeche), Lab D65')
    s.save(BEL + 'A_3_farbfelder.png')


def bild4():
    tiles = []
    for n, nn in ((1433, 11), (1435, 12), (1442, 13)):
        sm = np.load(SCR + 'A_schwanz_m%02d.npy' % nn)
        hm = np.load(SCR + 'A_haut_m%02d.npy' % nn)
        o = np.array(Image.open(SCR + 'A_orig/n%d.png' % n).convert('RGB')).astype(np.float32)
        o = np.clip(o - SCHWARZ, 0, 255)
        o = np.clip(o * 2.0, 0, 255).astype(np.uint8)          # nur zur Ansicht aufgehellt (x2)
        S = erode(voll(sm, nn, o.shape[:2], 0.5), 3)
        Hm = erode(voll(hm, nn, o.shape[:2], 0.5), 3)
        a = o.copy()
        kontur(a, S, (255, 0, 255), 1)
        kontur(a, Hm, (0, 255, 255), 1)
        n_, h_, ox, oy = MAP[nn]
        X0, Y0 = ox // 2 - 10, oy // 2 - 10
        crop = a[Y0:Y0 + 280, X0:X0 + 220]
        tiles.append(titel(Image.fromarray(cv2.resize(crop, None, fx=2, fy=2, interpolation=cv2.INTER_NEAREST)),
                           'Original n%d (h%d) -17, Ansicht x2 hell' % (n, n - 1350), 'magenta Schwanz, cyan Haut (KI-Maske birkin%02d, 3 px erodiert)' % nn))
        k = np.array(Image.open(SCR + 'A_slp/n%d.png' % n).convert('RGB'))
        Sk = erode(voll(sm, nn, k.shape[:2], 1.0), 6)
        Hk = erode(voll(hm, nn, k.shape[:2], 1.0), 6)
        b = k.copy()
        kontur(b, Sk, (255, 0, 255), 2)
        kontur(b, Hk, (0, 255, 255), 2)
        cropk = b[oy - 20:oy + 540, ox - 20:ox + 420]
        tiles.append(titel(Image.fromarray(cropk), 'KI _slp n%d (h%d), dieselbe Maske' % (n, n - 1350), ''))
    s = titel_kopf(reihe(tiles), 'Original-Video 4 gegen KI: Schwanz (Bogen) und Koerperhaut, Lab-Werte im Bericht')
    s.save(BEL + 'A_4_original_masken.png')


def maul_kachel(nn, F=6):
    c = KOPF[nn]
    rgb, al = load(nn)
    rm, mm = maul(nn, c)
    x0, y0, x1, y1 = mm['win']
    X0, Y0, X1, Y1 = max(c[0] - 34, 0), max(c[1] - 12, 0), min(c[0] + 40, rgb.shape[1]), c[1] + 58
    big = cv2.resize(rgb[Y0:Y1, X0:X1], None, fx=F, fy=F, interpolation=cv2.INTER_NEAREST)
    for m_, col in ((mm['hull'], (255, 0, 255)), (mm['teeth'], (255, 255, 0))):
        M = np.zeros(rgb.shape[:2], np.uint8)
        M[y0:y1, x0:x1] = m_
        kontur(big, cv2.resize(M[Y0:Y1, X0:X1], None, fx=F, fy=F, interpolation=cv2.INTER_NEAREST), col, 2)
    return titel(Image.fromarray(big), 'KI birkin%02d h%d Maul x%d-%d y%d-%d (x%d)' % (nn, MAP[nn][1], X0, X1 - 1, Y0, Y1 - 1, F),
                 'Huelle %dx%d px, Zahnflecken oben/unten %d/%d' % (rm['breite'], rm['hoehe'], rm['zaehne_oben'], rm['zaehne_unten']))


def bild5():
    v = cv2.cvtColor(cv2.imread(SCR + 'A_v1_kopfnah.png'), cv2.COLOR_BGR2RGB)[340:820, 230:700]
    vt = titel(Image.fromarray(v), 'Port V1 Kopf vorn nah (960er Render, Ausschnitt x230-699 y340-819)', 'senkrechtes Oval, Zahnreihen als helle Boegen oben/unten')
    s = titel_kopf(stapel([reihe([maul_kachel(18), maul_kachel(19), maul_kachel(20)]), reihe([maul_kachel(22), vt])]),
                   'A7 Maul: magenta = Huelle der Zahnflecken, gelb = Zahnflecken (L*>50, Chroma<25, 1-8 px)')
    s.save(BEL + 'A_5_maul.png')


if __name__ == '__main__':
    bild1(); bild2(); bild3(); bild4(); bild5()
    for f in ('A_1_augen_x8', 'A_2_schwanz_masken', 'A_3_farbfelder', 'A_4_original_masken', 'A_5_maul'):
        print(BEL + f + '.png', Image.open(BEL + f + '.png').size)
