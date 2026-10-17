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

(laufend — wird nach jeder Messung ergaenzt)

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
