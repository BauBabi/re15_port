# Waffen-FSM: Die DISPATCH-MECHANIK (Tabellen + Xrefs)

Quelle: `ghidra1_V2.txt` (RE1.5 PSX.EXE, t_addr=0x80010000), Rohtabellen aus `info/Re1.5/PSX.EXE`
(Dateioffset = 0x800 + (va − 0x80010000)). Alle Aussagen tragen ihre Adresse. Stand 2026-09-12.

## 0. Gesamtkette (ein Frame)

```
main ──@0x80020c88──> FUN_80030444                        Pad lesen + logisch remappen
  (in-game step) ──@0x8001c9b8──> PTR_LAB_8001069c[phase] Spiel-Phasen-Tabelle (5 Eintraege)
    Eintrag[2] @0x800106a4 = LAB_8001cbb8                 normale Gameplay-Phase
      ... Subsystem-Kette @0x8001cdec-0x8001ce38 ...
      @0x8001ce0c: jal FUN_80031c44                       SPIELER-ROOT (1x pro Frame)
        @0x80031ca0-b4: Mode-Tab 0x80073f90[DAT_800aca58] (8 Eintraege)
          Mode 1 = LAB_80031de8                           Boden-Steuerung
            @0x80031eec: Phase-A-Tab 0x80073fb0[DAT_800aca59] (16 States)
            @0x80031f14: Phase-B-Tab 0x80073ff0[DAT_800aca59] (16 States)
              State 7 Phase B = LAB_80032e44              ZIELEN/FEUERN (R1)
                @0x80032e7c: Waffen-Tab 0x80074030[DAT_800aca5d] (20 Eintraege)
                  0x80032e9c Standard ──@0x80032ebc──> Substate-Tab 0x800740f4[DAT_800aca5a] (6)
                  0x80034014 Dauerfeuer ──@0x80034034──> Substate-Tab 0x80074150[DAT_800aca5a] (10)
                    Sub 2 beider FSMs ──> Schuss-Tab 0x80074100[DAT_800aca5d]
                      Standard:   EINMALIG pro Schuss (Latch aca5b)   @0x800334d8
                      Dauerfeuer: JEDEN Frame der Feuerschleife       @0x800345e4
```

Spieler-Entity ist ein statischer Block: `sw s0(=0x800aca54) -> DAT_800ac784` @0x80031c6c.
Damit sind die "Globals" Entity-Felder: +0x00=aca54 (Flags), +0x04=aca58 (Mode), +0x05=aca59
(State), +0x06=aca5a (Substate), +0x07=aca5b (Sub-Substate), +0x7c=acad0, +0x94=acae8 (Clip),
+0x95=acae9 (Bild im Clip). Mehrbyte-Stores schreiben mehrere Stufen atomar (s.u.).

---

## 1. FUN_80030444 — Pad lesen + logisches Remapping (fuellt DAT_800ac768/76c)

Aufrufer: `main` @0x80020c88 (1x/Frame), dazu 0x80029dd8/0x80029e14.

```
80030464  jal PadRead(0)                       ; PsyQ libetc
80030468  sw v0 -> DAT_800ac75c                ; (Zwischenlager)
80030478  sw v0 -> DAT_800ac758                ; roh HELD (libetc-Layout)
80030484  a0 = DAT_800ac768 (alt)              ; voriger logischer HELD
80030490  DAT_800ac768 = 0
80030494  lhu DAT_800b0fcc                     ; Key-Konfig-Index (0..3)
8003049c  v1 = 0x80073e1c                      ; Zeiger-Tabelle auf Keymaps
800304ac  DAT_800ac770 = alter logischer HELD
; Schleife i=15..0 @0x800304b4-e8:
;   mask = keymap[i] (lhu);  if (raw_held & mask) DAT_800ac768 |= (1<<i)
800304f4  if (DAT_800aca40 & 0x01000000)       ; Input-Lock
8003051c    DAT_800ac768 &= 0xf000
; Edge-Bildung @0x80030548-57c:
80030548  raw_edge  = (alt_raw ^ neu_raw) & neu_raw   -> DAT_800ac75c
80030550  log_edge  = (alt_log ^ neu_log) & neu_log
8003057c  sw -> DAT_800ac76c                   ; logischer EDGE (neu gedrueckt)
80030564  sh raw_held_lo16  -> DAT_800ac760
800305a0  sh raw_edge_lo16  -> DAT_800ac762    ; roher EDGE (fuer L1/START-Abfragen)
80030588  sh raw_held_hi16  -> DAT_800ac764
80030594  sh raw_edge_hi16  -> DAT_800ac766
```

**Keymap-Zeiger-Tabelle @0x80073e1c** (PSX.EXE-Dump): `[0]=0x80073dbc (Typ A), [1]=0x80073ddc
(Typ B), [2]=0x80073dfc (Typ C), [3]=0x800b21cc (RAM/Custom)`. Auswahl per `DAT_800b0fcc`.

**Rohbit-Layout = PsyQ libetc** (`info/.../PSYQ_SDK/psyq/include/LIBETC.H:14-35`): 0x0001=L2,
0x0002=R2, 0x0004=L1, 0x0008=R1, 0x0010=Dreieck, 0x0020=Kreis, 0x0040=Kreuz, 0x0080=Quadrat,
0x0100=SELECT, 0x0800=START, 0x1000=UP, 0x2000=RIGHT, 0x4000=DOWN, 0x8000=LEFT.

**Logische Bits (Keymap Typ A @0x80073dbc, Bytes aus PSX.EXE):**

| log. Bit | Roh-Maske | Taste (Typ A) | Bedeutung im Spieler-FSM |
|---|---|---|---|
| 0x0001 | 0x1000 | UP    | vorwaerts |
| 0x0002 | 0x2000 | RIGHT | drehen rechts |
| 0x0004 | 0x4000 | DOWN  | rueckwaerts |
| 0x0008 | 0x8000 | LEFT  | drehen links |
| 0x0010 | 0x1000 | UP    | Ziel HOCH (im Anschlag) |
| 0x0020 | 0x4000 | DOWN  | Ziel TIEF (im Anschlag) |
| 0x0040 | 0x0080 | **SQUARE** | **ABZUG (Level!)** |
| 0x0080 | 0x0080 | SQUARE | Aktion/Tuer (Edge-Abfrage) |
| 0x0100 | 0x0008 | **R1** | **ZIELEN (Level!)** |
| 0x0200 | 0x0040 | CROSS | Rennen |
| 0x0400 | 0x0008 | R1 | (Duplikat) |
| 0x0800 | 0x0004 | L1 | (siehe Roh-Edge unten) |
| 0x1000 | 0x8000 | LEFT | (Menue) |
| 0x2000 | 0x2000 | RIGHT | (Menue) |
| 0x4000 | 0x0080 | SQUARE | (Menue-Confirm, vgl. Inventar @0x80073dbc) |
| 0x8000 | 0x0040 | CROSS | (Menue-Cancel) |

Typ B (0x80073ddc): identisch, nur Dreh-Duplikate auf RIGHT. Typ C (0x80073dfc): Abzug-Bit
0x0040 <- roh 0x0020 = **CIRCLE**, Zielen-Bit 0x0100 <- roh 0x0002 = **R2**.

**ANTWORT Pad-Frage:** "Abzug gehalten" = logisches Bit **0x40 in DAT_800ac768 (HELD-Wort,
LEVEL-getriggert)**; Default-Belegung SQUARE. Edge (DAT_800ac76c & 0x40) wird NUR fuer den
Nachlade-/Leerklick-Zweig benutzt. "Zielen gehalten" = Bit 0x100 (R1), ebenfalls LEVEL.

---

## 2. FUN_80031c44 — Spieler-Root (liest Mode-Tabelle 0x80073f90)

Aufrufer: 0x8001ce0c (Gameplay-Phase LAB_8001cbb8, 1x/Frame).

```
80031c44  addiu sp,-0x18
80031c4c  lhu v1 = DAT_800acaee
80031c54  lw a0 = DAT_800aca40
80031c5c  v0 = 0x800ac784 ; s0 = v0+0x2d0 = 0x800aca54  ; Spieler-Block
80031c6c  sw s0 -> DAT_800ac784                          ; Entity-Zeiger := 0x800aca54
80031c74  sh DAT_800acaee -> DAT_800b0fec
80031c78  bltz a0 -> LAB_80031da8                        ; DAT_800aca40 < 0: Steuerung AUS
80031c84  lhu DAT_800aca52 ; & 0xfffe -> zurueck         ; Bit0 ("hat gefeuert") jeden Frame loeschen @0x80031c9c
80031c8c  lbu v1 = DAT_800aca58                          ; MODE
80031ca0  at = 0x80073f90 + mode*4
80031cac  lw v0 = [at]  ->  jalr @0x80031cb4             ; MODE-DISPATCH
80031cbc  jal FUN_8002b544
80031cd8  sw zero -> DAT_800ac788 ; aca3c &= ~0x4000
80031ce8  jal FUN_8002d100(player, 0x12)                 ; Beruehrungs-/Kontaktsuche
80031cfc    if (ret && ret[0x82]+1 == DAT_800acad6):     ; Gegner gefunden
80031d20      DAT_800ac788 = ret ; aca3c |= 0x4000
80031d38  jal FUN_8002dc48(player)                       ; Kollision/Bewegung (schreibt bei Block
                                                         ;  aca59=0xd, aca5a=0/1 @0x8002dd1c/24!)
80031d4c  if !(aca3c & 0x4000):
80031d58    jal FUN_8002b498(player)
80031d70    jal FUN_8003b0a4(&aca88, [acacc]+6, 1)
80031d78  jal FUN_80037358
80031d8c  if (DAT_800aca5c & 4): jal FUN_80024c30(player)
80031da4  sh zero -> DAT_800acadc
80031da8  if !(aca54 & 0x400): jal FUN_8001b064(&DAT_800acb04, DAT_800acc0e)
```

**Mode-Tabelle @0x80073f90 (8 Eintraege, PSX.EXE-Dump):**

| Idx | Handler | Rolle (belegt) |
|---|---|---|
| 0 | 0x800318f8 | Init: setzt sofort **Wort aca58=0x00000001** @0x8003192c (Mode 1, State/Sub/SubSub=0), acae8=0 @0x80031924, acae9=0 @0x80031954, acc0d=8, acc27/acc0c/acc18/acc14/acc24/acc25=0, aca52&=0xfffc, Posebuf +0x5f8..0x5fe Vorgaben, FUN_8001af5c(0,0,0x1f4,0x258,&acb04,0x80808080), je aca5c Matrix-Inits |
| 1 | 0x80031de8 | Boden-Steuerung (Zwei-Phasen-Dispatch, s. §3) |
| 2 | 0x80035af0 | (nicht Auftrag; nicht analysiert) |
| 3 | 0x800366bc | (dito) |
| 4 | 0x80030660 | eigene Sub-Tabelle @0x80073e30 (Leser @0x800306a4), Eintraege u.a. [7]@0x80073e4c=0x80031080, [8]@0x80073e50=0x800311f0 — enthaelt ZWEITEN Feuer-Einstieg (s. §4c) |
| 5 | 0x80036834 | (nicht analysiert) |
| 6 | 0x800368c0 | (nicht analysiert) |
| 7 | 0x8003694c | schreibt acaf2 @0x800369e4 und aca59 @0x800369ec (nicht analysiert) |

---

## 3. LAB_80031de8 — Mode 1: Zwei-Phasen-State-Dispatch (0x80073fb0 / 0x80073ff0)

```
80031de8  DAT_800acc0c |= 0x12
80031e10  if (DAT_800aca59 != 7):                        ; NUR ausserhalb des Feuer-States:
80031e20    if (FUN_8003703c(0xfa0)&0xff):               ;   Zufalls-/Zeitpruefung
80031e3c      DAT_800acc0c &= 0xed
80031e48  a0 = lhu DAT_800acaec                          ; Ziel-Wort
80031e50  if (a0 & 7):                                   ; irgendein Ziel-Bit 0..2
80031e60    DAT_800acc27-- ; bei 0: neu 0x78 (=120), bei (a0&4) 8   @0x80031e78/8c
80031e9c    DAT_800acaee -= 2 @0x80031ea4; Untergrenze 1 falls !(a0&4) @0x80031ec8
80031ed4  lbu v0 = DAT_800aca59                          ; STATE
80031ee0  at = 0x80073fb0 + state*4
80031eec  lw -> jalr                                     ; PHASE A (Input/Transition)
80031efc  lbu v0 = DAT_800aca59                          ; STATE NEU LESEN!
80031f08  at = 0x80073ff0 + state*4
80031f14  lw -> jalr                                     ; PHASE B (Anim/Aktion)
```

Phase B wird mit dem von Phase A ggf. **frisch gesetzten** State aufgerufen (erneutes lbu
@0x80031efc) — ein Uebergang wirkt also NOCH IM SELBEN FRAME auf Phase B.

**State-Tabellen (16 Eintraege je, PSX.EXE-Dump):**

| aca59 | Phase A @0x80073fb0 | Phase B @0x80073ff0 | Rolle (belegt) |
|---|---|---|---|
| 0 | 0x80031f38 | 0x80032038 | Stehen/Idle (B: Idle-Anim-Maschine, Clip 3 @0x80032088, Fidget-Timer acaf0=0x5a+rand&0x1f @0x800320b4) |
| 1 | 0x800322e8 | 0x80032498 | Gehen vorwaerts |
| 2 | 0x80032604 | 0x80032778 | Rennen |
| 3 | 0x80032974 | 0x80032ae4 | Drehen auf der Stelle |
| 4 | 0x80032bb8 | 0x80032d20 | Rueckwaertsgehen |
| 5 | 0x80032e1c (stub) | 0x80032e24 (stub) | leer |
| 6 | 0x80032e2c (stub) | 0x80032e34 (stub) | leer |
| 7 | 0x80032e3c (stub) | **0x80032e44** | **ZIELEN/FEUERN — nur Phase B arbeitet** |
| 8 | 0x8003579c | 0x80035810 | (Treffer-/Sonderstates; acae8/9-Writes 0x8003585c ff.) |
| 9 | 0x80037fd0 | 0x80037fd8 | (dito) |
| 10 | 0x8003830c | 0x80038314 | (dito) |
| 11 | 0x80038848 | 0x80038850 | (dito) |
| 12 | 0x80038c58 | 0x80038c60 | (dito) |
| 13 | 0x80038ef4 | 0x80038efc | (dito, aca59=0xd auch von FUN_8002dc48 @0x8002dd1c gesetzt) |
| 14 | 0x80035ad0 | 0x80035ad8 | leer-nahe Stubs |
| 15 | 0x80035ae0 | 0x80035ae8 | leer-nahe Stubs |

---

## 4. Eintritte in State 7 (Feuer) — alle Schreiber von 0x701

State-Uebergaenge laufen als **32-bit-Wort-Store auf DAT_800aca58**: LE-Bytes = (Mode, State,
Substate, SubSubstate). `sw 0x701` setzt also Mode=1, State=7 **und aca5a=aca5b=0** atomar.

a) **Stehen** LAB_80031f38 (Phase A, State 0):
```
80031f4c  a2 = DAT_800ac768 (HELD)
80031f78  held&1   (UP)    -> 0x101 @0x80031f88; zusaetzlich held&0x200 (Kreuz) -> 0x201
80031fa0  held&4   (DOWN)  -> 0x401 (Rueckwaerts)
80031fac  held&0xa (L/R)   -> 0x301 (Drehen)
80031fc4  edge&0x80 (SQ neu): FUN_8002d474() !=0: exit; FUN_80042bac(player,1,0x10) !=0: exit (Tuer)
80031ffc  held&0x100 (R1) && DAT_800aca5d != 0:
80032018    v0=0x701 -> sw DAT_800aca58 @0x80032020     ; => FEUER-STATE
```
b) **Rueckwaertsgehen** LAB_80032bb8 (Phase A, State 4): identisches Muster; R1-Pruefung
@0x80032c9c-cb8, `sw 0x701` @0x80032d08 (via LAB_80032d04). Waffe 0 (=keine/Slot 0)
faellt auf Geh-Transitionen zurueck @0x80032cbc-d00.

c) **Mode-4-Nebenpfad** LAB_800311f0 (Sub-Tabelle @0x80073e50, Mode 4): setzt bei
Zieldistanz < 100 (SquareRoot0 ueber (aca88−acc10)², (aca90−acc12)² @0x80031160-8c) erst
aca59=6 @0x800311a8, dann bei `DAT_800acc18 & 4` das Wort **0x701 @0x800311d0 UND aca5a=2
@0x800311d8** — Feuer-Einstieg DIREKT in Substate 2 (Sofort-Schuss). (Mode-4-Identitaet nicht
Teil dieses Auftrags; Adressen dokumentiert.)

Ausserdem setzt der Damage-Entry FUN_80012d60 Mode/State direkt (sb @0x80012ebc/ed4 auf
+0x4/+0x5 des Spieler-Blocks) — Treffer brechen den Feuer-State von aussen ab.

---

## 5. LAB_80032e44 — State 7 Phase B: Der Waffen-Dispatch (liest 0x80074030)

```
80032e44  addiu sp,-0x18
80032e48  a1 = &DAT_800aca54
80032e58  lw v0=[aca54]; or 0x40000000; sw @0x80032e6c   ; Entity-Flag Bit30 = "im Anschlag"
80032e60  lbu v1 = DAT_800aca5d                          ; Waffen-Id (angelegt)
80032e70  at = 0x80074030 + id*4
80032e7c  lw v0 = [at]                                   ; Handler
80032e84  jalr v0                                        ; -> Waffen-FSM
80032e94  jr ra
```

**Waffen-Tabelle @0x80074030 (PSX.EXE-Dump, 4 B je Id 0..19; [20..23]=NULL):**
`[0..2]=0x80034e70 (Nahkampf), [3..11,13,15..18]=0x80032e9c (Standard),
[12,14,19]=0x80034014 (Dauerfeuer)`. Id kommt aus `DAT_800aca5d` (lbu). Eine Id >= 20 wuerde
NULL laden — Dispatch-Werte > 19 kommen im Auslieferungsstand nicht vor (Eintraege 20..23
im Image = 0x00000000 @0x80074080-8c).

---

## 6. LAB_80032e9c — Standard-Feuer-FSM (liest 0x800740f4 + Bank-Tabelle 0x8007408b)

```
80032e9c  lbu v0 = DAT_800aca5a                          ; Substate
80032eb0  at = 0x800740f4 + sub*4
80032ebc  lw -> jalr @0x80032ec4                         ; SUBSTATE-DISPATCH (6 Eintraege)
80032ecc  lbu v1 = DAT_800aca5d
80032ed4  a0 = 0x8007408b                                ; 5-Byte-Records je Waffe
80032edc  v0 = id*5
80032ee8  lbu v1 = DAT_800aca5c ; &4 -> 0/1
80032efc  a1 = lbu [0x8007408b + id*5 + 3 + (aca5c&4?1:0)]  ; Anim-BANK
80032f00  jal FUN_800369f8(0, bank)                      ; JEDEN Frame nach dem Substate
```

**Substate-Tabelle @0x800740f4 — 6 Eintraege; [3..5] LIEGEN IN 0x80074100 (Ueberlappung!):**

| aca5a | Adresse | Handler | Rolle |
|---|---|---|---|
| 0 | 0x800740f4 | 0x80032f18 | Anlegen/Zielhaltung aufbauen (Clip 6 @0x80032f70) |
| 1 | 0x800740f8 | 0x80033180 | Bereit/Halten (Clip 8/10/12) — Abzugs-/Nachlade-Entscheidung |
| 2 | 0x800740fc | 0x80033460 | SCHUSS (Clip 7/9/11) — ruft Schuss-Tabelle EINMALIG |
| 3 | 0x80074100 | 0x80033c74 | Absetzen (Clip 6, acaec&=0x1fff @0x80033cc0; Clip-Ende -> aca59=0, aca5a=0 @0x80033d4c/54, aca3c&=~0xC0 @0x80033d58-68) |
| 4 | 0x80074104 | 0x80033d7c | Nachladen (Clip 0xd=13 @0x80033dac, acaec neutral 0x4000 @0x80033dd0) |
| 5 | 0x80074108 | 0x80033eec | Sonder-Halten (Clip 8/10/12 @0x80033f4c; FUN_80037250(0x7530) -> acaf3 @0x80033f5c; bei 0 -> sh 0x101 -> aca5a @0x80033f70 = zurueck zu Sub 1 mit aca5b=1) |

**Bank-Record-Tabelle @0x8007408b (5 B je Waffe, PSX.EXE-Dump):** Waffen 1,3,4: `18 30 07 xx
xx`; 5..13,19: `18 30 0a xx xx`; 0,2,14..18,20: alles 0. Byte-Belegung (Leser):
- **[0]** = Drehrate im Anschlag (0x18=24/Frame): gelesen @0x80033030/0x8003307c (`0x80074090 + (id-1)*5` = Byte 0 des Records id)
- **[1]** = Drehrate-Basis 0x30=48: beim FEUERN halbiert benutzt (`>>1` @0x800335a4 = 24; ebenso @0x800335f0), im Halten voll @0x800333d0
- **[2]** = Mindest-Feuerbilder (7 bzw. 10): R1-Release bricht Schuss-Clip erst ab, wenn `acae9 > [2]` @0x80033634-4c
- **[3]/[4]** = Anim-Bank normal/alternativ (aca5c&4) fuer FUN_800369f8 @0x80032efc

Uebergaenge (Details in den fsm-sub*-Dossiers dieser Serie; hier die Dispatch-relevanten):
- Sub 0: FUN_8001f314-Rueckgabe (Clip zu Ende) zaehlt aca5b 1->2 @0x800330c0-cc; bei 2:
  `sh 1 -> aca5a` @0x8003316c (Halbwort: aca5a=1, aca5b=0). Hoch/Tief erst ab acae9>=8/7
  @0x8003311c/0x800330c4.
- Sub 1 @0x800331e0-ff: `!(held&0x100)` -> `sh 3 -> aca5a` @0x80033200 (Absetzen).
  `held&0x40` && FUN_8004ea6c()!=0 -> `sh 2 -> aca5a` @0x80033328 (SCHUSS).
  leer && `edge&0x40` && FUN_8004eb70()!=0 && id<9 -> `sh 4` @0x80033378 (Nachladen);
  sonst Leerklick FUN_80045024(0x1010001,&aca88) @0x8003338c. Roh-L1-Edge
  (`DAT_800ac762 & 4`) -> `sh 5 -> aca5a` @0x800332f8.
- Sub 2: Clip-Ende (FUN_8001f314 @0x80033668) -> `sh 1 -> aca5a` @0x8003367c — zurueck zu
  Halten; haelt der Spieler SQUARE weiter, feuert Sub 1 im Folgeframe erneut (LEVEL-Abzug =
  automatische Schussfolge im Clip-Takt).

---

## 7. LAB_80033460 — Standard Sub 2: der EINMALIGE Aufruf der Schuss-Tabelle 0x80074100

```
80033460  lbu v0 = DAT_800aca5b
80033478  bne v0,0 -> LAB_80033508                       ; LATCH: nur im Eintrittsframe
80033480  aca5b = 1
80033488  a0 = lhu DAT_800acaec                          ; Ziel-Wort (Bit15=hoch, Bit13=tief)
80033490  v1 = lbu DAT_800aca5d                          ; Waffen-Id
8003349c  DAT_800acae9 = 0                               ; Bildzaehler reset
800334a4  DAT_800acae3 = 7
800334a8  clip = ((acaec>>15)<<1) + 7 + ((acaec>>11)&4)  ; 7=mitte, 9=hoch, 11=tief
800334c8  sb clip -> DAT_800acae8
800334cc  at = 0x80074100 + id*4
800334d8  lw v0 = [at]                                   ; SCHUSS-HANDLER DER WAFFE
800334e0  jalr v0                                        ; ohne Argumente; Handler lesen Globals
800334f8  DAT_800acc24 = 0x5a (=90)
800334fc  DAT_800aca52 |= 1                              ; "hat gefeuert" (Frame-Flag, Reset @0x80031c9c)
; --- ab hier JEDEN Frame des Substates ---
80033508  if (id==8 && (acae9==3 || ==5 || ==7)):        ; Waffe 8: Mehrfach-Resolves
80033554    jal FUN_80011f50(id, [player+0x7c])
8003355c  Drehen: held&8/2 -> acabe -/+= [0x8007408c+id*5]>>1   @0x80033598/0x800335e4
80033604  if !(held&0x100) && acae9 > [0x8007408d+id*5]: ; R1 los + Mindestbilder voll
80033648    sh 3 -> aca5a                                ; Absetzen
80033658  FUN_8001f314(acbc4, acbc8, 0, 0x200)           ; Clip vorwaertsschalten
80033670  bei Rueckgabe 1 (Clip-Ende): sh 1 -> aca5a     ; zurueck zu Halten
80033680  id==9: Zusatz-FX 0x040d1000 bei acae9==0x13/0x16/0x18 je Ziel-Bit @0x800336a0-80033790
```

**WICHTIG:** Die Schuss-Tabelle wird hier durch das aca5b-Latch **genau EINMAL pro Schuss**
aufgerufen (Eintrittsframe von Substate 2), nicht pro Frame.

**Schuss-Tabelle @0x80074100 (PSX.EXE-Dump, indiziert mit Waffen-Id):**
`[0]=0x80033c74 [1]=0x80033d7c [2]=0x80033eec` — das sind die Substate-3/4/5-Handler
(Doppel-Nutzung! Ids 0..2 sind Nahkampf und erreichen Sub 2 nie); `[3,4]=0x800337bc
[5,6]=0x800338a8 [7]=0x800339a4 [8]=0x80033a58 [9]=0x80033b38 [10]=0x80033b58 [11]=0x80033b78
[12]=0x800347f8 [13]=0x80033b98 [14]=0x800c45a8 (RAM! zur Laufzeit geladen) [15..18]=NULL
[19]=0x80034a30`. Eintraege [20..23] @0x80074150-5c EXISTIEREN NICHT als Schuss-Handler —
dort beginnt die Dauerfeuer-Substate-Tabelle (Ueberlappung; Id 20 hat Dispatch NULL und
kommt nie hierher).

---

## 8. LAB_80034014 — Dauerfeuer-FSM (liest 0x80074150)

```
80034014  lbu v0 = DAT_800aca5a                          ; Substate
80034028  at = 0x80074150 + sub*4
80034034  lw -> jalr @0x8003403c                         ; 10 Substates
80034044  jal FUN_800369f8(0, 1)                         ; Bank FIX = 1 (nicht tabellengetrieben!)
```

**Substate-Tabelle @0x80074150 (PSX.EXE-Dump):** `[0]=0x80034060 [1]=0x80034278 [2]=0x80034510
[3]=0x80034c74 [4]=0x80034d68 [5]=0x80034ee8 [6]=0x800350c4 [7]=0x80035314 [8]=0x80035424
[9]=0x80035538 [10,11]=NULL`.

**Sub 1 = LAB_80034278 (Halten):**
```
80034294  aca5b==0: acae3=7; aca5b=1; acae9=0 @0x800342b8
800342bc  clip = 9 + 3*(acaec>>15) + 6*((acaec&0x2000)?1:0)   ; 9/12/15 -> acae8 @0x800342e8
800342f8  !(held&0x100) -> sh 3 -> aca5a @0x8003430c     ; R1 los: Absetzen
80034318  held&0x10 -> acaec hoch (0x8000), aca5b=0 @0x8003433c; held&0x20 -> tief; sonst neutral
800343f8  held&0x40 (ABZUG):
80034404    FUN_8004ea6c() != 0:
80034418      sh 2 -> aca5a  @0x80034418                 ; FEUERSCHLEIFE
80034420      acae9 = 0                                  ; Bildzaehler
80034428      DAT_800acaf2 = 0                           ; FEUERTAKT-ZAEHLER reset
80034434    leer: edge&0x40 && aca5d==0x12(18): FUN_8004eb70()!=0 -> sh 4 @0x80034474
              (toter Zweig: Id 18 dispatcht auf 0x80032e9c, nie hierher); sonst Klick
              FUN_80045024(0x1010001,&aca88) @0x80034488
80034490  Drehen hart +/-0x30 (48) @0x800344b4/800344d8  ; NICHT tabellengetrieben
800344f4  FUN_8001f314(acbc4,acbc8,0,0x200)
```

**Sub 2 = LAB_80034510 (Feuerschleife) — aca5b-Maschine:**
```
aca5b==0 @0x80034564: aca5b=1; acae3=7;
  clip = 7 + 3*(acaec>>15) + 6*((acaec&0x2000)?1:0)      ; 7/10/13 -> acae8 @0x800345a8
aca5b==1 @0x800345ac: JEDEN FRAME:
  DAT_800acc24=0x5a; DAT_800aca52|=1 @0x800345d4
  at = 0x80074100 + aca5d*4; lw; jalr @0x800345e4-ec     ; SCHUSS-HANDLER PRO FRAME
  Drehen -/+0x18 (24, halbiert) @0x80034618/8003463c
  !(held&0x100) -> sh 1 -> aca5a @0x800347e4             ; R1 los -> Halten
  FUN_8001f314(...) @0x80034664
  held&0x10/0x20 Zielwechsel -> aca5b=0 (Clip neu) @0x800346d8-e4 / 0x80034728-30
  !(held&0x40) -> sb 2 -> aca5b @0x80034754              ; ABZUG LOS -> Feuer-Ende
aca5b==2 @0x80034764: aca5b=3; acae9=0 @0x8003477c;
  clip = 8 + 3*b15 + 6*b13                               ; 8/11/14 (Abkling-Clip) @0x800347b4
aca5b==3 @0x800347bc: FUN_8001f314; bei Clip-Ende sh 1 -> aca5a @0x800347e4
```

**Der Feuertakt liegt NICHT im FSM, sondern im Waffen-Schuss-Handler** (pro Frame gerufen):

**LAB_800347f8 (Ingram M10, Id 12) — komplett:**
```
80034818  a0 = acae9; a0 % 3 (multu 0xAAAAAAAB @0x80034820)   ; MAGIC /3
8003483c  ==0: FUN_80019700(0x02010800, yaw=lh acabe, gunbone=[acbdc]+0x7a4,
            offs (0x46,0x41a,-0x3c)=(70,1050,-60); bei aca5c&4 (60,1020,-90))
            @0x80034844-b0                                ; Muendungsblitz alle 3 Bilder
800348b8  acae9 % 6 (mfhi>>2 @0x800348c8) ==0:
800348e8    FUN_80019700(0x03000b00, yaw, gunbone, gleiche Offsets)  ; alle 6 Bilder
80034910  lb acaf2 % 7 (Magic 0x92492493 @0x80034914-40) ==0:
8003494c    FUN_80019700(0x04000800, yaw, [acbdc]+0x7a4,
              offs (0xc8,0x64,-0x14)=(200,100,-20); bei aca5c&4 (160,100,-60)) ; alle 7 Takte
800349b8  acaf2++ @0x800349c4                             ; JEDEN Frame der Schleife
800349bc  if ((acae9 & 4) == 0):                          ; nur Bilder mit Bit2=0
800349c8    FUN_8004eae4()                                ; Munition -1
800349d0    !=0: FUN_80011f50(aca5d, [player+0x7c]) @0x800349ec   ; Schaden
800349fc    ==0: aca5b = 2 @0x80034a00 (Feuer-Ende) + Klick FUN_80045024(0x1010001,&aca88)
```

---

## 9. Zaehler/Globals — wer schreibt was

| Global | = Entity-Feld | Schreiber (Feuer-Pfad) |
|---|---|---|
| DAT_800aca58 (Mode) | +0x04 | Wort-Stores 0x…01/0x101/0x201/0x301/0x401/**0x701** (Liste §4); sb 0 @0x8001cbdc; FUN_80012d60 @0x80012ebc/ef4 (Damage) |
| DAT_800aca59 (State) | +0x05 | Teil der Wort-Stores; sb 0 bei Rueckkehr @0x80033d4c (Absetzen fertig), 0x80034d38, 0x80035500; FUN_8002dc48 setzt 0xd @0x8002dd1c |
| DAT_800aca5a (Substate) | +0x06 | Wort-Store nullt beim State-Wechsel; im FSM ausschliesslich **sh-Halbwort-Stores** (setzen aca5b gleich mit 0): 1 @0x800330fc/0x8003314c/0x8003316c/0x8003367c/0x800347e4, 2 @0x80033328/0x80034418, 3 @0x80033200/0x8003430c/0x80033648, 4 @0x80033378/0x80034474, 5 @0x800332f8, 0x101 @0x80033f70 |
| DAT_800aca5b (SubSub) | +0x07 | sb 1 als Einmal-Latch je Handler (0x80032f64, 0x80033480, 0x80033c94, 0x800342b0, 0x80034564 …), Zielwechsel-Resets sb 0 (0x80033230, 0x8003327c, 0x800332d0, 0x800346dc, 0x80034730), Dauerfeuer-Ende sb 2 @0x80034758/0x80034a00, 3 @0x80034768; Anlegen-Fortschritt aca5b += FUN_8001f314-Rueckgabe @0x800330c0-cc |
| DAT_800acae8 (Clip) | +0x94 | Substate-Inits: 6 @0x80032f70 (Anlegen), 8/10/12 @0x800331dc (Halten: 8+2*b15+4*b13), 7/9/11 @0x800334c8 (Schuss), 6 @0x80033ca0 (Absetzen), 0xd @0x80033dac (Nachladen); Dauerfeuer: 9/12/15 @0x800342e8, 7/10/13 @0x800345a8, 8/11/14 @0x800347b4; Idle-Anim 3/1 @0x80032088/0x80032140. KEIN annotierter Leser — wird als entity[0x94] vom Anim-System konsumiert |
| DAT_800acae9 (Bild) | +0x95 | Resets sb 0 an jedem Clip-Start (26 Stellen, u.a. 0x8003349c, 0x800331bc, 0x800342b8, 0x80034420); **Inkrement NUR in FUN_8001f314: `[DAT_800ac784]+0x95 += 1` @0x8001f610-1c, bei >= Cliplaenge -> =0 und Rueckgabe 1 @0x8001f628-3c** (zweite Instanz des Musters @0x8001fb4c-78 fuer den B-Puffer) |
| DAT_800acaf2 (Feuertakt) | +0x9e | Reset 0 @0x80034428 (Feuerschleifen-Eintritt); ++ @0x800349b8/c4 (Ingram, jeden Schleifen-Frame; MC51 0x80034a30 analog ab @0x80034a5c); ausserdem generischer Zaehler in States 8..11 (0x80035e70, 0x80035ff4, 0x80036174, 0x800364d8, 0x800389dc) |
| DAT_800aca52 | +0x5e | Bit0 "Schuss in diesem Frame": gesetzt @0x800334fc/0x800345c8-d4, geloescht jeden Frame @0x80031c9c |
| DAT_800aca54 | +0x00 (Flags) | Bit30 "im Anschlag" @0x80032e58-6c (LAB_80032e44) |
| DAT_800acaec | +0x98 (Ziel-Wort) | Bit14=neutral, Bit15=hoch, Bit13=tief; gesetzt/geloescht in Sub 0/1/2 beider FSMs (0x80032f98-a4, 0x80033238, 0x80033284, 0x800332c8, 0x8003310c, 0x8003315c, 0x80034344, …) |

Hilfsfunktionen: **FUN_8004ea6c** = "Magazin nicht leer?": Slot `DAT_800b25c8`, Inventar-Array
Basis 0x800b10ac (4 B/Slot), Feld+2==2 -> Slot-1 (Doppel-Slot), Slot==0x80 -> 0, sonst
`Feld+1 (Munition) != 0` (@0x8004ea6c-eae0). **FUN_8004eae4** = Munition−1 (bekannt).
**FUN_8004eb70** = Reserve-Pruefung fuer Nachladen (Aufrufkontext @0x8003334c/0x80034460).

---

## 10. RE2-Abgleich

- `ori reg,0x701`/kombinierte Mode-State-Wort-Stores existieren in `ghidra_re2_Leon.txt`
  NICHT (grep leer) — RE2 Retail hat die Spieler-FSM auf entity-eigene routine-Bytes
  umgebaut; die RE1.5-Mechanik (globale Wort-FSM 0x800aca58) ist ein Vorgaenger-Design.
  Struktur-Belege wurden daher NICHT aus RE2 uebernommen; alles oben ist RE1.5-nativ belegt.
- Tasten-Rohlayout belegt ueber PsyQ-SDK-Header statt RE2:
  `info/Resident_Evil_und_Playstation_Information/PSYQ_SDK/psyq/include/LIBETC.H:14-35`
  (PADl=1<<3=R1, PADRleft=1<<7=SQUARE, PADRdown=1<<6=CROSS, PADn=1<<2=L1, PADh=1<<11=START).

## 11. Offen / unklar

- Semantik der Mode-Tabelle 0x80073f90 Eintraege [2],[3],[5],[6],[7] und der Mode-4-Kette
  (Sub-Tabelle @0x80073e30; Eintrag [8]=0x800311f0 mit Sofort-Feuer) — Adressen notiert,
  Verhalten nicht Teil dieses Auftrags.
- DAT_800aca5c: Low-Nibble wird live aus Options-Global DAT_800b0ff0 gespiegelt
  (@0x80039760-8c, einziger sb-Schreiber; FUN_800314b0 bei Wechsel). Bit2 waehlt
  Alternativ-Bank (Record-Byte[4]) und alternative FX-Offsets. Vermutung Charakter/Kostuem
  (Leon/Elza) — NICHT belegt.
- Wozu Substate 5 (0x80033eec, roher L1-Edge) spielerisch dient (FUN_80037250-Wuerfel,
  Rueckkehr zu Halten) — Mechanik belegt, Zweck nicht.
- Der Nachlade-Zweig im Dauerfeuer-Sub-1 verlangt aca5d==0x12 (18) @0x80034448-58, obwohl
  Id 18 laut Dispatch nie im Dauerfeuer-FSM laeuft — toter Zweig oder Rest einer
  Entwicklungs-Belegung.
- Konsumenten von DAT_800acae0 (0x46/'F' bzw. 0x41/'A' aus Tabelle 0x80073ec4) und
  DAT_800acae3 (immer 7 im Feuerpfad) — Schreiber belegt, Leser laufen ueber Zeiger.
- Waffe 14 (Flammenwerfer): Schuss-Handler 0x800c45a8 liegt im RAM ausserhalb der EXE
  (Overlay/zur Laufzeit geladen) — hier nicht disassembliert.
