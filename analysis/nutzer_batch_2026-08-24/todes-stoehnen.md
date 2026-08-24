# Dossier — „Finales Todes-Stöhnen von Leon fehlt beim Fress-Finisher (Zombie + Hund)"

Nutzer-Report 2026-08-24 (RE2-KI-Modus): *„finales todes Stoehnen von Leon fehlt beim Zombie
Finisher, wenn sie ihn fressen. Bei den Hunden genauso."*

Status: **RE + Messung abgeschlossen. Ursache belegt. KEINE Code-Änderung in dieser Phase.**

---

## 0. Kurzfassung

| Frage | Antwort (belegt) |
|---|---|
| Welcher SE ist das Todes-Stöhnen? | `Se_on(0x04030001)` = **Bank 4 (CORE), Record 3**, Positional-Flag 1 |
| Was steckt hinter CORE-Record 3? | `SOUND/CORE00.EDH` EDT-Record 3 = `00 00 44 17` → Programm 0 / Tone 4 → **VAG 12592 B ≈ 0,50 s**, SPU-Stimme 7, Prio 4. Mit Abstand die längste CORE-Probe (2,7× die nächstlängste). Stimme 7 ist der Leon-Sprach-Kanal (CORE 0/1/2 = Griff-Schrei / Flinch-Schrei / Knockback-Schrei). |
| Zombie-Trigger | `jal 0x80045024` **@0x8010a864**, Frame-Gate `player+0x95 == 0x37` **@0x8010a820-24** (FUN_8010a6f8, Phase [1]) |
| Hund-Trigger | `jal 0x80045024` **@0x80111df4**, Frame-Gate `player+0x95 == 0x3a` **@0x80111db4-b8** (Maschine B @0x80111cf0, Phase [1] @0x80111d6c) |
| Spielt der Port ihn? | **RE1.5-Modus JA** (gemessen, byte-genau auf Frame 55 bzw. 58). **RE2-Modus NEIN** — weder Zombie noch Hund. |
| Wo genau fehlt es im Port? | `re15_port/engine/src/enemy_ai_common.c`: RE2-Hund-Zweig **Z. 1186–1196** und RE2-Fress-Kollaps-Zweig **Z. 1226–1275** enthalten **keinen einzigen Audio-Aufruf**. Der RE1.5-Zweig hat ihn (`re15_audio_core_se(3)`, **Z. 1293**), der RE1.5-Hund-Zweig ebenso (**Z. 1209**). |

Das deckt sich exakt mit dem Report: **im RE2-KI-Modus stirbt Leon im Fress-Finisher stumm.**

---

## 1. RE — Wer spielt was, mit Adressen

### 1.1 Se_on-Argumentformat (FUN_80045024, PSX.EXE)

```
80045024: addiu sp,sp,-88
80045028: srl   v1,a0,24            ; v1 = BANK   = a0>>24
8004505c: addiu at,at,8684          ; 0x800b21ec  = Bank-Handle-Tabelle
80045060: addu  at,at,v1
80045064: lb    a1,0(at)
80045068: addiu v0,zero,-1
8004506c: beq   a1,v0,0x8004539c    ; Handle == -1 -> Bank nicht resident -> SKIP
80045078: srl   v0,a0,16
8004507c: andi  s4,v0,0xff          ; s4 = RECORD = (a0>>16)&0xff
80045080: andi  a0,a0,0xff          ; p  = a0&0xff  (0 = nicht-positional, !=0 = positional)
80045094: sltiu v0,v1,0x6           ; Banks 0..5
800450b4: jr    0x80010e70[bank]
```

Bank-Dispatch @0x80010e70 (8 Einträge, gültig 0..5):

| Bank | Handler | EDT-Tabelle | Bound |
|---|---|---|---|
| 0 | 0x800450bc | `0x801fdd00` | `sltiu s4,0x21` |
| 1 | 0x800450d0 | `0x801fcd00` (ARMS/Waffe) | `sltiu s4,0x21` |
| 2 | 0x800450e4 → 0x80045130 | `*(DAT_800ac778+0x08)` = RDT-**snd0** | `sltiu s4,0x21` |
| 3 | 0x800450f8 | `*(DAT_800ac778+0x14)` = RDT-**snd1** | `sltiu s4,0x19` |
| 4 | 0x8004511c | `0x801fbd00` (resident **CORE**) | `sltiu s4,0x21` |
| 5 | 0x80045130 | `*(DAT_800ac778+0x08)` = snd0 (== Bank 2) | — |

`DAT_800ac778` ist der RDT-Basiszeiger: `@0x80039700-40` lädt `DAT_800ac780` und schreibt ihn
nach `0x800ac77c`, `0x800ac778` und `0x800bbeb0`. Der `p`-Parameter entscheidet
@0x800451c0-cc: `p != 0` → `jal 0x80045a64` mit der Emitter-Position (a1), `p == 0` → der
nicht-positionale Zweig @0x800451dc.

⇒ **`0x04030001` = Bank 4 (CORE), Record 3, positional.**

### 1.2 Zombie-Fress-Kollaps — FUN_8010a6f8 (STAGE1.BIN, Phase-Var `DAT_800aca5a`)

Phasen: 0 @0x8010a744 (fällt in 1), 1 @0x8010a7c0, 2 @0x8010a8a8.

**Phase 0 — Einstieg:**
```
8010a744: sb v0,0(a0)                ; DAT_800aca5a := 1
8010a74c: lbu v0,-13581(v0)          ; player+0x9f
8010a75c: sb  zero,-13591(at)        ; player+0x95 (Frame) := 0
8010a760: addiu v0,v0,6              ; Clip = player+0x9f + 6   (= 6 oder 7)
8010a768: sb v0,-13592(at)           ; player+0x94 := Clip
8010a798: sb  zero,-13597(at)        ; player+0x8f := 0 (harter Schnitt)
8010a7a0: jal 0x80019700             ; Einstiegs-Blut, a0 = 0x1500 (@0x8010a770)
8010a7b8: jal 0x80045024             ; Se_on(0x04010001)  = CORE 1  (a0 @0x8010a7a8/b4)
8010a7bc: addiu a1,a1,52             ; a1 = g_entity(cur)+0x34 = Zombie-Position
```

**Phase 1 — Frame-Kadenz auf `player+0x95` (DAT_800acae9):**
```
8010a7d8: jal 0x8001ad68             ; Wurzel-Platzierung (jeden Tick)
8010a7e0: lui v1,0x800b
8010a7e4: lbu v1,-13591(v1)          ; v1 = player+0x95
8010a7e8: ori v0,zero,0x23
8010a7ec: bne v1,v0,0x8010a824       ; != 0x23 -> ueberspringen
8010a7f4: lui a0,0x207
8010a7fc: lw  a1,-14460(a1)          ; a1 = g_entity(cur) = der ZOMBIE
8010a800: ori a0,a0,0x1              ; a0 = 0x02070001  (Bank 2 = RDT-snd0, Record 7)
8010a804: jal 0x80045024
8010a808: addiu a1,a1,52             ; a1 = entity+0x34
8010a80c: addiu v0,zero,-1
8010a814: sh  v0,-13586(at)          ; player+0x9a (HP) := -1
8010a818: lui v1,0x800b
8010a81c: lbu v1,-13591(v1)          ; v1 = player+0x95 (neu geladen)
8010a820: ori v0,zero,0x37
8010a824: bne v1,v0,0x8010a870       ; != 0x37 -> ueberspringen
8010a82c: ori a0,zero,0x2000         ; Blut-Skalierung
8010a84c: jal 0x80019700             ; Blut-Burst, Anker Bone 8 (acbdc + 0x5A0)
8010a854: lui a0,0x403
8010a85c: lw  a1,-14460(a1)          ; a1 = ZOMBIE
8010a860: ori a0,a0,0x1              ; a0 = 0x04030001  ** CORE 3 = TODES-STOEHNEN **
8010a864: jal 0x80045024
8010a868: addiu a1,a1,52             ; a1 = entity+0x34 = Emitter-Position
8010a880: jal 0x8001f314             ; anim_set(a3=0x200); Phase += Rueckgabe
8010a89c: sb v1,-13734(at)           ; DAT_800aca5a += done
```

**Phase 2 @0x8010a8a8:** `sb 7 -> 0x800aca58` (player+0x4 = 7 = Leiche/Game-Over-Zustand,
Handler @0x8003694c) und `sb 0 -> 0x800aca59`.

Das Gate ist eine **strikte Gleichheit** auf einem monoton wachsenden Frame-Zähler in einer
Phase, die per `anim_set` weiterzählt ⇒ **genau ein Auslöser pro Kollaps.**

### 1.3 Hund-Fress-Kollaps — Maschine B @0x80111cf0 (Phasen @0x801002d4, 5 Einträge)

Phase [0] @0x80111d28: `DAT_800aca5a := 1`, `player+0x94 := 4` (@0x80111d44),
`player+0x95 := 0`, `player+0x96 := 0`, `player+0x8f := 0`, Flags `|= 0xc0`. **Kein SE.**

Phase [1] @0x80111d6c, Frame-Kadenz auf `player+0x95`:
```
80111d8c: lbu v1,-13591(v1)          ; player+0x95
80111d94: ori v0,zero,0x4f
80111d98: bne v1,v0,0x80111db8
80111da0: ori a0,zero,0x2
80111da4: jal 0x80045630             ; FUN_80045630(2,0) = Boden-Material-SE (Koerperfall)
80111dac: lbu v1,-13591(v1)
80111db4: ori v0,zero,0x3a
80111db8: bne v1,v0,0x80111dfc       ; != 0x3a -> ueberspringen
80111dbc: ori a0,zero,0x2000
80111ddc: jal 0x80019700             ; Blut-Burst (Bone 8)
80111de4: lui a0,0x403
80111dec: lw  a1,-14460(a1)          ; a1 = der HUND
80111df0: ori a0,a0,0x1              ; a0 = 0x04030001  ** CORE 3 = TODES-STOEHNEN **
80111df4: jal 0x80045024
80111df8: addiu a1,a1,52
80111e00: lbu v1,-13591(v1)
80111e04: ori v0,zero,0x29
80111e08: bne v1,v0,0x80111e3c
80111e30: jal 0x80019700             ; zweiter Blut-Burst, KEIN SE
80111e4c: jal 0x8001f314             ; anim_set; Phase += done
```
Phase [2] @0x80111e74: Wundstempel FUN_80037edc(0,0xa)/(5,0x32)/(7,0x32), dann
`sw 7 -> 0x800aca58` @0x80111ea0.

**Der Hund benutzt exakt denselben SE wie der Zombie — nur das Frame-Gate ist ein anderes
(0x3a statt 0x37), weil sein Kollaps-Clip 90 statt 65 Frames hat.**

### 1.4 Spielweite Zählung von `Se_on(0x04030001)`

Vollständiger Scan aller `jal 0x80045024`-Stellen in `PSX.EXE` + allen `PSX/BIN/*.BIN` mit
Rückverfolgung des a0-Aufbaus (Skript: Scratchpad `seon_scan.py`). **15 Fundstellen, alle in
Spieler-Kill-/Kollaps-Maschinen, KEINE in der EXE, keine in TITLE/DEBUG:**

```
STAGE1: 0x8010a864 (Zombie-Devour)  0x80111df4 (Hund)  0x8011c4b8 (4. Greifer, Tabelle @0x80100414)
STAGE2: 0x8010a6f8
STAGE3: 0x8010a950  0x801108d4  0x80116148
STAGE4: 0x8010a818  0x801117e8  0x80116434  0x80116704
STAGE5: 0x8010a998  0x80111968  0x801165b4  0x80116884
```
Zum Vergleich, die anderen CORE-Sprachproben:
* `0x04000001` (CORE 0) — **Griff-Einstieg**: 0x8010a3e4 (Zombie cmd-5), 0x80111a50 (Hund Maschine A P0)
* `0x04010001` (CORE 1) — **Kollaps-Einstieg / Flinch-Clip 8**: 0x8010a7b8 + EXE-Damage-Pfad
* `0x04020001` (CORE 2) — **Knockback-Clip 9**
* `0x04030001` (CORE 3) — **nur Tod**

### 1.5 Was CORE-Record 3 tatsächlich ist (Datenmessung)

`SOUND/CORE00.EDH`, gelesen mit dem **Port-eigenen** Decoder (`re15_edt_decode` +
`re15_edt_resolve_layers_ex` + `re15_vab_parse`, Scratchpad `coreedt.exe`):

```
pBAV=0x40  edt_records=16   vab: vags=12
CORE se0  00001417 prog=0 tone=1 prio=4 voice=7  -> vag1( 3984 B = 0.158 s @44k1)
CORE se1  00002417 prog=0 tone=2 prio=4 voice=7  -> vag2( 4592 B = 0.182 s)
CORE se2  00003417 prog=0 tone=3 prio=4 voice=7  -> vag3( 2928 B = 0.116 s)
CORE se3  00004417 prog=0 tone=4 prio=4 voice=7  -> vag4(12592 B = 0.500 s)   <== TODES-STOEHNEN
CORE se4  00005300 prog=0 tone=5 prio=3 voice=-16 -> vag9( 1360 B = 0.054 s)  (UI/Direkt-Kanal)
CORE se5..se10                                   -> 0.03..0.23 s  (UI/Direkt-Kanal)
CORE se11..se15  ffffffff = leer
```

Belege für „Leon-Stimme":
* `se0..se3` teilen **SPU-Stimme 7** (`byte3 & 0x1f − 0x10 = 0x17 − 0x10 = 7`) — ein eigener,
  reservierter Kanal; `se4..se10` laufen auf dem Direkt-Kanal (voice −16/−15).
* `se0/se1/se2` sind über ihre Aufrufstellen als Griff-/Flinch-/Knockback-Schrei belegt
  (Port: `game_step_common.c:104-105` mappt Clip 8 → CORE 1, Clip 9 → CORE 2, mit den
  EXE-Adressen @0x80035e80/@0x80036004).
* `se3` ist mit 0,50 s die mit Abstand längste Probe der Bank (2,7× `se1`) und wird
  **ausschließlich** in Kollaps-/Kill-Maschinen gespielt (§1.4).

### 1.6 Der zweite, kleinere Aufruf: `Se_on(0x02070001)` @0x8010a804 (Frame 0x23)

Das ist **nicht** das Todes-Stöhnen, sondern **Bank 2 = RDT-snd0, Record 7** — ein
raum-abhängiger SE, direkt vor dem `hp := -1`-Store.

Census über alle 240 RDT in `shared_assets/PSX/STAGE*` (snd0-EDT = RDT-Header +0x08,
Record 7 = EDT+28..31, leer wenn `byte2==0 && byte3==0` — der Test des Ports selbst,
`vab_common.c:263`):

```
240 RDT | 34 zu klein/ohne Sound | rec7 LEER: 163 | rec7 belegt: 43
belegt u.a.: ROOM1090/1091, 10F0/10F1, 1170/1171, 11A0/11A1, 1260/1261,
             ROOM2000..20B1 (ganz STAGE2), ROOM3050..30B1
LEER u.a.:   ROOM1140 (Zombie-Raum), ROOM1190 (Hund), ROOM10D0
```
Beispiel ROOM1170: `snd0[7] = 00 00 45 10` → prog 0 / tone 4 → VAG 6688 B ≈ 0,27 s.

⇒ In den STAGE1-Kampfräumen ist dieser Aufruf **auch im Original stumm**; in STAGE2/STAGE3
spielt er einen raumeigenen SE. Er ist eine echte, aber nachrangige Portlücke (der Port hat
den Bank-2-Pfad `re15_audio_room_se_snd0()` bereits fertig verdrahtet — er wird an dieser
Stelle nur nicht gerufen). Der Kommentar in `enemy_ai_common.c:1284` („still deferred")
beschreibt genau diesen Aufruf — **nicht** das Stöhnen.

---

## 2. MESSUNG — spielt der Port ihn heute?

Alle Messungen mit **echten Assets** und der **echten Engine-Lib** (`libre15_engine.a` +
`libre15_test_support.a`, dessen `re15_audio_core_se`/`re15_audio_room_se_snd0`/
`re15_audio_room_se` Spione sind). Sonden liegen im Scratchpad (kein Repo-Code):
`probe_se_devour.c` (Kopie von `tests/unit/probe_re2z_devour.c` + SE-Log + RE1.5-Bank-Lader),
`probe_se_dog.c` (Kopie von `tests/unit/probe_dog_devour.c` + SE-Log + Flavor-Schalter).

### 2.1 Zombie — ROOM1140, echtes `game_step`, echter SCD-Spawn

**RE1.5-Flavor, RE1.5-Bänke (`shared_assets/PSX/EMD/CDEMD0.EMS`):**
```
### SE-MESSUNG  Flavor=RE1.5
   [RE1.5-Bank EM10: victim_ok=1 clips=14  c6=65f c7=65f]
== seed 0: GRIFF ab f70 (Zombie slot 2 typ 0x10) ==
   *** SE f70   vs=1 clip=3 afr=0  : CORE se=0        <- Griff  @0x8010a3e4
   *** SE f86   vs=2 clip=7 afr=0  : CORE se=1        <- Kollaps-Einstieg @0x8010a7b8
   *** SE f126  vs=2 clip=7 afr=40 : SND1 se=3        (Zombie-eigener SE)
   *** SE f141  vs=2 clip=7 afr=55 : CORE se=3   ***  <- TODES-STOEHNEN, Frame 55 = 0x37, GENAU EINMAL
(seed 1 identisch: CORE 3 @ afr=55)
```
⇒ Im RE1.5-Modus ist das Stöhnen **byte-genau auf dem Original-Frame** vorhanden.

**RE2-Flavor (RE2-Bänke, 116-Frame-Kollaps-Clip 13/15):**
```
### SE-MESSUNG  Flavor=RE2
== seed 0: GRIFF ab f59 (Zombie slot 2 typ 0x10) ==
   *** SE f80   vs=2 clip=15 afr=0   : CORE se=1
   *** SE f228+ vs=2 clip=15 afr=115 : SND1 se=3 (x18, Zombie-Fress-Loop)
SUMMARY: 2 Fress-Kollapse gemessen
```
⇒ **CORE se=3 kommt in 900 Frames × 2 Seeds NIE.** Kein SND0-Ereignis. Bestätigt.

### 2.2 Hund — `probe_dog_devour`-Harnisch, EM020-Victim-Bank (Clip 4 = 90 Frames)

**RE1.5-Flavor:**
```
### Flavor=RE1.5
EM020 geladen: VICTIM ok=1 clips=5  fc[4]=90
   *** SE t138  vs=2 clip=4 afr=58(0x3a) : CORE se=3   ***  <- @0x80111df4, GENAU EINMAL
  Kollaps: Leon motion=4, frame@state7=89 (= fc[4]-1, cmd7 @0x80111ea0)
  Blut: Kollaps-Spawns=2: (frame=0x29 scale=0x2000) (frame=0x3a scale=0x2000)
  Tod: t_state7=170 hp_after=-1
```

**RE2-Flavor (Kollaps über `re15_player_victim_devour()` erzwungen, weil der
RE1.5-Sub-9-Griff des Harnischs im RE2-Zweig nicht greift):**
```
### Flavor=RE2
  Kollaps: Leon motion=0, frame@state7=28
  Tod: t_state7=89 hp_after=-1
  (KEINE einzige SE-Zeile — weder CORE noch SND0)
```
⇒ Der RE2-Hund geht **komplett stumm** vom Griff bis zum Tod: der Einstiegs-SE entfällt schon
byte-true (`if (zombie->type == 0x20)`-Zweig in `re15_player_victim_devour`, Original
FUN_801100b4 spielt dort tatsächlich nichts), und der Kollaps-SE fehlt.

### 2.3 Nebenbefund aus der Messung: Clip-Längen

`re15_emd_parse_victim_bank` über `EMD/CDEMD0.EMS` (Scratchpad `vclips.exe`):
```
EM10/EM11/EM16 victim: clips=14  [0]=14 [1]=26 [2]=20 [3]=14 [4]=21 [5]=20 [6]=65 [7]=65 ...
EM13           victim: clips=14  [6]=65 [7]=65
EM20 (Hund)    victim: clips=5   [0]=29 [1]=18 [2]=60 [3]=30 [4]=90
EM21 (Kraehe)  victim: clips=3   [0]=14 [1]=36 [2]=20
```
Damit sind die Original-Gates erreichbar: Zombie 0x23=35 und 0x37=55 < 65; Hund 0x29=41,
0x3a=58, 0x4f=79 < 90. Die Klammer `if (fc - 1 < blood_fr) blood_fr = fc - 1;`
(`enemy_ai_common.c:1285`) greift mit echten RE1.5-Bänken also **nie**. (Sie greift sehr wohl,
wenn versehentlich eine RE2-Bank im RE1.5-Zweig landet — dann klemmt das Gate auf den
Halte-Frame und feuert **jeden** Tick neu; in der ersten, fehlerhaft konfigurierten Messung
kam CORE 3 dadurch 30×+ hintereinander. Siehe offene Frage O3.)

---

## 3. Die Port-Lücke, exakt lokalisiert

Datei: `c:\workspace\git\reAi_v2\re15_port\engine\src\enemy_ai_common.c`,
Funktion `re15_player_victim_tick()`, Zweig `g_player_victim == 2` (COLLAPSE).

| Zeilen | Zweig | CORE 3? |
|---|---|---|
| 1186–1196 | `if (re15_victim_is_re2_dog())` — **RE2-Hund** | **FEHLT** (kein einziger Audio-Aufruf) |
| 1197–1225 | `else if (g_player_victim_type == 0x20)` — RE1.5-Hund | vorhanden: `re15_audio_core_se(3)` **Z. 1209**, Gate `anim_frame == 0x3a` |
| 1226–1275 | `else if (re15_ai_re2_for_type(...) && re15_re2z_owns_type(...))` — **RE2-Fress-Kollaps** | **FEHLT** (kein einziger Audio-Aufruf) |
| 1276–1295 | `else` — RE1.5-Zombie | vorhanden: `re15_audio_core_se(3)` **Z. 1293**, Gate `anim_frame == blood_fr (0x37)` |

Der Einstiegs-SE CORE 1 ist dagegen in beiden Modi da (`re15_player_victim_devour()`,
Z. 1003, im `else`-Zweig für alle Nicht-Hund-Typen) — die Messung bestätigt ihn im RE2-Modus.

Der Wiedergabe-Pfad selbst ist vollständig und getestet:
`re15_audio_core_se(3)` → `audio_pc.c:944` → `se_play_layers(s_core_edt, &s_core_vab, …, 3)`
→ CORE00.EDH-Record 3 → VAG 12592 B. **Es fehlt nur der Aufruf.**

Das steht in direktem Widerspruch zum SOUND-MANDAT (Entscheidungen aus RE2, *Sounds und
Präsentation aus RE1.5*): die RE2-Zweige wurden ohne die RE1.5-Präsentation gebaut.

---

## 4. Was die Umsetzung braucht (nur Fakten, keine geratenen Zahlen)

* **SE-Aufruf:** `re15_audio_core_se(3)` — Beleg `Se_on(0x04030001)` @0x8010a864 (Zombie),
  @0x80111df4 (Hund); Bank-Dekodierung @0x80045028/@0x8004507c; Record-Inhalt
  `CORE00.EDH[3] = 00 00 44 17`.
* **Emitter-Position im Original:** `a1 = g_entity(cur)+0x34` (@0x8010a868 / @0x80111df8) —
  also die Position des **Greifers**, nicht Leons. (Der Port hat für CORE-SEs keinen
  Positional-Pfad; `re15_audio_core_se` ist mono. Das ist eine getrennte, bereits im
  Port-Kommentar `audio_pc.c:962-965` mit Adresse benannte Lücke, **nicht** Teil dieses Fixes.)
* **Frame-Anker RE2-Zombie:** offen, siehe O1.
* **Frame-Anker RE2-Hund:** offen, siehe O2.

---

## 5. Offene Fragen (bewusst NICHT geraten)

**O1 — Auf welchem Frame gehört CORE 3 in den RE2-Fress-Kollaps?**
Die RE2-Maschine 0x8010B464 (Phasen @0x8010022C = {b4c4, b5cc, b724, b744, b774, b78c, b7ac})
spielt in P0/P1 **keinen eigenen Sprach-SE**: die einzigen `jal` in P0 sind 0x8005ba28
(RE2-Blutlache, im Port bereits als solche belegt — `enemy_ai_common.c:708/1978`) und
0x800395b8; P1 @0x8010B5CC ruft nur 0x80015cb8 (Platzierung), 0x8002959c (Advance),
0x80015fe8 (RNG) und 0x8001bf10 (RE2-Gore) und schreibt @0x8010b6d8-0x8010b71c zwei
Effekt-Deskriptoren (0x00202030 / 0x00101030) in die Spieler-Modell-Struktur.
Damit gibt es **keine RE2-Adresse für einen Todes-Sprach-Frame**. Verfügbare, zitierbare
Anker: (a) die RE1.5-Absolutkonstante `0x37` @0x8010a820-24 — im 116-Frame-RE2-Clip
erreichbar; (b) der RE2-Phasenwechsel P1→P2/P3 @0x8010b724 (`sb 3,6(s2)` = der Tick nach
Clip-Ende, an dem RE2 auch `PL+0x156 := -32768` setzt, @0x8010b738). Welcher gewählt wird,
ist eine Präsentations-Entscheidung und braucht eine bewusste Festlegung — nicht meine.

**O2 — Auf welchem Frame gehört CORE 3 in den RE2-Hund-Finisher?**
Der RE2-Hund hat laut Port-Census **überhaupt keine Kollaps-Maschine** (cmd-6-Hook 0x80104ACC,
P2 @0x80104b04 → 0x80104b7c = nichts); Leon friert in der Endpose ein, der Tod fiel schon am
Biss (FUN_800401d4 r=2). Es gibt also weder einen RE2-Kollaps-Clip noch ein RE2-Frame-Gate.
Zitierbare Anker: (a) RE1.5 `0x3a` @0x80111db4-b8 auf dem RE2-Clip 0 (145 Frames laut
Port-Kommentar Z. 1148-1149); (b) der Tick, an dem der Port heute `hp=-1`/`state=7` setzt
(`at_end_prev`, Z. 1193-1196). Ebenfalls eine bewusste Festlegung.

**O3 — Retrigger-Schutz.** Das Original feuert per Gleichheits-Gate auf einem monoton
wachsenden Frame-Zähler genau einmal. Der Port-Zweig für RE1.5 nutzt
`int blood_fr = 0x37; if (fc - 1 < blood_fr) blood_fr = fc - 1;` (Z. 1285). Mit echten
RE1.5-Bänken (65f) ist das harmlos — gemessen genau ein Auslöser. Sobald das Gate aber auf
den Halte-Frame geklemmt wird, feuert es jeden Tick neu (in der Fehlkonfiguration
RE2-Bank-unter-RE1.5-Flavor gemessen: 30+ Aufrufe in Folge). Der neue RE2-Aufruf sollte
deshalb an ein einmaliges Latch, nicht an ein Klemm-Gate.

**O4 — `Se_on(0x02070001)` @0x8010a804 (snd0 Record 7, Frame 0x23).**
Im Port unimplementiert; im Original in 163/240 Räumen ohnehin leer (u.a. ROOM1140/1190/10D0),
in 43 Räumen belegt (ganz STAGE2, STAGE3 3050-30B1, STAGE1 1090/10F0/1170/11A0/1260). Der
Bank-2-Pfad `re15_audio_room_se_snd0()` existiert im Port und wird vom Schiebe-FSM benutzt.
Getrennter, kleiner Fix — nicht das gemeldete Symptom.

**O5 — Identität der 4. STAGE1-Opfer-Maschine** (Tabelle @0x80100414, 4 Phasen
{0x8011c314, 0x8011c31c, 0x8011c348, 0x8011c384}, CORE-3-Stelle @0x8011c4b8). Vermutlich der
ROOM11C0-Boss (Typ 0x27), aber nicht verifiziert — für dieses Dossier nicht nötig.

---

## 6. Reproduktion der Messungen

```bash
export PATH="/c/msys64/mingw64/bin:$PATH"
SP=<scratchpad>
cd re15_port/build
gcc -std=c11 -DRE15_ASSETS_PATH=.../shared_assets/PSX \
    '-DRE15_ASSET_PSX_DIR="C:/workspace/git/reAi_v2/re15_port/shared_assets/PSX"' \
    -DRE15_PLATFORM_PC=1 -I../include -o $SP/probe_se_devour.exe $SP/probe_se_devour.c \
    engine/libre15_engine.a tests/libre15_test_support.a -lm
cd ..                                  # cwd = re15_port
$SP/probe_se_devour.exe 2 900          # RE2-Flavor  -> KEIN CORE se=3
SE_FLAVOR=1 $SP/probe_se_devour.exe 2 700   # RE1.5    -> CORE se=3 @ afr=55
$SP/probe_se_dog.exe                   # RE1.5-Hund -> CORE se=3 @ afr=0x3a
SE_FLAVOR2=2 $SP/probe_se_dog.exe      # RE2-Hund   -> keine SE-Zeile
```
Disassembly-Belege:
```bash
S=.claude/skills/re15-psx-disasm/scripts/re15_disasm.py
python $S dis   0x8010a6f8 100      # Zombie-Kollaps-Maschine
python $S dis   0x80111cf0 100      # Hund-Maschine B
python $S dis   0x80045024 60       # Se_on Bank/Record/Positional-Dekodierung
python $S table 0x80010e70 8        # Bank-Dispatch
python $S table 0x801002d4 5        # Hund-Phasentabelle
```
