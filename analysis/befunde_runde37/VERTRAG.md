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
| H | zombieaktiv / r37_zombieaktiv | Nachtrag 2: Zombies zu passiv — GENERELL fuer alle Zombies das RE2-Verhalten (teilweise aktiv im Gang, auf Leon zu); Beispiel Montage nach Irons' Tod |
| I | feuerloescher / r37_feuerloescher | Nachtrag 4: Feuerloescher-Modell bauen; leer am Boden bei Elza (1091) und bei Leon nach dem Loeschen (1090); Leons Loescher aus dem Spind 1000 an einen besseren Ort |
| J | bissarm / r37_bissarm | Nachtrag 6: beim Zombie-Biss fehlt teilweise der dranhaengende Arm (am Spieler haengender Zombie-Arm bzw. baumelnder angeschossener Arm) — wie RE2 |
| K | roy / r37_roy | Nachtrag 7: Roy in Elzas Szenario — Modell + alle Animationen wie im Original, keine Dauerschleife |
| L | re2zombie / r37_re2zombie | Orchestrator-Auftrag L (zu Nachtrag 2 + Ergaenzung): die drei vorbestehenden Abweichungen der RE2-Zombie-KI schliessen (0x201-Produzenten @0x80101D68/EC4/F14, DECISION[0] @0x801013c8, EXEC[0] P1 @0x80101540-a0), Nahkampf-Zusammenbruch aufklaeren; Basis = Stand der Spur H 9c2a825d |
| M | kiste / r37_kiste | Nachtrag 9: Item-Kiste — Symbole so weit rechts, dass kein Name sie ueberdeckt; der Rahmen liegt beim Scrollen ueber den Items (re15_inv_screen.c Block 7b, inv_render_pc.c) |
| O | szene11c0 / r37_szene11c0 | Nachtrag 11: Ada/Marvin-Szene ROOM11C0 nach Irons' Tod — keine Fehlblicke/180-Grad-Drehungen, wer spricht, sieht den anderen an (irons_tod_1150.c Programm 4 k_p_11c0) |
| N | elzaems / r37_elzaems | Orchestrator-Auftrag N (zu Nachtrag 7): Elzas Szenario laedt jeden Typ wie das Original aus CDEMD1.EMS (Elza-Bit @0x80022334, Datei 0x27 @0x800223ac); Basis = Stand der Spur K |
| P | masken36 / r37_masken36 | Orchestrator-Auftrag P (Regression 53741fe5): R15M-Masken der Stages 3..6 ohne eigenen Atlas wieder mit dem BSS-Atlas zeichnen; Basis = Stand der Spur A |
| Q | kistenregel / r37_kistenregel | Orchestrator-Auftrag Q: Item-Benutzung nur mit dem Item im Inventar (RE2-Regel), wo die RE2-Item-Box eine Original-Bit-Abfrage oeffnet; Zensus aller Faelle, erster Fall Loescher 1090; Basis = Stand der Spur I |
| R | kiste2feld / r37_kiste2feld | Orchestrator-Auftrag R: 2-Feld-Symbole in der Kisten-Liste wie RE2 aus zwei Kacheln (u1 0x78 @0x8007183c/@0x80071848, FUN_80069c40 @0x80069cd8); Basis = Stand der Spur M |

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
- I: Port-Ids 29..31 in den Raeumen der Spur (1090/1091/1000 und der neue Fundort); Taken-Bit und Story-Flag des Loeschers
  bleiben die Original-Bits (9,0x86)/(3,0x85), der Boden-Loescher bei Leon haengt an (3,0x84).
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
(player_common.c, schuss_r36.c, Treffer-Resolver) (G), enemy_ai_re2_zombie.c / enemy_ai_common.c / irons_tod_1150.c (H).
Neue Logik in NEUEN Dateien engine/src/<thema>_r37.c + include/re15_<thema>_r37.h.
Spur M aendert den Box-Listen-Block 7b in re15_inv_screen.c und, falls noetig, den Zeichner inv_render_pc.c (Kommentar "Runde 37 Spur M" + Beleg).
Spur O aendert Programm 4 (k_p_11c0) in irons_tod_1150.c an Ort und Stelle (Kommentar "Runde 37 Spur O"); die uebrigen Programme (Spur H: Nachspawn 1130) und k_msg28 (Nachtrag 10, Integration) nicht anfassen.
Spur N erweitert roy_r37.c / roy_pc.c / gen/ems_toc_r37.inc der Spur K (oder eine neue elzaems_r37.c) und die Lader-Haken in main.c / enemy_common.c; Griff-/Biss-Code (J), RE2-KI (L) und der Spawn-Haken (H) nur lesen.
Spur P aendert den Masken-Lader (bg_pc.c re15_pri_load_cut_atlas_ex und was A dort angefasst hat) an Ort und Stelle; keine neuen Asset-Dateien, MASKS/* nicht umschreiben.
Spur Q: neue Logik in kistenregel_r37.c (Tabelle je Fall), Haken an der Stelle, wo das Original das Bit prueft (scd_vm.c Ck-Pfad oder je Raum), feuerloescher_r37.c (Spur I) nur fuer den 1090-Fall; Texte nur mit RE2-Beleg (Ids nach VERTRAG 1.2 begruenden).
Spur R aendert re15_inv_screen.c (Box-Symbol box_cell_icon_sized und die Kachel-Logik) und inv_render_pc.c an Ort und Stelle (Kommentar "Runde 37 Spur R").
Ausnahme Spur L: die Reparatur IST die RE2-Zombie-KI — L aendert enemy_ai_re2_zombie.c (DECISION[0], EXEC[0], EXEC[1], EXEC[2]) direkt an Ort und Stelle (Kommentar "Runde 37 Spur L" + @0x je Zeile). Die Griff-/Biss-Teile (Spur J) und re2z_init (Haken der Spur H) nur lesen. Jeder Riegel, der am gemeinsamen RE2-Zufallsstrom haengt und sich durch L verschiebt, steht mit altem/neuem Wert und Begruendung im Dossier (die Integration braucht die Liste).

### 1.5 Tests
Neue Tests nur ueber `re15_port/tests/unit/probes/r37_<thema>.cmake` (GLOB). Portabel fuer den Linux-Release-Bau:
POSIX-Aufrufe (popen ...) mit `_DEFAULT_SOURCE` vor den Includes, jedes neue `tests/integration/*.cmake` mit
`cmake_minimum_required(VERSION 3.16)` (v0.8.24: zwei Tests waren nur unter Windows gruen). RE15_MIN_TESTS hebt die
Integration an.

### 1.6 Assets
Kein Patch an shared_assets/PSX/* ausser den Port-Masken (A, ROOM1100.*) und neuen Port-Tuerarchiven
shared_assets/RE15DOOR/*.DO2 (C). Neue RE2-Dateien unter shared_assets/RE2/ (Paket-Gate-Eintrag im Dossier nennen).

### 1.7 Orchestrator-Freigabe 2026-10-12 (Spur A, nach Abnahme A#0 M1)
Spur A darf zusaetzlich zu MASKS/ROOM1100.* den Masken-LADER anfassen (platform/pc/src/entladen_pc.c re15_entladen_msk und
was die PRI-Masken je Raum laedt): Fuer Elzas Raeume (ungerade Raum-Id) fehlen in STAGE1 ALLE Masken-Dateien (MASKS/ hat
nur 1000, 1010, 1050, 1060, 10A0, 10C0, 10D0, 10E0, 10F0, 1100, 1110, 1120, 1130, 1140, 1160, 1180, 11B0, 11D0, 11F0, 1220,
1221, 1230 — Elza-Gegenstuecke ausser 1221 fehlen), der Port laedt `MASKS/ROOM%04X.MSK` mit der vollen Id. Erlaubt und
gewuenscht: Rueckfall auf die gerade Leon-Variante, wenn Kamera-Daten UND Hintergrund (RDT-Kamerasaetze, BSS/Bild je Cut)
beider Raeume nachweislich gleich sind — Zensus aller STAGE1-Paare (und spaeterer Stages ohne eigene ungerade Datei) mit
Ergebnis je Raum im Dossier, Riegel fuer 1101 und den Zensus. Wo sie sich unterscheiden: kein Rueckfall (im Dossier nennen).
Keine neuen Asset-Dateien noetig (Paket-Gate unveraendert).
