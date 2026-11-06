# Leser B (SF V4) L4: Farben je Region im Video (Bild 1 n1485 vorn, Bild 2 n1435 seitlich) und am Port V3 r2.
# Port vorn = Spielbild des Laufs V3r2 (RE15_FRAMEDUMP, nb2neu/c6.port, auf Videokoordinaten registriert), h135 (= Bild 1).
# Port Schwanz = Leiste SF_belege/v3/01_seite_h83_h85_h92.jpg, Spalte V3, Zeile h85 (Buehnen-Render, Spielkamera, mit Messlinien:
#   Linienfarben werden ausgeschlossen); das Spielbild V3r2 h85 zeigt den Schwanz nicht (andere Pose, gemessen am Bild).
# Alle Rechteck-Fenster: angegeben (x0,y0,x1,y1) inklusive, 2 px erodiert = Rechteck um 2 px je Seite verkleinert; Masken: binary_erosion 2.
# Aufruf: python b_farben.py VERSION80 SFV4 BELEGE NB2NEU RUNS LEISTE_01SEITE
import sys, os, json, numpy as np, cv2
from PIL import Image, ImageDraw
from scipy import ndimage as ndi
import b_common as C
V, S, BEL, NB2, RUNS, LEISTE = sys.argv[1:7]
os.environ['NB6_RUNS'] = RUNS; sys.path.insert(0, NB2)
import c6
L1 = json.load(open(f'{S}/B_L1.json')); LL = json.load(open(f'{S}/B_L2_L5.json')); HJ = json.load(open(f'{S}/B_H.json'))
I1 = np.asarray(Image.open(f'{V}/Beschreibung1_Vorlage.png').convert('RGB')).astype(float)
I2 = np.asarray(Image.open(f'{V}/Beschreibung2_Vorlage.png').convert('RGB')).astype(float)
I1k = np.load(f'{S}/B_I1k.npy')
P1 = cv2.cvtColor(c6.port('V3r2', 135), cv2.COLOR_BGR2RGB).astype(float)
P1_120 = cv2.cvtColor(c6.port('V3r2', 120), cv2.COLOR_BGR2RGB).astype(float)
def LAB(img): return C.lab(img.reshape(-1, 3)).reshape(img.shape)
def rect(x0, y0, x1, y1, e=2):
    m = np.zeros((480, 640), bool); m[y0 + e:y1 - e + 1, x0 + e:x1 - e + 1] = True; return m
def er(m, k=2): return ndi.binary_erosion(m, iterations=k)
def disc(x, y, r):
    yy, xx = np.mgrid[:480, :640]; return (xx - x) ** 2 + (yy - y) ** 2 <= r * r
LK1, LP1, L2_ = LAB(I1k), LAB(P1), LAB(I2)
reg = {}   # name -> dict(bild, maske, beschreibung)
kb = LL['L5_vorn']['kopfbox']
E1 = {z['text']: z['ende'] for z in L1['1']['zuordnung']}; E2 = {z['text']: z['ende'] for z in L1['2']['zuordnung']}
R2 = np.load(f'{S}/B_L2_regionen.npz')
def r2(key): return R2[key]
# ---------- Bild 1 vorn (Masken am abblende-korrigierten Bild bestimmt, gleiche Maske fuer alle Varianten)
reg['V1 Kopf-Kuppe'] = (rect(kb['x0'] + 10, kb['y0'], kb['x1'] - 10, kb['y0'] + 12), f"Rechteck x {kb['x0']+10}..{kb['x1']-10}, y {kb['y0']}..{kb['y0']+12} (Kopfbox oben)")
reg['V1 Augenumfeld'] = (C.fenster(*E1['Auge'], 15), 'Fenster 15x15 um Auge-Ende %s' % E1['Auge'])
reg['V1 Auge-Struktur'] = (er(r2('1_Auge')), 'L2-Region Auge (hell, 114 px), 2 px erodiert')
reg['V1 Haare'] = (er(r2('1_Haare')), 'L2-Region Haare (dunkel, 231 px), 2 px erodiert')
reg['V1 Nase'] = (C.fenster(*E1['Nase'], 9), 'Fenster 9x9 um Nase-Ende %s' % E1['Nase'])
reg['V1 Maul/Kinn dunkle Zone'] = (rect(378, 186, 398, 194, 0) & (LK1[..., 0] < 13), 'Rechteck x 378..398, y 186..194 und L*(korr) < 13 (dunkle Zone unter der Nase)')
box = rect(295, 160, 465, 250, 0)   # rechts bis x 465: dahinter helle Wand/Tuerrahmen (gemessen im Belegbild)
hell = box & (LK1[..., 1] < 10) & (LK1[..., 0] >= np.percentile(LK1[..., 0][box], 70))
reg['V1 Brust/Rumpf hell'] = (er(hell, 1), 'Rechteck x 295..465, y 160..250; a*<10 und L* >= 70. Perzentil der Box; 1 px erodiert (2 px leert die Flecken)')
reg['V1 Brust/Rumpf rot'] = (er(box & (LK1[..., 1] >= 15)), 'Rechteck x 295..465, y 160..250; a*(korr) >= 15; 2 px erodiert')
reg['V1 Oberarm (B) rechts = (W) links'] = (rect(452, 185, 482, 225), 'Rechteck x 452..482, y 185..225, 2 px erodiert')
reg['V1 Oberarm (B) links = (W) rechts'] = (rect(305, 180, 340, 230), 'Rechteck x 305..340, y 180..230, 2 px erodiert')
kl = (I1k[..., 0] - I1k[..., 1] > 18); kl &= rect(380, 330, 515, 365, 0)
reg['V1 Unterarm/Klaue (rot)'] = (kl, 'R-G > 18 (korr) in x 380..515, y 330..365 (Klauen beider Arme), nicht erodiert (Flecken 10-15 px)')
reg['V1 Jeans'] = (er(rect(365, 265, 430, 320, 0) & (LK1[..., 2] < -2)), 'Rechteck x 365..430, y 265..320 und b*(korr) < -2, 2 px erodiert')
reg['V1 Boden'] = (rect(40, 380, 250, 440), 'Rechteck x 40..250, y 380..440 (2 px erodiert)')
reg['V1 Wand links'] = (rect(20, 120, 180, 300), 'Rechteck x 20..180, y 120..300')
reg['V1 Wand rechts'] = (rect(520, 160, 630, 320), 'Rechteck x 520..630, y 160..320')
# ---------- Bild 2 seitlich
M = np.load(f'{S}/B_L3_masken.npz'); M1, M2, Mc = M['M1'], M['M2'], M['Mc']
reg['V2 Kopf seitlich: Auge-Umfeld'] = (C.fenster(*E2['Auge von der Seite etwa hier'], 9), 'Fenster 9x9 um %s' % E2['Auge von der Seite etwa hier'])
hb = np.load(f'{S}/B_seite_haarstreifen.npy')
reg['V2 Kopf seitlich: helle Flaeche vor dem Auge'] = (er(rect(440, 226, 458, 244, 0) & (L2_[..., 0] > 30)), 'Rechteck x 440..458, y 226..244 und L* > 30, 2 px erodiert')
reg['V2 Haare seitlich (dunkler Streifen)'] = (er(hb, 1), 'Abweichung vom Pfeiler-Hintergrund >= 12 L, x 455..468, y 229..249 (196 px), 1 px erodiert')
box2 = rect(330, 185, 440, 300, 0)
reg['V2 Buckel/Rumpf hell'] = (er(box2 & (L2_[..., 1] < 10) & (L2_[..., 0] >= np.percentile(L2_[..., 0][box2], 70)), 1), 'Rechteck x 330..440, y 185..300; a*<10, L* >= 70. Perzentil; 1 px erodiert')
reg['V2 Buckel/Rumpf rot'] = (er(box2 & (L2_[..., 1] >= 15)), 'Rechteck x 330..440, y 185..300; a* >= 15; 2 px erodiert')
reg['V2 Jeans'] = (er(rect(380, 280, 450, 340, 0) & (L2_[..., 2] < -2)), 'Rechteck x 380..450, y 280..340 und b* < -2, 2 px erodiert')
Me = er(M1 | M2, 2)
reg['V2 Schwanz nahe Ruecken'] = (Me & disc(*E2['Schwanz Beginn in den Ruecken ueberfuehrend.'], 15), 'Schwanzmaske (M1|M2) 2 px erodiert, Kreis r15 um Beginn (357,243)')
reg['V2 Schwanz breiter Teil'] = (Me & disc(308, 250, 12), 'Schwanzmaske 2 px erodiert, Kreis r12 um (308,250) (Achse f 0,5..0,7 Knick->Beginn)')
reg['V2 Schwanz am Knick'] = (er(Mc, 1) & disc(*E2['Schwanz Knick, von da an wird es breiter nach oben'], 8), 'Schwanzmaske (geschlossen) 1 px erodiert, Kreis r8 um Knick (290,308)')
reg['V2 Schwanz Spitze'] = (er(M2, 1) & disc(*E2['Schwanzspitze Ende'], 8), 'M2 1 px erodiert, Kreis r8 um Ende (294,344)')
reg['V2 Boden'] = (rect(40, 380, 250, 440), 'Rechteck x 40..250, y 380..440')
reg['V2 Wand links'] = (rect(20, 120, 180, 300), 'Rechteck x 20..180, y 120..300')
# ---------- Port V3 r2 vorn h135: Kopfbox wie Video gemessen (Oberkante R-G>=8), Merkmale an denselben Relativlagen
def oberkante(a, xs, y0, y1, thr=8, run=5):
    RG = a[..., 0] - a[..., 1]; out = {}
    for x in xs:
        col = RG[y0:y1, x] >= thr
        for yy in range(len(col) - run):
            if col[yy:yy + run].all(): out[x] = y0 + yy; break
    return out
# Port-Kopfbox von Hand aus der Lupe (SF_belege/v4_lesung/B_L4_port_kopf_lupe_x6.png): Oberkante y 155 (x 345..370), Seiten x 338/378
# (Schulter links (B) und rechts (B) beginnen dort), Unterkante y 213 (Unterkiefer unter dem offenen Maul). Die automatische Oberkante
# (R-G>=8) traf im Port den gelben Lampenrand (x 318..329, y 143) und ist daher nicht verwendet.
a0, a1, topp, ybp = 338, 378, 155, 213; hp = ybp - topp
kbp = dict(x0=a0, x1=a1, y0=topp, y1=ybp, breite_px=a1 - a0 + 1, hoehe_px=hp, B_durch_H=round((a1 - a0 + 1) / hp, 2), methode='von Hand aus der Lupe, siehe Kommentar')
def relp(name):
    r = LL['L5_vorn']['endpunkte_relativ'][name]
    return int(round(a0 + r['x_rel'] * kbp['breite_px'])), int(round(topp + r['y_rel'] * hp))
sc = kbp['breite_px'] / kb['breite_px']
w15 = max(3, int(round(15 * sc)) | 1); w9 = max(3, int(round(9 * sc)) | 1)
preg = {}
preg['V1 Kopf-Kuppe'] = (rect(a0 + 10, topp, a1 - 10, topp + 12), f'Port Kopfbox oben x {a0+10}..{a1-10}, y {topp}..{topp+12}')
preg['V1 Augenumfeld'] = (C.fenster(*relp('Auge'), w15), 'Port: Fenster %dx%d an Relativlage Auge %s' % (w15, w15, relp('Auge')))
preg['V1 Haare'] = (C.fenster(*relp('Haare'), w9), 'Port: Fenster %dx%d an Relativlage Haare %s' % (w9, w9, relp('Haare')))
preg['V1 Nase'] = (C.fenster(*relp('Nase'), w9), 'Port: Fenster %dx%d an Relativlage Nase %s' % (w9, w9, relp('Nase')))
yb0 = int(round(topp + (186 - kb['y0']) / kb['hoehe_px'] * hp)); yb1 = int(round(topp + (194 - kb['y0']) / kb['hoehe_px'] * hp))
xb0 = int(round(a0 + (378 - kb['x0']) / kb['breite_px'] * kbp['breite_px'])); xb1 = int(round(a0 + (398 - kb['x0']) / kb['breite_px'] * kbp['breite_px']))
preg['V1 Maul/Kinn dunkle Zone'] = (rect(xb0, yb0, xb1, yb1, 0), f'Port: Rechteck an Relativlage x {xb0}..{xb1}, y {yb0}..{yb1}')
pbox = rect(255, 160, 460, 330, 0)   # rechts bis x 460: dahinter Wand
preg['V1 Brust/Rumpf hell'] = (er(pbox & (LP1[..., 1] < 10) & (LP1[..., 0] >= np.percentile(LP1[..., 0][pbox], 70)), 1), 'Port: Rechteck x 255..460, y 160..330; a*<10, L* >= 70. Perzentil; 1 px erodiert')
preg['V1 Brust/Rumpf rot'] = (er(pbox & (LP1[..., 1] >= 15)), 'Port: Rechteck x 255..460, y 160..330; a* >= 15; 2 px erodiert')
preg['V1 Oberarm (B) rechts = (W) links'] = (rect(425, 200, 470, 250), 'Port: Rechteck x 425..470, y 200..250')
preg['V1 Oberarm (B) links = (W) rechts'] = (rect(268, 205, 310, 255), 'Port: Rechteck x 268..310, y 205..255')
pk = (P1[..., 0] - P1[..., 1] > 40) & (rect(255, 290, 300, 345, 0) | rect(455, 270, 505, 330, 0))
preg['V1 Unterarm/Klaue (rot)'] = (pk, 'Port: R-G > 40 in x 255..300, y 290..345 und x 455..505, y 270..330')
preg['V1 Jeans'] = (er(rect(300, 335, 405, 400, 0) & (LP1[..., 2] < -2)), 'Port: Rechteck x 300..405, y 335..400 und b* < -2, 2 px erodiert')
preg['V1 Boden'] = (rect(40, 380, 250, 440), 'Port: gleiches Rechteck (registriert)')
preg['V1 Wand links'] = (rect(20, 120, 180, 300), 'Port: gleiches Rechteck')
preg['V1 Wand rechts'] = (rect(520, 160, 630, 320), 'Port: gleiches Rechteck')
# ---------- Port Schwanz: Leiste 01_seite, Spalte V3 (x 2240..2799), Zeile h85 (y 774..1393); Magenta-Umriss fuellen, Linienfarben weg
Lst = np.asarray(Image.open(LEISTE).convert('RGB')).astype(float)[774:1394, 2240:2800]
gelb = (Lst[..., 0] > 115) & (Lst[..., 1] > 95) & (Lst[..., 2] < 75) & (Lst[..., 0] - Lst[..., 2] > 70)   # JPEG-Gelb gemessen ~(150,130,42)          # Mittellinie (gelb) der P_port-Messung
gelb[:, 300:] = False; gelb[:45] = False                                         # nur der Schwanzbogen links (Panel-x < 300), ohne Kopfzeile (y < 45)
lin = (Lst[..., 0] > 100) & (Lst[..., 2] > 90) & (Lst[..., 1] < 60) | gelb | ((Lst[..., 1] > 140) & (Lst[..., 0] < 130) & (Lst[..., 2] < 130))
lin = ndi.binary_dilation(lin, iterations=2)
tail_p = ndi.binary_dilation(gelb, iterations=7) & ~lin
ys, xs = np.nonzero(tail_p)
OUT = dict(port_kopfbox=kbp, port_fenster_massstab=round(sc, 3), port_schwanz_leiste=dict(px=int(tail_p.sum()), bbox_in_panel=[int(xs.min()), int(ys.min()), int(xs.max()), int(ys.max())] if len(xs) else None))
# Schwanz-Abschnitte im Port-Panel: entlang der Hoehe des Umrisses (oben = nahe Ruecken, unten = Spitze)
if len(ys):
    y0p, y1p = ys.min(), ys.max(); hh = y1p - y0p
    def band(f0, f1): m = tail_p.copy(); m[:int(y0p + f0 * hh)] = False; m[int(y0p + f1 * hh) + 1:] = False; return m
    ptail = {'V2 Schwanz nahe Ruecken': (band(0.0, 0.15), 'Port-Panel: Band +-7 px um die gelbe Mittellinie ohne Linienfarben, oberste 15 % der Hoehe'),
             'V2 Schwanz breiter Teil': (band(0.15, 0.45), 'Port-Panel: 15..45 %'),
             'V2 Schwanz am Knick': (band(0.45, 0.75), 'Port-Panel: 45..75 % (Port-Schwanz hat keinen Knick an der Videostelle; Abschnitt nach Lage)'),
             'V2 Schwanz Spitze': (band(0.88, 1.0), 'Port-Panel: unterste 12 %')}
# ---------- Auswertung
erg = {}
def st(img, m): return C.stats(img, m)
for name, (m, beschr) in reg.items():
    if name.startswith('V1'):
        d = dict(beschreibung=beschr, roh=st(I1, m), schwarz_ab=st(C.schwarz_ab(I1), m), abblende_korr=st(I1k, m), abblende_korr_schwarz_ab=st(C.schwarz_ab(I1k), m))
        if name in preg:
            pm, pb = preg[name]; d['port_V3r2_h135'] = dict(beschreibung=pb, roh=st(P1, pm))
            if d['port_V3r2_h135']['roh'] and d['abblende_korr']:
                d['dE00_port_vs_video_abblende_korr'] = C.de00(d['port_V3r2_h135']['roh']['lab'], d['abblende_korr']['lab'])
                d['dE00_port_vs_video_roh'] = C.de00(d['port_V3r2_h135']['roh']['lab'], d['roh']['lab'])
    else:
        d = dict(beschreibung=beschr, roh=st(I2, m), schwarz_ab=st(C.schwarz_ab(I2), m))
        if name in ('V2 Boden', 'V2 Wand links'):
            pm = reg[name][0]; P85 = cv2.cvtColor(c6.port('V3r2', 85), cv2.COLOR_BGR2RGB).astype(float)
            d['port_V3r2_h85'] = dict(beschreibung='Port Spielbild h85, gleiches Rechteck', roh=st(P85, pm))
            d['dE00_port_vs_video_roh'] = C.de00(d['port_V3r2_h85']['roh']['lab'], d['roh']['lab'])
        if len(ys) and name in ptail:
            pm, pb = ptail[name]; d['port_V3_leiste_h85'] = dict(beschreibung=pb, roh=st(Lst, pm))
            if d['port_V3_leiste_h85']['roh'] and d['roh']:
                d['dE00_port_vs_video_roh'] = C.de00(d['port_V3_leiste_h85']['roh']['lab'], d['roh']['lab'])
    erg[name] = d
OUT['regionen'] = erg
json.dump(OUT, open(f'{S}/B_L4.json', 'w'), indent=1, ensure_ascii=False)
for k, d in erg.items():
    r = d['roh']; kk = d.get('abblende_korr'); p = d.get('port_V3r2_h135') or d.get('port_V3_leiste_h85') or d.get('port_V3r2_h85')
    print(f"{k[:40]:40s} n={r['n'] if r else 0:5d} roh {r['rgb'] if r else '-'} Lab {r['lab'] if r else '-'}"
          + (f" | korr {kk['rgb']} Lab {kk['lab']}" if kk else '')
          + (f" | Port n={p['roh']['n']} {p['roh']['rgb']} Lab {p['roh']['lab']}" if p and p['roh'] else '')
          + (f" dE {d.get('dE00_port_vs_video_abblende_korr', d.get('dE00_port_vs_video_roh'))}" if p else ''))
print('Port-Kopfbox', kbp, 'sc', round(sc, 3), 'Schwanz-Panel', OUT['port_schwanz_leiste'])
# ---------- Belegbilder: Regionen eingefaerbt
def zeige(img, R, datei, box, sc_=3, titel=''):
    a = np.clip(img, 0, 255).astype(np.uint8).copy()
    cols = [(255, 0, 0), (0, 255, 0), (0, 128, 255), (255, 255, 0), (255, 0, 255), (0, 255, 255), (255, 128, 0), (128, 255, 128), (255, 128, 255), (128, 128, 255), (200, 200, 200), (255, 60, 120), (60, 255, 180), (180, 120, 60), (120, 60, 255)]
    for i, (nm, (m, _)) in enumerate(R.items()):
        rnd = m & ~er(m, 1); a[rnd] = cols[i % len(cols)]
    im = Image.fromarray(a).crop(box); im = im.resize((im.width * sc_, im.height * sc_), Image.NEAREST); d = ImageDraw.Draw(im)
    for i, nm in enumerate(R): d.text((4, 4 + 13 * i), nm, fill=cols[i % len(cols)])
    d.text((4, im.height - 14), titel, fill=(255, 255, 255)); im.save(datei)
zeige(np.clip(I1k * 1.8, 0, 255), {k: v for k, v in reg.items() if k.startswith('V1') and 'Wand' not in k and 'Boden' not in k}, f'{BEL}/B_L4_regionen_bild1_x3.png', (280, 120, 530, 380), 3, 'n1485 abblende-korr x1,8 (nur Anzeige); Raender der Regionen')
zeige(np.clip(I2 * 1.5, 0, 255), {k: v for k, v in reg.items() if k.startswith('V2') and 'Wand' not in k and 'Boden' not in k}, f'{BEL}/B_L4_regionen_bild2_x3.png', (260, 180, 480, 360), 3, 'n1435 x1,5 (nur Anzeige)')
zeige(P1, {k: v for k, v in preg.items() if 'Wand' not in k and 'Boden' not in k}, f'{BEL}/B_L4_regionen_port_V3r2_h135_x2.png', (240, 130, 520, 450), 2, 'Port V3r2 h135 (Spielbild, registriert)')
im = Image.fromarray(Lst.astype(np.uint8)).copy(); a = np.asarray(im).copy(); a[tail_p & ~er(tail_p, 1)] = (0, 255, 255); Image.fromarray(a).save(f'{BEL}/B_L4_port_schwanz_leiste_h85.png')
lu = Image.fromarray(np.clip(P1, 0, 255).astype(np.uint8)).crop((300, 135, 400, 230)).resize((600, 570), Image.NEAREST); dd = ImageDraw.Draw(lu)
for x in range(310, 400, 10): dd.line([((x - 300) * 6, 0), ((x - 300) * 6, 570)], fill=(0, 200, 200)); dd.text(((x - 300) * 6 + 2, 2), str(x), fill=(255, 255, 0))
for y in range(140, 230, 10): dd.line([(0, (y - 135) * 6), (600, (y - 135) * 6)], fill=(0, 200, 200)); dd.text((2, (y - 135) * 6 + 2), str(y), fill=(255, 255, 0))
dd.rectangle(((a0 - 300) * 6, (topp - 135) * 6, (a1 - 300 + 1) * 6, (ybp - 135 + 1) * 6), outline=(255, 0, 255), width=2)
lu.save(f'{BEL}/B_L4_port_kopf_lupe_x6.png')
