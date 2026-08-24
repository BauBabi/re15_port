# Dossier — FINDING 1a: „Die Biss-Sounds des Zombies fehlen" (RE2-KI-Modus)

Nutzer-Report 2026-08-24 (RE2-KI): die **laufenden** Biss-Geräusche, wenn der RE2-Zombie Leon
packt und beisst — Steh-Griff UND Kriecher-Bein-Biss. (NICHT der Fress-Finisher, der läuft
getrennt.)

Status: **RE + Messung abgeschlossen. KEINE Code-Änderung in dieser Phase.**

---

## 0. Kurzfassung

| Frage | Antwort (belegt) |
|---|---|
| Feuert der Biss-SE des RE2-Zombies im Port heute? | **JA.** Gemessen im echten `re15_game_step`: Steh-Griff `ENEMSE 3` → RE1.5-snd1 `SE 3` auf Frame-Slot **16** des Biss-Clips 12 (2× pro Griff, 38 Frames Abstand). Kriecher: Slot **1** des Clips 15 (3× pro Griff, 30 Frames Abstand). Beides deckt sich byte-genau mit `@0x801028ec` + Parameter-Tabelle `@0x80100014`. |
| Kommt der Biss aus EDD-Flag-Frames? | **NEIN.** Weder RE1.5- noch RE2-Biss-Clips tragen SE-Flag-Bits (Daten-Dump §1.5/§1.6). Der Biss ist ein **expliziter** `jal 0x8005bd6c` im Grab-EXEC. Die v0.3.17-Flag-Frame-SEs betreffen nur Kriech-Lokomotion (Clip 5 f2/f20, gemessen: feuert) und die Fress-Clips 18/19 (Finisher). |
| Was fehlt dann wirklich? | **Leons Griff-/Kampf-Laut.** Das Original spielt in der RE2-**Steh**-Kampfmaschine P0 `Se(0x04000001 \| (rand&1)<<17)` = **CORE 0 oder CORE 2** (`@0x8010aafc`). Der Port **unterdrückt das explizit** (`enemy_ai_common.c:712-714`) mit der Begründung, die Steh-Maschine P0 spiele ihn nicht — **diese Begründung ist durch `@0x8010aae0-aafc` widerlegt.** Gemessen: im gesamten RE2-Steh-Griff **0 CORE-Aufrufe**; im RE1.5-Griff CORE 0. |
| Zweiter, kleinerer Befund | Auch im RE1.5-Zweig fehlt die **Münzwurf-Variante**: RE1.5 spielt fest CORE 0 (`@0x8010a3e4`), RE2 würfelt CORE 0/CORE 2. Unter dem Sound-Mandat (Entscheidungen RE2, Klang RE1.5) ist der WÜRFEL eine RE2-Entscheidung → gehört in den RE2-Zweig. |
| Nicht die Ursache (ausgeschlossen durch Messung) | Prioritäts-Gate (`SE 3` prio 3 gewinnt game-weit gegen die Schritt-SEs prio 1 — Stimmen-Simulation §2.5); leerer RDT-Record (ROOM1140/1030 snd1 `se3` = `00 00 43 14`, belegt); fehlende Hook-Registrierung im Bank-Lader. |

---

## 1. RE — wer spielt was, mit Adressen

### 1.1 Die beiden Se_on-Zwillinge

**RE1.5 `Se_on` = FUN_80045024** (PSX.EXE) — Argumentformat aus dem Todes-Stöhnen-Dossier
(`analysis/nutzer_batch_2026-08-24/todes-stoehnen.md` §1.1), hier nur die drei tragenden Zeilen:

```
80045028: srl   v1,a0,24          ; BANK   = a0>>24     (4 = resident CORE @0x801fbd00)
80045078: srl   v0,a0,16
8004507c: andi  s4,v0,0xff        ; RECORD = (a0>>16)&0xff
80045080: andi  a0,a0,0xff        ; positional-Flag
```

**RE2 `Se_on` = FUN_8005ba28** (re2leon/PSX.EXE) — *identisches* Argumentformat, aus dem
Decompilat `RE2_Quellcode_V2/FUN_8005ba28.c` (selbst gelesen):

```c
uVar10 = param_1 >> 0x18;                                 /* BANK   */
uVar5  = (uint)(char)(&DAT_800d4c48)[uVar10];             /* Bank-Handle; -1 -> return */
uVar18 = param_1 >> 0x10 & 0xff;                          /* RECORD */
pbVar9 = (byte *)((&DAT_800dbb78)[uVar10] + uVar18 * 4);  /* EDT-Record */
local_38 = (ushort)param_1 & 0xff;                        /* positional */
```

⇒ `0x04000001` = CORE 0, `0x04020001` = CORE 2, `0x04030001` = CORE 3, `0x02020001` =
Bank 2 (RDT-snd0) Record 2 — in **beiden** Spielen gleich zu lesen.

**RE2-Gegner-SE = FUN_8005bd6c** (`ENEMSE`, eigene Bank; Port-Hook `s_re2z_se_fn`).

### 1.2 RE1.5-Griff — die VOLLSTÄNDIGE SE-Inventur

Greifer-Maschine `FUN_80102548` (STAGE1.BIN), Opfer-Maschine `FUN_8010a2cc`
(Phasentabelle `@0x8010017c` = {`8010a308`, `8010a404`, `8010a460`, `8010a4a4`, `8010a4e8`,
`8010a5b8`}), Opfer-Dispatcher `@0x8010a28c` über `0x801201a8[player+0x5]`.

| Stelle | Adresse | SE |
|---|---|---|
| Greifer [0] Griff-Commit | `@0x8010268c` | `FUN_800453d0(4)` = snd1 SE 4 |
| Opfer P0 | `@0x8010a3e4`, a0 gebaut `@0x8010a3bc/0x8010a3e0` (`lui a0,0x400` / `ori a0,a0,0x1`) | `Se_on(0x04000001)` = **CORE 0** (Leons Griff-Laut) |
| Greifer **[3] BISS-SCHLEIFE** | `@0x801027dc`–`@0x801028c8` | **KEIN Se-Aufruf.** Die einzigen `jal` sind `0x80019700` (Blut, `@0x80102818`) und `0x80037024` (Mash-Abfrage, `@0x80102820`) |
| Greifer [4] Abwurf | `@0x80102920` **und** `@0x80102960` | `FUN_800453d0(7)` — **zweimal** |
| Opfer P4 Wundstempel | `@0x8010a580-5b0` (`jalr 0x801201b8[+0x5]`) | kein SE (nur `FUN_80037edc`) |

Roh-Beleg der stummen Biss-Schleife (Auszug `@0x801027dc`):

```
801027dc: addiu v0,v0,-5         ; player HP -= 5
801027e4: sh    v0,-13586(at)    ; player+0x9a
80102818: jal   0x80019700       ; Biss-Blut (a0 = 0x1500 @0x80102808)
80102820: jal   0x80037024       ; Mash-Abfrage
80102848..801028c8               ; +0x9c / +0x9e / Devour-Handoff — KEIN jal 0x800453d0
```

### 1.3 Der RE1.5-Frame-Flag-Dekoder (die einzige verbleibende Quelle) — und warum er hier stumm ist

`FUN_8001b38c` (PSX.EXE) liest das Wort des LAUFENDEN Clip-Frames und ruft für jedes gesetzte
Bit der oberen 10 den Raum-SE:

```
8001b3a4: lw   v0,360(v0)        ; entity+0x168 = Zeiger auf das aktuelle Frame-Wort
8001b3ac: lw   v0,0(v0)
8001b3b4: srl  s0,v0,22          ; SFX-Maske = Wort >> 22
8001b3c0: andi v0,s0,0x1
8001b3cc: jal  0x800453d0        ; a0 = Bit-Index -> Raum-SE (snd1)
8001b3d8: bne  v0,zero,0x8001b3c0
```

`FUN_800453d0` selbst (Bank 3 = RDT-**snd1**):

```
80045404: lw    v0,0(v0)         ; Flags der aktuellen Entity
8004540c: andi  v0,v0,0x2000
80045418: addiu s5,a0,12         ; Bit gesetzt -> se_id += 12  (ZWEITE Tabellenhälfte)
80045420: sltiu v0,v1,0x19       ; se_id >= 0x19 -> return
80045474: srl   s2,v0,4          ; tone  = byte2>>4
8004547c: addiu s0,s0,-16        ; VOICE = (byte3 & 0x1f) - 0x10
800454b0: andi  s1,a1,0xf        ; prio-Nibble = byte2 & 0xf
800454bc: jal   0x80045a18       ; Prio-Gate -> !=0: KOMPLETT VERWERFEN
```

Aufrufer von `FUN_8001b38c` in STAGE1.BIN (eigener jal-Scan, Suchwort `0x0C006CE3`): 8 Treffer,
u.a. `@0x80100634` (Wurzel der Zombie-Familie) und `@0x8010aad8` (Wurzel 0x13) — **jeden Tick,
unabhängig vom Zustand**. Der Dekoder läuft also auch während des Griffs.

### 1.4 …aber die RE1.5-Griff-Clips tragen KEINE SE-Bits (Daten-Messung)

`EMD/CDEMD0.EMS` → EM10, Aktions-Bank (Paar 2), Frame-Wort `>>22` je Frame
(Scratchpad `flagdump.exe`, Port-eigener EMD-Parser):

```
Clip  3 ( 14 f)  Griff-Einzug   : KEINE SE-Bits
Clip  4 ( 21 f)  BISS-SCHLEIFE  : KEINE SE-Bits      <== der gehaltene Biss
Clip  5 ( 30 f)  Abwurf         : KEINE SE-Bits
Clip 11 ( 55 f)                 : f14 SE1, f46 SE0
Clip 13 ( 55 f)                 : f30 SE0, f48 SE0
Clip 17 ( 60 f)                 : f42 SE1
LOCO[1..5]                      : je 2× SE1 (die Schritte)
```

⇒ **Der RE1.5-Griff ist im Original während des Haltens STUMM.** Das ist ein byte-belegtes
Fakt, kein Portfehler — und es ist die Messlatte, an der ein „Chomp"-Fix im RE1.5-Zweig
scheitern würde. (Der RE1.5-Fress-Chomp kommt separat aus einem expliziten
`re15_audio_room_se(3)` bei Frame 0x28, `enemy_ai_common.c:3806`.)

### 1.5 RE2-Griff — EXEC[3] `@0x801025EC` und der Biss-SE

Phasentabelle `@0x8010001C` (10 Zeiger, byte-gelesen):
`{801026c0, 801027d8, 80102814, 80102838, 80102968, 801029a4, 80102be8, 80102c30, 80102c60, 80102eb4}`

Varianten-Wahl `s5` (`@0x8010266c-90`):
```
8010266c: lhu   v0,270(s1)       ; +0x10E
80102670: lbu   v1,8(s1)         ; TYP
80102674: andi  s5,v0,0x1        ; Kriecher-Bit
8010267c: beq   v1,23 -> 0x80102690
80102688: bne   v1,17 -> 0x80102694
80102690: addiu s5,s5,2
```

Zwei 8-Byte-Parameterblöcke werden per LWL/LWR auf den Stack kopiert
(`@0x8010261c-60`), roh gelesen:

```
0x8010000c -> sp+16 : 11 11 14 14 11 11 14 14     (GRIFF-Clip, Index s5*2)
0x80100014 -> sp+64 : 16 20 01 05 16 1e 01 0a     (BISS-Frame, SCHADEN; Index s5*2)
                       ^^ ^^  ^^ ^^  ^^ ^^  ^^ ^^
   s5=0 aufrecht  : Griff-Clip 11, Biss-Frame 16, Schaden 20
   s5=1 Kriecher  : Griff-Clip 14, Biss-Frame  1, Schaden  5
   s5=2 Typ 0x11  : Griff-Clip 11, Biss-Frame 16, Schaden 30
   s5=3 Typ 0x11-Kriecher : Griff-Clip 14, Biss-Frame 1, Schaden 10
```

P0 `@0x801026c0` (Griff-Commit) — Einstiegs-Moan, cooldown-gegatet:
```
8010278c: lbu   v0,569(s1)       ; +0x239 Moan-Cooldown
80102794: bne   v0,zero,0x801027c4  ; != 0 -> KEIN SE (byte-true)
8010279c: jal   0x80015fe8       ; rand
801027ac: addiu a0,zero,11
801027b0: addiu a0,zero,10
801027b4: jal   0x8005bd6c       ; ENEMSE 10 oder 11
801027c0: sb    v0,569(s1)       ; +0x239 = 150
```

P2 `@0x80102814` setzt den Biss-Clip (`Griff-Clip + 1`) und das Kampf-Budget:
```
80102820: addiu v0,zero,3        ; -> Phase 3
80102828: addiu v0,zero,148
8010282c: sh    v0,344(s1)       ; +0x158 = 148
80102830: addiu v1,v1,1          ; Clip = sp[16+s5*2] + 1
80102834: sw    v1,332(s1)
```

**P3 `@0x80102838` — DER BISS. Das ist die gesuchte Stelle:**
```
80102844: jal   0x80015cb8       ; Platzierung
80102858: jal   0x8002959c       ; Advance (a3 = 256) — wrappt +0x14D am Clip-Ende
80102860: jal   0x8001598c       ; Mash
80102874: addiu a0,a0,-2
80102878: subu  a0,a0,v1         ; +0x158 -= 2 + 5*mash
8010288c: addiu v0,zero,4        ; (nur wenn negativ) -> Phase 4 auf BEIDEN Seiten
80102888: sll   v1,s5,1          ; << Delay-Slot: gilt in BEIDEN Zweigen
80102898: addiu v0,sp,64
8010289c: addu  s0,v0,v1         ; s0 = &sp[64 + s5*2]
801028a0: lbu   v1,333(s1)       ; +0x14D = laufender Clip-FRAME
801028a4: lbu   v0,0(s0)         ; Soll-Biss-Frame aus der Tabelle
801028ac: bne   v1,v0,0x80102ebc ; nur auf dem exakten Frame
801028b8: jal   0x80039514       ; Rumble (6, 250, 0)
801028c4: jal   0x8003947c       ; Rumble (10, 0)
801028d4: jal   0x80039514       ; Rumble (4, 250, 15)
801028e0: jal   0x8003947c       ; Rumble (4, 13)
801028e8: addiu a0,zero,3
801028ec: jal   0x8005bd6c       ; *** ENEMSE 3 = DER BISS-SE ***
801028f0: addu  a1,s1,zero
801028f4: lbu   a0,1(s0)         ; Schaden aus derselben Tabelle
801028f8: jal   0x800401d4       ; Spieler-Schaden
```

Weil `+0x14D` durch `FUN_8002959C` am Clip-Ende **wrappt**, feuert das Gleichheits-Tor
**genau einmal pro Clip-Zyklus** — die „laufenden" Biss-Geräusche.

Weitere `jal 0x8005bd6c` derselben EXEC (vollständig, eigener jal-Scan `0x0C016F5B`):
`@0x80102b58` ENEMSE 2 (Kopf-Abriss Kriecher, Frame 22), `@0x80102b44` ENEMSE 8 (Zweig B,
Frame 27), `@0x80102ddc` ENEMSE 12/13 (Sturz nach Abwurf).

### 1.6 Der RE2-Frame-Flag-Dekoder ist an dieser Stelle NICHT beteiligt

`0x801016c8` dekodiert Bit `0x08000000` + Nibble `Wort>>28` und ruft `0x8005bd6c`.
Eigener jal-Scan über die GANZE `EMOVL10_S0.BIN` (Suchwort `0x0C0405B2`): **exakt zwei**
Aufrufstellen — `@0x80101d34` (EXEC[1] WALK) und `@0x80102454` (EXEC[2] BUMP). **Keine im
Griff.**

Daten-Gegenprobe (RE2 `CDEMD0.EMS`, EM010, Aktions-Bank, Bit `0x08000000` + Nibble):
```
Clip 11 (  5 f) Griff-Einzug aufrecht : KEINE Flag-Frames
Clip 12 ( 38 f) BISS aufrecht         : KEINE Flag-Frames   <== ENEMSE 3 kommt explizit
Clip 14 ( 10 f) Griff-Einzug Kriecher : KEINE Flag-Frames
Clip 15 ( 30 f) BISS Kriecher         : KEINE Flag-Frames   <== dito
Clip  5 ( 50 f) KRIECHEN              : f2:nib1  f20:nib1   (die Kriech-Schleife)
Clip 18 ( 67 f) FRESSEN               : f23:nib3 f48:nib3   (Finisher-Chomp)
Clip 19 ( 88 f) FRESSEN               : f44:nib3 f67:nib3   (Finisher-Chomp)
LOCO Clip 0 (65 f)                    : f20:nib1 f62:nib0   (die zwei Schritte)
```

⇒ Die v0.3.17-Konstruktion „Fress-Chomp/Kriech-SE via RE2-EDD-Flag-Frames"
(`re2z_frame_flag_se`, `enemy_ai_re2_zombie.c:862`) betrifft **Kriech-Lokomotion und
Finisher**, nicht den Griff-Biss. Sie ist damit für FINDING 1a irrelevant — sie feuert aber
(gemessen, §2.3).

### 1.7 Die RE2-OPFER-Maschinen — hier liegt die eigentliche Lücke

Auswahltabelle (roh gelesen, `EMOVL10_S0.BIN` RAW @0x80100000), Index = Spieler+0x5 = Variante:

```
0x8010cf2c: 8010a9b8  8010a9b8      -> Varianten 0/1 = STEH-Kampfmaschine
0x8010cf34: 8010af58  8010af58      -> Varianten 2/3 = KRIECH-Kampfmaschine
0x8010cf3c: 8010b464  8010b464      -> Kollaps/Fress-Maschine
```

**STEH-Maschine `0x8010A9B8`** (Prolog `addiu sp,sp,-72` @0x8010a9b8), Phasentabelle
`@0x801001dc` = {`8010aa50`, `8010ab14`, `8010ab70`, `8010ab90`, `8010aca0`, `8010ad78`,
`8010aedc`}. Phase 0 `@0x8010aa50`:

```
8010aa64: sb    v0,6(s1)         ; Phase = 1
8010aa6c: sh    v0,344(s1)       ; +0x158 = 2048
8010aa7c: sw    v1,332(s1)       ; Clip-Wort (Rate 0xF)
8010aa88: jal   0x80015558       ; Snap auf den Greifer
8010aae0: jal   0x80015fe8       ; ** rand **
8010aae8: lui   a0,0x400
8010aaec: ori   a0,a0,0x1        ; a0 = 0x04000001
8010aaf0: andi  v0,v0,0x1
8010aaf4: sll   v0,v0,17         ; 0  oder  0x20000
8010aaf8: or    a0,v0,a0         ; -> 0x04000001  ODER  0x04020001
8010aafc: jal   0x8005ba28       ; ** CORE 0 oder CORE 2, positional **
8010ab00: addiu a1,s1,56         ; a1 = Spieler-Position
```

**KRIECH-Maschine `0x8010AF58`**, Phase 0, `@0x8010b060`:
```
8010b05c: ori   a0,a0,0x1        ; a0 = 0x04000001  (fest CORE 0, kein Würfel)
8010b060: jal   0x8005ba28
8010b064: addiu a1,s1,56
```

Vollständige `jal 0x8005ba28`-Inventur der `EMOVL10_S0.BIN` (eigener Scan): **nur vier** —
`@0x8010aafc` (Steh-P0, CORE 0/2), `@0x8010b060` (Kriech-P0, CORE 0), `@0x8010b448`
(Kollaps-Dispatcher, `0x02020001` = snd0 Record 2, Tor `(+0x14D & 0xF) == 1`),
`@0x8010b510` (Kollaps, CORE 3 = das Todes-Stöhnen des Nachbar-Dossiers).

⇒ **Während der RE2-Kampf-Phasen 1..6 gibt es keinen Spieler-SE.** Der einzige Spieler-Laut
des Griffs ist der Eintritts-Laut in P0 — und genau der fehlt im Port für den Steh-Griff.

---

## 2. MESSUNG — was der Port heute tut

Harnisch: Scratchpad `probe_bite_se.c` (kein Repo-Code), gebaut gegen
`libre15_engine.a` + `libre15_test_support.a`. **Echter `re15_game_step`**, echte RDTs
(ROOM1140 / ROOM1030), echte SCD-Spawns, echte Bänke (RE2 `shared_assets/RE2/CDEMD0.EMS`
bzw. RE1.5 `shared_assets/PSX/EMD/CDEMD0.EMS`).
Protokolliert werden **alle vier** Kanäle: `re15_audio_room_se` (snd1, Spion in
`tests/test_support.c:49`), `re15_audio_room_se_snd0`, `re15_audio_core_se` und der
**ENEMSE-Hook** (eigene Registrierung via `re15_re2z_audio_hook`, die den Plattform-Mapper
`pc_re2z_se_re15` aus `platform/pc/main.c:590` 1:1 nachbaut).

> ⚠️ Grenze dieser Messung, ausdrücklich benannt: der Harnisch registriert den Mapper
> **selbst**. Er beweist, dass die ENGINE die Id feuert und wie der Mapper sie übersetzt —
> er beweist **nicht**, dass `pc_enemy_load_ex` den Hook im echten Binary registriert
> hat. Siehe offene Frage O1.

Die bestehenden Repo-Sonden `probe_bite_fx` / `probe_re2z_devour` / `probe_dog_devour` fahren
teils nur `re15_enemy_ai_run_all` + `re15_player_victim_tick`; diese Sonde fährt den vollen
`game_step` (Nachprüfung des Auftrags: `probe_re2z_devour` nutzt bereits `game_step`,
`probe_re2z_crawl_attack` ebenfalls — die Aussage „sehen den echten Pfad nicht" trifft auf
diese beiden nicht zu).

### 2.1 Steh-Griff, RE2-Flavor, ROOM1140, EM10

```
RDT ROOM1140 snd1 EDT: 0:X 1:X 2:X 3:X 4:X 5:X 6:X 7:X 8:X 9:X 10:- 11:- 12:X ... 21:X 22:- 23:-
Bank EM010: action clips 11=5f  12=38f  13=58f  14=10f  15=30f  16=41f

f146  Z s1=3 s2=1 clip=11 af=0   | PL vs=1        <- Griff-Commit (EXEC[3] P0)
      cd239=7  grabclip=11 bitefr=16 bitedmg=20   <- Einstiegs-Moan byte-true unterdrueckt
f150  Z s1=3 s2=2 clip=11 af=4
f151  Z s1=3 s2=3 clip=12 af=0   t158=148         <- Biss-Clip
  *** SND1 se=3   f167   clip=12 af=16 slot=16    <- ENEMSE 3 -> SE 3   ** DER BISS **
```
Zweiter Lauf ohne Positionierungshilfe, 900 Frames, drei Griffe:
```
f64 Biss-Clip -> SND1 se=3 @f80 (slot16) und @f118 (slot16)   = 2 Bisse / Griff (38f Abstand)
f488 ... -> SND1 se=3 @f504, @f542
```
**CORE-Aufrufe im gesamten Steh-Griff: 0.**

### 2.2 Kriecher-Griff, RE2-Flavor, ROOM1030, EM16 (Griff-Eintritt byte-true erzwungen
über `re15_ai_set_state_word(z, 0x101)` + `PL+0x1D3 |= 0x80`, exakt wie DECIDE `@0x80102FA4-C0`)

```
f13   Z s1=1 s2=1 clip=14 af=0  | PL vs=1
  *** SND1 se=5   f13     (ENEMSE 11 -> SE 5, Einstiegs-Moan)
  *** CORE se=0   f13                       <- Kriech-Variante spielt CORE 0 (byte-true @0x8010b060)
f23   Z s1=1 s2=3 clip=15 af=0             <- Biss-Clip 15 (30f), Biss-Frame 1
  *** SND1 se=3   f24    slot=1            ** BISS **
  *** SND1 se=3   f54    slot=1            ** BISS **
  *** SND1 se=3   f84    slot=1            ** BISS **
f99   Z s1=1 s2=5 clip=16                  <- Abwurf
  *** SND1 se=9   f121                     (ENEMSE 2 -> SE 9, Kopf-Abriss)
```
Zusätzlich feuert die Kriech-Lokomotion durchgehend `SND1 se=2` alle 32/18 Frames
(`re2z_frame_flag_se(e,1,2)`, Clip 5 f2/f20) — die v0.3.17-Flag-Frame-SEs **leben**.

### 2.3 Gegenprobe RE1.5-Flavor, ROOM1140, EM10

```
f70   Z s1=4 s2=1 clip=3 af=0  | PL vs=1
  *** SND1 se=4   f70            (Griff-Commit-SE @0x8010268c)
  *** CORE se=0   f70            (Leons Griff-Laut @0x8010a3e4)
f84   Z s1=4 s2=3 clip=4 af=0    <- BISS-SCHLEIFE, 101 Ticks
      ... KEIN EINZIGER SE ...   <- byte-true (§1.2 + §1.4)
f186  Z s1=6 s2=1 clip=10        <- Devour
  *** SND1 se=4  + CORE se=1
  *** SND1 se=3   f226  af=40    (Fress-Chomp, enemy_ai_common.c:3806, Frame 0x28)
  *** CORE se=3   f241           (Todes-Stoehnen)
```

### 2.4 Stimmen-Maschine — der Biss überlebt das Prio-Gate

Nachbau von `FUN_800453d0` / `FUN_80045a18` / `FUN_800458d4` über die echten
ROOM1140-snd1-EDT-Records, mit **allen** Zombies des Raums aktiv:

```
ROOM1140 snd1:  se0 v4 p1 | se1 v4 p1/nib9 | se3 v4 p3 (vag3 6768B = 0.269s) |
                se4 v5 p3/nib11 | se5 v5 p1/nib9 | se6 v4 p2 | se7 v4 p4 | se9 v4 p4

[VOICE] se=1 Stimme=4 prio=1(nib 9) laufend=3 -> VERWORFEN      (Schritte fallen aus)
[VOICE] se=3 Stimme=4 prio=3(nib 3) laufend=1 -> gespielt       ** BISS gewinnt **
[VOICE] se=3 Stimme=4 prio=3(nib 3) laufend=3 -> gespielt       ** BISS gewinnt (Gleichstand, nib 3 < 8) **
```
⇒ Das Gate ist **nicht** die Ursache. Der Biss-SE hat die höchste Priorität, die im Griff auf
Stimme 4 vorkommt, und Gleichstand mit sich selbst passiert er (Nibble 3 < 8, `@0x80045a50-58`).

### 2.5 Build

`bash re15_port/tools/local_build.sh` → **226/228**. Die zwei roten
(`integration_relatch_pin`, `integration_save_counter_pin`) sind vorbestehend und ohne Bezug zu
diesem Befund; in dieser Phase wurde **keine Zeile Code geändert**.

---

## 3. Die Lücke, exakt lokalisiert

### L1 — Leons Griff-/Kampf-Laut fehlt im RE2-STEH-Griff (CONFIRMED)

Datei `re15_port/engine/src/enemy_ai_common.c`, `re15_player_victim_latch_ex()`, **Z. 710-715**:

```c
if (zombie->type != 0x21 && zombie->type != 0x20 &&
    (g_player_victim == 0 || g_player_victim == 3)) {
    int re2 = re15_ai_re2_for_type(zombie->type) && re15_re2z_owns_type(zombie->type);
    if (!re2 || g_player_victim_variant >= 2)      /* <<< der Riegel */
        re15_audio_core_se(0);
}
```
Der Kommentar darüber begründet den Riegel mit *„die RE2-Struggle-Maschinen spielen ihn NUR im
Kriech-Fall (0x8010AF58 P0 `jal 0x8005ba28` @0x8010b060 — die Steh-Maschine P0 @0x8010aa50
nicht)"*. **Widerlegt:** die Steh-Maschine spielt ihn sehr wohl, in derselben Phase 0, nur
**23 Instruktionen weiter unten** und mit einem Münzwurf davor —
`@0x8010aae0` (`jal 0x80015fe8`) / `@0x8010aae8-aaf8` (a0-Aufbau) / **`@0x8010aafc`**
(`jal 0x8005ba28`). Dieselbe Stelle steht in **`enemy_ai_common.c:2066-2070`** ein zweites Mal
(`re15_player_victim_begin_re2()`: `if (g_player_victim_variant >= 2 && …) re15_audio_core_se(0);`)
mit wörtlich derselben, widerlegten Begründung — beide Stellen sind zu fixen.

**Wirkung, gemessen:** im RE2-Steh-Griff spielt der Port **null** CORE-SEs. Der Griff besteht
akustisch nur noch aus 2× `SE 3` (0,269 s) über 2,5 Sekunden — für den Spieler klingt das wie
„da fehlt der Sound, wenn er mich packt und beisst". Im RE1.5-Modus ist derselbe Moment mit
`SE 4` + `CORE 0` belegt, im RE2-Kriech-Griff mit `SE 5` + `CORE 0` — daher fällt genau die
**aufrechte RE2-Variante** aus dem Rahmen.

### L2 — Der Münzwurf CORE 0 / CORE 2 ist nirgends abgebildet (CONFIRMED)

`@0x8010aae0-aaf8`: `rand & 1` → `<<17` → `0x04000001` **oder** `0x04020001`. Der Port kennt in
beiden Zweigen nur `re15_audio_core_se(0)`. CORE 2 ist der Knockback-Schrei
(`game_step_common.c:323`, `Se_on(0x04020001)` @0x80035f90/@0x80036004) — das Sample existiert
also bereits im Port. Unter dem Sound-Mandat gehört der WÜRFEL (RE2-Entscheidung) in den
RE2-Zweig, das SAMPLE bleibt RE1.5-CORE.

### L3 — Der Biss-SE selbst ist NICHT die Lücke (widerlegt)

`re2z_se(3)` steht in `enemy_ai_re2_zombie.c:1437` mit korrektem Frame-Tor
(`re2z_frame_slot(e) == e->re2z_bitefr`, Parameter aus `re2z_param_bite` = den byte-gelesenen
`@0x80100014`), der Mapper `pc_re2z_se_re15` (`platform/pc/main.c:590`, `case 3: m = 3`) leitet
ihn auf RE1.5-snd1 `SE 3` — und beides **feuert gemessen** (§2.1/§2.2), überlebt das Prio-Gate
(§2.4) und trifft in ROOM1140/ROOM1030 einen belegten Record
(`snd1[3] = 00 00 43 14` → prog 0 / tone 4 → VAG 6768 B ≈ 0,269 s, Stimme 4, Prio 3).

### L4 — `se_id += 12` bleibt unportiert (bekannt, hier neu quantifiziert)

`@0x80045404-18`. Der Port dokumentiert das in `audio_pc.c:793-796` als „für ROOM10D0/ROOM1140
nachweislich inaktiv (Sce_em_set pc[5] == 0)". Neu belegt: die zweite Tabellenhälfte ist
**dasselbe Sample auf einer anderen SPU-Stimme** —
ROOM1140 `se3 (v4,p3) ↔ se15 (v6,p3)`, `se4 (v5) ↔ se16 (v7)`, `se0 (v4) ↔ se12 (v6)`;
ROOM1030 byte-identisch aufgebaut. Das ist der Mechanismus, mit dem das Original verhindert,
dass ein zweiter Gegner dem ersten die Stimme klaut. Für ROOM1030 (Kriecher-Raum) ist der
`pc[5]`-Zensus **noch nicht gemacht** → O2.

---

## 4. Offene Fragen (bewusst NICHT geraten)

**O1 — Ist `pc_re2z_se_re15` im echten Binary registriert?**
`pc_enemy_load_ex` (`platform/pc/main.c:621`) hat als erste Zeile
`if (type == 0 || re15_enemy_find(type)) return;` — ist die Bank schon in der Registry, wird
der Hook **nicht** registriert. `re15_enemy_reset()` (Raumwechsel, `room_common.c:303`) leert
die Registry, der Normalfall ist also gedeckt. **Nicht gedeckt:** ein AI-Wechsel
RE1.5 → RE2 im OPTIONS-Menü **ohne** Raumwechsel — dann bleibt der zuletzt registrierte Hook
stehen (im RE1.5-Modus der rohe ENEMSE-Player aus `main.c:719`), und die Zombie-SEs klängen
nach RE2-Samples statt nach RE1.5. Zu prüfen mit `RE15_SE_DEBUG=1` am echten `re15_pc.exe`
(die Instrumentierung dafür existiert bereits, `audio_pc.c:694`).

**O2 — `Sce_em_set pc[5]`-Zensus für ROOM1030 und die übrigen Kriecher-/Zombie-Räume.**
Entscheidet, ob L4 dort eine echte Divergenz ist oder wie in ROOM1140/10D0 folgenlos bleibt.
Produzent des Bits: `@0x8004229c-b0` (`entity+0x0 = 0x2001`).

**O3 — Welches CORE-Record entspricht `0x04020001` im RE1.5-Klang?**
CORE 2 ist im Port als Knockback-Schrei belegt (`game_step_common.c:323`). Ob der als
Griff-Alternative klanglich passt, ist eine Präsentations-Entscheidung und keine RE-Frage —
sie gehört bewusst festgelegt, nicht geraten.

**O4 — Der Kollaps-Dispatcher-SE `Se(0x02020001)` @0x8010b448.**
Bank 2 = RDT-**snd0** Record 2, Tor `(+0x14D & 0xF) == 1` (also alle 16 Frames) und
`PL+0x10C > *(PL+0x198)+0x60 - 500`. Der Port hat den Pfad
(`re15_audio_room_se_snd0`), ruft ihn hier aber nicht. Gehört zum Finisher-Dossier des
Nachbar-Agenten; ROOM1140 snd0 Record 2 ist belegt (`00 00 07 12`, Stimme 2, Prio 7).

**O5 — Wie oft soll der Steh-Biss im RE2-Modus klingen?**
Gemessen 2× pro Griff (Budget `+0x158 = 148`, −2/Tick = 74 Ticks; Biss-Clip 38 Frames).
Das ist byte-abgeleitet und braucht keinen Fix — hier nur festgehalten, damit ein späterer
„zu wenig Biss-Sound"-Report nicht in eine Frame-Zahl nach Gefühl mündet.

---

## 5. Reproduktion

```bash
export PATH="/c/msys64/mingw64/bin:$PATH"
SP=<scratchpad>
cd re15_port/build
gcc -std=c11 '-DRE15_ASSET_PSX_DIR="C:/workspace/git/reAi_v2/re15_port/shared_assets/PSX"' \
    -DRE15_PLATFORM_PC=1 -I../include \
    -o $SP/probe_bite_se.exe $SP/probe_bite_se.c \
    engine/libre15_engine.a tests/libre15_test_support.a -lm
cd ..                                   # cwd = re15_port
$SP/probe_bite_se.exe 0 1 900           # Steh-Griff, RE2   -> SND1 se=3 auf slot 16, KEIN CORE
$SP/probe_bite_se.exe 0 0 900           # Steh-Griff, RE1.5 -> Biss-Schleife stumm, CORE 0 am Griff
FORCE_GRAB=100 $SP/probe_bite_se.exe 1 1 700   # Kriecher, RE2 -> SND1 se=3 auf slot 1 + CORE 0
KEEP_ALL=1 $SP/probe_bite_se.exe 0 1 400       # alle Zombies -> Stimmen-/Prio-Simulation
$SP/edtdump.exe 1140 1030               # snd0/snd1-EDT-Records mit Stimme/Prio/VAG-Laenge
$SP/flagdump.exe                        # Frame-Flag-Bits RE1.5 (>>22) und RE2 (0x08000000|>>28)
```

Disassembly-Belege:
```bash
S=.claude/skills/re15-psx-disasm/scripts/re15_disasm.py
R="--bin ../../../re2leon/COMMON/BIN/EMOVL10_S0.BIN"
python $S dis   0x8010a2cc 140          # RE1.5-Opfer-Maschine (CORE 0 @0x8010a3e4)
python $S dis   0x801027dc  90          # RE1.5-Biss-Schleife  (KEIN Se-Aufruf)
python $S dis   0x8001b38c  20          # Frame-Flag-Dekoder   (srl >>22)
python $S dis   0x800453d0  70          # Raum-SE + Prio-Gate + se_id+12
python $S $R dis   0x801025ec 100       # RE2-Grab-EXEC  (s5-Wahl, Parameter-Tabellen)
python $S $R dis   0x80102898  40       # RE2-BISS       (ENEMSE 3 @0x801028ec)
python $S $R dis   0x8010a9b8  90       # RE2-Steh-Kampfmaschine (CORE 0/2 @0x8010aafc)
python $S $R table 0x8010001c  10       # RE2-Grab-Phasentabelle
```
⚠️ Werkzeug-Falle: `re15_disasm.py read` liefert für die RE2-BINs **falsche** Adressen
(gegengeprüft an `0x8010cf20`); Datenworte der RE2-Overlays direkt mit
`struct.unpack_from('<I', data, addr - 0x80100000)` lesen.
