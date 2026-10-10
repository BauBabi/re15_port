# Runde 36 — Spur L "elza" (Dossier, fortlaufend)

Baum: `.claude/worktrees/r36_elza`, Zweig `r36/elza`, Basis master 78a4a748.
Auftrag (AUFTRAG.md, woertlich, Spur-L-Punkte):

1. "Bei Elzas Szenario soll die Scheibe in Room 1120 die bei Leons Szenario zum schluss zerspringt in Stage1,
   bereits immer kaputt sein, und eine Kraehe mehr im Raum."
2. "Bei Elzas Szenario soll Irons immer bereits von anfang an tot da liegen. Wenn sie ihn anklickt soll da stehen
   "He has already passed..."."
3. "In ROOM 1211 in Elzas Cutscene wenn sie Kendo befreit fehlt noch der Unlock sound bei der Tuer wenn sie sie
   aufschliesst."
4. "In Elzas Cutscene in Room 1021 wo Kendo Roy erschiesst fehlen die Schuss Sounds."
5. "In Elzas Cutscene in Room 1021 wo Kendo Roy erschiesst fehlt noch ein Sterbe/Leid/Hinfall sound von Roy,
   sobald er erschossen wurde."
6. "Ich habe fuer Elzas Szenario Synchron Dateien hinzugefuegt. In Room 1101 - waehrend Roy bewusstlos wird -
   moechte ich das du das Voiceover main 16.5 abspielst. Auch wenn es aktuell keinen Text dafuer gibt. Erstelle
   gerne ein Untertitel dafuer, wie bei den anderen auch."

Zuteilung (VERTRAG.md): Bank-9-Bit 86 (Reserve), Nachrichten-IDs ROOM1151 20..23, ROOM1101 20..21,
ROOM1121/1021/1211 nur falls noetig ab RDT-Zahl (+3), Ereignis 26. Eigentum: fenster_1120.c (Elza-Zweig),
Irons-Texte 1151.

## 0. Stand / Fortschritt

- [x] Grundlagen (Variante, Werkzeuge, Baseline-Bau 78a4a748 gruen gebaut)
- [x] P1 Fenster 1121 kaputt + Kraehe (fenster_1120.c Elza-Zweig; unit + integration gruen)
- [x] P2 Irons 1151 tot + Text + Kopf fest (elza_r36.c; unit + integration gruen)
- [x] P3 1211 Unlock-Ton (RE2 ROOM1140 Satz 0x25, Anker @0x020F8 +20; unit + integration gruen)
- [x] P4 1021 Schuss-Toene (RE2 ROOM40A0 Satz 0x08, Anker @0x020CE/@0x020D6 +1; unit + integration gruen)
- [x] P5 1021 Roy Sterbe-Ton (RE1.5 Raum-SE 5 @0x80106da8, Anker @0x020DE +1; unit + integration gruen)
- [x] P6 main16.5 + Untertitel (ROOM1011 @0x00D54, Nachricht 24 = msg 10 "Roy: groans"; unit + integration gruen)

## 1. Grundlagen

### 1.1 Wie der Raumlader die Elza-Variante waehlt (Beleg)
* TITLE.BIN Auswahlschirm spiegelt die Wahl in DAT_800ACA3C Bit 31: Elza `lui v1,0x8000` @0x801024a8 +
  `or` @0x801024e4, Store `sw v0,-13764(at)` @0x801024ec (Port: include/re15_gameflow.h re15_char_variant()).
* Raumlader FUN_800396fc: Basisindex aus der Stage-Tabelle (@0x800397cc/@0x800397e0), Elza-Bit
  `srl a0,a0,31` @0x800397e4, `addu a0,a0,v0` @0x800397ec -> CD-Datei = Leon-RDT + 1. Im Port ist die Variante die
  niedrigste Hex-Ziffer der Raum-Id (ROOM1121/1151/1211/1021/1011 werden NUR in einem Elza-Spiel geladen).
* Gemessen (Lauf m1, Elza per RE15_PSELECT_AUTO_SWITCH): debug.log `[pl] Spieler-Familie PL04 (character=4,
  Elza-Bit=1)`, `[goto] queued room change -> ROOM1121`.
* ROOM1120.RDT == ROOM1121.RDT (cmp bytegleich); Hintergrund ROOM112.BSS ist variantenlos (eine Datei).

### 1.2 Werkzeuge dieser Spur
* `re15_port/tools/r36_l/re2_raum_se_zensus.py <satz-hex>...` — Zensus eines RE2-Raumbank-Satzes (Se_on Bank 2)
  ueber alle 495 RE2-Leon-RDTs (Wellen-Familien nach sha1).
* Messlaeufe: Kopie `re15_pc_r36l.exe` neben der exe, Arbeitsordner im Scratchpad (nie im Baum).

## 2. Messung vorher (Stand 78a4a748)

* m1 ROOM1121 (Elza, RE15_FORCE_CUT=1, Bild 90): Hintergrund Cut 1 mit UNBESCHAEDIGTEM Fenster; 3 Kraehen
  (`[spawn-diag] Sce_em_set type=0x21` Slots 0..2 = main00 @0x00D0C/@0x00D20/@0x00D34, Kill-Flags 0xA7/0xA8/0xA9).
  fenster_1120.c ist Leon-only (RE15_FENSTER_RAUM 0x1120, `if (room_id != RE15_FENSTER_RAUM) return;`).
* m2 ROOM1151 (Elza): Irons Typ 0x45 Slot 0 @0x00E6A (-21150,-720,-26131), sub02 Plc_motion(0,3)+Plc_flg 4 =
  Liege-Atem-Loop (`[scd] Plc_motion(entity=0, motion=3 ...) -> slot=1`). Ansprechen = AOT-Slot 2 sce 5
  (@0x00D92) -> work_vars[0] = 2 -> sub01 @0x00E9E `23 00 00 00 02 00` + Ck(3,0x9D)==0 -> sub03 msg 2
  "Irons: I'll be fine. Just worry about / yourself for now." (@0x011C3).

* Szenen-Zeitlinien vorher (probe_r36_elza_mess am echten VM, Bild = VM-Takt ab Szenenstart):
  - ROOM1211 sub02 (Flags (4,0xEC)): F904 @0x020F8 `3f 00 00 00` Elza Plc_motion(0,0,0) = Aufschliess-Geste
    (ROOM1211-RBJ rec0 Clip 0 = 20 Bilder), Sleep 60; F963 @0x02100 Rueckwaerts; F964 @0x0210C Zellentuer
    (Prop 0) schwingt 49 Bilder auf. KEIN Ton (`audio`-Zeilen der Sonde: nur Stimmen-Ereignisse kind 5).
  - ROOM1021 sub02 (Flags (4,0x60),(3,0xC9)): F543 @0x020C6 John Plc_motion(2,6) (eigene Bank, 10 Bilder,
    Zielen), F653 @0x020CE John Plc_motion(2,7) = Schuss 1 (eigene Bank Clip 7, 16 Bilder), F683 @0x020D6
    Plc_motion(2,7) = Schuss 2, im selben Bild @0x020DE Roy Plc_motion(1,7) (Loco-Bank EM047 Clip 7 = 1 Bild).
    KEIN Ton. Echte exe (m4_1021, RE15_FORCE_EVENT=2@40): Schuesse in Bild 698/728; Framedump Cut 9 zeigt John
    (weisses Hemd, Hosentraeger) mit einer PISTOLE beidhaendig: Clip-7-Bild 0 Arme vorn, Bild 2 Waffe hochgeschlagen
    (Rueckstoss) — der Schuss liegt also auf Clip-Bild 0..1.
  - ROOM1011 sub02 (Roy-Dialog): @0x00D3A msg 16 "Roy: Please, you have to help him...", @0x00D52 Cut_chg 1,
    @0x00D54 `3f 00 09 00` Roy Plc_motion(0,9) = Roy sackt zusammen, Sleep 100, @0x00D5C msg 17 "Elza: Roy, wake
    up! Please wake up!". main16.5.wav wird nirgends geladen (Lader kennt nur main%02d.wav, audio_pc.c:2180).

## 3. RE-Belege je Punkt

### P6 main16.5 — Raum, Stelle, Wortlaut
* "Room 1101" ist ROOM1101 = bytegleich ROOM1100 (Stimmschloss, 6 Texte, KEIN Roy, kein Sce_em_set 0x47). Roy wird
  NUR in ROOM1011 bewusstlos: sub02 @0x00D54 (s.o.), danach msg 18 "He has passed out from his injuries." (sub07
  @0x00E64). Die Nutzerdatei liegt passend in synchro/STAGE1/room1011/ (unversioniert im Hauptbaum, 8.10. 00:12).
  -> Gemeint ist ROOM1011, die Stelle ist @0x00D54 (zwischen msg 16 und msg 17 = "16.5").
* Datei gemessen (RIFF-Kopf): PCM 16 bit mono 32000 Hz, data 92138 B = 1,44 s = 43,2 Bilder. Huellkurve (50-ms-
  Fenster): 0,35 s Stille, EIN stimmhafter Ausbruch ~0,6 s (Nulldurchgangsrate 0,01-0,03), hauchiges Ende (0,14-0,18)
  — dieselbe Form wie main00.wav ("Officer: groans", gleich lang, andere Samples) und main10.wav ("Roy: groans").
  Kein Silbenmuster -> ein Stoehnen, kein Satz.
* Untertitel "wie bei den anderen": RE1.5 untertitelt Roys Stoehnen selbst als `Roy: groans` (ROOM1011 msg 10
  @0x010C9). Port-Text = diese Bytes WOERTLICH (PORT-WAHL der Zuordnung, Form + Wortlaut original).
* Id: VERTRAG nennt ROOM1101 20..21 — die Szene liegt aber in ROOM1011, dort sind 20/21 Originaltexte ("You've
  taken the Prison key." / "Several books..."). ROOM1011 traegt 24 Nachrichten (0..23) -> Port-Id 24 nach der
  VERTRAG-Regel "nur falls noetig ab RDT-Zahl".

### P3 Unlock-Ton ROOM1211
* RE1.5: sub02 ohne Se_on (Zensus aller 240 RDTs: 284 Se_on, alle Bank 2; keiner in 1211/1021). Tueren haben in
  RE1.5 keinen Schloss-/Aufschliess-Ton (include/re15_lock_se.h: @0x80043084 / @0x800430bc stumm).
* RE2-Ton: Tuer-Handler 0x80051514 nach "You unlocked the door using the X" (msg 5, @0x8005164c jal 0x8002fe38):
  @0x80051654 `lui a0,0x225` / @0x80051658 `jal 0x8005ba28` / @0x8005165c `addu a1,zero,zero` = Raumbank-Satz 0x25,
  nicht positional. Zensus Satz 0x25 (re2_raum_se_zensus.py): 7 Wellen; gewaehlt ROOM1140 (5264 B, sha1 e12a441e5282;
  ROOM114 = "Locked/Unlocked door" der Nutzer-SAP-Liste; dieselbe Raumbank liefert schon ZU_A = Port-Tuer-zu-Ton).
* RE2-Zeitpunkt (eine Figur schliesst im Skript ab): ROOM1010.RDT sub09 @0x01C00 (Kendo verriegelt den Laden):
  @0x01C18 `3f 00 00 00` Plc_motion(0,0,0) (ROOM1010-RBJ rec0 Clip 0 = 20 Bilder), @0x01C1C `09 0a 14 00` Sleep 20,
  @0x01C20 `36 02 0a 01 ..` Se_on(2,0x0A), @0x01C2C Sleep 10, @0x01C30 `3f 00 00 c0` rueckwaerts. -> Ton 20 Bilder
  nach dem Start der 20-Bilder-Geste (= Geste fertig). RE1.5 1211 hat dieselbe Form: @0x020F8 Plc_motion(0,0,0),
  Clip 0 ebenfalls 20 Bilder, dann Halt + Rueckwaerts @0x02100.

### P4 Schuesse ROOM1021
* RE1.5: kein Ton (s.o.); RE1.5 kennt im ganzen Spiel keinen Skript-Schuss (ROOM1041 sub11 @0x02200 Message_on 3 =
  Platzhaltertext "gunshot" statt eines Tons).
* RE2-Zeitpunkt: ROOM1010.RDT sub30 @0x02632 (Kendo schiesst): @0x02676 `04 ff 18 1f` Evt_exec sub31 und @0x0267A
  `3f 00 0c 00` Plc_motion(0,12,0) (Feuer-Clip) im selben Bild; sub31 @0x02680 `02 00` Evt_next, dann @0x0268E
  `36 02 0c 01 03 ..` Se_on(2,0x0C) + Muendungsfeuer -> Schuss-Ton 1 Bild nach dem Start des Feuer-Clips.
  Passt zur Bildmessung: Rueckstoss sichtbar ab Clip-Bild 2.
* RE2-Ton: Johns Waffe ist eine Pistole. RE2s Kendo-Schuss ROOM1010 Satz 0x0C ist bytegleich ARMS07 VAG 2
  (Schrotflinte, sha1 42bbd9b31614, 15808 B) -> passt NICHT. Skript-Pistolenschuss eines NPC in RE2: ROOM40A0.RDT
  sub04 @0x0287E: @0x02881 `3f 02 0e 04` Plc_motion(2,14,4) (Schiess-Clip), @0x02896 `36 02 08 01 02 00 00 00 00
  00 00 00` Se_on(2,8) + @0x028A2 Muendungsfeuer. Satz 0x08 = EDT @0x09E00 `00 00 a3 00`, VAG @0x17B80 4736 B
  (sha1 e552c5d77eed). Huellkurve: Anschlag 0 ms, Spitze 10 ms, hoerbar 260 ms, Nulldurchgang 0,19 (= Knall).
  (Gegenprobe ENEMSE Bank 30 "Ada ... with a notable added gunshot": VAG 6 db906adc10f9 hat dieselbe Knall-Form;
  der Skript-Satz aus ROOM40A0 ist die engere Vorlage, weil Skript + NPC-Clip.)

### P5 Roys Sterbe-Ton ROOM1021
* Roy ist hier ein Zombie ("That thing isn't Roy anymore!"). NUTZER-MANDAT 2026-08-23 (enemy_ai_re2_zombie.c:766):
  Zombie-Toene = RE1.5-Raumbank snd1. RE1.5-Zombie-Tod im STEHEN FUN_80106c18 Phase 0: @0x80106da4 `ori a0,zero,0x8`
  / @0x80106da8 `ori a0,zero,0x5` (ueberschreibt, kein Sprung dazwischen) / @0x80106dac `jal 0x800453d0` = Raum-SE 5,
  im Bild des Todes-Eintritts. ROOM1021 snd1 (EDT @0x145A8): Satz 5 = `00 00 69 15` -> VAG 6, 15392 B, hoerbar
  1,13 s, Anschlag 270 ms, Nulldurchgang 0,07 (stimmhaftes Sterbe-Stoehnen).
* Zeitpunkt: Roys Todes-Clip startet im Bild von Schuss 2 (@0x020DE nach @0x020D6); der Treffer kommt mit dem
  Schuss -> Sterbe-Ton im Bild des Schuss-Tons (Ursache vor Wirkung, beide im selben Takt).

### P1 Fenster + Kraehe ROOM1121
* ROOM1121.RDT == ROOM1120.RDT (cmp), Hintergrund ROOM112.BSS ohne Variante -> die Schadens-Ueberlagerung der
  Runde 35 (gen/fenster1120_schaden.inc, 309 Pixel-Operationen, Cut 1) passt unveraendert. Leon-Ereignis (Splitter,
  Knall, (9,79)) bleibt Leon; bei Elza "bereits immer kaputt" = ohne Ereignis.
* Zusatz-Kraehe = die Fensterkraehe, schon hereingeflogen:
  - Form: RE1.5-Sce_em_set wie die drei Raum-Kraehen main00 @0x00D0C/@0x00D20/@0x00D34 (`44 0n 21 00 00 00 00 a7..a9`,
    alle y = 0), Gegner-Slot 3 (main00 belegt 0..2).
  - Lage: Ende des 7-Bilder-Durchflugs der Leon-Fensterkraehe (RE2-Perch Sub 2 P2 @0x801038d8..: speed 300, -10/Bild
    @0x80103900-04, 7 Bilder @0x801038a0 `+0x219 = 6`), gemessen probe_r36_elza_mess 1120 9:73: Start
    (5400,-2500,12100) -> Bild 8 (5327,-2500,10217) -> am Boden (5327, 0, 10217). Gier 0x0418 (= Fensterkraehe).
  - Kill-Flag: Zensus pc[7] aller Sce_em_set + Set/Ck(7,..) in den 132 RDTs von STAGE1-3 (Zone 7 =
    re15_em_status_zone, Stage 1-3); frei u.a. 0xA0-0xA3, 0xAC, 0xAE, 0xAF; nicht in der Montage-Liste der Runde 35
    (test_r35_cut1150.c frei_zone7). Gewaehlt 0xAC (neben den 1120-Kraehen 0xA7-0xA9). Erlegt -> Gate @0x80042128-38.
  - Spawn: Port-Programm (22 B: Sce_em_set + Evt_end) ueber Ereignis 26 (VERTRAG L), Weiche in re15_fenster1120_ereignis.

### P2 Irons ROOM1151
* Original (ROOM1151 main00 @0x00E6A Irons, sub00 -> sub02 @0x00EB8 Plc_motion(0,3)+@0x00EBC Plc_flg 4 = Atem-Loop;
  Slot 2 @0x00D92 sce 5 -> work_vars[0] = 2 -> sub01 @0x00E9E -> sub03 msg 2 "I'll be fine"). NUTZER-VORGABE: tot.
* Totenpose = Leons Irons nach der Todesszene (Runde 35 Spur L): Clip 2 Bild 89 gehalten (Halt @0x80050da4). ROOM1151
  RBJ rec1 (Datei 0x1387C) = ROOM1150 rec1 {36,60,90,35,30,102,34} -> Clip 2 hat 90 Bilder, Bild 89 = Ende.
* Kopf: der NPC-Wurzelknoten ruft die Kopf-FSM 0x80037358 in JEDEM Takt (Irons-Wurzel 0x8011d140: @0x8011d278
  `jal 0x80037358`, nur @0x8011d160 Pause / @0x8011d180 +0x9&0x20 springen darueber). Daten-Weg, die Verfolgung zu
  beenden = Plc_neck: Modus 0 -> +0x1b8 = 0x80|0x12 (@0x80041ed8-f10), Geschwindigkeit pc[8..9] -> +0x9e/+0x9f
  (@0x80041f5c-6c); der Slew bewegt um das woertliche Byte (@0x800375dc / @0x800377a0), 0 haelt die Achse (ROOM10D0
  Plc_neck mit 0x0A00 = Gier steht). Mit Akkus 0 liegt der Kopf genau auf dem Clip.
* Text: "He has already passed..." (NUTZER-VORGABE woertlich), Form `04 02 .. 01 00` wie ROOM1021 msg 9 @0x024FB;
  Glyphen aus ROOM1011 @0x01233 ("He has"), ROOM10D0 @0x02226 (" already"), ROOM1011 @0x01239 (" passed"),
  @0x012BA ("..."). Id 20 (VERTRAG 1151 L 20..23). Platz: Slot 2 per Aot_reset-Semantik auf sce 1 (Flags 0x31 des
  Platzes, Maske 0xffff wie alle 524 sce-1-Saetze) -> work_vars[0] wird nie 2, sub03/msg 2 laufen nie.

## 4. Umsetzung

| Datei | Inhalt |
|---|---|
| engine/src/elza_r36.c + include/re15_elza_r36.h (NEU) | P2 Installer (Totenpose, Kopf, Text 20, Slot 2); Anker-Tabelle (Raum, Datei-Offset, 8-Byte-Signatur, Aktion, Verzug) fuer P3-P6; Takt fuer verzoegerte Aktionen; Stimmen-Dateistamm (1011,24)->main16.5; Messschiene RE15_ELZA_LOG |
| engine/src/fenster_1120.c + include/re15_fenster1120.h (Elza-Zweig) | schaden_sichtbar in 1121 = Cut 1; Zusatz-Kraehe per Port-Programm Ereignis 26 |
| platform/pc/src/elza_se_pc.c (NEU) | Lader der Mini-Bank ELZASE.VBS, re15_audio_re2_elza_se, Haken-Init |
| engine/src/gen/re2_elza_bank.inc + shared_assets/RE2/ELZASE.VBS (NEU) | tools/r36_l/re2_elza_se_cut.py --install (Satz 0 ROOM1140 0x25, Satz 1 ROOM40A0 0x08, bytegleich bis auf Satz-/Tonindex) |
| engine/src/scd_vm.c (3 Haken) | Anker-Scan in scd_register_current_rdt, Opcode-Haken (Wachposten Ankerzahl), Takt neben re15_irons_tod_tick |
| engine/src/scd_room_setup.c (1 Zeile + include) | re15_elza_r36_install am Ende des Installer-Blocks ("Runde 36 Spur L") |
| platform/pc/main.c (2 Zeilen) | Boot-/CONTINUE-Weg: re15_elza_r36_install; re15_pc_elza_se_init neben re15_pc_glas1120_init |
| platform/pc/src/audio_pc.c (klein) | re15_audio_se_play_bank (Durchreiche auf se_play_layers); Stimmen-Lader: Dateistamm ueber re15_elza_stimme_datei, Log nennt den echten Stamm |

Konstanten mit Beleg: Anker @0x020F8 (+20, RE2 ROOM1010 @0x01C1C), @0x020CE/@0x020D6 (+1, RE2 ROOM1010 sub31
@0x02680), @0x020DE (+1, im Takt von Schuss 2), @0x00D54 (0); Toene RE2 ROOM1140 Satz 0x25 / ROOM40A0 Satz 0x08, RE1.5
Raum-SE 5 (@0x80106da8); Kraehe Slot 3/Typ 0x21/Kill 0xAC/(5327,0,10217)/0x0418; Irons Clip 2/Bild 89/Neck 0x92,0;
Nachrichten 20 (1151) / 24 (1011).

## 5. Messung nachher (echte exe, Kopie re15_pc_r36l.exe)

* P1 (n1, Raumwechsel-Weg GOTO, Cut 1 Bild 90): `[spawn-diag]` 4 Kraehen, Slot 3 `pos=(5327,0,10217) dir=1048`;
  state.log Aktor 4 Typ 0x21 @(5327,10217). Bildvergleich Leon 1120 / Elza 1121 an den 309 Schadens-Pixeln:
  309/309 = LEON (+) Op exakt, 309 verschieden. Bild: Fenster mit Spruengen, die vierte Kraehe am Fenster.
* P2 (n2b, Spielstand + CONTINUE, Boot-Weg): Irons alle Bilder `[1 t=45 st=4 ss1=0 ss2=2 .. mo=2 af=89]`;
  Anklicken (AOT-Feuer-Pfad, Bild 60) -> `msg(a=1 fsm=1 id=20)`; Framedump Cut 7: Irons liegt tot (Arm herab) auf
  der Liege, Untertitel "He has already passed...".
* P3 (n3, CONTINUE in 1211 im Auto-Feld): elza.log `F924 Anker @0x020F8 -> Aktion 1`; Mitschnitt (CAP_SYNC):
  Spitze 9363 in Bild 935 (Median 1900) — der Ton setzt in 924 ein, die Welle hat ihren Anschlag ~11 Bilder spaeter
  (Ton-Tonhoehe Note 67 gegen Mitte 89 = langsam abgespielt).
* P4/P5 (n4b, AOT-Ereignis 2): Anker 699 (Schuss 1), 729 (Schuss 2 + Roy). Mitschnitt: Bild 699 Spitze 6513->7039
  (vorher ~1000), Bild 729 6228->7137, abklingend bis ~740 (Knall-Form); ab Bild 741 stimmhaft (Nulldurchgang
  0,02-0,03), Spitze 8073 bei 764 = Roys Sterbe-Stoehnen (Anschlag 270 ms + tiefe Tonhoehe).
* P6 (n6, CONTINUE 1021 -> Tuer 1 -> 1011 Cut 0, Nutzer-Ordner room1011 im Arbeitsordner): debug.log
  `[voice] clip loaded: main16.5.wav (32000 Hz x1 -> 63488 @44100 Hz)`, elza.log `F1939 Anker @0x00D54 -> Aktion 4`,
  state.log ab F1939 `msg(a=1 fsm=1 id=24)`; Mitschnitt-Korrelation mit main16.5.wav = 0,607 (Versatz 1 Bild),
  Energie steigt ab Bild 1952 (0,35 s Vorlauf der Datei) und klingt bis 1990 ab.

## 6. Tests

Neu (registriert NUR in tests/unit/probes/r36_elza.cmake): 8 Unit-Teile + 5 Integrationsteile = 13 Tests
(Orchestrator: RE15_MIN_TESTS um 13 anheben).

| Test | misst |
|---|---|
| unit_r36_elza_fenster | 1121: vier Kraehen, Elza-Kraehe Aktor 4 bei (5327,10217), Kill-Flag 0xAC gelatcht; Schaden nur Cut 1 ohne Ereignis/(9,79); erlegt -> drei; Leon 1120 unberuehrt |
| unit_r36_elza_irons | 1151: 150 Bilder Clip 2 Bild 89 Phase 2; Kopf 0x92/0/(0,0); Slot 2 = Text 20 Maske 0xffff; Anklicken -> Text 20, kein msg 2, (3,0x9D)=0; Glyphen = zitierte RDT-Bytes; Leon 1150 unberuehrt |
| unit_r36_elza_unlock | 1211: genau ein Ton Satz AUF, 20 Bilder nach der Geste @0x020F8, vor dem Aufschwingen @0x0210C |
| unit_r36_elza_schuss | 1021: Feuer-Clips 30 Bilder auseinander, zwei Schuss-Toene je +1 Bild |
| unit_r36_elza_roy | 1021: genau ein Raum-SE 5 im Bild von Schuss-Ton 2, gleicher VM-Takt, Schuss zuerst |
| unit_r36_elza_stoehnen | 1011: Nachricht 24 + Stimme 24 im Bild von @0x00D54 (Roy Clip 9), Datei (1011,24)->main16.5, Text = msg 10 @0x010C9 woertlich, danach msg 17 |
| unit_r36_elza_bank | ELZASE.VBS: 2 VAGs 5264/4736, EDT/Ton, VAG-Daten bytegleich RE2 ROOM1140 @0x04480 / ROOM40A0 @0x17B80 |
| unit_r36_elza_signatur | Anker nur bei passender 8-Byte-Signatur; Leon-Raeume ohne Anker |
| integration_r36_elza_fenster | echte exe: 4 Kraehen-Spawns, 309 Schadens-Pixel = Leon (+) Op |
| integration_r36_elza_irons | echte exe, Spielstand + CONTINUE: Irons alle Bilder tot, Anklicken -> Text 20, kein msg 2 |
| integration_r36_elza_unlock | echte exe, Spielstand + CONTINUE: Unlock-Aktion einmal, Mitschnitt-Spitze >= 3x Median im Fenster danach |
| integration_r36_elza_schuss | echte exe: 2 Schuesse 30 Bilder auseinander, Roy im Bild von Schuss 2, Mitschnitt-Spitzen (Schuss >= 4x, Roy >= 2x Median) |
| integration_r36_elza_stoehnen | echte exe, CONTINUE 1021 -> Tuer -> 1011: Aktion, `clip loaded: main16.5.wav`, Nachricht 24 offen |

Zusatzmessung Kopf (echte exe, RE15_NECK_TRACE, CONTINUE, Cut 7, je 201 Kopf-Takte, gleicher Bau):
* Elza ROOM1151: jede Zeile `slot=1 fl=92 ... acc=(0,0) ... step=(0,0)` -> Kopf liegt auf dem Clip, keine Verfolgung.
* Gegenprobe Leon ROOM1150 mit (9,73)=1 (Irons tot nach der Todesszene, Spur-J-Thema): `fl=00 ... acc=(-111,-52)
  step=(64,48)` -> der Kopf verfolgt den Spieler. Genau das waere Elzas toter Irons ohne den Plc_neck-Halt.

### Ergebnis gezielte Tests (KEINE volle Suite)
* `ctest -R "r36_elza|r35_fenster|r35_cut1150|irons_neck|crow|synchro_stimme|voice_ausreden|se_voice_machine|msg_dialog|text_freeze|elza_zweig|elza_vollstart|r35_werfer_elza|r33_tuer1120|r34n_d_adaruf_elza|r34n_f_leiche_stimme|r30_elza_selbsttuer|r30_tuerse_bank|r30_tuer_stimme|r30_irons|irons_mittelmodell|r35_inventar1050_elza" --timeout 600`
  -> `100% tests passed, 0 tests failed out of 69` (Engine-/Plattform-Stand dieses Zweigs; integration_r35_cut1150 531,9 s).
* Nach den letzten Test-Aenderungen (Kopf-Pruefung, Kartenwerkzeug): `ctest -R "r36_elza" --timeout 1200`
  -> `100% tests passed, 0 tests failed out of 13`.

## 7. OFFEN

1. **Roy faellt nicht sichtbar** (nicht beauftragt, Beobachtung): ROOM1021 sub02 @0x020DE `3f 01 07 00` = Motion-Sub 1
   = Loco-Kanal +0x84/+0x16c (Executor @0x80050e64 `lw a0,132(v0)` / @0x80050e6c `lw a1,364(v0)`); EM047 dir[1] Clip 7
   hat 1 Bild -> der Port zeigt Roy beim zweiten Schuss stehend. Die RBJ von ROOM1021 traegt in rec1 einen Clip 7 mit
   50 Bildern ({20,30,30,20,30,25,35,50,24,20}, Datei 0xCE50); ob das Original Roys Kanal dorthin legt, ist UNGEMESSEN
   (Weg: DuckStation-Savestate in der Szene, Roy-Entity +0x84/+0x16c lesen). Der Sterbe-Ton (P5) haengt am Treffer,
   nicht am Clip — er bleibt richtig, wenn der Clip spaeter korrigiert wird.
2. **Muendungsfeuer**: RE2 setzt zum Skript-Schuss ESP-Muendungsfeuer (ROOM40A0 @0x028A2, ROOM1010 sub31 `3a 00 18 ..`);
   nicht beauftragt, nicht gebaut.
3. **Wortlaut main16.5**: Untertitel = "Roy: groans" (RE1.5s eigene Zeile fuer Roys Stoehnen). Falls die Aufnahme Worte
   enthaelt: Wortlaut vom Nutzer (s. 8.).
4. Die Unit-Pruefung des Kopfes laeuft ohne Render-Pfad (die Kopf-FSM lebt in skeleton_common.c); belastbar ist
   integration_r36_elza_irons (RE15_NECK_TRACE an der echten exe).

## 8. Fuer den Nutzer / Orchestrator

* **Neue Assets (Paket-Gate)**: `re15_port/shared_assets/RE2/ELZASE.VBS` (13144 B, md5 8e996ef238defe2188292e55af7f8ab2;
  erzeugt von re15_port/tools/r36_l/re2_elza_se_cut.py --install).
* **Sprachdatei**: `synchro/STAGE1/room1011/main16.5.wav` (Nutzer-Datei, im Hauptbaum unversioniert, NICHT umbenannt) —
  der Port laedt sie unter genau diesem Namen fuer ROOM1011 Nachricht 24 (re15_elza_stimme_datei). Im Paket muss sie
  neben den anderen synchro-Dateien liegen.
* **Neue Quelldateien** (Android-Configure frisch, Memory reai-v2-android-glob-cache): engine/src/elza_r36.c,
  platform/pc/src/elza_se_pc.c. **RE15_MIN_TESTS +13** (8 Unit + 5 Integration).
* **Id-Abweichung vom VERTRAG**: main16.5 gehoert zu ROOM1011 (nicht 1101) -> Nachricht 24 (RDT-Zahl 1011); 1101 bleibt frei.
* **Frage an den Nutzer**: main16.5 ist ein einzelnes Stoehnen (gemessen) und traegt den Untertitel "Roy: groans" wie
  Roys Original-Zeile msg 10. Soll dort ein anderer Text stehen (welcher Wortlaut)?
* **Hinweis**: Roy sackt in ROOM1021 beim Schuss nicht sichtbar zusammen (OFFEN 1) — soll das als eigener Punkt
  (Original messen, ggf. RE2-Vorlage) in die naechste Runde?
