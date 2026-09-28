# Runde 31 — G: Generator ROOM11F0 (Abnahme erst, wenn der Zeiger FINAL auf 80 steht)

Zweig `r31/generator`, Baum `.claude/worktrees/r31_generator`.

Nutzer-Auftrag (woertlich, Auszug): "gleiches in room 11f0 bei den generator rätsel. warte erst bis
der zeiger final auf 80 steht, bevor du mit ok das abnimmst, das Licht anschaltest etc."

## Gliederung

1. Bestand im Port (panel_zeiger_common.c, Aufrufer, fruehere Dossiers)
2. Original-Reihenfolge RE2 ROOM2130.RDT sub04 (Datei-Offsets)
3. RE1.5 ROOM11F0 sub01/sub16/sub18 (Datei-Offsets)
4. Messung im Port heute (RE15_PANEL_LOG + Framedump) — Befund
5. Bau (Reihenfolge nach RE2, Belege je Konstante)
6. Abnahme (Log-Auszug, Framedump-Streifen, Sonde, Suite)
7. Offen

## 1. Bestand

- `re15_port/engine/src/panel_zeiger_common.c` + `re15_port/include/re15_panel_zeiger.h` (Belege im Kopf).
  Ablauf je Bild (`re15_panel_zeiger_tick`, gerufen am ENDE des Spiel-Ticks,
  `game_step_common.c:2343`, nach Notch-Stempel): Maske der zehn Schalterbits (Bank 5 / 13..22)
  -> Ziel = max(0, Summe Nutzergewichte); `wert` folgt mit 1 Punkt/Bild (RE2 `evt_next` @0x01216).
  Ab `Set(4,238,1)` steht das Ziel fest auf 80. Bestaetigungston RE2 Gruppe 2 / 0x0C an der
  steigenden Flanke von Flag 4:238.
- Gezeichnet wird nur in ROOM11F0 **Cut 10** (`re15_panel_zeiger_sicht`, `platform/pc/main.c:5247`).
- Die Loesung selbst entscheidet das RE1.5-SCD (sub01 laeuft jedes Bild neu, Memory
  `reai-v2-scd-per-frame-model`: FUN_8003f038 seedet Slot 1 unbedingt auf sub_scd[1]).
- Fruehere Dossiers: `analysis/befunde_2026-09-26/re2-schalterraetsel-2130.md` (RE2-Seite),
  `analysis/befunde_2026-09-26/raum11f0-raetsel-cursor.md` (RE1.5-Seite). Sonden
  `r26_panel_11f0`, `r27_panel_schalterwerte`.

**Vermutung aus dem Bestand (in §4 gemessen):** Die Loesungskette zieht in dem Bild, in dem das
letzte Schalterbit steht; sub18 schaltet sofort `Cut_chg 8` (@0x0173A) — der Zeiger (nur Cut 10)
ist damit weg, BEVOR er 80 erreicht. Der Nutzer sieht ihn nie auf 80 stehen.

## 2. RE2 ROOM2130 sub04

Walker: `analysis/nutzer_batch_2026-08-27/tools/re2_scd_walk.py` (Laengen aus der Dispatch-
Tabelle @0x800a74c8), Skript `build/r31_generator/re2_sub04.py`, Ausgabe
`generator_belege/re2_room2130_sub04.txt` (530 Zeilen, Status `ok`, sub04 = @0x01110..0x01820).

### 2.1 Reihenfolge je Schalter (Schalter 1, alle fuenf gleich gebaut)

| Datei | Bytes | Opcode | Wirkung |
|---|---|---|---|
| 0x01110 | `22 02 07 01` | set | Bank 2 Bit 7 = 1 -> **PAD-SPERRE fuer das ganze sub04** (s. 2.3) |
| 0x0118A | `2b 00 01 00 ff ef` | message_on | "It's a switch. Will you move it?" |
| 0x01190 | `02` | evt_next | |
| 0x01192 | `36 02 0a 01 ...` | se_on | Klick (Gruppe 2 / 0x0A) |
| 0x0119E | `06 00 7c 00` / `21 0b 1f 00` | if / ck | Ja gewaehlt? |
| 0x011BA | `26 00 00 10 24 00` | calc | var5 += 36 |
| 0x011C8 | `23 00 05 02 64 00` | cmp | Deckel 100 |
| 0x011E0 | `0f 06 36 00` | while | **NACHFUEHRSCHLEIFE** |
| 0x011E4 | `23 00 04 01 00 00` | cmp | while (var4 > 0) |
| 0x011FC | `26 00 00 10 09 00` | calc | var7 += 9 (Zeiger-X) |
| 0x0120A / 0x0120E | `1d 07 02 01` / `32 00 00 00 3e f5 c2 d4` | work_copy / pos_set | Zeiger setzen |
| 0x01216 | `02` | evt_next | **1 Bild je Punkt** |
| 0x01218 | `10 00` | ewhile | Schleifenende |
| 0x0121A..0x01290 | | else-Zweig | dasselbe fuer "Nein" (var5 -= 14, var7 -= 9) |
| 0x01294 | `64 01 16 02 ... c2 a0 ...` | sce_espr_on2 | Schalterlampe 1 an — **nach** der Schleife |
| 0x012A4 / 0x012A5 | `09` / `0a 1e 00` | sleep / sleeping | **30 Bilder Stillstand** |
| 0x012A8 | `2b 00 02 00 ff ef` | message_on | erst jetzt die Frage zu Schalter 2 |

### 2.2 Nach dem LETZTEN Schalter — die Abnahme

| Datei | Bytes | Opcode | Wirkung |
|---|---|---|---|
| 0x01708 | `10 00` | ewhile | Nachfuehrschleife Schalter 5 zu Ende — **Zeiger steht** |
| 0x0170C | `64 05 16 02 ... 22 a2 ...` | sce_espr_on2 | Lampe 5 |
| 0x0171C / 0x0171D | `09` / `0a 1e 00` | sleep 30 | **30 Bilder Stillstand vor jeder Auswertung** |
| 0x01720 / 0x01724 | `2e 01 00` / `32 00 5b a0 00 00 ae d3` | work_set / pos_set | Spieler zurueck |
| 0x0172C | `29 04` | cut_chg 4 | Kamera zurueck |
| 0x0172E / 0x01732 | `06 00 1c 00` / `23 00 05 00 64 00` | if cmp | ==100 -> "too high" |
| 0x0174E / 0x01752 | `06 00 80 00` / `23 00 05 00 50 00` | if cmp | **== 80** |
| 0x01758 | `2b 00 07 00 ff ff` | message_on | "Power supply OK." |
| 0x0175E | `22 04 3c 01` | set | Raetsel-geloest-Flag |
| 0x01762 | `36 02 0c 01 00 00 9b a0 00 fc f4 d3` | se_on | Bestaetigung Gruppe 2 / 0x0C |
| 0x0176E / 0x0176F | `09` / `0a 14 00` | sleep 20 | |
| 0x01772 | `29 08` | cut_chg 8 | Strom-Ansicht |
| 0x01774 / 0x01784 | `64 0b ...` / `64 0c ...` | sce_espr_on2 | |
| 0x01798 | `36 02 0f 01 ...` | se_on 0x0F | Strom fliesst |
| 0x01818 | `22 02 07 00` | set | Pad-Sperre aus |
| 0x0181C / 0x0181E | `3c 01` / `01 00` | cut_auto / evt_end | |

**Antwort auf die Frage des Auftrags:** Die Pruefung `var5 == 80` (@0x01752) liegt **NACH** dem
Ende der Nachfuehrschleife (@0x01708) — der Zeiger steht, wenn geprueft wird. Zwischen
Schleifenende und "OK" liegen: Lampe (@0x0170C), **30 Bilder Sleep** (@0x0171C `09` + @0x0171D
`0a 1e 00`, 0x1E = 30), Spieler-Reposition, `cut_chg 4`, dann die Auswertung. "OK" = Meldung
"Power supply OK." (@0x01758) + Flag (@0x0175E) + Bestaetigungston 0x0C (@0x01762) im selben
Skriptschritt (message_on @0x80054A8C kehrt mit `v0 = 1` = weiter zurueck, s. 2.4).

### 2.3 Sperrt RE2 die Eingabe waehrend der Fahrt? — JA

- sub04 setzt Bank 2 Bit 7 als ERSTEN Befehl (@0x01110 `22 02 07 01`) und loescht es als
  vorletzten (@0x01818 `22 02 07 00`). Die Nachfuehrschleife liegt vollstaendig dazwischen.
- Bank 2 = `0x800CFBDC` (Bank-Zeigertabelle @0x800A78C8, Eintrag 2, selbst gelesen:
  `0x800cfb74, 0x800cfbd8, 0x800cfbdc, ...`); Bit 7 = Maske `0x80000000 >> 7 = 0x01000000`
  (ck/set-Handler @0x80054354: `lui v0,0x8000 / srlv`).
- Leser (ghidra_re2_Leon.txt Z. 134810 ff.), Pad-Aufbereitung:
  ```
  800391f8  lw   v1,-0x424(v1)   ; 0x800CFBDC
  800391fc  lui  a0,0x100        ; 0x01000000
  80039200  and  v0,v1,a0
  80039204  beq  v0,zero,...
  80039210  lw   v0,0x800CE30C   ; logisches Pad-Wort
  8003921c  andi v0,v0,0x3c00    ; nur noch Menue-Tasten
  80039224  sw   v0,0x800CE30C
  ```
  => Solange Bit 0x01000000 steht, bleiben nur die Menuetasten 0x0400/0x0800/0x1000/0x2000
  (Auswahlbox) — und waehrend der Schleife ist keine Auswahlbox offen. **Waehrend der Fahrt
  und der 30 Bilder danach ist in RE2 keine Eingabe moeglich.**
- RE1.5 hat dasselbe Bit mit derselben Wirkung: Set(2,7,*) = `0x01000000` in `0x800aca40`,
  FUN_80030444 @0x800304f4..0x8003051c maskiert das virtuelle Pad-Wort auf `0xf000`
  (Port: `game_step_common.c:1136`). Die Cursor-/Schalter-Tasten des RE1.5-Panels
  (Sce_key_ck 0x01/0x02/0x04/0x08/0x40, sub01) liegen ausserhalb von 0xf000.

### 2.4 message_on blockiert NICHT selbst

RE2-Handler 0x2b = Tabelle @0x800a74c8 + 0x2b*4 = @0x800a7574 -> `0x80054A8C`:
`lw v0,28(a0)` / `addiu v0,v0,6` / `sw v0,28(a0)` (pc += 6) / `jal 0x8002fe38` (Meldung auf) /
`or v1,v1,s0` + `sw` nach `0x800CFBDC` (Pausenmaske u16@+4 << 16) / `addiu v0,zero,1` (weiter).
=> Meldung, Flag und Ton 0x0C fallen in RE2 in dasselbe Skript-Bild.
(evt_next 0x02 = @0x80053860: pc += 1, `addiu v0,zero,2` = Bild abgeben.)

## 3. RE1.5 ROOM11F0

Walker: `python re15_port/tools/scd_dump_room.py re15_port/shared_assets/PSX/STAGE1/ROOM11F0.RDT`
(694 Zeilen, main00 + 20 Subs). Auszug: `generator_belege/room11f0_sub01_16_17_18.txt`.

### 3.1 Wann und wie oft wird die Loesungskette ausgewertet? — JEDES BILD

sub01 (@0x0108C..0x012F2) wird in jedem Spielbild neu gestartet (FUN_8003f038 seedet Slot 1
unbedingt auf sub_scd[1], @0x8003f064/@0x8003f070/@0x8003f080; Memory
`reai-v2-scd-per-frame-model`). Die Kette ist NICHT an Bank 5 Bit 0 gebunden:

| Datei | Bytes | Wirkung |
|---|---|---|
| 0x012B6 | `06 00 36 00` | Ifel_ck (Block bis 0x012EE) |
| 0x012BA | `21 04 ee 00` | Ck(4,238) == 0 — noch nicht geloest |
| 0x012BE..0x012E2 | `21 05 0d 01` .. `21 05 16 00` | zehn Schalterbits == 1,0,1,0,1,0,1,0,1,0 |
| 0x012E6 | `04 ff 18 12` | **Evt_exec(sub18)** = die Abnahme |
| 0x012EA | `22 04 ee 01` | Set(4,238,1) — Kette zieht nie wieder |

Also: die Kette wird **je Bild** ausgewertet und zieht **im ersten Bild**, in dem die zehn Bits
stimmen — voellig unabhaengig vom (in RE1.5 gar nicht vorhandenen) Zeiger.

### 3.2 Wann steht das letzte Schalterbit? — am ENDE der 16-Bild-Kippung

sub06 (Schalter 1, @0x01322; sub07..15 gleich gebaut): `22 05 01 00` Zelle sperren @0x01322,
`2f 05 40 00` Speed_set(Achse 5, +64) @0x01332, `0d 00 04 00 10 00` For 16 @0x01336,
`30`/`02` Add_speed + Evt_next @0x0133C/0x0133D, **erst dann** `22 05 0d 01` Set(5,13,1)
@0x01340; Zelle frei `22 05 01 01` @0x0135C. => Das Bit (und damit Zeigerziel UND Loesungs-
kette) aendert sich im Bild nach der 16. Kippstufe.

### 3.3 Was macht sub18 (@0x016F6..0x017B6) der Reihe nach?

| Datei | Bytes | Wirkung |
|---|---|---|
| 0x016F6 | `22 04 f3 01` | Set(4,243,1) — Reservestrom global an |
| 0x016FA..0x0172A | 13 x `22 05 xx 00` | Bank 5 Bits 0..12 aus (Raetsel inaktiv; Bits 13..22 BLEIBEN) |
| 0x0172E / 0x01732 | `22 02 00 00` / `22 02 02 00` | Spieler-/KI-Pause aus |
| 0x01736 | `22 02 07 01` | **Pad-Sperre an** (dasselbe Bit wie RE2 @0x01110) |
| 0x0173A | `29 08` | **Cut_chg 8** — Kamera weg vom Panel |
| 0x0173C | `54 00 01 00 46 40` | Sce_bgm_control |
| 0x01742 | `2b 02 ff ff` | **Message_on 2 = "Power supply OK."** (RDT-Text @0x18F1) |
| 0x01746 | `02` | Evt_next |
| 0x01748 | `29 0d` | Cut_chg 0x0D |
| 0x0174A | `56 00 02 07 00 00` | Fade-Kanal-Konfig (Handler @0x80042a58) |
| 0x01750..0x0176C | `57 00 00 08` / `57 00 00 00` + Sleep 2/3/4/3 | **Licht-Flackern = "Licht an"** |
| 0x01770 / 0x01772 | `29 0e` / `09 0a 28 00` | Cut_chg 0x0E, 40 Bilder |
| 0x01776 | `46 01 01 31 02 00 ff ff 00 00` | Aot_reset slot 1 (Panel -> nur noch Text) |
| 0x01780 / 0x01782 | `29 08` / `3c 01` | Cut_chg 8, Cut_auto |
| 0x01784 | `22 02 07 00` | Pad-Sperre aus |
| 0x01788..0x017B0 | `2e 02 0n` / `34 0c 89/8a 00` + Sleep 5 | vier Gegner wecken |

**Das "OK" des Auftrags** ist die Meldung 2 "Power supply OK." (@0x01742; RDT-Text `id 2 @0x18F1`).
RE1.5 spielt dazu KEINEN Ton (0 Se_on im ganzen SCD); der Port legt seit Runde 26 den RE2-
Bestaetigungston Gruppe 2 / 0x0C (RE2 @0x01762) auf die Flanke von Set(4,238,1) @0x012EA — das
ist dasselbe Bild wie Evt_exec(sub18) und damit wie die Meldung. Meldung 3 "Power supply
incorrect." (@0x1909) steht im RDT, wird aber von keinem Sub benutzt (Beta-Luecke, nicht Teil
dieses Auftrags).

### 3.4 Unterschied RE1.5 <-> RE2, der den Befund erklaert

RE2 prueft die 80 erst, nachdem die Nachfuehrschleife fertig ist und 30 Bilder stand (§2.2).
RE1.5 hat keinen Zeiger und prueft den Schalterzustand sofort in jedem Bild. Der Port hat
den RE2-Zeiger nachgeruestet, die Abnahme aber auf der RE1.5-Zeitachse gelassen — die zieht
im Bild der letzten Schalterstellung, waehrend der Zeiger noch 20..30 Punkte unterwegs ist.

## 4. Messung heute (echte exe, vor dem Umbau)

Stand: Zweig `r31/generator` auf 10d0c706 + nur die erweiterte Messschiene (Bildnummer,
`strom`=4:243, `padsperre`=0x01000000, `msg`, `bestaet`=Zaehler des Bestaetigungstons).
Lauf: `generator_belege/lauf.sh` (DEBUG_JUMP 11F0, FIRE_AOT Slot 1 = Panel-Untersuchung,
Eingabeskript Basis Spielbild ab F320: Meldungen 0/1 blaettern, "Ja", dann echte Cursor-Fahrt
und Tastendruck auf Schalter 7, 9, 3, 1, 5 — Anzeige 0->20->50->40->60->80). Zwei Laeufe
(`vorher`, `vorher_fd`) liefern bildgleiche Protokolle (deterministisch).

Auszug `generator_belege/vorher_panel_F1180-1203.log`:

```
F1182 cut=10 aktiv=1 maske=145 ziel=60 wert=60 geloest=0 strom=0 padsperre=0 msg=0 bestaet=0
F1183 cut=10 aktiv=1 maske=155 ziel=80 wert=61 geloest=0 strom=0 padsperre=0 msg=0 bestaet=0   <- letztes Schalterbit
F1184 cut=10 aktiv=0 maske=155 ziel=80 wert=62 geloest=1 strom=1 padsperre=1 msg=1 bestaet=1   <- ABNAHME
F1185 cut=8  aktiv=0 maske=155 ziel=80 wert=63 geloest=1 ...                                   <- Kamera weg
F1202 cut=8  ...                   wert=80                                                     <- Zeiger erst hier auf 80
```

**Befund:** Die Loesungskette zieht im Bild NACH dem letzten Schalterbit (F1184) — Meldung
"Power supply OK." (msg=1), Bestaetigungston (bestaet 0->1), Reservestrom 4:243, Pad-Sperre von
sub18, und ab F1185 Cut 8. Der Zeiger steht da auf **62**; die 80 erreicht er erst in **F1202**,
18 Bilder spaeter und unsichtbar (Cut 8). Differenz zum RE2-Soll (§2.2: OK erst 31 Bilder nach
dem letzten Zeigerschritt): die Abnahme kommt **49 Bilder zu frueh** (Soll: 80 in F1202, OK in
F1233; Ist: OK in F1184).

Bildbeleg `generator_belege/vorher_streifen.png` (RE15_FRAMEDUMP, Readback vor Present;
F1182/F1183/F1184/F1185/F1190): In F1184 steht der rote Zeiger bei ~62 (Hoehe der "60") und
das "P" von "Power supply OK." tippt bereits; F1185 zeigt Cut 8.

## 5. Bau

(laufend)

## 6. Abnahme

(laufend)

## 7. Offen

(laufend)
