# RE2-Retail Pre-Intro: state2 / state3 / state4 (Exit) + Skip-Pfad

Status: IN ARBEIT (inkrementell)
Datum: 2026-08-30
Auftrag: Teilauftrag zu `re2-sequenzer.md` — die restlichen STATES und der Ausstieg.

Quelle: `info/re2leon/COMMON/BIN/OPENING.BIN` (13524 B), Ladebasis `0x801bfa18`,
`datei_offset = addr - 0x801bfa18`. Alle Adressen unten sind Overlay-RAM-Adressen.
Disassembliert mit eigenem MIPS-R3000-Dekoder
(`scratchpad/opdis.py`, reused `re2_disasm.dis_one`), verifiziert gegen die im
Hauptbericht belegte Stelle: `0x801bfb1c: sltiu v0,v0,0x191` ✅ exakt reproduziert.

## Arbeitslog
- [x] Dekoder aufsetzen + gegen 0x801bfb1c verifiziert
- [x] Register-Konstanten des Haupt-Ticks
- [x] state4 @0x801bfcfc (Exit/Halte-Gate)
- [x] state2 @0x801c0d58 — 12 Phasen, Tabelle @0x801bfa6c
- [x] state3 @0x801c1304 — 15 Phasen, Tabelle @0x801bfa9c
- [x] mode3 @0x801c0388 (Teardown / Rückgabe an die EXE)
- [ ] Skip-Pfad im Detail
- [ ] Bildschirm-Fade: fade_prim 0x801c1fe0 + mode2 0x801c051c + Renderer 0x801c1a0c

---

## 0. ⛔ KORREKTUR am Hauptbericht: drei Adressen sind dort um +0x10000 falsch

Der Hauptbericht rechnet bei `lui`+NEGATIVEM Displacement falsch (der Borrow fehlt).
Belegt am Prolog des Haupt-Ticks:

```
801bfaf8: lui   s4,0x800d
801bfafc: addiu s4,s4,-7420        ; 0x800d0000 - 0x1cfc = 0x800CE304
```

| Hauptbericht | korrekt | Beleg |
|---|---|---|
| Pad `0x800de304` | **`0x800ce304`** | `lui s4,0x800d; addiu s4,s4,-7420` @0x801bfaf8 |
| `DAT_800ebb70` | **`0x800dbb70`** | `lui v0,0x800e; lbu v0,-17552(v0)` @0x801bfd58 (0x800E0000−0x4490) |
| `DAT_800dfbd8` | **`0x800cfbd8`** | `lw v0,6356(s4)` @0x801bfb64, s4=0x800ce304 (+0x18d4) |

(`0x800cfbd8` wird auch in Exit-Phase 6 benutzt — dort steht im Hauptbericht `DAT_800dfbd8`.)

## 1. Register-Konstanten des Haupt-Ticks (Entry 0x801bfad8)

```
801bfaf8/fc: s4 = 0x800ce304   (Pad-Basis, u16 @+0)
801bfb00:    s3 = 1
801bfb04:    s2 = 2
801bfb08:    s1 = 3
801bfb0c:    s0 = 4
```
Diese fünf Konstanten erklären jedes `sb s3,…` / `sh s0,…` im Tick-Code.

Tick-Schwanz (@0x801bfca0..0x801bfcd4):
```
801bfca0..b4: frame = frame + 1
801bfcb8/bc:  bne frame, -1  →  sonst frame := 401   ; Überlauf-Klemme (401 = 0x191)
801bfcc8/cc:  jal 0x80031f94(1)                      ; VSync/Frame-Service
801bfcd0:     j 0x801bfb10                           ; ENDLOSSCHLEIFE — der Tick verlässt
                                                     ; sich NIE selbst; nur ein Moduswechsel
                                                     ; (mode3) bricht heraus.
```
Der Epilog @0x801bfcd8..0x801bfcf8 ist toter Code (nur per Sprung aus mode3 erreichbar).

---

## 2. state4 @0x801bfcfc — das EXIT-/HALTE-Gate (VOLLSTÄNDIG, 51 Instruktionen)

```
801bfcfc: addiu sp,sp,-24
801bfd00/04: v1 = *(0x801c2ed8)              ; Element-Array
801bfd08: a0 = 0                             ; i = 0
        ; --- Schleife: alle „fertig"-Elemente hart abschalten -------------
801bfd10: lbu v0,1(v1)                       ; elem[i].byte1
801bfd18: andi v0,v0,0x10                    ; „fertig"-Bit
801bfd1c: beq  v0,zero,0x801bfd28
801bfd24: sb   zero,0(v1)                    ; elem[i].byte0 := 0   (Element AUS)
801bfd28: a0++
801bfd30: lh v0, 0x801c24cc                  ; Element-Anzahl
801bfd38/3c: sltu a0,v0 → Schleife
801bfd40: v1 += 12                           ; 12-Byte-Records (bestätigt)
        ; --- Gate: nur wenn skip_state == 1 -------------------------------
801bfd48: lbu v1, 0x801c24d4                 ; skip_state
801bfd4c: v0 = 1
801bfd50: bne v1,v0, ENDE
801bfd5c: lbu v0, 0x800dbb70                 ; EXE-Flag (CD/Streaming-Busy)
801bfd64: sw zero, 0x801c24c4                ; ⇒ frame := 0  JEDEN Frame, solange
                                             ;   state4 hält (sperrt neuen Skip,
                                             ;   weil der Skip frame>=401 verlangt)
801bfd68: bne v0,zero, ENDE                  ; warte bis 0x800dbb70 == 0
801bfd70: jal 0x8002c350(0)                  ; xa_done(0) — Vorzeichen-Bit des XA-Streams
801bfd78: beq v0,zero, ENDE                  ; warte bis XA fertig
        ; --- Abschluss ----------------------------------------------------
801bfd84: sw zero, 0x801c2ee8                ; Element-Maske := 0
801bfd88: jal 0x801c1f64(a0=0, a1=1)         ; elem_set(0,1) — ALLE Elemente AUS
801bfd94: v1 = *(u16*)0x801c24c8             ; state_saved
801bfd98: v0 = 2
801bfda0: sb v0, 0x801c24d4                  ; skip_state := 2
801bfda4: v0 = 90
801bfdac: sh v0, 0x801c24d0                  ; ⇒ TIMER := 90   ⛔ Schlüssel-Konstante
801bfdb4: sh v1, 0x801c24d2                  ; STATE := state_saved  (Rücksprung!)
801bfdb8..c4: return
```

**Interpretation (byte-true):** `state4` ist **kein** eigener Fade-Zeichner, sondern das
**Warte-Gate nach dem Fade**. Es tut drei Dinge:

1. Räumt jedes Element ab, dessen `byte1 & 0x10` („Anim fertig") gesetzt ist — d. h.
   während state4 läuft, laufen die Ausblend-Animationen der Elemente noch aus und
   werden einzeln stillgelegt, sobald sie durch sind.
2. Hält, bis **beide** Bedingungen erfüllt sind: EXE-Flag `0x800dbb70 == 0`
   **und** `xa_done(0)` liefert das gesetzte Vorzeichen-Bit (Erzähler-Audio zu Ende).
   Solange gehalten wird, wird `frame := 0` gesetzt — ein zweiter START-Druck ist
   damit gesperrt (Skip verlangt `frame >= 401`).
3. Gibt dann an den **vorherigen** STATE zurück (`STATE := state_saved`) mit
   `timer := 90` — der Phasen-Zähler startet also NICHT bei 0, sondern bei 90.
   Da die Exit-Phase 6 auf `t == 100` prüft, bleiben exakt **10 Frames** bis zum
   CD-Request. Das ist die byte-true „Restlaufzeit" nach einem Skip.

`skip_state`-Zustandsmaschine (belegt):
| Wert | gesetzt von | Bedeutung |
|---|---|---|
| 0 | Init | kein Skip |
| 1 | Haupt-Tick @0x801bfb6c (`sb s3`) | Skip gedrückt, Fade läuft, state4 wartet |
| 2 | state4 @0x801bfda0 (`sb v0,…`) | Fade+XA fertig, Exit-Phase läuft |
| 3 | Haupt-Tick @0x801bfc2c (`sb s1`) | zweiter START während skip_state==2 → HART raus |

---

## 3. Die drei Phasen-Tabellen im Overlay-Header (roh verifiziert)

Alle drei Sprungtabellen liegen im 0xC0-Byte-Header von OPENING.BIN, bereits relokiert:

| Tabelle | Adresse | file-Offset | Einträge | Gate im Code |
|---|---|---|---|---|
| state0/1 | 0x801bfa3c | 0x24 | 11 (0..10) | `sltiu v0,v1,0xb` |
| — Null-Trenner — | 0x801bfa68 | 0x50 | `0x00000000` | |
| **state2** | **0x801bfa6c** | 0x54 | **12 (0..11)** | `sltiu v0,v1,0xc` @0x801c0d64 |
| **state3** | **0x801bfa9c** | 0x84 | **15 (0..14)** | `sltiu v0,v1,0xf` @0x801c1310 |

Die state3-Tabelle endet exakt bei 0x801bfad4; 0x801bfad8 ist der Entry-Prolog —
die Tabellen füllen den Header lückenlos aus.

**Timer-Semantik (wichtig für jede Zahl unten):** der Phasen-Timer wird am ENDE
jedes State-Handlers inkrementiert (state2 @0x801c12e8, state3 @0x801c19f0,
`addiu v0,v0,1; sh v0,0x801c24d0`). Ein Phasenwechsel setzt `timer := 0` und
läuft dann noch durch den Tail ⇒ **der erste Frame einer neuen Phase sieht `t == 1`.**
Deshalb sind die vielen `t==1`-Prüfungen „sofort beim Phasenantritt".

---

## 4. state2 @0x801c0d58 — 12 Phasen

Struktur: ausschließlich **Element-PAARE**, ausschließlich Zustand **7 (einblenden)**
und **11 (ausblenden)** — KEINE Sonder-Codes (23/27/39/43/71/75 wie in state0/1).
state2 nutzt genau **12 Elemente** (6 Paare; Masken 3/12/48/192/768/3072).

| Phase | Handler | t | Aktion (mit Adresse) |
|---|---|---|---|
| **0** | 0x801c0d8c | sofort | `fade_prim(3, 0, 0x00ffffff /*WEISS*/, 0)` @0x801c0d9c · `seq_ctl(0x01000000)` @0x801c0da4 · `*0x800d5b50 := 107` (BGM-Id) @0x801c0db4 · `PHASE:=1` @0x801c0dc0 · `timer:=0` @0x801c0dcc · `*0x801c24c0 := 3` @0x801c0dd4 |
| **1** | 0x801c0de0 | 240 | `xa_cue(1, 6)` @0x801c0df8 |
| | | 250 | Maske `3` = {0,1} → **7** @0x801c0e20 |
| | | 523 | Maske `3` → **11** @0x801c0e48 → PHASE 2 |
| **2** | 0x801c0e58 | 1 | `xa_cue(1, 7)` @0x801c0e70 |
| | | 11 | Maske `12` = {2,3} → **7** @0x801c0e98 |
| | | 190 | `xa_cue(1, 8)` @0x801c0eb8 |
| | | 388 | Maske `12` → **11** @0x801c0ee0 · **BILD 1**: `memcpy(0x80198000 ← 0x8011a000, 0x25800)` @0x801c0efc · `mode0_vram_kick()` @0x801c0f04 → PHASE 3 |
| **3** | 0x801c0f14 | 128 | `xa_cue(1, 9)` @0x801c0f2c |
| | | 138 | Maske `48` = {4,5} → **7** @0x801c0f54 |
| | | 336 | Maske `48` → **11** @0x801c0f80 |
| | | 396 | `mode2()` @0x801c0f9c · `*0x801c2ee4 := 8` @0x801c0fac → PHASE 4 |
| **4** | 0x801c0fb8 | 60 | `xa_cue(1, 10)` @0x801c0fd0 |
| | | 70 | Maske `192` = {6,7} → **7** @0x801c0ff8 |
| | | 274 | Maske `192` → **11** @0x801c1020 → PHASE 5 |
| **5** | 0x801c1030 | 1 | `xa_cue(1, 11)` @0x801c1048 |
| | | 11 | Maske `768` = {8,9} → **7** @0x801c1070 |
| | | 253 | Maske `768` → **11** @0x801c1098 · **BILD 2**: `memcpy(0x80198000 ← 0x8013f814, 0x25800)` @0x801c10b4 · `mode0_vram_kick()` @0x801c10bc → PHASE 6 |
| **6** | 0x801c10e0 | 128 | `xa_cue(1, 12)` @0x801c10f8 |
| | | 138 | Maske `3072` = {10,11} → **7** @0x801c1120 |
| | | 307 | Maske `3072` → **11** @0x801c114c |
| | | **517** | **AUSSTIEGS-SALVE**: `bgm_fade(120, 24)` @0x801c1168 · `*0x801c2ee4 := 8` @0x801c1178 · `*0x801c24c0 := 0` @0x801c1180 · `mode2()` @0x801c1184 · `skip_gate := 0` @0x801c1194 · `skip_state := 2` @0x801c119c |
| | | **≥518** | `sltiu v0,timer,0x206` @0x801c11ac (=518): sobald `xa_done(*0x801c24c0)` @0x801c11c0 wahr → Maske:=0 @0x801c11d4 · `elem_set(0,1)` @0x801c11d8 (ALLE aus) · `skip_gate := 1` @0x801c11e8 → PHASE **7** |
| **7** | 0x801c11f4 | 100 | nur wenn `*0x800dbb70 == 0` @0x801c120c: `*0x800cfbd8 &= ~0x10` @0x801c1234 · `cd_request(3)` @0x801c1230 → PHASE 8 |
| **8** | 0x801c1240 | — | warte bis `(*0x800cfb74 & 0x200) == 0` @0x801c124c → PHASE 9 |
| **9** | 0x801c1260 | 1 | `cd_request(16)` @0x801c1274 (a0 = **16 = 0x10**, `addiu a0,zero,16` @0x801c1278) → PHASE 10 |
| **10** | 0x801c1284 | — | warte bis `(*0x800cfb74 & 0x200) == 0` @0x801c1290 → PHASE 11 |
| **11** | 0x801c12b4 | 10 | `MODE := 3` @0x801c12cc · `STATE := 4` @0x801c12d8 |

Gemeinsame Phasenwechsel-Tails: 0x801c10c8 (`timer:=0; PHASE:=v0`) für Phasen 1..5,
0x801c129c (`PHASE:=v0; timer:=0`) für Phasen 6..10.

**Nominelle Länge state2 (ohne XA-Wartezeit):**
523 + 388 + 396 + 274 + 253 + 517 = **2351 Frames** (≈78 s bei 30 fps)
+ XA-Nachlauf + Exit-Kette (100 + CD-Wartezeit + 1 + CD-Wartezeit + 10).

---

## 5. state3 @0x801c1304 — das LANGE Kapitel (15 Phasen, 18 Elemente)

Wieder ausschließlich Zustände **7/11** auf Element-PAARE; **9 Paare**
(Masken 3, 12, 48, 192, 768, 3072, 12288, 0xc000, 0x30000 = Elemente 0..17).

| Phase | Handler | t | Aktion (mit Adresse) |
|---|---|---|---|
| **0** | 0x801c1338 | sofort | `fade_prim(3, 0, 0x00ffffff /*WEISS*/, 0)` @0x801c1348 · `seq_ctl(0x01000000)` @0x801c1350 · `*0x800d5b50 := 107` @0x801c1360 · `PHASE:=1` @0x801c136c · `timer:=0` @0x801c1378 · `*0x801c24c0 := 3` @0x801c1380 |
| **1** | 0x801c138c | 240 | `xa_cue(1, 6)` @0x801c13a4 |
| | | 250 | Maske `3` {0,1} → **7** @0x801c13cc |
| | | 523 | Maske `3` → **11** @0x801c13f4 → PHASE 2 |
| **2** | 0x801c1404 | 1 | `xa_cue(1, 7)` @0x801c141c |
| | | 11 | Maske `12` {2,3} → **7** @0x801c1444 |
| | | 438 | Maske `12` → **11** @0x801c146c → PHASE 3 |
| **3** | 0x801c147c | 10 | `xa_cue(1, 8)` @0x801c1494 |
| | | 20 | Maske `48` {4,5} → **7** @0x801c14bc |
| | | 140 | Maske `48` → **11** @0x801c14e4 → PHASE 4 |
| **4** | 0x801c14f4 | 1 | `xa_cue(1, 9)` @0x801c150c |
| | | 11 | Maske `192` {6,7} → **7** @0x801c1534 |
| | | 322 | Maske `192` → **11** @0x801c155c · **BILD 1**: `memcpy(0x80198000 ← 0x8011a000, 0x25800)` @0x801c1578 · `mode0_vram_kick()` @0x801c1580 → PHASE 5 |
| **5** | 0x801c1590 | 128 | `xa_cue(1, **14**)` @0x801c15a8 — der einzige Cue 14 im Overlay |
| | | 337 | `mode2()` @0x801c15c4 · `*0x801c2ee4 := 8` @0x801c15d4 → PHASE 6 |
| **6** | 0x801c15e0 | 90 | `xa_cue(1, 10)` @0x801c15f8 |
| | | 100 | Maske `768` {8,9} → **7** @0x801c1620 |
| | | 336 | Maske `768` → **11** @0x801c1648 → PHASE 7 |
| **7** | 0x801c1658 | 1 | `xa_cue(1, 11)` @0x801c1670 |
| | | 11 | Maske `3072` {10,11} → **7** @0x801c1698 |
| | | 266 | Maske `3072` → **11** @0x801c16c0 → PHASE 8 |
| **8** | 0x801c16d0 | 1 | `xa_cue(1, 12)` @0x801c16e8 |
| | | 11 | Maske `12288` {12,13} → **7** @0x801c1710 |
| | | 441 | Maske `12288` → **11** @0x801c173c · **BILD 2**: `memcpy(0x80198000 ← 0x8013f814, 0x25800)` @0x801c1758 · `mode0_vram_kick()` @0x801c1760 |
| | | 451 | Maske `0xc000` {14,15} → **7** @0x801c1788 (`ori v0,zero,0xc000` @0x801c1778) |
| | | 738 | Maske `0xc000` → **11** @0x801c17b4 |
| | | 768 | → PHASE 9 |
| **9** | 0x801c17e8 | 1 | `xa_cue(1, 13)` @0x801c1800 |
| | | 11 | Maske `0x30000` {16,17} → **7** @0x801c1828 (`lui v0,0x3` @0x801c1818) |
| | | 385 | Maske `0x30000` → **11** @0x801c1854 |
| | | **595** | **AUSSTIEGS-SALVE**: `bgm_fade(120, 24)` @0x801c1870 · `*0x801c2ee4 := 8` @0x801c1880 · `*0x801c24c0 := 0` @0x801c1888 · `mode2()` @0x801c188c · `skip_gate := 0` @0x801c189c · `skip_state := 2` @0x801c18a4 |
| | | **≥596** | `sltiu v0,timer,0x254` @0x801c18b4 (=596): sobald `xa_done(*0x801c24c0)` @0x801c18c8 → Maske:=0 · `elem_set(0,1)` @0x801c18e0 · `skip_gate := 1` @0x801c18f0 → PHASE **10** |
| **10** | 0x801c18fc | 100 | nur wenn `*0x800dbb70 == 0` @0x801c1914: `*0x800cfbd8 &= ~0x10` @0x801c193c · `cd_request(3)` @0x801c1938 → PHASE 11 |
| **11** | 0x801c1948 | — | warte `(*0x800cfb74 & 0x200) == 0` @0x801c1954 → PHASE 12 |
| **12** | 0x801c1968 | 1 | `cd_request(16)` @0x801c197c (a0=16 @0x801c1980) → PHASE 13 |
| **13** | 0x801c198c | — | warte `(*0x800cfb74 & 0x200) == 0` @0x801c1998 → PHASE 14 |
| **14** | 0x801c19bc | 10 | `MODE := 3` @0x801c19d4 · `STATE := 4` @0x801c19e0 |

Gemeinsame Tails: 0x801c17d0 (Phasen 1..8), 0x801c19a4 (Phasen 9..13).

**Nominelle Länge state3:** 523+438+140+322+337+336+266+768+595
= **3725 Frames** (≈124 s bei 30 fps) + XA-Nachlauf + Exit-Kette.

---

## 6. Struktur-Erkenntnis: state2/3 sind die REINEN Bild+Text-Kapitel

state0/1 (Hauptbericht §3.5) benutzt Sonder-Zustände 23/27/39/43/71/75 und
ungerade Element-Gruppen ({2..5}, {11..14}, {20..23}) — das ist das
Logo-/Effekt-Kapitel. **state2 und state3 sind dagegen streng regelmäßig:**

- ausschließlich Element-**Paare** (Maske = zwei benachbarte Bits)
- ausschließlich Zustand **7 (einblenden)** → Haltezeit → **11 (ausblenden)**
- pro Paar genau **ein** XA-Cue, **10 Frames VOR** dem Einblenden gestartet.
  Belegt: state2 Ph1 240/250, Ph2 1/11, Ph3 128/138, Ph4 60/70, Ph5 1/11,
  Ph6 128/138; state3 Ph2 1/11, Ph3 10/20, Ph4 1/11, Ph6 90/100, Ph7 1/11,
  Ph8 1/11, Ph9 1/11. ⇒ **10 Frames Audio-Vorlauf ist die feste Regel.**
- die Ausblend-Anweisung steht **direkt vor** dem Phasenwechsel, ohne Nachlauf;
  die Ausblend-ANIMATION läuft danach im Renderer weiter (state4 räumt das
  Element ab, sobald `byte1 & 0x10` gesetzt ist — §2).

**Sichtbare Haltezeiten (Einblenden → Ausblenden), Frames:**
| Kapitel | Paar 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9 |
|---|---|---|---|---|---|---|---|---|---|
| state2 | 273 | 377 | 198 | 204 | 242 | 169 | — | — | — |
| state3 | 273 | 427 | 120 | 311 | 236 | 255 | 430 | 287 | 374 |

(state2 Paar2 = 11→388; state3 Paar2 = 11→438 usw. — jeweils aus der Tabelle oben.)

**XA-Cue-Nummern (Erzähler-Audiospuren), belegt:**
state0/1 benutzt 1..5 (Hauptbericht), state2 benutzt **6,7,8,9,10,11,12**,
state3 benutzt **6,7,8,9,14,10,11,12,13**. Cue **14** gibt es NUR in state3
Phase 5 (@0x801c15ac) — der Phase ohne jedes Element, also eine reine
Audio-/Bild-Pause zwischen zwei Blöcken.

**Bild-Wechsel (identisch in state0/1, state2 und state3):**
genau ZWEI Vollbild-Wechsel je Kapitel, immer
`memcpy(dst=0x80198000, src, len=0x25800)` + `mode0_vram_kick()`:
- Bild 1: `src = 0x8011a000` (state2 Ph2 t=388 · state3 Ph4 t=322)
- Bild 2: `src = 0x8013f814` (state2 Ph5 t=253 · state3 Ph8 t=441)

`0x25800 = 153600 = 320·240·2` — exakt die Größe von `OPEN10..14.TIM`.
`0x8013f814 − 0x8011a000 = 0x25814 = 0x25800 + 0x14` ⇒ die Vollbilder liegen als
Block hintereinander im CD-Staging-Puffer, je mit 0x14 Byte TIM-Header.
⛔ Der Bildwechsel fällt IMMER mit einem `elem_set(11)` (Ausblenden) zusammen —
das Bild wird also NIE sichtbar hart umgeschaltet, sondern hinter dem
gerade ausblendenden Textpaar.

---

## 7. mode3 @0x801c0388 — der TEARDOWN (wie es zum nächsten Game-Mode geht)

```
801c0394..a0: do { frame_service(1); } while (xa_done(*0x801c24c0) == 0)   ; auf XA-Ende warten
801c03cc:  if (*0x800cfbd8 & 0x20)  jal 0x80012c2c                ; Sound-Sonderfall
801c03e4:  *(u16*)(sp+16) = 0xffff ; *(0x800dfd64) = &sp[16]      ; „kein Folge-BGM"-Sentinel
801c03f4:  jal 0x8005a97c        ; SEQ/VAB schliessen (SsSeqClose/SsVabClose; liest *DAT_800dfd64
                                 ;  und gibt 0xffffffff zurueck — FUN_8005a97c.c)
801c03fc:  jal 0x80059634
801c0418:  jal 0x8002bda8(a0=2, a1=0)   ; Fade-Prim-Array zuruecksetzen: DAT_800dfae0[i*0x98]
                                        ;  +0x8c=0, +0x90=0, +0x94=0x00f00140 (320x240), i=0..1
                                        ;  (FUN_8002bda8.c)
801c040c/18: *0x800cfbd8 &= ~0x10
801c0420:  jal 0x8002c324(0)  ;  801c0428: jal 0x8002c324(3)   ; XA-Slot 0 und 3 := 0xffff (FUN_8002c324.c)
801c0438:  *(u16*)0x800dfd00 = -1
801c0444:  *(u16*)0x800dfd02 = 0
801c044c:  *(u32*)0x800d5b50 = 120         ; ⇒ naechste BGM-Id = 120
801c0450:  jal 0x8003210c(0)               ; DAT_800d76a4[0] &= ~0x40 (FUN_8003210c.c)
801c0458:  jal 0x80031fe4                  ; *DAT_800d7824 = 0; CloseTh(...); ChangeTh(0xff000000)
                                           ;  (FUN_80031fe4.c)
```

**⛔ Kern-Antwort:** Der Sequenzer schaltet **nicht** über eine „nächster Modus"-Variable.
`0x80031fe4` schließt den Frame-Service-Thread und macht `ChangeTh(0xff000000)` —
der Kontext wechselt zurück in den EXE-Hauptthread. Die Endlosschleife des Ticks
(`j 0x801bfb10` @0x801bfcd0) wird nie regulär verlassen; der Epilog
@0x801bfcd8..0x801bfcf8 ist toter Code. Welcher Game-Mode danach läuft, entscheidet
die EXE-Hauptschleife (Dispatch-Tabelle @0x800a20bc), nicht das Overlay.
Das Overlay hinterlässt: BGM-Id **120** @0x800d5b50, Fade-Prims zurückgesetzt,
XA-Slots frei, `*0x800cfbd8 &= ~0x10`, `*0x800dfd00 = -1`, `*0x800dfd02 = 0`.
