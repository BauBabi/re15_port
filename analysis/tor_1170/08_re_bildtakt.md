# 08 — Bildtakt der Türsequenz (RE2) gegen den Spieltakt, RE1.5, Port

Stand 2026-09-28. Vorarbeit: `03_tuersequenz.md` §3.3 (+ Skeptiker N2/N3), `05_port_anschluss.md` §1.1.
Prüfskripte: `build/tor_1170/re_bildtakt/` (Aufrufe am Ende). Alle Adressen hier selbst disassembliert
mit `.claude/skills/re15-psx-disasm/scripts/re2_disasm.py` bzw. `re15_disasm.py`, Sprungziele mitgelesen.

## 0. Ergebnis

| | RE2 Retail | RE1.5 | Port heute |
|---|---|---|---|
| Takt-Stelle | Bildwechsel-Routine `FUN_8002b968`: `DrawSync(0)` `@0x8002b988`, `VSync([0x800dfc1a])` `@0x8002b994/98` | `FUN_8002137c`: `DrawSync(0)` `@0x8002138c`, `VSync([0x800b5456])` `@0x8002147c/80` | `main.c:3140` `target_fps = 30`, `main.c:3148` `1000/30` = 33 ms, Deckel `main.c:10066-10074` |
| Spiel | `VSync(2)` = 2 Austastungen je Bild = **29,913 Bilder/s** | `VSync(2)`, gemessen in 75 von 78 sauberen Savestates | 33 ms = 30,30 Bilder/s |
| Türsequenz | `VSync(0)` = 1 Austastung je Bild = **59,826 Bilder/s** | `VSync(0)` (Aufbau schreibt 0 `@0x80016208`) | keine Türszene |
| 291 Bilder (DOOR2E) | **4,864 s** | – | bei 33 ms: 9,603 s (doppelt so lang) |

NTSC-Wert aus psx-spx `graphicsprocessingunitgpu.md:1262-1264`: "Non-interlaced: 59.826 Hz". Beide
Spiele zeigen 320×240 ohne Zeilensprung (§5.2). Alle Zahlen setzen voraus, dass die Arbeit eines Bildes in
1 (Tür) bzw. 2 (Spiel) Austastungen passt; das ist nicht dynamisch gemessen (Offen 1).

---

## 1. `0x80085ea0` ist PsyQ-`VSync` (RE2)

### 1.1 Zugriffe und Rückgabe (selbst gelesen)

| Adresse | Instruktion | Bedeutung |
|---|---|---|
| `0x80085ea4` | `lw v0,-17444(v0)` = `[0x800abbdc]` | Zeiger, Wert `0x1f801814` (`read 0x800abbdc 2` -> 528488468) = **GPUSTAT** |
| `0x80085eac` | `lw v1,-17440(v1)` = `[0x800abbe0]` | Zeiger, Wert `0x1f801110` (528486672) = **Timer 1 Zählwert** |
| `0x80085ec0` | `lw s0,0(v0)` | GPUSTAT lesen |
| `0x80085ec4..d4` | `lw v0,0(v1)` / `lw v1,[0x800abbe4]` / `subu` | Timer1 minus Basis |
| `0x80085ed8/dc` | `bgez a0,…` / Delay-Slot `andi s1,v0,0xffff` | s1 = Zeilen seit letztem VSync (16 bit) |
| `0x80085ee4/e8` | `lw v0,[0x800acd00]` / `j 0x80085fd0` | **Modus < 0: Rückgabe Vcount**, kein Warten |
| `0x80085ef0..f8` | `addiu v0,zero,1` / `beq a0,v0,0x80085fd0` / DS `addu v0,s1,zero` | **Modus 1: Rückgabe s1**, kein Warten |
| `0x80085efc` | `blez a0,0x80085f1c` | Modus 0 -> `0x80085f1c` |
| `0x80085f04..18` | `lw v0,[0x800abbe8]` / `addiu v0,v0,-1` / `j` / DS `addu v0,v0,a0` | Modus n>0: Ziel = letzt − 1 + n |
| `0x80085f1c/20` | `lw v0,[0x800abbe8]` | Modus 0: Ziel = letzt |
| `0x80085f24..2c` | `blez a0,0x80085f30` / DS `addu a1,zero,zero` / `addiu a1,a0,-1` | Timeout-Faktor a1 = 0 (Modus 0) bzw. n − 1 |
| `0x80085f30/34` | `jal 0x80085fe8` / DS `addu a0,v0,zero` | **v_wait(Ziel, a1)** |
| `0x80085f44` | `lw s0,0(v0)` | GPUSTAT erneut |
| `0x80085f4c..58` | `lw a0,[0x800acd00]` / `addiu a1,zero,1` / `jal 0x80085fe8` / DS `addiu a0,a0,1` | **v_wait(Vcount + 1, 1)** = auf die nächste Austastung warten |
| `0x80085f5c..a0` | `lui v0,0x40` / `and v0,s0,v0` / … `lui a0,0x8000` / Schleife bis Bit 31 wechselt | nur bei GPUSTAT Bit 22 (Zeilensprung): auf Halbbildwechsel warten |
| `0x80085fa8..cc` | `sw v0,[0x800abbe8]` (letzt = Vcount), `sw v1,[0x800abbe4]` (Basis = Timer1) | Zustand für den nächsten Aufruf |
| `0x80085fc4` | `addu v0,s1,zero` | Rückgabe s1 |

`v_wait` `0x80085fe8`: `sll a1,a1,15` (`@0x80085fec`), Schleife `lw v0,[0x800acd00]` / `slt v0,v0,a0`
(`@0x80085ff8/80086000`, `@0x80086060/68`), Zähler −1 je Runde (`@0x80086018`); bei −1: `a0 = 0x80011f14`
(Bytes `56 53 79 6e 63 3a 20 74 69 6d 65 6f 75 74 0a 00` = "VSync: timeout\n"), `jal 0x8009881c`,
`jal 0x80095834(0)`, `jal 0x80095854(3,0)` (`@0x80086030..50`).

Vcount `0x800acd00` wird nur im VBlank-Handler erhöht: `startIntrVSync` `@0x800867b0` schreibt 0x107 in
`[0x800acd04]` (Timer-1-Modus), `@0x800867b8 sw zero` (Vcount = 0), `@0x800867cc jal 0x800860b4` mit a0 = 0,
a1 = `0x800867ec`. `0x800860b4` springt über `[0x800acccc]` (= `0x800accac`) +8 = `0x800864e0`; dort
`@0x800864f8..0x80086500` Slot `0x800abc48 + 4·a0` und `@0x80086550..54` Maskenbit `1 << a0` -> IRQ 0 =
VBLANK (psx-spx `interrupts.md:8`). Handler `0x800867ec`: `@0x80086810 addiu v0,v0,1`, `@0x80086818 sw v0,[0x800acd00]`.
Ghidra-Dump (PsyQ-Signaturen) benennt dieselben Adressen `VSync`, `v_wait`, `startIntrVSync`, `trapIntrVSync`,
`InterruptCallback` (`ghidra_re2_Leon.txt:242121`, `:243005`, `:243036`, `:242345`).

### 1.2 Vergleich mit der PsyQ-Bibliothek

- Handbuch `Psy-Q_47/DOCS/LibRef47.pdf` (Text: `pdftotext`, Eintrag "VSync", S. 7-117): Modus 0 "Blocks until
  vertical sync is generated"; n (n>1) "Blocks from the point VSync() processing is last completed … until n
  number of vertical syncs are generated"; −n: Zeit seit Start in Austastungen; 1: Zeit seit dem letzten
  VSync in Zeilen.
- `psyq-4.7-converted-full/lib/libetc/vsync.o`: Relokationen nennen `Vcount`, `puts`, `ChangeClearPAD`,
  `ChangeClearRCnt`, `.rdata` = "VSync: timeout\n". Modusweiche `.text+0x058..0x124` und `v_wait`
  `.text+0x178..0x20c` haben dieselben Befehle, Konstanten (`0x40`<<16, `0x8000`<<16, `sll a1,a1,15`) und
  relativen Branch-Abstände wie RE2 `0x80085ed8..0x80085fa8` / `0x80085fe8..0x80086080`.
- **Nicht wortgleich**: keine der vier Bibliotheken im Repo ist der Stand, den RE2 gelinkt hat. Längster
  Präfix-Treffer 7 von 52 Worten (`vsync_lib_suche.py`), weil 4.7 Delay-Slots anders füllt (z. B. 4.7
  `+0x074 beq … / +0x078 nop` gegen RE2 `@0x80085ef4 beq / @0x80085ef8 addu v0,s1,zero`) und zusätzlich
  Timer 1 stabil liest (`+0x024..0x03c`, `+0x13c..0x158`). Die Gleichheit ist also Befehl-für-Befehl-Semantik,
  nicht Byte-Gleichheit.

### 1.3 Parameter 0 gegen 2 (aus den Instruktionen oben)

- **VSync(0)**: `v_wait(letzt, 0)` kehrt sofort zurück (Vcount ≥ letzt, `@0x80086000/04`), dann
  `v_wait(Vcount+1, 1)`: wartet auf die **nächste** Austastung nach dem Aufruf. Periode = Arbeitszeit, auf
  eine Austastung aufgerundet; bei Arbeit < 1 Austastung genau 1.
- **VSync(2)**: `v_wait(letzt+1, 1)` wartet, bis seit dem Ende des letzten VSync mindestens 1 Austastung
  vergangen ist, dann `v_wait(Vcount+1, 1)` noch eine. Mindestens 2 Austastungen zwischen zwei Rückkehren;
  bei Arbeit zwischen 2 und 3 Austastungen werden es 3.
- Zusatzbeleg, dass das Spiel Modus 0 als doppelte Rate behandelt: `FUN_8003022c` (Hauptschleife
  `@0x8002b440`) liest das Modusbyte `@0x800309d0` und prüft einen Zähler bei Modus ≠ 0 mit `andi 0x18`
  (`@0x800309f4`), bei Modus 0 mit `andi 0x30` (`@0x800309e0`), also doppelt so viele Bilder je Periode.

---

## 2. Aufrufkette in RE2 und weitere Bremsen

### 2.1 Hauptschleife -> Task -> Bildwechsel

Hauptschleife `main`, Kopf `0x8002af20`, Rücksprung `@0x8002b460 j 0x8002af20`. Register: `@0x8002af08..10`
s0 = `0x800cc1e8`, s3 = s0 + 14732 = **`0x800cfb74`** (Status), `@0x8002af14..1c` s1 = `0x800dfae0`, s2 = `0x800dfc1b`.

| Schritt | Adresse | Inhalt |
|---|---|---|
| Task-Lauf | `@0x8002b320 jal 0x80031d8c` | 3 Tasks ab `0x800d76a4`, Schritt 128 (`@0x80031d98`, `@0x80031e4c`, Ende `0x800d7824` `@0x80031e50..5c`). Zustand 1: Zähler +2 −1 (`@0x80031e20..34`), bei 0 `ChangeTh` (`@0x80031e44 jal 0x80095764`) |
| Weiche | `@0x8002b33c..48` | `[s3] & 0x10000` = 0 -> Normalzweig `0x8002b440`, sonst Ladezweig |
| Normalzweig | `@0x8002b440 jal 0x8003022c`, `@0x8002b448 jal 0x8002c378` (Blende), **`@0x8002b450 jal 0x8002b968`**, `@0x8002b458 jal 0x80012984` | |
| Bildwechsel | `0x8002b968`: `@0x8002b96c addu a0,zero,zero` / `@0x8002b988 jal 0x800903a0` (**DrawSync(0)**, String `0x8001257c` "DrawSync(%d)...\n"), `@0x8002b994 lbu a0,[0x800dfc1a]` / **`@0x8002b998 jal 0x80085ea0`**, dann `PutDrawEnv` `@0x8002b9b0`, `PutDispEnv` `@0x8002b9c0` | |
| Ladezweig | `@0x8002b350..0x8002b438`: `DrawSync(0)` `@0x8002b41c`, **`VSync(0)` `@0x8002b424/28` (Literal)**, `@0x8002b438 j 0x8002af20` | läuft ebenfalls mit 1 Austastung |

Task-Schlaf `0x80031f94`: `@0x80031fb0 sh a0,2(v0)` (Zähler), `@0x80031fb4 sh v1,0(v0)` (Zustand 1),
`@0x80031fcc jal 0x80095764` mit a0 = `0xff000000`; `0x80095764` = `addiu t2,zero,176` / `jr t2` /
`addiu t1,zero,16` = BIOS B(10h) ChangeTh -> zurück in die Hauptschleife. Mit Argument 1 läuft die Task im
nächsten Task-Lauf wieder (1 − 1 = 0, `@0x80031e34 bgtz` nicht genommen).

**Also: ein Door_move-Durchlauf = ein Hauptschleifen-Durchlauf = genau ein `VSync`.**
Door_move (`0x80013eb4`): wartet per Task-Schlaf, solange `[0x800cfb74] & 0x10000` (`@0x80013ed0..f8`,
in dieser Zeit läuft der Ladezweig mit `VSync(0)`), schreibt dann `sb zero,[0x800dfc1a]` **`@0x80013f24`**
(übersprungen nur bei Objekt-0-Flag 0x8000, `@0x80013f0c..18`), Schleife: Scheduler `@0x80013f6c`,
Objekte `@0x80013f74`, Bildzähler `@0x8001401c/24`, **`@0x80014020 jal 0x80031f94`** mit `a0 = 1`
(`@0x80014018 addiu a0,zero,1`, Delay-Slot `@0x80014024` ist der Zähler-Store), Bedingung `@0x8001402c..34`. Door_exit (`0x8001417c`): Task-Schlaf bis Blende Kanal 0
fertig (`@0x80014184..9c`), dann **`@0x8001420c addiu v1,zero,2` / `@0x80014214 sb v1,[0x800dfc1a]`**,
Bit 0x2000000 löschen (`@0x80014218/20`), `@0x8001421c jal 0x80031fe4`.

### 2.2 Alle 34 VSync-Aufrufe in RE2 (`vsync_aufrufer.py`)

| Argument | Stellen | Wirkung auf die Türsequenz |
|---|---|---|
| −1 (Zähler lesen, wartet nicht) | 20 Stellen: libcd `0x800879e0…0x80088814` (darunter `0x80087c64`, a0 = −1 `@0x80087c48`), `0x8008c2e0…0x8008c63c`, `0x80092cd4/d08`, `0x80033690`, `0x800342a4`, `0x80052a1c` (a0 = −1 `@0x80052a00`) | keine |
| 1 (Zeilen lesen, wartet nicht) | `0x80019168`, `0x800191e8` | keine |
| `[0x800dfc1a]` | `0x8002b998` Bildwechsel | **Taktgeber** |
| 0 Ladezweig | `0x8002b424` | Vorlauf vor Door_move, 1 Austastung |
| 0 Pause | `0x8002b1a8`; Zweig nur, wenn `[s3] & 0x2202200` = 0 (`@0x8002b044..50`) — das Tür-Bit 0x2000000 schließt ihn aus | keine |
| 0 Reset-Zweig | `0x8002b1ec` (Bit 0x1000, `@0x8002afd4..e0`) | keine |
| 0 XA | `0x80012ce8` in Zustand 1 von `0x8009a418` (`@0x80012cd8..d14`, wartet bis `DsQueueLen` = 0), nur wenn `[0x800d5334]` = 1 (`@0x8001298c..94`); gesetzt `@0x80012b48` in `FUN_800129b4`, Aufrufer SCD-Opcode 0x59 (`table 0x800a762c` -> `0x80058028`, `@0x80058048`) und `0x800338a4` | Opcode 0x59 kommt in 11047 Türskript-Befehlen nicht vor (`tuerskript_dump.py stat`); `0x800338a4` nicht verfolgt (Offen 3) |
| 0 CD-Lader | `0x80013258` in `FUN_80012fb8`: nur wenn s2 = 0 (`@0x80013240`), sonst Task-Schlaf `@0x80013248`. s2 = a2, bei a2 ≥ 2 s2 = a2 − 2 (`@0x80012ff4`, `@0x80013054`). Ton laden `@0x80014d84 addiu a2,zero,3`, DO2 laden `@0x80015114 addiu a2,zero,3` -> s2 = 1 | **gibt pro Bild ab, bremst nicht** |
| 2 | `0x8003271c` in `FUN_80032340`, aufgerufen vom Bildwechsel `@0x8002ba28` nur bei `[0x800cfb74] & 0x200` (`@0x8002ba10..20`); Bit 0x200 gesetzt `@0x8003230c/24` in `FUN_80032150`, Aufrufer SCD-Opcode 0x6f (`@0x80058d80`) | 0x6f kommt in Türskripten nicht vor |
| 0 / 3 / 5 | `0x80032ebc`, `0x80033010`, `0x80033470`, `0x800334bc`, `0x80033530` (`FUN_80032ce8`, `FUN_80032fcc`, `FUN_800333c4`) | Aufrufer `0x800325c4/0x80032660/0x80032858/0x800324e4…` im Umfeld von `FUN_80032340`; nicht einzeln verfolgt (Offen 3) |

`DrawSync(0)` (`@0x8002b988`) wartet auf das Ende der GPU-Liste des Vorbilds, nicht auf eine Austastung.
Einen Bildzähler, der den Takt drosselt, gibt es auf dem Türpfad nicht: der Door_move-Zähler
`+558` von `[0x800c3a80]` (`@0x80014008..24`) wird nur hochgezählt und für die Textzeilen verglichen (03 §3.3).

---

## 3. Zahlen

| | Austastungen je Bild | Bilder/s (NTSC, 59,826 Hz) | ms je Bild |
|---|---|---|---|
| Türsequenz `VSync(0)` | 1 | **59,826** | 16,715 |
| Spiel `VSync(2)` | 2 | **29,913** | 33,430 |

DOOR2E: 291 Door_move-Durchläufe (Zahl aus `04_tuerkatalog.md:12`, Simulator, hier nicht nachgezählt)
-> **291 / 59,826 = 4,864 s**. Drehung 80 Bilder = 1,337 s; Einblenden 15 Bilder = 0,251 s,
Ausblenden 32 Bilder = 0,535 s (Bildzahlen aus 03 §3.4). Dieselben 291 Bilder mit Spieltakt: 9,728 s.
(Die aus psx-spx `:1297/1306` rechenbare 53,693182 MHz / (3413·263) = 59,817 Hz ändert das um < 1 ms.)

---

## 4. RE1.5: dieselbe Mechanik

| Schritt | Adresse |
|---|---|
| VSync | `0x80061fc0`, `v_wait` `0x80062108`; Zeiger `[0x800787e4]` = `0x1f801814`, `[0x800787e8]` = `0x1f801110`; Timeout-String `0x8001178c` = "VSync: timeout\n"; Vcount `0x800787dc` (= 0 `@0x80061ebc`, +1 `@0x80061f1c` im Handler `0x80061ef0`; eingetragen über `0x80061510` mit a0 = 0 `@0x80061ed0/d4`, `0x80061510` springt wie RE2 `0x800860b4` über `[0x80078780]`+8 `@0x80061514..28`) |
| Vergleich zu RE2 | `vsync_re2_re15_diff.py`: 4 von 121 Instruktionen abweichend nach Normalisierung: (a) `@0x80061fe8 jal 0x8007d904` statt `lui/lw v1,[Basis]`; (b) `@0x8006207c lui v0,0x8` (GPUSTAT Bit 19) statt `lui v0,0x40` (Bit 22); (c) String-Adresse. Modusweiche und v_wait gleich |
| Einschub `0x8007d904` | liest die Basis nach (`@0x8007d904/08 lw v1,[0x800787ec]` = was RE2 an der Stelle lädt), setzt Byte `[0x800aca5d]` 0x0c -> 0x13 (`@0x8007d930..40`) und 0x0d -> 0x08 (`@0x8007d950..60`), `jr ra` `@0x8007d978`. Keine Warteschleife |
| Bildwechsel | `0x8002137c`: `@0x8002138c jal 0x80068a60` = **DrawSync(0)** (String `0x80011b58` "DrawSync(%d)...\n"), `@0x800213a0 j 0x80021474` (springt über `0x800213a8..0x80021470`), `@0x80021474..7c lbu a0,[0x800b5456]`, **`@0x80021480 jal 0x80061fc0`** |
| Hauptschleife | Kopf `0x80020c10`, s0 = `0x800aca38` (`@0x80020bf0/f4`), s2 = `0x800b5464` (`@0x80020c08/0c`). Task-Lauf `@0x80020de4 jal 0x800298b0` (Tasks ab `0x800b2924`, Schritt 128, Zustand 1 = Zähler −1 `@0x80029944..58`). Weiche `@0x80020dec..f8` Bit 0x10000. Normal: `@0x80020f4c jal 0x8002137c`. Ladezweig: `DrawSync(0)` `@0x80020ee0`, **`VSync(0)` `@0x80020ee8/ec` (Literal)** |
| Task-Schlaf | `0x80029ac8`: `@0x80029adc sh a0,2(v1)`, `@0x80029ae8 sh v0,0(v1)` (1), `@0x80029ae4 jal 0x8006e3c8` = `addiu t2,zero,176 / jr t2 / addiu t1,zero,16` (ChangeTh) |
| Tür schreibt 0 | **im Aufbau** `0x800161e0`: `@0x80016204/08 sb zero,[0x800b5456]` — nicht in der Schleife `0x800164c8` (die Aufgabe nannte Door_move; die Schleife `@0x800164c8..504` schreibt das Byte nicht) |
| Tür schreibt 2 | Abbau `0x80016664`: nach Blende fertig (`@0x8001666c..84`), `@0x8001669c ori v1,zero,0x2` / `@0x800166a4 sb v1,[0x800b5456]` (fehlt in der Ghidra-Xref-Liste `ghidra1_V2.txt:512005`) |
| Türschleife | `@0x800164dc jal 0x80016518` (Skripte), `@0x800164e4 jal 0x800166c4` (Zeichnen), `@0x800164ec jal 0x80029ac8` DS `a0 = 1` |
| weitere Schreiber | `@0x80020d10`, `@0x80021314`, `@0x80026624`, `@0x80046718` schreiben 2; `@0x8002650c`, `@0x800460e0` schreiben 0 (Ghidra-Xrefs + Instruktionen davor gelesen) |
| Debug-Pause | `@0x80020d6c..0x80020dd4`: nur bei `[0x800aca38] & 0x800000`, `VSync(0)` `@0x80020da4` — nicht auf dem Türpfad |

Ergebnis RE1.5: gleiche Routine, gleiches Prinzip. Die Tür-Task läuft mit `VSync(0)`, das Spiel mit `VSync(2)`.
Unterschied: RE1.5 setzt 0 schon im Aufbau, RE2 erst nach dem Warten auf Bit 0x10000 in Door_move.

---

## 5. Messung an RE1.5-Savestates

### 5.1 Modusbyte (`savestate_vsync.py`, 85 Dateien, davon `boot_16.sav` ohne RAM-Signatur)

- 78 saubere Stände: **75 × Modus 2**, 3 × Modus 0: `boot_20.sav` (vor der Initialisierung, Status 0,
  `disp.h` 0), `mzd_inv_open.sav` (Inventar offen), `sub_loadgame.sav` (Laden-Menü).
- 6 gepatchte Stände (`@0x80026e4c` ≠ `08 00 e0 03`) ebenfalls Modus 2, nicht gewertet.
- Keiner der Stände liegt in einer Tür: Bit 0x2000000 = 0 und `[0x800b39ad]` = 0 in allen, auch in
  `doorA_square.sav`, `doorB_walkin.sav`, `doorB2_walkin.sav`. Der Tür-Modus 0 ist daher **nicht gemessen**.

### 5.2 Anzeige

In allen initialisierten Ständen `DISPENV[0/1].isinter` = 0 (`0x800b5438`, `0x800b544c`) und `disp.h` = 240
(`0x800b542e`). Statisch: RE1.5 `@0x80020fac ori s0,zero,0xf0` -> `SetDefDispEnv(…,0,240,320,h=240)`
`@0x80021034`; RE2 `@0x8002b4bc addiu s0,zero,240`, `@0x8002b518/1c/20`; RE2-`SetDefDispEnv` `0x8008f7b0`
schreibt `sb zero,16(v0)` (isinter) `@0x8008f7d8`, h aus dem Stack `@0x8008f7e8`. Der Halbbild-Warteteil
von VSync greift nur bei GPUSTAT Bit 22 (RE2) bzw. 19 (RE1.5, laut psx-spx `:851` nur mit Bit 22 gesetzt).

---

## 6. Port

### 6.1 Heute (Datei:Zeile, gelesen)

| Stelle | Inhalt |
|---|---|
| `re15_port/platform/pc/main.c:3140` | `int target_fps = 30;` (per `RE15_FPS` 15..240 überschreibbar, `:3142-3146`) |
| `main.c:3148` | `frame_budget_ms = (uint32_t)(1000 / target_fps)` = **33 ms** -> 30,30 Bilder/s |
| `main.c:10066-10074` | Hauptschleife: `SDL_Delay(frame_budget_ms - elapsed)` über `SDL_GetTicks()` (1 ms Raster) |
| `main.c:3158` | `FRAME_AT_60(n) = n * target_fps / 60` — halbiert 60-Hz-Zählungen bei 30 fps |
| `main.c:2019` | `pc_run_player_select`, eigene Schleife: `:2121 ps_last = SDL_GetTicks()`, `:2201-2202 if (el < 33) SDL_Delay(33 - el)` |
| `main.c:2599`, `:2687` | Optionsbild: gleicher 33-ms-Deckel |
| `main.c:3009`, `:3025-3026` | Titel-Blende: **16 ms** je Bild ("Tick = 2 Vsyncs") — einziger 60-Hz-Unterlauf im Port |
| `platform/pc/src/render_pc.c:561-563` | `SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC` (Monitor-Takt), nicht bei `RE15_SOFTWARE_RENDER` |

### 6.2 Rezept für die Türszene

1. Eigene Szenenfunktion nach dem Muster von `pc_run_player_select` (eigene Schleife, kehrt am Ende in die
   Hauptschleife zurück). Ein Schleifendurchlauf = ein Door_move-Durchlauf (Scheduler Threads 10..13,
   Objekte, Bildzähler +1, Zeichnen) = **eine PSX-Austastung**.
2. Takt nachbilden statt Millisekunden raten: ein virtueller Vcount aus `SDL_GetPerformanceCounter()`,
   `vc = floor((t − t0) · 59.826)` (psx-spx `graphicsprocessingunitgpu.md:1264`), und eine Funktion mit
   genau der VSync-Semantik aus §1.3: Modus 0 = warten bis `vc > vc_beim_Aufruf`; Modus n ≥ 2 = erst bis
   `vc ≥ letzt + n − 1`, dann bis zum nächsten Tick; `letzt = vc` beim Verlassen. Die ganzzahligen
   `SDL_GetTicks`-Deckel treffen die Rate nicht: 16 ms = 62,5 Bilder/s (+4,5 %, 291 Bilder = 4,656 s),
   17 ms = 58,8 (−1,7 %, 4,947 s); Soll 16,715 ms, 4,864 s.
3. Moduswechsel an denselben Punkten wie RE2: Modus 0 ab dem ersten Door_move-Bild nach dem Ladevorlauf
   (`@0x80013f24`), Modus 0 auch während Door_exit auf die Blende wartet, Modus 2 erst danach
   (`@0x80014214`). Danach übernimmt wieder der 33-ms-Deckel der Hauptschleife.
4. Alle Bildzahlen der Türskripte (Sleep-Zähler, Blendenschritte −512/+1024 je Bild, 80 Bilder Drehung,
   291 Bilder gesamt) sind **60-Hz-Bilder**. In der Türszene nicht durch `FRAME_AT_60` schicken und nicht
   auf 30-Hz-Ticks umrechnen: ein Skriptbild = ein Szenen-Durchlauf.
5. `PRESENTVSYNC` bleibt an: bei 60-Hz-Monitor passt das zum 16,715-ms-Soll, bei höherer Wiederholrate
   bestimmt der virtuelle Vcount den Takt.

---

## 7. Offen

1. **Dynamisch nicht gemessen**, dass ein Door_move-Bild (RE2) bzw. ein Spielbild in 1 bzw. 2 Austastungen
   passt. VSync(0) garantiert nur "mindestens 1". Nächster Weg: RE1.5 in DuckStation durch eine Tür fahren
   und je Bild `0x800787dc` (Vcount) und `0x800b5456` protokollieren; für RE2 gibt es im Repo keine Savestates.
2. RE2-Anzeige ohne Zeilensprung nur statisch (SetDefDispEnv schreibt isinter = 0); dass später niemand
   isinter setzt, ist nicht geprüft. Für die Austastungszahl ohne Belang, solange die Anzeige 240 Zeilen hat.
3. RE2-VSync-Aufrufe `0x80032ebc` (0), `0x80033010` (0), `0x80033470`/`0x800334bc` (5), `0x80033530` (3)
   sowie der XA-Setzer über `0x800338a4` sind nicht bis zu einem Aufrufer verfolgt, der während der Tür läuft
   oder nicht läuft.
4. RE1.5-Einschub `0x8007d904` (Byte `0x800aca5d`): Herkunft und Zweck nicht ermittelt; wartet nicht.
5. Die Bibliotheksversion, die RE2 gelinkt hat, liegt nicht im Repo; der PsyQ-Vergleich ist strukturell (§1.2).

---

## 8. Nachmessen

```
python .claude/skills/re15-psx-disasm/scripts/re2_disasm.py dis 0x80085ea0 121      # VSync + v_wait
python .claude/skills/re15-psx-disasm/scripts/re2_disasm.py dis 0x8002b968 12       # Bildwechsel
python .claude/skills/re15-psx-disasm/scripts/re2_disasm.py dis 0x8002b2bc 40       # Task-Lauf + Weiche
python .claude/skills/re15-psx-disasm/scripts/re2_disasm.py dis 0x80013eb4 50       # Door_move
python .claude/skills/re15-psx-disasm/scripts/re2_disasm.py dis 0x8001417c 46       # Door_exit
python .claude/skills/re15-psx-disasm/scripts/re15_disasm.py dis 0x8002137c 50      # RE1.5 Bildwechsel
python .claude/skills/re15-psx-disasm/scripts/re15_disasm.py dis 0x80020dd0 110     # RE1.5 Hauptschleife
python build/tor_1170/re_bildtakt/vsync_sig.py                # vsync.o (4.7) Symbole/Relokationen, Ganzvergleich: bester Teiltreffer 50/132
python build/tor_1170/re_bildtakt/vsync_kern_vergleich.py     # 4.7-Kerne gegen RE2/RE1.5 wortgenau: KEINE Treffer (Delay-Slots)
python build/tor_1170/re_bildtakt/vsync_lib_suche.py RE2 0x80085ed8 52   # Präfix-Suche in allen LIBETC
python build/tor_1170/re_bildtakt/vsync_re2_re15_diff.py      # RE2 gegen RE1.5, 4 Abweichungen
python build/tor_1170/re_bildtakt/vsync_aufrufer.py           # alle jal VSync mit Argument
python build/tor_1170/re_bildtakt/savestate_vsync.py          # Modusbyte, Status, isinter in stage_saves
python re15_port/tools/tor/tuerskript_dump.py stat info/re2leon/COMMON/DOOR   # Opcode 0x59/0x6f fehlen
```
