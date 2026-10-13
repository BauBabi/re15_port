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

- Leon und Elza verhalten sich gleich (die Elza-Dateien der Stages 3..6 sind bytegleich, die RDTs 3051/4041/6011 auch;
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

1. **Sektionen ohne Stehplatz im Modell (32 von 216):** dort findet die Erreichbarkeit (Saaten = AOTs mit eigenem Band + Tuer-
   Ankuenfte) keinen Stehplatz im Regionsviereck — meist Kameras auf Ebenen ohne Saat (z.B. ROOM3040 Cut 4..7/12 Laufsteg
   Band 1, ROOM5020 obere Baender, ROOM5090 Band 1, ROOM6010 Band 1/4). Diese Sektionen zeichnen wie am 2026-09-03 gebaut,
   ohne Pruefung auf Fehlverdeckung. Weg: Saaten fuer Treppen-Uebergaenge zwischen Baendern (Treppen-AOT/RAMP-Zonen mit
   beiden Baendern) — nicht gebaut.
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
8. **Stage 5: HINTER >= 60 % nur in ROOM5020.** Der Szenenfinder findet in den anderen Stage-5-Raeumen nur Teilverdeckungen
   (5100 C7 38/42 %, 5090 C14 27/33 %, 5050 C1 8/9 %, an der exe gemessen) - die Generator-Masken decken dort nur die
   unteren Teile der Objekte. Riegel Stage 5 = ROOM5020; die anderen Raeume sind gemessen (§4.2), nicht verriegelt.


## 7. Fuer den Nutzer

- **Was sich aendert (Stages 3..6, Leon und Elza):** Vordergrund-Objekte verdecken die Figuren wieder, wenn sie dahinter
  stehen — z.B. die Maschine in FACTORY LINE B (ROOM3050), der Pfeiler am Treppenaufgang (ROOM5020), die Wandbloecke im
  EAST/WEST PASSAGE (ROOM4040/4080), der Treppenpfeiler am Bahnsteig (ROOM6010). Seit v0.6.x (2026-09-08) war das in diesen
  Stages unbemerkt ganz ausgefallen. Steht die Figur VOR einem Objekt, wird nie etwas von ihr uebermalt.
- Stellen, an denen die alten, automatisch erzeugten Masken Leon faelschlich geloescht haetten (Bodenflaechen, Wandstuecke,
  das Band knapp vor Objekten — dieselbe Art Fehler wie Ihre Meldung vom 2026-09-03 "Leon verschwindet"), bleiben aus.
  Wo Ihnen eine Verdeckung fehlt: F9-Marke wie gewohnt; der Weg dafuer ist eine Freistellung wie in STAGE1.
- STAGE1/2 (Ihre Freistellungen) und Elzas Zwillings-Masken aus Runde 37 Spur A sind unveraendert.
- Assets/Paket: Spur P bringt keine neuen oder geaenderten Asset-Dateien (Stage-3..6-MASKS unveraendert). Die Entfernung
  von ROOM1221.MSK / ROOM2091.MSK + _PRI00..07.TIM im Spurbaum ist be2dbd26 der Integration (inhaltsgleich uebernommen),
  nicht neu. Neu im Code:
  `engine/src/masken36_r37.c` + `engine/src/gen/masken36_ausschluss_r37.inc` -> Android: .cxx-Cache neu konfigurieren
  (neue Quelldatei, reai-v2-android-glob-cache); APK neu bauen.
- Sprachdateien: keine. Bedienhinweise: keine neuen Schalter; im debug.log steht je Cut
  `[pri] ROOMxxxx Cut n: Port-Masken ohne eigenen Atlas -> BSS-Atlas des Cuts (Runde 37 Spur P)`.
- Entscheidungsfragen: keine.

