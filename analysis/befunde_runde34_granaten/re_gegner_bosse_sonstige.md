# Runde 34 (Granaten) — RE: Gegnerreaktion auf Explosion / Saeure / Brand — Bosse und Sondertypen

Stand: abgeschlossen 2026-09-29 (RE-Phase, keine Aenderung unter re15_port/). Werkzeuge: `re_gegner_sonst_werkzeug/`
(`reg_scan.py`, `init_zensus.py`, `spawn_zensus.py`, `tab2d.py`, `feld_xref.py`, `jal_scan.py`, `jal_args.py`, `re2_hp_minus1.py`,
`re2_gl_boss_records.py`), Laufzeit-Ausgaben: `build/r34g_gegner_sonst/` (untracked). RE2-Overlays: `build/extracted/re2_ems/`
(aus `re15_port/tools/re2_ems_cut.py`), gelesen mit `RE_OVERLAY_DIR=build/extracted/re2_ems re2_disasm.py ... --bin CDEMD0_EMxx_ai1.BIN`.

## Gliederung

0. Grundlage: der Explosions-Resolver FUN_80012d60 (Typ 2) — was er je Gegner schreibt; 0.1 Gate B = +0x93-Riegel
1. Typ -> Port-Tick (Zuordnung im Port selbst ermittelt) und Original-Wurzeln (Registrierungs-Scan)
2. Je Typ: Original-Reaktion (RE1.5) auf den Explosionstreffer
   2.1 Birkin 0x30/0x36 (UNFERTIG) · 2.2 Gorilla/Made 0x27 · 2.3 Alligator 0x23 · 2.4 Kakerlake 0x29 · 2.5 Tyrant 0x2b ·
   2.6 Ivy 0x2d · 2.7 Writher 0x1a (UNFERTIG) · 2.8 FX-Emitter 0x24 / Stub 0x22 · 2.9 NPCs 0x40+ (UNFERTIG)
3. Hitbox (+0x78) je Typ (+ Nachtrag Kasten-Versatz FUN_8002b498 nach §5)
4. RE2-Retail-Gegenstuecke: 4.1 G1 · 4.2 G5 (+ Tentakel 0x37) · 4.3 Alligator · 4.4 Ivy · 4.5 NPCs · 4.6 Kakerlake
5. Port-Abgleich je Typ (Weg A heute: Hitscan-Bruecke Waffe 9, Weg B: re15_enemy_take_damage(e,2))
6. Gesamttabelle Typ | Port-KI | HE | Saeure | Brand | Schaden | Clip(s) | Port heute | Luecke
KONSTANTEN FUER DEN BAU / PORT-ABGLEICH / OFFEN

## 0. Grundlage: was der Explosions-Resolver je Gegner schreibt (selbst disassembliert)

Aufruf in ESP-Routine 31 (Explosion, PSX.EXE), einziger Typ-2-Aufrufer im ganzen Spiel
(`jal_scan.py 80012d60`: genau 2 Treffer, 0x80018008 = Gegner-Biss Typ 0, 0x800185b8 = Granate):

```
80018598: ori a0,zero,0x1f4          ; Radius 500
800185a0..b0: Punkt = (slot+0x28, slot+0x2a - 500, slot+0x2c)
800185b4: ori a2,zero,0x2            ; attack_type 2
800185b8: jal 0x80012d60
```

FUN_80012d60, Gegner-Zweig (@0x80012f10-0x80013030), je getroffenem Gegner (Rueckwaerts-Reihenfolge):

| Schritt | Instruktion(en) | Wirkung |
|---|---|---|
| Kandidat | `80012db4 lw v0,0(s0) / andi v0,v0,0x1` + `jal 0x8002b5d0` @0x80012dd0 | nur aktive Eintraege (+0x0 Bit0) mit Hitbox-Ueberlappung |
| Gate A | `80012f40 lw v0,392(s1)` / `80012f48 addiu v0,v0,64` / `80012f4c beq v0,v1` | Selbstausschluss: (+0x188)+0x40 == attack_workstruct+0x74 |
| Gate B | `80012f54 lw v0,144(s1)` / `80012f58 lui v1,0x300` / `80012f60 beq v0,v1` | ueberspringt (+0x90 & 0x03000000) == 0x03000000 |
| SE nur Typ<2 | `80012f68 sltiu v0,s0,0x2` / `80012f74 jal 0x800453d0 (a0=0xa)` | fuer Typ 2 NICHT |
| Riegel | `80012f7c..88 lbu/andi 0x1/sb 147` | +0x93 &= 1 |
| Seite | `80012f94 jal 0x8001a7a8` / `80012fac ori v0,v0,0x80` | +0x93 \|= 0x80, wenn FUN_8001a7a8 != 0 = Angriffspunkt HINTER der Figur (Schwester-Dossier `re_schaden_resolver.md` §4.2; das Port-Etikett `hit_from_front` ist vertauscht, die Formel gleich) |
| Schon getroffen | `80012fbc andi v0,v1,0x1` / `80012fcc sb v0(=v1\|2),147(s1)` | +0x93 Bit0 gesetzt -> nur +0x93 \|= 2, KEIN Schaden |
| +0x7 / +0x6 | `80012fd4 sb zero,7(s1)` / `80012fd8 sb v0(=1),6(s1)` | +0x7 = 0, +0x6 = 1 |
| +0x5 | `80012fe0 addiu at,at,-3024 (0x8006f430)` / `80012fe8 lbu` / `80012ff0 sb v0,5(s1)` | +0x5 = DAT_8006f430[2] = **0x09** |
| HP | `80012ff4 lhu a0,0(s3)` (s3 = 0x8006f418+2*Typ) / `80012ffc subu` / `80013000 sh v1,154(s1)` | +0x9a -= DAT_8006f418[2] = **1000** |
| Zustand | `8001300c sb v0,147` (\|=1) / `80013018 sb v0(=2),4(s1)` (Delay-Slot) / `80013020 sb v0(=3),4(s1)` bei HP<0 | +0x93 \|= 1; +0x4 = 2, bei HP<0 (signiert, `lh` @0x80013004) +0x4 = 3 |

Tabellen (`re15_disasm.py read`):
`0x8006f418 s16[11] = {10,20,1000,1000,1000,50,100,200,300,1000,0}` (Bytes `0a 00 14 00 e8 03 e8 03 e8 03 32 00 ...`),
`0x8006f430 u8[11] = {3,3,9,10,11,14,15,16,17,18,20}`.

**Folgerung 1 — +0x5 ist eine WAFFEN-ID.** Der Waffen-Resolver FUN_80011f50 schreibt `800124bc sb fp,5(s1)`
(fp = Waffen-Id, a0 des Aufrufs) und liest den Schaden aus `0x8006e0d0 + Typ*0x58 + Waffe*4` (@0x800124b0-ec).
DAT_8006f430 bildet die Resolver-Typen also auf Waffen-Ids ab: Typ 2 -> 9 (Hand Grenade), Typ 3 -> 10 (Acid),
Typ 4 -> 11 (Incendiary), Typ 5 -> 14 (Flammenwerfer) ... Jeder Gegner-HURT/DEATH-Zweig, der auf +0x5 verzweigt,
sieht die Explosion also wie "Waffe 9".

**Folgerung 2 — keine Typ-Immunitaet im Resolver.** Der Explosions-Schaden kommt aus DAT_8006f418 (1000 flach),
NICHT aus der Waffen-Tabelle @0x8006e0d0. Die Null-Zeilen der Waffen-Tabelle (0x26, 0x2d, 0x36, 0x24, 0x2c ...)
schuetzen gegen die Explosion NICHT. Schutz gibt es nur ueber: (a) keine Hitbox-Ueberlappung (+0x78), (b) Gate B
(+0x90 & 0x03000000), (c) den +0x93-Bit0-Riegel, (d) den Typ-Handler selbst (was er mit +0x4=2/3 macht).
Bei 1000 Schaden stirbt jeder Gegner mit HP <= 1000 im ersten Explosionstreffer (+0x4 = 3) — sofern (a)-(d)
nicht greifen.

**Folgerung 3 — der Spieler wird mitgetroffen.** Der Spieler-Block 0x800aca54 wird IMMER getestet
(@0x80012e00-10), Typ 2 -> HP -= 1000 (@0x80012e44-64), +0x4 = 2 / 3 (Tod bei HP < 0). (Nicht Thema dieses Dossiers.)

Waffen-Tabelle @0x8006e0d0 (Referenz, NICHT vom Explosions-Resolver gelesen), Spalten 9/10/11 und 15/16/17
sind je Typ identisch (Explosiv/Saeure/Brand, Handgranate = Werfer-Munition), `read 0x8006e0d0 22 --w 2 --stride 4 --rows 0x38 --rowstride 0x58`:

| Typ | Zeile @ | W9 HE | W10 Saeure | W11 Brand | W15/16/17 |
|---|---|---|---|---|---|
| 0x1a Writher | 0x8006e9c0 | 100 | 200 | 100 | 100/200/100 |
| 0x22 Stub | 0x8006ec80 | 200 | 50 | 50 | 200/50/50 |
| 0x23 Alligator | 0x8006ecd8 | 70 | 30 | 30 | 70/30/30 |
| 0x24 FX-Emitter | 0x8006ed30 | 0 | 0 | 0 | 0 |
| 0x27 Gorilla/Maggot | 0x8006ee38 | 40 | 40 | 70 | 40/40/70 |
| 0x29 Kakerlake | 0x8006eee8 | 50 | 100 | 200 | 50/100/200 |
| 0x2b Tyrant | 0x8006ef98 | 50 | 200 | 100 | 50/200/100 |
| 0x2d Ivy | 0x8006f048 | 0 | 0 | 0 | 0 |
| 0x30 Birkin 1 | 0x8006f150 | 40 | 70 | 40 | 40/70/40 |
| 0x36 Birkin 5 | 0x8006f360 | 0 | 0 | 0 | 0 |
| 0x40..0x4d NPC | ab 0x8006f3b8 | Zeile ausserhalb der belegten 0x10..0x35 | | | |

Diese Spalten sind im Auslieferungsstand tot (kein FUN_80011f50-Aufruf mit Waffe 9/10/11/15-18: die Entlade-Handler
9/10/11 sind Munitions-Stubs, 15-18 NULL; `jal_scan.py 80011f50` = 11 Aufrufer, alle Schusswaffen-Handler).

### 0.1 Gate B ist der Treffer-Riegel +0x93 (Bits 0 und 1)

`lw v0,144(s1)` @0x80012f54 laedt das WORT +0x90..+0x93; `lui v1,0x300` = Maske 0x03000000 = Bits 24/25.
Little-Endian: Bits 24..31 des Worts sind das Byte **+0x93**. Gate B ist also `(+0x93 & 3) == 3` —
"schon getroffen (Bit0) UND Nachtreffer markiert (Bit1)". Dieselbe Probe steht in FUN_80011f50
(`lw v0,144(s0)` / `and v0,v0,s4` (s4 = 0x03000000) / `beq v0,s4` @0x800120f4-0x80012100, laut
Schwester-Dossier `re_schaden_resolver.md` §2.4). Folgen fuer dieses Thema:
* Ein Typ, dessen INIT `+0x93 = 3` setzt, wird vom Explosions-Resolver komplett uebersprungen (kein Schaden).
* Jeder HURT-Zweig, der in Phase 0 `+0x93 |= 2` setzt (Bit0 kommt vom Resolver), ist fuer den Rest der
  Trefferanimation gegen weitere Resolver-Treffer gesperrt, bis sein Ausgang `+0x93` loescht.
* Port: `re15_resolve_attack` laesst Gate B weg (`re15_damage.c` Kommentar "GATE B ... OMITTED"), weil der
  "Schreiber" als unportiert galt. Der Schreiber ist aber gerade der +0x93-Riegel (`e->hit_react`), der im Port
  existiert. Der Unterschied ist klein (der Port faellt danach in `hit_react &= 1` + `|= 2` = gleicher Endwert 3),
  loescht aber bei gesperrten Gegnern die Bits 2..7 von +0x93 (u.a. 0x80 "von hinten"), die das Original stehen laesst.

## 1. Typ -> Port-Tick (aus `re15_enemy_ai_run_all`, `enemy_ai_common.c` ab Z. 14169) und Original-Wurzel

Registrierung im Original: `sw handler, 0x80072bac+Typ*4` im Stage-Overlay-Init, selbst gescannt
(`re_gegner_sonst_werkzeug/reg_scan.py`, Ausgabe `build/r34g_gegner_sonst/reg_scan.txt`):

| Typ | Stage-Wurzel (sw @) | Port-Tick (Default) | Port-Flavor |
|---|---|---|---|
| 0x30 Birkin G1 | STAGE3 0x80116230 (@0x8011cf48), STAGE5 0x80116a44 (@0x8011dd68) | `re15_birkin_root` -> `re15_birkin_ai_tick` | RE1.5-Overlay-Port |
| 0x36 Birkin G5 | STAGE3 0x80116230 (@0x8011cf50); STAGE5 registriert 0x36 NICHT | ROOM5090/5091: `re15_g5_boss_tick` (`enemy_ai_boss_g5.c`), sonst wie 0x30 | 509x = RE2-G5-KI (Port-Umtypung 0x30->0x36 in `scd_vm.c`) |
| 0x37 G5-Tentakel | (nur RE2) | `enemy_ai_tentakel_g5.c` (vom G5-Modul gespawnt) | RE2 |
| 0x23 Alligator | STAGE2 0x8010c448 (@0x80116f64) | ROOM2090/2091: `re15_gator_boss_tick` (`enemy_ai_boss_gator.c`), sonst `re15_alligator_ai_tick` | 209x = Nutzer-Design + RE2-Mechanik |
| 0x27 Gorilla/Made | STAGE1 0x80116db8 (@0x8011e90c) | `re15_maggot_ai_tick` | RE1.5 |
| 0x29 Kakerlake | STAGE3 0x80110b00, STAGE4 0x8010c1a0, STAGE5 0x8010c320 | `re15_cockroach_ai_tick` | RE1.5 |
| 0x2b Tyrant | STAGE4 0x801118d0, STAGE5 0x80111a50 | `re15_tyrant_ai_tick` | RE1.5 |
| 0x2d Ivy | STAGE4 0x801168c4 (@0x80118408) | `re15_ivy_ai_tick` | RE1.5 |
| 0x1a Writher | STAGE1 0x8010c1ec (@0x8011e9ac, v1 aus @0x8011e880-84) | RE2-Flavor: `re15_re2arm_tick` (Zellenarm), sonst `re15_writher_ai_tick` | Schalter `re15_ai_re2_for_type(0x1A)` |
| 0x24 FX-Emitter | STAGE2 0x8010ee9c (@0x80116f74) | `re15_fx_emitter_ai_tick` | RE1.5 |
| 0x22 Stub | STAGE2 0x8010c080 (@0x80116f54) | NICHT geroutet (spawnt inert) | — |
| 0x40/42/45/47/49/4b/4d NPC | STAGE1..6, je Stage eigene Wurzeln (s. reg_scan.txt) | `re15_npc_ai_tick` | RE1.5 |

## 2. Je Typ: Original-Reaktion auf den Explosionstreffer (+0x5 = 9, +0x6 = 1, HP -= 1000)

### 2.1 Birkin 0x30/0x36 — RE1.5: UNFERTIG (Reaktions-Tabelle fuer Waffe 9..18 = NULL)

STAGE3-Wurzel 0x80116230, Zustandstabelle `table 0x8011ee84 8 --bin STAGE3.BIN`:
[0] 0x801166e0 INIT, [1] 0x80116d38 AKTIV, **[2] 0x8011a060 HURT, [3] 0x8011a3f0 DEATH**, [4] 0x8011a7b4, [7] 0x8011a7bc.
INIT: `8011690c ori v0,zero,0x12c / 80116910 sh v0,154(v1)` = **HP 300**; `80116760-6c lw v0,-4484(v0) (=*(0x8011ee7c) = 0x8011ee64) / sw v0,120(v1)`
= Hitbox @0x8011ee64 `00 00 60 fa 00 00 e8 03 a0 05 e8 03` = {0,-1440,0,1000,1440,1000}. Kein +0x93-Schreiber im INIT.

**HURT 0x8011a060** (erste Phase +0x7 == 0, fuer JEDE Waffe):
```
8011a0b8: ori v0,zero,0x9 / 8011a0c4: sb v0,478(a2)       ; +0x1de = 9
8011a0e4: jal 0x8001a6d4 (atan2 zum Spieler) / 8011a0fc: jal 0x8001aac4 (a2 = 0x20, drehen)
8011a104: jal 0x800453d0 (a0 = 0)                          ; Raum-SE 0
8011a120: sltiu v0,v0,0xfa1  (+0x1d0 Abstand)              ; Abstand >= 4001:
8011a130: ori s0,zero,0x2400 / 8011a134: addiu v0,v0,5 / 8011a138: sh v0,154(v1)   ; HP += 5, Blut 0x2400 statt 0x3000
8011a150..1d8: je +0x6 (0/1/2) jal 0x80019700 (Blut 0x3000/0x2400) an Knochen +408 / +1440 / +1612
8011a1e8: lbu a0,476(v0) / 8011a1f0: addiu v1,a0,255 / 8011a1f8: sb v1,476(v0)       ; +0x1dc--
8011a1f4: beq a0,zero,0x8011a214 ; war +0x1dc > 0: 8011a208 lw v0,472 / 8011a210 sw v0,4(v1) -> +0x4 := +0x1d8 (Treffer geschluckt)
8011a228: ori v0,v0,0x12 / 8011a22c: sb v0,440(v1)        ; +0x1b8 |= 0x12
8011a238: addiu a0,a0,-4284 (0x8011ef44)                   ; Tabelle [+0x5][+0x6], Zeile 32 B
8011a248: sll v1,v1,5 / 8011a250: sll v0,v0,2 / 8011a258: lw v0,0(v0) / 8011a260: jalr v0
```
**DEATH 0x8011a3f0** (+0x7 == 0): `8011a444 lbu v0,477(a2) / 8011a44c andi v0,v0,0x8` — ist +0x1dd Bit 3 gesetzt
(nur waehrend der Mutation: `801180b0 ori v0,v0,0x8` nach `80118094-98 HP = 0x96`, geloescht `801188c0 andi 0xf7`),
dann `8011a530 ori v0,zero,0x32 / 8011a534 sh v0,154(v1)` HP = 50, `8011a54c andi 0xfe` (+0x93 Bit0 weg),
`8011a560-68` +0x4 := +0x1d8 (kein Tod). Sonst Tabelle `8011a594 addiu a0,a0,-3612 (0x8011f1e4)` [+0x5][+0x6], `8011a5b8 jalr v0`.

Tabellen (`tab2d.py STAGE3.BIN 8011ef44 21 8` / `8011f1e4 22 8`):

| +0x5 | HURT 0x8011ef44 Sp. 0/1/2 | DEATH 0x8011f1e4 Sp. 0/1/2 |
|---|---|---|
| 1, 3..8, 19 | 0x8011a284 | 0x8011a5d8 |
| 0, 2 | NULL | NULL |
| **9, 10, 11** (Granaten) | **NULL** (@0x8011f064/84/a4) | **NULL** (@0x8011f304/24/44) |
| 12 Ingram, 13 SPAS, 14 Flamme, 15..18 GL, 20 | NULL | NULL |

STAGE5-Kopie (0x80116a44, Zustandstabelle `table 0x8011fe48`: [2] 0x8011a874, [3] 0x8011ac04; Tabellen
`8011aa50 addiu a0,a0,-248` = 0x8011ff08 und `8011ada8 addiu a0,a0,424` = 0x801201a8) ist identisch belegt:
Zeilen 1,3..8,19 -> 0x8011aa98 / 0x8011adec, Zeilen 0,2,9..18,20 = NULL (`tab2d.py STAGE5.BIN 8011ff08 21 3 --rowbytes 32`).

**Folge im Original:** Explosion trifft Birkin (Hitbox 1000/1440, keine Riegel im INIT) -> HP 300-1000 < 0 ->
+0x4 = 3 -> DEATH-Handler -> (ausser waehrend der Mutation) `jalr` auf das NULL-Wort @0x8011f308 = Sprung nach 0 =
**Haenger** (RAM ab 0 = `00000003 / 275a0c80 / 00000008 / 00000000` -> Endlosschleife 0x0..0xc, Standbild; Beleg
`re_absturz_original.md` §2.1b der Schwester-Spur). Waehrend der Mutation (+0x1dd&8): HP = 50, weiter wie vorher, kein Haenger; der Folge-Treffer
(50-1000) laeuft wieder in den Tod -> NULL. Im HURT-Pfad ebenso NULL (nur erreichbar, wenn HP >= 1000 waere).
**Einordnung: RE1.5-Birkin ist fuer Granaten (und Ingram/SPAS/Flamme/GL) NACHWEISLICH UNFERTIG** -> RE2 G1 (em30)
bzw. G5 (em36) ist das Ziel (Abschnitt 4).

### 2.2 Gorilla/Made 0x27 — RE1.5: FERTIG (eigene Explosions-Spur)

STAGE1-Wurzel 0x80116db8, Zustandstabelle `table 0x801213c8`: [2] HURT 0x8011af5c, [3] DEATH 0x8011b6fc.
HP 180 (`801170d8 addiu a0,a0,-4044` = Tabelle 0x8011f034 + Typ*32 + (rng&15)*2 -> Zeile @0x8011f514 = 16 x 180, `read 0x8011f514 16 --w 2`). Hitbox je +0x1e2 (§3; Standard [4] @0x80121350 {0,-1440,0,1600,1440,1600}). Explosion 1000 > 180
-> immer **DEATH**. Beide Handler verzweigen 1-dimensional auf +0x5:
`8011afe0 lbu v0,5(v0) / 8011aff0 addiu at,at,5288 (0x801214a8) / 8011b000 jalr v0` (HURT),
`8011b780 lbu v0,5(v0) / 8011b790 addiu at,at,5376 (0x80121500) / 8011b7a0 jalr v0` (DEATH).

| +0x5 | HURT @0x801214a8 | DEATH @0x80121500 |
|---|---|---|
| 0..6, 12, 14, 19, 20 | 0x8011b018 (Zucken, Clip 7) | 0x8011b7b8 (Boden-Tod, Clip 0xe) |
| 7, 8, 13 | 0x8011b1ec (Clip 8/9) | 0x8011b998 (Clip 0xa/0xb) |
| **9, 10, 11, 15..18** | **0x8011b400** | **0x8011bb9c** |
| 21 | 0x8011b1ec | 0x8011b7b8 |

**Explosions-Tod 0x8011bb9c** (Phasen auf +0x7, `8011bbac lbu v1,7(a1)`):
* Phase 0 @0x8011bbe8: `ori v0,v0,0x2 / sb 147` (+0x93 \|= 2), +0x7 = 1, `8011bc10 ori v0,zero,0xa / sb 148` Clip **10**,
  bei +0x93&0x80 (Treffer von hinten) `8011bc34 ori v0,zero,0xb` Clip **11**; +0x95 = 0; `8011bc54 ori v0,zero,0x7 / sb 143` Crossfade 7;
  `8011bc58 jal rng / andi 0x1f / addiu 80 / sh 140` +0x8c = (rng&31)+80; `8011bc80 sh zero,156` +0x9c = 0;
  Blut `8011bc7c ori a0,zero,0x2000` + `8011bcfc jal 0x80019700` (Knochen +0x40, Vektor @0x80121388 + (+0x6&1)*0x20 =
  bei +0x6 = 1 @0x801213a8 = {200,-800,0,0}); `8011bd04 jal 0x800453d0 (a0 = 0)` SE 0;
  `8011bd24 jal 0x8004ef90 (0x800b1038, +0x1c6)` Todes-Flag. Faellt in Phase 1.
* Phase 1 @0x8011bd2c: `jal 0x8001f314` (+0x7 += Clip-Ende); `8011bd74-84` +0x8c = 0x50 - 4*(+0x9c);
  `8011bd80 jal 0x800245d8 (a0 = 0x800)` Rueckstoss nach hinten; bei +0x93&0x80 zusaetzlich `8011bda8 jal 0x800245d8 (a0 = 0)`.
* Phase 2 @0x8011bdb8: `jal 0x80019700 (0x2000, +0x6a, (+0x188)+580, 0)`; `8011bdd4 ori v0,zero,0x7 / 8011bdd8 sw v0,4(v1)` -> Zustandswort 7 (Leiche).

**Explosions-Treffer 0x8011b400** (nur bei HP >= 1000 erreichbar; Sprungtabelle `table 0x801003ec 5`:
0x8011b440/570/5fc/660/69c): Phase 0 wie oben mit Clip 10/11, SE `8011b568 jal 0x800453d0 (a0 = 3)`;
Phase 1 Rueckstoss; Phase 2 `8011b614 ori v0,zero,0x10` Clip **16** (bei +0x93&0x80 `ori 0x11` = 17) Aufstehen;
Phase 4 `8011b6a8 sb zero,147` / `8011b6b8 sb 1,4` / `8011b6c8 sb 7,5` -> AKTIV Sub 7.

### 2.3 Alligator 0x23 — RE1.5: FERTIG (Tod waffenunabhaengig)

STAGE2-Wurzel 0x8010c448, `8010c4c4 addiu at,at,-29752` = Zustandstabelle 0x80118bc8: [2] HURT 0x8010e570, [3] DEATH 0x8010e9e8.
INIT: `8010c5c0 sb zero,147(v0)` (+0x93 = 0); HP aus `0x8011717c + Typ*32 + (rng&15)*2` (@0x8010c6a0-d4) = Zeile
@0x801175dc = 16 x **300**; `8010c700 lw v0,-29776(v0)` (*(0x80118bb0) = 0x80118b98) / `8010c708 sw v0,120(v1)` Hitbox
@0x80118b98 = {1000,-720,0, **2200**,720,**800**} (Sektor-Box: +6 != +0xa -> Winkelpfad in FUN_8002b5d0).
Explosion 1000 > 300 -> immer DEATH.

| +0x5 | HURT @0x80118c6c (`8010e5b0`) | DEATH @0x80118cc4 (`8010ea08`) |
|---|---|---|
| 0..6, 12, 14, 19, 20 | 0x8010e5d8 | 0x8010ea30 |
| 7..11, 13, 15..18, 21 | 0x8010e748 | 0x8010ea30 |

HURT prueft vorher +0x1e0 (Wasser): `8010e580 lbu v0,480(v1)` != 0 -> `jal 0x8010e91c` statt Tabelle.
**DEATH 0x8010ea30** (alle Waffen gleich): Phase 0 @0x8010ea7c: +0x93 \|= 2, +0x7 = 1, `8010eaa4 ori v0,zero,0xd` Clip **13**,
+0x95 = 0, Crossfade 7, +0x8c = 0, +0x9c = 0, `8010eaf8-eb18` +0x1ba = -(+0x82 * 1800) (Boden-Y),
Blut `jal 0x80019700` (a0 = 0x2000 aus Delay-Slot @0x8010ea5c, Knochen (+0x188)+2644), `8010eb50 jal 0x8004ef90 (0x800b1058, +0x1c6)`.
Phase 1: Anim; bei +0x1e0 == 0 und Bild 20 (`8010eba8 ori v0,zero,0x14`) -> Phase 2; ab Bild 21 `8010ebe8 jal 0x8001c1a4 (+0x8c, 0, -80, +0x1ba)`.
Phase 2 @0x8010ebf8: Blut 0x2000 an (+0x188)+2644, `8010ec14 ori v0,zero,0x7 / 8010ec18 sw v0,4(v1)` -> Leiche (Zustandswort 7).

### 2.4 Kakerlake 0x29 — RE1.5: FERTIG (eigene Explosions-Spur, Muster wie 0x27)

STAGE3-Wurzel 0x80110b00, `80110b58 addiu at,at,-4956` = Zustandstabelle 0x8011eca4: [2] 0x80114790, [3] 0x80114fb4.
INIT: `80110d14 sb zero,147(v0)`; HP `0x8011d1c8 + Typ*32 + (rng&15)*2` (@0x80110ee0-f10) = Zeile @0x8011d6e8 =
{81,109,97,83,99,113,87,101,117,89,91,103,121,93,105,95}; Hitbox je +0x1e4 aus `table 0x8011ec44` (@0x80110f78-88):
{0,-1080,0,1100,1080,1100} / {0,-1530,0,800,1530,800} / {0,-1080,0,800,1080,800}. Explosion -> immer DEATH.

| +0x5 | HURT @0x8011ed84 (`80114824`) | DEATH @0x8011eddc (`80115048`) |
|---|---|---|
| 0..6, 12, 14, 19, 20 | 0x8011484c | 0x80115070 |
| 7, 8, 13, 21 | 0x80114a40 | 0x80115280 (21 -> 0x80115070) |
| **9, 10, 11, 15..18** | **0x80114cb8** | **0x801154b4** |

**Explosions-Tod 0x801154b4**: Phase 0 @0x80115500: +0x93 \|= 2, +0x7 = 1, `80115528 ori v0,zero,0xa` Clip **10** / `8011554c ori v0,zero,0xb`
Clip **11** bei +0x93&0x80, +0x95 = 0, Crossfade 7, +0x8c = (rng&31)+80 (@0x80115570-88), +0x9c = 0, Blut 0x2000 an Knochen +0x40 mit
Vektor @0x8011ec64 + (+0x6&1)*0x20 (+0x6 = 1: {200,-800,0}), `8011561c jal 0x800453d0 (a0 = 7)` SE 7, Todes-Flag
`8011566c jal 0x8004ef90` (Bank 0x800b0fe0+88 bzw. +120 je `slti v0,v0,3` auf *(0x800b0fe0) @0x80115634).
Phase 1 @0x80115674: Anim, +0x8c = 0x50 - 4*(+0x9c), `801156c8 jal 0x800245d8 (a0 = 0x800)` Rueckstoss, bei +0x93&0x80 `801156f0 jal 0x800245d8 (0)`.
Phase 2 @0x80115700: `8011570c jal 0x80019700` an (+0x188)+580, `8011571c ori v0,zero,0x7 / 80115720 sw v0,4(v1)` -> Leiche.
STAGE4 (Tabellen 0x80119fb4 / 0x8011a00c: [9..11,15..18] -> 0x80110358 / 0x80110b54) und STAGE5 (0x8011fb1c / 0x8011fb74:
-> 0x801104d8 / 0x80110cd4) sind relozierte Kopien mit derselben Spur-Belegung.

### 2.5 Tyrant 0x2b — RE1.5: FERTIG (Tod waffenunabhaengig; Treffer-Schlucken fuer HE AUS)

STAGE4-Wurzel 0x801118d0, `8011192c addiu at,at,-24396` = Zustandstabelle 0x8011a0b4: [2] HURT 0x80114770, [3] DEATH 0x80114c68.
INIT: `80111ab8 sb zero,147(v0)`; HP `0x801185a0 + Typ*32 + (rng&15)*2` (@0x80111bd4-c04) = Zeile @0x80118b00 =
{86,89,103,119,91,107,121,93,109,124,117,97,113,126,99,101}; `80111c30 lw v0,-24404(v0)` (*(0x8011a0ac) = 0x8011a094) /
`80111c38 sw v0,120(v1)` Hitbox {0,-1710,0,800,1710,800}. Explosion -> immer DEATH.

HURT 0x80114770 hat vor der Tabelle einen "Schluck"-Test: `80114780 lh v0,476(a0)` (+0x1dc != 0) UND
`8011479c addiu at,at,-24152` Byte-Tabelle @0x8011a1a8[+0x5] != 0 UND +0x6 == 1 UND !(+0x93&0x80) ->
`801147d8 sw v0(=0x601),4(a0)` (AKTIV Sub 6), `801147dc jal 0x800453d0 (a0 = 0xb)`, `801147f0-f8` HP := +0x1dc, `80114808 sb zero,147`.
Byte-Tabelle (`read 0x8011a1a8 24 --w 1`): `[1,1,1,1,1,1,1,0,0,0,0,0,1,0,1,0,0,0,0,1,1,0]` -> **fuer 9/10/11 = 0**: eine
Explosion wird nie geschluckt. HURT-Tabelle @0x8011a1c0: 0..6/12/14/19/20 -> 0x80114850 (Clip 4, bei +0x1dc Clip 7),
7..11/13/15..18/21 -> 0x80114a40. **DEATH-Tabelle @0x8011a218: alle 22 -> 0x80114cb0** (Phasen `table 0x80100344 5`):
Phase 0 @0x80114cf0: +0x93 \|= 2, +0x7 = 1, `80114d24 ori v0,zero,0x8` Clip **8** (`80114d4c ori v0,zero,0x9` Clip **9** bei +0x93&0x80),
+0x95 = +0x96 = +0x8f = 0, Blut 0x2000 an Knochen +0x40 (Vektor @0x8011a168 + (+0x6&1)*0x20), `80114e04 jal 0x800453d0 (a0 = 2)`,
`80114e24 jal 0x8004ef90 (0x800b1058, +0x1c6)`. Phase 1: Anim + Fussanker `jal 0x80115bec`; Bild 24 (`80114ed8 ori v0,zero,0x18`)
-> `80114ee4 jal 0x800453d0 (a0 = 7)`, +0x7 = 2. Phase 2 @0x80114f00: Clip **0xa** (0xb bei +0x93&0x80), Crossfade 7. Phase 3 Anim, Phase 4 @0x80114fa0 Leiche.
STAGE5-Kopie (Wurzel 0x80111a50, Tabelle 0x8011fc1c; Byte-Tabelle 0x8011fd10 identisch, DEATH 0x8011fd80 [9..11] -> 0x80114e30).

### 2.6 Ivy 0x2d — RE1.5: kein Kampfgegner, gegen die Explosion IMMUN (Gate B)

STAGE4-Wurzel 0x801168c4 (nur Pause-Gate + `jalr`), Zustandstabelle `table 0x8011a2c0`: [0] 0x80116920, [1] 0x801169b8,
**[2] = 0xfa060000, [3] = 0x01c20000 = DATEN** (die {0,-1530,0,450,1530,450}-Box des 0x40-NPC) -> es gibt keinen HURT/DEATH-Zustand.
INIT 0x80116920: `8011693c ori v0,zero,0x3 / 80116944 sb v0,147(a1)` (**+0x93 = 3**), `80116948 ori v0,zero,0x64 / 80116954 sh v0,154(a1)` HP 100,
kein `sw ...,120` (keine Hitbox). Mit +0x93 = 3 schlaegt **Gate B** (@0x80012f54-60) zu: der Resolver ueberspringt die Ivy,
bevor er +0x4 schreibt -> keine Reaktion, kein Schaden, kein Sprung in die Daten-Eintraege. Waffenzeile @0x8006f048 = 0.
**Einordnung:** Die ausgelieferte Ivy ist eine geskriptete Kulisse (Zustand 1: wartet auf Flag z5:31 `jal 0x8004efe4` @0x80116a14,
dann Knochen-Schwingen); das Kampfsystem fehlt (Greif-Code 0x801165f4 verwaist, lt. Port-Kommentar). Fuer die Granate ist das
Original-Verhalten eindeutig: **keine Wirkung**. Ein RE2-Ivy-Kampf waere ein neues Gegner-System, nicht Teil der Granate.

### 2.7 Writher 0x1a — RE1.5: UNFERTIG (HURT/DEATH-Zeilen 9..17 = NULL)

STAGE1-Wurzel 0x8010c1ec, `8010c2ac addiu at,at,2364` = Zustandstabelle 0x8012093c: [2] 0x8010d0f8, [3] 0x8010d474, [4] 0x8010d768, [7] 0x8010d770.
INIT: `8010c3bc lw v0,2356(v0)` (*(0x80120934) = 0x8012091c) / `8010c3c4 sw v0,120(v1)` Hitbox {0,-1440,0,300,1440,300};
**kein HP-Schreiber** im ganzen Writher-Bereich 0x8010c1ec..0x8010d770 (`feld_xref.py ... 154` = 0 Treffer) -> HP aus dem Spawn.
HURT `8010d108 addiu a0,a0,2464` (0x801209a0) und DEATH `8010d484 addiu a0,a0,3220` (0x80120c94) sind 2D-Tabellen [+0x5][+0x6] (32 B/Zeile):

| +0x5 | HURT Sp. 0/1 | DEATH Sp. 1 (Sp. 0 und 2..7 = NULL) |
|---|---|---|
| 1, 3..6 | 0x8010d188 | 0x8010d4c4 |
| 7, 8, 18, 19 | 0x8010d188 | 0x8010d5d0 |
| **9, 10, 11** | **NULL** (@0x80120ac0..0x80120b1c) | **NULL** (@0x80120db4..0x80120e10) |
| 0, 2, 12..17, 20 | NULL | NULL |

**Folge im Original:** Explosion (+0x5 = 9, +0x6 = 1) -> HP -= 1000 -> +0x4 = 3 -> `8010d4ac jalr` auf NULL = **Haenger** (Sprung nach 0, s. §2.1)
(auch +0x4 = 2 fuehrte ueber `8010d130 jalr` auf NULL). Der Writher ist fuer Granaten, GL, Ingram, SPAS, Flamme NICHT fertig.
-> Beta->Retail: RE2-Gegenstueck = der RE2-Zellenarm (RE2-Typ 0x2d, em2d), den der Port im RE2-Flavor schon faehrt; dessen Zeilen 9/10/11 behandelt die Schwester-Spur `re_gegner_re2_familie.md` (RE2-Records Zellenarm: 60/60/20, 60/60/10, 60/60/10 @0x800A5220/34/48, `re2_gl_boss_records.txt`).

### 2.8 FX-Emitter 0x24 — immun (eigener Riegel +0x93 = 1); Stub 0x22 — nie gespawnt

**Spawn-Voreinstellung (gilt fuer JEDEN Typ, dessen INIT +0x78 nicht setzt):** Sce_em_set (PSX.EXE) schreibt
`800421e8 sb zero,147(s0)` (+0x93 = 0) und `800422c8 lui v0,0x8007 / 800422cc addiu v0,v0,11232 / 800422d0 sw v0,120(s0)`
-> +0x78 = **0x80072be0** = `read 0x80072be0 6 --w 2` **{0,0,0,1,1,1}** (Radius 1, Hoehe 1); +0x7c = *(0x800ac77c) (@0x800422c4/dc).
Ohne eigenen Kasten ist ein Typ also mit Radius 1 trotzdem vom Explosions-Resolver erreichbar (R = 1 + 500).

**0x24** (STAGE2-Wurzel 0x8010ee9c, Tabelle 0x80118d50; [2]/[3] zeigen auf die +0x5-Teilchen-Routinen 0x8010f130/0x8010f3fc):
INIT `8010ef28 ori v1,zero,0x1` / `8010ef58 sb v1,147(v0)` -> **+0x93 = 1**; kein +0x78 (Voreinstellung r = 1), kein HP-Schreiber;
kein weiterer +0x93-Schreiber im Bereich 0x8010ee9c..0x80110a00 (`feld_xref.py`). Erster Explosions-Treffer: Gate B greift nicht
(1&3 != 3), aber Bit0 -> `80012fcc` +0x93 = 3, **kein Schaden, kein +0x4**; jeder weitere: Gate B. -> **keine Reaktion.**
Raeume: nur ROOM20B0/20B1 (70 Saetze, `spawn_zensus.py`).

**0x22** (STAGE2-Wurzel 0x8010c080): INIT = `8010c0f4 jr ra` (Zustand bleibt 0), alle Blaetter `jr ra`.
`spawn_zensus.py` (alle 240 RDT-Dateien unter `re15_port/shared_assets/PSX/STAGE1..6`, Walker `scd_walk_lib.py`): **0 Sce_em_set-Saetze** -> fuer die Granate irrelevant.

### 2.9 NPCs 0x40/0x42/0x45/0x47/0x49/0x4b/0x4d — treffbar, KEINE Schadensreaktion (unfertig)

`init_zensus.py` (Ausgabe `build/r34g_gegner_sonst/init_zensus.txt`), jede Stage-Wurzel aus `reg_scan.txt`:

| Typ | Kasten (+0x78) | HP | +0x93 | Beleg (STAGE1) | Raeume (Auszug) |
|---|---|---|---|---|---|
| 0x40 | {0,-1530,0,450,1530,450} | -1 | 0 | `8011c724 sw v0,120` (*(0x80121594)) / `8011c748 sh ...,154` / `8011c7f0 sb zero,147` | 10D0, 11A0, 11B0, 1260, 2000, 30xx, 4000, 60xx |
| 0x42 | {0,-1530,0,450,1530,450} | -1 | 0 | `8011ccf4` / `8011cd18` / `8011cdc0` | 1050, 1090, 11B0, 11C0, 1260, 2000, 30xx, 4000, 50B0, 6020 |
| **0x45** | **{0,-1440,0,500,1440,500}** | -1 | 0 | `8011d300` (*(0x80121734) = 0x80121728) / `8011d320 addiu v0,zero,-1 / 8011d324 sh` / `8011d3cc` | **ROOM1150/1151** (Raum der Granate) |
| 0x47 | {0,-1530,0,450,1530,450} | -1 | 0 | `8011d894` / `8011d8b8` / `8011d960` | 1011, 1021, 1170 |
| 0x49 | {0,-1530,0,450,1530,450} | -1 | 0 | `8011dde0` / `8011de04` / `8011deac` | 22 Raeume |
| **0x4b** | **{0,-1440,0,300,1440,300}** | -1 | 0 | `8011e3b8` / `8011e3dc` / `8011e484` | 22 Raeume |
| 0x4d | {0,-1530,0,450,1530,450} | -1 | 0 | STAGE5 `8011d894`, STAGE6 `80101960` | 5090, 6000, 6030 |

Die Kaesten sind in allen Stages je Typ gleich (STAGE2..6-Zeilen in `init_zensus.txt`). **Kein Gate schuetzt die NPCs:** +0x93 = 0,
Wort +0x0 Bit0 aktiv. Explosion im Radius -> HP -1-1000 = -1001 -> **+0x4 = 3**, +0x5 = 9, +0x6 = 1.
NPC-DEATH (z.B. 0x45 `8011d5f4`: `8011d604 lbu v0,6(v0)` / `8011d614 addiu at,at,5992` (0x80121768) / `8011d624 jalr`) verzweigt auf
**+0x6** in die Bewegungs-Phasen-Tabelle der gemeinsamen NPC-Bibliothek: [1] = **0x80050ddc** (`8012176c`). Dieselbe Form bei
0x40 (0x80121630), 0x42 (0x80121700), 0x47 (0x801217d0), 0x4d (STAGE6 0x8010282c); 0x4b-HURT verzweigt auf +0x5 (@0x80121974, [9] = 0x80051148),
0x4b-DEATH auf +0x6 (@0x80121988, [1] = 0x80050ddc).
0x80050ddc (`dis 0x80050ddc`): spielt nur den laufenden Clip weiter (`80050e70 jal 0x8001f314`), setzt am Clip-Ende +0x6 = 2
(`80050ec4-c8`) -> naechstes Bild [2] = 0x80050f00 ... **Kein Reaktions-Clip, kein Tod, kein Zurueck** in Zustand 4 (Skript-Ausfuehrer
0x80050be8) oder 1 (Eskorte). Der NPC faellt dauerhaft aus seinem Skript. Das ist kein gestaltetes Verhalten (im Auslieferungsstand hat der
Spieler nie eine Granate, §1 Nachtrag Runde 30), sondern ein nie abgesichertes System -> **unfertig** -> RE2-Regel (Abschnitt 4).

### 2.10 Nebenbei: 0x26 in ROOM1090 = RE1.5-Feuer-Emitter (nicht die RE2-Spinne) — reagiert wie auf jeden Treffer

STAGE1-Wurzel 0x80116288, Zustandstabelle `table 0x80121268`: [2] = [3] = [4] = 0x8011697c; dort `8011698c lbu v0,5(v0)` / `8011699c addiu at,at,4752` (0x80121290) / `801169ac jalr`. `table 0x80121290 22`: +0x5 0..20 -> **0x80116a04**, 21 -> 0x801169c4.
0x80116a04 Phase 0: `80116a30/50` +0x93 = 3 (v0 = 3 aus dem Delay-Slot @0x80116a30), bei +0x1d0 >= 8 `80116a74 jal 0x80116b70`, sonst bei (grid&0x7f) < 3 Flag `80116ac8 jal 0x8004ef90 (0x800b1028, grid+29)`, +0x1d0 -= 1 (ab 2); Phase 1 `80116b04 jal 0x80116c68`; Phase 2 `80116b2c-30` +0x4-Wort = 0x10001, +0x5 = grid&0x7f, `80116b5c sb zero,147`. INIT (`init_zensus.txt`): +0x93 = 0 @0x801164ec, HP 100 @0x801164fc, Kasten *(0x80121264) = {0,0,0,600,720,600}. Die Waffen-Null-Zeile @0x8006ede0 schuetzt NICHT vor der Explosion; HP wird aber nie gelesen -> 1000 Schaden = dieselbe Flammen-Minderung wie jeder Treffer, kein Tod. Port: `enemy_ai_common.c` `re15_spider_ai_tick` case 2/3/4 (Z. ~8552) fuehrt genau diesen Ablauf fuer jede +0x5 -> fuer 9 deckungsgleich. (Die RE2-Spinnen-Flavor-Variante gehoert der Schwester-Spur.)

## 3. Hitbox (+0x78) je Typ — Zusammenfassung

| Typ | Original-Kasten (INIT) | Port `re15_enemy_apply_hitbox` (`re15_damage.c` ab Z. 3468) | Abweichung |
|---|---|---|---|
| 0x30/0x36 | {0,-1440,0,1000,1440,1000} (STAGE3 @0x8011ee64, STAGE5 @0x8011fe28) | 1000/1440 | — |
| 0x23 | {1000,-720,0,2200,720,800} @0x80118b98 (Sektor, **Versatz x = 1000**) | r_min 2200 / r_max 800 / h 720, **hit_offset_x = 0** | Versatz x 1000 fehlt im Port |
| 0x27 | je +0x1e2 aus `table 0x80121368` (INIT `801171b0-c0`): [0]/[2] {0,-1080,0,1100,1080,1100} @0x80121338, [1]/[3] {0,-1530,0,800,1530,800} @0x80121344, [4]/[6] {0,-1440,0,1600,1440,1600} @0x80121350, [5]/[7] {0,-2160,0,1100,2160,1100} @0x8012135c | 1600/1440 fest | nur Variante 4/6 |
| 0x29 | je +0x1e4: {1100,1080} / {800,1530} / {800,1080} / {800,1530} (`table 0x8011ec44`) | 1100/1080 fest | nur Variante 0 |
| 0x2b | {0,-1710,0,800,1710,800} @0x8011a094 | 800/1710 | — |
| 0x2d | keiner (Spawn-Voreinstellung r = 1), +0x93 = 3 | keiner | — (Gate B/Riegel schuetzt) |
| 0x1a | {0,-1440,0,300,1440,300} @0x8012091c | 300/1440 | — |
| 0x24 | keiner (r = 1), +0x93 = 1 | keiner, hit_react = 1 | — |
| NPC 0x40/42/47/49/4d | {450,1530} | 450/1530 | — |
| NPC 0x45 | **{500,1440}** @0x80121728 | 450/1530 | **abweichend** |
| NPC 0x4b | **{300,1440}** @0x801218c8 (STAGE1) | 450/1530 | **abweichend** |

Ohne Kasten ist ein Typ NICHT automatisch unverwundbar: die Spawn-Voreinstellung @0x80072be0 (r = 1) reicht mit dem
Explosionsradius 500 fuer einen Treffer, sofern kein +0x93-Riegel greift.

## 4. RE2-Retail-Gegenstuecke (fuer die unfertigen RE1.5-Faelle und fuer Saeure/Brand)

Grundlage (von der Schwester-Spur `re_gegner_re2_familie.md` §1 hergeleitet, hier nur benutzt): RE2 liefert GL-Treffer ueber
den Applier FUN_800470C0 mit Zeile = Hitcode & 0xFF: **9 = Explosiv, 10 = Brand, 11 = Saeure** (in RE2 ist 10 BRAND und 11
SAEURE — umgekehrt zur RE1.5-Item-Reihenfolge 0x0A Acid / 0x0B Incendiary!). Klammer (>>28) = Spalte 0/1/2 des Schadenswortes:
Flug-Kontakt K0 (alle Gegner in der Box), Explosion Op 47 K1, Brand-Stoss K0, Saeure-Stoss K1, Brand-Nachbrenner K2.
Schaden je Typ aus `0x800A6A88[Typ] + (Zeile-1)*20` (Ausgabe `build/r34g_gegner/re2_gl_records.txt` der Schwester-Spur).

**Zuordnung RE1.5-Granate -> RE2-Ereignis (wichtig fuer die Spalte):** Die RE1.5-Handgranate hat KEINEN Flugkontakt — Routine 29 (0x80018320..0x8001843c) ruft nur `jal 0x80045024` (SE, @0x80018358/@0x80018424), keinen Trefferresolver (`jal_scan.py 8002b5d0`: 4 Aufrufer, alle in FUN_80012d60 bzw. 0x8002b7e8ff, keiner in Routine 29). Ihr Schaden entsteht allein in der Explosion (Routine 31). In RE2 entspricht das dem Explosions-Aufruf Op 47 (Zeile 9, **Klammer 1**, +0x1D2 = Zone + 3), nicht dem Flugkontakt (Klammer 0). Brand entspricht dem Brand-Stoss Op 48 (Zeile 10, Klammer 0) plus Nachbrenner Op 40 (Zeile 10, Klammer 2), Saeure dem Saeure-Stoss Op 49 (Zeile 11, Klammer 1) (Hitcodes: Schwester-Spur §1.1). Fuer die RE2-Werte unten heisst das: HE -> Spalte K1, Brand -> K0 (+ K2-Nachbrenner), Saeure -> K1.

Eigene Gegenprobe der Applier-Gates (`re2_disasm.py dis 0x800470c0 60`, info/re2leon/PSX.EXE):
```
80047118  addiu s2,s7,15412      ; s7 = 0x800cc1e8 -> s2 = 0x800CFE1C (Liste)
8004712c  andi v0,v0,0x1 / 80047130 beq   -> aktiv
80047138  lbu v0,467(s0) / 80047140 bne   -> +0x1D3 != 0 ueberspringen
80047148  lh  v0,342(s0) / 80047150 bltz  -> HP (+0x156) < 0 ueberspringen
80047158  lhu v0,270(s0) / 80047160 andi v0,v0,0xc000 / 80047164 bne -> +0x10E & 0xC000 ueberspringen
```

### 4.1 RE2 G1 Birkin (em30, `CDEMD0_EM30_ai1.BIN`, RE_OVERLAY_DIR=build/extracted/re2_ems)

Routine-Tabelle `table 0x80107504` (Dispatch `8010033c lw v0,29956(at)` / `80100344 jalr`): [0] 0x80100434 Ctor, [1] 0x80100a90 AKTIV,
**[2] 0x801047f4 TREFFER, [3] 0x80105b40 TOD**, [4] 0x801062f8, [7] 0x8010633c.
Ctor: `801004c8 addiu v0,zero,500 / 801004cc sh v0,342(s2)` **HP 500** (Bit 0x20 von 0x800CFB74 -> 400 @0x801004e4, im Port-Befund nie gesetzt).
GL-Schaden (Records @0x800A5810/24/38): Zeile 9 = 60/60/60, Zeile 10 (Brand) = 70/70/5, Zeile 11 (Saeure) = 101/101/10.

TREFFER 0x801047f4, Phase 0 (`80104820 lbu v0,6(s2)` == 0):
* Blutspritzer: `80104948-6c` a0 = 0x10000 | (rng*8 + 6096) (`lui a0,0x1 / or`) -> `jal 0x8001bf10` (2x), SE 8 (`801049b8 addiu a0,zero,8 / 801049bc jal 0x8005bd6c`), ausser Zeile 1 und 16.
* **Zeilen-Effekte nur bei +0x1D2 < 3** (Klammer 0 = Flug/Stoss, `801049d0 sltiu v1,v1,0x3`): Zeile 9 -> `801049ec jal 0x80104f78`,
  17 -> `80104a04 jal 0x8010509c`, **10 -> `80104a1c jal 0x80104d30`**, **11 -> `80104a34 jal 0x80104e04`** — reine Partikel-Funktionen
  (`jal 0x8001bf10` mit a0 = 0x0504xxxx bzw. 0x040Fxxxx, z.B. 0x80104f98 `lui s1,0x504`, 0x80105010 `lui s1,0x40f`), kein HP-Tick.
  (Vollzensus der HP-Schreiber in em30: nur @0x801004cc/e8 Ctor, @0x80102b74/90, @0x80105e7c Tod — **kein Brand-/Saeure-DoT**.)
* HP < 200 und !(+0x21C&4) -> `80104acc sb v1(=50),537` / `80104ad0 ori 0x4` (Phase "verwundet").
* +0x21C&0x80 (beschaeftigt) -> Routinenwort aus +0x1FC zurueck (`80104af0-fc`), kein Taumeln.
* **Taumel-Akku +0x219**: Zeile 9: `80104b1c-3c` +0x219 += T[9]>>1, bei +0x1D2 < 3 zusaetzlich (`80104b44-58`) T[9]>>1;
  andere Zeilen `80104b60-78` +0x219 += T[Zeile]. T = Bytes `0x8010767f + Zeile` (`bytes 0x80107678 32`):
  Zeile 1..18 = `01 04 04 04 0a 14 06 07 0a 0a 14 06 04 14 01 00 14 02` -> **Zeile 9 = 10, 10 = 10, 11 = 20**.
  `80104ba4 sltiu v0,v0,0xc` -> ab **12** TAUMELN 0x80104c20, sonst weiter wie vorher (+0x1FC).
  => Explosiv-Flug +10, Explosion (K1) +5, Brand +10, Saeure +20 (sofort Taumeln).
* TAUMELN 0x80104c20: Clip-Wort `80104c70-80` = 0x3000A - FUN_80015910(Seite) (Clip 10 bzw. 9, Blend 3), +0x6 = 1, +0x21C \|= 0x80;
  **bei Zeile 11 zusaetzlich SE 0** (`80104c98 addiu v0,zero,11` / `80104ca8 jal 0x8005bd6c` a0 = 0); Clip-Ende (`80104cd0 jal 0x8002959c`)
  -> Wort 0x801 (`80104ce4 addiu v0,zero,2049 / 80104ce8 sw v0,4(s0)`), +0x1D3 &= 0x7F, +0x21C &= ~0x80, SE 7.
TOD 0x80105b40: bei +0x21C&0x10 nur Spray + dieselben Zeilen-Effekte und Rueckkehr (+0x1FC, Wort 0x901 -> 0x80102838);
sonst 0x80105e50: `80105e7c sh zero,342(s0)` HP = 0, Clip 0x3000A-Seite, +0x21C \|= 0x11, Zeilen-Effekte (`80105f30/48/60/78`),
Clip-Ende -> Clip-Wort 0x70011 (Clip 17) (`80105fe0-e8`) ... — **Todesanimation waffenunabhaengig**, nur die Partikel je Zeile.

### 4.2 RE2 G5 (em36, `CDEMD0_EM36_ai1.BIN`) — Vorlage des Port-Moduls `enemy_ai_boss_g5.c`

Routinen lt. `analysis/befunde_runde6_2026-09-12/birkin-g5-ki.md` §1: [2] TREFFER 0x801025BC, [3] TOD. Eigene Gegenprobe:
```
80102948  lbu v1,5(s3) / 8010294c addiu v0,zero,9 / 80102950 beq -> 80102960 jal 0x8010221c   ; Zeile 9 (und 17 @80102954)
80102970  bne v1,10 ... 80102978 jal 0x80101d9c                                               ; Zeile 10 Brand
80102988  bne v1,11 ... 80102990 jal 0x80101ff0                                               ; Zeile 11 Saeure
801029b0  lbu v0,5(s3) / 801029b4 lui at,0x8010 / 801029bc lbu v0,22195(at)                  ; Flinch-Byte @0x801056B3+Zeile
801029c4  sltiu v0,v0,0xb / 801029cc addiu v0,zero,7 / 801029d0 sb v0,549(s3)                 ; >= 11 -> +0x225 = 7 (Schuetteln)
80102a18..34  +0x222 += Flinch-Byte ; 80102a58 sltiu v0,v0,0xf -> ab 15 STAGGER 0x80102AD0
```
`bytes 0x801056b0 32`: `01 0d 00 00 05 05 05 05 0e 14 0e 14 0e 0e 0e 05 05 14 01 01 14 01` -> Byte[Zeile] @0x801056B3+Zeile:
Zeile 0 = 0, 1..4 = 5, 5 = 14, 6 = 20, 7 = 14, 8 = 20, **9 = 14, 10 = 14, 11 = 14** (@0x801056BC..BE), 12/13 = 5, 14 = 20, 15/16 = 1, 17 = 20, 18 = 1.
Die Effekt-Funktionen 0x8010221c / 0x80101d9c / 0x80101ff0 sind Partikel-Spawns (`jal 0x8001bf10`, erster a0 = 0x05042710 @0x80102228-44).
HP-Schreiber in em36 nur Ctor (@0x80100400/18) und Tod (@0x80103004/60) -> kein DoT. GL-Schaden (Records @0x800A5F7C/90/A4):
Zeile 9 = 80/80/80, 10 = 70/70/5, 11 = 70/70/10. Ctor-HP `801003fc addiu v0,zero,600 / 80100400 sh v0,342(s0)` (easy 400 @0x80100414/18).

**Tentakel 0x37 (em37, `CDEMD0_EM37_ai1.BIN`):** Ctor `80100530 addiu v0,zero,-1 / 80100534 sh v0,342(s1)` -> HP -1 -> vom
RE2-Applier (Gate 3 @0x80047148-50) **nie** getroffen, obwohl TREFFER/TOD-Routinen existieren (`table 0x80105688`: [2] 0x80104184,
[3] 0x801041e4, 2D-Tabellen @0x801057e4/@0x80105808 [+0x5][+0x1D2]). Port: `enemy_ai_tentakel_g5.c:194-196` legt g_actors Typ 0x37
mit hp = -1 und ohne Kasten an; der Port-Resolver (ohne HP-Gate) koennte sie im 500er-Radius auf state 3 setzen — das Modul liest
e->state nicht (kein Treffer in `grep state`), die Wirkung waere also nur der Zustandsbyte-Wechsel. RE2-Ziel: nie Kandidat.

### 4.3 RE2 Alligator (em23, `CDEMD0_EM23_ai1.BIN`) — Vorlage der Port-Mechanik in ROOM2090

Routine-Tabelle `table 0x8010461c` (Dispatch `801001f8 lw v0,17948(at)` / `80100200 jalr`): [2] TREFFER 0x80101ff0, [3] TOD 0x80102c84.
HP: `80100560-7c` HP = 25000 + u16 @0x801045bc bzw. @0x801045be, `80100580 sh v0,342(s0)` (in RE2 nicht mit Waffen toetbar;
Tod = Gasflaschen-Ereignis). GL-Schaden (Records @0x800A4AB4/C8/DC): Zeile 9 = 30/30/30, 10 = 31/31/5, 11 = 25/25/25.
TREFFER 0x80101ff0: `80102024 sb a1,560(s0)` (+0x230 = Zeile), +0x5 = 255, `80102028 jal 0x80101eac`, Bild -= 2 (`80102044-48`),
dann Tabelle `8010205c-6c` **0x8010467c[+0x1D2]**. FUN_80101eac ueberschreibt +0x1D2 mit der REAKTIONSART — **unabhaengig von der Zeile**:
`80101ec4 slti v0,v0,25000` -> +0x218 \|= 0x20; bei +0x226 == 7: Zeile 1 -> 0, sonst 1; Gas-Fall (`80101f34 lh v1,18464(v1)` == 8,
HP < 25000) -> 2/3; sonst `80101fac jal rng / andi 0x3 / bne` ein Viertel -> **4 = Grossreaktion** (`80101fbc-c4`),
+0x22E = (rng&0x3F)+120 (`80101fc8-d4`); sonst 0 = kleine Reaktion. Tabelle `table 0x8010467c 9`:
[0] 0x80102090 (klein, SE 5 `801020c0 addiu a0,zero,5 / 801020c4 jal 0x8005bd6c`), [4] 0x80102bd0 (gross, SE 0 `80102c0c addu a0,zero,zero / 80102c10 jal 0x8005bd6c`, +0x1D3 \|= 0x80 `80102c20`), [2]/[3] Gas.
**=> RE2 kennt beim Alligator KEINE GL-eigene Reaktion; Explosiv/Brand/Saeure wirken nur ueber den Schaden (30/31/25 je Ereignis).**

### 4.4 RE2 Ivy (em2e, `CDEMD0_EM2E_ai1.BIN`) — eigene Reaktion und eigener Tod je GL-Zeile

Routine-Tabelle `table 0x801058c4` ([2] 0x8010329c, [3] 0x80103a20). GL-Schaden (Records @0x800A5694/A8/BC): Zeile 9 = 30/30/30,
**Zeile 10 (Brand) = 70/30/5**, Zeile 11 = 40/40/10 (Brand-Flug K0 = 70 = die Brand-Schwaeche liegt im SCHADEN).
HURT `801032c0 lw v0,22896(at)` = Tabelle @0x80105970[+0x5]: 9 -> **0x8010371c**, 10 -> **0x80103860**, 11 -> **0x801038c4**, 14 -> 0x80103910, 16 -> 0x801039a8, sonst 0x80103314.
* 9: Phase 0 Teile-FX `80103798 jal 0x80105178` (a1 = 5 bzw. 11, a2 = 2 = FX-Art 2), dann gemeinsamer Treffer 0x80103314.
* 10: `8010388c-90` FUN_801037cc(e, 2, **6**) = 6x Teile-FX aus Liste @0x801058b4 (`bytes`: `00 01 09 0e 05 0a 14 11 02 15 0a 00 07 0f 03 09`), dann 0x80103314.
* 11: 0x80103314, dann `801038d8 addiu v0,zero,197 / 801038e0 sb v0,549(s0)` (+0x225 = 197, Ein-Bild-Wert, im Main @0x80100028-3c
  sofort wieder 0 = Anforderung an die EXE — dieselbe +0x225-Rolle wie der G5-Ruettler), +0x227 = rng&3, FUN_801037cc(e, 7, 2).
* FUN_80105178(e, Teil, Art): reine FX (Deskriptor @0x80105a38 + Art*6 -> `80105280 jal 0x8001bf10`).
DEATH `80103a44 lw v0,22980(at)` = Tabelle @0x801059c4[+0x5]: 9 -> **0x80103df8** (Explosions-Tod, Teile-FX Art 3), **10 -> 0x80103f4c
(Brand-Tod: 4x FUN_801037cc(e,3,6) @0x80103f8c/9c/80104018/28, Routinenwort-Wechsel @0x80104068, FUN_80104238)**, 11 -> **0x80104090**
(Saeure-Tod: FUN_801037cc(e, 8, 4) @0x801040bc-d0), 14 -> 0x80104128, sonst 0x80103a88. Keine HP-Tick-Schleife (HP-Schreiber em2e: Init
@0x801002a4/b4/e0, @0x80100f4c = Griff-Tod des SPIELERS `sh v0(-1),342(s1)`).

### 4.5 RE2 NPCs — vom Applier grundsaetzlich ausgeschlossen

`re2_hp_minus1.py`: HP := -1 steht in RE2 nur in zwei EXE-Stellen, beide NPC-Init:
```
8005d764  lhu v1,270(s0) / 8005d76c ori v0,v1,0x1000 / 8005d77c ori v0,v1,0x5000   ; +0x10E |= 0x5000 (Bit 0x4000!)
8005d7b0  sh  v0,270(s0)
8005d7b4  addiu v0,zero,-1 / 8005d7b8 sh v0,342(s0) / 8005d7bc sh v0,354(s0)          ; HP (+0x156) = -1, +0x162 = -1
8005d7c0..d8  Typ 65/67/69/79 (0x41 Ada, 0x43, 0x45 Sherry, 0x4F) -> globales Bit in 0x800CFBD8 loeschen
80058e70  ori v0,v0,0x5000 / 80058e74 andi 0xfbff / 80058e78 sh v0,270(a1) / 80058e8c-94 HP = -1   ; zweiter NPC-Init
```
Damit fallen NPCs im RE2-Applier DOPPELT heraus: Gate 3 `80047148 lh v0,342(s0) / 80047150 bltz` (HP < 0) und Gate 4
`80047158-64` (+0x10E & 0xC000, hier 0x4000). **RE2-Ziel fuer die RE1.5-NPCs: von der Explosion nicht getroffen.**
Die RE1.5-NPCs tragen dieselbe Markierung schon (HP = -1 im INIT, Abschnitt 2.9) — es fehlt nur die Abfrage.

### 4.6 RE2 Kakerlake (em29, `CDEMD0_EM29_ai1.BIN`) — nur Effekt-Variante je Zeile

Routine-Tabelle `table 0x80101cd8`: [2] = [3] = 0x801014b0. Dort `801014dc jal 0x80101acc` (a1 = Zeile): Sprungtabelle
`table 0x80100024 19` [Zeile-1]: **9 -> 0x80101b58** (a1 = 5, a2 = 4, a3 = 0x1300), **10 -> 0x80101b58**, **11 -> 0x80101b0c**
(a1 = 4, a2 = 9, a3 = 2048) -> `80101b64 jal 0x80101b7c` (Partikel). Danach +0x1D2 = (+0x21E&4) ? 1 : 0 (`801014ec jal 0x80101488`)
und Spalten-Tabelle `table 0x80101d20` [0] 0x80101548 / [1] 0x80101700 — **gleicher Zustandsablauf fuer alle Zeilen**.
GL-Schaden (Records @0x800A50A4/B8/CC): 60/60/15, 60/60/10, 60/60/10.
(Fuer die RE1.5-Kakerlake bleibt RE1.5 massgeblich — dort ist die Explosions-Spur fertig, §2.4.)

## 5. PORT-ABGLEICH je Typ (Datei:Zeile)

Zwei Wege im Port:
* **(A) heute aktiv** — Waffe 9: `game_step_common.c:1775` `ENT[9] = {1,1,1,0}` -> `:1809 re15_player_weapon_fire(eq_item)` = Hitscan-Bruecke
  (FUN_80011f50-Stellvertreter): naechstes Ziel im Band, Schaden = Waffen-Zeile [Typ][9] (§0 Tabelle bzw. RE2-Zeilen fuer die RE2-Familie),
  +0x5 = 9, beim ABZUG. Waffe 10/11: `:1779-1780` `resolve = 0` -> **keinerlei Schaden, keine Reaktion**.
* **(B) Original-Explosion** (Umbauziel der Wurf-Spur) — `re15_resolve_attack` (`re15_damage.c:3294`, Radius 500, Typ 2) ->
  `re15_enemy_take_damage(e, 2)` (`re15_damage.c:2959`): HP -= 1000, +0x5 = 9, +0x6 = 1, +0x7 = 0, hit_react \|= 1, state = 2 bzw. 3 —
  bytegleich zum Original-Zweig (§0). **Gate B fehlt** (`re15_damage.c:3334` "OMITTED"), obwohl es der vorhandene Riegel
  `hit_react & 3` ist (§0.1).

| Typ | (B) was der Port-Tick danach tut | Original | Luecke |
|---|---|---|---|
| 0x30/0x36 (nicht 509x) | HP 300-1000 < 0 -> `enemy_ai_common.c:11846` DEATH: Mutations-Schutz (+0x1dd&8 -> HP 50) sonst Morph-Tod 0x8011a5d8 — **ohne +0x5-Pruefung** | RE1.5: Zeile 9 der Tabellen = NULL -> jalr 0 (§2.1), unfertig | Ziel RE2 G1 (§4.1): HP 500, Explosion = Zeile 9 K1 = 60, Taumel-Akku +5 (Flugkontakt waere +10, gibt es bei der Handgranate nicht), Schwelle 12, Clip 9/10, Tod waffenunabhaengig; die Port-Tabellenwahl fuer Zeile 9..18 ist zu entscheiden (Kap. OFFEN) |
| 0x36 in 5090/5091 (G5-Modul) | `enemy_ai_boss_g5.c:1433` HP 600; `:1506` hp <= 0 -> TOD: **eine Explosion (1000) toetet G5 sofort**. Heute (A): Zeile 0x30 (Port-Bruecke `re15_damage.c` 0x36-Fall), Flinch-Zuschlag `:1535` w == 9 -> **20** | RE2 G5: Zeile 9 = 80 je Ereignis, Flinch-Byte **14** (@0x801056BC) | Schaden 1000 statt 80; Flinch 20 statt 14 (bei 14 taumelt G5 erst beim 2. Treffer innerhalb des Zerfalls); Zeilen-Partikel 0x8010221c/0x80101d9c/0x80101ff0 fehlen |
| 0x36 ROOM3080 | wie 0x30 (Tod-Handler), Waffenzeile heute 0 | RE1.5 NULL (Haenger) | wie 0x30 |
| 0x23 (nicht 2090) | `enemy_ai_common.c:13173` DEATH -> sofort state 7, kein Clip | Tod 0x8010ea30: Clip 13, Blut Knochen 15, Flag, Fall, Leiche (§2.3) | Todesablauf fehlt (allgemein, nicht granatenspezifisch) |
| 0x23 in 2090 (Boss) | `enemy_ai_boss_gator.c:742` state 3 -> GBP_DIE nur bei HP < 0; bei GB_HP 3000 (`:74`, DESIGN) -> 2000, state 2 -> `gb_absorb_hit` `:740` (Blut, Flinch an 300er-Schwellen) | RE2: HP 25000+, Zeile 9/10/11 = 30/31/25, Reaktion zeilenunabhaengig (§4.3) | 1000 je Explosion = 1/3 der Design-HP — Design-Entscheidung, kein Original-Wert |
| 0x27 | `:9415` DEATH, Spur `sub_state_1 >= 9` -> Explosions-Tod mit Clip 10/11 + Rueckstoss — **deckt 9/10/11** | 0x8011bb9c (§2.2) | fuer 9/10/11 keine; Randbefund: die Port-Spurgrenzen `<7 / <9 / sonst` bilden 12/14/19/20 (Original b018) und 13/21 (b1ec) falsch ab |
| 0x29 | `:11381` DEATH -> immer Clip 0xe + SE 7 | 0x801154b4: Clip 10/11, Rueckstoss, SE 7, Blut, Flag (§2.4) | **Explosions-Spur fehlt** (HURT `:11367` ebenso ohne +0x5-Spuren) |
| 0x2b | `:13575` DEATH einspurig (Clip 8/9 -> 0xa/0xb) | 0x80114cb0 fuer alle +0x5 (§2.5) | keine fuer die Explosion (Tod). HURT `:13555` ohne +0x5-Tabelle/Schluck-Gate (nur bei HP >= 1000 relevant) |
| 0x2d | hit_react = 3 -> `take_damage` Bit0 -> kein Schaden | Gate B (§2.6) | keine (Port erreicht dasselbe ueber den Riegel) |
| 0x1a RE1.5-Flavor | `:12913` DEATH -> Clip 3 + Blut -> Leiche 7 (fuer jede +0x5) | NULL (§2.7) | Ziel RE2 (Zellenarm, Schwester-Spur `re_gegner_re2_familie.md`) |
| 0x1a RE2-Flavor | `re15_re2arm_tick` (enemy_ai_re2_zellenarm.c) | RE2 em2d | Schwester-Spur |
| 0x24 | `:13234` hit_react = 1 -> immun | +0x93 = 1 (§2.8) | keine |
| 0x22 | nicht geroutet, nie gespawnt | — | keine |
| NPC 0x40.. | Kasten 450/1530 -> getroffen, HP -1-1000, state 3 -> `:10550` default: Idle-Pose, **faellt aus Eskorte/Skript** | RE1.5: faellt ebenso aus dem Skript (§2.9) — unfertig; RE2: HP<0/+0x10E-Gate schliesst NPCs aus (§4.5) | Ziel: NPCs (HP < 0) vom Explosions-Resolver ausnehmen; Kaesten 0x45 {500,1440} und 0x4b {300,1440} weichen ab (`re15_damage.c:3488`) |

**Nachtrag Hitbox-Versatz (gilt fuer Explosion UND Waffe):** Die Wurzel-Schwaenze rufen `jal 0x8002b498` (z.B. Maggot @0x80116e30,
Birkin STAGE3 Wurzel). FUN_8002b498 (`dis 0x8002b498`): `8002b4bc lw s0,120(s1)` (Kasten), `8002b4c0 lw s2,124(s1)` (+0x7c-Puffer),
Drehmatrix aus Yaw +0x6a (`8002b4c4 lhu v0,106(s1)` / `8002b4d4 jal 0x80068098`), dreht (Kasten+0, Kasten+4) (`8002b4f4 jal 0x800661c0`)
und schreibt `8002b504 sh +0x7c[0]` = gedrehtes x, `8002b510 sh +0x7c[1]` = Kasten+2, `8002b51c sh +0x7c[2]` = gedrehtes z; dazu
`8002b520 sb zero,450(s1)` (+0x1c2). Der Kastenmittelpunkt ist also Lage + R(Yaw)·(Kasten.x, Kasten.y, Kasten.z). Fuer alle Typen hier
ist das (0, -h, 0) — **ausser dem Alligator: (1000, -720, 0) = 1000 vor dem Maul**. Port: `re15_damage.c:3552 a->hit_offset_x = 0`
(und keine Drehung) -> der Alligator-Kasten sitzt im Port 1000 zu weit hinten.

## 6. Gesamttabelle

HE = Hand Grenade (Original-Explosion, Resolver-Typ 2 = 1000 flach, +0x5 = 9). Saeure/Brand: RE1.5 nur Daten (Art 3/4 = 1000, +0x5 = 10/11,
ohne Aufrufer, Schwester-Dossier `re_saeure_brand.md` §1.5) -> RE2-Ziel; in RE2 ist **Zeile 10 = Brand, 11 = Saeure**.

| Typ | Port-KI | Original-Reaktion HE | Saeure | Brand | Schaden | Clip(s) | Port heute | Luecke |
|---|---|---|---|---|---|---|---|---|
| 0x30 Birkin G1 (3070/5080/50E0...) | RE1.5-Port `re15_birkin_ai_tick` | RE1.5: Tod -> Tabelle [9][1] NULL = jalr 0 (unfertig). RE2 G1 fuer die Explosion (K1): Blut + SE 8, Taumel-Akku **+5**, ab 12 Taumeln; Partikel 0x80104f78 nur bei K0 (Flug), also nicht | RE2 G1 (Op 49, K1): Akku +20 -> sofort Taumeln + SE 0; Partikel 0x80104e04 nur bei K0 | RE2 G1 (Op 48, K0): Partikel 0x80104d30, Akku +10 | RE1.5 1000 (Tod, HP 300); RE2 HE K1 = 60, Saeure K1 = 101, Brand K0 = 70 (+ Nachbrenner K2 = 5) bei HP 500 | RE1.5 keine; RE2 Taumeln Clip 9/10 (Clip-Wort 0x3000A-Seite), Tod Clip 9/10 -> 17 | (A) 40 Schaden (Zeile 0x30 Sp. 9) + Port-Flinch; 10/11 nichts | Reaktion fuer 9..11 fehlt im Original -> RE2-G1-Werte uebernehmen oder Port-Wahl dokumentieren |
| 0x36 Birkin G5 (5090/5091) | RE2-G5-Modul | RE2: Flinch-Byte 14 (Schwelle 15), Partikel 0x8010221c | Flinch 14, Partikel 0x80101ff0 | Flinch 14, Partikel 0x80101d9c | RE2 HE K1 = 80, Brand K0 = 70 (+ K2 5), Saeure K1 = 70 bei HP 600 | Stagger Clip 8 (50 F), SE 12/9 | (A) Zeile 0x30 = 40, Flinch 20; (B) 1000 = Sofort-Tod | Schaden + Flinch-Wert falsch, Partikel fehlen |
| 0x36 ROOM3080 | RE1.5-Port | wie 0x30 (NULL) | — | — | 1000 | — | (A) 0 Schaden | wie 0x30 |
| 0x23 Alligator (nicht 2090) | RE1.5-Port | Tod 0x8010ea30 (fuer alle +0x5) | gleich | gleich | 1000 -> Tod (HP 300) | Tod Clip 13, Blut Knochen 15, Fall, Leiche | (A) 70 Schaden -> HURT-Zucken; (B) Tod ohne Clip | Todesclip/Fall fehlen; Kasten-Versatz 1000 fehlt |
| 0x23 Alligator 2090 | Nutzer-Design + RE2-Bausteine | RE2: keine GL-eigene Reaktion; 1/4 Grossreaktion (SE 0, Sperre 120..183 F), sonst klein (SE 5) | gleich | gleich | RE2 HE K1 30, Brand K0 31 (+ K2 5), Saeure K1 25 bei HP 25000+ | Grossreaktion 0x80102bd0 (Clip lt. Port-Modul 10, nicht selbst gelesen) | (A) 70; (B) 1000 von GB_HP 3000 | Schadensbemessung = Design-Entscheidung |
| 0x27 Gorilla/Made | RE1.5-Port | Tod-Spur 0x8011bb9c (Explosions-Spur) | gleiche Spur (+0x5 = 10) | gleiche Spur (11) | 1000 -> Tod (HP 180) | Clip 10/11 + Rueckstoss, Leiche | (A) 40 -> HURT-Spur b400 (Clip 10/11 -> Aufstehen 16/17); (B) korrekt | keine fuer 9..11 |
| 0x29 Kakerlake | RE1.5-Port | Tod-Spur 0x801154b4 | gleich | gleich | 1000 -> Tod (HP 81..121) | Clip 10/11 + Rueckstoss, SE 7 | (A) 50 -> Flinch; (B) Clip 0xe | Explosions-Spur fehlt im Port |
| 0x2b Tyrant | RE1.5-Port | Tod 0x80114cb0 (fuer alle +0x5); HURT-Schlucken fuer 9..11 aus | gleich | gleich | 1000 -> Tod (HP 86..126) | Clip 8/9 -> 0xa/0xb, SE 2/7 | (A) 50 -> HURT Clip 4/7; (B) korrekt | keine fuer die Explosion |
| 0x2d Ivy (4030/4031) | RE1.5-Kulisse | keine (Gate B, +0x93 = 3) | keine | keine | 0 | — | immun | keine. (RE2-Ivy haette eigene 9/10/11-Tode — nur relevant, falls die Ivy je Kampfgegner wird) |
| 0x1a Writher (1210/1211) | RE1.5 oder RE2-Zellenarm | RE1.5: Tabellen 9..11 NULL (unfertig) | NULL | NULL | 1000 -> Tod | — | RE1.5-Flavor: Clip 3 + Leiche fuer jede +0x5 | RE2-Zellenarm-Reaktion (Schwester-Spur) |
| 0x26 Feuer (1090, RE1.5-Herkunft) | RE1.5 `re15_spider_ai_tick` | Flammen-Minderung 0x80116a04 (wie jeder Treffer), kein Tod | gleich | gleich | 1000 (HP ungelesen) | — | (A) 0 Schaden (Null-Zeile), gleiche Reaktion | keine |
| 0x24 FX-Emitter | RE1.5 | keine (+0x93 = 1) | keine | keine | 0 | — | immun | keine |
| 0x22 Stub | nicht geroutet | nie gespawnt | — | — | — | — | — | keine |
| NPC 0x40..0x4d | RE1.5 NPC-Bibliothek | RE1.5: getroffen, faellt aus dem Skript (unfertig). RE2: vom Applier ausgeschlossen (HP -1, +0x10E&0x4000) | wie HE | wie HE | RE1.5 1000 auf HP -1 | RE1.5 Phasen-Handler 0x80050ddc... | (B) Idle, aus dem Skript | NPCs mit HP < 0 vom Explosions-Resolver ausnehmen |

## KONSTANTEN FUER DEN BAU

RE1.5 = info/Re1.5 (PSX.EXE bzw. STAGEn.BIN, `re15_disasm.py`); RE2 = info/re2leon/PSX.EXE bzw. `build/extracted/re2_ems/CDEMD0_EMxx_ai1.BIN`
(`RE_OVERLAY_DIR=build/extracted/re2_ems re2_disasm.py ... --bin`).

| Name | Wert | Adresse | Instruktion(en)/Bytes | Verwendung |
|---|---|---|---|---|
| Explosionsschaden Typ 2 | 1000 | RE1.5 @0x8006f41c | `e8 03`; Leser `80012ff4 lhu a0,0(s3)` | HP -= 1000 je getroffenem Gegner |
| Reaktions-Waffe Typ 2 | 9 | RE1.5 @0x8006f432 | `09`; `80012fe8 lbu` / `80012ff0 sb v0,5(s1)` | +0x5 = 9 |
| Art 3 / Art 4 (nur Daten) | 1000 / 1000, +0x5 = 10 / 11 | RE1.5 @0x8006f41e/20, @0x8006f433/34 | `e8 03 e8 03`, `0a 0b` | ohne Aufrufer (0x0A Acid / 0x0B Incendiary) |
| Treffer-Unterzustand | +0x6 = 1, +0x7 = 0 | RE1.5 @0x80012fd4/d8 | `sb zero,7(s1)` / `sb v0(=1),6(s1)` | alle Typen |
| Gate B | (+0x93 & 3) == 3 -> ueberspringen | RE1.5 @0x80012f54-60 | `lw v0,144(s1)` / `lui v1,0x300` / `and` / `beq v0,v1` | fehlt im Port (`re15_damage.c:3334`) |
| Spawn-Hitbox-Voreinstellung | {0,0,0,1,1,1} | RE1.5 @0x80072be0 | `800422c8 lui v0,0x8007 / 800422cc addiu v0,v0,11232 / 800422d0 sw v0,120(s0)` | Typen ohne eigenen Kasten (0x2d, 0x24) |
| Maggot HURT-Spur Explosion | 0x8011b400 fuer +0x5 9..11, 15..18 | STAGE1 @0x801214cc..d4, @0x801214e4..f0 | `table 0x801214a8 24` | 0x27 |
| Maggot DEATH-Spur Explosion | 0x8011bb9c fuer 9..11, 15..18 | STAGE1 @0x80121524..2c, @0x8012153c..48 | `table 0x80121500 24` | 0x27 |
| Maggot Explosions-Clip | 10, von hinten (+0x93&0x80) 11 | STAGE1 @0x8011bc10 / @0x8011bc34 | `ori v0,zero,0xa` / `ori v0,zero,0xb` (+0x93&0x80) | Tod; HURT gleich @0x8011b474/98 |
| Maggot Aufsteh-Clip | 16, von hinten 17 | STAGE1 @0x8011b614 / @0x8011b638 | `ori v0,zero,0x10` / `ori v0,zero,0x11` | nur HURT (HP >= 1000) |
| Maggot/Kakerlake Rueckstoss | +0x8c = (rng&31)+80, dann 0x50 - 4*(+0x9c), pos_advance(0x800) | STAGE1 @0x8011bc58-70, @0x8011bd70-80 | `andi v0,v0,0x1f / addiu v0,v0,80 / sh v0,140`; `ori a0,zero,0x800 / jal 0x800245d8` | 0x27, 0x29 |
| Maggot Explosions-Tod SE | Raum-SE 0 | STAGE1 @0x8011bd04 | `jal 0x800453d0` / `addu a0,zero,zero` | 0x27 |
| Kakerlake HURT/DEATH-Spur | 0x80114cb8 / 0x801154b4 | STAGE3 @0x8011eda8..b0, @0x8011ee00..08 | `table 0x8011ed84 22`, `table 0x8011eddc 22` | 0x29 (STAGE4 0x80110358/0x80110b54, STAGE5 0x801104d8/0x80110cd4) |
| Kakerlake Explosions-Tod | Clip 10, von hinten 11, SE 7 | STAGE3 @0x80115528 / @0x8011554c, @0x8011561c | `ori v0,zero,0xa` / `ori v0,zero,0xb`; `jal 0x800453d0` / `ori a0,zero,0x7` | 0x29 |
| Blut-Vektor bei +0x6 = 1 | {200,-800,0} | STAGE1 @0x801213a8, STAGE3 @0x8011ec84 | `c8 00 00 00 e0 fc ff ff 00 00 00 00` | 0x27/0x29 Explosions-Spur |
| Alligator-Tod | Clip 13, Blut an (+0x188)+2644, Bild 20 -> Phase 2 | STAGE2 @0x8010eaa4, @0x8010eb34, @0x8010eba8 | `ori v0,zero,0xd`; `addiu a2,a2,2644`; `ori v0,zero,0x14` | 0x23 (alle +0x5) |
| Alligator Kasten-Versatz | (1000, -720, 0), mit Yaw gedreht | STAGE2 @0x80118b98; FUN_8002b498 @0x8002b4f4-51c | `e8 03 30 fd 00 00 98 08 d0 02 20 03` | Hitbox-Mitte 1000 vor dem Koerper |
| Tyrant Schluck-Tabelle | Byte[9..11] = 0 | STAGE4 @0x8011a1b1..b3 (Basis 0x8011a1a8) | `00 00 00` | HE/Saeure/Brand nie geschluckt |
| Tyrant Tod | alle +0x5 -> 0x80114cb0; Clip 8/9 -> 0xa/0xb | STAGE4 @0x8011a218..26c, @0x80114d24/4c, @0x80114f18/3c | `table 0x8011a218 22` | 0x2b |
| Birkin RE1.5 Zeile 9 | NULL | STAGE3 @0x8011f064 (HURT), @0x8011f304 (DEATH); STAGE5 @0x80120028 / @0x801202c8 | `tab2d.py` | unfertig |
| Ivy RE1.5 Riegel | +0x93 = 3 | STAGE4 @0x8011693c/44 | `ori v0,zero,0x3` / `sb v0,147(a1)` | immun (Gate B) |
| FX-Emitter Riegel | +0x93 = 1 | STAGE2 @0x8010ef28/58 | `ori v1,zero,0x1` / `sb v1,147(v0)` | immun |
| Writher RE1.5 Zeile 9..11 | NULL | STAGE1 @0x80120ac0..0x80120b1c, @0x80120db4..0x80120e10 | `tab2d.py STAGE1.BIN 801209a0 21 8` / `80120c94 22 8` | unfertig |
| NPC-Kasten 0x45 | {0,-1440,0,500,1440,500} | STAGE1 @0x80121728 (Zeiger @0x80121734) | `8011d2f8 lw v0,5940(v0)` / `8011d300 sw v0,120(v1)` | Port hat 450/1530 |
| NPC-Kasten 0x4b | {0,-1440,0,300,1440,300} | STAGE1 @0x801218c8 (Zeiger @0x801218d4) | `8011e3b8 sw v0,120(...)` | Port hat 450/1530 |
| NPC-HP | -1 | STAGE1 0x45 @0x8011d320/24 (alle NPC-INITs in `init_zensus.txt`) | `addiu v0,zero,-1` / `sh v0,154(v1)` | Kennung "unverwundbar" |
| RE2 NPC-Ausschluss | HP -1 und +0x10E \|= 0x5000 | RE2 @0x8005d7b4-bc, @0x8005d77c; Gates @0x80047148-50, @0x80047158-64 | `addiu v0,zero,-1 / sh v0,342(s0)`; `lh v0,342(s0) / bltz` | NPCs nie Kandidat |
| RE2 Zeilen-Zuordnung | 9 = Explosiv, 10 = Brand, 11 = Saeure | RE2 Op 47/48/49 (Hitcodes 0x10020009 / 0x0002000A / 0x1002000B, Schwester-Spur §1.1) | — | RE1.5 0x0A (Acid) -> RE2-Zeile 11; 0x0B (Incendiary) -> Zeile 10 |
| RE1.5-Granate -> RE2-Klammer | HE = Zeile 9 K1 (Op 47), Brand = Zeile 10 K0 + K2 (Op 48/40), Saeure = Zeile 11 K1 (Op 49) | RE1.5 Routine 29 0x80018320..43c (kein Resolver: nur `jal 0x80045024` @0x80018358/@0x80018424) | `dis 0x80018320 71` | welche Schadensspalte/+0x1D2 ein RE2-Gegner fuer die Granate bekommt |
| RE2 G1 HP | 500 | em30 @0x801004c8 | `addiu v0,zero,500` / `sh v0,342(s2)` | G1 |
| RE2 G1 Taumel-Zuschlag | Zeile 9 = 10 (Klammer >= 1: 5), 10 = 10, 11 = 20 | em30 @0x80107688..8a (Basis 0x8010767f + Zeile) | `lbu v0,30335(at)`; Zeile 9 `srl v0,v0,1` @0x80104b2c/54 | Akku +0x219 |
| RE2 G1 Taumel-Schwelle | 12 | em30 @0x80104ba4 | `sltiu v0,v0,0xc` | ab 12 Taumeln 0x80104c20 |
| RE2 G1 Taumel-Clip | 0x3000A - Seite (Clip 10 bzw. 9, Blend 3) | em30 @0x80104c70-80 | `lui v1,0x3 / ori v1,v1,0xa / subu v1,v1,v0 / sw v1,332(s0)` | G1 |
| RE2 G1 Saeure-SE | SE 0 beim Taumeln | em30 @0x80104c98-a8 | `addiu v0,zero,11 / bne / addu a0,zero,zero / jal 0x8005bd6c` | Zeile 11 |
| RE2 G1 GL-Schaden | 60/60/60, 70/70/5, 101/101/10 | RE2 @0x800A5810/24/38 | `3c f0 c0 03`, `46 18 51 00`, `65 94 a1 00` | Zeile 9/10/11 K0/K1/K2 |
| RE2 G5 Flinch-Byte | 14 fuer Zeile 9/10/11 | em36 @0x801056BC..BE | `0e 0e 0e`; `801029bc lbu v0,22195(at)` | Port `enemy_ai_boss_g5.c:1535` hat 20 |
| RE2 G5 Schwelle / Schuetteln | 15 / >= 11 -> +0x225 = 7 | em36 @0x80102a58 / @0x801029c4-d0 | `sltiu v0,v0,0xf`; `sltiu v0,v0,0xb / addiu v0,zero,7 / sb v0,549(s3)` | G5 |
| RE2 G5 GL-Schaden | 80/80/80, 70/70/5, 70/70/10 | RE2 @0x800A5F7C/90/A4 | `50 40 01 05`, `46 18 51 00`, `46 18 a1 00` | statt 1000 |
| RE2 G5 HP | 600 | em36 @0x801003fc | `addiu v0,zero,600` / `sh v0,342(s0)` | Port hat 600 |
| RE2 Alligator Grossreaktion | 1/4 (rng&3 == 0), Sperre (rng&0x3F)+120 | em23 @0x80101fac-d4 | `andi v0,v0,0x3 / bne`; `andi v0,v0,0x3f / addiu v0,v0,120 / sb v0,558(s0)` | zeilenunabhaengig |
| RE2 Alligator GL-Schaden | 30/30/30, 31/31/5, 25/25/25 | RE2 @0x800A4AB4/C8/DC | `1e 78 e0 01`, `1f 7c 50 00`, `19 64 90 01` | 2090-Design |
| RE2 Ivy HURT/DEATH je Zeile | HURT 9/10/11 = 0x8010371c/0x80103860/0x801038c4; DEATH = 0x80103df8/0x80103f4c/0x80104090 | em2e @0x80105994..9c, @0x801059e8..f0 | `table 0x80105970 20`, `table 0x801059c4 20` | nur bei RE2-Ivy-Kampf |

## PORT-ABGLEICH

Ausfuehrlich in §5 (Datei:Zeile je Typ). Kurz:
* `game_step_common.c:1775/1809` — Hand Grenade = Hitscan-Bruecke beim Abzug (Waffen-Zeile [Typ][9]); `:1779-1780` Acid/Incendiary ohne
  jede Wirkung. `re15_resolve_attack` (`re15_damage.c:3294`) + `re15_enemy_take_damage` (`:2959`) bilden den Original-Zweig bytegleich ab,
  **ohne Gate B** (`:3334`).
* Pro Typ nach Umbau auf die Explosion: 0x27 und 0x2b korrekt; 0x29 ohne Explosions-Spur (`enemy_ai_common.c:11367/11381`); 0x23 ohne
  Todesablauf (`:13173`) und ohne Kasten-Versatz (`re15_damage.c:3552`); 0x30/0x36 laufen in einen Tod, den das Original nicht hat (NULL);
  G5-Modul stirbt an 1000 (`enemy_ai_boss_g5.c:1506`), Flinch-Zuschlag 20 statt 14 (`:1535`); NPCs werden getroffen und fallen aus dem Skript
  (`enemy_ai_common.c:10550`), Kaesten 0x45/0x4b falsch (`re15_damage.c:3488`); 0x2d/0x24 immun wie im Original.

## OFFEN

1. **Birkin G1 (0x30/0x36) Zielmodell.** RE1.5 hat fuer Zeile 9..18 NULL (Sprung auf 0). Das RE2-G1-Verhalten ist belegt (§4.1), der Port
   faehrt aber die RE1.5-Birkin-KI (HP 300, Morph-Tod). Ob die Explosion dort den RE1.5-Tod (1000 > 300) ausloesen oder RE2-G1-Schaden und
   -Taumeln bekommen soll, ist keine RE-Frage mehr, sondern die Wahl des Zielsystems. Wege versucht: STAGE3/STAGE5-Tabellen (beide NULL),
   Mutations-Schutz (+0x1dd&8, nur waehrend der Mutation), RE2 em30 vollstaendig gelesen.
2. **Zeilen-Partikel der RE2-Bosse** (G1 0x80104f78/0x80104d30/0x80104e04, G5 0x8010221c/0x80101d9c/0x80101ff0, Ivy-Deskriptoren
   @0x80105a38): Aufrufe und erste Codes belegt (0x0504xxxx / 0x040Fxxxx), die vollstaendigen FX-Listen (a0 je Aufruf ueber s-Register) nicht
   ausgeschrieben. Weg: `jal_args.py` um Registerverfolgung ueber s0..s7 erweitern bzw. je Funktion `dis` lesen.
3. **"jalr 0"-Verhalten** (Birkin-/Writher-NULL-Zeilen) ist statisch belegt (NULL-Wort + `jalr v0`); was ab Adresse 0 laeuft
   (Endlosschleife = Haenger), hat die Schwester-Spur `re_absturz_original.md` §2.1b aus Savestate-RAM gelesen. Eine Messung mit
   einem Birkin/Writher im Radius steht aus (im Auslieferungsstand gibt es keine Granate); fuer den Port nicht noetig.
4. **Zellenarm (RE2-Flavor 0x1a)** und die RE2-Familie: Schwester-Spur `re_gegner_re2_familie.md`.
5. **Mr.X/Tyrant RE2 (em2a/em2b)**: Routine-Tabellen nicht gefunden (kein `lw v0,imm(at)`+`jalr`-Muster ab 0x80100100 in 400 Instruktionen);
   nicht noetig, weil die RE1.5-Tyrant-Spur fertig ist. Nur die GL-Records sind gelesen (`re2_gl_boss_records.txt`).
6. **Nebenbefund, nicht gemessen:** `re15_player_weapon_fire` filtert nur `hit_radius_min > 0` (Schleife `re15_damage.c` ab Z. 1947); seit die
   NPCs einen Kasten tragen, koennten sie auch fuer Schusswaffen Ziele sein. Im Original entscheidet der Band-Test (`800120d0-ec`:
   Wort0 & Spieler & 0xE0000000; NPC-INIT setzt 0x40000000 @0x8011d2e0-e8). Weg: Sonde mit Pistole auf einen NPC.
7. **Ivy als Kampfgegner**: die RE1.5-Ivy ist Kulisse; die RE2-Ivy-Tode je Zeile (§4.4) sind nur relevant, wenn die Ivy je Kampfgegner wird.
