# Birkin schiebt den Spieler aus dem begehbaren Bereich (ROOM5090, Runde 20, 2026-09-21)

Nutzer, woertlich: *"Jetzt wurde ich von birkin rausgeschoben außerhalb des begehbaren
BEreiches - siehe Marker. Das darf nicht passieren."*

Marke: `re15_port/build/platform/pc/befund_5090_F962_marke1.bmp`, Protokoll
`re15_port/build/platform/pc/befund.log` ab Zeile 15308.

Worktree `.claude/worktrees/wf_e1e0203e-e6d-1`, Bauverzeichnis
`re15_port/build_birkin_push`, Sonde `re15_port/tests/unit/probe_r20_birkin_push.c`
(Registrierung `re15_port/tests/unit/probes/r20_birkin_push.cmake`).

---

## 0. Kurzfassung

**Reproduziert, und die Ursache ist NICHT der Boss-Schub selbst, sondern WANN der Port ihn
laufen laesst.**

1. **Der Port hatte den Koerper-Schub und die Wandklemme in den Normal-Zweig gehaengt.** Das
   Original ruft beide AUSSERHALB jedes Kommando-Handlers: `jalr v0` @0x80031cb4 (der
   Kommando-Handler), dann `jal 0x8002b544` @0x80031cbc (Koerper-Schub), dann
   `jal 0x8003b0a4` @0x80031d70 (Wandklemme). Waehrend Treffer-Reaktion, Knockdown, Grab und
   Tod lief im Port also KEIN Schub. Der Boss lief dem flinchenden Spieler **22 Bilder lang**
   in den Koerper (gemessen), die Ueberdeckung wuchs auf 2016 Einheiten an und entlud sich im
   ersten Normal-Bild danach als **EIN Sprung von 2112 Einheiten**.
2. **Der Positions-Spiegel Spieler+0x40/+0x42/+0x44 wurde im Port nur beim Leiter-Steigen
   geschrieben.** Das Original schreibt ihn in jedem Bild aus der Spielerposition
   (@0x8001d0d4/@0x8001d0dc/@0x8001d0e4). Genau diesen Spiegel liest der
   Vorzeichen-Dreh-Zweig von FUN_80034D0C (@0x80034ec4-0x80035044); dreht er, ist der Schub
   nicht `over`, sondern **+-2*r = +-12000**. Mit einem stehengebliebenen Leiter-Wert dreht er
   an 741 von 494 gepruefeten Punkten und erreicht **23911 Einheiten**.
3. **Die Wand, die ueberquert wurde, ist mit Spielerradius und Saum nur 2936 dick** (SCA-Zelle
   #28 `z -21700..-19700`, `density 2000`, plus 2x(450+0x12)). Beide Defekte liefern Spruenge
   in dieser Groessenordnung oder darueber — und ein Sprung ueber die Wand landet dort, wo die
   Klemme nichts mehr loesen kann.
4. **Die Zahl -19232 aus dem Protokoll des Nutzers ist kein Zufall.** Sie ist der Fixpunkt der
   Klemme zwischen zwei UEBERLAPPENDEN Band-0-Zellen: #23 schiebt nach
   `-19870-450-0x12 = -20338`, #28 schiebt von seiner Rueckseite nach
   `-19700+450+0x12 = -19232` zurueck, und weil FUN_8003b0a4 kein `break` hat und die LETZTE
   treffende Zelle gewinnt (@0x8003b510-20), ist die Summe exakt 0. Deshalb stand `z` im
   Protokoll ueber Hunderte Bilder auf -19232 fest, waehrend `x` pendelte. **Das ist
   Original-Geometrie plus die byte-true Schleife, kein Port-Defekt** — aber es ist der Grund,
   warum der Spieler nicht mehr herauskam.

**Zwei Deutungen des Auftrags sind gemessen GEFALLEN** (§5): 0x8004c1bc ist NICHT die
Raumklemme, und 0x8003567c/0x80077600 sind keine Klemme, sondern zwei Abfragen.

---

## 1. Reproduktion

### 1.1 Die Geometrie an der Marke (Sonde `zellen`)

Band-0-SCA-Zellen von ROOM5090 im Bereich der Marke (jede Zelle steht 5x im Dump, die
Quadranten-Gruppen sind bytegleiche Duplikate):

| # | typ | u0 | x | z | Breite x Tiefe |
|---|-----|----|---|---|----|
| 23 | 1 | 255 | -5673..28057 | -19870..-16864 | 33730 x 3006 |
| 28 | 1 | 255 | 4050..20225 | -21700..-19700 | 16175 x 2000 |
| 29 | 1 | 255 | 5300..20250 | -27200..-25200 | 14950 x 2000 |
| 30 | 6 | 255 | -425..5300 | -27200..-25200 | 5725 x 2000 |

Begehbar bei `x = 11102` ist damit nur `z = -24732..-22168` (Zellenkante + 450 + 0x12). Die
Marke des Nutzers liegt bei `z = -19232`, also **2936 hinter** der Korridorwand, INNERHALB von
Zelle #23.

### 1.2 Was im Protokoll des Nutzers steht

```
F811  pos=( 9000, 0,-23550)      <- steht im Korridor, z ueber Hunderte Bilder fest
F826  pos=( 9044, 0,-24943)      <- 211 hinter der -25200er-Wand
F841  pos=(11436, 0,-22142)      <- 26 hinter der -21700er-Wand
F856  pos=( 9688, 0,-19232)      <- DURCH, und ab hier z fest auf -19232
F962  pos=(11102, 0,-19232)      <- die Marke; Boss (4172,0,-23400), 8930 entfernt
```

Das Protokoll wird nur alle 15 Bilder geschrieben — der eigentliche Sprung ist darin nicht
aufgeloest. Deshalb der eigene Messlauf.

### 1.3 Der Messlauf (Sonde `lauf`, Mess-Schiene `RE15_PUSH_LOG`)

Neu in `game_step_common.c`: `RE15_PUSH_LOG` gibt je Bild die vier Stationen des
Spieler-Schritts aus (vor `player_tick`, nach `player_tick`, nach dem Koerper-Schub, nach der
Wandklemme). Neu in `enemy_ai_boss_g5.c`: je Segment Mitte, Radius, dx/dz, Broadphase,
Abstand und Ueberdeckung.

Spieler auf (9000,-23550) gesetzt (die Stelle aus F811), Boss zieht sich heran, kein
Pad-Eingriff:

```
f119 Spieler=(8805,-22320) hp=100 clip=212 hr=0   Boss=(4539,-23400)
f120 Spieler=(8900,-22304) hp= 60 clip=212 hr=1   Boss=(4718,-23400)   <- Treffer, 40 Schaden
f121 Spieler=(9100,-22306) hp= 60 clip=  8 hr=1   Boss=(4931,-23400)   <- Flinch-Clip 8
f124 Spieler=(9400,-22308) hp= 60 clip=  8 hr=1   Boss=(5760,-23400)
 ...  Spieler UNVERAENDERT ueber 22 Bilder, Boss laeuft 2150 Einheiten weiter ...
f142 Spieler=(9400,-22308) hp= 60 clip=  8 hr=1   Boss=(7092,-23400)
f143 Spieler=(11378,-22168) hp=60 clip=212 hr=0   Boss=(7068,-23400)   <- ENTLADUNG
```

Und die Segmentmessung an genau diesen Bildern:

```
[g5push] seg0 mitte=(2539,-1500,-23400) r=6000 rs=6450 dx=6266 dz=1080 dist=6353 over=  97
   (22 Bilder ohne Schub-Aufruf; die Zeile fehlt dort ganz)
[g5push] seg0 mitte=(5092,-1500,-23400) r=6000 rs=6450 dx=4308 dz=1092 dist=4434 over=2016
[push]  vor=(9400,-22308) tick=(9400,-22308) schub=(11378,-21759) d=(1978,549)
        klemme=(11378,-22168) dk=(0,-409)
```

**Der Schub von 1978/549 ist die auf 22 Bilder aufgelaufene Ueberdeckung, in einem Bild
freigegeben.** Er landete hier zufaellig genau auf der Wandkante (-22168) — 768 mehr und er
waere durch.

### 1.4 Warum die 22 Bilder keinen Schub hatten

`re15_game_step` (game_step_common.c) waehlt einen von acht Zweigen. Sieben davon
uebersprangen `re15_body_push_player()` und `re15_collision_constrain()` komplett; nur der
Normal-Zweig fuehrte beide. Der Treffer-Zweig (`s_hit_flinch > 0`) klemmte zwar fuer seinen
eigenen Rueckstoss, schob aber nicht aus dem Gegnerkoerper heraus.

---

## 2. Beleg: das Original bindet beides NICHT an das Kommandowort

`FUN_80031c44` (RE1.5-PSX.EXE, selbst disassembliert mit
`.claude/skills/re15-psx-disasm/scripts/re15_disasm.py`):

```
80031c54  lw   a0,-13760(a0)      a0 = g_pauseflags (0x800aca40)
80031c78  bltz a0,0x80031da8      <- die EINZIGE Schranke fuer den ganzen Block
80031c8c  lbu  v1,-13736(v1)      v1 = 0x800aca58 = Spieler+0x04 = das Kommandowort
80031ca4  addiu at,at,16272       at = 0x80073f90
80031cac  lw   v0,0(at)           v0 = Tabelle[cmd]
80031cb4  jalr v0                 <- der KOMMANDO-HANDLER laeuft hier
80031cbc  jal  0x8002b544         <- KOERPER-SCHUB, ausserhalb jedes Handlers
80031d38  jal  0x8002dc48
80031d4c  andi v0,v0,0x4400 -> 0x4000
80031d50  bne  v0,zero,0x80031d78 <- NUR der 0x4000-Latch (auf einem Objekt stehend)
                                     laesst die Wandklemme ausfallen
80031d58  jal  0x8002b498
80031d68  addiu a0,s0,52          a0 = Spieler+0x34 = Positionsvektor
80031d6c  lhu  a1,6(v0)           a1 = Radius aus 0x800acacc+6
80031d70  jal  0x8003b0a4         <- WANDKLEMME, ebenfalls ausserhalb der Handler
80031d74  ori  a2,zero,0x1        a2 = Solid-Maske 1
```

Die Kommando-Tabelle @0x80073f90, selbst ausgelesen:

```
[0] 0x800318f8   [1] 0x80031de8   [2] 0x80035af0 (Treffer/Knockdown)   [3] 0x800366bc (Tod)
[4] 0x80030660 (Treppe)   [5] 0x80036834 (Grab)   [6] 0x800368c0   [7] 0x8003694c (Tod)
```

Alle acht kehren zum `jal 0x8002b544` zurueck.

`FUN_8002b544` selbst hat keine Spieler-Zustands-Schranke:

```
8002b550  lbu  s1,-13746(s1)      s1 = g_active_count
8002b564  addiu s0,s0,-13268      s0 = enemy_array
8002b578  andi v0,v0,0x1          je Eintrag: nur word0-Bit 0 (aktiv)
8002b590  beq  a1,s0,0x8002b5a4   und: other != g_entity(cur)
8002b598  jal  0x8002aec4         -> der eigentliche Push-out
8002b5ac  addiu s0,s0,500         Stride 500 = 0x1F4
```

---

## 3. Beleg: der Positions-Spiegel +0x40/+0x42/+0x44

`FUN_8003b0a4` holt sich das "vorher" NICHT vom Aufrufer. Es dispatcht den Zellen-Handler mit

```
8003b47c  addu  a0,s4,zero        a0 = Zelle
8003b488  lbu   v0,-2(s2)         Zellentyp
8003b48c  addiu a1,a2,52          a1 = Entity+0x34 = LIVE-Position
8003b4a0  lw    v0,0(at)          Handler-Tabelle @0x800b2858
8003b4a8  jalr  v0
8003b4ac  addiu a2,a2,64          a2 = Entity+0x40 = der POSITIONS-SPIEGEL
```

und der Rechteck-Handler `FUN_8003bca8` liest ihn als Halbworte:

```
8003bd44  lh v0,0(a2)             prev.x
8003bd48  lh v1,4(a2)             prev.z
8003bd4c  subu v0,t3,v0           liveX - prevX   (Bewegungs-Vorzeichen fuer den Code)
```

Geschrieben wird der Spieler-Spiegel im Kopf der Haupt-Entity-Schleife, unbedingt, jedes Bild:

```
8001d0b4  lhu v1,-13688(v1)       v1 = *(u16*)0x800aca88 = Spieler+0x34 (X)
8001d0bc  lhu a0,-13684(a0)       a0 = *(u16*)0x800aca8c = Spieler+0x38 (Y)
8001d0c4  lhu a1,-13680(a1)       a1 = *(u16*)0x800aca90 = Spieler+0x3c (Z)
8001d0d4  sh  v1,-13676(at)       -> 0x800aca94 = Spieler+0x40
8001d0dc  sh  a0,-13674(at)       -> 0x800aca96 = Spieler+0x42
8001d0e4  sh  a1,-13672(at)       -> 0x800aca98 = Spieler+0x44
```

und je Aktor nach dessen Tick noch einmal:

```
8001d108  jal 0x8001e8c8          (der Entity-Tick)
8001d11c  lhu v1,52(v0)  / 8001d124 sh v1,64(v0)
8001d134  lhu v1,56(v0)  / 8001d13c sh v1,66(v0)
8001d14c  lhu v1,60(v0)  / 8001d154 sh v1,68(v0)
```

Im Port war `re15_actor_t.pos_s_x/y/z` **ausschliesslich** von `climb_common.c` beschrieben
(@0x80038210, @0x800384a0, @0x80038628 …) und wurde nie zurueckgesetzt.

**Messung (Sonde, Abschnitt D, 494 begehbare Kontaktpunkte am Segmentrand):**

| Spiegel-Inhalt | Vorzeichen-Drehungen | groesster Ein-Bild-Schub |
|---|---|---|
| Bildanfangs-Position (@0x8001d0d4-e4, der Fix) | 0 | 2297 |
| 0 (frisches Spiel, Port ohne Schreiber) | 0 | 2297 |
| stehengebliebener Leiter-Wert (-8000,-3600,-8000) | **741** | **23911** |

Mit korrektem Spiegel kann der Zweig nicht drehen: `h2 = y_vorher + (-1530) - (boss_y - 1500)`
ist auf dem Boden -30, das Band ist +-3030.

---

## 4. Der Fix

### 4.1 `game_step_common.c` — der Schwanz des Dispatchers

Neue Funktion `re15_player_body_and_walls(c, pl, alt_x, alt_z)`: Koerper-Schub
(@0x80031cbc) -> Wandklemme (@0x80031d70). Der Objekt-Pass FUN_8002bd44 (@0x8001ce14) ist
BEWUSST nicht dabei — er ist im Original ein eigener Top-Level-Aufruf neben dem
Spieler-Dispatcher, und ob er je Kommandowort laeuft, habe ich nicht disassembliert (§8).
Gerufen zusaetzlich in den vier Kampf-Zweigen, die sie bisher uebersprangen:

* Tod (cmd 3/6/7) — der Schub selbst steigt bei HP < 0 aus, die Klemme bleibt
* Grab (cmd 5) — das greifende Paar ist per +0x1000-AND ausgenommen (FUN_8002af14), ein
  dritter Gegner schiebt sehr wohl
* Knockdown (cmd 2 [4]/[5])
* Treffer-Reaktion (cmd 2) — NACH dem Handler, also nach dem Rueckstoss

### 4.2 `game_step_common.c` — der Positions-Spiegel

Am Anfang von `re15_game_step`, vor allem anderen:

```c
pl->pos_s_x = (uint16_t)(int32_t)pl->x;      /* @0x8001d0d4 */
pl->pos_s_y = (uint16_t)(int32_t)pl->y;      /* @0x8001d0dc */
pl->pos_s_z = (uint16_t)(int32_t)pl->z;      /* @0x8001d0e4 */
```

Die eigenen Spiegel-Schreiber des Klettern-Handlers laufen danach — dieselbe Reihenfolge wie
im Original (Schleifenkopf, dann Handler).

### 4.3 Nicht angefasst

Treppe (cmd 4 @0x80030660) und Klettern behalten ihren Schuss-Ausfall. Beim Klettern deckt
der 0x4000-Latch (@0x80031d4c-50) die Wandklemme ab und Substate 10 klemmt selbst
(@0x800384c4, der zweite der beiden Aufrufer von FUN_8003b0a4). Fuer die Treppe habe ich
diesen Beleg NICHT — sie steht als offener Punkt in §6, nicht als Fix.

---

## 5. Was die Messung gekippt hat

Beide Punkte kamen als gesetzt in den Auftrag und sind falsch:

1. **"@0x80026670 jal 0x8004c1bc = die RAUMKLEMME mit Radius 450."** Nein.
   `FUN_8004c1bc` liest die Position nur (`lw s4,0(s6)` / `lw s2,8(s6)` @0x8004c210-14),
   schreibt sie nirgends zurueck, und ihr Rueckgabewert landet als Halbwort in der OBEREN
   Haelfte von Spieler+0x110 (`sll v0,v0,16` / `or` / `sw v1,15136(s3)` @0x8002667c-84). Sie
   ist eine **Attribut-Abfrage**, das RE2-Gegenstueck zu RE1.5 FUN_8003b7f0/FUN_8003b93c.
2. **"Zwischen Schub und Klemme stehen zwei Aufrufe, die der Port moeglicherweise nicht hat."**
   Sie sind keine Klemme:
   * `FUN_8003567c(entity, 0x8000)` nullt Spieler+0x110 (@0x8003569c), ruft dann dieselbe
     Abfrage `0x8004c1bc` fuer die EIGENE Position (a0 = entity+0x84 @0x800356c0) und legt am
     Ende die Position als Halbwort-Spiegel nach +0x118/+0x11A (@0x800356ec-f8) — das RE2-
     Gegenstueck zu genau dem Spiegel aus §3.
   * `FUN_80077600(entity)` rechnet `(179*radius)>>7` (@0x8007762c-48), dreht das mit dem
     Yaw aus +0x76 (`jal 0x80077384` @0x80077650) und legt den so entstandenen
     VORAUSSCHAU-Punkt nach +0x15C/+0x160 (@0x8007766c/@0x80077680). Genau diese beiden
     Felder schreibt der Block danach in den Abfrage-Vektor 0x800c3a90 (@0x80026664/6c).
   Es sind also **zwei Abfragen** (am eigenen Platz und ein Stueck voraus), kein Schub und
   keine Klemme. Die RE2-Seite hat an dieser Stelle gar keine Wandklemme — die liegt in RE2
   woanders, und fuer den Port ist ohnehin die RE1.5-Kette aus §2 der Bezug.

Weitere gepruefte und gefallene Kandidaten stehen in §7.

---

## 6. Riegel

`re15_port/tests/unit/probes/r20_birkin_push.cmake` ->
`unit_r20_birkin_push_pin` (SKIP 77 ohne `shared_assets/RE2/CDEMD0.EMS`).

Gefahren wird der ECHTE Ablauf (`re15_game_step`, SCD-VM, RE2-Baenke EM036/EM037) von 40
Startplaetzen aus, je 200 Bilder = **7876 gemessene Bilder**. Jeder Startplatz ist begehbar
UND ausserhalb beider Boss-Segmente; die ersten acht sind die Stelle des Nutzers und ihre
Nachbarn an beiden Korridorwaenden, der Rest wird aus der lebenden Bossposition abgeleitet.

| Riegel | vorher | nachher |
|---|---|---|
| Abdeckung: Startplaetze / Bilder | 40 / 7876 | 40 / 7876 |
| Bilder ohne Koerper-Schub-Aufruf (@0x80031cbc) | **22** | **0** |
| Bilder auf einem unbegehbaren Punkt | 0 | 0 |
| groesster Ein-Bild-Weg des Spielers | **2112** | **1029** |
| duennste Band-0-Wand (Radius + Saum) | 2936 | 2936 |
| Bilder mit Treffer-Reaktion UND wirksamem Schub | **1** | **19** |
| Bilder mit falschem Positions-Spiegel (@0x8001d0d4-e4) | **7876** | **0** |
| Vorzeichen-Drehungen auf +-2*r | 0 | 0 |

**Gegenprobe**: mit ausgeschalteten Aufrufen (`if(0)`) fallen die Riegel
"kein Bild ohne Schub-Aufruf" (22 Bilder), "Schub wirkt in der Treffer-Reaktion" (1 Bild) und
— mit ausgeschaltetem Spiegel-Schreiber — "Positions-Spiegel" (7876 Abweichungen). Der Riegel
faellt also am alten Stand und ist kein nachgezogenes Gruen.

**Ehrlich zur Aussagekraft:** die Zeile "Bilder auf einem unbegehbaren Punkt" steht in diesem
Saatgut auf 0 *vor und nach* dem Fix — sie ist eine Wache fuer die Bedingung des Nutzers, aber
nicht die Zeile, die den Defekt unterscheidet. Das tun die beiden Mechanismus-Zeilen.

**Die Wahrheit fuer "begehbar" musste korrigiert werden.** Der Klemmpfad allein (die Definition
aus `probe_p2_floor_dump`) meldet die Stelle des Nutzers als begehbar, weil (11102,-19232) ein
FIXPUNKT der Klemme ist (§0.4). Die Sonde prueft deshalb zusaetzlich den Containment-Scan
`re15_collision_on_floor` (FUN_8003b7f0): liegt der Punkt IN einer soliden Band-Zelle, ist er
nicht begehbar. Erst damit meldet die Messung die Marke korrekt als `begehbar=NEIN`.
**Konsequenz ueber diesen Befund hinaus:** jede Standplatz-Zahl, die allein mit dem Klemmpfad
gewonnen wurde, ist an solchen Ueberlappungs-Fixpunkten zu optimistisch.

---

## 7. Gepruefte Kandidaten

| Kandidat | Ergebnis |
|---|---|
| (a) Springt der Schub in EINEM Bild ueber die Wand? | **JA, das ist der Defekt** — 2112 gemessen bei 2936 Wanddicke; mit stehengebliebenem Spiegel bis 23911. Ursache ist nicht der Schub, sondern seine 22-Bild-Unterdrueckung (§1.3) und der Spiegel (§3). |
| (b) Was tun 0x8003567c (a1=0x8000) und 0x80077600? | **Beides Abfragen, keine Klemme** (§5.2). Der Port braucht kein Gegenstueck; den Spiegel, den 0x8003567c nebenbei fuehrt, hat der Port jetzt ueber die RE1.5-Stelle @0x8001d0d4-e4. |
| (c) Klemmt der Port mit dem richtigen Bezugspunkt? | **Ja.** Das Original nimmt Spieler+0x40 (@0x8003b4ac), und das ist die Position vom Bildanfang (@0x8001d0d4-e4) — genau das `ox/oz`, das der Port hereingibt. Die im Auftrag genannten a2/a3 gehoeren zur Abfrage 0x8004c1bc, nicht zu einer Klemme. |
| (d) Greift `re15_collision_ensure_band(pl->y)` das richtige Band? | **Ja, Band 0.** Der Spieler steht im Protokoll durchgehend auf `y = 0`, und ROOM5090 traegt nur Band-0-Zellen im Kampfstreifen. Die rote Kiste im Bild ist Kulisse, keine Etage: `-(0/0x708) = 0`. |
| (e) Ist die Klemme gegated? | **Im Port nein, im RE1.5-Original nur ueber `bltz g_pauseflags` @0x80031c78 und den 0x4000-Latch @0x80031d4c-50.** Das im Auftrag genannte `+0x10E & 0x8000` @0x8002660c ist RE2-seitig und gated dort einen Abfrage-Block, nicht die Klemme. |
| Kippt `vor` (Segment-0-Mitte = x-2000) und springt damit die Mitte um 4000? | **Nein.** `s_g5.vor` wird beim Kampfstart eingefroren, der Boss startet westlich (@0x801011d0) — im Messlauf durchgehend +1. |
| Bewegt sich der Boss selbst sprunghaft? | **Nein.** Groesste Bossbewegung je Bild im Messlauf: 338. |
| Zwei-Zellen-Falle bei z=-19232 | **Vorhanden, aber Original-Verhalten** (§0.4). Die Klemme laesst den Spieler dort stehen — deshalb kam er nicht mehr heraus. |

---

## 8. Ehrlich offen

1. **Treppe (cmd 4 @0x80030660) und Klettern uebersprungen weiter den Koerper-Schub.** Fuers
   Klettern deckt der 0x4000-Latch die Klemme ab; fuer den Schub habe ich fuer beide keinen
   Beleg erarbeitet, dass er ausfallen darf. In ROOM5090 gibt es weder Treppe noch Leiter, der
   Befund des Nutzers ist davon nicht betroffen.
2. **Der per-Aktor-Spiegel (@0x8001d11c-54) ist nicht portiert.** Im Port liest ihn niemand
   (alle drei Leser — G5-Schub, Tentakel-Schub, `prop_contain` — nehmen den SPIELER-Spiegel).
   Sobald ein Gegner-Leser dazukommt, muss er nachgezogen werden.
3. **Ob der Vorzeichen-Dreh-Zweig in der Sitzung des Nutzers wirklich gefeuert hat, ist nicht
   bewiesen** — dazu braeuchte ich seinen `pos_s`-Stand im Moment F826. Bewiesen ist, dass er
   feuern KANN, sobald vorher irgendwo geklettert wurde, und dass der Port ihn vor diesem Fix
   mit einem stehengebliebenen Wert fuettert. Die 22-Bild-Unterdrueckung ist dagegen im
   Messlauf direkt reproduziert.
4. **Die RE2-seitige Wandklemme ist nicht lokalisiert.** Fuer diesen Befund war sie nicht
   noetig (der Port folgt der RE1.5-Kette), aber die Frage "wo klemmt RE2" bleibt unbeantwortet.
5. **Der Objekt-Pass FUN_8002bd44 (@0x8001ce14) laeuft im Port weiter nur im Normal-Zweig.**
   Im Original ist er ein eigener Top-Level-Aufruf neben dem Spieler-Dispatcher; ob und wie
   er je Kommandowort laeuft, ist nicht disassembliert. Ich habe ihn deshalb NICHT in den
   neuen Schwanz gezogen, obwohl das "plausibel" gewesen waere.
