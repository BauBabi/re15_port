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

### 2.2 RE1.5: die DREI Schuesse stehen in den Daten (CORE00.ESP, eigener Dump `esp_dump.py`)
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

## 4. Messung nachher (exe, eigener Bau mit 3f17b7db-Code; dieselben Laeufe wie 1.1, `mess_r37g/nachher_*`)
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
* Mit RE2-KI gilt RE2s Trefferpause je Gegner (RE2-Zeile 4): Zombie 1 Bild (gemessen: alle drei Kugeln treffen), Hund und
  Spinne 0 Bilder (Tabelle @0x800A4424/@0x800A4B90), KRAEHE 15 Bilder (@0x800A45A0) — eine Kraehe nimmt je Feuerstoss nur die
  erste Kugel, die Kugeln 2/3 gehen an das naechste Ziel (wie in RE2 Retail). Mit RE1.5-KI trifft jede Kugel; ein
  getroffener Zombie, der zu Boden geht, ist mit geradem Zielen kein Ziel mehr, dann trifft die naechste Kugel den naechsten.
* Ist nur noch 1 oder 2 Schuss im Magazin, gibt es nur 1 bzw. 2 Treffer (drei Blitze zeigt das Original trotzdem).
* Keine neuen Assets, keine Sprachdateien, keine Bedienaenderung.
