"""Leser A: Vergleichstafel 00_koepfe_re2_vs_ki.png.
    python re15_port/tools/r41_sichel/re2kopf/tafel.py OUTDIR ZIEL.png
Zeile 1 KI-Koepfe (FRONT + RECHTS, drei Bilder), Zeile 2 Beschreibung1/2 (Beschriftung + Kopf), Zeile 3 Referenz 2 (EM30 Mesh 9),
Zeile 4-6 RE2 0x30 / 0x31 / 0x33 Mesh 3: vorn, rechts (EA.render, gleiches kamerarelatives Licht), flach (Texelfarbe), TIM-Ausschnitt.
Alle Kacheln: Kopfbox auf KH Pixel Hoehe normiert (Kopfbox = Maske der Kopf-Pixel bzw. Ausschnittsbox der Vorlagen). Kacheln roh, keine Nachbearbeitung.
"""
import os, sys, json
import numpy as np
from PIL import Image, ImageDraw, ImageFont
OUT, ZIEL = sys.argv[1], sys.argv[2]
K = json.load(open(os.path.join(OUT, 'koepfe.json'))); V = json.load(open(os.path.join(OUT, 'vorlagen.json'))); M = json.load(open(os.path.join(OUT, 'mess.json')))
V80 = r'C:/workspace/git/reAi_v2/Version_80%/'
KH, TW, TH, PAD, LBL = 200, 250, 250, 10, 44
BG, FG, DIM, ACC = (18, 19, 22), (232, 233, 236), (160, 163, 170), (214, 138, 74)


def font(sz, bold=False):
    for nm in (('arialbd.ttf' if bold else 'arial.ttf'), 'DejaVuSans.ttf'):
        try:
            return ImageFont.truetype(nm, sz)
        except Exception:
            pass
    return ImageFont.load_default()


F1, F2, F3 = font(15, True), font(12), font(11)


def kachel(img, box=None):
    """Bild (RGB uint8) -> Kachel TW x TH; Ausschnitt box (x0,y0,x1,y1 inkl.) auf KH Hoehe skaliert, zentriert."""
    im = Image.fromarray(np.ascontiguousarray(img))
    if box is not None:
        x0, y0, x1, y1 = box
        im = im.crop((x0, y0, x1 + 1, y1 + 1))
    s = KH / im.height
    im = im.resize((max(1, int(round(im.width * s))), KH), Image.LANCZOS)
    t = Image.new('RGB', (TW, TH), (40, 40, 44))
    t.paste(im, ((TW - im.width) // 2, (TH - im.height) // 2))
    return t


def lade(nm):
    return np.asarray(Image.open(os.path.join(OUT, nm)).convert('RGB'))


zeilen = []
# Zeile 1: KI
z = []
for k in ('ki1_front', 'ki1_rechts', 'ki2_front', 'ki2_rechts', 'ki3_front', 'ki3_rechts'):
    a = M['augen_ki'].get(k, [])
    z.append((kachel(lade(k + '.png')), '%s  %s' % (V[k]['datei'], 'FRONT' if 'front' in k else 'RECHTS'),
              'Box %s  w/h %.2f' % (list(V[k]['box']), V[k]['w_h']),
              'Lab %s  Auge %s' % (V[k]['zonen']['gesamt'][:3], ', '.join('(%.2f, %.2f)' % (d['x_anteil'], d['y_anteil']) for d in a[:2]))))
zeilen.append(('1  KI-Bilder (Vorlage Nutzer, Kopf-Ausschnitte)', z))
# Zeile 2: Beschreibung
z = []
for b, box in ((1, (345, 100, 520, 200)), (2, (395, 165, 640, 300))):
    ann = np.asarray(Image.open(V80 + 'Beschreibung%d.png' % b).convert('RGB'))
    z.append((kachel(ann, box), 'Beschreibung%d.png (Nutzer-Linien)' % b, 'Ausschnitt %s' % (box,), ''))
    kb = V['beschreibung%d' % b]
    z.append((kachel(lade('beschreibung%d_kopf.png' % b)), 'Beschreibung%d-Kopf (%s)' % (b, 'vorn, entblendet' if b == 1 else 'seitlich, roh'),
              'Kopfbox %s  w/h %.2f' % (kb['box'], kb['w_h']), 'Lab gesamt %s  Zaehne %.3f Haar %.3f' % (kb['zonen']['gesamt'][:3], M['zaehne']['beschreibung%d' % b][0], M['haar']['beschreibung%d' % b])))
zeilen.append(('2  Nutzer-Beschriftung (Videobilder n1485 / n1435, ROOM50A0 Cut 12)', z))
# Zeile 3-6: Modelle
namen = {'ref2': ('Referenz 2 (gelobt): EM30_HANGAR.EMD Mesh 9', 'RE1.5-Format, TIM im EMD'),
         'em30': ('RE2 kind 0x30 = G1, Mesh 3 (Kopf)', 'CDEMD0.EMS Sektor 2788, 152188 B'),
         'em31': ('RE2 kind 0x31 = G2, Mesh 3 (Kopf)', 'CDEMD0.EMS Sektor 2944, 193124 B'),
         'em33': ('RE2 kind 0x33 = G3, Mesh 3 (Kopf)', 'CDEMD0.EMS Sektor 3114, 204968 B')}
for i, kand in enumerate(('ref2', 'em30', 'em31', 'em33')):
    k = K[kand]; z = []
    for vn in ('vorn', 'rechts'):
        img = lade('%s_kopf_%s.png' % (kand, vn))
        z.append((kachel(img, k[vn]['kopfbox_px']), '%s, Licht' % vn, '%d x %d Einh. (h x b)  w/h %.2f' % (k[vn]['hoehe_einh'], k[vn]['breite_einh'], k[vn]['seitenverhaeltnis_w_h']),
                  'Lab gesamt %s' % (k[vn]['zonen_licht']['gesamt'][:3],)))
    img = lade('%s_kopf_vorn_flach.png' % kand)
    rot = [a for a in k['vorn']['augen_flach']['rot'] if a['px'] < 2000][:3]
    z.append((kachel(img, k['vorn']['kopfbox_px']), 'vorn, flach (Texelfarbe)', 'Lab %s  Zaehne %.3f  Haar %.3f' % (k['vorn']['zonen_flach']['gesamt'][:3], M['zaehne'][kand][0], M['haar'][kand]),
              'rote Flecken: %s' % ', '.join('(%.2f, %.2f)' % (a['x_anteil'], a['y_anteil']) for a in rot)))
    tx = lade('%s_kopf_textur.png' % kand)
    t = k['texel']
    z.append((kachel(tx), 'TIM-Ausschnitt des Kopfes', 'u %s v %s (page,clut) %s' % (t['u'], t['v'], t['page_clut']), '%d Texel, Lab %s' % (t['n'], t['lab'][:3])))
    zeilen.append(('%d  %s  -  %s' % (i + 3, *namen[kand]), z))

COLS = max(len(z) for _, z in zeilen)
W = PAD * 2 + COLS * (TW + PAD)
H = PAD + sum(28 + TH + LBL + PAD for _ in zeilen) + 60
sheet = Image.new('RGB', (W, H), BG); dr = ImageDraw.Draw(sheet)
y = PAD
for titel, z in zeilen:
    dr.text((PAD, y), titel, font=F1, fill=ACC); y += 28
    for c, (t, l1, l2, l3) in enumerate(z):
        x = PAD + c * (TW + PAD)
        sheet.paste(t, (x, y))
        dr.text((x, y + TH + 2), l1, font=F2, fill=FG); dr.text((x, y + TH + 16), l2, font=F3, fill=DIM); dr.text((x, y + TH + 29), l3, font=F3, fill=DIM)
    y += TH + LBL + PAD
dr.text((PAD, y), 'Kopfbox je Kachel auf %d px Hoehe normiert. Modell-Kacheln: emd_ansichtsblatt.render (orthogonal, kamerarelatives Licht gleich fuer alle), Pose Clip 0 Bild 0, Kamera +X (vorn) / -Z (rechte Flanke des Wesens wie Beschreibung2).' % KH, font=F3, fill=DIM)
dr.text((PAD, y + 16), 'Lab = CIE-Lab D65 (skimage) als [L, a, b]. "flach" = Texelfarbe ohne Schattierung. Zaehne = Anteil heller ungesaettigter Pixel in der Maulzone (y 0,65-1), Haar = Anteil brauner Pixel (Farbwinkel 30-75, Chroma > 12) am Kopf. Zahlen: analysis/befunde_runde41/SF_belege/re2kopf/*.json', font=F3, fill=DIM)
os.makedirs(os.path.dirname(os.path.abspath(ZIEL)), exist_ok=True)
sheet.save(ZIEL); print('geschrieben', ZIEL, sheet.size)
