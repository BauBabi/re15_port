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
    tx = lambda st: ['14: %d px, 15: %d px, 28: %d px, 30: %d px sichtbar' % tuple(j2[st][str(b)]['sichtbar_px'] for b in (14, 15, 28, 30))]
    r1 = nebeneinander([beschriften(nb, ['Ihre 4 Schleifen ("Exakt 4 Krallen")']),
                        beschriften(pa, ['heute (Spiel-Asset): 28 eingezogen'] + tx('asset')),
                        beschriften(pc, ['K5c (vor dem Einziehen)'] + tx('k5c')),
                        beschriften(pv, ['Vorschlag: 28 mit K5c-Form zurück'] + tx('vorschlag'))],
                       ['Vorn links (Bild 2): Ihr Bild', 'heute', 'K5c', 'Vorschlag'], gr=24)
    zeilen = []
    for i in range(1, 5):
        d = ds['schleife%d' % i]['abstand_zu_modellspitze_k5c_px']
        zeilen.append(['Schleife %d (von links im Bild)' % i, ', '.join('%s: %.1f px' % (TEILNAME[int(b)], v) for b, v in sorted(d.items(), key=lambda t: t[1])[:2]),
                       {1: 'Kralle 30', 2: 'Kralle 14 (oder Handspitze 13 - Frage)', 3: 'Kralle 15', 4: 'Kralle 28 (heute eingezogen)'}[i]])
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
    fo = [(maske_v_box(MS['krallen_vorn_rechts_form1'], 2, box, f), (0, 60, 255)), (maske_v_box(MS['krallen_vorn_rechts_form3'], 2, box, f), (0, 60, 255))]
    me = (18, 19, 20, 29, 31)
    pa, _ = panel(ka, 2, box, f, me, fo, wz); pc, _ = panel(kc, 2, box, f, me, fo, wz); pv, _ = panel(kv, 2, box, f, me, fo, wz)
    def tx2(st):
        return ['%s: %d px sichtbar, davon %d über Hintergrund' % (b, j2[st][b]['sichtbar_px'], j2[st][b]['ueber_hintergrund_px']) for b in ('19', '20', '29', '31')]
    dr = KJ['drehungen']; k19 = KJ['kralle19']
    r2 = nebeneinander([beschriften(nb, ['Ihre 2 Krallenformen (blau) und der Schriftzug']),
                        beschriften(pa, ['heute: 19, 20, 29 eingezogen'] + tx2('asset')),
                        beschriften(pc, ['K5c: 20 und 29 stehen frei vor dem Hintergrund'] + tx2('k5c')),
                        beschriften(pv, ['Vorschlag: 19 gekürzt (x%.2f), 20/29 gedreht' % k19['laengenfaktor']] + tx2('vorschlag'))],
                       ['Vorn rechts (Bild 2): Ihr Bild', 'heute', 'K5c', 'Vorschlag (Annahme: verdeckt durch die Schrift)'], gr=24)
    zeilen = [['Kralle 31 = Ihre Form 1', 'bleibt wie heute', 'unteres Formende %.1f px neben der Spitze von 31' % ZI['krallen_vorn_rechts_form1']['form']['abstand_unteres_ende_zu_spitze_k5c_px']],
              ['Kralle 19 = Ihre Form 3', 'K5c-Lage, Länge x%.4f entlang der eigenen Achse' % k19['laengenfaktor'],
               'Spitze K5c %s -> Vorschlag %s; Ihr Strichende %s; Rest %.1f px (ein Faktor allein trifft nicht genau)' % (
                   tuple(k19['spitze_k5c_vorlage']), tuple(k19['spitze_vorschlag_vorlage']), tuple(round(v, 1) for v in k19['nutzer_spitze_vorlage']), k19['rest_abstand_px'])]]
    for b in ('20', '29'):
        d = dr[b]
        zeilen.append(['Kralle %s (nicht sichtbar, Annahme)' % b, 'nur um die Wurzel gedreht (%.0f Grad), Länge wie K5c' % d['drehwinkel_grad'],
                       'NICHT GELUNGEN: noch %d px über Hintergrund, %d Ecken stecken in der Hand; in 6000 Drehungen keine ganz im Schriftzug (Bild 1 danach: %d px über Hintergrund)' % (
                           d['vorschlag']['ueber_hintergrund_px'], d['vorschlag']['durchdringung_ecken'], KJ['bild1_kontrolle'][b]['vorschlag']['ueber_hintergrund_px'])])
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
                       'Krallen, gelb schraffiert = vom Schriftzug verdeckte Fläche. "K5c" ist die Form vor dem Einziehen der Krallen. Der Vorschlag liegt nur als '
                       'Kandidat vor (nicht im Spiel). Pixelzahlen in Video-Pixeln (640 x 480).')
    leg = tabelle(W, [('Farbe = Modellteil', 1.0)], [['%s (Teil %d)' % (TEILNAME[b], b)] for b in (14, 15, 28, 30, 19, 20, 29, 31)], gr=18,
                  farben=[KF[b] for b in (14, 15, 28, 30, 19, 20, 29, 31)])
    stapeln([titel, r1, l1, r2, l2, r3, leg]).save(os.path.join(ZIEL, '21_krallen.png'))


def kopf_tafel():
    ka = kand(ASSET)
    lf = 'Gesicht vorne.png'; dx = VS[lf]['dx']
    L = auf_weiss(lies(D + lf)); G = grund_auf_weiss(2)
    R2 = dict(np.load(ARBEIT + '/render_asset_2.npz'))
    aug = AK['augen']; kopf = AK['kopf']
    # Ausschnitt Augen (Grundlage-Koordinaten)
    x0, y0, x1, y1 = 15, 205, 205, 285; f = 5
    def gz(img):
        return cv2.resize(img[y0:y1, x0:x1], None, fx=f, fy=f, interpolation=cv2.INTER_NEAREST)
    p1 = gz(L[:, dx:dx + G.shape[1]].copy())
    heute = MS.get('augen_vorn__modell')
    mod = modell_grau(R2['rgb'], R2['maske'])
    hm = np.load(ARBEIT + '/kopf_augen_masken.npz')['augen_heute']
    p2 = mod.copy(); einfaerben(p2, hm, (255, 0, 255), 0.9)
    p3 = mod.copy(); einfaerben(p3, MS['augen_vorn__modell'], (255, 0, 255), 0.9)
    p2, p3 = gz(p2), gz(p3)
    def zk(m):
        return cv2.resize(m[y0:y1, x0:x1].astype(np.uint8), None, fx=f, fy=f, interpolation=cv2.INTER_NEAREST) > 0
    kontur(p3, zk(MS['augen_vorn__modell']), (255, 255, 255), 3)
    gestrichelt(p3, zk(hm & ~MS['augen_vorn__modell']), (60, 60, 60), 3, 7)
    for p in (p1, p2, p3):
        for a in aug['nutzerpunkte']:
            x, y = a['punkt_grundlage']; ex, ey = a['linienende_grundlage']
            cv2.circle(p, (int((x - x0 + 0.5) * f), int((y - y0 + 0.5) * f)), 22, (255, 200, 0), 3, lineType=cv2.LINE_AA)
            cv2.drawMarker(p, (int((ex - x0 + 0.5) * f), int((ey - y0 + 0.5) * f)), (0, 160, 0), cv2.MARKER_TILTED_CROSS, 18, 3)
    np_ = aug['nutzerpunkte']
    t1 = ['gelber Kreis = Augenpunkt (heller Fleck neben dem Linienende, grünes Kreuz)',
          'rechts (Wesen): %.1f px neben dem Linienende, L* %.0f gegen %.0f daneben' % (np_[0]['abstand_punkt_zu_linienende_px_grundlage'], np_[0]['lab_punkt'][0], np_[0]['lab_umgebung'][0]),
          'links (Wesen): %.1f px neben dem Linienende, L* %.0f gegen %.0f daneben' % (np_[1]['abstand_punkt_zu_linienende_px_grundlage'], np_[1]['lab_punkt'][0], np_[1]['lab_umgebung'][0])]
    t2 = ['magenta = heutige G4-Augenflächen (%d Flächen, %d in Bild 2 sichtbar)' % (aug['heute_g4_augen']['flaechen'], aug['heute_g4_augen']['sichtbar_bild2']),
          'rechtes Auge: Mitte %.1f Video-px vom Nutzerpunkt, überlappt' % np_[0]['heutige_augenflaeche_naechste']['abstand_mitte_px_vorlage'],
          'linkes Auge: Mitte %.1f Video-px vom Nutzerpunkt (nach innen/oben), keine Überlappung' % np_[1]['heutige_augenflaeche_naechste']['abstand_mitte_px_vorlage']]
    t3 = ['magenta = Vorschlag: Flächen unter Ihren Punkten = Auge (%s)' % ', '.join(ZI['augen_vorn']['modell_zuordnung']['liste']),
          'grau gestrichelt = heutige Augenflächen, die nicht unter Ihren Punkten liegen (würden Haut)',
          'rechtes Auge: Fläche 22|0|82 ist schon heute Auge; links neu: 22|1|112, 22|1|113']
    r1 = nebeneinander([beschriften(p1, t1, 18), beschriften(p2, t2, 18), beschriften(p3, t3, 18)],
                       ['Ihr Bild "Gesicht vorne" (Ausschnitt, x%d)' % f, 'Modellkopf heute (Bild-2-Pose)', 'Vorschlag nach Ihren Augenpunkten'], gr=24)
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
    mm = mod.copy(); einfaerben(mm, mk, (0, 170, 255), 0.45); kontur(mm, MS['gesicht_vorn'], (230, 0, 0), 3)
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


if __name__ == '__main__':
    krallen_tafel(); print('21')
    kopf_tafel(); print('22')
