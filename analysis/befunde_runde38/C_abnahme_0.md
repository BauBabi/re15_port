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
- [ ] Punkt 3 gemessen
- [x] RE-Gate (10 Stellen selbst disassembliert/gelesen) — §6
- [ ] Vertrag / Pfade / Nebenwirkungen

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
  auf derselben Stimme 0 neu angeschlagen (Diff F1730 == Diff F1647) — wie FUN_80045024 mit EINER Stimme (Satz-Byte 3
  0x10 -> Stimme 0, @0x8004523c `addiu s0,s0,-16`) es im Original auch taete.
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

## 4. Gezielte Riegel (selbst gefahren)
`ctest --test-dir re15_port/build -R "r38_werkstatt11b0|r36_szenenleon|r35_cut10f0|unit_npc_|unit_marvin_|
unit_elliot_1170_run|unit_1090_npc_prop_push|unit_1090_push|unit_audio_|unit_se_|unit_r31_tuer_ton|unit_r34_plattform_ton|
unit_msg_dialog|unit_anim_|unit_r37_kiste|unit_r36_elza_|unit_r37_roy_|unit_r37_szene11c0_" --timeout 900 --output-on-failure -j2`
-> **`100% tests passed, 0 tests failed out of 63`**, `Total Test time (real) = 321.29 sec` (unter Last: 7 fremde ctest-
Prozesse, ~20 fremde exe-Instanzen). Darin: integration_r38_werkstatt11b0 154.98 s, integration_r36_szenenleon 166.26 s,
integration_r35_cut10f0 249.84 s, unit_r38_werkstatt11b0_text/_takt/_uebergabe/_ton, unit_r36_szenenleon_* (6),
unit_1090_npc_prop_push/_push_pin/_push_anim, unit_r37_kiste*, unit_audio_*/unit_se_*, unit_npc_*/unit_marvin_*.
