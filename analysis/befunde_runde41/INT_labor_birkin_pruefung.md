# Unabhaengige Pruefung: Labor-Birkin, Angriffsbox KF N-2 (Integration Runde 41)

Geprueft: Fix 145316fc9 und Bericht d3dcddf1a auf r41/integration (Basis a3051c902).
Geprueft am 2026-11-09. Pruefer: eigener Agent, ohne die Messungen des Fixers zu uebernehmen.
Alle Laeufe und Werkzeuge liegen unter
`C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/labor_check/angriff/pruefung/`.

## Ergebnis

**Bestanden, ohne Befund gegen den Fix.**

- Der Versatz KF N-2 ist im Original belegt. Ich habe zusaetzlich den Schreiber von Teil+0x40 selbst gefunden (1.2): den Modell-Zeichner nach der KI-Schleife. Der Fixer hatte das nur indirekt ueber den Fussvortrieb belegt.
- Alle zitierten Bytes stimmen (1.5).
- Labor-Befund unabhaengig reproduziert, mit einer selbst gebauten a3051c902-exe. Mit dem Fix behoben: Der stehende Spieler wird nach dem Haken wieder gebissen (Leon und Elza, je 9 Bisse bei af 35).
- Der Birkin stirbt, die Bits 3:73 und 3:84 werden gesetzt, und die Tuer fuehrt danach nach 50D0 bzw. 50D1.
- Game Over ist im Labor nicht erreichbar, im Original genauso. Belegt mit Klemmen und Haken-Tabelle STAGE5 (2.2). Das ist ein Hinweis, kein Befund.
- STAGE3 3070: geaendert ist nur die Kralle, zwei Bilder spaeter, wie vorgesehen. Wurf-Tod (Game Over), Birkin-Tod, Bits 4:194/4:236 und die Tuer nach 30E0 laufen.
- Die ctest-Gruppe ist 33/33 gruen.
- Korrektur zum Fixer-Bericht und Auftrag: `integration_r41_birkin_pistole` ist kein STAGE3-Riegel, er laeuft in 5090/5091. Fuer STAGE3/3070 gibt es keinen Riegel.

## 1. Konstanten und Mechanismus, selbst disassembliert

Der Fix fuehrt keine neue Zahl ein. Er verschiebt nur das Bild, an dem `re15_bk_kontakt` die Klaue posiert:
`(anim_frame + fc - 2) % fc`. Zu pruefen war deshalb vor allem, ob der Versatz 2 aus dem Original folgt.
Alle Bytes unten habe ich selbst mit `re15_disasm.py` aus `info/Re1.5/PSX.EXE` und `PSX/BIN/STAGE5.BIN` bzw. `STAGE3.BIN` gelesen.

### 1.1 Die Box liest die gezeichnete Welt-Matrix (EXE FUN_8001a5e0)

| Adresse | Bytes | Befehl | Bedeutung |
|---|---|---|---|
| 0x8001a5ec | `40 00 04 26` | addiu a0,s0,64 | Teil+0x40 = Welt-Matrix |
| 0x8001a604 | `8a 9e 01 0c` | jal 0x80067a28 | ApplyMatrixSV(Teil+0x40, Versatz) |
| 0x8001a610 | `54 00 02 96` | lhu v0,84(s0) | + Teil+0x54 |
| 0x8001a638 | `5c 00 04 96` | lhu a0,92(s0) | + Teil+0x5c |
| 0x8001a678 | `06 00 50 94` | lhu s0,6(v0) | Radius; EXE 0x80073e94 = `00 00 06 fa 00 00 c2 01 fa 05 c2 01` -> 450 |
| 0x8001a6b4 | `2a 10 50 00` | slt v0,v0,s0 | Treffer = d < Radius + Reichweite |

### 1.2 Wer schreibt Teil+0x40 und wann (vom Fixer nicht belegt, hier nachgetragen)

Der Fixer belegt das Nachhinken nur indirekt ueber den Fussvortrieb. Ich habe den Schreiber direkt gesucht:

- FUN_8001ef54 / FUN_8001e9ec (die Teil-Zeichner): `jal 0x80022da0` mit a0 = *(Teil+0x6c) (Elternmatrix), a1 = Teil+0x18 (lokal), a2 = Teil+0x40.
  Bytes `68 8b 00 0c` @0x8001ef80 und @0x8001ea24, `addiu s1,s0,64` @0x8001ef78 / `addiu s1,s2,64` @0x8001ea18.
- Beide ruft nur der Modell-Zeichner FUN_8001e8c8 (Schleife ueber alle Teile, Schritt 0xac).
- FUN_8001e8c8 wird in der Hauptschleife zweimal gerufen: @0x8001d09c (Spieler) und @0x8001d108 (`32 7a 00 0c`, je aktiver Gegner, nur `+0 & 1`).
- Die KI-Schleife FUN_8001a50c laeuft vorher in derselben Hauptschleife (@0x8001ce04). Sie selbst komponiert nichts (disassembliert 0x8001a50c-5dc: nur der jalr-Dispatch ueber 0x80072bac).

Damit ist der Ablauf je Bild belegt: KI (anim_set posiert Teil+0x18), danach der Zeichner (Teil+0x40 = Eltern * Teil+0x18).

### 1.3 anim_set posiert vor dem Hochzaehlen (EXE FUN_8001f314)

| Adresse | Bytes | Befehl |
|---|---|---|
| 0x8001f35c | `95 00 02 91` | lbu v0,149(t0): Bild = +0x95 (alt) |
| 0x8001f36c | `68 01 06 ad` | sw a2,360(t0): Frame-Eintrag dieses Bildes |
| 0x8001f574 / 0x8001f5f0 | jal 0x80068098 | RotMatrix -> Teil+0x18 |
| 0x8001f618 | `01 00 42 24` | addiu v0,v0,1 |
| 0x8001f61c | `95 00 62 a0` | sb v0,149(v1): erst jetzt +0x95 += 1 |

Rechnung: Im Tick T steht vor anim_set +0x95 = M-1. anim_set posiert Bild M-1 nach Teil+0x18 und setzt +0x95 = M.
Die Box im selben Tick liest Teil+0x40 aus dem Zeichner von Tick T-1. Dort war Bild M-2 posiert. Box = KF (+0x95 nach anim_set) - 2.
Der Port zaehlt gleich: `re15_birkin_anim` erhoeht `anim_frame` vor der Fensterpruefung, die Fenster vergleichen den erhoehten Wert.
Der Versatz 2 ist also belegt.

### 1.4 Zwischen anim_set und Box liegt kein Komponierer

Eigener Scan, rueckwaerts von jeder Box bis zum letzten `jal 0x8001f314`:

| Overlay | Box | anim_set | Aufrufe dazwischen |
|---|---|---|---|
| STAGE5 Kralle | 0x80117f24 | 0x80117e54 | 0x8001a9cc (arc_test), 0x8001aac4 (Drehen) |
| STAGE5 Biss | 0x80118400 | 0x801183a8 | keine |
| STAGE5 Wurf | 0x801186b4 | 0x80118664 | keine |
| STAGE5 Tackle | 0x801192f0 | 0x80119230 | 0x8001aac4, 0x800453d0 (SE) |
| STAGE3 Kralle | 0x80117710 | 0x80117640 | 0x8001a9cc, 0x8001aac4 |
| STAGE3 Biss | 0x80117bec | 0x80117b94 | keine |
| STAGE3 Wurf | 0x80117ea0 | 0x80117e50 | keine |
| STAGE3 Tackle | 0x80118adc | 0x80118a1c | 0x8001aac4, 0x800453d0 |

Keiner dieser Aufrufe schreibt Teil+0x40. Alle vier Boxen sind `78 69 00 0c` (jal 0x8001a5e0).

### 1.5 Neu zitierte Adressen in den Kommentaren, Byte fuer Byte

| Adresse | Behauptet | Gelesen |
|---|---|---|
| STAGE5 0x801003d8 | `20 03 51 01 00 00 00 00` / 0x801003e0 `84 03` | gleich |
| STAGE5 0x80100404 | `84 03 51 01 00 00 00 00` / 0x8010040c `f4 01` | gleich |
| STAGE5 0x80100414 | `e8 03 51 01 00 00 00 00` / 0x8010041c `bc 02` | gleich |
| STAGE3 0x80100390 / 0x801003bc / 0x801003cc | dieselben Saetze | gleich |
| STAGE5 0x80117f24 / 0x80118400 / 0x801186b4 / 0x801192f0 | `78 69 00 0c` | gleich |
| STAGE5 0x80117094 | `95 00 82 a0` | gleich (STAGE3 0x80116880 ebenso) |
| STAGE5 0x801170a4 | `05 00 50 a0` | gleich (STAGE3 0x80116890 ebenso) |
| STAGE5 0x80117854 | `c4 09 42 2c` sltiu 0x9c4 | gleich |
| STAGE5 0x80117878 | `01 04 02 34` | gleich |
| STAGE3 0x80117040 | `c4 09 42 2c` | gleich; der alte Kommentar @0x80117038 zeigte auf `d0 01 82 8c` (lw +0x1d0), die Korrektur stimmt |
| STAGE5 0x801183e0-e4 | `dd ff 42 24` / `08 00 42 2c` (Fenster 0x23..0x2a) | gleich |
| STAGE5 0x8011848c | `f6 ff 42 24` (hp -10) | gleich |
| STAGE5 0x8011be88 / 0x8011bea8 | `54 00 64 8e` / `5c 00 64 8e` | gleich |
| STAGE5 0x8011feb4 | ACT-Tabelle, [4] = 0x801181ec | gleich |

Ergebnis 1: Alle zitierten Bytes stimmen, keine Konstante ohne Adresse.

## 2. Echter Weg im Labor (eigene Laeufe, Integrations-exe nach eigenem configure + build)

Der Bau war aktuell: `local_build.sh configure` und `build` -> `ninja: no work to do`, `=== LOCAL-BUILD-OK (build)`.
Der Quelltext ist gleich HEAD, das Objekt `enemy_ai_common.c.obj` stammt von 03:05:34, die exe von 03:05:36.

Aufbau wie im Riegel: Debug-Sprung nur fuer den Raum, Spieler am Ankunftspunkt der 50D0-Tuer, Route durch das Ausloesefeld.

| Lauf | Raum | Eingabe |
|---|---|---|
| L1 / E1 | 50E0 Leon / 50F1 Elza | Super Redhawk, stehen und feuern; ab F2860 an den Tuerpunkt gesetzt (RE15_POCC_TP, nur fuer den Weg zur Tuer), dann Quadrat |
| L2 / E2 | 50E0 / 50F1 | nur die Route, danach **keine Eingabe**: Spieler steht, bis F9000 |
| L2alt / E2alt | 50E0 / 50F1 | wie L2/E2, aber eine exe, die ich selbst aus a3051c902 gebaut habe (`git archive` nach scratchpad/.../pruefung/alt, eigener Baum) |

### 2.1 Spieler-Treffer (state.log, jede hp-Aenderung)

| Lauf | Treffer (Bild, hp, Birkin ss1/af) | Birkin tot | Bits 3:73 / 3:84 | Tuer |
|---|---|---|---|---|
| L1 | F931 100->90 (Kralle af 39), F1075 ->80 (Biss af 35), F1278 ->70, F1445 ->60 | F1785 (st 3, hp -25) | F1906 | `DOOR FIRE slot=0` -> room50d0.rdt |
| E1 | F873 (af 39), F1014 (Biss af 35), F1206, F1507, F1761 | F1885 (hp -30) | F2006 | `DOOR FIRE slot=0` -> room50d1.rdt (Elzas Tuerziel) |
| L2 | F925 (af 39), dann 9 Bisse je af 35: F1068 ... F3307, hp 100 -> 0 | - | - | - |
| E2 | F868 (af 39), dann 9 Bisse je af 35: F1008 ... F2954, hp 100 -> 0 | - | - | - |
| L2alt | nur F923 (Kralle af 37), danach bis F4500 kein Treffer, hp 90 | - | - | - |
| E2alt | nur F866 (af 37), danach bis F4500 kein Treffer, hp 90 | - | - | - |

Der Befund ist damit unabhaengig reproduziert und behoben. Mit dem alten Stand trifft der Birkin den stehenden Spieler nach dem Haken nie wieder. Mit dem Fix trifft jeder Biss bei +0x95 = 0x23. Die Kralle trifft jetzt bei af 39 statt 37, also genau zwei Bilder spaeter, wie es der Versatz vorhersagt.

### 2.2 Spieler kann sterben? Im Labor nein, und das Original auch nicht

In L2 / E2 steht hp nach dem zehnten Treffer bei 0. Der naechste Biss setzt -10 und klemmt auf 1. Bis F9000 gibt es kein Game Over (`finisher.log` wurde nie angelegt).
Das stimmt mit dem STAGE5-Overlay ueberein:

- Alle Treffer klemmen auf 1:
  - Kralle: `addiu v0,v0,-10` @0x80118004, `bgez` @0x80118014, `ori v0,zero,0x1` / `sh` @0x8011801c-24
  - Biss: `f6 ff 42 24` @0x8011848c, `03 00 41 04` bgez @0x8011849c, `01 00 02 34` / `sh` @0x801184a0-a8
  - Tackle: `addiu v0,v0,-5` @0x80119330, bgez @0x80119340, `sh` 1 @0x8011934c
- Kein Haken toetet. Die Haken-Tabelle @0x80120468 lautet [0x8011b018, 0x8011b40c, 0x8011b6e0, 0x8011b9bc]. Die Wege [0]/[1]/[2] enden mit aca58 = 1, also Loesen (@0x8011b3c0, @0x8011b698, @0x8011b93c).
- Der Wurf schreibt aca58 = 6 mit aca59 = 0 (@0x801186f8). Er landet also auf [0] und nicht auf dem Wurf-Tod [3].
- Im Birkin-Bereich von STAGE5 gibt es keinen Store von 7 oder 3 nach aca58. Mein Scan aller `sw ..,-13736(at)` zeigt 7 nur @0x80111a34, @0x80116708 und @0x801169d8, alle vor der Wurzel 0x80116ef4.
- Der allgemeine Tod (aca58 = 3 bei hp < 0) sitzt nur in FUN_80012d60. Der Birkin ruft ihn nicht.

Game Over durch den Labor-Birkin ist also im Original nicht erreichbar, und im Port mit Fix ebenfalls nicht. Der Fix hat daran nichts geaendert: Klemme und Haken-Maschine sind unberuehrt. Das ist kein Befund gegen den Fix. Der Nutzer sollte es aber wissen, weil die Pruefliste "Game Over erreichbar" erwartet hatte.

## 3. STAGE3 3070 (Leon): gegengefahren, alt gegen neu

Es gibt keinen STAGE3-Birkin-Riegel. `integration_r41_birkin_pistole` laeuft in 5090/5091, nicht in STAGE3; der Fixer-Bericht zaehlt ihn bei den STAGE3-Pruefungen mit.
Deshalb bin ich 3070 selbst gefahren. Der Ausloeser ist Aot_set Slot 2 (sce 3, flags 0x31, Rechteck (-26770,-9270,1230,1270)) unter `Ck 4:215 == 0`.
Startpunkt war (-25310,-8670) mit Blick 2048, also der Member_set-Punkt aus sub02. Dazu Quadrat. Das ist nicht der Weg ab der 30E0-Tuer, die bei (-11840,-18140) ankommt.

| Lauf | Ergebnis |
|---|---|
| S1 neu, ohne Eingabe | 4:215 F301. Kralle F1031 af 39, F1234 af 39, Biss F1338 af 35. **Wurf ab F1460 (hk 6/1), F1495 hp -1, pst 7, finisher `YOUDIED`**: Game Over erreichbar (STAGE3 cmd 6 = Wurf-Tod) |
| S2 neu, Redhawk ab ~F1086 | Kralle F1031 / F1238 af 39, Wurf-Tod F1438. Birkin nicht tot (hp 365) |
| S2alt (a3051c902), gleiche Eingabe | Kralle F1029 / F1236 af **37**, Wurf-Tod F1438, Birkin hp 365 |
| S4 / S5 / S6 neu, Raketenwerfer ab ~F700 | Birkin tot F1032 (st 3, hp -185), **4:194 / 4:236 F1154** |
| S6 | danach Tuer Slot 1: `DOOR FIRE slot=1` -> room30e0.rdt |
| S5 | Slot 0 (Fahrstuhl nach 3080) zeigt Meldung 5 ("The power seems to be off..."). Der Port fuehrt Slot 0 dort als MESSAGE-AOT; das ist vom Fix unabhaengig und unveraendert |

STAGE3 aendert sich nur so weit, wie es der Fix vorsieht: Die Kralle trifft zwei Bilder spaeter. Wurf, Tod des Spielers, Tod des Birkin, Folgebits und Tuer laufen.
"Unberuehrt" im woertlichen Sinn ist 3070 nicht. Das ist aber gewollt und belegt, weil der STAGE3-Biss dieselbe Box ohne Zwischenaufruf hat (Tabelle 1.4).

## 4. Testsummen, selbst gefahren

`ctest -R "birkin|labor|sichel|bahnsteig|form" -j 2 --timeout 1800` im Integrations-Bau, keine anderen Spiele parallel:

```
100% tests passed, 0 tests failed out of 33
Total Test time (real) = 875.92 sec
```

Darunter:
- integration_r41_labor_birkin_angriff Passed 221.43 s
  - leon: Haken F931, Biss F1075 af=35 -10, Tod F1785
  - elza: Haken F873, Biss F1014 af=35 -10, Tod F1885
- integration_r41_birkin_pistole Passed 475.68 s
- integration_r41_sichel_form Passed 64.25 s
- alle sechs integration_r41_form_wagenweg*
- unit_birkin_opfer, unit_birkin_ai, unit_birkin_bahnsteig, unit_r41_birkin_wand_*

Volles Protokoll: scratchpad/.../pruefung/ctest_gruppe.txt.
Die Gruppe des Fixers (40 Tests, mit `r41_birkin|g5`) und die unit/prop/probe-Suite (1100) bin ich nicht erneut gefahren. Der Auftrag nannte die Gruppe birkin|labor|sichel|bahnsteig|form.

## 5. Offene Punkte (keine Befunde gegen den Fix)

- Fussvortrieb FUN_8011bdd8 (Schritt 2 des Fixers): bleibt offen, wie im Bericht.
- Ueberblendung +0x8f: Bei KF 0x21 ist die Pose noch zu 1/8 die alte. Nicht simuliert. In allen Laeufen trafen die Bisse trotzdem bei af 35.
- Tackle: Die Wurzel-Lage der gezeichneten Matrix ist nicht geprueft. In keinem Lauf kam ein Tackle vor, weil der Spieler nie rannte.
- Labor: Game Over ist durch den 0x30-Birkin nicht erreichbar, im Original genauso (2.2). Das gehoert als Hinweis in die Release Notes.
- 3070 wurde ab dem Ausloesepunkt gefahren, nicht ab der 30E0-Tuer. Mit dem Redhawk stirbt der stehende Schuetze in 3070 am Wurf, bevor der Birkin faellt. Das gilt alt wie neu gleich (F1438).
