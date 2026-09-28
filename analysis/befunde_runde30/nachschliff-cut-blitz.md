# Runde 30 Nachschliff, Spur cut-blitz — ein Bild lang neue Kamera auf altem Hintergrund

Zweig `r30/n-cut-blitz`, Arbeitsbaum `.claude/worktrees/r30_n_cut`, Basis cac33993.
Werkzeuge: `analysis/befunde_runde30/nachschliff-cut-blitz_tools/`
(`r30_cb_lauf.sh` = ein Messlauf am gebauten Spiel, `r30_cb_pixel.py` = Pixelvergleich zweier
Framedumps). Läufe und Bilder unter `build/r30_cut_blitz/<marke>/` (nicht eingecheckt).

## 1. Symptom (Befund des Gegenprüfers)

ROOM1150, Tür-/Sprung-Weg, zu Fuß über Zone 6 nach Cut 2, Stand (−22691, −19941), Viereck
in F329 (Telefon = Speicherpunkt, sub06 `Cut_chg(6)`). Framedump F330: Buch und Memory Card
erscheinen groß und fast schwarz mitten auf dem Schreibtisch, 11060 abweichende Pixel,
Hülle x 148,5..213,2 / y 108,5..145,2 — das Bild des erzwungenen Cut 6 auf dem Hintergrund
von Cut 2. cam-trace F330: req=6, shown=2.

## 2. Messung vorher

### 2.1 Messschiene `RE15_CUT_SYNC_LOG` (neu, kein Verhalten)

`main.c pc_cut_sync_log`, je Spielbild NACH `re15_render_end_frame` eine Zeile:

    F<bild> room=<raum> bg=<raum>#<cut> view=<cut> sync=<0|1|-1> req=<cam_id> shown=<wv0A>
    fade0=<rgb>/<abr> tris=<n> dc=<todeskamera>

* `bg` — Herkunft des Hintergrunds, der in DIESEM Bild in den Framebuffer ging
  (`bg_pc.c re15_bg_last_blit_tag`, gesetzt in `re15_bg_blit` und `re15_bg_blit_montage`).
* `view` — der Cut der Kameramatrix, mit der die 3D-Projektion dieses Bilds lief. Nicht aus
  einer Variablen abgeschrieben, sondern zurückgerechnet: die tatsächlich benutzte `cam_view`
  gegen `re15_camera_build_view` jedes Cuts des Raums (erster Treffer).
* `sync` — 1: `cam_view` == Ansicht des Hintergrund-Cuts; 0: verschieden (Blitz-Bild).
* `fade0` — Überblendkanal 0 (255/2 = volles subtraktives Schwarz, dann ist nichts zu sehen);
  `tris` — gezeichnete texturierte Dreiecke (0 = kein 3D im Bild).

### 2.2 Nachstellung des Gegenprüfer-Wegs (Lauf `v_telefon2`)

Eingabe wörtlich aus dem Gegenprüfer-Lauf `laeufe/telefon2` übernommen
(`RE15_INPUT_SCRIPT="R0.6333,W0.2,U2.4333,W0.2,L0.2667,W0.2,U2.8,W0.5,R0.3333,W0.5,A0.04,W3"`,
Start Tick 330, Sprung 1150@240). cam-trace identisch zum Gegenprüfer (F143 0→1, F239 1→2,
F300 Stand (−22691,−19941), F330 req=6 shown=2).

| Bild | bg | view | sync | tris |
|---|---|---|---|---|
| F143 (Zone 0→1) | 1150#0 | 1 | **0** | 354 |
| F239 (Zone 6, 1→2) | 1150#1 | 2 | **0** | 353 |
| F330 (Telefon, `Cut_chg(6)`) | 1150#2 | 6 | **0** | 347 |

Pixel (`r30_cb_pixel.py`, Nulllauf `v_telefon2_null` = derselbe Weg mit den Genommen-Bits
(9,54)/(9,55), also ohne Buch und Karte):

| Vergleich | abweichend | Hülle (320er Lage) | RGB |
|---|---|---|---|
| F328 mit gegen ohne Props | 704 | x 136,5..159,2 / y 121,5..128,2 | (47,36,28) |
| F329 mit gegen ohne Props | 704 | x 136,5..159,2 / y 121,5..128,2 | (47,36,28) |
| **F330 mit gegen ohne Props** | **11060** | **x 148,5..213,2 / y 108,5..145,2** | (17,16,8) |

Damit ist der Befund des Gegenprüfers bitgenau nachgestellt (11060 Pixel, dieselbe Hülle).

### 2.3 Zensus: tritt der Versatz bei JEDEM Kamerawechsel auf?

| Lauf | Weg | Wechsel | davon `sync=0` | sichtbar |
|---|---|---|---|---|
| `v_telefon2` | Zonenwechsel zu Fuß (RVD) ROOM1150 | 2 (0→1, 1→2) | 2 | ja (tris > 0) |
| `v_telefon2` | SCD `Cut_chg` (Telefon) | 1 | 1 | ja |
| `v_telefon` | SCD `Cut_chg` aus Cut 0 (Spawn am Telefon) | 1 | 1 | ja |
| `v_intro` | ROOM1170 SCD `Cut_chg` (Hubschrauber-Szene) | 8 | 8 | 7 ja, F445 unter vollem Schwarz |
| `v_intro` | ROOM1170 Zonenwechsel (Leon läuft geskriptet) | 3 (0→1, 1→2, 2→3) | 3 | ja |
| `v_intro` | ROOM1240 Vorspann-Montage `Cut_chg` | 9 | 0 | kein 3D (tris=0) |
| `v_tuer` | echte Tür ROOM1150 → ROOM1130 (Viereck an der Tür) | 1 | Warp-Bild: BG 1150, 3D 1130 | nein: fade0=255/2 |
| `v_telefon2` | Debug-Sprung ROOM1240 → ROOM1150 | 1 | Warp-Bild wie Tür | nein: fade0=255/2 |

Ergebnis: **jeder** Kamerawechsel über den Präsentations-Apply (`re15_cam_present_tick`) —
Zonenwechsel zu Fuß wie SCD `Cut_chg` — erzeugt genau EIN Bild mit neuer Projektion auf altem
Hintergrund (14 von 14 Wechseln mit 3D). Der Raumwechsel (Tür, Sprung) hat dasselbe Mischbild
im Warp-Bild, dort deckt die Tür-Blende (Kanal 0 auf 0x7fff, subtraktiv 255) es vollständig ab.
Die ROOM1240-Montage hat kein 3D.
