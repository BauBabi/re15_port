# Messung: Hunde-Posenriegel Bit 0x80 @ +0x1D3 — trifft er den Nutzerfall, und wen sperrt er?

Status: **FERTIG** (2026-09-26). Ergebnis ist eine MESSUNG, kein Fix — es wurde kein
Engine-Code geändert.
Arbeitsbaum: `.claude/worktrees/wf_19e49a69-d81-4`
Sonde: `re15_port/tests/unit/probe_r26_pose80.c` + `re15_port/tests/unit/probes/r26_pose80.cmake`
Werkzeuge (selbst geschrieben): `tools/scan1d3.py` (findet JEDEN `sb rt,0x1d3(rs)` in einem
rohen Overlay-Image samt Kontext), `tools/scanjal.py` (jal/j/Pointer-Referenzen),
`tools/scanhalf.py` (`sh rt,off(rs)`).

## Kurzurteil in vier Sätzen

1. **ARGUMENT 1 des Prüfers hält, mein Einwand fällt** — und zwar gemessen, nicht argumentiert:
   der Hund geht beim gewöhnlichen Kugeltreffer sehr wohl zu Boden, trägt dabei aber **kein**
   Bit 0x80 (Original: kein Store auf +0x1D3 im ganzen generischen Hurt-Fächer; Port gemessen:
   Treffer-Byte `0x0D`, Bit 0x80 in **0 von 240** Bildern).
2. **ARGUMENT 2 hält ebenfalls, ist aber untertrieben**: nicht nur Spinne und Devour, sondern
   **alle drei Nicht-Hund-Typen** (0x21, 0x25, 0x26) haben im Port **keine erreichbare
   Freigabe** — gemessen 0 Freigaben in je 600 Bildern auf dem echten Pfad.
3. Beide Vorarbeiten haben die Setzer/Löscher **per Muster** gezählt und deshalb zu wenige
   gefunden. Die Vollzählung aller `sb …,0x1d3(…)` ergibt **17** Stellen im Hunde-Overlay
   (nicht 9 und nicht 12), **7** bei der Krähe, **4** bei 0x25 und **5** bei 0x26.
4. Die Umsetzung, die die Messung trägt, ist **nur Typ 0x20** — und sie ändert am gemeldeten
   Nutzerfall (gewöhnlicher Schuss) **nachweislich nichts**.

---

## 0. Vollzählung statt Stichprobe — die Methode

Beide Vorarbeiten haben Setzer/Löscher per **Muster-Suche** gezählt (`ori …,0x80` bzw.
`andi …,0x7f` direkt vor dem Store). Das verfehlt jeden Schreiber, der das Byte **direkt**
setzt (`addiu rX,zero,0x80` / `…,0xff` / `sb zero`). Ich habe deshalb ALLE
`sb rt,0x1d3(rs)` jedes Overlays gezählt — das ist erschöpfend, weil ein Byte nur per `sb`
geschrieben werden kann.

| Overlay | Datei (Ladebasis 0x80100000, headerlos) | Stores auf +0x1D3 |
|---|---|---|
| Hund 0x20 | `info/re2leon/COMMON/BIN/EMD0G_MOD0.BIN` | **17** |
| Krähe 0x21 | `info/re2leon/COMMON/BIN/EMOVL21_S0.BIN` | **7** |
| Typ 0x25 | `info/re2leon/COMMON/BIN/EMS25.BIN` | **4** |
| Typ 0x26 | `info/re2leon/COMMON/BIN/EMS26.BIN` | **5** |

Gegenprobe der Ladeadresse (Hund): `lui at,0x8010` @0x80100064 + `lw v0,21560(at)`
@0x8010006C ⇒ Zustandstabelle @0x80105438 mit lauter gültigen Overlay-Zeigern
(0x801000F4 / 0x801004DC / 0x801032A8 / 0x801040DC / 0x801049E4 / 0 / 0 / 0x801049EC).

Bestätigt (selbst nachgelesen, RE2 `info/re2leon/PSX.EXE`, RAM = 0x80010000 + off − 0x800):
das Gate ist unmaskiert — `lbu v0,467(s0)` @0x80047138, `nop` @0x8004713C,
`bne v0,zero,0x8004740c` @0x80047140. Und der Stempel rettet Bit 0x80 hinüber:
`lbu a0,467(s1)` @0x8004731C / `sb s5,5(s1)` @0x80047324 / `andi a0,a0,0x80` @0x8004732C /
`sb a0,467(s1)` @0x80047334 / `lw v0,4(a1)` @0x80047338 / `srl v0,v0,9` @0x80047340 /
`andi v0,v0,0x7f` @0x80047344 / `or a0,a0,v0` @0x80047348 / `sb a0,467(s1)` @0x8004734C.

---

## 1. HUND 0x20 — vollständige Bilanz (17 Stores)

### 1.1 SETZER (7)

| # | Store | Setz-Instruktion | Wert | Funktion / Rolle | im Port? |
|---|---|---|---|---|---|
| S1 | @0x80102CEC | `ori v0,v0,0x80` @0x80102CE4 | \|=0x80 | 0x80102C78 — Sprung/Satz Phase 0 (ACTIVE-Sub 14 **und** 15) | ja, `enemy_ai_re2_dog.c:1436` |
| S2 | @0x8010318C | `ori v0,v0,0x80` @0x80103188 | \|=0x80 | 0x80103138 — ACTIVE-Sub 16 Phase 0 | **NEIN** |
| S3 | @0x801038A4 | `ori v1,v1,0x80` @0x8010389C | \|=0x80 | 0x801037E8 — HP-Neuwurf / Wieder-Einsatz | ja, `dog.c:1789` |
| S4 | @0x80103E3C | `ori v0,v0,0x80` @0x80103E34 | \|=0x80 | 0x80103D9C — Hurt-Zeilen 10 und 16 | ja, `dog.c:1938` |
| S5 | @0x80104F34 | `addiu v1,zero,0xff` @0x80104F20 | **=0xFF** | 0x80104DF0 — Fress-Kette | **NEIN** |
| S6 | @0x80104F74 | `ori v0,v0,0x80` @0x80104F6C | \|=0x80 | 0x80104DF0 — Fress-Kette, zweiter Schreiber | ja, `dog.c:566` |
| S7 | @0x80105200 | `ori v1,v1,0x80` @0x801051F8 | \|=0x80 | **0x801051BC** — allgemeine „geh in HURT"-Hilfe | **NEIN** |

S7 im Wortlaut (Store im Delay-Slot von `jr ra`):
```
801051bc: beq   a0,zero,0x801051d4
801051c8: lw    a0,-488(a0)        ; 0x800cfe18
801051d8: addiu a0,a0,-1032        ; 0x800cfbf8
801051dc: lbu   v1,467(a0)
801051e0: addiu v0,zero,2
801051e4: sb    v0,4(a0)           ; state = 2 (HURT)
801051e8: addiu v0,a1,2
801051ec: sb    v0,5(a0)           ; sub = a1 + 2
801051f0: sb    zero,6(a0)
801051f4: sb    zero,7(a0)
801051f8: ori   v1,v1,0x80         ; RIEGEL AN
801051fc: jr    ra
80105200: sb    v1,467(a0)         ; (Delay-Slot)
```
Aufrufer: `jal 0x801051bc` @0x801043F4 und @0x80104FA4, beide unmittelbar hinter
`jal 0x80015910` (Richtungs-/Reaktionsberechnung in der EXE).

### 1.2 LÖSCHER (9) + 1 Dekrement

| # | Store | Lösch-Instruktion | Funktion | im Port? |
|---|---|---|---|---|
| D0 | @0x8010003C | `andi v0,v1,0x7f` @0x80100030 (Gate) / `addiu v0,v1,-1` @0x80100038 | Root-Dekrement — **kein** Löscher von Bit 0x80 | ja, `dog.c:2224-25` |
| C1 | @0x80100608 | `andi v0,v0,0x7f` @0x80100600 | 0x80100548 | **NEIN** |
| C2 | @0x80100894 | `andi v0,v0,0x7f` @0x80100890 | **0x80100824 = ACTIVE-Sub 1, Eintritt** | ja, `dog.c:753` |
| C3 | @0x80100C08 | `andi v0,v0,0x7f` @0x80100C00 | 0x80100BA8 | ja, `dog.c:809` |
| C4 | @0x80102E40 | `andi v1,v1,0x7f` @0x80102E3C | 0x80102E0C — Lande-Clip Ende (Sub 14) | ja, `dog.c:1474` |
| C5 | @0x80102FA4 | `andi v0,v0,0x7f` @0x80102FA0 | 0x80102F74 — Lande-Clip Ende (Sub 15) | ja, `dog.c:1502` |
| C6 | @0x801030B0 | `andi v0,v0,0x7f` @0x801030AC | 0x80102FBC | ja, `dog.c:1518` |
| C7 | @0x80103280 | `andi v0,v0,0x7f` @0x8010327C | 0x8010321C — Hurt-Zeile 0 | ja, `dog.c:1960` |
| C8 | @0x80103718 | `andi v0,v0,0x7f` @0x80103710 | 0x801035E8 — Hurt-Phase 2, Unterphase 2 (Ende Aufstehen) | ja, `dog.c:1621` |
| C9 | @0x801037A8 | `andi v1,v1,0x7f` @0x801037A4 | 0x8010376C — Hurt-Phase 3 | ja, `dog.c:1655` |

**Bilanz Original 7/9, Bilanz Port 4/7.** Im Port fehlen S2, S5, S7 und C1.

### 1.3 Der Riegel heißt „nicht auf den Beinen" — Beleg C2

ACTIVE-Sub 1 (Tabelle @0x80105464, Index 1 ⇒ 0x80100824) räumt beim Eintritt zwei Dinge
gemeinsam ab:
```
80100884: andi  v1,v1,0xfd      ; +0x1C0 &= ~0x2
80100888: sb    v1,448(s0)
80100890: andi  v0,v0,0x7f      ; +0x1D3 &= ~0x80
80100894: sb    v0,467(s0)
```
und der Sprung-Setzer S1 setzt exakt dieselben zwei Bits gemeinsam
(`ori v0,v0,0x80` @0x80102CE4 / `ori v1,v1,0x2` @0x80102CE8, Stores @0x80102CEC / @0x80102CF4).

### 1.4 ARGUMENT 1 — Wege zu Boden, und ob Bit 0x80 dabei steht

Die HURT-Zeile kommt aus +0x5 (`sb s5,5(s1)` @0x80047324). Zeilen-Tabelle @0x80105538,
20 Einträge: `[0]=0x8010321C · [1,2,3,5,6,7,8,12,13]=0x80103308 (generisch) ·
[4,15,18]=0x80103FF4 · [9,17,19]=0x80103CE4 · [10,16]=0x80103D9C · [11]=0x80103E60 ·
[14]=0x80103F00`. Der generische Zweig fächert per +0x6 über @0x80105588:
`[0]=0x80103344 [1]=0x801034C8 [2]=0x801035E8 [3]=0x8010376C [4]=0x801037E8`.

| Weg zu Boden | Setzt das ORIGINAL Bit 0x80? | Beleg |
|---|---|---|
| Sprung/Satz, ACTIVE-Sub **14** | **JA** | `ori v0,v0,0x80` @0x80102CE4 / `sb` @0x80102CEC |
| Sprung/Satz, ACTIVE-Sub **15** | **JA** | dieselbe Funktion 0x80102C78 (Zeiger @0x80105514 **und** @0x80105520) |
| ACTIVE-Sub **16** | **JA** | `ori v0,v0,0x80` @0x80103188 / `sb` @0x8010318C |
| **Gewöhnlicher Kugeltreffer** (Zeilen 1,2,3,5,6,7,8,12,13) | **NEIN** | im Bereich 0x80103344…0x801035E8 gibt es **keinen einzigen** Store auf +0x1D3 (Vollzählung §0). Phase 0 setzt nur `+0x1C0 \|= 0x2` @0x801033C0/C4 und `+0x10E &= ~0x2000` @0x801033B4/B8 |
| Hurt-Zeile **0** | **NEIN — löscht** | `andi v0,v0,0x7f` @0x8010327C / `sb` @0x80103280 |
| Feuer-Zeilen **10 / 16** | **JA** | `ori v0,v0,0x80` @0x80103E34 / `sb v0,467(a0)` @0x80103E3C |
| **Fress-Kette** | **JA, zweimal** | `addiu v1,zero,0xff` @0x80104F20 + `sb` @0x80104F34, danach `ori v0,v0,0x80` @0x80104F6C + `sb` @0x80104F74 |
| **HP-Neuwurf** 0x801037E8 | **JA** | `ori v1,v1,0x80` @0x8010389C / `sb` @0x801038A4 |
| **HURT-Eintritts-Hilfe** 0x801051BC | **JA** | `ori v1,v1,0x80` @0x801051F8 / `sb` @0x80105200 |

⛔ **Mein Einwand fällt.** Der Hund geht beim gewöhnlichen Kugeltreffer sehr wohl zu Boden:
Phase 0 @0x80103344 setzt Clip `0x00070011`, Phase 2 @0x801035E8 spielt Clip 18
(`sw v1,332(s0)` @0x801036CC) und dann `0x000F0007` (`sw v0,332(s0)` @0x801036DC) und kehrt
über `sw v0,4(s0)` @0x80103748 nach ACTIVE zurück — **aber ohne Pose-Riegel**. Der Riegel
gehört im Original nicht zum Liegen nach einer Kugel, sondern zum Sprung, zum
Feuer-Treffer, zum Gefressenwerden, zum Wieder-Einsatz und zum Hilfe-Eintritt.

⛔ Und die Prüfer-Aussage *„0x80103894 ist ein unerreichbarer `default:`-Arm"* ist am
ORIGINAL falsch: `tools/scanjal.py` findet FÜNF Tabellenzeiger auf 0x801037E8 —
@0x80105598 (generischer Fächer [4]), @0x801055CC, @0x80105674, @0x80105694, @0x801056A4
(letzter Schritt dreier weiterer Phasen-Tabellen @0x80105668 / @0x80105688 / @0x80105698).
Was 0x801037E8 tut: HP neu würfeln (`jal 0x80015FE8` @0x8010382C/@0x80103850, Tabellen
@0x80105340 bzw. @0x80105360, `sh v0,342(s0)` @0x8010387C), `+0x151/152/153 = 13`
(@0x80103884-8C), `+0x0 &= 0xE3FFFFFF` (@0x80103874/78/98/A0) und Bit 0x80 setzen.

---

## 2. KRÄHE 0x21 (EMOVL21_S0.BIN, 7 Stores)

| Rolle | Store | Instruktion | Bedeutung | im Port? |
|---|---|---|---|---|
| Dekrement | @0x80100174 | `andi v0,v1,0x7f` @0x80100168 / `addiu v0,v1,-1` @0x80100170 | Bit 0x80 überlebt | ja, `crow.c:1785-86` |
| **SETZER 1** | @0x801004A8 | `lhu v0,270(s1)` @0x80100490 / `andi v0,v0,0x40` @0x80100498 / `beq` @0x8010049C / `addiu v1,zero,128` @0x801004A0 | **im ROOT, JEDES Bild**, direkt hinter `jalr v0` @0x80100488; setzt zusätzlich `+0x0 \|= 0x8` @0x801004AC | **NEIN** |
| LÖSCHER | @0x80100FD0 | `and v0,v0,-9` @0x80100FC0 (`+0x0 &= ~8`) / `andi v1,v1,0x7f` @0x80100FC4 | exaktes Gegenpaar zu Setzer 1 | ja, `crow.c:583` |
| SETZER 2 | @0x801039F4 | `addiu v0,zero,0x80` @0x801039F0 | Leichen-Router [0] Normal | ja, `crow.c:1514` |
| SETZER 3 | @0x80103C1C | `addiu v0,zero,0x80` @0x80103C18 | Leichen-Router [1] GIB | ja, `crow.c:1597` |
| SETZER 4 | @0x80103C88 | `addiu v0,zero,0x80` @0x80103C84 | Leichen-Router [2] Wand-Splat | ja, `crow.c:1618` |
| SETZER 5 | @0x80103F08 | `addiu v0,zero,0x80` @0x80103F04 | Leichen-Router [3] Launch | ja, `crow.c:1695` |

⇒ **fünf Setzer, ein Löscher** (Prüfer-Zahl bestätigt, Dossier-Zahl falsch). Vier sind
Leichen-Zustände. Der fünfte (@0x801004A8) ist der gefährliche — Root, jedes Bild, hängt
allein an +0x10E Bit 0x40 — und **fehlt im Port vollständig**.

## 3. TYP 0x25 (EMS25.BIN, 4) und TYP 0x26 (EMS26.BIN, 5)

| Typ | Rolle | Store | Instruktion | im Port? |
|---|---|---|---|---|
| 0x25 | Dekrement | @0x80100100 | `andi v0,v1,0x7f` @0x801000F4 / `addiu v0,v1,-1` @0x801000FC | ja, `spider.c:2963-64` |
| 0x25 | SETZER | @0x80103214 | `ori v1,v1,0x80` @0x80103208 | ja, `spider.c:1823` |
| 0x25 | LÖSCHER | @0x80103380 | `andi v1,v1,0x7f` @0x80103378 | ja, `spider.c:1848` |
| 0x25 | **SETZER (Hilfe)** | @0x80105F64 | `ori v1,v1,0x80` @0x80105F5C, `jr ra` @0x80105F60 — baugleich zur Hunde-Hilfe 0x801051BC | **NEIN** |
| 0x26 | Dekrement | @0x80100054 | `andi v0,v1,0x7f` @0x80100048 | ja, `spider.c:2837-38` |
| 0x26 | INIT | @0x801001B0 | `addiu v0,zero,6` @0x80100190 ⇒ `+0x1D3 = 6` | ja, `spider.c:2591` |
| 0x26 | **SETZER =0xFF** | @0x801007C8 | `addiu v0,zero,255` @0x801007C4 | ja, `spider.c:2714` |
| 0x26 | LÖSCHER =0 | @0x801007E4 | `sb zero,467(s0)`, nur wenn `beq a3,zero,0x80100850` @0x801007D8 NICHT nimmt (a3 = Rückgabe `jal 0x80101020` @0x801007CC) | ja, `spider.c:2717` |
| 0x26 | **SETZER (Hilfe)** | @0x8010101C | `ori v1,v1,0x80` @0x80101014, `jr ra` @0x80101018 | **NEIN** |

⛔ **Die 0xFF-Mine, hart belegt.** `+0x1D3 = 255` (@0x801007C8, und beim Hund @0x80104F34)
hat low-7 = 127. Das Root-Dekrement zieht ab, **solange low-7 != 0** — also 127 Bilder lang,
bis das Byte auf genau **0x80** steht. Dort bleibt es: low-7 = 0 ⇒ kein Dekrement mehr, und
der einzige Löscher läuft nur im Setz-Bild und nur wenn a3 != 0. Ein Ganz-Byte-Gate macht so
einen Gegner **dauerhaft** unverwundbar.

---

## 4. DIE MESSUNG — echter Pfad, `probe_r26_pose80.c`

Aufbau: `game_step` + Pad (R1 zielen, SQUARE feuern), echte RDT, RE2-KI-Geschmack,
Gegner künstlich am Leben gehalten (`hp = 30000`), **kein nachgebauter Filter**.
Bau: `build_pose80` (eigenes Verzeichnis), Lauf `build_pose80/pose80.log`.

**Gesamtabdeckung: 11 666 getickte Bilder auf dem echten Pfad, 260 Treffer-Stempel.**

### 4.1 PASS A — Dauerfeuer, 900 Bilder je Typ (Abdeckung 5 × 900 = 4500)

| Typ / Raum | Bilder | Treffer-Stempel | Bilder mit Bit 0x80 | 0→1 | 1→0 |
|---|---|---|---|---|---|
| HUND 0x20 / ROOM1190 | 900 | 82 | **0** | 0 | 0 |
| KRÄHE 0x21 / ROOM10C0 | 900 | 55 | **0** | 0 | 0 |
| BABY 0x26 / ROOM1090 | 900 | 41 | **0** | 0 | 0 |
| FEUER 0x26 / ROOM1090 (ohne Baby) | 900 | **0** | 0 | 0 | 0 |
| SPINNE 0x25 | 900 | 82 | **0** | 0 | 0 |

⇒ Im normalen Spielbetrieb dieser fünf Szenarien wird Bit 0x80 **nie** gesetzt. Ein
Ganz-Byte-Gate würde hier nichts ändern. (Die Zeile „FEUER 0x26 ohne Baby" hat 0 Treffer —
für die Treffer-Frage ist sie ein Fehllauf und sagt nur etwas über die 900 Bilder Riegel.)

### 4.2 PASS B — Zwangsriegel: fällt Bit 0x80 auf dem echten Pfad wieder? (Abdeckung 2424)

Messänderung, **nicht der Fix**: `re2z_self1d3 = 0x80` von außen gesetzt (low-7 = 0, damit
nur der Pose-Riegel offen ist), danach läuft der echte Pfad **ohne** weitere Schüsse.

| Typ | Startzustand | Bilder | Riegel fiel |
|---|---|---|---|
| HUND 0x20 | st=4/0 | 24 | **ja, bei Bild 23** (danach st=1/2, Byte 0x00) |
| KRÄHE 0x21 | st=1/0 | 600 | ⛔ **nie** (Ende st=1/0, Byte 0x80) |
| BABY 0x26 | st=1/0 | 600 | ⛔ **nie** |
| FEUER 0x26 | st=1/0 | 600 | ⛔ **nie** |
| SPINNE 0x25 | st=1/4 | 600 | ⛔ **nie** (Ende st=1/4, Byte 0x80) |

⇒ **Nur beim Hund ist die Freigabe erreichbar.** Für 0x21/0x25/0x26 ist sie es nicht — genau
die Falle aus Runde 13/14. Der Prüfer nannte zwei Minen (Spinne 0x26, Devour); gemessen sind
es **drei Typen vollständig**.

### 4.3 PASS C — Hund, EIN gewöhnlicher Kugeltreffer (Abdeckung 240 + 1 Treffer)

```
Treffer bei f0: Byte 0x0D  (low7 = 13, bit80 = 0), Zustand st=2/3/2  (HURT)
240 Bilder danach: low7 erreichte 0 bei f7 (bit80 dort = 0)
                   bit80 stand in 0 von 240 Bildern
                   Endbyte 0x00, Endzustand st=1/0/1
```
⇒ **Der Nutzerfall, wie gemeldet, wird vom Pose-Riegel nicht berührt.** Der Hund ist nach
13 (gemessen: effektiv 7) Bildern wieder Kandidat, während er noch in der Aufsteh-Kette
steckt — und das Original macht es an dieser Stelle genauso, weil es Bit 0x80 dort gar nicht
setzt.

### 4.4 PASS D — Zustand erzwungen: setzt der PORT Bit 0x80? (Abdeckung 4502)

| Erzwungener Zustand | Bilder | Bit 0x80 |
|---|---|---|
| HUND ACTIVE-Sub 14 (Sprung) | 300 | gesetzt ab f0, **27 Bilder verriegelt, dann frei** (Ende st=1/0) |
| HUND ACTIVE-Sub 15 (Satz) | 300 | gesetzt ab f0, **27 Bilder verriegelt, dann frei** (Ende st=1/13) |
| HUND ACTIVE-Sub 16 | 300 | nie gesetzt (Setzer S2 fehlt im Port) |
| HUND HURT-Zeile 10 / 16 (Feuer) | 300 / 300 | nie gesetzt — **Messgrenze, s.u.** |
| HUND HURT-Zeile 1 / 0 | 300 / 300 | nie gesetzt (deckt sich mit dem Original) |
| KRÄHE state 7 sub 0/1/2/3 (Leichen-Router) | 4 × 300 | gesetzt ab f0, **300 von 300 Bildern verriegelt, nie frei** |
| KRÄHE state 3 (DEATH) | 300 | nie gesetzt |
| BABY 0x26 state 1/2/3/7 | 300/300/2/300 | nie gesetzt |

⛔ **Messgrenze, ehrlich benannt:** „HURT-Zeile 10/16 nie gesetzt" ist ein Artefakt der
Methode. Der Port setzt dort in einem `switch`-Arm, der **beim Eintritt** in HURT läuft
(`enemy_ai_re2_dog.c:1930-1939`); ein von außen gesetzter Zustand überspringt diesen
Eintritt. Diese Zeile sagt also **nichts** über die Erreichbarkeit im Spiel. Für Sub 14/15
gilt das nicht — dort läuft der Setzer im Phasen-Handler und hat gemessen ausgelöst.

---

## 5. Antworten auf die zwei Streitfragen

**ARGUMENT 1 — „erklärt den Befund nicht": der Prüfer hat recht.**
Der liegende Hund trägt Bit 0x80 **nur** nach Sprung/Satz (Sub 14/15/16), nach einem
Feuer-Treffer (Zeilen 10/16), beim Gefressenwerden und beim Wieder-Einsatz. Nach
Pistole/Schrot/Magnum liegt er **ohne** Riegel — im Original wie im Port, letzteres mit
240 Bildern Abdeckung gemessen. Mein Einwand („der Nutzer spricht vom liegenden Hund, genau
dort wird das Bit gesetzt") ist damit **widerlegt**.

**ARGUMENT 2 — „macht andere Gegner dauerhaft unverwundbar": der Prüfer hat recht, und es
ist schlimmer als beschrieben.** Gemessen hat **keiner** der drei Nicht-Hund-Typen eine
erreichbare Freigabe (0 von 600 Bildern, je Typ). Dazu kommen zwei 0xFF-Schreiber
(@0x801007C8 bei 0x26, @0x80104F34 beim Hund), die nach 127 Bildern Dekrement auf exakt
0x80 stehenbleiben und dort für immer bleiben.

---

## 6. Umsetzung — so eng, wie die Messung sie trägt

**(a) Was jetzt gebaut werden darf: NICHTS am Gate für 0x21/0x25/0x26.** Die Messung verbietet
es (Pass B: 0 Freigaben in je 600 Bildern).

**(b) Für Typ 0x20 wäre das Ganz-Byte-Gate sicher** — Pass B misst die Freigabe nach 23
Bildern, Pass D nach 27 Bildern im Sprung. Konkret: `re15_damage.c:3098`
`if ((e->re2z_self1d3 & 0x7fu) == 0u)` → für `e->type == 0x20` auf das ganze Byte
(Original `lbu v0,467(s0)` @0x80047138 / `bne v0,zero` @0x80047140, unmaskiert; `nop`
dazwischen @0x8004713C) und `enemy_ai_re2_dog.c:2226/2255` entsprechend.
**Aber:** Pass A zeigt, dass der Riegel in 900 Bildern Dauerfeuer in ROOM1190 **nie** stand
— der Nutzen dieser Änderung ist damit auf den Sprung-Hund beschränkt, und der gemeldete
Nutzerfall ändert sich **nicht** (Pass C).

**(c) Die fünf Port-Lücken gegen RE2-Retail — das ist der eigentliche Bau-Auftrag:**
| fehlt im Port | Original | Wirkung |
|---|---|---|
| S2 | @0x80103188-8C | ACTIVE-Sub 16 des Hundes setzt den Riegel nicht (Port-Stub) |
| S5 | @0x80104F20/34 (=0xFF) | Fress-Kette stempelt im Original zusätzlich 127 Stun-Bilder |
| S7 | @0x801051BC/@0x80105200 | **allgemeiner HURT-Eintritt mit Pose-Riegel fehlt ganz** |
| C1 | @0x80100600-08 | ein Freigeber fehlt |
| Krähe-Setzer 5 | @0x801004A0-A8 | Root-Riegel an +0x10E Bit 0x40 fehlt ganz |
| 0x25/0x26-Hilfe | @0x80105F64 / @0x8010101C | dieselbe HURT-Eintritts-Hilfe wie S7 |
⛔ Diese sechs zuerst bauen (sie sind reine Fehlstellen gegen das Original), **danach** das
Gate — sonst misst man wieder eine halbe Kette.

**(d) Einordnung nach dem Projektziel (RE1.5 = 40 %-Beta, RE2-Retail als Ziel, wo RE1.5
unfertig ist):** +0x1D3 ist ein **RE2-Retail-Mechanismus**; RE1.5 kennt an dieser Stelle
+0x93 (Voll-Scan Offset 147: 0 Treffer in EMD0G_MOD0.BIN, 0 in `info/re2leon/PSX.EXE` —
Vorarbeit, von mir nicht nachgefahren). Der Port fährt für diese Typen ohnehin die RE2-KI
(Default seit 2026-08-22). **RE2-Retail ist hier also maßgeblich**, und die sechs Fehlstellen
aus (c) sind echte Lücken, kein „das Original macht das auch nicht".

---

## 7. Was NICHT gefunden ist — und der nächste Weg

**Die Ursache der Nutzer-Beobachtung ist NICHT gefunden.** Gemessen ist nur, dass +0x1D3
Bit 0x80 sie nicht erklärt.

Versucht: (A) Vollzählung aller +0x1D3-Schreiber in vier Overlays; (B) Rückverfolgung des
generischen Hurt-Fächers 0x80103308 → 0x80103344/0x801034C8/0x801035E8/0x8010376C/0x801037E8;
(C) Gate 4 des Appliers (`lhu v0,270(s0)` @0x80047158 / `andi v0,v0,0xc000` @0x80047160 /
`bne` @0x80047164) — das Hunde-Overlay schreibt +0x10E nur **dreimal** und immer nur
`andi …,0xdfff` (@0x80101A38/40, @0x801033B4/B8, @0x80103700/04), setzt die Bits 0xC000 also
**nie**; (D) Messung des Ports über 11 666 Bilder.

**Nächster Weg D:** die Frage muss am ORIGINAL entschieden werden, nicht am Port —
DuckStation-Mitschnitt von +0x1D3, +0x10E und +0x156 eines echten RE2-Hundes **je Bild** über
einen gewöhnlichen Pistolentreffer hinweg (`re15-room-capture` → Hunde-Flur, dann
`re15-savestate-ghidra`). Bleibt der Hund dort trotz `+0x1D3 == 0` untreffbar, liegt der
Mechanismus in Gate 1 (`andi v0,v0,0x1` @0x8004712C auf +0x0) oder Gate 4 — und deren Setzer
liegen dann in der EXE, nicht im Overlay. Das ist der nächste Schritt; ich fülle die Lücke
hier nicht mit einer Zahl.
