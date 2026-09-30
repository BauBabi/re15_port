# Spur B — Hebetisch Irons Office (ROOM1150/1151): ABNAHME 1

Stufe: Abnahme 1 (unabhaengig, nach dem Bau). Pruefer hat den Code nicht geschrieben.
Auftrag: versuchen zu WIDERLEGEN, dass Spur B fertig ist.
Datum: 2026-09-30. Baum: `.claude/worktrees/r34n_hebetisch` (Zweig `r34n/hebetisch`, HEAD d1c2d8b0).

## 0. Urteil

(offen — wird am Ende gesetzt)

## 1. Gelesener Stand (git log, Dossier, Gegenpruefung)

* `git log cf0e68ba..HEAD`: 26 Commits Spur B (01:57-05:26) + Cherry-pick d1c2d8b0 (local_build.sh-Kill-Fix,
  nur Bau-Skript/Skills/green.sh, kein Port-Code). Letzter Code-Commit 724d438e (05:02, nur Kopf-Kommentar).
* Dossier `B_hebetisch.md` §0-§9 gelesen, Gegenpruefung `B_hebetisch.gegenpruefung.md` (Urteil
  haltbar_mit_auflagen, Auflagen 1-9) gelesen; §9.1 hakt alle neun Auflagen ab.
* Code gelesen: `include/re15_hebetisch_cursor.h`, `engine/src/hebetisch_cursor_1150.c`,
  `platform/pc/src/hebetisch_cursor_pc.c`, Haken in `scd_vm.c` (op_for), `game_step_common.c` (GENERIC-Ausgabe),
  `scd_room_setup.c` + `main.c` (Install an beiden Raumaufbau-Stellen, Zeichnen nach `pc_draw_effects`),
  Integrations-Haken `test_r34n_b_cursor.cmake`.

## 2. Bau + Suite

* `local_build.sh configure` + `build` (08:18): `ninja: no work to do` — die exe von 05:02 IST der Code-Stand
  (letzte Code-Aenderung davor), `=== LOCAL-BUILD-OK (build)`.
* Suite: (laeuft, Zeile folgt)

## 3. Nutzerpunkte an der echten exe

Alle Laeufe: echte exe dieses Baums als Kopie `re15_abn1.exe` (eigener Name; local_build.sh beendet nur
`re15_pc` unter seinem Bauverzeichnis), beschleunigter Renderer, `RE15_WINDOW_SCALE=3`, `RE15_FRAMEDUMP`,
KEIN AUTOSHOT/SOFTWARE_RENDER, Eingaben per `RE15_INPUT_SCRIPT` wie ein Spieler (A = SQUARE, X = CROSS,
S = START, T = Dreieck, U/D/L/R = D-Pad). Werkzeug `re15_port/tools/r34n_b/abn1_lauf.sh` (neu, Abnahme).
Eigene Eingabefolgen, NICHT die des Dossiers (andere Fehldruck-Stellen, Rand-Druck, andere Schliesstasten).

| Lauf | Weg | Belege |
|---|---|---|
| J1 | Debug-Sprung 1150, Ton an: Tisch, Fehldruck LINKS, Text mit SQUARE zu, Fehldruck 5 px RECHTS NEBEN der Kuppel, Text mit CROSS zu, Kuppeldruck UNTEN (227,185), Sicherung Ja, Granate Ja, Fahrt zu Ende, ZWEITE Aktion, Kuppel, leere Faecher | `B_belege/abn1_j1_bogen_a.png`, `abn1_j1_bogen_b.png`, `abn1_j1_lupe_rand.png`, `abn1_j1_log.txt` |
| J2 | Debug-Sprung 1150, Ton an: Cursor, Inventar auf + mit CROSS zu, Cursor 9 Takte, Inventar auf + mit START zu, Cursor 9 Takte zurueck, CROSS-Abbruch, erneute Aktion, Dreieck | `abn1_j2_bogen.png`, `abn1_j2_log.txt` |
| J3 | CONTINUE in ROOM1151 (Spielstand probe_r30_granate_karte), Ton an: Kuppel, Sicherung NEIN, Granate JA, erneute Aktion, Sicherung wieder angeboten, JA | `abn1_j3_bogen.png`, `abn1_j3_log.txt` |

### 3.1 Aktion am Tisch -> Cursor-Modus (Cursor erscheint, D-Pad bewegt, Spieler steht)

BESTANDEN. J1: Druck F230 -> `verlangt` F230, `aktiv` F241, Cursor ab F242 in der Bildmitte (160,119) ueber
Cut 4, Kuppel zu unten rechts (`abn1_j1_bogen_a.png` F248). Das Modell geht NICHT mehr von selbst auf: bis zum
Kuppeldruck F574 steht die Kuppel geschlossen (F248..F572). D-Pad: LEFT 15 Takte -> x -22554 (sx 121), RD/R ->
(272,172), LD/L -> (227,185) — genau 200 je VM-Takt, keine Randgrenze. Spieler steht (`[walk]`-Zeilen F240..F570
unveraendert `pl pos=(-22250,0,-18500) rot=0`).

### 3.2 Druck ausserhalb der Kuppel -> "Nothing happened" (+ Klickton-Verhalten)

BESTANDEN. J1 F290 (Cursor links auf den Hochhaeusern) und F452 (Heisspunkt (272,172), 5 px rechts neben der
Huelle, das Kreuz sichtbar NEBEN der Kuppel, `abn1_j1_lupe_rand.png`): je `nichts`, Text "Nothing happened."
tippt (F296 "Nothing", F464 "Nothing ha") und steht (F344, F500), eine Zeile unten links. KEIN `[se]` zwischen
Druck und Text (debug.log: kein Stimmen-Eintrag F290..F353 und F452..F515). Schliessen mit SQUARE (F353) oeffnet
den Text NICHT erneut (kein zweites `nichts`), Schliessen mit CROSS (F515) bricht NICHT ab (`abbruch=0`),
Cursor danach frei (F536 an derselben Stelle, dann weiter bewegt).

### 3.3 Druck auf der Kuppel (unten rechts) -> Klickton + bisheriger Ablauf (Deckel, Items)

BESTANDEN. J1 F574 SQUARE bei (227,185) (unterer Teil, Podestbereich): `kuppel treffer=1 klick=1`, im selben Bild
`[se] Stimme: se=10 layer=0 vag=9 note=66 fine=57 ... pitch=0x5f3` (RE2-Panel-Klick), F584 Deckel teilen sich,
F704 Hub oben, Sicherung-Modal (Bild 722), `[sicherung] Yes: genommen, Flag (9,53)`, `[granate] Yes: genommen,
Flag (9,56)`, F1148 Parklage -20224, danach `[pri] cut=0` = Raumkamera (`abn1_j1_bogen_b.png`).

### 3.4 Abbruchtaste / Wiederaktivierung

BESTANDEN. J2 F521 CROSS im Cursor -> `abbruch`, Raumkamera ab F524, Spieler frei; F584 erneute Aktion ->
`verlangt`/`aktiv` (Sitzung 2), Cursor wieder in der Mitte (`abn1_j2_bogen.png`). J1 zweite Sitzung nach der
Aufnahme: F1246 Aktion -> Cursor, F1313 Kuppel -> Fahrt mit LEEREN Faechern, kein Modal (F1460), zurueck in die
Raumkamera. Dreieck im Cursor (J2 F620): keine Wirkung.

### 3.5 Klickton = Generator-Klick ROOM11F0 (geladen/spielbar in 1150?)

BESTANDEN. J1/J3: der Kuppeldruck gibt in 1150 UND 1151 dieselbe Stimmenzeile wie der 11F0-Schalterdruck im
Dossier-Lauf m5 (`vag=9 note=66 fine=57 center=84 vol=127 pitch=0x5f3 (16397 Hz) -> SE-Stimme -16`). Die
negative Stimme ist der Direkt-Zweig `rec.voice < 0` (audio_pc.c:749ff., freier Mixer-Slot), also wirklich
abgespielt, nicht verworfen. Beim Fehldruck kein Klick (wie 11F0 ausserhalb einer Zelle).

## 4. Gegenproben

### 4.1 ROOM1151-Variante
BESTANDEN. J3 (CONTINUE 1151): Cursor F101, Kuppel F148 mit Klick, Sicherung NEIN -> `No/voll: nicht genommen`,
Granate JA -> Flag (9,56); zweite Aktion F877 -> Cursor, Kuppel F935 -> Sicherung wieder angeboten, JA -> Flag
(9,53); jede Fahrt endet in der Raumkamera (`abn1_j3_bogen.png`).

### 4.2 Laden/Speichern, Wiederbetreten
Laden: J3 (CONTINUE, main.c-Install-Weg) bestanden. Wiederbetreten: (offen)

### 4.3 Tisch-Items unveraendert (Sicherung Bit 53, Granate Bit 56, Diary/Karte)
Sicherung/Granate: J1/J3 bestanden (Modal, Ja/Nein, Flags, leere Faecher, erneutes Angebot nach Nein).

### 4.4 Nachbarverhalten des Originals unveraendert
(offen)

## 5. Code gegen RE-Gate

(offen)

## 6. Maengelliste

| # | Titel | Schwere | Beleg |
|---|-------|---------|-------|

## 7. Belege (Dateien)

`B_belege/abn1_*` (Bilder 320x240-Kacheln aus 960x720-Framedumps, 256 Farben), Laufwerkzeug
`re15_port/tools/r34n_b/abn1_lauf.sh`; Rohdaten (nicht versioniert) `build/r34n_b_abn1/<lauf>/`.
