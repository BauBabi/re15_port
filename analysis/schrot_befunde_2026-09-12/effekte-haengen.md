# BEFUND A — „Schrotflinten-Effekte fliegen auf der Stelle rum und bleiben da"

Datum: 2026-09-12. Nutzer-Report: „die Shotgun Kugeln/Effekte fliegen dann lustig auf der
Stelle rum und bleiben da."

**Wurzelursache (bewiesen): Row-Routine 38 (Schrothuelsen-Init, Original @0x800188b8, Dispatch-
Tabelle @0x80071d40[38]) fehlt im Port-Dispatch `esp_fx_dispatch` (re15_esp.c:505-642, `default:
break` @ Zeile 640).** Die Schrothuelse (CORE00.ESP id 4, sub 3, Row @0x1a08) traegt als
Routine-A-Selector 38; der Port behandelt sie als Noop. Der Slot bleibt dadurch fuer immer auf
Row 0 stehen: Geschwindigkeit (0,0,0), Beschleunigung (0,0,0), Flags 0x03 (sichtbar), und die
Anim-Tabelle von id 4 LOOPT endlos (Records [0..7] dur=1, Record [8] dur=0xFF → Loop auf 0) —
es gibt keinen Anim-Terminator, der Despawn kommt im Original ausschliesslich ueber die
Row-Kette 38 → 16 → Row 1 (Routine 11) → Boden-Bounce B=12 (2. Bodenkontakt = Kill). Ohne 38
wird Row 1 nie erreicht → pro Schuss bleibt eine an Ort und Stelle rotierende Huelsen-Sprite
dauerhaft in der Luft haengen. Exakt das gemeldete Symptom.

---

## 1. IST-Zustand des Ports (re15_port/engine/src/re15_esp.c)

- Row-VM-Dispatch Routine A: `esp_fx_dispatch` (re15_esp.c:505-642). Implementiert:
  **0, 3, 4, 5, 8, 9, 10, 11, 16, 17, 18**. Alles andere faellt in `default: break;`
  (re15_esp.c:640, Kommentar „stage-3c selectors: noop for now").
- Row-VM-Dispatch Routine B: `esp_fx_dispatch_b` (re15_esp.c:654-666). Implementiert NUR
  **12** (Boden-Bounce/Kill); jeder andere B-Wert = Noop. (CORE00.ESP nutzt als B-Wert in den
  Datei-Rows ausschliesslich 0 — B=12 wird zur Laufzeit von Routine 11 @0x80017774 bzw. B=28/29
  von Routinen 27/30 in die Row-KOPIE geschrieben.)
- Verhalten bei UNBEKANNTER Routine A: Noop → die Row wird nie fortgeschaltet, Flags bleiben
  auf dem Spawner-Init 0x03 (aktiv|sichtbar, re15_esp.c:708), die Physik integriert die
  Datei-Seeds (fuer id4-sub3-Row0: alles 0 → steht still), der Anim-Timer laeuft weiter
  (re15_esp.c:842-862) und despawnt NUR bei Terminator-Record (dur==0 && loop==0).
- sub-3-Basen: `re15_esp_row_stream` (re15_esp.c:212-235) liest die 8-Eintraege-u16-Subtabelle
  am Rowblock, `base = rowblk + lhu(rowblk + (sub&7)*2)*4`. CLUT-Seed `(sub>>3)*0x40` in
  `esp_fx_seed_header` (re15_esp.c:679-688) — byte-true zu FUN_80019700.
- Spawn-Stelle der Schrotflinten-FX: `game_step_common.c` Entlade-Tabelle
  `ENT[8] = {{2,3,0x0F00,...},{3,0,0x1400,...},{4,3,0x0920,...}}` (@Zeilen ~1434-1436, Belege
  @0x80033a5c-a4 / @0x80033aa8-c8 / @0x80033acc-f8), gespawnt via `re15_esp_fx_spawn_rows`
  (game_step_common.c:1504/1508). ENT[13] (SPAS-12) nutzt dieselbe Huelse {4,3,0x0920}
  (@0x80033c08-38).

## 2. Original-Beleg

### 2.1 Dispatch-Tabelle @0x80071d40 (PSX.EXE, t_addr 0x80010000, Datei-Offset 0x800+va−t_addr)

48 Eintraege; relevant: [38] = **0x800188b8**, ausserdem [15]=0x80017ac8, [16]=0x80017b80,
[26]=0x800180b0, [27]=0x8001810c, [30]=0x8001843c, [39]=0x80018918.

### 2.2 Routine 38 KOMPLETT (ghidra1_V2.txt, 0x800188b8–0x80018914)

```
800188b8 lui   v1,0x800b
800188bc lbu   v1,DAT_800aca5d        ; angelegte Waffe (Inventar-Item-Id)
800188c0 ori   v0,zero,0x8
800188c4 bne   v1,v0,LAB_800188dc     ; Waffe != 8 (Remington M870)?
800188c8 _ori  v0,zero,0x3            ;   (Delay-Slot) v0 = 3
800188cc lui   v1,0x800b              ; Waffe == 8:
800188d0 lw    v1,DAT_800b52c4        ;   v1 = aktueller Slot (VM-Cursor)
800188d4 j     LAB_800188e4
800188d8 _ori  v0,zero,0x17           ;   v0 = 0x17 (23)
800188dc lui   v1,0x800b              ; Waffe != 8:
800188e0 lw    v1,DAT_800b52c4        ;   (v0 bleibt 3)
800188e4 nop
800188e8 sh    v0,0x16(v1)            ; row[0x16] := 0x17 bzw. 3   (Haltedauer)
800188ec lui   v1,0x800b
800188f0 lw    v1,DAT_800b52c4
800188f8 lbu   v0,0xe(v1)             ; v0 = row[0x0e] = 0x67
80018900 sb    v0,0x6c(v1)            ; slot+0x6c (Flags) := 0x67
80018904 lui   v1,0x800b
80018908 lw    v1,DAT_800b52c4
8001890c ori   v0,zero,0x10
80018910 jr    ra
80018914 _sh   v0,0x0(v1)             ; row[0x00] (Routine A) := 16
```

Semantik: **Ein-Schuss-Init.** Liest die angelegte Waffe (DAT_800aca5d); schreibt die
Haltedauer in die Row-Kopie (`row[0x16] := 0x17` bei Schrotflinte 8, sonst 3 — die SPAS-12
(Item 13) faellt byte-true in den 3er-Zweig), setzt die Slot-Flags auf `row[0x0e]` = **0x67**
(aktiv 0x01 | sichtbar 0x02 | FOLLOW 0x04 | Physik-Freeze 0x20 | Anim-Freeze 0x40) und
schaltet den Selector auf **16** um. KEIN Row-Advance, KEIN Countdown in diesem Tick.

Ab dem naechsten Tick laeuft Routine 16 (@0x80017b80, im Port vorhanden, re15_esp.c:540-553;
Disasm gegengeprueft — Port ist byte-aequivalent): 0x17 = 23 Ticks Freeze-Hold (Flags jeden
Tick := row[0x0e] = 0x67), dann Release: Flags := row[0x1e] = 0x3, Anim-Index +0x6e :=
row[0x26] = **9** (die Schrothuelsen-Frames: Records [9..16], Loop-Record [17] → 9), Advance
auf Row 1. Row 1 (@0x1a30) = Routine **11** (@0x80017718, im Port, re15_esp.c:554-562):
RNG-Streuung auf vel (−18,−30,−100), dann `A := 0, B := 12`; B=12 (@0x8001779c, Port
`esp_fx_dispatch_b`) macht Bounce+Clink beim 1. und **Despawn beim 2. Bodenkontakt**.
Gesamt: 1 Tick Init + 23 Ticks Hold + 1 Tick Release ≈ 25 Ticks bis zum Auswurf =
die Pump-Action-Zeit.

### 2.3 Die Rowbloecke aus CORE00.ESP (shared_assets/PSX/DATA/CORE00.ESP, 0x1bb4 Bytes)

Global-Bank-Parse (FUN_8001923c): id-Header @0 = `03 08 00 02 04 FF`; Pointer-Tabelle
abwaerts ab round_up4(size)−4. Ergebnis: id2 eff_start=0xF00 (count_a=17, count_b=14)
→ **rowblk 0x0FC8**; id4 eff_start=0x1728 (count_a=36, count_b=28) → **rowblk 0x18C0**.
(Beide bestaetigen die SPEC-Angaben.)

**id 4 (Huelse), rowblk 0x18C0** — Subtabelle (u16-Woerter) `04 1A 30 50 66 7C 92 9E`
→ sub-3-Basis Datei **0x1A00**, 1 Stream, 2 Rows:

| Row | @Datei | A | B | w/h | accel | p0e | vel | p16 | p1e | eul | p26 |
|---|---|---|---|---|---|---|---|---|---|---|---|
| 0 | 0x1A08 | **38** | 0 | 1/1 | (0,0,0) | 0x67 | (0,0,0) | 0x18 | 0x3 | (0,0,0) | 0x9 |
| 1 | 0x1A30 | 11 | 0 | 4096/4096 | (0,10,0) | 0x1 | (−18,−30,−100) | 0x1 | 0x0 | (0,0,0) | 0x0 |

(Hinweis: p16=0x18 in Row 0 ist der DATEI-Seed; Routine 38 UEBERSCHREIBT row[0x16] zur
Laufzeit mit 0x17/3 — der Dateiwert wird nie konsumiert.)

**id 2 (Muendung), rowblk 0x0FC8** — Subtabelle `04 39 6E 8E 102 118 12E 1A2` → sub-3-Basis
Datei **0x1200**, **5 Streams** (der Grossblitz):

- Stream 0 (3 Rows @0x1208): A=8 (Flags 0x93, TPAGE|=0x20, Kind-Spawn cat2/sub=row[0x1e]=**5**/
  scale=row[0x26]=0x10C8) → A=9 (positionsgebundener BANG) → A=0.
- Streams 1–4 (je 2 Rows @0x1284/0x12D8/0x132C/0x1380): A=10 (Flags 0x93, TPAGE|=0x20,
  Anim-Index 0, euler.y 820/1640/2456/3276 = der Faecher) → A=0.
- Kind-Effekt id2 **sub 5** (@0x1430): A=10 (p26=0xA → Anim ab Record 10) → A=0.

**Alle Muendungs-/Rauch-Selectors (8, 9, 10, 0) sind im Port implementiert** und die
Anim-Tabellen terminieren (id2: Terminator-Records [5]/[9]/[13]; id3 Rauch: Terminator [23]).
Muendung + Rauch despawnen im Port korrekt — **nur die Huelse haengt.**

### 2.4 Warum genau „steht und bleibt": id4-Anim-Tabelle (Datei 0x1730 ff.)

Records [0..7] = dur 1 (Pistolenhuelsen-Drehframes 0x100–0x107), Record [8] = `param 0x10FF`
= **Loop → Record 0**. Der Port-Tick (re15_esp.c:842-862) despawnt nur bei dur==0&&loop==0 —
diesen Record erreicht die Huelse nie. Mit A=38→Noop: Position eingefroren am Spawnpunkt
(vel/accel 0), 8-Frame-Drehschleife endlos, ein Slot pro Schuss → „fliegen lustig auf der
Stelle rum und bleiben da".

## 3. Weitere fehlende Routinen der beiden Rowbloecke (Vollinventar)

CORE00.ESP-Selector-Inventar gegen den Port-Dispatch (fehlend = {15, 26, 27, 30, 38, 39}):

| Routine | Original | Vorkommen | Port heute erreichbar? |
|---|---|---|---|
| **38** | @0x800188b8 (oben) | id4 sub3 row0 | **JA** — ENT[8] Remington + ENT[13] SPAS: DER BUG |
| 15 | @0x80017ac8 | id2 sub2 (3 Rows), id4 sub2 (3 Rows), id3 sub2 (3 Rows) | JA — ENT[5]/[6] (M93R/Glock, FX-SUB 2 @0x800338ac/f4/28). Unsichtbarer Burst-Emitter: Flags := 0x65 (@0x80017adc, OHNE sichtbar-Bit); solange row[0x0e]≠0: row[0x0e]−− (@0x80017afc-b0c); bei 0: Spawn via FUN_800199d4(cat=row[0x16]>>8, sub=row[0x16]&0xFF, scale=slot[0x72], param=slot[0x2e], Matrix slot+0x74, Pos slot+0x40) (@0x80017b10-3c), dann Advance falls row[0x26] sonst `sb zero,0x6c` = Selbst-Despawn (@0x80017b4c-6c). id4 sub2 = 3 Huelsen im 2-Tick-Takt (der Burst); OHNE 15 haengt der id4-sub2-Slot heute genauso endlos (gleiche Loop-Anim [8]), id2-sub2 despawnt zufaellig ueber den Anim-Terminator [5], zeigt aber falsche Frames. |
| 26 | @0x800180b0 | id2 sub7 row1 | Nein (kein Port-Spawner nutzt id2 sub7). Positions-SE `FUN_80045024(0x020A0001, &slot+0x28)` + Noise-Latch DAT_800b5358:=1 + Advance (@0x800180c4-f4) — die 0x020A-Variante von Routine 9. |
| 27 | @0x8001810c | id4 sub4 row1, id4 sub7 row1 | Nein (kein Port-Spawner nutzt id4 sub4/7). Wie Routine 11 (RNG: drift.x−=rand&0xA, drift.y−=rand&0x14, drift.z+=rand&0x14), aber `B := 0x1C` (28) statt 12 und `row[0x0e] := 1` (@0x80018128-84); braucht zusaetzlich Main-Loop-Routine B=28 (@0x80018198). |
| 30 | @0x8001843c | id4 sub5 row0 | **JA** — Granatwerfer-Projektil (4,0x0D) @game_step_common.c:1628. Init: +0x6e:=0x17, Flags:=3, `B:=0x1D` (29), A:=0, row[0x1e]:=0x2A; Ballistik nach Elevations-Bits DAT_800acaec (0x8000→vel(0x17C,−0x6E,0x15)/accel_x −2; 0x4000→(0x118,−0x32,0x18)/−1; 0x2000→(0x50,0,1)/−1) + rand%4+7 in row[0x26] (@0x80018484-538). OHNE 30 haengt heute auch das Granaten-Projektil am Lauf (id4-Anim-Loop, A-Noop). Braucht zusaetzlich B=29 (@0x80018320). |
| 39 | @0x80018918 | id4 sub6 row0 | Teils — der Speedloader-Drop (W7-Reload) spawnt id4 sub6 derzeit ueber `re15_esp_fx_spawn_ex` OHNE Row-VM (player_common.c:947) → loopt ebenfalls endlos (kein Terminator). Routine 39 ist eine mehrphasige FSM (eigenes Phasenregister in row[0x0e]: 0/1/2 @0x80018960-8c; Phase 0: Flags:=1, Kind-Spawn `FUN_800199d4(0x400xxxx)` an slot+0x40+{0x19,0,0}, schreibt 3D-Sprite-Tabellen DAT_800a73b8/DAT_800a7424 @0x800189c4-a54; 253 Instruktionen bis 0x80018d08). Eigene Baustelle (Gruppe Revolver-Reload), NICHT Teil des Huelsen-Fixes. |

## 4. PLAN — exakte Implementierung (BEFUND A)

**Fix = Routine 38 in `esp_fx_dispatch` (re15_port/engine/src/re15_esp.c, Switch @505, vor
`default:` @640) nachziehen.** Alles Nachgelagerte (16 → 11 → B=12) ist bereits byte-true
vorhanden und gegen das Disasm verifiziert.

```c
case 38: {  /* @0x800188b8 (Dispatch @0x80071d40[38]): Schrothuelsen-INIT.
             * lbu DAT_800aca5d; ==8 -> 0x17, sonst 3 (@0x800188c0-d8);
             * sh row[0x16] (@0x800188e8); Flags := row[0x0e] = 0x67 (@0x80018900);
             * row[0x00] := 16 (@0x80018914). KEIN Advance, KEIN Countdown hier —
             * ab dem naechsten Tick laeuft Routine 16 (Hold), Release setzt Anim 9
             * und schaltet auf Row 1 (Routine 11 -> B=12 Bounce/Despawn). */
    extern int re15_player_equipped_weapon(void);   /* DAT_800aca5d (re15_damage.c) */
    uint16_t hold = (re15_player_equipped_weapon() == 8) ? 0x17 : 0x3;
    f->row[0x16] = (uint8_t)hold; f->row[0x17] = (uint8_t)(hold >> 8);
    f->flags = f->row[0x0e];                        /* 0x67: FOLLOW|Freeze|AnimFreeze */
    f->row[0x00] = 16; f->row[0x01] = 0;            /* Selector A := 16 */
    break;
}
```

Schritte:
1. `re15_port/engine/src/re15_esp.c`: obigen `case 38` einfuegen (nach `case 18`, Zeile ~639).
   Der `extern` steht lokal im Case (wie die bestehenden externs in game_step_common.c) oder
   oben neben `re15_engine_rand8` (re15_esp.c:18).
2. Dokumentierte faithful-line-Abweichung: Flags-Bit 0x04 (FOLLOW) laesst die Huelse im
   Original waehrend der 23 Hold-Ticks an der Gun-Bone-Matrix (slot+0x74, Kopie je Tick
   @0x80019f68-f94) mitreiten; der Port hat fuer Waffen-FX keinen Matrix-Anker
   (`follow_slot = -1`) → die Huelse haelt am Spawnpunkt. Positionsdelta ≈ die Pump-Bewegung
   der Hand, Verhalten (Freeze → Auswurf → Bounce → Despawn) identisch. Im Code-Kommentar
   festhalten.
3. Test (ctest, re15-room-probe-Stil): Global-Bank laden, Waffe 8 setzen,
   `re15_esp_fx_spawn_rows(gb, 4, 3, 0x920, x,y,z, floor_y, 0)`; ticken und pinnen:
   (a) nach Tick 1: row[0x00]==16, Flags==0x67, row[0x16]==0x17;
   (b) Ticks 2..24: Position unveraendert (Freeze), Frame==0 (Anim-Freeze);
   (c) Tick 25 (Release): Flags==0x03, Anim-Index → 9, Row-Cursor==1;
   (d) Tick 26: Routine 11 gelaufen (row[0x00]==0, row[0x02]==12, drift RNG-gestreut um
       (−18,−30,−100));
   (e) nach 2. Bodenkontakt: Slot inaktiv (Despawn). Gegenprobe Waffe 13 (SPAS): hold==3.
4. Visuelle Verifikation NUR per gdigrab (Skill re15-port-visual-verify): Schrotflinte im
   Spiel feuern — Huelse friert kurz, fliegt dann nach rechts-hinten weg, klimpert, despawnt;
   KEINE stehenbleibenden Sprites mehr nach mehreren Schuessen.
5. Folge-Befunde (NICHT Teil dieses Fixes, eigene Gruppen): Routine 15 (M93R/Glock-Burst-FX,
   id2/id4 sub2 — id4-sub2-Slot haengt heute identisch), Routine 30+B29 (Granatwerfer-
   Projektil haengt heute identisch), Routine 39 (Speedloader — Port-Spawn ausserdem noch
   spawn_ex statt spawn_rows), Routine 26/27+B28 (derzeit unerreichbar). Jeweils Adressen in
   §3.

## Belege (Kurzliste)

- Port-Dispatch + default-Noop: re15_port/engine/src/re15_esp.c:505-642 (default @640);
  B-Dispatch nur 12: re15_esp.c:654-666; Spawner-Flags 0x03: re15_esp.c:708.
- Routinen-Tabelle: PSX.EXE @0x80071d40 (t_addr 0x80010000, Datei-Offset 0x62540), [38]=0x800188b8.
- Routine 38: ghidra1_V2.txt 0x800188b8–0x80018914 (vollstaendig oben); Waffen-Byte DAT_800aca5d;
  VM-Slot-Cursor DAT_800b52c4.
- Routine 16 (Hold): @0x80017b80–0x80017bfc, Port re15_esp.c:540-553 byte-aequivalent.
- Routine 11 (Auswurf-RNG): @0x80017718 (B:=12 @0x80017774, A:=0 @0x80017780), Port re15_esp.c:554-562.
- B=12 Bounce/Kill: @0x8001779c (Kill 2. Kontakt @0x800178b0), Port re15_esp.c:654-666.
- CORE00.ESP: shared_assets/PSX/DATA/CORE00.ESP; id4 rowblk 0x18C0, sub3 @0x1A00, Rows @0x1A08/0x1A30;
  id2 rowblk 0x0FC8, sub3 @0x1200 (5 Streams); id4-Anim-Loop Record [8] (Datei 0x1730+8*8);
  id2-Anim-Terminatoren [5]/[9]/[13]; id3-Terminator [23].
- Spawn-Stellen Port: game_step_common.c ENT[8]/[13] (~1434-1452, Belege @0x80033a5c-f8 /
  @0x80033c08-38), Granate (4,0x0D) game_step_common.c:1628, Speedloader player_common.c:947.
