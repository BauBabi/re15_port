# SF V5 Kopf aus RE2 Runde 1 - Sicht-Pruefung (Pruefer, 2026-11-07 nachts)

Gegenstand: Variante C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/sfv4/emd/EM30_V4re2.EMD
= EM30_V4.EMD (sha256 f37bc5514c6892186d90a808dfd20f646aeecb59b0667933b3daa84f296ff18d, 262284 Byte, selbst gemessen). Basis EM30_V4ot3.EMD
(a1bc9309, Runde 3). Referenz 2 = C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel/re15_port/shared_assets/PSX/EMD/EM30_HANGAR.EMD
(sha256 cfd1ac42729e5636..., unveraendert). Zweig r41/sichel_nb2, HEAD dc0f76e47 (Bauer-Commits 0b6d20c63, 432c36b8f, 40de9d6de, dc0f76e47).

Nutzer-Saetze, gegen die gelesen wird: "bei den KI Bildern kann man so verdammt gut sehen wie das verschwommene Bild eigentlich aussehen
muss.... Man sieht alles einwandfrei! Das Auge die Haare, den Mund, den Schwanz" / "Vielleicht kannst du dir auch einen Birkin aus Resident
Evil 2 nehmen" / "da ist links ein Auge. Linke Seite ist so graeulich. Bis Nase. Dann rechts Seite sieht man Haare - braeunlich".

Gelesen: Bauer-Leisten C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel/analysis/befunde_runde41/SF_belege/v4/01_vorn_kamera12.jpg,
02_seite_wie_bild2.jpg, 03_kopf_nah_vorn_x4.jpg, 04_kopf_nah_seite_x4.jpg, 08_rundum_V4.jpg, 10_kopf_ki_re2_neu_x6.jpg, 11_spiel_koepfe.jpg;
KI-Bilder C:/workspace/git/reAi_v2/Version_80%/image_1.png, "image_2 (2).png", "image_3 (3).png"; Beschreibung1.png, Beschreibung2.png,
Beschreibung1_Vorlage.png, Vorderseite_Beschreibung.png; Dossier-Abschnitt "V5 Kopf aus RE2" in SF_sichel.md (ab Zeile 2046); Riegel
SF_belege/v4/re2/riegel_re2kopf_V4re2.txt (1 rot von 39 = T5); ctest_v4_re2.txt (4/4).

Eigene Gegenprobe (unabhaengig von leisten_re2kopf.py / riegel_re2kopf.py), Skripte und Bilder in
C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/sicht_re2/:
- render_re2.py: v4_gemein.zeichne (b2, Kamera 50A0/12, RDT-Licht und licht=False) fuer Referenz 2 / ot3 / neu, Posen "135" und "85", Id-Puffer.
- mess_re2.py -> mess_re2.json: Mesh-Bytevergleich (tv/tn/tf/tuv/qv/qn/qf/quv), CLUT-Zeilen, Texel; Kopf-Lab (CIE-Lab D65, skimage) auf
  Id 9, Koerper-Lab auf Ids 0-8/10-15, Farbton-Anteile (oliv 45-120 Grad, violett 270-345, blau 200-270, Chroma > 10), Zahn- und Rot-Pixel ohne
  Marker, Haelften, Groessen; dieselben Masse an den sechs KI-Kopfboxen (vorlagen_kopf.KI) und an Vorlage 1/2 (v4_gemein.vorlage).
- roh_uv_re2.py -> roh_uv.json, D_roh_dumps.jpg: Koerper-/Schwanz-UV-Texel (TPage-dekodiert) gegen Referenz 2 / ot3; rohe RE15_FRAMEDUMPs
  960 x 720 (c6.roh, Versatz 243 beide Laeufe) h105/h118/h125/h135 Kopfumfeld x3.
- kacheln_re2.py -> A_kopf_ki_neu_spiel.jpg (KI-Koepfe aus drei Bildern | Referenz 2 | neu RDT-Licht | neu Texelfarbe | Spiel h135/h85 |
  Video, alle 240 px hoch), B_spiel_folge.jpg (Spielbilder h85-h135 Referenz 2 / neu), C_figur.jpg (ganze Figur), spiel_mess.json.
Die Bauer-Zahlen sind reproduziert: Kopfbox neu Pose 135 (325,160,384,221) = 60 x 62 px, H 364 -> 0,165 x 0,170 H; Kopf-Median L* 15,6
(Bauer 15); Haelften 15,4 / 15,9; 8 CLUT-Zeilen; Koerper bytegleich.

## Ergebnis: NICHT bestanden (ein Punkt der dritten Klasse: der Kopf ist im Spiel ein fast schwarzer Klotz - Auge, graue Seite und Haarseite
## sind nicht zu erkennen; die KI-Bilder und das Video zeigen den Kopf HELLER als die Brust, neu ist er das dunkelste Teil der Figur)

Was stimmt: Der Kopf ist jetzt ein echtes Monsterkopf-Modell aus RE2 (G2-Kopf, 48 Ecken / 56 Dreiecke / 14 Vierecke, Herkunft im Dossier
byte-true belegt), kein gemalter Fleck und kein Block; er hat ein Maul mit zwei hellen Zahnreihen und dunkelrotem Inneren (G3-Kachel), das im
Render und im rohen Spielbild an der Schnauzen-Unterseite zu sehen ist; Groesse und Sitz zwischen den Schultern sind wie in den KI-Bildern;
keine oliv-gruenen, violetten oder blauen Toene (Anteile 0,028 / 0 / 0; Runde 3 hatte 0,128 oliv); Koerper und Schwanz sind bytegleich.
Was der Nutzer nach meiner Einschaetzung wieder ablehnt: Im Spielbild (Blatt B, D; Bauer-Leiste 11) ist der Kopf ein dunkles Loch zwischen
den Schultern. Das Auge ist ein 1-2 Bildpunkte grosser roter Punkt, die "graue linke Seite" und die "braune Haarseite" sind im Dunkel nicht
unterscheidbar. Genau diese drei Dinge ("Das Auge die Haare, den Mund") hat der Nutzer als das benannt, was die KI-Bilder "einwandfrei"
zeigen. Die Ursache ist messbar: der Kopf ist dunkler als der Koerper, waehrend er in jeder Vorlage heller ist als die Brust.

## (1) Echtes Monsterkopf-Modell mit Maul und Zaehnen - wie KI-Bild/Vorlage (Modell), Abweichung klein (Form)

- Geometrie: Mesh 9 neu hat 48 Ecken, 56 Dreiecke, 14 Vierecke (Referenz 2 / ot3: 61 Ecken, 73 / 18); nicht gleich ot3 (mess_re2.json
  kopf9_eq_ot3 false). Kopf-UV-Bereich x 0-267, y 160-249 (5494 Texel), ueberlappt 0 Koerper- und 0 Schwanz-Texel; CLUT-Rohwerte 31104 /
  31168 = Zeilen 486 / 487 (die beiden RE2-Zeilen), Koerper nutzt 480 / 481 / 482 / 484. Das ist ein eigenes Modell mit eigener Textur, kein
  gemalter Fleck auf dem Ref2-Klumpen.
- Maul: im Render Pose 135 (Blatt A Zeile 1, Kachel 5/6) eine helle Zahnreihe ueber dunkelrotem Innerem an der Schnauzen-Unterseite rechts;
  ohne Marker gezaehlt 18 Bildpunkte L* > 60 / Chroma < 20 (RDT-Licht), 58 in Texelfarbe; Zahn-Box (353,191)-(371,207). Im rohen Spielbild
  960 x 720 (Blatt D Zeile 2) sind die Zaehne bei h105 / h118 / h125 / h135 als hellbraune Hoecker am dunklen Kiefer sichtbar (unter dem Licht
  L* 45-55, Chroma 25-30, deshalb zaehlt die L* > 55 / Chroma < 25-Schwelle dort 0-15). KI-Koepfe: Zahn-Anteil 2,6-7,9 % der Kopfbox, neu
  0,7 % im Licht - die Zaehne sind da, aber kleiner und dunkler als im KI-Bild.
- Form: G2 ist ein kantiger Keilkopf (Kuppe, Schnauze nach vorn-unten); die KI-Koepfe sind glatte Kuppeln mit Schnauze (Robbe/Hai). Seitlich
  (Blatt A Zeile 2) stimmt die Silhouette: Schnauze nach vorn-unten, Auge nahe der Spitze. Vorn ist der Keil kantiger als die KI-Kuppel; bei
  48 Ecken hinnehmbar. Der offene Kiefer der KI-Bilder ist als Texturband (Zahn-Kachel auf 11 Unterseitenflaechen) umgesetzt, nicht als
  Geometrie - von vorn liest es sich trotzdem als Maul.
- Widerlegung versucht: "Maul nur im Marker-Render?" -> nein, ohne Marker 18 / 58 Bildpunkte und im rohen Spielbild sichtbar. "Kopf = Block?" ->
  nein, Keil mit Kuppe, Schnauze, Unterseite. Bewertung: Modell **wie Vorlage**, Form/Maul-Umsetzung **Abweichung klein**.

## (2) Auge Bild-links, Haarseite Bild-rechts, Nase - Lage wie Vorlage, Sichtbarkeit: Abweichung, die der Nutzer wieder ablehnen wuerde

- Lage (Bauer-Marker, reproduziert nicht angezweifelt): K4 0,060 H links der Kopfmitte / 0,097 H unter der Oberkante (Fenster 0,035-0,065 /
  0,07-0,12); S2 0,043 / 0,812 H; K7 Zahn-Schwerpunkt 0,60 / 0,66 der Kopfbox. Im Render Pose 135 (Blatt A) sitzt der rote Augenpunkt oben
  links in der Kopfbox, das Maul unten rechts (Gieren -34 Grad). Lage **wie Vorlage**.
- Sichtbarkeit: Das G2-Auge ist ein rot gluehender Punkt mit hellem Kern; im Render Pose 135 31 Bildpunkte a* > 25 im ganzen Kopf (die roten
  Adern eingerechnet), hellster Kopfpunkt (370,196) L* 64,7 liegt an den Zaehnen, nicht am Auge. Im rohen Spielbild 960 x 720 ist das Auge bei
  h105 / h118 ein roter Punkt von 1-2 Bildpunkten, bei h125 / h135 nicht auszumachen (Blatt D). KI: dunkle Pupille mit weissem Glanzpunkt in
  grauer Haut (hoher Kontrast); Video / Nutzer: heller Fleck ("da ist links ein Auge"; Runde 3 hatte den hellen Augenfleck L* 54). Neu: kein
  heller Fleck, kein Kontrast - das Auge ist im Spiel nicht zu sehen.
- Haarseite: Haelften-Median Pose 135 links 15,4 / rechts 15,9 L* (Bauer 15,4 / 15,9; "Offen 4"); Pose 85 17,0 / 16,5. Die Andunkelung der
  (W)-linken Haelfte ist von der Kamera 50A0/12 abgewandt. Im rohen Spielbild h135 (Fenster Kopfmitte) links 21,6 / rechts 16,6, h118 18,5 /
  21,7, h125 17,0 / 20,5 - mal links, mal rechts dunkler, also keine lesbare Haarseite. Video (Vorlage 1 entblendet, Kopfbox 368-416):
  links 24,3 / rechts 14,2 - das ist der Gegensatz, den der Nutzer beschreibt. KI image_1 FRONT: rechte Kuppelhaelfte 9-21 L* dunkler (Leser B).
- Nase: Mittelspalte angedunkelt (Bauer faktor 0,50); im Dunkel des Kopfes nicht als eigene Zone erkennbar.
- Bewertung: Die Lage stimmt, aber im Spiel sieht man weder Auge noch den Grau/Haar-Gegensatz. Der Nutzer hat beides zweimal woertlich
  verlangt. **Abweichung, die der Nutzer wieder ablehnen wuerde** (Ursache in Punkt 4).

## (3) Groesse und Sitz des Kopfes - wie KI-Bild/Vorlage

- Neu Pose 135: Kopfbox 60 x 62 px bei H 364 -> Hoehe 0,170 H, Breite 0,165 H; Breite zu Schulterbreite (230 px unter der Kopfbox) 0,261.
  KI-Figuren (image_1 / 2 / 3 FRONT, Figurhoehe 496 / 476 / 492 px): Kopf-Hoehe 0,222 / 0,200 / 0,203 H (inkl. Unterkiefer), Breite 0,202 /
  0,200 / 0,173 H, Breite zu Schulter 0,270 / 0,291 / 0,235. Referenz 2: 0,170 x 0,126 H, 0,199. Messblatt K10 0,14-0,18 x 0,15-0,20: erfuellt.
- Sitz: Oberkante auf Schulterhoehe, Kopf zwischen den Schultern vor dem Buckel (Blatt C, Leiste 01); in Pose 85 haengt er vorn-unten wie im
  KI-Bild RIGHT; im Spielbild h85 liegt er hinter dem Buckel (Lauf-Pose, nicht die b2-Pose; bei Referenz 2 genauso).
- Bewertung: **wie KI-Bild/Vorlage**.

## (4) Hautfarbe des Kopfes zum Original-Koerper - Farbton wie Vorlage; Helligkeit: Abweichung, die der Nutzer wieder ablehnen wuerde

- Farbton (Render Pose 135, RDT-Licht, Id 9): a* 5,2 / b* 1,8, Chroma 5,8 - neutrales Dunkelgrau; Anteile oliv 0,028, violett 0, blau 0,
  rot/rosa 0,358 (die G2-Adern und das Maul), unbunt 0,613. Koerper (Ids 0-8/10-15): a* 5,3 / b* 3,0, oliv 0,018, violett 0,031. Runde 3
  hatte 0,128 oliv (der abgelehnte Khaki-Ton) - der ist weg. Alle Kopf-Texel stammen aus zwei RE2-CLUT-Zeilen (Riegel T-R1b/c/d, von mir
  per CLUT-Rohwerten 486 / 487 bestaetigt): keine Vierfarben-Mischung, keine Fremdfarbe. Farbton **wie Vorlage**.
- Helligkeit: Kopf-Median L* 15,6 (P10 4,0 / P90 29,2) gegen Koerper 22,6 -> Kopf/Koerper 0,69 (Pose 85: 17,0 / 25,1 = 0,68; Texelfarbe
  ohne Licht 0,67 - also die Textur selbst, nicht das Licht). Referenz 2: 30,8 / 22,7 = 1,36; Runde 3 (ot3): 27,4 / 22,5 = 1,22.
  Vorlagen: Video n1485 entblendet Kopfbox 17,5 gegen Brust 11,0 = 1,59; KI image_1 / 2 / 3 Kopf 21,8 / 22,7 / 18,8 gegen Brust 13,3 /
  10,2 / 11,7 = 1,64 / 2,23 / 1,61. In JEDER Vorlage ist der Kopf das hellere Teil ueber der Brust; neu ist er das dunkelste Teil der Figur.
  Spielbild (c6.port h135, Fenster Kopfmitte): neu 20,7 gegen Brust 19,3 = 1,07; Referenz 2 1,44; rohes Spielbild 960 x 720 Kopfmitte neu
  L* 18-21 (P10 3,5-5,0) gegen Referenz 2 27-29.
- Im Videoraum (Vergleichsregel Messblatt Abschnitt 5) liegt der neue Kopf bei L* 21,1 (Video 17,5, KI 19-23) - absolut also nicht zu dunkel.
  Aber der gelobte Koerper ist im Spiel heller als im Video (Brust 19,3 gegen 11,0), und der Nutzer sieht das Verhaeltnis: ein fast schwarzer
  Kopf auf einem hellen, rosa-grauen Koerper (Blatt B/D). Darum sind Auge, graue Seite und Haarseite (Punkt 2) nicht erkennbar.
- Widerlegung versucht: "Dunkel nur durch das RDT-Licht?" -> nein, Texelfarbe 0,67. "Nur in Pose 135?" -> nein, Pose 85 0,68, Spiel h105-h135
  18-21 L*. "Nutzer sagt selbst 'graeulich', also dunkel erlaubt?" -> grau ja, aber in seinem Video ist die graue Seite L* 24 bei Brust 11;
  neu ist die Kopfseite dunkler als die Brust.
- Bewertung: **Abweichung, die der Nutzer wieder ablehnen wuerde**. Hinweis fuer Runde 2 (letzte Runde dieser Spur): eine gleichmaessige
  Aufhellung der ganzen Zeile 486 (ein Faktor auf L*, Farbton und Adern bleiben) ist keine "Farbmischung"; Ziel nach den Vorlagen Kopf/Brust
  1,4-1,6 (Kopf-Median im RDT-Licht Pose 135 etwa 30-36 L*, wie Referenz 2 30,8). Dazu das Auge: ohne hellen Kern-Fleck (Video) oder dunkle
  Pupille in hellerer Haut (KI) bleibt es unsichtbar; der rot gluehende G2-Punkt kommt weder im Video noch in den KI-Bildern vor.

## (5) Schwanz wie in den KI-Bildern - Abweichung klein gegen Beschreibung2; gegen KI-Bild offen (Nutzerentscheid noetig)

- Bytegleich zu Runde 3: Meshes 16-22 (Ecken/Normalen/Flaechen/UV) und alle 10356 Schwanz-UV-Texel = ot3 (0 abweichend); Riegel T-K2s/T-K3
  OK. Form nach Beschreibung2 (Knick, Spitze auf 0,70 H2 an der Flanke, Texel aus dem Ref2-Atlas).
- KI (alle drei Bilder, RIGHT/BACK/LEFT): ein schwerer C-Bogen aus dem Nacken ueber den Ruecken bis zum Boden (1,2-1,4 H, Leser B D7) mit
  Klingenspitze, geschuppt. Die Beschreibung2 des Nutzers (eigene Zeichnung) endet in Huefthoehe mit Knick. Der Bauer hat den Schwanz laut
  Auftrag nicht angefasst ("bleibt, sofern Leser B nichts anderes belegt" - Leser B belegt den Widerspruch, entschieden ist er nicht).
- Bewertung: gegen Beschreibung2 **Abweichung klein** (unveraendert seit der Lesung Runde 3). Gegen den Nutzer-Satz "den Schwanz" (KI) ist es
  eine grosse Abweichung, die ich nicht als dritte Klasse zaehle, weil der Nutzer selbst beide Vorlagen geliefert hat und der Orchestrator den
  Entscheid noch holen muss. Offen melden.

## (6) Koerper unveraendert Original - wie Vorlage

- Meshes 0-8 und 10-15: Ecken, Normalen, Flaechen, UV, CLUT-Verweise bytegleich zu EM30_HANGAR.EMD (mess_re2.json koerper_0_8_10_15_eq_ref2
  true, koerper_tuv_quv_eq_ref2 true); CLUT-Zeilen 480-485 (Index 0-5) bytegleich; alle 34716 vom Koerper belegten Texel bytegleich zu
  Referenz 2 (0 abweichend; Bauer T-K1c gleiche Zahl). Texel gegen ot3 nur im Kopfbereich veraendert (11629 abweichend, 0 ausserhalb
  x 0-125 / y 160-249 und x 256-267 / y 200-235). CLUT-Zeilen 8 (Texturhoehe 256 x 8 = 2048 <= 4096).
- Spiel-Asset EM30_HANGAR.EMD cfd1ac42 unveraendert; Koerper-Lab im Render identisch zu Referenz 2 (22,6 / 22,7, Anteile gleich).
- Bewertung: **wie Vorlage**.

## Zusammenfassung je Punkt
| Punkt | Soll | Ist | Klasse |
|---|---|---|---|
| 1 Monsterkopf-Modell mit Maul/Zaehnen | KI: Kuppel mit Schnauze, offenes Maul, zwei Zahnreihen | G2-Kopf 48 Ecken, Zahn-Kachel G3 auf 11 Unterseitenflaechen; Zaehne im Render (18 / 58 px) und rohen Spielbild sichtbar; Keil kantiger als KI-Kuppel | wie Vorlage (Modell) / klein (Form) |
| 2 Auge links, Haare rechts, Nase | Beschreibung1 / KI: Auge klar, rechte Seite braun-dunkler | Lage K4 0,060 / 0,097 H OK; Auge 1-2 px roter Punkt im Spiel; Haelften 15,4 / 15,9 L* - kein Gegensatz | ablehnen |
| 3 Groesse und Sitz | KI Kopf 0,20-0,22 H hoch, 0,24-0,29 Schulterbreite, zwischen den Schultern | 0,170 H, 0,261 Schulter, zwischen den Schultern vor dem Buckel | wie KI-Bild |
| 4 Hautfarbe zum Koerper | keine Fremdtoene; Kopf heller als Brust (Video 1,59, KI 1,6-2,2) | oliv 0,028 / violett 0 / blau 0, zwei RE2-Zeilen; aber Kopf/Koerper 0,69 (Texelfarbe 0,67) - dunkelstes Teil der Figur | Farbton wie Vorlage / Helligkeit ablehnen |
| 5 Schwanz | KI: C-Bogen bis zum Boden; Beschreibung2: Knick, Ende 0,70 H2 | bytegleich Runde 3 nach Beschreibung2 | klein (Beschreibung2); gegen KI offen |
| 6 Koerper | bytegleich Referenz 2 | Meshes, UV, CLUT 480-485, 34716 Texel: 0 abweichend | wie Vorlage |
