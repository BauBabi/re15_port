# Runde 37 — Spur E "codepanel" (Dossier, fortlaufend)

Baum `.claude/worktrees/r37_codepanel` (Zweig `r37/codepanel`, Basis master f1e019ca). Auftrag: `AUFTRAG.md` #9.
Belegbilder: `analysis/befunde_runde37/E_belege/`. Messlaeufe: `re15_port/build/r37_codepanel/runs/<name>/`
(Messkopie `re15_pc_r37e_mess.exe`, Laeufer `re15_port/build/r37_codepanel/lauf.sh`, nicht versioniert).

## Wortlaut (AUFTRAG.md #9, unveraendert)

> bei den Code panels möchte ich das, so lange die Code noch falsch eingegeben ist unten das blaue Licht leuchtet.
> Ist er dann richtig eingegeben das grüne Licht. Die Kamera darf noch auf den Panels bleiben wenn der Piep und
> unlock sound kommt.

Punkte:
- **E1** Lampe am Code-Panel: blau, solange der Code (noch) nicht richtig eingegeben ist; gruen, sobald er richtig ist.
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
