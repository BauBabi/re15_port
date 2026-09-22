# Das Fenster der Wegwerf-Abfrage — abgesichert (Titel berichtigt)

> ⛔ **BERICHTIGUNG, Runde 24 (2026-09-22).** Der ursprüngliche Titel dieses Dossiers
> lautete „geschlossen, nicht abgesichert“, und der Text sagte an mehreren Stellen, das
> Fenster sei **weg**. Das ist widerlegt und gemessen: es bleibt **1 Bild je Stelle** —
> das Schließ-Bild, in dem der Nachrichten-Freeze schon gelöst und die Abfrage noch
> nicht offen ist. Es wird von der Port-Zeile `re15_discard_frozen()` gehalten; RE2 hat
> dieses Bild nicht (`@0x80051810` dekrementiert, `@0x80051844 lui v1,0xff00` friert im
> **selben Aufruf** wieder ein, also 0 Bilder). Ebenfalls berichtigt: die hier genannte
> Zahl **4** Brücken-Bilder stammt vom **falschen Zeugen** (`RE15_PAUSE_PAD 0x01000000`,
> das nur die SCD-Pad-Wörter maskiert und zusätzlich vom Szenen-Fenster gehalten wird).
> Mit `RE15_PAUSE_PLAYER 0x80000000` (`@0x80031c54` / `@0x80031c78 bltz`) sind es
> **1 Bild an 10 von 10 Stellen**. Und ROOM1090 wurde in Teil J stillschweigend
> übersprungen statt als **AUSGELASSEN** ausgewiesen.
> **Gültig ist `discard-besitz.md` (Runde 24).**

Runde 23, 2026-09-22. Vorgänger: `discard-umsetzung.md` (Bau), `discard-nacharbeit.md`
(zwei Sperrgründe), `discard-loch.md` (Runde 22 — der Pad-Riegel, der eine Tautologie war).

Suite: **337/337** (`bash re15_port/tools/local_build.sh all`).

---

## 0. Kurzfassung

| | |
|---|---|
| **Der Auftrag** | Nicht das Fenster absichern, sondern es **schließen**: die Port-Wartezeit auf die RE2-Spanne kürzen. |
| **Woraus die Wartezeit bestand** | ROOM1100 sub02 msg 4: **84** Bilder Nachricht + **161** Bilder Überhang (32 Szene + 80 Faden-Ende + 49 Folge-Nachricht). Die **80** Faden-Bilder sind das Loch. ROOM4000 sub02 msg 2: 74 + **29**, alle 29 Faden. Zusammen über 10 Stellen: **887** Bilder Überhang, **191** davon mit freiem Pad. |
| **Der Fix** | Die beiden Port-Schranken (Faden-Ende, Szenen-Fenster) sind **ersatzlos weg**. Es bleibt RE2s einzige Schranke, das **Belegt-Bit des Nachrichtensystems** (`@0x800517f0 andi v0,v0,0x80` / `@0x800517f4`). Der Port hat dieses Bit wörtlich: `s_pause_open` (Open-Guard `@0x80027e7c`, Löschen `@0x80028598/@0x800286c0/@0x8002870c`). |
| **Warum das reicht** | Alle **16** ausgelieferten Benutzungsstellen öffnen ihre Nachricht mit Maske **0xFFFF0000** — darin Pad-Bit `0x01000000` (`@0x800304f4`) **und** Skript-Bit `0x02000000` (`@0x8003f044`). In der Spanne kann also weder der Spieler noch das Skript etwas tun. |
| **Was ersatzlos entfällt** | Der Pad-Riegel, die SCD-Pad-Maske (die `Sce_key_ck` fälschte) **und** die unbelegbare Port-Entscheidung „darf die Vormerkung den Raumwechsel überleben?". |
| **Riegel** | TEIL I (Spanne zerlegt, Überhang als Gegenprobe), TEIL J (px/pz hinter dem **echten** `re15_game_step`: JETZT 0 / GEGENPROBE 6357 / STUB 0), TEIL K (Skript parkt und läuft nach der Antwort weiter). |
| **Live** | ROOM1100, echter Renderpfad, 120 Wartebilder, **0** mit freiem Pad, `px/pz` konstant bei `-20615/-25000` bei **gehaltenem Vorwärts**. |

---

## 1. Auftrag 1 — WORAUS die Port-Wartezeit bestand

Gemessen mit dem ausgelieferten Unterprogramm aus der RDT, im echten VM, mit
registriertem RDT (ohne das sind `Gosub`/`Evt_exec` stille No-Ops). Riegel **TEIL I**,
`re15_port/tests/unit/r21_discard_wegwerfen.c`.

Die drei Schranken des Runde-22-Modells, jede mit ihrer Code-Stelle:

| Anteil | Bedingung im Port (Runde 22) | Original-Bezug |
|---|---|---|
| **NACHRICHT** | `re15_pauseflags_belegt()` | RE2 `@0x800517e0` `lbu DAT_800e873c` / `@0x800517f0` `andi 0x80` / `@0x800517f4` `bne` — **die einzige echte** |
| **SZENE** | `re15_cine_active()` = `flag(1,27) \|\| flag(2,7)` (`game_state.c`) | Port-Zutat Runde 21; die *Wirkung* des Bits ist belegt (`@0x800304f4`-`@0x8003051c`, Eingabe auf `0xf000`), die Schranke selbst hat in RE2 kein Gegenstück |
| **FADEN** | `g_scd.threads[slot].active` (`item_discard_common.c`, Runde 21) | **kein** Original-Bezug — reine Port-Zutat |

Und so verteilen sich die Bilder (Spalte „Frage" = wann die Abfrage JETZT aufgeht,
gerechnet ab dem Bild der Vormerkung):

```
Stelle                   Nachr. |   UEBERH.  Szene  Faden   Msg2 |   Loch   Frage
ROOM1090 sub03 m9            70 |       NIE  19727      1    200 |      1      70
ROOM1100 sub02 m4            84 |       161     32     80     49 |     80      84
ROOM1101 sub02 m4            84 |       161     32     80     49 |     80      84
ROOM11E0 sub21 m12           50 |       182    182      0      0 |      0      50
ROOM11E1 sub21 m12           50 |       182    182      0      0 |      0      50
ROOM3010 sub02 m1            64 |         1      0      1      0 |      1      64
ROOM3011 sub02 m1            64 |         1      0      1      0 |      1      64
ROOM3050 sub15 m5            72 |        85      2      0     83 |      0      72
ROOM3051 sub15 m5            72 |        85      2      0     83 |      0      72
ROOM4000 sub02 m2            74 |        29      0     29      0 |     29      74
ABDECKUNG: 10 von 16 Benutzungsstellen gefahren, 6 mit Grund ausgelassen
UEBERHANG zusammen 887 Bilder, davon 191 mit FREIEM Pad;
groesstes Loch ROOM1100 msg 4 mit 80 Bildern.
```

### 1a. ROOM1100 sub02 — die 80 Bilder, Opcode für Opcode

Das Unterprogramm (`re15_port/tools/scd_dump_room.py`, Datei-Offsets):

```
0x00C8C  Set 22 02 07 01     flag(2,7) := 1        <-- Szene AUF (Pad maskiert)
0x00C90  Message_on 2b 04 ff ff  msg 4 "You've used the Minidisc Player w/ Disc."
0x00C94  Evt_next
0x00C96..0x00CB9  6x (Cut_chg + Sleep 5)           = 30 Bilder Kamera-Folge
0x00CBA  Message_on 2b 02 ff ff  msg 2
0x00CC0  Set 22 02 07 00     flag(2,7) := 0        <-- Szene ZU, Spieler FREI
0x00CC4  Cut_chg 06 / 0x00CC6 Cut_auto
0x00CC8..0x00CE5  3x Aot_reset
0x00CE6..0x00D1D  4x (Work_set + Member_set + Sleep 0x14)  = 4 x 20 = 80 Bilder
0x00D1E  Evt_end
```

* **84 Bilder NACHRICHT** — solange msg 4 offen ist. Deren Maske ist `ff ff`, also
  `0xFFFF0000` (`scd_vm.c` `op_message_on`: `pause_mask = (pc[2] | pc[3]<<8) << 16`).
  Darin steckt `RE15_PAUSE_SCD` `0x02000000`, deshalb **parkt das Unterprogramm selbst**
  (`scd_vm.c`: `if (g_re15_pauseflags & RE15_PAUSE_SCD) return;`, byte-true `@0x8003f040`
  `lw` / `@0x8003f044` `lui v1,0x200` / `@0x8003f04c` `bne`). Das ist die Spanne, die RE2
  auch hat.
* **32 Bilder SZENE + 49 Bilder Folge-Nachricht** — die Kamera-Folge und msg 2, beide noch
  innerhalb des `flag(2,7)`-Fensters bzw. mit eigenem Belegt-Bit. Hier ist der Pad
  maskiert, es ist also **kein Loch** — aber es ist Wartezeit, die RE2 nicht hat.
* **80 Bilder FADEN-ENDE** — der Schwanz **nach** `Set(2,7,0)` `@0x00CC0`: drei
  `Aot_reset` und vier Prop-Animationsschritte mit `Sleep 0x14`. Hier hat das Skript die
  Spieler-Kontrolle ausdrücklich **zurückgegeben**, und nur die Port-Schranke
  „warte auf das Ende des Unterprogramms" hielt die Abfrage noch. **Das ist das Loch.**

### 1b. ROOM4000 sub02 — die 29 Bilder

```
0x0144A  Message_on 2b 02 ff ff  msg 2 "You've used the Blue Master Keycard."
0x01450  Set(3,32,1) / 0x01454 Endif / 0x01456 Ifel_ck / 0x0145A Ck(3,32,1)
0x0145E  Set(5,0,1) / 0x01462 Set(5,1,1)
0x01466  Gosub 18 03            --> sub03
0x0146A  Evt_end
sub03 @0x0146C: Work_set(3,0), Se_on, dann 7x (Speed_set + For n + Add_speed +
        Evt_next + Next) mit n = 1,2,1,20,2,1,...  = die Tuer-Animation
```

Kein Szenen-Fenster. Die 29 Bilder sind reine `Evt_next`-Schritte der Tür-Animation —
der Spieler ist frei, und er läuft auch: **2929 Einheiten** in diesen 29 Bildern (TEIL J).

### 1c. ROOM1090 — „NIE"

Dort endet der Faden im Prüfstand überhaupt nicht: `@0x261C Plc_dest(mode 9, flag 33)` +
`@0x2624 Gosub 5`, und sub05 ist `@0x26F4 Do` / `@0x26FA Edwhile` / `@0x26FC Ck(5,33,0)`
— eine Warteschleife auf das Ankunftsflag des Spieler-Weges. Das Runde-22-Modell hätte
hier also **gar nicht gefragt**, nicht „später". Das war schon Runde 22s eigener Befund;
jetzt ist es die Zeile `NIE` in der Tabelle.

---

## 2. Auftrag 2 — das Fenster geschlossen

### 2a. Die eine Schranke, wörtlich aus RE2

`re15_discard_tick`, Zustand `D_WARTET`, ist jetzt **eine** Zeile:

```c
if (re15_pauseflags_belegt()) return;
```

```
800517e0  lbu  v0,-0x78c4(v0)         DAT_800e873c
800517f0  andi v0,v0,0x80             das BELEGT-Bit des Nachrichtensystems
800517f4  bne  v0,zero,LAB_80051870   belegt -> return, NICHTS angefasst
```

`re15_pauseflags_belegt()` (`engine/src/game_state.c`) IST dieses Bit im Port:
`s_pause_open`, gesetzt vom Open-Guard `@0x80027e74` `lbu v0,DAT_800b8520` / `@0x80027e7c`
`andi v0,v0,0x80`, gelöscht an den drei Dismiss-Stellen `@0x80028598` / `@0x800286c0` /
`@0x8002870c` (`andi v0,v0,0x7f`) — dieselben drei, an denen RE2 auch den Freeze
zurücknimmt (`LAB_800307e0` `@0x800307e8` + `@0x800307f4`).

⛔ **Nicht** `g_scd.message_active` / `message_fsm_active`: der Port hält im
Untertitel-Nachhall (`msg_common.c` Zustand 7) beide Wörter weiter auf 1, obwohl
`re15_pauseflags_close()` den Freeze dort schon gelöst hat. Eine Schranke darauf gäbe den
Spieler frei und hielte die Abfrage trotzdem zurück — ein neues Loch.

### 2b. Warum die Spanne damit keinen Überhang mehr hat — gemessen, nicht gefolgert

Zensus über alle 16 Benutzungsstellen (`Message_on <id> <lo> <hi>`, Maske = `<hi><lo> << 16`):

```
ROOM1090 msg  9  sub03 @0x02502  0xFFFF0000
ROOM10D0 msg  9  sub20 @0x019C4  0xFFFF0000
ROOM10D1 msg  9  sub20 @0x019AE  0xFFFF0000
ROOM1100 msg  4  sub02 @0x00C90  0xFFFF0000
ROOM1101 msg  4  sub02 @0x00C90  0xFFFF0000
ROOM11E0 msg  9  sub20 @0x01FAC  0xFFFF0000
ROOM11E0 msg 12  sub21 @0x01FEE  0xFFFF0000
ROOM11E1 msg  9  sub20 @0x01FAC  0xFFFF0000
ROOM11E1 msg 12  sub21 @0x01FEE  0xFFFF0000
ROOM1230 msg  9  sub20 @0x0148C  0xFFFF0000
ROOM1231 msg  9  sub20 @0x0148C  0xFFFF0000
ROOM3010 msg  1  sub02 @0x02306  0xFFFF0000
ROOM3011 msg  1  sub02 @0x0233A  0xFFFF0000
ROOM3050 msg  5  sub15 @0x02608  0xFFFF0000
ROOM3051 msg  5  sub15 @0x02608  0xFFFF0000
ROOM4000 msg  2  sub02 @0x0144A  0xFFFF0000
```

**16 von 16.** `0xFFFF0000` enthält

* `0x01000000` = `RE15_PAUSE_PAD` → Eingabe auf `0xf000` (`@0x800304f4` `lw DAT_800aca40` /
  `@0x800304f8` `lui v1,0x100` / `@0x80030514` `andi v0,v0,0xf000` / `@0x8003051c` `sw`),
  der **Spieler** kann nichts tun;
* `0x02000000` = `RE15_PAUSE_SCD` → der Frame-Runner kehrt am Kopf um (`@0x8003f044`
  `lui v1,0x200` / `@0x8003f04c` `bne`), das **Skript** kann nichts tun.

Damit ist die Spanne „vorgemerkt" deckungsgleich mit „der Spieler ist eingefroren", und
zwar aus den ausgelieferten Daten heraus, nicht aus einem Port-Riegel.

### 2c. Das SCHLIESS-Bild — die eine Zeile, die noch dazugehört

`re15_discard_frozen()` ist jetzt

```c
if (s_zustand == D_FRAGT) return 1;
return s_zustand == D_WARTET && !re15_pauseflags_belegt();
```

Grund: RE2 lässt zwischen „Belegt-Bit frei" und „Abfrage eingefroren" **kein** Bild —
dieselbe Fortsetzung dekrementiert (`@0x80051810`), fragt (`@0x80051834`) und legt die
Maske im **selben Aufruf** wieder an (`@0x80051844` `lui v1,0xff00` / `@0x80051850`
`sw v0,DAT_800cfbdc`). Der Port tickt die Abfrage-FSM erst **nach** `re15_game_step`
(damit die Bestätigungs-Flanke nicht zusätzlich den AOT-Scan feuert, wie beim Item-Modal),
und `re15_msg_tick` löst den Freeze früher im Bild (`platform/pc/main.c:4635` gegen
`:6407`). Ohne diese Zeile wäre genau das eine Bild frei.

⛔ **Und dieses Bild gibt es in RE2 auch** — Berichtigung aus dem Urteil: `LAB_800307e0`
hat **zwei** Eingänge, und nur der erste ist die Taste:

```
case 5  @0x8003079c  lw   v0,DAT_800ce310
        @0x800307a8  andi v0,v0,0x3000      Bestaetigen?
        @0x800307b0  _ori v0,zero,0xffff
        @0x800307b8  sw   v0,DAT_800ce30c   Pad-Vorwort gestopft
        @0x800307bc  j    LAB_800307e0
case 6  @0x800307c4  lbu   v0,0x5c91(s1)    HALTE-ZAEHLER
        @0x800307cc  addiu v0,v0,-0x1
        @0x800307d0  sb    v0,0x5c91(s1)
        @0x800307d8  bne   v0,zero,default  ...0 -> FAELLT nach LAB_800307e0
```

Der zweite Ausgang läuft **ohne Tastendruck** ab und stopft das Pad-Vorwort **nicht**.
„Die Nachricht schließt nur auf die Bestätigungstaste" war also falsch — und genau darum
braucht der Port das Schließ-Bild in `re15_discard_frozen()`.

**Und die Brücke ist nicht wirkungslos** — Gegenprobe im selben Riegel-Lauf (TEIL I):
dieselbe Masken-Zählung **ohne** den `frozen()`-Ausweg findet über die 10 gefahrenen
Stellen **4 Spannen-Bilder ohne Pad-Bit**. Ohne die Zeile gäbe es also 4 freie Bilder;
ein `PRUEFE(bruecke_summe > 0, ...)` erzwingt, dass diese Zahl messbar bleibt, damit die
Masken-Prüfung darüber nicht sich selbst prüft.

Die Masken-Prüfung selbst steht je Stelle im Riegel: in **jedem** Bild der Spanne
„vorgemerkt + unsichtbar" müssen `0x01000000` **und** `0x02000000` stehen (oder das Bild
ist das Schließ-Bild) — 0 Verstöße über alle 10 Stellen, bei nachgewiesen nicht-leerer
Spanne (`PRUEFE(spanne > 0, ...)`).

### 2d. Was die SCD-Pad-Maske gefälscht hat — und wer der Geschädigte war

Runde 22 maskierte in `game_step_common.c` zusätzlich zu `g_re15_pauseflags` auch
`g_scd_pad_held`/`g_scd_pad_edge`, solange eine Abfrage vorgemerkt war. Diese beiden
Wörter sind aber die Quelle von `Sce_key_ck` (`0x51`/`0x52`). Der Geschädigte ist
namentlich benennbar und liegt im **selben Raum** wie das größte Loch — ROOM1100 sub01,
das Zahlenschloss:

```
0x00BB0  Sce_key_ck 51 01 01 00   -> Evt_exec sub 3   @0x00BB4
0x00BC8  Sce_key_ck 51 01 04 00   -> Evt_exec sub 4   @0x00BCC
0x00BE0  Sce_key_ck 51 01 02 00   -> Evt_exec sub 5   @0x00BE4
0x00BF8  Sce_key_ck 51 01 08 00   -> Evt_exec sub 6   @0x00BFC
0x00C1E  Sce_key_ck 51 01 40 00   -> Evt_exec sub 8   @0x00C22
0x00C46  Sce_key_ck 51 01 40 00   -> Evt_exec sub 2   @0x00C4A  (= die Benutzungsstelle)
```

Die geprüften Bits sind `0x01/0x02/0x04/0x08/0x40` — **alle** liegen unter `0xf000` und
wurden von der Maske weggeschnitten. Die Maske ist zurück auf `g_re15_pauseflags` allein;
den SPIELER-Pad hat sie ohnehin nie angefasst (`re15_player_tick(c->cam_view,
c->pad_current)` liest den rohen Wert).

### 2e. Die Port-Entscheidung „Raumwechsel" entfällt ersatzlos

Sie hing daran, dass der Spieler im Überhang einen Raum wechseln konnte. Den Überhang gibt
es nicht mehr, und in der verbliebenen Spanne sperrt die Maske `0xFFFF0000` Pad **und**
Skript — es kann also niemand mehr einen Raumwechsel auslösen. `scd_room_setup.c` ruft
deshalb wieder das schlichte `re15_discard_reset()`; `re15_discard_room_change()` und
`re15_discard_pad_locked()` sind **gelöscht**.

Das Save-Feld `discard_pending_item` bleibt stehen, ist aber jetzt beweisbar immer 0 (in
der Spanne kommt niemand an einen Speicherpunkt); `re15_discard_restore` bleibt als
**harter Reset** — ein Load darf keine Abfrage des vorigen Laufs erben.

---

## 3. Auftrag 3 — der Riegel, der rot werden KANN

### 3a. Warum der alte rot werden konnte: er konnte es nicht

Runde 22s TEIL G prüfte

```c
int pad_alt = !(g_re15_pauseflags & 0x01000000u);
int pad_neu = pad_alt && !re15_discard_pad_locked();
if (vorgemerkt && !sichtbar && pad_neu) loch_neu++;
```

mit `vorgemerkt = re15_discard_active()` und

```c
int re15_discard_active(void)     { return s_zustand != D_AUS; }   /* :357 */
int re15_discard_pad_locked(void) { return s_zustand != D_AUS; }   /* :360 */
```

Das ist `x && !sichtbar && pad_alt && !x` = **konstant 0**. Der Riegel stand grün, bevor
eine Zeile Fix geschrieben war. Dasselbe erklärt, warum ROOM1090 mit 20000 gesperrten
Bildern und `sichtbar=0` als Erfolg durchging.

### 3b. TEIL J — das px/pz-Delta hinter dem ECHTEN `re15_game_step`

Gemessen wird, was der Spieler spürt: die Summe |dx|+|dz| des Spielers **hinter**
`re15_game_step`, mit gehaltenem VORWÄRTS, im mit `re15_room_spawns[]` gebooteten Raum
(echter `scd_room_reenter`, echte Kollision, echtes Boden-Band). Drei Spalten im selben
Lauf:

```
Stelle                    JETZT   GEGENPROBE     STUB   wieder
ROOM1100 sub02 m4             0         1479        0     1479
ROOM1101 sub02 m4             0         1479        0     1479
ROOM11E0 sub21 m12            0            0        0        0
ROOM11E1 sub21 m12            0            0        0        0
ROOM3010 sub02 m1             0           85        0     2550
ROOM3011 sub02 m1             0           85        0     2550
ROOM3050 sub15 m5             0          150        0        0
ROOM3051 sub15 m5             0          150        0        0
ROOM4000 sub02 m2             0         2929        0     3030
Summe JETZT 0 / GEGENPROBE 6357 / STUB 0;
in 7 Stellen bewegte sich der Spieler im Ueberhang des Runde-22-Modells,
in 5 Stellen laeuft er nach der Antwort sofort wieder.
```

* **JETZT** = die Bilder mit vorgemerkter, unsichtbarer Abfrage im ausgelieferten Stand.
  `0` an jeder Stelle. Zusätzlich wird je Stelle geprüft, dass es diese Bilder überhaupt
  **gab** (`gesperrt > 0`) — sonst würde die Null nichts bedeuten.
* **GEGENPROBE** = dieselbe Strecke über die **Überhang-Spanne** aus TEIL I, gefahren ohne
  Vormerkung (damit das Skript nicht einfriert). `6357` Einheiten. Das ist das Loch in
  Einheiten, die der Spieler spürt.
* **STUB** = dieselbe Schleife **ohne** `re15_game_step`. `0` — damit ist belegt, dass die
  Null der ersten Spalte vom Stillstand kommt und nicht schon aus dem Messaufbau.
* **wieder** = 30 Bilder Vorwärts **nach** der Antwort. Wo `0` steht, hält den Spieler
  belegbar etwas anderes: `re15_cine_active()` (ROOM11E0/11E1 — das Szenen-Fenster
  `flag(2,7)` steht dort noch) oder `re15_pauseflags_belegt()` (ROOM3050/3051 — eine
  Folge-Nachricht, siehe die 83 „Msg2"-Bilder in TEIL I). Der Riegel verlangt genau das:
  `weg_wieder > 0 || re15_cine_active() || re15_pauseflags_belegt()` — nie die Abfrage.

Und je Stelle: `PRUEFE(b_frage >= 0, ...)` — **die Abfrage muss aufgehen**. Der
ROOM1090-Fall („20000 Bilder gesperrt, `sichtbar=0`") fällt damit durch, statt grün zu
zählen.

### 3c. TEIL K — der sichtbare Prompt friert das Skript ein, und es läuft weiter

Das ist die Stelle, an der Runde 21 sich geirrt hat. Dort stand: „weil ein sichtbarer
Prompt den SCD-Takt anhält, kam die Szene danach gar nicht mehr in Gang — sub03 stand auf
`@0x2508` fest". Das war ein Befund des **Prüfstands**, nicht des Spiels: dort wurde die
Frage nie beantwortet (getickt wurde mit `pad = 0`). Jetzt wird sie beantwortet:

```
ROOM1090 sub03 msg 9: Abfrage in Bild 72, Faden parkt 200 Bilder auf Datei-0x2507,
                      nach der Antwort laeuft er bis 0x259F und das
                      Szenen-Fenster @0x2508 geht auf.
```

Das Einfrieren selbst ist byte-true: RE2 legt beim Fragen sofort wieder `0xFF000000` an
(`@0x80051844` `lui v1,0xff00` / `@0x80051850` `sw v0,DAT_800cfbdc`), und darin steckt das
Skript-Bit `0x02000000` (Gate `@0x8003f044`). Das Unterprogramm parkt also genau an der
Anweisung, an der die Nachricht es gelassen hat.

### 3d. TEIL E — jetzt die Gegenrichtung

TEIL E prüfte früher „während einer Szene geht die Abfrage nicht auf". Das ist seit
Runde 23 **falsch** und wird jetzt umgekehrt geprüft:

```
Fire Extinguisher        ROOM1090 msg  9: waehrend der Nachricht zu,
                                          0 Bild(er) nach dem Belegt-Bit offen
                                          — MIT laufender Szene
Pliers                   ROOM11E0 msg 12: dito
Minidisc Player w/ Disc  ROOM1100 msg  4: dito
```

Offene Nachricht → Abfrage bleibt zu (und die Anzahl fällt noch nicht, `@0x80051810` liegt
hinter der Schranke). Nachricht ausgeredet → Abfrage geht auf, **egal** ob die Szene noch
läuft. Ein Loch entsteht dabei nicht: die Szene maskiert den Pad selbst.

---

## 4. Auftrag 4 — PSX nachgezogen

Befund: `platform/psx/` enthielt **keinen einzigen** Discard-Aufruf. `D_WARTET` wurde dort
also nie verlassen — die Abfrage kam nie, und der Pad-Riegel von Runde 22 hätte Pad und
Statusschirm **für immer** gesperrt. Drei Stellen plus ein Zeichner:

| Stelle | was |
|---|---|
| `platform/psx/main.c` (Spielschritt-Block) | `if (!re15_discard_frozen()) scd_vm_tick();` — derselbe Freeze wie im PC-Port |
| `platform/psx/main.c` (nach `re15_game_step`) | `re15_discard_tick(re15_pad_virtual_word(pad_pressed), re15_pad_virtual_word(pad_current))` — dieselbe Frame-Position, dieselben virtuellen Wörter |
| `platform/psx/main.c` (Textblock) | Prompt + Ja/Nein in derselben Box (34,180), Zahlen aus `msg_select_common.c` |
| `platform/psx/src/render.c` | `re15_render_item_prompt()` — der Glyphen-Stift über denselben SPRT-Zeichner wie der Untertitel-Text |

Der Prompt-Zeichner heißt jetzt auf **beiden** Plattformen `re15_render_item_prompt`
(Deklaration in `re15_engine.h`); der Skript-Lauf selbst war ohnehin geteilt
(`re15_item_prompt_walk`, Tabelle `@0x800C4FC6`).

⛔ **Ehrliche Grenze:** auf dieser Maschine gibt es keinen `mipsel-none-elf-gcc`
(Memory `reai-v2-psx-build-gap`), der PSX-Bau ist hier also **nicht** gefahren. Geprüft
ist stattdessen eine vollständige **Syntax-/Deklarations-Prüfung** beider geänderter
PSX-Dateien gegen die echten PSn00bSDK-Header aus
`info/Resident_Evil_und_Playstation_Information/PSn00bSDK-master/`:
aus `re15_port/` kommt **keine** Meldung; die zwei verbleibenden `error:`-Zeilen sind
vorbestehend und nicht aus diesem Batch (`psxapi.h:260 _get_errno` = SDK gegen Host-libc,
`psx/main.c:466 DrawSync` = fehlende Deklaration in dieser SDK-Kopie).

---

## 5. Auftrag 5 — harter Reset auf Game-Over und Neustart

1. **`re15_gameover_fsm_tick` vor die Freeze-Returns gehoben** (`game_step_common.c`). Der
   Beleg stand schon in derselben Datei: der Hauptloop ruft sie **unbedingt** und **vor**
   dem Spieler-Dispatcher —
   ```
   8001cdfc  jal 0x8001500c      GAME-OVER-FSM
   8001ce0c  jal 0x80031c44      Spieler-Dispatcher
   ```
   — und `FUN_8001500c` hat **kein** Pause-Gate (wortweiser EXE-Scan nach
   `lw rX,-13760(rY)`, der einzigen Ladeform von `0x800aca40`: genau 9 Leser —
   `0x800144a4`, `0x80019e28`, `0x8001cbcc`, `0x8001cc9c`, `0x8001cd14`, `0x8001cdd8`,
   `0x800304f4`, `0x80031c54`, `0x8003f040` — **keiner** in
   `0x8001500c`..`0x80015840`). Sie stand bisher **unter** den beiden Freeze-Returns; ein
   Item-Modal oder eine sichtbare Wegwerf-Abfrage hätte den Todes-Ablauf eingesperrt.
2. **`re15_gameflow_to_gameover()` und `re15_gameflow_to_title()`** räumen jetzt
   (`re15_discard_reset()`), ebenso **`re15_gameflow_new_game()`**.
3. Der Game-Over-Pfad in `platform/pc/main.c` setzte `g_gameflow.mode = RE15_MODE_TITLE`
   direkt hinter zwei `memset` (`g_inv`, `g_game`) — die Abfrage-FSM ist kein Feld von
   beiden und ritt deshalb in den nächsten Lauf, wo sie die erste echte Abfrage
   verschluckte (`notice_message` kehrt bei `s_zustand != D_AUS` sofort um). Dort steht
   jetzt `re15_gameflow_to_title()`.

---

## 6. Auftrag 6 — die Text-Berichtigungen

| Urteil | wo es stand | jetzt |
|---|---|---|
| „die Nachricht schließt nur auf die Bestätigungstaste" | `item_discard_common.c` (in `re15_discard_room_change` / `re15_discard_pad_locked`, beide gelöscht) | neu und vollständig an `re15_pauseflags_belegt()`: **zwei** Eingänge in `LAB_800307e0`, der zweite ist der Halte-Zähler `@0x800307c4`-`@0x800307d8` und braucht **keine** Taste |
| „die HAUPTSCHLEIFE ruft sie JEDES Bild" | `platform/pc/main.c`, `platform/psx/main.c` | der Aufruf liegt auf dem regulären Spielpfad und ist **gegatet**: `@0x800262ac lh v0,-0x79b6(at)` (Halbwort `0x800d4832`) / `@0x800262b4 bgez v0,LAB_8002632c` — negativ endet der Zweig `0x800262bc`..`0x80026328` mit `j LAB_80026738` und **überspringt** die Fortsetzung; dahinter können `@0x80026338`/`@0x80026348`/`@0x80026358` ebenfalls abbiegen. Erst `LAB_80026364` lädt (`@0x80026370`) und ruft (`@0x80026384`). |
| „byte-true dieselbe Spanne" | mehrere Kommentare | **gestrichen.** Übernommen ist die **Regel** (die Abfrage wartet auf genau dieses eine Bit), nicht die Bildzahl: die Spanne hängt am Port-Nachrichtensystem (eigene FSM, eigene Standzeiten, eigener Untertitel-Nachhall). |

---

## 7. Live-Abzug — ROOM1100, echter Renderpfad

`analysis/befunde_2026-09-22/discard-fenster/abzug_1100.sh`. `RE15_FRAMEDUMP` liest das
komplett komponierte Bild **innerhalb** von `re15_render_end_frame()` unmittelbar **vor**
`SDL_RenderPresent` — kein `RE15_AUTOSHOT`, kein Softwarerenderer.

Kalibrierung, gemessen: `RE15_INPUT_SCRIPT_START` zählt **gerenderte Bilder aller Modi**
(`input_pc.c` `s_input_ticks`, Titel eingeschlossen), die Messschiene und
`RE15_FORCE_EVENT` zählen `g_engine.frame_count`. Eigener Kalibrierlauf (START=150,
Skript `"U8"`): der Spieler setzt sich in Bild **F27** in Bewegung → Versatz **123**.
Für „Vorwärts ab dem Zündbild 300" steht START deshalb auf 423.

Protokoll (`bild/discard.log`, jede Zeile ein Bild):

```
F301  raum=1100 msg_aktiv=1 msg_fsm=1 | abfrage=1 frage=0 ... belegt=1 frost=0
      pausepad=1 px=-20615 pz=-25000
F421  raum=1100 msg_aktiv=0 msg_fsm=0 | abfrage=1 frage=8 gegenstand=0x44 wahl=0
      text=0/44 gefragt=1 weg=0 belegt=0 frost=1 pausepad=1 px=-20615 pz=-25000
F466  ... text=44/44 ...
F480  ... abfrage=0 frage=0 ... gefragt=1 weg=1 ... px=-20615 pz=-25000
```

* **120 Wartebilder** (F301..F420), davon **0** mit freiem Pad.
* `px/pz` bleiben über das **ganze** Wartefenster auf `-20615/-25000` — und in diesen
  Bildern wird **vorwärts gehalten** (Skript-Phase 1, `U4`).
* F421: das Belegt-Bit fällt (`belegt=0`) und die Abfrage steht im **selben** Bild
  (`frage=8`, `frost=1`). Kein freies Zwischenbild.
* F480: Antwort Ja, `weg=1`.

| Bild | was zu sehen ist |
|---|---|
| `bild/bild000416.png` | Wartefenster: „You've used the Minidisc Player w/ Disc.", Spieler steht, obwohl vorwärts gedrückt wird |
| `bild/bild000424.png` | Abfrage geht auf, Schreibmaschine bei „You d…" |
| `bild/bild000476.png` | **„You don't need this key any more. Discard it?"** ▶ Yes    No |

⛔ **Ehrlich zur Zündung:** die echte Zündung von sub02 steht in sub01 `@0x00C4A`
`Evt_exec cond=0xFF type=0x18 sub=2` und hängt an `Ck(5,4,1)` `@0x00C30`,
`Member_cmp member=0x0F == 5` `@0x00C3C` (Stellung des Zahlenschlosses) und
`Sce_key_ck 0x40` `@0x00C46`. Ein Abzug, der das Schloss per Steuerkreuz auf Kerbe 5
dreht, ist nicht deterministisch fahrbar; gefahren wird deshalb das **ausgelieferte**
sub02 durch denselben Ereignis-Dispatcher, den das Skript benutzt (`scd_event_fire`,
`RE15_FORCE_EVENT=2@300`). Am Skript ist nichts geändert.

---

## 8. Was steht, unverändert

* **Wortlaut RE1.5** `@0x800C508B` (DEBUG.BIN, Skript [6] von 8 der Tabelle `@0x800C4FC6`),
  Ja-Aktion `0x02` = wegwerfen `@0x800C50BB`, Ja vorbelegt.
* **Auswahl-Darstellung byte-true:** Zeile 196 (`@0x80027f14` + `@0x80028630`/`@0x80028674`),
  Yes 174 (`@0x80028680`), No 244, Cursor 160/230 (`@0x80028650`), Blink-Maske `0x18`
  `@0x80028600`, Zähler −1/Bild `@0x800285f0` — alle Zeichen-Stellen holen aus
  `engine/src/msg_select_common.c`.
* **Mechanismus RE2:** Verbrauchszähler im Inventarplatz (`@0x800D4A3C`), Dekrement
  `@0x80051810` hinter der Warte-Schranke `@0x800517f4`, Frage bei 0 `@0x80051834`;
  Ja: Platz genullt `@0x80051774`/`@0x80051794`; Nein: Anzahl := 1 `@0x800517C4`.
* **Sackgasse strukturell ausgeschlossen:** `Keep_Item_ck` (`0x5E`) kommt in 40674 Opcodes
  über 206 RDTs **0 mal** vor (TEIL C, unverändert).
* **Ghidras XREF-Liste ist unvollständig** — zu `DAT_800d4498` nennt sie fünf
  Schreibstellen, die sechste (`@0x800524e8`) adressiert über `s1` (`@0x800524e4
  addu at,s1,at`) und ist unbeschriftet. Gefunden mit einem Byte-Muster-Zensus
  (`b0 82 ?? ac` und `98 44 ?? ac`). Steht hier, weil die Lehre bleibt, auch wenn die
  Datenlage für die Port-Entscheidung nicht mehr gebraucht wird.

---

## 9. Offen, benannt

* **Abbrechen mit CROSS = „No"** bleibt Port-Konvention; RE2s Bestätigen-Taste
  (`FUN_80030844`, `DAT_800ce310 & 0x1000`, vgl. `@0x800307a8` mit Maske `0x3000`) ist
  nicht bis zur Tastenzuordnung aufgelöst.
* **Das Blinken** ist im Live-Bild nicht abgelichtet (Aus-Phase 8 von 32 Takten) — nur im
  Riegel gepinnt (TEIL D, über alle 256 Zählerstände).
* **6 der 16 Benutzungsstellen** kann TEIL I/J nicht selbst fahren: die Kartenleser
  (ROOM10D0/10D1/1230/1231, ROOM11E0/11E1 msg 9) hängen ihre Nachricht hinter
  `Ifel_ck`/`Ck(12,31,0)` — die Ja-Antwort auf die Frage davor (ROOM10D0 sub20 `@0x19B8`/
  `@0x19BC`). Ohne echte Spieler-Antwort geht der Zweig nach Else. Sie werden **mit Grund**
  ausgelassen und zählen nicht still als grün.
* **PSX ist hier nicht gebaut** (kein mipsel-Compiler auf dieser Maschine) — nur
  syntax-/deklarationsgeprüft, siehe §4.
* **Zone 0 (`DAT_800aca38`) und Zone 1 (`DAT_800aca3c`)** sind im Port weiterhin nicht an
  die echten Engine-Wörter gebunden (Zone 2 ist es). Kein gemessener Defekt hängt dran.
