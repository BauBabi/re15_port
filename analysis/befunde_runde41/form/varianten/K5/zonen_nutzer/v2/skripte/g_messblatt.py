"""MESSBLATT_ZONEN_V2.md und LIESMICH.txt aus zonen_v2.json + kandidat.json. Aufruf: python g_messblatt.py <v2-ordner> <tafel-ordner>"""
import json, os, sys
from g_basis import TEILNAME, ARBEIT

V2 = sys.argv[1]; TAF = sys.argv[2]
Z = json.load(open(os.path.join(V2, 'zonen_v2.json'), encoding='utf-8'))
KJ = json.load(open(os.path.join(V2, 'krallen_kandidat', 'kandidat.json'), encoding='utf-8'))
zonen = Z['zonen']; ZI = {z['id']: z for z in zonen}
aug, kopf = Z['augen'], Z['kopf']


def teile(z):
    zu = z['modell_zuordnung']
    t = ', '.join('%s %s (%d)' % (b, TEILNAME[int(b)], n) for b, n in zu['je_mesh'].items()) or '–'
    if zu.get('sichtbar') is not None and zu['sichtbar'] != zu['flaechen']:
        t += '; sichtbar %d' % zu['sichtbar']
    return t


def iou(z):
    m = z.get('messung', {})
    return '%.2f' % m['iou_zu_umrandung'] if 'iou_zu_umrandung' in m else '–'


FRAGEN = [
    'Kopfhöhe (Bild 2): Der Modellkopf reicht 70 Video-Pixel höher als Ihr Gesichtsumriss (oben y 196 statt 265). Im Video ist die Kopfoberkante nicht als Kante zu sehen '
    '(alles gleich dunkel). Gilt Ihre rote Linie oben als Kopfoberkante, d. h. ist das, was das Modell darüber zeigt, Brust ("großer fleischfarbener Teil")?',
    'Kopf von hinten (Bild 1): An der Stelle Ihres Kopf-Umrisses zeigt das Modell den rechten Oberarm, der Kopf ist dahinter verdeckt. Soll der Kopf dort sichtbar sein?',
    'Maul (Bild 2): An der Stelle Ihres "geöffneten Mauls" zeigt das Modell vor allem die Außenseite des Unterkiefers; das Maulinnere ist in dieser Pose fast unsichtbar (23 px). '
    'Ich ordne "geöffnetes Maul" dem Maulinneren und den Zähnen zu. Richtig?',
    'Krallen vorn rechts: Ihre Annahme "wahrscheinlich auch 4" lässt sich mit bloßem Drehen von Kralle 20 und 29 nicht erfüllen: In 6000 geprüften Drehungen liegt keine ganz '
    'hinter dem Schriftzug; die beste lässt noch %d bzw. %d px über dem Hintergrund stehen und steckt mit %d bzw. %d Ecken in der Hand, und in Bild 1 stünden sie dann sichtbar ab '
    '(%d bzw. %d px). Sollen 20 und 29 eingezogen bleiben (wie heute) oder kürzer zurückkommen?' % (
        KJ['drehungen']['20']['vorschlag']['ueber_hintergrund_px'], KJ['drehungen']['29']['vorschlag']['ueber_hintergrund_px'],
        KJ['drehungen']['20']['vorschlag']['durchdringung_ecken'], KJ['drehungen']['29']['vorschlag']['durchdringung_ecken'],
        KJ['bild1_kontrolle']['20']['vorschlag']['ueber_hintergrund_px'], KJ['bild1_kontrolle']['29']['vorschlag']['ueber_hintergrund_px']),
    'Kralle 19 (Ihre Form 3): Mit einem einzigen Längenfaktor (x%.2f) liegt die Spitze noch %.0f px neben Ihrem Strichende. Endet die Kralle an Ihrem Strichende, oder läuft sie '
    'hinter der Schrift weiter (dann bliebe die K5c-Länge)?' % (KJ['kralle19']['laengenfaktor'], KJ['kralle19']['rest_abstand_px']),
    'Vierte linke Kralle (Schleife 2): Kralle 14 (mein Vorschlag, weil die linke Hand genau 4 Krallenteile hat) oder die Handspitze 13?',
    'Krallenmarken hinten links: Ihre zwei roten Marken liegen rechts neben dem Modellfuß über Hintergrund. Sind die Krallen die drei Zehenspitzen des Fußes (mein Vorschlag), '
    'oder ragen sie weiter heraus?',
    'Zweiter grüner Strich (linkes Hinterbein, ohne Text): Er zeigt auf denselben weißen Fleck wie "weiße Musterung". Richtig, oder meinten Sie eine weitere Stelle?',
    'Bräunliche Musterung am rechten Hinterbein: Im Bild ist dort nichts Braunes messbar (graublau). Meinten Sie die helle Fläche direkt unter dem weißen Fleck, entlang Ihres '
    'zweiten dunkelroten Strichs?',
    'Alles, was in keinem der zwei Bilder zu sehen ist (Rückseiten, Unterseiten, Hals-Unterseite, rechter Hinterfuß): bleibt das Haut ohne eigene Farbe?',
]


def messblatt():
    L = []
    zz = Z['meta']['sicherheit_zaehlung']
    L += ['# Messblatt Zonen v2: erkannte Grundlage zur Abnahme (Runde 41, Riesenform)', '',
          'Stand 2026-11-09, Korrektor Runde 1 von 2. Nur Messung: kein Asset, keine Textur, keine Farbwahl. Spiel-Asset `re15_port/shared_assets/RE2/BAHNSTEIG_FORM.EMD` '
          '(md5 6e00f105..., K5e_f1) unverändert. Grundlage v1: `../MESSBLATT_ZONEN.md` (Commit e53cef212), Prüfung `../PRUEFUNG_ZONEN.md` (Commit d62ae97fa).', '',
          '## Ergebnis', '',
          '- **%d Zonen** aus allen 11 beschrifteten Nutzerbildern (jede Beschriftung, auch der zweite grüne Strich), dazu die Abschnitte, je Kralle eine Zone und 5 Kopfteile nach '
          'Modellbedeutung (Z4). Sicherheit: sicher %d, unsicher %d, verdeckt %d.' % (len(zonen), zz.get('sicher', 0), zz.get('unsicher', 0), zz.get('verdeckt', 0)),
          '- **Augen (Z2):** Beide Nutzerpunkte gemessen (hell neben dem Linienende). Rechtes Auge (Wesen): Punkt %.1f px neben dem Linienende, heutige G4-Augenfläche %.1f Video-px '
          'daneben und überlappend. Linkes Auge: Punkt %.1f px neben dem Linienende, heutige Augenfläche %.1f Video-px daneben (nach innen/oben), keine Überlappung. '
          'Vorschlag: Flächen unter den Punkten = Auge (%s).' % (
              aug['nutzerpunkte'][0]['abstand_punkt_zu_linienende_px_grundlage'], aug['nutzerpunkte'][0]['heutige_augenflaeche_naechste']['abstand_mitte_px_vorlage'],
              aug['nutzerpunkte'][1]['abstand_punkt_zu_linienende_px_grundlage'], aug['nutzerpunkte'][1]['heutige_augenflaeche_naechste']['abstand_mitte_px_vorlage'],
              ', '.join(ZI['augen_vorn']['modell_zuordnung']['liste'])),
          '- **Krallen:** vorn links 28 zurück (K5c-Form; Spitze 8,3 px neben Schleife 4). Vorn rechts 19 mit Faktor x%.4f (Rest %.1f px); 20 und 29 durch Drehen **nicht** '
          'versteckbar (Frage). Hinten unverändert; Vorschlag Krallen = 3 Zehenspitzen.' % (KJ['kralle19']['laengenfaktor'], KJ['kralle19']['rest_abstand_px']),
          '- **Kopf (Bild 2):** Modell minus Nutzerumriss oben %+.1f, unten %+.1f, links %+.1f, rechts %+.1f Video-px. Video-Oberkante nicht messbar (keine Helligkeitskante). '
          'Augenhöhe passt (Modell %.1f px höher), Augenabstand Modell %.1f gegen Video %.1f.' % (
              kopf['modell_minus_nutzer_vorlage_px']['oben'], kopf['modell_minus_nutzer_vorlage_px']['unten'], kopf['modell_minus_nutzer_vorlage_px']['links'],
              kopf['modell_minus_nutzer_vorlage_px']['rechts'],
              sum(kopf['augenhoehe_vorlage']['video']) / 2 - sum(kopf['augenhoehe_vorlage']['modell_heute']) / 2,
              kopf['augenabstand_vorlage_px']['modell_heute'], kopf['augenabstand_vorlage_px']['video']), '']
    L += ['## Verfahren', '',
          '- Ausrichtung wie v1 (Faktor 2, keine Drehung/Spiegelung; `abbildung.json`). Kennungs-Render **hochaufgelöst in Grundlage-Koordinaten** '
          '(Kameramatrix verkettet mit Vorlage -> Grundlage; Gegenprobe Silhouette IoU 0,9956 Bild 1, 0,978 Bild 2 - Rest = Modellteile links außerhalb des Grundlage-Rahmens). '
          'Pixelzahlen im Vorlage-Maßstab (Grundlage-px x s²).',
          '- Umrisse: Bauarten flut/huelle/loecher/band unverändert aus v1 (`skripte/g_bau.py` = Kopie von `zonen_bau.py`).',
          '- **Stellen neben Linienenden** (Memory reai-v2-beschriftungslinien-messen): Linienende per Skelett; dann `fleck` = Zusammenhangskomponente einer Klasse '
          '(weiß L* >= 70 und C* <= 30; Auge L* >= Median(21 px) + 12) nahe dem Ende, oder `wachsen` = Farbgebiet ab einer Saat 4-6 px hinter dem Ende in Linienrichtung '
          '(Delta E <= 14-20). Keine Kreise mehr.',
          '- **Zuordnung:** Teile-Zonen nur Flächen der benannten Teile, Fläche >= 50 % in der Zone oder >= 15 % der Modellpixel der Zone. Stellen: Kern der Stelle (1 px '
          'abgetragen), je Stelle Flächen mit >= 10 % des Kerns; Material nur an der Stelle, nicht auf der ganzen Fläche. Kopf Bild 2 (Z4): nach Modellbedeutung '
          '(`ruhe/messung/material.json`: Auge, Maulinneres, Zahn, Lippensaum; Unterkiefer außen = Teil 23 ohne Maulinneres/Zahn; Hals-Unterseite = Teil 21 mit Normalen '
          'nach unten >= 0,5). Krallen: je Nutzerschleife das Krallenteil mit der nächsten Spitze (K5c, Pose Bild 2).',
          '- **IoU** = zugeordnete Flächen (sichtbare Pixel) gegen die eigene Umrandung. Bei Stellen ist sie klein, weil eine Fläche größer ist als der Fleck.',
          '- Sicherheit: sicher = Umriss/Stelle gemessen und Modell trifft das benannte Teil; unsicher = Nutzer selbst unsicher, Annahme, oder Modell weicht ab; '
          'verdeckt = an der Stelle im Modell nichts Passendes sichtbar.', '']
    L += ['## Zonen (je Zone eine Zeile)', '',
          '| Nr | Zone | Bild | Nutzerwort | Material laut Nutzerwort | Modellteile (Flächen) | Zone px | IoU | Sicherheit | Grund | Änderung ggü. v1 |',
          '|---|---|---|---|---|---|---|---|---|---|---|']
    for z in zonen:
        L.append('| %s | %s | %d | "%s" | %s | %s | %s | %s | %s | %s | %s |' % (
            z['nr'], z['id'], z['bild'], z['nutzerwort'].replace('|', '/'), z['material'], teile(z), z.get('zone_px_vorlage', '–'), iou(z),
            z['sicherheit'], z['sicherheit_grund'].replace('|', '/'), z['aenderung_v1'] or '–'))
    L += ['', '## Änderungen gegenüber v1 (mit Grund)', '',
          '| Punkt | v1 | v2 | Grund / Beleg |', '|---|---|---|---|',
          '| Z1 weiße Musterung Hinterbein l | Kreis r 8 auf dem Linienende 352/209, Hüfte 0 (33 px) | weißer Fleck 5,0 px rechts neben dem Ende (L* 76 gegen 47), Kern auf Oberschenkel 8 (Fläche 8|1|20) | Prüfung IoU 0,06; Fleck selbst gemessen |',
          '| Z2 Augen | 2 Kreise r 5, 93 % Haut, Hüfte 0 dabei | 2 gemessene Punkte (5,1 / 2,2 px neben den Enden), Flächen 22|0|82, 22|1|112, 22|1|113 | Nutzer: Augen-Markierung gilt |',
          '| Z3 weiße Musterung Hinterbein r | Kreis, 49 px, nur Hüfte | ganzer Fleck 107 px, Hüfte 0 (0|1|135, 0|1|136) + Oberschenkel 9 (9|1|19) | Prüfung: Kreis deckte 39 %; Kern-Regel: nur 9|1|19 trägt Kernpixel, die zwei weiteren 9er-Flächen nur 2 Randpixel |',
          '| Z4 Kopf Bild 2 | Flächen nach Bildlage (fleisch_vorn mit 4799 px Kopf 22) | fleisch/Oberkörper ohne 22/23 (+7 Flächen hinter dem Modellkopf), Gesicht/Maul/Unterkiefer nach Modellbedeutung, 5 Modellteile (Nr 40-44) | Modellkopf 70 px zu hoch |',
          '| Z5 zweiter grüner Strich | nicht verwendet | Zone 15: zeigt auf denselben weißen Fleck (11,7 px) | Frage an den Nutzer |',
          '| Z6 Kopf hinten / Auge / Mund | Oberarm 16 als Kopf | verdeckt, Kopf nur nach Bedeutung (22/23) | Modellkopf in Bild 1 nicht sichtbar |',
          '| Z6 rotes Fleisch Zipfel | 4 Kreise | 4 rötliche Farbgebiete in Linienrichtung | Stelle neben dem Ende |',
          '| Z6 bräunliche Musterung l/r | Kreis bzw. Band um den Strich | Farbgebiet neben dem Ende (l: a* 27; r: graublau, unsicher) | Stelle neben dem Ende |',
          '| Z6 Fußkrallen hinten | Marken auf Schwanz 5 | 3 Zehenspitzen von Fuß 25 (und 27 nach "Rechts wäre es sicher genauso") | Marken liegen über Hintergrund (0 px auf Fuß 25) |',
          '| Z6 Krallen vorn | je Hand eine Zone | je Nutzerschleife/-form eine Zone mit Krallenteil | Prüfung: rechts 2 Formen, nicht 3 Spitzen |',
          '| Z6 Abschnitte, Schwanz, Wunde, Musterung, Hand-r, Ganzteil, Namensteile | ungeprüft | selbst geprüft; Ganzteil = benannte Teile ganz (Rest "Haut (nicht belegt)"); Abschnitt 1 der Hinterbeine unsicher (Hüfte im Umriss) | Tafeln 01-11 |', '']
    L += ['## Augen (Z2)', '', '| Auge (Wesen) | Linienende (beschriftet) | Punkt Grundlage | Punkt Video | Abstand zum Ende | Flächen unter dem Punkt (px, heute Auge?) | heutige Augenfläche: Abstand Mitte (Video-px), Überlappung |',
          '|---|---|---|---|---|---|---|']
    for a in aug['nutzerpunkte']:
        L.append('| %s | %s | %s | %s | %.1f | %s | %.1f, %d px |' % (a['name'], a['linienende_beschriftet'], a['punkt_grundlage'], a['punkt_vorlage'],
                                                                   a['abstand_punkt_zu_linienende_px_grundlage'],
                                                                   ', '.join('%s %d %s' % (f, v['px_grundlage'], 'ja' if v['heute_auge'] else 'nein') for f, v in a['flaechen_unter_dem_punkt'].items()),
                                                                   a['heutige_augenflaeche_naechste']['abstand_mitte_px_vorlage'], a['heutige_augenflaeche_naechste']['ueberlappung_px_grundlage']))
    L += ['', 'Heutige G4-Augen: %d Flächen, %d in Bild 2 sichtbar (%d Video-px). Vorschlag (Kern-Regel): %s; davon heute schon Auge: %s.' % (
        aug['heute_g4_augen']['flaechen'], aug['heute_g4_augen']['sichtbar_bild2'], aug['heute_g4_augen']['px_vorlage'],
        ', '.join(ZI['augen_vorn']['modell_zuordnung']['liste']), ', '.join(f for f in ZI['augen_vorn']['modell_zuordnung']['liste'] if f in aug['vorschlag_davon_heute_auge']) or '–'), '']
    L += ['## Krallen-Kandidat (nur `krallen_kandidat/`, nicht im Asset)', '',
          '- `krallen_kandidat/BAHNSTEIG_FORM.EMD` = Spiel-Asset mit ersetzten Ecken/Normalen der Teile %s (%d Bytes verschieden, Länge gleich); TIM unverändert kopiert.' % (
              KJ['emd']['meshes_mit_anderen_ecken'], KJ['emd']['bytes_verschieden']),
          '- 28: K5c-Ecken. 19: K5c-Ecken, gestreckt entlang Wurzel->Spitze mit Faktor %.4f; Spitze K5c %s -> %s, Nutzerstrichende %s, Rest %.1f px.' % (
              KJ['kralle19']['laengenfaktor'], KJ['kralle19']['spitze_k5c_vorlage'], KJ['kralle19']['spitze_vorschlag_vorlage'], KJ['kralle19']['nutzer_spitze_vorlage'],
              KJ['kralle19']['rest_abstand_px'])]
    for b in ('20', '29'):
        d = KJ['drehungen'][b]
        L.append('- %s: nur um die Wurzel gedreht (%.1f Grad, Achse %s), Länge %s -> %s. K5c: %d px sichtbar, %d über Hintergrund. Vorschlag: %d sichtbar, %d im Schriftzug, %d über '
                 'Hintergrund, %d von %d Ecken in der Hand. Suche: %d Drehungen, keine mit 0 px außerhalb des Schriftzugs; ohne Durchdringung bestenfalls %s px. **Nicht gelungen.**' % (
                     b, d['drehwinkel_grad'], d['drehachse_knochen'], d['laenge_k5c'], d['laenge_vorschlag'], d['k5c']['sichtbar_px'], d['k5c']['ueber_hintergrund_px'],
                     d['vorschlag']['sichtbar_px'], d['vorschlag']['in_schriftzug_px'], d['vorschlag']['ueber_hintergrund_px'], d['vorschlag']['durchdringung_ecken'],
                     d['vorschlag']['ecken_geprueft'], d['suche']['stichproben'], d['suche']['bestes_ohne_durchdringung_ausserhalb_px']))
    L += ['- Gegenprobe "nur kein Pixel über Hintergrund" (Krallen dürften über der Figur liegen): gleiches Ergebnis (alle Pixel außerhalb des Schriftzugs lagen über Hintergrund).',
          '- Bild 1 mit dem Vorschlag: 20 %d px / 29 %d px über Hintergrund (K5c: %d / %d).' % (
              KJ['bild1_kontrolle']['20']['vorschlag']['ueber_hintergrund_px'], KJ['bild1_kontrolle']['29']['vorschlag']['ueber_hintergrund_px'],
              KJ['bild1_kontrolle']['20']['k5c']['ueber_hintergrund_px'], KJ['bild1_kontrolle']['29']['k5c']['ueber_hintergrund_px']),
          '- Schriftzug-Fläche: helle Glyphen (L* >= 75, C* <= 30) im Textblock x 0-261, y 434-479 der Vorlage, 3 px verdickt, 9 px geschlossen (%d px). Hintergrund = außerhalb der '
          'Nutzer-Freistellung und außerhalb des Schriftzugs.' % KJ['wasserzeichen_px_vorlage'], '',
          '| Stand | 14 | 15 | 28 | 30 | 19 | 20 | 29 | 31 |', '|---|---|---|---|---|---|---|---|---|']
    for st in ('asset', 'k5c', 'vorschlag'):
        L.append('| %s (sichtbar / über Hintergrund) | %s |' % (st, ' | '.join('%d / %d' % (KJ['bild2_je_stand'][st][b]['sichtbar_px'], KJ['bild2_je_stand'][st][b]['ueber_hintergrund_px'])
                                                                     for b in ('14', '15', '28', '30', '19', '20', '29', '31'))))
    sp = Z['krallen']['spitzen']
    L += ['', 'Spitzen (Video-px, selbst gemessen): ' + '; '.join('Schleife %s -> %s (%s)' % (k[-1], v['naechste'], ', '.join('%s %.1f' % (b, d) for b, d in sorted(
        v['abstand_zu_modellspitze_k5c_px'].items(), key=lambda t: t[1])[:2])) for k, v in sp['vorn_links'].items()) +
          '; Form 1 unteres Ende -> 31 %.1f px; Form 3 unteres Ende -> 19 (K5c) %.1f px.' % (
              sp['vorn_rechts']['form1']['abstand_unteres_ende_zu_spitze_k5c_px'], sp['vorn_rechts']['form3']['abstand_unteres_ende_zu_spitze_k5c_px']), '']
    k = kopf
    L += ['## Kopfumriss (Bild 2, nur gemessen)', '',
          '| Kante | Nutzerumriss | Modell heute | Modell minus Nutzer | Video |', '|---|---|---|---|---|',
          '| oben | %.1f | %.1f | %+.1f | keine Kante (L*-Profile in `zonen_v2.json` kopf.video_oberkante) |' % (k['nutzerumriss_bbox_vorlage'][1], k['modellkopf_bbox_vorlage'][1], k['modell_minus_nutzer_vorlage_px']['oben']),
          '| unten | %.1f | %.1f | %+.1f | Maul-Unterkante |' % (k['nutzerumriss_bbox_vorlage'][3], k['modellkopf_bbox_vorlage'][3], k['modell_minus_nutzer_vorlage_px']['unten']),
          '| links | %.1f | %.1f | %+.1f | Silhouette in Augenhöhe %.1f, Modell %.1f |' % (k['nutzerumriss_bbox_vorlage'][0], k['modellkopf_bbox_vorlage'][0], k['modell_minus_nutzer_vorlage_px']['links'],
                                                                                     k['linke_kante'][0]['video_figur_links'], k['linke_kante'][0]['modellkopf_links']),
          '| rechts | %.1f | %.1f | %+.1f | dunkel auf dunkel, nicht messbar |' % (k['nutzerumriss_bbox_vorlage'][2], k['modellkopf_bbox_vorlage'][2], k['modell_minus_nutzer_vorlage_px']['rechts']),
          '', 'Modellkopf über der Nutzer-Oberkante: %d Video-px. IoU Modellkopf/Nutzerumriss %.3f.' % (k['modell_ueber_nutzer_oberkante_px_vorlage'], k['iou_modell_nutzer']), '']
    L += ['## Fragen an den Nutzer', ''] + ['%d. %s' % (i, f) for i, f in enumerate(FRAGEN, 1)] + ['']
    L += ['## Dateien', '',
          '| Datei | Inhalt |', '|---|---|',
          '| `zonen_v2.json` | meta, %d Zonen (Nutzerwort, Stelle, Material, Flächenliste, Ganzteil, Sicherheit, Änderung), augen, kopf, krallen.spitzen |' % len(zonen),
          '| `krallen_kandidat/` | BAHNSTEIG_FORM.EMD/.TIM, form.json, kandidat.json (Parameter, Ecken, Zählungen), krallen_masken_bild2.npz |',
          '| `tafeln/` | 00_uebersicht, 01-11 je Nutzerbild, 20_rundum, 21_krallen, 22_kopf_augen, LIESMICH.txt (Kopie im Entscheidungsordner 21_riesenform_grundlage_zonen) |',
          '| `skripte/` | g_basis, g_bau, g_fleck, g_spec, g_zonen, g_augen_kopf, g_krallen, g_krallen_spitzen, g_meta, g_ausgabe, g_tafel_basis, g_tafeln, g_tafeln2, g_messblatt |',
          '', 'Ablauf (Arbeitsordner mit asset/, k5c/, anno_*.npz, versatz.json, abbildung.json aus dem v1-Lauf): g_zonen -> g_augen_kopf -> g_krallen -> g_krallen_spitzen -> '
          'g_ausgabe -> g_tafeln -> g_tafeln2 -> g_messblatt. Python ohne -I.', '']
    open(os.path.join(V2, 'MESSBLATT_ZONEN_V2.md'), 'w', encoding='utf-8').write('\n'.join(L))


def liesmich():
    L = ['GRUNDLAGE RIESENFORM - SO HABE ICH IHRE ZERLEGUNG ERKANNT (v2, 2026-11-09)', '',
         'Bitte prüfen, bevor ich weiterarbeite. Es wurde nichts gebaut und nichts am Spiel geändert.', '',
         'WIE LESEN',
         '- Jede Tafel 01-11 gehört zu einem Ihrer Bilder: links Ihr Bild unverändert, in der Mitte das Modell in derselben Haltung und demselben Ausschnitt,',
         '  rechts beides übereinander. Darunter eine Vergrößerung und die Liste: Nummer, Ihr Wort, Modellteil, Material, Sicherheit.',
         '- Farbe + Nummer = eine erkannte Zone. Gefüllt = die Modellflächen, die ich zuordne. Gestrichelt = die Stelle, die ich in Ihrem Bild gelesen habe.',
         '  Schraffiert = an dieser Stelle zeigt das Modell nichts Passendes.',
         '- Seiten (links/rechts) immer aus Sicht des Wesens.',
         '- Sicherheit: "sicher" = gemessen und passt; "unsicher" = Sie selbst schreiben "wahrscheinlich", oder das Modell weicht ab; "verdeckt" = im Modell nicht sichtbar.',
         '- Rote Schrift in der letzten Spalte = anders als in der ersten Fassung.', '',
         'DIE TAFELN',
         '00_uebersicht.png   beide Bilder, alle Zonen auf einen Blick',
         '01-11               je eines Ihrer beschrifteten Bilder',
         '20_rundum.png       das Modell von vorn, hinten, links, rechts, schräg oben: wie weit jede Zone reicht; blass = nicht sichtbar, bleibt Haut',
         '21_krallen.png      Krallen vorn links, vorn rechts, hinten: heute | vor dem Einziehen (K5c) | Vorschlag',
         '22_kopf_augen.png   Ihre Augenpunkte, die heutigen Modellaugen, der Vorschlag, und der Kopfumriss', '',
         'IHR WORT -> SO ERKANNT']
    for z in zonen:
        L.append('%-5s "%s" -> %s; %s [%s]' % (z['nr'], z['nutzerwort'], ', '.join('%s' % TEILNAME[int(b)] for b in z['modell_zuordnung']['je_mesh']) or 'keine sichtbare Fläche',
                                               z['material'], z['sicherheit']))
    L += ['', 'FRAGEN AN SIE'] + ['%d. %s' % (i, f) for i, f in enumerate(FRAGEN, 1)] + ['']
    open(os.path.join(TAF, 'LIESMICH.txt'), 'w', encoding='utf-8').write('\n'.join(L))


if __name__ == '__main__':
    messblatt(); liesmich()
    json.dump(FRAGEN, open(os.path.join(ARBEIT, 'fragen.json'), 'w', encoding='utf-8'), ensure_ascii=False, indent=1)
    print('ok')
