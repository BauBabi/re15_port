# Runde 35 — Spur I "entladen": UNABHAENGIGE ABNAHME 2 (nach Nachbesserung 2)

Baum `.claude/worktrees/r35_entladen`, Zweig `r35/entladen`, Stand `2a03e9c0` (letzter Code-Commit
`e1cee092`, danach nur Dossier). merge-base mit master = `87cc8575` (= master HEAD, der Merge `7f547d99`
hat den aktuellen master geholt) -> `git diff master` ist genau der Spur-I-Anteil. Abnahme 2026-10-04.
Alles unten ist am gebauten Stand SELBST gemessen: eigene exe-Kopie `re15_pc_abn_i2.exe` (nach der
Messung geloescht), eigene Arbeitsverzeichnisse im Scratchpad (`abn_i2/<lauf>`), PATH msys64 zuerst,
kein taskkill. Drei unabhaengige Messwege neben der Messschiene des Bau-Agenten (`RE15_ENTLADEN_LOG`):
Bildvergleich gegen frische Prozesse (SHA1), RAM-Lesen mit gdb ueber die Symbole der exe
(Haltepunkte `re15_testhaken_ende`, `re15_entladen_ereignis`, `re15_audio_re2_raumbaenke_entladen`),
und Code-Zensus aller Lade-Caches nach Dingen, die der Zensus NICHT zaehlt.

## Ergebnis

| Punkt (Wortlaut AUFTRAG.md Z. 24) | Urteil |
|---|---|
| 1. "Nachdem ich gestorben bin und new game mache habe ich teilweise noch PRIs von meinen Spielstand davor ... angezeigt" | **erfuellt** |
| 2. "Wenn man tot ist, aber auch wenn man den Raum wechselt sollen saemtliche Assets von den Raeumen davor entladen sein" | **teilweise** (Mangel 1) |

Der Mangel der Abnahme 1 (RBJ/ROOM1170.RBJ, 55 060 B) ist behoben und mit gdb nachgemessen (Tuer und
Tod). Neu gemessen: die Lampen-Grafik des Generator-Bedienfelds ROOM11F0 (`LAMPE2130.TIM`, Schnitt aus
RE2 ROOM2130.RDT, dekodiert 2 x 32 x 32 x u16 = 4 096 B) bleibt nach Raumwechsel und Tod geladen; der
Zensus kennt sie nicht und meldet "alle 18 Faecher 0".

Gates: Suite gruen (531/531, Log im eigenen build/, nach dem exe-Bau; `r35_entladen` 14/14 selbst
gefahren) · @0x-Gate haelt (9 Stellen selbst disassembliert/gescannt, alle stimmen) · Pfad-/Vertrags-
Gate haelt (Ueberschreitung der 1-5-Zeilen-Regel offen im Dossier benannt) · Tests vorhanden und messend.
**bestanden = false** (Punkt 2 nur teilweise).

## Bau und Tests

```
cd <baum> && bash re15_port/tools/local_build.sh configure && bash re15_port/tools/local_build.sh build
  -> Toolchain OK / ninja: no work to do / === LOCAL-BUILD-OK (build)
     exe 2026-10-04 06:16 (Code-Stand; git diff e1cee092 HEAD -- re15_port = leer)
Dossier: woertlich "=== LOCAL-BUILD-OK (all) — Tests 531/531" (N >= 478) -> Suite nicht neu gefahren.
  Beleg re15_port/build/local_build_ctest.log (07:03, nach dem exe-Bau): "100% tests passed, 0 tests
  failed out of 531", Total 2559.82 sec; integration_boot_bg_pin/dark_start_pin/relatch_pin/
  save_counter_pin/weste_load_pin alle Passed; unit_r35_entladen_* und integration_r35_entladen_a..j Passed.
ctest --test-dir <baum>/re15_port/build -R r35_entladen -j2 --output-on-failure   (selbst)
  -> 14/14 Passed (a 22.4 s, b 15.8, c 39.7, d 45.6, e 27.4, f/g/h/i/j, 4 Unit), Total 119.85 s
```

## Punkt 1 — PRIs des alten Spielstands nach Tod + NEW GAME

Laeufe (je eigene exe-Kopie, `RE15_FPS=240`, Titel ueber `RE15_TITLE_SHOT=t.bmp RE15_TITLE_SHOT_AF=60
RE15_TITLE_CONFIRM_MS=8000 RE15_PSELECT_AUTO=1` = Titelmenue -> "PLEASE SELECT MAIN CAST" -> neues Spiel,
`RE15_ENTLADEN_SHOT=es` + `RE15_ENTLADEN_SHOT_BILD=10,30,60,...,3100` = komponiertes Bild vor dem Present):

| Lauf | Spiel 1 (Tod) | danach |
|---|---|---|
| a4_tod1030 | `RE15_DEBUG_JUMP=1030@5 RE15_KILL_AT=3200 RE15_BOOT_EXIT_AT=3` — Tod in ROOM1030 | Titel+Auswahl, NEW GAME, Montage 1240 -> Story-Tuer -> 1170 |
| a3_tod1170m | `RE15_KILL_AT=3200 RE15_BOOT_EXIT_AT=3` — Montage -> Tuer -> Tod in ROOM1170 (Helipad, Masken) | dito |
| a1_tod1170 | `RE15_GOTO_ROOM=1170 ...` — Tod in 1170 ohne Masken (Vergleich) | dito |
| f1_frisch / f1b (identisch, 14/14) | Montage -> Tuer -> 1170, Tod | nur Titel+Auswahl (BOOT_EXIT_AT=2) |
| f2_frisch_titel | kein Spiel davor (`RE15_TITLE_CONFIRM_MS=1 RE15_BOOT_EXIT_AT=1`), Bilder 1..640 | — |

Messschiene (Vorbedingung des Befunds vorhanden):
```
a4: VORHER spielende gen=3 raum=1030 | belegt pri_masken=22 pri_atlas=1 sld=1 tim=5 gegner=1 esp_bank=1 rdt=1 bg=1
    SUMME seit=raum bilder=3461 bilder_mit_masken=3276            (die 22 Masken wurden gezeichnet)
    EREIGNIS spielende gen=4 | belegt (alle 18 Faecher) 0
    SUMME seit=spielende bilder=599 bilder_mit_masken=0 bilder_fremde_masken_gezeichnet=0
a3: VORHER spielende gen=3 raum=1170 | belegt pri_masken=21 pri_atlas=1 sld=1 tim=8 gegner=1 figur=1 rbj=1 ...
    SUMME seit=raum bilder=3461 bilder_mit_masken=1856 ; EREIGNIS spielende gen=4 alle 0 ;
    SUMME seit=spielende bilder=599 bilder_mit_masken=0 ; in keinem Lauf eine BILD-Zeile
```
Bildvergleich (`vergleich.py`, SHA1):
```
neues Spiel nach dem Tod gegen frisch (f1, Spiel 1), Bild fuer Bild gleicher Nummer:
  a4 gen5 (ROOM1240) vs f1 gen2: 26/26 bytegleich   a4 gen6 (ROOM1170) vs f1 gen3: 27/27 bytegleich
  a3 gen5 vs f1 gen2: 26/26 bytegleich               a3 gen6 vs f1 gen3: 27/27 bytegleich
  a1 gen5/gen6: 26/26 + 27/27 bytegleich
Titel+Auswahl nach dem Tod in 1030 (22 Masken):
  a4 gen4 vs a1 gen4 (Tod ohne Masken): 14/14 bytegleich
  a4 gen4 in der Frisch-Menge f2 (600 Bilder): 12/14; die zwei ohne Treffer sind b010/b030 = Titel-
  Einblendung nach JEDEM Tod (auch a1), s. Hinweis H2 — kein Raumstueck.
```
=> Nach dem Tod in einem Raum mit 22 bzw. 21 gezeichneten Masken zeichnet weder Titel noch Auswahl noch
das neue Spiel eine Maske des alten Spielstands; das neue Spiel ist bytegleich mit einem Spiel ohne Tod
davor. Mechanismus: `re15_entladen_ereignis("spielende")` vor `goto re_title` (main.c, Haken am Ende von
main) leert Maskenliste/Atlas (`re15_render_pc_entladen_raum`), der PRI-Riegel leitet je Generation neu
ab. (Gesondert: die Auswahl nach einem Tod WAEHREND einer Szene zeigt Kinobalken — kein PRI, s. H1.)

**Urteil Punkt 1: erfuellt.**

## Punkt 2 — saemtliche Assets der Raeume davor entladen (Tod UND Raumwechsel)

### Was nachweislich faellt (gdb, RAM am Testhaken-Ende; Skript `speicher.gdb`)
| Lauf (echter Weg) | Vorbedingung (Zensus VORHER / debug.log) | RAM danach (gdb) |
|---|---|---|
| g1 Intro -> 1170 -> Tuer AOT 4 -> 1130 (`RE15_FIRE_AOT=4@3000#1170 RE15_EXIT_AT=100#1130`) = Abnahme-1-Lauf r9 | `VORHER raum gen=3 raum=1170 ... figur=1 rbj=1 rbj_datei raum=1170 bytes=55060` | `s_rbj=(nil) size=0`, `s_leih_buf=(nil)`, Bindung `s_room_rbj=(nil)`, Elliot md1/edd/emr `(nil)`, alle RE2-Baenke 0 |
| g2 Tod in 1170 -> NEW GAME -> Bild 200 ROOM1240 (`RE15_GOTO_ROOM=1170 RE15_KILL_AT=3200 RE15_EXIT_AT=200#1240`) = r10 | `VORHER spielende gen=3 raum=1170 ... rbj=1 rbj_datei raum=1170 bytes=55060` | `s_rbj=(nil)`, `s_leih_buf=(nil)`, `s_room_rbj=(nil)` |
| g3 Kraehen-Raum 10C0 -> Tuer AOT 0 -> 1060, Dummy-Ton | `[re2se] ENEMSE Bank 7 geladen`, `VORHER raum raum=10C0 gegner=1 ton=2 re2ton=1` | ENEMSE-Cache `[0] bank=-1 loaded=0`, `[1]/[2] loaded=0` |
| g4 Fahrstuhl 1080: Fahrt (`RE15_SUBSTART=7@40#1080`) -> Skript-Tuer `Aot_on 0` -> 1040, Dummy-Ton (Dossier O6) | gdb an der Grenze in 1080 VOR dem Entladen: `ELEVSE loaded=1 edt=0x502d1710` | direkt danach `ELEVSE loaded=0 edt=(nil)`; in 1040 Bild 100 dito |
| g5 10F0 (Leihe Spur K) -> Tuer AOT 0 -> 10D0 | `Animationsblock von ROOM11B0 geliehen (48168 B)`, `VORHER raum raum=10F0 pri_masken=27 gegner=2 rbj=1` | `s_leih_buf=(nil)`, `s_rbj=(nil)`; Bindung zeigt in die NEUE RDT (RDT-Alias 10D0) |

In allen Laeufen: jede `EREIGNIS`-Zeile `belegt` 0, keine `BILD`-Zeile. Dazu Punkt-1-Laeufe a1/a3/a4
(Tod) und die Montage-Tuer 1240 -> 1170 (raum). Die RBJ-Freigabe sitzt in `alles_entladen` Schritt (11)
(`re15_entladen_rbj_halten(NULL,0,0)` + `re15_cut10f0_pc_rbj_freigeben()`), Tuer- und Boot-Weg in main.c
uebergeben den Puffer. Fix erklaert den Befund der Abnahme 1: Vorbedingung `rbj_datei raum=1170
bytes=55060` steht im Protokoll, danach liest gdb `(nil)` (vorher Abnahme 1: Kopf `04 d7 00 00 ...`).

Gefensterte Freigabe geprueft (Code): zwischen `re15_room_load` (Schritt 11 gibt den RBJ-Puffer frei)
und dem Neu-Overlay im RBJ-Block von main.c liest niemand Leons Keyframes (`keyframe_data` kommt in
scd_vm.c/scd_room_setup.c/room_common.c nicht vor; nur emd_common/enemy_ai_common/re2_ems = Gegnerbaenke,
die Schritt (3) vorher verwirft).

### Mangel 1 — Lampen-Grafik ROOM11F0 (LAMPE2130) bleibt nach Raumwechsel und Tod geladen (NEU, gemessen)
`platform/pc/src/panel_lampen_pc.c` (master, Spur C Runde 34 Nacht): `laden()` liest
`shared_assets/RE2/LAMPE2130.TIM` (= byte-gleicher Schnitt ROOM2130.RDT[0x0E398, +4256), Dateikopf von
panel_lampen_pc.c), dekodiert in `static uint16_t s_zelle[2][32*32]` und merkt `static int s_zustand = 1`.
Gezeichnet nur in ROOM11F0/11F1 Cut 10 (`re15_panel_lampe_sicht`). `s_zustand` wird nirgends
zurueckgesetzt; `alles_entladen` (entladen_pc.c) ruft panel_lampen_pc nicht; kein Zensus-Fach.
Gemessen mit gdb (Skript `lampe.gdb`, Wert `*(int*)((char*)&s_zelle - 0x20)` = `s_zustand`, nm: liegt
direkt vor `s_zelle`), Lampe sichtbar gemacht ueber `RE15_DEBUG_JUMP=11F0@5 RE15_FORCE_CUT=10
RE15_SET_FLAG_AT=5:13,5:15,5:17@100` (Schalter-Maske 0x015 = obere Lampe, panel.log `cut=10 ... lampe_o=1`):
```
g6c (Tod):  BP re15_panel_lampen_pc_dekodieren in Raum 11F0 (zustand -1 -> wird geladen)
            BP re15_entladen_ereignis "spielende" (Eintritt)  Raum 11F0  lampe_zustand=1
            BP re15_entladen_ereignis "spielstart" (Eintritt, NACH dem Entladen am Tod)  lampe_zustand=1
            Testhaken-Ende im NEUEN Spiel (BOOT_EXIT_AT=2, Raum 1240)  lampe_zustand=1
            entladen.log: EREIGNIS spielende gen=4 raum=11F0 | belegt (alle 18 Faecher) 0
g6d (Tuer): RE15_FIRE_AOT=0@300#11F0 -> ROOM11E0 (debug.log "[fire-aot] slot=0", "PC loaded room11e0.rdt")
            BP "raum" (Eintritt) Raum 11F0 lampe_zustand=1 ; Testhaken-Ende 11E0 Bild 60 lampe_zustand=1
            entladen.log: EREIGNIS raum gen=4 raum=11F0 | belegt (alle 18 Faecher) 0
```
Einordnung mit der Beleg-Kette des Dossiers selbst: die Grafik ist ein Stueck RDT (RE2 ROOM2130, im
Original Raum-Arena) und gehoert zu demselben Bedienfeld, dessen Ton PANEL2130 die Nachbesserung 2 als
RE2-Raumbank-Satz an jeder Grenze freigibt (Schritt 12, `s_panel_state = 0`) — die Lampen-Grafik
desselben Felds bleibt. (`RE15_FORCE_CUT` erzwingt nur die Sichtbarkeit, die zum Laden fuehrt; im Spiel
laedt derselbe `laden()` ueber Cut 10 von sub16.)

### Code-Zensus der Lade-Caches (was sonst prozesslang lebt — kein Mangel, begruendet)
Alle `pc_read_shared/re15_pc_read_cd/_any/_re2`-Aufrufer gelesen: CDEMD0.EMS (RE1.5 + RE2, ganzes Archiv
einer CD-Datei, Dossier O2), STAGE%u.BIN (je Stage), BGM-Baenke (Original-Cache FUN_80044210, gleicher
Titel laeuft durch @0x80044280), Tuersequenz-/TORSE-Bank (O2, Runde 31; in g3 `tuer_edt` != NULL in 1060
= Ton der gerade durchschrittenen Tuer), globale Seiten (CORE00/TEX.TIM RE1.5+RE2, DEBUG.BIN, YOUDIED,
Titel/Auswahl/Karte/Inventar), Spieler-PLD/PLW. BSS-Puffer und Tuerarchive werden nach dem Dekodieren
freigegeben. Eingebackene `gen/*.inc` (granate/sicherung/irons_tisch_1150 u.a.) sind Teil der exe.
Einziger raumgebundener Lade-Cache ohne Freigabe: LAMPE2130 (Mangel 1).

**Urteil Punkt 2: teilweise** — Masken, Texturen, Modelle (inkl. Elliot), Effekte, Ton-/RE2-Raumbaenke
(ENEMSE, ELEVSE, TUERSE gemessen), Stimmen, RBJ-Datei + Leihe, RDT, BG und Montage-Schnappschuss fallen
nachweislich; die Lampen-Grafik ROOM11F0 bleibt messbar geladen.

## Gate 4 — RE-Gate

Selbst disassembliert/gescannt (`.claude/skills/re15-psx-disasm/scripts/re15_disasm.py` / `re2_disasm.py`,
eigener jal-Wort-Scan ueber die ganze EXE):
```
RE1.5 info/Re1.5/PSX.EXE
  8001b3f8: lui v0,0x800b / 8001b3fc: lw v0,-14472(v0) 0x800ac778 / 8001b404: lw a2,92(v0) /
  8001b40c: beq a2,zero,0x8001b4dc        bytes @0x8001b3fc: 78 c7 42 8c 00 00 00 00 5c 00 46 8c
  80039a00: jal 0x8003ef6c / 80039a08: jal 0x8001b3f8      Voll-Scan jal 0x8001b3f8: genau ['0x80039a08']
  80039738: sw a0,-14468(at) 0x800ac77c / 80039740: sw a0,-14472(at) 0x800ac778   (Arena-Reset)
  800397c0: lw a1,-14472(a1) / 800397e8: jal 0x80013b60                            (RDT ab der Basis)
  Voll-Scan jal 0x800396fc: genau ['0x8001d5ac', '0x8001d988']
RE2 info/re2leon/PSX.EXE
  8004a334: jal 0x80053528 / 8004a33c: jal 0x8005a09c   Voll-Scan: je genau dieser eine Aufrufer
  FUN_8005a09c: 8005a0e0 s0 = 0x800d4c4b / 8005a0ec lb a0,0(s0) / 8005a100 beq a0,-1 /
                8005a108 jal 0x80084ec0 / 8005a110 addiu v0,zero,-1 / 8005a114 sb v0,0(s0)
  FUN_80084ec0 (13 Aufrufer): vab_id < 0x10, Flag 0x800dcc68[id] == 1, jal 0x80084f44(0x800eadf0[id]),
                Flag = 0, Zaehler 0x800eade8 -= 1  = SsVabClose-Form (Sprungziel selbst gelesen)
```
Jede zitierte Stelle enthaelt das Behauptete. Der Unit-Pin `unit_r35_entladen_n2beleg` prueft genau diese
Worte (test_r35_entladen.c). Diff-Suche `deferred|tunable|interim|for now|faithful|plausib|TODO|FIXME|
vorerst|ungefaehr|geschaetzt|approx|hack|sieht richtig` in den +Zeilen von `git diff master -- re15_port`:
kein Treffer. Neue getenv nur `RE15_ENTLADEN_LOG/_SHOT/_SHOT_BILD` (Messhaken). Keine neue
Verhaltenskonstante (N2: nur Freigaben, Faecher, Generationen).

## Gate 5 — Vertrag, Pfade, Tests

`git diff master --name-only`: `analysis/befunde_runde35/I_*`, `engine/src/{enemy_common,entladen_common}.c`,
`include/{re15_enemy,re15_entladen}.h`, `platform/pc/{main.c, src/audio_pc.c, bg_pc.c, cut10f0_pc.c,
elliot_pc.c, entladen_pc.c, render_pc.c, room_pc.c}`, `tests/{integration/test_r35_entladen.cmake,
unit/probe_r35_entladen_karte.c, unit/probes/r35_entladen.cmake, unit/test_r35_entladen.c}`.
Kein `release/`, kein `platform/android/`, kein `shared_assets/PSX/`, keine Edits an den beiden
CMakeLists (Treffer: 0). Keine Bank-9-Bits, Nachrichten-IDs, AOT-Slots, Ereignisse (+Zeilen ohne
`flag_set/scd_event_fire/msg_install/aot_set/work_vars[`).
Umfang gemeinsamer Dateien (numstat gegen master): main.c +39/-51 (N2: +4/-3), audio_pc.c +91/-1 (N2 +41),
cut10f0_pc.c (Datei der Spur K) +11/-2, render_pc.c +70, room_pc.c +34/-1, bg_pc.c +14 — ueber "1-5 Zeilen",
aber im Dossier ("Offen", N2 Umsetzung) offen benannt; wie in Abnahme 1 als Hinweis gewertet, kein Gate-Bruch.
Tests: 4 Unit-Pins (Original-Bytes + Voll-Scans) + integration a..j (echte exe-Kopie, je mit
Vorbedingungs-Pruefung "der Lauf misst X nicht"). Einen Pin fuer Mangel 1 gibt es nicht (kein Fach).

## Hinweise (keine Maengel gegen den Wortlaut)

* **H1 Kinobalken ueber der Charakterwahl nach Tod waehrend einer Szene.** a3/f1/f1b (Tod bei Bild 3200
  in ROOM1170 waehrend des Helipad-Intros): in der Auswahl sind die Zeilen 0..23 und 216..239 schwarz
  (b400: Zeilen 13..17 je 288 Pixel anders, Titelbalken "PLEASE SELECT MAIN CAST" oben abgeschnitten;
  b350..b590 6/14 Bilder anders als nach einem Tod ohne Szene, a1/a4 dagegen 14/14 gleich). Ursache im
  Code: `g_letterbox_level` (fade_common.c:104) wird nur in der Spielschleife getickt
  (`re15_letterbox_tick`, main.c), render_pc.c:1195 zeichnet die Balken in JEDEM Modus. Original:
  einziger Aufrufer des Balken-Zeichners FUN_80021a0c ist `jal` @0x80020f34 in der Schleife @0x80020c10
  (gefolgt von `jal 0x80010000`, Fade `jal 0x80021880`, `jal 0x8002137c`); FUN_80021a0c rampt den Zaehler
  0x800b5568 um +-0x10 je Bild nach `0x800aca3c & 0x10` (@0x80021a10-a80) — nicht belegt habe ich, dass
  das Titelmodul diese Schleife durchlaeuft. Kein PRI und kein Raum-Asset; im normalen Spiel nur bei einem
  Tod bei offenen Balken erreichbar (hier per `RE15_KILL_AT`). Gleiche Fehlerklasse wie der Masken-Befund
  (Overlay des alten Spielstands ueber der Auswahl) — Empfehlung: an "spielende" mit behandeln oder mit
  Beleg begruenden. Bild: Scratchpad `abn_i2/cmp/b400_maskiert_vs_unmaskiert.png`.
* **H2** Der Titel nach JEDEM Tod blendet von WEISS ein (b010 Mittel 204, b030 Mittel 52), der erste
  Titel von Schwarz (Mittel 0). Kein Raum-Asset, nicht bewertet.
* **H3 O6 teilweise geschlossen:** ELEVSE selbst gemessen (g4, gdb `loaded=1 -> 0` an der Grenze 1080 ->
  1040). HINTSE und PANEL2130-Ton laufen durch dieselbe Funktion, nicht einzeln gefahren.
* **H4** main.c-Zweig "keine PL00-Basis" (`else`, ohne `re15_entladen_rbj_halten`) laesst Leons Overlay
  auf dem freigegebenen RBJ-Puffer stehen; nur erreichbar, wenn PL00 nicht ladbar ist.
* **H5** O1 (PSX nicht gebaut) nicht pruefbar; O4 (`heli_md1/pilot_md1`-Parse) bestaetigt totes Erbe.

## Maengel (nummeriert)

1. **Lampen-Grafik des Generator-Bedienfelds ROOM11F0 (LAMPE2130) bleibt nach Raumwechsel und Tod geladen.**
   `platform/pc/src/panel_lampen_pc.c`: `static int s_zustand` (1 = geladen) und `static uint16_t
   s_zelle[2][32*32]` (dekodiert aus `shared_assets/RE2/LAMPE2130.TIM` = ROOM2130.RDT[0x0E398, +4256))
   werden an keiner Grenze zurueckgesetzt; `alles_entladen` (entladen_pc.c) ruft panel_lampen_pc nicht,
   der Zensus hat kein Fach. Gemessen mit gdb: nach Laden in ROOM11F0 Cut 10 steht `s_zustand=1` nach der
   Grenze "spielende" (Tod in 11F0), nach "spielstart" und im neuen Spiel (ROOM1240), sowie nach der Tuer
   AOT 0 11F0 -> 11E0 (Bild 60); die EREIGNIS-Zeilen derselben Laeufe melden alle 18 Faecher 0.
   Naechster Schritt: in `alles_entladen` `s_zustand = 0` setzen (Funktion in panel_lampen_pc.c, Haken
   1 Zeile), Fach in den Zensus, Pin: Lauf g6c/g6d (Befehle oben) muss danach `s_zustand=0` bzw. das neue
   Fach 0 zeigen; Vorbedingung im Pin: Lampe in 11F0 Cut 10 sichtbar (panel.log `cut=10 ... lampe_o=1`)
   und vor der Grenze geladen (neues Fach = 1).

Artefakte (Scratchpad, fluechtig): `abn_i2/{a1_tod1170,a3_tod1170m,a4_tod1030,f1_frisch,f1b_frisch,
f2_frisch_titel,g1_tuer1170_1130,g2_tod1170_ng,g3_kraehe_tuer,g4_fahrstuhl,g4b_fahrstuhl,g5_leihe_tuer,
g6c_lampe_tod,g6d_lampe_tuer}`, Skripte `lauf.sh`, `gdblauf.sh`, `speicher.gdb`, `elev.gdb`, `lampe.gdb`,
`vergleich.py`, `naechstes.py`, Bilder `cmp/`.
