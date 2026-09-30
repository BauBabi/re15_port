# Spur G2 — ROOM1150 Irons' Buero: blinkende Schrift im Hintergrund

Stufe: ERMITTLUNG + BAUPLAN (kein Port-Code). Zweig r34n/schrift1170, Baum .claude/worktrees/r34n_schrift.
Status: Ermittlung abgeschlossen (Original gemessen, Port gemessen, Mechanismus disassembliert);
Bauplan Abschnitt 5, Abnahmeplan 6. Stand 2026-09-30 ~09:40.

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
* Nach dem Statusschirm (Inventar) setzt das Original DAT_800b5457 := 2 (@0x800466dc/@0x800466fc,
  Normalfall DAT_800b25c0 != 1) -> naechster Present ruft FUN_80021bbc mit DEMSELBEN Cut -> alle
  Masken wieder an (Buchstaben sichtbar bis zum naechsten :=0). Ebenso nach dem Optionsschirm
  (@0x8002e730, nur bei DAT_800aca38 & 0x40000000) und nach dem Speicherkarten-Schirm
  (@0x80026634 in FUN_80026594, Parameter 0). Auch Cut_chg/Cut_old (Opcode 0x29/0x2a) setzen den
  Dirty-Schalter (@0x800402f4/@0x80040354) und bauen damit neu auf.

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
| `engine/src/menu_common.c` `close_phase`, gemeinsamer Abbau (Z. ~1420) | `g_scd.cam_change_pending = 1;` (Normalzweig) | @0x800466d0-fc: DAT_800b25c0 != 1 -> `sb 2,DAT_800b5457` |
| Options-/Kartenschirm-Rueckkehr im Spiel (Port-Stellen im Bau suchen: Nachbauten von FUN_8002dfb0 / FUN_80026594, main.c ~1765 / ~2252) | `g_scd.cam_change_pending = 1;` | @0x8002e730 (nur DAT_800aca38 & 0x40000000), @0x80026634 (Parameter 0) |
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
| Neuaufbau nach Statusschirm | Dirty := 2 | `ori v0,zero,0x2` @0x800466dc, `sb` @0x800466fc |
| Neuaufbau nach Options-/Kartenschirm | Dirty := 1 bzw. 2 | @0x8002e728/30, @0x8002661c/@0x80026634 |

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
   nach Cut 2 laufen (`RE15_PLAYER_POS` + `RE15_INPUT_SCRIPT`) oder `RE15_FORCE_CUT`: nach dem Eintritt
   in Cut 2 zuerst AN (Aufbau), erstes AUS erst beim naechsten :=0 von sub05.
5. **Statusschirm:** in Cut 2 waehrend eines AUS-Laufs das Inventar oeffnen/schliessen
   (`RE15_INV_OPEN_AT`): nach dem Schliessen AN bis zum naechsten :=0 (Original @0x800466fc).
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
* **Dirty-Schalter nach Menues** (Haken menu_common.c / Options / Karte): setzt wie das Original
  work_vars[0x0C] (alter Cut) := gezeigter Cut (@0x80021bf4). Ein spaeteres `Cut_old` nach einem
  Inventarbesuch kehrt damit zum gezeigten Cut zurueck — Original-Verhalten; Zwischenszenen, die
  Cut_old benutzen, lassen kein Inventar zu. Ein Pin sollte work_vars[0x0C] nach dem Schliessen pruefen.
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
4. Raumeintritt MIT Cut 2 als Eintritts-Cut (kommt ueber Tueren nicht vor): im Original laeuft der
   erste sub05-Takt in der SCD-Init vor dem Aufbau (Aufbau erst beim Present) — der Port hat dieselbe
   Reihenfolge (scd_room_reenter vor load_bg_cut). Im Bau mit dem Lade-Weg-Haken (6.3) mitpruefen.
5. Kein Nutzer-Rueckfragebedarf: das Original blinkt nachweislich (3.5), der Befund des Nutzers ist
   bestaetigt.
