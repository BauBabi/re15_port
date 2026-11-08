"""SF V4 Kopf nach Nutzerbeschreibung (Bauer, Runde 3; Runde 1 in SF_belege/v4/ot1/, Runde 2 in ot2/): Bildleisten fuer die Abnahme
(analysis/befunde_runde41/SF_belege/v4/).

    python leisten_ref2tex.py [alle|01|02|03|04|05|07|08|09]

Spalten NUR: Nutzer-Vorlage (Originalbild) | Nutzer-Beschriftung | Referenz 2 (gelobt, Spiel-Asset EM30_HANGAR.EMD sha256 cfd1ac42) |
V4 neu (Runde 3, sfv4/emd/EM30_V4.EMD = EM30_V4ot3.EMD). Keine Spalten frueherer V4-Runden.
Render wie leisten_v4.py: b2 in der Spielkamera 50A0 Schnitt 12 (RDT-Licht, Hintergrund BSS 50A0/12), Posen posen_v4 "135" / "85", roh (kein
Gamma); 07 = Spiellaeufe (RE15_FRAMEDUMP, Laeufe sfv4/r/R2ot = Spiel-Asset, sfv4/r/V4ot3 = V4 neu Runde 3). Unterschriften = riegel_ref2tex.py.
09 = Kopf-Zonen x6 (Vorlage-Kopf | Beschreibung1-Kopf | Ref2-Kopf | neu, gleiches Licht, roh, Nutzerpunkte Auge gelb / Haare, Nase cyan).
Umgebungsvariablen RE15_LEISTE_NEU (EMD) / RE15_LAUF_NEU / RE15_LAUF_REF2.
"""
import os, sys
import numpy as np, cv2
HIER = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HIER)
sys.path.insert(0, os.path.join(os.path.dirname(HIER), 'nb2neu'))
import v4_gemein as G, b2, riegel_v4 as RV
import riegel_ref2tex as RR
import leisten_v4 as LV

AUS = LV.AUS
NAME = {'ref2': 'Referenz 2', 'neu': 'V4 neu'}
EMD = {'ref2': G.REF2, 'neu': os.environ.get('RE15_LEISTE_NEU', G.EMD_V4)}
LAUF = {'ref2': os.environ.get('RE15_LAUF_REF2', os.path.join(G.SFV4, 'r', 'R2ot')),
        'neu': os.environ.get('RE15_LAUF_NEU', os.path.join(G.SFV4, 'r', 'V4ot3'))}
KOPFZEILE = 'Spalten: Nutzer-Vorlage (Originalbild) | Nutzer-Beschriftung | Referenz 2 (gelobt) | V4 neu (Kopf nach Nutzerbeschreibung, Runde 3)'
GELB, CYAN = LV.GELB, LV.CYAN
_cache = {}


def mo(k):
    if ('mo', k) not in _cache:
        _cache[('mo', k)] = b2.modell(EMD[k])
    return _cache[('mo', k)]


def render(k, h):
    if ('r', k, h) not in _cache:
        _cache[('r', k, h)] = G.zeichne(mo(k), G.posen()[h], voll=True)
    return _cache[('r', k, h)]


def rg(k):
    """Riegelzeilen (riegel_ref2tex) je Kurzname, dazu die Mess-Infos von Kopf und Schwanz."""
    if ('rg', k) not in _cache:
        P = G.posen()
        Z, info = RR.pruefen(EMD[k])
        zk, ik, _ = RV.kopf_vorn(mo(k), P); zs, is_, _ = RV.kopf_seite(mo(k), P)
        d = {}
        for z in Z:
            n = z['mass'].replace('T-K4 ', '')
            key = n.split()[0] if not n.startswith('T-K') else ' '.join(n.split()[:1])
            if n.startswith('K8 Messblatt'):
                key = 'K8w'
            if n.startswith('T-K9b Rand'):
                key = 'T-K9b-Rand'
            d.setdefault(key, z)
        _cache[('rg', k)] = dict(z=d, ik=ik, is_=is_, st=info['schwanz'])
    return _cache[('rg', k)]


def kurz(z, n=64):
    return (('INFO ' if z.get('info') else ('OK ' if z['ok'] else 'ROT ')) + z['ist'])[:n]


def zl(k, keys, n=64):
    r = rg(k)['z']
    return ['%s %s' % (q, kurz(r[q], n)) for q in keys if q in r]


def l01():
    box = (250, 110, 560, 470)
    o = LV.vorlage_u8(1); bsch = LV.beschriftung(1)
    for n, p in LV.PKT1.items():
        LV.punkt(o, p, GELB, 3, n)
    t = [LV.kachel(o, box, 2, 'Vorlage 1 n1485 entblendet', ['H1 302 px (Messblatt V4)']),
         LV.kachel(bsch, box, 2, 'Beschreibung1 (Nutzer)', ['Auge / Haare / Nase'])]
    for k in ('ref2', 'neu'):
        img, ids, iv = render(k, '135'); img = img.copy()
        LV.punkt(img, rg(k)['ik']['blob']['sp'], GELB, 3, 'Auge')
        t.append(LV.kachel(img, box, 2, '%s Pose 135 (Kamera 50A0/12)' % NAME[k],
                           zl(k, ['T-K1a', 'T-K1b', 'T-K1c', 'T-K2', 'K2', 'K4', 'K5', 'K9'], 66)))
    LV.speichern('01_vorn_kamera12', [t], ['SF V4 Kopf nach Nutzerbeschreibung (Runde 3) - 01 vorn: Spielkamera 50A0 Cut 12 wie Vorlagebild 1 (h135), Pose posen_v4 "135"',
                                         KOPFZEILE])


def l02():
    box = (230, 160, 520, 450)
    o = LV.vorlage_u8(2); bsch = LV.beschriftung(2)
    for n, p in LV.PKT2.items():
        LV.punkt(o, p, GELB, 3, n)
    t = [LV.kachel(o, box, 2, 'Vorlage 2 n1435', ['H2 253 px (Messblatt V4)']), LV.kachel(bsch, box, 2, 'Beschreibung2 (Nutzer)', [''])]
    for k in ('ref2', 'neu'):
        img, ids, iv = render(k, '85'); img = img.copy(); s = rg(k)['st']
        for n in ('beginn', 'knick', 'ende'):
            if n in s:
                LV.punkt(img, s[n], GELB, 3, n.capitalize())
        LV.punkt(img, rg(k)['is_']['blob']['sp'], GELB, 3, 'Auge')
        t.append(LV.kachel(img, box, 2, '%s Pose 85 (Kamera 50A0/12)' % NAME[k],
                           zl(k, ['T-K3a', 'T-K3c', 'T2', 'T3', 'S2', 'S4'], 66)))
    LV.speichern('02_seite_wie_bild2', [t], ['SF V4 Kopf nach Nutzerbeschreibung (Runde 3) - 02 seitlich: Kamera/Pose wie Vorlagebild 2 (h85), posen_v4 "85"',
                                           KOPFZEILE])


def l03():
    o0 = LV.vorlage_u8(1); bsch = LV.beschriftung(1)
    ob = LV.kopfbox_rand(G.KB_ORIG[1])
    o = o0.copy()
    for n, p in LV.PKT1.items():
        LV.punkt(o, p, GELB if n == 'Auge' else CYAN, 1)
    z1 = [LV.kachel(o0, ob, 4, 'Vorlage 1 Kopf x4', ['links grau mit Auge (hell), bis Nase', 'rechts Haare braeunlich; Kuppe dunkel rotbraun']),
          LV.kachel(bsch, ob, 4, 'Beschreibung1 Kopf x4', ['Nutzer: Auge / Haare / Nase', 'Video-Lab nur Richtung (Messblatt Runde 3)'])]
    z2 = [LV.kachel(o, ob, 4, 'Vorlage 1 mit Nutzerpunkten', ['gelb Auge, cyan Haare/Nase']), LV.kachel(bsch, ob, 4, 'Beschreibung1', [''])]
    for k in ('ref2', 'neu'):
        img, ids, iv = render(k, '135'); r = rg(k); img0 = img.copy(); img = img.copy()
        kb = r['ik']['kopfbox']
        LV.punkt(img, r['ik']['blob']['sp'], GELB, 1)
        for kk in ('KV2', 'KV3'):
            bx = r['ik']['fenster'][kk][0]
            LV.punkt(img, (0.5 * (bx[0] + bx[2]), 0.5 * (bx[1] + bx[3])), CYAN, 1)
        z1.append(LV.kachel(img0, LV.kopfbox_rand(kb), 4, '%s Kopf x4' % NAME[k],
                            zl(k, ['K1', 'K2', 'K3', 'K4', 'K5', 'K6', 'K7', 'K8', 'K9', 'K10'], 58)))
        z2.append(LV.kachel(img, LV.kopfbox_rand(kb), 4, '%s mit Messpunkten' % NAME[k], ['gelb Augenfleck (Schwerpunkt), cyan KV2/KV3']))
    LV.speichern('03_kopf_nah_vorn_x4', [z1, z2], ['SF V4 Kopf nach Nutzerbeschreibung (Runde 3) - 03 Kopf vorn x4 (Kopfbox +8 px; Port = Bbox Id 9); Zeile 1 ohne, '
                                                  'Zeile 2 mit Marken', KOPFZEILE])


def l04():
    o0 = LV.vorlage_u8(2); bsch = LV.beschriftung(2)
    ob = LV.kopfbox_rand(G.KB_ORIG[2])
    o = o0.copy()
    for n in ('Auge', 'Haare'):
        LV.punkt(o, LV.PKT2[n], GELB if n == 'Auge' else CYAN, 1)
    z1 = [LV.kachel(o0, ob, 4, 'Vorlage 2 Kopf x4', ['Auge hell; Haarkante dunkel davor', 'Kopf grau, Oberseite dunkel rotbraun']),
          LV.kachel(bsch, ob, 4, 'Beschreibung2 Kopf x4', ['Nutzer: Auge / Haare von der Seite'])]
    z2 = [LV.kachel(o, ob, 4, 'Vorlage 2 mit Nutzerpunkten', ['gelb Auge, cyan Haare']), LV.kachel(bsch, ob, 4, 'Beschreibung2', [''])]
    for k in ('ref2', 'neu'):
        img, ids, iv = render(k, '85'); r = rg(k); img0 = img.copy(); img = img.copy()
        LV.punkt(img, r['is_']['blob']['sp'], GELB, 1)
        kb = r['is_']['kopfbox']
        bx = G.fenster(kb, RV.FENSTER['KS2']); LV.punkt(img, (0.5 * (bx[0] + bx[2]), 0.5 * (bx[1] + bx[3])), CYAN, 1)
        z1.append(LV.kachel(img0, LV.kopfbox_rand(kb, 14), 4, '%s Kopf seitlich x4' % NAME[k], zl(k, ['S1', 'S2', 'S3', 'S4'], 62)))
        z2.append(LV.kachel(img, LV.kopfbox_rand(kb, 14), 4, '%s mit Messpunkten' % NAME[k], ['gelb Augenfleck, cyan KS2']))
    LV.speichern('04_kopf_nah_seite_x4', [z1, z2], ['SF V4 Kopf nach Nutzerbeschreibung (Runde 3) - 04 Kopf seitlich x4; Zeile 1 ohne, Zeile 2 mit Marken', KOPFZEILE])


def l05():
    box = (240, 185, 420, 365)
    o = LV.vorlage_u8(2); bsch = LV.beschriftung(2)
    zug = [LV.PKT2['Beginn'], LV.BIEG2, LV.PKT2['Knick'], LV.PKT2['Ende']]
    o_ = o.copy()
    for im in (o_, bsch):
        cv2.polylines(im, [np.int32(zug)], False, GELB, 1, cv2.LINE_AA)
    z1 = [LV.kachel(o, box, 3, 'Vorlage 2 Schwanz x3', ['ohne Marken']), LV.kachel(bsch, box, 3, 'Beschreibung2 Schwanz x3', ['Beginn-Biegung-Knick-Ende'])]
    z2 = [LV.kachel(o_, box, 3, 'Vorlage 2 mit Polygonzug', ['gelb Beginn-Biegung-Knick-Ende']), LV.kachel(bsch, box, 3, 'Beschreibung2', [''])]
    for k in ('ref2', 'neu'):
        img, ids, iv = render(k, '85'); img0 = img.copy(); img = img.copy(); s = rg(k)['st']
        if 'beginn' in s and 'H' in s:
            H = s['H']; b = np.array(s['beginn'])
            zp = [tuple(b + np.array(((p[0] - 357) / G.H_ORIG[2], (p[1] - 243) / G.H_ORIG[2])) * H) for p in zug]
            cv2.polylines(img, [np.int32(zp)], False, GELB, 1, cv2.LINE_AA)
        z1.append(LV.kachel(img0, box, 3, '%s Schwanz x3' % NAME[k], zl(k, ['T-K3a', 'T-K3b', 'T-K3c', 'T-K2'], 70)))
        z2.append(LV.kachel(img, box, 3, '%s mit Polygonzug (gelb, am Port-Beginn H-skaliert)' % NAME[k],
                            zl(k, ['T2', 'T3', 'T4', 'T5', 'T5q', 'T6', 'TK'], 66)))
    LV.speichern('05_schwanz_x3', [z1, z2], ['SF V4 Kopf nach Nutzerbeschreibung (Runde 3) - 05 Schwanz x3 (unveraendert Runde 2): Zeile 1 ohne Marken (Textur), Zeile 2 Polygonzug der '
                                            'Vorlage', KOPFZEILE])


def l07():
    import c6
    import t7_koerperfest_v4 as T7K
    box = (230, 100, 560, 480)
    zeilen = []
    mk = {k: T7K.messen(EMD[k], os.path.join(LAUF[k], 'debug.log')) for k in ('neu',)}
    r7 = {z['mass'].split()[0]: z for z in T7K.riegel(mk['neu'])}
    for h in (85, 100, 118, 135):
        v = cv2.cvtColor(c6.vid(h), cv2.COLOR_BGR2RGB)
        if h == 135:
            v = LV.vorlage_u8(1)
        if h == 85:
            v = LV.vorlage_u8(2)
        bs = LV.beschriftung(1) if h == 135 else LV.beschriftung(2) if h == 85 else np.full((480, 640, 3), 30, np.uint8)
        if h not in (85, 135):
            b2.schrift(bs, 'keine Beschriftung fuer h%d' % h, 240, 300, (200, 200, 200), 0.6)
        t = [LV.kachel(v, box, 1, 'Video 4 h%d (n%d)' % (h, 1350 + h), ['']), LV.kachel(bs, box, 1, 'Beschriftung', [''])]
        for k in ('ref2', 'neu'):
            p = c6.port(LAUF[k], h)
            im = cv2.cvtColor(p, cv2.COLOR_BGR2RGB) if p is not None else np.zeros((480, 640, 3), np.uint8)
            unten = ['']
            if k == 'neu':
                tk = mk['neu']['takte'].get(h)
                unten = ['koerperfest Abw. B/Bg/K/E %s H' % '/'.join('%.3f' % tk['abw'][q] for q in ('Beginn', 'Biegung', 'Knick', 'Ende'))] if tk else ['']
            t.append(LV.kachel(im, box, 1, '%s Spiel h%d (RE15_FRAMEDUMP)' % (NAME[k], h), unten))
        zeilen.append(t)
    zs = ['%s %s %s' % (q, 'OK' if r7[q]['ok'] else 'ROT', r7[q]['ist'][:60]) for q in ('T7a', 'T7b', 'T7c', 'T7d', 'Boden')]
    LV.speichern('07_gang', zeilen, ['SF V4 Kopf nach Nutzerbeschreibung (Runde 3) - 07 Gang: Spiellaeufe (RE15_FRAMEDUMP, registriert); T7 koerperfest nur V4 neu',
                                     KOPFZEILE, ' | '.join(zs)[:300]])


def l08():
    t = []
    for k in ('ref2', 'neu'):
        m = mo(k); p0 = dict(G.posen()['85'])
        for dg in range(0, 360, 60):
            p = dict(p0, gier=(p0['gier'] + int(round(dg * 4096 / 360))) & 0xFFF)
            img, ids = b2.zeichne(m, p, None, licht=False)
            ys, xs = np.nonzero(ids >= 0)
            cx, cy = int(xs.mean()), int(ys.mean())
            t.append(LV.kachel(img, (max(cx - 150, 0), max(cy - 150, 0), min(cx + 150, 640), min(cy + 150, 480)), 1,
                               '%s Gier +%d Grad (neutral)' % (NAME[k], dg), ['']))
    LV.speichern('08_rundum_V4', [t[0:6], t[6:12]], ['SF V4 Kopf nach Nutzerbeschreibung (Runde 3) - 08 rundum: 6 Blickwinkel, neutrales Licht (Texelfarbe), '
                                                   'Pose 85 gedreht', 'Zeile 1 Referenz 2 (gelobt), Zeile 2 V4 neu (Runde 3)'])


def l09():
    """09 Kopf-Zonen x6: Vorlage-Kopf | Beschreibung1-Kopf | Ref2-Kopf | neu (Pose 135, RDT-Licht, roh); Zeile 1 mit den Nutzerpunkten (Vorlage) bzw.
    Messpunkten (neu: Augenfleck gelb, KV2 / KV3 cyan), Zeile 2 neu mit Zonenkarte (grau / braun / Kuppe aus den Texel-Klassen, riegel_ref2tex.zonen_emd)
    und den T-K9-Zeilen."""
    import birkin_formen as BF
    import ref2tex_v4 as RT
    o0 = LV.vorlage_u8(1); bsch = LV.beschriftung(1)
    ob = LV.kopfbox_rand(G.KB_ORIG[1], 6)
    o = o0.copy()
    for n, p in LV.PKT1.items():
        LV.punkt(o, p, GELB if n == 'Auge' else CYAN, 1)
    z1 = [LV.kachel(o, ob, 6, 'Vorlage 1 Kopf x6 (Nutzerpunkte)', ['gelb Auge, cyan Haare / Nase (Linienenden Messblatt 1a)']),
          LV.kachel(bsch, ob, 6, 'Beschreibung1 Kopf x6', ['Lesung: links grau bis Nase, rechts Haare'])]
    z2 = [LV.kachel(o0, ob, 6, 'Vorlage 1 Kopf x6 (ohne Marken)', ['']), LV.kachel(bsch, ob, 6, 'Beschreibung1', [''])]
    kp = BF._variante_v4()['kopf_v4']
    for k in ('ref2', 'neu'):
        img, ids, iv = render(k, '135'); r = rg(k); img0 = img.copy(); img = img.copy()
        kb = r['ik']['kopfbox']
        LV.punkt(img, r['ik']['blob']['sp'], GELB, 1)
        for kk in ('KV2', 'KV3'):
            bx = r['ik']['fenster'][kk][0]
            LV.punkt(img, (0.5 * (bx[0] + bx[2]), 0.5 * (bx[1] + bx[3])), CYAN, 1)
        z1.append(LV.kachel(img, LV.kopfbox_rand(kb, 6), 6, '%s Kopf x6 (Messpunkte)' % NAME[k], ['gelb Augenfleck (Schwerpunkt), cyan KV2 (Haar) / KV3 (Nase)']))
        if k == 'neu':
            A = RT.atlas(EMD[k]); zk = RR.zonen_emd(A, kp)
            zm, _ids = RT.zonen_render(mo(k), kp, G.posen()['135'], zk)
            vis = img0.copy().astype(float)
            for zi, col in ((1, (120, 120, 120)), (2, (150, 90, 20)), (3, (170, 30, 30))):
                m = zm == zi
                vis[m] = 0.45 * vis[m] + 0.55 * np.array(col)
            vis = vis.astype(np.uint8)
            z2.append(LV.kachel(vis, LV.kopfbox_rand(kb, 6), 6, 'V4 neu: Zonen (grau / braun / Kuppe)',
                                zl(k, ['T-K9a', 'T-K9b', 'T-K9b-Rand', 'T-K9c', 'T-K9d', 'T-K9e'], 90)))
        else:
            z2.append(LV.kachel(img0, LV.kopfbox_rand(kb, 6), 6, '%s Kopf x6 (ohne Marken)' % NAME[k], ['keine Zonen (Referenz 2 ist der gelobte Koerper)']))
    LV.speichern('09_kopf_zonen_x6', [z1, z2], ['SF V4 Kopf nach Nutzerbeschreibung (Runde 3) - 09 Kopf-Zonen x6: Pose 135 (Kamera 50A0/12, RDT-Licht, roh); '
                                               'Zeile 1 Nutzer-/Messpunkte, Zeile 2 ohne Marken bzw. Zonenkarte', KOPFZEILE])


def main():
    os.makedirs(AUS, exist_ok=True)
    w = sys.argv[1] if len(sys.argv) > 1 else 'alle'
    for k, f in (('01', l01), ('02', l02), ('03', l03), ('04', l04), ('05', l05), ('07', l07), ('08', l08), ('09', l09)):
        if w in ('alle', k):
            f()


if __name__ == '__main__':
    main()
