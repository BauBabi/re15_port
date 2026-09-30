# Spur B — Hebetisch Irons Office (ROOM1150/1151): ABNAHME 1

Stufe: Abnahme 1 (unabhaengig, nach dem Bau). Pruefer hat den Code nicht geschrieben.
Auftrag: versuchen zu WIDERLEGEN, dass Spur B fertig ist.
Datum: 2026-09-30, 08:10-09:05. Baum: `.claude/worktrees/r34n_hebetisch` (Zweig `r34n/hebetisch`, gepruefter
Code-Stand = HEAD d1c2d8b0; Abnahme-Commits darauf).

## 0. Urteil

**ABGENOMMEN** — mit vier kosmetischen Punkten (§6), kein blocker, nichts wesentliches.

Jeder Nutzerpunkt aus AUFTRAG.md (zweiter Punkt) laeuft an der echten exe mit eigenen Eingabefolgen durch
(§3): kein Direktoeffnen mehr, der 11F0-Cursor erscheint und faehrt mit dem D-Pad, Druck daneben zeigt
"Nothing happened." ohne Laut, Druck auf die Kuppel unten rechts gibt den 11F0-Klick (RE2-Panel-SE) und
startet den unveraenderten Ablauf mit Sicherung/Granate. Die Gegenproben (§4: ROOM1151, Laden, Nein-Zweig,
zweite Sitzung, Inventar mit CROSS/START, Abbruch, Nachbar-Examine) halten. Suite 437/437 in einem Durchgang
(§2). Code ohne Rate-Konstanten (§5). Widerlegt habe ich nur Nebensaetze der Dokumentation und das Verhalten im
Nicht-Standard-Modus RE15_FPS=60 (§6).

## 1. Gelesener Stand (git log, Dossier, Gegenpruefung)

* `git log cf0e68ba..HEAD`: 26 Commits Spur B (01:57-05:26) + Cherry-pick d1c2d8b0 (local_build.sh-Kill-Fix,
  nur Bau-Skript/Skills/green.sh, kein Port-Code). Letzter Code-Commit 724d438e (05:02, nur Kopf-Kommentar).
* Dossier `B_hebetisch.md` §0-§9 gelesen, Gegenpruefung `B_hebetisch.gegenpruefung.md` (Urteil
  haltbar_mit_auflagen, Auflagen 1-9) gelesen; §9.1 hakt alle neun Auflagen ab — an der exe nachgeprueft
  (Auflage 1 Inventar: J2; Auflage 2 Lade-Weg: J3; Auflage 4 Abbruch: J2; Auflage 6 Text: J1).
* Code gelesen: `include/re15_hebetisch_cursor.h`, `engine/src/hebetisch_cursor_1150.c`,
  `platform/pc/src/hebetisch_cursor_pc.c`, Haken in `scd_vm.c` (op_for), `game_step_common.c` (GENERIC-Ausgabe),
  `scd_room_setup.c` + `main.c` (Install an beiden Raumaufbau-Stellen, Zeichnen nach `pc_draw_effects`),
  Riegel `probe_r34n_b_cursor.c` / `probes/r34n_b_hebetisch.cmake`, Integrations-Haken `test_r34n_b_cursor.cmake`.

## 2. Bau + Suite

* `local_build.sh configure` + `build` (08:18): `ninja: no work to do` — die exe von 05:02 IST der Code-Stand,
  `=== LOCAL-BUILD-OK (build)`.
* `local_build.sh test` (08:31-08:49, nach dem Kill-Fix, keine eigenen exe-Laeufe waehrenddessen):
  **`=== LOCAL-BUILD-OK (test) — Tests 437/437`**, 1069 s, in EINEM Durchgang. Alle 9 neuen gruen
  (`integration_r34n_b_cursor` 231 s), ebenso die Nachbarn im selben Raum: `integration_r30_granate_laden`,
  `_sicherung_laden`, `_irons_tisch_bild`, `_irons_tisch_licht` (`B_belege/abn1_suite.txt`).
* Nebenfund: der Bauer hatte einen DRITTEN Suite-Lauf (Ende 05:41, `build/r34n_b_suite3.log`: 436/437,
  `integration_r30_irons_tisch_licht` rot mit "Spielstand in ROOM1150 nicht geladen (exit=1)" = Namens-Kill-
  Signatur vor dem Kill-Fix), der im Dossier §9.4 fehlt. Im Lauf oben gruen (46,8 s) -> keine Regression,
  nur eine Dokumentationsluecke (§6 K2).

## 3. Nutzerpunkte an der echten exe

Alle Laeufe: echte exe dieses Baums als Kopie `re15_abn1.exe` (eigener Name; der gefixte local_build.sh beendet
nur `re15_pc` unter seinem Bauverzeichnis), beschleunigter Renderer, `RE15_WINDOW_SCALE=3`, `RE15_FRAMEDUMP`,
KEIN AUTOSHOT/SOFTWARE_RENDER, Eingaben per `RE15_INPUT_SCRIPT` wie ein Spieler (A = SQUARE, X = CROSS,
S = START, T = Dreieck, U/D/L/R = D-Pad). Werkzeug `re15_port/tools/r34n_b/abn1_lauf.sh` (neu, Abnahme).
Eigene Eingabefolgen, NICHT die des Dossiers (andere Fehldruck-Stellen, Rand-Druck, Druck auf das Podest,
andere Schliesstasten, Nein-Zweig). Alle Bilder angesehen.

| Lauf | Weg | Belege |
|---|---|---|
| J1 | Debug-Sprung 1150, Ton an: Tisch, Fehldruck LINKS, Text mit SQUARE zu, Fehldruck 5 px RECHTS NEBEN der Kuppel, Text mit CROSS zu, Kuppeldruck UNTEN (227,185), Sicherung Ja, Granate Ja, Fahrt zu Ende, ZWEITE Aktion, Kuppel, leere Faecher | `B_belege/abn1_j1_bogen_a.png`, `abn1_j1_bogen_b.png`, `abn1_j1_lupe_rand.png`, `abn1_j1_log.txt` |
| J2 | Debug-Sprung 1150, Ton an: Cursor, Inventar auf + mit CROSS zu, 9 Takte RIGHT, Inventar auf + mit START zu, 9 Takte LEFT, CROSS-Abbruch, erneute Aktion, Dreieck | `abn1_j2_bogen.png`, `abn1_j2_log.txt` |
| J3 | CONTINUE in ROOM1151 (Spielstand probe_r30_granate_karte), Ton an: Kuppel, Sicherung NEIN, Granate JA, erneute Aktion, Sicherung wieder angeboten, JA | `abn1_j3_bogen.png`, `abn1_j3_log.txt` |
| J4 | Debug-Sprung 1150, Spieler OSTSEITE des Tischs (-17400,-18500) Blick West, Aktion | `abn1_j4_schild_ost.png` |
| J5 | wie J1-Anfang, aber `RE15_FPS=60` (Nicht-Standard), zwei gleich lange SQUARE-Tipps | `abn1_j5_60fps.txt` |

### 3.1 Aktion am Tisch -> Cursor-Modus (Cursor erscheint, D-Pad bewegt, Spieler steht)

BESTANDEN. J1: Druck F230 -> `verlangt` F230, `aktiv` F241, Cursor ab F242 in der Bildmitte (160,119) ueber
Cut 4, Kuppel zu unten rechts (`abn1_j1_bogen_a.png` F248). Das Modell geht NICHT mehr von selbst auf: bis zum
Kuppeldruck F574 steht die Kuppel geschlossen (F248..F572). D-Pad: LEFT 15 Takte -> x -22554 (sx 121), RD/R ->
(272,172), LD/L -> (227,185) — genau 200 je VM-Takt (J2: 9 Takte RIGHT = -17754, 9 Takte LEFT = wieder -19554).
Spieler steht (`[walk]`-Zeilen F240..F570 unveraendert `pl pos=(-22250,0,-18500) rot=0`).

### 3.2 Druck ausserhalb der Kuppel -> "Nothing happened" (+ Klickton-Verhalten)

BESTANDEN. J1 F290 (Cursor links auf den Hochhaeusern) und F452 (Heisspunkt (272,172), 5 px rechts neben der
Huelle; Lupe `abn1_j1_lupe_rand.png`: Kreuzmitte = Heisspunkt, liegt sichtbar NEBEN dem Lavendel-Rand): je
`nichts`, Text "Nothing happened." tippt (F296 "Nothing", F464 "Nothing ha") und steht (F344, F500), eine Zeile
unten links, Glyphen korrekt. KEIN `[se]` zwischen Druck und Text (debug.log F290..F353 und F452..F515 ohne
Stimmen-Eintrag). Schliessen mit SQUARE (F353) oeffnet den Text NICHT erneut (kein zweites `nichts`),
Schliessen mit CROSS (F515) bricht NICHT ab (`abbruch=0`), Cursor danach frei (F536 an derselben Stelle, dann
weiter bewegt). Treffergrenze: (227,185) im unteren Podestteil trifft, (272,172) 5 px rechts daneben nicht —
die Huelle folgt dem gerenderten Achteck.

### 3.3 Druck auf der Kuppel (unten rechts) -> Klickton + bisheriger Ablauf (Deckel, Items)

BESTANDEN. J1 F574 SQUARE bei (227,185): `kuppel treffer=1 klick=1`, im selben Bild
`[se] Stimme: se=10 layer=0 vag=9 note=66 fine=57 ... pitch=0x5f3` (RE2-Panel-Klick), F584 Deckel teilen sich,
F704 Hub oben, Sicherung-Modal (Bild 722), `[sicherung] Yes: genommen, Flag (9,53)`, `[granate] Yes: genommen,
Flag (9,56)`, F1148 Parklage -20224, danach `[pri] cut=0` = Raumkamera (`abn1_j1_bogen_b.png`). Laute der
Fahrt wie vorher (sub04 Se_on 0x0C/0x0D, `se=12`/`se=13`).

### 3.4 Abbruchtaste / Wiederaktivierung

BESTANDEN. J2 F521 CROSS im Cursor -> `abbruch`, Raumkamera ab F524, Spieler frei; F584 erneute Aktion ->
`verlangt`/`aktiv` (Sitzung 2), Cursor wieder in der Mitte (`abn1_j2_bogen.png`). J1 zweite Sitzung nach der
Aufnahme: F1246 Aktion -> Cursor, F1313 Kuppel -> Fahrt mit LEEREN Faechern, kein Modal (F1460), zurueck in die
Raumkamera. Dreieck im Cursor (J2 F620): keine Wirkung. Die Festlegung "jede Aktion bringt den Cursor wieder"
(Dossier §4.4, sub04 setzt Slot 1 nie zurueck) ist begruendet und so gebaut.

### 3.5 Klickton = Generator-Klick ROOM11F0 (geladen/spielbar in 1150?)

BESTANDEN. J1/J3: der Kuppeldruck gibt in 1150 UND 1151 dieselbe Stimmenzeile wie der 11F0-Schalterdruck im
Dossier-Lauf m5 (`vag=9 note=66 fine=57 center=84 vol=127 pitch=0x5f3 (16397 Hz) -> SE-Stimme -16`). Die
negative Stimme ist der Direkt-Zweig `rec.voice < 0` (audio_pc.c:749ff., freier Mixer-Slot), also wirklich
abgespielt, nicht verworfen. Aufruf identisch mit 11F0 (scd_vm.c:4957 `re15_audio_re2_panel_se(RE15_PANEL_SE_KLICK)`
gegen hebetisch_cursor_1150.c:307). Beim Fehldruck kein Klick (wie 11F0 ausserhalb einer Zelle, Dossier m5).

## 4. Gegenproben

### 4.1 ROOM1151-Variante
BESTANDEN. J3 (CONTINUE 1151): Cursor F101, Kuppel F148 mit Klick, Sicherung NEIN -> `No/voll: nicht genommen`,
Granate JA -> Flag (9,56); zweite Aktion F877 -> Cursor, Kuppel F935 -> Sicherung wieder angeboten, JA -> Flag
(9,53); jede Fahrt endet in der Raumkamera (`abn1_j3_bogen.png`).

### 4.2 Laden/Speichern, Wiederbetreten
* Laden: J3 (CONTINUE, main.c-Install-Weg, Auflage 2) bestanden; dazu Suite `integration_r34n_b_cursor` A/B/C
  (CONTINUE 1150/1151) und die alten Lade-Riegel r30 gruen.
* Speichern: der Modus ist fluechtig (kein Bank-9-Bit, kein Speicherstandfeld) und waehrend des Modus
  unerreichbar — sub04 setzt (2,0) im Druckbild (@0x0F96, im Lauf `thread start ... first_op=0x22` direkt
  vor dem Se_on im selben Bild F230) und loescht es erst @0x10AA; der Modus endet vorher (Kuppel/Abbruch).
  Text-Id 20 kollidiert nicht mit der Speicherstelle 1150/1151 (re15_savepoint.c:47 msg 1, Port-Id 0xFE) und
  nicht mit der Item-Box (re15_itembox.c:63 msg 3).
* Wiederbetreten: kein eigener Tuerlauf gefahren. Begruendung: der Debug-Sprung geht durch denselben
  Raumwechsel wie eine Tuer (`re15_room_request_change` -> `scd_room_reenter` -> Install
  scd_room_setup.c:427); aus demselben Grund wie oben kann der Spieler den Raum nie mit Modus != AUS
  verlassen; die raumuebergreifenden Zustaende (Signatur-Cache an (raw, raw_size, Raum), TIM-Upload je Sitzung
  `s_hochgeladen != sitzung`) sind im Code an den Raum bzw. die Sitzung gebunden. Zwei Sitzungen im selben
  Besuch (J1, J2, J3) zeigen je frischen Start (160,119) und frischen Textur-Upload (`Sitzung 2`).

### 4.3 Tisch-Items unveraendert (Sicherung Bit 53, Granate Bit 56, Diary/Karte)
BESTANDEN. Sicherung/Granate: J1/J3 (Modal, Ja/Nein, Flags, leere Faecher, erneutes Angebot nach Nein).
Diary/Memory Card (Bits 54/55): Code von B unberuehrt; `integration_r30_irons_tisch_bild`/`_licht` gruen.

### 4.4 Nachbarverhalten des Originals unveraendert
BESTANDEN. J4: Aktion von der OSTseite des Tischs (nur Slot 4 @0x0DBA im Blick, nicht Slot 1) -> das Schild
"There's a small sign in one corner. It reads..." wie bisher, KEIN Cursor (`cursor.log` leer)
(`abn1_j4_schild_ost.png`). ROOM1150 main00 fuehrt Ereignis 4 nur an Slot 1 @0x0D7E (Slots 3/5/6 -> 6/7/8,
`scd_dump_room.py`), der Haken armiert nur (1150|1151, Ereignis 4, Thread gestartet). op_for-Haken im Zustand
AUS = ein Vergleich; in jedem anderen Raum liefert `halt_pc` NULL. Harness-Weg RE15_FIRE_AOT ohne Cursor:
Suite-Lauf D + unit_r34n_b_harness gruen.

## 5. Code gegen RE-Gate

Geprueft: jede Konstante in `re15_hebetisch_cursor.h`, `hebetisch_cursor_1150.c`, `hebetisch_cursor_pc.c`.
* Belegt mit Datei-Offset/EXE-Adresse: Raeume/Ereignis (@0x00D7E/@0x00D8C), Signatur + Halt (@0x0FB2..@0x0FC5,
  1151 @0x0F90), Aufraeumbytes (@0x109A..@0x10B5, Abstand 0xDA), Cursor-Start (@0x00E54), Typ-4 -900
  (@0x8002c24c), Schritt 200 (@0x012F6..@0x0131A), Tastenmasken (@0x01098/@0x010B0/@0x010C8/@0x010E0/@0x01106,
  Preset @0x80073dbc), Kamera Cut 10 (@0x01A0), Licht Cut 10 (@0x0718), Textbytes (ROOM1000 @0x00D22..@0x00D34,
  ROOM3001 @0x0199D), Maske 0xffff0000 (@0x80043098/@0x800430a4), Klick (ROOM2130 @0x01192), Huelle
  (Engine-Projektion, von unit_r34n_b_kuppel zur Laufzeit nachgerechnet).
* Als PORT-WAHL/NUTZER-VORGABE gekennzeichnet, mit Grund: Flanke statt gehalten (0x52-Pruefung @0x80042978),
  CROSS-Abbruch (kein gemaltes EXIT in Cut 4), Tiefenversatz 65536, Text-Wortlaut.
* Keine Rate-Worte ("plausibel", "interim", TODO, Platzhalter) in den neuen Dateien (grep leer). Keine Stubs.
  `d > 0 && d < 65535` ist dieselbe Wache wie msg_common.c:343, `z < 64`/OFX 160/OFY 120 wie main.c.
* Commit-Messages zitieren die Adressen (f19450c1 geprueft).
* Die Laufzeit-Wahrheit habe ich nicht nur gelesen: Treffergrenze, Klick-Stimme, Text-Glyphen, Parklage,
  Raumkamera und Items sind in §3/§4 am Artefakt gemessen.

## 6. Maengelliste

| # | Titel | Schwere | Beleg |
|---|-------|---------|-------|
| K1 | Mit `RE15_FPS=60` (Nicht-Standard, nur env) geht JEDER SQUARE/CROSS-Druck verloren, dessen erstes Bild gerade ist (50 %), unabhaengig von der Tastdauer — Dossier §9.8 Nr. 6 spricht nur von "sehr kurzen Tipps". Ursache: die Port-Wahl Flanke (`g_scd_pad_edge`, game_step_common.c:1104 je Bild ueberschrieben) wird von der VM nur in geraden Bildern gelesen (main.c:5382). 11F0 (gehaltenes 0x0040 + Zellenbit) ist davon nicht betroffen. Im 30-Bilder-Standard kein Effekt (J1-J3). | kosmetisch | `B_belege/abn1_j5_60fps.txt`: 0.1-s-Tipp ab F392 ohne Wirkung, gleicher Tipp ab F459 -> `nichts` F460 |
| K2 | Dossier §9.4 verschweigt den dritten Suite-Lauf des Bauers (05:41, 436/437, `integration_r30_irons_tisch_licht` rot, exit=1 vor dem Laden). Nachgefahren: gruen, Namens-Kill vor d1c2d8b0. | kosmetisch | `build/r34n_b_suite3.log`, `B_belege/abn1_suite.txt` |
| K3 | Dossier-Kopf (Zeilen 3-5) steht noch auf der Ermittlungsstufe: "Einzige Code-Zugabe: die Mess-Sonde ... Suite bleibt 428" — widerspricht §9 (Modul, 9 Tests, 437). | kosmetisch | `B_hebetisch.md:3-5` |
| K4 | `RE15_MIN_TESTS` bleibt 428, die Suite hat 437 (bewusst, §9.7 Nr. 7, gemeinsame Datei aller Spuren) — die Integration muss die Wache anheben, sonst faengt sie ein Wegbrechen der 9 neuen Tests nicht. | kosmetisch | `local_build.sh` Kopf "WER TESTS HINZUFUEGT, HEBT DIESE ZAHL MIT" |

Hinweise (kein Mangel):
* Motor-Laut ROOM1150-0x0A kommt beim Tischdruck, 11 Bilder vor dem Cursor; beim Kuppeldruck nur der RE2-Klick —
  Folge von "sub04 unveraendert", im Dossier §9.0 offen genannt, an der exe bestaetigt (J1 `se=10 vag=3` F230).
* "Nothing happened." traegt einen Punkt (Nutzer schrieb ohne) — mit Zensus begruendet (Dossier §3.5/Auflage 6).
* Integration: `scd_vm.c` bekommt von A, F und B je eine Include-Zeile direkt hinter `re15_ai_flavor.h`
  (trivialer Textkonflikt); `main.c`/`scd_room_setup.c` teilt B mit E (Install-Nachbarschaft). Android: neue
  Datei `platform/pc/src/hebetisch_cursor_pc.c` braucht einen frischen Configure (GLOB-Cache, Dossier §9.8 Nr. 5).

## 7. Belege (Dateien)

`B_belege/abn1_*` (Kontaktboegen aus 960x720-Framedumps, 320x240-Kacheln, 256 Farben; Log-Auszuege),
Laufwerkzeug `re15_port/tools/r34n_b/abn1_lauf.sh`; Rohdaten (nicht versioniert) `build/r34n_b_abn1/<lauf>/`,
Suite-Protokoll `re15_port/build/local_build_ctest.log` + `build/abn1_suite.log`.
