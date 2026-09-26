# ROOM11D0 — "komische Bluteffekte" beim Start der Hunde-Cutscene: die RE1.5-Freigabe schreibt Sub-Index 0x0C, der Port liest ihn in der RE2-Tabelle als "Post-Latch" (= Blutspur des zubeissenden Hundes)

> Status: Mechanismus BELEGT, im Port reproduziert UND per Gegenprobe isoliert
> (`RE15_AI_FLAVOR=re15` → 0 Blutpartikel, Hunde rennen los). Kein Code geaendert (Regel 1
> dieser Runde) und nicht gebaut (Regel 2).

## 1. Was der Nutzer meldet / was ich gemessen habe

Nutzer: "Bei Room 11D0, nach dem aufheben der Beretta M93R gibt es nur komische
Bluteffekte beim starten der Cutscene wo die Hunde erscheinen...."

### 1a. Der Raum, aus den RDT-Bytes gelesen

Datei `re15_port/shared_assets/PSX/STAGE1/ROOM11D0.RDT` (130140 B).
Kopf @0x00: `00 0f 08 00 00 00 00 00` → nSprite=0, nCut=15, nOmodel=8.

| RDT-Feld | Datei-Offset | Wert |
|---|---|---|
| mainScd | 0x40 | 0x11D8 |
| subScd | 0x44 | 0x1684 |
| effect (ESP-Id-Header) | 0x4C | 0x2D04 |
| ESP-Ptr-Ende | 0x50 | 0x2FD0 |
| espTim | 0x54 | 0x125BC |

Der ESP-Id-Header @0x2D04 lautet `07 ff ff ff ff ff ff ff` → **der Raum hat GENAU EINEN
Effekt, Id 0x07** (EFF @0x2D0C, Kopfwort `0b 00 09 00` = 11 Anim-Saetze / 9 Zellen,
TIM @0x125BC). **Im gesamten SCD des Raums steht KEIN einziges `Sce_espr_on` (0x3A)** —
der Raum startet also von sich aus ueberhaupt keinen Effekt. Das Blut kann nur aus der
Gegner-KI kommen.

### 1b. Der Ablauf, aus dem SCD gelesen (Datei-Offsets in ROOM11D0.RDT)

* `main00` @0x11DA: bei `Ck bank=3 bit=152 == 0` fuenf `Sce_em_set`
  @0x012BE / 0x012D2 / 0x012E6 / 0x012FA / 0x0130E — alle mit **pc[2]=0x20 (Hund)** und
  **pc[3]=0x41**. Beispiel @0x012BE:
  `44 00 20 41 00 00 00 d1 e4 da 00 00 68 c5 00 00 00 00 00 00`.
  pc[3] ist das grid-Byte (+0x9) → **grid 0x41 = "Fenster-/Zwinger-Hund"**.
* `sub00` @0x016B4: `Aot_set 2c 09 03 31 00 00 14 d3 ba c8 08 07 08 07 ff 00 18 03 00 00`
  = AOT 9, Event-Typ 0x18, **Sub 3**, Rechteck (-11500,-14150,1800,1800).
* `sub03` @0x01892: die M93R-Aufnahme. Bei `Ck bank=9 bit=155 == 1` (Gegenstand genommen)
  → @0x018B6 `Evt_exec 04 ff 18 02` = **Sub 2**, danach @0x018BA `Set 22 03 98 01`
  (Flag 3.152 = 1 = "Hunde-Cutscene gelaufen").
* `sub02` @0x01728 = **die Hunde-Cutscene**. Ihre ersten Anweisungen:
  `2e 02 00` = `Work_set(kind 2 = Gegner, idx 0)`, dann `34 0c 42 00` =
  **`Member_set(member 0x0C, 66)`** — fuenfmal fuer die Gegner 0..4
  (@0x0172C, 0x01734, 0x0173C, 0x01744, 0x0174C). Danach `Sleep 30`, `Cut_chg 0x0d`,
  die Kaefigtueren (Obj 3..7 `Pos_set`/`Dir_set`), `Sce_bgm_control`, SCA-Umschaltung,
  `Cut_old`/`Cut_auto`. **Kein Effekt-Opcode.**

`Member_set` member 0x0C: selbst nachgelesen, **nicht** aus einem Decompilat uebernommen.
`FUN_8004116c` @0x8004116c prueft `sltiu v0,a1,0x14` und springt ueber die Tabelle
@0x80010c8c (Datei-Offset 0x148C in `info/Re1.5/PSX.EXE`). Eintrag **[12] = 0x800411F4**;
dort steht `j 0x80041230` mit Delay-Slot **`sb a2,9(a0)`** → **entity+0x09 (grid) := 66 = 0x42**.

### 1c. Reproduziert mit der VORHANDENEN Binaerdatei (nicht gebaut)

`re15_port/build/platform/pc/re15_pc.exe`, Lauf mit
`RE15_DEBUG_JUMP=11D0@120  RE15_SUBSTART=2@60#11D0  RE15_STATE_LOG=…  RE15_FX_LOG=…`.
Gemessen (Zustands-Protokoll; `g=` druckt `e->grid_id`, main.c:6576-6584):

```
F1..F5    [1 t=20 st=0 ss1=0  ss2=0 ss3=0 g=41 ...]        Spawn, INIT
F6..F60   [1 t=20 st=4 ss1=1  ss2=1 ss3=0 g=41 mo=8 af=1]  Fenster-Halten, Clip 8, eingefroren
F61       [1 t=20 st=1 ss1=12 ss2=0 ss3=0 g=00 ...]        sub02 lief -> Freigabe
F66       fx=4     (erste Effekt-Partikel)
F2649     fx=44    [1..4 t=20 st=1 ss1=12 ...]  — die Hunde bleiben fuer immer in Sub 12
```

Das Effekt-Protokoll (`RE15_FX_LOG`, 19072 Zeilen in ~2600 Bildern) enthaelt nur zwei Sorten:

```
id=0 sub=1 eidx=2 frame=9 w(-8935,-746,-17592) phys=1 ... slot=20     <- GLOBALES Blut-Blatt
id=7 sub=0 eidx=0 frame=3 w(-8935,-746,-17592) phys=1 ... slot=36     <- Raum-Effekt 7
```

Slot 20 = `RE15_TIM_SLOT_EFFECT_GLOBAL` = das byte-true extrahierte Blut-Blatt
(CORE00.ESP Effekt-Id 0). Also dauerhaft 44–47 lebende Blutpartikel und sonst nichts —
genau der Nutzer-Befund.

### 1d. GEGENPROBE mit derselben Binaerdatei: `RE15_AI_FLAVOR=re15`

Identischer Lauf, nur mit dem RE1.5-Gehirn statt dem RE2-Gehirn
(`enemy_ai_re2_zombie.c:101` `static re15_ai_flavor_t s_flavor = RE15_AI_FLAVOR_RE2;`
ist der ausgelieferte Default, Umschalter `enemy_ai_re2_zombie.c:126-129`):

```
F61        [1 t=20 st=1 ss1=12 ss2=0 ss3=0 g=00 mo=8 af=1]   dieselbe Freigabe
F62..F211  ss2=1, mo=8, af zaehlt 0..29                      Clip 8 laeuft EINMAL ab (EM020 Clip 8 = 30 F)
F212       ss2=2
F213       ss1=2  ss2=0                                      -> CHASE
F214+      mo=0, Position wandert (-9518,-15000) -> (-9518,-16232)   die Hunde rennen los
```

**`fx=0` in ALLEN 2476 Protokollzeilen; `RE15_FX_LOG` wurde nie angelegt — kein einziger
Blutpartikel.** Damit ist die Ursache isoliert: nicht die Freigabe, nicht das ESP-System,
nicht der Raum — sondern die Sub-Tabelle, in die der Port den Index 0x0C hineinliest.

## 2. Das Original — mit Adressen und Bytes

### 2a. Die Freigabe: `grid == 0x42` → Zustandswort `0x00000C01`

`info/Re1.5/PSX/BIN/STAGE1.BIN`, Code @0x80100000, selbst disassembliert
(`re15_disasm.py dis … --bin STAGE1.BIN`). Handler des Fenster-Hundes `FUN_80111658`:

```
80111688: ori v0,zero,0x1
8011168c: sb  v0,6(a0)          ; Phase 0 -> 1
80111698: ori v0,zero,0x8
8011169c: sb  v0,148(v1)        ; +0x94 = Clip 8
801116ac: sb  zero,149(v0)      ; +0x95 = 0
801116bc: sb  zero,143(v0)      ; +0x8f = 0
801116d4: jal 0x8001f314        ; anim_set
801116f0: addiu v0,v0,300       ; +0xbc/+0xbe += 300 (angehobene Kaefig-Pose)
8011170c: lbu v1,9(a0)          ; v1 = grid (+0x09)
80111710: ori v0,zero,0x42      ; Vergleichswert 0x42
80111714: bne v1,v0,0x80111754  ; != 0x42 -> weiter warten
80111718: ori v0,zero,0xc01     ; Delay-Slot: v0 = 0x0C01
8011171c: sb  zero,9(a0)        ; grid VERBRAUCHT -> 0
8011172c: sw  v0,4(v1)          ; entity+0x4 = 0x00000C01
                                ;   +0x4 (state)       = 0x01
                                ;   +0x5 (sub_state_1) = 0x0C
                                ;   +0x6/+0x7          = 0
80111744: addiu v0,v0,-300      ; +0xbc/+0xbe -= 300
```

Das deckt sich Bild fuer Bild mit der Messung (F61: `st=1 ss1=12 g=00`).

### 2b. Was Sub 0x0C im ORIGINAL ist

Die Hunde-Tabellen, aus `STAGE1.BIN` selbst gelesen:

* Zustands-Tabelle @0x80120F74:
  `[0]=0x8010D93C [1]=0x8010DBCC [2]=0x801108F0 [3]=0x80110DC0 [4..6]=0x80111350
   [7]=0x80111774 [8]=0x8010DDB8 [9]=0x8010DF94 [10]=0x8010E0C4 [11]=0x8010E568`
* Zustand 1 (ACTIVE) = Doppel-Dispatch auf +0x5:
  * DECIDE-Tabelle @0x80120F94: **[12] = 0x801101DC**
  * ACT-Tabelle    @0x80120FD4: **[12] = 0x801101E4**

`0x801101DC` disassembliert: `jr ra / nop` → **die DECIDE-Haelfte von Sub 12 ist leer.**

`0x801101E4` disassembliert (Phasen-Automat auf +0x6):

```
801101f4: lbu v1,6(a0)                ; Phase
80110230: sb  v0,6(a0)     (v0=1)     ; Phase 0 -> 1
80110240: sb  v0,148(v1)   (v0=8)     ; +0x94 = Clip 8
80110250: sb  zero,149(v0)            ; +0x95 = 0
80110260: sb  zero,150(v0)            ; +0x96 = 0
80110270: sb  v0,143(v1)   (v0=7)     ; +0x8f = 7
80110288: jal 0x8001f314              ; anim_set  (der EINZIGE jal der Funktion)
801102a4/a8/ac: lbu +0x6 / addu v0 / sb  ; Phase += Rueckgabe
801102b0: sb  v0,5(a0)     (v0=2)     ; Phase 2 -> +0x5 = 2  (= CHASE)
801102c0: sb  zero,6(v0)              ;            +0x6 = 0
```

**Sub 0x0C im Original = Clip 8 einmal abspielen, dann Sub 2 (CHASE).** Kein einziger
Effekt-Aufruf (kein `0x80105070`, kein `0x80019700`). Der Hund steht auf und rennt los.

### 2c. Was Sub 12 in RE2 ist — und warum dort Blut fliesst

`info/re2leon/COMMON/BIN/EMD0G_MOD0.BIN` (RE2-Cerberus-Overlay, Code @0x80100000),
Sub-Tabelle @0x80105464 selbst gelesen:

```
[0]=0x80100784 [1]=0x80100824 [2]=0x80100B20 [3]=0x801011EC [4]=0x80101830 [5]=0x80101B14
[6]=0x80101BDC [7]=0x80101CDC [8]=0x8010224C [9]=0x8010237C [10]=0x80102404 [11]=0x80102490
[12]=0x80102608  [13]=0x801028AC [14]=0x80102C28 [15]=0x80102E58 [16]=0x801030FC
```

`0x80102608` ist der **Post-Latch** (der Hund hatte Leon gerade im Maul). Sein Schwanz,
selbst disassembliert:

```
801027dc: lw  v0,376(s0) / 801027e4: lw s1,0(v0)   ; Anim-Frame-Wort (+0x178)
801027e8: lui v0,0x3     / 801027ec: and v0,s1,v0  ; Maske 0x30000
801027f0: beq v0,zero,0x8010284c
801027fc: sb  s2,543(s0)   (s2=1)                  ; +0x21F Effekt-Budget = 1
80102800: addiu a1,zero,4 / 80102808: a2 = 0
80102804: jal 0x80105070                           ; FX(Part 4, fx 0) = BLUTTROPFEN
8010280c: lui v0,0x2     / 80102810: and v0,s1,v0  ; Maske 0x20000
8010281c: jal 0x80015fe8 / 80102824: andi v0,v0,3  ; 1/4-Wuerfel
80102834: sb  s2,543(s0)
8010283c: addiu a1,zero,4 / 80102840: andi v0,v0,1 / 80102848: addiu a2,v0,1
80102844: jal 0x80105070                           ; FX(Part 4, fx 1|2) = BLUTSPRITZER
```

Die Effekt-Tabelle @0x801056AC (60 B) fuehrt fx 0 = Art 0 / Unterart 1 (Bluttropfen) und
fx 1/2/3 = Art 9 / Unterarten 0..2 (Spritzer) — genau die zwei Sorten, die das
Effekt-Protokoll zeigt (`id=0 sub=1` aus dem globalen Blut-Blatt; `id=7`, weil der Port
Art 9 → Id 7 dekodiert, `enemy_ai_re2_dog.c:369`).

## 3. Was der Port tut — mit datei.c:zeile

* `re15_port/engine/src/enemy_ai_common.c:7480-7484` — die RE1.5-Freigabe ist byte-true
  portiert:

  ```c
  if (e->grid_id == 0x42) {                /* SCD release mark 'B' @0x8011170c-14 */
      e->grid_id = 0;                      /* consumed @0x80111718 */
      e->state = 1; e->sub_state_1 = 0x0c; e->sub_state_2 = 0; e->sub_state_3 = 0;  /* word 0xc01 @0x8011172c */
  ```

* `re15_port/engine/src/enemy_ai_common.c:7581-7584` — die Flavor-Weiche:

  ```c
  if (e->state >= 4 && e->state <= 6) { re15_dog_state456(e, pl); return; }
  re15_re2dog_tick(slot);
  ```

  Nach der Freigabe ist `state == 1`, der Hund faellt also in den **RE2**-Zweig.
* `re15_port/engine/src/enemy_ai_re2_dog.c:2285-2302` — RE2-Sub-Dispatch,
  `case 12: re2d_sub12_postlatch(e, pl);   /* 0x80102608 */`.
* `re15_port/engine/src/enemy_ai_re2_dog.c:1341-1351` — der Blut-Schwanz
  (`re2d_fx(e, 4, 0)` @0x80102800-08 und `re2d_fx(e, 4, 1|2)` @0x80102838-48);
  `re15_port/engine/src/enemy_ai_re2_dog.c:332-381` = `re2d_fx` → `re15_esp_fx_spawn_rows`.
* Der RE1.5-Zweig HAT den richtigen Handler bereits —
  `re15_port/engine/src/enemy_ai_common.c:8018-8022`:

  ```c
  case 0x0c:   /* RELEASE 0x801101e4 (clip 0x08): the player broke free -> recover to CHASE */
      if (e->sub_state_2 == 0) { re15_dog_clip(e, 8); e->sub_state_2 = 1; }
      else if (e->sub_state_2 == 1) { if (re15_dog_anim(e)) e->sub_state_2 = 2; }
      else re15_dog_sub(e, 2);              /* exit -> CHASE @0x801102b0 */
  ```

  Er wird im ausgelieferten Stand nie erreicht, weil Typ 0x20 seit 2026-08-22 auf dem
  RE2-Flavor laeuft (`re15_ai_re2_for_type(0x20)`, Weiche `enemy_ai_common.c:7562`).

## 4. Der Unterschied, in einem Satz

Die RE1.5-Kaefig-Freigabe schreibt @0x8011172c den **RE1.5**-Sub-Index 0x0C
("Clip 8 aufstehen, dann jagen", @0x801101E4 — null Effekte); der Port gibt den Hund danach
an die **RE2**-Sub-Tabelle @0x80105464, deren Index 12 der Post-Latch @0x80102608 ist, und
der spritzt pro Bild Blut (@0x80102804 / @0x80102844) — also lauter Blut statt fuenf
losstuermender Hunde.

## 5. Umsetzungsplan (konkrete Dateien/Zeilen, jede Konstante mit @0x)

Grundsatz: dieselbe Regel, die der Port fuer die Zustaende 4/5/6 schon anwendet
(`enemy_ai_common.c:7576-7583`: "Skript-Zustaende sind RE1.5-Raum-Praesentation und laufen
auch im RE2-Flavor ueber die RE1.5-Maschine"), muss auch fuer den Sub-Index gelten, den
diese Maschine als Ausgang schreibt. 0x201 (Pounce-Land-Ausgang @0x8011162c) passt
zufaellig in beide Tabellen (RE1.5-Sub 2 = CHASE, RE2-Sub 2 = `re2d_sub2_run` @0x80100B20)
— **0x0C01 nicht.**

1. `re15_port/engine/src/enemy_ai_common.c`: den RE1.5-Sub-0x0C-Rumpf (heute inline in
   Zeile 8018-8022) in eine eigene Funktion `re15_dog_release0c(re15_actor_t *e)` ziehen,
   Schritt fuer Schritt wie @0x801101E4:
   * Phase 0 (`sub_state_2 == 0`): `motion = 8` (`sb v0,148` @0x80110240),
     `anim_frame = 0` (`sb zero,149` @0x80110250), `anim_frac = 0` (`sb zero,150` @0x80110260),
     `+0x8f = 7` (`sb v0,143` @0x80110270), `sub_state_2 = 1` (`sb v0,6` @0x80110230).
   * Phase 1: `anim_set` (`jal 0x8001f314` @0x80110288); bei Clip-Ende `sub_state_2 = 2`
     (`+0x6 += Rueckgabe` @0x801102A4-AC).
   * Phase 2: `sub_state_1 = 2` (`sb v0,5` @0x801102B0), `sub_state_2 = 0`
     (`sb zero,6` @0x801102C0).
   Zeile 8018-8022 ruft danach nur noch diese Funktion — RE1.5-Flavor bleibt unveraendert.
2. `re15_port/engine/src/enemy_ai_common.c:7583`, direkt hinter der 4/5/6-Weiche und VOR
   `re15_re2dog_tick(slot)`:

   ```c
   /* Der RE1.5-Fenster-Ausgang @0x8011172c schreibt 0x0C01. Sub 0x0C existiert nur in der
    * RE1.5-ACT-Tabelle @0x80120FD4[12] = 0x801101E4 (Clip 8 -> Sub 2). In RE2 ist
    * @0x80105464[12] = 0x80102608 der Post-Latch mit dem Blut-Schwanz
    * @0x80102804 / @0x80102844 — daher die Blutflut in ROOM11D0. */
   if (e->state == 1 && e->sub_state_1 == 0x0c) { re15_dog_release0c(e); return; }
   ```

   Der Ausgang `sub_state_1 = 2` landet danach byte-true im RE2-Sub 2
   (`re2d_sub2_run`, @0x80100B20) — dieselbe Landung wie der schon dokumentierte
   Pounce-Land-Ausgang 0x201 @0x8011162c.
3. Pin-Test unter `re15_port/tests/unit/`: Hund Typ 0x20 mit grid 0x41 spawnen,
   `re15_dog_state456` in state 4 / sub 1 bis `sub_state_2 == 1` ticken, `grid_id = 0x42`
   setzen, einen Tick → erwartet `state == 1 && sub_state_1 == 0x0c`; danach N Ticks →
   erwartet `motion == 8`, **Effekt-Zaehler unveraendert 0**, und am Clip-Ende
   `sub_state_1 == 2`. Dieser Test faellt im heutigen Stand durch.
4. Regressions-Check fuer die Nachbarn: `re2d_sub12_postlatch` bleibt unangetastet und
   erreichbar ueber den echten RE2-Latch-Weg (`re2d_sub7_latch` → sub 12), es geht nur der
   FALSCHE Eintritt aus der RE1.5-Kaefig-Freigabe weg.

### 5a. Umfang — Roh-Byte-Zensus ueber alle 206 ausgelieferten RDTs

Gesucht: `44 [00-1f] 20 41` (`Sce_em_set`, Typ 0x20 = Hund, grid 0x41) und
`34 0c 42 00` (`Member_set(0x0C, 0x42)` = die Freigabe 'B').

| Datei | Hunde mit grid 0x41 | Freigaben `34 0c 42 00` |
|---|---|---|
| ROOM11D0.RDT / ROOM11D1.RDT | 5 / 5 | 5 / 5 |
| ROOM3060.RDT / ROOM3061.RDT | 3 / 1 | 3 / 1 |

**Betroffen sind also vier Raeume: der Zwinger (11D0/11D1) und STAGE3 3060/3061** — zusammen
14 Hunde. Dass STAGE3 dieselbe Freigabe fuehrt, ist nachgelesen: `STAGE3.BIN` @0x801101EC-0x8011020C
ist Instruktion fuer Instruktion dieselbe Sequenz wie `STAGE1.BIN` @0x8011170C-0x8011172C
(`lbu v1,9(a0)` / `ori v0,zero,0x42` / `bne` / `ori v0,zero,0xc01` / `sb zero,9(a0)` /
`sw v0,4(v1)`). Ein Fix im Port deckt beide ab. Der Pounce-Zweig (grid 0x40 + `34 0c 43 00`) liegt in ROOM1190/1191 (je 3) und ist
NICHT betroffen: sein Ausgang @0x8011162c schreibt 0x201, und Sub 2 existiert in beiden
Tabellen als "rennen" (RE1.5 CHASE / RE2 `re2d_sub2_run` @0x80100B20).

`34 0c 42 00` steht ausserdem in ROOM2060/2061/20A0/20A1 (Gegner-Typ **0x25**, ebenfalls mit
grid 0x41) sowie ROOM4001/5040/5041/5070/5071. Das sind ANDERE Gegnertypen mit eigenen
grid-gegateten Handlern — ob dort dieselbe Fehlerklasse steckt, ist NICHT geprueft
(siehe Abschnitt 6).

## 6. Offen / nicht belegt

* **Der Mechanismus ist statisch vollstaendig belegt** (Freigabe-Bytes, beide Sub-Tabellen,
  beide Handler) und im Port reproduziert — aber ich habe die Cutscene **nicht** im Original
  (DuckStation) angesehen. Was genau der Spieler dort sieht (Kaefigtueren + fuenf Hunde,
  Kamera-Cut 0x0d) ist aus dem Skript erschlossen, nicht abfotografiert.
* **Nicht dekodiert: was der Raum-Effekt Id 0x07 darstellt** (EFF @0x2D0C: count_a=11 /
  count_b=9, TIM @0x125BC). Fuer diesen Befund nicht noetig — er wird nur als Ziel der
  `re2d_fx`-Art 9 gezogen —, bleibt aber offen.
* **Gegner-Typ 0x25 in ROOM2060/2061/20A0/20A1** faehrt dasselbe Muster (grid 0x41 +
  `Member_set(0x0C, 0x42)`). Ob sein Freigabe-Ausgang ebenfalls einen Sub-Index schreibt,
  den der Port in einer fremden Tabelle nachschlaegt, habe ich NICHT geprueft. Naechster Weg:
  den Typ-0x25-Root in STAGE2.BIN aufschlagen (Dispatch-Tabelle EXE @0x80072bac[0x25]) und
  seinen grid-0x42-Zweig disassemblieren. Gleiches gilt fuer die `34 0c 42 00`-Treffer in
  ROOM4001/5040/5041/5070/5071.
* **Nicht geprueft**: ob der RE1.5-Root-Tail von `FUN_8010DBCC` (@0x8010dd00-ddb0,
  Band-Stempel) nach dem Sub-0x0C-Tick noch laufen muesste. Der Port laesst ihn fuer die
  ebenfalls RE1.5-geroutete 4/5/6-Maschine heute auch aus; ob das fuer Sub 0x0C
  Unterschiede macht, ist offen.
