# Runde 38 — Spur C "werkstatt11b0": Abnahme 0 (unabhaengig, fortlaufend)

Baum: `.claude/worktrees/r38_werkstatt11b0` (Zweig `r38/werkstatt11b0`, Basis 1bcbd0ff, HEAD 467cb958).
Gepruefte Behauptung: Bau-Agent "Status: fertig", Commits 6cf17bcd..467cb958 (10 Commits).
Massstab: AUFTRAG.md Z.16/Z.34/Z.35 (Wortlaut), VERTRAG.md §1/§2, CLAUDE.md STOP-GATE.
Messwerkzeug der Abnahme: eigene exe-Kopie `re15_port/build/platform/pc/re15_pc_abn0c.exe` (Kopie von re15_pc.exe
17:14:48, juenger als jede geaenderte Quelldatei), Laeufe mit Arbeitsordner im Scratchpad, RE15_WINDOW_SCALE=1,
SDL_RENDER_DRIVER=direct3d11, SDL_AUDIODRIVER=dummy, RE15_NO_INTRO=1.

## 0. Stand der Abnahme
- [x] Diff gelesen (Code, Tests, Werkzeuge) — §5
- [x] Bau (local_build.sh configure + build): `=== LOCAL-BUILD-OK (configure)`, `ninja: no work to do.`,
      `=== LOCAL-BUILD-OK (build)` (exe 17:14:48 juenger als alle geaenderten Quellen; Arbeitsbaum = HEAD, `git status` sauber)
- [x] Riegel der Spur + beruehrte Module (ctest -R, 63 Tests) — §4
- [x] Punkt 1 gemessen (Echtweg Tuer 11C0 -> 11B0)
- [x] Punkt 2 gemessen (Echtweg, Mitschnitt A/B)
- [x] Punkt 3 gemessen (Echtweg, Gegenprobe am gebauten v0.8.25-Code)
- [x] RE-Gate (13 Stellen selbst disassembliert/gelesen) — §6
- [x] Vertrag / Pfade / Nebenwirkungen (ROOM1171 Leon-Modell + Elza, Ton) — §7/§8

## 1. Punkt 1 — "Ändere in ROOM 11B0 in der neuen Cutscene von Marvin seinen Text von "Hold on..." zu "But, hold on...""

Messlauf r1_tuer (ECHTER Weg: Raum 11C0 nach Bossen, Tuer-AOT 0 -> ROOM11B0, kein Sprung in die Szene):
`RE15_DEBUG_JUMP=11c0@250 RE15_SET_FLAG=3:0x43,3:0x82,4:0xf3,4:0x40 RE15_FIRE_AOT=0@60#11c0 RE15_MSG_LOG=1
RE15_SE_DEBUG=1 RE15_ANIM_TRACE=.. RE15_STATE_LOG=.. RE15_AUDIO_CAP_SYNC=.. RE15_FRAMEDUMP=1560-2400/1:.. RE15_EXIT_AT=2400#11b0`
-> exit 0, 841 Bilder.

- debug.log: `[fire-aot] slot=0 at F60 (Raum 11C0)` -> `[room] PC loaded room11b0.rdt` -> ... -> Z.1100/1101
  `[msg] room=11b0 id=20 savepoint=0` / `[msgtext] room=11b0 id=20 "Marvin: But, hold on..."` (zwischen Framedump F2123
  und F2124), danach `[scd] Plc_motion(entity=0, motion=15 ...)` (Geste) wie im Programm +0x000C.
- Bild: F2124..F2215 zeigen die Textbox "Marvin: But, hold on..." (Zoom F2160: Komma und drei Auslassungspunkte
  gerendert, Sprecherkopf blau wie die Original-Zeilen). Abnahme-Bilder: Scratchpad `abn/p1_msg20.png`, `abn/p1_msg20_zoom.png`.
- Bytes: Original ROOM11B0 (info/Re1.5/PSX/STAGE1) selbst gelesen: msg 12 @0x1BD0 `04 00 05 07 29 3d 4e 52 45 4a 16 05
  00 00 2b 47 3d 55 1a ...` ("Marvin: Okay. Be careful down there.", Ende @0x1BFA `04 01 01 5f`); msg 4 @0x19CD
  `... 2b 44 18 00 25 00 4f 41 41 57 57 57 00 20 3d 49 4a 00 45 50 1a` ("Oh, I see... Damn it.") -> Komma = `18`,
  Auslassung = `57 57 57` (Satzpunkt waere `1a`). Neue msg 20 = Kopf + `1e 51 50 18 00 44 4b 48 40 00 4b 4a 57 57 57` +
  `04 01 01 5f` — dieselbe Form.
- Generator: `tools/r36_j/szene11b0_bauen.py` mit umgelenktem OUT (Kopie im Scratchpad) neu laufen lassen ->
  Ausgabe **byte-identisch** zu `engine/src/gen/szl11b0_szene.inc` (cmp ohne Abweichung), Programm 294 B unveraendert.
- Restvorkommen "Hold on": nur eine Testbeschreibung in `tests/unit/test_r36_szenenleon.c:476` (Ausgabetext, kein
  Pruefwert) — kosmetisch.

**Urteil Punkt 1: erfuellt** (Wortlaut wortgleich, im Spiel ueber den echten Weg angezeigt, Form belegt).

## 2. Punkt 2 — "In ROOM 11B0 macht das Verschieben des COntainers mit Marvin zusammen in der Cutscene noch keinen Verschiebe Sound"

Messlauf r1_tuer (wie §1, ECHTER Weg) und Gegenlauf r1b_stumm (identische Schalter + `RE15_W11B0_SCHUB_STUMM=1` =
Ergaenzung aus = Stand vor Runde 38, ohne Framedump), je `RE15_AUDIO_CAP_SYNC`, Ende Bild 2400 in 11B0.

- debug.log r1_tuer: `[szl-r36] ROOM11B0 Spieler: Plc_dest Modus 0x13` + `[scd] Plc_dest(slot=1 mode=0x13 ...)` (Marvin),
  `[schub-r38] Ergaenzungsbank ROOM1171 snd0 geladen: 6 VAGs, Satz 7 @0x6EF8 00 00 45 10 -> Ton 4 -> VAG 4 (6688 B)`,
  dann ACHT `[schub-r38] F<n> ROOM11B0 snd0 Satz 7 leer ... -> ROOM1171 snd0 Satz 7` bei **F1646 x2, F1671 x2, F1696 x2,
  F1729, F1730**, JEDE direkt gefolgt von `[se] Stimme: se=7 layer=0 vag=3 note=63 fine=0 center=87 vol=110 pitch=0x400
  (11025 Hz) -> SE-Stimme 0 (SPU 16)` (8 von 8 gekeyt). Abstand 25 = Clip 0x12 (25 Bilder); anim.txt Marvin F1645 Clip 18
  Bild 0 -> F1646 = Bild 1 (Original-Takt +0x95 == 1 @0x80052ac0).
- Ton-Mitschnitt A (r1_tuer) gegen B (r1b_stumm), Werkzeug Scratchpad `tools/capdiff.py` (5880-B-Bloecke):
  ```
  Bloecke A 2881 B 2881 / Versatz (Bloecke - Endbild) 481 / B: 481
  erster verschiedener Block 2128 = Spielbild 1647
    verschieden: Bild 1647..1728  Summe|A-B| 279395136
    verschieden: Bild 1730..1762  Summe|A-B| 109329132
  Bild 1646: Energie A 859884  B 859884  |A-B| 0
  Bild 1649: Energie A 4212602 B 938264  |A-B| 4045776
  Bild 1733: Energie A 4423349 B 2049391 |A-B| 4045776
  ```
  Bis F1646 bitgleich, ab dem ersten Pumpen nach dem Anschlag verschieden; der Ton ist der lauteste Anteil der Mischung
  (F1649: 4.2 Mio gegen 0.94 Mio). Dieselben Summen wie im Sprung-Lauf des Bau-Agenten (279 395 136 / 109 329 132):
  bildgenau reproduzierbar, auch ueber den echten Weg. F1729 (Leon Phase 7) wird in F1730 von Marvins Phase-7-Anschlag
  auf derselben Stimme 0 neu angeschlagen (Diff F1730 == Diff F1647) — wie FUN_80045024 es im Original auch taete:
  Satz-Byte 3 0x10 -> Stimme 0 (@0x8004523c `addiu s0,s0,-16`), Prioritaet 5 aus Byte 2 0x45; FUN_80045a18 sperrt nur
  bei kleinerer Prioritaet oder bei gleicher mit Bit 3 (@0x80045a34 `sltu`, @0x80045a48..58) — hier nicht gesperrt.
- Ton = Leons Kisten-Ton: Original-Bytes selbst gelesen (info/Re1.5/PSX/STAGE1): ROOM1171 Kopf +0x08/+0x0C/+0x10 =
  0x6EDC/0x6F5C/0x87FC, Satz 7 @0x6EF8 `00 00 45 10` -> Prog 0 Ton 4 @0x77FC `00 00 6e 40 57 00 3f 3f ...` (vol 110, pan
  64, center 87, VAG 4) -> VAG-Groessen `48,4496,3840,6688,13920,7872`, VAG 4 @0xA8BC 6688 B sha1 f343933e; ROOM1090
  Satz 7 @0x118EC `00 00 45 10`, Ton 4 @0x121F0 bytegleich, VAG 4 @0x152B0 6688 B sha1 f343933e. ROOM11B0: Satz 7
  @0xE4A0 `00 00 00 00`, VH ps=1 ts=4 vs=3, Ton 0 @0xED24 `00 00 00 40 52 43 00 7f` (vol 0) -> Original stumm.
  Mischer-Probe unit_r38_werkstatt11b0_ton (echtes audio_pc.c) gruen; Code gelesen: vergleicht das 11B0-Fenster mit
  dem ROOM1090-Fenster Sample fuer Sample (kein Selbstvergleich).
- ROOM11B1 (Elza) Satz 7 @0x135F4 ebenfalls `00 00 00 00`, dort gibt es keine Schub-Szene; die Ergaenzung ist auf
  0x11B0/Satz 7/leeren Satz begrenzt (`re15_w11b0_ergaenzung_noetig`), Probe Fall 5 gruen.
- Einordnung Beta -> Retail (geprueft, kein Mangel): RE1.5 hat das Schiebe-System vollstaendig (Kiste @0x80035a18,
  Schub-Sub @0x80052acc/@0x80052bc8, EIN bytegleicher Ton in 1090/1171), unfertig ist nur die Raumbank 11B0. RE2-Gegenprobe
  selbst disassembliert (info/re2leon/PSX.EXE): @0x8003f2f4 `lbu v1,333(s0)` / @0x8003f304 `lui a0,0x204` / @0x8003f308
  `ori a0,a0,0x1` / @0x8003f30c `jal 0x8005ba28` = RE2-Spielerschub spielt Raumbank-Satz 4 (je Raum ein anderes Sample),
  keine Container-/Gemeinsam-Schub-Szene. Die Wahl des RE1.5-Tons (= Leons Kisten-Ton im Port) ist begruendet und
  im Header re15_werkstatt11b0_r38.h mit Adressen/Offsets hergeleitet.

**Urteil Punkt 2: erfuellt** (Schiebe-Ton klingt beim gemeinsamen Schieben, 8 Anschlaege im Original-Takt,
im Mitschnitt nachgewiesen; vorher stumm).

## 3. Punkt 3 — "In ROOM 11B0 macht Marvin während/nach dem verschieben des Containers so eine komische Fall Animation, die er im original nicht macht."

**Gegenprobe gebaut:** Basis 1bcbd0ff (Code == v0.8.25 2077bb8e: `git diff --stat 2077bb8e 1bcbd0ff -- re15_port` leer)
per `git archive` (ohne shared_assets) in den Scratchpad, PowerShell-Bau `cmake -S .. -B .. -G Ninja -DRE15_BUILD_PC=ON
-DRE15_BUILD_TESTS=OFF` + `cmake --build --target re15_pc` -> `[446/446] Linking C executable platform\pc\re15_pc.exe`,
BUILD_EXIT=0; als `re15_pc_abn0c_basis.exe` mit Arbeitsordner im Baum gestartet (gleiche Asset-/synchro-Aufloesung wie
die neue exe: `[asset] cd-root: <baum>/re15_port/shared_assets/PSX`). Lauf r0_basis = exakt die Schalter von r1_tuer
(Echtweg 11C0 -> 11B0, Framedump F1560..F2400 jedes Bild).

- anim.txt Marvin (Aktor 1, Typ 64), Spalten Bild/Aktor/Typ/motion/clip/Bildzahl/cur/slot/Uhr/loco/rueckw.:
  ```
  v0.8.25: 1634 1 64 5 5 20 0 0 20 1 0      neu: 1634 1 64 5 5 30 0 0 30 0 0    (Phase 1->2: Bank 1 Clip 5 statt Loco)
  v0.8.25: 1729 1 64 17 17 10 9 0 10 1 1    neu: 1729 1 64 17 17 10 9 0 10 1 1  (Phase 6 letztes Bild, Index 0)
  v0.8.25: 1730 1 64 17 17 10 9 9 10 0 0    neu: 1730 1 64 17 17 10 0 0 10 1 0  (Uebergabebild Phase 7)
  v0.8.25: 1731 1 64 1 1 16 0 0 16 0 0      neu: 1731 1 64 1 1 16 0 0 16 0 0    (Sub 6: Bank 1 Clip 1)
  ```
  `diff` der anim.txt F1560..F2400: Marvin NUR in F1634 und F1730 verschieden; **alle anderen Aktoren (auch Leon)
  bitgleich**. debug.log neu: `[w11b0-r38] F1730 Aktor 1 Typ 0x40: Schub-Sub Phase 7 (@0x80052bc0, kein anim_set) ->
  Pose Loco Clip 0x11 Bild 0 gehalten bis Sub 6 posiert`.
- Bild (RE15_FRAMEDUMP, D3D11-Renderer, Fenster 1x; Ausschnitt um Marvin x185..245 y40..200), mittlere |Bild(n)-Bild(n-1)|
  (Werkzeug Scratchpad `tools/framediff.py`):
  ```
  v0.8.25: F1728 1.52  F1729 0.95  F1730 4.02  F1731 1.43  F1732 1.64  F1733 1.77  F1734 1.42  F1735 0.94
  neu:     F1728 1.52  F1729 0.95  F1730 0.00  F1731 0.01  F1732 0.06  F1733 0.19  F1734 0.52  F1735 0.60
  ```
  neu gegen v0.8.25 je Bild (F1625..F1760): verschieden nur F1634..F1640 (1.85 -> 0.03, Uebergang Phase 1->2 samt Blend)
  und F1730..F1736 (4.02, 3.76, 3.10, 2.16, 1.26, 0.51, 0.07); alle uebrigen Bilder <= 0.005. Spitzensuche (> 2.0,
  breiter Ausschnitt x175..265) ueber F1626..F1800: v0.8.25 hat eine Einzelspitze F1730 (3.07), neu keine; die
  uebrigen Werte > 2 (F1634..F1644 Uebergang/Ansatz, F1648..F1679 Schub, F1795.. Ada tritt ins Bild) sind fortlaufende
  Bewegungen und in beiden Staenden gleich bis auf das Phase-1->2-Bild (F1634 2.64 -> 2.28, F1636 2.04 -> 1.85).
- Sichtpruefung (Scratchpad `abn/p3_basis_neu_F1727_F1736.png`, oben v0.8.25, unten neu): v0.8.25 F1730 sackt Marvin
  schlagartig in die vorgebeugte Schiebehaltung (Oberkoerper tief, Beine gegraetscht) und richtet sich F1731..F1734
  wieder auf = der gemeldete "Fall"; neu steht er F1727..F1736 aufrecht und geht ohne Sprung in Clip 1 ueber.
  Uebersichten neu F1630..F1718 (Schub) und F1740..F1830 (Stehen, Drehung zur Kamera, Ada kommt): kein weiterer
  Sturz-/Kipp-Moment (`abn/p3_marvin_F1630_F1718.png`, `abn/p3_marvin_F1740_F1830.png`, `abn/p3_marvin_F1724_F1739.png`);
  Phase 1->2: `abn/p3_basis_neu_F1632_F1640.png` (kleine Haltungsaenderung im einen Bild F1634, beide unauffaellig).
- Mechanismus (selbst disassembliert, §6): Phase 6 @0x80052b98 anim_set rueckwaerts, letzter Index 0 (@0x8001f34c/54);
  +0x95 = 0 am Clipende (@0x8001f63c); Phase 7 @0x80052bc0..bf0 OHNE anim_set; Sub 6 [0] -> [1] @0x80051844/@0x8005188c
  posiert erst im Folgebild Bank 1 Clip 1 mit +0x8f = 7. Der v0.8.25-Port zeigte im Phase-7-Bild Containerbank Clip 17
  VORWAERTS Bild 9 (cur 9, loco 0: +0x95 nicht zurueckgesetzt, Richtungsbit geloescht, Bank-1-Regel scheitert an
  Clip 17 >= 14) = genau die gemessene Fehlpose. Der Fix (anim_frame = 0 wie @0x8001f63c, Bank = zuletzt posierter
  Clip) erklaert die Messung vollstaendig: F1730 == F1729 im Bild (0.00), gleicher Keyframe.
- Herkunft: `git merge-base --is-ancestor 3d394d71 <tag>`: v0.8.23 nein, v0.8.24 ja, v0.8.25 ja — der Fall kam mit dem
  Schub-Sub selbst (Runde 36), kein Rueckfall eines richtigen Stands; am gebauten v0.8.25-Code gemessen (oben).

**Urteil Punkt 3: erfuellt** (Fall-Pose im Uebergabebild weg, Mechanismus belegt, sonst bildgleich zu v0.8.25).

## 4. Gezielte Riegel (selbst gefahren)
`ctest --test-dir re15_port/build -R "r38_werkstatt11b0|r36_szenenleon|r35_cut10f0|unit_npc_|unit_marvin_|
unit_elliot_1170_run|unit_1090_npc_prop_push|unit_1090_push|unit_audio_|unit_se_|unit_r31_tuer_ton|unit_r34_plattform_ton|
unit_msg_dialog|unit_anim_|unit_r37_kiste|unit_r36_elza_|unit_r37_roy_|unit_r37_szene11c0_" --timeout 900 --output-on-failure -j2`
-> **`100% tests passed, 0 tests failed out of 63`**, `Total Test time (real) = 321.29 sec` (unter Last: 7 fremde ctest-
Prozesse, ~20 fremde exe-Instanzen). Darin: integration_r38_werkstatt11b0 154.98 s, integration_r36_szenenleon 166.26 s,
integration_r35_cut10f0 249.84 s, unit_r38_werkstatt11b0_text/_takt/_uebergabe/_ton, unit_r36_szenenleon_* (6),
unit_1090_npc_prop_push/_push_pin/_push_anim, unit_r37_kiste*, unit_audio_*/unit_se_*, unit_npc_*/unit_marvin_*.

## 5. Diff gelesen (`git diff master...HEAD -- re15_port`, Code = 467cb958, Bau-exe juenger als jede Quelle)
- `engine/src/gen/szl11b0_szene.inc`: nur k_szl_msg20 (26 -> 33 B) und ein Listing-Kommentar; Programm unveraendert.
- `engine/src/szenenleon_r36.c` (+3): Include; erste Zeile von `re15_szl_schub_npc_kanal` fragt
  `re15_w11b0_schub_kanal(e)` (>= 0 = Aussage); Phase 7 (default-Zweig) ruft `re15_w11b0_schub_uebergabe(e)`.
- `engine/src/werkstatt11b0_r38.c` (neu): `re15_w11b0_ergaenzung_noetig` (nur 0x11B0, Satz 7, Byte 2/3 == 0);
  `re15_w11b0_schub_kanal`: NPC 0x40..0x4d, state 4, kein Walk; Sub 0x13 -> Clip 5 = Bank 1 (2), sonst Loco (1);
  danach im Uebergabebild (Merker gesetzt, Sub 6, +0x6 = 0, Clip 0x11) Loco, Merker verfaellt sobald Sub 6 posiert;
  `re15_w11b0_schub_uebergabe`: anim_frame = 0, Merker, Log. Gegen die alte Regel unterscheidet sich das Ergebnis nur im
  Bild Phase 1 -> 2 und im Phase-7-Bild — am gebauten Spiel bestaetigt (anim.txt-Diff §3/§7: genau diese zwei Bilder).
- `include/re15_werkstatt11b0_r38.h` (neu): Herleitung mit Adressen; Konstanten mit @0x/Offset.
- `platform/pc/src/werkstatt11b0_pc.c` (neu): schneidet beim ersten Bedarf die snd0-Bank aus ROOM1171.RDT (prueft Offsets,
  Satz 7 und VAG-4-Laenge, sonst stumm + Log), dekodiert die VAGs, spielt ueber `re15_audio_se_play_bank`;
  Mess-Haken `RE15_W11B0_SCHUB_STUMM`.
- `platform/pc/src/audio_pc.c` (+2): Include + erste Zeile in `re15_audio_room_se_snd0` fragt den Haken.
- `platform/pc/main.c` (+1): `re15_pc_w11b0_init()` im Init-Block neben den Runde-36/37-Ton-Haken.
- Riegel/Werkzeuge: test_r38_werkstatt11b0.c (text/takt/uebergabe), probe_r38_werkstatt11b0_ton.c (echtes audio_pc.c),
  test_r38_werkstatt11b0.cmake (echte exe, eigener exe-Name, A/B), probes/r38_werkstatt11b0.cmake, tools/r38_c/messlauf.sh,
  szene11b0_bauen.py (MSG20 + Kommentare).
- Anmerkung ohne Folgen: `re15_w11b0_schub_kanal` nimmt `const re15_actor_t *`, schreibt aber den statischen Merker
  (Seiteneffekt in einer Abfrage); der Merker wird beim Raumwechsel nicht geloescht. Ein haengender Merker wirkte nur
  bei einem NPC desselben Slots in Sub 6/+0x6 = 0/Clip 0x11 und verfaellt dann sofort. 11B0: Riegel uebergabe prueft
  "Merker im Folgebild verbraucht" (gruen); 1171: im Folgebild F1048 posiert Bank 1 Clip 1 (anim.txt), die
  Halte-Bedingung greift also nicht mehr.

## 6. RE-Gate — Stichproben selbst disassembliert (info/Re1.5/PSX.EXE, re15_disasm.py)
| Stelle | gelesen | Behauptung im Diff |
|---|---|---|
| 0x80076ca0[19] | `-> 0x80052864`; Phasentabelle @0x800112fc = 528a4/528f4/529b4/529f4/52a74/52ab4/52b84/52bc0 | Sub 0x13 = 0x80052864 — stimmt |
| 0x80052ac0..ad8 | `lbu v1,149(v0)` / `ori v0,zero,0x1` / `bne v1,v0` / `lui a0,0x207` / `jal 0x80045024` (a1 = +0x34) | Satz 7 bei +0x95==1 — stimmt |
| 0x80052bc0..bf0 | `lui a0,0x207` / `jal 0x80045024` / `sb v0,5(v1)` (6) / `sb zero,6(v0)`; KEIN `jal 0x8001f314` | Phase 7 ohne anim_set — stimmt |
| 0x80052b84..bbc | anim_set(+0x84,+0x16c, **a2=1**, 0x200) @0x80052b98, +0x6 += Rueckgabe | Phase 6 rueckwaerts — stimmt |
| 0x8001f314 | rueckwaerts `subu v0,t1,v0` / `addiu v0,v0,-1` = (Bildzahl - +0x95) - 1 | letzter Index 0 — stimmt |
| 0x8001f610..63c | `+0x95 += 1`, `sltu`, @0x8001f63c `sb zero,149(v1)` | +0x95 = 0 am Clipende — stimmt |
| 0x800517f0 (Sub 6) | [0] @0x80051844 +0x6=1, +0x94=1, +0x95=0, +0x8f=7, faellt durch in [1] @0x80051878 anim_set(+0x170,+0x174,0) | Bank 1 Clip 1 + Blend 7 im Folgebild — stimmt |
| 0x800528f4..9b0 | anim_set(+0x170,+0x174) @0x8005294c VOR Rastung `andi 0x3e0` @0x800529a0 und `+0x6=2` @0x800529b0 | Phase 1->2 Bank 1 Clip 5 — stimmt |
| 0x80035a00..a1c | a1 = 0x800acae9 (Spieler+0x95), `bne`, `lui a0,0x207`, `jal 0x80045024` | Kisten-Ton = Satz 7 — stimmt |
| 0x80045024 / 0x80010e70 | `srl v1,a0,24`; Tabelle[2] = 0x800450e4 -> `sltiu s4,0x21` -> 0x80045130 `lw v0,ac778` / `lw a0,8(v0)` / `sll v0,s4,2` / `addu` | Bank 2 = RDT+0x08 snd0 — stimmt |
| Zensus | `lui a0,0x207` + `jal 0x80045024` in PSX.EXE: genau 0x80035a18, 0x80052acc, 0x80052bc8 | stimmt |
| RDT-Bytes | ROOM11B0 @0x170E/@0x171A, ROOM1171 @0x1566/@0x1572 = `40 00 13 14 ...` (Plc_dest Modus 0x13) | stimmt |
| 0x8004523c / 0x80045a18 | Satz-Byte 3 >= 0x10 -> Stimmenpfad `addiu s0,s0,-16`; Prioritaetssperre nur bei `prio < gespeichert` oder gleich + Bit 3 | zwei Anschlaege im selben/naechsten Bild = ein Ton auf Stimme 0 — stimmt |

Diff-Suche (`git diff master...HEAD -- re15_port`) nach deferred/tunable/interim/for now/faithful/plausib/TODO/FIXME/
approx/vermut: **0 Treffer**. Einziger Env-Schalter: `RE15_W11B0_SCHUB_STUMM` (Mess-Haken, schaltet die Ergaenzung AB =
Stand vorher, Standard = Ergaenzung an; kein Ausweg-Schalter). Jede neue Konstante traegt Adresse/Datei-Offset
(RE15_W11B0_SCHUB_SATZ @0x80052acc/@0x80052bc8/@0x80035a18, QUELLE 0x6EDC/0x6F5C/0x87FC/@0x6EF8, VAG 4 6688 B; Text =
NUTZER-VORGABE mit Formbeleg @0x1BD0/@0x1BFA/@0x19CD).

Nebenbefund RE (ausserhalb des Auftrags, kein Mangel dieser Spur): Phase 1 ruft im Original vor dem anim_set
@0x80052930 `jal 0x80045630` (Schritt-Ton: FUN_80045630 liest Bodenart @FUN_800437d4 und spielt snd0-Satz) wenn das
aktuelle Bildwort Bit 0x4000 traegt (`andi v0,v1,0x4000` @0x80052910); der Port-Sub 0x13 (szenenleon_r36.c, Runde 36)
hat diesen Schritt-Ton nicht. In dieser Szene dauert Phase 1 je Schieber 1 Bild (anim.txt F1634) — der Schiebe-Ton
des Nutzer-Punkts ist davon unabhaengig.

## 7. Nebenwirkungen (selbst gemessen)
- **ROOM1171 (Elza, Partner Typ 0x49, derselbe Sub 0x13)**, Schalter wie Runde-36-J-Abnahme:
  `RE15_GOTO_ROOM=1171 RE15_SET_FLAG=3:0xc2 RE15_SET_FLAG_AT=5:28@200 RE15_MSG_LOG=1 RE15_SE_DEBUG=1 RE15_ANIM_TRACE=..
  RE15_EXIT_AT=1#10b1`
  - neue exe, Leon-Modell (h1171_neu): `Plc_dest Modus 0x13` -> `Schub-Sub zu Ende -> Sub 6` -> `[w11b0-r38] F1047
    Aktor 1 Typ 0x49 ... gehalten` -> `[room] PC loaded room10b1.rdt` -> EXIT_AT; 8x `[se] Stimme: se=7 ... -> SE-Stimme
    0` aus der EIGENEN 1171-Bank (keine `[schub-r38]`-Zeile: die Ergaenzung greift dort richtig nicht).
  - v0.8.25-exe, derselbe Lauf (h1171_basis): Szene laeuft ebenso durch; `diff` anim.txt basis/neu: NUR
    `950 1 73 5 5 20 0 0 20 1 0` -> `950 1 73 5 5 30 0 0 30 0 0` und `1047 1 73 17 17 10 9 9 10 0 0` ->
    `1047 1 73 17 17 10 0 0 10 1 0` — derselbe Fall-Fehler bestand in 1171 und ist dort mit behoben; sonst bitgleich.
  - neue exe MIT ELZA (h1171_elza, `RE15_TITLE_CONFIRM_MS=1500 RE15_PSELECT_AUTO=1 RE15_PSELECT_AUTO_SWITCH=1`):
    `[pl] Spieler-Familie PL04 (character=4, Elza-Bit=1)`, Sub 0x13 laeuft, `[w11b0-r38] F1047 Aktor 1 Typ 0x49 ...`,
    `[room] PC loaded room10b1.rdt` — kein Haenger, kein neuer Pfad.
- ROOM11B0-Ton: Mitschnitt v0.8.25-exe gegen neue exe mit `RE15_W11B0_SCHUB_STUMM=1`: **identisch ueber alle 2881
  Bloecke** — der Mess-Haken bildet den Stand vorher exakt ab. Neue exe gegen v0.8.25: verschieden NUR F1647..F1728
  und F1730..F1762 (279 395 136 / 109 329 132 = genau der Schiebe-Ton); v0.8.25-debug.log: 0 Zeilen `se=7 layer`,
  `[msgtext] room=11b0 id=20 "Marvin: Hold on."`. Keine anderen Ton- oder Ablaufaenderungen in der Szene.
- ROOM11B1 (Elza-Variante von 11B0): Satz 7 leer, keine Schub-Szene, Ergaenzung greift nicht (Probe Fall 5, Tor 0x11B0).
- ROOM10F0 (leiht die 11B0-RBJ): integration_r35_cut10f0 + 7 unit_r35_cut10f0_* gruen (§4).

## 8. Vertrag / Pfade / Gates
- `git diff master...HEAD --name-only` (15 Dateien): eigene Neudateien (engine/src/werkstatt11b0_r38.c,
  include/re15_werkstatt11b0_r38.h, platform/pc/src/werkstatt11b0_pc.c, tests/unit/test_r38_werkstatt11b0.c,
  tests/unit/probe_r38_werkstatt11b0_ton.c, tests/unit/probes/r38_werkstatt11b0.cmake,
  tests/integration/test_r38_werkstatt11b0.cmake, tools/r38_c/messlauf.sh, Dossier + C_belege), die Spur C
  zugeteilten gemeinsamen Dateien (tools/r36_j/szene11b0_bauen.py + gen/szl11b0_szene.inc) und kleine Haken mit
  Kommentar "Runde 38 Spur C": platform/pc/main.c 1 Zeile, platform/pc/src/audio_pc.c 2 Zeilen,
  engine/src/szenenleon_r36.c 3 Zeilen (Modul der 11B0-Szene). **Nicht** beruehrt: release/, platform/android/,
  shared_assets/PSX/, tests/CMakeLists.txt, tests/unit/CMakeLists.txt (Registrierung ueber den probes/*.cmake-GLOB).
  (`git diff master --stat` zeigt zusaetzlich r38_w2.js/UEBERGABE.md/mk_r38.py nur, weil master nach der Basis mit
  98d35560 weiterlief — kein Eingriff der Spur.)
- Ressourcen: keine Bank-9-Bits, keine neuen Nachrichten-Ids (msg 20 ist die Runde-36-Id, nur der Wortlaut), keine
  Items/Dokumente; `synchro/STAGE1/room11B0` endet bei main14.wav — keine Sprachdatei umgewidmet.
- Riegel je Punkt vorhanden und messend (P1: unit text + integration A; P2: unit takt + unit ton + integration A/B;
  P3: unit uebergabe + integration anim.txt). Die integration-Regex `^[0-9]+ 1 64 1[78] 1[78] (10|25) [0-9]+ [0-9]+
  [0-9]+ 0 ` faengt nachweislich die v0.8.25-Zeile `1730 1 64 17 17 10 9 9 10 0 0`. Mischer-Probe vergleicht das
  11B0-Fenster mit dem ROOM1090-Fenster (kein Selbstvergleich).
- Testzahl 788 -> 793 (+5); RE15_MIN_TESTS hebt laut Verfahren der Orchestrator bei der Integration.
- Commit 467cb958: jede Konstante mit @0x/Datei-Offset auch in der Nachricht; Dossier fortlaufend committet.
- Beta -> Retail: Dossier zitiert RE2_SOUND_LISTE.md nicht; geprueft wurde RE2 direkt (Code @0x8003f2f4..30c, Raumbaenke
  info/re2leon). Die Liste selbst nennt keinen Container-/Gemeinsam-Schub (nur ROOM203 "movable stepladder", ROOM603
  "box/elevator" = raumeigene Spieler-Schuebe) — deckt sich mit dem Befund, kein Mangel.

## 9. Hinweise (keine Maengel dieser Spur, fuer Orchestrator/Integration)
1. **Umschaltbild Phase 5 -> 6 (vorbestehend, Runde 36):** im Original posiert Phase 5 im Umschaltbild noch Clip 0x12
   (anim_set @0x80052af4 VOR der Flaggenpruefung @0x80052b44 und `+0x94 = 0x11` @0x80052b70); der Port zeigt dort schon
   Clip 0x11 rueckwaerts Index 9 und haelt ihn per Saat-Tick zwei Bilder (anim.txt F1719 und F1720 je `cur 0 slot 9`).
   Zeitachse gleich (Phase 7 in F1730), Bilddifferenz F1718->F1719 0.44 (Nachbarn 0.40..0.60) — unsichtbar, nicht gemeldet.
2. **Schritt-Ton in Phase 1 (vorbestehend):** Original @0x80052910 `andi v0,v1,0x4000` -> @0x80052930 `jal 0x80045630`
   (FUN_80045630 = Schritt-Ton nach Bodenart, FUN_800437d4); der Port-Sub 0x13 hat ihn nicht. Phase 1 dauert in dieser
   Szene 1 Bild je Schieber; unabhaengig vom gemeldeten Schiebe-Ton.
3. Leon F1634 PL00 Clip 5 (Dossier OFFEN 1): vom Bau-Agenten offengelegt, nicht Auftrag, von mir nicht nachgemessen
   (anim.txt Leon ist zwischen v0.8.25 und neu bitgleich — die Spur hat daran nichts veraendert).
4. Android: neue Datei platform/pc/src/werkstatt11b0_pc.c -> frischer Configure noetig (bekannte GLOB-Falle app/.cxx).
5. Kosmetik: `tests/unit/test_r36_szenenleon.c:476` Ausgabetext nennt noch "Hold on." (kein Pruefwert).
6. Integration mit Spur I (heliport1171): der Phase-7-Halt wirkt auch fuer den 1171-Partner (gemessen, Szene laeuft
   durch); bei Aenderungen an szenenleon_r36.c die 3 Haken-Zeilen erhalten.

## 10. Ergebnis
| Punkt | Urteil | Kernbeleg |
|---|---|---|
| 1 Text "But, hold on..." | erfuellt | Echtweg 11C0->11B0: `[msgtext] room=11b0 id=20 "Marvin: But, hold on..."`, Textbox F2124..F2215 (Komma + Auslassung gerendert), Form @0x1BD0/@0x1BFA/@0x19CD, Generator-Ausgabe byte-identisch; v0.8.25 zeigte "Marvin: Hold on." |
| 2 Schiebe-Ton | erfuellt | 8 gekeyte Toene F1646x2/F1671x2/F1696x2/F1729/F1730 im Original-Takt (@0x80052ac0/@0x80052bc8); Mitschnitt neu vs v0.8.25 nur F1647..F1762 verschieden; Ton == Leons Kisten-Ton ROOM1090 (VAG 4 sha1 f343933e) |
| 3 keine Fall-Animation | erfuellt | v0.8.25 F1730 Sprung 4.02 in die Schiebehaltung + Aufrichten; neu 0.00; anim.txt nur F1634/F1730 verschieden; Phase 7 ohne anim_set @0x80052bc0..bf0, +0x95=0 @0x8001f63c |

Gates: RE-Gate gehalten (13 Stellen selbst nachgeprueft, 0 Rate-Woerter, Konstanten mit @0x/Offset oder
NUTZER-VORGABE), Vertrag gehalten, Riegel 63/63 gruen, Nebenwirkungen 1171 (Leon-Modell und Elza) und Ton geprueft.
**bestanden = ja. Maengel: keine.**
