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
FRAGEN = [
    'Kopfhöhe (Bild 2): Der Modellkopf reicht 70 Video-Pixel höher als Ihr Gesichtsumriss (oben y 196 statt 265). Im Video ist die Kopfoberkante nicht als Kante zu '
    'sehen (alles gleich dunkel). Gilt Ihre rote Linie oben als Kopfoberkante, d. h. ist das, was das Modell darüber zeigt, Brust ("großer fleischfarbener Teil")?',
    'Kopf von hinten (Bild 1): An der Stelle Ihres Kopf-Umrisses zeigt das Modell den rechten Oberarm, der Kopf ist dahinter verdeckt. Soll der Kopf dort sichtbar sein?',
    'Augen, Größe (Bild 2): Ihre Augenpunkte sind klein: rechts %s, links %s Video-Pixel. Die heutigen Modellaugen sind %s bzw. %s Video-Pixel groß und liegen '
    '%s bzw. %s Pixel daneben. Mein Vorschlag (beide Augen nach derselben Regel): Auge ist NUR der helle Punkt, das übrige heutige Augenfeld wird Haut. '
    'Richtig, oder ist das ganze dunkle Augenfeld um den Punkt das Auge? Sollen beide Augen gleich groß sein (heute ist das linke fast doppelt so groß wie das rechte)? '
    'Wichtig dazu: Die heutige Textur benutzt die Texel unter Ihren Punkten auch an anderen Stellen. Färbt man nur diese Texel, erschiene das Auge in Bild 2 auf '
    '%s statt %s Video-Pixeln (%s-fach, auch auf Kopf, Brust und Hals). "Nur der Punkt" geht also nur, wenn das Auge später eigene Texel bekommt (Tafel 22, rechts).' % (
        k(NP[0]['punkt_px_vorlage']), k(NP[1]['punkt_px_vorlage']), k(GR[0]['px_vorlage'], 0), k(GR[1]['px_vorlage'], 0),
        k(NP[0]['heutige_augenflaeche_naechste']['abstand_mitte_px_vorlage']), k(NP[1]['heutige_augenflaeche_naechste']['abstand_mitte_px_vorlage']),
        k(AO['gesamt_px_vorlage'], 0), k(AO['punkte_px_vorlage'], 0), k(AO['faktor_gesamt_zu_punkten'])),
    'Augenhöhle: 14 Modellflächen rund um die Augen benutzen dieselbe rote Texturstelle wie die heutige Maul-Farbe (in Bild 2 sind 2 davon sichtbar, 23 Video-Pixel, direkt '
    'neben den Augen). In Runde 1 hatte ich sie fälschlich "Maulinneres" genannt. Sollen sie Haut sein (mein Vorschlag, Zone 40)?',
    'Maul (Bild 2): An der Stelle Ihres "geöffneten Mauls" zeigt das Modell vor allem die Außenseite des Unterkiefers und Zähne, weil der Modellkopf anders sitzt. '
    'Als "geöffnetes Maul" nehme ich jetzt die %d Flächen, die erst beim Öffnen des Mauls sichtbar werden (hinter den Zähnen; in Bild 2 sind %d davon zu sehen, '
    'alle in Ihrem Umriss). Die Zähne (Zone 41) haben kein Wort von Ihnen: eigenes Material, wie das Maul, oder Haut?' % (
        len(ML['maulinneres_lage']), ZI['maul_vorn']['modell_zuordnung']['sichtbar']),
    'Rote Streifen an der Hals-Unterseite: %d Flächen an der Kopf-Unterseite hinter dem Unterkiefer tragen heute die rote Maul-Farbe; 8 davon sind die großen '
    'Außenflächen, die im Spiel die "komischen roten Streifen" machen. Sie liegen außen, nicht im Maul. In Runde 2 hatte ich 6 davon dem "geöffneten Maul" '
    'zugeordnet. Jetzt: Haut ohne eigene Farbe (Zone 45). Richtig?' % len(ZI['kopf_unterseite_maulfarbe']['modell_zuordnung']['liste']),
    'Krallen vorn rechts: Ihre Annahme "wahrscheinlich auch 4": Kralle 20 und 29 lassen sich durch bloßes Drehen nicht hinter der Schrift verstecken (6000 Drehungen '
    'geprüft; die beste stand noch über dem Hintergrund, steckte in der Hand und stünde in Bild 1 sichtbar ab). Im Vorschlag bleiben 20 und 29 eingezogen wie heute. '
    'Sollen sie so bleiben, oder kürzer zurückkommen?',
    'Kralle 19 (Ihre Form links im Bild): In ihrer früheren Lage (K5c) liegt Kralle 19 zwar auf Ihrer Form (%d von %d Pixeln), aber VOR Ihrer Hand und verdeckt '
    '%d Video-Pixel Hand, die im Video zu sehen sind. Gekürzt (x%s) verdeckt sie noch %d Pixel Hand; auch um ihre Wurzel gedreht (Spitze an Ihrem Strichende, '
    '%d Lagen geprüft) mindestens %d Pixel. Deshalb bleibt 19 im Vorschlag eingezogen wie heute. Was ist richtig: (a) eingezogen lassen, (b) zurück, obwohl sie dann '
    'über der Hand liegt, oder (c) ist Ihre Form gar keine eigene Kralle, sondern ein Teil der Hand?' % (
        K5_19['in_ihrer_form_px'], K5_19['ihre_form_px'], K5_19['verdeckt_sichtbare_hand18_px'], k(A19['laengenfaktor'], 2), A19['verdeckt_sichtbare_hand18_px'],
        K19['B_loesungen'], B19['verdeckt_sichtbare_hand18_px']),
    'Vorn links, Schleife 2: Kralle 14 (mein Vorschlag, weil die linke Hand genau 4 Krallenteile hat) oder die Handspitze 13? Und Schleife 1: Kralle 30 ist im '
    'Modell fast unsichtbar (2 Pixel); ist sie trotzdem die Kralle dieser Schleife?',
    'Kralle 28 (vorn links, Schleife 4): Zurückgeholt in ihrer früheren Form liegt Kralle 28 nur zu 2 Prozent in Ihrer Schleife, der größte Teil (312 Video-Pixel) '
    'läge über Ihrem "Fußabschnitt", und 2 Ecken stecken in der Hand. Soll 28 so zurückkommen, oder soll ich sie an Ihre Schleife 4 anpassen?',
    'Krallenmarken hinten links: Ihre zwei roten Marken liegen rechts neben dem Modellfuß über Hintergrund. Sind die Krallen die drei Zehenspitzen des Fußes (mein '
    'Vorschlag), oder ragen sie weiter heraus?',
    'Zweiter grüner Strich (linkes Hinterbein, ohne Text): Er zeigt auf denselben weißen Fleck wie "weiße Musterung". Richtig, oder meinten Sie eine weitere Stelle?',
    'Bräunliche Musterung: Am rechten Hinterbein ist im Bild nichts Braunes messbar (L* 63,5, a* 7,7, b* -20,7, also graublau); meinten Sie die helle Fläche direkt '
    'unter dem weißen Fleck, entlang Ihres zweiten dunkelroten Strichs? Am linken Hinterbein ist die Stelle dunkel rot-violett (L* 27, a* 26, b* -6): ist das '
    'die bräunliche Musterung?',
    'Rotes Fleisch am Zipfel: Von den 4 Stellen an Ihren Linienenden sind 2 violett-rot, 1 rot und 1 nur schwach rötlich; die erste ist nur 1 Pixel groß. '
    'Sind alle vier "rotes Fleisch"?',
    'Schwanzspitze (Zipfel): Zwischen Ihren zwei orangen Umrissen ("Schwanz Musterung") liegt ein dunkelroter Spalt. Dort endet einer Ihrer lila Striche '
    '("verschieden farbiges rotes Fleisch"). Der Spalt gehört jetzt nicht mehr zur Musterung; die Musterung ist nur das Innere Ihrer zwei Umrisse. Als "rotes Fleisch" '
    'zähle ich nur die rote Stelle am Ende Ihres lila Strichs (gemessenes Farbgebiet), nicht den ganzen Spalt. Richtig, oder ist der ganze Spalt rotes Fleisch?',
    'Alles, was in keinem der zwei Bilder zu sehen ist (Rückseiten, Unterseiten, Hals-Unterseite, rechter Hinterfuß): bleibt das Haut ohne eigene Farbe?',
]


def messblatt():
    L = []
    zz = Z['meta']['sicherheit_zaehlung']
    tz = [z for z in zonen if z.get('texel')]
    L += ['# Messblatt Zonen v2: erkannte Grundlage zur Abnahme (Runde 41, Riesenform)', '',
          'Stand 2026-11-09, Korrektor Runde 3 von 3. Nur Messung: kein Asset, keine Textur, keine Farbwahl. Spiel-Asset `re15_port/shared_assets/RE2/BAHNSTEIG_FORM.EMD` '
          '(md5 6e00f105f6ea854b521e23fa1f4b9d5c) und `.TIM` (md5 4668e504198ad2c24a29a3e1ec590448) unverändert. Grundlage v1: `../MESSBLATT_ZONEN.md` (Commit e53cef212), '
          'Prüfung v1 `../PRUEFUNG_ZONEN.md` (Commit d62ae97fa), Prüfungen `PRUEFUNG_GRUNDLAGE_V2_r1.md` und `PRUEFUNG_GRUNDLAGE_V2_r2.md`.', '',
          '## Ergebnis', '',
          '- **%d Zonen** aus allen 11 beschrifteten Nutzerbildern (jede Beschriftung, auch der zweite grüne Strich), dazu Abschnitte, je Kralle eine Zone und 5 Kopf-/Halsteile '
          'als Modellgliederung ohne Nutzerwort (Nr. 40, 41, 42, 44, 45). Sicherheit: sicher %d, unsicher %d, verdeckt %d.' % (
              len(zonen), zz.get('sicher', 0), zz.get('unsicher', 0), zz.get('verdeckt', 0)),
          '- **Flecken texelgenau (T1):** %d Zonen (Wunde, Musterungen, Schwanz-Muster, rotes Fleisch, weiße/bräunliche Musterung, großer fleischfarbener Teil, Augen) '
          'sind jetzt Texel-Masken in der Form Ihrer Stelle statt ganzer Flächen; zusammen %d Texel. Rückprobe (Texel-Fleck gegen Ihre Stelle im Bild) IoU %s bis %s.' % (
              len(tz), sum(z['texel']['zahl'] for z in tz), k(min(z['texel']['rueckprobe']['iou'] for z in tz), 2), k(max(z['texel']['rueckprobe']['iou'] for z in tz), 2)),
          '- **Geteilte Texel (T2):** Die heutige G4-Textur benutzt dieselben Texel an vielen Stellen. Bei %d von %d Flecken nutzen andere Flächen dieselben Texel; '
          'größter Fall "großer fleischfarbener Teil": %d weitere Flächen, der Fleck erschiene in Bild 2 zusätzlich auf %d Video-px. Je Zone steht in `zonen_v2.json` '
          '`brauchen_eigene_texel` (nur benannt, nichts umgebaut); Tafel 23.' % (
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
          '(%d Video-px Hand verdeckt; gekürzt x%s noch %d px; %d Drehungen um die Wurzel mindestens %d px). Hinten unverändert. Kandidat: nur Teil %s anders, %d Bytes verschieden.' % (
              K5_19['verdeckt_sichtbare_hand18_px'], k(A19['laengenfaktor'], 2), A19['verdeckt_sichtbare_hand18_px'], K19['B_loesungen'], B19['verdeckt_sichtbare_hand18_px'],
              KJ['emd']['meshes_mit_anderen_ecken'], KJ['emd']['bytes_verschieden']),
          '- **Maul (Runde 3):** "geöffnetes Maul" nach LAGE: %d Flächen, die erst beim Öffnen sichtbar werden (hinter den Zähnen). Die material.json-Liste "Maulinneres" '
          'ist eine Farbliste (heute rote Texturzeile 0), keine Lageliste: keine ihrer 35 Flächen liegt im Maulinneren; 8 davon sind große Außenflächen der Kopf-Unterseite '
          '(%s; die roten Hals-Streifen) -> Zone 45 Haut (nicht belegt).' % (len(ML['maulinneres_lage']), ', '.join(ML['farbliste_maulinneres']['aussen_gross_sichtbar'])),
          '- **Schwanzspitze (Runde 3):** Zone 11 ist je oranger Umriss eine eigene Fläche; der dunkelrote Spalt dazwischen ist heraus. Zone 11 und Zone 12 überlappen nicht mehr '
          '(0 px); %d Texel statt 446.' % Z11['texel']['zahl'],
          ('- **Texel-Konflikte (Runde 3):** %d Paare Ihrer Zonen mit verschiedenem Material beanspruchen dieselben Texel (größtes: %s/%s %d Texel); Tabelle unten und Tafel 23.' % (
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
          '- **Maul nach Lage (Runde 3, `skripte/g_maul.py`):** Die material.json-Liste "Maulinneres" ist eine FARBliste (Flächen mit heute roter Texturzeile 0), keine '
          'Lageliste. Lage: Kopf 22 + Unterkiefer 23 allein, 84 Richtungen (Gier 0-330 in 30er-Schritten x Höhe -75/-45/-20/0/20/45/75), sichtbare Pixel je Fläche, Posen '
          'Clip 12/0 (= Bild 2, Maul offen), Clip 2/0 und Clip 0/0 (Maul fast zu). Maulinneres = von vorn (Gier 60-120, Höhe <= 20) >= 30 px bei offenem Maul UND in Clip 2 '
          'und Clip 0 je < 30 %% der Clip-12-Sichtbarkeit (wird erst durch das Öffnen sichtbar); Zähne bleiben Zone 41. Ergebnis %d Flächen (alle aus der Farbliste '
          '"Lippensaum", hinter den Zähnen). Farbliste "Maulinneres": 14 Flächen in Augenhöhe (Knochen-y < -200) = Zone 40 "Augenhöhle" wie Runde 2; übrige Flächen von '
          'Kopf 22 = Zone 45 (außen, Haut nicht belegt); übrige Flächen von Unterkiefer 23 = Unterkiefer außen (Zone 31). Sichtbarkeit je Fläche: `zonen_v2.json` maul_lage.' % len(ML['maulinneres_lage']),
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
    L += ['', '## Änderungen in Runde 3 (Prüfer Runde 2)', '',
          '| Prüfpunkt | Runde 2 | jetzt (Runde 3) | Beleg |', '|---|---|---|---|',
          '| Nr. 3 Zone 11 | eine Hülle über beide orange Umrisse, Spalt eingeschlossen; 446 Texel; 303 px Überlappung mit Zone 12 | je Umriss eigene Hülle; %d Texel; Überlappung 0 px; Konflikt-Texel 11/12 nur noch durch Mehrfachnutzung der Textur: %s | entfernter Spalt 906 Grundlage-px = 227 Video-px, L* 33, a* 25 (dunkelrot); im neuen Umriss L* 76 |' % (
              Z11['texel']['zahl'], next((str(x['texel']) for x in KONF if {x['zone_a'], x['zone_b']} == {'11', '12'}), '0')),
          '| Nr. 11 Zone 30 | 21 Flächen aus der Farbliste, darunter die 6 Hals-Streifen-Flächen | %d Flächen nach Lage (erst beim Öffnen sichtbar), 0 aus der Farbliste; die 10 Kopf-Flächen der Farbliste = Zone 45 (Haut nicht belegt); Kiefer-Dreiecke 23/0/3-6: von unten nur bei fast geschlossenem Maul sichtbar (Clip 2: 309-372 px in 84 Ansichten, Clip 12: 0-18 px), Lage außen -> Unterkiefer außen (Zone 31) | g_maul.py; maul_lage in zonen_v2.json |' % len(ML['maulinneres_lage']),
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
          '| Geteilte Texel (T2) | nicht geprüft | je Fleck gezählt und benannt (Tafel 23) | Nutzerauftrag T2 |',
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
          '| Stand (Bild 2: sichtbar / über Hintergrund) | 14 | 15 | 28 | 30 | 19 | 20 | 29 | 31 |', '|---|---|---|---|---|---|---|---|---|']
    for st in ('asset', 'k5c', 'vorschlag', 'variante_19'):
        L.append('| %s | %s |' % (st, ' | '.join('%d / %d' % (KJ['bild2_je_stand'][st][b]['sichtbar_px'], KJ['bild2_je_stand'][st][b]['ueber_hintergrund_px'])
                                                for b in ('14', '15', '28', '30', '19', '20', '29', '31'))))
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
          '| `tafeln/` | 00_uebersicht, 01-11 je Nutzerbild (mit Nahansichten der Flecken), 20_rundum, 21_krallen, 22_kopf_augen, 23_geteilte_texel, LIESMICH.txt |',
          '| `skripte/` | g_basis, g_bau, g_fleck, g_spec, g_maul (R3), g_zonen, g_texel, g_augen_kopf, g_krallen, g_kralle19 (R3), g_krallen_spitzen, g_meta, g_ausgabe, g_tafel_basis, g_tafeln, g_tafeln2, g_messblatt |',
          '', 'Ablauf (Arbeitsordner GRUNDLAGE_ARBEIT mit asset/, k5c/, anno_*.npz, versatz.json, abbildung.json aus dem v1-Lauf): g_maul -> g_zonen -> g_texel -> g_augen_kopf -> '
          'g_krallen -> g_krallen_spitzen -> g_kralle19 -> g_ausgabe -> g_tafeln -> g_tafeln2 -> g_messblatt. Python ohne -I.', '']
    open(os.path.join(V2, 'MESSBLATT_ZONEN_V2.md'), 'w', encoding='utf-8').write('\n'.join(L))


def liesmich():
    L = ['GRUNDLAGE RIESENFORM - SO HABE ICH IHRE ZERLEGUNG ERKANNT (v2, Runde 3, 2026-11-09)', '',
         'Bitte prüfen, bevor ich weiterarbeite. Es wurde nichts gebaut und nichts am Spiel geändert.', '',
         'WAS IST NEU GEGENÜBER DER LETZTEN FASSUNG',
         '- Flecken (Musterungen, Wunde, rotes Fleisch, großer fleischfarbener Teil, Augen) sind texelgenau: nur die Bildpunkte der Textur unter Ihrer Stelle,',
         '  in der Form wie im Video - keine ganzen Flächen ("Quader"). Je Fleck gibt es eine Nahansicht.',
         '- Schwanzspitze: Ihre zwei orangen Umrisse ("Schwanz Musterung") sind jetzt zwei getrennte Flächen. Der dunkelrote Spalt dazwischen gehört nicht mehr',
         '  zur Musterung (dort zeigt Ihr lila Strich auf "rotes Fleisch").',
         '- "geöffnetes Maul" sind jetzt die Flächen hinter den Zähnen, die erst beim Öffnen des Mauls sichtbar werden. Die Flächen an der Kopf-Unterseite,',
         '  die heute rot sind (die "komischen roten Streifen" am Hals), gehören NICHT zum Maul; sie bleiben Haut (Zone 45).',
         '- Kralle 19 vorn rechts bleibt eingezogen: zurückgeholt läge sie vor Ihrer Hand (Tafel 21, Frage).',
         '- Augen: Hinweis, dass die heutige Textur die Augen-Texel auch an anderen Stellen benutzt (Tafel 22 rechts).',
         '- Tafel 23 ist größer geschrieben und zeigt jetzt auch, wo zwei Ihrer Zonen dieselben Texel brauchen.',
         '- Die "grünen Streifen" auf dem Buckel vorn: eigener Bericht (streifen/STREIFEN_HERKUNFT.md); in diesen Tafeln ist nichts ausgeblendet oder umgefärbt.', '',
         'WIE LESEN',
         '- Jede Tafel 01-11 gehört zu einem Ihrer Bilder: links Ihr Bild unverändert, in der Mitte das Modell in derselben Haltung und demselben Ausschnitt,',
         '  rechts beides übereinander. Darunter eine Vergrößerung, dann je Fleck eine Nahansicht, dann die Liste: Nummer, Ihr Wort, Modellteil, Material, Sicherheit.',
         '- Kräftig gefüllt = Fleck (texelgenau, Farbe = Material). Dünner Umriss mit leichter Tönung = ganzes Teil oder Abschnitt, das Sie benannt haben.',
         '  Gestrichelt = die Stelle, die ich in Ihrem Bild gelesen habe. Schraffiert = an dieser Stelle zeigt das Modell nichts Passendes.',
         '- Texel = ein Bildpunkt der Textur des Modells. Video-Pixel = Bildpunkt im 640 x 480-Video.',
         '- Seiten (links/rechts) immer aus Sicht des Wesens.',
         '- Sicherheit: "sicher" = gemessen und passt; "unsicher" = Sie selbst schreiben "wahrscheinlich", oder das Modell weicht ab; "verdeckt" = im Modell nicht sichtbar.',
         '- Rote Schrift in der letzten Spalte = anders als in einer früheren Fassung.', '',
         'DIE TAFELN',
         '00_uebersicht.png       beide Bilder, alle Zonen auf einen Blick',
         '01-11                   je eines Ihrer beschrifteten Bilder, mit Nahansicht jedes Flecks',
         '20_rundum.png           das Modell von vorn, hinten, links, rechts, schräg oben: wie weit jede Zone reicht; blass = nicht sichtbar, bleibt Haut',
         '21_krallen.png          Krallen vorn links, vorn rechts, hinten: heute | vor dem Einziehen (K5c) | (vorn rechts: gekürzt, verworfen) | Vorschlag',
         '22_kopf_augen.png       Ihre Augenpunkte, die heutigen Modellaugen, der Vorschlag, der Vorschlag mit der heutigen Textur, und der Kopfumriss',
         '23_geteilte_texel.png   wo ein Fleck heute zusätzlich erscheinen würde, weil die Textur dieselben Texel mehrfach benutzt; unten: Texel, die zwei Ihrer',
         '                        Zonen mit verschiedenem Material zugleich brauchen (nur gezeigt, nichts geändert)', '',
         'HINWEIS ZUR TEXTUR (Tafel 23): Die heutige Textur benutzt dieselben Texel für viele Flächen. Ein Fleck nur an Ihrer Stelle braucht später für die dort',
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
