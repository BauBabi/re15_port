# ROOM1090 — "die Flammen verschwinden nach dem Loeschen nicht" (PORT-Seite)

Auftrag 2 von zweien. Auftrag 1 klaert die ORIGINAL-Seite. Dieses Dossier klaert
ausschliesslich: **was tut der PORT heute**, GEMESSEN, und welche Teile fehlen.

Werkzeuge, die dabei entstanden sind:
* `analysis/nutzer_batch_2026-08-27/tools/re15_scd_dis.py` — RE1.5-SCD-Disassembler
  (Opcode-Laengen aus `re15_port/engine/src/scd_vm.c:166 s_opcode_sizes[]`, dort je Eintrag
  gegen den Handler-PC-Advance auditiert; RDT-Zeiger `main_scd_start = u32@0x40`,
  `sub_scd_start = u32@0x44`, `rdt_common.c:238-239`).
* `re15_port/tests/unit/probe_1090_flame_out.c` — die MESSSONDE (registriert in
  `re15_port/tests/unit/CMakeLists.txt:2822-2833`, **ohne `add_test`**; `ctest -N` meldet
  vor wie nach der Aenderung `Total Tests: 236`).

---

## 0. Ergebnis in drei Saetzen

1. Die komplette Loesch-KETTE existiert im Port und laeuft: sub06 setzt `flag(3,0x81)`, feuert
   die SELBST-TUER, `scd_room_reenter` raeumt den Aktoren-Pool — **die sieben Feuer-Emitter
   (Typ 0x26) und ihre Schadenszone verschwinden korrekt** (gemessen: `n0x26 7 -> 0`,
   `tangibel 7 -> 0`).
2. **Was NICHT verschwindet, sind die ESP-Flammen-Partikel selbst** (Effekt-Id `0x10` x4 und
   `0x08` x3): sie loopen endlos weiter, weil der SELBST-TUER-Wiedereintritt des Ports den
   96-Slot-Effekt-Pool nicht wischt. Genau das sieht der Nutzer.
3. Im Original wischt **jedes** Raumladen diesen Pool (`FUN_80019354`, einziger Aufrufer
   `FUN_800396fc @0x8003996c` = die Raum-Ladekette). Im Port haengt der Wisch
   (`re15_esp_fx_reset()`) ausschliesslich am RAUMWECHSEL-Pfad und fehlt am
   Selbst-Wiedereintritt. **Ein-Zeilen-Fix, keine Neubau-Baustelle.**

---

## 1. Der Port-Pfad — wo die Flammen entstehen

### 1a. Spawn: 7x `Sce_em_set` Typ 0x26 aus ROOM1090 sub00

`re15_port/shared_assets/PSX/STAGE1/ROOM1090.RDT`, SUB-Block-Basis Datei `0x21B4`,
Sub-Tabelle `[0x21c4, 0x23f4, 0x2414, 0x24ce, 0x26e6, 0x26f4, 0x2702, 0x272c]`.

sub00 (Datei `0x21C4..0x23F4`), selbst disassembliert:

```
  0x021C4  06 Ifel_ck    00 22 00
  0x021C8  21 Ck         03 81 00      ; Feuer-Gate (Start-Cut)
  0x021CC  13 Switch     0A 16 00      ;   Case 0 -> Cut_chg 8, Case 3 -> Cut_chg 0x0B
  0x021E6  3C Cut_auto   01
  0x021E8  08 Endif      00
  0x021EA  06 Ifel_ck    00 B4 00
  0x021EE  21 Ck         03 81 00      ; <== DAS FEUER-GATE
  0x021F2  2D Obj_model_set 03 00 00 02 00 08 00 10 00 F4 0B 3E F5 E8 F9 50 00 56 07 ...
  0x02214  44 Sce_em_set 00 26 00 01 00 00 FF 04 08 F8 F8 CA FA 00 00 00 04 00 00
  0x02228  44 Sce_em_set 01 26 01 01 00 00 FF 47 09 F8 F8 E3 F6 00 00 00 04 00 00
  0x0223C  44 Sce_em_set 02 26 02 01 00 00 FF 20 0C F8 F8 56 F2 00 00 00 04 00 00
  0x02250  44 Sce_em_set 03 26 04 01 00 00 FF 9E 0B F8 F8 A4 FC 00 00 00 04 00 00
  0x02264  44 Sce_em_set 04 26 03 01 00 00 FF 3A 0B F8 F8 B8 01 00 00 00 04 00 00
  0x02278  44 Sce_em_set 05 26 03 01 00 00 FF D6 0A F8 F8 F8 07 00 00 00 04 00 00
  0x0228C  44 Sce_em_set 06 26 04 01 00 00 FF 5E 05 F8 F8 3C 05 00 00 00 04 00 00
  0x022A0  08 Endif      00
  0x022A2  06 Ifel_ck    00 40 00
  0x022A6  21 Ck         03 84 01      ; NACH dem Loeschen: Obj_model_set 02 + Sce_em_set Typ 0x42
  0x022E0  04 Evt_exec   FF 18 03      ;   + sub03 (die Rettungs-Cutscene)
  0x022E4  08 Endif      00
  0x022E6  06 Ifel_ck    00 1C 00
  0x022EA  21 Ck         03 81 00
  0x022EE  54 Sce_bgm_control 00 00 01 78 33   ; das Feuer-BGM-Programm
                                               ; (0x54 ist in RE1.5 Sce_bgm_control,
                                               ;  NICHT Sce_espr3d_on — scd_vm.c:4080 ff.)
  0x02306  06 Ifel_ck    00 24 00
  0x0230A  21 Ck         03 85 00
  0x0230E  2C Aot_set    02 01 B1 ...  ; sce 1 = MESSAGE (Vorstufe, noch ohne Loesch-Event)
  0x0232A  07 Else_ck    00 4C 00      ; ---- ELSE-Zweig: flag(3,0x85) IST gesetzt ----
  0x0232E  06 Ifel_ck    00 42 00
  0x02332  21 Ck         03 81 00
  0x02336  2C Aot_set    02 03 B1 01 00 64 00 14 05 3A 07 66 08 DE 0D 30 F8 08 07 10 F5 FF 00 06
                                       ; sce 3 = AUTO-EVENT, Ziel-Sub = 0x06
  0x02352  3B Door_aot_set 03 02 31 00 00 00 00 00 00 00 00 00 00 E4 04 F8 F8 1F F6 B5 0F 00 09 06 01 ...
                                       ; Slot 3, sce 2 = DOOR, Ziel Stage 0 / Raum 9 = ROOM1090
                                       ; SELBST, Eintritts-Cut 6, Spawn (1252,-1800,-2529)
  0x02376  06 Ifel_ck    00 78 00
  0x0237A  21 Ck         03 BB 01      ; 4 weitere Sce_em_set (Typ 0x10/0x11)
```

Der Feuer-Zustand haengt an **Flagbank 3 / Bit 0x81**. Ist das Bit 0, brennt es.

Roh-Gegenprobe (Byte-Scan `21 03 81` / `22 03 81`) ueber alle 240 ausgelieferten RDTs:
ROOM1090 `0x21C8 / 0x21EE / 0x22EA / 0x2332` = `Ck(3,0x81,0)`, `0x271E` = `Set(3,0x81,1)`;
ausserdem ROOM10B1 `0x17C6 Ck(3,0x81,1)`, `0x1832 Set(3,0x81,1)`, `0x1954 Set(3,0x81,0)`.
**Im Raum selbst gibt es genau EINEN Setzer.**

### 1b. Das Loesch-Event: ROOM1090 sub06 (Datei `0x2702..0x272C`)

```
  0x02702  2B Message_on 07 FF FF
  0x02706  02 Evt_next
  0x02708  2B Message_on 08 FF FF
  0x0270C  02 Evt_next
  0x0270E  06 Ifel_ck    00 18 00
  0x02712  21 Ck         0C 1F 00
  0x02716  22 Set        02 07 01      ; Pause-Bit 0x01000000 (Zone 2 = DAT_800aca40)
  0x0271A  09 Sleep      0A 0A 00
  0x0271E  22 Set        03 81 01      ; <== DAS LOESCH-FLAG
  0x02722  22 Set        03 84 01      ; <== "die Frau ist zu retten"
  0x02726  47 Aot_on     03            ; <== feuert die SELBST-TUER Slot 3
  0x02728  08 Endif      00
  0x0272A  01 Evt_end    00
```

Die Nachrichten 7 und 8 dieses Raums, Glyph fuer Glyph dekodiert (Message-Block
`RDT+0x3C = 0x2748`, 10 Eintraege; Glyph-Map aus `item-name-feuerloescher.md` §3e:
`0x1D..0x36 = A..Z`, `0x3D..0x56 = a..z`, `0x0C..0x15 = 0..9`, `0x00 = Space`):

| id | Datei | Text |
|----|-------|------|
| 7 | 0x28E6 | `I must hurry up and get something to / put out this fire to save that woman` |
| 8 | 0x2934 | `Will you use the ` + Steuercode `{05}` |
| 9 | 0x2961 | `You{3A}ve used the ` + Steuercode `{05}` |
| 0/2/4 | 0x275C/0x27BD/0x281F | die drei `Woman`-Rufe hinter dem Feuer |

Damit ist die Nutzer-Formulierung "nach dem **Loeschen**" datenseitig belegt: sub06 ist
das Feuerloescher-Event. (msg 8/9 sind wortgleich mit den Item-Prompt-Skripten
`use`/`used` aus `BIN/DEBUG.BIN` — `item-name-feuerloescher.md` §3f, Eintraege [4] und
[2]. Ob `{05}` dort die Namens-Einsetzung ist, habe ich NICHT nachgeprueft.)

**Gegenprobe im echten SCD-Bereich** (`0x21C4..0x2748`, 297 Opcodes; `0x2748` ist der
Anfang des Message-Blocks `RDT+0x3C`): ROOM1090 enthaelt **kein** `Sce_espr_kill`
(0x4C), **kein** `Keep_Item_ck` (0x57), **kein** `Item_lost` (0x5B) und **kein**
`Sce_item_get` (0x5D). Das Feuer wird also weder per Effekt-Opcode geloescht noch
haengt das Event an einer Item-Pruefung im Raumskript.

### 1c. Der Gate-Kandidat `Ck(0x0C,0x1F,0)` — was er im Original wirklich liest

Die `Set`/`Ck`-Zone ist im Original ein Index in die ZEIGER-TABELLE `PTR_DAT_80074664`
(Handler `LAB_8003fdd0 @0x8003fe04 lw v1,0(at)`; Herleitung in `game_state.c:85-112`).
Ghidra-Dump der Tabelle (`ghidra1_V2.txt:243204,243216-243217`):

```
        80074664 38 ca 0a 80     addr  DAT_800aca38      ; [0]
        ...
        80074690 44 ca 0a 80     addr  DAT_800aca44      ; [11]
        80074694 20 85 0b 80     addr  DAT_800b8520      ; [12]  <== Bank 0x0C
```

`Ck(0x0C,0x1F,*)` liest also Wort 0 an `0x800B8520` mit der Maske
`0x80000000 >> 31 = 0x00000001`, d.h. **Bit 0 der u32 an 0x800B8520**.
`DAT_800b8520` ist die Zustands-Zelle des DIALOG-Systems:

```
  80027e74: lbu v0,0(v1)      ; v1 = 0x800b8520   (FUN_80027e68 = Message_on)
  80027e7c: andi v0,v0,0x80   ; Bit 0x80 = "Text laeuft"
  80027e9c: sb  t1,0(v1)      ; t1 = 0x80  -> Latch setzen
  800286c4: sb  v0,0(v1)      ; v0 = alt & 0x7f -> Latch loeschen
  80028714: sb  v0,-31456(at) ; v0 = alt & 0x7f -> Latch loeschen
```

Alle drei bekannten Schreiber fassen **nur Bit 0x80** an; keiner schreibt Bit 0x01.
`Ck(0x0C,0x1F,0)` waere damit im Auslieferungsstand ein **immer-wahr**-Test.
Ein Roh-Byte-Scan (`21 0C 1F` / `22 0C 1F`) ueber alle 240 RDTs liefert 98 Treffer,
ALLE mit Erwartungswert 0 und **keinen einzigen `Set`** — opcode-ausgerichtet
nachgeprueft habe ich davon nur den ROOM1090-Treffer `@0x2712`; die uebrigen sind
Roh-Treffer und koennen Datenbytes sein. Ob ein vierter, indirekter Schreiber Bit 0
von `0x800B8520` setzt, ist **OFFEN und gehoert zu Auftrag 1**.

Im PORT ist die Frage bereits entschieden: `op_ck` (`scd_vm.c:1780-1798`) ruft
`re15_game_flag_get(12, 31)` → `g_game.flags[12][0]` (`game_state.c:78-82`), eine Bank,
die im Port **nichts** schreibt → immer 0 → die Bedingung ist **immer wahr**.
Der Port hat also kein Item-Gate an dieser Stelle — und das Skript hat auch keins
(kein `Keep_Item_ck` 0x57, kein `Item_lost` 0x5B in sub06).

### 1d. Der Emitter: `re15_spider_ai_tick` (Typ 0x26)

* Dispatch-Registrierung STAGE1: `8011E8F4 addiu v0,v0,25224` (= `0x80116288`),
  `8011E8FC sw v0,11332(at)` → `0x80072BAC + 0x26*4 = 0x80072C44`.
* Port: `re15_port/engine/src/enemy_ai_common.c:7628` `re15_spider_ai_tick`
  * **INIT** `enemy_ai_common.c:7682` → `re15_esp_type26_emerge(...)`
    = die FUNKEN (Effekt-Id `0x09`, `a0 = 0x09031800`, Gate `grid&0x80`
    `@0x801166c4-cc`, Spawn `jal 0x80019700 @0x801166e4`).
  * **ACTIVE sub 0** `enemy_ai_common.c:7696` → `re15_esp_type26_flame(...)`
    = die FLAMME (Effekt-Id `0x10` Raum-Bank / `0x08` CORE00-Bank je Varianten-
    Sprungtabelle `@0x80100364`, `jal 0x80019700 @0x80116d84`).
  * **Schadenszone**: Root-Schwanz `enemy_ai_common.c:7787-7818` —
    `spider_phase < 13` = intangibel (`sltiu v0,v0,0xd; bne @0x80116328-2c`), sonst
    `re15_body_push` (`jal 0x8002aec4 @0x80116368`) → Stagger cmd 2
    (`@0x8011638c-a4`) + `hp -= 2` pro Ueberlapp-Frame (`slti v0,v0,4 @0x801163c0-c4`,
    Store `@0x801163c8-d0`).
* ESP-Seite: `re15_port/engine/src/re15_esp.c:409` `re15_esp_type26_flame`,
  `re15_esp.c:420` `re15_esp_type26_emerge`, beide → `re15_esp_fx_spawn_ex`
  (`re15_esp.c:321`).

### 1e. Wer die Flamme am Leben haelt

`re15_esp_fx_tick` (`re15_esp.c:690`) despawnt einen Slot NUR bei
Terminator-Record (`dur==0 && loop==0`, `re15_esp.c:759`) oder Bank-Fehler
(`re15_esp.c:754`). Der Flammen-Effekt `0x10` hat als Record 10 den **Loop-Marker**
`00 01 ff 40` (`param&0xff == 0xFF` → `frame := desc&0xff == 0`), belegt und gepinnt in
`re15_port/tests/unit/test_1090_fire_pin.c:300-326` ("aktiv nach 60 Ticks, max_frame 9,
6 Rueckspruenge"). **Eine einmal gespawnte Flamme erlischt von sich aus nie.**

---

## 2. Die MESSUNG

`re15_port/tests/unit/probe_1090_flame_out.c`, gebaut und ausgefuehrt
(`cmake --build re15_port/build --target probe_1090_flame_out`,
`re15_port/build/tests/unit/probe_1090_flame_out.exe`). Bild-Ablauf identisch zu
`probe_1090_flame_touch.c`: `scd_vm_tick -> Walker -> cine/pmode -> re15_msg_tick ->
re15_cam_present_tick -> re15_game_step -> re15_esp_fx_tick`.

### A) Basis (alle Flags 0)

```
     f  n0x26 tang  fx10 fx08 fx09 rest  evtAOT  f(3,0x81)  hp
      0     7    0     0    0    7    0      1       0      100
      1     7    7     4    3    7    0      1       0      100
    ...
    199     7    7     4    3    7    0      1       0      100
```

7 Emitter, ab Bild 1 alle tangibel (Schadenszone scharf), 4 Raum-Flammen `0x10` +
3 CORE00-Flammen `0x08` + 7 Funken `0x09` — **ueber 200 Bilder unveraendert**.
(Die 4/3-Aufteilung deckt sich mit der Varianten-Tabelle `@0x80100364`:
`{0:0x08, 1:0x10, 2:0x10, 3:0x08, 4:0x10}` bei grid_ids 0,1,2,4,3,3,4.)

### B) Flag VOR sub00 gesetzt (Wiedereintritts-Semantik)

```
   flag( 3,0x81)=1 -> n0x26=0 tang=0 fx10=0 fx08=0 fx09=0 rest=0 evtAOT=1  <== AENDERT ETWAS
   flag( 3,0x84)=1 -> n0x26=6 tang=0 fx10=0 fx08=0 fx09=6 rest=0 evtAOT=1  <== AENDERT ETWAS
   flag( 3,0x85)=1 -> n0x26=7 tang=7 fx10=4 fx08=3 fx09=7 rest=0 evtAOT=1
   flag( 3,0x80)=1 -> n0x26=7 tang=7 fx10=4 fx08=3 fx09=7 rest=0 evtAOT=1
   flag( 3,0xBB)=1 -> n0x26=3 tang=3 fx10=1 fx08=2 fx09=3 rest=0 evtAOT=1  <== AENDERT ETWAS
   flag(12,0x1F)=1 -> n0x26=7 tang=7 fx10=4 fx08=3 fx09=7 rest=0 evtAOT=1
```

**Das Gate greift.** `flag(3,0x81)=1` vor sub00 = kein Feuer, keine Flamme, keine
Schadenszone. (`flag(3,0x84)`/`flag(3,0xBB)` sind Nebenwirkungen anderer Zweige und
hier nur zur Abgrenzung mitgemessen.)

⚠️ Falle, die die erste Messung verfaelscht hat und im Sonden-Kopf dokumentiert ist:
`scd_vm_init()` ruft `re15_game_state_init()` (`scd_vm.c:440`), das
`memset(&g_game, 0, sizeof(g_game))` macht (`game_state.c:68`) — **ein VOR dem Raum-Boot
gesetztes Flag ist danach weg.** Die Vorbelegung muss zwischen `scd_vm_init()` und
`scd_room_reenter()` liegen.

### C) Flag ZUR LAUFZEIT gesetzt

```
   flag( 3,0x81): vorher n0x26=7 fx10=4 fx08=3 | nachher n0x26=7 fx10=4 fx08=3  (unveraendert)
   flag( 3,0x84): ... (unveraendert)
   flag( 3,0x85): ... (unveraendert)
   flag( 3,0x80): ... (unveraendert)
   flag( 3,0xBB): ... (unveraendert)
   flag(12,0x1F): ... (unveraendert)
```

**Kein einziges Flag hat eine Laufzeit-Wirkung** — was byte-true ist: ein Voll-Slice
ueber `0x80116288..0x80116DA8` (die ganze Typ-0x26-Familie) enthaelt genau EINEN Aufruf
in die Flag-API, und das ist der SETZER `jal 0x8004ef90 @0x80116ac8` in der
Flinch-Phase. **`jal 0x8004efe4` (der Flag-LESER) kommt in der gesamten Familie nicht
vor** — die Feuer-KI liest `flag(3,0x81)` nie. Der Loesch-Weg kann also gar kein
KI-interner sein.

### D) Das Loesch-Event gefeuert (`scd_event_fire(6)`), Vorbedingung `flag(3,0x85)=1`

```
   AOT-Slots nach sub00: [0]type=1 ev=0 [1]type=1 ev=0 [2]type=0 ev=6 [3]type=1 ev=0
   scd_event_fire(6) -> Thread-Slot 10
   + 60 Bilder: n0x26=7 tang=7 fx10=4 fx08=3 f(3,0x81)=0 f(3,0x84)=0 evtAOT=1 | room_change pending=0
   +200 Bilder: n0x26=0 tang=0 fx10=4 fx08=3 f(3,0x81)=1 f(3,0x84)=0 evtAOT=1 | room_change pending=0
   +399 Bilder: n0x26=0 tang=0 fx10=4 fx08=3 f(3,0x81)=1 f(3,0x84)=0 evtAOT=1 | room_change pending=0
   ERGEBNIS: <== AKTOREN weg, FLAMMEN-FX BLEIBEN (Nutzer-Befund)
   Gegenprobe re15_esp_fx_reset() an dieser Stelle: fx10=0 fx08=0 fx09=0 -> das ist der fehlende Schritt
```

**Das ist der Befund, exakt reproduziert:**
* `n0x26 7 -> 0`, `tangibel 7 -> 0` — die Emitter und die Schadenszone verschwinden korrekt.
* `fx10 = 4`, `fx08 = 3` — **die sichtbaren Flammen brennen weiter, unendlich.**
* `room_change.pending` bleibt 0, weil es KEIN Raumwechsel ist: die Tuer zeigt auf
  ROOM1090 selbst.

Zwei Vorbedingungen, die eine unvorsichtige Messung verfehlt (beide im Sonden-Kopf
dokumentiert):
1. `flag(3,0x85)` MUSS 1 sein — nur im ELSE-Zweig von `Ck(3,0x85,0) @0x230A` installiert
   sub00 ueberhaupt die sce-3-Flaeche `@0x2336` **und** die Selbst-Tuer `@0x2352`.
   Mit `flag(3,0x85)=0` ist Slot 3 keine Tuer, `Aot_on 03` verpufft, und nichts passiert.
2. Die Dialoge muessen bestaetigt werden (virtuelle Taste `0x4000` = physisch SQUARE,
   `msg_common.c:409-422`), sonst haengt sub06 fuer immer in `Message_on 07`.

### E) Wiedereintritt / Kontrolle

```
   nach dem Event: flag(3,0x81)=1 flag(3,0x84)=0
   nach Neu-Boot: n0x26=0 tang=0 fx10=0 fx08=0 fx09=0 evtAOT=1
   Kontrolle flag(3,0x81)=1 + flag(3,0x84)=1 vor sub00: n0x26=0 tang=0 fx10=0 fx08=0 fx09=0
```

(`flag(3,0x84)` ist nach dem Event wieder 0, weil der Wiedereintritt sub00 →
`Evt_exec sub03 @0x22E0` faehrt und sub03 `@0x24CE` das Bit selbst loescht — dieselbe
Kette, die `probe_1090_gate_selfdoor.c` beschreibt. Das ist Original-Verhalten, kein Fehler.)

---

## 3. Die URSACHE, praezise

### 3a. Der Weg, den `Aot_on 03` im Port nimmt

`op_aot_on` (`scd_vm.c:3669-3678`) → `re15_aot_fire_slot(3)` (`aot_common.c:614 ff.`) →
Typ DOOR → `aot_fire_door(3)` (`aot_common.c:490-596`). Dort:

```c
unsigned dest_id = (((unsigned)d->dest_stage + 1u) << 12)
                 | ((unsigned)d->dest_room << 4)
                 | (g_current_room_id & 0x000Fu);
if (dest_id != g_current_room_id) {          /* aot_common.c:518 */
    re15_room_request_change(...);           /*  -> RAUMWECHSEL */
    ...
}
/* ---- SONST: reiner In-Raum-Teleport ---- */
g_scd.cam_id = d->target_cut; g_scd.cam_change_pending = 1;
g_actors[0].x/y/z/rot_y = d->spawn_*;
re15_collision_set_band(...);
if (d->dest_room != 0 &&
    (0x1000u | ((unsigned)d->dest_room << 4)) == g_current_room_id)
    g_scd_pending_scenario = (int)d->target_cut;   /* aot_common.c:573 */
```

Fuer ROOM1090 ist `dest_stage=0, dest_room=9` → `dest_id = 0x1090 == g_current_room_id`
→ **Selbst-Tuer-Zweig**, `g_scd_pending_scenario = 6`.

Konsumiert wird das in `game_step_common.c:1535-1541`:

```c
if (g_scd_pending_scenario >= 0 && c->rdt_ok) {
    uint8_t sc = (uint8_t)g_scd_pending_scenario;
    uint8_t entry_cut = g_scd.cam_id;
    g_scd_pending_scenario = -1;
    scd_room_reenter(c->rdt, pl->x, pl->z, sc);
    ...
}
```

`scd_room_reenter` (`scd_room_setup.c:110-180`) macht u.a.
`memset(&g_scd, ...)`, leert Flagbank 5 Wort 0, und — entscheidend —
raeumt den AKTOREN-Pool:

```c
for (int ai = 1; ai < RE15_ACTOR_MAX; ai++)
    if (g_actors[ai].active) memset(&g_actors[ai], 0, sizeof(g_actors[ai]));
g_actor_count = 1;
```

Deshalb `n0x26 7 -> 0`. **Der Effekt-Pool wird an KEINER Stelle dieses Pfades angefasst.**

### 3b. Wo `re15_esp_fx_reset()` heute haengt

Repo-weiter Zensus (`grep -rn "re15_esp_fx_reset" re15_port/engine re15_port/platform`,
ohne Tests) — **genau ein Produktions-Aufrufer**:

* `re15_port/platform/pc/src/room_pc.c:130` in `re15_room_reset_render_pc()`,
* und dessen einziger Aufrufer ist `re15_port/platform/pc/main.c:5235`
  (`rc.reset_render = re15_room_reset_render_pc;` fuer `re15_room_apply_pending`)
  = **ausschliesslich der RAUMWECHSEL-Pfad.**

### 3c. Warum das eine echte Divergenz ist (Original-Beleg)

Der Kommentar in `room_pc.c:126-129` nennt die Original-Funktion bereits; hier die
selbst nachdisassemblierten Bytes:

```
  80019354: addiu sp,sp,-64
  8001935c: ori   a0,zero,0x60          ; 0x60 = 96 Slots
  80019360: ori   v0,zero,0x3180
  80019364: addiu v0,v0,-132            ; Stride 132 = 0x84
  80019368: addiu a0,a0,-1              ; <-- Schleifenkopf
  8001936c: lui   at,0x800a
  80019370: addiu at,at,29732           ; 0x800A7424 = Basis des Effekt-Pools
  80019374: addu  at,at,v0
  80019378: sb    zero,0(at)            ; <== Slot-Feld +0x00 := 0  (= "inaktiv")
  8001937c: bne   a0,zero,0x80019368
  80019380: addiu v0,v0,-132
  ...
  80019388-800193f0: ID-Maps 0x800B2248 / 0x800B22D4 := -1
```

Aufrufer-Zensus aus `ghidra1_V2.txt:86198` — **genau einer**:

```
  FUN_80019354   XREF[1]: FUN_800396fc:8003996c(c)
  8003996c d5 64 00 0c   jal  FUN_80019354
```

`FUN_800396fc` ist die RAUM-LADEKETTE. Das Original kennt **keinen** Sonderweg
"gleicher Raum, kein Reload": eine Tuer auf den eigenen Raum laeuft durch dieselbe
Kette und wischt den Pool mit. Der Port hat den Sonderweg erfunden (bewusst, um den
Asset-Reload zu sparen) und dabei den Pool-Wisch verloren.

---

## 4. Was im Port GAR NICHT existiert vs. was nur nicht ausgeloest wird

| Teil | Status im Port | Beleg |
|---|---|---|
| Feuer-Gate `Ck(3,0x81,0)` in sub00 | **existiert und wirkt** | Messung B: `flag(3,0x81)=1` → n0x26=0 |
| Loesch-Event sub06 (`Set(3,0x81,1)`, `Set(3,0x84,1)`) | **existiert und laeuft** | Messung D: f(3,0x81) 0→1 |
| `Aot_on 03` → Selbst-Tuer → `scd_room_reenter` | **existiert und laeuft** | `scd_vm.c:3669`, `aot_common.c` Selbst-Tuer-Zweig, `game_step_common.c:1535` |
| Aktoren-Raeumung beim Wiedereintritt | **existiert und wirkt** | `scd_room_setup.c:176-178` Aktor-memset; Messung D: n0x26 7→0 |
| Schadenszone endet mit den Aktoren | **existiert und wirkt** | Messung D: tangibel 7→0 |
| **Effekt-Pool-Wisch beim Wiedereintritt** | **FEHLT KOMPLETT** | Zensus §3b: `re15_esp_fx_reset()` nur im Raumwechsel-Pfad |
| Laufzeit-Loeschen einzelner FX (`Sce_espr_kill` 0x4C) | Stub, nur PC-Advance | `scd_vm.c:3864-3868` — **aber ROOM1090 nutzt 0x4C nicht**, also fuer diesen Fehler irrelevant |
| Item-/Feuerloescher-Gate in sub06 | existiert im Original NICHT (kein `Keep_Item_ck`) | §1b/§1c |
| `Ck(0x0C,0x1F,0)`-Gate | Port: immer wahr (Bank 12 unbeschrieben) | `game_state.c:78-82`; Original-Zelle `0x800B8520`, §1c — **Semantik OFFEN** |

Es ist also **kein Neubau**: die gesamte Mechanik ist da, es fehlt **ein** Aufruf an
**einer** Stelle.

---

## 5. Bezug zu v0.3.21 ("Feuer-Schaden ueber die Spieler-Render-Bank ueberarbeitet")

`git show 508e71d0` (Release `7f5df514` = v0.3.21), Finding **F5**:

> F5 FEUER 1090 (Wiederholungs-Report — mein frueherer "Fix" war die falsche Haelfte):
> kein Feuer-Bug, sondern ein SPIELER-RENDER-BANK-Bug. Der Trigger feuerte laengst; die
> Verletzt-/Knockdown-/Todes-Pose wurde aber aus der RAUM-RBJ-Bank statt aus COMMON PL00
> gerendert und blieb dadurch auf einem eingefrorenen Keyframe stehen.

Betroffene Dateien des Commits: `anim_select_common.c`, `enemy_ai_common.c`,
`enemy_ai_re2_zombie.c`, `game_step_common.c`, `platform/pc/main.c` + Sonden.
**Kein Aufruf am ESP-Pool, keine Zeile in `re15_esp.c`, `room_pc.c`, `aot_common.c`
oder `scd_room_setup.c`.**

→ **Der Loesch-Pfad haengt NICHT mit v0.3.21 zusammen.** F5 hat die *Reaktion des
Spielers* auf ein brennendes Feuer korrigiert; hier geht es um das *Ende* des Feuers.

Eine inhaltliche Verbindung gibt es dennoch, und sie stuetzt den Befund: derselbe
Commit dokumentiert in der "Skeptiker-Nacharbeit" einen Fehler **derselben Klasse** —
`test_re2_hit_repeat` liess Blut-Partikel eines vorherigen Laufs weitertickern, weil
niemand den Effekt-Pool leerte; die Loesung war dort woertlich
`re15_esp_fx_reset() im bringup`. **Der ESP-Pool ist im Port schon einmal dadurch
aufgefallen, dass ihn niemand raeumt.**

---

## 6. Fix-Rezept (minimal, byte-belegt)

**Eine Zeile**, in `re15_port/engine/src/game_step_common.c`, im Konsum-Block des
Selbst-Tuer-Wiedereintritts (heute `:1535-1541`), unmittelbar bei
`scd_room_reenter(...)`:

```c
    g_scd_pending_scenario = -1;
    /* Byte-true: das Original wischt bei JEDEM Raumladen die 96 Effekt-Slots —
     * FUN_80019354, Schleife @0x80019368-80 (`sb zero,0(at)` @0x80019378, Basis
     * 0x800A7424, Stride 132, 0x60 = 96), einziger Aufrufer FUN_800396fc @0x8003996c
     * = die Raum-Ladekette. Das Original kennt keinen "gleicher Raum"-Kurzschluss;
     * eine Tuer auf den eigenen Raum laeuft durch dieselbe Kette. Ohne diesen Wisch
     * ueberleben die ROOM1090-Flammen (ESP-Id 0x10/0x08) das Loesch-Event, weil ihr
     * Anim-Record 10 ein Loop-Marker ist (0xFF -> frame 0) und sie deshalb nie
     * von selbst despawnen. */
    re15_esp_fx_reset();
    scd_room_reenter(c->rdt, pl->x, pl->z, sc);
```

Wichtig: **NUR** `re15_esp_fx_reset()`, **nicht** `re15_esp_set_room_bank(NULL)` — die
Raum-Bank bleibt gueltig, weil der Raum derselbe ist und kein Reload folgt.

Gegenprobe ist bereits in der Sonde und misst `fx10=0 fx08=0 fx09=0` an genau dieser
Stelle (Abschnitt D, letzte Zeile).

Zweiter Nutzniesser derselben Zeile: ROOM1170 (der andere Selbst-Tuer-Raum, Tueren 0
und 6, `game_step_common.c:1527-1533`) — auch dort wischt das Original den Pool und der
Port bisher nicht.

---

## 7. Was OFFEN bleibt (nicht behauptet, nicht geraten)

1. **`Ck(0x0C,0x1F,0)`**: Bank 12 = `DAT_800b8520` (Ghidra `80074694`), getestet wird
   Bit 0x01. Die drei gefundenen Schreiber der Zelle fassen nur Bit 0x80 an. Ob ein
   vierter (indirekter) Schreiber Bit 0 setzt — und damit, ob das Original an dieser
   Stelle die Antwort des "Will you use the …"-Dialogs auswertet — ist **nicht
   geklaert**. Naechster Weg: `FUN_80028134` (Dialog-FSM) und `FUN_8002877c` komplett
   disassemblieren, plus ein DuckStation-Savestate von ROOM1090 im Moment der Abfrage.
   **Auftrag 1.**
2. **Der Item-Verbrauch**: msg 8/9 sind Item-Prompt-Texte ("Will you use the" /
   "You've used the"), aber sub06 enthaelt kein `Keep_Item_ck` (0x57) und kein
   `Item_lost` (0x5B). Woher der Feuerloescher-Verbrauch im Original kommt, ist
   **nicht geklaert** — moeglicherweise ueber den Item-USE-Pfad des Inventars, der das
   Event von aussen feuert. **Auftrag 1.**
3. **Der Wisch-Zeitpunkt relativ zum Fade**: das Original raeumt den Pool innerhalb der
   Ladekette, also hinter dem Uebergangs-Fade. Der Port hat am Selbst-Tuer-Wiedereintritt
   keinen Fade. Ob das sichtbar ist (Flammen verschwinden "hart" statt hinter einem
   Schwarzbild), ist **nicht gemessen** — es braucht die gdigrab-Verifikation im echten
   Fenster (Skill `re15-port-visual-verify`).
4. **Visuelle Bestaetigung fehlt noch**: alles oben ist ueber `re15_game_step` auf der
   echten RDT gemessen, nicht im Fenster. Die Nachweiskette im Fenster ist dieselbe, die
   v0.3.21 offen liess ("vom Debug-Sprung kommt Leon nicht an die Flamme (Dach-Spawn)").

---

## Verifikation (unabhaengig nachgeprueft)

Adversariale Gegenpruefung, 2026-08-25. Jede Adresse selbst disassembliert
(`re15_disasm.py` auf `info/Re1.5/PSX.EXE` bzw. `info/Re1.5/PSX/BIN/STAGE1.BIN`),
jeder Datei-Offset selbst per `xxd` gelesen, die Sonde selbst gebaut und gefahren.

**Gesamturteil: TEILWEISE.** Der Mechanismus traegt vollstaendig und ich habe jede
Messung reproduziert. Sieben Einzel-Behauptungen sind falsch, unbelegt oder nicht mehr gueltig.

### BESTAETIGT (selbst nachgemessen)

* **RDT-Bytes.** `RDT+0x44 = 0x000021b4`; Sub-Tabelle `[0x21C4,0x23F4,0x2414,0x24CE,0x26E6,
  0x26F4,0x2702,0x272C]` — identisch. `@0x21EE 21 03 81 00`; die sieben
  `44 Sce_em_set` @0x2214/0x2228/0x223C/0x2250/0x2264/0x2278/0x228C mit Typ `26` und
  grid 0,1,2,4,3,3,4 — Byte fuer Byte bestaetigt. sub06 @0x2702..0x272A komplett
  bestaetigt, inkl. `@0x271E 22 03 81 01`, `@0x2722 22 03 84 01`, `@0x2726 47 03`.
* **RDT-Zensus.** 240 RDTs. `21 03 81` = **genau 5** Treffer (ROOM1090 0x21C8/0x21EE/
  0x22EA/0x2332 mit Erwartungswert 0; ROOM10B1 0x17C6 mit 1). `22 03 81` = **genau 3**
  (ROOM1090 0x271E=1; ROOM10B1 0x1832=1, 0x1954=0). Der Ein-Setzer-Befund stimmt.
* **`FUN_80019354`.** `8001935c ori a0,zero,0x60` / `80019364 addiu v0,v0,-132` /
  `80019370 addiu at,at,29732` / `80019378 sb zero,0(at)` / `8001937c bne a0,zero,
  0x80019368` — exakt wie zitiert. Ghidra `86198`: `XREF[1] FUN_800396fc:8003996c(c)`;
  `137732`: `8003996c d5 64 00 0c jal FUN_80019354` — exakt. `FUN_800396fc`
  (`RE_15_Quellcode_V2/FUN_800396fc.c`) ist tatsaechlich der RDT-Lader (Pointer-Fixups,
  dann `FUN_80019354(); FUN_80043eac(); FUN_80043fb0(); ... FUN_8003ef6c()`).
* **Pool-Identitaet — staerker belegbar als im Dossier.** Der Spawner `FUN_80019700` nutzt
  dieselbe Struktur: Basis `@0x80019720-24 lui s3,0x800a; addiu s3,s3,29624` = **0x800A73B8**,
  Stride 132 (`@0x80019794-9c sll v0,t3,5; addu v0,v0,t3; sll v0,v0,2`), Kappung
  `@0x8001978c sltiu v0,t3,0x60` (96), Aktiv-Byte `@0x800197a8 lbu v0,108(t0)` = **+0x6C**.
  Und dieselben ID-Maps `0x800B22D4` (@0x80019748) / `0x800B2248` (@0x80019764), die
  `FUN_80019354` @0x80019388-F0 auf -1 setzt. Damit ist zweifelsfrei: der von
  `FUN_80019354` gewischte Speicher IST der ESP-Effekt-Pool.
* **Typ-0x26-Slice.** Voll-Slice 720 Instruktionen ab `0x80116288` (STAGE1.BIN): die
  jal-Ziel-Multimenge ist **identisch** mit der zitierten. `jal 0x8004ef90 @0x80116ac8`
  ist der einzige Flag-Aufruf, `jal 0x8004efe4` kommt nicht vor. Und die Rollen stimmen:
  `0x8004ef90` = SETZER (`8004efac or v0,v0,a0` / `8004efb4 sw v0,0(v1)`),
  `0x8004efe4` = LESER (`8004f004 and v0,v0,v1` / `8004f000 jr ra`).
* **Varianten-Tabelle `@0x80100364`.** 5 Zeiger `{0x80116d44, 0x80116d5c, 0x80116d5c,
  0x80116d44, 0x80116d5c}`, Schranke `@0x80116d1c sltiu v0,v1,0x5`, Maske
  `@0x80116d18 andi v1,v0,0x7f`. Ziel A `@0x80116d58 lui v1,0x803` -> Id **0x08**;
  Ziel B `@0x80116d6c lui v1,0x1003` -> Id **0x10**; Spawn `jal 0x80019700 @0x80116d84`.
  Bei grid 0,1,2,4,3,3,4 also 4x 0x10 + 3x 0x08 — deckt sich mit Messung A.
  Funken-Gate ebenfalls bestaetigt: `@0x801166c4 andi v0,v0,0x80` / `@0x801166c8 bne`,
  `a0 = 0x09031800` (`lui a0,0x903` / `ori a0,a0,0x1800`), `jal 0x80019700 @0x801166e4`.
* **`0x80074694`.** ghidra `243217: 80074694 20 85 0b 80 addr DAT_800b8520`;
  Index = (0x80074694-0x80074664)/4 = **12**. Die drei Schreiber fassen wirklich nur
  Bit 0x80 an (`80027e9c sb t1,0(v1)` mit `t1=0x80`; `800286c0/800286c4 andi 0x7f; sb`;
  `8002870c/80028714 andi 0x7f; sb`). Das "OFFEN" in §7.1 ist korrekt gesetzt.
* **Alle Port-Zeilenverweise** stimmen: `aot_common.c:518` (`dest_id != g_current_room_id`)
  und `:573`, `game_step_common.c:1535-1543`, `scd_vm.c:3669`/`:3864-3868`,
  `re15_esp.c:299/321/409/420/690/754/759`, `enemy_ai_common.c:7628/7682/7696/7787`,
  `room_pc.c:130`, `main.c:5235`, `game_state.c:68/78-82`,
  `msg_common.c` `CONFIRM_BIT = 0x4000` physisch SQUARE, `test_1090_fire_pin.c:300-326`
  (`max_frame == 9`, `wraps == 6`, `anim[10].param&0xff == 0xFF`, `desc&0xff == 0`).
* **§5 v0.3.21.** `508e71d0` — F5-Text woertlich bestaetigt; die Datei-Liste des Commits
  enthaelt **keine** von `re15_esp.c`, `room_pc.c`, `aot_common.c`, `scd_room_setup.c`.
  `test_re2_hit_repeat.c` steht mit +10 Zeilen drin. Beide Aussagen tragen.
* **Die MESSUNGEN — selbst gebaut und gefahren.** `probe_1090_flame_out.exe`:
  A, B, C und E reproduzieren **Ziffer fuer Ziffer**. D reproduziert ebenfalls Ziffer fuer
  Ziffer, sobald man den inzwischen eingebauten Fix wieder abschaltet (ich habe
  `scd_room_setup.c:143` temporaer auskommentiert, neu gebaut, gemessen, restauriert):
  `+200/+399 Bilder: n0x26=0 tang=0 fx10=4 fx08=3 f(3,0x81)=1 ... pending=0`,
  `[aot] DOOR FIRE slot=3 target_cut=6 spawn=(1252,-1800,-2529)`. Mit dem Fix: `fx10=0 fx08=0`.
  Der Kern-Befund ist damit unabhaengig reproduziert.
* **Negativ-Zensus §1b** — nach Korrektur des Werkzeugs (siehe W3) nachgerechnet:
  in `0x21C4..0x2748` **kein** 0x4C, **kein** 0x57, **kein** 0x5B, **kein** 0x5D. Traegt.

### WIDERLEGT / ZU KORRIGIEREN

**W1 — "FEHLT KOMPLETT" / "genau EIN Produktions-Aufrufer" ist nicht mehr wahr.**
Der Fix ist eingebaut und inzwischen als **`43feb33f`** committet ("fix: ROOM1090 — nach dem
Feuerloescher verschwindet auch der Flammen-EFFEKT"). `re15_esp_fx_reset()` hat heute
**zwei** Produktions-Aufrufer: `scd_room_setup.c:143` (ganz oben in `scd_room_reenter`)
und `room_pc.c:130`. Damit sind §0.3, §3b, die Zeile "Effekt-Pool-Wisch beim Wiedereintritt
= FEHLT KOMPLETT" in §4 und §6 ueberholt. Zusaetzlich: §6 empfiehlt den Aufruf in
`game_step_common.c:1535` — eingebaut wurde er woanders (in `scd_room_reenter` selbst),
was auch den zweiten Selbst-Tuer-Raum ROOM1170 mit abdeckt. (Das Dossier wurde 10:35
geschrieben, der Fix 10:39 eingebaut — der Befund war zum Schreibzeitpunkt richtig.)

**W2 — Pool-Basis und geschriebenes Feld sind falsch etikettiert.**
`FUN_80019354` schreibt **nicht** "Feld +0x00 eines Pools mit Basis 0x800A7424".
Der Spawner `FUN_80019700` legt die Pool-Basis auf **0x800A73B8**
(`@0x80019720-24 lui s3,0x800a; addiu s3,s3,29624`) und liest das Aktiv-Byte bei
**+0x6C** (`@0x800197a8 lbu v0,108(t0)`). 0x800A7424 = 0x800A73B8 + 0x6C. `FUN_80019354`
loescht also **das Aktiv-Flag +0x6C jedes der 96 Slots**, nicht Feld +0x00 einer bei
0x800A7424 beginnenden Struktur. Die Schlussfolgerung wird davon nur staerker; die
Etikettierung ist aber falsch — und sie steht inzwischen **so auch im committeten Code**
(`scd_room_setup.c:129-131`, `room_pc.c:127`). Dort nachziehen.
(Nebenbefund, keine Dossier-Behauptung: `re15_esp_fx_reset()` nullt per `memset` den
ganzen Slot, das Original nur das eine Byte — funktional gleich, nicht byte-gleich.)

**W3 — Die §1a-Auflistung ist NICHT die Ausgabe des eigenen Werkzeugs, und das Werkzeug
desynct.** `re15_scd_dis.py` nimmt `s_opcode_sizes[0x2C] = 20` als feste Laenge.
`Aot_set` ist aber laengen-VARIABEL: `op_aot_set` schiebt
`t->pc += (t->pc[3] & 0x80) ? 28 : 20` (`scd_vm.c:2668`, byte-true `LAB_80040534`
@0x80040590 `lbu v0,0x3(v1)` / @0x80040598 `andi 0x80` / @0x8004059c `beq` -> +0x1c/+0x14).
Beide ROOM1090-`Aot_set` tragen `pc[3] = 0xB1`, also Langform (28). Das Werkzeug wie
ausgeliefert produziert deshalb zwischen 0x2322 und 0x232A Muell
(`10 Ewhile F5 / 07 Else_ck 00 FF FF / 00 Nop / 00 Nop`) und nochmal 0x234A..0x2351
(`10 Ewhile F5 / FF ? / 18 Gosub 06 / 00 / 00`) — es faengt sich nur zufaellig wieder.
Das Dossier zeigt eine saubere, offensichtlich handkorrigierte Auflistung, ohne das zu
sagen. Die Auflistung selbst ist richtig (ich habe sie unabhaengig nachgerechnet), aber:
**das Werkzeug ist defekt und sein Negativ-Zensus ist ungepruefte Ausgabe.** Ich musste
§1b mit einem korrigierten Walker neu rechnen (Ergebnis haelt, s.o.). Und die Zahl
"297 Opcodes" in `0x21C4..0x2748` ist mit korrigiertem Walker **287**.
Fix im Werkzeug: `0x2C` genauso konditional behandeln wie `0x2D`.

**W4 — Zwei zitierte Byte-Folgen sind je um ein Byte zu kurz** (verstoesst gegen die
Regel "mit den tatsaechlichen Bytes"):
* `@0x2352 Door_aot_set`: das Dossier zitiert **neun** `00` zwischen `31` und `E4`;
  die Datei hat **zehn** (0x2356..0x235F).
* `@0x2336 Aot_set`: das Dossier zitiert `... 10 F5 FF 00 06`; die Datei hat
  `... 10 F5 FF 00 18 06` — das `18` (@0x234E) fehlt.
Beide Lesarten (dest_stage=0 / dest_room=9 / cut=6 / spawn 1252,-1800,-2529 bzw. ev-Byte
`pc[25] = 0x06` @0x234F) sind trotzdem korrekt — ich habe sie gegen die Feld-Karte in
`op_aot_set` geprueft.

**W5 — §1c "98 Treffer, ALLE mit Erwartungswert 0".** Der Roh-Scan `21 0C 1F` ueber die
240 RDTs liefert **104** Treffer, davon 98 mit Erwartungs-Byte 0 (die restlichen 6 tragen
0x0B bzw. 0x34). Die zweite Haelfte stimmt exakt: `22 0C 1F` = **0** Treffer.

**W6 — Die tragende Original-Aussage ist im Dossier unbelegt.** §0.3/§3c behaupten "Das
Original kennt keinen 'gleicher Raum'-Sonderweg" ohne eine einzige Adresse. Das ist genau
die Stelle, an der die harte Regel greift. Die Aussage ist WAHR — ich habe sie selbst
disassembliert:

```
  8001d94c: lbu v0,9(a0)     ; Ziel-ROOM  -> 0x800b0fe2
  8001d930: lbu v1,10(a0)    ; Ziel-CUT   -> 0x800b0fe4
  8001d960: lbu v0,8(a0)     ; Ziel-STAGE
  8001d968: beq v1,v0,0x8001d988   ; NUR die STAGE wird verglichen (v1 = 0x800b0fe0)
  8001d980: jal 0x80039a30          ; nur der Stage-Wechsel-Zweig
  8001d988: jal 0x800396fc          ; RAUMLADER — von BEIDEN Zweigen erreicht
```

`FUN_8001d600` ist der Tuer-Warp (ghidra `95647`: `XREF[1] 8001ca54(c)`, aus der
Transitions-FSM). Der Raum wird also unbedingt neu geladen, und damit der Pool gewischt.
Diese Zitate stehen im Port-Kommentar `scd_room_setup.c:120-127`, aber nicht im Dossier —
das Dossier hat den Beleg aus dem Quelltext uebernommen, statt ihn zu posten.

**W7 — Kleinigkeit, Etikett.** §1c nennt `0x8003fe04` den `Ck`-Handler
("LAB_8003fdd0 @0x8003fe04"); der Port-Quelltext (`scd_room_setup.c`, `game_state.c`)
nennt dieselbe Adresse den `Set`-Handler. Eines der beiden Etiketten ist falsch. Fuer die
Bank-Index-Rechnung (Tabelle `PTR_DAT_80074664[12] = DAT_800b8520`) folgenlos.
