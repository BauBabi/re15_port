# Gegenpruefung: re_saeure_brand.md (Runde 34, Granaten — Acid 0x0A / Incendiary 0x0B)

Pruefer: Skeptiker-Agent, 2026-09-29. Methode: jede Zeile der Tabelle "KONSTANTEN FUER DEN BAU" und jede
Mechanismus-Behauptung, die in Code wandern soll, UNABHAENGIG selbst disassembliert (`re15_disasm.py` /
`re2_disasm.py`), Bytes selbst gelesen, Offsets NIE selbst gerechnet. Ziel: widerlegen.
Werkzeuge/Ausgaben: `re_saeure_brand_gegen_werkzeug/`, Laufzeit `build/r34g_saeure_brand_gegen/` (untracked).
Keine Aenderung unter re15_port/, keine git-Schreiboperation.

Urteile: **bestaetigt** / **widerlegt** (mit Korrektur) / **unklar** (mit dem, was fehlt).

## Gliederung

1. RE1.5 Entlade-Handler 9/10/11 (+ Dispatcher-Kette 0x80074030 / 0x800740F4)
2. RE1.5 FSM-Gate @0x8003368c + aca5d-Zensus (breit nachgefahren)
3. RE1.5 Waffen-Record +8 (Ziffern-CLUT)
4. RE1.5 Schadensspalten @0x8006E0D0 / Hitscan-Resolver FUN_80011f50
5. RE1.5 Resolver FUN_80012d60: Tabellen 0x8006F418 / 0x8006F430, Art 3/4, Aufrufer-Zensus
6. RE1.5 Routinen 29/30/31 + CORE00.ESP Effekt 4 Sub 5
7. RE2 Opcode-Tabelle, Runden-Skript Bank 2 Skr. 4, Op 17
8. RE2 Op 15 Flug
9. RE2 Op 49 Saeure (WIDERLEGT: Box-Wahl toter Zweig)
10. RE2 Op 48 Brand (WIDERLEGT: Bodenflammen kommen immer)
11. RE2 Bodenfeuer-Kette Ops 27/58/28/46/19/29/30/40/50
12. RE2 Toene (NEU: RE1.5 ARMS10/ARMS11 = RE2 ARMS0B/0A bytegleich)
13. Weitere Grundlagen (Handler 10/11, TEX.TIM-CLUTs)
14. PORT-ABGLEICH geprueft
15. KONSTANTEN FUER DEN BAU — Urteil je Zeile
16. Fehlende Mechanismen
17. Fazit

---

## 1. RE1.5 Entlade-Handler 9/10/11 — **bestaetigt**

`re15_disasm.py table 0x80074100 22`: [9] @0x80074124 -> 0x80033b38, [10] @0x80074128 -> 0x80033b58,
[11] @0x8007412c -> 0x80033b78. `bytes 0x80033b38 0x60`: alle drei Koerper bytegleich
`e8 ff bd 27 10 00 bf af b9 3a 01 0c 00 00 00 00 10 00 bf 8f 18 00 bd 27 08 00 e0 03 00 00 00 00`
(addiu sp,-24 / sw ra / **jal 0x8004eae4** / nop / lw ra / addiu sp,24 / jr ra / nop).
Sprungziel selbst disassembliert (`dis 0x8004eae4 34`): liest Slot-Index `0x800B25C8`, Inventar `0x800B10AC+4*i`
(Byte +1 = Menge), `bne v0,zero` @0x8004eb44 -> `addiu v0,v0,-1` + `sb v0,0(at)` @0x8004eb60, Rueckgabe 1; bei 0 Rueckgabe 0
-> **Munition -1 bestaetigt.** Kein weiterer jal.

Zusatz (Dispatcher-Kette, nicht im Dossier, aber fuer die Aussage "Gate wird fuer 10/11 erreicht" noetig):
`table 0x80074030 22` (Waffen-FSM-Eintritt, Leser `lbu v1,-13731` + `lw v0,0(at)` @0x80032e60-7c, `jalr` @0x80032e84):
[9]/[10]/[11] -> **0x80032e9c** (Standard). Dieser liest `aca5a` und springt ueber `table 0x800740f4 6` ->
[2] = **0x80033460** (FIRE). 10/11 laufen also wirklich durch dieselbe FIRE-Funktion wie 9. Parameter-Satz
`0x80074090 + (Id-1)*5` (`bytes 0x8007408c 0x68`): Id 9 @0x800740b8, 10 @0x800740bd, 11 @0x800740c2 = je
`18 30 0a 01 00` — identisch.

## 2. RE1.5 FSM-Gate @0x8003368c + aca5d-Zensus — Gate **bestaetigt**, Zensus **unvollstaendig** (Aussage haelt)

`dis 0x80033670 80`:
```
80033680: lui v1,0x800b
80033684: lbu v1,-13731(v1)      ; 0x800ACA5D
80033688: ori v0,zero,0x9
8003368c: bne v1,v0,0x800337ac   ; Delay-Slot 80033690 ori v0,zero,0x13 (nur Vorladen)
800336a0: bne v1,v0,0x80033700   ; acae9 != 19
800336b4: andi v0,v0,0x8000      ; UND acaec-Bit 15 (HOCH)  -> Offsets sp+16/20/24 = {0,0x12c,0x320}
80033714: andi v0,v0,0x4000      ; acae9 == 22 UND Bit 14 (MITTE) -> {0,0,0x1f4}
80033770: andi v0,v0,0x2000      ; acae9 == 24 UND Bit 13 (TIEF)  -> {0,0,0x12c}
800336bc/c0, 8003371c/20, 80033778/7c: lui a0,0x40d / ori a0,a0,0x1000 ; jal 0x80019700 @800336ec/80033748/800337a4
```
Bestaetigt. Ergaenzung fuer den Bau: jeder der drei Spawns ist ZUSAETZLICH an das Zielhoehen-Bit von `0x800ACAEC`
gebunden (0x8000/0x4000/0x2000); das Dossier nennt nur die Bildnummern.

**Zensus:** Eigener breiter Zensus `re_saeure_brand_gegen_werkzeug/aca5d_breit.py` (Konstantenpropagation je Register,
erkennt auch `lui+ori`, Spielerbasis+9, Halbwort/Wort-Lesen von 0x800ACA5C; Ausgabe `build/r34g_saeure_brand_gegen/aca5d_breit.txt`):
**48 Leser statt 46**. Die zwei fehlenden:
```
8007d920: lui t0,0x800a / 8007d924: ori t0,t0,0xca5d     ; 0x800ACA5D ueber ORI (vom Ermittler-Muster nicht erfasst)
8007d928: lbu t1,0(t0) / 8007d930: xori t1,t1,0xc / bne -> 8007d93c: ori t2,zero,0x13 / 8007d940: sb t2,0(t0)   ; 12 -> 19
8007d948: lbu t1,0(t0) / 8007d950: xori t1,t1,0xd / bne -> 8007d95c: ori t2,zero,0x8  / 8007d960: sb t2,0(t0)   ; 13 -> 8
```
FUN_8007d904, einziger Aufrufer `jal 0x8007d904` @0x80061fe8 in FUN_80061fc0 (VSync, liest GPU_REG1/TMR_HRETRACE) — ein
eingehaengter Haken im Auslieferungs-EXE, der jede VSync Waffe 12 -> 19 und 13 -> 8 umschreibt. **Kein Vergleich mit 10/11**
-> die Dossier-Aussage "kein Code-Pfad behandelt 0x0A/0x0B gesondert" haelt. Die vier von meinem Zensus zusaetzlich
markierten Stellen (0x800188bc: Konstante 16 = Op-Code, 0x8003200c: `sltiu 0xb` liest aca5a, 0x80033e34 = Speedloader,
0x8007d928 = 12/13) sind geprueft und keine 10/11-Vergleiche. (Nebenbefund fuer das Waffen-Dossier, nicht fuer 10/11.)

## 3. RE1.5 Waffen-Record +8 (Ziffern-CLUT) — **bestaetigt**

`bytes 0x80074da8 0x120`: Rec 9 @0x80074E14 = `fa 00 00 00 | 88 4c 07 80 | 03 00 00 00`, Rec 10 @0x80074E20 =
`fa 00 00 00 | 88 4c 07 80 | 01 00 00 00`, Rec 11 @0x80074E2C = `fa 00 00 00 | 88 4c 07 80 | 02 00 00 00`
-> +8 @0x80074E1C/28/34 = **3/1/2**, +9 = 0; Rec 14 @0x80074E50 +8 = 2, +9 = 1; Rec 15/16/17 +8 = 3/1/2 (alle wie Dossier).
Stride 12 (`sll v0,v1,1; addu; sll v0,v0,2` @0x80049af4-fc), Basis `addiu at,at,19888` = 0x80074DB0 (= +8).
Leser: eigener Basis-Zensus `basis_xref.py 0x80074da8 0x80074db4` (Ausgabe `basis_74da8.txt`, 15 Basen): +8 wird NUR ueber
Basis 0x80074DB0 gelesen, genau @0x80049b0c, @0x80049b6c, @0x8004c8a4, @0x8004d9fc, jeweils `lbu a2,0(at)` + `jal 0x80048f28`
(Basis 0x80074DA8 nur mit Offset 0 = +0, Basis 0x80074DAC = +4, 0x80074DB1 = +9). Im Sprungziel: `sb a2,24(sp)` @0x80048f6c,
`lbu v0,24(sp) / sll 1 / addiu at,at,9748 (0x800B2614) / lhu v0,0(at) / sh v0,0(s0)` @0x8004922c-4c.
Fuellung: `jal 0x8006b3d8` @0x80046174 mit a0 = 0x100 (@0x80046168) und a1 = 0x1ea (Delay-Slot @0x80046178), Ergebnis
`sh v0,9748(at)` @0x80046184 -> 0x800B2614. 0x8006b3d8 selbst disassembliert = `sll v0,a1,6 / sra a0,a0,4 / andi 0x3f / or`
= GetClut(x,y). **Byte +8 = n -> CLUT (256, 490+n) bestaetigt** (n = 3 -> 493, 1 -> 491, 2 -> 492). Kein Schadenstyp.
Port `re15_inv_screen.c:1066-1069`: `2 + bu8(PROP_TBL + id*12 + 8)` auf DAT_800b2610 = dieselbe Zeile (0x800B2610+2*(2+n)
= 0x800B2614+2n) — bestaetigt.

## 4. RE1.5 Schadensspalten @0x8006E0D0 / Hitscan-Resolver FUN_80011f50 — **bestaetigt** (tote Daten fuer 9/10/11)

Index selbst disassembliert (`dis 0x80012490 40`): `lbu v1,8(s1)` (Typ) @0x800124b0, `addiu a0,a0,-7984` (0x8006E0D0) @0x800124b8,
`sll 1 / addu / sll 2 / subu / sll 3` = Typ*0x58 @0x800124c0-d0, `andi v1,fp,0xff / sll v1,v1,2` = Waffe*4 @0x800124d8-dc,
`lhu v1,0(v1)` @0x800124ec, `subu a0,a0,v1 / sh a0,154(s1)` HP. `read 0x8006e0d0 22 --w 2 --stride 4 --rows 0x38 --rowstride 0x58`
(Ausgabe im Chat-Protokoll dieses Pruefers): alle Dossier-Zeilen stimmen (z.B. 0x10 @0x8006E650: w9/10/11 = 100/200/100, w14 = 10,
w15/16/17 = 100/200/100; 0x20 @0x8006EBD0: 100/100/200; 0x25 @0x8006ED88: 100/50/200; 0x2B @0x8006EF98: 50/200/100;
0x30 @0x8006F150: 40/70/40; Null-Zeilen 0x1B-0x1F, 0x24, 0x26, 0x2C-0x2F, 0x36, 0x37).
**Aufrufer von FUN_80011f50** (`jal_xref.py 0x80011f50`, Ausgabe `build/r34g_saeure_brand_gegen/xref_80011f50.txt`): 11 Stellen —
0x80012418 (Rekursion, a0 = s0 = `andi s0,fp,0xff` = dieselbe Waffe), 0x80033554 (Schrot-Zusatz, Gate `ori v0,zero,0x8 / bne`
@0x80033510-14), 0x80033880/0x8003396c/0x80033a34/0x80033b10/0x80033c50/0x800349ec/0x80034c24 (Entlade-Handler 3-8,12,13,19),
0x800353cc (Nahkampf-Sub, nur Ids 0..2 ueber `table 0x80074030` -> 0x80034e70), DEBUG.BIN 0x800c47a4 (Flammenwerfer). Keiner in
0x80033B38/58/78. -> Spalten 9/10/11 und Tester 0x800128A0 im Auslieferungsstand nie gelesen: **bestaetigt**.

## 5. RE1.5 Resolver FUN_80012d60 — Tabellen, Art 3/4, Aufrufer — **bestaetigt** (mit 2 Ergaenzungen)

`bytes 0x8006f410 0x30`: `8006f418: 0a 00 14 00 e8 03 e8 03 | 8006f420: e8 03 32 00 64 00 c8 00 2c 01 e8 03 00 00` und
`8006f430: 03 03 09 0a 0b 0e 0f 10 11 12 14 00`. -> Schaden[3] @0x8006F41E = `e8 03` = 1000, Schaden[4] @0x8006F420 = `e8 03` = 1000,
Reaktion[3] @0x8006F433 = 0x0a, Reaktion[4] @0x8006F434 = 0x0b. **Alle vier Tabellenwerte bestaetigt.**

Resolver selbst disassembliert (`dis 0x80012d60 150`, `dis 0x80012fb4 40`):
* Argumente: `addu s3,a0,zero` (Radius) @0x80012d98, `addu s5,a1,zero` (Punkt, VECTOR: `lw a1,0(s5)` / `lw a2,8(s5)` =
  x/z als 32 bit) @0x80012d74, `addu s7,a2,zero` (Art) @0x80012da0.
* Gegnerschleife: `enemy_array` 0x800ACC2C, Stride `addiu s0,s0,500` (Delay-Slot @0x80012dfc), aktiv-Bit `andi v0,v0,0x1` @0x80012dbc,
  `jal 0x8002b5d0` (a0 = Gegner, a1 = Punkt, a2 = Radius & 0xFFFF) @0x80012dd0, Treffer in Liste sp+16.
* Spieler-Zweig: `jal 0x8002b5d0` mit a0 = 0x800ACA54 @0x80012e10; nur wenn `+0x93 & 1 == 0` (@0x80012e24-30);
  `andi a0,s7,0xff / sll v1,a0,1 / lhu v1,0x8006F418(v1)` @0x80012e38-54, `subu`/`sh` HP @0x80012e5c-64; `sltiu a0,a0,0x2`
  @0x80012e58 -> nur Art 0/1 Blut-/Gift-Wuerfel; +0x04 := 2 (@0x80012ebc), +0x05 := `jal 0x8001a7a8` + 2 (@0x80012ec8-d4),
  +0x06 := 0, +0x93 |= 1; HP < 0 -> +0x04 := 3, +0x05 := 0, +0x06 := 0 (@0x80012ef0-fc).
* Gegner-Zweig: `andi s0,s7,0xff` @0x80012f10, `s3 = 0x8006F418 + 2*Art` @0x80012f14-20; Gate A `+0x188 + 64 == ws->+0x74`
  @0x80012f40-4c, Gate B `(+0x90 & 0x3000000) == 0x3000000` @0x80012f54-60; `sltiu v0,s0,0x2` -> `jal 0x800453d0(0xa)` nur
  Art 0/1 (@0x80012f68-78); **Ergaenzung 1:** `+0x93 := +0x93 & 1` (@0x80012f7c-88, loescht alle anderen Bits),
  `jal 0x8001a7a8` != 0 -> `+0x93 |= 0x80` (@0x80012f94-b0); **Ergaenzung 2:** ist `+0x93 & 1` schon gesetzt (schon getroffen),
  dann nur `+0x93 |= 2` und KEIN Schaden (`beq v0,zero` @0x80012fc0, `j 0x80013024` @0x80012fc8 mit Delay-Slot `sb v0,147(s1)`).
  Sonst: +0x07 := 0, +0x06 := 1, `lbu v0,0x8006F430[Art]` @0x80012fe8 -> `sb v0,5(s1)` @0x80012ff0, `lhu a0,0(s3)` @0x80012ff4,
  `subu v1,v1,a0 / sh v1,154(s1)` @0x80012ffc-13000, +0x93 |= 1, +0x04 := 2 bzw. 3 bei HP < 0 (@0x80013010-20).
  Beides aendert die Werte nicht, gehoert aber in die Port-Umsetzung (Doppeltreffer-Sperre ueber +0x93 Bit 0).

**Aufrufer-Zensus** (`jal_xref.py 0x80012d60`, Ausgabe `xref_80012d60.txt` hier neu gefahren) plus eigener Suchlauf nach
`j 0x80012d60` (Endaufruf), Datenwort 0x80012D60 (jalr) und `lui 0x8001`+`addiu/ori 0x2d60` in PSX.EXE, DEBUG.BIN, STAGE1-6,
TITLE: **genau 2 Aufrufe**, 0 Endaufrufe, 0 Datenwoerter, 0 Adresspaare. @0x80018008: `addu a2,zero,zero` @0x80018004 (Art 0,
Radius `ori a0,zero,0x1f4` @0x80017ffc). @0x800185b8: `ori a2,zero,0x2` @0x800185b4 (Art 2). **Art 3/4 werden nie erzeugt:
bestaetigt.**

## 6. RE1.5 Routinen 29/30/31 — **bestaetigt** (1 kleine Ungenauigkeit)

`table 0x80071d40 48`: [29] @0x80071db4 -> 0x80018320, [30] @0x80071db8 -> 0x8001843c, [31] @0x80071dbc -> 0x8001854c.
Slot-Layout aus dem Spawner selbst disassembliert (`dis 0x80019700 150`): `sb t8,112(t0)` = +0x70 fx_id @0x800197d4,
`sb t7,113(t0)` = +0x71 sub @0x800197d8, `sh s1,114(t0)` = +0x72 Skala @0x800197dc, `sh s2,46(t0)` = +0x2E Gier @0x800197e4,
`sh v0,50(t0)` = +0x32 CLUT = Kopf+4 + (sub>>3)*0x40 @0x8001987c-88. Routinen 29/30/31 vollstaendig disassembliert: **kein Lesen
von +0x70/+0x71/+0x72**, keine Waffen-Id. (Ungenauigkeit ohne Folgen: Routine 29 liest zusaetzlich +0x38 `lw v0,56(t0)` @0x800183c4.)

Routine 31 (`dis 0x8001854c 110`), fuer die Vorlage des Explosions-Aufrufs:
```
80018560: lhu v1,30(a1)                ; +0x1E Zuender
80018568: beq v1,zero,0x80018688       ; == 0 -> 0x030B5800 + sb zero,108(a1) (Platz frei)
80018570: bne v1,v0(7),0x800185f4      ; == 7:
8001857c: sb v0(1),21336(at)           ;   0x800B5358 := 1
80018584: sb v0(0x61),108(a1)          ;   +0x6C := 0x61
80018594..800185b0: lh 40/42/44(v1); addiu v0,v0,-500 ; P = (x, y-500, z) als VECTOR sp+16/20/24
80018598: ori a0,zero,0x1f4            ;   Radius 500
800185b4: ori a2,zero,0x2              ;   Art 2
800185b8: jal 0x80012d60               ;   Delay-Slot 800185bc sw v0,24(sp) (z)
800185c0/c4: lui a0,0x319 / ori 0x5000 ;   0x03195000, a1 = +0x2E, a2 = 0x80072D4C, a3 = &P, jal 0x800199d4 @800185dc
800185e4/e8: lui a0,0x408 / ori 0x1    ;   SE 0x04080001, a1 = &P, jal 0x80045024 @800185ec
80018604/08: == 2 -> 0x03195000 (@80018640) + 0x030B5400 (@80018648/4c, jal @80018660), beide an (x, y-500, z)
8001867c/84: +0x1E -= 1
```
**Bestaetigt** (Radius 500, P.y = y-500, Art 2, Kinder, SE). CORE00.ESP gegengeprueft (eigener Parser, Datei
`re15_port/shared_assets/PSX/DATA/CORE00.ESP`, 7092 B): Effekt 4 Sub-Tabelle @0x18C0 = `4 26 48 80 102 124 146 158`, Sub 5 ->
Block @0x1AB0, 1 Stream, Zeile @0x1AB8 = `1e 00 ...` (A = 30), Zeile @0x1AE0 = 0. Routinen-Histogramm aller CORE00-Zeilen: A = 30
genau 1x, 29/31 nie direkt -> 29/31 nur ueber 30 erreichbar. Zusammen mit dem Aufrufer-Zensus (§5: keine Art 3/4) haelt
"RE1.5 hat keinen Saeure-/Brand-Aufschlag".

## 7. RE2 Opcode-Tabelle, Runden-Skript, Op 17 — **bestaetigt**

`re2_disasm.py table 0x8009d868 70`: [15] 0x8001ed9c, [17] 0x8001f198, [19] 0x8001f2c0, [22] 0x8001f634, [27] 0x8001fa9c,
[28] 0x8001fbd0, [29] 0x8001fd5c, [30] 0x8001fecc, [40] 0x80020758, [46] 0x80020b60, [47] 0x80020c3c, **[48] @0x8009d928 ->
0x80020f3c**, **[49] @0x8009d92c -> 0x800215c8**, [50] 0x80021970, [58] 0x80022254 — alle wie Dossier. Dass es die FX-Op-Tabelle ist:
Leser `lui at,0x800a / lw v0,-10136(at)` (= 0x8009D868) mit `jalr` in der Pro-Slot-Routine @0x8001d6b8/0x8001d6f0 (Op A +0x00 / Op B +0x01).

RE2 CORE00.ESP (`info/re2leon/COMMON/DATA/CORE00.ESP`, 8572 B): Ids `03 05 00 01 02 06 07 04`, Zeigertabelle am Dateiende -> Bank 2 @0x1784,
Bank 5 @0x05F0 (bestaetigt). Skripttabelle = Kopf + (ca*2 + cb + 2)*4 — Formel aus dem RE2-Registrierer selbst gelesen (`dis 0x8001bca0`:
`srl a0,v0,16 / andi v0,v0,0xffff / sll 1 / addu / addiu 2 / sll 2` @0x8001bd08-1c, `sw v1,0(a1)` -> 0x800D4E18[id] @0x8001bd2c).
Bank 2: ca 23, cb 20 -> Tabelle 0x1894, Sub-Offsets `4 12 20 28 36 44 58 66`, Skript 4 -> **0x1924**, Step @0x192C =
`00 11 2f 2f 00 10 00 10 f6 00 00 0f 00 00 80 02 00 ...` (bytegleich Dossier). Spawner FUN_8001cbe8 gleiche Semantik (sub&7 Skript,
Tabelle 0x800D4E18, Kopf 0x800D4CD8, Pool 0x800D8CF0, Stride 124, 96 Plaetze `addiu t2,zero,96`).

Op 17 (`dis 0x8001f198 66`): `lbu v0,0(a1)` (a1 = 0x800CFD06) @0x8001f1a8, `addiu v0,v0,-9` @0x8001f1b4, `sb v0,27(v1)` @0x8001f1b8;
Op A := 22 @0x8001f1c4-c8, Op B := 15 (`addiu a2,zero,15 / sb a2,1(v0)` @0x8001f1d4-d8), +0x18 := 0xB403, +0x21 := 18, +0x2A |= 0x20;
`lhu v0,0(a1) / andi 0x1f / addiu v1,zero,9 / bne` @0x8001f224-30: Id 9 -> +0x0B := rng%3 + 10 (`0x55555556`-Idiom, `addiu v0,v0,10`,
`sb v0,11(v1)` im Delay-Slot @0x8001f274); sonst `sb a2(15),11(v0)` @0x8001f284; `jal 0x8001ed9c` @0x8001f288. **Bestaetigt.**
RE2-Id-Zuordnung: 9 Explosiv / 10 Brand (flame) / 11 Saeure (acid) -> Art 0/1/2 -> Op 47/48/49. RE1.5 0x0A heisst "Acid", 0x0B "Incendiary"
-> die Dossier-Zuordnung 0x0A -> Op 49, 0x0B -> Op 48 ist namensgleich und konsistent mit `re2z_row_from_weapon` (10 -> 11, 11 -> 10).

## 8. RE2 Op 15 Flug — **bestaetigt** (Referenz, nicht fuer den Bau)

`dis 0x8001ed9c 230`: Box `lwl/lwr` 8 Byte von 0x80010900 nach sp+32 (`88 fa 00 00 5e 01 fa 00` = {-1400,0,350,250});
Wasser `jal 0x800527b4` @0x8001ede8, `slt v0,v1,v0` @0x8001ee14, y += 500 (`addiu v0,t1,500` im Delay-Slot @0x8001ee3c), 0x1A051C00;
Lebensdauer -1 @0x8001ee64-70; `jal 0x8004fba0(sp+16, 2, 8192, 0)` @0x8001eea0, `addiu v0,v0,-10 / sw v0,20(v1)` @0x8001eeac-bc;
Hitcode `lui s2,0x3 / ori s2,s2,0x9` + `lb a3,27(v1)` + `addu a3,a3,s2` bei y+1000 (@0x8001eec8, jal @0x8001eed8) und y-1000
(`addiu v0,v0,-2000` @0x8001eef8, jal @0x8001ef08); Treffer oder Lebensdauer 0 -> +0x18 |= 0x80 @0x8001ef44-50, Lage -> 0x800CFB88/8C/90,
`lbu v0,2(v1)` (step[2]); Wand/Boden (`lw v0,-13368(v0)` = 0x800DCBC8 @0x8001ef84) -> Rueckprall-Rechnung, **`jal 0x8001d894`
@0x8001f0cc (im Dossier nicht erwaehnt)**, `lbu v0,3(v1)` (step[3]) @0x8001f0e0; Dispatch `lb v1,27(v1) / addu / lw v0,-10136(at) / jalr`
@0x8001f0e4-104. **Bestaetigt.** Wichtig fuer §9/§10: das Bit 0x80 in +0x18 geht beim Aufschlag-Op sofort verloren (s.u.).

## 9. RE2 Op 49 Saeure — Werte **bestaetigt**, Box-Auswahl **widerlegt** (toter Zweig)

`dis 0x800215c8 240` (Ausgabe `build/r34g_saeure_brand_gegen/re2_op49.dis`; `lwl/lwr/swl/swr` zeigt das Werkzeug als `.word op22/26/2a/2e`,
von Hand dekodiert: 16 Byte ab 0x8001093C nach sp+32..47, also sp+32 = {-1200,0,600,300}, sp+40 = {-600,0,300,150};
`bytes 0x80010900 0x60`: 0x8001093C `50 fb 00 00 58 02 2c 01`, 0x80010944 `a8 fd 00 00 2c 01 96 00`).
Sprungtabelle `table 0x80010950 5` = {0x80021678, 0x800217d4, 0x80021854, 0x80021894, 0x800218f4} (bestaetigt).

**Der Widerspruch** (Rohbytes auch im RE2-Ghidra-Dump `ghidra_re2_Leon.txt` Z. 101826-101842):
```
80021694: 03 84 02 34  ori v0,zero,0x8403
80021698: 18 00 62 a4  sh  v0,0x18(v1)        ; +0x18 := 0x8403  (Bit 0x80 = 0)
8002169c: sb zero,0(v1) / 800216a8: li v0,0x31 / 800216ac: jal 0x8005ba28 (SE) / 800216b0: sb v0,1(v1)
800216c0: 18 00 62 94  lhu v0,0x18(v1)        ; liest die eben geschriebene 0x8403
800216c8: 80 00 42 30  andi v0,v0,0x80        ; = 0
800216cc: 03 00 40 14  bne v0,zero,0x800216dc ; NIE genommen
800216d4: j 0x800216e0 / 800216d8: addiu s0,sp,32   ; IMMER Box sp+32 = {-1200,0,600,300}
```
Gegenprobe, dass nichts dazwischen +0x18 aendert: der SE-Spieler FUN_8005ba28 schreibt nur eigene Tonstrukturen (0x800C..., 0x800D4CA0/1),
und 0x800DCBD0 (aktueller Platz) wird nur in 0x8001d3xx-0x8001d6xx und 0x800235d4/0x80023700 geschrieben (eigener Suchlauf).
-> **Saeure trifft IMMER mit {-1200,0,600,300}**; die Box {-600,0,300,150} ist in Op 49 toter Code. Dossier-Satz "Box je Bit 0x80"
und KONSTANTEN-Zeile "RE2-Aufschlag-Boxen ... Gegner {-600,0,300,150}" gelten fuer Op 49 NICHT.

Uebrige Op-49-Werte bestaetigt: SE `lui a0,0x113 / ori a0,a0,0x1` @0x80021678-7c (a1 = sp+16 = Lage), `jal 0x8005ba28` @0x800216ac;
Phase := 1 @0x80021690, Op A := 0 @0x8002169c, Op B := 49 (Delay-Slot @0x800216b0); Hitcode `lui a3,0x1002 / ori a3,a3,0xb` @0x800216e4/f0,
a0 = sp+16 = (+0x34,+0x36,+0x38), a1 = +0x22, jal @0x800216ec; zweiter Aufruf y+1800 (`addiu v0,v0,1800` @0x8002170c, jal @0x80021718).
Lebensdauer 255: vel.y (+0x0E) := 240, acc.x (+0x08) := 0, vel.x (+0x0C) := 0, y -= 470 (@0x8002173c-60); sonst acc.x := -23
(`addiu v0,zero,-23` im Delay-Slot @0x80021738, `sb v0,8(a0)` @0x80021764), vel.x := 0, vel.y := 240 (@0x80021770-78); acc.y (+0x09) := 0
@0x8002177c. Kinder `jal 0x8001cbe8` (a1 = +0x22, a2 = 0x8009DB44 = Einheitsmatrix `00 10 00 00 00 00 00 00 00 10 ...`, a3 = Platz+0x34):
0x030F2000 @0x80021780/84, 0x040C2000 @0x800217a8/ac, 0x041D1800 (`lui a0,0x41d` @0x800217c8, `ori` im Delay-Slot @0x800217d0,
jal @0x800218e4). Phase 1: 0x031F2000 (@0x800217f0/0x80021804), Phase 2: 0x03142000 (@0x80021888/90), Phase 3: 0x040D2800 (@0x800218c8/cc),
Phase 4: 0x030F2000 (@0x80021920/24), dann Op B/Op A/+0x18 := 0 (@0x80021950-58). Kleinigkeit: in Phase 1 wird bei Lebensdauer 255
nach dem Spawn zusaetzlich y -= 470 (@0x80021840-50) — nur fuer den Wasser-Fall relevant.

## 10. RE2 Op 48 Brand — Werte **bestaetigt**, Bedingung der Bodenflammen **widerlegt** (sie kommen IMMER)

`dis 0x80020f3c 420` (Ausgabe `re2_op48.dis`). Rohbytes auch im Ghidra-Dump Z. 101380-101412:
```
80020fc4..80020fe4: lw v0,96/100/104(a3) -> sw sp+16/20/24     ; alte Translation +0x60/64/68 sichern
80020fe8..80020ff0: lh v1/a1/a2 = +0x34/+0x36/+0x38            ; Aufschlaglage
80020ff4: 00 80 02 34  ori v0,zero,0x8000
80020ff8: 18 00 e2 a4  sh  v0,0x18(a3)                         ; +0x18 := 0x8000 (Bit 0x80 = 0)
80021000/14/18: sw v1/a1/a2 -> +0x60/64/68                     ; Translation := Aufschlaglage
80021010: sh s1(1),18(a3) / 8002101c: sb v0(48),1(v1)          ; Phase 1, Op B 48
80021028: ori a0,a0,0x1 (a0 = 0x01120001) / 8002102c: jal 0x8005ba28 / 80021030: addiu a1,a1,96
80021040: lhu v0,0x18(v1) / 80021048: andi v0,v0,0x80 / 8002104c: bne v0,zero,0x800214e8   ; NIE genommen
```
-> Der Zweig 0x800214e8 (Box sp+40 = {-600,0,300,150}, keine Bodenflammen) ist **toter Code**. Brand laeuft IMMER durch
0x80021054..0x800214e0: zwei Treffer 0x0002000A (`lui a3,0x2 / ori a3,a3,0xa` @0x80021058/64 und @0x8002106c/70, beide a0 = Platz+0x60 =
Aufschlaglage, Box s0 = sp+32 = {-1200,0,600,300}, jal @0x80021060/0x8002108c), Kinder 0x040C2800 (`lui a0,0x40c / ori 0x2800`
@0x80021094/98, jal @0x800210b8, a1 = 0, a2 = Platz+0x4C, a3 = 0) und 0x041D2700 (@0x800210c0/c4, jal @0x800210d8), dann **immer drei
Bodenflammen** (jal @0x8002116c, @0x800212c0, @0x80021418). Das "+1800" wirkt nur auf die gesicherte Kopie sp+20 (Dossier richtig);
Ende @0x80021578-a0: +0x60/64/68 := sp+16/20/24 (alte Translation, y + 3600) — ohne Folge, Platz wird in Phase 1 frei (@0x800215a4-ac).
Dossier-Zeile "Bodenflammen ... nur bei Wand/Boden (Status-Bit 0x80 = 0, `andi v0,v0,0x80` @0x80021048)" -> **Korrektur: immer**.

Bodenflammen-Werte selbst nachgerechnet (bestaetigt): Skala `rng%8` (`bgez/sra 3/sll 3/subu`) `*3 <<8 + 7168` (`sll s0,v0,1 / addu /
sll s0,s0,8 / addiu s0,s0,7168` @0x80021104-10) `| 0x05050000` (`lui v0,0x505` @0x80021114, `or` im Delay-Slot @0x8002111c);
Gier: Flamme 1 `0x66666667 >> 4` -> rng%40 (@0x80021120-164), Flamme 2 `>> 5` -> rng%80 `+ 400` (`addiu a1,a1,400` @0x800212b8),
Flamme 3 rng%80 `- 400` (`addiu a1,a1,-400` @0x80021410); je Flamme (nur wenn Platz < 255): vel.x (+0x0C, `lhu a0,-29444(at)` =
0x800D8CFC + Platz*124) += rng%25 (`0x51eb851f >> 3`), acc.y (+0x09, 0x800D8CF9) += rng%8, +0x4A (0x800D8D3A) := 1
(`sh s1,-29382(at)` @0x8002121c; `sh v0,...` @0x8002135c/0x800214b4). Skript Bank 5 Skript 5 @0x07F8, Step @0x0800 =
`00 1b 2e 32 00 10 00 10 00 05 00 00 60 00 ...` (bestaetigt).

## 11. RE2 Bodenfeuer-Kette Ops 27/58/28/46/19/29/30/40/50 — **bestaetigt** (1 Praezisierung)

Alle selbst disassembliert (`re2_disasm.py dis`):
* **Op 27** @0x8001fa9c: +0x18 := 0xB003 (`ori v1,zero,0xb003 / sh v1,24(a0)` @0x8001fabc-c0), +0x2A |= 0x20, +0x21 := rng%3,
  +0x14 := `jal 0x8004fba0(P,2,8192,0)` @0x8001fb4c, Op A := 58 (@0x8001fb64-68), Op B := 28 (@0x8001fb74-78), +0x1B := 2
  (Delay-Slot @0x8001fb8c), +0x42 := rng%3 + 8 (`addiu v0,v0,8 / sh v0,66(v1)` @0x8001fbb4-b8). Bestaetigt.
* **Op 58** @0x80022254: +0x1B 2: Zaehler--, X *1010/1000 (`sll 6/subu/sll 3/addu/sll 1` @0x80022350-60, `0x10624dd3 >> 6`),
  Y *1007/1000 (@0x80022370-80); Zaehler 0 -> +0x1B := 1, Zaehler := 2 + rng%2 (@0x800223b8-e4). +0x1B 1: X *880/1000, Y *800/1000
  (@0x800222c0-328); Zaehler 0 -> +0x1B := 0. +0x1B 0: Op A := 0, +0x18 := 0 (@0x800222a0-a8). Bestaetigt.
* **Op 28** @0x8001fbd0: Wasser -> Op A/+0x18 := 0 (@0x8001fc20-24); vel.x < 0 -> acc.x/vel.x := 0 (@0x8001fc3c-48); Boden bei
  y-900 (`addiu v0,v0,-900` @0x8001fc6c, jal @0x8001fc7c) `slt v1,s0,v1` -> Op[step[2]] (@0x8001fcac-c8); sonst Wand (0x800DCBC8)
  -> Boden == +0x14 ? Op[step[3]] : Op[step[2]] (@0x8001fcf8-30); +0x14 := Boden. Bestaetigt.
* **Op 46** @0x80020b60: acc.y/step[2]/vel.y := 0, Op A := 19, Op B := 29, vel.x := 180, acc.x := -10 - rng%11 (`0x2e8ba2e9 >> 1`),
  +0x42 := rng%8 + 38, +0x12 &= 0xFFFE. Bestaetigt. **Praezisierung:** Op 46 setzt +0x1B NICHT. Landet die Flamme erst nach dem
  Luft-Wachsen (Op 58 hat +0x1B schon auf 1 gesetzt), startet Op 19 im Schrumpf-Zustand 1 (38..45 Bilder x0.99/x0.98, dann tot).
  Die Dossier-Dauer "(38..45) Wachsbilder + (90..100) Schrumpfbilder" gilt nur fuer eine Landung waehrend +0x1B = 2.
* **Op 19** @0x8001f2c0: nur wenn +0x4A != 0 (`lh v0,74(v1)` @0x8001f2d0): `sltiu v0,v0,0x10` auf +0x16 (@0x8001f2e8), `sltiu v0,v0,0x1001`
  auf X-Aspekt (@0x8001f2fc), `jal 0x80020758` @0x8001f308, +0x16++ @0x8001f31c-28 (nur in diesem Zweig). Zustand 2: X *1009, Y *1002
  (@0x8001f40c-444), dann +0x1B := 1, Zaehler := rng%11 + 90 (`addiu v0,v0,90` @0x8001f4c0); 1: X *990, Y *980 (@0x8001f3a4-3e4);
  3: Zaehler--, dann +0x1B := 2, Zaehler := rng%8 + 30 (@0x8001f4e8-51c); 0: tot. Bestaetigt. Gegenprobe: die generische Pro-Slot-
  Routine 0x8001d68c (Op A / 0x8001d894 / Op B / Physik bei +0x18 & 2 / Anim bei & 1) schreibt +0x16 nicht -> +0x16 zaehlt nur in Op 19.
* **Op 40** @0x80020758: Box 8 Byte von 0x80010910 (`a8 fd 00 00 2c 01 96 00` = {-600,0,300,150}), P.y - 100 (`addiu v0,v0,-100`
  @0x800207a4), Hitcode `lui a3,0x2002 / ori a3,a3,0xa` @0x80020794/a0, jal 0x800470c0 @0x800207bc, Treffer -> `jal 0x80021970`
  @0x800207cc. **Op 50** @0x80021970: step[2] := 64, acc.x := 0, step[3] := 0, vel.x := 0, +0x12 &= 0xFFFD. Bestaetigt.
* **Op 29** @0x8001fd5c: vel.x <= 0 -> acc.x/vel.x := 0; sonst Folgeflamme nur wenn vel.x >= 61 (`slti v0,v0,61` im Delay-Slot
  @0x8001fd78) und step[2] % 15 == 0 (`0x88888889`, `srl 3`, `*15` @0x8001fd94-b8): `lui v0,0x504` @0x8001fdf0, Skala `+0x3A*4*0x66666667>>33`
  = x0.8, a2 = Einheitsmatrix, a3 = Platz+0x34; Kind +0x4A := 1 (@0x8001fe20); step[2]++ (@0x8001fe30-3c); Wand bei y-100 -> Op[step[3]].
  Bestaetigt.
* **Op 30** @0x8001fecc: `jal 0x8001dd2c` (Op 2), Sub (+0x1E) == 4 -> +0x1B := 2, Zaehler := rng%8 + 2; sonst +0x1B := 3,
  Zaehler := (rng%6)*50 + 700. Bestaetigt. Folgeflammen-Skript Bank 5 Skr. 4 @0x07C0: 2 Steps, Step 0 @0x07C8 Op A 30, Step 1 @0x07E0
  `13 19 ...` = Op A 19 / Op B 25 (bestaetigt den Weg Op 30 -> Op A 19).

## 12. RE2 Toene — Werte **bestaetigt**; Datenquelle **widerlegt**: RE1.5 hat die Samples selbst

SE-Spieler RE2 selbst disassembliert (`dis 0x8005ba28`): Bank = `srl t1,a0,24` @0x8005ba30, Liste `lw a1,-17544(at)` = 0x800DBB78[Bank]
@0x8005ba8c, Satz `sll v0,s7,2` (s7 = (a0>>16)&0xFF) @0x8005ba90, 0xFFFFFFFF -> stumm (@0x8005baa0), Programm `andi s6,v0,0x7f`
(Byte 1), Ton `srl s2,a0,4` (Byte 2), Tonattribut = 0x800D75A0[Bank] + Prog*0x200 + Ton*0x20 + 2080 (@0x8005bae8-f8). Bestaetigt.
ARMS-Dateiindex `read 0x800a8118 26 --w 2`: [9]/[10]/[11] = 305/307/309, Paar-Raster (EDH ungerade, VB = Index+1 ab [20]).
RE2-EDH selbst gelesen: ARMS09 Satz 17 @0x44 = `00 00 33 20`, ARMS0A Satz 18 @0x48 = `00 00 33 20`, ARMS0B Satz 19 @0x4C =
`00 00 33 20`, die jeweils anderen = `ff ff ff ff`; VAB-Kopf 'pBAV' @0x80; Prog 0 Ton 3 @0x900 -> VAG 3; VAG-Groessentabelle:
ARMS0A VAG 3 = 11664 B, ARMS0B VAG 3 = 10992 B; `ARMSxx_00001.wav` = VAG 3 (Rahmenzahl*16/28+16 = 11664 / 10992), md5 2b1376ac…/26d05bb3…
— alles **bestaetigt**. Datei-Zuordnung 307 -> ARMS0A, 309 -> ARMS0B: nicht ueber die CD-TOC, aber in sich geschlossen
(Satz 18 existiert NUR in ARMS0A, Op 48 = Waffe 10 spielt 0x0112 -> sonst waere der Retail-Brandton stumm).

**NEU / Korrektur zur PORT-ABGLEICH-Zeile "Toene":** Die RE1.5-Disc hat die RE2-GL-Aufschlagtoene selbst, als Baenke der unfertigen
GL-Klasse 15/16/17 (Dateiindex-Muster ARMS<Id hex>): `md5sum` (info/Re1.5/PSX/SOUND = re15_port/shared_assets/PSX/SOUND):
* RE1.5 `ARMS10.VB` (Waffe 16 = GL Saeure) `39cec979…` == RE2 `ARMS0B.VB` (Saeure) `39cec979…`
* RE1.5 `ARMS11.VB` (Waffe 17 = GL Brand) `46833b5e…` == RE2 `ARMS0A.VB` (Brand) `46833b5e…`
* RE1.5 `ARMS0F.VB` (Waffe 15 = GL Explosiv) `786ad691…` == RE2 `ARMS09.VB` `786ad691…`
RE1.5-EDH (`pBAV` @0x40 -> Satztabelle 16 Eintraege): **Satz 10** @0x28 = `00 00 33 20` (Prog 0, Ton 3 -> VAG 3) in ARMS0F/10/11;
Tonattribut Prog 0 Ton 3 in ARMS10 == RE2 ARMS0B und ARMS11 == RE2 ARMS0A **byteweise gleich** (32 B:
`00 00 78 40 48|49 3b 3d 3d ... ff 80 c0 1f 00 00 03 00 ...`); VAG 3 = 10992 B (ARMS10) / 11664 B (ARMS11).
RE1.5-SE-Spieler Bank 1 = `lui a0,0x801f / ori a0,a0,0xcd00` (ARMS-EDH der ausgeruesteten Waffe) @0x800450d8-e0, Satz < 0x21 @0x800450d0.
Die RE1.5-Baenke der Granaten selbst sind bytegleich (ARMS09 = ARMS0A = ARMS0B, EDH `99288a7b…`, VB `45e8c0b6…`) und haben keinen
Aufschlagton (nur VAG 2 = 4352 B).
-> Folge fuer den Bau: Der Saeure-/Brand-Aufschlagton muss NICHT aus `info/re2leon` nach `shared_assets/RE2/` kopiert werden; er liegt als
RE1.5-Eigendaten in `shared_assets/PSX/SOUND/ARMS10.VB` / `ARMS11.VB` (VAG 3, ueber EDH Satz 10). Nach der Beta->Retail-Regel ist
das die vorzuziehende Quelle (RE1.5-Daten vorhanden, Retail-gleich). Der Aufruf-Zeitpunkt/Code (SE im Aufschlag-Bild) bleibt RE2-Beleg.

## 13. Weitere geprueft (nicht in der Konstanten-Tabelle, aber Grundlage)

* RE2-Waffen-Effekt-Tabelle `table 0x800a6fdc 12`: [10] @0x800a7004 -> 0x80044f44, [11] @0x800a7008 -> 0x80045090 (bestaetigt).
  Handler 10 (`dis 0x80044f44 84`): Bild +0x14D == 1 -> 0x01002000 (a1 = 0), **0x020C1000** (`lui a0,0x20c / ori 0x1000` @0x80044f9c/a0,
  a1 = `lh a1,118(s1)` @0x80044fa8, jal 0x8001bf10 @0x80044fac), 0x03081200; Matrix a2 = `lw s0,408(s1)` + 1964 (0x7AC); Versatz
  `addiu v0,zero,120 / 1200` @0x80044f7c/84. **Handler 11 == Handler 10 bytegleich** (eigener Vergleich 332 B, 0 Abweichungen).
  RE2-Entlade 0x800A6F90 [9..11] -> 0x80043fcc/ff4/401c, je nur `jal 0x8006a0cc` (bestaetigt).
* RE2 TEX.TIM gegen RE1.5 DATA/TEX.TIM (eigener TIM-Parser): RE2 CLUT (256,480) 32x19, Bild (0,0) 256x256; RE1.5 CLUT (256,480) 32x24,
  Bild 320x256. Zeilen 480..484 bei x 272..287 **gleich**, ab 485 verschieden — Dossier OFFEN 2 bestaetigt. Alle von Op 48/49 und den
  Flammen benutzten CLUT-Zeilen (Bank 3/4 Basis 480 + sub>>3 = 481/482/483, Bank 5 = 484) liegen im gleichen Bereich.

* Waffenbaenke (Dossier §1.7): `md5sum` PL00W09 = PL00W0A = PL00W0B `50cf41fd…`, PL04W09 = PL04W0A = PL04W0B `f8af5f07…` — bestaetigt.
* Gegenprobe "hat RE1.5 die RE2-GL-Effektdaten?": Suche der RE2-Step-Bytes (Runde `00 11 2f 2f 00 10 00 10 f6 00 00 0f 00 00 80 02`,
  Bodenflamme `00 1b 2e 32 00 10 00 10 00 05 00 00 60 00 00 00`, Folgeflamme) in ALLEN Dateien unter `info/Re1.5/PSX` (inkl. RDT-ESPs):
  **0 Treffer**; RE1.5 hat nur `DATA/CORE00.ESP` (Ids 3/8/0/2/4). Anders als bei den Toenen (§12) liegen die Effekte nur in RE2 ->
  Dossier-Einordnung "Effekt = RE2" haelt.

## 14. PORT-ABGLEICH — geprueft

| Dossier-Stelle | Befund |
|---|---|
| `game_step_common.c:1775` `[9] = {1,1,1,0}` | bestaetigt (Zeile 1775) |
| `:1779-1780` `[10]/[11] = {1,0,1,0}` | bestaetigt |
| `:1942-1961`, Gate `== 9` `:1951` | bestaetigt (`if (re15_player_equipped_weapon() == 9)` Z. 1951; Versaetze {0,0x12c,0x320}/{0,0,0x1f4}/{0,0,0x12c} wie @0x800336d8-e8/0x80033738/0x80033794) |
| `re15_esp.c` ohne Routinen 29/30/31, `esp_fx_dispatch` :523, `esp_fx_dispatch_b` :724 | bestaetigt (grep auf die Adressen/`case 29..31` leer; Funktionen an den Zeilen) |
| kein RE2-FX-System (`grep re2fx` leer) | bestaetigt (`re2fx`/`re2_fx` in engine/src + include leer) |
| `re15_damage.c:41-58` bytegleich 0x8006F418/0x8006F430 | bestaetigt |
| `enemy_ai_re2_zombie.c:3847` `re2z_row_from_atktype = {1,1,17,17,17,9,9,10,11,17,1}`, `:3829` `re2z_row_from_weapon` 10->11, 11->10 | bestaetigt. **Ergaenzung:** dieselbe Tabelle bildet auch Art 2 (Handgranate) auf 17 ab, das Nachbar-Dossier `re_schaden_resolver.md` §9 verlangt dort RE2-Zeile 9 — beim Bau zusammen anfassen |
| `re15_inv_screen.c:1066-1069` | bestaetigt (`2 + bu8(PROP_TBL + id*12 + 8)` auf 0x800B2610) |
| Toene: "ARMS0A/0B-VAG 3 fehlen in `shared_assets/RE2/`" | Tatsache richtig, **Folgerung falsch**: die identischen Samples liegen als RE1.5-Daten in `shared_assets/PSX/SOUND/ARMS10.VB` (Saeure) / `ARMS11.VB` (Brand), s. §12 |
| `platform/pc/main.c` RE15_GIVE/RE15_EQUIP | bestaetigt (Z. 4214-4246) |

## 15. KONSTANTEN FUER DEN BAU — Urteil je Zeile

| Dossier-Zeile | Urteil | Beleg / Korrektur |
|---|---|---|
| RE15_GRANATE_ART_SAEURE = 3 | bestaetigt (Werte); a2 = Port-Zuordnung (so gekennzeichnet) | @0x8006F41E `e8 03`, @0x8006F433 `0a` |
| RE15_GRANATE_ART_BRAND = 4 | bestaetigt (Werte) | @0x8006F420 `e8 03`, @0x8006F434 `0b` |
| Schaden Art 3/4 = 1000/1000 | bestaetigt | `lhu a0,0(s3)` @0x80012ff4, `subu`/`sh v1,154(s1)` @0x80012ffc-13000; Spieler `lhu v1,0(at)` @0x80012e54 |
| Reaktions-Id Art 3/4 = 10/11 | bestaetigt | `lbu v0,0(at)` @0x80012fe8, `sb v0,5(s1)` @0x80012ff0; Ergaenzung: nur wenn +0x93 Bit 0 frei (@0x80012fc0) |
| Explosions-Aufruf Radius 500, P.y-500, Art 2 | bestaetigt | @0x80018598/0x800185a8/0x800185b4/0x800185b8, VECTOR sp+16/20/24 |
| Waffen-Record +8 = 3/1/2 (Ziffern-CLUT) | bestaetigt | @0x80074E1C/28/34; GetClut 0x8006b3d8, 0x800B2614 = (256,490) |
| RE2 Runden-Skript Bank 2 Skr. 4 | bestaetigt | RE2 CORE00.ESP 0x1924/Step 0x192C, Formel aus 0x8001bd08-2c |
| RE2 Runden-Spawn 0x020C1000, {120,1200,0} | bestaetigt | @0x80044f9c/a0, @0x80044f7c/84; Handler 11 bytegleich 10 |
| RE2 Art-Byte = Id - 9 | bestaetigt **fuer RE2-Ids**; Warnung | Gilt NICHT fuer RE1.5-Ids: RE1.5 0x0A (Acid) - 9 = 1 = RE2-Brand! Bau braucht die explizite Zuordnung 0x0A -> Art 2 (Op 49), 0x0B -> Art 1 (Op 48) (Dossier §3.1 richtig, Konstantenzeile missverstaendlich) |
| RE2 Lebensdauer 10+rng%3 / 15 | bestaetigt (Referenz) | @0x8001f26c-74, @0x8001f1d4/0x8001f284 |
| Aufschlag-Op Saeure = Op 49 = 0x800215C8 | bestaetigt | `table 0x8009d868` [49] @0x8009D92C |
| Aufschlag-Op Brand = Op 48 = 0x80020F3C | bestaetigt | [48] @0x8009D928 |
| SE Aufschlag Saeure 0x01130001 | Code bestaetigt; **Datenquelle widerlegt** | RE2 @0x80021678-7c/jal @0x800216ac; Sample+Tonattribut liegen bytegleich in RE1.5 `ARMS10.VB`/`.EDH` (Satz 10 -> Prog 0 Ton 3 -> VAG 3, 10992 B) |
| SE Aufschlag Brand 0x01120001 | Code bestaetigt; **Datenquelle widerlegt** | RE2 @0x80020fd4/0x80021028/jal 0x8002102c; bytegleich in RE1.5 `ARMS11.VB`/`.EDH` (Satz 10, VAG 3, 11664 B) |
| Kinder Saeure Bild 0 | bestaetigt | 0x030F2000 @0x80021780/84, 0x040C2000 @0x800217a8/ac, 0x041D1800 @0x800217c8/d0 (jal @0x800218e4) |
| Kinder Saeure Bild 1..4 | bestaetigt | @0x800217f0/0x80021804, @0x80021888/90, @0x800218c8/cc, @0x80021920/24 |
| Saeure-Bewegung vel.y 240, vel.x 0, acc.x -23, acc.y 0 | bestaetigt (Werte); Bezugsrahmen fehlt | +0x18 = 0x8403 hat Bit 0x400 -> Bewegung im Rahmen der Runden-Matrix (Plan re2-fx-system.md §1, FUN_8001d894), s. §16 |
| Kinder Brand Bild 0 | bestaetigt | 0x040C2800 @0x80021094/98, 0x041D2700 @0x800210c0/c4; a2 = Platz+0x4C (NICHT Einheitsmatrix), a1 = 0, a3 = 0 |
| Bodenflammen 3 x 0x0505xxxx, Skala, Gier | Werte bestaetigt; **Bedingung widerlegt** | kommen IMMER (Status := 0x8000 @0x80020ff4-f8 vor dem Test @0x80021040-4c) |
| Flammen-Zusatz vel.x += rng%25, acc.y += rng%8, +0x4A := 1 | bestaetigt | @0x80021184-22c, @0x8002135c, @0x800214b4 |
| Bodenflamme Skript Bank 5 Skr. 5 @0x07F8/0x0800 | bestaetigt | Bytes `00 1b 2e 32 00 10 00 10 00 05 00 00 60 00 ...` |
| Luft-Wachsen 8 + rng%3, x1.010/x1.007 | bestaetigt | @0x8001fbb4-b8, @0x80022350-84 |
| Landung Op A 19, Op B 29, vel.x 180, acc.x -10-rng%11, 38+rng%8 | bestaetigt; Dauer-Aussage praezisiert | +0x1B wird bei der Landung nicht gesetzt (§11) |
| Brennen >=16 / >0x1000 / x1.009,x1.002 / 90+rng%11 / x0.99,x0.98 | bestaetigt | @0x8001f2e8, @0x8001f2fc, @0x8001f40c-444, @0x8001f4c0, @0x8001f3a4-3e4 |
| Folgeflamme 0x0504xxxx, x0.8, alle 15 Bilder, vel.x >= 61 | bestaetigt | @0x8001fd78, @0x8001fd94-b8, @0x8001fdf0 |
| Nachbrenner Box {-600,0,300,150}, y-100, 0x2002000A | bestaetigt | @0x80010910 `a8 fd 00 00 2c 01 96 00`, @0x800207a4, @0x80020794/a0 |
| RE2-Hitcodes Saeure 0x1002000B / Brand 0x0002000A | bestaetigt | @0x800216e4/f0, @0x80021058/64 |
| RE2-Aufschlag-Boxen Wand/Boden {-1200,0,600,300}, Gegner {-600,0,300,150} | **widerlegt** (Gegner-Box) | In Op 48 UND Op 49 wird Bit 0x80 nach dem eigenen Status-Schreiben getestet -> immer {-1200,0,600,300}; die Gegner-Box ist toter Code |

## 16. Fehlende Mechanismen (Vollstaendigkeit fuer den Bau)

1. **Bodenflammen-Bedingung / Box-Wahl** (Korrektur, §9/§10): Brand spawnt IMMER 3 Bodenflammen, Brand und Saeure treffen IMMER mit
   {-1200,0,600,300}. Fuer den Port faellt damit die Frage "Wand/Boden oder Gegner?" (die es beim RE1.5-Zuender ohnehin nicht gibt) weg.
   Dasselbe Fehlurteil steht im Nachbar-Dossier `re_gegner_re2_familie.md` §1.1 (Z. 118-150, "Bit 0x80 = 1: Box {-600,0,300,150}").
2. **Tonquelle aus RE1.5** (§12): `shared_assets/PSX/SOUND/ARMS10.VB` (Saeure) und `ARMS11.VB` (Brand) = RE2 ARMS0B/0A.VB bytegleich,
   EDH Satz 10 -> Prog 0 Ton 3 -> VAG 3, Tonattribut bytegleich. Im Dossier nicht erkannt ("fehlen").
3. **Was von Routine 31 fuer 10/11 bleibt**: Das Dossier legt nur den Resolver-Aufruf (a2 = 3/4) fest. Offen fuer den Bau: Laerm-Riegel
   `sb v0,21336(at)` (0x800B5358 := 1) @0x8001857c, +0x6C := 0x61 @0x80018584, HE-Kinder 0x03195000 (Zuender 7 und 2), 0x030B5400
   (Zuender 2), 0x030B5800 (Zuender 0), HE-SE 0x04080001, Platzfreigabe bei Zuender 0 (@0x800186b0) — laufen sie bei 10/11 weiter,
   oder uebernimmt der RE2-Aufschlag ab Zuender 7? Ohne Festlegung spielt der Bau entweder HE-Rauch UND Saeure/Brand oder er erfindet eine Grenze.
4. **Uebergabe RE1.5-Granatenplatz -> RE2-FX-Platz**: Op 48/49 lesen vom eigenen Platz +0x22 (Gier; Brand-Flammen-Gier = +0x22 + rng),
   +0x0B (Lebensdauer; == 255 = Wasser-Sonderfall mit y-470/-600), +0x34/36/38 (Weltlage), +0x4C.. (Matrix). Die Port-Zuordnung
   (+0x22 <- RE1.5 +0x2E Gier aus `lh a1,-13634` 0x800ACABE @0x800336cc; +0x0B != 255; Lage = RE1.5 +0x28/+0x2A/+0x2C oder die
   Explosionslage y-500?) ist im Dossier nur fuer die Lage genannt.
5. **Bezugsrahmen der Bewegung** (RE2 FUN_8001d894, Plan `analysis/konstruktion_2026-08-23/re2-fx-system.md` §1): Status 0x8403
   (Saeure-Spritzer) hat Bit 0x400 -> `welt = M.rot*(basis*lokal + offset) + M.t` mit M = Matrix der Runde (Waffenknochen beim Schuss);
   vel.y 240 heisst dort "entlang der Laufrichtung". Die Bodenflammen (Status 0xB003, ohne 0x400/0x800) bewegen sich
   `welt = M.t + RotY(+0x22)*lokal` (Plan-Formel; M.t = Aufschlaglage aus Op 48). Die RE1.5-Granate hat keine Waffen-Matrix; der Bau
   braucht eine Zuordnung fuer M.rot (z. B. aus der Wurf-Gier). Fehlt im Dossier.
6. **Boden-/Wand-/Wasser-Abfrage der Flammen**: Ops 27/28/29 rufen RE2 `jal 0x8004fba0(P,2,8192,0)` (schreibt 0x800DCBC8 @0x8004fc34/
   0x8004fc58/0x8004fd18/0x8005000c/0x8005008c — eigener Suchlauf) und `jal 0x800527b4` (Wasserspiegel). Semantik (Rueckgabe =
   Bodenhoehe? 0x800DCBC8 = Wandkontakt?) nicht im Dossier; der Port muss sie auf RE1.5-Boden/Kollision abbilden.
7. **RE2-Stempel bei Resolver-Weg**: Wird der Schaden ueber FUN_80012d60 (Art 3/4) zugestellt, braucht die RE2-KI trotzdem +0x1D2
   (Spalte = Zone + 3*Klammer) und +0x1D3 (Sperre). Retail: Saeure-Stoss Klammer 1 (0x1002000B >> 28), Brand-Stoss Klammer 0,
   Nachbrenner Klammer 2 (Nachbar-Dossier re_gegner_re2_familie.md §1.2). Welche Klammer der Bau bei Art 3/4 stempelt, sagt das Dossier nicht.
8. **RE1.5-KI-Reaktion auf +0x05 = 10/11**: laut `re_schaden_resolver.md` §8.1 (Z. 554) sind die Zeilen 10/11 der Zombie-Familie
   ebenfalls NULL (wie 9) -> im Original `jalr` auf 0. Das Dossier verweist nur auf die Gegner-Dossiers; fuer den Bau heisst das:
   RE1.5-Flavor braucht dieselbe Beta->Retail-Entscheidung wie bei 9.
9. **re2z_row_from_atktype[2] = 17** (Handgranate) widerspricht der Nachbar-Vorgabe "RE2-Zeile 9"; beim Umbau von [3]/[4] mitziehen.
10. **Flug-Aussehen**: Die geworfene Granate ist fuer alle drei derselbe Effekt 4 Sub 0x0D (CLUT-Zeile +1). RE1.5 hat fuer die Hand keine
    Farbvariante, RE2 keine Wurfgranate -> bleibt olivgruen; nur als Hinweis (keine Quelle fuer eine Abweichung).
11. **Nebenbefund aca5d-Zensus**: VSync-Haken FUN_8007d904 (Aufruf @0x80061fe8) schreibt jede VSync Waffe 12 -> 19 und 13 -> 8 um
    (@0x8007d928-60). Betrifft 10/11 nicht, gehoert aber ins Waffen-Dossier (Ingram/SPAS).

## 17. Fazit

Die RE1.5-Seite des Dossiers haelt vollstaendig: Stub-Handler, Id-9-Gate, Byte +8 = Ziffern-CLUT, tote Spalten 10/11, Resolver-Werte
Art 3/4 = 1000 / Reaktion 10/11 mit nur zwei Aufrufern (Art 0, Art 2), Routinen 29/30/31 ohne Varianten-Lesen. Der aca5d-Zensus war
um 2 Leser unvollstaendig (`lui+ori`-Adressierung), die Aussage bleibt richtig. Die RE2-Werte (Op 17/15/48/49/27/58/28/46/19/29/30/40/50,
Hitcodes, Kinder, SE-Codes, EDH-Saetze, Skripte) stimmen Byte fuer Byte. **Zwei Fehler, die in Code wandern wuerden:**
(1) Das Status-Bit 0x80 wird in Op 48 und Op 49 erst NACH dem eigenen Status-Schreiben (0x8000 bzw. 0x8403) getestet — die
"Gegner"-Zweige sind tot: Brand legt IMMER drei Bodenflammen, beide treffen IMMER mit {-1200,0,600,300}.
(2) Die Aufschlagtoene fehlen dem Port nicht: RE1.5 hat sie selbst (ARMS10/ARMS11 = RE2 ARMS0B/0A, bytegleich).
Dazu eine Warnung (Id - 9 nur fuer RE2-Ids) und die fehlenden Bau-Festlegungen §16 (Routine-31-Rest, Platz-Uebergabe, Bezugsrahmen,
Boden-Abfrage, RE2-Klammer bei Resolver-Weg).
