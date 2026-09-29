# Runde 34 / RE — Absturz des Originals beim Granateneinsatz (RE1.5-Beta, Auslieferungsstand)

Stand: 2026-09-29, RE-Phase abgeschlossen (keine Port-Aenderung, keine git-Schreiboperation). Offene Punkte: §5.
Auftrag: `AUFTRAG.md` (Nutzer-Nachtrag: "das Spiel im Original stuerzt unter Nutzung der Granate -
zum Beispiel gegen Zombies - ab"). Werkzeuge: `re_absturz_werkzeug/`, Laufzeit-Ausgaben:
`build/r34g_absturz/` (untracked). Quellen: `info/Re1.5/PSX.EXE` + `info/Re1.5/PSX/BIN/STAGE*.BIN`
per `re15_disasm.py` (jede zitierte Instruktion selbst disassembliert), Savestates `stage_saves/`
(nur saubere, NICHT `PATCHED-EXE_HASH-881C08B8082E53B6_*`), DuckStation mit der MZD-Disc (§2.1: 692 von
693 Dateien bytegleich zum Auslieferungsstand, einzige Abweichung CAPCOM.STR).
Schwester-Dossiers (nicht wiederholt, nur referenziert): `re_wurf_flug_explosion.md` (Wurf/Flug/
Explosion), `re_schaden_resolver.md` (FUN_80012d60), `re_gegner_*.md`, `re_saeure_brand.md`.

## 0. Ergebnis

**Der Absturz ist reproduziert und bis zur Instruktion belegt.** Er ist ein **Haenger (Standbild)**, keine
Ausnahme. Ablauf: Bild N — Routine 31 (Zuender 7) ruft `jal 0x80012d60` @0x800185b8 (500, P, Art 2); der
Resolver zieht dem Zombie **1000 HP** ab (DAT_8006f418[2]) und setzt **+0x4 = 3, +0x5 = 9** (DAT_8006f430[2]),
+0x6 = 1. Bild N+1 — Gegnerschleife FUN_8001a50c `8001a588: jalr v0` -> Zombie-Wurzel 0x80100424
`80100588: jalr v0` -> Todeswurzel 0x80106ba4 -> **`80106c00: jalr v0` mit v0 = 0** (Todestabelle
0x8011feac, Zeile 9 = +0x5, Spalte 1 = +0x6: Wort 0x8011ffd0 = 0). Bei physisch 0 steht im Spiel
`nop / addiu k0,k0,0xc80 / jr zero / nop` -> Endlosschleife; nur die Interrupts laufen weiter.
Gemessen: EPC = PC = 0x00000000, **RA = 0x80106c08**, v0 = 0, a0 = 0x8011feac, v1 = 0x8011ffcc,
a1 = Zombie; RA-Kette im Scratchpad 0x80106c08 <- 0x80100590 <- 0x8001a590 <- 0x8001ce0c (§2.7).

**Bedingung:** Explosion trifft einen **stehenden** (+9 & 0x80 = 0), **nicht gesperrten** (+0x93 Bit 0 = 0)
Gegner einer Familie mit 2D-Reaktionstabelle: Zombies 0x10/0x11/0x12/0x16/0x18/0x1c-0x1f (Stage 1-5),
Zombie-Maedchen 0x13, Gitterhaende 0x1a, G-Birkin 0x30/0x36 (Zeile 9 ueberall NULL, §1.5).
**Kein Absturz:** Wurf ohne Gegner (a), liegender/gesperrter Zombie (§2.6), Eigenschaden des Spielers
(§2.8, Tod), Gorilla 0x27 (§2.9, eigener Granaten-Tod), Hund/Kraehe/Spinne/Tyrant/NPC (statisch), Acid/
Incendiary (§2.10: Wurfanimation ohne Projektil). Beschaffung im Original: 0 Platzierungen, aber das
eingebaute **Item-Debug des Statusschirms** (SELECT im Item-Raster, R1 = Id+1, Menge 255, @0x8004a138-
0x8004a35c) liefert alle drei Granaten — so hat der Nutzer sie geholt; ohne RAM-Patch nachgefahren (§2.2b)
und damit der Haenger ein drittes Mal reproduziert (vb6, §2.5).

**Schaden:** existiert (1000) und wird angewandt (Zombie 81 -> -919, 105 -> -895, Gorilla 180 -> -820,
Spieler 100 -> -900). Kaputt ist nur die Gegnerreaktion Zeile 9 der 2D-Familien -> dort RE2 Retail als Ziel.

## Gliederung

0. Ergebnis
1. STATISCH — Kette und Absturz-Kandidaten
   1.1 Abzug -> Spawn · 1.2 Routinen 30/29/31 · 1.3 Kind-Effekte und SEs · 1.4 Resolver -> Gegner-KI
   (der Haenger) · 1.5 Zensus aller Gegnertypen aller Stages · 1.6 Kandidatenliste
2. DYNAMISCH — Reproduktion im Emulator
   2.1 Disc-Pruefung (+2.1b RAM ab 0x00000000) · 2.2 Inventar/Ausruesten (+2.2b Item-Debug SELECT+R1) ·
   2.3 Versuchsaufbau ·
   2.4 (a) Wurf ohne Gegner · 2.5 (b) Zombie im Radius = HAENGER · 2.6 Gegenproben liegend/gesperrt ·
   2.7 Absturz-Zustand (Register, Instruktion, RA-Kette) · 2.8 (s) Eigenschaden · 2.9 (c) Gorilla ·
   2.10 (d)/(e) Acid/Incendiary · 2.11 natuerliche Versuche
3. EINORDNUNG je Teil der Kette
4. Schadenswert DAT_8006f418[2] / DAT_8006f430[2]; Acid/Incendiary
5. OFFEN

Beleg-Bilder (Anzeigepuffer aus dem VRAM der Savestates, oben/unten nebeneinander, 2x):
`re_absturz_werkzeug/beleg_vb_haenger_standbild.png` (eingefrorenes Explosionsbild),
`beleg_va_explosion_ohne_gegner.png`, `beleg_va_granate_liegt.png`, `beleg_vs_eigenschaden_spieler.png`,
`beleg_vc3_gorilla_granatentod.png`.

---


## 1. STATISCH — Kette und Absturz-Kandidaten

Die Einzel-Mechanik von Wurf/Flug/Explosion ist in `re_wurf_flug_explosion.md`, der Resolver in
`re_schaden_resolver.md` belegt; hier nur die Stellen, die abstuerzen oder haengen KOENNEN, jeweils mit
eigener Disassembly (`re15_disasm.py`), und das Urteil.

### 1.1 Abzug -> Spawn (Waffen-FSM)

| Stelle | Instruktionen | Absturz-Risiko |
|---|---|---|
| Entlade-Dispatch `table 0x80074100` | [9] 0x80033B38, [10] 0x80033B58, [11] 0x80033B78; **[15]..[18] = 0** (Werfer) | 9/10/11: je 8 Instruktionen, nur `jal 0x8004eae4` (@0x80033b40 / @0x80033b60 / @0x80033b80) -> kein Risiko. Die NULL-Eintraege [15]..[18] betreffen nur Granatwerfer (0 Platzierungen, Runde 30 §3) |
| Spawn-Gate | `80033684: lbu v1,-13731(v1)` (0x800aca5d) / `80033688: ori v0,zero,0x9` / `8003368c: bne v1,v0,0x800337ac` | nur Id 9 spawnt; 10/11 laufen ohne Projektil weiter (dynamisch §2.10) |
| Spawn `jal 0x80019700` (a0 = 0x040D1000) | @0x800336bc-ec / @0x8003371c-4c / @0x80033778-a8 | Effekt 4 sub 0x0D liegt in CORE00.ESP (`re_wurf_flug_explosion.md` §3); kein Risiko |

### 1.2 Routinen 30 / 29 / 31 (Routinentabelle `table 0x80071d40 48`)

[29] 0x80018320, [30] 0x8001843c, [31] 0x8001854c — alle vorhanden; Routine A/B werden ueber diese
Tabelle per `jalr` gerufen (@0x80019e9c, @0x8001a2d4); die Granate setzt nur 0, 29, 31 -> kein NULL-Sprung.
Einzige Auffaelligkeit: Liege-SE @0x80018358 mit nicht beschriebenem a1 = sp+16 (Wurf-Dossier §1.2) ->
Lage-Daempfung aus Stapelmuell, **kein** Speicherzugriff ueber den Wert (FUN_80045a64 rechnet nur Abstand
und kappt, Wurf-Dossier §5). Dynamisch (a) ohne Absturz durchlaufen.

### 1.3 Kind-Effekte und SEs

| Aufruf | Stelle | Absturz-Risiko |
|---|---|---|
| FUN_800199d4(0x03195000 / 0x030B5400 / 0x030B5800) | @0x800185dc, @0x80018640, @0x80018660, @0x800186c8 | Effekt 3 sub 0x19 -> Zeilen-Tabelle [1] = base 0x3EC, sub 0x0B -> [3] = base 0x4C4 in CORE00.ESP (Wurf-Dossier §4); die Sub-Tabelle (sub & 7) @Datei 0x384 = `04 00 1a 00 30 00 50 00 66 00 7c 00 …` ist in [1] und [3] belegt (kein fehlendes sub). Dynamisch: 4 Kinder aktiv (va_b_01, vs_01), laufen ab, Plaetze frei |
| SE 0x04080001 (Bank 4 CORE, Satz 8) | `800185e4-ec` | CORE00.EDH Satz 8 vorhanden (Wurf-Dossier §5); Bank < 6 geprueft (`80045094: sltiu v0,v1,0x6`), Satz < 0x21 (`sltiu v0,s4,0x21`) |
| SE 0x010A0001 (Bank 1 ARMS, Satz 0x0A) | @0x80018358, @0x80018424 | ARMS09.EDH Satz 0x0A vorhanden |

### 1.4 FUN_80012d60 -> Gegner-KI: der Haenger

Der Resolver schreibt fuer jeden getroffenen, nicht gesperrten Gegner (`80012fd0`-`80013020`):
+0x7 := 0, +0x6 := 1, **+0x5 := DAT_8006f430[2] = 9**, HP -= **DAT_8006f418[2] = 1000**, +0x93 |= 1,
+0x4 := 2 bzw. 3 (HP < 0). Im naechsten Bild ruft die Gegnerschleife FUN_8001a50c die Typ-Wurzel
(`8001a588: jalr v0` ueber 0x80072bac[+0x8]); die Wurzel verteilt auf die Zustandstabelle [+0x4]; der
HURT- bzw. DEATH-Handler verteilt auf eine Reaktionstabelle mit **+0x5 = 9 als Index**.

Zombie-Familie STAGE1 (0x80072bac[0x10] = 0x80100424; Wurzel `80100568: lbu v0,4(v0)` /
`80100574`-`7c` Basis 0x8011f7b4 / `80100588: jalr v0`):
```
DEATH 0x80106ba4:  80106bb4 lbu v0,9(a1) / 80106bbc andi v0,v0,0x80 / 80106bc0 beq v0,zero,0x80106bd8
                   (liegend -> 80106bc8 jal 0x80107cb0, ohne Tabelle)
                   80106bd8 lui a0,0x8012 / 80106bdc addiu a0,a0,-340      -> 0x8011feac
                   80106be0 lbu v1,5(a1) / 80106be4 lbu v0,6(a1) / 80106be8 sll v1,v1,5 / 80106bec addu v1,v1,a0
                   80106bf0 sll v0,v0,2 / 80106bf4 addu v0,v0,v1 / 80106bf8 lw v0,0(v0) / 80106c00 jalr v0
HURT  0x80105a8c:  dasselbe Muster, Tabelle 0x8011fb90 (80105ae8 .. 80105b10 jalr v0)
read 0x8011feac 8 --rows 20 --rowstride 32:  Zeile 9 = [0,0,0,0,0x80107634,0,0,0]   -> [9][1] = 0
read 0x8011fcb0 8:                            Zeile 9 = [0,0,0,0,0,0,0,0]
```
Die angelegte Waffe 0x800aca5d fliesst hier NICHT ein: +0x5 kommt aus DAT_8006f430[Art]
(`80012fdc: lui at,0x8007` / `80012fe0: addiu at,at,-3024` / `80012fe4: addu at,at,s0` / `80012fe8: lbu v0,0(at)` /
`80012ff0: sb v0,5(s1)`). Die Waffen-Id als +0x5 schreibt nur der Hitscan FUN_80011f50 (`800124bc: sb fp,5(s1)`),
der fuer 9/10/11 nie laeuft (Schwester-Dossier `re_schaden_resolver.md` §6). Dass Zeile 9 = Waffe 9 ist, ist
Absicht der Tabelle (DAT_8006f430[2..4] = 9/10/11), nur ist die Zeile in den 2D-Tabellen leer.

**Kandidat K1 (bestaetigt, §2.5/§2.7): `80106c00: jalr v0` mit v0 = 0.** Gleiches fuer HURT `80105b10`
(nur bei HP >= 1000 erreichbar — kein Zombie hat das, HP 81..105 laut RAM).

### 1.5 Zensus aller Gegnertypen aller Stages (Zelle, die der Granatentreffer waehlt)

Werkzeug `re_absturz_werkzeug/typ_register.py` (Ausgabe `build/r34g_absturz/typ_register_alle.txt`):
sucht in jedem STAGE*.BIN die Eintraege in die Typ-Tabelle (`lui at,0x8007` / `sw rX,imm(at)` mit Ziel in
0x80072bac..+0x180, rX aus `lui/addiu`), dann in jeder Wurzel die +0x4-Tabelle und in deren [2]/[3] den
Tabellen-Sprung auf +0x5/+0x6. Gegenprobe STAGE1 gegen RAM (`mzd_stage1_engage_live.sav`, 0x80072bac):
dieselben 21 Typen mit denselben Wurzeln.

| Stage | Typ (Name laut Port) | HURT-Zelle | DEATH-Zelle | Urteil |
|---|---|---|---|---|
| 1-5 | 0x10 0x11 0x12 0x16 0x18 0x1c-0x1f Zombie | 2D [9][1] = **0** | 2D [9][1] = **0** (liegend +9&0x80: ohne Tabelle) | **HAENGER** (stehend) |
| 1-5 | 0x13 Zombie-Maedchen | 2D [9][1] = **0** | 2D [9][1] = **0** (liegend: `8010c038: jal 0x80107cb0`) | **HAENGER** (stehend) |
| 1 | 0x1a Gitterhaende/Writher | 2D 0x801209a0 [9][1] = **0** | 2D 0x80120c94 [9][1] = **0** (keine Liege-Pruefung) | **HAENGER** |
| 3, 5 | G-Birkin: ST3 0x30 + 0x36 (Wurzel 0x80116230), ST5 0x30 (Wurzel 0x80116a44) | HURT-Zweig ST3 0x8011a060 -> 2D 0x8011ef44 [9][1] = **0**; ST5 0x8011a874 -> 2D 0x8011ff08 [9][1] = **0** | 2D ST3 0x8011f1e4 / ST5 0x801201a8 [9][1] = **0** | **HAENGER** |
| 1, 3 | 0x20 Hund | 1D 0x80121018[9] = 0x80110b9c | 1D 0x80121070[9] = 0x80110eb0 | laeuft |
| 1 | 0x21 Kraehe | HURT = `jr ra` (0x80114e4c) | 1D 0x801211cc[9] = 0x801149c4 | laeuft |
| 1 | 0x26 Feuer-Emitter | 1D 0x80121290[9] = 0x80116a04 | dieselbe | laeuft |
| 1 | 0x27 Gorilla | 1D 0x801214a8[9] = 0x8011b400 | 1D 0x80121500[9] = **0x8011bb9c (eigener Granaten-Tod)** | laeuft (dynamisch §2.9) |
| 2 | 0x22, 0x23 Alligator, 0x24 FX, 0x25 Spinne | 0x8010c2d0 / 0x8010e748 / 0x8010f2c0 / 0x80113aa4 | 0x8010c330 / 0x8010ea30 / 0x8010f64c / 0x80113ff8 | laeuft (statisch) |
| 3-5 | 0x29 Schabe, 0x2b Tyrant | gueltige Zellen (Ausgabe) | gueltige Zellen | laeuft (statisch) |
| 4 | 0x2d Efeu | Zustandstabelle 0x8011a2c0: [2] = 0xfa060000, [3] = 0x01c20000 (Datenbytes, keine Zeiger) | — | nicht granatenspezifisch; OFFEN O4 |
| alle | NPC 0x40-0x4d | 1D auf +0x6: [1] = 0x80050ddc | 1D auf +0x6: [1] = 0x80050ddc | laeuft (Tod bei HP -1) |

**Folgerung:** Der Haenger ist keine Eigenschaft der Granate selbst, sondern der fehlenden Reaktionszeile 9
(= Resolver-Art 2) in allen Gegnerfamilien mit 2D-Reaktionstabelle (Zombies, Zombie-Maedchen,
Gitterhaende, G-Birkin). Familien mit 1D-Tabellen (Hund, Kraehe, Gorilla, Spinne, Alligator, Tyrant ...)
haben fuer die Zeilen 9/10/11 eigene Eintraege.

### 1.6 Kandidatenliste

| # | Adresse | Instruktion | Bedingung | Status |
|---|---|---|---|---|
| **K1** | STAGE1 `80106c00` (ST2 `80106a94`, ST3 `80106cec`, ST4 `80106bb4`, ST5 `80106d34`) | `jalr v0`, v0 = [0x8011feac + 9*32 + 1*4] = 0 | stehender Zombie der 0x10-Familie stirbt an der Explosion | **dynamisch bestaetigt (§2.5, §2.7)** |
| K2 | STAGE1 `80105b10` (u. Stage-Pendants) | `jalr v0`, Zombie-HURT [9][1] = 0 | Zombie ueberlebt 1000 (HP >= 1000) | unerreichbar (HP 81..105) |
| K3 | STAGE1 `8010c070` / `8010bffc` | Zombie-Maedchen DEATH/HURT [9][1] = 0 | stehendes 0x13 im Radius | statisch sicher (gleiches Muster wie K1) |
| K4 | STAGE1 `8010d4ac` / `8010d130` | 0x1a DEATH/HURT [9][1] = 0 | Gitterhaende im Radius (ROOM1210) | statisch sicher |
| K5 | STAGE3 `8011a5b8` / `8011a260`, STAGE5 `8011adcc` / `8011aa74` | G-Birkin DEATH/HURT [9][1] = 0 | Birkin im Radius | statisch sicher |
| K6 | EXE Entlade [15]..[18] = 0 (`table 0x80074100`) | `jalr` im Standard-FSM | nur Granatwerfer (0 Platzierungen) | nicht Granate |
| K7 | ESP Routine 29 @0x80018358 | SE mit a1 = Stapelmuell | jedes Liegenbleiben | kein Absturz (dynamisch (a)) |

## 2. DYNAMISCH — Reproduktion im Emulator

### 2.1 Disc-/Overlay-Pruefung — misst DuckStation den Auslieferungsstand? (JA, bis auf CAPCOM.STR)

Werkzeug `re_absturz_werkzeug/disc_vs_auslieferung.py`: liest das ISO9660-Dateisystem direkt aus
der MODE2/2352-.bin der MZD-Disc (Sektor 16 PVD, Nutzdaten ab Byte 24 je 2352-Sektor), extrahiert
jede Datei und vergleicht sie Byte fuer Byte mit `info/Re1.5/` (PSX.EXE) bzw. `info/Re1.5/PSX/`.
Ausgabe `build/r34g_absturz/disc_vs_auslieferung.txt`.

* **693 Dateien auf der Disc, 692 bytegleich**, 1 verschieden: `PSX/MOVIE/CAPCOM.STR` (Firmenlogo-Film,
  Disc 5 390 336 B / Baum 6 148 352 B) — der einzige Eingriff des "MZD Mod" auf Dateiebene, fuer die
  Granate belanglos.
* Bytegleich (md5) u.a.: `PSX.EXE` b55fdaa5…, `BIN/STAGE1.BIN` a482d196…, STAGE2..6, `DEBUG.BIN`,
  `TITLE.BIN`, **`DATA/CORE00.ESP` 3049f588…**, `PLD/PL00W09.PLW` = `PL00W0A.PLW` = `PL00W0B.PLW`
  (alle drei 50cf41fd… — die drei Granaten teilen dieselbe Waffendatei), alle `ROOM11x0/1.RDT`.
* "Nur im Baum" sind ausschliesslich Extraktions-Nebenprodukte (`VOICE/*.xa/.vag/.wav`, `*.bak`,
  `PLD/PL00_01` …) — nicht Teil der Disc.

**Folge:** Ein DuckStation-Lauf mit der MZD-Disc misst fuer die Granaten-Kette (EXE, Overlays, ESP,
Waffe, Raeume) den Auslieferungsstand. RAM-Gegenprobe der Zombie-Tabellen (Schwester-Dossier
`re_schaden_resolver.md` §8.1, `tab_bin_vs_ram.py`) ist damit konsistent.

#### 2.1b RAM-Inhalt ab physisch 0x00000000 (was ein Sprung nach 0 ausfuehrt)

`re15_ss.Ram(sav).bytes(0x80000000, 0x100)` in drei sauberen STAGE1-Saves
(`mzd_stage1_engage_live.sav`, `mzd_stage1_briefing_live.sav`, `room1140_entry.sav`) — identisch:

```
80000000: 00000003 275a0c80 00000008 00000000     <- "Garbage Area" des BIOS-Kernels
80000080: 3c1a0000 275a0c80 03400008 00000000     <- Ausnahmevektor (lui k0,0 / addiu k0,k0,0xc80 / jr k0 / nop)
```
Dekodiert (KUSEG 0x00000000 = RAM 0, dieselben Bytes):
```
00000000: 00000003  sra  zero,zero,0     (= nop)
00000004: 275a0c80  addiu k0,k0,0x0c80
00000008: 00000008  jr   zero            (rs = 0 -> Sprungziel 0x00000000)
0000000c: 00000000  nop                  (Verzoegerungsslot)
```
Ein `jalr` auf 0x00000000 landet also in einer **Endlosschleife 0x0 -> 0xc -> 0x0** (nur k0 waechst
um 0xc80 je Umlauf). Interrupts (VBlank) werden weiter bedient (Ausnahmevektor 0x80000080 ->
Kernel-Handler 0xc80 -> `rfe` zurueck in die Schleife), das Hauptprogramm kehrt nie zurueck:
**Standbild / Haenger**, kein Absturz mit Fehlermeldung. (Statische Vorhersage — dynamische
Bestaetigung §2.7.)

**Wort 0x8 im Vergleich aller sauberen Savestates** (`re15_ss.Ram(sav).u32(0x80000008)`, 78 Saves):
* **0x03400008 (`jr k0`)** in allen 25 Vorspann-/Titel-/Menue-Saves (boot_20..52, mzd_title, mzd_options,
  sub_newgame, nav_*, …) — so, wie psx-spx es fuer die Garbage Area beschreibt
  (`info/.../psx-spx.github.io-master/docs/kernelbios.md` Z. 137-152: `[00000008h]=03400008h`).
* **0x00000008 (`jr zero`)** in allen 53 Spiel-Saves (Raeume geladen: mzd_debugmenu, room1140_entry,
  lamp_1170_*, mzd_stage1_*, orig_1170_gp …).
Das Spiel selbst ueberschreibt also nach dem Spielstart das obere Halbwort von 0x8 mit 0 (Schreiber
nicht bestimmt, OFFEN O6). Folge: Im Spiel endet ein Sprung nach 0 **immer** in der Schleife 0x0..0xc
(Standbild); mit dem BIOS-Original `jr k0` waere es ein Sprung nach k0+0xc80 (irgendwo in den Code).
Die Wirkung ist damit durch den Spielzustand bestimmt, nicht durch den Emulator.

### 2.2 Inventar-/Ausruest-Adressen (belegt) und Ausruesten ueber den Original-Commit

Inventar 11 Plaetze x 4 Byte ab **0x800b10ac** {Id, Menge, Flag, 0}; ausgeruesteter Platz **0x800b25c8**;
ausgeruestete Waffen-Id **0x800aca5d** (Spieler 0x800aca54 + 9). Stand in allen sauberen STAGE1-Saves:
`01/0 03/15 15/50` (Messer, Browning 15, Munition 0x15 x50), 25c8 = 0 (Messer) bzw. 1 (Browning).

Ausruesten wie im Spiel, ohne Umweg: das Statusmenue schreibt beim Oeffnen einen Schnappschuss der
ausgeruesteten Id und vergleicht beim Schliessen (`re15_disasm.py dis 0x80046490 8` / `dis 0x800465f0 48`
/ `dis 0x800466a4 24`):
```
8004649c: sb v1,9678(at)            0x800b25ce := inv[25c8].Id   (Oeffnen: Schnappschuss)
800465f8: lbu v0,9672(v0)           25c8
80046600: lbu v1,9678(v1)           25ce
80046614: lbu v0,0(at)              inv[25c8*4].Id          (at = 0x800b10ac + 25c8*4 @0x80046608-10)
8004661c: beq v1,v0,0x800466cc      gleich -> kein Wechsel
80046688: sb v0,-13731(at)          0x800aca5d := inv[25c8].Id   (@0x80046654-84; 25c8==0x80 -> 1)
800466b0: jal 0x80036b68            Waffenmodell (Teil 11 <- PLW-Netz)
800466c4: jal 0x80043d8c            Ressource laden (a0 = aca5d @0x800466c0, a1 = 0x80198000 @0x800466b8/c8)
```
Vorgehen: `stage_saves/mzd_inv_open.sav` (Statusmenue OFFEN, ROOM1140 Briefing, Schnappschuss 25ce = 1)
per `re15_ss_patch.py` -> Inventarplatz 3 := `09 05 00 00` (Hand Grenade x5), 25c8 := 3
(`build/r34g_absturz/p1_inv_open_granate.sav`); geladen, START getippt -> der Original-Commit laeuft.
Ergebnis `build/r34g_absturz/p1/p1_nach_start.sav`: **aca5d = 0x09**, Menue zu (0x800b5359 = 0,
0x800aca3c = 0), Spieler Modus 1, Lage (-7600,0,-17600), Gier -96, HP 100.

#### 2.2b Beschaffung im Original ohne RAM-Eingriff: Item-Debug im Statusschirm (SELECT + R1)

Nutzer-Hinweis (nachgereicht, `re_absturz_werkzeug/NUTZER_HINWEIS.md`): "im player Inventory select und so
lange r druecken, bis die Granate im Inventar ausgewaehlt ist ... Cursor auf einem Player-Inventory-Item-Slot".
**Statisch belegt** — ITEM-Modus des Statusschirms FUN_8004a0cc (25c1 = 3), Kopf @0x8004a0cc-0x8004a35c
(`re15_disasm.py dis 0x8004a0cc 150`, `dis 0x8004a314 40`):
```
8004a138: lhu a1,-14494(a1)      0x800ac762 = rohe Flanke Pad 1
8004a140: andi v0,a1,0x100       SELECT
8004a144: beq v0,zero,0x8004a160
8004a150: sh v0,9832(at)         0x800b2668 := 1 (Debug-Zustand), SE 0x04090000 (8004a154/58: lui a0,0x409 / jal 0x80045024)
  Zustand 1: 8004a194-9c  jal 0x80013b60(a0 = 0xb, a1 = 0x801a0000, a2 = 0)   (Datei laden)
  Zustand 2: 8004a1d4     jal 0x800492b8 (Bild des Items fuer Platz 25bd)
             8004a1f0     sb v1,0(at)   inv[25bd].Id    := 0x800b2669
             8004a1f8/204 ori v1,zero,0xff / sb v1,1(at)   inv[25bd].Menge := 255
             8004a220     sb zero,1(at) (at = 0x800b10ae + 25bd*4)  inv[25bd].+3 := 0
             8004a224-2c  0x800b2668 := 3
  Zustand 3 (a1 = rohe Flanke): 8004a238 andi 0x8 (R1) -> 2669 += 1 (8004a258/60) ; andi 0x4 (L1) -> -1 ;
             andi 0x2 (R2) -> +10 ; andi 0x1 (L2) -> +246 (= -10) ; jeweils 2668 := 2 (neu schreiben)
             andi 0x20 (Kreis) -> 8004a308: sb zero,9832(at)  (Debug aus)
  Grenze:    8004a350 sltiu v0,v0,0x48 / 8004a35c sb v0(=0x47)   Id <= 0x47
```
**Dynamisch belegt** (`build/r34g_absturz/n1/`, KEIN RAM-Patch): Basis `stage_saves/inv_states/psx_inv_grid.sav`
(sauber: `@0x80026e4c = 08 00 e0 03`; Statusschirm offen, Tab ITEM, Raster, Cursor 25bd = 0 = Messer,
25c8 = 0, Schnappschuss 25ce = 1). Folge SELECT, 9x R1, Kreis, Kreuz, START:

| Save | 2668 | 2669 | Platz 0 | aca5d | Menue |
|---|---|---|---|---|---|
| n1_sel (nach SELECT) | 3 | 0 | `00 ff 00 00` | 01 | offen |
| n1_r9 (nach 9x R1) | 3 | **9** | **`09 ff 00 00`** (Hand Grenade x255) | 01 | offen |
| n1_exit (Kreis) | 0 | 9 | `09 ff 00 00` | 01 | offen |
| n1_cr (Kreuz) | 0 | 9 | `09 ff 00 00` | 01 | Tab-Auswahl (25c1 = 0) |
| n1_zu (START) | 0 | 9 | `09 ff 00 00` | **09** | zu (b5359 = 0) |

Da der ueberschriebene Platz 0 der ausgeruestete ist (25c8 = 0), ruestet der Schliess-Commit (§2.2) die
Granate sofort aus. **Die Granaten sind im Original also ohne Fremdeingriff erreichbar — ueber dieses
eingebaute Item-Debug (Hand Grenade = Id 9 = 9x R1, Acid = 10x, Incendiary = 11x).** Platzierungen in
Raeumen gibt es weiterhin keine (Runde 30 §3).

### 2.3 Versuchsaufbau

* Treiber `re_absturz_werkzeug/lauf.py` (vgamepad = SDL-0; `-statefile`; Zwischen-Savestates ueber den
  Hotkey SaveSelectedSaveState = SDL-0/LeftShoulder; Ende grazioes -> SaveStateOnExit). Prueft vorher,
  dass kein fremder Emulator laeuft.
* Auswertung `re_absturz_werkzeug/zustand.py` (ESP-Pool 0x800a73b8 x96 x0x84, Gegner 0x800acc2c x0x1f4,
  Spieler, aktueller Aktor 0x800ac784) und `re_absturz_werkzeug/ss_cpu.py` (CPU-Block des Savestates).
* **CPU-Block im Savestate** (Layout = `CPU::DoState` der installierten DuckStation, Stand
  e8938f06347e4837c32ef4c71c21cbd43245f244, `src/core/cpu_core.cpp`/`cpu_types.h`): Marker "CPU", 4 Worte
  Takt, `regs.r[34]` (r0..r31, hi, lo), pc, npc, BPC, BDA, TAR, BadVaddr, BDAM, BPCM, **EPC**, PRID, SR,
  CAUSE, DCIC, next/current_instruction, current_instruction_pc, 6 bool, Ladeverzoegerung,
  cache_control, **Scratchpad 1024 B**. Gegenprobe an drei sauberen Saves: jeweils `exc_raised = 1`,
  `next_instruction = 3c1a0000` (lui k0,0 = erste Vektor-Instruktion @0x80000080), EPC = unterbrochene
  Stelle (z.B. 0x800620f0 = VSync-Warteschleife), load_delay_reg = 34 (= keiner), cache_control
  0x0001e988 — DuckStation speichert am Bildende genau beim VBlank-Interrupt. **EPC ist damit die
  Stelle, an der die CPU beim Speichern stand.**

### 2.4 Variante (a) — Wurf TIEF, kein Gegner im Radius: laeuft durch, kein Absturz

`lauf.py --state p1/p1_nach_start.sav --seq "W1,DR1,W0.8,DDOWN,W0.6,TSQ,F6:0.4:va_a,UDOWN,UR1,F14:0.4:va_b,W2"`
(R1 halten, Steuerkreuz unten = TIEF, Viereck = Wurf). Filmstreifen `build/r34g_absturz/va/`:

| Save | Granaten-Platz (ESP[0], Kat 4 sub 0x0D) | Sonst |
|---|---|---|
| va_a_00 | noch nicht gespawnt | Spieler Modus 0x701 (Zielen) |
| va_a_01 | A=0 B=29 Zuender 42 Zaehler 5, Welt (-7175,-704,-17887), Flags 03 | Flug (Routine 29) |
| va_a_02 / 03 | Zaehler 4 / 3, y -43 / -16 | Abpraller |
| va_a_04 | **A=31 B=0 Zuender 42 Zaehler 0, (-5780,20,-18328), Flags 63** | liegt |
| va_a_05 / va_b_00 | Zuender 29 / 12 | Zuender zaehlt |
| va_b_01 | Platz frei, **4 Kind-Effekte (Kat 3 sub 0x19/0x0B) aktiv** | Explosion gelaufen |
| va_b_02..13, va_ende | alles frei | Spieler Modus 1, HP 100, alle 5 Zombies unveraendert (HP 97/81/81/105/103), EPC in normalem Spielcode (0x80025660, 0x8002551c, 0x8001ecd0 …) |

Liegestelle -> P = (-5780, 20-500, -18328); naechster Zombie (-1800,0,-19600) waagrecht ~4180 entfernt
(> 900), Spieler (-7835,0,-17390) ~2260 (> 950): der Resolver trifft niemanden. **Wurf, Flug, Abprall, Liegen, Zuender,
Explosion und Kind-Effekte laufen im Original ohne Absturz.**

### 2.5 Variante (b) — Explosion mit stehendem Zombie im Radius: REPRODUZIERT, das Spiel haengt

Aus `va/va_a_04.sav` (Granate liegt, Zuender 42 = 35 Bilder vor der Explosion) Zombie 1
(@0x800ace20, Typ 0x10, +9 = 0x00 = stehend/fressend, HP 81) per `re15_ss_patch.py` auf
(+0x34,+0x38,+0x3c) = (-5780, 0, -18628) gesetzt (300 neben der Liegestelle;
`build/r34g_absturz/p_vb_zombie1_an_granate.sav`). Pruefung gegen FUN_8002b5d0 (Schwester-Dossier
`re_schaden_resolver.md` §2.2): waagrecht 300 < 400+500, senkrecht |(20-500) - (0-1440)| = 960 < 500+1440 -> Treffer.
Lauf ohne jede Eingabe: `lauf.py --seq "F16:0.4:vb,W3"`, Filmstreifen `build/r34g_absturz/vb/`.

**Alle 16 Zwischenstaende (ueber ~7 s) und der Endstand sind bytegleich eingefroren:**
```
vb_00 … vb_15, vb_ende:  EPC=00000000 RA=80106c08
  Granate ESP[0]: A=31 Zuender=6 Flags=61 (unsichtbar)      <- Explosionsbild gelaufen (7 -> 6), danach nichts mehr
  Kind ESP[1]:    Kat 3 sub 0x19 (Feuerball), Flags 13, Satz 10  <- im Explosionsbild gespawnt, nie weitergeschaltet
  Zombie 1 @800ace20: Typ 10  +4=3 (DEATH) +5=9 +6=1 +7=0 +9=00  HP=-919 (= 81-1000)  +93=81
  aktueller Aktor 0x800ac784 = 800ace20 (= Zombie 1)
  Spieler: Modus 1, HP 100, Lage (-7335,0,-17565) = ~1730 von P (> 950)
```

**Wiederholung mit anderem Zombie-Typ (vb4):** Zombie 3 (@0x800ad208, **Typ 0x11**, HP 105, fressend)
auf (-5480, 0, -18328) gesetzt (`p_vb4_zombie3_typ11.sav`), Lauf ohne Eingabe (`build/r34g_absturz/vb4/`):
wieder alle Staende eingefroren, **EPC = 0x00000000, RA = 0x80106c08**, Zombie 3 +4/+5/+6 = 3/9/1,
HP 105-1000 = **-895**, aktueller Aktor 0x800ac784 = 0x800ad208.

**Dritte Wiederholung mit natuerlich beschaffter Granate (vb6):** Granate ueber §2.2b geholt (kein
Inventar-Patch), geworfen (`n2/n2_w1.sav`: liegt bei (-3755,20,-21811), Zuender 36), nur Zombie 2
(@0x800ad014, Typ 0x10, fressend) auf (-3455,0,-21811) gesetzt (`p_vb6_natuerliche_granate.sav`), Lauf ohne
Eingabe (`build/r34g_absturz/vb6/`): **EPC = 0x00000000, RA = 0x80106c08**, Zombie 2 +4/+5/+6 = 3/9/1,
HP -919, aktueller Aktor 0x800ac784 = 0x800ad014, alle 9 Staende eingefroren.

**Nicht verwertbar (vb5):** Versuch mit einem wachen Zombie (+5 = 2) aus `vn3_03/a_05.sav`; bis zum ersten
Zwischenstand liefen im Emulator schon mehrere Sekunden (der Treiber wartet 14 s ab Prozessstart, der Save
laeuft ab dem Laden weiter), der Zombie war aus dem Radius gelaufen (HP 81 unveraendert, nicht getroffen).
Kein Gegenbeleg — ohne Treffer keine Reaktion.

### 2.6 Gegenproben zu (b): liegender Zombie / Treffer-Riegel — kein Haenger

* **(b2) liegender Zombie 0x16** (Zombie 0 @0x800acc2c, +9 = 0x88, +0x93 = 0x01 wie in allen
  Briefing-Saves) an dieselbe Stelle (-5780,0,-18628) gepatcht (`p_vb2_liegend.sav`), Lauf ohne Eingabe
  (`build/r34g_absturz/vb2/`): Explosion laeuft, Spiel laeuft weiter (EPC in normalem Code), Zombie 0
  danach **+0x93 = 0x83, HP 97 unveraendert, +4 = 1** — genau der gesperrte Resolver-Zweig
  (`80012fb4: lbu v1,147(s1)` / `80012fbc: andi v0,v1,0x1` / `80012fc0: beq` / `80012fc4: ori v0,v1,0x2` /
  `80012fcc: sb v0,147(s1)`; Bit 0x80 = Punkt hinten @0x80012fac/b0): **kein Schaden, kein Zustandswechsel.**
* **(b3) derselbe mit +0x93 := 0** vor dem Laden (`p_vb3_liegend_93frei.sav`, gepatcht belegt:
  `+93=00`): Ergebnis identisch (+0x93 = 0x83, HP 97, laeuft weiter). Der Riegel ist also zum
  Explosionsbild wieder gesetzt; wer ihn beim liegenden 0x16 setzt, ist nicht bestimmt (28
  `ori …,0x1`/`sb …,147(…)`-Paare in STAGE1, Scan-Ausgabe in diesem Lauf) -> OFFEN O2. Fuer die Frage
  "Absturz" genuegt: **ein liegender Briefing-Zombie ist gegen die Granate immun, es gibt keinen
  Todeszweig und keinen Haenger.** Der Liege-Todeszweig selbst (FUN_80107cb0, +9 & 0x80 @0x80106bb4-c8)
  verzweigt auf +0x7 (`80107cc4: lbu v1,7(a0)`) und liest +0x5 nicht (`dis 0x80107cb0 120`: kein
  `lbu …,5(…)`, kein `jalr`; die einzige Tabelle @0x8011f784 wird mit dem Typ +0x8 indiziert,
  `80107e24: lbu v0,8(a2)` / `80107e38: addu at,at,v0`).

### 2.7 Absturz-Zustand: Register, Instruktion, Aufrufkette (aus `vb/vb_ende.sav`, identisch in vb_00)

`ss_cpu.py build/r34g_absturz/vb/vb_ende.sav`:
```
zero=00000000  at  =8011f7c0  v0  =00000000  v1  =8011ffcc
a0  =8011feac  a1  =800ace20  a2  =801223b6  a3  =00050000
s0  =800aca88  s1  =00000005  s2  =800ac784  s3  =80072bac
k0  =19f0f780  gp  =8006e548  sp  =1f8003a0  fp  =801ff400  ra  =80106c08
pc=00000000 npc=80000084  next_instr=3c1a0000  cur_instr_pc=00000000
EPC=00000000 SR=40000401 CAUSE=00000400 (ExcCode 0 = Interrupt, IP2 = Hardware-Interrupt ueber I_STAT, BD=0)
```
Deutung Register fuer Register (jede Zuordnung gegen die Instruktion, die das Register zuletzt schreibt):

| Register | Wert | geschrieben von | Bedeutung |
|---|---|---|---|
| ra | 0x80106c08 | `80106c00: jalr v0` (STAGE1, Todeswurzel 0x80106ba4) | Ruecksprung hinter dem `jalr` -> dieser Sprung war der letzte Aufruf |
| v0 | 0x00000000 | `80106bf8: lw v0,0(v0)` | Sprungziel = **NULL** |
| a0 | 0x8011feac | `80106bd8: lui a0,0x8012` / `80106bdc: addiu a0,a0,-340` | Todestabelle der Zombie-Familie |
| v1 | 0x8011ffcc | `80106be0: lbu v1,5(a1)` (=9) / `80106be8: sll v1,v1,5` / `80106bec: addu v1,v1,a0` | Zeile 9 (+0x5 = 9 = DAT_8006f430[2]) |
| (v0 vor dem lw) | 0x8011ffd0 | `80106be4: lbu v0,6(a1)` (=1) / `sll v0,v0,2` / `addu v0,v0,v1` | Zelle [9][1]; `re15_disasm.py read 0x8011feac 8 --rows 20 --rowstride 32`: Zeile 9 = `[0,0,0,0,0x80107634,0,0,0]` -> [9][1] = **0** |
| a1 | 0x800ace20 | `80106ba4: lui a1,0x800b` / `80106ba8: lw a1,-14460(a1)` (0x800ac784) | Zombie 1 |
| at | 0x8011f7c0 | `80100574/78: lui at,0x8012 / addiu at,at,-2124` + `addu at,at,v0` (v0 = +0x4*4 = 12) | Zustandstabelle 0x8011f7b4[3] = 0x80106ba4 (DEATH) |
| s3 / s2 / s1 | 0x80072bac / 0x800ac784 / 5 | FUN_8001a50c `8001a544/48` / `8001a54c` / `8001a528` | Gegnerschleife: Typ-Tabelle, Aktor-Zeiger, Zahl aktiver Gegner |
| pc = EPC | 0x00000000 | — | der Hardware-Interrupt (DuckStation speichert am Bildende, §2.3) wurde bei PC 0x00000000 genommen |
| k0 | 0x19f0f780 | `00000004: addiu k0,k0,0x0c80` (Schleife ab 0) | 0x19f0f780 / 0xc80 = 136 007 Umlaeufe seit dem letzten Interrupt-Ruecksprung (k0 = EPC = 0) = 544 028 Instruktionen je Bild ~ 33 MHz / 60 Hz: die CPU dreht die 4-Instruktionen-Schleife 0x0..0xc |

**Die abstuerzende Instruktion:** `80106c00: jalr v0` mit **v0 = 0x00000000** (geladen von
`80106bf8: lw v0,0(v0)` aus 0x8011ffd0 = Todestabelle Zombie [Zeile 9][Spalte 1]). Danach laeuft die
CPU in der Kernel-"Garbage Area" (§2.1b): `0x0 nop / 0x4 addiu k0,k0,0xc80 / 0x8 jr zero / 0xc nop` —
eine Endlosschleife. Kein Absturzbild, keine Ausnahme: Interrupts laufen weiter (VBlank-Handler kehrt per
`rfe` in die Schleife zurueck), das Hauptprogramm nie. Sichtbar: **Standbild (Spiel friert ein)**.

**Aufrufkette (RA-Kette aus dem Scratchpad-Stapel, sp = 0x1f8003a0; Scratchpad aus dem CPU-Block,
Gegenprobe load_delay_reg 34 / cache_control 0x1e988 an der erwarteten Stelle):**
```
1f8003a0: 800aca88 00000005 800ac784 80072bac   <- Rahmen 0x80106ba4 (addiu sp,sp,-24)
1f8003b0: 80100590 80100634 ffffe96c fffffe20   <- [sp+16] = RA 0x80100590
1f8003c0: ffffb868 00001136 00000002 8001a590   <- Rahmen FUN_80100424 (sp 1f8003b8, -24): [+16] s0 = 2, [+20] RA 0x8001a590
1f8003d0: 800aca34 800ac784 800b8520 800280ec   <- Rahmen FUN_8001a50c (sp 1f8003d0, -40)
1f8003e0: 800b5358 800ac784 00000000 00000060
1f8003f0: 8001ce0c 8001004c 00000000 801ff3e0   <- [+32] RA 0x8001ce0c
```
| Ebene | Funktion | Aufrufstelle | Beleg |
|---|---|---|---|
| 0 | Spielbild (Hauptlauf) | `8001ce04: jal 0x8001a50c` (Gegner VOR Spieler @0x8001ce0c und ESP @0x8001ce2c) | RA 0x8001ce0c @0x1f8003f0 |
| 1 | FUN_8001a50c Gegnerschleife | `8001a570: lbu v0,8(v1)` Typ / `8001a57c: addu v0,v0,s3` (0x80072bac) / `8001a580: lw v0,0(v0)` / **`8001a588: jalr v0`** (s0 im Verzoegerungsslot +1 -> 2 = zweiter Platz) | RA 0x8001a590 @0x1f8003cc, s0 = 2 @0x1f8003c8 |
| 2 | FUN_80100424 Zombie-Wurzel (0x80072bac[0x10]) | `80100568: lbu v0,4(v0)` (+0x4 = 3) / `80100574-7c` Basis 0x8011f7b4 / `80100580: lw v0,0(at)` / **`80100588: jalr v0`** | RA 0x80100590 @0x1f8003b0 |
| 3 | 0x80106ba4 Todeswurzel (0x8011f7b4[3]) | `80106bb4-c0: lbu v0,9(a1) / andi 0x80 / beq` (stehend -> Tabelle) / `80106be0-f8` Index / **`80106c00: jalr v0` -> 0x00000000** | ra 0x80106c08, v0 0 |
| 4 | 0x00000000 "Garbage Area" | `00000008: jr zero` | EPC 0, k0-Zaehler |

**Zeitlicher Ablauf:** Bild N: ESP-Treiber (`8001ce2c: jal 0x80019e20`) -> Routine 31, Zuender 7 ->
`jal 0x80012d60` (@0x800185b8) -> Zombie +0x4 := 3, +0x5 := 9, +0x6 := 1, HP 81-1000 = -919; Kind 0x03195000
gespawnt; Zuender 7 -> 6. Bild N+1: Gegnerschleife vor dem ESP-Treiber -> Todeswurzel -> `jalr` auf 0 ->
Haenger. Belegt dadurch, dass der Granaten-Zuender auf **6** steht (genau ein Abzug nach 7) und der
Feuerball auf seinem Startsatz 10 steht (nie ein weiterer ESP-Tick).

### 2.8 Variante (s) — Spieler laeuft in die eigene Explosion: Tod, kein Haenger

`lauf.py --state p1/p1_nach_start.sav --seq "W1,DR1,W0.8,DDOWN,W0.6,TSQ,W1.3,UDOWN,UR1,HUP:1.2,F14:0.35:vs,W2"`
(Wurf TIEF, danach 1,2 s vorwaerts zur Liegestelle). Filmstreifen `build/r34g_absturz/vs/`:
* vs_00: Granate liegt (-5780,20,-18328), Zuender 11; Spieler (-5353,0,-18271) = ~430 waagrecht entfernt.
* vs_01: 4 Kind-Effekte (2x sub 0x19 an y -480, 2x sub 0x0B steigend, vy -125/-115), **Spieler HP 100 -> -900,
  Modus 3, Clip +0x94 = 7** (Tod, Schwester-Dossier `re_schaden_resolver.md` §4.3), +0x93 = 1.
* vs_11..vs_ende: **Modus 7** (Blutlache / Todesablauf), EPC in normalem Spielcode — das Spiel laeuft weiter.
* Beleg-Bild `re_absturz_werkzeug/beleg_vs_eigenschaden_spieler.png`.
**Eigenschaden (1000, Spielerzweig FUN_80012d60 @0x80012e18-0x80012f00) laeuft im Original ohne Absturz.**

### 2.9 Variante (c) — anderer Gegnertyp: Gorilla 0x27 (ROOM11C0) stirbt sauber

Weg ohne Stellungs-Patch des Gegners: `p1/p1_nach_start.sav` (Granate ausgeruestet) -> SELECT oeffnet das
Debug-Menue (0x800bbe5c = 1, Zeile 0x800bbe5d = 1 = JUMP, Stage 0, Index 0x14; Aufruf FUN_8001443c aus
dem Hauptlauf `8001c988: jal 0x8001443c`) -> 8x RECHTS -> Index 0x1c -> Viereck -> ROOM11C0 mit
ausgeruesteter Granate (`g11c0/g11c0_geladen.sav`: 0x800b0fe2 = 0x1c, aca5d = 9, 25c8 = 3). Nach dem
Vorspann (Kreuz-Tipps, `g11c0/g_b.sav`) Spieler frei, zwei Gorillas wach (+9 = 0x10).
* Ohne Eingriff beisst der Gorilla den Wurf ab (`vc/`: HP 100 -> 40, Modus 0x202/0x302, kein Spawn).
* Daher beide Gorillas geparkt (+9 := 0x30, Tick-Sperre der Gorilla-Wurzel `80116dec: lbu v0,9(v1)` / `80116df4: andi v0,v0,0x20` / `80116df8: bne v0,zero,0x80116e88`, wie zu Raumbeginn) ->
  Wurf TIEF (`vc2/`): Granate liegt bei (-6310,20,-14003); Explosion trifft den **geparkten** Gorilla 1
  (HP 180 -> -820, +4 = 3, +5 = 9, +6 = 1) — ohne Tick keine Reaktion, Spiel laeuft.
* **(c) eigentlich:** `vc2/vc2_a_05.sav` (Zuender 27) mit Gorilla 1 +9 := 0x10 (wach, tickt)
  (`p_vc3_gorilla_wach.sav`), Lauf ohne Eingabe (`vc3/`): Gorilla 1 danach **+4 = 7, +7 = 2, Clip 10,
  HP -820, liegt in einer Blutlache**; Spiel laeuft weiter (EPC in normalem Code, alle 15 Staende).
  Beleg-Bild `re_absturz_werkzeug/beleg_vc3_gorilla_granatentod.png`.
  Mechanismus: Gorilla-Todeswurzel 0x8011b6fc -> `8011b780: lbu v0,5(v0)` / `8011b790: addiu at,at,5376`
  (0x80121500) / `8011b7a0: jalr v0`; `table 0x80121500`: **[9] = [10] = [11] = 0x8011bb9c** (eigener
  Granaten-Tod), [7]/[8]/[13] = 0x8011b998, sonst 0x8011b7b8; 0x8011bb9c setzt u.a. +7 := 1
  (`8011bc00: ori v0,zero,0x1` / `8011bc04: sb v0,7(v1)`) und schreibt am Funktionsende das Zustandswort
  (`8011bdd4: ori v0,zero,0x7` / `8011bdd8: sw v0,4(v1)` -> +4 = 7, +5..+7 = 0; `jr ra` @0x8011bde4) ->
  Zustand 7 = `table 0x801213c8` [7] = 0x8011be54 (dort wird +7 weitergezaehlt, `8011be94: sb v0,7(v1)`;
  gemessen +7 = 2).

### 2.10 Varianten (d)/(e) — Acid (0x0A) und Incendiary (0x0B): Wurfanimation ohne Granate, kein Absturz

Wie §2.2 ueber den Original-Commit ausgeruestet (`mzd_inv_open.sav`, Platz 3 := 0x0A bzw. 0x0B x5,
25c8 := 3, START), dann R1 (+ unten) und Viereck. Filmstreifen `build/r34g_absturz/vd/` (0x0A, TIEF)
und `build/r34g_absturz/ve/` (0x0B, MITTE):

| Save | aca5d | Platz 3 | Modus | Clip / Bild | acaec | ESP-Pool |
|---|---|---|---|---|---|---|
| vd_equip | 0x0a | 0a **05** | 1 | 3 / 0 | 0 | leer |
| vd_a_00..02 | 0x0a | 0a **04** | 0x701 | **11** / 11 -> 24 -> 38 | 0x2000 (TIEF) | **leer** |
| vd_a_03..07, vd_b_00 | 0x0a | 0a 04 | 0x701 | 12 / 0, dann 6 / 5 | — | leer |
| vd_b_01..ende | 0x0a | 0a 04 | 1 | 3 / 0 | 0 | leer |
| ve_equip | 0x0b | 0b **05** | 1 | 3 / 0 | 0 | leer |
| ve_a_00..01 | 0x0b | 0b **04** | 0x701 | **7** / 12 -> 25 | 0x4000 (MITTE) | **leer** |
| ve_a_02..ende | 0x0b | 0b 04 | 0x701 -> 1 | 8 / 0 ... 3 / 0 | — | leer |

* Die Wurfanimation laeuft (dieselbe Waffendatei: `PL00W0A.PLW` = `PL00W0B.PLW` = `PL00W09.PLW`, md5 50cf41fd…,
  §2.1), die Menge sinkt um 1 (Entlade-Handler `80033b60` / `80033b80: jal 0x8004eae4`), **das
  Spawnbild vergeht ohne Projektil** (Gate `8003368c: bne v1,v0,0x800337ac` mit v0 = 9), danach normaler
  Spielbetrieb. **Kein Absturz, kein Wurf, kein Schaden.**
* Erreichbarkeit im Auslieferungsstand: 0 Platzierungen in Raeumen (Runde 30 §3), aber **ueber das
  Item-Debug des Statusschirms** (§2.2b: SELECT im Item-Raster, 10x bzw. 11x R1) bekommt man Acid/Incendiary
  genauso wie die Hand Grenade. Das Debug-Menue des Spielbilds (FUN_8001443c) hat dagegen keinen Item-Weg:
  es verzweigt nur fuer Zeile 1 (JUMP, `80014698: beq v1,v0,0x800146b8` mit v0 = 1) und Zeile 2 (MEMORY
  VIEWER, `800146a8: beq v1,v0,0x80014a64` mit v0 = 2); Zeile 0 "UTILITY MENU" faellt nach
  `800146a0: bne v0,zero,0x80014ab4` direkt in die Anzeige; der MEMORY VIEWER 0x80014e78 gibt nur Text aus.
  **Ergebnis fuer 0x0A/0x0B: erreichbar, Wurfanimation und Munitionsabzug laufen, es fliegt nichts —
  unfertig, aber kein Absturz.**

### 2.11 Natuerliche Versuche ohne Gegner-Patch (Briefing-Zombies)

Aus `p1/p1_nach_start.sav` zu den fressenden Zombies gelaufen (Zwischenstaende `vn2/vw_00..09`, Laufen
~1050 Einheiten je 0,45 s) und aus drei Abstaenden TIEF geworfen (R1 zielt automatisch auf den naechsten
Gegner):

| Start | Liegestelle | naechster Zombie (Lage im Zwischenstand nahe Zuender 7) | Abstand waagrecht | Ergebnis |
|---|---|---|---|---|
| `vw_02` (Z2 3283 entfernt) | (-3211,20,-21848) | Z2 (-1800,-21600), fressend (+5 = 0x0c bis zum Ende) | ~1433 | kein Treffer, laeuft |
| `vw_02` + 0,3 s vor (`vn4/`) | (-2640,20,-21901) | Z2 erwacht, bei Zuender 16 schon bei (-1034,-19835) | ~2617 | kein Treffer, laeuft |
| `vw_03` (Z2 2248) (`vn3_03/`) | (-2411,20,-20609) | Z1 (-1420,0,-20706) bei Zuender 4 (3 Bilder nach der Explosion) | **996** (> 900) | knapp verfehlt, laeuft; Spieler danach gepackt |
| `vw_04` (Z2 1336) (`vn3_04/`) | — | — | — | Spieler vor dem Wurf getoetet |

Mit der natuerlich beschafften Granate (§2.2b, 255 Stueck) weitere Wuerfe ohne jeden Eingriff:
* `n2/`: 5 Wuerfe aus (-5486,0,-21669) Richtung Osten, alle liegen bei ~(-3757,20,-21828) (Menge 255 -> 250 =
  `09 fa`), ~1960 vor Z2 (-1800,-21600); **die fressenden Zombies wachen trotz 5 Explosionen nicht auf**
  (+5 bleibt 0x0c) — passt zum fehlenden Laerm-Mechanismus (§3).
* `n3_0.5/`, `n3_0.7/`, `n3_0.9/`: 0,5/0,7/0,9 s naeher, dann Wurf: Zombies erwachen (+5 = 2) und laufen
  nicht geradlinig auf Leon zu (z.B. Z2 in 0,4-s-Schritten (-1779,-20459) -> (-1830,-19960) -> (-1960,-19488) ->
  ... -> (-5209,-17896)), kein Treffer; bei 0,9 s wird Leon gepackt und getoetet.

Ein natuerlicher Treffer gelang in diesen Versuchen nicht (Zombies erwachen bei Annaeherung und bewegen
sich; die Reichweite ist 900 waagrecht). Fuer die Mechanik ist das ohne Belang: (b) zeigt, dass JEDER
stehende Zombie im Radius den Haenger ausloest; der Gegner-Patch in (b) setzt nur die Lage (+0x34/+0x3c),
der Tabellenzugriff haengt allein an +0x5/+0x6/+0x9 (§1.4).

## 3. EINORDNUNG je Teil der Kette (Grundlage Beta -> Retail)

"laeuft" = im Original gemessen ohne Absturz durchlaufen (RE1.5 byte-true massgeblich);
"kaputt" = fuehrt im Original zum Haenger (RE2 Retail als Ziel, Beleg aus RE2 — Schwester-Dossiers);
"unerreichbar" = im Auslieferungsstand ohne Fremdeingriff nicht erreichbar.

| Teil | Status | Beleg |
|---|---|---|
| Beschaffung Hand Grenade 0x09 | laeuft (nur ueber das eingebaute Item-Debug) | 0 Platzierungen (Runde 30 §3); Statusschirm SELECT + 9x R1 schreibt `09 ff` in den Cursor-Platz (@0x8004a138-0x8004a35c), ohne RAM-Patch nachgefahren (§2.2b, n1) |
| Ausruesten 0x09 | laeuft | Original-Commit @0x800465f4-0x800466c8, gemessen aca5d := 9 (§2.2) |
| Wurf (Zielen, Clip, Munitionsabzug, Spawn Effekt 4 sub 0x0D) | laeuft | va_a_00/01: Modus 0x701, Inventarplatz 3 Menge 5 -> 4 (p1_nach_start -> va_a_00), ESP[0] Kat 4 sub 0x0D A=0 B=29 Zuender 42 |
| Flug (Routine 29) | laeuft | va_a_01..03: ESP[0] B=29 (Routine 29), Welt (-7175,-704,-17887) -> (-6295,-43,-18177) -> (-5865,-16,-18312) |
| Abprall | laeuft | va_a_01..03: Zaehler 5 -> 4 -> 3 je Bodenkontakt, Welt-y -704 -> -43 -> -16 |
| Liegen + Zuender (Routine 31) | laeuft | va_a_04: A=31, Flags 0x63, Zuender 42 -> 29 -> 12 (va_a_05, va_b_00) |
| Explosionsbild / Kind-Effekte (0x03195000, 0x030B5400, 0x030B5800) | laeuft | va_b_01 / vs_01: 4 Kinder (2x sub 0x19, 2x sub 0x0B steigend), danach frei; Bilder `beleg_va_explosion_ohne_gegner.png` |
| Ton (SE 0x04080001, Abprall-/Liege-SE 0x010A0001) | laeuft (Aufruf ohne Absturz durchlaufen; Hoerbarkeit nicht gemessen) | Zweig Zuender 7 laeuft nach `800185ec: jal 0x80045024` weiter (Zuender danach 6, vb); Liege-SE mit Stapelmuell-Lage ohne Folgen (va) |
| Licht-Latch 0x800b5358 | laeuft | gesetzt `8001857c`, Leser `8001ce60` (Wurf-Dossier §6); im eingefrorenen Explosionsbild ist Leon orange angestrahlt (`beleg_vb_haenger_standbild.png`) |
| Laerm fuer Gegner | unerreichbar (kein Mechanismus) | der Latch ist Licht, kein Overlay liest ihn (Wurf-Dossier §6) |
| Schaden Spieler (1000, Tod Modus 3 -> 7) | laeuft | vs: HP 100 -> -900, Modus 3, Clip 7, danach Modus 7, Spiel laeuft |
| Schaden Gegner (HP -= 1000, +4/+5/+6 = 3/9/1) | laeuft | vb: Zombie HP 81 -> -919; vb4: 105 -> -895; vc2/vc3: Gorilla 180 -> -820 |
| Gegnerreaktion Zombie 0x10-Familie (stehend), 0x13, 0x1a, G-Birkin 0x30/0x36 | **kaputt (Haenger)** | Reaktionszeile 9 NULL (§1.4/§1.5); dynamisch vb/vb4: `80106c00: jalr v0` mit v0 = 0 -> Schleife 0x0..0xc |
| Gegnerreaktion liegender Zombie / Gegner mit Treffer-Riegel | laeuft (keine Wirkung) | vb2/vb3: +0x93 0x01 -> 0x83, HP unveraendert; Liege-Tod ohne Tabelle (§2.6) |
| Gegnerreaktion Gorilla 0x27 | laeuft (eigener Granaten-Tod) | vc3: +4 = 7, Clip 10, Blutlache; `table 0x80121500` [9] = 0x8011bb9c |
| Gegnerreaktion Hund, Kraehe, Feuer, Spinne, Alligator, Schabe, Tyrant, NPC | laeuft (statisch) | gueltige Zellen fuer +0x5 = 9 bzw. +0x6 = 1 (§1.5) |
| Acid 0x0A / Incendiary 0x0B | **kaputt/unfertig** (erreichbar ueber das Item-Debug, Wurf ohne Projektil, kein Absturz) | vd/ve: Clip 11/7 laeuft, Menge -1, kein ESP-Platz, Gate `8003368c: bne v1,v0,0x800337ac` (v0 = 9) (§2.10); Beschaffung 10x/11x R1 (§2.2b) |

**Kurz:** Wurf -> Explosion -> Flaechenschaden laufen im Original vollstaendig. Kaputt ist ausschliesslich
die **Gegnerreaktion** auf die Resolver-Art 2 (Reaktionszeile 9) bei den Familien mit 2D-Tabelle — also
genau "gegen Zombies", wie der Nutzer es beobachtet hat. Fuer diese Reaktion gilt die Beta->Retail-Regel
(RE2: Zeile 9 = Explosiv-Reaktion, Schwester-Dossier `re_gegner_re2_familie.md` §2.1).

## 4. Schadenswert DAT_8006f418[2] / DAT_8006f430[2]

`re15_disasm.py read 0x8006f418 11 --w 2 --signed` -> `[10, 20, 1000, 1000, 1000, 50, 100, 200, 300, 1000, 0]`;
`read 0x8006f430 11 --w 1` -> `[3, 3, 9, 10, 11, 14, 15, 16, 17, 18, 20]`;
`bytes 0x8006f418 36` -> `0a 00 14 00 e8 03 e8 03 e8 03 32 00 64 00 c8 00 / 2c 01 e8 03 00 00 00 00 03 03 09 0a 0b 0e 0f 10 / 11 12 14 00`.

* **DAT_8006f418[2] = 1000** (`e8 03` @0x8006f41c), **DAT_8006f430[2] = 9** (`09` @0x8006f432).
* **Wird im Original angewandt — ohne Absturz:** der Resolver-Zweig laeuft im Explosionsbild vollstaendig
  (Abzug `80012ff4: lhu a0,0(s3)` / `80012ffc: subu v1,v1,a0` / `80013000: sh v1,154(s1)`, Reaktion
  `80012ff0: sb v0,5(s1)`), gemessen: Zombie 81 -> **-919**, Zombie 105 -> **-895**, Gorilla 180 -> **-820**,
  Spieler 100 -> **-900** (je genau -1000), +0x5 = **9** beim Zombie und Gorilla. Der Haenger kommt erst im
  FOLGEBILD aus der Reaktionstabelle des Gegners (§2.7). Der Nutzer-Zweifel "existiert der Schaden?" ist
  damit beantwortet: **er existiert und wird angewandt**; nur die Gegnerreaktion (Zeile 9) fehlt bei den
  2D-Familien. Beim Gorilla (eigener Tod fuer 9/10/11) ist das ganze Ereignis im Original spielbar.
* **Acid/Incendiary (0x0A/0x0B):** Art 3/4 = 1000/10 bzw. 1000/11 (@0x8006f41e/20, @0x8006f433/34) sind
  ohne Aufrufer (Schwester-Dossier `re_schaden_resolver.md` §3: genau zwei `jal 0x80012d60`, Art 0 und 2).
  Im Original bekommt man die Items nur ueber das Item-Debug (§2.2b); ausgeruestet laeuft dann nur die
  Wurfanimation mit Munitionsabzug, **ohne Projektil** (§2.10). Diese Werte werden nie angewandt.

## 5. OFFEN

| # | Offen | Versucht | Naechster Weg |
|---|---|---|---|
| O1 | Literaler Haltepunkt auf `80106c00` (v0 im Moment des Sprungs) statt Rueckschluss aus RA/EPC | Savestate-CPU-Block (RA 0x80106c08, v0 0, a0/v1/a1 passend, EPC 0, k0-Zaehler) — fuer den Befund ausreichend | DuckStation `[Debug] EnableGDBServer` (Port 2345) + GDB-RSP-Client, Z0 auf 0x80106c00; oder PCSX-Redux-Exec-Haltepunkt |
| O2 | Welche Instruktion den Treffer-Riegel +0x93 Bit 0 beim liegenden 0x16 wieder setzt (vb3) | Scan: 28 `ori …,0x1`/`sb …,147(…)`-Paare in STAGE1 (§2.6) | Schreib-Haltepunkt auf 0x800accbf (Zombie 0 +0x93) mit re15-pcsx-watchpoint |
| O3 | NPC-Folgen eines Explosionstods (0x80050ddc, Skript-Zustand 4 verlassen) | nur statisch (§1.5; Schwester-Dossier O2) | Savestate mit NPC im Radius (ROOM1150 Irons), Lauf wie (b) |
| O4 | Efeu 0x2d (STAGE4): Zustandstabelle 0x8011a2c0 hat an [2]/[3] Datenbytes 0xfa060000 / 0x01c20000 | nur statisch (`dis 0x801168c4`, `read 0x8011a2c0 8 --bin STAGE4.BIN`) | pruefen, ob 0x2d je +0x4 = 2/3 bekommt (Riegel/HP); nicht granatenspezifisch |
| O5 | Voll natuerlicher Treffer (auch ohne Gegner-Patch) | 12 Wuerfe in 8 Laeufen (§2.11), 1 knapp verfehlt (996 > 900); Granate selbst natuerlich (§2.2b), Haenger damit + Gegner-Lage-Patch reproduziert (vb6) | Wurf auf einen noch fressenden Zombie aus ~2,3 k Abstand ohne ihn zu wecken (Weckabstand der Briefing-Zombies bestimmen) |
| O6 | Wer das obere Halbwort von RAM 0x8 nach Spielstart auf 0 setzt (0x03400008 -> 0x00000008) | Zensus aller Saves (§2.1b) | Schreib-Haltepunkt auf 0x80000008..b (re15-pcsx-watchpoint) |
| O7 | Tatsaechlich hoerbare Explosions-/Abprall-SEs | nur Aufruf ohne Absturz belegt | SPU-Aufzeichnung (DuckStation Audio-Dump) |
