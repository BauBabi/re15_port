"""MESSBLATT_ZONEN_V2.md und LIESMICH.txt aus zonen_v2.json + kandidat.json (Runde 2). Aufruf: python g_messblatt.py <v2-ordner> <tafel-ordner>"""
import json, os, sys
from g_basis import TEILNAME, ARBEIT

V2 = sys.argv[1]; TAF = sys.argv[2]
Z = json.load(open(os.path.join(V2, 'zonen_v2.json'), encoding='utf-8'))
KJ = json.load(open(os.path.join(V2, 'krallen_kandidat', 'kandidat.json'), encoding='utf-8'))
K19 = json.load(open(os.path.join(V2, 'krallen_kandidat', 'kralle19.json'), encoding='utf-8'))
A19, K5_19, B19 = K19['A_k5c_lage_ein_faktor'], K19['k5c_unveraendert'], K19['B_beste'][0]
zonen = Z['zonen']; ZI = {z['id']: z for z in zonen}
aug, kopf = Z['augen'], Z['kopf']
NP = aug['nutzerpunkte']; GR = aug['heute_g4_augen']['gruppen']


def k(x, d=1):
    return ('%.*f' % (d, x)).replace('.', ',')


def wort(z):
    return '– (kein Wort von Ihnen; Modellgliederung: %s)' % z.get('modell_beschreibung', '') if z.get('nutzerwort') is None else '"%s"' % z['nutzerwort']


def teile(z):
    if z.get('texel'):
        t = z['texel']
        ms = sorted({int(f.split('|')[0]) for f in t['traegerflaechen']})
        return 'texelgenau: %d Texel, %d Video-px auf %s (%d Trägerflächen)' % (t['zahl'], t['flaeche_video_px'], ', '.join('%d %s' % (b, TEILNAME[b]) for b in ms),
                                                                             len(t['traegerflaechen']))
    zu = z['modell_zuordnung']
    t = ', '.join('%s %s (%d)' % (b, TEILNAME[int(b)], n) for b, n in zu['je_mesh'].items()) or '–'
    if zu.get('sichtbar') is not None and zu['sichtbar'] != zu['flaechen']:
        t += '; sichtbar %d' % zu['sichtbar']
    return t


def iou(z):
    v = z.get('iou_gegen_nutzerstelle')
    return '–' if v is None else k(v, 2)


k19 = KJ['kralle19']; r28 = ZI['kralle_vorn_links_schleife4']
AO = aug['vorschlag']['anzeige_bild2_ohne_eigene_texel']
ML = Z['maul_lage']; KONF = Z.get('texel_konflikte', [])
Z11 = ZI['zipfel_musterung']
# Runde 4: Zusatzwerte (Prüfer r3 Nr. 5, 6, 7)
import numpy as _np
from g_basis import s_t as _s_t
KU = Z['kopf_unterseite_ganzes_modell']; KW = ML['kieferwinkel_grad']
V28 = KJ['kralle28_verdeckt_bild2']; J2A = KJ['bild2_je_stand']['asset']; J2V = KJ['bild2_je_stand']['vorschlag']
Z45 = ZI['kopf_unterseite_maulfarbe']['modell_zuordnung']['liste']
Z45_SICHT = [f for f in Z45 if f in KU['aussen_sichtbar']]; Z45_NIE = [f for f in Z45 if f not in KU['aussen_sichtbar']]
_MS = _np.load(ARBEIT + '/zonen_v2_masken.npz'); _s2 = _s_t(2)[0] ** 2
MAUL_PX = float(_MS['maul_vorn__modell'].sum() * _s2); MAUL_IN = float((_MS['maul_vorn__modell'] & _MS['maul_vorn']).sum() * _s2)
MAUL_NEU = ML['aenderung_gegen_runde3']['neu']


def _kurz(fl):
    return ', '.join(f.replace('|', '/') for f in fl)


FRAGEN = [
    'Kopfhöhe und grüne Streifen auf dem Buckel gehören zusammen (Tafel 22 unten, Tafel 23 oben): Der Modellkopf reicht in Bild 2 %s Video-Pixel höher als '
    'Ihr Gesichtsumriss (Oberkante y %s statt %s); im Video ist dort keine Kante zu sehen, alles ist gleich dunkel. Derselbe Kopf steckt mit dem Hinterkopf im '
    'Buckel und im Hals (seit Stand K4) und wird wegen der Zeichen-Reihenfolge darübergemalt: das sind die grünen Streifen. Ein tieferer oder kleinerer Kopf '
    'würde beides ändern. Gilt Ihre rote Linie oben als Kopfoberkante, d. h. soll der Kopf tiefer oder kleiner werden, und darüber ist Brust ("großer '
    'fleischfarbener Teil")?' % (k(-kopf['modell_minus_nutzer_vorlage_px']['oben']), k(kopf['modellkopf_bbox_vorlage'][1], 0), k(kopf['nutzerumriss_bbox_vorlage'][1], 0)),
    'Kopf von hinten (Tafel 02): An der Stelle Ihres Kopf-Umrisses zeigt das Modell den rechten Oberarm; der Kopf liegt dahinter. Soll der Kopf dort zu sehen sein?',
    'Augen (Tafel 22): Ihre Augenpunkte sind klein (rechts %s, links %s Video-Pixel). Die heutigen Modellaugen sind %s bzw. %s Video-Pixel groß und liegen %s bzw. '
    '%s Pixel daneben. Mein Vorschlag für beide Augen gleich: Auge ist NUR der helle Punkt, der Rest des heutigen Augenfelds wird Haut. Richtig, oder ist das '
    'ganze dunkle Feld um den Punkt das Auge? Hinweis: Die heutige Textur benutzt die Texel unter Ihren Punkten auch an anderen Stellen; ohne eigene Texel '
    'erschiene das Auge in Bild 2 auf %s statt %s Video-Pixeln (Tafel 22 rechts).' % (
        k(NP[0]['punkt_px_vorlage']), k(NP[1]['punkt_px_vorlage']), k(GR[0]['px_vorlage'], 0), k(GR[1]['px_vorlage'], 0),
        k(NP[0]['heutige_augenflaeche_naechste']['abstand_mitte_px_vorlage']), k(NP[1]['heutige_augenflaeche_naechste']['abstand_mitte_px_vorlage']),
        k(AO['gesamt_px_vorlage'], 0), k(AO['punkte_px_vorlage'], 0)),
    'Augenhöhle (Tafel 09, Zone 40): 14 Flächen rund um die Augen benutzen heute dieselbe rote Texturstelle wie das Maul (in Bild 2 sind 2 davon zu sehen, '
    '23 Video-Pixel, direkt neben den Augen). Sollen sie Haut sein (mein Vorschlag)?',
    'Maul und Zähne (Tafel 09, Zonen 30 und 41): Als "geöffnetes Maul" nehme ich die %d Flächen, die erst beim Öffnen des Mauls zu sehen sind (Gaumen und '
    'Mundboden). In Bild 2 sind %d davon zu sehen, %s Video-Pixel, davon %s in Ihrem Umriss. Die Zähne haben kein Wort von Ihnen: eigenes Material wie das '
    'Maul, oder Haut?' % (len(ML['maulinneres_lage']), ZI['maul_vorn']['modell_zuordnung']['sichtbar'], k(MAUL_PX, 0), k(MAUL_IN, 0)),
    'Rote Streifen am Hals (Tafel 23 Mitte, Zone 45): %d Außenflächen an der Kopf-Unterseite hinter dem Unterkiefer tragen seit Stand K5c die rote Maul-Farbe '
    'und machen im Spiel die "komischen roten Streifen". %d weitere Flächen mit dieser Farbe sind im ganzen Modell nie zu sehen (vom Hals verdeckt). Alle '
    'liegen außen, nicht im Maul. Mein Vorschlag: Haut ohne eigene Farbe. Richtig?' % (len(Z45_SICHT), len(Z45_NIE)),
    'Krallen 20 und 29 vorn rechts (Tafel 21 Mitte): Zu Ihrer Annahme "wahrscheinlich auch 4": Durch bloßes Drehen lassen sie sich nicht hinter der Schrift '
    'verstecken (6000 Drehungen geprüft; die beste stand noch über dem Hintergrund, steckte in der Hand und stünde in Bild 1 sichtbar ab). Im Vorschlag bleiben '
    'sie eingezogen wie heute. So lassen, oder kürzer zurück?',
    'Kralle 19 (Tafel 21 Mitte, Ihre Form links im Bild): In ihrer früheren Lage (K5c) liegt sie auf Ihrer Form (%d von %d Pixeln), aber VOR Ihrer Hand und '
    'verdeckt %d Video-Pixel Hand, die im Video zu sehen sind; gekürzt (x%s) noch %d, um die Wurzel gedreht (%d Lagen) mindestens %d. Deshalb bleibt sie im '
    'Vorschlag eingezogen. (a) eingezogen lassen, (b) zurück, obwohl sie dann über der Hand liegt, oder (c) ist Ihre Form ein Teil der Hand?' % (
        K5_19['in_ihrer_form_px'], K5_19['ihre_form_px'], K5_19['verdeckt_sichtbare_hand18_px'], k(A19['laengenfaktor'], 2), A19['verdeckt_sichtbare_hand18_px'],
        K19['B_loesungen'], B19['verdeckt_sichtbare_hand18_px']),
    'Vorn links, Schleifen 1 und 2 (Tafel 21 oben, Tafel 11): Schleife 2 = Kralle 14 (mein Vorschlag, die linke Hand hat genau 4 Krallenteile) oder die '
    'Handspitze 13? Schleife 1 = Kralle 30, obwohl sie im Modell nur 2 Pixel zeigt?',
    'Kralle 28 (Tafel 21 oben, Ihre Schleife 4): Zurückgeholt in ihrer früheren Form sind in Bild 2 %d Video-Pixel von ihr zu sehen, davon nur %d in Ihrer '
    'Schleife 4 und %d über Ihrem "Fußabschnitt". Dort verdeckt sie %d Pixel von Kralle 15 (Ihre Schleife 3), rund ein Drittel: Kralle 15 zeigt dann %d statt '
    '%d Pixel (Ihre Schleife 3 selbst bleibt frei). Dazu verdeckt sie %d Pixel Hand 13, und %d Ecken stecken in der Hand. So zurück, oder an Ihre Schleife 4 '
    'anpassen?' % (J2V['28']['sichtbar_px'], J2V['28']['in_schleife_px']['4'], J2V['28']['im_fussabschnitt_ohne_schleifen_px'], V28.get('15', 0),
                   J2V['15']['sichtbar_px'], J2A['15']['sichtbar_px'], V28.get('13', 0), KJ['durchdringung']['28']['ecken_in_der_hand']),
    'Krallen hinten links (Tafel 21 unten, Tafel 06): Ihre zwei roten Marken liegen rechts neben dem Modellfuß über Hintergrund. Sind die Krallen die drei '
    'Zehenspitzen des Fußes (mein Vorschlag), oder ragen sie weiter heraus?',
    'Zweiter grüner Strich ohne Text (Tafel 04): Er zeigt auf denselben weißen Fleck wie "weiße Musterung". Richtig, oder meinten Sie eine weitere Stelle?',
    'Bräunliche Musterung (Tafeln 04 und 05): Am rechten Hinterbein ist im Bild nichts Braunes messbar (graublau: L* 63,5, a* 7,7, b* -20,7); meinten Sie die '
    'helle Fläche direkt unter dem weißen Fleck? Am linken Hinterbein ist die Stelle dunkel rot-violett (L* 27, a* 26, b* -6): ist das die bräunliche Musterung?',
    'Rotes Fleisch am Zipfel (Tafel 03): Von den 4 Stellen an Ihren Linienenden sind 2 violett-rot, 1 rot und 1 nur schwach rötlich; die erste ist nur 1 Pixel '
    'groß. Sind alle vier "rotes Fleisch"?',
    'Spalt an der Schwanzspitze (Tafel 03): Zwischen Ihren zwei orangen Umrissen ("Schwanz Musterung") liegt ein dunkelroter Spalt, an dem einer Ihrer lila '
    'Striche endet. Die Musterung ist nur das Innere Ihrer zwei Umrisse; als "rotes Fleisch" zähle ich nur die rote Stelle am Ende des lila Strichs. Oder ist '
    'der ganze Spalt rotes Fleisch?',
    'Nicht sichtbare Bereiche (Tafel 20, blasse Farben): Alles, was in keinem der zwei Bilder zu sehen ist (Rückseiten, Unterseiten, Hals-Unterseite, '
    'Kopf-Unterseite, Oberlippe, rechter Hinterfuß): bleibt das Haut ohne eigene Farbe?',
]


def messblatt():
    L = []
    zz = Z['meta']['sicherheit_zaehlung']
    tz = [z for z in zonen if z.get('texel')]
    L += ['# Messblatt Zonen v2: erkannte Grundlage zur Abnahme (Runde 41, Riesenform)', '',
          'Stand 2026-11-09, Korrektor Runde 4 von 4. Nur Messung: kein Asset, keine Textur, keine Farbwahl. Spiel-Asset `re15_port/shared_assets/RE2/BAHNSTEIG_FORM.EMD` '
          '(md5 6e00f105f6ea854b521e23fa1f4b9d5c) und `.TIM` (md5 4668e504198ad2c24a29a3e1ec590448) unverändert. Grundlage v1: `../MESSBLATT_ZONEN.md` (Commit e53cef212), '
          'Prüfung v1 `../PRUEFUNG_ZONEN.md` (Commit d62ae97fa), Prüfungen `PRUEFUNG_GRUNDLAGE_V2_r1.md`, `_r2.md` und `_r3.md`.', '',
          '## Ergebnis', '',
          '- **%d Zonen** aus allen 11 beschrifteten Nutzerbildern (jede Beschriftung, auch der zweite grüne Strich), dazu Abschnitte, je Kralle eine Zone und 5 Kopf-/Halsteile '
          'als Modellgliederung ohne Nutzerwort (Nr. 40, 41, 42, 44, 45). Sicherheit: sicher %d, unsicher %d, verdeckt %d.' % (
              len(zonen), zz.get('sicher', 0), zz.get('unsicher', 0), zz.get('verdeckt', 0)),
          '- **Flecken texelgenau (T1):** %d Zonen (Wunde, Musterungen, Schwanz-Muster, rotes Fleisch, weiße/bräunliche Musterung, großer fleischfarbener Teil, Augen) '
          'sind jetzt Texel-Masken in der Form Ihrer Stelle statt ganzer Flächen; zusammen %d Texel. Rückprobe (Texel-Fleck gegen Ihre Stelle im Bild) IoU %s bis %s.' % (
              len(tz), sum(z['texel']['zahl'] for z in tz), k(min(z['texel']['rueckprobe']['iou'] for z in tz), 2), k(max(z['texel']['rueckprobe']['iou'] for z in tz), 2)),
          '- **Geteilte Texel (T2):** Die heutige G4-Textur benutzt dieselben Texel an vielen Stellen. Bei %d von %d Flecken nutzen andere Flächen dieselben Texel; '
          'größter Fall "großer fleischfarbener Teil": %d weitere Flächen, der Fleck erschiene in Bild 2 zusätzlich auf %d Video-px. Je Zone steht in `zonen_v2.json` '
          '`brauchen_eigene_texel` (nur benannt, nichts umgebaut); Tafel 24.' % (
              len([z for z in tz if z['geteilte_texel']['andere_flaechen']]), len(tz), ZI['fleisch_vorn']['geteilte_texel']['andere_flaechen'],
              ZI['fleisch_vorn']['geteilte_texel']['bild2_px_video']),
          '- **Augen (Z2):** Auge = nur die Texel unter den beiden Nutzerpunkten, eine Liste (`zonen_v2.json` Zone 29 `texel` = `augen.nutzerpunkte[*].auge_texel`). '
          'Rechts (Wesen) %d Texel / %s Video-px, links %d Texel / %s Video-px. Heutige G4-Augen %s bzw. %s Video-px, Mitte %s bzw. %s px neben dem Punkt. '
          'Ohne eigene Augen-Texel erschiene das Auge mit der heutigen Textur in Bild 2 auf %s statt %s Video-px (%s-fach; Tafel 22).' % (
              NP[0]['auge_texel']['zahl'], k(NP[0]['punkt_px_vorlage']), NP[1]['auge_texel']['zahl'], k(NP[1]['punkt_px_vorlage']),
              k(GR[0]['px_vorlage'], 0), k(GR[1]['px_vorlage'], 0), k(NP[0]['heutige_augenflaeche_naechste']['abstand_mitte_px_vorlage']),
              k(NP[1]['heutige_augenflaeche_naechste']['abstand_mitte_px_vorlage']), k(AO['gesamt_px_vorlage'], 0), k(AO['punkte_px_vorlage'], 0),
              k(AO['faktor_gesamt_zu_punkten'])),
          '- **Krallen:** vorn links 28 mit K5c-Form zurück; vorn rechts 31 bleibt; 19, 20 und 29 bleiben eingezogen (Fragen). 19 in K5c-Lage läge über Ihrer Hand '
          '(%d Video-px Hand verdeckt; gekürzt x%s noch %d px; %d Drehungen um die Wurzel mindestens %d px). Hinten unverändert. Kandidat: nur Teil %s anders, %d Bytes verschieden. Die zurückgeholte 28 verdeckt in Bild 2 %d px von Kralle 15 (sichtbar %d statt %d) und %d px Hand 13 (Runde 4: gezählt im fertigen Bild).' % (
              K5_19['verdeckt_sichtbare_hand18_px'], k(A19['laengenfaktor'], 2), A19['verdeckt_sichtbare_hand18_px'], K19['B_loesungen'], B19['verdeckt_sichtbare_hand18_px'],
              KJ['emd']['meshes_mit_anderen_ecken'], KJ['emd']['bytes_verschieden'], V28.get('15', 0), J2V['15']['sichtbar_px'], J2A['15']['sichtbar_px'], V28.get('13', 0)),
          '- **Maul (Runde 4):** "geöffnetes Maul" nach LAGE: %d Flächen, die erst beim Öffnen sichtbar werden. Vergleich jetzt mit wirklich geschlossenem Maul '
          '(Clip 5/0, 7/0, 2/0: Kieferwinkel %s / %s / %s Grad; offen = Clip 12/0 = Bild 2, %s Grad). Runde 3 nahm Clip 0/0 als "zu", dort steht das Maul aber %s Grad offen; '
          'dadurch fehlten %s (vorderer Mundboden und Gaumen). In Bild 2 zeigt Zone 30 jetzt %s Video-px (Runde 3: 448), davon %s in Ihrem Umriss. Die Oberlippe '
          '(Zone 42) ist jetzt nach Lage bestimmt (%d Flächen: Rand der Maul-Öffnung am Kopf, auch bei geschlossenem Maul außen), nicht mehr aus der Farbliste '
          '"Lippensaum" (die enthielt Gaumen). Farbliste "Maulinneres": die 10 Kopf-Unterseiten-Flächen sind Zone 45 (Haut nicht belegt); im ganzen Modell sind '
          'davon nur %d von unten zu sehen (%s, die roten Hals-Streifen; %s bis %s von %d Ansichten), %d nie (%s, vom Hals verdeckt).' % (
              len(ML['maulinneres_lage']), k(KW['5'], 2), k(KW['7'], 2), k(KW['2'], 2), k(KW['12'], 2), k(KW['0'], 2), _kurz(MAUL_NEU), k(MAUL_PX, 0), k(MAUL_IN, 0),
              len(ML['oberlippe_lage']), len(Z45_SICHT), _kurz(Z45_SICHT), min(KU['flaechen'][f]['ansichten_sichtbar'] for f in Z45_SICHT),
              max(KU['flaechen'][f]['ansichten_sichtbar'] for f in Z45_SICHT), KU['ansichten'], len(Z45_NIE), _kurz(Z45_NIE)),
          '- **Schwanzspitze (Runde 3):** Zone 11 ist je oranger Umriss eine eigene Fläche; der dunkelrote Spalt dazwischen ist heraus. Zone 11 und Zone 12 überlappen nicht mehr '
          '(0 px); %d Texel statt 446.' % Z11['texel']['zahl'],
          ('- **Texel-Konflikte (Runde 3):** %d Paare Ihrer Zonen mit verschiedenem Material beanspruchen dieselben Texel (größtes: %s/%s %d Texel); Tabelle unten und Tafel 24.' % (
              len(KONF), KONF[0]['zone_a'], KONF[0]['zone_b'], KONF[0]['texel'])) if KONF else '- Texel-Konflikte: keine.',
          '- **Kopf (Bild 2):** Modell minus Nutzerumriss oben %s, unten %s, links %s, rechts %s Video-px; nur gemessen.' % tuple(
              ('%+.1f' % kopf['modell_minus_nutzer_vorlage_px'][x]).replace('.', ',') for x in ('oben', 'unten', 'links', 'rechts')), '']
    L += ['## Verfahren', '',
          '- Ausrichtung wie v1 (Faktor 2, keine Drehung/Spiegelung; `abbildung.json`). Kennungs-Render in Grundlage-Koordinaten (Kameramatrix verkettet mit '
          'Vorlage -> Grundlage). Pixelzahlen im Video-Maßstab (Grundlage-px x s², s = 0,5005 bzw. 0,5010).',
          '- Umrisse: Bauarten flut/huelle/loecher/band aus v1 (`skripte/g_bau.py`).',
          '- Stellen neben Linienenden (Memory reai-v2-beschriftungslinien-messen): Linienende per Skelett; `fleck` = Zusammenhangskomponente einer Klasse '
          '(weiß L* >= 70 und C* <= 30; Auge L* >= Median(21 px) + 12) am Ende; `wachsen` = Farbgebiet ab einer Saat 4-6 px hinter dem Ende in Linienrichtung '
          '(Delta E <= 14-20), Runde 2: je Ende genau eine Komponente (die am saatnächsten Pixel).',
          '- **Texel-Maske (T1, `skripte/g_texel.py`):** Ausschnitt um die Stelle (+12 Grundlage-px), 5 x 5 Abtastpunkte je Grundlage-Pixel (die Pixelmitte ist einer davon), '
          'je Abtastpunkt baryzentrisch u, v und CLUT-Zeile (`v4f.rastern_id`). Mehrheitsregel je (Fläche, Texel): das Texel gehört zur Stelle, wenn mindestens die '
          'Hälfte der Abtastpunkte, mit denen diese Fläche das Texel zeigt, in der Stelle liegt (angeschnittene Randtexel vergrößern die Stelle nicht). '
          'Lückenschluss einmal: nicht getroffenes Texel nur, wenn alle 4 Nachbarn getroffen sind und es im UV-Bereich einer Trägerfläche liegt (insgesamt %d Texel). '
          'Kein Glätten. Teilfilter nur bei Augen (Kopf 22) und "großer fleischfarbener Teil" (Brust 10 / Hals 21, Render ohne Kopf 22 und Unterkiefer 23, Z4). '
          'Texel-Format `[Seite, CLUT-Zeile, u, v]`, u = Spalte in der TIM (512 x 256, 8 Bit) einschließlich 128 x Seite.' % sum(z['texel']['lueckenschluss'] for z in tz),
          '- **Geteilte Texel (T2):** UV-Bereich jeder Fläche (4 x 4 Proben je Texel, floor wie im Render). Andere Flächen mit einem Fleck-Texel im UV-Bereich; gezählt, wo '
          'der Fleck dadurch zusätzlich erscheinen würde: Bild 1 und 2 (Video-px) und 5 Rundum-Ansichten (Pixel der Rundum-Ansicht, 760 x 760).',
          '- Teil-Zonen: Flächen der benannten Teile, Fläche >= 50 % in der Zone oder >= 15 % der Modellpixel der Zone. Kopf Bild 2 (Z4) nach Modellbedeutung.',
          '- **Maul und Oberlippe nach Lage (Runde 4, `skripte/g_maul.py`):** Die material.json-Listen "Maulinneres" und "Lippensaum" sind FARBlisten, keine '
          'Lagelisten. Lage: Kopf 22 + Unterkiefer 23 allein, 84 Richtungen (Gier 0-330 in 30er-Schritten x Höhe -75/-45/-20/0/20/45/75), sichtbare Pixel je Fläche. '
          'Posen: offen Clip 12/0 (= Bild 2), zu Clip 5/0, 7/0 und 2/0 (Kieferwinkel = Drehung Unterkiefer gegen Kopf, gemessen wie `streifen/skripte/r3_lage.py`). '
          'Maulinneres = von vorn (Gier 60-120, Höhe <= 20) >= 30 px bei offenem Maul UND in JEDER Zu-Pose < 30 %% der Offen-Sichtbarkeit; Zähne bleiben Zone 41. '
          'Oberlippe = Fläche von Kopf 22, die eine Kante (gleiche Ecken im Knochenraum) mit Maulinnerem oder Zahn teilt UND in jeder Zu-Pose >= 30 %% der '
          'Offen-Sichtbarkeit (> 0 px) zeigt. Grenzfälle (Zu-Anteil 30-45 %%): %s. Farbliste "Maulinneres": 14 Flächen in Augenhöhe (Knochen-y < -200) = Zone 40; '
          'übrige Kopf-Flächen = Zone 45; übrige Unterkiefer-Flächen = Unterkiefer außen (Zone 31). Zone 45 im ganzen Modell: `skripte/g_kopfunterseite.py` (Clips 0-9, '
          '8 Gier-Richtungen x 4 Höhen von unten = 320 Ansichten). Sichtbarkeit je Fläche: `zonen_v2.json` maul_lage und kopf_unterseite_ganzes_modell.' % (
              ', '.join('%s (%s %%)' % (g['flaeche'].replace('|', '/'), k(100 * g['zu_anteil'], 0)) for g in ML['grenzfaelle_zu_anteil_30_bis_45'])),
          '- **Krallen-Sichtbarkeit (Runde 4, `skripte/g_krallen.py`):** sichtbar = Pixel, die im fertigen Bild des Standes der Kralle gehören (alle Teile, Maler-Reihenfolge '
          'wie das Spiel; Krallen verdecken sich gegenseitig und die Hand). Runde 1-3 zählten gegen ein Tiefenbild ohne alle Krallen (Spalte "ohne andere Krallen").',
          '- **Zone 11 (Runde 3):** je zusammenhängender oranger Linienzug (>= 100 px) eine eigene konvexe Hülle (Strich + Inneres), Zeiger-Reste (52 und 23 px) verworfen; '
          'keine gemeinsame Hülle. Zone 12 (gemessenes Farbgebiet) verliert, wo Ihr oranger Umriss liegt (Ihr Umriss hat Vorrang).',
          '- **IoU** nur gegen eine Nutzerstelle: Teil-Zonen = sichtbare zugeordnete Flächen gegen Ihren Umriss; Flecken = Rückprobe der Texel-Maske gegen die Stelle; '
          'Modellgliederung, Zehen, Krallen-Schleifen/-Formen = "–".', '']
    L += ['## Zonen (je Zone eine Zeile)', '',
          '| Nr | Zone | Bild | Nutzerwort | Material laut Nutzerwort | Modellteile / Texel | Stelle px | IoU | Sicherheit | Grund | Änderung |',
          '|---|---|---|---|---|---|---|---|---|---|---|']
    for z in zonen:
        L.append('| %s | %s | %d | %s | %s | %s | %s | %s | %s | %s | %s |' % (
            z['nr'], z['id'], z['bild'], wort(z).replace('|', '/'), z['material'], teile(z).replace('|', '/'), z.get('zone_px_vorlage', '–'), iou(z),
            z['sicherheit'], z['sicherheit_grund'].replace('|', '/'), (z['aenderung_v1'] or '–').replace('|', '/')))
    L += ['', '## Flecken texelgenau und geteilte Texel (T1, T2)', '',
          '| Nr | Zone | Texel (getroffen + Lücke) | Fläche Video-px (Stelle) | Rückprobe IoU | nicht übernommen (Teilfilter) | andere Flächen mit denselben Texeln | '
          'zusätzlich sichtbar Bild 1 / Bild 2 (Video-px) | Teile, die eigene Texel bräuchten |', '|---|---|---|---|---|---|---|---|---|']
    for z in tz:
        t, g = z['texel'], z['geteilte_texel']
        L.append('| %s | %s | %d (%d + %d) | %d (%d) | %s | %s | %d | %s / %s | %s |' % (
            z['nr'], z['id'], t['zahl'], t['getroffen'], t['lueckenschluss'], t['flaeche_video_px'], t['stelle_video_px'], k(t['rueckprobe']['iou'], 2),
            ', '.join('%s %d' % (v['teil'], v['px_vorlage']) for v in t['verworfen_durch_teilfilter'].values()) or '–', g['andere_flaechen'],
            k(g['bild1_px_video'], 0), k(g['bild2_px_video'], 0),
            ', '.join('%s %s (%d)' % (b, v['teil'], v['flaechen']) for b, v in sorted(g['je_mesh'].items(), key=lambda t: -t[1]['flaechen'])) or '–'))
    L += ['', '### Texel-Konflikte zwischen Zonen (Runde 3)', '',
          'Texel, die zwei Flecken-Zonen mit verschiedenem Material zugleich beanspruchen (gleiche Texel in der heutigen G4-Textur). Eine der beiden Stellen braucht '
          'später eigene Texel. Nur benannt. 9/11 sind zwei ähnliche Wörter ("Musterung/Maserung" und "Musterung").', '',
          '| Zonen | Material | gemeinsame Texel | Flächen mit diesen Texeln (Teile) |', '|---|---|---|---|']
    for kk_ in KONF:
        L.append('| %s / %s | %s gegen %s | %d | %d (%s) |' % (kk_['zone_a'], kk_['zone_b'], kk_['material_a'], kk_['material_b'], kk_['texel'],
                                                            kk_['flaechen_mit_diesen_texeln'], ', '.join(str(t) for t in kk_['teile'])))
    L += ['', '## Änderungen in Runde 4 (Prüfer Runde 3)', '',
          '| Prüfpunkt | Runde 3 | jetzt (Runde 4) | Beleg |', '|---|---|---|---|',
          '| Nr. 5 Zone 30 | 21 Flächen; "zu" = Clip 2/0 und Clip 0/0 (Clip 0: Kieferwinkel %s Grad, Maul halb offen) | %d Flächen; "zu" = Clip 5/0, 7/0, 2/0 (%s / %s / %s Grad); neu %s; in Bild 2 %s statt 448 Video-px, %s in Ihrem Umriss | g_maul.py; maul_lage.kieferwinkel_grad |' % (
              k(KW['0'], 2), len(ML['maulinneres_lage']), k(KW['5'], 2), k(KW['7'], 2), k(KW['2'], 2), _kurz(MAUL_NEU), k(MAUL_PX, 0), k(MAUL_IN, 0)),
          '| Nr. 5 Zone 31 | 77 Flächen | %d Flächen (Mundboden 23/1/26, 53, 54, 55 an Zone 30) | Unterkiefer außen = Teil 23 ohne Maulinneres und Zähne |' % ZI['unterkiefer_vorn']['modell_zuordnung']['flaechen'],
          '| Nr. 5 Zone 42 | 70 Flächen aus der Farbliste "Lippensaum" (mit Gaumen 22/1/40, 49) | %d Flächen nach Lage (Rand der Maul-Öffnung, außen); in Bild 2 %d px | g_maul.py oberlippe_lage |' % (
              len(ML['oberlippe_lage']), ZI['lippe']['modell_zuordnung']['px_sichtbar_vorlage']),
          '| Nr. 6 Zone 45 / Frage 6 | "8 Außenflächen machen die Streifen" | %d (%s), im ganzen Modell %s-%s von %d Ansichten; %s nie sichtbar | g_kopfunterseite.py |' % (
              len(Z45_SICHT), _kurz(Z45_SICHT), min(KU['flaechen'][f]['ansichten_sichtbar'] for f in Z45_SICHT), max(KU['flaechen'][f]['ansichten_sichtbar'] for f in Z45_SICHT),
              KU['ansichten'], _kurz(Z45_NIE)),
          '| Nr. 7 Krallen | sichtbar gegen ein Tiefenbild ohne alle Krallen; 15 im Vorschlag 699 px | im fertigen Bild: 15 %d px (heute %d); 28 verdeckt %d px von 15 und %d px Hand 13; Frage 10 ergänzt | kandidat.json kralle28_verdeckt_bild2 |' % (
              J2V['15']['sichtbar_px'], J2A['15']['sichtbar_px'], V28.get('15', 0), V28.get('13', 0)),
          '| Nr. 12 Tafeln | Farben je Tafel neu vergeben; Zone 30 in Tafel 09 unter der Gesichts-Tönung | eine Farbe je Zone in allen Tafeln; Unter-Zonen kräftiger getönt (50 statt 22 %) | g_tafeln.py globale_farben |',
          '| T6 Tafelreihe | Streifen-Tafel nur im Streifen-Ordner; geteilte Texel = Tafel 23 | Streifen-Tafel = Tafel 23 in tafeln/, geteilte Texel = Tafel 24; Antwort in LIESMICH; Frage 1 mit den Streifen verbunden; je Frage die Tafel | LIESMICH |', '']
    L += ['', '## Änderungen in Runde 3 (Prüfer Runde 2)', '',
          '| Prüfpunkt | Runde 2 | jetzt (Runde 3) | Beleg |', '|---|---|---|---|',
          '| Nr. 3 Zone 11 | eine Hülle über beide orange Umrisse, Spalt eingeschlossen; 446 Texel; 303 px Überlappung mit Zone 12 | je Umriss eigene Hülle; %d Texel; Überlappung 0 px; Konflikt-Texel 11/12 nur noch durch Mehrfachnutzung der Textur: %s | entfernter Spalt 906 Grundlage-px = 227 Video-px, L* 33, a* 25 (dunkelrot); im neuen Umriss L* 76 |' % (
              Z11['texel']['zahl'], next((str(x['texel']) for x in KONF if {x['zone_a'], x['zone_b']} == {'11', '12'}), '0')),
          '| Nr. 11 Zone 30 | 21 Flächen aus der Farbliste, darunter die 6 Hals-Streifen-Flächen | 21 Flächen nach Lage (erst beim Öffnen sichtbar), 0 aus der Farbliste; die 10 Kopf-Flächen der Farbliste = Zone 45 (Haut nicht belegt); Kiefer-Dreiecke 23/0/3-6: von unten nur bei fast geschlossenem Maul sichtbar (Clip 2: 309-372 px in 84 Ansichten, Clip 12: 0-18 px), Lage außen -> Unterkiefer außen (Zone 31) | g_maul.py; maul_lage in zonen_v2.json (in Runde 4 korrigiert, siehe oben) |',
          '| Nr. 12 Kralle 19 | K5c-Lage x0,49 im Kandidaten, lag über der Hand | eingezogen wie im Asset; Variante nur in krallen_kandidat/variante_19_k5c_gekuerzt/ (Tafel 21) | kralle19.json: K5c %d px Hand verdeckt; x0,49 %d px und %d Ecken in der Hand (Wurzelradius %s); %d Drehungen mindestens %d px |' % (
              K5_19['verdeckt_sichtbare_hand18_px'], A19['verdeckt_sichtbare_hand18_px'], A19['ecken_in_hand18'], k(A19['wurzelradius'], 0), K19['B_loesungen'], B19['verdeckt_sichtbare_hand18_px']),
          '| Nr. 8 Augen | Tafel 22 und Frage 3 ohne Hinweis | Hinweis: ohne eigene Texel %s statt %s Video-px | augen.vorschlag.anzeige_bild2_ohne_eigene_texel |' % (k(AO['gesamt_px_vorlage'], 0), k(AO['punkte_px_vorlage'], 0)),
          '| Nr. 10 Texel-Konflikte | nicht benannt | Tabelle oben und Tafel 23 | texel_konflikte in zonen_v2.json |',
          '| Nr. 15 Zone 31 | "Unterkiefer (kein Farbwort)" | "Haut (Teil benannt, kein Farbwort)" | wie die anderen Teilnamen |',
          '| Nr. 16 Tafeln, Fragen | Tafel 03 Spalt als Musterung, Tafel 21 ohne Hinweis, Tafel 23 kleine Schrift; Fragen fehlten | Tafeln neu; Tafel 23 Schrift 27 px; Fragen zu Spalt, Hals-Streifen, Kralle 19 | LIESMICH |',
          '| Nr. 17 Streifen-Bericht | "799 Bilder, seit K4" auch für das Rot | auf das Gemessene begrenzt | streifen/STREIFEN_HERKUNFT.md |', '']
    L += ['', '## Änderungen gegenüber v1 und Runde 1 (mit Grund)', '',
          '| Punkt | vorher | jetzt (Runde 2) | Grund / Beleg |', '|---|---|---|---|',
          '| Z1 weiße Musterung Hinterbein l | v1 Kreis auf dem Linienende, Hüfte 0 | weißer Fleck 5,0 px rechts neben dem Ende (L* 76 gegen 47), texelgenau (%d Texel) | Prüfung v1 IoU 0,06 |' % ZI['weisse_musterung_bein_hinten_links']['texel']['zahl'],
          '| Z2 Augen | v1 Kreise; Runde 1: 3 bzw. 6 ganze Flächen, links 8,7-fach zu groß | nur die Texel unter den Punkten, eine Liste (rechts %d, links %d Texel) | Prüfer r1 Nr. 7; Nutzer: Markierung gilt |' % (
              NP[0]['auge_texel']['zahl'], NP[1]['auge_texel']['zahl']),
          '| Z3 weiße Musterung Hinterbein r | v1 Kreis, 39 %% | ganzer Fleck (L* 90), texelgenau (%d Texel), Hüfte und Oberschenkel 9 | Prüfung v1 |' % ZI['weisse_musterung_bein_hinten_rechts']['texel']['zahl'],
          '| Z4 Kopf Bild 2 | v1 nach Bildlage | fleisch/Oberkörper ohne 22/23; Gesicht/Maul/Unterkiefer nach Bedeutung; Kopfteile ohne Nutzerwort = Modellgliederung, Material "nicht belegt" | Modellkopf 70 px zu hoch; Prüfer r1 Nr. 9 |',
          '| Z5 zweiter grüner Strich | nicht verwendet | Zone 15 auf denselben weißen Fleck (Frage) | Prüfung v1 |',
          '| Flecken allgemein (T1) | ganze Flächen ("komplette Quader") | Texel-Masken in der Form der Stelle, Nahansicht je Fleck | Nutzer 2026-11-09 |',
          '| Geteilte Texel (T2) | nicht geprüft | je Fleck gezählt und benannt (Tafel 24) | Nutzerauftrag T2 |',
          '| Zone 40 | "Maulinneres" (23 px an den Augen) | "Augenhöhle": 14 Flächen um die Augen mit der Maulinneres-Texturstelle, Material nicht belegt | Prüfer r1 Nr. 8: Liste nach Lage getrennt |',
          '| Zonen 41/42 | Material "Zahn" / "Lippe" unter "Ihr Wort" | kein Nutzerwort, Material "nicht belegt"; Zahnflächen nicht mehr in Zone 30 | Prüfer r1 Nr. 9 |',
          '| Zone 43 | "Unterkiefer außen" doppelt zu Zone 31 | entfällt (= Zone 31 "Teil des Unterkiefers") | Doppelung |',
          '| Zone 30 "geöffnetes Maul" | Maulinneres + Zähne, Material "Maul innen" | 21 Flächen Maulinneres, Material "Maul" | Prüfer r1 Nr. 9/14 |',
          '| Zone 7 "Mund?" | Material "Maul innen" | "Maul" | Prüfer r1 Nr. 14 ("innen" war dazuerfunden) |',
          '| Zone 25 "Fuß und Kralle" | Hand als Kralle | Hand = Fuß (Haut), Kralle im Modell nicht sichtbar | Prüfer r1 Nr. 14 |',
          '| Zone 33 | sicher | unsicher (Modell zeigt Hand und Kopf, IoU 0,16) | Prüfer r1 Nr. 16 |',
          '| Zone 16 / 19 bräunlich | 16 sicher; Messblatt-Werte 19 falsch | 16 unsicher (rot-violett); 19 Werte aus den Daten (L* 63,5, a* 7,7, b* -20,7) | Prüfer r1 Nr. 15 |',
          '| Zone 12 rotes Fleisch | 6 Teile bei 4 Enden | je Ende genau ein Farbgebiet; Farben je Stelle im Grund | Prüfer r1 Nr. 15 |',
          '| Krallen vorn links | Schleife 1 Spitze am Ansatz, 39.1 sicher | Spitze = fernster Punkt vom Fußabschnitt (%s gegen Runde 1 %s); 39.1 unsicher; Frage zu 28 | Prüfer r1 Nr. 11 |' % (
              ZI['kralle_vorn_links_schleife1']['spitze']['nutzerspitze_vorlage'], ZI['kralle_vorn_links_schleife1']['spitze']['spitze_runde1_vorlage']),
          '| Krallen-Kandidat | 20/29 gedreht (über Hintergrund, in der Hand) | 20/29 eingezogen wie im Asset (Runde 3 auch 19); nur 28 anders (%d Bytes) | Prüfer r1 Nr. 12/13, r2 Nr. 12 |' % KJ['emd']['bytes_verschieden'],
          '| Krallenformen vorn rechts | "Form 1" / "Form 3" | "Form rechts im Bild" / "Form links im Bild" | Prüfer r1 Nr. 18 |',
          '| IoU-Spalte | bei 21, 22, 40-44 gegen die eigene Zuordnung (1,00) | "–" ohne Nutzerstelle | Prüfer r1 Nr. 17 |', '']
    L += ['## Augen (Z2)', '',
          '| Auge (Wesen) | Linienende | Punkt Grundlage / Video | Abstand zum Ende | Punkt Video-px | Auge-Texel | Trägerflächen (Texel, Art) | heutiges G4-Auge: Video-px, Abstand Mitte, Überlappung |',
          '|---|---|---|---|---|---|---|---|']
    for a, g in zip(NP, GR):
        h = a['heutige_augenflaeche_naechste']
        L.append('| %s | %s | %s / %s | %s | %s | %d | %s | %s, %s px, %s px |' % (
            a['name'], a['linienende_beschriftet'], a['punkt_grundlage'], a['punkt_vorlage'], k(a['abstand_punkt_zu_linienende_px_grundlage']), k(a['punkt_px_vorlage']),
            a['auge_texel']['zahl'], ', '.join('%s %d %s' % (f.replace('|', '/'), c, a['auge_texel']['traeger_art'][f]) for f, c in a['auge_texel']['traeger'].items()),
            k(h['px_vorlage'], 0), k(h['abstand_mitte_px_vorlage']), k(h['ueberlappung_px_vorlage'])))
    L += ['', 'Heutige G4-Augen: %d Flächen, %d in Bild 2 sichtbar (%s Video-px). Vorschlag: Auge = die %d Texel unter den Punkten; die Flächen tragen sie nur. '
          'Augenhöhe Modell %s / Video %s, Augenabstand Modell %s / Video %s Video-px.' % (
              aug['heute_g4_augen']['flaechen'], aug['heute_g4_augen']['sichtbar_bild2'], k(aug['heute_g4_augen']['px_vorlage'], 0), aug['vorschlag']['auge_texel_zahl'],
              ' / '.join(k(v) for v in kopf['augenhoehe_vorlage']['modell_heute']), ' / '.join(k(v) for v in kopf['augenhoehe_vorlage']['video']),
              k(kopf['augenabstand_vorlage_px']['modell_heute']), k(kopf['augenabstand_vorlage_px']['video'])), '']
    L += ['## Krallen-Kandidat (nur `krallen_kandidat/`, nicht im Asset)', '',
          '- `krallen_kandidat/BAHNSTEIG_FORM.EMD` = Spiel-Asset mit ersetzten Ecken/Normalen der Teile %s (%d Bytes verschieden, gleiche Länge; mit `cmp -l` nachgezählt); '
          'TIM unverändert kopiert.' % (KJ['emd']['meshes_mit_anderen_ecken'], KJ['emd']['bytes_verschieden']),
          '- 28: K5c-Ecken (%d von %d Ecken außerhalb der Wurzel stecken in Hand 13, wie in K5c).' % (KJ['durchdringung']['28']['ecken_in_der_hand'], KJ['durchdringung']['28']['ecken_geprueft']),
          '- 19: bleibt eingezogen (Runde 3). Auftrag "K5c-Lage, ein Längenfaktor": Faktor %s (Spitze K5c %s -> %s, Ihr Strichende %s, Rest %s px). Gemessen (`kralle19.json`, '
          'Bild 2, Maler-Reihenfolge wie das Spiel): K5c-Lage %d von %d px in Ihrer Form, verdeckt %d Video-px sichtbare Hand 18; gekürzt %d px in der Form, %d px Hand '
          'verdeckt, %d px in Ihrem Hand-Umriss, %d von %d Ecken in Hand 18 (Wurzelradius 200 x Faktor = %s). Gegenprobe: %d Drehungen um die Wurzel (2 Winkel -90..90 Grad, '
          '3-Grad-Schritte) mit Spitze <= 2 px an Ihrem Strichende: keine ohne Hand und Hintergrund; die beste verdeckt %d px Hand und hat %d Ecken in der Hand. '
          'Die Variante "gekürzt" liegt nur für Tafel 21 in `krallen_kandidat/variante_19_k5c_gekuerzt/`.' % (
              k(k19['laengenfaktor'], 4), k19['spitze_k5c_vorlage'], k19['spitze_variante_vorlage'], k19['nutzer_spitze_vorlage'], k(k19['rest_abstand_px']),
              K5_19['in_ihrer_form_px'], K5_19['ihre_form_px'], K5_19['verdeckt_sichtbare_hand18_px'], A19['in_ihrer_form_px'], A19['verdeckt_sichtbare_hand18_px'],
              A19['in_ihrem_handumriss_px'], A19['ecken_in_hand18'], A19['ecken_geprueft'], k(A19['wurzelradius'], 0), K19['B_loesungen'],
              B19['verdeckt_sichtbare_hand18_px'], B19['ecken_in_hand18']),
          '- 20 und 29: eingezogen wie im Asset. Runde 1 hatte 6000 Drehungen um die Wurzel geprüft; keine lag ganz im Schriftzug ohne Pixel über Hintergrund und ohne '
          'Durchdringung der Hand (bestes Ergebnis 135 / 12 px über Hintergrund, 2 / 1 Ecken in der Hand, in Bild 1 330 / 683 px über Hintergrund). Frage an den Nutzer.',
          '- Schriftzug-Fläche: helle Glyphen (L* >= 75, C* <= 30) im Textblock x 0-261, y 434-479 der Vorlage, 3 px verdickt, 9 px geschlossen (%d px).' % KJ['wasserzeichen_px_vorlage'], '',
          '| Stand (Bild 2: sichtbar im fertigen Bild / ohne andere Krallen / über Hintergrund) | 14 | 15 | 28 | 30 | 19 | 20 | 29 | 31 |', '|---|---|---|---|---|---|---|---|---|']
    for st in ('asset', 'k5c', 'vorschlag', 'variante_19'):
        L.append('| %s | %s |' % (st, ' | '.join('%d / %d / %d' % (KJ['bild2_je_stand'][st][b]['sichtbar_px'], KJ['bild2_je_stand'][st][b]['sichtbar_ohne_andere_krallen_px'],
                                                                KJ['bild2_je_stand'][st][b]['ueber_hintergrund_px'])
                                                for b in ('14', '15', '28', '30', '19', '20', '29', '31'))))
    L += ['', 'Vorschlag gegen heute: die zurückgeholte 28 liegt im fertigen Bild über %d Video-px, die heute anderen Teilen gehören (%s); in Ihrer Schleife 4 '
          'nur %d von %d px, über dem Fußabschnitt (ohne Schleifen) %d px. Kralle 15 im Fußabschnitt heute %d, im Vorschlag %d px; in Ihrer Schleife 3 beide %d px.' % (
              sum(V28.values()), ', '.join('Teil %s: %d' % (b, n) for b, n in sorted(V28.items(), key=lambda t: -t[1])), J2V['28']['in_schleife_px']['4'],
              J2V['28']['sichtbar_px'], J2V['28']['im_fussabschnitt_ohne_schleifen_px'], KJ['kralle15_fussabschnitt']['asset'], KJ['kralle15_fussabschnitt']['vorschlag'],
              J2V['15']['in_schleife_px']['3'])]
    sp = Z['krallen']['spitzen']
    L += ['', 'Spitzen (Video-px, Regel: fernster Umrisspunkt vom Schwerpunkt des Fußabschnitts): ' + '; '.join(
        'Schleife %s %s -> %s (%s)' % (kk[-1], v['nutzerspitze_vorlage'], v['naechste'], ', '.join('%s %s' % (b, k(d)) for b, d in sorted(
            v['abstand_zu_modellspitze_k5c_px'].items(), key=lambda t: t[1])[:2])) for kk, v in sp['vorn_links'].items()) +
        '; Form rechts im Bild, unteres Ende -> 31 %s px; Form links im Bild, unteres Ende -> 19 (K5c) %s px.' % (
            k(sp['vorn_rechts']['form1']['abstand_unteres_ende_zu_spitze_k5c_px']), k(sp['vorn_rechts']['form3']['abstand_unteres_ende_zu_spitze_k5c_px'])), '']
    kk = kopf
    L += ['## Kopfumriss (Bild 2, nur gemessen)', '',
          '| Kante | Nutzerumriss | Modell heute | Modell minus Nutzer | Video |', '|---|---|---|---|---|',
          '| oben | %.1f | %.1f | %+.1f | keine Kante (L*-Profile in `zonen_v2.json` kopf.video_oberkante) |' % (kk['nutzerumriss_bbox_vorlage'][1], kk['modellkopf_bbox_vorlage'][1], kk['modell_minus_nutzer_vorlage_px']['oben']),
          '| unten | %.1f | %.1f | %+.1f | Maul-Unterkante |' % (kk['nutzerumriss_bbox_vorlage'][3], kk['modellkopf_bbox_vorlage'][3], kk['modell_minus_nutzer_vorlage_px']['unten']),
          '| links | %.1f | %.1f | %+.1f | Silhouette in Augenhöhe %.1f, Modell %.1f |' % (kk['nutzerumriss_bbox_vorlage'][0], kk['modellkopf_bbox_vorlage'][0], kk['modell_minus_nutzer_vorlage_px']['links'],
                                                                                     kk['linke_kante'][0]['video_figur_links'], kk['linke_kante'][0]['modellkopf_links']),
          '| rechts | %.1f | %.1f | %+.1f | dunkel auf dunkel, nicht messbar |' % (kk['nutzerumriss_bbox_vorlage'][2], kk['modellkopf_bbox_vorlage'][2], kk['modell_minus_nutzer_vorlage_px']['rechts']),
          '', 'Modellkopf über der Nutzer-Oberkante: %d Video-px. IoU Modellkopf/Nutzerumriss %.3f.' % (kk['modell_ueber_nutzer_oberkante_px_vorlage'], kk['iou_modell_nutzer']), '']
    L += ['## Fragen an den Nutzer', ''] + ['%d. %s' % (i, f) for i, f in enumerate(FRAGEN, 1)] + ['']
    L += ['## Dateien', '', '| Datei | Inhalt |', '|---|---|',
          '| `zonen_v2.json` | meta, %d Zonen (Nutzerwort, Stelle, Material, Flächen bzw. Texel-Maske, geteilte Texel, Sicherheit, Änderung), augen, kopf, krallen.spitzen |' % len(zonen),
          '| `krallen_kandidat/` | BAHNSTEIG_FORM.EMD/.TIM (Kandidat: nur 28 anders), form.json, kandidat.json, kralle19.json, krallen_masken_bild2.npz; variante_19_k5c_gekuerzt/ (nur Tafel 21) |',
          '| `tafeln/` | 00_uebersicht, 01-11 je Nutzerbild (mit Nahansichten der Flecken), 20_rundum, 21_krallen, 22_kopf_augen, 23_streifen_herkunft, 24_geteilte_texel, LIESMICH.txt |',
          '| `skripte/` | g_basis, g_bau, g_fleck, g_spec, g_maul (R3/R4), g_kopfunterseite (R4), g_zonen, g_texel, g_augen_kopf, g_krallen, g_kralle19 (R3), g_krallen_spitzen, g_meta, g_ausgabe, g_tafel_basis, g_tafeln, g_tafeln2, g_messblatt |',
          '', 'Ablauf (Arbeitsordner GRUNDLAGE_ARBEIT mit asset/, k5c/, anno_*.npz, versatz.json, abbildung.json aus dem v1-Lauf): g_maul -> g_kopfunterseite -> g_zonen -> g_texel -> g_augen_kopf -> '
          'g_krallen -> g_krallen_spitzen -> g_kralle19 -> g_ausgabe -> g_tafeln -> g_tafeln2 -> g_messblatt. Python ohne -I.', '']
    open(os.path.join(V2, 'MESSBLATT_ZONEN_V2.md'), 'w', encoding='utf-8').write('\n'.join(L))


def liesmich():
    L = ['GRUNDLAGE RIESENFORM - SO HABE ICH IHRE ZERLEGUNG ERKANNT (v2, Runde 4, 2026-11-09)', '',
         'Bitte prüfen, bevor ich weiterarbeite. Es wurde nichts gebaut und nichts am Spiel geändert.', '',
         'WAS IST NEU GEGENÜBER DER LETZTEN FASSUNG',
         '- Flecken (Musterungen, Wunde, rotes Fleisch, großer fleischfarbener Teil, Augen) sind texelgenau: nur die Bildpunkte der Textur unter Ihrer Stelle,',
         '  in der Form wie im Video - keine ganzen Flächen ("Quader"). Je Fleck gibt es eine Nahansicht.',
         '- "geöffnetes Maul" (Zone 30): die Flächen, die erst beim Öffnen des Mauls zu sehen sind, verglichen mit wirklich geschlossenem Maul.',
         '  Dazugekommen sind der vordere Mundboden und zwei Gaumenflächen; in Bild 2 zeigt das Maul jetzt %s statt 448 Video-Pixel.' % k(MAUL_PX, 0),
         '- Oberlippe (Zone 42): jetzt der Rand der Maul-Öffnung (%d Flächen), in Bild 2 nicht zu sehen.' % len(ML['oberlippe_lage']),
         '- Kralle 28 vorn links: zurückgeholt verdeckt sie rund ein Drittel von Kralle 15 (Ihre Schleife 3); das steht jetzt in Tafel 21 und Frage 10.',
         '- Jede Zone hat in allen Tafeln dieselbe Farbe und Nummer.',
         '- Neu in der Reihe: Tafel 23 (Woher kommen die Streifen?). Die Tafel zu geteilten Texeln heißt jetzt 24.', '',
         'DIE STREIFEN (Tafel 23, nur gemessen, nichts geändert)',
         '- Grüne Streifen auf dem Buckel vorn: Das ist der Kopf des Modells. Er steckt mit dem Hinterkopf im Buckel und im Hals (schon seit Stand K4).',
         '  Das Modell wird ohne Tiefenspeicher gemalt: jedes Dreieck nach seiner mittleren Entfernung, das fernste zuerst. Deshalb erscheinen Stücke des Kopfes',
         '  über dem Buckel. Grün ist die Farbe des Kopfes in Tafel 23. Im Spiel passiert dasselbe, wegen der dunklen Haut aber nur schwach sichtbar.',
         '  Kein Fehler der Tafel und keine Textur. Ein tieferer oder kleinerer Kopf würde das ändern - siehe Frage 1.',
         '- Rote Streifen am Hals (im Spiel): 6 Außenflächen an der Kopf-Unterseite hinter dem Unterkiefer tragen seit Stand K5c die rote Maul-Farbe.',
         '  Sie liegen außen, nicht im Maul, und gehören nicht zu Ihrem "geöffneten Maul". In der Haltung von Bild 2 sind sie nicht zu sehen. Siehe Frage 6.',
         '- Hinweis: Tafel 23 wurde vor der letzten Maul-Korrektur gezeichnet; in Feld 10 steht deshalb "alle 21 Flächen" - jetzt sind es %d.' % len(ML['maulinneres_lage']), '',
         'WIE LESEN',
         '- Jede Tafel 01-11 gehört zu einem Ihrer Bilder: links Ihr Bild unverändert, in der Mitte das Modell in derselben Haltung und demselben Ausschnitt,',
         '  rechts beides übereinander. Darunter eine Vergrößerung, dann je Fleck eine Nahansicht, dann die Liste: Nummer, Ihr Wort, Modellteil, Material, Sicherheit.',
         '- Kräftig gefüllt = Fleck (texelgenau, Farbe = Material). Umriss mit Tönung = ganzes Teil oder Abschnitt, das Sie benannt haben (Teile innerhalb',
         '  eines Teils, z. B. das Maul im Gesicht, kräftiger getönt). Gestrichelt = die Stelle, die ich in Ihrem Bild gelesen habe.',
         '  Schraffiert = an dieser Stelle zeigt das Modell nichts Passendes.',
         '- Texel = ein Bildpunkt der Textur des Modells. Video-Pixel = Bildpunkt im 640 x 480-Video.',
         '- Seiten (links/rechts) immer aus Sicht des Wesens.',
         '- Sicherheit: "sicher" = gemessen und passt; "unsicher" = Sie selbst schreiben "wahrscheinlich", oder das Modell weicht ab; "verdeckt" = im Modell nicht sichtbar.',
         '- Rote Schrift in der letzten Spalte = anders als in einer früheren Fassung.', '',
         'DIE TAFELN',
         '00_uebersicht.png         beide Bilder, alle Zonen auf einen Blick',
         '01-11                     je eines Ihrer beschrifteten Bilder, mit Nahansicht jedes Flecks',
         '20_rundum.png             das Modell von vorn, hinten, links, rechts, schräg oben: wie weit jede Zone reicht; blass = nicht sichtbar, bleibt Haut',
         '21_krallen.png            Krallen vorn links, vorn rechts, hinten: heute | vor dem Einziehen (K5c) | (vorn rechts: gekürzt, verworfen) | Vorschlag',
         '22_kopf_augen.png         Ihre Augenpunkte, die heutigen Modellaugen, der Vorschlag, der Vorschlag mit der heutigen Textur, und der Kopfumriss',
         '23_streifen_herkunft.png  woher die grünen Streifen auf dem Buckel und die roten Streifen am Hals kommen',
         '24_geteilte_texel.png     wo ein Fleck heute zusätzlich erscheinen würde, weil die Textur dieselben Texel mehrfach benutzt; unten: Texel, die zwei Ihrer',
         '                          Zonen mit verschiedenem Material zugleich brauchen (nur gezeigt, nichts geändert)', '',
         'HINWEIS ZUR TEXTUR (Tafel 24): Die heutige Textur benutzt dieselben Texel für viele Flächen. Ein Fleck nur an Ihrer Stelle braucht später für die dort',
         'genannten Flächen eigene Texel; sonst erschiene er an mehreren Stellen. Beim "großen fleischfarbenen Teil" betrifft das %d weitere Flächen.' % ZI['fleisch_vorn']['geteilte_texel']['andere_flaechen'], '',
         'IHR WORT -> SO ERKANNT']
    for z in zonen:
        if z.get('texel'):
            ziel = 'Fleck texelgenau, %d Texel auf %s' % (z['texel']['zahl'], ', '.join(sorted({TEILNAME[int(f.split('|')[0])] for f in z['texel']['traegerflaechen']})))
        else:
            ziel = ', '.join('%s' % TEILNAME[int(b)] for b in z['modell_zuordnung']['je_mesh']) or 'keine sichtbare Fläche'
        L.append('%-5s %s -> %s; %s [%s]' % (z['nr'], wort(z), ziel, z['material'], z['sicherheit']))
    L += ['', 'FRAGEN AN SIE'] + ['%d. %s' % (i, f) for i, f in enumerate(FRAGEN, 1)] + ['']
    open(os.path.join(TAF, 'LIESMICH.txt'), 'w', encoding='utf-8').write('\n'.join(L))


if __name__ == '__main__':
    messblatt(); liesmich()
    json.dump(FRAGEN, open(os.path.join(ARBEIT, 'fragen.json'), 'w', encoding='utf-8'), ensure_ascii=False, indent=1)
    print('ok')
