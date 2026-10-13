# Runde 37 — Spur G "beretta" (2026-10-12)

Baum: `.claude/worktrees/r37_beretta`, Zweig `r37/beretta`, Basis master f1e019ca.

## 0. Auftrag (Wortlaut, AUFTRAG.md Nachtrag 2026-10-12 / VERTRAG.md Spur G)

> Weitere findings - bei der Beretta M93R muss - wenn sie schon mehrfach schießt, das auch gelten das die Gegner
> mehrfach getroffen werden. Werden sie nur ein einziges mal getroffen bei 3 Schuss, ist es nur Schussverschwendung.

Lesart (VERTRAG Spur G): jeder Schuss des 3-Schuss-Feuerstosses trifft (Glock 18 mit demselben Burst-Handler pruefen).

## 1. Messung vorher

### 1.1 Port (exe, Stand f1e019ca, eigener Bau) — `G_beretta_mess/run_mess.sh` + `auswertung.py`
Aufbau wie Runde 36 C "frei": ROOM1140 per `RE15_DEBUG_JUMP=1140@250`, Leon `RE15_PLAYER_POS=200,-18200,1024` (Blick -z),
`RE15_GIVE=<w>:15`, `RE15_EQUIP=<w>`, EIN Abzug: `RE15_INPUT_SCRIPT=M2.0,MA0.1,M3.0,W2` (Square 3 Bilder), Ziel = Platz 5
(Typ 0x11, (200,-19600), d = 1396). Messkopie `re15_r37g.exe`. Logs `mess_r37g/<tag>/` (unversioniert, Zahlen hier).
| Lauf | Waffe | KI | Munition | Treffer (HP-Aenderungen) | Schaden | FX-Kinder (Muendung 2/0) |
|---|---|---|---|---|---|---|
| vorher_w5_re2 | 5 M93R | RE2 | 15 -> 12 (F61) | **1** (F61) | 250 -> 234 (**-16**) | F61, F64, F67 (+0/+3/+6) |
| vorher_w6_re2 | 6 Glock 18 | RE2 | 15 -> 12 | **1** | -16 | F61, F64, F67 |
| vorher_w3_re2 | 3 Browning | RE2 | 15 -> 14 | 1 | -16 | F61 |
| vorher_w5_re15 | 5 M93R | RE1.5 | 15 -> 12 | **1** | -16 (RE2-Zeile per Import-Option) | F61, F64, F67 |
| vorher_w6_re15 | 6 Glock 18 | RE1.5 | 15 -> 12 | **1** | -16 | F61, F64, F67 |
| vorher_w3_re15 | 3 Browning | RE1.5 | 15 -> 14 | 1 | -16 | F61 |
Knaelle (wf.log `SE arms_rec=0`) F62/F65/F68 = je Kind im 2. Takt (Routine 9). Befund bestaetigt: drei Schuesse sichtbar
und hoerbar, drei Patronen weg, EIN Treffer — und dessen Schaden ist gleich EINEM Browning-Schuss (16 = RE2-Zeile 4 =
Zeile 3), d.h. der Feuerstoss schadet je Patrone ein Drittel der Browning.

### 1.2 Original RE1.5 (DuckStation-GDB) — `G_beretta_mess/orig_burst.py`, Logs `G_beretta_mess/orig_w{5,6,3}/log.txt`
MZD-Disc, Spielstand `stage_saves/equip_test.sav` (ROOM1140, Browning angelegt, Leon zielt). Pruefung: `@0x80026e4c = 0800e003`
(Auslieferungs-EXE, nicht die gepatchte), Code im RAM == PSX.EXE fuer 0x800338a8..+0xfc, 0x80033460..+0x100, 0x80017ac8..+0xb8,
0x80011f50..+0x80. Eingabe OHNE virtuelles Pad: Haltepunkt @0x80020c90 (Ruecksprung aus FUN_80030444, `jal` @0x80020c88)
ueberschreibt je Bild 0x800ac768/0x800ac76c (R1 = 0x100 immer, Feuer = 0x40 in Bild 40/41). Waffe per `0x800aca5d := 5/6/3`.
Ziel: Zombie e3 (Typ 0x11, hp 105) per RAM auf (-5281,-17810) = 2000 vor Leon (Blick 1024); Leon selbst laesst sich per RAM
nicht versetzen (die Lage springt nach dem Schreiben zurueck, Lauf 2 im Log), deshalb das Ziel.
| Waffe | Entlade-Handler | Resolver 0x80011f50 | Gewinner / Sichtlinie / Ergebnis | HP e3 | Munition | Routine-15-Kinder (0x80017b38) |
|---|---|---|---|---|---|---|
| 5 M93R | 0x800338a8 B41 | **1x** (B41, a0 = 5) | 0x800ad208 (e3) / v0 0 / v0 1 | 105 -> **90 (-15)** | 15 -> 12 (3x 0x8004eae4 in B41) | 02/03/04 in **B41, B44, B47** |
| 6 Glock 18 | 0x800338a8 B41 | **1x** (a0 = 6) | e3 / 0 / 1 | 105 -> **90 (-15)** | 15 -> 12 | B41, B44, B47 |
| 3 Browning | 0x800337bc B41 | 1x (a0 = 3) | e3 / 0 / 1 | 105 -> 100 (-5) | 15 -> 14 | keine (Sub 0, direkt) |
Nach dem Treffer traegt e3 `+0x93 = 01` und Zustand 2/5 (HURT, Reaktion = Waffen-Id) bis B50; in B51 zurueck 1/2/0 mit
`+0x93 = 00` (RE1.5-Riegel lebt 10 Bilder). => **Das Original trifft je Feuerstoss EINMAL** (Schaden 15 = Spalte 5 der
Zeile 0x11, = 3 x Browning 5), zeigt aber drei Schuesse. Die drei Blitze/Knaelle kommen aus der FX-Kette (2.2).
Vorlaeufe (Leon am Spielstand-Ort, Ziel hinter dem Konferenztisch): die Sichtlinie lehnte den Gewinner 0x800ace20 ab
(`v0 = 1` @0x80012170) — gleicher Zaehlbefund: Resolver 1x, Kinder B21/B24/B27 (Log ueberschrieben, Zahlen hier). Lauf
`orig_w5_b/log.txt`: Leon per RAM auf (200,-18200) gehalten — sobald das Schreiben endet (B31), springt die Lage auf
(-5281,-15810) zurueck; deshalb wurde im Endlauf das ZIEL versetzt, nicht Leon.

## 2. RE-Belege (selbst disassembliert; `re15_disasm.py` / `re2_disasm.py`, CORE00.ESP selbst gedumpt)

### 2.1 RE1.5: der Feuerstoss ist EINE Treffer-Aufloesung (statisch)
Entlade-Handler der Waffen 5/6 = 0x800338A8 (Tabelle @0x80074100 + id*4 = @0x80074114/18):
```
800338ac lui a0,0x202 / ori a0,a0,0x800  ; 0x02020800 Muendung SUB 2   -> jal 0x80019700 @0x800338ec
800338f4 lui a0,0x302 / ori a0,a0,0xc00  ; 0x03020c00 Rauch    SUB 2   -> jal 0x80019700 @0x80033920
80033928 lui a0,0x402 / ori a0,a0,0x800  ; 0x04020800 Huelse   SUB 2   -> jal 0x80019700 @0x80033950
8003395c lw v0,g_entity / 80033964 lbu a0,0x800aca5d / 80033968 lw a1,124(v0)
8003396c jal 0x80011f50                  ; EIN Resolve (Schuss-Resolver, Schaden, Reaktion)
80033974 jal 0x8004eae4                  ; Munition -1
8003397c jal 0x8004eae4                  ; Munition -1
80033984 jal 0x8004eae4                  ; Munition -1   (Rueckgaben ungeprueft)
8003399c jr ra
```
Der Handler laeuft EINMAL je Feuerstoss: Standard-Sub-2 0x80033460, Latch `lbu v0,0x800aca5b` / `bne v0,zero,0x80033508`
@0x80033470-78, `sb v0(=1)` @0x80033480, `jalr v0` @0x800334e0 (Eintrittsbild). Weitere Resolves im Sub 2 NUR fuer
Waffe 8: `ori v0,zero,0x8` / `bne v1,v0,0x8003355c` @0x80033510-14, acae9 == 3/5/7 @0x80033518-38, `jal 0x80011f50`
@0x80033554. => RE1.5 (Auslieferungsstand) trifft je Feuerstoss EINMAL und zieht 3 Patronen ab.

### 2.2 RE1.5: die DREI Schuesse stehen in den Daten (CORE00.ESP, eigener Dump `G_beretta_mess/esp_dump.py`)
Die Sub-2-Effekte sind je EIN Strom aus drei Zeilen Routine 15 (= "unsichtbar warten, dann Kind spawnen"):
| Effekt | Zeile @Datei | A | Warte +0x0E | Kind (+0x17,+0x16) | +0x26 |
|---|---|---|---|---|---|
| Muendung 2/2 (rowblk 0xFC8, sub-Basis 0x1180) | 0x1188 / 0x11B0 / 0x11D8 | 15 | 0 / 2 / 2 | 2/0 (= Einzel-Muendung mit Knall-Routine 9) | 1 / 1 / 0 |
| Rauch 3/2 (rowblk 0x384, Basis 0x444) | 0x44C / 0x474 / 0x49C | 15 | 0 / 2 / 2 | 3/0 | 1 / 1 / 0 |
| Huelse 4/2 (rowblk 0x18C0, Basis 0x1980) | 0x1988 / 0x19B0 / 0x19D8 | 15 | 0 / 2 / 2 | 4/0 | 1 / 1 / 0 |
Routine 15 @0x80017ac8: `lhu v0,14(slot)` / `beq v0,zero,0x80017b10` @0x80017aec-f4, sonst `addiu v0,v0,-1` /
`sh v0,14(a3)` + RETURN @0x80017b04-0c; Kind `jal 0x800199d4` @0x80017b38 (Code = +0x17<<24 | +0x16<<16 | Skala +0x72);
`lhu v0,38(v1)` (+0x26) != 0 -> `jal 0x800174e4` (naechste Zeile) @0x80017b4c-5c, sonst `sb zero,108(v1)` @0x80017b6c.
=> Kinder in den Takten T, T+3, T+6 (Warte 0, dann 2+1, dann 2+1): DREI Muendungsblitze, drei Knaelle (Routine 9 der
Kinder 2/0), drei Rauchwolken, drei Huelsen im 3-Bild-Abstand — aber nur EIN Treffer (2.1). Genau der Nutzerbefund.

### 2.3 RE1.5-Schadenstabelle: die Spalte 5/6 ist der Schaden des GANZEN Feuerstosses
`read 0x8006e0d0 22 --w 2 --stride 4 --rows 0x30 --rowstride 0x58` (u16 @ 0x8006e0d0 + Typ*0x58 + Waffe*4):
Zombie-Zeilen 0x10..0x14/0x16..0x1A: Browning (3) 5 / SIG (4) 5 / M93R (5) 15 / Glock (6) 15 (= 3 x 5);
0x15: 10/10/24/24; 0x20: 10/10/24/24; 0x23: 7/7/21/21; 0x25: 10/10/24/24; 0x27: 12/12/28/28; 0x28: 6/6/18/18;
0x29: 14/14/25/25; 0x2A: 10/10/20/20; 0x2B: 10/10/20/20. Ein Resolve mit dem 2..3-fachen Pistolenschaden.

### 2.4 RE2 Retail (Beta -> Retail): die Custom Handgun (RE2-Item 4) trifft JE KUGEL
Waffen-FSM-Tabelle @0x800A6F38 + (+0x10E & 0xfff)*4 (`lw v0,28472(at)` @0x80043e40, `jalr` @0x80043e48):
Item 2/3 (Handgun) -> 0x80043230 (Standard), **Item 4 (Custom Handgun) -> 0x8004879C (eigene FSM)**, Substate-Tabelle
@0x800A7098 (`lw v0,28824(at)` @0x800487b8): Sub 2 = 0x800487D8 (Feuerstoss). Die Entlade-Tabelle @0x800A6F90[4] =
0x80043f24 ist ein leeres `jr ra` (keine Sammelabbuchung). Sub 2, JEDES Bild:
```
800488e4..80048904  Bit 126 von 0x800d4854 (FUN_80077360 = Bit-Test) && Bild==3 -> Bild=10 (Einzelschuss-Modus,
                    dieselbe Bit-Abfrage zeigt im Inventar @0x8006abe8-ac1c die Item-4-Anzeige um)
80048908 lbu v1,333(s0)              ; +0x14D Bildzaehler des Feuerclips (Eintritt: sw clip+0x70000,332 @0x80048870)
80048910 beq v1,zero,0x80048958      ; Bild 0 -> KUGEL
80048918 beq v1,v0(=3),0x80048928    ; Bild 3 -> Pruefung
80048920 bne v1,v0(=6),0x800489c4    ; Bild 6 -> Pruefung, sonst nur Resolver
80048928 andi v0,s2,0x40 / beq -> Bild=10   ; Abzug losgelassen -> Feuerstoss endet
80048938 jal 0x80069f54 / bne v0,zero,0x80048958  ; Munition da? -> KUGEL, sonst Bild=10
80048958 KUGEL: jal 0x80041098 (Treffer-Skript neu, +0x1ED=0 / +0x1EC=Zaehler) @0x80048990
                jal 0x8006a0cc(Item,0)  ; EINE Patrone @0x800489a0
                jal 0x80040350          ; Effekte @0x800489b0
                jal 0x80043d30          ; Knall @0x800489b8
800489c4..80048a04  jal 0x800410cc       ; Resolver JEDES Bild, vom Treffer-Skript gegatet
```
Der Bildzaehler +0x14D steigt je Bild um genau 1: Sub 2 ruft am Ende `jal 0x8002959c` (@0x80048aa0) -> FUN_80029614
`lbu v0,333(s2)` / `addiu v0,v0,1` / `sb v0,333(s2)` @0x80029b28-34 (Clip-Ende: `sb zero,333(s2)` @0x80029b48).
Im Eintrittsbild steht er auf 0 (`sw v0,332(s0)` mit Clip + 0x70000 @0x80048860-70 -> +0x14D = 0), d.h. Kugel 1 im
Eintrittsbild, Kugel 2/3 drei bzw. sechs Bilder spaeter.
Treffer-Skript (Paar @0x800A68E8 + Item*24 + Gruppe*8; Item 4 = `18 66 0a 80 6c 66 0a 80` @0x800A6948): Boxen
0x800A6618/34/50 (Handgun-Familie), Muster @0x800A666C = `00 01 00 ff` = {Record 0, 1 Bild}, {Ende}. Der Resolver
FUN_800410CC loest also genau EIN Bild nach jedem `jal 0x80041098` auf (Zaehler +0x1EC: -1 = 0xFF -> kein Test,
`*(char*)(p+0x1ec) != -1`; `+0x1ec -= 1` nach dem Kandidatenlauf). => RE2: bis zu DREI Treffer je Feuerstoss, in den
Bildern 0/3/6 des Feuerclips — derselbe 3-Bild-Abstand wie die RE1.5-Effektkette (2.2).

Schadenszeilen RE2 (Zeile = 0x800A6A88[Typ] + (Id-1)*20, w0 = Schaden je Klammer 10 Bit, w1 >> (k*7+9) & 0x7f =
Trefferpause): in 20 von 22 Tabellen ist die Custom-Handgun-Zeile 4 im Schaden BITGLEICH zur Handgun-Zeile 3
(Zombie @0x800A4168 w0 = 0x00E03C10 = @0x800A4154: 16/15/14), nur die Trefferpause ist kuerzer
(Zombie 0x800A412C: Zeile 3 Pause 5, Zeile 4 Pause **1** — w1 0x02850A14 vs 0x00810214). Eine Kugel des Feuerstosses
schadet in RE2 wie ein Handgun-Schuss, und die kurze Pause laesst die naechste Kugel (3 Bilder spaeter) wieder treffen.

## 3. Umsetzung

### 3.1 Entscheidung (NUTZER-VORGABE, Form aus RE2)
Das Original (RE1.5) trifft je Feuerstoss einmal (2.1, 1.2) — der Wunsch des Nutzers ist also eine NUTZER-VORGABE.
Ihre FORM kommt aus RE2 Retail (Beta -> Retail), wo der Feuerstoss der Custom Handgun genau so gebaut ist (2.4):
* **Zeitpunkte**: Kugel 1 im Eintrittsbild (wie bisher @0x8003396c), Kugel 2/3 im Bild +3/+6 — RE2 Bild 0/3/6
  (@0x80048910-20) und die RE1.5-FX-Kette (Warte 0/2/2, Kinder B41/B44/B47 am Original, F61/F64/F67 im Port) liegen auf
  denselben Bildern; Treffer, Blitz und Huelse fallen damit zusammen, der Knall folgt im naechsten Bild (Routine 9).
* **Resolve je Kugel**: derselbe Resolver re15_player_weapon_fire(5|6) (Sichtlinie @0x80012168, Gegnerwahl, Schaden,
  Reaktion, Blut, RE2-Trefferpause) — RE2 ruft je Kugel ebenso denselben Resolver FUN_800410CC nach dem Skript-Neustart.
* **Patronen**: unveraendert drei im Eintrittsbild (`jal 0x8004eae4` x3 @0x80033974/7c/84, byte-true); Kugel k nur, wenn
  ihre Patrone abgebucht wurde (RE2: `jal 0x80069f54` @0x80048938) — 1 Patrone im Magazin = 1 Treffer.
* **RE1.5-Riegel** +0x93 Bit 0 (`ori v0,v0,0x1` @0x800124F0): haelt im RE1.5-Modell jeden weiteren Treffer bis zum Ende der
  Trefferreaktion ab (am Original: B41..B50, Freigabe B51). Fuer Gegner ohne RE2-Trefferpause loest Kugel 2/3 den Riegel,
  den DIESER Feuerstoss gesetzt hat (Bits 0|1), solange der Gegner lebt (HP >= 0); Gegner mit RE2-Pause (+0x1D3) behalten
  die RE2-Regel (Zeile 4 = 1 Bild beim Zombie, 0 beim Hund/Spinne, 15 bei der Kraehe — RE2 Retail).
* **Schaden je Kugel = Spalte 3** (Einzelschuss-Pistole): RE2 Zeile 4 == Zeile 3 im Schaden (20/22 Tabellen, 2.4). Die
  RE1.5-Spalte 5/6 ist der Schaden des GANZEN Stosses (Zombie 15 = 3 x 5, 2.3) — mit drei Treffern a Spalte 5 waere der
  Stoss dreifach so stark wie im Original. Mit Spalte 3 ist die RE1.5-Zombiezeile exakt der Original-Stoss (3 x 5 = 15);
  die RE2-Zeilen des Ports (Zombie/Hund/Kraehe/Spinne) haben ohnehin Spalte 5 == Spalte 3 (16/11/18/15/17).
* **Ende**: der Feuerstoss endet mit dem Rueckstoss (re15_player_aim_interrupt bei Treffer/Griff, re15_player_aim_reset beim
  Raumwechsel) oder einem Waffenwechsel — RE2: Verlassen von Sub 2 beendet ihn ebenso.
* NICHT uebernommen aus RE2: Loslassen des Abzugs bricht ab (@0x80048928-30) und Einzelschuss per Bit 126 (@0x800488e4) —
  RE1.5 bucht alle drei Patronen sofort ab und zeigt immer drei Blitze; beides bleibt RE1.5.
* RE3 als weiteres Vorbild: im Repo liegt kein RE3 (info/ traegt nur Re1.5, re2leon und die Referenzsammlung) — RE2 Retail
  ist der naechste Nachfolger derselben Engine und hat genau diese Waffenklasse (Item 4).

### 3.2 Dateien
* NEU `re15_port/engine/src/beretta_r37.c`, `re15_port/include/re15_beretta_r37.h` (Belege im Kopf): `re15_beretta_r37_feuerstoss`
  (Eintrittsbild), `re15_beretta_r37_tick` / `_tick_ex` (je Bild, Kugel 2/3 im Bild 3/6), Riegel-Loesen, Mess-Zaehler.
  Konstanten: `BERETTA_KUGEL_ABSTAND 3` (@0x80048914/1c, CORE00.ESP +0x0E = 2 @0x11B0/0x11D8 + Routine 15 @0x80017aec-b38),
  `BERETTA_KUGELN 3` (@0x80033974/7c/84, drei Routine-15-Zeilen @0x1188/0x11B0/0x11D8); Waffen 5/6 (@0x80074114/18).
* Haken `re15_port/engine/src/game_step_common.c` (Kommentar "Runde 37 Spur G"): Include; je Bild
  `if (c->rdt_ok) (void)re15_beretta_r37_tick();` vor dem R1-Feuerblock; im Entlade-Block Rueckgabe des Resolvers und Zahl der
  abgebuchten Patronen merken, danach `re15_beretta_r37_feuerstoss(eq_item, r37_patronen, r37_treffer1)`.
* Haken `re15_port/engine/src/re15_damage.c` hinter der Python-Zeile: `if (weapon_id == 5 || weapon_id == 6) dmg = dmg_row[3];`.
* `re15_port/tests/unit/test_re2_hp_model.c`: Erwartungs-Index R35_W fuer 5/6 auf Spalte 3 (der Test pinnte den alten
  "ein Resolve = ganzer Stoss"-Schaden; Begruendung im Kommentar).
* Keine neuen Assets, keine Sprachdateien, kein Flag/Bank-9-Bit.

## 4. Messung nachher (exe, eigener Bau, Code-Stand = Commit 3f17b7db; dieselben Laeufe wie 1.1, `mess_r37g/nachher_*`)
| Lauf | Munition | Treffer (Bild, dHP) | HP | Blut id=0 (Alter 1) | FX-Kinder |
|---|---|---|---|---|---|
| nachher_w5_re2 (Leon, RE2) | 15 -> 12 (F61) | F61 -16, F64 -16, F67 -16 | 250 -> 202 | F61, F64, F67 | F61/F64/F67 |
| nachher_w6_re2 (Glock) | 15 -> 12 | F61, F64, F67 je -16 | 250 -> 202 | 3 | F61/F64/F67 |
| nachher_w5_re15 (RE1.5-KI) | 15 -> 12 | F61, F64, F67 je -16 (je Treffer st=2 HURT, ss1=5) | 250 -> 202 | F61, F64, F67 | F61/F64/F67 |
| nachher_w6_re15 | 15 -> 12 | F61, F64, F67 je -16 | 250 -> 202 | 3 | |
| nachher_w3_re2 (Gegenprobe Browning) | 15 -> 14 | F61 -16 (einer) | 250 -> 234 | F61 | F61 |
| nachher_w3_re15 (Gegenprobe) | 15 -> 14 | F61 -16 (einer) | 250 -> 234 | F61 | F61 |
| nachher_w5_re2_elza (Elza PL04, ROOM11E1, Platz 1 Typ 0x10) | 15 -> 12 (F91) | F91, F94, F97 je -16 | 58 -> 10 | | F91/F94/F97 |
| nachher_w6_re2_elza | 15 -> 12 | F91, F94, F97 je -16 | 58 -> 10 | | |
| nachher_w5_re15_elza (RE1.5-KI) | 15 -> 12 | F91 Platz 1, F94 Platz 2, F97 Platz 3 je -16 | | | |
| nachher_w3_re2_elza (Gegenprobe) | 15 -> 14 | F91 -16 (einer) | 58 -> 42 | | |
Elza-Start wie test_r35_werfer_elza (RE15_TITLE_CONFIRM_MS=1500, RE15_PSELECT_AUTO=1, RE15_PSELECT_AUTO_SWITCH=1); der
Debug-Sprung 11E0 landet in ihrer Variante ROOM11E1 (debug.log `[pl] Spieler-Familie PL04`, `loaded room11e1.rdt`).
ROOM1141 (Elzas Besprechungsraum) hat keine Gegner — Lauf dort ohne Ziel verworfen.
RE1.5-KI in ROOM11E1: der getroffene Zombie faellt (ss1 0x11, Clip 11, `g = 0x80` = liegend) und ist mit EBEN-Zielen kein
Kandidat (RE1.5-Band: liegend -> nur TIEF und nur unter 5000, `eband = 0x20000000` @0x80101630-38 -> @0x800129cc-f0, eben
-> LEVEL @0x801015f4-fc; re15_damage.c) -> Kugel 2/3 gehen ueber den Rueckgriff an den naechsten Zombie (je ein Treffer,
je -16). Das ist die RE1.5-Kandidatenregel, keine Luecke: jede Kugel trifft.

## 5. Tests (registriert NUR in `re15_port/tests/unit/probes/r37_beretta.cmake`)
* `unit_r37_beretta` (`tests/unit/test_r37_beretta.c`, echte ROOM1140.RDT, Resolver re15_player_weapon_fire): A Kugel 2/3
  in Tick 3/6 (Ticks 1/2/4/5/7/8 nichts); B drei Treffer auf DENSELBEN Zombie unter RE1.5-KI mit Negativkontrolle (ohne
  Riegel-Loesen trifft ein direkter zweiter Resolve NICHT: r=0), Zaehler 1/3/3/2, Reaktion state 2 / +0x5 = 5;
  C Schaden je Kugel: RE2-Import-Zeile 16, RE1.5-Zeile 5 (3 Kugeln = 15 = Original-Stoss), Hund (RE1.5-Zeile) 10 statt 24,
  Glock 18 wie M93R; D 1/2/3 Patronen -> 1/2/3 Kugeln; E Abbruch (tick_ex(0)) -> keine Kugel mehr; F Sterbender (HP<0) wird
  nicht erneut getroffen, Kugel 2/3 gehen an den Zombie dahinter (Kontrolle: dieser ist allein treffbar);
  G Browning 3 / Redhawk 7 / Ingram 12 / MC51 19 starten keinen Feuerstoss. Ergebnis `test_r37_beretta: OK`.
* `integration_r37_beretta` (`tests/integration/test_r37_beretta.cmake`, echte exe als Messkopie `re15_pc_r37g_haken_*`):
  leon_re2_w5 / leon_re2_w6 / leon_re15_w5: Munition -3 im Eintrittsbild E, HP-Abzuege GENAU in E, E+3, E+6;
  leon_re2_w3 (Gegenprobe Browning): ein Abzug in E, Munition -1; elza_re2_w5 (PL04, ROOM11E1): drei Abzuege E/E+3/E+6.
  Lauf 2026-10-12: `Passed 157.23 sec`; Werte (lauf_48823eac): Leon F61/F64/F67 je -16 (250 -> 202), Elza F91/F94/F97 je -16
  (58 -> 10), Browning F61 -16, Munition 15 -> 12 / 14. Am Stand f1e019ca liefert derselbe Aufbau EINEN Abzug (1.1) — der
  Riegel waere dort rot.
* Angepasst: `unit_re2_hp_model` (Erwartungs-Index 5/6 -> Spalte 3, s. 3.2) — gruen.

### 5.1 Gezielte Laeufe (keine volle Suite im Spurbaum)
Regex A (eigene Tests + alle Schuss-/Schadens-/Waffen-Gruppen, die game_step_common.c / re15_damage.c beruehren), Stand
nach dem letzten Bau (Code wie Commit-Kopf):
`ctest --test-dir re15_port/build -R "^(unit_r37_beretta|integration_r37_beretta|unit_r36_schuss|unit_r36_schuss_kraehe|unit_r36_elza_schuss|unit_re2_hp_model|unit_re2_weapon_rows|unit_re2_baby_spider_dmg|m93r_nachladen|unit_enemy_shootable|unit_room1140_combat|unit_damage|unit_weapon_select|unit_hit_reaction|unit_re2_hit_reaction|unit_re2_hit_repeat|unit_re2_hit_repeat_crow_spider|unit_r17_munition_pin|unit_r17_waffen_loop_pin|unit_autofire_pin|unit_zombie_hit_fixes|unit_re2_zombie_hitfilter|unit_r16_trefferhoehe_pin|unit_r16_aufstehen_schuss|unit_schrot_trennung|unit_r36_blut_pin_.*|unit_dog_hit|unit_re2_dog_hitwindow|unit_writher_hit_feedback|unit_aim_all_around|unit_aim_lower_exit|unit_r35_werfer|unit_r35_werfer_kombi|unit_r36_granate|unit_r35_granate|unit_r30_granate|unit_dog_aim_band|unit_re2z_rise_hittable|unit_sca_wall_hit|unit_r35_affen_schrot)$" --timeout 600`
-> **`100% tests passed, 0 tests failed out of 48`** (Total 107.80 sec).
Regex B (exe-Laeufe der beruehrten Schusspfade, Runde-36-C-Riegel inklusive; Code-Stand identisch, danach nur ein
Kommentar im Header geaendert): `ctest --test-dir re15_port/build -R "^(integration_r37_beretta|integration_r36_schuss|integration_r36_elza_schuss|integration_r35_werfer|integration_r36_granate)$" --timeout 900`
-> **`100% tests passed, 0 tests failed out of 5`** (r35_werfer 175.89 s, r36_elza_schuss 58.04 s, r36_granate 118.62 s,
r36_schuss 181.04 s, r37_beretta 117.60 s).
Auswertung aller Messlaeufe (vorher/nachher, Leon/Elza): `G_beretta_mess/auswertung_laeufe.txt`.

## 6. OFFEN
* Nichts aus dem Wortlaut. Bewusst NICHT gebaut (RE1.5 bleibt): Abbruch des Feuerstosses beim Loslassen des Abzugs (RE2
  @0x80048928-30) und RE2s Einzelschuss-Bit 126 (@0x800488e4) — RE1.5 bucht immer drei Patronen ab und zeigt immer drei
  Blitze.
* Elza "vorher" nicht eigens an der alten exe gemessen (der Entlade-Handler 0x800338A8 verzweigt nicht nach Figur, kein
  `aca5c`-Zweig @0x800338a8-9c; Leon vorher = 1 Treffer, 1.1).

## 7. Fuer den Nutzer
* Beretta M93R und Glock 18: jeder der drei Schuesse des Feuerstosses trifft jetzt einzeln — Schaden, Treffer-Reaktion und
  Blut je Kugel, im Takt der drei Muendungsblitze. Ein Feuerstoss auf einen Zombie macht jetzt 3 x 16 = 48 statt 16.
* Das Original (RE1.5-Prototyp) trifft je Feuerstoss nur einmal — am Original gemessen. Die Umsetzung folgt RE2 (Custom
  Handgun: je Kugel ein Treffer, Schaden je Kugel = Pistolenschuss).
* Mit RE2-KI (= Standard) gilt RE2s Trefferpause nur fuer die Gegner, die der Port mit RE2-Gehirn UND RE2-Pausenfilter fuehrt
  (RE2-Zeile 4): Zombies 1 Bild (gemessen: alle drei Kugeln treffen), Hund und Spinne 0 Bilder (@0x800A4460/@0x800A4BCC),
  KRAEHE 15 Bilder (@0x800A45DC) — eine Kraehe nimmt je Feuerstoss nur die erste Kugel und stirbt daran (10 HP, 15 Schaden).
  Alle anderen Gegner (Gorilla, Kakerlake, Alligator, Tyrant 0x2b, Birkin) laufen auch unter RE2-KI auf ihrem RE1.5-Gehirn
  und nehmen seit Nachbesserung 1 jede Kugel (gemessen 0x2b ROOM5040, Kakerlake ROOM3040, Birkin ROOM5080, Abschnitt 8.4).
  Mit RE1.5-KI trifft jede Kugel; ein getroffener Zombie, der zu Boden geht, ist mit geradem Zielen kein Ziel mehr, dann
  trifft die naechste Kugel den naechsten.
* Schaden je Kugel = ein Pistolenschuss (Browning/SIG-Spalte), wie RE2 es fuer die Custom Handgun festlegt. Gegen Zombies
  (RE2-Schadenszeile, beide KI-Modi) sind das 16 je Kugel. Gegen Gegner mit RE1.5-Schadenszeile (Gorilla, Kakerlake,
  Alligator, Tyrant, Birkin; Hund/Kraehe/Spinne nur unter RE1.5-KI) ist der Stoss damit 3 x Browning — in BEIDEN KI-Modi
  (bis Nachbesserung 1 war es unter der Standard-KI nur 1 x Browning, Abnahme 0 M1/M2). Die RE1.5-Tabelle fuehrte fuer
  M93R/Glock den Wert des GANZEN Stosses; der neue Stoss ist nie schwaecher als dieser alte Einzeltreffer: Kakerlake
  42 statt 25, Tyrant 0x2b 30 statt 20, Gorilla 36 statt 28, Birkin 45 statt 28, Alligator 21 = 21 (Spalte 3 / Spalte 5
  @0x8006e0d0 + Typ*0x58 + 3*4 / + 5*4).
* Ist nur noch 1 oder 2 Schuss im Magazin, gibt es nur 1 bzw. 2 Treffer (drei Blitze zeigt das Original trotzdem).
* Keine neuen Assets, keine Sprachdateien, keine Bedienaenderung.

## 8. Nachbesserung 1 (Abnahme 0: M1-M4, 2026-10-13)

Abnahmebericht: `G_abnahme_0.md`. Zwei fruehere Versuche wurden von Limits ohne Commit beendet; ab hier fortlaufend.

### 8.0 Zensus der betroffenen Klasse (Werkzeug geprueft)
`G_beretta_mess/typ_zensus.py <baum>` (Walker `re15_port/tools/aot_sce_census.py`, Sammler Sce_em_set 0x44, Typ = pc[2])
laeuft (844 Saetze) und deckt sich mit Abnahme 1f: 0x27 in 1141/11C0 (8), 0x29 in 15 Raeumen (32), 0x2b in 9 Raeumen
(4001/5040/5041/5070/5071/5110/5111/5130/5131, 28), 0x30 in 8 Raeumen (3070/3071/5080/5081/5090/5091/50E0/50F1; 5090/5091 werden
zum G5-Modul 0x36 umgetypt), 0x36 in 3080, 0x1a in 1210/1211; 0x23 (Alligator) hat keinen Sce_em_set-Satz (Boss-Modul 2090).

### 8.1 Messung vorher (Spur-Stand 6caac126 = Code 4a5571df, eigener Bau `LOCAL-BUILD-OK (build)`, Standard-KI ohne RE15_AI_FLAVOR)
Skript `G_beretta_mess/run_nb1.sh` (= Abnahme-Aufbau Anhang B, Messkopie `re15_r37g_nb1.exe`), Auswertung `auswertung.py`.
| Lauf | Raum / Ziel | Munition | Treffer | Blitze (wf SPAWN 2/0) |
|---|---|---|---|---|
| nb1v_t2b_def | ROOM5040 Platz 1 Typ 0x2b hp 121, Leon (-14493,-13185) Blick -z | F61 15 -> 12 | **F61 -10 (EINER)**, danach HURT 2/5 bis F70 | F61/F64/F67 |
| nb1v_roach_def | ROOM3040 Platz 2 Typ 0x29 hp 99, Leon (-13500,-10300) Blick -z | F61 15 -> 12 | **F61 -14 (EINER)**, HURT 2/5 bis F73 | F61/F64/F67 |
| nb1v_gor_def | ROOM11C0, Gorillas 0x27 (Platz 2/3, hp 180, ruhend) | 15 (kein Schuss) | — Leon zielt in 11C0 nach dem Sprung nicht (pst 0, pm 2 bis F200) | — |
=> Abnahme M1/M2 reproduziert (0x2b -10, 0x29 -14 = je EINE Kugel Spalte 3, Basis-exe laut Abnahme -20 / -25).
Gorilla an der exe nicht per Sprung messbar (11C0 haelt den Spieler fest); 0x27/0x30/0x23 deshalb in der Sonde (8.4).

### 8.2 RE-Belege (selbst gelesen)
* RE1.5-Riegel: der Resolver setzt +0x93 Bit 0 (`ori v0,v0,0x1` @0x800124F0, Abnahme 2 Nr. 3 bestaetigt); freigegeben wird
  er erst am ENDE der Trefferreaktion des RE1.5-Gehirns — Kakerlake `sb zero,147(a0)` STAGE3.BIN @0x801149ac (selbst
  disassembliert, nach Phase 0 `ori v0,v0,0x2` / `sb v0,147(a0)` @0x801148a0-a4). Diese Kette laeuft fuer 0x23/0x27/0x29/0x2b/
  0x30 in JEDEM KI-Modus: der Port hat fuer sie kein RE2-Gehirn (RE2-Gehirne nur Zombie-Familie, 0x20, 0x21, 0x25/0x26,
  0x1a; G5 0x36 in 5090/5091 = eigenes Modul).
* RE2-Pause: nur dort, wo ein Filter +0x93 Bit 0 nach +0x1D3 freigibt — game_step_common.c (Bild-Nachlauf):
  `re15_ai_re2_for_type(t)` und dann `re15_re2z_owns_type(t)` -> re15_re2z_hit_filter_apply, sonst
  re15_re2_pause_filter_apply, das selbst nur 0x20/0x21/0x25/0x26 annimmt (re15_damage.c, Typ-Tor in der ersten Zeile).
  Der Treffer stempelt +0x1D3 nur mit einer RE2-Zeile des Ports: re15_re2_stun_frames kennt 0x10-0x13/0x16/0x18/0x20/0x21/
  0x25/0x26, sonst `return 0` — fuer 0x23/0x27/0x29/0x2b/0x30 gibt es weder Stempel noch Filter.
* RE2 Retail (Beta -> Retail, `G_beretta_mess/re2_zeilen_nb1.py info/re2leon/PSX.EXE`): Zeile 4 (Custom Handgun) je Typ-Nummer
  @0x800A6A88[t] + 3*0x14: 0x10 Pause 1, 0x20 0, 0x21 15, 0x23 2, 0x25 0, 0x27 5, 0x29 15, 0x2b 0, 0x30 5, 0x36 5. Die
  RE2-Typ-Nummern 0x23..0x30 sind RE2-Kreaturen; der Port fuehrt fuer die RE1.5-Typen gleicher Nummer weder diese Zeilen noch
  ihre Pause (re15_enemy_dmg_row = RE1.5-Zeile @0x8006e0d0, Abnahme 1c). Fuer diese Gegner gilt also die RE1.5-Lage — und
  der Nutzer-Wortlaut ("die Gegner mehrfach getroffen") verlangt dort drei Treffer.
=> Ursache M1: das Kriterium in riegel_loesen() war der FLAVOR (`re15_ai_re2_for_type`), nicht "fuer diesen Aktor gibt ein
RE2-Pausenfilter den Riegel frei". Im RE2-Flavor fiel damit jeder RE1.5-Gehirn-Gegner durch beide Raster.

### 8.3 Umsetzung (Commit "NB1 M1")
* `engine/src/beretta_r37.c`: neues Kriterium `re2_pausenfilter_laeuft(t)` = `re15_ai_re2_for_type(t)` UND
  (`re15_re2z_owns_type(t)` ODER t in {0x20, 0x21, 0x25, 0x26}) — der Spiegel der Weiche im Bild-Nachlauf von game_step
  (game_step_common.c, Block "MIXED (2026-08-23)") und des Typ-Tors von re15_re2_pause_filter_apply (re15_damage.c).
  riegel_loesen() ueberspringt nur noch diese Aktoren; alle anderen behandelt der Feuerstoss in JEDEM KI-Modus wie unter
  RE1.5-KI (Riegel +0x93 Bits 0|1 `ori 0x1` @0x800124F0 / `|= 2` @0x8001240c vor Kugel 2/3 loesen, solange HP >= 0).
  Exportiert fuer Tests: `re15_beretta_r37_re2_pause_regelt(type)`.
* Keine neue Konstante; die Typ-Liste ist die vorhandene Filter-Weiche des Ports, die Pausen der RE2-Zeile 4 stehen als
  Beleg im Kommentar (@0x800A4168 Zombie 1, @0x800A4460 Hund 0, @0x800A4BCC Spinne 0, @0x800A45DC Kraehe 15).
* Unveraendert: Zombies unter RE2-KI (RE2-Filter, Pause 1), Hund/Kraehe/Spinne unter RE2-KI (RE2-Pause), alles unter
  RE1.5-KI und MIXED (dort ist nur 0x20 RE2 — wie vorher), Kugel-Takt, Patronen, Abbruch, Schaden je Kugel (Spalte 3).
  **Richtigstellung (Nachbesserung 2, Abnahme 1 N1):** "alles unter RE1.5-KI und MIXED" war fuer den Gitterarm 0x1A FALSCH —
  er hat in JEDEM Modus das RE2-Gehirn (`re15_ai_re2_for_type(0x1A)` = 1, enemy_ai_re2_zombie.c:172), aber keinen Filter;
  das NB1-Kriterium loeste deshalb seinen Riegel in jedem Modus (drei Treffer, Rueckzug dreimal neu). Behoben in Abschnitt 9.
* M2 braucht keine eigene Aenderung: Spalte 3 je Kugel war richtig, falsch war, dass Kugel 2/3 nicht trafen. Mit drei
  Treffern ist der Stoss 3 x Spalte 3 >= der alte Einzeltreffer Spalte 5 fuer jede betroffene Zeile (Alligator gleich).
* M4: `include/re15_beretta_r37.h` (PORT-ABBILDUNG, Riegel-Absatz) und der Kommentar an riegel_loesen() sagen jetzt
  "massgeblich ist der FILTER, nicht der KI-Modus"; Dossier §7 richtiggestellt (3 x Browning in BEIDEN KI-Modi erst seit
  Nachbesserung 1, Vergleichswerte je Gegner).

### 8.4 Messung nachher (exe nach "NB1 M1", Standard-KI ohne RE15_AI_FLAVOR, gleicher Aufbau wie 8.1)
| Lauf | Ziel | Munition | Treffer (Bild, dHP) | Stoss | vorher (8.1 / Abnahme-Basis) |
|---|---|---|---|---|---|
| nb1n_t2b_def (M93R) | ROOM5040 Platz 1 Typ 0x2b | F61 15 -> 12 | F61 -10, F64 -10, F67 -10 (je st 2/5) | **30** | Spur 10, Basis 20 |
| nb1n_t2b_w6_def (Glock 18) | dito | F61 15 -> 12 | F61/F64/F67 je -10 (st 2/6) | **30** | — |
| nb1n_roach_def (M93R) | ROOM3040 Platz 2 Typ 0x29 | F61 15 -> 12 | F61 -14, F64 -14, F67 -14 | **42** | Spur 14, Basis 25 |
| nb1n_roach_w3_def (Browning, Gegenprobe) | dito | F61 15 -> 14 | F61 -14 (einer) | 14 | unveraendert |
| nb1n_birk_def (M93R) | ROOM5080 Platz 1 Typ 0x30 (hp 300), Leon (-18100,1700) Blick -z | F61 15 -> 12 | F61 -15, F64 -15, F67 -15 | **45** | Tabelle: Basis 28 (1 x Spalte 5) |
Blitze je Lauf F61/F64/F67 (wf SPAWN 2/0), Reaktion je Treffer (Zustand 2, +0x5 = Waffe). Ein Blut-/FX-Protokoll schreibt die
exe in 5040/3040/5080 weder vorher noch nachher (fx.log = gezeichnete Partikel; das RE1.5-Blut dieser Gehirne ist im Port
nicht gezeichnet, z.B. Tyrant "blood FX 0x80019700 @0x80114998 — render-side, OPEN") — keine Aenderung dieser Spur.

### 8.5 Je Mangel (Ursache / vorher / Beleg / Aenderung / nachher)
| Mangel | Ursache | Messung vorher | Beleg | Aenderung | Messung nachher |
|---|---|---|---|---|---|
| M1 Standard-KI, RE1.5-Gehirn 1 Treffer | riegel_loesen() sprang ueber jeden Aktor mit `re15_ai_re2_for_type` — im RE2-Flavor JEDER Typ —, ein RE2-Pausenfilter laeuft aber nur fuer Zombie-Familie/0x20/0x21/0x25/0x26; 0x23/0x27/0x29/0x2b/0x30 behielten den RE1.5-Riegel bis zum Ende ihrer HURT-Kette | 0x2b F61 -10 (einer), 0x29 F61 -14 (einer), je 3 Patronen/3 Blitze (8.1) | Riegel `ori v0,v0,0x1` @0x800124F0; Freigabe Kakerlake `sb zero,147(a0)` STAGE3.BIN @0x801149ac; RE2-Gate +0x1D3 @0x80047138-40 nur ueber den Filter; Port-Stempel re15_re2_stun_frames `return 0` fuer diese Typen | Kriterium = laufender RE2-Pausenfilter (`re2_pausenfilter_laeuft`, beretta_r37.c) | 0x2b F61/F64/F67 je -10, 0x29 je -14, Birkin 0x30 je -15, Glock 18 auf 0x2b je -10; Sonde Teil I: 0x23/0x27/0x29/0x2b/0x30 je drei Treffer Tick 0/3/6 |
| M2 Schadens-Regression | Folge von M1: Spalte 3 je Kugel bei nur EINER Kugel | 0x2b Stoss 10 (Basis 20), 0x29 14 (Basis 25) | Spalte 3 / 5 aus PSX.EXE @0x8006e0d0 + Typ*0x58 + 3*4 / + 5*4: 0x23 7/21, 0x27 12/28, 0x29 14/25, 0x2b 10/20, 0x30 15/28 | keine eigene (behoben mit M1) | Stoss 0x2b 30, 0x29 42, 0x30 45 (exe); 0x27 36, 0x23 21 (Sonde) — jeweils >= der alte Einzeltreffer |
| M3 Testluecke | Riegel nur unter RE1.5-KI bzw. Zombies | — | — | unit_r37_beretta Teil I (RE2-Flavor + Bild-Nachlauf der RE2-Filter, 5 Typen, Gegenproben Spinne/Kraehe, Einordnung RE2/RE1.5/MIXED); integration_r37_beretta t2b_std_w5 / roach_std_w5 (Standard-KI, exe) | Teil I mit dem ALTEN Kriterium gebaut: 10 FAIL (je 1 Treffer) -> mit Fix gruen (8.6) |
| M4 Doku falsch | Header/Kommentar setzten "RE2-Flavor" = "hat RE2-Pausenfilter", §7 behauptete 3 x Browning | — | — | Header-Absatz PORT-ABBILDUNG, Kommentar riegel_loesen(), §7 neu | — |

### 8.6 Tests Nachbesserung 1 (gezielt, keine volle Suite)
* `unit_r37_beretta` Teil I (neu, checks 70-96): Einordnung `re15_beretta_r37_re2_pause_regelt` (RE2: Zombie-Familie/0x20/0x21/
  0x25/0x26 ja, 0x23/0x27/0x29/0x2b/0x30/0x36/0x1a nein [NB2: 0x1a aus dieser Liste genommen, s. 9.3]; RE1.5: keiner;
  MIXED: nur 0x20); je Typ 0x23/0x27/0x29(Glock 18)/0x2b/
  0x30 unter RE2-Flavor mit dem Bild-Nachlauf der echten RE2-Filter: Kugel 1 trifft, der Riegel bleibt im Nachlauf stehen,
  Kugel 2/3 in Tick 3/6, Stoss = 3 x Spalte 3 (21/36/42/30/45) >= alter Einzeltreffer Spalte 5, Reaktion je Treffer;
  Gegenproben Spinne 0x25 (Pause 0: drei Treffer ueber den Filter, Riegel-Loesen 0) und Kraehe 0x21 (Pause 15: nur Kugel 1).
  GEGENPROBE gegen den Defekt: mit dem alten Kriterium (`re15_ai_re2_for_type`) gebaut -> `test_r37_beretta: 10 FAIL`
  (checks 72/73/76/77/80/81/84/85/88/89, je "Treffer 1"); mit Fix gruen.
* `integration_r37_beretta` + t2b_std_w5 (ROOM5040 0x2b) / roach_std_w5 (ROOM3040 0x29), Standard-KI ohne RE15_AI_FLAVOR
  (`unset(ENV{RE15_AI_FLAVOR})`): drei Abzuege E/E+3/E+6, je -10 / -14, Munition -3. Lauf lauf_824a3ced: 0x2b F61/F64/F67
  121 -> 91, 0x29 F61/F64/F67 99 -> 57; Leon RE2 F61/F64/F67 je -16, Elza F91/F94/F97 je -16. Am Stand vor NB1 lieferte
  derselbe Aufbau EINEN Abzug (8.1) — der Riegel waere rot.
* Regex U (eigene + alle Schuss-/Schadens-/Waffen-Gruppen + Gegnertypen der betroffenen Klasse, wie Abnahme 0 §5), am
  Endstand: `ctest --test-dir re15_port/build -R "^(unit_r37_beretta|unit_r36_schuss|unit_r36_schuss_kraehe|unit_r36_elza_schuss|unit_re2_hp_model|unit_re2_weapon_rows|unit_re2_baby_spider_dmg|m93r_nachladen|unit_enemy_shootable|unit_room1140_combat|unit_damage|unit_weapon_select|unit_hit_reaction|unit_re2_hit_reaction|unit_re2_hit_repeat|unit_re2_hit_repeat_crow_spider|unit_r17_munition_pin|unit_r17_waffen_loop_pin|unit_autofire_pin|unit_zombie_hit_fixes|unit_re2_zombie_hitfilter|unit_r16_trefferhoehe_pin|unit_r16_aufstehen_schuss|unit_schrot_trennung|unit_r36_blut_pin_.*|unit_dog_hit|unit_re2_dog_hitwindow|unit_writher_hit_feedback|unit_aim_all_around|unit_aim_lower_exit|unit_r35_werfer|unit_r35_werfer_kombi|unit_r36_granate|unit_r35_granate|unit_r30_granate|unit_dog_aim_band|unit_re2z_rise_hittable|unit_sca_wall_hit|unit_r35_affen_schrot|unit_maggot_ai|unit_cockroach_ai|unit_birkin_ai|unit_alligator_ai|unit_tyrant_ai|unit_maggot_bone_square|unit_re2_dog_hp_reroll|unit_maggot_zone_leap|unit_gator_sweep|unit_gator_fress|probe_p2_birkin_g5|probe_p3_birkin_rest|unit_r17_birkin_1zu1_pin|unit_r18_birkin_restluecken_pin|unit_r20_birkin_push_pin|unit_r30_birkin_frost|unit_r30_n_room5080_birkin_wand|r30_hund_hitbox_stauchung|r31_trefferboxen|unit_r34n_f_leiche_latch|unit_r35_affen_.*|unit_r36_zombieki_affen.*|unit_pri_inventar_riegel)$" --timeout 600 --output-on-failure`
  -> **`100% tests passed, 0 tests failed out of 89`** (26.96 sec).
* Regex X (exe-Laeufe der beruehrten Schusspfade, Runde-36-C-Riegel inklusive): `ctest --test-dir re15_port/build -R "^(integration_r37_beretta|integration_r36_schuss|integration_r36_elza_schuss|integration_r35_werfer|integration_r36_granate)$" --timeout 900 --output-on-failure`
  -> **`100% tests passed, 0 tests failed out of 5`** (555.91 sec; r35_werfer 145.22, r36_elza_schuss 49.54, r36_granate 95.21,
  r36_schuss 126.07, r37_beretta 139.82). Danach nur Kommentar-Zeilen im Header geaendert (Code identisch), neu gebaut,
  Regex U erneut gruen.

### 8.7 OFFEN (Nachbesserung 1)
* Nichts aus M1-M4. Gorilla 0x27 (11C0: Spieler nach dem Sprung festgehalten) und Alligator 0x23 (Boss-Modul 2090) nur in der
  Sonde (Teil I) gemessen, nicht an der exe; Birkin 0x30 zusaetzlich an der exe (ROOM5080, drei Treffer je -15).
* Weiter bewusst NICHT gebaut (wie §6): RE2-Abbruch beim Loslassen (@0x80048928-30), Einzelschuss-Bit 126 (@0x800488e4).

### 8.8 Fuer den Nutzer (Nachbesserung 1)
* Der Feuerstoss der Beretta M93R / Glock 18 trifft jetzt auch unter der STANDARD-KI jeden Gegner dreimal — auch Gorilla,
  Kakerlaken, den Tyrant-Gegner der Stages 4/5 (0x2b) und Birkin, die auch im RE2-Modus mit ihrem RE1.5-Verhalten laufen.
  Vorher traf dort nur die erste Kugel (3 Patronen fuer einen Browning-Schuss). Stoss jetzt: Kakerlake 42, Tyrant 30,
  Gorilla 36, Birkin 45, Alligator 21.
* Ausnahme bleibt nur, wo der Port die RE2-Trefferpause fuehrt: die Kraehe (RE2-Pause 15 Bilder) nimmt nur die erste Kugel
  — sie stirbt daran (10 HP, 15 Schaden).
* Keine neuen Assets, keine Sprachdateien, keine Bedienaenderung.

## 9. Nachbesserung 2 (Abnahme 1: N1 Gitterarm 0x1A, 2026-10-13)

Abnahmebericht: `G_abnahme_1.md` (Abschnitt 1f / 5). Mangel N1: seit Nachbesserung 1 loest riegel_loesen() den +0x93-Riegel
des Gitterarms 0x1A vor Kugel 2/3 (re2_pausenfilter_laeuft(0x1A) = 0); der getroffene, sich zurueckziehende Arm nimmt alle
drei Kugeln, der Rueckzug startet je Kugel neu. RE2 schliesst ihn per +0x1D3 (EM2D-HURT `|= 0x80`) + Applier-Gate 2 aus.
Fortlaufend geschrieben; Abschnitte 9.1 ff. folgen je Schritt.

### 9.1 Messung vorher (Spur-Stand 46e72af2, exe md5 1a2b95d657bccb2751aba752662c1231 = die Abnahme-1-exe)
Skript `G_beretta_mess/run_nb2.sh` (Aufbau Abnahme 1f: `JUMP=1210`, Leon (-19500,-13000,1024), Eingabe
`U0.8,R0.5,M1.0,MA0.1,M2.0,W2`, `RE15_RE2_TRACE=1`, Trace `re2_ki.log` neben der exe -> ins Messverzeichnis verschoben),
Auswertung `auswertung.py`; Logs `mess_r37g/nb2v_*` (unversioniert). Arm = Platz 5 (Typ 0x1a), kommt F38 heraus (1/0 -> 1/1).
| Lauf | KI | Waffe | Munition | HP Arm 5 | Rueckzug (Trace `[re2arm] slot 5`) | Ende (hp -1, 1/7, verborgen) |
|---|---|---|---|---|---|---|
| nb2v_arm_w3_std | Standard | 3 Browning | F70 15 -> 14 | F70 250 -> 245 | 1/5/0 -> 1/5/1 EINMAL | **F80 (E+10)** bei (-21750,-15897) |
| nb2v_arm_w5_std | Standard | 5 M93R | F70 15 -> 12 | F70 -5, F73 -5, F76 -5 (250 -> 235) | 5/0 -> 5/1 -> 5/0 -> 5/1 -> 5/0 -> 5/1 (DREIMAL neu, Clip 4 je Kugel von vorn) | **F86 (E+16)** bei (-21854,-15957) |
| nb2v_arm_w5_re15 | RE1.5 | 5 M93R | F70 15 -> 12 | F70/F73/F76 je -5 | wie w5_std (Trace zeilengleich) | F86 |
Muendung (wf `SPAWN id=2 sub=0`) je Lauf F70/F73/F76 bzw. F70. => N1 reproduziert, in beiden KI-Modi gleich.
Ursache (Code gelesen): `re15_ai_re2_for_type(0x1A)` = 1 in jedem Flavor (enemy_ai_re2_zombie.c:172), aber
`re2_pausenfilter_laeuft(0x1A)` = 0 (kein Filter fuer 0x1A, game_step_common.c Nachlauf) -> riegel_loesen() loescht den
+0x93-Riegel des Arms vor Kugel 2/3 (NB1-Kriterium); das alte Kriterium (NB0) hatte ihn uebersprungen.

### 9.2 RE-Belege (selbst disassembliert, RE2 Retail = Vorbild des Arm-Gehirns und der Feuerstoss-Form)
Werkzeug `.claude/skills/re15-psx-disasm/scripts/re2_disasm.py`; EM2D-Overlay selbst geschnitten
(`python re15_port/tools/re2_ems_cut.py 0x2D` -> `build/extracted/re2_ems/CDEMD0_EM2D_ai1.BIN`, 5416 B, md5 82ab5760...,
gleich der Hauptbaum-Kopie), `RE_OVERLAY_DIR=build/extracted/re2_ems`.
* **Kugel-Resolver der Custom Handgun** (`jal 0x800410cc` @0x80048a04, info/re2leon/PSX.EXE), Kandidatenschleife:
  ```
  80041264  andi v0,v0,0x1 / 80041268 beq v0,zero,0x80041300      ; Gate 1 aktiv
  80041270  lbu  v0,467(s2)                                        ; +0x1D3, GANZES Byte
  80041278  bne  v0,zero,0x80041300                                ; != 0 -> naechster Kandidat
  80041280  lh   v0,342(s2) / 80041288 bltz v0,0x80041300           ; Gate 3 HP < 0
  80041290  lhu  v0,270(s2) / 80041298 andi v0,v0,0xc000 / 8004129c bne   ; Gate 4
  80041300  j    0x800417d0 / 80041304 addiu fp,fp,4                ; naechster Listeneintrag
  ```
  Dieselben vier Gates hat der Applier FUN_800470C0 (`lbu v0,467(s0)` / `bne v0,zero,0x8004740c` @0x80047138-40).
* **EM2D-HURT** (`CDEMD0_EM2D_ai1.BIN`, Routine 2 @0x80100F8C): `lbu v0,467(s0)` @0x80100f9c, `addiu v1,zero,1281` /
  `sw v1,4(s0)` @0x80100fa0-a4 (0x501 = Rueckzug), **`ori v0,v0,0x80` @0x80100fa8**, **`sb v0,467(s0)` @0x80100fb0**
  (Delay-Slot des `jal 0x80015fe8`). Port: arm_hurt (enemy_ai_re2_zellenarm.c:810-815, `e->re2z_self1d3 |= 0x80`).
* **Rueckzug B5**: P0 `ori v0,v0,0x80` / `sb v0,467(s0)` @0x80100df0/f8 (Clip 0xF0004 @0x80100dd0-e0), Ende
  `addiu v0,zero,1793` / `sw v0,4(s0)` @0x80100e40-44 (0x701), `sh v1(-1),342(s0)` @0x80100e4c-50, `ori 0x80` / `sb` @0x80100e54-58.
* **Wer +0x1D3 wieder loescht**: nur INIT `sb zero,467(s0)` @0x80100130 und das Wieder-Auftauchen in A0
  `sb zero,467(s1)` @0x801005b8 / @0x8010061c — also erst, wenn der Arm neu aus dem Gitter kommt.
* **Trefferpause der Custom Handgun fuer den RE2-Arm** (Typ 0x2D, Zeile 4 = *(0x800A6B3C) + 3*0x14 = @0x800A51BC,
  w1 0x02850A14 -> (w1 >> 9) & 0x7f = 5; selbst gelesen): auch ohne das HURT-Bit waere Kugel 2 (3 Bilder spaeter) noch in
  der Pause. Der Port stempelt fuer 0x1A keine Pause (re15_re2_stun_frames `default: return 0`); massgeblich ist das
  Bit 0x80 aus arm_hurt, das bis zum Wieder-Auftauchen steht.
=> RE2: ein getroffener Arm ist fuer Kugel 2/3 KEIN Kandidat (+0x1D3 = 0x80 seit dem Trefferbild); die Kugeln gehen an den
naechsten Kandidaten der Liste. Der Rueckzug laeuft einmal.
Regel fuer den Port (keine neue Konstante): riegel_loesen() laesst den Riegel stehen, wenn der Gegner ein RE2-Gehirn hat
und sein +0x1D3-Byte != 0 ist (Gate @0x80041270-78). Das trifft den Arm (0x80 aus arm_hurt) und laesst die RE1.5-Gehirne
0x23/0x27/0x29/0x2b/0x30 unberuehrt: ihr +0x1D3 bleibt 0 (Stempel re15_re2_stun_frames `return 0`, re15_damage.c:2695-2696,
kein Gehirn schreibt es). G5 0x36 traegt nach einem Schuss ebenso 0 (Abschnitt 9.5).

### 9.3 Riegel zuerst (Teil J in unit_r37_beretta, VOR dem Fix gegen den NB1-Code gebaut)
`test_r37_beretta.c` Teil J: Gitterarm 0x1A mit dem ECHTEN RE2-Arm-Gehirn (re15_re2arm_tick, RE2-Bank EM2D aus
`shared_assets/RE2/CDEMD0.EMS`, Laden wie test_p2_1210_arme_re2.c), herausgekommen ueber A0 (0x101), hinter ihm ein Typ 0x2b
als naechster Kandidat; Bildfolge Feuerstoss-Tick -> Arm-Tick -> RE2-Filter-Nachlauf. Checks 100-109: Kontrolle Browning
(ein Treffer, Rueckzug einmal, Ende-Tick = Bezug), dann M93R in RE2/RE1.5/MIXED: +0x1D3 & 0x80 vor Kugel 2 (101/104/107),
EIN Treffer am Arm + Riegel nicht geloest (102/105/108), Rueckzug einmal, Ende wie Browning, Kugel 2/3 am 0x2b (-20 =
2 x Spalte 3 @0x8006EFA4) (103/106/109). Pruefung 70 fuehrt 0x1a nicht mehr in der Liste "wie RE1.5 behandelt".
Gegen den NB1-Code: **`test_r37_beretta: 6 FAIL`** — 102/105/108 "hp 250 -> 245, Rest 235, weitere 2, Riegel geloest 2",
103/106/109 "Rueckzug 3 x, Ende Tick 16 wie Browning 10, 0x2b -0". Die Sonde zeigt damit dieselben Zahlen wie die exe (9.1:
Browning E+10, M93R E+16, 250 -> 235, Rueckzug dreimal). Kontrolle 100 und die 0x80-Pruefungen 101/104/107 gruen.

### 9.4 Umsetzung (Commit "NB2 N1")
* `engine/src/beretta_r37.c`: neue Abfrage `re2_kugel_gate_sperrt(e)` = `re15_ai_re2_for_type(e->type)` UND
  `e->re2z_self1d3 != 0` (GANZES Byte, wie `lbu v0,467(s2)` / `bne v0,zero,0x80041300` @0x80041270-78 im RE2-Kugel-Resolver
  FUN_800410CC, den die Custom Handgun @0x80048a04 ruft). riegel_loesen() ueberspringt solche Gegner — ihr +0x93-Riegel aus
  Kugel 1 bleibt stehen, der Resolver setzt Bit 1 dazu (`+0x93 |= 2` @0x8001240c) und greift auf den naechsten Kandidaten
  zurueck (@0x80012404-18). Reihenfolge der Ausnahmen: sterbend (HP < 0) -> Pausenfilter-Typen -> Kugel-Gate.
* Keine neue Konstante. Das Byte +0x1D3 fuehrt der Port bereits (`re2z_self1d3`); fuer den Arm schreibt es arm_hurt
  (`|= 0x80` @0x80100FA8-B0), B5 P0 (@0x80100DE4/F0/F8) und das Rueckzug-Ende (@0x80100E48-58); geloescht nur INIT
  (@0x80100130) und A0 (@0x801005B8 / @0x8010061C).
* Wen es NICHT aendert (Code gelesen): RE1.5-Gehirne 0x23/0x27/0x29/0x2b/0x30 — nach einem Schuss +0x1D3 = 0
  (re15_damage.c:2695-2696, Stempel `(alt & 0x80) | re15_re2_stun_frames` mit `default: return 0`, kein RE1.5-Gehirn
  schreibt das Feld); G5 0x36 — der Schusspfad stempelt ebenfalls 0, das Modul setzt nie Bit 0x80 an sich selbst
  (enemy_ai_boss_g5.c:1542/1621 nur Abzug/Abfrage); nur eine noch laufende GL-/Explosions-Sperre (re15_damage.c:4274)
  wuerde den Riegel halten — dieselbe Regel, nach der das G5-Modul Bit 0 selbst erst bei Sperre 0 freigibt
  (enemy_ai_boss_g5.c:1621-1624, "Sperre abgelaufen"). Zombie-Familie/0x20/0x21/0x25/0x26 laufen wie vorher ueber ihren Filter.
* `include/re15_beretta_r37.h`: Absatz PORT-ABBILDUNG richtiggestellt — die falsche Begruendung "Gitterarm 0x1a (ein Treffer
  toetet, HP < 0)" ist ersetzt durch das Kugel-Gate mit Adressen und "stirbt NICHT am Treffer (250 -> 245), sondern am Ende
  seines Rueckzugs (`sh -1,342` @0x80100E4C-50)".
* Tests: unit Teil J (9.3), Pruefung 70 ohne 0x1a, integration_r37_beretta + arm_std_w5 / arm_re15_w5 (9.7).

### 9.5 Messung nachher (exe nach "NB2 N1", md5 2bb9b0f0fbea6b9420802bd249f87f42, Aufbau wie 9.1, Logs `mess_r37g/nb2n_*`)
| Lauf | KI | Waffe | Munition | HP Arm 5 | Rueckzug (Trace) | Ende (hp -1) |
|---|---|---|---|---|---|---|
| nb2n_arm_w5_std | Standard | 5 M93R | F70 15 -> 12 | **F70 250 -> 245 (EINER)** | 5/0 -> 5/1 EINMAL | **F80 (E+10)** bei (-21750,-15897) |
| nb2n_arm_w5_re15 | RE1.5 | 5 M93R | F70 15 -> 12 | F70 -5 (einer) | einmal | F80 |
| nb2n_arm_w6_std | Standard | 6 Glock 18 | F70 15 -> 12 | F70 -5 (einer) | einmal | F80 |
| nb2n_arm_w3_std (Kontrolle) | Standard | 3 Browning | F70 15 -> 14 | F70 -5 | einmal | F80 |
Muendung je M93R-/Glock-Lauf F70/F73/F76 (drei Blitze, drei Patronen); Kugel 2/3 finden in diesem Aufbau keinen weiteren
Kandidaten in der Bahn (die anderen Arme sind verborgen) — in der Sonde 9.3 gehen sie an den 0x2b dahinter (-20).
Trace bis auf die Bildnummern zeilengleich mit dem Browning-Lauf (Ende bei (-21750,-15897) statt vorher (-21854,-15957)).
Gegenprobe der bestandenen Punkte an der exe: nb2n_birk_def (ROOM5080 Birkin 0x30, Standard-KI) F61/F64/F67 je -15 (300 -> 255)
— unveraendert drei Treffer; 0x2b/0x29/Zombie/Elza ueber integration_r37_beretta (9.7).

### 9.6 Je Mangel (Ursache / vorher / Beleg / Aenderung / nachher)
| Mangel | Ursache | Messung vorher | Beleg | Aenderung | Messung nachher |
|---|---|---|---|---|---|
| N1 Gitterarm 0x1A nimmt alle drei Kugeln, Rueckzug je Kugel neu | NB1-Kriterium: `re2_pausenfilter_laeuft(0x1A)` = 0 -> riegel_loesen() loeste den +0x93-Riegel des Arms vor Kugel 2/3, in JEDEM KI-Modus (der Arm hat immer das RE2-Gehirn, aber keinen Filter); RE2s Kugel-Gate +0x1D3 fehlte als Kriterium | exe: F70/F73/F76 je -5 (250 -> 235), Rueckzug 3 x neu, Ende F86 (E+16) statt F80, Standard- und RE1.5-KI gleich (9.1); Sonde Teil J gegen NB1-Code 6 FAIL mit denselben Zahlen (9.3) | RE2-Kugel-Resolver FUN_800410CC `lbu v0,467(s2)` / `bne v0,zero,0x80041300` @0x80041270-78 (Ruf `jal 0x800410cc` @0x80048a04); EM2D-HURT `ori v0,v0,0x80` @0x80100fa8 / `sb v0,467(s0)` @0x80100fb0; Loeschen nur @0x80100130 / @0x801005b8 / @0x8010061c (9.2) | `re2_kugel_gate_sperrt` in riegel_loesen() (9.4); Header/Dossier 8.3/8.6 richtiggestellt; Pruefung 70 ohne 0x1a; Riegel Teil J + arm_std_w5/arm_re15_w5 | exe: EIN Treffer F70 -5, Rueckzug einmal, Ende F80 = E+10 wie Browning, M93R/Glock 18, Standard- und RE1.5-KI (9.5); Sonde: RE2/RE1.5/MIXED gruen, Kugel 2/3 am naechsten Kandidaten |

### 9.7 Tests Nachbesserung 2 (gezielt, keine volle Suite)
* `unit_r37_beretta` Teil J (checks 100-109, 9.3) + Pruefung 70 ohne 0x1a: gegen den NB1-Code 6 FAIL, mit Fix
  `test_r37_beretta: OK` (60 x ok).
* `integration_r37_beretta` + arm_std_w5 / arm_re15_w5: die Liste "Bild-E:Platz:dHP" im Fenster [E, E+10] muss genau
  `0:S:-5;10:S:-246` sein (ein Treffer, Rueckzug-Ende E+10). Lauf: `[arm_std_w5]: OK — F70 Munition -3, Arm 0:5:-5;10:5:-246`,
  `[arm_re15_w5]: OK — F70 Munition -3, Arm 0:5:-5;10:5:-246`. Am NB1-Stand lieferte derselbe Aufbau `0:5:-5;3:5:-5;6:5:-5`
  (nb2v_arm_w5_std/_re15, Ende F86 ausserhalb des Fensters) — rot. Alle frueheren Faelle unveraendert gruen:
  leon_re2_w5/w6/re15_w5 je `0:5:-16;3:5:-16;6:5:-16`, Browning `0:5:-16`, elza_re2_w5 `0:1:-16;3:1:-16;6:1:-16`,
  t2b_std_w5 `0:1:-10;3:1:-10;6:1:-10`, roach_std_w5 `0:2:-14;3:2:-14;6:2:-14`.
* Regex U2 (Regex U aus 8.6 + die Gruppen des Gitterarms/Writhers/1210 und G5, weil der Diff deren Riegel beruehrt):
  `ctest --test-dir re15_port/build -R "^(unit_r37_beretta|unit_r36_schuss|unit_r36_schuss_kraehe|unit_r36_elza_schuss|unit_re2_hp_model|unit_re2_weapon_rows|unit_re2_baby_spider_dmg|m93r_nachladen|unit_enemy_shootable|unit_room1140_combat|unit_damage|unit_weapon_select|unit_hit_reaction|unit_re2_hit_reaction|unit_re2_hit_repeat|unit_re2_hit_repeat_crow_spider|unit_r17_munition_pin|unit_r17_waffen_loop_pin|unit_autofire_pin|unit_zombie_hit_fixes|unit_re2_zombie_hitfilter|unit_r16_trefferhoehe_pin|unit_r16_aufstehen_schuss|unit_schrot_trennung|unit_r36_blut_pin_.*|unit_dog_hit|unit_re2_dog_hitwindow|unit_writher_hit_feedback|unit_aim_all_around|unit_aim_lower_exit|unit_r35_werfer|unit_r35_werfer_kombi|unit_r36_granate|unit_r35_granate|unit_r30_granate|unit_dog_aim_band|unit_re2z_rise_hittable|unit_sca_wall_hit|unit_r35_affen_schrot|unit_maggot_ai|unit_cockroach_ai|unit_birkin_ai|unit_alligator_ai|unit_tyrant_ai|unit_maggot_bone_square|unit_re2_dog_hp_reroll|unit_maggot_zone_leap|unit_gator_sweep|unit_gator_fress|probe_p2_birkin_g5|probe_p3_birkin_rest|unit_r17_birkin_1zu1_pin|unit_r18_birkin_restluecken_pin|unit_r20_birkin_push_pin|unit_r30_birkin_frost|unit_r30_n_room5080_birkin_wand|r30_hund_hitbox_stauchung|r31_trefferboxen|unit_r34n_f_leiche_latch|unit_r35_affen_.*|unit_r36_zombieki_affen.*|unit_pri_inventar_riegel|unit_writher_ai|unit_writher_kill_flag|unit_writher_abtauchen|unit_1210_gitterhaende|unit_1210_arme|unit_1210_arme_re2|unit_r35_raeume_arme|unit_r35_karte_r1210|unit_g5_tentakel|unit_g5_morph|unit_g5_boss)$" --timeout 600 --output-on-failure`
  -> **`100% tests passed, 0 tests failed out of 100`** (30.66 sec).
* Regex X (exe-Laeufe der beruehrten Schusspfade): `ctest --test-dir re15_port/build -R "^(integration_r37_beretta|integration_r36_schuss|integration_r36_elza_schuss|integration_r35_werfer|integration_r36_granate)$" --timeout 900 --output-on-failure`
  -> **`100% tests passed, 0 tests failed out of 5`** (565.64 sec; r35_werfer 132.94, r36_elza_schuss 46.44, r36_granate 91.66,
  r36_schuss 127.78, r37_beretta 166.79). Keine Fenster-Flatterer.

### 9.8 OFFEN (Nachbesserung 2)
* Nichts aus N1. G5 0x36 nicht an der exe gemessen (Boss-Kampf ROOM5090); Lage im Code belegt (9.4: der Schuss stempelt 0,
  das Gate greift nur waehrend einer laufenden GL-Sperre — dieselbe Regel wie das G5-Modul selbst), unit_g5_* gruen.
* Weiter bewusst NICHT gebaut (wie §6/8.7): RE2-Abbruch beim Loslassen (@0x80048928-30), Einzelschuss-Bit 126 (@0x800488e4).
* Messlogs `mess_r37g/` unversioniert (Zahlen in 9.1/9.5). Das EM2D-Overlay fuer die Disassembly liegt unter
  `build/extracted/re2_ems/` (gitignored, mit `python re15_port/tools/re2_ems_cut.py 0x2D` reproduzierbar).

### 9.9 Fuer den Nutzer (Nachbesserung 2)
* Die Gitterhaende in ROOM1210/1211 (RE2-Arme): ein Feuerstoss trifft den Arm, den er erwischt, EINMAL — wie in RE2, wo der
  getroffene Arm sich zurueckzieht und bis zum Wieder-Auftauchen kein Ziel ist. Die Kugeln 2/3 fliegen weiter und treffen
  den naechsten Gegner in der Schusslinie. Vorher blieben alle drei Kugeln am ohnehin zurueckweichenden Arm haengen
  (Schussverschwendung, drei Schmerzlaute, verlaengerter Rueckzug).
* Alle anderen Gegner wie in Nachbesserung 1: drei Treffer je Feuerstoss (Zombie, Hund, Spinne, Gorilla, Kakerlake, 0x2b,
  Birkin, Alligator); die Kraehe stirbt an der ersten Kugel.
* Keine neuen Assets (die Sonde liest das vorhandene `shared_assets/RE2/CDEMD0.EMS`), keine Sprachdateien, keine
  Bedienaenderung.
