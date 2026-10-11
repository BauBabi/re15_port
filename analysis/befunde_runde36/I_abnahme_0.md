# Runde 36 — Spur I "inventar": UNABHAENGIGE ABNAHME 0

Pruefer: Abnahme-Agent 0 (2026-10-11). Baum `.claude/worktrees/r36_inventar`, Zweig `r36/inventar`,
HEAD ca823ef4, Basis (merge-base) 78a4a748. Massstab: AUFTRAG.md (Wortlaut), VERTRAG.md, CLAUDE.md RE-Gate.

Hinweis zum Diff: `git diff master` zeigt zusaetzlich 66 geloeschte `synchro/STAGE1/room10x1..1211/*.wav`
und -15 Zeilen AUFTRAG.md. Das ist KEINE Aenderung der Spur: master ist seit dem Abzweig auf 565da62b
weitergelaufen (836023f5 Nachtrag J, 565da62b Elza-Sprachdateien). Der echte Spur-Diff
`git diff 78a4a748` beruehrt 20 Dateien (siehe Gates).

## Stand: FERTIG — BESTANDEN (alle 4 Teilpunkte erfuellt, alle Gates halten; 3 nicht blockierende Hinweise, Abschnitt 5)

## 0. Bau

`bash re15_port/tools/local_build.sh configure` -> `=== LOCAL-BUILD-OK (configure)`;
`bash re15_port/tools/local_build.sh build` -> `ninja: no work to do.` / `=== LOCAL-BUILD-OK (build)`.
Die exe (`re15_port/build/platform/pc/re15_pc.exe`, 02:42) ist juenger als alle geaenderten Quellen
(inventar_r36.c 02:39, main.c 02:33, debug_menu_common.c 02:33, game_step_common.c 02:33), der Baum ist
sauber (`git status --short` leer), die exe traegt die neuen Protokoll-Texte (`grep -c inventar-r36` = 3,
`card=%s (slot %d qty %d)`). Gemessen wird also der Stand ca823ef4.

## 1. RE-Gate — selbst disassembliert (nicht aus dem Dossier uebernommen)

Werkzeug: `.claude/skills/re15-psx-disasm/scripts/re15_disasm.py` (RE1.5 PSX.EXE); RE2 MEM_CARD.BIN mit
`re2_disasm.dis_one` auf Ladeadresse 0x801C0000 (Skript im Scratchpad, roh ohne Kopf, 23932 Bytes).

| Behauptung (Code/Dossier) | selbst gelesen | Urteil |
|---|---|---|
| I4 Raumlader waehlt Datei = Basis + Elza-Bit `srl a0,a0,31` @0x800397e4, `addu a0,a0,v0` @0x800397ec | `800397b8 lw a0,-13764(a0)` (0x800aca3c), `800397e0 lhu v0,0(v1)`, `800397e4 srl a0,a0,31` (Bytes `c2 27 04 00`), `800397e8 jal 0x80013b60`, `800397ec addu a0,a0,v0` (Bytes `21 20 82 00`, Delay-Slot); Funktion beginnt @0x800396fc (`addiu sp,sp,-48`), liest @0x800397a8/b0 Stage 0x800b0fe0 / Raum 0x800b0fe2, kein `jr ra` dazwischen | bestaetigt |
| I4 Debug-JUMP schreibt nur Stage/Raum und springt in denselben Lader `j 0x8001d988` @0x8001d824, `jal 0x800396fc` @0x8001d988 | `8001d618 bne v0,zero,0x8001d82c` (0x800ac9a8), `8001d654 lbu v0,0(at)` (0x800bbe5f+Stage), `8001d660 sh v0,4066(at)` -> 0x800b0fe2, `8001d808 sh v0,4064(at)` -> 0x800b0fe0, `8001d818 sb zero` 0x800afbb5, `8001d820 sh zero` 0x800b0fe4, `8001d824 j 0x8001d988`, `8001d988 jal 0x800396fc` | bestaetigt |
| I1/I2 RE2 MEM_CARD.BIN Farbband-Abzug @0x801c118c-0x801c11e0 | `801c1180 jal 0x80076a00` (a0 = sp+496, a1 = 0x800D4A3C, a2 = 44 = Inventar sichern), `801c118c addiu a0,zero,30` (0x1E), `801c1190 jal 0x800696cc`, `801c11a4 lbu v0,19005(at)`, `801c11ac addiu v0,v0,-1` (`ff ff 42 24`), `801c11b8 sb v0,19005(at)`, `801c11c0 bne v0,zero,0x801c11e8`, `801c11d0 sb zero,19004(at)`, `801c11dc sb zero,19006(at)`, `801c11e0 jal 0x80069714`, `801c11e8..1200` 0x800D46C4 += 1 | bestaetigt |
| I1 Fehlerzweig nimmt zurueck @0x801c1290-12bc | `801c128c beq v0,zero,0x801c12c8`; sonst `801c12a8 jal 0x80076a00` (a0 = 0x800D4A3C, a1 = Sicherung, 44) + `801c12a4 addiu v0,v0,-1` / `801c12ac sh` Zaehler, `801c12bc sb t1,18109(at)` (46BD zurueck) | bestaetigt |
| I3 RE1.5 entfernt die leere Granate nicht: Entlade-Handler 0x80033B38/58/78 = nur `jal 0x8004eae4`; FUN_8004eae4 @0x8004eb44-60 | Entlade-Tabelle @0x80074100: [9] @0x80074124 -> 0x80033b38, [10] -> 0x80033b58, [11] -> 0x80033b78; jeder Handler `addiu sp / sw ra / jal 0x8004eae4 / lw ra / jr ra`. FUN_8004eae4: `8004eb3c lbu v0,0(at)` (0x800b10ad + Platz*4), `8004eb44 bne v0,zero` / `8004eb48 addiu v0,v0,-1`, bei 0 `8004eb4c j 0x8004eb68` mit `v0 = 0`, sonst `8004eb60 sb v0,0(at)` — KEIN Leeren des Platzes | bestaetigt |
| Ablegen UNEQUIP @0x8004ad44-4c | `8004ad44 ori v0,zero,0x80`, `8004ad4c sb v0,9672(at)` (0x800b25c8) | bestaetigt |
| Commit-Regel 0x80 -> Messer @0x80046654-88, Bank @0x800466c4 | `800465f8 lbu 25c8` / `80046600 lbu 25ce` / `8004661c beq` (nur bei Aenderung); `80046658 lbu v1,9672(v1)`, `8004665c ori v0,zero,0x80`, `80046660 bne`, `8004666c ori v0,zero,0x1`, `80046688 sb v0,-13731(at)` (0x800aca5d), `80046698..a0` player &= ~0x4000, `800466b0 jal 0x80036b68`, `800466c4 jal 0x80043d8c` (a1 = 0x80198000) | bestaetigt |
| Verbrauch/Verdichten @0x8004aee4-af2c | `8004aef0/af0c/af28 sb zero` -> 0x800b10ac/ad/ae [Platz], `8004af2c jal 0x8004dadc` | bestaetigt |
| Item-Ids 0x21 = Memory Card, 0x09/0x0A/0x0B = Handgranaten | DEBUG.BIN Namens-Offsets @0x495C (u16, Basis Blob @0x4A28): [0x09] 0x5b -> "Hand Grenade", [0x0A] 0x68 -> "Acid Grenade", [0x0B] 0x75 -> "Incendiary Grenade", [0x21] 0x1c2 -> @0x4BEA `29 41 49 4b 4e 55 00 1f 3d 4e 40` = "Memory Card" | bestaetigt |
| Waffen-Parametersatz @0x800740b8/bd/c2 je `18 30 0a 01 00` | `800740b8: 18 30 0a 01 00 18 30 0a 01 00 18 30 0a 01 00` | bestaetigt |

Konstanten im Code-Diff: 0x21 (@0x800C4BEA), 0x09..0x0B (Handler 0x80033B38/58/78), 0x80 = RE15_MESSER_NICHTS
(bestehend, @0x8004ad44/@0x8004665c), 1 = RE15_MESSER_ID (bestehend, @0x8004666c), `-1` (@0x801c11ac). Jede
traegt ihre Adresse. Der Zeitpunkt des Waffenwechsels nach dem Wurf ist als PORT-WAHL gekennzeichnet
(inventar_r36.c Kommentar (b), Dossier I3 "WANN"); die Entfernung der Granaten als NUTZER-VORGABE.
Wort-Suche im `+`-Diff von re15_port nach deferred/tunable/interim/for now/faithful/plausib/TODO/FIXME/
vorlaeufig/approx/geschaetzt: 0 Treffer. Neue `getenv` im Code-Diff: 0.

## 2. Messungen an der exe (Messkopie `re15_pc_abn0i.exe`, Arbeitsordner im Scratchpad)

Messskript: `lauf.sh <name> <timeout> KEY=WERT...` (Kopie der exe unter eigenem Namen neben die exe,
Arbeitsordner `scratchpad/abn_i/runs/<name>/`, debug.log dort). Bilder = RE15_FRAMEDUMP mit
RE15_WINDOW_SCALE=1 (320x240), per ffmpeg nach PNG; sie liegen im Scratchpad (nicht committet, der
Auftrag erlaubt nur diesen Bericht) — massgeblich sind die zitierten Protokollzeilen.

### Punkt 2 (I4) — Debug-Menue mit Elza in die xxx1-Raeume

Wortlaut: "Bei den Debug Menu, bei den man zwischen die Räume springt, schickst du, wenn man mit Elza
spielt Elza in Leons Räume. Wenn man mit Elza spielt, sind die Räume mit "1" hinten korrekt, zum
Beispiel ROOM 1071."

**M-I4a — ECHTER Tastenweg des Nutzers** (SELECT oeffnet, RUNTER auf die JUMP-Zeile, 3x LINKS, VIERECK):
```
gemeinsam: RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_WINDOW_SCALE=1 RE15_TITLE_CONFIRM_MS=1500 RE15_PSELECT_AUTO=1
           "RE15_PRESS=select@300,down@310,left@320,left@330,left@340,square@360" RE15_EXIT_AT=450
Elza (+RE15_PSELECT_AUTO_SWITCH=1), Lauf m_i4_elza_rechts:
  [pl] Spieler-Familie PL04 (character=4, Elza-Bit=1)
  [debug-menu] JUMP -> 121 DETENTI. AREA  (ROOM1211) spawn=(-26420,0,-2400) cut=0
  [room] PC loaded room1211.rdt (254616 bytes)
Leon, Lauf m_i4_leon_rechts:
  [pl] Spieler-Familie PL00 (character=0, Elza-Bit=0)
  [debug-menu] JUMP -> 121 DETENTI. AREA  (ROOM1210) spawn=(-26420,0,-2400) cut=0
  [room] PC loaded room1210.rdt (140596 bytes)
```
Dieselbe Menue-Eingabe laedt bei Elza die ANDERE Datei (254616 statt 140596 Bytes). Bild F440 zeigt Elza
bzw. Leon im Zellentrakt (`belege/i4_elza_menue_room1211_F440.png`, `i4_leon_menue_room1210_F440.png`).
Gegenprobe ohne Bewegen des Cursors (`select@300,down@310,square@330`, Lauf m_i4_elza_menue): Elza im
Startraum -> `JUMP -> 124 OPENING (ROOM1241)`, `loaded room1241.rdt` (ihr eigener Startraum, nicht 1240).

**M-I4b — Beispiel des Nutzers ROOM1071** (`RE15_DEBUG_JUMP=1070@250` = Cursor setzen + dieselbe
Lade-Flanke @0x80014A38 wie VIERECK, main.c:7203-7245):
```
Elza: [debug-menu] JUMP -> 107 LOBBY OFFICE   (ROOM1071) spawn=(850,0,3600) cut=0 / [room] PC loaded room1071.rdt
Leon: [debug-menu] JUMP -> 107 LOBBY OFFICE   (ROOM1070) spawn=(850,0,3600) cut=0 / [room] PC loaded room1070.rdt
```
Vorher-Stand (Dossier i4_vorher, 78a4a748): Elza -> `(ROOM1070)`, `loaded room1070.rdt`. Ursache und Fix
passen zusammen: main.c:7395 alt `droom = load_room << 4` (Variante immer 0), neu
`re15_debug_menu_ziel_id()` = `re15_room_for_char(load_room << 4)` = Basis | (character & 4 ? 1 : 0) —
genau die Rechnung des Raumladers (Basis + Elza-Bit @0x800397e4/ec, selbst disassembliert, Abschnitt 1).
Der Fix erklaert den Befund vollstaendig. Spawn bleibt der Tabellensatz (gleich fuer beide, wie im Original:
der JUMP-Executor schreibt nur Stage/Raum, die Datei waehlt der Lader). Code-Pruefung: einziger Sprung-
Codepfad (`grep "load_room << 4"` -> nur noch debug_menu_common.c:146); CONTINUE stellt
`g_gameflow.character` aus dem Spielstand her (main.c:3644, re15_savedata.c:266), der Sprung gilt also
auch nach dem Laden eines Elza-Spielstands (nicht eigens gemessen).
Riegel: unit_r36_inventar (H) prueft alle 101 belegten Sprungsaetze x 2 Charaktere ueber
re15_debug_menu_tick (202 Saetze, Datei vorhanden) — selbst gefahren, `=== GRUEN ===`.

**Urteil Punkt 2: ERFUELLT.**

### Punkt 1a/1b (I1/I2) — Memory Card beim Speichern -1, bei 0 weg

Wortlaut: "Mit Nutzung von Memory Cards beim Speichern soll sich die Anzahl reduzieren. Sind Memory Cards
... 0 sollen sie aus dem Player inventory verschwinden"

**M-I1 — zwei Speichervorgaenge HINTEREINANDER in einer Sitzung, echter Telefon-Weg** (Lauf m_i1_folge).
Spielstand: `probe_r36_inventar_karte re15_card.mcr 2` -> `Inventar 21:2 03:15 15:50, Ausruest-Platz 1,
Waffe 3` (ROOM1150 vor dem Telefon). Danach:
```
RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_WINDOW_SCALE=1 RE15_CONTINUE_TEST=1 RE15_CARD_AUTO=1 RE15_CARD_SLOT=0,1,2
RE15_MSG_LOG=1 "RE15_PAD_AT=150:A,200:A,290:A,370:A,430:S,520:S,600:A,650:A,740:A,820:A,880:S,980:S,1060:A,1110:A,1160:A"
"RE15_FRAMEDUMP=380-1200/20:f" "RE15_EXIT_AT=1250#1150"
debug.log:
[save] CONTINUE: resumed in room 1150 (hp=100)
[save] room=1150 msg=1 card_slot=0 -> FRAGE (Memory Card benutzen?)
[save] Antwort: JA -> Speicherbildschirm
[inventar-r36] Memory Card verbraucht: Platz 0 Rest 1 | Inventar 21:1 03:15 15:50 00:0 ... | Ausruest-Platz 0x01
[save] saved (room 1150) slot n=2 -> next=3; card=used (slot 0 qty 1)
[save] room=1150 msg=1 card_slot=0 -> FRAGE (Memory Card benutzen?)
[save] Antwort: JA -> Speicherbildschirm
[inventar-r36] Memory Card verbraucht: Platz 0 Rest 0 -> Platz entfernt | Inventar 03:15 15:50 00:0 ... | Ausruest-Platz 0x00
[save] saved (room 1150) slot n=3 -> next=4; card=used-last, slot removed (slot 0 qty 0)
[save] room=1150 msg=1 card_slot=-1 -> HINWEIS (keine Memory Card)
[flow] EXIT_AT: Bild 1250 in Raum 1150 erreicht -> exit
```
Geschriebene Bloecke derselben Karte (xxd, Ausruest-Platz/Waffe @+0x23, Inventar @+0x28):
```
Platz 0 (Ausgang)        @0x2100: eq/waffe=0103  inv=21020000 030f0000 15320000   (Karte x2)
Platz 1 (1. Speichern)   @0x4100: eq/waffe=0103  inv=21010000 030f0000 15320000   (Karte x1)
Platz 2 (2. Speichern)   @0x6100: eq/waffe=0003  inv=030f0000 15320000 00000000   (Karte weg, Browning rueckt
                                                                                  nach Platz 0, Ausruest-Platz zieht mit)
```
Statusschirm nach dem 1. Speichern (F480): Memory Card mit "1" (`belege/i1_nach_speichern1_karte_x1_zoom.png`);
nach dem 2. Speichern (F960): KEINE Memory Card, Browning 15 + Patronen 50, Equip Arms = Browning
(`belege/i2_nach_speichern2_statusschirm_F960_karte_weg.png`). Drittes Untersuchen des Telefons: nur noch
der Hinweis, kein Speichern (Folge der leeren Karte, wie RE2 ohne Farbband).

**M-I1-Gegenprobe NEIN** (Lauf m_i1_nein, Karte x2, `150:A,200:A,290:A,360:R,380:A,450:S`):
`[save] Antwort: NEIN -> zurueck`, kein `[save] saved`, kein `[inventar-r36]`; Statusschirm F500: Memory Card
"2" (`belege/i1_nein_karte_x2_zoom.png`). Abbruch im Speicherschirm ("Do not save") ist mit den
Messhaken nicht fahrbar (RE15_CARD_SLOT nimmt nur 0..4); per Code: `sd` ist lokal, der Live-Abzug steht
nur im Erfolgszweig `if (pc_run_memcard_screen(1, &sd, 0) >= 0)` (main.c:5063-5066), der Schirm liefert
nur nach `pc_do_save` Erfolg `result = cursor` (main.c:1867/1885), sonst -1 — konsistent mit RE2
(@0x801c128c-12bc, Abschnitt 1).

Vorher-Stand (Dossier i1_vorher_karte, 78a4a748): `card=kept (slot 0 qty 1)`, Block `21 01` — die Karte
wurde nie verbraucht (Runde-33-PORT-WAHL). Der Fix (Abzug im Block vor dem Schreiben, live nach Erfolg)
erklaert den Befund und erzeugt genau die RE2-Endzustaende.

**Urteil Punkt 1a (Anzahl sinkt beim Speichern): ERFUELLT. Punkt 1b (Memory Card bei 0 weg): ERFUELLT.**

### Punkt 1c (I3) — Handgranaten bei 0 weg

Wortlaut: "Sind Memory Cards oder Handgranaten 0 sollen sie aus dem Player inventory verschwinden"

Gemeinsames Rezept (Wurf-Rezept integration_r35_granate/r36_inventar, aber im GEGNERFREIEN ROOM1210, weil
in ROOM1140 die Zombies Leon im Zwei-Wurf-Lauf toeteten — Lauf m_i3_zwei: hp 100 -> -30 bei F710, der
2. Statusschirm ging nicht mehr auf; das ist KEIN Befund der Spur, der Platz fiel dort ebenso bei F455):
```
RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2 RE15_WINDOW_SCALE=1
RE15_GRANATE_LOG=gr.log RE15_STATE_LOG=state.log RE15_DEBUG_JUMP=1210@250 RE15_INPUT_SCRIPT_BASIS=spiel
RE15_INPUT_SCRIPT_START=1
```
**M-I3a — zwei Handgranaten, zwei Wuerfe** (Lauf m_i3_zwei_1210, `RE15_GIVE=9:2 RE15_EQUIP=9`,
`RE15_INPUT_SCRIPT=M0.6,MA0.2,M2.5,W6,S0.1,W3,S0.1,W2,M0.6,MA0.2,M2.5,W6,S0.1,W3`, Framedump 300-800/20):
```
[give] item 9 x2 -> slot 2
gr.log: F=41 SPAWN granate art=2 ...  /  F=476 SPAWN granate art=2 ...     (beide Wuerfe fliegen)
state.log F340 mg=1                                                       (nach Wurf 1: Platz bleibt, Menge 1)
[inventar-r36] F455 Platz 2 Id 09 Menge 0 -> entfernt (Ausruest-Platz 0x80, Waffe 9)   (Abzug Wurf 2 bei F454)
[inventar-r36] F545 Waffe 9 (leer, abgelegt) -> 1 (Messer-Rueckfall 0x80)
[equip] W-bank -> W01 (Clips 14, Rueckstoss-Clip7 fc=25)
state.log F780 mg=-1
```
Statusschirm F340 (nach Wurf 1): Equip Arms + Item list = Hand Grenade "1"
(`belege/i3_nach_wurf1_von2_statusschirm_F340_granate_x1.png`). Statusschirm F780 (nach Wurf 2): KEINE
Granate in der Item list (Browning 15 + Patronen), Equip Arms = Messer
(`belege/i3_nach_wurf2_von2_statusschirm_F780_granate_weg.png`). Spielbild F600: Messer in Leons Hand
(`belege/i3_F600_leon_messer_zoom.png`).

**M-I3b — Saeure-/Brandgranate, je eine** (Laeufe m_i3_10_1210 / m_i3_11_1210, `RE15_GIVE=10:1`/`11:1`,
`RE15_INPUT_SCRIPT=M0.6,MA0.2,M2.5,W6,S0.1,W3`, Framedump 340):
```
Saeure: F=41 SPAWN granate art=3 / [inventar-r36] F20 Platz 2 Id 0a Menge 0 -> entfernt / F110 Waffe 10 -> 1
Brand : F=41 SPAWN granate art=4 / [inventar-r36] F20 Platz 2 Id 0b Menge 0 -> entfernt / F110 Waffe 11 -> 1
```
Statusschirm F340 jeweils ohne Granate, Equip Arms = Messer (`belege/i3_saeure_statusschirm_F340_weg.png`,
`belege/i3_brand_statusschirm_F340_weg.png`).

**M-I3c — Nebenwirkungen des Uebergangs (Platz weg, Waffen-Id noch 9 bis Wurfende):**
* Nachdruecken mit gehaltenem R1 nach der letzten Granate (Lauf m_i3_nachdruck,
  `M0.6,MA0.2,M1.5,MA0.2,M1.0,MA0.2,M1.0,W4`, RE15_WAFFEN_LOG): VIERECK bei F70 und F106 ->
  `w=9 clip=8 rec=0 mag=0` (kein Rueckstoss), gr.log genau EIN `SPAWN` (F41) — keine Gratis-Granaten
  (Feuer-Tor `eq_item >= 3 && !re15_ammo_mag_nonzero()`, game_step_common.c:1716, Platz 0x80 -> mag 0).
  Messer erst bei F152 (R1 los bei F142).
* START waehrend die Granate fliegt (Lauf m_i3_start_im_flug, `M0.6,MA0.2,M1.0,W0.2,S0.1,W3,S0.1,W5`):
  Statusschirm F110 ohne Granate, Equip Arms = Messer (`belege/i3_start_im_flug_statusschirm_F110.png`);
  nach dem Schliessen fliegt die Granate weiter (wf.log F170/F180 fx=1, F200 fx=4) und die Waffe wechselt
  erst danach (`[inventar-r36] F202 Waffe 9 (leer, abgelegt) -> 1`). Kein Leerplatz sichtbar.
* Zwischen Platz-weg und Waffenwechsel haelt Leon die "Granaten-Hand" ohne Granate (Zoom F190 leere Hand,
  F250 Messer). Das ist die als PORT-WAHL gekennzeichnete Zeitwahl (Original wechselt die Waffe nur im
  Statusschirm-Commit @0x800465f4-0x800466c8, selbst gelesen) — der Wortlaut des Nutzers betrifft nur das
  Inventar und ist erfuellt.

Vorher-Stand (Dossier i3_vorher, 78a4a748): Granate "0" in Equip Arms und Item list. RE1.5 entfernt sie
nicht (FUN_8004eae4, selbst disassembliert) — die Entfernung ist korrekt als NUTZER-VORGABE gefuehrt, die
Form (Leeren + Verdichten = @0x8004aee4-af2c/FUN_8004dadc, Ablegen 0x80 = @0x8004ad44-4c, Commit-Regel
0x80 -> Messer = @0x80046654-88) ist belegt.

**Urteil Punkt 1c (Handgranaten bei 0 weg): ERFUELLT** (Hand-, Saeure-, Brandgranate).

## 3. Tests (selbst gefahren, KEINE volle Suite)

Unit (`ctest -R "^(unit_r36_inventar|unit_debug_menu|unit_savedata|unit_savepoint|unit_inv_fsm|unit_inv_grant|
unit_room1150_itembox|unit_itembox_transfer|unit_itembox_screen|unit_load_after_death|unit_r21_discard_wegwerfen|
unit_r26_inventar|unit_r30_elza_selbsttuer|unit_r30_granate|unit_map_speichern_laden|unit_r33_karte_speicher|
unit_r33_speichern|unit_r34_wurf|unit_r34n_d_adaruf_speicher|unit_r34n_d_adaruf_elza|unit_elza_zweig|
unit_r35_granate|unit_r35_inventar1050_.*|unit_r35_werfer|unit_r35_werfer_kombi|unit_r35_zgirl_messer|
unit_r30_hund_tod_elza|unit_r33_tuer1120_elza|unit_pri_inventar_riegel|r28_messer_dauerschlag)$" --timeout 600 -j4`):
**`100% tests passed, 0 tests failed out of 37`**. unit_r36_inventar einzeln: (A)-(C) Karte Block == live,
x3 -> x2, ohne Karte nichts; (D)-(G) leere Granaten/Karten weg, Messer erst nach dem Effekt; (H) 202 Saetze
Leon xxx0 / Elza xxx1, Datei vorhanden, `Elza: Menue 107 -> ROOM1071`, `=== GRUEN ===`.

Integration, nacheinander (`ctest -R "^(integration_r36_inventar|integration_r33_speichern|
integration_r35_werfer_elza|integration_save_counter_pin|integration_r35_granate|integration_r34_granaten|
integration_r30_granate_laden|integration_elza_vollstart|integration_r35_inventar1050|integration_r35_werfer|
integration_r35_werfer_ton|integration_r35_werfer_form|integration_weste_load_pin)$" --timeout 900 -j1`):
**`100% tests passed, 0 tests failed out of 13`** (1058 s). integration_r36_inventar meldet:
```
-- r36_inventar s2: Memory Card x2 -> x1 (Log + Kartenblock 21:1 03:15 15:50)
-- r36_inventar s1: letzte Memory Card -> Platz weg, Browning Platz 0 ausgeruestet (Block 03:15 15:50 leer, Platz 0 / Waffe 3)
-- r36_inventar g: letzte Granate geworfen, Platz weg F20, Messer ab F111
-- r36_inventar e1: Debug-Sprung 115 -> ROOM1151
-- r36_inventar e0: Debug-Sprung 115 -> ROOM1150
-- r33_speichern B: mit Karte + JA gespeichert, letzte Karte verbraucht (Log + Kartenblock 00 00)
-- r35_werfer_elza: OK — Elza PL04, W0F 14 Clips, 10 Runden mit ofs=(120,1200,0), 10 Explosionen
```
Kein Fenster-Haken rot, kein Nachfahren noetig.

Riegel je Nutzer-Punkt vorhanden und diskriminierend (Vorher-Stand 78a4a748 selbst gegengelesen:
main.c:7395 `droom = (unsigned)dm->load_room << 4`, main.c:5066 Log `"kept"`; die Vorher-Protokolle der
Spur `build/r36i_mess/i4_vorher` (`(ROOM1070)`), `i1_vorher_karte` (`card=kept (slot 0 qty 1)`) liegen vor):
1a s2 + unit (B); 1b s1 + unit (A) + r33 B; 1c g + unit (D)-(G); 2 e1/e0 + unit (H).
Abdeckungsluecke (nicht blockierend): die Entfernung einer LEEREN Brandgranate 0x0B ist nur ueber den
Bereich 0x09..0x0B im Code abgedeckt (unit (E) prueft 0x0A leer, 0x0B nur voll); an der exe selbst
gemessen (M-I3b).

## 4. Gates

* **RE-Gate:** erfuellt — 10 zitierte Stellen selbst disassembliert bzw. gelesen (Abschnitt 1), dazu
  @0x80033684-8c (`lbu 0x800aca5d` / `ori v0,zero,0x9` / `bne` = Projektil nur bei Waffe 9), @0x80032e60
  (`lbu 0x800aca5d`, Waffe je Bild gelesen) und @0x80018410-28 (`lui v1,0x10a` / `ori v1,v1,0x1` /
  `jal 0x80045024` = Abprall-Ton ARMS-Satz 0x0A) — alle enthalten das Behauptete, richtige Binaerdatei
  (RE1.5 PSX.EXE bzw. RE2 MEM_CARD.BIN, DEBUG.BIN fuer die Namen). NUTZER-VORGABE (Granaten weg) und
  PORT-WAHL (Zeitpunkt Messer) sind gekennzeichnet; keine Rate-Woerter, keine neuen Env-Schalter.
  Commit-Messages (ca823ef4, ed9976ec, 94a4275f) tragen die Adressen.
* **Fix erklaert Befund:** I4 alt `load_room << 4` (Variante 0) -> neu Basis | Elza-Bit = Rechnung des
  Raumladers; I1/I2 alt kein Abzug (Runde-33-PORT-WAHL) -> neu RE2-Abzug; I3 alt kein Entfernen
  (FUN_8004eae4) -> neu Tick. Jeweils am Vorher-Code/-Protokoll belegt.
* **Vertrag:** erfuellt — echter Spur-Diff (`git diff 78a4a748 --name-only`, 20 Dateien) ohne release/,
  platform/android/, shared_assets/PSX/, tests/unit/CMakeLists.txt, tests/integration/CMakeLists.txt; keine
  Bank-9-Bits, keine Nachrichten-IDs, keine Ereignis-Nummern (Spur I hat keine; `+`-Diff ohne
  flag_set/msg_install_text/scd_event_fire). Gemeinsame Dateien nur Haken: game_step_common.c 2 Zeilen vor
  `re15_menu_start_poll`; main.c 1 Include + 4 Codezeilen im Speicherfluss + 1 Zeile Sprungziel; Debug-Menue
  (debug_menu_common.c, re15_debug_menu.h) gehoert Spur I (VERTRAG §1.4). Neue Logik in
  inventar_r36.c/re15_inventar_r36.h; Tests registriert aus `tests/unit/probes/r36_inventar.cmake`;
  Arbeitsordner `r36_inventar_wd` != Targetname; Messkopie `re15_pc_r36i_haken_<id>.exe` wird geloescht.
  Angepasste fremde Riegel (r33_speichern B, r35_werfer_elza EXIT_AT#1001) folgen dem bestellten neuen
  Verhalten und sind begruendet.
* **Nebenwirkungen:** Leon-Sprung unveraendert xxx0 (gemessen 1070/1210/1150); Speicher-NEIN verbraucht
  nichts; Nachdruecken nach der letzten Granate wirft nichts; START waehrend des Flugs zeigt keinen
  Leerplatz; Inventar-Verdichten/Ausruest-Platz korrekt (Block @0x6100 eq 00 / Waffe 03, Statusschirm).

## 5. Maengel

Keine blockierenden Maengel.

Hinweise (nicht blockierend, beim Zusammenfuehren mitnehmen):
1. **Veraltete Kommentare widersprechen dem neuen Verhalten:** `re15_port/platform/pc/main.c:5011-5013`
   ("Die Karte wird NICHT verbraucht (PORT-WAHL, Nutzerwunsch; RE2 verbraucht das Farbband ...)") und
   `re15_port/include/re15_savepoint.h:76-78` ("Die Karte wird NICHT verbraucht — PORT-WAHL auf
   Nutzerwunsch"). Der Code verbraucht seit ca823ef4; ein spaeterer Agent koennte den Kommentar fuer die
   Vorgabe halten. Nachpruefbar: `grep -n "NICHT verbraucht" re15_port/platform/pc/main.c
   re15_port/include/re15_savepoint.h`. Zwei Kommentarzeilen anpassen (Runde 36 Spur I, Verbrauch wie RE2
   @0x801c118c-0x801c11e0).
2. **Zusammenfuehren:** der Zweig steht auf 78a4a748; master ist auf 565da62b weiter (836023f5 Nachtrag
   AUFTRAG.md, 565da62b 66 Elza-wav des Nutzers). `git diff master` zeigt diese deshalb als "geloescht" —
   per Merge zusammenfuehren, nicht den Spurstand ueber master legen. Tests anderer Spuren, die mit
   `RE15_PSELECT_AUTO_SWITCH=1` + `RE15_DEBUG_JUMP=xxx0` und `RE15_EXIT_AT=...#xxx0` arbeiten, landen
   jetzt in xxx1 (in diesem Baum nur r35_werfer_elza, angepasst) — in der Integrations-Suite darauf achten.
   Neue Quelldatei `engine/src/inventar_r36.c`: der Android-Bau cacht die GLOB-Liste (app/.cxx) — beim
   Paketbau neu konfigurieren, sonst fehlt der Tick dort (Linkfehler oder alter Stand).
   Robustheit integration_r36_inventar Lauf g: er wirft in ROOM1140 mit RE2-Zombies; im langen Zwei-Wurf-
   Lauf dort (m_i3_zwei) war Leon schon vor F340 verletzt (Statusschirm "Caution", state.log F450 hp=60)
   und bei F710 tot (hp=-30). Die Assertions des Tests
   liegen vor F400 und haengen nicht an HP (zweimal gruen); bei Flattern zuerst hieran denken.
3. **Item-Box** wird nicht geraeumt (Auftrag nennt das Spieler-Inventar; eine 0er-Granate/-Karte kann dort
   nicht mehr entstehen) — korrekt als OFFEN-Hinweis im Dossier gefuehrt.

## 6. Gesamturteil

| Punkt (Wortlaut AUFTRAG.md Z.28/29) | Urteil |
|---|---|
| 1a Memory Card: Anzahl sinkt beim Speichern | erfuellt |
| 1b Memory Card bei 0 aus dem Inventar | erfuellt |
| 1c Handgranaten bei 0 aus dem Inventar | erfuellt |
| 2 Debug-Menue mit Elza in die xxx1-Raeume (z.B. ROOM1071) | erfuellt |

Alle Gates halten. **BESTANDEN** (mit den nicht blockierenden Hinweisen 1-3).

