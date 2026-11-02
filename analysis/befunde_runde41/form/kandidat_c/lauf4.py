# -*- coding: utf-8 -*-
"""Generator Kandidat C, Endfassung. Aufruf: python lauf4.py TAG [--neu]
Geometrie teile.py, Knochen und Posen bau.py, Haut maler.py, Toene und Marken hier, Nachregeln im geschlossenen Kreis gegen beide Bilder."""
from bau import *
from maler import *
import form_video as FV
import shutil
from scipy.spatial import cKDTree

tag = sys.argv[1] if len(sys.argv) > 1 else 'x'
if '--neu' in sys.argv or not os.path.exists('teile.pkl'):
    T = alle_teile()
    for t in T:
        t.pop('_T', None)
        t.pop('_NB', None)
    pickle.dump(T, open('teile.pkl', 'wb'))
else:
    T = pickle.load(open('teile.pkl', 'rb'))
kn = knochen_tafel(T)
pz1, pz2, pzr = posen_definieren(kn)
tr1, tr2 = pz1.trafo(T), pz2.trafo(T)


def sicht_gewichte(t, ti):
    V = t['V']
    Vn = eck_normalen(t)
    R = np.array(t['ringe'])
    sp = np.zeros(R.shape[1])
    ze = np.zeros(R.shape[0])
    for s, tr in ((S1, tr1), (S2, tr2)):
        Q = tr(ti, V)
        Nq = tr(ti, V + Vn * 100.0) - Q
        Nq /= 100.0
        r = s.auge_aktor() - Q
        r /= np.linalg.norm(r, axis=1, keepdims=True)
        c = (Nq * r).sum(1) > 0.1
        sp = np.maximum(sp, c[R].mean(0))
        ze = np.maximum(ze, c[R].mean(1))
    ze = np.concatenate([[ze[0]], 0.5 * (ze[:-1] + ze[1:]), [ze[-1]]])
    return 0.4 + 0.6 * sp, 0.55 + 0.45 * ze


karten = []
nach_name = {}
for ti, t in enumerate(T):
    if t['name'].startswith('fl_'):
        karten.append(None)
        continue
    g = sicht_gewichte(t, ti)
    k = karte_bauen(t, 1.0, g[0], g[1])
    karten.append(k)
    nach_name[t['name']] = k
for i, t in enumerate(T):
    if karten[i] is None:
        karten[i] = nach_name['fr_' + t['name'][3:]]
print('1 Texel =', round(packen(karten), 1), 'Einheiten')
M = Maler(T, karten, {1: tr1, 2: tr2})
v1 = M.sicht(1, S1)
v2 = M.sicht(2, S2)
U1 = np.zeros((480, 640), np.uint8)
U2 = np.zeros((480, 640), np.uint8)
for ti, t in enumerate(T):
    raster(t['V'], t['F'], S1, U1)
    raster(tr2(ti, t['V']), t['F'], S2, U2)
E1 = vorlage_bild(1, U1)
e1 = lesen(E1, v1)
yt0 = (e1 @ YK.astype(np.float32)) / float(F1 @ YK)
E2 = vorlage_bild(2, U2)
e2 = lesen(E2, v2)
yt2 = (0.5 * e2[..., 1] / F2[1] + 0.5 * e2[..., 2] / F2[2])
si2 = ZIEL[2][v2['yi'], v2['xi']] == 255                               # nur wo Bild 2 sicher Figur zeigt: hinter dem Feuer sagt es nichts
si1 = ZIEL[1][v1['yi'], v1['xi']] == 255
beide = v1['vis'] & v2['vis'] & (v2['nv'] > 0.15) & si2
yt0 = np.where(beide, np.minimum(yt0, yt2 + 12.0), yt0)
# Quer-Sperren: Marke nur dort, wo das andere Bild nicht sicher dunkel zeigt
dunkel_in_2 = v2['vis'] & (v2['nv'] > 0.10) & si2 & (yt2 < 34.0)
dunkel_in_1 = v1['vis'] & (v1['nv'] > 0.05) & (ZIEL[1][v1['yi'], v1['xi']] > 0) & (yt0 < 60.0)
im_rumpf = M.ID == 0
haut = M.knoten(M.adern(M.grundhaut()))
rausch = M.R1(M.P.astype(float), 130.0)
_yh = haut @ YK.astype(np.float32)
HAUT_MOD = np.clip(1.0 + 0.012 * (_yh - 21.0), 0.86, 1.10).astype(np.float32)   # Risse und Knoten scheinen durch die Marken
MT = FB['zeichnung']['materialien']
FARBE = {'rosa_stelle': np.array([190.0, 126.0, 152.0]), 'helle_baender': BAND, 'feine_linien': LINIE, 'z9': np.array([85.0, 80.0, 60.0]), 'rotbraun_mittelton': np.array([96.0, 58.0, 78.0]),
         'untere_streifen': np.array([180.0, 165.0, 180.0]), 'weisskerne': np.array([222.0, 228.0, 232.0])}
GRENZE = {'rosa_stelle': MT['rosa_stelle'], 'helle_baender': MT['helle_baender'], 'feine_linien': MT['feine_linien'], 'rotbraun_mittelton': MT['rotbraun_mittelton']}
HALO = np.array([82.0, 46.0, 66.0])
hk = M.ID == [i for i, t in enumerate(T) if t['name'] == 'vorsprung'][0]
xi, yi = v1['xi'], v1['yi']
in_n2 = (xi >= 380) & (xi <= 420) & (yi >= 300) & (yi <= 358)          # FORMBLATT 2.3: Bein FR unter Z5 ohne Band
STELLEN = PR.stellen()
offen = np.ones((5, 5), np.uint8)


def weg_duenn(m):
    """Einzelne Texelstriche aus der Sichtgrenze entfernen."""
    return cv2.morphologyEx(m.astype(np.uint8), cv2.MORPH_OPEN, cv2.getStructuringElement(cv2.MORPH_ELLIPSE, (3, 3))).astype(bool)


ZUG = np.array(FB['zeichnung']['zug_q']['linie'], float)
_zm = np.zeros((480, 640), np.uint8)
cv2.polylines(_zm, [np.round(ZUG * 4).astype(np.int32)], False, 1, 3, lineType=cv2.LINE_8, shift=2)
ZUG_M = _zm.astype(bool)
_st = FB['zeichnung']['zug_q']
ZUG_M &= ~PR.rechteck(_st['stoerung_x'][0] - 3, _st['stoerung_y'][0], _st['stoerung_x'][1] + 3, _st['stoerung_y'][1])


def sichel_zellen():
    """Scheitelsichel: jede zweite bis dritte Hautzelle dunkler, Risse bleiben. So bleibt die Flaeche Haut, keine Platte."""
    g = np.random.default_rng(17)
    h = g.uniform(size=int(M.zelle.max()) + 2)
    z = np.maximum(M.zelle, 0)
    return (h[z] < 0.72) & ~M.rissmaske


STREIF = sichel_zellen()
GLATT5 = cv2.getStructuringElement(cv2.MORPH_ELLIPSE, (5, 5))


def malen(yt, gewinn):
    ym = cv2.medianBlur((yt + 4.0 * rausch).astype(np.float32), 5)      # keine Einzelpixel: Flecken bleiben gemalte Flaechen
    farbe, stufe = leiter(ym)
    lim = v1['vis'] & (v1['nv'] > 0.22) & ~in_n2
    mod = np.ones(ym.shape, np.float32)
    mod[lim & (ym >= 30.0)] = 1.3                                      # Stufen, die Risse, Adern und Knoten der Haut behalten
    mod[lim & (ym >= 38.0)] = 1.6
    mod[lim & (ym >= 45.0)] = 1.95
    rgb = haut * mod[..., None]
    w = lim & (stufe >= 4)
    wu = cv2.morphologyEx(w.astype(np.uint8), cv2.MORPH_OPEN, GLATT5)
    n_, lab, st, _ = cv2.connectedComponentsWithStats(wu, connectivity=4)
    gross = np.zeros(n_, bool)
    gross[1:] = st[1:, 4] >= 60
    wu = gross[lab]
    rgb[wu] = farbe[wu] * np.clip(0.8 + 0.2 * HAUT_MOD[wu, None] / HAUT_MOD[wu, None].mean(), 0.8, 1.1)
    sei2 = hk & v2['vis'] & (v2['nv'] >= v1['nv'])
    rgb[sei2] = np.clip(ROT * gewinn.get('H', np.ones(3)), np.array(MT['rotbraun_mittelton']['oberflaeche_von'], float) * 0.75, np.array(MT['rotbraun_mittelton']['oberflaeche_bis'], float))
    rp = PR.poly_maske(FB2['H']['polygon'])
    rand2 = hk & v2['vis'] & rp[v2['yi'], v2['xi']]
    rgb = wulst(rgb, rand2, BAND, grenzen=(np.array(MT['helle_baender']['oberflaeche_von'], float), np.array(MT['helle_baender']['oberflaeche_bis'], float)))
    # Zug Q: Unterkante Q2, Q3, Q5, P als eine Linie (FORMBLATT 2.3), nur die Stoerung y 242-254 bleibt frei
    zm = v2['vis'] & ZUG_M[v2['yi'], v2['xi']] & (v2['nv'] > 0.12) & ~dunkel_in_1
    grund = LINIE * gewinn.get('Q3', np.ones(3))
    gr = (np.array(MT['feine_linien']['oberflaeche_von'], float), np.array(MT['feine_linien']['oberflaeche_bis'], float))
    rgb[zm] = np.clip(grund * 0.9, gr[0], gr[1])
    for s in STELLEN:
        sv = v1 if s['bild'] == 1 else v2
        m = np.zeros((480, 640), bool)
        for q in s['polygone']:
            m |= PR.poly_maske(q)
        tr = sv['vis'] & m[sv['yi'], sv['xi']]
        if s['bild'] == 1:
            tr &= ~(dunkel_in_2 & im_rumpf)
            tr &= ~(v2['vis'] & (v2['nv'] > v1['nv']) & (si2 | hk) & (im_rumpf | hk))
        elif s['id'] == 'Q1':                                          # B01, B02 fallen in Bild 1 auf A36, A35 (Punktziele teile.py): keine Sperre, aber schmaler, sonst Streifen in Bild 1
            tr = sv['vis'] & cv2.erode(m.astype(np.uint8), GLATT5).astype(bool)[sv['yi'], sv['xi']]
        elif s['id'] != 'P':
            tr &= ~dunkel_in_1
        if s['bild'] == 2 and s['id'] in ('Q1', 'Q2', 'Q3', 'Q4', 'Q5', 'P'):    # Rumpfmarken: nicht auf die Beine, die in Pose 2 davor stehen koennen
            tr &= im_rumpf
        if s['bild'] == 2 and s['id'] in ('Q1', 'Q3', 'Q4', 'Q5'):
            tr &= sv['nv'] > (0.05 if s['id'] == 'Q1' else 0.14)       # streifende Texel wuerden die Linie auf der Haut breit ziehen
        tr = weg_duenn(tr) | (tr & (s['id'] in ('Q3', 'Q4', 'Q5', 'Q1')))
        mat = s['material']
        gr = None
        if mat in GRENZE and s.get('zaehlt', True):
            gr = (np.array(GRENZE[mat]['oberflaeche_von'], float), np.array(GRENZE[mat]['oberflaeche_bis'], float))
        grund = FARBE[mat] * gewinn.get(s['id'], np.ones(3))
        if s['id'] == 'Q2':                                            # Scheitelsichel: gestreift aus der Haut, keine flache Platte
            rgb[tr & ~STREIF] = (58, 32, 44)
            rgb[tr & STREIF] = np.clip(grund, gr[0], gr[1])
            rgb[tr & M.rissmaske] = (24, 11, 16)
            continue
        rgb = wulst(rgb, tr, grund, halo=None if mat in ('rotbraun_mittelton', 'z9') else HALO, grenzen=gr, mod=HAUT_MOD)
    return rgb


# Unsichtbare Seite: Spiegelbild der sichtbaren an der Mittelebene Z = zm, ueber alle Teile (FORMBLATT Abschnitt 5, Vorschlag). Das ist frei gesetzt.
ZM = 60.0
sicht = v1['vis'] | v2['vis']
vs = np.nonzero(sicht.ravel())[0]
hid = np.nonzero((M.belegt & ~sicht).ravel())[0]
Pf = M.P.reshape(-1, 3).astype(float)
Q = Pf[hid].copy()
Q[:, 2] = 2 * ZM - Q[:, 2]
baum = cKDTree(Pf[vs])
d_sp, i_sp = baum.query(Q, k=1, distance_upper_bound=210.0)
treffer = np.isfinite(d_sp)
print('Spiegel: verdeckt', len(hid), 'Treffer', int(treffer.sum()))


def verdeckte_seite(rgb):
    aus = rgb.reshape(-1, 3).copy()
    c = aus[vs[i_sp[treffer]]]
    hell = (c @ YK.astype(np.float32) > 42) & ~((c[:, 0] > c[:, 1] + 28) & (c[:, 0] > 150))      # Rosa nur vorn
    z = hid[treffer][hell]
    aus[z] = c[hell] * 0.85
    return aus.reshape(rgb.shape)


form = form_bauen(T, karten, kn, [('pose1', pz1), ('pose2', pz2), ('ruhe', pzr)])
FE.boden_nachfuehren(form, 0)
lage_json()
orig = {n: BILD[n].astype(np.float32) for n in (1, 2)}
gewinn = {}
# Letzte Pixel auf dem freien Boden: gemessen am echten Raster (ganzzahlige Port-Ecken), nicht am Schnellraster
kern_b = np.zeros((480, 640), bool)
for n_ in ('L1', 'L2', 'SCH'):
    kern_b |= poly(next(q for q in FB['nicht_figur_bild1'] if q['id'] == n_)['polygon']) & (ZIEL[1] == 0)
_nr = next(q for q in T if q['name'] == 'nr_ober')
for vi, ziel_px in ((44, (309.6, 343.6)), (45, (310.2, 346.3)), (46, (318.8, 350.9)), (55, (325.5, 355.0))):      # Taille links: Kante zwischen Soll (308,346) und Boden L2 (bis x=314 bei y=350)
    _nr['V'][vi] = S1.auf_tiefe(np.array(ziel_px, float), S1.tiefe(_nr['V'][vi]))
_nu = next(q for q in T if q['name'] == 'nr_unter')
for vi, ziel_px in ((14, (315.6, 355.0)), (15, (315.4, 354.8))):
    _nu['V'][vi] = S1.auf_tiefe(np.array(ziel_px, float), S1.tiefe(_nu['V'][vi]))
form = form_bauen(T, karten, kn, [('pose1', pz1), ('pose2', pz2), ('ruhe', pzr)])
FE.boden_nachfuehren(form, 0)
for rd in range(10):
    schreiben(form, np.zeros(M.P.shape, np.uint8) + 40, np.ones(M.P.shape[:2], bool))
    k = PR.Kandidat('lage.json')
    _, _, m, _ = k.rendern(1)
    tr = m & kern_b
    if not tr.any():
        break
    ys, xs = np.nonzero(tr)
    c = np.array([xs.mean() + 0.5, ys.mean() + 0.5])
    print('Boden-Treffer', int(tr.sum()), c)
    frei = cv2.distanceTransform((~kern_b).astype(np.uint8), cv2.DIST_L2, 3)
    gx = cv2.Sobel(frei.astype(np.float32), cv2.CV_32F, 1, 0, ksize=5)[int(c[1]), int(c[0])]
    gy = cv2.Sobel(frei.astype(np.float32), cv2.CV_32F, 0, 1, ksize=5)[int(c[1]), int(c[0])]
    g_ = np.array([gx, gy]) / (math.hypot(gx, gy) + 1e-6)
    Rk = S1.k['R']
    for t in T:
        if t['name'] == 'rumpf':
            continue
        p = S1.bild(t['V'])
        dd = np.linalg.norm(p - c, axis=1)
        q = dd < 4.5
        if q.any():
            tf = np.linalg.norm(t['V'][q] - S1.auge_aktor(), axis=1, keepdims=True)
            t['V'][q] += (g_[0] * (Rk[0] @ S1.R) + g_[1] * (Rk[1] @ S1.R)) * 2.0 * tf / 418.0
    form = form_bauen(T, karten, kn, [('pose1', pz1), ('pose2', pz2), ('ruhe', pzr)])
    FE.boden_nachfuehren(form, 0)
yt = yt0.copy()
RUNDEN = 8
for runde in range(RUNDEN):
    rgb = verdeckte_seite(malen(yt, gewinn))
    voll = blende_fuellen(rgb, M.belegt, 6)
    schreiben(form, np.clip(voll, 0, 255).astype(np.uint8), np.ones(voll.shape[:2], bool))
    if runde == RUNDEN - 1:
        break
    k = PR.Kandidat('lage.json')
    for n, sv in ((1, v1), (2, v2)):
        sauber, alb, m, _ = k.rendern(n)
        gef = FV.video_filter(sauber, bild=n, entzerren=False, rauschen=False).astype(np.float32)
        Yo = orig[n] @ YK.astype(np.float32)
        Yg = gef @ YK.astype(np.float32)
        if n == 1:
            r = cv2.GaussianBlur(Yo - Yg, (0, 0), 1.2) * cv2.erode(m.astype(np.uint8), offen)
            rt = lesen(r[..., None].astype(np.float32), v1)[..., 0]
            yt = yt + np.where(v1['vis'], 0.42 * rt, 0)
            yt = np.where(beide, np.minimum(yt, yt2 + 12.0), yt)
            print('Runde', runde, 'Rest im Umriss', round(float(np.abs(r[m]).mean()), 2))
        for s in STELLEN:
            if s['bild'] != n:
                continue
            p = np.zeros((480, 640), bool)
            for q in s['polygone']:
                p |= PR.poly_maske(q)
            p &= m
            if p.sum() < 8:
                continue
            f = np.clip((Yo[p].mean() + 12.0) / (Yg[p].mean() + 12.0), 0.85, 1.18)
            gewinn[s['id']] = np.clip(gewinn.get(s['id'], np.ones(3)) * f ** 0.7, 0.45, 1.7)
        if n == 2:                                                     # Hakeninneres: Rotbraun, Helligkeit nachstellen
            p = cv2.erode(PR.poly_maske(FB2['H']['polygon']).astype(np.uint8), np.ones((9, 9), np.uint8)).astype(bool) & m
            if p.sum() > 8:
                f = np.clip((Yo[p].mean() + 12.0) / (Yg[p].mean() + 12.0), 0.85, 1.18)
                gewinn['H'] = np.clip(gewinn.get('H', np.ones(3)) * f ** 0.7, 0.7, 1.2)
print({a: np.round(b, 2).tolist() for a, b in gewinn.items()})
cv2.imwrite('textur_%s.png' % tag, cv2.resize(np.clip(voll, 0, 255).astype(np.uint8)[:, :, ::-1], None, fx=2, fy=2, interpolation=cv2.INTER_NEAREST))
aus = pruefen(tag)
print(''.join(l for l in open(os.path.join(aus, 'bericht.txt'), encoding='utf-8') if ('NEIN' in l or l[:3] in ('(a)', '(b)', '(c)', '(d)', 'Bil') or 'FEHLER' in l or 'Kennz' in l or 'Rahmen' in l)))
for a_, b_ in (('blatt_bild1.png', 'vergleich_bild1.png'), ('blatt_bild2.png', 'vergleich_bild2.png'), ('rundum.png', 'rundum.png')):
    shutil.copy(os.path.join(aus, a_), os.path.join(AUS, b_))
