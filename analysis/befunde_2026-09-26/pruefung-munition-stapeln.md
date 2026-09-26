# Pruefung: munition-stapeln.md (Skeptiker-Urteil)

Geprueft am 2026-09-26. Keine Engine-Aenderung, kein Build.
Selbst nachgelesen: 19 Adressen/Tabellen in `info/Re1.5/PSX.EXE`,
`info/re2leon/PSX.EXE`, `info/Re1.5/PSX/BIN/STAGE{1..6}.BIN` — plus vier eigene
Roh-Byte-Zensus-Laeufe.

**Urteil: der KERNBEFUND haelt (ich habe ihn unabhaengig reproduziert), aber das
Dossier ist NICHT haltbar** — drei Saetze behaupten mehr, als ihre zitierten
Belege tragen, und der Umsetzungsplan §5.2 hat einen konkreten
Duplikations-Defekt.

---

## 0. Zuerst: ist ueberhaupt gegen die richtige Datei geprueft worden?

Ja. Nachgemessen:

| Datei | Header | t_addr | RAM-Abbildung | Dossier nutzt |
|---|---|---|---|---|
| `info/Re1.5/PSX.EXE` | 0x800 | 0x80010000 | `0x80010000 + off - 0x800` | richtig |
| `info/re2leon/PSX.EXE` | 0x800 | 0x80010000 (t_size 0xf0800) | dito | richtig |
| `info/Re1.5/PSX/BIN/STAGE*.BIN` | **kein** | — | `0x80100000 + off` | richtig (vom Skript-Header `re15_disasm.py:14` bestaetigt) |

Kein Overlay-/Header-Verwechsler. Das ist die haeufigste Falle dieser Codebasis
und sie ist hier vermieden.

---

## 1. Was ich selbst nachgelesen habe — und was bestaetigt ist

### 1.1 Eigener jal-Zensus RE1.5 (Roh-Byte, `(w>>26)==3`, ganze EXE ab 0x800)

Ich habe das Dossier nicht nachgelesen, sondern den Zensus neu gefahren —
**und zusaetzlich den Zeiger-Zensus, den das Dossier NICHT gemacht hat**
(ein `jal`-Zensus allein schliesst einen `jalr`-Aufruf ueber eine
Dispatch-Tabelle nicht aus):

| Ziel | jal-Treffer | Datenwort == Ziel |
|---|---|---|
| `0x8004dc4c` (Insert) | **1** → `0x8001e0c4` | **0** |
| `0x8004df2c` (erster freier Platz) | 3 → `0x8001df14`, `0x8004daec`, `0x8004debc` | 0 |
| `0x8004dfec` (gleiche Id suchen) | 2 → `0x8004ebbc`, `0x8004ec34` | 0 |
| `0x8004df90` (Zaehler frei) | 1 → `0x8004dc68` | 0 |
| `0x8004e054` (Menue-Zusammenlegung) | 1 → `0x8004b3b8` | 0 |
| `0x80043328` (SCD-Item-Get) | 0 | **1** → `0x800746c0` (Opcode-Tabelle) |

Die Tabelle des Dossiers stimmt Zahl fuer Zahl. Und weil `0x8004dc4c` in
keinem Datenwort steht, gibt es auch keinen indirekten Aufruf. Der
`jalr`-Ausweg, den das Dossier nicht geprueft hat, ist damit nachtraeglich zu.

### 1.2 Der Insert stapelt nicht — gelesen, nicht zitiert

```
8004debc: jal 0x8004df2c          ; erster FREIER Platz
8004dee4: sb  s0,0(at)   [4268]   ; slot.id  = Id
8004def4: sb  s1,0(at)   [4269]   ; slot.qty = Menge   <- ZUWEISUNG
8004df04: sb  zero,0(at) [4270]   ; slot.flag= 0
```
`s1` kommt unveraendert aus `a1` (`8004dc6c addu s1,a1,zero`). Kein Lesen des
alten qty vor diesem `sb`, keine Addition, kein Tabellenzugriff auf
`0x80074da8` im Bereich `0x8004dc4c..0x8004df28`.

`FUN_8004df2c` wertet `a0` nachweislich nicht aus (kein `andi a0,a0,0xff`, kein
Vergleich gegen a0) — `FUN_8004dfec` tut es (`8004e000 andi a0,a0,0xff`,
`8004e024 bne v0,a0`). Der Unterschied, den das Dossier behauptet, ist echt.

### 1.3 Eigener Zensus aller qty-Schreibstellen (das Dossier hat ihn NICHT gemacht)

Ich habe alle 61 Stellen mit `addiu rX,rY,0x10ad` (qty-Feld) aufgeloest und die
30 davon isoliert, die dahinter ein `sb 0(rX)` haben. Ergebnis: **keine einzige
Stelle im Aufnahme-Pfad addiert**. Die einzige Addition auf ein qty-Feld im
ganzen PSX.EXE ist `8004e398 addu v0,v0,a2` → `8004e3a8 sb` in der
Menue-Zusammenlegung.

### 1.4 Overlay-Zensus (eigener Lauf, Basis 0x80100000, kein Header)

| Ziel | S1 | S2 | S3 | S4 | S5 | S6 |
|---|---|---|---|---|---|---|
| `jal 0x8004dc4c` / `0x8004dfec` / `0x8004df2c` / `0x80043328` | 0 | 0 | 0 | 0 | 0 | 0 |
| Gegenprobe: `jal` in den EXE-Bereich gesamt | 1079 | 736 | 1124 | 881 | 1150 | 82 |

Reproduziert die Dossier-Zahlen exakt, inklusive der Gegenprobe 1079/736.

### 1.5 FSM-Starter

Zensus auf `DAT_80072d3b`: 10 Zugriffe, davon genau einer ausserhalb der FSM
(`0x8004334c sb`, plus `0x8004332c lbu`). Stimmt.

### 1.6 Obergrenzen-Tabelle @0x80074da8 (direkt aus der Datei gelesen)

Schrittweite 12 bestaetigt. `0x15..0x21` = **250**, ausser `0x18` FLAME FUEL =
**100**; `0x13`=100, `0x14`=6, `0x22`/`0x23`=1; `0x03`=15, `0x07`=6, `0x08`=7,
`0x0c`=100, `0x0d`=12. Alle Zahlen des Dossiers stimmen.

### 1.7 Menue-Arithmetik (alle fuenf zitierten Stellen gelesen)

`8004e13c lbu` + `8004e14c lbu` → `8004e160 addu s0,a0,v0` (Summe) ·
`8004e338 lbu v1` (cap) · `8004e340 sltu v0,v1,s0` · Teiluebertrag
`8004e3f4 sb v1` und `8004e410 subu v0,s0,v0` / `8004e420 sb` · Vollfall
`8004e398 addu v0,v0,a2` / `8004e3a8 sb`. Alles wie beschrieben.
Aufrufer `0x8004b3b8` sitzt hinter `8004b3a4 andi v0,v1,0x4000` = Bestaetigen im
Menue → die Zusammenlegung ist tatsaechlich eine **manuelle Menue-Aktion**.

### 1.8 RE2 (eigener Zensus, richtige Binaerdatei)

`jal 0x80069adc` = **2** (`0x80058864` SCD-Item-Get, `0x80072320` Box). Kein
Datenwort. Der Insert ruft `8006 9b00 jal 0x80069668` mit `80069af8 addu a0,zero,zero`
— und `FUN_80069668` liefert bei `a0==0` den ersten Platz mit `id==0`
(`80069684 lbu`, `8006968c bne v0,zero`, `80069694 bne a0,zero`). Kein
`jal 0x800696cc`, keine Addition. Der Item-Get-Aufrufer liest die Menge aus dem
Script-Operanden (`80058860 lbu a1,2(v0)`) und prueft davor nichts Inventarisches.
**Die RE2-Aussage haelt.**

### 1.9 Gegenhypothese "stapelt vielleicht auf anderem Weg" — aktiv gesucht, nicht gefunden

* RE1.5: die einzigen zwei Aufrufer des Gleiche-Id-Suchers (`0x8004ebbc`,
  `0x8004ec34`) sitzen in `FUN_8004eb70`/`FUN_8004ebdc`, die beide ueber die
  Deskriptor-Tabelle `0x80074dac` (Feld +4) vom **aktuell gewaehlten** Platz
  (`0x800b25c8`) auf eine Id kommen und daraufhin das Inventar durchsuchen
  (`8004eb70 → 8004ebc4 sll/slt` liefert ein Praedikat). Kein Schreiben.
* Die Menue-Zusammenlegung braucht `0x4000` = Tastendruck. Nichts Automatisches.
* RE2: 15 Aufrufer von `FUN_800696cc`. Ich habe zwei ungenannte selbst gelesen:
  `0x8003ab4c` (`addiu a0,zero,53` = Abfrage, ob Id 0x35 vorhanden) und
  `0x80051628` (`bltz s1` → Verzweigung). Beides Abfrage, kein Hinzufuegen.

**Die Kernbehauptung "beim Aufnehmen wird nicht gestapelt" ist damit von mir
unabhaengig reproduziert.**

---

## 2. Was NICHT haelt — Saetze, die mehr behaupten als die Belege tragen

### 2.1 ⛔ FALSCH: der Bereich des Inventar-Zensus (§2.6, letzter Absatz)

> "Und der Zensus ueber das Inventar-Array selbst (`0x800b10ac`) findet **55**
> Zugriffsstellen, **alle** im Inventar-Modul `0x8004a...0x8004e...` — es gibt
> keinen zweiten, versteckten Einfuege-Pfad ausserhalb."

Die **Zahl 55 stimmt** (mein Zensus: 55 Stellen mit `imm 0x10ac`).
Der **Bereich stimmt nicht**: **10 der 55** liegen unterhalb `0x8004a000` —
`0x80045e88, 0x80045f34, 0x80045f90, 0x80046414, 0x8004660c, 0x80046674,
0x80047d44, 0x80048f74, 0x80049114, 0x80049778`.

Und es gibt dort sehr wohl einen **zweiten Platz-Fueller ausserhalb** des
behaupteten Bereichs, den das Dossier nirgends erwaehnt:

```
80045e7c: lbu v1,0(at)   [0x80074bb8 + s0]   ; Start-Id aus Tabelle
80045e90: sb  v1,0(at)   [0x800b10ac + i*4]  ; slot.id
80045eb4: sb  zero,0(at) [0x800b10ae + i*4]  ; slot.flag
80045ec4: sb  v1,0(at)   [0x800b10ad + i*4]  ; slot.qty  aus 0x80074bd0
```
Tabellen direkt gelesen: `0x80074bb8 = 01 03 15`, `0x80074bd0 = 00 0f 32`
= KNIFE x0 / BROWNING x15 / H.GUN BULLETS x50. Das ist die
**Start-Ausruestung** — genau die Zeile `01:0 03:15 15:50`, die das Dossier in
§1 aus den Savestates liest, ohne zu merken, dass sie von hier kommt.

Folge fuer das Urteil: der *Aufnahme*-Schluss kippt dadurch nicht (die
Start-Ausruestung ist keine Aufnahme). Aber der Satz ist in beiden Haelften
falsch, und er ist genau der Satz, der die negative Aussage absichern soll.

### 2.2 ⛔ FALSCH: "kein `lbu` des alten qty im ganzen Insert" (§2.3)

> "Es gibt im ganzen Insert kein `lbu` des alten qty, keine Addition, keinen
> Vergleich gegen eine Obergrenze."

Der Code-Auszug des Dossiers springt stillschweigend von `8004dc7c` nach
`8004debc` und ueberspringt damit **142 Instruktionen** (`0x8004dc80..0x8004deb8`,
den 2-Zell-Waffen-Zweig). In diesem Zweig steht sehr wohl ein `lbu` des alten
qty: `8004de18 lbu v1,4269(v1)` (und `8004de80 sb s1,4269(at)`).

Ich habe den Zweig komplett durchgesehen: es ist ein **Verschiebe-Block**
(slot[i] → slot[i+2] samt Icons, `jal 0x80049390` 8x), keine Addition und kein
Tabellenzugriff. Die *Schlussfolgerung* haelt also — der *Satz* ist falsch, und
er ist falsch, weil ein Drittel der Funktion aus dem Zitat herausgeschnitten
wurde. Genau das Muster, das diese Codebasis schon mehrfach in die Irre gefuehrt
hat.

### 2.3 ⛔ UEBERZOGEN: "Munition ist byte-true der Id-Bereich 0x15..0x21" (§2.8)

Die zwei zitierten Instruktionen sagen das nicht. Ich habe beide gelesen:

```
80047d54: sltiu v0,v0,0x15   ; id < 0x15  -> Flag 0x10 (Waffen-Klasse)
80049124: sltiu v0,v0,0x22   ; id < 0x22  -> anderer Zweig
```
Sie begrenzen eine **Klasse** (nicht-Waffe und unter 0x22). Dass diese Klasse
"Munition" *ist*, steht nirgends im Code — es ist die Benennung des Ports.
Das Dossier fuehrt in derselben Tabelle unter der Ueberschrift
"**Zensus der Munitionssorten**" `0x21 = MEMORY CARD` (und `0x1d = EMPTY SHELLS`)
als Munitionssorte. Eine Memory Card ist keine Munition.

Konkrete Folge fuer den Plan: `re15_pickup_stapeln_nutzer` soll laut §5.1 genau
auf `re15_item_is_ammo` gaten (`inventory_common.c:239` = `id>=0x15 && id<0x22`).
Damit wuerde die neue Nutzer-Regel auch **MEMORY CARD und EMPTY SHELLS stapeln**
— eine Abweichung, die der Nutzer nicht gewuenscht hat und die im Dossier nicht
als solche benannt ist.

### 2.4 ⛔ UEBERZOGEN: der RE2-Satz stuetzt sich auf 3 von 15 Stellen (§2.10)

> "Die RE2-Stellen, die `FUN_800696cc` (gleiche Id) rufen, sind SCD-Opcodes zum
> **Entfernen/Pruefen**, nicht zum Hinzufuegen: `0x800584f0`, `0x800585e4`,
> `0x80058660`."

Mein Zensus findet **15** Aufrufer, u.a. `0x8003ab4c`, `0x8003ab68`, `0x8003ad8c`,
`0x8003adfc`, `0x8003ae30`, `0x80051628`, `0x80051b34`, `0x80052118`,
`0x8005233c` — neun davon liegen in ganz anderen Modulen als den drei genannten.
Der Satz ist eine Allaussage ueber eine Menge, von der ein Fuenftel geprueft
wurde. Ich habe zwei der neun stichprobenartig gelesen; beide sind Abfragen.
Die Aussage ueberlebt die Stichprobe, aber sie war zum Zeitpunkt der
Niederschrift nicht belegt.

### 2.5 ⛔ UEBERZOGEN: "alle 50 sauberen Savestates" (§1 und §6)

`stage_saves/` enthaelt **85** `.sav`, davon 6 `PATCHED-EXE_*` → **79 saubere**.
Gelesen wurden 50. "alle 50 sauberen Savestates" ist also "50 von 79". CLAUDE.md
sagt selbst "die uebrigen ~78 Savestates sind sauber". Materiell folgenlos (die
Savestates koennen die Frage ohnehin nicht entscheiden, das sagt §6 richtig) —
aber "alle" ist wieder ein Wort, das mehr behauptet als gemessen wurde.

---

## 3. RISIKO des Umsetzungsplans — §5.2 baut eine Munitions-Duplikation

Der Plan will `item_modal_common.c:226/234` so aendern, dass "kein freier Platz"
nicht mehr automatisch "kann ich nicht tragen" bedeutet, wenn ein gleicher Platz
noch Luft hat. Ich habe alle vier Verwendungen von `s_grant` gelesen
(`item_modal_common.c:39, 226, 232/233, 234, 293`). Der Plan uebersieht §7:

```c
/* item_modal_common.c:293 */
if (s_grant < 0 || s_msg_no || re15_inv_grant(s_type, s_amount) != 0) {
        ... s_state = 8;  return;       /* KEINE Aufnahme, Weltobjekt bleibt stehen */
}
```
und

```c
/* inventory_common.c:36 */
if (free_slot < 0) return -1;           /* full */
```

**Ausfall-Szenario (konkret):** Inventar voll (10/10 belegt), darunter ein Platz
H.GUN BULLETS mit 240. Der Spieler nimmt eine 30er-Packung auf (nach der
Halbierung 15). Neue Regel: Ziel ist stapelbar → `s_grant` gilt → Prompt 1
("WILL YOU TAKE …") → Zustand 7 stapelt bis `cap=250` (@0x80074da8+12*0x15),
Rest 5 geht laut Plan "den normalen `re15_inv_grant`-Weg" → dort `free_slot < 0`
→ `return -1` → die Bedingung in `:293` ist **wahr** → Zustand 8:
`s_taken`-Flag wird NICHT gesetzt, `g_aot.slots[].active` bleibt 1,
`scd_prop_hide_by_obj_id` laeuft nicht — **das Weltobjekt bleibt liegen, obwohl
10 Schuss schon im Beutel sind**. Wiederholtes Ansprechen = unbegrenzte Munition.

**Zweiter Schaden, byte-true:** `:234` speist `:239`
`re15_item_prompt_walk(s_prompt, …)` — den byte-true Glyphen-Zaehler der
Prompt-Schreibmaschine. Wer `s_prompt` in einem Fall von 2 auf 1 dreht, in dem
das Original 2 zeigt (`@0x8001df14` + `@0x8001e054`, beide von mir gelesen und
korrekt zitiert), aendert nicht nur einen Text, sondern die gemessene
Einblendlaenge — in einem Subsystem mit eigenen Pins.

**Sauberer Weg (ohne Aenderung an :226/:234):** das Stapeln ausschliesslich in
Zustand 7 vor `re15_inv_grant` haengen und nur dann Rest ueberhaupt entstehen
lassen, wenn `s_grant >= 0`. Dann bleibt der byte-true Voll-Zweig unberuehrt und
die einzige Abweichung ist "gleiche Munition landet auf einem Platz".

---

## 4. Fehlende Messung

Der ganze negative Beweis ist statisch. Die eine Messung, die ihn zur Gewissheit
machen wuerde, nennt das Dossier selbst und fuehrt sie **nicht** aus:
im echten Spiel (DuckStation, `re15-room-capture`) ROOM1020 laden, **beide**
H.GUN-BULLETS-AOTs aufnehmen (`test_item_stack_press.c:3`, main00 @0x1d6a und
@0x1d80) und danach `0x800b10ac` (Schrittweite 4, Anzahl aus `0x800b0fbc`)
auslesen: zwei Plaetze `15:x`/`15:y` oder einer mit der Summe. Die 50 gelesenen
Savestates zeigen ausnahmslos die Start-Ausruestung aus `0x80074bb8`/`0x80074bd0`
— sie enthalten keine einzige aufgenommene zweite Packung und koennen die Frage
deshalb nicht entscheiden.

---

## 5. Urteil

Kernbefund reproduziert: beim Aufnehmen stapelt weder RE1.5 noch RE2 — ein
Insert, ein Aufrufer, kein Zeiger, kein Overlay-Aufruf, keine Addition in 30
qty-Schreibstellen. **Haltbar ist das Dossier trotzdem nicht**: drei seiner
Saetze (§2.6-Bereich, §2.3 "kein lbu", §2.8 "Munition 0x15..0x21") sagen mehr,
als die zitierten Stellen hergeben, und §2.10/§1 verallgemeinern von
Stichproben. Der Umsetzungsplan §5.2 baut in der vorgeschlagenen Form eine
Munitions-Duplikation und faesst den byte-true Prompt-Zweig an — er darf so
nicht gebaut werden.
