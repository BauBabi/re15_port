# Spur P masken36 — Runde 37 (2026-10-12)

Orchestrator-Auftrag P (AUFTRAG.md, Regression 53741fe5): die ergaenzten Vordergrund-Masken der Stages 3..6
(30 MSK-Dateien ROOM3040..ROOM6031, erzeugt b8db7b2b 2026-09-03) werden seit 53741fe5 (2026-09-08) fuer NIEMANDEN
gezeichnet. Ziel: R15M-Cut ohne eigenen Atlas wieder mit dem BSS-Atlas zeichnen (wie am 2026-09-03 gebaut),
R15M-Cut mit eigenem Atlas unveraendert, Elza-Zwilling (Spur A) unberuehrt.

Baum: .claude/worktrees/r37_masken36 (Zweig r37/masken36, Basis bddd6b31 = Stand Spur A 06ac2b56 + AUFTRAG/VERTRAG).

## Stand

- [x] Schritt 0: Pflichtlektuere (AUFTRAG, VERTRAG, A_pri1100, A_abnahme_1, b8db7b2b/e100d610/02c62294, 53741fe5)
- [x] Schritt 1: Messung vorher (Zensus aller MSK, Bisect)
- [x] Schritt 2: Umsetzung
- [x] Schritt 3: Messung nachher (Blit-Pruefung, Figur-Messung an der exe, Leon und Elza)
- [x] Tests (probes/r37_masken36.cmake), gezielte Laeufe gruen (§5)
- Fortsetzung 2026-10-13 (nach Sitzungs-/Wochenlimit): be2dbd26 uebernommen (ROOM1221 kein eigener Fall, §1.1),
  Ausschluss-Tabelle neu erzeugt, Riegel-Defekt behoben (§5), Elza + weitere Raeume Stage 5/6 an der exe (§4.2),
  FREI-Szene als exe-Riegel (§4.3).

## 1. Messung vorher

### 1.1 Zensus aller MASKS/*.MSK (statisch, aus den Dateien; Skript Scratchpad `p37/py/zensus.py`)
Spiegelt die Leser des Ports: R15M-Container (pri_common.c re15_pri_msk_section_offset), Sektion
(re15_pri_parse_section: Gruppenanker + Masken-Byte, draw_count = deklariert & 0xFF), Original-Sektion der RDT
(RDT+0x24 Kamerasatz + Cut*0x20 + 0x1C, NULL = keine Masken), eigener Atlas `MASKS/ROOM%04X_PRI%02d.TIM`, BSS-Atlas wie
bg_pc.c re15_pri_cache_from_chunk (Block BSS/ROOM%04X/BG%02d.BSS bzw. Scheibe Cut*0x10000 von STAGE%u/ROOM%03X.BSS, Laenge
L aus der Stage-Tabelle, chunk[L-4] != 0, SLD-Block chunk[L-8]) mit Rueckfall BSS/ROOM%04X/PRI%02d.TIM.

54 MSK-Dateien, **323 Sektionen (Datei/Cut)**:

| Klasse | Sektionen | Dateien/Cuts | Lader heute (seit 53741fe5) | Lader bis 53741fe5 |
|---|---|---|---|---|
| eigener Atlas, Original NULL, kein BSS-Atlas | 94 | Stage 1/2 (Freistellungen des Nutzers): 1000,1010,1050,1060,10A0,10C0,10D0,10E0,10F0,1100,1110,1120,1130,1140,1160,1180,11B0,11D0,11F0,1230,2090,2091 — jeder Cut der Datei | eigener Atlas | eigener Atlas (Weg 3) |
| eigener Atlas, Original NULL, MIT BSS-Atlas | 5 | ROOM1220 Cut 0/2/4/6/8 | eigener Atlas | ⛔ BSS-Atlas (der Fall, den 53741fe5 richtig gestellt hat) |
| **kein eigener Atlas**, Original NULL, BSS-Atlas da | **224** | **Stages 3..6: alle 30 Dateien, 216 Sektionen**; dazu **ROOM1221 Cut 0..6, 8** (Elza, 8 Sektionen) | **tot (fg_atlas=0)** | BSS-Atlas |
| kein eigener Atlas, kein BSS-Atlas | 0 | — | — | — |
| R15M-Sektion, aber Original-Sektion NICHT NULL | 0 | — (die R15M-Sektion waere ohnehin nie gelesen) | — | — |

Je Datei (Cuts mit Sektion / Masken gezeichnet):
3040 & 3041 Cut 0..13 (566), 3050 & 3051 Cut 0..9,11,12,13,15 (449), 30A0 & 30A1 Cut 0..5 (178), 30E0 & 30E1 Cut 6..9 (90),
4040 & 4041 Cut 4..7,9..12 (137), 4080 & 4081 Cut 1..3,5..8 (105), 40A0 & 40A1 Cut 7 (19), 5020 & 5021 Cut 0,2..12 (422),
5050 & 5051 Cut 0..3 (136), 5090 & 5091 Cut 11..14 (209), 50B0 & 50B1 Cut 1..3 (132), 5100 & 5101 Cut 2..7 (160),
6000 & 6001 Cut 1..9 (262), 6010 & 6011 Cut 0..3,5..7,10..13 (301), 6030 & 6031 Cut 7,9..12 (73); ROOM1221 Cut 0..6,8 (131).
Die Elza-Dateien der Stages 3..6 sind bytegleich zu den Leon-Dateien (15 Paare, cmp), ROOM1221 != ROOM1220 (1816 / 4272 B).
Die tote Menge ist genau die Menge "R15M-Sektion ohne eigenen Atlas": jede dieser 224 Sektionen hat einen BSS-Atlas
(SLD im Block, `chunk[L-4] != 0`). Stage 1/2: ausser ROOM1221 hat jede Sektion einen eigenen Atlas (99/99).

**Nachtrag 2026-10-13 - ROOM1221 ist kein eigener Fall:** die Integration hat in be2dbd26 ROOM1221.MSK und ROOM2091.MSK +
_PRI00..07.TIM entfernt (veraltete Elza-Kopien; Elza nutzt dort ueber den Zwilling der Spur A die Leon-Masken mit eigenem
Atlas). Der Spurbaum traegt diesen Stand jetzt auch (cherry-pick -x, eab5d03c; Inhalt identisch, die Zusammenfuehrung mit
r37/integration ist konfliktfrei bis auf AUFTRAG/VERTRAG, `git merge-tree`). Zensus danach (`test_r37_masken36 zensus`):
**52 Dateien, 307 Sektionen: eigener Atlas 91, BSS-Atlas 216, tot 0, Blit passt 307, Original nicht NULL 0; alte Regel
53741fe5: 216 tot = genau die 216 Sektionen der Stages 3..6** (30 Dateien). Die tote Menge ist damit ausschliesslich
Stage 3..6.

### 1.2 Herkunft (git)
- b8db7b2b (2026-09-03 10:30) "234 Cuts Maskengeometrie nachgezeichnet": `analysis/esp_masken_2026-09-03/gen_masken.py` findet
  die Atlas-Bloecke per NCC im Hintergrund, Gruppenanker = Verschiebung, **Masken-Byte = src im BSS-Atlas** (`atlas_rgb(stage,
  rid, cut)` = SLD-TIM des Cuts aus dem BSS-Block), Container R15M; Pruefung `pruef_gen.py` blittet die Masken durch DIESEN
  Atlas auf den Hintergrund (0 Formatfehler, Farbabweichung Mittel 12.67/765). e100d610 und 02c62294 (selber Tag) bauen die
  Dateien neu (Tiefe kalibriert, Abdeckung 44 -> 74 %); seitdem sind die Stage-3..6-Dateien und ROOM1221.MSK unveraendert
  (`git log -- MASKS/ROOM3040.MSK` = b8db7b2b, e100d610, 02c62294).
- 53741fe5 (2026-09-08) "diag(pri)": `re15_pri_load_cut_atlas_ex(cut, nachgezeichnet)`: R15M-Merker -> NUR
  `MASKS/ROOM%04X_PRI%02d.TIM`, fehlt die Datei -> `set_pri_atlas(NULL)`, `return 0` (has_fg = 0, main.c zeichnet nur bei
  `has_fg && pri_n > 0`). Begruendung im Commit: "nachgezeichnete Rechtecke tragen srcX/srcY fuer IHREN Atlas" — richtig fuer
  die Freistellungen (Maskeneditor tools/maske packt einen eigenen Atlas), falsch fuer die Generator-Masken des 2026-09-03.
- Danach ist die Funktion nur noch in 4a07f568 (Spur A, Zwilling-Id im Pfad) geaendert worden
  (`git log -L /re15_pri_load_cut_atlas_ex/ 53741fe5..HEAD`) — v0.8.21..v0.8.24 tragen die Regel unveraendert.


### 1.3 Bisect (eigene Bauten, Scratchpad `p37/bisect/<commit>` per `git archive`, `RE15_TESTS=OFF local_build.sh`)
Lauf je exe: `RE15_DEBUG_JUMP=3040@250` (Sprung-Spawn (-25600,0,-1350), Cut 0), Logzeile `[pri] cut=0`:

| Stand | ROOM3040 Cut 0 |
|---|---|
| b721702f (= 53741fe5^, 2026-09-08) | `masks=19 fg_atlas=1` (gezeichnet) |
| **53741fe5** (2026-09-08) | `masks=19 fg_atlas=0` (tot) — verursachender Commit |
| bddd6b31 (Basis P = v0.8.24 + Spur A, exe `re15_pc_p37vorher.exe`) | `masks=19 fg_atlas=0` |
| P (exe `re15_pc_p37final.exe`) | `masks=12 fg_atlas=1` (19 - 7 ausgeschlossene, §3.3) |

v0.8.21..v0.8.24 tragen dieselbe Regel (Funktion seit 53741fe5 nur in 4a07f568 geaendert, §1.2). Der Fix erklaert den
Befund: die Regel "R15M nur mit eigenem Atlas" ist genau die Bedingung, an der alle 224 Sektionen scheitern (§1.1).

### 1.4 An der exe vorher (vorher-exe = Basis bddd6b31)
Alle 18 Szenen aus §4 und die Spawns aller 15 Leon-Raeume: `fg_atlas=0` fuer jeden Cut mit R15M-Sektion; Framedump der
vorher-exe **bitgleich** zum selben Lauf mit `RE15_NO_PRI=1` (0 Punkte > 24 im ganzen Bild, 18 von 18 Szenen) — vor P
zeichnete der Port in diesen Cuts keine einzige Maske; Leon stand in allen HINTER-Szenen sichtbar VOR den Vordergrundobjekten.

## 2. RE-Belege (PSX.EXE Auslieferungsstand, sha256 e6b340fd12ec43be.., `@0x80026e4c = 08 00 e0 03`; `re15_disasm.py dis/bytes`,
Namen der Bibliotheksaufrufe aus ghidra1_V2.txt (PsyQ-Signaturen))

**Welchen Atlas tastet eine Maske im Original ab? Immer den Vordergrund-Atlas des Cuts aus dem BSS-Block.**
1. Der BSS-Lader entpackt den SLD-Block des Cuts und laedt das TIM an eine FESTE VRAM-Stelle:
   `80021db8 jal 0x800c47e8` (SLD-Entpacker, Ziel 0x801a5800), `80021de8 ori v0,zero,0x15` (`15 00 02 34`) +
   `80021df0 sh v0,0(s0)` (`00 00 02 a6`, s0 = 0x800aca4c) + `80021dec jal 0x8004ee78` (`9e 3b 01 0c`, a0 = 0x801a5800).
   FUN_8004ee78: `8004ee84 jal OpenTIM` / `8004ee8c jal ReadTIM`, dann wird die Bildlage des TIM UEBERSCHRIEBEN:
   `8004eea8 sll a0,v1,6` (`80 21 03 00`, v1 = 0x15 -> 1344), `8004eeb8 addiu a0,a0,-1024` (`00 fc 84 24`, >= 16 -> x = 320),
   `8004eebc sh a0,0(v0)` (`00 00 44 a4`, rect.x), `8004eed0 sll v0,v0,8` (`00 12 02 00`) / `8004eed4 sh v0,2(v1)`
   (`02 00 62 a4`, rect.y = 256), `8004eee0 jal LoadImage` (`22 a3 01 0c`). -> Atlas des Cuts liegt bei VRAM (320,256).
   (Das TIM selbst traegt Bild @(0,0), CLUT @(0,480) — gemessen an ROOM3040 Cut 0/2, 5020 Cut 0, 6010 Cut 0, 1220 Cut 0, 1221 Cut 1:
   flag 0x9, CLUT 256x1 @(0,480) = CLUT-Wort 0x7800, Bild 128x256 Halbworte = 256x256 8-bpp.)
2. Jede Maske ist ein SPRT, das genau diese Seite abtastet (FUN_800392d4 baut, FUN_80039590 haengt ein):
   `80039408 lbu s7,0(a2)` (`00 00 d7 90`, Masken-Byte 0 = srcX), `80039418 lbu fp,-9(s2)` (`f7 ff 5e 92`, s2 = a2+10 ->
   Byte 1 = srcY), `800394dc sb s7,-2(s0)` (`fe ff 17 a2`, u0) / `800394e0 sb fp,-1(s0)` (`ff ff 1e a2`, v0),
   `80039498 ori v0,zero,0x7800` (`00 78 02 34`) + `800394fc sh v0,0(s1)` / `80039500 sh v0,0(s0)` (CLUT 0x7800 = VRAM (0,480)),
   `80039630 ori a3,zero,0x95` (`95 00 07 34`) -> `80039634 jal SetDrawMode` (`16 a6 01 0c`, tpage 0x95 = 8 bpp, x 5*64 = 320,
   y 256 — dieselbe Stelle wie 1.), `80039640 jal MargePrim`, `80039660 jal AddPrim`.
   -> u/v einer Maske sind im Original IMMER Koordinaten im BSS-Atlas des gezeigten Cuts; es gibt keinen zweiten Atlas.
3. Folgerung fuer die Port-Masken (R15M, NUTZER-VORGABE fuer NULL-Sektionen):
   - Generator 2026-09-03 (gen_masken.py, alle 216 Stage-3..6-Sektionen + ROOM1221): Masken-Byte = src im BSS-Atlas — exakt
     der Original-Mechanismus (Atlas @0x80021dec/@0x8004eee0, Abtastung @0x800394dc/@0x80039630). Diese Masken gehoeren
     zum BSS-Atlas.
   - Freistellungen (Maskeneditor tools/maske, eigener Atlas `MASKS/ROOM%04X_PRI%02d.TIM`): PORT-ERWEITERUNG, weil srcX/srcY
     u8 sind (@0x80039408/@0x80039418) und der Hintergrund (320 breit) damit nicht adressierbar ist; ihre Rechtecke tragen
     src im EIGENEN Atlas. Nur fuer sie gilt die Regel aus 53741fe5.
   - Entscheidungsmerkmal je Sektion: liegt der eigene Atlas des Cuts vor -> eigener Atlas; sonst -> BSS-Atlas (= Original).
     Gemessen statt angenommen in 2.1.

### 2.1 Blit-Pruefung (wie pruef_gen.py: Masken durch den Atlas auf den dekodierten Hintergrund, Abweichung je gemaltem Punkt
als Summe |dR|+|dG|+|dB| (0..765); Hintergrund = probe_bg_dump (Engine-MDEC-Dekoder, 1119 Bilder); Pixelclip wie SDL_RenderCopy)

| Menge | Sektionen | gemalte Punkte | Abweichung Mittel je Sektion (Mittelwert / Median / Max) | p95 max | Anteil > 48 max |
|---|---|---|---|---|---|
| ohne eigenen Atlas -> BSS-Atlas (alle) | 224 | 2 280 589 | 12.63 / 12.50 / 23.05 | 52 | 6.51 % |
| davon Stages 3..6 | 216 | 2 248 596 | 12.87 / 12.64 / 23.05 (ROOM5090/5091 Cut 12) | 52 | 6.51 % |
| davon ROOM1221 | 8 | 31 993 | 6.26 / 6.31 / 6.92 | 14 | 0 % |
| mit eigenem Atlas -> eigener Atlas | 99 | 1 236 765 | 6.27 / 6.32 / 8.61 | 19 | 0.54 % |
| GEGENPROBE ROOM1220 Cut 0/2/4/6/8 -> eigener Atlas | 5 | 24 259 | 5.06 / 4.92 / 5.84 | 10 | 0 % |
| GEGENPROBE ROOM1220 Cut 0/2/4/6/8 -> BSS-Atlas (falsch) | 5 | 14 182 | 77.71 / 82.75 / 85.32 | 265 | 65.34 % |

- Die 224 toten Sektionen passen zum BSS-Atlas (12.63 = der Wert des Generators vom 2026-09-03, "Mittel 12.67" ueber 234 Cuts;
  8-bpp-Palettenrundung). Keine Maske greift ueber den Atlasrand (0), keine Sektion ohne gemalte Punkte.
- Die Probe trennt: dieselbe Rechnung mit dem FALSCHEN Atlas (ROOM1220, Freistellungen mit eigenem Atlas, durch den BSS-Atlas)
  ergibt 77.7 im Mittel und 65 % Punkte > 48 — genau der Fehler, gegen den 53741fe5 abgesichert hat. Fuer die Freistellungen
  bleibt der eigene Atlas richtig.


## 3. Umsetzung

### 3.1 Lader-Regel (bg_pc.c, include/re15_masken36_r37.h)
`re15_pri_atlas_quelle(nachgezeichnet, eigener_atlas_da)`: R15M MIT eigenem Atlas -> eigener Atlas (unveraendert, schuetzt
u.a. ROOM1220 Cut 0/2/4/6/8, deren BSS-Atlas laut 2.1 falsch waere); R15M OHNE eigenen Atlas -> BSS-Atlas des Cuts (Wege 1/2
in re15_pri_load_cut_atlas_ex: Laufzeit-Auszug aus dem Block, Rueckfall BSS/ROOM%04X/PRI%02d.TIM) = Original-Mechanismus
(§2). Weg 3 (MASKS-Atlas als letzter Rueckfall) nur noch fuer Original-Sektionen (fuer R15M schon im Zweig gesucht). Neue
Logzeile `[pri] ROOMxxxx Cut n: Port-Masken ohne eigenen Atlas -> BSS-Atlas des Cuts (Runde 37 Spur P)`. Elza-Zwilling
(Spur A): der eigene Atlas wird weiter ueber `re15_pri_masken_raum` gesucht, der BSS-Auszug ist ohnehin je Raumpaar gleich
(Original @0x80021c90 Raum ohne Variantenziffer). Erste Messung: ROOM3040 Cut 0 `masks=19 fg_atlas=1` (vorher 0).

### 3.2 Befund beim Nachmessen: die Generator-Daten verdecken Figuren auch VOR Objekten
An der exe gemessen (Laeufe `s3040h`, `h3050c5`, Bilder `p37/img/h3050c5.png`, `s3040h.png`): ROOM3050 Cut 5, Leon
(-13300,-16368) steht frei im Gang VOR der Maschine (Fuesse deutlich unter deren Unterkante) — mit BSS-Atlas wird er bis auf
den Kopf geloescht (597 von 1318 Figurpunkten uebermalt, Schatten bleibt stehen); ROOM3040 Cut 1 (-22500,-11400): Beine
hinter der Kiste weg, Fuesse darunter sichtbar. Ursachen in den Daten (Masken-Bild je Cut, `p37/img/montage_aus.png`):
BODEN in der Auswahl (ROOM30E0 Cut 7, ROOM3050 Cut 6, ROOM5050 Cut 2 — Bodenflaechen als "Vordergrund"),
HINTERGRUND in der Auswahl (3050 Cut 5) und das Tiefenband des Generators (gen_masken.py DEPTH_SAFETY 0.90: die Maske liegt
10 % naeher als der Bodenkontakt -> wer knapp vor einem Objekt steht, verschwindet; z.B. Spinde 30A0 Cut 0). Genau diese
Fehlerklasse hat der Nutzer am 2026-09-03 gemeldet ("Leon verschwindet fast vollstaendig", Commit 6c29d834:
"Eine fehlende Maske laesst alles wie bisher, eine falsche uebermalt den Spieler") — fuer STAGE1 wurde damals auf
Freistellungen umgebaut, die Stages 3..6 sind seitdem unveraendert. Ein reiner Rueckbau "wie am 2026-09-03" erfuellt
deshalb das Ziel "Figuren VOR einem Objekt werden nicht verdeckt" NICHT.

### 3.3 Ausschluss der Fehlverdecker (engine/src/masken36_r37.c, gen/masken36_ausschluss_r37.inc)
Ohne MASKS/* anzufassen (Vertrag): der Lader laesst je Generator-Sektion genau die Rechtecke weg, die an einem erreichbaren
Stehplatz einen Figurpunkt verdecken, ohne dass zwischen Kamera und Figurpunkt eine solide Kollisionszelle steht.
- Verdeckt: Original-Urteil `re15_pri_mask_occludes(depth, vz)` = depth < (1023*vz)>>16 (@0x8002565c otz>>4, ZSF3=341
  @0x80066c70, Masken-OT = depth @0x80039650..60).
- Objekt davor: die Grundriss-Strecke Kamera -> Figurpunkt kreuzt eine solide Zelle des Bands (u0 & 1, floor>>4 == Band;
  FUN_8003b0a4 / re15_collision.c Kopf: die SCA-Zellen sind die Waende und Gegenstaende). Zellen, in denen Kamera oder
  Figurpunkt stehen, zaehlen nicht.
- Erreichbar: Gitter 100; frei = ausserhalb jeder soliden Zelle des Bands um den Spielerradius PR 450 aufgeblasen
  (DAT_80073e94[6], re15_collision.c `#define PR 450`); Flutfuellung je Band ab allen AOT-Mitten des Raums mit eigenem Band
  (Engine-SCD main00+sub00, 4+300 Takte wie probe_1190_props; ohne GENERIC/CAM_SWITCH/AUTO_EVENT) und den Ankuenften aller
  Tueren aller Raeume (Band aus Ziel-y); nur innerhalb der Kamerazonen des Bands (RVD zone.floor == Band oder 0xFF); je Cut
  nur im Regionsviereck (Akteur-Cull FUN_8002c18c, re15_rdt_get_region_quad). Band-Boden y = -Band*0x708.
- Figur = Leon, an der exe gemessen (RE15_TRILOG + Rueckprojektion, §4): Hoehe 3000, halbe Breite 350; 9 x 9 Punkte.
- Iteration: vorderstes Rechteck an einem fehlverdeckten Punkt raus, bis keine Fehlverdeckung bleibt.
- Ergebnis (`test_r37_masken36 erzeuge`, Saaten 1979) nach be2dbd26: **216 Sektionen, 6478 Masken, 944 ausgeschlossen,
  85 Tabellenzeilen** (Stage 3: 39, Stage 4: 0, Stage 5: 34, Stage 6: 12); 32 Sektionen ohne Stehplatz im Modell (dort
  bleibt alles wie gebaut, OFFEN). Gegen die Tabelle vom 2026-10-12 fallen genau die fuenf 0x1221-Zeilen weg, alle 85
  anderen Zeilen sind bitgleich (diff). (Vorher mit ROOM1221: 224 Sektionen, 6609 Masken, 993 ausgeschlossen.)
- Haken: `platform/pc/main.c` R15M-Zweig (2 Zeilen: `re15_masken36_filter(re15_pri_masken_raum(raum), cut, &pri)`),
  `platform/psx/main.c` (Gleichlauf, ungebaut). Die Freistellungs-Sektionen (eigener Atlas) stehen nicht in der Tabelle.


## 4. Messung nachher

### 4.1 Szenen (Szenenfinder `test_r37_masken36 suche`: erreichbare Stehplaetze nach §3.3, HINTER = Fuss und >= 60 % der
Figur unter Masken, DAVOR = Figur ueberlappt die Maskenflaeche, Fuss frei, nichts verdeckt; groesste Figur im Bild)
Je Szene vier Laeufe (Leon, `RE15_DEBUG_JUMP=<raum>@250 RE15_PLAYER_POS=<x>,<z>,0 RE15_FORCE_CUT=<cut>`, Framedump Bild 120,
`RE15_WINDOW_SCALE=1`): A = final (P), N = nur Lader-Regel ohne Ausschluss (Zwischenstand), B = final mit `RE15_NO_PRI=1`,
V = vorher-exe (bddd6b31). Dazu C = ohne Figur (Leon ausserhalb des Regionsvierecks, `RE15_NO_PRI=1`).
Figurpunkte S = |B - C| > 24 (Kanalmaximum) im Figurfenster (Projektion der Figur, Hoehe 3000 / +-350, +4), uebermalt =
S und |X - B| > 24. Lage = nach der Kollision gemessen (`RE15_STATE_LOG`).

| Szene | Raum/Cut, Lage | Figurpunkte | final (P) | ohne Ausschluss | vorher |
|---|---|---|---|---|---|
| HINTER h3050c2 | 3050 C2 (-22000,-20768) | 1234 | **1123 (91 %)** | 1123 | 0 |
| HINTER h30A0c3 | 30A0 C3 (-19800,19763) | 2824* | **1933** | 1933 | 0 |
| HINTER h4040c4 | 4040 C4 (-23200,-922) | 208 | **146 (70 %)** | 146 | 0 |
| HINTER h4080c1 | 4080 C1 (21875,-5210) | 186* | **182** | 182 | 0 |
| HINTER h5020c0 | 5020 C0 (-6933,-22525) | 794* | **741** | 741 | 0 |
| HINTER h5020c9 | 5020 C9 (-13133,-21625) | 322 | **199 (62 %)** | 199 | 0 |
| HINTER h6010c12 | 6010 C12 (-18612,-7920) | 1440 | **820 (57 %)** | 820 | 0 |
| HINTER h6010c13 | 6010 C13 (-18812,-20920) | 1250* | **903** | 903 | 0 |
| DAVOR v3050c2 | 3050 C2 (-20400,-14368) | 684 | **0** | 0 | 0 |
| DAVOR v30A0c4 | 30A0 C4 (-21843,17763) | 8210* | **0** | 0 | 0 |
| DAVOR v4040c7 | 4040 C7 (-7200,-20022) | 1170 | **0** | 0 | 0 |
| DAVOR v40A0c7 | 40A0 C7 (-17592,-24095) | 3442* | **0** | 0 | 0 |
| DAVOR v5020c9 | 5020 C9 (-12133,-13325) | 5007 | **0** | 0 | 0 |
| DAVOR v6010c12 | 6010 C12 (-14412,-5720) | 9489 | **0** | 0 | 0 |
| DAVOR v6000c3 | 6000 C3 (4933,-6384) Band 2 | 4495* | **0** | 0 | 0 |
| FREI f3050c5 | 3050 C5 (-13300,-16368) | 903* | **0** | **538** (bis auf den Kopf geloescht) | 0 |
| FREI f3040c1 | 3040 C1 (-22500,-11400) | 222* | **9** | 87 (Beine hinter der Kiste weg) | 0 |

(* Figurpunkte gegen das Hintergrundbild statt gegen einen Lauf ohne Figur gezaehlt — leicht ueberzaehlt, die Spalten
"uebermalt" sind davon unabhaengig.) In allen 18 Szenen ist die vorher-exe bitgleich zu B (§1.4).
- HINTER: die Masken verdecken Leon wieder (57..98 %; der Rest ist der Teil der Figur, der ueber/neben dem Objekt steht).
- DAVOR: kein einziger Punkt von Leon wird uebermalt.
- FREI: die Fehlverdecker sind weg (538 -> 0, 87 -> 9; die 9 Punkte in 3040 C1 sind Saumpunkte der verbliebenen Kistenmaske
  am Fussende, s. OFFEN).
- Bilder (A | N | B | V, 3-fach): `p37/img/final_montage1.png` (f3050c5, f3040c1, h3050c2, h5020c0), Einzelbilder
  `p37/img/z_<szene>.png`.

### 4.2 Elza und weitere Raeume der Stages 5/6 (Fortsetzung 2026-10-13)
Gleicher Aufbau (A final, B final `RE15_NO_PRI=1`, C ohne Figur, V vorher-exe bddd6b31; Elza = `RE15_TITLE_SHOT_CHAR=1`,
der Sprung laedt ihre Raumvariante ROOMxxx1 mit ihrer eigenen MSK-Datei; Fenster = Projektion der Figur wie §4.1;
S/H wie §4.1, jetzt jeweils gegen einen eigenen Lauf ohne Figur gezaehlt). Log je Lauf A `fg_atlas=1`, V `fg_atlas=0`;
**V ist in allen 28 neuen Szenenlaeufen bitgleich zu B** (0 Punkte > 24 im ganzen Bild).

| Szene | Raum/Cut, Lage | Leon: Figurpunkte / uebermalt | Elza: Figurpunkte / uebermalt |
|---|---|---|---|
| HINTER h3050c2 | 3050/3051 C2 (-22000,-20768) | 1234 / **1123 (91 %)** | 1260 / **1158 (92 %)** |
| DAVOR v3050c2 | 3050/3051 C2 (-20400,-14368) | 684 / **0** | 1067 / **0** |
| HINTER h4040c4 | 4040/4041 C4 (-23200,-922) | 208 / **146 (70 %)** | 231 / **185 (80 %)** |
| DAVOR v4040c7 | 4040/4041 C7 (-7200,-20022) | 1170 / **0** | 1253 / **0** |
| HINTER h5020c9 | 5020/5021 C9 (-13133,-21625) | 322 / **199 (62 %)** | 309 / **224 (72 %)** |
| DAVOR v5020c9 | 5020/5021 C9 (-12133,-13325) | 5007 / **0** | 5050 / **0** |
| HINTER h6010c12 | 6010/6011 C12 (-18612,-7920) | 1440 / **820 (57 %)** | 1105 / **639 (58 %)** |
| DAVOR v6010c12 | 6010/6011 C12 (-14412,-5720) | 9489 / **0** | 7931 / **0** |
| HINTER h5100c7 | 5100/5101 C7 (-16130,-2500) | 1228 / **471 (38 %)** | 950 / **397 (42 %)** |
| DAVOR v5100c7 | 5100/5101 C7 (-12130,-5600) | 4243 / **0** | 3685 / **0** |
| HINTER h5090c14 | 5090/5091 C14 (18317,-24665) | 565 / **152 (27 %)** | 556 / **182 (33 %)** |
| DAVOR v5090c14 | 5090/5091 C14 (17017,-22665) | 505 / **2** | 403 / **5** |
| HINTER h5050c1 | 5050/5051 C1 (-16400,-20850) | 5074 / **421 (8 %)** | 3463 / **304 (9 %)** |
| DAVOR v5050c0 | 5050/5051 C0 (-22400,-20950) | 13552 / **0** | 10666 / **0** |
| DAVOR v50B0c2 | 50B0/50B1 C2 (-15100,-19928) | 5162 / **0** | 4915 / **0** |
| HINTER h6000c7 | 6000/6001 C7 (20933,-18184) | 252 / **164 (65 %)** | 322 / **199 (62 %)** |

- ⛔ RICHTIGGESTELLT (Nachbesserung 1, Abnahme 0 M2): die Aussage "Leon und Elza verhalten sich gleich" galt fuer die
  gemessenen Szenen, NICHT allgemein - fuer ROOM30A1 (RDT und MSK bytegleich zu 30A0) hatte die Tabelle keine Zeile, Elza
  wurde VOR der Buero-Trennwand uebermalt (265 von 464 Punkten). Seit Nachbesserung 1 haben alle 15 Raumpaare dieselben
  Zeilen (Riegel). Urspruenglicher Text: (die Elza-Dateien der Stages 3..6 sind bytegleich, die RDTs 3051/4041/6011 auch;
  5021 weicht ab und hat eine eigene Menge Tabellenzeilen). Stage 5 hat HINTER-Szenen in 5020, 5100 (Beine hinter dem
  niedrigen Vordergrund-Objekt), 5090 (Beine hinter dem Maschinenteil) und 5050 (Knie/Messer hinter der dunklen Kante);
  >= 60 % liefert der Szenenfinder (`suche 25 5`) in Stage 5 nur in 5020 (Daten der Generator-Masken, §6).
- DAVOR v5090c14: 2 (Leon) / 5 (Elza) Saumpunkte am Rand der Pfeilermaske, s. OFFEN 2.
- Die Zeilen 1..8 laufen als Riegel `integration_r37_masken36_stage3..6` (Leon und Elza, Werte reproduziert, §5).
- Bilder: `p37/img/z4_<l|e><szene>.png`, `z5_*`, `z6_*` (A | B | V, 3-fach).

### 4.3 FREI an der exe als Riegel (Stage 3, Leon und Elza)
ROOM3050 Cut 5 (-13300,-16368) - Figur frei im Gang VOR der Maschine (§3.2), Lauf C = (-23996,1523) ausserhalb des
Regionsvierecks von Cut 5 ((-22600,-21300) (-22268,-13250) (1700,-3450) (1600,-20890)), Fenster 159 87 188 182:

| Stand | Leon: Figurpunkte / uebermalt | Elza |
|---|---|---|
| final (P) | 883 / **0** | 997 / **0** |
| nur Lader-Regel, ohne Ausschluss (exe `re15_pc_p37nachher.exe`) | 883 / **538** | - |
| vorher (bddd6b31) | 883 / 0 (keine Masken gezeichnet) | - |

Der Ausschluss ist damit an der exe belegt (538 -> 0); B des alten Laufs und B des Riegel-Laufs sind bitgleich (cmp).

### 4.4 Je Raum/Cut an der exe: JEDE der 216 Generator-Sektionen (Leon 108, Elza 108)
Auftrag "je Raum/Cut nachweisen": ein Lauf je Sektion (`RE15_DEBUG_JUMP=<Leon-Raum>@250`, fuer Elza zusaetzlich
`RE15_TITLE_SHOT_CHAR=1`, `RE15_FORCE_CUT=<cut>`, `RE15_EXIT_AT=40#<raum>`); Soll = Maskenzahl nach dem Ausschluss
(Engine-Leser `re15_pri_parse_section` + `re15_masken36_filter`, Hilfsprogramm Scratchpad `p37/erwart.c`); Ist = Logzeile
`[pri] cut=<c> ... masks=<n> fg_atlas=<f>` nach `loaded room<raum>.rdt` + Zeile der Spur P. Ergebnisliste:
`analysis/befunde_runde37/P_masken36_alle_cuts.txt`.
- **Stand P: 214 von 216 Sektionen `fg_atlas=1`, Maskenzahl == Soll, Zeile der Spur P (BSS-Atlas des Cuts).** Die zwei
  uebrigen sind ROOM3050/3051 Cut 6: alle 7 Rechtecke ausgeschlossen (Bodenflaechen in der Auswahl, §3.2) -> masks=0 ==
  Soll, nichts zu zeichnen, kein Atlas noetig.
- **Stand vor P (vorher-exe bddd6b31), dieselben 216 Laeufe: 216 von 216 TOT** - `fg_atlas=0`, Maskenzahl == ungefilterte
  Zahl der Datei, keine Zeile der Spur P. Damit ist die Regression je Raum/Cut belegt und behoben (Liste je Zeile mit
  beiden Staenden in `P_masken36_alle_cuts.txt`).


## 5. Tests

Alle in `re15_port/tests/unit/probes/r37_masken36.cmake` (keine Aenderung an tests/unit/CMakeLists.txt oder
tests/integration/CMakeLists.txt):

| Test | misst | Ergebnis |
|---|---|---|
| `unit_r37_masken36_zensus` | alle MSK-Dateien: je Sektion Original-NULL, Atlas-Quelle nach `re15_pri_atlas_quelle`, kein toter Cut, Blit-Pruefung (Mittel <= 40, Anteil > 48 <= 20 %); Stages 3..6 = 30 Dateien / 216 Sektionen / alle BSS; Gegenproben alte Regel (216 tot) und falscher Atlas (ROOM1220 Cut 0/2/4/6/8: 55.9..85.3 statt 4.7..5.8) | PASS (6/6) |
| `unit_r37_masken36_ausschluss` | Ausschluss-Tabelle == Nachrechnung aus MSK/BSS/RDT (Rechteck fuer Rechteck), keine verwaiste Tabellenzeile (85 == 85), danach keine Fehlverdeckung, je Stage 3..6 Verdeckung HINTER Zellen bleibt (656684 / 440334 / 142077 / 730252 Figurpunkte), Gegenprobe ohne Ausschluss Fehlverdeckungen Stage 3 / 5 / 6 = 405755 / 291880 / 877426 | PASS (5/5) |
| `unit_r37_masken36_verdeckung` | die an der exe gemessenen Leon-Szenen (§4.1) im Modell nach dem Ausschluss | PASS (3/3) |
| `integration_r37_masken36_stage3..6` | echte exe je Stage, **Leon und Elza**: HINTER >= Haelfte verdeckt, DAVOR 0 uebermalt, Log `fg_atlas=1` + Zeile der Spur P; Stage 3 zusaetzlich FREI (Leon und Elza 0 uebermalt) | PASS (4/4) |

**Riegel-Defekt behoben (Fortsetzung):** `unit_r37_masken36_ausschluss` pruefte "Tabelle == Nachrechnung" mit den
Abweichungen des ZWEITEN Laufs (nach `memset`), der die Tabelle mit sich selbst vergleicht - der Check war immer wahr.
Jetzt wird das Ergebnis der Nachrechnung gesichert und geprueft; neu ist der Check "keine verwaiste Tabellenzeile"
(er haette die fuenf 0x1221-Zeilen nach be2dbd26 gemeldet).

### 5.1 Gezielte Laeufe (KEINE volle Suite; Bau `local_build.sh configure` + `build`)
`ctest --test-dir re15_port/build -R "r37_masken36|r37_pri1100|pri_masken|r36_pri|pri_hashes|unit_pri_" --timeout 900 --output-on-failure`
-> **`100% tests passed, 0 tests failed out of 22`** (unit_pri_hashes, unit_pri_inventar_riegel, unit_pri_kopfschnitt,
unit_pri_eingemessen, unit_pri_silhouette, unit_r36_pri_absperrung, unit_r36_pri_schatten, integration_r36_pri_schatten,
unit_r37_masken36_zensus/ausschluss/verdeckung, integration_r37_masken36_stage3..6, unit_r37_pri1100_leck/marke/zwilling/
elza, integration_r37_pri1100_marke/elza, integration_pri_masken). Laufzeiten integration_r37_masken36_stage3..6:
206 / 159 / 129 / 128 s (Stage 3 mit FREI).
Zusammenfuehrung: `git merge-tree --write-tree r37/integration HEAD` -> Konflikte nur in AUFTRAG.md/VERTRAG.md
(Orchestrator-Dateien), Code konfliktfrei.

## 6. OFFEN

1. ~~Sektionen ohne Stehplatz im Modell (32 von 216)~~ - **erledigt in Nachbesserung 1:** die Ursachen waren fehlende
   Tuer-Saaten (ROOM30A1, M1), Saaten nur einer Raumvariante (ROOM5021) und schraege Zellen als Kasten (ROOM4040/4080 ganz zu,
   ROOM3040 Laufsteg nicht verbunden). Uebrig sind ROOM3050/3051 Cut 15, belegt ohne moegliche Figur (Ereignis-Cut,
   Akteur-Cull) - Liste im Riegel, jede weitere faellt durch.
2. **Saumpunkte:** ROOM3040 Cut 1 (Szene f3040c1): 9 von 222 Figurpunkten am Fussende werden noch von der verbliebenen
   Kistenmaske uebermalt (vorher 87). Die Lage (-22500,-11400) liegt im Modell nicht im erreichbaren Gitter dieses Cuts
   (Kiste auf dem Treppenabsatz). ROOM5090 Cut 14 DAVOR (17017,-22665): 2 (Leon) / 5 (Elza) von 505 / 403 Punkten am
   Rand der Pfeilermaske (§4.2) - die Figur ist dort breiter als das Modellmass 700 (Arm/Haare), das Modell sieht 0.
   Beide nicht weiter verfolgt; die Riegel-Szenen (§5) haben 0.
3. **Objekte ohne Kollisionszelle:** ihre Rechtecke fallen weg, sobald sie eine erreichbare Figur verdecken (keine Zelle
   davor) — z.B. Ueberhaenge, Gelaender ohne Zelle. Folge = Zustand vor P (keine Verdeckung), nie eine Fehlverdeckung.
   944 von 6478 Rechtecken sind ausgeschlossen (15 %).
4. **Figurmass:** Leon (gemessen 3000 x 700); Elza an der exe in 8 Szenen + 8 weiteren gemessen (§4.2), gleiches Bild.
   Groessere Figuren (z.B. Birkin, Tyrant-artige) sind nicht eigens geprueft.
5. **PSX:** `platform/psx/src/pri_psx.c` kennt keinen eigenen Atlas — Freistellungs-Masken wuerden dort den BSS-Atlas abtasten
   (vorbestehend, PSX-Ziel ungebaut). Der Ausschluss-Haken ist fuer den Gleichlauf in `platform/psx/main.c` eingetragen.
6. **ROOM1221 (Elza): erledigt.** be2dbd26 (Integration) im Spurbaum uebernommen, ROOM1221.MSK ist weg, Elza nutzt dort die
   Leon-Freistellungen (Spur A). Tabelle neu erzeugt (0x1221-Zeilen weg), der neue Riegel-Check "keine verwaiste
   Tabellenzeile" haelt das fest.
7. Die Generator-Masken selbst (Tiefenband 0.90, Bodenflaechen in der Auswahl) bleiben unveraendert (Vertrag: MASKS/*
   nicht umschreiben). Ein Neubau der Stages 3..6 aus Freistellungen wie in STAGE1 waere der Weg zu vollstaendiger und
   genauer Verdeckung — eigener Auftrag.
8. (NB1: ROOM5050 Cut 1 verliert 7 weitere Rechtecke, h5050c1 an der exe 421 -> 52 von 5074, s. NB1.4.)
   **Stage 5: HINTER >= 60 % nur in ROOM5020.** Der Szenenfinder findet in den anderen Stage-5-Raeumen nur Teilverdeckungen
   (5100 C7 38/42 %, 5090 C14 27/33 %, 5050 C1 8/9 %, an der exe gemessen) - die Generator-Masken decken dort nur die
   unteren Teile der Objekte. Riegel Stage 5 = ROOM5020; die anderen Raeume sind gemessen (§4.2), nicht verriegelt.


## 7. Fuer den Nutzer

- **Was sich aendert (Stages 3..6, Leon und Elza):** Vordergrund-Objekte verdecken die Figuren wieder, wenn sie dahinter
  stehen — z.B. die Maschine in FACTORY LINE B (ROOM3050), der Pfeiler am Treppenaufgang (ROOM5020), die Wandbloecke im
  WEST PASSAGE (ROOM4080), der Treppenpfeiler am Bahnsteig (ROOM6010). Seit v0.7.19 (2026-09-08, Commit 53741fe5) war
  das in diesen Stages unbemerkt ganz ausgefallen; die Masken selbst gibt es seit v0.3.96 (2026-09-03). Rechtecke, die
  eine Figur an einem erreichbaren Platz VOR einem Objekt uebermalen wuerden, bleiben aus (berechnet fuer jede der 216
  Sektionen, Leon und Elza; an der exe in den Szenen der Riegel 0 Punkte). Bekannte Reste: Saumpunkte am Rand einzelner
  Masken (ROOM3040 Cut 1: 9 Punkte am Fussende, ROOM5090 Cut 14: 2/5), s. OFFEN 2.
- Stellen, an denen die alten, automatisch erzeugten Masken Leon faelschlich geloescht haetten (Bodenflaechen, Wandstuecke,
  das Band knapp vor Objekten — dieselbe Art Fehler wie Ihre Meldung vom 2026-09-03 "Leon verschwindet"), bleiben aus.
  Wo Ihnen eine Verdeckung fehlt: F9-Marke wie gewohnt; der Weg dafuer ist eine Freistellung wie in STAGE1.
- Nachbesserung 1 (nach der unabhaengigen Abnahme): Elza wurde im Sewer Control Room (ROOM30A1) VOR der Buero-Trennwand
  bis auf einen Stiefel uebermalt - behoben (gemessen an der exe auf dem echten Weg von der Tuer: 265 -> 0 Punkte). Dabei
  zeigte sich dieselbe Fehlerart in den schraegen Gaengen von EAST/WEST PASSAGE (ROOM4040/4080, z.B. die Wandblock-Kante
  schnitt Leon an der Seite ab): auch dort bleiben diese Rechtecke jetzt aus. Folge: im EAST PASSAGE (ROOM4040 Cut 4/7)
  verdecken die Wandbloecke weniger als vorher; Verdeckung HINTER der Wand im WEST PASSAGE (ROOM4080 Cut 5) bleibt (879 von
  883 Punkten).
- STAGE1/2 (Ihre Freistellungen) und Elzas Zwillings-Masken aus Runde 37 Spur A sind unveraendert.
- Assets/Paket: Spur P bringt keine neuen oder geaenderten Asset-Dateien (Stage-3..6-MASKS unveraendert). Die Entfernung
  von ROOM1221.MSK / ROOM2091.MSK + _PRI00..07.TIM im Spurbaum ist be2dbd26 der Integration (inhaltsgleich uebernommen),
  nicht neu. Neu im Code:
  `engine/src/masken36_r37.c` + `engine/src/gen/masken36_ausschluss_r37.inc` -> Android: .cxx-Cache neu konfigurieren
  (neue Quelldatei, reai-v2-android-glob-cache); APK neu bauen.
- Sprachdateien: keine. Bedienhinweise: keine neuen Schalter; im debug.log steht je Cut
  `[pri] ROOMxxxx Cut n: Port-Masken ohne eigenen Atlas -> BSS-Atlas des Cuts (Runde 37 Spur P)`.
- Entscheidungsfragen: keine.


## Nachbesserung 1 (2026-10-13, nach Abnahme 0 `P_abnahme_0.md`)

Stand (fortlaufend):
- [x] M1 Elza ROOM30A1: Ursache, Beleg, Aenderung, Messung nachher (NB1.1)
- [x] M2 Riegel (leer wahr ohne Stehplatz, bytegleiche Paare, Elza 30A1 an der exe) + Dossier-Aussagen §4.2/§7 (NB1.2)
- [x] M3 Versionsangabe (v0.7.19 statt v0.6.x) (NB1.3)
- [x] Bau + gezielte Tests (NB1.5): `100% tests passed, 0 tests failed out of 30`

Werkzeuge (Scratchpad dieser Sitzung `nb1/lauf.sh`: exe-Kopie `re15_pc_nb1_<name>.exe` neben re15_pc.exe, Arbeitsordner
`nb1/runs/<name>`, `RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_WINDOW_SCALE=1 RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2
RE15_FRAMEDUMP=<bild>:f.ppm RE15_EXIT_AT=<bild>#<raum>`). exe "alt" = `re15_pc_p37final2.exe` (Stand P vor der
Nachbesserung, Tabelle 85 Zeilen; Log ROOM30A1 Cut 0 `masks=46`), exe "neu" = re15_pc.exe des Stands NB1. Zaehlung S/H
mit `probe_r37_masken36_ppm` wie §4 (A mit Masken, B `RE15_NO_PRI=1`, C Figur ausserhalb des Regionsvierecks), Fenster
aus `test_r37_masken36 proj` (neu, gibt fuer h4040c4 und f3050c5 genau die Fenster der Riegel aus). Neue Werkzeug-Modi
im Riegel-Programm (kein Riegel): `saaten <raum>` (Saaten mit Herkunft), `cut <raum> <cut>` (Regionsviereck, Zonen,
freie/erreichte Punkte je Band, Zelltypen), `fehl <raum> <cut> [x z]` (Stellen, an denen die UNGEFILTERTEN Masken eine
Figur ohne Zelle davor verdecken; mit x z je Figurpunkt), `proj <raum> <cut> <x> <z> [band]` (Figurfenster).

### NB1.1 Mangel M1 - Elza in ROOM30A1 VOR der Buero-Trennwand uebermalt

**Messung vorher (Weg der Abnahme nachgefahren, exe alt):** `RE15_TITLE_SHOT_CHAR=1 RE15_DEBUG_JUMP=30A0@250
RE15_PLAYER_POS=-25500,11700,3425 RE15_INPUT_SCRIPT=U1.07 RE15_INPUT_SCRIPT_START=420`, Bild 240:
`[walk] F240 cut=0 ... pl pos=(-24284,0,13657)` (Kamera schaltet selbst auf Cut 0), `[pri] cut=0 pri_offset=0x3C0 masks=46
fg_atlas=1`, Fenster 173 75 201 154, C = Zwangs-Cut 0 bei (-19800,19763): **Elza S=464 H=265** - die Zahl der Abnahme.

**Ursache (gemessen, Werkzeug `saaten`):** ROOM30A0 hat 12 Saaten (10 AOT + Tuer-Ankunft (-25500,11700) aus ROOM3090),
ROOM30A1 nur die 10 AOT-Saaten; die liegen alle in soliden Zellen (Abnahme `frei 0`) -> 0 Stehplaetze in allen Cuts ->
keine Tabellenzeile. Die Tuer gibt es aber auch fuer Elza (SCD-Ablauf `re15_port/tools/scd_dump_room.py`):
- ROOM3091.RDT main00 +0x1398 `Door_aot_set 3b 02 02 31 ... 64 9c 00 00 b4 2d 00 00 02 0a 00` = Slot 2, sce 2, Ankunft
  (-25500,0,11700), Nutzlast +8 Stage 2 / +9 Raum 0x0A - bytegleich zu ROOM3090 +0x1390.
- ROOM3091 sub00 +0x148E `Ifel_ck` / +0x1492 `Ck(4,16)=0` / +0x149A `Ck(4,8)=0` -> +0x14B2 `Aot_reset 46 02 01 31 08 00 ff ff`
  = Slot 2 wird beim ERSTEN Besuch zur Nachrichtenflaeche (sce 1); das Elza-Ereignis sub07 setzt (4,16) (+0x16D0
  `Set 22 04 10 01`) und schaltet die Tuer zurueck (+0x1D7C `Aot_reset 46 02 02 31 64 9c 00 00 b4 2d` sce 2). Ab dann ist
  Slot 2 die Tuer nach ROOM30A1.
- Der Generator nahm Tuer-Saaten nur aus dem VM-Stand nach main00+sub00 mit frischen Flags (= erster Besuch) -> genau
  diese Tuer fehlte.
- Die Raumvariante kommt nicht aus dem Tuersatz: der Warp liest nur Stage/Raum (FUN_8001d600 Tuerzweig @0x8001d82c:
  @0x8001d874 `lw a0,-13912(a0)` = 0x800ac9a8 Nutzlast-Zeiger, gesetzt vom Tuer-Handler @0x800430c4 `sw a0,-13912(at)`;
  @0x8001d94c `lbu v0,9(a0)` -> @0x8001d95c `sh v0,4066(at)` = Raum 0x800b0fe2, @0x8001d960 `lbu v0,8(a0)` = Stage,
  @0x8001d930 `lbu v1,10(a0)` -> Cut 0x800b0fe4). Derselbe Raum-Index 0x800b0fe2 (ohne Variante) waehlt den Hintergrund:
  @0x80021c90 `lh a1,4066(a1)`, Stage @0x80021ca8 `lh v0,4064(v0)`, Cut @0x80021c88 `lh a3,4068(a3)` -> Sektor-Tabelle
  @0x8007438c (@0x80021cbc..@0x80021cd4), Cut*32 Sektoren @0x80021cd8. Leon- und Elza-Variante eines Raums sind derselbe
  Ort mit demselben Hintergrund und (Stages 3..6) derselben MSK-Datei (15 Paare bytegleich).

**Aenderung (nur der Generator/Riegel `tests/unit/test_r37_masken36.c`; Lader und Laufzeit-Code unveraendert):**
1. Saaten aus JEDEM Tuersatz 0x3B jeder RDT, statisch gelaufen wie scripts/door_graph.py / test_r36_kartenbits.c
   (Regionen der main-/sub-Sektion, Laenge aus `scd_opcode_size_at` = Vorschub der VM; Satzlaenge 32/40 nach pc[3]&0x80
   @0x80040618 `lbu v0,3(v1)` / @0x80040620 `andi v0,v0,0x80` / @0x80040630 `addiu v0,v1,40` / @0x80040634 `addiu v0,v1,32`;
   Nutzlast pc+14 bzw. pc+22 @0x80042fb8 `addiu a0,s0,12` / @0x80042f90 `addiu a0,s0,20` mit s0 = pc+2). Ankunft (0,0,0)
   zaehlt nicht (aot_common.c). Saaten 1979 -> 2626.
2. Saaten des RAUMPAARS xxx0/xxx1: die Flutfuellung eines Raums startet auch an den Saaten seiner anderen Variante,
   laeuft aber in der Kollision der EIGENEN RDT (frei/in_zonen) - nur wo diese Variante stehen kann.
Wirkung auf die Tabelle (Zwischenstand vor NB1.2, `erzeuge` gegen die alte Tabelle): +3 Zeilen ROOM30A1 Cut 0/1/3
(25/21/14 Rechtecke, bitgleich zu 30A0), +2 Zeilen ROOM5021 Cut 6/12 (33/1, bitgleich zu 5020 - der Hinweis Abnahme §4.6),
ROOM6000/6001 Cut 9 +1 Rechteck (Tuer ROOM6010 +0xF88/+0xFAC nach ROOM6000, im VM-Stand nicht aktiv). Alle anderen Zeilen
bitgleich; ohne Stehplatz 32 -> 22.

**Messung nachher (exe neu, derselbe Weg):** Elza `masks=21 fg_atlas=1` + Zeile der Spur P, `[walk] F240 cut=0 ...
pl pos=(-24284,0,13657)`, **S=464 H=0** (Bild bitgleich zu B im Fenster); Leon an derselben Stelle S=602 H=0.
Bild Scratchpad `nb1/me1.png` (alt | neu | ohne Masken): alt bleibt von Elza nur der Stiefel unter der Trennwand, neu
steht sie ganz davor. Riegel: `integration_r37_masken36_stage3` faehrt diesen Weg (Elza und Leon, Bild 240, verlangt
Cut 0 von selbst, H == 0), `unit_r37_masken36_verdeckung` Szene f30A1c0 (FREI: ungefiltert 57 von 81 Figurpunkten
verdeckt, gefiltert 0).

### NB1.2 Mangel M2 - Riegel leer wahr, Paare ungeprueft, Dossier-Aussagen

**Ursache 1 - leere Sektionen:** "keine Fehlverdeckung" ist fuer Sektionen ohne Stehplatz leer wahr. Nach NB1.1 blieben 22
(11 Paare): ROOM3040/3041 Cut 6/12, 3050/3051 Cut 15, 4040/4041 Cut 9..12, 4080/4081 Cut 5..8. Gemessen (Werkzeug `cut`):
- ROOM4040 Cut 9: im Regionsviereck 0 freie Punkte auf Band 0; die soliden Zellen dort sind type 1:45, **2:60, 4:5, 6:5, 7:10**
  (Zaehlung mit Mehrfachnennung je Quadranten-Partition) - die schraegen Gaenge. ROOM4080 Cut 5 ebenso (2:40, 5:15 ...).
  Das Modell nahm JEDE Zelle als ihren Kasten (`x - PR .. x + width + PR`), der Kasten einer schraegen Zelle sperrt den ganzen
  Gang. Der Port-Resolver kennt die Formen (re15_collision.c: push_diag2/4/5/6/7, push_circle, push_caps8/9; Spieler-Aufruf
  `collision_constrain_impl(..., PR, 1u, ...)`, PR 450 = DAT_80073e94[6]).
- ROOM3040 Cut 6: Band 1 (Laufsteg) 2166 freie Punkte, 0 erreicht - der Laufsteg haengt nur ueber schraege Zellen
  (Band 1: 2:5 4:5 5:5 6:5) an den Saaten.
- ROOM3050 Cut 15: auf keinem Band erreicht. Keine RVD-Zone schaltet auf Cut 15 (nur Zone 38 `15 -> 0` = das
  Regionsviereck (-32700,12500)..(-6500,32700)); einzig das Ereignis sub10 schaltet ihn (ROOM3050.RDT +0x2596 `Cut_chg 29 0f`,
  nach `Message_on` 3/4 und `Ck(12,31)=0`), ausgeloest am Pruef-AOT Slot 10 (+0x1FB4 `Aot_set 2c 0a 03 31 ...`, Rechteck
  (-25900,-16900) 1000 x 2000, Ereignis `18 0a` = sub10). Die Figur steht dabei bei z -16900..-14900, ausserhalb des
  Regionsvierecks (z >= 12500) -> der Akteur-Cull (FUN_8002c18c, re15_rdt_get_region_quad) zeichnet sie in Cut 15 nicht.
  ROOM3051.RDT ist bytegleich zu ROOM3050.

**Aenderung 1 - Zellform aus dem Resolver:** `form_drin` fragt `re15_collision_constrain_contact_band` (= FUN_8003b0a4-Port)
mit genau dieser Zelle in jeder Quadranten-Partition (FUN_8003aea0), Spieler-Maske 1, Band der Zelle: drin = der Resolver
verschiebt einen dort STEHENDEN Spieler (alte = neue Lage; push_diag4..7 bestimmen die Seite aus der alten Lage, mit der
eigenen gilt die Seite des Punkts). Rechtecke (type 1) bleiben ihr Kasten - Raeume nur mit Rechtecken rechnen bitgleich
wie vorher. Frei-Test mit Radius 450; Sichtstrecke Kamera -> Figurpunkt gegen die Form im Raster 25 (Zellen mit Kamera
oder Figurpunkt in der FORM zaehlen nicht). Zellen entdoppelt, frei-Merker je Gitterpunkt, Sicht-Merker je Stehplatz
und Figurpunkt (Laufzeit `erzeuge` 52 s -> 30 s, `ausschluss` 85 s; Zeitgrenze der Unit-Riegel 300 -> 900 s).
Gegenprobe der Formfrage: Variante "aus jeder der vier Achsrichtungen um 1 heranruecken" gegen "stehend" - gleiche
Ausschluss-Bits in allen 216 Sektionen, nur ROOM4040/4041 Cut 11 5073 statt 5426 Stehplaetze. Raster 25 gegen Abtastung 25
direkt am Resolver: 2 Rechtecke mehr (ROOM4080/4081 Cut 8, 5090/5091 Cut 11; Rundung am Rand, sichere Richtung).

**Tabelle danach (`unit_r37_masken36_ausschluss`):** Saaten 2626, 216 Sektionen, 6478 Masken, **1224 ausgeschlossen
(vorher 944), 124 Zeilen (vorher 85)**, ohne Stehplatz **2** (ROOM3050/3051 Cut 15, vorher 32). Stage 4 hat jetzt Zeilen
(ROOM4040/4041 Cut 4..7, 9..11; ROOM4080/4081 Cut 1..3, 5..8); Fehlverdeckungen ohne Ausschluss je Stage 3/4/5/6 =
419912 / 25510 / 319668 / 880614 (Stage 4 vorher 0 - die schraegen Gaenge waren unsichtbar), HINTER-Zellen-Verdeckung mit
Ausschluss 682688 / 276494 / 180376 / 727312.

**Ist das an der exe so? (alt gegen neu, Zwangs-Cut, Bild 120, Werkzeug `fehl` fuer die Stelle):**

| Stelle | alt: masks / S / H | neu: masks / S / H |
|---|---|---|
| ROOM4040 Cut 4 (-22000,-3722) Leon neben dem Wandblock | 7 / 397 / **57** | 2 / 397 / **0** |
| ROOM4080 Cut 2 (8275,-21910) Leon weit hinten im Gang | 15 / 114 / **4** | 5 / 114 / **0** |
| ROOM4080 Cut 5 (17275,-12710) HINTER der Wand | 14 / 883 / 879 | 5 / 883 / **879** |
| ROOM4080 Cut 5 (9675,-9310) DAVOR, nah an der Kamera | 14 / 8374 / 0 | 5 / 8374 / **0** |

Bilder `nb1/m40.png` (4040 C4: alt schneidet die Wandblock-Kante durch Leon, neu = ohne Masken), `nb1/m80.png`,
`nb1/m85.png`. ROOM4080 C2: das Modell sagt 36 von 81 Punkten, die exe zeigt 4 von 114 - die Modellfigur (+-350 quer) ist
dort breiter als Leon von hinten; das Modell irrt in die sichere Richtung (mehr Ausschluss).
**Folge fuer die Riegel-Szenen der Stage 4:** h4040c4 (-23200,-922), h4080c1 und v4040c7 lagen auf Rechtecken, die jetzt
ausgeschlossen sind (sie uebermalten Leon VOR dem Wandblock, Zeile 1 der Tabelle oben) - ersetzt durch ROOM4080 Cut 5
HINTER (17275,-12710) / DAVOR (9675,-9310) (Szenenfinder `suche 50 4`, Werte der Tabelle oben).

**Aenderung 2 - Riegel:**
- `unit_r37_masken36_ausschluss`: neu "Sektionen ohne Stehplatz == belegte Liste (ROOM3050/3051 Cut 15)" - jede weitere
  Sektion ohne Stehplatz faellt durch (der Check "keine Fehlverdeckung" ist damit ausser in den zwei belegten Sektionen nie
  leer wahr); "RDT- und MSK-bytegleiche Raumpaare haben dieselben Tabellenzeilen" (8 von 8) und "jedes Raumpaar mit MSK hat
  dieselben Zeilen" (15 von 15). Gegenprobe: mit der Tabelle von 7495dc73 meldet der Paar-Check `Paar ROOM30A0/30A1:
  Tabellenzeilen verschieden - RDT und MSK BYTEGLEICH` und `ROOM5020/5021`.
- `unit_r37_masken36_verdeckung`: FREI-Szenen verlangen zusaetzlich die Gegenprobe "ungefiltert verdeckt" (f3050c5 54,
  f30A1c0 57 von 81); neu f30A1c0, h4080c5, v4080c5.
- `integration_r37_masken36_stage3`: der Weg aus M1 an der exe (Elza ROOM30A1 und Leon ROOM30A0, kein Zwangs-Cut, Bild
  240, Cut 0 im Log verlangt, H == 0). `integration_r37_masken36_stage4`: ROOM4080 Cut 5 (Leon und Elza).
- `P_masken36_alle_cuts.txt` vergleicht die Maskenzahl an der exe mit der Tabelle - das ist eine Lader-Pruefung
  (Tabelle kommt an), KEINE Pruefung auf Fehlverdeckung; so steht es jetzt im Kopf der Datei (neu erzeugt, NB1.4).

**Aenderung 3 - Dossier:** §4.2 ("Leon und Elza verhalten sich gleich") richtiggestellt, §6 OFFEN 1 erledigt, §7 neu
formuliert (keine "nie"-Aussage, Reste genannt).

### NB1.4 Messung nachher an der exe (Stand NB1)

**Je Raum/Cut, alle 216 Generator-Sektionen** (Leon 108, Elza 108; Lauf wie §4.4, Soll aus `test_r37_masken36 soll`,
Skript Scratchpad `nb1/alle_cuts.sh` + `nb1/ac_auswerten.py`): **214 OK** (`fg_atlas=1`, `masks == Soll`, Zeile der Spur P)
**+ 2 LEER** (ROOM3050/3051 Cut 6, alle 7 Rechtecke aus, `masks=0`) **+ 0 FALSCH**. Neu erzeugt:
`analysis/befunde_runde37/P_masken36_alle_cuts.txt` (Kopf sagt jetzt ausdruecklich: Lader-Pruefung, keine Pruefung auf
Fehlverdeckung). Beispiele der neuen Zeilen: ROOM30A1 Cut 0 `masks=21` (Soll 21 von 46, vorher P 46), ROOM5021 Cut 6
`masks=22` (22 von 55), ROOM4040 Cut 4 `masks=2` (2 von 7), ROOM4081 Cut 5 `masks=5` (5 von 14).

**Szenen, deren Cut eine neue/andere Zeile hat** (Zwangs-Cut, Bild 120, gleiche Lage wie §4.1/§4.2):

| Szene | alt (P vor NB1): masks / H | NB1: masks / S / H |
|---|---|---|
| v30A0c4 DAVOR ROOM30A0 Cut 4 (-21843,17763) | 14 / 0 | 13 / 8210 / **0** |
| v5050c0 DAVOR ROOM5050 Cut 0 (-22400,-20950) | 8 / 0 | 7 / 13552 / **0** |
| h5050c1 HINTER ROOM5050 Cut 1 (-16400,-20850) | 35 / 421 (8 %) | 28 / 5074 / **52 (1 %)** |
| h4080c5 / v4080c5 (Riegel Stage 4, neu) | 14 / 879 bzw. 0 | 5 / 883 / **879** bzw. 8374 / **0** |
| Elza ROOM4081 Cut 5 HINTER / DAVOR (Riegel) | - | 627 / **621** bzw. 6165 / **0** |
| M1-Weg Elza ROOM30A1 Cut 0 / Leon ROOM30A0 (Riegel) | 46 / 265 bzw. 21 / 0 | 21 / 464 / **0** bzw. 602 / **0** |

h5050c1 verliert 7 Rechtecke von Cut 1: dieselben Rechtecke uebermalen an erreichbaren Stellen eine Figur ohne Zelle davor
(Werkzeug `fehl 5050 1`: (-21500,-24550) 51 von 81 Figurpunkten) - nach der Regel "eine fehlende Maske laesst alles wie
bisher, eine falsche uebermalt den Spieler" (6c29d834) bleiben sie aus; die Teilverdeckung dort war schon vorher nur 8 %
(OFFEN 8). Die uebrigen Szenen aus §4.1/§4.2 liegen in Cuts mit unveraenderten Bits (Riegel-Werte Stage 3/5/6 unten
bitgleich zu vorher: 1123/1234, 0/684, Elza 1158/1260, 0/1067, FREI 0/883, 0/997; 6010 820/1440, 0/9489, Elza 639/1105,
0/7931; 5020 199/322, 0/5007, Elza 224/309, 0/5050).

### NB1.5 Tests
`ctest --test-dir re15_port/build -R "r37_masken36|r37_pri1100|pri_masken|r36_pri|pri_hashes|unit_pri_|sld_atlas|maskgrp|boot_bg_pin|unit_r35_entladen" --timeout 900 --output-on-failure -j3`
(Ausdruck der Abnahme; KEINE volle Suite). Schlussbau `local_build.sh build` -> `=== LOCAL-BUILD-OK (build)`, danach
**`100% tests passed, 0 tests failed out of 30`** (zweimal: vor und nach dem Werkzeug-Modus `soll`; Laufzeiten im
Schlusslauf stage3 308.75 s, stage4 129.45 s, stage5 129.46 s, stage6 129.43 s, ausschluss 87.56 s). Darin die Spur-A-Riegel
unit/integration_r37_pri1100_*, integration_pri_masken, unit/integration_r36_pri_*, unit_pri_hashes, boot_bg_pin -
gruen, unveraendert. Zusammenfuehrung `git merge-tree --write-tree r37/integration HEAD`: Konflikte nur in
AUFTRAG.md/VERTRAG.md (Orchestrator-Dateien), Code konfliktfrei.

### NB1.6 OFFEN nach Nachbesserung 1
1. Saumpunkte ROOM3040 Cut 1 (9 von 222) und ROOM5090 Cut 14 DAVOR (2 / 5) - Bits dieser Cuts unveraendert, wie §6.2.
2. Ausschluss ist rechteckweise: ein Generator-Rechteck, das hinten richtig und vorne falsch verdeckt, faellt ganz weg.
   Mit der Zellform betrifft das jetzt auch Stage 4 (ROOM4040 Cut 4: 5 von 7, Cut 5: 10 von 23 ...) und ROOM5050 Cut 1
   (h5050c1 421 -> 52). Vollstaendige Verdeckung braucht Freistellungen wie in STAGE1 (eigener Auftrag, §6.7).
3. Die Modellfigur (+-350 quer, 3000 hoch) ist an manchen Stellen breiter als Leon (ROOM4080 Cut 2: Modell 36 von 81,
   exe 4 von 114) - Irrtum in die sichere Richtung (mehr Ausschluss). Groessere Figuren als Leon/Elza weiter ungeprueft.
4. ROOM3050/3051 Cut 15 ohne Stehplatz - belegt ohne moegliche Figur (NB1.2), im Riegel als Liste.
5. PSX unveraendert (§6.5).
Werte aus dem Lauf: integration_r37_masken36_stage3 309 s (mit M1-Weg: Elza 464/0, Leon 602/0), stage4 130 s (ROOM4080
Cut 5: Leon 883/879 und 8374/0, Elza 627/621 und 6165/0), stage5 130 s, stage6 130 s, unit_r37_masken36_ausschluss 85 s.
Gegenprobe der neuen Riegel-Checks: mit der Tabelle von 7495dc73 (85 Zeilen) eingebaut faellt `ausschluss` durch -
`Abweichungen 47`, `Paar ROOM30A0/30A1: Tabellenzeilen verschieden - RDT und MSK BYTEGLEICH`, `Paar ROOM5020/5021:
Tabellenzeilen verschieden` (Paare 13 von 15 gleich, bytegleiche 7 von 8), "keine Fehlverdeckung" FAIL; danach die
Tabelle zurueck und neu gebaut (`soll` bitgleich zur Liste vor der Gegenprobe).

### NB1.3 Mangel M3 - Versionsangabe
`git tag --contains 53741fe5 | sort -V | head -1` -> **v0.7.19** (2026-09-08 15:19); `git tag --contains b8db7b2b | sort -V |
head -1` -> v0.3.96 (2026-09-03 12:30). §7 sagt jetzt "Seit v0.7.19 (2026-09-08, Commit 53741fe5) ... die Masken selbst
gibt es seit v0.3.96".


## Nachbesserung 2 (2026-10-14, nach Abnahme 1 `P_abnahme_1.md`)

Stand (fortlaufend):
- [ ] M1 ROOM3050/3051 Cut 15: Generator-Masken uebermalen den Konsolen-Cursor (NB2.1)
- [ ] M2 Kopfoberkante (und Fussende) vom Figurmodell nie abgetastet, ROOM30A0 Cut 0 (NB2.2)
- [ ] Bau + gezielte Tests (NB2.3)

Fortsetzung 2026-10-16 (der erste NB2-Agent brach am 2026-10-14 ~03:3x am Monatslimit ab): vorliegend ff2a5b87 (Generator
pixelgenaues Figurraster, Sektion ohne Stehplatz ganz aus, Werkzeug `punkt`, Riegel-Checks) und 9fd9428b (Tabelle neu,
UNGEPRUEFT: 126 Zeilen, + ROOM3050/3051 Cut 15 36 von 36). Danach master (v0.8.25) gemergt (2bb1bbe3). Diese Fortsetzung
prueft beides kritisch (Disassembly des zitierten NULL-Belegs, Tabelle == Nachrechnung auf dem gemergten Stand, exe-Messung
des Abnahme-Aufbaus) statt es zu uebernehmen. Vergleichs-exe "alt" = `re15_pc_nb2alt.exe` (Kopie der re15_pc.exe des
Stands fb4be428 = Abnahme 1, Tabelle 124 Zeilen).

Werkzeuge dieser Fortsetzung (Scratchpad `p38nb2/`): `lauf.sh <exe> <name> <exit_raum> <framedump> <exit_bild> ENV...`
(exe-Kopie `re15_pc_p38nb2_<quelle>.exe` neben re15_pc.exe, eigener Arbeitsordner `runs/<name>`, `RE15_NO_INTRO=1
RE15_NOAUDIO=1 RE15_WINDOW_SCALE=1 RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2`), Zaehlung `sh.py` (= abnP1/sh.py:
S = |B-C| > 24, H = davon |A-B| > 24). Bau: `local_build.sh configure` (nach dem master-Merge) + `build` -> LOCAL-BUILD-OK.

### NB2.1 Mangel M1 - ROOM3050/3051 Cut 15: Generator-Masken uebermalen den Cursor der Kran-Konsole

**Ursache:** Cut 15 hat im Modell keinen Stehplatz (NB1.2: nur das Ereignis sub10 schaltet ihn, ROOM3050.RDT +0x2596
`Cut_chg 29 0f`; die Figur steht dabei ausserhalb des Regionsvierecks und wird nicht gezeichnet). Die Regel bis NB1 liess
eine Sektion ohne Stehplatz UNVERAENDERT (alle 36 Rechtecke an) - "keine Figur" wurde als "nichts zu uebermalen" gelesen.
Gezeichnet wird in diesem Cut aber der Ziel-Cursor (Objekt 3, sub11..14 `Work_set 2e 03 03` + `Speed_set`), und die
Generator-Masken (Tiefe aus dem Bodenkontakt x0.90) liegen vor ihm.

**Messung vorher (Aufbau der Abnahme nachgefahren, exe alt = nb2alt):** `RE15_SET_FLAG=3:178 RE15_DEBUG_JUMP=3050@250
RE15_PLAYER_POS=-24757,-15900,3072 RE15_FORCE_EVENT=10@300 RE15_PRESS=square@420,square@422,square@480,...,square@960,
square@962` (20 Eintraege), Bild 900, A mit Masken, B `RE15_NO_PRI=1`. Log A: `[scd F783] Cut_chg(15)` -> `[pri] ROOM3050
Cut 15: Port-Masken ohne eigenen Atlas -> BSS-Atlas des Cuts (Runde 37 Spur P)` -> `[pri] cut=15 pri_offset=0x84C
masks=36 fg_atlas=1`; **A/B 138 Punkte > 24, davon 60 mit B = (0,21,0)/(2,60,0) = dunkelgruener Innenrand des Cursors** -
genau die Zahlen der Abnahme (Laeufe `runs/k15_nb2alt_A/B`).

**RE-Beleg (selbst disassembliert, `re15_disasm.py dis 0x800392d4 40`, info/Re1.5/PSX.EXE):** der Masken-Bauer
FUN_800392d4 liest den Masken-Zeiger des Cuts und behandelt FF FF FF FF als "keine Masken":
`800392d8 lw a0,-14472(a0)` (RDT-Zeiger 0x800ac778), `800392e0 lh v0,4068(v0)` (Cut 0x800b0fe4), `80039310 lw v1,36(a0)`
(RDT+0x24 Kameratabelle), `80039314 sll v0,v0,5` (Cut*0x20), `8003931c lw t5,28(v0)` (+0x1C Masken-Sektion),
`80039324 lw v1,0(t5)`, `80039328 addiu v0,zero,-1`, `8003932c bne v1,v0,0x8003933c`, `80039334 j 0x8003955c` mit
Delay-Slot `80039338 sb zero,0(a0)` (Maskenzahl 0, Ende) - sonst `80039358 sb t2,0(a0)` (Zahl = obere Haelfte des ersten
Worts). Bytes (Python, shared_assets/PSX/STAGE3): ROOM3050.RDT und ROOM3051.RDT RDT+0x24 = 0x60, Cut 15 Eintrag @0x240,
+0x1C @0x25C = 0x84C -> `ff ff ff ff` = NULL-Sektion. **Das Original zeichnet in Cut 15 keine einzige Maske.**

**Aenderung (ff2a5b87, hier geprueft):** Generator `sektion_pruefen`: Sektion OHNE Stehplatz -> ALLE Rechtecke aus
(= Original-Zustand des Cuts). Die Masken sind NUTZER-VORGABE fuer Figuren-Verdeckung; wo keine Figur stehen kann, koennen
sie nur Anderes uebermalen. Tabelle (9fd9428b): `{ 0x3050, 15, 36, { 0xFFFFFFFFu, 0x0000000Fu, ... } }` und dasselbe fuer
0x3051. Riegel `unit_r37_masken36_ausschluss`: neuer Check "Sektionen ohne Stehplatz sind GANZ ausgeschlossen" + Pruefung,
dass die Original-Sektion dieser Cuts NULL ist (liest die RDT-Bytes: `ROOM3050 Cut 15: Original-Sektion +0x84C = FF FF FF
FF (NULL - keine Masken)`). Lader und Laufzeit-Code unveraendert: mit 0 Masken nimmt main.c den Weg einer Original-NULL-
Sektion (`pri_nachgezeichnet = 0`, kein Overdraw, `has_fg && pri_n > 0` falsch).

**⛔ Falle beim Nachmessen (master-Merge):** derselbe Aufbau an der exe NEU zeigt in Bild 900 KEINEN Cursor - auch nicht
in B (`runs/k15_neu_A/B`, A bitgleich B, aber leer: Cursor-Innenrand-Punkte in B = 0). Ursache im Log: seit master v0.8.25
(Runde 37 Spur Q, `engine/src/kistenregel_r37.c` Fall K8) liest `Ck(3,0xB2)` @0x01F90 den BESITZ der Red Master Keycard
(Item 0x46) statt des Bits: `[kistenregel] ROOM3050 @0x01F90 Ck(3,0xB2) K8: Item 0x46 NICHT im Inventar ... Zweig ohne
Gegenstand`, danach fehlt `Obj_model_set[2] id=0x03 type=4 pos=(-19554,0,22684)` (das Cursor-Objekt). `RE15_SET_FLAG=3:178`
allein reicht nicht mehr; der Aufbau braucht zusaetzlich **`RE15_GIVE=0x46:1`** (`[kistenregel] ... Item 0x46 im Inventar
-> wie Original`, Objekt 3 wird gesetzt, `[prop-render] pi=2 oid=0x03`). Ein "A == B" ohne Cursor im Bild beweist nichts -
der Riegel verlangt deshalb Cursor-Punkte in B (unten).

**Messung nachher (exe neu, Aufbau + `RE15_GIVE=0x46:1`, Bild 900; Werkzeug `k15zaehl.py`):**

| Lauf | Log Cut 15 | A/B Punkte > 24 | davon Cursor-Innenrand | Cursor-Innenrand-Punkte in B | A bitgleich B |
|---|---|---|---|---|---|
| Leon alt (nb2alt, ohne GIVE - vor dem Merge noetig nicht) | `masks=36 fg_atlas=1` + Spur-P-Zeile | 138 | **60** | 62 | nein |
| Leon neu (`runs/k15g_neu_A/B`) | `masks=0 fg_atlas=1` | **0** | **0** | 62 | **ja** |
| Elza alt (`RE15_TITLE_SHOT_CHAR=1`, ROOM3051) | `masks=36 fg_atlas=1` | 138 | **60** | 62 | nein |
| Elza neu (`runs/k15ge_neu_A/B`) | `masks=0 fg_atlas=1` | **0** | **0** | 62 | **ja** |

`masks=0 fg_atlas=1` ohne Spur-P-Zeile: der R15M-Zweig hat nach dem Filter 0 Rechtecke, main.c nimmt den Weg einer
Original-NULL-Sektion (BSS-Atlas wie jeder Original-Cut, kein Overdraw). Bild `p38nb2/k15g_alt_neu.png` (alt A | alt B |
neu A | neu B, 4-fach): alt fehlt den gruenen Winkeln die dunkle Innenkante, neu ist der Cursor vollstaendig und gleich B.

### NB2.2 Mangel M2 - Kopfoberkante nie abgetastet (ROOM30A0 Cut 0)

**Ursache:** das Figurmodell tastete 9 x 9 Punkte mit Zeilen-MITTEN FIGUR_HOCH/9 * (z + 0.5) = 167 .. 2833 ab; die Baender
0..167 (Fuss) und 2833..3000 (Haar) und alles zwischen zwei Zeilen/Spalten wurden nie gefragt.

**Messung vorher (Zufallsweg der Abnahme nachgefahren):** `RE15_DEBUG_JUMP=30A0@250 RE15_INPUT_SCRIPT_START=520
RE15_INPUT_SCRIPT=R0.69,U1.12,R0.23,U1.15,R0.19,U0.98,R0.44,U0.88,L0.14,U1.02,L0.16,U2.18,R0.67,U1.89`, Bilder 360..440/20,
A mit Masken, B `RE15_NO_PRI=1`, C = Zwangs-Cut 0 bei (-19800,19763) ohne Figur. Lagen alt == neu (`[walk] F360
(-26046,13582)`, `F390 (-25605,13657)`, `F420 (-24897,13261)`):

| Bild | alt (nb2alt, `masks=21`): S / H | neu (`masks=18`): S / H |
|---|---|---|
| 360 | 495 / 0 | 495 / 0 |
| 380 | 328 / **4** | 328 / **0** |
| 400 | 355 / **7** | 355 / **0** |
| 420 | 666 / 0 | 666 / 0 |
| 440 | 995 / 0 | 995 / 0 |

(alt = genau die Zahlen der Abnahme, Bild 380/400.)

**Aenderung (ff2a5b87 + diese Fortsetzung, nur Generator/Riegel `tests/unit/test_r37_masken36.c`):** `figur_raster` tastet
den Figurkasten PIXELGENAU ab: so viele Spalten, wie der Kasten Bildpunkte breit ist (Fuss- oder Kopfkante, die breitere;
mindestens 9, ungerade, Raender eingeschlossen), und je Spalte ein Punkt in JEDER Bildzeile von der Kopfoberkante bis zum
Fuss (perspektivisch richtig auf der senkrechten Kante, an die Enden geklemmt). Diese Fortsetzung: Spaltengrenze
SP_MAX 129 -> 321 (eine Spalte je Bildspalte ueber die volle Bildbreite; `spalte` jetzt uint16_t) und eine SCHRANKE fuer
die zwei Faelle, in denen das Raster nicht pixelgenau ist - Kasten breiter als das Bild (nur dicht vor der Kamera) und
Spalten mit einem Ende hinter der Kamera (vz <= 64): keine Maske im Bildrechteck des Kastens (bzw. im ganzen Bild) hat
depth < bucket(vz des fernsten Punkts). Gemessen (`erzeuge`): **924866 Kaesten pixelgenau, breitester 2505.1 Bildpunkte,
breiter als 320: 28484 (davon Maske koennte verdecken: 0), Spalten halb hinter der Kamera: 65038 (davon: 0)** - die
Ausnahmen koennen keine Fehlverdeckung verbergen; Riegel-Check "wo es nicht pixelgenau ist ... kann keine Maske
verdecken". Die Tabelle ist mit Spaltengrenze 129 und 321 bitgleich (diff).
Konstanten: keine Spielkonstanten neu (FIGUR_HOCH 3000 / FIGUR_HALB 350 an der exe gemessen, Dossier §3.3; SP_MAX ist
Messaufbau = Bildbreite 320 + 1).

**Tabelle danach (Nachrechnung auf dem gemergten Stand, `test_r37_masken36 ausschluss`, 197 s):** Saaten 2626, 216
Sektionen, 6478 Masken, **1444 ausgeschlossen (NB1: 1224; davon 72 = ROOM3050/3051 Cut 15, 148 durch das pixelgenaue
Raster), Abweichungen 0, 126 Zeilen (NB1: 124)**, ohne Stehplatz 2 (belegt, jetzt ganz aus), Raumpaare 15/15 gleich,
bytegleich 8/8. ROOM30A0/30A1 Cut 0: 25 -> 28 Rechtecke aus (neu 5, 20, 21; 20 = der obere Rahmen-Balken der Trennwand).

### NB2.3 Riegel und Gegenprobe

- `unit_r37_masken36_ausschluss` (Generator, erweitert): "Sektionen ohne Stehplatz sind GANZ ausgeschlossen" + Original-
  Sektion dieser Cuts NULL (RDT-Bytes); "Figurraster pixelgenau - wo es nicht pixelgenau ist (Kasten breiter als das Bild,
  Spalte halb hinter der Kamera), kann keine Maske verdecken" (Schranke, NB2.2).
- `unit_r37_masken36_verdeckung` (ff2a5b87): neue FREI-Szene `f30A0c0k` (-25400,13550) ROOM30A0 Cut 0 = die Lage der
  Abnahme; "das Figurraster tastet in jeder Szene Kopfoberkante und Fuss ab (mittlere Spalte)" und Gegenprobe "die alte
  Abtastung (Zeilenmitten) verfehlt in f30A0c0k die Kopfoberkante" (Ausgabe: `Kopfoberkante Bildzeile 77 - Raster neu ja,
  alte Abtastung nein`). Werkzeug `punkt 30A0 0 -25400 13550`: ungefiltert 549 Punkte ohne Zelle davor verdeckt (Zeilen 77/78
  von Maske 20, darunter Maske 19 ...), mit Tabelle 0.
- NEU `integration_r37_masken36_nb2` (tests/integration/test_r37_masken36_nb2.cmake, registriert in probes/r37_masken36.cmake),
  echte exe:
  - M1 Cut 15, Leon und Elza: K_A mit Masken + Karte, K_B ohne Masken + Karte, K_C ohne Masken OHNE Karte (= dasselbe Bild
    ohne Cursor); Zaehlung im ganzen Bild S = Cursor-Punkte (|B-C| > 24), H = davon uebermalt; dazu A/B im ganzen Bild.
    Verlangt Log `Cut_chg(15)`, `Item 0x46 im Inventar`, `[pri] cut=15 pri_offset=0x84C masks=0`, S > 30, H == 0, A/B 0.
  - M2 Zufallsweg ROOM30A0 (Seed 37014 der Abnahme), Bild 380 und 400: S > 50, H == 0 (C = Zwangs-Cut 0 ohne Figur).
  Ergebnis: **Leon Cursor-Punkte 210, uebermalt 0, A/B verschieden 0 | Elza 210 / 0 / 0; Bild 380: 328 / 0, Bild 400:
  355 / 0 -> OK (292 s)**.
- **Gegenprobe an der exe:** derselbe Code (master-Merge) mit der Tabelle von fb4be428 (NB1, 124 Zeilen) gebaut
  (`re15_pc_nb2gegen.exe`, danach Tabelle zurueck und neu gebaut; neue re15_pc.exe bis auf PE-Zeitstempel/Pruefsumme
  (4 Bytes) gleich der gemessenen): der Riegel FAELLT DURCH - `r37_masken36_nb2[k_a]: Cut 15 zeichnet Masken: '[pri] cut=15
  pri_offset=0x84C masks=36 fg_atlas=1'`; aus denselben Laeufen: Cursor Leon S=210 **H=60**, Elza S=210 **H=60**, A/B 138 /
  138; Bild 380 S=328 **H=4**, Bild 400 S=355 **H=7** - genau die Befunde der Abnahme.
