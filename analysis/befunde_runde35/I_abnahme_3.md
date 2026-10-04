# Runde 35 — Spur I "entladen": UNABHAENGIGE ABNAHME 3 (nach Nachbesserung 3)

Baum `.claude/worktrees/r35_entladen`, Zweig `r35/entladen`, Stand `5d7db285` (letzter Code-Commit
`c7d97614`, danach nur Dossier; `git diff c7d97614 HEAD -- re15_port` = leer). merge-base mit master =
`87cc8575` = master HEAD, `git diff master` ist also genau der Spur-I-Anteil. Abnahme am 2026-10-04.
Alles unten habe ich am gebauten Stand SELBST gemessen: eigene exe-Kopie `re15_pc_abn_i3.exe` (nach der
Messung geloescht), eigene Arbeitsverzeichnisse im Scratchpad (`abn_i3/<lauf>`), PATH mit msys64 zuerst,
kein taskkill. Messwege: RAM lesen mit gdb ueber die Symbole der exe (unabhaengig von der Messschiene des
Bau-Agenten), Bildvergleich (SHA1) zwischen Laeufen mit und ohne Tod davor, die Messschiene
`RE15_ENTLADEN_LOG` als dritter Weg, dazu ein eigener Code-Zensus der Lade-Caches.

## Ergebnis

| Punkt (Wortlaut AUFTRAG.md Z. 24) | Urteil |
|---|---|
| 1. "Nachdem ich gestorben bin und new game mache habe ich teilweise noch PRIs von meinen Spielstand davor ... angezeigt" | **erfuellt** |
| 2. "Wenn man tot ist, aber auch wenn man den Raum wechselt sollen saemtliche Assets von den Raeumen davor entladen sein" | **erfuellt** |

Mangel 1 der Abnahme 2 ist behoben. Die Lampen-Grafik ROOM11F0 (LAMPE2130) habe ich ueber den ECHTEN
Panel-AOT (Slot 1 -> sub16) und die Schalter per D-Pad geladen. Mit gdb steht `s_zustand` vor der Grenze
auf 1 (1683 Texel belegt) und nach Schritt (13) auf 0 (0 Texel). Das gilt beim Tod in 11F0 und bei der Tuer
11F0 -> 11E0, und es bleibt so im neuen Spiel (ROOM1240) bzw. im Nachbarraum (ROOM11E0).

Gates: Suite gruen (534/534, Beleg im eigenen build/; `r35_entladen` 17/17 selbst gefahren) · @0x-Gate
haelt (11 Stellen selbst disassembliert bzw. gescannt, alle stimmen) · Pfad-/Vertrags-Gate haelt · Tests
vorhanden und messend. **bestanden = true.** Keine Maengel. Die Hinweise unten sind kein Widerspruch zum
Wortlaut.

## Bau und Tests

```
cd <baum> && bash re15_port/tools/local_build.sh configure && bash re15_port/tools/local_build.sh build
  -> configure OK / Toolchain OK / ninja: no work to do / === LOCAL-BUILD-OK (build)
     exe 2026-10-04 07:40 (nach Commit c7d97614 07:39:43, dem letzten Code-Stand)
Dossier Z. 823: woertlich "=== LOCAL-BUILD-OK (all) — Tests 534/534" (N >= 478, RE15_MIN_TESTS 517)
  -> Suite nicht neu gefahren. Beleg re15_port/build/local_build_ctest.log (08:29, nach dem exe-Bau):
     "100% tests passed, 0 tests failed out of 534", Total 2635.41 sec; integration_boot_bg_pin,
     dark_start_pin, relatch_pin, save_counter_pin, weste_load_pin alle Passed.
ctest --test-dir <baum>/re15_port/build -R r35_entladen -j2 --output-on-failure      (selbst)
  -> 17/17 Passed (unit beleg/gegner/n1beleg/n2beleg/n3beleg, integration a..l; k 3.94 s, l 19.07 s),
     Total 281.17 s
test_r35_entladen.exe n3beleg (direkt) -> "n3beleg: LAMPE2130.TIM = ESP-TIM der RDT ROOM2130 ...", exit 0
```

## Punkt 1 — PRIs des alten Spielstands nach Tod + NEW GAME

Drei Laeufe, je mit eigener exe-Kopie (Skript `abn_i3/lauf.sh`). Gemeinsam: `RE15_NO_INTRO=1
RE15_NOAUDIO=1 RE15_FPS=240 RE15_TITLE_SHOT=t.bmp RE15_TITLE_SHOT_AF=60 RE15_TITLE_CONFIRM_MS=8000
RE15_PSELECT_AUTO=1 RE15_ENTLADEN_SHOT=es RE15_ENTLADEN_SHOT_BILD=10,30,60,100,200,300,400,500,590,800,
1200,1600,2000,2400,2800,3100`. Das sind komponierte Bilder vor dem Present, je Ereignis-Strecke.

| Lauf | Spiel 1 | danach |
|---|---|---|
| p1_tod1030 | `RE15_DEBUG_JUMP=1030@5 RE15_KILL_AT=3200 RE15_BOOT_EXIT_AT=3`: Tod in ROOM1030 mit 22 gezeichneten Masken | Titel+Auswahl, NEW GAME, Montage 1240 -> Story-Tuer -> 1170, Tod, Titel |
| p2_tod1170goto | `RE15_GOTO_ROOM=1170 RE15_KILL_AT=3200 RE15_BOOT_EXIT_AT=3`: Tod in 1170 ohne Masken | dito |
| p0_frisch | `RE15_KILL_AT=3200 RE15_BOOT_EXIT_AT=2`: kein Spiel davor, Montage -> Tuer -> 1170, Tod | Titel+Auswahl |

Messschiene (Vorbedingung des Befunds liegt vor):
```
p1: VORHER spielende gen=3 raum=1030 | belegt pri_masken=22 pri_atlas=1 sld=1 tim=5 gegner=1 esp_bank=1 rdt=1 bg=1
    SUMME seit=raum bilder=3461 bilder_mit_masken=3276          (die 22 ROOM1030-Masken wurden gezeichnet)
    EREIGNIS spielende gen=4 raum=1030 | belegt (alle 19 Faecher) 0
    SUMME seit=spielende bilder=599 bilder_mit_masken=0 bilder_fremde_masken_gezeichnet=0   (Titel+Auswahl)
    SUMME seit=spielstart bilder=2842 bilder_mit_masken=0 bilder_fremd_belegt=0             (neues Spiel 1240)
    SUMME seit=raum bilder=3461 bilder_mit_masken=1856 bilder_fremd_belegt=0                (neues Spiel 1170,
                                                                  nur die eigenen 21 Masken von 1170)
alle 5 eigenen Laeufe (p0/p1/p2/t1/d1): 25 EREIGNIS-Zeilen, davon 0 mit Belegung; 0 BILD-Zeilen
```
Bildvergleich (`abn_i3/vergleich.py`, SHA1 je Bildnummer der Strecke):
```
p1 gen5 (neues Spiel nach Tod in 1030, ROOM1240) vs p0 gen2 (frisches Spiel):   15/15 bytegleich
p1 gen6 (neues Spiel, ROOM1170)                  vs p0 gen3 (frisches Spiel):   16/16 bytegleich
p1 gen4 (Titel+Auswahl nach Tod in 1030, 22 Masken) vs p2 gen4 (Tod ohne Masken): 9/9 bytegleich
p2 gen5 vs p0 gen2: 15/15 bytegleich
```
Das neue Spiel nach einem Tod in einem Raum mit 22 gezeichneten Masken ist Bild fuer Bild gleich einem
Spiel ohne Tod davor. Titel und Auswahl nach diesem Tod sind gleich dem Fall ohne Masken. Keine Maske des
alten Spielstands wird angezeigt. Mechanismus im Code: `re15_entladen_ereignis("spielende")` vor `goto
re_title`. Das leert Maskenliste und Atlas (`re15_render_pc_entladen_raum`, Schritt 1), der PRI-Riegel
leitet je Generation neu ab. (Gesondert gemessen: p1 gen4 vs p0 gen4 6/9. Die drei ungleichen Bilder
b400/b500/b590 unterscheiden sich nur in den Zeilen 13..23 und 216..239. Das sind Kinobalken nach einem Tod
WAEHREND der Helipad-Szene, kein PRI, s. Hinweis H1.)

**Urteil Punkt 1: erfuellt.**

## Punkt 2 — saemtliche Assets der Raeume davor entladen (Tod UND Raumwechsel)

### Mangel 1 der Abnahme 2 (LAMPE2130) — RAM-Messung mit gdb ueber den echten Weg
Skript `abn_i3/lampe3.gdb`, Lauf `abn_i3/gdblauf.sh`. Die Werte liegen in der exe so: `s_zustand =
*(int*)((char*)&s_zelle - 0x20)` und `s_gen = *(unsigned*)((char*)&s_zelle + 0x1000)`. Belegt habe ich das
am Disassembly der exe: `re15_panel_lampen_pc_belegt` liest 0x14081d140 `<s_gen>` und 0x14081c120
`<s_zustand>`, `re15_panel_lampen_pc_entladen` schreibt `movl $0x0` nach 0x14081c120 und ruft `memset(s_zelle,
0, 0x1000)`. `s_zelle_nz` = Zahl der Texel != 0 in `s_zelle`. Haltepunkte: Eintritt `re15_panel_lampen_pc_
dekodieren`, `re15_entladen_ereignis` (Anlass aus rcx), `re15_panel_lampen_pc_entladen`, die Anweisung
nach dem Aufruf in `alles_entladen` (+182, Ruecksprung aus Schritt 13) und `re15_testhaken_ende`.

**t1 — Tod, echter Panel-AOT:** `RE15_DEBUG_JUMP=11F0@240 RE15_FIRE_AOT=1@40#11F0` (Slot 1 = Panel
untersuchen -> sub16). Dazu Meldungen + "Ja" und die Schalter 3,1,5 per D-Pad/Quadrat (`RE15_INPUT_SCRIPT`
wie Spur C S3/S5, Basis spiel, Start 320), `RE15_KILL_AT=800 RE15_BOOT_EXIT_AT=2`, Titel automatisch:
```
debug.log: "[fire-aot] slot=1 at F40 (Raum 11F0)"; panel.log: 375 Bilder "raum=11F0 cut=10 ... maske=015
           ... lampe_o=1" (F686..F1060)
=== BP dekodieren-eintritt               raum=11F0 gen=3 s_zustand=-1 s_gen=3 s_zelle_nz=0
=== BP ereignis-eintritt anlass=spielende raum=11F0 gen=3 s_zustand=1  s_gen=3 s_zelle_nz=1683
=== BP freigabe-eintritt                 raum=11F0 gen=4 s_zustand=1  s_gen=3 s_zelle_nz=1683
=== BP alles_entladen-nach-schritt13     raum=11F0 gen=4 s_zustand=0  s_gen=3 s_zelle_nz=0
=== BP ereignis-eintritt anlass=spielstart raum=11F0 gen=4 s_zustand=0 s_zelle_nz=0
=== BP testhaken-ende (neues Spiel)      raum=1240 gen=5 s_zustand=0  s_gen=3 s_zelle_nz=0
entladen.log: VORHER spielende gen=3 raum=11F0 | ... msk=1 tim=18 gegner=2 esp_bank=1 rdt=1 bg=1 ... lampe=1
              EREIGNIS spielende gen=4 | belegt (alle 19 Faecher) 0 ; EREIGNIS spielstart gen=5 alle 0
```
**d1 — Raumwechsel, echte Tuer:** Panel per `RE15_SUBSTART=16@40#11F0` (sub16 = Evt_exec-Ziel des
Panel-AOT), dieselben Schalter 3,1,5 per D-Pad, dann die Tuer `RE15_FIRE_AOT=0@720#11F0` (main00 Slot 0
-> ROOM11E0, derselbe Pfad wie Hineinlaufen: `re15_aot_fire_slot`), `RE15_EXIT_AT=150#11E0`:
```
debug.log: "[substart] sub_scd[16] at F40", "[fire-aot] slot=0 at F720 (Raum 11F0)",
           "[room] PC loaded room11e0.rdt (209684 bytes)"; panel.log: 35 Bilder lampe_o=1 (F686..F720)
=== BP dekodieren-eintritt           raum=11F0 gen=3 s_zustand=-1 s_gen=3 s_zelle_nz=0
=== BP ereignis-eintritt anlass=raum raum=11F0 gen=3 s_zustand=1  s_gen=3 s_zelle_nz=1683
=== BP freigabe-eintritt             raum=11F0 gen=4 s_zustand=1  s_gen=3 s_zelle_nz=1683
=== BP alles_entladen-nach-schritt13 raum=11F0 gen=4 s_zustand=0  s_gen=3 s_zelle_nz=0
=== BP testhaken-ende                raum=11E0 gen=4 s_zustand=0  s_gen=3 s_zelle_nz=0
entladen.log: VORHER raum gen=3 raum=11F0 | ... msk=1 tim=20 gegner=2 esp_bank=1 rdt=1 bg=1 ... lampe=1
              EREIGNIS raum gen=4 raum=11F0 | belegt (alle 19 Faecher) 0 ; keine BILD-Zeile in 150 Bildern 11E0
Bilder (Lampenrechteck x212..233/y67..85, Mittel RGB):
  gen3 b700 ROOM11F0 (73.6, 148.3, 72.5) = obere Lampe gruen    abn_i3/d1_b700_11F0_lampe_an.png
  gen3 b060 ROOM11F0 vor dem Panel (8.1, 8.1, 6.0)
  gen4 b060/b150 ROOM11E0 (42.4, 40.2, 30.7) = Hintergrund 11E0, kein Gruen   abn_i3/d1_gen4_b060_11E0.png
```
Der Fix erklaert den Befund der Abnahme 2. Vorher (Abnahme 2, g6c/g6d) las gdb `s_zustand=1` nach den
Grenzen "spielende"/"spielstart"/"raum". Jetzt steht in beiden Laeufen die Vorbedingung im Protokoll (Lampe
sichtbar, `s_zustand=1`, 1683 Texel). Direkt nach Schritt (13) steht `s_zustand=0` mit 0 Texeln, und das
bleibt bis ins neue Spiel bzw. den Nachbarraum. Wiederladen nach einer Freigabe ist ebenfalls belegt: in
beiden Laeufen lief die Freigabe an "raum" gen 3 (`freigabe-eintritt raum=1240 gen=3 s_zustand=0`) VOR dem
ersten Laden, danach `dekodieren` mit `s_zustand -1 -> 1` und Gruen im Bild. Das ist derselbe Zustand
(`s_zustand=0`, `s_zelle`=0), den die Freigabe nach dem Laden herstellt.

### Die uebrigen Asset-Klassen (Masken, TIM, Gegner, Effekte, Ton, RDT, BG, Stimmen, Elliot, RBJ, RE2-Raumbaenke)
Seit Abnahme 2 ist am Code nur Schritt (13) + Fach `lampe` dazugekommen (`git diff 32239df5 HEAD --
re15_port`: entladen_pc.c +14/-3, panel_lampen_pc.c +10, re15_entladen.h +13, Tests). Die Freigaben
(1)..(12) sind unveraendert und in Abnahme 2 per gdb gemessen (RBJ, Leihe, ELEVSE, ENEMSE). In meinem Lauf
fahren die Pins a..j gruen. In meinen fuenf eigenen Laeufen ist jede EREIGNIS-Zeile leer (25/25), obwohl
die VORHER-Zeilen Masken, TIM-Slots, Gegnerbaenke, Raum-ESP, RDT, BG, RBJ und Elliot belegt zeigen.

### Eigener Code-Zensus (was die Messschiene NICHT zaehlt)
Alle Aufrufer von `re15_pc_read_{re2,cd,any,shared}` / `pc_read_shared` in platform/pc und die
statischen Puffer der mit master neu gekommenen Dateien (cut10f0_pc.c, fx_plattform_pc.c, audio_pc.c
`s_re2_arms`, engine granate_r35/werfer_r35/irons_tod_1150/cut_10f0/tuer1060_1040 ohne malloc) habe ich
gelesen. Global bzw. begruendet bleiben: CORE00/TEX.TIM (RE1.5+RE2), DEBUG.BIN, YOUDIED, Titel/Auswahl/
Konfig, ITEMALL/ITPS/MAP-PIX, Spieler-PLD/PLW, RE2-Waffen-Tonbaenke (`s_re2_arms`), CDEMD0.EMS,
STAGE%u.BIN, BGM-Cache, die Tuersequenz-Tonbank. Die Tuerarchive selbst gibt door_scene_pc.c nach dem
Lauf frei (`free(arch.datei)` Z. 329/460). Zur Tuer-Tonbank (O2) habe ich die Begruendung nachgelesen:
RE2 FUN_800597a4 schaltet nur Stimmen ab, deren SPU-Adresse minus 0x14441 (`ori s3,0xbbbf` @0x800597c0,
`addu v0,v0,s3` @0x80059800) unter 0x2980e liegt (`sltu` @0x80059804, `jal 0x80079498` @0x80059810). Das
ist der Bereich 0x14441..0x3DC4F, die Tuerbank @0x3DC50 liegt ausserhalb. `fx_plattform_pc.c s_licht_kopie`
ist Bild-Zustand (je Bild gesetzt und zurueckgeschrieben), kein Asset. `hebetisch_cursor_pc.c` parst nur
eingebackene Bytes. Weitere raumgebundene Caches ohne Freigabe habe ich nicht gefunden.

**Urteil Punkt 2: erfuellt.** Beim Tod und beim Raumwechsel fallen nachweislich alle raumgebundenen
Caches, jetzt auch die Lampen-Grafik ROOM11F0 (gdb, echter Weg).

## Gate 4 — RE-Gate

Selbst disassembliert (`.claude/skills/re15-psx-disasm/scripts/re2_disasm.py` / `re15_disasm.py`) und mit
eigenem jal-Voll-Scan (`abn_i3/../jal_i3.py`, alle Woerter des Textsegments):
```
RE2 info/re2leon/PSX.EXE
  80049e48: addiu sp,sp,-64 / 80049e50: lui s0,0x800d / 80049e54: addiu s0,s0,-15896 (0x800cc1e8)
  8004a178: lw a1,8508(s0)   (0x800cc1e8+0x213c = 0x800ce324)  / 8004a1c4: jal 0x80012fb8
  8004a2ec: jal 0x8001bba4   Voll-Scan jal 0x8001bba4: genau ['0x8004a2ec']
  8001bc78: lw v0,-7388(v0) 0x800ce324 / 8001bc80: lw a0,92(v0) / 8001bc84: lw a1,88(v0) / 8001bc88: jal 0x8001bd38
                             Voll-Scan jal 0x8001bd38: genau ['0x8001bc88']
  FUN_8001bd38 (Sprungziel gelesen): tim = basis + *(ende-4) je Eintrag, jal 0x80092f74/0x80092f84
     (OpenTIM/ReadTIM), jal 0x80090664 @0x8001be08 + @0x8001be3c (LoadImage prect/crect), jal 0x800903a0
     (DrawSync); RE2_Quellcode_V2/FUN_8001bd38.c bestaetigt OpenTIM/ReadTIM/LoadImage.
  FUN_80049e48: einziger Aufrufer jal @0x80026e1c (Raumwechsel-Schleife).
  FUN_800597a4 (O2-Begruendung): Bereichstest 0x14441..0x3DC4F, s.o.
RE2 info/re2leon/PL0/RDT/ROOM2130.RDT: [0x58] = 0x0E398, [0x5C] = 0x0F458;
  RDT[0x0E398, +4256) == shared_assets/RE2/LAMPE2130.TIM (4256 B, Kopf 10000000 08000000): True
RE1.5 info/Re1.5/PSX.EXE
  80039738: sw a0,-14468(at) 0x800ac77c / 80039740: sw a0,-14472(at) 0x800ac778   (Arena-Reset)
  800397c0: lw a1,-14472(a1) / 800397e8: jal 0x80013b60   (RDT ab der Arena-Basis)
  Voll-Scan jal 0x800396fc: genau ['0x8001d5ac', '0x8001d988']
  H1: 80020f34: jal 0x80021a0c / 80020f3c: jal 0x80010000; Voll-Scan jal 0x80021a0c: genau ['0x80020f34'],
      jal 0x80020bb0: genau ['0x800544e8']
```
Jede zitierte Stelle enthaelt das Behauptete. Der Unit-Pin `unit_r35_entladen_n3beleg` prueft genau diese
Worte und den Datei-Schnitt (test_r35_entladen.c `test_n3beleg`). Die Kette traegt die Einordnung "Lebensdauer
= ein Raum". Die Lampen-Kunst ist im RE2-Original ESP-TIM der Raum-RDT und wird bei jedem Raumladen aus der
NEUEN RDT hochgeladen. Der Port-Cache (`s_zelle`) faellt deshalb an jeder Grenze, wie der PANEL2130-Ton
desselben Bedienfelds (Schritt 12).
Diff-Suche `deferred|tunable|interim|for now|faithful|plausib|TODO|FIXME|vorerst|ungefaehr|geschaetzt|
approx|hack|sieht richtig|platzhalter` in den +Zeilen von `git diff master -- re15_port`: kein Treffer.
Neue getenv nur `RE15_ENTLADEN_LOG` (2x), `_SHOT`, `_SHOT_BILD` (Messhaken). N3 bringt keine neue
Verhaltenskonstante, nur Freigabe, Fach und Generation. Die Commit-Message `5d7db285` traegt alle Adressen.

## Gate 5 — Vertrag, Pfade, Tests

`git diff master --name-only`: `analysis/befunde_runde35/I_*` und `I_entladen_bilder/*`,
`engine/src/{enemy_common,entladen_common}.c`, `include/{re15_enemy,re15_entladen}.h`,
`platform/pc/{main.c, src/audio_pc.c, bg_pc.c, cut10f0_pc.c, elliot_pc.c, entladen_pc.c, panel_lampen_pc.c,
render_pc.c, room_pc.c}`, `tests/{integration/test_r35_entladen.cmake, unit/probe_r35_entladen_karte.c,
unit/probes/r35_entladen.cmake, unit/test_r35_entladen.c}`. Verbotene Pfade (`release/`,
`platform/android/`, `shared_assets/PSX/`, die beiden Test-CMakeLists): 0 Treffer. Bank-9-Bits,
Nachrichten-IDs, AOT-Slots und Ereignisse: keine (+Zeilen ohne `flag_set/scd_event_fire/msg_install/
aot_set/work_vars[`). Gemeinsame Dateien (numstat gegen master): main.c +39/-51, audio_pc.c +91/-1,
cut10f0_pc.c +11/-2, panel_lampen_pc.c +10 (N3), render_pc.c +70, room_pc.c +34/-1, bg_pc.c +14. Das liegt
ueber der "1-5 Zeilen"-Regel, ist im Dossier aber offen benannt (OFFEN N3). Wie in Abnahme 1/2 werte ich es
als Hinweis, nicht als Gate-Bruch. Tests: 5 Unit-Pins (Original-Bytes + Voll-Scans) und integration a..l
(echte exe-Kopie, je mit Vorbedingungs-Pruefung). N3 neu: `n3beleg`, `k` (Tod), `l` (Tuer). Beide pruefen
die Sichtbarkeit (panel.log `lampe_o=1`) und `lampe=1` VOR der Grenze. Laut Dossier schlagen sie ohne
Schritt (13) fehl. Meine gdb-Laeufe bestaetigen das Feld unabhaengig von der Messschiene.

## Hinweise (keine Maengel gegen den Wortlaut)

* **H1 / O7 Kinobalken ueber der Charakterwahl nach einem Tod waehrend einer Szene (weiter vorhanden).**
  p0 gen4 b400 (Tod bei Bild 3200 in ROOM1170 waehrend des Helipad-Intros) gegen p1 gen4 b400 (Tod in
  1030): verschieden nur in den Zeilen 13..23 und 216..239 (6 694 Pixel). Der Titelbalken "PLEASE SELECT
  MAIN CAST" ist oben abgeschnitten, unten liegt ein Balken (`abn_i3/p0_gen4_b400_auswahl_balken.png` gegen
  `abn_i3/p1_gen4_b400_auswahl.png`). Das ist weder PRI noch Raum-Asset. Die RE-Spur des Bau-Agenten habe
  ich geprueft (@0x80020f34 einziger Aufrufer von FUN_80021a0c, vor @0x80020f3c), O7 steht mit Messweg im
  Dossier. Fuer den Nutzer bleibt es ein sichtbarer Rest des alten Spielstands ueber der Auswahl. Empfehlung
  wie in Abnahme 2: den O7-Messweg (Savestate mit Tod waehrend einer Szene, 0x800b5568/0x800aca38 im Titel
  lesen) als naechsten Schritt fahren.
* **H2** Titel nach dem 1. Tod: Einblenden von WEISS (gen4 b010 Mittel 204.9) in p0/p1/p2 gleich. Nach dem
  2. Tod (gen7, 61 Bilder bis BOOT_EXIT_AT=3) von SCHWARZ (b010 Mittel 0.0), in p1 und p2 gleich, also
  unabhaengig vom Raum des ersten Spiels. Kein Raum-Asset, nicht bewertet.
* **H3** p1 gen7 gegen p2 gen7 b010/b030/b060: um genau eine Blendstufe (8 Werte) verschieden, nur im
  Logobereich x112..205 (Bilder fast schwarz). Ursache ist der Titel-Takt nach Wanduhr (main.c:3498ff "so
  viele Durchgaenge ... wie seit dem letzten Bild faellig"), kein Rauminhalt. Ich habe die Bilder deshalb
  nicht als Ungleichheit gewertet.
* **H4 Testqualitaet:** `test_n3beleg` (wie `test_beleg/n1beleg/n2beleg`) endet bei fehlender Datei mit
  `return 1`, ohne `s_fehler` zu erhoehen. `datei_lesen` druckt dann "FAIL: ... nicht lesbar", der Pin endet
  aber mit "OK" und exit 0. In einem Baum ohne `info/re2leon` waere der Pin still gruen. Hier liegen die
  Dateien vor, die Pruefungen laufen (Ausgabe oben).
* **H5** O1 (PSX nicht gebaut) nicht pruefbar. O4 (`heli_md1/pilot_md1`-Parse, totes Erbe) und O6
  (HINTSE/PANEL2130-Ton nicht einzeln gefahren) bleiben unveraendert benannt.

## Maengel (nummeriert)

Keine.

Artefakte (Scratchpad, fluechtig): `abn_i3/{t1_panelaot_tod,d1_sub16_tuer,p0_frisch,p1_tod1030,
p2_tod1170goto}` (gdb.txt, debug.log, entladen.log, panel.log, es_*.ppm), Skripte `lampe3.gdb`,
`gdblauf.sh`, `laeufe_lampe.sh`, `lauf.sh`, `vergleich.py`, `diffrows.py`, Bilder `*.png`; Ctest-Log
`abn_i3_ctest.log`.
