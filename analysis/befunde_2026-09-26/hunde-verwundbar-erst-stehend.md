# Hunde: erst wieder verwundbar, sobald sie wieder stehen — der Mechanismus EXISTIERT (+0x1D3 Bit 0x80, freigegeben am Clip-Ende), der Port maskiert ihn weg

STATUS: fertig (2026-09-26). Keine Engine-Aenderung in diesem Lauf — Beleg + Umsetzungsplan.

## 1. Was der Nutzer meldet / was ich gemessen habe

Nutzer: "Im Original Resident Evil 2 sind die Hunde erst dann wieder verwundbar,
sobald sie wieder stehen."

Zu loesen war der Widerspruch zu `reai-v2-runde13-hunde-trefferpause`
("⛔ RE2 SPERRT NACH EINEM TREFFER UEBER ZEIT, NICHT UEBER DEN ZUSTAND") und
`reai-v2-runde14-trefferpause-alle`.

**Aufloesung an den Bytes: `+0x1D3` ist KEIN reiner Zaehler, sondern ein Byte mit ZWEI
Haelften.** Runde 13 hat nur die untere Haelfte (low-7 = Bilder) gesehen und Bit 0x80
als "ueberlebt" abgehakt, ohne zu klaeren, was es TUT. Bit 0x80 ist der
Zustands-Riegel, den die Hurt-/Sturz-Zweige des Hunde-Overlays SETZEN und beim
Rueckweg nach ACTIVE LOESCHEN. Der Kandidatenfilter prueft das **ganze Byte**, nicht
die low-7 — also sperrt Bit 0x80 allein, zeitunabhaengig.

## 2. Das Original — mit Adressen und Bytes

### 2.1 Der Kandidatenfilter prueft das GANZE Byte (nicht low-7)

`FUN_800470C0`, RE2 `info/re2leon/PSX.EXE` (t_addr 0x80010000, RAM = 0x80010000 + off − 0x800).
Selbst disassembliert:

```
800470c4: lbu v0,-1037(v0)      ; 0x800cfbf3  Vorbedingung (Zielmodus aktiv)
8004712c: andi v0,v0,0x1        ; Gate 1: flags&1 (aktiv)
80047130: beq  v0,zero,0x8004740c
80047138: lbu  v0,467(s0)       ; Gate 2: +0x1D3  **GANZES BYTE**
80047140: bne  v0,zero,0x8004740c ; != 0 -> Kandidat FAELLT RAUS
80047148: lh   v0,342(s0)       ; Gate 3: hp (+0x156)
80047150: bltz v0,0x8004740c
80047158: lhu  v0,270(s0)       ; Gate 4: +0x10E & 0xC000
80047164: bne  v0,zero,0x8004740c
```

⛔ Entscheidend: `bne v0,zero` @0x80047140 steht auf dem **unmaskierten** `lbu`
@0x80047138. Es gibt nirgends ein `andi 0x7f` davor. **Bit 0x80 allein macht den Gegner
unantastbar — ohne jeden Zeitbezug.**

### 2.2 Der Root zieht NUR die untere Haelfte ab — Bit 0x80 ueberlebt jeden Frame

Hunde-Root `info/re2leon/COMMON/BIN/EMD0G_MOD0.BIN` @0x80100000 (Overlay laedt @0x80100000):

```
80100028: lbu  v1,467(s0)       ; 92 03 01 d3
80100030: andi v0,v1,0x7f       ; 30 62 00 7f   <- Gate NUR auf low-7
80100034: beq  v0,zero,0x80100040
80100038: addiu v0,v1,-1        ; 24 62 ff ff   (delay slot)
8010003c: sb   v0,467(s0)       ; a2 02 01 d3
```

Der Abzug ist bedingungslos (vor jeder Weiche), aber er laeuft nur, solange low-7 != 0.
Erreicht low-7 die 0, bleibt das Byte auf **0x80**, wenn Bit 0x80 gesetzt ist — und der
Filter @0x80047140 sperrt weiter. **Der Zeitgeber laeuft ab, die Sperre nicht.**

### 2.3 Der Schadens-Stempel rettet Bit 0x80 ausdruecklich hinueber

`FUN_800470C0`-Applier (RE2 PSX.EXE), zwei identische Bloecke:

```
8004731c: lbu  a0,467(s1)
8004732c: andi a0,a0,0x80       ; 30 84 00 80   <- Bit 0x80 BEHALTEN
80047334: sb   a0,467(s1)
80047338: lw   v0,4(a1)         ; Schadenszeile
80047340: srl  v0,v0,9
80047344: andi v0,v0,0x7f       ; Stun-Bilder aus der Zeile
80047348: or   a0,a0,v0
8004734c: sb   a0,467(s1)
```
(zweiter Block byte-gleich @0x8004756C-0x8004759C)

Der Stempel schreibt also `0x1D3 = (alt & 0x80) | stun_frames`. Bit 0x80 gehoert NICHT
dem Treffer — es gehoert dem Zustand.

### 2.4 Wer SETZT Bit 0x80 (Hunde-Overlay, alle selbst nachgelesen)

Muster `ori rX,rX,0x80` (Wort `3442 0080` / `3463 0080`) direkt vor `sb rX,467(...)`:

| Setzer (ori) | Store | Funktion |
|---|---|---|
| 0x80102ce4 | 0x80102cec | 0x80102c78 |
| 0x80103188 | 0x8010318c | 0x80103138 |
| 0x8010389c | 0x801038a4 | 0x801037e8 |
| 0x80103e34 | 0x80103e3c | 0x80103d9c |
| 0x80104f6c | 0x80104f74 | 0x80104df0 |
| 0x801051f8 | 0x80105200 | 0x80105070 |

### 2.5 Wer LOESCHT Bit 0x80

Muster `andi rX,rX,0x7f` (Wort `3042 007f` / `3063 007f`) direkt vor `sb rX,467(...)`:

| Loescher (andi 0x7f) | Store | Funktion |
|---|---|---|
| 0x80100600 | 0x80100608 | 0x80100548 |
| 0x80100890 | 0x80100894 | 0x80100824 |
| 0x80100c00 | 0x80100c08 | 0x80100ba8 |
| 0x80102e3c | 0x80102e40 | 0x80102e0c |
| 0x80102fa0 | 0x80102fa4 | 0x80102f74 |
| 0x801030ac | 0x801030b0 | 0x80102fbc |
| 0x8010327c | 0x80103280 | 0x8010321c |
| 0x80103710 | 0x80103718 | 0x801035e8 |
| 0x801037a4 | 0x801037a8 | 0x8010376c |

(Zuordnung Setzer/Loescher -> Zustand folgt in diesem Dossier.)

### 2.6 Hunde-Zustandstabelle

`lbu v0,4(s0)` @0x80100054 -> `lw v0,21560(at=0x80100000)` @0x8010006c
=> Tabelle **@0x80105438**:
`[0]=0x801000f4  [1]=0x801004dc  [2]=0x801032a8  [3]=0x801040dc  [4]=0x801049e4
 [5]=0  [6]=0  [7]=0x801049ec`


### 2.7 Die Kette: SETZEN beim Absprung — LOESCHEN erst am ENDE des Lande-Clips

ACTIVE (state[1] = 0x801004dc) waehlt per `lbu v0,5(s0)` @0x801004ec aus Tabelle
@0x80105464. Die Sturz-Zweige sind die Eintraege [14]=0x80102c28, [15]=0x80102e58,
[16]=0x801030fc. Jeder von ihnen faechert per `lbu v0,6(...)` weiter auf:

| Zweig | Phasentabelle | Phase 0 | Phase 1 | Phase 2 | Phase 3 |
|---|---|---|---|---|---|
| [14] 0x80102c28 (`lw v0,21780(at)` @0x80102c4c) | @0x80105514 | 0x80102c78 **SET** | 0x80102d98 | 0x80102e0c **CLR** | — |
| [15] 0x80102e58 (`lw v0,21792(at)` @0x80102e7c) | @0x80105520 | 0x80102c78 **SET** | 0x80102ea8 | 0x80102f74 **CLR** | 0x80102fbc **CLR** |
| [16] 0x801030fc (`lw v0,21808(at)` @0x80103118) | @0x80105530 | 0x80103138 **SET** | 0x80102d98 | — | — |

**Phase 0 setzt den Riegel** (0x80102c78, per `lbu v1,7(s0)` @0x80102c94 auf Unterphase 0):
```
80102cd4: lbu  v0,467(s0)
80102cd8: lbu  v1,448(s0)
80102cdc: sb   zero,537(s0)
80102ce0: sb   zero,557(s0)
80102ce4: ori  v0,v0,0x80      ; 34 42 00 80   <- RIEGEL AN
80102ce8: ori  v1,v1,0x2
80102cec: sb   v0,467(s0)      ; a2 02 01 d3
80102cf0: j    0x80102d7c
80102cf4: sb   v1,448(s0)
```

**Phase 2 loescht ihn — und zwar NUR, wenn die Aufsteh-Animation durch ist:**
```
80102e0c: addiu sp,sp,-24
80102e18: jal  0x80101574      ; Aufsteh-Routine; Rueckgabe != 0 heisst "Clip zu Ende"
80102e1c: addu s0,a0,zero
80102e20: beq  v0,zero,0x80102e44   ; NICHT fertig -> gar nichts tun, Riegel bleibt
80102e24: addiu v0,zero,2
80102e28: lbu  v1,467(s0)
80102e2c: sb   v0,5(s0)        ; zurueck in ACTIVE-Sub 2
80102e30: sh   zero,6(s0)
80102e34: sb   zero,541(s0)
80102e38: sb   zero,556(s0)
80102e3c: andi v1,v1,0x7f      ; 30 63 00 7f   <- RIEGEL AUS
80102e40: sb   v1,467(s0)
```
Byte-gleich im Zweig [15] @0x80102f74:
```
80102f80: jal  0x80101574
80102f88: beq  v0,zero,0x80102fa8
80102f8c: addiu v0,zero,13
80102f90: sb   v0,5(s0)
80102f94: lbu  v0,467(s0)
80102fa0: andi v0,v0,0x7f
80102fa4: sb   v0,467(s0)
```

### 2.8 "Clip zu Ende" ist woertlich der Animations-Rueckgabewert

`FUN_80101574` (Hunde-Overlay) gibt nur an EINER Stelle 1 zurueck:
```
80101654: jal  0x8002959c        ; Pose setzen + Frame weiterzaehlen
80101658: addiu a3,zero,256
8010165c: bne  v0,zero,0x80101668  ; Ergebnis != 0 -> Rueckgabe 1
80101660: addiu v0,zero,1          ; (delay slot)
80101664: addu v0,zero,zero        ; sonst Rueckgabe 0
```
`FUN_8002959c` (RE2-EXE @0x8002959c) ist ein Tail-Call auf `FUN_80029614`, und dessen
Rueckgabe ist woertlich "Clip umgeschlagen":
```
bVar3 = *(char *)(iVar15 + 0x14d) + 1;   // Animations-Frame +0x14D
*(byte *)(iVar15 + 0x14d) = bVar3;
bVar1 = uVar16 <= bVar3;                 // Frame >= Clip-Laenge ?
if (bVar1) *(undefined1 *)(iVar15 + 0x14d) = 0;
return bVar1;
```
(Decompilat `RE2_Quellcode_V2/FUN_80029614.c`; die Aufrufkette selbst ist oben
disassembliert, `lbu v0,332(a0)`/`lbu v0,333(a0)` @0x800295a8/@0x800295d0 =
Clip-Index +0x14C / Frame +0x14D.)

**Damit haengt die Freigabe an der ANIMATION, nicht an einer Zahl.**

### 2.9 Auch die Hurt-Zustandsmaschine (state[2]) fuehrt denselben Riegel

`state[2] = 0x801032a8` waehlt per `lbu v0,5(a0)` @0x801032c4 (dort traegt +0x5 die
**Waffen-Id**, gestempelt `sb s5,5(s1)` @0x80047324) aus Tabelle @0x80105538.
Generischer Zweig 0x80103308 faechert per `lbu v0,6(a0)` @0x80103310 in @0x80105588:
`[0]=0x80103344 [1]=0x801034c8 [2]=0x801035e8 (CLR @0x80103718)
 [3]=0x8010376c (CLR @0x801037a8) [4]=0x801037e8 (SET @0x801038a4)`.
Waffenzeilen [10] und [16] gehen auf 0x80103d9c (SET @0x80103e3c).
`0x8010376c` setzt in Phase 0 zugleich den Clip `sw a0,332(s0)` @0x801037a0 mit
`a0 = 0x00030f16` (`lui a0,0x3` @0x8010378c / `ori a0,a0,0xf16` @0x80103790) und gibt
den Riegel frei; auf Clip-Ende (`jal 0x8002959c` @0x801037b0, `beq v0,zero` @0x801037b8)
geht es nach `+0x4 = 0x201` (`addiu v0,zero,513` @0x801037c8 / `sw v0,4(s0)` @0x801037d0).

## 3. Was der Port tut — mit datei.c:zeile

### 3.1 Der Port setzt und loescht Bit 0x80 byte-true — er WERTET es nur nicht aus

Alle Setz-/Loesch-Stellen des Originals stehen im Port, mit Adresse:

| datei.c:zeile | Code | Original |
|---|---|---|
| `enemy_ai_re2_dog.c:1436` | `re2z_self1d3 \|= 0x80u` | @0x80102CD4-EC (Sprung-Start) |
| `enemy_ai_re2_dog.c:1474` | `re2z_self1d3 &= 0x7fu` | @0x80102E28-40 (Landung sub 14) |
| `enemy_ai_re2_dog.c:1502` | `&= 0x7fu` | @0x80102F94-A4 (Landung sub 15) |
| `enemy_ai_re2_dog.c:1518` | `&= 0x7fu` | @0x801030A0-B0 |
| `enemy_ai_re2_dog.c:1621` | `&= 0x7fu` | @0x80103708-18 (Ende Hurt-P2 = aufgestanden) |
| `enemy_ai_re2_dog.c:1655` | `&= 0x7fu` | @0x801037A4-A8 (Hurt-P3) |
| `enemy_ai_re2_dog.c:1938` | `\|= 0x80u` | @0x80103E28-3C (Hurt-Zeilen 10/16) |
| `enemy_ai_re2_dog.c:1960` | `&= 0x7fu` | @0x80103268-80 (Zeile 0) |
| `enemy_ai_re2_dog.c:566` | `\|= 0x80u` | @0x80104F64-74 (Spieler-Toetung) |

### 3.2 ⛔ DER DEFEKT: das Gate maskiert Bit 0x80 weg

`re15_port/engine/src/re15_damage.c:3098` (`re15_re2_pause_filter_apply`, zustaendig fuer
Hund 0x20, Kraehe 0x21, Spinne 0x25/0x26):

```c
if ((e->re2z_self1d3 & 0x7fu) == 0u) e->hit_react &= (uint8_t)~1u;
```

Das Original prueft an derselben Stelle das GANZE Byte:
`lbu v0,467(s0)` @0x80047138 / `bne v0,zero,0x8004740c` @0x80047140 — ohne `andi`.

Zum Vergleich: die ZOMBIE-Familie macht es richtig — `enemy_ai_re2_zombie.c:8597`:
```c
&& (spawn_pose || e->re2z_self1d3 == 0u)       /* (2) +0x1D3     @0x80047138-40 */
```

### 3.3 Die zweite, zu fruehe Freigabe steckt im Hunde-Root

`re15_port/engine/src/enemy_ai_re2_dog.c:2224-2255`:
```c
if (e->re2z_self1d3 & 0x7fu) {
    e->re2z_self1d3 = (uint8_t)((e->re2z_self1d3 & 0x80u) | ((e->re2z_self1d3 & 0x7fu) - 1u));
    if ((e->re2z_self1d3 & 0x7fu) == 0u) {     /* :2226 */
        ...
        e->hit_react &= (uint8_t)~1u;          /* :2255 */
    }
}
```
Der Kommentar darueber (`:2237-2242`) sagt woertlich: *"Ein Zustands-Gate gibt es in RE2
NICHT: FUN_800470C0 hat vier Gates ... und prueft die Hurt-Animation nirgends."*
**Das ist genau die halbe Wahrheit, die diese Runde aufloest**: FUN_800470C0 prueft die
Animation tatsaechlich nicht — aber es prueft das Byte, in dem der Zustands-Riegel WOHNT
(Bit 0x80), und dieses Bit loescht die Lande-/Aufsteh-Animation
(@0x80102E18-40, @0x80102F80-A4, @0x80103708-18, @0x801037A4-A8).

Aufrufstelle des Filters: `re15_port/engine/src/game_step_common.c:2163-2174`.

## 4. Der Unterschied, in einem Satz

Der Port gibt den Hund frei, sobald der Bilder-Zaehler (+0x1D3 low-7) abgelaufen ist, und
uebersieht dabei den Pose-Riegel im selben Byte (Bit 0x80), den das Original im selben Gate
mitprueft (`bne v0,zero` @0x80047140 auf dem unmaskierten `lbu` @0x80047138) und erst am
ENDE der Lande-/Aufsteh-Animation loescht.

### 4.1 ⛔ Praezisierung — wofuer der Pose-Riegel im Original scharf ist

Damit die Umsetzung nicht ueberschiesst (Beleg-Grenze klar benennen): am HUND wird Bit 0x80
nur an drei erreichbaren Stellen gesetzt, nicht bei jedem Treffer.

| Fall | Setzer | Freigabe |
|---|---|---|
| Sprung/Satz (ACTIVE-Subs 14/15, Fenster/Hindernis) | @0x80102CE4-EC | Lande-Clip zu Ende @0x80102E18-40 / @0x80102F80-A4 |
| Hurt-Zeilen 10 und 16 | @0x80103E34-3C | Ende der Aufsteh-Kette @0x80103708-18 (P2) bzw. @0x801037A4-A8 (P3) |
| Spieler wird gefressen | @0x80104F64-74 | — (Spieler ist dann tot) |
| ACTIVE-Sub 16 | @0x80103188-8C | im Port nicht erreichbar (Stub `re2d_sub16_open`) |
| Hurt-+0x6 == 4 / Death-Router[3] | @0x8010389C-A4 | belegt unerreichbar (+0x6 nie > 3) |

Zeile 10 und Zeile 16 sind nach der im Port dokumentierten Zuordnung
(`enemy_ai_re2_zombie.c:3693-3709`) **GL Brand** (RE1.5-Waffen 11 und 17) und
**Flammenwerfer** (RE1.5-Waffe 14) — die FEUER-Zeilen. Fuer Pistole/Schrot/Magnum setzt der
Hund Bit 0x80 im Original NICHT; dort endet die Sperre nach den Stun-Bildern der
Schadenszeile (Hund 0x800A4424: meist 15).

**Folgerung, ehrlich:** "erst wieder verwundbar, sobald sie wieder stehen" ist am Original
BELEGT fuer Sprung/Satz und fuer die Feuer-Zeilen; fuer die gewoehnliche Kugel ist es am
Original NICHT belegt. Der Port ist in allen belegten Faellen zu frueh treffbar.

## 5. Umsetzungsplan (konkrete Dateien/Zeilen, jede Konstante mit @0x)

**Schritt 1 — Gate byte-true machen (die eigentliche Korrektur).**
`re15_port/engine/src/re15_damage.c:3098`
```c
/* Gate (2) FUN_800470C0: `lbu v0,467(s0)` @0x80047138 / `bne v0,zero` @0x80047140 —
 * UNMASKIERT. Bit 0x80 ist der Pose-Riegel (Setzer @0x80102CE4-EC / @0x80103E34-3C,
 * Freigeber @0x80102E28-40 / @0x80102F94-A4 / @0x80103708-18 / @0x801037A4-A8). */
if (e->re2z_self1d3 == 0u) e->hit_react &= (uint8_t)~1u;
```

**Schritt 2 — die zweite Freigabe im Root nachziehen.**
`re15_port/engine/src/enemy_ai_re2_dog.c:2226` und `:2255`: die Bedingung fuer
`hit_react &= ~1u` von `(self1d3 & 0x7f) == 0` auf `self1d3 == 0` heben. Der Dekrement in
`:2224-2225` bleibt unveraendert — er ist byte-true zu @0x80100028-3C, das `andi 0x7f`
@0x80100030 gehoert DORT hin. Den Kommentarblock `:2229-2253` berichtigen: die Aussage
"Ein Zustands-Gate gibt es in RE2 NICHT" ist falsch und durch §2.1/§2.7/§2.8 zu ersetzen.

**Schritt 3 — vor dem Scharfschalten MESSEN, ob jedes Setzen eine erreichbare Freigabe hat.**
⛔ Das ist die Falle aus `reai-v2-runde14-trefferpause-alle`: ein Gate auf einen nie
geloeschten Riegel sperrt fuer immer. Bit 0x80 ist zwar kein Zaehler, aber die
Setzer/Loescher-Bilanz ist je Typ unterschiedlich:
* Hund: 4 Setzer / 8 Loescher (Tabelle §3.1) — Bilanz ok.
* **Kraehe: 4 Setzer, 1 Loescher.** `enemy_ai_re2_crow.c:1514/1597/1618/1695` schreiben
  `re2z_self1d3 = 128` (Original `li v0,128` + `sb v0,467` @0x801039F0-F4, @0x80103C18-1C,
  @0x80103C84-88, @0x80103F04-08 in EMOVL21_S0.BIN — selbst nachgelesen), Freigabe nur
  `:583` (`andi v1,v1,0x7f` @0x80100FC4 / `sb v1,467` @0x80100FD0). **Hier zuerst messen.**
* Spinne: `enemy_ai_re2_spider.c:1823` setzt (@0x80103214), `:1848` loescht (@0x80103380);
  `:2714` schreibt 255 (@0x801007C8), `:2717` nullt (@0x801007E4).

Messung: eine Sonde nach dem Muster `probe_hund_trefferluecke` (echter Pfad, NICHT ein
nachgebauter Filter — `reai-v2-schiene-mass-am-hindernis`), die ueber N Bilder je Aktor
protokolliert, ob `re2z_self1d3 & 0x80` wieder 0 wird. Solange die Kraehe das nicht zeigt,
Schritt 1/2 nur fuer Typ 0x20 scharf schalten und Kraehe/Spinne mit dem Messergebnis
nachziehen — kein Env-Schalter, kein Dauerzustand.

**Schritt 4 — Regressionstest.** Erweiterung von `tests/unit/test_re2_dog_hitwindow` bzw.
`probe_hund_trefferluecke`: den Hund ueber Sub 15 in die Luft bringen und nachweisen, dass
er zwischen `+0x1D3 |= 0x80` (@0x80102CE4-EC) und dem Lande-Clip-Ende (@0x80102E18-40) in
KEINEM Bild Kandidat ist und im Bild danach wieder.
⛔ Der Test muss beim ZIELEN anfangen (`game_step` + Pad), nicht bei
`re15_enemy_take_damage` — sonst misst er die falsche Haelfte der Kette
(`reai-v2-hit-latch-93`).

## 6. Offen / nicht belegt

1. **Welche Waffe hinter Reaktions-Id 0x0A und 0x10 steht.** Belegt ist, dass die
   HURT-Zeilen 10 und 16 Bit 0x80 setzen (@0x80103E34-3C) und dass die Zeile aus +0x5 kommt
   (`lbu v0,5(a0)` @0x801032C4, gestempelt `sb s5,5(s1)` @0x80047324). Die a3-Werte der
   Aufrufer habe ich selbst gescannt (`jal 0x800470c0` an 17 Stellen; a3-Low-Byte
   0x01/0x09/0x0A/0x0B/0x0C/0x0E/0x10/0x11; 0x0A @0x800207BC, @0x80021060, @0x8002108C,
   @0x800214F8, @0x80021524 — 0x10 @0x800233F0). **Die Rueckverfolgung dieser Aufrufer bis
   zur Waffen-Id habe ich NICHT selbst gemacht**; die Zuordnung GL-Brand/Flammenwerfer
   stammt aus der Port-Dokumentation `enemy_ai_re2_zombie.c:3693-3709`, ist also Hinweis,
   nicht Beleg. Naechster Weg: umgebende Funktionen (RE2_Quellcode_V2/FUN_80020758.c,
   FUN_80021970.c, FUN_80021a48.c) gegen die Munitions-Records 0x800A9D10/1C/28 haengen.
2. **Welchen der beiden Faelle der Nutzer gesehen hat.** Der Sprung-Fall ist belegt und im
   Port zu frueh treffbar; der gewoehnliche Kugel-Treffer ist im Original NICHT
   pose-gesperrt. Ohne Messung am Original (DuckStation, `+0x1D3` des Hundes ueber die
   Hurt-Kette) kann ich das nicht entscheiden. Naechster Weg: `re15-room-capture` auf den
   Hunde-Flur, `+0x1D3` je Bild mitschreiben.
3. **Freigabe des Devour-Riegels** (`enemy_ai_re2_dog.c:566`, @0x80104F64-74): im
   Hunde-Overlay habe ich keinen Loescher dafuer gefunden. Im Original ist der Spieler dann
   tot — nicht gemessen.
4. **Kraehe/Spinne-Bilanz** aus Schritt 3 ist gezaehlt, nicht gemessen.
5. **Widerlegt, nicht offen**: `reai-v2-runde13-hunde-trefferpause` ("RE2 sperrt ueber ZEIT,
   NICHT ueber den Zustand" / "prueft die Hurt-Animation NIRGENDS") gilt nur fuer die
   low-7-Haelfte und ist als Aussage ueber +0x1D3 falsch — Beleg §2.1 (unmaskiertes Gate)
   und §2.7/§2.8 (Freigabe am Clip-Ende). `reai-v2-hit-latch-93` bleibt richtig: +0x93 ist
   der RE1.5-Riegel, RE2 kennt das Feld nicht (Voll-Scan Offset 147 selbst nachgefahren:
   0 Treffer in EMD0G_MOD0.BIN, 0 in info/re2leon/PSX.EXE).
