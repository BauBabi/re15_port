# Runde 39 — Vertrag der Spuren (verbindlich)

Basis: master nach dem Release v0.8.26 (Tag v0.8.26). Jede Spur arbeitet in ihrem eigenen Arbeitsbaum
`.claude/worktrees/r39_<thema>` (Zweig `r39/<thema>`). Wortlaut des Nutzers: `AUFTRAG.md` (massgeblich). Regeln wie
Runde 38 (`analysis/befunde_runde38/VERTRAG.md`): STOP-GATE, Beta -> Retail (wo RE1.5 unfertig ist, gilt RE2; Sound ist
RE2, `RE2_SOUND_LISTE.md` zuerst), KEINE volle Suite im Spurbaum (nur `ctest -R`), Dossier fortlaufend committen, nur
eigene Pfade adden. Elza-Punkte immer mit Elza messen (RE15_PSELECT_AUTO=1 RE15_PSELECT_AUTO_SWITCH=1); Elza-Raeume sind
die ROOMxxx1-Varianten.

Ablauf (Nutzer 2026-10-17: "Stueck fuer Stueck, damit nicht zu viel Tokens verbrannt werden"): drei Wellen zu je vier
Spuren; nach jeder Welle Integration + gezielte Tests, volle Suite einmal am Ende, dann Release v0.8.27.

## 0. Spuren

| Welle | Spur | Thema / Baum | Punkte (AUFTRAG.md) |
|---|---|---|---|
| 1 | A | stimmen / r39_stimmen | 1050 main09-11 vor der Ada-Rettung; 1150 Irons-Tod; 11B0 "But hold on" + "unexpected Visitors"; 1171 "Alright, let's get down from here" |
| 1 | B | toene / r39_toene | 10F0 Schublade (RE2); 1090 Gitter der Hunde-Szene (Stage-2-Spinnengitter); 1090 Schrank nach dem Raetsel (RE2 2F-Regale) |
| 1 | C | inventar / r39_inventar | 1000 Spind: erst Handgun-Munition, dann "Nothing useful"; Memory Cards stapelbar; Elza-Inventar ohne Leons Bild/Karte; gelbe Karte nach "discard" weg |
| 1 | L | optik / r39_optik | F9-Marke 1100 Cut 1: Bodenstreifen deckt Leon; Schluessel 10F0/10F1 kleiner/dezenter |
| 2 | D | treffer / r39_treffer | halb aufstehende Zombies treffbar; Zombies ueber den Tisch treffbar; Auto-Aim nur fuer sichtbare Gegner |
| 2 | E | gegner / r39_gegner | Parking-Lot-Gorillas doppelte Energie + Flinch erst nach 3 Handgun-Treffern; kein paralleler Hundebiss (1090); liegende Zombies kriechen (1220/1221, 1010); Gorilla-Schatten beim Sprung bleibt am Boden (Nachtrag 2026-10-18) |
| 2 | F | zustand / r39_zustand | volle Heilung = ganze Kleidung; Gift zieht Energie (EKG) wie RE2 |
| 2 | H | elza / r39_elza | 1031 Hand nur einmal heben; nach Roys Tagebuch sofort die Karte mit Markern; Elza klettert in 1091 nicht aufs Autowrack |
| 3 | G | karten / r39_karten | Karten 1F/2F/3F/B2 nach map*_correct.png, B1 neu; 1090-Marker im Raum |
| 3 | I | funk10f1 / r39_funk10f1 | neue Elza-Szene 10F1 am Kommunikationssystem (3 Zeilen) |
| 3 | J | dining1141 / r39_dining1141 | ROOM1141 Umbau (8 Unterpunkte); kurze Panik-Musik beim Wechsel 1141 -> 1171 |
| 3+ | M | schluessel2 / r39_schluessel2 | Schluessel 10F0/10F1 hoechstens halb so gross (Nachtrag 5) |
| 3+ | N | stimmen3 / r39_stimmen3 | Nutzer-Aufnahmen 1141 main12-14 + 10F1 main06/main08 zuordnen, 1141 msg 12-14 stumm (Nachtrag 8) |
| 3 | K | heliport1171 / r39_heliport1171 | John 1171 "Elza hurry up": Heran-Winken statt Klettern; uebrige Szenen-Animationen 1171 |

## 1. Ressourcen

### 1.1 Bank-9-Bits (gespeichert mit g_game.flags)
Bank 9 traegt im Original die ITEM-TAKEN-Bits; Port-Bits stehen im Riegel `tests/unit/test_r36_kartenbits.c` (k_port[]).
Nach Runde 38 frei in Original UND Port (2026-10-18, selbst nachpruefen): 69, 70, 94, 95, 96, 97.
Zuteilung: **C = 69** (Munition im Spind ROOM1000), **I = 70** (Funkszene 10F1 gesehen), **J = 94, 95**.
96 und 97 vergibt der Orchestrator auf Anfrage (im Dossier begruenden). Jedes neu belegte Bit traegt die Spur in
k_port[] ein (kleiner Haken, eine Zeile).

### 1.2 Nachrichten-Ids (Port-Text-Plaetze)
Port-Ids liegen hinter den Original-Nachrichten eines Raums; vorhandene Port-Ids per grep `MSG_ID` (include/, gen/)
ausschliessen. Zuteilung: **I**: 10F1 Ids 40-44; **J**: 1141 Ids 50-59 (40-49 hat Runde 38 F); **C**: 1000 nur, wenn
das Aufnahme-Modal einen eigenen Text braucht (Id 40). Alle anderen Spuren: nur vorhandene Nachrichten.
Sprachdateien (synchro/) folgen der Message-Id (synchro/README.md) — eine Id, zu der der Nutzer schon eine WAV hat,
nie umwidmen. Nutzer-Dateien unter synchro/ werden NIE umbenannt oder veraendert (Zuordnung im Code).

### 1.3 Gemeinsame Dateien (nur kleine Haken, 1-5 Zeilen, Kommentar "Runde 39 Spur X")
scd_room_setup.c (Installer-Zeile am ENDE des Blocks), scd_vm.c, platform/pc/main.c, audio_pc.c (A, B), render_pc.c (L),
inventory_common.c / re15_inv_screen.c (C, F), enemy_ai_common.c (D, E), enemy_ai_re2_zombie.c (D, E), enemy_ai_re2_dog.c
(E), Karten-Code (G), elza_r36.c (H), dining_r38.c + gen/dining_r38_programme.inc (J), heliport1171_r38.c (K),
test_r36_kartenbits.c k_port[] (C, I, J). N: Tabelle engine/src/stimmen_r39.c (Spur A), Riegel test_r39_stimmen.c (A) und
test_r39_funk10f1.cmake (I, Stellvertreter-Datei). `release/`, `platform/android/`, `shared_assets/PSX/*`,
`release/RELEASE_NOTES.md`: TABU fuer alle.

### 1.4 Ueberschneidungen (bekannt, bei der Integration aufzuloesen)
- ROOM1171: A (Stimme "Alright, let's get down from here") und K (Johns Animationen) — Welle 1 gegen Welle 3; K baut auf
  dem integrierten A auf.
- ROOM1141 -> 1171: J (Musik beim Wechsel) und K (Szene 1171) — dieselbe Welle; J aendert nur den Uebergang/die Musik.
- ROOM1090: B (Gitter- und Schrank-Ton), E (Hundebiss), G (Kartenmarker), H (Elza 1091 Klettern) — verschiedene Stellen.
- ROOM10F0/10F1: B (Schublade), L (Schluesselgroesse), I (Funkszene 10F1) — verschiedene Stellen.

## 2. Abnahme-Massstab
Je Nutzer-Punkt: am gebauten Spiel gemessen (Framedump RE15_WINDOW_SCALE=1, Ton-Mitschnitt RE15_AUDIO_CAP_SYNC, Logs),
Original-Beleg (Adresse/Bytes) oder NUTZER-VORGABE/PORT-WAHL mit belegter Form, mindestens ein messender Riegel je Punkt,
Nebenwirkungen auf beide Szenarien geprueft. Neue Tests: parallel-fest (eigene Laufordner, exe-Kopien mit Praefix je
Test, keine Umgebungsvariablen des Aufrufers erben), Linux-fest (libm/`m` bei sqrt/sin/cos, `cmake_minimum_required`
in jedem -P-Skript, POSIX-Feature-Makro, keine .exe-/Backslash-Pfade) — Lehren aus dem Release v0.8.26.
