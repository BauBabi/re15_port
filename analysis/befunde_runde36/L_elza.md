# Runde 36 — Spur L "elza" (Dossier, fortlaufend)

Baum: `.claude/worktrees/r36_elza`, Zweig `r36/elza`, Basis master 78a4a748.
Auftrag (AUFTRAG.md, woertlich, Spur-L-Punkte):

1. "Bei Elzas Szenario soll die Scheibe in Room 1120 die bei Leons Szenario zum schluss zerspringt in Stage1,
   bereits immer kaputt sein, und eine Kraehe mehr im Raum."
2. "Bei Elzas Szenario soll Irons immer bereits von anfang an tot da liegen. Wenn sie ihn anklickt soll da stehen
   "He has already passed..."."
3. "In ROOM 1211 in Elzas Cutscene wenn sie Kendo befreit fehlt noch der Unlock sound bei der Tuer wenn sie sie
   aufschliesst."
4. "In Elzas Cutscene in Room 1021 wo Kendo Roy erschiesst fehlen die Schuss Sounds."
5. "In Elzas Cutscene in Room 1021 wo Kendo Roy erschiesst fehlt noch ein Sterbe/Leid/Hinfall sound von Roy,
   sobald er erschossen wurde."
6. "Ich habe fuer Elzas Szenario Synchron Dateien hinzugefuegt. In Room 1101 - waehrend Roy bewusstlos wird -
   moechte ich das du das Voiceover main 16.5 abspielst. Auch wenn es aktuell keinen Text dafuer gibt. Erstelle
   gerne ein Untertitel dafuer, wie bei den anderen auch."

Zuteilung (VERTRAG.md): Bank-9-Bit 86 (Reserve), Nachrichten-IDs ROOM1151 20..23, ROOM1101 20..21,
ROOM1121/1021/1211 nur falls noetig ab RDT-Zahl (+3), Ereignis 26. Eigentum: fenster_1120.c (Elza-Zweig),
Irons-Texte 1151.

## 0. Stand / Fortschritt

- [x] Grundlagen (Variante, Werkzeuge, Baseline-Bau 78a4a748 gruen gebaut)
- [ ] P1 Fenster 1121 kaputt + Kraehe
- [ ] P2 Irons 1151 tot + Text
- [ ] P3 1211 Unlock-Ton
- [ ] P4 1021 Schuss-Toene
- [ ] P5 1021 Roy Sterbe-Ton
- [ ] P6 main16.5 + Untertitel

## 1. Grundlagen

### 1.1 Wie der Raumlader die Elza-Variante waehlt (Beleg)
* TITLE.BIN Auswahlschirm spiegelt die Wahl in DAT_800ACA3C Bit 31: Elza `lui v1,0x8000` @0x801024a8 +
  `or` @0x801024e4, Store `sw v0,-13764(at)` @0x801024ec (Port: include/re15_gameflow.h re15_char_variant()).
* Raumlader FUN_800396fc: Basisindex aus der Stage-Tabelle (@0x800397cc/@0x800397e0), Elza-Bit
  `srl a0,a0,31` @0x800397e4, `addu a0,a0,v0` @0x800397ec -> CD-Datei = Leon-RDT + 1. Im Port ist die Variante die
  niedrigste Hex-Ziffer der Raum-Id (ROOM1121/1151/1211/1021/1011 werden NUR in einem Elza-Spiel geladen).
* Gemessen (Lauf m1, Elza per RE15_PSELECT_AUTO_SWITCH): debug.log `[pl] Spieler-Familie PL04 (character=4,
  Elza-Bit=1)`, `[goto] queued room change -> ROOM1121`.
* ROOM1120.RDT == ROOM1121.RDT (cmp bytegleich); Hintergrund ROOM112.BSS ist variantenlos (eine Datei).

### 1.2 Werkzeuge dieser Spur
* `re15_port/tools/r36_l/re2_raum_se_zensus.py <satz-hex>...` — Zensus eines RE2-Raumbank-Satzes (Se_on Bank 2)
  ueber alle 495 RE2-Leon-RDTs (Wellen-Familien nach sha1).
* Messlaeufe: Kopie `re15_pc_r36l.exe` neben der exe, Arbeitsordner im Scratchpad (nie im Baum).

## 2. Messung vorher (Stand 78a4a748)

* m1 ROOM1121 (Elza, RE15_FORCE_CUT=1, Bild 90): Hintergrund Cut 1 mit UNBESCHAEDIGTEM Fenster; 3 Kraehen
  (`[spawn-diag] Sce_em_set type=0x21` Slots 0..2 = main00 @0x00D0C/@0x00D20/@0x00D34, Kill-Flags 0xA7/0xA8/0xA9).
  fenster_1120.c ist Leon-only (RE15_FENSTER_RAUM 0x1120, `if (room_id != RE15_FENSTER_RAUM) return;`).
* m2 ROOM1151 (Elza): Irons Typ 0x45 Slot 0 @0x00E6A (-21150,-720,-26131), sub02 Plc_motion(0,3)+Plc_flg 4 =
  Liege-Atem-Loop (`[scd] Plc_motion(entity=0, motion=3 ...) -> slot=1`). Ansprechen = AOT-Slot 2 sce 5
  (@0x00D92) -> work_vars[0] = 2 -> sub01 @0x00E9E `23 00 00 00 02 00` + Ck(3,0x9D)==0 -> sub03 msg 2
  "Irons: I'll be fine. Just worry about / yourself for now." (@0x011C3).

