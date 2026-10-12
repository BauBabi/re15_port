# Runde 37 — Vertrag der Spuren (verbindlich)

Basis: master nach v0.8.24 (Release e8b3958f, Abschluss-Doku 8281cdbc). Jede Spur arbeitet in ihrem eigenen
Arbeitsbaum `.claude/worktrees/r37_<thema>` (Zweig `r37/<thema>`). Wortlaut des Nutzers: `AUFTRAG.md` (massgeblich).
Regeln wie Runde 36 (`analysis/befunde_runde36/VERTRAG.md` §2): STOP-GATE, Beta -> Retail (wo RE1.5 unfertig ist, gilt
RE2; Sound ist RE2, `RE2_SOUND_LISTE.md` zuerst), KEINE volle Suite im Spurbaum (nur `ctest -R`), Dossier fortlaufend
committen, nur eigene Pfade adden.

## 0. Spuren

| Spur | Thema / Baum | Punkte (AUFTRAG.md #) |
|---|---|---|
| A | pri1100 / r37_pri1100 | #1 ROOM1100: Zombie auf der F9-Marke wird falsch verdeckt (Marken im HAUPTBAUM, s.u.) |
| C | fahrstuhl / r37_fahrstuhl | #6 Fahrstuhl-Tuer der Tuersequenz ROOM1080 korrekt (passende RE2-Tuer oder eigener Bau), #7 Tuersequenz beim Aussteigen |
| D | itemband / r37_itemband | #8 Items auf erhoehten Flaechen nur vom richtigen Band aufnehmbar (Band-Gate + Member 0x12), alle betroffenen Items bleiben auf dem vorgesehenen Weg erreichbar |
| E | codepanel / r37_codepanel | #9 Code-Panels: blaues Licht unten solange der Code falsch ist, gruenes bei richtigem Code; Kamera bleibt waehrend Piep + Aufschliessen auf dem Panel |
| F | sperre1050 / r37_sperre1050 | #10 "Chief first"-Sperre zusaetzlich an der Tuer ROOM1050 -> 10A0 (Ada-Szenen-Tuer), #5 ROOM50B1 Elza-Text ohne Ada-Bezug |
| G | beretta / r37_beretta | Nachtrag: Beretta M93R — jeder Schuss des 3-Schuss-Feuerstosses trifft (Glock 18 mit demselben Burst-Handler pruefen) |

Nicht gebaut (Nutzer): Blutlache an Roys Platz (R2), Prison-Key-Hinweis fuer Elza, V6 (Montage-Folgen bei Elza) — ohne
Zustimmung nichts davon anfassen. Die Werkstatt-Tuer 11B0 bleibt wie in v0.8.24.

## 1. Ressourcen

### 1.1 Bank-9-Bits (gespeichert mit g_game.flags)
Bank 9 traegt im Original die ITEM-TAKEN-Bits (Item_aot_set pc[18] @0x80040680, FUN_8004efe4 @0x800406dc). Belegt vom
Original (Zensus `tools/r36_kartenbits/bank9_zensus.py`, Riegel `unit_r36_kartenbits`): 85 Bits, u.a. 2-22, 52, 85, 86, 90,
91, 92, 98, 99, 104, 105, 108, 109, 134-253. Belegt vom Port: 53-63, 65, 71-84, 87-89, 93 (77/78 = Lageplaene B2/B1).
Frei in Original UND Port: 64, 66-70, 94-97 (selbst nachpruefen).
Zuteilung: E = 94, 95; F = 96, 97 — NUR wenn noetig (lieber vorhandene Story-Flags lesen). Jedes neu belegte Bit traegt die
Spur in die Port-Liste des Riegels `tests/unit/test_r36_kartenbits.c` (k_port[]) und ins Dossier ein.

### 1.2 Nachrichten-Ids (Port-Text-Plaetze)
- F: ROOM1050 Port-Id 26..28 (Spur D Runde 34 Nacht belegt 25 = "I have to help the Survivor first!"); ROOM50B1 =
  Ueberschreibung von msg 0 ueber den Text-Weg der Spur P (`tools/r36_p/texte_bauen.py`, `gen/story_r36_texte.inc`).
- E: eigene Texte nur, wenn RE2 sie belegt; Ids je Raum im Dossier begruenden, Kollision mit vorhandenen Port-Ids
  (grep `RE15_.*MSG_ID` in include/) ausschliessen.

### 1.3 Ereignisse / Slots
Port-Ereignisnummern nur nach Zensus (vorhandene: adaruf 13 in ROOM1050 u.a.; grep `scd_event_fire`/`_ereignis` in
engine/src). AOT-Slots nur umwidmen, wie re15_tuer1060.h/re15_adaruf.h es tun (nach dem Init-Lauf von main00).

### 1.4 Gemeinsame Dateien (nur kleine Haken, 1-5 Zeilen, Kommentar "Runde 37 Spur X")
scd_room_setup.c (Installer-Zeile am ENDE des Blocks), scd_vm.c (Message_on-/Ereignis-Haken), platform/pc/main.c,
aot_common.c + actor_common.c + re15_collision.c (D), door_scene_pc.c / door_seq_*.c / gen/re15_tuer_eigen.inc /
gen/tuer_zuordnung.inc (C), audio_pc.c + panel_lampen_pc.c (E), render_pc.c + shared_assets/PSX/MASKS/ROOM1100.* (A), Schuss-/Waffen-Code
(player_common.c, schuss_r36.c, Treffer-Resolver) (G).
Neue Logik in NEUEN Dateien engine/src/<thema>_r37.c + include/re15_<thema>_r37.h.

### 1.5 Tests
Neue Tests nur ueber `re15_port/tests/unit/probes/r37_<thema>.cmake` (GLOB). Portabel fuer den Linux-Release-Bau:
POSIX-Aufrufe (popen ...) mit `_DEFAULT_SOURCE` vor den Includes, jedes neue `tests/integration/*.cmake` mit
`cmake_minimum_required(VERSION 3.16)` (v0.8.24: zwei Tests waren nur unter Windows gruen). RE15_MIN_TESTS hebt die
Integration an.

### 1.6 Assets
Kein Patch an shared_assets/PSX/* ausser den Port-Masken (A, ROOM1100.*) und neuen Port-Tuerarchiven
shared_assets/RE15DOOR/*.DO2 (C). Neue RE2-Dateien unter shared_assets/RE2/ (Paket-Gate-Eintrag im Dossier nennen).
