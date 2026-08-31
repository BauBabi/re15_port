# 1210-re2-verhalten — Schiedsspruch

RE2-Adressen mit `re2_disasm.py` selbst disassembliert, Port-Code selbst nachgelesen.

## ⛔ WERKZEUG-BEFUND VORWEG

Die Auftrags-Anleitung *"RE2-EXE: info/re2leon/PSX.EXE (per --bin relativ zu info/Re1.5/PSX/BIN/)"*
**funktioniert nicht.** In `re15_disasm.py` (`def load`) wird `--bin` nur fuer Adressen
>= 0x80100000 ausgewertet; darunter wird hart `EXE_PATH = info/Re1.5/PSX.EXE` genommen
(selbst im Quelltext geprueft, Zeilen 58-64). Jede RE2-EXE-Nachpruefung mit dieser Methode
disassembliert **still die falsche EXE**. Korrektes Werkzeug:
`.claude/skills/re15-psx-disasm/scripts/re2_disasm.py` (EXE_PATH = info/re2leon/PSX.EXE,
OVERLAY_DIR = info/re2leon/COMMON/BIN). Das gehoert in die Anleitung.

## URSACHE (belegt)

**Die heutigen Tor-Konstanten des Ports haben keine Herkunft.** `enemy_ai_common.c:11135`:

```c
enum { RE15_WRITHER_REACH_Z = 850, RE15_WRITHER_REACH_X = 11000, ... };
int reach = (dz < RE15_WRITHER_REACH_Z) && (dx < RE15_WRITHER_REACH_X);
```

Beide tragen — anders als jede andere Konstante desselben Enums — **kein `@0x…`**. Selbst
nachgelesen und bestaetigt. REACH_X 11000 deckt die ganze Flurbreite ab, deshalb reagieren in
jeder Spur beide Reihen. Das ist der belegte Kern des Findings.

**RE2s verankerter Greifer misst Radius + Sektor, kein Rechteck.** Selbst disassembliert,
vollstaendig (EMOVL10_S0.BIN @0x80102ee4):

```
80102f0c: lw    s0,496(s1)          ; entity+0x1F0
80102f10: jal   0x80015614          ; TOT (v0 nie geprueft)
80102f14: addiu a3,zero,1024
80102f2c: jal   0x80015614          ; TOT
80102f30: addiu a3,zero,256
80102f38: addiu s2,s2,-1032         ; s2 = 0x800cfbf8 (Spieler)
80102f3c: sltiu s0,s0,0x514         ; ABSTAND < 1300
80102f40: beq   s0,zero,0x80103008
80102f4c: lbu   v0,-565(v0)         ; 0x800cfdcb = Spieler+0x1D3
80102f54: andi  v0,v0,0x80          ; EIN-ANGREIFER-RIEGEL
80102f58: bne   v0,zero,0x80103008
80102f60: lbu   v1,262(s1)          ; entity+0x106
80102f68: lbu   v0,-770(v0)         ; 0x800cfcfe
80102f70: bne   v1,v0,0x80103008    ; ETAGEN-GLEICHHEIT
80102f80: andi  v0,v0,0x20          ; +0x21A Bit 0x20 -> Arm 1 aus
80102f90: lh    a2,118(s1)
80102f98: jal   0x80015758
80102f9c: addiu a2,a2,256           ; Sektor Mitte yaw+256, Halbwinkel 256
80102fa4: addiu v0,zero,257
80102fa8: sw    v0,4(s1)            ; +0x5 = 1  -> Griff im SELBEN Bild
80102fb8: ori   v0,v0,0x80
80102fc0: sb    v0,-565(at)         ; das TOR setzt den Riegel selbst
80102fcc: andi  v0,v0,0x40          ; Zwilling: +0x21A Bit 0x40, yaw-256
```

`FUN_80015758` gibt 0 = INNERHALB (`sltu v0,v0,s0` / `xori v0,v0,0x1` @0x800157b4-b8).
`FUN_80015614` hat ausser Stack-Stores **keinen einzigen Store** (@0x80015614-98 selbst
gelesen) — die beiden Aufrufe sind wirklich tot. Der freie Zombie nutzt 0x4b0 (1200)
@0x801018f4 mit demselben Sektorpaar.

## BESTRITTEN

1. **"byte-gleich zu 0x80102ee4" — FALSCH.** RE2s 1300 misst `entity+0x1F0` =
   **Ursprung gegen Ursprung** (`lw v0,56/64(s0)` gegen den Spieler, `jal 0x8008d2f4`,
   `sw v0,496(s0)` @0x800265a4-e0). Der Vorschlag legt dieselbe Zahl auf
   `dist(Spieler, Ursprung + 2420 + 1671)`. Das ist eine begruendete **Nachruestung**, kein
   Original-Beleg. Die faktisch neue Konstante ist der Versatz 4091.

2. **Fix 1 ist mechanisch falsch — der Ausdruck kippt, sobald der Arm sich bewegt.**
   Selbst im Port nachgelesen: `re15_writher_step` (enemy_ai_common.c:10958-10970) **bewegt
   e->x/e->z**. Nach der Lunge steckt LUNGE_NET 2420 also bereits im Ursprung; der Vorschlag
   addiert sie ein zweites Mal. Genau deshalb misst das **bestehende** Griff-Tor des Ports nur
   `+ MESH_REACH 1671` (Zeilen 11274-11280) — der Vorschlag ist mit dem eigenen, funktionierenden
   Tor inkonsistent. Der Pruefer hat es gemessen: im Bild der groessten Naehe liefert der
   Ausdruck fuer den nahen Arm 2031 (reach FALSCH), fuer die Gegenreihe 497..535 (reach WAHR)
   — die **Umkehrung** der versprochenen Wirkung. Verschaerfend: `reach` ist zugleich das
   **Rueckzugs-Tor** (`if (!reach) { e->sub_state_1 = 3; ... }`, Zeile 11306).

3. **Fix 2 tut nicht, was er begruendet.** `case 0` setzt `e->sub_state_2 = 1` und **faellt
   durch** (enemy_ai_common.c:11176-11180, Kommentar zitiert @0x8010c768/@0x8010c7bc).
   `sub_state_2 >= 1` ist also im **ersten** Bild wahr, nicht im vierten. Die 4-Bild-Bedingung
   waere `sub_state_2 >= 2` (@0x8010c7e8). So formuliert kann der Griff mit ein Drittel
   ausgefahrenem Arm feuern — genau die Fehlerklasse, die der Nutzer gemeldet hat.

4. **Fix 3 (GRAB_DIST 1200 -> 1300) wird GESTRICHEN.** Drei unabhaengige Gruende:
   - **Wirkungslos**: ein Pruefer hat die Sonde gefahren, in allen 30 Sweep-Zeilen sind
     T_grab 1200 und 1300 bitgleich.
   - **Falsche Begruendung**: 1300 gehoert dem VERANKERTEN Zombie (@0x80102f3c), 1200 dem
     FREIEN (@0x801018f4). Zwei Gegner, nicht "ein Tor mit zwei Zahlen". RE2 hat gar kein
     Ausfahr-Tor, dort kann nichts "dieselbe Zahl" sein.
   - **Kippt einen ⛔-Marker im Produktivcode** (enemy_ai_common.c, Block vor Zeile 11281:
     *"AUSDRUECKLICH NICHT getan: die Schwelle 1200 anheben. Das waere eine erfundene Zahl fuer
     genau dieses Problem"*) — ohne eine einzige Messung, die den Wechsel traegt.

5. **Die RE2-Tor-Beschreibung ist unvollstaendig.** Zwei harte Bedingungen fehlen in der
   Zusammenfassung (beide oben woertlich disassembliert): der Ein-Angreifer-Riegel
   @0x80102f48-58 — der **vor** dem Winkeltest gelesen und **im Tor selbst** gesetzt wird
   (@0x80102fb8-c0), nicht erst im Griff-Schritt — und die Etagen-Gleichheit @0x80102f60-70.

6. **"RADIUS + zwei Halb-Sektoren statt Kegel" traegt die Uebertragung nicht.**
   Sektor 1 = [yaw, yaw+512], Sektor 2 = [yaw-512, yaw]; Vereinigung = yaw +-512 = **+-45 Grad**
   — dieselbe Weite wie der echte Kegel der Schwester-Variante (`jal 0x80015614`, a3=512,
   v0 dort GEPRUEFT @0x80103ab8-c0). Die Zweiteilung entscheidet nur, **welcher der beiden Arme**
   greift (+0x21A Bit 0x20 / 0x40). Der Port haengt die ODER-Verknuepfung an eine Kreatur, die
   selbst EIN Arm ist — das verdoppelt das Fenster pro Arm von +-22,5 auf +-45 Grad, ohne dass
   eine Adresse das deckt. (Betrifft auch das **bestehende** Griff-Tor, Zeilen 11283-11288.)

7. **Die Wirkungszahlen "aus 0 Griffen werden 4 bzw. 6" sind ein Modell-Artefakt.**
   Abschnitt (D) der Sonde laeuft mit `re15_enemy_ai_set_paused(1)` — die Arme stehen 1200 Bilder
   auf der Heimatposition, ihr Bewegungsmodell ist `adv = 800*ext_f`, ab Bild 4 fest 2420.
   Das Original zieht dazwischen **zurueck**: Port-Zeile 11203 fuehrt es byte-true
   (`re15_writher_step(e, 0x800)`, `ori a0,zero,0x800` @0x8010c8b4, +0x8c=20 ueber 30 Bilder)
   — die Hand ist in den Bildern 5..35 bis zu 800 Einheiten kuerzer, genau im Griff-Fenster.
   Zusaetzlich fehlen in (D) die `re15_player_is_grabbed()`-Schranke und der Riegel, die Original
   (@0x80102f48-58) und Port (Zeile 11281) beide fuehren; `grabbed[i]` wird pro Arm unabhaengig
   gezaehlt. Belegbar ist **"aus 0 wird mindestens 1"**, nicht 4 bzw. 6.
   Die Rueckzugsregel der Sonde (`ext_f > 39 && dout > T_OUT + 400`) enthaelt ausserdem eine
   **erfundene 400er-Hysterese** und eine 39-Bild-Sperre, die im Fix nirgends vorkommen.

8. **Kleinere Fehlaussagen:** "in JEDER Spur reagieren ALLE 10 Arme" — die eigene Tabelle zeigt
   fuer West 75 und West 200 je 9. "T_out 2600/3200 kippt auch die Wand-Spuren" gilt nur West
   (Ost bleibt bei 2600 auf 6). "Verankertes Ruhen dreht sich nur alle (rng&7)+7 Bilder um 24"
   ist eine Fehllesung: ab Anim-Bild >= 21 (`slti v0,v0,21` @0x801030c8) dreht es in JEDEM Bild,
   die (rng&7)+7 loesen nur eine ZUSAETZLICHE Drehung aus. "+0x1F0 wird pro Bild gerechnet" ist
   bedingt: `lhu v0,270(s0) / andi 0x8000 / bne` @0x80026590-9c ueberspringt Rechnung und
   Hirn-Aufruf.

## OFFEN

- **MESH_REACH 1671** wurde in dieser Sitzung von niemandem neu gemessen (der Befund sagt es
  selbst). Es traegt die Haelfte des vorgeschlagenen 4091er-Versatzes UND das bestehende
  Griff-Tor. Wer es anzweifelt, muss `probe_1210_reach` Abschnitt (5) erneut fahren.
- **"Das ist der Fenster-/Gitter-Zombie"** ist unbelegt. Belegt ist nur: ungerade Werte von
  `+0x10E & 0x3F` (Tabelle @0x8010c854) zeigen auf das 3-Zustands-Hirn 0x80101210. Welche
  RE2-Kreatur / welcher Raum diesen Spawn-Wert traegt, ist nicht gezeigt — und damit auch nicht,
  ob Zustand [0] (Sektorpaar, 0x80102ee4) oder Zustand [2] (echter Kegel + `+0x154 & 0x800`,
  0x80103a70) das richtige Vorbild ist.
- **Bedeutung von +0x21A Bit 0x20 / 0x40**: als "je Arm abschaltbar" gelesen, kein Setzer gezeigt
  (76 Schreiber auf +0x21A im Overlay). Ohne das bleibt die ODER-Verknuepfung im Port unbelegt.
- **Welche Lesart von "zu weit weg"** der Nutzer meint (LATERAL: Gegenreihe reagiert; ZEITLICH:
  Arm steht 28..36 Bilder zu spaet da), ist nicht belegt.
- **Keine visuelle Verifikation.** Alles hier ist Headless-Geometrie, bei einem VISUELL
  gemeldeten Fehler.
- Beide Sonden setzen `pl->x/pl->z` direkt und verwerfen, was die Engine im selben Bild
  zurueckschiebt; der Spieler laeuft nie ueber `re15_player_move`, sein `rot_y` bleibt 0.
- Warum Arme in den Lauf-Spuren nicht auf die Heimatposition zurueckgestellt werden (Arm 5 auf
  x=-22580, Arm 6 auf -23160, Arm 9 auf -16240) — unverfolgt; verschiebt die minHand-Werte und
  damit einen Teil der zitierten Geometrie-Luecke.
- Etagen-Bedingung `entity+0x106 == [0x800cfcfe]` (@0x80102f60-70) im Port nicht abgebildet;
  ob sie in ROOM1210 irrelevant ist, ungeprueft.

## NAECHSTER SCHRITT

**Implementierungsreif: NICHTS von den drei Fixes.** Alle drei sind zu streichen:

- **Fix 1** — nicht "byte-gleich" (Messgroesse gewechselt) und mechanisch invertierend, weil
  `e->x` waehrend der Lunge wandert und `reach` zugleich das Rueckzugs-Tor ist.
- **Fix 2** — Bedingung trifft Bild 0 statt Bild 4.
- **Fix 3** — wirkungslos, falsch begruendet, kippt einen ⛔-Marker. **Enthaelt keine erfundene
  Zahl, aber verschiebt eine belegte Zahl an den falschen Gegner** — nach Regel 3 dasselbe
  Ergebnis: raus.

⛔ **Rahmen-Warnung:** REACH_Z/REACH_X ersatzlos zu streichen ergibt "alle zehn Arme reagieren"
— den vom Nutzer **verworfenen** Zustand. Die Sweep-Zeilen T_out 2600/3200 (9 bzw. 10 Arme)
liegen ebenfalls in dieser verbotenen Zone. Jede Zwischenloesung, die das Tor weitet, ist keine
Loesung.

**Was belastbar ist und bleiben darf:**
- Die Feststellung, dass REACH_Z 850 / REACH_X 11000 unbelegt sind (Befund haelt).
- Die RE2-Zahlen 1300 (@0x80102f3c), 1200 (@0x801018f4), Halbwinkel 256 (@0x80102f94), die
  Totheit der beiden `jal 0x80015614`, LUNGE_NET-Bausteine 3x800 (@0x8010c7b8) + 1x20
  (@0x8010c7f8) mit +0x9c=3 (@0x8010c7a8), Spieler-Tempo 75 (@0x80030b20) / 200 (@0x80030d58).

**Zuerst zu klaeren, bevor irgendein Tor angefasst wird:**

1. **Welche Lesart der Nutzer meint** — beim Nutzer nachfragen ist hier ausnahmsweise der
   guenstigste Weg, weil beide Fehlbilder gemessen vorliegen und der Fix je nach Antwort
   verschieden ausfaellt. Vorher: gdigrab-Aufnahme des echten Fensters
   (Skill `re15-port-visual-verify`), damit die Frage an einem Bild haengt.
2. **MESH_REACH 1671 neu messen** (`probe_1210_reach` Abschnitt 5). Es traegt schon heute das
   ausgelieferte Griff-Tor.
3. **Ein Lauf mit LEBENDER KI** statt der Modell-Sonde: die echte case-1/2/3-Maschine inkl. der
   -800 in Phase 2 (@0x8010c8b4) und der echten Rueckzugsbedingung (`!reach -> sub_state_1 = 3`),
   mit Riegel und Halte-Budget (RE15_WRITHER_HOLD_BUDGET 148 @0x80102828-2C). Erst danach hat
   irgendeine Wirkungszahl Wert.
4. **Anker-Frage entscheiden**: wenn ein Reichweiten-Tor auf die Hand zeigen soll, muss es an
   der **Heimatposition** des Arms haengen (`s_writher_home_x/z`), nicht an `e->x`. Das ist eine
   Nachruestung und muss so benannt werden — RE2 kennt gar kein Ausfahr-Tor.
5. **Sektor-Form**: entweder pro Arm EIN Sektor (Halbwinkel 256, entsprechend RE2s Arm-Bit)
   statt der ODER-Verknuepfung, oder der Setzer von +0x21A Bit 0x20/0x40 wird RE't und die
   Uebertragung belegt. Betrifft auch das bestehende Griff-Tor.
6. **Wache**: `test_1210_gitterhaende` parkt den Spieler 400 Bilder an der Wand — nur deshalb
   feuert der Griff dort ueberhaupt. Um einen Constrain-LAUF erweitern, sonst haelt sie den
   Kernfehler weiterhin nicht fest.

**⛔ Ausdruecklich nicht einbauen** (vom Befund selbst korrekt ausgeschlossen und von der
Gegenmessung bestaetigt): den rechnerischen Vorlauf `T_out = T_grab + 4*Tempo` (1600/2100).
Er macht in der Flurmitte aus 0 ausfahrenden Armen 10, die nicht greifen koennen.
