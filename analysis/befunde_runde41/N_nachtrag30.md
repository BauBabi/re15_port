# Runde 41 — Spur N "nachtrag30" (Dossier, fortlaufend)

Baum `.claude/worktrees/r41_nachtrag30`, Zweig `r41/nachtrag30`, Basis cd48e3c7 (= Merge master 21aad0c3).
Auftrag: AUFTRAG.md Nachtrag 30 / 30b / 30c (Wortlaut massgeblich). Zuteilung VERTRAG 1.2 "N": Bits 100-103/106/107/125
(+126-133 nach Zensus), Ids 2000/20B0 40, 2030 40-55, 2010/2011 40-44, 2050 40-44; Lautstaerke-Modul der Spur D.

## 0. Nutzer-Frage (Nachtrag 30c, Wortlaut) und Antwort

> Aber ich raffe es immer noch nicht... wie kommst du auf 9, 2, 1, 8? Wir haben 6 valves: 1 - schwarz, 2 - grün. 3 - rot,
> 7 schwarz, 8 grün, 9 rot und dann das Plakat. Ich wollte sagen das es laut Plakat "Grün" und "Schwarz" vertauscht hat,
> also 9 und 2. Wie kommst du auf die Lösung?

Waehrend dieser Spur kamen Nachtrag 30d (17:23, Plakat NEU 1. GREEN 01 - 2. RED 02 - 3. BLACK 09 - 4. RED 08, Loesung
09 -> 02 -> 01 -> 08; ersetzt 30c) und 30e (Wasser in 2010 bis zur Flut) hinzu (vom Orchestrator in diesen Zweig committet,
4bd6c7e2; master danach gemergt 89dfbe48). Antwort/Rechenweg fuer den Nutzer: §3.

## Punkte (Stand nach 30d/30e)

1. Schrank 2010 klappt auf -> Sicherungskasten wie 2060 (fuse_box.bmp), Mechanismus wie 2060.
2. Panel-Raetsel 2030 (Druck 80) — Entwuerfe zur Freigabe (gebaut wird erst nach Freigabe).
3. Notiztext ("green and red" + Nummern "black"/"green") + Plakat NEU + Loesung 09 -> 02 -> 01 -> 08 (Nachtrag 30d).
4. RE2-Ton der Plattformfahrt 2030.
5. RE2-Alarmton zusaetzlich durchgehend (Warntext 46 bis Flut/Verlassen).
6. Lautstaerke: ALLE Toene (Tuersequenzen, Wassersprung ...).
7. (30e) 2010: Wasser die Leiter herunter und ueber den Weg zum Becken in der Mitte, bis zur Flut in 2030.

## 2. Panel-Raetsel 2030 — Entwuerfe zur Freigabe (Nachtrag 30/30b; gebaut wird erst nach der Freigabe)

Ziel aller Entwuerfe: die Druckanzeige zeigt 80, danach die bestehende Frage Id 43 "The pressure gauge reads 80. / Lower the
bridge?". Bilder (320x240, doppelt): `N_belege/panel_entwurf_a.png`, `_b.png`, `_c.png`; Kamera und Ausschnitt:
`N_belege/panel_ausschnitt.png`.

**Kamera (gemessen).** Das Nutzerbild ist Cut 2: schaltschrank.png = Cut 2 um 10 px versetzt (Schablonenvergleich 0,999 bei
Massstab 1,0; naechster Cut 0,25). `ROOM2030.RDT` Cut 2 @0x0A0 (3883,-8101,-7091)->(-11568,-5157,-9778) zeigt das ganze Panel
schraeg, der linke Rand faellt aus dem Bild. Cut 1 @0x080 (-3858,-8101,-3691)->(11593,-5157,-12615) zeigt es fast von vorn, der
untere Teil liegt hinter dem Gelaender. Beide fov 26684 -> H 208. Aus 15 Punkten in beiden Cuts trianguliert (Strahlenabstand
<= 42): Tafelebene z -10550, Scheiben ca. 35 davor, Tafel ca. x -920..670, y -9030..-6390. Beide Cuts sind darauf entzerrt; die
Quelle hat rund 3 px je 100 Einheiten.

**Was wirklich auf dem Panel ist:** fuenf Roehrenelemente (Paar, einzeln, Paar: grauer Kopf, helles Schauglas oder Griff, Loch im
Fuss); eine runde Anzeige; vier gleiche runde Scheiben in einer Reihe (weiss, schwarzer Balken ueber den Durchmesser); Schild oben
rechts (rotes und helles Feld); rotes Warnschild; weisses Schild im Metallrahmen; blauer Kasten; fuenf runde Tasten (eine einzeln,
vier in einer Reihe unter den Scheiben, darunter je ein schwarzes Feld). Der Beleg-Bogen benennt alle.

**Vorbilder, nicht kopiert:** 2050 = vier Hebel ein/aus, feste Werte 30/20/40/10, Summe 80. 11F0 = zehn Schalter mit + und -,
Summe 80. Beide Original-Nahansichten (2050 Cut 15, 11F0 Cut 10) tragen denselben blauen Stahlrahmen (Zeilen 0..15 und 224..239
bitgleich).

**Gemeinsam:** Rahmen pixelgleich aus 2050 Cut 15, EXIT-Feld pixelgleich aus 11F0 Cut 10 (x 210..238, y 175..204), Cursor aus einem
echten Spielbild 2050. Lage, Farbverlauf und Wand stammen aus dem entzerrten Echtbild; Roehren, Scheiben und Schilder sind an den
echten Stellen neu gezeichnet, weil die Quelle dafuer zu grob ist. Ausschnitt = oberer Teil der Tafel, unten sichtbar die
Oberkanten von weissem Schild und blauem Kasten. Schild oben rechts neu beschriftet: BRIDGE / PRESSURE 80. Bedienung wie 2050:
Steuerkreuz bewegt den Cursor, SQUARE betaetigt, CROSS oder EXIT verlaesst das Panel. Eingang "Will you operate the panel?"
(Frageform ROOM2050 msg 0). Alle Raetselwerte und Texte sind PORT-WAHL. Beim Bau entfaellt Ventil 13 (Slot 13, Ereignis 25, Texte
41/42/47), Bit 101 bedeutet dann "Druck 80 erreicht". Die Zeit unter einer Minute ist aus der Zugzahl geschaetzt, nicht am
Spieler gemessen. Pruefung der Regeln: Durchsuchen aller Zustaende (Werte je Entwurf).

### Entwurf A "Druckspeicher" (`panel_entwurf_a.png`, Stand 50/20/30, Quelle C gewaehlt)

* Aufbau: drei Tanks auf einer Skala, A = linkes Paar (fasst 100), B = einzelne Roehre (50), C = rechtes Paar (30). Das Fenster im
  Metall ist so lang, wie der Tank fasst. Die Anzeige liest Tank A, rote Marke bei 80.
* Bedienung: Cursor auf einen Tank, SQUARE waehlt die Quelle (gruene Lampe), Cursor auf einen zweiten Tank, SQUARE fuellt um.
* Regel: es fliesst, bis die Quelle leer oder das Ziel voll ist.
* Loesung: Start 100/0/0; A nach B (50/50/0), B nach C (50/20/30), C nach A (80/20/0).
* Weg dorthin: die Nadel steht ueber der roten 80, es muessen 20 weg; beim Probieren bleiben in B genau 20 zurueck, 50 + 30 = 80.
* Fehlversuch: gibt es nicht, jeder Stand fuehrt weiter; EXIT setzt auf den Start zurueck. Leere Quelle oder volles Ziel: Fehlerton
  wie 2050 und ein Satz.
* Texte: "The pressure gauge reads 100." / "Pick a tank to drain, / then a tank to fill." / "The tank is empty." / "The tank is full."
* Geprueft: 16 Zustaende, keine Sackgasse, genau eine kuerzeste Loesung (3 Zuege), von jedem Stand hoechstens 4 Zuege bis 80.

### Entwurf B "Stellventile" (`panel_entwurf_b.png`, Stand 2-0-1-2, Anzeige 20)

* Aufbau: die vier Scheiben sind Drehventile in einem aufgedruckten Leitungsbild von PUMP zur Anzeige. Offen = Balken in Linie mit
  der Leitung (senkrecht), Lampe darueber leuchtet. Je offenes Ventil 20 auf der Anzeige.
* Bedienung: Cursor auf eine Scheibe, SQUARE dreht 60 Grad.
* Regel: jede Scheibe nimmt ihre rechte Nachbarin mit, die letzte dreht allein. Der Balken steht nach drei Klicks wieder senkrecht.
* Loesung: Start 2-0-1-2 (Rasten, 0 = senkrecht): Ventil 1 einmal, Ventil 2 zweimal, Ventil 4 einmal; Reihenfolge egal.
* Weg dorthin: schon der erste Klick zeigt die Kopplung; von links nach rechts klappt es immer.
* Fehlversuch: gibt es nicht, weiterdrehen; EXIT setzt auf den Start zurueck.
* Texte: "The pressure gauge reads 20." / "A valve is open when it stands / in line with the pipe." / "The valves are linked."
* Geprueft: 81 Zustaende, jeder loesbar, aus diesem Start 4 Klicks (Anzeige 20, 20, 20, 60, 80 in der genannten Folge), aus dem
  schlechtesten Start 8.

### Entwurf C "Pumpe und Ablass" (`panel_entwurf_c.png`, Stand 60, Hitze 2)

* Aufbau: linkes Paar = Griff PUMP, rechtes Paar = Griff BLEED, einzelne Roehre = OIL TEMP in drei Stufen (oberste rot), Anzeige =
  Druck. Rotes Schild: DO NOT OVERHEAT / RELIEF VALVE.
* Bedienung: Cursor auf einen Griff, SQUARE zieht ihn einmal.
* Regel: PUMP +30 Druck, +1 Hitze. BLEED -10 Druck, -1 Hitze (nicht unter 0). Hitze 3 oder Druck ueber 100: das Ueberdruckventil
  bricht auf, alles zurueck auf 0.
* Loesung: PUMP, PUMP, BLEED, PUMP oder PUMP, BLEED, PUMP, PUMP. Dreimal PUMP geht nicht, die dritte Pumpe ueberhitzt.
* Weg dorthin: die Werte stehen an den Griffen, die Hitze steigt sichtbar; wer ueberhitzt, sieht die Regel.
* Fehlversuch: Zischen, Nadel und Hitze fallen auf 0, Satz, neuer Versuch.
* Texte: "The pressure gauge reads zero." / "The oil overheated. / The relief valve blew!" / "The pressure is too high. / The relief
  valve blew!"
* Geprueft: 23 Zustaende, keine Sackgasse, genau die zwei genannten kuerzesten Folgen (4 Zuege), von jedem Stand hoechstens 5 Zuege.

### Empfehlung

**Entwurf A.** Er nutzt das Auffaelligste des Panels (Roehren neben der Anzeige), hat nichts zum Ein- und Ausschalten und ist darum
am weitesten von 2050 entfernt, und die Loesung hat einen echten Aha-Moment ohne Rechenarbeit. Kein Fehlversuch, keine Sackgasse.
Einziger Punkt zum Entscheiden: die Nadel startet bei 100 und muss herunter. B ist 2050 am naechsten (vier Elemente, je 20). C ist
die Wahl, wenn ein Fehlversuch mit Folgen gewuenscht ist.

## 6. Lautstaerke — Messung vorher (Stand 89dfbe48 = master + nichts geaendert; exe 17:23)

Nutzer-Einstellung im Hauptbaum (nur gelesen): `re15_port/build/platform/pc/re15_options.cfg` = `volume=20` / `sound=mono`
(geschrieben 16:48, drei Minuten vor Nachtrag 30).

* Weg der Toene (Code gelesen): EIN SDL-Geraet (audio_pc.c re15_audio_init, `SDL_OpenAudioDevice` mit `audio_callback`); in
  audio_callback laufen nacheinander s_active (alle SE: Raum/Kern/Waffen/RE2-Tuerbank `re15_audio_re2_tuer_se` der
  Tuersequenz door_scene_pc.c, Schritte `re15_audio_footstep` = auch Wasser-Schritt/Landung), s_xa (Stimmen), s_fmv_audio (FMV),
  re15_ss_render_bgm (MAIN/SUB/SUB2 inkl. RE2-Alarm SUB_25), re15_amb_mix (Rotor) — danach Mono-Faltung und als LETZTES die
  Endstufe der Spur D (`(out*q)>>15`). Kein zweiter Ausgabeweg (grep SDL_OpenAudio/QueueAudio/PlaySound/waveOut: nur diese Stelle).
  Die Tuersequenz (door_scene_pc.c) pumpt denselben Mischer (`re15_audio_se_pumpe` / `re15_audio_tick`).
* Messung A (deterministisch, RE15_AUDIO_CAP_SYNC = dieselbe Mischkette ohne Geraet), Lauf 20A0 -> Tuer Slot 0 (Tuersequenz
  `DOOR26`, `[tuer] Bild 60 Se_on Tonkopf-Eintrag 0`, Schliesston) -> 2090 mit Sprung ins Becken (climb.log `sub=10 kind=0x02
  ph=4` = Landung), volume=100 gegen volume=50: **2 499 000 von 2 499 000 Samples exakt `v50 == (v100*0x4000)>>15`**, 785 Takte mit
  Ton, Verhaeltnis je Takt 0,5000 (Tuer-Takte 185..245 mit Spitzen bis 16681 -> 8340).
* Messung B (echter SDL-Rueckruf, RE15_AUDIO_CAP, Nutzer-Einstellung MONO) volume=100 gegen volume=20, gleicher Lauf:
  Spitze 17274 -> 3674 (0,213), Median-RMS 1468 -> 298 (0,203), 95-%-Spitze 9322 -> 1740 (0,187) — Echtzeit, darum nicht
  bitgleich. Werkzeug scratchpad `n30/laut/vergleich.py`, Laeufe `n30/laut/{land100,land50,rt100,rt20}`.
* Pruefung "fremde Prozesse": alle 707 Agenten-Laeufe seit 08:00 (debug.log im scratchpad) und 262 Test-Laeufe 16:00-17:10 in den
  Spurbaeumen liefen mit RE15_NOAUDIO oder CAP_SYNC (stumm), ausser einem Lauf 10:53 (int_kette) und meinen zwei Echtzeit-Laeufen
  17:32/17:33 (Messung B, waren hoerbar). Die Toene beim Nutzer kamen also aus seinem Spiel.
* Zwischenstand: der Mischweg skaliert JEDE Quelle, auch Tuersequenz und Wassersprung. Die Ursache der Beobachtung liegt woanders ->
  weiter (§6 unten).

## FORTSETZUNG 2026-10-30 (Protokoll, neueste zuerst)

Stand zu Beginn: master gemergt (89571618), Plakat 30d, Kopf `RE15_R41C_LOESUNG {5,1,0,4}`, Panel-Bilder, Lautstaerke-Messung vorher.
Massgeblich: Nachtraege 30, 30b, 30d, 30e, 34 (N-E2), 36, 37. Baseline-Bau 21:37 gruen.

## 3. Notiz und Loesung 09 -> 02 -> 01 -> 08

**Messung vorher:** `shared_assets/RE2/FILES/FILE31_p01/p02` trugen noch den Satz aus Nachtrag 13 ("They mixed up black, green, and red,
and swapped the numbers for black!"). Plakat (`gen/ventilkette_r41_plakat.inc`, 30d) und Kopf (`{5,1,0,4}`) waren schon neu.

**Rechenweg fuer den Nutzer:** Plakat "1. GREEN 01 - 2. RED 02 - 3. BLACK 09 - 4. RED 08". Notiz: (1) gruen und rot verwechselt, (2) die Nummern von
"black" und "green" (= der Zeilen BLACK und GREEN) vertauscht. Zeile 1 GREEN -> Farbe rot, Nummer aus der BLACK-Zeile 09 = 09 rot. Zeile 2 RED -> Farbe gruen, 02 =
02 gruen. Zeile 3 BLACK -> Farbe bleibt, Nummer aus der GREEN-Zeile 01 = 01 schwarz. Zeile 4 RED -> Farbe gruen, 08 = 08 gruen. Jede Zeile trifft ein echtes Rad
(Kopf "Ventil-RAEDER": 09 rot, 02 gruen, 01 schwarz, 08 gruen).

**Beleg Schrift:** Der Satz braucht `"`. Der Atlas (85 Zeichen, re2_doc_satz.atlas_bauen) kennt es nicht, weil es aus zwei Spalten-Segmenten besteht und die Zeile dort
verworfen wird. RE2 setzt es aber: 13 Zeilen in FILE03/04/10/17/20/23/24, darunter die Vorlage selbst (FILE17 S07 `the chief "to serve` / `and protect"!?`); in allen 13 gilt
Segmentzahl = Zeichenzahl + Zahl der `"`. Gemessen aus genau diesen Zeilen (`dokument_satz.py anfuehrung_messen`): zwei Formen, offen `25025/40040/65065`
(12 Belege), zu `52052/04004/56056` (12 Belege), links 1, Vorschub 9. re2_doc_satz.py bleibt unveraendert.

**Umsetzung:** `tools/r41_ventilkette/dokument_satz.py` ersetzt genau einen Satz (SATZ_ALT -> SATZ_NEU, Wortlaut Nachtrag 30b); Rest = Nutzerwortlaut Nachtrag 13.
FILE31 neu gesetzt und installiert: zwei Textseiten wie vorher (max_page 2). Rueckleseprobe am Artefakt: 77 Woerter WORTGLEICH, 0 Kernpixel ohne Glyphe.
Seite 2: `that they swapped the / numbers of "black" and / "green"! What are they? / Colorblind? How is anyone / supposed to work with / something like this....`.

## 1. Schrank 2010/2011 klappt auf (Nachtrag 30; Logik der Sicherung = Spur O)

**Messung vorher:** Aktion am Ventil-Sicherungskasten (Slot 8, x 4900..5900, z -3470..-2070) zeigte nur Text 40; Cut 3 blieb unveraendert (die Spur-C-Fassung hat kein Bild und keine Bewegung).

**RE-Belege:**
* `fuse_box.bmp` (320 x 240) ist ROOM2060 Cut 6: mittlere Abweichung 1,04 je Kanal bei Verschiebung 0; Cut 10 = 1,15, alle anderen Cuts >= 21,3 (Bildvergleich gegen alle BSS-Cuts von ROOM2060). Cut 10 ist derselbe Blick nach dem Einsetzen
  (sub19 @0x016C5 `4b 06 0a` Cut_replace 6 <-> 10).
* Nahansichten des Sicherungsfelds: Cut 8 = Sicherung fehlt, rote Lampe (sub18 @0x0168C `29 08`), Cut 9 = eingesetzt, gruene Lampe (sub19 @0x016A2 `29 09`).
* Form "aufklappen": Original-Hebel ROOM11F0 sub06 `2f 05 40 00` Speed_set(5,+0x40) @0x01332, For 16 `0d 00 04 00 10 00` @0x01336 = 16 Bilder, 64 je Bild = 1024 = 90 Grad. Die Original-RDTs haben keinen Schrank-Fluegel als Modell;
  der Fluegel selbst ist Port-Modell im Format des Aushangs.
* Das Original-Einsetzen in ROOM2060 hat einen Cursor-Schritt (obj 4, sub14..17 +-200, Member_cmp @0x01324 == 7, SQUARE @0x0132E -> sub19). Nicht gebaut: laut Auftrag aendert N nur Bild und Aufklappen, siehe Frage unten.

**Umsetzung:** `engine/src/schrank2010_r41.c`, `include/re15_schrank2010_r41.h`, `gen/schrank2010_r41.inc` (Generator `tools/r41_nachtrag30/schrank_bild.py`: Innenflaeche = Schrank aus BG06 auf die Rueckwand entzerrt, Viereck Q_INNEN; Tuer-Innenseite = Tuer aus
BG06, Viereck Q_TUER; Tuer-Aussenseite = BG03 auf die Tuerebene zurueckprojiziert, darum deckt sie sich geschlossen mit dem Hintergrund). Lage gemessen in Cut 3 @0xC0, Projektion H 208: Ostwand x 5400, z -3300..-2000, y -3650..-50, Angel (5400,-2000). Drei Port-Props obj 5/6/7,
Band 2, Licht neutral (`re15_schrank2010_r41_licht`: Textur 1:1, sonst schluckt das Raumlicht die Innenseite). Programme `2010_kasten_*` bekommen den Vorspann "Aufklappen" (Marker (5,0x1E), Sperre an/aus), der Ja-Zweig davor die Nahansichten (Marker (5,0x1D), je 30 Bilder = PORT-WAHL:
Sleep 20 aus ROOM2080 sub02 @0x00CD8 + 10 zum Lesen). Einsetz-Logik der Spur C (Fragen, Bit 125, Standbild, Ton) laeuft unveraendert dahinter. Der offene Zustand ist Raum-Scratch: beim naechsten Betreten geht die Tuer wieder auf (Original zeigt in 2060 den Schrank offen, die Tuer in 2010 ist Port-Form).

**Messung nachher (Leon + Elza, Riegel `integration_r41_nachtrag30_schrank_*`, `_laden_*`):** Klick F330 -> Text erst nach `[schrank2010] Tuer auf (rot_y 1024)`; Flaeche x 224..258 aendert sich um 25,7 Graustufen, Gegenprobe links 0,0. Ja-Zweig: F635 == ROOM2060 BG08, F665 == BG09 (100 % gleich), danach Bit 125, Standbild.
Bilder: `N_belege/schrank_cut3_zu.png`, `schrank_cut3_offen.png` (beschleunigter Renderer).

## 2. Panel-Raetsel 2030 (Nachtrag 37, Entwurf A) — Umsetzung

**Messung vorher:** Pult ohne Ventil 13 = Text 40 "kein Druck"; Ventil 13 am Tank (Slot, Ereignis, Texte 41/42).

**Belege Form:** Vorbild ROOM2050 (Cursor, Steuerkreuz je Bild, SQUARE, CROSS bricht ab, Bits (2,0)/(2,2) sub01 @0x021FA, zurueck sub10 @0x0240C). Cursor-Schritt ROOM2050 sub05..08 @0x023C2 `2f 02 c8 00` = +-200 Welt je Bild = 2,515 Bildpunkte unter Cut 15. Zielwert 80, Ruhe 30 Bilder, Toene: RE2 ROOM2130 sub04
@0x01752 / @0x0171C / @0x01762 / @0x01192 / @0x01800 (re15_panel_zeiger.h). Umfuellen in 16 Bildern wie ROOM11F0 sub06 @0x01336. Tankwerte 100/50/30, Start, Loesung, Text 43, EXIT = NUTZER-VORGABE Nachtrag 37. Die Pad-Sperre (2,7) waere falsch: sie maskiert die SCD-Pad-Woerter auf 0xf000, der Cursor stand still; (2,0)/(2,2) wie sub01.

**Umsetzung:** `engine/src/panel2030_r41.c`, `include/re15_panel2030_r41.h`, `gen/panel2030_r41.inc` (Generator `tools/r41_nachtrag30/panel_kit.py`, `panel_bild.py`), Programm `2030_panel_ein`, `panel30_tick` in ventilkette_r41.c, Vollbild-Ebene ventilkette_r41_pc.c, Textebene `re15_render_pc_fmv_text` (render_pc.c).
Bit 101 heisst jetzt BIT_DRUCK80. Texte: 40 Frage, 41 Anleitung ("The pressure gauge reads 100." / "Pick a tank to drain, then a tank to fill."), 42 "The tank is empty.", 47 "The tank is full.", 43/44 wie bisher. Ventil 13 (Slot, Rechteck, Programme `2030_v13_*`, `2030_pult_ohne`) entfernt.
Vollsuche im Riegel: 16 Staende, keine Sackgasse, kuerzeste Loesung A->B, B->C, C->A.

**Messung nachher (Leon + Elza, echte exe):** Tasten-Skript: Pult -> Frage 40 -> Ja -> `[panel2030] offen: A 100 B 0 C 0` -> Text 41 -> `Tank B: umgefuellt -> A 50 B 50 C 0` -> `Tank C: umgefuellt -> A 50 B 20 C 30` -> `Tank A: umgefuellt -> A 80 B 20 C 0` -> `Anzeige 80 erreicht` -> 30 Bilder -> `GELOEST` -> Text 43 -> Ja -> Cut 4 ->
Bruecke -> Text 44. Das gleiche nach CONTINUE und nach der Ring-Tuer 2070 Slot 1 @0x00A5A zu Fuss zum Pult (`_laden_*`). Fenster Tank B im Bild: leer 28, voll 146 (F780/F900). Bilder: `N_belege/panel_spiel_start.png`, `panel_spiel_a_nach_b.png`, `panel_spiel_80.png`.

## 4. RE2-Ton der Plattformfahrt

**Messung vorher:** `lift2030_se_pc.c`/`LIFT6030.VBS` bedienen die zwei Lifte (Anker sub02/sub04); die Bruecke faehrt im Port-Programm `2030_pult_senken`, dort liefen keine Se_on: stumm (Riegel Zwilling unten zeigt den Ton allein).

**RE-Belege:** RE2s zentrale Bruecke = ROOM4080: msg 31 "Insert the valve handle and rotate it to move the central bridge up or down", ROOM4090 msg 37 "I can't cross unless the bridge is up"; `RE2_SOUND_LISTE.md`: ROOM400.SAP "Center Bridge Moving sounds", ROOM408.SAP "Center bridge room sounds".
sub13 @0x031EE senkt sie: @0x031F2 `36 02 0f 00 ...` Se_on Raumbank Satz 0x0F vor der Fahrt, @0x03314 `36 02 10 01 ...` Se_on Satz 0x10 am Ende. EDT @0xA3C0 + 4*0x0F `00 01 53 16` -> Programm 1, Taste 64 (Ton 5), VAG-Nummer 13 (12656 B, Flags 2 .. 3 = Dauerschleife), + 4*0x10 `00 01 64 16` -> Taste 65 (Ton 6), VAG-Nummer 14 (24448 B, einmal). Beide Stimme 6
(Byte 3 = 0x16), der Halt schneidet den Motor (Key-On auf dieselbe Stimme). Op 0x8C in sub13 ist Vibration (0x800593e4 -> FUN_800395b8 -> FUN_80038bbc), kein Ton.

**Umsetzung:** Bank `shared_assets/RE2/BRUECKE4080.EDT/.VH/.VB` (`tools/r41_nachtrag30/re2_toene_export.py --pruefen`, bytegleich mit der RDT). Port-Programm setzt Marker (5,0x14) vor dem For und (5,0x15) nach Member_set; der C-Takt spielt die Saetze (`ton_spielen`). Die Port-SE-Stimmen kennen keine Schleife: die PCM-Daten der Motor-Welle liegen dreimal aneinander (R41N_MOTOR_WIEDERHOLUNG, 121 Takte >= Fahrt 60).

**Gefunden durch die Messung:** Der erste Mitschnitt zeigte den Motor nach 40 Takten aus und den Halt dreimal: die Wiederholung lag auf der falschen Welle. `samples[]` zaehlt ab 0, die VH-Nummer ab 1 (Log `se=15 ... vag=12`). Behoben (`RE15_R41C_BRUECKE_MOTOR_VAG - 1`), der Riegel verlangt jetzt die letzten 10 Bilder hoerbar und +120 Takte bitgleich.

**Messung nachher (Zwilling mit Stummel-Bank, `integration_r41_nachtrag30_ton`):** Motor capTick 667, Halt 727 (60 Bilder). `CAPDIFF VOR 0 WAEHREND 1903.5 NACH 0`; letzte 10 Bilder vor dem Halt RMS 2093,1.

## 5. Alarm: Sirene aus ROOM6000 (Nachtrag 36), zusaetzlich zu SUB_25

**Messung vorher:** In der Minute lief nur RE2-SUB_25 (Spur C).

**RE-Belege:** RE1.5 ROOM6000: Tabelle UNK_80074828[0x62] = 0xFF65 -> MAIN25 (Sce_bgm_control @0x00D52 `54 00 01 00 46 40`). MAIN25.BGM ist bytegleich RE2 MAIN11.BGM (md5 52613db8df9fbcb41a9000663c7143b2), RE2 setzt ROOM6000 selbst darauf: ROOM6040 @0x02360 `57 00 00 06 11 ff 00 00`.
Sirene darin = Kanal 2, Programm 1, VAG 2 (4384 B), Note 60: 0,804 s an, 0,142 s aus, Periode 0,946 s. RE2 `SUB_34.BGM` traegt dieselbe Welle (VAG 2 sha1 9d010af0e8e9) und die Stimme (VAG 1 sha1 52678419f9ef) ohne die Musik: Kanal 1 Sirene, 2 Echo, 0 Stimme, 3 Brummen, Schleife 14,3 s. In keinem Leon-RDT per 57er-Satz referenziert, SEQ#2 leer.

**Umsetzung:** `shared_assets/RE2/SOUND/SUB_34.BGM`; audio_pc.c eigener Sequencer `s_ss_r41_sirene` + `re15_audio_r41_sirene(an)` (Lader des SUB-Platzes wiederverwendet, Bank Id 6, Schleife); `re15_ventilkette_r41_sirene()` = in 2030/2031, Zaehler laeuft, keine Flut, nicht tot; Flanke in `pc_bild`. Start = Zaehlerstart = nach der Ansage (Set(5,0x1D) steht hinter dem Warntext 46).
SUB_25 laeuft unveraendert daneben.

**Messung nachher:** Zwilling mit Stummel-SUB_34 ueber die ganze Minute: an capTick 674, bis 2653 kein Takt mit RMS(A-B) < 100, 1-s-Mittel 1960..2287 (Schleife 14,3 s mehrfach durchlaufen), vorher bitgleich, nach dem Verlassen des Raums (+120) bitgleich. Echte Folge (Riegel `integration_r41_ventilkette_alarm`, Leon + Elza): Warntext 46 -> Sirene an -> Flut, Sirene aus im Bild des
Flut-Beginns. Elza-Korrelation mit dem reinen SUB_25-Signal sinkt durch die Sirene von 0,571 auf 0,432 (verlangt >= 0,3), Kontrolle 0,025.

## 6. Lautstaerke N-E2 — nachher

**Beleg/Herkunft:** NUTZER-VORGABE Nachtrag 34 "selbst mit Lautstaerke bei 10 ist es immer noch zu laut. Da muss wahrscheinlich doppelt so viel gedreht werden und Standard sollte dann 50 sein." Die Endstufe bleibt die letzte Zeile der Mischung (Messung vorher oben: jeder Ton laeuft dort durch).

**Umsetzung:** `engine/src/lautstaerke_r41.c`: `q15 = Wert * Wert * 0x8000 / (100 * 100)`, also 100 -> 0x8000, 50 -> 0x2000, 10 -> 327; Standard 50 ohne Options-Datei. Messhaken `re15_lautstaerke_r41_voreinstellung`: ohne RE15_OPTIONS_FILE und mit RE15_AUDIO_CAP/_SYNC bleibt es bei 100, damit die Pegel-Riegel weiter die volle Mischung messen.

**Messung nachher (`integration_r41_nachtrag30_laut`):** Lauf 20A0 -> Tuersequenz DOOR26 + Schliesston -> 2090, Sprung ins Becken (Landung (6363,0,-14032)). Alle 2 499 000 Abtastwerte exakt `v50 == (v100 * 0x2000) >> 15` und `v10 == (v100 * 327) >> 15`; 477 850 Werte ueber +-2000, Spitze 16681. `integration_r41_kleinkram_a_lautstaerke`: RMS(50)/RMS(100) = 0,25 auch mit fehlender Options-Datei.

## 7. Wasser die Leiter herunter (Nachtrag 30e)

**Messung vorher:** ROOM2010/2011 ist das Buero (Schreibmaschine, Computer, BG00..BG05): nur die Tuer Slot 0 @0x00746 nach 2000, keine Band-Stufe, kein Kletter-Platz, kein Sce_espr_on, kein Wasser-Objekt. Der Wortlaut "die Leiter runter ... ueber den Weg bis zum Wasser Pool in der Mitte" trifft Wort fuer Wort ROOM2000/2001, die Nachbartuer: Leiter nach 2050 (Slot 4 @0x0169E, Holme (-26748,-1800,9034)/(-26715,-1800,9945)),
Weg = West-Steg Band 1, y -1800, x -27000..-25200, Pool in der Mitte = Kanal x -25200..-21650, Wasser-Objekte obj 0..2 @0x016E6/@0x01708/@0x0172A Typ 3, y -900. Gebaut wurde dort; Rueckfrage unten.

**RE-Belege Form:** Leiterstrom = derselbe Effekt wie die drei Strahlen am Gitter derselben Wand, ROOM2000 main00 @0x0174C/@0x0175C/@0x0176C `3a 00 0b 00 00 00 00 19 x y z 00 04` = Sce_espr_on Effekt 0x0B, Sub 0, Skala 0x1900, Parameter 0x400 (FUN_80041864 -> FUN_80019700), absolut, dauerhaft; die Teilchen fallen in Welt-y. Spritzen = Wasser-Effekt 6:
`lui a0,0x605` / `ori a0,a0,0xd00` @0x800456b8/bc = Effekt 6, Sub 5, Skala 0x0D00 (Wasserboden FUN_80045630). Nasse Spur = Wasserflaeche des Moduls Ventilkette (ROOM2080 Prop 0).

**Umsetzung:** `engine/src/wasserlauf_r41.c`, `include/re15_wasserlauf_r41.h`, `gen/ventilkette_r41_wasser.inc` (`re15_r41c_wasserlauf_md1`, Rechteck x -26750..-25200, z 9050..9950). 6 Dauer-Strahlen Skala 0x0C00, Spritzer Effekt 6 Sub 5/Sub 3 auf dem Weg und im Kanal, Takt 11/7 (PORT-WAHLEN: Skala, Takt, Punkte; gemessen Laenge 3300 je Strahl, deshalb Anker y -6900/-6000/-5100).
Ende = dieselbe Bedingung wie die Sperre der Leiter: Bit (9,100) oder (9,103) gesetzt. `re15_wasserlauf_r41_soll` haengt nur an Raum und Bits (der Prop-Lader fragt unabhaengig vom Install).

**Messung nachher (Leon + Elza, `integration_r41_nachtrag30_wasser_*`, `_laden_*`):** Cut 10 gegen den Lauf mit Bit 100: Leiter 20,7, Weg 8,0, Gegenprobe rechte Bildhaelfte 0,0 Graustufen; auch nach der echten Tuer 2010 -> 2000 und nach CONTINUE (Leiter 17,3, Weg 8,5). Leiter bleibt gesperrt (Text 40). Bilder: `N_belege/wasser_2000_cut10.png`, `wasser_2000_cut9.png`.

