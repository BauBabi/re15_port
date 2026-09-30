# Gegenpruefung Runde 1 — Linse UMGEHUNG/ROBUSTHEIT (Stufe-4-Kette + Geraete-Entpacker N1)

Pruefer: Gegenpruefer UMGEHUNG/ROBUSTHEIT, Runde 1 (Sitzung 2026-09-30).
Baum: C:/workspace/git/reAi_v2/.claude/worktrees/r34a_android (Zweig r34a/android-gate).
Gegenstand: Commits `git log --oneline be8b60f3..HEAD` — Stufe-4-Kette (B1-B4, README;
Dossier android_gate_r4_kette.md) und Geraete-Entpacker N1 (Dossier android_entpacker_n1.md).
Regel: Ich aendere KEINE Werkzeuge/Quellen. Mutanten/Zusatzfaelle nur in Kopien unter build/r34a/pruefer_u1/.

Ziel: ein falsches Ergebnis erzwingen (Umgehung) oder einen verschluckten Fehler finden (Robustheit).

## 0. Stand / Fortschritt

- [ ] 0.1 Commit-Liste und Stand aufnehmen
- [ ] 1 Kette: veralteter Android-Satz (nur PC, abgelehnte APK, APK geloescht, --ohne-android, --only)
- [ ] 2 Kette: sha256-Kennung der APK im Split-Satz
- [ ] 3 Kette: make_package-PC-Pfad mit kaputtem Gate (Mutant) -> Abbruch?
- [ ] 4 Selbsttest: eigene neue Ein-Zeilen-Mutanten (auch v2-Manifest-Teil)
- [ ] 5 Format v2: Faelschungen
- [ ] 6 Entpacker: Code + PC-Unit-Test + Zusatzfaelle (Pfade, Abbruch/Neustart, JNI-Fehlerpfade, SHA-256)
- [ ] 7 Urteil


## 0.1 Ausgangslage (gelesen, nicht gemessen)

- HEAD `6b8e8cc8` (Zweig r34a/android-gate). Gegenstand `be8b60f3..HEAD`: Kette 42d72ec1 (B2), 3b677ea0 (B1/B3/B4,
  README), 7f269d04, 80559d2f (B1 nachgeschaerft), cb6617fd (Temp-Pfade), 93f85469 (Dossier); N1 eda2360b ..
  6b8e8cc8 (asset_abgleich.{h,c}, android_glue.c, build.gradle v2, Gate v2 248 Faelle + 116 innere Proben,
  Unit-Test 272 Pruefungen). Parallel arbeitet der Pruefer ECHTER LAUF im selben Baum (pruefer_echtlauf_r4_1.md) -
  ich committe nur meine Pfade, Emulator nur nach `adb devices`/qemu-Pruefung.
- Gelesen: beide Dossiers ganz, asset_abgleich.{h,c} ganz, android_glue.c ganz, Unit-Test ganz, build.gradle
  (stageAssets/writeAssetManifest), make_package.sh (Diff be8b60f3..HEAD + Zip-/Git-Abschnitt), apk_pruefen.sh
  (Schritte 0-6), apk_asset_gate.py (Kopf, Konstanten, `_pfad_fehler`, `manifest_lesen`, `manifest_pruefen`,
  Selbsttest-Harness `_fall_laufen`/`_fall_bewerten`/`selbsttest`, `main`), main.c um die Bootstrap-Aufrufe,
  Mutanten-Werkzeug der R2-Kampagne (`mutanten_teil.py`: "Nur Pruefcode wird mutiert (nicht Selbsttest/Fixture/main)").

## 0.2 Hypothesen aus dem Lesen (werden unten gemessen)

- H1 (B4): Das Urteil jedes Gate-Aufrufs ist NUR sein Exit-Code (make_package.sh `(( rc_selbst == 0 ))`, `case "$rc"
  in 0)`; apk_pruefen.sh `(( rc == 0 ))`). Den Exit-Code erzeugt derselbe Code, der geprueft werden soll. Eine
  leere/abgeschnittene Gate-Datei (Memory reai-v2-patchskript-truncate: das ist schon einmal passiert) oder ein
  Ein-Zeilen-Mutant im Ausgangspfad (`sys.exit(main())` -> `main()`, `RC_ABWEICHUNG = 0`, `return rc if ...` ->
  `return RC_GLEICH`) liefert fuer Selbsttest UND jede Pruefung rc 0 - niemand verlangt die OK-Zeilen.
  Die Mutanten-Kampagnen haben main/Selbsttest ausdruecklich ausgenommen.
- H2: build_android.sh uebernimmt `APK_GATE_DATEI` aus der Umgebung (apk_pruefen.sh `${APK_GATE_DATEI:-...}`), ohne
  den Pfad anzuzeigen - ein fremdes/altes Gate prueft dann still.
- H3 (B1): B1 haengt am exakten Namen `${NAME}_android.z*`. SHA256SUMS.txt und git add nehmen weiter JEDE Datei
  `${NAME}_*.z*` (z.B. `${NAME}_ANDROID.zip`, `${NAME}_android_alt.z01`, `${NAME}_android.apk.zip`) ungeprueft mit,
  auch mit `--ohne-android`.
- H4: neue Gate-Mutanten im v2-Teil (`manifest_lesen`/`_pfad_fehler` standen in keiner systematischen Kampagne,
  nur G1-G22 von Hand): `if not z:` -> `if not z.strip():` (Zeile nur aus Leerraum), Steuerzeichen-Grenze
  `c < 0x1f`.
- H5 (Format v2): Faelschungen gegen C-Leser und Gate gemeinsam; dazu Unicode-Gross/klein (Kelvin-Zeichen U+212A,
  Ae/ae) und NFC/NFD - die Dublettenregel faltet nur ASCII, der App-Speicher (ext4/f2fs casefold) faltet Unicode.
- H6 (Entpacker): Unicode-Faltung auf dem Geraet (H5), Reste die nie entfernt werden (weg nur mit gueltiger alter
  Liste; `.neu` nur fuer Pfade der neuen Liste), `unlink(pf_liste)` ohne Rueckgabepruefung, nach Fehlern laeuft das
  Spiel nach 3 s Meldung mit altem/halbem Baum weiter (main.c:3075 hat keine Rueckgabe), SHA-256 gegen Python
  (Zufallslaengen/Stueckelungen, Datei > 4 GiB).

## 0.3 Laufprotokoll (fortlaufend)

- ⛔ Eigener Regelverstoss, sofort geprueft: beim Anlegen von 0.1 lief EINMAL `python3 --version` (nur Versionsabfrage).
  `type -a python3` zeigt danach: erster Treffer ist der WindowsApps-Alias
  (`AppData/Local/Microsoft/WindowsApps/python3` -> PythonManager 26.3), er gab "Python 3.10.11" aus (bestehende
  Installation). Schnappschuss mit dem R4-Werkzeug `py_zustand.ps1` direkt danach
  (`build/r34a/pruefer_u1/py_zustand_nach_python3.txt`, 121 Zeilen) == R4-Endstand `py_zustand_2_ende.txt`
  (`diff` rc 0): kein neuer PythonCore-Schluessel, kein Startmenue-Eintrag, `LocalAppData\Python` unveraendert
  (neuester Eintrag `_cache/last_welcome.txt` 2026-09-29T15:39:13), kein pymanager/msiexec-Prozess. Ab hier nur
  `/c/Python310/python`.

## 1. H1 - der Selbsttest (B4) urteilt nur ueber den Exit-Code des geprueften Codes

Werkzeug `u1_mutanten.py` (Kopien unter `build/r34a/pruefer_u1/mut/`, je Mutant `diff` = genau 1 geaenderte Zeile;
G0/G4 sind Kuerzungen), `u1_selbsttests.sh` (Selbsttest direkt, Rueckgabe selbst abgefangen). Beleg
`selbsttests_h1h4_teil1.txt`:

| Gate | Selbsttest EXIT | gedruckte Aussage |
|---|---|---|
| ECHT (sha256 8f84f136...) | 0 (25 s) | `SELBSTTEST-OK: 248/248` |
| G0 leere Datei (0 B) | **0** (0 s) | nichts - 0 Byte Ausgabe |
| G4 abgeschnitten vor dem `main()`-Block (Anweisungsgrenze, 184185 B) | **0** (0 s) | nichts |
| G1 `sys.exit(main())` -> `main()` | **0** (30 s) | `SELBSTTEST-FEHLER: 234 von 248 Faellen falsch` |
| G2 `RC_GLEICH, RC_ABWEICHUNG, RC_FEHLER = 0, 1, 2` -> `0, 0, 2` | **0** (40 s) | `SELBSTTEST-FEHLER: 165 von 248` |
| G3 `return rc if rc in (RC_GLEICH, RC_ABWEICHUNG) else RC_FEHLER` -> `return RC_GLEICH` | **0** (50 s) | `SELBSTTEST-FEHLER: 165 von 248` |

make_package.sh (`(( rc_selbst == 0 ))`) und apk_pruefen.sh (`(( rc == 0 ))`) werten NUR diese Exit-Codes aus. Dieselben
Gates liefern auch fuer `--quellbaum`, `--paket` und die APK-Pruefung rc 0 (G0/G4: es laeuft gar nichts; G1-G3: jede
Abweichung endet mit 0). Die ganze Kette mit diesen Gates: Abschnitt 2.
Warum die Kampagnen das nicht sahen: `mutanten_teil.py` (R2) "Nur Pruefcode wird mutiert (nicht Selbsttest/Fixture/
main)", G1-G22 (N1) nur `manifest_lesen`/`manifest_pruefen`/`_pfad_fehler`, MU1-MU8 (R3) Pruefcode.
