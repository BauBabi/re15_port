"""Tafeln 21_krallen.png und 22_kopf_augen.png. Aufruf: python g_tafeln2.py <zielordner> <krallen_kandidat_ordner>"""
import json, os, sys
import numpy as np, cv2
from PIL import Image
from g_basis import *
from g_tafel_basis import *
import g_bau

ZIEL = sys.argv[1] if len(sys.argv) > 1 else ARBEIT + '/tafeln'
KK = sys.argv[2] if len(sys.argv) > 2 else ARBEIT + '/krallen_kandidat'
os.makedirs(ZIEL, exist_ok=True)
MS = dict(np.load(ARBEIT + '/zonen_v2_masken.npz'))
KJ = json.load(open(KK + '/kandidat.json', encoding='utf-8'))
KM = dict(np.load(KK + '/krallen_masken_bild2.npz'))
AK = json.load(open(ARBEIT + '/augen_kopf.json', encoding='utf-8'))
ZJ = json.load(open(ARBEIT + '/zonen_v2.json', encoding='utf-8'))
ZI = {z['id']: z for z in ZJ['zonen']}
KF = {14: (255, 140, 0), 15: (0, 170, 60), 28: (230, 0, 200), 30: (0, 190, 230), 19: (230, 30, 30), 20: (140, 220, 0), 29: (130, 60, 220),
      31: (0, 130, 130), 13: (150, 150, 170), 18: (150, 150, 170), 25: (150, 150, 170), 27: (150, 150, 170)}


def render_box(k, n, box, f):
    """Kennungs-Render der Vorlage-Box (x0, y0, x1, y1) mit Faktor f; Rueckgabe rgb, K."""
    kam = P.kamera(n)
    Mv = np.asarray(kam.abbildung['matrix'], float).reshape(3, 3)
    x0, y0, x1, y1 = box
    S = np.array([[f, 0, -x0 * f], [0, f, -y0 * f], [0, 0, 1]], float)
    lage, gier, clip, bild = k.stand(n)
    li, _ = P.licht_fuer(n, k.licht)
    tris = v4f.dreiecke_id(k.mo, k.kf(clip, bild), lage, gier, kam, li)
    H, W = int((y1 - y0) * f), int((x1 - x0) * f)
    ziel = np.zeros((H, W, 3), np.uint8)
    K = v4f.rastern_id(tris, k.tex, ziel, dict(matrix=(S @ Mv).tolist()))
    return ziel, K


def v_box(n, box, f):
    v = vorlage(n); x0, y0, x1, y1 = box
    return cv2.resize(v[y0:y1, x0:x1], None, fx=f, fy=f, interpolation=cv2.INTER_CUBIC)


def maske_v_box(m, n, box, f, quelle='g', dx=0):
    """Maske aus Grundlage- (quelle g) oder Vorlage-Koordinaten (v) in die Box."""
    x0, y0, x1, y1 = box; H, W = int((y1 - y0) * f), int((x1 - x0) * f)
    if quelle == 'g':
        s, t = s_t(n)
        M = np.array([[s * f, 0, (t[0] - x0) * f], [0, s * f, (t[1] - y0) * f]], np.float32)
    else:
        M = np.array([[f, 0, -x0 * f], [0, f, -y0 * f]], np.float32)
    return cv2.warpAffine(m.astype(np.float32), M, (W, H), flags=cv2.INTER_LINEAR) >= 0.5


def panel(k, n, box, f, meshes, user=(), wz=None, zehen=None, bild_dunkel=0.55):
    rgb, K = render_box(k, n, box, f)
    out = (v_box(n, box, f).astype(float) * bild_dunkel + 255 * (1 - bild_dunkel) * 0.35).astype(np.uint8)
    m = K['maske']
    g = cv2.cvtColor(rgb, cv2.COLOR_RGB2GRAY).astype(np.float32)
    out[m] = np.repeat(np.clip(110 + g * 1.2, 0, 220)[..., None], 3, 2)[m].astype(np.uint8)
    for b in meshes:
        q = (K['mesh'] == b) & m
        einfaerben(out, q, KF[b], 0.85)
    if zehen is not None:
        key = np.where(m, K['mesh'].astype(np.int64) * 1000000 + K['flaeche'], -1)
        zz = np.zeros_like(m)
        for kk in np.unique(key[m]):
            if fl_schluessel(int(kk // 1000000), int(kk % 1000000)) in zehen:
                zz |= key == kk
        einfaerben(out, zz, (250, 200, 0), 0.9)
    if wz is not None:
        schraffur(out, wz, (255, 230, 0), 9)
        kontur(out, wz, (200, 170, 0), 2)
    for um, rgb_ in user:
        kontur(out, um, rgb_, 3)
    return out, K


def beschriften(img, zeilen, gr=19):
    """Text unter das Bild."""
    h = len(zeilen) * int(gr * 1.35) + 10
    pad = np.full((h, img.shape[1], 3), 255, np.uint8)
    im = Image.fromarray(np.vstack([img, pad]))
    for i, z in enumerate(zeilen):
        text(im, (6, img.shape[0] + 5 + i * int(gr * 1.35)), z, gr)
    return np.array(im)


def legende_meshes(meshes, breite):
    return tabelle(breite, [('Farbe = Modellteil', 1.0)], [['%s (Teil %d)' % (TEILNAME[b], b)] for b in meshes], gr=18, kopf_gr=19,
                   farben=[KF[b] for b in meshes])


def krallen_tafel():
    ka, kc, kv = kand(ASSET), kand(K5C), P.Kandidat(KK + '/form.json')
    kvar = P.Kandidat(KK + '/variante_19_k5c_gekuerzt/form.json')
    K19 = json.load(open(KK + '/kralle19.json', encoding='utf-8'))
    j2 = KJ['bild2_je_stand']
    reihen = []
    # ---------------- vorn links
    box = (380, 355, 490, 435); f = 7
    lf = 'Vorderes_Linkes_Bein_mit_fuß.png'
    L = auf_weiss(lies(D + lf))
    s, t = s_t(2); dx = VS[lf]['dx']
    bx = [int((box[0] - t[0]) / s) + dx, int((box[1] - t[1]) / s), int((box[2] - t[0]) / s) + dx, int((box[3] - t[1]) / s)]
    nb = cv2.resize(L[bx[1]:bx[3], bx[0]:bx[2]], ((box[2] - box[0]) * f, (box[3] - box[1]) * f), interpolation=cv2.INTER_CUBIC)
    schl = [maske_v_box(MS['kralle_vorn_links_schleife%d' % i], 2, box, f) for i in range(1, 5)]
    us = [(m, (0, 60, 255)) for m in schl]
    me = (13, 14, 15, 28, 30)
    pa, _ = panel(ka, 2, box, f, me, us); pc, _ = panel(kc, 2, box, f, me, us); pv, _ = panel(kv, 2, box, f, me, us)
    sp = {k: v['naechste'] for k, v in ZJ['krallen']['spitzen']['vorn_links'].items()}
    ds = ZJ['krallen']['spitzen']['vorn_links']
    # Runde 4 (Pruefer r3 Nr. 7/12b): sichtbar im fertigen Bild, Krallen verdecken sich gegenseitig
    tx = lambda st: ['sichtbar (Video-px): 14: %d, 15: %d, 28: %d, 30: %d' % tuple(j2[st][str(b)]['sichtbar_px'] for b in (14, 15, 28, 30))]
    v28 = KJ['kralle28_verdeckt_bild2']
    r1 = nebeneinander([beschriften(nb, ['Ihre 4 Schleifen ("Exakt 4 Krallen")']),
                        beschriften(pa, ['heute (Spiel-Asset): 28 eingezogen'] + tx('asset')),
                        beschriften(pc, ['K5c (vor dem Einziehen)'] + tx('k5c')),
                        beschriften(pv, ['Vorschlag: 28 mit K5c-Form zurück (%d Ecken stecken in Hand 13, wie in K5c)' % KJ['durchdringung']['28']['ecken_in_der_hand']] + tx('vorschlag')
                                    + ['28 verdeckt %d px von Kralle 15 und %d px Hand 13' % (v28.get('15', 0), v28.get('13', 0))])],
                       ['Vorn links (Bild 2): Ihr Bild', 'heute', 'K5c', 'Vorschlag'], gr=24)
    zeilen = []
    for i in range(1, 5):
        d = ds['schleife%d' % i]['abstand_zu_modellspitze_k5c_px']
        zeilen.append(['Schleife %d (von links im Bild)' % i, ', '.join('%s: %.1f px' % (TEILNAME[int(b)], v) for b, v in sorted(d.items(), key=lambda t: t[1])[:2]),
                       {1: 'Kralle 30 (unsicher: im Modell nur 2 px sichtbar)', 2: 'Kralle 14 oder Handspitze 13 (Frage)', 3: 'Kralle 15',
                        4: 'Kralle 28 (heute eingezogen; zurückgeholt liegen nur %d von %d px in Ihrer Schleife, %d px über dem Fußabschnitt, und sie verdeckt '
                           'dort %d px von Kralle 15 = Ihre Schleife 3 - Frage)' % (j2['vorschlag']['28']['in_schleife_px']['4'], j2['vorschlag']['28']['sichtbar_px'],
                                                                               j2['vorschlag']['28']['im_fussabschnitt_ohne_schleifen_px'], v28.get('15', 0))}[i]])
    l1 = tabelle(r1.size[0], [('Ihre Schleife', 0.25), ('nächste Modellspitzen (Abstand der Spitzen in Video-Pixeln)', 0.45), ('so erkannt', 0.3)], zeilen, gr=19)
    # ---------------- vorn rechts
    box = (130, 320, 300, 480); f = 5
    lf = 'vorne_rechte_Hand_und_Kralle.png'; dx = VS[lf]['dx']
    L = auf_weiss(lies(D + lf))
    # Nutzerbild in die Box (ausserhalb des Ausschnitts weiss)
    nb = np.full(((box[3] - box[1]) * f, (box[2] - box[0]) * f, 3), 255, np.uint8)
    M = np.array([[s * f, 0, (t[0] - box[0]) * f - s * f * dx], [0, s * f, (t[1] - box[1]) * f]], np.float32)
    nb = cv2.warpAffine(L, M, (nb.shape[1], nb.shape[0]), flags=cv2.INTER_CUBIC, borderValue=(255, 255, 255))
    wz = maske_v_box(KM['wasserzeichen'], 2, box, f, 'v')
    fo = [(maske_v_box(MS['krallen_vorn_rechts_form1'], 2, box, f), (0, 60, 255)), (maske_v_box(MS['krallen_vorn_rechts_form3'], 2, box, f), (0, 60, 255)),
          (maske_v_box(MS['hand_vorn_rechts'], 2, box, f), (160, 0, 200))]
    me = (18, 19, 20, 29, 31)
    pa, _ = panel(ka, 2, box, f, me, fo, wz); pc, _ = panel(kc, 2, box, f, me, fo, wz); pv, _ = panel(kv, 2, box, f, me, fo, wz)
    pw, _ = panel(kvar, 2, box, f, me, fo, wz)
    def tx2(st):
        return ['%s: %d px sichtbar, davon %d über Hintergrund' % (b, j2[st][b]['sichtbar_px'], j2[st][b]['ueber_hintergrund_px']) for b in ('19', '20', '29', '31')]
    k19 = KJ['kralle19']; A = K19['A_k5c_lage_ein_faktor']; K5 = K19['k5c_unveraendert']; Bb = K19['B_beste'][0]
    r2 = nebeneinander([beschriften(nb, ['Ihre 2 Krallenformen (blau) und der Schriftzug;', 'lila = Ihr Umriss "vordere rechte Hand mit Krallen"']),
                        beschriften(pa, ['heute: 19, 20, 29 eingezogen'] + tx2('asset')),
                        beschriften(pc, ['K5c: 19 liegt auf Ihrer Form, aber auch', 'über Ihrer Hand (%d px Hand verdeckt);' % K5['verdeckt_sichtbare_hand18_px'],
                                         '20 und 29 stehen frei vor dem Hintergrund'] + tx2('k5c')),
                        beschriften(pw, ['NICHT übernommen: 19 gekürzt (x%.2f)' % A['laengenfaktor'], 'liegt über Ihrer Hand: %d px Hand verdeckt,' % A['verdeckt_sichtbare_hand18_px'],
                                         'nur %d von %d px in Ihrer Form, %d Ecken in der Hand' % (A['in_ihrer_form_px'], A['ihre_form_px'], A['ecken_in_hand18'])]),
                        beschriften(pv, ['Vorschlag: 19, 20 und 29 bleiben eingezogen', 'wie heute, bis Sie entscheiden (Frage)'] + tx2('vorschlag'))],
                       ['Vorn rechts (Bild 2): Ihr Bild', 'heute', 'K5c', 'gekürzt (verworfen)', 'Vorschlag'], gr=24)
    zeilen = [['Kralle 31 = Ihre Form rechts im Bild', 'bleibt wie heute', 'unteres Formende %.1f px neben der Spitze von 31' % ZI['krallen_vorn_rechts_form1']['form']['abstand_unteres_ende_zu_spitze_k5c_px']],
              ['Kralle 19 = Ihre Form links im Bild', 'bleibt eingezogen wie heute (Frage)',
               'In K5c-Lage liegt 19 zwar auf Ihrer Form (%d von %d px), aber vor Ihrer Hand und verdeckt %d Video-px Hand, die das Video zeigt. '
               'Gekürzt mit einem Faktor (x%.2f, Spitze %.0f px neben Ihrem Strichende) verdeckt sie noch %d px Hand und liegt nur mit %d px in Ihrer Form. '
               'Auch %d Drehungen um die Wurzel mit Spitze an Ihrem Strichende verdecken alle mindestens %d px Hand.' % (
                   K5['in_ihrer_form_px'], K5['ihre_form_px'], K5['verdeckt_sichtbare_hand18_px'], A['laengenfaktor'], A['spitze_abstand_px'],
                   A['verdeckt_sichtbare_hand18_px'], A['in_ihrer_form_px'], K19['B_loesungen'], Bb['verdeckt_sichtbare_hand18_px'])]]
    for b in ('20', '29'):
        zeilen.append(['Kralle %s (nicht sichtbar, Annahme nach Ihrem Wort)' % b, 'bleibt eingezogen wie heute',
                       'Drehen um die Wurzel reicht nicht: in Runde 1 lag keine von 6000 Drehungen ganz hinter der Schrift; die beste stand noch über dem Hintergrund, '
                       'steckte in der Hand und stünde in Bild 1 sichtbar ab. Entscheidung bei Ihnen (Frage)'])
    l2 = tabelle(r2.size[0], [('Kralle', 0.22), ('Vorschlag', 0.3), ('Ergebnis', 0.48)], zeilen, gr=19)
    # ---------------- hinten
    box = (180, 355, 290, 435); f = 7
    lf = 'linker_rechter_Fuß_hinten.png'; dx = VS[lf]['dx']; s1, t1 = s_t(1)
    L = auf_weiss(lies(D + lf))
    M = np.array([[s1 * f, 0, (t1[0] - box[0]) * f - s1 * f * dx], [0, s1 * f, (t1[1] - box[1]) * f]], np.float32)
    nb = cv2.warpAffine(L, M, ((box[2] - box[0]) * f, (box[3] - box[1]) * f), flags=cv2.INTER_CUBIC, borderValue=(255, 255, 255))
    Mk, _, _ = g_bau.lade(lf)
    rot = Mk['rot'][:, dx:dx + grund_rgba(1).shape[1]]
    um = [(maske_v_box(rot, 1, box, f), (255, 0, 0))]
    zehen = set(ZI['fusskrallen_hinten_links']['modell_zuordnung']['liste'])
    pa, _ = panel(ka, 1, box, f, (25,), um, None, zehen)
    na = ZI['fusskrallen_hinten_links'].get('nutzermarken_auf_modell', {})
    r3 = nebeneinander([beschriften(nb, ['Ihre roten Marken: "Kaum sichtbar, aber wahrscheinlich 3 Krallen"']),
                        beschriften(pa, ['heute = Vorschlag (hinten unverändert); gelb = 3 Zehenspitzen von Fuß 25',
                                         'Ihre Marken liegen rechts neben dem Modellfuß über Hintergrund (0 px auf Fuß 25)'])],
                       ['Hinten links (Bild 1): Ihr Bild', 'Modell: Fuß 25, Zehenspitzen = Krallen'], gr=24)
    W = max(r1.size[0], r2.size[0], r3.size[0])
    titel = titelzeile(W, '21  Krallen: heute | K5c | Vorschlag',
                       'Ausschnitte aus dem Video (Bild 2 bzw. Bild 1), vergrößert. Grau = Modell, Farben = einzelne Krallen, blau umrandet = Ihre gezeichneten '
                       'Krallen, lila umrandet = Ihr Hand-Umriss, gelb schraffiert = vom Schriftzug verdeckte Fläche. "K5c" ist die Form vor dem Einziehen der Krallen. Der Vorschlag liegt nur als '
                       'Kandidat vor (nicht im Spiel). Pixelzahlen in Video-Pixeln (640 x 480), gezählt im fertigen Bild: eine Kralle vor einer anderen verdeckt diese. Vorn rechts haben Sie zwei Formen gezeichnet: "Form rechts im Bild" und "Form links im Bild".')
    leg = tabelle(W, [('Farbe = Modellteil', 1.0)], [['%s (Teil %d)' % (TEILNAME[b], b)] for b in (14, 15, 28, 30, 19, 20, 29, 31)], gr=18,
                  farben=[KF[b] for b in (14, 15, 28, 30, 19, 20, 29, 31)])
    stapeln([titel, r1, l1, r2, l2, r3, leg]).save(os.path.join(ZIEL, '21_krallen.png'))


def kopf_tafel():
    ka = kand(ASSET)
    lf = 'Gesicht vorne.png'; dx = VS[lf]['dx']
    L = auf_weiss(lies(D + lf)); G = grund_auf_weiss(2)
    R2 = dict(np.load(ARBEIT + '/render_asset_2.npz'))
    mod_g = modell_grau(R2['rgb'], R2['maske'])
    aug = AK['augen']; kopf = AK['kopf']
    # Ausschnitt Augen (Grundlage-Koordinaten)
    x0, y0, x1, y1 = 15, 205, 205, 285; f = 5
    def gz(img):
        return cv2.resize(img[y0:y1, x0:x1], None, fx=f, fy=f, interpolation=cv2.INTER_NEAREST)
    p1 = gz(L[:, dx:dx + G.shape[1]].copy())
    import g_texel as GT
    from g_tafeln import spot_masken
    Kb = GT.render_box(2, (x0, y0, x1, y1))
    fb = f / GT.F
    def gb(img):
        return cv2.resize(img, None, fx=fb, fy=fb, interpolation=cv2.INTER_NEAREST)
    mod = modell_grau(Kb['rgb'], Kb['maske'])
    aset = set(json.load(open(MATERIAL_JSON, encoding='utf-8'))['material']['Auge']['liste'])
    keyb = np.where(Kb['maske'], Kb['mesh'].astype(np.int64) * 1000000 + Kb['flaeche'], -1)
    hm = np.isin(keyb, [int(q.split('|')[0]) * 1000000 + int(q.split('|')[1]) * 100000 + int(q.split('|')[2]) for q in aset])
    auf, fremd = spot_masken(Kb, 'augen_vorn')
    p2 = mod.copy(); einfaerben(p2, hm, (255, 0, 255), 0.9)
    p3 = mod.copy(); einfaerben(p3, auf, (255, 0, 255), 0.95)
    p4 = mod.copy(); einfaerben(p4, auf, (255, 0, 255), 0.95); einfaerben(p4, fremd & ~auf, (120, 0, 120), 0.9)
    p2, p3, p4 = gb(p2), gb(p3), gb(p4)
    gestrichelt(p3, gb((hm & ~auf).astype(np.uint8)) > 0, (60, 60, 60), 2, 7)
    for p in (p1, p2, p3, p4):
        for a in aug['nutzerpunkte']:
            ex, ey = a['linienende_grundlage']
            cv2.drawMarker(p, (int((ex - x0 + 0.5) * f), int((ey - y0 + 0.5) * f)), (0, 160, 0), cv2.MARKER_TILTED_CROSS, 18, 3)
    pk = np.load(ARBEIT + '/augen_punkte.npz')
    for p in (p1, p2, p3, p4):
        for k_ in ('rechts', 'links'):
            m_ = cv2.resize(pk[k_][y0:y1, x0:x1].astype(np.uint8), None, fx=f, fy=f, interpolation=cv2.INTER_NEAREST) > 0
            kontur(p, m_, (255, 255, 255), 4); kontur(p, m_, (255, 200, 0), 2)
    np_ = aug['nutzerpunkte']
    t1 = ['gelb umrandet = Augenpunkt (heller Fleck neben dem Linienende, grünes Kreuz)',
          'rechts (Wesen): %.1f px neben dem Linienende, %.1f Video-px groß' % (np_[0]['abstand_punkt_zu_linienende_px_grundlage'], np_[0]['punkt_px_vorlage']),
          'links (Wesen): %.1f px neben dem Linienende, %.1f Video-px groß' % (np_[1]['abstand_punkt_zu_linienende_px_grundlage'], np_[1]['punkt_px_vorlage'])]
    gr_ = aug['heute_g4_augen']['gruppen']
    t2 = ['magenta = heutige Augen des Modells (ganze Flächen)',
          'rechtes Auge heute %.0f Video-px, Mitte %.1f px neben Ihrem Punkt, Überlappung %.1f px' % (gr_[0]['px_vorlage'], np_[0]['heutige_augenflaeche_naechste']['abstand_mitte_px_vorlage'], np_[0]['heutige_augenflaeche_naechste']['ueberlappung_px_vorlage']),
          'linkes Auge heute %.0f Video-px, Mitte %.1f px neben Ihrem Punkt, keine Überlappung' % (gr_[1]['px_vorlage'], np_[1]['heutige_augenflaeche_naechste']['abstand_mitte_px_vorlage'])]
    t3 = ['magenta = Vorschlag: Auge NUR an Ihren Punkten (gelb umrandet), texelgenau',
          'rechts %d Texel, links %d Texel; beide Augen nach derselben Regel' % (np_[0]['auge_texel']['zahl'], np_[1]['auge_texel']['zahl']),
          'grau gestrichelt = heutige Augenflächen außerhalb Ihrer Punkte (wären dann Haut - Frage)']
    ao = aug['vorschlag']['anzeige_bild2_ohne_eigene_texel']
    t4 = ['dunkel-lila = dort erschiene das Auge AUCH, weil die heutige Textur',
          'dieselben Texel an anderen Stellen benutzt (Tafel 24).',
          'Ganz Bild 2: %.0f Video-px statt %.0f px Ihrer Punkte (%.1f-fach);' % (ao['gesamt_px_vorlage'], ao['punkte_px_vorlage'], ao['faktor_gesamt_zu_punkten']),
          'außerhalb der Punkte %.0f px: Kopf %.0f, Brust %.0f, Hals %.0f.' % (ao['ausserhalb_px_vorlage'], ao['ausserhalb_je_teil'].get('Kopf (Oberkiefer)', 0),
                                                                          ao['ausserhalb_je_teil'].get('Brust/Buckel', 0), ao['ausserhalb_je_teil'].get('Hals', 0)),
          'Damit das Auge nur der Punkt ist,',
          'braucht es später eigene Texel.']
    r1 = nebeneinander([beschriften(p1, t1, 18), beschriften(p2, t2, 18), beschriften(p3, t3, 18), beschriften(p4, t4, 18)],
                       ['Ihr Bild "Gesicht vorne" (Ausschnitt, x%d)' % f, 'Modellkopf heute (Bild-2-Pose)', 'Vorschlag nach Ihren Augenpunkten',
                        'Vorschlag mit der heutigen Textur'], gr=24)
    # Kopfumriss
    gx0, gy0, gx1, gy1 = 0, 60, 330, 530; f2 = 2.0
    b = G.copy()
    mk = np.load(ARBEIT + '/kopf_augen_masken.npz')['modellkopf']
    kontur(b, MS['gesicht_vorn'], (230, 0, 0), 3); gestrichelt(b, mk, (0, 170, 255), 3, 8)
    for a in np_:
        x, y = a['punkt_grundlage']; cv2.circle(b, (int(x), int(y)), 6, (255, 200, 0), 2)
    for c in aug['heute_g4_augen']['gruppen']:
        x, y = c['mitte_grundlage']; cv2.drawMarker(b, (int(x), int(y)), (255, 0, 255), cv2.MARKER_CROSS, 12, 2)
    b = cv2.resize(b[gy0:gy1, gx0:gx1], None, fx=f2, fy=f2, interpolation=cv2.INTER_LINEAR)
    mm = mod_g.copy(); einfaerben(mm, mk, (0, 170, 255), 0.45); kontur(mm, MS['gesicht_vorn'], (230, 0, 0), 3)
    mm = cv2.resize(mm[gy0:gy1, gx0:gx1], None, fx=f2, fy=f2, interpolation=cv2.INTER_LINEAR)
    d = kopf['modell_minus_nutzer_vorlage_px']
    zeilen = [['oben', '%.1f' % kopf['nutzerumriss_bbox_vorlage'][1], '%.1f' % kopf['modellkopf_bbox_vorlage'][1], '%+.1f' % d['oben'],
               'im Video keine Helligkeitskante (L* 3-12 von der Brust bis zum Gesicht); nur Ihr Umriss belegt die Oberkante'],
              ['unten', '%.1f' % kopf['nutzerumriss_bbox_vorlage'][3], '%.1f' % kopf['modellkopf_bbox_vorlage'][3], '%+.1f' % d['unten'], 'Unterkante des geöffneten Mauls'],
              ['links (im Bild)', '%.1f' % kopf['nutzerumriss_bbox_vorlage'][0], '%.1f' % kopf['modellkopf_bbox_vorlage'][0], '%+.1f' % d['links'],
               'Video-Silhouette in Augenhöhe bei x %.1f; Modellkopf dort %.1f' % (kopf['linke_kante'][0]['video_figur_links'], kopf['linke_kante'][0]['modellkopf_links'])],
              ['rechts (im Bild)', '%.1f' % kopf['nutzerumriss_bbox_vorlage'][2], '%.1f' % kopf['modellkopf_bbox_vorlage'][2], '%+.1f' % d['rechts'], 'dunkel auf dunkel, im Video nicht messbar'],
              ['Augenhöhe', '%.1f / %.1f' % tuple(kopf['augenhoehe_vorlage']['video']), '%.1f / %.1f' % tuple(kopf['augenhoehe_vorlage']['modell_heute']),
               '%+.1f' % (np.mean(kopf['augenhoehe_vorlage']['modell_heute']) - np.mean(kopf['augenhoehe_vorlage']['video'])), 'passt fast (Modell 4 px höher)'],
              ['Augenabstand', '%.1f' % kopf['augenabstand_vorlage_px']['video'], '%.1f' % kopf['augenabstand_vorlage_px']['modell_heute'],
               '%+.1f' % (kopf['augenabstand_vorlage_px']['modell_heute'] - kopf['augenabstand_vorlage_px']['video']), 'Modellaugen stehen enger']]
    r2 = nebeneinander([beschriften(b, ['rot = Ihr Gesichtsumriss', 'blau gestrichelt = Modellkopf (Kopf 22 + Unterkiefer 23)',
                                        'gelbe Kreise = Ihre Augenpunkte, magenta Kreuze = heutige Modellaugen'], 20),
                        beschriften(mm, ['blau = Modellkopf (Kopf 22 + Unterkiefer 23)', 'rot = Ihr Gesichtsumriss',
                                         'Der Modellkopf reicht 70 Video-px höher als Ihr Umriss'], 20)],
                       ['Kopfumriss auf Ihrem Grundbild', 'Modell in derselben Pose'], gr=24)
    W = max(r1.size[0], r2.size[0])
    tb = tabelle(W, [('Kante', 0.12), ('Ihr Umriss / Video', 0.14), ('Modell heute', 0.12), ('Modell minus Ihr Umriss (Video-px)', 0.16), ('Bemerkung', 0.46)], zeilen, gr=20)
    titel = titelzeile(W, '22  Kopf und Augen (Bild 2)',
                       'Oben: Ihre Augenpunkte, die heutigen Modellaugen und der Vorschlag. Unten: Kopfumriss Video/Ihr Umriss gegen das Modell. '
                       'Nur gemessen, an der Kopfform wurde nichts geändert. Video-px = Pixel im 640 x 480-Video.')
    stapeln([titel, r1, r2, tb]).save(os.path.join(ZIEL, '22_kopf_augen.png'))


def geteilt_tafel():
    """Tafel 24 (T2): wo erschiene jeder Fleck AUCH, weil die heutige Textur dieselben Texel an mehreren Stellen benutzt."""
    from g_tafeln import spot_masken, MATFARBE, TX, wort
    R = {n: dict(np.load(ARBEIT + '/render_asset_%d.npz' % n)) for n in (1, 2)}
    reihen = []
    zs = [z for z in ZJ['zonen'] if z['id'] in TX]
    boxen = {}
    for n in (1, 2):
        ys, xs = np.nonzero(R[n]['maske']); boxen[n] = (max(xs.min() - 8, 0), max(ys.min() - 8, 0), xs.max() + 9, ys.max() + 9)
    GR = 27
    for z in zs:
        c = MATFARBE.get(z['material'], (255, 0, 0))
        bilder = []
        for n in (1, 2):
            rr = R[n]
            out = modell_grau(rr['rgb'], rr['maske'])
            auf, fremd = spot_masken(rr, z['id'])
            einfaerben(out, auf, c, 0.95)
            rot = cv2.dilate(fremd.astype(np.uint8), np.ones((3, 3), np.uint8)) > 0
            einfaerben(out, rot & ~auf, (0, 0, 0), 0.85)
            x0, y0, x1, y1 = boxen[n]
            out = cv2.resize(out[y0:y1, x0:x1], None, fx=1.0, fy=1.0, interpolation=cv2.INTER_AREA)
            bilder.append(out)
        g = z['geteilte_texel']
        txt = ['Nr. %s %s' % (z['nr'], wort(z)), 'Material: %s' % z['material'],
               'Fleck: %d Texel, %d Video-px an Ihrer Stelle.' % (z['texel']['zahl'], z['texel']['flaeche_video_px']),
               'Dieselben Texel nutzen %d weitere Flächen.' % g['andere_flaechen'],
               'Dort erschiene der Fleck AUCH (schwarz): Bild 1 %.0f, Bild 2 %.0f Video-px.' % (g['bild1_px_video'], g['bild2_px_video']),
               'Teile mit Mitbenutzung: ' + ', '.join('%s (%d Fl.)' % (v['teil'], v['flaechen']) for v in sorted(g['je_mesh'].values(), key=lambda v: -v['flaechen'])[:5])
               + (' ...' if len(g['je_mesh']) > 5 else '')]
        hb = max(b.shape[0] for b in bilder)
        tb = np.full((hb, 900, 3), 255, np.uint8)
        im = Image.fromarray(tb)
        y = 8
        for i, t_ in enumerate(txt):
            for l in umbruch(t_, GR if i else GR + 2, 880, i == 0):
                text(im, (8, y), l, GR if i else GR + 2, i == 0); y += int(GR * 1.3)
        reihen.append(nebeneinander([np.array(im), bilder[0], bilder[1]], ['', 'Bild 1 (Rückansicht)', 'Bild 2 (Vorderansicht)'], gr=26))
    # Konflikte zwischen Zonen (Pruefer r2 Nr. 10)
    kf = ZJ.get('texel_konflikte', [])
    W0 = max(r.size[0] for r in reihen)
    if kf:
        zeilen = [['%s / %s' % (k['zone_a'], k['zone_b']), '"%s" gegen "%s"' % (k['material_a'], k['material_b']), str(k['texel']),
                   '%d Flächen (Teile %s)' % (k['flaechen_mit_diesen_texeln'], ', '.join(str(t) for t in k['teile']))] for k in kf]
        reihen.append(titelzeile(W0, 'Texel, die zwei Ihrer Zonen mit verschiedenem Material zugleich brauchen', 'Dieselben Texel liegen heute unter zwei Zonen. '
                                 'Beide Materialien zugleich geht nur, wenn eine der beiden Stellen später eigene Texel bekommt. Nur benannt, nichts geändert.', gr=30, unter_gr=26))
        reihen.append(tabelle(W0, [('Zonen', 0.12), ('Material laut Ihrem Wort', 0.4), ('gemeinsame Texel', 0.14), ('Flächen, die diese Texel heute benutzen', 0.34)],
                              zeilen, gr=GR, kopf_gr=GR))
    W = max(r.size[0] for r in reihen)
    titel = titelzeile(W, '24  Geteilte Texel: wo der Fleck heute AUCH erscheinen würde', gr=38, unter=
                       'Die heutige Textur benutzt dieselben Texel (Bildpunkte der Textur) für viele Flächen. Würde man einen Fleck nur in die Texel unter Ihrer '
                       'Stelle malen, erschiene er überall dort mit, wo andere Flächen dieselben Texel benutzen. Farbe = der Fleck an Ihrer Stelle; '
                       'schwarz = dort erschiene er zusätzlich. Diese Flächen bräuchten in der späteren Textur eigene Texel (Liste je Zone in zonen_v2.json, '
                       '"brauchen_eigene_texel"). Hier wird nichts umgebaut, nur gezeigt und gezählt.', unter_gr=28)
    stapeln([titel] + reihen).save(os.path.join(ZIEL, '24_geteilte_texel.png'))


if __name__ == '__main__':
    krallen_tafel(); print('21')
    kopf_tafel(); print('22')
    geteilt_tafel(); print('23')
