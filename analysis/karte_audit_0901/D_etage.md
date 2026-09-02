# D — Schaltet die Karte die Etage im echten Spiel um?

Prüfauftrag: den Fix „Etage = `re15_collision_band_from_y(Spieler-Y)` statt
`g_actors[SLOT_PLAYER].floor`" nachprüfen, den zweiten Verdacht (KI/AOT lesen dasselbe
tote Feld) belegen oder verwerfen, den Weg mit einer Sonde nachstellen.

Alle Zahlen unten sind SELBST GEMESSEN (Sonden `d_etage.c`, `d_1060.c`, `d_floorgate.c`,
`d_enemy.c`, `d_arrive.c`, `d_ramp.c` im Scratchpad, gelinkt gegen
`re15_port/build_fix/engine/libre15_engine.a`) oder disassembliert aus `ghidra1_V2.txt`
bzw. `RE_15_Quellcode_Overlays/`.

---

## 0. Der eigentliche Befund: `player.floor` und das Kollisionsband SIND EIN BYTE

Der Aufrufer des ACTION-Scans übergibt den Spieler-Entity-Zeiger:

```
80031fdc  ec 02 04 26   addiu  a0 => DAT_800aca54, s0, 0x2ec     ; s0 = 0x800ac768
80031fe0  01 00 05 34   ori    a1, zero, 0x1
80031fe4  eb 0a 01 0c   jal    FUN_80042bac                       ; ACTION-Scan(player,1,0x10)
80031fe8  10 00 06 34   _ori   a2, zero, 0x10
```

→ **Spieler-Entity-Basis = 0x800AC768 + 0x2EC = 0x800ACA54.**

Unabhängige Gegenprobe (steht schon im Port zitiert, `game_step_common.c:988`):
`addiu s0,s0,-13740` @0x80012E04 = 0x800B0000 − 0x35AC = **0x800ACA54**, danach
`sb v0,4(s1)` = player+0x4 = das cmd-Wort.

Feld-Abgleich mit den bekannten Globals — alle passen exakt:

| Offset | Adresse | bekannt als |
|---|---|---|
| +0x05 | 0x800ACA59 | `DAT_800aca59` Spieler-Modus |
| +0x07 | 0x800ACA5B | `DAT_800aca5b` Treppen-Subphase |
| +0x34/38/3c | 0x800ACA88/8C/90 | `DAT_800aca88` X/Y/Z (die Overlay-Distanztests nehmen `&DAT_800aca88`) |
| +0x6a | 0x800ACABE | `DAT_800acabe` Yaw-Rest |
| **+0x82** | **0x800ACAD6** | **`DAT_800acad6` = das Kollisionsband** |
| +0x94 | 0x800ACAE8 | Clip-Index (Memory `model-anim-groundtruth`) |
| +0x95 | 0x800ACAE9 | Anim-Cursor (`lbu v0,149(t0)` @0x8001f35c) |

**Im Original gibt es also nur EIN Byte.** Der Port hat daraus ZWEI gemacht:
`s_coll_band` (`re15_collision.c:171`) und `g_actors[0].floor` — und pflegt nur das
erste. Gemessen: `g_actors[SLOT_PLAYER].floor == 0` in **allen 38 STAGE1-Räumen** nach
Raum-Init + 30 SCD-Ticks (`d_floorgate.exe`). Geschrieben wird es im ganzen Port nur in
`re15_savedata.c:187` (Spielstand laden) und über den generischen SCD-Member-Setter
`actor_common.c:159`.

---

## 1. Ist `band_from_y(Spieler-Y)` im Spiel die Etage?

### Die Schreiber von DAT_800acad6 im Original (alle `sb`-XREFs)

| Adresse | Wert | Port |
|---|---|---|
| 0x8001D7A4 / 0x8001D8F8 | Tür-/Raum-Init: `= Tür-Byte +0xb`; direkt daneben `DAT_800aca8c = band * -0x708` (die Spieler-Y!) | `room_common.c:395`, `aot_common.c:561` — vorhanden |
| 0x80038208 | Klettern: `= band + 1` (gleichzeitig `p->y -= 1800`) | `climb_common.c:415` — vorhanden |
| 0x800384BC | Absprung: `= -(h/1800)` — **h ist die BODENHÖHE aus `room_coll`, nicht die Spieler-Y** | `climb_common.c:483` — vorhanden |
| 0x800385E0 | Landen auf Objekt: `= -(top/1800)`, `top = obj.y - 1800` | `climb_common.c:503` — vorhanden |
| **0x80038A28** | **Treppe AUFWÄRTS Init: `= 7`** (`ori v0,zero,0x7` @0x80038A18) | **FEHLT im Port** |
| **0x80038CE4** | **Treppe ABWÄRTS Init: `= 7`** (`ori v1,zero,0x7` @0x80038CD4) | **FEHLT im Port** |
| 0x80038C40 / 0x80038EDC | Treppen-Ende: `= -(DAT_800acc0e / 0x708)` | `stair_common.c:141` — vorhanden |

Die Division @0x80038E8C-EDC ist byte-verifiziert:
`lh v1,DAT_800acc0e` → `subu v1,zero,v1` → `mult v1, 0x91A2B3C5` → `mfhi / addu / sra 10 / subu`.
0x91A2B3C5 = ceil(2^42/1800) − 2^32, also exakt `(-Y)/1800` = **`re15_collision_band_from_y`**.

### Antwort

**Im Ruhezustand ja, unterwegs nein.**

* Am Ende jedes Treppenlaufs ist `DAT_800acad6` per Konstruktion `-(Y/0x708)` — dort ist
  `band_from_y(Y)` byte-true.
* **Während** des Laufs steht im Original die **7** (kein Stockwerk), im Port bleibt das
  globale Band auf der Startetage, und `band_from_y(Y)` liefert die Zwischenwerte 3
  bzw. 1. Gemessen (ROOM1170, X-Treppe 4→2): T32 y=−5400 → `band_from_y`=3, glob=4,
  Original=7. **Drei verschiedene Werte.** Für die KARTE folgenlos, weil das Inventar
  während der Treppe verriegelt ist (`game_step_common.c:937 … && !re15_stair_active()`),
  für alles andere nicht.
* Die Kletter-Pfade @0x800384BC/@0x800385E0 setzen das Band aus der **Bodenhöhe**, nicht
  aus der Spieler-Y. Solange jede Bodenhöhe ein Vielfaches von 1800 ist, stimmen beide
  überein (gemessen in ROOM1090: Kistendeckel −3600, Y −3600, Band 2 — kein Unterschied).
  Ein Gegenbeispiel habe ich nicht gefunden; die Formel des Originals ist aber eine
  andere, deshalb ist die Y kein Ersatz für das Byte, nur ein meist richtiger Schätzer.

### Band an jedem Treppenabschnitt vs. was die Türen sagen

**ROOM1060 (Treppenhaus)** — Türen (`Door_aot_set` pc[4] = Band, pc[23] = Zielraum):

| Türband | Zielraum | dessen Kartenseite | Titel |
|---|---|---|---|
| 0 | 0x1040 | 2 | 1F |
| 4 | 0x10C0 | 3 | 2F |
| 8 | 0x1120 | 4 | 3F |

Treppenzonen (gemessen): Bänder 0→2→4→6→8, acht Zonen in vier Paaren. Gemessene
Kartenseite je Band (`d_1060.exe` / `d_etage.exe`):

```
Band 0 → Seite 2 "1F"      Band 5 → Seite 3 "2F"
Band 1 → Seite 2 "1F"      Band 6 → Seite 3 "2F"   (Zwischenpodest)
Band 2 → Seite 2 "1F"      Band 7 → Seite 4 "3F"
Band 3 → Seite 3 "2F"      Band 8 → Seite 4 "3F"
Band 4 → Seite 3 "2F"      Band 9 → Seite 4 "3F"
```

Die drei Tür-Bänder 0/4/8 treffen exakt 1F/2F/3F. **Der Nutzer-Fall „im Treppenhaus ganz
oben bei 3F zeigt die Karte immer noch 1F" ist damit erklärt und behoben:** mit
`actor.floor` (immer 0) gab `re15_map_floor_lookup(0x1060, 0, 0)` **immer** Seite 2 =
„1F", auf jedem Band. Die Zwischenpodeste 2 und 6 fallen bei Gleichstand auf die UNTERE
Etage (`d < bestd`, Tabelle nach Band sortiert) — eine Setzung, kein Beleg.

**ROOM1170 (Helipad)** — Türen: Band 4 → Selbst-Tür ROOM1170 (spawn_y −7200 = Band 4);
Band 0 → ROOM1240 **und** ROOM1130. Treppen: X-Treppe Hof(4)↔Plattform(2),
Z-Treppe Plattform(2)↔Grube(0). Gemessener Lauf (`d_etage.exe`):

```
X-Treppe Hof(4) -> Plattform(2)
  VOR dem Start        y=-7200  band=4  SEITE 5 "(DACH)"
  Abschnitt T32        y=-5400  band=3  SEITE 5 "(DACH)"
  Abschnitt T62        y=-3600  band=2  SEITE 4 "3F"      <-- schaltet um
  NACH dem Lauf T63    y=-3600  band=2  SEITE 4 "3F"
X-Treppe Plattform(2) -> Hof(4):  SEITE 4 "3F" -> SEITE 5 "(DACH)"
Z-Treppe 2 <-> 0:                 bleibt SEITE 4 "3F"  (kein eigener Etageneintrag)
```

**Der Nutzer-Fall „beim Treppe runterlaufen von ROOM1170 schaltet er die Karte nicht nach
3F um" ist behoben** — mit `actor.floor` = 0 lieferte `re15_map_floor_lookup(0x1170, 1, 0)`
immer Seite 4; die Karte konnte den Dach-Zustand gar nicht erst zeigen und ihn danach
auch nicht verlassen.

Der Pin `test_map_re2_system` läuft mit der Änderung durch (32 PASS, Abschluss „OK").

---

## 2. Der ZWEITE Fehler — er ist REAL

Das Original vergleicht in der Gegner-KI das **globale Spielerband** gegen das +0x82 des
Gegners. Wörtlich, `RE_15_Quellcode_Overlays/STAGE1_overlay.c` (7×: Zeilen 533, 765, 849,
1011, 1478, 1502, 1892):

```c
(DAT_800acad6 == *(char *)(_DAT_800ac784 + 0x82))
```

`_DAT_800ac784` ist der laufende Gegner (`sw s1,0x0(a0)=>DAT_800ac784` @0x80042C24).
Weil 0x800ACAD6 = Spieler+0x82 ist (§0), ist der Port-Ausdruck `player->floor == e->floor`
**formal richtig — aber er liest ein Feld, das der Port nie beschreibt.**

Dasselbe für den AOT-Bandgate, byte-verifiziert:

```
80042cac  lbu v0,0x2(s0)       ; s0 = AOT-Record, Byte 2 = Band
80042cb4  andi v0,v0,0x80
80042cb8  bne  v0,zero,+0x18   ; Bit 0x80 -> Gate aus
80042cc0  lbu v1,0x82(s1)      ; s1 = die SCANNENDE Entität (= der Spieler)
80042ccc  bne v1,v0,LAB_8004301c
```

`aot_common.c:755/1077/1132` nimmt dafür `g_actors[es].floor` — für den Spieler also
konstant 0. (Der Tür-Gate `aot_common.c:1009` nimmt dagegen `re15_collision_debug_band()`;
die beiden Pfade widersprechen sich im Port, im Original sind sie dasselbe Byte.)

### Gemessene Auswirkung (STAGE1, `d_enemy.exe`)

| Raum | Gegner | deren +0x82 | Port-Gate `pl.floor==e.floor` | Original, Spieler auf derselben Ebene |
|---|---|---|---|---|
| ROOM10B0 | 4× Zombie 0x10, y=−1800 | 1 | **0 — nie** | 1 |
| ROOM1200 | 2× Zombie 0x10, y=−1800 | 1 | **0 — nie** | 1 |
| ROOM1090 | 7× Typ 0x26 (Feuer), y=−1800 | 1 | **0 — nie** | 1 |
| ROOM11A0 | NPC 0x40, y=−1800 | 1 | **0 — nie** | 1 |
| ROOM1190 | 3× Hund 0x20 | 2 | 0 | Hund nutzt `e->floor >= pl->floor` → 2≥0 wahr, unauffällig |
| ROOM11C0 | 2× Gorilla 0x27 | 2 | 0 | dito `>=` |

Der Gate steht in den **Grab-Commit**-Zweigen des Zombies (`enemy_ai_common.c:378/442/464`,
Girl-Row-11 @0x80103650-68, `enemy_ai_re2_zombie.c:2710/2901/3008/3104/3126`,
`enemy_ai_re2_dog.c:428/458/514/678/1216`). Konkret: **die vier Zombies in ROOM10B0 und
die zwei oberen in ROOM1200 können den Spieler im Port niemals packen** — die
Gleichheitsbedingung ist 0 == 1.

Ankunftsbänder (aus `spawn_y` aller Türen, `d_arrive.exe`) bestätigen, dass der Spieler
dort tatsächlich auf Band ≠ 0 steht: ROOM1090 → Band 1 oder 5, ROOM11A0 → Band 1,
ROOM1170 → Band 4 oder 0, ROOM1060 → Band 0/4/8. (ROOM10B0 und ROOM11C0 haben keine
STAGE1-Tür auf sich; ihre SCA-Zellen führen die Bänder 0/1/5 bzw. 0/1.)

Nebenwirkung im AOT-Stempel: von 16 Spieler-Pool-Wasser/Rampen-AOTs in allen sechs Stages
ist genau einer betroffen — **ROOM20A0 Slot 5 (Wasser, Band 1) wird im Port nie auf den
Spieler gestempelt.** Klein, aber derselbe Defekt.

**Urteil: der zweite Fehler ist real, mit Gameplay-Wirkung, und er ist NICHT durch die
Karten-Änderung entstanden — er war schon vorher da.**

---

## 3. Sonde

`d_etage.c` stellt den Weg komplett nach: ROOM1170 laden (`scd_register_room_events` +
`scd_room_reenter` + 10 VM-Ticks), Spieler auf die Treppenzone setzen,
`re15_stair_try_start` / `re15_stair_tick` wie `probe_stair_1170.c`, und nach **jedem**
committeten Abschnitt `re15_inv_map_page_shown()` ausgeben. `d_1060.c` macht dasselbe für
ROOM1060. Ergebnisse siehe §1 — die Erwartung der Beleglage (1170: Band 4 → Seite 5,
Band 0 → Seite 4; 1060: 0/4/8 → 2/3/4) wird an jedem Punkt getroffen.

---

## 4. Empfehlung — was noch fehlt

1. **Das doppelte Byte zusammenführen (die eigentliche Reparatur).**
   `re15_collision_set_band()` soll `g_actors[RE15_ACTOR_SLOT_PLAYER].floor`
   MITSCHREIBEN (Beleg: Spieler+0x82 = 0x800ACAD6, §0). Dann sind Karte, AOT-Stempel und
   alle KI-Gates in einem Zug richtig, und `aot_common.c:1009` (globales Band) und
   `aot_common.c:1077` (`.floor`) widersprechen sich nicht mehr.
2. **Die Karte soll das BAND lesen, nicht `band_from_y(Y)`.** Nach 1. also
   `g_actors[…].floor` bzw. `re15_collision_debug_band()`, mit Rückfall auf `band_from_y`
   solange der Wert −1 ist (`re15_collision_reset_band`). Grund: das Original leitet das
   Band bei den Kletter-Pfaden aus der BODENHÖHE ab (@0x800384BC/@0x800385E0), nicht aus
   der Spieler-Y; und während der Treppe steht dort die 7, nie ein Zwischenwert.
3. **Fehlender Original-Schreiber: `= 7` beim Treppen-Start** (@0x80038A28 aufwärts,
   @0x80038CE4 abwärts). Ohne ihn bleibt der Port während des Laufs auf der Startetage,
   das Original ist „auf keiner Etage". ⚠ Vorher prüfen, was im Port sonst noch am Band
   hängt (Tür-Gate, Kamera-Zonen-Gate `FUN_80014230` @0x80014288, Prop-Push @0x8002BF38) —
   die verlieren dann während der Treppe alle ihren Treffer, genau wie im Original.
4. **Offen (nicht aus D entscheidbar):** ROOM1170s Band 0 trägt ZWEI Türen mit
   verschiedenen Zielseiten — ROOM1130 (Seite 4 „3F") und ROOM1240 (Seite 1 „B2"). Der
   Generator hat Seite 4 gewählt; die Quelle dieser Wahl gehört belegt oder die Regel
   dokumentiert. Ebenso die Gleichstands-Regel für Zwischenpodeste (ROOM1060 Band 2/6,
   ROOM1170 Band 2) — sie fällt derzeit auf die untere Etage, das ist eine Setzung ohne
   Original-Beleg.
