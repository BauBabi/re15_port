# Spur G2 — ROOM1150 Irons' Buero: blinkende Schrift im Hintergrund

Stufe: ERMITTLUNG + BAUPLAN + BAU. Zweig r34n/schrift1170, Baum .claude/worktrees/r34n_schrift.
Status: Ermittlung abgeschlossen (Original gemessen, Port gemessen, Mechanismus disassembliert);
Bauplan Abschnitt 5, Abnahmeplan 6; **Bau umgesetzt und an der echten exe abgenommen: Abschnitt 9**
(Abschnitte 4, 5.2, 5.4, 6.5, 7.2 nach der Gegenpruefung berichtigt). Stand 2026-09-30 ~10:20.

## 0 Kurzfassung

* **Was blinkt:** die rote Leuchtschrift **"HEAVEN"** eines Gebaeudes draussen, gesehen durch die
  Jalousien hinter Irons' Schreibtisch — nur in **Cut 2** (Kamera auf den Schreibtisch), Bildbereich
  x147..211 y25..40 (558 Bildpunkte). Gleich in ROOM1151 (Elza).
* **Mechanismus im Original (disassembliert):** das Raumskript **sub05** (von sub00 gestartet,
  Endlosschleife) schaltet mit SCD-Opcode **0x45** (`Col_chg_set`, Handler @0x800428d4 ->
  FUN_800396a8) die **sprite.pri-Maskengruppen 6..11** von Cut 2 im Wechsel **aus** (20 Takte) und
  **ein** (20 Takte). Die sechs Masken sind die sechs Buchstaben, gemalt als *unbeleuchtete*
  Jalousie-Lamellen; der BSS-Hintergrund darunter zeigt die Lamellen *rot beleuchtet*. Maske an =
  Buchstaben grau auf Rot, Maske aus = gleichmaessiges Rotlicht.
* **Takt (gemessen, 6 Savestates desselben deterministischen Laufs):** 1 SCD-Takt = 2 VBlanks
  (VSync-Modus DAT_800b5456 = 2), Umschaltpunkte auf dem Raster Vcount 7052/7053 + 40·k (±1 VBlank
  Phasenlage des Speicherpunkts) — **40 VBlanks je Zustand = 20 Spielbilder, Periode 80 VBlanks**
  (bei 59,83 Hz 0,669 s an / 0,669 s aus). RAM-Zustand
  (Record-Byte0), SCD-Faden (pc/Sleep-Rest) und **beide Bildspeicher** stimmen in allen 6 Staenden
  ueberein (Maskenpixel bitgleich 558/558 bzw. 13/558).
* **Port (gemessen):** die SCD-VM fuehrt sub05 im richtigen Takt aus (RE15_SCD_TRACE: 0x45 bei F25,
  F45, F65 …), aber Opcode 0x45 ist `op_unknown` (nur Laenge 3) — keine Wirkung. Der Renderer
  zeichnet **alle** Masken jedes Bild: die Schrift steht dauerhaft im Masken-Zustand (41 Framedumps
  F300..F500, alle bitgleich zur AN-Referenz, max |d| 0).
* **Bau noetig:** ja. Opcode 0x45 + ein Sichtbarkeitsbit je Maske (Byte0) + Gruppenindex je Maske
  (Byte1) + Neuaufbau "alle an" beim Cut-Wechsel + Renderer ueberspringt Masken mit Bit 0. Kein neuer
  Taktgeber: der Takt kommt aus dem Skript (Sleep 20) und der vorhandenen 30-Hz-SCD-Tick-Basis.
* ⛔ **Allgemeine Korrektur:** Opcode 0x45 steht in **10 Raeumen / 120 Stellen**
  (ROOM1150/1151, 1211, 3000/3001, 3010/3011, 3071, 5060/5061). In den anderen Raeumen blendet er
  per-Bild (sub01) Maskengruppen je Cut aus bzw. spielt in ROOM3071 eine Lichtfolge — der Port zeichnet
  dort heute Masken, die das Original nicht zeichnet (Abschnitt 7.1).

## 1 Nutzerwortlaut + Lesart

Wortlaut (AUFTRAG.md, letzter Punkt + Korrektur): *"In ROOM 1170 blinkt im Hintergrund die Schrift des
Gebäudes. Bei uns nicht."* — korrigiert: *"Nein, nicht beim Heliport, sondern in Irons Office room
1150 blinkt die Schrift eigentlich im Hintergrund."*

**Zensus der Schriften in ROOM1150.** Alle 9 Hintergruende (ROOM115.BSS = 0x90000 Byte, Cut n =
Datei[n*0x10000]) mit den Engine-Funktionen des Ports dekodiert (`probe_r34n_g_bss`, Bild
`G_belege/G2_01_bss_room115_alle_cuts.jpg`). Schrift im Hintergrund gibt es in genau einem Cut:

| Kandidat | Cut | Lage (320x240) | Beleg |
|---|---|---|---|
| rote Leuchtschrift **"HEAVEN"** draussen hinter den Jalousien (drittes Fensterfeld hinter Irons' Schreibtisch) | 2 | x147..211 y25..40 | `G2_02`, `G2_03`, `G2_05` |
| Fenster mit Jalousien, ohne Schrift | 0, 1, 3 | — | `G2_01` |
| Item-Kiste "ITEM LIST / NO DATA" (Bildschirmgrafik, keine Gebaeudeschrift) | 8 | ganzes Bild | `G2_01` |

**Gewaehlte Lesart:** die Leuchtschrift "HEAVEN" in Cut 2. Begruendung: einzige Schrift, die im
Hintergrund liegt (Gebaeude gegenueber, durch das Fenster), und **genau diese Stelle** schaltet das
Original periodisch um — im laufenden Original gemessen (Abschnitt 3.5). Es ist der einzige
Blink-Mechanismus in ROOM1150/1151 (Opcode-Zensus 3.4: sub05 ist die einzige 0x45-Stelle dort; ESP-
Effekt siehe 3.6). "blinkt" passt woertlich: zwei Zustaende im festen Wechsel.
ROOM1151 (Elza) ist byte-gleich an allen beteiligten Stellen: sprite.pri Cut 2 (0x66C..0x8D0),
Kameratabelle, sub05 (1150 @0x10B6 == 1151 @0x1094, 52 Byte), sub00 (`04 ff 18 02 04 ff 18 05 01 00`),
gleiche BSS.

## 2 Ist-Zustand im Port (gemessen)

**Code-Pfad.**
* `scd_vm.c`: `s_opcode_sizes[0x45] = 3`, `s_op_table[0x45] = op_unknown` (Zeile 248: alle nicht
  zugeordneten Opcodes) — `op_unknown` rueckt nur `pc += 3` vor und laeuft weiter.
* `pri_common.c re15_pri_parse_section`: flacht Gruppen zu einer Maskenliste ab, **ohne**
  Gruppenindex und **ohne** Sichtbarkeitsbit (`re15_pri_mask_t` = src/dst/depth/w/h).
* `main.c` (~5741): beim (Raum,Cut)-Wechsel `re15_render_pc_set_pri_rects(..., pri.draw_count)`.
* `render_pc.c` (~984-1107): `mask_n = s_pri_rect_count` — **jede** Maske wird jedes Bild geblittet.

**Messung 1 — echte exe, Debug-Sprung, Cut erzwungen** (`re15_pc_g2.exe` = Kopie der exe dieses
Baums, Bau-Stand cf0e68ba + nur Sonden/Dokumente seither, `git diff --stat cf0e68ba..HEAD -- engine
platform include` leer):
`RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2 RE15_DEBUG_JUMP=1150@240
RE15_WINDOW_SCALE=3 RE15_FORCE_CUT=2 RE15_CUT_SYNC_LOG=cutsync.log RE15_FRAMEDUMP="300-500/5:f"
RE15_EXIT_AT="500#1150"`. debug.log: `[pri] cut=2 pri_offset=0x66C masks=54 fg_atlas=1`.
Auswertung `port_schrift1150_eval.py`: **41/41 Dumps F300..F500 = AN-Referenz, max |d| 0** im
Rechteck x139..216 y25..40 (Referenz AUS: max |d| 195). Die Schrift blinkt nicht; sie steht dauernd im
Masken-Zustand ("HEAVEN" grau auf Rot). Bild `G2_07`.

**Messung 2 — SCD-Spur derselben exe** (`RE15_SCD_TRACE=1`, `RE15_DEBUG_JUMP=1150@240`,
`RE15_EXIT_AT="200#1150"`): sub05 laeuft im richtigen Takt — `@0x10B6..0x10C5 op=0x45` (:=0) bei
F45, F85, F125, F165, `@0x10CC..0x10DB op=0x45` (:=1) bei F25, F65, F105, F145, jeweils gefolgt von
`op=0x09` (Sleep) und 20 Bilder spaeter `op=0x17` (Goto). **Der Takt stimmt also bereits; es fehlt
allein die Wirkung des Opcodes.** Auszug `G2_08`.

**Messung 3 — Lade-Weg (Spielstand, Raumlader wie im Spiel, kein Debug-Sprung).** Eigene Karte
`probe_r34n_g_karte` (Spielstand in ROOM1150 bzw. ROOM1151, Spieler (-22250,0,-18500) im Regionsviereck
von Cut 2, `camera_cut` = 2), dann `RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_CONTINUE_TEST=1 RE15_CARD_AUTO=1
RE15_CARD_SLOT=0 RE15_WINDOW_SCALE=3 RE15_CUT_SYNC_LOG=cutsync.log RE15_FRAMEDUMP="100-300/2:f"
RE15_EXIT_AT="300#<raum>"`. debug.log: `[save] CONTINUE: resumed in room 1150` bzw. `1151`,
`[pri] cut=2 pri_offset=0x66C masks=54 fg_atlas=1`. Ergebnis je Raum: **101/101 Dumps F100..F300 = AN,
max |d| 0**, Lebendnachweis 100/100 Dumps mit Aenderung ausserhalb des Rechtecks (Leon steht im Bild,
Leerlauf-Animation). Bild `G2_09`. Ein echter Durchlauf Titel -> ... -> ROOM1150 per Eingabeskript ist
nicht gefahren (Raumkette zu lang); der Lade-Weg benutzt denselben Raumlader wie Tuer/LOAD und
schliesst den Debug-Sprung als Ursache aus. Der Befund ist ohnehin strukturell (Opcode fehlt, Renderer
kennt kein Sichtbarkeitsbit).

⛔ Nebenbefund Werkzeug: `tools/maske/original.py atlas()` (Python-SLD-Entpacker) liefert fuer
ROOM1150 Cut 2 einen Atlas, der in **11978 von 65536 Texeln** vom Port-Entpacker abweicht; der
Port-Entpacker (`probe_r34n_g_sld` = `re15_sld_used_len` + `re15_sld_atlas_from_chunk`) ist bytegleich zu
`BSS/ROOM1150/PRI02.TIM`. Die ersten Zahlen dieses Dossiers (08:35: "671 Pixel") stammten aus dem
Python-Atlas und sind ersetzt; alle Werkzeuge der Spur lesen jetzt die TIM-Datei (Abschnitt 8).

## 3 Original-/RE2-Mechanismus (Adressen, Bytes, Instruktionen)

### 3.1 Die Schleife im Raumskript (ROOM1150.RDT, Datei-Offsets)

`scd_dump_room.py`, ROOM1150 sub05 (Region 0x10B6..0x10EA), gestartet von sub00
(`Evt_exec 04 ff 18 05` @0x00EB6):

    0x010B6  45 05 00   Col_chg_set  Gruppe 6  := 0
    0x010B9  45 06 00   Col_chg_set  Gruppe 7  := 0
    0x010BC  45 07 00   Col_chg_set  Gruppe 8  := 0
    0x010BF  45 08 00   Col_chg_set  Gruppe 9  := 0
    0x010C2  45 09 00   Col_chg_set  Gruppe 10 := 0
    0x010C5  45 0a 00   Col_chg_set  Gruppe 11 := 0
    0x010C8  09 0a 14 00  Sleep 20
    0x010CC  45 05 01 ... 0x010DB 45 0a 01   Gruppen 6..11 := 1
    0x010DE  09 0a 14 00  Sleep 20
    0x010E2  17 ff ff 00 d4 ff   Goto -0x2C -> 0x010B6 (Endlosschleife)

ROOM1151: dieselben 52 Byte @0x01094..0x010C7 (sub05, gestartet von sub00 @0x00E94).

### 3.2 Opcode 0x45 im Original (PSX.EXE, selbst disassembliert mit re15_disasm.py)

Tabelleneintrag: `table 0x800745bc` (= 0x800744a8 + 0x45·4) -> **0x800428d4**.

    800428e4: lw  v0,28(s0)          ; pc
    800428ec: lbu a0,1(v0)           ; op1
    800428f0: lbu a1,2(v0)           ; op2 (neuer Wert)
    800428f4: jal 0x800396a8
    800428f8: addiu a0,a0,1          ; a0 = op1+1
    80042900: ori v0,zero,0x1        ; Rueckgabe 1 = weiter im selben Takt
    80042904: addiu v1,v1,3          ; pc += 3

FUN_800396a8 (einziger Aufrufer: `jal` @0x800428f4 — jal-Wort-Zensus der EXE):

    800396b0: lw  v0,-14472(v0)      ; DAT_800ac778 = RDT im RAM
    800396b8: lbu a3,0(v0)           ; a3 = RDT[0] = Maskenzahl des AKTUELLEN Cuts
    800396c0: lw  v1,9604(v1)        ; DAT_800b2584 = Record-Tabelle, 4 Byte je Maske
    800396c4: beq a3,zero,0x800396f0
    800396d0: lbu v0,1(v1)           ; Record-Byte1
    800396d8: bne v0,a0,0x800396e4
    800396e0: sb  a1,0(v1)           ; Byte1 == op1+1 -> Record-Byte0 := op2
    800396e8: bne v0,zero,0x800396d0 ; fuer alle a3 Records
    800396ec: addiu v1,v1,4

Aufbau und Verbrauch der Record-Tabelle (Decompilate `RE_15_Quellcode_V2/FUN_80039270.c`,
`FUN_800392d4.c`, `FUN_80039590.c`; Aufrufstellen per jal-Zensus):
* FUN_80039270 (Raumladen): DAT_800b2584 = Pool-Zeiger, Platz fuer RDT[7] Records.
* FUN_800392d4 (einziger Aufruf @0x80021c28 in FUN_80021bbc = Cut anwenden): alle RDT[7] Records
  Byte0 := 0; dann je gebauter Maske **Byte0 |= 1**, **Byte1 := Gruppenindex+1**, Halbwort +2 :=
  Tiefe; RDT[0] := Maskenzahl (Kopf-Halbwort hoch). -> **jeder Cut-Wechsel schaltet alle Masken ein.**
* FUN_80039590 (einziger Aufruf @0x8001ce54, jedes Bild, gegatet durch DAT_800aca38 & 0x100000
  @0x8001ce3c-4c): fuer Record i < RDT[0] **nur wenn Byte0 & 1**: SetSprt/SetDrawMode(TPage 0x95) +
  AddPrim in die OT bei Tiefe (Halbwort +2).
* Reihenfolge im Bild (FUN_8001cce0): SCD-Laeufer `jal 0x8003f038` @0x8001cdec **vor** dem
  Maskenzeichnen @0x8001ce54 -> ein Umschalten wirkt noch im selben Bild.
* Der SCD-Laeufer FUN_8003f038 wird uebersprungen, solange g_pauseflags & 0x02000000
  (@0x8003f040-4c) — waehrend Pausen steht das Blinken (Maskenzustand bleibt, wie er ist).

=> Opcode 0x45 `Col_chg_set g v` setzt das Sichtbarkeitsbit aller Masken mit Byte1 == g+1
(= Gruppe g+1, 1-basiert) im aktuell gezeigten Cut.

### 3.3 Was die Gruppen 6..11 in ROOM1150 sind

sprite.pri von Cut 2 (@0x0066C, Kopf `0B 00 36 00` = 11 Gruppen, 54 Masken). Die Gruppen 6..11 haben
je genau eine Maske (`schrift1150_masken.py`, Atlas = Port-Entpacker):

| Gruppe | Record (Datei) | Atlas src | Bild dst | Groesse | Tiefe | deckende Texel |
|---|---|---|---|---|---|---|
| 6 | 0x0089C | (168,80) | (139,25) | 16x16 | 0 | 70 |
| 7 | 0x008A4 | (184,80) | (160,25) | 8x16 | 0 | 92 |
| 8 | 0x008B0 | (192,80) | (167,25) | 16x16 | 0 | 90 |
| 9 | 0x008B8 | (208,80) | (176,25) | 16x16 | 0 | 87 |
| 10 | 0x008C0 | (224,80) | (190,25) | 16x16 | 0 | 110 |
| 11 | 0x008C8 | (240,80) | (201,25) | 16x16 | 0 | 109 |

Die deckenden Texel bilden die Buchstaben **H E A V E N** (`G2_03`, untere Zeile = Deckungsmaske),
gemalt als Lamellen **ohne** Rotlicht; der BSS-Hintergrund zeigt an derselben Stelle Lamellen **mit**
rotem Leuchten. Masken gezeichnet (Byte0 = 1) = Buchstaben grau auf Rot; ausgeschaltet (Byte0 = 0) =
gleichmaessiges Rotlicht. Unterschied: 558 Bildpunkte, Rechteck x147..211 y25..40, mittlere Helligkeit
im Rechteck R/G/B 135,5/128,1/126,3 (aus) gegen 114,6/113,1/111,2 (an) (`G2_02`). In keinem anderen
Cut von ROOM1150/1151 gibt es eine Gruppe >= 6 (`G2_04`): der Opcode wirkt nur, solange Cut 2 steht.

### 3.4 Zensus Opcode 0x45 ueber alle 240 RDTs

`col_chg_zensus.py` (`G2_04`): **10 Raeume, 120 Stellen.** Einzeln in Abschnitt 7.1.

### 3.5 Dynamische Messung am Original (DuckStation, MZD-Disk = `info/Re1.5`, Software 1x)

Weg (ohne Pad-Eingabe, Einstellungen des Nutzers unveraendert, Werkzeuge `ds_lauf.py`,
`re15_ss_patch.py`, `ss_schrift1150.py`, Protokoll `G2_06`):
1. Die Pad-Bindungen in `settings.ini` sind Akkorde (`Down = Keyboard/DownArrow & SDL-0/DPadDown`),
   ein virtueller Pad allein wirkt nicht (gemessen 08:42: 15x Links + Quadrat, Menue blieb auf
   "JUMP 124 OPENING"). Deshalb RAM-Patch wie der Quadrat-Zweig des Debug-Menues @0x80014a44-58:
   DAT_800bbe5f[0] := 0x15, DAT_800b5359 := 1, DAT_800ac9a8 := 0, DAT_800bbe5c := 0.
   FUN_8001d600 laedt bei DAT_800ac9a8 == 0 den Raum aus dem Menue-Cursor (Decompilat Z. 6-24).
   Ergebnis: sauberer Stand ROOM1150 (Raum 0x15, Stub @0x80026e4c = 0x03e00008), Cut 0, Vcount 7002.
2. Cut 2 erzwungen: DAT_800aca3c := 0x100 (Zonen-Scan aus), DAT_800afbb5 := 2 -> der Present-Pfad
   ruft FUN_80021bbc (Cut anwenden, darin FUN_800392d4).
3. Sechs Laeufe verschiedener Dauer ab DEMSELBEN Stand (Emulation ohne Eingabe = deterministisch),
   je Endstand (SaveStateOnExit) ausgewertet. resume.sav/_1.sav/_1.bak vorher gesichert, nachher
   zurueckgelegt, SHA-256 geprueft.

| Stand | Vcount | sub05-Faden (Pool 0x800b2b4c, Faden 3 @0x800b2f9c) | Byte0 Gr. 6..11 | Bildspeicher y=0 / y=240 |
|---|---|---|---|---|
| c2_w9 | 7050 | pc = RDT+0x10E2 (Sleep#2 fertig, naechster Takt Goto + :=0) | 1 | AN, 558/558 Maskenpixel bitgleich |
| c2_w10.5 | 7499 | Sleep#2 (nach :=1), Rest 16 | 1 | AN, 558/558 |
| c2_w11.2 | 7567 | Sleep#1 (nach :=0), Rest 2 | 0 | AUS, 13/558 |
| c2_w12.7 | 7600 | Sleep#2, Rest 5 | 1 | AN, 558/558 |
| c2_w12 | 7647 | Sleep#1, Rest 2 | 0 | AUS, 13/558 |
| c2_w13.5 | 7697 | Sleep#1, Rest 17 | 0 | AUS, 13/558 |

**Takt-Rekonstruktion.** Ein Sleep(20) laeuft 20 Takte; im Takt des Umschaltens steht der Rest schon
auf 19 (Sleep setzt, Sleeping zaehlt im selben Takt: 0x09 gibt 1 zurueck @0x8003f424, 0x0A gibt 2
zurueck @0x8003f48c). Rest r => Umschalten liegt (19 - r) Takte zurueck. Mit 2 VBlanks je Takt:
7499 - 2·3 = 7493 (:=1), 7567 - 2·17 = 7533 (:=0), 7600 - 2·14 = 7572 (:=1), 7647 - 2·17 = 7613 (:=0),
7697 - 2·2 = 7693 (:=0) — alle auf dem Raster **7052/7053 + 40·k** (je ±1 VBlank Phasenlage des
Speicherpunkts). Damit gemessen: **2 VBlanks je SCD-Takt (VSync-Modus 2 in allen Staenden), 20 Takte
= 40 VBlanks je Zustand, Periode 80 VBlanks.** Der erste Abschnitt ab Vcount 7002 ist laenger (Cut-
Wechsel mit MDEC-Dekodierung und einem ausgelassenen Bild @0x80021558-60), danach kein Bildausfall.
Bilder beider Zustaende aus dem Original-Bildspeicher: `G2_05`.

### 3.6 Andere Kandidaten ausgeschlossen

* **Raum-ESP (RDT+0x4C = 0x14460):** eine Effekt-Id 0x01 (`01 ff ff ff ff ff ff ff`), TIM @0x1E520
  4 bpp. Er ist nicht die Schrift: die Schrift-Umschaltung ist vollstaendig durch Masken-Byte0 erklaert
  (Bildspeicher bitgleich AN bzw. AUS in allen 6 Staenden — ein zusaetzliches Sprite im Rechteck
  haette die 558/558 bzw. 13/558 gestoert). Wer Effekt 0x01 startet, ist nicht Teil dieses Punkts.
* **SCD:** in ROOM1150/1151 ist sub05 die einzige periodische Schleife auf Bildinhalt (sub04 = Hebetisch,
  sub03/sub08 = Zwischenszenen, sub06/07 = Nahansichten mit Nachricht).
* **Hintergrund-Upload / Licht:** wie in ROOM1170 (Dossier G, 3.1): der Hintergrund wird je Bild
  unveraendert aus 0x80198000 hochgeladen; kein anderer Weg aendert ihn.

### 3.7 RE2 (Retail) zum Vergleich

RE2s Maskenzeichner `RE2_Quellcode_V2/FUN_80049ca8.c` hat dieselbe Record-Form (4 Byte, Zahl aus
`*DAT_800ce324`, Tiefe im Halbwort +2, SetSprt + SetDrawMode(...,0x95) + AddPrim) und dieselbe
Bedingung **`(*pbVar5 & 1) != 0`** — plus eine zweite Pruefung `FUN_80077360(&DAT_800d4928 + ..., byte)`
(Flag-Test je Record). RE2 behaelt also das Sichtbarkeitsbit, das Opcode 0x45 schaltet. Einordnung nach
reai-v2-beta-zu-retail: RE1.5 hat das System **vollstaendig** (Daten in 10 Raeumen, Opcode, Aufbau,
Zeichnen) — RE1.5 ist massgeblich, RE2 bestaetigt nur die Architektur. Kein RE2-Nachbau noetig.

## 4 Soll-Verhalten (Zeitlinie)

Takt = SCD-Takt des Spiels (Original 2 VBlanks = VSync(2); Port 1 Bild der 30-Hz-Schleife =
1 SCD-Takt, main.c: `frame_budget_ms = 1000 / 30`, SCD je Bild bei target_fps 30).

* Raum betreten: sub00 startet sub05 (Evt_exec). Erster Takt von sub05: Gruppen 6..11 := 0, dann
  20 Takte Pause, := 1, 20 Takte, Goto, := 0, ... unabhaengig vom gezeigten Cut.
* Steht Cut 2: Takt t0 (:=0) -> ab diesem Bild keine Buchstaben-Masken, Rotlicht (BSS); Takt t0+20
  (:=1) -> ab diesem Bild Buchstaben grau auf Rot; Takt t0+40 (:=0) ... Umschalten wirkt im selben Bild
  (SCD @0x8001cdec vor Masken @0x8001ce54).
* Wechsel IN Cut 2: FUN_800392d4 schaltet alle Masken ein -> Buchstaben sichtbar bis zum naechsten
  :=0 von sub05 (0..40 Takte, je nach Phase). Wechsel aus Cut 2 heraus und zurueck setzt das jedes Mal.
* In anderen Cuts: 0x45 findet keine Records mit Byte1 6..11 -> keine Wirkung.
* Pause (g_pauseflags & 0x02000000, z.B. Item-Modal): kein SCD-Takt -> Zustand steht.
* Zwischenszenen sub03/sub04/sub08 (eigene Faeden) laufen parallel; sub05 laeuft weiter.
* ⛔ BERICHTIGT (Gegenpruefung Auflage 1, im Bau nachgemessen §9.5): Nach dem Statusschirm
  (Inventar) setzt das Original DAT_800b5457 := **2** (@0x800466fc, Wert @0x800466dc), nach dem
  Speicherkarten-Schirm ebenfalls := 2 (@0x80026634, Wert @0x8002661c). FUN_80021bbc springt bei
  Dirty == 2 (@0x80021bc4 `lbu v1,21591(v1)` / @0x80021bc8 `ori v0,zero,0x2` / @0x80021bd4
  `beq v1,v0,0x80021df8`) UEBER den Aufbau @0x80021c28 und die Cut-Schreiber @0x80021bf4/@0x80021bfc:
  **kein Neuaufbau, Zustand bleibt stehen, work_vars[0x0C] unveraendert**; da der SCD-Laeufer im
  Menue steht (@0x8003f040-4c), laeuft der Sleep-Rest danach weiter. Der Optionsschirm (Dirty := 1
  @0x8002e730, nur bei DAT_800aca38 & 0x40000000) ist im Spiel nur ueber SELECT+START erreichbar
  (@0x8001cd24-60 -> Transitions-FSM @0x8001cb08-30 startet Task 0x8002dde4 STATT des Statusschirms)
  — dieser Weg ist im Port nicht portiert (menu_common.c:179/2395), siehe §9.1 Auflage 2.
  Cut_chg/Cut_old (Opcode 0x29/0x2a) setzen den Dirty-Schalter := 1 (@0x800402f4/@0x80040354) und
  bauen damit neu auf.

## 5 Bauplan

Einordnung: RE1.5 hat das System vollstaendig (Daten, Opcode, Aufbau, Zeichnen) -> **RE1.5 byte-true**,
keine RE2-Angleichung, keine Nutzer-Vorgabe noetig. Kein neuer Taktgeber: der Takt ist Skript-Daten
(`Sleep 20` in sub05) auf der vorhandenen SCD-Tick-Basis des Ports (1 SCD-Takt je Bild der
30-Hz-Schleife = VSync(2)-Aequivalent; RE15_SCD_TRACE belegt 20 Bilder je Zustand, Abschnitt 2).
Die Runde-30-Fehlerklasse (Zaehler je MONITOR-Bild) tritt hier nicht auf.

### 5.1 Neue Dateien (Spur G2)

* `re15_port/include/re15_masken_gruppen.h` + `re15_port/engine/src/masken_gruppen.c` — die
  Record-Tabelle DAT_800b2584 als Engine-Zustand, plattformneutral (PC und PSX):

      #define RE15_MG_MAX 256        /* RDT[7] ist u8; Aufbau-Schleife @0x80039370-84 */
      typedef struct { uint8_t an; uint8_t gruppe1; } re15_mg_rec_t;   /* Byte0 / Byte1 */
      /* FUN_800392d4 (@0x80021c28 aus FUN_80021bbc): Tabelle fuer (rdt, cut) neu aufbauen.
       *  - NULL-Sektion (erstes u32 == 0xFFFFFFFF @0x80039324-2c): Zahl := 0 (@0x80039338)
       *  - Zahl (RDT[0]) := (erstes u32 >> 16) & 0xFF (@0x80039330 srl 16, @0x80039358 sb)
       *  - Byte0 := 0 fuer RDT[7] Records (@0x8003936c-84), dann je gebauter Maske
       *    Byte0 |= 1 (@0x800393d8-e4), Byte1 := Gruppenindex+1 (@0x800393e8-ec).
       *  Gruppenindex je Maske aus den Gruppenkoepfen (u16 Anzahl je 8-Byte-Kopf), gleiche
       *  Reihenfolge wie re15_pri_parse_section (Leer-Gruppen zaehlen mit). */
      void re15_mg_aufbauen(const re15_rdt_t *rdt, int cut);
      /* FUN_800396a8 (Opcode 0x45): fuer i < Zahl mit gruppe1 == g1 -> an := wert (@0x800396d0-e0). */
      void re15_mg_setzen(uint8_t g1, uint8_t wert);
      /* FUN_80039590 @0x800395e8-f4: gezeichnet nur wenn Byte0 & 1. i >= Zahl -> 1 (Masken ohne
       * Original-Record = nachgezeichnete R15M-Masken; das Original hat dort RDT[0] = 0). */
      int  re15_mg_sichtbar(int i);
      int  re15_mg_zahl(void);       /* fuer Log/Pins */

  Log-Zeile (env-gegatet, z.B. `RE15_MG_LOG`): `[maskgrp] F%u Raum %04x Cut %d: Gruppe %d := %d
  (%d Records)` und `[maskgrp] Aufbau Raum %04x Cut %d: %d Records`. Kein Speicherstand-Feld (das
  Original speichert die Tabelle nicht; jeder Raum-/Cut-Aufbau setzt alles auf 1).
* `re15_port/tests/unit/probe_r34n_g_maskgrp.c` + Eintrag in `tests/unit/probes/r34n_g_schrift.cmake`
  mit `add_test(unit_r34n_g_maskgrp ...)` (Pin 6.1) und ein Integrationshaken
  `re15_port/tests/integration/test_r34n_g_schrift1150.cmake` (Abnahme 6.3, echte exe, Lade-Weg-Karte
  `probe_r34n_g_karte`). `RE15_MIN_TESTS` in local_build.sh um die neuen add_test anheben.

### 5.2 Haken-Zeilen in gemeinsamen Dateien (je 1-3 Zeilen, keine Umformatierung)

| Datei / Stelle | Haken | Original-Beleg |
|---|---|---|
| `engine/src/scd_vm.c` Opcode-Tabelle (neben `s_op_table[0x47] = op_aot_on;`, Z. 335) | `s_op_table[0x45] = op_col_chg_set;` + Handler: `re15_mg_setzen(t->pc[1] + 1, t->pc[2]); t->pc += 3; return 1;` | Tabelle @0x800745bc -> 0x800428d4; op1+1 @0x800428f8; pc+3 @0x80042904; Rueckgabe 1 @0x80042900 |
| `platform/pc/main.c` `pc_cam_present_apply`, im Zweig `if (re15_cam_present_tick()) { ... }` | `if (rdt_ok) re15_mg_aufbauen(rdt, active_cut_idx);` | jeder Apply = FUN_80021bbc -> FUN_800392d4 (@0x80021c28), auch bei GLEICHEM Cut (Dirty-Schreiber @0x80021514, @0x800402f4, @0x80040354) |
| `engine/src/room_common.c` Schritt (9), direkt nach `c->load_bg_cut(cut);` (Z. 381) | `re15_mg_aufbauen(c->rdt, cut);` | Raumlader FUN_8001d600 -> `DAT_800b5457 = 1` @0x8001daec -> FUN_80021bbc; SCD-Init (scd_room_reenter) VOR dem Aufbau wie im Original |
| `platform/pc/main.c` Boot/CONTINUE nach `re15_bg_load_cut((int)g_scd.cam_id)` (Z. ~4739) und Sonderweg `re15_bg_load_cut(0)` (Z. ~8026) | `re15_mg_aufbauen(&rdt, <cut>);` | Session-Start/LOAD `DAT_800b5457 = 1` @0x8001d5c8 |
| `platform/pc/src/render_pc.c` Maskenliste (Z. ~984-987, `mask_order[i] = i`) | nur sichtbare Indizes einsortieren: `if (!re15_mg_sichtbar(i)) continue;` (mask_n = Zahl der eingetragenen) | FUN_80039590 @0x800395e8-f4 |
| ~~`engine/src/menu_common.c` `close_phase`~~ | ⛔ ENTFAELLT (Auflage 1): Original schreibt Dirty := 2 | @0x800466fc (Wert @0x800466dc) -> Sprung @0x80021bd4 ueber @0x80021c28 |
| ~~Options-/Kartenschirm-Rueckkehr~~ | ⛔ ENTFAELLT (Auflagen 1/2): Kartenschirm Dirty := 2; Optionsschirm im Spiel im Port nicht portiert | @0x80026634 (Wert @0x8002661c); @0x8002e730 nur ueber SELECT+START @0x8001cd24-60 |
| PSX: `platform/psx/src/render.c` Maskenschleife (Z. ~466-469) | `if (!re15_mg_sichtbar(i)) continue;` | wie oben; PSX-Bau hier nicht pruefbar (memory reai-v2-psx-build-gap) |

Nicht anfassen: `pri_common.c` / `re15_pri.h` (die Gruppenkoepfe liest das neue Modul selbst),
`bg_pc.c`, die sprite.pri-Tiefenlogik, `op_unknown`.

### 5.3 Reihenfolge im Bild (muss so bleiben)

Port: `pc_cam_present_apply` (Bildanfang, Aufbau) -> `scd_vm_tick` (Z. 5378, 0x45 schaltet) ->
pri-Block (Z. ~5680, Rechtecke nur bei (Raum,Cut)-Wechsel) -> `re15_render_end_frame` (liest
`re15_mg_sichtbar`). Original: FUN_80021bbc am Ende des Vorbilds (Present) -> SCD @0x8001cdec ->
Masken @0x8001ce54. Beide: Aufbau VOR dem SCD-Takt, Umschalten wirkt im selben Bild. ⛔ Der Aufbau darf
NICHT in den pri-Block (nach dem SCD-Takt): dort wuerde ein Umschalten im Cut-Wechsel-Bild sofort
wieder ueberschrieben (im Original bleibt es stehen).

Zeichenebene (Skill re15-pc-render-order): die Masken bleiben in Ebene 3 (3D-Dreiecke + sprite.pri-
Overdraw, Tiefen-Mischung in `re15_render_end_frame`); der Bau entfernt nur ausgeschaltete Masken aus
`mask_order`, Reihenfolge und Tiefenschluessel (`re15_pri_mask_camera_z`) bleiben unveraendert. Die sechs
Buchstaben haben Tiefe 0 = kleinster Schluessel = zuletzt gezeichnet (im Original OT-Index 0 der
1024er-OT, AddPrim @FUN_80039590), liegen also ueber allem 3D in Cut 2 — wie heute.

### 5.4 Konstanten-Tabelle

| Wert | Bedeutung | Beleg |
|---|---|---|
| 0x45 / Laenge 3 | Opcode `Col_chg_set`, pc += 3 | Tabelle @0x800745bc -> 0x800428d4; `addiu v1,v1,3` @0x80042904; Port `s_opcode_sizes[0x45] = 3` bleibt |
| Rueckgabe 1 | weiter im selben Takt (kein Yield) | `ori v0,zero,0x1` @0x80042900 |
| op1 + 1 | Gruppenschluessel = Record-Byte1 | `addiu a0,a0,1` @0x800428f8; Vergleich `lbu v0,1(v1)` / `bne v0,a0` @0x800396d0/d8 |
| op2 | neuer Byte0-Wert (0 = aus, 1 = an) | `lbu a1,2(v0)` @0x800428f0; `sb a1,0(v1)` @0x800396e0 |
| Zahl = RDT[0] | Records, die 0x45 und das Zeichnen sehen | `lbu a3,0(v0)` @0x800396b8; `lbu s4,0(v0)` @0x800395c0; gesetzt `srl t2,v1,16` @0x80039330 + `sb t2,0(a0)` @0x80039358 |
| RDT[7] | Records, deren Byte0 beim Aufbau geloescht wird | `lbu a3,7(v0)` @0x8003936c, Schleife @0x80039370-84 |
| Byte0 oder-gleich 1 | beim Cut-Aufbau jede gebaute Maske an | `ori v0,v0,0x1` @0x800393e0, `sb` @0x800393e4 |
| Byte1 = Gruppe + 1 | Gruppenindex+1 | `addiu v0,a3,1` / `sb v0,-1(t1)` @0x800393e8-ec |
| Byte0 & 1 | zeichnen | `andi v0,v0,0x1` / `beq` @0x800395f0-f4 |
| NULL-Sektion | Zahl 0 | `addiu v0,zero,-1` / `bne` / `sb zero,0(a0)` @0x80039328-38 |
| Gruppen 6..11, Cut 2 | die Buchstaben H E A V E N | ROOM1150/1151.RDT sprite.pri @0x0066C, Records @0x0089C..0x008C8 (Daten, nicht im Code) |
| Sleep 20 / Periode 40 Takte | Blinktakt | ROOM1150 @0x010C8 / @0x010DE `09 0a 14 00` (Daten); gemessen 40 VBlanks je Zustand (G2_06) |
| ⛔ KEIN Neuaufbau nach Statusschirm | Dirty := 2 -> FUN_80021bbc springt @0x80021bd4 ueber den Aufbau | `ori v0,zero,0x2` @0x800466dc, `sb` @0x800466fc; @0x80021bc4/c8/d4 |
| ⛔ KEIN Neuaufbau nach Kartenschirm; Optionsschirm (Dirty := 1) im Port nicht vorhanden | Dirty := 2 bzw. 1 | @0x8002661c/@0x80026634; @0x8002e728/30 (Weg SELECT+START @0x8001cd24-60) |

Keine PORT-WAHL und keine NUTZER-VORGABE im Kern. Einzige Port-Konstruktion: `re15_mg_sichtbar(i) = 1`
fuer i >= Zahl (nachgezeichnete R15M-Masken, die es im Original nicht gibt — Grund: das Original hat
dort RDT[0] = 0, der Opcode findet also nichts; die Port-Masken sollen unveraendert bleiben).

## 6 Abnahmeplan

1. **Pin `unit_r34n_g_maskgrp` (ohne exe, echte RDT-Bytes):** ROOM1150.RDT laden, `re15_mg_aufbauen(rdt, 2)`
   -> Zahl 54, Records 48..53 mit gruppe1 6..11, alle an; SCD-Bytes `45 05 00` ueber die echte VM
   (Opcode-Tabelle, nicht direkt die Modulfunktion) -> Record 48 aus, 47 und 49 unveraendert, pc + 3,
   selber Takt; `45 05 01` -> wieder an; Aufbau erneut -> alles an; Cut 0 -> Zahl 2, `45 05 00`
   aendert nichts; NULL-Cut 5 -> Zahl 0, `re15_mg_sichtbar(0) == 1`. Gegenprobe (Mutation): ohne
   `s_op_table[0x45]` faellt der Pin.
2. **Takt-Pin (Raumsonde, ohne exe):** ROOM1150-SCD (main00 + sub00 -> sub05) ueber `scd_vm_tick` 200 Takte
   laufen lassen (Skill re15-room-probe), Sichtbarkeit von Record 48 je Takt aufzeichnen: nach dem
   ersten Aufbau Wechsel **exakt alle 20 Takte**; gleiches fuer ROOM1151.
3. **Echte exe, Lade-Weg (Integrationshaken):** Karte `probe_r34n_g_karte <mcr> 1150 2` -> CONTINUE,
   `RE15_FRAMEDUMP="100-300/1:f"`, `RE15_SCD_TRACE=1`; Auswertung `port_schrift1150_eval.py`:
   AN- und AUS-Laeufe wechseln sich ab, jeder volle Lauf **genau 20 Bilder**, Umschaltbild = Bild der
   0x45-Zeile im Trace; max |d| 0 gegen die jeweilige Referenz. Dasselbe fuer 1151. (Vorher: 101/101 AN.)
4. **Debug-Sprung + Cut-Wechsel:** `RE15_DEBUG_JUMP=1150@240`, in Cut 1 und ueber RVD-Satz 6 (1 -> 2)
   nach Cut 2 laufen (`RE15_PLAYER_POS` + `RE15_INPUT_SCRIPT`): nach dem Eintritt
   in Cut 2 zuerst AN (Aufbau), erstes AUS erst beim naechsten :=0 von sub05. ⛔ BERICHTIGT
   (Auflage 4): NICHT mit `RE15_FORCE_CUT` — der setzt in pc_cam_present_apply JEDES Bild pending,
   baut also jedes Bild neu auf, das Blinken ist darunter unsichtbar.
5. **Statusschirm (BERICHTIGT, Auflage 5):** in Cut 2 waehrend eines AUS- und eines AN-Laufs das
   Inventar oeffnen/schliessen: der Zustand beim Schliessen == Zustand beim Oeffnen, das naechste
   Umschalten kommt nach dem REST des Sleep (SCD stand im Menue), kein Aufbau, work_vars[0x0C]
   unveraendert (Original Dirty := 2 @0x800466fc -> Sprung @0x80021bd4). Gemessen §9.5.
6. **Andere Raeume (7.1), je ein Messlauf mit `RE15_MG_LOG` + `RE15_PRI_LOG` + Framedump:** ROOM3000
   Cut 0 in der Zombie-Variante (Flag (4,9) = 1 -> (5,0) = 0) -> 0 von 43 Masken gezeichnet;
   ROOM3071 sub02 (Elza-Szene) -> Lichtfolge Gruppe 13..3 je 20 Takte in Cut 9; ROOM1211 Cut 7 nach
   Flag (5,2); ROOM5060 Cut 11 nach Flag (5,4). Gegenprobe gegen DuckStation (RAM-Patch-Weg aus 3.5)
   mindestens fuer ROOM3000 Cut 0: Byte0 der 43 Records und beide Bildspeicher.
7. **Gegenprobe Original (liegt vor, G2_06):** 40 VBlanks = 20 SCD-Takte je Zustand; der Port muss
   20 Bilder je Zustand zeigen (1 Bild = 1 SCD-Takt).
8. Suite: volle Suite ueber local_build.sh; Bild-Haken mit echter exe bei Fehlschlag einzeln 2x
   nachfahren (memory reai-v2-gui-tests-flattern-bei-parallelen-agenten).

## 7 Risiken, Softlocks, Wechselwirkungen

### 7.1 Wirkung der allgemeinen Umsetzung auf alle Raeume mit Opcode 0x45

Zensus `G2_04` (Stellen) und Gruppengroessen je Cut (`col_chg_zensus.py gruppen_je_cut`). "per Bild" =
sub01 wird im Original JEDES Bild neu gestartet (FUN_8003f038 @0x8003f064-84, memory
reai-v2-scd-per-frame-model); Switch auf work_vars[0x0A] = gezeigter Cut.

| Raum | Wann | Cut | Gruppen aus | Masken aus / im Cut |
|---|---|---|---|---|
| ROOM1150/1151 | sub05, Endlosschleife | 2 | 6..11 im Wechsel 20/20 Takte | 6 / 54 |
| ROOM1211 | sub01 per Bild, Ck(5,2) == 1 | 7 | 1, 2 | 10 / 70 |
| ROOM3000/3001 | sub01 per Bild, Ck(5,0) == 0 (main00: (5,0) := 0 wenn Ck(4,9) == 1, dann Zombies) | 0 / 1 / 2 / 3 | 1-3 / 8-12 / 8-12 / 4-8 | 43/43, 50/93, 34/85, 47/105 |
| ROOM3010/3011 | sub01 per Bild, Ck(5,0) == 0 (main00: (5,0) := 0 wenn Ck(4,10) == 1) | 0 / 1 / 2 / 3 / 4 / 6 | 1,2 / 1,2,6,7 / 1 / 1,2 / 1 / 3,4 | 26/56, 55/94, 9/57, 17/83, 14/82, 26/49 |
| ROOM3071 | sub01 per Bild, Ck(5,3) == 0: alle aus; sub02 (Szene, setzt (5,3) := 1): Gruppe 13 an 20 Takte, 13 aus + 12 an, ... bis 3 an | 9 | 3..13 | 11 / 30 (Lichtfolge) |
| ROOM5060/5061 | sub01 per Bild, Ck(5,4) == 1 | 11 | 1 | 1 / 1 |

Heute zeichnet der Port in all diesen Faellen die Masken, die das Original ausblendet — dort verdecken
im Port Masken Figuren, die im Original sichtbar sind (ROOM3000 Cut 0: der ganze Maskensatz). Nach dem
Bau entspricht das Bild dem Original; das ist gewollt (memory reai-v2-original-oder-nicht), muss aber
je Raum abgenommen werden (6.6). ROOM3071 ist der Raum mit dem offenen Elza-Softlock (memory
reai-v2-runde30-neun-befunde): die Lichtfolge ist rein visuell, sub02 wartet nicht auf Masken — kein
neuer Softlock, der bestehende bleibt unberuehrt.

### 7.2 Weitere Risiken

* **Kein Softlock:** Opcode 0x45 laeuft weiter im selben Takt (wie heute `op_unknown`), keine Wartebedingung
  haengt an Masken. Die Pfadlaenge (3) ist unveraendert.
* ⛔ BERICHTIGT (Auflage 1): **kein Dirty-Schalter nach Menues.** Das Original schreibt nach dem
  Statusschirm und dem Kartenschirm Dirty := 2 und ueberspringt damit Aufbau UND die Cut-Schreiber
  (@0x80021bd4 -> 0x80021df8): work_vars[0x0C] bleibt, ein spaeteres `Cut_old` kehrt zum Cut VOR dem
  letzten Dirty-1-Apply zurueck. Der Port hat keinen Menue-Haken und bekommt keinen; gemessen:
  wv0C vor und nach dem Inventar gleich (§9.5).
* **Nachgezeichnete Masken (R15M):** bleiben unberuehrt (Zahl 0 -> sichtbar). R15M gibt es nur bei
  NULL-Sektion (main.c ~5697), also nie gemischt mit Original-Records im selben Cut.
* **Spur B (Hebetisch ROOM1150/1151):** keine Ueberschneidung — die Blink-Gruppen gibt es nur in Cut 2,
  B arbeitet mit AOT 11/12, Nachrichten 20/21, obj 8 und Cut 4 (sub04). Gemeinsame Dateien bei der
  Zusammenfuehrung: scd_vm.c (eine Tabellenzeile), main.c, render_pc.c, room_common.c, menu_common.c —
  je eigene Zeilen. Spuren A/C/D/E/F: keine gemeinsamen Stellen.
* **PSX-Ziel:** render.c braucht dieselbe Zeile; der PSX-Bau ist hier nicht pruefbar (bekannte Luecke).
* **Leistung:** 54 Byte-Vergleiche je 0x45 und je Bild ein Byte-Test je Maske — vernachlaessigbar.

## 8 Offene Punkte

1. ⛔ **Werkzeugfehler (nicht Teil dieses Auftrags):** `tools/maske/original.py atlas()` entpackt den
   SLD-Atlas anders als der Port (ROOM1150 Cut 2: 11978/65536 Texel verschieden; Port ==
   `BSS/ROOM1150/PRI02.TIM`). Wer mit `original.py` Masken misst, misst falsche Pixel. Meldung an die
   Besitzer der Masken-Werkzeuge; die Spur G2 benutzt es nicht mehr.
2. Raum-ESP ROOM1150 (Effekt-Id 0x01, TIM @0x1E520): nicht die Schrift (3.6). Wer ihn startet, ist
   offen und nicht Teil dieses Punkts.
3. DuckStation-Steuerung: die Pad-Bindungen in `settings.ini` (Stand 2026-09-26) sind Akkorde
   `Keyboard/X & SDL-0/Y` — vgamepad-Navigation (re15_quickload.py) wirkt damit nicht mehr. Der
   RAM-Patch-Weg (3.5, `ds_lauf.py`) braucht keine Eingabe. Einstellungen nicht geaendert; der Skill
   re15-room-capture sollte den Befund bekommen.
4. Raumeintritt MIT Cut 2 als Eintritts-Cut kommt ueber Tueren nicht vor: die einzigen Tueren nach
   ROOM1150 stehen in ROOM1130/1131 main00 @0x008CE (Door_aot_set, Ziel Raum 0x15, Eintritts-Cut 0 —
   Zensus aller STAGE1-RDTs); der Debug-Sprung setzt ebenfalls Cut 0 (FUN_8001d600 Z. 24). Nur ein
   Spielstand mit camera_cut 2 (Port-Werkzeug) startet in Cut 2. Im Original liefe der erste sub05-Takt
   in der SCD-Init vor dem Aufbau (Aufbau erst beim Present) — der Port hat dieselbe Reihenfolge
   (scd_room_reenter vor load_bg_cut). Im Bau mit dem Lade-Weg-Haken (6.3) mitpruefen.
5. Kein Nutzer-Rueckfragebedarf: das Original blinkt nachweislich (3.5), der Befund des Nutzers ist
   bestaetigt.

## 9 Umsetzung (Stufe BAU)

Stand 2026-09-30 ~10:45. Bau im Baum `.claude/worktrees/r34n_schrift`, Zweig `r34n/schrift1170`.
Ergebnis: **die Leuchtschrift "HEAVEN" in ROOM1150/1151 Cut 2 blinkt jetzt wie im Original** —
20 Bilder AN / 20 Bilder AUS, Umschaltbild = Bild des Opcode-0x45-Takts von sub05, am Lade-Weg und
nach Debug-Sprung mit Zonen-Kamerawechsel an der echten exe gemessen (§9.5). Dieselbe Korrektur
wirkt in acht weiteren Raeumen (§9.6), fuer ROOM3000 Cut 0 gegen DuckStation gegengeprueft.

### 9.1 Auflagen der Gegenpruefung (abgehakt / abgelehnt mit Beleg)

| Nr | Auflage | Umsetzung / Beleg |
|---|---|---|
| 1 | Kein Neuaufbau nach Statusschirm und Kartenschirm | **erfuellt.** Kein Haken in menu_common.c und am Karten-/Speicherschirm. Beleg im Code (Kopf `re15_masken_gruppen.h`, Kommentar am Haken in main.c): Dirty := 2 @0x800466fc (Wert @0x800466dc) bzw. @0x80026634 (Wert @0x8002661c), FUN_80021bbc @0x80021bc4/@0x80021bc8/@0x80021bd4 springt bei 2 nach 0x80021df8 ueber @0x80021bf4, @0x80021bfc, @0x80021c28. Dossier 4, 5.2, 5.4, 6.5, 7.2 berichtigt. Gemessen (§9.5, G2_12): Inventar in AUS- und AN-Phase -> kein Aufbau, Zustand bleibt, wv0C unveraendert. Als Riegel: Lauf C von `integration_r34n_g_schrift1150`; Mutation "pending = 1 in close_phase" -> ROT (§9.3). |
| 2 | Optionsschirm-Haken nur mit Nachweis | **kein Haken (abgelehnt: im Port gibt es keinen Ort dafuer).** Selbst disassembliert: im Spiel erreicht man den Optionsschirm nur ueber SELECT+START — @0x8001cd24 `ori v1,zero,0x900`, @0x8001cd2c `lhu v0,DAT_800ac760`, @0x8001cd34 `andi v0,v0,0x900`, @0x8001cd38 `bne`; @0x8001cd48 aca3c \|= 0x8000, @0x8001cd58-60 aca38 \|= 0x08000000; die Transitions-FSM startet dann @0x8001cb08-30 Task 1 = 0x8002dde4 (Optionen) STATT 0x8004603c (Statusschirm, @0x8001cb38-3c). Die 1 @0x8002e730 kommt also nicht aus dem Statusschirm, und keine 2 folgt ihr (Dirty-Store-Zensus G2g). Ob sie den Apply erreicht, haengt zusaetzlich am Gate @0x8002151c (DAT_800b536c, einziger Schreiber FUN_80021634 @0x80021638) — nicht zu Ende belegt. Der Port hat diesen Weg nicht: menu_common.c:179/2395 "SELECT+START ... alternate task 0x8002dde4 ... not ported". Wer ihn portiert, muss @0x8002e730 (+ Gate) mitnehmen (§9.8). |
| 3 | PSX vollstaendig oder gar nicht | **erfuellt:** `platform/psx/src/render.c` Maskenschleife `if (!re15_mg_sichtbar(i)) continue;` UND `platform/psx/main.c` im `re15_cam_present_tick()`-Zweig `re15_mg_aufbauen(...)`; den Raumlader deckt das gemeinsame room_common.c. PSX-Bau hier nicht pruefbar (bekannte Luecke, memory reai-v2-psx-build-gap). |
| 4 | Blink-Abnahme ohne RE15_FORCE_CUT | **erfuellt:** Integrations-Riegel ohne FORCE_CUT (Lade-Weg-Karte); 6.4 ueber Debug-Sprung + zu Fuss (RVD). Festgehalten in 6.4, im Kopf des Riegels und am Haken in main.c: FORCE_CUT (main.c pc_cam_present_apply) setzt jedes Bild pending -> Aufbau jedes Bild -> Blinken unsichtbar; die r30-Tisch-Riegel (FORCE_CUT=2) sind deshalb kein Blink-Beleg (sie bleiben gruen, §9.4). Fuer die per-Bild-sub01-Raeume (§9.6) ist FORCE_CUT unschaedlich: sub01 schaltet im selben Takt nach dem Aufbau wieder aus. |
| 5 | 6.5 mit richtiger Erwartung | **erfuellt, gemessen** (§9.5, G2_12): AUS-Phase: := 0 F119, Menue F126..F176, AUS bis F190, := 1 F191 = 20 SCD-Takte nach F119; AN-Phase: := 1 F139, Menue F146..F196, AN bis F210, := 0 F211. wv0C = 0 vor und nach dem Menue (Mess-Zeile traegt wv0A/wv0C). Als Riegel: Lauf C des Integrations-Riegels. |
| 6 | Aufbau-Zaehler als Pin | **erfuellt:** `RE15_MG_LOG` schreibt je Aufbau Bild, Raum, wv0A/wv0C, Cut, pri_offset, Zahl, Grund (raum/cut). Gemessen: CONTINUE genau 1 Aufbau (F0); Debug-Sprung 1240->1150: je Cut-Ereignis einer (1240 F0/F162, 1150 Eintritt "raum", F2 RVD 0->1, F318 RVD 1->2), nie zwei in Folge ohne Ereignis; Inventar: keiner. ROT/GRUEN im Integrations-Riegel: `--aufbau 1` in den Laeufen A, B und C (C = mit Statusschirm). |
| 7 | Tabelle == gezeigter (Raum, Cut) | **erfuellt:** Aufbau liest `rdt->raw`/`raw_size`/`cuts[cut].pri_offset` des AKTUELLEN Parse (masken_gruppen.c), kein eigener Puffer. Aufbau-Zeile == `[pri]`-Zeile in allen Laeufen: Debug-Sprung aus ROOM1240 -> 1150 Eintritts-Cut 0 `pri_offset=0x500 Zahl 2` / `[pri] cut=0 pri_offset=0x500 masks=2`; RVD Cut 1 0x51C/28; Cut 2 0x66C/54; CONTINUE 1150 und 1151 Cut 0 0x500/2 (Opcode 0x45 dort 0 Treffer, beide Masken an) und Cut 2 0x66C/54; ROOM3000 Cut 0 0x288/43, ROOM3010 0x564/56, ROOM1211 Cut 7 0x1464/70, ROOM5060 Cut 11 0x26EC/1. |
| 8 | local_build.sh nicht anfassen | **erfuellt** (unveraendert; Suite 430 >= 428). |
| 9 | Sichtbare Aenderung in acht weiteren Raeumen ankuendigen | **erfuellt:** Raeume in Dossier (7.1, §9.6), Commit-Texten und Versionshinweis-Vorschlag (§9.6); Vorher/Nachher-Bilder fuer ROOM3000, 3010, 5060, 1211, 3071 (G2_13..G2_18), Elza-Varianten 3001/3011/5061 per Log; **DuckStation-Gegenprobe ROOM3000 Cut 0 liegt vor** (G2_16): Original normal 43/43 Records Byte0 = 1, Zombie-Variante 0/43, Bildspeicher ohne die Leichen — gleich dem Port. |
| 10 | Begruendung im Code-Kommentar praezisieren | **erfuellt:** Kommentar am Haken in main.c nennt die Dirty-1-Setzer @0x8001d5c8/@0x80021514/@0x800402f4/@0x80040354, room_common.c den Raumlader @0x8001daec; "einen Dirty-2-Pfad hat der Port nicht und bekommt keinen". |

### 9.2 Dateien und Haken

Neu (Spur G2):
* `re15_port/include/re15_masken_gruppen.h`, `re15_port/engine/src/masken_gruppen.c` — Record-Tabelle
  DAT_800b2584 (Byte0/Byte1), Zahl = RDT[0]; `re15_mg_aufbauen` (FUN_800392d4), `re15_mg_setzen`
  (FUN_800396a8), `re15_mg_sichtbar` (FUN_80039590 @0x800395f0-f4), `op_col_chg_set` (Opcode 0x45,
  @0x800428d4). Dateiname nicht `<thema>_<raum>.c`, weil raumuebergreifend (Gegenpruefung §4).
* Pin `tests/unit/probe_r34n_g_maskgrp.c` -> `unit_r34n_g_maskgrp`; Auswerter
  `tests/unit/probe_r34n_g_schrift_eval.c`; Integrations-Riegel
  `tests/integration/test_r34n_g_schrift1150.cmake` -> `integration_r34n_g_schrift1150`; beide
  registriert in `tests/unit/probes/r34n_g_schrift.cmake`.
* Werkzeuge `re15_port/tools/r34n_g/beleg_blinkt.py` (Belegbild Takt), `vorher_nachher.py` (alte gegen
  neue exe), `ss_masken_records.py` (Record-Tabelle + Codevergleich + Bildspeicher eines Savestates),
  `ss_zensus_raeume.py` (Stage/Raum/Cut aller Savestates).
* `tests/unit/probe_r34n_g_karte.c` erweitert: jeder Raum, `p=x,z`, `f=bank:bit` (fuer 6.6).

Haken in gemeinsamen Dateien (je 1-6 Zeilen, keine fremde Zeile umformatiert):

| Datei | Haken | Beleg |
|---|---|---|
| `engine/src/scd_vm.c` register_opcodes | `s_op_table[0x45] = op_col_chg_set;` (Block mit extern) | Tabelle @0x800745bc -> 0x800428d4 |
| `engine/src/room_common.c` Schritt 9 | `re15_mg_aufbauen(c->rdt, cut, "raum");` + include | Raumlader Dirty := 1 @0x8001daec -> @0x80021c28 |
| `platform/pc/main.c` pc_cam_present_apply, Zweig `re15_cam_present_tick()` | `re15_mg_aufbauen(rdt_ok ? rdt : NULL, active_cut_idx, "cut");` | Dirty-1-Setzer @0x8001d5c8/@0x80021514/@0x800402f4/@0x80040354 |
| `platform/pc/src/render_pc.c` Maskenliste | nur `re15_mg_sichtbar(i)` in `mask_order` | FUN_80039590 @0x800395f0-f4 |
| `platform/psx/main.c` Zweig `re15_cam_present_tick()` | `re15_mg_aufbauen(re15_test_rdt_ok ? &re15_test_rdt : NULL, target_cut, "cut");` | wie PC |
| `platform/psx/src/render.c` Maskenschleife | `if (!re15_mg_sichtbar(i)) continue;` | wie PC |

Kein Haken am Boot-/CONTINUE-Weg (Plan 5.2 Zeile 4): der setzt schon `g_scd.cam_change_pending = 1`
(main.c ~4451 Neues Spiel, ~4701 CONTINUE), der erste Praesentations-Apply baut also im ersten Bild
auf — gemessen: `[maskgrp] F0 ... Aufbau Cut 2 ... Grund cut` nach CONTINUE. Auch kein Haken am
Sonderweg `re15_bg_load_cut(0)` (main.c ~8026): der gehoert zur Mess-Schiene RE15_CLIP_TEST in
ROOM1170 (keine 0x45-Stelle).

Reihenfolge im Bild (Plan 5.3) eingehalten: Aufbau im Praesentations-Apply am Bildanfang -> SCD-Takt
(0x45) -> pri-Block -> end_frame. Beim Tuerweg baut Schritt 9 im Bild des Raumwechsels auf; dieses
Bild ist wegen der Tuer-Blende (Pegel 0x7FFF, Schwarz) nicht sichtbar, ab dem naechsten Bild passen
Maskenliste und Tabelle zum neuen (Raum, Cut) (gemessen §9.1 Nr 7). Eine 0x45 in der SCD-Init eines
neuen Raums trifft vor Schritt 9 noch die alte Tabelle und wird von Schritt 9 ueberschrieben — im
Original findet sie RDT[0] = 0 der frisch geladenen RDT (206/206 RDTs Byte 0 == 0): beide ohne Wirkung.

### 9.3 Pins und Mutationsproben

* `unit_r34n_g_maskgrp` (echte RDT-Bytes, echte VM, echter Raumstart): Teil A Aufbau/Opcode (13
  Pruefungen), Teil B Takt 1150 und 1151 (Wechsel bei Takt 39 59 79 ... 199 = exakt 20),
  Teil C Cut 2 -> 0 -> 2 in einer AUS-Phase (sofort AN, naechstes AUS erst beim naechsten := 0).
  **Mutation 1:** `s_op_table[0x45] = op_col_chg_set` entfernt -> 8 FAIL; zurueck -> OK.
* `integration_r34n_g_schrift1150` (echte exe, Lade-Weg, ohne FORCE_CUT): Referenz ohne Masken
  (RE15_NO_PRI), Lauf A 1150 und B 1151 je 101 Bilder gegen das Log-Orakel, genau 1 Aufbau, 4 volle
  Laeufe je 20 Bilder, AN-Bild 558 Bildpunkte Unterschied (== Dossier 3.3); Lauf C 1150 mit
  Statusschirm in der AUS-Phase (genau 1 Aufbau, Zustand vor/nach dem Schirm gleich, 3 volle Laeufe).
  **Mutation 2:** Filter in render_pc.c aus -> 42 Bilder falsch, 0 AUS, ROT. **Mutation 3:**
  Aufbau-Haken in main.c aus -> "kein Aufbau Cut 2", 101 Bilder falsch, ROT. **Mutation 4
  (Auflage 1):** `g_scd.cam_change_pending = 1` im gemeinsamen Abbau von close_phase (menu_common.c)
  -> "2 Aufbau-Zeilen", Lauflaenge falsch, ROT. Alle zurueckgesetzt (git diff leer) und neu gebaut.

### 9.4 Suite

* Lauf 1 (10:10-10:30, parallel zu eigenen exe-Messlaeufen): **429/430**; der eine Ausfall war
  `integration_r34n_g_schrift1150` selbst — das Skript war waehrend des Laufs um Lauf C erweitert
  worden, der Auswerter aber noch der alte (ohne `--menue`, verglich die Schirm-Bilder). Nach dem
  Neubau einzeln gruen (75 s), auch nach Mutation 4 + Ruecknahme.
* Lauf 2 (Endstand, Commit 28e74760 + unveraenderter Code, ohne parallele Messlaeufe): `=== LOCAL-BUILD-OK (all) — Tests 430/430` (771 s). Darin gruen: unit_r34n_g_maskgrp, integration_r34n_g_schrift1150 (79 s, Laeufe A/B/C), integration_r30_irons_tisch_laden/_bild/_licht (FORCE_CUT=2 in ROOM1150 — unberuehrt, kein Blink-Beleg).

### 9.5 Eigene Abnahme an der echten exe (Bilder)

Alle Laeufe mit einer KOPIE der exe (re15_pc_g2b..e.exe), `RE15_WINDOW_SCALE=3`, beschleunigter
Renderer, `RE15_FRAMEDUMP` (Readback des fertig komponierten Bilds vor Present). Auswertung
`port_schrift1150_eval.py` (Referenzen AN/AUS aus BSS + Atlas, max|d| 0) bzw. der Riegel-Auswerter.

| Lauf | Weg | Ergebnis | Beleg |
|---|---|---|---|
| Lade-Weg 1150 | CONTINUE, Karte Cut 2 | 121/121 Dumps F100..F220 exakt AN oder AUS; AN F100-118, AUS 119-138, AN 139-158, AUS 159-178, AN 179-198, AUS 199-218; Log: := 0 F119/F159/F199, := 1 F139/F179/F219; 1 Aufbau | `G2_10_port_blinkt_1150_ladeweg.png`, `G2_10_eval_ladeweg_1150.txt` |
| Lade-Weg 1151 | wie oben | identisch (gleiche Bilder, gleiche Takte) | `G2_10_eval_ladeweg_1151.txt` |
| Debug-Sprung + zu Fuss | JUMP 1150@240, `RE15_PLAYER_POS=-19000,-13700,1684`, `RE15_INPUT_SCRIPT=U2.4` (Basis Spielbilder, Start 250) | Cut 0 (Aufbau raum, Zahl 2) -> F2 RVD Cut 1 (Zahl 28) -> F318 RVD Cut 2 (Zahl 54): AN F318-324 (Aufbau), AUS ab F325 (:= 0 von sub05), dann 20/20 | `G2_11_port_debugsprung_rvd_cut2.png`, `G2_11_eval_debugsprung.txt` |
| Inventar AUS-Phase | CONTINUE, START F125 / nach 36 Bildern zu | AUS F119-125, Schirm F126-176, AUS F177-190, AN ab F191 (20 SCD-Takte nach F119), kein Aufbau, wv0C 0/0 | `G2_12_port_inventar_in_aus_phase.png`, `G2_12_eval_inventar_aus.txt`, `G2_12_mglog_inventar_aus.txt` |
| Inventar AN-Phase | START F145 | AN F139-145, Schirm F146-196, AN F197-210, AUS ab F211 | `G2_12_eval_inventar_an.txt`, `G2_12_mglog_inventar_an.txt` |
| Eintritts-Cut 0 | CONTINUE 1150/1151 Cut 0 | Aufbau `pri_offset=0x500 Zahl 2`, 0x45 dort 0 Treffer | (Log, §9.1 Nr 7) |

Selbst angesehen: G2_10/G2_11/G2_12 — die Buchstaben erscheinen im AN-Zustand als dunkle Lamellen
auf Rot, im AUS-Zustand gleichmaessiges Rotlicht, wie im Original-Bildspeicher (G2_05).

**Andere Raeume (Abnahme 6.6, alte exe = Stand cf0e68ba ohne Opcode 0x45, neue exe, gleiche Umgebung):**

| Raum / Cut | Weg | RE15_MG_LOG (neue exe) | sichtbar (selbst angesehen) | Beleg |
|---|---|---|---|---|
| ROOM3000 Cut 0, Zombie | Debug-Sprung, RE15_SET_FLAG 4:9 | je Bild Gruppen 1..3 := 0 (9+20+14 = 43/43) | die 43 Masken sind die vorgerenderten LEICHEN der Polizisten; alt: Leichen UND Zombies, neu: nur der Blutfleck des Hintergrunds | `G2_13_raum3000_zombie_vorher_nachher.jpg` |
| ROOM3000 Cut 0, normal | Debug-Sprung ohne Flag | kein 0x45 (43/43 an) | unveraendert (Leichen da) | Log |
| ROOM3000 Cut 0, **Original** | DuckStation, sauberer Stand per RAM-Patch des Debug-Sprungs | Records normal 43/43 Byte0 = 1, Zombie 0/43 | Original-Bildspeicher Zombie: keine Leichen | `G2_16_original_raum3000_normal_zombie.png`, `G2_16_original_raum3000_records.txt`, `G2_16_protokoll.txt` |
| ROOM3010 Cut 0, Zombie | Debug-Sprung, 4:10 | Gruppen 1,2 := 0 (16+10 = 26/56) | Leichen-Masken in der Mitte weg | `G2_14_raum3010_zombie_vorher_nachher.jpg` |
| ROOM5060 Cut 11 | Debug-Sprung, FORCE_CUT 11, SET_FLAG_AT 5:4@300 | ab F300 Gruppe 1 := 0 (1/1) | Hebel/Einsatz im Schacht weg, roter Pfeil des Hintergrunds | `G2_15_raum5060_cut11_vorher_nachher.jpg` |
| ROOM1211 Cut 7 | Karte 1211/7, FORCE_CUT 7, SET_FLAG_AT 5:2@60 | ab F60 Gruppen 1,2 := 0 (5+5 = 10/70) | Bett im Hintergrund-Zustand (heller) statt der dunkleren Masken-Fassung | `G2_17_raum1211_cut7_vorher_nachher.jpg` |
| ROOM3071 Cut 9 | Karte 3071/9, FORCE_CUT 9, (5,3) = 0 | Gruppen 3..13 := 0 (11/30) | blaue Lichtleiste an der Wand AUS (alt: dauernd an); die Elza-Szene sub02 schaltet sie nacheinander an | `G2_18_raum3071_cut9_vorher_nachher.jpg` |
| ROOM3001 / 3011 / 5061 | Karte + f=4:9 / f=4:10 / 5:4 | 43/43, 26/56, 1/1 wie die Grundvarianten | — | Log |

⛔ `stage_saves/PATCHED-EXE_HASH-881C08B8082E53B6_3.sav` (ROOM3000) taugt fuer diese Frage NICHT: dort sind
der SCD-Laeufer (@0x8003f088 `jal 0x8007153c`) und der Cut-Apply (@0x80021bf8/@0x80021c08) gepatcht
(`ss_masken_records.py`: Code != PSX.EXE). Deshalb der neue saubere Stand per RAM-Patch.

### 9.6 Abweichungen vom Plan (mit Grund) und sichtbare Aenderung in anderen Raeumen

* Menue-/Karten-/Options-Haken entfallen (Auflagen 1/2, §9.1).
* Boot-/CONTINUE-Haken entfaellt (§9.2: der Weg setzt schon pending, der erste Apply baut auf; gemessen).
* `re15_mg_aufbauen` hat einen dritten Parameter `grund` (nur fuer die Mess-Zeile, Auflage 6).
* Mess-Zeile schreibt zusaetzlich wv0A/wv0C (Auflage 5).
* Byte0 bekommt das GANZE op2-Byte (@0x800396e0 `sb a1,0(v1)`), gezeichnet wird Bit 0
  (@0x800395f0) — wie im Original (Pin Teil A: `45 06 02` -> Byte0 2 -> unsichtbar).
* Kein Takt-Pin mit den Original-VBlanks noetig: der Takt ist Skript-Daten (Sleep 20) auf der
  vorhandenen 30-Hz-SCD-Basis (1 SCD-Takt je Bild; Original VSync(2), G2_06: 40 VBlanks je Zustand).

**Sichtbare Aenderung in acht weiteren Raeumen (Auflage 9), Vorschlag fuer den Versionshinweis:**
"Vordergrund-Masken, die ein Raumskript per Opcode 0x45 ausschaltet, werden jetzt wie im Original
nicht mehr gezeichnet: ROOM1150/1151 Cut 2 (die Leuchtschrift HEAVEN hinter Irons' Schreibtisch
blinkt), ROOM3000/3001 und ROOM3010/3011 in der Zombie-Variante (die vorgerenderten Leichen der
Polizisten verschwinden, wenn sie als Zombies aufstehen), ROOM3071 Cut 9 (Lichtleiste aus bis zur
Elza-Szene, dort Lichtfolge), ROOM1211 Cut 7 (nach Flag (5,2)), ROOM5060/5061 Cut 11 (nach Flag (5,4))."

### 9.7 Commits

20a7761d (Modul + Haken), f52dc5ce (Pin), 97e47445 (Abnahme exe), b0fc996a (Auflage 5),
cdea3286 (Integrations-Riegel), 0488fdb8 (Dossier), 308dbd28 (6.6 3000/3010/5060), 20ce50da
(DuckStation-Gegenprobe ROOM3000), 47a75ba9 (Riegel Lauf C + Kartenwerkzeug), 53fee0cf (6.6 1211/3071),
28e74760 (Dossier Abschnitt 9), danach der Abschluss-Commit mit der Suite-Zeile.

### 9.8 Offene Punkte

1. **PSX-Ziel nicht gebaut** (bekannte Luecke): die zwei PSX-Zeilen sind nur gelesen, nicht kompiliert.
2. **Optionsschirm im Spiel** (SELECT+START, Task 0x8002dde4) ist im Port nicht portiert. Wer ihn
   portiert: Original schreibt beim Verlassen Dirty := 1 (@0x8002e730, nur bei DAT_800aca38 &
   0x40000000) -> Neuaufbau "alle an", sofern das Gate @0x8002151c (DAT_800b536c) es durchlaesst —
   dieses Gate ist noch zu belegen.
3. **Zustand der ANDEREN Raeume im Original** nur fuer ROOM3000 Cut 0 gemessen (DuckStation, Records +
   Bildspeicher). ROOM1211/3010/3071/5060 sind statisch (SCD-Dumps, Gegenpruefung §3) und im Port
   gemessen, nicht im Original.
4. Aus der Ermittlung (§8) unveraendert offen: Werkzeugfehler `tools/maske/original.py atlas()`
   (11978/65536 Texel falsch fuer ROOM1150 Cut 2); Raum-ESP ROOM1150 (Effekt 0x01) — nicht Teil
   dieses Punkts.
5. Mess-Schiene RE15_FORCE_CUT: unter ihr ist das Blinken in ROOM1150 unsichtbar (Aufbau jedes Bild) —
   gewollt, dokumentiert; wer den Blink-Zustand pruefen will, nimmt die Lade-Weg-Karte.
