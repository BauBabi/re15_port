# Runde 30, Thema I — Hund: der Spieler stirbt nicht ("hund-tod")

Stand: master 437905cb, v0.8.15. Phase ERMITTLUNG, kein Bau. Dossier wird fortlaufend geschrieben.
Werkzeuge: `analysis/befunde_runde30/hund-tod_tools/`, Sonde `re15_port/tests/unit/probe_r30_hund_tod.c`
(+ `probes/r30_hund-tod.cmake`), Bau `re15_port/build_r30_hund-tod`, Ausgaben `build/r30_hund-tod/`.

## 1. Symptom / Auftrag

Nutzer (2026-09-28): "I cannot die from a dog. He bites my neck, and i am standing again."

Beleg `analysis/befunde_runde30/nutzer_marken/befund_hund_2026-09-28.log` (eine Zeile je 15 Bilder).
Eigene Auszaehlung der hp-Laeufe (awk ueber Spalte 2/4, `uniq -c`):

| Raum | hp | Zeilen (x15 Bilder) | Deutung |
|---|---|---|---|
| R1190 | 100 -> 80 -> 60 -> 40 | 35 / 10 / 3 / 6 | drei Bisse zu je 20 |
| R11D0 | 40 -> 20 | 48 / 53 | vierter Biss, 20 |
| R11D0 | 0 | 4 (= 60 Bilder) | fuenfter Biss: 20 - 20 = 0, Spieler LEBT (hp >= 0) |
| R11D0 | -20 | **10 (= 150 Bilder)** | sechster Biss: 0 - 20 = -20 = toedlich, Kehlbiss laeuft |
| R11D0 | 0 | 43 | **hp wieder 0, Spieler steht und laeuft** |
| R1230 / R11D0 | 0 | 4 / 27 | Raumwechsel mit hp 0 |
| R11D0 | -20 | **10 (= 150 Bilder)** | erneut toedlich gebissen |
| R11D0 | 0 | 14, dann R1230 10 | **wieder auferstanden** |

Jeder Biss kostet 20; die toedliche Phase dauert beide Male genau 10 Logzeilen (135..150 Bilder),
danach steht hp auf 0. Das ist ein Schreiber mit festem Takt, kein Zufall.

## 2. MESSUNG im Port

Sonde `re15_port/tests/unit/probe_r30_hund_tod.c` (Registrierung `probes/r30_hund-tod.cmake`, kein
`add_test`). Sie laedt die echte `ROOM11D0.RDT`, faehrt main00 + sub00 hoch (freier Hunde-Satz, Flag
3:152 = 1, Else-Zweig @Datei 0x13EE), laedt die RE2-Bank EM020 aus `shared_assets/RE2/CDEMD0.EMS`,
stellt den Spieler mit **hp 20** auf die Nutzer-Lage (-7878, 0, -17384) und tickt den echten
`re15_game_step` mit der Live-KI (Flavor RE2 = Auslieferungs-Default).

**Messfalle, vor dem ersten gueltigen Lauf gefunden:** `re15_enemy_reset()` loescht alle Baenke
(`memset`, `engine/src/enemy_common.c:129-134`). Eine Bank, die VOR dem Hochfahren geladen wird, ist
danach weg; die Hunde-Clips haben dann Laenge 1, die Opfer-FSM startet nie (gemessen: `vs=0`, Clip
23/24/25 je EIN Bild, 0 von 32 Laeufen auferstanden = falsches Gruen). Die Sonde laedt deshalb NACH
dem Reset und protokolliert `[bank] 0x20 ok=1 victim_ok=1 clips=27 victim_clips=1`.
(`probe_r27_hund_biss.c` laedt vor dem Reset — dessen Hunde-Messungen tragen denselben Fehler; nicht
Gegenstand dieses Themas, aber fuer spaetere Runden vermerkt.)

### 2.1 Abdeckung: 16 Blickrichtungen x 3 Druck-Arten (`suche`, `build/r30_hund-tod/suche_hp20.log`)

Aufruf `probe_r30_hund_tod suche 20 1500 -7878 -17384 3`:

```
ABDECKUNG: Laeufe 48 | toedlich gebissen 48 | mit Latch 46 | AUFERSTANDEN 15 | Game Over 33
```

| Druck | Laeufe | davon Latch (Kehlbiss) | auferstanden | Game Over erreicht |
|---|---|---|---|---|
| 0 = keine Taste | 16 | 15 | **0** | 16 |
| 1 = Kreuz jedes 2. Bild | 16 | 15 | **15** | 1 (der eine Lauf OHNE Latch, yaw 2048) |
| 3 = R1 gehalten (zielen) | 16 | 16 | **0** | 16 |

R1 liegt nicht in der Tastenmaske 0xF0F0 der Mash-Abfrage; wer nur zielt, stirbt.

Jeder Latch-Lauf MIT Tastendruck endet mit hp 0 und einem freien, stehenden Spieler. Jeder Lauf OHNE
Tastendruck endet im Game Over. Der Lauf ohne Latch (Boden-Biss, yaw 2048) stirbt in beiden Faellen
ueber cmd 3 (`cmd3@284`, `gameover@544`). Der Fehler sitzt also ausschliesslich im Latch-Pfad.

### 2.2 Verlauf je Bild (yaw 0; `lauf_druck{0,1,2}_hp20_yaw0.log`)

Spalten: Bild, hp, Spieler-state, Opfer-FSM `vs`, Griff `gr`, tot, Praesentation, Game Over | Hund:
sub_state_1/2/3, Clip, Frame, `+0x21E`, Port-Marker `re2d_abort21c`.

| Bild | hp | st | vs | gr | tot | praes | go | Hund s1/s2/s3 clip/afr | 21E | abort | Ereignis |
|---|---|---|---|---|---|---|---|---|---|---|---|
| 0 | 20 | 0 | 0 | 0 | 0 | 0 | 0 | 0/0/0 1/19 | 0 | 0 | Start |
| 93 | **0** | 0 | 0 | 0 | 0 | 0 | 0 | 3/0/1 20/3 | 0 | 0 | 1. Biss 20-20 = 0, ueberlebt |
| 102 | **-20** | 0 | 1 | 1 | 0 | 0 | 0 | (anderer Hund) | 0 | 0 | 2. Biss, Rueckgabe 2, LATCH |
| 103 | -20 | 0 | 3 | 1 | 0 | 0 | 0 | | | | Opfer-FSM Phase 3 (Clip 0 der Opfer-Bank) |
| 249 | -20 | 0 | 0 | 1 | 1 | 1 | 0 | 7/0/1 23/143 | 2 | **1** (Druck) / 0 | Opfer-Clip zu Ende |
| 251 (Druck 1) | **0** | 0 | 0 | 0 | 0 | 0 | 0 | 7/1/0 24/0 | 2 | 0 | **hp := 0, Spieler frei** |
| 264 (Druck 1) | 0 | 1 | 0 | 0 | 0 | 0 | 0 | 7/1/0 24/13 | | | Spieler laeuft wieder (state 1) |
| 251 (Druck 0) | -20 | 7 | 2 | 1 | 1 | 1 | 0 | 7/1/0 24/0 | 2 | 0 | state 7 + Devour-Kollaps |
| 510 (Druck 0) | -20 | 7 | 2 | 1 | 1 | 1 | **1** | | | | Game Over |

Latch -> Auferstehung = 251 - 102 = **149 Bilder**. Das Nutzer-Log zeigt die toedliche Phase zweimal
mit 10 Zeilen x 15 Bilder = 135..150 Bilder. Messung und Nutzer-Beleg decken sich.

Druck 2 (EIN Druck im ersten Bild nach dem Latch, Bild 103): kein Aufstehen, Game Over @510. Der
Druck faellt, solange der Hund noch fliegt (Sub 7 P1 laeuft noch nicht); es zaehlt also jeder Druck
WAEHREND der 144 Bilder von Clip 23, nicht einer davor.

### 2.3 WER setzt hp von -20 auf 0 (gdb-Hardware-Watchpoint auf `g_actors[0].hp`)

Werkzeug `analysis/befunde_runde30/hund-tod_tools/r30_hundtod_watch.gdb`, Ausgabe
`build/r30_hund-tod/watch_hp_druck1.log`. Drei Schreiber im ganzen Lauf:

| neuer Wert | Schreiber (Aufrufkette) |
|---|---|
| 0 | `re2z_player_damage` enemy_ai_re2_zombie.c:796 <- `re15_re2_player_damage` :837 <- `re2d_contact` enemy_ai_re2_dog.c:557 <- `re2d_flight_probe` :609 |
| -20 | dieselbe Kette, anderer Hund (`g_actors+1820` = Slot 1) |
| **0** | **`re2d_sub7_latch` enemy_ai_re2_dog.c:1253** (`pl->hp = 0;`, gdb meldet die Folgezeile 1254) <- `re15_re2dog_tick` :2429 <- `re15_dog_ai_tick` enemy_ai_common.c:7713 <- `re15_enemy_ai_run_all` :14164 <- `re15_game_step` game_step_common.c:2184 |

Der Ruecksetzer ist also der Hunde-Latch selbst, Zweig `if (pl->hp < 0) { if (e->re2d_abort21c) {
pl->hp = 0; re15_player_victim_throwoff(); } else { pl->state = 7; re15_re2z_victim_devour(e, 0); } }`
(`enemy_ai_re2_dog.c:1251-1259`). `re2d_abort21c` wird in `:1278` aus `re15_re2z_mash()` gesetzt, und
das ist `re15_mash_pressed()` = Flanke auf irgendeiner Richtungs- oder Aktionstaste (Maske 0xF0F0,
`enemy_ai_common.c:559`).

## 3. ORIGINAL-MECHANISMUS

Der Port faehrt fuer Typ 0x20 seit 2026-08-22 das RE2-Gehirn (`enemy_ai_re2_dog.c`, Modul
`info/re2leon/COMMON/BIN/EMD0G_MOD0.BIN`, 22266 Byte, roh @0x80100000). Massgeblich ist deshalb RE2;
RE1.5 steht als Gegenprobe darunter (3.6). Alle Instruktionen unten sind in dieser Runde SELBST
disassembliert (`re2_disasm.py dis ... --bin EMD0G_MOD0.BIN`, `re2_disasm.py dis ...` fuer
`info/re2leon/PSX.EXE`, DIEDEMO.BIN mit Ladeadresse 0x80190000).

### 3.1 Der Schadenseintritt FUN_800401D4 (RE2 PSX.EXE) — hier faellt die Entscheidung

```
80040248: lhu  v0,342(a2)        ; a2 = 0x800CFBF8 (Spielerblock), +0x156 = hp
80040250: subu a0,v0,a0          ; hp - Schaden
8004025c: sh   a0,342(a2)        ; hp := hp - Schaden           <- EINZIGER Abzug
80040274: bne  a1,zero,0x800402ec ; Modus != 0 (und != 1) -> Rueckgabe 1
80040284: bgez v0,0x800402e8     ; hp >= 0  -> Rueckgabe 0 (ueberlebt)
80040288: slti v0,v0,-14
8004028c: bne  v0,zero,0x800402c0 ; hp < -14 -> TOD
80040294: lhu  v0,340(a2)        ; +0x154
8004029c: andi v0,v0,0x1000      ; Einmal-Rettung schon verbraucht?
800402a0: bne  v0,zero,0x800402c0 ; ja -> TOD
800402b0: sh   zero,342(a2)      ; EINMAL-RETTUNG: hp := 0
800402b4: ori  v1,v1,0x1000
800402bc: sh   v1,340(a2)        ; Riegel setzen, Rueckgabe 1
800402c0: lbu  v1,467(a2)        ; TOD:
800402cc: ori  v1,v1,0x80
800402d0: sb   v1,467(a2)        ;   Spieler+0x1D3 |= 0x80
800402d8: lui  a0,0x400
800402e4: sw   v1,0(a1)          ;   0x800CFB74 |= 0x04000000   (globales Todesbit), Rueckgabe 2
```

Die Entscheidung "steht wieder auf" gegen "stirbt" faellt also **im Augenblick des Bisses** und nur
hier: hp >= 0 -> lebt; hp in -14..-1 und Riegel frei -> Einmal-Rettung (hp := 0); sonst Tod. Es gibt
keinen spaeteren Rueckweg.

Der Riegel (Spieler+0x154 Bit 0x1000) wird im ganzen RE2-EXE an GENAU EINER Stelle geloescht — in der
Heil-Funktion:

```
800402f8: lhu  v0,-690(v0)       ; hp
80040300: lhu  v1,-692(v1)       ; +0x154
80040304: addu v0,v0,a0          ; hp + Heilwert
80040308: andi v1,v1,0xefff      ; Riegel 0x1000 loeschen
80040310: sh   v0,-690(at)
8004031c: sh   v1,-692(at)
```

(Werkzeug `r30_hundtod_bitscan.py fd4c 1000` + `r30_hundtod_absscan.py 800cfd4c 800cfd4d`: 7 Zugriffe
im EXE; der Raumlader @0x80049E94-A8 loescht nur 0x0600 — `andi v0,v0,0xf9ff`.)

### 3.2 Der Hunde-Kontakt 0x80104DF0 — was der Hund aus der Rueckgabe macht

```
80104eb8: addiu a0,zero,20       ; Biss-Schaden 20
80104ebc: jal  0x800401d4
80104ec0: addu a1,zero,zero      ; Modus 0
80104ee0: addiu v0,zero,2
80104ee4: bne  v1,v0,0x80104f7c  ; Rueckgabe != 2 -> Boden-Biss
80104ef0: jal  0x80015910        ; Blickkontakt Hund/Spieler
80104ef8: bne  v0,zero,0x80104f7c ; != 0 -> Boden-Biss
80104f00: lbu  v0,15(s3)         ; self+0x227
80104f08: bne  v0,zero,0x80104f7c
80104f10: lbu  v1,8(s1)          ; Spieler+0x8 = FIGUREN-NUMMER
80104f18: beq  v1,v0,0x80104f78  ; == 15 -> kein Latch
80104f24: sb   v0,6(s3)          ; LATCH: self+0x21E = 2
80104f28: sh   zero,324(s0)      ; Hund-Tempo 0
80104f34: sb   v1,467(s1)        ; Spieler+0x1D3 = 255
80104f3c: sw   v0,0(s1)          ; Spieler-Wort0 |= 0xA
80104f50: sh   v0,118(s1)        ; Spieler-Yaw = Hund-Yaw + 2048
80104f60: sw   v0,0(v1)          ; 0x800CFB74 &= 0xFBFFFFFF   <- Todesbit wird ZURUECKGESTELLT
80104f74: sb   v0,467(s0)        ; self+0x1D3 |= 0x80
  ...
80104f7c: sh   v0,6(s0)          ; Boden-Biss: self+0x6 = 3
80104f84: sb   v0,6(s3)          ; self+0x21E = 1
80104f88: lh   v0,342(s1)        ; Spieler-hp
80104f90: bltz v0,0x80104fb4     ; hp < 0 -> 0x80105204
80104fa4: jal  0x801051bc        ; hp >= 0: Spieler +0x4 = 2 (Treffer), +0x5 = Richtung+2
80104fb4: jal  0x80105204        ; hp <  0: Spieler +0x4 = 3 (TOD), +0x5/6/7 = 0
```

Sprungziele selbst gelesen: 0x801051BC schreibt `sb 2,4(a0)` @0x801051E4, `sb a1+2,5(a0)` @0x801051EC,
Spieler+0x1D3 |= 0x80 @0x80105200. 0x80105204 schreibt `sb 3,4(v1)` @0x80105220 und nullt +0x5/6/7
@0x80105224-30.

**Der Kehlbiss (Latch) entsteht AUSSCHLIESSLICH auf Rueckgabe 2 = der Spieler ist bereits tot.** Er ist
die Toetungs-Animation, kein Griff, aus dem man sich befreit. Das Todesbit wird nur ZURUECKGESTELLT,
damit das Game Over nicht vor der Animation startet.

### 3.3 Sub 7 (Latch) 0x80101CDC / 0x80101D1C — wann das Todesbit wiederkommt

P0 @0x80101D5C-0x80101EBC: `sb 1,7(s2)` @0x80101D6C, Zaehler `sb 12,362(s2)` @0x80101D70/7C,
Clip-Wort 0x1F0017 (Clip 23) @0x80101D5C-84, `sw s2,-596(at)` = 0x800CFDAC := Hund @0x80101D9C,
**`sw 6,-1028(at)` = Spieler+0x4 := 6** @0x80101DBC-C4 (Spieler-Routine 6 = vom Gegner gefuehrt),
Spieler+0x1D3 |= 0x80 @0x80101DCC-F8, 0x800CFD80/84 := Hund+0x188/+0x18C @0x80101DB8-E00.

P1 @0x80102000-0x801020C4 (das Stueck, das der Port als "Mash-Fenster" gelesen hat):

```
80102000: lb   v0,362(s2)        ; Zaehler (Start 12)
80102008: beq  v0,zero,0x80102024 ; 0 -> Freigabe-Block
80102010: lbu  v0,8(s3)          ; s3 = 0x800CFBF8 -> Spieler+0x8 = FIGUREN-NUMMER
80102018: andi v0,v0,0x1
8010201c: beq  v0,zero,0x801020c4 ; gerade Nummer -> nur Zaehler-1 speichern
80102020: addiu v0,v1,-1
80102024: lbu  v0,8(s4)          ; self+0x220 Einmal-Riegel
8010202c: bne  v0,zero,0x80102070
80102034: lui  a1,0x400
80102038: ori  a1,a1,0x100       ; a1 = 0x04000100
80102048: sb   v0,8(s4)          ; self+0x220 = 1
80102058: or   v0,v0,a1
80102060: sw   v0,0(a0)          ; 0x800CFB74 |= 0x04000100   <- TODESBIT + Finisher-Bit
80102068: sh   v1,-1036(at)      ; 0x800CFBF4 |= 0x40 (Rudel-Signal)
80102074: jal  0x80015350        ; Hund faellt (Schwerkraft +40 @0x80102088-94, Bodenklemme @0x80102098-A0)
```

`Spieler+0x8` ist NICHT ein Tastensignal. Beleg aus dem EXE:
- FUN_80049E48 (`RE2_Quellcode_V2/FUN_80049e48.c:40-59`): `if (DAT_800cfc00 != DAT_800d482c) { ...
  DAT_800cfc00 = (byte)DAT_800d482c; FUN_80068f9c(DAT_800d482c); ...` — 0x800CFC00 = 0x800CFBF8+8 wird
  aus der gespeicherten Figuren-Nummer 0x800D482C gesetzt (Lesen `lbu v0,8(s2)` @0x80049F20, Schreiben
  `sb v0,8(s2)` @0x80049FD4).
- FUN_8001B934 (`RE2_Quellcode_V2/FUN_8001b934.c`): `if ((DAT_800cfc00 & 1) != 0)` waehlt Datei 0x1D6
  statt 0x1D5 (Stimmen-Satz der weiblichen Figur).
- 0x80104F10-18 oben: Figur 15 bekommt gar keinen Latch.

Bit 0 von `Spieler+0x8` heisst also "ungerade Figur" (die Claire-Seite). Fuer sie laeuft der
Freigabe-Block sofort, fuer die gerade Figur (Leon) nach 12 Bildern.

Gegenprobe ueber das ganze Hunde-Modul (Werkzeuge in `analysis/befunde_runde30/hund-tod_tools/`):

| Frage | Werkzeug | Ergebnis |
|---|---|---|
| ruft das Modul die Mash-Abfrage 0x8001598C? | `r30_hundtod_jalscan.py 8001598c` | **0 Aufrufe** (Zombie-Overlay EMOVL10_S0: 1, @0x80102860) |
| liest das Modul das Pad 0x800CE300..1F? | `r30_hundtod_absscan.py 800ce300 800ce31f` | **0 Zugriffe** |
| schreibt das Modul die Spieler-hp? | `r30_hundtod_offscan.py 342` | 8 Leser, 2 Schreiber; beide Schreiber `sh v0,342(s0)` @0x80100234 / @0x8010387C = die EIGENE hp des Hundes (INIT / Neuwurf) |
| wie oft ruft es den Schadenseintritt? | `r30_hundtod_jalscan.py 800401d4` | 1 Aufruf, @0x80104EBC |

0x8001598C selbst: `lw v0,-7408(v0)` (0x800CE310) / `andi v0,v0,0x34f` / `sltu v0,zero,v0`
@0x8001598C-A0 — das ist die echte Tastenabfrage, und der Hund benutzt sie nicht.

### 3.4 Der Spieler im Kehlbiss: Hunde-eigener Handler 0x80104ACC

```
80104ae8: lbu  v1,5(s0)          ; Spieler+0x5 (Phase)
80104b10: sb   v0,5(s0)          ; P0: Phase := 1
80104b1c: sw   v1,332(s0)        ;     Clip-Wort 0x001F0000 = Clip 0
80104b28: sb   v0,448(s0)        ;     +0x1C0 |= 1
80104b34: lhu  v0,118(s0)        ; P1: Yaw +2048 fuer die Platzierung
80104b40: jal  0x80015cb8
80104b60: addiu v0,v0,-2048      ;     Yaw zurueck
80104b68: jal  0x8002959c        ;     Clip vorruecken (a3 = 128)
80104b70: beq  v0,zero,0x80104b7c
80104b78: sb   v0,5(s0)          ;     Clip zu Ende -> Phase := 2
80104b04: j    0x80104b7c        ; P2: NICHTS (Endpose steht)
```

Clip 0 der Opfer-Bank des RE2-Hundes (EM_TYPE20.EMD Paar 3, 1 Clip): **145 Bilder, Kopfhoehe Bild 0
= -2426 (steht), letztes Bild = -160 (Kopf am Boden)** — gemessen mit der gerenderten Pose
(`probe_dog_victim_pose`, Ausgabe `build/r30_hund-tod/victim_pose.log`; Referenz stehend -2513). Der
Clip IST die Todesanimation: Leon geht zu Boden und bleibt liegen. Ein Aufsteh-Clip existiert in der
Bank nicht.

### 3.5 Wann kommt das Game Over (RE2)

Hauptschleife, EXE:

```
800266c8: lw   v0,0(s0)          ; s0 = 0x800CFB74
800266cc: lui  v1,0x400
800266d4: beq  v0,zero,0x80026738 ; Todesbit 0x04000000 nicht gesetzt -> weiter
800266dc: jal  0x80032138        ; laeuft Aufgabe 1 schon?
8002671c: jal  0x80031e80        ; FUN_80031e80(1, 2) = Game-Over-Aufgabe starten
80026734: sw   v0,0(s0)          ; Todesbit loeschen
```

Die Aufgabe ist `COMMON/BIN/DIEDEMO.BIN` (14536 Byte, Ladeadresse 0x80190000, Zustandstabelle
@0x801936D4 = {0x80190118, 0x80190344, 0x80190388, 0x80190EEC, 0x80190FB0, 0x801911A8, 0x801912CC}).
Zustand 0 liest das Finisher-Bit 0x100, das der Hund mitgesetzt hat:

```
80190120: lw   v1,-1164(v1)      ; 0x800CFB74
8019015c: andi v1,v1,0x100
80190160: beq  v1,zero,...       ; kein Finisher -> keine Wartezeit
8019016c: lw   a2,-596(a2)       ; 0x800CFDAC = der Greifer (vom Hund in P0 gesetzt)
80190174: lbu  a1,8(a2)          ; dessen Typ
801901cc: lbu  v0,14020(at)      ; Typ-Tabelle  @0x801936C4
8019021c: lhu  v0,13568(at)      ; Wartezeit    @0x80193500[index]
80190224: sh   v0,14504(at)      ; -> 0x801938A8
801902f8: andi v0,v1,0x800
80190300: andi v0,v1,0x100       ; (0x800 UND 0x100) -> Zustand += 1 (WARTEN), sonst += 2
```

Zustand 1 @0x80190344 zaehlt 0x801938A8 herunter und schaltet bei < 0 weiter.
Tabellen (selbst gelesen): Typen @0x801936C4 = {0, 16, 34, **32**, 48, 39, 46, 51, 49, 35, 42, 43, 52,
54, 36, 255}; Wartezeiten @0x80193500 = {180, 95, 10, **90**, 145, 145, 0, 84, 81, 0, 0, 90, 95, 100,
10, 0}. Hund 0x20 = 32 = Index 3 -> **90 Bilder**.

Ablauf RE2 also: toedlicher Biss -> Latch -> P0 -> 12 Bilder (Leon) -> Todesbit -> Game-Over-Aufgabe
-> (falls Bit 0x800) 90 Bilder Warten -> Todes-Praesentation. Die Toetungs-Animation (145 Bilder)
laeuft darunter weiter.

Bit 0x800 von 0x800CFB74: Leser sind DIEDEMO (4 Stellen), Hund @0x80101DDC und Zombie @0x8010B53C
(dort verschiebt es die Rumble-Zeitpunkte um 15). Ein Setzer mit Direktwert ist weder im EXE noch in
den 25 Overlays zu finden (`r30_hundtod_bitscan.py fb74 800`). **Bedeutung NICHT BELEGT.**

### 3.6 Gegenprobe RE1.5 (STAGE1.BIN + PSX.EXE)

Spieler-Schadenseintritt FUN_80012D60:

```
80012ee8: bgez v1,0x80012f00     ; hp >= 0 -> nichts
80012ef0: ori  v0,zero,0x3
80012ef4: sb   v0,4(s1)          ; Kommando 3 = TOD
80012ef8: sb   zero,5(s1)
80012efc: sb   zero,6(s1)
```

RE1.5-Hund, Biss:

```
8010f2bc: lhu  v0,-13586(v0)     ; Spieler-hp
8010f2c8: addiu a0,v0,-10
8010f2d0: sh   a0,-13586(at)     ; hp -= 10
8010f2d4: lh   v0,484(v1)        ; self+0x1E4 (scharf?)
8010f2dc: bne  v0,zero,0x8010f468 ; scharf -> Griff
8010f2e4: bltz v0,0x8010f468     ; hp < 0  -> Griff (zweite Stelle gleich: @0x8010f44c-60)
8010f468: jal  0x8001a780        ; Blickkontakt
8010f478: addiu v0,v0,9
8010f47c: sb   v0,5(v1)          ; Sub 9/10 = Griff
```

RE1.5-Hund, Fress-Schleife des Griffs:

```
8010fa64: lbu  a0,158(v1)        ; +0x9E Fress-Zaehler
8010fa70: beq  a0,zero,0x8010fa8c ; 0 -> Sub 0xB
8010fa7c: lh   v0,-13586(v0)     ; Spieler-hp
8010fa84: bgez v0,0x8010fa9c     ; hp >= 0 -> Tastenabfrage
8010fa98: ori  v1,zero,0xb       ; hp <  0 -> Sub 0xB (gefressen)   <- KEINE Tastenabfrage
8010fa9c: jal  0x80037024        ; Tastenabfrage (nur mit hp >= 0)
8010facc: sh   a0,156(a1)        ; +0x9C -= 1 + 100 * Taste
8010faec: sb   v0,6(v1)          ; < 0 -> Schritt 4 (befreit)
```

RE1.5 kennt also ein Freikaempfen aus dem Hunde-Griff — aber nur solange der Spieler lebt. Bei hp < 0
ueberspringt auch RE1.5 die Tastenabfrage (@0x8010FA84). **Beide Originale sind sich einig: nach einem
toedlichen Biss steht niemand wieder auf.**

## 4. URSACHE im Port

### 4.1 Hauptursache D1 — der Latch belebt den Spieler wieder (`enemy_ai_re2_dog.c:1245-1260`)

```c
if (pl->hp < 0) {
    if (e->re2d_abort21c) {                /* Mash-Escape (Port-Marker) */
        pl->hp = 0;
        re15_player_victim_throwoff();
    } else {
        pl->state = 7;
        re15_re2z_victim_devour(e, 0);
    }
}
```

Der Kommentar darueber nennt es selbst "PORT-MAPPING, dokumentiert OPEN": der Spieler-Handler der
Greif-Art 6 sei "nicht RE'd", deshalb "Mash im 12er-Fenster = One-Save (HP=0, Throw-off-Choreo)".
Dahinter stehen zwei Fehllesungen:

1. `lbu v0,8(s3)` / `andi v0,v0,0x1` @0x80102010-18 wurde als "Spieler-Struggle-Signal" gelesen und
   auf `re15_re2z_mash()` gelegt (`enemy_ai_re2_dog.c:1277`). Es ist die Figuren-Nummer (3.3).
2. `0x800CFB74 |= 0x04000100` @0x8010204C-60 wurde als "Spieler-Break-Free-Signal" gelesen. Es ist das
   Todesbit plus das Finisher-Bit fuer DIEDEMO (3.5) — das Gegenteil.

Der Handler der Greif-Art 6 ist inzwischen gelesen: Spieler-Routinen-Tabelle @0x800A4030, Eintrag [6]
= 0x800400D0, und der ist ein reiner Verteiler — `lw a2,436(a0)` (Greifer), `lbu v0,8(a2)` (dessen Typ),
`lw v0,-6104(v1)` = Tabelle 0x800CE400[Typ], `jalr v0` @0x800400F4-11C. Den Eintrag fuer 0x20 setzt das
Hunde-Modul selbst: `addiu v0,v0,19148` (0x80104ACC) / `sw v0,-7040(at)` (0x800CE480) @0x801004AC-B4.
Mehr als 0x80104ACC (3.4) gibt es auf der Spielerseite nicht.

Der Port hat damit eine Rettung eingebaut, die es in keinem der beiden Originale gibt, und sie an
eine Taste gehaengt, die jeder Spieler im Griff drueckt (Maske 0xF0F0 = jede Richtung, jede
Aktionstaste).

### 4.2 Zweite Ursache D2 — die Opfer-FSM laeuft im falschen Zustand und gibt den Toten frei

Auch OHNE Tastendruck ist der Ablauf falsch (Riegel-Lauf vor dem Bau,
`build/r30_hund-tod/riegel_vor_dem_bau.log`: 12 von 12 Laeufen):

| Messgroesse | Druck 0/2/3 (9 Laeufe) | Druck 1 (3 Laeufe) |
|---|---|---|
| Bilder mit STEH-Pose (motion 200) bei hp < 0 | 1..2 | 1..2 |
| Opfer-Clip neu gestartet | **1** | 0 (der Spieler ist da schon frei) |
| auferstanden | 0 | **3 von 3** |

Mechanismus, gemessen mit `RE15_RE2_TRACE=1` (`build/r30_hund-tod/re2_ki.log` Zeile 60):

```
[victim] AUTO-RELEASE vs1->3 type=0x20 zslot=1 (zst=1 zsub=3)
```

`re2d_contact` startet die Opfer-FSM im Zustand 1 (`re15_re2z_victim_begin`, `:584`). Der Hund ist in
diesem Bild noch im Flug (Sub 3) und bestellt den Griff-Pin nicht; der Pin kommt erst in Sub 7 P1
(`re15_re2z_player_pin()`, `:1299`). Die Opfer-FSM sieht ein Bild lang keinen Pin und wechselt in die
FREIGABE (Zustand 3, `enemy_ai_common.c:1673-1700`). Dort laeuft Clip 0 einmal durch — zufaellig
dasselbe Bild wie im Original — und am Ende gibt `enemy_ai_common.c:1896-1960` den Spieler frei:
`g_player_victim = 0`, `motion = 200`, `anim_frame = 0`. Zwei Bilder spaeter endet Clip 23 des
Hundes, und der Block aus 4.1 entscheidet: mit Taste frei und hp 0, ohne Taste
`re15_re2z_victim_devour` — und weil `anim_frame` inzwischen 0 ist, spielt Leon die 145 Bilder ein
zweites Mal, diesmal ohne Hund am Hals.

Der fuer den RE2-Hund vorgesehene Zweig ("Clip 0 EINMAL, P2 haelt die ENDPOSE",
`enemy_ai_common.c:1707-1714` und `:1748-1766`) wird im Latch also nie so durchlaufen, wie er gedacht
war.

### 4.3 Keine Regression von v0.8.13 -> v0.8.15

- `git blame -L 1240,1282 re15_port/engine/src/enemy_ai_re2_dog.c`: alle Zeilen d2055fc1c vom
  2026-08-16 (erster Port des RE2-Hundes). Erreichbar ist der Latch seit der Kiefer-Naehe vom
  2026-08-21 (`:529-553`).
- `git diff v0.8.13 HEAD -- .../enemy_ai_re2_dog.c`: vier Bloecke (@660 Landung, @2165/@2196 INIT,
  @2241 Tick) — keiner beruehrt `re2d_contact` oder `re2d_sub7_latch`.
- Gemessen: dieselbe Sonde gegen den Quellstand v0.8.13 (`git archive v0.8.13 re15_port/...` in ein
  Wegwerf-Verzeichnis, eigener Bau, kein Worktree noetig; danach geloescht).
  `build/r30_hund-tod/suche_hp20_v0813.log`:

| Stand | Laeufe | toedlich gebissen | mit Latch | auferstanden | Game Over |
|---|---|---|---|---|---|
| v0.8.13 | 32 | 26 | 20 | **10** (alle 10 Latch-Laeufe mit Taste) | 16 |
| v0.8.15 | 32 | 32 | 30 | **15** (alle 15 Latch-Laeufe mit Taste) | 17 |

Der Fehler ist in beiden Staenden derselbe. Neu ist nur, dass die Hunde seit v0.8.14/15 zuverlaessiger
treffen (32 statt 26 toedliche Laeufe, 30 statt 20 Latch) — der Nutzer erreicht den Kehlbiss jetzt
haeufiger.

### 4.4 N2 (Tod waehrend des Zielens) ist eine ZWEITE, unabhaengige Ursache

Gemessen mit R1 gehalten (`druck=3`, 16 Blickrichtungen + 3 Riegel-Laeufe): 101 Bilder aktive
Zielphase vor dem Tod, alle Laeufe enden im Latch (wer zielt, schaut den Hund an -> Blickkontakt 0
-> Latch), **ZIELPOSE_IM_TOD = 0** in 19 von 19 Laeufen. Im Kehlbiss ueberdeckt die Zielpose nichts:
`re15_re2z_victim_begin` ruft `re15_player_aim_interrupt()` (`enemy_ai_common.c:2607`), und der
Render-Override `platform/pc/main.c:7616` ist auf `re15_player_victim_state() == 0` gegated.

N2 sitzt im cmd-3-Pfad (Boden-Biss von hinten, Kraehe, Spinne, Gorilla, Birkin — alles, was ohne
Griff toetet): `re15_player_death_cmd3()` (`game_step_common.c:171-181`) ruft
`re15_player_aim_interrupt()` nicht, und der Render-Override hat kein Todes-Gate. Messung dazu liegt
vollstaendig vor und wird nicht wiederholt: `analysis/befunde_runde30/android-r1-toggle.md` M7 (Echtlauf
`RE15_KILL_AT=600`: zielend steht die Figur bei F630 und F710 aufrecht in der Messerpose) und M2
Fall I (Zielphase aktiv 40/40). Original: `sb 3,4(s1)` / `sb zero,5(s1)` / `sb zero,6(s1)`
@0x80012EF0-EFC ersetzt das Kommando-Register; RE2 dasselbe Muster @0x80105220-30.
Einen Hunde-Lauf mit Boden-Biss-Tod WAEHREND des Zielens hat die Sonde nicht erzeugt (0 von 19) —
fuer den Hund ist N2 deshalb NICHT GEMESSEN, nur hergeleitet.

### 4.5 ZENSUS — andere Gegner mit Griff/Biss

Sonde `re15_port/tests/unit/probe_r30_hund_tod_zensus.c`, EIN Lauf je Prozess
(`build/r30_hund-tod/zensus_einzeln.log`). Spieler mit hp 5 (Alligator 30, Kraehe 4) neben den Gegner,
echter Raum-Eintritt `scd_room_reenter`, echter `re15_game_step`, Flavor RE2, Bankwahl wie
`pc_enemy_load_ex`. Druck 0 = keine Taste, Druck 1 = Kreuz jedes zweite Bild.

| Gegner | Typ | Raum | Bank | Treffer | hp min | Griff | tot @ | Game Over @ (Druck 0 / 1) | auferstanden | Urteil |
|---|---|---|---|---|---|---|---|---|---|---|
| Hund | 0x20 | 11D0 | RE2 | 1 | -15 | 102 | 249 | 510 / **nie** | **Druck 1: Bild 251** | **FEHLER** |
| Zombie | 0x10 | 1010 | RE2 | 1 | -15 | 2 | 23 | 284 / 284 | nein | stirbt |
| Zombie (fressend) | 0x10 | 1140 | RE2 | 1 | -25 | 64 | 85 | 346 / 346 | nein | stirbt |
| Kraehe | 0x21 | 10C0 | RE2 | 2 | -5 | 87 | 392 | 653 / 653 | nein | stirbt |
| Spinne | 0x25 | 2090 | RE2 | 1 | -15 | - | 24 | 285 / 285 | nein | stirbt |
| Gorilla | 0x27 | 11C0 | RE1.5 | 1 | -7 | - | 651 | 912 / 912 | nein | stirbt |
| Alligator | 0x23 | 2090 | RE2 | 3 | -60 | 78 | 333 | 353 / 353 | nein | stirbt |
| Birkin G5 | 0x36 | 5090 | RE2 | 1 | -1 | - | 1649 | 1910 / 1910 | nein | stirbt |
| Gitterarme | 0x1A | 1210 | RE2 Art 0x2D | 0 | 5 | 156 (bis 327) | - | - | - | greift, schadet nie |
| **Birkin Form 1** | 0x30 | 3070 | RE1.5 | 1 | **+1** | 141 | - | **nie** | - | **nicht toetbar** |

Einordnung der beiden Ausreisser:

- **Gitterarme:** kein Schaden ist das Original. Das RE2-Modul EM2D ruft den Schadenseintritt nie
  (`r30_hundtod_jalscan.py 800401d4` auf `CDEMD0_EM2D_ai0/ai1.BIN`: 0 Aufrufe; zum Vergleich
  `build/r30_hund-tod/re2_schadensaufrufe.txt`: Zombie 1, Hund 1, Kraehe 1, Spinne 1, Alligator 1,
  G-Birkin EM30 4, EM36 3).
- **Birkin Form 1 in ROOM3070:** der RE1.5-Prototyp klemmt die hp an DREI Angriffsstellen auf 1
  (STAGE3.BIN, selbst disassembliert): `addiu v0,v0,-10` / `bgez v0,...` / `ori v0,zero,0x1` /
  `sh v0,-13586(at)` @0x801177F0-810, dasselbe @0x80117C78-94, und mit -5 @0x80118B1C-38. Der Port
  bildet das byte-true ab (`enemy_ai_common.c:11578-79`, `:11618-19`, `:11690-91`). Gemessen:
  hp 5 -> 1, danach kein weiterer Abzug. Das ist kein Port-Fehler, sondern ein unfertiger Boss der
  Beta — siehe Abschnitt 6, Frage F2. (Die Aufstellung im Zensus ist synthetisch: das Skript parkt
  den Boss auf (30000, 30000); die Sonde holt ihn neben den Spieler.)
- Gegenprobe Flavor RE1.5 (`R30_FLAVOR=re15`, `zensus_einzeln_re15.log`): Hund, Zombie, Kraehe
  sterben mit und ohne Taste. Der RE1.5-Hund ist also richtig portiert; nur die RE2-Fassung traegt
  den Fehler.

⛔ Messfalle: der Modus `alle` faehrt die Faelle im selben Prozess und traegt Zustand weiter. 2 von 18
Zeilen wichen ab (Gorilla starb 10 Bilder frueher, Birkin wurde gar nicht getroffen). Massgeblich
sind die Einzellaeufe.

### 4.6 Nebenbefund — die Einmal-Rettung wird je Raum erneuert

Port: `re15_re2z_rng_reset()` ruft `re15_re2z_onesave_reset()` (`enemy_ai_re2_zombie.c:339-340`), und
`scd_room_reenter` ruft `re15_re2z_rng_reset()` bei JEDEM Raumladen (`scd_room_setup.c:306`). RE2:
der Raumlader FUN_80049E48 fasst Spieler+0x154 nur mit `andi v0,v0,0xf9ff` an (@0x80049E94-A8), das
Bit 0x1000 bleibt stehen. Geloescht wird es @0x80040308 (3.1) und beim Figuren-Aufbau FUN_8003BAF0
(`sh zero,340(s2)` @0x8003BD24; einziger Aufrufer @0x80049FD8, nur wenn die Figur wechselt).
Im Port bekommt der Spieler damit in jedem Raum eine neue Rettung. Fuer den Befund des Nutzers
spielt das keine Rolle (20 -> 0 -> -20 laeuft an der Rettung vorbei), es macht den Spieler aber
insgesamt zaeher als im Original. Offen ist der Aufrufer von 0x800402F4 — siehe Abschnitt 7.

### 4.7 N2 selbst nachgemessen (Nachtrag zu 4.4)

Modus `n2` der Sonde (`build/r30_hund-tod/n2_vor_dem_bau.log`), Hunde abgeschaltet, R1 gehalten,
in Bild 90 faellt hp auf -1:

```
N2: Zielbilder vor dem Tod 90 von 90 | cmd3 ab Bild 90 | Bilder tot 200 | davon mit aktiver Zielphase 200
N2 ROT
```

cmd 3 wird also sofort scharf, aber die Zielphase bleibt in 200 von 200 Todesbildern aktiv, und
damit greift der Render-Override `main.c:7616`. Deckt sich mit M7/M2 Fall I des Android-Dossiers.

## 5. UMSETZUNGSPLAN fuer den Bau-Agenten

Reihenfolge einhalten. Jeder Schritt nennt Datei, Stelle und Beleg. Schritte 1 bis 4 beheben den
Befund des Nutzers; Schritt 5 ist N2; Schritt 6 der Riegel. Keine neue Verhaltens-Konstante ohne
Adresse — die Schritte 1 bis 4 ENTFERNEN Port-Erfindungen und fuehren genau eine neue Zuordnung ein
(Figur ungerade = `re15_char_variant()`).

### Schritt 1 — die Wiederbelebung entfernen (D1)

Datei `re15_port/engine/src/enemy_ai_re2_dog.c`, Funktion `re2d_sub7_latch`.

- Zeilen 1245-1260 (Kommentar "SPIELER-SCHICKSAL am Biss-Ende" bis `e->re2d_abort21c = 0;`)
  ersatzlos streichen. Am Clip-Ende von P1 schreibt das Original nur die Hunde-Seite: Clip-Wort
  0x1F0018 @0x80101F04-28, `sh 1,6(s2)` @0x80101F20, `jal 0x80015e7c` @0x80101F24, `jal 0x8002959c`
  @0x80101F38, `sb zero,543(s2)` @0x80101F44. Kein Schreibzugriff auf den Spieler.
- Zeilen 1322-1324 (`if (g_actors[RE15_ACTOR_SLOT_PLAYER].hp >= 0) re15_player_victim_throwoff();`)
  streichen. Das Original schreibt dort nur `sb 12,5(s1)` / `sh zero,6(s1)` @0x8010221C-20.
- Der Kommentarblock 1265-1275 ist zu berichtigen: Bit 0 von Spieler+0x8 ist die Figuren-Nummer
  (Beleg 3.3), 0x04000100 ist Todesbit + Finisher-Bit (Beleg 3.5).

### Schritt 2 — das "Mash-Fenster" auf die Figur legen

Datei `enemy_ai_re2_dog.c`, Zeilen 1276-1299.

- `int mash = re15_re2z_mash();` wird `int ungerade = re15_char_variant();`
  (`include/re15_gameflow.h:70`, 0 = Leon, 1 = Elza). Beleg: `lbu v0,8(s3)` / `andi v0,v0,0x1`
  @0x80102010-18 und @0x801020A4-AC; RE2 benutzt dasselbe Bit fuer den Stimmen-Satz der weiblichen
  Figur (FUN_8001B934). Das ist eine ZUORDNUNG RE2-Figur -> RE1.5-Figur und als solche zu
  kommentieren.
- Zeile 1278 (`if (mash) e->re2d_abort21c = 1;`) streichen. `+0x21C` ist im Original das
  Abdreh-Flag des Laufs (Set @0x80100DD0-D4, Verbrauch @0x80100E14-28, Port `:964`); Sub 7 fasst es
  nicht an.
- Zeilen 1280 und 1294: `mash` durch `ungerade` ersetzen, sonst unveraendert. Der Zaehler 12
  (@0x80101D70/7C) und die Saettigung bei 0 bleiben.

### Schritt 3 — der Spieler im Kehlbiss: ein Clip, Endpose, nie frei (D2)

Dateien `enemy_ai_common.c`, `include/re15_ai_flavor.h`, `enemy_ai_re2_dog.c`.

- Neue Funktion neben `re15_re2z_victim_devour` (`enemy_ai_common.c:2657`), Deklaration in
  `re15_ai_flavor.h` neben Zeile 254:

  ```c
  /* RE2-Hund, Spieler-Haken 0x80104ACC (Tabelle 0x800CE400[0x20], gesetzt @0x801004AC-B4;
   * Verteiler = Spieler-Routine 6 @0x800400D0): P0 Clip 0 (Clip-Wort 0x001F0000 @0x80104B18-1C),
   * P1 einmal durchspielen (Advance a3 = 128 @0x80104B54-68), P2 = nichts (@0x80104B04). */
  void re15_re2dog_victim_latch(re15_actor_t *hund, re15_actor_t *pl)
  {
      extern void re15_player_aim_interrupt(void);
      re15_player_aim_interrupt();      /* Routine 6 ersetzt den Kommando-Zustand @0x80101DBC-C4 */
      pl->motion = 0;                   /* Clip 0 */
      pl->anim_frame = 0;               /* re15_player_victim_devour setzt fuer den RE2-Hund
                                         * bewusst NICHT zurueck — hier ist es der Start */
      re15_player_victim_devour(hund);  /* Zustand 2: einmal spielen, Endpose halten, dann state 7 */
  }
  ```

- `enemy_ai_re2_dog.c:584`: `re15_re2z_victim_begin(e, pl, 0);` wird
  `re15_re2dog_victim_latch(e, pl);`. Der Anker-Aufruf davor (`re2d_grab_anchor`, `:583`) bleibt.
- Damit laeuft der schon vorhandene RE2-Hunde-Zweig des Kollaps (`enemy_ai_common.c:1748-1766`): Clip 0
  einmal, Endpose, Todes-Stoehnen auf Bild 0x3a, am Ende `state = 7`. Der Zustand 1 mit seiner
  Auto-Freigabe (`:1673-1700`) wird fuer den Hund nicht mehr betreten.
- Der Todes-Zeitpunkt bleibt, wo er heute im Lauf OHNE Taste liegt: `re15_player_is_dead()` haelt
  waehrend der Opfer-FSM still (`re15_damage.c:292-298`) und schaltet mit `state == 7` frei.
  Keine Aenderung dort.

### Schritt 4 — zwei Kleinigkeiten in `re2d_contact`

- `enemy_ai_re2_dog.c:560`: `&& pl->state != 15` streichen. Verglichen wird im Original die
  Figuren-Nummer (`lbu v1,8(s1)` @0x80104F10, `beq v1,15` @0x80104F18), nicht der Zustand; der Port
  kennt keine Figur 15. Kommentar entsprechend.
- `enemy_ai_re2_dog.c:597-600`: der Platzhalter `e->anim_flags |= 0;` wird der ausdrueckliche Aufruf
  `re15_player_death_cmd3();` (Deklaration `re15_damage.h`). Beleg: `bltz v0,0x80104fb4` @0x80104F90,
  `jal 0x80105204` @0x80104FB4, dort `sb 3,4(v1)` @0x80105220 und `sb zero,5/6/7(v1)` @0x80105224-30.
  Heute faengt das der Abfall-Erkenner in `game_step_common.c:1244-1246` ein Bild spaeter auf
  (gemessen tot@144, cmd3@145); der direkte Aufruf macht den Pfad unabhaengig davon.

### Schritt 5 — N2: Tod waehrend des Zielens

- `re15_port/engine/src/game_step_common.c:171-181`, `re15_player_death_cmd3()`: nach
  `s_death3_on = 1;` den Aufruf `re15_player_aim_interrupt();` einfuegen. Beleg: cmd 3 ersetzt das
  Kommando-Register `sb 3,4(s1)` / `sb zero,5(s1)` / `sb zero,6(s1)` @0x80012EF0-EFC, Verteilung
  @0x80031C8C ueber Tabelle 0x80073F90; derselbe Aufruf steht schon bei cmd 2
  (`game_step_common.c:329`) und cmd 5 (`enemy_ai_common.c:2607`).
- `re15_port/platform/pc/main.c:7616`: die Bedingung des Ziel-Overrides um
  `&& g_actors[RE15_ACTOR_SLOT_PLAYER].hp >= 0` erweitern (Guertel zum Hosentraeger; die Zielphase
  ist nach dem ersten Punkt ohnehin aus).

### Schritt 6 — Riegel

`re15_port/tests/unit/probes/r30_hund-tod.cmake`, am Ende anfuegen:

```cmake
add_test(NAME unit_r30_hund_tod COMMAND probe_r30_hund_tod riegel)
set_tests_properties(unit_r30_hund_tod PROPERTIES TIMEOUT 300 SKIP_RETURN_CODE 77)
add_test(NAME unit_r30_hund_tod_n2 COMMAND probe_r30_hund_tod n2)
set_tests_properties(unit_r30_hund_tod_n2 PROPERTIES TIMEOUT 120 SKIP_RETURN_CODE 77)
```

Der Riegel faehrt 3 Blickrichtungen x 4 Druck-Arten (keine Taste / Kreuz jedes zweite Bild / ein
Druck / R1 gehalten) mit hp 20 und verlangt je Lauf:

| Bedingung | heute | Soll |
|---|---|---|
| toedlicher Biss gefallen (hp_min < 0) | 12 / 12 | 12 / 12 |
| hp nie wieder >= 0 | 9 / 12 | 12 / 12 |
| hp am Ende < 0 | 9 / 12 | 12 / 12 |
| Todes-Praesentation gestartet | 12 / 12 | 12 / 12 |
| Game Over erreicht | 9 / 12 | 12 / 12 |
| 0 Bilder Steh-Pose (motion 200) bei hp < 0 | 0 / 12 | 12 / 12 |
| 0 Neustarts des Opfer-Clips | 3 / 12 | 12 / 12 |
| 0 Bilder Zielpose bei hp < 0 | 12 / 12 | 12 / 12 |
| Abdeckung: Laeufe mit Latch | 12 | mindestens 6, sonst ROT |

Heute: `RIEGEL ROT (30)`, exit 1 (`build/r30_hund-tod/riegel_vor_dem_bau.log`).

### Abnahme-Messung nach dem Bau

1. `probe_r30_hund_tod riegel` -> `RIEGEL GRUEN (0)`, Abdeckung >= 6 Latch-Laeufe.
2. `probe_r30_hund_tod n2` -> `N2 GRUEN`, Zielbilder vor dem Tod > 0.
3. `probe_r30_hund_tod suche 20 1500 -7878 -17384 3` -> `AUFERSTANDEN 0`, `Game Over 48` von 48.
4. `probe_r30_hund_tod lauf 1 20 900 -7878 -17384 0`: Verlauf hp 20 -> 0 -> -20, danach nie wieder
   >= 0; Opfer-FSM `vs` durchgehend 2 vom Latch bis `state 7`; `afr` steigt einmal 0 -> 144.
5. Zensus Einzellaeufe (`probe_r30_hund_tod_zensus fall <name> <druck>`): Zeile `hund` wird
   "stirbt, Game Over" fuer Druck 0 UND 1; alle anderen Zeilen bleiben, wie sie in 4.5 stehen.
6. gdb-Watch (`r30_hundtod_watch.gdb`): genau ZWEI Schreiber auf `g_actors[0].hp` nach dem Start,
   beide `re2z_player_damage`. `re2d_sub7_latch` taucht nicht mehr auf.
7. Volle Suite (`bash re15_port/tools/local_build.sh`), besonders `test_re2_dog_ai`,
   `test_re2_dog_grab_anchor`, `test_re2_dog_jaw_contact`, `test_dog_ai`. Tests, die das alte
   Freikaempfen des RE2-Hundes festschreiben, sind mit Verweis auf 3.3 umzustellen, nicht zu loeschen.
8. Sichtpruefung im Echtlauf nach Skill `re15-port-visual-verify` (Fenster-Handle, nicht
   Fenstertitel): ROOM11D0, hp auf 20 herunterbeissen lassen, Kehlbiss, dabei Tasten druecken —
   Leon geht zu Boden und bleibt liegen, YOU DIED folgt.

## 6. Risiken / offene Fragen

**F1 — Zeitpunkt des Game Over im Kehlbiss.** Der Plan laesst die Todes-Praesentation dort
starten, wo sie heute im Lauf ohne Taste startet: nach dem Ende des Opfer-Clips (Latch + 147
Bilder), Game Over 261 Bilder spaeter. RE2 setzt das Todesbit 12 Bilder nach P0 (@0x8010204C-60),
die Hauptschleife startet DIEDEMO (@0x800266C8-8002671C), und DIEDEMO wartet fuer den Hund 90 Bilder
(@0x80193500[3]) — aber nur, wenn zusaetzlich Bit 0x800 von 0x800CFB74 steht (@0x801902F8-304). Dessen
Bedeutung ist nicht belegt. Mit Wartezeit begaenne die Praesentation 12 + 90 = 102 Bilder nach P0,
ohne sie nach 12. Solange das offen ist, kommt keine der beiden Zahlen in den Code; der heutige
Zeitpunkt ist eine benannte Port-Zuordnung. Naechster Weg: ein RE2-Savestate aus laufendem Spiel,
0x800CFB74 lesen (Skill `re15-savestate-ghidra`); im Repo liegt keiner.

**F2 — Birkin Form 1 in ROOM3070 kann nicht toeten.** Original der Beta (4.5). Nach der Regel
Beta -> Retail waere RE2 das Ziel; das RE2-Modul EM30 ruft den Schadenseintritt an vier Stellen
(@0x80101704, @0x80101B9C, @0x8010214C, @0x80106268), deren Schadenswerte und Modi sind noch nicht
gelesen. Eigener Auftrag, nicht Teil dieses Baus.

**F3 — Einmal-Rettung je Raum (4.6).** Der Port erneuert sie bei jedem Raumladen, RE2 nicht. Umbau
erst, wenn der Aufrufer von 0x800402F4 feststeht (Abschnitt 7).

**R1 — Tests, die das Freikaempfen festschreiben.** `test_re2_dog_ai.c` und
`test_re2_dog_grab_anchor.c` koennten den Zweig `re2d_abort21c` oder den Zustand 1 der Opfer-FSM im
Hunde-Latch pruefen. Vor dem Bau `grep -n "abort21c\|victim_state() == 1\|throwoff"` ueber beide
Dateien.

**R2 — Start der Opfer-Animation 2 bis 3 Bilder zu frueh.** Der Port startet sie beim Kontakt im
Flug, das Original in Sub 7 P0 (`sw 6,-1028(at)` @0x80101DC4). Das ist eine bestehende, im Code
begruendete Abweichung (Anker-Sprung, `enemy_ai_re2_dog.c:562-583`) und bleibt unberuehrt.

**R3 — Messstand anderer Sonden.** `probe_r27_hund_biss.c` laedt die Bank VOR `re15_enemy_reset()`
und misst deshalb mit Cliplaenge 1. Seine Zahlen zu Biss-Haeufigkeit und Latch sind mit Vorsicht zu
lesen. Nicht Teil dieses Baus.

**R4 — Elza.** Mit Schritt 2 laeuft der Freigabe-Block fuer Elza ab dem ersten Bild von P1 (der
Hund faellt sofort), fuer Leon nach 12 Bildern. Das ist RE2-Verhalten fuer die ungerade Figur. Die
Sonde misst nur Leon (`g_gameflow.character = 0`).

## 7. Was NICHT belegt ist

| Aussage | Stand |
|---|---|
| Bedeutung und Setzer von Bit 0x800 in 0x800CFB74 | NICHT BELEGT. 0x800CFB74 ist Flag-Bank 0 der RE2-Skripte (Zeigertabelle @0x800A78C8[0]), Bit 0x800 = Index 20. Kein Setzer mit Direktwert in EXE und 25 Overlays; der Skript-Laeufer findet kein Set/Ck(0,20) (171 Bloecke brachen ab), die Roh-Suche 91 Treffer ohne Pruefung. |
| Ob DIEDEMO im Hunde-Kill 90 Bilder wartet | NICHT BELEGT (haengt an Bit 0x800). Die Zahl 90 selbst ist belegt (@0x80193500[3], Typ-Tabelle @0x801936C4[3] = 0x20). |
| Wer FUN_800402F4 (hp += a0, loescht den Rettungs-Riegel) aufruft | NICHT BELEGT. 0 `jal` im EXE, 0 in COMMON/BIN, kein Zeiger im EXE. |
| Gerenderte Pose in den 1 bis 2 Steh-Bildern | NICHT GEMESSEN. Gemessen ist der Zustandswert `motion == 200` (Leerlauf-Marke), nicht das Bild. |
| N2 im Hunde-Boden-Biss | NICHT GEMESSEN (0 von 19 Laeufen mit R1 endeten im Boden-Biss). N2 selbst ist gemessen (4.7). |
| Verhalten mit Elza als Spielfigur | NICHT GEMESSEN. |
| Spieler+0x227 (`lbu v0,15(s3)` @0x80104F00) | Im Port ohne Produzenten, bleibt 0; Bedeutung laut Kopfkommentar "Partner-Ziel", hier nicht nachgelesen. |
| Schadens-Skalierungen in FUN_800401D4 (x1,5 / x5 / x2 @0x800401E4-80040244) | Im Port bewusst nicht uebernommen; von diesem Thema nicht beruehrt. |

## Artefakte

| Was | Wo |
|---|---|
| Sonde Hund | `re15_port/tests/unit/probe_r30_hund_tod.c` (Modi `lauf`, `suche`, `riegel`, `n2`) |
| Sonde Zensus | `re15_port/tests/unit/probe_r30_hund_tod_zensus.c` (Modi `raum`, `fall`, `alle`) |
| Registrierung | `re15_port/tests/unit/probes/r30_hund-tod.cmake` |
| Werkzeuge | `analysis/befunde_runde30/hund-tod_tools/` — `r30_hundtod_flagscan.py`, `_bitscan.py`, `_absscan.py`, `_offscan.py`, `_jalscan.py`, `_re2_flag0.py`, `r30_hundtod_watch.gdb` |
| Messungen | `build/r30_hund-tod/` — `suche_hp20.log`, `suche_hp20_v0813.log`, `lauf_druck{0,1,2}_hp20_yaw0.log`, `watch_hp_druck1.log`, `riegel_vor_dem_bau.log`, `n2_vor_dem_bau.log`, `zensus_einzeln.log`, `zensus_einzeln_re15.log`, `zensus_alle.log`, `victim_pose.log`, `re2_ki.log`, `re2_schadensaufrufe.txt`, `re2_hund_hp_zugriffe.txt` |
| Bau | `re15_port/build_r30_hund-tod/` |
