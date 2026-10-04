# Runde 35 Spur F "inhalt" — Dossier (fortlaufend)

Baum: `.claude/worktrees/r35_inhalt`, Zweig `r35/inhalt`, Basis master 154a73c1 (geprueft: status leer).
Zuteilung (VERTRAG.md): Bank-9-Bits 80 (Memory Card 1010), 81 (Schrot 1090); Nachrichten-IDs 1010 ab 3
(max 5), 1090 ab 10 (max 12); Ereignisse 28 (1010), 29 (1090).

## Punkte (Wortlaut AUFTRAG.md)
1. Doppeltueren unsymmetrisch (Griffe)
2. Codes in den selbsterstellten Dokumenten gruen
3. ROOM1010 / ROOM1220 stehende Zombies weiter von der Tuer
4. Memory Card im Regal ROOM1010 (add_card.bmp, Marke (272,157))
5. Schrot-Munition auf dem rechten Aussenluefter ROOM1090 (Shotgun.bmp, Marke (107,133))
6. RE2-Karten-Weltmodelle (World Items) extrahieren und ablegen

## Protokoll

### Punkt 2 — Codes in den eigenen Dokumenten gruen

**Messung vorher.** Die eigenen Dokumente sind GERASTERTE 4bpp-Seiten im RE2-FILE-Format
(`shared_assets/RE2/FILES/FILE25..29_*.TIM`, gesetzt mit `tools/re2_doc_satz.py` +
`tools/r34n_e/doc_satz_brief.py`); der Port zeigt sie mit ihrer EIGENEN CLUT
(`engine/src/re2doc_common.c` re15_re2doc_pixel: CLUT-Eintrag des Seiten-TIMs).
Codes stehen nur in zwei Texten (grep Ziffern in `analysis/befunde_runde34_nacht/E_texte/*.txt`
und `analysis/befunde_runde30/irons_diary_en.txt`): dok3 Marvin "4312" (FILE28), dok4 Armory
"5632" (FILE29). Irons Diary = nur Datumsangaben, Elliot "Only 3 more hours" = kein Code.
Index-Zensus ueber alle 191 RE2-Textseiten (Skript scratchpad clut_zensus.py): Pixel nutzen nur
Index 0 (Papier), 1..6 (Kern-Grauverlauf), 8 (Kontur), Index 15 genau 1 Pixel; **9..14 = 0 Pixel**.
RE2 selbst faerbt in seinen Dokumentseiten also nichts — "diesem Gruen" ist das Gruen der
Spieltexte, in denen das Original die Codes faerbt.

**RE-Beleg (Original-Codes sind gruen).** Die zwei Original-Nachrichten mit den Codes:
- ROOM1110.RDT @0x0DB0: `61 05 01 10 0f 0d 0e 05 00` = ":" Farbe 1, "4312", Farbe 0
- ROOM1230.RDT @0x172F: `61 05 01 11 12 0f 0e 05 00` = ":" Farbe 1, "5632", Farbe 0
Steuerbyte 0x05 im Textmaler FUN_80028868 (PSX.EXE, selbst disassembliert):
```
8002896c  lbu  v0,0(a2)        ; Argument N
80028974  andi v1,v0,0x4
80028978  sltu v1,zero,v1      ; (N&4)!=0
8002897c  andi v0,v0,0x3
80028980  sll  v0,v0,1         ; (N&3)*2
80028984  addiu v1,v1,480
80028988  addu v0,v0,v1
8002898c  sll  v0,v0,6
80028994  ori  s5,v0,0x10      ; CLUT = (x 256, y 480+(N&3)*2+((N&4)!=0))
```
N=1 -> CLUT-Zeile y 482 = Zeile 2 des CLUT-Blocks von DATA/TEX.TIM (Kopf: CLUT x 256 y 480,
32x24). Rohbytes TEX.TIM @0x94 (Zeile 2, Eintraege 0..6):
`00 00 e0 16 80 12 20 0e e0 09 80 09 40 05` -> 1..6 = (0,184,40) (0,160,32) (0,136,24)
(0,120,16) (0,96,16) (0,80,8).
Zeile 0 (weiss, @0x14) Eintraege 1..6 = `7b 67 39 5f b5 4e 31 42 ce 35 4a 29` =
(216,216,200)..(80,80,80) — **RGB-gleich** dem Kern-Grauverlauf 1..6 der RE2-Dokumentseiten
(Zensus oben, CLUT[1..6] auf allen 191 Seiten gleich). Dieselbe Rampe, also bildet Index i des
Dokument-Kerns 1:1 auf Eintrag i der gruenen Zeile ab — keine Schaetzung noetig.

**Entscheidung (Umsetzung).** Satz-Werkzeug `doc_satz_brief.py` bekommt `--gruen TOKEN`:
die Kernpixel der Glyphen des Tokens bekommen Index v+8 (9..14), die CLUT-Eintraege 9..14 der
Seite = TEX.TIM Zeile 2 Eintraege 1..6 (gelesen aus der Datei, kein Zahlenwert im Werkzeug).
Kontur (Index 8) bleibt die der RE2-Seite (PORT-WAHL: die Dokumentkontur (24,24,32) ist nicht die
des Textmalers (56,48,72), es gibt kein Original-Gegenstueck fuer eine gruene Dokumentkontur).

**Umsetzung Punkt 2.**
- `re15_port/tools/r34n_e/doc_satz_brief.py`: `--gruen TOKEN` (Optional 5 im Kopf): Kernpixel
  1..6 der Token-Glyphen -> 9..14; CLUT 9..14 = TEX.TIM Zeile 2 Eintraege 1..6 (`tex_zeile(1)`,
  Formel @0x80028974-94), Pruefung Vorlage-CLUT[1..6] == TEX.TIM Zeile 0 [1..6] (sonst Abbruch);
  Kontur mit Kern 1..6+9..14; nur Seiten MIT Token bekommen die erweiterte CLUT.
- `re15_port/tools/r34n_e/satz_bauen.sh`: FILE28 `--gruen 4312`, FILE29 `--gruen 5632`;
  Soll-Liste jetzt `analysis/befunde_runde35/F_belege/satz_md5.txt` (FILE26/27-Zeilen unveraendert
  aus der r34-Liste). Lauf: alle vier "gleich der md5-Liste" (reproduzierbar).
- Neue Assets (Paket-/Android-Gate, GROESSENGLEICH -> Gate-Pin noetig!):
  `re15_port/shared_assets/RE2/FILES/FILE28_p01_page.TIM` (md5 ad9d0f6f..., vorher 69268c01),
  `re15_port/shared_assets/RE2/FILES/FILE29_p01_page.TIM` (md5 f39ce419..., vorher e9c2c39d).
- Pixel-Differenz alt/neu (gemessen): FILE28_p01 82 Pixel, nur x 147..179 y 100..107, Paare
  (1,9)..(6,14); FILE29_p01 104 Pixel, x 5..36 y 148..155; CLUT 0..8 und 15 unveraendert.
  Bericht: FILE28 "p01 Zeile 6 (y 96..111): '4312' Kern-x 147..179 gruen", FILE29 "p01 Zeile 9
  (y 144..159): '5632' Kern-x 5..36 gruen".
