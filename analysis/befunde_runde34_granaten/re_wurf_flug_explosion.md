# Runde 34 / RE — Wurf, Flug, Abprall, Zuender, Explosion der Hand Grenade (RE1.5-Original, ESP-Row-VM)

Stand: 2026-09-29, RE-Phase abgeschlossen (Abschnitte 0-9 + Schluss), offene Punkte unter OFFEN. Werkzeuge: `re_wurf_werkzeug/`, Laufzeit-Ausgaben: `build/r34g_wurf/`.
Quellen: `info/Re1.5/PSX.EXE` (t_addr 0x80010000), `re15_disasm.py` (alle Zitate unten selbst disassembliert),
RE2: `re2_disasm.py`. Decompilate nur als Gegenprobe.

## Gliederung
0. Einordnung fertig/unfertig je Mechanismus
1. Routinen 30 / 29 / 31 (jede Instruktion, jeder Zweig) + Slot-Feld-Karte
2. ESP-Hauptschleife (Dispatch A/B, Integration, Bodenkontakt +0x2a, Wurfrichtung, Waende, Kontakt, Bildrate)
3. CORE00.ESP Effekt 4 sub 0x0D (beide Zeilen, Zeichnung im Flug)
4. Kind-Effekte 0x03195000 / 0x030B5400 / 0x030B5800
5. SEs (Abprall, Zuendung, Explosion) — Kodierung FUN_80045024, Bank
6. Mündungslicht-Latch 0x800b5358 — alle Leser (kein Laerm!)
7. Zeitlinie je Zielhoehe
8. Granate in der Hand nach dem Loslassen
9. Nachtraege/Korrekturen gegenueber Runde 30, RE2-Gegenprobe
KONSTANTEN FUER DEN BAU / PORT-ABGLEICH / OFFEN

---

## 0. Einordnung (fertig / unfertig)

| Mechanismus | RE1.5-Stand | Massgeblich |
|---|---|---|
| Spawn Effekt 0x040D1000 im Waffen-FSM (nur Id 9) | vorhanden, vollstaendig (@0x8003368c-0x800337a8) | RE1.5 |
| Wurf-Init Routine 30, Flug/Abprall Routine 29, Explosion Routine 31 | vorhanden, vollstaendig, keine Stub-Zweige | RE1.5 |
| Bodenkontakt | nur Ebene Welt-y = 0 (kein Raumkollisionsaufruf, keine Waende) — so ausgeliefert | RE1.5 (vollstaendig, aber schlicht) |
| Flaechenschaden FUN_80012d60(500, P, 2) | vorhanden | RE1.5 (Tiefe: Schadens-Dossier) |
| Explosions-Licht (Latch 0x800b5358) | vorhanden, Licht sitzt VOR DEM SPIELER, nicht an der Granate | RE1.5 |
| Abprallzahl "RNG" | RNG FUN_8001af20 wertet NUR das Aufrufer-Register a0 aus -> deterministisch aus Spielerstatus 0x800acaec | RE1.5 |
| RE2-Gegenstueck zu R29/R30/R31 | keines (Musterscan §9) -> kein Retail-Vorbild fuer die Handgranate | RE1.5 |
| Acid/Incendiary (0x0A/0x0B) | kein Spawn (Gate `ori v0,zero,0x9` / `bne` @0x80033688/8c), Entlade-Handler = reine Munitions-Stubs | UNFERTIG -> RE2 (anderes Dossier) |

## 1. Die drei Routinen (selbst disassembliert, PSX.EXE)

Routinentabelle @0x80071d40 (48 Eintraege, `re15_disasm.py table 0x80071d40 48`):
[26] -> 0x800180b0, [29] -> **0x80018320**, [30] -> **0x8001843c**, [31] -> **0x8001854c**.
Der "Slot"-Zeiger ist das Global **0x800b52c4** (`lui t0,0x800b / lw t0,21188(t0)`); der Disassembler
annotiert es als "attack_workstruct" — es ist der **aktuelle ESP-Slot** (FUN_80019e20 schreibt ihn
@0x80019e58 / @0x80019eb8 / @0x80019ecc / @0x8001a494 als Iterator ueber den Pool 0x800a73b8).

### 1.1 Routine 30 @0x8001843c — Wurf-Init (Routine A der Zeile 0)

```
8001843c addiu sp,sp,-24
80018440 lui v1,0x800b ; 80018444 lw v1,21188(v1)        v1 = Slot
80018448 ori v0,zero,0x17 ; 80018450 sb v0,110(v1)         +0x6e := 0x17   (Anim-Satz 23)
8001845c ori v0,zero,0x3  ; 80018460 sb v0,108(v1)         +0x6c := 3      (aktiv|sichtbar)
8001846c ori v0,zero,0x1d ; 80018470 sh v0,2(v1)           +0x02 := 29     (Routine B)
80018474 ori v0,zero,0x2a ; 80018478 sh zero,0(v1)         +0x00 := 0      (Routine A = noop)
8001847c sh v0,30(v1)                                      +0x1e := 42     (Zuender)
80018480 lui a0,0x800b ; 80018484 lhu a0,-13588(a0)        a0 = u16 @0x800acaec (Spieler +0x98)
8001848c andi v0,a0,0x8000 ; 80018490 beq v0,zero,0x800184b4
   HOCH: 80018494 ori v0,zero,0x17c / 80018498 sh v0,16(v1)   +0x10 := 380
         8001849c addiu v0,zero,-110 / 800184a0 sh v0,18(v1)  +0x12 := -110
         800184a4 ori v0,zero,0x15 / 800184a8 sh v0,20(v1)    +0x14 := 21
         800184ac j 0x800184d8 / 800184b0 addiu v0,zero,-2    (v0 = -2, weiter im RNG-Zweig!)
800184b4 andi v0,a0,0x4000 ; 800184b8 beq v0,zero,0x80018510
   MITTE: 800184bc ori v0,zero,0x118 / 800184c0 sh v0,16(v1)  +0x10 := 280
          800184c4 addiu v0,zero,-50 / 800184c8 sh v0,18(v1)  +0x12 := -50
          800184cc ori v0,zero,0x18 / 800184d0 sh v0,20(v1)   +0x14 := 24
          800184d4 addiu v0,zero,-1
   HOCH+MITTE gemeinsam:
          800184d8 jal 0x8001af20 / 800184dc sh v0,8(v1)      +0x08 := -2 (HOCH) / -1 (MITTE); RNG(a0 = acaec)
          800184e0 addu v1,v0,zero ; 800184ec bgez v1,.. ; 800184f0..80018500   v0 = rng % 4 (C-Rest)
          80018504 addiu v0,v0,7 ; 80018508 j 0x8001853c ; 8001850c sh v0,38(a0)   +0x26 := rng%4 + 7
80018510 andi v0,a0,0x2000 ; 80018514 beq v0,zero,0x8001853c
   TIEF:  80018518 ori v0,zero,0x50 / 8001851c sh v0,16(v1)   +0x10 := 80
          80018520 ori v0,zero,0x1 / 80018524 sh v0,20(v1)    +0x14 := 1
          80018528 addiu v0,zero,-1 / 8001852c sh v0,8(v1)    +0x08 := -1
          80018530 ori v0,zero,0x5 / 80018534 sh zero,18(v1)  +0x12 := 0
          80018538 sh v0,38(v1)                               +0x26 := 5
8001853c lw ra,16(sp) ; 80018540 addiu sp,sp,24 ; 80018544 jr ra
```

**Korrektur zum Runde-30-Dossier:** Der HOCH-Zweig springt nach `0x800184d8` (den RNG-Aufruf) — HOCH
bekommt also ebenfalls `+0x26 = rng%4+7`, nicht nur MITTE. Kein Bit gesetzt -> keine Geschwindigkeit,
kein Zaehler (kommt nicht vor: der FSM spawnt nur mit gesetztem Bit, §7).

**Das "RNG" ist hier deterministisch.** FUN_8001af20 (`dis 0x8001af20`):
```
8001af20 lui v0,0x800b ; 8001af24 addiu v0,v0,-14476      &0x800ac774
8001af28 lhu t1,0(v0)                                     (Zustand geladen, danach NIE benutzt)
8001af30 srl v1,a0,7 ; 8001af34 andi v1,v1,0xff           v1 = (a0>>7)&0xff
8001af38 addu a0,a0,v1 ; 8001af3c andi a0,a0,0xff         a0 = (a0+v1)&0xff
8001af40 sll v1,v1,8 ; 8001af44 or a0,a0,v1 ; 8001af48 sw a0,0(v0)   Zustand := a0|(v1<<8)
8001af4c andi v0,a0,0xff ; 8001af54 jr ra                 return (a0+v1)&0xff
```
Rohbytes `00 00 49 94` @0x8001af28 = `lhu t1`, `c2 19 04 00` @0x8001af30 = `srl v1,a0,7` (Decompilat
`RE_15_Quellcode_V2/FUN_8001af20.c` ebenso: `FUN_8001af20(uint param_1)`). Das Ergebnis haengt also nur
vom Register a0 des Aufrufers ab. In Routine 30 ist a0 = `lhu a0,-13588(a0)` @0x80018484 = das volle
u16 @0x800acaec, zwischen @0x80018484 und dem `jal` @0x800184d8 unveraendert (HOCH: @0x8001848c-b0,
MITTE: @0x800184b4-d4 schreiben nur v0). Die Zielhoehen-Schreiber erhalten die unteren 13 Bits
(`andi v0,v0,0x1fff` + `ori 0x8000/0x4000/0x2000`, z.B. @0x80033228-38 / @0x800332bc-c8). Untere Bits:
0x0002 = Gift (gesetzt `ori v0,v0,0x2` @0x80012eac im Spieler-Schadenszweig, geloescht `andi 0xfffd`
@0x8004af8c/@0x8004b010, Statusanzeige `andi v0,v0,0x2` @0x8004eda8). Daraus:

| acaec | a0 | (a0>>7)&0xff | Rueckgabe | +0x26 |
|---|---|---|---|---|
| 0x8000 (HOCH, gesund) | 0x8000 | 0x00 | 0x00 | **7** |
| 0x4000 (MITTE, gesund) | 0x4000 | 0x80 | 0x80 | **7** |
| 0x8002 (HOCH, vergiftet) | 0x8002 | 0x00 | 0x02 | **9** |
| 0x4002 (MITTE, vergiftet) | 0x4002 | 0x80 | 0x82 | **9** |
| 0x2000 / 0x2002 (TIEF) | — | — | — | **5** (fest, @0x80018530-38) |

Allgemein: `+0x26 = (((acaec & 0xff) + ((acaec >> 7) & 0xff)) & 3) + 7` fuer HOCH/MITTE.

### 1.2 Routine 29 @0x80018320 — Flug und Abprall (Routine B, laeuft je Bild im Hauptlauf)

```
80018320 lui t0,0x800b ; 80018324 lw t0,21188(t0)         t0 = Slot
80018330 lh t1,42(t0)                                     t1 = +0x2a = WELT-y (s16, dieses Bild berechnet)
80018338 blez t1,0x8001842c                               Welt-y <= 0 -> nichts (in der Luft)
80018340 lhu v0,38(t0) ; 80018348 bne v0,zero,0x80018388  Zaehler +0x26 != 0 -> Abprall
  --- Zaehler == 0: liegen bleiben ---
  80018350 lui a0,0x10a ; 80018354 ori a0,a0,0x1
  80018358 jal 0x80045024 ; 8001835c addiu a1,sp,16       SE 0x010A0001, a1 = sp+16 (NICHT beschrieben!)
  80018368 ori v0,zero,0x63 ; 8001836c sb v0,108(v1)      +0x6c := 0x63 (aktiv|sichtbar|Physik-Stopp|Bild-Stopp)
  80018378 ori v0,zero,0x1f ; 8001837c sh v0,0(v1)        +0x00 := 31 (Routine A = Explosion)
  80018380 j 0x8001842c ; 80018384 sh zero,2(v1)          +0x02 := 0 (Routine B aus)
  --- Zaehler != 0: Abprall ---
  80018388 lhu a3,16(t0) ; 8001838c ori v1,v1,0x5556      v1 = 0x55555556
  80018390..800183bc  a2 = (s16)vx * 0x55555556 >>32 - sign  = vx/3 (auf 0 gerundet)
  800183c8 subu a3,a3,a2 ; 800183cc sh a3,16(t0)          vx := vx - vx/3
  800183d0 addiu v1,v1,-1 ; 800183d4 sh v1,38(t0)         +0x26 -= 1
  800183c4 lw v0,56(t0) ; 800183dc subu v0,v0,t1 ; 800183e0 sw v0,56(t0)   xlat_y(+0x38) -= Welt-y (auf y=0 setzen)
  800183e4..800183f8  vy := -((s16)vy / 3)                (0x55555556-Idiom, auf 0 gerundet)
  800183fc..80018414  sp+16/20/24 := (s32)+0x28, +0x2a, +0x2c   (Welt-Punkt, VOR der Korrektur gelesen)
  80018418 lhu a0,38(t0) ; 80018420 sll a0,a0,8 ; 80018428 or a0,a0,v1   a0 = 0x010A0001 | (n<<8), n = Zaehler NACH -1
  80018424 jal 0x80045024                                 SE
8001842c lw ra,32(sp) ; 80018430 addiu sp,sp,40 ; 80018434 jr ra
```

* **vz wird beim Abprall NICHT gedaempft** (nur `+0x10` und `+0x12` werden geschrieben). Mit Euler=0
  ist lokal x die Wurfrichtung, lokal z die kleine Seitendrift (§2.3).
* Der Punkt fuer den Abprall-SE (@0x800183fc-0x80018414) wird mit `lh v0,42(t0)` NACH dem `sw` auf
  +0x38 gelesen — +0x2a selbst ist aber noch der alte Wert (die Weltlage wird erst im naechsten Bild neu
  gerechnet), also die Stelle des Eindringens.
* Beim Liegenbleiben ist a1 = sp+16 **nicht initialisiert** (der Pfad @0x80018350-58 schreibt sp+16..24
  nicht). FUN_80045024 legt a1 bei `(a0 & 0xff) != 0` in FUN_80045a64 (Lautstaerke nach Lage,
  @0x800451c0-cc). Die Lage dieses einen SEs ist im Original also Stapelmuell -> §5, OFFEN.

### 1.3 Routine 31 @0x8001854c — Zuender und Explosion (Routine A, laeuft je Bild in Schleife 1)

```
8001854c lui a1,0x800b ; 80018550 lw a1,21188(a1)          a1 = Slot
80018560 lhu v1,30(a1)                                     v1 = Zuender +0x1e
80018568 beq v1,zero,0x80018688                            == 0 -> Ende (unten)
8001856c ori v0,zero,0x7 ; 80018570 bne v1,v0,0x800185f4   != 7 -> weiter
  --- Zuender == 7: EXPLOSION ---
  80018574 ori v0,zero,0x1 ; 80018578 lui at,0x800b ; 8001857c sb v0,21336(at)   0x800b5358 := 1 (Licht-Latch, §6)
  80018580 ori v0,zero,0x61 ; 80018584 sb v0,108(a1)       +0x6c := 0x61 (aktiv|Physik-Stopp|Bild-Stopp, UNSICHTBAR)
  80018594 lh v0,40(v1) ; 8001859c sw v0,16(sp)            P.x = +0x28
  800185a0 lh v0,42(v1) ; 800185a8 addiu v0,v0,-500 ; 800185ac sw v0,20(sp)   P.y = +0x2a - 500 (500 UEBER dem Boden)
  800185b0 lh v0,44(v1) ; 800185bc sw v0,24(sp)            P.z = +0x2c
  80018598 ori a0,zero,0x1f4 ; 800185a4 addiu a1,sp,16 ; 800185b4 ori a2,zero,0x2
  800185b8 jal 0x80012d60                                  Treffer-Resolver(500, &P, 2)
  800185c0 lui a0,0x319 ; 800185c4 ori a0,a0,0x5000        a0 = 0x03195000
  800185d0 lui a2,0x8007 ; 800185d4 addiu a2,a2,11596      a2 = 0x80072d4c (Einheitsmatrix, T=0)
  800185d8 lh a1,46(v0)                                    a1 = +0x2e (Gier des Werfers)
  800185dc jal 0x800199d4 ; 800185e0 addiu a3,sp,16        Kind-Effekt 0x03195000 an P
  800185e4 lui a0,0x408 ; 800185e8 ori a0,a0,0x1
  800185ec jal 0x80045024 ; 800185f0 addiu a1,sp,16        SE 0x04080001 an P
800185f4 ... 80018600 lhu v1,30(a1) ; 80018604 ori v0,zero,0x2 ; 80018608 bne v1,v0,0x80018668
  --- Zuender == 2: Nachbrand ---
  80018610..80018638  P := (+0x28, +0x2a-500, +0x2c)
  80018640 jal 0x800199d4                                  Kind 0x03195000 (a0 @0x8001860c/14), a2 = 0x80072d4c (@0x80018620-34)
  80018648 lui a0,0x30b ; 8001864c ori a0,a0,0x5400
  80018660 jal 0x800199d4                                  Kind 0x030B5400, gleiche P/a1/a2
80018668..80018684  +0x1e -= 1  (j 0x800186d0 / sh v0,30(v1) im Verzoegerungsslot)
  --- Zuender == 0: Ende ---
  80018688..800186ac  P := (+0x28, +0x2a-500, +0x2c)
  80018698 lui a0,0x30b ; 800186a8 ori a0,a0,0x5800        a0 = 0x030B5800
  800186b0 sb zero,108(a1)                                 +0x6c := 0 -> Slot FREI
  800186c8 jal 0x800199d4                                  Kind 0x030B5800
800186d0 lw ra,36(sp) ; 800186d8 addiu sp,sp,40 ; 800186dc jr ra
```

* Bei Zuender 7 laeuft danach noch der Block @0x800185f4 (Pruefung auf 2, faellt durch) und der
  Abzug @0x80018668-84 -> Zuender 6. Die Pruefung auf 0 kommt erst im naechsten Aufruf.
* Der Explosionspunkt P liegt **500 Einheiten ueber der Liegestelle** (PSX-y waechst nach unten).
* Alle drei Kind-Spawns uebergeben die **Einheitsmatrix** 0x80072d4c als Anker (`read 0x80072d4c 16 --w 2`
  = `4096,0,0,0,4096,0,0,0,4096,0,0,0,...`) und P als Versatz -> Weltlage der Kinder = P.

### 1.4 Slot-Felder (0x84 Byte, Pool 0x800a73b8, 96 Plaetze) — was die Granate beruehrt

| Slot | Typ | Bedeutung | Schreiber / Leser (Adresse) | Port `re15_esp_fx_t` |
|---|---|---|---|---|
| +0x00 | u16 | Routine A | Zeile; R30 := 0 @0x80018478; R29 := 31 @0x8001837c | `row[0x00]` |
| +0x02 | u16 | Routine B | R30 := 29 @0x80018470; R29 := 0 @0x80018384 | `row[0x02]` |
| +0x08/0a/0c | s16 | Beschleunigung x/y/z | Zeile (0,10,0); R30 +0x08 := -2/-1/-1 | `accel_x/y/z` |
| +0x10/12/14 | s16 | Geschwindigkeit x/y/z (lokal) | R30 (Wurf), R29 (Abprall), Tick | `drift_x/y/z` |
| +0x18/1a/1c | s16 | Winkelgeschw. | Zeile (0,0,0) | `row[0x18..]` |
| +0x1e | u16 | **Zuender** | R30 := 42 @0x8001847c; R31 zaehlt | `row[0x1e]` |
| +0x20/22/24 | s16 | Euler | Zeile (0,0,0); Tick += Winkelgeschw. | `row[0x20..]` |
| +0x26 | u16 | **Abprall-Zaehler** | R30 @0x8001850c/@0x80018538; R29 -1 @0x800183d4 | `row[0x26]` |
| +0x28/2a/2c | s16 | **Weltlage** (je Bild neu) | Tick @0x8001a1fc-0x8001a2a4; R29/R31 lesen | fehlt (Port: `x/y/z` + `xlat_*` ohne Drehung) |
| +0x2e | s16 | Gier (Spawn a1 = `lh 0x800acabe`) | FUN_80019700 @0x800197e4 | `param` |
| +0x30/32 | u16 | TPAGE/CLUT | Spawner | `tpage/clut` |
| +0x34/38/3c | s32 | xlat (lokal, gedreht) | Tick; R29 +0x38 -= y @0x800183e0 | `xlat_x/y/z` |
| +0x40/44/48 | s32 | Spawn-Versatz (a3) | FUN_80019700 @0x800197e8-800 | fehlt |
| +0x4c..0x6b | MATRIX | Anker-Schnappschuss (a2) | FUN_80019700 @0x80019820-5c | fehlt (Port: `x/y/z`) |
| +0x6c | u8 | Flags | 3 / 0x63 / 0x61 / 0 | `flags` |
| +0x6d | u8 | Anim-Zeitgeber | Spawner = Satz0.byte2 @0x800198bc; Tick | `timer` |
| +0x6e | u8 | Anim-Satz | Spawner := 1 @0x800198a4; R30 := 0x17 | `frame` |
| +0x6f | u8 | Zeilen-Cursor | Spawner := 0 @0x800198f8 | `row_cursor` |
| +0x70/71 | u8 | Kategorie / sub | Spawner @0x800197d4/d8 | `effect_id/sub_index` |
| +0x72 | u16 | Skala (0x1000) | Spawner @0x800197dc | `scale16` |
| +0x74 | ptr | Anker-Zeiger | Spawner @0x80019808/1c | fehlt |
| +0x78/7c/80 | ptr | Anim-/Koord-/Zeilenbasis | Spawner | `bank/eff_idx/rows_base` |

## 2. ESP-Hauptschleife FUN_80019e20 (je Spielbild) — Reihenfolge, Integration, Weltlage

### 2.1 Aufruf und Bildrate
* Einziger Spielpfad-Aufruf: `jal 0x80019e20` @0x8001ce2c im Spielbild, NACH `jal 0x8001a50c` (Gegner,
  @0x8001ce04) und `jal 0x80031c44` (Spieler/Waffen-FSM, @0x8001ce0c). Zweiter Aufrufer @0x8004cd34 im Zweig
  0x8004cca4 = Tabelle @0x80074bf4[1] (Dispatch ueber u8 @0x800b25c0, `lbu v0,9664(v0)` / `jalr` @0x80046504-28);
  dort laufen die Effekte also ebenfalls weiter. Fuer den Wurf ohne Belang (der Spawn liegt im Spielbild).
* **Folge:** Der Spawn im Waffen-FSM (Bild N) liegt VOR dem Tick desselben Bilds -> Routine 30 laeuft
  noch im Spawn-Bild (Schleife 1), Routine 29 ab demselben Bild im Hauptlauf.
* Takt: je Spielbild genau ein Tick. Spielbild = `VSync(u8 @0x800b5456)` (`lbu a0,0(s0)` @0x8002147c,
  `jal 0x80061fc0` @0x80021480); alle Schreiber setzen 2 (`ori v0,zero,0x2` @0x8001d5e4, @0x8001669c,
  @0x80020d08, @0x8002130c, @0x8002661c, @0x80046710) -> 2 VBlanks = **30 Hz**. Kein Unterschritt.
* Pause-Gate: `lw v0,-13760(v0)` (0x800aca40) `& 0x10000000` -> ganzer Tick aus (@0x80019e28-40).

### 2.2 Ablauf je Bild
```
Schleife 1 @0x80019e64-0x80019ec4: fuer ALLE 96 Plaetze (0x800a73b8, +0x84):
     Flags&1 -> jalr Tabelle[+0x00] (Routine A)                    @0x80019e84-9c
Hauptlauf @0x80019ee0-0x8001a49c: fuer jeden Platz:
  (a) Flags&8 (Kind aus FUN_800199d4): Flags ^= 9 (-> 0x03), Routine A einmal   @0x80019ef4-f30
  (b) Flags&1 sonst weiter; Flags&4: Ankermatrix *(+0x74) -> +0x4c neu          @0x80019f58-fa4
  (c) Weltlage +0x28/2a/2c (Flags&0x80 == 0 fuer die Granate)                   @0x8001a118-a2a4
  (d) Routine B (+0x02)                                                        @0x8001a2b4-d4
  (e) Physik NUR wenn Flags&0x20 == 0                                           @0x8001a2e8-388
      euler(+0x20..) += winkelgeschw.(+0x18..); xlat(+0x34.. s32) += vel(+0x10.. s16);
      vel += acc(+0x08..)
  (f) Anim-Zeitgeber +0x6d / Satz +0x6e (Flags&0x40 friert den Satz ein)         @0x8001a38c-47c
```
Kein Zeilenvorschub im Tick; Zeilen wechseln nur, wenn eine Routine FUN_800174e4 ruft (29/30/31 tun
das nicht -> die Granate bleibt die ganze Zeit auf Zeile 0).

### 2.3 Weltlage und Wurfrichtung (Flags&0x80 == 0)
```
8001a16c lhu v0,32(a1) / 8001a174 sh v0,0(s0)          w.x = euler.x
8001a178 lhu v0,34(a1) / 8001a17c lhu v1,46(a1) / 8001a184 addu   w.y = euler.y + GIER(+0x2e)
8001a190 lhu v0,36(a1)                                  w.z = euler.z
8001a1a0 jal 0x80068098                                 RotMatrix(&w, m)    (Ghidra: RotMatrix)
8001a1b8..8001a1e4 jal 0x800661c0                       ApplyMatrix(m, xlat(lo16), r)
8001a1f4..8001a220 sh ... 40/42/44(a0)                  +0x28/2a/2c := r
8001a224..8001a248 jal 0x800661c0 (a0 = slot+0x4c)      ApplyMatrix(Anker-R, +0x40.. (lo16), r2)
8001a250..8001a2a4                                      +0x28 += r2.x + (+0x60); +0x2a += r2.y + (+0x64); +0x2c += r2.z + (+0x68)
```
**Welt = Anker.R·Versatz(+0x40) + Anker.T + RotMatrix(euler.x, euler.y+Gier, euler.z)·xlat** (16-Bit-Addition).
Fuer die Granate (Euler = Winkelgeschw. = 0 laut Zeile) ist das RotY(Gier). RotMatrix @0x80068098
(eigene Disassembly): m[0][2] = sin(ry) `sh t6,4(a1)` @0x8006816c, m[0][0] = cos(rz)·cos(ry) @0x8006820c
-> RotY = [[c,0,s],[0,1,0],[-s,0,c]], also lokal (vx,vy,vz) -> Welt (c·vx + s·vz, vy, −s·vx + c·vz).
* **Lokal +x = Blickrichtung.** Gegenprobe im selben Programm: der Licht-Latch (§6) setzt das Licht mit
  FUN_8004f008(Gier, (1200,·,0)) (= RotMatrix(0,Gier,0)+ApplyMatrix, @0x8004f008-70) VOR den Spieler, und
  der Geh-Schritt FUN_800245d8 bewegt ebenfalls (v,0,0) (Port actor_locomotion.c:328-344 zitiert
  @0x80024658/64). Die Wurfgeschwindigkeit vx = 380/280/80 ist also Vorwaertsgeschwindigkeit, vz = 21/24/1
  eine Seitendrift, die NIE gedaempft wird (R29 schreibt nur +0x10/+0x12).
* **Gier = Spielergier beim Spawn:** a1 = `lh a1,-13634(a1)` (0x800acabe = Spieler +0x6a) @0x800336cc /
  @0x8003372c / @0x80033788 -> FUN_80019700 `sh s2,46(t0)` @0x800197e4.
* **Anker = Knochen-11-Weltmatrix beim Spawn:** a2 = `lw a2,-13348(a2)` (0x800acbdc) + 0x7a4
  (@0x800336d0-f0); a3 = &{0,0x12c,0x320} (HOCH, @0x800336d8-e8) / {0,0,0x1f4} (MITTE, @0x80033738-44) /
  {0,0,0x12c} (TIEF, @0x80033794-a0). FUN_80019700 kopiert a3 nach +0x40.. (@0x800197e8-800) und die
  8 Worte der Matrix nach +0x4c.. (@0x80019820-5c). Flags 3 ohne Bit2 -> **kein Nachfuehren**: die Granate
  fliegt unabhaengig davon, was Leon danach tut.

### 2.4 Bodenkontakt, Waende, Treffer
* **Boden = Ebene Welt-y = 0**, sonst nichts: R29 testet `lh t1,42(t0)` / `blez t1` (@0x80018330-38).
  Weder R29 noch R30/R31 noch der Tick rufen die Raumkollision (FUN_8001c6e8) oder eine Bodenhoehe.
  -> In Raeumen/Bereichen mit Boden != 0 faellt die Granate bis y = 0 (bzw. prallt ueber tieferen
  Boeden in der Luft ab). **Keine Wandkollision**: bei den Wurfweiten aus §7 (bis ~18800 Einheiten)
  fliegt sie durch Waende.
* **Kein Kontakt mit Gegner/Spieler:** 29/30/31 lesen keine Aktorlage. Einziger Treffer ist der
  Flaechentest bei Zuender 7 (§1.3).
* Die Eindringtiefe wird beim Abprall per `xlat_y -= Welt-y` (@0x800183dc-e0) ausgeglichen, beim
  Liegenbleiben NICHT (@0x80018350-84 schreibt +0x38 nicht) -> die liegende Granate steckt um die letzte
  Eindringtiefe (gerechnet 9..20) im Boden; P der Explosion = (x, Tiefe−500, z).

### 2.5 Flaechenschaden-Test (Ausschnitt, Tiefe im Schadens-Dossier)
FUN_80012d60(a0 = 500, a1 = &P, a2 = 2): fuer jeden aktiven Gegner (0x800acc2c, Schritt 0x1f4) UND den
Spieler (0x800aca54) FUN_8002b5d0(Ziel, &P, 500):
* waagrecht: `andi s1,s7,0xffff` / `addu s0,v0,s1` (@0x8002b700-04) -> R = Ziel-Radius(+0x78→+6) + 500;
  Kasten |dx|,|dz| < R (@0x8002b708-4c), dann `jal 0x80065f60` SquareRoot0(dx²+dz²) < R (@0x8002b764-70).
* senkrecht: `lhu v0,8(s2)` / `addu a2,s1,v0` (@0x8002b778-80) -> |dy| < 500 + Ziel-Hoehe (@0x8002b794-a8).
* Mittelpunkt = Ziel +0x34/+0x38/+0x3c + Versatz aus Ziel+0x7c.
* Schaden `s16 @0x8006f418[2]` = **1000** (`read 0x8006f418 12 --w 2 --signed` = 10,20,**1000**,1000,…);
  Gegner-Reaktion +0x05 := `u8 @0x8006f430[2]` = **9**; HP < 0 -> +0x04 := 3 (Tod).
* **Spieler:** Hitbox-Zeiger `lw v1,16032(v1)` (0x80073ea0 = 0x80073e94) -> +0x78 @0x80031640-64;
  Satz @0x80073e94 = `00 00 06 fa 00 00 c2 01 fa 05 c2 01` -> Radius 450, Hoehe 1530. Leon stirbt also,
  wenn er waagrecht naeher als **950** an P steht und |P.y − Spieler-y| < 500+1530 = 2030 (HP -= 1000,
  nur wenn Treffer-Sperre +0x93 Bit0 frei; Tod bei HP < 0). Spieler-Versatz +0x7c = 0x800b2354
  (`addiu v1,v1,9044` @0x8003166c; Wert 0 im Datei-Abbild, einziger Verweis) -> Mitte = Spielerlage.

## 3. CORE00.ESP Effekt 4 sub 0x0D — die fliegende Granate

Werkzeug `re_wurf_werkzeug/esp_effekt.py` (Ausgabe `build/r34g_wurf/core00_eff4.txt`), Bytes per `xxd`:

| Was | Datei-Offset | Bytes | Bedeutung |
|---|---|---|---|
| Effekt-4-Kopf | 0x1728 | `24 00 1c 00 d1 7a 1f 00` | ca=36 Anim-Saetze, cb=28 Koordinaten, CLUT 0x7AD1, TPAGE 0x001F |
| Sub-Tabelle [5] (sub&7) | 0x18CA | `7c 00` | base = 0x18C0 + 0x7C·4 = **0x1AB0** |
| Stromkopf | 0x1AB0 | `01 00 00 00 02 00 00 00` | 1 Strom (1 Platz), 2 Zeilen |
| Zeile 0 | 0x1AB8 | `1e 00 00 00 00 10 00 10 00 00 0a 00 00 00 …` (Rest 0) | A=30, B=0, w=h=0x1000, acc=(0,**10**,0), vel=0, Winkel=0, Euler=0 |
| Zeile 1 | 0x1AE0 | 40 × `00` | wird NIE geladen (29/30/31 rufen FUN_800174e4 nicht) |

* **CLUT:** FUN_80019700 `lhu v0,4(t5)` + `sll s0,v1,6` ((sub>>3)·0x40) -> `sh v0,50(t0)` (@0x8001987c-88):
  0x7AD1 + 0x40 = **0x7B11 -> VRAM (272,492)**. In DATA/TEX.TIM liegt diese Zeile @Datei 0x334:
  `0000 b631 b1ef adce a98c a129 98e7 90a5 a107 9ce6 98c5 94a4 9083 8c62 8841 8420` (oliv-grau). Die
  Huelsen (sub < 8) nehmen dagegen (272,491) @0x2F4 `0000 637b 4af7 …` (orange-braun).
  Beleg-Bild: `build/r34g_wurf/flugzellen_x8.png` (oben 0x7B11 = Granate, unten 0x7AD1), erzeugt mit
  `re15_port/tools/tex_tim_effect_slice.py --word1 0x001f7b11` + `re_wurf_werkzeug/flugzellen_bild.py`.
* **TPAGE** 0x001F -> Seite (960,256), 4 bpp. Flags 3 (kein Bit4) -> **kein Halbtransparent**, deckendes Sprite.
* **Anim:** Spawner setzt +0x6e := 1 (`sb v0,110(t0)` @0x800198a4) und +0x6d := Satz0.Byte2 = 1
  (`lbu v1,10(t5)` / `sb v1,109(t0)` @0x8001989c/bc). R30 setzt noch im Spawn-Bild +0x6e := **23**.
  Tick im Spawn-Bild: +0x6d 1 -> 0 (kein Weiterschalten) -> gezeichnet wird Satz 23. Danach je Bild +1:

| Anim-Satz | Datei | Bytes | Zelle (u,v) Kante 16, Versatz (−8,−8) |
|---|---|---|---|
| 23 | 0x17E8 | `10 01 01 10` | Koord 16 = (0,112) |
| 24 | 0x17F0 | `11 01 01 10` | (16,112) |
| 25 | 0x17F8 | `12 01 01 10` | (32,112) |
| 26 | 0x1800 | `14 01 01 10` | (64,112) |
| 27 | 0x1808 | `15 01 01 10` | (80,112) |
| 28 | 0x1810 | `16 01 01 10` | (96,88) |
| 29 | 0x1818 | `17 01 01 10` | (96,104) |
| 30 | 0x1820 | `16 01 01 10` | (96,88) |
| 31 | 0x1828 | `15 01 01 10` | (80,112) |
| 32 | 0x1830 | `13 01 01 10` | (48,112) |
| 33 | 0x1838 | `12 01 01 10` | (32,112) |
| 34 | 0x1840 | `11 01 01 10` | (16,112) |
| 35 | 0x1848 | `17 01 ff 10` | Schleife -> Satz 0x17 = 23 |

  -> **12-Bilder-Taumelzyklus** (je 1 Bild), die Granate dreht sich im Flug ueber die Zellen. Koordinatensaetze
  @0x1890.. (`00 70 f8 f8` = u0 v112 dx−8 dy−8 …).
* **Zeichnen** (FUN_80053240/FUN_800534c4, Decompilat + Port-Disasm-Zitate in main.c pc_draw_effects):
  Sprite an der Weltlage +0x28.. per RTPS, keine Drehung (achsparallele Vierecke), Groesse aus
  Satz-Byte3 (16) · Skala +0x72 (0x1000) · Zeile +0x04/+0x06 (0x1000) / Tiefe; Zellversatz = s8 aus
  Koordinatensatz Byte2/3. Nur mit Flags Bit0 UND Bit1 (@0x800532fc-0c).
* Liegen (Flags 0x63): Bit6 friert den Satz ein (`andi v0,v0,0x40` @0x8001a3b0) -> die Granate liegt
  mit der zuletzt gezeigten Zelle still. Zuender 7 (Flags 0x61, Bit1 weg) -> **unsichtbar**.

## 4. Kind-Effekte der Explosion (FUN_800199d4)

FUN_800199d4 (@0x800199d4-0x80019ca0) ist bytegleich zu FUN_80019700 bis auf das Start-Flag
`ori v0,zero,0xa` (@0x80019a88) statt 3 -> Bit3: Der Hauptlauf desselben Bilds macht `Flags ^= 9`
(-> 0x03) und ruft Routine A einmal (@0x80019ef4-30). Da R31 in Schleife 1 laeuft, sind die Kinder
**im Explosionsbild schon initialisiert UND gezeichnet**, unabhaengig von ihrer Platznummer.
Argumente aller drei Aufrufe: a1 = Gier des Werfers (+0x2e), a2 = 0x80072d4c (Einheitsmatrix, T=0),
a3 = &P -> Weltlage der Kinder = P + RotY(Gier)·xlat.

Kodierung a0 = (Kategorie<<24)|(sub<<16)|Skala (Decode @0x800199e0-f0 wie FUN_80019700 @0x8001970c-30):

| a0 | Effekt | sub | Zeilen-Tabelle (sub&7) | CLUT (+ (sub>>3)·0x40) | Skala | Zuender |
|---|---|---|---|---|---|---|
| 0x03195000 | 3 (Wolkenblatt, TPAGE 0x001E -> (896,256) 4bpp) | 0x19 | [1] = `1a 00` @0x386 -> base 0x3EC | 0x7811+0xC0 = **0x78D1** (272,483) @TEX.TIM 0xF4 (`0000 ffff ebde d7de bf1c …`) | 0x5000 | 7 und 2 |
| 0x030B5400 | 3 | 0x0B | [3] = `50 00` @0x38A -> base 0x4C4 | 0x7811+0x40 = **0x7851** (272,481) @TEX.TIM 0x74 (`0000 9ce7 98c6 94a5 …`) | 0x5400 | 2 |
| 0x030B5800 | 3 | 0x0B | wie oben | 0x7851 | 0x5800 | 0 |

(Effekt-3-Kopf @0x8: `1d 00 a3 00 11 78 1e 00` = ca 29, cb 163, CLUT 0x7811, TPAGE 0x001E.)

Zeilen (je 1 Strom, 2 Zeilen):

| sub | Zeile | Datei | A | acc | +0x0e (Flags) | vel | +0x16 (TPAGE-OR) | +0x26 (Anim-Satz) |
|---|---|---|---|---|---|---|---|---|
| 0x19 | 0 | 0x3F4 | 10 | (0,0,0) | 0x13 | (0,0,0) | 0 | 10 |
| 0x19 | 1 | 0x41C | 0 | (0,0,0) | 0 | (0,0,0) | 0 | 0 |
| 0x0B | 0 | 0x4CC | 10 | (0,5,0) | 0x13 | (0,−130,0) | 0x40 | 8 |
| 0x0B | 1 | 0x4F4 | 0 | (0,5,0) | 0 | (0,**−135**,0) | 0 | 0 |

Routine 10 @0x800176b0 (eigene Disassembly):
```
800176c0 lbu v0,14(v1) ; 800176c8 sb v0,108(v1)      Flags := Zeile+0x0e (0x13 = aktiv|sichtbar|ABE)
800176d8 lhu v0,48(v1) ; 800176dc lhu a1,22(v1) ; 800176e4 or ; 800176ec sh v0,48(v1)   TPAGE |= Zeile+0x16
800176e0 lhu a0,30(v1) ; 800176e8 sll a0,a0,6 ; 800176f0..fc                       CLUT += Zeile+0x1e<<6 (hier 0)
800176f4 lbu a1,38(v1) ; 80017704 sb a1,110(v1)       Anim-Satz := Zeile+0x26
80017700 jal 0x800174e4                                Zeilenvorschub -> Zeile 1 (acc/vel neu!)
```
Folgen:
* **0x19 = Feuerball:** TPAGE 0x1E|0 -> ABR 0 (halb/halb) mit ABE, steht still (Zeile 1: vel=acc=0),
  Anim-Satz 10..22 (je Dauer 1, @0x60..0xC0), Satz 23 @0xC8 = `00 00 00 00` = Ende -> Platz frei.
  **13 Bilder sichtbar.** Beleg-Bild `build/r34g_wurf/explosion_wolken_x3.png` obere Zeile (weiss-gelber
  Kern waechst, wird orange-rot, zerfaellt).
* **0x0B = schwarzer Rauch:** TPAGE 0x1E|0x40 = 0x5E -> **ABR 2 = Hintergrund minus Vordergrund
  (abdunkelnd)**; nach dem Vorschub vel_y = −135, acc_y = +5 -> steigt (nach 14 Bildern ~1435 hoch);
  Anim-Satz 8..22 -> **15 Bilder sichtbar**. Beleg-Bild untere Zeile (dunkle Wolke).
* Anim-Start: Spawner-+0x6d = Effekt-3-Satz0.Byte2 = 1 (@0x10 `00 01 01 08`) -> im Spawnbild wird der
  von R10 gesetzte Satz gezeichnet, danach +1 je Bild (Tick @0x8001a3cc-47c).

**Was man bei der Explosion sieht/hoert (Zusammenfassung):**
| Bild (rel. Zuender 7) | Ereignis |
|---|---|
| 0 | Granate unsichtbar; Feuerball #1 (5,0×) an P = Liegestelle − 500 in y; SE CORE Satz 8; Punktlicht 1 Bild (§6); Flaechenschaden 1000 |
| +5 (Zuender 2) | Feuerball #2 (5,0×) + Rauch #1 (5,25×, subtraktiv, steigt) |
| +7 (Zuender 0) | Rauch #2 (5,5×); Granaten-Platz frei |
| — | **kein Bildschirmwackeln, kein Rumble**: R29/R30/R31 rufen nur 0x80045024, 0x8001af20, 0x80012d60, 0x800199d4 |

## 5. SEs — Kodierung FUN_80045024, Baenke, Saetze

FUN_80045024(a0, a1 = &Lage) (eigene Disassembly `build/r34g_wurf/dis_80045024.txt`):
```
80045028 srl v1,a0,24                         Byte3 = Bank (0..5)
8004505c..64 lb a1, 0x800b21ec[Bank]          VAB-Handle je Bank; -1 -> Ende (@0x8004506c)
80045078 srl v0,a0,16 ; 8004507c andi s4,v0,0xff     Byte2 = SATZ
80045080 andi a0,a0,0xff ; 8004509c sw a0,40(sp)     Byte0 = Lage-Flag (!=0 -> positional)
80045094 sltiu v0,v1,0x6                       Bank < 6, Sprungtabelle @0x80010e70
   Bank 1 -> 0x800450d0: sltiu v0,s4,0x21, a0 = 0x801fcd00  (ARMS-Tabelle der ausgeruesteten Waffe)
   Bank 4 -> 0x8004511c: sltiu v0,s4,0x21, a0 = 0x801fbd00  (CORE-Tabelle)
80045140 sll v0,s4,2 ; 80045144 addu a0,a0,v0  Satz = Tabelle + Satz*4
800451b8 lw t1,40(sp) ; 800451c0 beq t1,zero   Byte0 != 0 -> 800451c8 lw a0,32(sp) / 800451cc jal 0x80045a64 (Lage)
```
**Byte1 wird nirgends ausgelesen** (a0 wird nach @0x80045080 ueberschrieben). Das `n<<8` im Abprall-SE
(@0x80018418-28) ist also wirkungslos: jeder Abprall spielt denselben Satz.

| Aufruf | a0 | Bank | Satz | Lage | Datei-Satz | Ton |
|---|---|---|---|---|---|---|
| Abprall R29 @0x80018424 | 0x010A0001 plus (n<<8) | 1 = ARMS (W09) | 0x0A | ja, Punkt = Eindringstelle | ARMS09.EDH @0x28 `00 00 13 10` | Prog 0 Ton 1 -> **VAG 2** (4352 B), vol 80, Stimme 16 |
| Liegen R29 @0x80018358 | 0x010A0001 | 1 | 0x0A | ja, **a1 = sp+16 nicht beschrieben** | wie oben | wie oben |
| Explosion R31 @0x800185ec | 0x04080001 | 4 = CORE | 0x08 | ja, P | CORE00.EDH @0x20 `00 00 93 00` | Prog 0 Ton 9 -> **VAG 6** (5904 B), vol 110, Stimme 0 |
| (nur Vergleich) R26 @0x800180e0 | 0x020A0001 | 2 | 0x0A | ja | — | nicht Granate |

* ARMS09.EDH = ARMS0A.EDH = ARMS0B.EDH und die .VB ebenso (`cmp`, bytegleich). EDT-Werkzeug
  `re_wurf_werkzeug/edh_dump.py` (Ausgaben `build/r34g_wurf/arms09_edt.txt`, `core00_edt8.txt`).
  ARMS09 hat nur 2 VAGs: VAG 1 = 48 B (leer), VAG 2 = 4352 B. Satz 0x00 (`00 00 13 30`) nimmt denselben
  Ton 1 (+1 Lage auf nicht vorhandenem Ton 2).
* Bank 1 ist die ARMS-Bank der **jeweils ausgeruesteten** Waffe (Port-Kommentar audio_pc.c:198-205,
  FUN_80043d8c). Wechselt Leon zwischen Wurf und Liegen die Waffe, kommt Satz 0x0A der neuen Bank.
* Lage-Zweig FUN_80045a64 (@0x80045a64..): Abstand Kamera-Auge (Kamerasatz `lw v1,36(v1)` = [0x800ac778]+0x24,
  plus Cut*32, Felder +4/+8/+0xc) zum Punkt, `jal 0x80065f60` SquareRoot0, dann
  `lui v1,0x1062 / ori v1,v1,0x4dd3 / multu / mfhi / srl s4,v0,4` = x·0x10624dd3>>36 = **/250**, gekappt `slti v0,s4,128` -> 127
  (@0x80045b3c-5c) = Daempfung; Panorama ueber FUN_80045d6c. Nicht granatenspezifisch (alle positionalen SEs).

## 6. Latch 0x800b5358 — KEIN Laerm, sondern ein Ein-Bild-Muendungslicht

Vollscan `re_wurf_werkzeug/xref_global.py 0x800b5358` (EXE + STAGE1..6 + TITLE + DEBUG.BIN, lui/Offset-Paare,
Ausgabe `build/r34g_wurf/xref_800b5358.txt`):

| Stelle | Instruktion | Wer |
|---|---|---|
| 0x80017694 | `sb v0,21336(at)` | Routine 9 (Schuss-Knall, Muendung) |
| 0x800180f0 | `sb v0,21336(at)` | Routine 26 (SE 0x020A0001 + Vorschub) |
| 0x8001857c | `sb v0,21336(at)` | Routine 31, Zuender 7 (Granate) |
| **0x8001ce60** | `lbu v0,21336(v0)` | **einziger Leser** (Spielbild, direkt nach ESP-Tick) |
| 0x8001d170/74, 0x8001d1b4 | `addiu s0,s0,21336` / `lbu v0,0(s0)` / `sb zero,0(s0)` | Zuruecksetzen + Loeschen |

**Kein Overlay (STAGE1..6) und kein Gegner liest den Latch** -> Gegner hoeren die Granate darueber NICHT.

Leser @0x8001ce5c-0x8001d084 (L = Lichtsatz des angezeigten Cuts = `lw v0,44(v0)` auf [0x800ac778] +
`lh 0x800b0fe4`*40, das 40-Byte-Format aus re15_light.h:55-62):
```
8001ce84..a0 jal 0x8004ee38 (memcpy)  Sicherung: [0x800ac77c] := L (40 B)
8001ceb8 lh a0,-13634(a0) (Gier) ; 8001cebc ori v0,zero,0x4b0 ; 8001cec8 sh zero,48(at)
8001cecc jal 0x8004f008               v = RotMatrix(0,Gier,0)*(1200,.,0)
8001cef8 sb zero,3(v0)                L+0x03 := 0          (Typ Licht 2)
8001cf20..34 sltiu 0xd2 / sb 10(v1)   L+0x0A := max(L+0x0A, 0xD2)   (Licht-2-Farbe R)
8001cf5c..70 sltiu 0x8c / sb 11(v1)   L+0x0B := max(.., 0x8C)       (G)
8001cf98..ac sltiu 0x50 / sb 12(v1)   L+0x0C := max(.., 0x50)       (B)
8001cfd8..e4 lhu playerX + v.x -> sh 28(v0)   L+0x1C := Spieler-x + v.x
8001d010..1c lhu 0x800aca8c ; addiu -800 -> sh 30(v0)   L+0x1E := Spieler-y - 800
8001d04c..58 lhu playerZ + v.z -> sh 32(v0)   L+0x20 := Spieler-z + v.z
8001d080 ori v1,zero,0x1770 ; 8001d084 sh v1,38(v0)    L+0x26 := 6000   (Licht-2-Helligkeit)
... Aktoren/Szene werden gezeichnet ...
8001d174 lbu v0,0(s0) ; 8001d1ac jal 0x8004ee38   L := Sicherung ; 8001d1b4 sb zero,0(s0)   Latch := 0
```
* **Das Licht sitzt 1200 vor Leon und 800 ueber seinen Fuessen — auch bei der Granate, die Tausende
  Einheiten entfernt explodieren kann.** Es wirkt genau ein Bild (das Explosionsbild).
* Port: kein Verbraucher. game_step_common.c:1867 nennt den Latch "noise global ... consumer un-RE'd";
  re15_esp.c:646-650 (Routine 9) setzt ihn nicht einmal. Der Befund gilt fuer Schuss UND Granate.

## 7. Zeitlinie (Bild 0 = Spawn-Bild)

**Mechanik (belegt §1/§2):** Welt-y(k) = h + Summe_{j<k} vy(j), vy(j+1) = vy(j) + 10; R29 prueft Welt-y(k) > 0
VOR der Integration von Bild k. Abprall bei Zaehler n>0: vx -= vx/3, vy := -(vy/3), y auf 0; bei n = 0
liegen (Bild L). Zuender: R31 ab Bild L+1 je Bild -1 ab 42 -> **Explosion = L + 36**, Zuender 2 = L + 41,
Platz frei = L + 43. Abprall-SEs = Anfangszaehler (7/9/5) + 1 (das Liegen spielt denselben Satz).

Simulator `re_wurf_werkzeug/wurf_sim.py` (Ganzzahl, Reihenfolge wie §2.2, Ausgabe
`build/r34g_wurf/zeitlinie_tabelle.md` fuer h = -400..-2000). Die **Spawnhoehe h** haengt an der
Knochen-11-Matrix des Wurfbilds; statisch nicht ohne Posen-Rechnung ableitbar. Als Anhalt dient eine
**PORT-Messung** (vorhandene exe, `re_wurf_werkzeug/lauf_spawnhoehe.sh`, RE15_FX_LOG, ROOM1140,
Leon bei (-7600,0,-17600) rot_y 0; Port-Anker = Renderer-Knochen 11, 1 Bild alt):

| Zielhoehe | Clip / Spawnbild (FSM) | Port-Spawn (x,y,z) | h |
|---|---|---|---|
| HOCH | Clip 9 (`acae8 = 7+2*(acaec>>15)+((acaec>>11)&4)` @0x800334a8-c8), Bild 19 | (-7140, -3171, -16839) | -3171 |
| MITTE | Clip 7, Bild 22 | (-6851, -2474, -18279) | -2474 |
| TIEF | Clip 11, Bild 24 | (-6798, -772, -18246) | -772 |

Vorhersage mit diesen h (gesund: acaec-Unterbits 0; vergiftet: Bit 0x2 -> Zaehler 9):

| Fall | 1. Kontakt | Kontakte (SEs) | Liegen L | Explosion | Zuender 2 | frei | Weg lokal x / z bis L |
|---|---|---|---|---|---|---|---|
| HOCH gesund | 40 | 8 | 88 | **124** | 129 | 131 | 18760 / 1848 |
| MITTE gesund | 29 | 8 | 73 | **109** | 114 | 116 | 11929 / 1752 |
| TIEF gesund | 13 | 6 | 40 | **76** | 81 | 83 | 1543 / 40 |
| HOCH vergiftet | 40 | 10 | 94 | 130 | 135 | 137 | 18721 / 1974 |
| MITTE vergiftet | 29 | 10 | 79 | 115 | 120 | 122 | 11938 / 1896 |

Abprallbilder MITTE gesund: 29, 47, 55, 60, 64, 67, 70, Liegen 73 (Eindringtiefen 136, 90, 16, 25, 16, 3, 9;
Liegen bei Welt-y 9). Weitere Einzelspuren: `build/r34g_wurf/zeitlinie_port_h.txt`.
**Schadensbild (Zuender 7):** P = (x_L, Welt-y_L - 500, z_L); getroffen wird jeder Gegner mit
waagrechtem Abstand < r_Ziel + 500 und |dy| < 500 + h_Ziel (1000 Schaden, Reaktion 9), der Spieler bei
< 950 (§2.5). Die Weglaengen zeigen: HOCH/MITTE fliegen weit durch Waende (keine Wandkollision).

## 8. Die Granate in der Hand nach dem Loslassen — bleibt SICHTBAR

* Es gibt im gesamten Code genau drei Vergleiche der ausgeruesteten Waffen-Id (0x800aca5d) mit 9/10/11
  (`re_wurf_werkzeug/waffe9_gates.py`, Ausgabe `build/r34g_wurf/waffe9_gates.txt`):
  @0x80033368 `sltiu v0,v0,0x9` (Nachlade-Sperre), @0x80033688 `ori v0,zero,0x9` (Spawn-Gate),
  @0x80033e4c `ori v0,zero,0xa` (Nachladen, Vergleich mit Bild 10 der Waffe 7). Keiner schaltet ein Netz.
* Der FIRE-Substate @0x80033460-0x800337ac ruft fuer Id 9 nur: Entlade-Handler (`jalr` @0x800334e0 ->
  0x80033B38 -> `jal 0x8004eae4` @0x80033b40), anim_set `jal 0x8001f314` @0x80033668, Spawn
  `jal 0x80019700`. Keine Schreibzugriffe auf Teile-/Sichtbarkeitsfelder.
* Der Bank-Lader FUN_80036b68 (tauscht Teil 11 gegen das PLW-Netz) wird nur bei Raum-Init
  (`jal` @0x800316f0 in FUN_800314b0) und Waffenwechsel (@0x800466b0) gerufen, nie je Bild.
* FUN_8004eae4 (@0x8004eae4-68): nur `Zaehler -= 1` des ausgeruesteten Inventarplatzes (`sb v0,0(at)`
  @0x8004eb60); bei 0 Rueckgabe 0, **kein Entfernen, kein Ablegen**. Die letzte Granate hinterlaesst
  Leon also mit dem W09-Netz (Hand + Granate) in der Hand und Menge 0.
* Folge fuer das Bild: ab dem Spawnbild sind ZWEI Granaten zu sehen (Netz in der Hand + fliegendes
  Sprite), bis der Clip endet — so ist es im Original.

## 9. Nachtraege / Korrekturen gegenueber Runde 30

* **+0x2a ist kein "Bodenkontakt"-Feld**, sondern die je Bild neu gerechnete Welt-y (s16) des Platzes
  (Tick @0x8001a1fc-0x8001a29c). Kontakt = Welt-y > 0 (`blez` @0x80018338).
* HOCH hat ebenfalls einen RNG-Zaehler (`j 0x800184d8` @0x800184ac), und der "RNG" ist durch a0 = acaec
  deterministisch (7 gesund / 9 vergiftet).
* Der Latch 0x800b5358 ist ein Licht-, kein Laerm-Latch; kein Gegner liest ihn.
* Die Explosion trifft auch Leon (Spielerzweig in FUN_80012d60 ohne Selbstausschluss), 1000 Schaden.
* RE2 Retail: die Konstanten der Routinen 29/30 (`ori v0,zero,0x17c`, `ori v0,zero,0x118`,
  `addiu v0,zero,-110`) kommen in `info/re2leon/PSX.EXE` und allen `COMMON/BIN/*.BIN` NICHT vor
  (Werkzeug `re_wurf_werkzeug/re2_muster.py`, Ausgabe `build/r34g_wurf/re2_muster.txt`: in RE2 nur
  `lui a0,0x40d` @0x800218c8 = anderer Effekt 0x040D2800, `lui a0,0x10a` @0x800469e8, `lui a0,0x408` x4;
  0x17c/0x118/−110/`lui a0,0x319` in 0 von 22 RE2-Dateien). RE2 hat also kein
  Gegenstueck zur geworfenen Handgranate -> fuer Wurf/Flug/Explosion der Id 9 ist RE1.5 massgeblich
  (Beta->Retail-Regel greift nur fuer 0x0A/0x0B, anderes Dossier).

## KONSTANTEN FUER DEN BAU

| Name | Wert | Adresse | Instruktion(en)/Bytes | Verwendung |
|---|---|---|---|---|
| SPAWN_CODE | 0x040D1000 (Effekt 4, sub 0x0D, Skala 0x1000) | @0x800336bc-c0 (+@0x8003371c-20, @0x80033778-7c) | `lui a0,0x40d` / `ori a0,a0,0x1000` | Spawn im FIRE-Substate, nur Id 9 (`ori v0,zero,0x9` / `bne` @0x80033688-8c) |
| SPAWN_BILD_HOCH / MITTE / TIEF | 0x13 / 0x16 / 0x18 | @0x80033690 / @0x800336a4+@0x800336fc / @0x80033758 | `ori v0,zero,0x13` usw. gegen `lbu v1,-13591(v1)` (0x800acae9) | Wurfbild im Clip |
| ZIEL_BIT HOCH/MITTE/TIEF | 0x8000 / 0x4000 / 0x2000 | @0x800336b4 / @0x80033714 / @0x80033770 | `andi v0,v0,0x8000` usw. auf `lhu 0x800acaec` | Zielhoehe |
| CLIP je Zielhoehe | 9 / 7 / 11 | @0x800334a8-c8 | `srl v0,a0,15; sll v0,v0,1; addiu v0,v0,7; srl a0,a0,11; andi a0,a0,0x4; addu` | Wurf-Clip der W09-Bank |
| VERSATZ_HOCH | {0, 0x12c, 0x320} | @0x800336d8-e8 | `ori v0,zero,0x12c; sw v0,20(sp); ori v0,zero,0x320; sw zero,16(sp); sw v0,24(sp)` | Knochen-11-lokal |
| VERSATZ_MITTE | {0, 0, 0x1f4} | @0x80033738-44 | `ori v0,zero,0x1f4; sw zero,16(sp); sw zero,20(sp); sw v0,24(sp)` | " |
| VERSATZ_TIEF | {0, 0, 0x12c} | @0x80033794-a0 | `ori v0,zero,0x12c; …` | " |
| ANKER | [0x800acbdc] + 0x7a4 (Knochen 11), einmalig kopiert | @0x800336d0-f0, Kopie @0x80019820-5c | `lw a2,-13348(a2)`, `addiu a2,a2,1956` | Weltpunkt = R·Versatz + T |
| GIER | Spieler +0x6a (0x800acabe) -> Slot +0x2e | @0x800336cc, @0x800197e4 | `lh a1,-13634(a1)`, `sh s2,46(t0)` | Wurfrichtung (Port: `rot_y`) |
| R30_ANIMSATZ | 0x17 | @0x80018448-50 | `ori v0,zero,0x17; sb v0,110(v1)` | Flug-Anim ab Satz 23 |
| R30_FLAGS | 3 | @0x8001845c-60 | `ori v0,zero,0x3; sb v0,108(v1)` | aktiv+sichtbar |
| R30_ROUTINE_B | 29 | @0x8001846c-70 | `ori v0,zero,0x1d; sh v0,2(v1)` | Flug |
| ZUENDER_START | 42 | @0x80018474/7c | `ori v0,zero,0x2a; sh v0,30(v1)` | Zuender +0x1e |
| WURF_HOCH | v=(380,−110,21), acc_x=−2 | @0x80018494-b0 + @0x800184dc | `ori 0x17c; addiu -110; ori 0x15; addiu v0,zero,-2; sh v0,8(v1)` | Anfangsgeschw. lokal |
| WURF_MITTE | v=(280,−50,24), acc_x=−1 | @0x800184bc-dc | `ori 0x118; addiu -50; ori 0x18; addiu v0,zero,-1` | " |
| WURF_TIEF | v=(80,0,1), acc_x=−1, Zaehler 5 | @0x80018518-38 | `ori 0x50; ori 0x1; addiu -1; ori 0x5; sh zero,18(v1); sh v0,38(v1)` | " |
| ZAEHLER_HOCH_MITTE | ((a+((a>>7)&0xff))&0xff)%4 + 7, a = u16 0x800acaec | @0x80018484, @0x800184d8-0x8001850c, RNG @0x8001af30-4c | `lhu a0,-13588(a0)`; `jal 0x8001af20`; `addiu v0,v0,7; sh v0,38(a0)` | 7 gesund, 9 mit Gift-Bit 0x2 |
| GRAVITATION | acc_y = 10 | CORE00.ESP @0x1AC2 (Zeile @0x1AB8 +0x0a) | Bytes `0a 00` | Zeile 0 |
| BODEN | Welt-y > 0 | @0x80018330-38 | `lh t1,42(t0); blez t1,0x8001842c` | einzige Kollision |
| ABPRALL_X | vx −= trunc(vx/3) | @0x80018388-cc | `lui v1,0x5555; ori v1,v1,0x5556; mult; mfhi; subu a2,a2,a1; subu a3,a3,a2; sh a3,16(t0)` | vz bleibt! |
| ABPRALL_Y | vy := −trunc(vy/3); xlat_y −= Welt-y | @0x800183b4-f8, @0x800183c4-e0 | `mult v0,v1; mfhi; subu v0,v0,a0; subu v0,zero,v0; sh v0,18(t0)`; `lw v0,56(t0); subu v0,v0,t1; sw v0,56(t0)` | |
| SE_ABPRALL | Bank 1 Satz 0x0A (Byte1 = n wirkungslos), positional | @0x80018410-28 | `lui v1,0x10a; ori v1,v1,0x1; sll a0,a0,8; or a0,a0,v1; jal 0x80045024` | ARMS09.EDH @0x28 `00 00 13 10` -> VAG 2 |
| LIEGEN | SE 0x010A0001; Flags 0x63; A=31; B=0; keine y-Korrektur | @0x80018350-84 | `ori v0,zero,0x63; sb v0,108(v1)`; `ori v0,zero,0x1f; sh v0,0(v1)`; `sh zero,2(v1)` | |
| EXPLOSION_BEI | Zuender == 7 (= Liegen + 36 Bilder) | @0x8001856c-70 | `ori v0,zero,0x7; bne v1,v0,0x800185f4` | |
| LICHT_LATCH | 0x800b5358 := 1 | @0x80018574-7c | `ori v0,zero,0x1; sb v0,21336(at)` | 1 Bild Licht (§6) |
| FLAGS_EXPLOSION | 0x61 (unsichtbar) | @0x80018580-84 | `ori v0,zero,0x61; sb v0,108(a1)` | |
| EXPLOSIONSPUNKT | (x, Welt-y − 500, z) | @0x80018594-bc | `addiu v0,v0,-500` @0x800185a8 | |
| FLAECHENSCHADEN | FUN_80012d60(500, &P, 2) | @0x80018598/a4/b4/b8 | `ori a0,zero,0x1f4; addiu a1,sp,16; ori a2,zero,0x2; jal 0x80012d60` | Port: re15_resolve_attack(radius 500, type 2, attacker −1) |
| SCHADEN / REAKTION | 1000 / 9 | 0x8006f418[2], 0x8006f430[2] | read: `10,20,1000,…` / `3,3,9,…` | Port re15_damage_table / re15_react_table |
| SPIELER_HITBOX | r 450, h 1530, Versatz 0 | 0x80073e94 (Zeiger 0x80073ea0), 0x800b2354 | `00 00 06 fa 00 00 c2 01 fa 05 c2 01`; +0x78/+0x7c @0x80031640-74 | Leon stirbt < 950 |
| KIND_FEUERBALL | 0x03195000 (Effekt 3, sub 0x19, Skala 0x5000) | @0x800185c0-c4, @0x8001860c-14 | `lui a0,0x319; ori a0,a0,0x5000` | Zuender 7 und 2 |
| KIND_RAUCH | 0x030B5400 / 0x030B5800 | @0x80018648-4c / @0x80018698+a8 | `lui a0,0x30b; ori a0,a0,0x5400` / `…0x5800` | Zuender 2 / 0 |
| KIND_ANKER | Einheitsmatrix 0x80072d4c, Versatz P, Gier +0x2e | @0x800185d0-e0 | `lui a2,0x8007; addiu a2,a2,11596`; read `4096,0,0,0,4096,…` | |
| KIND_FLAGS | 0x0a (Init im Hauptlauf desselben Bilds) | @0x80019a88 | `ori v0,zero,0xa` | FUN_800199d4 |
| SE_EXPLOSION | Bank 4 Satz 8, positional | @0x800185e4-ec | `lui a0,0x408; ori a0,a0,0x1; jal 0x80045024` | CORE00.EDH @0x20 `00 00 93 00` -> VAG 6 |
| ZUENDER_NACHBRAND / FREI | 2 / 0 | @0x80018604-08, @0x80018568 | `ori v0,zero,0x2; bne`; `beq v1,zero,0x80018688`; `sb zero,108(a1)` @0x800186b0 | |
| CLUT_GRANATE | 0x7B11 = 0x7AD1 + (0x0D>>3)·0x40 -> VRAM (272,492) | @0x8001987c-88; TEX.TIM @0x334 | `lhu v0,4(t5); addu v0,v0,s0; sh v0,50(t0)`; `0000 b631 b1ef …` | Farbe der fliegenden Granate |
| FLUG_ANIM | Saetze 23..34 je 1 Bild, 35 = Schleife -> 23 | CORE00.ESP @0x17E8..0x1848 | `10 01 01 10` … `17 01 ff 10` | 12-Bilder-Taumeln |
| FEUERBALL_ZEILE | A=10, Flags 0x13, Anim 10, steht | CORE00.ESP @0x3F4 | `0a 00 00 00 00 10 00 10 … 13 00 … 0a 00` | 13 Bilder, CLUT 0x78D1, ABR0 |
| RAUCH_ZEILEN | A=10, Flags 0x13, TPAGE|=0x40, Anim 8; nach Vorschub vel_y −135, acc_y 5 | CORE00.ESP @0x4CC / @0x4F4 | `… 13 00 00 00 7e ff 00 00 40 00 … 08 00` / `… 79 ff …` | 15 Bilder, CLUT 0x7851, ABR2 subtraktiv |
| LICHT | Licht 2 des Cuts: Typ 0, Farbe max(0xD2,0x8C,0x50), Lage Spieler + RotY·(1200,·,0), y−800, Helligkeit 6000, 1 Bild | @0x8001cef8, @0x8001cf28/64/a0, @0x8001cebc, @0x8001d018, @0x8001d080 | `sb zero,3(v0)`; `sltiu v0,v0,0xd2` …; `ori v0,zero,0x4b0`; `addiu v1,v1,-800`; `ori v1,zero,0x1770` | Rueckkopie @0x8001d1ac, Loeschen @0x8001d1b4 |
| TAKT | 30 Hz, 1 ESP-Tick je Spielbild, Tick NACH Spieler-FSM | @0x8002147c/80, @0x8001ce0c < @0x8001ce2c | `lbu a0,0(s0)` (=2) / `jal 0x80061fc0` | |

## PORT-ABGLEICH

| Nr | Port heute (Datei:Zeile) | Original | Luecke |
|---|---|---|---|
| 1 | `game_step_common.c:1741-1809` Entlade-Tabelle `ENT[9] = {1,1,1,0}` -> `re15_player_weapon_fire(9)` beim Abzug (Kommentar Z.1772-1775 "PORT-BRUECKE") | Handler 0x80033B38 = nur `jal 0x8004eae4` | resolve fuer 9 auf 0; Schaden kommt erst aus R31 (Zuender 7) |
| 2 | `game_step_common.c:1942-1962` Spawn bei Bild 19/22/24 mit richtigen Versaetzen, aber `re15_esp_fx_spawn_rows(…, gp3[0..2], pl->y, 0)` | a1 = Spielergier -> +0x2e; kein Boden-Parameter | param = `pl->rot_y` (Port-rot_y == +0x6a, Walker actor_locomotion.c:339-344); `floor_y` ist fuer die Granate bedeutungslos (Boden = Welt-y 0) |
| 3 | `re15_esp.c:523-712` esp_fx_dispatch kennt 0,3,4,5,8,9,10,11,15,16,17,18,38 | R30/R31 als Routine A | Routinen 30 und 31 fehlen |
| 4 | `re15_esp.c:724-738` esp_fx_dispatch_b nur 12 | R29 als Routine B | Routine 29 fehlt |
| 5 | `re15_esp.c:903-923` Physik + **allgemeine Bodenklemme** (Z.916-922, "50% restitution" Z.918) fuer JEDEN phys-Partikel | Tick hat keine Klemme; Boden nur in Routine B (12/29) | Klemme fuer die Granate (und byte-true fuer alle) abschalten, R29 macht den Boden |
| 6 | Weltlage `f->y + f->xlat_y` (re15_esp.c:728/916) und Zeichnen `f->x + f->xlat_x` (main.c:296-298), ungedreht | Welt = Anker + RotMatrix(euler + (0,Gier,0))·xlat (@0x8001a118-2a4) | Drehung um Gier (+Euler) fehlt -> Granate flöge immer entlang Welt +x; R29 braucht Welt-y aus derselben Formel |
| 7 | `main.c:5413` `re15_esp_fx_tick` im SCD-Block VOR `re15_game_step` (main.c:6521, dort Spawn); beide im selben `while (running)`-Durchgang (main.c:4790, kein Schleifenende dazwischen) | ESP-Tick @0x8001ce2c NACH Spieler-FSM @0x8001ce0c | jede im game_step gespawnte Partikel tickt ein Bild zu spaet (R30 erst im Folgebild) -> Zeitlinie +1 |
| 8 | Kinder ueber `re15_esp_fx_spawn_rows` (Flags 3) | FUN_800199d4 Flags 0x0a, Init + Zeichnen im selben Bild (@0x80019ef4-30), Anker Einheitsmatrix + Versatz P, Gier des Elternplatzes | Kind-Spawn mit Bit3-Semantik + param fehlt; Routine 10 selbst ist im Port korrekt (re15_esp.c:652-665) |
| 9 | Globales Blatt Effekt 4 = `extracted_fx/effect4_shell.tim` (16 bpp, CLUT (272,491) eingebacken; main.c:3715-3719), Effekt 3 = `effect3_smoke.tim` (CLUT 480) | Granate CLUT 0x7B11 (272,492); Feuerball 0x78D1 (483); Rauch 0x7851 (481) | falsche Farben (Granate orange statt oliv, Feuerball grau). Weg: 4-bpp-Schnitte aus TEX.TIM per `tools/tex_tim_effect_slice.py --word1 0x001f7b11 / 0x001e78d1 / 0x001e7851` oder CLUT-abhaengige Blattwahl ueber `f->clut` |
| 10 | SEs: `re15_audio_weapon_se(idx)` = Bank 1 (audio_pc.c:1052), `re15_audio_core_se(idx)` = Bank 4 (audio_pc.c:976); in re15_esp.c nur `re15_esp_shell_clink_hook`/`re15_esp_bang_hook` | R29: Bank 1 Satz 0x0A (je Kontakt), R31: Bank 4 Satz 8 | Haken fuer ARMS 0x0A und CORE 8 fehlen; Lage-Zweig FUN_80045a64 (Abstand Kamera/250, max 127, Panorama) fehlt allgemein (audio_pc.c:670-690 OFFEN-Block an se_play_layers, :975, :1051) |
| 11 | Latch 0x800b5358: kein Verbraucher (`game_step_common.c:1867` "consumer un-RE'd"), Routine 9 (re15_esp.c:646-650) setzt ihn nicht | 1-Bild-Licht 2 (§6) | Licht-Aufblitzen fehlt fuer Schuss UND Granate; Lichtsatz-Format liegt in re15_light.h:55-62 vor |
| 12 | RNG `re15_engine_rand8` = xorshift (re15_damage.c:73-79; Kommentar Z.60-66 "leftover register a0") | R30: a0 = acaec gezielt geladen (@0x80018484) | Zaehler deterministisch aus Zielbit + `status_flags` (re15_actor.h:92, Bit 0x2 Gift) bilden, nicht aus dem Port-RNG |
| 13 | `re15_resolve_attack` (re15_damage.c:3294-3350) testet Spieler immer, Gegner mit Selbstausschluss per `attacker_slot` | FUN_80012d60(500,&P,2): kein Gegner ausgeschlossen (Anker = Spieler-Knochen) | aufrufen mit radius 500, type 2, attacker_slot −1; Tabellen re15_damage_table[2]=1000 / re15_react_table[2]=9 stimmen (re15_damage.c:41-58) |
| 14 | Hand-Netz W09 bleibt waehrend/nach dem Wurf sichtbar | ebenso (§8) | keine |

## OFFEN

1. **Lage des Liegen-SEs** (R29, @0x80018358): a1 = sp+16 ist in diesem Pfad nicht beschrieben
   (Stapelrest). Versucht: vollstaendige Disassembly von R29 (kein Schreiber auf sp+16..24 vor dem jal),
   Aufrufkette FUN_80019e20 (gleiche Stapeltiefe fuer alle Routine-B-Aufrufe). Was dort typischerweise
   steht, ist nur dynamisch messbar (PCSX-Redux-Haltepunkt @0x80018358, Skill re15-pcsx-watchpoint) —
   im Auslieferungsstand gibt es aber keine Granaten-Platzierung (Runde-30-Zensus), also keinen
   Spielstand mit Wurf. Fuer den Bau ist die Wahl "Lage = Granate" eine Port-Wahl und so zu kennzeichnen.
2. **Spawnhoehe h im Original**: nur als PORT-Messung (Renderer-Knochen 11) vorhanden (§7). Versucht:
   statisch (haengt an der Knochen-11-Weltmatrix = Pose Clip 9/7/11 Bild 19/22/24 + Skelett, also ein
   Pose-Ergebnis, keine Code-Konstante); dynamisch: Zensus `re_wurf_werkzeug/ss_granate_zensus.py` ueber
   alle 79 sauberen Savestates (boot_16.sav nicht dekodierbar, PATCHED-EXE_* ausgelassen; Ausgabe
   `build/r34g_wurf/ss_granate_zensus.txt`): **0** mit Waffe 9, Granate im Inventar oder Granaten-Platz
   (Waffen: 50x Id 1, 25x Id 0, 3x Id 3; Latch 0x800b5358 ueberall 0). Die Zeitlinie ist als Funktion von
   h vollstaendig (`wurf_sim.py tabelle`); die konkreten Bildnummern gelten fuer die gemessenen Port-h.
   Naechster Weg fuer eine Original-Messung: ROOM1140-Savestate per `re15_ss_patch.py` mit Id 9 im
   Inventar versehen, im Spiel ausruesten (virtuelles Pad, Skill re15-room-capture, damit die W09-Bank
   wirklich geladen wird), Wurf ausloesen, Pool-Platz 0x800a73b8+k*0x84 je Bild mitschreiben.
3. **Ueberschneidungen mit dem Schadens-Dossier**: Gegner-Reaktion 9 (+0x05) und die Treffer-Sperre
   +0x93 im Detail (Wirkung je Gegnertyp) — hier nur die Aufrufsemantik belegt (§2.5).
4. **Wurf in Raeumen mit Boden != 0**: Das Original faellt bis Welt-y 0 (belegt, §2.4, kein anderer
   Bodentest in 29/30/31/Tick). Wie das in RE1.5-Raeumen mit angehobenem Boden aussieht, ist nicht
   gemessen (kein Original-Spielstand mit Granate, s. Punkt 2).
5. **Acid/Incendiary (0x0A/0x0B)**: nicht Teil dieses Dossiers (RE1.5 ohne Spawn: nur Id 9 am Gate
   @0x8003368c) — RE2-Dossier der Parallelrunde.
