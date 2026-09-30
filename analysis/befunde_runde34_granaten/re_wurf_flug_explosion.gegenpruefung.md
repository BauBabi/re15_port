# Gegenpruefung (Skeptiker) — re_wurf_flug_explosion.md

Stand: 2026-09-29, ABGESCHLOSSEN (fortlaufend gespeichert). Pruefer: Gegenpruefungs-Agent Runde 34.
Methode: jede Zeile der Tabelle "KONSTANTEN FUER DEN BAU" und jede Mechanismus-Behauptung selbst
disassembliert (`re15_disasm.py`, `re2_disasm.py`), Bytes selbst gelesen, Sprungziele selbst verfolgt.
Werkzeuge: `re_wurf_gegen_werkzeug/`, Ausgaben: `build/r34g_wurf_gegen/`.

## Gliederung
A. Routine 30 (Wurf-Init) — Konstanten, Zweige, Delay-Slots
B. RNG FUN_8001af20 + Abprallzaehler
C. Routine 29 (Flug/Abprall/Liegen)
D. Routine 31 (Zuender/Explosion/Kinder)
E. Spawn im Waffen-FSM (Gate, Bilder, Versaetze, Anker, Gier, Clip)
F. ESP-Tick FUN_80019e20 (Reihenfolge, Weltlage, Physik, Anim, Flags)
G. FUN_80019700 / FUN_800199d4 (Spawner, CLUT, Flags 0x0a)
H. CORE00.ESP Effekt 4 sub 0x0D + Effekt 3 Kinder (Datei-Bytes)
I. SE FUN_80045024 (Bank/Satz/Lage)
J. Flaechenschaden FUN_80012d60 / FUN_8002b5d0 (Aufrufsemantik, Tabellen)
K. Licht-Latch 0x800b5358
L. Takt / Aufrufreihenfolge
M. Hand-Netz nach dem Wurf
N. Vollstaendigkeit (fehlende Mechanismen)
O. RE2-Gegenprobe
Nachtraege (in Schreibreihenfolge): F.2 Anker-Matrix/Zeitpunkt, C.2 Zeitlinie, H.2 Farben live
Tabelle: Urteil je Konstante / PORT-ABGLEICH-Stichproben / Fazit / Werkzeuge
**Kurzurteil:** Kern bestaetigt; WIDERLEGT: Spieler-Trefferzonen-Versatz (0,-1530,0 statt 0) und die Takt-Begruendung;
FEHLEND fuer den Bau: Gegner-Gates FUN_80012d60, Drehung/Abbruch im Wurf, Raumwechsel-Loeschen (Abschnitt N).

## A. Routine 30 @0x8001843c — BESTAETIGT

Selbst disassembliert (`dis 0x8001843c 68`, `table 0x80071d40 48`: [29]=0x80018320, [30]=0x8001843c,
[31]=0x8001854c, [0]=0x80017248 = `jr ra`/`nop` = No-op — noetig, weil R30 +0x00 := 0 setzt).
Jede Konstante und jeder Delay-Slot stimmt:
* +0x6e := 0x17 (`ori v0,zero,0x17` @0x80018448, `sb v0,110(v1)` @0x80018450), +0x6c := 3 @0x8001845c/60,
  +0x02 := 0x1d @0x8001846c/70, +0x00 := 0 (`sh zero,0(v1)` @0x80018478), +0x1e := 0x2a (`ori` @0x80018474,
  `sh v0,30(v1)` @0x8001847c).
* HOCH: 0x17c/-110/0x15 nach +0x10/12/14; `j 0x800184d8` @0x800184ac mit Delay `addiu v0,zero,-2` @0x800184b0;
  +0x08 := v0 im Delay-Slot des `jal 0x8001af20` @0x800184d8/dc -> HOCH acc_x = -2, MITTE -1 (Delay @0x800184d4
  faellt durch). Die Delay-Slots der `beq` @0x80018490/b8/80018514 (`ori v0,..`) werden im Nicht-Sprung-Pfad
  gebraucht und im Sprung-Pfad sofort ueberschrieben -> kein Seiteneffekt.
* TIEF: +0x10 := 0x50, +0x14 := 1, +0x08 := -1, +0x12 := 0 (`sh zero,18(v1)` @0x80018534), +0x26 := 5 @0x80018538.
* Rest: `bgez v1` @0x800184ec / `sra`/`sll`/`subu` = C-Rest %4; RNG-Rueckgabe ist `andi v0,a0,0xff` >= 0, also
  %4 == &3. +0x26 := rest+7 im Delay-Slot `sh v0,38(a0)` @0x8001850c (a0 = Slot neu geladen @0x800184e4/e8).
* Kein Bit gesetzt -> keine Schreibung auf +0x10/12/14/08/26 (Zeilenwerte bleiben). Korrekt beschrieben.

## B. RNG FUN_8001af20 — BESTAETIGT (mit Einschraenkung zur Bitbelegung, s. B.2)

`dis 0x8001af20 16` + `bytes`: `lhu t1,0(v0)` @0x8001af28 (t1 danach nie gelesen), `srl v1,a0,7`
(Bytes `c2 19 04 00` = SPECIAL rs=0 rt=a0 rd=v1 sa=7 funct=srl), `addu a0,a0,v1` (`21 20 83 00`),
Rueckgabe `andi v0,a0,0xff` @0x8001af4c. a0 zwischen `lhu a0,-13588(a0)` @0x80018484 und `jal` @0x800184d8
unveraendert (nur v0/v1-Schreiber dazwischen). Tabelle 0x8000->7, 0x4000->7, 0x8002->9, 0x4002->9 nachgerechnet.

### B.2 Einschraenkung "7 gesund / 9 vergiftet" (Pruefung der uebrigen acaec-Bits folgt in Abschnitt N)

## C. Routine 29 @0x80018320 — BESTAETIGT

`dis 0x80018320 72`: `lh t1,42(t0)` / `blez t1,0x8001842c` @0x80018330/38 (Delay `nop`); `lhu v0,38(t0)` /
`bne v0,zero,0x80018388` mit Delay `lui v1,0x5555` @0x8001834c (im Liegen-Pfad wirkungslos, v1 wird @0x80018360
neu geladen). Liegen: a0 = 0x010A0001 (`lui a0,0x10a`/`ori a0,a0,0x1`), `jal 0x80045024` @0x80018358, Delay
`addiu a1,sp,16`; zwischen `addiu sp,sp,-40` @0x80018328 und dem jal wird NUR `sw ra,32(sp)` geschrieben ->
sp+16..27 ist im Liegen-Pfad wirklich unbeschrieben (Befund bestaetigt). Danach +0x6c := 0x63, +0x00 := 0x1f,
+0x02 := 0 (`sh zero,2(v1)` im Delay-Slot von `j` @0x80018380/84).
Abprall: Division /3 per `0x55555556`-mult/mfhi minus Vorzeichen (`sra a1,a1,31` / `subu a2,a2,a1`) = C-Division
(auf 0 gerundet); vx := vx - vx/3 (`subu a3,a3,a2` / `sh a3,16(t0)`), Zaehler-1 (@0x800183d0/d4),
+0x38 := +0x38 - t1 (`lw v0,56(t0)` @0x800183c4, `subu v0,v0,t1` @0x800183dc, `sw` @0x800183e0), vy := -(vy/3)
(@0x800183e4-f8), sp+16/20/24 := lh +0x28/+0x2a/+0x2c (s16 sign-extended), a0 = 0x010A0001 | (Zaehler_neu << 8)
(`or a0,a0,v1` im Delay-Slot @0x80018428). +0x14 (vz) wird nirgends geschrieben — bestaetigt.
Hinweis Bau: `sll a0,a0,8` auf `lhu` -> fuer n <= 9 nur Byte1 betroffen.

## D. Routine 31 @0x8001854c — BESTAETIGT (inkl. Delay-Slots)

`dis 0x8001854c 104`: `beq v1,zero,0x80018688` (Delay `ori v0,zero,0x7`), `bne v1,v0,0x800185f4` (Delay
`ori v0,zero,0x1` = Latch-Wert). Zuender 7: Latch @0x8001857c, Flags 0x61 @0x80018584, P = (lh +0x28, lh +0x2a - 500,
lh +0x2c) nach sp+16/20/24 (letzte Schreibung `sw v0,24(sp)` im Delay-Slot des `jal 0x80012d60` @0x800185b8/bc),
a0 = 0x1f4, a2 = 2. Kind 0x03195000 (a2 = 0x80072d4c, a1 = `lh a1,46(v0)`, a3 = sp+16 im Delay @0x800185e0),
SE 0x04080001 (a1 = sp+16 im Delay @0x800185f0). Zuender 2 (@0x80018600-64): zwei Kinder 0x03195000/0x030B5400,
a2 = s0 = 0x80072d4c. Abzug @0x80018668-84 (`sh v0,30(v1)` im Delay von `j 0x800186d0`). Zuender 0 (@0x80018688-cc):
`sb zero,108(a1)` @0x800186b0 VOR dem Kind-Spawn 0x030B5800 @0x800186c8.
**Zusatzbefund:** P auf sp+16 bleibt ueber alle drei Aufrufe intakt: FUN_80012d60 schreibt nie ueber s5 (=a1)
(`grep` auf `s[wbh] ..(s5)` in `build/r34g_wurf_gegen/dis_80012d60.txt`: 0 Treffer, nur `lw a1,0(s5)`/`lw a2,8(s5)`),
FUN_8002b5d0 schreibt nie ueber s6 (=a1), FUN_80019700/FUN_800199d4 lesen a3/a2 nur (`lw` @0x800197e8-f4 bzw.
@0x80019820-5c). -> Kind und SE sitzen wirklich auf P.
**Zusatzbefund:** Weil `sb zero,108(a1)` vor dem Spawn liegt und FUN_800199d4 den ERSTEN freien Platz nimmt
(Suche ab Index 0, @0x8001978c-c4 bzw. Gegenstueck), kann Rauch #2 den Granaten-Platz selbst belegen.
Verhalten gleich; fuer den Port nur relevant, falls er Platz-Reihenfolge nachbildet.

## F. ESP-Tick FUN_80019e20 — BESTAETIGT

`dis 0x80019e20 420` (`build/r34g_wurf_gegen/dis_80019e20.txt`): Pause-Gate `lw v0,-13760(v0)` & 0x10000000
@0x80019e28-40; Pool s0 = 0x800a73b8, Ende s2 = s0+12672 (= 96 x 0x84). Schleife 1 @0x80019e64-c4: Flags&1 ->
`jalr` Tabelle[+0x00]. Hauptlauf ab @0x80019ee0: (a) Flags&8 -> `xori v0,v1,0x9` (Delay @0x80019efc) + Routine A;
(b) Flags&1 sonst weiter @0x80019f50, Flags&4 -> Matrix aus *(+0x74) @0x80019f60-a4; (c) Flags&0x80 == 0 ->
@0x8001a118: Einheitsmatrix nach Scratch 0x1f800000, w = (+0x20, +0x22 + +0x2e, +0x24) (u16-Addition @0x8001a184),
`jal 0x80068098` (RotMatrix), xlat lo16 (+0x34/+0x38/+0x3c) -> `jal 0x800661c0` (Ausgabe VECTOR s32 @0x1f800020/24/28),
+0x28/2a/2c := r (lo16); zweites ApplyMatrix mit Slot+0x4c auf Versatz +0x40/44/48 (lo16); +0x28 += r2 + (+0x60)
usw. (@0x8001a250-2a4). (d) Routine B @0x8001a2b4-d4 (fuer JEDEN aktiven Platz, B=0 -> Tabelle[0] = `jr ra`).
(e) Flags NEU gelesen nach Routine B (`lbu v0,108(a2)` @0x8001a2e8) & 0x20 -> Physik aus; sonst Euler += +0x18..,
xlat(s32) += (s16)vel, DANACH vel += acc (+0x08/0a/0c) @0x8001a2fc-388. (f) Anim: Zeitgeber +0x6d != 0 -> -1;
== 0 -> (Flags&0x40 ? Satz bleibt : Satz+1), Satz mit Byte2==0 && Byte0==0 -> Flags := 0 (@0x8001a408/0c),
Byte2 == 0xff -> Satz := Byte0; Zeitgeber := Byte2 - 1.
Richtungskonvention selbst geprueft: RotMatrix @0x80068098 bei (0,y,0): m[0][0] = cos y (`sh t6,0(a1)` @0x8006820c),
m[0][2] = sin y (`sh t6,4(a1)` @0x8006816c), m[2][0] = -sin y (`sh t6,12(a1)` @0x800682a4, t8 = cos z·(-sin y));
RotMatrixY @0x800659d0 (Geh-Schritt) liefert dieselbe Matrix (t1 = -sin, Zeile0' = c·Zeile0 + s·Zeile2). Geh-Schritt
FUN_800245d8: Vektor (+0x8c, 0, 0) @0x800245f0-600, Winkel `lh v0,106(v0)` + a0 @0x80024658/64. -> lokal +x = vorwaerts.

## G. Spawner FUN_80019700 / FUN_800199d4 — BESTAETIGT

Instruktionsvergleich beider Funktionen (relativ normierte Sprungziele): genau 1 Unterschied,
`ori v0,zero,0x3` @0x800197b4 vs `ori v0,zero,0xa` @0x80019a88. FUN_80019700: +0x6c := 3 (Delay-Slot-Wert),
+0x70/71/72 := Kategorie/sub/Skala @0x800197d4-dc, +0x2e := a1 (`sh s2,46(t0)` im Delay @0x800197e4),
+0x40/44/48 := a3[0..2] (4 Worte kopiert, das 4. landet auf +0x4c und wird von der Matrixkopie ueberschrieben),
+0x74 := a2, +0x4c..+0x6b := 8 Worte ab a2, xlat und +0x28.. := 0, CLUT = Kopf+4 + (sub>>3)<<6 (`sll s0,v1,6`
@0x80019754, `sh v0,50(t0)` @0x80019888), TPAGE = Kopf+6, +0x78 = Kopf+8, +0x6e := 1 @0x800198a4,
+0x6d := Byte @Kopf+10 (= Satz0.Byte2) @0x800198bc, +0x7c = Kopf+8+ca*8, erste 40 Zeilenbytes -> +0x00..+0x27.
Platzsuche linear ab Index 0 (`sltiu v0,t3,0x60`), erster Platz mit Flags == 0.

## H. CORE00.ESP — BESTAETIGT (eigener Parser nach FUN_8001945c)

Werkzeug `re_wurf_gegen_werkzeug/esp_zeilen_gegen.py`. Loader-Logik selbst disassembliert: FUN_8001945c
(@0x8001948c-ec) liest Id-Bytes vom Dateianfang, Zeiger rueckwaerts vom Dateiende, setzt 0x800b2248[id] = Kopf,
0x800b22d4[id] = Kopf + (2·ca + cb + 2)·4. Datei `shared_assets/PSX/DATA/CORE00.ESP` == `info/Re1.5/PSX/DATA/CORE00.ESP`
(`cmp`). Id-Liste `03 08 00 02 04 ff ff ff`. Alle Dossier-Werte nachgelesen:
| Was | Offset | Bytes (selbst gelesen) |
|---|---|---|
| Eff4-Kopf | 0x1728 | ca 36, cb 28, CLUT 0x7AD1, TPAGE 0x001F |
| Eff4 Sub-Tab-Eintrag sub&7=5 | 0x18CA | 0x007C -> Strom 0x1AB0 `01 00 00 00 02 00 00 00` |
| Eff4 Zeile 0 | 0x1AB8 | `1e 00 00 00 00 10 00 10 00 00 0a 00` + 28x`00` (A 30, acc_y 10) |
| Eff4 Zeile 1 | 0x1AE0 | 40x `00` |
| Eff4 Saetze 23..35 | 0x17E8..0x1848 | `10/11/12/14/15/16/17/16/15/13/12/11 01 01 10`, 35 = `17 01 ff 10` |
| Eff4 Koord 16..23 | 0x1890.. | (0,112)…(80,112), 22 = (96,88), 23 = (96,104), alle dx/dy -8 |
| Eff3-Kopf | 0x8 | ca 29, cb 163, CLUT 0x7811, TPAGE 0x001E, Sub-Tab 0x384 |
| Eff3 sub 0x19 | 0x386 = 0x1A -> 0x3EC | Zeile0 @0x3F4 A10 +0e 0x13 +26 10; Zeile1 @0x41C Nullen (w/h 0x1000) |
| Eff3 sub 0x0B | 0x38A = 0x50 -> 0x4C4 | Zeile0 @0x4CC A10 acc_y 5 +0e 0x13 vel_y -130 +16 0x40 +26 8; Zeile1 @0x4F4 acc_y 5 vel_y -135 |
| Eff3 Satz 10..22 / 23 | 0x60..0xC0 / 0xC8 | Dauer je 1 (Byte2), 23 = `00 00 00 00` -> Platz frei |
**Praezisierung:** Byte1 der Anim-Saetze ist die TEILEZAHL (Eff3: 6/9/9/9/9/12/12/16/16/9/9/9/6 Zellen je Bild ab
Koord Byte0) — Feuerball und Rauch sind MEHRTEILIGE Sprites; die Granate hat je Bild genau 1 Zelle (Byte1 = 1).
Fuer den Bau: der Zeichner muss Byte1 Zellen je Satz ziehen — der Port tut das bereits (main.c:335 "byte1 = quads this frame").
Rauch beginnt bei Satz 8/9 mit je 4 Zellen (Byte1 = 4), danach wie der Feuerball.

## E. Spawn im Waffen-FSM — BESTAETIGT

`dis 0x80033440 240` (`build/r34g_wurf_gegen/dis_80033440.txt`). Gate `ori v0,zero,0x9` @0x80033688 / `bne v1,v0,0x800337ac`
@0x8003368c (v1 = `lbu 0x800aca5d`). Bild-Vergleiche gegen `lbu v1,-13591(v1)` (0x800acae9): 0x13 (Delay @0x80033690),
0x16 (Delay @0x800336a4 und @0x800336fc), 0x18 @0x80033758. Zielbits `andi 0x8000/0x4000/0x2000` @0x800336b4/@0x80033714/
@0x80033770 auf `lhu 0x800acaec`. a0 = `lui a0,0x40d` (Delay-Slot der `beq` @0x800336bc/@0x8003371c/@0x80033778) +
`ori a0,a0,0x1000`; a1 = `lh a1,-13634(a1)` (0x800acabe); a2 = `lw a2,-13348(a2)` + 1956 (Delay-Slot des `jal`);
a3 = sp+16 mit {0,0x12c,0x320} (HOCH: `sw zero,16(sp)` @0x800336e4, `sw v0,20(sp)` = 0x12c @0x800336dc, `sw v0,24(sp)` =
0x320 @0x800336e8), {0,0,0x1f4} (MITTE), {0,0,0x12c} (TIEF). Clip: `srl v0,a0,15; sll v0,v0,1; addiu v0,v0,7; srl a0,a0,11;
andi a0,a0,0x4; addu` @0x800334a8-bc, `sb v0,-13592(at)` (0x800acae8) @0x800334c8 -> 9/7/11 nachgerechnet.
Entlade-Tabelle `table 0x80074100`: [9] = 0x80033b38, [10] = 0x80033b58, [11] = 0x80033b78 — alle drei nur
`jal 0x8004eae4` (Zaehler -1, `sb v0,0(at)` @0x8004eb60, Rueckgabe 0 bei Menge 0, sonst 1; die Rueckgabe wird im
FIRE-Init @0x800334e0 NICHT ausgewertet).
Nebenbefund: In der Treffer-Tabelle je Waffe 0x8006e548 (`table 0x8006e548 14`) zeigen [9]/[10]/[11] auf eine eigene
Funktion 0x800128a0 — sie wird aber nur aus FUN_80011f50 (Schuss-Treffer) erreicht, das der Granaten-FIRE-Pfad nicht ruft.

## I. SE FUN_80045024 — BESTAETIGT mit Praezisierungen

`dis 0x80045024` (`build/r34g_wurf_gegen/dis_80045024.txt`): Bank = a0>>24 (`srl v1,a0,24` @0x80045028), VAB-Handle
`lb a1` 0x800b21ec[Bank] (-1 -> Ende), Satz = (a0>>16)&0xff (@0x80045078/7c), Byte0 -> sp+40 (@0x80045080/9c), Byte1
nirgends gelesen (a0 @0x80045080 ueberschrieben) -> `n<<8` wirkungslos: bestaetigt. Sprungtabelle `table 0x80010e70 6`:
[1] -> 0x800450d0 (a0 = 0x801fcd00, Satz < 0x21), [4] -> 0x8004511c (a0 = 0x801fbd00, Satz < 0x21).
Bank-Lader selbst geprueft: FUN_80043d8c laedt Datei-Id `0x8007492c[Waffe]` nach 0x801fcd00 und legt den Handle nach
0x800b21ed (= Bank 1); FUN_800440c4 laedt `0x80073a88[aca5c]` nach 0x801fbd00, Handle 0x800b21f0 (= Bank 4).
Datei-Ids ueber die Dateitabelle 0x8006f43c (8 B je Id, Groesse u32 @+0) nach Groesse zugeordnet: Id 137/138 =
3156/4400 B = ARMS09.EDH/.VB, 139/140 = ARMS0A, 141/142 = ARMS0B, 161/162 = 3176/40464 = CORE00.EDH/.VB.
ARMS09/0A/0B .EDH und .VB bytegleich (`cmp`). EDT selbst gelesen: ARMS09.EDH @0x28 = `00 00 13 10`,
CORE00..0D/0F.EDH @0x20 = `00 00 93 00` (CORE0E abweichend `00 00 a3 30`; ueber 0x80073a88 sind nur CORE00..03
waehlbar -> fuer den Bau egal).
**Praezisierung 1 (Stimmen-Pfad):** Byte3 & 0x1f = s0 (`andi s0,v0,0x1f` @0x8004517c). ARMS-Satz 0x0A hat s0 = 16 ->
Zweig @0x8004523c: Stimme s0-16 = 0 im Warteschlangen-System 0x800b2420 (18 B je Eintrag), vorher Prioritaetstor
FUN_80045a18(Stimme, Byte2&0xf = 3) (@0x80045244): verwirft, wenn laufende Prio > 3 oder (== 3 und Nibble >= 8).
CORE-Satz 8 hat s0 = 0 -> Sofortpfad `jal 0x80059d3c` @0x8004522c. Die Lage (Byte0 != 0 -> FUN_80045a64) wird in
BEIDEN Pfaden ausgewertet (@0x800451b8-cc bzw. @0x80045268-7c) — die Dossier-Aussage "positional" haelt fuer beide SEs.
Der Port hat das Prio-Tor bereits (audio_pc.c:703 zitiert FUN_80045a18).
**Praezisierung 2 (Lage-Formel FUN_80045a64):** kein reiner 3D-Abstand. h = sqrt(dx^2+dz^2) (Kamerasatz
[0x800ac778]+0x24 + Cut*32, Felder +4/+0xc gegen P+0/+8), dann d = sqrt((cam.y - |cam.y - P.y|)^2 + h^2)
(`lw v1,8(s2); subu v1,v1,s0` @0x80045b18-20, s0 = |cam.y - P.y|), Pegel = min(d*0x10624dd3>>36, 127)
(@0x80045b3c-5c). Allgemeiner SE-Mechanismus, nicht granatenspezifisch.

## J. Flaechenschaden FUN_80012d60 / FUN_8002b5d0 — TEILWEISE WIDERLEGT

Bestaetigt: Gegnerliste 0x800acc2c Schritt 500, Zaehlung ueber g_active_count 0x800aca4e, Spieler 0x800aca54,
Aufruf FUN_8002b5d0(Ziel, &P, 500) (`andi a2,s3,0xffff`); `read 0x8006f418 12 --w 2 --signed` =
[10,20,**1000**,1000,1000,50,100,200,300,1000,0,0], `read 0x8006f430 12 --w 1` = [3,3,**9**,10,11,14,...];
Spieler-Hitbox `bytes 0x80073e90`: Zeiger 0x80073ea0 -> 0x80073e94, Satz `00 00 06 fa 00 00 c2 01 fa 05 c2 01`
(hb[6] = hb[10] = 450 -> keine Winkel-Interpolation, Hoehe hb[8] = 1530). Treffer-Test waagrecht: Kasten
(unsigned `sltu a2(=2R), dx+R`), dann SquareRoot0 < R (`blez s0` @0x8002b770); senkrecht `-a2 < dy < a2` mit
a2 = 500 + hb[8] (@0x8002b778-7a8).

**WIDERLEGT — Spieler-Versatz (Dossier §2.5 und Zeile SPIELER_HITBOX "Versatz 0", "Mitte = Spielerlage"):**
0x800b2354 ist im Datei-Abbild 0, wird aber zur Laufzeit von **FUN_8002b498** ueber den +0x7c-Zeiger beschrieben:
```
8002b4bc lw s0,120(s1)      ; Hitbox-Satz
8002b4c0 lw s2,124(s1)      ; Versatz-Zeiger (Spieler: 0x800b2354)
8002b4c4 lhu v0,106(s1)     ; Gier -> RotMatrix(0,Gier,0) @0x8002b4d4
8002b4e0 lhu v0,0(s0) / 8002b4ec lhu v0,4(s0)  ; (hb[0], ., hb[4]) -> ApplyMatrix @0x8002b4f4
8002b504 sh v0,0(s2)        ; Versatz.x = gedreht
8002b508 lhu v0,2(s0) / 8002b510 sh v0,2(s2)   ; Versatz.y = hb[2]  (Spieler: 0xfa06 = -1530)
8002b51c sh v0,4(s2)        ; Versatz.z = gedreht
```
Laufzeitbeleg: `re_wurf_gegen_werkzeug/ss_takt_gegen.py` ueber alle 78 sauberen Savestates
(`build/r34g_wurf_gegen/ss_takt.txt`): 0x800b2354 = `00 00 06 fa 00 00` in **53** Spielstaenden, Nullen nur in 25
Titel-/Menue-/Boot-Staenden. Der einzige statische Verweis bleibt `addiu v1,v1,9044` @0x8003166c (Zeigerablage);
geschrieben wird indirekt (gefunden per Suche aller `lw rX,124(..)` mit Store ueber rX im Savestate-RAM,
`re_wurf_gegen_werkzeug/scans_gegen.py ptr7c <RAM-Dump>`, Ausgabe `build/r34g_wurf_gegen/scan_ptr7c.txt`). Der Dossier-Satz "Wert 0 im Datei-Abbild, einziger Verweis -> Mitte =
Spielerlage" ist ein Schluss aus statischer Abwesenheit, den das RAM widerlegt.
**Korrektur fuer den Bau:** Leon wird getroffen, wenn waagrecht sqrt(dx^2+dz^2) < 950 (unveraendert, hb[0]=hb[4]=0)
UND senkrecht -2030 < P.y - (Leon.y - 1530) < 2030, also **-3560 < P.y - Leon.y < +500** (nicht |P.y - Leon.y| < 2030).
Bei P.y = Liegestelle - 500 auf gleichem Boden (ca. -491) aendert sich das Ergebnis nicht; es aendert sich, sobald Leon
mehr als 500 unter P steht bzw. hoeher als 2030 ueber P (Treppe/Stufe) — dort weichen Original und Dossier-Modell ab.
Dasselbe gilt fuer JEDES Ziel: die Mitte ist Ziel +0x34/38/3c + (von FUN_8002b498 gedrehter) Versatz.

**Praezisierung Radius (Dossier "R = Ziel-Radius(+0x78->+6) + 500"):** gilt nur bei hb[6] == hb[10]
(`beq v1,v0,0x8002b6fc` @0x8002b61c). Sonst interpoliert FUN_8002b5d0 winkelabhaengig zwischen hb[6] und hb[10]:
Winkel = FUN_80065de0 (@0x8002b648) aus (P - Zielmitte) minus Ziel-Gier (`lhu v1,106(s3)` @0x8002b650), auf 0..0x400
gefaltet (@0x8002b65c-694), dann r = hb[6] + (hb[10]-hb[6])*trig(Winkel)>>12 (FUN_800683e8 bzw. FUN_80068348,
@0x8002b6a8-6f8). Fuer Gegner mit ungleichen Radien relevant.

**Nicht im Dossier (Gegnerzweig FUN_80012d60 @0x80012f28-8001302c), fuer den Bau noetig:**
* Ausschluss 1: `lw v1,21188(v1)` (aktueller ESP-Platz 0x800b52c4), `lw v1,116(v1)` (Platz+0x74 = Anker-Zeiger) gegen
  `lw v0,392(s1)` + 0x40 (Gegner+0x188 + 0x40) — gleich -> Gegner uebersprungen (@0x80012f38-4c). Bei der Granate
  ist +0x74 = [0x800acbdc]+0x7a4 (Spieler-Knochen) -> praktisch kein Ausschluss (Dossier nennt das nur indirekt).
* Ausschluss 2: `(Gegner+0x90 & 0x03000000) == 0x03000000` -> uebersprungen (@0x80012f54-60).
* Gegner+0x93 := (Gegner+0x93 & 1) (@0x80012f7c-88), FUN_8001a7a8(Gegner, P.x, P.z) != 0 -> +0x93 |= 0x80
  (@0x80012f8c-b0); ist Bit0 schon gesetzt -> +0x93 |= 2 und KEIN Schaden (@0x80012fb4-cc).
* Sonst: +0x07 := 0, +0x06 := 1, +0x05 := 0x8006f430[2] = 9, HP(+0x9a) -= 1000, +0x93 |= 1, +0x04 := 2, bei HP < 0
  +0x04 := 3 (@0x80012fd0-80013020). (Dossier nennt nur +0x05 und HP<0 -> +0x04 := 3.)
* Spielerzweig (@0x80012e20-efc): HP -= 1000; fuer Typ 2 (>= 2): +0x04 := 2, +0x05 := FUN_8001a7a8(Spieler,P.x,P.z)+2,
  +0x06 := 0, +0x93 |= 1; HP < 0 -> +0x04 := 3, +0x05 := 0, +0x06 := 0. Kein Gift-Wurf (nur Typ < 2, @0x80012e58-eb4).
  Spieler-HP hoechstens 100 (Schreiber `ori v0,zero,0x64` / `sh v0,player.hp` @0x80031710-18; Savestates: 43x 100,
  sonst weniger) -> 1000 Schaden ist immer toedlich.
* **Tabellen-Beobachtung fuer 0x0A/0x0B:** 0x8006f418[3]/[4] = 1000/1000, 0x8006f430[3]/[4] = 10/11 (= Item-Ids
  0x0A/0x0B). Die Tabelle hat also Plaetze fuer drei Granaten-Explosionen (Typ 2/3/4, Reaktion 9/10/11). Aufrufer mit
  a2 = 3/4: siehe Abschnitt N.

## K. Licht-Latch 0x800b5358 — BESTAETIGT

Eigener Xref-Scanner `re_wurf_gegen_werkzeug/xref_gegen.py 0x800b5358` (EXE + alle STAGE/TITLE/DEBUG.BIN):
Schreiber @0x80017694, @0x800180f0, @0x8001857c (je `sb v0,21336(at)`), Leser `lbu v0,21336(v0)` @0x8001ce60,
Zeiger `addiu s0,s0,21336` @0x8001d170 (-> `lbu` @0x8001d174, `sb zero,0(s0)` @0x8001d1b4). Basis-Zeiger in der Naehe
(Suche 0x800b5200..0x800b5358 als `addiu`-Basis): keiner, der mit Offset auf 0x800b5358 kommt. Kein Overlay-Verweis.
Leser-Block @0x8001ce5c-d084 selbst gelesen: Sicherung per `jal 0x8004ee38` (a2 = 0x28), FUN_8004f008(Gier, (1200,?,0))
(0x4b0 nach 0x1f80002c, 0 nach 0x1f800030; y-Eingabe unbeschrieben, fuer x/z bei reiner Y-Drehung ohne Wirkung),
L+3 := 0, L+0xA/B/C := max(.., 0xD2/0x8C/0x50) (`sltiu` + `beq` + `sb`), L+0x1C := playerX + v.x, L+0x1E := lhu
0x800aca8c - 800, L+0x20 := playerZ + v.z, L+0x26 := 0x1770. Rueckkopie @0x8001d1ac, Latch := 0 @0x8001d1b4. Alles
wie im Dossier.

## L. Takt / Aufrufreihenfolge — Ergebnis BESTAETIGT, eine Begruendung WIDERLEGT

Reihenfolge `dis 0x8001cdf8`: `jal 0x8001a50c` @0x8001ce04, `jal 0x80031c44` @0x8001ce0c, ..., `jal 0x80019e20`
@0x8001ce2c. Aufrufer von FUN_80019e20 (`re_wurf_gegen_werkzeug/jal_callers.py`, EXE + Overlays + nachgeladener
RAM-Bereich): genau @0x8001ce2c und @0x8004cd34. VSync-Aufruf `lbu a0,0(s0)` / `jal 0x80061fc0` @0x8002147c/80.
**WIDERLEGT (Begruendung):** "alle Schreiber setzen 2" stimmt nicht. Eigener Xref auf 0x800b5456: vier Schreiber
setzen **0** (`sb zero,21590(at)` @0x80016208 in FUN_800161e0 [setzt zugleich 0x800aca38 |= 0x02000000],
@0x8001d268, @0x8002650c, @0x800460e0); die 2-Schreiber sind @0x800166a4 (Delay-Wert v1 = 2 aus `ori v1,zero,0x2`
@0x8001669c), @0x8001d5ec, @0x80020d10, @0x80021314, @0x80026624, @0x80046718. Die 0-Schreiber liegen in
Menue-/Bildschirm-Aufbau-Pfaden (320x240-Setup `ori a0,zero,0x140`/`ori a1,zero,0xf0` davor bzw. danach).
**Ergebnis trotzdem bestaetigt:** 75 von 78 sauberen Savestates zeigen 0x800b5456 = 2 (alle Spielstaende), 0 nur in
sub_loadgame.sav und zwei Boot-Staenden (`build/r34g_wurf_gegen/ss_takt.txt`). Im Spielbild = 30 Hz.
Fuer den Bau: Takt 2 VBlanks gilt fuer das Spielbild; die Begruendung im Kommentar darf nicht "alle Schreiber" heissen.

## F.2 Anker-Matrix "Knochen 11" und ihr Zeitpunkt — Dossier-Luecke teilweise geschlossen

* [0x800acbdc] zeigt auf den Posenpuffer (room1140_entry.sav: 0x801d3498). Eigener Scan dieses Puffers auf gueltige
  Weltmatrizen (Spaltennormen 3900..4300, T im Umkreis von 4000 um Leon): 16 Treffer mit Schritt **0xAC** ab +0x40
  (+0x40, +0xEC, +0x198, … +0x7A4, … +0xA54). +0x7a4 = 0x40 + 11*0xAC -> **Teil 11** (Dossier-Bezeichnung
  bestaetigt, Beleg jetzt aus dem RAM). Nachbarn bei +-0x20 sind KEINE Matrizen (kein 0x20-Knochenfeld).
  Leon (-7600,0,-17600, Gier -96): Teil 11 T relativ (152, -1643, -536).
* Teile-Datensatz (Schritt 0xAC, Basis = *(Aktor+0x188); Spieler: 0x800aca54+0x188 = 0x800acbdc): +0x18 lokale Rotation (RotMatrix aus +0x78 bzw. aus dem interpolierten +0x60 in anim_set,
  `jal 0x80068098` @0x8001f574/@0x8001f5f0), +0x40 Weltmatrix = Eltern(+0x6c-Zeiger) x lokal (`jal 0x80022da0` mit
  a2 = Teil+64 @0x8001ea24 in FUN_8001e9ec).
* **Zeitpunkt:** FUN_8001e9ec wird nur aus FUN_8001e8c8 gerufen (@0x8001e990), und FUN_8001e8c8(Spieler) laeuft im
  Spielbild @0x8001d09c (a0 = 0x800ac784+720 = 0x800aca54), also NACH dem FSM-Spawn @0x8001ce0c und NACH dem
  ESP-Tick @0x8001ce2c. anim_set im FIRE-Substat (@0x80033668) schreibt nur die LOKALEN Matrizen (+0x18).
  -> Der Spawn liest die im VORBILD komponierte Weltmatrix von Teil 11 (Pose + Lage des Vorbilds). Die Port-Messung
  "Renderer-Knochen 11, 1 Bild alt" entspricht damit dem Original-Zeitverhalten; offen bleibt nur, ob das Port-Skelett
  die Pose bytegleich trifft (Dossier OFFEN 2 bleibt, aber mit belegtem Zeitbezug).

## M. Hand-Netz nach dem Wurf — BESTAETIGT (mit Ergaenzung)

Eigener Scan aller 50 `lbu ..,-13731(..)` (0x800aca5d) auf Vergleiche mit 9..0xC in den 7 Folgeinstruktionen:
@0x80033368 `sltiu v0,v0,0x9`, @0x80033688 `ori v0,zero,0x9`, @0x80033e4c `ori v0,zero,0xa` (vergleicht das
FSM-Bild 0x800acae9 fuer Waffe 7, nicht die Id) — deckungsgleich mit dem Dossier. Aufrufer FUN_80036b68 nur
@0x800316f0 und @0x800466b0 (bestaetigt).
Ergaenzung: Im RAM liegt ab 0x800c0000 nachgeladener Code (= DEBUG.BIN, s. Werkzeug-Befund in N; nicht im EXE-Text,
der bei 0x800bf000 endet), den die
EXE an ~48 Stellen direkt anspringt (z. B. `jal 0x800c00a8` @0x80031680, `jal 0x800c01c4` @0x800316d0 in der
Raum-Init). Er liest die Waffen-Id (@0x800c479c) und enthaelt einen Dauerfeuer-Entladepfad (Spawns 0x031D1200 /
0x03000B00 / 0x04000800 am Anker [0x800acbdc]+0x7a4, `jal 0x8004eae4`, `jal 0x80011f50`). Fuer Id 9 wird er nicht
erreicht: `ram_vs_exe_gegen.py` zeigt alle 19 granatenrelevanten Code-/Tabellenbereiche (R29-31, R0/R10, Tabelle
0x80071d40, Spawner, Tick, FIRE-Substat, 0x80074100, 0x80033b38-98, FUN_80012d60, FUN_8002b5d0, FUN_8002b498,
FUN_80045024, FUN_80045a64, Hauptbild 0x8001ce00-d1c0, Schadens-/Reaktionstabelle, RNG, SE-Banktabelle, 0x800740f4,
0x80074030) in 9 sauberen Savestates **bytegleich** zur Datei (`build/r34g_wurf_gegen/ram_vs_exe.txt`). Die
Waffen-Id-Scans des Dossiers (EXE-only) decken diesen Bereich nicht ab; fuer die Granate folgenlos.

## C.2 / §7 Zeitlinie — BESTAETIGT (als Funktion von h)

Eigener Simulator `re_wurf_gegen_werkzeug/wurf_sim_gegen.py` (Reihenfolge nach eigener Disassembly: Schleife 1 ->
(c) Welt-y -> (d) R29 -> (e) Physik wenn Flags&0x20 == 0; s16-Kappung wie `lh`/`sh`; C-Division) liefert fuer die
Port-h exakt die Dossier-Tabelle (`build/r34g_wurf_gegen/zeitlinie_gegen.txt`):
HOCH 40/8/88/124/129/131, 18760/1848; MITTE 29/8/73/109/114/116, 11929/1752; TIEF 13/6/40/76/81/83, 1543/40;
vergiftet HOCH 94/130, MITTE 79/115. Abprallbilder MITTE 29,47,55,60,64,67,70, Liegen 73 (Tiefen 136,90,16,25,16,3,9,9).

## H.2 Farben/Koepfe live — BESTAETIGT (Laufzeit-Gegenprobe)

`re_wurf_gegen_werkzeug/ss_clut_gegen.py` (room1140_entry / equip_test / room1090_orig / doorA_square):
0x800b2248[3] = 0x801ecd08 (CLUT 0x7811, TPAGE 0x001E), 0x800b2248[4] = 0x801ee428 (CLUT 0x7AD1, TPAGE 0x001F) —
Kopfwoerter zur Laufzeit NICHT umgeschrieben (der Port-Kommentar re15_esp.c "hdr words are runtime-patched by the TIM
installer FUN_800194f8" trifft fuer Effekt 3/4 nicht zu). VRAM-CLUT-Zeilen (272,481/483/491/492) je 32 B bytegleich
zu TEX.TIM @0x74/0xF4/0x2F4/0x334 (`build/r34g_wurf_gegen/ss_clut.txt`). Granate oliv (492), Feuerball 483, Rauch 481.
Zeichnen FUN_80053240/FUN_800534c4 selbst geprueft: Sichtbarkeit nur mit Flags&1 UND Flags&2 (@0x800532fc-0c),
Weltlage = Platz+0x28 (`lh v0,-68(s0)` mit s0 = Platz+0x6c), Teilezahl = Satz-Byte1 (`lbu v0,1(s3)` @0x80053354),
Halbtransparenz = Flags-Bit4 (`srl v0,v0,3; andi v0,v0,0x2` @0x80053500-04), Groesse aus Satz-Byte3 x Skala +0x72
x Zeile +0x04/+0x06 (@0x80053584-5ec). Zeichenschleife laeuft von Platz 95 ABWAERTS bis 0 (s0 = Pool+0x31ec,
`addiu s0,s0,-132` @0x800532f0). Bei erschoepftem Primitivpuffer wird der Platz geloescht (`sb zero,0(s0)`
@0x800533b8).

## N. Vollstaendigkeit — was der Bau braucht und im Dossier fehlt oder nur behauptet ist

| Nr | Mechanismus | Beleg (selbst) | Dossier | Port |
|---|---|---|---|---|
| N1 | Spieler-Trefferzone mit Versatz (0,-1530,0) (FUN_8002b498) | @0x8002b4bc-51c; RAM 0x800b2354 = `00 00 06 fa 00 00` (53 Spielstaende) | falsch ("Versatz 0") | richtig (re15_damage.c:3460 `hit_offset_y = -1530`) — NICHT "korrigieren" |
| N2 | Gegner-Ausschluss ueber ESP-Platz+0x74 == Gegner+0x188+0x40 | @0x80012f38-4c | nur indirekt ("Anker = Spieler-Knochen") | attacker_slot-Vergleich (re15_damage.c GATE A) -> fuer Granate -1 |
| N3 | Gegner-Ausschluss (+0x90 & 0x03000000) == 0x03000000 | @0x80012f54-60 | fehlt | "OMITTED" (re15_damage.c:3334 GATE B) |
| N4 | Gegnerzweig: +0x93 := +0x93&1, Front-Bit 0x80 (FUN_8001a7a8), Sperre Bit0 -> +0x93\|=2 ohne Schaden, sonst +0x07:=0/+0x06:=1/+0x05:=9/HP-=1000/+0x93\|=1/+0x04:=2 (HP<0 -> 3) | @0x80012f7c-80013020 | nur +0x05 und Tod | re15_enemy_take_damage (Schadens-Dossier) |
| N5 | Spielerzweig: +0x04:=2, +0x05:=FUN_8001a7a8(Spieler,P.x,P.z)+2, +0x06:=0, +0x93\|=1; HP<0 -> +0x04:=3/+0x05:=0/+0x06:=0; kein Giftwurf bei Typ 2 | @0x80012e20-efc | nur "HP -= 1000, Tod" | re15_player_take_damage |
| N6 | Radius-Interpolation hb[6]/hb[10] nach Winkel | @0x8002b61c-6f8 | vereinfacht (nur hb[6]) | vorhanden (re15_hitbox_test) |
| N7 | Drehen waehrend des Wurfs: pad&8 -> Gier -= 24, pad&2 -> Gier += 24 je Bild (0x80074091[(9-1)*5] = 48, `srl v0,v0,1`) — die Gier beim Spawn ist die GEDREHTE | @0x8003355c-0x80033600, `read 0x80074091 14 --w 1 --stride 5` | fehlt | Rate 24 im Rueckstoss (player_common.c:980-990); fuer Id 9 zu pruefen |
| N8 | Wurfabbruch: R1 (pad&0x100) los und FSM-Bild > 0x80074092[(9-1)*5] = 10 -> 0x800aca5a := 3, kein anim_set, KEIN Spawn; Munition schon im FIRE-Init (@0x800334e0) weg | @0x80033604-50 | fehlt | Kommentar game_step_common.c:1945 ("Schwelle 10") |
| N9 | Raumwechsel loescht alle 96 ESP-Plaetze (liegende Granate verschwindet ohne Explosion/Schaden) | FUN_80019354 `sb zero` @0x80019378 (0x800a7424 = Pool+0x6c), gerufen @0x8003996c | fehlt | zu pruefen |
| N10 | Pool voll -> FUN_80019700 liefert 0xff, keine Granate | @0x800197c8-cc | fehlt | RE15_ESP_FX_MAX = 96 (re15_esp.h:145) |
| N11 | Anker-Zeitpunkt: Weltmatrix Teil 11 aus dem VORBILD (FUN_8001e8c8 @0x8001d09c nach FSM/Tick) | s. F.2 | offen gelassen | "1 Bild alt" passt |
| N12 | Zaehler: Formel statt 7/9-Tabelle; +0x98 per SCD Member_set Id 17 frei beschreibbar (FUN_8004116c, `sh a2,152(a0)` @0x80041220); die RAISE-Schreiber @0x80033100-04 (`andi 0xbfff; ori 0x2000`, Bild >= 7 und Pad&0x20) und @0x80033150-54 (`andi 0xbfff; ori 0x8000`, Bild >= 8 und Pad&0x10) loeschen nur 0x4000 -> 0xA000 entsteht, wenn beide Pad-Bits im selben Bild gesetzt sind (Steuerkreuz: praktisch nicht; PC-Tastatur: moeglich). Dann R30 = HOCH (Zaehler 7/9 per Formel), aber der FSM spawnt in Bild 19 (0x8000) UND Bild 24 (0x2000) und der Clip wird 7+2+4 = 13 (@0x800334a8-bc) — Randfall fuer den Bau (Eingabe filtern oder Original-Verhalten bewusst nachbilden) | s. B | Formel angegeben, Tabelle nur Sonderfall | Formel verwenden |
| N13 | SE-Stimmenpfad ARMS (s0=16 -> Warteschlange 0x800b2420, Prio-Tor FUN_80045a18 Nibble 3) | @0x8004523c-64 | fehlt | Prio-Tor vorhanden (audio_pc.c:703) |
| N14 | Zeichenreihenfolge der ESP-Plaetze 95 -> 0 | @0x800532f0 | fehlt | zu pruefen (Feuerball/Rauch-Ueberlagerung) |

**Hinweise fuer 0x0A/0x0B (nur Datenlage, kein Mechanismus-Beleg — gehoert ins RE2-/Saeure-Brand-Dossier):**
* FUN_80012d60 hat im GESAMTEN RE1.5 (EXE, STAGE1-6/TITLE/DEBUG, nachgeladener Bereich) genau zwei Aufrufer:
  @0x80018008 (R25, a2 = 0) und @0x800185b8 (R31, a2 = 2). Die Tabellenplaetze Typ 3/4 (0x8006f418[3]/[4] =
  1000/1000, 0x8006f430[3]/[4] = 10/11 = Item-Ids 0x0A/0x0B) werden nie benutzt -> vorbereitete, unverdrahtete
  Eintraege.
* TEX.TIM hat direkt hinter der Granaten-Palette (272,492) die Paletten (272,493) (rot, Mittel-RGB 163/77/70) und
  (272,494) (blau, 79/110/150). Mit Effekt 4 sub 0x15 / 0x1D (derselbe Strom 0x1AB0, A = 30, CLUT 0x7AD1 +
  (sub>>3)*0x40) waeren das rote/blaue Wurfkoerper. Kein Spawn im Auslieferungsstand erreicht die Zeilen 493/494
  (Scan aller `jal 0x80019700/0x800199d4` mit `lui a0` im Umfeld). Hinweis, keine belegte Absicht.

**Werkzeug-Befund:** DEBUG.BIN wird zur Laufzeit bei **0x800c0000** geladen (62792 von 65536 Worten der Datei gleich
RAM@0x800c0000 in room1140_entry.sav), nicht bei 0x80100000, wie `re15_disasm.py`/`re2_disasm.py` fuer alle BIN
annehmen. Adressen aus DEBUG.BIN-Scans sind um 0x40000 zu verschieben; lui/imm-Global-Xrefs (z. B. 0x800b5358)
bleiben gueltig.

## O. RE2-Gegenprobe (Beta->Retail-Einordnung) — BESTAETIGT (mit Methodengrenze)

Eigener Strukturscan `scans_gegen.py re2struct` (`build/r34g_wurf_gegen/scan_re2struct.txt`): Muster von R29
(`lh rX,42(..)` -> `blez rX` -> `ori ..,0x5556` in 40 Instruktionen) findet in RE1.5 genau R29 @0x80018330
(Kontrolle) und in RE2 `info/re2leon/PSX.EXE` + allen 25 `COMMON/BIN/*.BIN` **0** Treffer. Konstantenworte
`ori v0,zero,0x17c` (0x3402017c), `ori v0,zero,0x118`, `addiu v0,zero,-110`, `lui a0,0x319`: 0 in RE2; `lui a0,0x40d`
1x in RE2-PSX.EXE (anderer Effekt, wie im Dossier). Grenze: ein anders geformtes Gegenstueck (andere Slot-Offsets)
schliesst ein Musterscan nicht aus; fuer die Hand Grenade (Id 9) ist RE1.5 vollstaendig vorhanden -> RE1.5 massgeblich
(Dossier-Einordnung haelt).

## Urteil je Zeile "KONSTANTEN FUER DEN BAU"

| Name | Urteil | Anmerkung |
|---|---|---|
| SPAWN_CODE 0x040D1000 | bestaetigt | `lui a0,0x40d` je im Delay-Slot der `beq` |
| SPAWN_BILD 0x13/0x16/0x18 | bestaetigt | |
| ZIEL_BIT | bestaetigt | |
| CLIP 9/7/11 | bestaetigt | |
| VERSATZ_HOCH/MITTE/TIEF | bestaetigt | {0,300,800}/{0,0,500}/{0,0,300} |
| ANKER +0x7a4 | bestaetigt + praezisiert | Teil 11 (Schritt 0xAC ab +0x40), Weltmatrix des VORBILDS |
| GIER | bestaetigt | |
| R30_ANIMSATZ / R30_FLAGS / R30_ROUTINE_B / ZUENDER_START | bestaetigt | |
| WURF_HOCH/MITTE/TIEF | bestaetigt | acc_x im Delay-Slot des RNG-`jal` |
| ZAEHLER_HOCH_MITTE | bestaetigt (Formel) | 7/9 nur Sonderfall; Formel im Bau (N12) |
| GRAVITATION 10 | bestaetigt | CORE00.ESP @0x1AC2 `0a 00` |
| BODEN | bestaetigt | |
| ABPRALL_X | bestaetigt | Adressbereich unvollstaendig: `lui v1,0x5555` steht @0x8001834c (Delay-Slot der `bne` @0x80018348) |
| ABPRALL_Y | bestaetigt | |
| SE_ABPRALL | bestaetigt | Stimmenpfad s. N13 |
| LIEGEN | bestaetigt | sp+16 wirklich unbeschrieben |
| EXPLOSION_BEI | bestaetigt | L+36 per eigenem Simulator |
| LICHT_LATCH | bestaetigt | |
| FLAGS_EXPLOSION 0x61 | bestaetigt | Unsichtbar per @0x80053308-0c |
| EXPLOSIONSPUNKT | bestaetigt | P bleibt ueber alle Folgeaufrufe intakt |
| FLAECHENSCHADEN | bestaetigt | Gegner-Gates N2-N4 fehlen im Dossier |
| SCHADEN / REAKTION 1000/9 | bestaetigt | |
| SPIELER_HITBOX | **widerlegt (Versatz)** | r 450 / h 1530 richtig; Versatz (0,-1530,0), senkrecht -3560 < P.y-Leon.y < 500 |
| KIND_FEUERBALL / KIND_RAUCH / KIND_ANKER / KIND_FLAGS | bestaetigt | |
| SE_EXPLOSION | bestaetigt | VAG 6, vol 110, Stimme 0 (eigener VH-Parser) |
| ZUENDER_NACHBRAND / FREI | bestaetigt | |
| CLUT_GRANATE 0x7B11 | bestaetigt (Datei + Laufzeit + VRAM) | |
| FLUG_ANIM 23..34 | bestaetigt | |
| FEUERBALL_ZEILE / RAUCH_ZEILEN | bestaetigt | mehrteilige Sprites (Byte1 = 6..16 Zellen) |
| LICHT | bestaetigt | |
| TAKT 30 Hz | bestaetigt (Ergebnis), Begruendung widerlegt | es gibt 4 Schreiber von 0 (Menues); Spielbild = 2 |

## PORT-ABGLEICH des Dossiers — Stichproben

Nr 1 (ENT[9] = {1,1,1,0} game_step_common.c ~1775), Nr 2 (Spawn mit param 0, game_step_common.c:1960-1961),
Nr 3/4 (Routinen-Faelle re15_esp.c:528-686 ohne 29/30/31; dispatch_b nur 12, re15_esp.c:724-736), Nr 5
(Bodenklemme re15_esp.c:916-922, "50% restitution"), Nr 6 (ungedrehtes `f->x + f->xlat_x`, main.c:296),
Nr 8 (spawn_rows setzt Flags 0x03, re15_esp.c:784), Nr 9 (effect3_smoke/effect4_shell main.c:3717-3718),
Nr 11 (Latch-Kommentar game_step_common.c:1867), Nr 12 (xorshift re15_damage.c:73-79) — **bestaetigt**.
Nr 7: Reihenfolge bestaetigt (re15_esp_fx_tick main.c:5413 im 30-Hz-Block ab Z.5377, re15_game_step jetzt
**main.c:7358** im Block ab Z.5622/6528, beide in `while (running)` ab Z.4790) — die Dossier-Zeile "main.c:6521"
stimmt nicht mehr. Nr 13: richtig, aber GATE B fehlt im Port (N3) und der Spieler-Versatz ist im Port bereits
richtig (N1).

## Fazit

Die RE-Arbeit des Ermittlers haelt der Gegenpruefung im Kern stand: R29/R30/R31, RNG, ESP-Tick, Spawner,
CORE00.ESP-Daten, SEs, Licht-Latch, Zeitlinie und RE2-Abwesenheit sind unabhaengig nachdisassembliert bzw. live
bestaetigt, und der granatenrelevante Code ist zur Laufzeit ungepatcht (19 Bereiche, 9 Savestates).
Widerlegt ist die Spieler-Trefferzone ("Versatz 0" — tatsaechlich (0,-1530,0) ueber FUN_8002b498, der Port hat das
bereits richtig und darf nicht zurueckgebaut werden) sowie die Begruendung "alle VSync-Schreiber setzen 2".
Fuer den Bau fehlen im Dossier: die Gegner-Gates in FUN_80012d60 (N2-N4), die Drehung und der Abbruch waehrend
des Wurfs (N7/N8), das Loeschen beim Raumwechsel (N9) und der Anker-Zeitbezug (N11, hier geschlossen).

## Werkzeuge und Ausgaben dieser Gegenpruefung

Ordner `analysis/befunde_runde34_granaten/re_wurf_gegen_werkzeug/`, Ausgaben `build/r34g_wurf_gegen/` (untracked):
* `xref_gegen.py <addr> [span]` — lui/imm-Xrefs EXE + alle BIN (Latch, 0x800b5456, 0x800acaec, 0x800b2354, 0x800acbdc).
* `jal_callers.py <ziel> [ram.bin]` — alle `jal` auf ein Ziel (FUN_80019e20, FUN_80036b68, FUN_8001f278, FUN_80022da0, FUN_8001e8c8/e9ec).
* `esp_zeilen_gegen.py` — CORE00.ESP nach FUN_8001945c/FUN_80019700 (Koepfe, Stroeme, Zeilen, Saetze, Koordinaten).
* `wurf_sim_gegen.py` — unabhaengiger Wurf-Simulator -> `zeitlinie_gegen.txt`.
* `ss_takt_gegen.py` — 78 saubere Savestates: 0x800b5456, 0x800aca38, 0x800acaec, 0x800b2354, Latch -> `ss_takt.txt`.
* `ss_clut_gegen.py` — Laufzeit-Effektkoepfe + VRAM-CLUT-Zeilen gegen Datei -> `ss_clut.txt`.
* `ram_vs_exe_gegen.py` — 19 granatenrelevante Bereiche Datei vs RAM (9 Savestates) -> `ram_vs_exe.txt`.
* `ss_dump_gegen.py`, `dis_raw_gegen.py` — RAM-Dump + Disassembly nachgeladenen Codes (DEBUG.BIN @0x800c0000).
* `scans_gegen.py ptr7c|w98|resolver|clutmap|re2struct` — Sammel-Scans -> `scan_*.txt`.
* `ptr_store_gegen.py`, `brace_depth.py` — Hilfsscans (Store ueber geladenen Zeiger; Klammertiefe main.c).
