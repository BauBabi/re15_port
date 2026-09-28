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

## 3. Original-Mechanismus (PSX.EXE, selbst disassembliert aus `ghidra1_V2.txt`)

### 3.1 Reihenfolge je Durchlauf

Hauptschleife `main` (Kopf `LAB_80020c10`):

    80020c58 b3 a3 01 0c  jal ClearOTagR     OT[DAT_800aca34] (8 / 0x400 / 0x10 Eintraege,
    80020c6c b3 a3 01 0c  jal ClearOTagR      @0x80020c4c/60/74 lbu DAT_800aca34)
    80020c80 b3 a3 01 0c  jal ClearOTagR
    ...                   Task-Planer FUN_800298b0 (ChangeTh) -> Spiel-Task
    80020f34 83 86 00 0c  jal FUN_80021a0c   Letterbox
    80020f3c 00 40 00 0c  jal FUN_80010000
    80020f44 20 86 00 0c  jal FUN_80021880   Blenden-Takt
    80020f4c df 84 00 0c  jal FUN_8002137c   PRESENT
    80020f54 04 83 00 08  j   LAB_80020c10

Spiel-Task (Rumpf ab `LAB_8001c97c`, Schleife `@0x8001d1f8 bne ... LAB_8001c97c`):

    8001ccec 8c 50 00 0c  jal FUN_80014230   RVD-Zonen-Scan (schreibt nur DAT_800afbb5 @0x80014300)
    8001cdec 0e fc 00 0c  jal FUN_8003f038   SCD (Cut_chg: @0x800402f4 sb 1,DAT_800b5457;
                                             @0x800402fc sh DAT_800b0fe4; @0x80040300 jal FUN_800142f4)
    8001d1b8 28 e7 00 0c  jal FUN_80039ca0   NPC zeichnen
    8001d1c0 63 b0 00 0c  jal FUN_8002c18c   Props zeichnen
    8001d1e0 b2 a6 00 0c  jal FUN_80029ac8   (a0=1) Bildende der Task

Anforderung (Zone oder SCD) und Zeichnen in die OT desselben Durchlaufs laufen also VOR der
Present-Routine, mit der Kamera, die der letzte Apply gesetzt hat.

### 3.2 Present FUN_8002137c

    800214d4 84 a4 01 0c  jal PutDispEnv          (jedes Bild, VOR dem Vergleich)
    800214f8 e4 0f 63 84  lh  v1,DAT_800b0fe4     angezeigter Cut
    80021500 b5 fb 42 90  lbu v0,DAT_800afbb5     angeforderter Cut
    80021508 03 00 62 10  beq v1,v0,LAB_80021518
    80021514 57 54 22 a0  sb  v0(=1),DAT_800b5457 dirty
    80021538 0b 00 40 10  beq dirty,zero,LAB_80021568   -> Normalpfad
    80021558 ef 86 00 0c  jal FUN_80021bbc        APPLY
    80021560 7f 85 00 08  j   LAB_800215fc        <- springt UEBER:
    8002157c 1c 0e 01 0c    jal FUN_80043870      Hintergrund LoadImage(0x80198000 -> Zeichenpuffer)
    800215bc f1 a3 01 0c    jal DrawOTag  (x3: @0x800215bc / @0x800215d0 / @0x800215e4)
    800215f4 01 00 42 38    xori v0,v0,0x1
    800215f8 00 00 02 a2    sb  v0,DAT_800aca34   Pufferwechsel

Im Apply-Bild wird die OT, die mit der ALTEN Kamera gebaut wurde, nie gezeichnet, und der
Pufferwechsel bleibt aus: `PutDispEnv` zeigt im nächsten Present denselben Puffer, also das
vorige Bild noch einmal. Die nächste Schleife löscht die OT (`ClearOTagR` oben) und baut sie
mit der neuen Kamera neu.

### 3.3 Apply FUN_80021bbc — Hintergrund und Kamera in einem Zug

    80021bfc e4 0f 23 a4  sh  v1,DAT_800b0fe4     angezeigter Cut = angeforderter
    80021d2c 14 4f 00 0c  jal FUN_80013c50        Hintergrund von CD nach 0x80190000
    80021e34 a3 4e 01 0c  jal FUN_80053a8c        MDEC in den Zeichenpuffer (Rechteck y = 0/0xf0)
    80021e44 3b a3 01 0c  jal StoreImage          Kopie nach 0x80198000 (die FUN_80043870 danach
                                                  in jedem Bild zurückschreibt)
    80021e58 e4 0f 42 84  lh  v0,DAT_800b0fe4
    80021e68 02 00 44 94  lhu a0,0x2(v0)          fov des Cuts
    80021e6c 0c 9b 01 0c  jal FUN_80066c30        = gte_ldH
    80021e70 c2 21 04 00  srl a0,a0,0x7           (Verzögerungsschlitz: H = fov >> 7)
    80021e80 e4 0f 84 84  lh  a0,DAT_800b0fe4
    80021e8c 29 4f 01 0c  jal FUN_80053ca4        Blickmatrix DAT_800b5288 aus Cut-Satz (+0x24, x0x20)
    80021e98 57 54 20 a0  sb  zero,DAT_800b5457

`FUN_80053ca4` hat in der EXE sonst nur drei Aufrufstellen: @0x80015a68 (Todeskamera
FUN_80015850), @0x80016494 (Spielstart/Raumladen), @0x80046140 (Moduswechsel FUN_800460b8).
Dynamisch (3.4) kommen Aufrufe aus Overlay-Code dazu (Rücksprung 0x801015ac 41-mal,
0x80101bdc einmal), alle VOR dem ersten Spielbild (Vorspann/Titel). Im Spiel setzt allein der
Apply die Blickmatrix — Hintergrund, Projektionsabstand und Blickmatrix wechseln gemeinsam.

**Zeigt das Original selbst ein Bild Versatz?** Nein. Jedes gezeigte Bild trägt in Hintergrund
und Projektion denselben Cut. Das Bild des Anforderungsdurchlaufs (alte Kamera, neue Logik)
wird verworfen, stattdessen steht das vorige Bild ein weiteres Mal (plus die CD-Ladezeit von
FUN_80013c50); das erste Bild des neuen Cuts ist das im nächsten Durchlauf gezeichnete.

### 3.4 Dynamischer Beleg im Original (PCSX-Redux)

`nachschliff-cut-blitz_tools/r30_cb_present_trace.lua`, gestartet über `pcsx_drive.py` des
Skills re15-pcsx-watchpoint (`-interpreter -debugger`, MZD-Disc; deren Bytes an
@0x80021558 `ef 86 00 0c 00 00 00 00 7f 85 00 08` und @0x80021e6c `0c 9b 01 0c c2 21 04 00`
sind gleich der `info/Re1.5/PSX.EXE` @0x11d58 / @0x1266c). Exec-Haltepunkte (Rückruf immer
`true`) auf Present-Eintritt 0x8002137c, Apply-Aufruf 0x80021558, BG-LoadImage 0x8002157c,
erstes DrawOTag 0x800215bc, Pufferwechsel 0x800215f8, CD-BG 0x80021d2c, Blickmatrix
0x80021e8c und den Eintritt 0x80053ca4 (Rücksprungadresse gezählt). New Game, ROOM1240-Montage,
ROOM1170-Hubschrauberszene; Lauf bis Present 7603.

| | Presents | DrawOTag | Pufferwechsel | BG-LoadImage | CD-BG | Blickmatrix | Puffer davor = danach |
|---|---|---|---|---|---|---|---|
| mit Apply | **22** | **0** | **0** | **0** | 22 | 22 | 22 von 22 |
| ohne Apply | 7581 | 7581 | 7581 | — | — | — | — |

`FUN_80053ca4`-Rücksprünge: 0x80021e94 (= Apply @0x80021e8c) 22-mal, 0x8001649c (@0x80016494)
3-mal (Raumladungen), Overlay 0x801015ac 41-mal und 0x80101bdc einmal — die beiden letzten
alle vor dem ersten Apply (Vorspann).

Die 22 Applies in Reihenfolge (angezeigter Cut beim Eintritt → nach dem Apply):
Montage 0,0,1,2,3,4,5,6,7,8; ROOM1170 7, 0, 2, 1, 2, 0, **0→1, 1→2, 2→3** (Zonenwechsel:
nur der Apply ändert DAT_800b0fe4), 6, 4, 3. Zonenwechsel und SCD `Cut_chg` laufen durch
denselben Apply, und in KEINEM Apply-Bild wird gezeichnet oder der Puffer gewechselt.

**Zeitvergleich ROOM1170** (Abstände zwischen den Applies, Original in Presents, Port in
Bildern der `[fade-log] CUT`-Zeilen):

| Wechsel | Original | Port vorher | Port nachher |
|---|---|---|---|
| 7→0 → 0→2 | 34 | 34 | 34 |
| 0→2 → 2→1 | 503 | 503 | 503 |
| 2→1 → 1→2 | 61 | 61 | 61 |
| 1→2 → 2→0 | 187 | 187 | 187 |
| 2→0 (SCD) → 0→1 (Zone) | **94** | 95 | **94** |
| 0→1 → 1→2 (Zone) | 36 | 36 | 36 |
| 1→2 → 2→3 (Zone) | 32 | 32 | 32 |
| 2→3 (Zone) → 3→6 (SCD) | 19 | 20 | 21 |
| 3→6 → 6→4 | 135 | 135 | 135 |
| 6→4 → 4→3 | 220 | 220 | 220 |

Vorher wandte der Port SCD-Anforderungen im selben Bild, Zonen-Anforderungen ein Bild später
an; jetzt beide ein Bild nach der Anforderung wie der Present des Originals — die Zeile
SCD→Zone stimmt damit (94). Die Zeile Zone 2→3 → `Cut_chg(6)` weicht in der LOGIK um zwei
Bilder ab (Anforderung Port 1392 / 1413, also 21, gegen 19 im Original); das liegt vor dem
Apply (Laufweg oder Skripttakt der Szene) und ist hier nicht behandelt (Abschnitt 7).

## 4. Ursache im Port

`platform/pc/main.c`, Reihenfolge je Bild vor dem Umbau:

1. `re15_bg_blit` — der Hintergrund des bis dahin angezeigten Cuts geht in den Framebuffer,
2. `scd_vm_tick` — `Cut_chg` setzt `g_scd.cam_id`,
3. `re15_cam_present_tick` + Apply im Zeichenblock — `re15_bg_load_cut(neu)` lädt nur den
   CACHE, im Framebuffer steht schon der alte,
4. `view_cut = active_cuts[active_cut_idx]` — Projektion mit der NEUEN Kamera,
5. `re15_game_step` (Zonen-Scan, AOT, Tür) und das Zeichnen.

Anforderungen aus dem Zonen-Scan (5. in Bild N) wurden in Bild N+1 unter 3. angewandt,
SCD-Anforderungen (2.) im selben Bild — in beiden Fällen NACH 1. Das ist die ganze Ursache;
der Weg (Zone, SCD) spielt keine Rolle. Die PSX-Fassung des Ports hat den Fehler nicht: sie
blittet den Hintergrund erst in `end_frame` und lässt im Wechselbild das 3D weg
(`skip_3d_frame`).

Nebenbefund derselben Klasse: `pc_fx_set_camf(..., g_scd.cam_id)` nahm den Projektionsabstand
der Effekte aus dem ANGEFORDERTEN Cut. Im Original lädt H nur der Apply, aus DAT_800b0fe4
(@0x80021e58–@0x80021e70).

## 5. Änderung

* `pc_cam_present_apply()` (Dateiebene in `main.c`): RE15_FORCE_CUT, cam-trace,
  `re15_cam_present_tick` und der Apply (Regions-Viereck, Montage-Schnappschuss, BG-Laden,
  Licht) laufen am **Bildanfang, vor dem Hintergrund-Blit und vor dem SCD-Takt** — das ist
  die Stelle von FUN_8002137c am Ende des vorigen Durchlaufs. `s_last_cut_idx` und
  `cam_region_*` stehen dafür auf Dateiebene; der Zeichenblock liest nur noch.
* Effekte: `pc_fx_set_camf(..., active_cut_idx)` — H des angezeigten Cuts.
* `re15_scd.h`: Aufrufstelle im Kommentar von `re15_cam_present_tick` nachgetragen.
* **Port-Wahl, keine Original-Adresse:** das Wiederholen des vorigen Bilds im
  Anforderungsbild (@0x80021560) ist NICHT nachgebaut. Der Port zeigt dort das mit dem alten
  Cut gezeichnete Bild (Hintergrund UND Projektion alt, Logik dieses Bilds). Beide Bilder
  tragen denselben Cut; der Unterschied ist die Logik eines Bilds. Grund: der PC präsentiert
  jedes Bild sofort (kein Doppelpuffer-Versatz wie `PutDispEnv` vor `DrawOTag`), ein
  Auslassen von `SDL_RenderPresent` würde Taktung und Framedump berühren. Gemessen: das so
  gezeigte Bild ist in sich stimmig (6.1: F330 = F329 bitgleich).
* Zeitfolge: ein Wechsel, den Zonen-Scan oder SCD in Bild N anfordern, erscheint in Bild
  N+1 — beim Zonenwechsel dasselbe Bild wie vorher (F143, F239), beim SCD `Cut_chg` ein Bild
  später als vorher (ROOM1170: F480 statt des Mischbilds F479). Das entspricht dem Original:
  dort erscheint der neue Cut erst mit der nach dem Apply gezeichneten OT. Nebenwirkung auf
  die Logik: `work_vars[0x0A]` eines Zonenwechsels steht jetzt schon beim SCD-Takt des
  nächsten Bilds (wie im Original, dessen SCD nach dem Present läuft); vorher sah der SCD
  noch ein Bild lang den alten Wert.
* Tür/Sprung (`re15_room_apply_pending` im Spielschritt) bleibt unberührt: das Warp-Bild
  liegt unter voller Türblende (gemessen: 0 nicht-schwarze Pixel), wie im Original
  (Mode-2-Schwarz im Raumlader FUN_8001d600).

## 6. Messung nachher

### 6.1 Telefon-Weg (Abnahme)

Läufe `n_telefon2` / `n_telefon2_null` (gleiche Eingabe wie 2.2):

| Vergleich | vorher | nachher |
|---|---|---|
| F330 mit gegen ohne Props | 11060 Pixel, Hülle x 148,5..213,2 / y 108,5..145,2, RGB (17,16,8) | **704 Pixel, Hülle x 136,5..159,2 / y 121,5..128,2, RGB (47,36,28)** = Props in Cut-2-Lage, gleich F328/F329 |
| F330 gegen F329 (mit Props) | 223660 | **0** |
| F330 gegen F329 (ohne Props) | 222442 | **0** |
| F329 vorher gegen nachher | — | **0** |
| cutsync F330 | bg 1150#2, view 6, sync 0 | bg 1150#2, view 2, sync 1 (req 6 wird im nächsten Bild angewandt) |

### 6.2 Zonenwechsel zu Fuß (ROOM1150, `b_zonen` = Basis-exe cac33993 / `n_zonen`)

| Vergleich | vorher | nachher |
|---|---|---|
| F142 gegen F143 | 223131 (nur das 3D springt) | 689699 (ganzes Bild: neuer Cut) |
| F143 gegen F144 | 482756 (erst jetzt der Hintergrund) | **6497** (nur die Figur läuft, Hülle x 167..189) |
| F238 gegen F239 | 304695 | 688034 |
| F239 gegen F240 | 493013 | **28156** (nur die Figur, Hülle x 191..249) |

Vorher gegen nachher: F142, F144, F238, F240 je **0**; nur F143 (477447) und F239 (468937)
unterscheiden sich — genau die beiden Mischbilder.

### 6.3 SCD `Cut_chg` ROOM1170 (Sprung 1170@240, `b_1170` / `n_1170`)

| Vergleich | vorher | nachher |
|---|---|---|
| F478 gegen F479 | 156777 (3D in Cut 2 auf Hintergrund Cut 0) | 17019 (nur die Textzeile, y 181..208) |
| F479 gegen F480 | 424972 | 518437 (ganzes Bild: neuer Cut) |
| F445 (Selbst-Wiedereintritt) | 0 nicht-schwarze Pixel | 0 nicht-schwarze Pixel |

Vorher gegen nachher: F444–F447, F478, F480–F482 je **0**; F479 (139758).

**Korrektur (Nachbesserung, Abschnitt 8.1):** der Schluss „nur F479 weicht ab“ war falsch — F448–F477 waren nicht verglichen. In dieser Fassung (e998bd32) wichen zusätzlich F448–F453 ab (Elliots Kopf, 8/579/926/1020/351/352 Pixel). Behoben in 8.1; danach weicht F440–F491 nur F479 ab.

### 6.4 Tür ROOM1150 → ROOM1130 (`b_tuer` / `n_tuer`, Viereck an der Tür F60)

Warp-Bild F0: 0 nicht-schwarze Pixel in beiden Fassungen; F0, F1, F2, F5, F8 vorher gegen
nachher je **0** Pixel.

### 6.5 Vorspann-Zensus nachher (`n_intro`, New Game bis ROOM1170 Bild ~2400)

3931 Bilder, ein einziges `sync=0`: F0 in ROOM1170 = Warp-Bild aus ROOM1240 (bg 1240#8,
fade0 255, tris 0). Alle elf Wechsel in ROOM1170 (F446, F480, F983, F1044, F1231, F1325,
F1361, F1393, F1414, F1549, F1769) in `sync=1`. Vorher: elf `sync=0` (Abschnitt 2.3).

### 6.6 Riegel `integration_r30_cut_blitz`

`tests/integration/test_r30_cut_blitz.cmake`, angemeldet in `tests/unit/probes/r30_cut-blitz.cmake`.
Zwei Läufe der echten exe mit `RE15_CUT_SYNC_LOG`; verlangt in jedem Bild mit 3D `sync=1`
(außer unter voller Türblende, fade0=255) und die Wechsel selbst (A ≥ 2, B ≥ 1).
Gemessen: A 501 Bilder, 261 mit 3D, Wechsel F143 und F239; B 731 Bilder, 45 mit 3D,
Wechsel F446 und F480; grün in 42 s.

**Mutationsprobe:** Aufruf von `pc_cam_present_apply` hinter den Hintergrund-Blit verschoben
→ **rot**, gemeldet werden genau F143 (bg 1150#0, view 1) und F239 (bg 1150#1, view 2).
Danach `git checkout` der Datei und Neubau.

### 6.7 Suite

`ctest --test-dir re15_port/build --timeout 240`: **395/395 grün** (394 Bestand + der neue
Riegel).

## 7. Offen / nicht gemessen

* **Bildwiederholung im Anforderungsbild** (Original @0x80021560: kein DrawOTag, kein
  Pufferwechsel, dynamisch 22/22) ist nicht nachgebaut — Port-Wahl, keine Original-Adresse
  (Abschnitt 5). Der Port zeigt dort das mit dem alten Cut gezeichnete, in sich stimmige Bild.
* **Logik-Versatz ROOM1170** Zone 2→3 → `Cut_chg(6)`: 21 Bilder im Port gegen 19 Presents im
  Original (3.4). Unabhängig vom Apply; nicht untersucht.
* **Tür-Warp-Bild**: im Port weiter Hintergrund des alten Raums + 3D des neuen, vollständig
  unter der Türblende (0 nicht-schwarze Pixel, 6.4). Nicht umgebaut, weil unsichtbar; der
  Riegel lässt `sync=0` nur unter fade0=255 zu.
* **PSX-Fassung des Ports** (`platform/psx/main.c`) nicht angefasst: sie blittet den
  Hintergrund in `end_frame` und lässt im Wechselbild das 3D weg (`skip_3d_frame`), hat den
  Mischfehler also nicht. Nicht gebaut/gemessen (PSX-Ziel baut derzeit nicht).
* **gdigrab** am Fenster nicht gemacht; die Bildabnahme läuft über RE15_FRAMEDUMP
  (Readback vor SDL_RenderPresent, beschleunigter Renderer).

## 8. Nachbesserung nach der Gegenprüfung

Der Gegenprüfer fand zwei Nebenwirkungen des Apply-Vorzugs und zwei Lücken im Riegel. Beide
Nebenwirkungen hat die erste Abnahme (Abschnitt 6) übersehen: dort wurden in ROOM1170 nur
F444–F447, F478 und F480–F482 verglichen, und die Montage wurde gar nicht verglichen. Die Aussage
„nur F479 weicht ab“ in 6.3 war deshalb falsch (Korrektur dort).

Läufe unter `build/r30_cut_blitz/<marke>/`. „vorher“ ist die exe des Gegenprüfers aus 4273b3f6
(Messschiene, Verhalten wie cac33993), `build/r30_pruef_n_cut-blitz/build_vorher/`. Vergleich mit
`serie_diff.py` des Gegenprüfers (jedes Byte zählt).

### 8.1 Elliots Kopf ROOM1170 F448–F453

**Messung (nachgestellt, 1170@240, F440–F491):** e998bd32 gegen vorher weicht in F448–F453 ab:
8 / 579 / 926 / 1020 / 351 / 352 Pixel, Hülle x 151,5..163,2 / y 80,5..104,2 (Elliots Gesicht).
F451–F453 liegen ohne Blende (fade0=0), sind also voll sichtbar. state.log ist bitgleich.

`RE15_NECK_TRACE` zeigt die Ursache. Vorher hat Elliot (Slot 1) eine Nacken-Zeile mehr, und zwar
die erste (`kf=(-7,188)`, Akku Pitch 0 → −48). Das ist F445, das Anforderungsbild des
Selbst-Wiedereintritts (SCD fordert 7→0). Seit dem Apply-Vorzug wird F445 noch mit Cut 7
gezeichnet. Elliot steht bei (1261, 10091) außerhalb des Cut-7-Vierecks und wird deshalb nicht
gezeichnet (`tris=0`). Der Port rechnet die Nacken-FSM aber in `re15_skel_compute_pose`, also im
Zeichenpfad. Der Akku lag danach einen Schritt zurück (−48 statt −96, −43 statt −48, 5 statt 0)
und holte bis F454 auf. Die Pause-Flags erlauben den Schritt: F445 `pf=00000007`, erst F446
`pf=FF000007`.

**Original (selbst disassembliert, PSX.EXE und STAGE1.BIN):**

    TAKT      8001ce04  jal FUN_8001a50c          Entity-Schleife der Spiel-Task
    FUN_8001a50c:
              8001a544  lui s3,0x8007
              8001a548  addiu s3,s3,11180         s3 = 0x80072bac (Takt-Tabelle)
              8001a570  lbu v0,8(v1)              Typ-Byte +0x8
              8001a578  sll v0,v0,2
              8001a57c  addu v0,v0,s3
              8001a588  jalr v0                   Typ 0x47 (Elliot, state.log t=47)
    STAGE1    8011e940  lui v0,0x8012
              8011e944  addiu v0,v0,-10540        0x8011d6d4
              8011e948  lui at,0x8007
              8011e94c  sw v0,0x2cc8(at)          @0x80072cc8 = Tabelle[0x47]
    FUN_8011d6d4:
              8011d6e8  lw v0,0(s0)               g_pauseflags 0x800aca40
              8011d6ec  lui v1,0x2000
              8011d6f0  and v0,v0,v1
              8011d6f4  bne v0,zero,0x8011d81c    KI-Pause -> Nacken aus
              8011d708  lbu v0,9(a0)
              8011d710  andi v0,v0,0x20
              8011d714  bne v0,zero,0x8011d81c    +0x9&0x20 -> Nacken aus (Elliot: g=40)
              ...       (Abstand, Zustands-Tabelle @0x801217a0, Anim, Kollision)
              8011d80c  jal FUN_80037358          NACKEN-FSM (@0x800374fc–@0x800377ec)

    ZEICHNEN  8001d108  jal FUN_8001e8c8          je Entity, NACH dem Takt
    FUN_8001e8c8:
              8001e970  lw a1,-14448(a1)          DAT_800ac790 (Regions-Viereck)
              8001e974  jal FUN_80014368          Punkt im Viereck?
              8001e97c  beq v0,zero,0x8001e9c0    aussen ->
              8001e990  jal FUN_8001e9ec          innen: je Part zusammensetzen + zeichnen
              8001e9b4  jal FUN_8001ef54          aussen: je Part NUR zusammensetzen
    FUN_8001ef54:
              8001ef80  jal FUN_80022da0          Matrix-Verkettung, kein Zeichnen

    APPLY     80021c00  jal 0x80014324
              80021c0c  sw v0,-14448(at)          DAT_800ac790 - einziger Schreiber
                                                  (Wort-Scan nach sw mit Offset 0xc790,
                                                  jede Basis: PSX.EXE nur hier, STAGE1–6 keiner)

Die Nacken-FSM hängt also am Takt und liest weder das Viereck noch den angezeigten Cut
(DAT_800b0fe4). Das Viereck wirkt nur im Zeichnen. Ein gecullter NPC wird dort trotzdem
zusammengesetzt. Im Anforderungsbild cullt das Original mit dem ALTEN Viereck, weil erst der
Apply @0x80021c0c es umsetzt. Der Nacken taktet trotzdem. Die Fassung e998bd32 cullte also richtig
und verlor dabei einen Takt, der am Zeichnen hing. Die alte Fassung hatte den Takt nur zufällig,
weil sie im Anforderungsbild schon das NEUE Viereck benutzte, was wiederum nicht dem Original
entspricht.

**Änderung (main.c, NPC-Schleife):** Ein NPC außerhalb des Vierecks wird nicht mehr vor der Pose
verworfen. Die Schleife merkt sich `npc_region_culled`, lässt Schatten und TIM-Bindung aus, rechnet
die Pose (Nacken-FSM, Überblend-Schnappschuss, Kopf-Weltlage) und verwirft den NPC erst danach.
Das entspricht dem Außen-Zweig @0x8001e9b4. Die Typ-0x36-Ausnahme (G5-Front) bleibt unverändert.

**Messung nachher** (`f_1170` gegen vorher `e_1170_v`):

| | e998bd32 | nachher |
|---|---|---|
| abweichende Bilder F440–F491 | 7 (F448–F453, F479) | **1 (F479 = das behobene Mischbild)** |
| RE15_NECK_TRACE gegen vorher | 1 Zeile weniger (F445) | **bitgleich** |
| state.log | bitgleich | bitgleich |

**Breitere Wirkung**, weil jetzt jeder gecullte NPC posiert wird. Gemessen wurde vorher gegen
nachher, jedes Bild:

| Weg | verglichen | abweichend | davon Mischbilder von vorher (sync=0) | state.log |
|---|---|---|---|---|
| New Game → ROOM1170 F440–F1800 | 1361 | 9 | 9 (F479, F982, F1043, F1230, F1325, F1361, F1393, F1548, F1768) | bitgleich |
| ROOM1140 zu Fuß, 5 Zombies, F0–F450 | 451 | 0 | — | bitgleich |
| ROOM1030 zu Fuß, 6 Zombies, F0–F450 | 451 | 5 | 5 (F2, F88, F140, F203, F366) | bitgleich |
| ROOM1100 Zonen, F130–F200 | 71 | 1 | 1 (F160) | bitgleich |
| Tür 1150 → 1130, F0–F12 | 13 | 0 | — | bitgleich |
| Telefon 1150, F140–F330 | 191 | 3 | 3 (F143, F239, F330) | bitgleich |

Jedes abweichende Bild ist ein Mischbild der alten Fassung. Weitere Abweichungen durch das Posieren
gecullter NPCs traten auf keinem dieser Wege auf.

### 8.2 Montage ROOM1240 (Reihenfolge Schalter / Apply)

**Messung:** e998bd32 gegen vorher, New Game F0–F1415: 114 Bilder ab (F48–F161, je 552960 Pixel,
(0,0,0) statt (1,1,1)). Die Ursache ist die Reihenfolge. `pc_cam_present_apply` lief vor
`re15_montage_fx_set_active`, und `re15_montage_fx_on_cut(0)` kehrt bei inaktiver Montage sofort
zurück (re15_montage_fx.c `if (!re15_montage_fx_active()) return;`).

**Änderung:** Der Schalter steht vor dem Apply, mit derselben Bedingung wie in cac33993
(Hintergrund geladen). **Port-Wahl, keine Original-Adresse** (die Montage ist die RE2-Präsentation):
Bis cac33993 lief im Aktivierungsbild der Takt VOR dem ersten `on_cut`. Das war ein Takt ohne
Ebene, der nur die globalen Puls-Zähler `pan_ctr`/`zoom_ctr` weiterschaltet. Genau so läuft er
jetzt im Aktivierungsbild, danach wie gehabt Apply → Takt → Blit. Die drei gemessenen Fassungen
(New Game F0–F1415 gegen vorher):

| Aktivierungsbild | abweichende Bilder |
|---|---|
| Takt nach dem Apply | 1 (F47: die Rampe erreicht 0x60 ein Bild früher) |
| Takt weggelassen | 118 (Pan-Pulse alle 11, Zoom-Pulse alle 4 Bilder um eins verschoben) |
| **Takt vor dem Apply (eingebaut)** | **0** |

Messschiene: `RE15_CUT_SYNC_LOG` hat das neue Feld `mfx=<neu>/<vorige>`. Nachher gilt F0 0/0,
F1 2/0 … F48 96/0, und beim Wechsel F162 2/94 (die vorige Ebene blendet aus).

### 8.3 Riegel

`integration_r30_cut_blitz` fährt jetzt drei Läufe:

* **A** endet bei `RE15_EXIT_AT=330#1150` statt 260 und prüft das Telefon-Bild F330. Verlangt
  sind bg 1150#2, view 2, sync 1, req 6 und 3D im Bild. Bis 331 geht es nicht: nach F330 läuft
  kein Spielbild mehr, weil das Telefon sein Fenster öffnet (gemessen: keine Zeile F331 in 240 s).
* **B** setzt `RE15_NECK_TRACE` und verlangt einen Nacken-Takt in einem Bild ohne 3D. Gemessen:
  41 Takte, davon F445 ohne 3D. Die Zeile der Messschiene beginnt jetzt mit `F<bild>`
  (skeleton_common.c).
* **C (neu)**: New Game bis 200#1240. Das erste Bild muss einblenden (gemessen 160 Bilder), und
  unter Cut 1 muss eine Ausblend-Ebene stehen (gemessen 39 Bilder).

Grün in 70 s. Gegenproben (`nachschliff-cut-blitz_tools/r30_cb_mutiere.py M1|M2|M3`), jede einzeln
gebaut und danach per `git checkout` zurückgesetzt:

| Mutation | Ergebnis |
|---|---|
| Montage-Schalter hinter den Apply | **rot** in C („erstes Bild ein: 0, Ausblenden unter Cut 1: 0“) |
| gecullte NPCs wieder vor der Pose verwerfen | **rot** in B („kein Nacken-Takt in einem Bild ohne 3D“) |
| Apply hinter den Hintergrund-Blit | **rot** in A (F143, F239) |
| exe vorher (4273b3f6) | **rot** in A (F143, F239 und **F330** bg 1150#2 view 6) |

Die Mutation „Apply hinter den Blit“ setzt den Apply vor den SCD-Takt. Deshalb fällt F330 dort
nicht auf. Die echte alte exe zeigt F330 rot.

Suite nachher (`ctest --test-dir re15_port/build --timeout 240`, Stand b8ce0e96): **395/395 grün**, 489 s.

`RE15_MIN_TESTS` in `tools/local_build.sh` ist unverändert. Der Auftrag behält die Zahl dem
Integrator vor. Die Suite hat weiter 395 Tests, weil kein neuer Test dazukam, sondern der
bestehende erweitert wurde.

### 8.4 Bildwiederholung im Anforderungsbild (weiter Port-Wahl)

Das Original zeigt im Anforderungsbild das vorige Bild noch einmal
(@0x80021558 Apply, @0x80021560 `j LAB_800215fc` über DrawOTag und Pufferwechsel). Der Port zeigt
dort das mit dem alten Cut gezeichnete Bild dieses Durchlaufs. Das bleibt **Port-Wahl, keine
Original-Adresse**. Gemessen wurde, wie groß der Unterschied ist (Telefon-Weg nachher,
Anforderungsbild gegen das Bild davor, das das Original noch einmal zeigen würde):

| Anforderungsbild | gegen Vorbild | Hülle |
|---|---|---|
| F142 (Zone 0→1) | 54957 Pixel | x 0,2..85,2 / y 70,5..239,8 (Leons Schritt) |
| F238 (Zone 1→2) | 13457 Pixel | x 53,8..87,2 / y 70,5..146,8 (Leons Schritt) |
| F330 (Telefon, `Cut_chg(6)`) | 0 | — |

Gemischte Bilder entstehen dabei nicht. Der Unterschied ist ein Bild Bewegung der Figur. Den
Nachbau des Originals (vorigen Framebuffer halten, statt zu präsentieren) habe ich nicht versucht:
er berührt `re15_render_end_frame`, die Taktung und den Framedump. Das ist offen.

### 8.5 Weiter offen

* **Spieler, dieselbe Kopplung:** Leon wird nur posiert, wenn er sichtbar ist
  (`main.c player_visible`). Im Original laufen sein Nacken im Takt (@0x80031d78 in FUN_80031c44)
  und das Zusammensetzen auch gecullt (FUN_8001e8c8 @0x8001d09c). Auf keinem gemessenen Weg
  ausgelöst und nicht umgebaut (Leon-Bestand).
* **Gate +0x9&0x20** (@0x8011d708–14): die Nacken-FSM des Ports prüft nur die Pause, nicht dieses
  Entity-Bit. Das galt schon vorher für gezeichnete NPCs. Elliot hat es nicht gesetzt (g=40).
* Getrennte Folge-Spuren (Gegenprüfer): Logik-Versatz Zone 2→3 → `Cut_chg(6)` (Original 19,
  Port 21) sowie die fehlenden Present-Gates @0x80021518–24 (DAT_800b536c) und @0x80021540–50
  (aca38&0x200000).
