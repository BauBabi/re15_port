# Runde 34 Android - Stufe 4 Nachbesserung (Runde 1)

Angelegt 2026-09-30 10:40. Auftrag: Gegenpruefer-Befunde pruefer_*_r4_1.md beheben (H1 hoch; H5, H3, H2 mittel; niedrige nach Moeglichkeit).


## 0. Stand / Plan

Gegenstand: Gegenpruefer-Befunde `pruefer_umgehung_r4_1.md` (H1 hoch; H5, H3, H2 mittel; V1/V2, U1-U4 niedrig) und
`pruefer_echtlauf_r4_1.md` (E1 niedrig = U2). Arbeitsordner (nicht versioniert): `build/r34a/nb/`. Ausgangsstand HEAD
`79ebd085` (+ Dossier-Commit `e777d796`). Python nur `/c/Python310/python` bzw. `release/python_finden.sh`.

- [x] 1 Befunde selbst nachmessen (H1, H2, H5 schnell; H3 per make_package in eigener Sandbox)
- [ ] 2 Bauplan
- [ ] 3 H1 + H2: Gate-Urteil aus Rueckgabe UND Ausgabe, Gate-Pin, keine Umgebungsvariable mehr
- [ ] 4 H3: SHA256SUMS.txt + git add aus einer Positivliste, fremde Versionsdateien -> Abbruch
- [ ] 5 H5 (+U3, V1/V2): Pfade nur druckbares ASCII, Segment <= 251 B - Gradle, Gate, Geraete-Leser, Tests
- [ ] 6 niedrig: U1, U2/E1 (Waisen), U4 (fail closed) im Entpacker
- [ ] 7 Nachweise: Selbsttest, Mutanten/Kette gegen die neuen Skripte, PC-Suite, Android-Bau, Emulator
- [ ] 8 Endstand

## 1. Befunde selbst nachgemessen (Stand HEAD, vor jeder Aenderung)

Werkzeug `build/r34a/nb/nb_repro.sh` (Mutanten mit dem Pruefer-Werkzeug `u1_mutanten.py`, Sandbox mit
`r4_sandbox_anlegen.sh` unter `build/r34a/nb/sb`; release/ und der Index des Arbeitsbaums unberuehrt). Logs
`build/r34a/nb/logs/repro/`, Zusammenfassung `build/r34a/nb/logs/repro_teil1.txt`.

| Befund | Messung | Ergebnis |
|---|---|---|
| H1 | Selbsttest G0 (0-Byte-Gate) | **EXIT 0**, 0 B Ausgabe |
| H1 | Selbsttest G1 (`sys.exit(main())` -> `main()`) | **EXIT 0**, letzte Zeile `== SELBSTTEST-FEHLER: 234 von 248 Faellen falsch` |
| H1 | Sandbox `build_android.sh --gate-only` Referenz-APK v0.8.19 (Liste v1) mit G0 als Gate | **EXIT 0**, `APK-PRUEFUNG-OK`, `ANDROID-GATES-OK` (6 s) |
| H2 | dasselbe mit ECHTEM Gate in release/, Umgebung `APK_GATE_DATEI=<G0>` | **EXIT 0**, `ANDROID-GATES-OK` |
| H5 | Gate `manifest_lesen` auf Listen mit je einem Paar Kelvin/K, NFC/NFD, ss/sz, Ae/ae | alle vier **angenommen** (0 Fehler) |

Code bestaetigt die Ursache: make_package.sh wertet nur `rc_selbst`/`rc_quelle`/`rc` aus (`(( rc_selbst == 0 ))`,
`case "$rc" in 0)`), apk_pruefen.sh nur `(( rc == 0 ))` bzw. `case`; `APK_GATE_DATEI` wird in apk_pruefen.sh Schritt 5 per
`${APK_GATE_DATEI:-...}` aus der Umgebung genommen. H3: siehe 1.1.
