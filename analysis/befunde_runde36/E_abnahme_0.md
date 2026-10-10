# Runde 36 Spur E "gegnerzahl" — UNABHAENGIGE ABNAHME 0

Baum `.claude/worktrees/r36_gegnerzahl`, Zweig `r36/gegnerzahl`, HEAD 1e4e373d (Basis master 78a4a748).
Abnahme 2026-10-10. Fortlaufend geschrieben (Sitzungslimit).

## 0. Gegenstand

Nutzer-Punkt (AUFTRAG.md Z.19-20, woertlich):
> "Der Port verwaltet höchstens 15 Gegner je Raum, das Original 20. In ROOM1040 und 1030 fehlen deshalb die letzten
> fünf Zombies, das war auch vorher schon so." --> Das auch!

VERTRAG §0 Spur E: RE15_ACTOR_MAX anheben, alle Nutzer zensieren, ROOM1040/1030 Records 15..19 erscheinen.

Gelesen: AUFTRAG.md, VERTRAG.md, E_gegnerzahl.md, `git log master..HEAD` (12 Commits), `git diff master --stat`
(12 Dateien), den vollstaendigen Code-Diff (re15_actor.h, irons_tod_1150.c, render_pc.c, Tests, Werkzeuge).

## 1. Stand der Abnahme (fortlaufend)

- [x] Bau (§2)
- [x] RE-Gate (§3: 4 Stellen selbst disassembliert, alle wie zitiert, + 2 weitere Belege gefunden)
- [x] Messung Nutzer-Punkt an der exe (§4: M1/M2 echter Tuerweg neu gegen alt, K1/K2 Kampf + Tod + Speichern/Laden)
- [~] Spur-Tests + Testgruppen der beruehrten Module (§5: Unit 496/496 gruen; Integration laeuft)
- [x] Zensus-Stichprobe (§6)
- [x] Vertrag (§7)

## 2. Bau

`cd <baum> && bash re15_port/tools/local_build.sh configure && bash re15_port/tools/local_build.sh build` ->
`=== LOCAL-BUILD-OK (configure)`, `ninja: no work to do.`, `=== LOCAL-BUILD-OK (build)`, EXIT=0. Baum sauber
(`git status --short` leer), HEAD 1e4e373d. mtime render_pc.c 21:50:00 < re15_pc.exe 21:50:04 (re15_actor.h 21:18,
irons_tod_1150.c 21:31) -> die exe ist der Stand von HEAD. Messkopien (eigene Namen, Scratchpad `E0/bin/`):
`re15_pc_abn0_neu.exe` (md5 498327f8...) = Spurbaum-exe; `re15_pc_abn0_alt.exe` (md5 2fd81d20...) = Hauptbaum-exe vom
2026-10-04 18:37 (Code-Stand v0.8.23, RE15_ACTOR_MAX 16) als Vorher-Referenz; Kartenwerkzeug = Spurbaum
`probe_r36_gegnerzahl_karte.exe` (Speicherformat unveraendert, beide exe laden die Karte).

## 3. RE-Gate — selbst disassembliert (`re15_disasm.py dis`, info/Re1.5/PSX.EXE des Auslieferungsstands)

| Zitat des Bau-Agenten | selbst gelesen | Urteil |
|---|---|---|
| FUN_80037250 `sltiu v0,v0,0x14` @0x800372b4, Umbruch auf 0x800acc2c @0x800372c4/c8 | `800372ac addiu a2,a2,1` / `800372b0 andi v0,a2,0xff` / `800372b4 sltiu v0,v0,0x14` / `800372b8 bne v0,zero,0x800372cc` / `800372bc addiu a1,a1,500` / `800372c0 addu a2,zero,zero` / `800372c4 lui a1,0x800b` / `800372c8 addiu a1,a1,-13268` (= 0x800acc2c). Basis a1 = 0x800aca4e + 478 = 0x800acc2c (@0x80037254/58), Index = (Ziel - Basis) * 0x10624dd3 >> 5 = /500 | stimmt |
| Feld-Ende 0x800af33c @0x80039b38/@0x80039b40 | `80039b34 lui v1,0x800b` / `80039b38 addiu v1,v1,4556` (0x800b11cc) / `80039b40 addiu v1,v1,-7824` -> 0x800b11cc - 0x1e90 = **0x800af33c**; Stride 0x6C (`80039b58..68`: n*8-n, *4, -n, *4 = 108n), Zaehler `lbu v0,-1097(v0)` = 0x800afbb7. 0x800acc2c + 20*0x1F4 = 0x800acc2c + 0x2710 = 0x800af33c | stimmt (Annotation des Werkzeugs "0x800ae170" ist dessen naive lui-Basis, nicht der Wert) |
| Sce_em_set: Slot = pc[1]&0x7f @0x800420f8, Entity = Basis + Slot*0x1F4 @0x800420dc/@0x80042118, ohne Grenze | `800420ac/b0` a2 = 0x800ac77c, `800420dc addiu a0,a2,1200` = 0x800acc2c, `800420f0 lbu v0,1(a1)`, `800420f8 andi s4,v0,0x7f`, `80042100..80042110` s4*500, `80042118 addu s0,v0,a0` (Delay-Slot). Zwischen @0x800420f8 und dem Limit kein Vergleich von s4 | stimmt |
| Tot-Bit-Gate @0x80042120-38 VOR Limit @0x80042214-3c | `80042120 lbu a1,7(a1)` / `80042128 beq a1,0xff` / `80042130 jal 0x8004efe4` (a0 = a3 = 0x800ac77c+18620 = **0x800b1038** Zone 7, bzw. +18652 = 0x800b1058 wenn `lh 0x800b0fe0 >= 3`) / `80042138 beq v0,zero`; Limit `80042214 lh v0,0(a1)` (0x800b0ff2) / `8004221c lh v1,4084(v1)` (0x800b0ff4) / `80042224 slt` / `80042228 bne` / voll: `80042230 ori v0,zero,0x8000` + `80042238 sw v0,0(s0)` / frei `8004223c sh v0,0(a1)` | stimmt |
| Pro-Bild-Schleife @0x8001a528/@0x8001a5b4 an den Zaehler 0x800aca4e gebunden | `8001a528 lbu s1,-13746(s1)` (0x800aca4e), Zeiger 0x800ac784 := 0x800acc2c (`8001a52c/40`), `8001a564 andi v0,v0,0x1` / `8001a588 jalr v0` / `8001a58c addiu s0,s0,1` (Delay-Slot), `8001a5a8 addiu v0,v0,500`, `8001a5b4 sltu v0,v0,s1` | stimmt |
| (nicht zitiert, ZUSAETZLICH gefunden) | **FUN_8001a4c0** (Feld-Loeschen): `8001a4c0 ori a0,zero,0x13` / `8001a4e0 addiu at,at,-13268` (0x800acc2c) / `8001a4e8 sw zero,0(at)` / `8001a4f4 bne v0,zero` / `8001a4f8 addiu a0,a0,-1` = Plaetze 19..0. **FUN_80011f50** (Zielsuche beim Schiessen, Decompilat Z.51): `while ((bVar9 != 0) && (puVar11 = puVar11 + 0x7d, bVar7 < 0x14))` | zwei weitere unabhaengige Belege fuer 20 Plaetze |

RE-Gate fuer die eine Verhaltenskonstante des Diffs (`RE15_ACTOR_MAX 21` = Spieler + 20, Port-Slot = Skript-Slot + 1,
`SCRIPT_SLOT_TO_ACTOR` scd_vm.c:3186): **haelt**. Kommentar in re15_actor.h traegt @0x800372b4, @0x80039b38/@0x80039b40,
@0x800420f8, @0x800420dc/@0x80042118. `SHADOW_QUAD_MAX RE15_ACTOR_MAX` (render_pc.c) zitiert @0x800372b4 + FUN_8001b064 je
Entity; Sentinel irons_tod_1150.c = `RE15_ACTOR_MAX - 1` (symbolisch, kein neuer Zahlenwert). Grep des Diffs nach
deferred/tunable/interim/for now/faithful/plausibel/TODO: s. §6.

## 4. Messung des Nutzer-Punkts an der exe (eigene Laeufe, andere Faelle als der Bau-Agent)

Werkzeug: Scratchpad `E0/run.sh` (Karte `probe_r36_gegnerzahl_karte` mit Tot-Bits Zone 7, CONTINUE, Aktionstaste an der
ORIGINAL-Tuer, Muster gz_lauf) + `E0/ana.py` (state.log). Umgebung: `RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_WINDOW_SCALE=1
RE15_CONTINUE_TEST=1 RE15_CARD_AUTO=1 RE15_CARD_SLOT=0 RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=60
RE15_INPUT_SCRIPT=W10,A0.2,W200 RE15_STATE_LOG=state.log RE15_SPAWN_DIAG=1 RE15_EXIT_AT=350#<ziel>` (Tuerdruck erst bei
Spielbild 360, weil frame_count beim Raumwechsel auf 0 springt (main.c:8114) und das Skript im Zielraum neu laeuft —
so bleiben 350 Bilder Beobachtung im Zielraum ohne zweiten Tuerdruck).

### M1: ROOM1040 -> Tuer -> ROOM1030, in 1030 Records 0..11 tot (Bits 7:0-11), 1040 leer (7:20-39)
Soll (Original-Regel Tot-Bit vor Limit 6 @0x1de2, 20 Plaetze): Records 12..17 = Aktor 13..18.
| exe | debug.log | letzte state.log-Zeile F350 (t=16) |
|---|---|---|
| neu | `CONTINUE: resumed in room 1040`, `[room] PC loaded room1030.rdt`, `EXIT_AT: Bild 350 in Raum 1030` | **6: {13,14,15,16,17,18}**, alle st=1, laufen auf den Spieler zu (Aktor 16: d 31956 -> 27613, 18: 31985 -> 27878) |
| alt | dito | 3: {13,14,15} — die Records 15..17 fehlen (Befund reproduziert) |

### M2: ROOM1030 -> Tuer -> ROOM1040, in 1040 Records 0..12 tot (7:20-32), 1030 leer (7:0-19)
Soll (Limit 5 @0x11f0): Records 13..17 = Aktor 14..18.
| exe | letzte state.log-Zeile F350 (t=16) |
|---|---|
| neu | **5: {14,15,16,17,18}**, st=1, hinter dem Tor (x -27772..-29528, z 14875..22251) |
| alt | 2: {14,15} |

### C20: Nebenbefund des Bau-Agenten (CONTINUE in ROOM20B0, Kamera bleibt Cut 0) — Gegenprobe alt/neu
Der Bau-Agent schrieb im Dossier §8 "Ursache **vermutlich** der synthetische Kartenstand" (Rate-Wort, ungemessen). Selbst
gemessen: Karte ROOM20B0 (-12635,-2905) Blick 0 ohne Bits, CONTINUE, `RE15_INPUT_SCRIPT=W200`, Ende Bild 300, beide exe:
| exe | Spieler F0 -> F150 -> F300 | cam | pm | Emitter t=24 letzte Zeile | debug.log |
|---|---|---|---|---|---|
| neu | (2380,-4132) -> (-15917,-4132) -> (-15920,-4132) | 0 durchgehend | 2 | 20 | `[scd F0] Cut_chg(11)`, `[schatten] gezeichnet 0, verworfen 0 (Kapazitaet 21)` |
| alt | identisch Bild fuer Bild (F0/F50/F100/F150/F200/F250/F300) | 0 durchgehend | 2 | 15 | `[scd F0] Cut_chg(11)` |
=> Das Verhalten besteht schon im Vorher-Stand (v0.8.23-Code) unveraendert; es ist KEINE Folge dieser Spur. Die
Ursachen-Aussage "vermutlich ... Kartenstand" bleibt unbelegt (Hinweis H2), blockiert die Spur aber nicht.

### K1/K2: die Aktoren 16..20 sind vollwertige Zombies — beschiessbar, toetbar, Tod bleibt ueber Speichern/Laden
(Der Nutzer-Satz "es fehlen die letzten fuenf Zombies" ist erst erfuellt, wenn die fuenf auch als Gegner funktionieren.)
K1/K2a: `run_jump.sh` = Debug-Sprung `RE15_DEBUG_JUMP=1030@250`, `RE15_SET_FLAG=7:0,...,7:14` (Records 0..14 tot),
`RE15_GIVE=7:30 RE15_EQUIP=7` (Super Redhawk), `RE15_PLAYER_POS=-10000,-22500,1024`, Skript `W8` + 40x `M0.3,MA0.1,M0.6`
(Zielen + Schuss), Ende 1600 (K1a) bzw. Speichern bei Bild 1500 `RE15_SAVE_TEST_AGAIN=1500 RE15_CARD_AUTO=1
RE15_CARD_SLOT=0 RE15_SAVE_TEST_EXIT_AFTER=1` (K2a). K2b: `run_cont.sh` = CONTINUE von der in K2a geschriebenen Karte.
| Lauf | Ergebnis (state.log / debug.log) |
|---|---|
| K1a (Blick 1024) | Aktor 16..20 erscheinen; **16, 18, 19, 20 getoetet** (st 0->1->3->7, hp -> -1), 17 lebt (hp 50) |
| K1b (Blick 3072, Gegenprobe abgewandt) | kein Treffer: alle fuenf st 1, hp 50..80 (die Schuesse in K1a trafen wirklich die Aktoren 16..20) |
| K2a (= K1a + Speichern) | `[save] saved (room 1030) slot n=0 -> next=1`; dieselben vier tot, 17 lebt |
| K2b (CONTINUE dieser Karte) | `CONTINUE: resumed in room 1030`, 20x `Sce_em_set type=0x16`, letzte Zeile F120: **1 Zombie {17}** auf Aufstellungs-Fall 0 (-8978,-27878) |
=> Die Todes-Bits der Records 15/17/18/19 (Zone 7, Bits 15/17/18/19 aus `em_flag_id` = pc[7], scd_vm.c:3803) werden
gesetzt, gespeichert und beim Raumaufbau vor dem Limit gelesen; nur Record 16 (Aktor 17) kommt wieder. Der ganze
Kreislauf (erscheinen -> kaempfen -> tot bleiben) laeuft mit den neuen Plaetzen.

### EL: Elza-Szenario ueber den echten Weg (ROOM1041 -> Tuer -> ROOM1031)
Der Debug-Sprung taugt dafuer nicht (gemessen: `RE15_DEBUG_JUMP=1031@250` laedt `ROOM1030` — das ist der Spur-I-Befund
"Debug-Menue schickt Elza in Leons Raeume"), und `probe_r36_gegnerzahl_karte` kennt keinen Charakter. Deshalb: Karte mit dem
Werkzeug geschrieben (Raum 1040, (-20083,-3559) Blick 0, Bits `7:20-39 7:0-14 3:125` = 1041 leer, 1031 Records 0..14 tot,
Vorbedingung Ck(3,0x7d) der 1031-Liste @0x01E54) und im Scratchpad mit `E0/karte_elza.py` umgestellt (re15_savedata_t:
room @24 -> 0x1041, character @34 -> 4 = PL04, Bytesummen-Checksumme @940 neu; Satz @0x2100 der Karte). CONTINUE +
Aktionstaste (`run_cont.sh`, Skript `W10,A0.2,W200`, Ende 350#1031):
| exe | debug.log | letzte state.log-Zeile F350 in ROOM1031 (t=16) |
|---|---|---|
| neu | `[pl] Spieler-Familie PL04 (character=4, Elza-Bit=1)`, `CONTINUE: resumed in room 1041`, `DOOR FIRE slot=1` -> `room1031.rdt`, Raumskript `DOOR FIRE slot=18` -> `room1241.rdt` -> `room1031.rdt` | **5: {16,17,18,19,20}**, st=1 |
| alt | dieselbe Raumfolge 1041 -> 1031 -> 1241 -> 1031 (Elza-Ablauf, unabhaengig von dieser Spur) | **0: {}** — die fuenf fehlten auch bei Elza |

### RDT-Bytes der Records 15..19 selbst gelesen (xxd, shared_assets/PSX/STAGE1)
`ROOM1030.RDT @0x1de2: 24 12 06 00 | 44 00 16 0d ...` (Save(0x12,6), unmittelbar danach Record Slot 0);
`@0x1f12: 44 0f 16 0d 00 00 01 0f` (Slot 15, Typ 0x16, pc[7] = Tot-Bit 0x0F) ... `@0x1f62: 44 13 16 0d 00 00 01 13` (Slot 19,
Tot-Bit 0x13); `ROOM1040.RDT @0x11f0: 24 12 05 00` (Save(0x12,5)), `@0x1328: 44 0f 16 0d 00 00 01 23` (Slot 15, Tot-Bit 0x23).
Zensus-Werkzeug `tools/r36_e/sce_em_slot_zensus.py shared_assets/PSX` selbst gefahren: `files=240 Sce_em_set-Records=844
walk-stops=0`, Slot >= 15 nur in ROOM1030/1031/1040/1041 (Slots bis 19) und ROOM20B0/20B1 (bis 34) — wie Dossier §3.4.

## 5. Tests (selbst gefahren am Spurbaum, HEAD 1e4e373d)

| Lauf | Befehl | Ergebnis |
|---|---|---|
| Unit gesamt + Spawn/Tuer-Sonden (der Diff aendert RE15_ACTOR_MAX, also JEDES Modul) | `ctest --test-dir re15_port/build -R "^unit_\|^prop_spawn_entered$\|^probe_r16_tuer1040_" -E "android_bash" --timeout 600 -j4` | `100% tests passed, 0 tests failed out of 496` (183,91 s), darunter unit_r36_gegnerzahl_{feld,raum1030,raum1031,raum1040,raum1041,raum20b0,kein_platz}, unit_r35_cut1150_* (12), unit_1030_*, unit_room1040_switch, unit_gator_*, unit_alligator_ai |

(Integration: s. unten, wird nachgetragen.)

## 6. Zensus-Stichprobe (unabhaengig vom Bau-Agenten, grep im Spurbaum)

| Klasse | Muster | Treffer mit Aktor-Bezug |
|---|---|---|
| Schleifen mit fester 15/16 | `for (...; ... < 1[56];` in engine/src, platform/pc | keine (bss_mdec, climb/collision `prop_count && q < 16` = PROPS, re2z_part_* je Teil, pad, memcard-CLUT, inv_render, msgfont-CLUT, Pad-Konfig) |
| Felder [15]/[16] | Deklarationen | keine je Aktor (HP-Tabellen `rng&0xf`, NPC-Parametertabellen je Typ, s_re2d_fx_zaehler je Effekt-Nr, nav A/B/C je Tiefe, s_prop_push_count je Objekt, Pad-/Menue-Tabellen) |
| Felder [17..20] | Deklarationen | keine je Aktor (re2z_pool_cost_w1[20] je Angriffs-Id, s_re2*_rec_w0[20]/s_re2z_geo[20] je Waffe) — wichtig, weil Aktor 20 jetzt ein gueltiger Index ist |
| Vergleiche Slot-Variable gegen 15/16 | `(slot\|ai\|si\|idx\|aktor\|...) (<\|<=\|>\|>=\|==) (15\|16\|0xf\|0x10)` | keine (grid1-Logger idx, Pad-Slot) |
| Bitmasken `1u << slot` | | u32 (enemy_common.c rbj_record_marker, main.c s_diag_seen ai < 32) — reicht fuer 21 |
| Defines 15/16 | `#define X 1[56]` | keine Aktor-Kapazitaet (SCD_SLEEP_SLOT_COUNT je Faden, RE15_DOK1_SLOT = AOT-Slot, SS_CHANNELS Audio, ...) |
| "hoechster freier Platz" | `RE15_ACTOR_MAX - 1` | main.c:7502 Alligator 2090 (jetzt Slot 20, RDT nutzt dort keinen Slot >= 15) und der Sentinel irons_tod_1150.c:697 |
| Schuss-Zielauswahl (Original FUN_80011f50 `bVar7 < 0x14`) | re15_damage.c | alle Schleifen `s < RE15_ACTOR_MAX` (Z.919/1005/1065/1104/1976/3540/4029/4333) — gemessen: K1a trifft Aktor 16..20 |
| Work_set kind 2 auf Gegner 15..19 (ROOM1030 sub04/06, 1040 sub09/10) | scd_vm.c scd_work_bind | `slot < RE15_ACTOR_MAX && g_actors[slot].active` (Z.3224) — symbolisch |
| Gore-/TIM-Slots | main.c:932 `46 + (eb - g_enemy)` | je BANK (RE15_ENEMY_MAX 4), nicht je Aktor |

Ergebnis: kein weiterer fester 15/16-Nutzer mit Aktor-Bezug gefunden; die zwei Funde des Bau-Agenten (Sentinel, Schattenliste)
sind die einzigen. Schattenliste: FUN_8001b064 = "character floor SHADOW draw (per entity)" (RE15_FUN_CATALOG.md:223,
Decompilat liest die laufende Entity DAT_800ac784) — eine Liste in Aktor-Groesse ist folgerichtig.

## 7. Vertrag (VERTRAG.md)

* Pfade (`git diff master --name-only`, 12 Dateien): analysis/befunde_runde36/E_gegnerzahl.md, include/re15_actor.h (Eigentum
  E laut §1.4), engine/src/irons_tod_1150.c (Eigentum K: 2 Zeilen = Sentinel + Kopfkommentar), platform/pc/src/render_pc.c
  (gemeinsame Datei: 1 Konstante + Verwurf-Zaehler + 7-Zeilen-Messhaken, s. H3), tests/integration/test_r35_cut1150.cmake und
  test_r35_granate.cmake (Erwartungen der alten 15er-Grenze, s. §5), eigene Tests/Werkzeuge. KEINE Treffer fuer release/,
  platform/android/, shared_assets/PSX/, tests/unit/CMakeLists.txt, tests/integration/CMakeLists.txt, RELEASE_NOTES.
* Bank-9-Bits / Nachrichten-IDs / Ereignisse: keine benutzt (E hat laut §1.3 keine) — Diff bestaetigt.
* Rate-Woerter im Diff (`+`-Zeilen: deferred/tunable/interim/for now/faithful/plausib/TODO/vermutlich/ungefaehr/geschaetzt):
  ein Treffer, Dossier §8 "Ursache vermutlich der synthetische Kartenstand" (Nebenbefund ausserhalb der Spur; s. C20, H2).
* Env-Schalter: `RE15_SCHATTEN_LOG` ist ein reiner Messhaken (nur stderr), kein Verhaltensschalter, kein Abschluss-Ausweg.
* Die geaenderten r35-Erwartungen sind KEINE Aufweichung: Lauf F verlangt jetzt "5 erscheinen im Port, Auffuellung 0" und
  "Zaehler 5 -> 5" statt "0 ... Auffuellung 5" / "5 -> 0" — dieselbe Schaerfe, die Bilanzzeile ("5 Zombies, durchs Tor 5")
  bleibt unveraendert; granate Lauf gator Slot 15 -> 20 folgt aus der Rueckwaertssuche (main.c:7502).
