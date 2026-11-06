# Spur S (spinnengore), Abnahme 2 (Runde 41, 2026-11-01)

Ich habe als unabhängiger Abnehmer geprüft und dem Bau nichts geglaubt. Geprüft wurde der Zweig `r41/spinnengore` mit Kopf
`426adbe47` (Nachbesserung 1 auf `df700e30c`).

Den Bau habe ich selbst über `local_build.sh configure` und `local_build.sh build` gemacht (LOCAL-BUILD-OK, exe
`2026-11-01 20:20:48`). Nach der Gegenprobe (Abschnitt 3) habe ich den Stand wiederhergestellt und neu gebaut (exe
`20:36:57`). `git status` ist danach sauber.

Alle Fensterläufe liefen über die eigene Messkopie `re15_pc_S41abn2.exe` mit dem beschleunigten Renderer. Ich habe weder
`AUTOSHOT` noch `SOFTWARE_RENDER` noch `TITLE_SHOT` gesetzt. Titel und Spielerwahl liefen über die echten Menüs
(`RE15_TITLE_CONFIRM_MS=1500`, `RE15_PSELECT_AUTO`, für Elza zusätzlich `_SWITCH`), danach kam `RE15_DEBUG_JUMP`.

- Aufnahme: gdigrab am Fenster-Handle der eigenen PID, 10 Bilder/s. Beendet habe ich nur die eigene PID.
- Ich habe ein eigenes Feuerskript genommen, mit anderem Takt als der Bauer und der Riegel.
- Skripte, Rohbilder und Logs liegen unter `scratchpad/S41abn2/`. Dazu gehören `cap.ps1`, `ana.py`, `farben.py`,
  `vbytes.py` mit `stellen*.txt` und die Probe `probe/baby_tod.c`.
- Die Belege liegen unter `S_belege/abn2/b01…b09`.

## Urteil: NICHT bestanden

Der Kern hält:

- Unter der Standard-KI platzt die Spinne bei einem Magnum-Treffer sichtbar, und das Blut ist grün. Das gilt für Leon und
  Elza, für beide Magnum-Revolver (Super Redhawk w7 und Colt Python w20) und auch unter der RE1.5-KI.
- Jede zitierte Adresse stimmt mit den Bytes überein, im Mod-Baum genauso wie auf der Disc.
- Ohne den Bau sind die Riegel rot, und zwar jede der beiden Hälften für sich.
- Der Schalter wirkt.
- Nebenwirkungen habe ich keine gefunden.

Es bleiben zwei Mängel, beide am Spinnen-Gore:

1. **Die Lache unter der Spinnenleiche ist in RE1.5 und in RE2 mit Bytes belegt, im Port aber nicht gebaut.** Das Dossier
   führt sie als „nur Video, Gestaltung nach Bild, Mechanismus nicht gesucht“. Das ist falsch (Abschnitt 5.1).
2. **Wenn eine Babyspinne stirbt, spritzt sie weiter rotes Zombieblut.** In RE2 ist es Spinnenblut (Record 0, Sub
   0x19). Die Aussagen „jeder Spinnen-Blutsatz ist grün“ und „Rot bleibt nur beim Babybiss“ in N5 und in den Release Notes
   stimmen deshalb nicht (Abschnitt 5.2).

## 1. Eigene Läufe im echten Fenster

Ausgewertet habe ich `RE15_SPINNEN_LOG` (Zustand, `{G..}`-Klammer) und `RE15_FX_CLUT_LOG` (je gezeichnetem Partikel Id,
Sub, Skala und CLUT) mit eigenen Auswertern.

| Lauf | Einstellung | Log | Bild |
|---|---|---|---|
| a Leon, ROOM2070, Redhawk w7 | Standard | F38 Slot 2: st 3, Zeile 5, HP -20, +0x93 = 0x41, **ta=1, sh=80001, fl=7fff8**. F110 Slot 1 ebenso. Je Tod 11 Bilder lang **5× `id=0 sub=8 skala=2000 clut=7991`**. Rotes `0x1500` nur in F61–81 und F265–344. Jede dieser Strecken beginnt genau in dem Bild, in dem Leons HP um 1 fallen (F61, 62, 71, 265, 284, 305, 334: Babybiss) | Bild 259: Der Rumpf zerfällt in gelbgrüne Scherben. Bild 260–261: eine **grüne** Wolke am Rumpf, die Beine fliegen ab (`b01`). Kein Rot im Wolkenfeld |
| b Elza, ROOM2071, w7 | Standard | F38 und F146 wie bei Leon, 110× Sub 8 / 0x7991 | Bild 259–263: Platzen mit grüner Wolke und Scherben (`b02`) |
| c Leon, ROOM2070, **Colt Python w20** | Standard | F38 Slot 2: Zeile 13 (generisch), +0x93 = 0x41 (Krit-PORT-WAHL w20, `re15_damage.c:2745`), ta=1, sh=80001, 5× Sub 8 | Bild 258–261: Es platzt wie mit dem Redhawk (`b03`). Auch der zweite Magnum-Revolver lässt die Spinne also platzen |
| d Leon, ROOM2070, w7, `RE15_KEIN_80PROZENT_GORE=1` | Schalter aus | F38/F110 **ta=0**, sh=0, je 2× Sub 8 (`re2s_blut`, hängt nicht am Schalter) | Bild 259–267: Die Spinne bäumt sich auf und dreht sich auf den Rücken, sie bleibt ganz, es fliegen keine Scherben (`b05`) |
| e Elza, ROOM2031 (Leon-Raum 2030 + 1), w7 | Standard | F77 Slot 1 platzt (ta=1, sh=80001), 5× Sub 8 | Die Spinne steht außerhalb des Kamerabilds, nur einzelne Scherben fliegen ins Bild. Kein Bildbeleg |
| g Elza, ROOM2071, w7, `RE15_AI_FLAVOR=re15` | RE1.5-KI | F38/F74: st 3, Zeile 7.1.1, ta=1, sh=80001, fl=7fff8. 66× Sub 8, **kein Rot** | Bild 258–263: Die Spinne direkt vor der Kamera platzt in große gelbgrüne Scherben, die grüne Wolke steht in Bild 260 (`b09`) |

Nebenbefund aus Lauf c, kein Mangel:

- Mit w20 läuft die Spinne über die generische RE2-Zeile 13. Die Leiche bekommt HP 1 und bleibt beschießbar (F217). Das
  ist RE2: Es gibt kein Zustands-Tor für die Leiche (`re15_damage.c:2053-2060`, @0x8004712C ff.).
- Ein zweiter Schuss auf die Leiche (F218) ruft den Tod frisch auf. `re15_spinnengore_re2_tod` setzt dann Teil 0/19 noch
  einmal auf `|= 0x4a` und spawnt 5 grüne Partikel.
- Im Bild ist das nur eine zweite grüne Wolke über den Resten (Bild 319–321, `b04`). Neue Scherben sind nicht zu sehen,
  denn die Teile waren schon gelandet.

## 2. Zitierte Adressen gegen die Bytes

Den Prüfer habe ich selbst geschrieben, mit capstone und unabhängig vom Skill-Disassembler (`vbytes.py`). RE1.5 lese ich in
`info/Re1.5` und in `Version_40%`, RE2 in `info/re2leon`.

**72 von 72 zitierten Stellen stimmen.** Bei RE1.5 gibt es 0 Abweichungen zwischen Mod und Disc. Ausgabe:
`scratchpad/S41abn2/vbytes_zitate.txt`.

| Stelle | Gelesen |
|---|---|
| FUN_80019700 | `srl v0,a0,16` @0x8001970c, `srl t8,a0,24` @0x80019728, **`andi t7,v0,0xff` @0x80019730**, `andi v0,t7,7` @0x80019734, `srl v1,t7,3` @0x8001973c, `sll s0,v1,6` @0x80019754, **`sb t7,0x71(t0)` @0x800197d8**, `sh s1,0x72(t0)` @0x800197dc, `lhu v0,4(t5)` @0x8001987c, `addu v0,v0,s0` @0x80019884, `sh v0,0x32(t0)` @0x80019888 |
| Krit FUN_80011f50 | `ori v0,zero,8` @0x80012380, `ori v0,zero,7` @0x80012388, `sltiu v0,v0,0xbb8` @0x80012398, `ori v0,v0,0x40`/`sb v0,0x93(s1)` @0x800123b4-b8 |
| Spinnenblut STAGE2 | `lui a0,8` @0x80113b10, `ori a0,a0,0x2000` @0x80113b30, `jal 0x80019700` @0x80113b50 sowie @0x8011463c/40/54. Anker `lw v1,0x188(v0)` @0x80113b48, `addiu a2,v1,0x40` @0x80113b54. Krit `andi v0,v0,0x40` @0x80113b74, `andi v0,v1,0x40` @0x801146b0 |
| Waffentabelle | @0x80118f38 hat 22 Einträge (7–11, 13, 16–18, 21 → 0x80113ff8, sonst 0x8011461c). Wort 22 = 0x80114e60. Die Nachbartabelle `lbu v0,5(v0)` @0x80114e28, `lui at,0x8012`/`addiu at,at,-0x7070` @0x80114e34-38 = 0x80118f90, `jalr v0` @0x80114e48 |
| RE2 FUN_8001bf10 | `andi v0,t5,7` @0x8001bf24, `srl v0,t5,3`/`sll v0,v0,6` @0x8001c01c-20, `addu v1,v1,v0`/`sh v1,0x32(t0)` @0x8001c034-38 |
| RE2 EMS25 | a0-Aufbau @0x80105860-878 (`sll a0,s4,0x18`, `lbu v0,1(s3)`, `lhu v1,4(s3)`), `jal 0x8001bf10` @0x80105874. Records @0x80106684 = `00 19 00 00 00 20 / 00 19 00 00 00 28 / 09 0a … / 00 09 00 00 00 10 / 85 03 …`. Teil-Anker `lw s1,0x198(s2)` @0x80105758 |
| RE2 Aufrufstellen | Record 0 (`move a2,zero`): @0x80102dd0, @0x80103de4, @0x80103f60, @0x80104488. Record 1 (`addiu a2,zero,1` bzw. `a1 = 0x13, a2 = 1` @0x80104560-64): @0x80104580, @0x80104694, @0x80104840, @0x801048fc, @0x80105d00, @0x80105d18. Record 3: @0x80103754/84. Record 2: @0x8010490c. FUN_80104890 @0x80104588/9c/848. **Die Zuordnung der 14 `re2s_blut`-Stellen stimmt** |
| RE2 EMS26 | Biss @0x80100edc `jal 0x80100f28` mit `addiu a2,zero,1` @0x80100ee0. Records @0x801010dc = `00 19 00 00 00 10 / 00 01 00 08 00 08 / 1a 00 … / 05 04 …` |
| RE2 EMZ0 (MC51) | `andi v0,v0,3` @0x80107794, `bnez` @0x80107798, `addiu v0,zero,0xa` @0x8010779c, `lb v0,0x16b(s2)` @0x801077a0, `sh v0,0x156(s2)` @0x801077b0 |
| STAGE1 | `ori v0,zero,0x1e`/`sh v0,0x9a(v1)` @0x80108ccc-d0. Zeilen 7/8/19 Spalte 0 = 0x80108abc |

Paletten (eigener TIM-Leser, `pal2.py`):

- Der CLUT-Block von TEX.TIM liegt bei (256,480) und ist 32×24 groß. Bei x 272 gilt **y 485 = `000000 601010 580808 …`
  (rot)** und **y 486 = `000000 185008 184808 …` (grün)**.
- Die drei Dateien von Mod, Disc und `shared_assets` unterscheiden sich als Ganzes (md5). In diesen Zeilen sind sie aber
  gleich.
- CORE00.ESP Effekt 0 hat in allen drei Ständen das CLUT-Wort 0x7951.
- RE2 hat Effekt 0 mit 0x7a11. Dort ist y 488 = `880000 …` (rot), y 489 = `386030 …` (grün) und y 491 = `888840 …`
  (oliv). `info/re2leon` und `shared_assets/RE2` sind gleich.

## 3. Sind die Riegel ohne den Bau rot?

Ich habe drei Gegenproben gemacht, jeweils `local_build.sh build` und dann `ctest -R ^unit_r41_spinnengore$`:

| Zurückgesetzt auf `df700e30c` | Ergebnis |
|---|---|
| `re15_damage.c`, `enemy_ai_re2_spider.c`, `zombiegore_r15.c` | **5 Fehler**: L3 „gruen 0 rot 1“, L4 „rot 1 alt-0x1500 1“, L5 „gruen 0 rot 5 alt-0x1500 2“, L6 „gruen 0 rot 2“, L7 „gruen 0 rot 1“ |
| nur `re15_damage.c` (`& 7` zurück) | **3 Fehler**: L3, L5 „gruen 2 rot 3“, L7 |
| nur `enemy_ai_re2_spider.c` (`0x1500` zurück) | **3 Fehler**: L4, L5 „gruen 3 rot 2 alt-0x1500 2“, L6 |

Jede Hälfte des Baus hat also einen eigenen Riegel. Danach habe ich den Stand wiederhergestellt (`git checkout HEAD --`)
und neu gebaut. `git status` ist sauber.

## 4. Die Gore-Inventur in Stichproben selbst gemessen

- Das Platzen der Spinne unter beiden Gehirnen, mit w7 und w20, für Leon und Elza, und der Schalter: Abschnitt 1, alles
  wie im Dossier.
- **Probe Babytod** (`scratchpad/S41abn2/probe/baby_tod.c`). Sie ist gegen `libre15_engine.a` des Arbeitsbaums gelinkt und
  bindet `fx_plattform_pc.c` ein. Ein echtes RE2-Baby (`re2s_baby_spawned = 1`) wird nach INIT in DEATH gesetzt und einmal
  getaktet.
  - Ergebnis für +0x5 = 0, 3, 7 und 8: jeweils **`id=0 sub=0 skala=1500 clut=7951`**, also TEX.TIM y 485 und damit rot.
  - Siehe Mangel 2.
- Im Fenster ist in keinem meiner 7 Läufe ein Baby gestorben. Auch in keinem der 20 Logs von
  `integration_r38_spinnen_echtlauf` und `integration_r41_spinnengore` (`st=3` bei `t=26`: 0) ist eines gestorben. Lauf f
  (Leon tritt und schießt nach unten auf die Babys) hat keines getroffen. Ein Bildbeleg für den Babytod fehlt deshalb. Die
  Probe ist der Beleg.

## 5. Das Maximum aus RE1.5, RE2 und den Videos je Typ

Ich habe selbst aus `Version_80%/Resident Evil 1.5 (All Footage).mp4` geschnitten, mit 10 Bildern/s:

- **S01, B 5:17.9–5:20.2** (`b07`): Bei B 5:18.3 fliegen gelbgrüne Brocken auseinander, bis B 5:19.1 liegen grüne Flecken
  an Boden und Wand. Bei B 5:19.2 kommt der Schnitt. Das bestätige ich.
- **Z28, Trailer B 10:00.3–10:02.0** (`b08`): Bei B 10:00.7 steht eine rote Wolke am Kopf. Von B 10:00.9 bis 10:01.5
  bleiben rote Spritzer an der Wand, dann kommt der Schnitt. Das bestätige ich (Offen 11).
- **K01, B 1:11.5–1:16.2**: Bei B 1:11.8 und B 1:15.0 bleiben schwarze Fetzen am Boden liegen, bei B 1:13.4 steht eine
  rote Wolke. Das bestätige ich (Offen 15).

### 5.1 MANGEL 1: Die Lache der Spinnenleiche ist in RE1.5 und RE2 belegt, im Port fehlt sie, und das Dossier ordnet sie falsch ein

Offen-Punkt 1 der Nachbesserung lautet: „Grüne Flecken … Einen Flecken-Mechanismus für die Spinne habe ich in RE1.5 und RE2
nicht gesucht. Gestaltung nach Bild, nicht gebaut.“ Ich habe gesucht. **Beide Quellen haben ihn**, und zwar mit genau der
Mechanik, die der Port für Zombie, Hund, Krähe und Gorilla schon zeichnet: den Schatten-Record einfärben und wachsen lassen.

**RE1.5, STAGE2** (Mod = Disc, `vbytes_lache.txt`):

```
Zustandstabelle Spinne @0x80118e24: [3] 0x80113f40 DEATH ... [7] 0x801153d4 CORPSE
Todesphase 2:  80114578 ori v0,zero,7 / 80114580 sw v0,4(v1)      ; +0x4 = 7, +0x7 = 0 -> CORPSE Phase 0
CORPSE 0x801153d4: 801153e4 lbu v1,7(v0) -> Tabelle @0x801002a4 = {0x80115414, 0x8011546c, 0x801155d4, 0x801154bc, 0x80115534}
Phase 0:  8011541c ori v0,zero,0x5a / 80115420 sb v0,0x9e(v1)    ; 90 Takte, faellt in Phase 1
Phase 1:  8011546c lui a2,0xdf / 80115470 ori a2,a2,0x809f       ; Tinte 0x00DF809F
          80115494 sw v0,0xc4(a0) / 801154a0 sw v1,0xec(a0)      ; (alt & 0xff000000) | Tinte
          801154a8/ac addiu +8 / 801154b0 sh v0,0xbc(a0) / 801154b8 sh v1,0xbe(a0)   ; Lache waechst 8 je Takt
          801155a8 lbu a0,0x9e(v1) ...                            ; bis +0x9e abgelaufen ist
```

Die Phase-3-Lache des Platzers (@0x80114584, Tinte 0x00FFFF38) ist dagegen tot, denn Phase 2 setzt das Wort und damit
+0x7 = 0. Erreichbar ist die Lache der CORPSE-Phase 0/1, und die erreicht auch die geplatzte Spinne.

**RE2, EMS25 FUN_80104DE8 „Boden-Automat +0x225“:**

```
80100180 jal 0x80104de8            ; die Wurzel ruft ihn in JEDEM Takt
80104d7c addiu v0,zero,2 / 80104d80 sb v0,0x225(s0)   ; CORPSE setzt +0x225 = 2
Tabelle @0x801000b4 = {0x80104e28, 0x80104e54, 0x80104e68, 0x80104ec8, 0x80104f00}
[2] 80104e70 jal 0x800527b4 (Wasser?) / 80104e7c lui a1,0xbf / 80104e80 ori a1,a1,0x10bf   ; Tinte 0x00BF10BF
    80104ea8 sw v0,0x1c(s1) / 80104eb0 sw v1,0x44(s1)          ; Record +0x16C, Farbwoerter +0x1C/+0x44
    80104eac addiu v0,zero,0x5a / 80104eb4 sb v0,0x16b(s0)      ; 90 Takte
[3] 80104ed0/d4 addiu +8 / 80104ed8 sh v0,4(s1) / 80104edc sh v1,6(s1)   ; Lache waechst 8 je Takt
```

Zum Vergleich die RE2-Zombielache: `lui a2,0xbf` / `ori a2,a2,0xbf10` @0x8010a4c0-c4 = 0x00BFBF10. Der Port zeichnet sie
als `sh_r = 0x10, sh_g = 0xBF, sh_b = 0xBF` auf dem subtraktiven Schatten (`main.c:10042`; `render_pc.c` um Zeile 762:
„SUBTRACTIVE blend … dark-RED pool“).

- Die Spinnentinte 0x00BF10BF vertauscht dagegen Rot- und Grünbyte. Nach derselben Port-Konvention zieht sie Rot und Blau ab
  und lässt Grün stehen. Das ergibt eine **grüne** Lache.
- Die RE1.5-Tinte 0x00DF809F zieht Grün am wenigsten ab, das ergibt eine dunkle, grünstichige Lache.
- Die Farbe habe ich nicht auf der PSX gemessen. Abgeleitet ist sie aus dem Byte und der Blend-Konvention, die der Port für
  die Zombielache schon verwendet.

Die RE2-Babyspinne färbt beim Platschen denselben Record: `lui a1,0xbf` / `ori a1,a1,0x10bf` @0x80100cb4-b8,
`sw v0,0x1c(a2)` / `sw v1,0x44(a2)` @0x80100cd8-e0 (EMS26).

**Port:**

- `main.c:10010-10107` zeichnet Lachen nur für 0x10–0x12/0x16/0x18, 0x20, 0x21 und den Gorilla, **nicht für 0x25 oder
  0x26**.
- `enemy_ai_common.c:11476` (RE1.5 CORPSE): „color-tint/sink(+8/frame) … = OPEN (render/cosmetic)“.
- `enemy_ai_re2_spider.c:75`: „FUN_80104DE8 … nicht modelliert“.
- `enemy_ai_re2_spider.c:2968` (Baby): „Schattenfarbe 0xBF10BF … kein Port-Kanal“.
- Am Bild: Unter der geplatzten Spinne (Lauf a) und unter der ganzen Spinnenleiche (Lauf d) wächst von Aufnahmebild 263
  bis 300 nichts. Es bleibt der graue Standardschatten (`b06`).

**Bewertung:** Nach der Nutzerregel gilt beim Gore das Maximum aus RE1.5, RE2 und den Videos. Die Lache ist in beiden
Quellen belegt und im Video zu sehen (S01: grüne Flecken am Boden). Der Port hat den Kanal (`crow_pool`,
`crow_shadow_w/h`, Farbe je Typ in `main.c`). Gebaut ist sie nicht, und das Dossier verkauft sie als reine
Video-Gestaltung ohne Mechanismus. Damit ist die Gore-Inventur der Spinne an dieser Stelle falsch.

**Nachbesserung:**

- Entweder die Lache bauen, für 0x25 aus RE2 FUN_80104DE8 bzw. RE1.5 0x8011546c, je Flavor mit eigener Tinte wie beim
  Hund. Für 0x26 käme der Fleck @0x80100cb4-e0 dazu.
- Oder sie mindestens mit diesen Adressen als „belegt, nicht gebaut“ in die Offen-Liste setzen und die Einordnung „nur
  Video“ streichen.
- Vor dem Bau ist nach dem RE-Gate zu messen. Der Wasserzweig (`jal 0x800527b4` @0x80104e70) gehört dazu.

### 5.2 MANGEL 2: Der Tod der Babyspinne spritzt rotes Zombieblut statt Spinnenblut

N3 und N4 des Dossiers prüfen am Baby nur den Biss (@0x80100EDC, Record 1, Sub 1) und den Platscher (@0x80100AD0,
Record 2). Daraus schließen sie: „Rot bleibt nur Leons und Elzas eigenes Blut beim Babybiss“. Den **Todesspritzer** prüfen
sie nicht.

```
EMS26 re2sb_death (SPLAT @0x80100C84):
80100d00 lbu v0,5(s0)                ; +0x5 (Zeile)
80100d08 lui at,0x8010 / 80100d0c addu at,at,v0
80100d10 lbu a2,0x10c7(at)           ; tbl @0x801010C7 = 80 00 00 00 00 00 00 00 00 00 03 04 00 00 00 00 03 ...
80100d14 jal 0x80100f28              ; FUN_80100F28(self, 0, tbl[+0x5])
80100f50 andi v1,a2,0x7f             ; Record = a2 & 0x7f, Basis @0x801010dc
```

Für die Zeilen 0–9 und 12–15 sowie fürs Zertreten und den Wasser-Tod (+0x5 = 0) ist das **Record 0 =
`00 19 00 00 00 10`**: Effekt 0, Sub 0x19. Das ist dasselbe Spinnenblut wie Record 0/1 der Adult, also RE2 y 491 (oliv),
nicht die rote Grundzeile.

Der Port ruft dort `re2s_gore` (`enemy_ai_re2_spider.c:2990`, `0x1500`). Die Probe misst
`id=0 sub=0 skala=1500 clut=7951`, das ist TEX.TIM y 485 und damit rot.

Folgen:

- Der Satz in N5 „Damit ist jeder Spinnen-Blutsatz beider Gehirne grün“ stimmt nicht.
- Der Baustein für die Release Notes „Rot bleibt nur Ihr eigenes Blut, wenn eine Babyspinne Sie beißt“ stimmt ebenfalls
  nicht.
- Der Fehler gehört zur selben Klasse wie Mangel 1 der Abnahme 1.
- Er ist nicht neu durch diesen Bau. Die Nachbesserung hat ihn aber ausdrücklich als „bleibt rot, Spielerblut“ eingeordnet.
  Das gilt nur für den Biss.

**Nachbesserung:**

- `re2sb_death` @0x80100D14 bekommt für Record 0 den grünen Stand-in. Die Zeilen 10/16 (Record 3, Effekt 5) und 11
  (Index 4) bleiben, wie sie sind.
- Den Riegel L um den Babytod ergänzen.
- N5 und die Release Notes richtigstellen.
- Ein Fensterbeleg braucht einen Weg, auf dem ein Baby wirklich stirbt. Den habe ich in 7 Läufen nicht gefunden. Ob Babys im
  Spiel überhaupt sterben (Treffer oder Zertreten), ist eine eigene offene Frage und kein Gore.

### Was ich nicht als Mangel werte

- Die Offen-Punkte 2–17 der Nachbesserung. Benannt sind sie, und meine Stichproben Z28 und K01 bestätigen das Video.
- Dass die RE2-Records 2/4/5/6/7/10 weiter als `0x1500` laufen (Offen 4). Sie sind kein Effekt-0-Blut, und ihr Aussehen ist
  nicht belegt.

## 6. Nebenwirkungen

Unit-Zielauswahl, 73 Tests, **73/73 grün**:

- **Spinnen und Babys:** `unit_r41_spinnengore`, `unit_r41_spinnenanim`, `unit_r38_spinnen`, `unit_spinne_klettern`
  (Wandspinnen, Runde 42), `unit_re2_spider_ai`, `unit_adult_spider_ai`, `unit_spider_ai`, `unit_re2_baby_spider_dmg`,
  `unit_re2_spider_water_2090`, `unit_r41_gatorszene_spinnen`, `probe_spinne_elevation`, `unit_r38_spiderman`,
  `unit_r38_griff_spiderman_*`.
- **Filter, Trefferpause, Latch und Tod-Bits:** `unit_re2_hit_repeat`, `unit_re2_hit_repeat_crow_spider`,
  `unit_re2_zombie_hitfilter`, `r29_1d3_doppelabzug`, `unit_r34n_f_leiche_latch`, `unit_writher_kill_flag`,
  `unit_r35_zgirl_killflag`, `unit_flag_gate`.
- **Effekte und Nutzer von `re15_enemy_fx19700`:** `unit_esp_parse`, `unit_fx_emitter_ai`, `unit_montage_fx`,
  `unit_r34_plattform*`, `unit_r34_re2fx*`, `unit_r41_kanister2080_fx_*`, `unit_r38_affen*`, `unit_birkin_opfer`,
  `unit_r35_affen_teile`, `unit_r41_masken36_teile`.
- **Gore:** `unit_zombie_gore`, `unit_re2_gore*`, `unit_re2_gore80`, `unit_re15_re2z_gore_se`, `unit_r35_redhawk`,
  `unit_r35_granate`, `unit_r36_granate`, `unit_re2_weapon_rows`, `unit_r34_reaktion`.
- **Hund und Krähe:** `unit_r30_hund_tod*`, `unit_re2_dog_ai`, `unit_dog_hit`, `unit_re2_dog_hitwindow`, `unit_re2_crow_ai`,
  `unit_r36_schuss_kraehe`, `unit_crow_death_corpse`.

Integration einzeln nacheinander, **4/4 grün**:

- `integration_r41_spinnengore` 97 s;
- `integration_r38_spinnen_echtlauf` 116 s (Babys, Bisse, Gift);
- `integration_r41_spinnenanim` 129 s (Schachtspinne, Figur-Matrix);
- `integration_r35_redhawk` 17 s.

Aus dem Code:

- `re15_enemy_fx19700` hat außerhalb der Spinne nur Aufrufer mit Sub 0: `0x2000`, `0x1500`, `0x2500` in
  `enemy_ai_common.c` 5160–13704 und in `affen3_r38.c:195`. `zombiegore_r15.c fx()` hat ebenfalls nur Sub 0, auch
  `0x05002800`.
- Ein volles Sub-Byte liest nur der Zeichner (`>> 3`) und die Strom-Suche (`& 7`, `re15_esp.c:219/232`). Ein Index ohne
  Maske gibt es nicht.
- `re2s_blut` steht nur in den Adult-Funktionen. Biss, Platscher und Tod des Babys sind unverändert.
- Der Commit fasst Tod-Bits, `em_flag_id`, Trefferpause (`re2z_self1d3`) und Latch nicht an.
- `RE15_FX_CLUT_LOG` ist reine Ausgabe (`main.c:442-455`).

## 7. Der Schalter `RE15_KEIN_80PROZENT_GORE`

Er wirkt. In Lauf d gilt ta=0, die Spinne bleibt im Bild ganz, und es fliegen keine Scherben. Am Schalter hängt nur die
Hebung. Das grüne `re2s_blut` hängt bewusst nicht daran (2× Sub 8 bleiben), so wie es das Dossier unter N4 beschreibt.

## Mängel (für die Nachbesserung)

1. **Lache der Spinnenleiche (0x25, dazu der Babyfleck 0x26): in RE1.5 und RE2 belegt, im Port nicht gebaut, im Dossier
   als „nur Video, nicht gesucht“ eingeordnet.**
   - RE1.5 CORPSE 0x801153d4, Phase 0/1: `ori 0x5a` / `sb 0x9e` @0x8011541c-20, Tinte `lui 0xdf` / `ori 0x809f`
     @0x8011546c-70, `+0xbc/+0xbe += 8` @0x801154a8-b8.
   - RE2 FUN_80104DE8: Aufruf @0x80100180, `+0x225 = 2` @0x80104d80, Tinte 0x00BF10BF @0x80104e7c-80, 90 Takte
     @0x80104eac-b4, `rec+4/+6 += 8` @0x80104ed0-dc.
   - Baby: 0x00BF10BF @0x80100cb4-e0.
   - Im Port fehlt der Zweig in `main.c:10010-10107`, und die drei OPEN-Stellen bleiben offen.
   - Nachbesserung: bauen (PORT-WAHL Gore-Maximum, Mechanik byte-true) oder mit diesen Adressen als „belegt, nicht gebaut“
     benennen. Offen-Punkt 1 ist dabei richtigzustellen.
2. **Der Tod der RE2-Babyspinne spritzt rotes `0x1500` statt Spinnenblut.**
   - EMS26 @0x80100d10-14: `lbu a2,0x10c7(at)` / `jal 0x80100f28`. Die Tabelle @0x801010C7 ergibt Record 0 =
     `00 19 00 00 00 10`, also Sub 0x19.
   - Der Port nimmt `re2s_gore` (`enemy_ai_re2_spider.c:2990`). Die Probe misst `clut=7951`, also rot.
   - N5 („jeder Spinnen-Blutsatz grün“) und der Baustein für die Release Notes („Rot bleibt nur beim Babybiss“) sind zu
     berichtigen. Der Riegel L ist um den Babytod zu ergänzen.
