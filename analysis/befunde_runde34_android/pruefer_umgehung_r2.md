# Gegenpruefung Runde 2 — Linse UMGEHUNG/ROBUSTHEIT (Android-Asset-Gate)

Stand: 2026-09-29, laufend fortgeschrieben. Gegenpruefer, aendert KEINE Werkzeuge.
Baum: C:/workspace/git/reAi_v2/.claude/worktrees/r34a_android (Zweig r34a/android-gate)
Pruefgegenstand: Werkzeugstand 35d25455 (release/apk_asset_gate.py, apk_pruefen.sh, build_android.sh,
make_package.sh, python_finden.sh, app/build.gradle; seit 35d25455 nur Doku-Commits des Pruefers echtlauf r2).
Arbeitsordner (nicht committet): build/r34a/pruefer_umgehung_r2/ ; Belege: pruefer_umgehung_r2_belege/.
Parallel im Baum: Pruefer "echtlauf r2" (Android-Bau + make_package.sh in release/). Ich fasse release/ und
platform/android/ NICHT an; Kettenlaeufe gegen einen Schattenbaum laufen ueber KOPIEN der Skripte.

## 0. Vorgehen
- Werkzeuge: `r2_faelschen.py` (eigener roher ZIP-Leser/-Schreiber, schreibt die APK neu = unsigniert,
  signiert wird danach mit apksigner 35.0.0 und dem Debug-Schluessel des Release-Baus), `r2_mutanten.py`
  (TEILWEISE Abschwaechungen, je genau eine Textstelle), Schattenbaum `build/r34a/pruefer_umgehung_r2/schatten`
  (PSX + synchro als Hardlinks, RE15DOOR/RE2/extracted_fx als echte Kopien, build.gradle + die vier
  Skripte als unveraenderte Kopien -> `schatten/release/build_android.sh --gate-only` prueft gegen den Schatten).
- Referenz: Kopie der Archiv-APK v0.8.19 (sha256 514bebd5... = Archiv-SUMS), `build/r34a/pruefer_umgehung_r2/ref.apk`.
- Werkzeug-Kontrolle: K0 (Neuschreiben ohne Aenderung) -> echtes Gate rc 0 (Werkzeug transparent);
  Schatten-Kontrolle: Schatten-Gate gegen ref rc 0.

## 1. Messungen (laufend)

### 1.1 Selbsttest faengt TEILWEISE Abschwaechungen nicht (5/5 Mutanten: SELBSTTEST-OK 72/72)
`r2_mutanten.py release/apk_asset_gate.py build/.../mutanten`, dann je `<mutant> --selbsttest`
(`selbsttests_uebersicht.txt`) und je Mutant eine Faelschung an der echten APK (`mutanten_gegen_faelschungen.txt`):

| Mutant (eine Zeile) | Selbsttest | Faelschung | echtes Gate | Mutant |
|---|---|---|---|---|
| M1 Manifest-Groesse `!=` -> `<` (nur "Manifest zu gross" gemeldet) | OK 72/72 | P2DS-Zeile 78243 statt 78244, Kopf nachgezogen | 1 | **0** |
| M9 `crc != e.crc or n != e.usize` -> nur CRC | OK 72/72 | classes.dex usize +100 in LFH+CD | 1 | **0** |
| M5 EOCD `!=` -> `<` (zu grosse Kommentarlaenge nicht gemeldet) | OK 72/72 | EOCD-Kommentarlaenge +10 | 2 | **0** |
| M14 Data-Descriptor-Bit aus dem CD statt aus dem LFH | OK 72/72 | P07G: DD-Bit nur im CD, LFH-CRC ^1 | 1 | **0** |
| M16 uebrige Eintraege nur Stored pruefen | OK 72/72 | classes.dex (Deflate) Byte gekippt | 1 | **0** |
Echtes Gate Selbsttest: OK 72/72 (7 s). Signierbarkeit (apksigner 35.0.0 sign, Debug-Schluessel):
F_M1 -> sign rc 0, verify rc 0 (v2 true) = NUR das Gate schuetzt; F_M9/F_M14/F_M16 -> `Malformed ZIP entry`
(nicht signierbar -> in der Kette faengt apksigner); F_M5 ueberlebt kein Signieren (apksigner schreibt neu).
Ursache: keine Selbsttest-Faelle fuer "Manifest-Groesse KLEINER als APK", "Laenge != usize bei richtiger
CRC (Nicht-Asset)", "Kommentarlaenge zu gross", "DD-Bit nur im CD", "Nicht-Asset komprimiert" (alle
_NICHT_ASSETS der Fixture sind Stored: `zf.writestr(name, b)` ohne compress_type).

### 1.2 Konsistent unvollstaendiger Quellbaum: 29 von 30 Tuerarchiven -> ganze Kette gruen
Schatten ohne RE15DOOR/P2DS.DO2 + APK ohne den Eintrag (Manifestzeile weg, Kopf nachgezogen), signiert:
`schatten/release/build_android.sh --gate-only F_ohne_P2DS_sig.apk --version v0.8.19` -> **EXIT=0**,
`RE15DOOR:  Quelle 29, APK 29, sha256 gleich 29/29`, `ANDROID-GATES-OK` (`kette_schatten_ohne_P2DS.log`).
Die Engine erwartet 30 (engine/src/gen/re15_tuer_eigen.inc: `re15_tuer_eigen[30]` mit Groesse + FNV-1a je
Archiv; door_scene_pc.c:218 prueft beim Laden). Gradle-doFirst verlangt nur >= 1 *.DO2 (build.gradle:122-128),
PFLICHT_ORDNER im Gate ebenso (apk_asset_gate.py:368-372), make_package check_tree ist relativ
(`pc_check_tree_ohne_P2DS.log`: `Port-Tuerarchive im Paket: 29`, CHECK_TREE_OK).
Variante 0-Byte-Archiv: Schatten-P07G.DO2 0 B + APK-Eintrag 0 B -> Gate rc 0 `RE15DOOR 30/30`
(`schatten_gate_P07G_leer.log`), dagegen make_package check_tree (unveraenderte Funktion, awk):
`ABBRUCH: Port-Tuerarchiv fehlt/leer im Paket: shared_assets/RE15DOOR/P07G.DO2` (`pc_check_tree_P07G_leer.log`).
Schatten danach wiederhergestellt (`diff -r` RE15DOOR Schatten = Repo), Repo unberuehrt (`git status` leer).

### 1.3 Nur das Python-Gate (Kette faengt es)
- F_praefix (16 Null-Bytes vor dem ersten Local Header, alle Offsets verschoben): Gate **rc 0**
  `ZIP-Struktur ... wie Android sie liest`; libziparchive (aapt2/aapt 35.0.0): `Zip: Entry at offset zero has
  invalid LFH signature 0 ... failed opening zip: Invalid file.` (`libziparchive_praefix.txt`). In der Kette
  faengt aapt badging (und apksigner, unsigniert) - das Gate allein bildet libziparchive nicht vollstaendig nach.
- F_kopf_arabisch (Kopfzeile `# re15 assets ٣٦٠٣ ٣٥٦٦٧٨٢٧٧`): Gate rc 0 (KOPF_RE nutzt `\d`, das
  Groessenfeld dagegen `[0-9]`); das Geraet (sscanf `%ld`, android_glue.c:163) liest 0/0 -> nur Anzeige/Marker.

## 9. Laufprotokoll
- 23:32 Dossier angelegt; Bestand gelesen; Python-Schnappschuss 0 (`py_zustand_0_vorher.txt`, 121 Zeilen).
- 23:34 ref kopiert (sha256 514bebd5... = Archiv), K0 + Faelschungen, echtes Gate je Faelschung.
- 23:37-23:38 Selbsttest echtes Gate + 5 Mutanten (je 6-7 s), Mutanten gegen Faelschungen.
- 23:39 Signieren (Debug-Schluessel); 23:40 Schattenbaum; 23:40-23:43 Kette ohne P2DS, 0-Byte-P07G, check_tree.
