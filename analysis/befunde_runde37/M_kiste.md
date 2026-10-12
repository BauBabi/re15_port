# Runde 37 — Spur M "kiste" (Item-Kiste: Symbole rechts, Rahmen ueber den Items)

Baum `.claude/worktrees/r37_kiste`, Zweig `r37/kiste`, Basis 70030492. Wortlaut: AUFTRAG.md Nachtrag 9.

## Wortlaut des Nutzers (Nachtrag 9, woertlich)

> In der Kiste müssen die Icons offenbar so weit wie möglich rechts liegen, damit sie nicht vom Text überdeckt werden.
> Außerdem ist mir aufgefallen, das beim scrollen/bewegen in der Kiste währendessen die Items oberhalb des Kastens liegen.
> Das darf nicht sein, der Kasten muss layertechnisch über den Items liegen.

Punkte:
- **M1** Symbole so weit rechts wie moeglich, kein Name ueberdeckt ein Symbol.
- **M2** Beim Scrollen liegt der Rahmen ("Kasten") layertechnisch ueber den gleitenden Item-Zeilen.

## 1. Messung vorher (exe dieses Baums, Stand 70030492)

Weg: echter Spielstand + echte Aktionstaste, kein Sprung. Kartenwerkzeug `probe_r37_kiste_karte` (neu, s. Tests)
schreibt ROOM1150, Leon bei (-25650,-12100) Gier 0 (Blick +x; Aktions-Vorwaertspunkt 620 vor der Figur,
aot_common.c "FORWARD-620", liegt im Kisten-AOT main00+0x70 `2C 05 03 31 00 00 F4 9D C8 CE 40 06 E8 03`
= x -25100..-23500 / z -12600..-11600). Kiste: Ring 63 = 0x15 x100, 0 = 0x44 "Minidisc Player w/ Disc",
1 = 0x0F Grenade Launcher x6 (2-Feld, Size 3), 2 = 0x43, 3 = 0x1D x12, 4 = 0x47, 5 = 0x46, 6 = 0x0B x3.
exe: `RE15_CONTINUE_TEST=1 RE15_CARD_AUTO=1 RE15_WINDOW_SCALE=1 RE15_PAD_AT=40:A,90:A,100:D,120:U
RE15_FRAMEDUMP=95-130/1:f` (A = Viereck: F40 Kiste oeffnen ueber das AOT, F90 Box-Seite, F100 RUNTER, F120 HOCH).

**M1 vorher** (Bild `M_belege/vorher_1150_zeilen_zoom.png`, F90, 4-fach): Symbol-Spalte ICON_X 158, Namen ab NAME_X 16.
"Minidisc Player w/ Disc" laeuft mit dem "c" IN das Symbol (Name liegt oben, Op-Liste: Name vor Symbol);
"Empty Grenade Shells" beruehrt es mit dem "s". Das Grenade-Launcher-Symbol (2-Feld, 49 px) reicht bis x 206 —
ueber die innere Rahmenlinie (x 204) an die Scrollschiene (x 206). Die Symbole haben einen DECKENDEN Grund
(ITEMALL-Kachel, CLUT-Index != 0), Ueberlappung = Rechteck-Ueberlappung.

Zensus ALLER 101 Namen der Namensbank (gen/inv_name_bank.inc, Glyphen aus DATA/TEX.TIM Spalten 256..511 wie
inv_render_pc.c, Vorschub include/font_width.h, Start NAME_X 16), rechtestes gemaltes Pixel:

| Id | Name | rechtes Pixel | vorher (Symbol ab 158) |
|---|---|---|---|
| 0x44 | Minidisc Player w/ Disc | 173 | ueberdeckt 16 px |
| 0x5B | K. Sullivan - R. Aiken | 171 | ueberdeckt 14 px |
| 0x59 | E. Dewey - K. Dooley | 164 | ueberdeckt 7 px |
| 0x5A | J. Frost - F. Speyer | 159 | ueberdeckt 2 px |
| 0x1D | Empty Grenade Shells | 158 | ueberdeckt 1 px |
| 0x43 | Communications Card | 155 | frei |
| 0x47 | Blue Master Keycard | 153 | frei |
| (2-Feld 0x0E..0x13) | Grenade Launcher | 137 | frei |

Dazu (andere Spur, nach der Integration im Spiel): Spur I "Empty Fire Extinguisher" (Id 0x31 bei Leon nach dem
Loeschen, r37/feuerloescher feuerloescher_r37.c k_name_leer) = 177.

**M2 vorher** (Bilder `M_belege/vorher_1150_runter_F100-107.png`, `vorher_1150_gleiten_kopf_fuss.png`):
RUNTER (RE2 Zustand 3, Versatz -3 je Bild): die Zeilen gleiten nach oben, "Minidisc Player w/ Disc" samt Symbol
liegt UEBER dem Kopfband "ITEM BOX"; unten waechst eine leere Luecke (die einlaufende Zeile fehlt). HOCH: "Red
Master Keycard" liegt ueber dem Fussband und dem grauen Pfeil. Pixelzaehlung `glide_check.py` (Pixel AUSSERHALB
des Listenfensters W = x 13..203 / y 52..152, innerhalb des Listen-Panels x 6..219 / y 25..180, Griffspalte
208..213 ausgenommen, gegen das Ruhebild F99):

| Bild | anders | davon oben | unten | seitlich |
|---|---|---|---|---|
| F102..F107 (runter, Versatz -3..-18) | 98 / 260 / 521 / 915 / 1319 / 1509 | bis 1417 | 0 | 48..92 |
| F108..F121 (Ruhe nach dem Schritt) | 98 | 0 | 0 | 98 (2-Feld-Symbol x 204..207 ueber dem Rahmen) |
| F122..F127 (hoch, +3..+18) | 136 / 295 / 691 / 1097 / 1317 / 1433 | 0 | bis 1391 | 42..86 |

(Anzeige 1 Bild versetzt: Taste in F100 -> Gleiten sichtbar F102..F107, Commit F108.)

Ursache im Code: re15_inv_screen.c build_box_mode — die Op-Liste wird von HINTEN gerastert (frueher Eintrag liegt
oben). Block 7b (Zeilen) kommt VOR dem Panel-Op (BOXPANEL.BIN mit Rahmen, Kopf-/Fussband UND Listengrund), also
liegen die Zeilen ueber dem ganzen Panel. Ein Fenster/Abschneiden gibt es nicht.

## 2. RE-Belege (RE2 Retail = Ziel; der Kisten-Schirm ist RE2, RE1.5 hat keinen)

Quelle: ghidra_re2_Leon.txt (Disasm), RE2_Quellcode_V2/FUN_800710dc.c / FUN_80072c6c.c / FUN_8007300c.c /
FUN_80031070.c. Panel-Ursprung boxX = DAT_800d5c40 = 7, boxY = DAT_800d5c42 = 14 (FUN_80070e58 Ende). Der Port
blittet das RE2-Panel 1:1 bei x 6 (Bild-Ursprung = RE2 (6,13)), also Port-x = RE2-x; Port-y = RE2-y + 12
(RE15_BOX_PANEL_DY).

### 2.1 Zeichenreihenfolge: der Rahmen liegt in RE2 UEBER Symbolen, Namen und Ziffern (OT-Reihenfolge, kein Clip)

- Menue-OT: 8 Eintraege je Puffer ab 0x800cc1e8 + buf*0x20. Hauptschleife: `08 00 05 24 li a1,0x8` @0x8002af34,
  `sll v0,v0,0x5 / addu v0,s0,v0 / sw v0,0x40(s0)` @0x8002af44-54 (DAT_800cc228 = 0x800cc1e8 + buf*0x20),
  `jal ClearOTagR` @0x8002af90 (umgekehrt verkettet: Eintrag i -> i-1). Gezeichnet ab dem LETZTEN Eintrag:
  `lw a0,0x40(s2) / jal DrawOTag / addiu a0,a0,0x1c` @0x8002bd54-5c (ebenso @0x8002b42c-34). Reihenfolge der
  GPU: Eintrag 7, 6, 5, 4, 3, 2, 1, 0 — ein spaeterer Eintrag liegt OBEN.
- FUN_800710dc (Box-Liste): `lui t1,0x800d / addiu t1,t1,-0x3e18` @0x80071118-1c (0x800cc1e8),
  `addiu v0,t1,0x8` @0x8007112c + `sll v1,a0,0x5` @0x80071148 -> Eintrag **2** (+8): die 4 Zeilenstreifen, beide
  Pfeile, die 5 Scrollmarken, die **16 Rahmen-Sprites** (Positionen @0x800a9c38, darunter Kopfband (6,6)+(134,6)
  je 20 hoch und Fussband (6,127)+(134,127)), Naht- und Auswahllinien — alle per AddPrim in Eintrag 2.
  `addiu v0,t1,0x18` @0x80071154 -> Eintrag **6** (+0x18): die Zeilen-SYMBOLE (POLY_FT4), eingehaengt
  `lw v0,0(t1)=>DAT_800cc200 ... sw` @0x80071784-a8 (einlaufende Zeile) und @0x80071900-3c (5 Zeilen).
- Namen: FUN_800693d0 -> FUN_80031070(x, y, 6|0x4000, id): Kopf `0x800e2ab0 + buf*0xc0 + ((6&7)*2)*0xc` = Kopf 12;
  FUN_80031bf0 (Hauptschleife @0x8002b298 nach ClearOTagR) haengt Kopf 2i/2i+1 in OT-Eintrag i
  (`jal AddPrim` @0x80031c3c / @0x80031c48, `addiu s1,s1,0x18`, `addiu s0,s0,0x4`) -> Kopf 12 = Eintrag **6**.
- Mengen-Ziffern: FUN_8007300c `AddPrim(&UNK_800cc1f4 + buf*0x20, ...)` -> Eintrag **3** (+0xc).
- Folge: GPU zeichnet Eintrag 6 (Symbole und Namen), dann 3 (Ziffern), dann 2 (Rahmen). (Die Reihenfolge
  Name/Symbol INNERHALB von Eintrag 6 haengt daran, wann FUN_80031bf0 die Textkoepfe relativ zum Kisten-Zeichner
  einhaengt — hier nicht bestimmt und fuer diese Spur ohne Belang, weil danach kein Name ein Symbol erreicht.) **Der Rahmen deckt alles zu, was beim Gleiten aus dem
  Fenster ragt.** Kein DrawArea/Clip im Box-Code (keine SetDrawArea/DR_AREA-Prim in FUN_800710dc/FUN_80072c6c).
- Kein Clip: SetDrawArea/DR_AREA kommt im Menue-Code (0x80068000-0x80080000) nicht vor (PutDrawEnv nur
  @0x8002b9b0/@0x8003274c in der Hauptschleife); die 12-Byte-Prims der Liste sind DR_MODE (`jal SetDrawMode`
  @0x8006f9b4 fuer 0x800d6bd8, Texturfenster NULL `sw zero,0x10(sp)` @0x8006f9ac).
- Das Fenster: Kopfband endet y 39 (boxY+6+20), Fussband beginnt y 141 (boxY+127); Listenstreifen x
  `addiu s6,t1,0xc5` @0x80071238 (rechte Kante boxX+0xc5 = 204, exklusiv), Auswahllinien x2
  `addiu v0,t1,0xc4` @0x80071adc (= 203, inklusiv). Gemessen in BOXPANEL.BIN: Listengrund (24,24,40) Bild-x 7..197,
  Bild-y 27..127 = RE2 x 13..203 / y 40..140 = Port x 13..203 / y 52..152.
- Gleitweite: Versatz DAT_800d5c15 +-3 je Bild, 6 Bilder (@0x80070118-24 / @0x800702a4-b0) = 18 px, Commit-Sprung
  2 px -> eine Zeile (20 px) verschwindet ganz unter dem 20 px hohen Kopf-/Fussband.

### 2.2 RE2 zeichnet beim Gleiten die EINLAUFENDE Zeile (Name, Symbol, Ziffern)

- Name: Zustand 2 (HOCH): Platz (scroll-1)&0x3f @0x80070128-48, `addiu a0,a0,0x7` @0x8007016c (x boxX+7),
  `addiu a1,a1,0x9` @0x80070174 (y boxY+Versatz+9 = Zeilenoberkante-20 +2); leer -> Id 0x64 @0x8007017c.
  Zustand 3 (RUNTER): Platz (scroll+5)&0x3f @0x800702b4-d4, `addiu a1,a1,0x81` @0x800702fc (y boxY+Versatz+0x81).
- Symbol: FUN_800710dc `local_50 = -0x14` (Zustand 2) / `100` (Zustand 3), `local_48 = -1 / 5`
  (Block vor LAB_800717ac @0x800716b8 `addiu a0,t1,0x1b`, @0x80071718 `addiu a2,t1,0x2e`), x wie die 5 Zeilen.
- Ziffern: FUN_80072c6c `li s7,0x52` @0x80072e48 (Zustand 3, Platz scroll+5), `li s7,-0x26` @0x80072e74
  (Zustand 2, Platz scroll-1), y = 70 + Versatz + s7.
- Der Port zeichnet nur die 5 Zeilen (re15_inv_screen.c `for (k = 0; k < RE15_BOX_WINDOW && k < 5; k++)`) — die
  einlaufende Zeile fehlt (Luecke im Vorher-Bild).

### 2.3 Lage von Symbol, Name, Ziffern in der Zeile

- Symbol: `addiu t1,t1,0x92` @0x800717d4 (x0 = boxX+0x92 = 153), Breite `li a3,0x30` @0x80071834 (Size != 0) /
  `li a3,0x18` @0x80071840, `addiu v1,v1,0x93` @0x80071884 (x1 = boxX+0x93+w, POLY-Kante exklusiv) -> schmal
  153..177 (25 px), breit 153..201 (49 px). Hoehe 0x1b..0x2e (19 px). RE2 setzt BEIDE Breiten an dieselbe linke
  Kante; das breite Symbol endet 2 px vor dem Fensterrand 203.
- Name: `addiu a0,t1,0x7` @0x800719b4 (x boxX+7 = 14), y = Zeilenoberkante + 2 (`addiu a1,s8,0x1d` @0x8007197c).
- Ziffern: FUN_80072c6c Tabelle @0x800aae2c..3f `bd ff ee ff / bd ff 02 00 / bd ff 16 00 / bd ff 2a 00 / bd ff 3e 00`
  = (-67, -18+20k), Ursprung (220,70) `li v0,0xdc / sh v0,0x34(s1)` @0x80074b88-8c, `li v0,0x46 / sh v0,0x36(s1)`
  @0x80074b90-94 -> x = 153 = linke Symbolkante, y = Zeilenoberkante + 11; FUN_8007300c Schritt 7 je Ziffer.
  **RE2 legt die Mengen-Ziffern ins Symbol (unten links), nicht daneben** — wie RE1.5s eigenes Inventar
  (FUN_80048f28: x Zelle+3, y Zelle+20, also ebenfalls unten links IM Symbol).
- Namen und Symbole teilen in RE2 den OT-Eintrag 6 (2.1); RE2s Proportionalschrift (Vorschub-Tabelle 0x8009de34, Flag 0x4000)
  bleibt vor x 153. Der Port nutzt RE1.5s breitere Schrift (bis 173 px) — deshalb ueberdeckt der Name dort das
  Symbol. Die Abhilfe ist die LAGE (kein Name erreicht das Symbol), nicht die Ebene.
- Gegenprobe am Bild des laufenden RE2 (analysis/itembox_re2/referenz/re2_itembox_original.png, Ausschnitt
  `M_belege/re2_referenz_zeilen_zoom.png`): schmale (Spraydose) und breite Symbole (Waffen) beginnen gemeinsam bei
  x 153, die Mengenanzeige (gruenes Unendlich der Waffen) steht unten links IM Symbol.

## 3. Umsetzung

Dateien: `re15_port/engine/src/re15_inv_screen.c` (Block 7b, Haken "Runde 37 Spur M"), `re15_port/include/re15_kiste_r37.h`
(neu, alle Konstanten mit Beleg), `re15_port/include/re15_inv_screen.h` (+ Op `RE15_INV_OP_PANELTEIL = 7`),
`re15_port/platform/pc/src/inv_render_pc.c` (+ Rasterzweig PANELTEIL, 16 Zeilen). Keine neuen Assets.

| Konstante (re15_kiste_r37.h) | Wert | Beleg |
|---|---|---|
| Fenster Bild-x 7..197 / Bild-y 27..127 | Port x 13..203 / y 52..152 | boxX+6; boxX+0xc4 `addiu v0,t1,0xc4` @0x80071adc; Kopfband (6,6)+20 / Fussband (6,127) aus @0x800a9c38; BOXPANEL.BIN Listengrund gemessen |
| RE15_KISTE_SYMBOL_RECHTS | 204 (exklusiv) | boxX+0xc5 `addiu s6,t1,0xc5` @0x80071238 |
| RE15_KISTE_ZIFFER_DX / _DY | 0 / 11 | FUN_80072c6c Tabelle @0x800aae2c (-67,-18+20k) + Ursprung (220,70) @0x80074b88/@0x80074b90 |
| RE15_KISTE_EINLAUF_HOCH / _RUNTER | -1 / 5 | Name @0x80070128-74 / @0x800702b4-0304; Symbol local_50 -0x14 / 100; Ziffern `li s7,-0x26` @0x80072e74 / `li s7,0x52` @0x80072e48 |
| Rahmen-Teile (oben/unten/links/rechts) | Panel-Bild ohne Fenster | RE2 Rahmen OT-Eintrag 2 ueber 6/3 (§2.1) |

**M2 (Rahmen ueber den Items).** Op-Reihenfolge im Block 7b jetzt (frueher = oben): Auswahl-Band -> Scrollgriff ->
**vier Rahmen-Teile** (PANELTEIL, Panel-Bild ohne Listenfenster, 1:1 an derselben Stelle) -> Zeilen -> (danach)
Textbox und voller Panel-Op mit dem Listengrund. Damit liegt der Rahmen wie in RE2 ueber Symbolen, Namen und
Ziffern; was beim Gleiten aus dem Fenster ragt, verschwindet unter Kopf- bzw. Fussband (beide 20 px = eine Zeile).
Der Griff steht VOR den Rahmen-Teilen, weil RE2 die Scrollmarken vor den 16 Rahmen-Sprites in Eintrag 2 einhaengt
(AddPrim vorn -> Marken ueber der Schiene). Die EINLAUFENDE Zeile (RE2 Zustand 2: Platz scroll-1 als Zeile -1,
Zustand 3: scroll+5 als Zeile 5) wird mitgezeichnet (Name, Ziffern, Symbol), solange der Versatz != 0 ist; bei
Versatz 0 laege sie vollstaendig unter dem 20 px hohen Band (Port y 33..51 unter 32..51 bzw. 153..171 unter
153..172), das Bild ist dort identisch. Leere Plaetze bleiben leer wie die 5 Zeilen (RE1.5 kennt RE2s "leer"-Text nicht).

**M1 (Symbol so weit rechts wie moeglich).** Jedes Symbol endet buendig am rechten Fensterrand: x = 204 - Breite
(schmal 25 px -> x 179..203, 2-Feld 49 px -> x 155..203). NUTZER-VORGABE fuer die schmalen Symbole (RE2 setzt
beide Breiten an dieselbe linke Kante 153 und laesst schmalen Symbolen 24 px Luft); die Grenze rechts ist RE2s
Fensterkante boxX+0xc5. Die Mengen-Ziffern ziehen mit ins Symbol, unten links und ueber dem Symbol, wie RE2
(FUN_80072c6c: x = linke Symbolkante, y = Zeilenoberkante + 11) und wie RE1.5s eigenes Inventar (Ziffern unten
links im Symbol, FUN_80048f28) — rechts neben dem Symbol ist kein Platz mehr (vorher x 188..206, jetzt Fensterrand).
Namen unveraendert ab x 16.

Unveraendert (Riegel C, Bildvergleich §4): Auswahl-Band (x 12..203, y 92/112, Farben), Scrollgriff (Lage/Farbe),
Textbox, Inventar-Gitter rechts, Cursor, Namens-x 16, Kopf-/Fussband-Pixel.

## 4. Messung nachher (exe dieses Baums, gleicher Weg wie §1)

Drei Raeume, je Spielstand + echte Aktionstaste am Kisten-AOT (kein Debug-Oeffnen): **Leon ROOM1150**, **Elza ROOM1151**
(Kisten-AOT slot 5 main00 @0x00DCE bytegleich zu 1150: `2c0503310000f49dc8ce4006e803ff0018070000`), **Leon ROOM2010**
(slot 5 main00 @0x007A8 `2c0503310000420e38e64006e803ff0018030000` = x 3650..5250 z -6600..-5600, Figur (3050,-6100)
Gier 0; Message_on msg 2 in sub03 @0x008F0 = Kisten-Meldung laut re15_itembox.c). Gleiche Kisten-Belegung.

| Messung | 1150 Leon | 1151 Elza | 2010 Leon |
|---|---|---|---|
| glide_check.py F100..F129 (Pixel ausserhalb des Fensters anders, gegen F99) | 0 in allen 30 Bildern | 0 | 0 |
| probe_r37_kiste_bild: Rahmen bitgleich BOXPANEL.BIN (x 12..214) | 31/31 Bilder | 31/31 | 31/31 |
| Ruhebild F99: Symbol ab x 179 (schmal) / 155 (2-Feld), Spalte davor Listengrund | 5/5 Zeilen | 5/5 | 5/5 |
| F107 einlaufende Zeile von unten / F127 von oben (Symbolpixel im Feld) | 325 / 325 | 325 / 325 | 325 / 325 |

Gegenprobe vorher (Bilder aus §1 mit demselben Bildwerkzeug): F99 Rahmen verletzt (49 px: 2-Feld-Symbol x 204..206
auf Rahmenlinie/Schiene), Zeilen 0/2/3/4 "Spalte vor dem Symbol NICHT frei", 2-Feld-Symbol nicht ab x 155, F102..F107
bis 3177 px, einlaufende Zeile oben/unten je 0 px -> KISTE-BILD FEHLER 38.

Unveraendert (Ruhebild F99 vorher gegen nachher, gleicher Spielstand): **0 Pixel** ausserhalb des Listen-Panels
(Textbox, Inventar-Gitter, Cursor, Chrome, Hintergrund) und 0 Pixel in den aeusseren Panel-Spalten x 6..11 / 215..219
(dort liegen Menue-Chrome bzw. Inventar-Gitter ueber dem Panel — deshalb prueft das Bildwerkzeug x 12..214).

Bilder: `M_belege/nachher_1150_zeilen_zoom.png` (F99, 4-fach: "Disc" endet x 173, Symbol ab 179; Grenade Launcher
155..203 im Fenster; "6"/"12" unten links im Symbol), `nachher_1150_gleiten_F104.png` (Versatz -9: "Minidisc..." unter
dem Kopfband, "Red Master Keycard" kommt unter dem Fussband hervor, Schriftzug "ITEM BOX" unversehrt),
`nachher_1150_gleiten_sheet.png` (F100/102/104/106/107/108/124/127), `nachher_1151elza_2010leon_F99_F104_F125.png`.

Zensus nachher (unit_r37_kiste, alle Ids 1..255, drei Spielzustaende): 101 Namen; laengster 0x44 "Minidisc Player w/
Disc" bis x 173, schmales Symbol ab 179 -> kleinste Luecke **5 px**; 2-Feld-Namen bis x 137, Symbol ab 155 -> 17 px.
Nach der Integration mit Spur I: "Empty Fire Extinguisher" (Leon, nach dem Loeschen) bis x 177 -> Luecke 1 px
(rechnerisch aus k_name_leer r37/feuerloescher; der Zensus-Zustand "alle Flags Leon" setzt (3,0x81) und erfasst den
Namen dann automatisch).

## 5. Tests (probes/r37_kiste.cmake, nur dort registriert)

- **unit_r37_kiste** (test_r37_kiste.c, Engine + echte Kisten-FSM, 0,2 s): (A) Zensus ALLER Item-Ids 1..255 in drei
  Spielzustaenden (Anfang / alle Flags + Leon / alle Flags + Elza): 101 Namen, rechtestes GEMALTES Namenspixel
  (TEX.TIM-Glyphen wie inv_render_pc.c) links vom Symbol, Symbol buendig am Fensterrand 204, Ziffern im Symbol
  (y Zeilenoberkante+11) und in der Op-Liste ueber ihm, Namen ab x 16; (B) 57 Bilder (Ruhe + 3x RUNTER + 3x HOCH +
  HOCH ueber die Ring-Naht 0 -> 63): vier Rahmen-Teile = Tabelle, 1:1 auf dem Panel, Teile + Fenster = Panel;
  Reihenfolge Band < Griff < Rahmen-Teile < alle Zeilen-Ops < Panel-Grund; jede Zeilen-Op im Panel; Symbole
  waagerecht im Fenster; Versatz -+3 je Bild (RE2 @0x80070118/@0x800702a4); einlaufende Zeile in 24 HOCH- und 18
  RUNTER-Gleitbildern; (C) Auswahl-Band, Scrollgriff, Panel-Ops unveraendert.
  **Gegenprobe**: derselbe Riegel gegen den alten Block 7b (re15_inv_screen.c aus 70030492, nur Testziel neu
  gelinkt): exit 1, 1138 FAIL-Zeilen (Symbol x 158..182 nicht am Rand, Ziffern y 98 statt 104, Name -16 px im
  Symbol, keine Rahmen-Teile, keine einlaufende Zeile); (C) gruen = Band/Griff/Panel waren schon so.
- **integration_r37_kiste** (echte exe, 58 s): drei Laeufe Leon 1150 / Elza 1151 / Leon 2010 auf dem echten Weg
  (Spielstand, CONTINUE, Viereck am Kisten-AOT), Framedumps F95..F130 -> probe_r37_kiste_bild: 31/31 Bilder Rahmen
  bitgleich BOXPANEL.BIN, Ruhebild Symbol ab 179/155 mit freier Spalte davor, einlaufende Zeile F107/F127.
  Gegenprobe mit den Vorher-Bildern: KISTE-BILD FEHLER 38.
- Werkzeuge: probe_r37_kiste_karte (Spielstand vor der Kiste, Kiste mit den laengsten Namen), probe_r37_kiste_bild.

**Gezielte Laeufe** (keine volle Suite; Regex = eigene Tests + alle Testgruppen des Inventar-/Kisten-/Karten-
Schirms, dessen Zeichner re15_inv_screen.c + inv_render_pc.c der Diff beruehrt; -j 4, Maschine unter Last
paralleler Spuren):

- Lauf 1 (Bau nach 6382fb63): `ctest -R "r37_kiste|itembox|inv_|inventar|map_|karte|dokument|dachkiste|kartenitems"
  --timeout 900 -j 4` -> `100% tests passed, 0 tests failed out of 71` (Total Test time (real) = 1563.04 sec).

## 6. OFFEN (nicht Teil von Nachtrag 9, nicht angefasst)

- Vorbestehend: das 2-Feld-Symbol der Kisten-Liste (box_cell_icon_sized, Breite 49) tastet in inv_render_pc.c die
  SCHMALE ITEMALL-Kachel (40 Texel) ab; Spalten 40..48 wiederholen Spalte 39 (sichtbar als verlaengerter Lauf rechts,
  `M_belege/nachher_1150_zeilen_zoom.png` Zeile 2). RE2 zeichnet das breite Zeilensymbol aus ZWEI 20x30-Cache-Spalten
  (u1 = 0x78 = 120 Texel, `li t1,0x78` @0x8007183c im Delay-Slot von `j` @0x80071838, `sb t1,-0xe(s0)` @0x80071848; zweite Kachel fuer Id 14..18 in
  FUN_80069c40 @0x80069cd8-d64). Der Port hat die 80x30-Kunst (ITPS, Pfad w==80 im ITEMTILE-Zweig). Lage und Ebene
  (Nutzer-Punkte) sind davon unabhaengig — Kandidat fuer eine eigene Nachbesserung, wenn der Nutzer es wuenscht.
- Integration: Spur I ("Empty Fire Extinguisher", 177 px) passt mit 1 px Luecke vor das schmale Symbol (179); der
  Zensus-Zustand "alle Flags Leon" deckt den Namen nach dem Merge automatisch ab. Das Include von re15_kiste_r37.h
  steht neben re15_itembox.h (Spur I fuegt nach re15_schluessel_r36.h ein) — `git merge-tree` gegen r37/feuerloescher:
  re15_inv_screen.c automatisch zusammengefuehrt, Konflikt nur in AUFTRAG.md (master-Fassung nehmen).
- Elza "vorher" nicht eigens gemessen: build_box_mode (re15_inv_screen.c Z. 1618..1888) und re15_itembox.c haben
  keinen Figuren-/Raum-Zweig (grep char_variant/character/g_gameflow/current_room: 0 Treffer) — Leons Vorher-Messung
  ist derselbe Codeweg; nachher sind Leon UND Elza gemessen (§4).

## 7. Fuer den Nutzer

- Neue Assets: **keine** (Paket-Gate unveraendert; BOXPANEL.BIN wird nur zusaetzlich ausschnittweise gezeichnet).
  Sprachdateien: keine.
- Sichtbar neu in der Kiste:
  1. Die Symbole stehen ganz rechts, buendig am rechten Rand der Liste (schmale ab x 179, Waffen ab x 155); der
     laengste Name ("Minidisc Player w/ Disc") endet 5 px davor — kein Name ueberdeckt mehr ein Symbol.
  2. Die Mengenzahl steht jetzt unten links IM Symbol (so zeigt es RE2 in der Kiste und RE1.5 in der Item-Liste
     rechts) — rechts neben dem Symbol ist kein Platz mehr.
  3. Beim Blaettern gleiten die Zeilen UNTER den Rahmen: oben verschwinden sie unter dem "ITEM BOX"-Band, unten unter
     dem Fussband, und die naechste Zeile kommt wie in RE2 unter dem Band hervor (vorher blieb dort eine Luecke).
- Bedienung unveraendert. Keine Entscheidungsfrage.
