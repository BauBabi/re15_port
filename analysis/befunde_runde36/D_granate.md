# Runde 36 — Spur D: Granate (HE ohne Feuer an Zombies; Wurf-Aussetzer)

Stand: 2026-10-10, Zweig `r36/granate`, Baum `.claude/worktrees/r36_granate`, Basis master 5a9dbb7b (v0.8.23 + Auftrag).
Dossier wird FORTLAUFEND geschrieben (Sitzungslimit-Schutz). Auftrag: `AUFTRAG.md` (woertlich), Vertrag: `VERTRAG.md`.
Vorrunde: `analysis/befunde_runde35/A_granate.md` (Wand/Reichweite/Sound/Brutalitaet), `B_werfer.md` (Granatwerfer).

## 0. Auftrag (Wortlaut des Nutzers)
1. "Bei normalen Granaten brauche ich keinen Feuer Effekt bei den Zombies, sondern nur das Zerplatzen der Körperteile.
   [Feuer] würde ich nur bei den incendiary Granaten erwarten."
2. "Manchmal selten löst die Granate bei der Verwendung nicht aus/wird nicht geworfen."

## 1. Ausgangslage (Stand master 5a9dbb7b, selbst nachgelesen)
* Handgranate Art 2 (Item 9), Saeure Art 3 (0x0A), Brand Art 4 (0x0B). Routinen 30/29/31 in re15_esp.c; Wand/Reichweite/
  Explosion in granate_r35.c (Runde 35 Spur A); Explosion stempelt RE2-Zeile 9, Klammer 0, Zone 1 -> DEATH[9][1] =
  0x80108BEC `re2z_death_rip` (Zerreissen) bzw. 0x80109610 (Wegschleudern).
* `re2z_death_rip` Phase 0 ruft fuer Zeile 9/17 ohne +0x10E&0x80 `re2z_gore_soot` = FUN_8010640C ("Spreng-Russ",
  `jal 0x8010640C` @0x80108CEC). Diese spawnt RE2-Effekt-Ids 0x05032710 (Part 0, v={0,200,0}, @0x80106418-44) und
  0x050313E8 (Part 3 R-Unterarm, @0x80106450-70) und setzt die Russ-Tinten (@0x8010648C-F8).
* Port-Dekoder `re2z_gore_fx_ex` (enemy_ai_re2_zombie.c:1107): RE2-Klasse 5 -> RE1.5-CORE00-Id 8 (= FEUER), "Klassen-
  Zuordnung, Sichtpruefung offen" (Runde 4, gore-vollausbau.md §2.1). Dieselbe Klasse 5 nutzt die Brand-Verkohlung
  FUN_80106128 (Zeile 10/16).

(Fortsetzung: Messung vorher / RE-Belege / Umsetzung / Messung nachher / Tests / OFFEN / Fuer den Nutzer)

## 2. Punkt 1 — HE-Granate: Feuer an den Zombies

### 2.1 Messung vorher (exe 5a9dbb7b, Messkopie `re15_pc_r36d.exe`, Lauf `build/r36d_mess/he_zombie`)
Aufstellung = integration_r35_granate "zombie": ROOM1140 per RE15_DEBUG_JUMP=1140@250, Leon (-1676,-18070) Blick 1076,
RE2-KI, RE15_GIVE=9:5 RE15_EQUIP=9, Skript `MD0.6,MDA0.2,MD2.5,W5` (TIEF-Wurf), RE15_FX_LOG=fx.log, RE15_EXIT_AT=330.
```
gr.log:  F=43 SPAWN granate art=2 ... T=329 liegen ... T=365 EV resolver art=2 P=(-2199,-480,-20548) r=2000 eingriffe=3
state.log F119: [2 t=10 st=3 ss1=9 ss2=0 mo=2 @(-1334,-19530)] hp=-150  [3 t=10 st=3 ss1=9 ss2=0 ...] = Explosionsbild X=119
fx.log (gezeichnete Teilchen je Bild): ab F=120 bis F=330 (Ende) in JEDEM Bild 4 Zeilen `id=8 sub=3 ... A=18` =
  F120: w(-1277,-522,-19546) w(-848,-741,-18907)      (Zombie 2: Huefte y+200 / rechter Unterarm)
        w(-1452,-1640,-20946) w(-1323,-975,-20727)   (Zombie 3)
  Summe 844 id=8-Zeilen = 211 Bilder x 4 Flammen; sie erloeschen bis zum Ende nicht.
```
= je zerrissenem Zombie ZWEI Flammen (RE1.5-CORE00-Id 8 = FEUER, Sub 3, Routine A 18), ab dem Bild nach dem
Explosionsbild (= `re2z_death_rip` Phase 0, ss2 0 -> 1). Keine andere Feuerquelle im Log: die HE-Explosion selbst
spawnt nur die RE1.5-Kinder 0x03195000/0x030B5400/0x030B5800 (Id 3, re15_esp.c case 31) — im fx.log `3/25`, `3/11`.

### 2.2 RE-Beleg (RE2 EMOVL10_S0.BIN, selbst disassembliert `re2_disasm.py dis 0x8010640c 66 --bin EMOVL10_S0.BIN`)
FUN_8010640C ("Spreng-Russ", Zeile 9 HE / 17 Rakete):
```
80106418 lui a0,0x503 / 8010641c ori a0,a0,0x2710      a0 = 0x05032710 (Klasse 5 = RE2-CORE00-Id 5 FEUER, Sub 3, Skala 10000)
80106424 addiu v0,zero,200 / 8010642c-34 sh zero,16 / sh v0,18 / sh zero,20   a3 = (0,200,0)
80106438 lw v0,408(s0) / 8010643c lh a1,118(s0) / 80106440 jal 0x8001bf10 / 80106444 addiu a2,v0,72   Anker Part 0
80106450 lw v0,516(a2) / 80106458 andi v0,v0,0x1 / 8010645c beq v0,zero,0x80106478                 Part 3 vorhanden?
80106460 lui a0,0x503 / 80106464 ori a0,a0,0x13e8 / 8010646c addiu a2,a2,588 / 80106470 jal 0x8001bf10   0x050313E8 Part 3
80106478-801064f8  Russ-Tinten 0x404040/0x484848/0x707070/0x505050 nach +0x70 der Parts 0,1,2,5,9,10,11,12,13,14
```
Aufruf nur im Zerreiss-Tod: FUN_80108BEC Phase 0 `lbu v1,5(s3) / addiu v0,zero,9 / beq v1,v0,0x80108cd8` @0x80108cc0-c8,
`addiu v0,zero,17 / bne v1,v0,0x80108cf4` @0x80108ccc-d0, `lhu v0,270(s3) / andi 0x80 / bne` @0x80108cd8-e4,
`jal 0x8010640c` @0x80108cec. Ebenso die Leitern @0x801051E8-208 (HURT) / @0x801084A0-B8 (DEATH-Zwilling).
RE2-CORE00.ESP Id 5 (Dump `re2esp_dump.py`): Sub 3 = Skript @0x0788, Step 0 Op A 2 (Anim-Start 0 + rand%5), Status 0xB803
(Bit 0x800 = folgt der Anker-Matrix), Step 1 Op B 25; Anim 0..9 = die Flammenbilder (UV 0x00A8.. Seite 0x1E, CLUT 0x7911),
dieselben wie Sub 0/1 der Verkohlung FUN_80106128 (Brand, Zeile 10/16). Also: RE2 Retail laesst die HE-Leiche brennen.
Port-Dekoder `re2z_gore_fx_ex` (enemy_ai_re2_zombie.c:1118): Klasse 5 -> RE1.5-CORE00-Id 8 (FEUER) Sub 3 = die gemessenen
Teilchen. Der Dekoder-Pfad `re2z_gore_soot` (enemy_ai_re2_zombie.c:4820-4835) ist 1:1 FUN_8010640C.

### 2.3 NUTZER-VORGABE (Abweichung von RE2 Retail)
"Bei normalen Granaten KEIN Feuer-Effekt an den Zombies, nur das Zerplatzen; Feuer nur bei incendiary." Die FORM der
Abweichung: die beiden Flammen-Spawns von FUN_8010640C (@0x80106418-44 und @0x80106450-74) entfallen fuer die Zeilen 9/17
(HE-Handgranate 0x09 = Art 2, GL-Explosivrunde w15, Rakete w18 — alle "explosiv, nicht incendiary"); die Russ-Tinten
(@0x80106478-F8, die Schwaerzung der Teile) bleiben, ebenso das Zerreissen/Wegschleudern (0x80108BEC/0x80109610). Die
Brand-Verkohlung FUN_80106128 (Zeile 10 = Brandgranate 0x0B Art 4, Zeile 16 Flammenwerfer) bleibt unveraendert (Feuer).
Saeure (Zeile 11, FUN_80106310) unveraendert.
