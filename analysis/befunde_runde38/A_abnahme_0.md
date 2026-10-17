# Runde 38 — Spur A "cursor": Abnahme 0 (unabhaengig)

Stand: 2026-10-17, Baum `.claude/worktrees/r38_cursor` (Zweig `r38/cursor`, Basis 1bcbd0ff, HEAD 8b8cd852).
Pruefer: unabhaengige Abnahme 0. Geglaubt wird nur, was hier selbst gemessen/disassembliert ist.
Messdaten (nicht versioniert): Scratchpad der Abnahme-Sitzung (`.../scratchpad/abn/`), Pfade je Messung unten.

## Nutzer-Punkt (Wortlaut, AUFTRAG.md)

> Der Cursor, der zum Beispiel in ROOM 1150 oder ROOM 11F0 verwendet wird, hat immer noch so einen komischen
> Schatten. Den hat er im Original nicht...

## 0. Bau

* `bash re15_port/tools/local_build.sh configure && ... build` am HEAD 8b8cd852: `=== LOCAL-BUILD-OK (build)`,
  "ninja: no work to do" (exe 05:17 = Stand nach der letzten Code-Aenderung; Tests/Sonden ebenfalls aktuell).


## 1. Tests der Spur (selbst gefahren, HEAD 8b8cd852)

* `ctest -R "^unit_r38_cursor" -V`: 4/4 gruen (nclip, kasten, raeume 23/23, wicklung: 11 raeumliche Modelle ohne
  w > 0, Feuerloescher stehend/liegend je w<0 148, 8 flache erkannt; Gegenproben v1<->v2 schlagen an).
* `ctest -R "^integration_r38_cursor$" -V`: **Passed 179.16 sec** (Laeufe A 11F0, B 1150 Lade-Weg, C 10D0, D Elza
  11F1, E 10F0 Cut 0+5, F Elza 10F1 Cut 0+6); A/D Klassenkarte 0 Abweichungen, alle Cursor-Plaetze 0 Dreiecke mit
  NCLIP < 0, E/F Schluessel MAC0 > 0 in 2 von 2 Laeufen. Log: `.../scratchpad/abn/ctest_integration_r38_cursor.log`.

## 2. Original-Bild selbst nachgeprueft

`analysis/grundwahrheit/psx_room11f0_cut10.sav` (DuckStation, "Biohazard 1.5 (MZD Mod)") mit
`re15_ss.py` entpackt: @0x80026e4c = `08 00 e0 03 21 10 00 00` (Auslieferungs-EXE, NICHT der Patch `24 c2 01 08`);
Objekt-Pool 0x800b3f98 obj0 = `01 01 .. 04 00 ff 09`, +0 = 0x0101, Typ 4, Attribut (+0x0C) 0 = der Cursor; beide
320x240-Anzeigepuffer im VRAM (y0 = 0 und 240) gegen `psx_room11f0_cut10.png`: **0 von 8560** Stichproben abweichend
(Gegenprobe y0 = 256: 5085 abweichend). Das PNG ist also ein echtes Original-Bild.

## 3. Messung Punkt 1 — ROOM11F0 ueber den ECHTEN Raetsel-Einstieg (Aktionstaste am Panel)

Weg: Debug-Sprung 11F0 (der Raum selbst), Spieler vor das Panel (`RE15_PLAYER_POS=-1800,-14500,1024`: Blick -z, Punkt
620 voraus = (-1800,-15120) in der Untersuchen-Zone Aot slot 1 (-2200,-15850,800,1400), ROOM11F0.RDT @0x00D64
`2c 01 03 31 ... ff 00 18 10` = Evt_exec sub16), dann 16 Aktions-Druecke (`RE15_INPUT_SCRIPT`, Basis Spielbild, ab
F150): Text 0 (mehrseitig) + Text 1 (Ja/Nein) wie sub16 @0x015A2/@0x015A8, danach **`[scd F603] Cut_chg(10)`** — die
Raetselkamera kommt aus dem SCD (nicht RE15_FORCE_CUT, keine gesetzten Flags). Danach Cursor per Steuerkreuz bewegt
(R/D/L/U). Laeufe `runs/m1b_{neu,alt}_s{1,3}` (neu = exe dieses Baums, alt = v0.8.25-exe des Hauptbaums, 14.10. 07:19;
`git diff 2077bb8e 1bcbd0ff -- re15_port` ist leer, also derselbe Code wie die Basis), RE15_TRILOG ganzer Schirm,
RE15_FRAMEDUMP 640-960/20.

Eigenes Auswertewerkzeug (nicht das des Bau-Agenten): `scratchpad/abn/trilog_ana.py` (NCLIP aus den geloggten
Schirmecken v0,v1,v2) und `scratchpad/abn/klassen.py` (eigene Klassen G/g/Y/y nach Farbschwellen).

| Lauf | Platz 4 (Cursor) eingereihte Dreiecke | NCLIP < 0 | = 0 | > 0 |
|---|---|---|---|---|
| alt (v0.8.25) | 76108 | **10345** | 24296 | 41467 |
| neu | 65763 | **0** | 24296 | 41467 |

-> genau die rueckseitigen Dreiecke fallen weg, alle anderen bleiben (= 0 und > 0 bitgleich gezaehlt). (Vorlauf `m1_*` mit
2 Druecken zu viel im Raetsel — ein Schalter umgelegt —: alt 9065, neu 0 rueckseitige von 71868/62803.)

Zweiter Weg OHNE Debug-Sprung (`runs/m9_11f0_laden_neu`): Spielstand in ROOM11F0 vor dem Panel
(probe_r37_feuerloescher_karte `11F0 -1800 -14500 1024 cut:0`) -> Titel -> LOAD GAME -> `[save] CONTINUE: resumed in
room 11f0` -> 16 Aktions-Druecke -> Text 0/1 -> `[scd F543] Cut_chg(10)`; Platz 4: 0 von 29256 rueckseitig; Klassenkarte
F560 und F700 gegen das Original **0 Abweichungen**. (Tuer-Weg 11E0 -> 11F0 nicht gefahren: die Tuer @0x01512 in
ROOM11E0.RDT setzt in 11F0 bei (250,0,250) ab, das Panel liegt ~14800 Einheiten entfernt; Raumlader und Objekt-Zeichner
sind fuer Tuer, Sprung und Laden derselbe — der Cursor-Zeichenweg haengt nicht vom Eintrittsweg ab.)

Cursor BEWEGT (Steuerkreuz R/D/L/U, Bilder F740/F820/F900/F960, Cursor-Mitte (191,118)/(156,150)/(127,86)):
`scratchpad/abn/m1b_bewegt_zoom.png` (Spalten alt S1 | neu S1 | alt S3 | neu S3): alt zeigt abseits der Bildmitte
eine deutlich VERSETZTE, kleinere dunkle Kopie von Kreuz und Eckwinkeln (= "verzerrter/versetzter Schatten" der
frueheren Meldung 2026-09-26), neu an allen vier Lagen und in beiden Skalen keine Kopie.

Cursor-Fenster x 140..179, y 100..137, Bild F640 (Cursor auf der Startzelle, gleiche Schalterlage wie im Original):

| Bild | Abweichungen der Klassenkarte gegen das Original | G | g (dunkelgruen) | Y | y (dunkelgelb) |
|---|---|---|---|---|---|
| Original (DuckStation) | - | 38 | 68 | 14 | 22 |
| neu Skala 1 | **0** (F640 und F680) | 38 | 68 | 14 | 22 |
| alt Skala 1 | 21 (Spalte 151 Zeilen 110-114/123-128 u.a. = innere dunkle Kopie) | 38 | 89 | 14 | 22 |
| neu Skala 3 (Nutzerfenster), je 9 Fensterpunkte = 1 PSX-Punkt | - | 46.6 | 60.8 | 11.7 | 19.6 |
| alt Skala 3 | - | 46.6 | 83.7 | 11.7 | 19.6 |

Verhaeltnis dunkel/hell (gruen): Original 1,79, neu Skala 3 **1,30**, alt Skala 3 1,80 (+23 PSX-Punkte dunkelgruen =
die Kopie). Bild: `scratchpad/abn/m1b_cmp_f000640.png` (oben Skala 1 x3, unten Skala 3; je Original | alt | neu): alt
mit dickem dunklem inneren L in jedem Eckwinkel und versetzter Kreuz-Kopie, neu ohne; der verbleibende feine dunkle
Saum rechts/unten ist der in die Textur gemalte Schlagschatten (TIM @0x018DAC selbst dekodiert,
`scratchpad/abn/cursor_tim_oberseite.png`: dunkelgruene/braune Kante +5u/+3v an jedem Element) — im Original ebenso
sichtbar und bei Skala 3 sogar ANTEILIG kleiner als im Original (1,30 statt 1,79).

## 4. Messung Punkt 1 — ROOM1150 Hebetisch-Cursor ueber den LADE-Weg

Weg wie ein Spieler: Spielstand (probe_r30_granate_karte, ROOM1150) -> Titel -> LOAD GAME (`RE15_CONTINUE_TEST=1
RE15_CARD_AUTO=1`) -> `[save] CONTINUE: resumed in room 1150 (hp=100)` -> Aktionstaste am Tisch (F90) -> cursor.log
`F101 raum=1150 aktiv cursor=2 ... cam=4`, dann Steuerkreuz R/D/L/U (Cursor wandert sx 160->200, sy 119->158 und
zurueck). Laeufe `runs/m2_{neu,alt}_s{1,3}`, RE15_TRILOG ganzer Schirm, Bilder F140..F460.

| Lauf | Platz 28 (RE15_HC_TIM_SLOT) eingereihte Dreiecke | NCLIP < 0 | = 0 | > 0 |
|---|---|---|---|---|
| alt (v0.8.25) | 76744 | **8056** | 27858 | 40830 |
| neu | 68688 | **0** | 27858 | 40830 |

Bild `scratchpad/abn/m2_1150_zoom.png` (5 Cursor-Lagen x [alt S1 | neu S1 | alt S3 | neu S3]): alt an jeder Lage die
versetzte dunkle Kopie (abseits der Mitte deutlich verschoben), neu an keiner. Nebenwirkung derselben Ansicht (Cut 4,
`scratchpad/abn/m2_1150_cut4_alt_neu_mark.png`): 1326 Punkte (x 78..237, y 37..165) = das dunkelbraune Band zwischen den
Papierstapel-Hochhaeusern (Rueckseite von Prop 0, Original-Objekt aus ROOM1150 Obj_model_set) faellt weg, dahinter
Boden/Gitter. Das ist dasselbe Original-Zeichnerpaar FUN_800254a0/FUN_800256b0 mit demselben `bgez` (s. §7) — kein
DuckStation-Bild dieser Ansicht vorhanden (stage_saves/ und analysis/grundwahrheit/ selbst durchsucht), Bewertung
daher nur ueber den Mechanismus.

## 5. Messung Punkt 1 — Elza (ROOM11F1) ueber den echten Raetsel-Einstieg

`RE15_PSELECT_AUTO=1 RE15_PSELECT_AUTO_SWITCH=1`, Debug-Sprung 11F0@250 -> debug.log `[pl] Spieler-Familie PL04
(character=4, Elza-Bit=1)`, `EXIT_AT: Bild 1302 in Raum 11f1`; Panel per Aktionstaste wie §3 -> `[scd F833]
Cut_chg(10)`. Laeufe `runs/m3_elza_{neu,alt}`:

| Lauf | Platz 4 Dreiecke | NCLIP < 0 | Klassenkarte gegen das Original (F900 / F950) |
|---|---|---|---|
| alt | 99428 | 10297 | 21 Abweichungen (F900) |
| neu | 89131 | **0** | **0 / 0** (G 38, g 68, Y 14, y 22 = Original) |

ROOM1151 (Raumvariante, Lade-Weg mit Raum 1151; das Kartenwerkzeug schreibt Familie PL00 — der Cursor-Zeichner ist
spielerunabhaengig): `CONTINUE: resumed in room 1151`, `F101 raum=1151 aktiv cursor=2 cam=4`; Platz 28: alt 4192 von
42824 rueckseitig, **neu 0** von 38632.

## 6. Nebenwirkungen (selbst gemessen, alt = v0.8.25-exe, neu = dieser Baum)

| Raum / Lauf | Ergebnis | Bewertung |
|---|---|---|
| 10F0 Leon, Debug-Sprung, Cut 0, Bild 200 (`runs/m4_10f0_*`) | debug.log neu `[prop-render] pi=4 oid=0x04 pos=(8050,-1460,2700) rot=(2048,256,0)` (alt `rot=(0,256,0)`); Platz 8: neu 560 Dreiecke MAC0 > 0, 0 < 0; alt 560 < 0 | Schluessel sichtbar, Vorderseite (`scratchpad/abn/m4_10f0_schluessel.png`) |
| 1050 Loescher an der Wand (Lade-Weg wie test_r37_feuerloescher Lauf E, Cut 6, Bild 120) | alt gegen neu **0 Punkte** bei Skala 1 und Skala 3 | unveraendert, neu gewickeltes Modell + Verwurf = altes Bild |
| 1090 Loescher liegend (Lauf F, Cut 5, Bild 98) | Skala 1: 2 Punkte (155..158,75..77, Kante eines Original-Props); Skala 3: 16 Punkte derselben Kante + 3 Randpunkte am Loescher (489..492,392..393) | Loescher praktisch unveraendert |
| 1150 Cut 4 (Hebetisch-Sicht) | dunkelbraunes Band faellt weg (§4) | Original-Mechanismus |
| 5010 Debug-Sprung, Bild 200 | 89 Punkte (142..172,117..121): dunkelroter Rueckseiten-Streifen ueber einem Geraet faellt weg, gruene Lampe wird sichtbar (`scratchpad/abn/m6_5010_zoom.png`) | Original-Mechanismus, sieht richtiger aus |
| 2040 Debug-Sprung, Bild 200 | 38 Punkte: dunkler 4x4-Block (3,10,12) neben Leons Hand wird Hintergrund (48,48,44) | Rueckseite eines Raum-Objekts, nicht Leon |
| 20A0 Debug-Sprung, Cut 0, Bild 200 | 0 Punkte | (Spur W: Rolladen in Cut 2, Dossier §4.4 — in der Integration nachmessen) |
| 11D0 Cut 7 (Debug-Sprung, RE15_FORCE_CUT=7) | 493 Punkte (0..35,196..225): helles Blatt (Rueckseite der B1-Lageplan-Karte) lag alt AUF der Mauerkrone ausserhalb des Raums, neu weg (`scratchpad/abn/m7_11d0_cut7_alt_neu.png`) | Artefakt beseitigt; Karte in den Cuts mit Kamera im Raum sichtbar (Scan des Bau-Agenten 14/15) |

Weiterer Cursor-Raum ausserhalb Stage 1 (Stichprobe "zum Beispiel"): ROOM5050 Debug-Sprung, Cut-Durchlauf 0..5 mit
RE15_FORCE_CUT (`runs/m8_5050_c*`): Cursor (Platz 4) nur in Cut 4/5 im Bild; neu 0 von 27968 rueckseitig, alt (Cut 4)
4256 von 32224; Bild `scratchpad/abn/m8_5050_c4_alt_neu.png`: alt mit innerer dunkler Kopie, neu ohne. (ROOM1100: in
keinem der 13 Cuts liegt der Cursor ohne laufendes Raetsel im Bild — nicht weiter verfolgt, gleiche Bauart laut
unit_r38_cursor_raeume.)

ROOM10D0 Ziffernfeld im Nutzerfenster (Skala 3; Weg wie Riegel C: Debug-Sprung, sub17 per RE15_SUBSTART, Text zu,
Bild 260; `runs/m10_10d0_{neu,alt}_s3`): Platz 5 neu 0 von 20535 rueckseitig, alt 2997 von 23532; Bild
`scratchpad/abn/m10_10d0_s3_alt_neu.png`: alt dunkle innere L-Kopien + Kreuz-Kopie, neu ohne.

## 7. Gegenproben der Riegel (selbst, an MEINEN Laeufen statt an denen des Bau-Agenten)

`probe_r38_cursor bild` auf die Ausgaben der echten Wege (§3/§4): alt 11F0 F640 -> **FEHLER (3)** (10345 rueckseitige
Dreiecke, 33 reingruene Punkte im Rahmen, 21 Abweichungen der Klassenkarte), neu -> OK; alt 1150 F140 -> **FEHLER (2)**
(8056 / 33), neu -> OK. `probe_r38_cursor sicht 8` auf 10F0 Cut 0: alt -> **FEHLER (1)** (4 Dreiecke MAC0 < 0), neu ->
OK (4 Dreiecke MAC0 > 0). Die Riegel messen also das Behauptete und schlagen am alten Stand an.

## 8. RE-Gate (selbst disassembliert, `re15_disasm.py` auf info/Re1.5/PSX.EXE, `re2_disasm.py` auf info/re2leon/PSX.EXE)

| Behauptung | selbst gesehen |
|---|---|
| FUN_800254a0 (GT3) verwirft MAC0 < 0 | `lh v0,2(t1)` @0x800254f4, `lh v1,6(t1)` @0x800254f8, `lh v0,10(t1)` @0x80025508 (v0,v1,v2) -> lwc2 @0x8002552c..40 -> RTPT `4a280030` @0x8002554c; NCLIP `4b400006` @0x80025580; `swc2 $24,0(t4=sp)` `e9980000` @0x8002559c; `lw v0,0(sp)` @0x800255a0; **`bgez v0,0x800255d0` @0x800255a8**; sonst `addiu a1,a1,80` @0x800255b0, `j 0x80025694` = `bne t2,zero,0x80025520` (naechste Flaeche) |
| FUN_800256b0 (GT4) auf v0,v1,v2 | `lh` 2/6/10(t2) @0x8002570c/@0x80025710/@0x80025720 -> RTPT @0x80025764; NCLIP @0x80025798; swc2 @0x800257b4; **`bgez v0,0x800257e8` @0x800257c0**; sonst `addiu a1,a1,104`, `j 0x8002591c` (Schleifenende) |
| FUN_80022f0c (flach GT3) | NCLIP @0x80022fec, swc2 @0x80023004, **`bltz v0,0x80023080` @0x80023010**, Ziel = `bne t1,zero,0x80022f84` (Schleife) |
| FUN_80023098 (flach GT4) | NCLIP @0x80023168, swc2 @0x80023184, **`bgez v0,0x800231b8` @0x80023190** |
| Objekt-Schleife ruft nur diese | FUN_8002c18c: Typ (`lbu v1,8(s1)`) == 3 -> `jal 0x8003e64c` @0x8002c2c8; sonst `jal 0x80014368` @0x8002c2dc, Attribut `lw 12(s1) & 0x10` @0x8002c2f4 -> `jal 0x80022f0c` @0x8002c320 + `jal 0x80023098` @0x8002c350, sonst `jal 0x800254a0` @0x8002c380 + `jal 0x800256b0` @0x8002c3b0. `scan`: keine weiteren Aufrufe (kein Schattenzeichner). FUN_8003e64c ruft 0x80022f0c @0x8003e6ac / 0x80023098 @0x8003e6dc |
| RE2 verwirft genauso | FUN_8002d0e8: RTPT @0x8002d170, NCLIP @0x8002d18c, `swc2 $24,0(sp)` `ebb80000` @0x8002d1a0, **`bgez v0,0x8002d1bc` @0x8002d1ac** (Ziel AVSZ3 `4b58002d`), sonst `j 0x8002d148` + `addiu t0,t0,80`. FUN_80027bec: NCLIP @0x80027ca8, **`bgtz` @0x80027cc8** (RE2 verwirft dort auch = 0; der Port folgt RE1.5 `bgez`) |
| RE2-Lagen der flachen Schluessel | ROOM6150.RDT @0x2C98 `2d 08 00 .. 10 00 ba f1 b0 f8 b6 bc c0 ff 00 07 00 06` (Dir -64/0x700/0x600), ROOM2020.RDT @0x1D90 `2d 02 .. ea a2 24 fa 6e a6 00 00 00 00 00 00`, ROOM1050.RDT @0x1274 `2d 00 .. e4 cb 88 fa be c4 00 00 00 00 00 00` — bytegleich mit dem Header-Zitat |
| GTE NCLIP / Saettigung | psx-spx `geometrytransformationenginegte.md` :437-439 (`MAC0 = SX0*SY1 + SX1*SY2 + SX2*SY0 - SX0*SY2 - SX1*SY0 - SX2*SY1`), :418-419 (SX2/SY2 -400h..+3FFh), :294-295 (FLAG 14/13) |
| Port nimmt dieselben Ecken | `re15_md1.h`: Dreieck `n0,v0,n1,v1,n2,v2` (v0 @+2, v1 @+6, v2 @+10), Viereck `n0,v0,..,n3,v3` = die `lh`-Offsets des Originals; Bildschirmwerte mit OFX/OFY = cx/cy = SCREEN_XRES/2, SCREEN_YRES/2 (main.c:5932) wie SX/SY der GTE |
| PSX-Ziel des Ports | `platform/psx/src/mesh_psx.c:489-493` `gte_nclip(); ... if (nclip_result < 0) continue;`, `platform/psx/main.c:719/756` backface_cull 1 |

Konstanten im Diff: `RE15_NCLIP_R38_SXY_MIN/MAX` (-0x400/+0x3FF) = psx-spx :418-419; Schwelle "< 0" = die vier Verzweigungen;
`RE15_R36Q_PRISON_ROT_X 0` = RE2 @0x1274 Dir 0; `RE15_R36Q_PARK_ROT_X 2048` = 180-Grad-Wendung eines flachen Modells mit
Umlauf-Normale -Y, hergeleitet aus dem Verwurf + RE2 @0x2C98 (RE2 wendet dasselbe Modell) — die Achse (x statt z) ist eine
benannte Port-Entscheidung zur Lage der Runde 36 (dort PORT-WAHL); Zeile entfaellt laut Dossier beim Merge mit Spur K.
Feuerloescher-.inc: keine neue Konstante, nur Eckenreihenfolge (Regel w < 0 aus dem Zensus, Riegel unit_r38_cursor_wicklung).
Wort-Suche im Diff (`deferred|tunable|interim|for now|faithful|plausib|TODO|FIXME|approx`): **0 Treffer**; kein neuer
`getenv`-Schalter in engine/platform/include (Verwurf ist unbedingt, kein Env-Ausweg).
**Erklaert der Fix den Befund?** Ja: gemessen fallen genau die NCLIP<0-Dreiecke des Cursor-Platzes weg (Zaehlungen = 0 und
> 0 alt/neu identisch, §3-§5), und das Bild ist danach klassengleich mit dem Original-Bild.

## 9. Vertrag

* Pfade (`git diff master...HEAD --name-only`): nur analysis/befunde_runde38/A_*, engine/src/{nclip_r38.c,
  schluessel_r36.c, gen/feuerloescher_r37.inc}, include/{re15_nclip_r38.h, re15_schluessel_r36.h}, platform/pc/{main.c,
  src/hebetisch_cursor_pc.c}, tests/{integration/test_r38_cursor.cmake, unit/probe_r38_cursor.c, unit/probes/r38_cursor.cmake},
  tools/r37_feuerloescher/feuerloescher_modell.py. **Kein** release/, platform/android/, shared_assets/PSX/, keine
  Test-CMakeLists. (`git diff master --stat` zeigt zusaetzlich AUFTRAG/VERTRAG/orchestrator — das sind master-Commits nach der
  Basis 1bcbd0ff, nicht Aenderungen dieses Zweigs.)
* Gemeinsame Dateien: main.c +3 (Include + 2 Haken, je Kommentar "Runde 38 Spur A") — klein. schluessel_r36.c +2
  ("Runde 38 A"), re15_schluessel_r36.h +2 Defines (+10 Kommentarzeilen) — Dateien der Spur K (VERTRAG §1.4), klein;
  gen/feuerloescher_r37.inc (610 Zeilen, mit dem Werkzeug neu erzeugt) + Werkzeug +29 — kein "Haken", aber notwendig fuer
  die eigene Nebenwirkung und konfliktfrei (s.u.).
* Bank-9-Bits / Nachrichten-Ids / Slots / Ereignisse: keine belegt (A hatte keine Zuteilung) — eingehalten.
* Probe-Merge `git merge-tree --write-tree r38/integration r38/cursor`: Konflikte NUR in schluessel_r36.c und
  re15_schluessel_r36.h (Spur K, bereits in r38/integration, Commit 97490964) — genau der im Dossier §3c/§8 beschriebene
  Fall ("K's Seite nehmen"); main.c, .inc, Werkzeug mischen automatisch.

## 10. Befund des Bau-Agenten selbst nachgeprueft (Modell)

Eigener MD1-Leser auf ROOM11F0.RDT @0x001928 (65 Bytes Kopf-Laenge-Feld, 1 Netz, 16 Dreiecke, 98 Vierecke): Unterseite
y = +923 = Dreiecke **T03 (u 4..125, v 41..82), T04 (u 65..125, v 0..82), T05 (u 4..125, v 0..41), T11 (u 4..65, v 0..82)**
— das ganze Muster-Feld; Oberseite y = -900: 49 Vierecke im Musterfeld (v <= 82). Umlauf in (x,z) von oben: Unterseite
"+", Oberseite "-" — gegensinnig, also ist bei Blick von oben genau eine der beiden Flaechen rueckseitig. Das deckt die
Ursache "Unterseite mit demselben Muster scheint durch die weitgehend durchsichtige Oberseite" (selbst gezaehlt: im
Musterfeld u 4..125, v 0..82 sind 7682 von 10126 Texeln = 75,9 % Index 0 = durchsichtig; die 92,8 % des Dossiers beziehen
sich auf die ganze Oberseite, nicht nachgezaehlt).

## 11. Gezielte Regression (Regex des Bau-Agenten, gegen `ctest -N` und die 20 Bild-Haken-Skripte geprueft)

Regex = r38_cursor + Bild-/Prop-/Panel-/Hebetisch-Gruppen (`scratchpad/abn/rx.txt`; alle 20 Integrationsskripte mit
RE15_FRAMEDUMP/FBDUMP/TRILOG sind darin, `grep -l` selbst geprueft).
* Teil 1 (`-E "^integration_" -j4 --timeout 600`): **`100% tests passed, 0 tests failed out of 148`** (7.98 sec).
* Teil 2 (48 Laeufe mit der exe, `-E "^unit_|^integration_r38_cursor$" -j2 --timeout 900`; integration_r38_cursor
  separat in §1): **`100% tests passed, 0 tests failed out of 48`** (Total Test time (real) = 3367.40 sec, parallel zu
  Laeufen anderer Spuren auf derselben Maschine; kein roter Fenster-Haken, nichts nachzufahren). Darunter
  integration_r34n_b_cursor, integration_r37_codepanel, integration_r36_schluessel(+_befreiung),
  integration_r37_feuerloescher, integration_r36_kartenitems_bild, integration_r34n_e_dokumente_bild,
  integration_r30_irons_tisch_bild, integration_r30_sicherung_bild, integration_r36_pri_schatten,
  integration_r37_pri1100_{marke,elza}, integration_pri_masken.
* Summe gezielt: 148 + 48 + 1 = **197/197 gruen**.

## 12. Integration mit K (bereits in r38/integration) und F — selbst nachgerechnet

* Spur K, Daten aus `r38/integration` (`gen/itemorte_r38_md1.inc`, `gen/itemorte_r38_lage.inc`): beide Pinnwand-Modelle
  (k_r38k_park_md1 / k_r38k_prison_md1, je 4 Dreiecke) haben Umlauf-Normale (v1-v0)x(v2-v0) = (0,-1,0); mit der
  Port-Matrix `pc_prop_rot_q12` (main.c:687, Ry*Rx*Rz) und K's Lagen PARK rot (3072,0,2048) / PRISON rot (1024,0,0)
  -> Welt (0,0,-1) = in die Wand z = -3400. Kameras ROOM10F0/10F1 (RDT offs[7] = 0x60, 12 Saetze a 32 B, selbst
  gelesen): z von -1637 bis 12824, alle > -3400 -> die Umlauf-Normale zeigt in JEDEM Cut von der Kamera weg ->
  MAC0 > 0 -> sichtbar mit dem Verwurf. Gegenprobe der Regel am Cursor selbst: Oberseite (gezeichnet) hat g_y > 0,
  Unterseite (verworfen) g_y < 0 bei Blick von oben (§10).
* Spur F (`r38/dining1141`, `gen/dining_r38_schluessel.inc`): re15_r38f_key_md1 4 Dreiecke, g = (0,+5776,0) = +Y,
  Lage rot_y 0 flach am Boden ROOM1011 (-2000,0,7450) -> g zeigt von der Kamera (oben) weg -> sichtbar.
* Daraus fuer die Integration: Konflikt schluessel_r36.c/.h -> K's Seite (wie Dossier §8); danach
  `integration_r38_cursor` (E/F an K's Pinnwand-Cuts 5/6) und `unit_r38_cursor_wicklung` an der Integrations-exe fahren;
  Spur W (r38/rolltor20a0) hat r38/cursor bis 10e39eda bereits gemergt (fb5111e8) und misst mit dem Verwurf.

## 13. Urteil

| Punkt (Wortlaut) | Urteil | Beleg |
|---|---|---|
| "Der Cursor, der zum Beispiel in ROOM 1150 oder ROOM 11F0 verwendet wird, hat immer noch so einen komischen Schatten. Den hat er im Original nicht..." | **erfuellt** | 11F0 ueber Aktionstaste am Panel (Debug-Sprung UND Lade-Weg) und Elza 11F1: 0 rueckseitige Cursor-Dreiecke (alt 10345/10297), Klassenkarte = DuckStation-Original (0 Abweichungen; alt 21); 1150/1151 Lade-Weg: 0 (alt 8056/4192); bewegter Cursor an 4 bzw. 5 Lagen ohne Kopie; Nutzerfenster Skala 3 (befund.log des Nutzers: `[window] windowed 960x720`): keine versetzte Kopie mehr, verbleibender feiner Saum = gemalter Textur-Schlagschatten, anteilig kleiner als im Original (dunkel/hell 1,30 gegen 1,79). Ursache (Unterseite T03/T04/T05/T11 mit dem Muster, gegensinnig gewunden) selbst im MD1 nachgelesen, Original-Verwurf an vier Zeichnern + Dispatch selbst disassembliert. |

Gates: Bau OK (§0); RE-Gate OK (§8); Vertrag OK (§9, Hinweis H1); Riegel vorhanden und messend (§1, §7);
gezielte Regression 197/197 gruen (§11). **Ergebnis: bestanden.**

## 14. Maengel / Hinweise

Maengel: keine.

Hinweise (nicht blockierend, fuer Integration/Orchestrator):
* H1 Vertrag §1.4: ausserhalb der eigenen Module geaendert — re15_schluessel_r36.h (+2 Defines, +10 Kommentarzeilen),
  gen/feuerloescher_r37.inc (mit dem Runde-37-Werkzeug neu erzeugt, nur Eckenreihenfolge von 112 Vierecken) und
  tools/r37_feuerloescher/feuerloescher_modell.py (+29). Notwendig fuer die eigenen Nebenwirkungen (§6: Loescher 1050
  danach pixelgleich), Probe-Merge gegen r38/integration: nur schluessel_r36.c/.h im Konflikt -> K's Seite nehmen.
* H2 Nach dem Merge an der Integrations-exe fahren: integration_r38_cursor (E/F = K's Pinnwand-Cuts 5/6; nachgerechnet
  §12: sichtbar), unit_r38_cursor_wicklung; Spur-W-Messung ROOM20A0 Cut 2 mit Verwurf (W hat r38/cursor bis 10e39eda
  bereits gemergt); Spur F (Herz-Schluessel) ist nachgerechnet sichtbar (§12).
* H3 Fuer ROOM1150 Cut 4 (Band hinter den Papierstapel-Hochhaeusern faellt weg) und ROOM10D0 gibt es kein
  DuckStation-Bild; Bewertung nur ueber den disassemblierten Mechanismus (derselbe Zeichner fuer alle Objekte).
* H4 Harness-Beobachtung (nicht Spur A, alt und neu gleich): Text 0 des 11F0-Panels ist mehrseitig und langsam; ein
  Messlauf braucht ~16 Aktions-Druecke im Abstand von 1 s, bis `Cut_chg(10)` kommt (F603) — kein Haenger, nur Tipp fuer
  kuenftige Messlaeufe.
