# Runde 38 Spur W "rolltor20a0" — Abnahme 1 (unabhaengig, nach Nachbesserung 1)

Datum 2026-10-17. Baum `.claude/worktrees/r38_rolltor20a0` (Zweig `r38/rolltor20a0`, HEAD 2ff291a9, Basis df08d83a).
Pruefer: Abnahme 1 (glaubt nur, was hier selbst am gebauten Stand gemessen ist). Bericht wird fortlaufend geschrieben.
Werkzeuge/Laeufe dieser Abnahme: Scratchpad `.../scratchpad/abnW1_rolltor/`.

Nutzer-Punkt (AUFTRAG.md Nachtrag 1, woertlich):
> And additionally: in ROOM 20A0 the shutter that you have to open to push the box down is somehow DIRECTLY in the
> camera. I cannot see anything... this is not like this in the original....

Maengel der Abnahme 0: M1 (ROOM5060/5061 Cut 4 deckend dunkles Halbviereck statt halbtransparent), M2 (Zensus einseitig).

## 0. Stand und Bau

* `git log master..HEAD`: 71 Commits (W, gemergte A-Commits, Abnahme 0, 17 Commits Nachbesserung 1 1579b758..2ff291a9).
  master hat seit der Basis df08d83a nur `doc(r38)`-Commits.
* Bau: `bash re15_port/tools/local_build.sh configure` -> `=== LOCAL-BUILD-OK (configure)`, `... build` ->
  `ninja: no work to do.` / `=== LOCAL-BUILD-OK (build)`. `git status` leer; juengste Quelle
  `platform/pc/src/objabe_r38_pc.c` 08:45:03.44, Objekt `objabe_r38_pc.c.obj` 08:45:04.56, `re15_pc.exe` 08:45:04.79 ->
  die exe entspricht HEAD.

(Fortsetzung folgt.)

## 1. Code-Diff der Nachbesserung 1 (gelesen)

`git diff d6a55686..HEAD -- re15_port`: neu `engine/src/objabe_r38.c`, `include/re15_objabe_r38.h`,
`platform/pc/src/objabe_r38_pc.c`; Haken in `platform/pc/main.c` (Include, `re15_objabe_r38_pc_raum_neu()` am Anfang von
pc_load_room_prop_set, `re15_objabe_r38_pc_textur()` nach jedem Raum-Objekt-TIM-Upload, die drei Dreiecks-Aufrufe der
Prop-Schleife ueber `re15_objabe_r38_pc_dreieck(attr, uv->page, ...)`, Flach-Bedingung `(attr & 0x12) == 0x10` ->
`re15_objabe_r38_flach(attr, typ)`), `platform/pc/src/render_pc.c` (Include; `set_tri_blend` nimmt 0..7; Flush holt fuer
3..7 die STP-Teil-Textur, NULL = Stapel auslassen; `re15_render_textured_tri_lit` nimmt das Eckpunkt-Alpha aus
`s_tri_alpha` statt fest 0xFF; `re15_render_pc_renderer()` nicht mehr nur unter `__ANDROID__`), `objpoly_r38_pc.c`
(Halbviereck ueber denselben ABE-Weg); Riegel `tests/unit/test_r38_rolltor20a0_abe.c`,
`tests/integration/test_r38_rolltor20a0_abe.cmake`, Registrierung in `tests/unit/probes/r38_rolltor20a0.cmake`.

Selbst geprueft:
* Teil-Texturen (`re15_objabe_r38_teilen`) haben dasselbe Layout wie `re15_render_pc_upload_tim_slot` (Breite
  `tim->width`, Hoehe `tim->height x CLUT-Zeilen`, gleiche Texel-Dekodierung `re15_tim_texel_argb`, gleiches
  `SDL_CreateTexture`/`SDL_BLENDMODE_BLEND`) -> die UV-Rechnung des Flush (inv_w/inv_h des Slots) passt.
* Ohne ABE: unveraendert EIN `re15_render_textured_tri_lit` mit der normalen Slot-Textur (Mischmodus 0, Alpha 255).
  ABE-Objekt ohne STP-1-Texel: Durchgang 1 = normale Textur deckend, Durchgang 2 = NULL (ausgelassen) = GPU-Regel.
* Flush-Schleife: das innere `int bl` (Zeile 1189) verdeckt das aeussere `uint8_t bl`; `cur_blend = bl` steht ausserhalb
  des inneren Blocks und nimmt das aeussere (Modus des naechsten Dreiecks) — korrekt.
* Eckpunkt-Alpha: alle Setzer im Baum UND in `r38/integration` 127100ef (`git grep set_tri_alpha`) — main.c ESP-Effekte,
  glas1120_pc.c, re2fx_pc.c, objabe_r38_pc.c — setzen nach Gebrauch 255 zurueck (auch nach dem `break` der ESP-Schleife);
  door_scene_pc.c / hebetisch_cursor_pc.c setzen nur den Mischmodus. Kein Weg hinterlaesst Alpha != 255 fuer Figuren.
* Port-Modelle: `attr` = 0 (Feuerloescher) bzw. 0x0010 (Kartenitems), kein Bit 0x2; ihre Slots bekommen keine Teil-
  Textur (re15_objabe_r38_pc_raum_neu leert alle 64 Slots am Raumanfang) -> unveraendert.
* Typ-3-Teil der Flach-Regel: alle 54 Typ-3-Saetze (Zensus, selbst nachgefahren) tragen 0x001a oder 0x0012, also das
  Flach-Bit 0x10 — die Regel `Typ 3 -> flach` aendert ueber das Flach-Bit hinaus nichts. Geaendert (Licht -> Flach
  0x808080) werden genau die 69 ABE-Saetze mit Bit 0x10 (0x0012 x37, 0x001a x32).

## 2. Eigene Riegel der Spur (selbst gefahren, exe = HEAD)

`ctest --test-dir re15_port/build -R "r38_rolltor20a0" --timeout 1500 --output-on-failure -V` ->
**`100% tests passed, 0 tests failed out of 5`**, `Total Test time (real) = 475.29 sec` (Log `abnW1_rolltor/ctest_spur.log`):
* unit_r38_rolltor20a0 (R1..R7 wie Abnahme 0, u. a. `R1 ... 48 Vierecke wie im Original`, `R2/R3 ... neu 0 Pixel, alt
  67728`), unit_r38_rolltor20a0_windung (`O2: ... 0 raeumliche nicht in Original-Windung`).
* integration_r38_rolltor20a0 271.5 s: a_leon / b_elza / d_leon_karte je 9 Bilder Cut 11 Anteil **0.9420..0.9503**,
  `OBJPOLY Cut 11 obj 1: 450 Bilder, Abweichungen 0`; c_kiste `F1278 room=20A0 p=0 LANDUNG y=-3600 band=2`.
* unit_r38_rolltor20a0_abe: A1 9 Regel-Faelle; A2 10 Original-Saetze (`ROOM5060 obj 4 @0x02D72 Attribut 0x0012: ABE 1,
  ABR 0 bei 2/2 Polygonen = Original` ... `ROOM4090 obj 2 @0x00B36 Attribut 0x000e: ABE 1, ABR 3 bei 152/152`); A3
  STP-Teilung `ROOM11B0 obj 0 ... STP 1 1513, STP 0 31255, falsch 0`; A4 `max. Abweichung ABR0 1, ABR1 0, ABR2 0, ABR3 1`.
* integration_r38_rolltor20a0_abe 203.7 s, je 31 Bilder: 5060c4 (Leon) und 5061c4_elza `(32.2,15.4,26.5)` gegen Original
  `(31.6,15.3,26.1)` Abstand 0.6; 5060c3 `(124.8,47.7,46.9)` gegen `(125.4,46.2,44.0)` 2.9; 20a0c2 `(0.4,0.4,0.3)` 0.0;
  4090c0 `(140.6,138.9,147.2)` gegen `(140.5,139.4,147.9)` 0.7.
* Die Original-Referenzwerte des Riegels habe ich mit einem eigenen Skript (`abnW1_rolltor/tools/mittel.py`, PIL) an den
  PNGs nachgerechnet: `orig_abe_5060_cut4_buf0.png` Dreieck 1749 px (31.6,15.3,26.1); `orig_abe_20a0_cut2_buf0.png`
  7840 px (0.4,0.4,0.3); `orig_abe_4090_cut0_buf0.png` 21280 px (140.5,139.4,147.9); `orig_abe_5060_cut3_buf0.png`
  2000 px (125.4,46.2,44.0) — identisch mit den Konstanten in `test_r38_rolltor20a0_abe.c`.

## 3. RE-Gate (selbst disassembliert, `info/Re1.5/PSX.EXE` md5 b55fdaa5..., `re15_disasm.py dis`)

| Zitat im Diff | selbst gelesen | Urteil |
|---|---|---|
| ABE = Attribut & 2: FUN_8002c18c @0x8002c2ac/@0x8002c2b4/@0x8002c2b8 | `lw v0,12(s1)` (Pool+0x0C) / `andi v0,v0,0x2` / `sltu s2,zero,v0` | stimmt |
| Parameter 4 an alle Zeichner @0x8002c304/@0x8002c32c/@0x8002c364/@0x8002c38c | je `addu a3,s2,zero` vor `jal 0x80022f0c` @0x8002c320, `jal 0x80023098` @0x8002c350, `jal 0x800254a0` @0x8002c380, `jal 0x800256b0` @0x8002c3b0; a2 = s1+112 (`addiu s0,s1,112` @0x8002c2fc im Verzoegerungsplatz) | stimmt |
| Typ 3 @0x8002c2bc/@0x8002c2c0/@0x8002c2c8/@0x8002c2cc | `lbu v1,8(s1)` @0x8002c2b0 (Typ = Pool+8), `ori v0,zero,0x3`, `bne v1,v0,0x8002c2d8`, `jal 0x8003e64c`, Verzoegerung `addu a1,s2,zero` | stimmt |
| FUN_8003e64c nur Flach-Zeichner, Farbe obj+0x70, ABE | @0x8003e664 `addiu s1,s3,112`, @0x8003e668 `addu a2,s1,zero`, @0x8003e66c `addu a3,s2,zero`, @0x8003e6ac `jal 0x80022f0c`; @0x8003e6b4/@0x8003e6b8 dasselbe, @0x8003e6dc `jal 0x80023098` | stimmt |
| Flach = Attribut & 0x10 @0x8002c2f4 | `andi v0,v0,0x10` / @0x8002c2f8 `beq v0,zero,0x8002c360` (-> Licht-Zeichner) | stimmt |
| Code = Grundcode \| ABE<<1 | FUN_80022f0c @0x80022f18 `sll a3,a3,1` / @0x80022f1c `ori a3,a3,0x34` / @0x80022f38 `sb a3,3(a2)`; FUN_80023098 @0x800230ac/@0x800230b0 `ori a3,a3,0x3c`/@0x800230d0 `sb a3,3(s0)`; FUN_800254a0 @0x800254ac/@0x800254b0 (0x34)/@0x800254d4; FUN_800256b0 @0x800256bc/@0x800256c0 (0x3c)/@0x800256ec; Wort 1 des Primitivs @0x80023024 `lw v0,0(a2)` / @0x8002302c `sw v0,4(a1)` | stimmt |
| ABR = Texpage \| (Attribut & 0xC) << 3 | FUN_80025940 @0x800259bc `lw v0,0(a0)` / @0x800259c4 `andi v0,v0,0xc` / @0x800259cc `sll t7,v0,19` / @0x80025a08 `lw v0,-4(t1)` / @0x80025a14 `or v0,t7,v0` / @0x80025a18 `sw v0,-8(a3)`; FUN_80025a98 @0x80025b1c/@0x80025b24/@0x80025b2c/@0x80025b68/@0x80025b70/@0x80025b74 (`sw v0,-24(a3)`); a0 = Pool+0x0C aus FUN_8002b898 @0x8002b920 `addiu s0,s0,12` / `jal 0x80025940` @0x8002b92c | stimmt |
| Flach-Farbe 0x808080 (FUN_8002b898) | @0x8002b8fc `lui v1,0x80` / @0x8002b900 `ori v1,v1,0x8080` / @0x8002b91c `sw v1,112(s0)`; Ecken 1/2 des GT3 beim Bind `sw t5,-16(a3)` / `sw t5,-4(a3)` mit t5 = 0x808080 (@0x800259e0/e4) | stimmt |
| psx-spx GPU (Mischformeln, STP je CLUT-Eintrag, 0000h durchsichtig, Klemmung) | `docs/graphicsprocessingunitgpu.md:381`, `:1129-1132`, `:1158-1161`, `:1416-1436` woertlich | stimmt |

Rate-Woerter in den hinzugefuegten Code-Zeilen der Nachbesserung (`git diff d6a55686..HEAD -- re15_port | grep "^+" |
grep -i -E "deferred|tunable|interim|for now|faithful|plausib|TODO|approx|PORT-WAHL|NUTZER-VORGABE|ungefaehr|geschaetzt|vermut"`):
**keine**; kein neuer `getenv`. SDL-Alpha 128/64 sind die SDL-Abbildung der psx-spx-Faktoren 0,5/0,25 (derselbe Weg wie
main.c ESP-Effekte / glas1120_pc.c / re2fx_pc.c), Riegel A4 belegt <= 1 Stufe. Neue Konstanten 0x0002, 0x000C, 0x0010,
Typ 3 tragen je @0x....

Zensus (M2) selbst nachgefahren: `python zensus_beidseitig.py <shared_assets/PSX>` und `python zensus_abe.py ...` ->
Ausgabe **byte-gleich** mit den eingecheckten `zensus_beidseitig.txt` / `zensus_abe.txt` (diff leer). Inhalt: MEHR 3
(ROOM1170 obj 3 Cut 0; ROOM5060/5061 obj 4 Cut 4), WENIGER 22 (18 aus §8 + ROOM2050 obj 4 Cut 11/12/13 und ROOM20A0
obj 6 Cut 3, alle Typ 3), Eltern 4 Saetze gerechnet, pc[5]=0x80 (ROOM4030/4031) gemeldet — das sind genau die Faelle,
die meine Abnahme-0-Skripte (`zensus_mehr.py`, `zensus_typ3.py`) gefunden hatten.
