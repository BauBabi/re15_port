# Runde 36 — Ressourcen-Vertrag und Arbeitsregeln (verbindlich fuer alle Spuren)

Zweck: die Spuren bauen parallel in eigenen Arbeitsbaeumen (`.claude/worktrees/r36_<spur>`, Zweig
`r36/<spur>`, Basis master = Runde-36-Auftrag auf v0.8.23 3ec5b1f8). Jede Spur nimmt NUR die hier
zugeteilten Plaetze. Wer mehr braucht: im eigenen Dossier begruenden, aus dem eigenen Reservebereich,
NIE aus dem einer anderen Spur. Dossier je Spur: `analysis/befunde_runde36/<SPUR>_<thema>.md` (ZUERST
anlegen, fortlaufend schreiben, frueh committen — das Sitzungslimit beendet alle Agenten gleichzeitig).

## 0. Die Spuren (Auftrag = AUFTRAG.md, woertlich)

| Spur | Thema | Auftrags-Punkte |
|---|---|---|
| A | klettern | ⛔ BLOCKER: auf die verschiebbaren Leitern/Tritte (z.B. ROOM10F0) kann nicht mehr geklettert werden — Regression seit v0.8.21, bisecten (v0.8.21 9362dd7e -> v0.8.22 87cc8575 -> v0.8.23 3ec5b1f8), Ursache belegen, beheben, Pin |
| B | kleidung | Leons Kleidung hat keine Blutflecken/Risse mehr bei Warning/Danger — Regression, bisecten, beheben, Pin |
| C | schuss | Grenze 1: allgemeiner Schusspfad ohne Sichtlinie (RE1.5 FUN_8001b9b4 via FUN_80011f50 @0x80012168-70) und ohne RE2-Fresser-Sperre (+0x1D3/+0x10E, RE2 @0x80041270-9c) — Python trifft ueber den Konferenztisch und fressende Zombies; fuer ALLE Schusswaffen beheben. Dazu: Blut bei getroffenen Zombies manchmal an der falschen Stelle; Frage: Super Redhawk laesst Kraehen komplett zerplatzen/verschwinden — ist das in RE2 so? (belegen; wenn nicht: RE2-Verhalten bauen) |
| D | granate | Normale Handgranate: KEIN Feuer-Effekt an Zombies, nur Zerplatzen der Koerperteile; Feuer nur bei der Brandgranate (incendiary). Granate wird manchmal nicht ausgeloest/geworfen |
| E | gegnerzahl | Grenze 2: Port fuehrt 15 Gegner je Raum, Original 20 (FUN_80037250 @0x800372b4 `sltiu 0x14`; Sce_em_set @0x800420dc ohne Grenzpruefung): RE15_ACTOR_MAX anheben, alle Nutzer zensieren, ROOM1040/1030 Records 15..19 erscheinen; Voll-Suite |
| F | zombie-ki | RE2-Zombies bleiben ewig stehen, obwohl Leon fast vor ihrer Nase steht (auch im selben Raum) — gegen RE2 messen (Regression? Sichtfilter Runde 35 H/J?), beheben. Monkeys ROOM11C0 oft blockiert — verbessern (am Original belegen). Monkeys: Blutlache laeuft nach dem Tod nicht aus |
| G | tueren | Tuersequenz-Modelle Fahrstuhl ROOM1080 und grosse Tuer ROOM2090 (beide in RE2 vorhanden) korrekt bauen; Tuer-Textur ROOM10C0<->10D0 "billig"; Tueren, die mit Karte o.ae. geoeffnet werden: in RE2 erst "piep", dann Unlock-Ton — bei uns nicht |
| H | ziel1190 | Zielscheiben-Texte gehoeren HINTEN an die Zielscheiben (beim Anklicken), nicht vorne an die Knoepfe; bei Elza (ROOM1191) sind es die anderen zwei Scheiben -> Texte vertauscht. Dazu Frage: Kiste in ROOM1090 — Verwendungszweck? (am RDT/Original belegen) |
| I | inventar | Memory Cards: beim Speichern Anzahl -1; Memory Cards oder Handgranaten bei 0 aus dem Inventar entfernen. Debug-Menue Raumsprung: bei Elza in die "1"-Raeume (z.B. ROOM1071), nicht in Leons |
| J | szenen-leon | ROOM11B0 Cutscene (Leon/Ada/Marvin verlassen Stage 1): (1) Container-Schieben mit Marvin (Animation + gemeinsames Schieben) klappt nicht; (2) Erweiterung am Ende — **geaendert 14:00 (Nachtrag AUFTRAG.md): Marvin schliesst NUR die Werkstatt-Tuer ab, NICHT die Parkplatz-Tuer 11B0->11C0** — Marvin "Hold on", rennt zur Innenseite der Werkstatt-Tuer, Zuschliess-Animation + Zuschliess-Ton (RE2 Robert Kendo Spielanfang), Tuer bleibt zu (gleiche Meldung wie Elzas Szenario "The door is locked." ROOM11B1 msg 12), zurueck zu Leon/Ada: "I just want to make sure we don't have any unwanted followers."; die Werkstatt-Tuer danach mit Elzas Text (Bit 84). Die Parkplatz-Tuer bleibt unberuehrt (nur Spur Q retypt sie, Schluessel-Sperre). ROOM10F0-Szene: Kamera bleibt beim Abgang von Marvin/Ada auf Leon, nur Tuer-zu-Ton am Ende (kein "Ploppen"). Irons: sobald tot (9,73), kein Kopf-Tracking hinter Leon mehr |
| K | szene-1150 | Irons-Todesszene/Montage: Tuerknall ist nur ein Tuer-zu-Ton — echten Tuerknall/Einbruch-Ton aus RE2 (RE2_SOUND_LISTE.md: Door sounds ROOM207/209/20A, Zombies breaking through wood ROOM205/112/106); 11C0-Schnitt: Marvin und Ada sprechen MITEINANDER (zugewandt, nicht ins Leere); Reihenfolge je Montage-Schritt: erst Knall, DANN Zombies spawnen, DANN Kamerawechsel (nicht Kamera vor Spawn) |
| L | elza | Elza (xxx1-Raeume): ROOM1121 Fenster von Anfang an kaputt (Schaden-Overlay aus Spur M r35) + eine Kraehe mehr; ROOM1151 Irons von Anfang an tot auf der Liege, Anklicken: "He has already passed..." (NUTZER-VORGABE, Original msg 2 lebend); ROOM1211 Kendo-Befreiung: Unlock-Ton beim Aufschliessen; ROOM1021 Kendo erschiesst Roy: Schuss-Toene + Roys Sterbe-/Hinfall-Ton; ROOM1101: waehrend Roy bewusstlos wird Voiceover `synchro/STAGE1/room1011/main16.5.wav` abspielen (Datei liegt in room1011!) + Untertitel dafuer anlegen |
| M | karte | Treppenhaus 10A0 B1: Treppen fehlen; ROOM11D0: Marker steht nach der Tuer in der Mitte statt am Eingang; ROOM1090: zwei getrennte Bereiche (Gang mit Luke / Westen-Raum) sauber getrennt; Tuer ROOM11C0 um 90 Grad verdreht; kleine Tuerbereiche ROOM1110 vorab "besucht"; Fahrstuhl-Etagen vorab "besucht" |
| N | karten-items | RE2-Lageplan-Weltmodell (extracted_re2_items karte01/karte02) als aufsammelbare Karte: 3F in ROOM1150 (map3F.bmp), 2F ROOM10D0 (map2F.bmp), 1F ROOM1030 (map1F.bmp), B2 ROOM1200 (mapB2.bmp), B1 ROOM11D0 (mapB1.bmp); Aufnehmen schaltet die Etagenkarte frei wie in RE2 (ganze Etage sichtbar, mit Text) |
| O | pri | ROOM10F0 Cut 2: neue PRI-Maske des Nutzers pri/STAGE1/10F0/02_02.png (Absperrung ueberdeckt Marvin) einbauen; ROOM1100: Leon wird von halbtransparentem Boden ueberdeckt (Marker-Datei fehlt — selbst messen, alle Cuts) |
| Q | schluessel | Leon: Tuer ROOM11B0 -> ROOM11C0 (Parkplatz) bleibt verschlossen, bis der Parking-Lot-Schluessel in ROOM10F0 gefunden ist (RE2-Item-Modell, das am ehesten nach Parkplatz-Schluessel aussieht); Elza: Prison Key NICHT mehr in ROOM1011, sondern an derselben Stelle in ROOM10F1 (RE2-Modell fuer einen Gefaengnis-Schluessel) — beide Spuren teilen EINE Position, Q legt sie fest. Story-Hintergrund (Nachtrag 14:00, nur Dossier): Kendo/John sitzt wegen des sabotierten Funkraums in Untersuchungshaft, die Beamten entzogen Roy die Befugnis — deshalb liegt der Schluessel im Communications Room; KEIN leerer Haken fuer Leon. Spur J retypt die Parkplatz-Tuer NICHT mehr (nur Werkstatt-Tuer) — der frueher vermerkte Slot-Konflikt entfaellt |
| P | story | (Analyse-Teil erledigt, `analysis/story_leon_elza/`.) BAU seit 2026-10-10 14:00 (Nachtrag AUFTRAG.md): (P1) Roys Leiche in Leons ROOM1020 (Record @0x01DEE, Text msg 0) erst NACH Einschalten des Stroms ((4,0xF3)/(4,0x06) — belegen, welches Bit der Port fuer "Strom an" fuehrt); (P2) Leon ROOM1210: Text-AOT an Kendos/Johns Zelle "The cell door is open..." (Position = Zellentuer aus ROOM1211 msg 10-AOT); (P3) Elza ROOM1091: Text-AOT am ausgebrannten Schuppen ("burnt-out shed"; Position = Leons Feuer-/Loesch-AOT in ROOM1090); (P4) Text-Sweep "Leon und Elza sind sich NIE begegnet": jede Textstelle beider Szenarien, die eine Begegnung/Bekanntschaft andeutet, per re15_msg_install_text-Override nach dem Raumladen umschreiben (bekannt: ROOM30C1 msg 2-6, ROOM6030 msg 1-5, ROOM6031 msg 0-2, ROOM3060 msg 1-8 im Leon-RDT — Sweep ueber texte_leon.md/texte_elza.md: "Elza" bei Leon, "Leon" bei Elza, "separated", "met", Szenen-Subs mit beiden Sprechern); alt->neu-Tabelle im Dossier, Wortlaut = PORT-WAHL (Story), Szenenfunktion erhalten |

## 1. Zuteilung

### 1.1 Persistente Zustandsbits — Bank 9 (gespeichert mit g_game.flags)
Belegt (Zensus 2026-10-10, re15_port/include/*.h): 53-63 (Runde 34), 65 (Ada-Ruf), 71/72 (K r35), 73-76
(L r35), 79 (Fenster 1120), 80/81 (F r35). Frei: 64, 66-70, 77, 78, 82-255.

| Bits | Spur | Verwendung |
|---|---|---|
| 82, 83 | Q | 82 = Parking-Lot-Schluessel genommen (Leon 10F0), 83 = Prison Key genommen (Elza 10F1) |
| 84, 85 | J | 84 = Marvin hat die Werkstatt-Tuer in 11B0 verschlossen (Szene gelaufen; seit 14:00 NUR diese Tuer), 85 Reserve |
| 86 | L | Reserve (Elza-Zustaende bevorzugt aus dem Szenario-Flag ableiten) |
| 87-91 | N | 87 = Karte 3F, 88 = 2F, 89 = 1F, 90 = B2, 91 = B1 genommen/freigeschaltet |
| 92 | P | Reserve fuer die Story-Spur (P1 haengt am ORIGINAL-Strom-Bit, braucht kein eigenes; nur mit Begruendung im Dossier) |
| 93-95 | — | Reserve (nur mit Begruendung im Dossier) |

Vorhandene Flags duerfen GELESEN werden ((3,94) erste Irons-Szene, (9,71) 10F0-Szene gesehen, (9,73) Irons
tot, (9,79) Fenster 1120 kaputt, (4,64) Parkplatz erreicht, Szenario-/Charakter-Flag des Spielers).

### 1.2 Portseitige Nachrichten-IDs (re15_msg_install_text; IDs < 32)
RDT-Nachrichtenzahl (rdt_msgdump.py, 2026-10-10): 11B0=16, 11B1=15, 10F0=6, 10F1=6, 1150=15 (20..31 r35
vergeben), 1151=4, 1021=15, 1101=6, 1211=11, 1121=9, 1190=6 (6..11 r35 H), 1191=6, 10D0=28, 10D1=12,
1030=18, 1031=24, 1200=0, 1201=0, 11D0=5, 11D1=5, 1011=24, 1090=10, 1091=0, 1100=6.

| Raum | Spur | IDs |
|---|---|---|
| ROOM11B0 | J | 20..27 ("Hold on", "I just want to make sure...", Tuer-zu-Texte = Elzas msg 12 Bytes uebernehmen) |
| ROOM11B0/11B1 | Q | 28..29 (Parkplatz-Tuer verschlossen ohne Schluessel) |
| ROOM10F0/10F1 | Q | Item-Modal (Item-Name aus der Tabelle); Texte nur falls noetig 28..31 |
| ROOM1151 | L | 20..23 ("He has already passed...") |
| ROOM1101 | L | 20..21 (Untertitel zu main16.5) |
| ROOM1121, 1021, 1211 | L | nur falls noetig ab RDT-Zahl, max +3 |
| ROOM1190/1191 | H | 6..11 (r35-Zuteilung bleibt; H baut sie um) |
| ROOM1150/10D0/1030/1200/11D0 (+ xxx1) | N | Karten: je Raum 2 IDs ab 28 (1150: 28/29 frei? r35 nahm 20..31 — N prueft und nimmt notfalls Reserve 30/31 bzw. weicht auf das Item-Modal aus) |
| ROOM1090 | H | nur Dossier (Kiste), keine IDs |
| ROOM1210 (Leon, RDT-Zahl 0) | P | 20..21 ("The cell door is open...") |
| ROOM1091 (Elza, RDT-Zahl 0) | P | 20..21 ("burnt-out shed") |
| ROOM1020 | P | keine neue ID (msg 0 "It's the RPD officer, Roy..." bleibt, nur an die Leiche gebunden) |
| ROOM30C1, 6030, 6031, 3060 + Sweep-Treffer | P | OVERRIDE bestehender IDs per re15_msg_install_text NACH re15_msg_load_room_block (Dauer neu per re15_msg_install_durations); keine neuen IDs ausser 20..21 je Raum, falls ein Ersatzsatz mehr Zeilen braucht |

Sprachdateien: `synchro/STAGE1/room<RAUM>/main<ID>.wav` (Port bindet sie automatisch). Spur L: die Datei
`main16.5.wav` liegt in room1011 (nicht 1101!) — Zuordnung belegen und ggf. den Nutzer-Pfad als
Sonderfall einbinden, nicht umbenennen/verschieben (Nutzer-Datei).

### 1.3 Ereignis-Nummern (Port-Programme ueber scd_event_fire, < 32, nicht die RDT-Subs des Raums)
Vergeben: ROOM1050 2 (Rolltor), 13 (Ada-Ruf); ROOM1150 4 (Hebetisch), 21 (Irons-Tod/Montage, Spur K
uebernimmt); ROOM10F0 20 (Szene, J uebernimmt); ROOM1120 24 (Fenster); ROOM1190 27 (Zielscheiben, H).
Neu: J = 22, 23 in ROOM11B0; Q = 25 (10F0/10F1/11B0); L = 26 (1151/1121/1021/1101/1211); N = 28 (alle
Karten-Raeume); P = 29 (1020/1210/1091 — nur falls ein Port-Programm noetig ist; Text-AOTs und Overrides brauchen keins); I/M/O/A/B/C/D/E/F/G: keine.

### 1.4 Dateien — Konfliktvermeidung beim Zusammenfuehren
* Neue Logik in NEUEN Dateien `engine/src/<thema>_r36.c` + `include/re15_<thema>_r36.h`; gemeinsame
  Dateien (scd_room_setup.c, scd_vm.c, game_step_common.c, enemy_ai_common.c, re15_esp.c, main.c,
  re15_map_zones.h, msg_common.c, menu_common.c, inventory_common.c, re15_damage.c, climb_common.c,
  render_pc.c) NUR mit kleinen Haken (1-5 Zeilen) an klar benannten Stellen, Installer-Zeile am ENDE des
  Blocks in scd_room_setup.c mit Kommentar `Runde 36 Spur X`.
* Eigentuemer bestehender Runde-35-Module: K = irons_tod_1150.c; J = cut_10f0.c (Kamera/Abgang) +
  11B0-Szene; L = fenster_1120.c (Elza-Zweig) + adaruf/Irons-Texte in 1151; H = ziel_1190.c; D =
  granate_r35.c + Gore-Zweig der Explosion; C = re15_damage.c (Schusspfad) + Blut-Anker; F =
  enemy_ai_re2_zombie.c / affen_11c0.c / enemy_ai_common.c-Haken (Sichtfilter); E = RE15_ACTOR_MAX + alle
  Arrays; I = inventory_common.c + re15_savepoint.c + Debug-Menue; N/M = Karte (map_*; N nur Items +
  Freischaltung, M nur Zonen/Tuermarken/Besucht-Latches); O = PRI-Masken/pri-Pipeline; Q = inhalt_r35.c-
  Muster (Item-AOT) in neuer Datei schluessel_r36.c + Tuer-Sperre nach tuer1120_1130.c-Muster; P = story_r36.c
  (Leichen-Gate 1020 nach dem Muster der Spawn-Overrides, Text-AOTs 1210/1091 nach dem Muster der Text-AOTs
  (04 02 ... 01 00), Text-Overrides als Installer-Zeilen je Raum am ENDE des Blocks in scd_room_setup.c).
* Tests: NEUE Sonden/Pins in `tests/unit/probes/r36_<spur>.cmake` (GLOB) + eigene
  `tests/unit/test_r36_<spur>*.c` / `tests/integration/test_r36_<spur>*.cmake` (registriert aus der
  eigenen probes-Datei). NICHT tests/unit/CMakeLists.txt, NICHT tests/integration/CMakeLists.txt.
  Haken mit Framedump-Pixeln IMMER mit RE15_WINDOW_SCALE=1 (Runde 35: Fenstergroesse haengt am Desktop).
  Arbeitsordner eines Tests NIE gleich dem Namen eines Targets (Linux: kein .exe-Suffix).
* `release/`, `platform/android/`, RELEASE_NOTES.md: NUR der Orchestrator.
* Assets: KEIN Patch an `shared_assets/PSX/*`. Neue Daten als `gen/*.inc` oder neue Dateien unter
  `shared_assets/RE2/` (RE2-Modelle/-Toene; im Dossier nennen — Paket-Gate).

### 1.5 Bauen und Testen im eigenen Baum
```
cd <baum>
bash re15_port/tools/local_build.sh configure     # einmal (GLOB), danach build/test/all
bash re15_port/tools/local_build.sh all           # Suite; Schranke RE15_MIN_TESTS=592
```
Unter Last flattern die Fenster-Haken (integration_boot_bg_pin, dark_start_pin, relatch_pin,
save_counter_pin, weste_load_pin, r30_sicherung_*, r30_titel_puls, r34n_e_dokumente_bild) und einzelne
exe-Starts brechen mit exit 1 nach 0,1 s ab — einzeln 2-3x nachfahren, nicht als Regression werten.
NIE taskkill /IM re15_pc.exe. Zwei Builds im selben build/: NIE. Messkopie der exe unter eigenem Namen.
Der schlanke Baum traegt re15_port, analysis, Quellcode-/Overlay-Dumps, info/Re1.5, info/re2leon, voice,
synchro, stage_saves, release, tools, pri, extracted_re2_*; NICHT `info/Resident_Evil_und_Playstation_
Information/` und `.agent_refs/` (aus dem Hauptbaum C:/workspace/git/reAi_v2/ lesen). Keine Junctions.

### 1.6 Commits
Frueh und in Schritten (`wip(r36-<spur>): ...`, am Ende `fix/feat(r36-<spur>): ...`). Jede Konstante mit
`@0x...` bzw. Datei-Byte-Offset im Code UND in der Commit-Message; Nutzer-Vorgaben (Positionen, Texte,
Choreografie) als NUTZER-VORGABE / PORT-WAHL gekennzeichnet, ihre FORM (Opcodes, Clips, Handler) belegt.
Rohbytes/Backticks per `git commit -F <datei>`. Nur eigene Pfade adden, nie `git add -A`.

## 2. Regeln (Kurzfassung, Details CLAUDE.md)
1. STOP-GATE: MESSEN/REPRODUZIEREN -> ORIGINAL disassemblieren (RE1.5; wo RE1.5 unfertig ist, RE2 Retail —
   Sound ist RE2, `RE2_SOUND_LISTE.md`) + Adresse ins Dossier -> DANN Code -> VERIFIZIEREN.
2. Regressionen (A, B, F, D-Wurf, C-Blut): ZUERST bisecten (v0.8.21 -> v0.8.22 -> v0.8.23, dann Commits der
   Runde 35 — `git log --oneline 9362dd7e..3ec5b1f8`), den verursachenden Commit nennen, DANN beheben; der
   Fix muss den Befund ERKLAEREN.
3. Nutzer-Beobachtung schlaegt Code-Hypothese; "sieht richtig aus" ist kein Beleg; die Abnahme misst am
   gebauten Stand gegen den WORTLAUT (AUFTRAG.md).
4. Kein "deferred/tunable/Env-Schalter" als Abschluss; Offenes mit Adresse + Messweg unter OFFEN.
5. Das Dossier ist das Produkt; Abschlussantwort < 15 Zeilen.
