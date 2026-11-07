# WS — Entscheidungsblatt Waffenschaden (Rakete + Handgranate), Runde 41

Stand 2026-11-07. Dieses Prüfer-Blatt fasst Leser A und Leser B zusammen. Jede entscheidende Zahl habe ich SELBST nachgelesen.

Gelesen, nicht geändert:
* Integrationsbaum `C:/workspace/git/reAi_v2/.claude/worktrees/r41_integration` (r41/integration, Kopf 9867e3913)
* `C:/workspace/git/reAi_v2/info/re2leon/PSX.EXE`
* `C:/workspace/git/reAi_v2/info/re2leon/COMMON/BIN/EMOVL10_S0.BIN` und `.../EM23_OVL_0000.BIN`
* `C:/workspace/git/reAi_v2/info/Re1.5/PSX.EXE` und `C:/workspace/git/reAi_v2/info/Re1.5/PSX/BIN/STAGE1.BIN` bis `STAGE4.BIN`

Werkzeuge: `C:/workspace/git/reAi_v2/.claude/skills/re15-psx-disasm/scripts/re2_disasm.py` und `re15_disasm.py` (dis/read).
Ich habe nichts gebaut, nichts geändert, keinen Test gestartet und nichts committet.

Quellblätter:
* Leser A: `C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/ws/WS_LESER_A.md`
* Leser B: `C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/ws/WS_LESER_B.md`

---

## (a) Nutzerentscheid und Lesung

Nutzer 2026-11-07, wörtlich:

> "mach wie resident evil 2. Auch bei der Granate. mach sie so stark, daß sie gerade so die Standard Gegner mit einer Granate töten kann."

Lesung des Orchestrators (bindend):

* **(R) Rakete:** Der Schaden je Gegnertyp kommt aus der RE2-Schadenstabelle (Zeile 17, Klammer 0). Der RE1.5-Wert 1000
  (DAT_8006f418[9], `e8 03` @0x8006f42a) hat im Original keinen Aufrufer, deshalb ist RE2 das Vorbild (Beta -> Retail).
  Port-Gegner ohne RE2-Gegenstück bekommen den RE2-Wert des nächsten vergleichbaren Typs, mit Begründung.
* **(G) Handgranate:** Es gibt EINEN Schadenswert D_G. Er ist der kleinste Wert, mit dem ein Wurf jeden Standard-Gegner aus voller
  Gesundheit tötet. Mit der Todesregel hp < 0 gilt D_G = max HP + 1, und zwar über alle Standard-Gegner, alle Varianten, beide KIs
  und alle Schwierigkeitsgrade des Ports. Die Boss-Regel der Spur GR (Rest R, danach N Pistolenschüsse; GR_REST_* 40/110/50)
  ENTFÄLLT. Bosse nehmen D_G wie jeder andere Treffer, ohne Sonderfall.

Todesregel, selbst gelesen:

* RE1.5 FUN_80012d60: `subu v1,v1,a0` @0x80012ffc / `sh v1,154(s1)` @0x80013000 / `lh v1,154(s1)` @0x80013004 /
  `bgez v1,0x80013024` @0x80013014 (Zustand 2). Zustand 3 gibt es nur im Durchfall `ori v0,zero,0x3` @0x8001301c.
* RE2 FUN_800470C0: `lh v1,342(s1)` @0x8004727c / `bgez v1,0x80047294` @0x80047284 / `addiu v0,zero,3` @0x8004728c.
* **Mit 0 HP lebt ein Gegner noch. Tot ist er erst bei hp < 0.**

---

## 1. Selbst nachgerechnet (Prüfung von A und B)

### 1.1 RE2-Applier und Rakete (Leser A bestätigt)

| Stelle | Instruktion (selbst gelesen) | Ergebnis |
|---|---|---|
| Klammer | `srl s6,s5,28` @0x80047114 | K = Hitcode >> 28 |
| alle/erster | `lui v0,0x1` / `and v0,s5,v0` / `beq v0,zero,0x80047434` @0x80047208-10 | Bit 0x10000 |
| Zeile | `andi v1,s5,0xffff` @0x80047214 | Zeile = Hitcode & 0xFFFF |
| Typzeiger | `lbu v0,8(s1)` @0x80047218 … `lw a1,27272(at)` @0x8004722c (= 0x800a6a88) | *(0x800A6A88 + Typ*4) |
| Datensatz | `sll/addu/sll` @0x80047230-38, `addiu v0,v0,-20` @0x8004723c | + (Zeile-1)*20 |
| Schaden | `lw v1,0(a1)` @0x8004724c, `srlv v1,v1,v0` @0x80047254, `andi v1,v1,0x3ff` @0x8004725c | (w0 >> 10K) & 0x3FF |
| HP | `lhu v0,342(s1)` @0x80047258, `subu` @0x80047260, `sh v0,342(s1)` @0x80047268 | hp -= dmg |
| Kandidat | `lh v0,342(s0)` @0x80047148, `bltz v0,0x8004740c` @0x80047150 | hp < 0 = kein Ziel |
| Rakete, Gegnerkontakt | `lui a3,0x3` @0x8001f7b0, `jal 0x800470c0` @0x8001f7bc, `ori a3,a3,0x11` @0x8001f7c0 | 0x00030011 = Zeile 17, K0, alle |
| Rakete, Boden | `lui a3,0x2` @0x8001f820, `ori a3,a3,0x11` @0x8001f82c, `jal 0x800470c0` @0x8001f83c | 0x00020011 = Zeile 17, K0, erster |
| Raketenexplosion | Op 47: `addiu v0,zero,12` @0x80020d34, `bne v1,v0,0x80020dc0` @0x80020d38 | Sub 13 springt am Applier vorbei. Im ganzen Op 47 (0x80020c3c-0x80020f34) gibt es `jal 0x800470c0` nur @0x80020d78 und @0x80020db0 (Sub 12) |
| Boxen | `read 0x80010908` = {-800,0,400,200}; `read 0x80010918` = {-2000,0,1000,500} (`addiu a2,a2,2328` @0x80020c5c); zweiter Prüfpunkt `addiu v0,v0,900` @0x80020d98 | Flugbox der Rakete; Explosionskasten |

**Folge:** In RE2 trifft die Rakete jeden Gegner einmal, mit Zeile 17 und K0. Im Schadenspfad steht kein Flächenschaden,
kein Abfall, keine Schwierigkeit und kein Zufall: @0x80047208-0x8004727c enthält kein RNG-`jal` und liest 0x800CFB74 nicht.

### 1.2 RE2-Raketentabelle Zeile 17 (selbst gelesen, Python auf PSX.EXE, Zeiger @0x800A6A88 + Typ*4)

| RE2-Typ | Tabelle | Z17 @ | w0 | K0/K1/K2 | Sperre (w1>>9)&0x7F |
|---|---|---|---|---|---|
| 0x10/0x11 Zombie | 0x800a412c | @0x800a426c | 0x384e1384 | 900/900/900 | 15 |
| 0x16 Zombie | 0x800a42a8 | @0x800a43e8 | 0x384e1384 | 900 | 15 |
| 0x20 Hund | 0x800a4424 | @0x800a4564 | 0x12c4b12c | 300 | 15 |
| 0x21 Krähe | 0x800a45a0 | @0x800a46e0 | 0x12c4b12c | 300 | 15 |
| 0x22 Licker rot | 0x800a471c | @0x800a485c | 0x0c8320c8 | 200 | 15 |
| **0x23 Alligator** | 0x800a4a14 | **@0x800a4b54** | 0x06e1b86e | **110** | 15 |
| 0x25/0x26 Spinne/Baby | 0x800a4b90 | @0x800a4cd0 | 0x0c8320c8 | 200 | 15 |
| 0x27 G-Embryo | 0x800a4d0c | @0x800a4e4c | 0x0c9324c9 | 201 | 15 |
| 0x29 Kakerlake | 0x800a5004 | @0x800a5144 | 0x12c4b12c | 300 | 15 |
| 0x2A/0x2B Mr. X/Tyrant | 0x800a52fc / 0x800a5478 | @0x800a543c / @0x800a55b8 | 0x1f47d1f4 | 500 | 15 |
| 0x2D Zombie-Arme | 0x800a5180 | @0x800a52c0 | 0x0280a028 | 40 | 15 |
| 0x2E Ivy | 0x800a55f4 | @0x800a5734 | 0x12c4b12c | 300 | 15 |
| 0x30 G1 | 0x800a5770 | @0x800a58b0 | 0x0c8320c8 | 200 | 15 |
| 0x31 G2 | 0x800a58ec | @0x800a5a2c | 0x12c4b12c | 300 | 15 |
| 0x33 G3 | 0x800a5a68 | @0x800a5ba8 | 0x19064190 | 400 | 15 |
| **0x34 G4** | 0x800a5be4 | **@0x800a5d24** | 0x12d4b52d | **301** | 15 |
| **0x36 G5** | 0x800a5edc | **@0x800a601c** | 0x0c8320c8 | **200** | 15 |
| 0x37 (Vorgabezeiger) | 0x800a412c | @0x800a426c | 0x384e1384 | 900 | 15 |

Alle Werte von Leser A stimmen. Zeile 17 hat in jeder Klammer denselben Wert, und die Sperre ist überall 15.

### 1.3 HP der Standard-Gegner (Leser B bestätigt)

| Wert | Instruktion / Bytes (selbst gelesen) |
|---|---|
| RE2-Zombie, Tabelle | `andi v0,v0,0x40` @0x801006d4, `lhu v0,-14736(at)` @0x80100708 (= 0x8010c670), `sh v0,342(s2)` @0x80100710; `read 0x8010C670` = {80,94,**128**,75,60,95,58,75,50,83,79,66,80,65,82,65}. Die Tabelle @0x8010C600 liegt um 10 niedriger (EMOVL10_S0.BIN) |
| +15 bei weniger als 4 Gegnern | `sltiu v0,v0,0x4` @0x801007fc, `addiu v0,v0,15` @0x80100810 -> max 143 |
| **0x11 = 250 fest** | `addiu v0,zero,17` @0x801008a0, `bne v1,v0,0x801008d8` @0x801008bc, **`addiu v1,zero,250` @0x801008c8**, `sh v1,342(s2)` @0x801008cc (EMOVL10_S0.BIN). Die Stelle liegt im Code hinter dem +15 und überschreibt daher Wurf und Bonus |
| Bit 0x40 immer gesetzt | `lw v0,-1164(v0)` (0x800cfb74) @0x8002b4a4, `ori v0,v0,0x40` @0x8002b4ac, `sw` @0x8002b4b4 (RE2 PSX.EXE) |
| Port-Umsetzung | `re15_re2_init_hp` (re15_damage.c:957-990): 0x11 -> `hp = 250` (Zeile mit `@0x801008C8`). Vorgaben: Import AN (`s_re15_import = 1`, enemy_ai_re2_zombie.c:240), RE2-Modell AN (`s_re2_model = 1`, re15_damage.c:680) |
| RE1.5-Zeilen (Basis STAGE1 0x8011f034 + Typ*0x20) | 0x10 @0x8011f234 max 101; 0x11 @0x8011f254 max 107; 0x16 @0x8011f2f4 max 99; **0x18 @0x8011f334, Spalte 3 = 1058** (@0x8011f33a), sonst max 109; 0x20 @0x8011f434 max 111; 0x27 @0x8011f514 16 x 180; STAGE2 0x25 @0x8011761c max 136 (@0x80117634); 0x23 @0x801175dc 16 x 300; STAGE3 0x29 @0x8011d6e8 max 121; STAGE4 0x2B @0x80118b00 max 126 |

**Das Maximum der Standard-Gegner ist in allen Menü-Modi 250, Typ 0x11.** Menü-Modi sind RE2-KI, RE1.5-KI mit Import und MIXED.

### 1.4 Explosionsabfall

* **RE2:** Es gibt keinen Abfall. Op 47 hat einen Hitcode je Kasten (0x10020009, K1), und der Applier rechnet keinen Abstand (siehe 1.1).
* **Port:** `re15_granate_r35_explosion` (granate_r35.c:363-392) sammelt die Kandidaten an P und P+900. Jeder Slot zählt höchstens
  einmal (`schon`-Prüfung :377). Je Kandidat folgt `re15_resolver_gegnerzweig(…, art, punkt, NULL)` (:389). Den Schaden setzt
  `re15_enemy_take_damage_at`: bei `type == 2u` gilt `dmg = re15_granate_schaden(e)` (re15_damage.c:3222-3223). Abstand und
  Klammer spielen keine Rolle (gl = NULL).
* **Ergebnis:** Im Kasten wirkt der volle Wert D_G, außerhalb wirkt nichts. Die Kastengrenzen je Typ stehen bei Leser B §4.

### 1.5 Widersprüche zwischen A und B, mit Beleg entschieden

| Punkt | A | B | Entscheid + Beleg |
|---|---|---|---|
| 0x1A unter RE1.5-KI | RE2-EM2D-Gehirn in jedem Flavor | RE1.5-HP 72..99 (Nachrüstung) | **A.** `re15_ai_re2_for_type`: `if (type == 0x1Au && !s_writher_re15_pin) return 1;` (enemy_ai_re2_zombie.c:181). `s_writher_re15_pin` setzt nur ein Test-Setter (:171). Bs 72..99 gilt also nur unter diesem Testhaken. Für D_G ohne Einfluss |
| G5-Granate (E16 = 80) | Ist-Stand | Folge der Lesung: der Block muss weg | **B.** Lesung (G) verlangt einen Wert ohne Sonderfall. Die 80 sind RE2-Granatwerfer Zeile 9 (`05014050` @0x800A5F7C). Diese Zuordnung hat GR §4 für die Zombies schon verworfen. Art 3/4 (70) bleiben |
| G5 und Rakete | — (Gegenprüfung: "drei Raketen -> genau 0") | — | 600 - 3 x 200 = 0, der G5 lebt -> **4 Raketen**. So ist es auch in RE2 (gleicher Applier, gleiche Todesregel) |
| Gorilla, Rakete | Vorschlag 200 (Licker) oder 201 (gleiche Id) | — | **200** (@0x800a485c). Bei 360 HP ergeben 200 und 201 beide 2 Raketen, die Wahl ändert kein Ergebnis |
| Gator-Boss, HP | 25300 | 25300 | Beide übernehmen den Port-Wert. **Neuer Befund, siehe H3:** Mit gesetztem Bit 0x40 wären es byte-true 25240 |

---

## (b) Raketenschaden je Port-Gegnertyp

So läuft es heute im Port: Die RE2-Kandidaten (`re2_gl_typ`, re15_damage.c:4289) bekommen schon den RE2-Datensatz
(`s_re2gl_rec16_*`, :4233-4249). Alle anderen laufen über Art 9 und nehmen 1000. NEU kommt `re15_rakete_schaden(e)` dazu (siehe f).

| Port-Typ | RE2-Vorbild | Rakete NEU | Quelle | heute | Begründung, wenn das Gegenstück nicht gleich ist |
|---|---|---|---|---|---|
| 0x10/0x11/0x12/0x13/0x18 Zombies | 0x10.., gleiche Id | 900 | @0x800a426c | 900 (RE2-KI und Import), sonst 1000 | — |
| 0x16 Zombie | 0x16 | 900 | @0x800a43e8 | 900 | — |
| 0x14/0x15/0x17/0x19 (nicht gespawnt) | gleiche Id | 900 | @0x800a426c / @0x800a43e8 | 1000 | — |
| 0x1A Gitterhände | 0x2D Zombie-Arme | 40 | @0x800a52c0 | 40 | Das Port-Gehirn ist EM2D (enemy_ai_re2_zombie.c:181) |
| 0x20 Hund (beide KIs) | 0x20 | 300 | @0x800a4564 | RE2 300 / RE1.5-KI 1000 | — |
| 0x21 Krähe | 0x21 | 300 | @0x800a46e0 | RE2 300 / RE1.5-KI 1000 | — |
| 0x23 Alligator (RE1.5-KI, nur Messhilfe) | 0x23 | 110 | @0x800a4b54 | 1000 | — |
| 0x23 Gator-Boss 2090/2091 | 0x23 | 110 | @0x800a4b54 | 1000 | RE2-Alligator-Modul |
| 0x25 Spinne (beide KIs) | 0x25 | 200 | @0x800a4cd0 | RE2 200 / RE1.5-KI 1000 | — |
| 0x26 RE2-Baby | 0x26 | 200 | @0x800a4cd0 | 200 | — |
| 0x26 Feuer-Emitter 1090 | — | 200 | @0x800a4cd0 | 1000 | gleiche Id, kein Gegner (H9) |
| **0x27 Gorilla** | kein Gegenstück | **200** | **@0x800a485c** (Licker rot 0x22) | 1000 | RE2 hat keinen Affen, RE2-0x27 ist der G-Embryo. Am nächsten kommt der Licker, ein agiler Springer und Nahkämpfer. Die Alternative 201 (@0x800a4e4c, gleiche Id) ergibt dieselbe Zahl Raketen (2) |
| 0x29 kleiner Affe | kein Gegenstück | 300 | @0x800a5144 | 1000 | Alle kleinen RE2-Tiere tragen 300 (Hund @0x800a4564, Krähe @0x800a46e0, Kakerlake @0x800a5144). Der Wert ist damit eindeutig |
| 0x2B Tyrant | 0x2A/0x2B | 500 | @0x800a55b8 (= @0x800a543c) | 1000 | gleiche Kreatur |
| 0x2D Ivy | 0x2E Ivy | 300 | @0x800a5734 | 1000 | gleiche Kreatur; RE2-Id 0x2D sind die Arme |
| 0x30 Birkin (alle Menschen- und Mutationsformen) | 0x30 G1 | 200 | @0x800a58b0 | 1000 | gleiche Id. Nach der Mutation wäre G2 = 300 (@0x800a5a2c); die Treffer bis zum Tod bleiben gleich (H6) |
| **Riesenform** (0x30 mit G4-Bank) | 0x34 G4 | **301** | **@0x800a5d24** | 1000 (-> genau 0 HP) | G4-Overlay und G4-HP 1000 (`addiu v0,zero,1000` @0x80100678, GR N3.1) |
| **0x36 G5** (5090/5091) | 0x36 | **200** | **@0x800a601c** | 200 | — |
| 0x36 ROOM3080 | 0x36 | 200 | @0x800a601c | 1000 | Szene, nicht beschießbar (GR N3.4) |
| alle übrigen Typen (Vorgabe) | Vorgabezeiger | 900 | @0x800a426c (z. B. Zeiger 0x14 @0x800a6ad8 -> 0x800a412c) | 1000 | RE2-Typen ohne eigene Tabelle zeigen auf die Zombie-Tabelle |
| 0x22, 0x24, 0x37, 0x40-0x4D | — | — | — | — | kein Ziel (inert bzw. HP −1, `bltz` @0x80047150) |

---

## (c) D_G der Handgranate

**D_G = 251, also 250 + 1.**

* 250 ist das Maximum der Standard-Gegner und gehört zu Typ 0x11 (RE2-EM011): `addiu v1,zero,250` @0x801008C8 /
  `sh v1,342(s2)` @0x801008CC, in EMOVL10_S0.BIN.
* +1 kommt aus der Todesregel: `bgez v1` @0x80013014 (RE1.5) bzw. @0x80047284 (RE2).

Damit gilt 250 - 251 = -1 < 0, der 0x11 stirbt. Hätte ein Gegner 251 HP, stünde er danach mit 0 HP da und lebte.
251 ist also der kleinste Wert, der "gerade so" tötet.

| KI-Modus (Menü) | max HP Standard | Typ | D_G |
|---|---|---|---|
| RE2-KI (Vorgabe) | 250 | 0x11 (RE2-HP, `re15_re2_hp_sync`) | 251 |
| RE1.5-KI (Import an = Vorgabe) | 250 | 0x11 (RE2-HP über den Import) | 251 |
| MIXED | 250 | 0x11 (Import) | 251 |
| nur per Umgebungsvariable `RE15_RE15_RE2Z_IMPORT=0` / `RE15_RE2_DMG_MODEL=0` | 1058 | 0x18 Spalte 3 (@0x8011f33a) | (1059, nicht im Menü, H4) |

* **Schwierigkeit:** Der Port hat keine Schwierigkeitswahl. Das RE2-Bit 0x40 ist immer gesetzt (@0x8002b4ac), also gilt nur
  die Zombie-Tabelle @0x8010C670.
* **Abstand:** Es gibt keinen Explosionsabfall (1.4). D_G gilt im ganzen Kasten {-2000,0,1000,500} + r1ee/4 an P und P+900,
  außerhalb wirkt nichts.
* **Rangfolge darunter:**
  * 0x10/0x13/0x16/0x18: 143
  * Spinne RE1.5: 136
  * Hund und Spinne RE2: 132
  * Tyrant: 126
  * 0x29: 121
  * Ivy: 100

  Ohne 0x11 wäre D_G = 144 (H1).
* **Einstufung** (mit Beleg, Leser B §2):
  * **Standard:** 0x10, 0x11, 0x13, 0x16, 0x18, 0x20, 0x21, 0x25, 0x26 (RE2-Baby), 0x29, 0x2B, 0x2D.
  * **Bosse:** Gorilla 0x27 (Nutzer GR N2: "Der Gorilla ist ein Endgegner"), Birkin 0x30 in allen Formen samt Riesenform,
    G5 0x36 und der Gator-Boss 0x23.
  * **Gefahrenquelle, nie tot:** Gitterhände 0x1A. RE2 setzt im DEATH-Zweig 250 zurück (`e->hp = 250` @0x80101090-94,
    enemy_ai_re2_zellenarm.c:825). Mit 251 lösen sie trotzdem in einem Wurf ihren DEATH-Zweig aus (H8).
* **Stütze aus GR:** Schon in GR wollte der Nutzer, dass die Granate "auch die dicken Zombies zerfetzen" soll (re15_damage.c:3213).
  0x11 ist dieser dicke Zombie.

---

## (d) Treffer bis zum Tod (für die Release Notes)

Gerechnet aus belegten HP und Schadenswerten, für Gegner im Kasten bzw. in der Flugbox und aus voller Gesundheit.
**Nicht gemessen:** Die Messung gehört in die Umsetzung (f).

### Standard-Gegner

| Gegner | max HP (Beleg) | Rakete | Raketen | Granaten (251) |
|---|---|---|---|---|
| Zombies 0x10/0x12/0x13/0x16/0x18 | 143 (128 @0x8010C674 + 15 @0x80100810) | 900 | 1 | 1 |
| dicker Zombie 0x11 | 250 (@0x801008C8) | 900 | 1 | 1 (Rest −1) |
| Hund 0x20 | 132 RE2 / 111 RE1.5 | 300 | 1 | 1 |
| Krähe 0x21 | 10 / 0 | 300 | 1 | 1 |
| Spinne 0x25 / RE2-Baby 0x26 | 132 RE2 / 136 RE1.5; Baby 1 | 200 | 1 | 1 |
| kleiner Affe 0x29 | 121 (@0x8011d700) | 300 | 1 | 1 |
| Tyrant 0x2B | 126 (@0x80118b1a) | 500 | 1 | 1 |
| Ivy 0x2D | 100 | 300 | 1 | 1 |
| Gitterhände 0x1A (nie tot) | 250, nach DEATH wieder 250 | 40 | 7 bis zum DEATH-Zweig, dann Rückzug und wieder 250 | 1 bis zum DEATH-Zweig |

### Bosse

| Boss / Form | HP (Beleg) | Rakete | Raketen | Granaten | RE2-Vergleich |
|---|---|---|---|---|---|
| Gorilla 0x27 (1141, 11C0) | 360 = 180 @0x8011f514 x 2 | 200 | **2** (160, −40) | **2** (109, −142) | — |
| Birkin 600-Form (Leon 3070, 5080, Hangar 50A0, Labor 50E0/50F1) | 600 (0x12c @0x8011690c x 2) | 200 | **4** (400, 200, 0, −200) | **3** (349, 98, −153) | — |
| Birkin Form 1 bis zur Mutation (3071, 5090) | 600, Mutation bei hp < 200 | 200 | **3** (bis 0 < 200) | **2** (bis 98 < 200) | — |
| Birkin während der Mutation | 300; ein tödlicher Treffer setzt 100 (Revive @0x8011a530 x 2) | — | stirbt nie | stirbt nie | — |
| Birkin nach der Mutation (3071) | 300 (0x96 @0x80118094 x 2) | 200 | **2** | **2** (49, −202) | — |
| **Riesenform** (5090) | 1000 (G4-Ctor @0x80100678) | 301 | **4** (699, 398, 97, −204) | **4** (749, 498, 247, −4) | RE2-G4: 4 Raketen |
| **G5** 0x36 (5090) | 600 (@0x801003fc) | 200 | **4** (400, 200, 0, −200) | **3** (349, 98, −153) | RE2-G5: 4 Raketen |
| Gator-Boss 0x23 (2090), bis zur Schwäche | 25300 (Port), Schwäche bei hp < 25000 (`slti` @0x80101ec4) | 110 | **3** bis zur Schwäche (24970) | **2** bis zur Schwäche (24798) | töten kann nur der Kanister |
| Alligator 0x23 RE1.5-KI (kein RDT-Spawn, Messhilfe) | 300 (@0x801175dc) | 110 | 3 | 2 | — |

Die Birkin-Fern-Rückgabe von +5 je Treffer ab Abstand 0xfa1 (@0x8011a134) ändert keine dieser Zahlen. Ich habe jede Zeile
mit +5 je Treffer gegengerechnet.

### Ganze Kämpfe

| Kampf | Raketen | Granaten | vorher Raketen / Granaten |
|---|---|---|---|
| Leon 3070 | 4 | 3 | 1 / 1 Granate + 8 Schuss |
| Elza 3071 (Form 1 + nach der Mutation) | 3 + 2 = 5 | 2 + 2 = 4 | 2 / 2 Granaten + 16 Schuss |
| Endkampf 5090 (Form 1 + Riesenform + G5) | 3 + 4 + 4 = 11 | 2 + 4 + 3 = 9 | 1 + 2 (0-HP-Lage) + 3 |
| Gator-Boss bis zur Kanister-Szene | 3 | 2 | 1 / 1 Granate + 8 Schuss |
| Gorilla | 2 | 2 | 1 / 1 Granate + 4 Schuss |

---

## (e) Betroffene Riegel und Tests (git grep im Integrationsbaum)

Die Pfade sind relativ zu `C:/workspace/git/reAi_v2/.claude/worktrees/r41_integration/re15_port/`.

| ctest / Datei | Stelle | Erwartung neu |
|---|---|---|
| unit_r41_granate / `tests/unit/test_r41_granate.c` | PRUEF 1 (:269 `zk[i].hp - 1000`) | `- RE15_GRANATE_DG` (251); Zustand 3 bleibt (Brad 250 -> −1) |
|  | PRUEF 2 (:278 `107 - 1000`) | 107 − 251 = −144, Zustand 3 |
|  | PRUEF 4 (:314-318 `d == 1000`) | `d == 251` für alle 10 Typen x 3 KIs |
|  | [6] boss, PRUEF 70-82, 90, 91 (:455-560) | neu fassen: je Fall die **Zahl der Granaten bis zum Formende**. Gorilla 2, Alligator 2, 600-Form 3, Form 1 2, Mutation nie, nach der Mutation 2, Riesenform 4, G5 3, Gator 2 bis zur Schwäche. Pruefung 90 (Gorilla schwächer als Birkin) und 91 (Pistolensummen) entfallen. Neu: Schaden je Boss == D_G |
|  | PRUEF 102 (:706 `dmg_art[2] == 1000`) | `== 251` (0x36 in 3080, Vorgabe D_G) |
|  | PRUEF 61, [4] aus (:948 `250 - 1000`) | 250 − 251 = −1; der Schaden hängt nicht am Gore-Schalter |
|  | Kopfkommentar :1-26, :899 | Text "1000/Boss-Regel" -> D_G |
| unit_r41_granate_aus | wie [4] | wie PRUEF 61 |
| unit_r34_schaden / `tests/unit/probe_r34_schaden.c` | CHECK 13 (:93 `180 - 320`), 13b (:99 `180 - 1000`) | beide 180 − 251 = −71, Zustand 3 |
|  | CHECK 99 (:411 `d2 == 1000`) | `d2 == 251` |
|  | CHECK 31 (:496), 37 (:607), 38 (:615), je `80 - 1000` | 80 − 251 = −171 |
|  | CHECK 34 (:533 `250 - 1000`) | 250 − 251 = −1, Zustand 3 ("gerade so") |
|  | CHECK 35 (:574 `300 - 1000`, 0x16 mit HP 300) | 300 − 251 = **49**: Der Dummy hat mehr HP als jeder echte 0x16 und lebt jetzt. Erwartung ändern oder die Dummy-HP auf höchstens 250 setzen |
| unit_r34_wurf / `tests/unit/probe_r34_wurf.c` | PRUEF 24 (:344 `180 - 320`) | 180 − 251 = −71 |
|  | PRUEF nr+3 (:522, Art 3/4, `180 - 1000`) | unverändert, nur Art 2 ändert sich |
| unit_r35_granate / `tests/unit/test_r35_granate.c` | PRUEF 112 (:251 `180 - 320`) | −71 |
|  | PRUEF nr (:298 `100 - 1000`), 234 (:809) | 100 − 251 = −151 |
| unit_r34_reaktion (+ _ohne80) / `tests/unit/probe_r34_reaktion.c` | CHECK 190 (:991 `e->hp == 520`) | 600 − 251 = 349 |
|  | CHECK 194 (:1018 `e->hp == 440`) | 600 − 2 x 251 = 98 |
|  | CHECK 212 (:1305 `-900`) | 100 − 251 = −151 |
|  | CHECK 222 (Brad stirbt an HE) | bleibt grün (250 − 251 = −1); Kommentar :411 anpassen |
|  | 187 / 197 (HP 100, angeschlagen), Zensus 210/211 | nur Kommentare; laufen lassen, keine Erwartungsänderung erwartet |
| unit_birkin_bahnsteig / `tests/unit/test_birkin_bahnsteig.c` | M4 (:525 `re15_enemy_take_damage(b, 9)` muss Form 1 mit 600 töten) | mit 200 nicht mehr tödlich -> vor dem Treffer `b->hp` auf höchstens 199 setzen (Thema bleibt der tödliche Erst-Treffer) |
| unit_r35_werfer / `tests/unit/test_r35_werfer.c` | CHECK 84 (:361, Zombie 75 HP) | bleibt grün (900); Text "(1000/900)" -> 900 |
| unit (test_re2_weapon_rows, test_re2_zombie_teardeath, probe_re2_ztear) | Rakete am Zombie | 900, unverändert |
| unit_damage (`test_damage.c:85`) | Spieler, Art 2 = 1000 | **unverändert**: der Spielerzweig (re15_damage.c:232) bleibt DAT_8006f418[2] |
| test_writher_ai (:203) | HP 0 -> Tod | bleibt grün (0 − 251); Kommentar anpassen |
| integration_r35_granate / `tests/integration/test_r35_granate.cmake` | Lauf gator (:406 `r35_boss(gator … 250)`) | **251** |
|  | Lauf birkin (G5; :440 `- 80`, Text :420/:442) | **251** (600 -> 349; Reaktion und Weg wie bisher) |
| integration_r41_g5_tod / `tests/integration/test_r41_g5_tod.cmake` | Ablauf F413..F636, MA9, Kopf :17-22 | neu messen. Form 1 braucht 3 Raketen bis zur Mutation, die Riesenform 4 (1000 -> 699 -> 398 -> 97 -> −204), die "0-HP-Form" entfällt. Das Feuer (MA) muss bis zur 4. Riesenform-Rakete reichen und vor dem G5 enden, sonst trifft es den G5 (200 je Rakete). Die Bildnummern verschieben sich |
| integration_r41_g5_westen / `…/test_r41_g5_westen.cmake` | :17-22, `ein_lauf(... MA9 ...)` :161-163 | wie g5_tod; die Tasten ab W4.2 liegen relativ zum Szenenende |
| integration_r41_birkin_pistole | nur Pistole | nicht betroffen, die Form-HP bleiben gleich. Laufen lassen |
| integration_r34_granaten, integration_r36_granate, integration_r36_inventar, integration_r35_werfer_form (rakete10e0), integration_r35_werfer_ton | Zombies in 1140/1220/10E0 sterben weiterhin (max 250) | keine Erwartungsänderung; laufen lassen |
| integration_r41_re2gator / unit test_r41_re2gator, test_r41_alligator2 | nur Waffe 0 | nicht betroffen |

Ergänzend: Die Release Notes der Spur GR (Boss-Regel, Tabelle N3.3) und `analysis/befunde_runde41/GR_granate.md` N2/N3
als "überholt durch WS" markieren.

---

## (f) Bauplan (kleinster Eingriff)

| # | Datei | Funktion | Eingriff |
|---|---|---|---|
| 1 | `include/re15_damage.h` | — | `#define RE15_GRANATE_DG (250 + 1)` mit Kommentar: Maximum Standard 0x11 `addiu v1,zero,250` @0x801008C8 (EMOVL10_S0.BIN) + Todesregel `bgez` @0x80013014 / @0x80047284. Dazu die Deklaration `int re15_rakete_schaden(const re15_actor_t *e);` |
| 2 | `engine/src/re15_damage.c` | `re15_granate_schaden` (:3152-3172) | Rumpf wird `(void)e; return RE15_GRANATE_DG;`. `GR_REST_*` / `GR_HP_*` (:3147-3151) und den Boss-Kopfblock (:3114-3146) löschen; neuer Kopf mit Nutzerentscheid 2026-11-07 und Lesung G. Die Signatur bleibt |
| 3 | `engine/src/re15_damage.c` | `re15_enemy_take_damage_at`, E16-Block (:3255-3257) | Bedingung `type >= 2u && type <= 4u` wird `type >= 3u && type <= 4u` mit `dmg = 70` (Säure/Brand bleiben RE2-Record Z11/Z10 @0x800A5FA4/@0x800A5F90). Art 2 am G5 ist dann D_G |
| 4 | `engine/src/re15_damage.c` | `re15_enemy_take_damage_at`, direkt nach `if (type == 2u) …` (:3222-3223) | `if (type == 9u) dmg = (int16_t)re15_rakete_schaden(e);`. Art 9 hat im Port genau einen Erzeuger, die Applier-Zeile 17 für RE1.5-Kandidaten (re15_damage.c:4603). Der gl-Block danach (:3232-3240) gibt den Import-Zombies dieselben 900 |
| 5 | `engine/src/re15_damage.c` (bei `re2_gl_rec16_17`, :4240) | neu: `re15_rakete_schaden` | `switch (e->type)`, siehe Liste unter der Tabelle |
| 6 | `engine/src/birkin_formen.c` | Kopf von `re15_bf_granate_form` (:81-97) | nur Kommentar: wird jetzt nur noch als Riesenform-Prädikat der Rakete gebraucht (Granate = D_G) |
| 7 | `include/re15_gator_re2_r41.h` | — | **nicht** ändern (H3 ist eine Nutzerfrage) |

Zu #5, die Fälle von `re15_rakete_schaden`:

* 0x10-0x13 und 0x18 -> 900 @0x800A426C
* 0x16 -> 900 @0x800A43E8
* 0x1A -> 40 @0x800A52C0
* 0x20 -> 300 @0x800A4564
* 0x21 -> 300 @0x800A46E0
* 0x23 -> 110 @0x800A4B54
* 0x25/0x26 -> 200 @0x800A4CD0
* 0x27 -> 200 @0x800A485C (Licker; Begründung in den Kommentar)
* 0x29 -> 300 @0x800A5144
* 0x2B -> 500 @0x800A55B8
* 0x2D -> 300 @0x800A5734 (RE2 0x2E)
* 0x30 -> 301 @0x800A5D24 für die Riesenform, sonst 200 @0x800A58B0. Prädikat: `re15_bf_granate_form(e,&hp0,&ende)` mit
  `hp0 == RE15_BB_FORM_HP`, das gleiche wie birkin_formen.c:102
* 0x36 -> 200 @0x800A601C
* default -> 900 @0x800A426C (Vorgabezeiger, z. B. @0x800a6ad8)

Die Werte nicht als lose Zahlen eintragen: Die w0-Wörter mit Adresse als `static const uint32_t` ablegen und den Schaden mit
`(w0 >> 0) & 0x3FF` daraus rechnen.

**Unverändert bleiben:**
* der Applier `re15_re2_gl_apply` und die RE2-Records `s_re2gl_rec16_*`
* `re2_fx.c` op_24 und `granate_r35.c`
* die HP aller Gegner
* der Spielerzweig (re15_damage.c:232)
* die Zerfetzen-Regel (Tod durch Explosivgranate, unabhängig vom Betrag)

**Prüfhaken und Gegenprobe** (ohne Umgebungsschalter, siehe Memory "Original oder nicht"):

* **Vorher-Lauf:** Den neuen Testabschnitt `[8] ws` in `tests/unit/test_r41_granate.c` gegen die Engine VOR der Änderung bauen
  (Engine-Dateien per `git stash`, Test unverändert, Muster GR N2.1). Er muss genau an diesen Stellen rot werden:
  * Rakete an der Riesenform: 1000 -> 0, sie lebt (neu 301)
  * Rakete an Gorilla, Birkin und Gator: 1000
  * Granate am G5: 80
  * Granate an Gorilla 320, Gator 250, Birkin 490/290/190/890
* **Im selben Lauf, "gerade so":** Ein Dummy 0x11 mit HP 250 stirbt (−1). Ein Dummy mit HP = D_G (251) lebt mit 0 (Zustand 2).
  Das belegt Minimalität und Todesregel zugleich.
* **Rakete:** Je Port-Typ über `re15_re2_gl_apply(P, 0, {-800,0,400,200}, 0x30011)` den Schaden messen, unter RE2-KI, RE1.5-KI
  und MIXED (Dummy-HP 2000). Soll ist Tabelle (b).
  * Riesenform über den echten Eintritt (`g4_haken`): Nach 3 Raketen hat sie 97 HP und lebt, die 4. tötet.
  * G5: Nach 3 Raketen hat er 0 HP und lebt, die 4. tötet.
* **Granate:** [6] boss zählt je Fall die Granaten bis zum Formende (Tabelle d). Dazu je Typ: Schaden == D_G, ohne Ausnahme.
* **Danach:** gezielt `ctest -R "granate|r34_schaden|r34_wurf|r34_reaktion|bahnsteig|werfer|g5|birkin|gator|damage|writher"`,
  dann die Integrationsläufe aus (e) mit Neumessung von g5_tod und g5_westen.

---

## (g) Hinweise und Grenzfälle für die Release Notes

* **H1 – 0x11 bestimmt D_G.** In RE2 hat der Typ 0x11 feste 250 HP; dort ist er Brad Vickers und kommt nur einmal vor. Im Port ist
  er ein regulärer, häufiger Zombie (72 Records in 11 Raumpaaren) und zählt deshalb als Standard. Ohne ihn wäre D_G = 144 (143 + 1).
  Die nächsten Typen liegen dicht beieinander: 143 / 136 / 132 / 126 / 121.
* **H2 – Der Gorilla zählt als Boss.** Nach dem Nutzerzitat (GR N2) ist er Endgegner. Als Standard-Gegner gezählt wäre D_G = 361
  (360 + 1), und eine Granate würde jeden Birkin-Kampf deutlich verkürzen. Mit D_G 251 braucht er 2 Granaten und 2 Raketen.
* **H3 – Gator-Boss-HP: Widerspruch im Port (neu).**
  * RE2 setzt die Start-HP mit `andi v0,v1,0x40` @0x80100524 / `beq v0,zero,0x80100558` @0x80100528.
  * Ist Bit 0x40 gesetzt, liest RE2 0x801045ec bzw. 0x801045ee. Beide enthalten **240** (`read` = [240, 240]), das ergibt **25240**.
  * Nur ohne Bit 0x40 liest RE2 0x801045be = 300, das ergibt 25300.
  * Der Init setzt das Bit bedingungslos (`ori v0,v0,0x40` @0x8002b4ac). Für die Zombie-Tabelle, den G4 (1000) und den G5 (600)
    nimmt der Port genau diesen Zweig.
  * `RE15_GR2_HP_INIT 25300` trägt dagegen den Kommentar "(Bit 0x40 aus)" (include/re15_gator_re2_r41.h:28). Byte-true wären also 25240.
  * Folge: **Eine** Granate statt zwei schwächt ihn und startet die Kanister-Szene (25240 − 251 = 24989 < 25000). Die Raketen bleiben bei 3.
  * Das ist nicht Teil von WS: den Nutzer entscheiden lassen, nichts ändern.
* **H4 – Umgebungs-Modi.** Mit `RE15_RE15_RE2Z_IMPORT=0` oder `RE15_RE2_DMG_MODEL=0` gilt die byte-true RE1.5-HP-Zeile; einen
  Menüpunkt dafür gibt es nicht. Dort hat 0x18 einmal 1058 HP (@0x8011f33a, 1 von 16 Würfen). Dieser 0x18 überlebt eine Granate
  mit 807 HP und stirbt nach 2 Raketen.
* **H5 – Kein Abfall.** Die Granate wirkt im ganzen RE2-Kasten mit vollem Wert und außerhalb gar nicht. Der RE2-Granatwerfer
  unterscheidet dagegen Direkttreffer (K0) und Splitter (K1), beim Zombie zum Beispiel 200/50. Die Handgranate des Ports hat keinen
  Direkttreffer, der Flugkontakt löst nur aus. Darum gibt es nur den einen Wert.
* **H6 – Birkin nach der Mutation.** RE1.5 kennt nur den Typ 0x30 und nimmt deshalb G1 = 200. Wer der RE2-Mutation G1 -> G2 folgt,
  bekäme 300 (@0x800a5a2c). Die Zahl der Raketen bleibt 2, denn 300 − 300 = 0 und mit 0 HP lebt er noch.
* **H7 – Mit 0 HP lebt ein Gegner.** G5 und Birkin-600-Form stehen nach 3 Raketen mit genau 0 HP da, erst die 4. tötet. Das ist
  original (`bgez` @0x80013014 / @0x80047284) und gilt in RE2 genauso.
* **H8 – Gitterhände.** Sie sterben nie. D_G 251 löst aber in einem Wurf ihren DEATH-Zweig aus (250 − 251 = −1): Die HP gehen
  zurück auf 250, der Arm zieht sich zurück (@0x80101090-94).
* **H9 – Feuer-Emitter 0x26 in ROOM1090.** Er ist kein Gegner. Granate und Rakete treffen ihn mit 251 bzw. 200 statt 1000.
  Ob er im Spiel überhaupt Kandidat ist, habe ich nicht gemessen.
* **H10 – Gator-Boss in der Großreaktion.** `gb_absorb_hit` nimmt in GBP_FLINCH und GBP_RE2_TOD den Schaden zurück
  (enemy_ai_boss_gator.c:801-802; RE2 +0x1D3 |= 0x80 @0x80102c20-28). Ein Treffer in dieser Phase zählt nicht.
* **H11 – Endkampf mit der Rakete.** Er braucht 11 Raketen (3 + 4 + 4) statt 3. Laut Item-Zensus (GR N3.5) liegen im Raum keine Raketen.
* **H12 – Was gleich bleibt:**
  * Säure- und Brandgranate (Art 3/4)
  * Werferrunden (Art 6..8)
  * die Spielerseite (Art 2 am Spieler = 1000)
  * alle HP-Werte
