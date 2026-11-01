
## Teil-Schritt: Gore-Sichtung der 80%-Videos (2026-11-01, nichts gebaut)

Ergebnis steht in `S_video_gore.md`, die Belege in `S_belege/video/g01…g31`. Kurz:

- Die Spinne platzt im Video (B 5:18.3, g09): gelbgrüne Brocken, der Kadaver mit Beinen bleibt.
- Unter der Standard-KI gibt der Port jede 0x25 an das RE2-Gehirn ab (`enemy_ai_re2_spider.c:3071`, `enemy_ai_common.c:11156`).
  Dort ist ZERPLATZEN nur Clip 12 mit einem ESP-Effekt am Ursprung (`:2214`, `:1754`), der Teile-Flug ist OPEN.
  Das RE1.5-Platzen (`enemy_ai_common.c:11459-11466`) wird nicht erreicht.
  Das ist am Code bestätigt, eine Laufzeitmessung steht noch aus.
- Zombies zerplatzen im Video durch Granaten ganz (B 3:31.4, 3:36.0, 8:42.2). Im Port bleibt der Rumpf (Zeile 9 auf 8BEC).
- Die Krähe zerfällt in schwarze Fetzen, der Port zeigt nur Blut.
- Kopf weg und kopflos weiter: im Video wahrscheinlich (B 2:28.4, 2:34.9), die Waffe ist offen.
- Drei Aussagen aus BEFUND §5 sind am Einzelbild widerlegt: der Kopf bei 2:45.7 und bei 11:36 bleibt, und es gibt keinen Torso-Abriss bei 8:10.

## Teil-Schritt 2: RE der Spinnen-Gore-Mechanik (2026-11-01, laufend)

Werkzeug: `.claude/skills/re15-psx-disasm/scripts/re15_disasm.py` (RE1.5) und `re2_disasm.py` (RE2, `EMS25.BIN`).
Jede RE1.5-Stelle unten ist in **beiden** Staenden gelesen: Mod-Baum `info/Re1.5` und echte Disc `Version_40%`
(Wrapper `scratchpad/S_re/dis40.py`). Alle Ausgaben waren bis auf die Dateikopfzeile gleich (`diff` leer).

### 2a. Wer setzt +0x93 Bit 0x40 (Krit)? — FUN_80011f50, PSX.EXE (Mod = Disc)

```
80012370: lbu v0,147(s1)          ; +0x93
80012378: andi v0,v0,0x1
8001237c: sb v0,147(s1)           ; +0x93 &= 1
80012380: ori v0,zero,0x8
80012384: bne v1,v0,0x800123a4    ; Waffe != 8 -> Magnum-Test
8001238c: lui v0,0x8009
80012390: lw v0,-2592(v0)         ; 0x8008f5e0 = Abstand zum Ziel
80012398: sltiu v0,v0,0xbb8       ; < 3000
8001239c: bne v0,zero,0x800123ac  ; Schrot nah -> Krit
800123a0: ori v0,zero,0x7
800123a4: bne v1,v0,0x800123c0    ; Waffe != 7 -> kein Krit
800123ac: lbu v0,147(s1)
800123b4: ori v0,v0,0x40
800123b8: sb v0,147(s1)           ; +0x93 |= 0x40
```
- Krit = Waffe 7 (Super Redhawk) immer, Waffe 8 (Remington M870) bei Abstand < 3000. Keine Zufallszahl:
  die Quote ist **100 %** fuer diese Treffer. Gilt fuer jeden Gegnertyp (der Resolver ist typblind).
- Der Port setzt das Bit im gemeinsamen Resolver fuer beide Gehirne (`re15_damage.c` ~Z. 2745), dazu w20
  Colt Python (PORT-WAHL Runde 35 Spur B). Unter der RE2-KI kommt das Bit also an der Spinne an.

### 2b. RE1.5 Spinne 0x25 (STAGE2.BIN, Mod = Disc)

**HURT 0x80113aa4** (ueberlebter Treffer, Phase +0x7 == 0):
- Blut 0x80019700(0x82000) an Teil 0 (`a2 = +0x188 + 64`) @0x80113b50, SE 2 @0x80113b58 — **jeder** Treffer.
- `andi v0,v0,0x40` / `beq v0,zero,0x80113e48` @0x80113b74-78: nur bei Krit:
  Bein = Tabelle @0x80118f28 {3,5,7,9,11,13,15,17}[rng&7] (`jal 0x8001af20`, `andi v0,v0,0x7` @0x80113b80-90);
  Liste @0x80118f30 (Overlay-Daten, **allen Spinnen gemeinsam**, Zaehler je Spinne +0x1de) — schon drin -> nichts.
  Sonst Teil `leg` und `leg+1`: Farbwort +0x68 = 0x207f60 (`lui t0,0x20`/`ori t0,t0,0x7f60` @0x80113c58/98,
  `sw t0,104(v1)` @0x80113cdc), +0x94..+0x9e = {0,-50,0,3,0,0} (@0x80113ce0-cf4), Wort |= 0x4a (@0x80113cf8-cfc,
  Teil leg+1 @0x80113d50-d78); Blut 0x82000 zweimal an Teil leg+2 (`addiu a2,a2,408` @0x80113dc8/@0x80113e20),
  SE 5 @0x80113e24, +0x1de++ @0x80113e38-44.
- Quote: Krit 100 % (2a), Bein: 1 - (belegte Plaetze der gemeinsamen Liste)/8.

**DEATH 0x80113f40**: Wurzel springt ueber `lbu v0,5(v0)` (= Waffen-Id des letzten Treffers, Resolver `+0x5 =
weapon_id` @0x800124bc) in die Tabelle **@0x80118f38** (`addiu at,at,-28872` @0x80113fd0, `jalr v0` @0x80113fe0):
```
[0..6]   -> 0x8011461c   (unbewaffnet, Messer, Rohr, Pistolen, Beretta, Glock)
[7..11]  -> 0x80113ff8   (Redhawk, Remington, Handgranaten HE/Saeure/Brand)
[12]     -> 0x8011461c   (Ingram)
[13]     -> 0x80113ff8   (SPAS-12)
[14,15]  -> 0x8011461c   (Flammenwerfer, GL HE)
[16..18] -> 0x80113ff8   (GL Saeure/Brand, Raketenwerfer)
[19,20]  -> 0x8011461c   (MC51, Colt Python)
[21]     -> 0x80113ff8
```
**0x80113ff8 = ZERPLATZEN der starken Waffen, OHNE Krit- und OHNE Zufallstest** (Phase-Tabelle @0x8010023c,
Phase 0 @0x8011403c):
- +0x1d8 = 1, Clip +0x94 = 1, +0x7 = 1, +0x95 = 0, +0x8f = 7, +0x8c = 0, +0x9c = 30 (@0x80114044-ac).
- Blut 0x82000 an Teil 0 @0x801140c4.
- **Teil 0** (Rumpf): 0x207f60, {0,-50,0,3,0,0}, |= 0x4a (@0x801140cc-10c);
  **Teil 19** (+3268 = 19*172, Hinterleib): dasselbe (@0x80114110-14c).
- Blut 0x82000 an Teil 0 (@0x80114164) und an Teil 19 (`addiu a2,a2,3332` @0x8011418c), SE 5 @0x80114190.
- Schleife +0x1e0 = 9, `--` bis 0 (@0x8011419c-1c8, @0x80114518-52c): fuer i = 8..1 die Beinpaare A = 2i+1,
  B = 2i+2 (`pool + 344*i + 172`), nur wenn `(A.w & 0x41) == 1` (sichtbar, noch nicht abgeloest, @0x80114218-21c):
  A.w |= 0x1062 (@0x80114220-224), B.w |= 0x1062 (@0x80114264-268), je Teil +0x75 = 20 (Schwerkraft,
  @0x801142a0 / @0x801143f0), +0x82 = -128 (Drehung, @0x801142d8 / @0x80114428), +0x76 = 8 (Drehschritt,
  @0x80114310 / @0x80114460), +0x38/+0x3a/+0x3c = 0/-100/0 (@0x80114348/80/b8 bzw. @0x80114498/d0/508).
  0x1062 = 0x1000 eigener Lichtbezug + 0x40 eigene Matrix + 0x20 Flug FUN_8001f024 + 0x02.
  **Alle noch dranhaengenden Beine fliegen ab** (8 Paare), mit Schwerkraft auf den Boden.
- Phase 1 @0x80114534: anim_set(+0x84,+0x16c,0,0x200), +0x7 += fertig; Phase 2 @0x80114570: +0x4 = 7 (CORPSE).
- Quote: **100 %** bei jedem toedlichen Treffer der Waffen 7, 8, 9, 10, 11, 13, 16, 17, 18, 21.

**0x8011461c = Tod der schwachen Waffen**: Phase 0 @0x80114b78: Clip 4, +0x93 = 0 (`sb zero,147` @0x80114bd4),
+0x1d8 = 1. Blut an Teil 0 bei +0x7 == 0 @0x80114654. Ist die Spinne schon tot (+0x1d8 != 0), springt ein neuer
Treffer nach Phase 3 (@0x80114668-678). In Phase 1 oder 3 mit `+0x93 & 1` und `andi v0,v1,0x40` @0x801146b0:
derselbe Komplett-Platzer wie oben (Teile 0/19 @0x801146bc-734, Blut @0x8011474c/@0x80114770, alle Beine
@0x80114780-b0c), danach +0x5 = 7 (@0x80114b20). Im Original ist dieser Krit-Zweig praktisch tot: Krit gibt es nur
fuer w7/w8, und die laufen ueber +0x5 in 0x80113ff8. Im Port erreicht ihn w20 (Python, Krit-PORT-WAHL).

**Befund am Port (RE1.5-Gehirn, `enemy_ai_common.c` case 3):** verkuerzt. Er prueft nur das Krit-Bit, nicht die
Waffentabelle @0x80118f38, und laesst die Beine nicht fliegen. Granaten, SPAS, Rakete und Schrot >= 3000 platzen dort
nicht, obwohl das Original sie immer platzen laesst.

### 2c. RE2 Retail Spinne (EMS25.BIN)

- **FUN_80105BF0 (Bein ab)** @0x80105c44-cf4: Teile 2k+3 und 2k+4 je: Wort |= 0x4a (@0x80105cc0-c4), +0xa0 = 0,
  +0x9c = 600, +0x9a = -120, +0x9e = 10, +0xa4 = -100 (@0x80105ccc-cdc), Farbwort +0x70 = 0x2f2f2f
  (`lui a1,0x2f`/`ori a1,a1,0x2f2f` @0x80105c88-8c, `sw a1,112(v1)` @0x80105ce0), +0x98 = Blick+2048
  (@0x80105ce4-e8). Nur wenn +0x221 >= 3 (`sltiu v1,v1,0x3` @0x80105c20) und das Bein frei ist (@0x80105c34-3c).
  RE2-Bit 0x08 ist die Drift FUN_80028DAC (29 Bilder, dann weg), 0x40 eigene Matrix: das Bein treibt dunkel weg.
- **FUN_80104890 (Hinterleib platzt)**, gerufen aus der Tod-Sonderzeile @0x80104588 (Zeilen 5,6,7,8,17) und
  ueber Zeile 9: +0x1c0 |= 1 (@0x801048b4-c0); **Teil 19** (+3268): |= 0x4a (@0x801048c4-d0), +0x9a = -240,
  +0x9e = 15, +0x9c = 0, +0xa0 = 0, +0xa4 = 0 (@0x801048d8-f0), Farbwort 0x2f2f2f (@0x8010489c-a0, `sw t0,3380(v1)`
  @0x801048f4), +0x98 = Blick+2048 (@0x801048f8-900); dann FUN_801056DC(self,19,1) und 4x FUN_801056DC(self,19,2)
  (@0x801048fc-91c). Der Port-Kommentar "Sprays" an `re2s_gore` @0x80104588 trifft das nicht: es ist der Hinterleib.
- RE2 hat also eigenes Spinnen-Gore: Bein ab (Hurt Zeilen 5-8/17, 11, 4/15/18) und Hinterleib platzt (Tod Zeilen
  5-9/17), dazu 6-9 Babys. Im Port steht davon nur die Buchfuehrung (+0x220/+0x221) und je ein ESP-Effekt am
  Ursprung; gezeichnet wird nichts (die RE2-Teile-Bruecke `re15_re2z_gore_active` kennt Typ 0x25 nicht und fasst
  ohnehin nur 16 Teile, die Spinne hat 20).

## Teil-Schritt 1: Messung VORHER (echte exe, Stand aa88a1b6a + nur Messschiene)

Werkzeug: die bestehende Messschiene `RE15_SPINNEN_LOG` (Runde 38, `spinnen_r38.c`) bekam hinter jeder Spinnen-Klammer
eine Gore-Klammer `{G<slot> o= ta= sh= fl= un= lg= ln= z=}`: o = 1 RE2-Spinnen-Gehirn, ta = Teilschicht aktiv,
sh/fl/un = Masken der 20 Teile (Scherben 0x08 / Flug 0x20 / Bit 0 aus), lg/ln = RE2 +0x220/+0x221, z = RE2-Zeile.
Kein Verhalten. Laeufe ueber die Messkopie `re15_pc_r41S_mess.exe`, Skript `scratchpad/S_mess/run.sh`, Auswerter
`scratchpad/S_mess/ana.py`. Weg: `RE15_DEBUG_JUMP=2070@250` (Leon steht bei (4851,-6250)), `RE15_GIVE=<w>:<n>
RE15_EQUIP=<w>`, Zielen + Feuern im Wechsel (`M0.3,MA0.1,M0.8,MA0.1,...`). In ROOM2070 sind auf diesem Weg **2**
Spinnen aktiv (Slots 1/2, HP 111/110 unter RE2), nicht 12 (die 12 Saetze sind bedingt, vgl. R38 1.1/4.1).

**Wie erreicht man die Magnum?** Item_aot_set ROOM3050/3051 slot 6 @RDT 0x1ec6, floor 1, Item 7 x6 (eigener Zensus
ueber alle RDTs des Repo-Baums, Feld u16 @+14); nur ueber die gefallene Kiste (R36 R_abnahme_0). Das ist STAGE3, die
Spinnen 0x25 stehen nur in STAGE2. Remington M870 (8): ROOM10F0/10F1 @0x103e. SPAS-12 (13): ROOM2030/2031 @0x1bd6.
Ob man mit der Magnum nach STAGE2 zurueck kann, habe ich nicht geprueft. Gemessen wurde mit `RE15_GIVE`/`RE15_EQUIP`.

| Lauf | Gehirn | Waffe | Ergebnis je Spinne (Bild: Zustand, Zeile, HP, +0x93, Teile) |
|---|---|---|---|
| m_w7 Leon | RE2 (o=1) | 7 Redhawk | Bild 68: 1 Schuss, HP 110 -> -20, Zustand 3, Zeile 5, **+0x93 = 0x41 (Krit gesetzt)**, ta=0, sh/fl/un = 0, 6 Babys, fx 6. Bild 79 CORPSE. Slot 1 Bild 122 genauso. **Kein Teil platzt.** |
| e_w7 Elza ROOM2071 | RE2 | 7 | Bild 95 / 176: wie Leon (HP -20 / -19, +0x93 = 0x41, ta=0). |
| m_w8 Leon | RE2 | 8 Remington | Bild 95: HURT Zeile 7, +0x93 = 0x41, RE2 trennt Bein k=4 ab (lg=10, ln=7) — **nur Buchfuehrung, ta=0**. Slot 1 Bild 98: +0x93 = 0x01 (>= 3000), lg=80. Tod Bild 311/314 Zeile 7: ta=0. |
| m_w3 Leon | RE2 | 3 Browning | je Treffer 17 Schaden, Zeile 3 generisch, +0x93 = 0x01, Tod Bild 257 generisch, ta=0. |
| r15_w8 Leon | RE1.5 (`RE15_AI_FLAVOR=re15`, o=0) | 8 | Bild 95 Krit-Treffer: ta=1, **sh = 0x60000 (Teile 17/18)**, nach 29 Bildern un = 0x60000. Tod Bild 149 mit +0x93 = 0x43: **sh = 0x80001 (Teile 0/19)**, keine fliegenden Beine (fl = 0). Slot 1 Tod Bild 203 mit +0x93 = 0x03 (Abstand >= 3000): **ta=0, nichts platzt**, obwohl das Original w8 immer ueber 0x80113ff8 platzen laesst (2b). |
| r15_w7 Leon | RE1.5 | 7 | kein Treffer: die RE1.5-Spinnen stehen auf diesem Weg 6000+ entfernt (y -6400), der Redhawk-Streifen erreicht sie nicht. Nicht weiter verfolgt (Unit-Riegel deckt den Pfad). |

Damit gemessen (nicht mehr nur gelesen):
1. Unter der Standard-KI gehoert jede 0x25 dem RE2-Gehirn (o=1 in allen Laeufen, Leon und Elza).
2. Das Krit-Bit kommt dort an (+0x93 = 0x41 bei w7 und bei w8 < 3000), wird aber nicht gelesen.
3. Die Teilschicht wird unter RE2 nie eingeschaltet (ta=0): kein Bein, kein Rumpf, kein Hinterleib platzt sichtbar.
   Sichtbar sind nur ESP-Effekte am Ursprung (fx) und die Babys.
4. Unter RE1.5 platzt die Spinne nur bei Krit; Beine fliegen nie; w8 aus >= 3000 und alle Granaten/SPAS/Rakete
   platzen dort nicht (Port verkuerzt gegen 0x80113ff8).

## Teil-Schritt 4: Bau (Stand im Arbeitsbaum, noch nicht committet)

- Neu `engine/src/spinnengore_r41.c` + `include/re15_spinnengore_r41.h`: die RE1.5-Bausteine (Teil platzt,
  Beine fliegen, Komplett-Platzer 0x8011403c, Beinpaar ab) und die Hebung ins RE2-Gehirn. Schalter wie beim Zombie:
  `RE15_KEIN_80PROZENT_GORE=1` schaltet die Hebung ab (Test-Vorgabe `re15_spinnengore_test_schalter`).
- RE2-Gehirn (`enemy_ai_re2_spider.c`): HURT merkt +0x220 vor dem Zeilen-Handler; jedes neu abgetrennte RE2-Bein wird
  als RE1.5-Beinpaar gezeichnet (PORT-WAHL: gruenes RE1.5-Platzen statt RE2-Drift dunkel 0x2f2f2f), Krit ohne
  RE2-Abtrennung gibt das RE1.5-Krit-Bein (Beinmaske des Tiers statt der gemeinsamen RE1.5-Liste, PORT-WAHL).
  DEATH im frischen Takt: Komplett-Platzer fuer RE1.5-Platzwaffen (@0x80118f38), Krit, RE2-Platzzeilen
  5/6/7/8/9/11/17; sonst RE1.5-Todesblut an Teil 0 (@0x80114654).
- RE1.5-Gehirn (`enemy_ai_common.c` case 3): Waffentabelle @0x80118f38 statt nur Krit, Beine fliegen, Todesblut an
  Teil 0 bei den schwachen Waffen. Byte-true RE1.5, haengt NICHT am Schalter.
- `actor_common.c re15_actor_alloc`: Teilschicht des Platzes leeren (Babys in freien Plaetzen erbten sonst Teile).
- `platform/pc/main.c`: neutrale Teile der Teilschicht ueberschreiben die RE2-Part-Tinte nicht mehr
  (Spinnen-Brand 0x202f2f / Saeure 0x101f3f blieben sonst unsichtbar, sobald die Teilschicht an ist).

### Messung NACHHER (dieselben Laeufe, `n_w*`)

| Lauf | Ergebnis |
|---|---|
| n_w7 Leon RE2 | Bild 68 Tod Zeile 5, +0x93 0x41: **ta=1, sh = 0x80001 (Teile 0/19 platzen), fl = 0x7fff8 (Teile 3..18 fliegen)**. Nach 29 Bildern sind 0/19 weg (un 0x80001, Scherben-Ende @0x8001f208-214). Slot 1 Bild 122 genauso. |
| n_w8 Leon RE2 | Bild 95 Treffer Zeile 7 (Krit 0x41): RE2 trennt Bein k=4 ab -> **sh = 0x01800 (Teile 11/12)**. Slot 1 Bild 98 (kein Krit, >= 3000): RE2 trennt k=7 ab -> **sh = 0x60000 (17/18)**. Weitere Abtrennungen Bild 149 (k=1 -> 5/6) und Bild 230 (k=2 -> 7/8). |
| n_w3 Leon RE2 | unveraendert: kein Krit, Zeile 3, Tod generisch -> nur Todesblut an Teil 0 (fx 7 statt 6 im Todesbild), ta=0. |

Ein Befund am Rand, byte-true und nicht veraendert: Nach der Landung verschwinden einige fliegende Oberschenkel.
FUN_8001f024 loescht Bit 0, wenn vy == 0 und t.y == 0 (`lh v0,58(a2)` / `bne` @0x8001f104-10c, `lw v0,24(s1)` /
`bne` @0x8001f114-11c, `and v0,v0,-2` @0x8001f12c-134; Mod = Disc). Mit Schwerkraft 20 trifft das ein Teil, das mit
vy 160..167 aufschlaegt (-(vy)>>3 = -20, im naechsten Bild 0). Gemessen mit einer Wegwerf-Spur in `flug()`:
Oberschenkel Teil 3 startet bei y -519, schlaegt mit vy 160 auf, steht ein Bild spaeter mit vy 0 auf y 0 und
verschwindet. Die Unterschenkel bleiben liegen.

### Bildbeleg aus dem echten Fenster (gdigrab per Fenster-Handle der eigenen PID, beschleunigter Renderer, ohne AUTOSHOT/SOFTWARE_RENDER)

Skript `scratchpad/S_mess/cap.ps1` (Messkopie `re15_pc_r41S_bild.exe`, 10 Bilder/s, `RE15_WINDOW_SCALE=2`).
- `S_belege/s01_re2ki_magnum_platzt_2070_leon.jpg`: Leon, ROOM2070, Standard-KI, Redhawk. Aufnahmebild 110 Schuss,
  111-113 gelbgruene Scherben fliegen strahlenfoermig, rotes Blut am Rumpf, 116-124 die Beine liegen am Boden.
- `S_belege/s02_gegenprobe_schalter_aus_2070_leon.jpg`: derselbe Weg mit `RE15_KEIN_80PROZENT_GORE=1`: die Spinne
  dreht sich in Clip 12 auf den Ruecken und bleibt ganz, keine Scherben. Das ist der Stand vor dem Bau.
- `S_belege/s03_re2ki_magnum_platzt_2071_elza.jpg`: Elza, ROOM2071, dasselbe Platzen (Aufnahmebild 118-122).
- Auffaellig, aber Mechanik des Originals: einzelne Scherben fliegen bis vor die Kamera (Elza Bild 124-126, grosses
  gruenes Viereck oben rechts). Der Versatz ist FUN_80023410 (`re15_teil_scherbe`, Tabelle @0x80073454 bis 465 x
  Eckpunkt/32). Die Scherbe ist nicht falsch gezeichnet, sie fliegt so weit.

## Teil-Schritt 6: Riegel

- `unit_r41_spinnengore` (tests/unit/test_r41_spinnengore.c, probes/r41_spinnengore.cmake): Byte-Pins aus STAGE2.BIN
  (Tabelle @0x80118f38 gegen `re15_spinnengore_platzwaffe`, `ori v0,a0,0x1062` @0x80114220, `ori t0,t0,0x7f60`
  @0x80113c98, `andi v0,v0,0x40` @0x80113b74) und die Mechanik beider Gehirne: B RE2-Tod w7 platzt ganz, C Schalter
  aus, D w3 nicht, E w15 (RE2 Zeile 9) platzt, F RE2-Bein gezeichnet, G RE1.5-Krit-Bein, G2 ohne Krit nichts, H RE1.5
  w9 platzt / w3 nicht, I Platz leeren.
- `integration_r41_spinnengore` (tests/integration/test_r41_spinnengore.cmake): echte exe, L7 Leon Redhawk "platzt",
  E7 Elza Redhawk "platzt", L8 Leon Remington "bein", L7AUS Schalter aus "aus". Gruen in 89,5 s.
- **Gegenprobe ohne den Bau** (enemy_ai_common.c, actor_common.c, enemy_ai_re2_spider.c auf HEAD zurueckgesetzt,
  Modul und Test bleiben): unit **8 Fehler** (B, E, F x2, G x2, H, I), integration **rot** (L7, E7: Platzen 0,
  Teilschicht 0; L8: Bein 0). Danach Bau wiederhergestellt, unit wieder OK.
- Bestehende Riegel gruen (Zielauswahl): unit 29 Tests `spider|spinn|zombie_gore|re2_gore80|...` (u.a.
  unit_r38_spinnen, unit_r41_spinnenanim, unit_spinne_klettern, unit_zombie_gore, unit_re2_spider_ai,
  unit_adult_spider_ai, unit_re2_hit_repeat_crow_spider, unit_re2_baby_spider_dmg), weitere 121 Unit-Tests
  `gore|teile|blut|montage|...|hund`, dazu integration_r38_spinnen_echtlauf (114,8 s) und
  integration_r41_spinnenanim (117,7 s).

## Teil-Schritt 3/5: Gore-Inventur je Merkmal und je Typ

Messung: Sonde `tests/unit/probe_r41_gore_inventur.c` (Ziel `probe_r41_gore_inventur`, kein ctest). Je Typ und Gehirn:
INIT, toedlicher Treffer wie der Resolver (+0x5 Waffe, +0x6 Hoehe, +0x93 = 1|Krit; RE2-Besitz: +0x6 = 0 wie
`re15_re2_stamp_hit`, RE2-Zombie: +0x5 = RE2-Zeile, +0x1D2 = Spalte wie `RE15_ZGORE_TEST`), 60 Takte
`re15_enemy_ai_run_all`. Gemessen: Besitzer, RE1.5-Teilschicht (an/Masken), RE2-Part-Bruecke (an, neue 0x40-Parts),
ESP-Hoechststand. **Grenze der Sonde:** bankfrei (wie die Unit-KI-Pins). Die RE2-Zombie-Phasen, die an EDD-Bildflags
haengen, laufen dort nicht ganz durch. Fuer den RE2-Zombie gelten deshalb zusaetzlich die bestehenden Riegel
(`unit_re2_gore80` u.a., alle gruen) als Messung.

Ausgabe (Auszug, `RE2` = Standard-KI, `RE15` = `RE15_AI_FLAVOR=re15`; Hund/Spinne/MC51 = Stand nach Spur S):
```
Zombie w7 Mitte (Kopf)       RE2  owner=RE2   teile=0              | re2bridge=1 p40=00100 (Kopf Part 8)
Zombie w7 Mitte (Kopf)       RE15 owner=RE1.5 teile=1 sh=04000     (Gore-Teil 14 platzt, 0x80106edc)
Zombie w7 unten (Beine)      RE15 owner=RE1.5 teile=1 sh=0006c aus=00010 grid=81 hp=30 (Beine platzen -> Kriecher)
Zombie w8 Mitte Krit         RE15 owner=RE1.5 teile=1 sh=04900 aus=03600 (Zerfetzen)
Zombie w19 MC51 unten        RE2  st=7 hp=-1 (normaler Sturz, vor Spur S) | RE15 sh=0006c grid=81 (Beine platzen)
Zombie w9 Granate            RE2  p40=01300 fx=32 (8BEC)        |  RE15 grid=81 hp=30 fx=37 (Gestaltung Oberkoerper)
Wuergearm 0x1a w7            RE2 und RE15: owner=RE2, teile=0, bridge=0
Hund w7                      RE2  p40=0079c (Teile-Wurf), bridge=0 vor Spur S | RE15 nichts
Kraehe w7                    RE2  bridge=1 p40=01fff (13 Parts fliegen) | RE15 teile=0 (GIB versteckt nur)
Spinne w7                    RE2  teile=1 sh=80001 fl=7fff8 (neu) | RE15 teile=1 sh=80001 fl=7fff8
Gorilla 0x27 / 0x29          beide Flavors owner=RE1.5, kein Teil-Gore
```

### 3. Die Merkmale der Gore-Commits (Parallelzweig, alle in master)

| Commit | Merkmal | Standard-KI (RE2) | RE1.5-KI | Warum nein / Stand nach Spur S |
|---|---|---|---|---|
| b2ca28b82 | Zombie-Treffer w5/6 (Clips 6-7-8 + Blut), w7/8 Rueckstoss, w19 Rutschen (0x80106290/6624/6048) | nein | ja (RE1.5-Zweig `re15_zgore_treffer`, enemy_ai_common.c:5196) | RE2 besitzt 0x10-0x13/0x16/0x18 (`re15_re2z_owns_type`) und hat eigene Treffer (MAIN/7438/7EF0/RAGDOLL). Treffer = RE2 nach Nutzerregel, kein Gore-Verlust |
| b2ca28b82 | Zombie-Tod Magnum Mitte: Kopf platzt, 50 % kopflos weiter (0x80106edc/0x80107244) | ja, als RE2 92C4 P3/P4 (Sonde: Part 8 0x40) | ja (Sonde sh 0x04000) | — |
| b2ca28b82 | Zombie-Tod Schrot Mitte Krit: Zerfetzen / Oberkoerper ab (0x80107ee0) | ja: Zerfetzen = 8cda79eca, Oberkoerper ab = RE2 66FC (`unit_re2_gore80` gruen) | ja (Sonde sh 0x04900) | — |
| b2ca28b82 | Zombie-Tod nach unten: Beine platzen -> Kriecher (0x80108abc) fuer w7/w8/**w19** | w7/w8 ja (RE2 7438). **w19 nein** (Sonde: normaler Sturz) | ja (Sonde grid 0x81) | RE2-Zeile 18 hat in der Beinzone nur MAIN (@0x8010CEAC). **Behoben** (Abschnitt 5) |
| 710c7dd18 | RE1.5-Teilschicht (Zeichner) | ja (Zeichner flavorblind; die Spinne nutzt sie jetzt) | ja | — |
| 7950a6b04 | Messhaken RE15_ZGORE_TEST | ja (8cda79eca: auch RE2-Zombies) | ja | kein Spielmerkmal |
| 5c41aeb22 | Granaten-Tod RE1.5 (Oberkoerper ab, kriecht; Gestaltung) | nein, dafuer 4b84afcc8 | ja (Sonde grid 0x81 hp 30) | RE2-Gehirn: Zeile 9 -> 8BEC (4b84afcc8) |
| 661a36e59 | Wuergearm-Hand fliegt ab (0x8010D5D0) | **nein** | **nein** | 0x1A laeuft in JEDEM Flavor auf dem RE2-Zellenarm (`re15_ai_re2_for_type`, enemy_ai_re2_zombie.c:179, Runde 35 Spur H M1); nur der Test-Pin `re15_ai_writher_re15_pin` erreicht ihn. Sonde: owner=RE2 in beiden. RE2-Zellenarm hat keinen Teil-Wurf. **Offen** (Nutzerfrage) |
| 661a36e59 | Spinne Beine bei Krit / Platzen bei Krit-Tod | **vorher nein, jetzt ja** | ja (jetzt byte-true ueber @0x80118f38 + fliegende Beine) | Teil-Schritt 4 |
| c1469a3ab | Leons Koerperteile beim Birkin-Wurf-Tod (STAGE3 @0x8011b2f4-554) | ja | ja | Birkin 0x30/0x36 hat kein RE2-Gehirn (run_all-Zweig `t == 0x30 || t == 0x36`), der Spieler-Zeichner liest die Teilschicht flavorblind. Nicht gemessen (kein Birkin-Lauf in dieser Spur) |
| c1469a3ab | Blut an Teil 0 beim Absturz von 0x29 (affen3_r38.c) | ja | ja | 0x29 ohne RE2-Gehirn (Sonde owner=RE1.5 in beiden) |
| 53981ba5d | Zombie-Spawn-Varianten (@0x801008b0, 25 %) | ja (8cda79eca: im RE2-INIT, auf RE2-Slots umgesetzt) | ja | Zeichner liest Skala 0x400 und Vertex-Tinte 0x80 flavorblind (main.c:10795/10967) |
| 4b84afcc8 | RE2-Granaten-Tod zerreisst (8BEC) | ja (Sonde Zeile 9: p40 0x01300, fx 32) | nur mit RE2-Import-Option (RE1.5 hat 5c41aeb22) | — |
| 8cda79eca | Zerfetzen + Spawn-Variante im RE2-Gehirn | ja (`unit_re2_gore80`) | — | — |

### 5. Je Typ mit RE2-Gehirn: RE1.5-Gore, RE2-Gore, Standard vorher, Stand nach Spur S

| Typ | RE1.5-Gore (Disc) | RE2-Gore (Retail) | Standard vorher | Spur S | Am Modell gescheitert / offen |
|---|---|---|---|---|---|
| Zombie-Familie 0x10-0x13/0x16/0x18 | Kopf, kopflos, Zerfetzen, Oberkoerper, Beine (w7/8/19 nach unten), Spawn-Varianten | 92C4, 66FC, 7438, 8BEC, 9610, Arm-Abwurf, Brand/Saeure | alles ausser MC51-Beine | **MC51 nach unten -> 7438** (`re2z_death_zelle`, Zeile 18, Spalte % 3 == 0, Schalter RE15_KEIN_80PROZENT_GORE); Riegel J | Ganz-Zerplatzen durch Granate (Video Z08/Z11/Z24) hat weder RE1.5 noch RE2 als Mechanismus -> Gestaltung nach Bild, nicht gebaut (offen) |
| Hund 0x20 | Blut/Lache (keine Teile; Sonde RE15 teile=0) | **Teile-Wurf 0x80104440**: 7 Parts {2,3,4,7,8,9,10} @0x80105680 driften dunkel weg (Zeilen 5,6,9,17,19) | Felder gesetzt, **nicht gezeichnet** (Bruecke kannte 0x20 nicht; Sonde bridge=0) | **Bruecke fuer 0x20**, beim Wurf geseedet (`re15_gore_r41_hund_bruecke`); Riegel K | RE2-Slots 7/10 haben kein RE1.5-Mesh (k_perm_dog -1): diese zwei Teile zeichnen nichts. Bild aus dem Fenster nicht gelungen (die Kamera zeigt den Tod nicht) -> offen |
| Kraehe 0x21 | GIB 13 Knochen @0x80114a50 (versteckt nur) | GIB 0x80102CA0, 13 Parts fliegen (Bruecke seit R36) | ja (Sonde p40 0x01fff) | — | Feder-Sprite (Video K01) ohne Mechanismus in beiden -> offen |
| Spinne 0x25 | Krit-Bein, Platzen (Waffentabelle), Beine fliegen | Bein ab (FUN_80105BF0), Hinterleib (FUN_80104890), Babys | nur Babys + ESP | **alles gehoben** (Teil-Schritt 4) | RE2-Partikel FUN_801056DC an Teil-Matrizen: der Port spawnt am Ursprung (`re2s_gore`) -> offen |
| Babyspinne 0x26 (nur RE2-Spawns) | — (RE1.5-0x26 = Feuer-Emitter) | kein Teil-Gore im Modul | — | — | — |
| Zellenarm 0x1A | Hand ab (0x8010D5D0) | keiner | nein | — | Hand-ab gibt es nur im RE1.5-Writher; der laeuft nie (Entscheidung R35 H M1) -> offen, Nutzerfrage |
| Alligator 0x23 | (Spur B) | Kiefer platzt (R41 Spur B, Bruecke) | ja | nicht angefasst | — |
| Gorilla 0x27, 0x29, Birkin 0x30/0x36 | eigenes RE1.5-Gore | kein RE2-Gehirn | ja (RE1.5 in beiden Flavors) | — | — |

Hund, Byte-Beleg (RE2 EMD0G_MOD0.BIN, `re2_disasm.py dis 0x80104440 --bin EMD0G_MOD0.BIN`):
```
80104470: lbu v1,22144(at)   ; Tabelle @0x80105680 = 02 03 04 07 08 09 0a
801044a4: ori v0,v0,0x4a / 801044a8: sw v0,0(v1)
801044b4: sh zero,160(v1)  801044b8: sh t2(800),156(v1)  801044bc: sh t1(-150),154(v1)
801044c0: sh t0(10),158(v1) 801044c4: sh a3(-100),164(v1) 801044c8: sw a2(0x101040),112(v1)
801044cc: sh v0(+0x76),152(v1)   801044d0: sltiu v0,a1,0x7
```
MC51, Byte-Beleg (RE1.5 STAGE1.BIN, Mod = Disc, `read 0x8011feac 8 --rows 22 --rowstride 32`):
```
0x8011ff8c: [ 7] [0x80108abc, 0x80106edc, 0, 0, 0x80107634, 0x80107244, 0, 0]
0x8011ffac: [ 8] [0x80108abc, 0x80107ee0, 0, 0, 0x80107634, 0x80107244, 0x80106c18, 0]
0x8012010c: [19] [0x80108abc, 0x80106c18, 0, 0, 0x80107634, 0, 0, 0]
```

### 5b. Was die RE2-Gehirne ueber Angriff und Treffer hinaus ersetzen (nur berichtet, nicht umgebaut)

| Typ | Ersetzt durch RE2 (RE1.5-Inhalt fehlt unter Standard-KI) | Beleg |
|---|---|---|
| Spinne 0x25 | **Klettern an Wand/Absatz/Decke** (Runde 42, 0x801118a8 + ACT 9/10/12/13) laeuft nur im RE1.5-Gehirn; die RE2-Oberflaechenmodi 1/2/3 sind unter den RE1.5-Raumdaten unerreichbar (beide Deskriptoren 0x00/0x41 -> Modus 0): unter Standard klettert die Spinne nie. Toene: RE1.5-Raum-SE 2/5 (Treffer/Tod) statt RE2-ENEMSE-Bank 11. HP-Tabelle RE1.5 (`s_aspider_hp_row`) statt RE2 @0x801062C4 (gemessen: RE2 110/111, RE1.5 80/129). Tod: RE1.5 Clip 4 bzw. 1 + Leichen-Verblassen statt RE2 Clip 12/13. | include/re15_spinne_r42.h:4; enemy_ai_re2_spider.c Block "WARUM NUR MODUS 0"; Messung Teil 1 |
| Zombie-Familie | RE1.5-Trefferreaktionen je Waffe (b2ca28b82), RE1.5-Entscheidungszustaende (Tabelle @0x8011f840, u.a. Fressen/Schlafen) ausser dem Sitz-/Aufsteh-Import, RE1.5-Schritt-Toene (Raum-SE-Maske) statt ENEMSE. Brennende Zombies bleiben (`re15_brand_re2_takt`). | enemy_ai_common.c:10916-10926, :14855-14870 |
| Hund 0x20 | alles ausser den Skript-Zustaenden 4/5/6 (Spawn-Drop, Kill-Maschinen), die bleiben RE1.5 | enemy_ai_common.c:7832-7845 |
| Kraehe 0x21 | alles ausser Ereigniskraehe (R38 I) und Fensterkraehe ROOM1120 (R35 M, dort erzwungen RE2) | enemy_ai_common.c:6912-6926 |
| Zellenarm 0x1A | in jedem Flavor RE2: der RE1.5-Writher (Bewegung, Hand-ab-Tod) ist ganz weg | enemy_ai_re2_zombie.c:172-179 |
Die Spalte stammt aus dem Code und den Besitz-Toren; gemessen ist davon nur der Besitz (Sonde) und die HP der Spinne.

### Bau Schritt 5 (zweiter Commit) und Riegel

- `enemy_ai_re2_zombie.c re2z_death_zelle`: Zeile 18 (MC51), Beinzone (Spalte % 3 == 0) -> 7438, nur mit dem
  80%-Schalter (RE15_KEIN_80PROZENT_GORE schaltet ab, wie die Granaten-Zeile 9).
- `enemy_ai_re2_zombie.c re15_re2z_gore_active`: Typ 0x20 nimmt die Part-Bruecke, sobald Record 0 Bit 0 traegt;
  `enemy_ai_re2_dog.c re2d_teile_wurf` seedet sie vor dem Wurf (`re15_gore_r41_hund_bruecke`, spinnengore_r41.c).
  Nicht am Schalter: das ist RE2-eigenes Gore, das bisher nur nicht gezeichnet wurde (wie Kraehe R36 / Alligator R41 B).
- Riegel `unit_r41_spinnengore` erweitert: J (Rohzelle @0x8010CEAC Spalte 0 = MAIN; Zelle mit Hebung = 7438;
  Rumpfzone bleibt MAIN), K (RE2-Hund Redhawk-Tod: Bruecke vor dem Wurf aus, danach an, 7 Parts 0x4b, w9c 800,
  Seed Part 0 = 1 / 0x808080).
- **Gegenprobe ohne diesen Bau** (enemy_ai_re2_zombie.c, enemy_ai_re2_dog.c auf den Stand des ersten Commits):
  **10 Fehler** (J: Zelle 1 statt 2; K: Bruecke aus, 7 Parts ohne Bit 0, kein Seed). Danach wiederhergestellt: OK.
- Weitere Zielauswahl gruen: 80 Unit-Tests `dog|hund|re2_gore|zombie_gore|re2z|weapon_rows|r36_granate|
  r34_reaktion|r35_redhawk|plattform|kraehe|crow`, integration_r35_redhawk (Hund + Redhawk, ESP-Zahl),
  integration_r41_spinnengore.
- Bildbeleg `S_belege/s04_re2ki_schrot_bein_platzt_2070_leon.jpg`: Remington, Aufnahmebild 117: Muendungsblitz,
  gelbgruene Beinstuecke links an der Spinne, rotes Blut; 118-121 die Spinne laeuft mit einem Beinpaar weniger.
  Gefunden ueber eine Pixelzaehlung (gelbgruen) ueber alle Aufnahmebilder: nur Bild 117 im Schrot-Lauf.
- Hund: Bild aus dem echten Fenster **nicht** gelungen. Auf dem Weg des Riegels r35_redhawk (ROOM11D0, Leon
  (-7000,-15900)) zeigt die Kamera den Tod nicht (die Hunde laufen aus dem Bild). Nur Unit-Beleg K und Sonde.

## Ergebnis Spur S (Stand 2026-11-01, Commits ebfd10c31 und 9fb1c73e2 auf r41/spinnengore)

Antwort auf die Nutzerfrage: Der Gore-Zweig war vollstaendig in master (0 fehlende Commits, Merge b539a5337).
Das Platzen der Spinne lag aber im RE1.5-Gehirn, und unter der Standard-KI besitzt das RE2-Gehirn jede 0x25.
Gemessen: Das Krit-Bit kam an (+0x93 = 0x41), die Teilschicht blieb aus. Deshalb platzte die Spinne nicht.
Jetzt platzt sie unter der Standard-KI bei jedem toedlichen Treffer mit Magnum, Schrot, SPAS, Granaten, Rakete und
Krit, im RE1.5-Original belegt und dort ohne Zufall (Tabelle @0x80118f38 -> 0x80113ff8). Rumpf und Hinterleib
zerplatzen gelbgruen, alle Beine fliegen, Blut kommt an Rumpf und Hinterleib. Beine fallen bei jedem
Bein-Abtrennen von RE2 und bei Krit. Belegt mit Bildern aus dem Fenster (Leon, Elza, Gegenprobe).

## Offen (benannt, nicht gedeutet)

1. **Wuergearm 0x1A, Hand fliegt ab (661a36e59, 0x8010D5D0)**: Das ist in KEINEM Flavor erreichbar. 0x1A laeuft
   immer auf dem RE2-Zellenarm (Entscheidung Runde 35 Spur H M1), der keinen Teil-Wurf hat. Ob die RE1.5-Hand in den
   Zellenarm gehoben werden soll, ist eine Nutzerentscheidung. Teilnummern und Zeitpunkt (der RE2-Arm hat keinen
   RE1.5-Tod) muessten neu belegt werden.
2. **Zombie zerplatzt durch Granate ganz** (Video Z08/Z11/Z24): Kein Mechanismus in RE1.5 oder RE2 laesst den Rumpf
   verschwinden. 8BEC (Rumpf bleibt) ist das Staerkste vorhandene. Ein Nachbau waere Gestaltung nach Bild. Nicht gebaut.
3. **Spinne, Waffe in Video S01 (B 5:18.3)**: Pistole oder Magnum ist nicht zu erkennen. Mit Handfeuerwaffen platzt
   die Spinne weder im RE1.5- noch im RE2-Original. Sollte das Video eine Pistole zeigen, waere das Platzen bei
   Pistolen Gestaltung nach Bild. Nicht gebaut.
4. **RE2-Partikel FUN_801056DC** (Spinne, alle `re2s_gore`-Stellen) sitzen im Port am Aktor-Ursprung statt an der
   Teil-Matrix (+0x198 + 172*Teil). Die RE1.5-Blutsaetze der Hebung sitzen am Teil (`re15_enemy_fx19700`).
5. **Hund-Teile-Wurf**: Bild aus dem echten Fenster fehlt (Kamera auf dem r35-Weg zeigt den Tod nicht). Unit K und
   Sonde belegen den Mechanismus. RE2-Slots 7/10 haben kein RE1.5-Mesh (k_perm_dog -1).
6. **Kraehe, Feder-Sprite** (Video K01): Mechanismus fehlt in beiden Quellen, offen wie in S_video_gore.md.
7. **RE1.5-Spinnentod der schwachen Waffen** (0x8011461c, Clip 4, Phasen @0x80114b78-e04): Der Port nimmt weiter
   Clip 1. Das ist eine vorbestehende Verkuerzung, kein Gore.
8. **Beta-Eigenheit, nicht geaendert**: Fliegende Teile, die mit vy 160..167 auf y = 0 aufschlagen, verschwinden
   (FUN_8001f024 @0x8001f104-134). Einige Oberschenkel verschwinden nach der Landung, die Unterschenkel bleiben.
9. **Nutzerentscheidung 5b**: Was die RE2-Gehirne ueber Angriff/Treffer hinaus ersetzen (Tabelle 5b), vor allem
   das Klettern der Spinne (Runde 42), das unter der Standard-KI nie laeuft.
10. **Colt Python (w20) bei der Spinne**: Die RE2-Zeilenkarte der Spinne fuehrt w20 noch auf Zeile 13 (generisch),
    die des Zombies seit Runde 35 auf 5 (Magnum). Das Gore deckt die Hebung ueber das Krit-Bit ab. Die Trefferzeile
    ist nicht angeglichen.
11. Nicht gemessen in dieser Spur: Birkin-Wurf-Teile (c1469a3ab) und Blut an Teil 0 bei 0x29 im Echtlauf. Belegt
    ist nur der Besitz (kein RE2-Gehirn).

## Nachbesserung 1 (nach Abnahme 1, 2026-11-01)

Mängel aus `S_abnahme_1.md`: (1) Spinnenblut rot statt grün, (2) Offen-Liste deckt `S_video_gore.md` §4 nicht ab,
(3) Kommentar „24 Worte“ und die MC51-Folge für die Release Notes. Werkzeuge und Rohbilder: `scratchpad/S_nb1/`
(`cap.ps1` = eigene Messkopie `re15_pc_S41nb1.exe`, echter Titel und echte Spielerwahl über `RE15_TITLE_CONFIRM_MS=1500`
und `RE15_PSELECT_AUTO`, dann `RE15_DEBUG_JUMP`; beschleunigter Renderer, kein AUTOSHOT, kein SOFTWARE_RENDER, gdigrab
am Fenster-Handle der eigenen PID, 10 Bilder/s, `RE15_WINDOW_SCALE=2`). Belege: `S_belege/nb1/n01…n05`.

### N1. Neue Messschiene `RE15_FX_CLUT_LOG` (kein Verhalten)

`platform/pc/main.c`, Zeichenschleife der Effekte: je gezeichnetem Platz eine Zeile
`F=<Bild> id= sub= skala= clut= cw= slot= w(x,y,z)`. `sub` ist Platz +0x71, `skala` +0x72, `clut` +0x32 so, wie der
Zeichner es rechnet (`re15_pc_esp_clut`), `cw` ist das Wort, das er wirklich bindet. Eigene Datei, weil die Auswerter
von `RE15_FX_LOG` ` F=<n>` am Zeilenende verankern (`r34_plattform.cmake:142`, `test_r36_granate.cmake:100`,
`test_r41_restpunkte_effektquader.cmake:181`). Ein erster Versuch, die Felder an `RE15_FX_LOG` anzuhängen, ist deshalb
verworfen.

### N2. Messung VORHER (Stand 4e7eb80cd + nur die Messschiene)

Lauf `v_a_leon_w7`: Leon, ROOM2070, Standard-KI, Redhawk, derselbe Weg wie Integration L7.
- Spinnen-Log wie in der Abnahme: F68 Slot 2 Zeile 5, +0x93 = 0x41, ta=1, sh=80001, fl=7fff8; F122 Slot 1 ebenso.
- CLUT-Log am Tod, je Bild F68–78 und F122–132: **3× `id=0 sub=0 skala=2000 clut=7951`** (die drei Blutsätze
  `0x82000` der Hebung an Teil 0, 0 und 19, das Sub-Byte 8 ist weg) und **2× `id=0 sub=0 skala=1500 clut=7951`** (der
  Stand-in `re2s_gore` des RE2-Gehirns, Zeile 5 @0x80104580 und FUN_80104890 @0x80104588). Alle fünf auf CLUT 0x7951 =
  TEX.TIM y 485 = **rot**.
- Bild: Aufnahmebilder 321–324, rote Wolke am platzenden Rumpf (`n01`, obere Reihe). Im Wolkenfeld (Box 150,300–240,390)
  bis 943 rote und 0 grüne Pixel.

Daraus folgt: Das `& 7` zu beheben, reicht nicht. Zwei der fünf Partikel am Tod und **jeder gewöhnliche Treffer** unter
der Standard-KI (@0x80102DD0) kommen aus dem RE2-Stand-in mit dem Zombie-Code `0x1500`.

### N3. RE (alle RE1.5-Stellen in Mod und Disc gelesen, `scratchpad/S_nb1/dis2.py`, Abweichungen 0)

**FUN_80019700, PSX.EXE**: Das ganze Sub-Byte geht in den Platz, nur die Strom-Wahl maskiert.
```
8001970c srl  v0,a0,16
80019730 andi t7,v0,0xff        ; Sub = ganzes Byte
80019734 andi v0,t7,0x7         ; Strom-Index (Zeilentabelle (sub&7)*2)
8001973c srl  v1,t7,3
80019754 sll  s0,v1,6           ; (Sub >> 3) * 0x40
800197d8 sb   t7,113(t0)        ; Platz +0x71 = GANZES Byte
800197dc sh   s1,114(t0)        ; +0x72 = Skala
8001987c lhu  v0,4(t5)          ; EFF-Kopf +4 = CLUT-Wort
80019884 addu v0,v0,s0
80019888 sh   v0,50(t0)         ; +0x32 = CLUT
```
**Spinnenblut-Code RE1.5, STAGE2.BIN:**
- HURT, jeder Treffer: `lui a0,0x8` @0x80113b10, `ori a0,a0,0x2000` @0x80113b30, `jal 0x80019700` @0x80113b50.
- Tod der schwachen Waffen: `lui a0,0x8` / `ori a0,a0,0x2000` @0x8011463c-40, `jal` @0x80114654.
- a0 = 0x00082000 = Effekt 0, Sub 8, Skala 0x2000.

**Paletten** (`n05`): RE1.5 CORE00.ESP Effekt 0 hat CLUT-Wort 0x7951 (x 272, y 485). TEX.TIM y 485 = `601010 580808 …`
(rot), y 486 = `185008 184808 …` (grün). Also ist die Spinne y 486 und der Zombie y 485.

**RE2 Retail, was das RE2-Gehirn eigentlich zeichnen würde:**
- FUN_801056DC (EMS25.BIN) baut a0 = `(rec[0]&0x7f)<<24 | rec[1]<<16 | u16 rec[4]` (@0x80105860-878) und ruft
  FUN_8001bf10 @0x80105874. Die Records @0x80106684 (je 6 Byte):
  - Rec 0 `00 19 00 00 00 20`, Rec 1 `00 19 .. 00 28`, Rec 3 `00 09 .. 00 10`: Effekt 0 (Blut).
  - Rec 2 `09 0a ..` (Effekt 9), Rec 4/5 `85 ..`, Rec 6/7/10 `84 ..`, Rec 8 `1a ..`, Rec 9 `86 ..`: kein Effekt 0.
- FUN_8001bf10 (RE2 PSX.EXE) zerlegt wie RE1.5: `andi v0,t5,0x7` @0x8001bf24, `srl v0,t5,3` / `sll v0,v0,6`
  @0x8001c01c-20, `addu v1,v1,v0` / `sh v1,50(t0)` @0x8001c034-38.
- RE2 CORE00.ESP Effekt 0 hat CLUT 0x7a11 = y 488 = `880000 780000 …` (rot, Grundzeile).
  - Spinne Rec 0/1 (Sub 0x19) = y 491 = `888840 808038 …` (oliv-gelb).
  - Spinne Rec 3 (Sub 0x09) = y 489 = `386030 305028 …` (grün).
  - `shared_assets/RE2/CORE00.ESP` und `TEX.TIM` sind byte-gleich mit `info/re2leon/COMMON/DATA/`.
- Record je Aufruf, aus den Bytes (`scratchpad/S_nb1/scan_jal.py`, `a2` vor dem Aufruf bzw. im Delay-Slot):

| Record | Aufrufstellen |
|---|---|
| Rec 0 | @0x80102DD0, @0x80103DE4, @0x80103F60, @0x80104488; dazu @0x80102EE0/F5C und @0x801042A0 (Modus 1–3, im Port nicht portiert) |
| Rec 1 | @0x80104580, @0x80104694, @0x801047BC, @0x80104840, @0x801048FC, @0x80105D00, @0x80105D18 |
| Rec 2 | 4× @0x8010490C (in FUN_80104890 nach Rec 1) |
| Rec 3 | @0x80103754, @0x80103784 |
| Rec 4/5 | @0x80103890, @0x8010395C, @0x80103C04, @0x801049A0-A0C, @0x80104AC0-B2C |
| Rec 6 | @0x80104C10/C20 |
| Rec 7 | @0x80103AA0, @0x80103E70, @0x80104058, @0x8010441C, @0x80104618, @0x80104C00 |
| Rec 10 | @0x80103B3C, @0x80104CA4/CB4 |

- **Babyspinne, EMS26.BIN** FUN_80100F28. Records @0x801010dc: Rec 0 `00 19 .. 10`, Rec 1 `00 01 00 08 00 08`,
  Rec 2 `1a 00 ..`, Rec 3 `05 04 ..`.
  - Biss @0x80100EDC = Rec 1 (`addiu a2,zero,1` @0x80100ee0), Sub 1, also die rote Grundzeile. Das ist **Leons Blut**
    und bleibt rot.
  - Der „Platscher“ @0x80100AD0 ist Rec 2 (`addiu a2,zero,130` = 0x80|2), Effekt 0x1a, kein Blut.

Schluss: In RE1.5 ist das Spinnenblut grün (y 486). In RE2 ist es nie die rote Grundzeile (y 491 oliv-gelb, y 489 grün).
Im Video ist es grün (S01/S02/S05). Der Port nahm an beiden Stellen die rote Zombie-Zeile.

### N4. Bau

- `engine/src/re15_damage.c re15_enemy_fx19700`: `(a0 >> 16) & 0xff` statt `& 7` (@0x80019730, @0x800197d8).
  - Der Zeichner (`re15_pc_esp_clut`, `(sub & 0xff) >> 3`) und die Strom-Suche (`re15_esp_row_stream`, `sub & 7`)
    zerlegen das Byte selbst.
  - Die Zombie-Codes 0x1500/0x2000/0x2500 haben Sub 0 und bleiben unverändert.
- `engine/src/zombiegore_r15.c fx()`: dieselbe Zeile. Das ist ohne Wirkung (alle Codes dort haben Sub 0), nur byte-true.
- `engine/src/enemy_ai_re2_spider.c`: neuer Stand-in `re2s_blut` mit dem RE1.5-Spinnencode `0x00082000`
  (@0x80113b50/@0x80114654).
  - Er gilt für die RE2-Blut-Records 0, 1 und 3, an 14 Stellen (Liste N3). FUN_80104890 = Rec 1 + 4× Rec 2 bleibt ein
    Aufruf; Rec 2 ist wie bisher nicht portiert.
  - Alle anderen Records und die Babyspinne behalten `re2s_gore` (`0x1500`).
  - **PORT-WAHL (Stand-in):** welcher RE1.5-Code die RE2-Records vertritt. Belegt ist die Richtung.
  - Der Anker bleibt der Aktor-Ursprung (Offen 4).
  - Hängt nicht am Schalter `RE15_KEIN_80PROZENT_GORE`. Das ist keine 80%-Sache, sondern die Farbe des vorhandenen
    Stand-ins.
- Riegel `unit_r41_spinnengore` Teil L (bindet `platform/pc/src/fx_plattform_pc.c` ein wie `probe_r34_plattform`):
  - L1 Byte-Pins: PSX.EXE @0x80019730/34/3c/54/d8/84/88, STAGE2 @0x80113b10/30/50 und @0x8011463c/40/54, EMS25 Rec 0/1/3
    und `jal 0x801056dc` / `addu a2,zero,zero` @0x80102dd0-d4.
  - L2 Paletten: RE1.5 0x7951, y 485 rot, y 486 grün; RE2 0x7a11, y 488 rot, y 489/491 nicht rot.
  - L3 Helfer: 0x82000 ergibt Sub 8 und Zeichner-CLUT 0x7991, 0x2000 ergibt 0x7951.
  - L4 RE2-Treffer w3 ist grün, kein 0x1500.
  - L5 RE2-Tod w7: 5 grüne Partikel.
  - L6 Schalter aus: 2 grüne Partikel.
  - L7 RE1.5-Tod w3 ist grün.
- **Gegenprobe:** `re15_damage.c` und `enemy_ai_re2_spider.c` auf HEAD zurückgesetzt und gebaut: **5 Fehler**.
  - L3 „gruen 0 rot 1“, L4 „rot 1 alt-0x1500 1“, L5 „gruen 0 rot 5 alt-0x1500 2“, dazu L6 und L7.
  - Danach wiederhergestellt und gebaut: `OK unit_r41_spinnengore`.

### N5. Messung NACHHER (exe 2026-11-01 19:56, dieselben Wege)

| Lauf | CLUT-Log | Bild |
|---|---|---|
| `n_a_leon_w7` Leon 2070 Redhawk | F68–78 und F122–132 je **5× `id=0 sub=8 skala=2000 clut=7991`**, kein 0x1500. Ein einzelner `sub=0 skala=1500 clut=7951` in F83–93 an w(4929,-1280,-6337): In F83 fällt Leons HP von 80 auf 79 (Babybiss, Leons Blut, richtig rot) | Bilder 381–384: grüne Wolke am Rumpf (`n01`, untere Reihe). Im Wolkenfeld höchstens 18 rote und bis 254 grüne Pixel |
| `n_b_elza_w7` Elza 2071 Redhawk | F95 / F176 Tod wie bei Leon, 110× Sub 8 / 0x7991. Einziger 0x1500 in F118 an Elza, ihre HP fallen von 80 auf 79 (Babybiss) | Bilder 397–400: grüne Wolke hinter der Spinne (`n02`). Das Rot im Feld sind Elzas Hosen |
| `n_d_leon_w3` Leon 2070 Browning | Treffer F70/95/122/149/176 (Zeile 3, @0x80102DD0) je **1× Sub 8 / 0x7991**, kein 0x1500 im ganzen Lauf | Bilder 393, 405, 443: grüner Spritzer am Fußpunkt der Spinne (`n03`; Anker = Ursprung, Offen 4) |
| `n_e_re15_w8` RE1.5-KI Remington | 143× Sub 8 / 0x7991, kein Rot | Bild 260/261 grünes Treffer-Blut, Bild 296 Krit-Bein mit grünem Blut (`n04`) |
| `n_c_leon_w7_aus` Schalter aus | Tod F68/F122: ta=0 (kein Platzen), je **2× Sub 8** (`re2s_blut`). 55 rote 0x1500-Zeichnungen in F83, 319, 335, 344/345 und 357: Jedes Mal fällt Leons HP um 1 (80 auf 75), das sind Babybisse | — |

~~Damit ist jeder Spinnen-Blutsatz beider Gehirne grün (TEX.TIM y 486). Rot bleibt nur Leons und Elzas eigenes Blut beim
Babybiss (RE2 EMS26 Rec 1, Sub 1 = Grundzeile).~~

**Berichtigt in Nachbesserung 2 (Abnahme 2, Mangel 2):** Der Satz war falsch. Der Tod der RE2-Babyspinne
(EMS26 @0x80100d10-14, Record 0 = Spinnenblut) lief mit diesem Stand weiter als rotes `0x1500`. Seit Nachbesserung 2 ist er
grün (Abschnitt NB2.4). Danach gilt: Jeder Spinnen-Blutsatz beider Gehirne ist grün, Adult und Baby. Rot bleibt nur Leons
und Elzas eigenes Blut beim Babybiss (EMS26 Record 1, Sub 1) und die RE2-Records, die kein Effekt-0-Blut sind
(Offen 4).

### N6. Tests (Zielauswahl, Windows)

- Unit, 58/58 grün:
  - die 41 Tests der Abnahme (Spinne, Babys, Zombie-Gore, Trefferfilter, Hund, Krähe, Teile);
  - dazu `unit_r34_plattform`, `unit_esp_*`, `unit_r38_heliport1171`, `unit_r35_granate`, `unit_birkin_opfer` und
    `unit_r38_affen*` (Nutzer von `re15_enemy_fx19700` und von Effekt 0).
- Integration, einzeln nacheinander, 6/6 grün:
  - `integration_r35_redhawk` 16,7 s, `integration_r38_spinnen_echtlauf` 116,2 s, `integration_r41_spinnengore`
    122,8 s, `integration_r41_spinnenanim` 111,2 s;
  - `integration_r34_plattform_takt` 22,8 s und `integration_r34_plattform_esp_eintritt` 9,5 s, beide wegen der
    Änderung in der Zeichenschleife von `main.c`.

### N7. Mangel 3

- Kommentar `re15_spinnengore_platzwaffe`: Die Tabelle hat 22 Einträge (Waffen-Ids 0..21). Wort 22 @0x80118f90 =
  0x80114e60 ist schon Eintrag 0 der nächsten Waffentabelle: `lbu v0,5(v0)` @0x80114e28, `lui at,0x8012` /
  `addiu at,at,-28784` @0x80114e34-38, `jalr` @0x80114e48 (Mod = Disc).
- MC51-Folge nachgelesen (RE2 EMZ0.BIN, Weg 7438 mit `death=1`, Phase 2):
  - `andi v0,v0,0x3` / `bne` @0x80107794-98.
  - Nur bei `rand & 3 == 0` **und** `lb v0,363(s2)` != 0 (@0x801077a0-a8) wird er zur Leiche.
  - Sonst `addiu v0,zero,10` @0x8010779c / `sh v0,342(s2)` @0x801077b0: HP 10 und Kriecher.
  - RE1.5 0x80108abc: `ori v0,zero,0x1e` / `sh v0,154(v1)` @0x80108ccc-d0 = HP 30 (STAGE1, Mod = Disc).

### Für die Release Notes (Baustein für den Release-Schritt)

**Spinnen: Das Blut ist jetzt grün.** Im Port war das Spinnenblut rot, weil die Farbwahl des Original-Effekts verloren
ging (FUN_80019700: Effekt-Byte `(a0>>16)` @0x80019730, Farbzeile `(Sub>>3)*0x40` @0x8001973c-88). Jetzt ist es grün
wie im Original RE1.5 (TEX.TIM Zeile 486) und in den 80%-Videos (B 5:18, 5:31, 7:40). Das gilt beim Platzen, bei jedem
Treffer und bei abgeschossenen Beinen, unter der Standard-KI und unter der RE1.5-KI. Unter der Standard-KI vertritt der
grüne RE1.5-Spinnencode die RE2-Spinnenpartikel, die der Port nicht zeichnen kann (in RE2 sind sie oliv-gelb bzw. grün,
nie rot). ~~Rot bleibt nur Ihr eigenes Blut, wenn eine Babyspinne Sie beißt.~~ *(Berichtigt in Nachbesserung 2: der Satz
stimmte erst, nachdem auch der Tod der Babyspinne grün spritzt. Gültiger Baustein unten unter NB2.7.)*

**Zombies, MC51 auf die Beine (Folge der Gore-Hebung).** Unter der Standard-KI tötet ein tödlicher MC51-Treffer in die
Beine (Tief-Zielen) meist nicht mehr. Die Beine platzen, und der Zombie kriecht mit 10 Lebenspunkten weiter (RE2-Weg
7438, `sh 10` @0x801077b0). Liegen bleibt er nur mit 1:4, und nur, wenn RE2s Byte +0x16B gesetzt ist (@0x80107794-a8).
Das deckt sich mit RE1.5, dort kriecht er mit 30 Lebenspunkten weiter (@0x80108ccc-d0). Mit `RE15_KEIN_80PROZENT_GORE=1`
gilt der alte Sturz.

### Offen, Stand nach Nachbesserung 1 (ersetzt die Liste „Offen“ oben)

Nach dem Auftrag gilt je Typ das Maximum aus RE1.5, RE2 und den Videos. Ob ein Merkmal, das nur die Videos zeigen,
gebaut wird, ist PORT-WAHL. Hier steht alles, was `S_video_gore.md` §4 zeigt und der Port nicht hat.

Spinne 0x25:
1. **Grüne Flecken, die nach dem Platzen gut 1 s auf Boden und Wand liegen bleiben** (S01, B 5:18.3, g09).
   - Die Farbe hat der Port jetzt, aber keine liegenbleibenden Flecken: Die ESP-Partikel laufen ihre Zeilen ab und
     verschwinden.
   - ~~Einen Flecken-Mechanismus für die Spinne habe ich in RE1.5 und RE2 nicht gesucht, nur die Effekt-0-Partikel.~~
   - ~~Gestaltung nach Bild, nicht gebaut.~~
   - **Berichtigt in Nachbesserung 2 (Abnahme 2, Mangel 1):** Die Einordnung "nur Video" war falsch. Die LACHE unter der
     Spinnenleiche ist in RE1.5 (CORPSE 0x801153d4, Tinte 0x00DF809F @0x8011546c-70, +8 je Takt @0x801154a8-b8) und in
     RE2 (FUN_80104DE8, Tinte 0x00BF10BF @0x80104e7c-80, +8 je Takt @0x80104ed0-dc) belegt, dazu der Fleck der
     Babyspinne (EMS26 @0x80100cb4-e0). Alle drei sind seit Nachbesserung 2 gebaut (Abschnitt NB2). Offen bleiben davon
     nur die Wandflecken aus dem Video (keine Wand-Decals in RE1.5 und RE2, Gestaltung nach Bild, nicht gebaut).
2. **Grüne Treffer-Spritzer am getroffenen Körper** (S02 B 5:31–5:32.5, S05 B 7:40.4).
   - Die Farbe ist jetzt grün (N5, `n03`).
   - Sie sitzen aber am Aktor-Ursprung (Fußpunkt), nicht an der Teil-Matrix. RE2 FUN_801056DC spawnt an
     `self+0x198 + 172*Teil` (@0x80105758-760), RE1.5 an Teil 0 (`lw v1,392(v0)` @0x80113b48, `addiu a2,v1,64` @0x80113b54). Siehe Punkt 4.
3. Waffe in S01: Pistole oder Magnum ist nicht zu erkennen (wie bisher Offen 3).
4. RE2-Partikel sitzen am Ursprung statt an der Teil-Matrix, an allen `re2s_gore`- und `re2s_blut`-Stellen (wie bisher
   Offen 4). Die RE2-Records 2/4/5/6/7/10 (Effekte 9, 5, 4, 6, 0x1a) werden weiter als rotes `0x1500`-Blut vertreten,
   obwohl sie in RE2 kein Effekt-0-Blut sind. Nicht angefasst, weil ihr Aussehen nicht belegt ist.
5. RE1.5-Spinnentod der schwachen Waffen nimmt Clip 1 statt Clip 4 (wie bisher Offen 7).
6. Beta-Eigenheit: Oberschenkel verschwinden nach der Landung (wie bisher Offen 8).
7. Nutzerentscheidung 5b: Klettern unter der Standard-KI (wie bisher Offen 9).
8. Colt Python (w20) in der RE2-Zeilenkarte der Spinne (wie bisher Offen 10).

Zombie-Familie 0x10–0x13/0x16/0x18:

9. **Ganzes Zerplatzen durch Granaten** (Z08 B 3:31.4, Z11 B 3:36.0, Z14, Z24 B 8:42.2). Im Port bleibt der Rumpf
   (8BEC). Gestaltung nach Bild, nicht gebaut (wie bisher Offen 2).
10. **Kopf weg und kopflos weiter mit einer Nicht-Magnum-Waffe** (Z05 B 2:28.4, Z06 B 2:34.9, beide wahrscheinlich;
    die Waffe ist wahrscheinlich Schrot, nicht sicher).
    - Im Port läuft der Zombie kopflos nur nach der Magnum-Zeile 5 (92C4 P4, `re2z_death_magnum`).
    - Mit Schrot verliert er den Kopf nur im 80%-Zerfetzen (Zeile 7 Spalte 1, Krit und rand&1), läuft dann aber nicht
      weiter.
    - Erst die Waffe sichern, dann PORT-WAHL.
11. **Wandspritzer, die an der Wand bleiben** (Z28, Trailer B 10:00.8–10:01.5, Schrot-Nahschuss, wahrscheinlich). Der
    Port hat keine Wand-Decals, nur die Wund-Decals des Spielers (`re15_damage.c:115`). Gestaltung nach Bild.
12. **Große Blutlache unter einem noch lebenden, liegenden Zombie** nach Granatentreffern (Z23 B 8:10.3–8:25). Im Port
    wächst die Lache nur an der Leiche (`re2z_corpse_pool_grow`). Gestaltung nach Bild.
13. **Brennende Leiche brennt weiter** (Z19 B 12:59–13:01, CD4 0:48.5–0:51). Am Port nicht geprüft.
    - Der Brand-Tod (`re15_brand_tod`, 0x80108d74) setzt die acht Flammen auf Routine 22 und geht in die Leiche
      (`sh 7,4` @0x8010915c-60).
    - Ob die Flammen dort weiterlaufen, zeigt erst ein Fensterlauf. Nicht als fehlend gezählt.
14. Würgearm 0x1A, die Hand fliegt ab: in keinem Flavor erreichbar (wie bisher Offen 1, Nutzerfrage).

Krähe 0x21:

15. **Schwarze Fetzen und Federn, die liegen bleiben** (K01 B 1:11.8–1:15.8). Der Mechanismus fehlt in beiden Quellen
    (wie bisher Offen 6).

Hund 0x20 und Birkin:

16. Hund: RE2-Slots 7/10 haben kein RE1.5-Mesh (wie bisher Offen 5; das Bild hat die Abnahme 1 geliefert, `abn1/a04`).
17. Birkin-Wurf-Teile und Blut an Teil 0 bei 0x29 sind im Echtlauf nicht gemessen (wie bisher Offen 11).

Nebenbei, nicht von diesem Bau: Der Kommentar über `re2z_hit_knockdown` nennt Spalte 0 noch „im Spiel unerreichbar“.
Das ist seit Runde 16 überholt (Tief-Zielen trifft die Beinzone). Nicht angefasst.

## Nachbesserung 2 (nach Abnahme 2, 2026-11-01)

Mängel aus `S_abnahme_2.md`:

1. Die Lache unter der Spinnenleiche ist in RE1.5 und RE2 belegt, im Port nicht gebaut, und Offen 1 ordnete sie falsch als
   "nur Video" ein.
2. Der Tod der RE2-Babyspinne spritzte rotes Zombieblut `0x1500` statt Spinnenblut. N5 und der Baustein für die Release
   Notes waren deshalb falsch.

Beide sind gebaut. Die falschen Sätze sind oben durchgestrichen und berichtigt (N5, Baustein, Offen 1).

Werkzeuge und Rohbilder: `scratchpad/S_nb2/`. `cap.ps1` startet die eigene Messkopie `re15_pc_S41nb2.exe` mit echtem Titel
und echter Spielerwahl (`RE15_TITLE_CONFIRM_MS=1500`, `RE15_PSELECT_AUTO`), danach `RE15_DEBUG_JUMP`. Gezeichnet wird mit
dem beschleunigten Renderer, ohne AUTOSHOT und ohne SOFTWARE_RENDER. Aufgenommen wird per gdigrab am Fenster-Handle der
eigenen PID mit 10 Bildern/s; beendet wird nur die eigene PID. Belege: `S_belege/nb2/l01…l03`.

### NB2.1 Messschiene `RE15_LACHE_LOG` (kein Verhalten)

`platform/pc/main.c` schreibt nach der Projektion je Schatten einer 0x25 oder einer RE2-Babyspinne eine Zeile:
`F= slot= typ= st=<+4>/<+5>/<+6>/<+7> hx= hz= pool= rgb= y= w(x,z) sichtbar=`. Dabei sind `hx`/`hz` die Halbmaße, die
der Zeichner bekommt, `rgb` die Tinte für `re15_render_shadow_quad_c`, und `sichtbar=1` heißt: Alle vier Ecken haben
vz >= 64, das Viereck geht an den Zeichner. Dazu kommt `<log>.flecken` mit einer Zeile je liegengebliebenem Babyfleck.

### NB2.2 Messung VORHER (Stand 7982d54bd + nur die Messschiene)

| Lauf | Log | Bild |
|---|---|---|
| `v_a_leon_w7` Leon, ROOM2070, Redhawk, Standard-KI | Tod Slot 2 F38. **772 von 772** Leichenzeilen (st=7) tragen `hx=1300 hz=1400 pool=0 rgb=000000`. Babys stehen auf 500 x 600 (Port-Standard), RE2 hat 200 x 200 (NB2.3) | Bilder 290/350/400: unter der Leiche liegt der graue Schatten, nichts wächst (`l01`, obere Reihe). Grünpixel im Bodenfeld (0,400)-(640,478): 0 bis 125 |
| `v_g_elza_re15_w7` Elza, ROOM2071, Redhawk, `RE15_AI_FLAVOR=re15` | **104 von 104** Leichenzeilen 1300 x 1400, pool=0 | Der Lauf war im Bild zu kurz, kein Bildbeleg |

### NB2.3 RE

Die RE1.5-Stellen habe ich in Mod und Disc gelesen. Gleich sind STAGE2 0x80113f40-0x801155e0, 0x80118e24-48,
0x801002a4-b8 und 0x8010025c-78 (eigener Vergleich, Abweichungen 0). RE2 stammt aus `info/re2leon`.

**RE1.5 STAGE2, CORPSE 0x801153d4** (Zustandstabelle @0x80118e24[7]):

```
801153e4 lbu v1,7(v0)                                  ; +0x7 = Phase; Tabelle @0x801002a4 =
                                                       ;   {0x80115414, 0x8011546c, 0x801155d4, 0x801154bc, 0x80115534}
Phase 0  8011541c ori v0,zero,0x5a / 80115420 sb v0,158(v1)     ; +0x9E = 90
         8011542c ori v0,zero,0x1  / 80115430 sb v0,7(v1)       ; +0x7 = 1
         80115448 ori v0,v0,0x2 / 80115464 ori v0,v0,0x40       ; word0 |= 2 | 0x40   (faellt in Phase 1)
Phase 1  8011546c lui a2,0xdf / 80115470 ori a2,a2,0x809f       ; Tinte 0x00DF809F
         8011547c lui a1,0xff00 / 80115494 sw v0,196(a0) / 801154a0 sw v1,236(a0)   ; +0xC4/+0xEC = (alt & 0xff000000) | Tinte
         801154a8 addiu v0,v0,8 / 801154ac addiu v1,v1,8
         801154b0 sh v0,188(a0) / 801154b8 sh v1,190(a0)        ; +0xBC/+0xBE += 8
         801154b4 j 0x8011559c
Phase 3  801154c4 ori v0,zero,0x78 / 801154c8 sb v0,158(v1)     ; +0x9E = 120
         801154d4 ori v0,zero,0x4  / 801154d8 sb v0,7(v1)       ; +0x7 = 4
         8011551c ori v0,zero,0x5  / 80115520 sb v0,148(v1)     ; Clip 5   (faellt in Phase 4: Tinte @0x80115534-68,
                                                                ;           +8 @0x80115570-7c, anim_set @0x80115594)
Zaehler  801155a8 lbu a0,158(v1) / 801155b0 addiu v0,a0,-1 / 801155b4 bne a0,zero,0x801155d4 / 801155b8 sb v0,158(v1)
         801155bc ori a0,zero,0x9 / 801155c8 ori v0,zero,0x2 / 801155cc jal 0x800453d0 / 801155d0 sb v0,7(v1)
                                                                ; war +0x9E schon 0: +0x7 = 2, Raum-SE 9
Eintritt 80114578 ori v0,zero,0x7 / 80114580 sw v0,4(v1)        ; Platzweg 0x80113ff8 Phase 2: Wort = 7 -> +0x7 = 0
         80114d44 jal 0x800453d0 (a0 = 7) / 80114d4c lui v1,0x300 / 80114d58 ori v1,v1,0x7 / 80114d60 sw v1,4(v0)
                                                                ; Weg 0x8011461c Phase 4: Wort = 0x03000007 -> +0x7 = 3
         80114b1c ori v0,zero,0x7 / 80114b20 sb v0,5(v1)        ; Krit-Platzer im Weg 0x8011461c: +0x5 = 7, die Wurzel
                                                                ; springt danach ueber @0x80118f38[7] in den Platzweg
```

Daraus ergeben sich die Schritte so:

- Der Zähler prüft den alten Wert. Phase 0/1 läuft deshalb 91 Takte (90 bis 0), also 1300 + 728 = **2028** und
  1400 + 728 = **2128**.
- Phase 3/4 läuft 121 Takte, also **2268 x 2368**.
- Die Tinte `0x00FFFF38` im Platzweg (@0x80114584) und im Weg 0x8011461c Phase 5 (@0x80114d64-ac) ist nicht erreichbar.
  Die Phase davor schreibt jeweils das Wort mit +0x4 = 7.
- Die Schattenseite ist subtraktiv. Laut Decompile FUN_8001af5c setzt sie `*(param_5+0x26) = 0x5f`, und die abr-Bits 5-6
  sind 2 (B-F). So steht es auch in `render_pc.c` ("SUBTRACTIVE blend").

**RE2 EMS25, FUN_80104DE8 "Boden-Automat +0x225":**

```
80100180 jal 0x80104de8                     ; Wurzel, jeder Takt direkt nach dem Zustands-Handler
CORPSE   80104d68 lbu v0,6(s0) / 80104d70 bne v0,zero
         80104d7c addiu v0,zero,2 / 80104d80 sb v0,549(s0)      ; +0x6 == 0: +0x225 = 2
FUN_80104DE8: 80104dfc lbu v1,549(s0) / 80104e00 lw s1,364(s0)  ; +0x225, Record +0x16C
         Tabelle @0x801000b4 = {0x80104e28, 0x80104e54, 0x80104e68, 0x80104ec8, 0x80104f00}
[0]      80104e30 jal 0x800527b4 -> trocken word0 &= ~0x400, Wasser word0 |= 0x400   ; Schatten zeigen / verbergen
[1]      80104e5c ori v0,v0,0x400                               ; Schatten verbergen
[2]      80104e70 jal 0x800527b4 / 80104e78 bne v0,zero,0x80104f00   ; im Wasser bleibt es bei 2, keine Lache
         80104e7c lui a1,0xbf / 80104e80 ori a1,a1,0x10bf       ; Tinte 0x00BF10BF
         80104e84 addiu v0,zero,3 / 80104e8c sb v0,549(s0)
         80104ea8 sw v0,28(s1) / 80104eb0 sw v1,68(s1)          ; rec+0x1C/+0x44 = (alt & 0xff000000) | Tinte
         80104eac addiu v0,zero,90 / 80104eb4 sb v0,363(s0)     ; +0x16B = 90
         80104ebc addiu v1,zero,-1025 / 80104ec4 sw v0,0(s0)    ; word0 &= ~0x400   (faellt in [3])
[3]      80104ed0 addiu v0,v0,8 / 80104ed4 addiu v1,v1,8 / 80104ed8 sh v0,4(s1) / 80104edc sh v1,6(s1)
         80104ee8 addiu v0,v0,-1 / 80104eec sb v0,363(s0) / 80104ef4 bne / 80104efc sb v0(4),549(s0)
[4]      80104f00 Ruecksprung
INIT     80100368 lui a2,0x320 / 8010036c ori a2,a2,0x320 -> FUN_80016480 `sw a2,4(t0)` @0x80016530: rec+4/+6 = 800/800
```

Damit wächst die Lache 90 Takte lang, insgesamt um 720. Zur Farbe:

- Die Zombielache 0x00BFBF10 (@0x8010a4c0-c4) zeichnet der Port als `sh_r 0x10, sh_g 0xBF, sh_b 0xBF` (`main.c`).
- Bei der Spinne 0x00BF10BF sind Rot- und Grünbyte vertauscht, das gibt `sh_r 0xBF, sh_g 0x10, sh_b 0xBF`. Auf dem
  subtraktiven Schatten bleibt Grün stehen.
- RE1.5 0x00DF809F wird zu `sh_r 0x9F, sh_g 0x80, sh_b 0xDF`. Das zieht Grün am wenigsten ab, die Lache ist dunkel und
  grünstichig.
- Gemessen sind die Farben an der PSX nicht. Abgeleitet sind sie aus den Bytes und der Konvention, die der Port schon für
  Zombie, Hund, Krähe und Gorilla benutzt.

**RE2 EMS26, Babyspinne:**

```
INIT     8010024c lui a2,0xc8 / 80100250 ori a2,a2,0xc8 / 8010025c jal 0x80016480   ; Record 200 x 200, Lage-Zeiger self+0x38
SPLAT    80100c94 lbu v0,6(s0) / 80100c9c bne v0,zero,0x80100d24 ; zweiter Takt -> FUN_8001B250 (@0x80100d24)
0x80100C84 80100ca4 sb v0(1),6(s0)
         80100ca8 lbu v0,546(s0) / 80100cac lw a2,364(s0) / 80100cb0 bne v0,zero,0x80100ce4   ; +0x222 != 0 = in der Luft
         80100cb4 lui a1,0xbf / 80100cb8 ori a1,a1,0x10bf / 80100cd8 sw v0,28(a2) / 80100ce0 sw v1,68(a2)   ; am Boden: Tinte
         80100cec ori v0,v0,0x400 / 80100cf0 sw v0,0(s0)        ; in der Luft: Schatten verbergen
         80100cf8 jal 0x8005bd6c (SE 7)
         80100d00 lbu v0,5(s0) / 80100d10 lbu a2,4295(at) / 80100d14 jal 0x80100f28   ; Record = Tabelle @0x801010C7[+0x5]
FUN_80100F28 80100f50 andi v1,a2,0x7f / 80100f74 addiu v1,v1,4316 ; Record @0x801010dc + 6*Index
Tabelle @0x801010C7 = 80 00 00 00 00 00 00 00 00 00 03 04 00 00 00 00 03 00 00 00 00 00
Record 0 @0x801010dc = 00 19 00 00 00 10   (Effekt 0, Sub 0x19 = Spinnenblut)
Record 3 @0x801010ee = 05 04 00 00 00 10   (Effekt 5)    Record 4 @0x801010f4 = 04 0f 00 00 00 0c   (Effekt 4)
```

Daraus folgt für die Zeilen:

- Index 0 (Zertreten, Wasser-Tod) ist Record 0 mit Bit 0x80: Die Lage wird dann auf die Wasser-Y gesetzt
  (@0x80100f84-b4).
- Die Zeilen 1-9, 12-15 und 17-21 sind Record 0.
- Die Zeilen 10 und 16 sind Record 3, Zeile 11 ist Record 4.

**RE2 PSX.EXE: Der Record des toten Babys bleibt liegen.**

```
FUN_8001B250  8001b258 sw zero,0(a0) / 8001b25c sw zero,4(a0) / 8001b268-70 (0x800cfbf3)--   ; nur word0/word1, kein Record
Entity-Schleife 800267cc lw v0,0(s0) / 800267d4 andi v0,v0,0x1 / 800267d8 beq ...           ; word0 & 1 == 0 -> uebersprungen
              800267e0 lw s1,364(s0)                                  ; Record, VOR dem Handler geladen
              80026888 jalr v0                                        ; Handler (hier: Despawn)
              800268c8 lh a1,450(s0) / 800268d0 lh a2,118(s0) / 800268d4 andi a3,a3,0x400 / 800268d8 jal 0x800168b4 / 800268dc srl a3,a3,9
FUN_800168b4  800168d8 beq s0,zero / 800168e0 lbu v0,14(s0) / 800168e8 beq v0,zero   ; freier Record: nichts
              8001693c sh s1,16(s0) (Drehung +0x76) / 80016948 sh s2,10(s0) (Y = +0x1C2) / 80016950 sh v0,8(s0) / 80016970 sh v0,12(s0)
              80016960 ori v1,v1,0x4 / 80016968 sb v1,14(s0) / 80016974 andi v0,v1,0xfb (nur bei a3 != 0)
FUN_8001699c  50 Records ab 0x800ce698 zu 104 Byte; 80016a04 lbu v1,8(s1) / 80016a0c andi 0x4 / 80016a14 andi 0x8   ; zeichnet Bit 4 ohne Bit 8
Freigabe      FUN_8001633c 8001639c sb zero,-50(s1) (alle 50 Plaetze); einziger Aufrufer 80026e0c in FUN_80026b7c (Raumwechsel)
```

Andere Freigaben gibt es nicht:

- `sb zero,14` steht sonst nur in FUN_8005df38 (`lw v0,364(s3)` @0x8005df54, kein `jal`-Aufrufer gefunden) und in den
  Krähen-Overlays.
- In EMS25 und EMS26 gibt es 0 Treffer (eigener Vollscan).
- `lw …,364(…)` steht in EMS26 nur @0x80100cac und in EMS25 nur @0x80104e00.

Folge: Im Despawn-Takt läuft FUN_800168b4 noch einmal mit word0 = 0, also mit gesetztem Bit 4. Danach wird die tote
Entity übersprungen. Der Record bleibt mit Tinte und letzter Lage (x, +0x1C2, z, +0x76) stehen, bis zum Raumwechsel. Das
ist statisch belegt. An RE2 selbst habe ich es nicht gemessen, dafür gibt es hier keinen Harnisch.

### NB2.4 Bau

- **Neu:** `engine/src/spinnenlache_r41.c` und `include/re15_spinnenlache_r41.h`. Alle Adressen stehen im Kopf.
  - Der Kanal ist derselbe wie bei Hund, Krähe und Gorilla: `crow_shadow_w/h` und `crow_pool`.
  - Die Tinte merkt sich das Modul je Platz.
  - Beim ersten Einfärben übernimmt der Record das gezeichnete Maß: 1300 x 1400 für 0x25 (Spur L), 200 x 200 für das
    Baby.
- **RE1.5-Gehirn** (`enemy_ai_common.c`):
  - Case 3 setzt beim Übergang +0x7 je Todesweg: Platzweg 0, sonst 3 (@0x80114578-80 / @0x80114d4c-60).
  - Der Krit-Platzer im schwachen Weg setzt +0x5 = 7 (@0x80114b1c-20).
  - Case 7 läuft `re15_spinnenlache_r15_leiche`: Phasen 0/1/3/4, +0x9E = `grab_kill_ctr` wie beim Hund, Raum-SE 9 am Ende.
- **RE2-Gehirn** (`enemy_ai_re2_spider.c`):
  - Die Wurzel ruft `re15_spinnenlache_re2_boden` an der Stelle von @0x80100180, mit +0x225 = `re2s_gs225` und
    +0x16B = `re2z_gaitrow`.
  - Der Wasserzweig läuft über `re15_aot_water_at` (= FUN_800527B4).
- **Babyspinne:**
  - Der SPLAT am Boden färbt den Record (`re15_spinnenlache_baby_platsch`).
  - Der Despawn legt den Fleck ab (`re15_spinnenlache_baby_ende`).
  - **Der Todesspritzer nimmt den Record aus der Tabelle @0x801010C7.** Record 0 bekommt den grünen Stand-in
    `re2s_blut`, die Records 3 und 4 bleiben auf `re2s_gore` (Mangel 2).
- **Raumwechsel:** `re15_re2z_rng_reset` (aus `re15_actor_init` und `scd_room_setup.c`) leert die Flecken, wie
  FUN_8001633c den Pool leert.
- **Zeichner** (`main.c`):
  - `re15_spinnenlache_render` steht hinter dem Spur-L-Maß und setzt Maß, `corpse_pool` und Tinte.
  - Neu ist eine Schleife, die die liegengebliebenen Flecken vor den Gegnern zeichnet, mit derselben Projektion wie der
    Gegnerschatten.
- **Messhaken `RE15_BABYTOD_TEST=<Spielbild>`** (nur Messläufe): Ab diesem Bild löst jedes RE2-Baby am Boden die Wirkung
  des Zertret-Tors aus, also +0x4 = 3 mit +0x5 = 0 (@0x80100448). Ohne den Haken stirbt im Port kein Baby (NB2.5).
- **PORT-WAHL:**
  - Die Basis ist das gezeichnete Spur-L-Maß 1300 x 1400. Der RE2-Record der Adult wäre 800 x 800 (@0x80100368-6c).
  - Record 0 des Babys nimmt denselben Stand-in wie die Adult (`0x00082000`, Skala 0x2000). Den 16-Bit-Wert des
    RE2-Records (0x1000) übernehme ich nicht. RE2 legt ihn als `sll v0,a0,16` / `sw v0,56(t0)` @0x8001c00c-10 ab, und
    dass das dieselbe Skala ist wie RE1.5 +0x72, ist nicht belegt.
  - Die Lache selbst ist **keine** PORT-WAHL, beide Originale haben sie. Sie hängt deshalb nicht am Schalter
    `RE15_KEIN_80PROZENT_GORE`, genau wie die Lachen von Hund, Krähe und Gorilla.
- **Nicht gebaut** (siehe NB2.8): FUN_80104DE8 [0]/[1] (Schatten verbergen), der graue Schattenrest toter Babys aus der
  Luft oder dem Wasser, das RE1.5-Flagwort |= 2 | 0x40 und das Posieren der RE1.5-Leiche (Port weiter Clip 5).

### NB2.5 Messung NACHHER (dieselben Wege, exe 2026-11-01 ab 21:15)

| Lauf | Log | Bild |
|---|---|---|
| `n_a_leon_w7` Leon 2070 Redhawk, Standard-KI | Slot 2: Leiche ab F49, **F50 1308 x 1408** pool=1 `rgb=bf10bf`, F139 **2020 x 2120**, dann fest bis F470. Das sind 90 Schritte zu 8. Slot 1: F122 bis F211 genauso | `l01` untere Reihe: Bilder 290/350/400 zeigen eine grüne Lache unter der Leiche. Grünpixel im Bodenfeld: 2906 (Bild 270), 6293 (290), 7907 (400). Vorher waren es 0 bis 125 |
| `n_e_leon_re15_w8` Leon 2070 Remington, RE1.5-KI | Slot 2: Leiche ab F157, **F158 1308 x 1408** `rgb=9f80df`, F248 **2028 x 2128**, Phase 2. Das sind 91 Schritte. 314 Zeilen, `sichtbar=1` | `l02`: Bilder 300/310/320/330/400 zeigen, wie die dunkle, grünstichige Lache wächst. Mittelwert im Feld (0,380)-(330,470): (27.4,28.1,20.7) in Bild 300, (20.5,22.0,15.1) ab Bild 330. Grünpixel 205 → 1072. Ab Bild 330 bleibt der Wert stehen, das ist das Ende der 91 Takte |
| `n_g_elza_re15_w7` Elza 2071 Redhawk, RE1.5-KI | Slot 1/2: 1300 → 2028 x 2128, `rgb=9f80df`, Phase 2 | Die Leichen liegen am unteren Bildrand. In dieser Kamera fällt das Viereck an der Nahebene weg (Integration E7R15: `sichtbar 0`). Kein Bildbeleg aus diesem Lauf |
| `n_h_babytod` Leon 2070 Redhawk, `RE15_BABYTOD_TEST=200` | F200: 9 Babys sterben. Je Baby ein grüner Satz: 99 Zeichnungen `id=0 sub=8 skala=2000 clut=7991`. Rotes `0x1500` gibt es nur in F61-81. Dort fallen Leons HP in F61, F62 und F71 von 100 auf 97 (Babybisse). 9 Flecken `hx=200 hz=200 rgb=bf10bf`, je 270 Bilder bis zum Ende, 8 davon `sichtbar=1` | `l03`: In Bild 309/312 läuft das Baby noch. Ab Bild 320 liegt an seiner Stelle ein kleiner grüner Fleck, er ist bis Bild 400 unverändert |
| `n_f_baby1` (gehen) und `n_f_baby2_w8` (Schrot nach unten), beide ohne Haken | Kein Baby stirbt (`st=3` bei Typ 0x26: 0). Leon geht (`pmo=105`) zwischen den Babys, sie beißen ihn (HP 100 → 92). Nach dem Tod der Adults steht die Zielhilfe auf `zh=-1` | — |

Die Wege sind dieselben wie in NB2.2. Die Bildnummern sind vergleichbar, aber nicht bildgenau gleich, weil der Start des
Laufs schwankt.

### NB2.6 Riegel, Gegenproben, Tests

- **`unit_r41_spinnengore`, neu L8:** Byte-Pins @0x80100d10/14/f50, die Tabelle @0x801010C7 und Record 0. Babytod mit den
  Zeilen 0/3/7/8/13/20 ergibt genau einen grünen Satz und kein `0x1500`. Die Zeilen 10/11/16 bleiben `0x1500`, weil Records
  3/4 kein Blut sind.
- **Neu M1-M10:**
  - M1: 43 Byte-Pins aus STAGE2, EMS25, EMS26 und RE2-PSX.EXE (alle Zitate aus NB2.3).
  - M2: Platzweg 90 Takte 2020, 91 Takte 2028 / Phase 2. Raum-SE 9 kommt genau einmal am Ende, der Zeichner gibt
    `9f80df`.
  - M3: Weg 0x8011461c gibt 2268.
  - M4: Eintritts-Phase für w7, w9, w3, w20 mit Krit und w20 ohne Krit.
  - M5: RE2 kommt auf +0x225 = 4 und 2020 x 2120 mit `bf10bf`.
  - M6: RE2-Leiche im Wasser bleibt auf 2, keine Lache.
  - M7: Baby 200 x 200 grau, SPLAT färbt `bf10bf`, nach dem Despawn liegt 1 Fleck an (x, +0x1C2, z, +0x76). Der
    Raumwechsel löscht ihn.
  - M8: Baby in der Luft hinterlässt keinen grünen Fleck.
  - M9: Der Feuer-Emitter 0x26 bekommt kein Spinnenmaß.
  - M10: Die lebende Spinne behält das Spur-L-Maß.
- **`integration_r41_spinnengore`:** Lauf L7 zusätzlich `pruef_lache re2`, die Leiche muss sichtbar 2020 x 2120 mit
  `bf10bf` erreichen. Neuer Lauf E7R15 (Elza, RE1.5-KI) mit `pruef_lache r15` auf 2028 x 2128 mit `9f80df`.
- **`unit_re2_spider_ai`, geänderte Erwartung:** Nach dem ersten Leichentakt steht +0x225 auf **3** (vorher 2).
  - FUN_80104DE8 läuft im selben Wurzeltakt (@0x80100180) und schaltet trocken weiter (@0x80104e84-8c, +0x16B 90 → 89).
  - Die alte Erwartung 2 galt nur, solange der Automat fehlte.
- **Gegenprobe 1** (`enemy_ai_common.c` und `enemy_ai_re2_spider.c` auf HEAD, Modul und Zeichner bleiben):
  - unit **22 Fehler**: L8 ×6, M2 ×4, M3, M4 ×5, M5 ×3, M7 ×3.
  - integration rot: L7 `pruef_lache re2` Lache 0, E7R15 `pruef_lache r15` Lache 0.
- **Gegenprobe 2** (nur die Zeile `re15_spinnenlache_render` in `main.c` entfernt): integration rot, L7 und E7R15 mit
  Lache 0. Der Zeichner-Zweig hat damit einen eigenen Riegel.
- Danach habe ich den Stand wiederhergestellt (`cmp` gegen die gesicherten Dateien gleich) und neu gebaut.
- **Zielauswahl Unit, 222 Tests.** Muster `spinn|spider|baby|gore|esp_|fx|plattform|re2fx|kanister|affen|birkin|masken36|
  hit_repeat|hitfilter|1d3|leiche|kill|flag_gate|redhawk|granate|weapon_rows|reaktion|hund|dog|crow|kraehe|re2z|
  re2_zombie|lache|schatten|corpse|actor`, `-j 6`.
  - 221 grün, 1 rot (`unit_re2_spider_ai`, die erwartete Änderung oben).
  - Nach der Anpassung ist der Test grün, und die Spinnen-Tests habe ich noch einmal laufen lassen: 5/5.
- **Integration, einzeln nacheinander, 4/4 grün:** `integration_r35_redhawk` 16,8 s, `integration_r38_spinnen_echtlauf`
  114,6 s, `integration_r41_spinnenanim` 120,0 s, `integration_r41_spinnengore` 113,0 s.

### NB2.7 Für die Release Notes (ersetzt den Spinnen-Baustein aus Nachbesserung 1)

**Spinnen: grünes Blut und grüne Lachen.**

- Das Spinnenblut ist jetzt grün, wie im Original RE1.5 (TEX.TIM Zeile 486) und in den 80%-Videos (B 5:18, 5:31, 7:40).
  Vorher ging die Farbwahl des Original-Effekts verloren (FUN_80019700 @0x80019730, @0x8001973c-88).
- Das gilt beim Platzen, bei jedem Treffer, bei abgeschossenen Beinen und beim Tod der Babyspinnen. Es gilt unter der
  Standard-KI und unter der RE1.5-KI.
- Unter der toten Spinne breitet sich jetzt eine Lache aus, wie in beiden Originalen:
  - unter der RE1.5-KI dunkelgrün, 91 Bilder lang (Tinte 0x00DF809F @0x8011546c),
  - unter der Standard-KI grün, 90 Bilder lang (RE2 0x00BF10BF @0x80104e7c).
- Eine tote Babyspinne lässt einen kleinen grünen Fleck zurück. Er bleibt liegen, bis Sie den Raum verlassen (RE2 gibt
  den Schatten-Record erst beim Raumwechsel frei).
- Rot bleibt nur Ihr eigenes Blut, wenn eine Babyspinne Sie beißt.

### NB2.8 Offen (Ergänzung zur Liste "Offen, Stand nach Nachbesserung 1")

Punkt 1 der Liste ist berichtigt, siehe oben. Die Lache ist gebaut, offen sind nur die Wandflecken aus dem Video.

18. **Babyspinnen sterben im Port praktisch nie.** Ohne den Messhaken gab es in `n_f_baby1` und `n_f_baby2_w8` 0 Tode,
    in der Abnahme 2 in 7 Läufen und in 20 Integrations-Logs ebenfalls 0.
    - Gemessen: Nach dem Tod der Adults wählt die Zielhilfe nichts (`zh=-1`), und die Schüsse treffen den
      20-Einheiten-Würfel (@0x80100168-84) nicht.
    - Das Zertret-Tor (@0x801003D4-460) verlangt Spieler-Zustand 1 mit Substate 1..4. Leon ging zwischen den Babys
      (`pmo=105`) und wurde gebissen, getreten wurde keines.
    - Die Ursache am Spieler-Substate ist nicht zu Ende gemessen. Das ist kein Gore, aber ohne Tod gibt es auch keinen
      Fleck.
19. **Grauer Schattenrest toter Babys.** Stirbt ein Baby in der Luft (word0 |= 0x400 @0x80100cec) oder sinkt es im
    Wasser, bleibt nach FUN_8001B250 der ungefärbte Record stehen. Grund: FUN_800168b4 setzt im Despawn-Takt Bit 4
    (@0x800268d8, a3 = 0). Das ist statisch belegt und kein Gore, deshalb nicht gebaut.
20. **FUN_80104DE8 [0]/[1] (word0 0x400): Schatten verbergen.** Im Original verbirgt die RE2-Spinne ihren Schatten im
    Wasser und an der Wand. Im Port zeigt sie ihn, zum Beispiel bei den beiden ROOM2090-Spinnen im Wasser. Der Automat
    hat dafür keinen Port-Konsumenten, nicht portiert.
21. **Grundmaß des Records unter der Standard-KI:** 1300 x 1400 (Spur L, RE1.5-INIT) statt RE2 800 x 800
    (@0x80100368-6c). Die Lache wächst deshalb auf 2020 x 2120 statt 1520 x 1520 (PORT-WAHL, NB2.4).
22. **RE1.5-Leiche:** Das Flagwort |= 2 | 0x40 (@0x80115448/64, @0x801154f0/0c) hat keinen Port-Kanal. Der Port posiert
    die Leiche wie bisher mit Clip 5. Im Original hält Phase 0/1 das letzte Todesbild, nur Phase 3 setzt Clip 5
    @0x80115520.
23. **Die Skala der RE2-Blut-Records** (Baby Record 0: 0x1000, Adult Record 1/3: 0x2800/0x1000) ist nicht übernommen. Der
    Stand-in bleibt 0x2000, Begründung in NB2.4.
24. **Beobachtung, nicht von diesem Bau:** In der Kamera von ROOM2071 (Elza) fällt das Schattenviereck einer Leiche nahe
    der Kamera ganz weg, sobald eine Ecke vz < 64 hat (`main.c`, `if (_vz < 64) { nok = 0; break; }`). Das betrifft
    jeden Gegnerschatten. Ob das Original das Polygon ebenso verwirft, ist nicht geprüft.
