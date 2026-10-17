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
