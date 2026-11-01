# Spur S (spinnengore), Abnahme 4 (Runde 41, 2026-11-02)

Ich habe als unabhängiger Abnehmer geprüft und dem Bau nichts geglaubt. Geprüft wurde der Zweig `r41/spinnengore` mit Kopf
`874183557` (Nachbesserung 3 auf `8430ceac7`).

- Bau selbst über `local_build.sh configure` und `local_build.sh build`: LOCAL-BUILD-OK, `ninja: no work to do`. Die exe
  von 2026-11-01 23:26 entspricht also dem Kopf (md5 `34c7dd1640ce7c8986231477248b1938`).
- Alle Fensterläufe liefen über **eine** eigene Messkopie `re15_pc_S41abn4.exe`, einmal vom Kopf-Bau kopiert (md5 gleich).
  Beschleunigter Renderer, weder `AUTOSHOT` noch `SOFTWARE_RENDER` noch `TITLE_SHOT`. Titel und Spielerwahl liefen über
  die echten Menüs (`RE15_TITLE_CONFIRM_MS=1500`, `RE15_PSELECT_AUTO`, für Elza `_SWITCH`), danach `RE15_DEBUG_JUMP`.
  Aufnahme per gdigrab am Fenster-Handle der eigenen PID, 10 Bilder/s. Beendet habe ich nur die eigene PID.
- Eigene Feuerskripte mit anderem Takt als Bauer und Abnahme 3: Spinne `W0.2,M0.6,(MA0.1,M0.7)×11,W12` ab Spielbild 12,
  Krähe Redhawk `W2.6,M0.5,(MA0.1,M0.8)×6,W6`, Krähe Pistole w3 `W2.4,M0.4,(MA0.1,M0.5)×14,W5`.
- Eigene Werkzeuge in `scratchpad/S41abn4/`: `cap.ps1` (Aufnahme), `a4dis.py` und `a4cmp.py` (capstone, unabhängig vom
  Skill-Disassembler und von `re2_disasm.py`), `ana4.py` (Spinnen-, Lachen-, FX- und Flecken-Log), `feder4.py`
  (Federlog, Versatz selbst nachgerechnet), `gruen4.py` (Grünanteil je Bild), `sheet4.py`, `gegen4.py` (Gegenproben).
- Belege: `S_belege/abn4/d01…d13` (Bilder d01–d10, Logauszug d11, Bytes d12, Tests und Gegenproben d13).

## Urteil: BESTANDEN

Die drei Mängel der Abnahme 3 sind behoben:

1. **Krähenfedern.** Im echten Fenster platzt die Krähe beim Tod, mit Leon und mit Elza. Dabei entstehen 11 Federn
   (Soll 8..11). Ein Pistolentreffer im Flug erzeugt 2 Federn (Soll 2..3), der Sturz danach 2 weitere. Alle Federn
   entstehen am Körper der Krähe, im Bereich ±512 und als s8·4 um ihre Lage, wie FUN_80104678. Sie pendeln und sinken.
   Bild und Ops sind gegen die RE2-Bytes geprüft. Das Dossier ist an allen drei Stellen berichtigt.
2. **Riegel für den Babyfleck.** Der Zeichenaufruf in `main.c` und das Leeren beim Raumwechsel hängen jetzt an Riegeln.
   Baue ich sie aus, wird BT rot („gezeichnet 0“), ebenso M7b und M7c.
3. **Adresszitat.** Die 50 steht @0x8001649c, @0x80016350 und @0x800169a4. Der Header zitiert jetzt diese drei Stellen.

Der Kern hält im echten Fenster:

- Leon in ROOM2070 und Elza in ROOM2071: Unter der Standard-KI platzt die Spinne bei jedem Magnum-Treffer, mit grünem Blut.
- Darunter wächst eine grüne Lache, 90 Schritte bis 2020 x 2120.
- `RE15_KEIN_80PROZENT_GORE=1` schaltet das Platzen ab.

Tests:

- 326/326 Unit-Tests (Zielauswahl).
- 14/14 Integrationstests, einzeln nacheinander.

Es bleiben keine Mängel, nur Hinweise (unten). Zwei davon betreffen die Offen-Liste für die Folgespur.

## 1. Eigene Läufe im echten Fenster

| Lauf | Einstellung | Log (eigene Auswerter) | Bild |
|---|---|---|---|
| a Leon, ROOM2070, Redhawk w7 | Standard-KI | Slot 2 F36: st 1→3, s=5.1.0, HP 110→-20, +0x93 = 0x41, **ta=1 sh=80001 fl=7fff8**. Slot 1 F108 ebenso. Grüne Sub-8-Partikel (CLUT 0x7991) ab F36, 110 Zeichnungen. **Lache Slot 2: F48 1308 x 1408 `bf10bf` → 2020 x 2120**, 90 Schritte ab 1300, sichtbar 513/513 | Bild 270 ganz, **Bild 272 platzt** in gelbgrüne Scherben, 274–282 fliegen sie (`d01`). Die grüne Lache wächst im Bild: 1021 → 1778 → 3188 grüne Pixel in Bild 286/298/310, danach stabil, Mittel (19, 77, 17) (`d02`) |
| b Elza, ROOM2071, w7 | Standard-KI | Slot 2 F36 und Slot 1 F61: ta=1 sh=80001. Lache Slot 2 F48 → 2020 x 2120 `bf10bf`, sichtbar 513/513 | Bild 272 ganz, **Bild 274 platzt** mit grüner Wolke und Scherben, ab 278 grüne Lache; Grünpixel 1041 → 2307 → 3177 (Bild 286/302/310) (`d03`) |
| c Leon, ROOM2070, w7, `RE15_KEIN_80PROZENT_GORE=1` | Schalter aus | F36/F108 **ta=0 sh=00000**. Sub 8 nur 44 Zeichnungen (`re2s_blut`). Lache wächst wie in a | Bild 266–310: Die Spinne liegt auf dem Rücken und bleibt ganz, keine Scherben. Die grüne Lache wächst trotzdem (so im Dossier NB2.4 festgelegt) (`d04`) |
| d Leon, ROOM10C0, Krähe, Redhawk w7 | Standard-KI | **F94 GIB, 11 Federn** (Plätze 95..85), Versatz zur Krähe (4393,-2840,1685): x -496..268, y -392..492, z -504..360, alle s8·4, Gier alle rand<<4. 169 Bilder mit Feder, in jedem Bild `gezeichnet = quads`, `tex=1` | Bild 354 Krähe im Anflug, **Bild 356 platzt sie** in dunkelrote Teile und Blut, die Federn entstehen dort, wo sie platzt (`d06`, Ausschnitt). Bild 360–384: Die Federn pendeln quer durch den Gang und sinken zu Boden (`d05`) |
| e Elza, ROOM10C1, Krähe, w7 | Standard-KI | F94 GIB 11 Federn, bitgleich zu d (gleiche Saat) | Bild 292 Krähe, **Bild 294 platzt sie**, Federn an der Stelle, 296–314 pendeln und sinken (`d07`) |
| f Leon, ROOM10C0, Krähe, **Pistole w3** | Standard-KI | **F85 Flug-Treffer: 2 Federn** (`anlass=treffer`, Versatz x -176..-16, y -452..244, z -296..60), **F88 Sturz: 2 × 1 Feder**. Zusammen 4, wie N4b. Die Krähe stirbt mit diesem Treffer (HP -5) | Bild 374 Treffer, rotes Blut am Körper der Krähe und eine Feder darunter, Bild 376 Blutwolke mit Feder, 378–390 sinken die Federn (`d08`) |
| g Leon, ROOM2070, w7, `RE15_BABYTOD_TEST=200` | Messhaken | 15 Babys sterben bei F200. `lache.log.flecken`: 5400 Zeilen, **2880 sichtbar, alle 2880 mit `gezeichnet=1`**, 200 x 200 `bf10bf`, `raum=2070` | Der Fleck ist im Fenster kaum zu sehen. 5 der 8 sichtbaren Flecken liegen in der Fläche der großen Lache ((2794,-6333) ±2020/2120). Einer zeigt sich rechts unten (Bild 440, x 500..540, y 460..480) als schwacher dunkelgrüner Fleck, der in Lauf a fehlt (`d10`) |

Die Federfarbe ist Absicht: Meine eigene Dekodierung von `esp0C.tim` (`d09`) zeigt die CLUT `0000 39ad 316b … 1422 1001 0c00`,
also blaugrau bis dunkelviolett. Das passt zum Bild im Fenster.

## 2. Zitierte Adressen gegen die Bytes

Ich habe mit eigenem capstone-Werkzeug gelesen (`d12_bytes.txt`). RE2 stammt aus `info/re2leon`, RE1.5 aus dem Mod-Baum
`info/Re1.5` und der Disc `Version_40%`.

**Die neuen Effekt-Ops (RE2 PSX.EXE)**, Op-Tafel @0x8009d868:

- [5] = 0x8001df90, [16] = 0x8001f128, [34] = 0x80020364 (@0x8009d8f0), [35] = 0x800204f8, [36] = 0x80020510.
- **Op 34**:
  - `lhu v0,0x12` / `beqz → 0x800204d4` @0x80020374-7c.
  - Bei +0x12 == 0: `lh v0,0xe` / `bgtz` / `sh 1,0x12` @0x800204d4-e4.
  - Sonst `lh 0xe` / `lh 0x4a` / `slt` / `beqz` @0x80020384-94.
  - Wurf: `jal 0x80015fe8` @0x8002039c, /70 über `0xea0ea0eb` und `sra 6` @0x800203a4-bc, `addiu 0x32` @0x800203e8,
    `lb 0x42` · → `sh a2,0xc` @0x800203f8.
  - /50 über `0x51eb851f` und `sra 4` → `sh 0x10` @0x80020448.
  - /20 über `0x66666667` und `sra 3`, `addiu 0x28` @0x80020478.
  - `sh zero,0x12` @0x800204a8, `sh v0,0xe` @0x800204ac, Kippen `negu` / `sll 24` / `sra 24` / `sh 0x42` @0x800204b0-bc,
    -(3v)/10 → `sh 0x4a` @0x800204d0.
- **Op 35**: `sb zero,(v0)` @0x80020504, `sh zero,0x18` @0x8002050c.
- **Op 36**: derselbe Wurf ohne +0x12. Danach `lw a2,0x3c` / `lhu a3,0x40` @0x8002061c-20, `sw a2,0x34` / `sh a3,0x38`
  @0x8002063c-40 und `sh v0,0x4a` @0x80020654.

Der Port-Code (`re2_fx.c` op_34/35/36, `feder_wurf`, `feder_wurf_ende`) folgt dem Zeile für Zeile. Auch die Reihenfolge
stimmt: Op 36 liest +0x3C erst nach den drei Zufallszahlen. `rng()` liefert 0..255, wie `andi v0,v0,0xff` @0x80016014.

**In RE1.5 gibt es diese Ops nicht.** Bei 0x80020364/0x800204f8/0x80020510 steht in Mod und Disc anderer Code (gleich in
beiden, 0 Abweichungen). Den Op-35-Körper und das /70-Idiom habe ich im ganzen RE1.5-EXE gesucht, in beiden Ständen ohne
Treffer. Die Zitate sind richtig als RE2 bezeichnet.

**EMOVL21_S0 FUN_80104678**:

- Schleife `move v0,s0` / `addiu s0,-1` / `beqz` @0x8010468c-94, das sind n Federn.
- Je Feder `sll 24` / `sra 22` auf `lw 0x38/0x3c/0x40(s1)` → `sh 0x10/0x12/0x14(sp)` @0x801046a4-e8.
- `lui a0,0xc00` / `ori 0x800` @0x801046ec-f0, `sll a1,v0,4` @0x801046f4, a2 = 0x8009DB44 @0x801046f8-fc,
  `jal 0x8001bf10` @0x80104700.

**Aufrufer**:

- GIB: `andi 3` / `ori a1,v0,8` @0x80102eb4-bc.
- Flug-Treffer: `andi 1` / `ori a1,v0,2` @0x80102a14-1c.
- Sturz: zweimal `bnez` (nur bei Zähler 0), dann je 1 Feder @0x80102a60-70 und @0x80102a80-90.
- Boden: `addiu a1,v0,3` @0x80103044.
- Launch: `addiu a1,v0,5` @0x8010330c.
- Picken 1/4 @0x80102638-4c.

**Der Spawner FUN_8001bf10**: Id = `a0 >> 24` @0x8001bf14, `ori v0,zero,0xa003` @0x8001bfa8, kein `jal` bis `jr ra` @0x8001c21c.

**Das Zitat aus Mangel 3**:

- `addiu v1,zero,0x32` @0x8001649c (im Delay-Slot von `beqz` @0x80016498).
- `addiu s3,zero,0x32` @0x80016350.
- `addiu s5,zero,0x32` @0x800169a4.
- An @0x800164a4 steht `beqz v1`.
- `re15_spinnenlache_r41.h:56-59` nennt jetzt die drei richtigen Stellen. Sonst nennt nichts mehr @0x800164a4 (`grep`).

**Asset:** `GLAS1090_0C.TIM` md5 `170131b28e4936781d87122daefec407` ist gleich `room1090/esp0C.tim` und
`room2110/esp0C.tim`. Er steht in ROOM1090.RDT bei @0x336C8, ist 4 bpp und 256 x 120. Die Raum-ESP ist bytegleich mit
`room1090/effect.esp`: Id-Liste `29 10 11 12 13 14 0c 19`, Bankkopf @0x994 `1d 00 10 00 40 78`, Skript @0xAD4
`01 00 00 00 01 00 00 00 22 10 23 24 00 10 00 10 00 fe 00 22 …`.

**RE1.5-Stichprobe Mod gegen Disc**, 0 Abweichungen:

- Krit-Resolver @0x80012370-b8.
- Sprung über die Waffentabelle @0x80113fc8-e0.
- Die Tabelle @0x80118f38 (22 Worte, wie Dossier 2b).
- Platzweg @0x80114044/0x80114218-24/0x80114b1c-20.
- CORPSE-Zähler @0x801155a8-b8.
- Krit-Zweig @0x801146b0.

**Eine Ungenauigkeit ohne Mängelrang:** Der Kommentar in `enemy_ai_re2_crow.c` (re2c_feather) schreibt „Boden - 250 im
Sitzen, `addiu -250` @0x80102B60“. Das `addiu s1,v1,-0xfa` steht @0x80102b40. @0x80102b60 ist `sw s1,0x3c(s0)`. Der Wert
ist keine Konstante im Port-Code, und das Dossier (NB3.1) zitiert richtig (`@0x80102b3c-44`, `sw` @0x80102b60).

## 3. Sind die Riegel ohne den Bau rot? (eigene Gegenproben, `gegen4.py`)

Ich habe einzelne Stellen ausgebaut, jeweils mit `local_build.sh build`. Danach liefen die Riegel. Am Ende kam
`gegen4.py restore` (`git checkout HEAD --` der fünf Dateien, kein `GEGENPROBE-ABN4` mehr im Baum).

| Gegenprobe | Ausgebaut | Ergebnis |
|---|---|---|
| X1 | `re15_render_shadow_quad_c` in der Flecken-Schleife von `main.c` (der Babyfleck wird nicht mehr gezeichnet) | **rot**: `integration_r41_spinnengore` [BT] `pruef_flecken: Zeilen 3000, sichtbar 1200, gezeichnet 0, sichtbar ohne Zeichnung 1200`. Die übrigen Läufe (L7/E7/L8/L7AUS/E7R15/K/KE) bleiben grün |
| X2 | `re15_spinnenlache_raum_reset()` in `re15_re2z_rng_reset` (kein Leeren beim Raumwechsel) | **rot**: `unit_r41_spinnengore` M7b („re15_re2z_rng_reset loescht die Flecken nicht“), M7c ×2 (über `re15_actor_init`), dazu als Folge M7d ×2 und M8 |
| Y1 | GIB `\| 0x8` → `\| 0x4` (4..7 Federn) | **rot**: N2 „GIB 7 Federn (Soll 8..11)“, Integration K/KE „Satzgroesse falsch 1“ |
| Y2 | Federort x + 1024 (neben dem Körper) | **rot**: N2/N4/N5 „Versatz (1408,32,352) nicht s8*4 um die Kraehe“, K/KE „ausserhalb +-512 4“ |
| Y3 | Op 34/36 ohne Kippen von +0x42 | **rot**: N3 „0 Richtungswechsel (+0x42 @0x800204b0-bc)“ |
| Y4 | `glas1120_pc.c` zeichnet Bank 0x0C nicht | **rot** nur in der Integration: K/KE „Bilder mit Feder 149, davon gezeichnet 0“. Die Unit-Tests sehen den PC-Zeichner nicht, das ist erwartet |
| Y5 | Flug-Treffer `\| 0x2` → ohne (0..1 Federn) | **rot**: N4 „Flug-Treffer 1 Federn (Soll 2..3)“ |

`unit_re2_spider_ai`, `unit_re2_baby_spider_dmg` und `unit_re2_crow_ai` blieben in allen Gegenproben grün. Sie hängen
nicht an diesen Stellen.

**Nach dem Zurückholen** habe ich neu gebaut (LOCAL-BUILD-OK). `git status` zeigt danach nur meine Belege. Die Riegel
`unit_r41_spinnengore`, `unit_re2_spider_ai`, `unit_re2_baby_spider_dmg`, `unit_re2_crow_ai` und
`integration_r41_spinnengore` (208 s) sind wieder grün. Die md5 der exe weicht vom Kopf-Bau ab, weil `main.c`
`__DATE__`/`__TIME__` einbaut. Alle Fensterläufe stammen vom Kopf-Bau vor den Gegenproben (Messkopie, md5 gleich).

## 4. Nebenwirkungen

Die Tests liefen aus PowerShell mit `C:\msys64\mingw64\bin` vorn im PATH und `SDL_ASSERT=always_ignore`, auf dem Kopf-Bau
vor den Gegenproben.

**Unit-Zielauswahl: 326/326 grün** (`-j 6`, 164 s, `unit_auswahl.txt`). Das Muster:

```
^(unit|probe)_(.*(spinn|spider|baby|crow|kraehe|gore|lache|schatten|corpse|re2z|re2fx|esp|plattform|glas|fenster|1090|hund|dog|redhawk|granate|schuss|zombie)|re2_|r41_)
```

Darin sind:

- Spinnen und Babys: `unit_r41_spinnengore`, `unit_re2_spider_ai`, `unit_re2_baby_spider_dmg`.
- Krähe: `unit_re2_crow_*` und `unit_crow_*` (9 + 4).
- Glas und Fenster 1090: `unit_r35_fenster_glas`, `_kraehe`, `_knall`, `_ereignis`, `_satzform` und
  `unit_r36_elza_fenster`.
- Hund: `unit_re2_dog_ai`, `unit_r30_hund_tod*` und weitere, zusammen 28.
- Gore: `unit_re2_gore`, `unit_re2_gore80`, `unit_re2_gore_decoder`, `_fly`, `_render` und `unit_zombie_gore`.
- `unit_re2z_*` (15) und alle `unit_re2_*`.
- RE2-FX: `unit_r34_re2fx*`, `unit_esp_parse`.
- Schatten: `unit_r36_pri_schatten`.
- Alle 134 `unit_r41_*`.

Das ist wichtig, weil Op 34 und Op 36 je Wurf 3 Zahlen aus dem gemeinsamen RE2-Zufallsstrom ziehen (wie RE2
FUN_80015fe8).

**Integration einzeln nacheinander: 14/14 grün** (`-j 1`, 944 s, `integ.txt`):

| Test | Dauer |
|---|---|
| `integration_r35_fenster` (Glas und Fensterkrähe 1120, gleicher Zeichner `glas1120_pc.c`) | 26 s |
| `integration_r35_entladen_j` (Krähentod ROOM10C0) | 52 s |
| `integration_r35_redhawk` | 17 s |
| `integration_r36_elza_fenster` | 52 s |
| `integration_r36_pri_schatten` | 29 s |
| `integration_r37_elzaems_elza10c1kraehe` / `_leon10c0kraehe` | 42 s / 51 s |
| `integration_r37_re2zombie` | 56 s |
| `integration_r38_spinnen_echtlauf` | 121 s |
| `integration_r39_gegner_schatten` | 61 s |
| `integration_r39_gegner_hunde1190` / `integration_r39_hund1121` | 50 s / 90 s |
| `integration_r41_spinnenanim` | 126 s |
| `integration_r41_spinnengore` | 173 s |

Die Ausgabe von `integration_r41_spinnengore` lautet:

```
pruef_flecken: Zeilen 3000, sichtbar 1200, gezeichnet 1200, sichtbar ohne Zeichnung 0, fremd 0
pruef_federn gib: Saetze 1, Federn 8 (nicht gespawnt 0, ausserhalb +-512 0, Satzgroesse falsch 0), Bilder mit Feder 159, davon gezeichnet 159, Bild geladen 1   (K und KE)
```

**Messlauf ohne Aufnahme**, `m_leon_10c0_w7_re15ki`: Krähe in ROOM10C0 unter `RE15_AI_FLAVOR=re15`, Redhawk. Die
Krähe stirbt (`st=3 ss1=7`), aber `feder.log.spawn` entsteht nicht, und `feder.log` ist leer. Unter der RE1.5-KI gibt es
also keine Federn (Hinweis 3).

## 5. Das Dossier: offene Punkte der Folgespur

Der Orchestrator hat diese Punkte aus der Runde genommen. Ich prüfe nur, ob das Dossier sie als offen führt:

| Merkmal | Im Dossier | Bewertung |
|---|---|---|
| Granate zerreißt Zombie | Offen 9 (Liste nach NB1), Tabelle 5 | offen geführt |
| Kopflos mit anderen Waffen | Offen 10 | offen geführt |
| Wandspritzer | Offen 11 | offen geführt |
| Lache unter lebendem Zombie | Offen 12 | offen geführt |
| Krähenfetzen (liegend) | Offen 15 (berichtigt), NB3.8 Nr. 26 | offen geführt, RE2 gibt Federn bei Bodenkontakt frei (Op 35, selbst gelesen) |
| Liegende Spinnenflecken | Offen 1: „Offen bleiben davon nur die Wandflecken aus dem Video“ | **nur zum Teil**, siehe Hinweis 1 |
| Klettern der Spinne unter RE2 | Tabelle 5b, Offen 7 | offen geführt |
| Fressen/Schlafen (Zombie) | nur in Tabelle 5b, Zeile Zombie-Familie | **nur zum Teil**, siehe Hinweis 2 |
| Würgearm, Hand fliegt ab | Offen 14 | offen geführt |

## Mängel

Keine.

## Hinweise ohne Mängelrang

1. **Liegende Spinnenflecken.** Das Video S01 zeigt grüne Flecken, die „gut 1 s auf Boden und Wand“ liegen bleiben
   (`S_video_gore.md:109`). Seit NB2 sagt Offen 1, offen seien „nur die Wandflecken“. Die Lache ist aber ein wachsendes
   Viereck unter der Leiche, nicht die verstreuten Bodenflecken aus dem Video. Für die Folgespur sollte Offen 1 die
   verstreuten Bodenflecken wieder neben den Wandflecken nennen.
2. **Fressen/Schlafen.** Die Offen-Liste nach NB1 hat Offen 9 („Nutzerentscheidung 5b, Tabelle 5b“) auf Punkt 7
   „Klettern“ verengt. Fressen/Schlafen der Zombie-Familie steht nur noch in Tabelle 5b, nicht in der Liste. Für die
   Folgespur sollte Punkt 7 wieder auf die ganze Tabelle 5b verweisen.
3. **Krähe unter `RE15_AI_FLAVOR=re15`:** keine Federn. `re2c_feather` gibt es nur im RE2-Gehirn. Dasselbe gilt für die
   Ereigniskrähe (R38 I), die auch unter Standard auf RE1.5 läuft. Das Dossier nennt das nicht. Nach der Nutzerregel
   „Gore = Maximum“ ist das eine Frage an den Nutzer oder die Folgespur, kein Mangel dieser Runde (Auftrag: Standard-KI).
4. Die Tabelle 5 nennt beim Hund noch „Bild aus dem Fenster nicht gelungen“. Das ist durch Offen 16 (NB1) und den
   Fensterbeleg der Abnahme 3 (`abn3/c09`) überholt.
5. Der Babyfleck ist im echten Fenster von ROOM2070 kaum zu erkennen (Lauf g, `d10`). Er wird gezeichnet
   (`gezeichnet=1`), liegt aber meist in der großen Lache oder auf dunklem Boden. Für die Release Notes ist er also
   kein sichtbares Merkmal.
6. Ein Kommentar ist verrutscht: `enemy_ai_re2_crow.c:83`. Der Kommentar „Runde 38 Spur B: kein Kraehen-Angriff…“ hängt
   jetzt hinter `#include "re15_engine.h"` statt hinter `re15_szenenschutz_r38.h`. Das ist kosmetisch.
7. Ein Kommentar zitiert ungenau: `enemy_ai_re2_crow.c` (re2c_feather) nennt „`addiu -250` @0x80102B60“. Das `addiu`
   steht @0x80102b40, @0x80102b60 ist das `sw` (Abschnitt 2). Die -250 ist keine Konstante im Code, und das Dossier
   zitiert richtig. Bei der nächsten Berührung der Datei sollte die Stelle berichtigt werden.
