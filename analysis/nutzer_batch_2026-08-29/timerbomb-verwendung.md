# RECON — ROOM1110 „Timer bomb": Gibt es einen Verwendungsort?

**Datum:** 2026-08-29 · **Status:** GEFUNDEN — die Bombe HAT einen Verwendungsort: **ROOM2040/2041 (STAGE2, Kanalisation, die verrostete Tür)**.
**Kernbefund in einem Satz:** Die Zeitbombe ist im Auslieferungsstand **KEIN Inventar-Item** — Aufnahme und Verwendung laufen komplett über **Messages + Flag-Bank 3** (nehmen = Bit 0x92 in ROOM1110, zünden = Bit 0x35 in ROOM2040); das im DEBUG.BIN benannte Inventar-Item id 0x35 „Timer Bomb" wird von keinem einzigen der 240 RDTs jemals vergeben.

Methode: `re15_port/tools/aot_sce_census.py` (unverändert, 240 RDTs, **100.00 % SCD-Coverage, 0 Desync-Stopps**) + erweiterte Scratchpad-Kopie (Flag-Op-Sweep 0x21/0x22 über alle RDTs, 4912 Flag-Ops), Voll-Disassembly der ROOM1110/2040-SCDs, Message-Sektionen dekodiert (Glyph-Map aus `analysis/nutzer_batch_2026-08-27/item-name-feuerloescher.md` §3e, dort maschinell verifiziert), EXE-Disasm via `re15_disasm.py`.

---

## 1. Die Item-ID — und warum sie hier gar nicht benutzt wird

### 1a. Namensbank: id 0x35 = „Timer Bomb"

`re15_port/shared_assets/PSX/BIN/DEBUG.BIN` (mappt @0x800c0000): Offset-Tabelle @0x495c, `off[0x35] = 0x2ed` → Blob-Datei-Offset **0x4d15**, Bytes:

```
30 45 49 41 4e 00 1e 4b 49 3e   =  T i m e r _ B o m b
```

### 1b. ROOM1110s sce-9-Items (Census, `ROOM1110.RDT`, main00) — KEIN 0x35 darunter

| Datei-Offset | Slot | Typ | Name | Menge | taken-Bit (Zone 9) | Prop |
|---|---|---|---|---|---|---|
| 0x0b02 | 6 | 0x16 | Shotgun Shells | 14 | 0xe6 | 0 |
| 0x0b18 | 7 | 0x15 | H. Gun Bullets | 30 | 0xe7 | 1 |
| 0x0b2e | 8 | 0x38 | Blue Keycard | 1 | 0x34 | 2 |
| 0x0b44 | 9 | 0x22 | First Aid Spray | 1 | 0x8f | kein (0xff) |
| 0x0b5a | 10 | 0x13 | H&K MC51 | 100 | 0x62 | 3 |

(ROOM1111 = byte-gleiche Records an denselben Offsets.)

### 1c. Die Bombe im Raum ist ein MESSAGE-Pickup, kein Item

`ROOM1110.RDT`, messageStart @0x3C = 0xd54, 10 Messages. Dekodiert:

* **msg 8 @Datei 0x0f46**: `04 02 33 45 48 48 00 55 4b 51 00 50 3d 47 41 00 50 44 41 00 05 01 30 45 49 41 00 1e 4b 49 3e 05 00 1b 03 02 01 00`
  = **„Will you take the Time Bomb?"** (`{03}{02}` = Yes/No-Halt; Name steht als LITERAL-Glyphen im Text — **kein** `{06}`-Item-Name-Insert, keine Item-ID beteiligt. Schreibweise hier „Time Bomb", die Namensbank sagt „Timer Bomb".)
* **msg 9 @Datei 0x0f6c**: **„You've taken the Time Bomb."**

SCD (Voll-Walk, Auszug):

```
sub00 @0x0ccc: 06 00 1a 00              If
      @0x0cd0: 21 03 92 00              Ck(Bank 3, Bit 0x92, ==0)      ; Bombe noch nicht genommen
      @0x0cd4: 2c 14 03 31 00 00 88 c8 1e fb 20 03 e8 03 ff 00 18 03 00 00
               Aot_set Slot 20, sce 3 EVENT, ACTION-gegatet (sat 0x31),
               Rect x=-14200 z=-1250 w=800 d=1000  → sub03
sub03 @0x0d24: 2b 08 ff ff              Message_on(8)   "Will you take the Time Bomb?"
      @0x0d2a: 06 00 1a 00 / 0x0d2e: 21 0c 1f 00   If Ck(Bank 0x0c, Bit 0x1f, ==0)  ; = YES
      @0x0d32: 22 03 92 01              Set(Bank 3, Bit 0x92, 1)       ; "genommen"-Latch
      @0x0d36: 46 14 00 00 ...          Aot_reset(20, sce 0)           ; Zone tot
      @0x0d40: 2b 09 ff ff              Message_on(9)   "You've taken the Time Bomb."
```

**Yes/No-Beleg:** Ck-Bank-Tabelle @0x80074664 (16 Zeiger, aus `PSX.EXE` gedumpt): Bank 0x0c → **0x800b8520** = das Message-State-Wort; dessen Bit 0 = „No gewählt" (belegt in `analysis/nutzer_batch_2026-08-26/item-modell-weg.md` §1b, `@0x800287b0 andi v0,v0,0x1`). Ck testet `0x80000000 >> (idx & 0x1f)` (Referenz-Zeile `RE15_SCD_OPCODES_REFERENCE.md:88`; dasselbe srlv-Muster im sce-4-Handler @0x80043150-0x80043180), idx 0x1f → Bit 0. `Ck(0x0c,0x1f,==0)` = „No-Bit ist 0" = **YES**.

**Es gibt kein Bomben-Prop-Modell:** die 4 `Obj_model_set` des Raums (@0x0b70/0x0b92/0x0bb4/0x0bd6) sind über die tk_prop-Bytes 0/1/2/3 vollständig an die 4 Item-Pickups mit Modell vergeben. Die sichtbare Bombe liegt also im vorgerenderten Hintergrund (HYPOTHESE nur bzgl. „im BG sichtbar"; belegt ist: kein 3D-Prop im SCD).

Die Bank-3-Zuordnung: @0x80074664[3] = **0x800b0ff8** (Story-Flags, Memory `reai-v2-stage1-flag-map` „z3 Story").

---

## 2. Der Verwendungsort: ROOM2040/2041 (STAGE2) — game-weit der EINZIGE Leser

Game-weiter Sweep ALLER `Ck`/`Set` (0x21/0x22) über alle 240 RDTs (4912 Flag-Ops erfasst), Filter Bank 3 / Bit 0x92:

```
ROOM1110 sub00 @0x0cd0: Ck  ==0   (Prompt-Wache, s.o.)
ROOM1110 sub03 @0x0d32: Set  =1   (das Nehmen)
ROOM1111 sub00/sub03            (byte-gleich)
ROOM2040 sub00 @0x141a: Ck  ==0   ← DER VERWENDUNGSORT
ROOM2041 sub00 @0x141a: Ck  ==0   (SCD- und Message-Sektion byte-identisch zu 2040)
```

### 2a. Der Mechanismus in ROOM2040 (`ROOM2040.RDT`, sub-SCD @0x1404)

```
sub00 @0x1416: If  @0x141a: Ck(3, 0x92, ==0)          ; Bombe NICHT im Besitz:
      @0x141e:   Aot_set Slot 8, sce 1 MESSAGE msg 0   ;   nur der Aushang (Rect x=4050 z=-6600 w=2000 d=1000)
      @0x1432-42: Sca_id_set(0..4, 0x13, 0xff)         ;   Kollision "Tür zu"
      @0x1446:   Cut_replace(0x0d, 0x02)
      @0x144a: Else
      @0x144e: If  @0x1452: Ck(3, 0x35, ==0)           ; Bombe im Besitz + noch nicht gezündet:
      @0x1456:   2c 08 03 31 03 00 d2 0f 38 e6 d0 07 e8 03 ff 00 18 07 00 00
                 Aot_set Slot 8, sce 3 EVENT, ACTION-gegatet → sub07   (gleiches Rect)
      @0x146a:   Aot_set Slot 9 sce 5; @0x147e Obj_model_set Prop 0; Sca_id wie oben; Cut_replace
      (beide Ck falsch = Tür bereits gesprengt → weder Message noch Sperr-Kollision)

sub07 @0x16e8: Message_on(0)                           ; der Aushang
      @0x16ee: Message_on(5)                           ; "Will you use the Time Bomb?"  (Yes/No)
      @0x16f4: If  @0x16f8: Ck(0x0c, 0x1f, ==0)        ; YES
      @0x16fc:   29 0e                                 ; Cut/Montage-Display-Switch (Opcode 0x29 @0x800402a0,
                                                       ;   RE15_SCD_OPCODES_REFERENCE.md:96)
      @0x16fe-0x1712: Set(5, 0..5, 1)                  ; Event-Bank scharf (Bank 5 → 0x800b1028)

sub01 (läuft PER FRAME, Memory reai-v2-scd-per-frame-model):
      @0x1570: If @0x1574: Ck(5, 4, ==1)               ; von sub07-YES gesetzt
      @0x1578:   Work_set(3,0)                          ; 2e 03 00
      @0x1580:   Member_cmp  3e 00 0f 00 09 00          ; Nähe-/Zustands-Check
      @0x158a:   Key-Ck      51 01 40 00                ; ACTION-Taste
      @0x158e:   04 ff 18 02  =  Evt_exec → sub02      ; DIE ZÜNDUNG
      (Bits 0..3 + gehaltene Richtungstasten → Evt_exec sub03/04/05/06 = Speed_set/Add_speed-
       Mikro-Skripte @0x16b8-0x16e6 — Positionier-Interaktion)

sub02 @0x1604: löscht Bank-5-Bits 0..5, Bank-2-Bits 0/2
      @0x1624: 22 03 35 01                             ; Set(3, 0x35, 1) = "gezündet"-Latch
      @0x1628: Aot_reset(8, sce 0)                     ; Use-Spot tot
      @0x1632: 29 02 / @0x1634: 3c 01 Cut_auto
      @0x1636: Message_on(4)                           ; "Bomb activated, I have 10 seconds..."
      @0x163c: Set(2,7,1) @0x1640: Set(1,0x1b,1)       ; Global-Bits (Bank 1/2 = 0x800aca3c/40)
      @0x1648: 40 00 09 20 ee 34 e8 ea + Do-While-Schleifen auf Ck(5,0x20)   ; Explosions-/Shake-
               Sequenz (Opcode 0x40, 8 B — nicht RE'd, HYPOTHESE: Effekt/Kamera)
      @0x1680: Cut_replace(0x02, 0x0d)
      @0x1684-0x16a8: Sca_id_set/Sca_floor_set(0..4, 0x13, 0x00)   ; ⇒ DIE TÜR IST AUFGESPRENGT
      @0x16ae: Set(2,7,0) / Set(1,0x1b,0)
```

### 2b. Die Message-Texte von ROOM2040 (messageStart 0x1750) — der Story-Kontext

* **msg 0 @0x175c** — „**Notice of damage door** … While doing maintenance and construction work on the Raccoon sewage system we found a problem with a door, it seems that it rusted due to water leaks, we tried everything and nothing works! **Unless we use explosives or something** …"
* **msg 5 @0x19e4** — „**Will you use the Time Bomb?**" (Yes/No, wieder literal, kein Item-Insert)
* **msg 4 @0x19a6** — „**Bomb activated, I have 10 seconds before it detonates!**"

Das Detonations-Latch Bank 3/Bit 0x35 wird game-weit NUR von ROOM2040/2041 gelesen/geschrieben (derselbe Sweep: 4 Treffer — 2× Ck sub00 @0x1452, 2× Set sub02 @0x1624).

**Antwort auf die Nutzerfrage:** Die Zeitbombe aus ROOM1110 (RPD-Asservaten-/Waffenkammer — msgs 0-7: toter Polizist mit Zettel „4312", beschlagnahmte Waffen, Arklay-Raketenwerfer) wird an der **verrosteten Kanalisations-Tür in ROOM2040/2041 (STAGE2)** verwendet: Aushang lesen → „Will you use the Time Bomb?" → YES → per ACTION zünden → 10-Sekunden-Message, Explosionssequenz, Kollision 0x13 wird freigeräumt. Im selben Raum liegt dahinter u. a. das Quest-Item **0x32 „Head of Akuma"** (Item_aot_set @0x1364, Census).

---

## 3. Negativ-Beweise (alle geforderten Suchwege)

### 3a. Item_aot_set game-weit: Typ 0x35 existiert NICHT

Census (240 Dateien, 164 `0x50`-Records, 100 % Coverage): Typ-Histogramm = {0x04,0x05,0x07,0x08,0x0c,0x0d,0x13,0x15,0x16,0x17,0x22,0x23,0x24,0x25,0x26,0x30,0x31,0x32,0x36,0x37,0x38,0x39,0x3f,0x41,0x44,0x46,0x47} — **kein 0x35**, in keinem Raum, keiner Stage.

### 3b. Aot_reset-Retypes auf sce 9: ebenfalls kein 0x35

Alle 8 ausgelieferten `0x46`-sce-9-Retypes: ROOM1190/1191 (Typ 0x37 Red Keycard), ROOM40A0/40A1 (0x22, 0x26, 0x22). 

### 3c. Tür-AOTs (0x3B) haben in RE1.5 KEIN Key-Item-Feld

Der sce-2-Tür-Handler (Dispatch-Tabelle @0x8007469c[2], aus der EXE gedumpt) ist 13 Instruktionen kurz und prüft NICHTS:

```
800430bc: ori v0,zero,0x1
800430c4: sw  a0,-13912(at)     ; 0x800ac9a8 = Payload-Zeiger merken
800430d4: sb  v0,21337(at)      ; 0x800b5359 = 1 (Transition pending)
800430d8-e4: pauseflags |= 0xff000000
800430e8: jr  ra
```

Kein Inventar-Lesen, kein Lock-Feld (anders als RE2-Retail). Verschlossene Türen sind in RE1.5 reine SCD-Logik (Flag-Ck + Message). Gegenprobe: `FUN_8004dfec` (Find-Item-by-ID) hat exakt **2** Aufrufer — 0x8004ebbc/0x8004ec34, beide im Ammo-/Reload-Pfad; und ALLE 51 Code-Referenzen auf das Inventar-Array `DAT_800b10ac` liegen in 0x80046xxx-0x8004exxx (Status-Screen/Inventar-Subsystem). Kein Tür-/Event-Code liest je das Inventar.

### 3d. Es gibt KEINEN „Item-Besitz"-SCD-Opcode

`Ck` (0x21) testet ausschließlich die Flag-Bänke der Tabelle @0x80074664 (Bank 3 Story @0x800b0ff8, Bank 5 Event @0x800b1028, Bank 9 taken-Bits @0x800b1078, Bank 0x0c Message-State @0x800b8520 …). 0x4C ist **Sce_espr_kill** (Port `scd_vm.c:318`; Länge 18 = LAB_80040858), 0x4D/0x4E/0x4F sind Entity-Parameter-Setter (selbst disassembliert: 0x4D @0x800408a8 und 0x4E @0x80041980 schreiben über die Zeiger-Tabelle 0x800b2368, 0x4F @0x80016f20 über 0x800b23f4) — keine Item-Opcodes. Schlüssel-Item-Progression läuft im Preview über die **taken-Bits (Bank 9)** bzw. Story-Flags (Bank 3) — genau so ist auch die Bombe verdrahtet.

### 3e. EXE: Item-USE-Dispatch und Combine-Graph kennen 0x35 nicht

* **USE-Klassifizierer @0x8004aa64** (Status-Screen, Port-Zitat `menu_common.c:488ff`, selbst nachdisassembliert):
  ```
  8004ab48: sltiu v0,(id-0x22),0xe     ; Heal-Fenster 0x22..0x2f
  8004ab54: beq   a0,0x25              ; Red Medicine raus
  8004ab6c: ori   v0,zero,0x6
  8004ab74: sb    v0,9667(at)          ; 0x800b25c3 = c3 := 6
  ```
  Alle ids ≥ 0x30 — auch 0x35 — landen in Klasse 6 = **„You can't use it here."** Es gibt keinen Use-Handler für 0x35.
* **Combine-Graph @0x80074C88** (Pair-Lists, 288 Bytes bis 0x80074DA8, komplett gedumpt): das Byte 0x35 kommt in der gesamten Region **nicht vor** (weder als Partner noch als Ergebnis). Per-Item-PROP-Tabelle @0x80074DA8 (stride 12, 72 Zeilen ids 0x00..0x47, endet exakt @0x80075108): Zeile 0x35 @0x80075024 = `01 00 00 00 88 4c 07 80 00 00 00 00` → Pair-Count-Byte (+9, `DAT_80074db1 + id*0xc` im Matcher FUN_8004e900) = **0**, Listen-Zeiger = 0x80074C88 = die gemeinsame Null-Liste (`00 00 00 00`). Der Matcher bricht für 0x35 doppelt ab (`count==0` und `*list==0`).
* **Grant-Pfade:** `DAT_800afbb6` (Modal-Grant-ID) hat im ganzen Spiel EINEN Schreiber — den sce-9-Handler @0x8004335c; `FUN_8004dc4c` (Inventar-Insert) hat EINEN Aufrufer — @0x8001e0c4 (Item-Get-Modal). Die Overlays (`RE_15_Quellcode_Overlays/`, grep) referenzieren weder 0x800afbb6 noch 0x800b10ac noch FUN_8004dc4c. ⇒ Ohne sce-9-Record mit Typ 0x35 (3a) kann das Item **auf keinem Codepfad** ins Inventar gelangen. Das Debug-Menü (FUN_80014444) hat nur JUMP + MEMORY VIEWER, keinen Item-Grant.

### 3f. Die Item-0x35-ASSETS existieren trotzdem vollständig (geplant, nicht verdrahtet)

`ITEM/ITPS.ITP` Block 0x35 (@0x35*0x3000, TIM 8bpp 112x72): zeigt einen **Sprengsatz mit Zünder + Fernauslöser** (gerendert und angesehen); Block 0x38 zeigt zur Kontrolle die blaue „GATE SYSTEM / RACCOON POLICE"-Karte — die Asset-Indizes sind NICHT verschoben. `DATA/ITEMALL.PIX` Tile 0x35 (72 Tiles à 1200 B) existiert ebenso. D. h.: Name, Foto, Icon und PROP-Zeile für ein Inventar-Item „Timer Bomb" sind im Build vorhanden — die ROOM1110/2040-Skripte benutzen stattdessen die Message+Flag-Implementierung und die Literal-Schreibweise „Time Bomb".

---

## 4. Port-Relevanz (statisch geprüft, NICHT live verifiziert)

* Alle Opcodes der Kette sind im Port registriert: 0x21 `op_ck` (byte-true XOR-Prädikat, `scd_vm.c:1817`), 0x22 `op_set` (OR/AND/XOR-Selektor), 0x2C/0x46 AOT-Install/Retype, 0x2B Message, 0x2E `SCD_OP_WORK_SET`, 0x3E `SCD_OP_MEMBER_CMP`, 0x51 `op_sce_key_ck`, 0x29 `SCD_OP_CUT_CHG`, 0x37/0x39 Sca_id/Sca_floor, 0x04 Evt_exec (`re15_scd.h:612-634`, `scd_vm.c:262ff`).
* **Nicht geprüft** (nächster Schritt, braucht Live-Lauf): ob der Port (a) Bank 0x0c auf das Yes/No-Bit des Message-Systems mappt, (b) Bank 3 Bit 0x92 über den Stage-Wechsel STAGE1→STAGE2 persistiert, (c) die sub01-Per-Frame-Reseed-Semantik in ROOM2040 korrekt fährt. Verifikationsweg: `RE15_DEBUG_JUMP="1110@…"` → Bombe nehmen → `2040` → Tür-Sequenz per gdigrab (Skill `re15-port-visual-verify`); Vergleich DuckStation.

## 5. Offen / Hypothesen (markiert)

1. **HYPOTHESE:** Die in ROOM1110 sichtbare Bombe liegt im BG-Render (belegt ist nur: kein Obj_model-Prop für sie).
2. Opcode 0x40 (8 B) in sub02 (Explosions-/Shake-Block) ist nicht RE'd; ebenso 0x42/0x43/0x3f-Details der Sequenz. Für die Verwendungsfrage irrelevant.
3. Die sub03-06-Mikro-Skripte (Speed_set/Add_speed nach Bank-5-Bits 0-3 + gehaltener Richtungstaste) sind funktional nicht zu Ende gedeutet (Positionier-/Kamera-Interaktion vor der Zündung).
4. Ob der 10-Sekunden-Countdown aus msg 4 mechanisch existiert (Timer/Spieler-Schaden) oder reiner Text ist: nicht untersucht.
