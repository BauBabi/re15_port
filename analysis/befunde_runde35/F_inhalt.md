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

### Punkt 6 — RE2-Welt-Item-Modelle extrahiert und abgelegt

**Wortlaut** "die Karten Modelle in der Welt zum Einsammeln, also die World items, aus Resident
Evil 2 extrahierst und irgendwo ablegst wo ich es sehen kann". Abgelegt sind ALLE RE2-Welt-Items
(Leon, info/re2leon/PL0/RDT, 495 RDTs) — "die World items" — mit eigenem Karten-Bogen fuer die
einsammelbaren Karten (Card Keys).

**RE-Beleg (Mechanismus, RE2 PSX.EXE, Belegkette aus re2_doc_worldmodels.py, Commit 2b95b592):**
Item_aot_set Op 0x4E -> LAB_80054CD4 (Tabelleneintrag @0x800A7600); Record 22 Byte, +18 flag
(`lhu a1,0x12(s0)` @0x80054CF4), +20 md1 (`lbu s2,0x14(s0)` @0x80054CF8); md1 < 32
(`sltiu v0,s2,0x20` @0x80054D98) = Slot der Raum-Modelltabelle (Lader FUN_80052D14: nOmodel
`lbu s2,0x2(v0)` @0x80052D70, Tabelle `lw s4,0x30(v0)` @0x80052D74, Schritt 8 @0x80052DF4);
md1 = 255 = kein Weltmodell. Das Modell haengt an der PLATZIERUNG, nicht an der Item-Id.

**Umsetzung.** Neues Werkzeug `tools/re2_sicherung/re2_item_worldmodels.py` (verallgemeinert
re2_doc_worldmodels.py von Dokumenten auf alle Ids; Op 0x69 4-Punkt-Zonen mit erstem Punkt als
Lage). `tools/re2_sicherung/re2_scd_walk.py`: `RE15_INFO_DIR`-Ausweichpfad fuer
information293.txt (schlanker Baum; nur gelesen aus dem Hauptbaum).
Aufruf: `RE15_INFO_DIR=C:/workspace/git/reAi_v2/info/Resident_Evil_und_Playstation_Information
C:/Python310/python.exe tools/re2_sicherung/re2_item_worldmodels.py` (18 s).

**Ergebnis (gemessen, `extracted_re2_items/_bericht.txt`):** 269 Item_aot_set-Platzierungen,
206 mit Weltmodell, 63 ohne (md1 255); **81 verschiedene Meshes** (md5 der MD1-Bytes), 63 Item-Ids
mit Weltmodell. SCD 2553 Bloecke, 45 desynchron (94,55 % der Bytes gewalkt — dieselbe Dunkelziffer
wie re2_doc_worldmodels). Karten: mesh043 Blue Card Key (room10B0 main00 @0x01714), mesh066/067 Lab
Card Key (room60A0 sub18 @0x014B4, room6150 sub00 @0x02CFE); Red Card Key (room2150 @0x012A8) hat
md1 255 = kein Weltobjekt. 8 Platzierungen mit md1 >= nOmodel (im Bericht einzeln, z.B. room1000
sub01 Id 6 md1 7 bei nOmodel 0) — der Pool-Slot kommt dort nicht aus der Raumtabelle.
**Fuer den Nutzer:** `extracted_re2_items/uebersicht.html` (Galerie, zwei Ansichten je Mesh, Namen,
Ids, Raeume), `kontaktbogen.png` (alle 81), `karten.png` (die 3 Card-Key-Meshes), `katalog.csv`
(Item-Id; Name; Karte; Meshes; Bilddateien; Platzierungen), `platzierungen.csv`,
`ohne_modell.csv`, `modelle/meshNNN_<md5>.{md1,tim,obj,_a.png,_b.png}`.
Lageplaene ("map") sind in RE2 KEINE Items: die Namentabelle 0x00..0x8B fuehrt 0 Maps (3 Card
Keys); die 7 Lageplan-Funde ("police station map" ROOM20B0 @0x3BA6, "police B1 map" ROOM2130
@0x1B41, "sewage disposal map" ROOM3060 @0x2B4E, "sewer map" ROOM4040 @0x1BAE, "factory map"
ROOM5040 @0x2357 / ROOM5060 @0x110C, "laboratory map" ROOM6120 @0x18B1) laufen ueber
Nachricht+Ja/Nein-Ereignis, nicht ueber Item_aot_set (OFFEN unten, falls der Nutzer DIESE meinte).

**Messung nachher Punkt 2 (echte exe, beschleunigter Renderer, RE15_FRAMEDUMP; gdigrab liefert in
dieser Sitzung weisse Bilder).** Kopie `re15_pc_r35f_doc.exe` neben der exe, Arbeitsverzeichnis mit
`analysis/befunde_runde30/nutzer_marken/re15_card_nutzer_2026-09-27.mcr`, `RE15_CONTINUE_TEST=1
RE15_CARD_AUTO=1 RE15_DOC=28|29 RE15_PAD_AT=400:S,520:M,580:A,600:A,700:R` (Muster
tools/r34n_e/leser_lauf.sh, jetzt ohne Umbenennung, die Tabelle kennt FILE28/29), Framedump F780 =
Seite 2/3: `F_belege/p2_exe_FILE28_p01.png` ("The new code is: 4312", 4312 gruen),
`F_belege/p2_exe_FILE29_p01.png` ("...with the code: 5632", 5632 gruen). Restlicher Text weiss.
**Test:** `unit_r35_inhalt_doku` 19/19 (A Gruen = TEX.TIM Zeile 2 aus der Datei; B je Code 82 bzw.
104 gruene Pixel, alle im Code-Kasten, ganze Breite, 0 weisse Kernpixel im Kasten; C FILE25..29
sonst 0 gruene Pixel). Gegenprobe: mit den alten Seiten faellt B (0 gruene Pixel).
