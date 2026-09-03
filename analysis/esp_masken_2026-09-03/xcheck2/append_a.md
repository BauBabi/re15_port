
---

# Gegenpruefung

Unabhaengige Nachpruefung (2026-09-03). Jede zitierte Adresse wurde in `ghidra1_V2.txt` /
`ghidra_re2_Leon.txt` neu aufgeschlagen, jede Datenbehauptung mit eigenem Python gegen die
echten Bytes unter `re15_port/shared_assets/PSX/` nachgerechnet (Werkzeuge:
`analysis/esp_masken_2026-09-03/xcheck2/`).

**Ergebnis: 17 von 22 Behauptungen halten vollstaendig. Eine Kernbehauptung (Nr. 14) ist
sachlich WIDERLEGT, vier weitere tragen Fehler in Adresse, Zaehlung, Methode bzw. Arithmetik.**

## G.1 WIDERLEGT — Nr. 14: Das Gate-Bit ist KEIN Einweg-Latch

Behauptet: *"es existiert kein `lui .,0xffef` und kein Store, der genau dieses Bit wieder
loescht — es ist ein Einweg-Latch."*

Das ist falsch. Das Bit wird geloescht — nicht per AND-Maske, sondern per **wortweisem
Nullen ueber einen Zeiger**, weshalb die Suche des Berichts (AND-Masken, `lui 0xffef`) es
nicht sehen konnte.

```
FUN_80021eb4:
80021eb8 sw    s0,0x20(sp)
80021ebc lui   s0,0x800b
80021ec0 addiu s0,s0,0xfe8                    ; s0 = 0x800B0FE8
80021ec4 addiu a0=>DAT_800aca38,s0,-0x45b0    ; a0 = 0x800B0FE8-0x45B0 = 0x800ACA38
80021ee4 jal   FUN_8004ee60
80021ee8 _ori  a1,zero,0x7                    ; n = 7 Worte
```
```
FUN_8004ee60:                                 ; word-bzero
8004ee60 sw    zero,0x0(a0)
8004ee64 addiu a1,a1,-0x1
8004ee68 bgtz  a1,FUN_8004ee60
8004ee6c _addiu a0,a0,0x4
8004ee70 jr    ra
```

`FUN_8004ee60(&DAT_800aca38, 7)` nullt 0x800ACA38..0x800ACA53 **komplett**, Bit 0x00100000
eingeschlossen. Bestaetigt durch das Decompile `RE_15_Quellcode_V2/FUN_80021eb4.c`:

```c
FUN_8004ee60(&DAT_800aca38,7);
...
DAT_800aca38 = DAT_800aca38 | 0xc00000;
```

Drei Aufrufstellen (Ghidra `FUN_80021eb4 XREF[3]`): `@0x8001d200` (nach einem
`and 0x40000000`-Test), `main:@0x80020d14`, `@0x80021100` (direkt nach `ResetGraph`).
Das ist die Grafik-/Zustands-Reinitialisierung.

**Konsequenz fuer den Port:** der Masken-Zeichenpfad ist nach dem 27-Frame-Setzer
(`@0x80015230`) NICHT dauerhaft tot, sondern wird bei der naechsten Reinitialisierung wieder
scharf. Ein Port, der das Gate als Einweg-Latch modelliert, verliert die Vordergrund-Masken
ab dem ersten Tod-/Cutscene-Zustand fuer den Rest der Sitzung.

Gegenprobe zur Vollstaendigkeit: `grep "lui *[a-z0-9]*,0xffef" ghidra1_V2.txt` → **0 Treffer**
(insoweit stimmt der Bericht), und alle 17 direkten `sw`-Stores auf `DAT_800aca38` wurden
einzeln aufgeschlagen; die verwendeten AND-Masken sind `0xfbffffff`, `0xbfffffff`,
`0xf7ffffff`, `0xfffbffff`, `0xffffbfff`, `0xffff7fff`, `0xfffeffff`, `0xdfffffff`,
`0x7fffffff` — keine beruehrt Bit 20. Der Loescher ist ausschliesslich der indirekte bzero.

## G.2 WIDERLEGT — Nr. 14, zitierte Setzer-Adresse

Behauptet: *"Einziger Setzer der EXE ist @0x80039230"*. An `0x80039230` steht:

```
8003922c sra   v0,v0,0x10
80039230 mult  v0,v0
80039234 mflo  a0
80039238 jal   SquareRoot0
```

— eine Distanzberechnung, kein Store und kein Bit-Setzer. Der tatsaechliche Setzer ist
`@0x80015230 or v0,v0,v1` / `@0x80015240 sw v0,0x0(a1)`. Der Fliesstext des Berichts
(Zeile 426-434) nennt ihn korrekt; nur die Zusammenfassung traegt die falsche Adresse.

## G.3 WIDERLEGT — Nr. 18, Dateizaehlung

Behauptet: *"1678 extrahierte BG*.BSS"*. Gemessen:

```
find re15_port/shared_assets/PSX/BSS -name "BG*.BSS" | wc -l   -> 1688
```
1688, keine Datei kleiner als 1 KiB, keine weiteren `*.BSS` im Baum. 1678 ist um 10 zu klein.

## G.4 WIDERLEGT — Nr. 18, Methode: die Trailer-Regel allein reicht NICHT

Der Bericht (Zeile 697) formuliert die Rekonstruktion als *"letztes Nicht-Null-Byte, auf 4
aufgerundet"*. Genau so implementiert (`xcheck2/tr.py`) liefert sie:

```
files 1688 | trailer hits 818 | Abweichungen gegen Brute-Force: 322
z.B. BSS/ROOM1020/BG11.BSS -> off=22244 raw=22244 (Offset zeigt auf sich selbst), find_sld: []
      Groessen darunter: 939539392, 1069059, 13676 ... = Muell
```

Der Grund ist strukturell: der Disasm holt das Record-Ende NICHT aus den Daten, sondern aus
einer Laufzeit-Tabelle — `@0x80021d40 lh v1,DAT_800b0fe4` / `@0x80021d48 lh v0,0x0(s0)` /
`@0x80021d50 sll v0,v0,0x5` / `@0x80021d5c lhu v0,0x0(v1)` → u16-Endoffset pro (Cut, Kamera),
erst dann `-0x4` (Flag) und `-0x8` (SLD-Offset). Ohne diese Tabelle ist "letztes Nicht-Null-Byte"
nur eine Heuristik.

**Mit** einer Gueltigkeitsschranke (Offset < end-8, `0x2000 <= raw <= 0x40000`, erste vier
entpackte Bytes `10 00 00 00` = TIM-Magic) reproduziert sie den Brute-Force dann exakt
(`xcheck2/tr2.py`):

```
brute-force files with SLD: 496 [(66080, 418), (61984, 76), (58400, 2)]
trailer  files with SLD:    496 [(66080, 418), (61984, 76), (58400, 2)]
agree 496 | only_tr 0 | only_bf 0 | diff 0
```

Die Zahlen des Berichts (496 / 418 / 76 / 2) sind damit bestaetigt — die **Regel als solche**
aber nur zusammen mit der Validierung. Fuer den Port ist das der Unterschied zwischen
"funktioniert" und "laedt in 322 von 1688 Faellen Muell".

## G.5 WIDERLEGT — Nr. 20(e), Arithmetik

Behauptet: *"188 ... 114 davon STAGE5 ... 74+2 die nie extrahierten Atlanten"*.
114 + 74 + 2 = **190**, nicht 188. Gemessen (alle 206 RDTs gegen `BSS/ROOM####/PRI##.TIM`):

```
mask cuts 480 | have PRI.TIM 292 | missing 188
missing by room: 1020:12 1021:12 1030:12 1031:12 1040:5 1041:5 1070:8 1071:8   = 74
                 5030:4 5031:4 5040:6 5041:6 5060:12 5061:12 50A0:10 50A1:10
                 50C0:4 50C1:4 5110:4 5111:4 5120:6 5121:6 5140:11 5141:11     = 114
```
188 = 114 + 74. Die zwei 58400-Byte-Bloecke (ROOM4040/4041 Cut 5) sind SLD-Bloecke **ohne**
Maskenrecords und gehoeren gar nicht in diese Menge. Die Kernzahl 188 stimmt exakt; die
Aufschluesselung ist falsch summiert.

## G.6 Bestaetigt (kurz)

Alle uebrigen Behauptungen wurden Instruktion fuer Instruktion bzw. Byte fuer Byte
nachvollzogen und halten:

- **1, 2** — `FUN_80039270` Arena-Schnitt N*0x44 (`@0x8003928c/94/a0/ac/b8/c4/d0`), genau ein
  Aufrufer (`FUN_800396fc:@0x800399cc`); Loader-Rebase `@0x8003986c/74/78` ueber
  `lbu v0,0x1(v1)` Cuts, Cut-Tabelle `lw v0,0x24(v1)`, Recordbreite `sll a2,0x5`. Kein
  Obergrenzen-Vergleich im Code.
- **3, 4, 5, 6, 7** — Section-/Gruppen-/Masken-Layout und Slot-Belegung exakt wie beschrieben
  (Disasm 0x800392d4-0x80039558 vollstaendig gelesen). SPRT-Slot 32 B, DR_MODE 12 B
  (`SetDrawMode` schreibt `len=2` `@0x80069868`), beide Frame-Puffer identisch gefuellt.
- **8, 9, 10, 11** — Kontroll-Record 4 B; `SetSprt`-Body `@0x8006b704 len=4`,
  `@0x8006b70c/14 code=0x64`; `SetShadeTex`-Body `@0x8006b61c ori v0,v0,0x1` → tot;
  `MargePrim` `@0x8006b958-64: len = 2+4+1 = 7`; OT-Basis `0x800B0000-0x35CC-0x235C =
  0x800AA6D8`, Frame-Schritt `sll v0,0xc` = 1024 Worte, Index `sll a0,0x2` = depth x1.
- **12** — TPage 0x95 → (320,256)/8-bit/ABR0, CLUT 0x7800 → (0,480); `@0x80021de8/f0`
  `sh 0x15` und `FUN_8004ee78` erzwingen `prect->x=320` (`@0x8004eea8/b0/b8`),
  `prect->y=256` (`@0x8004eec8-d4`), `crect->y=0x1e0` (`@0x8004ef30-3c`). Datenprobe
  `BSS/ROOM1170/PRI01.TIM`: len 66080, CLUT-Rect (0,480,256,1), Bild-Rect (0,0,128,256).
- **13** — `@0x80039328-38` Zweig + Delay-Slot; Loeschschleife erst `@0x80039370-84`.
- **15** — Tabelle `0x800744a8`, Eintrag `0x800745bc` → `LAB_800428d4`, Index 0x45;
  `FUN_800396a8` setzt `ctrl[i].flags` fuer `ctrl[i][+1] == a0`.
- **16** — `DAT_800b2584`: 5 echte Instruktionen in 4 Funktionen; `@0x8005c200` ist tatsaechlich
  ein Ghidra-Fehlalarm (`@0x8005c1f8 lui at,0x800b` + `@0x8005c1fc addu at,at,v0`).
  `DAT_800bb4d4/d8`: genau die 5 genannten Instruktionen in 3 Funktionen. Alle 15
  `jal SetDrawMode`-Stellen einzeln aufgeschlagen — nur `@0x80039630` traegt 0x95. Alle
  RDT+0x24-Leser geprueft: keiner ausser `@0x8003931c`/`@0x8003986c` dereferenziert +0x1C
  (auch `FUN_80053ca4` nicht — dessen `0x1c(..)`-Zugriffe gehen auf `DAT_80072d68`/`DAT_800b52a4`).
- **17** — RE2 `@0x80049d68-db0` exakt wie zitiert (32-B-Slot, SPRT +0xC, TPage 0x95,
  depth x1, Rueckwaertslauf `@0x80049d28`, Extra-Gate `@0x80049d44`, dtd=0
  `@0x80049d84 clear a2`).
- **18 (Daten)** — `BSS/ROOM1020/BG00.BSS` @0xa418 = `b8 7b 00 00`, @0xa41c = `01 00 00 00`,
  @0x7bb8 = `20 f2 00 00`; `BSS/ROOM1220/BG01.BSS` @0x8824 = `ec 66 00 00 01 00 00 00`,
  @0x66ec = `20 02 01 00`. Byte fuer Byte bestaetigt.
- **19** — Trailer-/SLD-Scan auf `STAGE5/*.BSS` reproduziert **jeden** genannten Offset:
  ROOM503 Cut1-4 = 0x5a84/0x79d0/0x6b28/0x6b18; ROOM504 Cut0,1,2,3,4,6 =
  0x76b0/0x781c/0x5100/0x5458/0x7b20/0x5540; ROOM506/50A/50C/511/512/514 ebenso, alle 66080.
  Unter `BSS/` existieren tatsaechlich nur `ROOM5000/` und `ROOM5001/`.
- **20 (a-d)** — `re15_pri.h:13` und `pri_common.c:13/62` sagen "baseRaw ... TPage" (falsch);
  `render.c:463 setDrawTPage(tp,0,1,...)` = dfe 0 (SDK-Signatur `(p,dfe,dtd,tpage)`, bestaetigt
  in `PSn00bSDK/.../psxgpu.h:34` und `Psy-Q_47/INCLUDE/LIBGPU.H:279`) gegen `a1=1` im Original;
  `pri_psx.c:34/58` klemmt auf 128 Zeilen (Atlanten 256/240/226 Zeilen, nachgerechnet aus
  66080/61984/58400); `render.c:452-453` klemmt den OT-Index, das Original nicht.
- **21** — `t_addr=0x80010000`, `t_size=0x000af000` → Textende **0x800BF000**, also liegt
  0x800C47E8 ausserhalb; `ghidra1_V2.txt` Z.574973 zeigt dort `?? ??`; Loader
  `@0x80013124 ori a0,zero,0x7` / `@0x80013128 lui a1,0x800c` / `jal FUN_80013b60`. Die
  Einstufung "teilbelegt" ist korrekt.
- **22** — korrekt als NICHT BELEGT gekennzeichnet; die Einschraenkung (Emitter mit variabler
  TPage nicht erschoepfend durchsucht) ist berechtigt: `@0x80021848/@0x80021864`
  (`lbu a3,0x4(s0)` + `sll 5`) und `@0x800295f8/@0x80029624/@0x8002e074`
  (`andi a3,v0,0xffff`) ziehen ihre TPage tatsaechlich aus Variablen.

## G.7 Zusatzbefunde aus der Gegenpruefung (nicht im Bericht)

1. **Die unteren 12 Bit des Masken-size-Feldes sind die AUTORISIERTE TPage.** Ueber alle
   16654 Maskenrecords: `0x095` 8210x, `0x0B5` 3454x, `0x0D5` 3284x, `0x0F5` 1682x,
   `0x000` 20x, `0x800` 4x. Das sind TPage 0x95 mit vier verschiedenen ABR-Stufen (Bits 5-6).
   Die Runtime ignoriert sie und haert 0x95/ABR0 ein — exakt das gleiche Muster wie beim
   Gruppenfeld +2 (`0x7800` = CLUT). Nr. 5 ("werden nie benutzt") ist als Laufzeit-Aussage
   richtig; als Format-Aussage waere sie falsch.
2. **Gruppenfeld +2 unabhaengig nachgezaehlt** (206 RDTs, 2188 Cuts, 480 mit Masken,
   2750 Gruppen): `0x7800` 2712x, `0x7c00` 26x, `0x7c10` 8x, `0x8080` 2x, `0x8088` 2x —
   identisch zu Nr. 4.
3. **ROOM1210/ROOM1211 Cut 4: declaredCount 75, Gruppensumme 77.** In 478 von 480
   Masken-Cuts sind beide gleich; hier nicht. Da der Parser 77 Slots fuellt, der Emitter aber
   `*DAT_800ac778` = 75 als Schleifengrenze nimmt (`@0x800395c0/@0x8003966c`), werden im
   Original zwei gebaute Masken **nie gezeichnet**. `pri_common.c` baut korrekt 77 — der
   ZEICHEN-Pfad muss dann aber auf 75 begrenzt werden, sonst zeichnet der Port zwei Masken
   zu viel. Byte-true Divergenz, die Nr. 3 impliziert, aber nicht ausspricht.
4. **Alle Tiefenwerte liegen in 0..1023** (16654 Records, kein einziger >= 0x8000). Der
   1024-Wort-OT wird exakt ausgeschoepft, und das `lh` (signed) im Emitter kann bei echten
   Daten nie negativ werden — die Klemmung des PSX-Ports (Nr. 20d) ist bei realen Daten
   folgenlos, bleibt aber eine Abweichung.
5. **Zwei Port-Abweichungen fehlen in Nr. 20:** `pri_common.c` bricht bei
   `group_count == 0 || mask_count_decl == 0` und bei `> 256` ab. Das Original kennt
   ausschliesslich den Test `u32 == 0xFFFFFFFF` (`@0x80039328-2c`); bei `groupCount == 0`
   laeuft es regulaer durch (`@0x800393a8 beq v0,zero,LAB_8003955c` — erst NACH
   `@0x80039358 sb t2,0x0(a0)`, die Zeichenzahl bleibt also auf declaredCount stehen
   statt auf 0).
