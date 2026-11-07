"""SF V4 (Bauer): Bildleisten fuer die Abnahme (analysis/befunde_runde41/SF_belege/v4/), Messblatt V4 Riegel A1.

    python leisten_v4.py [alle|01|02|03|04|05|06|07|08]

Spalten NUR: Nutzer-Vorlage (Originalbild) | Nutzer-Beschriftung | vorher | neu.
  Runde 3: vorher = V4 Runde 2 (sfv4/emd/EM30_V4r2.EMD sha256 174b7879, Lauf sfv4/r/V4r2), neu = V4 Runde 3 (sfv4/emd/EM30_V4.EMD
  = EM30_V4r3.EMD sha256 dfbc30bc,
  Lauf sfv4/r/V4r3); Umgebungsvariablen RE15_LEISTE_VORHER / RE15_LEISTE_NEU (EMD) und RE15_LAUF_VORHER / RE15_LAUF_NEU.
  Runde 4: vorher = V4 Runde 3 (sfv4/emd/EM30_V4r3.EMD sha256 dfbc30bc, Lauf sfv4/r/V4r3), neu = V4 Runde 4 (sfv4/emd/EM30_V4.EMD,
  Lauf sfv4/r/V4r4); Runde-3-Leisten liegen in SF_belege/v4/r3/. Neue Riegel in den Unterschriften: TQ/TK (05), FO (01/06), KS (03/07).
  Vorlage 1 = Beschreibung1_Vorlage.png (n1485) entblendet (Messblatt V4 Abschnitt 1, k 0,766/0,764/0,768), Vorlage 2 =
  Beschreibung2_Vorlage.png (n1435); Beschriftung = linke 640 Spalten von Beschreibung1.png / Beschreibung2.png (wie gezeichnet).
  V3 r2 / V4 = b2-Render in der Spielkamera 50A0 Schnitt 12 (RDT-Licht, Hintergrund BSS 50A0/12) in posen_v4 "135" / "85"
  (Lichtgegenprobe gegen RE15_FRAMEDUMP: dE00 0,1, riegel_v4). 07 = Spiellaeufe (RE15_FRAMEDUMP, auf das Videobild registriert).
Nutzerpunkte (Linienenden, Messblatt V4 Abschnitt 1a) gelb in den Original-Spalten; im Port die gemessenen Gegenstuecke (riegel_v4).
  Runde 5: vorher = V4 Runde 4 (sfv4/emd/EM30_V4r4.EMD sha256 00d642b7, Lauf sfv4/r/V4r4), neu = V4 Runde 5 (sfv4/emd/EM30_V4.EMD, Lauf
  sfv4/r/V4r5); Runde-4-Leisten in SF_belege/v4/r4/. Neue Riegel in den Unterschriften: TN/TV/TR (05), AF/FO (01), dazu 09 Schwanzverlauf
  (Farbe und Helligkeit entlang der Achse: Original | V4 r4 | neu, als Kurve und als Streifen).
"""
import os, sys, json
import numpy as np, cv2
HIER = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HIER)
sys.path.insert(0, os.path.join(os.path.dirname(HIER), 'nb2neu'))
import v4_gemein as G, b2, riegel_v4 as RV
from PIL import Image

AUS = os.path.join(G.BAUM, 'analysis', 'befunde_runde41', 'SF_belege', 'v4')
NAME = {'vorher': 'V4 r4', 'neu': 'V4 r5'}
EMD = {'vorher': os.environ.get('RE15_LEISTE_VORHER', os.path.join(G.SFV4, 'emd', 'EM30_V4r4.EMD')),     # sha256 00d642b7 (V4 Runde 4)
       'neu': os.environ.get('RE15_LEISTE_NEU', G.EMD_V4)}                                                 # V4 Runde 5
LAUF = {'vorher': os.environ.get('RE15_LAUF_VORHER', os.path.join(G.SFV4, 'r', 'V4r4')),
        'neu': os.environ.get('RE15_LAUF_NEU', os.path.join(G.SFV4, 'r', 'V4r5'))}
PKT1 = {'Auge': (382, 164), 'Haare': (404, 160), 'Nase': (395, 177)}
PKT2 = {'Auge': (460, 234), 'Haare': (468, 241), 'Beginn': (357, 243), 'Knick': (290, 308), 'Ende': (295, 343)}
BIEG2 = (295, 252)
GELB, CYAN, MAG, GRUEN = (255, 255, 0), (0, 255, 255), (255, 0, 255), (0, 255, 0)
_cache = {}


def mo(k):
    if k not in _cache:
        _cache[k] = b2.modell(EMD[k])
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
        zq, _iq = RV.schwanz_form(m, P); zo, _io = RV.oberkoerper_vorn(m, P); zk2 = RV.kopf_buehne320(m, P)
        zv, _iv = RV.schwanz_verlauf(m, P); za, _ia = RV.ausreisser_farben(m, P)          # Runde 5 (O8 / O9)
        zt = zt + zq + zv
        zf = zf + zo + za
        zk = zk + zk2
        _cache[key] = dict(zk={z['mass'].split()[0]: z for z in zk}, zs={z['mass'].split()[0]: z for z in zs},
                           zt={z['mass'].split()[0]: z for z in zt}, zf=zf, ef=ef, ik=ik, is_=is_, st=st, fr=rest[3])
    return _cache[key]


def kurz(z):
    return ('OK ' if z['ok'] else 'ROT ') + z['ist']


def speichern(name, zeilen, kopf):
    p = os.path.join(AUS, name + '.jpg')
    b2.leiste(zeilen, kopf, p, q=90)
    print('geschrieben', p)


KOPFZEILE = 'Spalten: Nutzer-Vorlage (Originalbild) | Nutzer-Beschriftung | V4 Runde 4 (vorher) | V4 Runde 5 (neu)'


# ═══════════════════════════════════════════════════ 01 / 02 ganze Figur
def l01():
    box = (250, 110, 560, 470)
    o = vorlage_u8(1); bsch = beschriftung(1)
    for n, p in PKT1.items():
        punkt(o, p, GELB, 3, n)
    t = [kachel(o, box, 2, 'Vorlage 1 n1485 entblendet', ['H1 302 px (Messblatt V4)']),
         kachel(bsch, box, 2, 'Beschreibung1 (Nutzer)', ['Auge / Haare / Nase'])]
    for k in ('vorher', 'neu'):
        img, ids, iv = render(k, '135'); r = riegel(k)
        img = img.copy()
        bl = r['ik']['blob']; punkt(img, bl['sp'], GELB, 3, 'Auge')
        t.append(kachel(img, box, 2, '%s Pose 135 (Kamera 50A0/12)' % NAME[k],
                        ['K4 ' + kurz(r['zk']['K4']), 'K2 ' + kurz(r['zk']['K2'])[:60], 'K5 ' + kurz(r['zk']['K5'])[:60],
                         'K11 ' + kurz(r['zk']['K11'])[:60],
                         'FO ' + kurz(next(z for z in r['zf'] if z['mass'].startswith('FO')))[:66],
                         'AF ' + kurz(next(z for z in r['zf'] if z['mass'].startswith('AF Ausreisserfarben Pose 135')))[:66]]))
    speichern('01_vorn_kamera12', [t], ['SF V4 - 01 vorn: Spielkamera 50A0 Cut 12 wie Vorlagebild 1 (h135), Figur in Pose posen_v4 "135"',
                                         KOPFZEILE])


def l02():
    box = (230, 160, 520, 450)
    o = vorlage_u8(2); bsch = beschriftung(2)
    for n, p in PKT2.items():
        punkt(o, p, GELB, 3, n)
    t = [kachel(o, box, 2, 'Vorlage 2 n1435', ['H2 253 px (Messblatt V4)']), kachel(bsch, box, 2, 'Beschreibung2 (Nutzer)', [''])]
    for k in ('vorher', 'neu'):
        img, ids, iv = render(k, '85'); r = riegel(k)
        img = img.copy(); s = r['st']
        for n, q in (('Beginn', s['beginn']), ('Knick', s['knick']), ('Ende', s['ende'])):
            punkt(img, q, GELB, 3, n)
        punkt(img, r['is_']['blob']['sp'], GELB, 3, 'Auge')
        t.append(kachel(img, box, 2, '%s Pose 85 (Kamera 50A0/12)' % NAME[k],
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
    for k in ('vorher', 'neu'):
        img, ids, iv = render(k, '135'); r = riegel(k); img0 = img.copy(); img = img.copy()
        kb = r['ik']['kopfbox']
        punkt(img, r['ik']['blob']['sp'], GELB, 1)
        for n, kk in (('Haare', 'KV2'), ('Nase', 'KV3')):
            bx = r['ik']['fenster'][kk][0]
            punkt(img, (0.5 * (bx[0] + bx[2]), 0.5 * (bx[1] + bx[3])), CYAN, 1)
        zl = ['K1 ' + kurz(r['zk']['K1']), 'K2 ' + kurz(r['zk']['K2'])[:58], 'K3 ' + kurz(r['zk']['K3'])[:58],
              'K4 ' + kurz(r['zk']['K4'])[:58], 'K5 ' + kurz(r['zk']['K5'])[:58], 'K7 ' + kurz(r['zk']['K7']), 'K9 ' + kurz(r['zk']['K9']),
              'K11 ' + kurz(r['zk']['K11'])[:58], 'KS320 ' + kurz(r['zk']['KS'])[:56]]
        z1.append(kachel(img0, kopfbox_rand(kb), 4, '%s Kopf x4' % NAME[k], zl))
        z2.append(kachel(img, kopfbox_rand(kb), 4, '%s mit Messpunkten' % NAME[k], ['gelb Augenfleck (Schwerpunkt), cyan KV2/KV3']))
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
    for k in ('vorher', 'neu'):
        img, ids, iv = render(k, '85'); r = riegel(k); img0 = img.copy(); img = img.copy()
        punkt(img, r['is_']['blob']['sp'], GELB, 1)
        kb = r['is_']['kopfbox']
        bx = G.fenster(kb, RV.FENSTER['KS2']); punkt(img, (0.5 * (bx[0] + bx[2]), 0.5 * (bx[1] + bx[3])), CYAN, 1)
        z1.append(kachel(img0, kopfbox_rand(kb, 14), 4, '%s Kopf seitlich x4' % NAME[k],
                         ['S1 ' + kurz(r['zs']['S1'])[:62], 'S2 ' + kurz(r['zs']['S2']), 'S3 ' + kurz(r['zs']['S3'])[:62],
                          'S4 ' + kurz(r['zs']['S4'])]))
        z2.append(kachel(img, kopfbox_rand(kb, 14), 4, '%s mit Messpunkten' % NAME[k], ['gelb Augenfleck, cyan KS2']))
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
    for k in ('vorher', 'neu'):
        img, ids, iv = render(k, '85'); r = riegel(k); img = img.copy(); s = r['st']
        H = s['H']; b = np.array(s['beginn'])
        rel = [((p[0] - 357) / G.H_ORIG[2], (p[1] - 243) / G.H_ORIG[2]) for p in zug]
        zp = [tuple(b + np.array(q) * H) for q in rel]
        cv2.polylines(img, [np.int32(zp)], False, GELB, 1, cv2.LINE_AA)
        cv2.polylines(img, [np.int32(s['C'][::3])], False, CYAN, 1, cv2.LINE_AA)
        f = s['T5']
        t.append(kachel(img, box, 3, '%s Schwanz x3 (gelb Vorlage am Port-Beginn, H-skaliert; cyan Mittellinie)' % NAME[k],
                        ['T2 ' + kurz(r['zt']['T2']), 'T3 ' + kurz(r['zt']['T3']), 'T4 ' + kurz(r['zt']['T4'])[:60],
                         'T5 %.3f/%.3f/%.3f/%.3f H' % (f[0.1], f[0.3], f[0.5], f[0.7]), 'T5q ' + kurz(r['zt']['T5q'])[:64],
                         'T6 ' + kurz(r['zt']['T6'])[:64], 'TQ ' + kurz(r['zt']['TQ'])[:66], 'TK ' + kurz(r['zt']['TK'])[:66],
                         'TN ' + kurz(r['zt']['TN'])[:66], 'TR ' + kurz(r['zt']['TR'])[:66], 'TV ' + kurz(r['zt']['TV'])[:66]]))
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
        for k in ('vorher', 'neu'):
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
        for k in ('vorher', 'neu'):
            r = riegel(k)
            la, n = r['ef'][k_]
            d = G.de00(la, ziel) if n else 99
            z.append(feld(BF._lab_rgb(*la) if n else None, '%s %s dE00 %.1f' % ('OK' if d <= 5 and abs(la[0] - ziel[0]) <= 4 else 'ROT',
                                                                         '/'.join('%.1f' % v for v in la), d)))
        reihen.append(z)
    # Runde 2: Sicht-Boxen (FS) und Schwanzachse (T9) aus den Riegelzeilen
    for k in ('vorher', 'neu'):
        riegel(k)
    for mass_ in [z['mass'] for z in riegel('neu')['zf'] if z['mass'].startswith('FS')]:
        zv = {k: next((z for z in riegel(k)['zf'] if z['mass'] == mass_), None) for k in ('vorher', 'neu')}
        ziel = [float(x) for x in zv['neu']['soll'].split(',')[0].split('/')]
        z = [feld(BF._lab_rgb(*ziel), '%s Ziel %s' % (mass_[3:], zv['neu']['soll'][:22])), feld(None, 'Sicht-Box (Pruefung V4 r1/r2)')]
        for k in ('vorher', 'neu'):
            if zv[k] is None:
                z.append(feld(None, '-')); continue
            ist = zv[k]['ist']
            la = [float(x) for x in ist.split()[0].split('/')]
            z.append(feld(BF._lab_rgb(*la), '%s %s' % ('OK' if zv[k]['ok'] else 'ROT', ist[:46])))
        reihen.append(z)
    for k_ in ('F15', 'F16', 'F17', 'F18'):
        ziel = RV.ZIEL[k_]
        z = [feld(BF._lab_rgb(*ziel), '%s Ziel %s' % (k_, '/'.join('%.1f' % v for v in ziel))), feld(None, 'Schwanz (T8)')]
        for k in ('vorher', 'neu'):
            la, n = riegel(k)['fr'][k_]
            d = G.de00(la, ziel) if n >= 3 else 99
            z.append(feld(BF._lab_rgb(*la) if n >= 3 else None, '%s %s dE00 %.1f' % ('OK' if d <= 6 else 'ROT',
                                                                               '/'.join('%.1f' % v for v in la), d)))
        reihen.append(z)
    speichern('06_farben', reihen, ['SF V4 - 06 Farben: Ziel (Videoraum, Messblatt V4 Abschnitt 5) | Fenster | V4 r4 | V4 r5 '
                                    '(Vergleichsregel sigma 1,5, V = a + b x P, Lab D65, dE00)', KOPFZEILE])


# ═══════════════════════════════════════════════════ 07 Gang (Spiellaeufe)
def l07():
    """Runde 3 (O3): Spielbilder h100/h118/h135 beider Laeufe und die koerperfeste Messung (t7_koerperfest_v4.py) in den Unterschriften;
    letzte Zeile: Feder-Stand des neuen Laufs an Pose 85 (wie Runde 2, nur Ansicht) mit der koerperfesten Abweichung."""
    import c6
    import t7_koerperfest_v4 as T7K
    box = (230, 100, 560, 480)
    zeilen = []
    mk = {k: T7K.messen(EMD[k], os.path.join(LAUF[k], 'debug.log')) for k in ('vorher', 'neu')}
    rg = {k: {z['mass'].split()[0]: z for z in T7K.riegel(mk[k])} for k in ('vorher', 'neu')}

    SPB = {k: RV.spielbild(os.path.join(LAUF[k], 'debug.log')) for k in ('vorher', 'neu')}

    def ab(k, h):
        t = mk[k]['takte'].get(h)
        if not t:
            return ['kein Takt h%d' % h]
        return ['koerperfest Abw. B/Bg/K/E %s H' % '/'.join('%.3f' % t['abw'][q] for q in ('Beginn', 'Biegung', 'Knick', 'Ende')),
                'Biegung %.0f Grad (Keyframe %.0f), Strecke %.3f H (Kf %.3f)' % (t['w3'], t['w3k'], t['abfall'], t['abfall_k'])]
    for h in (85, 100, 118, 135):
        v = cv2.cvtColor(c6.vid(h), cv2.COLOR_BGR2RGB)
        if h == 135:
            v = vorlage_u8(1)
        if h == 85:
            v = vorlage_u8(2)
        bs = beschriftung(1) if h == 135 else beschriftung(2) if h == 85 else np.full((480, 640, 3), 30, np.uint8)
        if h not in (85, 135):
            b2.schrift(bs, 'keine Beschriftung fuer h%d' % h, 240, 300, (200, 200, 200), 0.6)
        t = [kachel(v, box, 1, 'Video 4 h%d (n%d)' % (h, 1350 + h), ['']), kachel(bs, box, 1, 'Beschriftung', [''])]
        for k in ('vorher', 'neu'):
            p = c6.port(LAUF[k], h)
            im = cv2.cvtColor(p, cv2.COLOR_BGR2RGB) if p is not None else np.zeros((480, 640, 3), np.uint8)
            zus = []
            if h in (118, 135):
                zs4, _i = SPB[k]
                zus = [z['mass'].split()[0] + ' ' + kurz(z)[:70] for z in zs4 if z['mass'].endswith('h%d' % h)]
            t.append(kachel(im, box, 1, '%s Spiel h%d (RE15_FRAMEDUMP)' % (NAME[k], h), ab(k, h) + zus))
        zeilen.append(t)
    zs = []
    for k in ('vorher', 'neu'):
        zs.append(['%s: %s %s' % (NAME[k], ('OK' if rg[k][q]['ok'] else 'ROT'), rg[k][q]['ist'][:120]) for q in ('T7a', 'T7b', 'T7c', 'T7d', 'Boden')])
    gg = RV.gang(EMD['neu'], os.path.join(LAUF['neu'], 'debug.log'), (100, 110, 118, 125, 135))
    tz = []
    for h in (100, 110, 118, 125, 135):
        if h in gg and 'bild' in gg[h]:
            t = mk['neu']['takte'].get(h, {})
            tz.append(kachel(gg[h]['bild'][0], (230, 160, 520, 450), 1, '%s Feder h%d an Pose 85 (Ansicht)' % (NAME['neu'], h),
                             ['koerperfest max %.3f H' % max(t['abw'].values()) if t else '']))
    zeilen.append(tz)
    speichern('07_gang', zeilen, ['SF V4 - 07 Gang: Spiellaeufe (RE15_FRAMEDUMP, registriert), T7 koerperfest (Kette gegen das Feder-Ziel '
                                  'der gezeichneten Pose, H = Figurhoehe, Riegel <= 0,06 H)', KOPFZEILE + '; letzte Zeile: nur V4 r5',
                                  ' | '.join(zs[0])[:300], ' | '.join(zs[1])[:300]])


# ═══════════════════════════════════════════════════ 08 Rundum
def l08():
    m = mo('neu'); p0 = dict(G.posen()['85'])
    t = []
    for i, dg in enumerate(range(0, 360, 60)):
        p = dict(p0, gier=(p0['gier'] + int(round(dg * 4096 / 360))) & 0xFFF)
        img, ids = b2.zeichne(m, p, None, licht=False)
        ys, xs = np.nonzero(ids >= 0)
        cx, cy = int(xs.mean()), int(ys.mean())
        t.append(kachel(img, (max(cx - 150, 0), max(cy - 150, 0), min(cx + 150, 640), min(cy + 150, 480)), 1,
                        '%s Gier +%d Grad (neutral)' % (NAME['neu'], dg), ['']))
    speichern('08_rundum_V4', [t[:3], t[3:]], ['SF V4 - 08 rundum: 6 Blickwinkel, neutrales Licht (Texelfarbe), Pose 85 gedreht',
                                               'nur V4 Runde 5 (neu)'])


# ═══════════════════════════════════════════════════ 09 Schwanzverlauf (Runde 5, Entscheid O8)
def _verlauf(img_v, maske, P, punkte):
    """Lab (3x3 in der Maske) entlang der Mittellinie P, Bogenlaenge auf die Punkte Beginn / Biegung / Knick / Ende normiert (0 / 1 / 2 / 3)."""
    L = G.lab(img_v)
    lab = []
    for x, y in np.round(P).astype(int):
        mm = maske[y - 1:y + 2, x - 1:x + 2]
        lab.append(L[y - 1:y + 2, x - 1:x + 2][mm].mean(0) if mm.any() else L[y, x])
    s = np.r_[0, np.cumsum(np.linalg.norm(np.diff(P, axis=0), axis=1))]
    k = [s[int(np.argmin(np.linalg.norm(P - np.asarray(q, float), axis=1)))] for q in punkte]
    t = np.interp(s, k, [0, 1, 2, 3], left=np.nan, right=np.nan)
    vor = s < k[0]
    t[vor] = (s[vor] - k[0]) / max(k[1] - k[0], 1e-6)
    return t, np.array(lab)


def l09():
    import matplotlib
    matplotlib.use('Agg')
    import matplotlib.pyplot as plt
    from skimage.color import lab2rgb
    import r5_schwanz_textur as ST
    import t2_mass_v4 as T2M
    from scipy import ndimage as nd
    Po, mo_, lm_o = ST.achse_original()
    reihen = [('Original Vorlage 2 (n1435, roh)', ) + _verlauf(G.vorlage(2), nd.binary_erosion(mo_), Po, lm_o)]
    for k in ('vorher', 'neu'):
        img, ids, iv = render(k, '85'); r = riegel(k); s = r['st']
        tp = T2M.port(ids, s['H'])
        P = np.asarray(tp['P'], float); P = np.array([P[max(0, j - 2):j + 3].mean(0) for j in range(len(P))])
        lm = [tuple(s['beginn']), tuple(tp['mitte']), tuple(s['knick']), tuple(s['ende'])]
        reihen.append(('%s (Pose 85, Vergleichsregel)' % NAME[k], ) + _verlauf(G.videoraum(img), nd.binary_erosion(np.isin(ids, G.SCHWANZ)), P, lm))
    fig = plt.figure(figsize=(14, 9), dpi=100)
    ax1 = fig.add_axes([0.06, 0.58, 0.9, 0.34]); ax2 = fig.add_axes([0.06, 0.20, 0.9, 0.30])
    farben = ['black', 'tab:red', 'tab:blue']
    for (nm, t, lab), c in zip(reihen, farben):
        ok = np.isfinite(t)
        ax1.plot(t[ok], lab[ok, 0], color=c, label=nm); ax2.plot(t[ok], lab[ok, 1], color=c, label=nm)
    for ax, yl in ((ax1, 'L*'), (ax2, 'a*')):
        for x0, txt in ((0, 'Beginn'), (1, 'Biegung'), (2, 'Knick'), (3, 'Ende')):
            ax.axvline(x0, color='grey', lw=0.6, ls='--'); ax.text(x0, ax.get_ylim()[1], txt, fontsize=8, va='top')
        ax.set_ylabel(yl); ax.set_xlim(-0.6, 3.05); ax.grid(alpha=0.3)
    ax1.legend(fontsize=8, loc='upper right')
    ax1.set_title('SF V4 - 09 Schwanzverlauf entlang der Achse (Mittellinie t2_mass_v4; x = Abschnitt Beginn-Biegung-Knick-Ende, < 0 = Ruecken)',
                  fontsize=10)
    # Streifen: je Quelle die Farbe entlang der Achse (gleiche x-Achse)
    for i, (nm, t, lab) in enumerate(reihen):
        ax = fig.add_axes([0.06, 0.13 - i * 0.045, 0.9, 0.035])
        xs = np.linspace(-0.6, 3.05, 400)
        ok = np.isfinite(t)
        streif = np.stack([np.interp(xs, t[ok], lab[ok, c], left=np.nan, right=np.nan) for c in range(3)], -1)
        rgb = np.where(np.isfinite(streif), lab2rgb(np.nan_to_num(streif).reshape(1, -1, 3)).reshape(-1, 3), 1.0)
        ax.imshow(np.repeat(rgb[None], 8, 0), aspect='auto', extent=(-0.6, 3.05, 0, 1))
        ax.set_yticks([]); ax.set_xticks([]); ax.set_ylabel(nm.split(' (')[0], rotation=0, ha='right', fontsize=8, va='center')
    pfad = os.path.join(AUS, '09_schwanz_verlauf.jpg')
    fig.savefig(pfad, dpi=100); plt.close(fig)
    print('geschrieben', pfad)


def main():
    os.makedirs(AUS, exist_ok=True)
    w = sys.argv[1] if len(sys.argv) > 1 else 'alle'
    for k, f in (('01', l01), ('02', l02), ('03', l03), ('04', l04), ('05', l05), ('06', l06), ('07', l07), ('08', l08), ('09', l09)):
        if w in ('alle', k):
            f()


if __name__ == '__main__':
    main()
