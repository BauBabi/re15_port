# Spur G2 — ROOM1150 Irons' Buero: blinkende Schrift im Hintergrund

Stufe: ERMITTLUNG + BAUPLAN (kein Port-Code). Zweig r34n/schrift1170, Baum .claude/worktrees/r34n_schrift.
Status: Ermittlung abgeschlossen (Original gemessen, Port gemessen, Mechanismus disassembliert);
Bauplan Abschnitt 5. Stand 2026-09-30 ~09:15.

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
  (VSync-Modus DAT_800b5456 = 2), Umschaltpunkte exakt bei Vcount 7052 + 40·k — **40 VBlanks je
  Zustand = 20 Spielbilder, Periode 80 VBlanks** (bei 59,83 Hz 0,669 s an / 0,669 s aus). RAM-Zustand
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

**Messung 3 — echter Durchlauf / Spielstand:** (folgt, Abschnitt 2.1)

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

(folgt — Einordnung nach reai-v2-beta-zu-retail: RE1.5 hat das System VOLLSTAENDIG (Daten + Opcode +
Zeichnen) — RE1.5 ist massgeblich.)

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

## 5 Bauplan

(folgt)

## 6 Abnahmeplan

(folgt)

## 7 Risiken, Softlocks, Wechselwirkungen

(folgt; 7.1 = Wirkung auf die 10 Raeume mit Opcode 0x45)

## 8 Offene Punkte

(folgt)
