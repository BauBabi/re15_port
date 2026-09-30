# Gegenpruefung UMGEHUNG/ROBUSTHEIT, Runde 2 (Stufe-4-Kette B1-B4 + Geraete-Entpacker N1)

Pruefer: Linse UMGEHUNG/ROBUSTHEIT, Runde 2. Baum .claude/worktrees/r34a_android, Zweig r34a/android-gate.
Gegenstand: git log --oneline be8b60f3..HEAD (Kette B1-B4 + README, Entpacker N1, Nachbesserung).
Ich aendere KEINE Werkzeuge/Quellen; Mutanten/Zusatzfaelle nur in Kopien unter build/r34a/pruefer_u2/.

Stand: angelegt 2026-09-30T11:56:50+02:00 - Abschnitte folgen.

## 0. Stand / Plan

- [x] 0.1 Ausgangslage (gelesen)
- [ ] 1 Selbsttest gegen NEUE Ein-Zeilen-Mutanten (v2-Teil, Quellpfade, Ausgabe-/Zaehlcode), Ueberlebende mit Urteil
- [ ] 2 Urteil (gate_urteil) / Pin: Taeuschung durch Ausgabe, Umgebung
- [ ] 3 Format v2: Korpus Gate == Geraete-Leser (neue ASCII-/Segmentregel), Regel-Luecken
- [ ] 4 Entpacker: Waisen (Loeschen), Pfade/Puffer, Abbruch/Neustart, Fehlerpfade (JNI, verschluckt), SHA-256
- [ ] 5 Kette: veraltete Saetze (alle Varianten), fremde Namen, APK getauscht/geloescht, Split-Satz-sha256
- [ ] 6 Urteil

## 0.1 Ausgangslage (gelesen, nicht gemessen)

- HEAD beim Start `8cbf88d6` (Abschluss Nachbesserung R4-1), mein Dossier-Commit `41d10775`. Gegenstand `be8b60f3..HEAD`:
  Kette R4 (B1-B4, README), Entpacker N1, Gegenpruefung R4-1 (meine Linse: `nicht haltbar` - H1 hoch; H5, H3, H2 mittel;
  V1/V2, U1-U4 niedrig) und Nachbesserung R4-1 (`android_r4_nachbesserung.md`: alle behoben). Parallel im selben Baum:
  Pruefer ECHTER LAUF R2 (`pruefer_echtlauf_r4_2.md`, sein Emulator `emulator-5586` lief beim Start) - ich committe nur
  meine Pfade, Emulator nur nach `adb devices`/qemu-Pruefung, eigene Bau-/Sandbox-Ordner unter `build/r34a/pruefer_u2/`.
- Gelesen (HEAD): asset_abgleich.{h,c} ganz (SHA-256, Leser, Pfadregel, Plan, Waisen), android_glue.c ganz (Bootstrap,
  entpacken, liste_schreiben, fehler_halten, Waisen-Aufruf, weg-Schleife), apk_pruefen.sh ganz (gate_pin_pruefen,
  gate_festhalten, gate_urteil, gate_laufen, apk_pruefen 0-6), make_package.sh (Gate-Kopie, Selbsttest, Quellbaum, APK,
  B1, fremde_versionsdateien, verify_split, verify_apk_im_zip, Zippen, Positivliste, Git), python_finden.sh,
  build.gradle writeAssetManifest, Gate: `_pfad_fehler`, `manifest_lesen`, `manifest_pruefen`, `quelldateien`,
  `quellpfade_pruefen`, `pruefen`, `paket_pruefen`, `nur_quellbaum`, `_befunde_ausgeben`, `_widerspruch_pruefen`,
  Selbsttest-Harness (`_fall_laufen`, `_innere_proben`, `_MANIFEST_PROBEN` 69, `_QUELLPFAD_PROBEN` 5, `selbsttest`, `main`),
  Mutanten-Werkzeug der R2-Kampagne (`nachbesserung_r2_belege/mutanten_teil.py`: 449 Mutanten auf Gate v6, 09-30 03:23 -
  also VOR N1/NB: `manifest_lesen`, `_pfad_fehler`, `quellpfade_pruefen` und der v2-Teil von `manifest_pruefen`/`pruefen`
  liefen nie durch eine systematische Kampagne, nur G1-G22 (N1) und 11 Hand-Mutanten (NB) von Hand).

## 0.2 Hypothesen (werden unten gemessen)

- Y1 Selbsttest-Deckung: systematische Ein-Zeilen-Mutanten (Klassen der R2-Kampagne) in `_pfad_fehler`, `manifest_lesen`,
  `manifest_pruefen`, `quellpfade_pruefen`, `quelldateien`, `pruefen`, `_befunde_ausgeben`, `_widerspruch_pruefen`
  ueberleben Selbsttest 258 + 132. Der Pin schuetzt NICHT gegen einen Fehler, den jemand bei einer gewollten
  Gate-Aenderung einbaut und dann (wie vorgeschrieben) neu pinnt - dann ist der Selbsttest das einzige Netz.
- Y2 Urteil: `gate_urteil` laesst sich mit einer stimmigen Ausgabe erfuellen, obwohl das Gate etwas fand, das die
  Zaehlung nicht aendert; oder Zeilen aus Pfadnamen (Zeilenumbruch in einem APK-Namen) mischen sich ins Urteil.
- Y3 Umgebung: `RE15_PYTHON`, `RE15_APK_SIGNER_SHA256`, `PYTHONPATH`/`sitecustomize` steuern Interpreter/Signer/Module
  des Gates (Grenze, sofern angezeigt/dokumentiert).
- Y4 Format v2: Gate == Geraete-Leser auch unter der neuen ASCII-/Segmentregel; Regel-Luecke: ein ORDNER-Segment
  `X.neu` neben einer Datei `X` besteht Gradle, Gate und Leser, aber der Entpacker kann `X` danach nie mehr schreiben
  (seine Zwischendatei `X.neu` ist ein Ordner -> EISDIR) - Geraet dauerhaft fail closed, Gate gruen (Klasse U3).
- Y5 Waisen (neuer Loesch-Code): Loeschen ausserhalb der Baeume, gelisteter Dateien, ueber lange Namen/Tiefe/Symlinks/
  Gross-klein; unlesbare Ordner nur Warnung.
- Y6 Fehlerpfade: verschluckte Fehler (`unlink` der weg-Pfade ohne Pruefung/Meldung), `fehler_halten` ohne Renderer,
  ein Weg zurueck nach main() ohne vollstaendigen Baum.
- Y7 Kette: veraltete Saetze in allen Varianten (auch der PC-Satz der ANDEREN Plattform: "frueherer Lauf, nicht neu
  geprueft" - Asymmetrie zu B1), fremde Namen (Gross/klein, Zusatzendungen), APK getauscht/geloescht, sha256 im Split-Satz.
- Y8 SHA-256 gegen Testvektoren und grosse Dateien (Stand HEAD).
- Y9 Abbruch + Neustart mit dem neuen Waisen-Schritt (halbe Liste, .neu-Reste).

## 0.3 Laufprotokoll

