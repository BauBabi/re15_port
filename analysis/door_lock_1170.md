# ROOM1170 — Tür am Treppenende („It's locked from the other side.")

**Report (Nutzer):** Die Tür am Treppenende rechts in ROOM1170 ist im Port begehbar;
im Original ist sie verschlossen und zeigt „It's locked from the other side."

**Stand:** Original-Mechanismus vollständig aus den RDT-Bytes + EXE-Disasm belegt (§1–§3).
Port-Parse/Install-Pfad byte-für-byte geprüft und per Probe gemessen (§4). Live-Messung §5.

---

## 1. DATEN — ROOM1170.RDT, alle Door_aot_set-Records (main00 @Datei 0x11f4)

RDT-Adresstabelle: msg @0x1920, mainScd @0x11f4, subScd @0x13cc (Datei-Offsets;
Header-Layout RE15_KNOWLEDGE.md §1.1). Alle Offsets unten = SCD-relativ zu main00.

| SCD-Off | Datei-Off | Record | Slot | sce | Rect (x,z,w,d) | Ziel | Gate |
|---|---|---|---|---|---|---|---|
| 0x0012 | 0x1206 | Door_aot_set | 0 | 2 | (1500,14400,2100,1700) | **1170 selbst**, cut 11, Spawn (−11710,−7200,−26500) | — (Rolltor/Hof-Selbsttür) |
| 0x0032 | 0x1226 | Door_aot_set | 1 | **0=INERT** | (5040,−9400,1240,2920) | ROOM10B0 | — (nie scharf in main) |
| 0x00a8 | 0x129c | Door_aot_set | 2 | 2 | (0,0,0,0)-Sentinel | ROOM1240, Spawn (−26214,0,−3861) | `Ck(3,0xc1)==0` (Intro-Handoff) |
| 0x00d8 | 0x12cc | Door_aot_set | 3 | 2 | (0,0,0,0)-Sentinel | 1170 selbst | `Ck(3,0x7d)==0` (Narrator-Handoff) |
| 0x0104 | 0x12f8 | Door_aot_set | 4 | 2 | (−22370,−18380,2280,910) | **ROOM1130 cut 7** | — (immer Tür) |
| 0x012c | 0x1320 | **Aot_set sce=1** | 5 | 1 | (−22740,−27610,2280,1210) | — MESSAGE **msg 0x0c** | `Ck(4,0xc3)==0` (IF-Zweig) |
| 0x0144 | 0x1338 | Door_aot_set | 5 | 2 | (−22740,−27610,2280,1210) | **ROOM1140 cut 6**, Spawn (−7250,0,−1200) | ELSE-Zweig (`flag(4,0xc3)==1`) |
| 0x0166 | 0x135a | Door_aot_set | 6 | 2 | (−11940,−28450,1750,1200) | 1170 selbst (zurück zum Helipad-Teil) | — |

Der IF/ELSE-Block im Wortlaut (main00, Datei 0x1318 ff.):

```
0x0124  Ifel_ck       06 00 1c 00          ; -> skip 0x0144
0x0128  Ck            21 04 c3 00          ; flag(zone 4, bit 0xc3) == 0 ?
0x012c  Aot_set       2c 05 01 31 00 00 2c a7 26 94 e8 08 ba 04 0c 00 ff ff 00 00
                      ;  slot5 sce=1(MESSAGE) flags=0x31 band=0
                      ;  rect x=-22740 z=-27610 w=2280 d=1210
                      ;  payload u16@0 = 0x000C = msg-Index, u16@2 = 0xFFFF (pause)
0x0140  Else_ck       07 00 26 00          ; -> 0x0166
0x0144  Door_aot_set  3b 05 02 31 00 00 2c a7 26 94 e8 08 ba 04 ae e3 00 00 50 fb
                      00 04 00 14 06 00 00 00 00 00 00 00
                      ;  slot5 sce=2(DOOR), gleiches Rect, next=(-7250,0,-1200)
                      ;  ndir=0x0400, stage=0 room=0x14 -> ROOM1140, cut=6
```

**Identifikation „Treppenende rechts":** Die vier Treppen-AOTs (Aot_set sce 12/13,
Slots 7–10) führen Band 4 → 2 → 0 (Hof → Zwischenpodest → Erdgeschoss;
Handover 2026-08-02 §2b: Treppe 1 (−20690,−25595) … Treppe 4 (−25670,−20645)).
Im Erdgeschoss (Band 0) liegen genau ZWEI Türen: Slot 4 (−21230,−17925 Mitte)
→ ROOM1130 (immer offen) und Slot 5 (−21600,−27005 Mitte) → ROOM1140 —
die einzige Tür des Raums mit Lock-Mechanik, und ihre Message ist wörtlich der
gemeldete Text (§2). Elza-Variante ROOM1171: dieselbe Tür ist Slot 4
(main @0x00e2 Aot_set sce=1 msg 4 / @0x00fa Door_aot_set → room 0x14 cut 6),
Gate ebenfalls `Ck(4,0xc3)`.

## 2. Die Message — msg-Index 0x0c im .msg-Block

.msg-Block @Datei 0x1920, 17 Pointer (u16-LE). `msg[12]` @Block+0x267 = Datei **0x1B87**
(Bytes datei-verifiziert):

```
04 02 25 50 3a 4f 00 48 4b 3f 47 41 40 00 42 4e 4b 49 00 50 44 41 00 4b 50 44 41 4e 00 4f 45 40 41 57 01
= <04:Timer><LF> "It's locked from the other side." <END>
  (Encoding: Byte = ASCII − 0x24 → 0x25='I', 0x50='t', 0x3a=Apostroph-Glyphe,
   0x00=Space, 0x57=Punkt-Glyphe '{'; 0x02=LF, 0x04=Dauer-Code, 0x01=Terminator)
```

Der sce-1-Payload `0c 00` (u16@0 des AOT-Payloads) IST dieser Index. In ROOM1171:
msg[4] mit identischem Text.

## 3. ORIGINAL-MECHANIK — es gibt KEIN Lock-Byte im Door-Record

### 3.1 Installer: beide Opcodes legen NUR den rohen Record ab

`Aot_set` 0x2C @LAB_80040534 und `Door_aot_set` 0x3B @LAB_800405bc sind identisch
aufgebaut — sie speichern `pc+2` als Live-AOT-Record in die Slot-Tabelle DAT_800ac9b0:

```
80040578 lw  v0,0x1c(a0)        ; thread->pc
80040580 addiu v0,v0,0x2        ; Record = pc+2  (rec[0]=sce, rec[1]=flags, rec[2]=band, rect @+4)
80040584 sw  v0,0x0(v1)         ; DAT_800ac9b0[slot*4] = Record-Pointer
80040590 lbu v0,0x3(v1); andi 0x80; beq -> pc += 0x14 (kurz) / 0x1c (4P)   ; (0x2C)
80040618 lbu v0,0x3(v1); andi 0x80; beq -> pc += 0x20 (kurz) / 0x28 (4P)   ; (0x3B)
```

„Verschlossen" ist also reine DATENWAHL: derselbe Slot bekommt entweder das
sce-1-Record (MESSAGE) oder das sce-2-Record (DOOR). Ein Key/Lock-Feld wie in
RE2-Retail (`key_id`/`key_type` im DOOR_AOT_SET) existiert im RE1.5-Record nicht —
die Tail-Bytes +25..+31 des 1170-Slot-5-Records sind `04 00 00 00 00 00 00` (nfloor=4, Rest 0).

### 3.2 Action-Scan: Dispatch über das sce-Byte

FUN_80042bac (ACTION-Scan, kind=0x10 = Square-Press):

```
80042f48 lbu v0,0x0(s0)         ; rec[0] = sce
80042f50 beq v0,zero,...        ; sce 0 = inert -> Skip
80042f74 lbu v0,0x0(s0)
80042f7c sll v0,v0,0x2
80042f84 lw  v0,[0x8007469c + sce*4]   ; Handler-Tabelle PTR_LAB_8007469c
80042f8c jalr v0                        ; a0 = rec+0x14 (Polygon) bzw.
80042fb8   (a0 = rec+0xC)               ;      rec+0xC  (Rechteck) = PAYLOAD
```

### 3.3 Handler sce=1 (MESSAGE) @LAB_80043084 — „locked"-Pfad

```
80043084 addiu sp,sp,-0x18
80043088 move  v0,a0            ; a0 = Payload (rec+0xC)
8004308c clear a0               ; arg0 = 0
80043090 ori   a1,zero,0x300    ; arg1 = 0x300
80043098 lhu   a3,0x2(v0)       ; pause-Bits (hier 0xFFFF)
8004309c lhu   a2,0x0(v0)       ; MSG-INDEX  (hier 0x000C)
800430a0 jal   FUN_80027e68     ; Message-Anzeige (Room-.msg-Block)
800430a4 _sll  a3,a3,0x10
```

→ Square an der verschlossenen Tür zeigt msg[0x0c] an und wechselt KEINEN Raum.

### 3.4 Handler sce=2 (DOOR) @LAB_800430bc — Tür-Pfad (prüft NICHTS)

```
800430bc ori v0,zero,0x1
800430c4 sw  a0,DAT_800ac9a8    ; Payload-Pointer der Tür merken (next_pos/stage/room/cut)
800430d4 sb  v0,DAT_800b5359    ; Transition-Latch = 1
800430d8 lw  v0,DAT_800aca40
800430e0 lui v1,0xff00
800430e4 sw  v0|0xff000000,DAT_800aca40   ; Fade/Übergangs-Bits
```

12 Instruktionen, kein Lock-Test — der Handler feuert IMMER, wenn er dispatcht wird.
Der Lock kann daher NUR über die Datenwahl in §3.1 kodiert sein. q.e.d.

### 3.5 Ck-Prädikat (Gate) @LAB_8003fcf4

```
8003fcf4 lw  v0,0x1c(a0); lbu a2,0x1(v0)  ; zone
8003fd00 lhu a1,0x2(v0)                   ; u16: low=bit-Index, high=want
8003fd24 [0x80074664 + zone*4]            ; Zonen-Tabelle (zone 4 -> Bank @+0x10)
8003fd10 srl v1,a1,0x3; andi v1,0x1c      ; Wort-Byte-Offset = (idx>>3)&0x1c
8003fd40 srlv v0,0x80000000,idx&0x1f      ; MSB-first-Maske
8003fd48 sltu v0,zero,v0&Wort             ; Bool
8003fd50 xor v0,(want==0)                 ; Ergebnis = (flag!=0) XOR (want==0)
```

Der Port implementiert exakt dieselbe Packung (`game_state.c:29-43`: word=idx>>5,
mask=0x80000000>>(idx&0x1f) — äquivalent, (idx>>3)&0x1c ist der Byte-Offset desselben Worts).

## 4. Wer setzt flag(4,0xc3)? — Census über alle 240 RDTs

Byte-genauer SCD-Walk (beide Zweige) über alle main/sub aller RDTs; Treffer für
zone=4, idx=0xc3:

```
1140 main[00]@0x0026 Ck  (4,0xc3)==0   ; Briefing-Raum, EIGENE Seite derselben Tür
1140 sub[02] @0x0190 Set (4,0xc3)=1    ; <- der UNLOCK
1141 main[00]@0x0026 Ck  / sub[05]@0x04e6 Set   (Elza-Variante)
1170 main[00]@0x0128 Ck  (Leon) / 1171 main[00]@0x00de Ck (Elza)
```

ROOM1140-Seite (main @Datei 0xa50):

```
0x0022 Ifel_ck; 0x0026 Ck(4,0xc3)==0
0x002a   Door_aot_set slot1 sce=0 (INERT!)  rect(-8250,-750,2000,1000) -> room 0x17 (1170) cut 9
0x004a   Aot_set      slot2 sce=3 (EVENT)   gleiches Rect, payload ff 00 18 02 -> sub_scd[2]
0x005e Else_ck
0x0062   Door_aot_set slot1 sce=2 (Tür scharf) -> 1170 cut 9
```

sub[02] (Action an der Tür von INNEN):

```
0x0190 Set(4,0xc3,1)                  ; Unlock-Flag
0x0194 Message_on 2b 00 ff ff         ; msg[0]
0x0198 Evt_next
0x019a Aot_reset 46 01 02 31 4f aa 00 00 8f 9b   ; Slot 1 -> sce=2, Payload = next_pos
                                                  ; (-21937,0,-25713) = Spawn in 1170
0x01a4 Evt_end
```

Das Original-Design: Die Tür ist von der 1170-Seite verschlossen, bis der Spieler sie in
ROOM1140 von innen öffnet (Square an der Tür → sub02 → `Set(4,0xc3,1)` + Aot_reset).
Das Flag persistiert (Spielstand-Flags), danach ist die Tür von beiden Seiten offen.

## 5. Census — dieselbe Lock-Mechanik game-weit

36 sce-1/Door-Zwillingspaare (Slot+Rect identisch) über alle 240 RDTs, u.a.:

| Raum | Slot | Gate | Message |
|---|---|---|---|
| 1170/1171 | 5/4 | Ck(4,0xc3) | „It's locked from the other side." |
| 11A0/11A1 | 0 | Ck(3,0x97) | „It's locked from the other side." |
| 11A0/11A1 | 4 | Ck(3,0xce) | „It's locked from the other side." |
| 2030/2031 | 2 | Ck(3,0xdb) | „It's locked from the other side." |
| 3060/3061 | 2 | Ck(4,0xc4) | „It's locked from the other side." |
| 6000/6001 | 2 | Ck(3,0x33) | „It's locked from the other side." |
| 10D0/10D1 | 0 | Ck(3,0x32) | „Communication Room — electronically locked …" |
| 11E0/11E1 | 3 | Ck(3,0x8b) | „Prisons — electronically locked …" |
| 1230/1231 | 4 | Ck(3,0x89) | „Weapon Storage — electronically locked …" |
| 3010/3011 | 6 | Ck(3,0x3b) | „It's locked. An ID card is required …" |
| 1040/10C0/1120/… | div. | Ck(4,0xf3) | Fahrstuhl ohne Strom |

Kein einziges dieser Records benutzt ein Lock-Byte im Door-Record — ausnahmslos das
IF/ELSE-Zwillingsmuster. Ein Fix muss also NUR den generischen Pfad (Ck/Ifel/Else +
Aot_set sce=1 + Door_aot_set) korrekt ausführen; Spezialfälle pro Tür gibt es nicht.
FLAG_CHG-AOTs (sce=4) schreiben game-weit AUSSCHLIESSLICH zone 5 (Census §4-Skript) —
kein AOT kann zone 4 setzen; zone-4-Writes kommen nur aus SCD `Set` (und Save-Load).

## 6. PORT-IST

- `op_door_aot_set` (scd_vm.c:3038) liest pc[1..24]; die Tail-Bytes +25..+31 liest er
  nicht — **korrekt**, denn sie tragen keinen Lock (§3.1). Er installiert sce==0 als
  NONE (scd_vm.c:3090) und die Tür sonst über `re15_aot_set_door`.
- `op_aot_set` sce=1 (scd_vm.c:2289-2295) installiert `RE15_AOT_TYPE_MESSAGE` mit
  `event_id = pc[14]` (= msg-Index) — entspricht LAB_80043084.
- Scan: MESSAGE-Fall zeigt msg und wechselt keinen Raum (aot_common.c:1035-1042);
  DOOR-Fall feuert nur auf Square-Edge (aot_common.c:946).
- `op_ck`/`op_set` (scd_vm.c:1587/1608) = byte-true LAB_8003fcf4/LAB_8003fdd0.

**Probe-Messung** (`tests/unit/probe_door_1170.c`, echte ROOM1170.RDT, frische Flags):

```
== Variante Flag(4,0xc3)=0 ==            == Variante Flag(4,0xc3)=1 ==
slot 5: type=MESSAGE ev=0x0c             slot 5: type=DOOR -> ROOM1140 cut 6
        rect c=(-21600,-27005)                   spawn=(-7250,0,-1200)
```

Der Port nimmt auf frischem Spielstand also den KORREKTEN Zweig. (Live-Lauf §7.)

## 7. Live-Messung — Divergenz REPRODUZIERT

Autopilot-Lauf (2026-08-03): Titel → NEW GAME → 1240-Narrator → 1170-Intro → Tor →
Hof, mit `RE15_FLAG_CENSUS=1 RE15_AOT_DUMP=1` (stderr landet via `freopen` in
`debug.log`, main.c:1588 — NICHT im Shell-Redirect!). Ergebnis:

```
[flagcensus] room=1240 WRITE Set  zone=3 idx=139 op=1      ; 1240-eigene SCD-Sets
[flagcensus] room=1240 WRITE Set  zone=1 idx=27  op=1
[flagcensus] room=1240 WRITE Set  zone=2 idx=7   op=1
[flagcensus] room=1170 READ  Ck   zone=4 idx=195 exp=0 got=1   ; <- BEIM ERSTEN 1170-main00
[aot] --- Raum 1170 ---
[aot]  5 DOOR  Mitte(-21600,-27005) halb( 1140,  605) ev=0 sce=31 band=0 -> ROOM1140 cut=6
```

**flag(4,195) ist im Live-Lauf schon beim ersten 1170-main00 GESETZT** — obwohl im
gesamten Census KEIN `Set zone=4 idx=195` auftaucht (nur die bekannten idx=242-Sets
aus sub02/sub11). Der Schreiber läuft also am SCD-`Set`-Pfad (und damit am Census)
vorbei. Slot 5 installiert deshalb als scharfe Tür nach ROOM1140 — exakt der
Nutzer-Report („begehbar", keine Message).

## 8. ROOT CAUSE — Boot-Pre-Stage in main.c

`re15_port/platform/pc/main.c:2391-2402` ([RL-1]-Block, läuft bei JEDEM Boot mit
boot_room 0x1240 (= NEW GAME) oder 0x1170):

```c
if (boot_room == 0x1170 || boot_room == 0x1240) {
    re15_game_flag_set(3, 193, 1);        /* Intro-Keystone (sub03-Vorbesuch) — ok */
    /* „Außenbereich door hub: … door 6 … + door 5 … + examines only register
     *  when (4,195)==1 … (4,195)==0 makes the outdoor area a DEAD END." */
    re15_game_flag_set(4, 195, 1);        /* <- DER BUG */
}
```

Die Kommentar-Begründung ist eine FEHLLESUNG der Blockstruktur: laut Bytes (§1)
umfasst der `Ifel_ck/Else_ck`-Block NUR Slot 5 —
`Else_ck 07 00 26 00` @0x0140 springt nach 0x0140+0x26 = **0x0166**, und dort
STEHT `Door_aot_set slot6` bereits HINTER dem Block (unconditional), ebenso die
Treppen-AOTs 7–10 (@0x0186 ff.) und Tür 4 → ROOM1130 (@0x0104, VOR dem Block).
Mit (4,195)==0 ist der Außenbereich also KEIN Dead-End (Probe §6: Türen 4/6 +
4 Treppen installieren; nur Slot 5 wird MESSAGE) — der Pre-Stage „löst" ein
Problem, das die Daten nie hatten, und schaltet dabei die verschlossene Tür frei.

Warum der Census nichts sah: main.c ruft `re15_game_flag_set` direkt (C-Ebene) —
`[flagcensus]` loggt nur die SCD-Opcodes `Ck`/`Set` (scd_vm.c:1594/1618).

Referenz-Suchlauf: `flag_set(4, 195` hat GENAU EINE Fundstelle im Port
(main.c:2401); kein Test hängt daran.

## 9. Fix-Plan

1. **main.c:2401 löschen** (`re15_game_flag_set(4, 195, 1);`) und den Kommentar
   2397–2400 durch den Befund ersetzen (Ifel/Else umfasst nur Slot 5; Unlock kommt
   byte-true aus ROOM1140 sub02 `Set(4,0xc3,1)` @sub-SCD 0x0190, §4).
   Die `(3,193)`-Zeile bleibt (separater Intro-Keystone, dokumentiert [RL-1]).
2. Sonst NICHTS: `op_ck` (= LAB_8003fcf4), `op_aot_set` sce-1 (= LAB_80043084),
   `op_door_aot_set`/`re15_aot_retype` (= LAB_800405bc / LAB_80040738) und der
   Action-Scan-Dispatch (= FUN_80042bac @0x80042f48-90) sind byte-true belegt und
   von der Probe bestätigt. Damit sind automatisch ALLE 36 Lock-Zwillinge
   game-weit korrekt (§5) — der Fix ist global, ohne Tür-Sonderfälle.
3. **Verifikation nach dem Fix:**
   a. `probe_door_1170` (frisch: MESSAGE 0x0c / Flag gesetzt: DOOR 1140) — bleibt grün.
   b. Live-Lauf wie §7: Census muss `Ck zone=4 idx=195 exp=0 got=0` zeigen,
      AOT-Dump `5 MESSAGE ev=12`; Square an (−21600,−27005) → Untertitel
      „It's locked from the other side.", KEIN Raumwechsel.
   c. Unlock-Runde: in ROOM1140 an der Innenseite Square (sub02: Message + 
      `Set(4,0xc3,1)` + Aot_reset Slot 1) → zurück nach 1170 → Slot 5 = DOOR,
      Durchgang nach 1140 cut 6.
   d. Regression: Intro-Montage 1240/1170 läuft unverändert ((3,193) unberührt);
      Tür 4 → ROOM1130 unverändert (Rolltor-Route/Harness §2b).

## 10. Offene Punkte

- DuckStation-Gegenprobe der Original-Seite (Tür anlaufen → Message hören/sehen)
  steht aus — statisch ist der Mechanismus vollständig belegt (§1–§4), und der
  Nutzer-Report bestätigt das Original-Verhalten unabhängig.
- Die Autopilot-Route bleibt am bekannten Hof-Hindernis hängen (Handover §2b
  „OFFEN"; Pendeln um x≈−11 400/−12 300, z≈−24 900/−26 700 — gemessen in diesem
  Lauf), daher wurde der Square-Druck an der Tür live nicht erreicht; die
  Divergenz ist über Flag-Census + AOT-Dump trotzdem vollständig gemessen.
- ROOM1240 setzt per eigener SCD `Set(3,139,1)` — idx 0x8b wird u.a. von ROOM11E0
  (Prisons-Lock) gelesen. Datengetrieben identisch im Original (nur notiert,
  kein Port-Thema).
