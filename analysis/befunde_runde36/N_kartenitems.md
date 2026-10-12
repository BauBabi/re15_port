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
Nachrichten-IDs je Raum 2 ab 28. ⛔ Seit dem Nachtrag kartenbits (Abschnitt 9): **B2 = 77, B1 = 78** — 90/91 sind
Original-Item-Taken-Bits (ROOM2090/2091 @0x00A12, ROOM2040/2041 @0x01364).

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

### 1.2 Port vorher (Stand 78a4a748, gebaut und gemessen)
* Keine Karte an den fuenf Stellen: Framedump Lade-Weg je Raum (Nutzer-Cut) = Hintergrund unter der Marke
  unveraendert (Laeufe "ohne" in 4.1 sind genau dieser Zustand).
* Kartenblatt: zeichnet nur besuchte Orte — ohne Plan je Etage 0 unbesuchte Kacheln (unit_r36_kartenitems K:
  im Kartenraum stehend je 1 Kachel); einzige Fundstelle des Ports war der Laborplan ROOM5030 (Flag 3,115),
  keine Polizei-Etage hatte einen Plan.
* Etagenwahl nur fuer besuchte Blaetter oder das Hinweis-Ziel (re15_map_blatt_waehlbar).

### 1.3 Kartenblaetter der fuenf Raeume (re15_map_zones.h, Spalte Blatt)
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

## 3. Umsetzung

### 3.1 Dateien
| Datei | Inhalt |
|---|---|
| `re15_port/engine/src/kartenitems_r36.c` + `include/re15_kartenitems_r36.h` | Installer (Prop + Zone, Attribut 0x0010), Port-Programm Ereignis 28, Texte 30/31, Besitz je Blatt |
| `re15_port/engine/src/gen/kartenitems_r36.inc` | ERZEUGT: RE2-TIM karte01 (unveraendert), 5 MD1 (nur Ecken skaliert), Texte, Lage-Tabelle |
| `re15_port/tools/r36_kartenitems/karten_export.py` | Generator (Quelle RE2 ROOM3060.RDT, md5-Pruefung, Kameras/Ebenen/Marken, Massstab) |
| Haken (je 1-3 Zeilen, "Runde 36 Spur N") | `scd_room_setup.c` (Installer am Blockende), `platform/pc/main.c` (Installer Boot-/CONTINUE-Weg, Prop-Lader MD1+TIM obj 15; seit Nachbesserung 1 der generische Flach-Zweig `attr & 0x12 == 0x10` im Prop-Zeichner statt eines Karten-Hakens), `scd_vm.c` (scd_event_fire-Kette; Obj_model_set speichert pc[8..9]), `re15_scd.h` (Prop-Feld `attr`), `re15_map_owned.c` (Besitz), `map_hint_common.c` (Etagenwahl) |
| Tests | `tests/unit/test_r36_kartenitems.c`, `tests/unit/probe_r36_kartenitems_bmp.c`, `tests/unit/probe_r36_kartenitems_licht.c` (Nachbesserung 1), `tests/integration/test_r36_kartenitems_bild.cmake`, registriert in `tests/unit/probes/r36_kartenitems.cmake` |

### 3.2 Modell
karte01 (md5 99dae133, RE2 `ROOM3060.RDT` MD1 @0x0045C8, TIM @0x02C6D4 md5 bf0fbcc7; = extracted_re2_items/
modelle/karte01_99dae133.*, lageplaene.csv): 4 Dreiecke um (0,-85,0), Flaeche 1080 (Modell-z) x 720 (Modell-x),
Normale (0,-4096,0) @MD1+0x6C, UV u 0..126 mit +z, v 0..84 mit +x (MD1+0xBC). karte02 hat dieselbe Geometrie und
Kartenkunst, aber 2 CLUTs und fremde Kunst im TIM -> karte01. Je Raum ein MD1: dieselben Bytes, nur die 5 Ecken
@0x44 mit s skaliert (Pruefung im Test: ausserhalb 0x44..0x6B kein Byte verschieden).

### 3.3 Lage, Massstab, Drehung (PORT-WAHL aus den Nutzer-Marken, erzeugt) — Licht: seit Nachbesserung 1 Abschnitt 8
Mitte = Sehstrahl der Markenmitte x Ebene. Massstab s = groesster Wert, bei dem die vier projizierten
Kartenecken im Nutzer-Cut in der Marken-Huelle liegen (Bildraum; das Ebenen-Mass war auf der schraegen Wand
11D0 28 % zu hoch: Framedump y 61..97 gegen Marke 65..93). Drehung: Modell +x -> Welt unten, +y -> in die
Wand, +z -> rechts; Euler nach pc_prop_rot_q12 (Ry*Rx*Rz). Ursprung = Mitte - 85*s*n. Zone 1000 x 1000 um
Mitte + 300*n (r35-Muster: haeufigste Item-Groesse 69 von 162).

| Raum (Blatt) | Cut | Ebene | Mitte | s -> Flaeche | rot (x,y,z) | Zone (Ecke) / Slot |
|---|---|---|---|---|---|---|
| 1140 (4 = 3F) | 5 | z = -8710 (Tafel) | (-2050,-2831,-8710) | 0,503 -> 543 x 362 | (0,1024,1024) | (-2551,-9510) / 40 |
| 10D0 (3 = 2F) | 6 | z = 21624 | (2133,-1928,21624) | 0,362 -> 391 x 260 | (0,3072,1024) | (1634,21424) / 40 |
| 1030 (2 = 1F) | 3 | z = -23580 | (-1691,-2769,-23580) | 0,677 -> 732 x 488 | (0,3072,1024) | (-2192,-23780) / 8 (Telefon -> 39) |
| 1200 (1 = B2) | 2 | x = -26330 (Vitrinenfront) | (-26330,-2572,-15682) | 0,690 -> 745 x 497 | (0,0,1024) | (-26530,-16183) / 40 |
| 11D0 (0 = B1) | 0 | schraeg, n (-0,941;0;0,338) | (1642,-2199,-19130) | 0,638 -> 689 x 459 | (2048,3871,3072) | (860,-19530) / 40 |
(Die frueher hier gefuehrte Spalte "Licht" (156,155,153)/(181,186,180)/(47,70,104)/(85,85,75)/(74,75,77) war
der Mangel M1 der Abnahme 0 und ist entfernt — Abschnitt 8.)

Ebenen je Raum (Messung, Kameras aus `probe_r34n_e_dokumente kamera`):
* 1140 Cut 5: Tafel (Staffelei) z = -8710 — rechte Tafelkante aus Cut 4 + Cut 5 trianguliert:
  (-1249,-3482,-8764) Fehlabstand 58, (-1169,-1323,-8655) Fehlabstand 45; parallel zur Rueckwand angenommen.
* 10D0 Cut 6: Wand z = 21624 — Bodenkante (y = 0) zurueckprojiziert: (5113,21738), (2649,21644), (1187,21588);
  die SCA-Grenze 21888 liegt 240 davor (Kollisionsrand).
* 1030 Cut 3: Rueckwand z = -23580 — Bodenkante Bildzeile 151,5 (Helligkeitssprung in den Spalten 130..170).
* 11D0 Cut 0: rechte Wand SCHRAEG, 19,8 Grad gegen z (Bodenkante 4 Punkte (2390,-17045) .. (1186,-20393);
  Ziegelfugen-Steigung ~1,0 im Strukturtensor bestaetigt ~17,5 Grad); die SCA-Grenze x = 1050 ist achsparallel.
* 1200 Cut 2: Vitrinenfront x = -26330 — obere Vitrinenecken aus Cut 1 + Cut 2 trianguliert (Fehlabstand 4/53)
  = SCA 6 (x -28256..-26256); die Marke liegt rechts neben der Vitrine (z -16051..-15274).

LICHT (Stand nach Nachbesserung 1, Abschnitt 8): die Karte traegt das Attribut-Wort 0x0010 (Pool+0x0c,
Flach-Bit wie die RE2-Wandplaene room20B0 @0x03158 / room2130 @0x00FDC und wie RE1.5 ROOM1200 obj 0/1
@0x007DA/@0x00804) und wird vom Prop-Zeichner wie im Original ohne Raumlicht gezeichnet (FUN_8002c18c
@0x8002c2f4 -> FUN_80022f0c @0x8002c320, Farbe obj+0x70 = 0x808080). Die urspruengliche Fassung dieser
Spur hatte hier eine Tint-Konstante je Raum aus dem Hintergrundbild ("hellste 15 %") erfunden — Mangel M1
der Abnahme 0, behoben in Abschnitt 8; das RDT-Figurenlicht liefert an allen fuenf Stellen 31..70 von 128,
mit NCCT waere die Karte ein fast schwarzes Rechteck (1140 Cut 5: Karte 32/32/29 gegen Tafel 91/83/74).

### 3.4 Aufnahme (Port-Programm, Ereignis 28)
```
+00 2b 1e ff ff        Message_on 30 Frage          ROOM5030 sub02 @0x010AC
+04 02 00              Evt_next + Nop                @0x010B0
+06 06 00 2c 00        Ifel_ck (Ende +0x36)          @0x010B2 (Laengen-Regel 0x010B2+4+0x1E = 0x010D4)
+0A 21 0c 1f 00        Ck(12,31,0) = Ja              @0x010B6
+0E 46 SS 00 ..        Aot_reset(Slot,0)             @0x010BA
+18 09 0a 01 00        Sleep 1                       @0x010C4
+1C 22 09 BB 01        Set(9,Bit,1)                  @0x010C8 (Rolle); RE2 Set(33,n) @0x037DA + Set(35,n) @0x037E6
+20 2e 03 0f / 00      Work_set(Objekt 15) + Nop     RE2 @0x037F2/@0x037F5 (RE1.5-Art 3 = Objekt, scd_work_bind)
+24 32 00 20 4e x3     Pos_set(20000,20000,20000)    RE2 @0x037F6
+2C 02 00              Evt_next + Nop                RE2 @0x037FE
+2E 2b 1f ff ff        Message_on 31 Meldung         @0x010CC / RE2 @0x03800
+32 02 00 / 08 00 / 01 00                           @0x010D0 / @0x010D2 / @0x010D4
```
Kein Se_on: der Ja/Nein-Kasten des Ports spielt bei der Bestaetigung bereits Kern-SE 6 (Ja) / 5 (Nein)
(msg_common.c Fall 3, RE2 @0x80030944/@0x8003093c) — dieselben Toene, die RE2 room20B0 im Skript spielt
(`36 04 06 ..` @0x037CA / `36 04 05 ..` @0x0380A); ein zweites Se_on wuerde doppelt toenen.
Zone: EVENT wie ROOM5030 sub00 @0x0108A (sce 3, sat 0x31, p0 0x00FF, p1 = 28<<8|0x18), Band 0.
Texte 30/31 beim Ausloesen eingesetzt (Muster rolltor_1050.c), RE2-Wortlaut aus room2130 sub00/sub12 mit
RE1.5-Rahmen ROOM2030 msg 2/3 (`04 02`, Name gruen `05 01`..`05 00`, `57`, `08` = RE2 `FC` Zeilenumbruch,
Ende `1b 03 02 01 00` bzw. `57 01 00`). Etage im Namen ("police 3F map") = PORT-WAHL nach RE2s "police B1 map".

ROOM1030/1031 — VORRANG: Slot 8 ist das Muenztelefon (main00 Aot_set @0x01D2E / @0x01D9C, sce 1, Rechteck
4096 x 4096 ueber der ganzen Rueckwand). Der Aktions-Scan feuert je Druck den KLEINSTEN treffenden Slot
(aot_common.c "one action handler per press", FUN_80042bac); in einem hohen Slot waere die Karte dort nie
erreichbar. Der Telefon-Satz zieht deshalb unveraendert in Slot 39 um, die Karte nimmt Slot 8; nach der
Aufnahme (Aot_reset(8,0)) antwortet das Telefon wieder ueberall, beim naechsten Raumstart steht es wie
ausgeliefert in Slot 8 (Bit gesetzt -> kein Umzug). Census: kein RDT-Skript von 1030/1031 spricht Slot 8 nach
main00 an (Aot_reset nur Slots 4, 6, 17).

### 3.5 Karte und Etagenwahl
`re15_map_owned_page/bits` (Port-Gegenstueck zu RE2 Bank 33 @0x8006E66C) fragt zusaetzlich die Bits 87..91
(seit Abschnitt 9: 87/88/89/77/78)
(nicht in `s_map_funde`, deren Riegel test_karte_besitz nur RDT-belegte Fundstellen zulaesst).
`re15_map_blatt_waehlbar` = bekannt || Hinweis-Ziel || BESITZ (RE2 setzt Bank 35 bei der Aufnahme:
room20B0 Set(35,2..4) @0x037E6-EE, gelesen @0x8006D934/@0x8006D998).
Nebenwirkung (beabsichtigt, RE2-gleich): auch der Laborplan ROOM5030 (Flag 3,115, Blaetter 9/10/11) macht seine
Blaetter jetzt waehlbar — RE2 setzt beim Laborplan ebenfalls Bank 35 (room6120 Set(35,0x0D..0x11), lageplaene.csv).

## 4. Messung nachher

### 4.1 Framedumps (Lade-Weg CONTINUE, RE15_FORCE_CUT = Nutzer-Cut, RE15_WINDOW_SCALE=1)
Zwei Laeufe je Raum, einziger Unterschied das Bit; `probe_r34n_e_bild` (|dR|+|dG|+|dB| > 30):

| Raum | Pixel im Fenster | Huelle der Aenderung | Nutzer-Marke | ausserhalb |
|---|---|---|---|---|
| 1140 Cut 5 | 73 | x230..241 y72..80 | x231..241 y72..81 | 0 |
| 10D0 Cut 6 | 219 | x270..289 y120..134 | x270..289 y120..135 | 0 |
| 1030 Cut 3 | 81 | x154..164 y104..111 | x155..164 y103..113 | 0 |
| 1200 Cut 2 | 177 | x57..73 y79..92 | x58..74 y80..93 | 0 |
| 11D0 Cut 0 | 169 | x233..245 y65..93 | x227..250 y65..93 | 0 |

### 4.2 Echter Weg (exe, ROOM1140, RE15_INPUT_SCRIPT)
Spieler (-2050,-9900) Blick zur Tafel: Aktion -> "A police 3F map. / Will you take it? > Yes No" (Name gruen)
-> Ja -> "You've taken the / police 3F map." (Framedumps F100/F140/F220); debug.log
"[kartenitems] ROOM1140 Ereignis 28 -> Port-Programm Karte Blatt 4". Danach Statusschirm + L1: Blatt
"POLICE STATION 3F" zeigt ALLE 3F-Raeume (unbesuchte als Umriss in RE2-Zeile 498), ohne Aufnahme nur den
eigenen Raum — 537 Pixel Unterschied, Huelle x120..193 y80..158.

### 4.3 Erreichbarkeit (probe_r34n_e_dokumente abdeckung, Raster 100, 64 Blickrichtungen)
1140: 92 Standorte / 870 Treffer, 0 abgefangen; 10D0: 139 / 1530, 0; 1200: 183 / 2009, 672 von Slot 2
(Vitrinen-Ereignis, Ueberlapp z -16183..-15800 — davor gewinnt die Vitrine, rechts davon die Karte);
11D0: 142 / 1407, 0; 1030: alle 5760 Treffer landen in Slot 8 = der Karte.

### 4.4 Kartenschirm je Etage (exe, Lade-Weg, Statusschirm + L1, RE15_INV_FB_SHOT inv-Bild 140)
Je Raum derselbe Lauf mit/ohne Plan-Bit (integration_r36_kartenitems_bild Teil 1c), probe_r36_kartenitems_bmp im
Kartenfeld (20..300, 50..215): 3F 537, 2F 623, 1F 905, B2 763, B1 866 Pixel mehr MIT Plan, ausserhalb je 0.
Bild: mit Plan ist die ganze Etage gezeichnet (unbesuchte Raeume als Umriss = RE2-Zeile 498, der eigene Raum
gefuellt), ohne Plan nur der eigene Raum — fuer alle fuenf Blaetter (3F/2F/1F/B2/B1).

## 5. Tests

| Test | misst | Ergebnis |
|---|---|---|
| `unit_r36_kartenitems` | T Tabelle (Blatt = Zonen-Blatt, Bits 87..91, MD1/TIM parsen, MD1 nur Ecken verschieden, Texte = RE2-Wortlaut, Frage mit 0x03); R Raumstart aller 10 Raeume (Leon+Elza): Prop obj 15 an Lage/Drehung, EVENT-Satz Ereignis 28 sat 0x31, 1030/1031 Telefon -> Slot 39, Installer idempotent; G Bit gesetzt -> nichts, Slot 8 = Telefon; D echter Bild-Ablauf (re15_game_step) 1140 Ja, 1140 Nein, 10D1 (Elza) Ja, 1030 Ja: Frage 30 -> Bit/Zone weg/Modell (20000,..)/Meldung 31/Blatt besitzt+waehlbar bzw. nichts; 1030 danach an derselben Stelle Telefon-Text (Nachricht 3 aus Slot 39); K Kartenblatt je Etage ohne Plan 0 unbesuchte Kacheln, mit Plan Zeile 498 (+6/+10/+10/+9/+6 Kacheln), fremdes Blatt erst mit SEINEM Plan waehlbar; S Spielstand-Rundlauf | 201 Pruefungen gruen |
| `integration_r36_kartenitems_bild` | echte exe, 12 Laeufe: je Raum Lade-Weg mit/ohne Bit, Karte NUR im Marken-Fenster (ausserhalb 0 Pixel), Boot-Haken-Zeile, danach Kartenschirm des Blatts: MIT Bit die ganze Etage; ROOM1140 echter Weg (Aktion, Ja) -> Ereignis 28, Kartenschirm 3F mit allen Raeumen (>= 268 Pixel mehr) | gruen (437,7 s; Schwellen kalibriert: Wand-Fenster je ~Haelfte, Kartenschirm 3F 268 / 2F 311 / 1F 452 / B2 381 / B1 433) |
| `unit_r36_kartenbits` (Nachtrag, Abschnitt 9) | Z Zensus der Bank-9-Bits des Originals (alle RDTs, 85 Bits; Gegenproben 206 RDTs / 40674 Opcodes / 164 Item_aot_set / bekannte Bits); P alle Port-Bank-9-Bits (Makros + live k_r36k_lage) ausserhalb des Original-Satzes, Gegenprobe alte Belegung -> {90, 91}; V Plan nehmen (echtes Port-Programm) -> Stage-2-Items 0x32/0x3F bleiben; S Alt-Spielstaende (Nutzerkarte, v0.8.23-Stand) | 141 ok, 0 FAIL |

Gezielte Laeufe (KEINE volle Suite, VERTRAG 1.5 / Auftrag):
* `ctest --test-dir re15_port/build -R "r36_kartenitems" --timeout 1200 --output-on-failure`
  -> **"100% tests passed, 0 tests failed out of 2"** (Endstand, Total Test time 437,89 s)
* Regressionsgruppen der beruehrten Module (Karte/Besitz/Hinweis, Prop-Lader/Licht, scd_event_fire-Kette,
  Raumstart-Installer, die fuenf Raeume):
  `ctest --test-dir re15_port/build -R "karte|map|besitz|hinweis|r34n_e|r35_inhalt|r34n_a|r34n_d|fenster|cut10f0|r35_cut|1030|1140|10d0|1200|11d0|item_stack|r36_kartenitems" --timeout 600 --output-on-failure`
  -> **"100% tests passed, 0 tests failed out of 100"** (Total Test time 1596,55 s; darin 9 exe-Haken: r34n_e_dokumente_bild,
  r35_cut10f0, r35_cut1150, r35_fenster, r35_karte, r35_karte_fahrstuhl, r36_kartenitems_bild, map_uebergang,
  map_raum_live — alle im ersten Lauf gruen)

## 6. OFFEN
* **Groesse** (PORT-WAHL, Entscheidungsfrage 7.2): die Karten sind in die Nutzer-Marken eingepasst (Flaeche
  391..745 x 260..497 Einheiten); die RE2-Originalgroesse waere 1080 x 720 (2F: rund 2,8-mal so gross, gut 55 x 40
  statt 20 x 15 Pixel — sie deckte dort die Fahndungsplakate und das Foto zu).
* **Licht**: erledigt in Nachbesserung 1 (Abschnitt 8) — Flach-Attribut 0x0010 statt Tint-Konstante. OFFEN
  daraus: das Halbtransparenz-Bit 0x2 des Attribut-Worts (FUN_8002c18c @0x8002c2b4 -> Prim-Code 0x36, je
  STP-Texel) ist im Port fuer Objekte weiterhin nicht umgesetzt (Abschnitt 8.7).
* **Tiefe** in anderen Cuts: die Bildlage im Nutzer-Cut ist eingepasst; in anderen Cuts folgt sie der
  gemessenen Ebene. 1140: Tafel-Ebene parallel zur Rueckwand ANGENOMMEN (nur die rechte Kante trianguliert);
  1030: Wandtiefe aus der Bodenkante (+-1 Bildzeile = +-500 Einheiten) — nur Cut 3/10 sieht die Rueckwand.
* **ROOM1030 Telefon**: bis zur Aufnahme antwortet im Karten-Rechteck die Karte, sonst ueberall das Telefon.
* **ROOM1200**: im Ueberlapp mit dem Vitrinen-Ereignis (Slot 2, z -16183..-15800) gewinnt die Vitrine
  (kleinerer Slot) — vor der Karte (z > -15800) kommt die Karte (gemessen 183 Standorte).
* **Elza**: Mechanik in allen fuenf xxx1-Raeumen im Unit-Test (Raumstart + Aufnahme in 10D1); Framedumps
  nur Leon-Varianten (dieselben Kameras/BSS je Variante sind nicht geprueft).
* RE2s Bank-32-Varianten der Kachelzeilen (503/507) fuehrt der Port nicht (vor dieser Runde schon so).

## 7. Fuer den Nutzer
* **Neue Assets / Paket-Gate**: KEINE neuen Dateien unter shared_assets — Modell (RE2 karte01) und Texte
  sind in `re15_port/engine/src/gen/kartenitems_r36.inc` eingebacken (Quelle info/re2leon/PL0/RDT/
  ROOM3060.RDT, md5-geprueft). Keine Sprachdateien.
* **Bedienung**: an die markierte Stelle gehen, Aktion: "A police 3F map. / Will you take it?" -> Yes ->
  "You've taken the / police 3F map.". Danach zeigt die Karte (START, L1) die ganze Etage — nicht besuchte
  Raeume als Umriss (RE2-Zeile 498), besuchte gefuellt; die Etage ist mit HOCH/RUNTER waehlbar, auch wenn
  man sie nie betreten hat. Gilt fuer Leon und Elza. Der Zustand liegt im Spielstand (Bank-9-Bits 87..91; seit
  Abschnitt 9: 87/88/89/77/78).
* **Entscheidungsfragen**
  1. map3F.bmp zeigt den Konferenzraum ROOM1140 (3F), nicht Irons' Buero ROOM1150 — die 3F-Karte haengt
     deshalb an der Tafel in ROOM1140 (dort, wo du markiert hast). Richtig so?
  2. Groesse: eingepasst in deine Marken (ein Blatt Papier). Moechtest du die RE2-Originalgroesse
     (1,08 x 0,72 m, deutlich groesser)?
  3. ROOM1030: die Karte haengt im Bereich des Muenztelefon-Textes; bis zur Aufnahme kommt dort die
     Kartenfrage, daneben weiter der Telefon-Text. Passt das?

## 8. Nachbesserung 1 (nach Abnahme 0, `N_abnahme_0.md` M1 Licht) — 2026-10-11

Stand vorher: 2a741535 (Code 3c0737c0). Scratch `%TEMP%/claude/.../scratchpad/N_nb1/` (zensus_0x2d*.py,
edit1.py, lauf.sh, vorher.sh, paar1030.sh, runs/). Abnahme-Urteile zu den Punkten 1-2 bleiben unberuehrt
(ihre Riegel: `integration_r36_kartenitems_bild` Teil 1/2, `unit_r36_kartenitems` T/R/G/D/K/S).

### 8.1 Mangel M1 — Ursache
Der Prop-Zeichner in `platform/pc/main.c` kannte fuer Objekte nur den NCCT-Weg (`re15_light_shade_vertex`,
Raumlicht des aktiven Cuts). Die RDT-Lichtsaetze von ROOM1140/10D0/11D0 sind reine Ambient-Saetze
(Abnahme 4: 1140 Lichtblock @0x38C alle Lichter aus, Ambient (83,78,78) -> Vertexfarbe (41,39,39) =
32 % fuer JEDE Normale), die Karte war damit "fast schwarz" (Dossier 3.3). Statt den Original-Mechanismus
fuer ein unbeleuchtetes Objekt zu suchen, hatte die Spur eine Konstante je Raum aus dem Hintergrundbild
erfunden (Spalte `licht`, BG_REF "hellste 15 %", Haken `re15_kartenitems_r36_licht`) — ohne Adresse.
Der Mechanismus existiert (Abschnitt 8.3): das Attribut-Wort Pool+0x0c des Objekts, Bit 0x10 = FLACH.

### 8.2 Messung vorher (Stand 3c0737c0, alte exe md5 6f16291b = Messkopie der Abnahme)
Uebernommen aus Abnahme 0 Abschnitt 4 (nicht wiederholt): Karte 1140 Cut 5 Median-Summe 320 gegen Leons
hellsten Pixel 210 daneben; 1030 Karte B:R 2,05 neben Leon 1,10; Haken cut-unabhaengig trotz Raumlicht 38..89.
Eigene Messung mit dem neuen Auswerter `probe_r36_kartenitems_licht` (Kartenpixel = Diff MIT/OHNE Bit im
Marken-Fenster +-3; "PALETTE k von n" = wie viele davon EXAKT eine Farbe der 256er-CLUT der RE2-TIM sind,
5 Bit << 3 wie `rgb555_to_argb8888`) auf den Framedumps des Abnahme-Laufs `lauf_35afb981_*` (alte exe,
Lade-Weg F100, Nutzer-Cut; Lauf `4fed10ed` identisch):

| Raum (Cut) | Haken-Tint | Kartenpixel | Mittel RGB (Summe) | in der Palette |
|---|---|---|---|---|
| 1140 (5) | (156,155,153) | 73 | (105,107,96) 310 | **73 von 73** |
| 10D0 (6) | (181,186,180) | 219 | (101,104,94) 299 | **219 von 219** |
| 1030 (3) | (47,70,104) | 81 | (37,57,77) 172 | 0 von 81 |
| 1200 (2) | (85,85,75) | 177 | (68,70,56) 194 | 1 von 177 |
| 11D0 (0) | (74,75,77) | 169 | (61,64,59) 185 | 0 von 169 |

Befund dazu (gemessen, nicht vermutet): in 1140 und 10D0 hatte der Haken NIE gewirkt — `psx_prim_to_sdl_vert`
(render_pc.c:2616, `(v*255+64)/128`, Saettigung 255) bildet jede Tint > 128 auf 255 = neutral ab; die beiden
Karten wurden schon vorher exakt mit Texturfarbe gezeichnet (jeder Pixel eine Palettenfarbe). Das ist genau
das Bild des Flach-Weges. In 1030/1200/11D0 (Tint < 128) war die Karte verdunkelt bzw. blau getoent.

### 8.3 RE-Belege (selbst disassembliert, ghidra1_V2.txt; Decompilate RE_15_Quellcode_V2)
Objekt-Zeichner FUN_8002c18c, je Pool-Eintrag (Schrittweite 0x94):
```
8002c2ac 0c 00 22 8e   lw    v0,0xc(s1)        ; Pool+0x0c = Attribut-Wort
8002c2b4 02 00 42 30   andi  v0,v0,0x2         ; Bit 0x2 -> s2 = Halbtransparenz (Parameter 4, Prim-Code 0x36)
8002c2c0 05 00 62 14   bne   v1,v0(=3),LAB_8002c2d8   ; Typ 3 -> FUN_8003e64c (@0x8002c2c8)
8002c2dc ..            jal   FUN_80014368       ; Objekt-Cull (Region-Viereck)
8002c2ec 0c 00 22 8e   lw    v0,0xc(s1)
8002c2f4 10 00 42 30   andi  v0,v0,0x10        ; Bit 0x10
8002c2f8 19 00 40 10   beq   v0,zero,LAB_8002c360   ; 0 -> NCCT-beleuchtet: FUN_800254a0 @0x8002c380, FUN_800256b0 @0x8002c3b0
8002c2fc 70 00 30 26   _addiu s0,s1,0x70       ; sonst a2 = obj+0x70 (Farbe)
8002c320 c3 8b 00 0c   jal   FUN_80022f0c      ; FLACH: Dreiecke
8002c350 26 8c 00 0c   jal   FUN_80023098      ; FLACH: Vierecke
```
FUN_80022f0c (RE_15_Quellcode_V2/FUN_80022f0c.c:24-53): `param_3->cd = param_4 << 1 | 0x34; gte_ldrgb(param_3)`,
je Dreieck `*(CVECTOR *)(param_2 + 1) = *param_3` — die Primitivfarbe ist die KONSTANTE obj+0x70, kein ncct.
FUN_8002b898.c:16 (Objekt-Init): `*(undefined4 *)(&DAT_800b4008 + iVar1) = 0x808080` -> obj+0x70 = 0x808080
= PSX-neutral (Texel x 0x80/0x80 = unveraendert).
Das Attribut-Wort kommt aus dem Obj_model_set-Opcode, LAB_80040914:
```
800409a8 08 00 c2 84   lh    v0,0x8(a2)        ; pc[8..9]
800409b0 0c 00 a2 ac   sw    v0,0xc(a1)        ; -> Pool+0x0c (DAT_800b3fa4)
```
(erreicht aus beiden Zweigen @0x80040980/@0x80040988, also auch bei der Zweit-Installation).
RE2 (Ziel fuer den RE2-Gegenstand): FUN_800363f0.c:41-54 `if ((puVar4[4] & 0x10) == 0) FUN_80027bec/FUN_80027dbc
(beleuchtet) else FUN_8002d0e8/FUN_8002d244 (unbeleuchtet)`; FUN_80035710.c:16 `(&DAT_800d03a0)[param_1*0x7e] =
0x808080`. Die beiden RE2-Polizeiplaene tragen im Obj_model_set das Attributwort 0x0010 (room20B0 obj 8 @0x03158
`2d 08 00 00 00 00 00 00 00 00 0a 00 10 00 ...`, room2130 obj 0 @0x00FDC `2d 00 .. 0a 00 10 00 ...`; Abnahme 4.1).
Die RE2-Handler-Zuordnung Opcode-Wort -> Pool+0x10 wurde per Muster im RE2-Dump gesucht (drei Muster: `lh 0xc(a2)`
-> `sw 0x10(a1)`, `lhu 0xa(a2)` + `ori 1`, Positionswort -> `sw 0x38`) und NICHT gefunden; sie bleibt wie in
der Abnahme positionell (Enable-Wort, dann Attribut-Wort, wie RE1.5 pc[6..7]/pc[8..9]). Fuer die Konstante der
Karte reicht RE1.5 selbst: ROOM1200 obj 0/1 (@0x007DA/@0x00804, dieselbe Wand wie die B2-Karte) tragen 0x0010.

Zensus (zensus_0x2d_typ.py, alle 240 RDTs in shared_assets/PSX, 672 Obj_model_set, 0 Abbrueche):
type 0: 328x 0x0000, 169x 0x0010, 15x 0x0012, 6x 0x000E, 2x 0x0006; type 3: 22x 0x0012, 54x 0x001A; type 4: 76x
0x0000. Das Flach-Bit 0x10 tragen 260 Objekte in 46 Raeumen — 184 vom Typ != 3 auf dem Zweig @0x8002c2f8
(1040(3) 1041(3) 1090(3) 1091(1) 1190(1) 1191(1) 11A0(2) 11A1(2) 11B0(4) 11B1(3) 11C0(1) 11C1(1) 11E0(1)
11E1(1) 1200(3) 1201(3) 1210(1) 1211(1) 20A0(2) 20A1(2) 20B0(8) 20B1(8) 4000(6) 4001(7) 4030(1) 4031(1)
4040(5) 4041(5) 4080(5) 4081(5) 5040(4) 5041(4) 5060(8) 5061(8) 5070(8) 5071(8) 50A0(11) 50A1(11) 5100(4)
5101(4) 5130(4) 5131(4) 5140(2) 5141(2) 6000(6) 6001(6)) und alle 76 Typ-3-Objekte (die das Original ohnehin
ueber FUN_8003e64c -> FUN_80022f0c flach zeichnet). Der Port zeichnete bis jetzt ALLE 672 ueber NCCT —
eine vorbestehende Abweichung, die nicht diese Spur eingefuehrt hat; der generische Zweig behebt sie mit.

### 8.4 Umsetzung (Commit 0a13946d + Folgecommits)
| Datei | Aenderung |
|---|---|
| `include/re15_scd.h` | Prop-Feld `uint16_t attr` = Pool+0x0c (Kommentar mit den Adressen oben, Zensus) |
| `engine/src/scd_vm.c` op_obj_model_set | 1 Zeile: `attr = pc[8] \| pc[9] << 8` (@0x800409a8/@0x800409b0) |
| `platform/pc/main.c` Prop-Zeichner | Dreieck: der 3-Zeilen-Karten-Haken ersetzt durch `if (attr & 0x10) Vertexfarbe 0x80` (FUN_8002c18c @0x8002c2f4 -> FUN_80022f0c @0x8002c320, 0x808080); Viereck: dieselben 2 Zeilen (FUN_80023098 @0x8002c350) |
| `include/re15_kartenitems_r36.h` | `licht[3]` und `re15_kartenitems_r36_licht` entfernt; `RE15_R36K_PROP_ATTR 0x0010` + Beleg-Absatz LICHT |
| `engine/src/kartenitems_r36.c` | Installer setzt `attr = RE15_R36K_PROP_ATTR`; Licht-Funktion entfernt |
| `tools/r36_kartenitems/karten_export.py` + `gen/kartenitems_r36.inc` | BG_REF, "hellste 15 %", Textur-Median, Tint entfernt; inc neu erzeugt (Diff = nur die Tabelle, MD1/TIM/Texte bytegleich) |
| Tests | `unit_r36_kartenitems` Teil A (Attribut-Wort) + Prop-Attribut-Pruefung in R; neuer Auswerter `probe_r36_kartenitems_licht`; `integration_r36_kartenitems_bild`: Farbpruefung je Raum + ROOM1030 Cut 0 (zweiter Cut, anderes Raumlicht) |

Keine neue Konstante ohne Adresse: 0x10 (@0x8002c2f4), 0x80 = 0x808080 (FUN_8002b898.c:16), 0x0010 (RE2
@0x03158/@0x00FDC, RE1.5 @0x007DA). Der SDL-Weg bildet 0x80 ueber `psx_prim_to_sdl_vert` auf 255 ab (= Texel x 1).

### 8.5 Messung nachher (exe ab178a50 = Messkopie `re15_pc_nb1n_kartenitems.exe`; Lade-Weg CONTINUE, F100,
RE15_WINDOW_SCALE=1, Auswerter `probe_r36_kartenitems_licht` auf den Paaren MIT/OHNE Bit des Integrationslaufs)

| Raum (Cut) | Raumlicht an der Karte (Abnahme 0, NCCT) | Kartenpixel | Mittel RGB (Summe) | in der Palette |
|---|---|---|---|---|
| 1140 (5) | (41,39,39) | 73 | (105,107,96) 310 | 73 von 73 |
| 10D0 (6) | (41,39,39) | 219 | (101,104,94) 299 | 219 von 219 |
| 1030 (3) | (54,54,59) | 88 | (101,102,93) 297 | 88 von 88 |
| 1030 (0) | (89,89,94) | 48 | (105,107,96) 310 | 48 von 48 |
| 1200 (2) | (70,70,70) | 201 | (100,102,93) 296 | 201 von 201 |
| 11D0 (0) | (36,41,41) | 221 | (102,105,96) 304 | 221 von 221 |

Jeder Kartenpixel ist eine Palettenfarbe der RE2-TIM — die Karte wird ohne Raumlicht gezeichnet; ihre
Helligkeit (Summe 296..310) ist in allen sechs Ansichten dieselbe bis auf die Texel-Abtastung, obwohl das
Raumlicht an den Stellen zwischen 36 und 89 von 128 liegt (1030 Cut 3 gegen Cut 0: 297 gegen 310, Abstand
13, bei 1,65-fachem Raumlicht; mit NCCT laege der Abstand bei rund 120). Die Karte ist damit so hell wie
ihre Textur: genau das Bild, das RE2 fuer seine Wandplaene zeichnet (FUN_800363f0 unbeleuchtet) und das
RE1.5 fuer jedes Objekt mit dem Flach-Bit zeichnet (FUN_8002c18c @0x8002c2f4). Dass sie in 1140 Cut 5 heller
als Leon daneben ist (Abnahme 0: Leon NCCT bei 41/128), ist die Folge des Original-Mechanismus — eine Figur
wird beleuchtet, ein Flach-Objekt nicht — und keine Port-Konstante mehr. Erster Lauf mit der generischen
Fassung (exe 18f177fe, `lauf_71545489_*`, 441,43 s gruen) und Endstand (exe ab178a50, Lauf im
Regressionsblock 8.6, `lauf_7ae9aa6a_*`, 430,93 s gruen) liefern fuer die Karte dieselben Zahlen — nachgemessen mit dem Auswerter auf beiden Laeufen, Tabelle oben identisch (die Karte traegt 0x0010, Bit 0x2 = 0).
Bild: `N_nb1/v_1030_karte.png` (links alte exe Cut 3, blau getoent; Mitte neue exe Cut 3; rechts Cut 0 —
die Karte haengt als helles Blatt zwischen den Aushaengen, so hell wie die gemalten Zettel daneben).

### 8.6 Tests
| Test | neu/geaendert | Ergebnis |
|---|---|---|
| `unit_r36_kartenitems` | R: Karten-Prop `attr == 0x0010` (10 Raeume); A: VM speichert pc[8..9] — ROOM1200/1201 obj 0/1 = 0x0010 (@0x007DA/@0x00804), ROOM1140/1141 obj 0 = 0x0000 (@0x00B28) | 217 Pruefungen gruen (vorher 201) |
| `integration_r36_kartenitems_bild` | je Raum zusaetzlich d) jeder Kartenpixel eine Palettenfarbe (`probe_r36_kartenitems_licht`, exit 0 nur bei PALETTE == n), sechster Fall ROOM1030 Cut 0 (Fenster x88..101 y104..115, Schwelle 24), Summen Cut 3/Cut 0 hoechstens 1/10 auseinander | gruen: `ctest -R "^integration_r36_kartenitems_bild$"` -> "100% tests passed, 0 tests failed out of 1" (441,43 s, exe 18f177fe); Endstand im Regressionsblock unten |

Gezielte Laeufe (KEINE volle Suite):
* `ctest --test-dir re15_port/build -R "^unit_r36_kartenitems$" --timeout 300` -> "100% tests passed, 0 tests failed out of 1" (0,11 s)
* Regressionsblock = Regex der Abnahme 0 (3.3) + die Raeume/Module, deren Objekte den Flach-Zweig nehmen
  (1040/1090/1190/11A0/11B0/11C0/11E0/1210/1260, climb, obj_model, scd_, r34n_b, affen, gitter, espr):
  `ctest --test-dir re15_port/build -R "karte|map|besitz|hinweis|r34n_e|r35_inhalt|r34n_a|r34n_d|fenster|cut10f0|r35_cut|1030|1140|10d0|1200|11d0|item_stack|r36_kartenitems|irons_tisch|aot|msg_|action_msg|prop_|boot_bg|dark_start|relatch|save_counter|weste_load|savepoint|continue|1040|1090|1190|11a0|11b0|11c0|11e0|1210|1260|climb|obj_model|scd_|r34n_b|affen|gitter|espr" --timeout 900 --output-on-failure`
  -> **"100% tests passed, 0 tests failed out of 196"** (Total Test time 1897,46 s, CTEST_EXIT=0, `N_nb1/ctest_regr.log`; alle 196 im ERSTEN Lauf gruen, darin die exe-Haken integration_weste_load_pin 6,5 s, r34n_e_dokumente_bild 105,4 s, r35_cut1150 528,6 s, r35_karte 84,7 s, **r36_kartenitems_bild 430,9 s** (exe ab178a50), boot_bg_pin 3,0 s, dark_start_pin 2,5 s)

### 8.7 Nebenwirkung: Objekte mit dem Flach-Bit in anderen Raeumen (gemessen, alte gegen neue exe)
Der Zweig ist generisch (so hatte es die Abnahme verlangt: "Zweig '& 0x10' im Prop-Zeichner"); er betrifft
alle 169 Objekte mit Attribut 0x0010 (Zensus 8.3). Stichprobe per Debug-Sprung (RE15_DEBUG_JUMP=<raum>@250,
RE15_FORCE_CUT, Framedump F700, alte exe 6f16291b gegen neue; `ppmdiff.py`, |dR|+|dG|+|dB| > 30):

| Raum (Cut) | Objekt | attr | Pixel | Mittel vorher -> nachher | Bild |
|---|---|---|---|---|---|
| 1200 (2) | obj 0/1 Leichenschubladen-Fronten | 0x0010 | 2204 | (19,18,13) S49 -> (45,44,31) S120 | `v_1200_2.png`: vorher fast schwarze Kaesten vor den gemalten Schraenken, nachher dieselbe Helligkeit wie der gemalte Hintergrund |
| 1200 (1) | dito | 0x0010 | 1323 | S63 -> S155 | |
| 11B0 (0) | obj 1 (fern) | 0x0010 | 30 | S508 -> S566 | |
| 1210 (6) | obj 0 Zellentuer | 0x0010 | 4341 | (22,19,15) S55 -> (52,46,36) S134 | `v_1210_6.png`: Tuer mit sichtbarem Gitterfenster statt dunkelbrauner Flaeche |
| 11C0 (12) | obj 0 | 0x0010 | 52 | S27 -> S107 | |
| 11E0 (5) | obj 0 Rolltor (en 0x03FC) | 0x0010 | 5152 | (5,6,6) S17 -> (20,22,24) S67 | `v_11E0_5.png`: vorher ein fast schwarzer Block im Gang, nachher das gewellte Tor mit Textur |
| 11B0 (5) | obj 0 Kiste (4,6 % STP-Texel) | 0x0012 | 1098 generisch | S51 -> S146 generisch; Endstand (nur deckend): 192 Pixel = obj 2 (0x0010, sx 215/sy 174) S48 -> S103, die Kiste unveraendert | `v_11B0_5.png` |
| 5030 (2) | obj 1..11 Roehren, Typ 3 (34 % STP + 66 % schwarz) | 0x0012 | 767 generisch | S198 -> S290 generisch; Endstand (nur deckend): DIFF 0 Pixel, unveraendert | |
| 1040 (2/3) | obj 1-3 Glasflaechen (0 % STP) | 0x0012 | — (Raum beim Sprung dunkel, F400-700 schwarz) | — | |

Urteil aus den Bildern: die 0x0010-Objekte waren mit NCCT in den Ambient-Raeumen fast schwarze Kloetze vor
hellem gemaltem Hintergrund (Rolltor 11E0, Schubladen 1200, Zellentuer 1210); flach gezeichnet haben sie die
Helligkeit ihrer Textur und fuegen sich in den vorgerenderten Hintergrund ein — das ist der Original-Weg.
Bit 0x2 (halbtransparent): FUN_8002c18c gibt es als Parameter 4 an FUN_80022f0c weiter (Prim-Code 0x36),
die PSX mischt dann NUR Texel mit STP-Bit (psx-spx, Texel 0x8000..0xFFFF). STP-Zensus (`zensus_stp.py`,
TIM je obj_id aus der RDT+0x30-Tabelle): alle Typ-3-Objekte (11A0 obj 2/3, 1260 obj 0/1, 2000-20B0, 5030 obj
1-11) tragen zu 100 % STP-Texel (5030: 34 % + 66 % Schwarz = durchsichtig), 5060 obj 2-4 zu 50 %, die Kiste
11B0 obj 0 zu 4,6 %, 1040 obj 1-3 und 20A0 obj 4-6 zu 0 %. Der Port-Zeichner gibt jedem Texel Alpha 0xFF
(render_pc.c `rgb555_to_argb8888`, ABE-Kommentar beim Slot-Upload) und mischt nur je Primitiv (s_tri_blend);
eine STP-genaue Mischung ist Zeichner-Arbeit ausserhalb dieser Spur. Flach-deckend haette die 100-%-STP-
Objekte (Wasserflaechen/Glas) etwa doppelt so hell gezeigt wie das Original (Original = 0,5 Textur + 0,5
Hintergrund), die bisherige NCCT-Darstellung lag dort zufaellig naeher. PORT-ENTSCHEIDUNG (gekennzeichnet,
main.c): der Flach-Zweig gilt fuer `(attr & 0x12) == 0x10`; die 91 flach+halbtransparenten Objekte bleiben auf
dem bisherigen Weg, bis Parameter 4 portiert ist. Die Karte (0x0010) ist davon unberuehrt.

### 8.8 OFFEN (aus der Nachbesserung)
* Halbtransparenz-Bit 0x2 des Attribut-Worts fuer Objekte (FUN_8002c18c @0x8002c2b4 -> FUN_80022f0c Parameter
  4 -> Prim-Code 0x36, STP-Texel-Mischung; ABR aus dem tpage-Wort der MD1-Polygone): 91 Objekte in 1040/1041,
  11A0/11A1, 11B0/11B1, 1260/1261, 2000/2001, 2020/2021, 2040/2041, 2050/2051, 2070/2071, 2080/2081,
  2090/2091, 20A0/20A1, 20B0/20B1, 5030/5031, 5060/5061 — darunter 12 mit 0 % STP-Texeln (1040/1041 obj 1-3,
  20A0/20A1 obj 4-6), die das Original deckend und flach zeigt und die deshalb jetzt noch zu dunkel sind
  (NCCT). Eigene Spur: Texel-Alpha aus dem STP-Bit + Blend je Primitiv.
* Typ-3-Weg FUN_8003e64c (Kachel-Verdeckung der Fixtures) weiterhin nicht gesondert portiert (vorbestehend).
* RE2-Handler-Zuordnung Opcode-Wort [12..13] -> Pool+0x10 nicht aufgesucht (positionell, Abnahme 4.1).
* 1040 obj 1-3 nicht gemessen (Raum beim Debug-Sprung dunkel).

### 8.9 Fuer den Nutzer (Ergaenzung zu Abschnitt 7)
* Keine neuen Assets, keine Sprachdateien.
* Sichtbare Aenderung ausserhalb der Karten (Release-Notes-Hinweis): Objekte, die im Original ohne Raumlicht
  gezeichnet werden (Flach-Attribut; z.B. Rolltor ROOM11E0, Zellentuer ROOM1210, Schubladen ROOM1200,
  Objekte in 1090/1190/11A0/11B0/11C0 und in den Stages 2/4/5/6), erscheinen jetzt in Texturhelligkeit statt
  fast schwarz. Halbtransparente Objekte (Wasser/Glas) sind unveraendert.

## 9. Nachtrag Bank-9-Kollision (Zweig `r36/kartenbits`, Basis r36/integration 7a0fd453) — 2026-10-12

Baum `.claude/worktrees/r36_kartenbits`. Wird fortlaufend geschrieben (Sitzungslimit) — ein Nachfolger macht beim
ersten Unterabschnitt ohne Ergebnis weiter. Commits je Schritt (`wip(r36-kartenbits): ...`), am Ende `fix(...)`.

### 9.0 Befund und Auftrag (Orchestrator, aus R_abnahme_3.md H3 + eigenem Zensus)
Spur N speichert "Karte genommen" in Bank-9-Bits 87..91 (3F=87 ROOM1140, 2F=88 ROOM10D0, 1F=89 ROOM1030,
B2=90 ROOM1200, B1=91 ROOM11D0). Bank 9 ist im Original die Zone der Item-TAKEN-Bits. Die Bits 90 und 91 sind
Taken-Bits ORIGINALER Items in Stage 2 -> wer in Stage 1 den Lageplan B2/B1 nimmt, findet diese Items nicht;
umgekehrt meldet ein Spielstand mit genommenem Item die Karte als besessen. Punkte: P1 Kollision an der Engine
messen, P2 Zensus Original + Port, P3 Fix (zwei freie Bits), P4 Riegel-Test, P5 Alt-Spielstaende, P6 Regression.

### 9.1 RE-Belege (selbst disassembliert, `re15_disasm.py` auf info/Re1.5/PSX.EXE)
**Item_aot_set (Opcode 0x50, Sprungtabelle 0x800744a8[0x50] = 0x80040644)** — der Taken-Test beim Anlegen:
```
80040654 lw   a2,28(a0)        ; a2 = pc
8004065c lbu  v0,3(a2)         ; pc[3]
80040664 andi v0,v0,0x80       ; Langform?
80040668 beq  v0,zero,0x80040680
80040670 lhu  a1,26(a2)        ; LANG: Bit = u16 pc[26..27]
80040674 lbu  s1,28(a2)        ;       Prop  = pc[28]
80040680 lhu  a1,18(a2)        ; KURZ: Bit = u16 pc[18..19]
80040684 lbu  s1,20(a2)        ;       Prop  = pc[20]
800406d4 lui  a0,0x8007
800406d8 lw   a0,18056(a0)     ; a0 = *(0x80074688) = 0x800b1078 = Flag-Tabelle 0x80074664[9]
800406dc jal  0x8004efe4       ; Bit-Test
800406e4 beq  v0,zero,0x8004071c ; Bit 0 -> Satz bleibt scharf
800406f4 sb   zero,0(v0)       ; Bit 1 -> rec[0] (sce) = 0: Satz still
800406f8..80040718 sw v1(0x80000000),0x800b3f98+s1*148 ; Modell pool[s1] versteckt
```
**FUN_8004efe4 (Bit-Test):** `srl v1,a1,5` / `sll v1,v1,2` / `addu v1,v1,a0` / `andi a1,a1,0x1f` /
`lui v0,0x8000` / `lw v1,0(v1)` / `srlv v0,v0,a1` / `jr ra` / `and v0,v0,v1` — Wort = Basis + (Bit>>5)*4,
Maske 0x80000000 >> (Bit & 31).
**Ck (0x21) @0x8003fcf4:** `lbu a2,1(v0)` Bank = pc[1], `lhu a1,2(v0)` Bit = pc[2], Wort `(a1>>3)&0x1c`,
Maske `0x80000000 >> (a1 & 0x1f)` (@0x8003fd38-40), Tabelle `0x80074664 + Bank*4` (@0x8003fd20-2c).
**Set (0x22) @0x8003fdd0:** `lbu v1,1(v0)` Bank, `lbu a1,2(v0)` Bit, `lbu a2,3(v0)` Op (0 loeschen @0x8003fe4c,
1 setzen @0x8003fe64, 7 umschalten @0x8003fe78), Wort `(Bit>>5)*4` (@0x8003fdf0-f4), dieselbe Tabelle 0x80074664.
**Flag-Tabelle 0x80074664** (`re15_disasm.py table`): [9] = 0x80074688 -> **0x800b1078**, [10] -> 0x800b1098
=> Bank 9 = 0x800b1078..0x800b1097 = 256 Bits. Item_aot_set-Taken-Bit, Ck(9,n) und Set(9,n) adressieren
DASSELBE Bit (gleiche Basis, gleiche Wort-/Masken-Rechnung). Der Port fuehrt Bank 9 als `g_game.flags`-Zone 9
(`re15_game_flag_get(9,..)`; op_item_aot_set scd_vm.c "TAKEN-BIT persistence" liest `re15_game_flag_get(9, tk_bit)`).
**Weitere Leser/Schreiber von 0x800b1078 in der EXE** (XREF ghidra1_V2.txt Z. 493170: 8001e0d4, 80021fd4,
800406d8, 80074688): 0x8001e0d4 (Aufnahme-Modal, `jal 0x8004ef90` @0x8001e0d0, Bit = Record-Feld) und
0x80021fd4 (`lhu a1,4(s0)` @0x80021fcc mit s0 = Record+12 bzw. +20 -> Record+16/+24 = pc[18]/pc[26],
`jal 0x8004ef90` @0x80021fd8) setzen das Bit AUS DEM Item_aot_set-Satz — keine eigenen Bitnummern.

### 9.2 P1 Messung vorher (Stand 7a0fd453 + neuer Test, exe-unabhaengig an der Engine)
Neuer Test `unit_r36_kartenbits` (tests/unit/test_r36_kartenbits.c, registriert in tests/unit/probes/r36_kartenbits.cmake),
Teil V, am UNVERAENDERTEN Spur-N-Stand gebaut und gefahren. Ablauf je Plan: Raum des Plans starten (scd_room_reenter mit
allen Port-Installern), Ereignis 28 feuern (scd_event_fire = was der Aktions-Scan des EVENT-Satzes tut), Ja/Nein mit JA
beantworten (virtuelles Bestaetigen-Bit 0x4000), Faden bis zum Ende; danach mit DEMSELBEN Bank-9-Stand die Stage-2-Raeume
starten. Die Saetze, gegen die gemessen wird (Bytes im RDT geprueft):
* ROOM2040/2041 main00 @0x01364 `50 07 09 31 03 00 a4 06 62 c6 e8 03 e8 03 32 00 01 00 5b 00 ff 00` = Slot 7, Item 0x32
  "Head of Akuma" (Name aus der byte-true Namensbank DEBUG.BIN @0x800c495c/@0x800c4a28), Taken-Bit u16 pc[18] = 0x5b = **91**
* ROOM2090/2091 main00 @0x00A12 `50 05 09 31 01 00 e6 14 c4 be e8 03 e8 03 3f 00 01 00 5a 00 00 00` = Slot 5, Item 0x3F
  "Pocket Watch", Taken-Bit = 0x5a = **90**, Weltmodell obj 0 (pc[20])

Ergebnis (ctest-Ausgabe, gekuerzt):
```
ok:   ROOM2040 Slot 7: frisch scharf (1), mit Bit (9,91) still (1) — Gegenprobe der Messung     (ebenso 2041, 2090/2091 mit 90)
ok:   Plan Blatt 4 (ROOM1140): Port-Programm Ereignis 28 lief zu Ende, setzte 1 Bank-9-Bit(s): (9,87)  -> alle 4 Saetze scharf
ok:   Plan Blatt 3 (ROOM10D0): ... (9,88) -> alle 4 scharf;  Plan Blatt 2 (ROOM1030): ... (9,89) -> alle 4 scharf
ok:   Plan Blatt 1 (ROOM1200): Port-Programm Ereignis 28 lief zu Ende, setzte 1 Bank-9-Bit(s): (9,90)
FAIL:   nach Plan Blatt 1 (Bit 9,90): ROOM2090 Slot 5 Item 0x3F (Pocket Watch, Taken-Bit 90) STILL - das Item ist weg
FAIL:   nach Plan Blatt 1 (Bit 9,90): ROOM2091 Slot 5 Item 0x3F (Pocket Watch, Taken-Bit 90) STILL - das Item ist weg
ok:   Plan Blatt 0 (ROOM11D0): Port-Programm Ereignis 28 lief zu Ende, setzte 1 Bank-9-Bit(s): (9,91)
FAIL:   nach Plan Blatt 0 (Bit 9,91): ROOM2040 Slot 7 Item 0x32 (Head of Akuma, Taken-Bit 91) STILL - das Item ist weg
FAIL:   nach Plan Blatt 0 (Bit 9,91): ROOM2041 Slot 7 Item 0x32 (Head of Akuma, Taken-Bit 91) STILL - das Item ist weg
FAIL: Original-Items 0x32/0x3F genommen (Bits 90/91): Besitz der Polizei-Blaetter 0..4 = 0x03
FAIL:   ROOM1200: Plan Blatt 1 haengt noch (Prop obj 15 + Zone Slot 40 Ereignis 28)
FAIL:   ROOM11D0: Plan Blatt 0 haengt noch (Prop obj 15 + Zone Slot 40 Ereignis 28)
```
Befund damit an der Engine bestaetigt, in BEIDE Richtungen:
* Lageplan B2 (ROOM1200) genommen -> in ROOM2090/2091 fehlt die Taschenuhr (Satz still, Weltmodell obj 0 versteckt,
  Port op_item_aot_set = Original @0x800406f4/@0x800406f8-718); Lageplan B1 (ROOM11D0) -> in ROOM2040/2041 fehlt
  "Head of Akuma". 3F/2F/1F (87/88/89) treffen keinen dieser Saetze.
* Item 0x3F bzw. 0x32 genommen (Bits 90/91) -> Kartenblatt B2 (Seite 1) und B1 (Seite 0) gelten als besessen
  (Besitz-Maske 0x03), und die Plaene in ROOM1200/11D0 werden gar nicht erst angelegt.
* Softlock? Kein Skript prueft die Bits 90/91 (Ck/Set-Zensus 9.3), keiner der beiden Gegenstaende hat eine
  Benutzungsstelle unter den 20 "You've used the X"-Meldungen (gen/discard_sites.inc). Einzige Textstelle zu einem der
  beiden: ROOM2001 id 11 @0x2081 "Elza: My, that's a nice pocket watch!" (rdt_msgdump.py). Die Folge ist also ein
  FEHLENDES Original-Item + falscher Kartenbesitz; ein harter Softlock ist nicht belegt.
* Messfalle (beim Bau des Tests gefunden): `scd_vm_init` ruft `re15_game_state_init` (scd_vm.c) und nullt damit alle
  Flags — ein Test, der zwei Raeume nacheinander mit scd_vm_init startet, misst sonst einen frischen Stand (erste Fassung
  meldete faelschlich "mit Bit still (0)"). Der Test bewahrt g_game um scd_vm_init herum auf (Raumwechsel des Spiels
  nullt die Flags nicht, scd_room_setup.c "scd_vm_init MINUS re15_game_state_init").

### 9.3 P2 Zensus (Original + Port) — Datei `analysis/befunde_runde36/N_belege/bank9_zensus.txt`
Werkzeug `re15_port/tools/r36_kartenbits/bank9_zensus.py` (Laengentabelle = scd_walk_lib = scd_vm.c s_opcode_sizes;
Teile original | exe | port), Ausgabe mit Auswertungskopf eingecheckt.

**Wer im Original Bank 9 anfasst — vollstaendig belegt, nicht nur die Skripte:**
* Die Flag-Tabelle 0x80074664 wird in der EXE an genau fuenf Stellen indiziert (ghidra1_V2.txt `PTR_DAT_80074664
  XREF[5]`): Ck 0x21 @0x8003fd2c, Flag-Ck2 0x58 @0x8003fda0, Set 0x22 @0x8003fe04, Flag-Set2 0x59 @0x8003fed0,
  AOT-FLAG_CHG sce 4 LAB_80043120 @0x80043140 (`lhu v1,0(a0)` Bank, `lhu a1,2(a0)` Bit, `lhu a0,4(a0)` an/aus
  @0x80043120-28). Eintrag [9] direkt: nur Item_aot_set @0x800406d8.
* `DAT_800b1078 XREF[4]`: 0x8001e0d4, 0x80021fd4, 0x800406d8, 0x80074688 — die zwei Aufnahme-Wege setzen das Bit aus
  dem Item-Satz (`lhu a1,4(s0)` @0x80021fcc, s0 = Satz+12/+20).
* Die 105 Aufrufe der Bit-Helfer FUN_8004ef90/efb8/efe4 in EXE + STAGE1..5.BIN: in den Overlay-Dekompilaten nur die
  Basen 0x800b1028 (Bank 5), 0x800b1038 (Bank 7), 0x800b1058 (Bank 8); `unaff_s0+0x48a4` = Bank 5 (gleiches Muster wie
  PSX.EXE @0x80051784, Ghidra `a0=>DAT_800b1028`); der Direkt-Scan (lui/addiu-Paare) findet in keinem Overlay
  0x800b1078 oder die Tabelle.
=> Original-Bank-9-Bits = Item_aot_set-Taken-Bits + Ck/Set(9,n) + FLAG_CHG(9,n) (+ 0x58/0x59 mit Bank 9: keine).

**Ergebnis Original** (206 RDTs mit Kopf + 34 Stummel, 40674 Opcodes, **0 Desync**, 164 Item_aot_set = dieselbe Zahl
wie der C-Zensus in r21_discard TEIL C): 54 x Ck(9,n), 1 x Set(9,21,1) (ROOM3091 sub06 @0x016C2), 0 x 0x58/0x59 mit
Bank 9, FLAG_CHG nur Bank 5 (26 Saetze), kein Taken-Bit >= 256. **85 von 256 Bits belegt** (Zahl wie der Runde-30-
Zensus r30_idw_zensus.py): `2 6 10 11 12 14 15 16 18 19 20 21 22 52 85 86 90 91 92 98 99 104 105 108 109 134 135 136
138 140..144 146..155 162..169 180..198 216 217 218 223..231 252 253`. Einziges reines Skript-Bit: 21; alle anderen
sind Item-Taken-Bits. Frei: `0..1, 3..5, 7..9, 13, 17, 23..51, 53..84, 87..89, 93..97, 100..103, 106..107, 110..133,
137, 139, 145, 156..161, 170..179, 199..215, 219..222, 232..251, 254..255`.

**Ergebnis Port** (grep include/ engine/src platform/pc + Tabellen-Makros; Runde 30 bis 36): 53 Sicherung, 54/55
Irons-Tisch, 56 Granate, 57..60 Dokumente, 61/62 Leichen, 63 Rolltor, 65 Ada-Ruf, 71 10F0-Szene, 72 10F0 Ziel 2,
73 Irons-Tod, 74/75/76 Spawns, 79 Fenster 1120, 80/81 r35-Inhalt, 82/83 Q-Schluessel, 84 J-Werkstatt-Tuer,
**87..91 N-Lageplaene**, 93 Q-Parkplatz-Tuer. (Item-Modal / menu_common / op_item_aot_set benutzen das Bit des
Satzes.) Spur R (nach der Basis gemergt, ba384fac) bringt kein Bank-9-Bit (diff 7a0fd453..r36/integration geprueft).

**Schnitt = {90, 91}** — genau der Befund. Zusaetzlich: die VERTRAG-Reserven **85 (J), 86 (L), 92 (P) sind Original-
Taken-Bits** und duerfen nie vergeben werden: 85 = ROOM3040/3041 sub00 @0x014B2 Item 0x36 "Green Keycard"
(Ck(9,85,1) sub01 @0x014CE), 86 = ROOM4040/4041 main00 @0x00F66 Item 0x41 "Spark Plug", 92 = ROOM3050/3051 main00
@0x01EC6 Item 0x07 "Super Redhawk" x6 — kein Port-Code benutzt sie (Zensus).
Der VERTRAG-Zensus vom 2026-10-10 hatte nur `re15_port/include/*.h` gezaehlt (VERTRAG 1.1 Z. 34) — die Original-
Belegung fehlte ganz; daher "Frei: 64, 66-70, 77, 78, 82-255".

**Wahl B2/B1 = 77/78** (frei in beiden): im Original-Block 53..84, den der Port seit Runde 30 fuer eigene Bits nutzt;
in keiner Port-Version je Bank-9-Bit (`git log --all -G` ueber `#define *BIT* 77|78|0x4D|0x4E`,
`re15_game_flag_*(9, 77|78)`, SCD `0x21/0x22, 0x09, 0x4D/0x4E`: 0 Treffer; Tags v0.8.21/v0.8.22/v0.8.23: 0 Treffer —
Gegenprobe der Suche: dieselbe Suche findet `RE15_R36Q_TUER_BIT 93` in c5a6d7d4). VERTRAG Runde 35 1.1 hatte 77/78 als
L-Reserve gefuehrt ("77 = 1060->1040-Dialog nur falls noetig, 78 Reserve") — nie belegt. (64, 66..70 waeren ebenso frei;
64 kommt in der Geschichte nur als Bank-4-Bit RE15_CUT10F0_ZIEL1_ERREICHT_BIT vor.) 87/88/89 bleiben: im Original frei.

### 9.4 P3 Fix — B2 = 77, B1 = 78
Geaendert (nur das Bit; Lage, Modell, Texte, Programm unveraendert):
| Datei | Aenderung |
|---|---|
| `re15_port/tools/r36_kartenitems/karten_export.py` | RAEUME: ROOM1200 B2 90 -> **77**, ROOM11D0 B1 91 -> **78**; Kommentar mit Taken-Test @0x80040680/@0x800406dc, Tabelle 0x80074664[9] @0x80074688, Zensus, alte Bits @0x00A12 / @0x01364 |
| `re15_port/engine/src/gen/kartenitems_r36.inc` | NEU ERZEUGT (`karten_export.py --schreiben`); `git diff`: genau die zwei Zeilen `k_r36k_lage[3]`/`[4]`, nur Feld 3 (90 -> 77, 91 -> 78). Gegenprobe: der Generator erzeugt den Stand VOR der Aenderung bytegleich (Lauf auf 7a0fd453-Stand: kein Inhalts-Diff) |
| `re15_port/include/re15_kartenitems_r36.h` | Kopf-Absatz "BANK-9-BITS" (Herleitung mit Adressen, Liste der nicht freien Reserve-Bits); Feld `bit` und Haken-Kommentar 87/88/89/77/78 |
| `re15_port/engine/src/re15_map_owned.c` | Kommentar 87/88/89/77/78 |
| `re15_port/tests/unit/test_r36_kartenitems.c` | `bits[5] = { 87, 88, 89, 77, 78 }`, Teil S setzt 87 + **78** (B1), Kommentare |
| `re15_port/tests/integration/test_r36_kartenitems_bild.cmake` | Feld 7 der Zeilen b2/b1: 77/78; Kopfkommentar |
| `analysis/befunde_runde36/VERTRAG.md` 1.1 | N-Zeile 87-89, 77, 78; Nachtrag: 85/86/90/91/92 sind Original-Taken-Bits (J/L/P-Reserven 85/86/92 gestrichen), Liste "frei in Original UND Port" |

Warum 77/78 und nicht anders: Abschnitt 9.3 (frei im Original-Block 53..84, nie im Port). 87/88/89 bleiben (im Original
frei) — ein Umzug aller fuenf wuerde Staende mit genommenem 3F/2F/1F-Plan ohne Not entwerten.

Messung nachher (unit_r36_kartenbits Teil V, derselbe Test wie 9.2, jetzt gruen):
```
ok:   Plan Blatt 1 (ROOM1200): Port-Programm Ereignis 28 lief zu Ende, setzte 1 Bank-9-Bit(s): (9,77)
ok:     nach Plan Blatt 1 (Bit 9,77): ROOM2090 Slot 5 Item 0x3F (Pocket Watch, Taken-Bit 90) scharf   (ebenso 2091, 2040, 2041)
ok:   Plan Blatt 0 (ROOM11D0): Port-Programm Ereignis 28 lief zu Ende, setzte 1 Bank-9-Bit(s): (9,78)
ok:     nach Plan Blatt 0 (Bit 9,78): ROOM2040 Slot 7 Item 0x32 (Head of Akuma, Taken-Bit 91) scharf   (ebenso 2041, 2090, 2091)
ok:   Original-Items 0x32/0x3F genommen (Bits 90/91): Besitz der Polizei-Blaetter 0..4 = 0x00
ok:     ROOM1200: Plan Blatt 1 haengt noch (Prop obj 15 + Zone Slot 40 Ereignis 28)    (ebenso alle fuenf)
```
unit_r36_kartenitems: 217 PASS, 0 FAIL (Zeile 3: ROOM1200 Bit 77, Zeile 4: ROOM11D0 Bit 78; Teil S Bits 87/78 -> Blatt 4 + 0).

### 9.5 P4 Riegel — `unit_r36_kartenbits` (tests/unit/test_r36_kartenbits.c, tests/unit/probes/r36_kartenbits.cmake)
Drei Teile, alle an echten Daten / echtem Port-Code:
* **Z Zensus des Originals** (a/b): alle ausgelieferten RDTs (STAGE1..6, Leon- und Elza-Varianten), main- und sub-SCD,
  Laengen aus `scd_opcode_size_at` (scd_vm.c, schon fuer r21_discard exportiert und genau die Lese-Funktion fuer
  `s_opcode_sizes`, die der Auftrag erlaubt — **scd_vm.c blieb unveraendert**), Sektionsende/Regionen/Vorwaertsziel-Regel
  1:1 wie scd_dump_room.py. Gesammelt: Item_aot_set-Bit (u16 pc[18]/pc[26]), Ck(9,n), Set(9,n), FLAG_CHG-Saetze mit Bank 9
  (Aot_set/Aot_reset sce 4); 0x58/0x59 mit Bank 9 sind verboten (Bit erst zur Laufzeit). Gegenproben: 206 RDTs, **1579
  Regionen** (= Runde-30-Zensus "1579 SCD-Bloecke"), **40674 Opcodes** (= bank9_zensus.py, zweite unabhaengige
  Implementierung), 0 Desync, 164 Item_aot_set, 85 Bits; die bekannten Bits 85, 86, 90, 91, 92 sowie 52/136/138
  (Keycard-Tore) und 21 (Skript-Bit ROOM3091) werden gefunden.
* **P Port** (c/d): explizite Liste aller 30 Bank-9-Eintraege des Ports mit Makro + Quelle (Werte aus den Koepfen
  re15_sicherung.h, re15_irons_tisch.h, re15_granate.h, re15_dokumente.h, re15_leiche.h, re15_rolltor.h, re15_adaruf.h,
  re15_cut10f0.h, re15_irons_tod.h, re15_fenster1120.h, re15_inhalt_r35.h, re15_schluessel_r36.h, re15_szenenleon_r36.h,
  re15_tuer1060.h; je Eintrag Bank == 9 geprueft) + die 5 Lageplan-Bits LIVE aus `k_r36k_lage`: keines im Original-Satz,
  kein Lageplan-Bit gleich einem anderen Port-Bit. GEGENPROBE im Test: die alte Spur-N-Belegung {87..91} durch DIESELBE
  Pruefung -> genau {90, 91}.
* **V Verhalten** (Abschnitt 9.2): Plan ueber das echte Port-Programm nehmen -> Stage-2-Items bleiben scharf;
  Rueckrichtung.

Ergebnis Endstand: `100% tests passed, 0 tests failed out of 1` (127 ok, 0 FAIL).

**Gegenprobe (d) am gebauten Code** — `gen/kartenitems_r36.inc` voruebergehend auf die alte Belegung (B2 90, B1 91)
zurueckgesetzt, gebaut, Riegel gefahren (exit 1), danach .inc wiederhergestellt (bytegleich zum Commit, cmp), neu
gebaut, Riegel gruen (exit 0). Ausgabe mit der alten Belegung (alle 10 FAIL):
```
FAIL: Bit  90 Lageplan Blatt 1 ROOM1200 (k_r36k_lage, Spur N) — KOLLISION mit ROOM2090 main00 @0x00A12 Item_aot_set
FAIL: Bit  91 Lageplan Blatt 0 ROOM11D0 (k_r36k_lage, Spur N) — KOLLISION mit ROOM2040 main00 @0x01364 Item_aot_set
FAIL: 35 Port-Bits gegen 85 Original-Bits: 2 Kollision(en)
FAIL:   nach Plan Blatt 1 (Bit 9,90): ROOM2090 Slot 5 Item 0x3F (Pocket Watch, Taken-Bit 90) STILL - das Item ist weg
FAIL:   nach Plan Blatt 1 (Bit 9,90): ROOM2091 Slot 5 Item 0x3F (Pocket Watch, Taken-Bit 90) STILL - das Item ist weg
FAIL:   nach Plan Blatt 0 (Bit 9,91): ROOM2040 Slot 7 Item 0x32 (Head of Akuma, Taken-Bit 91) STILL - das Item ist weg
FAIL:   nach Plan Blatt 0 (Bit 9,91): ROOM2041 Slot 7 Item 0x32 (Head of Akuma, Taken-Bit 91) STILL - das Item ist weg
FAIL: Original-Items 0x32/0x3F genommen (Bits 90/91): Besitz der Polizei-Blaetter 0..4 = 0x03
FAIL:   ROOM1200: Plan Blatt 1 haengt noch (Prop obj 15 + Zone Slot 40 Ereignis 28)
FAIL:   ROOM11D0: Plan Blatt 0 haengt noch (Prop obj 15 + Zone Slot 40 Ereignis 28)
```
Nebenbefund (bestaetigt den Port): op_item_aot_set liest das Taken-Bit nur als uint8 pc[18]/pc[26], das Original als
u16 (`lhu` @0x80040680/@0x80040670). Zulaessig, weil KEIN ausgelieferter Satz ein Bit >= 256 traegt (Riegel Z prueft
"kein Item-Taken-Bit >= 256"; ein solches Bit liefe im Original ueber Bank 9 hinaus in Bank 10 = 0x800b1098).
Grenze des Riegels: Teil P kennt nur die Port-Bits, die in seiner Liste stehen. Ein NEUES Port-Bit muss dort eingetragen
werden (Kommentar an der Liste; bank9_zensus.py Teil port findet alle Zugriffsstellen per grep).

### 9.6 P5 Alt-Spielstaende
**Die neuen Bits 77/78 koennen in keinem bisherigen Spielstand stehen:**
* Original: kein Item_aot_set, kein Ck/Set, kein FLAG_CHG mit 77/78, kein 0x58/0x59 mit Bank 9 (Zensus 9.3, Riegel Z) —
  die ausgelieferten Skripte, die der Port-VM ausfuehrt, setzen sie nie; die EXE setzt Bank 9 nur mit dem Bit eines
  Item-Satzes (@0x8001e0d0 / @0x80021fd8).
* Port: in KEINER Version je als Bank-9-Bit benutzt — `git log --all -G` (alle Zweige, auch r35/*, r34n/*) ueber
  `#define RE15_*BIT* 77|78|0x4D|0x4E`, `re15_game_flag_(get|set)(9, 77|78)`, SCD-Bytes `0x21/0x22, 0x09/9, 0x4D/0x4E/77/78`:
  0 Treffer (Gegenprobe der Suche: sie findet `RE15_R36Q_TUER_BIT 93` in c5a6d7d4); `git grep` in den Tags v0.8.21,
  v0.8.22, v0.8.23: 0 Treffer. Einzige Ausnahme waeren die Test-Schalter `RE15_SET_FLAG`/`RE15_SET_FLAG_AT`
  (platform/pc/main.c Zeile 4778 / 7302, setzen beliebige Bits) — kein Spielweg.
* Gemessen an echten Karten (Riegel Teil S, nur gelesen): die eingecheckte Nutzerkarte 2026-09-27 (Fixture
  probes/r30_karte_nutzer_2026-09-27.mcr, 4 Plaetze) und eine Kopie der aktuellen Karte neben der exe des Hauptbaums
  (re15_port/build/platform/pc/re15_card.mcr, Stand 2026-10-05, md5 4990b84f, 5 Plaetze; Aufruf
  `RE15_R36KB_KARTE=<karte> test_r36_kartenbits.exe`): in keinem Platz 77 oder 78 gesetzt. Gesetzte Bank-9-Bits dort nur
  Original-Item-Bits und Port-Bits der Liste (53..63, 65, 80), z.B. Platz 3 (ROOM1071): `2 10 53 54 55 56 57 60 62 63 80
  99 104 105 134 136 138 ...`.
* Spur N war nie in einem Release und nie in master (kartenitems_r36.c fehlt in v0.8.21/22/23 und master f3f7584b; nur
  r36/integration seit cb1d5f59) — die alten Lageplan-Bits 90/91 koennen nur interne Integrations-Testlaeufe gesetzt
  haben. Ein solcher interner Stand (B2/B1-Plan mit 90/91 genommen) zeigt nach dem Fix: Plan haengt wieder (77/78 = 0),
  laesst sich erneut nehmen; die Taschenuhr / "Head of Akuma" bleiben dort verschwunden (90/91 gesetzt) — nur
  Testsstaende, kein Nutzerstand betroffen.

**Was ein v0.8.23-Spielstand mit genommenem Item 0x32/0x3F jetzt zeigt** (Spielstand-Format v9, re15_savedata.h/.c seit
v0.8.23 unveraendert — `git diff v0.8.23 HEAD` leer): Riegel Teil S2 schreibt einen Stand mit Bits 90/91 ueber die ECHTE
Speicherkarte (re15_memcard_save -> re15_memcard_load -> re15_savedata_restore) und misst:
```
ok:   Spielstand-Format v9 = v0.8.23 (RE15_SAVE_VERSION 9, re15_savedata.h/.c seit v0.8.23 unveraendert)
ok:   Stand mit Items 0x3F/0x32 genommen ueber die Speicherkarte geschrieben und geladen (Bits 90/91 gesetzt)
ok:   nach dem Laden: kein Polizei-Blatt im Besitz (Maske 0x00) — mit der alten Belegung waere es 0x03
ok:   nach dem Laden: Plan Blatt 1 haengt in ROOM1200 (aufnehmbar, setzt dann Bit 77)
ok:   nach dem Laden: Plan Blatt 0 haengt in ROOM11D0 (aufnehmbar, setzt dann Bit 78)
ok:   nach dem Laden: ROOM2040 Item 0x32 bleibt genommen (Satz still, @0x800406f4)   (ebenso 2041, 2090, 2091)
```
Die alte Belegung haette denselben Stand als "B1 + B2 besessen" gezeigt (Maske 0x03, Abschnitt 9.2 Rueckrichtung) und die
beiden Plaene gar nicht angelegt.
Riegel-Endstand: 141 ok, 0 FAIL.

### 9.7 P6 Regression (gezielt, KEINE volle Suite)
Bau: `bash re15_port/tools/local_build.sh build` -> `=== LOCAL-BUILD-OK (build)` (Endstand, inkl. Re-Configure fuer die
neue probes-Datei).
```
PATH="/c/msys64/mingw64/bin:$PATH" ctest --test-dir re15_port/build   -R "r36_kartenitems|kartenitem|r36_kartenbits|map_|karte|r33_karte|r35_karte|unit_r36_|item" --timeout 900 -j 4
...
121/123 Test #547: integration_r35_karte_fahrstuhl ....   Passed  185.82 sec
122/123 Test #603: integration_r36_karte ..............   Passed  190.91 sec
123/123 Test #606: integration_r36_kartenitems_bild ...   Passed  473.71 sec

100% tests passed, 0 tests failed out of 123

Total Test time (real) = 476.00 sec
```
Darin u.a. unit_r36_kartenbits, unit_r36_kartenitems, alle unit_r36_* (Spuren A..R), unit_r33_karte_*, unit_r35_karte_*,
integration_r35_karte, integration_r36_karte, integration_map_uebergang, integration_map_raum_live,
integration_item_name_census. Die exe des Laufs integration_r36_kartenitems_bild schreibt mit den neuen Bits
(debug.log `lauf_*_b2_mit`: `[kartenitems] ROOM1200: Karte Blatt 1 (Bit 9,77) ...`, `lauf_*_b1_mit`: `ROOM11D0: Karte
Blatt 0 (Bit 9,78) ...`; die Laeufe mit gesetztem Bit 77/78 legen keine Karte an). Parallel lief auf der Maschine eine
fremde Sitzung (ctest + re15_pc_r36r_haken-exe); kein Test flatterte.

### 9.8 OFFEN
* `RE15_MIN_TESTS` (re15_port/tools/local_build.sh) steht weiter auf 592; dieser Zweig bringt +1 Test
  (unit_r36_kartenbits, Suite hier 694). Anheben ist Sache der Integration (VERTRAG 1.5).
* Der Riegel Teil P kennt nur die Port-Bits seiner Liste. Wer ein NEUES Bank-9-Bit vergibt, traegt es dort ein
  (Kommentar an `k_port[]`) und in VERTRAG 1.1; `bank9_zensus.py port` listet alle Zugriffsstellen.
* Nicht Teil dieses Auftrags: Port-eigene Zustandsbits in ANDEREN Banken (z.B. Bank 7 Tot-Bits der Montage
  RE15_IT_* 40..43, 46..49, 75..79 laut re15_irons_tod.h, eigener Zensus aller Sce_em_set im irons_tod-Dossier §2.3; Bank 3/4 Lesezugriffe auf Original-Flags) — derselbe
  Kollisions-Typ waere dort mit dem Muster von bank9_zensus.py je Bank pruefbar.
* Interne r36-Integrationsstaende, in denen der B2/B1-Plan mit den alten Bits 90/91 genommen wurde, behalten ihr
  verschwundenes Stage-2-Item (Bit bleibt gesetzt); der Plan haengt dort wieder. Kein Nutzerstand betroffen (9.6).

### 9.9 Fuer den Nutzer
* Was war: die Lageplaene **B2** (ROOM1200) und **B1** (ROOM11D0) merkten sich "genommen" auf denselben Speicherplaetzen
  wie zwei Original-Gegenstaende in Stage 2 — die **Taschenuhr** in ROOM2090 und **"Head of Akuma"** in ROOM2040. Wer den
  Plan nahm, fand den Gegenstand spaeter nicht mehr; wer den Gegenstand zuerst nahm, hatte die Etagenkarte schon, ohne
  den Plan je gesehen zu haben.
* Jetzt: die beiden Plaene haben eigene, freie Plaetze (77/78). 3F/2F/1F waren nie betroffen.
* Spielstaende: unveraendert ladbar (gleiches Format). Kein veroeffentlichter Stand ist betroffen — die Lageplaene gab es
  in keiner bisher ausgelieferten Version (erst ab v0.8.24).
* Keine neuen Dateien, keine Sprachdateien, keine sichtbare Aenderung an den Karten selbst.
