"""SF V4 (Bauer): Bildleisten fuer die Abnahme (analysis/befunde_runde41/SF_belege/v4/), Messblatt V4 Riegel A1.

    python leisten_v4.py [alle|01|02|03|04|05|06|07|08]

Spalten NUR: Nutzer-Vorlage (Originalbild) | Nutzer-Beschriftung | V3 r2 (vorher) | V4 neu.
  Vorlage 1 = Beschreibung1_Vorlage.png (n1485) entblendet (Messblatt V4 Abschnitt 1, k 0,766/0,764/0,768), Vorlage 2 =
  Beschreibung2_Vorlage.png (n1435); Beschriftung = linke 640 Spalten von Beschreibung1.png / Beschreibung2.png (wie gezeichnet).
  V3 r2 / V4 = b2-Render in der Spielkamera 50A0 Schnitt 12 (RDT-Licht, Hintergrund BSS 50A0/12) in posen_v4 "135" / "85"
  (Lichtgegenprobe gegen RE15_FRAMEDUMP: dE00 0,1, riegel_v4). 07 = Spiellaeufe (RE15_FRAMEDUMP, auf das Videobild registriert).
Nutzerpunkte (Linienenden, Messblatt V4 Abschnitt 1a) gelb in den Original-Spalten; im Port die gemessenen Gegenstuecke (riegel_v4).
"""
import os, sys, json
import numpy as np, cv2
HIER = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HIER)
sys.path.insert(0, os.path.join(os.path.dirname(HIER), 'nb2neu'))
import v4_gemein as G, b2, riegel_v4 as RV
from PIL import Image

AUS = os.path.join(G.BAUM, 'analysis', 'befunde_runde41', 'SF_belege', 'v4')
LAUF = {'V3r2': os.path.join(os.path.dirname(G.SFV4), 'sfv3', 'r', 'V3r2'), 'V4': os.path.join(G.SFV4, 'r', 'V4r2')}   # Runde 2: Lauf V4r2
PKT1 = {'Auge': (382, 164), 'Haare': (404, 160), 'Nase': (395, 177)}
PKT2 = {'Auge': (460, 234), 'Haare': (468, 241), 'Beginn': (357, 243), 'Knick': (290, 308), 'Ende': (295, 343)}
BIEG2 = (295, 252)
GELB, CYAN, MAG, GRUEN = (255, 255, 0), (0, 255, 255), (255, 0, 255), (0, 255, 0)
_cache = {}


def mo(k):
    if k not in _cache:
        _cache[k] = b2.modell(G.EMD_V3 if k == 'V3r2' else G.EMD_V4)
    return _cache[k]


def beschriftung(b):
    a = np.asarray(Image.open(G.V80 + 'Beschreibung%d.png' % b).convert('RGB'))
    return np.ascontiguousarray(a[:, :640])


def vorlage_u8(b):
    return np.clip(G.vorlage(b), 0, 255).astype(np.uint8)


def kachel(img, box, f, titel, zeilen):
    return b2.tafel(np.ascontiguousarray(img), titel, zeilen, f=f, box=box)


def punkt(im, p, col, r=3, txt=None):
    cv2.circle(im, (int(round(p[0])), int(round(p[1]))), r, col, 1, cv2.LINE_AA)
    if txt:
        b2.schrift(im, txt, p[0] + 4, p[1] - 3, col, 0.35)


def render(k, h):
    key = ('r', k, h)
    if key not in _cache:
        _cache[key] = G.zeichne(mo(k), G.posen()[h], voll=True)
    return _cache[key]


def riegel(k):
    key = ('rg', k)
    if key not in _cache:
        P = G.posen(); m = mo(k)
        zk, ik, _ = RV.kopf_vorn(m, P); zs, is_, _ = RV.kopf_seite(m, P); zt, st, rest = RV.schwanz(m, P)
        zf, ef, _ = RV.koerper_farben(m, P)
        zt = zt + []
        _cache[key] = dict(zk={z['mass'].split()[0]: z for z in zk}, zs={z['mass'].split()[0]: z for z in zs},
                           zt={z['mass'].split()[0]: z for z in zt}, zf=zf, ef=ef, ik=ik, is_=is_, st=st, fr=rest[3])
    return _cache[key]


def kurz(z):
    return ('OK ' if z['ok'] else 'ROT ') + z['ist']


def speichern(name, zeilen, kopf):
    p = os.path.join(AUS, name + '.jpg')
    b2.leiste(zeilen, kopf, p, q=90)
    print('geschrieben', p)


KOPFZEILE = 'Spalten: Nutzer-Vorlage (Originalbild) | Nutzer-Beschriftung | V3 r2 (vorher) | V4 neu'


# ═══════════════════════════════════════════════════ 01 / 02 ganze Figur
def l01():
    box = (250, 110, 560, 470)
    o = vorlage_u8(1); bsch = beschriftung(1)
    for n, p in PKT1.items():
        punkt(o, p, GELB, 3, n)
    t = [kachel(o, box, 2, 'Vorlage 1 n1485 entblendet', ['H1 302 px (Messblatt V4)']),
         kachel(bsch, box, 2, 'Beschreibung1 (Nutzer)', ['Auge / Haare / Nase'])]
    for k in ('V3r2', 'V4'):
        img, ids, iv = render(k, '135'); r = riegel(k)
        img = img.copy()
        bl = r['ik']['blob']; punkt(img, bl['sp'], GELB, 3, 'Auge')
        t.append(kachel(img, box, 2, '%s Pose 135 (Kamera 50A0/12)' % k,
                        ['K4 ' + kurz(r['zk']['K4']), 'K2 ' + kurz(r['zk']['K2'])[:60], 'K5 ' + kurz(r['zk']['K5'])[:60]]))
    speichern('01_vorn_kamera12', [t], ['SF V4 - 01 vorn: Spielkamera 50A0 Cut 12 wie Vorlagebild 1 (h135), Figur in Pose posen_v4 "135"',
                                         KOPFZEILE])


def l02():
    box = (230, 160, 520, 450)
    o = vorlage_u8(2); bsch = beschriftung(2)
    for n, p in PKT2.items():
        punkt(o, p, GELB, 3, n)
    t = [kachel(o, box, 2, 'Vorlage 2 n1435', ['H2 253 px (Messblatt V4)']), kachel(bsch, box, 2, 'Beschreibung2 (Nutzer)', [''])]
    for k in ('V3r2', 'V4'):
        img, ids, iv = render(k, '85'); r = riegel(k)
        img = img.copy(); s = r['st']
        for n, q in (('Beginn', s['beginn']), ('Knick', s['knick']), ('Ende', s['ende'])):
            punkt(img, q, GELB, 3, n)
        punkt(img, r['is_']['blob']['sp'], GELB, 3, 'Auge')
        t.append(kachel(img, box, 2, '%s Pose 85 (Kamera 50A0/12)' % k,
                        ['T2 ' + kurz(r['zt']['T2']), 'T3 ' + kurz(r['zt']['T3']), 'S2 ' + kurz(r['zs']['S2'])]))
    speichern('02_seite_wie_bild2', [t], ['SF V4 - 02 seitlich: Kamera/Pose wie Vorlagebild 2 (h85), posen_v4 "85"', KOPFZEILE])


# ═══════════════════════════════════════════════════ 03 / 04 Kopf nah
def kopfbox_rand(kb, m=8):
    return (kb[0] - m, kb[1] - m, kb[2] + m, kb[3] + m)


def l03():
    # Runde 2: Zeile 1 ohne Marken (Sicht-Pruefung: Beschriftung deckte den Kopf zu), Zeile 2 mit kleinen Marken ohne Text
    # (gelb = Auge / Nutzerpunkt Auge, cyan = Haare und Nase)
    o0 = vorlage_u8(1); bsch = beschriftung(1)
    ob = kopfbox_rand(G.KB_ORIG[1])
    o = o0.copy()
    for n, p in PKT1.items():
        punkt(o, p, GELB if n == 'Auge' else CYAN, 1)
    z1 = [kachel(o0, ob, 4, 'Vorlage 1 Kopf x4', ['Auge hell 46,9/7,2/-1,1', 'Haarfeld 13,3/7,4/1,5']),
          kachel(bsch, ob, 4, 'Beschreibung1 Kopf x4', ['Nase 18,2/7,1/1,3', 'Kuppe rot 22,6/15,4/5,2'])]
    z2 = [kachel(o, ob, 4, 'Vorlage 1 mit Nutzerpunkten', ['gelb Auge, cyan Haare/Nase']), kachel(bsch, ob, 4, 'Beschreibung1', [''])]
    for k in ('V3r2', 'V4'):
        img, ids, iv = render(k, '135'); r = riegel(k); img0 = img.copy(); img = img.copy()
        kb = r['ik']['kopfbox']
        punkt(img, r['ik']['blob']['sp'], GELB, 1)
        for n, kk in (('Haare', 'KV2'), ('Nase', 'KV3')):
            bx = r['ik']['fenster'][kk][0]
            punkt(img, (0.5 * (bx[0] + bx[2]), 0.5 * (bx[1] + bx[3])), CYAN, 1)
        zl = ['K1 ' + kurz(r['zk']['K1']), 'K2 ' + kurz(r['zk']['K2'])[:58], 'K3 ' + kurz(r['zk']['K3'])[:58],
              'K4 ' + kurz(r['zk']['K4'])[:58], 'K5 ' + kurz(r['zk']['K5'])[:58], 'K7 ' + kurz(r['zk']['K7']), 'K9 ' + kurz(r['zk']['K9'])]
        z1.append(kachel(img0, kopfbox_rand(kb), 4, '%s Kopf x4' % k, zl))
        z2.append(kachel(img, kopfbox_rand(kb), 4, '%s mit Messpunkten' % k, ['gelb Augenfleck (Schwerpunkt), cyan KV2/KV3']))
    speichern('03_kopf_nah_vorn_x4', [z1, z2], ['SF V4 - 03 Kopf vorn x4 (Kopfbox +8 px; Port = Bbox Id 9); Zeile 1 ohne, Zeile 2 mit Marken',
                                               KOPFZEILE])


def l04():
    o0 = vorlage_u8(2); bsch = beschriftung(2)
    ob = kopfbox_rand(G.KB_ORIG[2])
    o = o0.copy()
    for n in ('Auge', 'Haare'):
        punkt(o, PKT2[n], GELB if n == 'Auge' else CYAN, 1)
    z1 = [kachel(o0, ob, 4, 'Vorlage 2 Kopf x4', ['Auge hell 48,4/4,0/0,5', 'Haarkante 19,1/1,2/1,1']),
          kachel(bsch, ob, 4, 'Beschreibung2 Kopf x4', ['Kuppe rot 27,6/15,3/3,3'])]
    z2 = [kachel(o, ob, 4, 'Vorlage 2 mit Nutzerpunkten', ['gelb Auge, cyan Haare']), kachel(bsch, ob, 4, 'Beschreibung2', [''])]
    for k in ('V3r2', 'V4'):
        img, ids, iv = render(k, '85'); r = riegel(k); img0 = img.copy(); img = img.copy()
        punkt(img, r['is_']['blob']['sp'], GELB, 1)
        kb = r['is_']['kopfbox']
        bx = G.fenster(kb, RV.FENSTER['KS2']); punkt(img, (0.5 * (bx[0] + bx[2]), 0.5 * (bx[1] + bx[3])), CYAN, 1)
        z1.append(kachel(img0, kopfbox_rand(kb, 14), 4, '%s Kopf seitlich x4' % k,
                         ['S1 ' + kurz(r['zs']['S1'])[:62], 'S2 ' + kurz(r['zs']['S2']), 'S3 ' + kurz(r['zs']['S3'])[:62],
                          'S4 ' + kurz(r['zs']['S4'])]))
        z2.append(kachel(img, kopfbox_rand(kb, 14), 4, '%s mit Messpunkten' % k, ['gelb Augenfleck, cyan KS2']))
    speichern('04_kopf_nah_seite_x4', [z1, z2], ['SF V4 - 04 Kopf seitlich x4; Zeile 1 ohne, Zeile 2 mit Marken', KOPFZEILE])


# ═══════════════════════════════════════════════════ 05 Schwanz
def l05():
    box = (240, 185, 420, 365)
    o = vorlage_u8(2); bsch = beschriftung(2)
    zug = [PKT2['Beginn'], BIEG2, PKT2['Knick'], PKT2['Ende']]
    for im in (o, bsch):
        cv2.polylines(im, [np.int32(zug)], False, GELB, 1, cv2.LINE_AA)
    t = [kachel(o, box, 3, 'Vorlage 2 Schwanz x3', ['Beginn-Biegung-Knick-Ende (gelb)', 'Breite f0,1/0,3/0,5/0,7 =', '0,03/0,065/0,125/0,14 H']),
         kachel(bsch, box, 3, 'Beschreibung2 Schwanz x3', ['Endstueck 0,035 H, 0,145 H lang'])]
    for k in ('V3r2', 'V4'):
        img, ids, iv = render(k, '85'); r = riegel(k); img = img.copy(); s = r['st']
        H = s['H']; b = np.array(s['beginn'])
        rel = [((p[0] - 357) / G.H_ORIG[2], (p[1] - 243) / G.H_ORIG[2]) for p in zug]
        zp = [tuple(b + np.array(q) * H) for q in rel]
        cv2.polylines(img, [np.int32(zp)], False, GELB, 1, cv2.LINE_AA)
        cv2.polylines(img, [np.int32(s['C'][::3])], False, CYAN, 1, cv2.LINE_AA)
        f = s['T5']
        t.append(kachel(img, box, 3, '%s Schwanz x3 (gelb Vorlage am Port-Beginn, H-skaliert; cyan Mittellinie)' % k,
                        ['T2 ' + kurz(r['zt']['T2']), 'T3 ' + kurz(r['zt']['T3']), 'T4 ' + kurz(r['zt']['T4'])[:60],
                         'T5 %.3f/%.3f/%.3f/%.3f H' % (f[0.1], f[0.3], f[0.5], f[0.7]), 'T6 ' + kurz(r['zt']['T6'])[:64]]))
    speichern('05_schwanz_x3', [t], ['SF V4 - 05 Schwanz x3: Polygonzug der Vorlage (Beginn, Biegung, Knick, Ende) ueber dem Port',
                                     KOPFZEILE])


# ═══════════════════════════════════════════════════ 06 Farben
def feld(rgb, text, w=300, h=70):
    im = np.full((h, w, 3), 16, np.uint8)
    if rgb is not None and np.all(np.isfinite(rgb)):
        im[6:h - 24, 6:w - 6] = np.clip(np.asarray(rgb, float), 0, 255).astype(np.uint8)
    b2.schrift(im, text, 6, h - 7, (255, 255, 255), 0.4)
    return im


def l06():
    import birkin_formen as BF
    reihen = []
    zeilen_def = [('KV1 Kuppe rot vorn', 'kuppe_v', 1, 'K9'), ('Auge vorn', 'auge_v', 1, 'K2'), ('KV2 Haarfeld', 'haar_v', 1, 'K5'),
                  ('KV3 Nase', 'nase', 1, 'K7'), ('KV4 Kinn', 'kinn', 1, 'K8'), ('KS1 Kuppe seitl.', 'kuppe_s', 2, 'S4'),
                  ('Auge seitl.', 'auge_s', 2, 'S1'), ('KS2 Haarkante', 'haar_s', 2, 'S3')]
    fen = {'K9': 'KV1', 'K5': 'KV2', 'K7': 'KV3', 'K8': 'KV4', 'S4': 'KS1', 'S3': 'KS2'}
    for name, zk, b, rg in zeilen_def:
        ziel = RV.ZIEL[zk]
        z = [feld(BF._lab_rgb(*ziel), '%s Ziel %s' % (name, '/'.join('%.1f' % v for v in ziel)))]
        bs = beschriftung(b).copy()
        ob = G.KB_ORIG[b]
        if rg in fen:
            bx = G.fenster(ob, RV.FENSTER[fen[rg]]); cv2.rectangle(bs, bx[:2], bx[2:], GELB, 1)
        crop = cv2.resize(bs[ob[1] - 10:ob[3] + 10, ob[0] - 10:ob[2] + 10], (120, 120 * (ob[3] - ob[1] + 20) // (ob[2] - ob[0] + 20)),
                          interpolation=cv2.INTER_NEAREST)
        f = np.full((70, 300, 3), 16, np.uint8); hh = min(70, crop.shape[0]); f[:hh, :120] = crop[:hh]
        b2.schrift(f, 'Fenster gelb', 130, 40, (255, 255, 255), 0.4)
        z.append(f)
        for k in ('V3r2', 'V4'):
            r = riegel(k)
            zz = r['zk'].get(rg) or r['zs'].get(rg)
            lab_ist = None
            ist = zz['ist']
            try:
                lab_ist = [float(x) for x in ist.split()[0].split('/')]
            except Exception:  # noqa: BLE001
                pass
            z.append(feld(BF._lab_rgb(*lab_ist) if lab_ist else None, '%s %s' % ('OK' if zz['ok'] else 'ROT', ist[:44])))
        reihen.append(z)
    for k_ in ('F6', 'F7', 'F8', 'F9', 'F10', 'F14', 'F19'):
        ziel = RV.ZIEL[k_]
        z = [feld(BF._lab_rgb(*ziel), '%s Ziel %s' % (k_, '/'.join('%.1f' % v for v in ziel))), feld(None, 'Koerper-Id (Messblatt 7)')]
        for k in ('V3r2', 'V4'):
            r = riegel(k)
            la, n = r['ef'][k_]
            d = G.de00(la, ziel) if n else 99
            z.append(feld(BF._lab_rgb(*la) if n else None, '%s %s dE00 %.1f' % ('OK' if d <= 5 and abs(la[0] - ziel[0]) <= 4 else 'ROT',
                                                                         '/'.join('%.1f' % v for v in la), d)))
        reihen.append(z)
    # Runde 2: Sicht-Boxen (FS) und Schwanzachse (T9) aus den Riegelzeilen
    for k in ('V3r2', 'V4'):
        riegel(k)
    for mass_ in [z['mass'] for z in riegel('V4')['zf'] if z['mass'].startswith('FS')]:
        zv = {k: next(z for z in riegel(k)['zf'] if z['mass'] == mass_) for k in ('V3r2', 'V4')}
        ziel = [float(x) for x in zv['V4']['soll'].split(',')[0].split('/')]
        z = [feld(BF._lab_rgb(*ziel), '%s Ziel %s' % (mass_[3:], zv['V4']['soll'][:22])), feld(None, 'Sicht-Box (Pruefung V4 r1)')]
        for k in ('V3r2', 'V4'):
            ist = zv[k]['ist']
            la = [float(x) for x in ist.split()[0].split('/')]
            z.append(feld(BF._lab_rgb(*la), '%s %s' % ('OK' if zv[k]['ok'] else 'ROT', ist[:46])))
        reihen.append(z)
    for k_ in ('F15', 'F16', 'F17', 'F18'):
        ziel = RV.ZIEL[k_]
        z = [feld(BF._lab_rgb(*ziel), '%s Ziel %s' % (k_, '/'.join('%.1f' % v for v in ziel))), feld(None, 'Schwanz (T8)')]
        for k in ('V3r2', 'V4'):
            la, n = riegel(k)['fr'][k_]
            d = G.de00(la, ziel) if n >= 3 else 99
            z.append(feld(BF._lab_rgb(*la) if n >= 3 else None, '%s %s dE00 %.1f' % ('OK' if d <= 6 else 'ROT',
                                                                               '/'.join('%.1f' % v for v in la), d)))
        reihen.append(z)
    speichern('06_farben', reihen, ['SF V4 - 06 Farben: Ziel (Videoraum, Messblatt V4 Abschnitt 5) | Fenster | V3 r2 | V4 '
                                    '(Vergleichsregel sigma 1,5, V = a + b x P, Lab D65, dE00)', KOPFZEILE])


# ═══════════════════════════════════════════════════ 07 Gang (Spiellaeufe)
def l07():
    import c6
    import riegel_v3 as R3
    box = (230, 100, 560, 480)
    zeilen = []
    gg = RV.gang(G.EMD_V4, os.path.join(LAUF['V4'], 'debug.log'), (100, 110, 118, 125, 135))
    for h in (100, 118, 135):
        v = cv2.cvtColor(c6.vid(h), cv2.COLOR_BGR2RGB)
        if h == 135:
            v = vorlage_u8(1)
        bs = beschriftung(1) if h == 135 else np.full((480, 640, 3), 30, np.uint8)
        if h != 135:
            b2.schrift(bs, 'keine Beschriftung fuer h%d' % h, 240, 300, (200, 200, 200), 0.6)
        t = [kachel(v, box, 1, 'Video 4 h%d (n%d)' % (h, 1350 + h), ['']), kachel(bs, box, 1, 'Beschriftung', [''])]
        for k in ('V3r2', 'V4'):
            p = c6.port(LAUF[k], h)
            im = cv2.cvtColor(p, cv2.COLOR_BGR2RGB) if p is not None else np.zeros((480, 640, 3), np.uint8)
            zl = ['RE15_FRAMEDUMP Lauf %s' % k]
            if k == 'V4' and h in gg and 'abw' in gg[h]:
                zl.append('T7 (Feder an Pose 85): ' + ' '.join('%s %.3f' % (q, gg[h]['abw'][q]) for q in ('T2', 'T3', 'T4')))
            t.append(kachel(im, box, 1, '%s Spiel h%d' % (k, h), zl))
        zeilen.append(t)
    # Schwanz in Bewegung: Feder-Stand des V4-Laufs an Pose 85
    tz = []
    for h in (100, 110, 118, 125, 135):
        if h in gg and 'bild' in gg[h]:
            im = gg[h]['bild'][0]
            tz.append(kachel(im, (230, 160, 520, 450), 1, 'V4 Feder h%d an Pose 85' % h,
                             ['T2/T3/T4 Abw. %.3f/%.3f/%.3f H' % (gg[h]['abw']['T2'], gg[h]['abw']['T3'], gg[h]['abw']['T4'])]))
    zeilen.append(tz)
    speichern('07_gang', zeilen, ['SF V4 - 07 Gang: Spiellauf (RE15_FRAMEDUMP, registriert) und Feder-Stand des V4-Laufs an Pose 85 (T7)',
                                  KOPFZEILE + '; letzte Zeile: nur V4'])


# ═══════════════════════════════════════════════════ 08 Rundum
def l08():
    m = mo('V4'); p0 = dict(G.posen()['85'])
    t = []
    for i, dg in enumerate(range(0, 360, 60)):
        p = dict(p0, gier=(p0['gier'] + int(round(dg * 4096 / 360))) & 0xFFF)
        img, ids = b2.zeichne(m, p, None, licht=False)
        ys, xs = np.nonzero(ids >= 0)
        cx, cy = int(xs.mean()), int(ys.mean())
        t.append(kachel(img, (max(cx - 150, 0), max(cy - 150, 0), min(cx + 150, 640), min(cy + 150, 480)), 1,
                        'V4 Gier +%d Grad (neutral)' % dg, ['']))
    speichern('08_rundum_V4', [t[:3], t[3:]], ['SF V4 - 08 rundum: 6 Blickwinkel, neutrales Licht (Texelfarbe), Pose 85 gedreht',
                                               'nur V4'])


def main():
    os.makedirs(AUS, exist_ok=True)
    w = sys.argv[1] if len(sys.argv) > 1 else 'alle'
    for k, f in (('01', l01), ('02', l02), ('03', l03), ('04', l04), ('05', l05), ('06', l06), ('07', l07), ('08', l08)):
        if w in ('alle', k):
            f()


if __name__ == '__main__':
    main()
