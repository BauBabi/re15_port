# Runde 38 — Spur G "tagebuch" (Dossier, fortlaufend)

Baum `.claude/worktrees/r38_tagebuch`, Zweig `r38/tagebuch`, Basis e2cc7673. Angelegt 2026-10-16 16:19.

Punkte (Wortlaut AUFTRAG.md Z. 36-64, massgeblich):
1. Elza, ROOM1011: Roy, der am Boden liegt, noch einmal anklicken -> Tagebuch "Roys Diary" (Hintergrund
   FILE16_title_paper) STATT der Code-Nachricht; Text woertlich (Z. 39-59); Code 0513 gruen "wie alle Codes in den
   Dokumenten".
2. "Und danach": auf der Karte einmal den Communications Room und einmal den Prison Room markieren, "so wie auch schon
   bei Leon im Communications Room gemacht".

Ressourcen (VERTRAG §1): Bank-9-Bit 68 (genutzt); Msg-Ids 1011 40-44 (NICHT gebraucht); Dokument 5 per Zensus.

## Stand
- [x] Messung vorher (V1/V2)
- [x] RE-Belege (ROOM1011 sub07/msg 18, RE2 Dokument-Aufnahme, RE2 Hinweis 0x84, RE2-Tagebuchform, Gruen, Zensus)
- [x] Umsetzung (tagebuch_r38.c + FILE30 + 6 Haken)
- [x] Messung nachher (N1-N3, echte exe)
- [x] Tests (unit_r38_tagebuch_* 4, integration_r38_tagebuch_* 3, gezielte Regression)

## Messung vorher

Bau des Baums (Stand e2cc7673 + Dossier), `local_build.sh configure` + `build` = LOCAL-BUILD-OK.

### V1 — Roy (Elza, ROOM1011) nach dem Dialog untersuchen, echter Weg ueber Spielstand + CONTINUE
Lauf `scratchpad/G/v1`: Karte `probe_r36_elza_karte re15_card.mcr 1011 -1900 6830 3072 4:0xF1 4:0xFA 3:0x74`
(Stand NACH dem Dialog sub02: (4,0xF1)/(4,0xFA)/(3,0x74) = sub02 @0x00AFE/@0x00B02/@0x00B06), Elza vor Roy, Blick +z
(Gier 3072: Vorwaertspunkt x + 620 cos, z - 620 sin, aot_common.c:1273 / @0x80042bd0), CONTINUE, Viereck (Aktion,
game_step_common.c:1110) in Bild 60. debug.log:
    [schluessel] ROOM1011: Roy -> sub07 (Zettel 0513) statt sub06 (Prison key)      <- Runde 36 Q: Ereignis 6 -> 7
    [scd] Plc_motion(entity=1, motion=11, flags=0x00)                                 <- sub07 @0x00E5C, Elza beugt sich
    [msg] room=1011 id=18 savepoint=0
    [msgtext] room=1011 id=18 "He has passed out from his injuries.He is holding a slip.The numbers 0513" are printed on the slip."
Bild `G_belege/vorher_1011_msg18.png` (Bild 60 / Bild 120): die Code-Nachricht msg 18, Seite 1 mit Weiter-Pfeil.
Damit Ist-Zustand: JEDES Untersuchen des liegenden Roy zeigt msg 18 (Zettel mit 0513); kein Dokument, keine Karte.

### V2 — Karte bei Elza
Hinweis-Tabelle `map_hint_common.c` s_hints[]: 3 Eintraege, alle Leon (Quelle 0x1150 Ziel 0x10F0; Quelle 0x10F0 Ziele
0x11C0/0x1150). Fuer Elza gibt es keinen Eintrag -> keine Markierung.

## RE-Belege

### R1 — Die Code-Nachricht im Original (ROOM1011.RDT, selbst gelesen, `tools/scd_dump_room.py`, `xxd`)
    sub00 @0x00A2C Ck(4,0xFA)==1 -> @0x00A3E Sce_em_set Roy (Typ 0x47) + @0x00A52 Aot_set Slot 5 sce 3 -> sub06
    sub02 @0x00DC0 `46 05 03 31 ff 00 18 06 00 00`  Ende des Dialogs: Slot 5 -> sub06
    sub06 msg 19 "He is holding something..." + msg 20 "You've taken the Prison key." (Runde 36 Q: Ereignis 6 -> 7)
    sub07 @0x00E4E (Datei-Bytes 0xE4E..0xE83):
      46 05 00 00 00 00 00 00 00 00   Aot_reset(5,0)          Slot aus
      2e 01 00 00                      Work_set(1,0)+Nop       Elza
      3f 01 0b 00                      Plc_motion(1,11)        @0x00E5C Elza beugt sich
      09 0a 1e 00                      Sleep 30
      2b 12 ff ff                      Message_on 18, Maske 0xffff   @0x00E64  <- DIE Code-Nachricht
      02 00                            Evt_next
      3f 01 0b 00 43 00 80 00          Plc_motion(1,11)+Plc_flg 0x80 (zurueck)
      09 0a 1e 00                      Sleep 30
      42 00                            Plc_ret
      46 05 03 31 ff 00 18 07 00 00    Aot_reset Slot 5 -> sub07  @0x00E78 (JEDES weitere Untersuchen = sub07)
      01 00                            Evt_end                    @0x00E82
    msg 18 @0x1231: `04 02 24 41 00 44 3d 4f ... 02 00 ... 02 00 30 44 41 00 4a 51 49 3e 41 4e 4f 00 61 05 01 0c 11 0d 0f
      05 00 19 ...` = "He has passed out from his injuries.<Seite>He is holding a slip.<Seite>The numbers "<GRUEN>0513<WEISS>"
      are printed on the slip." — `05 01` faerbt 0513 gruen (Ziffern = ASCII - 0x24: 0c 11 0d 0f).
    synchro/STAGE1/room1011: main00..main17 — fuer msg 18 gibt es keine Sprachdatei (nichts geht verloren).

### R2 — Gruen = Textmaler-Zeile 482 (PSX.EXE, re15_disasm.py)
    FUN_80028868 Steuerbyte 0x05: 80028974 andi v1,v0,0x4 / 80028978 sltu v1,zero,v1 / 8002897c andi v0,v0,0x3 /
    80028980 sll v0,v0,1 / 80028984 addiu v1,v1,480 / 80028988 addu v0,v0,v1 / 8002898c sll v0,v0,6 /
    80028994 ori s5,v0,0x10  -> CLUT-y = 480 + (1&3)*2 + 0 = 482 = DATA/TEX.TIM CLUT-Block Zeile 2.
    Dieselbe Abbildung wie FILE28 "4312"/FILE29 "5632" (Runde 35 F, doc_satz_brief.py Optional 5): Kern 1..6 -> 9..14,
    CLUT 9..14 = TEX.TIM Zeile 2 Eintraege 1..6 = 16e0 1280 0e20 09e0 0980 0540 (aus der Datei gelesen).

### R3 — Dokument = RE2-Dokument-Aufnahme (RE2 info/re2leon/PSX.EXE, re2_disasm.py) — Antwort auf "in die Akten?"
    FUN_80071ba0: 80071bbc sltiu v0,a3,0x68 / 80071bc0 beq v0,zero,0x80071d00   Id >= 0x68 -> Dokument-Zweig
                  80071d00 jal 0x800692dc / 80071d04 addiu a0,a3,-104           AN DIE FILE-LISTE (VOR dem Lesen)
                  80071d10 srl v1,v0,3 / 80071d14 andi v0,v0,0x7                 Platz -> Reihe/Spalte
                  80071d30 sb zero,23555(at)                                      Seite 0
                  80071dc0 addiu v0,zero,4 / 80071dc4 sb v0,1(s1)                 Modus 4 = Leser
    FUN_800692dc: 800692e0 addiu a2,zero,255 / 800692ec lbu v0,0(v1) / 800692f4 bne v0,a2,0x80069304 /
                  80069300 sb a0,0(v1) / 80069308 sltiu v0,a1,0x18 — erster freier Platz, KEINE Doppelpruefung.
    Item-Zone FUN_80051884: 800518f8 sb v1(=2),[0x800d5c00] / 80051904 sb v1(=1),[0x800df348] — keine Abfrage.
    Abraeumen nach der Meldung (Zustand 6): 80072b1c bne (Meldung steht -> warten) / 80072b40 sb zero,0(v1) (Zone
    [0x800ce5d8] aus, Verzoegerungsplatz) / 80072b8c jal 0x8007730c (Aufnahme-Flag) — ein genommenes Dokument wird
    nie ein zweites Mal angeboten; deshalb fuehrt die Liste kein Dokument doppelt.
    => RE2 nimmt ein gefundenes Dokument IMMER in die Akten auf. Port: der vorhandene Weg re15_menu_request_doc
       (Runde 30), Item-Id 0x48 + 5 (RE1.5 erste Dokument-Id u8 @0x800c7370 an der Stelle von RE2s 0x68).

### R4 — Kartenhinweis (RE2 Opcode 0x84, re2_disasm.py) und Leons Form
    800591d4 addiu v0,zero,4 / 800591dc sb v0,[0x800D5C00]   Statusschirm-Modus 4 (Hinweis)
    800591e0 addiu v0,zero,1 / 800591e8 sb v0,[0x800DF348]   Anforderung
    800591ec ori v1,v1,0x8000 / 800591fc sw v1,[0x800CFBD8]  Bit 0x8000
    80059208 lbu v0,1(v0) / 80059210 sb v0,[0x800D69F2]      Hinweis-Nummer = Skriptbyte 1 -> EIN Ziel je Opcode.
    Leon im Port: Runde 30/33 (ROOM1150 sub08-Evt_end @0x012EC -> Communications Room ROOM10F0, START schliesst) und
    Runde 35 K (Szene IM Communications Room ROOM10F0 -> Kette 11C0 -> 1150, "kurz eine Weile" je Ziel =
    RE15_CUT10F0_HINWEIS_PERIODEN 3 x 78 VBlanks, START schaltet weiter, danach blinken beide in der normalen Karte bis
    zum Ziel). "einmal den Communications Room markieren und einmal den Prison Room" = zwei Ziele nacheinander = genau
    diese Kettenform; sie wird unveraendert benutzt (Folge + zeitgesteuert, keine neue Konstante).

### R5 — Satz: RE2-Tagebuchform und Vorlage FILE16 (gemessen, tools/r38_g/tagebuch_satz.py, Bericht G_belege/FILE30_satz.txt)
    * FILE16 = RE2-Dokument 16 "Chief's diary" (Transkription FILE16.txt, Item-Id 0x78). RE2-Record @0x800AA184
      (= 0x800AA144 + 16*4) = `06 00 50 00` (max_page 6, y_off 0x50 -> H 176); erster Slot @0x800A9AE0 = 0x6b = 107.
    * FILE16_title_paper.TIM: 128x256 8bpp, CLUT[0] = 0x7fff, KEIN CLUT-Eintrag 0x0000, Zeilen 176..255 ohne Index 0
      -> die Illustration (Tagebuch/Organizer mit Stift) wird deckend gezeichnet wie in RE2; byte-gleiche Kopie.
    * Datumskoepfe: in allen 6 RE2-Tagebuechern (FILE00/07/08/15/16/17) steht JEDER der 20 Datumskoepfe als erste Zeile
      einer Seite (Einsatzberichte FILE04/09: Kopfzeile davor); Form mit Ordinal ("September 23rd").
    * Satzspiegel FILE16 gemessen: Rand 10 (39 Zeilen), Grenze x 253; 11 Zeilen je Seite (H 176 / 16).
    * Titel: breiteste einzeilige RE2-Titelzeile 247 px (FILE23); "ROYS DIARY" 83 px -> eine Zeile, Oberkante 80
      (Vorlagenband y 83..91), Mitte 127,0.
    * Atlas md5 8cbe2c52... = der Atlas der Runde 34 (reproduzierbar).

### R6 — Zensus (tools/r38_g/zensus.py, G_belege/zensus.txt)
    240 RDT: Item-Id 0x4D in 0 Item-Zonen (416 Item_aot_set + 52 Aot_reset sce 9); Bank 9 Bit 68: 0 Set/Ck, 0 als
    Aufnahme-Bit; Bildsaetze vor dem Bau 00..29 -> 30 frei. Port-Dokumente 0..4 (Irons Diary, 4 x Runde 34) -> Nr 5.
    Prison Room = ROOM1211 "DETENTI. AREA" (DEBUG.BIN JUMP-Satz 33 = 0x1210, include/debug_jump_table.h): John
    sub00 @0x01E36 `44 00 49 40 ...` Typ 0x49 bei (-26225,0,-25100), msg 10 "It's locked.It seems that there is
    someone inside..."; Zone 0 (Hauptzeile) Blatt 1 (B2) Rechteck 3. Communications Room = ROOM10F1 (Satz 15 =
    "COMMUNIC. ROOM"), Zone 0 Blatt 3 (2F) Rechteck 9 (Gastzeile Blatt 2 wird uebersprungen).

## Umsetzung

Neue Dateien:
* `re15_port/include/re15_tagebuch_r38.h`, `re15_port/engine/src/tagebuch_r38.c` — Haken an Message_on 18 in ROOM1011:
  fordert den Dokument-Leser an (re15_menu_request_doc(5, 68, -1, -1): kein Aufhebe-Slot, kein Weltmodell — NUTZER-
  VORGABE "Muss nicht unbedingt ein Welt Model haben"), PARKT den Faden auf dem Opcode, bis der Statusschirm samt
  Blenden durch ist, und laesst ihn dann HINTER das Message_on laufen (Elzas Beugen davor und Aufrichten danach bleiben
  die des Originals). Erstes Lesen (Bit 68 vorher 0, nachher 1) -> am Evt_end des Fadens (@0x00E82, Versatz 0x1E vom
  Message_on) die Kartenkette (re15_map_hint_request). Spaeter: nur lesen.
* `re15_port/tools/r38_g/tagebuch_satz.py` (Satz, Rueckleseprobe, --installieren), `re15_port/tools/r38_g/zensus.py`.
* `re15_port/shared_assets/RE2/FILES/FILE30_{title_page,p00..p05_page,title_paper}.TIM` (8 Dateien, md5 im Bericht).

Haken in gemeinsamen Dateien (Kommentar "Runde 38 Spur G"):
* `engine/src/scd_vm.c` op_message_on: 1 Include + 3 Zeilen hinter dem Leichen-Haken (vor dem Oeffnen jeder Nachricht).
* `engine/src/game_step_common.c`: 1 Include + `re15_tagebuch_r38_tick();` vor der Annahme der Hinweis-Anforderung.
* `engine/src/menu_common.c`: 1 Include, Merker `s_doc_nur_lesen`, Dokument-Aufnahme: steht das Dokument schon in der
  Liste -> nicht anhaengen; Leser-Schliessen: nur gelesen -> Schirm zu (keine Meldung). Fuer die Welt-Dokumente der
  Runden 30/34 aendert sich nichts (ihre Zone ist nach dem Aufheben weg, sie kommen nie zweimal).
* `engine/src/map_hint_common.c`: 1 Include + 2 Tabellenzeilen (Eintrag 3 Comms ROOM10F1 Folge 4, Eintrag 4 Prison
  ROOM1211; "gezeigt" (9,68), zeitgesteuert 1, erreicht (9,83) bzw. (4,0xFC)).
* `engine/src/re15_files.c`: Dokument 5 { 0x4D, 30, 5, 176, "Roys Diary" }.
* `tests/unit/test_r36_kartenbits.c` k_port[]: Bit 68 (+ Include).

Konstanten (alle mit Beleg in re15_tagebuch_r38.h):
| Konstante | Wert | Beleg |
|---|---|---|
| RAUM | 0x1011 | Roy nur dort (Sce_em_set 0x47 @0x009F6/@0x00A3E; Runde 37 K §2.2b) |
| MSG | 18 | sub07 @0x00E64 `2b 12 ff ff`, Text @0x1231 |
| EVT_END | 0x1E | @0x00E82 - @0x00E64 (`01 00`) |
| DOK / Item | 5 / 0x4D | Zensus R6; RE1.5 u8 @0x800c7370 = 0x48; RE2 `addiu a0,a3,-104` @0x80071d04 |
| BILDSATZ | 30 | PORT-WAHL, naechster freier Satz (Regel der Runde 34) |
| MAX_PAGE | 5 | gemessen am Satz (p05 da, p06 nicht) |
| SEITE_H | 176 | RE2 @0x800AA184 `06 00 50 00` (Vorlage FILE16) |
| BIT | 68 | VERTRAG 1.1; Zensus R6 |
| ZIEL1 | 0x10F1, erreicht (9,83) | R6; Prison Key = schluessel_r36.h RE15_R36Q_PRISON_BIT |
| ZIEL2 | 0x1211, erreicht (4,0xFC) | R6; ROOM1211 sub02 @0x01F76 `22 04 fc 01` |

PORT-WAHLEN (begruendet, keine Wahlfrage):
* "Erreicht" der Ziele = Aufgabe erledigt (Prison Key genommen / John befreit) statt Besucht-Bit: beide Raeume koennen
  VOR dem Lesen schon besucht sein (Roy erst spaeter untersucht) — dieselbe Lage, fuer die Runde 35 K eigene Erreicht-
  Flags eingefuehrt hat (ROOM11C0: Ankunftsszene (4,64); ROOM1150: Latch (9,72)).
* Erneutes Untersuchen: jedes Mal das Tagebuch (die Code-Nachricht kam bei JEDEM Untersuchen, sub07 legt Slot 5 auf
  sich selbst zurueck @0x00E78), aber nur gelesen — RE2s FILE-Liste fuehrt nie ein Dokument doppelt.
* Leerzeile nach dem Datum bleibt (Nutzerzeilen und Leerzeilen sind NUTZER-VORGABE, Regel doc_satz_brief.py); die
  Seitenteilung folgt der RE2-Tagebuchform (jedes Datum beginnt eine Seite, R5).
* Typografischer Apostroph U+2019 (9x im Nutzertext) -> ASCII-Apostroph: die RE2-Dokumentschrift hat genau eine
  Apostroph-Glyphe (gemessen im Atlas); einzige Zeichen-Abbildung.

## Messung nachher (eigener Bau r38/tagebuch, echte exe, Framedump RE15_WINDOW_SCALE=1)

### N1 — erstes Untersuchen (Lauf scratchpad/G/n1, gleiche Karte wie V1)
    [schluessel] ROOM1011: Roy -> sub07 (Zettel 0513) statt sub06 (Prison key)
    [msg] room=1011 id=18 savepoint=0
    [tagebuch] ROOM1011 msg 18 -> Roys Diary (Dokument 5, Bildsatz 30, erstes Mal: ablegen)
    [tagebuch] Leser zu: Bit (9,68)=1, Kartenkette nach dem Evt_end
    [tagebuch] Evt_end erreicht: Kartenkette Eintrag 3 (ROOM10F1 Blatt 3 Rechteck 9) -> Folge 4 (ROOM1211 Blatt 1 Rechteck 3)
    [hint] F535 begin ... [hint] F653 Folge-Hinweis 3 -> 4 (Zeit) ... [hint] F771 Zeit um ... schliessen
  KEINE `[msgtext] room=1011 id=18`-Zeile mehr. Bilder:
  * `G_belege/nachher_1011_tagebuch_leser.png`: Titelseite "ROYS DIARY" auf der FILE16-Illustration (Seitenzaehler
    1/6), Seiten 2/6..6/6, auf 6/6 "...and its code: / 0513." mit 0513 GRUEN, Ende-Stellung (EXIT), Meldung
    "The Roys Diary has been filed.", zurueck im Raum.
  * `G_belege/nachher_1011_hinweiskette.png`: Hinweis-Schirm "POLICE STATION 2F" — Kachel des Communications Room
    blinkt (rot/Umriss), danach "POLICE STATION B2" — die T-Kachel des Detention Area blinkt, dann zu.
  * Pixel (probe_r38_tagebuch_bild): Bild 360 = 87 Pixel in den 6 TEX.TIM-Gruentoenen, alle im Code-Kasten x 36..67
    y 110..125 (Seite bei (25,30) + Satz x 11..42 y 80..95), 0 ausserhalb. Blinkwechsel 2F-Kachel: 1480 Pixel, Huelle
    (157,77)-(196,113) im Kasten Blatt 3/Rechteck 9 = (156,76)-(203,115), erster bei Bild 560; B2-Kachel: 776 Pixel,
    Huelle (188,71)-(225,128) im Kasten Blatt 1/Rechteck 3 = (187,70)-(250,149), erster bei Bild 675.
### N2 — zweites Untersuchen + FILE-Liste (Lauf n2)
    [tagebuch] ROOM1011 msg 18 -> Roys Diary (Dokument 5, Bildsatz 30, schon abgelegt: nur lesen)
    [tagebuch] Leser zu: Bit (9,68)=1, kein Hinweis (schon abgelegt)
  `G_belege/nachher_zweites_untersuchen_nur_lesen.png` (Titelseite, Seite 2/6, KREUZ -> direkt zurueck, keine Meldung,
  kein Hinweis); `G_belege/nachher_files_liste.png`: Files -> "Roys Diary" (Akten wie in RE2).
### N3 — normale Karte danach (Lauf n3: Statusschirm -> MAP, HOCH, RUNTER, RUNTER)
  `G_belege/nachher_normalkarte_2F_B2.png`: 1F (aktueller Raum), HOCH -> 2F mit blinkender Comms-Kachel, RUNTER ->
  1F, RUNTER -> B2 (B1 unbekannt, uebersprungen) mit blinkender Detention-Kachel — Leons Form (Runde 33/35).

## Tests

Neu (probes/r38_tagebuch.cmake; tests/unit/test_r38_tagebuch.c, tests/unit/probe_r38_tagebuch_bild.c,
tests/integration/test_r38_tagebuch.cmake):
* `unit_r38_tagebuch_dokument` (21 Pruefungen): Tabelle, FILE30-Bytes (FNV) == Satz, Papier == FILE16, 0513 gruen
  ueber den echten Seitenlader (87 Pixel, nur im Kasten, keine anderen gruenen Pixel).
* `unit_r38_tagebuch_ablauf` (18): echte VM + Statusschirm: sub07 parkt an @0x00E64, keine Nachricht, Leser
  (Bildsatz 30, 6 Seiten, Liste Platz 0 = Dok 5 VOR dem Lesen), RECHTS x6 -> Ende, "filed" Id 0x4D, Bit 68, Kette ERST
  im Bild des Evt_end (@0x00E76 zu Bildbeginn, danach kein Faden), Slot 5 -> Ereignis 7, Hinweis 1 Blatt 3/9, Hinweis
  2 Blatt 1/3, zu; zweites Untersuchen: Tagebuch, Liste bleibt 1, keine Meldung, kein Hinweis.
* `unit_r38_tagebuch_karte` (14): Kette/Folge/Zeitsteuerung, Ziele + Blattfreigabe, Erreicht-Regeln, Leon unveraendert.
* `unit_r38_tagebuch_leon` (2): ROOM1010 gleicher Druck -> nichts; Haken nur ROOM1011/msg 18.
* `integration_r38_tagebuch_lesen` / `_karte` / `_leon`: N1/N3-Messungen und Leon-Gegenprobe an der echten exe.
* `unit_r36_kartenbits` (k_port[] + Bit 68).

### Gezielte Regression (keine volle Suite; Regex = eigene Tests + jedes beruehrte Modul/jeder beruehrte Raum)
Beruehrt: scd_vm.c (Message_on), menu_common.c (Dokument-Aufnahme), map_hint_common.c (Hinweistabelle), re15_files.c
(Dokumenttabelle), game_step_common.c (Hinweis-Annahme), ROOM1011/Roy, Bank-9-Riegel. Regex:
`r38_tagebuch|r30_irons|r30_hinweis|r33_karte|r34n_e_dokumente|r35_inhalt_doku|r35_cut10f0|r36_kartenbits|r36_karte_|r36_schluessel|r36_elza_roy|r37_roy|unit_savedata|unit_msg_dialog|unit_action_msg_gate`
* Lauf 1 (Bau vor dem Alt-Riegel-Abgleich): `98% tests passed, 1 tests failed out of 60` — rot NUR
  `unit_r36_schluessel_zelle` Pruefung R: sie erwartete "Roy untersuchen -> msg 18 (Zettel 0513)". Das ist genau das
  Verhalten, das der Nutzer jetzt ersetzt haben will ("Das Tagebuch soll kommen statt der Code Message") — ALT-RIEGEL
  WIDERLEGT, nicht Regression. Abgleich (test_r36_schluessel.c teil_zelle, Kommentar "Runde 38 Spur G"): der Beleg der
  Runde 36 bleibt unveraendert (sub07 statt sub06, keine msg 19/20, (4,0xEC) bleibt 0); statt "msg 18 offen" prueft
  er jetzt "Faden parkt an @0x00E64, Roys Diary angefordert, keine Nachricht", danach baut er den Statusschirm ab.
  Leons Hinweise (unit_r30_hinweis_*, unit_r33_karte_*, unit_r35_cut10f0_* + integration_r35_cut10f0), die Dokumente
  (unit_r34n_e_dokumente, integration_r34n_e_dokumente_bild, unit_r30_irons_*, unit_r35_inhalt_doku), Roy
  (unit/integration_r37_roy_*, unit_r36_elza_roy) und unit_r36_kartenbits: alle gruen.
* Lauf 2 (Endstand, Bau mit allen Commits dieser Spur, 2026-10-16):
  `ctest --test-dir re15_port/build -R "r38_tagebuch|r30_irons|r30_hinweis|r33_karte|r34n_e_dokumente|r35_inhalt_doku|r35_cut10f0|r36_kartenbits|r36_karte_|r36_schluessel|r36_elza_roy|r37_roy|unit_savedata|unit_msg_dialog|unit_action_msg_gate" --timeout 600`
  -> **`100% tests passed, 0 tests failed out of 60`** (Total Test time (real) = 976.72 sec).
* Satzwerkzeug reproduzierbar: zweiter Lauf `tagebuch_satz.py --out build/r38_g/satz2` = dieselben 8 md5 wie die
  installierten FILE30-Dateien; Atlas md5 8cbe2c52 = Runde 34.

## OFFEN
- Nichts aus dem Auftrag offen. Hinweise fuer die Integration:
  * map_hint_common.c: meine zwei Eintraege stehen an Platz 3/4, Folge-Index 4 ist positionsabhaengig. Haengt eine
    andere Spur Eintraege davor ein, muss der Index mit — `unit_r38_tagebuch_karte` prueft folge(k1) == k2 und faengt das.
  * Spur K (Prison Key an der Pinnwand-Stelle): "erreicht" von Ziel 1 liest RE15_R36Q_PRISON_BIT (9,83); behaelt K das
    Aufnahme-Bit, passt es von selbst. Inhaltlich passt das Tagebuch ("locked it up in the Communications Room").
  * Spur F (Schluessel an Roys Liegestelle NACH der 1021-Szene): anderes Ereignis/andere Phase; mein Haken greift nur an
    msg 18 in sub07, und sub07 gibt es nur, solange (4,0xFA)==1 (Roy liegt in 1011).
- Die Kette schaltet wie Leons Kette der Runde 35 K nach je 3 Blinkperioden (~3,9 s) oder mit START weiter; Leons
  ERSTER Comms-Hinweis (Runde 30/33) wartet dagegen auf START. Fuer zwei Ziele nacheinander gibt es im Port nur die
  Kettenform (RE2 kennt je Opcode 0x84 nur EIN Ziel, @0x80059210) — so gebaut, im Dossier begruendet.

## Fuer den Nutzer
- NEUE ASSETS (Paket-Gate): `re15_port/shared_assets/RE2/FILES/FILE30_title_page.TIM`, `FILE30_p00_page.TIM` ..
  `FILE30_p05_page.TIM`, `FILE30_title_paper.TIM` (8 Dateien; Bau: `C:/Python310/python.exe
  re15_port/tools/r38_g/tagebuch_satz.py --installieren`). Keine Sprachdateien.
- Bedienung: Roy nach dem Dialog untersuchen -> Tagebuch (RECHTS blaettert, VIERECK/KREUZ schliesst) -> "The Roys
  Diary has been filed." -> Elza richtet sich auf -> Karte: 2F (Communications Room), dann B2 (Prison) je ca. 4 s
  (START schaltet weiter). Danach blinken beide in der normalen Karte, bis der Prison Key genommen bzw. John befreit ist.
- Schreibweise: Titel/Listenname "Roys Diary" woertlich wie im Auftrag (ohne Apostroph); im Text wurden die
  typografischen Apostrophe zum Apostroph der RE2-Schrift.
