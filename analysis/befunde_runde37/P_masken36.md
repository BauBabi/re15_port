# Spur P masken36 — Runde 37 (2026-10-12)

Orchestrator-Auftrag P (AUFTRAG.md, Regression 53741fe5): die ergaenzten Vordergrund-Masken der Stages 3..6
(30 MSK-Dateien ROOM3040..ROOM6031, erzeugt b8db7b2b 2026-09-03) werden seit 53741fe5 (2026-09-08) fuer NIEMANDEN
gezeichnet. Ziel: R15M-Cut ohne eigenen Atlas wieder mit dem BSS-Atlas zeichnen (wie am 2026-09-03 gebaut),
R15M-Cut mit eigenem Atlas unveraendert, Elza-Zwilling (Spur A) unberuehrt.

Baum: .claude/worktrees/r37_masken36 (Zweig r37/masken36, Basis bddd6b31 = Stand Spur A 06ac2b56 + AUFTRAG/VERTRAG).

## Stand

- [x] Schritt 0: Pflichtlektuere (AUFTRAG, VERTRAG, A_pri1100, A_abnahme_1, b8db7b2b/e100d610/02c62294, 53741fe5)
- [ ] Schritt 1: Messung vorher (Zensus aller MSK, Bisect)
- [ ] Schritt 2: Umsetzung
- [ ] Schritt 3: Messung nachher (Blit-Pruefung, Figur-Messung an der exe)
- [ ] Tests (probes/r37_masken36.cmake)

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

(folgt)

## 4. Messung nachher

(folgt)

## 5. Tests

(folgt)

## 6. OFFEN

(folgt)

## 7. Fuer den Nutzer

(folgt)
