# ROOM10F0 — die kleine Tür „It's a small office. It's locked."

**Frage:** Lässt sich die kleine Tür öffnen? Braucht sie einen Schlüssel / ein Flag?

**Antwort:** **Nein — sie ist keine Tür.** Der Auslöser ist ein reiner
`Aot_set` **sce = 1 (MESSAGE)**, Slot 9. Er hat keine Tür-Nutzlast, kein Ziel,
keinen Schlüssel und kein Flag. In ROOM10F0/10F1 existiert kein einziger
`Aot_reset` (0x46), der den Slot je in eine Tür umtypen könnte. Der Text ist
Kulisse — im Auslieferungsstand permanent und ohne jeden Öffnungspfad.

Alles unten ist aus den ausgelieferten Bytes gelesen (Datei-Byte-Offsets in
`re15_port/shared_assets/PSX/STAGE1/ROOM10F0.RDT`, 225 092 B).
ROOM10F1.RDT ist **byte-identisch** (beide sha256 `d43d8e351b6d08ba…`,
0 abweichende Bytes) — das Zweitszenario ändert nichts.

---

## 1. Der Auslöser — Slot 9, `Aot_set` sce=1

```
Datei 0x0FFE (main00):
  2c 09 01 31 00 00 26 e9 ce 18 d0 07 e8 03 05 00 ff ff 00 00
  ^^ ^^ ^^ ^^ ^^ ^^ ^^^^^ ^^^^^ ^^^^^ ^^^^^ ^^^^^ ^^^^^
  |  |  |  |  |  |  rectX rectZ rectW rectD msg   pause
  |  |  |  |  |  super=0
  |  |  |  |  floor/band=0
  |  |  |  flags(sat)=0x31  -> Bit 0x10 gesetzt = ACTION-Scan, Bit 0x20 = Forward-Reach
  |  |  sce = 1  = MESSAGE
  |  slot = 9
  op 0x2C Aot_set (Kurzform: pc[3]&0x80 == 0 -> 20 Byte)
```

Feld-Deutung: `re15_port/engine/src/scd_vm.c:2532-2542` (Rect LE, NW-Ecke +
Ausdehnung) und `:2609-2631` (sce-1-Zweig: `msg = pc[14]`, `pmask = pc[16..17]`,
mit Original-Zitaten `LAB_80043084` / `@0x80043098` / `@0x800430a4`).

- Rect: **x[-5850 … -3850], z[6350 … 7350]** (2000 × 1000 Einheiten)
- Nutzlast: `05 00` = **Message-Index 5**, `ff ff` = Pause-Maske 0xFFFF
  (Welt friert ein, solange der Text steht — wie bei allen 524 sce-1-Records
  game-weit, `aot_common.c:108-114`).
- Kein `dest_stage`, kein `dest_room`, kein `cut`, kein Key-Item-Feld — der
  20-Byte-Record hat diese Felder schlicht nicht. Nur `Door_aot_set` (0x3B,
  32 B) trägt sie, und `re15_aot_door_params_t` (`re15_port/include/re15_aot.h:126-147`)
  hat **überhaupt kein Key-Item-/Lock-Feld** — RE1.5-Türen sperren nicht über
  den AOT-Record.

## 2. Der Text — Message 5

Message-Sektion: `RDT+0x3C` = `0x00001168` (Header-Feld `messageStart`,
`RE15_KNOWLEDGE.md` §1.1). Offset-Tabelle 6 Einträge:
`0x0c 0x6d 0x9c 0xc9 0x116 0x14e`.

```
msg 5 = Datei 0x12B6 .. 0x12DC (38 Byte):
  04 02 25 50 3a 4f 00 3d 00 4f 49 3d 48 48 00 4b 42 42 45 3f 41 57
  02 00 25 50 3a 4f 00 48 4b 3f 47 41 40 57 01 00
```
Dekodiert mit dem Port-Zeichensatz (`re15_port/engine/src/msg_common.c:177-201`;
0x1D–0x36 = A–Z, 0x3D–0x56 = a–z, 0x3A = `'`, 0x57 = `.`, 0x02 = Seitenumbruch,
0x01 = Ende):

> **„It's a small office."** ⏎ **„It's locked."**

Game-weiter Sweep über alle 240 ausgelieferten RDTs: dieser Text existiert
**nur** in ROOM10F0.RDT und ROOM10F1.RDT, msg 5, beide @0x12B6.

### Alle 6 Messages des Raums (zur Einordnung)

| # | Datei-Offset | Text |
|---|---|---|
| 0 | 0x1174 | „A large communications device. It's out of commission. I can't send any messages from here." |
| 1 | 0x11D5 | „Nothing suspicious nor out of the ordinary." |
| 2 | 0x1204 | „There isnt anything useful on this shelf." |
| 3 | 0x1231 | „There are various devices. This must be where they gathered information." |
| 4 | 0x127E | „Several files from past cases. Nothing useful here." |
| 5 | **0x12B6** | **„It's a small office. It's locked."** |

## 3. Vollständige AOT-Karte von ROOM10F0

`main00` (RDT+0x40 = 0x0F30, Zeigertabelle 1 Eintrag → Code ab 0x0F32) läuft
**komplett bedingungsfrei** durch: kein `If`, kein `Ck`, kein `Switch`, kein
`Aot_reset` — 17 Opcodes, dann `Evt_end` @0x10D0.

| Datei | Op | Slot | sce | Bedeutung | Rect x / z | Nutzlast |
|---|---|---|---|---|---|---|
| 0x0F32 | 0x3B | 0 | 2 DOOR | Tür | 8800…9800 / -1300…700 | dest **0x10D0**, cut 0, spawn (1700,0,-7000) |
| 0x0F52 | 0x3B | 1 | 2 DOOR | Tür | -5350…-4150 / -2250…-1250 | dest **0x1090**, cut 0, spawn (-13600,-9000,1300), yaw 3072, band 1 |
| 0x0F72 | 0x2C | 2 | 1 MSG | Funkgerät | 7000…7800 / 2600…12300 | msg 0 |
| 0x0F86 | 0x2C | 3 | 1 MSG | Funkgerät | 900…6900 / 12200…13000 | msg 0 |
| 0x0F9A | 0x2C | 4 | 1 MSG | — | -1350…-550 / 4300…14600 | msg 1 |
| 0x0FAE | 0x2C | 5 | 1 MSG | — | 1950…2750 / -2100…7000 | msg 1 |
| 0x0FC2 | 0x2C | 6 | 1 MSG | Regal | -3600…-400 / -1400…-600 | msg 2 |
| 0x0FD6 | 0x2C | 7 | 1 MSG | Geräte | -8250…-7450 / -1600…6600 | msg 3 |
| 0x0FEA | 0x2C | 8 | 1 MSG | Akten | 700…2300 / -3800…-3000 | msg 4 |
| **0x0FFE** | **0x2C** | **9** | **1 MSG** | **kleine Tür** | **-5850…-3850 / 6350…7350** | **msg 5** |
| 0x1012 | 0x50 | 11 | 9 ITEM | Item | — | typ 0x15, n 30, taken-bit 0xA2 |
| 0x1028 | 0x50 | 12 | 9 ITEM | Item | — | typ 0x15, n 30, taken-bit 0xA3 |
| 0x103E | 0x50 | 13 | 9 ITEM | Item | — | typ 8, n 7, taken-bit 0xA4 |
| 0x1054 | 0x50 | 14 | 9 ITEM | Item | — | typ 0x22, n 1, taken-bit 0xE5 |
| 0x106A/0x108C/0x10AE | 0x2D | Obj 1/2/3 | — | Requisiten | | (2650,-1248,7750) / (2550,-1248,-2850) / (-1450,-1300,3850) |

**Slot 9 hat keinen Nachbarn:** kein `Door_aot_set`, kein Item, kein
Obj-Modell liegt in seinem Rechteck x[-5850…-3850] z[6350…7350]. Die beiden
echten Türen sitzen ganz woanders (x 8800 bzw. x -5350 / z -2250).

## 4. Die einzige Bedingung im Raum betrifft NICHT die kleine Tür

`sub00` (RDT+0x44 = 0x10D4, Zeigertabelle 2 Einträge: 0x04 → sub00 @0x10D8,
0x6C → sub01 @0x1140):

```
0x10D8: 06 00 3e 00   If         -> sonst 0x111A
0x10DC: 21 03 66 00   Ck   zone=3 idx=102 val=0     (wahr, wenn Flag CLEAR)
0x10E0: 2c 0a 05 44 …  Aot_set slot 10 sce=5 EXAMINE  x[-5800…-3750] z[-1800…250]
0x10F4: 2d 00 04 …     Obj_model_set obj 0, rtype 4, pos (-6050,0,4950)
0x1116: 07 00 28 00   Else       -> 0x113E
0x111A: 2d 00 04 …     Obj_model_set obj 0, rtype 4, pos (-4850,0,-850)
0x113E: 01 00         Evt_end

sub01 @0x1140:
0x1140: 06 00 1a 00   If
0x1144: 21 03 66 00   Ck   zone=3 idx=102 val=0
0x1148: 2e 03 00      Work_set kind=3 idx=0
0x114C: 06 00 0c 00   If
0x1150: 3e 00 0f 00 0a 00  Member_cmp
0x1156: 22 03 66 01   Set  zone=3 idx=102 op=1 (Bit SETZEN)
0x115A: 08 00 / 0x115C: 08 00  EndIf / EndIf
0x115E: 01 00         Evt_end
```

`op_ck`-Semantik: `zone=pc[1]`, `idx=pc[2]`, `value=pc[3]`, Bedingung
`(flag!=0) XOR (value==0)` — `re15_port/engine/src/scd_vm.c:1817-1836`
(byte-true `LAB_8003fcf4`, `@0x8003fd48` / `@0x8003fd50`).

**Flag(3,102) steuert ausschließlich ein bewegliches Objekt (obj 0, rtype 4)
und dessen sce-5-Examine-Zone (Slot 10).** Slot 9 ist davon nicht berührt.

**Game-weiter Flag-Sweep** (4914 Flag-Opcodes über alle 240 RDTs, gelaufen mit
dem verifizierten Walker `re15_port/tools/aot_sce_census.py`): Flag zone 3 /
idx 102 wird **in keiner anderen RDT** gelesen oder geschrieben — nur die 6
Stellen in 10F0/10F1 oben. Der Raum hat sonst **keine weiteren Flag-Ops**.

## 5. Warum der Slot nie zur Tür werden kann

1. **Kein `Aot_reset` im Raum.** Der Zensus über alle 240 RDTs listet für
   10F0/10F1 **null** Records mit Opcode 0x46 (die einzige Operation, die einen
   installierten Slot umtypen kann — `re15_aot_retype`,
   `aot_common.c:211-256`, Original `@0x80040738`/`@0x8004076c`).
2. **Kein raumfremder Zugriff möglich.** Die AOT-Tabelle wird bei jedem
   Raumwechsel komplett genullt: `re15_aot_init()` → `memset(&g_aot, 0, …)`
   (`aot_common.c:36-39`), aufgerufen im Raum-Setup
   (`re15_port/engine/src/scd_room_setup.c:263`, byte-true an der Stelle des
   Original-Raumladers). Ein `Aot_reset` aus einem anderen Raum kann Slot 9
   also grundsätzlich nicht erreichen.
3. **Kein Schlüsselmechanismus vorhanden.** RE1.5-Türen tragen im Record kein
   Key-Item-Feld (`re15_aot.h:126-147`); eine gesperrte Tür wird im Spiel über
   `Door_aot_set` sce=0 (inert) + späteren `Aot_reset` sce=2 aufgeschlossen
   (game-weit 37 sce-0-Türen, davon 16 später scharfgeschaltet, 21 dauerhaft
   tot — `re15_aot.h:72-79`). ROOM10F0 hat **keine** sce-0-Tür.

## 6. Zielraum?

**Es gibt keinen.** Ein sce-1-Record hat kein Zielfeld.

Die beiden echten Türen des Raums zeigen auf existierende RDTs
(Formel `dest_id = ((dest_stage+1)<<12) | (dest_room<<4) | variante`,
`aot_common.c:493` / `:516`):

| Slot | pc[22] stage | pc[23] room | dest_id | RDT im CD-Baum |
|---|---|---|---|---|
| 0 | 0 | 0x0D | **0x10D0** | `STAGE1/ROOM10D0.RDT` (286 320 B) ✔ / `ROOM10D1.RDT` ✔ |
| 1 | 0 | 0x09 | **0x1090** | `STAGE1/ROOM1090.RDT` (336 440 B) ✔ / `ROOM1091.RDT` ✔ |

ROOM10F0 wird selbst nur aus ROOM10D0/10D1 (Slot 0) betreten (Zensus über alle
`Door_aot_set`-Ziele).

Nebenbefund aus demselben Zensus: In STAGE1 haben nur **ROOM1250** und
**ROOM1270** überhaupt keine eingehende Tür — beides sind keine plausiblen
„kleinen Büros" hinter dieser Wand (Slot 9 hat, wie gezeigt, kein Zielfeld,
das auf sie zeigen könnte).

## 7. Portcheck — verhält sich der Port identisch?

Ja, der sce-1-Pfad ist deckungsgleich:

- **Installation:** `re15_port/engine/src/scd_vm.c:2609-2631` — sce-1 →
  `RE15_AOT_TYPE_MESSAGE`, `msg = pc[14]` (= 5), `pause_mask16 = pc[16..17]`
  (= 0xFFFF). Genau die Bytes aus §1.
- **Auslösung:** `re15_port/engine/src/aot_common.c:1227-1229` —
  MESSAGE feuert nur bei `gen_reach && g_aot_action_pressed && !msg_block
  && !action_fired`, also **Aktionstaste (□) mit Forward-Reach**, passend zu
  `flags = 0x31` (Bit 0x10 = Action-Scan, Bit 0x20 = Forward-Test).
- **Wirkung:** `aot_common.c:1306-1316` — `re15_scd_show_message(event_id,
  pause_mask<<16)` und **explizit kein Raumwechsel** (Kommentar + Original-Zitate
  `LAB_80043084`, `@0x80043098`, `@0x800430a4`).

Erwartetes Verhalten im Port = im Original: Spieler steht in
x[-5850…-3850] / z[6350…7350], drückt □ → Text „It's a small office. It's
locked.", Welt friert bis zum Textende ein, danach passiert nichts weiter.

## 8. Offenes / nicht untersucht

- **Optik:** Ob an dieser Stelle im BSS-Hintergrund bzw. in der Raumgeometrie
  überhaupt ein Türblatt zu sehen ist, wurde nicht geprüft (rein statische
  Auswertung von RDT-Bytes; kein Render, kein Screenshot).
- **Absicht:** Ob die Tür im fertigen Spiel hätte funktionieren sollen, ist
  aus den Daten nicht belegbar. Belegt ist nur: im **Auslieferungsstand**
  (`info/Re1.5/PSX/`-Baum, gespiegelt in `shared_assets/PSX/`) existiert
  kein Öffnungspfad.
- **`Member_cmp` @0x1150** (`3e 00 0f 00 0a 00`) in sub01 wurde nicht bis auf
  die Feld-Semantik aufgelöst — für die Fragestellung irrelevant, da Flag(3,102)
  nachweislich nur Slot 10 und obj 0 steuert.

---

### Werkzeuge / Reproduktion

- Zensus: `python re15_port/tools/aot_sce_census.py <out.json>` → 240 Dateien,
  2295 Installs, **100,00 % Opcode-Coverage, 0 Desync-Stopps**.
- Message-Dekodierung: Zeichensatz aus `re15_port/engine/src/msg_common.c:177-201`,
  Tabellen-Lesart aus `:310-345` (`off[0]/2` Einträge, jeder ein Message-Start).
