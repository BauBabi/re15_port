# 1210-geraeusch — "es fehlt das laute Geraeusch wie bei RE 2"

## URSACHE (belegt)

### A. Der Original-Writher ist VOLLSTAENDIG STUMM

Eigener jal-Zensus ueber 0x8010c1ec..0x8010d800 (STAGE1.BIN, 1381 Instruktionen). Ziele:
```
0x8001af20 x15, 0x8001f314 x11, 0x800245d8 x5, 0x80019700 x3,
0x80065f60, 0x8001bd60, 0x8002b498, 0x8002aec4, 0x8002b544, 0x8003b0a4, 0x8001af5c
```
**`jal 0x800453d0` (Se): 0 Treffer.** Auch der Skript-Pfad ist stumm: sub02 @0x1ec8 enthaelt nur
Opcode 0x22, 0x46, 0x2e, 0x34 — kein Audio-Opcode (0x57).

⇒ **Jeder Laut an diesen Armen ist eine Nachruestung.** Das gilt fuer den Zupack-Laut genauso wie
fuer einen Ausfahr-Laut. Der Befund vergibt fuer Schritt 1 die Note "doppelt belegt" und nur fuer
Schritt 3 "bewusste Erfindung" — **das ist eine inkonsistente Etikettierung.** Belegt sind die
beiden **Vorlagen**-Instruktionen, nicht dass der Arm hier einen Laut hat.

### B. Die beiden Vorlagen — beide von mir verifiziert

RE1.5-Zombie, Griff-Einstieg, **unbedingt**:
```
80102684: ori a0,zero,0x4
80102688: ori v0,v0,0x1
8010268c: jal 0x800453d0
```
RE2-Zombie, Griff-Einstieg, mit Wurf und Cooldown (EMOVL10_S0.BIN):
```
801027a4: andi v0,v0,0x1
801027a8: beq  v0,zero,0x801027b4
801027ac: addiu a0,zero,11
801027b0: addiu a0,zero,10
801027b4: jal 0x8005bd6c
801027bc: addiu v0,zero,150
801027c0: sb   v0,569(s1)       ; +0x239 = 150
```
**BELEGT.** Cooldown-Tor davor @0x8010278c/94.

### C. Die "250" beim Biss ist VIBRATION, keine Lautstaerke

`addiu a1,zero,250` @0x801028b4 ist der 2. Parameter von `FUN_80039514`; ein Pruefer hat in
`info/re2leon/PSX.EXE` verifiziert, dass das Byte in `[1]` eines Rumble-Ring-Records landet
(`addiu s0,s0,-21464` = 0x800eac28 @0x80039538, `sb s2,1(v0)` @0x80039580) und dass
`FUN_80038bbc` die beiden Ringe per PadSetAct-Thunk @0x80038c90 an Port 0 schickt, gegatet auf
Pad-Typ 4/7. **Ein-Pruefer-verifiziert, von mir nicht nachgemessen** (die `--bin`-Aufloesung auf
die RE2-EXE hat in meiner Sitzung nicht gegriffen). Die Deutung ist damit gut gestuetzt, aber
nicht doppelt.

### D. Das Prio-Tor — von mir selbst disassembliert, und es sagt etwas ANDERES als der Befund

```
80045a30: andi a1,a1,0x7        ; NEUE Prio wird auf 3 Bit maskiert
80045a34: sltu v0,a1,v1         ; neu(&7) < laufend ?
80045a38: beq  v0,zero,0x80045a48
80045a44: ori  v0,zero,0x1      ;   -> VERWERFEN
80045a48: bne  v1,a1,0x80045a5c ; ungleich -> durchlassen
80045a50: andi v0,a2,0xff       ; GLEICHSTAND: volles Nibble
80045a54: sltiu v0,v0,0x8
80045a58: xori v0,v0,0x1        ;   Nibble >= 8 -> VERWERFEN
```
ROOM1210 snd1 (@Datei 0x382c / 0x3830): SE4 = `00 00 5b 15` -> Nibble **0xb**, Stimme 5;
SE5 = `00 00 69 15` -> Nibble **9**, Stimme 5.

⇒ SE4 verdraengt ein laufendes SE5 (3 > 1). **Aber SE4 verwirft sich bei Gleichstand selbst**,
weil 0xb = 11 >= 8. **BELEGT.**

---

## BESTRITTEN — und von mir entschieden

| Behauptung | Entscheidung | Beleg |
|---|---|---|
| **Headline: "Der verankerte RE2-Zombie gibt UEBERHAUPT KEINEN Sound ab"** | **WIDERLEGT** | Geprueft wurden nur Substate 7 und 8. Ich habe die Dispatch-Tabelle selbst dekodiert (`801011e4: lw v0,-14132(at)` -> 0x8010c8cc): **Eintrag [6] = 0x80103954**. Dessen `+0x6==0`-Kante (@0x801039b0) ruft `jal 0x8005bd6c` @0x801039f0 mit Id 10/11 (`andi v0,v0,0x1` @0x801039e0, `addiu a0,zero,11` @0x801039e8 / `addiu a0,zero,10` @0x801039ec) — **ohne ROOT-Test (`andi 0x4000`) und ohne Cooldown-Tor**. Die Headline ist nicht bewiesen; sie traegt die Widerlegung des Vor-Dossiers, und die faellt mit ihr. |
| "SE4 ... traegt die hoechste Prioritaet und wird deshalb nie vom Prio-Tor verworfen" / "wird garantiert gekeyt" | **WIDERLEGT** | @0x80045a50-58, s.o.: Nibble 0xb = 11 >= 8 ⇒ Verwurf bei Gleichstand. Bei zehn Armen ist Gleichstand der Regelfall. Diese falsche Begruendung steht bereits im Produktivcode-Kommentar. |
| "Der Port-Arm (case 4) spielt beim Zupacken nichts" | **WIDERLEGT** | Der Griff-Uebergang liegt in **case 2** (`enemy_ai_common.c:11215-11298`), nicht case 4. Und der Arbeitsbaum hat dort bereits `re15_audio_room_se(4);` (Zeile ~11290). Ein Einbau im case-4-Eintritt wuerde im selben Bild einen ZWEITEN SE feuern. |
| Beleg "sub02 @Datei 0x1ED6..0x1F25 besteht ausschliesslich aus 10x 2e/34" | **Offset falsch** | Sub-Tabelle @0x1ea0 = `06 00 26 00 28 00` ⇒ sub02 beginnt bei **0x1ec8**. Die weggelassenen 14 Bytes sind `22 03 2c 01` und `46 06 00 00 00 00 00 00 00 00`. Schlussfolgerung (kein Audio-Opcode) ueberlebt. |
| Beleg "0x80102920 + 0x80102960 `ori a0,zero,0x7`" | **falsch abgeschrieben** | @0x8010291c steht `ori v0,zero,0x7`, benutzt als `sb v0,143(v1)` (+0x8f). a0=7 wird @0x8010290c / @0x80102958 geladen. Aussage (Se(7) zweimal) haltbar, Zitat nicht. |
| "verankert zykelt +0x6 nur 1<->2" (Substate 8) | **Mechanismus falsch, Folgerung richtig** | Der `+0x6==1`-Handler springt bei ROOT @0x80103ca0 in den Epilog; der `+0x6==2`-Handler wird nie betreten. Der Sound in +0x6=3/4 bleibt unerreichbar. |
| "MESSUNG A zeigt, dass der Griff-SE fehlt" | **vakuant** | Das eigene Phasen-Histogramm (2538+390+62+10 = 3000 = 10 Arme x 300 Bilder) laesst **null** Bilder fuer case 4 — in der Messung kam kein einziger Griff zustande. Die einzige Messung mit Griff (B) setzt `sub_state_1 = 2` **von Hand** und ueberspringt damit die Lunge. |

---

## OFFEN

1. ⛔ **Die Id-Verschiebung, die keine der sechs Analysen zitiert.** Von mir gefunden:
   ```
   80045404: lw   v0,0(v0)          ; word0 von *(0x800ac784)
   8004540c: andi v0,v0,0x2000
   80045410: beq  v0,zero,0x8004541c
   80045418: addiu s5,a0,12         ; ID += 12
   80045420: sltiu v0,v1,0x19       ; und danach: id < 25, sonst Abbruch
   ```
   Ist Bit 0x2000 im Griff-Moment gesetzt, trifft `Se(4)` **Record 16**, nicht Record 4.
   **Die gesamte "SE4 = Tone 5 = Stimme 5 = Nibble 0xb"-Kette haengt an diesem ungeprueften
   Gate.** Naechster Weg: Schreiber von 0x800ac784[0] Bit 0x2000 disassemblieren
   (der Port dokumentiert das Gate in `platform/pc/src/audio_pc.c:788/793`).
2. **Hoerbarkeit ist ungemessen.** Die Sonde laeuft gegen `re15_audio_room_se` aus
   `tests/test_support.c` = reines Logging. Weder SE4 noch SE5 sind je durch
   `s_se_loaded` / Prio-Tor / Stimmen-Pumpe gemessen worden. Weg: `re15_pc.exe` mit
   `RE15_SE_DEBUG=1` + `RE15_DEBUG_JUMP="1210@<frame>"` + `RE15_INPUT_SCRIPT`, die `[se]`- und
   GATE-Zeilen lesen.
3. **Lautstaerke ist strukturell falsch, unabhaengig von der Id.** Das Original rechnet voll/volr
   positional aus der sendenden Entity (`FUN_800453d0` ruft `FUN_80045a64` @0x800454e0-ec,
   PAN_LUT @0x80074728, ATT_LUT @0x800747a8). Der Port hat in `re15_audio_room_se(id)` keinen
   Emitter. Solange das offen ist, kann **kein** Fix eine Aussage ueber die tatsaechliche
   Lautstaerke machen — auch der eingebaute nicht.
4. **Erreichbarkeit von Substate 6** (0x80103954) fuer den verankerten RE2-Zombie. Weg: den
   `+0x5`-Setzer-Baum 0x8010a4c0..0x8010a820 disassemblieren, insbesondere den **relativen**
   Schreiber `addu v1,v1,v0` / `sb v1,5(s0)` @0x8010a7fc-804.
5. **Welche RE2-Szene der Nutzer meint, ist unbekannt.** Damit fehlt das Vorbild, an dem "laut"
   ueberhaupt gemessen werden koennte. Weg: sauberer SCD-Walk (kein Muster-Scan) ueber
   `info/re2leon/PL0/RDT` nach `Sce_em_set` (0x44) mit `+0x10E & 0x4000`.
6. **`0x8001bd60`** (aus dem Writher-Root @0x8010c288) bleibt unidentifiziert. Ein Pruefer hat
   nachgesehen, dass es nur 0x8003b7f0 ruft, also kein Sound — von mir nicht nachgeprueft.
7. **ROOM1210 snd0** ist als SE-Bank praktisch tot (VH progs=1/tones=7/vags=5, EDT-Ids 5 und
   10..18 zeigen auf nicht existierende Tones). Ob Datenbefund oder Reader-Fehler des Ports —
   nicht geklaert. Betrifft potenziell auch die Schrittgeraeusche in diesem Raum.

---

## NAECHSTER SCHRITT

**Implementierungsreif (mit `@0x`):**

- **Das fehlende Cooldown-Tor am eingebauten Griff-SE.** Der Port feuert `re15_audio_room_se(4)`
  in `case 2` **unbedingt**; die RE2-Vorlage gatet an derselben Stelle
  (`lbu v0,569(s1)` @0x8010278c / `bne v0,zero` @0x80102794) und setzt danach
  `+0x239 = 150` (@0x801027bc-c0). Gemessen (Gegensonde): ohne Tor wiederholt sich
  Lunge -> Tor -> Abbruch alle ~43 Bilder, 7x SE(4) in 300 Bildern — inklusive Faellen, in denen
  case 4 sofort wieder abbricht (`g_player_victim == 0`), also **Greif-Laut ohne Griff**.
  ⛔ **Aber:** `re2z_cd239` ist dasselbe Feld, das der Stoehner in `case 2` unmittelbar davor auf
  150 setzt. Ein gemeinsames Tor wuerde die beiden gegenseitig unterdruecken. Das ist eine
  **Entscheidung**, keine Ableitung — sie muss gemessen und benannt werden, nicht geraten.
- **Kommentar-Korrektur** (kein Verhalten): der Satz "SE4 ... wird nie vom Prio-Tor verworfen"
  ist falsch (@0x80045a50-58); der Griff-SE ist genau so eine **Nachruestung** wie ein
  Ausfahr-Laut (Zensus 0x8010c1ec..0x8010d800: 0 Se-Aufrufe).

**GESTRICHEN:**

- **"Schritt 3" (Laut beim Ausfahren)** — ohne Beleg in RE1.5 wie in RE2. Der Befund schlaegt
  korrekt keine Zahl vor; es bleibt draussen.
- **"Schritt 2" (Stoehner als Paar 4/5) in seiner Begruendung** — die Wirkungsbehauptung
  ("die Haelfte landet auf SE4 und wird garantiert gekeyt") ist widerlegt. Die *Form* des Wurfs
  ist zwar belegt (`andi v0,v0,0x3` @0x80102e30 / `andi v0,v0,0x1` @0x80102e44 /
  `ori a0,zero,0x5` @0x80102e4c / `ori a0,zero,0x4` @0x80102e50), aber sie stammt aus dem
  RE1.5-**Zombie**, nicht aus dem Writher — also erneut Nachruestung, und ohne Hoerbarkeits-
  Messung (OFFEN 2/3) ist "macht lauter" unbelegt. **Erst messen, dann entscheiden.**
- **Ein Rumble-Port** waere aus @0x801027c4-d4 / @0x801028b0-e4 zitierbar, ist aber eine eigene
  Entscheidung und beruehrt den Sound-Befund nicht. Ausserdem ungeprueft, ob RE1.5 (1996, vor dem
  DualShock) ueberhaupt eine Vibrations-Maschine hat.
