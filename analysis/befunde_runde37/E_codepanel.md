# Runde 37 — Spur E "codepanel" (Dossier, fortlaufend)

Baum `.claude/worktrees/r37_codepanel` (Zweig `r37/codepanel`, Basis master f1e019ca). Auftrag: `AUFTRAG.md` #9.
Belegbilder: `analysis/befunde_runde37/E_belege/`. Messlaeufe: `re15_port/build/r37_codepanel/runs/<name>/`
(Messkopie `re15_pc_r37e_mess.exe`, Laeufer `re15_port/build/r37_codepanel/lauf.sh`, nicht versioniert).

## Wortlaut (AUFTRAG.md #9, unveraendert)

> bei den Code panels möchte ich das, so lange die Code noch falsch eingegeben ist unten das blaue Licht leuchtet.
> Ist er dann richtig eingegeben das grüne Licht. Die Kamera darf noch auf den Panels bleiben wenn der Piep und
> unlock sound kommt.

Punkte:
- **E1** Lampe am Code-Panel — in der Fassung von Nachtrag 3/3b (AUFTRAG.md; der urspruengliche Wortlaut "blau ... gruen"
  ist damit ersetzt): die rot gemalte Lampe leuchtet rot, solange der Code nicht richtig ist ("rot = geschlossen"); ist er
  richtig, leuchtet die andere gemalte Lampe in ihrer Farbe (2F Corridor 10D0/10D1 blau, SECUGUARD gruen). Gebaut und
  geprueft ist diese Fassung (§2.4, Abnahme 0).
- **E2** Kamera bleibt auf dem Panel, waehrend Piep und Aufschliess-Ton kommen.

---

## 0. Zensus aller Code-Panels (alle STAGE1..6-RDTs, selbst erhoben)

Werkzeuge: `re15_port/tools/rdt_msgdump.py` (alle 240 RDTs, Texte mit "number/code/digit/panel/lock/card reader"),
`re15_port/tools/scd_dump_room.py` (Cursor-Raster = `Aot_set sce=5 flags=0x44` + Cursor-Prop `Obj_model_set type=4`),
Hintergruende aller Stages ueber `probe_bg_dump` (Port-MDEC-Dekoder), Nahaufnahmen = Cuts mit dem blauen Metallrahmen
(42 Stueck, Bogen angesehen).

| Raum | Ziffern-Eingabe | Panel-Cut (sub17 `Cut_chg`) | Code | Schloss-Flag (sub01) | gemalte Lampen im Panel-Cut |
|---|---|---|---|---|---|
| ROOM10D0 / 10D1 | ja ("Enter the first number." msg 0..4) | 11 (@0x018D6 / @0x018C0 `29 0b`) | 4312 (Zettel ROOM1110) | (3,50) @0x0152A / @0x01514 | ROT rund links (x 116..127, y 72..81), BLAU rund rechts (x 164..173, y 72..81), unter dem Kartenschlitz |
| ROOM11E0 / 11E1 ("Prisons") | ja | 15 (@0x01EBE `29 0f`) | 0513 (Zettel ROOM1011) | (3,139) @0x01A8A | SECUGUARD: ROT / GELB / GRUEN eckig UNTEN (x 122..135 / 149..161 / 174..187, y 166..183) |
| ROOM1230 / 1231 ("Weapon Storage") | ja | 9 (@0x0139E `29 09`) | 5632 (Zettel ROOM1230 msg 10) | (3,137) @0x00FF2 | wie 11E0 (dieselbe SECUGUARD-Kunst, Pixel gleich) |

Keine weiteren Ziffern-Panels: ROOM4090 Cut 5 ("A numerical keypanel. If I had the codes, I could use the machine,
but..." msg 6) ist nur Text, ohne Raster. Die uebrigen Raster-Raeume (1080/4020 Fahrstuhl, 1100 VUS-Stimmschloss,
11F0 Generator, 10F0/11A0/2040/2050/2060/20A0/3050/30E0/5020/5050) haben keine Ziffern-Eingabe. ROOM1100 (VUS) hat
keine Lampen, nur eine Anzeige (Cuts 7..12 = SYSTEM READY / SAY HELLO / INVALID / TRY AGAIN / ACCEPTED / leer).
Bild: `E_belege/panels_10d0_11e0_1230.png`.

Alle sechs Raeume tragen in sub19 (Aufschliessen) denselben 18-Byte-Schwanz (selbst gelesen):
```
X+0  2a           Cut_old          10D0 X=@0x0199A, 10D1 @0x01984, 11E0/11E1 @0x01F82, 1230/1231 @0x01462
X+1  00           Nop
X+2  3c 01        Cut_auto 1
X+4  2b 05 ff ff  Message_on 5     "You've opened the lock." (Pausemaske 0xffff)
X+8  02           Evt_next
X+9  00           Nop
X+10 22 01 1b 00  Set(1,27,0)
X+14 22 02 07 00  Set(2,7,0)       Eingabe frei
X+18 01 00        Evt_end
```
und in sub01 dieselbe Abnahme `Ck(3,F,0) Ck(5,13..16,1) Evt_exec sub19 Set(3,F,1)` (F = 50 / 139 / 137).

---

## 1. Messung vorher (Stand f1e019ca, echte exe)

Lauf `v10d0_code`: `RE15_DEBUG_JUMP=10D0@120 RE15_SUBSTART=17@150#10D0` (= Tastenfeld auf wie nach "You've used the
Blue Keycard.", sub20 @0x019CA), `RE15_PAD_AT=200:A,300:A` (Nachricht 0 bzw. 5 schliessen),
`RE15_SET_FLAG_AT=5:13,5:14,5:15,5:16@230` (= vier richtige Ziffern), `RE15_CUT_SYNC_LOG`, `RE15_AUFSCHLIESS_LOG`,
`RE15_SCD_TRACE`, Framedumps F226..F300/2.

```
auf.log:  F231 raum=10D0 nachricht=5 art=KARTE_AUF satz=1(PIEP) bild=+0
          F246 raum=10D0 nachricht=5 art=KARTE_AUF satz=2(AUF_E) bild=+15
evt:      F231 Evt_exec sub=19
scd:      F231 @0x199A op=0x2A (Cut_old) / @0x199C 0x3C / @0x199E 0x2B (Message_on 5) / @0x19A2 0x02 (Evt_next)
          F302 @0x19A3 .. @0x19AC (nach dem Schliessen der Nachricht: Set(1,27,0), Set(2,7,0), Evt_end)
cut.log:  F220 bg=10d0#11 req=11 shown=11
          F231 bg=10d0#11 req=0 shown=0        <- Cut_old im Bild des Pieps
          F232 bg=10d0#0                       <- ab hier Raumansicht
```
Bild `E_belege/e2_vorher_10d0_F230_F232_F246_F300.png`: F230 Panel, F232 Raum ("Y"), F246 Raum ("You've o", der
Aufschliess-Ton), F300 Raum mit der ganzen Zeile. **Die Kamera verlaesst das Panel im Bild des Pieps; der
Aufschliess-Ton (F246) kommt in der Raumansicht.** (E2-Befund bestaetigt.)

E1 vorher: keine Lampe leuchtet — die gemalten Lampen sind in allen Panel-Cuts unbeleuchtet (dunkles Glas), RE1.5 hat
fuer die Code-Panels weder ESP noch einen zweiten (beleuchteten) Cut (ROOM10D.BSS 15 Chunks = 15 Cuts, ROOM11E.BSS 16 =
16, ROOM123.BSS 11 = 11; kein Zusatz-Chunk). Port-Bilder `runs/p10d0_auf`, `p11e0_auf`, `p1230_auf`.

---

## 2. RE-Belege

### 2.1 RE1.5 (selbst disassembliert / gelesen)

* Opcode-Tabelle `PTR_LAB_800744a8`: [42] @0x80074550 -> 0x8004032c (Cut_old), [60] @0x80074598 -> 0x800403ac (Cut_auto),
  [43] @0x80074554 -> 0x800404f4 (Message_on).
* Cut_old 0x8004032c: @0x8004033c `lbu a0,0x3f7b(a0)` (gemerkter Cut DAT_800b3f7b), @0x8004035c `sh v1` -> 0x800b0fe8,
  @0x80040364 `sh a0` -> 0x800b0fe4 (aktiver Cut), @0x80040368 `jal 0x800142f4`, @0x80040370..0x80040384
  DAT_800aca3c &= ~0x100 (Auto-Kamera an). Cut_auto 0x800403ac: pc[1]==1 -> @0x800403c8..0x800403e0 dasselbe Bit loeschen.
* sub19-Reihenfolge (oben): Kamera zurueck (X, X+2) VOR Message_on 5 (X+4) — deshalb verlaesst die Kamera das Panel im
  Bild des Pieps (der Piep haengt seit Runde 36 an Message_on 5).
* RE1.5s EIGENES Schloss-Panel ROOM1100 (VUS, Minidisc) haelt die Kamera bis NACH der Aufschliess-Nachricht
  (sub02, selbst gelesen): @0x00C90 `2b 04 ff ff` Message_on 4 / @0x00C94 Evt_next / @0x00C96..0x00CB6 sechsmal
  Cut_chg 0x0C/0x0B mit Sleep 5 ("ACCEPTED" blinkt) / **@0x00CBA `2b 02 ff ff` Message_on 2 "You've opened the lock."
  / @0x00CBE Evt_next / @0x00CC0 Set(2,7,0) / @0x00CC4 `29 06` Cut_chg 6 / @0x00CC6 `3c 01` Cut_auto 1**.
* RE1.5 schaltet Lampen sonst ueber eigene Cuts (ROOM1080 Cuts 1..4 = Ring der Etage gruen, ROOM4020 Cuts 4..8 rot) —
  fuer die Code-Panels gibt es keine solchen Cuts (s. 1.). Fuer E1 ist RE1.5 also unfertig -> RE2 (Beta -> Retail).

### 2.2 RE2 Retail (selbst gelesen; `re15_port/tools/r34n_c/re2_scd_dump.py`, `re2_disasm.py`)

**Kartenleser ROOM2110 ("Weapon Storage", "The power to the card reader is off/on"):**
```
sub03 (Raumaufbau, gosub aus sub00 @0x017C2)
  @0x01832 if Ck(4,0x3C)==0 (kein Strom):  @0x0183A `64 01 16 00 ..` / @0x0184A `64 02 16 00 ..`  Zeile 0 ROT
  @0x0185E elif Ck(4,0x0E)==0 (zu):       @0x01866 `64 01 16 10 ..` / @0x01876 `64 02 16 10 ..`  Zeile 2 GRUEN
  else (offen):                           @0x0188A `64 01 16 08 ..` / @0x0189A `64 02 16 08 ..`  Zeile 1 BLAU
sub08 (Karte angenommen)
  @0x01B2C `29 0c` Cut_chg 12 (Kamera AUF den Leser)
  @0x01B4C Se_on(2,0x0C) Karte ein / @0x01B58 Sleep 15
  @0x01B5C Se_on(2,0x0D) PIEP
  @0x01B68 `65 01` / @0x01B6A `65 02` Lampen aus, @0x01B6C / @0x01B7C `64 0n 16 08 ..` Lampen BLAU  (im Piep-Bild)
  @0x01B8C Sleep 15 / @0x01B90 Se_on(2,0x0B) AUFSCHLIESSEN
  @0x01B9C `29 09` Cut_chg 9 / @0x01B9E `3c 01` Cut_auto 1   (Kamera erst NACH dem Aufschliess-Ton zurueck)
```
**Schloss ROOM4070 ("Clearance identification device", Wolf/Adler):** zwei RUNDE Lampen, links ROT, rechts BLAU
(gemaltes Glas `ROOM40706.bmp` rot x 168..175 / y 200..205, blau x 202..208 / y 204..209):
```
sub03  @0x00870 Ck(4,0x76)==1 (offen): @0x0088C `64 03 16 0c ..` BLAU rechts  | sonst @0x008C6 `64 02 16 04 ..` ROT links
sub08  @0x00D68 `29 06` Cut_chg 6 ... @0x00DC2 `64 03 16 0c ..` BLAU an / @0x00DD2 `65 02` ROT aus /
       @0x00DD4 Se_on(2,0x0C) PIEP / @0x00DF8 Sleep 0x32 / @0x00DFC `29 00` Kamera zurueck
```
Selbst projiziert (`re15_port/tools/r37_e/re2_lampen_projektion.py`, camera.rid, LookAt wie camera_common.c): die
ESP-Lagen fallen genau auf das gemalte Glas (4070 rot -> (171.3, 202.9) gegen Glasmitte (171.5, 202.5), blau ->
(204.6, 207.2) gegen (205.0, 206.5)); 2110 rot/gruen/blau liegen auf dem rot-braun GEMALTEN Streifen des Lesers
(z.B. (80,56,47)) — RE2 laesst diese rot gemalte Lampe GRUEN und BLAU leuchten. Bild `E_belege/re2_schlosslampen_2110_4070.png`.

**Handler (RE2 PSX.EXE, Tabelle @0x800a74c8):** [100] @0x800a7658 -> 0x80056644 sce_espr_on2: @0x8005666c `lhu s0,2(s1)`,
@0x8005667c `sll a0,s0,24` (ESP-Id), @0x80056680/84 `srl s0,s0,8`/`sll s0,s0,16` (Unterindex), @0x8005668c `lhu a1,6(s1)`
(scale16), @0x80056690/9c/a8 Lage, @0x800566c0 `jal 0x8001c8c4`, @0x800566cc PC+16. [101] @0x800a765c -> 0x800568e8
sce_espr_kill2: @0x80056900 `lbu a0,1(v0)` / @0x80056904 `jal 0x8001cfbc`, PC+2. Spawner 0x8001c8c4: @0x8001c8d4 `andi
t5,v0,0xff` (Unterindex), @0x8001c8d8 `andi v0,t5,0x7` (Strom), @0x8001c9e0 `srl v0,t5,3` / @0x8001c9e4 `sll v0,v0,6` /
@0x8001c9f8 `addu v1,v1,v0` / @0x8001c9fc `sh v1,50(t0)` -> CLUT = Kopf-CLUT + (Unterindex>>3) Zeilen.

**Kunst ESP 0x16** (`info/re2leon/PL0/RDT/room2110/esp16.tim` == `room2130/esp16.tim` == Port-Datei
`shared_assets/RE2/LAMPE2130.TIM`, `cmp` bytegleich -> KEIN neues Asset): 4 bpp 256x32, CLUT 16x4: Zeile 0 ROT, 1 BLAU,
2 GRUEN, 3 GELB, alle Nicht-Null-Eintraege mit Bit 15 (additiv). ESP-0x16-Block in effect.esp (ROOM2110 @0x0BFC ==
ROOM2130 @0x08, 492 B gleich): Zellen @+0x70 `00 00 f0 f0` .. `80 00 f0 f0` (0..4, S 32, Versatz -16), `a0 00 f4 f4`
(5, Versatz -12), `b8 00 f0 f0` (6), `d8 00 f0 f0` (7); Anim-Saetze @+0x10: 0..3 = Zellen 0,1,2,(Schleife) = Strom 0
(Rechteck, Kartenleser 2110); 4..6 = Zellen 3,4,(Schleife) = Strom 2 (Quadrat, 2130/6150); 7..11 = Zelle 5 (S 0x18),
7, 6, 7, (Schleife -> 7) = Strom 4 (rund, Schloss 4070). Zensus aller 495 RE2-RDTs: ESP 0x16 in 2110 (Strom 0, Zeilen
0/1/2), 2130 (Strom 2 Zeile 0; Strom 0 Zeilen 0/2), 4070 (Strom 4 Zeilen 0/1), 60C0 (Strom 0 Zeilen 0/1/3), 6150
(Strom 2 Zeile 1).

### 2.3 Einordnung

* E2 ist RE2- UND RE1.5-belegt: RE2 2110 haelt die Kamera auf dem Leser durch Piep und Aufschliess-Ton (Kamera erst
  @0x01B9C nach @0x01B90); RE1.5s eigenes Schloss-Panel 1100 haelt sie bis nach "You've opened the lock."
  (@0x00CBA..0x00CC6). Die Code-Panels 10D0/11E0/1230 schalten als einzige VOR der Nachricht zurueck.
* E1: RE2 hat genau dieses System (Schloss-/Kartenleser-Lampe, Farbwechsel im Piep-Bild).

### 2.4 Nachtrag 3 / 3b des Nutzers (AUFTRAG.md, Commits 75b08666 / 985f0ad0) — gilt vor dem Wortlaut #9

> Nein, beim Panel in korridor 2 ist falsch - dort soll natürlich rot leuchten - richtig eingegeben - dann blau....
> rot = geschlossen
>
> Es muss schon in der richtigen Farbe leuchten das Panel... hintergrund rot - rot leuchten - hintergrund blau - blau leuchten...

* **"Korridor 2" = ROOM10D0**: Original-Debug-Menue (`include/debug_jump_table.h`, aus DEBUG.BIN erzeugt) Stage 0 Index 13
  (0x0D) = "2F CORRIDOR"; der Raum traegt das Panel mit der ROT und der BLAU gemalten runden Lampe (Cut 11). Zum
  Vergleich: Index 30 (0x1E) "B2 CORRIDOR" = ROOM11E0 und Index 35 (0x23) "B1 CORR. LIGHT" = ROOM1230 tragen die
  SECUGUARD-Kunst (rot / gelb / gruen).
* **Lesart (umgesetzt):** solange zu (Code falsch oder noch nicht eingegeben) leuchtet die ROT gemalte Lampe ROT; sobald der
  Code richtig ist, geht sie aus und die andere gemalte Lampe leuchtet in IHRER Farbe — 10D0/10D1 BLAU, SECUGUARD
  (11E0/11E1/1230/1231) GRUEN; die gelbe SECUGUARD-Lampe bleibt wie gemalt.
* **Das ist jetzt RE2-Mechanik in RE2-Farben** (vorher Farben NUTZER-VORGABE):
  - RE2 ROOM4070 (zwei RUNDE Lampen, links ROT gemalt, rechts BLAU gemalt — dieselbe Anordnung wie 10D0): sub03 zu
    @0x008C6 `64 02 16 04` (Unterindex 0x04 = Strom 4 rund, CLUT-Zeile 0 ROT) auf der roten, offen @0x0088C `64 03 16 0c`
    (Strom 4, Zeile 1 BLAU) auf der blauen Lampe; sub08 @0x00DC2 blau an / @0x00DD2 `65 02` rot aus / @0x00DD4 Piep.
  - RE2 ROOM2130 sub04 (Waffenkammer-Leser, Cut 8, selbst gelesen): @0x01772 `29 08` / @0x01774, @0x01784 `64 0b|0c 16 00`
    ROT / @0x01794 Sleep 30 / @0x01798 Se_on(2,0x0F) Piep / @0x017A4, @0x017A6 `65 0b|0c` rot aus / @0x017A8, @0x017B8
    `64 0d|0e 16 10` GRUEN / @0x017C8 Sleep 30 / @0x017CC `29 04` Kamera zurueck — rot (zu) -> gruen (offen) am Piep, die
    Kamera bleibt dabei auf dem Leser.
  - Gemaltes Glas (Mittel ueber die Farbmaske, Panel-Cut aus dem Port-MDEC-Dekoder) und HSV-Farbton:
    10D0 rot (70,40,32) 13 Grad, 10D0 blau (62,77,95) 213 Grad, SECUGUARD rot (47,10,9) 2 Grad, SECUGUARD gruen (8,51,9)
    121 Grad. RE2-ESP-0x16-Zeilen: 0 rot 356..0 Grad, 1 blau 213..231 Grad, 2 gruen 120 Grad -> jede Lampe bekommt die
    Zeile ihrer gemalten Farbe.
* Mein erster Stand (Commits ca9575f5/8aa2bfbf: blau = falsch auf der blau bzw. rot gemalten Lampe, gruen = richtig auf der
  rot bzw. gruen gemalten) ist damit ueberholt; Mechanik, Kamera-Halt, Lage, Groesse und Zellfolge bleiben.
* Hinweis zum Ablauf: waehrend dieser Spur hat ein zweiter Agent (Orchestrator-Nachbesserung) die drei Code-Dateien im Baum
  bereits auf Nachtrag 3/3b umgestellt (uncommittet, 11:32..11:34; `logf_` mit `%s` kompilierte nicht). Ich habe die
  Aenderung uebernommen, den Uebersetzungsfehler behoben und Tests/Messungen darauf umgestellt.

---

## 3. Umsetzung

| Datei | Inhalt |
|---|---|
| `re15_port/include/re15_codepanel_r37.h` (neu) | Belege (Kopf), Tabelle `re15_cp_panel_t`, Lampen-/Kamera-API |
| `re15_port/engine/src/codepanel_r37.c` (neu) | Tabelle der sechs Raeume, Anker X (18-Byte-Signatur), Kamera-Halt, Lampenzustand + Zellfolge |
| `re15_port/platform/pc/src/codepanel_r37_pc.c` (neu) | ESP-TIM-Dekoder (Zellen 3..7, CLUT-Zeilen 0/1/2) + additiver Zeichner |
| `re15_port/engine/src/scd_vm.c` (Haken) | `#include`; `re15_codepanel_r37_raum_scan(...)` in `scd_register_current_rdt` (1 Zeile + Kommentar); Dispatcher vor dem Opcode: `if (g_re15_codepanel_anker_n) { int cp = re15_codepanel_r37_pc(t); if (cp == 2) break; if (cp == 1) continue; }` (1 Zeile + Kommentar) — Wachposten 0 in allen anderen Raeumen |
| `re15_port/platform/pc/main.c` (Haken) | 1 Zeile + Kommentar hinter den Generator-Lampen: `re15_codepanel_r37_pc_zeichnen(s_last_cut_idx)` (Hintergrund-Ebene, vor 3D/Text) |
| `re15_port/tests/unit/probes/r37_codepanel.cmake`, `tests/unit/probe_r37_codepanel.c`, `tests/integration/test_r37_codepanel.cmake` | Riegel (Abschnitt 5) |
| `re15_port/tools/r37_e/re2_lampen_projektion.py`, `lampen_vorschau.py` | RE2-Projektion der Schlosslampen, Vorschau (Belegwerkzeuge) |

**E2 (Kamera).** Steht der Faden auf X (Cut_old in sub19), werden X..X+3 (`2a 00 3c 01`) uebersprungen; Message_on 5 (X+4)
laeuft im selben Takt (Piep +0 wie bisher). Erreicht sub19 X+10 (`22 01 1b 00`, erst nach dem Schliessen der Nachricht,
Evt_next X+8 unter der Pausemaske 0xffff), laufen die ORIGINAL-Handler `op_cut_old` (0x8004032c) und `op_cut_auto`
(0x800403ac) auf den Original-Bytes X und X+2 — aber erst, wenn die Aufschliess-Folge dieser Nachricht gespielt ist
(Zahl der Toene aus `re15_aufschliessen_r36_folge` = 2: Piep +0, Aufschliessen +15, RE2 ROOM2110 Sleep 15 @0x01B58/@0x01B8C);
bis dahin wartet sub19 an X+10 (Notbremse: letztes Folgebild + 2 Wartebilder). Reihenfolge damit wie RE1.5 ROOM1100
(Nachricht, dann Kamera) und mindestens so lang wie RE2 ROOM2110 (Kamera nach dem Aufschliess-Se_on). Flags, Plaetze,
Tuer, Nachricht unveraendert.

**E1 (Lampe).** Zustand je Bild aus dem Schloss-Flag (3,50)/(3,139)/(3,137) (sub01 Set(3,F,1) im Takt des Evt_exec sub19
= Piep-Takt), nur wenn der angezeigte Cut der Panel-Cut ist. Konstanten:

| Konstante | Wert | Beleg |
|---|---|---|
| Panel-Cut | 11 / 15 / 9 | sub17 @0x018D6/@0x018C0 `29 0b`, @0x01EBE `29 0f`, @0x0139E `29 09` |
| Schloss-Flag | (3,50) / (3,139) / (3,137) | sub01 @0x0152A/@0x01514 `22 03 32 01`, @0x01A8A `22 03 8b 01`, @0x00FF2 `22 03 89 01` |
| X | 0x0199A / 0x01984 / 0x01F82 / 0x01462 | 18-Byte-Signatur, alle sechs RDTs (Riegel anker) |
| Lampenmitten (halbe Pixel) | 10D0 rot (243,153), blau (337,153); SECUGUARD rot (257,349), gruen (361,349) | Glasmasken der Panel-Cuts (§0) |
| Farbzeile zu / offen | 0 rot / 10D0: 1 blau, SECUGUARD: 2 gruen | Nachtrag 3/3b; Unterindex>>3 @0x8001c9e0..0x8001c9fc; RE2 4070 @0x008C6/@0x0088C, 2130 @0x01774/@0x017A8 |
| Zellfolge rund / eckig | 5 (S 0x18), 7, 6, 7 / 3, 4 | ESP-0x16-Anim-Saetze 7..11 `05 01 01 18` .. `07 01 ff 20` / 4..6 `03 01 01 20` .. `04 01 ff 20` |
| Kante rund | 2 px je Texel (Zelle 5: 48 px, 6/7: 64 px) | RE2 4070 Cut 6: S*scale16*H/(SZ*256) (FUN_80077ed0 @0x80077f14..0x8007800c), scale16 0x0900 @0x00DC2+6 -> 31,0 px auf Glas 7x6 bzw. 30,2 px auf 8x6; x Glasverhaeltnis (10/7+10/6)/2 bzw. (12/8+10/6)/2 -> 48 |
| Kante eckig | 26 px | RE2 2130 Schalterlampe 25,8 px auf Glas 17x15 (RE15_PANEL_LAMPE_KANTE-Herleitung) x (14/17+18/15)/2 |
| Mischung | B+F additiv, Index 0 durchsichtig | Flags 0xBA03 & 0x1000 -> Prim 0x2E @0x80077a44..0x80077a50, TPAGE |= 0x20 @0x8001dc48..0x8001dc60 (wie Generator-Lampe) |

---

## 4. Messung nachher

### 4.1 E2 — Kamera (echte exe)

Lauf `n2_10d0_echt` (Stand aee83230, ECHTER WEG: Blue Keycard im Inventar, Kartenleser per RE15_FIRE_AOT=1 -> sub20,
"Ja", "You've used the Blue Keycard.", Tastenfeld, 4-3-1-2 per D-Pad + Viereck, Nachricht 5, Wegwerf-Abfrage):
```
cp.log  F433  lampe ZU: Zeile 0 (0 rot, 1 blau, 2 gruen) Cut 11 Flag (3,50)
        F820  kamera halten: sub19 @0x0199A Cut_old/Cut_auto verschoben (Folge 2 Toene, letzter +15)
        F821  lampe OFFEN: Zeile 1 (0 rot, 1 blau, 2 gruen) Cut 11 Flag (3,50)
        F1011 kamera zurueck: Toene 2/2, gewartet 0 Bilder
auf.log F373 KARTE_EIN / F820 PIEP +0 / F835 AUF_E +15
cut.log F815..F1010 bg=10d0#11 (Panel), F1011 req=0, F1012 bg=10d0#0 (Raum)
```
Ton (RE15_AUDIO_CAP_SYNC, `tools/tueren/r36_cap_ticks.py`, Versatz Tick = F + 123): Ticks 943..957 Anteil 1350..1550 Hz
0,30..0,70 (Piep, F820..F834), ab Tick 958 (F835) Aufschliessen, Spitze 2332 bei Tick 964 (F841) mit 96 % unter 250 Hz.
**Beide Toene kommen, waehrend der Panel-Cut 11 angezeigt wird**; die Kamera geht nach dem Schliessen von Nachricht und
Wegwerf-Abfrage zurueck (vorher: F231 Piep / F232 Raum, §1).

Nachricht frueh geschlossen (Viereck ab dem Piep gehalten, Lauf `n10d0_halten`, Kamera-Logik identisch zum heutigen Stand):
`F231 kamera halten`, Nachricht zu in F245 (sub19 @0x19A3), `F247 kamera zurueck: Toene 2/2, gewartet 2 Bilder`, AUF_E F246
— sub19 wartet bis zum Aufschliessen. Mit Viereck alle 2 Bilder (Lauf `n10d0_mash`) ging die Nachricht erst in F258 zu.

ROOM1230 (`n2_1230`, Tastenfeld per RE15_SUBSTART=17, Ziffern-Riegel per RE15_SET_FLAG_AT): F241 halten + PIEP, F256 AUF_E,
F322 zurueck, F323 Raumansicht.

### 4.2 E1 — Lampen (echte exe + Zeichner-Riegel)

Bild `E_belege/e1_e2_nachher_10d0_1230_falsch.png` (Zeile 1/2: 10D0 echter Weg F440, F818 rot | F822, F836 (Aufschliessen),
F906 ("You've opened the lock."), F950 (Wegwerf-Abfrage), F1010 blau | F1014 Raum; Zeile 3: 1230 F230 rot | F244, F256,
F320 gruen; Zeile 4: 10D0 falscher Code 9999 F300/F500 ("Wrong code, try again.") rot, F536 zu, F540 Raum).

Mittel ueber das gemalte Glas (Riegel zeichner, echter PC-Zeichner auf dem echten MDEC-Hintergrund, vier Bilder = alle Zellen):

| Panel / Zustand | gemaltes Glas | Farbton | leuchtend (Zellen) | Farbton |
|---|---|---|---|---|
| 10D0 zu (rot gemalt) | (70,40,31) | 14 | (241,79,73) / (180,48,47) / (242,64,70) | 2 / 0 / 358 |
| 10D0 offen (blau gemalt) | (62,77,95) | 213 | (122,172,229) / (86,125,208) / (109,161,225) | 212 / 221 / 213 |
| SECUGUARD zu (rot gemalt) | (46,10,8) | 3 | (232,52,50) / (212,29,38) | 1 / 357 |
| SECUGUARD offen (gruen gemalt) | (8,51,9) | 121 | (64,225,65) / (38,199,38) | 120 / 120 |

Die jeweils andere gemalte Lampe bleibt bitgleich. In den Framedumps der exe: Bild 700 (zu) rot gemaltes Glas Mittel
rot-dominant, Bild 900 (offen) blau gemaltes Glas blau-dominant; 1230 Bild 230 rot / Bild 300 gruen (Riegel integration).

Lampenwechsel: Bild nach dem Piep-Takt (F820 Piep -> F821 offen; 1230 F241 -> F242) — die Hintergrund-Ebene wird in main.c
VOR dem VM-Takt gezeichnet (wie Zeiger, Generator-Lampen, Cut-Wechsel: F1011 Cut_old -> F1012 Raumbild).

---

## 5. Tests

Neu (`re15_port/tests/unit/probes/r37_codepanel.cmake`):

| Test | misst |
|---|---|
| `unit_r37_codepanel_anker` | sub19-Schwanz/sub17-Cut/sub01-Flag in allen sechs RDTs; RE1.5 ROOM1100 @0x00CBA (Kamera nach der Nachricht); RE2 ROOM2110 @0x01B5C/@0x01B68/@0x01B6C/@0x01B90/@0x01B9C, ROOM4070 @0x008C6/@0x0088C/@0x00DC2/@0x00DD2/@0x00DD4, ROOM2130 @0x01772..@0x017CC; LAMPE2130.TIM == room2110/esp16.tim, CLUT-Zeilen 0/1/2; Port-Tabelle == unabhaengige Sollzahlen (Cut, Flag, X, Lampenmitten, Farbzeilen, Form) |
| `unit_r37_codepanel_kamera` | E2 headless am ECHTEN Skriptweg aller sechs Raeume (sub17, Code per Cursor + Viereck: 4312 / 0513 / 5632): Piep bei Panel-Cut, Aufschliessen +15 bei Panel-Cut, Kamera erst danach auf den Cut von vorher, Endzustand wie Original; GEGENPROBE ohne Haken (= Befund vorher: Kamera im Piep-Bild weg); Nachricht frueh zu -> sub19 wartet |
| `unit_r37_codepanel_lampe` | E1 headless: rot (zu) vom Panel-Auf bis zum Piep-Bild, offen ab dem naechsten Bild, Lage/Kante/Farbzeile in jedem Bild (0 Abweichungen), nie ausserhalb des Panel-Cuts; falscher Code 9999 -> "Wrong code" bei roter Lampe, nie offen; Zellfolgen; anderer Raum/Cut -> keine Lampe |
| `unit_r37_codepanel_zeichner` | echter PC-Zeichner auf dem echten Hintergrund: leuchtende Lampe in der Farbe des gemalten Glases dominant (>= 30), Farbton <= 20 Grad vom gemalten Glas, heller (>= +100), andere Lampe bitgleich, ausserhalb des Panel-Cuts nichts |
| `integration_r37_codepanel` | echte exe: B 10D0 echter Weg (Kartenleser + Ja + 4312 per D-Pad), C 1230 SECUGUARD, D 10D0 falscher Code — Logzeilen, Piep/Aufschliessen-Bilder gegen cut.log (Panel-Cut am Aufschliess-Bild, Kamera danach), Framedumps (rot/blau/gruen dominant am gemalten Glas) |

### 5.1 Gezielte Laeufe (KEINE volle Suite)

Regex = eigene Tests + Testgruppen der beruehrten Module/Raeume (SCD-VM-Haken: SCD/AOT/Nachrichten/Kamera; Raeume
10D0/1230/11E0; Aufschliess-Toene Runde 36; Wegwerf-Abfrage; Panel/Generator-Lampen; PC-Hauptschleife: Entladen,
headless, Tueren):
```
ctest --test-dir "<baum>/re15_port/build" -R "r37_codepanel|r36_tueren|r21_discard|unit_cut_old_displayed|unit_cam_selfheal|unit_zombie_10d0|unit_msg_dialog|unit_action_msg_gate|unit_scd_key_ck|unit_aot_|unit_scd_aot_opcodes|r34n_c_generator|r31_generator|r26_panel_11f0|r27_panel_schalterwerte|r34n_f_leiche|r35_entladen|integration_keydoor|integration_door_lock|r35_karte_r1230|unit_camera_view|integration_headless_run|unit_espr_11e0|unit_zombie_se_ab_10d0|r19_marke1_10d0" --timeout 600 --output-on-failure -j 3
```
Ergebnis (Stand aee83230 + Dossier, Protokoll `re15_port/build/r37_codepanel/ctest_gezielt.log`):
```
100% tests passed, 0 tests failed out of 62
```
Nach dem letzten Quell-Commit (Kopf-Kommentar in `re15_codepanel_r37.h`) neu gebaut und `-R "r37_codepanel"`:
```
100% tests passed, 0 tests failed out of 5
```
Bank-9-Bits / Nachrichten-Ids / Ereignisse: keine belegt.

---

## 6. OFFEN / Grenzen

* **ROOM11E0/11E1 ("Prisons", Yellow Keycard) ist im normalen Spiel nie offen zu bekommen:** das Schloss-Flag (3,139) setzt
  schon der Neuspiel-Startraum ROOM1240 sub02 @0x0055A `22 03 8b 01` (vgl. include/re15_item_discard.h); main00 nimmt dann
  den Else-Zweig (fertige Tuer), das Tastenfeld erscheint dort nie. Lampe und Kamera sind fuer 11E0/11E1 trotzdem gebaut
  und headless gemessen (Flag geloescht), in der exe aber nur per Haken erreichbar (dort zeigt das Panel sofort "offen").
* **Wegwerf-Abfrage auf dem Panel:** die Runde-21/22-Abfrage "You don't need this key anymore" kommt direkt nach "You've
  opened the lock." und laeuft jetzt noch in der Panel-Ansicht (sub19 laeuft erst nach ihr weiter, erst dann geht die
  Kamera zurueck). Vorher stand sie in der Raumansicht. Logik/Antwort unveraendert (unit_r21_discard_wegwerfen gruen).
* **Nachricht in weniger als 15 Bildern geschlossen (Taste gehalten):** sub19 wartet bis zum Aufschliess-Ton (gemessen 2
  Bilder); in dieser Zeit ist die Eingabe noch gesperrt (Set(2,7,0) folgt erst hinter X+10), die Welt laeuft.
* **Bildversatz +1** fuer Lampenwechsel und Kamera-Rueckkehr gegenueber dem Skript-Takt (Port-Struktur, s. 4.2); RE2 zeigt
  die Lampe im Takt des Piep-Se_on.
* **Hof der runden Lampe** (48/64 px) ist nach RE2 4070 hergeleitet und ueberdeckt die Tasten 1..3 leicht (weicher Schein).
* **Paralleler Schreiber:** waehrend der Spur hat ein zweiter Agent uncommittet in diesem Baum gearbeitet (s. 2.4); der
  Orchestrator ist informiert. Falls er weiterlaeuft, muss vor der Integration abgeglichen werden.

---

## 7. Fuer den Nutzer

* **Was man sieht/hoert:** Code-Panels in "2F Corridor" (Funkraum, Blue Keycard, 4312) und "B1 Corridor" (Waffenkammer,
  Red Keycard, 5632): solange der Code nicht richtig ist, leuchtet die ROTE Lampe rot (auch bei "Wrong code, try again.").
  Ist der Code richtig, geht sie aus und es leuchtet im 2F Corridor die BLAUE, am SECUGUARD-Panel die GRUENE Lampe; die
  Kamera bleibt dabei auf dem Panel — Piep, eine halbe Sekunde spaeter das Aufschliessen, dazu "You've opened the lock."
  (und ggf. die Frage zum Wegwerfen der Karte). Erst danach schaltet die Kamera in den Raum zurueck.
* **Neue Assets:** keine (die Leuchtkunst ist `shared_assets/RE2/LAMPE2130.TIM` = RE2 room2110/esp16.tim, seit Runde 34
  im Paket-Gate; die Toene `shared_assets/RE2/KARTSE.VBS` seit Runde 36).
* **Sprachdateien:** keine.
* **Bau/Paket:** neue Datei `platform/pc/src/codepanel_r37_pc.c` -> Android-GLOB-Cache neu konfigurieren
  (reai-v2-android-glob-cache); RE15_MIN_TESTS +5 (4 unit + 1 integration).
* **Messhaken:** `RE15_CODEPANEL_LOG=<datei>` (Lampenzustand, Kamera halten/zurueck je Bild).
