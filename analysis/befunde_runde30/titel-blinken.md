# Runde 30 — Thema D: Titelmenü blinkt zu schnell

Stand: master 8d83a025 (v0.8.15). Phase: ERMITTLUNG, kein Bau. Datum 2026-09-27/28.
Build: `re15_port/build_r30_titel-blinken/`. Ausgaben: `build/r30_titel-blinken/`.
Werkzeuge: `analysis/befunde_runde30/titel-blinken_tools/`.
Sonde: `re15_port/tests/unit/probe_r30_titel-blinken_puls.c` +
`re15_port/tests/unit/probes/r30_titel-blinken.cmake`.

An `engine/`, `platform/`, `include/`, `shared_assets/` wurde nichts geändert; keine
git-Schreiboperation.

---

## 1. Symptom / Auftrag

Nutzer (AUFTRAG.md Abschnitt D, wörtlich):

> Im Titelbild bei der Auswahl New Game, Load Game, Option blinkt der ausgewählte Bereich
> in der Frequenz im Vergleich zum Original zu schnell.

Kurzfassung des Ergebnisses:

| | Aufrufe je Periode | Dauer eines Schritts | Periode |
|---|---|---|---|
| **Original** | 60 (0x3c @0x80102918) | 2 VBlanks = 33,435 ms (gemessen) | **120 VBlanks = 2006 ms** |
| **Port v0.8.15**, diese Maschine (Anzeige 144 Hz) | 60 | 1 Bild der Anzeige = 6,94 ms (gemessen) | **416 ms** |

Der Port pulst auf dieser Maschine **4,82-mal zu schnell**. Die Pulsfolge selbst
(Zählerstart, Schrittweite, Periode in Aufrufen) ist im Port richtig; falsch ist allein,
**wie oft je Sekunde** der Schritt ausgeführt wird.

![Puls Original gegen Port](../../build/r30_titel-blinken/puls_original_gegen_port.png)

---

## 2. MESSUNG im Port

### 2.1 Zeitstempel je Pulsschritt (instrumentierte Kopie)

Werkzeug `titel-blinken_tools/r30_build_instr.sh`: kopiert `render_pc.c` nach
`build/r30_titel-blinken/instr/`, hängt in der KOPIE hinter den Pulsschritt eine Logzeile
(Mikrosekunden aus `SDL_GetPerformanceCounter`, Zähler, Wert) und linkt sie gegen die
unveränderten übrigen Objekte → `re15_pc_r30instr.exe`. Lauf: `RE15_NO_INTRO=1`,
beschleunigter Renderer, keine Eingabe, 14 s im Titelmenü.

Auswertung `r30_pulse_log_stats.py` (`build/r30_titel-blinken/port_pulse_stats_default.txt`):

```
Aufrufe               : 1741  (erste 120 uebersprungen)
Messdauer             : 12.074 s
Aufrufabstand  Median : 6.940 ms   Mittel 6.939 ms   min 2.804   max 9.491
Aufrufrate (Mittel)   : 144.11 Hz
Ruecksetzungen ctr==0 : 29
PULSPERIODE           : Median 416.5 ms   Mittel 416.3 ms   min 409.4   max 418.2   (n=28)
Pulswerte             : min 0x80 max 0xbe, 32 verschiedene
Aufrufe je Periode    : [60]
```

Anzeige dieser Maschine: `Win32_VideoController.CurrentRefreshRate = 144` (Intel UHD,
1920x1080). Aufrufrate 144,11 Hz = Bildrate der Anzeige.

### 2.2 Das echte Fenster (gdigrab, unveränderte exe)

Werkzeug `r30_port_capture.sh`: startet die UNVERÄNDERTE `re15_pc.exe` (beschleunigt, kein
AUTOSHOT, kein SOFTWARE_RENDER), nimmt das Fenster über sein **Fenster-Handle** auf
(`-i hwnd=…`), Maßstab 1, in die linke obere Ecke geschoben.

⚠ Messfalle, in die ich zuerst lief: `-i "title=RE1.5 Rebuilt — PC"` griff das Fenster
eines parallel arbeitenden Agenten (Bild mit Touch-Overlay). Das Bild ist verworfen; alle
Zahlen unten stammen aus der Aufnahme über das Handle (`pid=63072 hwnd=1904252`).

Aufnahme `build/r30_titel-blinken/port_title_capture.mkv` (500 Bilder in 5,994 s),
Helligkeit je Bild über `crop,signalstats`, Auswertung `r30_yavg_period.py`
(`build/r30_titel-blinken/port_row_period.txt`):

| Region | YAVG min | YAVG max | Spanne | Periode |
|---|---|---|---|---|
| Zeile NEW GAME (aktiv) | 48,794 | 50,381 | 1,587 | **416,7 ms** (15 Durchgänge, min 411,7 / max 423,3) |
| Zeile LOAD GAME (inaktiv) | 39,084 | 39,084 | 0,000 | kein Puls |

Zwei unabhängige Wege, dieselbe Zahl: 416,3 ms (Log) und 416,7 ms (Fenster).

### 2.3 Wie der Puls im Port wirkt

- Nur die **aktive** Zeile pulst, und zwar in der **Helligkeit**. Die CLUT-Wahl ist fest
  (aktiv = weiße Unterpalette, inaktiv = blaue). Es gibt keinen Sichtbar/Unsichtbar-Wechsel.
- Die inaktiven Zeilen und die Copyright-Zeile des Ports decken sich **pixelgenau** mit dem
  Original-Bildpuffer (`build/r30_titel-blinken/port_vs_orig_rows.txt`):

```
port_puls_minimum.png  NEW GAME (aktiv)  :   479 von 4352 Pixeln weichen vom Original-Bildpuffer ab (max 9 Stufen von 31)
port_puls_minimum.png  LOAD GAME         :     0 von 4352 Pixeln weichen vom Original-Bildpuffer ab
port_puls_minimum.png  OPTION            :     0 von 4352 Pixeln weichen vom Original-Bildpuffer ab
port_puls_minimum.png  Copyright         :     0 von 5376 Pixeln weichen vom Original-Bildpuffer ab
```

- Die aktive Zeile weicht ab — das ist der **Nebenbefund Helligkeit** (§4.3): mittlere
  Zeilenhelligkeit im Port 37,67 … 38,74, im Original 39,86 … 43,74 (gleiches Maß,
  `port_rows.txt` / `orig_row_sim_targets.txt`).

Bilder: [Port Puls-Minimum](../../build/r30_titel-blinken/port_puls_minimum.png),
[Port Puls-Maximum](../../build/r30_titel-blinken/port_puls_maximum.png),
[Original val 0x88](../../build/r30_titel-blinken/orig_boot_48_buf0.png),
[Original val 0xbc](../../build/r30_titel-blinken/orig_boot_52_buf0.png).

### 2.4 Puls während des Bestätigungs-Fades

Instrumentierter Lauf mit `RE15_INPUT_SCRIPT="W20,A1,W60"`
(`build/r30_titel-blinken/port_pulse_confirm_stats.txt`):

```
Titelschleife : 487 Aufrufe, Abstand Median 6.95 ms (= 144.0 Hz)
Fade-Schleife : 330 Aufrufe, Abstand Median 16.68 ms, Mittel 16.49 ms; Dauer 5.44 s
Pulswert aendert sich WAEHREND des Fades: 330 mal (Original: 0 - FUN_80102a10 ruft FUN_801028ec nicht)
Pulsperiode waehrend des Fades: Median 1000.1 ms (n=5)
```

Der Port pulst also in zwei verschiedenen Tempi (416 ms im Menü, 1000 ms im Fade); das
Original pulst im Fade gar nicht (§3.5).

---

## 3. ORIGINAL-MECHANISMUS

Quellen: `info/Re1.5/PSX/BIN/TITLE.BIN` (11832 Byte, lädt @0x80100000 OHNE Kopf,
Datei-Offset = Adresse − 0x80100000) und `info/Re1.5/PSX.EXE` (Datei-Offset =
0x800 + Adresse − 0x80010000). Disassembliert mit `re15_disasm.py`; Volltext in
`build/r30_titel-blinken/title_disasm_80101f00.txt` und `exe_main_80020c80.txt`.

Der Code im RAM der Savestates ist byte-gleich zu diesen Dateien
(`build/r30_titel-blinken/ram_vs_file.txt`: 6 Bereiche × 3 Savestates, je 0 abweichende
Byte). Die Savestates laufen auf der ungepatchten EXE (`@0x80026e4c` ≠ `24 c2 01 08`).

Die Sonde `probe_r30_titel_blinken_puls` prüft jedes unten zitierte Befehlswort gegen die
Dateien: 28 Stellen in TITLE.BIN, 17 in PSX.EXE, Ergebnis „alle zitierten Stellen BELEGT"
(`build/r30_titel-blinken/probe_puls.txt`).

### 3.1 Der Puls-Handler FUN_801028ec (vollständig)

```
801028ec  lui   v1,0x8010
801028f0  lhu   v1,10566(v1)        ; v1 = Zaehler  @0x80102946
801028f4  lui   a0,0x8010
801028f8  lhu   a0,10564(a0)        ; a0 = Pulswert @0x80102944
801028fc  sltiu v0,v1,0x1f          ; Zaehler < 31 ?   (Vergleich VOR dem Inkrement)
80102900  beq   v0,zero,0x80102910
80102904  nop
80102908  j     0x80102914
8010290c  addiu a0,a0,2             ;   ja:   Pulswert += 2
80102910  addiu a0,a0,-2            ;   nein: Pulswert -= 2
80102914  addiu v1,v1,1             ; Zaehler += 1
80102918  ori   v0,zero,0x3c
8010291c  bne   v1,v0,0x8010292c    ; Zaehler == 60 ?
80102920  nop
80102924  ori   v1,zero,0x0         ;   Zaehler  := 0
80102928  ori   a0,zero,0x80        ;   Pulswert := 0x80
8010292c  lui   at,0x8010
80102930  sh    v1,10566(at)        ; -> 0x80102946
80102934  lui   at,0x8010
80102938  sh    a0,10564(at)        ; -> 0x80102944
8010293c  jr    ra
80102940  nop
80102944  .half 0x0080, 0x0000      ; Startwerte (Datei 0x2944 / 0x2946)
```

Die Sonde führt genau diese Bytes in einem Mini-R3000 aus:

```
Aufruf   1: Zaehler  1  Pulswert 0x82
Aufruf  31: Zaehler 31  Pulswert 0xbe
Aufruf  32: Zaehler 32  Pulswert 0xbc
Aufruf  59: Zaehler 59  Pulswert 0x86
Aufruf  60: Zaehler  0  Pulswert 0x80
=> Aufrufe je Periode: 60   Pulswert min 0x80 max 0xbe (Maximum nach Aufruf 31)
=> 31 Schritte aufwaerts (+2), 28 abwaerts (-2), dann Ruecksetzen auf 0x80
```

Die Welle ist also kein symmetrisches Dreieck: 31 Schritte hoch, 28 runter, dann Sprung
0x86 → 0x80.

### 3.2 Wer ruft den Puls — und wie oft je Durchgang

Aufrufer von FUN_801028ec (Suche nach dem Wort `0c040a3b` in TITLE.BIN):
`0x80102234`, `0x80102ad0`, `0x80102ba0`.

| Aufrufstelle | Funktion | erreichbar? |
|---|---|---|
| 0x80102ba0 | Menü-Handler **FUN_80102b00** = Sub-State 1, Tabelle 0x801026ac[1] | **ja — der Live-Pfad** |
| 0x80102ad0 | FUN_80102a8c = Sub-State 0, Tabelle 0x801026ac[0] | nein: Init setzt Sub-State 1 (`ori t0,zero,1` @0x801020c4, `sb t0,0x26c5(at)` @0x801020cc) |
| 0x80102234 | FUN_801021ac | nein: in TITLE.BIN weder `jal` noch `j` noch Zeiger noch `lui/addiu`-Paar darauf, in PSX.EXE kein Zeiger |

Im Menü-Handler steht der Aufruf **genau einmal**, unbedingt, vor den drei Zeilen:

```
80102ba0  jal 0x801028ec            ; 1 Pulsschritt
80102bc0  jal 0x801027a0            ; Zeile 0 (0x20,0x85), a2 = (cursor==0)<<4 | 1
80102bdc  jal 0x801027a0            ; Zeile 1 (0x20,0x99), a2 = (cursor==1)<<4 | 2
80102bf8  jal 0x801027a0            ; Zeile 2 (0x20,0xad), a2 = (cursor==2)<<4 | 3
80102c00  jal 0x80102948            ; Copyright
```

Die Titel-Task-Schleife (Einsprung 0x80101f7c = Overlay-Tabelle der EXE @0x80073bdc[0]:
Datei-Id 6, Einsprung 0x80101f7c):

```
80101f98  lbu  v0,9924(v0)          ; Titel-State @0x801026c4
80101fa4  sll  v0,v0,2
80101fa8  addu v0,v0,s0             ; s0 = 0x8010269c (State-Tabelle)
80101fac  lw   v0,0(v0)
80101fb4  jalr v0                   ; State-Handler; State 2 = 0x80102100 -> Tabelle 0x801026ac[Sub-State]
80101fbc  jal  0x80029ac8           ; Task gibt ab ...
80101fc0  ori  a0,zero,0x1          ; ... fuer 1 Durchgang
80101fc4  j    0x80101f98
```

### 3.3 Die Taktung: ein Durchgang = VSync(2)

**Abgeben** FUN_80029ac8(n):

```
80029ad0  lw   v1,11044(v1)         ; v1 = laufender Task-Slot (DAT_800b2b24)
80029ad4  ori  v0,zero,0x1
80029adc  sh   a0,2(v1)             ; Wartezaehler := n
80029ae4  jal  0x8006e3c8           ; ChangeTh(0xff000000) -> zurueck in den Hauptfaden
80029ae8  sh   v0,0(v1)             ; Zustand := 1 (ruht)
```

**Scheduler** FUN_800298b0, einmal je Durchgang der Hauptschleife (`jal 0x800298b0`
@0x80020de4):

```
80029944  lhu   v0,-6(s0)           ; Wartezaehler
8002994c  addiu v0,v0,-1
80029950  sh    v0,-6(s0)
80029958  bgtz  v0,0x80029970       ; > 0 -> Task ruht weiter
80029960  sh    s2,0(s1)            ; sonst Zustand := 0x7f und
80029968  jal   0x8006e3c8          ;   ChangeTh(Task)
```

Mit n = 1 läuft der Titel-Task also in **jedem** Durchgang der Hauptschleife genau einmal.

**Hauptschleife** (`main`, Schleifenkopf 0x80020c10): … `jal 0x800298b0` @0x80020de4
(Tasks) … `jal 0x80021880` @0x80020f44 (Fade-Tick) … `jal 0x8002137c` @0x80020f4c (Flip)
→ `j 0x80020c10`.

**Flip** FUN_8002137c:

```
8002138c  jal   0x80068a60          ; DrawSync(0)   (a0 = 0 @0x80021380)
80021474  lui   s0,0x800b
80021478  addiu s0,s0,21590         ; s0 = &DAT_800b5456
8002147c  lbu   a0,0(s0)            ; a0 = DAT_800b5456
80021480  jal   0x80061fc0          ; VSync(a0)
```

**Der Wert von DAT_800b5456 im Titel = 2.** Die Boot-Task (LAB_80021138, registriert am
Ende von FUN_80020f8c) setzt ihn unmittelbar vor dem Start von TITLE.BIN:

```
8002130c  ori  v0,zero,0x2
80021310  lui  at,0x800b
80021314  sb   v0,21590(at)         ; DAT_800b5456 := 2
80021318  jal  0x80029a28           ; FUN_80029a28(0) = Overlay-Tabelle[0] laden + Task ersetzen
8002131c  addu a0,zero,zero
```

Alle Schreiber von DAT_800b5456 (Suche über PSX.EXE und alle `BIN/*.BIN` nach
Befehlen mit Immediate 0x5456):

| Adresse | Wert | Kontext |
|---|---|---|
| 0x80021314 | 2 | Boot-Task, vor TITLE.BIN |
| 0x80020d10 | 2 | Soft-Reset in `main` |
| 0x800166a4 | 2 | `ori v1,zero,2` @0x8001669c |
| 0x8001d5ec | 2 | `ori v0,zero,2` @0x8001d5e4 |
| 0x80026624 | 2 | FUN_80026594, Karten-Bildschirm verlassen |
| 0x80046718 | 2 | `ori v0,zero,2` @0x80046710 |
| 0x80016208 | 0 | FUN_800161e0 |
| 0x8001d268 | 0 | `sb zero` |
| 0x8002650c | 0 | FUN_800264e8, Karten-Bildschirm betreten |
| 0x800460e0 | 0 | FUN_800460b8 |

In TITLE.BIN selbst steht **kein** Zugriff mit diesem Immediate. Wert im EXE-Abbild
(Datei 0xa5c56): 0.

**VSync** FUN_80061fc0 — als VSync belegt durch die Zeichenkette `"VSync: timeout\n"`
@0x8001178c, die seine Warteschleife FUN_80062108 @0x80062150-58 ausgibt:

```
80061ff8  bgez  a0,0x80062010       ; n < 0  -> gibt Vcount zurueck
80062010  ori   v0,zero,0x1
80062014  beq   a0,v0,0x800620f0    ; n == 1 -> gibt nur die HBlank-Differenz zurueck
8006201c  blez  a0,0x8006203c
80062024  lui   v0,0x8008
80062028  lw    v0,-30736(v0)       ; letzter Vcount @0x800787f0
80062030  addiu v0,v0,-1
80062038  addu  v0,v0,a0            ; Ziel = letzter - 1 + n
80062050  jal   0x80062108          ; warten bis Vcount >= Ziel (Zeitgrenze (n-1)<<15)
80062068  lui   a0,0x8008
8006206c  lw    a0,-30756(a0)       ; Vcount @0x800787dc
80062074  jal   0x80062108          ; warten bis Vcount >= Vcount + 1
80062078  addiu a0,a0,1
800620cc  lw    v0,-30756(v0)
800620dc  sw    v0,-30736(at)       ; letzter := Vcount
```

Mit n = 2: erst bis `letzter + 1`, dann noch ein VBlank → der Rücksprung liegt
**2 VBlanks** nach dem vorigen. Vcount zählt der VBlank-Rückruf 0x80061ef0 (eingetragen
@0x80061ec8-d0): `addiu v0,v0,1` @0x80061f14, `sw v0,-30756(at)` @0x80061f1c.

**Bildrate.** DISPENV im RAM der Titel-Savestates (`boot_40.sav`, `mzd_title.sav`):
Anzeige 320×240, `isinter` @0x800b5438 = **0**. Der Titel läuft also **nicht** in 480i.
psx-spx (`graphicsprocessingunitgpu.md`, „Vertical Refresh Rates", Zeile 1260-1264): NTSC
non-interlaced **59,826 Hz**. Die DuckStation-Aufnahme trägt 59,8173 Bilder/s.

⚠ Berichtigung: `analysis/title_fade_voice.md` §1 und der Kommentar `main.c:2996` erklären
die 2-VBlank-Paare mit „der Titel läuft 480i". Das ist widerlegt (isinter = 0). Die Paare
kommen aus VSync(2).

### 3.4 Wie der Pulswert auf das Bild wirkt — Zeichner FUN_801027a0

Aufruf `(a0 = x, a1 = y, a2 = (aktiv << 4) | Deskriptor-Index)`.
Deskriptor-Tabelle 0x801028ac, 16 Byte je Eintrag (Datei 0x28ac):

| Index | +0 (aktiv) | +4 (inaktiv) | +8 (Größe) |
|---|---|---|---|
| 0 | 0x7fc00000 | 0x7fcc0000 | 0x00100100 |
| 1 NEW GAME | 0x7fc01000 | 0x7fcc1000 | 0x00100100 |
| 2 LOAD GAME | 0x7fc02000 | 0x7fcc2000 | 0x00100100 |
| 3 OPTION | 0x7fc03000 | 0x7fcc3000 | 0x00100100 |

(Wort = CLUT << 16 | v << 8 | u; CLUT 0x7fc0 = VRAM (0,511) weiß, 0x7fcc = VRAM (192,511)
blau; Größe = 0x100 breit, 0x10 hoch.)

```
801027d4  andi t0,a2,0xf            ; Deskriptor-Index
801027e4  srl  s5,a2,4              ; s5 = aktiv
801027f4  ori  s4,zero,0x0          ; Durchgang 0
801027f8  lui  t0,0x500             ; Prim-Tag: 5 Worte
80102800  lui  t0,0xe100
80102804  addiu t0,t0,149           ; Texpage-Befehl 0xe1000095
80102808  beq  s4,zero,0x80102820
80102810  lui  t3,0x1               ; Durchgang 1: xy + 0x10000  = y + 1
80102814  addu t3,s2,t3
8010281c  ori  t0,t0,0x40           ; Durchgang 1: 0xe10000d5 = ABR 2 (B - F)
80102824  ori  t0,t0,0x20           ; Durchgang 0: 0xe10000b5 = ABR 1 (B + F)
80102830  lui  t0,0x6681
80102834  addiu t0,t0,-32640        ; Befehl 0x66808080 = texturiertes Rechteck, halbtransparent, MODULIERT
80102838  sw   t0,8(s0)
8010283c  beq  s5,zero,0x8010285c   ; inaktiv -> Farbe bleibt 0x80 0x80 0x80
80102840  lw   t1,4(s3)             ;   (Verzoegerungsplatz) CLUT/uv = Deskriptor +4 (blau)
80102844  lui  t0,0x8010
80102848  lhu  t0,10564(t0)         ; aktiv: t0 = Pulswert @0x80102944
8010284c  lw   t1,0(s3)             ;        CLUT/uv = Deskriptor +0 (weiss)
80102850  sb   t0,8(s0)             ;        R := Pulswert
80102854  sb   t0,9(s0)             ;        G := Pulswert
80102858  sb   t0,10(s0)            ;        B := Pulswert
8010286c  jal  0x8006b538           ; AddPrim
80102880  beq  s4,zero,0x801027f8   ; zweiter Durchgang
80102884  addiu s4,s4,1
```

Wirkung:

- **Der Pulswert ist das Farbbyte des modulierten Rechtecks.** psx-spx („Modulation"):
  `kanal = texel * farbe / 128`, Sättigung bei 31. 0x80 = Faktor 1,0; 0xBE = Faktor 1,484.
- Beide Durchgänge (additives Leuchten bei y, subtraktiver Schatten bei y+1) tragen
  denselben Pulswert.
- **Die CLUT wechselt nicht mit dem Puls**, nur mit „aktiv/inaktiv".
- **Es gibt keinen Sichtbar/Unsichtbar-Blinker.** Die drei Zeilen werden in jedem
  Durchgang unbedingt gezeichnet (0x80102bc0/dc/f8); der Zeichner hat keinen Zweig, der
  ein Rechteck auslässt.

### 3.5 Im Bestätigungs-Fade ruht der Puls

Der Fade FUN_80102ccc zeichnet das Menü über FUN_80102a10 neu (`jal 0x80102a10`
@0x80102d10 und @0x80102d60). FUN_80102a10 (0x80102a10-0x80102a88) ruft dreimal den
Zeichner und einmal die Copyright-Zeile — **FUN_801028ec ruft er nicht** (Sonde: Aufrufe
im Menü 1, im Neuzeichner 0, im Fade 0). Der Pulswert bleibt auf dem Stand des
Bestätigungs-Durchgangs stehen.

### 3.6 DYNAMISCHE MESSUNG im Original

**(a) Savestate-Reihe** `stage_saves/boot_40/44/48/52.sav` — alle im Titelmenü
(State @0x801026c4 = 2, Sub-State @0x801026c5 = 1), Werkzeug `r30_title_pulse_measure.py`
(`build/r30_titel-blinken/pulse_measure_boot.txt`):

```
savestate     Vcount  lastV  vsArg ctr  val   val_soll
boot_40.sav   1536    1535   2     49   0x9a  0x9a
boot_44.sav   1846    1845   2     24   0xb0  0xb0
boot_48.sav   2156    2155   2     59   0x86  0x86
boot_52.sav   2466    2465   2     34   0xb8  0xb8

a            b            dV   dCtr  H30:(dV/2)%60  H60:dV%60  Urteil
boot_40.sav  boot_44.sav  310  35    35.0           10         H30 passt / H60 FAELLT
boot_44.sav  boot_48.sav  310  35    35.0           10         H30 passt / H60 FAELLT
boot_48.sav  boot_52.sav  310  35    35.0           10         H30 passt / H60 FAELLT
```

310 VBlanks ↔ 155 Pulsschritte (= 2·60 + 35). „Ein Schritt je VBlank" fällt (verlangte
Rest 10, gemessen 35). `val` stimmt in allen vier mit der aus `ctr` gerechneten Folge
überein. `vsArg` = DAT_800b5456 = 2 in **allen 13** Savestates, die im Titelmenü stehen
(State 2, Sub-State 1; `build/r30_titel-blinken/savescan.txt`). Gegenprobe: der eine
Savestate mit geladenem TITLE.BIN und `vsArg` = 0 ist `sub_loadgame.sav` — er steht im
Karten-Bildschirm (Sub-State 3), der den Wert selbst auf 0 setzt (@0x8002650c).

Grenze dieser Messung: aus Rest-Arithmetik allein wären auch 35, 95, 215 oder 275 Schritte
je 310 VBlanks verträglich. Eindeutig wird es durch (b) und (c).

**(b) Beide Bildpuffer eines Savestates halten aufeinanderfolgende Pulswerte.** Werkzeug
`r30_title_row_sim.py` rechnet die aktive Zeile aus dem Modell von §3.4 nach und vergleicht
pixelgenau mit dem VRAM (`build/r30_titel-blinken/orig_row_sim.txt`):

| Savestate | RAM-Pulswert | Puffer y=0 | Puffer y=240 | abweichende Pixel |
|---|---|---|---|---|
| boot_40 | 0x9a | 0x9c | 0x9e | 0 / 0 von 4352 |
| boot_44 | 0xb0 | 0xac | 0xae | 0 / 0 |
| boot_48 | 0x86 | 0x88 | 0x8a | 0 / 0 |
| boot_52 | 0xb8 | 0xbc | 0xba | 0 / 0 |
| mzd_title | 0xa0 | 0xa4 | 0xa2 | 0 / 0 |
| nav_down1 | 0xbc | 0xbc | 0xbe | 0 / 0 |

Zwölf Bildpuffer, zwölfmal 0 abweichende Pixel. Damit ist das Modell aus §3.4
(Modulation `min(31, (texel5 · wert) >> 7)`, Reihenfolge subtraktiv-dann-additiv) **am
Bild gemessen**, nicht nur gelesen — und jedes gezeigte Bild trägt genau einen Pulsschritt.

**(c) DuckStation-Aufnahme** `shots/title_confirm_capture.avi` (640×480, 59,8173 Bilder/s,
ein Bild je VBlank; Bilder 0-68 zeigen das stehende Menü vor dem Bestätigen). Helligkeit
der Zeile NEW GAME je Bild, Werkzeug `r30_orig_capture_pairs.py`
(`build/r30_titel-blinken/orig_capture_pairs.txt`):

```
Bilder 1..68 (0.0502 s .. 1.1702 s)
Helligkeitsstufen: 34
Haltedauer je Stufe (Bilder -> Anzahl): {2: 34}
Bildabstand der Aufnahme: 16.7176 ms  (59.8173 Bilder/s)
mittlere Haltedauer (ohne Randstufen): 2.000 Bilder = 33.435 ms
=> 60 Pulsschritte = 2006.1 ms
hellstes Bild: 12 (YAVG 56.3144); danach 28 Stufen abwaerts bis Bild 68
```

34 Stufen, **jede hält genau 2 Bilder**. (Die Aufnahme deckt 34 von 60 Schritten; die
volle Periode in einem Stück ist über (a) abgedeckt: 310 VBlanks = 2,58 Perioden.)

### 3.7 Ergebnis Original

- Periode = **60 Aufrufe × 2 VBlanks = 120 VBlanks**.
- In Zeit: **2006,1 ms** bei den gemessenen 59,8173 Hz; 2005,8 ms bei 59,826 Hz (psx-spx).
- Schrittdauer 33,435 ms (gemessen).
- Wirkung: Helligkeit der aktiven Zeile, Faktor 1,000 … 1,484, 31 Schritte hoch,
  28 runter, Rücksprung.

RE2 wird hier nicht herangezogen: RE1.5 hat dieses System vollständig, also gilt RE1.5.

---

## 4. URSACHE

### 4.1 Frequenz (der gemeldete Fehler)

`re15_render_pc_title_menu()` (render_pc.c:1790-1805) **zeichnet und zählt in einem**: der
Pulsschritt (Zeilen 1803-1804) läuft bei jedem Aufruf. Die Titel-Schleife
(main.c:2959-3114) ruft die Funktion in jedem Schleifendurchgang (main.c:2971) und hat
**keine eigene Taktung**: ihr einziger Takt ist `SDL_RenderPresent` mit
`SDL_RENDERER_PRESENTVSYNC` (render_pc.c:563), also die Bildrate der Anzeige.

| | Original | Port |
|---|---|---|
| Pulsschritt je | Durchgang der Hauptschleife = VSync(2) | Schleifendurchgang = 1 Bild der Anzeige |
| Rate | 59,8173 / 2 = 29,91 Hz | 144,11 Hz (gemessen, diese Maschine) |
| Periode | 2006 ms | 416 ms |

Die übrigen Front-End-Schleifen haben einen Zeitdeckel (Player-Select main.c:2201-2202,
Config main.c:2687, Bestätigungs-Fade main.c:3025-3026) — die Titel-Schleife nicht. Der
Kommentar main.c:3086 („Bei 60-fps-Frames: Tick = tblink>>1") setzt 60 Durchgänge je
Sekunde voraus; erzwungen wird das nirgends.

Nicht die Ursache (gemessen): Zählerstart (0/0x80 wie Datei 0x2944/0x2946), Schrittweite,
Aufrufe je Periode (60), doppelter Aufruf je Bild (genau einer je Durchgang).

### 4.2 Puls im Fade

Die Fade-Schleife ruft dieselbe Funktion (main.c:3020) und zählt damit den Puls weiter
(§2.4: 330 Änderungen, Periode 1000 ms). Im Original ruht er (§3.5).

### 4.3 Nebenbefund: Helligkeit der aktiven Zeile

render_pc.c:1311: `mod = 200 + (val - 0x80) * 55 / 0x3e` → Faktor 0,784 … 1,000. Diese
Abbildung trägt **keine Adresse** und ist nicht die des Originals (Faktor `val/128` =
1,000 … 1,484, Sättigung 31). Der Port ist im hellsten Zustand höchstens so hell wie das
Original im dunkelsten. Gemessen: 431-479 von 4352 Pixeln der aktiven Zeile weichen ab
(`frame_check_port.txt`), während alle inaktiven Regionen pixelgenau stimmen.

### 4.4 Folgefehler derselben Ursache: Titel-Einblende

main.c:3106 rechnet `tk = tblink >> 1; B = 255 - tk*8` — B erreicht 0 nach 64
Schleifendurchgängen. Mit der gemessenen Schleifenrate 144,11 Hz sind das 0,444 s; das
Original braucht 32 Durchgänge à 2 VBlanks = 1,07 s (`FUN_800217b0(0x200,-0x400,7,3)`
@0x80102054-64, Schritt 0xfc00 @0x80102058). **Die Dauer der Einblende im Port habe ich
nicht gemessen** — die 0,444 s sind aus der gemessenen Schleifenrate und der Code-Zeile
gerechnet.

---

## 5. UMSETZUNGSPLAN (für den Bau-Agenten)

Drei getrennte Schritte, drei getrennte Commits. Schritt 1 ist der Auftrag.

### Schritt 1 — Titel-Tick: ein Pulsschritt je 2 VBlanks

**Dateien:** `re15_port/platform/pc/src/render_pc.c`, `re15_port/platform/pc/main.c`;
neu (damit der Pulsschritt ohne SDL testbar ist) `re15_port/engine/src/title_pulse.c` +
`re15_port/include/re15_title_pulse.h`.

1. **Pulsschritt aus dem Zeichnen lösen.**
   - neu `void re15_title_pulse_reset(void)` — Zähler 0, Wert 0x80 (Datei TITLE.BIN
     0x2946 / 0x2944).
   - neu `void re15_title_pulse_step(void)` — der Rumpf von FUN_801028ec, unverändert
     gegenüber render_pc.c:1803-1804.
   - neu `int re15_title_pulse_value(void)`.
   - `re15_render_pc_title_menu()` setzt nur noch Textur/Cursor/Sichtbarkeit und liest den
     Wert; **kein** Schritt mehr darin.

   | Konstante | Wert | Beleg |
   |---|---|---|
   | Schwelle aufwärts | Zähler < 0x1f, Vergleich vor dem Inkrement | @0x801028fc |
   | Schritt hoch | +2 | @0x8010290c |
   | Schritt runter | −2 | @0x80102910 |
   | Periode | 0x3c Aufrufe | @0x80102918 |
   | Rücksetzwert Zähler / Wert | 0 / 0x80 | @0x80102924 / @0x80102928 |
   | Startwert Wert / Zähler | 0x0080 / 0x0000 | TITLE.BIN Datei 0x2944 / 0x2946 |

2. **Titel-Tick in die Titel-Schleife** (main.c, `while (re15_gameflow_mode() ==
   RE15_MODE_TITLE)` ab 2959). Der Tick läuft über die **Zeit**, nicht über die Zahl der
   Schleifendurchgänge — sonst hängt er wieder an der Anzeige:

   ```
   tick_soll = vergangene_Zeit / T_TICK        (ganzzahlig)
   solange tick_ist < tick_soll:  re15_title_pulse_step(); tick_ist++
   ```

   | Konstante | Wert | Beleg |
   |---|---|---|
   | VBlanks je Tick | 2 | `ori v0,zero,2` @0x8002130c → `sb` @0x80021314 (DAT_800b5456); gelesen @0x8002147c, VSync @0x80021480 |
   | Abgeben je Tick | 1 Durchgang | `ori a0,zero,1` @0x80101fc0 (FUN_80029ac8 @0x80101fbc) |
   | Pulsschritte je Tick | 1 | `jal 0x801028ec` @0x80102ba0 |
   | VBlank-Rate | 59,826 Hz | psx-spx `graphicsprocessingunitgpu.md` Z. 1260-1264 (NTSC non-interlaced); isinter = 0 @0x800b5438 (Savestate); DuckStation-Aufnahme 59,8173 |
   | T_TICK | 2 / 59,826 s = 33,430 ms | aus den beiden Zeilen darüber |

   Die Schleife selbst läuft weiter im Takt der Anzeige (Eingabe, Audio, `tblink`
   unverändert). `tblink` bleibt der Zähler der **Schleifendurchgänge** — an ihm hängen
   `RE15_TITLE_SHOT_AF`, `RE15_CONTINUE_TEST` (tblink 8 / 16) und fünf Integrations-Pins.

   Aufholen nach einem Stillstand (Fenster gezogen, Haltepunkt) begrenzen und danach neu
   aufsetzen. Die Grenze ist Port-Infrastruktur und hat kein Original-Gegenstück; sie
   darf den Normalfall (höchstens 1 Tick je Durchgang bei Anzeigen ab 30 Hz) nicht
   berühren.

3. **Einblende auf denselben Tick legen** (main.c:3106): `B = 255 - tick*8` statt
   `(tblink >> 1) * 8`. Beleg: Schritt −0x400 je Fade-Tick (`ori a1,zero,0xfc00`
   @0x80102058, `jal 0x800217b0` @0x80102060), B = level >> 7 → 8 je Tick;
   FUN_80021880 einmal je Durchgang (@0x80020f44). Bei `tblink = 0`-Rücksetzungen
   (main.c:3073, 3081) den Tick-Nullpunkt der Einblende mit zurücksetzen — **nicht** den
   Puls (der läuft im Original über die Unterbildschirme hinweg weiter, s. §7).

4. **Kommentare berichtigen:** render_pc.c:1761 („the 60-frame highlight pulse are
   not yet reproduced") und main.c:2996 („480i-Title" → VSync(2), isinter = 0).

### Schritt 2 — Puls ruht im Bestätigungs-Fade

**Datei:** `re15_port/platform/pc/main.c:3020`. Nach Schritt 1 erledigt sich das von
selbst, sofern die Fade-Schleife `re15_title_pulse_step()` **nicht** ruft. Beleg:
`jal 0x80102a10` @0x80102d10 / @0x80102d60; in FUN_80102a10 (0x80102a10-0x80102a88) kein
`jal 0x801028ec`.

### Schritt 3 — Helligkeit der aktiven Zeile (Nebenbefund)

**Datei:** `re15_port/platform/pc/src/render_pc.c` (tmoji_strip ab 1765, Zeichnen
1302-1318).

- Die Zeile `mod = 200 + (val - 0x80) * 55 / 0x3e` (1311) entfällt.
- Die aktive Zeile wird je Pulswert als eigene Textur gebaut:
  `kanal5 = min(31, (texel5 * wert) >> 7)`, dann `<< 3`. 32 Pulswerte (0x80 … 0xBE,
  Schritt 2) × 3 Zeilen, beim ersten Gebrauch erzeugt und behalten. Farbmodulation der
  Textur bleibt 255.
  ⚠ Keine `SDL_UpdateTexture` auf eine schon gezeichnete STATIC-Textur (Skill
  re15-port-visual-verify) — neue Textur je Wert.
- Mischreihenfolge und -arten bleiben (subtraktiv bei y+1 zuerst, additiv bei y danach).

| Konstante | Wert | Beleg |
|---|---|---|
| Farbbytes der aktiven Zeile | R = G = B = Pulswert | `lhu` @0x80102848, `sb` @0x80102850 / 54 / 58 |
| Farbbytes inaktiv | 0x80 | Befehl 0x66808080 @0x80102830-38, Zweig @0x8010283c |
| Befehl | 0x66 = Rechteck, texturiert, halbtransparent, moduliert | @0x80102830-34 |
| Modulation | texel · farbe / 128, Sättigung 31 | psx-spx „Modulation"; am Bild gemessen §3.6 (b) |
| Texpage additiv / subtraktiv | 0xe10000b5 / 0xe10000d5 | @0x80102800-04, @0x80102824 / @0x8010281c |
| Versatz Schatten | y + 1 | @0x80102810-14 |

### Riegel / Sonde

1. `probe_r30_titel_blinken_puls` (liegt vor, ohne `add_test`): die Original-Seite.
2. Neuer Unit-Test (Bau-Agent): `re15_title_pulse_step()` 180-mal gegen die aus
   TITLE.BIN ausgeführte Folge — der Mini-R3000 der Sonde lässt sich übernehmen.
   Soll: 180 von 180 Wertepaaren gleich.
3. Neuer Unit-Test für die Tick-Rechnung: 2 005 800 µs → 60 Ticks; 33 429 µs → 0;
   33 431 µs → 1.

### Wie gemessen wird, dass es stimmt

| Messung | Werkzeug | Soll |
|---|---|---|
| Periode, Zeitstempel | `r30_build_instr.sh` (Anker an die neue Stelle des Pulsschritts legen) + `r30_pulse_log_stats.py` | Periode 2006 ms ± 1 Bild der Anzeige; 60 Schritte je Periode; Werte 0x80 … 0xBE |
| Periode, echtes Fenster | `r30_port_capture.sh <exe> <out.mkv> 10` + `r30_yavg_period.py` | 2006 ms; inaktive Zeile Spanne 0,000 |
| Unabhängigkeit von der Anzeige | derselbe Lauf mit `RE15_SOFTWARE_RENDER=1` (kein VSync) — **nur** für die Zeitmessung | Periode bleibt 2006 ms |
| Puls im Fade | instrumentierter Lauf mit `RE15_INPUT_SCRIPT="W20,A1,W60"` | 0 Änderungen des Pulswerts ab dem Bestätigen |
| Helligkeit (Schritt 3) | gdigrab-Bild → `r30_port_frame_check.py <bild.png> <cursor>` | 0 abweichende Pixel in allen vier Regionen, bester Pulswert in 0x80 … 0xBE |

Das Abnahme-Werkzeug `r30_port_frame_check.py` rechnet nur aus `DATA/TITLEU.TIM` und
`DATA/TMOJI.TIM` und ist an sechs Original-Bildpuffern geprüft
(`build/r30_titel-blinken/frame_check_orig.txt`, sechsmal „deckt sich pixelgenau").
Für den Port v0.8.15 meldet es erwartungsgemäß ABWEICHUNG (`frame_check_port.txt`).

---

## 6. Risiken / offene Fragen

- **Fenster-Verwechslung bei gdigrab.** Mehrere Agenten, derselbe Fenstertitel. Nur über
  das Fenster-Handle aufnehmen (`r30_port_capture.sh`).
- **`tblink`-Haken.** `test_boot_bg_pin`, `test_dark_start_pin`, `test_relatch_pin`,
  `test_save_counter_pin`, `test_weste_load_pin` fahren über `RE15_TITLE_SHOT` /
  `RE15_CONTINUE_TEST`. Sie prüfen den Inhalt von `title.bmp` nicht; solange `tblink` der
  Durchgangszähler bleibt, ändert sich für sie nichts.
- **`test_dark_start_pin`** verlässt den Titel bei `tblink = 4` mit noch laufender
  Einblende. Mit dem Zeit-Tick ist B dort 255 statt 239 — der Pin prüft weiter, dass der
  Rest beim Verlassen gelöscht wird.
- **Neue Datei unter `engine/src/`:** der Android-Bau friert die GLOB-Liste ein
  (Memory `reai-v2-android-glob-cache`).
- **GUI-Tests unter Last** (Memory `reai-v2-gui-tests-flattern-bei-parallelen-agenten`).
- **Bildrate des Nutzers unbekannt.** Gemessen ist der Faktor 4,82 auf dieser Maschine
  (144 Hz). Für andere Anzeigen folgt aus „ein Schritt je Bild der Anzeige" rechnerisch
  1000 ms bei 60 Hz — gemessen habe ich das nicht.

---

## 7. Was ich ausdrücklich NICHT belegen konnte

1. **Periode des Ports auf einer 60-Hz-, 90-Hz- oder 120-Hz-Anzeige** — NICHT GEMESSEN
   (nur 144 Hz verfügbar; die Bildrate umzustellen hätte die anderen Agenten gestört).
2. **Dauer der Titel-Einblende im Port** — NICHT GEMESSEN, nur gerechnet (§4.4).
3. **Neustart des Pulses beim Wiedereintritt in den Titel** (nach Tod / Soft-Reset) —
   NICHT BELEGT. `mzd_title_after_death.sav` (ctr 50, val 0x98) zeigt nur, dass der Puls
   dort läuft, nicht mit welchem Startwert. Ob TITLE.BIN dabei neu von der CD kommt
   (dann 0x80/0), habe ich nicht verfolgt.
4. **Puls über die Unterbildschirme hinweg.** `mzd_options.sav`, `mzd_edit_panelA/B.sav`
   und `sub_option.sav` zeigen den Titel-Task im Zustand 0x41 mit stehendem Puls
   (0xbc/32 bzw. 0x8e/55). Dass er nach der Rückkehr von dort weiterzählt und nicht neu
   beginnt, folgt aus dem Code (kein Schreiber außer FUN_801028ec), ist aber nicht an
   einem Savestate-Paar vor/nach gemessen.
5. **Bildrate echter Hardware.** 59,826 Hz (psx-spx) gegen 59,8173 Hz (DuckStation) —
   0,015 % Unterschied, 0,3 ms je Periode. Welche Zahl die Konsole trifft, ist hier nicht
   entscheidbar.
6. **Sub-State 0 (FUN_80102a8c)** — im ausgelieferten Ablauf nicht erreicht (kein
   Savestate mit Sub-State 0 im State 2); nicht dynamisch untersucht.
7. **Warum `port_puls_maximum.png` nicht mit dem Original bei 0x80 zur Deckung kommt**
   (431 Pixel Abweichung, obwohl der Port dort rechnerisch Faktor 1,0 zeigt). Die
   Aufnahme lief mit ~83 Bildern/s gegen 144 Schritte/s; ob das Bild den Scheitel
   verfehlt hat oder SDLs 8-Bit-Modulation anders rundet, habe ich nicht getrennt.
   Für den Plan ohne Belang (Schritt 3 ersetzt die Modulation).
