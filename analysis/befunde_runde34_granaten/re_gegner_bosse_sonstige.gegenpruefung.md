# Gegenpruefung: re_gegner_bosse_sonstige.md (Runde 34, Granaten)

Pruefer: Skeptiker-Agent, 2026-09-29. Methode: jede Zeile der Tabelle "KONSTANTEN FUER DEN BAU" und jede
Mechanismus-Behauptung, die in Code wandern soll, selbst disassembliert (`re15_disasm.py` / `re2_disasm.py`),
Bytes selbst gelesen, Offsets NIE selbst gerechnet. Werkzeuge/Ausgaben: `re_gegner_sonst_gegen_werkzeug/`,
Laufzeit `build/r34g_gegner_sonst_gegen/` (untracked). Keine Aenderung unter re15_port/, keine git-Schreiboperation.

Urteile: **bestaetigt** / **widerlegt** (mit Korrektur) / **unklar** (mit dem, was fehlt).

## Gliederung

1. Resolver FUN_80012d60 (Typ 2): Aufruf, Gates, Schreiber, Tabellen 0x8006f418/0x8006f430
2. Gate B = +0x93-Riegel (Little-Endian-Behauptung)
3. Spawn-Voreinstellung +0x78 = 0x80072be0
4. Maggot 0x27 (Spuren, Clips, Rueckstoss, SE, Blut-Vektor)
5. Kakerlake 0x29 (Spuren, Clips, SE)
6. Alligator 0x23 (Tod, Kasten, Versatz FUN_8002b498)
7. Tyrant 0x2b (Schluck-Tabelle, Tod)
8. Birkin 0x30/0x36 RE1.5 (NULL-Zeilen)
9. Ivy 0x2d / FX-Emitter 0x24 / Writher 0x1a
10. NPCs (Kaesten, HP, Riegel, DEATH-Pfad)
11. RE2-Zeilen: G1 em30, G5 em36, Alligator em23, Ivy em2e, NPC-Ausschluss, Zeilen-Zuordnung, Klammer
12. Port-Abgleich (Datei:Zeile)
13. Fehlende Mechanismen (Vollstaendigkeit)
14. Fazit

(Abschnitt 10 steht wegen des fortlaufenden Speicherns direkt hinter 5.)

## 0. Kurzurteil je Zeile der Tabelle "KONSTANTEN FUER DEN BAU" (42 Zeilen)

| Zeile(n) | Urteil | Anmerkung |
|---|---|---|
| Explosionsschaden 1000 @0x8006f41c; Reaktions-Waffe 9 @0x8006f432; Art 3/4 | bestaetigt | Typ 3/4: kein `jal` und kein Datenwort-Zeiger auf 0x80012d60 |
| Treffer-Unterzustand; Gate B | bestaetigt | Gate B = (+0x93&3)==3, Port-Folge praezisiert (§2) |
| Spawn-Hitbox {0,0,0,1,1,1} @0x80072be0 | bestaetigt | liegt IN der Dispatch-Tabelle (Typ 0x0d..0x0f), live bestaetigt |
| Maggot HURT/DEATH-Spur, Clips 10/11 und 16/17, Rueckstoss, SE 0 | bestaetigt | von hinten netto ~0 Rueckstoss |
| Kakerlake Spuren (STAGE3/4/5), Clip 10/11, SE 7; Blut-Vektor | bestaetigt | Vektor ist i32, nicht s16 |
| Alligator-Tod; Kasten-Versatz | bestaetigt | Wurzel ruft FUN_8002b498 @0x8010c4e4 |
| Tyrant Schluck-Tabelle; Tyrant Tod | bestaetigt | Port-Ph.2-Clip falsch (§12) |
| Birkin Zeile 9 NULL | bestaetigt | gelesenes Wort = Zeile+4 (Spalte 1) |
| Ivy-Riegel 3; FX-Riegel 1; Writher 9..11 NULL | bestaetigt | |
| NPC-Kasten 0x45/0x4b; NPC-HP -1 | bestaetigt | 0x4b live |
| RE2 NPC-Ausschluss | bestaetigt mit Einschraenkung | skriptgesteuerter verwundbarer Modus (0x80058dac) |
| RE2 Zeilen-Zuordnung; RE1.5->RE2-Klammer | bestaetigt | Itemnamen aus DEBUG.BIN gegengelesen |
| RE2 G1 HP/Zuschlag/Schwelle/Clip/Saeure-SE/GL-Schaden | bestaetigt | Zerfall, HP<200-Zwang, Umgehung fehlen (§11d) |
| RE2 G5 Flinch-Byte/Schwelle/GL-Schaden/HP | bestaetigt | Port-Zerfall 15 statt 16, Zuschlag nach Waffe (§11e) |
| RE2 Alligator Grossreaktion/GL-Schaden | bestaetigt | Vorbedingung +0x225 fehlt im Text |
| RE2 Ivy HURT/DEATH je Zeile | bestaetigt | |

---

## 1. Resolver FUN_80012d60 (Typ 2) — BESTAETIGT (selbst disassembliert, PSX.EXE)

Aufrufstelle (Routine 31): `80018598 ori a0,zero,0x1f4` (Radius 500), `800185a8 addiu v0,v0,-500` (Punkt-Y = slot+0x2a - 500),
`800185b4 ori a2,zero,0x2`, `800185b8 jal 0x80012d60`. Der Rueckgabewert (v0 = s4&0xff, Trefferzahl) wird danach NICHT gelesen
(`800185c0 lui a0,0x319` ... `800185c8 lui v0,0x800b` ueberschreibt v0).

Gegner-Zweig, eigene Lesung (alle Adressen = Instruktion selbst, nicht Decompilat):

| Dossier-Behauptung | eigene Bytes/Instruktion | Urteil |
|---|---|---|
| Kandidat: +0x0 Bit0 + `jal 0x8002b5d0` | `80012db4 lw v0,0(s0)` / `80012dbc andi v0,v0,0x1` / `80012dd0 jal 0x8002b5d0` (a1 = Punkt, a2 = Radius&0xffff im Delay-Slot @80012dd4), Schritt `80012dfc addiu s0,s0,500` | bestaetigt |
| Rueckwaerts-Reihenfolge | `80012f24 addiu s2,s2,-1` / `80012f34 lw s1,16(v0)` (Liste[s2]) / `8001302c bne v0,zero` mit Delay-Slot `80013030 addiu s2,s2,-1` | bestaetigt |
| Gate A | `80012f40 lw v0,392(s1)` / `80012f44 lw v1,116(v1)` (v1 = *(0x800b52c4)) / `80012f48 addiu v0,v0,64` / `80012f4c beq v0,v1,0x8001302c` | bestaetigt |
| Gate B | `80012f54 lw v0,144(s1)` / `80012f58 lui v1,0x300` / `80012f5c and v0,v0,v1` / `80012f60 beq v0,v1,0x8001302c` | bestaetigt |
| SE nur Typ < 2 | `80012f68 sltiu v0,s0,0x2` / `80012f6c beq v0,zero,0x80012f7c` / `80012f74 jal 0x800453d0` (a0 = 0xa im Delay-Slot) | bestaetigt |
| Riegel `+0x93 &= 1` | `80012f7c lbu` / `80012f84 andi v0,v0,0x1` / `80012f88 sb v0,147(s1)` | bestaetigt |
| Seite `|= 0x80` | `80012f94 jal 0x8001a7a8` / `80012f9c beq v0,zero` / `80012fac ori v0,v0,0x80` / `80012fb0 sb` | bestaetigt |
| schon getroffen -> nur `|= 2` | `80012fbc andi v0,v1,0x1` / `80012fc0 beq v0,zero,0x80012fd0` / Delay `80012fc4 ori v0,v1,0x2` / `80012fc8 j 0x80013024` / Delay `80012fcc sb v0,147(s1)` | bestaetigt (Treffer zaehlt trotzdem: Sprungziel 0x80013024 = `addiu s4,s4,1`) |
| +0x7 = 0 / +0x6 = 1 | `80012fd0 ori v0,zero,0x1` / `80012fd4 sb zero,7(s1)` / `80012fd8 sb v0,6(s1)` | bestaetigt |
| +0x5 = DAT_8006f430[Typ] | `80012fe0 addiu at,at,-3024` (0x8006f430) / `80012fe4 addu at,at,s0` / `80012fe8 lbu v0,0(at)` / `80012ff0 sb v0,5(s1)` | bestaetigt |
| HP -= DAT_8006f418[Typ] | `80012f18 addiu v1,v1,-3048` / `80012f1c sll v0,s0,1` / `80012f20 addu s3,v0,v1`; `80012ff4 lhu a0,0(s3)` / `80012ffc subu v1,v1,a0` / `80013000 sh v1,154(s1)` | bestaetigt |
| +0x93 |= 1; +0x4 = 2 (Delay-Slot), 3 bei HP < 0 | `80013004 lh v1,154(s1)` (signiert) / `8001300c sb v0,147(s1)` / `80013014 bgez v1,0x80013024` / Delay `80013018 sb v0(=2),4(s1)` / `80013020 sb v0(=3),4(s1)` | bestaetigt |

Tabellen (`read`/`bytes`, PSX.EXE):
`0x8006f418` = `0a 00 14 00 e8 03 e8 03 e8 03 32 00 64 00 c8 00 2c 01 e8 03 00 00` = {10,20,1000,1000,1000,50,100,200,300,1000,0} -> **Typ 2 @0x8006f41c = 1000** bestaetigt,
Typ 3/4 @0x8006f41e/0x8006f420 = 1000/1000 bestaetigt.
`0x8006f430` = `03 03 09 0a 0b 0e 0f 10 11 12 14` -> **Typ 2 @0x8006f432 = 9**, Typ 3/4 @0x8006f433/34 = 0x0a/0x0b bestaetigt.

Folgerung 1 (+0x5 = Waffen-Id): `80011f58 addu fp,a0,zero` / `800124bc sb fp,5(s1)`; Zeilenadresse `800124b8 addiu a0,a0,-7984`
(0x8006e0d0) + Typ*0x58 (`800124c0..d0`: v1*3*4-v1 = v1*11, *8 = v1*88) + `800124dc sll v1,v1,2` (Waffe*4) -> bestaetigt.

Waffen-Tabelle (nur Referenz): Zeilen 0x1a/0x22/0x23/0x24/0x27/0x29/0x2b/0x2d/0x30/0x36 Spalten 9/10/11 und 15/16/17 selbst gelesen,
alle Werte wie im Dossier (z.B. 0x27 @0x8006ee38: `.. 40, 40, 70, .. 40, 40, 70`; 0x36 @0x8006f360: 22 x 0).
**Kleiner Fehler (nicht baurelevant):** "0x40..0x4d NPC | ab 0x8006f3b8" ist falsch. 0x8006f3b8 ist die Zeile **0x37**;
Zeile 0x40 laege bei 0x8006e0d0 + 0x40*0x58 = 0x8006f6d0 (und ueberlappt dort schon fremde Daten, die Tabelle 0x8006f418 beginnt
direkt nach Zeile 0x37). Aussage "fuer NPCs gibt es keine echte Zeile" bleibt richtig.

## 2. Gate B = +0x93-Riegel — BESTAETIGT

`lw v0,144(s1)` laedt +0x90..+0x93 als Little-Endian-Wort; Maske `lui v1,0x300` = 0x03000000 = Bits 24/25 = Byte +0x93 Bits 0/1.
Die Wort-Adresse ist ausgerichtet (Gegner-Basis 0x800acc2c, Schritt 500 = 4*125). Gleiche Probe in FUN_80011f50:
`800120c0 lui s4,0x300` / `800120f4 lw v0,144(s0)` / `800120fc and v0,v0,s4` / `80012100 beq v0,s4,0x80012124` — bestaetigt.

**Praezisierung zur Port-Folge (§0.1 des Dossiers):** Ohne Gate B faellt ein Gegner mit (+0x93&3)==3 im Port in
`&= 1` -> Seitentest -> `|= 2`. Das ergibt nicht nur "Bits 2..7 geloescht": Bit 0x80 wird aus dem NEUEN Angriffspunkt
NEU berechnet (FUN_8001a7a8 laeuft), das Original laesst das alte Bit stehen. Ausserdem zaehlt der Port den Treffer
(Original: Gate-B-Sprungziel 0x8001302c liegt HINTER `addiu s4,s4,1` @0x80013024 -> kein Zaehler). Der Zaehler ist fuer die
Granate folgenlos (Rueckgabe ungenutzt, s.o.). Die Luecke im Port ist damit real, aber klein; Urteil zur Dossier-Aussage: bestaetigt,
mit dieser Ergaenzung.

## 3. Spawn-Voreinstellung +0x78 = 0x80072be0 — BESTAETIGT (statisch UND live), mit Warnhinweis

Sce_em_set (PSX.EXE): `800421e8 sb zero,147(s0)` (+0x93 = 0); `800422c8 lui v0,0x8007` / `800422cc addiu v0,v0,11232` /
`800422d0 sw v0,120(s0)` (+0x78 = 0x80072be0); `800422c4 lw s1,-14468(s1)` (*(0x800ac77c)) / `800422dc sw s1,124(s0)` (Delay-Slot
von `jal 0x8004ee60`) = +0x7c. `read 0x80072be0 6 --w 2` = `00 00 00 00 00 00 01 00 01 00 01 00` = {0,0,0,1,1,1}.

**Warnhinweis (fehlt im Dossier):** 0x80072be0 liegt INNERHALB der Typ-Dispatch-Tabelle 0x80072bac (Eintraege Typ 0x0d/0x0e/0x0f;
Beleg Dispatch FUN_8001a50c: `8001a548 addiu s3,s3,11180` = 0x80072bac, `8001a578 sll v0,v0,2`, `8001a580 lw v0,0(v0)`,
`8001a588 jalr v0`). Der Kasten bleibt nur deshalb {0,0,0,1,1,1}, weil keine Stage einen Typ < 0x10 registriert. Gegenprobe:
* statisch: eigenes `reg_sw_scan.py 0x80072bac 0x80072cf0` ueber PSX.EXE + STAGE1..6/DEBUG/TITLE -> niedrigster Schreibzugriff
  0x80072bec (Typ 0x10); **kein** Schreiber in 0x80072bac..0x80072beb (`build/r34g_gegner_sonst_gegen/reg_sw_scan.txt`);
* live: 4 saubere Savestates (`@0x80026e4c = 08 00 e0 03`): mzd_stage1_npc, mzd_stage1_maggot, room1090_orig, orig_1170_gp ->
  0x80072be0 = `00 00 00 00 00 00 01 00 01 00 01 00` (`ss_box.py`, `build/r34g_gegner_sonst_gegen/ss_box.txt`).
Fuer den Port heisst das: den Voreinstellungs-Kasten als eigene Konstante fuehren (nicht aus einer Typ-Tabelle ableiten).

Registrierungs-Wurzeln (Dossier §1) selbst gelesen: STAGE3 `8011cf40 addiu v0,v0,25136` (0x80116230) / `8011cf48 sw -> 0x80072c6c` (0x30)
/ `8011cf50 sw -> 0x80072c84` (0x36); STAGE5 `8011dd60` 0x80116a44 -> 0x80072c6c (0x30), **kein** STAGE5-Schreiber auf 0x80072c84 (0x36);
STAGE5 0x2b 0x80111a50 @0x8011dd58; STAGE2 0x22 0x8010c080 @0x80116f54, 0x23 0x8010c448 @0x80116f64, 0x24 0x8010ee9c @0x80116f74;
STAGE1 0x27 0x80116db8 @0x8011e90c, 0x1a v1 = 0x8010c1ec (@0x8011e880-84) @0x8011e9ac; STAGE4 0x2b 0x801118d0 @0x801183f8,
0x2d 0x801168c4 @0x80118408. Live-RAM STAGE1 (ss_box.txt) zeigt dieselben Werte (0x1a -> 0x8010c1ec, 0x27 -> 0x80116db8). **bestaetigt.**

Nebenbei live bestaetigt (STAGE1): NPC 0x40 +0x78 = 0x80121588 {0,-1530,0,450,1530,450}, 0x42 0x80121658 (gleich), 0x47 0x80121790
(gleich), **0x4b 0x801218c8 {0,-1440,0,300,1440,300}**, alle HP = -1, +0x93 = 0, Wort0 = 0x40000001; Maggot 0x27 +0x78 = 0x80121350
{0,-1440,0,1600,1440,1600}, HP 180; Feuer 0x26 (ROOM1090) 0x80121258 {0,0,0,600,720,600}, HP 100, +0x93 = 0.

## 4. Maggot/Gorilla 0x27 (STAGE1) — BESTAETIGT

| Behauptung | eigene Lesung | Urteil |
|---|---|---|
| Zustandstabelle 0x801213c8 [2]/[3] | Wurzel `80116e10 addiu at,at,5064` (0x801213c8) / `80116e20 jalr`; `table`: [2] 0x8011af5c, [3] 0x8011b6fc | bestaetigt |
| HURT-Dispatch auf +0x5 | `8011afe0 lbu v0,5(v0)` / `8011aff0 addiu at,at,5288` (0x801214a8) / `8011b000 jalr v0` | bestaetigt |
| DEATH-Dispatch auf +0x5 | `8011b780 lbu v0,5(v0)` / `8011b790 addiu at,at,5376` (0x80121500) / `8011b7a0 jalr v0` | bestaetigt |
| HURT-Spur 9..11, 15..18 -> 0x8011b400 | `table 0x801214a8 24`: [9..11] und [15..18] = 0x8011b400; 0..6/12/14/19/20 = 0x8011b018; 7/8/13/21 = 0x8011b1ec | bestaetigt (@0x801214cc..d4, @0x801214e4..f0) |
| DEATH-Spur 9..11, 15..18 -> 0x8011bb9c | `table 0x80121500 24`: [9..11], [15..18] = 0x8011bb9c; 7/8/13 = 0x8011b998; 0..6/12/14/19/20/**21** = 0x8011b7b8 | bestaetigt (@0x80121524..2c, @0x8012153c..48) |
| Tod Ph.0: +0x93|=2, +0x7=1, Clip 10/11 | `8011bbf0 ori v0,v0,0x2` / `8011bc04 sb v0(=1),7` / `8011bc10 ori v0,zero,0xa` / `8011bc14 sb v0,148` / `8011bc30 beq` + Delay `8011bc34 ori v0,zero,0xb` / `8011bc38 sb v0,148` | bestaetigt |
| +0x95=0, Crossfade 7, +0x8c=(rng&31)+80, +0x9c=0 | `8011bc48 sb zero,149`; `8011bc54 ori v0,zero,0x7` / Delay `8011bc5c sb v0,143`; `8011bc60 andi v0,v0,0x1f` / `8011bc6c addiu v0,v0,80` / `8011bc70 sh v0,140`; `8011bc80 sh zero,156` | bestaetigt |
| Blut 0x2000 an (+0x188)+0x40 mit Vektor | `8011bc7c ori a0,zero,0x2000`; `8011bca8 addiu at,at,5000` (0x80121388) + (+0x6&1)<<5; `8011bcf4 lw a2,392(v1)` / Delay `8011bd00 addiu a2,a2,64` / `8011bcfc jal 0x80019700` | bestaetigt |
| Blut-Vektor +0x6=1 @0x801213a8 | `bytes`: `c8 00 00 00 e0 fc ff ff 00 00 00 00 00 00 00 00` = **i32** {200,-800,0,0} (4 Worte werden nach sp+16..28 kopiert) | bestaetigt (Typ i32, nicht s16!) |
| SE 0 | `8011bd04 jal 0x800453d0` / Delay `8011bd08 addu a0,zero,zero` | bestaetigt |
| Todes-Flag | `8011bd18 lbu a1,454(v0)` / `8011bd20 addiu a0,a0,4152` (0x800b1038) / `8011bd24 jal 0x8004ef90` | bestaetigt |
| Ph.0 faellt in Ph.1 | nach `8011bd24 jal` (Delay nop) folgt direkt 0x8011bd2c | bestaetigt |
| Ph.1 Anim + Rueckstoss | `8011bd40 jal 0x8001f314` (a3 = 0x200) / `8011bd5c addu v1,v1,v0` / `8011bd60 sb v1,7`; `8011bd70 lh v1,156` / `8011bd74 ori v0,zero,0x50` / `8011bd78 sll v1,v1,2` / `8011bd7c subu` / Delay `8011bd84 sh v0,140`; `8011bd6c ori a0,zero,0x800` / `8011bd80 jal 0x800245d8`; bei +0x93&0x80 `8011bda8 jal 0x800245d8` (a0 = 0) | bestaetigt |
| Ph.2 Blut + Leiche | `8011bdb8`: a3 = 0, `8011bdc8 addiu a2,a2,580`, `8011bdc4 jal 0x80019700` (a0 = 0x2000 aus Delay-Slot `8011bbdc`); `8011bdd4 ori v0,zero,0x7` / `8011bdd8 sw v0,4(v1)` | bestaetigt |
| HURT-Spur 0x8011b400: Phasen, Clips, SE 3, Aufstehen 16/17, Ausgang | Sprungtabelle `8011b428 addiu at,at,1004` (0x801003ec) = {0x8011b440, 0x8011b570, 0x8011b5fc, 0x8011b660, 0x8011b69c}; `8011b474 ori 0xa` / `8011b498 ori 0xb`; `8011b568 jal 0x800453d0` / Delay `8011b56c ori a0,zero,0x3`; `8011b614 ori v0,zero,0x10` / Delay `8011b638 ori v0,zero,0x11`; `8011b6a8 sb zero,147` / `8011b6b8 sb 1,4` / `8011b6c8 sb 7,5` / `8011b6d8 sb zero,6` / `8011b6e8 sb zero,7` | bestaetigt |
| HP 180 | `801170d8 addiu a0,a0,-4044` (0x8011f034) + Typ<<5 + (rng&15)<<1; `read 0x8011f514 16 --w 2` = 16 x 180; live (mzd_stage1_maggot) HP = 180 | bestaetigt |
| Kasten je +0x1e2 | `801171b0 addiu at,at,4968` (0x80121368) / `801171c0 sw v0,120(v1)`; `table 0x80121368 8` = {338,344,338,344,350,35c,350,35c}; Kaesten {0,-1080,0,1100,1080,1100}, {0,-1530,0,800,1530,800}, {0,-1440,0,1600,1440,1600}, {0,-2160,0,1100,2160,1100} | bestaetigt |

**Ergaenzungen (fehlen im Dossier, fuer den Bau relevant):**
1. **Rueckstoss bei +0x93&0x80 = netto ~0.** FUN_800245d8 addiert nur x/z (`800246e4 sw v0,52(a0)`, `800246f8 sw v0,60(a0)`) aus
   (+0x8c,0,0) gedreht um +0x6a + a0 (`80024658 lh v0,106(v0)` / Delay `80024664 addu a0,v0,a0` -> `jal 0x800659d0`). Der zweite
   Aufruf mit a0 = 0 hebt den ersten (a0 = 0x800) also bis auf Rundung auf: von hinten getroffen -> Clip 11 **ohne** Rueckstoss.
   Das ist Original-Verhalten und muss im Port genau so bleiben (zwei Aufrufe, nicht "einer vorwaerts").
2. **HURT/DEATH-Eingang vor dem +0x5-Dispatch** (`8011af6c`/`8011b70c`): bei +0x1e0 != 0 (Decke/Wand) ruft der Handler
   `jal 0x8001c1a4 (0, 0, -50, +0x1ba)`; bei Erfolg `8011afa0 sb zero,480` (+0x1e0 = 0), `8011afd0 sw v0,120(v1)` (+0x78 = Kasten
   [+0x1e2] aus 0x80121368) und `8011afcc jal 0x800453d0` (a0 = 2 aus Delay-Slot `8011af90`). Gilt fuer jede Waffe, also auch fuer
   die Granate (eine Made an der Decke faellt herunter). Nicht granatenspezifisch, aber Teil der Reaktion.

## 5. Kakerlake 0x29 (STAGE3/4/5) — BESTAETIGT

| Behauptung | eigene Lesung | Urteil |
|---|---|---|
| STAGE3 Zustandstabelle 0x8011eca4 [2]/[3] | `table`: [2] 0x80114790, [3] 0x80114fb4 | bestaetigt |
| HURT-Tabelle 0x8011ed84 / DEATH 0x8011eddc | `80114824 addiu at,at,-4732` (0x8011ed84); `80115048 addiu at,at,-4644` (0x8011eddc) | bestaetigt |
| Spuren STAGE3 | `tabc.py`: HURT [9,10,11,15..18] -> 0x80114cb8, [7,8,13,21] -> 0x80114a40, Rest -> 0x8011484c; DEATH [9,10,11,15..18] -> 0x801154b4, [7,8,13] -> 0x80115280, [0..6,12,14,19,20,**21**] -> 0x80115070 | bestaetigt |
| STAGE4 relozierte Kopie | Zustandstabelle 0x80119ed4 ([2] 0x8010fe30, [3] 0x80110654); HURT liest `8010fec4 addiu at,at,-24652` (0x80119fb4), DEATH `801106e8 addiu at,at,-24564` (0x8011a00c); Spuren -> 0x80110358 / 0x80110b54 | bestaetigt |
| STAGE5 relozierte Kopie | Zustandstabelle 0x8011fa3c ([2] 0x8010ffb0, [3] 0x801107d4); HURT `80110044 addiu at,at,-1252` (0x8011fb1c), DEATH `80110868 addiu at,at,-1164` (0x8011fb74); Spuren -> 0x801104d8 / 0x80110cd4 | bestaetigt |
| Tod Clip 10/11 | `80115528 ori v0,zero,0xa` / `8011552c sb v0,148`; `80115548 beq` + Delay `8011554c ori v0,zero,0xb` / `80115550 sb` | bestaetigt |
| SE 7 | `8011561c jal 0x800453d0` / Delay `80115620 ori a0,zero,0x7` | bestaetigt |
| Todes-Flag je Bank | `80115628 addiu a0,a0,4064` (0x800b0fe0) / `8011562c lh v0,0(a0)` / `80115634 slti v0,v0,3` -> a0 = 0x800b0fe0+88 (`80115654`) bzw. +120 (`80115668`) -> `8011566c jal 0x8004ef90` | bestaetigt |
| Blut-Vektor +0x6=1 | `801155c0 addiu at,at,-5020` (0x8011ec64) + (+0x6&1)<<5; `bytes 0x8011ec84` = `c8 00 00 00 e0 fc ff ff 00 00 00 00 00 00 00 00` = i32 {200,-800,0,0} | bestaetigt |
| Ph.1 Rueckstoss / Ph.2 Leiche | `801156b4 ori a0,zero,0x800` / `801156c8 jal 0x800245d8` / Delay `801156cc sh v0,140`; `801156f0 jal 0x800245d8` / Delay `801156f4 addu a0,zero,zero`; `80115710 addiu a2,a2,580` / `8011571c ori v0,zero,0x7` / `80115720 sw v0,4(v1)` | bestaetigt (auch hier: von hinten netto ~0 Rueckstoss) |
| HP-Zeile | `80110ee4 addiu a0,a0,-11832` (0x8011d1c8) + Typ<<5 + (rng&15)<<1, `80110f10 sh v0,154`; `read 0x8011d6e8 16` = {81,109,97,83,99,113,87,101,117,89,91,103,121,93,105,95} | bestaetigt |
| Kasten je +0x1e4 | `80110f68 lbu v0,484(v1)` / `80110f78 addiu at,at,-5052` (0x8011ec44) / `80110f88 sw v0,120`; `table 0x8011ec44 8`: [0]/[2] 0x8011ec14 {0,-1080,0,1100,1080,1100}, [1]/[3]/[5]/[7] {800,1530} (0x8011ec20/0x8011ec38), [4]/[6] 0x8011ec2c {0,-1080,0,800,1080,800} | bestaetigt (Tabelle hat 8 Eintraege, 3 verschiedene Kaesten) |
| +0x93 = 0 im INIT | `80110d14 sb zero,147(v0)` | bestaetigt |

## 10. NPCs 0x40..0x4d (STAGE1) — BESTAETIGT, Folge praezisiert

| Behauptung | eigene Lesung | Urteil |
|---|---|---|
| 0x45 Kasten {0,-1440,0,500,1440,500} | `8011d2f8 lw v0,5940(v0)` (*(0x80121734) = 0x80121728) / `8011d300 sw v0,120(v1)`; `read 0x80121728 6` = {0,-1440,0,500,1440,500} | bestaetigt |
| 0x4b Kasten {0,-1440,0,300,1440,300} | `8011e3b0 lw v0,6356(v0)` (*(0x801218d4) = 0x801218c8) / `8011e3b8 sw v0,120(v1)`; live mzd_stage1_npc Slot 2: +0x78 = 0x801218c8 {0,-1440,0,300,1440,300} | bestaetigt (statisch + live) |
| NPC-HP -1 | 0x45 `8011d320 addiu v0,zero,-1` / `8011d324 sh v0,154(v1)`; 0x4b `8011e3d8` / `8011e3dc`; live alle NPC HP = -1 | bestaetigt |
| +0x93 = 0 | 0x45 `8011d3cc sb zero,147(v0)`; 0x4b `8011e484 sb zero,147(v0)` | bestaetigt |
| Wort0 |= 0x40000000 | `8011d2e0 lui v1,0x4000` / `8011d2e4 or` / `8011d2e8 sw v0,0(a0)`; live Wort0 = 0x40000001 | bestaetigt |
| 0x45 DEATH verzweigt auf +0x6 | Zustandstabelle 0x80121738: [0] 0x8011d2b8, [2] 0x8011d5ac, [3] 0x8011d5f4, [4] 0x80050be8; DEATH `8011d604 lbu v0,6(v0)` / `8011d614 addiu at,at,5992` (0x80121768) / `8011d624 jalr`; `table 0x80121768`: [1] = 0x80050ddc | bestaetigt |
| 0x80050ddc spielt Clip, am Ende +0x6 = 2 | `80050df4 beq v1(+0x6),1 -> 80050e58`; `80050e70 jal 0x8001f314`; `80050ec4 ori v0,zero,0x2` / `80050ec8 sb v0,6(v1)`; bei +0x1c4&4 `80050eec sb 1,6` (Schleife) | bestaetigt |

**Praezisierung (Dossier sagt nur "naechstes Bild [2] = 0x80050f00 ..."):** 0x80050f00 prueft selbst +0x6:
`80050f10 lbu v1,6(a0)` / `80050f18 beq v1,1` / `80050f20 slti v0,v1,2` / `80050f24 beq v0,zero,0x80051014` (-> `jr ra`).
Mit +0x6 = 2 kehrt der Handler also SOFORT zurueck, jeden Frame. Kein Handler [0..7] der Tabelle schreibt +0x4 (`scan` aller
acht: nur +0x5/+0x6-Schreiber). Ergebnis im Original: **der NPC spielt seinen laufenden Clip zu Ende (bzw. endlos, wenn
+0x1c4&4) und steht danach fuer immer in Zustand 3** (eingefroren). Das stuetzt die Einordnung "unfertig".

Uebrige NPC-DEATH-Dispatcher selbst gelesen (alle auf +0x6, alle [1] = 0x80050ddc): 0x40 DEATH 0x8011ca90 `8011caa0 lbu v0,6` /
`8011cab0 addiu at,at,5680` (0x80121630); 0x42 0x8011d060 -> 0x80121700; 0x47 0x8011db88 -> 0x801217d0; 0x4b 0x8011e784 `8011e794 lbu v0,6` /
`8011e7a4 addiu at,at,6536` (0x80121988); STAGE6 0x4d Wurzel 0x801017a0 -> Zustandstabelle 0x80102794, [3] 0x80101ccc `80101cdc lbu v0,6` /
`80101cec addiu at,at,10284` (0x8010282c), [1] = 0x80050ddc. **bestaetigt.**

## 6. Alligator 0x23 (STAGE2) — BESTAETIGT

| Behauptung | eigene Lesung | Urteil |
|---|---|---|
| Zustandstabelle 0x80118bc8 | `8010c4c4 addiu at,at,-29752`; [2] 0x8010e570, [3] 0x8010e9e8 | bestaetigt |
| HURT-/DEATH-Tabelle | HURT `8010e5a0 lbu v0,5(v1)` / `8010e5b0 addiu at,at,-29588` (0x80118c6c); DEATH `8010e9f8 lbu v0,5(v0)` / `8010ea08 addiu at,at,-29500` (0x80118cc4) | bestaetigt |
| HURT 0..6/12/14/19/20 -> 0x8010e5d8, 7..11/13/15..18/21 -> 0x8010e748; DEATH alle 22 -> 0x8010ea30 | `tabc.py` | bestaetigt |
| Wasser-Vorpruefung HURT | `8010e580 lbu v0,480(v1)` / `8010e588 beq` / `8010e590 jal 0x8010e91c` | bestaetigt |
| INIT +0x93 = 0, HP 300, Kasten | `8010c5c0 sb zero,147(v0)`; `8010c6a4 addiu a0,a0,29052` (0x8011717c) ... `8010c6d4 sh v0,154(a1)`, `read 0x801175dc 16` = 16 x 300; `8010c700 lw v0,-29776(v0)` (*(0x80118bb0) = 0x80118b98) / `8010c708 sw v0,120(v1)` | bestaetigt |
| Kasten-Bytes | `bytes 0x80118b98 12` = `e8 03 30 fd 00 00 98 08 d0 02 20 03` = {1000,-720,0,2200,720,800} | bestaetigt |
| Tod Ph.0 | `8010ea88 sb (+0x93 or 2)`; `8010ea98 sb 1,7`; `8010eaa4 ori v0,zero,0xd` / `8010eaa8 sb v0,148`; `8010eab8 sb zero,149`; `8010eac4 ori v0,zero,0x7` / `8010eac8 sb v0,143`; `8010ead8 sh zero,140`; `8010eae8 sh zero,156`; `8010eaf8 lbu v1,130` ... `8010eb14 subu v0,zero,v0` / `8010eb18 sh v0,442` (v1*7, *32, +v1 = 225*v1, *8 = 1800*v1 -> -(+0x82)*1800) | bestaetigt |
| Blut + Flag | a0 = 0x2000 aus Delay-Slot `8010ea5c` (bis `8010eb30` nicht ueberschrieben), `8010eb2c lh a1,106` / `8010eb30 jal 0x80019700` / Delay `8010eb34 addiu a2,a2,2644`, a3 = 0; `8010eb4c addiu a0,a0,4184` (0x800b1058) / `8010eb50 jal 0x8004ef90` (a1 = +0x1c6) | bestaetigt |
| Ph.1: Bild 20 -> Ph.2, ab Bild 21 Bodenfolge | `8010eb9c lbu v0,480(a0)` / `8010eba4 bne` / Delay `8010eba8 ori v0,zero,0x14` / `8010ebb4 bne v1(+0x95),v0` / `8010ebbc sb 2,7`; `8010ebd4 sltiu v0,v0,0x15` / `8010ebe0 lh a0,140` / `8010ebe4 lh a3,442` / `8010ebe8 jal 0x8001c1a4` (a1 = 0, a2 = -80 im Delay-Slot) | bestaetigt |
| Ph.2 Blut + Leiche | `8010ebfc..08` Blut (a0 = 0x2000 aus Delay `8010ea70`, a2 = (+0x188)+2644); `8010ec14 ori v0,zero,0x7` / `8010ec18 sw v0,4(v1)` | bestaetigt |

**Kasten-Versatz — Mechanismus selbst gelesen (Dossier-Nachtrag §5 BESTAETIGT, mit Ergaenzung):**
FUN_8002b498: `8002b4bc lw s0,120(s1)` (Kasten), `8002b4c0 lw s2,124(s1)` (+0x7c-Puffer), Drehvektor (0, +0x6a, 0) -> `8002b4d4 jal 0x80068098`
(Matrix), dann Vektor (Kasten.x, [Yaw bleibt in sp+18 stehen], Kasten.z) (`8002b4e0 lhu v0,0(s0)` / `8002b4ec lhu v0,4(s0)`) ->
`8002b4f4 jal 0x800661c0` -> `8002b504 sh +0x7c[0]` = gedrehtes x, `8002b508 lhu v0,2(s0)` / `8002b510 sh +0x7c[1]` = Kasten.y (ungedreht),
`8002b51c sh +0x7c[2]` = gedrehtes z, `8002b520 sb zero,450(s1)`. (Der y-Eintrag des Drehvektors ist die Yaw-Zahl; bei einer reinen
Y-Drehmatrix wirkt er nur auf das verworfene Ausgangs-y.)
**Leser FUN_8002b5d0** nimmt den Mittelpunkt aus +0x7c, nicht aus +0x78: `8002b60c lw s5,124(s3)`; x-Mitte `8002b630 lh a1,0(s5)` + `8002b63c lw v0,52(s3)`,
z-Mitte `8002b624 lh a0,4(s5)` + `8002b628 lw v0,60(s3)`, y-Mitte `8002b784 lh v0,2(s5)` + `8002b77c lw v1,56(s3)`. Kreis bei
`8002b61c beq v1(box+6),v0(box+0xa)`, sonst Winkel ueber `jal 0x80065de0` und Interpolation mit `jal 0x800683e8`/`jal 0x80068348`;
Summe mit Angriffsradius `8002b704 addu s0,v0,s1`, Achsen-Vorpruefung `8002b724/48 sltu`, Kreis `8002b764 jal 0x80065f60`, Hoehe
`8002b778 lhu v0,8(s2)` + Radius, `8002b798 slt` / `8002b7a0 slt`.
**Die Alligator-Wurzel ruft FUN_8002b498 jeden Frame** (`8010c4e4 jal 0x8002b498`, nach `8010c4d4 jalr v0` = Zustands-Handler).
Eigener `jal_find.py 0x8002b498`: 58 Aufrufer (EXE 0x80031d58 + Stage-Wurzeln), `build/r34g_gegner_sonst_gegen/jal_8002b498.txt`.
Port: `re15_damage.c:3552 a->hit_offset_x = 0;` (Alligator ohne x = 1000 und ohne Drehung); `re15_hitbox_test` (`:3276`) addiert den
Versatz ungedreht (`target->x + target->hit_offset_x`). **Luecke bestaetigt.**

## 7. Tyrant 0x2b (STAGE4) — BESTAETIGT

| Behauptung | eigene Lesung | Urteil |
|---|---|---|
| Zustandstabelle 0x8011a0b4 | `8011192c addiu at,at,-24396`; [2] 0x80114770, [3] 0x80114c68 | bestaetigt |
| Schluck-Test | `80114780 lh v0,476(a0)` / `80114788 beq`; `8011479c addiu at,at,-24152` (0x8011a1a8) / `801147a4 lbu` / `801147ac beq`; `801147b4 lbu v1,6(a0)` / `801147bc bne v1,1`; `801147c4 lbu v0,147` / `801147cc andi 0x80` / `801147d0 bne`; Delay `801147d4 ori v0,zero,0x601` / `801147d8 sw v0,4(a0)`; `801147dc jal 0x800453d0` / Delay `801147e0 ori a0,zero,0xb`; `801147f0 lhu v0,476` / `801147f8 sh v0,154`; Delay `80114808 sb zero,147` | bestaetigt |
| Schluck-Tabelle [9..11] = 0 | `bytes 0x8011a1a8 24` = `01 01 01 01 01 01 01 00 00 00 00 00 01 00 01 00 00 00 00 01 01 00` -> @0x8011a1b1..b3 = `00 00 00` | bestaetigt |
| HURT-Tabelle 0x8011a1c0 | `80114828 addiu at,at,-24128`; `tabc`: 0..6/12/14/19/20 -> 0x80114850, 7..11/13/15..18/21 -> 0x80114a40 | bestaetigt |
| DEATH-Tabelle 0x8011a218: alle -> 0x80114cb0 | `80114c78 lbu v0,5(v0)` / `80114c88 addiu at,at,-24040` / `80114c98 jalr`; `tabc`: [0..21] -> 0x80114cb0 | bestaetigt |
| Tod-Phasen | Phasen-Sprungtabelle `80114cd8 addiu at,at,836` (0x80100344) = {0x80114cf0, 0x80114e2c, 0x80114f00, 0x80114f64, 0x80114fa0} | bestaetigt |
| Clip 8/9, SE 2, Flag | `80114d04 ori v0,v0,0x2`; `80114d24 ori v0,zero,0x8`; `80114d44 beq` + Delay `80114d48 ori a0,zero,0x2000` + `80114d4c ori v0,zero,0x9` / `80114d50 sb` (nur bei +0x93&0x80); `80114d60/70/80` +0x95/+0x96/+0x8f = 0; `80114e04 jal 0x800453d0` / Delay `80114e08 ori a0,zero,0x2`; `80114e20 addiu a0,a0,4184` / `80114e24 jal 0x8004ef90` | bestaetigt |
| Bild 24 -> SE 7, Ph.2 Clip 0xa/0xb, Leiche | `80114ed8 ori v0,zero,0x18` / `80114ee4 jal 0x800453d0` / Delay `80114ee8 ori a0,zero,0x7` / `80114efc sb 2,7`; `80114f18 ori v0,zero,0xa`, Delay `80114f3c ori v0,zero,0xb`; `80114f5c/60` Crossfade 7; `80114fcc ori v0,zero,0x7` / `80114fd0 sw v0,4(v1)` | bestaetigt |
| INIT: +0x93 = 0, HP-Zeile, Kasten | `80111ab8 sb zero,147(v0)`; `80111bd8 addiu a0,a0,-31328` (0x801185a0) ... `80111c04 sh v0,154`; `read 0x80118b00 16` = {86,89,103,119,91,107,121,93,109,124,117,97,113,126,99,101}; `80111c30 lw v0,-24404(v0)` (*(0x8011a0ac) = 0x8011a094) / `80111c38 sw v0,120(v1)`; Kasten {0,-1710,0,800,1710,800} | bestaetigt |

## 8. Birkin 0x30/0x36 RE1.5 (STAGE3/STAGE5) — BESTAETIGT (NULL-Zeilen und Haenger)

| Behauptung | eigene Lesung | Urteil |
|---|---|---|
| STAGE3-Wurzel dispatcht 0x8011ee84 | `8011644c addiu at,at,-4476` (0x8011ee84) / `8011645c jalr v0`; `80116490 jal 0x8002b498` | bestaetigt |
| Zustandstabelle | [0] 0x801166e0, [1] 0x80116d38, [2] 0x8011a060, [3] 0x8011a3f0, [4] 0x8011a7b4, [5]/[6] = 0, [7] 0x8011a7bc | bestaetigt |
| INIT HP 300, Kasten, kein +0x93-Schreiber | `8011690c ori v0,zero,0x12c` / `80116910 sh v0,154(v1)`; `80116764 lw v0,-4484(v0)` (*(0x8011ee7c) = 0x8011ee64) / `8011676c sw v0,120(v1)`; INIT 0x801166e0..0x80116d30 (`jr ra`) ohne `...,147(` (Dump `build/r34g_gegner_sonst_gegen/birkin_s3_init.txt`) | bestaetigt |
| HURT-2D-Dispatch | `8011a23c addiu a0,a0,-4284` (0x8011ef44) / `8011a240 lbu v1,5(v0)` / `8011a244 lbu v0,6(v0)` / `8011a248 sll v1,v1,5` / `8011a250 sll v0,v0,2` / `8011a258 lw v0,0(v0)` / `8011a260 jalr v0` | bestaetigt |
| DEATH: +0x7 != 0 -> sofort Tabelle; +0x7 == 0 und +0x1dd&8 -> HP 50 | `8011a434 lbu v0,7(a2)` / `8011a43c bne v0,zero,0x8011a56c`; `8011a444 lbu v0,477(a2)` / `8011a44c andi v0,v0,0x8` / `8011a450 beq`; `8011a530 ori v0,zero,0x32` / `8011a534 sh v0,154`; `8011a54c andi v0,v0,0xfe`; `8011a560 lw v0,472` / `8011a568 sw v0,4` | bestaetigt |
| DEATH-2D-Dispatch | `8011a594 addiu a0,a0,-3612` (0x8011f1e4) / `8011a598 lbu v1,5` / `8011a59c lbu v0,6` / `8011a5a0 sll v1,v1,5` / `8011a5a8 sll v0,v0,2` / `8011a5b0 lw` / `8011a5b8 jalr` | bestaetigt |
| Tabellen STAGE3 | `tabc.py --rowbytes 32 --cols 8`: HURT Zeilen 1,3..8,19 Sp. 0..2 -> 0x8011a284, alle anderen Zeilen (0,2,9..18,20) komplett 0; DEATH Zeilen 1,3..8,19 Sp. 0..2 -> 0x8011a5d8, Zeilen 0,2,9..18,20,21 = 0; `bytes 0x8011f304 12` und `bytes 0x8011f064 12` = 12 x `00` | bestaetigt |
| Mutations-Bit | `80118094 ori v0,zero,0x96` / `80118098 sh v0,154`; `801180b0 ori v0,v0,0x8` / `801180b4 sb 477`; `801188c0 andi v0,v0,0xf7` | bestaetigt |
| STAGE5-Kopie | Wurzel `80116c60 addiu at,at,-440` (0x8011fe48) / `80116c70 jalr`; [2] 0x8011a874, [3] 0x8011ac04; `8011aa50 addiu a0,a0,-248` (0x8011ff08), `8011ada8 addiu a0,a0,424` (0x801201a8); Zeilen 9..18 = 0, Zeilen 1,3..8,19 -> 0x8011aa98 / 0x8011adec | bestaetigt |
| jalr 0 = Haenger | RAM 0x80000000 in 3 sauberen Spiel-Savestates (mzd_stage1_npc, room1090_orig, orig_1170_gp): `00000003 sra zero,zero,0` / `275a0c80 addiu k0,k0,3200` / `00000008 jr zero` / `00000000 nop` = Endlosschleife 0x0..0xc (KUSEG 0 = RAM-Spiegel). Nur im Titel-Savestate steht an 0x8 `03400008 jr k0` | bestaetigt |

**Hinweis zur Tabellen-Konstante:** Bei +0x6 = 1 liest der Sprung **Spalte 1**, also HURT @0x8011f068 und DEATH @**0x8011f308**
(STAGE5 @0x8012002c / @0x801202cc). Die KONSTANTEN-Zeile nennt die Zeilenanfaenge @0x8011f064/@0x8011f304 bzw. @0x80120028/@0x801202c8:
als Zeilenadresse richtig, das gelesene Wort liegt aber 4 Byte dahinter. Alle 8 Spalten der Zeile 9 sind 0, das Ergebnis ist gleich.

## 9. Ivy 0x2d / FX-Emitter 0x24 / Writher 0x1a / Feuer 0x26

**Ivy 0x2d (STAGE4) — BESTAETIGT.** Wurzel 0x801168c4 = Pause-Gate (`801168dc and v0,v0,v1` mit 0x20000000) + `801168f8 addiu at,at,-23872`
(0x8011a2c0) + `80116908 jalr v0`; **kein** `jal 0x8002b498`. `table 0x8011a2c0`: [0] 0x80116920, [1] 0x801169b8, [2] 0xfa060000,
[3] 0x01c20000, [4] 0x01c205fa (= Kasten-Daten), [5] 0x8011a2c8, [6] 0x80116d20, [7] 0x80116ec8. INIT: `8011693c ori v0,zero,0x3` /
`80116944 sb v0,147(a1)`; `80116948 ori v0,zero,0x64` / `80116954 sh v0,154(a1)`. Dump des ganzen Ivy-Bereichs 0x801168c4..0x80116d1c
(`build/r34g_gegner_sonst_gegen/ivy_s4.txt`): **einziger** +0x93-Schreiber ist 0x80116944, kein `sw ...,120(` (nur ein Lesen
`80116ccc lw v0,120(a0)`). Gate B greift also dauerhaft. Wichtig fuer den Bau: fiele Gate B weg und die Ivy bekaeme +0x4 = 2/3, sprang
das Original in Daten (0xfa060000) — der Port darf die Ivy nie in state 2/3 schicken (er schuetzt heute ueber `hit_react` Bit0, s. §12).

**FX-Emitter 0x24 (STAGE2) — BESTAETIGT.** Wurzel `8010eef4 addiu at,at,-29360` (0x80118d50) / `8010ef04 jalr`; [2] 0x8010f130,
[3] 0x8010f3fc. INIT `8010ef28 ori v1,zero,0x1` / `8010ef58 sb v1,147(v0)`. Dump 0x8010ee9c..0x80110a00 (`fx24_s2.txt`): einziger
+0x93-Schreiber 0x8010ef58, kein `,120(`- und kein `,154(`-Schreiber. Ablauf beim 1. Explosionstreffer (eigene Lesung §1): Gate B nicht
(1&3 != 3) -> `&= 1` -> ggf. `|= 0x80` -> Bit0 gesetzt -> `|= 2` -> kein Schaden, kein +0x4. Ab dem 2. Treffer Gate B. **bestaetigt.**

**Writher 0x1a (STAGE1) — BESTAETIGT, eine Ungenauigkeit.**
`8010c2ac addiu at,at,2364` (0x8012093c): [2] 0x8010d0f8, [3] 0x8010d474, [4] 0x8010d768, [5]/[6] = 0, [7] 0x8010d770.
Kasten `8010c3bc lw v0,2356(v0)` (*(0x80120934) = 0x8012091c) / `8010c3c4 sw v0,120(v1)`, {0,-1440,0,300,1440,300}.
HURT `8010d108 addiu a0,a0,2464` (0x801209a0) + `8010d110/14 lbu +0x5/+0x6` / `8010d118 sll v1,v1,5` / `8010d120 sll v0,v0,2` / `8010d130 jalr`;
DEATH `8010d484 addiu a0,a0,3220` (0x80120c94) ... `8010d4ac jalr` — **ohne jede Vorbedingung** (kein +0x7-/Flag-Test vor dem Sprung).
`tabc.py --rowbytes 32 --cols 8`: HURT Zeilen 1,3..8,18,19 Sp. 0/1 -> 0x8010d188, Zeilen 0,2,9..17,20 = 0; DEATH Zeilen 1,3..6 Sp. 1 -> 0x8010d4c4,
7,8,18,19 Sp. 1 -> 0x8010d5d0, sonst 0. Zeile 9..11 = 0 @0x80120ac0..0x80120b1c / @0x80120db4..0x80120e10 **bestaetigt**.
*Ungenau:* "HP aus dem Spawn" — Sce_em_set (0x800420a0..0x80042674, eigene Dumps `sce_em_set*.txt`) schreibt **kein** +0x9a
(einzige Feldschreiber dort u.a. `800421e8 sb zero,147`, `800422d0 sw v0,120`, `800422dc sw s1,124`). Das Writher-HP ist also
der Slot-Altinhalt, nicht ein Spawn-Wert. Fuer die Granate folgenlos (HURT- und DEATH-Zeile 9 sind beide NULL -> Haenger in jedem Fall).

**Feuer 0x26 (STAGE1, ROOM1090) — BESTAETIGT.** `table 0x80121268`: [2] = [3] = [4] = 0x8011697c; `8011698c lbu v0,5(v0)` /
`8011699c addiu at,at,4752` (0x80121290) / `801169ac jalr`; `tabc`: [0..20] -> 0x80116a04, [21] -> 0x801169c4. 0x80116a04 Ph.0:
`80116a2c beq v1,zero,0x80116a50` + Delay `80116a30 ori v0,zero,0x3` / `80116a50 sb v0,147(a0)` (+0x93 = 3). Live (room1090_orig):
7 x Typ 0x26, HP 100, +0x93 = 0, Kasten 0x80121258 {0,0,0,600,720,600}.

**0x22 Stub / Raum-Zensus 0x24:** nicht selbst nachgezaehlt (kein Bau-Wert; 0x22 ist im Port nicht geroutet). -> unklar/irrelevant.

## 11a. Aufrufer-Zensus und Einmaligkeit (RE1.5) — BESTAETIGT

Eigener `jal_find.py` (PSX.EXE + STAGE1..6 + DEBUG + TITLE) plus Datenwort-Suche (Funktionszeiger):
* `jal 0x80012d60`: genau 0x80018008 (a2 = 0 @0x80018004) und 0x800185b8 (a2 = 2). Kein Datenwort 0x80012d60 in irgendeiner Binaerdatei
  -> **Typ 3/4 (0x8006f41e/20, 0x8006f433/34) haben keinen Aufrufer** — bestaetigt.
* `jal 0x8002b5d0`: 0x80012dd0, 0x80012e10 (FUN_80012d60), 0x8002b840, 0x8002b864 (FUN_8002b7e8) — bestaetigt.
* Routine 29 (0x80018320..0x80018434 `jr ra`): nur `80018358 jal 0x80045024`, `80018424 jal 0x80045024`; Routine 30 (0x8001843c..0x80018544):
  nur `800184d8 jal 0x8001af20` -> **kein Flugkontakt** — bestaetigt.
* **Ergaenzung — ein Schadensereignis je Explosion:** Routine 31 ruft den Resolver nur, wenn der Slot-Zaehler +0x1e genau 7 ist:
  `80018560 lhu v1,30(a1)` / `80018568 beq v1,zero,0x80018688` / Delay `8001856c ori v0,zero,0x7` / `80018570 bne v1,v0,0x800185f4`.
  Fuer RE2-Werte "je Ereignis" (G5 80, G1 60 ...) heisst das: **genau ein Ereignis pro Granate** (sofern +0x1e den Wert 7 einmal
  durchlaeuft; das zaehlt die Schwester-Spur re_wurf_flug_explosion.md).
* `jal 0x80011f50`: 11 Stellen, davon 0x80012418 = Selbstaufruf in FUN_80011f50 (Weitergabe bei +0x93 Bit0) und DEBUG.BIN 0x801047a4.
  "alle Schusswaffen-Handler" ist also ungenau (nicht baurelevant).

## 11b. RE2-Applier FUN_800470C0 und Zeilen/Klammern — BESTAETIGT, mit wichtiger Ergaenzung

Gates selbst gelesen (info/re2leon/PSX.EXE): `8004712c andi v0,v0,0x1` / `80047130 beq` (aktiv); `80047138 lbu v0,467(s0)` / `80047140 bne`
(+0x1D3 != 0 -> ueberspringen); `80047148 lh v0,342(s0)` / `80047150 bltz` (HP < 0); `80047158 lhu v0,270(s0)` / `80047160 andi v0,v0,0xc000` /
`80047164 bne` — bestaetigt. Schadensweg: `80047114 srl s6,s5,28` (Klammer K); Record = *(0x800a6a88 + Typ*4) + (Zeile*20 - 20)
(`8004722c lw a1,27272(at)`, `80047230..40`), Schaden = (Wort0 >> (K*10)) & 0x3ff (`80047244..5c`: `sll v0,s6,2 / addu / sll v0,v0,1 /
srlv / andi v1,v1,0x3ff`), `80047268 sh v0,342(s1)`; +0x4-Wort = 2 bzw. 3 bei HP < 0 (`80047288`/`80047290`); `80047324 sb s5,5(s1)`
(+0x5 = Hitcode & 0xff); +0x1D2 = Zone + 3*K (`80047310..30`).
Hitcodes selbst gelesen: Op 47 `80020d54 lui a3,0x1002` / `80020d58 ori a3,a3,0x9` / `80020d78 jal 0x800470c0` (0x10020009: Zeile 9, K1);
Op 48 `80021058 lui a3,0x2` / Delay `80021064 ori a3,a3,0xa` zu `80021060 jal 0x800470c0` (0x0002000A: Zeile 10, K0; zweiter Aufruf
`800214f8`); Op 49 `800216e4 lui a3,0x1002` / Delay `800216f0 ori a3,a3,0xb` zu `800216ec jal 0x800470c0` (0x1002000B: Zeile 11, K1);
Op 40 `80020794 lui a3,0x2002` / `800207a0 ori a3,a3,0xa` (0x2002000A: Zeile 10, K2). Zeile 10 = Brand folgt auch aus Op 40
(liegenbleibende Bodenflamme) — **Zeilen-Zuordnung 9/10/11 = Explosiv/Brand/Saeure bestaetigt**.

**Ergaenzung A (fehlt im Dossier, baurelevant fuer jede RE2-Uebernahme):** Das Record-Wort 1 setzt eine **Treffersperre**:
`80047338 lw v0,4(a1)` / `80047340 srl v0,v0,9` / `80047344 andi v0,v0,0x7f` / `80047348 or a0,a0,v0` / `8004734c sb a0,467(s1)`
-> +0x1D3 = (+0x1D3 & 0x80) | ((rec[1] >> 9) & 0x7f). Solange +0x1D3 != 0, ueberspringt der Applier den Gegner (Gate @0x80047138).
Wer RE2-Schaden "je Ereignis" uebernimmt, braucht diese Sperre (und ihren Abbau im Gegner-Main) mit — das Dossier nennt nur Wort 0.
**Ergaenzung B:** Bit 16 des Hitcodes (0x10000) entscheidet Einzel- vs. Mehrfachziel: `80047208 lui v0,0x1` / `8004720c and v0,s5,v0` /
`80047210 beq v0,zero,0x80047434` -> ohne Bit 16 wird nur der ERSTE Gegner mit Box-Treffer (`800471fc addu s1,s0,zero`) behandelt.
Alle vier GL-Hitcodes haben Bit 16 = 0: **die RE2-Explosion trifft nur EINEN Gegner** (Schwester-Dossier bestaetigt "erster ungesperrter
Gegner"). Fuer die RE1.5-Granate bleibt der Traeger FUN_80012d60 (alle Gegner im Radius, RE1.5 fertig); die RE2-Werte gelten nur als
Reaktion/Schaden je getroffenem RE2-Typ. Diese Trennung sollte im Bau-Dossier ausdruecklich stehen.
(Einzelziel-Zweig ab 0x80047434 gegengelesen: dort ebenso `80047574 sb s5,5(s1)`, +0x1D2 = Zone + 3K `80047564..80`, Sperre
`80047588 lw v0,4(a1)` / `80047590 srl v0,v0,9` / `80047594 andi v0,v0,0x7f` / `8004759c sb a0,467(s1)`.)

## 11c. RE2-Schadensrecords — BESTAETIGT (eigener Leser ueber die Typ-Zeigertabelle)

Eigenes `re2_gl_rec.py`: Tabelle je Typ = *(0x800a6a88 + Typ*4) (Leser `8004722c lw a1,27272(at)`), Record = Tabelle + Zeile*20 - 20.
Ausgabe `build/r34g_gegner_sonst_gegen/re2_gl_rec.txt`:

| Typ | Tabelle | Zeile 9 (Adresse, Bytes, K0/K1/K2) | Zeile 10 | Zeile 11 | Sperre (w1>>9)&0x7f |
|---|---|---|---|---|---|
| G1 em30 | 0x800a5770 | @0x800a5810 `3c f0 c0 03` 60/60/60 | @0x800a5824 `46 18 51 00` 70/70/5 | @0x800a5838 `65 94 a1 00` 101/101/10 | 15 (w1 = 0x078f1e0a) |
| G5 em36 | 0x800a5edc | @0x800a5f7c `50 40 01 05` 80/80/80 | @0x800a5f90 `46 18 51 00` 70/70/5 | @0x800a5fa4 `46 18 a1 00` 70/70/10 | 15 |
| Alligator em23 | 0x800a4a14 | @0x800a4ab4 `1e 78 e0 01` 30/30/30 | @0x800a4ac8 `1f 7c 50 00` 31/31/5 | @0x800a4adc `19 64 90 01` 25/25/25 | 15 |
| Ivy em2e | 0x800a55f4 | @0x800a5694 30/30/30 | @0x800a56a8 70/30/5 | @0x800a56bc 40/40/10 | 15 |
| Kakerlake em29 | 0x800a5004 | @0x800a50a4 60/60/15 | @0x800a50b8 60/60/10 | @0x800a50cc 60/60/10 | 15 |
| Zellenarm em2d | 0x800a5180 | @0x800a5220 60/60/20 | @0x800a5234 60/60/10 | @0x800a5248 60/60/10 | 15 |

Alle Werte und Adressen des Dossiers **bestaetigt**. Neu: die Sperre ist in allen GL-Zeilen 15 Bilder; ihr Abbau steht im
Gegner-Main (G1 `80100260 lbu v1,467(s1)` / `80100268 andi v0,v1,0x7f` / `8010026c beq` / `80100274 sb v0(=v1-1),467`; G5 identisch
`801000ec..80100100`) — also **-1 je Bild**.

## 11d. RE2 G1 em30 — BESTAETIGT, Zerfall fehlt im Dossier

Selbst gelesen (`build/r34g_gegner_sonst_gegen/em30_full.txt`, Overlay `CDEMD0_EM30_ai1.BIN`, gegen 0x80100000 gelinkt laut TOC in
`re2_ems_cut.py`): Dispatch `8010033c lw v0,29956(at)` (0x80107504) / `80100344 jalr`; `table 0x80107504`: [0] 0x80100434, [1] 0x80100a90,
[2] 0x801047f4, [3] 0x80105b40, [4] 0x801062f8, [7] 0x8010633c. Ctor `801004c8 addiu v0,zero,500` / `801004cc sh v0,342(s2)`; Flagwort
`801004b0 addiu v1,v1,-1164` (0x800cfb74), `801004d8 andi v0,v0,0x20` -> `801004e4 addiu v0,zero,400` / `801004e8 sh`.
Zeilen-Effekte nur bei +0x1D2 < 3: `801049c4 lbu v1,466(s2)` / `801049d0 sltiu v1,v1,0x3` / `801049d4 beq`; Zeile 9 `801049ec jal 0x80104f78`,
17 `80104a04 jal 0x8010509c`, 10 `80104a1c jal 0x80104d30`, 11 `80104a34 jal 0x80104e04`. Taumel-Tabelle `bytes 0x80107678 32` =
`60 3e 10 80 f0 40 10 80 01 04 04 04 0a 14 06 07 0a 0a 14 06 04 14 01 00 14 02` -> T[9] @0x80107688 = 0x0a, T[10] = 0x0a, T[11] = 0x14.
Zeile 9: `80104b1c lui v0,0x8010` / `80104b20 lbu v0,30344(v0)` (0x80107688) / `80104b2c srl v0,v0,1` / `80104b3c sb a0,537`; bei +0x1D2 < 3
zusaetzlich `80104b4c lbu v0,30335(at)` / `80104b54 srl v0,v0,1`; andere Zeilen `80104b6c lbu v1,30335(at)` / `80104b74 addu` / `80104b78 sb`.
Schwelle `80104ba4 sltiu v0,v0,0xc`; darunter `80104bb4 lw v0,508(a0)` / `80104bbc jal 0x80100a90` / Delay `80104bc0 sw v0,4(a0)`;
ab 12 `80104bd0 sb zero,537` / `80104bd4 sb zero,536` / `80104bec jal 0x80104c20`. TAUMELN: `80104c68 jal 0x80015910` / `80104c70 lui v1,0x3` /
`80104c74 ori v1,v1,0xa` / `80104c78 subu` / `80104c80 sw v1,332(s0)`; Zeile 11: `80104c98 addiu v0,zero,11` / `80104c9c bne` / `80104ca4 addu a0,zero,zero` /
`80104ca8 jal 0x8005bd6c`; Ende `80104cd0 jal 0x8002959c` -> `80104ce4 addiu v0,zero,2049` / `80104ce8 sw v0,4(s0)`, `80104cf4 andi 0x7f` (+0x1D3),
`80104cf8 andi 0xff7f` (+0x21C), `80104d00 jal 0x8005bd6c` (a0 = 7). TOD: `80105e78 beq` + Delay `80105e7c sh zero,342(s0)`, `80105ec8..e8` Clip
0x3000A - Seite, `80105fe0 lui v0,0x7` / `80105fe4 ori v0,v0,0x11` / `80105fe8 sw v0,332(s0)`. HP-Schreiber em30 (Vollzensus): 0x801004cc,
0x801004e8, 0x80102b74, 0x80102b90, 0x80105e7c — **bestaetigt**, kein DoT im Overlay.

**Fehlt im Dossier (baurelevant, falls das G1-Modell uebernommen wird):**
1. **Akku-Zerfall:** `80104b84 andi v0,v1,0x2` / `80104b88 bne` / `80104b90 sh (+0x21C\|2)` / `80104b94 addiu v0,zero,15` / `80104b98 sb v0,536`
   startet beim ersten Akku-Treffer ein Fenster +0x218 = 15. Im Main (`8010028c lbu v1,536` / `80100294 addiu v0,v1,255` / `80100298 bne v1,zero` /
   Delay `8010029c sb v0,536`; `801002a0..b0` +0x219 -= 1; `801002b8 addiu v0,zero,15` / Delay `801002c0 sb v0,536`; bei +0x219 == 0
   `801002cc andi 0xfffd`) sinkt der Akku um **1 je 16 Bilder**. Ohne diesen Zerfall stimmt "ab 12 Taumeln" bei verteilten Treffern nicht.
2. **Umgehung:** `80104b00 andi v0,v1,0x1` / `80104b04 bne v0,zero,0x80104b9c` — bei +0x21C & 1 wird nichts addiert, nur die Schwelle geprueft.
3. **HP < 200 setzt den Akku auf 50:** `80104ab4 lh v0,342` / `80104abc slti v0,v0,200` / Delay `80104ac4 addiu v1,zero,50` / `80104acc sb v1,537(s2)`
   (+0x219 = 50!) / `80104ad0 ori v0,v0,0x4`. Das Dossier nennt das "Phase verwundet"; tatsaechlich erzwingt es beim ersten Treffer unter HP 200
   **sofort** das Taumeln (50 >= 12).

## 11e. RE2 G5 em36 — BESTAETIGT, Zerfall-Periode im Port falsch

Selbst gelesen (`em36_full.txt`): Zeile 9/17 `80102948 lbu v1,5(s3)` / `8010294c addiu v0,zero,9` / `80102950 beq` / `80102960 jal 0x8010221c`;
10 `80102970 bne` / `80102978 jal 0x80101d9c`; 11 `80102988 bne` / `80102990 jal 0x80101ff0`; Flinch-Byte `801029bc lbu v0,22195(at)` (0x801056b3 + Zeile);
`bytes 0x801056b0 32` = `01 0d 00 00 05 05 05 05 0e 14 0e 14 0e 0e 0e 05 05 14 01 01 14 01` -> **Zeile 9/10/11 @0x801056bc..be = 0e 0e 0e = 14**;
`801029c4 sltiu v0,v0,0xb` / Delay `801029cc addiu v0,zero,7` / `801029d0 sb v0,549(s3)`; Akku `80102a28 lbu v1,22195(at)` / `80102a30 addu` /
`80102a34 sb v0,546(s3)`; Fenster `80102a38 andi v0,a0,0x1` / `80102a44 sh` / `80102a48 addiu v0,zero,15` / `80102a4c sb v0,545(s3)`;
Schwelle `80102a58 sltiu v0,v0,0xf` / `80102a5c beq v0,zero,0x80102a80` -> `80102a84/88` Akku und Fenster = 0 -> `80102a9c jal 0x80102ad0` (STAGGER).
Ctor `801003fc addiu v0,zero,600` / `80100400 sh v0,342(s0)`, easy `80100414 addiu v0,zero,400` (Delay) / `80100418 sh`. HP-Schreiber em36:
0x80100400, 0x80100418, 0x80103004, 0x80103060 — **bestaetigt**. Umgehung bei +0x226&2 (`801029dc andi v0,v0,0x2` / `801029e0 beq`).
Zerfall im Main: `80100118 lbu v1,545(s3)` / `80100120 addiu v0,v1,255` / `80100124 bne v1,zero` / Delay `80100128 sb v0,545`; `8010012c..3c` Akku -1;
`80100144 addiu v0,zero,15` / Delay `8010014c sb v0,545` -> **-1 je 16 Bilder**.
**Port-Abweichung (fehlt im Dossier):** `enemy_ai_boss_g5.c:1500` `if (g->flinch_akku > 0 && ++g->flinch_takt >= 15)` = -1 je **15** Bilder und
ohne das Fenster-Armieren beim Treffer. Ausserdem waehlt der Port den Zuschlag nach `re15_player_equipped_weapon()` (`:1534-1535`), nicht nach der
Treffer-Zeile +0x5: fuer 10/11 (Saeure/Brand) ergibt das **5** statt 14, und ein Waffenwechsel waehrend des Granatenflugs aendert den Zuschlag.
Ziel laut Original: Zuschlag = Byte[+0x5] (@0x801056b3 + Zeile).

## 11f. RE2 Alligator em23 / Ivy em2e / Tentakel em37 — BESTAETIGT

* em23: `table 0x8010461c`: [2] 0x80101ff0, [3] 0x80102c84. HP `80100564 lhu v0,17852(v0)` (0x801045bc) / `8010056c addiu v0,v0,25000` bzw.
  `80100574`/`8010057c` (0x801045be), `80100580 sh v0,342(s0)`; `read 0x801045bc 2` = {300,300} -> **HP 25300**. TREFFER `80102024 sb a1,560(s0)` /
  `80102028 jal 0x80101eac` / Delay `8010202c sb v0,5(s0)`; Tabelle `80102064 lw v0,18044(at)` (0x8010467c) [+0x1D2]. FUN_80101eac:
  `80101ec4 slti v0,v0,25000`; +0x226 == 7 (`80101ef4..14`); Gas (`80101f34 lh v1,18464(v1)` == 8); Vorbedingungen `80101f8c lb v0,549` != 0 oder
  `80101f9c lbu v0,558` != 0 -> klein; `80101fac jal 0x80015fe8` / `80101fb4 andi v0,v0,0x3` / `80101fb8 bne` / Delay `80101fbc addiu v0,zero,4` /
  `80101fc4 sb v0,466`; `80101fc8 andi v0,v0,0x3f` / `80101fcc addiu v0,v0,120` / Delay `80101fd4 sb v0,558(s0)` — **bestaetigt**, zeilenunabhaengig.
  (Das Dossier laesst die Vorbedingung +0x225 != 0 aus; fuer die Grossreaktion gilt: +0x225 == 0 UND +0x22E == 0 UND rng&3 == 0.)
* em2e: Routinen `table 0x801058c4` [2] 0x8010329c, [3] 0x80103a20; HURT `801032c0 lw v0,22896(at)` (0x80105970), DEATH `80103a44 lw v0,22980(at)`
  (0x801059c4). `tabc`: HURT 9 -> 0x8010371c, 10 -> 0x80103860, 11 -> 0x801038c4, 14 -> 0x80103910, 16 -> 0x801039a8; DEATH 9 -> 0x80103df8,
  10 (und 16) -> 0x80103f4c, 11 -> 0x80104090, 14 -> 0x80104128 — **bestaetigt**. "sonst 0x80103314 / 0x80103a88" ist vereinfacht (HURT 0 -> 0x8010319c,
  4/15/18 -> 0x80103650, 17/19 -> 0x80103468; DEATH 5..8 -> 0x80103cf0, 17/19 -> 0x801042ac) — fuer die Granate ohne Belang.
* em37: `80100530 addiu v0,zero,-1` / `80100534 sh v0,342(s1)`; `table 0x80105688` [2] 0x80104184, [3] 0x801041e4 — **bestaetigt**.

## 11g. RE2 NPC-Ausschluss — im Kern BESTAETIGT, "nie" ist zu stark

Selbst gelesen: `8005d764 lhu v1,270(s0)` / `8005d76c ori v0,v1,0x1000` / `8005d770 sh` / **`8005d774 andi v0,v0,0x400` / `8005d778 beq v0,zero,0x8005d7ac`**
/ Delay `8005d77c ori v0,v1,0x5000`; nur im Zweig ohne 0x400: `8005d7b0 sh v0,270(s0)`, `8005d7b4 addiu v0,zero,-1` / `8005d7b8 sh v0,342(s0)` /
`8005d7bc sh v0,354(s0)`. Zweite Stelle `80058e70 ori v0,v0,0x5000` / `80058e74 andi v0,v0,0xfbff` / `80058e78 sh` / `80058e8c..94` HP -1.
**Nicht im Dossier:** Die "zweite Stelle" ist ein Skript-Opcode-Handler (Tabellenplatz `table 0x800a768c` = 0x80058e48), und sein Gegenstueck
**0x80058dac** (Tabellenplatz 0x800a7688) macht den Arbeits-NPC (+0x154) VERWUNDBAR: `80058dd4 andi v0,v0,0xafff` / `80058dd8 ori v0,v0,0x400` /
`80058ddc sh v0,270(a1)` (0x4000 und 0x1000 weg), `80058dec lhu a0,19332(a0)` (0x800d4b84) -> +0x156 (HP) = Partner-HP (`_sh a0,0x156` im Delay
@0x80058e10), `80058e04 sh v0(=200),354(a1)`. In diesem Modus besteht der NPC Gate 3 (HP >= 0) und Gate 4 (0xC000 frei) — RE2-NPCs sind also
**skriptgesteuert treffbar**. Fuer den Bau aendert das die Regel nicht: "HP < 0 -> kein Kandidat" IST RE2-Gate 3 und schliesst genau die
unverwundbaren NPCs aus. RE1.5 hat das Umschaltpaar nicht (Byte-Muster `andi ..,0xafff` + `ori ..,0x400` bzw. `ori ..,0x5000` + `andi ..,0xfbff`
nur in der RE2-EXE gefunden), dort bleibt NPC-HP = -1 aus dem INIT.

## 12. Port-Abgleich (Datei:Zeile selbst gelesen)

| Dossier-Aussage | eigene Lesung | Urteil |
|---|---|---|
| Weg A: `game_step_common.c:1775` `[9] = {1,1,1,0}`, `:1809 re15_player_weapon_fire(eq_item)`, `:1779-1780` 10/11 resolve = 0 | Struct `:1740` `{aktiv, resolve, ammo, n_fx, fx[3]}`; `:1775 [9] = {1,1,1,0,...}`; `:1779 [10] = {1,0,1,0,...}`, `:1780 [11]` ebenso; `:1809` Aufruf | bestaetigt |
| Weg B: `re15_resolve_attack` `re15_damage.c:3294`, `re15_enemy_take_damage` `:2959`, Gate B fehlt `:3334` | `:3294` Funktion; `:3334-3339` Kommentar "GATE B ... OMITTED"; `:3343` `hit_react &= 1`, `:3344` `hit_from_front` -> `|= 0x80`, `:3351` take_damage; Tabellen `:41`/`:56` = {10,20,1000,...}/{3,3,9,10,11,...} | bestaetigt (s. §2: auch 0x80 wird neu berechnet und der Treffer gezaehlt) |
| 0x30/0x36 `enemy_ai_common.c:11846` Tod ohne +0x5-Pruefung | `:11846` `case 3` "mutation-revive guard, else the morph-tail down-machine 0x8011a5d8" | bestaetigt |
| G5 `enemy_ai_boss_g5.c:1433` HP 600, `:1506` Tod bei hp <= 0, `:1535` w == 9 -> 20 | `:1433 e->hp = 600`, `:1506 if (e->hp <= 0 && g->routine != 3)`, `:1534-1535` Zuschlag aus `re15_player_equipped_weapon()` | bestaetigt; zusaetzlich Zerfall `:1500` 15 statt 16 Bilder und 10/11 -> 5 statt 14 (§11e) |
| 0x23 `:13173` Tod -> sofort state 7 | `:13173 case 3: e->state = 7; ...` | bestaetigt |
| Gator-Boss `enemy_ai_boss_gator.c:74` GB_HP 3000, `:740/742` | `:74 #define GB_HP 3000` (DESIGN), `:740 gb_absorb_hit`, `:742 if (e->state == 3) { g->phase = GBP_DIE; ...}` | bestaetigt |
| 0x27 `:9415` Tod, Spur `sub_state_1 >= 9` = Explosionsspur, "(B) korrekt" | `:9419 < 7`, `:9439 < 9`, sonst Explosionsspur; Ph.0 `hit_react \|= 2`, Clip 0xa/0xb nach 0x80, `crow_speed = (rand&0x1f)+80`, `ai_timer = 0`, Ph.1 `0x50 - (ai_timer<<2)`, `re15_dog_advance_ofs(e,0x800)` und bei 0x80 `(e,0)` | fuer 9/10/11 bestaetigt. **Randbefund des Dossiers ungenau:** im TOD fuehren die Port-Grenzen 12/14/19/20/**21** faelschlich in die Explosionsspur (Original 0x8011b7b8) und 13 ebenso (Original 0x8011b998); das Dossier nennt dort die HURT-Ziele b018/b1ec |
| 0x29 `:11381` Tod immer Clip 0xe + SE 7, HURT `:11367` ohne Spuren | `:11381 case 3` "collapse clip 0xe (70f) + Se(7)"; `:11367 case 2` Flinch Clip 7/8 | bestaetigt (Explosionsspur fehlt) |
| **0x2b `:13575` "korrekt"** | Port Ph.2 `:13590` `re15_birkin_clip(e, (e->hit_react & 2) ? 0x0b : 0x0a)`; Original `80114f2c lbu v0,147(v1)` / **`80114f34 andi v0,v0,0x80`** / `80114f38 beq` / Delay `80114f3c ori v0,zero,0xb` / `80114f40 sb v0,148`. Port-Ph.0 setzt ausserdem kein `hit_react \|= 2` (Original `80114d04 ori v0,v0,0x2` / `sb 147`) | **widerlegt**: der Port waehlt den Liegeclip nach dem falschen Bit. Von hinten getroffen (+0x93&0x80) zeigt das Original 0xb, der Port 0xa; nach einem Doppeltreffer zeigt der Port 0xb, das Original nicht. Fuer die Granate relevant, weil jede Explosion den Tyrant toetet |
| 0x1a `:12913` Tod fuer jede +0x5 | `:12913 case 3` "clip 3 topple + gore -> CORPSE 7" | bestaetigt |
| 0x24 `:13234` hit_react = 1 | `:13234 e->hit_react = 1; /* +0x93 = 1 @0x8010ef58 */` | bestaetigt |
| NPC `:10550` default: Idle | `:10550 default: /* all other NPC states ... hold the idle pose */` | bestaetigt |
| NPC-Kaesten `re15_damage.c:3488` | `:3488-3490` `case 0x40..0x4d: r = 450; h = 1530` | bestaetigt (0x45/0x4b falsch) |
| Tentakel `enemy_ai_tentakel_g5.c:194-196` hp -1 ohne Kasten | `:195 e->type = 0x37u; e->active = 1; ...` `:196 e->hp = -1` | bestaetigt |
| Nebenbefund 6: `re15_player_weapon_fire` filtert nur `hit_radius_min > 0` | `:1922` Funktion, `:1956 if (e->hit_radius_min <= 0) continue;`, kein Band-/NPC-Filter | bestaetigt |

**Zu Nebenbefund 6 (Bandtest) — Folgerung korrigiert:** Die Spieler-Bandbits kommen aus +0x98: `80011fb0 lui a0,0x1fff` / `80011fb4 ori a0,a0,0xffff` /
`80011fcc and` (Wort0 &= 0x1fffffff), `80011fc8 lhu v0,-13588(v0)` (0x800acaec = Spieler+0x98) / `80011fd0 sll v0,v0,16` / `80011fe0 or` / `80011fe8 sw`.
Die Waffen-FSM setzt +0x98 u.a. auf `80033104 ori v0,v0,0x2000`, **`800332c0 ori v0,v0,0x4000`**, `80033154 ori v0,v0,0x8000` (alle `sh` auf 0x800acaec,
eigener `reg_sw_scan.py 0x800acaec 0x800acaee`: 27 Schreiber). 0x4000 << 16 = 0x40000000 = genau das NPC-Band (Wort0 0x40000001, live). Der Bandtest
@0x800120d0-ec schliesst NPCs also **nicht** grundsaetzlich aus (nur je Zielrichtung). Ob Schusswaffen NPCs im Original treffen, entscheidet danach
der Typ-Test `80012108 lw v0,0(s5)` / `80012110 jalr v0` (Tabelle ab 0x8006e548) — nicht geprueft, nicht Teil der Granate.

**Itemnamen (Grundlage der Saeure/Brand-Umkehr) selbst gelesen:** Namensleser `8002884c addiu at,at,18780` (0x800c495c) / `80028854 lhu v1,0(at)` /
`8002885c addiu v0,v0,18984` (0x800c4a28) / `80028864 addu`; Daten in DEBUG.BIN (Ladeadresse 0x800c0000): Id 0x0A Offset 0x0068 -> @0x800c4a90
`1d 3f 45 40` = "Acid" (Schrift-Kodierung a = 0x3d, A = 0x1d), Id 0x0B Offset 0x0075 -> @0x800c4a9d `25 4a 3f 41 4a 40 45 3d 4e 55` = "Incendiary".
-> RE1.5 +0x5 = 10 = Acid, 11 = Incendiary; RE2 Zeile 10 = Brand, 11 = Saeure. Umkehr **bestaetigt**.

## 13. Fehlende Mechanismen (Vollstaendigkeit fuer den Bau)

1. **RE2-Treffersperre +0x1D3** (Record-Wort 1, `(w1>>9)&0x7f` = 15 fuer alle GL-Zeilen; gesetzt @0x8004734c/@0x8004759c, Gate @0x80047138,
   Abbau -1 je Bild im Gegner-Main, G1 @0x80100260-74, G5 @0x801000ec-100). Wer RE2-Schaden "je Ereignis" (G5 80, G1 60 ...) uebernimmt, braucht sie.
   Das Port-G5-Modul hat kein Gegenstueck (es loescht `hit_react` Bit0 sofort, `enemy_ai_boss_g5.c:1525`).
2. **Akku-Zerfall G1/G5**: Fenster 15 beim ersten Treffer (G1 @0x80104b94-98, G5 @0x80102a48-4c), Akku -1 je **16** Bilder (G1 @0x8010028c-d0,
   G5 @0x80100118-15c). Port-G5 zerfaellt je 15 Bilder ohne Fenster (`enemy_ai_boss_g5.c:1500`).
3. **G1: HP < 200 setzt den Akku auf 50** (@0x80104ab4-d4) = erzwungenes Taumeln; **Umgehung** bei +0x21C&1 (@0x80104b00-04). G5-Umgehung +0x226&2 (@0x801029dc-e0).
4. **G5-Zuschlag nach Treffer-Zeile**: Original Byte[+0x5] @0x801056b3+Zeile (9/10/11 = 14); Port nach ausgeruesteter Waffe (`:1534-1535`): 9 -> 20, 10/11 -> 5.
5. **Tyrant-Tod Phase 2**: Clip 0xb nur bei +0x93&0x80 (@0x80114f34); Port nimmt `hit_react & 2` (`enemy_ai_common.c:13590`) und setzt in Ph.0 kein
   `|= 2` (@0x80114d04). Das Dossier fuehrt 0x2b als "korrekt" — **Luecke fehlt**.
6. **Ein Schadensereignis je Explosion**: Resolver nur bei Slot+0x1e == 7 (@0x80018560-70). Fuer die RE2-Werte "je Ereignis" die Kopplung benennen.
7. **Traeger-Semantik**: RE2-Op 47 trifft nur den ERSTEN Gegner (Hitcode-Bit 16 = 0, @0x80047208-10); RE1.5 FUN_80012d60 trifft alle im Radius.
   Das Dossier sollte festhalten, dass der RE1.5-Traeger bleibt und RE2 nur Reaktion/Schaden je Typ liefert.
8. **Voreinstellungs-Kasten** {0,0,0,1,1,1} liegt in der Dispatch-Tabelle (Typ 0x0d..0x0f); Port nutzt fuer kastenlose Typen Radius/Hoehe 0
   (Unterschied 1 Einheit — vernachlaessigbar, aber nicht byte-true).
9. **Rueckstoss "von hinten" = netto ~0** (zwei `pos_advance`-Aufrufe 0x800 und 0, FUN_800245d8 addiert nur x/z): fuer die noch zu bauende
   Kakerlaken-Explosionsspur genau so uebernehmen (Maggot-Port macht es bereits richtig).
10. **Maggot-Tod-Spurgrenzen im Port** (12/14/19/20/21 und 13 falsch zugeordnet) — nicht granatenspezifisch, aber Reaktions-Korrektheit.

## 14. Fazit

Alle 42 Zeilen der Tabelle "KONSTANTEN FUER DEN BAU" sind gegen die Bytes geprueft; **Werte und Adressen stimmen** (RE1.5 EXE/STAGE1..5,
RE2 EXE, RE2-Overlays em23/em2e/em30/em36/em37). Der Kern — Explosion = FUN_80012d60 Typ 2, 1000 flach, +0x5 = 9, keine Typ-Immunitaet ausser
Riegel/Gate B, Birkin/Writher-NULL-Zeilen = Endlosschleife ab Adresse 0, fertige Spuren fuer 0x27/0x29/0x2b/0x23 — ist **bestaetigt**, teils
zusaetzlich live (Savestate-RAM).
**Widerlegt:** (1) Port-Abgleich 0x2b "korrekt" — der Tyrant-Liegeclip haengt im Port am falschen Bit. (2) "RE2-NPCs nie Kandidat" — es gibt einen
skriptgesteuerten verwundbaren Modus (0x80058dac); die Bau-Regel "HP < 0 -> kein Kandidat" bleibt trotzdem richtig.
**Ungenau (nicht baurelevant):** NPC-Zeile der Waffentabelle (0x8006f3b8 = Zeile 0x37), "Writher-HP aus dem Spawn" (Sce_em_set schreibt kein HP),
"HP <= 1000 stirbt" (richtig: HP < 1000, bei genau 1000 -> +0x4 = 2), Birkin-Wortadresse (Spalte 1 = +4), Maggot-Randbefund (HURT-Ziele statt DEATH-Ziele),
Alligator-Grossreaktion ohne Vorbedingung +0x225, Bandtest schliesst NPCs nicht aus.
**Fehlend (baurelevant):** RE2-Treffersperre +0x1D3, Akku-Zerfall G1/G5 (16 Bilder) samt Fenster und Umgehungen, G1-Zwangstaumeln unter HP 200,
G5-Zuschlag nach Zeile statt Waffe (10/11 -> 14), ein Ereignis je Explosion, Einzelziel-Semantik von Op 47.

Werkzeuge: `analysis/befunde_runde34_granaten/re_gegner_sonst_gegen_werkzeug/` (`ss_box.py`, `reg_sw_scan.py`, `tabc.py`, `jal_find.py`,
`re2_gl_rec.py`, `ss_ram0.py`); Ausgaben `build/r34g_gegner_sonst_gegen/` (`ss_box.txt`, `reg_sw_scan.txt`, `jal_8002b498.txt`, `re2_gl_rec.txt`,
`em23_full.txt`, `em30_full.txt`, `em36_full.txt`, `birkin_s3_init.txt`, `ivy_s4.txt`, `fx24_s2.txt`, `sce_em_set*.txt`).
