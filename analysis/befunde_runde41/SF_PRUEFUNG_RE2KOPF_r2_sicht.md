# SF V5 Kopf aus RE2 Runde 2 - Sicht-Pruefung (Pruefer, 2026-11-07 nachts, nach Bauer-Commits aa6c8a181 / 24c3d423a / c8936d133)

Gegenstand: Variante C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/sfv4/emd/EM30_V4re2.EMD
= EM30_V4.EMD (sha256 e9c66c2edcb846f8613ea2f309810bb82b09bb7b4e25bd0917ba7ae9297363a5, 262796 Byte, selbst gemessen). Basis EM30_V4ot3.EMD
(a1bc9309). Referenz 2 = C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel/re15_port/shared_assets/PSX/EMD/EM30_HANGAR.EMD (cfd1ac42, unveraendert).

Nutzer-Saetze, gegen die gelesen wird: "bei den KI Bildern kann man so verdammt gut sehen wie das verschwommene Bild eigentlich aussehen muss....
Man sieht alles einwandfrei! Das Auge die Haare, den Mund, den Schwanz" / "Vielleicht kannst du dir auch einen Birkin aus Resident Evil 2 nehmen" /
"Linke Seite ist so graeulich. Bis Nase. Dann rechts Seite sieht man Haare - braeunlich.... Warum mischst du die Farben so daemlich?"

Gelesen: Bauer-Leisten C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel/analysis/befunde_runde41/SF_belege/v4/01_vorn_kamera12.jpg,
02_seite_wie_bild2.jpg, 03_kopf_nah_vorn_x4.jpg, 04_kopf_nah_seite_x4.jpg, 08_rundum_V4.jpg, 10_kopf_ki_re2_neu_x6.jpg, 11_spiel_koepfe.jpg;
Runde-1-Leisten in SF_belege/v4/re2r1/; Suchbelege SF_belege/v4/re2/suche/ (spiel_vgl.png, diag1.json, ebene.json); Riegel
SF_belege/v4/re2/riegel_re2kopf_V4re2.md (1 rot von 43 = T5), Protokoll kopf_re2_protokoll.json, lauf_V4re2b.txt (EXIT=0, 191 Bilder),
ctest_v4_re2.txt (4/4); Dossier SF_sichel.md Abschnitt "V5 Kopf aus RE2 - Runde 2" (Zeilen 2176-2290); KI-Bilder
C:/workspace/git/reAi_v2/Version_80%/image_1.png, "image_2 (2).png", "image_3 (3).png"; Beschreibung1.png, Beschreibung2.png, Vorderseite_Beschreibung.png.

Eigene Gegenprobe (unabhaengig von leisten_re2kopf.py / riegel_re2kopf.py), Skripte und Bilder in
C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/sicht_re2_r2/
(die Runde-1-Skripte, auf Lauf V4re2b und die neue Variante gestellt):
- render_re2.py: v4_gemein.zeichne (b2, Kamera 50A0/12, RDT-Licht und Texelfarbe) fuer Referenz 2 / ot3 / neu, Posen 135 / 85, Id-Puffer.
- mess_re2.py -> mess_re2.json: Mesh-Bytevergleich, CLUT-Zeilen, Kopf-/Koerper-Lab, Farbton-Anteile, Haelften, Groessen; dieselben Masse an den
  sechs KI-Kopfboxen und an Vorlage 1/2.
- roh_uv_re2.py -> roh_uv.json, D_roh_dumps.jpg: Koerper-/Schwanz-UV-Texel gegen Referenz 2 / ot3; rohe RE15_FRAMEDUMPs 960 x 720 h105-h135.
- kacheln_re2.py -> A_kopf_ki_neu_spiel.jpg (KI-Koepfe | Referenz 2 | neu Licht | neu Texelfarbe | Spiel h135/h85 | Video, 240 px),
  B_spiel_folge.jpg (h85-h135 Referenz 2 / neu), C_figur.jpg, spiel_mess.json.
- NEU adern_mess.py -> adern_mess.json, E_adern_hell.jpg: Anteil heller Bildpunkte (L* > 55, L* > P50+20), helle unbunte / helle rosa Punkte,
  Farbton der grauen Haut (a*/b* der Punkte L* 30-65, Chroma < 25; Anteil b* < 0), Violett-/Blau-Anteil - fuer KI-Koepfe, Video, Referenz 2
  und neu (Render Id 9 und rohe Dumps); auge_x6.py -> F_auge_x3.jpg (KI-Koepfe 300 px | Referenz 2 roh | neu roh, Kopf 100 px x3, ohne Marken).
Bauer-Zahlen reproduziert: Kopfbox neu Pose 135 (325,160,384,221) = 60 x 62 px, H 364 -> 0,165 x 0,170 H; Kopf P50 L* 23,8 (RDT-Licht),
Haelften 29,0 / 21,0; Pose 85 37,0; 9 CLUT-Zeilen (Kopf 486/487/488, Koerper 480/481/482/484); Koerper-Meshes 0-8/10-15 und 34716
Koerper-Texel bytegleich Referenz 2; Schwanz-Meshes 16-22 und 10356 Schwanz-Texel bytegleich ot3.

## Ergebnis: NICHT bestanden (ein Punkt der dritten Klasse, eng umrissen: das Adern-Netz der G2-Kuppe ist nach der Belichtung x 1,9 HELLER
## als die Haut und damit der hellste Fleck der ganzen Figur - im Spielbild liest sich der Kopf als grau-lila Keil mit leuchtenden
## weiss-rosa Linien, was kein Vorbild zeigt; alle uebrigen Punkte sind "wie Vorlage" oder "klein")

Was gegen Runde 1 besser ist (gemessen): Der Kopf ist kein schwarzer Klotz mehr - Kopf/Brust im registrierten Spielbild h135 1,30 (Runde 1 1,07,
Referenz 2 1,44, Video 1,59); die Haelften sind jetzt unterschieden (rohes Spielbild h135 links 27,7 / rechts 21,7 L*; Runde 1 mal links,
mal rechts dunkler; Video 24,3 / 14,2); das Auge ist statt eines 1-2-px-Punktes eine dunkle Hoehle von 0,044 x 0,071 H (Faecher L* 18
gegen Umgebung 36 im Videoraum); die Zaehne sind im rohen Spielbild 158 Punkte L* > 60 / Chroma < 20 (Runde 1: 0-15 bei L* > 55).

## (1) Echtes Monsterkopf-Modell mit Maul und Zaehnen - wie KI-Bild/Vorlage (Modell), Abweichung klein (Form, Maul als Texturband)

- Geometrie unveraendert Runde 1: RE2 G2 (kind 0x31) Mesh 3, 48 Ecken / 56 Dreiecke / 14 Vierecke, Herkunft byte-true im Protokoll
  (CDEMD0.EMS Sektor 2944, TIM Sektor 2895; Zahn-Kachel G3 0x33 TIM Sektor 3065 u 2-13 v 5-40). Kein gemalter Fleck, kein Block.
- Maul: in den rohen Dumps (D, F) an der Schnauzen-Unterseite ein dunkelrotes Kieferband mit heller Zahnreihe, bei h105-h135 jedes Mal da;
  im Render Pose 135 Zahn-Box (329,166)-(371,207). Von vorn liest es sich als geoeffnetes Maul mit Zaehnen, aber als Band, nicht als
  Geometrie (KI: aufgerissener Kiefer mit zwei Reihen). Bei 640 x 480 ist die Zahnreihe ein heller Saum, einzelne Zaehne nicht.
- Form: Keil (Kuppe, Schnauze vorn-unten) gegen die glatte Kuppel der KI-Koepfe; seitlich (Leiste 04, A Zeile 2) stimmt die Silhouette.
- Widerlegung versucht: "Maul nur im Marker-Render?" -> nein, ohne Marker in allen vier rohen Dumps. Bewertung: Modell **wie Vorlage**,
  Form/Maul **Abweichung klein** (unveraendert Runde 1).

## (2) Auge Bild-links, dunkle Haarseite Bild-rechts, Nase/Maul-Lage - Abweichung klein

- Lage (Bauer-Marker reproduziert): K4 0,060 H links / 0,097 H unter OK; K7 Zahn-Schwerpunkt 0,60 / 0,66 der Kopfbox; S2 0,043 / 0,812 H -
  alle in den Messblatt-Fenstern. Im Render und im rohen Spielbild (F) sitzt die dunkle Augenhoehle links unten in der Kopfbox, die dunkle
  braun-graue Seite rechts, das Maul unten rechts: Beschreibung1 (Auge links, Haare rechts, Nase) ist in der Lage getroffen.
- Haarseite: rohes Spielbild h105/h118/h125/h135 links 26,7/27,7/27,8/27,7 gegen rechts 23,0/24,0/21,6/21,7 L* (Verhaeltnis 0,78-0,86;
  Video 0,58; Referenz 2 0,76-1,08 - dort zufaellig). Die dunkle Zone ist braun-grau (KV2 a* 10,6 / b* 6,3, Bauer) - "braeunlich" im Sinn
  des Nutzers ist das knapp; es ist eher ein dunkles Grau mit Rotstich.
- Auge: im Spielbild eine dunkle Hoehle ohne lesbaren Glanzpunkt (Glanzkern L* 29,7 gegen Faecher 10,6 im RDT-Licht; der Lichtbeleg des
  Bauers - Augenfaecher im Umgebungslicht 0,24 - ist nachvollziehbar). KI zeigt eine schwarze Pupille mit weissem Glanz in grauer Haut, das
  Video einen hellen Fleck. Bei x3 (F) erkennt man die Hoehle als Auge, bei 640 x 480 (B) ist es ein dunkler Punkt am linken Kopfrand.
- Widerlegung versucht: "zweites Auge sichtbar?" -> Spiegelbox 28,3 <= Kopf-Median 29,2 (Bauer K6); im rohen Dump h118 ist rechts der
  Hoehle eine zweite dunkle Mulde (der angedunkelte Gesichtsspalt u 41-60), nicht als Auge lesbar. Bewertung: **Abweichung klein**
  (Lage wie Vorlage; Auge als Hoehle statt Pupille mit Glanz; Haarseite dunkel, aber nur schwach braun).

## (3) Groesse und Sitz des Kopfes - wie KI-Bild/Vorlage

- Pose 135: Kopfbox 60 x 62 px bei H 364 -> 0,165 x 0,170 H; Breite zu Schulterbreite 0,261 (KI 0,235-0,291; Referenz 2 0,199).
  KI-Kopfhoehe 0,200-0,222 H inkl. Unterkiefer - neu 0,170 ist am unteren Rand, Messblatt K10 (0,14-0,18 x 0,15-0,20) erfuellt.
- Sitz zwischen den Schultern vor dem Buckel (C, Leiste 01); im Spiellauf h85/h95 liegt der Kopf hinter dem Buckel (Lauf-Pose, bei
  Referenz 2 genauso). Bewertung: **wie KI-Bild/Vorlage** (unveraendert Runde 1).

## (4) Hautfarbe des Kopfes zum Original-Koerper - Abweichung, die der Nutzer wieder ablehnen wuerde (Adern-Netz heller als Haut; kuehler Grauton)

Zuerst die Einwaende, die ich NICHT halten konnte (adern_mess.json):
- "Zu viele helle Punkte": Anteil L* > 55 im Kopf, rohes Spielbild h105-h135: 3,0-4,3 %; KI-Koepfe 5,5-12,0 %; Video 10,3 %; Referenz 2 roh
  1,6-2,5 %. Anteil L* > P50+20: neu 9,3-13,8 %, KI 12,2-21,2 %, Video 18,5 %, Referenz 2 3,9-4,9 %. Die MENGE heller Punkte liegt in der
  KI-Spanne; Referenz 2 liegt darunter. Einwand entkraeftet.
- "Violett": neu roh 1,2-1,8 % (Chroma > 10, 270-345 Grad), Render Pose 135 1,2 %; der gelobte Koerper selbst hat 3,1 %. Nur Pose 85
  (Render, Kopf im Hauptlicht) 6,1 % - im Spiellauf ist der Kopf in dieser Pose hinter dem Buckel. Einwand entkraeftet (Pose 85 als Hinweis).
- "Oliv": 0,03 (Runde 3 hatte 0,128). Kein Einwand.
Was bleibt und was der Nutzer sieht (A, B, D, E, F):
- Die G2-Adern sind in der Quelle ROT auf dunkelgrauer Haut (Leiste 10, "RE2 G2 0x31 Mesh 3 vorn (Quelle)"). Nach der Belichtung x 1,9 der
  Zeile 486 verlieren 49 % der Eintraege Chroma (Bauer: "die Adern der grauen Seite werden blasser") - die Adern sind jetzt WEISS-ROSA und
  HELLER als die Haut: hellster Kopfpunkt im Render Pose 135 (344,167) L* 81,4 (Bauer: 69,2 im Videoraum), im rohen Spielbild P99 66,7-71,6,
  waehrend der ganze Koerper P99 48,3 hat. Der hellste Fleck der Figur ist damit ein Linienmuster auf der Kuppe (E: cyan = L* > 55 - bei
  Referenz 2 und bei den KI-Koepfen liegen diese Punkte auf Zaehnen, Glanz und Maul, bei neu auf den Adern der Kuppe und am Nasenruecken).
- Am Koerper ist der Kontrast umgekehrt: dunkelrote Adern auf heller Haut (Referenz 2). Kopf: helle Adern auf dunklerer Haut. Zwei Materialien
  nebeneinander - genau das "Mischen", das der Nutzer zweimal abgelehnt hat (Runde 3 Vierfarben, Runde 1 schwarz/rosa).
- Grauton: die graue Kopfhaut ist kuehler als jedes Vorbild - Anteil b* < 0 unter den grauen Punkten: Render 40 % (Pose 135) / 31 % (85),
  rohes Spielbild 15-22 %; KI-Koepfe 0-10 %, Video 10 %, Referenz 2 < 1 %; Mittel b* 2,3-3,8 (KI 3,7-6,2). Im Spielbild wirkt der Kopf
  silber-lila (A "neu SPIEL h135", F), die KI-Koepfe warm braun-grau.
- Kein Vorbild (drei KI-Bilder, Video Vorlage 1/2) zeigt helle Linien auf der Kopfkuppe. Der Nutzer-Satz "Linke Seite ist so graeulich" meint
  eine ruhige graue Flaeche (Video: Kopf L* 24 glatt), nicht ein Adern-Netz.
- Widerlegung versucht: "Bei 640 x 480 verschwindet das Muster?" -> nein, B h118-h135: der weisse T-foermige Strich auf der Kuppe ist das
  auffaelligste Detail des Kopfes. "Ist das nicht die Texel-Treue (byte-true G2)?" -> die Texel sind bytegleich, aber die Palette ist eine
  Port-Wahl (Zeile 486 = Transformation); der Effekt ist eine Folge der Wahl, nicht der Quelle.
- Bewertung: **Abweichung, die der Nutzer wieder ablehnen wuerde** - eng umrissen auf die Palettenzeile 486: die Eintraege mit hoher
  Quell-Chroma (die Adern, a* > 20 in der Quelle) duerfen nach der Belichtung nicht heller als der Haut-Median werden (am Koerper sind die
  Adern dunkler als die Haut); dazu der Farbton der grauen Eintraege waermer (b* >= 0, wie KI 3,7-6,2). Das ist ein Palettenregel-Fix, kein
  Malen und keine Geometrie.

## (5) Schwanz wie in den KI-Bildern - Abweichung klein gegen Beschreibung2; gegen KI-Bild offen (Nutzerentscheid noetig, unveraendert)

- Bytegleich zu Runde 3 (Meshes 16-22, 10356 Texel: 0 abweichend; Riegel T-K2s/T-K3 OK). Form nach Beschreibung2 (Knick, Ende in Huefthoehe).
- KI: C-Bogen aus dem Nacken ueber den Ruecken bis zum Boden mit Klingenspitze (alle drei Bilder). T5 (Breite nach oben f 0,5 0,098 statt
  0,125) ist seit Runde 5 rot und bekannt.
- Bewertung wie Runde 1: gegen Beschreibung2 **Abweichung klein**; gegen den Nutzer-Satz "den Schwanz" (KI) gross, aber der Nutzer hat beide
  Vorlagen geliefert und der Orchestrator muss den Entscheid holen - nicht als dritte Klasse gezaehlt, offen gemeldet.

## (6) Koerper unveraendert Original - wie Vorlage

- Meshes 0-8 und 10-15 bytegleich EM30_HANGAR.EMD (mess_re2.json), CLUT-Zeilen 480-485 gleich, 34716 Koerper-Texel 0 abweichend
  (roh_uv.json); Koerper-Lab im Render identisch Referenz 2 (22,6 / 22,7). Spiel-Asset cfd1ac42 und Pin unveraendert. 9 CLUT-Zeilen,
  Texturhoehe 256 x 9 = 2304 <= 4096. Bewertung: **wie Vorlage**.

## Zusammenfassung je Punkt
| Punkt | Soll | Ist | Klasse |
|---|---|---|---|
| 1 Monsterkopf-Modell mit Maul/Zaehnen | KI: Kuppel mit Schnauze, offener Kiefer, zwei Zahnreihen | G2-Kopf 48 Ecken, Zahn-Kachel G3 als Band an der Unterseite; in allen rohen Dumps sichtbar (158 px L* > 60) | wie Vorlage (Modell) / klein (Form) |
| 2 Auge links, Haare rechts, Nase | Beschreibung1 / KI: Auge klar, rechte Seite braun-dunkler | K4 0,060 / 0,097 H; Auge = dunkle Hoehle 0,044 x 0,071 H ohne Glanz; roh links/rechts 27,7 / 21,7 (0,78); dunkle Zone a* 10,6 b* 6,3 | klein |
| 3 Groesse und Sitz | KI 0,20-0,22 H, 0,24-0,29 Schulter | 0,170 H, 0,261 Schulter, zwischen den Schultern | wie KI-Bild |
| 4 Hautfarbe zum Koerper | ruhige graue Seite, warm braun-grau, Adern wie am Koerper dunkler als Haut; keine Fremdtoene | helle Punkte in KI-Spanne, oliv/violett unauffaellig; ABER Adern weiss-rosa und heller als Haut (Kopf L* max 81, Koerper P99 48), hellster Fleck der Figur auf der Kuppe; Grau kuehl (b* < 0 bei 15-22 %, KI <= 10 %) | ablehnen (Palettenzeile 486) |
| 5 Schwanz | KI: C-Bogen bis Boden; Beschreibung2: Knick, Ende Huefte | bytegleich Runde 3 nach Beschreibung2; T5 rot bekannt | klein (Beschreibung2); gegen KI offen |
| 6 Koerper | bytegleich Referenz 2 | 0 abweichend (Meshes, UV, CLUT 480-485, 34716 Texel) | wie Vorlage |

Belegbilder dieser Pruefung (alle unter C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/sicht_re2_r2/):
A_kopf_ki_neu_spiel.jpg, B_spiel_folge.jpg, C_figur.jpg, D_roh_dumps.jpg, E_adern_hell.jpg, F_auge_x3.jpg; Zahlen mess_re2.json,
spiel_mess.json, roh_uv.json, adern_mess.json, dateien.json.
