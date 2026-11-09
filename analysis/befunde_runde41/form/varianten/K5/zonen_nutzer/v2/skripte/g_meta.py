"""Sicherheit (sicher / unsicher / verdeckt) mit Grund und Aenderung gegenueber v1 je Zone. Grundlage: zonen_v2_roh.json,
augen_kopf.json, krallen_spitzen.json (Zahlen dort); Texte fuer Tafeln und Messblatt."""
S, U, V = 'sicher', 'unsicher', 'verdeckt'
META = {
    'oberkoerper_hinten': (S, 'Umriss deckt das Modell zu 99 %; Rumpfteile Hüfte, Rücken, Brust', ''),
    'wunde_hinten': (S, 'Ihr grauer Umriss liegt ganz auf der Hüfte; nur dieser Umriss wird zur Wunde (Texel-Maske), nicht die ganzen Flächen', 'R2: texelgenau statt 3 ganzer Flächen'),
    'musterung_rechts_hinten': (S, 'Ihr grüner Umriss liegt ganz auf der Hüfte; Texel-Maske in Form des Umrisses', 'R2: texelgenau statt ganzer Fläche'),
    'musterung_links_hinten': (U, 'liegt am Rand der Figur; das Modell zeigt nur 20 von 54 Video-px der Stelle', 'R2: texelgenau'),
    'kopf_hinten': (V, 'an dieser Stelle zeigt das Modell den rechten Oberarm (666 px); der Modellkopf ist in Bild 1 nicht zu sehen',
                    'v1 ordnete die Flächen von Oberarm 16 zu; v2: nichts nach Bildlage, Kopf nur nach Bedeutung (Teile 22/23)'),
    'auge_hinten': (V, 'Nutzer: "wahrscheinlich"; Modellkopf an dieser Stelle verdeckt (0 px)', ''),
    'mund_hinten': (V, 'Nutzer: "Mund?"; Modellkopf an dieser Stelle verdeckt (5 px Oberarm)', 'v1 nannte 5 px Oberarm 16; v2 ordnet nichts zu'),
    'schwanz': (S, 'Umriss deckt das Modell zu 98 %; Schwanzglieder 1-5', ''),
    'schwanz_muster': (S, '4 grüne Umrisse auf den Schwanzgliedern 3-5; Texel-Maske in Form der Umrisse', 'R2: texelgenau statt 11 ganzer Flächen'),
    'zipfel': (S, 'Umriss deckt die Schwanzspitze; unten endet das Nutzerbild am Schriftzug', ''),
    'zipfel_musterung': (S, '2 orange Umrisse auf der Schwanzspitze; Texel-Maske in Form der Umrisse', 'R2: texelgenau statt 4 ganzer Flächen'),
    'rotes_fleisch_zipfel': (U, 'nicht umrandet: je Linienende das Farbgebiet in Linienrichtung (4 Stellen). Gemessen: 2 Stellen violett-rot (a* 51/46, b* -43/-40), 1 rot (a* 44, b* -7), 1 nur schwach rötlich (a* 20, b* 0); die erste Stelle ist nur 1 Video-px groß', 'v1: Kreise; R2: je Ende genau ein Farbgebiet, texelgenau'),
    'bein_hinten_links': (S, 'Umriss deckt das Modell zu 92 %; Innenkante vom Nutzer nicht gezogen (Sehne gesetzt)', ''),
    'bein_hinten_links_abschnitt1': (U, 'Modellbein beginnt tiefer: im obersten Abschnitt liegen auch Hüfte (183 px) und Schwanz (123 px)', ''),
    'weisse_musterung_bein_hinten_links': (S, 'weißer Fleck 5 px rechts neben dem Linienende (L* 76 gegen 47 daneben), vor allem auf Oberschenkel 8; texelgenau', 'Z1: v1-Kreis lag auf dunkler Hüfte; jetzt der weiße Fleck, texelgenau'),
    'weisse_musterung_bein_hinten_links_strich2': (U, 'zweiter grüner Strich ohne eigenen Text; an seinem Ende (11,7 px) liegt derselbe weiße Fleck wie Nr. 14 (Frage)', 'Z5: neu; zweiter Zeiger auf denselben Fleck'),
    'braeunliche_musterung_bein_hinten_links': (U, 'Farbgebiet direkt am Linienende ist dunkel rot-violett (L* 27, a* 26, b* -6 gegen L* 20, a* 17 daneben), kaum braun', 'v1: Kreis; R2: Farbgebiet neben dem Ende, texelgenau; Sicherheit sicher -> unsicher'),
    'bein_hinten_rechts': (S, 'Umriss deckt das Modell zu 95 %', ''),
    'bein_hinten_rechts_abschnitt1': (U, 'im obersten Abschnitt liegt im Modell auch die Hüfte (938 px)', ''),
    'weisse_musterung_bein_hinten_rechts': (S, 'weißer Fleck am Linienende (L* 90); liegt im Modell zu 88 % auf der Hüfte, Rest Oberschenkel 9; texelgenau', 'Z3: ganzer Fleck statt Kreis; texelgenau'),
    'braeunliche_musterung_bein_hinten_rechts': (U, 'im Grundbild nicht braun messbar (L* 63,5, a* 7,7, b* -20,7: graublau); gemeint vermutlich die helle Fläche links vom Linienende unter dem zweiten dunkelroten Strich (Frage)', 'v1: Band um den Strich; R2: Farbgebiet neben dem Ende, texelgenau'),
    'fuss_hinten_links': (U, 'Modellfuß deckt nur 26 % der Nutzer-Hülle; die Krallenmarken liegen neben dem Modellfuß', ''),
    'fusskrallen_hinten_links': (U, 'Nutzermarken liegen über Hintergrund rechts neben dem Modellfuß (0 px auf Fuß 25); Vorschlag nach Bedeutung: die 3 Zehenspitzen von Fuß 25',
                                 'v1: Marken auf Schwanz 5; v2: Zehenspitzen von Fuß 25 nach Bedeutung'),
    'fusskrallen_hinten_rechts': (V, 'nur Textangabe "Rechts wäre es sicher genauso"; Fuß 27 in keinem Bild beschriftet', 'neu'),
    'vorderbein_rechts': (S, 'Umriss deckt das Modell zu 97 %', ''),
    'weisse_musterung_vorderbein_rechts': (S, 'grüner Umriss auf Oberarm 16', ''),
    'fuss_kralle_vorderbein_rechts': (U, 'das Modell zeigt hier nur die Hand 18 (95 px) = Fuß; eine Kralle ist im Modell an dieser Stelle nicht zu sehen', 'R2: Hand nicht mehr als Kralle geführt'),
    'oberkoerper_vorn': (U, 'der Modellkopf reicht 70 px in diesen Umriss (4966 px); ohne Kopf gezählt', 'Z4: Kopfflächen herausgenommen'),
    'fleisch_vorn': (U, 'der Modellkopf deckt 4812 px dieses Umrisses; Texel-Maske nur auf Brust 10 und Hals 21, gemessen im Bild ohne Kopf (Z4)', 'Z4 + R2: ohne Kopf 22/Unterkiefer 23, texelgenau'),
    'gesicht_vorn': (U, 'Modellkopf reicht 70 px höher als Ihr Umriss; Zuordnung nach Bedeutung: ganzer Kopf 22 und Unterkiefer 23',
                     'Z4: nach Bedeutung statt nach Bildlage'),
    'augen_vorn': (S, 'zwei helle Punkte 5,1 bzw. 2,2 px neben den Linienenden (L* 30 gegen 5-7); Auge = nur die Texel unter den Punkten', 'Z2: gemessene Punkte statt Kreise; R2: nur die Texel, nicht ganze Flächen'),
    'maul_vorn': (U, 'Ihr Maul-Umriss zeigt im Modell die Außenseite des Unterkiefers (1445 px) und Zähne, weil der Modellkopf anders sitzt; das Maulinnere ist in Bild 2 im Modell nicht zu sehen (0 px). Zuordnung nach Bedeutung: die 21 Flächen des Maulinneren', 'Z4 + R2: ohne Zähne, ohne die 14 Augenhöhlen-Flächen'),
    'unterkiefer_vorn': (U, 'Band am lila Strich; dort zeigt das Modell Unterkiefer (89 px) und Unterschenkel (48 px); nach Bedeutung: Unterkiefer außen',
                         'Z4: nach Bedeutung'),
    'arm_hand_krallen_vorn_rechts': (S, 'Umriss deckt das Modell zu 87 %', ''),
    'arm_vorn_rechts': (U, 'an Ihrem Abschnitt zeigt das Modell die Hand 18 (239 px) und den Kopf 22 (45 px), nicht den Arm; Zuordnung deckt die Stelle schlecht (IoU 0,16)', 'R2: sicher -> unsicher'),
    'hand_vorn_rechts': (S, 'Umriss deckt Hand 18 zu 100 %', ''),
    'krallen_vorn_rechts': (U, '2 Krallenformen gezeichnet (Kralle 31 und 19), dazwischen der Schriftzug; im Asset ist 19 eingezogen',
                            'v1: eine Zone; v2: Form 1, Form 3 und Fläche dazwischen getrennt'),
    'vorderbein_links': (S, 'Umriss deckt das Modell zu 96 %', ''),
    'beinabschnitt_vorn_links': (S, 'Oberarm 11 und Unterarm 12', ''),
    'fussabschnitt_vorn_links': (S, 'Hand 13 mit Krallen', ''),
    'krallen_vorn_links': (S, '4 geschlossene Schleifen = "Exakt 4"', 'v2: je Schleife eine eigene Zone'),
    'kralle_vorn_links_schleife1': (U, 'Spitze 4,7 px neben der Spitze von Kralle 30, aber Kralle 30 ist im Modell nur 2 px sichtbar (1 px in der Schleife)', 'R2: Spitze am richtigen Ende; sicher -> unsicher'),
    'kralle_vorn_links_schleife2': (U, 'Spitze 2,2 px neben der Handspitze 13 und 6,4 px neben Kralle 14; Vorschlag Kralle 14 (die linke Hand hat genau 4 Krallenteile: 14, 15, 28, 30)',
                                    'neu (eigene Zone)'),
    'kralle_vorn_links_schleife3': (S, 'Spitze 3,0 px neben Kralle 15', 'neu (eigene Zone)'),
    'kralle_vorn_links_schleife4': (V, 'Spitze 8,3 px neben Kralle 28 in K5c-Form; im heutigen Asset ist 28 eingezogen (0 px). Zurückgeholt liegt 28 nur zu 2 % in Ihrer Schleife, der Rest über dem Fußabschnitt (Frage)', 'neu (eigene Zone)'),
    'hals_unterseite': (V, 'kein Wort von Ihnen; Flächen von Hals 21 mit Normalen nach unten; bleibt Haut (nicht belegt)', 'neu (Z4, Modellgliederung)'),
}
META.update({
    'augenhoehle': (U, 'kein Wort von Ihnen. 14 Flächen um die Augen benutzen dieselbe rote Texturstelle wie die heutige Maul-Farbe (u 91-105, v 157-170); in Bild 2 sind 2 davon sichtbar (23 px), keine unter Ihren Augenpunkten', 'R2: war "Maulinneres" (Prüfer: lag an den Augen)'),
    'zaehne': (U, 'kein Wort von Ihnen; Zähne des Modells, 391 von 453 px liegen in Ihrem Maul-Umriss (Frage: eigenes Material?)', 'R2: Material nicht mehr "Zahn", nicht belegt'),
    'lippe': (U, 'kein Wort von Ihnen; Oberlippe des Kopfes 22, nur 297 px in Ihrem Maul-Umriss', 'R2: Material nicht mehr "Lippe", nicht belegt'),
    'krallen_vorn_rechts': (U, '2 Krallenformen gezeichnet (rechts im Bild auf Kralle 31, links im Bild auf Kralle 19), dazwischen der Schriftzug; im Asset ist 19 eingezogen',
                            'v1: eine Zone; v2: die zwei Formen und die Fläche dazwischen getrennt'),
})
# Runde 3 (Pruefer r2 Nr. 3, 8, 11, 12, 15): neue Gruende/Aenderungen
META.update({
    'zipfel_musterung': (S, '2 orange Umrisse auf der Schwanzspitze; je Umriss eine eigene Fläche (Strich + Inneres). Der dunkelrote Spalt zwischen den Umrissen '
                            '(227 Video-px, L* 33, a* 25) gehört NICHT dazu; Texel-Maske in Form der Umrisse',
                         'R2: texelgenau; R3: je Umriss eine eigene Fläche, der Spalt ist heraus (vorher eine gemeinsame Hülle)'),
    'rotes_fleisch_zipfel': (U, 'nicht umrandet: je Linienende das Farbgebiet in Linienrichtung (4 Stellen). Gemessen: 2 Stellen violett-rot (a* 51/46, b* -43/-40), '
                                '1 rot (a* 44, b* -7), 1 nur schwach rötlich (a* 20, b* 0); die erste Stelle ist nur 1 Video-px groß. Die dritte Stelle liegt im dunkelroten '
                                'Spalt zwischen Ihren orangen Umrissen; wo Ihr oranger Umriss liegt, gilt Ihr Umriss (Musterung)',
                             'v1: Kreise; R2: je Ende ein Farbgebiet, texelgenau; R3: keine Überlappung mehr mit Zone 11'),
    'augen_vorn': (S, 'zwei helle Punkte 5,1 bzw. 2,2 px neben den Linienenden (L* 30 gegen 5-7); Auge = nur die Texel unter den Punkten. Achtung: dieselben Texel '
                      'benutzt die heutige Textur auch an anderen Stellen; ohne eigene Augen-Texel erschiene das Auge in Bild 2 auf 73 statt 19 Video-px (3,9-fach)',
                   'Z2: gemessene Punkte statt Kreise; R2: nur die Texel; R3: Hinweis auf die Vergrößerung durch geteilte Texel'),
    'maul_vorn': (U, 'Ihr Maul-Umriss zeigt im Modell vor allem den Unterkiefer außen (1445 px) und Zähne, weil der Modellkopf anders sitzt. Das Maulinnere ist nach '
                     'LAGE bestimmt: die 21 Flächen, die erst durch das Öffnen des Mauls sichtbar werden (hinter den Zähnen). In Bild 2 sind 8 davon sichtbar '
                     '(448 Video-px), alle in Ihrem Umriss; sie decken 19 % davon',
                  'R3: nach Lage statt nach der Farbliste; die 10 Kopf-Unterseiten-Flächen mit heutiger Maul-Farbe (rote Hals-Streifen) sind heraus (Zone 45)'),
    'unterkiefer_vorn': (U, 'Band am lila Strich; dort zeigt das Modell Unterkiefer (89 px) und Unterschenkel (48 px); nach Bedeutung: Unterkiefer außen '
                            '(alle Flächen von Teil 23 außer Maulinnerem und Zähnen)',
                         'Z4: nach Bedeutung; R3: Material wie die anderen Teilnamen, ohne die neuen Maul-Flächen'),
    'lippe': (U, 'kein Wort von Ihnen; Oberlippe des Kopfes 22 (ohne die Flächen, die jetzt zum Maulinneren gehören), nur 297 px in Ihrem Maul-Umriss',
              'R2: Material nicht mehr "Lippe"; R3: 8 Flächen an das Maulinnere abgegeben'),
    'kopf_unterseite_maulfarbe': (V, 'kein Wort von Ihnen. 10 Flächen an der Kopf-Unterseite hinter dem Unterkiefer, die heute die rote Maul-Farbe tragen (darunter die '
                                     '8 großen Außenflächen 22|1|1, 8, 9, 194, 201, 209, 210, 217, die die roten Hals-Streifen machen); in Bild 2 nicht sichtbar. '
                                     'Lage außen, nicht im Maul: bleibt Haut (nicht belegt)',
                                  'neu (R3): war in Runde 2 Teil von "geöffnetes Maul"'),
    'krallen_vorn_rechts': (U, '2 Krallenformen gezeichnet (rechts im Bild auf Kralle 31, links im Bild auf Kralle 19), dazwischen der Schriftzug; im Asset und im '
                               'Vorschlag ist 19 eingezogen (Kralle 19 läge sonst über Ihrer Hand)',
                            'v1: eine Zone; v2: die zwei Formen und die Fläche dazwischen getrennt; R3: 19 eingezogen'),
})
KRALLE_ZUORDNUNG = {
    'kralle_vorn_links_schleife1': [30], 'kralle_vorn_links_schleife2': [14], 'kralle_vorn_links_schleife3': [15],
    'kralle_vorn_links_schleife4': [28], 'krallen_vorn_rechts_form1': [31], 'krallen_vorn_rechts_form3': [19],
    'krallen_vorn_rechts_dazwischen': [20, 29],
}
