# Sherry läuft nach ihrer Verschwinde-Cutscene noch herum — RECON

Nutzer-Report: „Sherry läuft nach der Cutscene wo sie verschwindet immer noch rum."
Stand 2026-08-29. ⛔ Reiner Recon — kein Code geändert, kein Build. Alle Zitate selbst
erhoben (RDT-Byte-Dumps mit begrenztem SCD-Walker, eigene MIPS-Disasm via
`re15_disasm.py`, Port-Quelltext im aktuellen Arbeitsbaum).

---

## 0. Kurzfassung

| Frage | Antwort |
|---|---|
| Wer ist „Sherry"? | **Entity-Typ 0x4B** (Leon-Route). ⚠️ NICHT 0x45 — die alte RE2-BioModels-Etikettierung (0x45=Sherry) trägt in RE1.5 nicht. |
| Raum/Cutscene | **ROOM11B0** (Parkhaus B1, „Where you first find Sherry"), **sub04** — Leon sieht das Mädchen, „H-hey!" (msg00), sie läuft weg. |
| Story-Flag | **Bank 3 (Story), Flag 0x82** — `22 03 82 01` @ROOM11B0.RDT **0x12A6** (erste Op von sub04). Gate: sub01 @0x11AA-11C0. |
| „Verschwinden" im Original | KEIN Entity-Kill, KEIN Spawn-Gate: **Pos_set-Park auf (-30000, 0, -30000)** @**0x13AC** — plus die **SCA-Wand-Klemme im NPC-Root-Tail** (`jal 0x8003b0a4` @0x8011e318), die die noch laufende Figur an der Raum-Außenwand festhält (unsichtbar). |
| Port-Divergenz | Die Wand-Klemme existiert im Port (`re15_npc_wall_clamp`), hängt aber **nur im Eskorte-Blatt (State 1)** — `enemy_ai_common.c:9624-9628`. Der **State-4-Executor (die Plc_dest-Walk-Subs) klemmt NIE** (`enemy_ai_common.c:9727-9732`), ebenso der run_all-NPC-Zweig (`enemy_ai_common.c:13162-13176`, nur `re15_npc_ai_tick`, kein Clamp/Push-Tail). Wird sie mitten im Skript-Walk geparkt, läuft sie von (-30000,-30000) **durch die Südwand zurück in den Raum** — sichtbar. |

---

## 1. Identität: Sherry = Typ 0x4B (Leon-Route)

Die alte Zuordnung in `RE15_NPC_AI.md` („0x45 = Sherry (EM045), BioModels-bestätigt")
stammt aus `info/.../BioModels-master/src/BioModels.h:153` — das ist die **RE2-Retail**-
Modelltabelle (Claire/PL01 etc.), deren EM-Nummern in RE1.5 NICHT gelten. Der Port
selbst behandelt 0x45 längst als **Irons** (ROOM1150; `re15_port/tests/unit/probe_irons_neck_1150.c:109`
lädt EM045 für den 1150-NPC) und 0x40 als **Marvin** (`analysis/marvin_10d0.md:36`),
0x42 als „Ada" (`analysis/nutzer_batch_2026-08-26/ada-folgt.md` §0).

Belege für 0x4B = Sherry:
1. **Szenario-Paarung** (game-weiter Sce_em_set-Sweep, 838 Records, eigener begrenzter
   Walker): Leon-Räume (\*0) spawnen die Begleiter **0x40+0x42**, Elza-Räume (\*1)
   **0x49+0x4B** — z.B. ROOM3000 (0x40) vs. ROOM3001 (0x49+0x4B), 3010/3011, 3020/3021,
   3070/3071, 3080/3081, 4000/4001, 6020/6021. Elzas Begleiter-Duo = John + **Sherry**
   (Kind); Sherry ist die konstante zweite Figur.
2. **Raum-Kontext**: `info/.../information74.txt:114`: Raum **11B** = „CHIKA CYUU B /
   Parking Garage B … **Where you first find Sherry**". ROOM11B0 spawnt 0x40/0x42/**0x4B**
   (main00 @0x1080/0x1094/0x10A8), ROOM11B1 0x4B/0x42/0x40.
3. **Dialog**: `RE15_Messages_Translated.txt:19ff` — room11B0/11B1 = „Sherry's
   Introduction Scene".
4. Modell: Typ 0x4B = CDEMD0.EMS Blob-Index 23 (`re15_port/engine/src/re15_ems.c`
   `s_ems_order[]`); der Nutzer erkennt das Kind-Modell als Sherry.

## 2. Spawn-Räume von Typ 0x4B (Sce_em_set, 20 Byte, Sweep über alle 6 Stages)

STAGE1: **ROOM11B0** main00 @0x10A8 (Leon), **ROOM11B1** main00 @0x1044 (Elza),
ROOM11A1 sub00 @0x11A0 (Elza). STAGE2: 2001, 2011 (Elza). STAGE3: 3001, 3011, 3021,
3061, 3071, 3081, 30E1 (Elza). STAGE4: 4001. STAGE5: 5011, 5021, 5031. STAGE6:
6021, 6030, 6031. **Für Leon in STAGE1 ist ROOM11B0 der einzige 0x4B-Raum.**
(Nutzer spielt Leon — ROOM1210-Batch, „Griff zieht Leon…".)

Spawn-Record ROOM11B0 main00 @Datei-Offset **0x10A8** (20 Byte):

    44 02 4B 40 00 00 00 FF D0 8A 00 00 D0 8A 00 00 F8 9B 00 00
    op slot=2 type=0x4B behavior=0x40(stationär) floor=0 …
    persist(pc[7])=0xFF  → Kill-Flag-Spawn-Gate UMGANGEN (@0x80042128)
    x=0x8AD0=-30000  y=0  z=0x8AD0=-30000  dir=0x9BF8

**Sie spawnt bereits GEPARKT bei (-30000, 0, -30000)** — außerhalb des Raums. Es gibt
KEIN Flag-Gate um den Spawn (main00 ist dort linear); Wieder-Betreten spawnt sie erneut
geparkt. Das „Verschwinden" ist also KEIN Spawn-Suppress — sie ist einfach off-map.

## 3. Die Cutscene (ROOM11B0 sub04) und ihr Story-Flag

Trigger (sub01, läuft jeden Frame), Datei-Offsets:

    0x11AA  06 Ifel_ck
    0x11AE  21 Ck   03 82 00      ; Bank 3 (Story) Flag 0x82 == 0  (Szene noch nicht gelaufen)
    0x11B6  21 Ck   04 06 01      ; Bank 4 Flag 0x06 == 1          (von main00 @0xF52-F5A armiert, wenn 04:F3)
    0x11BA  04 Evt_exec FF 18 04  ; → sub04

sub04 (0x12A6..0x145A) — die Verschwinde-Szene, Kern-Ops:

    0x12A6  22 Set 03 82 01            ; STORY-FLAG 3:0x82 = 1  (Einmaligkeit)
    0x12AA  22 Set 02 07 01            ; Cutscene-Klammer an
    0x12AE  22 Set 01 1B 01
    …Spieler (Work_set 01 00) wird positioniert und läuft Wegpunkte (Plc_dest mode 5)…
    0x1320  2E Work_set 02 02          ; em-Slot 2 = Typ 0x4B (Sherry)  [Port: Actor-Slot 3]
    0x1324  32 Pos_set 00 DA C5 00 00 10 A2   ; sie ERSCHEINT bei (-14886, 0, -24048)
    0x1330  04 Evt_exec FF 18 05       ; → sub05: ihr WEGLAUFEN
    0x1338  2B Message_on 00 00 00     ; msg00 = Leon: "H-hey!"  (Maske 0x0000 = non-blocking)
    …Spieler-Gesten (Plc_motion 0F) + Läufe (3 Wegpunkte mit Ankunfts-Wait Gosub 09)…
    0x1396  0F While 06 0A 00
    0x139A  23 Cmp 00 0A 05 08 00      ; warte bis work_vars[0x0A] != 8 endet, d.h. KAMERA-CUT == 8
                                       ; (work_vars[0x0A] = aktiver Cut; Port scd_vm.c:1298-1301)
    0x13A4  29 Cut_chg 06 / 3C Cut_auto 01
    0x13A8  2E Work_set 02 02          ; Sherry
    0x13AC  32 Pos_set 00 D0 8A 00 00 D0 8A   ; ⛔ DER "DESPAWN": Park auf (-30000, 0, -30000)
    …
    0x144A  22 Set 02 07 00 / 0x144E 22 Set 01 1B 00 / 0x1452 2E Work_set 01 00 / 0x1456 42 Plc_ret

sub05 (0x145A..0x1478) — ihr Weglaufen:

    0x145A  2E Work_set 02 02
    0x145E  22 Set 05 20 00            ; Ankunfts-Flag Bank5/0x20 löschen
    0x1462  40 Plc_dest 00 04 20 E7 E8 59 A2  ; WALK (Executor-Sub 4) → (-5913, -23975), Ankunfts-Flag 0x20
    0x146A  0F While … 21 Ck 05 20 00  ; warte auf Ankunft

Es gibt in sub04/sub05 **keinen Entity-Kill, kein Member_set(+0x0=0x8000), kein
Aot/Em-Reset auf sie** — das Verschwinden IST der Pos_set-Park @0x13AC.
(ROOM11B1/Elza hat KEINEN solchen Park — Byte-Scan `32 00 D0 8A 00 00 D0 8A` = 0 Treffer;
dort bleibt Sherry ja bei Elza. Der Mechanismus ist Leon-spezifisch.)

## 4. Original-Mechanismus: warum sie danach WIRKLICH weg ist

### 4.1 Der Park trifft sie (rechnerisch) mitten im Lauf — in BEIDEN Versionen

Ihr Lauf: 8973 Einheiten bei Tempo **50/Tick** (Tabelle @**0x80076c00**, selbst
gedumpt aus PSX.EXE: `[75,48, 75,48, 75,48, 75,48, 70,48, …, 50,48(idx22=Typ 0x4B), …]`
→ byte[(0x4B-0x40)*2] = **50**; Port identisch: `s_npc_walk_param[11]=50`,
enemy_ai_common.c:8964ff) + Align-Phase (yaw 3064→~0, Slew 96/Tick ≈ 11 Ticks)
≈ **190+ Ticks**. Die Skript-Strecke von Lauf-Start (0x1330) bis Park (0x13AC):
Sleeps 30+30+31 + Spieler-Läufe (~5941+893+2661 Einheiten @200/Tick ≈ 48 Ticks
+ Align/Ankunfts-Overhead) + While(Cut==8) ≈ **150-180 Ticks**.
**HYPOTHESE (gerechnet, nicht gemessen):** der Park landet typischerweise 10-40 Ticks
VOR ihrer Ankunft → ihr Walk-Sub (State 4 / +0x5=4, Phase 2) bleibt SCHARF mit Ziel
(-5913,-23975), und sie läuft ab dem Park von (-30000,-30000) Richtung Raum zurück.
(Pos_set schreibt nur +0x34/38/3C — LAB_80041048 — und cancelt den Walk nicht.)

### 4.2 Was das Original dann rettet: die SCA-Wand-Klemme im NPC-Root-Tail

Typ-0x4B-Root (STAGE1): Dispatch @0x80072bac[0x4B] = **0x8011e22c** — Registrierung
selbst disassembliert @0x8011e960-6c (`addiu v0,v0,-7636` = 0x8011e22c →
`sw v0,11480(at)` = 0x80072cd8). Der Root-TAIL läuft NACH dem State-Dispatch, für
**jeden** State (eigene Disasm, STAGE1.BIN):

    8011e2e0: jal 0x8002b498      ; Kontakt-Clear
    8011e2f0: jal 0x8002aec4      ; Body-Push (Spieler)
    8011e2f8: jal 0x8002b544      ; Body-Push (Entities)
    8011e30c: lw  v0,120(a0)      ; v0 = entity+0x78 (Hitbox)
    8011e310: ori a2,zero,0x4     ; Solid-Maske 4
    8011e314: lhu a1,6(v0)        ; Radius = box+6 (= 450, Kasten @0x80121658: 00 00 06 fa 00 00 c2 01 fa 05 c2 01)
    8011e318: jal 0x8003b0a4      ; ⛔ SCA-WAND-KLEMME
    8011e31c: addiu a0,a0,52      ; a0 = entity+0x34 (Position)

(Identischer Tail in Root 0x8011c5a0/Typ 0x40 @0x8011c688-698 und 0x8011cb70/Typ 0x42
@0x8011cc58-68.)

Und ROOM11B0 HAT eine Wand zwischen Park-Punkt und Raum — SCA-Sektion @0xDE4
(Header 24 B, 19 Einträge à 12 B), Süd-Wand-Band:

    x=-28800  z=-28720  w=32500  d=2000  type=1  u0=0xFF  floor=3
    (Zellen: Westwand x=-29100 z=-29100 w=2000 d=15900, Nordwand, Ostwand analog)

Ihr Rückweg (-30000,-30000)→(-5913,-23975) kreuzt dieses Band; die Klemme (Maske 4,
u0=0xFF blockt alles) hält sie **außerhalb** des Raums fest — sie tritt auf der Stelle
gegen die Außenwand, unsichtbar, für immer. Ergebnis im Original: „sie ist weg".
Beim Wieder-Betreten: Spawn erneut geparkt (persist=0xFF egal), sub01-Gate `Ck 03 82`
verhindert die Szene — sie bleibt off-map.

## 5. Port-Stand: warum sie im Port weiterläuft

Alle Bausteine des Skripts sind im Port korrekt: Work_set kind 2 → Actor idx+1
(scd_vm.c:2786-2817), Pos_set auf die Work-Entity (scd_vm.c:1889-1913), Plc_dest
setzt für die NPC-Familie state=4/+0x5=mode (scd_vm.c:2170-2219), der Walk-Sub läuft
in `re15_npc_sub_walk` (enemy_ai_common.c:9027, Phase 2: Steer + `re15_dog_advance`
@:9089, Ankunft d<100 → Flag(5,bit) + Sub 6 @:9095-9101), `work_vars[0x0A]` = aktiver
Kamera-Cut (scd_vm.c:1298-1301), `op_cmp` op 5 = `!=` (scd_vm.c:3011-3035), und die
NPC-Hitbox 450/1530 ist seit dem „Ada kann durch alles durchlaufen"-Fix für die ganze
Familie gesetzt (re15_damage.c:2169-2171).

**Die Divergenz ist die PLATZIERUNG der Wand-Klemme:**

1. `re15_npc_wall_clamp` (enemy_ai_common.c:9360-9366, byte-Zitat @0x8011cc58-68 im
   Kommentar) wird **nur** im Eskorte-Blatt gerufen — State 1, und dort auch nur im
   `grid_id&0xf==0`-Zweig: enemy_ai_common.c:**9624-9628**
   (`decide; exec; re15_npc_wall_clamp(e, ox, oz);`). Der decide0-Zweig (:9604-9612)
   returnt OHNE Klemme.
2. `re15_npc_ai_tick` **case 4** (der Executor, in dem alle Plc_dest-Walks laufen)
   ruft `re15_npc_executor(e)` **ohne** ox/oz-Erfassung und **ohne** Klemme:
   enemy_ai_common.c:**9727-9732**.
3. Der run_all-NPC-Familienzweig ruft nur `re15_npc_ai_tick(s)`:
   enemy_ai_common.c:**13162-13176** — anders als Zombie/Hund/Spinne/Maggot
   (`re15_enemy_sca_clamp(e, ox, oz, 4u)` @:13066/:13109/:13132/:13160) gibt es dort
   weder Klemme noch Body-Push-Tail.

Folge in ROOM11B0: Park @0x13AC trifft sie im Walk-Sub → sie läuft ab (-30000,-30000)
mit scharfem Ziel (-5913,-23975) **ungeklemmt durch das Südwand-Band zurück in den
Raum** (~23 s sichtbarer Marsch quer durchs Parkhaus), erreicht das Ziel und bleibt
dort als Idle stehen — exakt „Sherry läuft nach der Cutscene wo sie verschwindet
immer noch rum".

(Die Yield-Gates verhindern das nicht: `in_motion_pose = state==4 && sub<=9` →
kein SCD-Yield, der Executor tickt sie weiter; enemy_ai_common.c:9693-9698.)

## 6. Fix-Vorschlag (byte-true)

Die Klemme an die **Root-Position** heben — exakt wie der Original-Tail, der NACH dem
State-Dispatch für JEDEN State läuft (@0x8011e2d8-e318, s. §4.2):

- In `re15_npc_ai_tick` (enemy_ai_common.c:9631): `ox/oz` VOR dem `switch (e->state)`
  erfassen und NACH dem switch **einmal** `re15_npc_wall_clamp(e, ox, oz)` rufen
  (Radius kommt bereits aus `hit_radius_min` = box+6 = 450, re15_damage.c:2169-2171).
- Den Blatt-Aufruf @9627 dorthin falten (kein Doppel-Klemmen), damit auch der
  decide0-Zweig (:9604-9612) und der default-Zweig klemmen — im Original klemmt der
  Tail alle.
- Die frühen Returns (Pause-Gate, grid&0x20, SCD-Yield) klemmen NICHT — deckungsgleich
  mit dem Original, dessen Pause-Gate den Tail überspringt (`bne …,0x8011e360`-Muster,
  vgl. @0x8011c5c0/e0 im 0x40-Root).
- Optional/separat (gleicher Tail, eigenes Finding): Body-Pushes 0x8002aec4/0x8002b544
  + Kontakt-Clear 0x8002b498 fehlen der NPC-Familie ebenfalls.

**Wache (Pin auf den ZUSTAND, nicht den Weg** — Memory `reai-v2-pin-prueft-zustand-nicht-weg`):
Probe lädt ROOM11B0.RDT, seedet den 0x4B-Actor byte-true in die Park-Mitten-im-Walk-Lage
(state=4, sub_state_1=4, sub_state_2=2, steer=(-5913,-23975), pos=(-30000,0,-30000),
Hitbox via `re15_enemy_apply_hitbox`), tickt ~800 Frames `re15_enemy_ai_run_all` und
verlangt: z bleibt dauerhaft ≤ -28720+Radius (sie überquert das Südwand-Band NIE) UND
die Klemme ist nachweislich gelaufen (Zähler == Tick-Zahl, nicht nur Endposition).
Zweiter Teil: voller Skript-Lauf (main00 + sub01→sub04) und dieselbe Assertion.

## 7. Offenes

1. **Race nicht gemessen**: dass der Park sie mitten im Lauf trifft, ist gerechnet
   (§4.1), nicht am Emulator/Port gemessen. Für die Abnahme: Proben-Lauf (oben) bzw.
   DuckStation-Vergleich (re15-parity-verify). Der Fix ist davon unabhängig richtig —
   die Klemme gehört für jeden State in den Tail; kommt sie doch rechtzeitig an,
   ändert die Klemme nichts (byte-true no-op).
2. **Elza-Variante ROOM11B1**: kein Park-Pos_set — die Szene endet anders
   (Sherry bleibt). Sub-Layout dort bei Bedarf separat RE'en.
3. **Meta**: `RE15_NPC_AI.md` trägt noch die falschen RE2-Namen (0x45=Sherry,
   0x4B=Katherine). Richtigstellen (0x40=Marvin, 0x42=„Ada", 0x45=Irons,
   0x47=Elliot, 0x49/0x4B=Elza-Duo John/Sherry), sonst rennt die nächste Suche
   wieder nach ROOM1150.
4. Der zweite Park @0x141A ist der Spieler-Cutaway (wird @0x142A zurückgesetzt) —
   kein Handlungsbedarf.
5. Body-Push-Tail für NPCs (s. §6 optional) als eigenes Finding führen.
