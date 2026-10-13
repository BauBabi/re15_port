# Runde 37 Spur L "re2zombie" — Abnahme 1 (unabhaengig, nach Nachbesserung 1)

Baum `.claude/worktrees/r37_re2zombie`, Zweig `r37/re2zombie`, gepruefter Stand `d3f7417e` (Basis `e399ee3f`; Code-Stand
`ab2bf2b9`, `git diff ab2bf2b9 -- re15_port` leer — selbst geprueft). Abnahme 2026-10-14. Alles unten ist SELBST gemessen bzw.
SELBST disassembliert; aus Abnahme 0 ist nichts ungeprueft uebernommen (wo ich mich auf sie beziehe, steht es dabei).
Mess-Skripte (Scratch, nicht eingecheckt): `abn1_lauf.sh` (Treiber), `abn1_state.py` (Wechsel/HP/Griffe je Raumbesuch),
`abn1_messer.py` (Messertreffer je Anmarsch).

## Ergebnis

| # | Punkt (Wortlaut, AUFTRAG.md) | Urteil | Kern-Beleg (selbst gemessen) |
|---|---|---|---|
| 1 | "die Zombies sind immer noch zu passiv ... nach Irons tod ... bleiben einfach nur Stumpf alle im Gang stehen ... In Resident Evil 2 bewegen sie sich doch auch teilweise aktiv im Gang" | **erfuellt** | echte Tuer 1150 -> 1130 nach der Montage (L1/L2/L3): alle fuenf gehen ab Bild 6 auf Leon zu; Arme hoch (1/2) bei 1991 / 1936 / 1978 / 4692 / 2346 (rennt), Griff aus 0x201 bei 1108 / 1193 / 1180, Griff aus dem Ausfall (D12) bei 1280; Biss: Leon-HP 100 -> -20. Basis-exe (B1): kein 1/2 |
| 2 | "das soll nicht nur da gelten, das darf generell für die Zombies gelten" | **erfuellt** | Normalspiel ohne Montage: Leon 1180 (L4), **Elza** 1181 (L5, PL04), Leon 1010 (L6): Arme hoch bei 1995/1991/1978, Griffe aus 0x201; Basis (B4/B6) nie 1/2. Mechanismus im gemeinsamen EXEC[1] |
| 3 | "schaue einfach nach wie Resident Evil 2 das macht" | **erfuellt** | (a)(b)(c)+D12 Zeile fuer Zeile gegen EMZ0.BIN (Abschnitt 3); RE2-Original-Rohdaten selbst ausgezaehlt: 1/1->1/2 bei 1999/3565/1987, Griff aus 1/2 bei 1180/1152, Rueckfall Takt 777 — Port 1991/1936/1978/1995 bzw. 4692, Griff 1193/1180/1108 |
| 4 | Orchestrator-Auftrag L: (a) drei 0x201-Produzenten, (b) DECISION[0] Block 3, (c) EXEC[0] P1; Zensus; exe: Arme hoch, auf Leon zu, greifen, beissen; Nahkampf nicht schlechter | **erfuellt** | Code = Bytes (Abschnitt 3); an der exe alle drei Produzenten ((2) 1991, (1) 4692/4283, (3) 2346) und D12 beobachtet; Nahkampf: Harness 368 -> 373, exe ueber alle 15 Schlagphasen gleich viele Messertreffer je Anmarsch (Abschnitt 2) |
| N1 | Nachbesserung 1 (Abnahme-0-M1: Messer-Satz an die exe-Messung) | **teilweise** | Satz wie verlangt umgeschrieben — aber die neue allgemeine Aussage "je Anmarsch weniger Schlaege" haelt an der exe nicht (**M2**), und der neue Mechanismus-Beleg beschreibt die RE2-Erholungsleiter falsch und nennt einen abweichenden Port-Weg "byte-true RE2" (**M1**) |

Gates: **RE-Gate verletzt** (M1: die zitierte Stelle @0x80105B38-40 enthaelt nicht das Behauptete — die Leiter endet im
Normalfall in 0x10101 @0x80105B44-50). Die Stichproben (b) @0x801013c8, (2) @0x80101EC4-F10, (c) @0x80101530-5A0, 0x4B0
@0x801018F4/@0x80102114 und der neue OFFEN-Fund @0x80105AA0-B0 stimmen. Vertrag haelt (Pfade, keine Ressourcen,
Treffer-Handler unberuehrt). Tests: Unit-Auswahl 231/231, Spur verbose 9/9, Integration 4/4 (r34_granaten, r35_granate,
r37_re2zombie, r37_zombieaktiv).

**bestanden: NEIN** — alle vier Nutzer-/Auftragspunkte sind im Verhalten erfuellt; die Nachbesserung 1 hat aber zwei
Aussage-Maengel ins Dossier gebracht (M1 RE-Beleg, M2 Messer-Aussage). Kein Code-Mangel der Spur L.

## Maengel

* **M1 — Nachbesserung 1, Mechanismus-Beleg Punkt 1/2 (`L_re2zombie.md` Abschnitt "Nachbesserung 1"; Commit d3f7417e
  "byte-true RE2") beschreibt die RE2-Erholungsleiter falsch.** Behauptet: Leiter "... sonst 0x101 (`addiu v0,zero,257`
  @0x80105B38 / `sw v0,4(s1)` @0x80105B40)", danach "Faellt die Leiter auf 0x101, laeuft im ersten Gang-Takt EXEC[1] P0 nach P1
  durch", "Das Verhalten ist byte-true RE2". Selbst disassembliert (EMZ0.BIN, Rohbytes Abschnitt 3): `80105b3c bne a0,v1,0x80105ba0`
  vergleicht `+0x218` (`lbu a0,536(s1)` @0x80105B30) mit `+0x14C` (`lbu v1,332(s1)` @0x80105B34); der 0x101-Store im Delay-Slot
  ist nur bei UNGLEICH das Ergebnis. Bei Gleichheit — der Normalfall, weil P0 desselben Handlers den Clip auf den Gang-Clip zwingt
  (@0x801054C8-D4) — schreibt das Original **0x10101** (@0x80105B44-50: Gang Phase 1 OHNE EXEC[1]-P0) mit +0x16B, +0x158 = 30,
  +0x16A, **+0x15A = rand + 300** (@0x80105B4C-80) und `jal 0x800152c8` @0x80105B98. Der Port `re2z_hit_main` case 3
  (`enemy_ai_re2_zombie.c` Z.5619, vorbestehend seit d39b815b) hat die Bedingung UMGEKEHRT (`motion != walkclip` -> 0x10101) und
  faehrt deshalb im Normalfall 0x101 -> EXEC[1] P0 (Clip-Neustart auf Zufallsbild, 5 Wuerfe, +0x15A = (rand>>3)+100 = 100..131
  statt 300..555). An BEIDEN exe gemessen: `L8_messer_W21.3` F1228 P3 `mo=2` (= Gang-Clip) -> F1229 `ss2=0` (0x101);
  `B7_messer_W20` F1234 -> F1235 `ss2=0` -> F1236 `af 22 -> 18` (P0-Zufallsbild). Wirkt auf die Aktivitaet: ja (Timer-Produzent
  (1) nach Treffern 3-4x frueher als in RE2; Zufallsstrom +2 Wuerfe je Erholung). Verlangt: (1) Beleg-Text korrigieren
  (vollstaendige Leiter @0x80105B30-B98; "P0 nach P1" ist der PORT-Weg, nicht RE2; "byte-true RE2" nur fuer das, was es ist);
  (2) die umgekehrte Bedingung als OFFEN mit Adresse, Wirkung und Mess-Zeilen eintragen und entweder schliessen (Treffer-Handler
  liegt ausserhalb VERTRAG §1.4 der Spur L -> Freigabe des Orchestrators + Riegel-Liste des Zufallsstroms) oder mit Adresse
  begruenden, warum nicht. Nachpruefbar: `re2_disasm.py dis 0x80105B14 40 --bin EMZ0.BIN`; Port Z.5614-5625; state.log-Zeilen oben.

* **M2 — Nutzer-Aussage zum Messer gilt an der exe nicht allgemein.** `L_re2zombie.md` "Fuer den Nutzer" Punkt 4 ("bleiben je
  Anmarsch WENIGER Schlaege, bevor ein Zombie zupackt ... 6 statt vorher 10 Messertreffer am ersten Zombie"), N2-Zusatz ("An der
  exe nimmt der erste anrueckende Zombie mit Spur L weniger Messertreffer") und Commit d3f7417e ("gleiche Schwelle, schnellerer
  Anmarsch, weniger Schlaege"). Selbst gemessen (Abschnitt 2, 18 Paare Spur L / Basis mit identischer Eingabe, alle 15
  Schlagphasen des 0,5-s-Takts + drei Versaetze): Anmarsch 1 **L 7 / Basis 4**, Anmarsch 2 (16 vollstaendige Paare) **L 31 /
  Basis 33**, alle Platz-5-Treffer bis Bild 1400 **43 / 41**, Griffe 35 / 35; je Phase mal einer mehr, mal einer weniger. Die
  "6 statt 10" sind die vier Laeufe der Abnahme 0 (eine Schlagphasen-Auswahl; vom Bau-Agenten nach eigener Angabe nicht
  wiederholt) — eine Stichprobe, keine Regel. Verlangt: Aussage auf das phasenrobuste Ergebnis umstellen (Messer trifft wie
  vorher; je Anmarsch im Mittel gleich viele Treffer, je nach Schlagtakt einer mehr oder weniger; der Zombie schliesst im
  Arme-hoch-Gang schneller, die Trefferreaktion haelt ihn je Schlag an) und die Mechanismus-Folgerung "also weniger Schlaege"
  streichen. Im selben Satz: "44 statt 32 Einheiten je Takt" ist die Summe |dx|+|dz| (`L_belege/re2_0x201_ausw.py`, Weg-Zaehler);
  die Wegstrecke je Takt ist 35,6 / 25,6 (selbst aus `re2_original_room20a0_jeder_takt.txt.gz`, gleiche Takt-Auswahl) —
  Verhaeltnis gleich, Zahl als Weg falsch. Nachpruefbar: Abschnitt 2 (Skript und Zaehlregel dort).

## Hinweise (kein Mangel dieser Spur)

* **H1 — neuer OFFEN-Fund des Bau-Agenten bestaetigt:** P3-Ausfall prueft im Original `0x800CFDCB & 0x80` vor dem 0xB01-Wurf
  (@0x80105AA0-B0, Bytes `0d 80 02 3c cb fd 42 90 00 00 00 00 80 00 42 30 09 00 40 14`), Port Z.5603-5605 nicht. Mit Adresse und
  Vertragsgrund im Dossier OFFEN — zusammen mit M1 (gleicher Handler, gleiche Phase) als ein Punkt behandeln.
* **H2 — Navigator-Tasche ROOM1030 und `probe_r36_blut` auf Maske 0** (Abnahme 0 H1/H2) bestehen unveraendert; im Dossier OFFEN.
* **H3 — gemeinsame Phase P3:** M1 und H1 liegen beide in `re2z_hit_main` case 3; ein Fix verschiebt den gemeinsamen
  RE2-Zufallsstrom in jedem Treffer-mit-Erholung-Fall (Messer-/Granaten-/Blut-Riegel), daher Riegel-Liste mit alt/neu wie in 5b.

## 0. Bau und Grundlage

* `bash re15_port/tools/local_build.sh configure` -> `=== LOCAL-BUILD-OK (configure)`; `... build` -> `ninja: no work to do.` /
  `=== LOCAL-BUILD-OK (build)`. `re15_pc.exe` 2026-10-12 15:22:05 juenger als `enemy_ai_re2_zombie.c` 15:11:22; Baum sauber.
* Nachbesserung 1 = `fe329139` + `d3f7417e`: nur `analysis/befunde_runde37/L_re2zombie.md` (+92/-3 seit `d90dba65`).
* Code-Diff `git diff e399ee3f -- re15_port/engine re15_port/include` (402 Zeilen) selbst gelesen: nur
  `enemy_ai_re2_zombie.c` (Produzenten (1)(2)(3) in EXEC[1], (b) DECISION[0] Block 3, (c) EXEC[0] P1-Wecker, DECISION[12],
  Sonden-Maske/Zaehler, Kommentar-Zensus) und NEU `include/re15_re2zombie_r37.h`.

## 1. Messungen an der exe (eigener Treiber `abn1_lauf.sh`, Messkopie `re15_pc_abn1L_<lauf>.exe`, eigener Auswerter `abn1_state.py`)

Treiber: Karte per `probe_r36_gegnerzahl_karte` (Montage: `1150 -17150 -11960 0 9:71 3:94 9:73-76 4:4-5 4:15 7:0xd3-0xd7
7:0xc6-0xca`), `RE15_CONTINUE_TEST=1 RE15_CARD_AUTO=1 RE15_CARD_SLOT=0 RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_WINDOW_SCALE=1`,
`RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=60`, `RE15_STATE_LOG=state.log`, `RE15_EXIT_AT`. Weg in den Gang =
ECHTE Tuer 1150 -> 1130 (`W1,A0.2,W2`; debug.log `CONTINUE: resumed in room 1150`, `Nachspawn 1130 (Programm 10)`).
Basis = exe des abgenommenen Spur-H-Baums `r37_zombieaktiv` (2026-10-12 14:31:49; `git diff e399ee3f d1b4b4f2 -- re15_port`
selbst gelesen: nur Kommentare/Makro-Umbruch in `irons_tod_1150.c` / `re15_zombieaktiv_r37.h`), Messkopie im exe-Ordner der
Spur L (gleiche Assets).

### 1.1 Nutzer-Beispiel ROOM1130 nach Irons' Tod, echte Tuer, Leon steht 1100 Bilder

| Lauf | Ergebnis |
|---|---|
| L1 (Spur L) | alle 5 ab F6 1/1 (Gang), Wege 15410..24481; Platz 5 (Typ 0x11) **F869 1/1 -> 1/2 bei +0x1F0 1991** (Arme hoch), **F888 1/2 -> 1/3 bei 1108** (Griff aus 0x201); Leon gr 0->1 F888; Leon-HP 100 -> 70 (F909) -> 40 (F947) |
| B1 (Basis) | alle 5 ab F6 1/1, Wege 14806..25775; KEIN 1/2; Platz 5 F896 1/1 -> 1/3 bei 1166 (Griff aus dem langsamen Gang); HP 100 -> 70 (F917) -> 40 (F955) |
| L2 (Spur L) `W1,A0.2,W2,R0.33,U25` (Leon geht den Gang hinunter) | Platz 5 F458 1/2 bei **1936** -> F474 Griff (1193); Platz 3 F707 1/2 bei **4692** (Produzent (1), < 0x1388); Platz 4 F772 1/2 bei **1978** -> F793 Griff (1180); Platz 2 F818 1/2 bei 4283 -> F819 zurueck 1/1 (Leon von Platz 4 gehalten, gr=1 ab F793 = Rueckfall wie im RE2-Original Takt 777); Leon-HP 100 -> 70 (F495) -> 40 -> 10 -> -20 (Tod F852) |
| L3 (Spur L) `...,R0.33,UX25` (Leon RENNT; erster 1130-Besuch, danach Tod-/Continue-Schleife bis Zeitgrenze) | Platz 5 F343 / Platz 4 F346 / Platz 1 F474 je 1/2 bei **2346** (< 0x9C4 bei rennendem Leon = Produzent (3)), Platz 3 F461 bei 2442; Platz 4 F352 Griff aus 0x201 (1180); Platz 3 F471 1/2 -> 1/12 (Ausfall), **F481 1/12 -> 1/3 bei 1280** (Griff aus dem Ausfall = DECISION[12], < 0x514); Leon-HP 100 -> 70 (F373) -> ... -> -20 (F610) |

Alle fuenf Montage-Zombies laufen in beiden Staenden ab Bild 6 im Gang auf Leon zu (das ist Spur H). Spur L fuegt den
RE2-Arme-hoch-Gang (1/2) mit Griff aus 0x201 und den Griff aus dem Ausfall hinzu; an der Basis-exe tritt 1/2 nie auf. Die Werte
sind bitgleich zu Abnahme 0 (I, a1, a2) — der Stand ist deterministisch und unveraendert.

### 1.2 "generell": Normalspiel ohne Montage, Leon UND Elza

| Lauf | Ergebnis |
|---|---|
| L4 Leon ROOM1180 (Karte `1180 -4729 -16018 0`, debug.log `PL00`), steht 900 Bilder | Platz 3 (Typ 0x10) **F283 1/2 bei 1995** -> F311 Griff (1193) -> F414 1/5 (Abwurf) -> F728 Gang -> **F802 1/2 bei 1991** -> F836 Griff (1180); Leon-HP 100 -> 80 -> 60 -> 40 -> 20 |
| B4 Basis, gleich | KEIN 1/2; Griffe aus 1/1 (F333 bei 1187, F693 bei 1173) |
| L5 **Elza** ROOM1181 (`probe_r36_elza_karte 1181 -4729 -16018 0`, debug.log `Spieler-Familie PL04 (character=4, Elza-Bit=1)`) | identisch zu L4 (gleiche Raumdaten, gleicher Strom): 1/2 bei 1995 / 1991, Griffe aus 0x201 bei 1193 / 1180; Elza-HP 100 -> 20 |
| L6 Leon ROOM1010 (Karte `1010 3650 6900 2048`, Normalspiel; erster Besuch, Leon stirbt F626, danach Continue-Schleife) | Platz 2 **F72 1/2 bei 1978**, Platz 1 **F83 1/2 bei 1995**; F93 Platz 2 Griff aus 0x201 (1193); F94 Platz 1 zurueck 1/1 (Leon gehalten); F286 Platz 1 1/2 bei 849 -> F287 Griff aus 0x201; F561 Platz 2 1/2 bei 1995; Leon-HP 100 -> 80 (F114) -> ... -> -20 (F626) |
| B6 Basis, gleich | KEIN 1/2; Griffe aus 1/1 (F101 bei 1173, F209 bei 849, F494 bei 1193); Leon-HP -> -20 (F553) |

Ergebnis "generell": in jedem gemessenen Raum (1130 Montage, 1180, 1181 Elza, 1010) und jeder Spieler-Haltung (steht, geht,
rennt) heben gehende Zombies vor Leon die Arme (0x201) und greifen aus diesem Gang; an der Basis-exe nie. Der Mechanismus sitzt
im gemeinsamen RE2-Gang-Executor EXEC[1] (`re2z_exec_walk`), den jeder RE2-Zombie durchlaeuft.

## 2. Messer an der exe — Nachpruefung der neuen Nutzer-Aussage ("je Anmarsch WENIGER Schlaege ... 6 statt vorher 10")

Aufbau (eigener, Paare Spur L / Basis mit identischer Eingabe): Montage-Karte, echte Tuer 1150 -> 1130, `RE15_EQUIP=1`
(debug.log `[equip] RE15_EQUIP -> item 1`, `W-bank -> W01`), Skript `W1,A0.2,W2,W<w>` + 50x `M0.4,MA0.1` (R1 halten, alle
0,5 s = 15 Bilder ein Schlag), `RE15_EXIT_AT=1400#1130`. Die Bildzaehlung beginnt je Raum neu, das Skript laeuft in 1130 neu an;
Schlaege also ab Bild 156 + 30*w. Gezaehlt (eigener Zaehler `abn1_messer.py`): HP-Abfaelle von Platz 5 (Typ 0x11, HP 250,
3 je Treffer) vor seinem 1. Griff (Anmarsch 1) und zwischen 1. und 2. Griff (Anmarsch 2). w = 20 + k/30 s, k = 0..14 deckt
alle 15 Schlagphasen ab; dazu w = 21.3 / 22.2 / 23.7 (Versaetze wie Abnahme 0, eigene Kadenz).

| w | Spur L Anm. 1 / Anm. 2 (Griffe) | Basis Anm. 1 / Anm. 2 (Griffe) |
|---|---|---|
| 20.000 | 0 / 2 (F888, F1236) | 1 / 3 (F917, F1265) |
| 20.033 | 0 / 1 (F888, F1213) | 1 / 2+ (F918, kein 2. Griff bis F1400) |
| 20.067 | 0 / 1 | 1 / 1 |
| 20.100 | 0 / 1 | 0 / 2 |
| 20.133 | 0 / 1 | 0 / 2 |
| 20.167 | 0 / 4+ (kein 2. Griff; Platz 4 greift F1393) | 0 / 2 |
| 20.200 | 0 / 3 | 0 / 2 |
| 20.233 | 0 / 3 | 0 / 2 |
| 20.267 | 0 / 3 | 0 / 2 |
| 20.300 | 1 / 2 | 0 / 2 |
| 20.333 | 1 / 1 | 0 / 2 |
| 20.367 | 1 / 1 | 0 / 2 |
| 20.400 | 1 / 1 | 0 / 2 |
| 20.433 | 1 / 2 | 0 / 2 |
| 20.467 | 1 / 2 | 0 / 2 |
| 21.3 | 1 / 2 | 0 / 2 |
| 22.2 | 0 / 3 | 0 / 2 |
| 23.7 | 0 / 3 | 1 / 3 |
| **Summe** | **Anm. 1: 7**; Anm. 2 (16 vollstaendige Paare): **31** | **Anm. 1: 4**; Anm. 2: **33** |

Alle Platz-5-Treffer bis Bild 1400: Spur L 43, Basis 41; Griffe von Platz 5: 35 / 35. Je Phase schwankt es in beide Richtungen
(L weniger z.B. bei 20.033-20.133 und 20.333-20.400, L mehr bei 20.200-20.267 und 22.2). Mechanismus in den Bildfolgen sichtbar
(z.B. `Ls_k6`: F1217 1/2 bei 1982, Treffer F1189 / F1219 / F1249, Erholung F1211 -> 1/1, F1217 -> 1/2, F1241 / F1271 -> 1/2, Griff F1272; `Bs_k6`: Treffer F1174 /
F1204, Erholung F1196 -> 1/2, F1226 -> 1/1, Griff F1227): der Arme-hoch-Gang schliesst schneller, aber die Trefferreaktion
haelt den Zombie je Schlag an — netto bleibt es im Mittel gleich. Die Aussage "je Anmarsch weniger Schlaege, 6 statt 10" gilt
fuer die vier Laeufe der Abnahme 0 (deren Schlagphase), aber NICHT allgemein -> **M2**. Fuer Punkt 4 ("der Nahkampf wird nicht
schlechter") ist das Ergebnis guenstig: an der exe ueber alle Schlagphasen gleich viele Messertreffer je Anmarsch.

## 3. RE-Gate — selbst disassembliert (`.claude/skills/re15-psx-disasm/scripts/re2_disasm.py`, RE2 `info/re2leon/COMMON/BIN/EMZ0.BIN`)

| Stelle (Diff bzw. Nachbesserung 1) | selbst gelesen | Urteil |
|---|---|---|
| (b) DECISION[0] Block 3 @0x801013c8 | `801013bc jal 0x80015fe8` / `801013c4 andi v0,v0,0x1` / `801013c8 beq v0,zero,0x801013d4` / `801013cc addiu v0,zero,3073` / `801013d0 sw v0,4(s0)`; Block 2 `8010136c bne v0,zero,0x80101380` | stimmt: Block 3 Commit bei (rand&1)==1, Block 2 bei ==0; Port Z.1416-1418 |
| (a) Produzent (2) @0x80101EC4-F10 | `lw v0,496(s1)` / `80101ecc sltiu v0,v0,0x7d0` / `beq -> 0x80101f14` / a1,a2 = 0x800cfc30/38 / `80101ee8 jal 0x80015614` (`addiu a3,zero,512`) / `bne` / `80101efc lbu 0x800cfdcb` / `andi 0x80` / `bne` / `80101f0c addiu v0,zero,513` / `80101f10 sw v0,4(s1)` | stimmt mit Port `enemy_ai_re2_zombie.c` Z.1708-1710 |
| (c) EXEC[0] P1 @0x80101530-5A0 | `80101534 andi 1` / `80101538 bne -> 0x80101544` (Delay `addiu v0,zero,2`) / `80101540 sb v0,6(s0)` faellt durch nach `80101544 lh v0,344(s0)` / `8010154c bne -> 0x80101590` / `8010155c sltiu 0x1d4c` / `80101570 andi 0x800` / `8010157c sw v0,4(s0)` (257) / `80101588 beq v0,zero,0x801016ac` / `8010159c j 0x801016ac` (Delay `sh` Dekrement) | stimmt: Wecker im Wurf-Takt, jeder Weg aus P1 -> Epilog; Port Z.1499-1508 (Maske C: Block nur innerhalb `if (sub_state_2 == 1)` erreichbar) |
| Nachbesserung 1: Griffschwelle 0x4B0 in beiden Gaengen | DECISION[1] `801018f4 sltiu v0,s2,0x4b0`; DECISION[2] (Tabelle 0x8010c88c [2] = 0x80101f7c) `80102114 sltiu v0,s2,0x4b0`; Commits `80102174 addiu v0,zero,769` / `80102178 sw`, `801021b8 addiu 769` / `801021bc sw` | stimmt |
| Nachbesserung 1: neuer OFFEN-Fund Riegel 0x800CFDCB im P3-Ausfall | `80105aa0 lui v0,0x800d` / `80105aa4 lbu v0,-565(v0)` (= 0x800cfdcb) / `80105aac andi v0,v0,0x80` / `80105ab0 bne v0,zero,0x80105ad8` / erst `80105ab8 jal 0x80015fe8`; Port `re2z_hit_main` case 3 Z.5603-5605 ohne diese Pruefung | Fund stimmt (Bytes `0d 80 02 3c cb fd 42 90 .. 80 00 42 30 09 00 40 14`) |
| **Nachbesserung 1: Erholungsleiter P3 @0x80105A50, "sonst 0x101 (@0x80105B38 / @0x80105B40)"** | `80105b14 jal rand` / `80105b1c andi 1` / `80105b20 beq v0,zero,0x80105b30` / `80105b24 addiu 513` / `80105b2c sw` — dann **`80105b30 lbu a0,536(s1)` / `80105b34 lbu v1,332(s1)` / `80105b38 addiu v0,zero,257` / `80105b3c bne a0,v1,0x80105ba0` / `80105b40 sw v0,4(s1)` (Delay)** / `80105b44 lui v0,0x1` / `80105b48 ori v0,v0,0x101` / `80105b4c jal rand` / `80105b50 sw v0,4(s1)` (= **0x10101**) / `80105b5c sb +0x16B = (r&0xf)<<1` / `80105b68 sh +0x158 = 30` / `80105b78 sb +0x16A = (r&0x1f)+30` / `80105b80 sh +0x15A = r+300` / `80105b98 jal 0x800152c8` (wenn +0x218 == +0x14C). Rohbytes `80105b30: 18 02 24 92 4c 01 23 92 01 01 02 24 18 00 83 14 04 00 22 ae 01 00 02 3c 01 01 42 34 fa 57 00 0c 04 00 22 ae` | **stimmt NICHT wie behauptet** -> Mangel M1 (Abschnitt 4) |

Guess-Tells im Diff (`deferred|tunable|interim|for now|faithful|plausib|TODO|vorerst|ungefaehr|geschaetzt|approx`): keine in
hinzugefuegten Zeilen. `getenv` in hinzugefuegten Engine-Zeilen: keins. Die Maske in `re15_re2zombie_r37.h` ist ein
Sonden-Schalter ohne Env-Weg (Standard 0x3F = alles an).

## 4. Die Erholungsleiter P3 — Port gegen Original (Grundlage fuer M1)

Original (Bytes oben): nach dem 0x201-Wurf schreibt P3 im Delay-Slot `sw 0x101` @0x80105B40, verzweigt aber nur dann ins Epilog,
wenn `+0x218 != (u8)+0x14C` (`bne a0,v1,0x80105ba0` @0x80105B3C). Ist der laufende Clip der Gang-Clip (`+0x218 == +0x14C`),
schreibt es **0x10101** (`lui 1 / ori 0x101 / sw` @0x80105B44-50) — Gang Phase 1 OHNE EXEC[1]-P0 — und setzt +0x16B, +0x158 = 30,
+0x16A, **+0x15A = rand + 300** (3 Wuerfe @0x80105B4C/64/74) und faehrt `jal 0x800152c8` @0x80105B98. Der Haupt-Handler zwingt
den Clip in P0 auf den Gang-Clip (`beq v1,v0,0x801054d8` @0x801054C8 / `sw v0,332(s1)` @0x801054D4; in P1/P2 schreibt nichts
+0x14C — selbst gegrept), also ist in P3 der Gleichheitsfall der Normalfall.

Port `re2z_hit_main` case 3 (`enemy_ai_re2_zombie.c` Z.5618-5625, vorbestehend seit d39b815b, von Spur L nicht beruehrt):
```c
re15_ai_set_state_word(e, 0x101);                          /* 257 @0x80105B38-40 */
if ((uint8_t)e->motion != e->re2z_walkclip) {              /* @0x80105B30-3C */
    re15_ai_set_state_word(e, 0x10101);                    /* 0x10101 @0x80105B44-50 */
```
Die Bedingung ist gegenueber `bne a0,v1` umgekehrt (derselbe Vergleich @0x80105B84-90 steht im Port richtig als `==`,
`re2z_hit_move` Z.4342; P0 @0x801054C8 `beq` richtig als `!=`, Z.5483).

An der exe gemessen (state.log, Platz 5 Typ 0x11, Messer-Laeufe aus Abschnitt 2):
* Spur L `L8_messer_W21.3`: F1228 `st=2 ss1=1 ss2=3 mo=2 af=21` (P3, Clip 2 = Gang-Clip, denn im Gang 1/1 laeuft mo=2) ->
  F1229 `st=1 ss1=1 ss2=0 mo=2 af=22` (Port schreibt 0x101) -> F1230 Griff aus 1/1.
* Basis `B7_messer_W20`: F1234 `st=2 ss1=1 ss2=3 mo=2 af=21` -> F1235 `st=1 ss1=1 ss2=0 af=22` (0x101) -> F1236 `ss2=1 af=18`
  (EXEC[1] P0: Zufalls-Startbild, Clip neu gesetzt). Das Original schriebe in beiden Faellen 0x10101 (ss2 = 1, Bild laeuft weiter).

Wirkung (vorbestehend, beide exe): nach jeder Treffer-Erholung, die nicht ins 0x201/0xB01/0xC01 faellt, faehrt der Port EXEC[1]
P0 (5 Wuerfe, Clip-Neustart auf Zufallsbild, **+0x15A = (rand>>3)+100 = 100..131**) statt der Original-Fortsetzung (3 Wuerfe,
Clip laeuft weiter, **+0x15A = rand+300 = 300..555**; RE2-rand @0x80015FE8 liefert `andi v0,v0,0xff` @0x80016014, also 0..255). Der Timer-Produzent (1) kann im Port nach einem Treffer also deutlich
frueher feuern als im RE2-Original; der Zufallsstrom verschiebt sich um 2 Wuerfe je Erholung. Produzent (2) laeuft in beiden
Wegen im ersten Gang-Takt (Original 0x10101 -> P1 direkt; Port 0x101 -> P0 faellt nach P1 @0x80101b20).

## 5. Vertrag (VERTRAG.md) und Tests

* Pfade `git diff e399ee3f --name-only`: `analysis/befunde_runde37/L_*`, `engine/src/enemy_ai_re2_zombie.c` (VERTRAG §1.4
  Ausnahme Spur L), NEU `include/re15_re2zombie_r37.h`, Tests (`tests/unit/test_r37_re2zombie.c`, `tests/unit/probes/
  r37_re2zombie.cmake`, `tests/integration/test_r37_re2zombie.cmake`; angepasst `test_r36_zombieki_eintritt.c`,
  `probe_r36_blut.c`, `probe_10d0_situp_re2.c`, `test_r34_granaten.cmake`, `test_r35_granate.cmake`, je mit @0x-Begruendung im
  Kommentar — selbst gelesen). Keine release/, platform/android/, shared_assets/PSX/, keine Test-CMakeLists, keine neue .c.
* Bank-9-Bits / Nachrichten-Ids / Ereignisse / Slots: keine (Engine-Diff schreibt keine Flags/Texte).
* Griff-/Biss-Code (Spur J) und `re2z_init` (Spur H): im Diff nur Kommentar-Zeilen in EXEC[3]/EXEC[5]/EXEC[11]-Koepfen;
  `re2z_hit_main` (Treffer-Handler) unberuehrt.
* Nachbesserung 1 aendert nur `L_re2zombie.md` (`git diff ab2bf2b9 -- re15_port` leer).

Testlaeufe (Baum r37_re2zombie, keine volle Suite):
1. `ctest -R "re2|zomb|zgirl|granat|lyer|messer|crawl|kriech|gegnerzahl|1140|1130|1180|enemy|biss|griff|grab|bump|kfjump|teardeath|blut|bandlock|rise|r37_|irons|1150|dog|crow|spider|affen|situp|10d0|hund|kraehe|gore|wecker|birkin|elza" -E "^integration_" -j 4 --timeout 600`
   -> **`100% tests passed, 0 tests failed out of 231`** (u.a. unit_re2_zombie_abc 6,0 s, unit_r37_re2zombie_nahkampf 6,6 s,
   unit_r36_zombieki_eintritt 19,6 s, blut_pin x9, probe_10d0_situp_handoff).
2. `ctest -R "^unit_r37_re2zombie_" -V` verbose gelesen: prod1 4/4, prod2 5/5, prod3 5/5, b 3/3, c 6/6, d12 6/6, nahkampf 2/2
   (`373 gegen 368 (>= 95 %)`), raum1130 4/4 (1 Wechsel bei 1991, Gegenprobe Maske 0: 0), raum1180 4/4 (2 Wechsel, Mittel 1993).
   Die Pins pruefen die Grenzwerte (0x1387/0x1388, 0x7CF/0x7D0, 0x9C3/0x9C4, 0x513/0x514, Bild 11) und Gegenproben mit Maske 0.
3. `ctest -R "^integration_(r37_re2zombie|r37_zombieaktiv|r34_granaten|r35_granate)$" --timeout 900` (seriell)
   -> **`100% tests passed, 0 tests failed out of 4`** (r34_granaten 225,8 s, r35_granate 160,7 s, r37_re2zombie 51,5 s,
   r37_zombieaktiv 25,1 s).

