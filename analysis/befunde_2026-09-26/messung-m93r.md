# Messung: M93R laedt nicht automatisch nach — die Ursache steht

Arbeitsbaum: `.claude/worktrees/wf_19e49a69-d81-3`
Sonde: `re15_port/tests/unit/probe_m93r_nachladen.c` (+ Registrierung in
`re15_port/tests/unit/CMakeLists.txt`), Protokoll `out_m93r.log`.
Messhaken (NUR LESEND, **Messaenderung, nicht der Fix**):
`re15_port/engine/src/player_common.c` — `int re15_player_aim_melee_dbg(void)`
direkt hinter `re15_player_aim_phase_debug()`.

---

## 0. Ergebnis in einem Satz

Der Port laedt die M93R im sauberen Weg **sehr wohl** nach (gemessen, Lauf A/C) —
er verschluckt das Nachladen **stumm**, sobald der Zieleintritt vorher mit dem
**Messer** stattgefunden hat: `re15_player_reload_start()` bricht an
`player_common.c:340` an `s_aim_melee` ab. Diese Bedingung hat im Original
**kein Gegenstueck**; dort trennt der Dispatch die Klassen, und der liest die
angelegte Waffe **jedes Bild frisch** aus `0x800aca5d` (@0x80032e60).
Der Port **latcht** sie einmalig beim Zieleintritt (`player_common.c:916`).

Gemessen (Lauf D2, 312 Bilder): mit gesetztem Latch **feuert** die M93R voellig
normal (15 -> 12 -> 9 -> 6 -> 3 -> 0, Burst zu 3), und danach passiert **nichts** —
kein Nachladen, kein Leer-Klick, die Munitionsschachtel bleibt bei 50.
Das ist woertlich das gemeldete Bild.

---

## (a) Original @0x80033300-94 — selbst disassembliert (info/Re1.5/PSX.EXE)

`python .claude/skills/re15-psx-disasm/scripts/re15_disasm.py dis 0x80033300 40`

```
  80033300: lw    v0,-14488(v0)      ; 0x800ac768 Pad HALTEN
  80033304: nop
  80033308: andi  v0,v0,0x40
  8003330c: beq   v0,zero,0x80033394
  80033314: jal   0x8004ea6c         ; Magazin > 0 ?
  8003331c: beq   v0,zero,0x80033334 ; leer -> Leer-Zweig
  80033320: ori   v0,zero,0x2        ; (Delay)
  80033328: sh    v0,-13734(at)      ; 0x800aca5a := 2 (SCHUSS)
  8003332c: j     0x80033450
  ; --- LEER ---
  80033338: lw    v0,-14484(v0)      ; 0x800ac76c Pad-FLANKE
  80033340: andi  v0,v0,0x40
  80033344: beq   v0,zero,0x80033394 ; nur gehalten -> NICHTS
  8003334c: jal   0x8004eb70         ; Reserve vorhanden ?
  80033354: beq   v0,zero,0x80033384
  80033358: lui   a0,0x101           ; (Delay) SE 0x01010001
  80033360: lbu   v0,-13731(v0)      ; 0x800aca5d angelegte Waffe
  80033368: sltiu v0,v0,0x9
  8003336c: beq   v0,zero,0x80033384
  80033370: ori   v0,zero,0x4        ; (Delay)
  80033374: lui   at,0x800b
  80033378: sh    v0,-13734(at)      ; 0x800aca5a := 4 (NACHLADEN)
  8003337c: j     0x80033394
  80033384: lui a1,0x800b / addiu a1,a1,-13688 / jal 0x80045024 / ori a0,a0,0x1   ; Leer-Klick
```

**Antwort auf (a): BESTAETIGT.** Im ganzen Block stehen **genau fuenf** Bedingungen:
SQUARE gehalten @0x80033308, Magazin==0 @0x8003331c, SQUARE-**FLANKE** @0x80033344,
Reserve @0x80033354, Waffen-Id < 9 @0x8003336c. Zwischen `sltiu v0,v0,0x9`
@0x80033368 und `sh 4 -> 0x800aca5a` @0x80033378 liegen nur `beq` @0x8003336c,
das Delay-`ori 4` @0x80033370 und `lui at` @0x80033374.
**Kein Gegenstueck zu `s_aim_melee`. Auch keines zu `s_aim_recoil`.**

### Wo die Klassentrennung im Original wirklich sitzt

Nicht in einem Flag, sondern im **Dispatch, der JEDES BILD neu liest**
(`re15_disasm.py dis 0x80032e44 20`):

```
  80032e60: lbu   v1,-13731(v1)      ; 0x800aca5d  <-- JEDES BILD frisch
  80032e68: sll   v1,v1,2
  80032e74: addiu at,at,16432        ; 0x80074030
  80032e78: addu  at,at,v1
  80032e7c: lw    v0,0(at)           ; Klassen-Handler
  80032e84: jalr  v0
```
Ids 0..2 -> `0x80034E70` (Nahkampf), 3..11/13/15..18 -> `0x80032E9C` (Standard),
12/14/19 -> `0x80034014`. Eine Nahkampfwaffe erreicht 0x80033300 nie, **weil der
Dispatch sie wegleitet** — nicht, weil dort ein Melee-Flag geprueft wuerde.
Wechselt die Waffe, folgt das Original im **naechsten Bild**.

### Der Port

```c
/* player_common.c:913-916 — Latch NUR beim Uebergang AIM_NONE -> RAISE */
if (s_player_aim_phase == RE15_AIM_NONE) {
    s_player_aim_phase = RE15_AIM_RAISE;
    s_aim_melee = (re15_player_equipped_weapon() < 3);
    ...
}
/* player_common.c:340 */
if (s_player_aim_phase != RE15_AIM_READY || s_aim_recoil || s_aim_melee) return;
```
Die ersten beiden Abbrueche sind **redundant**: der einzige Aufrufer
`game_step_common.c:1597` verlangt schon `re15_player_aim_ready()`, und das ist
laut `player_common.c:386` genau `phase==READY && !recoil`.
**Uebrig bleibt genau EINE zusaetzliche Sperre ohne Original-Gegenstueck: `s_aim_melee`.**

---

## (b)+(c) Die Messung — `probe_m93r_nachladen.c`

Die Sonde faehrt den echten Weg: `re15_game_step()` + Pad (R1 halten bis zielbereit,
dann Abzugszyklen mit echter SQUARE-**Flanke** im ersten Bild und losgelassenem Abzug
in den letzten sechs). Cliplaengen = die **selbst geparsten** PLW-EDD-Werte
(`PL00W05.PLW` EDD @0x8: 14 Clips `[22,16,52,1,50,30,10,22,1,23,1,23,1,32]`,
`PL00W03.PLW`: `[...,23,1,24,1,24,1,32]` — Clip 13 = Nachladen = 32 Bilder in beiden).

**Abdeckung: 2188 Bilder in sechs Laeufen** (A 290, B 730, C 210, D 232, D2 312, E 414).

| Lauf | Aufbau | Nachlade-Phase (3) | Nachfuellung |
|---|---|---|---|
| **A** | M93R Id 5, Magazin 15, Munition 50, normal angelegt | **Bild 131** | Bild 162: Magazin 0->15 (sofort -3 durch den gehaltenen Abzug = 12), Munition **50 -> 35** |
| **B** | Browning Id 3, Magazin 15, Munition 50 (Gegenprobe, 1 Schuss/Abzug, 18 Zyklen) | **Bild 371** | ja |
| **C** | M93R, Magazin schon 0 — nur das Gate | **Bild 11** | Bild 42, Munition 50 -> 35 |
| **D** | M93R, Magazin 0, aber **mit dem Messer angezielt** | **NIE** (232 Bilder) | nie |
| **D2** | M93R, Magazin **15**, mit dem Messer angezielt | **NIE** (312 Bilder) | nie |
| **E** | Echter Weg: Messer zielen -> START -> Inventar | Menue-Haelfte = **Fehllauf**, s.u. | — |

### Lauf A — der Port laedt die M93R nach (Auszug)

```
Start: Waffe=5 Magazin=15 Munition0x15=50 eqSlot=1
ZIELBEREIT nach 10 R1-Bildern (melee_latch=0)
  11 | R1+SQ^ | 5 |  12 |   50 |   0   | 22 01 |   1   |  2  |  7  0/22
  32 | R1+SQ  | 5 |   9 |   50 |   0   | 22 11 |   1   |  2  |  7  0/22
  53 | R1+SQ  | 5 |   6 |   50 |   0   | 22 11 |   1   |  2  |  7  0/22
  74 | R1+SQ  | 5 |   3 |   50 |   0   | 22 11 |   1   |  2  |  7  0/22
  95 | R1+SQ  | 5 |   0 |   50 |   0   | 22 11 |   0   |  2  |  7  0/22
 116 | R1+SQ  | 5 |   0 |   50 |   0   | 22 10 |   0   |  2  |  8  0/1
 131 | R1+SQ^ | 5 |   0 |   50 |   0   | 23 00 |   0   |  2  | 13  0/32   <== Clip 0x0D
 162 | R1+SQ  | 5 |  12 |   35 |   0   | 32 01 |   1   |  2  |  7  0/22   <== Nachfuellung
```
Damit sind alle bisher vermuteten M93R-Ursachen **gemessen widerlegt**: eigene FSM,
Munitionssorte, Nachlade-Portion, Clip-Laenge, Burst-Teilbarkeit, Dauerfeuer-Latch,
Slot-Aufloesung. Im sauberen Weg funktioniert die Kette Ende zu Ende.

### Lauf D2 — der Defekt, mit exakt dem gemeldeten Bild

```
ZIELBEREIT nach 32 R1-Bildern (melee_latch=1)
  -> Waffe im Zielen auf 5 gewechselt; melee_latch bleibt 1
  33 | R1+SQ^ | 5 |  12 |   50 |   1   | 22 01 |   1   |  2  |  7  0/22
  54 | R1+SQ  | 5 |   9 |   50 |   1   | 22 11 |   1   |  2  |  7  0/22
  75 | R1+SQ  | 5 |   6 |   50 |   1   | 22 11 |   1   |  2  |  7  0/22
  96 | R1+SQ  | 5 |   3 |   50 |   1   | 22 11 |   1   |  2  |  7  0/22
 117 | R1+SQ  | 5 |   0 |   50 |   1   | 22 11 |   0   |  2  |  7  0/22
 153 | R1+SQ^ | 5 |   0 |   50 |   1   | 22 00 |   0   |  2  |  7 21/22
 193 | R1+SQ^ | 5 |   0 |   50 |   1   | 22 00 |   0   |  2  |  7 21/22
 233 | R1+SQ^ | 5 |   0 |   50 |   1   | 22 00 |   0   |  2  |  7 21/22
 273 | R1+SQ^ | 5 |   0 |   50 |   1   | 22 00 |   0   |  2  |  7 21/22
ABDECKUNG: 312 Bilder; Nachlade-Phase(3) zuerst in Bild -1; Nachfuellung in Bild -1
```
**Die Waffe feuert voellig normal** (Burst zu 3, Magazin 15 -> 0), und ab Magazin 0
passiert bei **jeder** neuen Abzugs-Flanke (Bilder 153/193/233/273) **nichts**:
`ph` bleibt 2, `res`=2 (Munition liegt in Slot 2), Munitionsmenge bleibt 50.
Kein Nachladen **und kein Leer-Klick** — weil `game_step_common.c:1602` wegen
`res>0 && id<9` den Nachlade-Zweig waehlt und
`re15_player_reload_start()` dort an `player_common.c:340` **stumm** zurueckkehrt.
Der `else`-Zweig mit dem Klick (`game_step_common.c:1605`) wird nie erreicht.

### Erreichbarkeit im echten Spiel — was gemessen ist und was nicht

Gemessen (Lauf E, erster Teil):
```
ZIELBEREIT (Messer) nach 32 R1-Bildern: melee_latch=1 Waffe=1
nach START-Flanke: menue_eingefroren=1
im Menue (20 Bilder): Zielphase=2 recoil=0 melee_latch=1 (eingefroren erhalten)
```
* Das Inventar **oeffnet im Zielzustand** — `s_inv_open_allowed`
  (`game_step_common.c:1134-1136`) kennt Flinch, Knockdown, Grab, Tod, Cinematic und
  Treppe, aber **keine** Zielbedingung.
* Waehrend `re15_menu_gameplay_frozen()` kehrt `game_step` vorzeitig zurueck
  (`game_step_common.c:1141-1143`), der Spieler-Tick laeuft nicht — Zielphase **2**
  und `s_aim_melee` **1** ueberleben den ganzen Menue-Aufenthalt.
* Der Schliess-Commit `menu_common.c:1347` (bzw. `:1887`) ruft
  `re15_player_set_equipped_weapon(wid)` und ruehrt den Zielzustand **nicht** an.
  Genau diesen Aufruf faehrt Lauf D/D2.

⛔ **FEHLLAUF, sagt NICHTS:** die zweite Haelfte von Lauf E (Menue wirklich schliessen)
laeuft in dieser Unit-Sonde nicht durch — die Menue-FSM bleibt bei `stage=2 phase=0`
stehen und ist nach 200 Bildern immer noch eingefroren (sie braucht die
Plattform-Schicht). Die Kette Menue-Schliessen -> Commit ist damit **nicht live
gemessen**, sondern nur aus den drei oben zitierten Code-Stellen belegt; der
Commit-Effekt selbst ist in D/D2 gemessen.

⛔ **Nicht bewiesen:** dass der Nutzer *genau diesen* Weg gegangen ist. Bewiesen ist:
(1) im sauberen Weg laedt die M93R nach (A/C), (2) es gibt im Port **genau einen**
Zustand, der das Nachladen stumm verschluckt und dabei das Feuern unberuehrt laesst
(D2), und (3) dieser Zustand hat im Original kein Gegenstueck.

---

## (d) Umsetzungsplan

**Einordnung (Projektziel-Regel 5):** RE1.5 hat das Nachlade-System **vollstaendig** —
Gate @0x80033300-94, Sub 4 @0x80033d7c, Nachfuellung `FUN_8004ebdc` @0x8004ebdc.
Hier ist **RE1.5 massgeblich**, RE2 wird nicht gebraucht. Zu bauen ist kein neues
Verhalten, sondern das **Entfernen einer im Port erfundenen Bedingung**.

**Schritt 1 (der eigentliche Fix, minimal und byte-true).**
`re15_port/engine/src/player_common.c:340`
```c
if (s_player_aim_phase != RE15_AIM_READY || s_aim_recoil || s_aim_melee) return;
```
-> `s_aim_melee` streichen. Begruendung mit Beleg im Kommentar:
das Original prueft an `sh 4 -> 0x800aca5a` **@0x80033378** keine Klasse; die
Klassentrennung erledigt der Dispatch **@0x80032e60 / Tabelle @0x80074030**, und der
liest die angelegte Waffe jedes Bild neu. Ein Messer kann trotzdem nicht nachladen,
weil der einzige Aufrufer `game_step_common.c:1597` schon `eq_item >= 3` verlangt
(Spiegel desselben Dispatches) und `game_step_common.c:1602` zusaetzlich
`eq_item < 9` (`sltiu` @0x80033368).
`s_aim_recoil`/`phase != READY` koennen bleiben — sie sind durch
`re15_player_aim_ready()` (`player_common.c:386`) ohnehin schon erfuellt.

**Schritt 2 (die Wurzel, eigene Runde, eigene Messung).**
Der Latch `player_common.c:916` selbst ist die Abweichung: das Original hat
**keinen** — `lbu 0x800aca5d` @0x80032e60 laeuft jedes Bild. Byte-true waere,
die Klasse pro Bild aus `re15_player_equipped_weapon()` abzuleiten statt sie zu
latchen. Das beruehrt neun weitere Lesestellen (`player_common.c:305, 328, 378, 444,
847, 882, 884, 923, 935, 957, 999`) und gehoert deshalb **nicht** in denselben Commit.
Gemessen ist bereits (Lauf D2, 312 Bilder), dass der stale Latch das **Feuern**
nicht stoert — der Handlungsdruck liegt also ganz auf Schritt 1.

**Schritt 3 (Regressions-Wache).** `probe_m93r_nachladen.c` als `add_test`
aufnehmen und auf `rD2 > 0` pruefen (heute -1). Dann `RE15_MIN_TESTS` in
`re15_port/tools/local_build.sh` um 1 anheben.

---

## Offen / nicht gemessen

* Die Menue-Schliess-Kette live (Lauf E, zweite Haelfte) — **Fehllauf** in der
  Unit-Sonde. Naechster Weg: `RE15_FRAMEDUMP` + `RE15_PRESS` im echten Renderpfad
  (Messer zielen, START, im Menue die M93R anlegen, START, Abzug) und der
  `RE15_WAFFEN_LOG`-Haken um `melee=` erweitern.
* Ob der Nutzer diesen Weg gegangen ist. Ein einziger Nutzerlauf mit
  `RE15_WAFFEN_LOG` + dem neuen `melee=`-Feld entscheidet es.
* Ob die Zielphase im Original einen Menue-Aufenthalt ueberhaupt ueberlebt
  (Port: ja, gemessen). Nicht nachgeschlagen, weil es am Befund nichts aendert:
  ueberlebt sie, folgt das Original trotzdem der neuen Waffe (@0x80032e60);
  ueberlebt sie nicht, erst recht.
