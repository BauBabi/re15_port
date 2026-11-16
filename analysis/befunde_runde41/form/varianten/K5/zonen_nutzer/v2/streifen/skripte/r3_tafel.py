"""Runde 3: dritte Zeile der Tafel 23 (Pruefer-Befunde 11 und 17): wo die roten Flaechen am Kopf sitzen, das Maul von vorn,
Rot ueber alle Bilder, seit welchem Stand. Wird von s_tafel.py aufgerufen."""
import json
import cv2
import numpy as np
from PIL import Image, ImageDraw
from s_basis import AUS, ZIEL

KLASSE_LEGENDE = [((215, 25, 35), 'die 6 roten Flächen'), ((245, 150, 0), 'alte Zone 30, außen'),
                  ((40, 110, 230), 'alte Zone 30, im Maul/Gelenk'), ((196, 160, 235), 'übriges Maul-Inneres'),
                  ((250, 248, 225), 'Zähne'), ((175, 190, 175), 'Kopf, Haut'), ((160, 178, 200), 'Unterkiefer, Haut')]


def lies(p):
    return cv2.cvtColor(cv2.imread(p), cv2.COLOR_BGR2RGB)


def zuschnitt(rgb):
    m = (rgb < 250).any(2)
    ys, xs = np.nonzero(m)
    return rgb[max(ys.min() - 6, 0):ys.max() + 7, max(xs.min() - 6, 0):xs.max() + 7]


def legende(T, w):
    zeilen = (len(KLASSE_LEGENDE) + 1) // 2
    out = Image.new('RGB', (w, 30 * zeilen + 4), (255, 255, 255)); dr = ImageDraw.Draw(out)
    for i, (f, t) in enumerate(KLASSE_LEGENDE):
        xx = (i % 2) * int(w * 0.56); yy = (i // 2) * 30
        dr.rectangle([xx, yy + 4, xx + 22, yy + 24], fill=f, outline=(120, 120, 120)); dr.text((xx + 30, yy + 3), t, font=T.F(18), fill=(0, 0, 0))
    return out


def balken(T, w, h, je_clip, titel_links):
    """je_clip: {clip: [bilder_mit_rot, n, ...]} -> Balken je Clip: Anteil der Bilder mit sichtbarem Rot."""
    out = Image.new('RGB', (w, h), (255, 255, 255)); dr = ImageDraw.Draw(out)
    clips = sorted(je_clip, key=int)
    x0, y0, y1 = 46, 30, h - 40
    bw = (w - x0 - 10) / len(clips)
    dr.text((0, 2), titel_links, font=T.F(18, True), fill=(0, 0, 0))
    for p in (0, 50, 100):
        y = y1 - (y1 - y0) * p / 100
        dr.line([(x0, y), (w - 10, y)], fill=(225, 225, 225)); dr.text((2, y - 10), '%d %%' % p, font=T.F(15), fill=(90, 90, 90))
    for i, c in enumerate(clips):
        a, n = je_clip[c][0], je_clip[c][1]
        y = y1 - (y1 - y0) * a / max(n, 1)
        xa = x0 + i * bw + 2; xb = x0 + (i + 1) * bw - 2
        dr.rectangle([xa, y, xb, y1], fill=(215, 25, 35) if a else (230, 230, 230))
        if i % 3 == 0 or len(clips) <= 14:
            dr.text((xa, y1 + 4), str(c), font=T.F(14), fill=(60, 60, 60))
    dr.text((x0, h - 18), 'Clip-Nummer (Balken = Anteil der Bilder des Clips mit sichtbarem Rot)', font=T.F(14), fill=(60, 60, 60))
    return out


def zeile_c(T):
    PW, PH = T.PW, T.PH
    kg = json.load(open(ZIEL + '/messung/r3_kugel_griff.json', encoding='utf-8'))['zusammenfassung']
    kz = json.load(open(ZIEL + '/messung/r3_kugel_zu.json', encoding='utf-8'))['zusammenfassung']
    sp = json.load(open(ZIEL + '/messung/r3_spiegel.json', encoding='utf-8'))
    # C1: wo sitzen sie (von unten + von links, Griff-Haltung)
    u = T.einpassen(zuschnitt(lies(AUS + '/r3_klasse_griff_kopf_kiefer_unten.png')), PW // 2 - 6, 330, Image.LANCZOS)[0]
    uh = T.einpassen(zuschnitt(lies(AUS + '/r3_klasse_griff_mit_hals_unten.png')), PW // 2 - 6, 330, Image.LANCZOS)[0]
    obenpaar = Image.new('RGB', (PW, 330), (255, 255, 255)); obenpaar.paste(u, (0, 0)); obenpaar.paste(uh, (PW // 2 + 6, 0))
    li = T.einpassen(zuschnitt(lies(AUS + '/r3_klasse_griff_kopf_kiefer_links.png')), PW, 170, Image.LANCZOS)[0]
    C1 = T.feld('9  Wo die roten Flächen am Kopf sitzen', T.unter_einander(obenpaar, li, legende(T, PW)),
                '"Alte Zone 30" = was die Grundlage in Runde 2 Ihrem "geöffneten Maul" zugeordnet hatte. Kopf und Unterkiefer allein, Griff-Haltung. Oben links von unten (Schnauze oben), oben rechts mit Hals, unten von der '
                'linken Seite des Wesens. Die 6 roten Flächen sitzen außen an der Kopf-Unterseite hinter dem Unterkiefer, wo der Hals '
                'ansetzt. Flächen 9 und 209 sind nur auf der rechten Seite des Wesens rot; ihre Spiegel-Flächen links (0 und 202) sind Haut.')
    # C2: das Maul von vorn
    vo = T.einpassen(zuschnitt(lies(AUS + '/r3_klasse_griff_kopf_kiefer_vorn.png')), PW, 300, Image.LANCZOS)[0]
    vz = T.einpassen(zuschnitt(lies(AUS + '/r3_klasse_zu_kopf_kiefer_vorn.png')), PW, 200, Image.LANCZOS)[0]
    gp = json.load(open(ZIEL + '/messung/r3_gegenprobe_zone30_neu.json', encoding='utf-8'))
    r6 = [gp['richtungen_zu_griff_bild2']['alt'][f] for f in ('22|1|1', '22|1|8', '22|1|9', '22|1|194', '22|1|201', '22|1|209')]
    lo = min(min(v) for v in r6); hi = max(max(v) for v in r6)
    C2 = T.feld('10  Ihr "geöffnetes Maul" im Modell', T.unter_einander(vo, vz),
                'Von vorn, oben Maul offen (17 Grad), unten zu (0 Grad). Maß: aus wie vielen von 162 Richtungen eine Fläche zu sehen ist. '
                'Maul-Inneres wird erst beim Öffnen sichtbar. Die 6 roten Flächen sind offen wie zu gleich gut zu sehen (%d-%d Richtungen): '
                'sie gehören nicht zum Maul. Die neue Zone 30 der Grundlage (Runde 3) enthält keine der 6; alle %d ihrer Flächen werden '
                'beim Öffnen sichtbarer.' % (lo, hi, gp['zuwachs_beim_oeffnen']['neu']['mehr_sichtbar_offen']))
    # C3: Rot ueber alle Bilder (K5e)
    ra = json.load(open(ZIEL + '/messung/r3_rot_alle_K5e_f1.json', encoding='utf-8'))
    z_u = ra['zusammenfassung']['von unten']; z_s = ra['zusammenfassung']['Spiel 5090']
    b1 = balken(T, PW, 230, z_u['je_clip_bilder_mit_rot_von_n_min_median_max'], 'Kamera unter der Figur')
    b2 = balken(T, PW, 230, z_s['je_clip_bilder_mit_rot_von_n_min_median_max'], 'Spielkamera 5090 (Bild-2-Platz)')
    C3 = T.feld('11  Rot an der Hals-Unterseite, alle %d Bilder' % z_u['bilder'], T.unter_einander(b1, b2),
                'Gezählt wird nur, was wirklich vorn liegt, und nur Rot aus den 6 Flächen. Von unten: in %d von %d Bildern sichtbar '
                '(Median %d Bildpunkte). Von der Spielkamera 5090: in %d von %d Bildern, je nach Haltung.'
                % (z_u['bilder_mit_rot_aus_streifen'], z_u['bilder'], z_u['median_rot_streifen'],
                   z_s['bilder_mit_rot_aus_streifen'], z_s['bilder']))
    # C4: seit wann
    st = json.load(open(ZIEL + '/messung/r3_staende.json', encoding='utf-8'))
    tab = Image.new('RGB', (PW, 330), (255, 255, 255)); dr = ImageDraw.Draw(tab)
    sp_x = [0, 215, 345, 490]
    for x, t in zip(sp_x, ('Stand', 'rote Flächen', 'Rot von unten', 'Spielkamera')):
        dr.text((x, 0), t, font=T.F(18, True), fill=(0, 0, 0))
    y = 34
    for e in st['staende']:
        if e['name'] == 'Krallen-Kandidat':
            continue
        dr.text((sp_x[0], y), '%s (%s)' % (e['name'], e['datum'][8:10] + '.' + e['datum'][5:7] + '.'), font=T.F(18), fill=(0, 0, 0))
        dr.text((sp_x[1], y), ('%d von %d' % (e['davon_rot'], e['flaechen_an_der_stelle'])) if e['flaechen_an_der_stelle'] else 'Kopf anders',
                font=T.F(18), fill=(170, 0, 0) if e['davon_rot'] else (0, 0, 0))
        dr.text((sp_x[2], y), '%d / %d' % (e['von_unten'][0], e['von_unten'][1]), font=T.F(18), fill=(0, 0, 0))
        dr.text((sp_x[3], y), '%d / %d' % (e['spiel_5090'][0], e['spiel_5090'][1]), font=T.F(18), fill=(0, 0, 0))
        y += 36
    kk = st['k5e_gegen_krallen']
    dr.text((0, y + 6), 'Bilder mit sichtbarem Rot aus den 6 Flächen / gemessene Bilder.', font=T.F(16), fill=(60, 60, 60))
    C4 = T.feld('12  Seit wann?', tab,
                '"rote Flächen" = wie viele Flächen an der Stelle der 6 im jeweiligen Spiel-Stand rote Farbe tragen. Ältere Stände: '
                'jedes 2. Bild gemessen. Krallen-Kandidat: dieselben Bilder mit Rot wie K5e (Bildpunkte weichen in %d von %d Ansichten um '
                'höchstens %d ab, weil Krallen davor liegen).' % (kk['abweichend'], kk['ansichten'], kk['max_abweichung_px']))
    return T.zeile_bauen([C1, C2, C3, C4])
