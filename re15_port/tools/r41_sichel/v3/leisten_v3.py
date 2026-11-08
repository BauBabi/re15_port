"""Spur SF "V3": Bildleisten fuer den Nutzer (analysis/befunde_runde41/SF_belege/v3/), Nachfolger von v2/leisten_v2.py.

python leisten_v3.py seite|vorn|gang|kopf|schwanz|rundum|farbe
Spalten: KI (_slp-Freisteller birkinNN) | Original (Video 4, Schwarzwert abgezogen) | Referenz 2 | V1 | V3.
Offline-Kacheln (seite, vorn, kopf, schwanz): Buehne in der Spielkamera ROOM50A0 Schnitt 12 bei der an das Original angepassten
Pose (sfv2/posen_*.json, anpassen_v2.py), dieselbe Pose fuer alle Modelle. Gang: RE15_FRAMEDUMP der Laeufe lauf_v3.sh
(sfv3/r/R2n = Spiel-Asset Referenz 2, V1s, V3). H = Oberkante bis Sohle im selben Bild. Zahlen = Riegel-Messung (riegel_v3.py,
P_port.py, P_kopf.auge) bzw. Messblatt V3 fuer KI/Original.
"""
import sys, os, json
import numpy as np
import cv2
HIER = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HIER); sys.path.insert(0, os.path.join(os.path.dirname(HIER), 'v2'))
sys.path.insert(0, os.path.join(os.path.dirname(HIER), 'nb2neu'))
import b2
import leisten_v2 as LV
import birkin_buehne as BB
import emd_ansichtsblatt as EA
import birkin_formen as BF
import P_port
import riegel_v3 as RV
from P_farbe import lab, de00, ch, r1

SCR = 'C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad'
EMD = {'Ref2': os.path.join(b2.BAUM, 're15_port', 'shared_assets', 'PSX', 'EMD', 'EM30_HANGAR.EMD'),
       'V1': os.path.join(SCR, 'sfv2', 'emd', 'EM30_V1_SPITZ.EMD'),
       'V3': os.path.join(SCR, 'sfv3', 'emd', 'EM30_V3.EMD')}
LAEUFE = {'Ref2': os.path.join(SCR, 'sfv3', 'r', 'R2n'), 'V1': os.path.join(SCR, 'sfv3', 'r', 'V1s'),
          'V3': os.path.join(SCR, 'sfv3', 'r', 'V3r2')}   # Runde 2: Lauf mit der V3-EMD Runde 2 (lauf_v3.sh)
AUS = os.path.join(b2.BELEGE, 'v3')
MOD = ('Ref2', 'V1', 'V3')
_mo = {}


def modell(k):
    if k not in _mo:
        _mo[k] = b2.modell(EMD[k])
    return _mo[k]


LV.modell = modell
x2, kachel, linie = LV.x2, LV.kachel, LV.linie
GRUEN, GELB, MAG, CYAN, ORANGE = LV.GRUEN, LV.GELB, LV.MAG, LV.CYAN, LV.ORANGE


def posen(art):
    return json.load(open(os.path.join(SCR, 'sfv2', 'posen_%s.json' % art)))


def leer(text='kein KI-Bild fuer dieses h'):
    im = np.full((960, 1280, 3), 30, np.uint8)
    b2.schrift(im, text, 520, 480, (200, 200, 200), 0.8)
    return im


# ═══════════════════════════════════════════════════ 01 Seite
MB_KI = {83: 'KI h83: Dicke 0,2/0,5/0,9 = 0,195/0,100/0,039 H; max 28,4 Grad, 4 Knicke (H 265: 3); L/Haut 0,61, C 0,60',
         85: 'KI h85: Dicke 0,179/0,088/0,037 H; max 28,3 Grad, 2 Knicke; L/Haut 0,63, C 0,64, dE00 8,5',
         92: 'KI h92: Dicke 0,163/0,079/0,030 H; max 32,9 Grad, 2 Knicke; L/Haut 0,69, C 0,83, dE00 6,0'}
MB_O = {83: 'Original n1433: L/Haut 0,48, C 0,43, dE00 11,5 (Messblatt 2)', 85: 'Original n1435: L/Haut 0,52, C 0,46, dE00 10,3',
        92: 'Original n1442: L/Haut 0,57, C 0,66, dE00 7,5'}


def schwanz_text(d):
    f, c, m = d['form'], d['farbe'], d['muster']
    return ['Dicke 0,2/0,5/0,9: %.3f/%.3f/%.3f H; Austritt %.3f H unter OK' % (f['dicke_H'][0.2], f['dicke_H'][0.5], f['dicke_H'][0.9],
                                                                            d['austritt_unter_OK_H']),
            'max %.1f Grad je 0,05 H, %d Knicke > 20 Grad, Drehung %.0f Grad' % (f['max_dwinkel_005H'], f['knicke'], f['gesamtdrehung']),
            'L/Haut %.2f, C/Haut %.2f, dh %+.1f, Rot %.2f/%.2f, dE00 %.1f, Rest-Std a* %.2f' % (
                c['L_verh'], c['C_verh'], c['dh'], c['rot_anteil_schwanz'], c['rot_anteil_haut'], c['de00_haut'], m['rest_std_a'])]


def leiste_seite():
    P = posen('seite')
    zeilen, tab = [], {}
    box = (440, 300, 1000, 920)
    for h in (83, 85, 92):
        pose = P[str(h)]
        r = []
        im, z = LV.seite_original(h, True); r.append(kachel(im, box, 'KI birkin%s h%d' % (b2.KI_VON_H[h], h), [MB_KI[h]]))
        im, z = LV.seite_original(h, False); r.append(kachel(im, box, 'Original h%d x2 (Schwarzwert ab)' % h, [MB_O[h]]))
        for k in MOD:
            im, _ids = b2.zeichne(modell(k), pose, b2.video_sw(h))
            d, vis = P_port.messen(EMD[k], h, pose)
            tab['%s h%d' % (k, h)] = d
            t = x2(np.ascontiguousarray(vis))
            # Messbild (Maske magenta, Mittellinie gelb, Dicken gruen) halb ueber das Spielbild
            im2 = cv2.addWeighted(x2(im), 0.55, t, 0.45, 0)
            r.append(kachel(np.ascontiguousarray(im2), box, '%s h%d (Buehne + P_port-Messung)' % (k, h), schwanz_text(d)))
        zeilen.append(r)
    kopf = ['SF V3 - Seitenansicht h83/h85/h92: KI | Original | Referenz 2 | V1 | V3 (Schwanz)',
            'Modelle: Buehne Spielkamera 50A0/12, Pose an das Original angepasst (gleich fuer alle). Messung P_port.py (Riegel R15-R27): '
            'Maske magenta, Mittellinie gelb, Dicken gruen. H = Oberkante bis Sohle (265 px).',
            'Vorgabe: Dicke 0,15-0,20 / 0,07-0,10 / 0,028-0,045 H; <= 36 Grad, <= 3 Knicke; L/Haut 0,50-0,72, C/Haut 0,45-0,90, '
            'Rot <= 0,8 x Haut, dE00 6-11, Rest-Std a* <= 2,0.']
    b2.leiste(zeilen, kopf, os.path.join(AUS, '01_seite_h83_h85_h92.jpg'))
    return tab


# ═══════════════════════════════════════════════════ 02 Vorn
def leiste_vorn():
    P = posen('vorn')
    zeilen, tab = [], {}
    box = (500, 260, 1100, 940)
    for h in (118, 125):
        pose = P[str(h)]
        r = []
        im, z = LV.vorn_original(h, True); r.append(kachel(im, box, 'KI birkin%s h%d' % (b2.KI_VON_H[h], h),
                                                            ['KI: ein dunkles Auge oben, Maul waagerecht, Schulterauge B-links']))
        im, z = LV.vorn_original(h, False); r.append(kachel(im, box, 'Original h%d x2' % h, ['Original: Schwanz hinter dem Koerper']))
        for k in MOD:
            im, ids = b2.zeichne(modell(k), pose, b2.video_sw(h))
            sicht = int(((ids >= 16) & (ids <= 22)).sum())
            tab['%s h%d' % (k, h)] = dict(schwanz_sichtbar=sicht)
            r.append(kachel(np.ascontiguousarray(x2(im)), box, '%s h%d (Buehne)' % (k, h),
                            ['Schwanz sichtbar: %d Bildpunkte (640er)' % sicht,
                             'Koerper = Referenz 2' if k != 'Ref2' else 'Referenz 2 (gelobter Stand)']))
        zeilen.append(r)
    kopf = ['SF V3 - Vorderansicht h118/h125: KI | Original | Referenz 2 | V1 | V3',
            'Buehne bei der an das Original angepassten Pose. Vorgabe (Formblatt V2 S15): Schwanz von vorn hinter dem Koerper.']
    b2.leiste(zeilen, kopf, os.path.join(AUS, '02_vorn_h118_h125.jpg'))
    return tab


# ═══════════════════════════════════════════════════ 04 Gang (Spiellauf)
def leiste_gang():
    import c6
    B = {k: RV.bewegung(os.path.join(LAEUFE[k], 'debug.log'))['je_takt'] for k in MOD}
    hs = list(range(84, 133, 6))
    zeilen = []
    box = (300, 140, 1060, 900)
    for h in hs:
        r = []
        k_ = b2.ki(h)[0]
        r.append(kachel(k_ if k_ is not None else leer(), box, 'KI h%d' % h, ['']))
        r.append(kachel(x2(b2.video_sw(h)), box, 'Original h%d x2' % h, ['']))
        for k in MOD:
            im = c6.port(LAEUFE[k], h)
            im = np.zeros((480, 640, 3), np.uint8) if im is None else cv2.cvtColor(im, cv2.COLOR_BGR2RGB)
            w = B[k].get(h)
            z = ['Bild-Gelenkwinkel max %.0f Grad, %d > 20' % (max(w['bild']), sum(1 for x in w['bild'] if x > 20)) if w else '-',
                 'Raum: %s Grad' % ' '.join('%.0f' % x for x in w['raum']) if w else '']
            r.append(kachel(x2(im), box, '%s h%d (Spiellauf)' % (k, h), z))
        zeilen.append(r)
    zus = {k: RV.bewegung(os.path.join(LAEUFE[k], 'debug.log')) for k in MOD}
    kopf = ['SF V3 - Gang h84-h132: KI | Original | Referenz 2 | V1 | V3 (Spiellaeufe lauf_v3.sh, Feder-Kette schwanz_r41.c)',
            'Knickpruefung in Bewegung: Schwanzgelenke je Takt (debug.log) in die Spielkamera projiziert; Gelenkwinkel im Bild '
            '(Vorgabe <= 36 Grad, <= 3 ueber 20 Grad) und im Raum.',
            'Gang h84-h132 max: ' + ' | '.join('%s %.0f Grad / %d ueber 20' % (k, zus[k]['gang_h84_h132']['max_bild_grad'],
                                                                          zus[k]['gang_h84_h132']['max_knicke_je_takt']) for k in MOD)]
    b2.leiste(zeilen, kopf, os.path.join(AUS, '04_gang_h84-h132.jpg'))
    return {k: {kk: v for kk, v in zus[k].items() if kk != 'je_takt'} for k in MOD}


# ═══════════════════════════════════════════════════ 05 Kopf nah (x4)
def leiste_kopf():
    Pv, Ps = posen('vorn'), posen('seite')
    zeilen = []
    for h, P, art in ((118, Pv, 'vorn'), (125, Pv, 'vorn'), (85, Ps, 'Seite')):
        pose = P[str(h)]
        k, _m = b2.ki(h)
        box = (650, 270, 890, 510) if art == 'vorn' else (740, 350, 980, 590)
        f = 2
        def zu(im):
            c = im[box[1]:box[3], box[0]:box[2]]
            return cv2.resize(c, None, fx=f, fy=f, interpolation=cv2.INTER_NEAREST)
        r = [kachel(zu(k), (0, 0, 480, 480), 'KI birkin%s h%d (%s) x4' % (b2.KI_VON_H[h], h, art),
                    ['KI (Messblatt K3-K14): Auge 0,019-0,022 x 0,009-0,012 H, Maul 0,084-0,092 x 0,042-0,048 H']),
             kachel(zu(x2(b2.video_sw(h))), (0, 0, 480, 480), 'Original h%d x4' % h, ['Original: helle Kuppe, rote Zone; Details nicht aufgeloest'])]
        for key in MOD:
            im, _ids = b2.zeichne(modell(key), pose, b2.video_sw(h))
            r.append(kachel(zu(x2(im)), (0, 0, 480, 480), '%s h%d (%s, Spielkamera) x4' % (key, h, art), [{'Ref2': 'Referenz 2: Wundfleisch, kein Gesicht',
                     'V1': 'V1: Ringaugen mit Kreuz, senkrechtes Maul (abgelehnt)', 'V3': 'V3 r2: Kopfbild + Seitenbild (Mesh 9), Lippen/Stummel, 1 Viereck'}[key]]))
        zeilen.append(r)
    # Messansicht (Riegel R1-R14) als eigene Zeile
    r = []
    kv = cv2.cvtColor(cv2.imread(os.path.join(b2.V80, 'birkin19.png')), cv2.COLOR_BGR2RGB)
    r.append(kachel(cv2.resize(kv[0:130, 90:260], (480, 367), interpolation=cv2.INTER_NEAREST), (0, 0, 480, 367),
                    'KI birkin19 (Freisteller) Kopf x2,8', ['Auge B/H 2,2, L/Haut 0,43; Maul B/H 2,0']))
    r.append(kachel(np.full((367, 480, 3), 30, np.uint8), (0, 0, 480, 367), 'Original', ['nicht aufgeloest']))
    for key in MOD:
        mo = BF.modell_aus_blob(open(EMD[key], 'rb').read())
        b, alp, _r = RV.nah_bild(mo)
        k_ = RV.kopf(EMD[key])
        ka, mu = k_.get('kopfauge'), k_.get('maul')
        c = cv2.resize(b[230:890, 380:960], (480, 546), interpolation=cv2.INTER_AREA)
        z = ['Auge %.4f x %.4f H, B/H %.2f, L/Haut %.2f, Lid hell %.2f' % (ka['b_H'], ka['h_H'], ka['bh'], ka['L_auge_haut'], ka['lid_hell']) if ka else '-',
             'Maul %.4f x %.4f H, B/H %.2f, Zaehne %d/%d' % (mu['b_H'], mu['h_H'], mu['bh'], mu['oben'], mu['unten']) if mu else '-']
        r.append(kachel(c, (0, 0, 480, 546), '%s Messansicht Kopf vorn nah (Riegel)' % key, z))
    zeilen.append(r)
    b2.leiste(zeilen, ['SF V3 - Kopf nah: KI | Original | Referenz 2 | V1 | V3 (Zeilen 1-3: Spielkamera 50A0/12 x4, h118/h125 vorn, h85 Seite)',
                       'Zeile 4: Messansicht der Riegel R1-R14 (0,9 px/E., H 3479 px, neutrales Licht). Masse in H (Oberkante Buckel bis Sohle).'],
              os.path.join(AUS, '05_kopf_nah.jpg'))


# ═══════════════════════════════════════════════════ 07 Schwanz nah (Seite)
def leiste_schwanz():
    P = posen('seite')
    pose = P['85']
    r = []
    k, _m = b2.ki(85)
    box = (420, 330, 900, 810)
    r.append(kachel(k, box, 'KI birkin12 h85 (Seite)', ['KI: Hautfamilie, L/Haut 0,63, C/Haut 0,64, glatter Bogen, max 28 Grad']))
    for key in ('V1', 'V3'):
        im, _ids = b2.zeichne(modell(key), pose, b2.video_sw(85))
        d, _v = P_port.messen(EMD[key], 85, pose)
        r.append(kachel(np.ascontiguousarray(x2(im)), box, '%s h85 (Spielkamera)' % key, schwanz_text(d)[1:]))
    b2.leiste([r], ['SF V3 - Schwanz nah, Seite h85: KI birkin12 | V1 | V3 (Spielkamera 50A0/12, angepasste Pose, x2)',
                    'V1: lachsrot, Ringe, Haken mit Knicken. V3: Haut der Figur (Streifen A, Zeile 482), glatter Bogen.'],
              os.path.join(AUS, '07_schwanz_nah.jpg'))


# ═══════════════════════════════════════════════════ 08 Rundum V3
def leiste_rundum():
    mo = modell('V3')
    tafeln = []
    kf = BB.kf_von_clip(mo, 1, 0)
    V, N, F = mo.geometrie(kf)
    mitte = (V.min(0) + V.max(0)) / 2
    ausd = (V.max(0) - V.min(0)).max()
    for grad, name in ((0, 'vorn'), (60, ''), (120, 'hinten links'), (180, 'hinten'), (240, ''), (270, 'rechte Flanke')):
        a = np.radians(grad)
        depth = np.array([np.cos(a), 0.0, np.sin(a)])
        right = np.array([-np.sin(a), 0.0, np.cos(a)])
        col, alp = EA.render(mo, V - mitte, N, F, right, np.array([0.0, 1.0, 0.0]), depth, 480, 480, 480 * 0.92 / ausd, 240, 240)
        b = (np.clip(col * 1.35, 0, 1) * 255).astype(np.uint8); b[alp == 0] = (92, 96, 104)
        tafeln.append(kachel(b, (0, 0, 480, 480), 'V3 %d Grad %s' % (grad, name), ['']))
    R, T = mo.pose(kf)
    for titel, mt, right, depth, sk in (
            ('Kopf vorn nah', T[9].astype(float) + np.array([150.0, -150, 0]), np.array([0, 0, 1.0]), np.array([1.0, 0, 0]), 0.45),
            ('Schwanz rechte Flanke nah', b2.kette_punkte(mo, kf).mean(0), np.array([1.0, 0, 0]), np.array([0, 0, -1.0]), 0.17)):
        col, alp = EA.render(mo, V - mt, N, F, right, np.array([0.0, 1.0, 0.0]), depth, 480, 480, sk, 240, 240)
        b = (np.clip(col * 1.35, 0, 1) * 255).astype(np.uint8); b[alp == 0] = (92, 96, 104)
        tafeln.append(kachel(b, (0, 0, 480, 480), 'V3 %s' % titel, ['']))
    b2.leiste([tafeln[:4], tafeln[4:]], ['SF V3 - Rundum V3 (Clip 1 Bild 0, neutrales Licht), dazu Kopf und Schwanz nah'],
              os.path.join(AUS, '08_rundum_V3.jpg'))


# ═══════════════════════════════════════════════════ 09 Farbe
def feld(rgb, titel, zeilen, w=240, h=150):
    t = np.zeros((h, w, 3), np.uint8); t[:] = np.clip(np.round(rgb), 0, 255).astype(np.uint8)
    return kachel(t, (0, 0, w, h), titel, zeilen)


def leiste_farbe():
    P = posen('seite')
    Pk = json.load(open(os.path.join(b2.BELEGE, 'v3_lesung', 'P_schwanz_ki.json')))
    rk = Pk['birkin12']['farbe']
    ki_s, ki_h = np.array(rk['lab_schwanz']), np.array(rk['lab_haut'])
    werte = {}
    for k in MOD:
        d, _v = P_port.messen(EMD[k], 85, P['85'])
        werte[k] = d['farbe']
    r = [feld(rk['rgb_haut'], 'KI-Haut birkin12', ['Lab %s' % r1(ki_h)]),
         feld(rk['rgb_schwanz'], 'KI-Schwanz birkin12', ['Lab %s, dE00 zur KI-Haut %.1f' % (r1(ki_s), de00(ki_s, ki_h))]),
         feld(werte['V3']['rgb_haut'], 'Port-Haut (h85)', ['Lab %s' % werte['V3']['lab_haut']])]
    for k in ('Ref2', 'V1', 'V3'):
        s = np.array(werte[k]['lab_schwanz']); hh = np.array(werte[k]['lab_haut'])
        r.append(feld(werte[k]['rgb_schwanz'], '%s-Schwanz (h85)' % k, ['Lab %s; dE00 Haut %.1f' % (r1(s), de00(s, hh)),
                                                               'L %.2f C %.2f Rot %.2f; dE00 zum KI-Schwanz %.1f' % (
                                                                   werte[k]['L_verh'], werte[k]['C_verh'], werte[k]['rot_anteil_schwanz'], de00(s, ki_s))]))
    # Augen: KI (Messblatt K6/K7/K11) gegen V3 (Riegel-Messung)
    k3 = RV.kopf(EMD['V3'])
    ka = k3['kopfauge']
    r2 = [feld((66, 53, 59), 'KI Auge gesamt h118', ['Lab [22,5 6,5 -1,3] (K7)']), feld((18, 6, 11), 'KI Pupille h118', ['Lab [2,6 4,7 -0,6] (K6)']),
          feld((183, 171, 178), 'KI heller Kern h118', ['Lab [71,1 5,7 -2,2] (K11)']),
          feld(ka['rgb_auge'], 'V3 Auge gesamt (Messansicht)', ['Lab %s; L/Haut %.2f' % (ka['lab_auge'], ka['L_auge_haut'])]),
          feld(ka['rgb_pupille'], 'V3 Pupille', ['Lab %s; L/Haut %.2f' % (ka['lab_pupille'], ka['L_pup_haut'])]),
          feld(ka.get('rgb_glanz', (0, 0, 0)), 'V3 heller Kern', ['Lab %s' % ka.get('lab_glanz')])]
    b2.leiste([r, r2], ['SF V3 - Farbfelder (Lab D65, dE = CIEDE2000, Flaechen 2 px erodiert): Haut und Schwanz KI | Port; Augen KI | V3',
                        'KI-Werte birkin12 (P_schwanz_ki.json) und Messblatt K6/K7/K11; Port-Werte P_port.py h85 (Spielkamera, RDT-Licht) '
                        'und riegel_v3 Messansicht.'], os.path.join(AUS, '09_farbe.jpg'))
    return {k: werte[k] for k in MOD}


def main():
    os.makedirs(AUS, exist_ok=True)
    was = sys.argv[1:] or ['seite', 'vorn', 'gang', 'kopf', 'schwanz', 'rundum', 'farbe']
    erg = {}
    for w in was:
        r = globals()['leiste_' + w]()
        if r:
            erg[w] = r
        print('fertig', w, flush=True)
    if erg:
        json.dump(erg, open(os.path.join(AUS, 'masse_%s.json' % '_'.join(was)), 'w'), indent=1, default=str)


if __name__ == '__main__':
    main()
