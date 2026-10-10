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
- [ ] P1 Fenster 1121 kaputt + Kraehe
- [ ] P2 Irons 1151 tot + Text
- [ ] P3 1211 Unlock-Ton
- [ ] P4 1021 Schuss-Toene
- [ ] P5 1021 Roy Sterbe-Ton
- [ ] P6 main16.5 + Untertitel

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
