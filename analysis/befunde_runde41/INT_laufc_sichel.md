# Integration Runde 41, Lauf C: Spur SF Sichel-Form V7 in r41/integration

Stand 2026-11-09 00:30. Worktree C:/workspace/git/reAi_v2/.claude/worktrees/r41_integration, Zweig r41/integration.
Der Sichel-Baum C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel wurde nur gelesen.

## 1. Ausgangslage

- Kopf vorher: ebfc5f0cf (doc(r41-integration): Pruefung Fix gestreckte Sherry 6031 / Annette 6030 - bestanden).
- git merge-base --is-ancestor master HEAD: ja (master b27d96891 ist enthalten).
- Arbeitsbaum sauber bis auf das unversionierte Testing/ (ctest-Ausgabe).
- Eingemergt: r41/sichel_nb2, Kopf 5ea6d94f0 (Abnahme 0 BESTANDEN, SF_abnahme_0.md). Merge-Base 789f6e681. 122 Commits.
- Software-Renderer-Fix 4dde57452: Der Sichel-Zweig aendert render_pc.c nicht
  (git log 789f6e681..r41/sichel_nb2 -- render_pc.c ist leer, git cherry zeigt keinen Cherry-pick).
  Der Fix liegt danach genau einmal vor, aus r41/integration.

## 2. Merge

- Merge-Commit: 91636662f9f851254197ad7fed8ea5d6c3d881c8
  "merge(r41): Spur SF Sichel-Form V7 (r41/sichel_nb2, Abnahme bestanden)".
- Konflikte: 5 Dateien mit zusammen 10 Stellen, wie von git merge-tree vorhergesagt.
- release/make_package.sh wurde automatisch gemergt.
- Die Merge-Nachricht belegt die Loesung je Datei; sie enthaelt keine Zeilen "# Conflicts:".

### Konfliktloesung je Datei (beide Seiten vollstaendig erhalten)

**re15_port/engine/src/birkin_formen.c** (4 Stellen)
1. Includes vereinigt.
   - Vier Header standen auf beiden Seiten: re15_camera.h, re15_aot.h, re15_skeleton.h, re15_engine.h. Sie sind jetzt je einmal eingebunden.
   - Ihr Kommentar nennt beide Verwender: Nachbesserung 4 und Spur H.
2. Beide Funktionen bleiben nebeneinander:
   - re15_bf_granate_form (Integration, GR NB2/NB3 und WS: Raketen-Praedikat, Formende),
   - re15_bf_labor_aktiv (SF, Nutzer E17: Labor 50E0/50F0).
3. re15_bf_reset vereint beide Seiten:
   - aus form_nb4: s_rs_kam_halt = 0,
   - aus SF: Riegel-Freigabe bei Raumwechsel, re15_schwanz_r41_reset, Phase BF_H_WARTEN und cut_zurueck.
4. Hangar-Raumeintritt: Hier gilt der SF-Ablauf (Spur H: Figuren erst ab dem Ausloeser, bf_hangar_birkin_setzen).
   - Die Integrationsseite hatte an dieser Stelle nur die HP-Skala geaendert (git diff 789f6e681 HEAD: einzige Zeile b->hp, Commit 2fb07b019 GR NB3).
   - Diese Aenderung ist in bf_hangar_birkin_setzen nachgezogen: b->hp = RE15_BIRKIN_HP_INIT (0x12c @0x8011690c x RE15_BIRKIN_HP_FAKTOR 2 = 600) statt 300.

**re15_port/include/re15_birkin_formen.h** (2 Stellen, nur im Kopfkommentar)
- Modellzeilen:
  - EM30_HANGAR mit Schwanzkette (Knochen 16-22), Hangar und Labor (SF);
  - RE2/BAHNSTEIG_FORM aus Stufe 3 (Integration).
  - Die SF-Zeile zu BAHNSTEIG_G4 entfaellt, weil die Datei in der Form-Linie geloescht ist.
- Ablauftexte:
  - HANGAR-Absatz aus Spur H mit den Modell-Voraussetzungen (SF);
  - BAHNSTEIG 5080/5081 und ZUG 5090/5091 mit der Riesenform ab der Raumladung (Integration).
- Die Prototypen beider Seiten, RE15_BB_FORM_HP 1000 und RE15_BIRKIN_HP_* kamen ohne Konflikt herein (re15_bf_labor_aktiv, re15_bf_riesenform_raum, re15_bf_flucht_raum).

**re15_port/platform/pc/main.c** (1 Stelle, Rohr-Tor)
- Beide Bedingungen sind zusammengefuehrt:
  - npc_bones == 16 (SF: Nur das Capcom-EM30 hat 16 Knochen. Bei der Sichel-Form ist Mesh 16 das erste Schwanzglied.)
  - UND !re15_bb_bank_da(npc) (Integration: Die Bahnsteig-Form erbt birkin_rohr aus INIT @0x80116974.)
  - Beide Kommentare stehen im Code.
- Ohne Konflikt hereingekommen und behalten:
  - gwl_aus-Cull (G2),
  - re15_g5_tentakel_kette_strecken (G5A),
  - re15_schwanz_r41_pose (SF N1.9),
  - die Messhaken RE15_GRIFF_LOG und RE15_BEIN_LOG (SF, nur Messung).

**re15_port/tests/unit/CMakeLists.txt** (2 Stellen)
- unit_r41_g5auftritt (Integration) UND unit_schwanz_r41 (SF).
- integration_r41_birkin_pistole mit TIMEOUT 1300 und Kommentaren (Integration) UND der neue Test integration_r41_sichel_form mit TIMEOUT 400 (SF).
- Keine doppelten add_test- oder add_executable-Namen (gemessen mit sort | uniq -d: leer).

**re15_port/tests/unit/test_birkin_szenen.c** (1 Stelle)
- Die Stufe-3-CHECKs 5080/5081/5091/50A0 (Integration) bleiben.
- Dazu kommen die Labor-CHECKs 50E0/50F1 (SF): Hangar-Form gewaehlt, Bank mit 23 Knochen und 23 Meshes.

**release/make_package.sh** (automatisch gemergt)
- Das Runde-41-Tor (Liste mit GATOR40A0.RBJ und BAHNSTEIG_FORM.EMD/.TIM) und das EM30-Pin-Tor der SF-Seite stehen je einmal im Skript.
- Der Kommentar "wie BAHNSTEIG_G4" ist auf "wie BAHNSTEIG_FORM (Liste oben)" korrigiert.
- bash -n meldet keinen Fehler.
- Pin release/em30_hangar.sha256 = 319bbd17f10ba0cdd703861c6d844a00fb414d82b513e77057bf0ff80fda65f6.
  sha256sum von re15_port/shared_assets/PSX/EMD/EM30_HANGAR.EMD im Integrationsbaum ergibt denselben Wert (= V7).

## 3. Bau

- RE15_BUILD_DIR=<baum>/re15_port/build bash re15_port/tools/local_build.sh configure -> "=== LOCAL-BUILD-OK (configure)".
- ... build -> [969/969], "=== LOCAL-BUILD-OK (build)".
- Keine Kompilierfehler aus der Konfliktloesung; kein Fix-Commit noetig.
- Die drei Warnungen in main.c (discarded-qualifiers bei 9152, 10609 und 11086) liegen ausserhalb der Merge-Stellen.

## 4. Tests (Summenzeilen woertlich)

Logs liegen in C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/laufc/.
Waehrend der Laeufe liefen keine fremden Bau- oder Testprozesse; die Prozessliste war vor dem Bau leer.

**(a)** ctest_a.log, 22:23:56-22:29:16:
- Ausdruck: ctest -j 6 --timeout 600 -R "^(unit_|prop_|probe_)" -E "^(unit_r38_releaseskript_|unit_r30_android_|unit_r35_android_|unit_r39_titelversion_cmake|unit_r39_titelversion_ablauf$)"
- Ergebnis: 100% tests passed, 0 tests failed out of 1100 / Total Test time (real) = 319.67 sec
- Zusaetzlich ausgeschlossen ist unit_r39_titelversion_ablauf. Das ist der einzige der 7 df-haengenden Skripttests (PF_abnahme_2.md), den das Grundmuster nicht schon erfasst. Die uebrigen sechs: releaseskript_trocken/ablauf/fehler_bau/fehler_umfeld/cmd und android_bash_mutanten.

**(b)** ctest_b.log, Start 22:29:34, Ende 00:06:10:
- Ausdruck: ctest -j 2 --timeout 1800 -R "unit_birkin|unit_schwanz_r41|integration_r41_sichel_form|integration_r41_birkin_pistole|unit_r41_birkin|form|bahnsteig|g5_tod|g5_westen|uv_textur|npcglieder|annette|6030|r41_gatorfix|r41_re2gator|waffenschaden|hangar"
- Ergebnis: 100% tests passed, 0 tests failed out of 108 / Total Test time (real) = 5796.10 sec
- Darin 35 Integrationsriegel, unter anderem:
  - integration_r41_birkin_pistole (475,65 s) und integration_r41_sichel_form (64,44 s),
  - integration_r41_annette, integration_r41_g5_tod, integration_r41_g5_westen,
  - alle integration_r41_gatorfix_*, integration_r41_re2gator, integration_r41_form_wagenweg_*.
- Ausserdem unit_r41_uv_textur, unit_r41_npcglieder_annette6030 und _sherry6031.

**SF-Abnahme Gruppe A** (ctest_sfA.log):
- Ausdruck: ctest -j 2 -R "unit_birkin|unit_schwanz_r41|integration_r41_sichel_form|integration_r41_birkin_pistole|unit_r41_birkin|hangar" --timeout 900
- Ergebnis: 100% tests passed, 0 tests failed out of 12 / Total Test time (real) = 489.39 sec
- Im Sichel-Zweig waren es 11 Tests. Der zwoelfte, unit_r41_birkin_wand_g5saat, kommt aus r41/integration.

**SF-Abnahme Gruppe B** (ctest_sfB.log):
- Ausdruck: ctest -j 2 -R "birkin|hangar|50a|spiderman|formen|r38_birkin|room50|sichel" --timeout 900
- Ergebnis: 100% tests passed, 0 tests failed out of 21 / Total Test time (real) = 476.50 sec

Rot: keiner. Kein Fenster-Riegel musste nachgefahren werden.

## 5. Software-Renderer

- **integration_r41_sichel_form** setzt RE15_SOFTWARE_RENDER=1 (test_r41_sichel_form.cmake Z. 40). Er ist in (b) und in beiden SF-Gruppen gruen.
- **unit_r41_uv_textur** ist in (a) und (b) gruen.

**Eigener Szenenlauf 50A0 mit RE15_SOFTWARE_RENDER=1**
- Aufruf: Kopie von re15_port/tools/r41_sichel/v4/lauf_v4.sh mit BAUM bzw. EXE_SRC = Integrationsbaum, EMD "-" (Spiel-Asset, kein Overlay), Exitbild 1110, FD=200-1100/1, eigene exe-Kopie re15_laufc_sw_<pid>.exe, timeout 400.
- Laufordner: scratchpad/laufc/sw_szene.
- **Ergebnis**
  - EXIT=0 und 901 Bilder.
  - Der debug.log meldet "cd-root ...r41_integration/re15_port/shared_assets/PSX", "EM30: Ersatzmodell EMD/EM30_HANGAR.EMD (Raum 50A0)" und "EM30 loaded: 23 meshes, 23 bones, 21 clips".
  - Renderer laut debug.log: "SDL-Renderer software ... ABR2 (B-F) NICHT unterstuetzt -> Rueckfall BLEND".
- **Szenenverlauf**
  - [hangar] Szene Start: "Schnitt 4 angefordert -> Cut 12, Birkin Slot 35, Spider-Man Slot 34, Spieler (-8951,-13304)".
  - [hangar] Szene Ende: "Cut 4, Birkin Zustand 0".
  - Die Szene laeuft also bis zum Ende.
- Letzte state.log-Zeile: F1110 pad=0000 PL(-11949,-13620,rot=3713,hp=80) ... [35 t=30 st=1 ss1=4 ss2=1 ...].
- **Vergleich mit dem freigegebenen Lauf sfv4/r/V7_szene (direct3d)**
  - Die state.log hat 1131 Zeilen. Normalisiert wurde zweierlei: Die Felder hk/io/hr/ms gibt es nur in r41/integration, und der Referenzlauf hat CRLF-Zeilenenden.
  - Danach ist die state.log bytegleich bis auf die HP des Birkin (Slot 35): 600 statt 300 in allen 877 Zeilen.
  - Das ist die HP-Skala der Integration (GR NB3 2fb07b019, RE15_BIRKIN_HP_INIT), keine Abweichung im Ablauf.
- **Bilder**
  - 0 von 901 Bildern sind fast schwarz.
  - Bild 300 habe ich selbst angesehen: Birkin und Spider-Man sind vollstaendig texturiert, kein Mosaik.
  - Abweichend von direct3d ist der Letterbox-Balken hell (240,240,240) statt schwarz. Das ist der bekannte Rueckfall des SDL-Software-Renderers bei ABR2 (subtraktiv -> BLEND); Memory reai-v2-sdl-gl-customblend, debug.log-Zeile oben.
  - Die Sichel-Spur aendert render_pc.c nicht. Der Unterschied ist also nicht durch den Merge entstanden, und der Spieler sieht ihn nicht (Windows nutzt direct3d).

## 6. Fixes

Keine. Bau und alle Tests waren nach der Konfliktloesung gruen.

## 7. Hinweise

- Ein NUTZER_ENTSCHEIDE-Eintrag zur V7-Freigabe (Hinweis der SF-Abnahme) ist nicht Teil dieses Laufs. Er bleibt fuer die Doku-Nacharbeit offen.
- Die Hangar-Szene startet den Birkin jetzt mit 600 HP (Integrationsskala). Die SF-Abnahme hatte noch 300 gemessen. Das Aussehen und der Ablauf der Szene sind unveraendert (state.log ohne HP bytegleich).
