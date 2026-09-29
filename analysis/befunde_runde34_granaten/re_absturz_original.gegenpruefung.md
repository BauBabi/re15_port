# Runde 34 / Gegenpruefung — Absturz des Originals beim Granateneinsatz

Gegenpruefer (Skeptiker) fuer `re_absturz_original.md`. Stand: 2026-09-29, ABGESCHLOSSEN (Urteil §6: bestaetigt,
Abweichungen A1-A6 in §4.2). Reihenfolge der Abschnitte = Arbeitsreihenfolge (1, 3, 2, 4, 2.4, 5, 6).
Regeln: nur ermitteln/dokumentieren; keine Aenderung unter `re15_port/`, kein Build, keine git-Schreiboperation.
Werkzeuge der Gegenpruefung: `re_absturz_gegen_werkzeug/` (eigene Skripte), Laufzeit-Ausgaben `build/r34g_absturz/gegen/`.
Jede Instruktion hier SELBST disassembliert (`re15_disasm.py`, Binaerdatei im Kopf jeder Ausgabe geprueft), jede
RAM-/Registerzahl SELBST aus dem Savestate gelesen — mit einem eigenen Leser (`gp_ss.py`), nicht mit
`ss_cpu.py`/`zustand.py` des Ermittlers.

## Pruefpunkte (Auftrag)
1. Jede Adresse/Instruktion der Kette selbst disassemblieren (Binaerdatei, Stage, Delay-Slots, Tabelleninhalt).
2. Absturzstelle = Ursache oder Folgeschaden?
3. Auslieferungsstand gemessen (keine gepatchte EXE, keine Mod-Overlays)?
4. Teile-Einordnung (laeuft/kaputt/unerreichbar); Schaden im Original ohne Absturz anwendbar?

---

## 1. Statische Kette — Instruktion fuer Instruktion nachgeprueft

Alle EXE-Adressen aus `info/Re1.5/PSX.EXE` (t_addr 0x80010000, Header 0x800), alle Overlay-Adressen aus
`info/Re1.5/PSX/BIN/STAGEn.BIN` RAW @0x80100000 (Werkzeugkopf der Ausgabe nennt die Datei).

| # | Stelle | selbst gelesen | Urteil |
|---|---|---|---|
| 1 | Entlade-Tabelle `table 0x80074100` | [9] 0x80033b38, [10] 0x80033b58, [11] 0x80033b78, [15..18] = 0 (Bytes `@0x8007413c..4b` = `00`) | bestaetigt |
| 2 | 0x80033b38/58/78 | je `addiu sp,-24 / sw ra / jal 0x8004eae4 / nop / lw ra / addiu sp / jr ra / nop` | bestaetigt |
| 3 | Sprungziel 0x8004eae4 (Zitat-Regel) | liest Platz `0x800b25c8`, Menge `0x800b10ad+4*Platz`; `8004eb44: bne v0,zero` / `8004eb48: addiu v0,v0,-1` / `8004eb60: sb v0,0(at)` -> Menge-1, v0 = 1 (0 bei Menge 0) | bestaetigt: Munition −1 |
| 4 | Spawn-Gate | `80033684: lbu v1,-13731(v1)` (0x800aca5d) / `80033688: ori v0,zero,0x9` / `8003368c: bne v1,v0,0x800337ac` (Slot: `ori v0,zero,0x13`) | bestaetigt |
| 5 | Spawn | drei Wurfhoehen je Bild +0x95 = 0x13/0x16/0x18 und Maske 0x800acaec & 0x8000/0x4000/0x2000; je `lui a0,0x40d` / `ori a0,a0,0x1000` / `jal 0x80019700` (@0x800336ec, @0x80033748, @0x800337a4) | bestaetigt (a0 = 0x040D1000) |
| 6 | Routinentabelle `table 0x80071d40 48` | [29] 0x80018320, [30] 0x8001843c, [31] 0x8001854c | bestaetigt |
| 7 | ESP-Treiber 0x80019e20 | Pool 0x800a73b8, Schritt 132, Ende +12672 (= 96 Plaetze); Routine A `80019e70: lbu v0,108(v1)` & 1 -> `80019e84: lhu v0,0(v1)` / `80019e9c: jalr v0` ueber 0x80071d40; Routine B `8001a2b4: lhu v0,2(v0)` / `8001a2d4: jalr v0` | bestaetigt |
| 8 | Routine 31 0x8001854c | Zuender = `lhu v1,30(a1)`; ==0 -> 0x80018688 (Kind 0x030B5800, Flags := 0); ==7 -> `8001857c: sb v0,21336(at)` (0x800b5358 := 1), `80018584: sb v0(=0x61),108(a1)`, P = (+40, +42−500, +44) auf sp+16, `80018598: ori a0,zero,0x1f4`, `800185b4: ori a2,zero,0x2`, **`800185b8: jal 0x80012d60`** (Slot `sw v0,24(sp)`), `800185dc: jal 0x800199d4` (a0 0x03195000), `800185ec: jal 0x80045024` (a0 0x04080001); danach `8001867c: addiu v0,v0,-1` / `80018684: sh v0,30(v1)` -> **Zuender 7 -> 6 im Explosionsbild** | bestaetigt |
| 9 | Aufrufer des Resolvers | Wortsuche `jal 0x80012d60` = 0x0C004B58 in EXE + allen BINs: **genau 2** (@0x80018008 Art 0 `addu a2,zero,zero`, @0x800185b8 Art 2); kein Datenwort 0x80012d60, kein `lui 0x8001/addiu 0x2d60`-Paar (die 22 `addiu …,0x2d60`-Treffer in den Overlays bilden 0x80072d60) | bestaetigt: Art 3/4 ohne Aufrufer |
| 10 | Resolver-Gegnerzweig 0x80012d60 | Trefferliste ueber `jal 0x8002b5d0` (@0x80012dd0); je Treffer: Eigentuemer-Test `80012f40..4c`, Maske `+0x90 & 0x03000000 == 0x03000000` -> ueberspringen (`80012f54..60`), `80012f88: sb v0,147(s1)` (+0x93 &= 1), Punkt-hinten 0x80 (`80012fac`), Riegel `80012fbc: andi v0,v1,0x1` / `80012fc0: beq` (gesperrt -> `ori 0x2`, weiter); sonst `80012fd4: sb zero,7(s1)`, `80012fd8: sb v0(=1),6(s1)`, `80012fe8: lbu v0,0(at)` (0x8006f430+Art), `80012ff0: sb v0,5(s1)`, `80012ff4: lhu a0,0(s3)` (0x8006f418+2*Art), `80012ffc: subu` / `80013000: sh v1,154(s1)`, `8001300c: sb v0,147(s1)` (+0x93 \|= 1), `80013018: sb v0(=2),4(s1)`, `80013014: bgez v1` sonst `80013020: sb v0(=3),4(s1)` | bestaetigt (HP genau 0 -> +4 = 2 HURT) |
| 11 | Tabellen | `read 0x8006f418 11 --w 2 --signed` = [10,20,1000,1000,1000,50,100,200,300,1000,0]; `read 0x8006f430 11 --w 1` = [3,3,9,10,11,14,15,16,17,18,20]; Bytes `e8 03` @0x8006f41c, `09` @0x8006f432 | bestaetigt |
| 12 | Hauptlauf | `8001ce04: jal 0x8001a50c` (RA 0x8001ce0c) … `8001ce2c: jal 0x80019e20` | bestaetigt: Gegner VOR ESP im selben Bild |
| 13 | Gegnerschleife 0x8001a50c | `8001a540: sw v0,0(v1)` (0x800ac784 := 0x800acc2c), s3 = 0x80072bac, `8001a570: lbu v0,8(v1)` / `8001a578: sll` / `8001a57c: addu v0,v0,s3` / `8001a580: lw v0,0(v0)` / **`8001a588: jalr v0`** (Slot `addiu s0,s0,1`) | bestaetigt |
| 14 | Typ-Tabelle 0x80072bac[0x10] | im RAM (eigener Leser, `vb_ende.sav` und `mzd_stage1_engage_live.sav`) = 0x80100424; Typen 10,11,12,16,18,1c-1f -> 0x80100424, 13 -> 0x8010a8c8, 1a -> 0x8010c1ec, 20/21/26/27 … (21 Eintraege, deckt sich mit dem Zensus des Ermittlers) | bestaetigt |
| 15 | Zombie-Wurzel STAGE1 0x80100424 | Sperren `8010043c` (g_pauseflags & 0x20000000) und `8010045c` (+9 & 0x20 = geparkt); `80100568: lbu v0,4(v0)` / `80100570: sll v0,v0,2` / `80100574: lui at,0x8012` / `80100578: addiu at,at,-2124` (0x8011f7b4) / `80100580: lw v0,0(at)` / **`80100588: jalr v0`** / Slot `nop` | bestaetigt |
| 16 | Zustandstabelle `table 0x8011f7b4` | [2] 0x80105a8c (HURT), [3] 0x80106ba4 (DEATH) | bestaetigt |
| 17 | Todeswurzel 0x80106ba4 | `80106ba8: lw a1,-14460(a1)`; `80106bb4: lbu v0,9(a1)` / `80106bbc: andi v0,v0,0x80` / `80106bc0: beq v0,zero,0x80106bd8` (liegend -> `80106bc8: jal 0x80107cb0`); `80106bd8: lui a0,0x8012` / `80106bdc: addiu a0,a0,-340` (0x8011feac) / `80106be0: lbu v1,5(a1)` / `80106be4: lbu v0,6(a1)` / `80106be8: sll v1,v1,5` / `80106bec: addu v1,v1,a0` / `80106bf0: sll v0,v0,2` / `80106bf4: addu v0,v0,v1` / `80106bf8: lw v0,0(v0)` / `80106bfc: nop` / **`80106c00: jalr v0`** / `80106c04: nop` / `80106c08: lw ra,16(sp)`. Rohbytes `bytes 0x80106bf8 16`: `00 00 42 8c` (lw v0,0(v0)), `00 00 00 00`, **`09 f8 40 00` = 0x0040F809 = jalr ra,v0**, `00 00 00 00` | bestaetigt; RA nach diesem jalr = 0x80106c08 |
| 18 | Todestabelle `read 0x8011feac 8 --rows 22 --rowstride 32` | Zeile 9 = [0,0,0,0,0x80107634,0,0,0]; Rohbytes `bytes 0x8011ffcc 32` = `00…00 \| 34 76 10 80 \| 00…` -> **Wort 0x8011ffd0 = 00 00 00 00** | bestaetigt: Zelle [9][1] = NULL, kein Muell |
| 19 | HURT-Tabelle 0x8011fb90 | Zeile 9 = 8 x 0 | bestaetigt |
| 20 | Stage-Pendants | eigene Kette Wurzel -> +4-Tabelle -> [3] -> Tabelle: ST2 Wurzel 0x801002b8 `8010040c` 0x801178b8[3] = 0x80106a38, Tabelle 0x80117fb0, **jalr @0x80106a94**, Zeile 9 = [0,0,0,0,X,0,0,0]; ST3 0x80100510 -> 0x8011d904[3] = 0x80106c90, 0x8011dffc, **@0x80106cec**, [9][1] = 0; ST4 0x801003d8 -> 0x80118cdc[3] = 0x80106b58, 0x801193d4, **@0x80106bb4**, [9][1] = 0; ST5 0x80100558 -> 0x8011e844[3] = 0x80106cd8, 0x8011ef3c, **@0x80106d34**, [9][1] = 0; jeweils mit derselben Liege-Weiche `lbu v0,9(a1)` / `andi 0x80` | bestaetigt |

**Befund zu Punkt 1:** Jede zitierte Adresse der Kette stimmt in der richtigen Binaerdatei, mit korrekten
Verzoegerungsslots; die NULL-Zelle [9][1] steht **statisch in STAGE1.BIN** (kein Laufzeit-Muell, kein
Ueberschreiben). Die Stage-Pendants sind die jalr-Stellen der jeweiligen Zombie-DEATH-Wurzel.

## 3. Auslieferungsstand (vorgezogen) — was lief im Emulator wirklich?

### 3.1 Disc (eigener ISO9660-Leser `iso_check.py`, MODE2/2352, Nutzdaten ab Byte 24)
`build/r34g_absturz/gegen/iso_check_mzd.txt`: **gleich 692, anders 1 (`PSX/MOVIE/CAPCOM.STR`), fehlt im Baum 0.**
Bytegleich u.a. PSX.EXE (md5 b55fdaa5…), SYSTEM.CNF, STAGE1..5.BIN, DEBUG.BIN (c2c11aab…), CORE00.ESP
(3049f588…), PL00W09.PLW. -> deckt sich mit §2.1 des Ermittlers.

### 3.2 RAM des Absturz-Saves gegen den Auslieferungsstand (`ram_vs_disc.py`, Ausgabe `ram_vs_disc_vb_ende.txt`)
Wichtiger als die Disc: DuckStation fuehrt nach `-statefile` den **RAM des Savestates** aus. Daher RAM gegen Dateien:
* Kopf `vb_ende.sav`: Titel "Biohazard 1.5 (MZD Mod) Update 25-01-2025", `HASH-957757946319438E`,
  Medium `…\Biohazard 1.5 (MZD Mod) Update 25-01-2025.cue` (nicht `re15_save_final`).
* `@0x80026e4c` = `08 00 e0 03 21 10 00 00` -> **ungepatchter Auslieferungs-Stub** (nicht `24 c2 01 08`).
* **Overlay 0x80100000..+137648 = STAGE1.BIN, 0 abweichende Bytes** (ganzes Overlay).
* EXE-Code 0x80010000..0x80068000: **genau eine** abweichende Stelle, `0x80013b7c..83`: RAM `b0 00 03 08 00 00 00 00`
  (= `j 0x800c02c0` / `nop`) statt Datei `0c 80 01 3c 9c ed 20 ac`. **Neu, beim Ermittler nicht erwaehnt.**
  Herkunft selbst belegt — es ist KEIN Mod, sondern der spieleigene Debug-Lader der Auslieferungs-EXE:
  ```
  8001311c: addiu sp,sp,-4 / 80013124: ori a0,zero,0x7 / 80013128: lui a1,0x800c
  8001312c: jal 0x80013b60        (Datei 7 = DEBUG.BIN nach 0x800c0000 laden)
  80013134: lui t0,0x8001 / 80013138: lw t0,12632(t0)      (Wort @0x80013158 = 080300b0 = j 0x800c02c0)
  8001313c: lui t1,0x8001 / 80013140: addiu t1,t1,15200    (0x80013b60)
  80013144: sw t0,28(t1)          -> 0x80013b7c := j 0x800c02c0
  8001314c: sw zero,32(t1)        -> 0x80013b80 := nop
  ```
  Ziel 0x800c02c0 liegt in DEBUG.BIN (Datei-Offset 0x2c0, bytegleich gefunden) = Ladeprotokoll
  "Loading %s at %x from %x" (String @0x800c0384), fuehrt die zwei ersetzten Instruktionen nach
  (`800c0308: lui at,0x800c` / `800c030c: sw zero,-4708(at)`) und springt `800c0310: j 0x80013b84` zurueck.
  RAM 0x800c0000..+0x40000 = DEBUG.BIN bis auf 0x800c7934..0x800ccb73 (Laufzeitdaten des Debug-Moduls).
  Liegt auch im Titel-Save `mzd_title.sav` und in `mzd_inv_open.sav` vor. **Fuer die Granaten-Kette belanglos**
  (Dateilader, kein Teil der Kette), aber ein Beleg, dass "RAM-Code == PSX.EXE" nicht woertlich gilt.
* Alle Funktionen/Tabellen der Kette im RAM == Datei: 0x80012d60+0x30c, 0x8001854c+0x198, 0x8001a50c+0xd4,
  0x8001cdec+0x50, 0x80019e20+0x120, 0x80033640+0x17c, 0x80033b38+0x60, 0x80074100+0x50, 0x80071d40+0xc0,
  0x8006f418+0x24 (EXE); 0x80100424+0x170, 0x80106ba4+0x74, 0x8011f7b4+0x30, 0x8011feac+0x2c0, 0x8011fb90+0x2c0 (STAGE1).
* Basis-Save `stage_saves/mzd_inv_open.sav`: identisches Bild (Stub ungepatcht, Overlay 0 Abweichungen, gleicher Debug-Haken).

**Befund zu Punkt 3:** Gemessen wurde der Auslieferungsstand (Disc bis auf den Firmenlogo-Film bytegleich;
ausgefuehrter RAM-Code der Kette = PSX.EXE/STAGE1.BIN; kein Save-Patch). Einzige Code-Abweichung im RAM ist
der spieleigene DEBUG.BIN-Haken, den die Auslieferungs-EXE selbst setzt.

## 2. Absturzstelle = Ursache, nicht Folgeschaden

### 2.1 CPU-Block mit eigenem Leser (`gp_ss.py`, Marker `03 00 00 00 "CPU"`, Plausibilitaet r0 = 0 / Ladeverzoegerung 34)
`build/r34g_absturz/vb/vb_ende.sav` (Werte identisch zum Ermittler):
```
at=8011f7c0 v0=00000000 v1=8011ffcc a0=8011feac a1=800ace20  s0=800aca88 s1=5 s2=800ac784 s3=80072bac
k0=19f0f780 gp=8006e548 sp=1f8003a0 fp=801ff400 ra=80106c08   pc=0 npc=80000084 EPC=0 SR=40000401 CAUSE=400
bools=[0,0,0,0,1,0] ld_reg=34 cache_control=0001e988 ; Scratchpad ab sp:
1f8003a0: 800aca88 00000005 800ac784 80072bac | 1f8003b0: 80100590 ...  | 1f8003c0: ... 00000002 8001a590
1f8003f0: 8001ce0c ...
```
Nachrechnung gegen die Instruktionen (§1 Nr. 13-18): Zombie 1 hat im Save +5 = 9, +6 = 1, +9 = 0 ->
v1 = 9<<5 + 0x8011feac = **0x8011ffcc**, Zelladresse = 1<<2 + v1 = 0x8011ffd0, RAM[0x8011ffd0] = **0** (eigener Leser)
-> v0 = 0. at = 0x8011f7b4 + 3*4 = **0x8011f7c0**. ra = Adresse des jalr + 8 = **0x80106c08**. Der Stapel traegt
genau die Rahmen 0x80106ba4 (-24, [sp+16] = 0x80100590), 0x80100424 (-24, [+16] s0 = 2, [+20] 0x8001a590),
0x8001a50c (-40, [+32] 0x8001ce0c). Kein Register traegt Spuren einer spaeteren Funktion: haette das Sprungziel
Code ausgefuehrt, der ra/v0/v1/a0 veraendert oder per Epilog zurueckkehrt, stuenden dort andere Werte.

### 2.2 Eigene Reproduktion (K0) und GEGENPROBE durch Aendern genau eines Wortes (K1, K2)
Werkzeuge `gp_patch.py` (eigener Repacker, Rueckleseprobe + CPU-Block-Vergleich) und `gp_lauf.py` (keine Eingabe,
5 Zwischenstaende im Abstand 1,5 s, grazioeses Schliessen). Basis = Ermittler-Save `p_vb_zombie1_an_granate.sav`;
vorher selbst geprueft: er unterscheidet sich von seiner Quelle `va/va_a_04.sav` in **genau 4 RAM-Bytes**
(0x800ace54/55, 0x800ace5c/5d = Zombie 1 +0x34/+0x3c), CPU-Block identisch; Zombie 1 Typ 0x10, +4..+7 = 01 0c 03 00,
+9 = 00, +0x93 = 00, HP 81; Granate ESP[0] A=31, Zuender 42.

| Lauf | einzige Aenderung | Ergebnis (eigener Leser `gp_zustand.py`, Ausgaben `build/r34g_absturz/gegen/k*_zustand.txt`) |
|---|---|---|
| **K0** Kontrolle | keine | alle 6 Staende: EPC = pc = 0, **RA = 0x80106c08**, v0 = 0, k0 = 0x19f0f780; Granate A=31 **Zuender 6**, Kind ESP[1] unveraendert; Zombie 1 +4..+7 = **03 09 01 00**, +0x93 = 0x81, HP **-919** -> Haenger **unabhaengig reproduziert** |
| **K1** | RAM 0x8011ffd0 := 0x80114e4c (ein `jr ra` / `nop` im selben Overlay, Kraehen-HURT, selbst disassembliert) | alle Staende: EPC in normalem Code (0x80062130 VSync, 0x80025520, 0x800258bc), ESP-Pool leer (Explosion + Kinder abgelaufen), Spieler-Unterzustand wechselt (+6 05 -> 07 -> 05), Zombie 1 bleibt 03 09 01 00 / HP -919 (tut nichts) -> **kein Haenger** |
| **K2** | RAM 0x8011ffd0 := 0x80106c18 (= Zelle [3][1], der normale Pistolen-Tod derselben Tabelle) | Spiel laeuft; Zombie 1 +7 = 1 -> 2, Lage (-5780,-18628) -> (-4934,-19485) -> (-4759,-19660) (Todes-Clip mit Wurzelbewegung), HP -919 -> **Zombie stirbt normal, kein Haenger** |

**Befund zu Punkt 2:** Die gemessene Stelle ist die **Ursache**, kein Folgeschaden. Mit genau einem geaenderten
Wort (die NULL-Zelle [9][1]) verschwindet der Haenger; alle anderen Zustaende (Schaden, +4/+5/+6, Riegel, Kinder)
sind dabei unveraendert. Ein zerstoerter Stapel oder ein spaeterer Zugriff scheidet aus. Das ersetzt den offenen
Punkt O1 (literaler Haltepunkt) durch einen staerkeren, kausalen Beleg.

### 2.3 Was nach dem Sprung auf 0 passiert (Form des "Absturzes")
RAM 0x00000000..0x0f im Absturz-Save (eigener Leser) = `00000003 275a0c80 00000008 00000000` (nop / addiu k0,k0,0xc80 /
**jr zero** / nop) -> Endlosschleife, Interrupts laufen weiter (CAUSE 0x400 = IP2, EPC 0). Zensus des Worts 0x8 ueber
**alle** lesbaren sauberen Saves unter `stage_saves/**` (97, einer nicht lesbar: `boot_16.sav`): **25x 0x03400008**
(Vorspann/Titel/Menue) und **72x 0x00000008** (Spiel) — der Ermittler zaehlte 78 (ohne Unterordner), das Muster ist
dasselbe; der Stub @0x80026e4c ist in allen 97 ungepatcht (`0800e003`). Wer das obere Halbwort nullt, bleibt offen
(O6 des Ermittlers); es bestimmt nur die FORM (Standbild statt wildem Sprung auf k0+0xc80), nicht die Ursache.

## 4. Zensus / Einordnung — eigene Stichproben

### 4.1 Bestaetigt
* Zombie-Familie ST1-5: Typen 0x12/0x16/0x18/0x1c-0x1f teilen die Wurzel von 0x10/0x11 (dasselbe `sw v0`-Band: ST1
  `8011e89c..8011e8cc`, ST2 `80116f14..44`, ST3 `8011cee8..18`, ST4 `801183a8..d8`, ST5 `8011dd08..38`); DEATH-Zelle
  [9][1] = 0 in allen fuenf Stages (§1 Nr. 20).
* Zombie-Maedchen 0x13 ST1: `table 0x80120208` [3] = 0x8010c014; `8010c024: lbu v0,9(a1)` / `andi 0x80` / `8010c038: jal 0x80107cb0`
  (liegend), sonst Tabelle 0x8012063c, `8010c070: jalr v0`; Zeile 9 = [0,0,0,0,0x80107634,0,0,0]; HURT 0x8010bf80 -> 0x8012039c Zeile 9 = 8 x 0.
* Gitterhaende 0x1a ST1: `8010c29c: lbu v0,4(v0)` / Tabelle 0x8012093c [2] = 0x8010d0f8, [3] = 0x8010d474; beide ohne
  Liege-Weiche direkt `lbu v1,5` / `lbu v0,6` -> 0x801209a0 bzw. 0x80120c94, Zeile 9 = 8 x 0 (`jalr` @0x8010d130 / @0x8010d4ac).
  Der Riegel +0x93 Bit 0 wird im 0x1a-Code gesetzt UND geloescht (`8010cf1c` ori 1, `8010cfc4` andi 0xfe, ...) -> treffbar
  moeglich; dynamisch nicht belegt (wie beim Ermittler).
* Gorilla 0x27: `table 0x80121500` [9] = [10] = [11] = **0x8011bb9c**, [7]/[8]/[13] = 0x8011b998, Rest 0x8011b7b8; HURT
  `0x801214a8` [9]/[10] = 0x8011b400. Dynamik vc3 mit eigenem Leser: vorher Gorilla +9 = 0x10, HP 180, Granate Zuender 27;
  danach **+4..+7 = 07 00 00 02, HP -820**, EPC in normalem Code.
* Hund 0x20: DEATH `80110e78: lbu v0,5(v0)` / Basis 0x80121070 / `80110e98: jalr` -> [9] = 0x80110eb0; HURT `801109a8` /
  0x80121018 / `801109c8` -> [9] = 0x80110b9c. Kraehe 0x21: HURT = `80114e4c: jr ra`, DEATH `80114700` / 0x801211cc /
  `80114720` -> [9] = 0x801149c4. Die Ziele 0x80110eb0 / 0x8011b400 verzweigen danach nur ueber +7 (`sltiu v0,v1,0x5` +
  Sprungtabelle) — kein weiterer +5-Tabellensprung.
* Spieler-Eigenschaden (vs, eigener Leser): vs_00 HP 100 / +4 = 1 -> vs_01 **HP -900, +4 = 3, +0x94 = 7**, +0x93 = 1 ->
  vs_ende +4 = 7; EPC normal. Der Spielerzweig setzt bei HP < 0 +5 := 0 (`80012ef8: sb zero,5(s1)`), keine Zeile-9-Tabelle.
* Liegender Briefing-Zombie (vb2/vb3): G0 Typ 0x16, +9 = 0x88; in `p_vb3` +0x93 = 00 (gepatcht), nach der Explosion
  **0x83**, HP 97 unveraendert, Spiel laeuft -> gesperrter Zweig `80012fbc/80012fc0/80012fc4/80012fcc`.
* Wiederholungen vb4 (Typ 0x11, HP -895) und vb6 (Typ 0x10, natuerlich beschaffte Granate, HP -919): eigener Leser ->
  EPC 0, RA 0x80106c08, +4..+7 = 03 09 01 00.
* Item-Debug: `8004a138: lhu a1,-14494(a1)` / `8004a140: andi v0,a1,0x100` / `8004a150: sh v0,9832(at)`; Zustand 2
  `8004a1f0: sb v1,0(at)` (Id), `8004a1f8/204: ori v1,zero,0xff / sb v1,1(at)`; Zustand 3 R1 `8004a238: andi v0,v1,0x8` ->
  `8004a258: addiu v0,v0,1`; Kappung `8004a350: sltiu v0,v0,0x48` / `8004a35c: sb v0(=0x47)`. n1 mit eigenem Leser: Basis
  `psx_inv_grid.sav` Stub ungepatcht, Platz 0 `01 00 00 00` -> n1_sel `00 ff` -> n1_r9 **`09 ff 00 00`** -> n1_zu aca5d = **09**.
* Acid/Incendiary (vd/ve, eigener Leser): aca5d 0x0a/0x0b, Platz 3 Menge **05 -> 04**, Clip 11 (Bild 11/24/38) bzw. 7
  (Bild 12/25), **ESP-Pool in allen 30 Staenden leer**, EPC normal.
* Laerm: alle Zugriffe auf 0x800b5358 in EXE + allen BINs (Wortsuche Immediate 0x5358): Schreiber @0x80017694,
  @0x800180f0, @0x8001857c; Leser nur `8001ce60: lbu v0,21336(v0)` und `8001d170: addiu s0,s0,21336` (EXE, Lichtpfad) —
  kein Overlay-Leser.

### 4.2 Abweichungen / Praezisierungen
* **A1 — K2 ist nicht "unerreichbar".** Die Zombie-HP kommen aus einer Tabelle je Typ x 16 (Zufall `rng & 0xf`):
  ST1 `801007c4: lui a0,0x8012` / `801007c8: addiu a0,a0,-4044` (0x8011f034) / `801007d8: lbu v1,8(a1)` / `801007e0: sll v1,v1,5`
  / `801007ec: lhu v0,0(v0)` / `801007f4: sh v0,154(a1)`. Zeile Typ 0x18 (`read 0x8011f334 16 --w 2`) =
  [71, 93, 75, **1058**, 75, 95, ...] — in ALLEN Stages identisch (Tabellen ST2 0x8011717c, ST3 0x8011d1c8, ST4 0x801185a0,
  ST5 0x8011e108, je Zeile 0x18 Eintrag 3 = 1058). Ein Typ-0x18-Zombie mit Zufallsindex 3 hat nach dem Treffer HP 58
  -> `80013014: bgez` -> +4 = 2 -> HURT 0x80105a8c: stehend (`80105a9c..a8`) direkt `80105ae8..80105b10: jalr v0` ueber
  0x8011fb90 [9][1] = 0 -> **ebenfalls Haenger**. Die Aussage "HP 81..105 -> unerreichbar" stuetzt sich nur auf die
  fuenf Briefing-Zombies. Am Gesamtergebnis aendert das nichts (beide Zellen NULL).
* **A2 — G-Birkin haengt nur in bestimmten Phasen.** Beide Birkin-Handler pruefen VOR der 2D-Tabelle eigene Felder:
  HURT ST3 0x8011a060: `8011a0a8: lbu v0,7(a2)` / `8011a0b0: bne` (nur +7 = 0 -> Blut + Zaehler), dann
  `8011a1e8: lbu a0,476(v0)` / `8011a1f0: addiu v1,a0,255` / `8011a1f4: beq a0,zero,0x8011a214` / `8011a1f8: sb v1,476(v0)`
  -> ist der Zaehler +0x1dc != 0, wird er nur heruntergezaehlt und `8011a208: lw v0,472(v1)` / `8011a210: sw v0,4(v1)`
  stellt den vorherigen Zustand wieder her — **kein Tabellensprung**. Erst bei +0x1dc = 0: `8011a260: jalr v0` ueber
  0x8011ef44 [9][1] = 0. DEATH ST3 0x8011a3f0: `8011a444: lbu v0,477(a2)` / `8011a44c: andi v0,v0,0x8` / `8011a450: beq`
  -> ist +0x1dd Bit 3 gesetzt: `8011a534: sh v0(=0x32),154(v1)` (HP := 50), `8011a54c: andi v0,v0,0xfe` (+0x93 frei),
  `8011a568: sw v0,4(v1)` (Zustand zurueck) — **kein Tabellensprung**; nur sonst `8011a5b8: jalr v0` ueber 0x8011f1e4
  [9][1] = 0. ST5 gleich (`8011a8bc` / `8011a9fc..aa24` bzw. `8011ac48` / `8011ac58..64` / `8011ad74..7c`). Schreiber (ST3):
  +0x1dc := 9 @0x80117fb8, := Zufall+3 @0x8011a31c; +0x1dd Bit 3 gesetzt @0x801180b4, geloescht @0x801188c4.
  -> K5 ist "statisch moeglich, phasenabhaengig", nicht "statisch sicher". Praktisch entscheidet DEATH: Init-HP der
  ST3-Birkin = 300 (`8011690c: ori v0,zero,0x12c` / `80116910: sh v0,154(v1)`), in einer spaeteren Phase 150 mit
  gesetztem Schutzbit (`80118094: ori v0,zero,0x96` / `80118098: sh` / `801180b0..b4: ori 0x8` auf +0x1dd) -> 1000
  Schaden ergibt immer HP < 0 -> +4 = 3; Haenger nur, solange +0x1dd Bit 3 = 0 (bzw. +7 != 0).
* **A3 — Efeu 0x2d (O4) ist aufloesbar: kein Granaten-Absturz.** Die Wurzel 0x801168c4 springt ueber
  `801168e8: lbu v0,4(a0)` / `801168f8: addiu at,at,-23872` (0x8011a2c0) / `80116908: jalr v0`, und dort stehen fuer
  [2]/[3] tatsaechlich die Datenworte 0xfa060000 / 0x01c20000 (Bytes `@0x8011a2c8 = 00 00 06 fa 00 00 c2 01`) — ein
  Treffer mit +4 = 2/3 waere ein echter Absturz (Sprung in ungueltigen Speicher). ABER der Init-Zustand
  0x80116920 setzt `8011693c: ori v0,zero,0x3` / `80116944: sb v0,147(a1)` (+0x93 = 3, Bit 0 = Riegel), und im
  Efeu-Code gibt es keinen Loescher (einziger `sb ...,147` im Bereich 0x801168c4..0x80116be0; der naechste,
  `80116e34: sb zero,147(v0)`, gehoert zur Tabelle @0x8011a2d8 eines anderen Typs). Der Resolver ueberspringt
  gesperrte Gegner (`80012fbc/80012fc0`, +0x93 &= 1 laesst Bit 0 stehen) -> die Granate setzt +4 beim Efeu nie.
  (Einschraenkung: EXE-Schreiber `800421e8: sb zero,147(s0)` gehoert zu einem Entity-Reset, der auch +4 := 0 setzt,
  danach laeuft wieder der Init.) Zuordnung selbst geprueft: die NPC-Wurzel 0x80116be4 (Typ 0x40 ST4) springt
  `80116c70: lbu v0,4(v0)` / `80116c80: addiu at,at,-23848` ueber 0x8011a2d8, [0] = 0x80116d20 -> darin liegt 0x80116e34.
* **A4 — Parken und Maske fehlen in der Bedingung.** Die Zombie-Wurzel tickt nicht, solange `8010045c` (+9 & 0x20,
  geparkt) oder `8010043c` (g_pauseflags & 0x20000000) greift; der Resolver prueft das Parken NICHT (nur +0 & 1 und die
  Trefferbox). Ein geparkter Zombie im Radius bekommt also +4/+5/+6 = 3/9/1 und haengt das Spiel erst, wenn er
  entparkt wird (wie beim geparkten Gorilla vc2 zu sehen: getroffen, aber "ohne Tick keine Reaktion").
  Ausserdem ueberspringt der Resolver Gegner mit `+0x90 & 0x03000000 == 0x03000000` (`80012f54..60`) und den
  Eigentuemer (`80012f40..4c`); beides fehlt in der Bedingung des Ermittlers (fuer die Briefing-Zombies nicht wirksam,
  dort gemessen).
* **A5 — "RAM-Code == PSX.EXE" gilt nicht woertlich** (siehe §3.2: DEBUG.BIN-Haken @0x80013b7c, von der
  Auslieferungs-EXE selbst gesetzt @0x80013144/0x8001314c). Kein Mod, fuer die Kette belanglos.
* **A6 — Wortlaut "RA-Kette: 0x80106c08 @sp".** 0x80106c08 steht im Register ra, nicht im Stapel; an sp (0x1f8003a0)
  steht 0x800aca88 (Argumentbereich). Die gespeicherten Ruecksprungadressen liegen bei sp+16 (0x80100590), 0x1f8003cc
  (0x8001a590), 0x1f8003f0 (0x8001ce0c) — so auch im Dossier §2.7 korrekt; nur die Kurzfassung ist ungenau.
* Zombie-Maedchen 0x13 in ST2 zusaetzlich selbst: Wurzel 0x8010a75c `8010a8a0: lbu v0,4(v0)` / 0x8011830c [3] = 0x8010bea8,
  `8010beb8: lbu v0,9(a1)` (Liege-Weiche `8010becc: jal 0x80107b44`), Tabelle 0x80118740, `8010bf04: jalr v0`,
  Zelle 0x80118864 = 0. Registrierung von 0x13 in ST1-5 vorhanden (`sw …,11256(at)` = 0x80072bf8 @ST1 0x8011e87c,
  ST2 0x80116fe4, ST3 0x8011cfc0, ST4 0x80118478, ST5 0x8011dde8); 0x1a nur ST1 (0x8011e9ac); 0x30 ST3/ST5, 0x36 ST3.

### 2.4 Einfrieren belegt (Nachtrag zu §2.2)
RAM-Vergleich erster gegen letzten Zwischenstand (~7 s):
* K0 (Haenger): **4 Byte** verschieden (0x80007264 Kernel, 0x800787dc/dd, 0x8008fd24 — von den weiterlaufenden
  Interrupts), Scratchpad und alle Register identisch -> das Spiel steht, nur die Interrupt-Zaehler laufen.
* K1 (Zelle gefuellt): **10 063 Byte** verschieden, Register und Scratchpad verschieden -> das Spiel laeuft.

## 5. Punkt 4 — Einordnung und die Schadensfrage

| Teil | Ermittler | Gegenpruefung |
|---|---|---|
| Beschaffung 0x09/0x0A/0x0B | laeuft nur ueber Item-Debug | bestaetigt (statisch + n1 mit eigenem Leser) |
| Ausruesten, Wurf, Munition −1, Spawn 0x040D1000 | laeuft | bestaetigt (Code §1; va/vd/ve-Saves) |
| Flug/Abprall/Liegen/Zuender (R30 -> B := 29 `8001846c/70`, Zuender := 42 `80018474/7c`; R29 -> A := 31 `80018378/7c`) | laeuft | bestaetigt |
| Explosionsbild, Kinder, SEs, Licht-Latch | laeuft | bestaetigt (K1: Kinder laufen ab) |
| Laerm fuer Gegner | kein Mechanismus | bestaetigt (kein Overlay-Leser von 0x800b5358) |
| Schaden Spieler 1000 | laeuft (Tod) | bestaetigt (vs) |
| Schaden Gegner 1000 (+4/+5/+6) | laeuft | bestaetigt (wird im Explosionsbild IMMER geschrieben; spielbare Folge nur bei Familien mit gueltiger Zeile 9) |
| Reaktion Zombies 0x10-Familie, 0x13, 0x1a | kaputt (Haenger) | bestaetigt; zusaetzlich auch ueber HURT (A1) |
| Reaktion G-Birkin 0x30/0x36 | kaputt (Haenger) | **nur phasenabhaengig** (A2) |
| Reaktion liegend/gesperrt | laeuft (keine Wirkung) | bestaetigt (vb2/vb3) |
| Reaktion Gorilla 0x27 | laeuft (eigener Granaten-Tod) | bestaetigt (Tabelle + vc3) |
| Reaktion Hund/Kraehe/Feuer/Spinne/Alligator/Schabe/Tyrant/NPC | laeuft (statisch) | Stichproben Hund, Kraehe, Spinne, Alligator, Tyrant bestaetigt (gueltige Zellen, Index-Register geprueft) |
| Efeu 0x2d | OFFEN (O4) | **kein Granaten-Absturz** (dauerhaft gesperrt, A3) |
| Acid 0x0A / Incendiary 0x0B | unfertig, kein Projektil, kein Absturz | bestaetigt (Gate `8003368c`; vd/ve ESP-Pool leer; keine Aufrufer fuer Art 3/4) |

**Die Schadensfrage ("ist der Schaden am Gegner/Spieler im Original wirklich nie ohne Absturz anwendbar?"): NEIN — er
ist anwendbar.** DAT_8006f418[2] = 1000 und DAT_8006f430[2] = 9 werden im Explosionsbild bei jedem ungesperrten Treffer
geschrieben (`80012fe8..80013020`). Ohne Absturz spielbar ist das beim Spieler (Tod, vs) und beim Gorilla (eigener
Granaten-Tod ueber Zelle [9], vc3), statisch bei allen 1D-Familien. Nur bei den 2D-Familien (Zombies, Zombie-Maedchen,
Gitterhaende, G-Birkin in ungeschuetzter Phase) folgt im naechsten Bild der Haenger. K2 zeigt zusaetzlich: traegt die
Zelle [9][1] einen normalen Todes-Handler, stirbt der Zombie mit genau diesem Schaden sauber — der Schadenswert selbst
ist nicht die Ursache.

## 6. Urteil

**bestaetigt** — Reproduktion (4. Mal, selbst: K0), abstuerzende Instruktion (`80106c00: jalr v0`, v0 aus Zelle
0x8011ffd0 = 0 in STAGE1.BIN und im RAM), Kette, Register/RA-Kette, Tabellenwerte und Auslieferungsstand halten der
Nachpruefung stand; die Ursache ist durch die Ein-Wort-Gegenprobe K1/K2 kausal belegt. Abweichungen A1-A6 sind
Praezisierungen der Bedingung/des Zensus, keine Widerlegung.

Offen (nicht Teil des Urteils): natuerlicher Treffer ohne Lage-Patch (O5 des Ermittlers; die Granate selbst ist in vb6
natuerlich, der Nutzer hat den natuerlichen Fall beobachtet); Schreiber des Halbworts 0x0000000a (O6; im Titel noch
0x0340, im Spiel 0x0000 — nur 0xa..0xb der Garbage Area aendern sich zwischen `mzd_title` und `mzd_debugmenu`; keine
Speicherung mit Basisregister `zero` auf 0x8..0xb in EXE/Overlays -> vermutlich Schreiben ueber einen NULL-Zeiger,
nur per Schreib-Haltepunkt bestimmbar); dynamische Belege fuer 0x13/0x1a/G-Birkin.

Werkzeuge: `re_absturz_gegen_werkzeug/gp_ss.py` (Leser), `gp_patch.py` (Patcher), `gp_lauf.py` (Laeufer),
`gp_zustand.py` (Kurzbild), `gp_vdve.py` (vd/ve), `ram_vs_disc.py`, `iso_check.py`. Laufzeit: `build/r34g_absturz/gegen/` (k0/k1/k2,
`iso_check_mzd.txt`, `ram_vs_disc_vb_ende.txt`, `k*_zustand.txt`).
