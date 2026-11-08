# Integration Runde 41, Lauf C (Spur SF Sichel-Form V7): unabhaengige Gegenpruefung

Stand 2026-11-09. Integrationsbaum `C:/workspace/git/reAi_v2/.claude/worktrees/r41_integration`, Zweig r41/integration.
Kopf 154e59a89 (nur `analysis/befunde_runde41/INT_laufc_sichel.md`, 148 Zeilen, kein Code), darunter Merge
91636662f9f851254197ad7fed8ea5d6c3d881c8, Eltern ebfc5f0cf (Integration) und 5ea6d94f0 (r41/sichel_nb2), Merge-Base 789f6e681.
Geprueft wurde nur lesend, gebaut wurde inkrementell, eigene Ausgaben liegen unter
`C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/laufc/gp/`.

## Ergebnis

**In Ordnung, kein Befund.**

- Es fehlt kein Hunk, weder von der SF-Seite noch von der Integrationsseite. Der Abgleich lief ueber den ganzen Baum (B/S/I/M), nicht nur ueber Stichproben.
- Alle Konfliktloesungen wirken fachlich fuer beide Seiten.
- Das Asset ist V7. Pin und Paket-Tor stimmen. BAHNSTEIG_FORM ist unveraendert.
- Die Summenzeilen des Integrators stimmen mit seinen ctest-Logs ueberein.
- Die eigenen sieben Laeufe (6 Riegel, npcglieder in 2 Teilen) sind gruen.

Eine Anmerkung ohne Wirkung (kein Befund): `birkin_formen.c` bindet `<stdio.h>` jetzt zweimal ein, Zeile 20 unbedingt von der
Integrationsseite und Zeilen 30-32 unter `#ifdef RE15_PLATFORM_PC` von der SF-Seite. Das wurde automatisch gemergt.
Wegen der Include-Waechter ist es harmlos. Die vier vom Integrator genannten Doppel-Header (camera/aot/skeleton/engine) sind tatsaechlich je einmal vorhanden.

## 1. Verlorene Hunks

Methode: `git ls-tree -r` fuer B=789f6e681, S=r41/sichel_nb2 (5ea6d94f0), I=ebfc5f0cf und M=91636662f, dann je Pfad ein Blob-Vergleich:
- Nur S geaendert: M muss S sein.
- Nur I geaendert: M muss I sein.
- Beide gleich geaendert: M muss diesem Stand gleichen.

Ergebnis: **0 Abweichungen** ueber 7070 Pfade, die nur I aendert, und 1239 Pfade, die nur S aendert. Auf beiden Seiten verschieden geaendert sind 7 Pfade:

| Datei | M = S | M = I | Pruefung |
|---|---|---|---|
| engine/src/re15_damage.c | nein | **ja** | Der einzige SF-Hunk (Spinnenblut, Sub-Byte `& 0xffu`, Kommentar @0x8001970c..@0x80019888) ist inhaltsgleich schon in I (Spur S NB1). HEAD Zeile 3525: `(uint8_t)((a0 >> 16) & 0xffu)`. Nichts fehlt. |
| engine/src/birkin_formen.c | nein | nein | Zeilenabgleich unten |
| include/re15_birkin_formen.h | nein | nein | alle Zeilen beider Seiten enthalten |
| platform/pc/main.c | nein | nein | alle Zeilen beider Seiten enthalten |
| tests/unit/CMakeLists.txt | nein | nein | alle Zeilen beider Seiten enthalten |
| tests/unit/test_birkin_szenen.c | nein | nein | alle Zeilen beider Seiten enthalten |
| release/make_package.sh | nein | nein | eine SF-Kommentarzeile umformuliert (G4 -> FORM), sonst alles enthalten |

Zeilenabgleich mit Multimengen je Seite: Was eine Seite hinzufuegt, muss in M stehen, und was eine Seite entfernt, darf nicht bleiben.
In birkin_formen.c fehlen je Seite genau 5 Zeilen, alle belegt:
- 4 Include-Zeilen je Seite (camera, aot, skeleton, engine) sind zu je einer Zeile mit beiden Kommentaren vereinigt (HEAD Zeilen 15-19).
- S `b->hp = 300` und I `b->hp = RE15_BIRKIN_HP_INIT` (alte Hangar-Stelle, die S in `bf_hangar_birkin_setzen` verlegt hat) sind zu
  `b->hp = RE15_BIRKIN_HP_INIT;  /* INIT-Wert 0x12c @0x8011690c (x Faktor, GR NB3 2fb07b019) ... */` geworden.
  Das ist die Integrationsabsicht GR NB3, der Wert ist 600 (`RE15_BIRKIN_HP_INIT (300 * RE15_BIRKIN_HP_FAKTOR)`, re15_birkin_formen.h:72).
- `bf_anim_halt` fehlt in M. S hat die Funktion mit dem Hangar-Umbau entfernt, I hat sie gegenueber B nicht geaendert. Das ist also eine legitime Loeschung, der Bau ist gruen.

Zeilenzahlen von birkin_formen.c: B 439, S 1144 (+705), I 896 (+457), M 1597 = 439 + 705 + 457 - 4 (vereinigte Includes).

Integrationsabsichten aus `integration_welle2_absichten.md`, im HEAD geprueft:
- `static int s_kam_g5` gibt es nicht mehr. Der Name steht nur noch im Kommentar Zeile 1497.
- Die Nahsicht-Wache hat das Gate `!re15_g5a_laeuft()` (Zeile 1408).
- Raumtick: `bf_nahsicht_wache()` (Zeile 1034) und danach `re15_g5a_tick()`.
- `bf_rueckstoss_tick`: `case 3` Sturz (Zeile 1582) und `case 2` mit `re15_bf_g5_auftritt_start(slot)` (Zeilen 1586/1592).
- re15_birkin_formen.h: `#define RE15_BB_FORM_HP 1000` genau einmal.
- main.c: `if (gwl_aus) continue;` 1x, `re15_g5_tentakel_kette_strecken(npc_skel` 1x.
- CMakeLists: kein doppeltes `add_test` und kein doppeltes `add_executable` (sort | uniq -d leer). test_n2_griff_start, test_r41_g5auftritt, test_za_spawnvariante und test_schwanz_r41 stehen je 1x.
- test_birkin_szenen.c: `s_hook_rufe` 0x.
- make_package.sh: `BAHNSTEIG_G4.` 0x. Die Zeilen ANNETTE_4D und R704_LAMPE stehen je 1x.
- re15_gator_re2_r41.h: `re15_gr2_geschwaecht_slot` vorhanden.

Sherry-Fix 2c1c0eb1a ist Vorfahr von HEAD. enemy_common.c, probes/r41_npcglieder.cmake und test_r41_npcglieder.c sind blobgleich mit I.
Software-Renderer-Fix 4dde57452 ist Vorfahr. In render_pc.c steht `#define RE15_SW_PRODUKT_MAX` 1x und der Kopf "SDLs CPU-RASTERER: 32-BIT-UEBERLAUF" 1x.
`git diff ebfc5f0cf HEAD -- render_pc.c enemy_common.c` ist leer.

## 2. Konfliktstellen fachlich (git show 91636662f --cc)

- **birkin_formen.c, Includes:** vereinigt, siehe Anmerkung oben.
- **birkin_formen.c, Funktionen:** `re15_bf_granate_form` (I) und `re15_bf_labor_aktiv` (S) stehen nebeneinander und kollidieren nicht. `re15_bf_labor_aktiv` wird nur in `re15_bf_emd_datei` gerufen (Zeile 57), nach `re15_bf_hangar_aktiv`. Die Riesenform laeuft ueber eine eigene Bank (`re15_bf_bahnsteig_bank_faellig`), deshalb ist die Reihenfolge ohne Wirkung.
- **birkin_formen.c, `re15_bf_reset`:**
  - I-Teil: `memset(s_bf_nach_mutation)`, `re15_g5a_reset()`, Kamera-Statics der Wache, `s_rs_kam_halt = 0`.
  - S-Teil: Riegelfreigabe `if (s_h.riegel) { bf_riegel(0); g_scd.cut_auto_enabled = 1; }`, `re15_schwanz_r41_reset()`, `s_h.phase = BF_H_WARTEN`, `cut_zurueck = 0`.
  - Es gibt kein fruehes return. Beide Seiten laufen vollstaendig.
- **birkin_formen.c, `re15_bf_raumtick`:** Der Integrationsteil (Rueckstoss, Nahsicht-Wache, G5A-Tick) laeuft unveraendert vor dem Hangar-Tor `if (!re15_bf_hangar_aktiv()) {...; return; }`. Danach folgt nur der SF-Ablauf: Eintritt mit `bf_hangar_birkin_setzen`, WARTEN -> Ausloeser, FREI, Takt, Ende. Kein frueheres return schneidet einen Integrationsteil ab.
- **main.c, Rohr-Tor:** `npc_bones == 16` (S) UND `!re15_bb_bank_da(npc)` (I), beide Kommentare. Die Bahnsteig-Form (32 Knochen) scheidet schon an `== 16` aus, das I-Tor bleibt als zweite Sicherung. Die automatisch gemergten Teile `gwl_aus`, `kette_strecken`, `re15_schwanz_r41_pose` und die Messhaken GRIFF/BEIN_LOG (nur ueber getenv) sind vorhanden.
- **tests/unit/CMakeLists.txt:** unit_r41_g5auftritt (120 s) und unit_schwanz_r41 (30 s). Dazu integration_r41_birkin_pistole mit TIMEOUT 1300 samt beiden Kommentarpaaren und integration_r41_sichel_form mit TIMEOUT 400. Keine Doppel.
- **test_birkin_szenen.c:** Stufe-3-CHECKs 5080/5081/5091/50A0 und Labor-CHECKs 50E0/50F1 (23 Knochen / 23 Meshes) stehen hintereinander in `test_modellwahl`.
- **make_package.sh:** Das Runde-41-Tor (Liste mit GATOR40A0.RBJ und BAHNSTEIG_FORM.EMD/.TIM, Zeilen 289-300) und das EM30-Tor (Zeilen 318-333: Datei nicht leer, `cmp` gegen den Quellbaum, sha256 gegen `release/em30_hangar.sha256`) stehen je 1x. `bash -n` ohne Fehler.
- **re15_birkin_formen.h:** Konflikt nur im Kopfkommentar. Prototypen beider Seiten stehen da (`re15_bf_riesenform_raum`, `re15_bf_flucht_raum`, `re15_bf_labor_aktiv`).

## 3. Asset

- `git show HEAD:re15_port/shared_assets/PSX/EMD/EM30_HANGAR.EMD | sha256sum` ergibt 319bbd17f10ba0cdd703861c6d844a00fb414d82b513e77057bf0ff80fda65f6 bei 268948 Byte.
  Die Datei im Arbeitsbaum hat denselben Hash, die Vorlage `scratchpad/sfv4/emd/EM30_V7.EMD` ebenfalls.
- `release/em30_hangar.sha256` im HEAD lautet `319bbd17f10ba0cdd703861c6d844a00fb414d82b513e77057bf0ff80fda65f6  re15_port/shared_assets/PSX/EMD/EM30_HANGAR.EMD`.
- `git diff --stat ebfc5f0cf HEAD -- '*BAHNSTEIG_FORM*'` ist leer. re15_port/shared_assets/RE2/BAHNSTEIG_FORM.EMD/.TIM sind unveraendert.
- `RE15_ASSETS_PATH` im Bau zeigt auf den Integrationsbaum (`.../r41_integration/re15_port/shared_assets/PSX`).

## 4. Tests

**Summenzeilen des Integrators** gegen seine Logs in `scratchpad/laufc/`:

| Lauf | Log | Summenzeile im Log | Zeilen mit Passed |
|---|---|---|---|
| (a) | ctest_a.log | `100% tests passed, 0 tests failed out of 1100` / `Total Test time (real) = 319.67 sec` | 1100 |
| (b) | ctest_b.log | `100% tests passed, 0 tests failed out of 108` / `Total Test time (real) = 5796.10 sec` | 108 |
| SF-A | ctest_sfA.log | `100% tests passed, 0 tests failed out of 12` / `Total Test time (real) = 489.39 sec` | 12 |
| SF-B | ctest_sfB.log | `100% tests passed, 0 tests failed out of 21` / `Total Test time (real) = 476.50 sec` | 21 |

Zu den ctest-Dateien unter `re15_port/build/Testing/Temporary/`:
- `LastTest.log` (09.11. 00:14-00:22) ist der Lauf SF-B: 21 Tests, 21x "Test Passed", 0 Fehlschlaege.
- `LastTestsFailed.log` stammt vom 08.11. 11:17, also vor dem Merge (22:20). Er ist veraltet und gehoert nicht zu Lauf C.

Software-Renderer-Lauf des Integrators:
- `sw_lauf.txt`: EXIT=0, 901 Bilder, EM30_HANGAR mit 23/23/21 geladen.
- `diff <(sed s/hp=600/hp=300/ sw_norm.log) ref_norm.log` ergibt 0 Zeilen. Die normalisierte state.log ist also bytegleich bis auf die HP-Skala (Integrationsabsicht GR NB3).

**Eigener Bau:** `RE15_BUILD_DIR=.../r41_integration/re15_port/build bash re15_port/tools/local_build.sh build` meldete `ninja: no work to do.` und `=== LOCAL-BUILD-OK (build)`.
Die Binaerdateien entsprechen also dem HEAD-Quellstand. Der Arbeitsbaum ist sauber bis auf `Testing/`.

**Eigener Lauf:** `ctest -j 3 --timeout 900 -R "^(unit_birkin_formen|unit_birkin_szenen|unit_schwanz_r41|integration_r41_sichel_form|unit_r41_uv_textur|unit_r41_npcglieder_.*)$"`.
Unter dem Namen unit_r41_npcglieder ist der Test als `unit_r41_npcglieder_sherry6031` und `unit_r41_npcglieder_annette6030` registriert.
Die Ausgabe steht in `scratchpad/laufc/gp/ctest_gp.log`.

    100% tests passed, 0 tests failed out of 7

    Total Test time (real) =  64.40 sec

Einzelzeiten:

| Test | Zeit |
|---|---|
| unit_schwanz_r41 | 0.04 s |
| unit_r41_npcglieder_annette6030 | 0.01 s |
| unit_r41_npcglieder_sherry6031 | 0.01 s |
| unit_birkin_formen | 0.02 s |
| unit_r41_uv_textur | 0.03 s |
| unit_birkin_szenen | 0.12 s |
| integration_r41_sichel_form (RE15_SOFTWARE_RENDER=1) | 64.34 s |

Die npcglieder-Teile haben wirklich geprueft, sie geben je 5 `ok:`-Zeilen aus, darunter "groesste relpos-Abweichung 0, Abstand Kind-Eltern 0 (Toleranz 0, @0x800224a8)".
integration_r41_sichel_form, Laufverzeichnis `build/tests/integration/r41_sichel_form_wd/lauf_1cea2506`:
- 50A0: `EM30: Ersatzmodell EMD/EM30_HANGAR.EMD (Raum 50A0)` und `EM30 loaded: 23 meshes, 23 bones, 21 clips -> slot 13`.
- 50E0: dieselben Meldungen fuer Raum 50E0, dort `slot 11`.
- Zeichner: `ZEICHNE Typ 0x30: 23 Teile (Bones 23, Meshes 23) grid 0x10 rohr 0 remap 0`. Das Rohr-Tor greift also nicht auf die Schwanzkette.
