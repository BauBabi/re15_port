# Runde 36 — Spur D "granate": UNABHAENGIGE ABNAHME 1

Stand: 2026-10-10 (Beginn 19:39), Baum `.claude/worktrees/r36_granate`, Zweig `r36/granate`, HEAD b632aa60
(Basis master 78a4a748). Abnehmer: unabhaengige Abnahme-Instanz (glaubt nur, was hier selbst gemessen ist).
Gegenstand: Nachbesserung 1 nach Abnahme 0 (`D_abnahme_0.md`: Punkt 1 erfuellt, Punkt 2 teilweise, M1/M2).
Bericht wird FORTLAUFEND geschrieben (Sitzungslimit-Schutz).

## 0. Grundlage
* Wortlaut (AUFTRAG.md Z. 25 / Z. 51):
  1. "Bei normalen Granaten brauche ich keinen Feuer Effekt bei den Zombies, sondern nur das Zerplatzen der
     Körperteile. [Feuer] würde ich nur bei den incendiary Granaten erwarten."
  2. "Manchmal selten löst die Granate bei der Verwendung nicht aus/wird nicht geworfen."
* Commits seit Abnahme 0 (95d15ec7): af295a4b 0bb12234 da446e88 0fbda7ab 7ae21507 b632aa60. Engine-Aenderung nur
  in 0bb12234 (game_step_common.c, granate_r36.c, re15_granate_r36.h); danach nur Tests/Dossier (`git show --stat`).
* Diff master..HEAD (`git diff master --stat`): 14 Dateien, +1938/-15. Engine: granate_r36.c (neu, 87),
  enemy_ai_re2_zombie.c (+5), game_step_common.c (+7/-1), player_common.c (+4), re15_esp.c (+11); include:
  re15_granate_r36.h (neu, 106), re15_esp.h (+2); Tests: probes/r36_granate.cmake, test_r36_granate.c,
  integration/test_r36_granate.cmake (neu), probe_r34_wurf.c, integration/test_r34_granaten.cmake (fremd, angepasst);
  Dossiers D_granate.md, D_abnahme_0.md. Arbeitsbaum sauber (`git status --short` leer).

## 1. Bau
* `bash re15_port/tools/local_build.sh configure` -> `LOCAL-BUILD-OK (configure)` (EXIT 0).
* `bash re15_port/tools/local_build.sh build` -> `ninja: no work to do.` / `LOCAL-BUILD-OK (build)` (EXIT 0).
  re15_pc.exe 19:21:17 (sha1 3f5d5a70) ist juenger als jede Engine-Quelle des Diffs (granate_r36.c 19:21:11,
  re15_granate_r36.h 19:13:08, game_step_common.c 19:13:38) -> exe = Engine-Stand HEAD.
* Messkopie `re15_port/build/platform/pc/re15_pc_abn1d.exe` (sha1 3f5d5a70). Vorher-Stand = `re15_pc_r36dn1_vor.exe`
  (sha1 e18c61ef = Abnahme-0-Stand 44a3eaf6, dort als re15_pc_abn0d.exe gleich gehasht).

## 2. Code-Diff gelesen (Nachbesserung 1)
* `game_step_common.c:1842-1846`: Entlade-Handler im Rueckstossbild 0 — `if (!re15_granate_r36_verbrauch_im_spawnbild(eq_item))`
  bewacht NUR die Munitions-Schleife (`for (az ...) re15_ammo_consume()`), Resolve/Muendungs-FX unberuehrt; fuer 9/10/11
  ist `ent->resolve = 0`, `n_fx = 0` (ENT[9..11] = `{1,0,1,0}`), also faellt dort nur der Abzug weg.
* `game_step_common.c:2025`: `if (gs) re15_granate_r36_geworfen();` hinter `re15_esp_granate_spawn` (Spawnbild 19/22/24,
  Spawn-Gate 9/10/11) — Verbrauch nur bei vergebenem Platz. `re15_granate_r36_geworfen()` = `re15_ammo_consume()`
  (inventory_common.c:177 = FUN_8004eae4: aufgeloester Ausruest-Platz, qty==0 -> 0, sonst qty--).
* Folgen selbst geprueft: Zielhoehe im Rueckstoss eingefroren (player_common.c:1155 Wechsel nur `READY && !s_aim_recoil`),
  also kann kein Hoehenwechsel das Spawnbild verfehlen; zweiter Abzug erst nach Clip-Ende (35/40 Bilder > 19/22/24),
  also kein zweiter Wurf aus derselben Granate; Abzug-Gate `re15_ammo_mag_nonzero` unveraendert (kein Wurf bei Menge 0).

## 3. RE-Gate — selbst disassembliert (richtige Binaerdatei, Hashes gleich Hauptbaum)
`info/Re1.5/PSX.EXE` sha1 2637b76d, `info/Re1.5/PSX/BIN/STAGE1.BIN` sha1 770e8e63, `info/re2leon/PSX.EXE` sha1 d57bd18e.
* **Verbrauch im Bild 0** (`re15_disasm.py dis 0x80033460 50`, `table 0x80074100 16`, `dis 0x80033b38 24`):
  `80033470 lbu v0,0(v1)` [0x800aca5b] / `80033478 bne v0,zero,0x80033508` / `80033480 sb v0(=1)` (nur erster Durchlauf),
  `800334c0 sll v1,v1,2` (Waffe*4) / `800334d0 addiu at,at,16640` (0x80074100) / `800334e0 jalr v0`; Tabelle
  [9] = 0x80033b38, [10] = 0x80033b58, [11] = 0x80033b78; je `addiu sp,-24 / sw ra / jal 0x8004eae4 / ... / jr ra`
  @0x80033b40/60/80. **Stimmt.** FUN_8004eae4 (`dis 0x8004eae4 44`): Platz `lbu 0x800b25c8`, Menge `lbu 0x800b10ad+4*i`,
  `bne v0,zero` / `addiu v0,v0,-1` @0x8004eb44-48, `sb v0` @0x8004eb60, Rueckgabe 1/0. **Stimmt** (= re15_ammo_consume).
* **Abgang 19/22/24** (`dis 0x80033680 76`): `80033688 ori v0,zero,0x9 / 8003368c bne v1,v0,0x800337ac`;
  `80033690 ori 0x13` + `800336b4 andi 0x8000` + `800336ec jal 0x80019700`; `800336a4 ori 0x16` + `80033714 andi 0x4000` +
  `80033748 jal 0x80019700`; `80033758 ori 0x18` + `80033770 andi 0x2000` + `800337a4 jal 0x80019700`; danach
  `800337ac lw ra` — Rueckgabe von 0x80019700 ungeprueft. **Stimmt.**
* **Griff** (`dis 0x80113ad0 28 --bin STAGE1.BIN`): `80113af8 ori v1,zero,0x2 / 80113b00 sb v1,-13736(at)` (0x800aca58 := 2),
  `80113b30 sh zero,-13734(at)` (0x800aca5a := 0). **Stimmt.**
* **Statusschirm-Ende** (`dis 0x8001cba8 16`): `8001cbb4 sb v0(=3),21337(at)` (0x800b5359), `8001cbdc sb zero,-13736(at)`
  (0x800aca58 := 0). **Stimmt.**
* **Rueckstoss-Abbruch** (`dis 0x800335f8 26`, `bytes 0x800740b4 16`): `80033600 andi v0,a0,0x100 / bne`, `8003362c addiu
  at,at,16530` (0x80074092), `8003363c sltu v0,v0,a0 / beq`, `8003364c sh v0(=3),0x800aca5a`, `80033650 j 0x800337ac`;
  Satz Waffe 9 @0x800740b8 = `18 30 0a 01 00` (Byte 2 = 10). **Stimmt.**
* **RE2-Retail-Vorbild** (`re2_disasm.py dis 0x80042ce0 30`, `table 0x800a6f90 18`, `dis 0x80043fcc 10`, `dis 0x80044b90 6`):
  `80042ce8 lbu v0,7(s1) / 80042cf0 bne` + `80042d00 sb v1(=1),7(s1)` (nur Eintritt), `80042d34 lw v0,28560(at)`
  (0x800a6f90+4*Id) / `80042d3c jalr v0`; Tabelle [9] = 0x80043fcc = `lhu a0,270(a0) / jal 0x8006a0cc`; Geschoss
  `80044b98 lbu v1,333(s0) / 80044b9c addiu v0,zero,1 / 80044ba0 bne` (Bild 1). **Stimmt.**
* Konstanten im Diff: Ids 9/10/11 (Tabelle @0x80074100 [9..11] + Spawn-Gate @0x8003368c/E1), 19/22/24 (@0x80033690/a4/58),
  Blut-Id 0 + kleinster Index (PORT-WAHL, gekennzeichnet), Verbrauchsstelle (ORCHESTRATOR-ENTSCHEIDUNG gemaess Nutzer-
  Wortlaut, gekennzeichnet in granate_r36.c/.h). Wortsuche in den hinzugefuegten Zeilen (`deferred|tunable|interim|for
  now|faithful|plausib|TODO|FIXME|approx|vermutlich|sieht richtig`): **kein Treffer**; `getenv` in hinzugefuegten
  Engine-Zeilen: **keiner** (kein Env-Schalter).
* Erklaert der Fix den Befund? Ja (selbst gemessen, Abschnitt 5): der Verlust entstand, weil der Abzug (Bild 0) und der
  Abgang (19/22/24) auseinanderliegen und jeder Abbruch dazwischen (Griff/Treffer/Statusschirm) den Abzug stehen liess;
  Verbrauch = Abgang schliesst genau diese Luecke.

## 4. Vertrag
* `git diff master --name-only`: keine `release/`, `platform/android/`, `shared_assets/`, keine CMakeLists (Tests ueber
  `tests/unit/probes/r36_granate.cmake`). Keine Bank-9-Bits, Nachrichten-IDs, Ereignisse (hinzugefuegte Engine-Zeilen ohne
  `game_flag_set|scd_event_fire|msg_install`) — Spur D hat laut VERTRAG §1.3 keine.
* Gemeinsame Dateien: game_step_common.c +1 include, 2 Code-Zeilen, 1 Kommentarzeile geaendert; enemy_ai_re2_zombie.c
  (Gore-Zweig der Explosion = D laut VERTRAG §1.4) +1 include, Gate um 2 Zeilen; player_common.c +4; re15_esp.c +1 include,
  Freigabe-Funktion (4 Zeilen) + 1 Aufrufzeile — klein, an benannten Stellen.
* Fremde Tests verhaltensbedingt angepasst: `probe_r34_wurf.c` PRUEF 92/98/104/109 (Menge nach Abzug 5, im Spawnbild 4),
  `test_r34_granaten.cmake` `r34_abzug` (Abzug = Beginn Rueckstoss-Clip statt Mengen-Stufe). Gelesen: beide pruefen
  jetzt die NEUE Regel (Menge im Spawnbild) und schwaechen die alte Pruefung nicht ab (Abzug weiter Bild 0, Spawn A+22/A+24).

## 5. Punkt 2 — "Manchmal selten loest die Granate bei der Verwendung nicht aus / wird nicht geworfen"
Treiber: exe im eigenen Arbeitsordner (`scratchpad/abn1/runs/<lauf>`), Umgebung wie der Haken (RE15_NO_INTRO=1
RE15_NOAUDIO=1 RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2 RE15_WINDOW_SCALE=1 RE15_STATE_LOG RE15_GRANATE_LOG
RE15_WAFFEN_LOG RE15_FX_LOG), Default-KI (RE2), RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=1. Auswertung
`abn1_eval.py`: Abzug = erstes Bild des Rueckstoss-Clips 7/9/11 (`ac=`), Verbrauch = Bild, in dem `mg=` faellt,
Wurf = `F=<n> SPAWN granate` (gr.log), Explosion = `EV resolver` (Tick -> Bild ueber die Platzzeilen), Griff = erstes `gr=1`.
"neu" = re15_pc_abn1d.exe (3f5d5a70, HEAD), "vor" = re15_pc_r36dn1_vor.exe (e18c61ef, Stand Abnahme 0).

### 5.1 Mangel M1 der Abnahme 0 — Griff waehrend der Wurfbewegung
| Lauf | Aufstellung (ROOM1140, RE15_GIVE=9:5) / Skript | exe | Abzuege | Griff | SPAWN | Verbrauch | Menge |
|---|---|---|---|---|---|---|---|
| c2 (Abnahme-0-Lauf) | (-1000,-17000,1024) `MD0.6,MDA0.2,MD2.667,MDA0.2,MD2.0,W3` | neu | 19, 105 | 113 | 43 | **43** | **5 -> 4** |
| c2 | dito | vor | 19, 105 | 113 | 43 | 19, 105 | 5 -> 3 |
| g1 (eigen) | (-1000,-17000,1024) `MD3.2,MDA0.2,MD2.0,W3` (ein TIEF-Abzug) | neu | 97 | 113 | — | — | **5 -> 5** |
| g1 | dito | vor | 97 | 113 | — | 97 | 5 -> 4 |
| g2 (eigen) | dito, MITTE `M3.2,MA0.2,M2.0,W3` | neu | 97 | 113 | — | — | **5 -> 5** |
| g2 | dito | vor | 97 | 113 | — | 97 | 5 -> 4 |
| g4 (eigen, Randfall) | dito, MITTE `M3.0,MA0.2,M2.0,W3` | neu | 91 | 113 | 113 (= 91+22) | 113 | 5 -> 4 |
| g3 (eigen) | (-1300,-17400,1024) `MD0.6,MDA0.2,MD3.5,MDA0.2,MD2.0,W3` | neu | 19 | 116 | 43 | 43 | 5 -> 4 |
* g4: Griff im Spawnbild selbst -> der Wurf geht noch ab (Explosion Bild 172, Eingriffe 4), verbraucht im Spawnbild —
  keine Luecke an der Grenze. g3: der zweite Abzug kam wegen des Griffs (116) nie zustande, Menge korrekt 5 -> 4.
* Ergebnis: an der HEAD-exe kostet ein vom Griff abgebrochener Wurf **keine** Granate mehr (3 Skripte: c2 der Abnahme 0 +
  g1/g2 eigene mit anderem Abzugsbild und anderer Zielhoehe; dazu Elza-c2 in 5.4; Gegenprobe "vor" je 1 Granate verloren).
  M1 behoben.

### 5.2 Statusschirm waehrend der Wurfbewegung (Bau-Agent: an der exe NICHT gefahren — hier gefahren)
ROOM1220 (-19533,-9700,2048), RE15_GIVE=9:5, Skript `MD0.6,MDA0.2,MDS0.2,MD0.2,W2.0,S0.2,W2.0,MD0.6,MDA0.2,MD2.0,W2`
(Abzug TIEF, 6 Bilder spaeter START = Statusschirm auf, ~2 s spaeter START = zu, danach ein normaler Wurf):
| exe | Abzuege | Statusschirm | SPAWN | Verbrauch | Menge |
|---|---|---|---|---|---|
| neu (`s1_neu`) | 19, 181 | auf F25 (pad 0848, Zustand eingefroren `mo=215 ac=11` bis F100; debug.log `[inv] Sicherung im Statusschirm`), zu F97, danach `mo=210` -> `mo=200` (Zielaktion beendet) | 205 (= 181+24) | **205** | **5 -> 4** |
| vor (`s1_vor`) | 19, 181 | dito | 205 | 19, 181 | 5 -> 3 |
-> Wurf 1 wird vom Statusschirm abgebrochen (kein SPAWN bei 43), die Granate bleibt (neu) bzw. ist weg (vor). Der Bau-Agent
hatte diesen Fall nur in der Sonde 405; an der exe bestaetigt.

### 5.3 R1 frueh losgelassen (der haeufige Aussetzer; Abnahme-0-Aufstellung ROOM1100) — Regression durch N1?
ROOM1100 (-20975,-25000,3917), RE15_GIVE=9:30, 9 Bloecke `<H>0.5,<H>A0.1,<H><h>,W1.8`, h = 0.0..0.8 s:
| Lauf (neu) | Abzuege | SPAWN | Verbrauch | Explosionen | Menge |
|---|---|---|---|---|---|
| MITTE `r1_mitte` | 16 88 163 241 322 406 493 583 676 | 38 110 185 263 344 428 515 605 698 (je +22) | = SPAWN | 9 | 30 -> 21 |
| HOCH `r1_hoch` | dito | 35 107 182 260 341 425 512 602 695 (je +19) | = SPAWN | 9 | 30 -> 21 |
| TIEF `r1_tief` | dito | 40 112 187 265 346 430 517 607 700 (je +24) | = SPAWN | 9 | 30 -> 21 |
-> 27/27 Abzuege = 27 Wuerfe = 27 Explosionen, jede Mengen-Stufe im Spawnbild; Abzugsbilder bitgleich zur Abnahme 0
(v0.8.23 dort: 6/27 Wuerfe). Kein Rueckschritt.

### 5.4 Elza (Bau-Agent: in der Nachbesserung NICHT gefahren — hier gefahren)
Start-Rezept test_r35_werfer_elza (RE15_TITLE_CONFIRM_MS=1500 RE15_PSELECT_AUTO=1 RE15_PSELECT_AUTO_SWITCH=1, ohne
TITLE_SHOT); debug.log je Lauf `[pl] Spieler-Familie PL04 (character=4, Elza-Bit=1)`, `[equip] W-bank -> W09 (Clips 14,
Rueckstoss-Clip7 fc=35)`.
| Lauf | Aufstellung / Skript | exe | Abzuege | Griff | SPAWN | Verbrauch | Menge |
|---|---|---|---|---|---|---|---|
| elza_r1 | ROOM1000 (21850,-13400,2048), 9:30, MITTE/HOCH/TIEF je R1 los nach 0.0/0.3/0.5 s | neu | 19, 100, 190 | — | 41 (+22), 119 (+19), 214 (+24) | = SPAWN | 30 -> 27 |
| elza_c2 | c2-Aufstellung ROOM1140 | neu | 19, 105 | 113 | 43 | **43** | **5 -> 4** |
| elza_c2_vor | dito | vor | 19, 105 | 113 | 43 | 19, 105 | 5 -> 3 |
-> Elza verhaelt sich wie Leon (3/3 Wuerfe, Griff kostet keine Granate mehr).

### 5.5 Gehaltenes Quadrat (Dauer-Wurf) und letzte Granate (ROOM1220)
* `held` (9:5, `MD0.2,MDA4.0,MD0.5,W3`): neu 3 Wuerfe (SPAWN 39, 78, 117), Verbrauch 39/78/117, Menge 5 -> 2;
  vor: gleiche Wuerfe, Verbrauch 15/54/93 (Abzugsbilder). Abstand 39 = Clip 11 (40 Bilder) — kein Doppel-/Fehlwurf.
* `letzte` (9:1, `M0.6,MA0.2,M1.5,MA0.2,M1.5,W3`): Abzug 19 -> SPAWN 41, Menge 1 -> 0 im Spawnbild; zweiter Quadrat-Druck
  F70 ohne Rueckstoss-Clip, kein SPAWN, Menge bleibt 0 — kein "Gratis-Wurf" durch den spaeten Verbrauch.

### 5.6 Wurf im Gore-Fenster nach einer HE-Explosion (Pool-Last, Teil 2b)
ROOM1140 (-2400,-17200,900), Abzug 1 F19 (Explosion F119, 3 Eingriffe), Abzug 2 F100/F103/F106 (`pool_a/b/c`):
SPAWN 124/127/130, Verbrauch = SPAWN, Menge 5 -> 3, Explosion 2 je 76 Bilder spaeter. `fx=` in state.log steigt nach
der Explosion auf hoechstens **93** (F130) — der volle Pool (96/96) ist an der exe auch hier nicht erreicht; Teil 2b
(Blut-Platz raeumen) bleibt wie in Abnahme 0 nur durch Sonde 300-304 + Code-Lesung belegt.

### 5.6b Saeure/Brand (Ids 10/11) und Quadrat-Tipp waehrend des Hebens (ROOM1220, neu)
* `saeure_r1` (10:5, MITTE R1 sofort los / HOCH 0,2 s): Abzuege 19, 100 -> SPAWN 41, 119 (`art=3`), Verbrauch 41/119,
  Menge 5 -> 3, 2 Explosionen + 2 Aufschlag-Zeilen. `brand_r1` (11:5, MITTE / TIEF): SPAWN 41, 124 (`art=4`),
  Verbrauch 41/124, 5 -> 3, 2 Explosionen.
* `tipp_heben` (9:5, `M0.1,MA0.1,M1.5,W2` = Quadrat in Bild 4-6, vor READY): kein Rueckstoss-Clip, kein SPAWN, Menge 5 -> 5;
  Gegenprobe Pistole (`tipp_heben_hg`, 3:15): ebenfalls kein Schuss, 15 -> 15 = Original-Verhalten aller Waffen (Held-Bit
  @0x80033300-0c), kein Granaten-Thema, kein Verlust.

### 5.7 Summe Punkt 2 (alle Laeufe dieser Abnahme, neu)
Ueber 23 neu-Laeufe (inkl. Punkt-1-Laeufe, Abschnitt 6): 52 SPAWN = 52 Explosionen (kein Blindgaenger), jede Mengen-Stufe faellt in
einem SPAWN-Bild (`Verbrauch == Spawn: True` in allen neu-Laeufen), jeder Abzug ohne SPAWN hat eine gemessene Ursache
(Griff g1/g2/c2, Statusschirm s1) und kostet keine Granate. Gegenprobe "vor": in denselben Faellen je 1 Granate verloren.

## 6. Punkt 1 — "keinen Feuer-Effekt bei den Zombies, nur das Zerplatzen; Feuer nur bei incendiary" (am HEAD-Stand neu gemessen)
Eigene Aufstellung (weder die des Bau-Agenten (-1676,-18070) noch die der Abnahme 0 P1/P2): ROOM1140, Leon
(-1300,-17400) Blick 1024, RE15_GIVE=<id>:5, Skript `MD0.6,MDA0.2,MD2.5,W5` (TIEF-Wurf), RE15_EXIT_AT=330#1140,
RE15_FRAMEDUMP=113-209/12, exe neu.
| Lauf | Granate / KI | Explosion (gr.log) | Zustand Bild 119/120 | Flammen `id=8` nach X (fx.log) | Blut 0/0 | Brocken 7/2 |
|---|---|---|---|---|---|---|
| p1_he | HE 9 / RE2 | F119 `art=2 P=(-2324,-480,-20234) eingriffe=3` | Zombies 2/3 (0x10) `st=3 ss1=9 mo=2` hp -150/-120, in 10 Bildern ~1800 weggeschleudert ((-1321,-19525) -> (-1804,-21256)); 0x11 `st=2 ss1=9` | **0** | 913 | 156 |
| p1_brand | Brand 11 / RE2 | F119 `art=4 ... eingriffe=3`, Aufschlag-Zeile 1 | dito | **1899** | 451 | — |
| p1_he_re15 | HE 9 / RE15_AI_FLAVOR=re15 | F119 `art=2 ... eingriffe=2` | Zombie 3 `st=3 ss1=9` hp -135, 0x11 `st=2` | **0** | 616 | — |
* Bild `scratchpad/abn1/bilder/p1_vergleich.png` (Zeilen: p1_he / p1_brand / p1_he_re15; Spalten Bild 113/125/137/161/197):
  HE — Explosion + Blutschwall (125), danach liegen die zerrissenen Leichen dunkel (russ-getoent) ohne jede Flamme (161/197);
  Brand — ab 137 Flammen, 161/197 grosse Flammen auf den Leichen; RE1.5-KI — Explosion, kein Feuer.
* Der Detektor misst Feuer (Brand 1899 Zeilen), HE bleibt bei 0 — in beiden KI-Varianten. Zerplatzen unveraendert vorhanden
  (Zerreiss-Tod Zeile 9, Brocken 7/2 = 156 Zeilen, Blut 913).
* **Urteil Punkt 1: ERFUELLT** (wie Abnahme 0; der Code von Punkt 1 ist seit 44a3eaf6 unveraendert, die Nachbesserung
  beruehrt ihn nicht). Hinweise ohne Mangel: Russ-Tinten bleiben (OFFEN 2, kein Feuer); das Gate gilt fuer alle Aufrufer
  von FUN_8010640C (auch GL-Explosivrunde/Rakete = "nicht incendiary", im Dossier "Fuer den Nutzer" genannt).

## 7. Tests (selbst gefahren, KEINE volle Suite)
* Gezielt: alle Tests der Spur + der beruehrten Module (Granate/Wurf/Werfer/Munition/Ziel-FSM aus game_step_common.c +
  player_common.c; Gore/Zerreissen/ESP aus enemy_ai_re2_zombie.c + re15_esp.c; Gegnerreaktion/Schaden der Granate) —
  10 mehr als die 26 des Bau-Agenten:
  `ctest --test-dir re15_port/build -R "^(unit_r36_granate|integration_r36_granate|unit_r34_wurf|integration_r34_granaten|
  unit_r35_granate|integration_r35_granate|unit_r30_granate|integration_r30_granate_laden|unit_r17_munition_pin|
  unit_r17_waffen_loop_pin|m93r_nachladen|unit_r35_werfer|unit_r35_werfer_kombi|integration_r35_werfer|
  integration_r35_werfer_ton|integration_r35_werfer_elza|integration_r35_werfer_form|unit_esp_parse|unit_re2_gore|
  unit_re2_gore_render|unit_re2_gore_fly|unit_re2_gore_decoder|unit_re2_zombie_teardeath|unit_r19_zerreiss_10e0c7|
  unit_aim_lower_exit|unit_aim_all_around|unit_re15_re2z_gore_se|unit_r34_reaktion|unit_r34_schaden|unit_r34_plattform|
  unit_weapon_select|unit_autofire_pin|unit_re2_weapon_rows|unit_item_use|unit_schrot_trennung|unit_r35_affen_schrot)$"
  --timeout 900 --output-on-failure -V` -> **100% tests passed, 0 tests failed out of 36**, `Total Test time (real) =
  778.25 sec` (Log `scratchpad/abn1_ctest1.log`; integration_r34_granaten 209.38 s, integration_r35_granate 160.01 s,
  integration_r36_granate 93.35 s). Kein Fenster-Haken rot, kein Nachfahren noetig.
* Inhalt (aus -V): test_r36_granate `ALLE PRUEFUNGEN GRUEN` mit 103-108, 201-209 (je 32 Abzuege HOCH/MITTE/TIEF = 32 Wuerfe,
  `Menge ok 32`), 300-304, 400-405; integration_r36_granate `[he]` Flammen 0, `[brand]` 1899, `[serie]` 8 = 8 = 8,
  Verbrauch 38;104;173;245;320;398;479;563, `[griff]` Abzuege 19;105, Griff 113, Wurf 43, 5 -> 4, Verbrauch 43;
  probe_r34_wurf `ALLE PRUEFUNGEN GRUEN` (Waffe 9/10/11: Menge nach Abzug 5, Spawn A+22).
* **Zaehne des Pins selbst geprueft**: `cmake -DRE15_PC_EXE=.../re15_pc_r36dn1_vor.exe -DR36_NUR=griff -DWORKDIR=... -P
  tests/integration/test_r36_granate.cmake` -> exit 1, `r36_granate [griff]: Granaten 5 -> 3, soll 5 -> 4 (vorher 5 -> 3:
  verbraucht, aber nicht geworfen)`; dieselbe Probe mit re15_pc_abn1d.exe (`R36_NUR=griff;serie`) -> ok. Die Unit-
  Mutationsprobe des Bau-Agenten (alte Verbrauchsstelle -> 208/209/400/401/403/404 rot) ist NICHT nachgebaut (Abnahme
  aendert keinen Code); die Sonden fahren den echten re15_game_step (test_r36_granate.c:94/256, Treffer ueber den echten
  HP-Drop-Detektor, Statusschirm-Ende ueber re15_player_cmd_zero).

## 8. Urteile
| Punkt (Wortlaut) | Urteil | Beleg |
|---|---|---|
| 1. HE-Granate: kein Feuer an Zombies, nur Zerplatzen; Feuer nur bei incendiary | **erfuellt** | Abschnitt 6: p1_he 0 Flammen-Zeilen (RE2-KI) / p1_he_re15 0 (RE1.5-KI), Zerreiss-Tod Zeile 9 + 156 Brocken + 913 Blut; p1_brand 1899 Flammen; Bild p1_vergleich.png; Haken [he]/[brand] + Sonden 103-108 gruen |
| 2. Granate loest manchmal nicht aus / wird nicht geworfen | **erfuellt** | Abschnitt 5: R1-frueh 27/27 (+ Elza 3/3, Saeure/Brand 4/4, Dauer-Wurf 3/3) Wuerfe; Griff (c2, g1, g2, Elza-c2) und Statusschirm (s1) brechen den Wurf ab, die Granate bleibt (vor: je 1 verloren); 52 SPAWN = 52 Explosionen, Verbrauch == Spawn in 23/23 Laeufen; M1/M2 der Abnahme 0 behoben (Wahlfrage gestrichen, Dossier "Entscheidungsfragen: keine") |

Gates: Bau OK; RE-Gate OK (6 Stellen selbst disassembliert, alle wie zitiert; keine Rate-Woerter, kein Env-Schalter);
Vertrag OK (Pfade, keine Bits/IDs/Ereignisse, Haken klein); Tests 36/36, Pin mit Zaehnen.

## 9. Maengel
Keine.

### Hinweise (kein Mangel, fuer den Orchestrator/Nutzer)
1. Voller Effektpool (Teil 2b, PORT-WAHL Blut-Platz raeumen) ist an der exe weiterhin nicht nachstellbar (hier max. 93/96
   im Gore-Fenster, Abschnitt 5.6) — nur Sonde 300-304 + Code-Lesung, wie in Abnahme 0.
2. Treffer OHNE Griff waehrend der Wurfbewegung nur in der Sonde 400 (echter HP-Drop-Detektor) belegt, an der exe nur
   Griff und Statusschirm gefahren; der Verbrauch haengt allein am Spawn, nicht am Abbruchgrund.
3. Quadrat-Tipp waehrend des Hebens (vor READY) loest — wie bei allen Waffen im Original — nichts aus (5.6b); kein Verlust.
4. Griff im Spawnbild selbst (g4, Bild 113): der Wurf geht noch ab — Reihenfolge Spieler-/Gegner-Tick ist vor Runde 36
   so (`g4_vor`: ebenfalls SPAWN 113, Explosion 172, nur Verbrauch schon in 91), keine Luecke beim Verbrauch.
5. re15_esp.c-Haken (Freigabe-Funktion + Aufruf, Teil 2b, unveraendert seit Abnahme 0) liegt mit 6 Code-Zeilen knapp
   ueber "1-5 Zeilen" des Vertrags, an klar benannter Stelle (Zugriff auf das statische s_esp_fx noetig).
6. Zusammenfuehren mit Spur I ("Handgranaten bei 0 aus dem Inventar entfernen"): die Menge erreicht 0 jetzt erst im
   Spawnbild (5.5 `letzte`: 1 -> 0 in F41 = SPAWN) — Spur I darf die Waffe dort entfernen, der Wurf ist dann schon ab.

**Ergebnis: BESTANDEN.**
