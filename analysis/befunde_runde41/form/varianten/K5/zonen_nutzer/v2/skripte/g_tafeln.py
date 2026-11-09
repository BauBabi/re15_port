"""Vorlage-Tafeln fuer den Nutzer (Grundlage v2). Aufruf: python g_tafeln.py <zielordner>"""
import json, os, sys
import numpy as np, cv2
from PIL import Image, ImageDraw
from g_basis import *
from g_tafel_basis import *
from g_meta import META, KRALLE_ZUORDNUNG

ZIEL = sys.argv[1] if len(sys.argv) > 1 else ARBEIT + '/tafeln'
os.makedirs(ZIEL, exist_ok=True)
ZJ = json.load(open(ARBEIT + '/zonen_v2.json', encoding='utf-8'))
ZONEN = ZJ['zonen']
ZI = {z['id']: z for z in ZONEN}
MS = dict(np.load(ARBEIT + '/zonen_v2_masken.npz'))
R = {n: dict(np.load(ARBEIT + '/render_asset_%d.npz' % n)) for n in (1, 2)}
BILDER = ['Rücken.png', 'Kopf_von_hinten.png', 'Schwanz_hinten.png', 'linkes_bein_hinten.png', 'rechtes_bein_hinten.png',
          'linker_rechter_Fuß_hinten.png', 'Bein_Vorne_rechts.png', 'Vorne_Oberkörper.png', 'Gesicht vorne.png',
          'vorne_rechte_Hand_und_Kralle.png', 'Vorderes_Linkes_Bein_mit_fuß.png']
SICH_FARBE = {'sicher': (0, 120, 0), 'unsicher': (200, 120, 0), 'verdeckt': (150, 0, 0)}
RANG = {'teil': 0, 'abschnitt': 1, 'stelle': 2, 'kralle': 3, 'bedeutung': 2}


def rang(z):
    if z['bau'] == 'abschnitt':
        return 1
    if z.get('eltern') is None and z['bau'] not in ('modellbedeutung',):
        return 0
    if z['bau'] in ('schleife', 'krallenform'):
        return 4
    return 2 + (1 if z['bau'] in ('fleck', 'fleck_n', 'wachsen', 'wachsen_n', 'modellbedeutung') else 0)


def zonen_von(bild):
    zs = [z for z in ZONEN if z.get('nutzerbild') == bild]
    if bild == 'Gesicht vorne.png':
        zs += [ZI['hals_unterseite']]
    return sorted(zs, key=lambda z: (rang(z), -z.get('zone_px_vorlage', 0)))


def modellteile_text(z):
    zu = z['modell_zuordnung']
    teile = []
    for b, nfl in zu['je_mesh'].items():
        teile.append('%s (Teil %s): %d Fl.' % (TEILNAME[int(b)], b, nfl))
    t = '; '.join(teile) if teile else 'keine sichtbare Fläche'
    if zu.get('sichtbar') is not None and zu['flaechen'] != zu['sichtbar']:
        t += ' (davon %d in diesem Bild sichtbar)' % zu['sichtbar']
    if z.get('ganzes_teil'):
        t += '; ganzes Teil inkl. Rückseite: ' + ', '.join('%s' % TEILNAME[b] for b in z['ganzes_teil']['meshes'])
    if z.get('teilflaeche'):
        t += ' – Material nur an der Stelle, nicht auf der ganzen Fläche'
    return t


def panel_modell(n, zs, dx, H, W, farben, mit_nummern=True, k=None):
    """Modell (grau) im Rahmen des beschrifteten Bildes, Zonen eingefaerbt; Rueckgabe Bild + Markenpunkte."""
    rr = k or R[n]
    h, w = rr['maske'].shape
    out = np.full((H, W, 3), 255, np.uint8)
    out[:h, dx:dx + w] = modell_grau(rr['rgb'], rr['maske'])
    marken_ = []
    for z in zs:
        c = farben[z['id']]
        mz = MS.get(z['id'] + '__modell'); mg = MS.get(z['id'])
        ziel = out[:h, dx:dx + w]
        if z.get('teilflaeche') and mg is not None and mg.any():
            # Stelle: nur der Fleck selbst kraeftig; die tragenden Flaechen nur als feine Linie
            kontur(ziel, mz, c, 1)
            einfaerben(ziel, mg & rr['maske'], c, 0.9)
        elif mz is not None and mz.any():
            einfaerben(ziel, mz, c, 0.70)
        elif mg is not None:
            schraffur(ziel, mg, c, 6)
        if mg is not None and mg.any() and z['bau'] != 'modellbedeutung':
            gestrichelt(ziel, mg, tuple(int(v * 0.6) for v in c), 2)
        q = (mg & rr['maske']) if (z.get('teilflaeche') and mg is not None and (mg & rr['maske']).any()) else (mz if (mz is not None and mz.any()) else mg)
        if q is not None and q.any():
            ys, xs = np.nonzero(q)
            # Punkt im Inneren nahe dem Schwerpunkt
            dt = cv2.distanceTransform(np.pad(q, 1).astype(np.uint8), cv2.DIST_L2, 3)[1:-1, 1:-1]
            cx, cy = xs.mean(), ys.mean()
            sc = dt - 0.02 * np.hypot(np.arange(w)[None, :] - cx, np.arange(h)[:, None] - cy)
            sc[~q] = -1e9
            y, x = np.unravel_index(np.argmax(sc), sc.shape)
            marken_.append([x + dx, y, z['nr'], c])
    return out, marken_


def legende(zs, farben, breite):
    zeilen, fb, mk = [], [], []
    for z in zs:
        si, grund, aend = z['sicherheit'], z['sicherheit_grund'], z['aenderung_v1']
        zeilen.append([z['nr'], '"%s"' % z['nutzerwort'], modellteile_text(z), z['material'], '%s: %s' % (si, grund), aend or '–'])
        fb.append(farben[z['id']]); mk.append(bool(aend))
    sp = [('Nr', 0.045), ('Ihr Wort (wörtlich)', 0.19), ('Modellteil(e) und Flächenzahl', 0.22), ('Material laut Ihrem Wort', 0.12),
          ('Sicherheit und Grund', 0.26), ('Änderung gegenüber v1', 0.165)]
    return tabelle(breite, sp, zeilen, gr=20, kopf_gr=21, farben=fb, markierung=mk)


def dateiname(bild):
    t = bild.replace('.png', '').replace(' ', '_')
    for u, e in (('ä', 'ae'), ('ö', 'oe'), ('ü', 'ue'), ('Ä', 'Ae'), ('Ö', 'Oe'), ('Ü', 'Ue'), ('ß', 'ss')):
        t = t.replace(u, e)
    return t


def nutzer_tafel(i, bild):
    lf = bild; n = 1 if VS[lf]['grund'].startswith('R') else 2; dx = VS[lf]['dx']
    L = lies(D + lf); H, W = L.shape[:2]
    links = auf_weiss(L)
    zs = sorted(zonen_von(bild), key=lambda z: [int(x) for x in z['nr'].split('.')])
    zs_mal = sorted(zs, key=lambda z: (rang(z), -z.get('zone_px_vorlage', 0)))
    farben = {z['id']: farbe(k) for k, z in enumerate(zs)}
    mitte, mk = panel_modell(n, zs_mal, dx, H, W, farben)
    grund = np.full((H, W, 3), 255, np.uint8); g = grund_auf_weiss(n); grund[:g.shape[0], dx:dx + g.shape[1]] = g
    rechts = (0.5 * mitte + 0.5 * grund).astype(np.uint8)
    # Detailausschnitt: Huelle aller Zonen (+ Rand)
    alle = np.zeros((H, W), bool)
    for z in zs:
        for k in (z['id'], z['id'] + '__modell'):
            if k in MS and MS[k].shape == g.shape[:2]:
                alle[:g.shape[0], dx:dx + g.shape[1]] |= MS[k]
    ys, xs = np.nonzero(alle)
    x0, y0, x1, y1 = max(xs.min() - 30, 0), max(ys.min() - 30, 0), min(xs.max() + 30, W), min(ys.max() + 30, H)
    detail = (x1 - x0) * (y1 - y0) < 0.45 * H * W
    oben_titel = ['Ihr Bild (unverändert)', 'Modell (gleiche Pose, gleicher Ausschnitt)', 'Modell halb durchsichtig über Ihrem Grundbild']
    def detail_reihe(x0, y0, x1, y1, titel_zusatz, nur=None):
        f = min(4.0, 1000.0 / (x1 - x0), 820.0 / (y1 - y0))
        def zoom(b):
            return cv2.resize(b[y0:y1, x0:x1], None, fx=f, fy=f, interpolation=cv2.INTER_NEAREST)
        dl, dm, dr = zoom(links_roh), zoom(mitte_roh), zoom(rechts_roh)
        mk2 = [[(x - x0) * f, (y - y0) * f, t, c] for x, y, t, c in mk if x0 <= x <= x1 and y0 <= y <= y1 and (nur is None or t in nur)]
        dm = marken_seitlich(dm, mk2, 21); dr = marken_seitlich(dr, mk2, 21)
        dl = np.pad(dl, ((0, 0), (int(21 * 3.2),) * 2, (0, 0)), constant_values=255)
        return nebeneinander([dl, dm, dr], ['Vergrößerung (x%.1f)%s: Ihr Bild' % (f, titel_zusatz), 'Vergrößerung: Modell mit Zonen und Nummern',
                                            'Vergrößerung: überlagert'], gr=24)
    if detail:
        mitte_roh, rechts_roh, links_roh = mitte.copy(), rechts.copy(), links.copy()
        teile = []
        for b_ in (links, mitte, rechts):
            cv2.rectangle(b_, (int(x0), int(y0)), (int(x1), int(y1)), (0, 0, 0), 2)
        unten = [detail_reihe(x0, y0, x1, y1, '')]
        # zweite Vergroesserung fuer kleine Zonen, wenn die erste zu schwach vergroessert
        f1 = min(4.0, 1000.0 / (x1 - x0), 820.0 / (y1 - y0))
        klein = [z for z in zs if z.get('zone_px_vorlage', 0) < 1500 and z['id'] in MS]
        if f1 < 2.2 and klein:
            km = np.zeros((H, W), bool)
            for z in klein:
                km[:g.shape[0], dx:dx + g.shape[1]] |= MS[z['id']]
            ys2, xs2 = np.nonzero(km)
            if len(xs2):
                a0, b0, a1, b1 = max(xs2.min() - 25, 0), max(ys2.min() - 25, 0), min(xs2.max() + 25, W), min(ys2.max() + 25, H)
                if (a1 - a0) * (b1 - b0) < 0.4 * (x1 - x0) * (y1 - y0):
                    for b_ in (links, mitte, rechts):
                        cv2.rectangle(b_, (int(a0), int(b0)), (int(a1), int(b1)), (0, 0, 200), 2)
                    unten.append(detail_reihe(a0, b0, a1, b1, ', blauer Rahmen', nur={z['nr'] for z in klein}))
        oben = nebeneinander([links, mitte, rechts], [t + ' – Rahmen = Vergrößerung unten' if k == 0 else t for k, t in enumerate(oben_titel)], gr=24)
        teile = [oben] + unten
    else:
        mitte = marken_seitlich(mitte, mk, 19); rechts = marken_seitlich(rechts, mk, 19)
        links = np.pad(links, ((0, 0), (int(19 * 3.2),) * 2, (0, 0)), constant_values=255)
        teile = [nebeneinander([links, mitte, rechts], oben_titel, gr=24)]
    breite = max(t.size[0] for t in teile)
    titel = titelzeile(breite, '%02d  %s  (Bild %d, %s)' % (i, bild.replace('.png', ''), n, 'Rückansicht' if n == 1 else 'Vorderansicht'),
                       'Farbe und Nummer = erkannte Zone. Gefüllt = Modellflächen, die ich der Zone zuordne. Gestrichelt = die Stelle, die ich in Ihrem Bild '
                       'gelesen habe (Ihr Umriss, oder der Fleck neben Ihrem Linienende). Schraffiert = im Modell an dieser Stelle nichts Passendes sichtbar. '
                       'Seiten immer aus Sicht des Wesens. Rote Schrift in der letzten Spalte = geändert gegenüber der ersten Fassung (v1).')
    leg = legende(zs, farben, breite)
    t = stapeln([titel] + teile + [leg])
    name = '%02d_%s.png' % (i, dateiname(bild))
    t.save(os.path.join(ZIEL, name))
    return name


def uebersicht():
    reihen = []
    for n in (1, 2):
        zs = sorted([z for z in ZONEN if z['bild'] == n], key=lambda z: (rang(z), -z.get('zone_px_vorlage', 0)))
        farben = {z['id']: farbe(k) for k, z in enumerate(zs)}
        g = grund_auf_weiss(n); h, w = g.shape[:2]
        mitte, mk = panel_modell(n, zs, 0, h, w, farben)
        F = 1.5
        g = cv2.resize(g, None, fx=F, fy=F, interpolation=cv2.INTER_LINEAR)
        mitte = cv2.resize(mitte, None, fx=F, fy=F, interpolation=cv2.INTER_NEAREST)
        mitte = marken_seitlich(mitte, [[x * F, y * F, t, c] for x, y, t, c in mk], 17)
        zs = sorted(zs, key=lambda z: [int(x) for x in z['nr'].split('.')])
        reihen.append((nebeneinander([g, mitte], ['Bild %d: Video (Ihr Grundbild)' % n, 'Bild %d: Modell, gleiche Pose, alle erkannten Zonen' % n], gr=26),
                       zs, farben))
    teile = [titelzeile(1600, '00  Übersicht: alle erkannten Zonen',
                         'Oben Bild 1 (Rückansicht), unten Bild 2 (Vorderansicht). Nummern wie in den Einzeltafeln 01-11. Gefüllt = zugeordnete Modellflächen, '
                         'schraffiert = an dieser Stelle im Modell nicht sichtbar.')]
    for (b, zs, farben) in reihen:
        zeilen = [[z['nr'], '"%s"' % z['nutzerwort'], z['material'], z['sicherheit']] for z in zs]
        leg = tabelle(1500, [('Nr', 0.07), ('Ihr Wort', 0.48), ('Material laut Ihrem Wort', 0.28), ('Sicherheit', 0.17)], zeilen, gr=17, kopf_gr=18,
                      farben=[farben[z['id']] for z in zs])
        row = Image.new('RGB', (b.size[0] + leg.size[0] + 20, max(b.size[1], leg.size[1])), (255, 255, 255))
        row.paste(b, (0, 0)); row.paste(leg, (b.size[0] + 20, 0))
        teile.append(row)
    t = stapeln(teile)
    t.save(os.path.join(ZIEL, '00_uebersicht.png'))


TEILGRUPPEN = [('Oberkörper (Hüfte, Rücken, Brust, Hals)', (0, 7, 10, 21)), ('Kopf und Unterkiefer', (22, 23)), ('Schwanz', (1, 2, 3, 4, 5)),
               ('Zipfel (Schwanzspitze)', (6,)), ('linkes Bein hinten', (8, 24)), ('linker Fuß hinten', (25,)), ('rechtes Bein hinten', (9, 26)),
               ('rechter Fuß hinten', (27,)), ('rechtes Bein vorn (Arm, Hand)', (16, 17, 18)), ('Krallen vorn rechts', (19, 20, 29, 31)),
               ('linkes Bein vorn (Arm, Hand)', (11, 12, 13)), ('Krallen vorn links', (14, 15, 28, 30))]
MATFARBE = {'fleischfarben': (240, 150, 120), 'fleischige Wunde/Haut': (200, 60, 90), 'rotes Fleisch': (220, 20, 40), 'Musterung': (60, 170, 70),
            'Musterung/Maserung': (20, 110, 40), 'weiße Musterung': (120, 200, 255), 'bräunliche Musterung': (150, 90, 30), 'Kralle': (250, 200, 0),
            'Auge': (255, 0, 255), 'Maul innen': (130, 0, 200), 'Zahn': (0, 200, 200), 'Lippe (kein Farbwort)': (0, 90, 200),
            'Unterkiefer (kein Farbwort)': (110, 140, 255)}


def ansicht(fz, az, el, gross=760, ziel_h=430):
    a, K = v4f.rundum_bild(ASSET, 2, 0, az, el, gross, grund=255)
    m = K['maske']; out = np.full(a.shape, 255, np.uint8)
    g = cv2.cvtColor(a, cv2.COLOR_RGB2GRAY).astype(np.float32)
    out[m] = np.repeat(np.clip(120 + g * 0.5, 0, 225)[..., None], 3, 2)[m].astype(np.uint8)
    key = np.where(m, K['mesh'].astype(np.int64) * 1000000 + K['flaeche'], -1)
    sch = out.astype(float)
    for kk in np.unique(key[m]):
        f = fl_schluessel(int(kk // 1000000), int(kk % 1000000))
        if f in fz:
            q = key == kk
            sch[q] = sch[q] * 0.3 + np.array(fz[f]) * 0.7
    out = sch.astype(np.uint8)
    ys, xs = np.nonzero(m)
    c = out[max(ys.min() - 12, 0):ys.max() + 12, max(xs.min() - 12, 0):xs.max() + 12]
    f = ziel_h / c.shape[0]
    if c.shape[1] * f > 760:
        f = 760 / c.shape[1]
    return cv2.resize(c, None, fx=f, fy=f, interpolation=cv2.INTER_AREA)


def rundum():
    """Zwei Reihen: (A) Ihre Teilnamen, kraeftig = in einem Bild belegt, blass = nur benannt (nicht sichtbar, Haut nicht belegt);
    (B) Material laut Ihrem Wort (nur Zonen mit eigenem Materialwort). Ruhepose Clip 2 Bild 0."""
    belegt = set()
    for z in ZONEN:
        if z['bau'] not in ('modellbedeutung',) and z.get('modell_zuordnung'):
            for f in z['modell_zuordnung']['liste']:
                belegt.add(f)
    sicht = set()
    for n in (1, 2):
        rr = R[n]; k = np.where(rr['maske'], rr['mesh'].astype(np.int64) * 1000000 + rr['flaeche'], -1)
        sicht |= {fl_schluessel(int(x // 1000000), int(x % 1000000)) for x in np.unique(k[rr['maske']])}
    ka_ = kand(ASSET); alle = alle_flaechen(ka_.mo)
    fa = {}
    for i, (name, ms) in enumerate(TEILGRUPPEN):
        c = farbe(i)
        for f in alle:
            if int(f.split('|')[0]) in ms:
                fa[f] = c if (f in belegt and f in sicht) else tuple(int(255 - (255 - v) * 0.30) for v in c)
    fb = {}
    rang_mat = sorted([z for z in ZONEN if z['material'] in MATFARBE], key=lambda z: rang(z))
    for z in rang_mat:
        for f in z['modell_zuordnung']['liste']:
            fb[f] = MATFARBE[z['material']]
    ans = [('von vorn', 90, 0), ('von hinten', -90, 0), ('von links (Wesen)', 180, 0), ('von rechts (Wesen)', 0, 0), ('schräg von oben', 45, 40)]
    ra = nebeneinander([ansicht(fa, az, el) for t, az, el in ans], [t for t, _, _ in ans], gr=24)
    rb = nebeneinander([ansicht(fb, az, el) for t, az, el in ans], [t for t, _, _ in ans], gr=24)
    W = max(ra.size[0], rb.size[0])
    la = tabelle(W, [('Ihr Teilname (Reihe A)', 0.5), ('kräftig = in Bild 1/2 belegt; blass = nur benannt, im Video nicht sichtbar', 0.5)],
                 [[n, 'Teile ' + ', '.join(str(m) for m in ms)] for n, ms in TEILGRUPPEN], gr=19, kopf_gr=20, farben=[farbe(i) for i in range(len(TEILGRUPPEN))])
    mats = list(MATFARBE.items())
    lb = tabelle(W, [('Material laut Ihrem Wort (Reihe B)', 0.5), ('Zonen-Nummern', 0.5)],
                 [[m, ', '.join(z['nr'] for z in ZONEN if z['material'] == m)] for m, _ in mats], gr=19, kopf_gr=20, farben=[c for _, c in mats])
    titel = titelzeile(W, '20  Rundum: wie weit reicht jede Zone?',
                       'Heutiges Modell (Spiel-Asset) in Ruhepose aus fünf Richtungen. Reihe A: Ihre Teilnamen. Kräftige Farbe = Fläche, die in Bild 1 oder 2 '
                       'zu sehen und einer Ihrer Zonen zugeordnet ist; blasse Farbe = gehört zum benannten Teil, ist aber in keinem der zwei Bilder zu sehen '
                       '(bleibt "Haut (nicht belegt)"). Reihe B: nur die Zonen, für die Ihr Wort ein Material nennt; grau = Haut. '
                       'Krallen 28, 19, 20, 29 sind im heutigen Modell eingezogen und hier kaum zu sehen (siehe Tafel 21).')
    ka_t = titelzeile(W, 'Reihe A: Ihre Teilnamen', gr=26); kb_t = titelzeile(W, 'Reihe B: Material laut Ihrem Wort', gr=26)
    stapeln([titel, ka_t, ra, la, kb_t, rb, lb]).save(os.path.join(ZIEL, '20_rundum.png'))


if __name__ == '__main__':
    namen = []
    for i, b in enumerate(BILDER, start=1):
        namen.append(nutzer_tafel(i, b)); print(namen[-1])
    uebersicht(); print('00')
    rundum(); print('20')
    json.dump(namen, open(ARBEIT + '/tafel_namen.json', 'w', encoding='utf-8'), ensure_ascii=False)
