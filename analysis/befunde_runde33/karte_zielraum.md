# Runde 33 / Thema K — Karte: 2F nach dem Irons-Hinweis wählbar + Zielraum-Markierung

Arbeitsbaum `.claude/worktrees/r33_karte`, Zweig `r33/karte`. Laufend fortgeschrieben.

## Auftrag (wörtlich)

> Nach der Cutscene mit Irons und dem Anzeigen des Communication Room, wo man hin soll, muss man
> hinterher noch in der Lage sein bei der Map zu 2F zu wechseln, um den Raum zu sehen, auch wenn man
> noch nicht auf 2F war. Außerdem muss der Raum irgendwie angezeigt bleiben, wie bei Resident Evil 2
> bei Zielräumen auch.

## Stand

- [x] 1. Messen im Port (Etagenwahl, Darstellung, Datei:Zeile) — Abschnitt 1
- [x] 2. RE2 prüfen (Etagenwahl, Zielraum-Darstellung) — Abschnitt 2
- [x] 3. Bauen — Abschnitt 3 (Commit 2d14ec04)
- [x] 4. Riegel (probes/r33_karte.cmake), RE15_MIN_TESTS 416 -> 419 — Abschnitt 4
- [x] 5. Abnahme im echten Spiel (Framedumps) — Abschnitt 5

**Kurz:** RE2 macht am Hinweis kein Blatt waehlbar (Gatter Bank 35 = Blatt besucht, gesetzt beim
Raumeintritt und bei der Kartenaufnahme) und kennt keine Zielraum-Markierung in der normalen Karte
(Bank 32 ist eine Grundriss-Variante, keine Markierung). Beides ist deshalb Port-Wahl auf den
ausdruecklichen Nutzerwunsch, im Stil und Takt der RE2-Karte, abgeleitet aus Flag (3,94) und dem
Besucht-Bit des Funkraums — **Speicherstand unveraendert v9**.

## Protokoll

---

## 1. MESSUNG im Port (Ist-Zustand, Stand 53b69a1b)

Sonde `re15_port/tests/unit/test_r33_karte.c messen` (gebaut ueber `probes/r33_karte.cmake`).
Aufbau: Weg bis Irons' Buero OHNE 2F — alle Haupt-Zeilen (`etage == 0`) der Blaetter 2 (1F),
4 (3F) und 5 (Dach) besucht, ausser Orten mit einer Zeile auf Blatt 3; Spieler in ROOM1150;
dann der Hinweis so, wie das Spiel ihn zeigt (sub08 setzt (3,94) an seinem Anfang
@0x01110, Anker = Evt_end @0x012EC), START schliesst, danach Statusschirm + L1.
Ausgabe `karte_belege/messung_vorher.txt`:

```
[M0] Weg ohne 2F: 18 Haupt-Zeilen besucht (Blaetter 2/4/5); Ziel = Blatt 3 Rechteck 9
[M1] vor dem Hinweis: flag(3,94)=0  bekannt: 0:0 1:1 2:1 3:0 4:1 5:1 6:0 ...
[M2] nach dem Hinweis: flag(3,94)=1  offen=0  bekannt: 0:0 1:1 2:1 3:0 4:1 5:1 6:0 ...
[M2] Ziel Blatt 3 Rechteck 9: Zustand UNVISITED, Blatt im Besitz 0
[M3] normale Karte: substate 1, Blatt 4
[M3] RUNTER -> Blatt 2 (Ton ja)      <- 2F (Blatt 3) wird UEBERSPRUNGEN
[M3] RUNTER -> Blatt 1 (Ton ja)
[M3] HOCH   -> Blatt 2 (Ton ja)
[M3] HOCH   -> Blatt 4 (Ton ja)      <- und wieder uebersprungen
```

Befund: nach dem Hinweis ist 2F in der normalen Karte NICHT waehlbar, und selbst wenn es das
waere, zoege der Zeichner den Funkraum nicht (unbesucht + Blatt nicht im Besitz). Der Hinweis
hinterlaesst nichts (so gebaut in Runde 30, RE2-treu: `karte-3010.md` §3.7).

Woran es haengt (Datei:Zeile, Stand 53b69a1b):

| Stelle | Inhalt |
|---|---|
| `engine/src/menu_common.c:1537-1552` | HOCH/RUNTER in `map_mode` case 1: Reihenfolge `ORDER[13]`, naechstes Blatt nur `if (re15_map_page_known(ORDER[j]))` (Z. 1546), sonst weiter |
| `engine/src/re15_map_zones.c:919-934` | `re15_map_page_known`: ein Blatt ist bekannt, wenn eine Zone darauf besucht ist ODER ein Etagen-Bit einer Zeile auf ihm steht — sonst nichts |
| `engine/src/re15_inv_screen.c:2597-2598` | Kachel-Schleife: `rs == UNVISITED && !re15_map_owned_page(page) -> continue` (RE2 @0x8006E744) |
| `engine/src/re15_inv_screen.c:2570-2579` | die Zielkachel wird NUR im Hinweis-Schirm (`hint_aktiv`) gezeichnet |

---

## 2. RE2 (Retail, Leon) — Etagenwahl und Zielraeume

Alle Adressen `info/re2leon/PSX.EXE` (t_addr 0x80010000, Kopf 0x800), selbst disassembliert mit
`.claude/skills/re15-psx-disasm/scripts/re2_disasm.py`. Skript-Zensus ueber alle Leon-RDTs
(`info/re2leon/PL0/RDT`) mit `karte_werkzeug/r33_re2_bank_zensus.py` (Ausgabe
`karte_belege/re2_bank_zensus.txt`; Walker
`analysis/befunde_runde30/tools/r30_re2_scd.py`; 171 Bloecke desynchronisieren im Walker und
sind nicht erfasst — die Aussagen unten gelten fuer die sauber gelaufenen Bloecke, die vier
Hinweis-Bloecke gehoeren dazu).

### 2.1 Die Flag-Baenke der Karte (Bank-Zeigertabelle @0x800A78C8)

Die SCD-Befehle lesen ihre Bank ueber diese Tabelle: Ck 0x21 @0x80054354 (`lbu v1,1(v0)` Bank,
`lhu a1,2(v0)` Bit, `lw v1,30920(at)` = `[0x800A78C8 + Bank*4]` @0x80054384), Set 0x22
@0x800543B4 (`lbu v1,1(v0)` Bank, `lbu a1,2(v0)` Bit, `lbu a2,3(v0)` Op 0/1/7,
`lw v1,30920(at)` @0x800543E8). Tabelle selbst gelesen:

| Bank | Adresse | Bedeutung (belegt durch den Leser) |
|---|---|---|
| 9 | 0x800D490C | Raum besucht (Satzbyte +12; Zeichner @0x8006E67C-0x8006E688) |
| 31 | 0x800D4A34 | Gegenstands-/Uebergangsmarken erledigt (FUN_8006DCC0, Runde 30 §3.9) |
| 32 | 0x800D4920 | Kachel-VARIANTE je Raum (Satzbyte +13; @0x8006E604-0x8006E620, @0x8006E70C-0x8006E72C) |
| 33 | 0x800D4924 | Karte des Blatts im Besitz (Bit = `[0x800AAA3D + Blatt*8]`, @0x8006E658-0x8006E66C) |
| 35 | 0x800D4908 | **Blatt besucht = Etagenwahl-Gatter** (2.2) |

(Fruehere Dossiers nennen Bank 35 "Bank 36" und Bank 31 "Bank 32" — die Zeigertabelle zaehlt ab
0: `[0x800A7954]` = 0x800D4908 ist Index 35, `[0x800A7944]` = 0x800D4A34 ist Index 31.)

### 2.2 (a) Woran die Waehlbarkeit einer Etage haengt

Kartenschirm Zustand 3 (interaktiv), HOCH/RUNTER:

```
8006d8e0: lw   v0,0(a0)          ; a0 = 0x800CFB74, Bit 31 = Wiederhol-Takt
8006d8e8: bgez v0,0x8006d9e0     ; kein Takt -> kein Etagenwechsel
8006d8f0: lbu  a1,26(s0)         ; Blatt [0x800D5C0A]
8006d8f8: sltiu v0,a1,0x2 / bne  ; Blatt < 2: keine Etagen
8006d904: lhu  v0,8476(s1) / 8006d90c: andi v0,v0,0x1000   ; HOCH gehalten
8006d920: lbu  a1,-25876(at)     ; Nachbar oben  [0x800A9AEC + Blatt]
8006d928: sltiu v0,a1,0x2 / bne  ; kein Nachbar
8006d934: jal  0x80077360        ; Bit PRUEFEN ...
8006d938: addiu a0,a0,19860      ; ... in 0x800CFB74 + 0x4D94 = 0x800D4908 (Bank 35)
8006d93c: beq  v0,zero,...       ; Blatt nie besucht -> NICHTS
8006d958: sb   v0,26(s0)         ; Blatt := Nachbar
8006d964: andi v0,v0,0x4000      ; RUNTER: dasselbe mit [0x800A9B04 + Blatt]
8006d990: lui a0,0x800d / 8006d994: addiu a0,a0,18696   ; 0x800D4908
8006d998: jal  0x80077360 / 8006d9a0: beq v0,zero,...
8006d9bc: sb   v0,26(s0)
8006d9d4: lui  a0,0x404 / 8006d9d8: jal 0x8005ba28      ; Se(4,4) beim Wechsel
```

Dasselbe Gatter zeichnet die Pfeil-Reiter (`addiu a0,a0,18696` @0x8006E3A4) und steht im
zweiten Blaetterer (@0x8006EE28) — mit @0x8006D994 die EINZIGEN direkten Bezuege auf 0x800D4908
(`analysis/befunde_runde30/r30_mips_dis.py --find-addr 0x800d4908`).

**Wer setzt Bank 35?**

1. **Das Betreten eines Raums.** FUN_8006931C (einziger Aufrufer `jal` @0x8004A39C am Ende der
   Raum-Init FUN_80049E48, die ihrerseits nur aus dem Tuer-Uebergang @0x80026E1C gerufen wird):
   ```
   80069390: addiu a0,s0,238       ; 0x800D481E + 238 = 0x800D490C (Bank 9)
   80069394: jal 0x8007730c        ; Bit SETZEN: Raum besucht (Stage-Basis + Raum)
   800693a8: jal 0x8006e7f0        ; (Stage, Raum) -> Blatt des Raums
   800693b0: addiu a0,s0,234       ; 0x800D481E + 234 = 0x800D4908 (Bank 35)
   800693b4: jal 0x8007730c        ; Bit SETZEN: Blatt besucht
   ```
   (`0x8007730C` = Bit setzen, `or` @0x80077328; `0x80077360` = Bit pruefen, `and` @0x80077380.)
2. **Skripte**, 31 Set-Records (0 Ck). Das Muster ist die AUFNAHME EINER KARTE: derselbe Block
   setzt Besitz (Bank 33) UND Blatt-besucht (Bank 35) fuer dieselben Blaetter, z.B.
   ROOM20B0 sub10 `22 21 02 01`/`03`/`04` @0x037DA-E2 und `22 23 02 01`/`03`/`04` @0x037E6-EE
   (Polizeiwache 1F/2F/3F), ROOM2130 sub05 @0x01842/@0x01846 (Blatt 5), ROOM6120 sub03
   @0x01504-16 / @0x01522-34 (Blaetter 13-17). Weitere Setzer ohne Besitz: ROOM1120 sub03
   @0x02EDA (4), ROOM4010/4030/D010/D030 (7, 8), ROOM6030 sub03/04/10 (13, 14), ROOM60E0 sub06
   (16, 17), ROOMA120 sub01 (4).

**Setzt der Hinweis 0x84 oder die Szene davor etwas, das die Etage waehlbar macht? NEIN.**
- Handler 0x84 @0x800591C4-0x80059228 schreibt genau vier Dinge (Modus, Phase, Bit 0x8000,
  Hinweis-Nummer — `karte-3010.md` §3.2); der Hinweis-Modus hat 0 Bit-Setzer (13 x
  `jal 0x80077360`, 0 x `0x8007730C` im Bereich 0x8006F1C4-0x8006F900, §3.5 d).
- Die vier Szenenbloecke mit einem Hinweis-Record (ROOM3010 sub02 `84 02` @0x026EE, ROOM3040
  sub24 `84 04` @0x01BB0, ROOM30B0 sub15 `84 01` @0x01A86, ROOM6030 sub21 `84 03` @0x035C8)
  enthalten KEINEN Set auf Bank 35 (ROOM6030 setzt Bank 35 nur in sub03/04/10).
- Zwei dieser Bloecke bereiten die Karte aber vor: ROOM3040 sub24 gibt direkt vor `84 04` die
  Karten der Blaetter 2-8 (Set(33, 2..8) @0x01B90-A8) — der Zielraum steht danach in der
  normalen Karte als Umriss (Zeile 498, Besitz ohne Besuch); ROOM30B0 sub15 schaltet vor `84 01`
  die Kachel-Variante des Zielraums ein (Set(32,1) @0x01A60, 2.3).

Ergebnis (a): **in RE2 ist eine Etage genau dann waehlbar, wenn Bank 35 ihr Bit traegt** — gesetzt
beim Betreten eines Raums dieses Blatts oder per Skript (Kartenaufnahme). Der Hinweis selbst macht
KEIN Blatt waehlbar.

### 2.3 (b) Gibt es eine Darstellung von ZIELRAEUMEN in der normalen Karte? NEIN.

Der normale Zeichner FUN_8006E120 kennt je Kachel genau diese CLUT-Zeilen (Raumschleife
@0x8006E46C-0x8006E770, selbst disassembliert):

| Zeile | Bedingung | Stelle |
|---|---|---|
| 501 | Raum besucht (Bank 9, Satzbyte +12) | `addiu s5,zero,501` @0x8006E614 |
| 506 | besucht UND Bank-32-Bit (Satzbyte +13) | `addiu s5,zero,506` @0x8006E620 |
| +1 (502/507) | aktuelles Blatt UND aktueller Raum | `bne v0,a3` @0x8006E630, `bne s2,a3` @0x8006E640, `addiu s5,s5,1` @0x8006E648 |
| 498 | Karte im Besitz, unbesucht | `addiu s5,zero,498` @0x8006E71C |
| 503 | dito UND Bank-32-Bit | `addiu s5,zero,503` @0x8006E72C |
| — | ohne Karte und unbesucht: nicht gezeichnet | `beq v0,zero,0x8006e768` @0x8006E744 |
| 509/510, 506/510 | nur Blatt 2 Raum 14 (Sonderzweig mit Bank-32-Bits 8/9/11/12) | @0x8006E4A0-0x8006E600 |

Es gibt KEINEN Zweig "Ziel". Die einzige blinkende Raumdarstellung ist der Hinweis-Modus 4
(Zeichner FUN_8006F1C4, Aufrufer nur @0x8006F840/@0x8006F8E0), und der hinterlaesst nichts
(`karte-3010.md` §3.7). Die normale Karte spielt auch keinen Hinweis-Ton: in FUN_8006D650 und im
Zeichner 0x8006DEA0-0x8006E7F0 stehen genau drei `jal 0x8005ba28` — Se(4,9) @0x8006D7EC
(`lui a0,0x409` @0x8006D7DC), Se(4,4) @0x8006D9D8, Se(4,5) @0x8006D9FC; kein Se(2,0x2B).

**Was Bank 32 ist (sie ist der naheliegende Verdacht fuer "Zielraum").** Nur die 9 Raeume mit
Satzbyte +13 != 0 haben Kachel-Pixel mit Palettenindex >= 5 (Histogramm aller Kacheln der
Blaetter 2/3/5/6/8/16/17: jeder andere Raum 0 solche Pixel). Die Zeilen 503/506/507 machen genau
diese Indizes sichtbar (Eintraege 5-8 = Farben 1-4, in 498/501/502 durchsichtig) und faerben
11/13/14/15 um. Gerendert (`karte_werkzeug/r33_re2_kachel_varianten.py`,
`karte_belege/re2_b32_area*.png`, je besucht / besucht+b32 / unbesucht / unbesucht+b32):
Blatt 3 Raum 2 bekommt einen zusaetzlichen FLURARM nach oben, Blatt 5 Raum 8 eine zusaetzliche
TUER in der linken Wand. **Bank 32 = "die Kachel zeigt den veraenderten Grundriss"** (Durchgang
geoeffnet, Tuer freigelegt) — Setzer sind 26 Skript-Records (ROOM20E0, ROOM2160, ROOM3040,
ROOM30B0, ROOM4040, ROOM60B0/60C0/6160 u.a.), Leser nur die drei Zeichnerstellen
(@0x8006E608, @0x8006E710, @0x8006F5D0) und der Sonderzweig (s3 = 0x800D481E + 258). Farbe
und Blinken bleiben die gewoehnlichen.

Die 14 Marken von FUN_8006DCC0 (Tabelle @0x800A9B1C, Blinkhelligkeit nach [0x800D5C19]) sind
PUNKTE an festen Kartenlagen, keine Raeume; sie verschwinden, wenn ihr Bit in Bank 31 steht
(Runde 30 §3.9, `analysis/karte_2026-08-31/C_re2_karte.md` §3.2).

### 2.4 Folgerung fuer den Bau

- **Etagenwahl:** RE2 hat das Mittel, ein Blatt ohne Besuch waehlbar zu machen — ein Set auf
  Bank 35 aus dem Skript (Kartenaufnahme ROOM20B0 sub10 @0x037E6-EE). RE2 benutzt es am Hinweis
  NICHT. Der Nutzer will es dort ausdruecklich: der Port gibt deshalb mit dem Hinweis das Blatt
  des Zielraums frei — **Port-Wahl auf Nutzerwunsch**, gebaut als Gegenstueck zu RE2s
  Set(35, Blatt); das Gatter selbst (nur freigegebene Blaetter) bleibt RE2s Regel.
- **Zielraum-Markierung:** RE2 hat KEINE (2.3). Gebaut als **Port-Wahl auf ausdruecklichen
  Nutzerwunsch**, im Stil der RE2-Karte: derselbe Rot/Umriss-Wechsel wie im Hinweis (CLUT 502 /
  498, `addiu s2,s2,1` @0x8006F514 / `addiu s2,zero,498` @0x8006F5DC), getaktet wie RE2s
  Karten-Pulszaehler — der normale Kartenschirm faehrt in Zustand 3 denselben Zaehler wie der
  Hinweis-Zeichner (@0x8006D87C-0x8006D8D4: `sltiu v0,v0,0xa` @0x8006D894, `sb zero,41(s0)`
  @0x8006D8A0, `addiu v0,v0,-2` @0x8006D8AC, `sltiu v0,v0,0x51` @0x8006D8B8, `addiu v0,v0,2`
  @0x8006D8D0; s0 = 0x800D5BF0, +40 = Zaehler 0x800D5C18, +41 = Richtung 0x800D5C19), einmal je
  VBlank (Teiler 0 @0x80068A1C gilt fuer den ganzen Status-Task). OHNE Ton (2.3). Sie bleibt,
  bis der Zielraum besucht ist.


---

## 3. BAU (Zweig r33/karte, Commit 2d14ec04)

### 3.1 Verhalten

| | vorher | nachher |
|---|---|---|
| normale Karte nach dem Hinweis, 2F nie betreten | RUNTER 3F -> 1F, 2F uebersprungen | RUNTER 3F -> **2F** (Se(4,4)) -> 1F; HOCH 1F -> 2F -> 3F |
| Funkraum ROOM10F0 auf Blatt 3 | nicht gezeichnet (unbesucht, Blatt nicht im Besitz) | **blinkt** wie im Hinweis: CLUT 502 (dunkelrot) / 498 (nur Wandlinie), Phasen 39 VBlanks = 0,652 s, kein Ton |
| nach dem Betreten von ROOM10F0 (oder Elzas ROOM10F1) | — | Markierung aus; Kachel folgt der normalen Regel (AKTUELL im Raum, danach BESUCHT) |
| Karte oeffnet auf | Blatt des Spielers | unveraendert Blatt des Spielers (RE2 `jal 0x8006e7f0` @0x8006D6B8) |
| andere Blaetter | nur nach Besuch | unveraendert nur nach Besuch |
| Hinweis-Schirm selbst | Runde 30 | unveraendert (Riegel `unit_r30_hinweis_*` gruen) |

### 3.2 Dateien

| Datei | Aenderung |
|---|---|
| `engine/src/map_hint_common.c` | Tabelleneintrag + Szenen-Flag (3,94) (sub08 @0x01110, main00 @0x00DE6); `ziel_zone`; Abschnitt 4 mit allen RE2-Belegen; `re15_map_ziel_blatt_frei`, `re15_map_blatt_waehlbar`, `re15_map_ziel_aktiv`; eigener, stummer Zielkachel-Blinker `re15_map_ziel_blink_*` (Schritt = `zaehl_schritt`, RE2 @0x8006D87C-0x8006D8D4) |
| `include/re15_map_hint.h` | Deklarationen mit Belegblock |
| `engine/src/menu_common.c` | HOCH/RUNTER: `re15_map_blatt_waehlbar` statt `re15_map_page_known`; `menu_task_step`: je Bild `ziel_aktiv/ziel_rot/ziel_page/ziel_rect` (nur MAP-Unterschirm, nie im Hinweis), Blinker-Neustart je Kartenansicht; Abbau in `close_phase` und `re15_menu_toggle` |
| `include/re15_inv_screen.h` | vier Felder am Strukturende |
| `engine/src/re15_inv_screen.c` | Kachel-Schleife: Zielkachel-Zweig direkt hinter dem Hinweis-Zweig, gleiche Zeichnung (1. Durchgang, ohne Besuchs-/Besitz-Gatter) |

### 3.3 Port-Wahlen (keine Original-Adresse), begruendet

1. **Blatt freigeben am Hinweis.** RE2 hat das Mittel (Set auf Bank 35 per Skript, Kartenaufnahme
   ROOM20B0 sub10 @0x037E6-EE), benutzt es am Hinweis nicht. Nutzerwunsch. Frei wird NUR das Blatt
   der Hauptzeile des Zielraums; das Gatter bleibt fuer alles andere RE2s Besuchsregel.
2. **Zielkachel in der normalen Karte, bis der Ort besucht ist.** RE2 hat keinen Ziel-Zustand (2.3).
   Stil = der einzige Ziel-Stil, den RE2 hat (Hinweis-Zeichner, CLUT 502/498); Takt = RE2s
   Karten-Pulszaehler, den der normale Kartenschirm ohnehin faehrt (@0x8006D87C-0x8006D8D4); ohne
   Ton, weil RE2s normale Karte den Hinweis-Ton nie spielt. "Bis besucht" statt "fuer immer":
   ein besuchter Raum ist ohnehin gezeichnet (Zeile 501), die Markierung hat dann keinen Zweck mehr;
   so verschwindet sie genau dann, wenn der Auftrag des Hinweises erfuellt ist.
3. **Blinker-Start je Kartenansicht** mit den Hinweis-Startwerten (10/1): RE2s normaler Schirm
   setzt den geteilten Zaehler nicht neu (Schreiber in FUN_8006D650 nur @0x8006D8A0/C4/D4). Der
   Port hat keinen geteilten Zaehler; er beginnt jede Ansicht wie der Hinweis (rot ab Schritt 2).
4. **Kein Speicherfeld.** Abgeleitet aus Szenen-Flag (3,94) und Besucht-Bit des Zielorts, die
   beide schon im Stand liegen (g_game.flags seit v1, visited seit v6). Ein eigenes Feld waere eine
   zweite Wahrheit; seine Hebung fuer alte Staende muesste exakt diese zwei Bits lesen. (3,94) ist
   eindeutig: 2 Records in allen 240 RDTs, beide ROOM1150 (`karte_werkzeug/r33_re15_flag_zensus.py`:
   main00 @0x00DE6 Ck, sub08 @0x01110 Set). Zwischen Set am Szenenanfang und Hinweis am Szenenende
   ist kein Speichern moeglich (Szenen-Flags (2,7)/(1,27) ab @0x01114/@0x01118, der Spieler steht).

### 3.4 Speicherstand

**KEINE Formataenderung: Version bleibt 9, `sizeof(re15_savedata_t)` bleibt 944** (Riegel
`unit_r33_karte_speicher` haelt beides fest). Abweichung vom Auftrag ("Version anheben, alte Staende
heben"), weil kein neuer Zustand entsteht (3.3 Punkt 4). Folge fuer das Zusammenfuehren mit
`r33/speichern`: von diesem Zweig kommt KEINE Versionsnummer, kein Feld, keine Hebung; alte v9-Staende
nach der Irons-Szene zeigen die Markierung ohne weiteres (Flag gesetzt, Ziel unbesucht).

### 3.5 Elza

ROOM1151 hat die Szene nicht (Runde 30: 8 Subs, kein sub08, kein Ck(3,94)), kein Elza-Skript setzt
(3,94) (Zensus) — Elza bekommt weder Hinweis noch Freigabe noch Markierung (Riegel etage, letzter
Punkt). Betritt jemand Elzas Funkraum ROOM10F1, loescht das die Markierung ebenso (geteiltes
Besucht-Bit, `zone_bit` maskiert `room & ~1`; Riegel markierung, letzter Punkt).

---

## 4. RIEGEL (`tests/unit/test_r33_karte.c`, `tests/unit/probes/r33_karte.cmake`)

| Riegel | prueft |
|---|---|
| `unit_r33_karte_etage` | ohne Hinweis: RUNTER 3F -> 1F; nach dem Hinweis: Blatt 3 bekannt 0 / frei 1 / waehlbar 1, Besucht- und Etagen-Bits bitgleich, kein anderes Blatt frei, Karte oeffnet auf 4, RUNTER 4->3 mit Se(4,4), ->2, HOCH ->3, ->4; ohne (3,94) (Elza) nichts frei |
| `unit_r33_karte_markierung` | im Hinweis-Schirm ziel_aktiv nie gesetzt; Ziel 3/9 = (156,76) 48x40 uv (208,80), UNVISITED, nicht im Besitz; Blatt 4 unveraendert (Irons' Buero AKTUELL); 4 s auf Blatt 3: Kachel in jedem Bild, CLUT = ziel_rot ? 502 : 498, Wechsel bei Schritt 2+39k auf ein Bild genau (30, 60, 144 Bilder/s), kein Se(2,0x2B), keine CORE-Toene, kein fremdes AKTUELL; ROOM10F0 betreten -> aus, Kachel stetig AKTUELL; zurueck in 1150 -> BESUCHT; ROOM10F1 loescht ebenso |
| `unit_r33_karte_speicher` | Version 9 / 944 Byte; capture -> Zustand loeschen -> restore: Markierung + Freigabe zurueck, RUNTER -> 3 mit Zielkachel; nach dem Betreten gespeichert -> geladen: aus |

Messsonden im selben Programm: `messen` (Abschnitt 1; nachher `karte_belege/messung_nachher.txt`:
RUNTER 4 -> 3 mit Zielkachel CLUT 0x0012, -> 2, HOCH -> 3, -> 4) und `karte <pfad.mcr>`
(Spielstaende einer Speicherkarte).

### 4.1 Gegenproben (`karte_werkzeug/r33_karte_gegenprobe.sh`)

Je eine Mutation am Bau, Build, drei Riegel (Rueckgabe 0 = gruen, 1 = rot), Datei danach aus git:

```
M1 Freigabe weg      (blatt_waehlbar = nur bekannt):        etage=1 markierung=1 speicher=1
M2 Zielkachel weg    (Zeichner-Zweig aus):                  etage=0 markierung=1 speicher=1
M3 Hinweis-Ton       (Zielblinker spielt Se(2,0x2B)):       etage=0 markierung=1 speicher=0
M4 Besuch egal       (ziel_aktiv ohne Besucht-Test):        etage=0 markierung=1 speicher=1
unveraendert:                                               etage=0 markierung=0 speicher=0
```

---

## 5. ABNAHME an der echten exe (RE15_FRAMEDUMP, beschleunigter Renderer)

Werkzeug `karte_werkzeug/r33_karte_abnahme.sh` (Muster Runde 30), Lauf-exe als Kopie
`re15_pc_r33k.exe` (vorher: `re15_pc_r33k_vorher.exe`, gebaut aus 53b69a1b), Speicherkarte = KOPIE
von `re15_port/build/platform/pc/re15_card.mcr` des Hauptbaums. Deren Platz 0 ist genau der Fall des
Nutzers (Sonde `test_r33_karte karte`): `Raum 1150, flag(3,94)=0, Blatt3 bekannt=0`. Titel -> LOAD
GAME (Platz 0) -> Debug-JUMP 1150 in die AUTO-Zone (-20500,-22800) wie Runde 30 -> die Szene laeuft
von selbst -> Hinweis; Tasten per RE15_INPUT_SCRIPT auf der Spielbild-Achse. Kein AUTOSHOT, kein
SOFTWARE_RENDER. Bilder, Logs und Karten liegen in `build/r33_karte_abnahme/` (nicht versioniert);
Kontaktbogen `karte_belege/abnahme_kontaktbogen.jpg`, Einzelbilder `karte_belege/lauf_a_002250.jpg`
(Umriss) / `lauf_a_002270.jpg` (rot). Alle Bilder angesehen.

Kachel-Mittelwert der Zielkachel (156,76) 48x40 (x3 im 960x720-Bild), `r33_karte_kontaktbogen.py`:
rot = (46,12,56), Umriss = (4,20,94) — dieselben Werte wie der Hinweis in Runde 30 (49,13,55)/(4,20,94);
Hintergrund ohne Kachel (6,23,90).

| Lauf | Ablauf (Log) | Befund |
|---|---|---|
| vorher_a (alte exe) | Hinweis F1879, START F1999 schliesst, START F2098, L1 F2149 (se=4, Blatt 4), RUNTER F2230: **kein Ton, kein Blattwechsel** | Bild F2250: weiter "POLICE STATION 3F" — 2F unerreichbar (in diesem Stand ist auch 1F unbekannt) |
| lauf_a (neue exe) | Hinweis F1874 .. F1999 wie gehabt; L1 F2149 -> Blatt 4; RUNTER F2230 -> se=4, **Blatt 3**; RUNTER F2401 -> nichts (1F unbekannt); HOCH F2452 -> Blatt 4; HOCH F2503 -> Blatt 5 (Dach) | Bild F2240 rot / F2250-2260 Umriss / F2270-2280 rot / ... bis F2450 im 20-Bilder-Wechsel (0,65 s bei ~30 Bildern/s); auf Blatt 4 und 5 keine Zielkachel |
| lauf_b | wie lauf_a bis zum Schliessen des Hinweises, Speichern F2150 auf Platz 3 (`[save] saved (room 1150)`) | Sonde auf der Karte danach: `Platz 3: Raum 1150 flag(3,94)=1 Blatt3 bekannt=0 waehlbar=1 Markierung=1` |
| lauf_c | Platz 3 laden, KEIN Sprung; START F91, L1 F142 (Blatt 4), RUNTER F223 -> Blatt 3 | kein `[hint]` (Szene laeuft nicht erneut); Bild F250 Umriss, F260 rot — **ueberlebt Speichern/Laden** |
| lauf_d | Platz 3 laden, Debug-JUMP ROOM10F0 (F61), START F181, L1 F232 -> Blatt 3 (Spieler dort), Speichern F450 auf Platz 4 | Bilder F260-F380: Kachel stetig (46..48,13,56) = AKTUELL mit Spielermarker, **kein Blinken** |
| lauf_e | Platz 4 laden (in 10F0), Debug-JUMP 1150, L1 F232 (Blatt 4), RUNTER F313 -> Blatt 3 | Bilder F320-F400: Kachel stetig (18,55,49) = BESUCHT (gruen), **Markierung weg** |

Toene (lauf_a, `[se]`-Zeilen, RE15_SE_DEBUG=1): zwischen L1 (F2149) und dem Schliessen (F2566)
genau 4 x se=4 (Oeffnen der Karte + drei Blattwechsel), KEIN se=43; alle 4 Hinweis-Toene se=43 des
Laufs fallen in den Hinweis (F1874-F1999).

### 5.1 Suite

Voller Lauf 1 (`build_r33_suite1.log`): 417/419 — `integration_r30_cut_blitz` (Lauf B endete mit
exit 1 nach 250 Bildern) und `integration_r30_granate_laden` rot; beide einzeln wiederholt gruen,
`cut_blitz` zusaetzlich mit der alten exe (`re15_pc_r33k_vorher.exe`) gegengefahren: beide gruen,
bitgleiche Ausgabe (731 Bilder, 45 mit 3D, 2 Wechsel). Voller Lauf 2 (`build_r33_suite2.log`,
nach den Gegenproben, `local_build.sh all`): 417/419 — `integration_r30_cut_blitz` (diesmal Teil C,
exit 1) und `integration_r30_titel_puls` (Bilddauer im Titel bis 79,8 ms = Last); beide einzeln
wiederholt gruen (`ctest -R`, 2/2). Unter Last flatternde GUI-Tests (Memory
`reai-v2-gui-tests-flattern-bei-parallelen-agenten`, drei andere Agenten bauten parallel), in
keinem Fall derselbe Test zweimal an derselben Stelle, kein Bezug zur Karte. Die drei neuen Riegel
und die fuenf `unit_r30_hinweis_*` waren in beiden Laeufen gruen.

---

## 6. Offen / nicht gemessen

* Ton nicht hoerbar geprueft (SDL-Dummy); belegt sind die Abspiel-Aufrufe (`[se]`-Zeilen).
* 60/144 Bilder/s nur im Riegel (Wanduhr-Zeiten gleich), an der echten exe nur ~30 Bilder/s.
* Kein gdigrab am Fenster; Bildbeweis ist RE15_FRAMEDUMP (komponierter Frame vor dem Present).
* PSX-/Android-Bau nicht gefahren (keine neue Quelldatei; nur vorhandene engine/src-Dateien geaendert).
* Leeres Blatt: wer 2F nie betreten hat, sieht auf 2F NUR die Zielkachel (wie im Hinweis, Runde 30
  R2). Ohne Karte zeigt RE2 unbesuchte Raeume ebenfalls nicht (@0x8006E744).
* Die Tuer des Funkraums braucht weiter die Blue Keycard (Runde 30 R3) — die Markierung zeigt auf
  einen zunaechst verschlossenen Raum; unveraendert.
* Die 171 im RE2-Walker desynchronisierten Skriptbloecke sind im Bank-Zensus nicht enthalten
  (die vier Hinweis-Bloecke laufen sauber); fuer Aussage 2.2 (Hinweis setzt Bank 35 nicht) genuegt
  das, fuer eine Vollzaehlung aller Bank-35-Setzer nicht.
* Die Bedeutung der RE2-Bank-31-Marken (14 Punkte, FUN_8006DCC0) ist nicht aufgeloest; fuer den
  Auftrag ohne Belang (keine Raeume).
