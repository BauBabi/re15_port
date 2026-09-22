# Die Riegel der Wegwerf-Abfrage — vier neue, jeder mit seinem eigenen Rückbau

Runde 25, 2026-09-22. Vorgänger: `discard-besitz.md` (Runde 24, jetzt mit
Berichtigungs-Banner), `discard-fenster.md` (23), `discard-nacharbeit.md`,
`discard-loch.md` (22), `discard-umsetzung.md` (Bau).

Suite: **337/337** grün — `bash re15_port/tools/local_build.sh all` →
`=== LOCAL-BUILD-OK (all) — Tests 337/337`, Testlaufzeit 196 s.
Die Zahl bleibt 337, weil die neuen Riegel **Teile des bestehenden** Prüfprogramms
`unit_r21_discard_wegwerfen` sind (ein Testziel, jetzt 13 Teile: A/B, C+C1–C7, D, E, **O**,
**P**, **Q**, L, N, M, I, J, K) — kein neues `add_test`, also auch keine Anhebung von
`RE15_MIN_TESTS`.

---

## 0. Kurzfassung

| | |
|---|---|
| **Der Sperrgrund** | Nicht das Verhalten. Fünf Rückbauten machten Riegel rot, **einer nicht**: die *Hauptbehauptung* der Runde 24 — die fail-closed-Kopplung — stand in **keinem** Riegel und war vollständig zurückbaubar (`exit 0`). Dazu: `TEIL M` grün bei „Nachhall-Bilder 0" (dem Zustand, den sein eigener Kommentar wertlos nennt), `re15_discard_restore` ohne jeden Riegel, keine Abdeckungs-Schranken. |
| **Gebaut** | `TEIL O` (fail-closed, 16 Stellen × 6 Fälle), `TEIL P` (`restore`, 16 × 4), `TEIL Q` (JA-Zweig nach Kompaktierung, 16 × 2), `TEIL M` zweiter Lauf ohne Besitz, `PRUEFE(gefahren >= 10)` in `TEIL L`, `PRUEFE(erreicht)` in `TEIL N`, `TEIL C7` (die anderen zwei Flag-Opcodes). |
| **Jeder Riegel einzeln zurückgebaut** | **6 von 6** neuen Prüfungen werden rot, wenn die Sache, die sie behaupten, ausgebaut wird — und bei den Schranken zusätzlich gezeigt, dass **nur sie** den Defekt fangen (mit Schranke rot, ohne Schranke `exit 0`). Werkzeug: `discard-riegel/rueckbau.sh`. |
| **Zwei Code-Änderungen** | `anzahl_zurueck()` zieht den Platz nach (die Id entscheidet); der JA-Zweig steigt fail-closed aus, wenn der Platz den Gegenstand nicht mehr trägt (`@0x800517C4`). Beide mit Riegel `TEIL Q` samt Gegenprobe. |
| **Drei Textzeilen berichtigt** | „es gibt keinen Überhang mehr zu sichern" (Code), „das einzige Tor `Ck(3,32,0)`" (Dossier), „einen Schreiber und keinen Löscher" (nur über `Set` gemessen). |
| **Werkzeug** | `discard_verlustwege.py` gibt jetzt den **Wiederöffner** mit Raum + Unterprogramm + Offset aus, seine **Nebenwirkungen** samt eigener Umkehrbarkeit, und die **Rufkette**. Damit steht das ROOM1090-Dreiglied und `flag(3,129)` `@0x1954` im Werkzeug. |

---

## 1. Die Regel, gegen die geprüft wird — RE2s Tür-Handler

Aus `ghidra_re2_Leon.txt`, unverändert gegenüber Runde 24, hier nur die vier Zeilen, auf
die sich `TEIL O` stützt:

```
80051628  jal   FUN_800696cc          ; Inventarplatz des Schluessels SUCHEN
80051630  move  s1,v0                 ;   s1 = gefundener Platz, < 0 = kein Treffer
80051634  bltz  s1,LAB_800516a0       ; KEIN TREFFER -> anderer Zweig
--- Treffer-Zweig ---
8005164C  jal   FUN_8002fe38          ; ERST JETZT die Nachricht oeffnen
80051670  sw    v0=>LAB_80051718,-0x7d50(at)   ; und DANACH die Fortsetzung EINHAENGEN
--- Nicht-Treffer-Zweig ---
800516B4  addiu a2,s0,-0x4c           ; eine ANDERE Nachrichten-Id
800516B8  jal   FUN_8002fe38          ; andere Zeile oeffnen
800516C0  j     LAB_800516f8          ; return - NICHTS eingehaengt
```

Der entscheidende Satz steht in `@0x800516C0`: wer an `@0x80051628` **vorbei**kommt,
hängt in RE2 keine Fortsetzung ein und kann deshalb nie eine Abfrage auslösen. Genau das
ist die fail-closed-Kopplung im Port — und genau das war bis Runde 25 unbelegt.

---

## 2. `TEIL O` — die fail-closed-Kopplung, gefahren statt behauptet

Sechs Fälle je Benutzungsstelle, alle über die Kopplung selbst (ohne RDT — das *ist* der
Punkt: ein fremder Öffnungsweg ruft `op_message_on` eben nicht):

| Fall | Aufbau | Verlangt | Gemessen |
|---|---|---|---|
| **O1 Direktaufruf** | Gegenstand im Inventar, `re15_discard_notice_message` **ohne** jeden Vorentscheid | nichts vorgemerkt, Anzahl unberührt | 16/16 |
| **O2 fremde Stelle** | Vorentscheid **mit Treffer** für eine *andere* Stelle, dann `notice` für diese | nichts | 16/16 |
| **O3 fremde Nachricht** | Vorentscheid für (derselbe Raum, Nachricht die keine Stelle ist) | nichts | 16/16 |
| **O4 ohne Besitz** | Vorentscheid für die richtige Stelle, Inventar leer (`@0x80051634`) | nichts | 16/16 |
| **O5 POSITIVFALL** | Vorentscheid mit Treffer **+** `notice` derselben Stelle | **vorgemerkt**, Anzahl noch 1 | 16/16 |
| **O6 verbraucht** | nach O5 ein zweites `notice` ohne neuen Vorentscheid | nichts | 16/16 |

**O5 ist die Gegenprobe.** Ohne ihn stünde der ganze Teil auch dann grün, wenn
`re15_discard_notice_message` nie etwas vormerkt — also wenn das Merkmal ganz ausgebaut
ist. Und O5 prüft zusätzlich, dass die Anzahl beim Einhängen **nicht** fällt
(`@0x800517f4` verlässt die Routine vor dem Dekrement `@0x80051810`).

**Rückbau R-A** (`rueckbau.sh A`): die Kopplung aus, `notice_message` sieht die Stelle
selbst in `discard_sites.inc` nach (das ist die Runde-23-Form). Ergebnis:

```
EXIT=1
FEHLER: O1 ROOM1090 msg 9: der DIREKTAUFRUFER hat vorgemerkt (0x31) - die
        fail-closed-Kopplung greift nicht (Vorbild @0x800516C0)
FEHLER: O2 ROOM1090 msg 9: ein Vorentscheid fuer ROOM10D0 msg 9 hat DIESE Stelle armiert
FEHLER: O3 ROOM1090: ein Vorentscheid fuer msg 0 hat msg 9 armiert
FEHLER: O6 ROOM1090 msg 9: der verbrauchte Vorentscheid hat ein zweites Mal armiert
FEHLGESCHLAGEN: 68 Pruefungen
```

Damit ist der eine Rückbau, der in Runde 24 `exit 0` lieferte, jetzt rot — und zwar 68×.

---

## 3. `TEIL P` — ein Laden belebt nie eine Abfrage

Pflicht-Korrektur 5 der Vorrunde: `re15_discard_restore` hatte **gar keinen** Riegel. Bis
Runde 23 stellte die Funktion bei vorhandenem Gegenstand wieder auf `D_WARTET` — ein
Wiederbeleben ohne Beleg. Sie ist seither ein harter Reset; das ist jetzt gemessen:

| Fall | Ausgangszustand | Verlangt | Gemessen |
|---|---|---|---|
| **P1** | `D_WARTET` (vorgemerkt, Anzahl 1) | nichts aktiv, Anzahl bleibt 1 | 16/16 |
| **P2** | `D_FRAGT` (Prompt sichtbar, Anzahl auf 0 `@0x80051810`) | nichts aktiv, Anzahl **zurück auf 1** (`@0x800517C4`) | 16/16 |
| **P3** | `D_AUS`, Gegenstand im Inventar | `restore(item)` erzeugt **nichts** | 16/16 |
| **P4 GEGENPROBE** | dieselbe Vorgeschichte, **kein** `restore` | die Abfrage **steht** | 16/16 |

P4 ist nötig, weil P1/P2 sonst auch dann grün wären, wenn überhaupt nie etwas vorgemerkt
würde. Zusätzlich prüft P2 vorher, dass die Anzahl beim offenen Prompt wirklich 0 ist —
sonst wäre „wieder 1" keine Aussage.

**Rückbau R-C** (`restore` auf die Runde-23-Form): `EXIT=1`, **51 Prüfungen rot**, u.a.

```
FEHLER: P1 ROOM1090 msg 9: das Laden hat die Abfrage BELEBT
FEHLER: P2 ROOM1090: der Prompt steht nach dem Laden weiter
FEHLER: P3 ROOM1090: restore(0x31) hat aus dem Nichts eine Abfrage erzeugt
        - genau das Wiederbeleben von Runde 23
```

---

## 4. `TEIL Q` — der JA-Zweig nach einer Kompaktierung

### Der Befund

Beim Übergang auf die Abfrage fällt die Anzahl auf 0 (`@0x80051810 addiu v0,v0,-1`).
Weggeworfen wird im JA-Zweig aber nur, wenn der **gemerkte Platz** den Gegenstand noch
trägt. `re15_inv_remove_slot` kompaktiert das Inventar (`FUN_8004dadc`) — ein gemerkter
*Index* kann danach auf einen fremden Platz zeigen. Dann fiel der Zweig durch, **ohne** die
Anzahl zurückzudrehen: der Gegenstand blieb mit **Anzahl 0** im Inventar liegen, sichtbar
und unbenutzbar.

### Was geändert wurde (zwei Zeilen Sorte „fail-closed")

1. `anzahl_zurueck()` zieht den Platz nach — **die Id entscheidet, nicht der Index**.
   Dasselbe Argument liegt schon zweimal in derselben Datei (beim Einhängen und beim
   Fragen), mit derselben Begründung (`FUN_8004dadc` kompaktiert).
2. Der JA-Zweig steigt über `re15_discard_reset()` aus, wenn nichts weggeworfen wurde.
   Vorbild: RE2 schreibt die 1 in **jedem** Zweig zurück, der nicht wegwirft —
   `@0x800517C4 sb v1,count` mit `v1 = 1`.

Dass der Fall im Spiel nicht auftreten *soll* (der sichtbare Prompt friert die Welt ein,
`@0x80051844 lui v1,0xff00` / `@0x80051850 sw v0,DAT_800cfbdc`), ist ausdrücklich **kein**
Grund, ihn offen zu lassen: dieser Freeze ist eine **Port**-Zeile (`re15_discard_frozen`),
kein Original-Zustand.

### Gefahren

Zwei Plätze, der Gegenstand auf dem hinteren; nach dem Aufgehen der Abfrage wird der
vordere Platz geleert, die Kompaktierung zieht den Gegenstand nach vorn, der gemerkte
Index zeigt ins Leere — dann JA.

| | Verlangt | Gemessen |
|---|---|---|
| **Q1** | kein Platz trägt den Gegenstand mit Anzahl 0 | 16/16 |
| **Q2 GEGENPROBE** | ohne die Verschiebung wirft JA wirklich weg | 16/16 |

**Rückbau R-E** (JA-Zweig ohne `anzahl_zurueck`): `EXIT=1`, **17 Prüfungen rot**:

```
FEHLER: Q1 ROOM1090 msg 9: 0x31 liegt auf Platz 0 mit ANZAHL 0 - ein Gegenstand, den der
        Spieler sieht und nicht benutzen kann (@0x800517C4 schreibt die 1 zurueck)
```

---

## 5. `TEIL M` — der zweite Lauf, ohne den die 0 nichts wert war

`TEIL M` meldete „Nachhall-Bilder 0, UEBERLAPP 0" und war grün. Eine 0 hat aber **zwei**
Ursachen: der Nachhall wurde beendet (das *will* der Riegel zeigen) **oder** er ist im
Aufbau nie entstanden (dann zeigt er nichts). Der eigene Kommentar des Teils sagte das
sogar — nur gab es keinen Lauf, der die beiden trennt.

Jetzt zwei Läufe im selben Raum (ROOM4000 sub02 msg 2), **ein** Unterschied — der Besitz:

```
LAUF 1 MIT  Besitz: Prompt-Bilder 121, Nachhall-Bilder 0, UEBERLAPP 0
LAUF 2 OHNE Besitz: Prompt-Bilder   0, Nachhall-Bilder 4   (GEGENPROBE)
```

Ohne Besitz geht keine Abfrage auf, also beendet auch nichts den Nachhall — er entsteht
und ist zählbar. Erst damit heißt die 0 aus Lauf 1: *beendet*.

| Rückbau | Ergebnis |
|---|---|
| **R-B1** — die erzwungene Aufnahme im 2. Lauf weg (Nachhall-Bilder 0) | `EXIT=1`: „der Aufbau erzeugt GAR KEINEN Nachhall (0 Bilder im Lauf ohne Abfrage) — dann ist die 0 aus Lauf 1 wertlos" |
| **R-B1 + F3** — derselbe Defekt, aber die neue Schranke entfernt | `EXIT=0` — **nicht rot**. Die Schranke ist also das, was den Defekt fängt. |
| **R-B2** — `re15_msg_nachhall_beenden()` weg | `EXIT=1`: „4 Bilder Nachhall-Ueberlapp" (`@0x800307e8`/`@0x800307f4` sind ein Paar, `@0x80051834` = `@0x8005164C`) |

R-B1 ist der schärfste Befund dieser Runde: **der Zustand, in dem der alte Riegel grün
war, macht den neuen rot.**

---

## 6. Abdeckungs-Schranken — und der Beweis, dass nur sie greifen

Zwei Zeilen, `PRUEFE(gefahren >= 10)` in `TEIL L` und `PRUEFE(erreicht)` in `TEIL N` (a).

Der Einwand „das fängt doch schon `mit_abfrage == gefahren`" trifft **nicht**: die
Gleichheit bleibt wahr, wenn beide Zahlen *zusammen* fallen. Gemessen:

| Rückbau | Ergebnis |
|---|---|
| **R-D1** — der Prüfstand erreicht in `TEIL L` nur noch 8 von 16 Stellen | `EXIT=1`: „ABDECKUNG: nur 8 von 16 Stellen erreicht (erwartet >= 10)" — **eine** Prüfung rot |
| **R-D1 + F** — derselbe Defekt, Schranke entfernt | `EXIT=0` — **nicht rot** |
| **R-D2** — der OHNE-Besitz-Lauf von `TEIL N` erreicht die Stelle nicht mehr | `EXIT=1`: „ROOM4000: die Benutzungsstelle wurde im OHNE-Lauf gar nicht erreicht — dann prüft (a) nichts" |
| **R-D2 + F2** — derselbe Defekt, Schranke entfernt | `EXIT=0` — **nicht rot** |

Das Paar „Defekt rot / Defekt ohne Schranke grün" ist der einzige Nachweis, der bei einer
Abdeckungs-Schranke etwas taugt: er zeigt, dass **sie** der Riegel ist und nicht eine
andere Prüfung, die zufällig mitfeuert.

Die 10 bleibt die gemessene Zahl: 10 von 16 Stellen sind im Prüfstand erreichbar, die
anderen 6 hängen an AOTs/Kartenlesern, die er nicht betritt — sie stehen mit Grund in der
Tabelle von `TEIL L`.

### Und ein Gegencheck auf die ALTEN Riegel

**Rückbau R-G** (`re15_discard_frozen()` gibt immer 0 = Rückbau R4 der Vorrunde):
`EXIT=1`, **30 Prüfungen rot**, u.a. „ROOM1090 msg 9: 1 von 71 Spannen-Bildern OHNE
Spieler-Bit 0x80000000 (`@0x80031c78`)". Die Änderungen dieser Runde haben `TEIL I`/`J`
also nicht stumpf gemacht.

---

## 7. Die drei berichtigten Texte

### 7.1 `item_discard_common.c` — „es gibt keinen Überhang mehr zu sichern"

Der Satz stand noch **im Code**, 120 Zeilen über seiner eigenen Widerlegung
(`re15_discard_frozen()` weiter unten in derselben Datei). Er ist durch das ersetzt, was
gemessen ist:

> Aus **161 Bildern** Überhang (ROOM1100/ROOM1101, Runde-22-Modell) ist **genau 1 Bild je
> Stelle** geworden — das Schließ-Bild, in dem `re15_msg_tick` den Freeze schon gelöst und
> `re15_discard_tick` die Abfrage noch nicht aufgemacht hat. Gehalten wird es von der
> **Port**-Zeile `re15_discard_frozen()`, gemessen je Stelle von `TEIL I`
> (`ohne_bruecke == 1` an 10 von 10 Stellen) und in seiner Wirkung von `TEIL J` (Rückbau
> dieses einen Bildes: 680 Einheiten Bewegung). **RE2 hat hier 0 solche Bilder**, weil
> Dekrement und neuer Freeze im selben Aufruf liegen: `@0x80051810 addiu v0,v0,-1`,
> `@0x80051834 jal FUN_8002fe38`, `@0x80051844 lui v1,0xff00` / `@0x80051850
> sw v0,DAT_800cfbdc`.

### 7.2 `discard-besitz.md` — „das einzige Tor `Ck(3,32,0)`"

Zeile 22 widersprach Zeile 187 desselben Dossiers („`Ck(3,32,0)` ist NICHT das einzige
Tor"). Richtig ist: es ist das einzige **endgültig geschlossene** Tor. Auf dem Pfad zur
Stelle liegen **fünf** (`tools/discard_verlustwege.py`):

```
ROOM4000 sub02  Message_on 2  (Gegenstand 0x47)
  TORE      : Ck(3,32,0)@0x0142A, Ck(12,31,0)@0x01446, Ck(3,101,1)@0x01374,
              Ck(5,0,0)@0x01382, Ck(5,1,0)@0x01386
  ENDGUELTIG: Ck(3,32)@0x0142A wird nirgends wieder geoeffnet
```

Zeile 22 ist berichtigt; das Dossier trägt jetzt oben ein Berichtigungs-Banner.

### 7.3 `C6` zählte nur `Set` (0x22) → neuer `TEIL C7`

Der Satz „das Flag hat im ganzen Bestand **einen** Schreiber und **keinen** Löscher" war
über **einen** Opcode gemessen. RE1.5 hat aber drei Flag-Opcodes:

| Opcode | Rolle | Beleg | Vorkommen im Bestand |
|---|---|---|---|
| `0x22` Set | Schreiber, Zone+Index statisch | — | 2973 (davon `flag(3,32)`: 1 setzen / 0 löschen) |
| `0x59` Flag-Set2 | **zweiter Schreiber**, indiziert | `LAB_8003fe90` → `@0x8003ff3c or` / `@0x8003ff24 nor+and` / `@0x8003ff50 xor`; Bank statisch `pc[1]` (`@0x8003fed0`), **Index erst zur Laufzeit** aus `work_vars[pc[2]]` (`lhu @0x8003feb8`) | **2**, beide **Bank 5** → 0 auf Bank 3, 0 auf Bank 9 |
| `0x58` Flag-Ck2 | **nur Leser** | `LAB_8003fd54`, nur `and`, kein Store (`op_flag_ck2` ruft nur `re15_game_flag_get`) | **0** |

Weil der Index von `0x59` statisch nicht bestimmbar ist, wird die **Bank** gezählt: trifft
kein einziges `0x59` die Bank 3 bzw. 9, kann darin auch kein Index etwas schreiben. Die
**2** ist zugleich die Gegenprobe, dass der Zähler den Opcode überhaupt sieht — eine 0
hätte auch „Zähler blind" bedeutet. Damit tragen `C2` (kein zone-9-Bit wird gelöscht) und
`C6` (ein Schreiber für `flag(3,32)`) jetzt über **alle** Flag-Opcodes der Maschine.

```
C6 Schreiber von flag(3,32) per Set (0x22): 1 x setzen, 0 x loeschen/umschalten
C7 Flag-Set2 (0x59): 2 Vorkommen, davon Bank 3: 0, Bank 9: 0 | Flag-Ck2 (0x58, nur LESER): 0
```

---

## 8. Das ROOM1090-Dreiglied — jetzt im Werkzeug

Gemessen mit dem Längen-Vorschub des Motors (`scd_opcode_size_at` bzw.
`discard_zensus.op_size`), Datei-Offsets:

```
sub00 @0x022A6  Ck  21 03 84 01   Ck(3,132,1)    das Tor
sub00 @0x022E0  Evt_exec 04 ff 18 03             und der Aufruf dahinter
sub03 @0x024CE  Set 22 03 84 00   Set(3,132,0)   loescht das Tor als ERSTE Anweisung
sub03 @0x02502  Message_on 2b 09 ff ff           die Benutzungsstelle (Gegenstand 0x31)
sub06 @0x02712  Ck  21 0c 1f 00   Ck(12,31,0)    die Ja-Antwort des Spielers
sub06 @0x0271E  Set 22 03 81 01   Set(3,129,1)
sub06 @0x02722  Set 22 03 84 01   Set(3,132,1)   setzt das Tor WIEDER
```

`sub06` hat **keinen** `Evt_exec`/`Gosub`-Aufrufer — es hängt an einem AOT, den `sub00`
nur im Zweig `Ck(3,129,0)` `@0x02332` installiert:

```
0x02336  Aot_set  2c 02 03 b1 01 00  64 00 14 05 3a 07 66 08
                  de 0d 30 f8 08 07 10 f5  ff 00 18 06 00 00     (LANGE Form)
                                           ^^^^^^^^^^^  Nutzlast {0xFF,0x18,sub}
```

`pc[25] = 0x06` ist die `eventId` und damit `sub06` (Lange Form: `ev = pc[25]`,
`scd_vm.c:2823`; dieselbe Nutzlast-Form wie ROOM1150 `@0x0D7E` mit `… ff 00 18 04`).
`sub06` nimmt sich mit seinem eigenen `Set(3,129,1)` `@0x0271E` also **selbst vom Netz**.
Ob die Stelle damit wiederholbar ist, hängt an der **Umkehrbarkeit von `flag(3,129)`**,
und die steht in einem anderen Raum:

```
ROOM10B1 sub02 @0x01832  Set(3,129,1)   setzt
ROOM10B1 sub03 @0x01954  Set(3,129,0)   LOESCHT   <== der Umkehrer
```

`discard_verlustwege.py` gibt das jetzt aus, statt nur „es gibt einen Wiederöffner":

```
ROOM1090 sub03  Message_on 9  (Gegenstand 0x31)
  TORE      : Ck(3,132,1)@0x022A6
  TOETEND   : Set(3,132,0)@0x024CE (vor der Nachricht) schliesst Ck@0x022A6
  ENDGUELTIG: keins - jedes geschlossene Tor hat einen Wiederoeffner
  WIEDEROEFF: flag(3,132) von ROOM1090 sub06@0x02722, flag(3,132) von ROOM11B1 sub02@0x011EC
  NEBENWIRK : ROOM1090 sub06@0x02716 Set(2,7,1)   -> umkehrbar (153 Stellen): ...,
              ROOM1090 sub06@0x0271E Set(3,129,1) -> umkehrbar (1 Stellen): ROOM10B1 sub03@0x01954,
              ...
  RUFKETTE  : sub00@0x022E0 ruft sub03
```

Neu im Werkzeug: `WIEDEROEFF` (Raum + Unterprogramm + Offset), `NEBENWIRK` (was derselbe
Wiederöffner sonst noch schreibt, je mit eigener Umkehrbarkeit — Liste auf drei Beispiele
gekappt, weil Zonen wie `flag(2,7)` dreistellig viele Schreiber haben und dann die
**Länge** die Auskunft ist), `RUFKETTE` (bis drei Ebenen hoch) und eine Summenzeile
„Stellen, deren Wiederöffner eine **nicht** umkehrbare Nebenwirkung hat: 3" (ROOM20A0/20A1
`sub01` setzen `flag(3,170/171/204)` dauerhaft — ausgewiesen, nicht als Defekt behauptet:
diese Flags sind keine Tore der betroffenen Stellen).

---

## 9. Rückbau-Tafel (alles selbst gefahren)

| Rückbau | Was ausgebaut wird | Riegel | Ergebnis |
|---|---|---|---|
| **R-A** | die fail-closed-Kopplung in `notice_message` | `TEIL O` | `EXIT=1`, **68** Prüfungen rot (O1/O2/O3/O6 je Stelle) |
| **R-B1** | die erzwungene Aufnahme im 2. M-Lauf → Nachhall 0 | `TEIL M` | `EXIT=1`, 1 Prüfung rot |
| **R-B1+F3** | derselbe Defekt **+** Schranke entfernt | — | `EXIT=0` — die Schranke ist der Riegel |
| **R-B2** | `re15_msg_nachhall_beenden()` | `TEIL M` | `EXIT=1`, Überlapp **4** Bilder |
| **R-C** | `restore` zurück auf die Runde-23-Form | `TEIL P` | `EXIT=1`, **51** Prüfungen rot |
| **R-D1** | Prüfstand erreicht in `TEIL L` nur 8 von 16 | `TEIL L` | `EXIT=1`, 1 Prüfung rot |
| **R-D1+F** | derselbe Defekt **+** Schranke entfernt | — | `EXIT=0` |
| **R-D2** | OHNE-Lauf von `TEIL N` erreicht die Stelle nicht | `TEIL N` | `EXIT=1`, 1 Prüfung rot |
| **R-D2+F2** | derselbe Defekt **+** Schranke entfernt | — | `EXIT=0` |
| **R-E** | JA-Zweig ohne `anzahl_zurueck` | `TEIL Q` | `EXIT=1`, **17** Prüfungen rot |
| **R-G** | `re15_discard_frozen()` → 0 (= R4 der Vorrunde) | `TEIL I`/`J` | `EXIT=1`, **30** Prüfungen rot |

Werkzeug: `analysis/befunde_2026-09-22/discard-riegel/rueckbau.sh [A B1 B2 C D1 D2 E G …]`
(auch kombiniert, z.B. `D1+F`). Es sichert die zwei berührten Dateien **als Kopie** und
stellt sie danach daraus wieder her.

> ⛔ **Und eine eigene Falle, die hier zugeschnappt ist.** Die erste Fassung des Skripts
> nahm die Rückbauten mit `git checkout --` zurück — bei einem PATH, der `git` gar nicht
> enthält (das Skript setzt den minimalen msys64-PATH, damit `cc1` seine DLLs findet).
> Folge: `command not found`, die Rückbauten **stapelten sich still**, und die
> anschließende Reparatur per `git checkout --` warf die noch nicht eingecheckte Arbeit
> weg. Seither: Rücknahme über Kopien, und **vor** jedem Rückbau-Lauf ein Commit.

---

## 10. Berührte Dateien

| Datei | Änderung |
|---|---|
| `re15_port/engine/src/item_discard_common.c` | Überhang-Satz berichtigt (`:183` → §7.1); `anzahl_zurueck()` zieht den Platz nach; JA-Zweig steigt fail-closed aus (`@0x800517C4`) |
| `re15_port/include/re15_item_discard.h` | Belege an `notice_message` (TEIL O) und `restore` (TEIL P) |
| `re15_port/tests/unit/r21_discard_wegwerfen.c` | `TEIL O`, `TEIL P`, `TEIL Q`, `TEIL C7`, `TEIL M` zweiter Lauf, `PRUEFE(gefahren >= 10)`, `PRUEFE(erreicht)` |
| `re15_port/tests/unit/probes/r21_discard.cmake` | Runde-25-Abschnitt mit allen Rückbau-Zahlen |
| `re15_port/tools/discard_verlustwege.py` | `WIEDEROEFF` / `NEBENWIRK` / `RUFKETTE`, ROOM1090-Dreiglied und `flag(3,129)` `@0x1954` |
| `analysis/befunde_2026-09-22/discard-besitz.md` | Berichtigungs-Banner + Zeile 22 |
| `analysis/befunde_2026-09-22/discard-riegel/` | `rueckbau.sh`, `rueckbau.py`, `riegel.log`, `verlustwege.txt`, `rb_*.log` |

## 11. Was offen bleibt

* **6 von 16 Stellen** sind im Prüfstand nicht erreichbar (Kartenleser ROOM10D0/10D1/11E0/
  11E1/1230/1231, `sub20`): sie hängen an einem AOT, den der Prüfstand nicht betritt. Für
  sie gilt `TEIL A`/`O`/`P`/`Q` (Mechanismus, 16/16), nicht aber `TEIL L`/`I`/`J` (echtes
  Unterprogramm). Um sie zu erreichen, müsste der Prüfstand die AOT-Zone betreten — das ist
  ein Ausbau des Prüfstands, keine Frage am Merkmal.
* **Das eine Schließ-Bild** je Stelle bleibt eine Port-Eigenschaft (RE2 hat 0). Es ist
  gehalten, gemessen und ausgewiesen — aber nicht weg. Weg wäre es nur, wenn Dekrement,
  Frage und neuer Freeze im Port ebenfalls in **einem** Aufruf lägen (`@0x80051810` …
  `@0x80051850`); dafür müsste die Abfrage-FSM in `re15_msg_tick` hinein statt danach
  laufen.
* **`flag(3,129)`-Umkehr nur statisch belegt.** Dass ROOM10B1 `sub03` `@0x01954` im Spiel
  auch *erreichbar* ist, ist hier nicht gefahren — nur, dass der Umkehrer in den
  ausgelieferten Daten existiert.
