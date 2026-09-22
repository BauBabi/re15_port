# Das Besitz-Gate der Wegwerf-Abfrage — an derselben Stelle der Kette wie im Original

Runde 24, 2026-09-22. Vorgänger: `discard-umsetzung.md` (Bau), `discard-loch.md` (Runde 22),
`discard-nacharbeit.md`, `discard-fenster.md` (Runde 23 — dort steht jetzt ein
Berichtigungs-Banner).

Suite: **337/337** grün, `bash re15_port/tools/local_build.sh all` →
`=== LOCAL-BUILD-OK (all) — Tests 337/337`.
Bild aus dem laufenden Spiel: `discard-besitz/bild/bild000476.png` (ROOM1100, echter
Renderpfad über `RE15_FRAMEDUMP` in `re15_render_end_frame()` vor `SDL_RenderPresent` —
kein `RE15_AUTOSHOT`, kein Softwarerenderer), selbst angesehen.

---

## 0. Kurzfassung

| | |
|---|---|
| **Der Sperrgrund** | „Ein Ja kann den Spielstand töten": die Abfrage erscheine an 2 von 16 Stellen **ohne Besitz**, Ursache sei das Besitz-Gate `item_discard_common.c:93/94`. |
| **Gemessen** | Die **Zahl ist 0**, vorher wie nachher: an **0 von 10** im Prüfstand erreichbaren Stellen kommt die Abfrage ohne Besitz. Das Gate liegt seit dem ersten Commit des Merkmals (`df154e0f`) vor der Vormerkung. |
| **Was WIRKLICH falsch war** | Die **Kettenposition**. RE2 entscheidet die Besitzfrage **vor** dem Öffnen der Nachricht (`@0x80051628` / `@0x80051634`), der Port entschied sie **dahinter**. Rückbau-Messung: **20 von 20** Vorentscheiden fielen bei belegtem Nachrichtensystem, jetzt **0 von 20**. |
| **Was der Sperrgrund richtig gesehen hat** | ROOM4000 ist ein Sonderfall — aber ein **Daten**-Sonderfall, kein Port-Defekt: `Set(3,32,1)` `@0x01450` schließt das einzige Tor `Ck(3,32,0)` `@0x0142A`, und das Flag hat im ganzen Bestand **einen** Schreiber und **keinen** Löscher. Die Karte 0x47 liegt in **ROOM4010**, also hinter dieser Tür. |
| **Neu gebaut** | Besitz-Gate als zweistufige Kette (Vorentscheid + Fail-closed-Einhängen), `re15_discard_restore` hart, Nachhall-Überlapp geschlossen, Riegel-Teile L / M / N / C6, `tools/discard_verlustwege.py`. |

---

## 1. Die Regel, gegen die geprüft wird — RE2s Tür-Handler, nachdisassembliert

Aus `ghidra_re2_Leon.txt`, Instruktion für Instruktion (2026-09-22 nachgelesen):

```
80051628  jal   FUN_800696cc          ; Inventarplatz des Schluessels SUCHEN
8005162C  _move a0,s0                 ;   a0 = Gegenstands-Id (s0)
80051630  move  s1,v0                 ;   s1 = gefundener Platz, < 0 = kein Treffer
80051634  bltz  s1,LAB_800516a0       ; KEIN TREFFER -> anderer Zweig
--- Treffer-Zweig ---
80051638  _clear a0
8005163C  li    a1,0x100
80051640  li    a2,0x5                ; Msg 5 = "You have used the <Name>."
80051648  sb    s0,-0x78c1(at)        ; DAT_800e873f = Gegenstands-Id
8005164C  jal   FUN_8002fe38          ; ERST JETZT die Nachricht oeffnen
80051650  _lui  a3,0xff00             ;   Pause-Maske 0xFF000000
80051670  sw    v0=>LAB_80051718,-0x7d50(at)   ; die Fortsetzung EINHAENGEN
80051674  addiu v0,s1,0x1
80051680  sb    v0,-0x7f9f(at)=>DAT_800d4249   ; Platz+1 merken
--- Nicht-Treffer-Zweig ---
800516A0  lui   a0,0x216
800516A4  jal   FUN_8005ba28
800516B4  addiu a2,s0,-0x4c           ; ANDERE Nachrichten-Id (item_id - 0x4C)
800516B8  jal   FUN_8002fe38
800516C0  j     LAB_800516f8          ; return - KEINE Fortsetzung, also nie eine Abfrage
```

Die Reihenfolge ist damit belegt: **(1)** Platz suchen, **(2)** ohne Treffer abzweigen,
**(3)** Nachricht öffnen, **(4)** Fortsetzung einhängen.

**Der Port macht jetzt genau das** (`engine/src/item_discard_common.c`):

| RE2 | Port |
|---|---|
| `@0x80051628` `jal FUN_800696cc` | `re15_inv_find_item(item)` in `re15_discard_besitz_vor_nachricht()` |
| `@0x80051634` `bltz s1` | `if (slot < 0 …) return 0;` — Vorentscheid ungültig |
| `@0x8005164C` `jal FUN_8002fe38` | `re15_dialog_open_mask` / `msg_show` in `op_message_on` |
| `@0x80051670` `sw LAB_80051718,…` | `re15_discard_notice_message()` unmittelbar danach |
| `@0x800516C0` `j LAB_800516f8` | Fail-closed: ohne Vorentscheid mit Treffer wird **nichts** eingehängt |

RE1.5 wählt die Nachricht im **Skript**, der Port darf sie also nicht tauschen — der
Nicht-Treffer-Zweig besteht hier nur aus „nichts einhängen". Das ist genau `@0x800516C0`.

### Warum „fail-closed" und nicht bloß eine zweite Prüfung

`re15_discard_notice_message()` merkt **nur** vor, was der Vorentscheid für **dieselbe**
`(Raum, Nachricht)` mit Treffer freigegeben hat. Ein künftiger neuer Öffnungsweg für
Nachrichten kann die Abfrage damit nicht versehentlich ohne Besitzprüfung armieren — er
bekommt keinen Vorentscheid und also keine Vormerkung.

---

## 2. Die Kettenposition — gemessen, mit Rückbau

Neue Messgröße `re15_discard_vorentscheid_belegt()`: wie viele Vorentscheide fielen,
**obwohl** das Nachrichtensystem schon belegt war (`@0x800517f0 andi v0,v0,0x80`)? RE2
entscheidet vor `@0x8005164C`, die Zahl muss also 0 sein.

| | Vorentscheide | davon bei belegtem Nachrichtensystem |
|---|---|---|
| **vorher** (Aufruf in `re15_discard_notice_message`, also hinter `re15_dialog_open_mask`) | 20 | **20** |
| **nachher** (Aufruf in `op_message_on` vor dem Öffnen) | 20 | **0** |

Der Rückbau ist ein echtes Ein-Variablen-Experiment an der Quelle: derselbe Riegel, nur der
Aufruf um zwei Anweisungen verschoben. Riegel `TEIL L` verlangt 0 und nennt beide Zahlen.

---

## 3. Der Spielstand-Killer: die 16 Stellen, gezählt

### 3a. Die Port-Abfrage ohne Besitz — 0 vorher, 0 nachher

Riegel `TEIL L` fährt je Stelle das **ausgelieferte** Unterprogramm im echten VM (nicht die
6-Byte-Ersatzfolge aus TEIL A), mit einem Spieler, der jede Ja/Nein-Frage des Skripts mit
**JA** beantwortet (virtuelles Bestätigen-Bit 0x4000). Zwei Läufe je Stelle, **ein**
Unterschied — der Besitz:

```
  Stelle                   item | OHNE Besitz            | MIT Besitz
  ROOM1090 sub03 m9        0x31 | erreicht=1 abfrage=nein | abfrage=JA
  ROOM10D0 sub20 m9        0x38 | erreicht=0 abfrage=nein | abfrage=nein  (nicht erreicht)
  ROOM10D1 sub20 m9        0x38 | erreicht=0 abfrage=nein | abfrage=nein  (nicht erreicht)
  ROOM1100 sub02 m4        0x44 | erreicht=1 abfrage=nein | abfrage=JA
  ROOM1101 sub02 m4        0x44 | erreicht=1 abfrage=nein | abfrage=JA
  ROOM11E0 sub20 m9        0x39 | erreicht=0 abfrage=nein | abfrage=nein  (nicht erreicht)
  ROOM11E0 sub21 m12       0x30 | erreicht=1 abfrage=nein | abfrage=JA
  ROOM11E1 sub20 m9        0x39 | erreicht=0 abfrage=nein | abfrage=nein  (nicht erreicht)
  ROOM11E1 sub21 m12       0x30 | erreicht=1 abfrage=nein | abfrage=JA
  ROOM1230 sub20 m9        0x37 | erreicht=0 abfrage=nein | abfrage=nein  (nicht erreicht)
  ROOM1231 sub20 m9        0x37 | erreicht=0 abfrage=nein | abfrage=nein  (nicht erreicht)
  ROOM3010 sub02 m1        0x36 | erreicht=1 abfrage=nein | abfrage=JA
  ROOM3011 sub02 m1        0x36 | erreicht=1 abfrage=nein | abfrage=JA
  ROOM3050 sub15 m5        0x46 | erreicht=1 abfrage=nein | abfrage=JA
  ROOM3051 sub15 m5        0x46 | erreicht=1 abfrage=nein | abfrage=JA
  ROOM4000 sub02 m2        0x47 | erreicht=1 abfrage=nein | abfrage=JA
  ABDECKUNG: 10 von 16 Benutzungsstellen im Pruefstand erreicht (6 nicht erreicht)
  OHNE BESITZ eine Abfrage: 0 von 10 erreichten Stellen
  MIT  BESITZ eine Abfrage: 10 von 16 Stellen (GEGENPROBE)
  Das Gate selbst: 20 Vorentscheide, davon 10 mit Besitz
  KETTENPOSITION: 0 von 20 Vorentscheiden fielen bei belegtem Nachrichtensystem
```

**Die Zahl über alle 16 Stellen: 0 vorher, 0 nachher.** Das Gate liegt seit `df154e0f`
(erster Commit des Merkmals) vor der Vormerkung; es gab hier nichts zu reparieren, und ich
erfinde dafür keine Regression. Was sich geändert hat, ist die **Abdeckung** (gefahren wird
jetzt das ausgelieferte Unterprogramm statt einer Ersatzfolge), die **Kettenposition**
(§2) und die **Fail-closed-Eigenschaft**.

Die 6 nicht erreichten Stellen sind die **Kartenleser** (ROOM10D0/10D1/1230/1231/11E0/11E1
je msg 9). Sie hängen hinter `Ck(12,31,0)` (die Ja-Antwort) **und** `Ck(9,<tk_bit>,1)` (das
„genommen"-Bit) — ohne beides geht der Zweig nach Else. Das ist *nicht erreicht*, nicht
*grün*; der Live-Abzug aus Runde 23 belegt, dass die Abfrage dort im Spiel aufgeht.

**Die Gegenprobe ist der MIT-Lauf**: ohne ihn stünde die 0 auch dann, wenn die Abfrage
nirgends käme. Sie kommt an 10 von 10 erreichbaren Stellen. Zusätzlich sind die Zähler des
Gates ausgewiesen (20 Vorentscheide / 10 Treffer) — so ist belegt, dass es **gefragt**
wurde und nicht bloß nichts passierte.

### 3b. ROOM4000 und ROOM1090, beide Fälle gefahren (`TEIL N`)

```
  (a) ROOM4000 sub02 m2  OHNE Besitz : Stelle erreicht=1, Abfrage=nein, flag(3,32) 0->1
  (b) ROOM4000 sub02 m2  MIT  Besitz : Abfrage=JA auf 0x47 (Auswahl 0), flag(3,32) 0->0,
                                       Ja -> Gegenstand WEG
  (a) ROOM1090 sub03 m9  OHNE Besitz : Stelle erreicht=1, Abfrage=nein, flag(3,132) 0->0
  (b) ROOM1090 sub03 m9  MIT  Besitz : Abfrage=JA auf 0x31 (Auswahl 0), flag(3,132) 0->0,
                                       Ja -> Gegenstand WEG
```

In (b) steht `flag(3,32)` noch auf 0, weil der **sichtbare** Prompt das Skript einfriert
(`re15_discard_frozen()`, byte-true `@0x80051844`/`@0x80051850`) — `Set(3,32,1)` `@0x01450`
läuft erst nach der Antwort. Genau so verlangt es RE2.

---

## 4. ⛔ Zwei Berichtigungen zum Sperrgrund — am Byte

Das ausgelieferte `ROOM4000` sub02 (Datei-Offsets, mit dem Längen-Vorschub des Motors
ausgelesen, `tools/scd_dump_room.py`):

```
0x01426  Ifel_ck      06 00 2c 00
0x0142A  Ck           21 03 20 00     ; Ck(3,32,0)   Tuer noch unbenutzt
0x0142E  Se_on        36 02 0f 00 03 00 00 00 00 00 00 00
0x0143A  Message_on   2b 00 ff ff     ; "The door is locked ... An ID card is required to unlock it."
0x0143E  Evt_next     02
0x01440  Message_on   2b 01 ff ff     ; "Will you use the Blue Master Keycard?"   (0x03 = Ja/Nein)
0x01444  Evt_next     02
0x01446  Ck           21 0c 1f 00     ; Ck(12,31,0)  ANTWORT JA
0x0144A  Message_on   2b 02 ff ff     ; "You've used the Blue Master Keycard."  <== die Stelle
0x0144E  Evt_next     02
0x01450  Set          22 03 20 01     ; Set(3,32,1)  Tuer ab jetzt benutzt
0x01454  Endif        08 00
0x01456  Ifel_ck      06 00 10 00
0x0145A  Ck           21 03 20 01     ; und der Zweig dahinter oeffnet sie
0x0145E  Set          22 05 00 01
0x01462  Set          22 05 01 01
0x01466  Gosub        18 03           ; sub03 = der Durchgeh-Ablauf
```

**(1) `Ck(3,32,0)` ist NICHT das einzige Tor.** Das Urteil nennt es so und lässt
`@0x01446 Ck(12,31,0)` aus — die **Ja-Antwort des Spielers** auf `Message_on 1`. Und
`Set(3,32,1)` `@0x01450` liegt **hinter** diesem Ck: `op_ck` ist selbst das Prädikat und
gibt bei falsch `SCD_R_IF_FALSE` zurück, worauf der Dispatcher auf das `block_end` des
umgebenden `Ifel_ck` springt (`scd_vm.c:1992-2011`). Ohne Ja fällt das Flag also nicht.
Das Urteil zitiert außerdem `Message_on 0 ff ff` als die Stelle, an der die Abfrage hängt —
die Stelle ist `Message_on 2` `@0x0144A`.

**(2) „Ohne Besitz fällt `Set(3,32,1)` nicht" ist im Auslieferungsstand nicht erfüllbar**,
ohne dem Skript eine Besitzprüfung anzudichten: sub02 hat keine. Und das ist kein
Versehen — die **Blue Master Keycard 0x47 wird im ganzen Spiel genau einmal ausgegeben, in
ROOM4010**, also *hinter* dieser Tür (`Item_aot_set` 0x50,
`tools/discard_verlustwege.py`). Wer die Tür ohne Karte nicht öffnen könnte, käme nie an
die Karte. Gemessen (`TEIL N` (a)): `flag(3,32) 0->1` **ohne** Besitz — byte-true.

Daraus folgt der eigentliche, neue Befund: **die Benutzungsstelle für 0x47 ist im
Auslieferungsstand nie mit Besitz erreichbar.** Der erste Durchgang verbraucht sie
zwangsläufig ohne Karte; danach ist `Ck(3,32,0)` für immer falsch. Der Eintrag
`{ 0x4000, 2, 0x47 }` in `discard_sites.inc` ist damit toter, aber harmloser Code — er
wird bewusst **nicht** entfernt (das Tabellen-Kriterium ist datengetrieben, und eine
Reichweiten-Bedingung wäre ein neues, eigenes Modell). Er ist hier ausgewiesen, damit
niemand ihn für einen Port-Defekt hält.

**Und ROOM1090 ist NICHT derselbe Fall.** Das Urteil nennt es „derselbe Fall, nur nicht
deterministisch". Gemessen über alle RDTs:

| Flag | `Set(…,1)` | `Set(…,0)` | `Set(…,7)` |
|---|---|---|---|
| `flag(3,32)` (ROOM4000) | ROOM4000 @0x1450 | — | — |
| `flag(3,129)` (ROOM1090 @0x271E) | ROOM1090 @0x271E, ROOM10B1 @0x1832 | **ROOM10B1 @0x1954** | — |
| `flag(3,132)` (Tor von sub03) | ROOM1090 @0x2722, ROOM11B1 @0x11EC | ROOM1090 @0x24CE | — |

`flag(3,32)` hat **einen** Schreiber und **keinen** Löscher — endgültig. Die ROOM1090-Flags
haben beides und sind damit wieder öffenbar. Riegel `TEIL C6` pinnt das (1 Setzer /
0 Löscher) im Vollzensus über die 206 RDTs.

---

## 5. Weitere Verlustwege — selbst gesucht, alle 16 Stellen

`re15_port/tools/discard_verlustwege.py` (neu) beantwortet je Stelle: *Setzt ein „Ja" ein
Flag, das ein Tor schließt, und war das Tor der einzige Weg?* Gearbeitet wird mit dem
Block-Modell des Motors (ein `Ck` ist ein Tor für alles, was in seinem Block dahinter
steht — auch ohne eigenes `Ifel_ck`, siehe `Ck(12,31,0)` `@0x01446`), einer Ebene
Aufrufer-Analyse (`Evt_exec`/`Gosub`) und einem globalen Schreiber-Zensus, der „einmalig"
von „endgültig tot" trennt.

**Ergebnis über die 16 Stellen:**

| Eigenschaft | Zahl |
|---|---|
| ohne **Besitztor im Skript** (kein `Ck` auf Flag-Zone 9 = „genommen"-Bit) | **10 von 16** |
| mit einem Set, das ein eigenes Tor schließt | 8 von 16 |
| davon **endgültig** geschlossen (kein Wiederöffner im ganzen Bestand) | **1 von 16** |
| davon Tor = einziger Weg zu dieser Stelle | **1 von 16** |
| davon zugleich ohne Besitztor | **1 von 16** |

Die eine Stelle ist **ROOM4000 msg 2**. Die 6 Stellen **mit** Besitztor sind die
Kartenleser: `Ck(9,52,1)` (ROOM10D0/10D1, Blue Keycard), `Ck(9,136,1)` (ROOM1230/1231, Red
Keycard), `Ck(9,138,1)` (ROOM11E0/11E1, Yellow Keycard). Die anderen 10 fragen ohne
Besitzprüfung — deshalb muss die Prüfung im Port sitzen, und deshalb ist §1 nicht optional.

Die 7 weiteren Stellen mit „schließendem Set" sind Zonen-5-Ereignis-Latches
(`Set(5,5,0)` `@0x00C7C` in ROOM1100, `Set(5,22,0)` `@0x01FCE` in ROOM11E0,
`Set(5,4,0)` `@0x025F4` in ROOM3050, `Set(3,132,0)` `@0x024CE` in ROOM1090) — alle haben
einen Wiederöffner und sind damit wiederholbar, nicht verloren.

---

## 6. Die sechs Pflicht-Korrekturen

| # | Auftrag | Erledigt |
|---|---|---|
| 1 | Pin auf `RE15_PAUSE_PLAYER 0x80000000` (`@0x80031c54 lw a0,g_pauseflags` / `@0x80031c78 bltz a0,0x80031da8`) statt `0x01000000` | `TEIL I` zählt jetzt `RE15_PAUSE_PLAYER` und `RE15_PAUSE_SCD`. Der falsche Zeuge war das Pad-Bit: es maskiert nur die SCD-Pad-Wörter (`game_step_common.c:1061-65`) **und** wird zusätzlich vom Szenen-Fenster gehalten — deshalb kamen nur 4 Brücken-Bilder heraus. |
| 2 | Die Zahl „4" berichtigen auf „1 Bild an 10 von 10 Stellen, 605 Einheiten im Rückbau" | Gemessen: **1 Bild an 10 von 10 Stellen** (`PRUEFE(ohne_bruecke == 1)` je Stelle). Rückbau: **605 Einheiten** an den 9 Stellen mit Überhang-Spanne, **680** einschließlich des jetzt ausgewiesenen ROOM1090 (dort 75) — beide Zahlen stehen in der Riegel-Ausgabe. |
| 3 | Den Absolutsatz „Das Fenster ist WEG, nicht abgesichert" streichen | Titel und Aussage in `discard-fenster.md` tragen ein Berichtigungs-Banner; `TEIL I` gibt jetzt aus: *161 Bilder Überhang → 1 Bild je Stelle, gehalten von `re15_discard_frozen()`; RE2 = 0 Bilder (`@0x80051810` dekrementiert, `@0x80051844 lui v1,0xff00` friert im selben Aufruf wieder ein)*. |
| 4 | ROOM1090 in `TEIL J` als `AUSGELASSEN` ausgeben | Steht jetzt in der Tabelle mit Grund: *Faden endet nicht, Warteschleife* `Ck(5,33,0)` `@0x26FC` (sub05 `@0x26F4 Do` / `@0x026FC Ck(5,33,0)` / `@0x02700 Evt_end`, betreten per `Gosub 5` aus sub03). Bis Runde 23 fiel die Stelle per `continue` heraus und zählte als Erfolg. |
| 5 | `re15_discard_restore` hart machen | Liest `item` nicht mehr und belebt nie eine Abfrage: `(void)item; re15_discard_reset();`. Begründung im Header — RE2s Fortsetzungs-Zeiger `DAT_800D4498` (`@0x80051670`) liegt im RAM und in keinem Speicherformat, und der Auslieferungsstand kann nicht speichern (21 RDTs: „Save is not available in this preview"). Das Feld im Spielstand bleibt für die Formatstabilität erhalten. |
| 6 | Den Nachhall-Überlapp benennen und, wenn belegbar, schließen | **Benannt und geschlossen.** `TEIL M` misst: **4 Bilder** Überlapp im Rückbau, **0** jetzt. |

### Zu 6: warum das Schließen belegbar ist

RE2 kennt den Zustand „Text steht, Freeze gelöst" nicht — zwei unabhängige Belege:

```
800307E0  lbu   v0,0x5c8c(s1)
800307E4  lw    v1,0x5cac(s1)
800307E8  andi  v0,v0,0x7f            ; Belegt-Bit 0x80 loeschen
800307EC  sb    v0,0x5c8c(s1)
800307F0  lui   at,0x800d
800307F4  sw    v1,-0x424(at)=>DAT_800cfbdc   ; Pause-Schnappschuss zuruecklegen
```
Ein Paar im selben Basisblock, keine Bildgrenze dazwischen. Und:
```
8005182C  li    a1,0x100
80051830  li    a2,0x9                ; Prompt-Skript 9
80051834  jal   FUN_8002fe38          ; DIESELBE Routine wie @0x8005164C
```
Ein Kanal, eine Zeile: die Abfrage legt sich **auf** die vorige Zeile. Der
Untertitel-Nachhall des Ports (msg-FSM Zustand 7) ist eine ausdrückliche
**Nutzer-Entscheidung** vom 2026-09-20 und **bleibt** — er hält den Spieler nicht auf.
Geschlossen wird nur die Überlappung: `re15_discard_tick` ruft beim Aufgehen
`re15_msg_nachhall_beenden()` (`msg_common.c`), das ausschließlich im Zustand 7 wirkt und
kein Pause-Bit anfasst.

---

## 7. Was gebaut wurde

| Datei | Änderung |
|---|---|
| `engine/src/item_discard_common.c` | `re15_discard_besitz_vor_nachricht()` (Vorentscheid = `@0x80051628`/`@0x80051634`), `notice_message` fail-closed (`@0x80051670`), `restore` hart, `re15_msg_nachhall_beenden()` beim Aufgehen, Zähler `vorentscheide` / `_mit_besitz` / `_belegt` |
| `engine/src/scd_vm.c` | `op_message_on` ruft den Vorentscheid **vor** `re15_dialog_open_mask`/`msg_show` |
| `engine/src/msg_common.c` | `re15_msg_nachhall_beenden()` |
| `include/re15_item_discard.h`, `include/re15_msg.h` | Herleitung + Deklarationen |
| `tests/unit/r21_discard_wegwerfen.c` | `TEIL L` (Besitz-Gate an den ausgelieferten Subs), `TEIL M` (Nachhall-Überlapp), `TEIL N` (ROOM4000/ROOM1090 beide Fälle), `C6` (Schreiber von `flag(3,32)`), `TEIL I` auf den richtigen Zeugen umgestellt + `ohne_bruecke == 1` je Stelle, `TEIL J` um `RUECKBAU` erweitert und ROOM1090 ausgewiesen |
| `tests/unit/probe_besitz_zensus.c`, `probes/r24_besitz.cmake` | Messonde (kein `add_test`), mit der die Zahl 0/10 zuerst gemessen wurde |
| `tools/discard_verlustwege.py` | Verlustwege-Zensus über alle 16 Stellen |
| `analysis/befunde_2026-09-22/discard-fenster.md` | Berichtigungs-Banner |

---

## 8. Das Bild aus dem laufenden Spiel

`discard-besitz/bild/bild000476.png` — ROOM1100 (Evidence Corridor), Minidisc Player
w/ Disc 0x44. Zu sehen: die Abfrage „You don't need this key any more. Discard it?" mit dem
Cursor auf **Yes**, „No" rechts daneben, Text vollständig getippt. **Kein** stehender
Untertitel der auslösenden Zeile daneben — das ist §6 im Bild.

Messschiene desselben Laufs (`discard-besitz/bild/discard.log`,
Abzug: `discard-besitz/abzug_1100.sh`):

```
F301  abfrage=1 frage=0 ... belegt=1 frost=0 pausepad=1 px=-20615 pz=-25000
F421  abfrage=1 frage=8 gegenstand=0x44 wahl=0 text=0/44  ... frost=1 px=-20615 pz=-25000
F479  abfrage=1 frage=8 gegenstand=0x44 wahl=0 text=44/44 ... frost=1 px=-20615 pz=-25000
Wartebilder insgesamt 120, davon mit FREIEM Pad 0
px/pz über das Wartefenster: EIN Paar (px=-20615 pz=-25000)
```

Der Abzug zündet `sub02` über `RE15_FORCE_EVENT=2@300`, also über denselben
Ereignis-Dispatcher (`scd_event_fire`), den das Skript benutzt — die echte Zündung hängt an
der Stellung des Zahlenschlosses (`Member_cmp member=0x0F == 5` `@0x00C3C`) und ist nicht
deterministisch fahrbar. Das Skript selbst ist unangetastet; das ist im Abzug-Skript
ausdrücklich vermerkt.

---

## 9. Offen

* **6 von 16 Stellen** (die Kartenleser) sind im Prüfstand nur über einen Live-Abzug
  erreichbar, weil ihr Zweig an `Ck(12,31,0)` **und** `Ck(9,<tk_bit>,1)` hängt. Sie sind
  als *nicht erreicht* ausgewiesen, nicht als grün. Ein Riegel, der die Ja-Antwort und das
  „genommen"-Bit im Prüfstand setzt und dann fährt, würde die Abdeckung auf 16/16 heben.
* **ROOM4000 msg 2** bleibt ein toter Tabelleneintrag (§4). Ob die Tabelle eine
  Reichweiten-Bedingung bekommt, ist eine eigene Entscheidung — hier nur gemessen.
* **PSX-Bau bleibt offen** — aber nicht mehr am gleichen Punkt. `cmake/psx_toolchain.cmake`
  setzte die Werkzeuge ohne `.exe`; die C-Prüfung findet den Compiler noch (GNU 12.3.0),
  die ASM-Prüfung verlangt einen vollen Pfad auf eine existierende Datei und brach mit
  *"is not a full path to an existing compiler tool"* ab. Das ist behoben (Endung wird
  bestimmt, nicht geraten). Danach scheitert `find_package` an der SDK-Installation selbst:
  `C:/PSn00bSDK/sdk/PSn00bSDK-0.24-win32/include/libpsn00b/` hat **kein `stdint.h`**
  (`gcc`s eigenes macht `include_next <stdint.h>` und findet nichts). Das ist eine
  unvollständige SDK-Installation, keine Port-Frage.
  **Ersatzweise gemessen**, damit die Änderung nicht unbelegt für PSX behauptet wird: die
  drei geänderten GEMEINSAMEN Quellen sind mit `mipsel-none-elf-gcc 12.3.0`
  (`-march=r3000 -mabi=32 -msoft-float -ffreestanding -DRE15_PLATFORM_PSX=1`) gegen einen
  freistehenden `stdint`-Ersatz geprüft: `item_discard_common.c`, `msg_common.c` und
  `scd_vm.c` gehen ohne Fehler durch (scd_vm.c meldet nur die vorbestehende
  `getenv`-Warnung des freistehenden Aufbaus). Das Besitz-Gate liegt damit auf BEIDEN
  Zielen im selben gemeinsamen Code — es gibt keine PC-Sonderbehandlung.
* `TEIL J` fährt 10 Stellen; **1** davon (ROOM1090) ohne Überhang-Spanne, und
  **2** (ROOM11E0/11E1 m12) haben im Brücken-Bild 0 Einheiten, weil sie dort ohnehin eine
  Szene hält. Beide Fälle sind je Stelle ausgewiesen.
