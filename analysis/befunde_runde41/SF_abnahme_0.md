# Runde 41 - Spur SF (Sichel-Form V7 als Spiel-Asset): unabhaengige Abnahme 0 (2026-11-08)

## Ergebnis: BESTANDEN

**Kein blockierender Befund.** Es gibt 0 kleine Befunde und 7 Hinweise (H1-H7, Abschnitt 8).

- Das Spiel zeigt genau die freigegebene V7:
  - Der eigene Lauf mit dem Spiel-Asset (EMD "-", ohne Overlay) ergibt 901 von 901 Bildern (F200-F1100) bytegleich mit dem freigegebenen Lauf `sfv4/r/V7_szene`. Das gilt auch fuer die Bilder 225-450 (226 von 226). `state.log` ist bytegleich.
  - Im echten Fenster (gdigrab, Direct3D) sind 20 von 20 Szenenbildern pixelgleich mit dem jeweils passenden freigegebenen Bild.
- Asset, Pin und Paket-Tor sind konsistent: sha256 `319bbd17...` in allen drei. Das nachgebaute Tor laesst V7 durch und haelt das alte Asset an. `v7_kette.sh` laeuft selbst mit Exit 0.
- Alle Tests sind gruen, alle selbst gebaut und gefahren:
  - Spur-Gruppe: 11 von 11.
  - Dossier-Gruppe: 20 von 20.
  - `^unit_` mit -j 6: 1013 von 1013.
- RE-Gate: 6 Zitate habe ich an den Bytes geprueft (siehe Abschnitt 4), alle stimmen. Gestaltungswerte sind als PORT-WAHL oder Gestaltung markiert. Es gibt keinen neuen Env-Schalter fuer die Gestaltung. Die neuen Messhaken tun nur etwas, wenn ihre Env-Variable gesetzt ist.
- Keine Regression ausserhalb von Hangar und Labor:
  - Die Aenderung in `re15_damage.c` ist identisch mit master und r41/integration.
  - Der Rohr-Tausch gilt nur noch fuer 16 Knochen. Das trifft das Capcom-EM30 (16 Knochen, gemessen) nicht. Die Bahnsteig-Form (27 Knochen) wird dadurch so gezeichnet wie in r41/integration.
- Diese Abnahme liegt nach dem letzten Code-Commit (Abschnitt 1).

**Arbeitsordner:** `C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/abn0/`. Unten steht er als `<abn0>`.

**Belege:** `analysis/befunde_runde41/SF_belege/abn0/` (Bilder roh).

**Gepruefter Stand:**
- Zweig `r41/sichel_nb2`, Worktree `C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel`.
- HEAD `6a25db27bfa304f8e48ed3cd4a405454d067924a` (2026-11-08 20:59:15).
- `git status` war vor diesem Bericht leer.

Geaendert habe ich nur diesen Bericht und `SF_belege/abn0/`. Nichts in `Version_80%/`, `pri/`, `synchro/` und `BAHNSTEIG_FORM`. Ich habe keine exe per Name beendet.

## 1. Zeitpunkt

```
git -C .../r41_sichel log -1 --format="%H %ci" -- re15_port/engine re15_port/include re15_port/platform re15_port/tests re15_port/tools release
b1d2e864bc3f94ea6d06d7d8fc384ea13284d8f1 2026-11-08 20:49:15 +0100
```

- Der Bau dieser Abnahme lief ab 21:0x.
- Die Tests endeten um ca. 21:45.
- Der Bericht entstand am 2026-11-08 um 22:06 (+0100).

Die Abnahme liegt also nach dem letzten Code-Commit b1d2e864b. Die Forderung aus `reai-v2-abnahme-nach-letztem-code-commit` ist damit erfuellt.

Danach kam nur der Doku-Commit 6a25db27b (Dossier).

## 2. Asset, Pin und Tor

### 2.1 Hashes (selbst gemessen)

| Gegenstand | sha256 |
|---|---|
| Arbeitsbaum `re15_port/shared_assets/PSX/EMD/EM30_HANGAR.EMD` (268948 Byte) | `319bbd17f10ba0cdd703861c6d844a00fb414d82b513e77057bf0ff80fda65f6` |
| Committet (`git show HEAD:...EM30_HANGAR.EMD`) | `319bbd17...` |
| Pin `release/em30_hangar.sha256` | `319bbd17...` |
| Freigegebene Vorlage `scratchpad/sfv4/emd/EM30_V7.EMD` | `319bbd17...` |
| Overlay des freigegebenen Laufs `sfv4/r/V7_szene/cd/EMD/EM30_HANGAR.EMD` | `319bbd17...` |

### 2.2 Paket-Tor

Das Tor steht in `release/make_package.sh` Z. 309-324. Es prueft:
- dass die Datei nicht leer ist (`-s`),
- dass Quellbaum und Paket gleich sind (`cmp`),
- und `sha256sum` gegen Zeile 1, Feld 1 des Pins `$HERE/em30_hangar.sha256`.

Ich habe den Tor-Block nachgebaut (`SF_belege/abn0/tor_nachbau.sh`, Logik Zeile fuer Zeile wie Z. 312-323):

| Fall | Ergebnis |
|---|---|
| V7 im Paket und im Quellbaum | `TOR-OK 319bbd17...`, Exit 0 |
| Alter Stand ed254a7fe (Referenz 2) in Paket und Quellbaum (cmp gleich) | `DIE: pin cfd1ac42... != 319bbd17...`, Exit 1 |

Der Pin haelt also auch dann, wenn cmp nichts merkt.

### 2.3 Eigener EMD/TIM-Leser

Leser: `SF_belege/abn0/emdlesen.py`, Ausgabe `emdlesen_v7.txt`.

- **Verzeichnis:** 9 Eintraege.
- **Bank 0:** EDD 21 Clips, EMR 23 Knochen.
- **MD1:** object_count 46, also **23 Meshes / 23 Knochen / 21 Clips**.
- **TIM:** 8 bpp, Bild 384 x 256, CLUT bei (0, 480) mit 256 x **13** Zeilen (3328 Eintraege).
- **Port-Texturhoehe:** 256 x 13 = **3328**. Das ist hoechstens 4096, nach der Formel aus `render_pc.c re15_render_pc_upload_tim_slot`: `tex_h = height * clut_entries/256`.
- **CLUT-Indizes:** Ueber alle 943 Flaechen ist der Index `((clut>>6)&0x1ff) - 480` 0 Mal ungueltig.
  - Das ist wichtig, weil `render_pc.c` einen ungueltigen Index still auf 0 setzt.
  - Histogramm (Index: Flaechen): 0: 275, 1: 88, 2: 62, 4: 50, 6: 91, 7: 242, 8: 34, 9: 43, 10: 34, 11: 6, 12: 18.
- **Belegung je Mesh** (deckt sich mit den Riegeln des Bauers):

| Mesh | Teil | CLUT-Belegung |
|---|---|---|
| 9 | Kopf | 91 auf 486 |
| 8 | Brust | 34 auf 488 |
| 13 | Oberarm | 43 auf 489 |
| 12 | Hand | 34 auf 490, 6 auf 491, 18 auf 492 |
| 16-22 | Schwanz | alle auf 487 |

### 2.4 v7_kette.sh selbst gefahren

Lauf in einem eigenen Zielordner `<abn0>/kette`, Protokoll `SF_belege/abn0/kette_lauf.txt`:
- **Exit 0** in 20 s.
- Alle 6 Stufen stimmen mit den Soll-Hashes ueberein: a1bc9309 / 38562ced / 99703bbd / 784c8f05 / 9e0e58ef / 319bbd17.
- Abschluss: `V7-KETTE-OK sha256 319bbd17f10ba0cdd703861c6d844a00fb414d82b513e77057bf0ff80fda65f6`.

Nachher:
- `git status` ist leer.
- Die Verzeichnis-Verbindungen sind geloest. `ZIEL/baum` enthaelt nur noch `re15_port/`, und `shared_assets/PSX` darin nur `EMD`.
- Arbeitsbaum `analysis/` (109 Eintraege) und `shared_assets/PSX` (18) sind unversehrt.

## 3. Code-Delta

Gelesen mit `git diff 789f6e681..HEAD -- re15_port/engine re15_port/include re15_port/platform re15_port/tests release`: 14 Dateien, +2493/-229.

### 3.1 Wen treffen die Aenderungen?

**`birkin_formen.c`**
- `re15_bf_emd_datei` liefert `EMD/EM30_HANGAR.EMD` nur in zwei Faellen:
  - Typ 0x30 und `re15_bf_hangar_aktiv()` (Raum & 0xFFFE == 0x50A0),
  - oder `re15_bf_labor_aktiv()` (0x50E0/0x50F0, also 50E0/50E1/50F0/50F1).
- Mit `RE15_BIRKIN_FORM=hangar` kommt dieselbe Datei ebenfalls. Diese Env-Variable gab es schon vorher.
- 3070/5080/5090 bekommen weiter NULL, also das Capcom-EM30.
- Die Hangar-Szene laeuft nur unter `re15_bf_hangar_aktiv()` (`re15_bf_raumtick`).
- `re15_bf_szene_zeigt` und `re15_bf_szene_effekte` liefern nur bei `s_h.riegel && s_h.raum && Cut == 12` einen Wert ungleich 0.

**`schwanz_r41.c`**
- `re15_schwanz_r41_pose` wird in `main.c` fuer jeden NPC aufgerufen.
- Die Funktion bricht aber ab, ausser bei Typ 0x30 und `kette_ok`. `kette_ok` heisst: bone_count == 23, Knochen 16 am Knochen 8, Kette 17-22.
- Das passt nur auf EM30_HANGAR. Das Capcom-EM30 hat 16 Knochen, die Bahnsteig-Form 27.

**`main.c` Rohr-Tausch**
- Die Bedingung lautet jetzt `npc_bones == 16` statt `> 15`.
- Das Capcom-EM30 hat 16 Knochen und 17 Meshes, gemessen aus CDEMD0.EMS Blob EM30 ueber `birkin_formen.lade_em30` (`em30_capcom.txt`). Fuer das Original-Modell aendert sich damit nichts.
- Die Bahnsteig-Form G4 (27 Knochen) zeichnet seit diesem Zweig kein Rohr mehr an Teil 15. In der merge-base tat sie das noch.
- r41/integration hat dasselbe Ergebnis auf anderem Weg: `!re15_bb_bank_da(npc)`, `main.c` Z. 10782-10785 in r41/integration. Also keine Regression. Zum Zusammenfuehren siehe H5.

**`re15_damage.c` (`re15_enemy_fx19700`)**
- Neu: Sub = das ganze Byte `(a0>>16)&0xff`.
- Der Funktionsblock ist zeichengleich mit master und r41/integration. Gemessen mit `diff` des Blocks: IDENTISCH. Die Aenderung stammt aus Spur S und steht schon auf master.
- Sie trifft das Spinnenblut (Sub 8). Die Zombie-Codes haben Sub 0 und bleiben gleich.

**`re15_esp.h`**
- Neues Feld `abr_port`.
- `re15_esp_fx_spawn_ex` setzt den Platz auf 0 (`memset`, `re15_esp.c:349`).
- Gesetzt wird das Feld nur in `birkin_formen.c` (gruenes Blut der Szene, Z. 728/739).
- Ausgewertet wird es in `main.c` Z. 528 nur bei einem Wert ungleich 0. Alle anderen Effekte laufen den alten Weg.

**Env-Variablen und Haken**
- Neue Env-Variablen sind nur Messhaken: `RE15_HANGAR_LOG`, `RE15_GRIFF_LOG`, `RE15_BEIN_LOG`. Sie werden je einmal per `getenv` gelesen, nur mit gesetzter Variable wird geschrieben, und sie aendern kein Verhalten.
- `re15_bf_hangar_vergessen` wird nur von Tests aufgerufen (kein Aufrufer ausserhalb von `tests/`).
- `re15_schwanz_r41_ziel_gelenk` wird nur in Messzeilen unter `RE15_HANGAR_LOG` verwendet.

**Assets**
- Unter `shared_assets` hat sich nur `EMD/EM30_HANGAR.EMD` geaendert.
- Keine Datei in `Version_80%/`, `pri/`, `synchro/` ist geaendert, nichts mit "bahnsteig" im Namen, kein `birkin_bahnsteig.c` und kein `enemy_ai_common.c`.

### 3.2 Gestaltung

Die Zahlen der Szene und des Schwanzes sind als "Videomessung oder Gestaltung, KEIN Auslieferungscode" bzw. als "PORT-WAHL, keine Original-Konstante" gekennzeichnet. Das gilt fuer `birkin_formen.c` (Kopf des Hangar-Blocks) und `schwanz_r41.c` (Kopf, alle SW_*).

Wo es einen Nutzer-Wortlaut gibt, ist er zitiert, z. B. bei `k_schwanz_gewicht_v4`: "Ich will EXAKT diese Umsetzung". Ich habe keine erfundene @0x-Adresse an einem Gestaltungswert gefunden.

## 4. RE-Gate: Stichprobe der Original-Zitate

Gegen `info/Re1.5/PSX.EXE` und `info/Re1.5/PSX/BIN/STAGE*.BIN` geprueft. Werkzeug: eigenes `mipsdis.py` (capstone; STAGE-BIN ohne Kopf @0x80100000, EXE mit Kopf 0x800). Belege `disasm_*.txt`.

| # | Zitat im Delta | Bytes / Instruktion | Urteil |
|---|---|---|---|
| 1 | `re15_damage.c`: Sub = ganzes Byte, @0x8001970c / 80019730 / 80019734 / 8001973c / 80019754 / 800197d8 / 8001987c-88 | 8001970c `02140400` srl v0,a0,16; 80019730 `ff004f30` andi t7,v0,0xff; 80019734 `0700e231` andi v0,t7,7; 8001973c `c2180f00` srl v1,t7,3; 80019754 `80810300` sll s0,v1,6; 800197d8 `71000fa1` sb t7,0x71(t0); 8001987c `0400a295` lhu v0,4(t5) / 80019884 addu v0,v0,s0 / 80019888 `320002a5` sh v0,0x32(t0) | stimmt Wort fuer Wort |
| 2 | Birkin 0x0B GRAB-THROW ACT[5] @0x80117d30, Wurf bei Bild 0x2C | STAGE3: 80117d30 `d8ffbd27` addiu sp,-0x28 (Funktionsanfang); 80117df4 `0b000234` ori v0,zero,0xb + 80117df8 `940062a0` sb v0,0x94(v1); 80117e80 lbu v1,0x95(a0) + 80117e84 `2c000234` ori v0,zero,0x2c + bne | stimmt (STAGE3) |
| 3 | Spider-Man 0x16 Sprung B[11] @0x80113560 `+0x94 = 0x16` | STAGE4: 80113560 = Fallbeginn hinter `j 0x80113a68` @0x80113558; 80113594 `16000234` ori v0,zero,0x16 + 80113598 `940062a0` sb v0,0x94(v1) | stimmt (STAGE4) |
| 4 | Spider-Man HURT Clip 7 @0x801148e8 | STAGE4: 801148e8 `07000234` ori v0,zero,7 + 801148ec `940062a0` sb v0,0x94(v1) | stimmt |
| 5 | Birkin WALK Clip 1 @0x80117220 | STAGE3: 80117200 `01000334` ori v1,zero,1 (Verzoegerungsslot vor beqz -> 0x80117220); 80117230 `940043a0` sb v1,0x94(v0) | stimmt |
| 6 | Birkin INIT Clip 0 @0x8011683c | STAGE3: 8011683c `940040a0` sb zero,0x94(v0); 8011684c sb zero,0x95(v0) | stimmt |

Keine Ungereimtheit. Zur fehlenden Overlay-Angabe in den Kommentaren siehe H6.

## 5. Tests

### 5.1 Bau

`RE15_BUILD_DIR=<baum>/re15_port/build bash re15_port/tools/local_build.sh configure` und danach `... build`:
- `=== LOCAL-BUILD-OK (configure)`
- `=== LOCAL-BUILD-OK (build)` (`SF_belege/abn0/bau.txt`)

Die exe ist aktuell: `re15_pc.exe` vom 2026-11-07 12:37:35, sha1 `3e249135426839c604ba3fd85fb885978d9d68c0`. `find re15_port/engine re15_port/include re15_port/platform -newer re15_pc.exe` liefert nichts. Der letzte Commit an engine/include/platform ist 6488773d2 (nur Kommentar-Text); ninja hatte nichts zu tun.

Waehrend der Tests liefen keine fremden Bau- oder Testprozesse (Prozessliste vorher leer).

### 5.2 Summenzeilen (woertlich, Logs in SF_belege/abn0/)

**Gruppe A** (`ctest_A.txt`):
```
ctest -R "unit_birkin|unit_schwanz_r41|integration_r41_sichel_form|integration_r41_birkin_pistole|unit_r41_birkin|hangar" --timeout 900
100% tests passed, 0 tests failed out of 11
Total Test time (real) = 450.45 sec
```
Gruen sind:
- unit_birkin_ai, unit_birkin_formen, unit_birkin_bahnsteig,
- unit_r41_birkin_wand_rein, unit_r41_birkin_wand_g5, unit_r41_birkin_wand_griff (124,38 s),
- unit_birkin_szenen, unit_schwanz_r41,
- integration_r41_birkin_pistole (240,60 s), integration_r41_sichel_form (64,20 s),
- unit_birkin_opfer.

**Gruppe B**: der Ausdruck aus SF_sichel.md (`ctest_B.txt`):
```
ctest -R "birkin|hangar|50a|spiderman|formen|r38_birkin|room50|sichel" --timeout 900
100% tests passed, 0 tests failed out of 20
Total Test time (real) = 515.52 sec
```

**Gruppe C** (`ctest_C_summe.txt`):
```
ctest -j 6 --timeout 600 -R "^unit_" -E "^(unit_r38_releaseskript_|unit_r30_android_|unit_r35_android_|unit_r39_titelversion_cmake)"
100% tests passed, 0 tests failed out of 1013
Total Test time (real) = 290.46 sec
```

H_hangar.md nennt keinen eigenen -R-Ausdruck.

Kein Test war rot, also musste ich nichts nachfahren.

**Direkter Lauf, um leere Tests auszuschliessen:**
- `test_birkin_formen.exe` meldet "23 Meshes, 23 Knochen, 21 Clips ... 11 Teile umtexturiert", "Port-Texturhoehe 3328", "30 blaue grau" und "test_birkin_formen: OK".
- `test_schwanz_r41.exe` meldet "Nachlauf max 189", "Szenenziel getroffen auf 380 (ohne Ziel 1400)" und "test_schwanz_r41: OK".

## 6. Spiel

### 6.1 Hangar-Szene, deterministisch

Befehl:
```
FD=200-1100/1 bash re15_port/tools/r41_sichel/v4/lauf_v4.sh <abn0>/szene - 1110
```
- EMD "-" heisst: Spiel-Asset, kein Overlay.
- Eigene exe-Kopie `re15_sfv4_<pid>.exe`, timeout 400.
- **EXIT=0**, 901 FRAMEDUMPs, 48 s.

Auszug aus `debug.log` (`szene_debug_auszug.txt`):
- `[asset] cd-root: C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel/re15_port/shared_assets/PSX`
- `[enemy] EM30: Ersatzmodell EMD/EM30_HANGAR.EMD (Raum 50A0)`
- `EM30 loaded: 23 meshes, 23 bones, 21 clips -> slot 12`
- `[hangar] ROOM50A0 Szene Start: Schnitt 4 angefordert -> Cut 12, Birkin Slot 35, Spider-Man Slot 34`
- `[szenenschutz] F235 an`
- `[hangar] ROOM50A0 Szene Ende: Cut 4, Birkin Zustand 0`
- `[szenenschutz] F393 aus`
- `EXIT_AT: Bild 1110`
- 0 Zeilen mit error, fehler oder assert.

Damit beginnt und endet die Szene wie im freigegebenen Lauf (F235 / F393, gleiche Zeilen).

Vergleich gegen `sfv4/r/V7_szene` mit eigenem `fdvgl.py` (`szene_vgl.txt`):
- Bilder 225-450: **226 von 226 bytegleich**.
- Bilder 200-1100: **901 von 901 bytegleich**.
- `state.log` ist **bytegleich**.

### 6.2 Spieler danach frei, kein Haenger

Zweiter Lauf `<abn0>/frei`: dasselbe Skript, aber Laufen nach dem Szenenende (`...,W3.7,R0.5,UX3,L0.4,UX2,W5`, Ende F750). Ergebnis in `frei_state.log`:
- F400: Spieler steht (-8588,-13453), rot 256.
- F420: pad 4010, (-9203,-13820), rot 1696.
- F440: (-11949,-15829).
- F460: (-11949,-17827).

Der Spieler dreht und laeuft also, sobald die Szene endet. Ab F480 trifft der Birkin (hp 100 -> 90).

Der Prozess endet ordentlich an `EXIT_AT: Bild 750`. Im 1110-Bilder-Lauf greift der Birkin weiter an (hp 80 bei F760), bis F1110.

### 6.3 Echtes Fenster per gdigrab

Werkzeug: `SF_belege/abn0/fenster.sh`.
- Renderer: `[render] SDL-Renderer direct3d, Videotreiber windows`. Kein `RE15_SOFTWARE_RENDER`, kein `RE15_AUTOSHOT`, kein `RE15_FRAMEDUMP`.
- `RE15_TITLE_SHOT` ist gesetzt, nur fuer den Titel-Vorlauf (einmaliges Titelbild vor dem Raum). Ohne ihn blieb der erste Versuch im Titel stehen.
- ffmpeg gdigrab mit 4 Bildern/s auf das Fenster "RE1.5 Rebuilt — PC", 121 Bilder. Die exe endete an EXIT_AT 900.

Uebersicht: `fenster/kontakt.png`. Die Szene liegt in g0031-g0050.

Eigener Abgleich `fenster_vgl.txt`:
- Fuer jedes der 20 Szenenbilder habe ich unter F225-F450 das naechste freigegebene Bild gesucht (F235, 243, ..., 377).
- In voller Aufloesung 960x720 sind **20 von 20 Fensterbildern pixelgleich** mit dem jeweils passenden freigegebenen Bild.
- Das naechste fremde Bild liegt bei MAD 0,49-9,50. Die Zuordnung ist also eindeutig.

Selbst angesehen: g0036 (Griff, Spider-Man oben), g0042 (Spider-Man am Boden), g0048 (Birkin frontal) und g0070 (nach der Szene, Cut 4).

Was zu sehen ist:
- Die Sichel-Form ist vollstaendig texturiert: graue Haut, rote Wunden, blaue Jeans, Krallen der linken Hand rot, Schwanz durchgehend texturiert.
- Es gibt keine falsche CLUT-Zeile, kein Mosaik und keine schwarzen Flaechen.

### 6.4 Labor 50E0 / 50F1

Werkzeug `labor.sh`:
- Debug-Sprung wie in `integration_r41_sichel_form`, Pistole (`RE15_EQUIP=3 RE15_GIVE=3:400`).
- 12-mal Zielen und Feuer (R1 + Quadrat), Ende F1100.
- Spieler 6200 bzw. 6500 vor dem Birkin.
- Fuer Elza zusaetzlich `RE15_TITLE_SHOT_CHAR=1`.

| Raum | Ergebnis |
|---|---|
| 50E0 (Leon) | `EM30: Ersatzmodell EMD/EM30_HANGAR.EMD (Raum 50E0)`, `23 meshes, 23 bones, 21 clips -> slot 11`. Der erste Schuss F378 (pad 8800) trifft: Birkin Slot 1 geht im selben Bild von `st=1 ss1=9` nach `st=2 ss1=3 ss2=1`; Munition 144 -> 132. |
| 50F1 (Elza, `[pl] PL04, Elza-Bit=1`) | `(Raum 50F1)`, 23/23/21 geladen. 12 Schuesse aus (-12100,-25000) auf den Birkin bei (-12100,-31500) treffen nicht (st bleibt 1). |
| Gegenprobe Capcom-EM30 (`RE15_BIRKIN_FORM=original`, 16 Knochen) | `state.log` 50E0 und 50F1 **bytegleich** mit dem V7-Lauf. Der V7-Lauf und ein Lauf mit Referenz 2 (Overlay) zeigen in 50E0 dieselben Zustandswechsel. |

Die Form aendert also kein Verhalten im Labor. Treffer werden genommen (50E0).

Dass der Labor-Birkin nach dem Debug-Sprung stehen bleibt, ist vorbestehend und haengt nicht an der Form. Siehe H1.

Belege: `labor/state_*.log`, `labor/debug_*.txt`.

## 7. Vorbefunde

**H_abnahme_0.md / H_abnahme_1.md (2026-10-24)**

Beide betreffen Spur H "kollision" (Zweig r41/kollision), nicht den Hangar.
- Abnahme 0 war "bestanden = NEIN", nur wegen der Vertragsabweichungen V1/V2.
- Diese wurden per Entscheid H-E1 angenommen. Abnahme 1 ist "bestanden = JA", ohne Maengel.
- Offen ist dort nichts.

Fuer Spur H "hangar" (Merge 7f2ea1b7d) liegt in diesem Zweig keine eigene Abnahme vor. Diese Abnahme deckt die Hangar-Szene mit ab (Abschnitte 6.1-6.3, Tests A/B). Siehe H7.

**Juengste SF_PRUEFUNG-Berichte**

| Bericht | Ergebnis |
|---|---|
| RE2KOPF_r2_sicht (2026-11-07 23:46) | NICHT bestanden: Adernetz der G2-Kuppe zu hell |
| RE2KOPF_r2_zahlen | BESTANDEN |
| KOPF_r3_sicht | NICHT bestanden: Haarton |
| KOPF_r3_zahlen | BESTANDEN |
| REF2TEX_r2_sicht | NICHT bestanden: Haarfeld |
| REF2TEX_r2_zahlen | BESTANDEN |
| V4_r5_zahlen | nicht bestanden: 8 rote Gestaltungszeilen |

Alle offenen Punkte sind Aussehen-Punkte frueherer Zwischenstaende (V4ot2/V4ot3/V4re2/V4r5). Sie wurden ersetzt: V5k mit dem Original-Kopf, dann V5f, V6 und V7. Der Nutzer hat das Aussehen von V7 am 2026-11-08 freigegeben ("Naja, gehen wir so erst mal ins rennnen."). Sie sind damit vom Nutzer entschieden.

Keiner dieser Berichte nennt einen technischen Mangel, der im Spiel-Asset weiterlebt:
- Ein ungueltiger CLUT-Index wuerde in 2.3 auffallen.
- Absturz und Haenger wuerden in 6 auffallen.

## 8. Befunde

Blockierend: keine. Klein: keine.

**H1 (hinweis): Labor-Birkin steht nach dem Debug-Sprung.**
- In 50E0 und 50F1 steht der Labor-Birkin in `st=1 ss1=9` mit `mo=0 af=16` (keine Animation).
- In 50E0 bleibt er nach dem Treffer in `st=2 ss1=3 ss2=1`, ebenfalls ohne Animationsfortschritt.
- Mit dem Capcom-EM30 ist es bitgleich. Es ist also vorbestehend und nicht Folge der Form. Vermutlich wartet der Mod-Kampf auf einen SCD-Ausloeser, den der Debug-Sprung nicht setzt.
- Den Kampfverlauf am echten Weg hat diese Abnahme nicht geprueft.

**H2 (hinweis): Riegel fuer die V4-Kette gelockert.**
- `test_schwanz_r41.c` prueft die V4-Kette des Spiel-Assets mit gelockerten Schwellen:
  - Nachlauf > 60 statt > 300,
  - Mitte ab 0 Takten,
  - Szenenziel nur noch "naeher als ohne Ziel" (gemessen 380 statt < 40).
- Der Gegenlauf mit der Gliederung der Referenz 2 behaelt die alten Schwellen (gemessen 1007 / 6 / 0).
- Die Abweichung der Schwanzspitze vom Bahnpunkt im Spiel ist ungemessen. Sie steht im Dossier als offen.
- Die Szene hat der Nutzer abgenommen, und das Spiel zeigt sie bytegleich (6.1). Es ist also kein Mangel.

**H3 (hinweis): Software-Renderer-Fix fehlt in diesem Zweig.**
- Der Fix 4dde57452 fehlt hier.
- V7 hat eine Port-Textur 384 x 3328. SDLs CPU-Rasterer laeuft ueber, sobald `s2.y * area` > 2^31, also ab ca. 645000 Flaechen-Einheiten bei v ~3327.
- Das tritt nur bei grossen Dreiecken in Nahaufnahme und nur mit `RE15_SOFTWARE_RENDER` oder im CPU-Rueckfall auf.
- `integration_r41_sichel_form` (Software) lief gruen. Der beschleunigte Renderer ist nicht betroffen.
- Der Fix muss mit der Integration mitkommen; er steht in r41/integration.

**H4 (hinweis): `birkin_formen.py` ohne `--variante` ist eine Falle.**
- Ohne `--variante` ueberschreibt `birkin_formen.py` das Spiel-Asset mit Referenz 2.
- Kopfkommentar und `die`-Meldung warnen davor, und das Paket-Tor faengt es ab (2.2).
- Ein Rest-Kommentar in `make_package.sh` Z. 309 nennt noch "(tools/birkin_formen.py, mit Schwanzkette ...)". Er ist veraltet, aber harmlos.

**H5 (hinweis): Probe-Merge mit r41/integration hat 5 Konflikte.**
- Ergebnis von `git merge-tree --write-tree r41/integration HEAD`: Inhaltskonflikte in `birkin_formen.c`, `re15_birkin_formen.h`, `main.c`, `tests/unit/CMakeLists.txt` und `test_birkin_szenen.c`.
- Im Probe-Merge liegen das V7-Asset (Blob 73c800e0) und der Pin `319bbd17` korrekt vor.
- In `main.c` muessen beide Rohr-Bedingungen zusammengefuehrt werden: `npc_bones == 16` aus diesem Zweig und `!re15_bb_bank_da(npc)` aus der Integration.
- Danach Gruppe A/B in der Integration nachfahren (Memory `reai-v2-gezielte-integration-querbezug`).

**H6 (hinweis): Overlay fehlt bei den Adress-Zitaten.**
- Die @0x-Zitate der Overlay-Funktionen nennen das Overlay nicht: Birkin = STAGE3, Spider-Man = STAGE4.
- Gemessen stimmen sie (Abschnitt 4).

**H7 (hinweis): Keine eigene Abnahme fuer Spur H "hangar".**
- Fuer Spur H "hangar" gibt es keine eigene Abnahme. H_abnahme_0/1 gehoeren zu Spur H "kollision".
- `NUTZER_ENTSCHEIDE.md` fehlt in diesem Zweig. Der Bauer fuehrt das als Offen 7; der Eintrag muss mit der Integration kommen.
