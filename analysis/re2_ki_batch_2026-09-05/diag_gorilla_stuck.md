# Diagnose Symptom 5 — „Gorillas beim Parking lot haengen zwischen den Autos und koennen kaum angreifen"

ROOM11C0 (Parkgarage B1), Typ 0x27 (EM027, „Maggot"/Gorilla-Boss), laeuft in BEIDEN AI-Modi auf dem
RE1.5-Brain `re15_maggot_ai_tick` (re15_port/engine/src/enemy_ai_common.c:8259 ff., Root 0x80116db8).
Alle @0x-Zitate unten stammen aus EIGENEM Disasm dieser Session
(`python .claude/skills/re15-psx-disasm/scripts/re15_disasm.py dis <addr> <n> --bin STAGE1.BIN`
bzw. ohne `--bin` = RE1.5 PSX.EXE); SCA-Daten selbst aus
`re15_port/shared_assets/PSX/STAGE1/ROOM11C0.RDT` geparst (Header-Ptr @Datei 0x20 → SCA @0x1410,
59 Zellen in 5 Quadranten-Partitionen 11/13/19/16/0).

---

## 0. Kurzbefund

Der Original-Gorilla umgeht die Autos NICHT durch Steuern/Wandtasten — er **springt per
„ZONE-LEAP" ueber sie hinweg**. Das ist ein Dreiteiler:

1. **Entscheidung** in A[4] (SELECTOR-Decide 0x80117e40): eine SCA-Attribut-Zonen-Abfrage
   `FUN_8003b93c` findet Marker-Zellen (`u1 == 0x10` bzw. `0x20`, `u0 = 0` = nicht solide) und
   schreibt die Ueberquerungs-Richtung in `entity+0x90`; passt die Blickrichtung (±45°), wird
   `+0x5=7` (LEAP) mit Variante `+0x7=1` bzw. `3` committet.
2. **Launch** in B[7] (0x80118908): Yaw-SNAP auf die Zonen-Richtung und — der Kernpunkt —
   **`entity+0x82 = 1`** (Kollisions-BAND auf 1) @0x80118af0: waehrend des Flugs klemmen die
   Band-0-Autozellen NICHT mehr, nur die Band-1-Zellen (Waende/hohe Bloecke).
3. **Landung**: `+0x82 = 0` zurueck @0x80118ca4.

ROOM11C0 enthaelt **vier solcher Leap-Pad-Zonen**, exakt in den Auto-Gassen platziert; die
Ost-Gasse ist fuer den 1600er-Radius **geometrisch unpassierbar** (Spalt 2711 < 2·1600), und der
zweite Kampf-Spawn liegt **in** einem Pad. Der Port fuehrt die Zonen-Leap-ENTSCHEIDUNG als
dokumentiertes OPEN (enemy_ai_common.c:8401-8410) — sie feuert **nie**. Folge: der Port-Gorilla
schleift dauerhaft an den Autozellen (`+0x1d6`-Analog `sca_wall_hit` = 1 in jedem Frame), und
genau dieses Kontakt-Flag sperrt zusaetzlich byte-true den REAR-UP (@0x80117b18) und den
Fern-LEAP (@0x80118054) → „koennen kaum angreifen" ist die direkte Folge des Haengens, nicht
eines falschen Angriffs-Gates (die Gates messen sich byte-true, §5).

Zweitbefund: selbst der VORHANDENE Leap-Code des Ports klemmt am falschen Band — der Port leitet
das Gegner-Band pro Frame aus Y ab (`re15_collision_band_from_y`, re15_collision.c:797/186)
statt aus dem `+0x82`-Zustands-Byte; das vom Port bereits geschriebene `e->floor=1/0`
(enemy_ai_common.c:8529/8553) hat **keinen Konsumenten** (dieselbe Fehlerklasse wie Memory
„Ein Original-Byte, zwei Port-Felder").

---

## 1. Original: Wie navigiert der Gorilla um die Autos? (AUFTRAG 1+4)

### 1.1 Es gibt KEIN Hund-artiges Reroute und KEINEN Stuck-Zaehler-Konsumenten

Brain-Tail STATE[1] (eigenes Disasm @0x801173f0-80):

```
801173f8  lh v0,476(a0)        ; +0x1dc (Attack-Lockout)
80117400  beq v0,zero,+2       ; nur wenn !=0:
80117408  addiu v0,v1,-1
8011740c  sh v0,476(a0)        ;   +0x1dc--
8011741c  lbu v0,481(v1)       ; +0x1e1 (Pin-Cooldown)
80117434  addiu v0,v0,-1       ;   +0x1e1--
80117448  lhu v0,470(v1)       ; +0x1d6 (SCA-Wand-Kontakt, Rueckgabe FUN_8003b0a4)
80117450  beq v0,zero,8011746c
80117458  lhu v0,478(v1)       ; +0x1de
80117460  addiu v0,v0,1        ;   Kontakt: +0x1de++
80117468  sh v0,478(v1)
8011746c  sh zero,478(v1)      ;   kein Kontakt: +0x1de = 0
```

`+0x1de` ist der Steckenbleib-Zaehler — er wird gezaehlt, aber im gesamten Maggot-Baum
(0x80116db8..0x8011be54) **von niemandem gelesen** (nur INIT-Clear @0x8011706c + dieser Block;
Befund deckungsgleich mit analysis/gorilla_11c0/verhalten.md §D6 Voll-Scan). `+0x1dc` ist beim
Maggot ein **Angriffs-Lockout** (No-Decision-Gate @0x80117490-98, Bite-Gate @0x80117a88-90,
Heavy-Gate @0x80117eb0), NICHT der Hund-Blocked-Zaehler. Der Port bildet beide Timer korrekt ab
(enemy_ai_common.c:8730-8731, +0x1de bewusst weggelassen: Kommentar 8727-8729). **AUFTRAG 4
Antwort: Nein — es gibt keinen Zaehler-getriebenen Kurswechsel im Original; der Kurswechsel bei
Blockade ist der Zonen-Leap (§1.2). Der Port fuehrt dazu KEINEN No-Op — ihm fehlt die
Entscheidung komplett.**

A[3] CHASE-Decide 0x80117a3c komplett disassembliert (150 Instr.): emittiert ausschliesslich
+0x5=5 BITE (@0x80117a94-98, Gates §5), das REAR-UP-Upgrade +0x5=15 (@0x80117b34-3c), die
Blind→Sighted-Wiedereintritte, und +0x5=4 SELECTOR bei `dist>=0x1771 && LOS` (@0x80117c34-54).
**Kein** +0x1d6/+0x90-Lese im CHASE-Decide, kein Ausweichen. Das deckt sich 1:1 mit dem Port
(enemy_ai_common.c:8347-8392). D.h. auch das Original „schiebt" im CHASE stumpf gegen das
Hindernis — bis der SELECTOR-Pfad den Leap zuendet.

### 1.2 Der Zonen-Leap: A[4]-Decide 0x80117e40 (eigenes Disasm, 180 Instr.)

Reihenfolge im SELECTOR-Decide:

1. Abort: `player.hit_react!=0 && (+0x1d0&1)` → +0x5=3 CHASE (@0x80117e50-74).
2. HEAVY-Commit: `player.hit_react==0 && a804(0xfa0,0xc0) && +0x1dc==0` → +0x5=6
   (@0x80117e88-ec4). *(byte-gleich im Port :8398-8400)*
3. **PATH A — ZONEN-LEAP (@0x80117ec8-0x80118024), im Port FEHLEND:**
```
80117ed4  lbu v0,482(v1)       ; +0x1e2 (INIT=4, immer !=0)
80117edc  beq v0,zero,80118028 ; ==0 -> Path B
80117ee0  ori a3,zero,0x10     ; ATTR 0x10
80117ee4  lbu a2,130(v1)       ; a2 = +0x82 (BAND-Byte!)
80117ee8  lw v0,120(v1)        ; +0x78 Box-Ptr
80117ef4  lhu a1,6(v0)         ; a1 = box[6] = 1600
80117efc  jal 0x8003b93c       ; Zonen-Abfrage
80117f00  addiu a1,a1,100      ;   (Radius+100 = 1700)
80117f04  beq v0,zero,80117f78 ; kein 0x10-Pad -> 0x20-Zweig
80117f18  lbu a1,144(a0)       ; +0x90 (von b93c FRISCH geschrieben, s. 1.3)
80117f1c  lh v1,106(a0)        ; +0x6a rot_y
80117f20  andi v0,a1,0xf0
80117f24  sll v0,v0,4          ; Zonen-Richtung als 12-bit-Yaw
80117f28  subu v0,v0,v1
80117f2c  addiu v0,v0,512
80117f30  andi v0,v0,0xfff
80117f34  slti v0,v0,1024      ; |zonenYaw - rot_y| < 512 (±45°)?
80117f38  beq v0,zero,80117f78
80117f40  sb a1,159(a0)        ; +0x9f = +0x90 (Richtung latchen)
80117f50  sb v0,5(v1)          ; +0x5 = 7 (LEAP)
80117f60  sb zero,6(v0)        ; +0x6 = 0
80117f74  sb v0,7(v1)          ; +0x7 = 1  (Zonen-Leap Variante 1)
```
   Danach identisch mit `a3=0x20` (@0x80117f98 jal 0x8003b93c): Treffer + **LOS-Latch
   `+0x1d0&1` GESETZT → Sprung zu 0x80118048** (= direkt in die Path-B-Gates, LOS-Check
   uebersprungen); LOS-Latch FREI → gleiches Yaw-Fenster → `+0x5=7, +0x7=3`
   (Blind-Zonen-Leap, @0x80117fc8-8011802
4). 
4. PATH B — Fern-Leap (@0x80118028-100): `LOS(+0x1d0&1)` @0x8011803c, **`+0x1d6==0`**
   @0x80118054-5c, `+0x1d4>=0x1771` @0x8011806c, `a9cc(±0x20)==0` @0x80118084-8c,
   `aca58==0x701 || rng&1` @0x80118090-b4, `a780==0` @0x801180bc-c4 → `+0x5=7, +0x7=0`.
   *(byte-gleich im Port :8411-8419)*

### 1.3 FUN_8003b93c — die SCA-Attribut-Zonen-Abfrage (EXE, eigenes Disasm, 110 Instr.)

Args: `a0 = entity+0x34` (Pos-Vektor), `a1 = Radius+100`, `a2 = +0x82` (Band-Byte),
`a3 = Attr` (0x10/0x20). Ablauf:

```
8003b9a8  jal 0x8003b068       ; Quadranten-Wahl (dieselbe Partition wie der Resolver)
8003b9cc  lw s1,0(v0)          ; Partition [start..
8003b9d0  lw v1,4(v0)          ;             ..end)
; Schleife, Zellen-Stride 12, s0 = Zelle+10:
8003b9f8  lhu v1,0(s0)         ; halfword = u1 | (floor<<8)
8003ba00  sll v0,v1,16
8003ba04  sra v0,v0,28         ; = floor>>4 = BAND der Zelle
8003ba08  bne a2,v0,skip       ; Band muss dem +0x82 des Gorillas gleichen
8003ba0c  andi v1,v1,0xff      ; = u1
8003ba14  bne v1,v0,skip       ; u1 muss EXAKT == Attr sein (v0 = a3)
8003ba1c  lh a0,-6(s0)         ; Zelle.x
8003ba28  subu a0,a0,a1        ; x - (r+100)
8003ba2c  subu v1,v1,a0        ; pos.x - ...
8003ba30  addu v0,v0,a3        ; width + 2*(r+100)   (a3 hier = a1<<1)
8003ba34  sltu v1,v1,v0        ; Broadphase X       (Z analog @0x8003ba40-58)
8003ba68  jal 0x8001bf04       ; RICHTUNGS-Code Aktor vs. Zelle
8003ba70  sra v0,v0,4
8003ba74  addiu v0,v0,8
8003ba84  andi v1,v1,0x3       ; + (u1 & 3)
8003ba8c  sb v0,144(a0)        ; entity+0x90 = (Code>>4)+8+(u1&3)   << SCHREIBT +0x90 SELBST
8003ba98  ori v0,zero,0x1
8003ba9c  j exit               ; return 1 beim ERSTEN Treffer
8003baa0  sw s1,436(v1)        ; +0x1b4 = &Zelle
```

D.h. das Yaw-Fenster in A[4] liest NICHT den letzten Wand-Kontakt, sondern die von b93c **in
derselben Instruktionsfolge geschriebene Richtung des Aktors relativ zum PAD** (FUN_8001bf04:
westlich der Zelle → 0x000 = Ost-Heading, oestlich → 0x800 = West, suedlich → 0x400, noerdlich →
0xc00, innerhalb → 0xf00; Port-Aequivalent `coll_contact_dir`, re15_collision.c:678-694). Die
Bedingung „Blick ±45° auf die Ueberquerungs-Richtung" heisst praktisch: **der Gorilla schiebt
gerade frontal gegen die Auto-Reihe, hinter der der Spieler steht → er springt drueber.**

### 1.4 B[7]-Launch: Band-Wechsel + Yaw-Snap (eigenes Disasm @0x80118a2c-b10)

```
80118a44  andi v0,v0,0x1f ; rng
80118a50  addiu v0,v0,200      ; +0x8c = rng&0x1f + 200
80118a64  lbu v1,482(a0)       ; +0x1e2 (=4)
80118a6c/70/78  *10, add       ; +0x8c += +0x1e2*10  (=> 240..271)
80118a90  lbu v1,7(a0)         ; +0x7
80118a98  bne v1,0x3,+2
80118aa0  sh v0,140(a0)        ; Variante 3: +0x8c = 0x32a (810)
80118ab0  sb a0,480(v0)        ; +0x1e0 = 1 (airborne)
80118ac0  lbu v0,7(v1)
80118ac8  beq v0,zero,80118afc ; nur Zonen-Varianten (+0x7 != 0):
80118ad0  lbu v0,159(v1)       ;   +0x9f
80118ad8  andi v0,v0,0xf0
80118adc  sll v0,v0,4
80118ae0  sh v0,106(v1)        ;   rot_y = ZONEN-RICHTUNG (Yaw-SNAP)
80118af0  sb a0,130(v0)        ;   +0x82 = 1   << KOLLISIONS-BAND AUF 1
80118b08  ori v0,v0,0x1
80118b10  sb v0,147(v1)        ; +0x93 |= 1
```

Landung (@0x80118c3c-cc4): `c1a4`-Rueckgabe != 0 → `+0x6=3`, `+0x8c=0x64`, Se(2),
`+0x1e0=0` @0x80118c94, **`+0x82=0`** @0x80118ca4, `+0x7=0` @0x80118cb4, `+0x93=0` @0x80118cc4.

**Wirkung des `+0x82=1`:** Der Root-Tail klemmt JEDEN Frame via FUN_8003b0a4
(@0x80116e64-84, `+0x1d6 = Rueckgabe`), und der Resolver gateet Zellen strikt auf
`band == (cell.floor>>4)` (Port-Aequivalent re15_collision.c:726). Mit Band 1 klemmen die
Band-0-Autozellen nicht mehr — nur die Band-1-„hohen" Zellen (§2, Waende + Zentralblock-Kern).
Der Zonen-Leap fliegt also ueber Autos, bleibt aber im Raum eingesperrt. Beim Fern-/
Retaliation-Leap (+0x7=0) wird +0x82 NICHT angefasst → der bleibt Band 0 und wird waehrend des
ganzen Flugs von den Autozellen horizontal geklemmt (kann also NICHT ueber Autos springen —
konsistent: Kreuzen geht im Original NUR ueber die Pads).

---

## 2. ROOM11C0-Geometrie: die Autos, die Pads, der 1600er-Radius (AUFTRAG 2)

SCA selbst geparst (59 Zellen; Duplikate = Quadranten-Partitionen). Auszug (Band 0, fuer den
Gorilla solide = `u0 & 4`):

| Zellen | Typ | u0 | Spanne | Rolle |
|---|---|---|---|---|
| 2/14/26/46 | 1 | ff | x[-11900..-1940] z[-6810..10849] | Zentralblock West |
| 7/18/35/54 | 1 | ff | x[-3862..4800] z[-7200..8400] | Zentralblock Ost |
| 28 | 4 | ff | x[7511..11831] z[-11500..-3650] | **Auto-Reihe Ost (diag)** |
| 29 | 6 | ff | x[7511..14613] z[-3600..410] | **Auto-Reihe Ost (diag)** |
| 37 | 1 | ff | x[10000..16900] z[-20800..-2400] | Ost-Block |
| 41/58, 42, 30/48 | 2/3/1 | ff/ff/**04** | z[-11200..-3020] Gassen-Sued | 30/48 = **u0=0x04, NUR fuer Gegner solide** |
| 13/45, 50, 33/52, 27, 3, 10, 20, 21, 23 | – | ff | – | weitere Bloecke/Diagonalen |

**Leap-Pads (u0=0x00 → fuer NIEMANDEN solide, reine Marker):**

| Zellen | u1 | Spanne | Lage |
|---|---|---|---|
| 36 | **0x10** | x[4900..9500] z[-6080..-2180] | **exakt im Ost-Spalt** Block7↔Auto-Reihe 28/29 |
| 47 | **0x10** | x[-17397..-12096] z[-6590..-1690] | West-Gasse Block13↔Zentralblock |
| 4/31 | **0x20** | x[7500..10700] z[-200..8201] | Ost-Tasche (zwischen 29, 3, Ostwand) |
| 19/55 | **0x20** | x[-11900..-9300] z[200..9900] | Westkante des Zentralblocks |

**Band-1-Zellen (floor 0x13 → Band 1, klemmen NUR den fliegenden Zonen-Leaper):**
16/51 (West-Streifen), 8/22 (Sued), 38/56 (Nord), 9/39 (Ost-Streifen x[9300..19600]),
40/57 (Zentralblock-Kern x[-11800..4800] z[-8200..-700]).

**Messung der Passierbarkeit (2·r = 3200 noetig):**
- Ost-Spalt Block 7 (x_max 4800) → Auto-Reihe 28/29 (x_min 7511) = **2711 < 3200 →
  UNPASSIERBAR** fuer den Gorilla am Boden. Pad 36 (attr 0x10) deckt exakt diesen Spalt.
- West-Gasse Block 13 (x_max −17790) → Block 2 (x_min −11900) = 5890 (passierbar, Pad 47 dort
  trotzdem als Abkuerzung).
- Nord-Gasse Block 33 (z_max −17095) → 41 (z_min −11200) = 5895 (passierbar).
- **Kampf-Spawn #2 (9434, 2189) liegt IN Pad 4/31** (x[7500..10700] z[-200..8201]), in der
  Ost-Tasche, deren einziger Boden-Ausgang der 2711er-Spalt bzw. Diagonale 3 ist — dieser
  Gorilla ist im Port faktisch eingesperrt, solange der Zonen-Leap fehlt. (Spawns/Layout:
  verhalten.md §3, im Port byte-gleich gemessen.)

**Klemm-Parameter-Vergleich (Port ↔ Original) — hier ist der Port korrekt:**
- Radius: Original `lhu a1,6(+0x78)` = 1600 (@0x80116e64-70, Box @0x80121350
  {0,-1440,0,1600,1440,1600}); Port `e->hit_radius_min` = 1600 (re15_damage.c:2233,
  Aufruf enemy_ai_common.c:12942-43 via re15_enemy_sca_clamp :13386).
- Maske: Original hart `ori a2,zero,0x4` (@0x80116e68); Port `4u` (:13386). Die
  Gegner-Spezialzelle 30/48 (u0=0x04) klemmt in beiden.
- Frequenz: beide UNBEDINGT jeden Frame nach dem State-Dispatch (Root @0x80116e64-84; Port
  :13383-13386, kein Bewegungs-Gate).
- **DIVERGENZ Band-Quelle:** Original `+0x82` (Zustands-Byte; 0 am Boden, **1 im Zonen-Leap**);
  Port `re15_collision_band_from_y(e->y)` = `-(y/0x708)` (re15_collision.c:797, 186). Damit
  (a) ist das Port-`e->floor` (von B[7] :8529/8553 geschrieben) toter Code, (b) wechselt JEDER
  Port-Leap (auch +0x7=0, z.B. der Retaliation-Leap nach jedem Flinch, :8755-8760) mit der
  Flughoehe das Band (Apex ~4680 → Band 2 = gar keine Zellen → voellig ungeklemmt), waehrend das
  Original bei +0x7=0 die ganze Flugbahn an den Band-0-Autozellen klemmt. Der Port-Gorilla kann
  so bei jedem Beschuss-Leap ueber/auf Autos und in Taschen geraten, aus denen sein Radius am
  Boden nicht mehr herauskommt — ein zweiter, eigenstaendiger „haengt zwischen den Autos"-Weg.

---

## 3. Port-Stand des Zonen-Leaps (AUFTRAG 1, Port-Zitate)

- Die ENTSCHEIDUNG fehlt, explizit als OPEN dokumentiert: enemy_ai_common.c:8401-8410
  („the port has neither the b93c SCA-attr zone query nor the FUN_8003b0a4 +0x90
  escape-heading writer — the B[7] +0x7-variant launch code below is in place for when they
  land"). Anm.: der zweite Halbsatz ist inzwischen veraltet — `+0x90` WIRD im Port geschrieben
  (re15_enemy_sca_clamp uebergibt `&e->ai_contact`, :12942-12944; Schreiber
  collision_constrain_impl re15_collision.c:766-767). Was fehlt, ist ausschliesslich die
  b93c-Abfrage + der A[4]-Path-A-Block. Der Kommentar :13379-13382 („der Maggot LIEST +0x90
  nie") ist in dieser Absolutheit falsch — @0x80117f18/@0x80117fc8 lesen +0x90 (aber die von
  b93c geschriebene Fassung).
- Die AUSFUEHRUNG ist vorhanden und wartet: B[7]-Varianten `sub_state_3!=0` — Windup-Slew auf
  `(+0x9f&0xf0)<<4` (:8513-8519 == @0x801189cc-a00), Blind-Impuls 0x32a (:8525 ==
  @0x80118a90-aa0), Yaw-Snap + `e->floor=1` (:8527-8530 == @0x80118ab4-af0), Landung
  `e->floor=0` (:8553 == @0x80118ca4). `dog_aux9f` (+0x9f) existiert; es setzt ihn nur niemand.
- `re15_dog_blocked(e)` = `sca_wall_hit` (=+0x1d6-Analog, :6645) gatet byte-true REAR-UP :8358
  und Path-B :8413 — d.h. der dauerklemmende Port-Gorilla verliert genau diese Angriffe.

## 4. Warum „koennen kaum angreifen" (AUFTRAG 3)

Alle Angriffs-Gates habe ich gegen eigenes Disasm geprueft — sie sind byte-true:

| Gate | Original | Port |
|---|---|---|
| BITE-Commit | `player.hit_react==0` @0x80117a54-5c, `a804(0xbb8,0x180)` @0x80117a60-74, `+0x1dc==0` @0x80117a88-90 | :8348-8351 (3000, 384) |
| REAR-UP-Upgrade | `a9cc(±0x20)>=0` @0x80117ab8-aec, `a804(0x9c4,0x100)` @0x80117af4-b04, **`+0x1d6==0`** @0x80117b18-20, `+0x1e1==0` @0x80117b28-30 | :8356-8360 |
| HEAVY-Commit | `a804(0xfa0,0xc0)` @0x80117e8c-9c, `+0x1dc==0` @0x80117eb0 | :8398-8399 (4000, 0xc0) |
| Fern-LEAP | LOS, **`+0x1d6==0`** @0x80118054, `dist>=0x1771` @0x8011806c, `a9cc==0` @0x80118084, `0x701||rng&1` @0x80118090-b4, `a780==0` @0x801180bc | :8412-8419 |
| Treffer-Fenster | Bone-QUADRATE `FUN_8001bff8` Biss Bone 9 r=1000 @0x801183c0-cc, Heavy dual 6/10 r=800 @0x801186f0-714 | :8445, :8476-8477 (bone_square, inkl. D4-Scale-Korrektur :8176-8179) |

Kein Schwellen-Defekt. Die Kette ist: **haengt an der Autozelle → `sca_wall_hit`=1 jeden Frame →
REAR-UP + Fern-LEAP hart gesperrt (byte-true!) → es blieben nur BITE (3000/±384) und HEAVY
(4000/±192), die der Gorilla hinter dem Auto nie in Reichweite/Kegel bekommt.** Im Original
loest der Zonen-Leap die Blockade auf, danach greifen die Gates wieder. Sekundaer verstaerkend
(verhalten.md D2, weiterhin offen): der aec4(player→gorilla)-Push des Root
(@0x80116e40-54) fehlt im 0x27-Zweig (:13383-13386 ruft nur `re15_enemy_body_push_tail`
= b544-Haelfte + Klemme) — Leon schiebt sich in Kollisionen statt der Gorilla.
Gorilla-vs-Gorilla-Separation ist paritaetisch (FUN_8002aec4 nutzt Box[6]/Box[10] = 1600/1600
als Ellipse, Decompilat RE_15_Quellcode_V2/FUN_8002aec4.c; Port `re15_body_push` mit
hit_radius_min 1600 beidseitig :12894-12900).

## 5. Messplan fuer den Hauptagenten (AUFTRAG 5)

Port-Sonde (Muster: verhalten.md §4 `probe_gorilla_11c0.c`; Harness-Fallen dort beachten —
EM027-Bank `bank->ok=1` + `pl->hit_react` ruecksetzen):
1. ROOM11C0 Kampf-Layout laden (SCD-Flag Zone 4 Bit 0x40 SETZEN, Zone 3 Bit 0x43 frei →
   else-Zweig spawnt 2× 0x27 grid 0x10 @(-9013,-15461)/(9434,2189)).
2. Spieler-Anker z.B. West-Gasse (-14000, 6000) setzen; 3600 Frames ticken. Pro Gorilla/Frame
   loggen: `x,z,y`, `state`, `sub_state_1..3`, `motion`, `sca_wall_hit` (+0x1d6-Analog),
   `ai_contact` (+0x90-Analog), `dog_blocked_ctr` (+0x1dc), `mag_pin_cd` (+0x1e1), dist.
3. Metriken:
   - Klemm-Quote: Frames mit `sca_wall_hit==1` bei `sub_state_1 ∈ {3,4}` / alle — Erwartung
     heute: Gorilla #2 (Ost-Tasche) nahe 100 %, Netto-Weg pro 300-Frame-Fenster < ~500.
   - Zonen-Leap-Zaehler: Eintritte `sub_state_1==7 && sub_state_3!=0` — heute exakt 0
     (Beweis „Entscheidung fehlt"); nach dem Fix > 0, wenn der Gorilla im Pad-Bereich
     (36: x[4900..9500] z[-6080..-2180] ±1700) frontal klemmt.
   - Angriffs-Rate: Eintritte sub 5/6/15 pro 3600 Frames vor/nach.
4. Original-Referenz: DuckStation `stage_saves/mzd_stage1_maggot.sav` bzw. Kampf-Provokation +
   RAM-Lesen `+0x5/+0x7/+0x82/+0x1d6` (Skill re15-savestate-ghidra); alternativ PCSX-Redux-
   Write-Watchpoint auf `+0x82` des Gorilla-Slots (Schreiber @0x80118af0) — jeder Treffer =
   ein Original-Zonen-Leap (Skill re15-pcsx-watchpoint; Rueckruf-Falle beachten).

## 6. Fix-Skizze (KEIN Code geschrieben)

1. **b93c portieren** (re15_collision.c): `re15_collision_zone_query(rdt, x, z, r+100, band,
   attr, &contact)` — Quadranten-Partition des Aktors, Match `u1 == attr && (floor>>4)==band`,
   Broadphase mit ±(r+100), beim ERSTEN Treffer `contact=(coll_contact_dir(...)>>4)+8+(u1&3)`
   schreiben (in `e->ai_contact`, wie @0x8003ba8c) und 1 liefern.
2. **A[4]-Path A einsetzen** (enemy_ai_common.c case 4, zwischen HEAVY-Commit :8398-8400 und
   Path B :8411): exakt die Sequenz aus §1.2 — `mag_boost!=0` → Query 0x10 → Yaw-Fenster
   `(((ai_contact&0xf0)<<4) - rot_y + 512)&0xfff < 1024` → `dog_aux9f=ai_contact`,
   `re15_dog_sub(e,7)`, `sub_state_3=1`; sonst Query 0x20 → bei LOS-Latch in die Path-B-Gates
   ab dem `+0x1d6`-Check springen, ohne LOS → gleiches Fenster → `sub_state_3=3`.
3. **Band-Byte statt band_from_y fuer den Gegner-Klemmpfad**: `re15_enemy_sca_clamp` (bzw.
   mindestens der 0x27-Zweig) liest `e->floor` (Original `+0x82`; INIT muss es aus dem Spawn-Y
   seeden), damit `e->floor=1/0` aus B[7] wirkt und der +0x7=0-Leap wieder am Band 0 klemmt.
   (Gleiche Pruefung fuer die anderen Boden-Gegner-Typen ansetzen — Original liest ueberall
   +0x82, @0x80117ee4 hier, Zombie/Hund analog.)
4. Danach D2 (aec4-player-Push) aus verhalten.md nachziehen (unabhaengig, sekundaer).

## 7. Offene Punkte (nicht erfunden, naechster RE-Weg)

- **FUN_8001bf04-Feinsemantik fuer „innerhalb der Zelle" (0xf00):** Wirkung im Yaw-Fenster
  (Ziel-Yaw 3840) plausibel als Diagonal-Fallback, aber nicht dynamisch verifiziert. Weg:
  PCSX-Redux-Watchpoint auf +0x9f-Write @0x80117f40/@0x80117ff0 im echten Kampf, +0x90-Wert
  und Gorilla-Position mitloggen.
- **Landeort-Klemme des Zonen-Leaps im Original** (klemmt Band 1 die volle Flugbahn inkl.
  9/39-Ost-Streifen?): statisch klar (Root-Tail laeuft airborne weiter, Gate +0x9&0x40 betrifft
  nur den Schatten-Block @0x80116e9c), Flugbahn-Endpunkte aber nur dynamisch messbar (gleicher
  Watchpoint-Lauf).
- Ob zusaetzlich die A/B-Lanes 9-14 (+0x1e3-Sonderleaps, verhalten.md D6) in ROOM11C0 je
  aktiv werden: kein STAGE1-Writer von +0x1e3!=0 gefunden (Port :8709-8712) — fuer dieses
  Symptom irrelevant.
