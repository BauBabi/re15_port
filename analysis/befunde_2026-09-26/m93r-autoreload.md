# Beretta M93R: automatisches Nachladen — der Original-Mechanismus ist vollstaendig belegt, der Port spiegelt ihn Bedingung fuer Bedingung; die M93R-spezifische Ursache ist damit NICHT gefunden

## 1. Was der Nutzer meldet / was ich gemessen habe

Nutzer: "Bei der Beretta M93R klappt das automatische Nachladen noch nicht, wenn
sie leer ist, aber noch Munition im Inventar vorhanden."

Ich konnte in diesem Lauf **nicht spielen und nicht bauen** (Rundenregel 1+2), also
habe ich statisch gemessen: Original-Bytes aus `info/Re1.5/PSX.EXE`, Asset-Bytes aus
`re15_port/shared_assets/PSX/**`, Port-Quelltext. Der einzige Laufzeit-Lauf war
`re15_port/build/tests/unit/probe_r17_munition.exe` (bereits gebaut, nur lesend):

```
=== C: NACHLADEN ist NICHT halbiert (FUN_8004ebdc @0x8004ebdc) ===
  Startausruestung: Waffe slot=1 qty=15, Reserve slot=2 qty=50
  [OK  ] Magazin leer
  nach Nachladen: Magazin=15 Reserve=35
```

Das heisst: der Nachlade-Kern des Ports funktioniert — gemessen, fuer die
BROWNING HP (Id 3). Fuer Id 5 gibt es keine gebaute Sonde.

## 2. Das Original — mit Adressen und Bytes

### 2.1 Der Waffen-FSM-Dispatch @0x80074030 (4 Byte je ITEM-Id)

`re15_disasm.py read 0x80074030 21 --w 4` (selbst gelesen):

| Ids | Zielhandler | Klasse |
|---|---|---|
| 0,1,2 | `0x80034E70` | Nahkampf |
| **3..11, 13, 15..18** | **`0x80032E9C`** | Standard-Schusswaffen-FSM |
| 12, 14, 19 | `0x80034014` | Dauerfeuer |
| 20 | `0x00000000` | feuert nie |

**Die M93R (Id 5) laeuft in der STANDARD-FSM `0x80032E9C`** — derselben wie die
Browning HP. Sie hat keine eigene Waffen-FSM.

### 2.2 Waffen-Eigenschaftstabelle @0x80074da8 (Schrittweite 0xC, Index = ITEM-Id)

`re15_disasm.py bytes 0x80074da8 192` (selbst gelesen):

```
  80074da8: 00 00 00 00 88 4c 07 80 00 00 00 00   id 0
  80074db4: 00 00 00 00 88 4c 07 80 00 00 00 00   id 1  KNIFE
  80074dc0: 00 00 00 00 88 4c 07 80 00 00 00 00   id 2  PIPE
  80074dcc: 0f 00 00 00 8c 4c 07 80 03 01 00 00   id 3  BROWNING HP
  80074dd8: 0f 00 00 00 90 4c 07 80 03 01 00 00   id 4  SIG P228
  80074de4: 0f 00 00 00 94 4c 07 80 03 01 00 00   id 5  BERETTA M93R   <<<
  80074df0: 0f 00 00 00 98 4c 07 80 03 01 00 00   id 6  GLOCK 18
  80074dfc: 06 00 00 00 9c 4c 07 80 03 01 00 00   id 7  SUPER REDHAWK
  80074e08: 07 00 00 00 a0 4c 07 80 03 01 00 00   id 8  REMINGTON M870
```

Wort 0 = Nachlade-Portion (**M93R: 15 @0x80074de4**), Wort 1 = Zeiger auf die
Kombinations-Paarliste, Wort 2 = {+8 Art, **+9 Paar-Anzahl = 1**}.

### 2.3 Die Paarliste der M93R @0x80074c94 — hier steht die Munitionssorte

```
  80074c88: 00 00 00 00      Null-Zeile ("keine Munition")
  80074c8c: 15 03 02 00      id 3 -> Partner 0x15 H.GUN BULLETS, Ergebnis 3, Aktion 2 = NACHLADEN
  80074c90: 15 04 02 00      id 4
  80074c94: 15 05 02 00      id 5 M93R -> Partner 0x15, Ergebnis 5, Aktion 2      <<<
  80074c98: 15 04 02 00      id 6 GLOCK -> Ergebnis 0x04 (die bekannte Glock->SIG-Marotte)
  80074c9c: 17 07 02 00      id 7 -> 0x17 MAGNUM
  80074ca0: 16 08 02 00      id 8 -> 0x16 SHELLS
```

Die M93R zieht **H.GUN BULLETS (0x15)**, dieselbe Sorte wie die Browning.

### 2.4 FUN_8004eb70 @0x8004eb70 — "Reserve im Inventar?" (selbst disassembliert)

```
  8004eb74: lbu  v0,9672(v0)        ; 0x800b25c8 = angelegter SLOT
  8004eb80: sll  v0,v0,2
  8004eb88: addiu at,at,4268        ; 0x800b10ac = Inventar-Array (4 B je Slot)
  8004eb90: lbu  v1,0(at)           ; ITEM-Id des angelegten Slots
  8004eb98: sll  v0,v1,1            ; id*2
  8004eb9c: addu v0,v0,v1           ; id*3
  8004eba0: sll  v0,v0,2            ; id*12
  8004eba8: addiu at,at,19884       ; 0x80074dac = Tabellenbasis + 4 (Zeiger-Spalte)
  8004ebb0: lw   v0,0(at)           ; -> Paarliste (fuer id 5: 0x80074c94)
  8004ebb8: lbu  a0,0(v0)           ; Partner-Id = 0x15
  8004ebbc: jal  0x8004dfec         ; Slot dieser Id suchen
  8004ebc4: sll  v0,v0,24
  8004ebc8: slt  v0,zero,v0         ; >0 ?  <-- SLOT 0 zaehlt als "nicht gefunden"
```

`FUN_8004dfec` @0x8004dfec liefert den Slot-Index sign-extended
(`sll v0,v1,24` @0x8004e028 / `sra v0,v0,24` @0x8004e030) bzw. **-1**
(`addiu v0,zero,-1` @0x8004e048). Das `slt zero,(slot<<24)` @0x8004ebc8 macht daraus:
**Slot 0 -> 0 = "keine Reserve".** Das ist eine echte Marotte des Originals, kein
Port-Fehler (der Port bildet sie in `inventory_common.c:212` nach).

### 2.5 Das Nachlade-Gate im HALTEN-Zustand (Sub 1) @0x80033300-0x80033394

```
  80033300: lw    v0,-14488(v0)        ; 0x800ac768 = Pad-HALTEN (virtuell)
  80033308: andi  v0,v0,0x40           ; Abzug
  8003330c: beq   v0,zero,0x80033394   ; nicht gedrueckt -> raus
  80033314: jal   0x8004ea6c           ; Magazin > 0 ?
  8003331c: beq   v0,zero,0x80033334   ; LEER -> Leer-Zweig
  80033320: ori   v0,zero,0x2          ;   (Delay) Sub = 2 = SCHUSS
  80033328: sh    v0,-13734(at)        ; 0x800aca5a := 2
  ; --- LEER ---
  80033338: lw    v0,-14484(v0)        ; 0x800ac76c = Pad-FLANKE
  80033340: andi  v0,v0,0x40
  80033344: beq   v0,zero,0x80033394   ; nur GEHALTEN -> passiert NICHTS
  8003334c: jal   0x8004eb70           ; Reserve vorhanden ?
  80033354: beq   v0,zero,0x80033384   ; nein -> Leer-Klick
  80033358: lui   a0,0x101             ;   (Delay) SE 0x01010001
  80033360: lbu   v0,-13731(v0)        ; 0x800aca5d = angelegte Waffe
  80033368: sltiu v0,v0,0x9            ; Id < 9 ?
  8003336c: beq   v0,zero,0x80033384
  80033370: ori   v0,zero,0x4          ;   (Delay) Sub = 4 = NACHLADEN
  80033378: sh    v0,-13734(at)        ; 0x800aca5a := 4
  80033384: lui a1,0x800b / addiu a1,a1,-13688 / jal 0x80045024 / ori a0,a0,0x1  ; Leer-Klick
```

**0x800ac76c ist tatsaechlich die PRESS-FLANKE** — es gibt genau EINEN Schreiber,
@0x8003057c, und der rechnet sie so aus (selbst disassembliert):

```
  80030550: xor  v0,v0,a1     ; alt ^ neu
  80030554: and  v0,v0,a1     ; & neu   = neu gedrueckt
  8003057c: sw   v0,-14484(at); -> 0x800ac76c
```

### 2.6 Der Nachlade-Sub 4 @0x80033d7c und sein Abschluss @0x80033ea8-0x80033ed4

```
  80033d90: lbu  v0,0(v1)            ; 0x800aca5b Riegel
  80033d98: bne  v0,zero,0x80033ddc  ; schon laufend -> nur ticken
  80033da0: sb   v0,0(v1)            ; Riegel := 1
  80033da4: ori  v0,zero,0xd
  80033dac: sb   v0,-13592(at)       ; 0x800acae8 := Clip 0x0D  (NACHLADE-CLIP)
  80033dc0: sb   zero,-13591(at)     ; 0x800acae9 := 0 (Bild 0)
  80033dc8: sb   v1,-13597(at)       ; 0x800acae3 := 7 (Crossfade)
  80033dcc: andi v0,v0,0x1fff
  80033dd0: ori  v0,v0,0x4000        ; acaec Elevation -> WAAGERECHT
  ...
  80033ea8: jal  0x8001f314          ; Anim-Stepper
  80033eb0: beq  v0,zero,0x80033ed8  ; Clip noch nicht fertig -> raus
  80033eb4: ori  v0,zero,0x1
  80033ebc: sh   v0,-13734(at)       ; Sub := 1 (HALTEN)
  80033ec0: jal  0x8004ebdc          ; DIE NACHFUELLUNG
  80033ec8: lui a0,0x103 / ori a0,a0,0x1 / jal 0x80045024   ; SE 0x01030001
```

**Es gibt keinen waffenspezifischen Zweig im Nachladen.** Kein `lbu 0x800aca5d`
ausser dem `sltiu 9` @0x80033368.

### 2.7 Vollstaendigkeits-Zensus statt Vermutung

Roh-Byte-Scan ueber `PSX.EXE` nach `jal <ziel>` (Opcode 0x0C, 4-Byte-Raster,
RAM = 0x80010000 + Dateioffset - 0x800):

| Ziel | Aufrufstellen |
|---|---|
| `FUN_8004ea6c` (Magazin>0) | **0x80033314**, 0x80034404 |
| `FUN_8004eb70` (Reserve?)  | **0x8003334c**, 0x80034460 |
| `FUN_8004ebdc` (Nachfuellen) | **0x80033ec0**, 0x80034e44 |
| `FUN_8004eae4` (1 Patrone) | 0x80033888, **0x80033974/7c/84**, 0x80033a3c, 0x80033b18/40/60/80, 0x80033c58, 0x800349c8, 0x80034c00 |

Die jeweils zweiten Stellen (0x80034xxx) liegen in der DAUERFEUER-FSM
`0x80034014` (Ids 12/14/19) und sind fuer die M93R unerreichbar.
**Fuer Id 5 existiert also GENAU EIN Nachlade-Pfad — der aus 2.5/2.6.**

### 2.8 Der M93R-Entladehandler 0x800338A8 (das einzige wirklich Waffenspezifische)

```
  8003396c: jal 0x80011f50    ; EIN Schadens-Resolve
  80033974: jal 0x8004eae4    ; Patrone 1
  8003397c: jal 0x8004eae4    ; Patrone 2
  80033984: jal 0x8004eae4    ; Patrone 3
```

3-Schuss-Burst, **3 Patronen je Abzug**. Magazin 15 (2.2) -> 15/12/9/6/3/**0**:
die Null ist exakt erreichbar, und `FUN_8004eae4` schreibt bei qty==0 gar nicht
(Unterlaufschutz @0x8004eb44-4c). Der Burst kann das Magazin also weder
ueberspringen noch negativ machen.

### 2.9 Die ausgelieferten Daten (Datei-Byte-Offsets)

* **Die M93R liegt in `STAGE1/ROOM11D0.RDT` @0x16C8** (und ROOM11D1 @0x16C8),
  `Item_aot_set` (Opcode 0x50, 22 B — Kodierung `scd_vm.c:3830`):
  ```
  ROOM11D0.RDT +0x16C8: 50 0a 09 31 00 00 00 00 00 00 00 00 00 00 05 00 0f 00 9b 00 ff 00
                        ^op ^slot ^sce ^sat                          ^id=05 ^Menge=0x0f=15  ^tk ^prop=ff
  ```
  **Ausgeliefert mit 15 Schuss** = genau die Nachlade-Portion aus 2.2.
* **Die Animationsbank ist nicht der Grund.** `PL00W05.PLW` selbst geparst
  (Verzeichnis am Datei-Ende, Kopf[0] = Verzeichnis-Offset, EDD @0x8, Cliptabelle
  nach `emd_common.c:55-62`):

  | Bank | Clips | Bildzahlen je Clip |
  |---|---|---|
  | PL00W03 (Browning) | 14 | 22,16,52,1,50,30,10,**23**,1,24,1,24,1,**32** |
  | **PL00W05 (M93R)** | **14** | 22,16,52,1,50,30,10,**22**,1,23,1,23,1,**32** |
  | PL00W09/0A/0B (Granaten) | 13 | Clip 0x0D fehlt — duerfen per `sltiu 9` @0x80033368 aber nie nachladen |

  Clip 0x0D (Nachladen) existiert in W05 und ist **32 Bilder lang**, genau wie bei
  der Browning. Ein "Clip fehlt / Laenge 0"-Haenger scheidet damit aus.

## 3. Was der Port tut — mit datei.c:zeile

| Original | Port |
|---|---|
| `andi 0x40` Abzug HALTEN @0x80033308 | `game_step_common.c:1598` `c->pad_current & RE15_PAD_BIT_SQUARE` |
| `jal FUN_8004ea6c` @0x80033314 | `game_step_common.c:1599` `!re15_ammo_mag_nonzero()` -> `inventory_common.c:164` |
| Pad-FLANKE @0x80033338/44 | `game_step_common.c:1600` `c->pad_pressed & RE15_PAD_BIT_SQUARE` |
| `jal FUN_8004eb70` @0x8003334c | `game_step_common.c:1602` `re15_ammo_reserve_slot() > 0` -> `inventory_common.c:205-213` |
| `sltiu aca5d,9` @0x80033368 | `game_step_common.c:1602` `eq_item < 9` |
| `sh 4 -> aca5a` @0x80033378 | `game_step_common.c:1603` `re15_player_reload_start()` -> `player_common.c:337-346` |
| Clip 0x0D + Crossfade 7 + Elevation waagerecht @0x80033da4-dd0 | `player_common.c:341-345` |
| Abschluss `jal FUN_8004ebdc` + SE 0x01030001 @0x80033ec0-d4 | `player_common.c:1021-1028` |
| `FUN_8004ebdc` Nachfuellung | `inventory_common.c:219-235` |
| Leer-Klick 0x01010001 @0x80033384 | `game_step_common.c:1605` |
| Dispatch-Klasse @0x80074030 | `player_common.c:916-919` (`melee = id<3`, `auto = 12/14/19`) |
| Entladehandler 0x800338A8, 3 Patronen | `game_step_common.c:1650-1655` `ENT[5] = {1,1,3,3,…}` (`ammo = 3`) |
| Munitionssorte 0x15, Portion 15 | `inventory_common.c:189-198` `s_wpn_props[5] = {15,0x15}` |
| Slot-0-Marotte `slt zero,(slot<<24)` @0x8004ebc8 | `inventory_common.c:212` `(as > 0) ? as : 0` |

**Ergebnis der Gegenueberstellung: fuer ITEM-Id 5 ist im Port keine einzige
Bedingung falsch.** Weder eine fehlende Id, noch die falsche Munitionssorte, noch
ein Dauerfeuer-FSM-Zustand (`s_aim_auto` wird in `player_common.c:919` exakt auf
12/14/19 gelatcht, Id 5 ist nicht dabei).

### 3.1 Die drei Zustaende, die GENAU das gemeldete Symptom erzeugen

Ich habe rueckwaerts gesucht: welcher Zustand laesst `re15_ammo_reserve_slot()`
Null werden bzw. das Gate stumm bleiben, obwohl Munition im Beutel liegt?

**(i) Munition in Inventar-Slot 0 — BYTE-TRUE, kein Port-Fehler.**
`inventory_common.c:212` gibt fuer `as == 0` eine Null zurueck, genau wie
`slt zero,(slot<<24)` @0x8004ebc8. Liegen die H.GUN BULLETS in Slot 0, laesst sich
**keine** Handfeuerwaffe nachladen — es kommt nur der Leer-Klick. Slot 0 ist seit
der Wegwerf-Abfrage (Runde 25) erreichbar: `re15_inv_remove_slot()`
(`inventory_common.c:96-104`) ruft `re15_inv_compact()`, und das schiebt
`slots[1] -> slots[0]` (`inventory_common.c:77-82`, byte-true FUN_8004dadc
@0x8004dadc, Schleifenrumpf @0x8004db3c-dba8).

**(ii) Abzug nur GEHALTEN, keine neue Flanke — BYTE-TRUE.**
@0x80033344 `beq v0,zero,0x80033394`: mit gehaltenem Abzug passiert bei leerem
Magazin **nichts**. Der Port macht dasselbe (`game_step_common.c:1600`). Wer nach
dem letzten Burst den Abzug gedrueckt haelt, sieht kein Nachladen — das ist
Original-Verhalten und darf nicht "gefixt" werden.

**(iii) Angelegter Slot >= 11 — ECHTE PORT-ABWEICHUNG.**
Beim Aufsammeln einer BREITEN Waffe (Ids 0x0e..0x13: Flammenwerfer, die drei
Granatwerfer, Raketenwerfer, **H&K MC51 — und die liegt in ROOM1110, also in
STAGE1**) schiebt das Original das Inventar um zwei Zellen und erhoeht den
angelegten Slot **ungeprueft** um 2 (selbst disassembliert):
```
  8004dc70: addiu v0,s0,-14
  8004dc74: andi  v0,v0,0xff
  8004dc78: sltiu v0,v0,0x6         ; Id 0x0e..0x13 ?
  8004dc7c: beq   v0,zero,0x8004debc
  8004dc8c: lbu   v0,0(v1)          ; 0x800b25c8
  8004dc94: addiu v0,v0,2
  8004dc9c: sb    v0,0(v1)
```
Der Port macht denselben +2 (`inventory_common.c:51`) — aber sein
`inv_resolve_slot()` (`inventory_common.c:153-160`) liefert fuer
`s >= RE15_INV_MAX_SLOTS` (=11, `re15_inventory.h:19`) eine **-1**, und daraus
folgt `re15_ammo_mag_nonzero() == 0` UND `re15_ammo_reserve_slot() == 0`: die
angelegte Waffe gilt als leer und laesst sich nicht nachladen. Das Original hat
diese Sperre nicht — es indiziert `0x800b10ac + slot*4` ungeprueft weiter
(`lbu v1,0(at)` @0x8004eb90, `lbu v0,…` @0x8004eacc).
Das ist eine belegte Divergenz mit exakt dem gemeldeten Symptombild. Ob der
Nutzer sie getroffen hat, ist **nicht gemessen**.

## 4. Der Unterschied, in einem Satz

Im Nachlade-Pfad selbst gibt es fuer die M93R **keinen** Unterschied zwischen
Original und Port — der einzige belegte Unterschied liegt daneben: der Port
verwandelt einen (byte-true entstandenen) angelegten Slot >= 11 in "keine Waffe"
und damit in "leer und nicht nachladbar", waehrend das Original weiterindiziert.

## 5. Umsetzungsplan (konkrete Dateien/Zeilen, jede Konstante mit @0x)

**Schritt 1 — MESSEN, bevor irgendetwas geaendert wird (RE-Gate).**
`re15_port/engine/src/game_step_common.c:1580-1588` fuehrt schon den Messhaken
`RE15_WAFFEN_LOG`. Er druckt aber nur `mag=%d` (das Bool aus FUN_8004ea6c).
Vier Felder ergaenzen, alle rein lesend:
* `slot=`  `re15_inv_equipped_slot()` (Spiegel DAT_800b25c8, gelesen @0x8004eb74),
* `res=`   `re15_ammo_reserve_slot()` (Spiegel FUN_8004eb70 @0x8004eb70),
* `resq=`  `g_inv.slots[res].qty`,
* `flanke=` `(c->pad_pressed & RE15_PAD_BIT_SQUARE)` (Spiegel 0x800ac76c @0x80033338).

Damit trennt EIN Nutzerlauf mit der M93R die drei Faelle aus 3.1 eindeutig:
`slot>=11` -> (iii); `res==0` bei `slot<11` -> (i); `flanke==0` -> (ii).
Ohne diese Messung waere jede Aenderung am Gate geraten.

**Schritt 2 — die belegte Divergenz (iii) schliessen.**
`re15_port/engine/src/inventory_common.c:51`
```c
re15_inv_set_equipped_slot(re15_inv_equipped_slot() + 2);   /* @0x8004dc94 addiu v0,v0,2 */
```
Der +2 bleibt (byte-true @0x8004dc8c-9c). Zu aendern ist die PORT-SEITIGE
Interpretation in `inventory_common.c:153-160`: das Original kennt keinen
"-1"-Sonderfall fuer 11/12, es liest `0x800b10ac + slot*4` weiter
(`lbu v1,0(at)` @0x8004eb90 / `lbu` @0x8004eacc). Der Port hat nur 11 Slots
(`re15_inventory.h:19`), muss also entweder das Array auf die Original-Weite
bringen oder den Ueberlauf so behandeln, wie das Original ihn erlebt.
⛔ **Welche der beiden Formen byte-true ist, ist noch NICHT belegt** — dazu muss
zuerst die Groesse von `0x800b10ac` und der Inhalt von `0x800b10d4`/`0x800b10d8`
aus einem sauberen Savestate gelesen werden (`stage_saves/`, NICHT die sechs
`PATCHED-EXE_HASH-881C08B8082E53B6_*.sav`). Vorher keine Zeile Code.

**Schritt 3 — (i) und (ii) NICHT anfassen.** Beide sind byte-true belegt
(@0x8004ebc8 bzw. @0x80033344). Wenn die Messung aus Schritt 1 einen davon zeigt,
lautet die Antwort an den Nutzer "so macht es das Original", nicht ein Fix.

## 6. Offen / nicht belegt

* **Der M93R-spezifische Mechanismus ist NICHT gefunden.** Versucht und
  ausgeschlossen: (a) eigene Waffen-FSM — es gibt keine, Dispatch @0x80074030
  zeigt Id 5 auf 0x80032E9C; (b) eigene Munitionssorte — 0x15 wie die Browning,
  @0x80074c94; (c) fehlende Nachlade-Portion — 15 @0x80074de4; (d) zweiter
  Nachlade-Pfad, den der Port nicht hat — Roh-Byte-Zensus ueber alle vier
  Munitions-Funktionen (2.7) findet keinen; (e) Burst laesst das Magazin nicht
  auf 0 fallen — `FUN_8004eae4` hat den Unterlaufschutz @0x8004eb44, 15 ist durch
  3 teilbar, ausgeliefert mit 15 (ROOM11D0.RDT +0x16D8); (f) Nachlade-Clip fehlt
  in der Bank W05 — PL00W05 hat 14 Clips, Clip 0x0D = 32 Bilder; (g) falscher
  Dauerfeuer-Zustand — `s_aim_auto` latcht exakt 12/14/19 (`player_common.c:919`);
  (h) Equip-Pfad setzt Slot und Id inkonsistent — `menu_common.c:1345-1347` und
  `re15_damage.c:1027-1038` leiten beide aus derselben Quelle ab.
  **Naechster Weg:** der Messhaken aus 5/Schritt 1 in einem echten Nutzerlauf mit
  der M93R (ROOM11D0 aufsammeln, leerschiessen, Abzug LOSLASSEN und neu druecken).
  Ohne diese vier Zahlen ist jede weitere Aussage geraten.
* **Nebenbei belegt (kleine Divergenz, NICHT die Ursache):** `FUN_8004ea6c`
  @0x8004ea6c aufloest die "zweite Haelfte eines 2-Zellen-Gegenstands" (liest das
  FLAGS-Byte `0x800b10ae + slot*4` @0x8004ea84-8c, vergleicht mit 2 @0x8004ea90-94,
  `addiu v0,v0,-1` @0x8004eaa4) und prueft danach auf 0x80 (@0x8004eab4).
  `FUN_8004eb70` @0x8004eb70 macht **weder das eine noch das andere** — es indiziert
  mit dem ROHEN Slot (@0x8004eb74-90). Der Port fuehrt in BEIDEN Funktionen
  dieselbe `inv_resolve_slot()`-Aufloesung (`inventory_common.c:206` bzw. `:164`).
  Fuer die M93R (1 Zelle, flags 0) ergibt das dasselbe Resultat, fuer die breiten
  Waffen 0x0e..0x13 nicht zwingend.
* **Nicht gemessen:** ob `0x800b10ac` im Original mehr als 11 Slots traegt
  (siehe 5/Schritt 2). Der Live-Zaehler ist `0x800b0fbc` (gelesen @0x8004dff0),
  sein Wert im Auslieferungsstand ist in diesem Lauf nicht nachgeschlagen worden.
* **Nicht geprueft:** die Reihenfolge der FX-Spawns des Bursts gegen
  @0x800338ac/f4/28 — das ist Gegenstand des Runde-17-Dossiers
  `analysis/befunde_2026-09-20/waffen-loop.md`, nicht dieses Befunds.
