# Spur E — Abnahme 1 (unabhaengig, nach dem Bau)

Stand: IN ARBEIT (Zwischenstand, wird fortlaufend gefuellt). Pruefer: Abnahme-Agent, hat den Code
nicht geschrieben. Auftrag: versuchen zu WIDERLEGEN, dass Spur E fertig ist.

## 0. Urteil

(offen)

## 1. Gelesener Stand (git log, Dossier, Gegenpruefung)

* Zweig `r34n/dokumente`, HEAD `3030e61c` (doc 9.7 Suite 430/430). Bau-Commits: `16f19248` Dok 1,
  `34838629` Dok 2, `bbdd45b5` Dok 3, `2d7ede7b` Dok 4, `5e50910c`/`54abec03` Bild-Riegel + Fremd-Riegel,
  Bau-Fix `8e78e13d` (cherry-pick, `local_build.sh` beendet nur noch die eigene exe).
* Dossier `E_dokumente.md` gelesen (Abschnitte 0..9.10). Eine Datei `E_dokumente.gegenpruefung.md`
  gibt es NICHT (die Gegenpruefung starb am Limit, Dossier 9.0 sagt das selbst; der Bau hat
  stattdessen `selbstpruefung.py` gefahren, 56/57 PASS, eine Berichtigung nOmodel 1021).
* Nutzerwortlaut: `AUFTRAG.md` Z. 20..58; Zuteilung `VERTRAG.md` 1.1..1.5.

## 2. Eigener Bau + Suite-Zeile

(laeuft)

## 3. Nutzerpunkte aus AUFTRAG.md an der echten exe

Werkzeug: `E_belege/abnahme1/abn1_lauf.sh` (eigene exe-Kopie `re15_pc_abn1.exe` in
`build/r34n_e/abn1/mess`, CONTINUE von der Nutzerkarte Runde 30, Framedump bei RE15_WINDOW_SCALE=3).

### 3.0 Texte (Zeichen fuer Zeichen)

* Unabhaengig (eigene Extraktion aus `AUFTRAG.md`, CRLF, Zitatpraefix) gegen `E_texte/dok*.txt`:
  4/4 Texte und 4/4 Titel zeichengleich; die Nutzerzeilen der vier Texte sind reines ASCII
  (keine typografischen Apostrophe/Auslassungszeichen im Auftrag selbst).
* Die ausgelieferten Seiten `shared_assets/RE2/FILES/FILE26..29_*.TIM` selbst dekodiert (eigener
  4bpp-TIM-Leser) und Zeile fuer Zeile gelesen: alle Woerter, Satzzeichen, `...`, `'`, `:`,
  `4312`, `5632`, Absaetze (Leerzeilen Dok 2, Dok 3) stimmen. Leerzeile vor „Thanks!" faellt auf
  den Seitenkopf von p02 und entfaellt (Irons-Regel). Titelseiten in Versalien, Dok 1 zweizeilig.

### 3.1 Dok 1 — Police Officer's Final Diary Entry (ROOM1050, Leiche vor dem Rolltor)

* Lauf `a_dok1`: CONTINUE -> Sprung 1050, Stand (14300,-6750) Blick Ost, 1 s GEHEN (Kollision haelt
  bei x 15232), VIERECK -> Leser Satz 26 (st 7/3, Seiten 0..4/4), 4x RECHTS bis EXIT, KREUZ ->
  Meldung „The Police Officer's Final Diary Entry has been filed.", VIERECK -> Welt. Danach echter
  Speicher-Durchgang (`RE15_SAVE_TEST_AGAIN`) auf Kartenplatz 4: `[save] saved (room 1050)`.
* Lauf `c_dok1_laden`: CONTINUE von genau dieser Karte Platz 4 -> `resumed in room 1050`, KEINE Zeile
  `[dokumente] Boot-Weg` (Bit 57 geladen), Schoss leer (Boot-Weg), danach Sprung 1050 (Tuer-Weg
  `scd_room_reenter`) -> Schoss leer; FILE-Liste Zeile 0 „Police Officer's Final Diary Entry"
  (`liste0=1`). Beleg: `E_belege/abnahme1/a1_dok1_schoss_vorher_laden_wieder.png`.

(weiter offen)
