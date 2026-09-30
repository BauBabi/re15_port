# Spur D — Gegenpruefung des Bauplans (Skeptiker)

Geprueft: `analysis/befunde_runde34_nacht/D_adaruf.md` (Stand `5207481a`, Bauplan Ada-Ruf an der Tuer ROOM1050 -> ROOM10A0).
Pruefer: Skeptiker-Agent, hat das Dossier nicht geschrieben. **Kein Port-Code geaendert.** Eigene Messungen nur mit
Werkzeugen ausserhalb des Baums (Scratchpad) bzw. Lese-Laeufen der vorhandenen Werkzeuge; neue Belege unter
`D_belege/gegenpruefung_*`.

## Urteil: haltbar_mit_auflagen

Der Kern traegt: jede tragende Adresse/jedes zitierte Byte, das ich nachgeprueft habe, stimmt (Tabelle (a)); die wichtigste
Korrektur des Dossiers an der Aufgabenstellung — Freigabe ueber **(3,0xBB)** statt (3,0x6E) — ist richtig und verhindert
einen echten Softlock; der Rueckschritt ist nicht nur an 7, sondern an **5310 begehbaren Druckstellen** robust (eigenes
Raster, (c)). Widerlegt bzw. nicht tragfaehig sind Belege und Nebenbegruendungen, nicht der Plan: das Tuergraph-Werkzeug
ignoriert das Stage-Byte (falsche Kanten, Schluss zufaellig trotzdem richtig), der Gestenkatalog laesst Kandidaten aus
(von mir gerendert — keiner passt besser), die Begruendung fuer zwei Leon-Nachrichten zitiert ein Vorbild, das eher das
Gegenteil zeigt, und der Abnahmeplan misst Laden/Speichern, Doppeldruck, Sprachdateien und die Sichtbarkeit der Gesten in
Cut 4 nicht am Artefakt. Dazu ein unnoetiger Konflikt-Edit (`local_build.sh`). Alles per Auflage behebbar, kein Umbau.

## (a) Tragende Konstanten/Adressen selbst nachgeprueft

Werkzeuge: `re15_port/tools/scd_dump_room.py`, `rdt_msgdump.py`, `.claude/skills/re15-psx-disasm/scripts/re15_disasm.py`
(PSX.EXE, Auslieferungsstand `info/Re1.5/PSX.EXE`), eigene Walker-Zensus ueber `scd_walk_lib` (240 RDTs, 40674 Opcodes).

| Behauptung im Dossier | selbst gelesen | Ergebnis |
|---|---|---|
| Tuer Slot 4 ROOM1050 main00 @0x00B5A | `3b 04 02 31 00 00 3c 41 94 c6 e8 03 d0 07 58 66 c0 c7 e0 60 00 08 00 0a 00 08 …` Byte 22 = 0x00 (Stage 1), Byte 23 = 0x0A | ✓ ROOM10A0 |
| Slot 5 @0x00B7A -> 1090, Slot 1..3 -> 1000 | Byte 22/23 = 00/09 bzw. 00/00 (Cut 6/3/0) | ✓ |
| (3,0xBB) nur gesetzt ROOM1090 sub03 @0x024D2, nie geloescht | Walker-Zensus: `Set 3 187 1` nur @0x024D2, `Ck 3 187 1` nur ROOM1090 sub00 @0x0237A; roh-Byte-Treffer `22 03 bb` in 1030/1040/1050/10D0/1130/1240/6040… liegen alle AUSSERHALB der SCD-Sektionen (Daten) | ✓ |
| (3,0x6E) gesetzt @0x024D6, geloescht ROOM1050 sub03 @0x0D88 | `22 03 6e 01` @0x024D6; `22 03 6e 00` @0x0D88 = ERSTES Opcode von sub03 (Ada laeuft davon), gestartet aus sub00 @0x0C8A/@0x0CA2 | ✓ — die Aufgabenpraemisse "(3,0x6E) = Freigabe" ist damit WIDERLEGT, die Korrektur des Dossiers noetig |
| (9,65)/(9,66) frei | kein Ck/Set (9,65/66) in 240 RDTs; Item-Taken-Bits aller 164 Item_aot_set: 2..253, keins 65/66; kein Treffer in `r34n_*`/`r34g_*`-Baeumen | ✓ |
| Spieler-Plc_dest-Tabelle @0x80073e30 | `table`: [6]=0x800517f0 [8]=0x800311f0 [9]=0x80031360 | ✓ |
| Modus 8 @0x800311f0 | `ori 0x46`/`sh -13600` @0x80031210/18 (+0x8c=70), Clip 0 @0x8003122c, `addiu a2,zero,-48` @0x80031254, `jal 0x800245d8` + `ori a0,0x800` @0x80031258/5c, anim_set aus 0x800acad8/0x800acbc0 @0x800312a4-b0, SquareRoot0 + `slti 100` @0x800312f4/fc | ✓ |
| Modus 9 @0x80031360 | Clip 5 `ori v0,0x5`/`sb` @0x8003139c/a4, `jal arc_test` + `ori a2,0x60` @0x800313d0/d4, Ankunft `sb 6,+0x5` + `jal 0x8004ef90` @0x800313e8-f8, Drehrate `ori a2,0x60` @0x80031440, KEIN pos_advance | ✓ |
| Plc_motion @0x80041b90 (Dispatch 0x800744a8[0x3F]=0x800745a4) | `lhu a1,2(v0)` @0x80041b9c, `sb a1,148` @0x80041ba8, `srl`/`sh a1,452` @0x80041bac/c8, `sb 4,4(v0)` @0x80041bb0, `sb a2,5` @0x80041bc4, PC+=4 @0x80041bd4 | ✓ |
| Plc_flg @0x80041fb8 ([0x43]), Plc_ret [0x42]=0x80041f88 | Unterbefehl 0 -> `or v0,v0,a2` @0x80041ffc auf +0x1c4 | ✓ |
| Ereignis-Handler sce 3 | Typtabelle @0x8007469c [3]=0x800430f0: `lhu a0,0` @0x800430fc, `lbu a1,3` @0x80043100, `jal 0x8003ee3c` @0x80043104 | ✓ |
| Bank-Tabelle @0x80074664 | [1]=0x800aca3c [2]=0x800aca40 [3]=0x800b0ff8 [9]=0x800b1078 | ✓ |
| Letterbox FUN_80021a0c | `andi 0x10` @0x80021a24, +16 @0x80021a54, +240 @0x80021a7c | ✓ |
| Vorbild-Bytes | ROOM1130 @0x0A1C `46 03 01 31 01 00 ff ff 00 00`; ROOM11B0 @0x1478 `22 03 83 01`; ROOM1050 @0x0C22 `2c 07 03 31 … ff 00 18 02 00 00`; ROOM1170 @0x15F0 `3f 00 11 00` / @0x15F4 `09 0a 64 00`; ROOM1090 sub02/sub03/sub04 alle Versaetze laut Dump | ✓ |
| k_ruf 142 B | Laengen gegen `s_opcode_sizes` (Set 4, Work_set 3+Nop, Plc_dest 8, Sleep 4, Message_on 4, Do 4, Evt_next 1+Nop, Edwhile 2, Ck 4, Plc_motion 4, Plc_flg 4, Plc_ret 1+Nop, Aot_reset 10, Evt_end 2) | ✓ |
| Nachrichten 22..25 | Kopf/Ende identisch zu ROOM1090 msg 0/1 (`04 00 05 02 33 4b 49 3d 4a 16 05 00 00` … `04 01 01 63`); Glyphen Buchstabe fuer Buchstabe dekodiert (S=0x2f, ","=0x18, "!"=0x1a, "?"=0x1b, "."=0x57, Umbruch 0x08 wie tuer1120_1130.c) | ✓ |
| Clip 17 nie rueckwaerts, Clip 19 11x von 26 | `plc_motion_zensus.tsv`: Hash ce75d881cd9d n=10 rev=0; fde929aa6e6b n=26 rev=11 | ✓ |
| **Tuergraph `D_belege/tuergraph_stage1_leon.txt`** | `tuer_graph.py` Z. 8: "Stage aus dem Raumnamen" — Byte 22 (Stage) wird IGNORIERT. ROOM11A0 @0x01006 Slot 4 Byte 22 = **0x01** -> **ROOM20A0**, nicht ROOM10A0; 11A0 Slot 0 -> ROOM2070, Slot 1 -> ROOM3000; ROOM1260 Slot 1/4 -> ROOM2000, nicht ROOM1000 | **WIDERLEGT (Beleg)**; Schluss bleibt: korrigierte Eingaenge von ROOM10A0 = 1050 S4 @0x00B5A, 1180 S1 @0x00A34, 11E0 S2 @0x01552, 1230 S1 @0x00B34 — alle ausser 1050 hinter 10A0; ROOM1170 S1 -> 10B0 ist sce 0 und wird nie scharf (ROOM1170 hat nur Aot_on 2 @0x01614 und Aot_on 3 @0x016D0) |

## (b) Passt der Plan zum Nutzerwortlaut und zu seinen Bildern?

* **L1 erster Druck** — traegt ("Drückt man nach dieser Cutscene **erneut** die Tür" setzt einen ersten Druck voraus).
* **L2 "Woman:"** — vom Nutzer bestaetigt (AUFTRAG.md Nachtrag). Sprecherfarbe 02 wie ROOM1090 msg 0 @0x275C.
* **L2b zwei Leon-Nachrichten — Begruendung nicht tragfaehig, Entscheidung als PORT-WAHL kennzeichnen.** Das zitierte
  Vorbild zeigt eher das Gegenteil: ROOM1090 msg 5 @0x2861 ("Leon: We can talk later... It's not safe / here.") traegt ZWEI
  Saetze in EINEM Kasten, und waehrend dieser EINEN Nachricht laufen ZWEI Gesten nacheinander (sub03 @0x02640 Message_on 5,
  @0x02648/@0x02650 Clip 18 vor/rueck, @0x0265C/@0x02664 Clip 19 vor/rueck). Weitere Einkasten-Vorbilder: ROOM6030 msg 0
  "Leon: Hang on, Marvin. / We're getting out of here." (Clip 0 vor/rueck, 2, 7 in einer Nachricht), ROOM1150 msg 4. Der
  Nutzer schrieb die Leon-Zeile als EINE Zeile; zwei Zeilen passen in einen Kasten (218 px + ~130 px, je <= 271 px).
  Zwei Nachrichten sind trotzdem vertretbar (Sprach-Takt ueber `voice_wait`), aber dann als Port-Wahl mit diesem Gegenbeleg.
  Folge zum Messen: Dialogzeilen erscheinen per `04 00` sofort ganz (msg_common.c:505-524), msg 23 steht ohne Sprachdatei
  nur **51 Bilder** (Sleep 25+26), die Originale geben einer Leon-Zeile ~100 Bilder (sub02 @0x0248A..@0x024B6 = 101).
* **L3 Rueckschritt** — traegt (Modus 8, dieselbe Weglaenge wie ROOM1090 @0x0247C; echte exe 1090 Bild 301..311 im Beleg
  `r1090_sub02_rueckschritt_echt.png` angesehen).
* **L4/L5 Gesten — Lesart traegt, Katalog war unvollstaendig.** Semantik der Original-Verwendung (Zensus-Texte): Clip 19 (fast
  immer vor+rueck) begleitet erklaerende Saetze ("We can talk later... It's not safe here.", "Ada, what is this place...?",
  "What should we do?"), Clip 17 (nie rueckwaerts) die nachdrucklichen ("Now what am I gonna do?", "Hurry up! They're
  coming!", "Go on ahead! I'll take care of this!", "Wait, what!?") — passt zu "Another civilian survivor." bzw. "I have to
  help her!". ABER der Katalog `gesten_katalog_vorn.png` zeigt nur Clip 15..23, obwohl das ROOM1050-RBJ 15..**25** hat, und
  die raumeigenen Spieler-Clips ausserhalb der Bibliothek fehlen ganz (ROOM1150 rec0 Clip 9..14 in sub03/sub08, ROOM1170 rec0
  Clip 25 @0x015E4, ROOM50B0 Clip 13/14, ROOM2020 Clip 0). Selbst gerendert (`gesten_streifen.py`, Beleg
  `D_belege/gegenpruefung_weitere_leon_clips.png`): Clip 24 (=ROOM3070 Clip 24, dort vor+rueck @0x0350C/@0x03514) =
  Kopf/Oberkoerper-Wendung ohne Arm, Clip 25 = Ganzkoerper-Drehung 90 Grad (nie gerufen), ROOM1170 Clip 25 = Hand ans Gesicht,
  ROOM1150 Clip 10/11/12 = KNIEND. Keiner ist eine stehende Arm-Geste "180 Grad nach rechts" — die Wahl 19/17 bleibt die beste.
  Nebenbefund: `plc_motion_zensus.py` nimmt in ROOM6030 den ersten Record mit Bit 0 (rec0, 1 Clip) statt rec2 (Marker 3) ->
  25 Aufrufe ohne Hash; Clip-Laengen von rec2 (20/30/30/20/30/25/35/24) = Bibliothek, aendert nichts.
* **Sichtbarkeit in Cut 4** — Cut 4 sieht Leon schraeg von hinten links (Kamera (14999,-3549,-8140) -> (15618,-2173,-12830));
  in `geste19_17_kamera4.png` Zeile "kamera" ist Clip 19 stark verkuerzt, Clip 17 gut sichtbar. Die Nutzerforderung haengt an
  erkennbaren Gesten — das muss am echten Framedump abgenommen werden (Auflage 7), nicht am Synthetikbild.
* **L8 Elza** — ROOM1091 sub00 @0x0222E / sub01 @0x02230 = `01 00`, main00 ohne Ada/Feuer (nur Zombies Typ 0x10/0x11) ✓.
* **L9 Kamera** — RVD ROOM1050 @0x1B0 selbst dekodiert: Umschaltzonen an der Tuer sind reine z-Baender ueber x 12800..18300
  (4->5 z -16700..-15700 @0x2B4, 5->4 z -15700..-14700 @0x2DC, 4->3 z -11500..-10500 @0x2A0); der Rueckschritt aendert z nicht
  -> kein Schnitt ✓.

## (c) Softlocks, Regressionen, Laden/Speichern, Raumvarianten

* **Softlock-Kette (mit A und tuer1120, im Dossier FEHLEND):** 1170 -S4 @0x012F8 (unbedingt)-> 1130 -S2-> 1150 (Irons-Szene
  setzt (3,94) sub08 @0x01110; Sicherung im Hebetisch, sub04 vom Port ueber Slot 1 @0x0D7E ohne Vorbedingung scharf) -> 1130
  -S1 (tuer1120: erst nach (3,94))-> 1120 -> 1060/1080 -> 1040 -> 1030 -S0-> 1050 Nord -> Rolltor (Spur A: Sicherung) ->
  1050 Sued -S3 @0x00B3A-> 1000 Umkleide (Feuerloescher (3,133)) -> 1050 -S5 @0x00B7A-> 1090 (sub06 -> Selbsttuer -> sub03
  setzt (3,187)) -> 10A0. Nichts davon liegt hinter 10A0 -> **kein Softlock** (bestaetigt). Das Dossier wertet keine Bedingung
  aus und nennt weder Spur As Sicherungspflicht noch die tuer1120-Sperre auf dem Weg — nachtragen (Auflage 3).
* **Rueckschritt-Haenger: widerlegt nicht, sondern viel staerker bestaetigt.** Eigenes Raster (`D_belege/gegenpruefung_raster.txt`,
  Quelle `gegenpruefung_raster_probe.c.txt` = D-Sonde + Raster-main, gegen `libre15_engine.a` dieses Baums gelinkt): alle
  Stellen mit Punkt 620 voraus im Tuerrechteck, x/z-Schritt 50, Gierung je 128, begrenzt auf den BEGEHBAREN Flur (gemessene
  Ostgrenze 16732 fuer z > -15000, 16482 fuer z <= -15000): **5310 Druckstellen, 5310x Ereignis 13, 0 Haenger, Rueckschritt
  <= 10 Bilder, Weg 632..703, Szene <= 314 Bilder.** Falle fuer den Riegel: ohne Begehbarkeitsfilter liefern Stellen IN der
  Wand (x 17300, z -15300..-15100; Pad UP bewegt dort 40 Bilder nichts) 77-80 Bilder lange Rueckwaerts-Schleifen mit
  328-Einheiten-Sprung — ein Mess-Artefakt, im Spiel unerreichbar, aber ein Riegel mit "gesetzter" Position ohne Filter
  wuerde falsch rot.
* **Doppel-Ausloesung:** waehrend der Szene blockt `in_cinematic` (aot_common.c:1061/:1153, `player_mode == 2 ||
  letterbox_countdown`) alle Nicht-Kamera-AOTs, bis die Balken weg sind; dann ist Slot 4 schon Text-Platz (Aot_reset +0x82).
  Die Weiche selbst prueft (9,65) aber nicht — eine zweite Flanke im Druckbild (vor dem ersten VM-Takt) startet einen zweiten
  Faden auf DENSELBEN `s_prog`. Billige Absicherung: Weiche nur bei (9,65)=0 (Auflage 5).
* **Laden/Speichern:** `g_game.flags` wird komplett gespeichert (re15_savedata.c:210 `memcpy(out->flags …)`, :274 zurueck) —
  (9,65) und (3,187) ueberleben. Im Abnahmeplan fehlt der Kartenlauf (Auflage 8).
* **Alt-Spielstaende:** wer (heutiger Port) schon ohne Rettung hinter 10A0 ist, kommt ueber 10A0 S0 nach 1050 zurueck; der
  naechste Druck an Slot 4 zeigt dann Szene+Sperre, die Rettung bleibt erreichbar -> kein Softlock, aber im Dossier nennen.
* **Regressionen:** kein registrierter Test geht mit Default-Flags durch ROOM1050 Slot 4 (probe_r33_tueren `zuordnung` liest
  RDT-Bytes, `durchgang_pruefen` nur 1000->1050/3050/4080; probe_r31_tueren 1050 -> 1090 = Slot 5; test_r18_ada_eintritt
  prueft nur Adas Clip). Tuersequenz P07G haengt an `aot_fire_door` — nach der Rettung unveraendert (Sonde Fall C).
  Kartenmarker-Zug (re15_inv_screen.c:656 filtert `type == DOOR`) gilt nur fuer synth-Zonen, ROOM1050 ist keine.
* **Sprachausgabe:** Text-Platz ohne Stimme bestaetigt (scd_vm.c:1511-1513 "No voiceover"); `synchro/STAGE1/room1050/` hat nur
  `main08.wav` -> keine Fremdaufnahme auf 22..25. Laeuft main22.wav > 100 Bilder, beginnt der Rueckschritt noch waehrend
  "Woman" spricht (kein Opcode wartet auf Stimme) — Nutzerwunsch ist "nach Adas Dialog" (Auflage 9).

## (d) Ressourcen-Kollisionen (VERTRAG.md, andere Spuren)

* Bank 9 Bit 65 (66 Reserve), Slot 4 umgewidmet, Nachrichten 22..25 — alles im Vertrag. Ereignis 13 ist kein Vertragsgut,
  kollidiert aber nicht: ROOM1050 hat 5 Subs (Tabelle @0xC10), Spur A faengt Ereignis 2 ab, Spur E nutzt keine Ereignisse
  (Item-Zone Slot 15, E_dokumente.md Z. 333/544).
* Haken: Spur As tatsaechlicher Haken (Zweig `r34n/rolltor`, scd_vm.c) ist `const uint8_t *pc = re15_rolltor_ereignis(...);
  if (!pc) pc = s_current_rdt->sub_scd[event_id];` — Ds Zeile `if (!pc) pc = re15_adaruf_ereignis(...)` passt genau dazwischen ✓.
* E_dokumente.md Punkt 11 (D-Slots 13/14 koennten das Tagebuch-Rechteck abfangen) entfaellt: D benutzt Slot 4
  (x 16700..17700, z -14700..-12700), E x 15250..16250, z -7250..-6250.
* **`local_build.sh` RE15_MIN_TESTS +1 streichen:** die Zahl ist eine UNTERE Schranke (`-ge ${RE15_MIN_TESTS:-428}`,
  local_build.sh:345) — ein zusaetzlicher Test braucht sie nicht; B und C heben dieselbe Zeile (A_rolltor.md:365) -> nur
  Mischkonflikte. Die Integration setzt die Zahl einmal.

## (e) Misst der Abnahmeplan das Nutzer-Symptom am Artefakt?

Gut: echte exe, echter Tuerwechsel 1000 -> 1050 (Installation laeuft ueber den echten Raumaufbau), RE15_FRAMEDUMP (komponiert
VOR dem Present inkl. Balken, main.c:10750ff), Protokolle, echter Rettungslauf 1090 -> 1050. Alle genutzten Schalter existieren
(`RE15_PRESS` main.c:6906, `RE15_INPUT_SCRIPT_BASIS` input_pc.c:110, `RE15_DEBUG_SUB` main.c:7068, `RE15_PSELECT_*`).
Es fehlen: Kartenlauf Speichern/CONTINUE nach der Szene; ein ECHTER Raumwechsel statt `RE15_SET_FLAG=9:65`; Quadrat
waehrend der Szene; Sichtbarkeit der Gesten im echten Cut-4-Bild; ein Lauf mit Platzhalter-WAVs (laenger als die Sleeps),
der zeigt, dass `voice_wait` die Gesten an den Saetzen haelt und wie weit main22 in den Rueckschritt reicht; ein gemeinsamer
A+D-Lauf (Sicherung -> Tor -> Szene) nach der Zusammenfuehrung.

## (f) Uebersehenes besseres Vorbild?

Kein besseres. ROOM1090 sub01/sub02 (der Ruf der "Woman") ist das richtige Vorbild und wird benutzt. Ergaenzend zitierbar
(stuetzt die Folgetext-Form): RE1.5 hat im selben Handlungsstrang schon einen Hinweistext, solange die Bedingung fehlt —
ROOM1090 sub00 @0x0230A `Ck(3,133)==0` -> @0x0230E Text-Platz msg 7 "I must hurry up and get something to / put out this fire
to save that woman!". RE2 sperrt Story-Tueren stumm (tuer1120-Beleg ROOM2190/ROOM6010) — deckungsgleich mit msg 25 ohne Ton.

## Widerlegte Behauptungen

1. Aufgabenpraemisse "Ada-Rettung = Flag (3,0x6E)" — ROOM1050 sub03 @0x0D88 `22 03 6e 00` loescht es beim ersten
   Wiederbetreten; tragfaehig ist (3,0xBB) (@0x024D2, nie geloescht). (Vom Dossier selbst richtig korrigiert.)
2. `tuergraph_stage1_leon.txt`: Kanten ROOM11A0 -> ROOM1070/1000/10A0 und ROOM1260 -> ROOM1000 sind falsch (Stage-Byte
   ignoriert; ROOM11A0 @0x01006 Byte 22 = 0x01 -> ROOM20A0). Der Schluss "10A0 nur ueber 1050 Slot 4" bleibt mit dem
   korrigierten Graphen richtig.
3. L2b "RE1.5 setzt zwei ganze Saetze desselben Sprechers ebenfalls als zwei Nachrichten" als Beleg fuer die Aufteilung —
   das Vorbild ROOM1090 msg 5/6 hat je zwei Saetze PRO Kasten und zwei Gesten in EINER Nachricht (sub03 @0x02640..@0x0266C).
4. §3.5 "alle neun Bibliotheks-Gesten" — die Bibliothek im ROOM1050-RBJ hat elf (Clip 24/25 fehlen im Katalog); dazu die
   raumeigenen Spieler-Clips (1150/1170/50B0/2020). Nachgeholt, keiner passt besser.

## AUFLAGEN

1. **Tuergraph korrigieren:** `tuer_graph.py` liest das Stage-Byte (Byte 22, bei Viereck-Saetzen +16) statt die Stage aus dem
   Quellraum zu nehmen; `tuergraph_stage1_leon.txt` neu erzeugen und §7 mit den korrigierten Eingaengen von ROOM10A0
   (1050 S4, 1180 S1, 11E0 S2, 1230 S1) und dem toten ROOM1170 S1 -> 10B0 (sce 0, nie Aot_on) belegen.
2. **Gestenkatalog vervollstaendigen:** Clip 24/25 des ROOM1050-RBJ und die raumeigenen Spieler-Clips (ROOM1150 9..14,
   ROOM1170 25, ROOM50B0 13/14, ROOM2020 0) als GEPRUEFT-UND-VERWORFEN mit Bild in §3.5 aufnehmen (Beleg
   `gegenpruefung_weitere_leon_clips.png`); ROOM6030 im Zensus ueber den richtigen Spieler-Record (rec2, Marker 3) zaehlen.
3. **Softlock-Abschnitt §7 erweitern** um die ganze Kette MIT Bedingungen: tuer1120 (3,94) auf dem Weg 1130 -> 1120, Spur As
   Sicherung (1150, vor dem Tor), Feuerloescher (3,133), Rettung (3,187) — je mit Offset wie in (c); Alt-Spielstaende hinter
   10A0 ohne Rettung nennen.
4. **L2b neu entscheiden und kennzeichnen:** entweder EINE Nachricht "Leon: Another civilian survivor. / I have to help her!"
   (RE1.5-Form, Nutzerwortlaut; Gesten per Sleep wie ROOM1090 sub03 @0x02640..@0x0266C; eine Sprachdatei main23.wav) oder zwei
   Nachrichten ausdruecklich als PORT-WAHL (Grund: Sprach-Takt) mit dem Gegenbeleg aus (b). Bei zwei Nachrichten die
   Lesbarkeit von msg 23 (51 Bilder ohne Stimme) am Framedump abnehmen; reicht sie nicht, Sleep-Kette verlaengern und die
   Zahl als Port-Wahl begruenden.
5. **Weiche absichern:** `re15_adaruf_ereignis` liefert nur bei `room == 0x1050 && event_id == 13 && (9,65) == 0 &&
   (3,0xBB) == 0` das Programm, sonst NULL (Rueckfall `sub_scd[13]` = NULL -> Ereignis verworfen, harmlos). Riegel-Fall:
   zweiter Quadrat-Druck im Druckbild und waehrend der Szene -> genau EIN Faden, (9,65) einmal gesetzt.
6. **Riegel mit Raster statt 7 Punkten:** die Rueckschritt-Pruefung ueber ein Raster begehbarer Druckstellen (z.B. 100 x 100,
   Gierung je 256 = 728 Stellen, 8,6 s gemessen; oder 200/512 = 120 Stellen, 2,1 s) mit Begehbarkeitsfilter (Ostgrenze
   16732/16482 bzw. Lauf-Gegenprobe); Soll: Ereignis 13 ueberall, Faden endet, Rueckschritt <= 12 Bilder, Weg 600..760,
   z-Aenderung < 100 (keine Kamerazone). Die Stellen in der Wand NICHT einbeziehen (Artefakt aus (c)).
7. **Gesten am echten Bild abnehmen:** Framedumps (RE15_WINDOW_SCALE=3) von Clip 19 hin/zurueck und Clip 17 in Cut 4 ansehen
   und ins Dossier; ist Clip 19 aus dieser Kamera nicht als "Arm dreht auf und geht nach rechts" erkennbar, im Dossier
   ausdruecklich festhalten und die Kamerawahl L9 neu begruenden (die Nutzerforderung sind SICHTBARE Gesten).
8. **Abnahmeplan ergaenzen:** (a) Kartenlauf: Szene sehen -> in 1120/1150 speichern -> CONTINUE -> 1050 -> Druck = msg 25,
   keine Szene; Speichern nach der Rettung -> CONTINUE -> Tuer + Tuersequenz P07G. (b) "auch nach Raumwechsel" als echter
   Wechsel (1050 -> 1000 -> 1050) statt `RE15_SET_FLAG=9:65`. (c) Quadrat waehrend der Szene (`RE15_PRESS` mehrfach) ->
   keine zweite Szene. (d) Nach der Zusammenfuehrung mit A: ein Lauf Sicherung -> Tor -> Szene an der 10A0-Tuer.
9. **Stimme gegen Zeitlinie messen:** Lauf mit voruebergehenden Platzhalter-WAVs (z.B. vorhandene Aufnahmen umkopiert, 4-5 s,
   NICHT committen, danach loeschen): belegen, dass `voice_wait` msg 23/24 bis zum Ende der Vorzeile haelt und die Gesten am
   Satz bleiben, und messen, wie weit eine > 3,3 s lange main22 in den Rueckschritt reicht. Den Laengenhinweis (<= 100 Bilder)
   in die Aufnahmeliste fuer den Nutzer uebernehmen oder den Rueckschritt auf das Stimmende warten lassen (dann als Port-Wahl
   mit Beleg der Wartebedingung).
10. **`local_build.sh` nicht anfassen** (RE15_MIN_TESTS ist eine Untergrenze, local_build.sh:345; B/C/A heben dieselbe Zeile);
    die Integration setzt die Zahl einmal.
11. **Folgetext-Vorbild ergaenzen:** in 5.4 zur Form des Hinweistexts ROOM1090 @0x0230A/@0x0230E (msg 7, derselbe
    Handlungsstrang) zitieren; bleibt ohne Ton (RE2-Beleg tuer1120).
