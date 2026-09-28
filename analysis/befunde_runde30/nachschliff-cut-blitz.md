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

Vorher gegen nachher: F444–F447, F478, F480–F482 je **0**; nur F479 (139758).

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
