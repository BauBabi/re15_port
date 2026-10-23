# Runde 41 — Vertrag der Spuren (verbindlich)

Basis: master nach dem Release v0.8.28 (f632a17b) + Masken 5924939d + Runde-41-Doku. Jede Spur arbeitet in ihrem
eigenen Arbeitsbaum `.claude/worktrees/r41_<thema>` (Zweig `r41/<thema>`, Helfer `orchestrator/mk_wt41.sh`). Wortlaut
des Nutzers: `AUFTRAG.md` (massgeblich). Regeln wie Runde 38/39 (`analysis/befunde_runde39/VERTRAG.md`): STOP-GATE,
Beta -> Retail (wo RE1.5 unfertig ist, gilt RE2; Sound ist RE2, `RE2_SOUND_LISTE.md` zuerst), KEINE volle Suite im
Spurbaum (nur `ctest -R`), Dossier fortlaufend committen, nur eigene Pfade adden. Elza-Punkte immer mit Elza messen
(RE15_PSELECT_AUTO=1 RE15_PSELECT_AUTO_SWITCH=1); Elza-Raeume sind die ROOMxxx1-Varianten.

Ablauf (Nutzer 2026-10-23: "Brich all diese Aufgaben in immer wenige parallele Tasks runter"): hoechstens 4 Spuren
gleichzeitig (Semaphor im Workflow), Wellen; nach jeder Welle Integration + gezielte Tests + alle unit_/prop_/probe_,
volle Suite einmal am Ende im Release-Container, dann Release v0.8.29.

## 0. Spuren

| Welle | Spur | Thema / Baum | Punkte (AUFTRAG.md) |
|---|---|---|---|
| 1 | A | wasser20b0 / r41_wasser20b0 | 20B0-Wasserschwall im Original messen + EM024 im Port reparieren; STAGE2 Wassereffekt beim Sprung von der Kante ins Wasser; NPCs teilen die Wasser-Effekte |
| 1 | B | re2gator / r41_re2gator | RE2-Alligator: Finisher + fehlende Animationen extrahieren; Verfolgung (Animation, Schreie, Beschleunigung an gleichen Stellen); Waende wackeln + Putz faellt; Kanister-Aufnahme + Sterbeanimation mit geplatztem Kiefer wie RE2 (Mechanik mit Testhaken); Klettern ueber die Plattform BLEIBT |
| 1 | C | ventilkette / r41_ventilkette | STAGE2-Kette: 2000 Leiter ("water pressure"), 2030 Bruecke oben + Pult (Ventil 13), Ventilraetsel-Platzhalter -> Warnung, 60 s, Wasser steigt ueber die Ringebene, Tod mit Ertrink-Effekt, Ost-Tuer -> 20B0; danach 20B0 statt 2000 (Leiter frei); 2050-Pult (80 V) sperrt den Hebel; 2080-Rechner entleert auch 2030 |
| 1 | D | kleinkram_a / r41_kleinkram_a | Lautstaerke-Option (alles lauter/leiser); Feuerloescher 1030 Blink-Effekt (RE2 uebernehmen; API fuer Spur F); Spinnengift bei Danger toedlich |
| 1 | E | kleinkram_b / r41_kleinkram_b | Leon "fliegt" an der Kurbel in 2020; Karte B1: Marker springt vor der 1090-Vordertuer in den hinteren Teil; Werkstatt-Tuer 11B1 bei Elza dauerhaft zu |
| 1 | M | masken36 / r41_masken36 | STAGE3-6-Masken schliessen (Loecher); Sichtpruefung der 53 neuen Masken 11A0/STAGE2 im Spiel |
| 2 | F | gatorszene / r41_gatorszene | 2090-Szene: Zombie am Boden mit blinkendem Schluessel, Alligator springt aus dem Wasser und frisst ihn samt Schluessel; nach dem Tod Schluessel bei der Leiche (blinkend); Spinnen aus 2090 entfernen |
| 2 | G | kanister2080 / r41_kanister2080 | nach der 2090-Szene in 2080: Metalltuer-Schliess-SE + 8 Zombies; Gaskanister an der Reling (Gas_Can.bmp); Szene nach genug Treffern: Marvin/John koepft die 8 Zombies, nimmt den Kanister, "Hey, Leon, try with this!", Wurf ueber die Reling -> liegt in 2090 im Wasser; Alligator nimmt ihn auf (Spur-B-Mechanik), Schuss -> RE2-Tod |
| 2 | H | kollision / r41_kollision | NPCs (Ada, Marvin) fest, laufen nicht ineinander; Kisten in 2050 nicht ineinander schiebbar |
| 2 | I | zombiefresser / r41_zombiefresser | Zombies in 2000/2001, 3000, 3010 usw.: fressen statt aufzustehen, wenn man naeher kommt |
| 3 | J | stage3szenen_a / r41_stage3szenen_a | 3060 Marvins Animation am Ende seines Abschnitts; 3061 Sherry krabbelt in den Schacht; Elzas Animation am Ende dieser Szene |
| 3 | K | stage3szenen_b / r41_stage3szenen_b | ab 30C1 Sherrys Modell kaputt + ohne Waffe; 3091-Szene mit staendig falschen Animationen |
| 3 | L | spinnenanim / r41_spinnenanim | Spinnen: Herausklettern aus dem Gitter + Herunterklettern an der Wand |

Abhaengigkeiten: F und G bauen auf dem integrierten B (RE2-Alligator) und D (Blink-API) auf; G auf F (Bit "Szene
gesehen"). C und A beruehren beide 20B0 (A: Effekt EM024; C: Raumwechsel/Weg) — verschiedene Stellen.

## 1. Ressourcen

### 1.1 Bank-9-Bits (gespeichert mit g_game.flags)
Zensus 2026-10-23 (`re15_port/tools/r36_kartenbits/bank9_zensus.py` + Riegel `unit_r36_kartenbits`): im Original UND
im Port frei sind u.a. 100-103, 106-107, 110-133. Zuteilung: **C = 100, 101, 102, 103, 106, 107** (Leiter frei /
Ventil 13 / Bruecke unten / Flut gelaufen / 2050-Pult / 2080-Entleerung — im Dossier benennen), **F = 110-113**
(Szene gesehen 110, Schluessel liegt 111, Schluessel genommen 112), **G = 114-119**, **E = 120** (nur wenn kein
Original-Flag reicht), **J/K = 121-124** bei Bedarf, 125-133 vergibt der Orchestrator. Jedes neu belegte Bit traegt
die Spur in `tests/unit/test_r36_kartenbits.c` k_port[] ein (eine Zeile) — 104/105 sind Original!

### 1.2 Nachrichten-Ids (Port-Text-Plaetze)
Port-Ids liegen hinter den Original-Nachrichten eines Raums (ab 40; vorhandene per grep `MSG_ID`/Ids in include/ und
gen/ ausschliessen). Zuteilung: **C**: 2000/20B0 (+Elza-Varianten) Id 40 (Leiter), 2030 Ids 40-49, 2050 Ids 40-44,
2080 Ids 40-41; **F**: 2090/2091 Ids 42-46; **G**: 2080/2081 Ids 42-47 (42 = "Hey, Leon/Elza, try with this!",
Stimmen `synchro/STAGE2/room2080/main42.wav` und `room2081/main42.wav` — im Dossier fuer den Nutzer nennen);
**D/E/H/I/J/K/L/M**: nur vorhandene Nachrichten. Nutzer-Dateien unter synchro/ und pri/ werden NIE veraendert.

### 1.3 Gemeinsame Dateien (nur kleine Haken, 1-5 Zeilen, Kommentar "Runde 41 Spur X")
scd_room_setup.c (Installer-Zeile am ENDE des Blocks: C, E, F, G, I), scd_vm.c (C, F, G), platform/pc/main.c (D
Lautstaerke; F/G Szenen-Haken), audio_pc.c (D), render_pc.c (D Blink), enemy_ai_boss_gator.c (B = Eigentuemer; F/G nur
Haken), enemy_ai_common.c (A EM024-Zweig; L Spinne nur Haken), enemy_ai_re2_zombie.c (I; G nur Kopfschuss-Aufruf),
enemy_ai_re2_spider.c / spinnen_r38.c (L), re15_collision.c / game_step_common.c / aot_common.c (H), karte_r39.c /
re15_map_zones.c (E), zustand_r39.c / re15_damage.c (D), wasser_r38.c / re15_esp.c (A), anim_select_common.c / NPC-
Code (J, K), elzazombies_r39.c (E), test_r36_kartenbits.c k_port[] (C, F, G, E). `release/`, `platform/android/`,
`shared_assets/PSX/*` (Ausnahme: **M** darf `shared_assets/PSX/MASKS/` schreiben), `release/RELEASE_NOTES.md`,
`pri/`, `synchro/`: TABU fuer alle. Neue RE2-Daten nach `shared_assets/RE2/` (im Dossier nennen, Paket-Gate).

### 1.4 Ueberschneidungen (bekannt, bei der Integration aufzuloesen)
- ROOM2090: B (Alligator-Verhalten), F (Szene, Spinnen weg), G (Kanister landet dort), A (Wasser-Effekte) — B zuerst
  (Welle 1), F/G auf dem integrierten B; G setzt den Kanister ueber B's Testhaken-Pfad.
- ROOM2080: C (Rechner entleert 2030), G (Zombies, Kanister, Szene) — verschiedene Stellen.
- ROOM20B0: A (EM024-Flut) und C (Weg/Leiter nach der Flut).
- Zombies: I (Fresser wachen auf) und G (8 Zombies in 2080, NPC-Kopfschuss) — G aendert die Zombie-KI nicht.

## 2. Abnahme-Massstab
Je Nutzer-Punkt: am gebauten Spiel gemessen (Framedump RE15_WINDOW_SCALE=1, Ton-Mitschnitt RE15_AUDIO_CAP_SYNC, Logs),
Original-Beleg (Adresse/Bytes; bei RE2-Uebernahmen Adresse in `info/re2leon`/EM23_OVL + Byte-Offsets der Clips) oder
NUTZER-VORGABE/PORT-WAHL mit belegter Form, mindestens ein messender Riegel je Punkt, Nebenwirkungen auf beide
Szenarien geprueft. Neue Tests: parallel-fest (eigene Laufordner, exe-Kopien mit Praefix je Test, keine
Umgebungsvariablen des Aufrufers erben), Linux-fest (libm/`m` bei sqrt/sin/cos, `cmake_minimum_required` in jedem
-P-Skript, POSIX-Feature-Makro, keine .exe-/Backslash-Pfade, `_DEFAULT_SOURCE` fuer popen).
