# Sicht-Pruefung Farbe V3 Runde 5 (K5c, Krallen wie Runde 3), 2026-11-07 Abend

Pruefer: Sicht (Auge des Nutzers). Gegenstand: `farbe/K5c_farbe_v3/` = `farbe/K5C_FARBE_V3.EMD` (sha256 090e0b80e214ae45...) / `.TIM` (512828067ea7f048...),
selbst nachgerechnet; Commits 6349350a4 / e2816e70f / 2b3bb75ba / 278a4792c auf r41/form. Runde 4 zum Vergleich `farbe/K5c_farbe_v3r4/` + `farbe/v3r4/`
(EMD 07d679a57254b56e...), Runde 3 `farbe/v3r3/` (d19fe9012ff8a5a1 / abd0ba1b2390c942). Tafeln 01-10 in `farbe/`, Tupfen `farbe/krallen_tupfen.json`,
Riegel `farbe/riegel_K5c_farbe_v3.txt` (77/80 bindend), Dossier `FARBE.md` Abschnitt "Versuch 3 Runde 5". Vorrunde `FARBE_PRUEFUNG_v3_r4_sicht.md`.

Eigene Ausschnitte und Messungen (nicht im Repo):
`C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/formfarbe4/sicht_v3r5/`
- `g_spiel_ruhe_r5080_s1_r5_r4_r3.png`, `g_spiel_pose1_r5080_s1_r5_r4_r3.png`, `g_spiel_pose1_nachstellung_r5080_r5_r4_r3.png`, `g_spiel_pose2_r5090_s11_r5_r4_r3.png`
  (rohe gdigrab-Spielbilder, 3-fach, Runde 5 | Runde 4 | Runde 3)
- `h_tafel06_z3..z5_k5c_k4_g4_f18_x3.png` (Kopfzellen Tafel 06, 3-fach), `i_tafel05_*_clip0_clip2_x3.png` (Tafel 05, 3-fach), `j_tafel03_k5c_drei_x2.png`
- `l_tafel03_kopfzone.png`, `l_tafel03_schulterzone.png`, `k_diff5090_r5_r4_rot_kralle_gruen.png`
- `mess_links_rechts.json`, `mess_zonen.json` (Lab D65, OpenCV float; Farbtonklassen wie Runde 4: violett h -90..-15, mauve/rot -15..15, braun 15..80, neutral C* <= 6)

Seiten: (W) = aus Sicht des Wesens, (B) = Betrachter.

## Massgebende Saetze

1. **Nutzer, Ausloeser dieses Laufs (woertlich, bindend, geht jeder Orchestrator-Zeile vor):** "Nein, es passt eben nicht.... man wy... ich weiss nicht wie ich das noch
   beschreiben soll.... da ist links ein Auge. Linke Seite ist so graeulich. Bis Nase. Dann rechts Seite sieht man Haare - braeunlich.... Warum mischst du die Farben
   so daemlich?"
2. Nutzer 2026-11-07 ~17:10: "entfernst die uebertriebenen roten feuerrillen wieder, und stellst es wieder nach vorlage um?"
3. Nutzer zu Runde 3: "zu viel rot im Vergleich zur Vorlage, aber immerhin bedrohlich".
4. Orchestrator: Runde 5 eng auf die Krallen begrenzt; "Der Zweifarben-Befund unter weissem Licht ist bewusst unveraendert (Nutzerfrage) und kein Grund fuer
   'nicht bestanden'." - Dieser Satz ist durch Satz 1 ueberholt: Der Nutzer hat die Frage beantwortet, und zwar mit Nein.

## Ergebnis

**Nicht bestanden. Ein Punkt der dritten Klasse: die zweifarbige Haut (violetter Rumpf, braune Vorderbeine/Schulter, grau-violetter Kopf), die der Nutzer in
Satz 1 ablehnt.** Der eng gefasste Krallen-Auftrag der Runde 5 ist dagegen vollstaendig erfuellt; daran liegt es nicht.

### Was Runde 5 richtig macht (Krallen-Auftrag)

- **Aenderungsumfang (AE1) selbst nachgerechnet:** EMD Runde 5 gegen Runde 4: 504 verschiedene Bytes an 252 Stellen, jeweils das 16-Bit-CLUT-Feld
  0x7C00 -> 0x7840 (Zeile 16 -> Zeile 1, Spalte gleich). Kein anderes Byte, Dateilaenge gleich (170 424). TIM bytegleich. Tafeln 01, 06, 08 bytegleich zu
  `farbe/v3r4/`. Spielbilder 5080 Runde 5 gegen Runde 4: 179 / 173 / 50 Pixel ueber Schwelle 8, alle in den Krallen-Kaesten (209-316 x 193-220 usw.).
  5090: 34 853 Pixel, davon 26 488 im animierten Feuer und 8 365 in dunklen Figurpixeln (Feuerlicht flackert; die Figur steht gleich). Geometrie 0. **GRUEN.**
- **Krallen (Tafel 10, `g_spiel_pose2_*`):** Runde 5 zeigt in allen drei Zeilen dieselbe dunkelrote Klaue mit khaki Spitze und durchgehendem Laengsverlauf wie
  Runde 3; die Sprenkel der Runde 4 (schwarze Klaue mit hellen Tupfen) sind weg, auch im echten Spielfenster 5090 (Z.3). Tupfen (Zaehlung des Bauers,
  gleiche Regel fuer alle Spalten): Bild 2 roh 67 px / 38 Komp. (Runde 4 393 / 90, Runde 3 67 / 38), gefiltert 31 / 8 (39 / 17), Spiel 19 / 8 (77 / 26, Runde 3 21 / 7).
  Kennungsmaske im Spielbild 1356 Pixel, von mir nachgezaehlt. A7 Kralle 0,096 wie Runde 3. **Wie Runde 3 / wie Spielfamilie.**
- **Feuerrillen (Satz 2):** weiterhin keine hellen roten Adern in 01-09; Adern nur als dunkle Struktur (Tafel 08 bytegleich Runde 4, RV6 GRUEN).
- **Rot nach Vorlage (Satz 3, Tafel 09):** Bild 1 Schwanz 23,4 % (Vorlage 23,5), Figur -22,4 Grad (Vorlage -15,8, Fenster bis -30,6), Bild 2 Ruecken 58,5 % / -8,9 Grad
  (Vorlage 53,6 / -5,3), Vorderbeine ohne Krallen 75,0 % (Vorlage 64,7, Fenster 53,3..76,1). RV 45/45. Die Tupfenverteilung in Bild 1 liest sich wie das Video.
- **Fairness:** Tafel 10 hat je Zeile gleichen Ausschnitt und gleiches Licht, Tupfenzahl in der Unterschrift, Original daneben; die Original-Spalte traegt zu Recht den
  Hinweis, dass die Maske dort Hintergrund trifft. 02/03/05/07/09 wie Runde 4 (gleiche Kette). Hashes in Bau-Log, Spiellauf-Log, Archiv und Dateien stimmen ueberein.
  **Fair.**

### Der Ablehnungsgrund (Satz 1)

Der Nutzer beschreibt ein Tier, dessen Farben auf einer Seite grau und auf der anderen braun-haarig sind. Genau das zeigen die 3/4-Ansichten und das echte Spielfenster
5080 (Koerper bytegleich Runde 4; der Sicht-Pruefer Runde 4 hatte es als "groesstes Restrisiko" gefuehrt):
- **Tafel 03 / 05, 3/4 vorn (`j_tafel03_k5c_drei_x2.png` links, `i_tafel05_34vorn_*`):** Kopf dunkelgrau-violett mit dunklem Auge und Maulstrich; unmittelbar rechts
  daneben die Schulter und das Vorderbein braunrot mit laengs gestreifter, faseriger G4-Zeichnung ("Haare"); dahinter der Rumpf violett. Gemessen in der 3/4-vorn-Zelle
  (`mess_zonen.json`): Kopfzone L\* 22,7, neutral 59,2 %, mauve 24,1 %, braun 6,2 %; Schulter/Vorderbein-Zone L\* 15,8, braun 27,1 %, violett 26,0 %, mauve 0,9 %.
  Das ist eine harte Grenze zwischen zwei Farbwelten an der Kopf-Hals-Naht.
- **Tafel 05 vorn (`i_tafel05_vorn_*`):** Brust und Vorderbeine braunrot, Rumpf dahinter violett; `mess_links_rechts.json` vorn Clip 0: linke Haelfte (B) L\* 25,7,
  braun 34,7 %, violett 0 %; rechte Haelfte L\* 12,0, violett 22,5 %, braun 25,6 %.
- **Tafel 06 Z.3-5 (`h_tafel06_z3_*`):** der Kopf allein ist eine dunkle Weinrot-Mauve-Flaeche mit grauen Lappen (Z.3 links(B) mauve 52,6 % / rechts(B) neutral 43,1 %,
  violett 26,7 %); keine Augen-/Nasenzeichnung, die ein Gesicht ordnet. Gegen K4 (einheitlich schwarz) und RE2-G4 (einheitliche Fleischhaut mit Schattenzeichnung)
  liest sich das als zusammengesetzt.
- **Echtes Spielfenster 5080 Schnitt 1 (`g_spiel_ruhe_r5080_*`, `g_spiel_pose1_r5080_*`, Tafel 07 Z.2/3):** Runde 5 (= Runde 4) ist auf 1x ein **violettes Tier mit
  braunroten Vorderbeinen und grau-schwarzem Kopf**; Runde 3 daneben eine einheitlich schwarze Figur mit roten Adern. Zahlen des Sicht-Pruefers Runde 4 fuer
  dasselbe Bild (Koerper bytegleich): violett 28,7 % (h -39,3), braun 33,9 % (h 35,8), neutral 37,3 %; Runde 3: 61,7 % neutral, 17,4 % mauve/rot, 2,1 % violett.
- Ursache nach Dossier V3R4.4 / Zahlen-Pruefer Runde 4 Z.224: Gruppe "Rumpf Flanke" h -42,8 bei C 48,6 (violett), Gruppe "Vorderbeine" h +39,9 (braun), Haut oberhalb
  G4-L\* 21,7 unbunt grau (s(L) = 0). Die Gruppen wurden je Region gegen RV1-RV4 gesucht, nicht gegen eine gemeinsame Hautfarbe. Der Nutzer sieht das Ergebnis dieser
  Trennung: "Warum mischst du die Farben so daemlich?"

**Bewertung:** Abweichung, die der Nutzer ablehnt (er hat es getan). Dritte Klasse. Dass die Runde 5 laut Auftrag nur die Krallen anfassen durfte, aendert nichts
daran, dass die Variante so nicht vor den Nutzer kann: Er hat sie bereits so gesehen und abgelehnt.

### Was ich sonst zu widerlegen versucht habe

1. **Ist Runde 5 irgendwo schlechter als Runde 4?** Nein. Nur die 252 Krallenflaechen sind anders und dort besser (Tupfen weg). Die Krallen sind im Feuerlicht roeter als
   die Video-Klaue (Krallen allein 78,1 % gegen 51,0 %, Dossier V3R5.1); auf Tafel 10 Z.2/Z.3 ist das eine dunkelrote Klaue vor rotem Boden und faellt nicht auf. Klein.
2. **Sind die Krallen nur gefiltert glatt?** Nein, Tafel 10 Z.1 roh und Z.3 Spielfenster zeigen den Verlauf ohne Sprenkel. Trug nicht.
3. **Ist die Form noch bedrohlich?** Dunkel (L\* 7,9-9,4 unter weissem Licht), Glanz 0,00 %, Zaehne, dunkle Augen; in 5090 ein schwarzer Block mit Klauen im Feuer.
   Bedrohlich ja, aber nach Satz 1 als Farbflickwerk gelesen - das Bedrohliche rettet die Zweifarbigkeit nicht.
4. **"Pokemon"/Comic?** Keine Konturen, kein Glanz; der rosa Zungenzipfel (Tafel 06 Z.4) wie Runde 3. Trug nicht.
5. **Familie (Tafel 03):** Machart (Texelgroesse, Adern als dunkle Struktur) wie G4; Helligkeit halb so hoch wie G4; Farbton violett statt Fleischrot. Neben EM30/G5/G4
   ist K5c das einzige Tier mit zwei getrennten Hautfarben. Das ist Satz 1 aus der Familienperspektive.

## Befunde je Punkt

| Punkt | Soll | Ist | Bewertung |
|---|---|---|---|
| Aenderungsumfang gegen Runde 4 (AE1, eigener Byte-Diff, Tafel-Hashes, Spielbild-Diff) | nur Krallenflaechen, Geometrie 0, TIM gleich | 504 Bytes = 252 x CLUT-Feld 0x7C00 -> 0x7840 (Zeile 16 -> 1); TIM bytegleich; Tafeln 01/06/08 bytegleich; 5080-Spielbilder nur in den Krallen-Kaesten verschieden (179/173/50 px); 5090-Diff = Feuer + Feuerlicht | wie verlangt |
| Vorderkrallen Bild 2 roh / gefiltert / Spiel 5090 (Tafel 10, `g_spiel_pose2_*`) | Runde-3-Klaue: dunkelrot, khaki Spitze, durchgehender Verlauf, keine Sprenkel; Tupfen <= Runde 3 | identisch mit Runde 3 (67/38 roh, 31/8 gefiltert, 19/8 Spiel gegen Runde 4 393/90, 39/17, 77/26); A7 0,096; mit dem Auge ein Verlauf, kein Mosaik, keine Tupfen | wie Runde 3 / wie Spielfamilie |
| Feuerrillen (Nutzersatz 17:10; Tafeln 02, 03, 05, 08) | keine hellen roten Adern | unveraendert Runde 4: Adern nur dunkel, RV6 alle Regionen dL negativ | wie Vorlage |
| Rotmenge, Verteilung, Farbton Bild 1 und Bild 2 (Tafel 09, RV 45/45) | wie Vorlage: verstreute mauve Tupfen, -15,8 Grad Figur; Bild 2 dunkel karmin | Schwanz 23,4 % / Vorlage 23,5; Figur -22,4 Grad; Ruecken Bild 2 58,5 % / -8,9 Grad; Vorderbeine ohne Krallen 75,0 % im Fenster 53,3..76,1 | wie Vorlage |
| Zweifarbige Haut: violetter Rumpf, braune Vorderbeine/Schulter, grau-violetter Kopf (Tafel 03/05 3/4 vorn + vorn, Tafel 07 Z.2/3, `g_spiel_*5080*`, `j_*`, `h_*`) | Nutzer: "schwarz rot ... verbrannt", eine Haut aus einem Guss; Familie: eine Hautfarbe je Tier | Kopfzone neutral 59 % / mauve 24 %, Schulter/Vorderbein braun 27 % / violett 26 %; vorn links(B) braun 34,7 % gegen rechts(B) violett 22,5 %; im echten Fenster 5080 violett 28,7 % + braun 33,9 % (Runde 3: 61,7 % schwarz). Nutzer woertlich: "links ein Auge. Linke Seite ist so graeulich. Bis Nase. Dann rechts Seite sieht man Haare - braeunlich.... Warum mischst du die Farben so daemlich?" | **Abweichung, die der Nutzer ablehnt (abgelehnt am 2026-11-07)** |
| Kopf allein (Tafel 06 Z.3-5, `h_tafel06_*`) | G4-Machart, gemalte Struktur, dunkle Augen, Gesicht lesbar | dunkle Weinrot-Mauve-Flaeche mit grauen Lappen, Augen dunkel, Zaehne weiss, Lippe lachsrosa; keine ordnende Gesichtszeichnung; Teil des Zweifarben-Befunds (Kopf grau gegen Hals braun) | siehe Zeile darueber |
| Krallen roeter als Video-Klaue im Feuerlicht (Dossier V3R5.5 Punkt 1) | Vorlage 51,0 % Rot eng | 78,1 % (Runde 3 gleich); auf Tafel 10 Z.2/Z.3 nicht stoerend, dunkelrote Klaue vor rotem Boden | Abweichung klein |
| Leuchtstelle F1 Bild 1 / Maulband Bild 2 (Tafel 01, 02 Nahkachel, 06 Z.1/Z.2) | wie Runde 3 bis zur Nutzerantwort | unveraendert Runde 4: F1 ohne roten Saum, Maulband mit Zaehnen, gefiltert matter als Original | Abweichung klein (wie Runde 3/4) |
| Bedrohlich / nicht ausgewaschen / kein Comic (Tafel 02, 03, 07) | dunkler Grundton, Form, Zaehne | L\* 7,9-9,4, Glanz 0,00 %, Zaehne, dunkle Augen; 5090 schwarzer Block im Feuer | wie Vorlage |
| Familie (Tafel 03) | passt neben EM30/G5/G4 | Machart wie G4, halb so hell; einziges Tier mit zwei Hautfarben (violett/braun) | Abweichung klein in Machart, dritte Klasse in Farbe (siehe oben) |
| Fairness der Tafeln | gleiches Licht, gleiche Kette, Original daneben, Zahlen in der Unterschrift | Tafel 10 fair (Hinweis zur Original-Maske steht da); 02/03/05/07/09 wie Runde 4; Hashes konsistent | fair |

## Was jetzt zu tun ist (Befund, keine Konstanten)

- Nicht weiter polieren im Rahmen der Runde 5 (Krallen sind erledigt). Die Zweifarbigkeit ist kein Krallen-Thema, sondern die Gruppen-Farben "Rumpf Flanke" (violett)
  und "Vorderbeine"/"Hals" (braun) samt grauer Haut oberhalb G4-L\* 21,7. Fuer eine naechste Runde (Nutzerentscheid noetig, da letzte Runde vor v0.8.30 erklaert):
  eine gemeinsame Hautfarbe fuer alle Koerpergruppen (Farbton nach Bild 1 Figur -15,8 Grad, Buntheit klein), Vorderbein- und Halsgruppe nicht getrennt braun,
  Kopf in derselben Haut; danach RV1-RV6 neu und Tafel 07 Z.2/3 dem Nutzer zeigen.
- Alternativ Runde 3 als Grundlage (einheitlich schwarz mit Adern, vom Nutzer als "bedrohlich" angenommen) und nur Rotmenge/Farbton der Adern nach Vorlage
  daempfen, statt die Haut je Region neu zu faerben.
- Rueckfrage Messblatt V3 7.1 (Becken oder Gesicht) bleibt offen.
