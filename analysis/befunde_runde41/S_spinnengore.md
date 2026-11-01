
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
