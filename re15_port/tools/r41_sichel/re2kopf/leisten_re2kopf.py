"""SF V5 Kopf aus RE2 (Bauer, Runde 3): Bildleisten fuer die Abnahme (analysis/befunde_runde41/SF_belege/v4/).

    python leisten_re2kopf.py [alle|01|02|03|04|08|10|11]

Runde 3 (Nutzer 2026-11-08: "Es ist abgerundet so wie es auf dem Original Foto aussieht"): Spalten 01-04/11 NUR Originalfoto
(Version_80%/Beschreibung1_Vorlage.png bzw. Beschreibung2_Vorlage.png, roh) | Nutzer-Beschriftung (Beschreibung1/2) | Referenz 2 (gelobt, Spiel-Asset
EM30_HANGAR.EMD sha256 cfd1ac42) | neu (sfv4/emd/EM30_V5r3.EMD = EM30_V4.EMD). Keine Spalten frueherer Runden, keine KI-Bilder (08/10 zeigen sie
weiter als Formvergleich, sind aber nicht Teil der Runde-3-Abnahme).
Render: b2 in der Spielkamera 50A0 Schnitt 12 (RDT-Licht, Hintergrund BSS 50A0/12), Posen posen_v4 "135" / "85", roh (kein Gamma).
10 = Kopf x6 unter GLEICHEM Licht (emd_ansichtsblatt.render, orthogonal, kamerarelatives Licht wie Leser A re2_kopf.py): KI-Kopf vorn |
KI-Kopf seitlich | RE2-Quellkopf G2 (0x31 Mesh 3) | neu vorn | neu seitlich (Mesh 9 des neuen EMD).
11 = Spielbilder (RE15_FRAMEDUMP, Laeufe sfv4/r/R2ot = Spiel-Asset, sfv4/r/V4re2 = neu), Kopf-Ausschnitte h85 / h135.
Unterschriften: riegel_re2kopf.py (JSON SF_belege/v4/re2/riegel_re2kopf_V4re2.json).
"""
import os, sys, json
import numpy as np, cv2
from PIL import Image
HIER = os.path.dirname(os.path.abspath(__file__))
TOOLS = os.path.dirname(os.path.dirname(HIER))
sys.path.insert(0, HIER); sys.path.insert(0, TOOLS); sys.path.insert(0, os.path.join(TOOLS, 'r41_sichel', 'v4')); sys.path.insert(0, os.path.join(TOOLS, 'r41_sichel', 'nb2neu'))
import v4_gemein as G, b2, riegel_v4 as RV
import leisten_v4 as LV
import emd_ansichtsblatt as EA
import re2_leser as RL
import kopf_re2_bau as KB
import kopf_re2_mess as KM

AUS = LV.AUS
RE2DIR = os.path.join(G.BAUM, 'analysis', 'befunde_runde41', 'SF_belege', 'v4', 're2')
NAME = {'ref2': 'Referenz 2', 'neu': 'neu (V5 Runde 3: grau, abgerundet)'}
EMD = {'ref2': G.REF2, 'neu': os.environ.get('RE15_LEISTE_NEU', os.path.join(G.SFV4, 'emd', 'EM30_V5r3.EMD'))}
LAUF = {'ref2': os.environ.get('RE15_LAUF_REF2', os.path.join(G.SFV4, 'r', 'R2ot')),
        'neu': os.environ.get('RE15_LAUF_NEU', os.path.join(G.SFV4, 'r', 'V5r3'))}
RIEGEL_JSON = os.environ.get('RE15_LEISTE_RIEGEL', os.path.join(G.BAUM, 'analysis', 'befunde_runde41', 'SF_belege', 'v4', 're2r3', 'riegel_re2kopf_V5r3.json'))
KI_BILD = G.V80 + 'image_1.png'
KI_FIGUR = dict(FRONT=(60, 0, 470, 500), RIGHT=(520, 0, 1024, 500), BACK=(60, 512, 470, 1012), LEFT=(520, 512, 1024, 1012))
KI_KOPF = dict(FRONT=(205, 5, 305, 115), RIGHT=(925, 80, 1010, 170))          # vorlagen_kopf.KI (Leser A, von Hand gesetzt)
KI_KOEPFE = {'image_1.png': ((205, 5, 305, 115), (925, 80, 1010, 170)), 'image_2 (2).png': ((205, 15, 300, 110), (865, 85, 945, 160)),
             'image_3 (3).png': ((205, 10, 290, 110), (590, 45, 660, 125))}
KOPFZEILE = 'Spalten: Originalfoto (Beschreibung1/2_Vorlage, roh) | Nutzer-Beschriftung | Referenz 2 (gelobt, Spiel-Asset) | neu (RE2-G2-Kopf, Palettenregel r3 grau, Schnauze auf Kappe gerundet; roh, kein Gamma)'
GELB, CYAN = LV.GELB, LV.CYAN
_cache = {}


def foto(b):
    """Originalfoto Version_80%/Beschreibung{b}_Vorlage.png, roh (nicht entblendet, kein Gamma)."""
    if ('foto', b) not in _cache:
        _cache[('foto', b)] = np.asarray(Image.open(G.V80 + 'Beschreibung%d_Vorlage.png' % b).convert('RGB'))
    return _cache[('foto', b)]


def ki(datei=KI_BILD):
    if ('ki', datei) not in _cache:
        _cache[('ki', datei)] = np.asarray(Image.open(datei).convert('RGB'))
    return _cache[('ki', datei)]


def ki_kachel(box, f, titel, zeilen, hoehe=None, datei=KI_BILD):
    """Ausschnitt des KI-Bilds als Kachel; mit hoehe: auf diese Pixelhoehe skaliert (gleich grosse Koepfe)."""
    im = ki(datei)[box[1]:box[3], box[0]:box[2]]
    if hoehe:
        s = hoehe / im.shape[0]
        im = cv2.resize(im, (int(round(im.shape[1] * s)), int(hoehe)), interpolation=cv2.INTER_AREA)
    return b2.tafel(np.ascontiguousarray(im), titel, zeilen, f=f)


def mo(k):
    if ('mo', k) not in _cache:
        _cache[('mo', k)] = b2.modell(EMD[k])
    return _cache[('mo', k)]


def render(k, h):
    if ('r', k, h) not in _cache:
        _cache[('r', k, h)] = G.zeichne(mo(k), G.posen()[h], voll=True)
    return _cache[('r', k, h)]


def mess(k):
    if ('m', k) not in _cache:
        _cache[('m', k)] = KM.messen(mo(k), bilder=True)
    return _cache[('m', k)]


def riegel_neu():
    if 'rg' not in _cache:
        p = RIEGEL_JSON
        d = {}
        if os.path.exists(p):
            for z in json.load(open(p))['zeilen']:
                key = z['mass'].replace('T-R2 ', '').replace('T-K4 ', '').replace('INFO ', '').split()[0]
                d.setdefault(key, z); d.setdefault(z['mass'].split()[0], z)
        _cache['rg'] = d
    return _cache['rg']


def zl(keys, n=66):
    r = riegel_neu()
    return ['%s %s' % (q, (('OK ' if r[q]['ok'] else 'ROT ') + r[q]['ist'])[:n]) for q in keys if q in r]


def kachel_mark(k, h, box, f, titel, zeilen, marken=True):
    img, ids, _iv = render(k, h); img = img.copy()
    if marken and k == 'neu':
        m = mess(k)[h]
        img[m['ma']] = GELB; img[m['mz']] = CYAN
    return LV.kachel(img, box, f, titel, zeilen)


def l01():
    box = (250, 110, 560, 470)
    bsch = LV.beschriftung(1)
    t = [LV.kachel(foto(1), box, 2, 'Originalfoto Beschreibung1_Vorlage (roh)', ['Video n1485, Kamera 50A0/12']),
         LV.kachel(bsch, box, 2, 'Beschreibung1 (Nutzer)', ['Auge links / Haare rechts / Nase'])]
    for k in ('ref2', 'neu'):
        t.append(kachel_mark(k, '135', box, 2, '%s Pose 135 (Kamera 50A0/12)' % NAME[k],
                             zl(['T-K1a', 'T-R1d', 'T-V1', 'T-V2a', 'K10', 'K4', 'K5', 'T-K9e'], 70) if k == 'neu' else ['Spiel-Asset EM30_HANGAR.EMD cfd1ac42'], marken=False))
    LV.speichern('01_vorn_kamera12', [t], ['SF V5 Kopf aus RE2 (Runde 3) - 01 vorn: Spielkamera 50A0 Cut 12 wie Originalfoto 1 (h135), Pose posen_v4 "135"', KOPFZEILE])


def l02():
    box = (230, 160, 520, 450)
    bsch = LV.beschriftung(2)
    t = [LV.kachel(foto(2), box, 2, 'Originalfoto Beschreibung2_Vorlage (roh)', ['Video n1435, Kamera 50A0/12']),
         LV.kachel(bsch, box, 2, 'Beschreibung2 (Nutzer)', ['Auge / Haare von der Seite'])]
    for k in ('ref2', 'neu'):
        t.append(kachel_mark(k, '85', box, 2, '%s Pose 85 (Kamera 50A0/12)' % NAME[k],
                             zl(['S2', 'KS2', 'T-V2c', 'T-V2b', 'T-K3a', 'T2'], 70) if k == 'neu' else ['Spiel-Asset EM30_HANGAR.EMD cfd1ac42'], marken=False))
    LV.speichern('02_seite_wie_bild2', [t], ['SF V5 Kopf aus RE2 (Runde 3) - 02 seitlich: Kamera/Pose wie Originalfoto 2 (h85), posen_v4 "85"', KOPFZEILE])


def l03():
    bsch = LV.beschriftung(1); ob = LV.kopfbox_rand(G.KB_ORIG[1])
    z1 = [LV.kachel(foto(1), ob, 4, 'Originalfoto 1 Kopf x4 (roh)', ['Kopfbox Messblatt 368-416 x 139-191']),
          LV.kachel(bsch, ob, 4, 'Beschreibung1 Kopf x4', ['Nutzer: Auge / Haare / Nase'])]
    z2 = [LV.kachel(foto(1), ob, 4, 'Originalfoto 1', ['']), LV.kachel(bsch, ob, 4, 'Beschreibung1', [''])]
    for k in ('ref2', 'neu'):
        img, ids, _iv = render(k, '135'); kb = G.kopfbox(ids)
        z1.append(kachel_mark(k, '135', LV.kopfbox_rand(kb), 4, '%s Kopf x4' % NAME[k],
                              zl(['T-V1', 'K10', 'K4', 'K1k', 'K5', 'T-R3'], 60) if k == 'neu' else ['Mesh 9 Referenz 2 (rosa Klumpen, Leser A)'], marken=False))
        z2.append(kachel_mark(k, '135', LV.kopfbox_rand(kb), 4, '%s mit Marken' % NAME[k], ['gelb Auge (Marker-Render), cyan Zaehne'] if k == 'neu' else ['']))
    LV.speichern('03_kopf_nah_vorn_x4', [z1, z2], ['SF V5 Kopf aus RE2 (Runde 3) - 03 Kopf vorn x4 (Kopfbox +8 px; Port = Bbox Id 9); Zeile 1 ohne, Zeile 2 mit Marken', KOPFZEILE])


def l04():
    bsch = LV.beschriftung(2); ob = LV.kopfbox_rand(G.KB_ORIG[2])
    z1 = [LV.kachel(foto(2), ob, 4, 'Originalfoto 2 Kopf x4 (roh)', ['Vorderkante x 464-465 vor dem weissen Pfosten']),
          LV.kachel(bsch, ob, 4, 'Beschreibung2 Kopf x4', ['Nutzer: Auge / Haare von der Seite'])]
    z2 = [LV.kachel(foto(2), ob, 4, 'Originalfoto 2', ['']), LV.kachel(bsch, ob, 4, 'Beschreibung2', [''])]
    for k in ('ref2', 'neu'):
        img, ids, _iv = render(k, '85'); kb = G.kopfbox(ids)
        im2 = img.copy()
        if k == 'neu':
            m = mess(k)['85']; b = m['KS2']['box']; cv2.rectangle(im2, (b[0], b[1]), (b[2], b[3]), CYAN, 1); im2[m['ma']] = GELB
        z1.append(LV.kachel(img, LV.kopfbox_rand(kb, 14), 4, '%s Kopf seitlich x4' % NAME[k],
                            zl(['S2', 'KS2', 'T-V2a', 'T-V2c'], 62) if k == 'neu' else ['Spiel-Asset']))
        z2.append(LV.kachel(im2, LV.kopfbox_rand(kb, 14), 4, '%s mit Marken' % NAME[k], ['gelb Auge, cyan Fenster KS2'] if k == 'neu' else ['']))
    LV.speichern('04_kopf_nah_seite_x4', [z1, z2], ['SF V5 Kopf aus RE2 (Runde 3) - 04 Kopf seitlich x4; Zeile 1 ohne, Zeile 2 mit Marken', KOPFZEILE])


def l08():
    z0 = [ki_kachel(KI_FIGUR[v], 1, 'KI-Bild image_1.png %s' % v, ['']) for v in ('FRONT', 'RIGHT', 'BACK', 'LEFT')]
    z0 += [LV.kachel(LV.beschriftung(1), (250, 110, 560, 470), 1, 'Beschreibung1', ['']), LV.kachel(LV.beschriftung(2), (230, 160, 520, 450), 1, 'Beschreibung2', [''])]
    zeilen = [z0]
    for k in ('ref2', 'neu'):
        t = []
        m = mo(k); p0 = dict(G.posen()['85'])
        for dg in range(0, 360, 60):
            p = dict(p0, gier=(p0['gier'] + int(round(dg * 4096 / 360))) & 0xFFF)
            img, ids = b2.zeichne(m, p, None, licht=False)
            ys, xs = np.nonzero(ids >= 0)
            cx, cy = int(xs.mean()), int(ys.mean())
            t.append(LV.kachel(img, (max(cx - 150, 0), max(cy - 150, 0), min(cx + 150, 640), min(cy + 150, 480)), 1,
                               '%s Gier +%d Grad (neutral)' % (NAME[k], dg), ['']))
        zeilen.append(t)
    LV.speichern('08_rundum_V4', zeilen, ['SF V5 Kopf aus RE2 (Runde 2) - 08 rundum: Zeile 1 KI-Bild 4 Ansichten + Beschriftungen, Zeile 2 Referenz 2, Zeile 3 neu; '
                                         '6 Blickwinkel, neutrales Licht (Texelfarbe), Pose 85 gedreht', KOPFZEILE])


def kopf_render(mo_, bone, view, KH=240, TILE=300, kf=None):
    """Nur das Kopf-Mesh, orthogonal, kamerarelatives Licht (EA.render) - wie Leser A re2_kopf.kopf_render."""
    kf0 = mo_.frames[mo_.clips[0][0]] & 0xfff if kf is None else kf
    V, N, F = mo_.geometrie(kf0)
    Fk = [f for f in F if f['bone'] == bone]
    vi = sorted({v for f in Fk for v in f['v']})
    right, down, depth = (np.asarray(v, float) for v in view)
    KV = V[vi]; ys = KV @ down; xs = KV @ right
    h = ys.max() - ys.min(); SS = 2; sc = KH * SS / h; W = H = TILE * SS
    cx = W / 2 - (xs.min() + xs.max()) / 2 * sc; cy = H / 2 - (ys.min() + ys.max()) / 2 * sc
    col, alp = EA.render(mo_, V, N, Fk, right, down, depth, W, H, sc, cx, cy)
    rgb = np.clip(col * 1.62, 0, 1) ** (1 / 1.18)
    img = (rgb * 255).astype(np.uint8); a = alp > 0.5
    img = cv2.resize(img, (TILE, TILE), interpolation=cv2.INTER_AREA)
    a = cv2.resize(a.astype(np.uint8), (TILE, TILE), interpolation=cv2.INTER_NEAREST) > 0
    bg = np.full_like(img, 40); bg[a] = img[a]
    return bg


def l10():
    """10 Kopf x6: KI-Kopf vorn | KI-Kopf seitlich | RE2-Quellkopf (G2 0x31 Mesh 3 vorn / rechts) | neu vorn | neu seitlich - gleiches Licht
    (EA.render), Kopfbox auf 240 px."""
    import re2_leser as RL_
    g2 = RL_.modell(KB.G2)
    neu = mo('neu')
    zeilen = []
    for datei, (bf, br) in KI_KOEPFE.items():
        t = [ki_kachel(bf, 1, 'KI-Kopf %s FRONT' % datei, [''], hoehe=240, datei=G.V80 + datei),
             ki_kachel(br, 1, 'KI-Kopf %s RIGHT' % datei, [''], hoehe=240, datei=G.V80 + datei)]
        if datei == 'image_1.png':
            t += [b2.tafel(kopf_render(g2, 3, EA.V_FRONT), 'RE2 G2 0x31 Mesh 3 vorn (Quelle)', ['CDEMD0.EMS Sektor 2944; TIM Sektor 2895 Zeile 0']),
                  b2.tafel(kopf_render(neu, KB.KOPF, EA.V_FRONT), 'neu Mesh 9 vorn (+X des Modells)', ['G2-Kopf, Texel bytegleich; Zeile 486 grau / 488 dunkel; Maul G3']),
                  b2.tafel(kopf_render(neu, KB.KOPF, EA.V_RIGHT), 'neu Mesh 9 rechte Flanke (-Z)', ['Pose Clip 0 Bild 0 des EMD'])]
        elif datei == 'image_2 (2).png':
            t += [b2.tafel(kopf_render(g2, 3, EA.V_RIGHT), 'RE2 G2 0x31 Mesh 3 rechts (Quelle)', ['']),
                  b2.tafel(kopf_render(neu, KB.KOPF, EA.V_LEFT), 'neu Mesh 9 linke Flanke (+Z)', ['Haarseite (W)-links']),
                  b2.tafel(kopf_render(neu, KB.KOPF, EA.V_TOP), 'neu Mesh 9 von oben (-Y)', ['Kuppe mit roten Adern'])]
        else:
            g3 = RL_.modell(KB.G3)
            t += [b2.tafel(kopf_render(g3, 3, EA.V_FRONT), 'RE2 G3 0x33 Mesh 3 vorn (Zahn-Quelle)', ['TIM Sektor 3065 Zeile 0, Kachel u 2-13 v 5-40']),
                  b2.tafel(kopf_render(neu, KB.KOPF, ((0, 0, 1), (1, 0, 0), (0, 1, 0))), 'neu Mesh 9 von unten (+Y)', ['Maul mit Zahnreihen (G3-Kachel)']),
                  b2.tafel(kopf_render(neu, KB.KOPF, EA.V_BACK), 'neu Mesh 9 von hinten (-X)', [''])]
        zeilen.append(t)
    LV.speichern('10_kopf_ki_re2_neu_x6', zeilen, ['SF V5 Kopf aus RE2 (Runde 2) - 10 Kopf x6: je Zeile ein KI-Bild (FRONT | RIGHT) und drei Modell-Kacheln unter GLEICHEM Licht '
                                                  '(emd_ansichtsblatt.render, orthogonal, kamerarelativ; Kopfbox 240 px)',
                                                  'Zeile 1: RE2-Quellkopf vorn | neu vorn | neu rechts; Zeile 2: Quelle rechts | neu links (Haar) | neu oben; Zeile 3: G3-Zahnquelle | neu unten (Maul) | neu hinten'])


def l11():
    import c6
    boxen = {135: (280, 110, 430, 260), 85: (240, 100, 540, 400)}     # Kopf im Spielbild (c6.port, registriert): h135 ~ x 320-400; h85 ganzer Oberkoerper
    fakt = {135: 3, 85: 2}                                             # (der Kopf liegt im Spielbild h85 hinter dem Buckel, anders als in der b2-Pose 85)
    zeilen = []
    for h in (135, 85):
        bs = LV.beschriftung(1) if h == 135 else LV.beschriftung(2)
        b_ = 1 if h == 135 else 2
        t = [LV.kachel(foto(b_), boxen[h], fakt[h], 'Originalfoto %d x%d (roh)' % (b_, fakt[h]), ['']),
             LV.kachel(bs, boxen[h], fakt[h], 'Beschreibung%d Kopf x%d' % (b_, fakt[h]), [''])]
        for k in ('ref2', 'neu'):
            p = c6.port(LAUF[k], h)
            im = cv2.cvtColor(p, cv2.COLOR_BGR2RGB) if p is not None else np.zeros((480, 640, 3), np.uint8)
            t.append(LV.kachel(im, boxen[h], fakt[h], '%s Spiel h%d (RE15_FRAMEDUMP) x%d' % (NAME[k], h, fakt[h]), ['Lauf %s' % os.path.basename(LAUF[k])]))
        zeilen.append(t)
    LV.speichern('11_spiel_koepfe', zeilen, ['SF V5 Kopf aus RE2 (Runde 3) - 11 Spiel-Koepfe: RE15_FRAMEDUMP-Bilder h135 (Zeile 1) und h85 (Zeile 2), registriert wie Abnahme 3',
                                            KOPFZEILE])


def main():
    os.makedirs(AUS, exist_ok=True)
    w = sys.argv[1] if len(sys.argv) > 1 else 'alle'
    for k, f in (('01', l01), ('02', l02), ('03', l03), ('04', l04), ('08', l08), ('10', l10), ('11', l11)):
        if w in ('alle', k):
            f()


if __name__ == '__main__':
    main()
