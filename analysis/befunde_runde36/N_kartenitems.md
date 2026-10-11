# Runde 36 Spur N — Karten-Items (RE2-Lageplan als aufsammelbare Etagenkarte)

Baum `.claude/worktrees/r36_kartenitems`, Zweig `r36/kartenitems`, Basis master 78a4a748.
Dossier wird fortlaufend geschrieben (Sitzungslimit).

## Auftrag (woertlich, AUFTRAG.md Z. 73-77)

> - Extrahiere mir Karte01 oder Karte02 von deinen Karten hier: "RE2-Weltmodelle: Öffnen Sie
>   extracted_re2_items/uebersicht.html im Browser. Oben stehen die Schlüsselkarten und die drei
>   Lageplan-Karten zum Aufsammeln.". Hänge sie mir beispielhaft an die Stelle von map3F.bmp, und schalte
>   damit die Map für Etage 3F frei, wie das Kartensystem in Resident Evil 2 auch funktioniert. Auch mit
>   den Text.
> - Bei Room 10D0 möchte ich das du die map für 2F da platzierst wo steht map2F.bmp.
> - Bei Room 1030 möchte ich das du die map für 1F da platzierst wo steht map1F.bmp
> - Map B2 soll im Room 1200 sein bei mapB2.bmp.
> - Map B1 soll im Room 11D0 sein bei mapB1.bmp

Zuteilung (VERTRAG.md): Bank-9-Bits 87 = 3F, 88 = 2F, 89 = 1F, 90 = B2, 91 = B1; Ereignis 28;
Nachrichten-IDs je Raum 2 ab 28.

## 1. Messung vorher

### 1.1 ⛔ map3F.bmp ist ROOM1140, NICHT ROOM1150 (Annahme in AUFTRAG.md widerlegt)

AUFTRAG.md Z. 82 nimmt fuer map3F.bmp "ROOM1150 (Irons' Buero ...)" an. Der Nutzer hat fuer 3F keinen
Raum genannt ("an die Stelle von map3F.bmp"). Gemessen mit dem Engine-Dekoder (probe_bg_dump, alle
428 STAGE1-Hintergruende) + `re15_port/tools/r34n_e/marken.py`-Verfahren (mittlere Abweichung je Pixel
ausserhalb der Marke), ueber ALLE STAGE1-Cuts:

| Marke | bester Cut | Abweichung | Zweitbester |
|---|---|---|---|
| map3F.bmp | **ROOM1140 Cut 5** | **8,5** | ROOM1230 Cut 2: 75,2 (alle ROOM1150-Cuts >= 113,8) |
| map2F.bmp | ROOM10D0 Cut 6 | 2,0 | ROOM1210 Cut 1: 70,1 |
| map1F.bmp | ROOM1030 Cut 3 | 12,8 | ROOM1030 Cut 10: 15,1 (dieselbe Kamera, Matrix identisch) |
| mapB1.bmp | ROOM11D0 Cut 0 | 1,3 | ROOM10A0 Cut 0: 57,1 |
| mapB2.bmp | ROOM1200 Cut 2 | 3,7 | ROOM11E0 Cut 9: 54,0 |

ROOM1140 (Konferenzraum, Blatt 4 = 3F laut `re15_map_zones.h` Zeile `{ 0x1140, ..., 4, 6, ...}`) —
die Marke sitzt auf der Pinnwand-Tafel (Staffelei) rechts neben der Tuer zum Gang. Etage passt (3F).
Marken-Huellen (Pixel, gemessen): 3F (231..241, 72..81), 2F (270..289, 120..135), 1F (155..164,
103..113), B1 (227..250, 65..93), B2 (58..74, 80..93).

### 1.2 Kartenblaetter der fuenf Raeume (re15_map_zones.h, Spalte Blatt)
1140 -> 4 (3F), 10D0 -> 3 (2F), 1030 -> 2 (1F), 1200 -> 1 (B2), 11D0 -> 0 (B1)
(Blatt-Reihenfolge menu_common.c: "Seite 1 = B2, 0 = B1, 2 = 1F, 3 = 2F, 4 = 3F").

## 2. RE-Belege

### 2.1 RE2: Aufnahme eines Lageplans = Raum-Ereignis, kein Inventar-Item
RE2 room20B0 sub10 (Bytes aus dem RDT, `tools/re2_sicherung/re2_scd_walk.py`-Walker, RDT 0x037BA..0x0381A):
```
0x037BA 2b 00 00 00 00 ff          Message_on msg 0 "A police station map. Will you take it?" (Ja/Nein)
0x037C0 02 00                      Evt_next, Nop
0x037C2 06 00 44 00                Ifel_ck
0x037C6 21 0b 1f 00                Ck(11,31,0) = Ja
0x037CA 36 04 06 01 04 08 00..     Se_on(4,6)  Bestaetigungston
0x037D6 22 08 53 01                Set(8,0x53)   Weltobjekt genommen
0x037DA 22 21 02 01 .. 0x037E2     Set(33,2..4)  Karten-BESITZ 1F/2F/3F
0x037E6 22 23 02 01 .. 0x037EE     Set(35,2..4)  Etagen-WAHL 1F/2F/3F
0x037F2 2e 04 08                   Work_set(Objekt 8)
0x037F6 32 00 20 4e 20 4e 20 4e    Pos_set(20000,20000,20000) = Modell weg
0x037FE 02 00                      Evt_next, Nop
0x03800 2b 00 11 00 00 ff          Message_on msg 17 "You've taken a police station map."
0x03806 07 00 12 00                Else_ck
0x0380A 36 04 05 01 04 08 00..     Se_on(4,5)  Abbruchton
0x03818 01 00                      Evt_end
```
B1-Plan room2130 sub05 analog: Set(33,5) + Set(35,5) + Set(4,0x7A) (lageplaene.csv).
Flag-Banken (RE2 PSX.EXE, Zeigertabelle PTR_DAT_800a78c8, Index*4): Bank 9 -> 0x800D490C (Besucht),
Bank 32 -> 0x800D4920, **Bank 33 -> 0x800D4924** (`800a794c 24 49 0d 80`), Bank 34 -> 0x800D48DC,
**Bank 35 -> 0x800D4908** (`800a7954 08 49 0d 80`).

### 2.2 RE2-Kartenzeichner FUN_8006E120 (selbst disassembliert, ghidra_re2_Leon.txt)
```
8006e660 3d aa 25 90  lbu   a1,-0x55c3(at)   ; DAT_800aaa3d[blatt*8] = Karten-Id des Blatts
8006e668 24 49 84 24  addiu a0,a0,0x4924     ; Bank 33 (Besitz)
8006e66c d8 dc 01 0c  jal   FUN_80077360     ; Bit-Test
8006e674 2e 00 40 10  beq   v0,zero,0x8006e730 ; ohne Plan -> nur Besuchte
8006e688 ... jal FUN_80077360 / addiu a0,a3,-0x18 (=0x800D490C Bank 9 Besucht)
8006e690 2e 00 40 14  bne   v0,zero,0x8006e74c ; besucht -> normale Zeile
8006e71c f2 01 15 24  li    s5,0x1f2         ; UNBESUCHT mit Plan: CLUT-Zeile 498
8006e72c f7 01 15 24  li    s5,0x1f7         ; (Bank-32-Variante 503)
8006e744 08 00 40 10  beq   v0,zero,0x8006e768 ; OHNE Plan + unbesucht -> nicht zeichnen
8006e750 0a 3e 02 0c  jal   GetClut(0x100,s5)
```
Etagenwahl (FUN um 0x8006D8F0): `8006d920 lbu a1,-0x6514(at)` (Nachbar-Etage oben), `8006d934 jal
FUN_80077360` mit `8006d938 addiu a0,a0,0x4d94` (= 0x800D4908, Bank 35) -> nur gesetzte Etagen waehlbar;
unten `8006d994 addiu a0,a0,0x4908` / `8006d998 jal FUN_80077360`.
Port hat das Vier-Zustands-System bereits (re15_inv_screen.c Besitz-Gatter `re15_map_owned_page`,
CLUT RE15_INV_CLUT_MAP_UNBESUCHT = Zeile 498) — es fehlte nur die FUNDSTELLE fuer die Polizei-Blaetter
(re15_map_owned.c: einzige Fundstelle ROOM5030 Laborplan Flag(3,115)).

### 2.3 RE1.5 hat dieselbe Aufnahme-FORM (Laborplan ROOM5030 sub02, scd_dump_room.py)
```
0x010AC 2b 00 ff ff   Message_on 0 "A map of the lab is available here. Will you file the laboratory map?"
0x010B0 02 00         Evt_next + Nop
0x010B2 06 00 1e 00   Ifel_ck (Blockende 0x010D4)
0x010B6 21 0c 1f 00   Ck(12,31,0) = Ja  (RE1.5 Ja/Nein-Bit; port msg_common.c Fall 3)
0x010BA 46 05 00..    Aot_reset(5, sce 0)
0x010C4 09 0a 01 00   Sleep 1
0x010C8 22 03 73 01   Set(3,115,1)
0x010CC 2b 01 ff ff   Message_on 1 "You've taken the laboratory map."
0x010D0 02 00 / 0x010D2 08 00 / 0x010D4 01 00
```
Ausloeser sub00 @0x0108A: `Aot_set` Slot 5, sce 3 (EVENT), sat 0x31(+0x80 Viereck), p0 0x00FF,
p1 0x0218 (= sub 2). Text-Rahmen RE1.5 (ROOM2030 msg 2/3 "Will you take the Fuse?" / "You've
taken the Fuse."): `04 02` .. `05 01` NAME `05 00` `1b` `03 02 01 00` bzw. `57 01 00`.

### 2.4 Texte (RE2-Wortlaut, RDT-Bytes)
room2130 sub00 `fa 02 1d 00 f9 01 "police B1 map" f9 00 01 fc "Will you take it" 1b fb 40 fe 00`
= "A police B1 map. / Will you take it?"; sub12 `fa 02 "You've taken the" 00 fc f9 01 "police B1 map"
f9 00 01 fe 00` = "You've taken the / police B1 map.".
RE2-Steuercodes (Message-FSM ab 0x800303C4, selbst disassembliert): 0xFB -> Zustand 4 Auswahl
(@0x800304b8), 0xF9 nn -> Farbe (@0x80030564), 0xFA -> Start/Tempo (@0x800304d0), 0xFD -> Seite
(@0x8003047c), 0xFE -> Ende (@0x80030460), 0xFC -> FSM ueberspringt 1 Byte (@0x8003044c `slti 0xfd`
-> 0x8003055c) = Zeilenumbruch des Renderers. RE1.5-Gegenstuecke (port msg_common.c): 04 = Tempo,
05 = Farbe, 08 = Zeilenumbruch, 03 = Auswahl, 01 = Ende.

## 3. Weltmodell und Lage (in Arbeit)
Modell: karte01 (md5 99dae133, RE2 ROOM3060.RDT MD1 @0x0045C8, TIM @0x02C6D4; lageplaene.csv) —
Flaeche 1080 x 720 (Modell-z x Modell-x) auf y = -85, 4 Dreiecke, UV 127 x 85 Texel, 1 CLUT.
karte02 hat dieselbe Geometrie/Kartenkunst, aber 2 CLUTs + fremde Kunst im TIM -> karte01 gewaehlt.

Ebenen je Raum (Messung, Skripte scratchpad geom.py/place.py, Kameras aus `probe_r34n_e_dokumente kamera`):
* 1140 Cut 5: Tafel-Ebene z = -8710 (rechte Tafelkante aus Cut 4 + Cut 5 trianguliert:
  (-1249,-3482,-8764) Fehlabstand 58, (-1169,-1323,-8655) Fehlabstand 45), parallel zur Rueckwand.
* 10D0 Cut 6: Wand z = 21624 (Boden-Wand-Kante zurueckprojiziert auf y=0: (5113,21738), (2649,21644),
  (1187,21588); SCA-Grenze 21888 liegt 240 davor).
* 1030 Cut 3: Rueckwand z = -23580 (Bodenkante Bildzeile 151,5 in Spalten 130..170).
* 11D0 Cut 0: rechte Wand SCHRAEG (19,8 Grad gegen z; Bodenkante 4 Punkte, Ziegelfugen-Steigung ~1,0
  bestaetigt ~17,5 Grad).
* 1200 Cut 2: Ebene der Glasvitrinen-Front x = -26330 (Ecken aus Cut 1 + Cut 2 trianguliert,
  Fehlabstand 4 / 53); die Marke liegt rechts neben der Vitrine (z -16051..-15274).

