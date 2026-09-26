# Spur A — der eigene Tentakel-Schub (ROOM5090, Marke `befund_5090_F962_marke1.bmp`)

Nutzer-Befund: *"Jetzt wurde ich von birkin rausgeschoben außerhalb des begehbaren
BEreiches"*, Marke bei `(11102, 0, -19232)`.

**Urteil in einem Satz:** Die Spur trägt als **belegte Reihenfolge-Divergenz** — der
Tentakel-Schub endet im Port ungeklemmt und setzt den Spieler messbar auf unbegehbare
Punkte (6 von 162 reinen Schüben; mit der Original-Reihenfolge 0 von 162). Sie trägt
**nicht** als Erklärung des Nutzer-Symptoms: der Schub erreicht die Klemm-Falle nicht
(Bedarf **1460**, größter gemessener Schub **1331**), und jede gemessene Ausreise wird im
Folgebild zurückgeholt. Reproduziert wird das Symptom `z = -19232` dagegen vom
**Körper-Schub aus dem Boss-Zylinder** — in **einem** lebenden Bild, danach 1200 Bilder
unverändert (§5.1) — und genau in diesen Zustand kann der ungeklemmte Tentakel-Schub den
Spieler setzen (3 bzw. 4 gemessene Bilder). Das ist die offene Verbindung.

Sonde: `re15_port/tests/unit/probe_r21_tentakel_schub.c` (Modi `lauf seg jagd raster opfer
zensus kriech druck bedarf kette kettefix`), Bauverzeichnis `re15_port/build_r21tent`.
Kein Engine-Code geändert (`git status` zeigt nur die Sonde + dieses Dossier).

---

## 1. Was der Port tut, und was das Original tut

### 1.1 Der Schub selbst

| | Port | Original |
|---|---|---|
| Code | `tent_kind_kollision`, `enemy_ai_tentakel_g5.c:595-687`, gerufen `:808` aus `tent_tick` | Kind-Entity `0x80104F64` + `FUN_80034D0C` |
| Segmente | 4, `r = {800, 600, 600, 600}` an part3 / part2 / part1 / Mitte(part1,part2) | Ctor `@0x80100674-c4` (seg0 r=800, seg1..3 r=600), Aufbau `@0x80104f94-5074`, Mitte `sll 16 / sra 17` `@0x80105024-38` |
| Ergebnisbit | `t->kontakt |= 6` | `+0x220 |= 6*r` `@0x80105078-a4` |
| Spieler-Zylinder | r 450, Höhe 1530, ein Segment | `li v1,0x1c2` `@0x8003bdc0` → `@0x8003bdc4-d0`; Höhe `+0x9E` `@0x8003bdec`; `+0x1E8=1` `@0x8003bddc` |
| Schub | `pl->x += px; pl->z += pz;` (`enemy_ai_tentakel_g5.c:685`) | radialer Push-out `@0x80034e7c-eb4` / `@0x80034ec0-f00` |
| **Wandklemme dahinter** | **keine** | **`jal 0x8003b0a4` `@0x80031d70`** (im Spieler-FSM desselben Bildes) |

### 1.2 Die Reihenfolge im Original — selbst disassembliert

```
; PSX.EXE, In-Game-Zweig (erreicht über  8001cd08: bne v0,zero,0x8001cdec)
  8001cdec: jal 0x8003f038
  8001cdf4: jal 0x8004f090
  8001cdfc: jal 0x8001500c
  8001ce04: jal 0x8001a50c     <-- ENTITAETEN-Schleife  (Tentakel-Kind-Kollision)
  8001ce0c: jal 0x80031c44     <-- SPIELER-FSM
  8001ce14: jal 0x8002bd44         Objekt-Tick
  8001ce1c: jal 0x800436a8         AUTO-Scan
  ...
  8001ce68: beq v0,zero,0x8001d088   (Sprung in denselben Durchlauf, weiter unten)
  ...
  8001d0b4: lhu v1,-13688(v1)    v1 = player+0x34   (LIVE X, 0x800aca88)
  8001d0d4: sh  v1,-13676(at)    player+0x40 = SPIEGEL X (0x800aca94)
  8001d0dc: sh  a0,-13674(at)    +0x42
  8001d0e4: sh  a1,-13672(at)    +0x44
```

Die Entitäten-Schleife dispatcht per Typ-Tabelle:
```
  8001a570: lbu v0,8(v1)        Entity-Typ
  8001a57c: addu v0,v0,s3       s3 = 0x80072bac
  8001a588: jalr v0
```

Der Spieler-FSM:
```
  80031c44: addiu sp,sp,-24
  80031c78: bltz a0,0x80031da8      a0 = g_pauseflags
  80031cb4: jalr v0                 Kommando-Handler (Tisch 0x80073f90)
  80031cbc: jal 0x8002b544          KOERPER-Schub
  80031d68: addiu a0,s0,52          a0 = player+0x34
  80031d70: jal 0x8003b0a4          WANDKLEMME
```

Und die Wandklemme liest ihre **Vorposition** aus dem Spiegel, nicht aus einem Argument:
```
  8003b484: lw    a2,-14460(a2)     a2 = g_entity(cur) = Spieler
  8003b48c: addiu a1,a2,52          a1 = Entity+0x34  (LIVE-Position)
  8003b4a0: lw    v0,0(at)          Handler-Tisch 0x800b2858[Zelltyp]
  8003b4a8: jalr  v0
  8003b4ac: addiu a2,a2,64          a2 = Entity+0x40  (POSITIONS-SPIEGEL = prev)
```

`DAT_800aca94` hat genau **einen** Pro-Bild-Schreiber: `@0x8001d0d4`. Die übrigen neun
Schreibstellen sind Raumwechsel (`@0x8001d6f0`, `@0x8001d898`), das Kletter-FSM
(`@0x80038210`, `@0x800382c8`, `@0x80038480`, `@0x800385f0`, `@0x80038628` — im Port
`climb_common.c` mit denselben Adressen) und `@0x8003904c`/`@0x8003909c`.

### 1.3 Die Reihenfolge im Port

| Original | Port |
|---|---|
| `8001ce04` Entitäten | `game_step_common.c:2096` `re15_enemy_ai_run_all` |
| `8001ce0c` Spieler-FSM | `:1483` `re15_player_tick` |
| `80031cbc` Körper-Schub | `:1488` `re15_body_push_player` |
| `80031d70` Wandklemme | `:1495` `re15_collision_constrain(rdt, ox, oz, …)`, `ox/oz` aus `:1481` |
| `8001d0b4-d0e4` Spiegel | `:940` `pl->pos_s_x/y/z = pl->x/y/z` |

**Zwei Divergenzen, nicht eine:**

**(D1) Die Entitäten laufen am Step-ENDE, nicht am Anfang.** Damit endet das Bild mit einer
Spielerposition, die keine Wandklemme gesehen hat. Hinter `:2096` steht keine Klemme; die
einzige dort ist an `grabbed_branch && re15_player_is_grabbed()` gebunden (`:2168`).

**(D2) Der Positions-Spiegel ist verseucht.** Im Original ist `prev` (Entity+0x40,
`@0x8003b4ac`) die Lage am Bild**ende** *nach* der Wandklemme (`@0x8001d0d4` steht hinter
`@0x8001ce0c`) — also **immer wandaufgelöst**, und die Entitäten-Schübe des *laufenden*
Bildes sind darin bewusst nicht enthalten. Im Port ist `ox/oz` (`:1481`) und der Spiegel
(`:940`) die Lage *nach* dem Entitäten-Durchgang des Vor-Steps — also möglicherweise
mitten in einer Wand.

Das ist nicht kosmetisch, weil `push_rect` (`re15_collision.c:53`, `FUN_8003bca8`) genau
diese Vorposition als Riegel benutzt:

```c
    /* code == 0: Schubrichtung stimmt auf BEIDEN Achsen mit der Bewegung ueberein */
    if (*lx == prevx && *lz == prevz) { /* min-Achse: losmachen */ }
    *lx = prevx; *lz = prevz;      /* sonst: Ruecksprung auf die VORPOSITION */
```

Steckt `prev` in der Wand, hält dieser Riegel den Spieler **in** der Wand fest.

---

## 2. Die Geometrie der Falle — nachgerechnet und gemessen

Band-0-Zellen an der Marke (`probe_r20_birkin_push zellen`; jede Zelle liegt fünffach vor,
#23/#55/#87/#119/#151 bzw. #28/#60/#92/#124/#156):

```
  #23  typ=1 u0=255 band=0   x  -5673.. 28057   z -19870..-16864
  #28  typ=1 u0=255 band=0   x   4050.. 20225   z -21700..-19700
```

Begehbar in der Marken-Spalte `x=11102`: `z -24600 … -22200`. Darüber nichts bis
`z -16400`.

Handrechnung mit `push_rect` (r=450, Saum `0x12`=18) für den Punkt `(11102,-19232)`:

* `#23`: `penZ_süd = (-19870-450+19232)-18 = -1106`, `penZ_nord = 2836`, `penX = -17243`
  → min-Achse Z → `z = -19232-1106 = -20338`
* `#28` (kommt später in der Liste, `prev` bleibt `(11102,-19232)`):
  `penZ_nord = (-19700+450+20338)+18 = +1106`, `penZ_süd = -1830`, `penX = -7520`;
  `code = 3` (beide Achsen gegen die Bewegung) → min-Achse Z → `z = -20338+1106 = -19232`

→ **Fixpunkt.** Das bestätigt Fakt 4 der Aufgabe unabhängig; kein `break` in
`FUN_8003b0a4` (`@0x8003b510-20`), die letzte treffende Zelle gewinnt.

### 2.1 Was ein ungeklemmter Schub leisten müsste (`bedarf`)

Je x-Spalte: der kleinste **+z**-Schub ab dem nördlichsten begehbaren Punkt, nach dem die
Klemme des **Folgebildes** (`ox/oz` = die Außenlage selbst) auf einem unbegehbaren Punkt
landet.

| x | Nordrand z0 | Bedarf | Endlage P1 | Klemme des Folgebildes |
|---|---|---|---|---|
| 5000 | -22160 | 1420 | -20740 | (4092, **-21325**) |
| 5500 … 14000 (alle 500) | -22160 | **1460** | -20700 | (x, **-19232**) |

**Der Bedarf ist 1460, und das Ergebnis ist für jedes x von 5500 bis 14000 exakt
`z = -19232`** — die z-Koordinate der Nutzer-Marke. Ein einziger ungeklemmter +z-Schub von
1460 ab dem Korridor-Nordrand genügt, um dauerhaft auf dem Fixpunkt zu parken.

---

## 3. Messungen am Tentakel-Schub

Alle Zahlen aus `probe_r21_tentakel_schub` auf `ROOM5090.RDT` + RE2-Bänke EM036/EM037,
echter `re15_game_step`, RNG-Saat `0x0badf00d`.

### 3.1 Zensus über Posen × Standplätze (`zensus`)

1276 begehbare Rasterpunkte (150er-Raster), 30000 Bilder; je Bild wird der Spieler auf den
Rasterpunkt gesetzt, der einer rotierenden Armspitze am nächsten liegt.

| Größe | Wert |
|---|---|
| gemessene Bilder | 479 (29521 als Griff/Maul übersprungen) |
| Bilder mit Tentakel-Kontakt | 169 |
| Bilder mit Körper-Schub-Wirkung | 108 |
| **reine Tentakel-Schübe** | **162** |
| größter reiner Tentakel-Schub (Manhattan) | **1331** — (14200,-22250) → (14979,-22802) |
| **Endlage unbegehbar, PORT** (Klemme lief davor) | **6 von 162** |
| **Endlage unbegehbar, ORIGINAL-Reihenfolge** (Klemme dahinter) | **0 von 162** |
| kleinster Weg, der port-seitig hinausführt | 305 |
| von den 6 Außenlagen: Folgebild-Klemme holt zurück | **6 von 6** (0 festgefahren) |
| Bilder mit Vorzeichen-Drehung | 34; größter Weg darin 1263; davon 4 unbegehbar |

Die 6 Außenlagen im Protokoll (Auszug):
```
 RAUS arm0 ( 13600,-24650)->( 13975,-25068) weg= 793  | gleiche Klemme -> ( 13975,-24732) begehbar
 RAUS arm0 ( 14200,-24650)->( 14031,-24786) weg= 305  | gleiche Klemme -> ( 14031,-24732) begehbar
 RAUS arm0 ( 14200,-24650)->( 13497,-24967) weg=1020  | gleiche Klemme -> ( 13497,-24732) begehbar
 RAUS arm2 ( 14200,-24650)->( 13984,-25697) weg=1263  | gleiche Klemme -> ( 13984,-24732) begehbar
 RAUS arm0 ( 14200,-24650)->( 13517,-24988) weg=1021  | gleiche Klemme -> ( 13517,-24732) begehbar
 RAUS arm2 ( 14200,-24650)->( 14086,-24853) weg= 317  | gleiche Klemme -> ( 14086,-24732) begehbar
```

⛔ **Einschränkung, die ich nicht verstecke:** 29521 von 30000 Bildern fielen aus der
Stichprobe, weil der Spieler unmittelbar an der Armspitze fast dauernd gegriffen wird oder
die Opfermaschine läuft. Die 162 reinen Schübe sind also eine kleine Stichprobe; jede
Aussage „kommt nie über X" gilt nur für sie.

### 3.2 Der echte Kampf, laufender Spieler (`kette`) — mit A/B

Startplätze am Korridor-Nordrand, jeder begehbar **und** außerhalb beider Boss-Segmente
(Vorschrift aus `probe_r20_birkin_push`; Radien gemessen: seg0 **r=6000**, seg1 **r=1300**).
Der Spieler hält VORWÄRTS gegen die Nordwand (Yaw 3072 = +z, geeicht: dz=802 in 12 Bildern).

| | `kette` (Port-Reihenfolge) | `kettefix` (Klemme hinter dem Schub) |
|---|---|---|
| gültige Starts | 6 | 5 |
| **Starts, die ein Bild außerhalb endeten** | **1** | **0** |
| Ursache des ersten Außen-Bildes | nur Tentakel 1, nur Körper 0 | — |
| Bilder, in denen ein reiner Tentakel-Schub **im Boss-Zylinder** endete | **3** | **4** |
| Nachlauf: weitere Außen-Bilder / Bilder auf `z=-19232` | 1 / **0** | 0 / 0 |

Die Spur des Außen-Bildes (`B` = Körper-Schub wirkte, `T` = Tentakel-Kontakt):
```
  ( 13000,-22168)->( 12902,-22216) d=(  -98,  -48) B=0 T=1  begehbar
  ( 12902,-22216)->( 12731,-22203) d=( -171,  +13) B=0 T=1  begehbar
  ( 12731,-22203)->( 12620,-22164) d=( -111,  +39) B=0 T=1  begehbar
  ( 12620,-22164)->( 12548,-22152) d=(  -72,  +12) B=0 T=1  begehbar
* ( 12548,-22152)->( 12501,-22149) d=(  -47,   +3) B=0 T=1  RAUS
  n01 ( 12501,-22149)->( 12475,-22153)               B=0 T=1  begehbar   <- schon zurueck
```

In einem früheren Lauf derselben Sonde (ohne Nachlauf, der die Kampf-Zeitachse ändert)
gingen **3 von 6** Starts hinaus, alle mit `B=0 T=1`, u.a. in **einem** Bild:
```
* ( 13250,-22200)->( 13132,-21671) d=( -118, +529) B=0 T=1  RAUS
* ( 13500,-22200)->( 13617,-21814) d=( +117, +386) B=0 T=1  RAUS
```
`z=-21671` liegt **innerhalb** von Zelle #28 (ab -21700) — der Schub hat den Spieler durch
die Südfläche der Wand gesetzt, und das Bild endete dort.

### 3.3 Gegenproben, die nichts fanden

| Lauf | Bilder | Außen-Bilder |
|---|---|---|
| `lauf` (Nutzer-Position (9000,-23550), stehend, 900 Bilder) | 900 | **0** (größter Ein-Bild-Weg 1029, davon der größte mit Tentakel-Kontakt) |
| `opfer` (Spieler in der Boss-Bahn, 4000 Bilder) | 4000 | **0** (42 Kontakt-Bilder, größter Weg 965) |
| `kriech` (19 Startplätze am Nordrand, stehend, je 1500 Bilder) | 1094 gemessen | **0** (größter Ein-Bild-Weg nach der Klemme 1279) |
| `jagd` (169 Rasterpunkte, 30000 Bilder) | 475 | **0 von 6** Kontakt-Bildern |
| `raster` (4566 Standplätze, eine Pose) | 4566 Standplätze | **0 von 2** |

---

## 4. Die vier Teilfragen der Aufgabe

### 4.1 Original-Funktion, Radien, Segmentlagen, Aufrufort, Klemme dahinter
Siehe §1.1/§1.2. **Eine Wandklemme steht hinter dem Schub nur im Original** — und zwar
nicht im Tentakel-Code, sondern im Spieler-FSM desselben Bildes (`jal 0x8003b0a4`
`@0x80031d70`), weil die Entitäten-Schleife `@0x8001ce04` **vor** dem Spieler-FSM
`@0x8001ce0c` läuft. Im Port steht nichts dahinter.

### 4.2 Größter Ein-Bild-Weg, verglichen mit der Fallen-Schwelle
* größter reiner Tentakel-Schub: **1331** (Manhattan; der reine +z-Anteil ist kleiner)
* größter Weg in einem Dreh-Bild: **1263**
* größter Ein-Bild-Weg nach der Klemme im laufenden Kampf: **1279**
* r20-Schwelle **1250** gilt für den **Körper**-Schub *mit* Klemme im selben Bild —
  1331 liegt darüber.
* Die für **diesen** Befund maßgebliche Schwelle ist die des **ungeklemmten** Schubs:
  **1460** (§2.1). **Darüber kommt kein gemessener Tentakel-Schub.**
* Rechnerische Obergrenze eines Segments: ohne Drehung `|p| ≤ over ≤ r+450 = 1250`;
  **mit** Drehung `|p| ≤ 2r + |g| ≤ 1600 + 737 = 2337` (`@0x80034f98-ac` /
  `@0x8003503c-44`, größtes gemessenes `over` = 737). Die 1460 sind damit **nicht
  ausgeschlossen**, nur nicht beobachtet.

### 4.3 Y-Lage und Höhentest
Die Arm-**Entities** stehen auf y −715 … −2458 (Ankertabelle `@0x80105668` + Boss-y), der
Höhentest benutzt aber das **Segment**-Welt-y. Gemessen über 900 Bilder × 4 Arme × 4
Segmente (14400 Stichproben, Modus `seg`):

* Segment-Welt-y **≥ 0** (Boden oder darunter): **981**; **< 0**: 13419
* Kreis-Kontakte mit bestandener Höhenprüfung: **154**; an der Höhe gescheitert: **374**
* größtes `over`: **737**
* Segment-x-Spanne über alle Bilder: **7306 … 22742** — das begehbare Band 0 liegt bei
  x 3400…14000, die Arme fegen also weit über den Boden hinaus
* Bone-Translationen (EMD 0x37, 4 Bones): max `|trans.x|` = 0 / 3750 / 6753 / 8261

Das Band stimmt: `hs = Segmenthöhe 0 + Spielerhöhe 1530` (`+0x9E` `@0x8003bdec`); die
Tentakel-Segmente tragen im Ctor `@0x80100674-c4` nur Radien, keine Höhe. Der Höhentest
lässt die Arme also erreichen — mit `pl->y = 0` genau für Segment-y ∈ (−1530, +1530).

### 4.4 Runde-18-Altlast (`RE15_PL_SEG_LOKAL_Y = -1530`, `@0x8003bde0`/`@0x8003bde4`)
Im Tentakel-Schub reduziert sich die Bedingung mit `pl->y = pos_s_y = 0` auf

```
  h2      = pos_s_y + (-1530) - seg_y   ->  feuert  <=>  seg_y >= 0      (oder seg_y <= -3060)
  h2_alt  = pos_s_y +     0   - seg_y   ->  feuert  <=>  |seg_y| >= 1530 — und das schliesst
                                             das Hoehen-Gate (-1530 < -seg_y < 1530) aus
```

also: **mit −1530 lebt der Zweig, mit 0 war er beweisbar tot.** Gemessen (`zensus`):

| | Drehungen |
|---|---|
| mit `-1530` (jetziger Stand) | **35** |
| mit `0` (Stand vor Runde 18) | **0** |
| Höhenprüfung überhaupt erreicht | 238 |

Wirkung auf den Weg: 34 Bilder mit Drehung, größter Weg darin **1263** (gegenüber 1331
insgesamt), 4 davon endeten unbegehbar. In den Läufen `lauf`, `seg` und `opfer` feuerte er
0-mal. Der Zweig ist damit die **einzige** Stelle dieses Schubs, die über `over`
hinauskommen kann — und die 2337er Obergenze stammt allein von ihm.

---

## 5. Was den Befund tatsächlich erklärt — die offene Verbindung

`probe_r20_birkin_push repro` misst den **Körper**-Schub von Standplätzen aus, die
**innerhalb** des Boss-Zylinders liegen. Ergebnis (Diagnose-Abschnitt A derselben Sonde):

```
  (  1500,-22400) -> (  5379,-19881) d=( 3879, 2519) -> (  5379,-19232) aktiv  UNBEGEHBAR
  (  1750,-22150) -> (  5273,-19690) d=( 3523, 2460) -> (  5273,-19232) aktiv  UNBEGEHBAR
  (  2000,-21900) -> (  5178,-19564) d=( 3178, 2336) -> (  5178,-19232) aktiv  UNBEGEHBAR
  ... 290 Standplaetze mit Boss-Kontakt, 77 landen nach Schub+Klemme UNBEGEHBAR
  groesster Sprung: (1000,-22900) -> (5778,-19232)  Manhattan 8446
  groesster Ein-Bild-Schub im Raster: 8760
```

**Die Endlage ist `z = -19232` — die Nutzer-Marke.** Runde 20 hat diese Zahlen
ausdrücklich als *„kein Riegel — gesetzte Tiefpositionen … ein Zustand, in den das laufende
Spiel den Spieler nicht bringt (der Schub läuft ja jedes Bild)"* verworfen.

Diese Annahme ist **nicht dicht**: gemessen (`kette`, Spalte „im Boss-Zylinder") endete ein
**reiner** Tentakel-Schub (B=0, T=1) in **3 bzw. 4 Bildern** innerhalb eines
Boss-Segments — weil er ungeklemmt ist und im Port bis zum nächsten Bild stehen bleibt.
Der Körper-Schub des Folgebildes setzt dann bei tiefer Durchdringung bis zu 8760 Einheiten
in einem Bild ab, und die Klemme desselben Bildes landet auf dem Fixpunkt.

### 5.1 Das Symptom des Nutzers, im LEBENDEN `re15_game_step` reproduziert

Modus `druck` (Spieler hält VORWÄRTS gegen die Nordwand). Die Startplätze dieses Modus
sind **nicht** gegen den Boss-Zylinder gefiltert — der erste Start lag darin. Was das
Spiel selbst daraus macht, in **einem** Bild:

```
  RAUS x 5500 f  0 (  5500,-22600)->(  9083,-19232) serie=1     <- EIN Bild, d=(+3583,+3368)
  RAUS x 5500 f  1 (  9083,-19232)->(  9334,-19232) serie=2
  ...
  RAUS x 5500 f  7 (  9555,-19232)->(  9555,-19232) serie=8
  ... unveraendert bis serie=1200 (Ende des Laufs)

  D2) gemessene Bilder 14133 (Griff/Maul 6267)
  D2) Bilder UNBEGEHBAR: 2585 ; laengste Serie: 1200
  D2) noerdlichste Lage: z=-19232 (x=9083) ; Bilder auf dem Fixpunkt -19232: 2225
```

Das ist **exakt der Nutzer-Befund**: `z = -19232`, unbegehbar, und der Spieler kommt nie
wieder los (1200 Bilder Serie; der Nutzer sah F916..F962 unverändert). Die Vorbedingung
ist „Spieler im Boss-Zylinder", der Beweger der Körper-Schub (`@0x80031cbc`) mit der
Klemme desselben Bildes (`@0x80031d70`) — **nicht** der Tentakel-Schub. Beim Nutzer klebt
zusätzlich das x noch wenige Bilder weiter (9083 → 9334 → 9451 → 9507 → 9533 → 9544 →
9555), also derselbe abklingende Verlauf wie seine Zeilen F886 (10615) → F901 (11102) →
F916 unverändert.

⛔ Diese Kette ist **plausibel und in ihren Einzelgliedern gemessen, aber als Ganzes nicht
end-to-end reproduziert**: in keinem meiner Läufe ist ein Tentakel-Schub in den Zylinder
gegangen und im Folgebild auf `-19232` gelandet. Das gehört als nächster Schritt gemessen
(und nicht von mir behauptet). Bemerkenswert: `kettefix` (Klemme hinter dem Schub) senkt
die Außen-Bilder auf 0, **verhindert die Zylinder-Landungen aber nicht** (4 statt 3) — die
Wandklemme schiebt nicht aus Gegner-Zylindern heraus. Das entscheidet die Wahl des Fixes.

---

## 6. Fix-Vorschlag (umsetzungsreif, in diesem Lauf NICHT umgesetzt)

**F1 — Entitäten-Durchgang vor den Spieler-Schritt.**
`re15_enemy_ai_run_all` (`game_step_common.c:2096`) gehört **vor** `re15_player_tick`
(`:1483`), byte-true nach `8001ce04: jal 0x8001a50c` vor `8001ce0c: jal 0x80031c44`. Dann
laufen hinter jedem Tentakel-Schub im **selben** Bild der Körper-Schub
(`80031cbc: jal 0x8002b544`) und die Wandklemme (`80031d70: jal 0x8003b0a4`) — das räumt
beide Glieder der Kette ab: die Wand-Durchdringung *und* die Zylinder-Landung.
⚠️ Das ist der Eingriff mit Folgen: die jetzige Platzierung ist an mehreren Stellen
begründet (`:2098-2103` Objekt-Ausschiebung, `:2155` Trefferfilter, `:2185` Griff-Schub).
Diese Nachbarn müssen mitwandern bzw. neu begründet werden — das Original hat für sie
eigene Plätze (`8001ce14` Objekt-Tick, `8001ce1c` AUTO-Scan).

**F2 — Positions-Spiegel ans Step-ENDE.**
`pl->pos_s_x/y/z = pl->x/y/z` (`:940`) gehört hinter die Wandklemme und hinter den
Entitäten-Durchgang, byte-true nach `8001d0b4-d0e4` (das **hinter** `8001ce0c` im selben
Durchlauf steht). Dann ist `prev` in `push_rect` — im Original Entity+0x40,
`addiu a2,a2,64` `@0x8003b4ac` — **immer** eine wandaufgelöste Lage, und der
`code == 0`-Riegel (`*lx = prevx; *lz = prevz`) kann den Spieler nicht in einer Zelle
parken. Dasselbe gilt für `ox/oz` (`:1481`), die im Port als `old_x/old_z` in
`re15_collision_constrain` gehen: sie müssen aus dem Spiegel kommen, nicht aus der
Live-Position nach dem Entitäten-Durchgang.

**Nicht empfohlen: die Minimal-Variante.** Nur eine zweite Wandklemme hinter `:2096`
anzuhängen (das, was `kettefix` simuliert) senkt die Außen-Bilder messbar auf 0
(1/6 → 0/5), lässt aber die Zylinder-Landungen stehen (3 → 4) **und** ruft
`FUN_8003b0a4` zweimal pro Bild — im Original läuft es genau einmal (`@0x80031d70`). Das
wäre eine neue Divergenz, keine Reparatur.

**Riegel, der dazugehört** (`probes/r21_tentakel-schub.cmake`, Modus `zensus`):
„reine Tentakel-Schübe mit unbegehbarer Endlage = 0" — aktueller Stand **6 von 162**;
dazu „`kette`: Starts, die außerhalb endeten = 0" — aktueller Stand **1 von 6**.

---

## 7. Was widerlegt ist

* **„Der Tentakel-Schub schiebt den Spieler auf `(11102,-19232)`."** Widerlegt: Bedarf
  **1460**, größter gemessener Schub **1331**; in 900 (`lauf`) + 4000 (`opfer`) + 1094
  (`kriech`) + 475 (`jagd`) + 479 (`zensus`) gemessenen Bildern **kein einziges** Bild auf
  dem Fixpunkt. Der Fixpunkt *wird* erreicht (2225 Bilder, §5.1) — vom **Körper**-Schub
  aus dem Boss-Zylinder.
* **„Der Schub ist harmlos, weil die Klemme ihn ohnehin auffängt."** Widerlegt: **6 von
  162** reinen Schüben enden auf einem unbegehbaren Punkt und werden so **gerendert**; mit
  der Original-Reihenfolge sind es **0 von 162**.
* **„Der Dreh-Zweig ist im Tentakel-Pfad ohne Wirkung."** Widerlegt: **35** Drehungen mit
  `-1530` gegen **0** mit `0`; größter Weg darin 1263, 4 unbegehbare Endlagen.
* **„Runde 20 hat den Körper-Schub abschließend erledigt."** Eingeschränkt: die dortige
  Vorschrift „Startplatz außerhalb beider Segmente" setzt voraus, dass das Spiel den
  Spieler nie in den Zylinder bringt. Gemessen: der ungeklemmte Tentakel-Schub tut es
  (3 bzw. 4 Bilder).

## 8. Offen

1. Die Kette §5 end-to-end messen: Tentakel-Schub in den Zylinder → Körper-Schub des
   Folgebildes → Fixpunkt. Dafür braucht die Sonde einen Lauf, der genau auf dieses
   Bildpaar filtert und danach 200 Bilder weiterläuft.
2. Die Stichprobe: 29521 von 30000 Zensus-Bildern fielen als Griff/Maul aus. Ohne eine
   Führung, die den Spieler in Kontakt **ohne** Griff hält, bleibt „kommt nie über 1331"
   eine Aussage über 162 Schübe, nicht über den Schub.
3. Die vier weiteren Kandidaten des Prüfurteils (Treppe, Klettern, Text-Freeze
   `RE15_PAUSE_PLAYER`, die 360 Bilder Stillstand F451..F811) sind hier nicht berührt.

---

## 9. Nachtrag 2026-09-26 — offener Punkt 8.1 gemessen, die Kette faellt

Neuer Sondenmodus `verbund` (`probe_r21_tentakel_schub verbund`). Er faehrt dieselbe
Vorschrift wie `kette` (Start AUSSERHALB beider Boss-Segmente), aber mit breiterer
Fuehrung — 4 Tastenbilder (VOR, VOR+LINKS, VOR+RECHTS, nichts) x 4 Blickrichtungen je
Startspalte, je 600 Bilder — und verfolgt JEDEN Eintritt in einen Boss-Zylinder ueber die
naechsten 12 Bilder.

```
  VB) verworfene Spalten: 0 ohne begehbare Zelle,
                          32 weil der Boss 400 Bilder lang keinen freien Platz liess
  VB) Laeufe 16 (Startspalten verworfen 32) ; gemessene Bilder 2584 (Griff/Maul 7016)
  VB) Bilder UNBEGEHBAR: 0 ; laengste Serie: 0
  VB) Bilder EXAKT auf dem Klemm-Fixpunkt z=-19232 und unbegehbar: 0
  VB) EINTRITTE in einen Boss-Zylinder: 43 (nur Tentakel 0, nur Koerper 40, beides 0, keins 3)
  VB) davon binnen 12 Bildern UNBEGEHBAR: 0 ; davon auf dem Fixpunkt: 0
```

**Das erste Glied der Kette aus §5 tritt hier NICHT auf.** Von 43 Eintritten in einen
Boss-Zylinder geht **kein einziger** auf einen reinen Tentakel-Schub zurueck (40 auf den
Koerper-Schub selbst, 3 auf keinen von beiden). `kette` hatte 3 solche Bilder gesehen —
bei EINEM Tastenbild und EINER Blickrichtung. Und in keinem der 2584 Bilder verlaesst der
Spieler den begehbaren Bereich ueberhaupt.

Damit ist die in §5 vorgeschlagene Kette nicht bestaetigt, sondern an ihrem ersten Glied
bestritten — und der Fix-Vorschlag **F1 (Entitaeten-Durchgang vor den Spieler-Schritt)
bleibt deshalb ungebaut.** Er waere mit dieser Messlage auf eine Vermutung gebaut.

**⛔ Die Stichprobe ist weiter duenn und das ist keine Entwarnung:** 32 von 33
Startspalten fielen aus, weil der Boss 400 Bilder lang keinen freien Platz liess; gemessen
wurde faktisch eine Spalte (x=12000). 7016 von 9600 Bildern fielen als Griff/Maul aus. Der
erste Lauf dieses Modus war sogar ein reiner Fehllauf (17 von 17 Spalten verworfen, 0
Bilder) — und die Schiene meldete dazu zunaechst woertlich „die Kette ist widerlegt". Diese
Zeile ist berichtigt: 0 Laeufe heissen jetzt ausdruecklich „sagt NICHTS".

**Zwei Bausteine selbst nachgeprueft** (statt der Runde-20-Behauptung zu vertrauen):

* Die Wandklemme hat wirklich **kein** `break`. Der Schleifenfuss ist
  `@0x8003b518 bne v0,zero,0x8003b224` hinter `@0x8003b510 addiu s4,s4,12`; der Treffer
  setzt nur ein Sammelbit (`@0x8003b500 ori s7,s7,0x1`). Die letzte treffende Zelle
  gewinnt — der Fixpunkt zwischen zwei ueberlappenden Zellen ist Original-Verhalten.
* Der Boss-Zylinder hat wirklich **r = 6000**. Im Overlay
  `build/extracted/re2_ems/CDEMD0_EM36_ai1.BIN`:
  `@0x8010053C 24021770` = `addiu v0,zero,0x1770` (6000),
  `@0x80100540 a602009a` = `sh v0,154(s0)` (Seg0 +0x16).

**Was daraus folgt.** Der Spieler kann die Falle nur betreten, wenn ihn etwas dorthin
SETZT; hinlaufen kann er nicht, weil die Klemme ihn vorher haelt. Der Koerper-Schub ist
stark genug (bis 8760 in einem Bild), aber in 2584 legalen Bildern hat er es nicht getan.
Die naechsten Wege sind deshalb die vier bislang unberuehrten Kandidaten des Pruefurteils
— Treppe, Klettern, der Text-Freeze `RE15_PAUSE_PLAYER` und die 360 Bilder Stillstand
F451..F811 im Protokoll des Nutzers — und das Protokoll selbst: die Zeilen F826 (9044,
-24943) -> F841 (11436,-22142) -> F856 (9688,-19232) sehen nach LAUFEN aus (je 15 Bilder
Abstand, rund 190 Einheiten je Bild), nicht nach einem Schub.
