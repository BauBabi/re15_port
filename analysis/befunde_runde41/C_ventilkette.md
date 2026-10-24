# Runde 41 — Spur C "ventilkette" (STAGE2-Kette 2000 / 2030 / 20B0 / 2050 / 2080)

Baum `.claude/worktrees/r41_ventilkette`, Zweig `r41/ventilkette`, Basis master 116e8b0f. Begonnen 2026-10-23 17:12.
Wortlaut des Nutzers: `AUFTRAG.md` ("Vorab vereinbart"); Zuteilung: `VERTRAG.md` (Bank 9 Bits 100-103, 106, 107;
Text-Ids 2000/20B0 40, 2030 40-49, 2050 40-44, 2080 40-41). Vorrunde: `befunde_runde40/UEBERGABE.md` "Befund STAGE2".
Modul: `re15_port/engine/src/ventilkette_r41.c` + `include/re15_ventilkette_r41.h` (alle Konstanten mit Beleg im Kopf),
Daten `engine/src/gen/ventilkette_r41_{texte,original,programme,wasser}.inc` aus
`tools/r41_ventilkette/gen_r41_ventilkette.py` + `programme.py`. Bilder: `C_belege/`.

## 0. Stand / Protokoll (neueste zuerst)

* 2026-10-24 ab 10:00 **Nachbesserung 3** (Abnahme 2 `C_abnahme_2.md`, Maengel N1-N4 + Hinweis H-a; Nachtraege 6-10 des
  Nutzers, AUFTRAG.md 09:17-09:55). Baum sauber (`status --porcelain` leer, HEAD 4abf0500). Arbeitsplan und Ergebnis in §0d.
  Messordner scratchpad `r41c_nb3/`.
* 2026-10-24 11:04 Bau `local_build.sh configure` (neue Riegel registriert: #1128 integration_r41_ventilkette_weg,
  #1129 _weg_elza, #1130 _sperre; `_kette` "uebersprungen: Spur A fehlt") + `build` -> `=== LOCAL-BUILD-OK`. Gezielte
  Unit-Gruppe (Regex der Abnahme 2, scratchpad `r41c_nb3/regex_unit.txt`): `100% tests passed, 0 tests failed out of 164`
  (34.55 s); unit_r41_ventilkette `PASS (376 Pruefungen)`.
* 2026-10-24 11:05-11:53 gezielte Integrationsgruppe (exe 11:03): `100% tests passed, 0 tests failed out of 5`
  (Haupt 257.45 s, _b 505.60 s, _weg 897.27 s, _weg_elza 908.15 s, _sperre 344.98 s), §5 "Nachbesserung 3".
* 2026-10-24 11:55 **Nachbesserung 3 abgeschlossen** (§0d): N1 Weg nach der Flut ueber den Strom zu Fuss (Leon + Elza),
  N2 Entleeren am Strom (3,126) / Rechner 2080 Original, N3 Relinghoehe y -3600 + unterer Bereich gesperrt + Bilder, N4
  Ventilraetsel am Sued-Landeplatz erst mit Bruecke und Sicherung; H-a durch Nachtrag 10 erledigt.
* 2026-10-24 07:50-09:00 **Nachbesserung 2 abgeschlossen** (§0c): Tuergraph (Werkzeug + C_belege/nb2_tuergraph_nach_flut.txt),
  Weg zu Fuss gemessen (2050 -> 2020, 2030 Nordgalerie -> 2060, 2040 -> Treppe -> Galerie -> 2070 Nordgang -> 11A0 bis
  zur Sperre, Leon + Elza; Gegenprobe (3,206); 2080 ab 2090 -> Rechner -> Bit 107). Ergebnis: Weg existiert nicht,
  ORIGINAL-Sperre ROOM11A0 Slot 4 ((3,206) @0x00FEA/@0x00FEE, nur 20A0 sub10 @0x01FF0 setzt es) -> nichts erfunden,
  kein Spiel-Code geaendert, OFFEN Frage §7 (j). Riegel unit J (286 Pruefungen) + integration_r41_ventilkette_weg
  (Passed 439.47 s); Unit-Gruppen 67/67 und 164/164. Dossier §0b M5/M7, §6, §7 richtiggestellt.
* 2026-10-24 ab 07:50 **Nachbesserung 2** (Abnahme 1 `C_abnahme_1.md`, Mangel N1; Orchestrator-Entscheid C-E2b
  07:49 als Commit 8933352d direkt in diesem Zweig). Baum sauber (`status --porcelain` leer), exe 06:37 = Code-Stand
  HEAD (seit 5ddd98c0 nur analysis/). Der FORTSETZUNGS-Block des Auftrags (exe 00:56 mit Spur-A-Haken, Unit-Gruppe am
  Stand 271b05ea, Dossier 4-7) ist seit der vierten Fortsetzung (63e30719) und 49606c0e erledigt -> nicht wiederholt.
  Arbeitsplan und Ergebnis in §0c.
* 2026-10-24 06:30-07:10 Nachbesserung 1, Nachtrag: die Orchestrator-Entscheide **C-E1..C-E3** (AUFTRAG.md 05:25,
  Commit 10332fb6 direkt in diesem Zweig, beim Start uebersehen und um 06:30 im `git log` gefunden) umgesetzt: C-E2 Ring-
  Zugaenge des gefluteten 2030 gesperrt (M5, Code + Riegel), C-E1 Ketten-Riegel heisst `integration_r41_ventilkette_kette`
  und wird nur mit A's Modul registriert (vorher: `_szene` mit Laufzeit-SKIP), C-E3 Haken angenommen (M6). Fragen f/i
  damit beantwortet; offen bleiben g (M7) und h (H2).
* 2026-10-24 05:40-06:40 **Nachbesserung 1** (Abnahme 0 `C_abnahme_0.md`; Opus): §0b je Mangel. Code: M1 (Kisten-Hebel
  Ereignis 2 gesperrt), M3 (Bank-5-Marker nur in 2030), M8 (Text 49); Riegel: unit +16 Pruefungen (255), _b E2/E3/E4,
  C misst die Blendenfarbe im beschleunigten Renderer (M4), neuer Ketten-Riegel (M2; seit C-E1
  `integration_r41_ventilkette_kette`, nur mit Spur A registriert). Die ganze Kette in EINEM exe-Lauf mit dem wortgleichen Haken der Spur A
  gemessen (temporaerer Messbau, nicht committet, Quelle danach zurueck, ohne Haken neu gebaut). Entscheidungsfragen
  f/g/h/i (§7) an den Orchestrator (M5, M7, H2, M6). Tests §5 "Nachbesserung 1".
* 2026-10-24 ab 05:40 (Nachbesserung 1 nach Abnahme 0, `C_abnahme_0.md`, Opus). Baum sauber (`status --porcelain`
  leer, HEAD 4af8fa3d). Arbeitsplan je Mangel M1-M9 + H1-H3 in §0b; Messordner scratchpad `r41c_nb1/`.
* 2026-10-24 02:51-03:10 (vierte Fortsetzung, Opus; Vorgaenger vom Sitzungslimit ~01:00 beendet). Baum sauber
  (`status --porcelain` leer, HEAD 271b05ea): der TEMPORAERE Nachbau des Spur-A-Hakens in aot_common.c war schon
  zurueckgenommen, die exe 00:56 aber MIT ihm gebaut -> `local_build.sh build` -> `=== LOCAL-BUILD-OK (build)`
  (exe 02:51; Quelle = Bau 00:41, auf dem beide r41-Integrationsriegel gruen liefen -> nicht wiederholt).
  Gezielte Unit-Gruppe (Regex §5) 02:56: `100% tests passed, 0 tests failed out of 37`. Lauf kette_mit_a (mit dem
  Haken, 00:56-00:58) ausgewertet -> §6 Querbezug A (ohne den Haken haengt die 20B0-Szene, mit ihm laeuft sie bis
  zur Freigabe). NEUE Messung ring_nach_flut (Eintritt in das geflutete 2030 ueber die 20B0-Ost-Tuer, 03:03-03:05)
  -> §4, §6 OFFEN, §7 Frage (f). Dossier nachgezogen: §1.1 Cut der 2000-Tuer Slot 3 (3, nicht 13), §3.3/§4 Spritzer
  = EIN Ereignis (ddb65cfc), §5 Teil B + Gruppenlauf 2/2 + Unit 239 Pruefungen.
* 00:30-00:58 (Vorgaenger, aus Commits und Logs scratchpad r41c2/ uebernommen, NICHT wiederholt): e1cb3352/b486f182
  integration_r41_ventilkette_b (erste Fahrt 00:36-00:41 PASS, 291.85 s, exe 22:17); ddb65cfc Spritzer ohne
  erfundene Rate + Nachmessung flut_spritzer (exe 00:41); 271b05ea Riegel C prueft den EINEN Spritzer, beide
  r41-Riegel 00:46:38-00:55:43 auf der exe 00:41 gruen (2/2, 545.09 s); ab 00:56 temporaerer Spur-A-Haken (nicht
  committet, vom Orchestrator zurueckgenommen, Patch scratchpad r41c2/temp_haken_spur_a_aot_common.patch) + Lauf
  kette_mit_a (scratchpad r41c2/mess/kette_mit_a, Skript r41c2/mess_kette_a.sh).
* 2026-10-24 00:26-00:29 Luecken am Spiel geschlossen (neue Messlaeufe, keine Wiederholung; Skript scratchpad
  r41c2/mess_b.sh, exe-Kopie re15_pc_r41c_mess2.exe): Punkt 5 GANZ (Pult -> 80 V -> "Power supply OK." -> Hebel =
  Original sub04 -> Wasser steigt) und Hebel vorher mit der Aktionstaste gesperrt; Punkt 2 Pult ohne Ventil 13
  (Text 40, zwei Seiten); Punkt 4 Leiter 20B0 -> 2050 und 2050-Leiter -> 20B0; Punkt 6 2030 mit Bit 107 bitgleich
  zum Stand ohne Flut. Belegbilder pult2050_loesung_sheet / pult2030_ohne_ventil / raum2030_leer_vergleich (§4).
  Befund beim Lesen von integration A: ohne den Haken der Spur A haengt die 20B0-Szene (§6, Querbezug A).
* 2026-10-24 00:21 (dritte Fortsetzung, Opus; Vorgaenger vom Fable-Limit ~23:45 beendet). Baum sauber, HEAD 2088dd38.
  Bau `local_build.sh build` -> `ninja: no work to do.` / `LOCAL-BUILD-OK (build)`: die exe vom 23.10. 22:17 ist der
  Stand aller Commits (seit 76fdeca4 nur Dossier/Bilder/Auftrag; `git diff --stat 76fdeca4 HEAD -- re15_port` leer),
  also gelten die Laeufe des Vorgaengers. Aus dessen Protokollen (scratchpad r41c/) uebernommen, NICHT wiederholt:
  Integrationsgruppe 16/16 gruen (Ende 23:46:27, 4908 s), Unit-Gruppe 37/37 gruen (23:46:55), Messlaeufe Ventil 13 /
  Pult -> Bruecke / Rechner 2080 (23:47-23:51) mit den drei Belegbildern 55f511a8 — Logs gelesen, in §4/§5 eingeordnet.
* 22:30 (Fortsetzung) Integrationsgruppe der beruehrten Raeume (16 Haken, seriell) gestartet; Ergebnis §5. Dossier
  §3.3/§4/§5/§6/§7 um die Laeufe der Fortsetzung ergaenzt. Orchestrator-Entscheide C-a..e (AUFTRAG.md 21:37) alle
  angenommen -> keine offene Entscheidungsfrage mehr; die Port-Texte (§7) bleiben Vorschlag bis zur Freigabe.
* 22:24 Ertrink-Blende: gemessen, dass "ABR 0 + Maske Blau" im PC-Renderer (render_pc.c:1282, nur ABR 1/2) als B-F
  gezeichnet wurde -> Blau WEG (Bildmitte F2740 (29,36,29) -> F2760 (30,37,0), gelbgruen). Behoben: ABR 2 (die
  ausgelieferte Form @0x174a) + Maske Rot+Gruen -> (0,0,30) = Blau bleibt (C_belege/flut_blende_vergleich.png).
  integration_r41_ventilkette A+B+C PASS (214 s); Commit 76fdeca4.
* 22:00 Die Flut am Spiel gemessen (exe, RE15_STATE_LOG, C_belege/flut_echt_sheet.png): Spinnen als Spielzustand
  "schon getoetet" ueber ihre Kill-Bits (kein Haken); Warnung zu F820 -> Flut F2621 (= 1800) -> Fuesse F2728 ->
  Spritzer ab F2733 -> Tod F2746 (= +125) -> YOU DIED. integration_r41_ventilkette: A neu getaktet (Debug-Sprung
  blickt auf die Ost-Tuer, nicht auf die Ventile; Ja erst beim 4. Druck, Warnung 1 Druck spaeter), B Leiter-
  Position -26000 (bei -25900 traf der Vorwaertspunkt das Rechteck nicht), C = Flut-Lauf neu.
* 21:50 Fortsetzung nach dem Sitzungslimit (Vorgaenger ~19:25). Bau 1745af8e + e19d755d: LOCAL-BUILD-OK. Gezielte
  Unit-Gruppe (37 Tests): 1 rot = unit_r41_ventilkette D2 (Riegel-Erwartung 324, gemessen 323: der erste der 324
  Schritte laeuft im Marker-Takt) -> Erwartung korrigiert, PASS; Commit 029c5460.
* 18:35 Alle sechs Punkte gebaut und gemessen. unit_r41_ventilkette PASS (227 Pruefungen, Leon + Elza); gezielte
  Unit-Gruppe 18/18. Integrationsgruppe (exe, seriell) laeuft; Ergebnis in §5.
* 17:44 Generator (Texte/Original-Scheiben/Programme/Wasser) + Modul + Haken; erster Bau ab 17:50 (Engine durfte
  re15_render_tile nicht rufen -> Zeichner als Abfrage, main.c zeichnet).
* 17:12 Baum sauber (status leer, HEAD 116e8b0f). AUFTRAG/VERTRAG/UEBERGABE gelesen. SCD-Dumps aller Kettenraeume
  (tools/scd_dump_room.py) + Texttabellen (tools/rdt_msgdump.py) gezogen, Belege unten in §1.

## 0a. Wegweiser je Nutzer-Punkt (Messung vorher / Beleg / Umsetzung / Messung nachher / Riegel)

| Punkt (AUFTRAG.md) | vorher | Beleg | Umsetzung | nachher (exe) | Riegel |
|---|---|---|---|---|---|
| 1 Leiter 2000 gesperrt + Text, bis die Flut lief | §2 (Slot 4 = Tuer nach 2050) | §1.1 @0x0169E, Strahlen @0x0174C.., Form @0x01B12 | §3.1 | §4 "Leiter" (C_belege/leiter2_vergleich.png) | unit B; integration_r41_ventilkette Lauf b; _b Lauf K (Elza 2001) |
| 2 Bruecke oben, Pult -> Ventil 13 -> "Lower the bridge?" -> Fahrt | §2 (Planke y -1800) | §1.3 obj 0 @0x01B6C, Lift-Fahrt @0x01ED2.., Pult/Tank-Lage | §3.2 | §4 "Bruecke", "Punkt 2 am Spiel", "P2 Pult ohne Ventil 13" | unit C; _b Lauf F |
| 3 Ventile -> Warnung -> 60 s -> Flut ueber den Ring -> Tod mit Ertrinken; Ost-Tuer -> 20B0 | §2 (keine Stelle, Cut 14/15 nie) | §1.2 @0x019C2/sub02/sub05, §1.5 Rate @0x00CC8, Blende @0x174a, Spritzer @0x800456b8 | §3.3 | §4 "Flut am Spiel", "Spritzer nachgemessen", "Blende"; Szene 20B0 §6 Querbezug A | unit D/D2; integration_r41_ventilkette Laeufe a/c |
| 4 danach 20B0 statt 2000, Leiter frei (kein Wasser) | §2 (20B0 nur per Sprung) | §1.1/§1.2 (kein Tuersatz -> 20B0, Kamera-Zwillinge, Strahlen @0x01E96..) | §3.4 | §4 "Leiter", "P4 Leiter 20B0 -> 2050", "P4 2050-Leiter -> 20B0" | unit E; _b Laeufe G/H |
| 5 2050: Hebel erst nach dem Pult-Raetsel (80 V) | §2 (Hebel immer bedienbar) | §1.4 sub04 @0x022E6, Slot 8 @0x01D38, Cursor @0x01D4C, 2060/11F0/RE2 ROOM2130 | §3.5 | §4 "Pult 2050", "P5 Pult -> Hebel", "P5 Hebel vorher" | unit F; _b Laeufe D/E/J (Elza 2051) |
| 6 2080-Rechner leert auch 2030 | §2 ((3,245) Raum-Merkflag) | §1.5 sub02 @0x00C86..@0x00CF1 | §3.6 | §4 "Punkt 6 am Spiel", "P6 2030 nach dem Rechner" | unit G; _b Lauf I |

unit = unit_r41_ventilkette (Abschnitte A-I, D2), _b = integration_r41_ventilkette_b, _kette =
integration_r41_ventilkette_kette (nur mit Spur A registriert, Entscheid C-E1). Offen nach Messung: §6 (Querbezug A; Ring-Eintritt in das geflutete 2030 = Frage
§7 f). **Nachbesserung 1 (§0b):** Punkt 5 + Kisten-Hebel (M1, unit F, _b E2/E3/E4), Punkte 3/4 ganze Kette in einem
Lauf mit dem Haken der Spur A (M2, _kette), Blendenfarbe (M4, integration C), Bank 5 (M3, unit H), Text 49 (M8),
Ring-Zugaenge des gefluteten 2030 gesperrt (M5 / C-E2, unit I, _b L/L2/M/M2). **Nachbesserung 2 (§0c):** Punkt 6 nach
der Flut — Tuergraph + Weg zu Fuss: kein Weg zum Rechner (ORIGINAL-Sperre ROOM11A0 Slot 4, (3,206)); OFFEN, Frage §7 (j);
Riegel unit J, integration_r41_ventilkette_weg.
**Nachbesserung 3 (§0d, Nachtraege 6-10):** Punkt 6 durch Nachtrag 7 ersetzt — 2030 laeuft mit dem Strom (3,126) aus dem
Generator 2060 bis auf Relinghoehe (y -3600, nur Schachtoeffnung) leer, Ring-Tueren dann offen, Treppe hinab gesperrt (Text 48),
Rechner 2080 nur Original; Ventilraetsel am Sued-Landeplatz erst mit Bruecke (Bit 102) und Sicherung (3,108), sonst Text 49;
der ganze Weg nach der Flut zu Fuss mit Leon und Elza. Riegel unit K/L (+E/G/I neu), integration _weg/_weg_elza/_sperre.

## 0b. Nachbesserung 1 (Abnahme 0 vom 2026-10-24, `C_abnahme_0.md` §4) — je Mangel: Ursache, Messung vorher, Beleg, Aenderung, Messung nachher

Messordner: scratchpad `r41c_nb1/` (`C:/Users/MJOEDI~1/AppData/Local/Temp/claude/c--workspace-git-reAi-v2/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/r41c_nb1`).
Die Messungen der Abnahme (`abn0/mess/*`) werden NICHT wiederholt, sondern als "vorher" zitiert.

### M1 (hoch, Punkt 5) — Kisten im Becken: Hebel = Ereignis 2 hebt das Wasser ohne das Pult
* **Ursache:** `re15_ventilkette_r41_ereignis` sperrte in 2050/2051 nur Ereignis 4. Derselbe Hebel-Platz Slot 6 traegt
  Ereignis 2, sobald die drei Kisten im Becken liegen — das Original-Kistenraetsel des Raums hob das Wasser dann
  weiter ohne Bit 106.
* **Messung vorher** (Abnahme 0, nicht wiederholt): `abn0/mess/kisten_hebel` — `AOT slot 6 typ 0 ev 2` (F404), Aktion ->
  kein Port-Programm -> sub02: msg 0 -> Ja -> obj 1/2/3 m0 -17700, obj 4 -1400, obj 0 y 0, Cut 4, For 180 ->
  `FLAG (3,109) = 1` bei `FLAG (9,106) = 0`. Gegenprobe des neuen Unit-Riegels am ALTEN Modul (ventilkette_r41.c von
  4af8fa3d eingesetzt, gebaut, gefahren; Protokoll scratchpad `r41c_nb1/unit_gegen.txt`):
  `FAIL: ROOM2050 Kisten ohne Strom: Sperrsatz 40, (3,109)=1, kein Cut 4 (1), Wasser y -16200 -> -3600 (erwartet -16200)`
  (ebenso ROOM2051).
* **Beleg** (selbst aus den Bytes, `tools/scd_dump_room.py` ROOM2050.RDT und ROOM2051.RDT, beide gleich):
  sub01 @0x02006 `06 00 1c 00` Ifel_ck, @0x0200A `21 03 6d 00` Ck(3,109)=0, @0x0200E `21 05 00 01` / @0x02012
  `21 05 01 01` / @0x02016 `21 05 02 01` Ck(5,0..2)=1, **@0x0201A `46 06 03 31 ff 00 18 02 00 00` Aot_reset Slot 6
  sce 3 sat 0x31 -> Ereignis 2**, Endif @0x02024. (5,0)/(5,1)/(5,2) setzt sub01 selbst, wenn Kiste obj 1/2/3 auf Feld
  3/4/5 steht (@0x02036 `3e 00 0f 00 03 00` Member_cmp -> @0x0203C `22 05 00 01` usw. bis @0x0212C).
  sub02 @0x0220E `2b 00 ff ff` msg 0, @0x02218 `21 0c 1f 00` Ja, @0x0221C `22 03 6d 01` Set(3,109), @0x02244
  `34 00 dc ba` Kisten m0 -17700, @0x02260 `34 01 88 fa` obj 4 -1400, @0x02268 `34 01 00 00` Wasser obj 0 y 0,
  @0x02270 `29 04` Cut 4, @0x0227C `2f 01 ec ff` Speed_set(1,-20), @0x02280 `0d 00 06 00 b4 00` For 180 = das Wasser
  steigt 3600 (-> y -3600). Zensus der Ereignis-Plaetze in 2050 (alle Aot_set/Aot_reset mit Nutzlast `18 ee`): Slot 8
  Ereignis 9 (@0x01D38, @0x0241C), Slot 6 Ereignis 4 (@0x01E28, @0x01EB2), Slot 6 **Ereignis 2 (@0x0201A)** — weitere
  Wege, das Wasser zu heben, gibt es nicht.
* **Aenderung:** `RE15_R41C_EV_2050_KISTEN 2` (Kopf, mit den Bytes oben); `re15_ventilkette_r41_ereignis` legt in
  2050/2051 Ereignis 4 UND 2 ohne Bit 106 auf `2050_hebel_sperre` (derselbe Sperrsatz 40, Form ROOM2060 sub12
  @0x015D0). Mit Bit 106 bleibt beides das Original. Scheibe `re15_r41c_org_2050_kisten` (@0x0201A, 10 B) im
  Generator, Riegel A vergleicht sie mit beiden RDTs.
* **Messung nachher:**
  - unit_r41_ventilkette Fall F (Leon + Elza): nach dem Setzen der Kisten-Bits steht Slot 6 auf Ereignis 2; ohne
    Bit 106 `kisten ohne msgs: 40@Cut0  Cuts: 0` ((3,109)=0, Wasser obj 0 bleibt -16200 = @0x01D92), mit Bit 106
    `kisten 106 msgs: 0@Cut0  Cuts: 0 4 7` (Original sub02, Wasser -3600). PASS (255 Pruefungen).
  - exe (scratchpad `r41c_nb1/mess/kisten_hebel`, Aufbau und Wegplan der Abnahme, zu Fuss in die Nische):
    `[ventilkette41] ROOM2050 Ereignis 2 -> Port-Programm 2050_hebel_sperre` -> `[msgtext] room=2050 id=40 "The pump
    has no power. The switchboard must be set first."`; kein Cut_chg(4), kein Member_set der Kisten/des Wassers.
    (Der Plan wartet danach 600 Bilder in der Nische; ohne die Wasser-Szene greifen die Gegner des Raums an und
    toeten Leon — Messmittel, kein Befund; deshalb endet dieser Lauf im Timeout.)
  - Kurzlauf scratchpad `r41c_nb1/mess/kisten2` (Aufbau wie Riegel E2, Framedumps F290-520/10): Aktion F330 ->
    `[ventilkette41] ROOM2050 Ereignis 2 -> Port-Programm 2050_hebel_sperre` -> `[msgtext] room=2050 id=40 ...`, 0x
    Cut_chg(4), Ende `EXIT_AT: Bild 520 in Raum 2050`. Bildbogen `C_belege/nb1_kisten_hebel_sheet.png` (Text 40, kein
    Wasser-Cut; die Sprung-Kamera Cut 0 zeigt die Nische nicht).
  - Riegel integration_r41_ventilkette_b, neue Laeufe E2 (Leon, Kisten ohne Pult -> gesperrt), E3 (Gegenprobe mit
    Bit 106 -> Original sub02: `[msg] room=2050 id=0`, `obj=0x01 m=0 → val=-17700` F456, Cut_chg(4), `obj=0x05 m=1 →
    val=-5000` F657 = das Kistenraetsel bleibt nach dem Pult loesbar), E4 (Elza 2051, Kisten ohne Pult -> gesperrt):
    `Passed 444.40 sec` (§5 "Nachbesserung 1").

### M2 (hoch, Punkte 3/4) — 20B0-Auswasch-Szene braucht den AUTO-Haken der Spur A
* **Ursache (Port, nicht Spur C):** `re15_aot_scan` ueberspringt waehrend einer Szene jeden Platz ausser CAM_SWITCH
  (aot_common.c:1169 `if (in_cinematic && a->type != RE15_AOT_TYPE_CAM_SWITCH) continue;`). Die Auswasch-Szene braucht
  aber die AUTO-Zone Slot 7 WAEHREND der Pad-Sperre von sub02: ROOM20B0 sub00 @0x01DF2 `2c 07 03 41 01 00 08 bc 04 de
  ca 08 f8 11 ff 00 18 05 00 00` (Slot 7, sce 3, sat 0x41 = AUTO, Band 1, Rechteck (-17400,-8700,2250,4600),
  Ereignis 5 = sub05). sub02 setzt @0x01EF8 `22 02 07 01` Set(2,7)=1 und @0x01EFC `22 01 1c 01` Set(1,28)=1
  (Korrektur zu §6: dort stand "(1,27)" — die Bytes sagen Bank 1 Bit 28); erst sub05 loescht @0x01FC0 `22 02 07 00`
  und @0x01FC4 `22 01 1b 00` (Bank 1 Bit 27).
* **Original (selbst disassembliert, PSX.EXE):** die AUTO-Auswertung steht in der UNBEDINGTEN Aufruffolge der
  Hauptschleife @0x8001cdf4..@0x8001ce34 (`jal 0x8001a50c` Aktoren, `jal 0x80031c44` Spieler, `jal 0x8002bd44`
  Objekte, **@0x8001ce1c `jal 0x800436a8`**, `jal 0x8004f0b0`, `jal 0x80019e20` Aktionstreiber); FUN_800436a8 ruft
  als Erstes @0x800436bc-cc `ori a1,zero,0x1` / `jal 0x80042bac` mit dem Spieler — keine Szenen-Abfrage davor. Den
  Port-Haken dafuer hat Spur A gebaut (Zweig r41/wasser20b0, b03b68b4 + 0636c300: `re15_wasser20b0_r41_auto_aot`).
* **Messung vorher (ohne Haken):** Abnahme 0 L3 (`abn0/mess/osttuer`: ab F109 Cut 14, pm 2 bis F1100). Neu im Riegel
  (Spurbaum, kurzer Lauf bis F400 in 20B0, `build/tests/integration/r41_ventilkette_szene/lauf_b2a8b015_kurz`):
  `[scd] Plc_dest(slot=0 mode=0x05 dest=(-15920,-6355) ...)` @0x01F24, danach KEIN `first_op=0x46` (sub05) und kein
  `letterbox closed` bis `EXIT_AT: Bild 400 in Raum 20b0`.
* **Entscheidung im Baum:** der Haken der Spur A wird hier NICHT uebernommen (Orchestrator-Vorgabe der vierten
  Fortsetzung: "der echte Haken kommt mit Spur A bei der Integration; NICHT in deinen Baum uebernehmen"). Ein
  Cherry-Pick ginge auch technisch nicht sauber: b03b68b4 ist ein Sammel-Commit (EM024-Sequencer, Zeichenmaske,
  AUTO-AOT), und 0636c300 aendert dieselbe Zeile erneut (WATER/RAMP) -> Teilstand gegen Endstand = Konflikt.
* **Messung mit dem Haken — die GANZE Kette in EINEM exe-Lauf** (wortgleicher Haken aus 0636c300 als temporaerer
  Messbau: `aot_common.c`-Zeile + `re15_wasser20b0_r41_auto_aot`-Rumpf aus wasser20b0_r41.c:171-176, Skript scratchpad
  `r41c_nb1/haken_a.py`; danach Quelle zurueck (`git status --porcelain` leer) und ohne Haken neu gebaut).
  Lauf scratchpad `r41c_nb1/mess/kette3` (05:53-05:55, Spinnen-Kill-Bits wie Abnahme L3, Eingabe wie L3 + Wegplan):
  2030 Text 45 -> Ja -> Text 46 -> `Tuer Slot 0 -> ROOM20B0 Cut 11 (Zaehler laeuft)` -> Drehung + Aktionstaste ->
  `[aot] DOOR FIRE slot=0 rect=(7350,-200,hw=500,hh=1750) target_cut=11` (kein FIRE_AOT) -> `loaded room20b0.rdt` ->
  `READ Ck zone=3 idx=235 exp=0 got=0` (@0x01AAA) -> `Cut_chg(11)` -> `Zaehler lief beim Verlassen von 2030 -> Bits 103 +
  100` -> Plc_dest Modus 5 -> **`thread start slot=11 first_op=0x46`** (sub05) -> `[scd F105] Cut_chg(7)` ->
  `[scd F146] Cut_chg(15)` -> `[scd F228] Cut_chg(6)` -> **`[flagcensus] room=20B0 WRITE Set zone=3 idx=235 op=1`**
  (@0x01FBC) -> **`[scd F242] letterbox closed -> gameplay (player_mode=0)`** -> Wegplan `FLAG (3,235) = 1`,
  `FLAG (9,103) = 1`, `FLAG (9,100) = 1` -> Mittelgang (-21101,-9618) Blick West + Aktion: Sprung in den Kanal (F334,
  mo 230) -> (-24593,-9618) Blick West + Aktion: Aufstieg auf den Westgang (F493, mo 230) -> Leiter (-26014,10245)
  Blick West + Aktion -> `[aot] DOOR FIRE slot=4 rect=(-27100,10300,hw=500,hh=500) target_cut=0
  spawn=(9100,-5400,4100)` -> `loaded room2050.rdt` -> `EXIT_AT: Bild 60 in Raum 2050`. Bildbogen
  `C_belege/nb1_kette_ein_lauf_mit_haken_a_sheet.png` (2030 Ost-Tuer, Westgang, Cut 7, Cut 15, Cut 6, frei, Sprung,
  Kanal, Aufstieg, Westgang, Leiter ohne Strahl, 2050).
  Weg zur Leiter: Mittelgang (Szenenende) und Westgang (Leiter) sind in Band 1 durch den Kanal getrennt (SCA ROOM20B0
  Zelle 18 x -25200..-21650 Band-1-Wand u0 01, darin WATER-Zone Slot 5); hinueber geht es zu Fuss nur per Sprung/
  Aufstieg an der Kante (Aktionstaste), sonst ueber 1260 (20B0 Slot 0 -> 1260 -> 20B0 Slot 1).
* **Riegel `integration_r41_ventilkette_kette`** (Orchestrator-Entscheid C-E1, AUFTRAG.md 2026-10-24 05:25): genau diese
  Kette in einem Lauf; prueft den Szenen-Abschluss (sub05-Start, Cut 7/15/6, Set(3,235), `letterbox closed`), den Weg
  durch den Kanal und Leiter -> 2050. Registriert in probes/r41_ventilkette.cmake NUR, wenn A's Modul im Baum liegt
  (`if(EXISTS ${CMAKE_SOURCE_DIR}/engine/src/wasser20b0_r41.c)`); sonst Configure-Zeile
  `-- integration_r41_ventilkette_kette uebersprungen: Spur A fehlt (engine/src/wasser20b0_r41.c, AUTO-Haken
  re15_wasser20b0_r41_auto_aot)` (gemessen: `r41c_nb1/configure2.log` Zeile 180; `ctest -N -R r41_ventilkette` listet
  dann nur unit/integration/_b). Ist er registriert, muss die Kette durchlaufen (startet sub05 nicht -> FAIL).
  - mit dem wortgleichen Haken auf dem ENDSTAND dieses Baums (Messbau 06:37 aus 74fb9fae + Haken, `cmake -P` mit einer
    Quellkopie mit Haken, scratchpad `r41c_nb1/riegel_kette_mit_a/lauf_9c9a636d_voll`): `-- r41_ventilkette_kette: 2030
    Ventile -> Ost-Tuer zu Fuss -> 20B0 Szene sub02/sub05 -> (3,235)=1 -> frei -> Kanal -> Leiter -> ROOM2050 in einem
    Lauf (Haken der Spur A: 1)`, `rc=0`.
  - Vorlaeufer (vor dem Fund von C-E1): derselbe Ablauf als `integration_r41_ventilkette_szene` mit Laufzeit-SKIP —
    ohne Haken `***Skipped 61.97 sec`, mit Haken zweimal PASS; durch die Registrier-Regel C-E1 ersetzt.
  Der Orchestrator misst die Kette im Integrationsbaum A+C (C-E1).

### M3 (mittel) — Installer loeschte (5,0x1D)/(5,0x1E) in jedem Raum
* **Ursache:** `re15_ventilkette_r41_install` loeschte die Port-Marker nach dem Init-Lauf in JEDEM Raum; in fremden
  Raeumen sind (5,0x1D)/(5,0x1E) Original-Bits.
* **Messung vorher** (Abnahme 0): `abn0/mess/h1171` — SCD-Trace F250 slot 10 @0x13B2 `22 05 1e 01` im Init-Takt, danach
  der Installer. Gegenprobe des neuen Riegels am alten Modul: `FAIL: ROOM1171: Installer laesst (5,0x1D)/(5,0x1E) stehen`
  und `FAIL: ROOM1171 (3,194): sub02 lief ((3,194)=0) und (5,0x1E) = 0 nach dem Installer (erwartet 1, @0x013B2)`.
* **Beleg:** ROOM1171 sub02 (scd_dump_room.py): @0x01398 `22 03 c2 00` Set(3,194)=0, **@0x013B2 `22 05 1e 01`** vor dem
  ersten Sleep @0x013C6 `09 0a 01 00` (also im Init-Takt), @0x01404 `22 05 1d 01`, erst @0x014A4 `22 05 1e 00` /
  @0x014A8 `22 05 1d 00` loeschen beide (nach dem Warten auf (5,28) @0x013F4). Bank 5 Wort 0 wischt das Raumladen
  ohnehin: FUN_8003ecec @0x8003ed74 `sw zero,4136(at)` = 0x800b1028 (scd_room_setup.c:319 `g_game.flags[5][0] = 0`).
* **Aenderung:** die beiden `re15_game_flag_set(5, 0x1D/0x1E, 0)` laufen nur noch, wenn der Raum 2030/2031 ist
  (ventilkette_r41.c `re15_ventilkette_r41_install`; Kopf-Kommentar der Marker angepasst).
* **Messung nachher:** unit_r41_ventilkette Fall H PASS: Installer in 1171 laesst beide Bits stehen, in 2031 loescht
  er sie; echter Raumstart ROOM1171 mit (3,194) (sub00 -> Gosub sub07 -> Evt_exec sub02) -> (3,194)=0 und (5,0x1E)=1
  nach dem Installer. Querbezug-Riegel der Heliport-Szene (unit_r38/r39_heliport1171_*, integration_r38/r39_heliport1171_*)
  in §5 "Nachbesserung 1".

### M4 (mittel) — Riegel C belegte die Farbe der Ertrink-Blende nicht
* **Ursache:** Riegel C fuhr im Software-Renderer (`RE15_SOFTWARE_RENDER=1`) und pruefte vom Framedump F2750 nur, dass
  die Datei existiert. SDLs Software-Renderer kennt die subtraktive Blende nicht (`[render] SDL-Renderer software ...
  ABR2 (B-F) NICHT unterstuetzt -> Rueckfall BLEND`): die Kachel (160,160,0) wird dort DECKEND gelb gezeichnet.
* **Messung vorher** (Abnahme 0 L3b): Software `abn0/mess/flut2` ab dem Todesbild (160,160,0); Direct3D
  `abn0/mess/flut3` (0,0,30); alle Riegel-Laeufe seit 22:23 gelb.
* **Beleg der Blende:** Form ROOM11F0 @0x174a `56 00 02 07 00 00` (Handler 0x56 @0x80042a58, ABR = Byte 2 = 2 = B-F,
  OT-Fach `ori a3,zero,0x7` @0x80042a70), Maske Rot+Gruen 0x6 (FUN_800217b0 Config-Bits 2+1), Helligkeit
  level >> 7 (`sra a0,v0,23` @0x800218d0) -> 0x5000 = 160 (PORT-WAHL, §6).
* **Aenderung:** Lauf C laeuft im beschleunigten Renderer (direct3d unter Windows, opengl unter Linux — dort seit
  81ee1efc ABR2-faehig, Memory reai-v2-sdl-gl-customblend); der Riegel verlangt `ABR2 (B-F) ok`, legt Framedumps
  F2730-2770/2 an und misst mit dem neuen Werkzeug `test_r41_ventilkette ppm` die Bildmitte (120,80)..(200,160)
  4 Bilder vor und 4-5 Bilder nach dem Todesbild: vorher Rot/Gruen >= 15 (unverschleiert), nachher Rot/Gruen <= 4 und
  Blau >= 20. Schwellen aus der Messung: scratchpad `r41c_nb1/mess/flut_d3d` (direct3d, 05:43), Bildmitte F2700
  (24,29,24), F2740 (30,37,30), **F2750 (0,0,30)**, F2760 (0,0,30), Vollbild F2750 (0,0,30); `[fade-log] config ch=0
  abr=2 mask=6 step=0 bucket=7` / `kick ch=0 value=0x5000`.
* **Messung nachher (Riegel):** `integration_r41_ventilkette` (ctest unten, `Passed 213.05 sec`), Lauf C
  `build/tests/integration/r41_ventilkette/lauf_6b7e0e00_c`: `[render] SDL-Renderer direct3d, Videotreiber windows, ABR2
  (B-F) ok`; Flut F2621 (cam 4), Tod F2746 (hp -1) = 125 Bilder; Bildmitte (Werkzeug nachgerechnet) F2740 (29,36,29),
  F2742 (30,37,30), F2744 (30,37,30), **F2746 (0,0,30)** (die Blende steht im Todesbild selbst), F2748..F2752 (0,0,30).
  Der Riegel wertet F2742 (vorher) gegen F2750 (nachher). Bildbogen `C_belege/nb1_blende_d3d.png` (Messlauf flut_d3d).
* **Einschraenkung (auch §4/§5):** mit `RE15_SOFTWARE_RENDER=1` (nur Mess-/Testbetrieb) erscheint die Ertrink-Blende
  deckend gelb; was der Nutzer sieht (direct3d/opengl/GLES2), ist der blaue Schleier. Eine B-F-Nachbildung im
  SDL-Software-Renderer waere render_pc.c (Spur D) und betraefe alle ABR-2-Flaechen — nicht Teil dieser Spur.

### M5 — geflutetes 2030 ueber den Ring betretbar (Frage f; Orchestrator-Entscheid C-E2, umgesetzt)
* **Ursache:** nach der Flut (Bit 103 ohne 107) fuehrten die Ring-Tueren unveraendert in das geflutete 2030; Leon watete
  dort bis zur Brust (Ertrinken nur waehrend der Flut).
* **Messung vorher:** Abnahme 0 L8 (`abn0/mess/ring`, 20B0 Ost-Tuer -> `DOOR FIRE slot=3 ... spawn=(6550,-5400,-100)` ->
  `ROOM2030: unter Wasser (y -7200)`, Bild `ring_nach_flut_sheet.png`); eigener Lauf ring_nach_flut (§4).
* **Entscheid C-E2** (AUFTRAG.md 2026-10-24 05:25): bis Bit 107 sind die Zugaenge in den gefluteten Ring gesperrt
  (Aktionstaste -> Port-Text, kein Raumwechsel); Ids 20B0/20B1 41, 2070/2071 40 (VERTRAG §1.2 ergaenzt); nach Bit 107
  wieder offen.
* **Beleg — Tuer-Zensus aller ROOM*.RDT** (Skript scratchpad `r41c_nb1/tuer_zensus_2030.py` ueber scd_dump_room.py,
  Door_aot_set mit Ziel pc[22]=1 / pc[23]=3 = ROOM2030):
  - Ring (Ankunft (6550,-5400,-100) Richtung 2048, Cut 0, nfloor 3): **20B0/20B1 Slot 3 @0x019C2** `3b 03 02 31 01 00 4a 15
    24 e1 e8 03 b8 0b 96 19 e8 ea 9c ff 00 08 01 03 00 03` (Rechteck (5450,-7900,1000,3000), Band 1), **2070/2071 Slot 1
    @0x00A5A** `3b 01 02 31 00 00 88 13 24 e1 e8 03 b8 0b 96 19 e8 ea 9c ff 00 08 01 03 00 03` (Rechteck (5000,-7900,
    1000,3000), Band 0) und **Slot 2 @0x00A7A** `3b 02 02 31 00 00 12 b2 24 13 ...` (Rechteck (-19950,4900,1000,3000)).
  - Tuerebene (Ankunft y -16200, nfloor 9 = ueber dem Wasser y -7200): 2020/2021 Slot 1 @0x00866/@0x0088A, 2060/2061
    Slot 0 @0x00D9A, 2080/2081 Slot 0 @0x00BA2 -> bleiben offen. **⛔ BERICHTIGT (Nachbesserung 2, N1):** ein "Weg
    zum Rechner 2080" ist das NICHT — 2020/2060 kommen auf der Nordgalerie an, 2080 auf der Suedgalerie; beide trennt
    Wand 66 (Abnahme 1 L9, SCA ROOM2030 @0x880); die Suedgalerie erreicht man nur per Lift B vom (gesperrten,
    gefluteten) Ring. Den Weg nach der Flut gibt es nicht (§0c: Original-Sperre ROOM11A0 Slot 4, (3,206)).
  - 2070 hat nur msg 0 "You've opened the lock." (Port-Id 40 frei), 20B0 keine Texttabelle (Id 40 = Leiter, Id 41 frei).
* **Aenderung:** `ring_sperren()` (ventilkette_r41.c): bei Bit 103 ohne 107 wird die Tuer (nur wenn sie wirklich nach
  Stage 1 Raum 3 fuehrt) zum Text-Platz sce 1 im Tuer-Rechteck — Form der Leiter 2000 (ROOM2030 main00 @0x01B12), Band aus
  Door_aot_set pc[4] auf den Platz uebertragen; 20B0 Slot 3 -> Id 41, 2070 Slots 1/2 -> Id 40 (neues `install_2070`).
  Wortlaut (PORT-WAHL, Form der Leiter-Vorgabe, zur Freigabe): "I cannot open the door / because of the water pressure."
  Texttabelle: 20B0 traegt jetzt den Leiter-Satz der 2000 (Id 40) UND den eigenen Satz (Id 41); 2000 hat keine Id 41.
  Konstanten im Kopf: `RE15_R41C_SLOT_20B0_RING 3`, `RE15_R41C_SLOT_2070_RING_A/B 1/2`, `RE15_R41C_RAUM_2030_ID 0x03`
  (Door_aot_set pc[23]), `RE15_R41C_MSG_RING_20B0 41`, `RE15_R41C_MSG_RING_2070 40` — je mit den Bytes oben.
* **Messung nachher:**
  - unit_r41_ventilkette Fall I (Leon + Elza): `ROOM20B0 ring zu msgs: 41@Cut0`, `ROOM2070 ring zu msgs: 40@Cut0`,
    `ROOM20B1 ... 41`, `ROOM2071 ... 40`, je kein Raumwechsel (`g_room_change.pending` 0); mit Bit 107 wieder Tuer nach
    Stage 1 Raum 3; 2070 Slot 2 gesperrt, Slot 0 bleibt Tuer (-> 20B0); 2080 Slot 0 (Tuerebene) bleibt Tuer. Texte 20B1/41
    und 2071/40 dekodiert, 2000 ohne Id 41. `PASS (274 Pruefungen)`.
  - integration_r41_ventilkette_b, neue Laeufe (Laufordner `build/tests/integration/r41_ventilkette_b/lauf_54f814b2_*`):
    L (20B0, Bits 103+100+(3,235)) `[ventilkette41] ROOM20B0: Ring-Tuer Slot 3 nach 2030 gesperrt (geflutet) -> Sperrtext
    41` + `[msgtext] room=20b0 id=41 "I cannot open the door because of the water pressure."`, kein `DOOR FIRE slot=3`,
    kein `loaded room2030.rdt`; L2 (+Bit 107) `[aot] DOOR FIRE slot=3 rect=(5950,-6400,hw=500,hh=1500) target_cut=0
    spawn=(6550,-5400,-100)` -> `loaded room2030.rdt`; M (2070, Bit 103) Slot 1 und Slot 2 gesperrt + `[msgtext] room=2070
    id=40 "I cannot open the door because of the water pressure."`, kein Raumwechsel; M2 (+107) `[aot] DOOR FIRE slot=1
    rect=(5500,-6400,...)` -> `loaded room2030.rdt`. `Passed 512.29 sec`.

### M6 (formal) — Haken in main.c / game_step_common.c, zwei fremde Riegel
* VERTRAG §1.3 nennt C nur bei scd_room_setup.c, scd_vm.c und test_r36_kartenbits.c. Die uebrigen Haken (alle klein,
  "Runde 41 Spur C" kommentiert, Muster der Vorrunden-Module feuerloescher_r37 / dining_r38 / johnruf_r39):
  1. platform/pc/main.c `#include "re15_ventilkette_r41.h"` (1 Zeile).
  2. platform/pc/main.c `pc_load_room_prop_set` (5 Zeilen): MD1/TIM der zwei Wasserflaechen obj 4/5 in 2030/2031
     laden — Modelle/Texturen laedt nur die Plattform (Muster dining_r38 direkt darueber).
  3. platform/pc/main.c Boot-/CONTINUE-Weg (1 Zeile): der Lade-Weg ruft scd_room_reenter nicht; ohne die Zeile fehlten
     nach LOAD GAME Sperren/Plaetze (Muster johnruf_r39 direkt darueber).
  4. platform/pc/main.c Zeichner (4 Zeilen): Zeiger + Hebelmarken des 2050-Pults mit re15_render_tile (die Engine-
     Bibliothek hat keinen Zeichner; Muster 11F0-Zeiger direkt darunter).
  5. engine/src/game_step_common.c (1 Zeile): der Spiel-Takt (Zaehler-Latch, Flut/Ertrinken, Pult). scd_vm.c waere
     zugeteilt, aber der Takt laeuft dort an anderer Stelle des Bilds; die gemessenen Zeiten (Zaehler 1800 +- 3, Tod
     125 +- 2 Bilder) haengen an dieser Stelle -> nicht verlegt.
  6. Zwei fremde Riegel (test_r36_dachkiste.cmake +2 Zeilen `RE15_SET_FLAG=9:100`, test_r37_itemband.cmake
     `RE15_SET_FLAG=9:101,9:102`): die neuen Sperren (Leiter 2000, Steg 2030) widerlegen deren alte Wege; die Laeufe
     setzen den Spielzustand "Leiter frei" / "Bruecke unten" (Gegenstand der Riegel ist der Kisten-Record bzw. das
     Itemband, nicht die Leiter/der Steg). Beide gruen (Abnahme 0, Querbezug 8/8).
* **Orchestrator-Entscheid C-E3** (AUFTRAG.md 2026-10-24 05:25): die Haken in main.c (4 Stellen) und game_step_common.c
  (1 Zeile) sowie die Querbezugs-Flags in test_r36_dachkiste.cmake / test_r37_itemband.cmake sind angenommen (wie B5).
  Kein Mangel mehr; kein Code geaendert. (Die Ring-Sperre C-E2 kam ohne neue Haken aus: sie liegt im Installer.)

### M7 (niedrig, Dossier) — 2070 nach der Flut nur ueber 2040
* Richtig ist: nach der Flut (Bit 103) ist 2070 (mit der Tuer nach 11A0) nur noch ueber 2040 erreichbar — 2030 Slot 0
  bleibt auch mit Bit 107 auf 20B0 Cut 11 umgelenkt (`tueren_umlenken`, unit E "mit Bit 107: Ost-Tuer bleibt -> 20B0",
  Abnahme L6 `nach2030`), 20B0 hat (wie im Original-20B0, Slot 3 @0x019C2 -> 2030) keine Tuer nach 2070, und 2040
  braucht den Aufzug mit Strom (Nutzer: "Der Umweg 2070 -> 2040 -> 2050 geht nicht, ... Aufzug der Strom braucht").
  §6 und §3.6 sind korrigiert ("mit Bit 107 alles Original" galt nie fuer die Ost-Tuer).
* **⛔ BERICHTIGT (Nachbesserung 2, §0c):** ROOM2070 sind DREI getrennte Gaenge (SCA ROOM2070 @0x714: Mittelgang
  Slots 0/1 = 2000 <-> Ring, Suedgang Slots 2/3 = Ring <-> 2000, Nordgang Slots 4/5 = 11A0 <-> 2040). Die Tuer nach
  11A0 liegt im Nordgang; den betritt man nur aus 2040 (Galerie, Slot 4 @0x0131C) oder aus 11A0 nach dem Riegel
  (3,151) (2070 sub02 @0x00CE4). Die 2030-Ost-Tuer fuehrte (ohne Umlenkung) in den MITTELgang, nicht zur 11A0-Tuer.
  Einen Aufzug hat 2040 nicht: Halle <-> Galerie = Treppe sce 13 (Slot 5/6 @0x0133C/@0x01350), Halle Sued <-> Nord =
  Rost-Tuer (Zelle 19, frei erst nach der Zeitbombe (3,53) @0x01624; Bombe aus ROOM1110 (3,146) @0x00D32). Nach der
  Flut erreicht man den Nordgang also ueber 2050/2060 -> 2040 -> Rost-Tuer -> Treppe -> Galerie (gemessen §0c Glied 6).
* **Entscheidungsfrage (g):** soll 2030 Slot 0 nach dem Entleeren (Bit 107) wieder nach 2070 fuehren (Original-Ziel,
  2070 und 11A0 wieder direkt erreichbar), oder bleibt 2030 <-> 20B0 dauerhaft (Original-20B0 kennt 2070 nicht)?

### M8 (niedrig, Text) — Ventile melden nach dem Entleeren weiter "flooded"
* **Ursache:** das Programm der Ventile waehlte nur zwischen Frage (vor der Flut) und `2030_ventile_zustand` (Bit 103).
* **Messung vorher:** Abnahme L6/L8; Gegenprobe des neuen Riegels am alten Modul: `FAIL: ROOM2030 mit Bit 107: Ventile =
  Zustandssatz 49 (msgs 1, erste 48)` (ebenso 2031).
* **Aenderung (PORT-WAHL, Wortlaut zur Freigabe):** Id 49 (VERTRAG §1.2: 2030 Ids 40-49) "The valves are open. / The pool
  has been drained." in der Form von Id 48 (Generator `gen_r41_ventilkette.py`, Kodierung + Rueckdekodierung geprueft),
  Programm `2030_ventile_leer` (`2b 31 ff ff` `02 00` `01 00`), gewaehlt bei Bit 107.
* **Messung nachher:** unit_r41_ventilkette Fall E (Leon + Elza): mit Bit 107 zeigt die Aktion an den Ventilen genau
  Text 49; Fall A dekodiert Text 2031/49 woertlich. PASS.

### M9 (niedrig, Doku) — Kopf des Unit-Riegels nannte (5,0x3D)/(5,0x3E)
* Kopf von `tests/unit/test_r41_ventilkette.c`: Zaehler (5,0x1D) / Flut (5,0x1E) (Bank 5 Wort 0, wie der Code), dazu die
  neuen Faelle E (Text 49), F (Kisten), H (fremde Raeume) und das ppm-Werkzeug beschrieben.

### H1-H3 (Hinweise)
* **H1** `RE15_R41C_ERTRINK_TIEFE 900` steht jetzt ausdruecklich als PORT-WAHL mit entliehenem Mass im Kopf. Selbst
  disassembliert (re15_disasm.py, PSX.EXE): @0x8002c1f8 `addiu s6,s6,148` (Objekt-Schleife, Schritt 148), @0x8002c234
  `lbu v1,8(s1)` (Objekttyp), @0x8002c238 `ori v0,zero,0x4`, @0x8002c23c `bne v1,v0,0x8002c254`, @0x8002c244
  `lw v0,96(s1)`, @0x8002c24c `addiu v0,v0,-900`, @0x8002c250 `sw v0,96(s1)` = die y-Lage von Typ-4-Objekten wird
  um 900 angehoben. Keine Koerperhoehe; Ertrinken gibt es im Original nicht. Wert unveraendert.
* **H2** Steht Leon waehrend der Flut an den Ventilen (Ost-Streifen), liegt sein Spritzer ausserhalb der Flut-Kamera
  Cut 4 (Draufsicht); sichtbar sind das steigende Wasser, der blaue Schleier und der Tod. Eine Rueckschaltung auf
  Leons Zonen-Kamera vor dem Tod waere eine neue Kamera-Regel ohne Vorbild -> nicht gebaut, Frage §7 (h).
* **H3** PSX-Ziel: in diesem Baum weder gebaut noch gemessen (keine PSn00bSDK-Kette in der Spur); die Wasserflaechen-
  Daten sind nur `RE15_PLATFORM_PC` (Laufzeitbudget), Bits/Zaehler/Flut/Tod/Sperren sind plattformneutral. Bleibt OFFEN
  fuer die Integration (§6).

## 0c. Nachbesserung 2 (Abnahme 1 `C_abnahme_1.md` Mangel N1; Orchestrator-Entscheid C-E2b vom 2026-10-24 07:49)

Messordner: scratchpad `r41c_nb2/` (`C:/Users/MJOEDI~1/AppData/Local/Temp/claude/c--workspace-git-reAi-v2/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/r41c_nb2`).
Die Messungen der Abnahme 1 (`C41abn1/mess/*`, im Bericht `<a1>`) werden NICHT wiederholt, sondern zitiert.

### N1 (hoch, Punkt 6) — Weg zum Rechner 2080 nach der Flut
* **Auftrag (C-E2b, AUFTRAG.md 07:49):** Tuergraph aller STAGE2-Raeume fuer "nach der Flut" (Bits 103+100, ohne 107)
  aus den Door_aot_set-Zielen + Sperrbedingungen (SCD-Offsets); den Weg 20B0/2050 -> 2080 zu Fuss messen (Leon UND Elza),
  Riegel. C-E2 (Ring-Sperre) bleibt. Steht nur eine PORT-Sperre der Runde 41 im Weg -> anpassen; steht eine ORIGINAL-
  Sperre im Weg oder gibt es den Weg nicht -> nichts erfinden, den blockierenden Punkt (Tuer, Bit, SCD-Offset, Bild)
  benennen und als OFFEN melden. Dossier §7 und §0b M5 richtigstellen.
* **Ursache des Mangels:** der in §7 / Frage f genannte Weg "2050 -> 2020 -> 2030 Tuerebene -> 2080" war aus den
  Tuersaetzen abgeleitet, aber nie gegangen; die Tuerebene von 2030 besteht aus zwei getrennten Galerien (Abnahme 1 L9,
  SCA @0x880 Wand 66). Er existiert nicht.
* **Messung vorher:** Abnahme 1 L9 (`C41abn1/mess/tuerebene3`): 2020 Slot 1 -> 2030 (-450,-16200,-9700) -> Nordgalerie ->
  `KLEMME ... Rest 3918` bei (-6983,-3918); SCA ROOM2030 @0x880 Wand 66. Nicht wiederholt.
* **Tuergraph (statisch, C-E2b Punkt 1)** — Werkzeug `re15_port/tools/r41_ventilkette/tuergraph_nach_flut.py`
  (Walker = tools/scd_dump_room.py, alle Door_aot_set aus main UND subs aller 206 Raum-RDTs (240 Dateien, davon 34
  Platzhalter < 0x60 B ohne Kopf), Ifel/Else-Kontext mit Ck-Offset,
  Setzer jedes Bedingungs-Bits aus allen RDTs, Elza-Saetze Byte fuer Byte verglichen: alle "Elza gleich"); Ausgabe
  `C_belege/nb2_tuergraph_nach_flut.txt`. Ergebnisse, die den Weg entscheiden:
  - **Zugaenge ROOM2080:** nur ROOM2030/2031 Slot 1 @0x01AEA (floor 9 = Suedgalerie der Tuerebene) und ROOM2090/2091
    Slot 0 @0x00996 (Ankunft (-9950,-14400,-25700) nfloor 8). **Zugaenge ROOM2090:** nur 2080 Slot 1 @0x00BC2 (floor 8; bis
    (3,245)=1 inert, davor Text msg 1 "The water level is very high..." sub00 @0x00C06 / Aot_reset @0x00C50) und 20A0 Slot 0
    @0x01746. **Zugaenge ROOM20A0:** nur 2090 Slot 1 @0x009B6 und **ROOM11A0/11A1 Slot 4 @0x01006 — nur im Sonst-Zweig von
    `06 00 1c 00` @0x00FE6 / `21 03 ce 00` Ck(3,206)=0 @0x00FEA; sonst steht dort @0x00FEE `2c 04 01 31 01 00 54 6f c8 00
    e8 03 98 08 02 00 ff ff 00 00` = Text-Platz msg 2 "It's locked from the other side."** (msg @0x142C). **(3,206)=1 setzen
    nur ROOM20A0/20A1 sub10 @0x01FF0 `22 03 ce 01`** (von innen: @0x01FE6 Aot_reset 10, @0x01FF4 Message_on 2 "You've
    opened the lock.", @0x01FFA Aot_reset 2 sce 2 = Tuer nach 11A0) — dieselbe Form "von der anderen Seite verschlossen"
    wie 11A0/2070 mit (3,151) (2070 sub02 @0x00CE4).
  - **2030 Suedgalerie (Tuer nach 2080)** nur ueber Lift B vom Ring (Abnahme 1 L9, SCA); Lift B: Zone oben Slot 6 / unten
    Slot 7 @0x01D74..@0x01DD6 je nach (3,220); Strom Ck(3,126) @0x01E14 = Generator 2060 Set(3,126) @0x012AE, der die
    Sicherung braucht (2060 sub00 Ck(3,108) @0x010A2 / Ck(3,144) @0x010C2); die Sicherung liegt in 2030 Slot 9 @0x01BAA
    (Ck(3,108)=0 @0x01BA6) auf dem Steg-Suedlandeplatz (Band 1). Ring nach der Flut: gesperrt (C-E2, bleibt) und geflutet.
  - **ROOM2070 = drei getrennte Gaenge** (SCA ROOM2070 @0x714, 12 Zellen je Region): Mittelgang z -8200..-4600 (Zellen 0-3;
    Slot 0 -> 2000, Slot 1 -> Ring), Suedgang z 4650..8100 (Zellen 4-7; Slot 2 -> Ring, Slot 3 -> 2000), Nordgang
    z -21000..-17450 (Zellen 8-11; Slot 4 -> 11A0, Slot 5 -> 2040). In den Nordgang fuehren nur 2040 Slot 4 @0x0131C und
    11A0 Slot 0 @0x00F5A (nur mit (3,151), das nur 2070/2071 sub02 @0x00CE4 IM Nordgang setzt; R36 R_dachkiste N1).
  - **Vor der Flut** ist die Menge {2050, 2020, 2030-Nordgalerie, 2060, 2040, 2070-Nordgang, 11A0, 20A0, 2090, 2080,
    2030-Suedgalerie} von aussen nur ueber die Leiter 2000 Slot 4 (bis Bit 100 Sperrtext 40, Punkt 1) und Lift B (Strom aus
    2060, das selbst in der Menge liegt) erreichbar -> **bei der Flut gilt in JEDER Spielreihenfolge (3,206)=0, (3,245)=0,
    (3,126)=0.** Nach der Flut erreicht man von 20B0 aus: 2050 -> 2020 -> 2030 Nordgalerie -> 2060 -> 2040 -> 2070 Nordgang
    -> 11A0 (-> STAGE3 ueber 11A0 Slot 1 @0x00F7C, unbedingt). Nicht: Ring/Steg/Buero/Suedgalerie von 2030, 2070 Mittel-/
    Suedgang, 20A0, 2090, 2080.
  - **Port-Eingriffe der Runde 41** (Abschnitt C der Ausgabe): Leiter (nur vor Bit 100), Tueren nach 2000 -> 20B0, 2030
    Ost-Tuer -> 20B0, Ring-Tueren 20B0 Slot 3 / 2070 Slots 1/2 (C-E2), Lift-Rufzonen oben. **Keine Port-Sperre auf dem Weg
    2050 -> ... -> 11A0 -> 20A0 -> 2090 -> 2080** (C-E2b Punkt 2: nichts anzupassen).
* **Messung nachher — der Weg zu Fuss** (exe 06:37 = HEAD-Code, Bits 103+100 ohne 107; Laeufe scratchpad
  `r41c_nb2/mess/<lauf>`, Skript `r41c_nb2/lauf.sh`, eigene exe-Kopie je Lauf, Wegplan-Treiber RE15_WEGPLAN):
  1. 20B0 -> Leiter -> 2050: Abnahme 1 L4 (nicht wiederholt).
  2. 2050 -> 2020 (`h2050_2020`, Leon; Zustand nach dem 2050-Raetsel (3,109)=1, das seit M1 erst nach dem Pult (Bit 106)
     geht): `ZELLE r0[51] LAUFZEIT: x -7550..-6150 z -4500..850 typ 1 u0 0x00` (RDT-Grundzustand u0 0xff; sub00
     @0x01FEE..@0x01FFE `37 0r 33 00` im (3,109)-Zweig oeffnet sie), Leiter-Ankunft (9100,4100) -> Nordsteg (9100,-6175) ->
     (-6712,-6338) -> durch Zelle 51 (-6886,2386) -> (-5240,3529) Blick Sued -> Aktion -> `[aot] DOOR FIRE slot=1
     rect=(-5150,4450,hw=1000,hh=500) target_cut=0 spawn=(11350,-1800,-3150)` -> `loaded room2020.rdt`.
  3. 2020 -> 2030 Nordgalerie: R36 `integration_r36_dachkiste` weg2020 (Hinweis 2010 (3,114), Szene, Luecke r*[15],
     Slot 1 mit (3,219)) und Abnahme 1 L9 (nach der Flut) — nicht wiederholt.
  4. 2030 Nordgalerie -> 2060 (`h2030n_2060`, Leon, + Spinnen-Kill-Bits 7:155..158 wie L9, (3,219)): (-450,-9700) ->
     (-6874,-9664) -> (-6955,-7368) Blick West -> Aktion -> `[aot] DOOR FIRE slot=3 rect=(-7700,-7250,hw=500,hh=900)
     target_cut=0 spawn=(7600,0,3950)` -> `loaded room2060.rdt`; im Log davor `ROOM2030: unter Wasser (y -7200)`.
  5. 2060 -> 2040 Halle: R36 tritt2060 (Tritt (3,172), Slot 1 @0x00DBA -> (-12850,-5400,1600)) — nicht wiederholt.
  6. 2040 Halle -> Galerie -> 2070 Nordgang (`w2040_leon`, Zeitbombe benutzt: (3,146)+(3,53) gesetzt): Suedgang
     (-14626,1672) -> Verbinder (-14715,-5290) -> Nordgang (4961,-5418) -> durch die Rost-Tuer (Zelle 19 x 4150..5950)
     (5061,-7194) -> (5020,-10382) -> (-1342,-10490) -> (-1356,-13782) Blick Nord -> Aktion -> mo 221 (Treppe sce 13
     @0x0133C) -> (-1363,-9000,-18368) Band 5 -> (-2132,-19815) Blick West -> `DOOR FIRE slot=2` -> (-3900,-19850) ->
     (-19646,-19806) -> `[aot] DOOR FIRE slot=4 rect=(-20450,-19800,hw=500,hh=1000) target_cut=9 spawn=(4650,0,-19200)` ->
     `loaded room2070.rdt`. Gegenprobe `t2040_ohne53` (ohne (3,53)): `KLEMME ... Rest 1618` bei (5050,-5582) vor der
     Rost-Tuer, Aktion -> `[msgtext] room=2040 id=0 "Notice of damage door: ..."` (Slot 8 @0x01456 Ereignis 7 -> sub07
     @0x016E8: msg 0, msg 5 "Will you use the Time Bomb?"; Zeitbombe = ROOM1110 sub03 Set(3,146) @0x00D32; Sprengung sub02
     @0x01604 Set(3,53) @0x01624 + Sca_id_set Zelle 19 frei @0x01684). **Einen "Aufzug 2040" gibt es nicht** (C-E2b nahm
     einen an): Halle <-> Galerie = Treppe sce 13 Slot 5/6 @0x0133C/@0x01350, Halle Sued <-> Nord = Rost-Tuer (Original-
     Raetsel Zeitbombe, sub00 @0x01432/@0x014A0 sperrt Zelle 19 bis (3,53)).
  7. 2070 Nordgang -> 11A0 -> Slot 4 (`w11a0_leon` Leon; `w11a1_elza` Elza 2071/11A1): (-16000,-19300) -> (-18471,-19256)
     Blick West -> Aktion -> `[msgtext] room=2070 id=0 "You've opened the lock."`, FLAG (3,151) 0 -> 1 -> Aktion ->
     `[aot] DOOR FIRE slot=4 rect=(-19400,-19250,hw=500,hh=1000) target_cut=7 spawn=(6500,0,-3200)` -> `loaded room11a0.rdt`
     (Kanal, Wasser-Platz Slot 7) -> Kante (6500,-693) Blick Sued -> Aktion -> mo 230/231 (Aufstieg) -> (6500,-1800,455)
     Band 1 -> Steg nach Osten (Cut 9) -> (28457,-1800,1319) Blick Ost -> Aktion -> **`[msgtext] room=11a0 id=2 "It's locked
     from the other side."`**, FLAG (3,206) = 0 vorher und nachher, kein `loaded room20a0`. Elza: `Spieler-Familie PL04`,
     `loaded room2071.rdt`, `room=2071 id=0 "You've opened the lock."`, `loaded room11a1.rdt`, **`[msgtext] room=11a1 id=2
     "It's locked from the other side."`**. Bilder **C_belege/nb2_11a0_sperre_leon_sheet.png** und
     **C_belege/nb2_11a1_sperre_elza_sheet.png** (Ankunft im Kanal, Aufstieg, Steg Cut 9, der Satz an der Tuer).
  8. Gegenprobe `g11a0_206` ((3,206)=1, Leon, vor Slot 4): `[aot] DOOR FIRE slot=4 rect=(29000,1300,hw=500,hh=1100)
     target_cut=8 spawn=(25800,-3600,-800)` -> `loaded room20a0.rdt` = (3,206) ist an dieser Stelle die einzige Sperre.
  9. Rest des C-E2b-Wegs, NUR wenn (3,206)=1 waere: 20A0 Slot 0 -> 2090 und 2090 Slot 0 -> 2080 feuern (Abnahme 1
     weg20a0/weg2090, nicht wiederholt); NEU `w2080_von2090` (Leon): Ankunft aus 2090 (-9950,-25700) Band 8 ->
     (-10496,-24008) -> (-10541,-8106) -> (-11140,-7350) Blick Sued -> Aktion -> `ROOM2080 Ereignis 2 -> Port-Programm
     2080_rechner_erw` -> msg 0 -> `Cut_chg(7)` -> `Cut_chg(3)` -> `[msgtext] room=2080 id=40 "The treatment pool is being
     drained as well."` -> FLAG (9,107) = 1, (3,245) = 1. Nicht gefahren: die Strecke im Kanalraum 20A0 (11A0-Tuer -> Slot 0)
     und durch das Alligator-Becken 2090 (Gebiet der Spuren B/F/G).
* **Ergebnis (C-E2b Punkt 2):** Den Weg 20B0/2050 -> 2080 nach der Flut gibt es im Port NICHT. Alles bis 11A0 ist mit den
  Original-Raetseln (2050 Pult/Kisten/Hebel, 2010-Hinweis + 2020-Szene, 2060-Tritt, 2040-Zeitbombe, 2070-Riegel) zu Fuss
  begehbar (Glieder 1-7); **blockierender Punkt = ORIGINAL-Sperre ROOM11A0/11A1 Slot 4: main00 @0x00FEA Ck(3,206)=0 ->
  @0x00FEE Text-Platz msg 2 "It's locked from the other side."; (3,206) setzt nur ROOM20A0/20A1 sub10 @0x01FF0, also nur,
  wer 20A0 schon von 2090 aus betreten hat — und 2090 erreicht man nur aus 2080 oder 20A0.** Bilder nb2_11a0/nb2_11a1. Keine
  Port-Sperre der Runde 41 steht auf diesem Weg -> **nichts erfunden, nichts geaendert; OFFEN fuer den Orchestrator**
  (§6, §7 Frage j). Der Ring-Weg (Lift B) bleibt nach C-E2 gesperrt.
* **Aenderung:** KEINE Code-Aenderung am Spiel (C-E2b Punkt 2: die Sperre ist ORIGINAL -> nichts erfinden; eine
  Port-Sperre der Runde 41 steht nicht auf dem Weg). Neu: das Werkzeug `tools/r41_ventilkette/tuergraph_nach_flut.py`
  (+ Ausgabe C_belege/nb2_tuergraph_nach_flut.txt), die Riegel unit_r41_ventilkette Fall J und
  integration_r41_ventilkette_weg, die Belegbilder nb2_11a0/nb2_11a1, die Richtigstellungen in §0b M5/M7, §6, §7 und
  die Entscheidungsfrage §7 (j).
* **Gegner auf dem Weg (kein Befund der Spur, fuer die Messung neutralisiert):** im 2070-Nordgang stehen beim Eintritt
  aus 2040 (Switch-Fall 9) bzw. aus 11A0 (Fall 12) zwei Spinnen (Sce_em_set @0x00C06/@0x00C1A bzw. @0x00C6C/@0x00C80,
  Typ 0x25, Kill-Bit pc[7] 0x62/0x63 = (7,98)/(7,99) ueber das EM-Status-Gate FUN_800420a0 @0x80042120-38, Zone 7 =
  STAGE1-3, re15_em_status_zone). Ohne die Kill-Bits toeteten sie Leon im ersten Riegel-Lauf (`lauf_baaaad3f_a`: nach
  dem Riegel mo 9/214 -> mo 7 bei (-18939,-19818), Game Over); der Riegel setzt sie als "schon getoetet" (wie die
  2030-Spinnen 7:155..158 der Abnahme). 2050 laeuft mit GEGNERWEG (R36-Konvention), 2040/11A0 ohne Gegnerkontakt.
* **Hinweis zur Annahme in C-E2b** ("Ring unter Wasser (bis -16200)"): der Port-Ruhepegel nach der Flut ist y -7200
  (`RE15_R41C_WASSER_Y_RUHE`, §3.6) = 1800 ueber dem Ring (y -5400); -16200 ist die Tuerebene. An der exe steht und laeuft
  Leon dort in Brusthoehe (Abnahme 0 L8, §4 "Eintritt in das geflutete 2030", Bild C_belege/ring_nach_flut_sheet.png).
* **Riegel:** unit_r41_ventilkette Fall J (Zugangs-Zensus + 11A0-Sperre an der Engine, Leon + Elza; `PASS (286
  Pruefungen)`) und integration_r41_ventilkette_weg (die Glieder 2/4/6/7/8/9 an der exe, Glied 6+7 mit Leon und Elza;
  `Passed 439.47 sec`) — §5 "Nachbesserung 2". Beide halten den Befund fest und fallen, sobald jemand die Sperre oder
  die Zugaenge aendert (dann mit dem Entscheid zu Frage (j) neu fassen).

## 0d. Nachbesserung 3 (Abnahme 2 `C_abnahme_2.md` N1-N4 + H-a; Nachtraege 6-10, AUFTRAG.md 2026-10-24 09:17-09:55)

Messordner: scratchpad `r41c_nb3/` (`C:/Users/MJOEDI~1/AppData/Local/Temp/claude/c--workspace-git-reAi-v2/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/r41c_nb3`).
Messungen der Abnahme 2 (`C41abn2/mess/*`, im Bericht `<a2>`) und der Nachbesserung 2 werden zitiert, nicht wiederholt.

### Arbeitsplan (Massstab = N1-N4 + H-a der Abnahme 2; Nutzer-Wortlaut Nachtraege 6-10)
* N2 (Nachtrag 7): Entleeren haengt am Strom (3,0x7E) aus 2060 (Setzer 2060 sub01 @0x012AE, Leser 2030 sub01 @0x01E14),
  nicht am Rechner. Ring-Sperre C-E2 nur bei "Flut gelaufen UND Strom aus". Rechner 2080 = nur Original (sub02, (3,245));
  Port-Programme `2080_rechner_erw`/`2080_rechner_nur2030` und Text 2080 Id 40 entfernen (VERTRAG 1.2: entfallen).
* N3 (Nachtrag 8): Ruhepegel nach dem Strom auf Relinghoehe (aus der SCA belegt), Ring trocken und begehbar, unterer
  Bereich geflutet und gesperrt; Bilder vorher/nachher (Ring-Kameras, Draufsicht Cut 4); Gegenstandsliste unten.
* N4 (Nachtraege 9 + 10): Ventilraetsel erst mit Bruecke unten (Bit 102) UND aufgehobener Sicherung (Taken-Bit aus dem
  RDT-Zensus); Lage der nummerierten Ventile aus den Hintergruenden belegen; vorher Port-Text (Id aus 2030 40-49).
* N1 (Nachtrag 7): der Weg nach der Flut zu Fuss (Leon UND Elza) mit Riegel; unit J und integration `_weg` neu fassen.
* H-a: durch Nachtrag 10 entschieden (Nutzer: "Flut erst mit Sicherung") -> umgesetzt mit N4.

### RE-Belege der Nachbesserung 3 (selbst aus den Bytes/Bildern gelesen, VOR dem Code)
Werkzeuge scratchpad `r41c_nb3/`: `bits_nb3.py` (Flag-Zensus aller ROOM*.RDT mit dem Walker von tools/scd_dump_room.py +
ROH-Gegenprobe), `sca2030.py` (SCA-Zellen + Kameras), `overlay2030.py` (SCA-Bodenzellen in die BSS-Bilder projiziert,
Kameramathematik tools/maske/geom.build_view), `reling.py` (Gelaender-Hoehen), `md1_2080.py` (Wassermodell 2080).
ROOM2031 ist mit ROOM2030 Byte fuer Byte gleich (282076 B, 0 Abweichungen) -> alle 2030-Offsets gelten fuer Elza.

**B1 Strom (3,0x7E) — Zensus aller RDTs (`bits_nb3.py`, 0 ROH-Funde ausserhalb gewalkter Opcodes):**
* Setzer NUR ROOM2060/2061 sub01 @0x012AE `22 03 7e 01` (hinter der Loesungspruefung @0x01296 `21 03 7e 00`, @0x0129A
  `21 05 06 01`, @0x0129E `21 05 07 00`, @0x012A2 `21 05 08 01`, @0x012A6 `21 05 09 00` = Schalter 1,0,1,0, und Evt_exec
  sub13 @0x012AA: msg 2 "It seems that the generator is working now."). **Kein Loescher** (kein `22 03 7e 00`/`07` in
  irgendeinem Raum) -> einmal gesetzt bleibt (3,126) gesetzt.
* Leser: ROOM2030/2031 sub01 @0x01E10 `06 00 1a 00` / @0x01E14 `21 03 7e 00` -> @0x01E18 `46 05 01 31 01 00 ff ff 00 00`
  und @0x01E22 `46 07 01 31 01 00 ff ff 00 00` (ohne Strom sind die unteren Lift-Rufzonen Slot 5 (Lift A) und Slot 7
  (Lift B) Text msg 1; sub01 laeuft jedes Spielbild, FUN_8003f038, Abnahme 2 G1); ROOM2060 sub00 @0x01066, sub01 @0x01296.
* => **Wahl (Nachtrag 7 "Bit 107 mit dem Strom setzen ODER Zustand direkt an (3,0x7E)"): der Zustand haengt DIREKT an
  (3,126)** — dasselbe Bit, mit dem das Original in 2030 die Lifte einschaltet; da es nie geloescht wird, ist es selbst
  der Riegel. Bit 107 wird nicht mehr gebraucht (frei, k_port[]-Zeile entfaellt). Vor der Flut ist 2060 nie erreichbar
  (Leiter-Sperre bis Bit 100, Abnahme 2 L10) -> bei der Flut gilt immer (3,126)=0; "geflutet" = Bit 103 und (3,126)=0,
  "auf Relinghoehe" = Bit 103 und (3,126)=1.
* Rechner 2080 = nur Original: sub00 @0x00C02 `21 03 f5 00` -> Slot 4 Ereignis 2 @0x00C1A, sub02 @0x00C86..@0x00CF1
  (Frage msg 0, Set(3,245) @0x00C94, Tuer Slot 1 nach 2090 an @0x00CAC, Wasser For 148 @0x00CCC) — unveraendert; die
  Port-Programme `2080_rechner_erw`/`_nur2030` und Text 2080 Id 40 entfallen.

**B2 Relinghoehe (SCA ROOM2030 @0x880, Kopf `50 da 0e cb`, 5 Regionen x 76 Zellen, je 12 B w,d,x,z,typ,u0,u1,fl; Band = fl >> 4):**
* Gelaender des UNTEREN Bereichs = Wandzellen Band 1 (fl 0x11, u1 00): Zelle 3 @0x008BC (Steg West x -2550..-2050),
  Zelle 4 @0x008C8 (Steg Ost x -50..450), Zelle 5 @0x008D4 / Zelle 10 @0x00910 (Sued-Landeplatz, z 5850..6350), Zelle 11
  @0x0091C (Nord-Landeplatz, z -6900..-6400). Boeden des unteren Bereichs (u1 01, fl 0x01 = Band 0..1): Zellen 13-17
  @0x00934..@0x00964 (Nord-Landeplatz x -2550..5000 z -8500..-6900, Steg, Sued-Landeplatz x -5600..4800 z 6350..7900).
* Ring = Bodenzellen u1 01 fl **0x23** (Band 2..3): Zellen 36-41 @0x00A48..@0x00A84 (NW-Balkon, Nord-, Ost-, Sued-Streifen,
  SW, West-Gang zu Lift B); Ring-Reling = Wandzellen Band 3 (fl 0x33): 19 @0x0097C (Ost, x 5400..5900), 20 @0x00988 (Nord,
  z -9000..-8500), 21 @0x00994 (Sued, z 8150..8650), 22 @0x009A0 (SW, x -6350..-5850), 31 @0x00A0C / 32 @0x00A18 (NW-Balkon).
* Hoehe der unteren Gelaender im BILD gemessen (`reling.py`, Bildbogen **C_belege/nb3_reling_hoehe_sheet.png**): die
  Wandzellen 11 (Cut 3), 10 (Cut 5) und 5 (Cut 6) bei 0/600/900/1200/1500/1800 ueber dem Landeplatz y -1800 projiziert —
  der Fuss liegt auf 0 (weiss), der obere Handlauf auf **+1800** (magenta) = **y -3600 = Band 2**.
* Dasselbe Mass im selben Stage, Original: ROOM2050 Wasser obj 0 nach dem Kisten-Raetsel y **-3600** @0x01F88 (Laufebene
  dort -5400) = 1800 unter dem Steg. Und die Ring-Bodenzellen beginnen bei Band 2 (fl 0x23).
* => **Ruhepegel nach dem Strom y -3600** (Oberkante der unteren Reling = Band 2): Ring (-5400) 1800 darueber trocken,
  Landeplaetze/Steg/Tank (-1800) 1800 unter Wasser (Kopfhoehe) = "nur noch der untere Bereich geflutet".
* Die Wasserflaeche darf nur die SCHACHTOEFFNUNG decken (das BSS hat keine Tiefe — unter dem Ring gezeichnet laege sie
  im Bild auf dem Ringboden): Innenkanten der Ring-Reling: R1 x -4200..5400 z -8500..-6400 (Nordwand z -8500 = Zelle 20,
  Ost x 5400 = Zelle 19; West bis x -4200, weil dort die Treppe Slot 8 (x -6050..-2350, von -5400 auf -1800) UEBER dem
  Pegel liegt), R1b x -5400..-4200 z -6900..-6400 (suedlich der Treppe bis zur Balkon-Reling Zellen 31/32), R2 x -7700..5400
  z -6400..2450 (West bis zur Aussenwand x -7700 = Zellen 26/27, Zelle 31 Suedkante z -6400, Zelle 23 Nordkante z 2450),
  R3 x -5850..5400 z 2450..8150 (Zelle 22 Innenkante x -5850, Zelle 21 Innenkante z 8150). Vorschau der Flaeche in den
  Kameras (Projektion, nicht das Spiel): scratchpad `r41c_nb3/poly_a.png`/`poly_d.png`.
* Modell der Flaeche = ROOM2080 Prop 0 (MD1 @0x000D98, `md1_2080.py`): 1 Netz, 130 Ecken (13 x 10 Gitter, Schritt 1800),
  108 Vierecke, 1 Normale (0,-4096,0), EIN gemeinsamer UV-Satz `7f 7e 00 78 7f 00 80 00 00 7e 00 00 00 00 00 00` (eine
  Kachel je 1800 x 1800-Viereck, Seite 0x0080, CLUT 0x7800). Fuer die Oeffnung wird ein Netz im SELBEN Format erzeugt
  (gen/*.inc, Generator): Vierecke <= 1800 x 1800 ueber R1/R1b/R2/R3, je eine Kachel mit diesem UV-Satz, dieselbe TIM.
* Unterer Bereich gesperrt: der EINZIGE Weg vom Ring hinab ist die Treppe Slot 8 @0x01B8E `2c 08 0c 31 80 00 5e e8 30 df
  74 0e 78 05 01 00 02 00 00 00` (sce 12, Band 0x80 = jedes Band, FUN_80042cac); einziger sce-12-Satz in 2030; Lift A
  faehrt Ring <-> Buero (Band 6), Lift B Ring <-> Suedgalerie (Band 9). Sperre = Text-Platz im Treppen-Rechteck (Form der
  Leiter 2000 / ROOM2030 @0x01B12), Band 3 (nur vom Ring aus).
* Gegenstaende im unteren Bereich (fuer den Nutzer): Slot 9 @0x01BAA Ereignis 6 = die **Sicherung** (nur solange
  (3,108)=0, @0x01BA6) und Slot 10 @0x01BC0 `50 0a 09 31 01 00 fe 10 fa 19 20 03 e8 03 16 00 0e 00 6c 00 ff 00` =
  Item_aot_set **Item 0x16 "Shotgun Shells" x14** (Taken-Bit (9,108) = pc[18] 0x6c; Name re15_inhalt_r35.h, DEBUG.BIN
  @0x4988), beide Band 1 am Sued-Landeplatz. Slot 11 @0x01BD6 (Item 0x0d x12) liegt im Buero (Band 6), nicht unten.

**B3 Lage der nummerierten Ventile (Nachtrag 9) — aus den Hintergruenden (Bildbogen C_belege/nb3_ventile_lage_sheet.png:
BSS-Cuts mit den projizierten SCA-Bodenzellen, gruen = Band 1, cyan = Band 3):**
* Cut 3/14 (Kamera (-2805,-4065,187), Blick Nord): Schilder **01 02 03** mit Ventilraedern an der Nordwand des Schachts,
  UEBER dem Nord-Landeplatz (Zelle 13), unter dem Ring (Zellen 20/37 oben im Bild). Cut 4 (Draufsicht) und Cut 1 dasselbe.
* Cut 0/15 (Ring, Blick Ost): **04 05 06** an der Ostwand UNTER der Ring-Reling (Zelle 19), ueber dem Becken.
* Cut 5/6/7: **07 08 09** mit Ventilraedern an der Suedwand UEBER dem Sued-Landeplatz (Zelle 16); Rueckprojektion auf die
  Wandflaeche z 7900 (Zelle 7 Innenkante): Schild 09 x ~-2500, rotes Rad x ~-2670, gruenes Rad x ~200..420, 08 x ~1300,
  07 x ~4400 (`ruecks.py`).
* Cut 2: **13** am Tank neben dem Treppenfuss (= Ventil 13, Zelle 2).
* => Die Ventile liegen ALLE im unteren Bereich, keines am Ring. Der bisherige Platz (Ring-Ost-Streifen x 4900..6300
  z -2000..1500 Band 3 = UEBER den Schildern 04/05/06) lag falsch. **Nachtrag 9: der Ausloeser gehoert dorthin** — an die
  Suedwand vor 09/08 (Sued-Landeplatz, nur ueber den Steg = mit gesenkter Bruecke erreichbar; die Nordwand 01-03 erreicht
  man ueber die Treppe auch ohne Bruecke). Rechteck x -3400..1600 z 7400..8400 Band 1 (Wandflaeche z 7900 +- 500 wie der
  Pult-Platz an der Nordwand; frei von Slot 9 x -5900..-5100 und Slot 10 x 4350..5150).
* Nachtrag 10: Sicherung aufgehoben = **(3,108)** — Setzer nur ROOM2030/2031 sub06 @0x01FF2 `22 03 6c 01` (Ja auf msg 2
  "Will you take the Fuse?", dann Aot_reset Slot 9 @0x01FF6 + msg 3 "You've taken the Fuse."), Leser 2030 main00
  @0x01BA6 (Platz nur bei 0) und ROOM2060 sub00 @0x010A2 (Sicherungskasten: ohne (3,108) msg 3 "One of the fuses is
  missing...", mit (3,108) und ohne (3,144) Ereignis 18 "Will you use the Fuse?", sub19 @0x0169E Set(3,144)); kein
  Loescher. Das ist das Taken-Bit (sub06 legt kein Inventar-Item an); es bleibt 1, auch wenn die Sicherung in 2060
  eingesetzt ist. Bedingung des Ventilraetsels = Bit 102 UND (3,108); vorher Port-Text Id 49 (Vorschlag).

### Umsetzung der Nachbesserung 3 (Dateien, Konstanten — je mit Beleg im Kopf von re15_ventilkette_r41.h)
* `re15_ventilkette_r41_zustand_2030()`: TROCKEN (Bit 103 = 0) / GEFLUTET (103, (3,126)=0) / RELING (103, (3,126)=1);
  `RE15_R41C_STROM_BANK/BIT` 3/126 (@0x012AE Setzer, @0x01E14 Leser), `RE15_R41C_SICHERUNG_BANK/BIT` 3/108 (@0x01FF2).
* N2: `ring_sperren()` nur noch bei GEFLUTET; Lift-Rufzonen oben (Slots 4/6) = msg 1 nur bei GEFLUTET; `install_2080`,
  die Programme `2080_rechner_erw`/`_nur2030` (programme.py) und Text 2080 Id 40 (gen_r41_ventilkette.py TEXTE) entfernt;
  Bit 107 frei (Header, k_port[]-Zeile in tests/unit/test_r36_kartenbits.c entfernt).
* N3: bei RELING EINE Flaeche obj 4 bei (0, -3600, 0) (`RE15_R41C_WASSER_Y_RELING`), Netz
  `re15_r41c_wasser_reling_md1` (gen/ventilkette_r41_wasser.inc, Generator `reling_netz()`: MD1-Kopf/Normale/UV-Satz wie
  ROOM2080 Prop 0, EIN gemeinsames Gitter ueber `RE15_R41C_RELING_RECHTECKE` = 117 Ecken, 95 Vierecke <= 1800 x 1800,
  4052 B); `re15_ventilkette_r41_md1_bytes()`/`_obj_id()` liefern bei RELING dieses Netz und nur obj 4 (main.c-Haken
  unveraendert). Treppe Slot 8 (@0x01B8E) -> Band 3 + Sperrtext 48. Ventil-Platz nur noch bei TROCKEN.
  Erste Fassung mit getrennten Teilgittern je Rechteck zeigte im Bild eine Rissreihe entlang z -6400 (T-Stoesse;
  scratchpad `r41c_nb3/probe_c4.png`) -> ein gemeinsames Gitter (Kanten = alle Rechteckgrenzen).
* N4: Ventil-Platz Slot 14 an der Suedwand (`RE15_R41C_VENTILE_RECT` -3400, 7400, 5000, 1000, Band 1); Ereignis 26:
  Zaehler laeuft -> `2030_ventile_laeuft` (Message_on 46), sonst Bit 102 UND (3,108) -> `2030_ventile`, sonst
  `2030_ventile_vorher` (Id 49); nach der Flut kein Platz/kein Programm. Texte 48/49 neu (Vorschlag, §7).

### Messung nachher (exe 10:27, beschleunigter Renderer direct3d, RE15_WINDOW_SCALE=1)
* **Bilder vorher/nachher** (scratchpad `r41c_nb3/serie_bilder.sh`, je Lauf RE15_FORCE_CUT + Framedump F330, Spinnen-
  Kill-Bits 7:155..157, Bits 101/102 + (3,108)): **C_belege/nb3_pegel_vorher_nachher_sheet.png** — Zeilen Cut 0/1/2/4/7/15,
  Spalten "vor der Flut" / "Flut, ohne Strom" / "Flut + Strom (3,126)". Ohne Strom steht die Flaeche ueber dem Ring
  (Leon in Cut 0/15 bis zu den Knien im Wasser). Mit Strom: Ring (gruenes Gitter) in Cut 0/1/2/7/15 trocken, Wasser nur
  in der Schachtoeffnung bis knapp unter die Ventil-Schilder (Cut 1: Nord-Landeplatz mit Tank und Tuer unter Wasser;
  Cut 7/15: 09/08/07/06 bzw. 06/05/04 ueber dem Wasser; Cut 4: Becken, Steg, Landeplaetze ueberflutet; Cut 2: der obere
  Teil der Treppe ragt heraus). Log je Lauf: geflutet `[ventilkette41] ROOM2030: unter Wasser (y -7200), Lift-Rufzonen
  4/6 -> msg 1` + `[prop-render] pi=4 oid=0x04 pos=(-10800,-7200,-12400)` / `pi=5 oid=0x05 pos=(-10800,-7200,3800)`;
  mit Strom `[ventilkette41] ROOM2030: Strom (3,126) -> Wasser auf Relinghoehe y -3600 (Schachtoeffnung), Treppe Slot 8
  -> Sperrtext 48, Ring frei` + `[prop-render] pi=4 oid=0x04 pos=(0,-3600,0)`, KEIN obj 5.

### Nachbesserung 3 je Mangel (Ursache / Messung vorher / Beleg / Aenderung / Messung nachher)
Messungen an der exe 10:27 bzw. 10:31 (Quellstand = Commit 6a51727b ff.), Laeufe scratchpad `r41c_nb3/mess/<lauf>`
(Runner `lauf.sh`, eigene exe-Kopie `re15_pc_nb3c_<lauf>.exe` je Lauf, danach geloescht; Serien `serie_bilder.sh`,
`serie_ventile.sh`, `serie_2060.sh`, `serie_weg.sh`; Wegplaene `plaene/`).

**N2 (hoch, Nachtrag 7) — Entleeren am Strom statt am Rechner**
* Ursache: der Zustand "2030 trocken" hing an Bit 107, das nur das Port-Programm `2080_rechner_erw` setzte; die Ring-Sperre
  C-E2 und die Wasserflaeche fragten Bit 107, nicht den Strom.
* Messung vorher: Abnahme 2 L11 `ring_strom` (Flut + (3,126): 20B0-Ring-Tuer weiter Text 41), `galerie_strom` (mit Strom
  "unter Wasser (y -7200)"), L6 `rechner`/`rechner2` (`2080_rechner_erw`, Set(9,107), Text 40) — nicht wiederholt.
* Beleg: B1 oben ((3,126) nur 2060/2061 sub01 @0x012AE gesetzt, nie geloescht; Leser 2030 sub01 @0x01E14).
* Aenderung: §0d Umsetzung (Zustand GEFLUTET/RELING an (3,126); Ring-Sperre und Lift-Rufzonen nur GEFLUTET; 2080 = Original).
* Messung nachher: unit_r41_ventilkette E/G/I (Leon + Elza): mit Strom Zustand RELING, Ring-Tueren 20B0 Slot 3 / 2070
  Slot 1 sind Tueren nach 2030 und die Aktion loest den Raumwechsel aus, Lift B unten Slot 7 = Ereignis 4; 2080 in drei
  Zustaenden (ohne Flut / Flut / Flut + Strom) Ereignis 2 ohne Port-Programm -> Original sub02: msg 0, Cut 7 -> 3,
  (3,245)=1, Wasser -3400, kein Bit 107. exe g2 (Leon): der Generator setzt (3,126) ueber das ORIGINAL-Raetsel
  (`[flagcensus] room=2060 WRITE Set zone=5 idx=6 op=1` Schalter 1 -> `zone=5 idx=8 op=1` Schalter 3 -> `zone=3
  idx=126 op=1` -> `[msgtext] room=2060 id=2 "It seems that the generator is working now."`); exe w3 (Leon): danach in
  2030 `Strom (3,126) -> Wasser auf Relinghoehe y -3600 ...`, KEIN "unter Wasser (y -7200)".

**N3 (hoch, Nachtrag 8) — Relinghoehe, unterer Bereich gesperrt**
* Ursache: es gab nur "geflutet -7200" oder "ganz trocken" (Bit 107).
* Messung vorher: Abnahme 2 L11; Bildbogen nb3_pegel_vorher_nachher_sheet.png Spalte "Flut, ohne Strom".
* Beleg: B2 oben (untere Reling Oberkante +1800 = y -3600 im Bild, Band 2 der Ring-Bodenzellen fl 0x23, ROOM2050 -3600
  @0x01F88; Schachtoeffnung aus den Ring-Reling-Zellen; Treppe Slot 8 @0x01B8E einziger Weg hinab; Gegenstaende unten).
* Aenderung: §0d Umsetzung (eine Flaeche obj 4 mit dem Netz der Schachtoeffnung auf y -3600, Treppe -> Text 48 Band 3).
* Messung nachher: Bildbogen **C_belege/nb3_pegel_vorher_nachher_sheet.png** (Ring trocken, nur die Schachtoeffnung unter
  Wasser); unit E: Treppen-Aktion vom NW-Balkon = Text 48, Spieler bleibt auf y -5400; ohne Flut bleibt Slot 8 Treppe.

**N4 (mittel, Nachtraege 9/10) — Ventilraetsel erst mit Bruecke UND Sicherung**
* Ursache: der Ventil-Platz lag am Ring-Ost-Streifen (ueber den Schildern 04/05/06, ohne Bedingung).
* Messung vorher: Abnahme 2 L11 `ventile_ohne_102` (ohne Bruecke Frage 45 -> Warnung 46 -> Zaehler).
* Beleg: B3 oben (alle nummerierten Ventile im unteren Bereich, Bildbogen nb3_ventile_lage_sheet.png; (3,108) nur 2030
  sub06 @0x01FF2).
* Aenderung: §0d Umsetzung (Platz an der Suedwand vor 09/08, Bedingung Bit 102 UND (3,108), sonst Text 49).
* Messung nachher: unit K (Leon + Elza) und exe v2 (Leon, Wegplan zu Fuss): `AOT slot 14 typ 0 ev 26 flags 0x31 band 1 mitte
  (-900,7900) halb (2500,500)` -> Aktion an den Ventilen ohne Sicherung -> `Port-Programm 2030_ventile_vorher` + `[msgtext]
  room=2030 id=49 "Numbered valves. If I turn them, this whole level will be flooded.I should look around here first."`,
  FLAG (3,108) = 0, kein Zaehler -> Sicherung (Original Slot 9 -> sub06) `[msgtext] room=2030 id=2 "Will you take the
  Fuse?"` -> `id=3 "You've taken the Fuse."`, FLAG (3,108) = 1 -> Ventile `Port-Programm 2030_ventile` -> 45 -> 46 ->
  `Zaehler laeuft: 1800 Takte` -> zu Fuss zurueck (Steg, Treppe mo 221, Ring) -> `[aot] DOOR FIRE slot=0 rect=(7350,-200,
  hw=500,hh=1750) target_cut=11` -> `loaded room20b0.rdt` -> `Zaehler lief beim Verlassen von 2030 -> Bits 103 + 100`;
  keine Flut, kein Ertrinken. Zeit: Warnung zu F1702 -> Tuer-Aktion F2776 = **1074 Bilder (36 s) GEHEND**, also
  726 Bilder Reserve auf die 60 s (Wegplan geht nur; laufen ist schneller). Wiederholung ab der Ost-Tuer (Lauf v2b, die Kamera
  schaltet dann wie im Spiel: Cut 5 vor 09, Cut 4 Steg, Cut 2 Treppe, Cut 0 Ost-Tuer): Warnung zu F2491 -> Tuer F3568 =
  1077 Bilder; Bildbogen **C_belege/nb3_ventile_zu_fuss_leon_sheet.png** (Text 49 vor dem Schild 09, Sicherung am
  Westende mit Ja/Nein, "You've taken the Fuse.", Frage 45, Warnung 46, zurueck ueber Steg, Treppe, Ring, Ost-Tuer). Ohne Bruecke ist der Platz nicht erreichbar
  (Steg-Zelle 12 u0 0xff; exe v1 `ZELLE r0[12] ... u0 0xff`), mit Bit 102 ohne Sicherung Text 49 (unit K, exe D).

**N1 (hoch, Nachtrag 7) — der Weg nach der Flut zu Fuss**
* Ursache: der Weg hing am Rechner 2080, den man nach der Flut nicht erreicht (Abnahme 2 L9/L10).
* Messung vorher: Abnahme 2 L9 (`weg_leon`/`weg_elza` bis zur 11A0-Sperre), L10 (Zensus) — nicht wiederholt.
* Beleg: B1 (Strom), Tuersaetze und Lifte 2030 (§1.3, Abnahme 2 L10: Lift B Slot 6/7 @0x01D74..@0x01DD6 je nach (3,220),
  Strom-Pruefung sub01 @0x01E14; Suedgalerie-Tuer Slot 1 @0x01AEA nach 2080; 2080 Tuer Slot 1 @0x00BC2 nach 2090 erst
  nach sub02 @0x00CAC).
* Aenderung: keine eigene — der Weg folgt aus N2/N3 (Ring mit Strom offen und trocken, Lift B mit Strom original).
* Messung nachher (zu Fuss, Wegplan; Raetsel ausserhalb der Kette als geloest gesetzt, Spinnen als getoetet):
  - w1 (Leon): 20B0 (Ende der Szene (-20620,-9705)) -> Kanal (Sprung hinab bei x -21150, Aufstieg bei -24600) -> Leiter
    `DOOR FIRE slot=4 rect=(-27100,10300,hw=500,hh=500)` -> 2050 -> `DOOR FIRE slot=1 rect=(-5150,4450,...)` -> 2020 ->
    `DOOR FIRE slot=1 rect=(-14200,3300,hw=1000,hh=500) target_cut=13 spawn=(-450,-16200,-9700)` -> 2030 Nordgalerie
    (`unter Wasser (y -7200)` = ohne Strom) -> `DOOR FIRE slot=3 rect=(-7700,-7250,hw=500,hh=900)` -> `loaded room2060.rdt`.
  - g1 (Leon): Sicherungskasten 2060 -> msg 3 "One of the fuses is missing..." -> msg 4 "Will you use the Fuse?" -> Ja ->
    `Cut_chg(8)` -> Cursor obj 4 RIGHT 6 Bilder in Zone 7 -> SQUARE -> `WRITE Set zone=3 idx=144 op=1` -> msg 5 "You've
    used the Fuse." (Original sub18/sub19 @0x0169E).
  - g2 (Leon): s. N2 (Generator-Raetsel: Cursor LEFT 23 + DOWN 17 -> SQUARE Schalter 1, RIGHT 30 -> SQUARE Schalter 3 ->
    (3,126)).
  - w3 (Leon): 2060 -> `DOOR FIRE slot=0 rect=(8450,3950,hw=500,hh=1000) target_cut=13 spawn=(-6950,-16200,-7200)` ->
    2030 Nordgalerie (`Strom (3,126) -> Wasser auf Relinghoehe y -3600 ...`) -> `DOOR FIRE slot=2 rect=(-450,-10400,
    hw=900,hh=500) target_cut=5 spawn=(-14200,-1800,2450)` -> 2020 -> `DOOR FIRE slot=0 rect=(11400,-3850,hw=1000,hh=500)
    target_cut=5 spawn=(-5150,-5400,3450)` -> 2050 -> `DOOR FIRE slot=0 rect=(9100,5700,hw=1000,hh=1000) target_cut=9
    spawn=(-26450,-1800,10250)` -> `loaded room20b0.rdt`. Erste Fassung lief in 2060 direkt nach Norden und schob den
    Tritt obj 2 (-3143,2075) bzw. blieb an Zelle 8 (x 6300..8400 z -7750..-5250) haengen -> Weg ueber x -4500 / x 6000.
  - w4 (Leon): 20B0 (Leiter-Ankunft (-26450,10250)) -> Kanal in Gegenrichtung (Sprung hinab bei x -25700 Blick Ost,
    Aufstieg bei -22250, mo 230) -> Mittelgang -> Ost-Tuer **`DOOR FIRE slot=3 rect=(5950,-6400,hw=500,hh=1500)
    target_cut=0 spawn=(6550,-5400,-100)`** (mit Strom KEIN "Ring-Tuer Slot 3 ... gesperrt") -> 2030 Ring (`Strom (3,126) ->
    Wasser auf Relinghoehe ...`) zu Fuss Ost -> Sued -> SW -> West-Gang -> Lift B Kabine (-9800,3600): FLAG (3,220) 0 ->
    Aktion -> `Cut_chg(9)` (Fahrt) -> FLAG (3,220) = 1 -> Suedgalerie -> **`DOOR FIRE slot=1 rect=(-7700,6400,hw=500,hh=900)
    target_cut=0 spawn=(-23200,-9000,-16700)`** -> 2080 -> Treppe (Slot 2 sce 12, Aktion an der Ankunft Blick Ost) ->
    Rechner (-11160,-7315): `[msgtext] room=2080 id=0 "From here maybe I can control the water level.Will you operate the
    computer?"` -> Ja -> `Cut_chg(7)` -> `Cut_chg(3)`, FLAG (3,245) 0 -> 1, **kein Port-Programm, kein Text 40** ->
    **`DOOR FIRE slot=1 rect=(-9150,-25650,hw=500,hh=500) target_cut=0 spawn=(-8200,0,-25550)`** -> `loaded room2090.rdt`.
    (Erste Fassungen: Ost-Tuer-Zielpunkt x 4900 tol 150 endete bei x 4804 -> Vorwaertspunkt 5424 knapp vor dem Rechteck
    x 5450 -> kein Feuer; 2090-Tuer von Sueden blockiert bei x -9668 -> von Westen (Blick Ost) angefahren.)
  - **Elza** (ROOM20B1/2051/2021/2031/2061/2081/2091, `Spieler-Familie PL04`): v1, v2, g1, g2, w1, w3, w4 dieselben Saetze
    (scratchpad `r41c_nb3/mess/*_elza`): v1 Zelle 12 0xff -> 41/42 -> 43 -> Cut 4 -> obj 0 -1800 -> Zelle 12 0x00; v2 49 ->
    Sicherung -> 45/46 -> Zaehler -> Ost-Tuer -> `loaded room20b1.rdt` + Bits 103/100, keine Flut; g1 `room=2061 WRITE Set
    zone=3 idx=144`; g2 `room=2061 ... idx=126`; w1 bis `loaded room2061.rdt` mit `ROOM2031: unter Wasser (y -7200)`; w3
    `ROOM2031: Strom (3,126) -> Wasser auf Relinghoehe ...` bis `loaded room20b1.rdt`; w4 bis `loaded room2091.rdt` mit
    `[msgtext] room=2081 id=0 "From here maybe I can control the water level..."` und FLAG (3,245) 0 -> 1.
* Riegel (neu gefasst, H-d der Abnahme 2): **integration_r41_ventilkette_weg** (Leon) und **_weg_elza** (Elza) = Glieder
  v1/v2/g1/g2/w1/w3/w4 mit den Saetzen oben; **integration_r41_ventilkette_sperre** = die frueheren Laeufe a/b/c (die
  Original-Sperre 11A0 Slot 4 bleibt, Nachtrag 6); unit_r41_ventilkette J (Zensus + Sperre) bleibt, K/L neu.

## 1. RE-Belege (Original, selbst aus den Bytes gelesen)

Alle Datei-Offsets beziehen sich auf `re15_port/shared_assets/PSX/STAGE2/ROOMxxxx.RDT` (Leon); die Elza-Varianten
ROOM2001/2031/2051/2081/20B1 sind in den hier zitierten Regionen byte-gleich (der Riegel vergleicht alle Scheiben
in beiden Fassungen).

### 1.1 ROOM2000 — die Leiter nach 2050 (Punkt 1)
* Tuersaetze main00 @0x0161E..@0x016BD (fuenf `3b`): Slot 0 @0x0161E -> ROOM1260 (hinauf), Slot 1 @0x0163E -> ROOM1260
  Cut 8, Slot 2 @0x0165E -> ROOM2010, Slot 3 @0x0167E (Rechteck (-20300,-8200,1000,4000)) -> ROOM2070 Cut 3 (pc[22..25] `01 07 03 00`; Cut 13 tragen
  umgekehrt die 2070-Tueren nach 2000, §1.2),
  **Slot 4 @0x0169E = die Leiter nach ROOM2050**:
  `3b 04 02 31 01 00 30 94 48 26 e8 03 e8 03 8c 23 e8 ea 04 10 00 04 01 05 00 03 00 00 00 00 00 00`
  = sce 2, sat 0x31, Band 1, Rechteck x -27600 z 9800 1000x1000, Ziel (9100,-5400,4100) Richtung 1024,
  Stage 1 Raum 5 (= ROOM2050) Cut 0, nfloor 3.
* Die drei Wasserstrahlen an der Leiterwand (aus dem Gitterfenster ueber der Leiter, Bild C_belege/leiter2_vergleich.png
  unten rechts): main00 @0x0174C/@0x0175C/@0x0176C je `3a 00 0b 00 00 00 00 19 xx xx 4c eb c0 2b 00 04` (ESP 0x0b,
  x -23900/-23300/-22700, y -5300, z 11200) — IMMER an (kein Ck davor); je Satz 6 Stroeme (re15_esp.c:1006).
* Kein Skript in 2000 liest (3,235); kein Tuersatz in irgendeinem Raum fuehrt nach ROOM20B0 (Port-Tuertabellen
  gen/tuer_zuordnung.inc + gen/re15_tuer_eigen.inc: 0 Treffer "-> ROOM20B0"). Das Original schaltet 2000 -> 20B0
  also NICHT selbst; 20B0 ist nur per Debug-Menue erreichbar -> der Port lenkt um (§3.4).
* FORM fuer "Tuer wird zum Sperrtext" im selben Stage: ROOM2030 main00 @0x01B0A `06 00 1c 00` Ifel_ck, @0x01B0E
  `21 03 db 00` Ck(3,219)=0 -> @0x01B12 `2c 02 01 31 09 00 ba fa 6c d5 08 07 e8 03 00 00 ff ff 00 00` (Text-Platz
  Slot 2, msg 0 "It's locked from the other side.", GLEICHES Rechteck/Band wie die Tuer) / Else @0x01B2A Tuersatz
  Slot 2. Aot_reset-Form `46 ss 01 31 mm 00 ff ff 00 00` (ROOM2030 sub01 @0x01E18).

### 1.2 ROOM20B0 — die geflutete 2000-Variante (Punkte 3/4)
* main00 @0x01962..@0x01A90: dieselben fuenf Tueren wie 2000, aber Slot 3 @0x019C2 ist die OST-Tuer
  `3b 03 02 31 01 00 4a 15 24 e1 e8 03 b8 0b 96 19 e8 ea 9c ff 00 08 01 03 00 03 ...` = Rechteck (5450,-7900,
  1000,3000), Ziel (6550,-5400,-100) Richtung 2048, Stage 1 Raum 3 = ROOM2030 Cut 0, nfloor 3 (der Ring, Band 3).
  In 2000 fuehrt derselbe Slot 3 nach 2070 — die Entwickler haben in 20B0 den Umweg 2070 weggelassen.
* sub00 @0x01AA6 `06 00 64 03` Ifel_ck + @0x01AAA `21 03 eb 00` **Ck(3,235)=0 = Flut noch nicht gelaufen**:
  35 Emitter EM024 (`44 nn 24 05 01 ...` @0x01AAE..@0x01D56, x 7200, y -1800, z -8150..-4750 = die Wasserwand
  vor der Ost-Tuer; Spur A repariert den Effekt), Tuerfluegel obj 3/4 (West) @0x01D6A/@0x01D8C, obj 5/6 (Ost,
  x 5850) @0x01DAE/@0x01DD0, Zone Slot 7 sce 3 Ereignis 5 @0x01DF2 (Rechteck (-17400,-8700,2250,4600), Band 1),
  **@0x01E06 `04 ff 18 02` Evt_exec sub02 = Eintrittsszene**.
  Else @0x01E0A: obj 3 liegt am Boden (-23450,0,-4300) rot (0,-2880,-1024) @0x01E0E, obj 4 angelehnt rot 256,
  die drei Strahlen @0x01E96/@0x01EA6/@0x01EB6 (gleiche Bytes wie 2000 @0x0174C ff.).
* sub02 @0x01EF8: `22 02 07 01` Pad-Sperre, `22 01 1c 01`, `29 0b` Cut 11, `3c 01`, Work_set(1,0) Spieler,
  @0x01F08 `32 00 4c 09 f8 f8 2d e7` Pos_set (2380,-1800,-6355), @0x01F10 `34 04 00 08` Richtung 2048,
  Evt_exec sub03/sub04 (Ost-Fluegel obj 5/6 schwingen: `2f 04 80 00` / `2f 04 80 ff`, For 8), @0x01F24
  `40 00 05 00 d0 c1 2d e7` Plc_dest Modus 5 nach (-15920,-6355), Gosub 8 (wartet auf (5,32)=0).
* sub05 @0x01F5C (Zone Slot 7 = Auswasch-Szene): Cut 7, Pos_set (-15400,-1800,-7305), Plc_dest..., sub06/sub07
  (West-Tuer fliegt: obj 4 `2f 04 40 fe` For 4; obj 3 Keyframes @0x02038..@0x020C6), Cut 15, Sleep 60, Cut 6,
  Plc_ret, Cut_auto 1, **@0x01FBC `22 03 eb 01` Set(3,235)=1**, Sca_id_set 0x13/0x14 -> ff.
* ROOM20B0 hat KEINE Texttabelle (msg_sec 0). Port-Id 40 (Leiter-Satz) kommt per re15_msg_install_text.
* Kameras (RDT+0x60, 32 B je Cut, Tabellen 2000/20B0 selbst verglichen): 20B0 Cut 0..4 = 2000 Cut 0..4; 20B0 5/6/7/8/9
  = 2000 6/7/8/9/10; 20B0 10 = 2000 14; 2000 Cut 5/11/12/13/15 ohne Zwilling. Tueren nach 2000 tragen Cut 0
  (1260 @0x114A), 4 (1260 @0x11AA), 10 (2050 @0x1C4A), 13 (2070 @0xA3A/@0xA9A), 14 (2010 @0x746).

### 1.3 ROOM2030 — Bruecke, Lifte, Ventile (Punkte 2/3)
* main00 @0x01ACA Tuer Slot 0 (OST, Ring Band 3): `3b 00 02 31 03 00 c2 1a 62 f8 e8 03 ac 0d c0 12 00 00 64 e7 00 08
  01 07 00 00 ...` = Rechteck (6850,-1950,1000,3500), Ziel (4800,0,-6300) Richtung 2048, Stage 1 Raum 7 = ROOM2070
  Cut 0 (2070 = der Gang zwischen 2000 und 2030: Slots 0/3 @0xA3A/@0xA9A -> 2000, Slots 1/2 @0xA5A/@0xA7A -> 2030).
  Slot 1 @0x01AEA -> ROOM2080 (Band 9), Slot 2 @0x01B2A -> ROOM2020 (nur wenn (3,219)=1), Slot 3 @0x01B4C
  -> ROOM2060 (Band 9).
* **Bruecke = obj 0 @0x01B6C** `2d 00 00 00 00 00 0a 00 00 00 cd fb f8 f8 ed fe 00 00 00 00 ...` = Typ 0, Band 0,
  Lage (-1075,-1800,-275), rot 0; MD1 @0x0020CC 636 B = 18 Dreiecke, x -899..900, y 0, z -5685..5686 (die
  gelb-schwarz markierte Gitterplanke; tools/rdt_objekt_ansicht.py). Von keinem Skript bewegt (kein `2e 03 00`).
  Die Planke ist NICHT im BSS gemalt (Bild C_belege/bruecke_vergleich.png: hochgefahren liegt darunter nur das Becken).
* Lift A obj 1 (6725, -5400 | -10800, 12050) sub00 @0x01CFA/@0x01D48, Lift B obj 2 (-10675, -5400 | -16200, 4050)
  @0x01D9C/@0x01DEA. **FORM der Objekt-Fahrt (derselbe Raum!)**: sub03 @0x01ED2 `2e 03 01` Work_set(3,1),
  @0x01ED6 `2f 01 88 ff` Speed_set(1,-120), @0x01EDA `0d 00 06 00 2d 00` For 45 { `30` Add_speed, `02` `02`
  Evt_next x2 }, @0x01EE6 `34 01 d0 d5` Member_set(1,-10800). Rueckweg @0x01EF2 `2f 01 78 00` Speed_set(1,+120),
  For 45, @0x01F02 `34 01 e8 ea` Member_set(1,-5400). 45 x 120 = 5400 = die Hoehendifferenz. Lift B: For 90
  (@0x01FB4/@0x01FD0), 90 x 120 = 10800. => 120 Einheiten je Add_speed, Add_speed jeden zweiten Takt, Endlage per
  Member_set(1). Speed_set steht IMMER direkt vor dem Add_speed desselben Objekts (Speed ist je Faden, scd_vm.c
  op_speed_set LAB_80040f14 `sh thread+0x158`).
* Pad-Sperre fuer eine Objektfahrt: sub02 @0x01E30 `22 02 07 01` ... @0x01EC4 `22 02 07 00`; Cut_auto 0 @0x01E34,
  Cut_auto 1 @0x01EC2; Kamera der Fahrt `29 0a` @0x01E60 (Lift A oben) / `29 07` @0x01E8E (unten).
* Texte: msg 0 "It's locked from the other side." @0x2014, 1 "The elevator is not responding...Maybe it should be
  activated from somewhere." @0x2038, 2 "Will you take the Fuse? " @0x208B (Ja/Nein-Form `1b 03 02 01 00`), 3 "You've
  taken the Fuse." @0x20AC (`57 01 00`). Port-Ids 40..49 frei (VERTRAG).
* Ebenen (Runde 40, auswahl.json "ebene"): Ring Band 3 y -5400 (Cuts 0/1/2/7/8/15), Mittelsteg Band 1 y -1800
  (Cuts 3/5/6/14), Buero Band 6 y -10800 (10/11/12), Tuerebene Band 9 y -16200 (9/13). PSX-y: kleiner = hoeher.
* SCA (RDT+0x20, 5 Regionen x 76 Zellen, 12 B: w, d, x, z, type, u0, u1, floor): Zelle 12 (-2050,-5950,2000,11400)
  Band 1 u0 **00** = begehbarer Steg (in allen 5 Regionen gleich), Zelle 2 (-3050,-6900,1000,500) Band 1 = der Tank
  am Treppenfuss, Zelle 20 (-5900,-9000,11800,500) Band 3 = Nord-Gelaender, Zelle 24 (-9700,-12350,18950,2000) Band 3
  = Nordwand (Flaeche z -10350), Zellen 18/19 Ost-Wand/-Gelaender (x 7250 / 5400..5900). Sca_id_set-Form (Region,
  Zelle, u0): ROOM2050 sub04 @0x02346 `37 0r 32 ff` (Port op_sca_id_set -> sca_entry_at(region, index)->u0).
* Zone Slot 8 @0x01B8E `2c 08 0c 31 80 00 5e e8 30 df 74 0e 78 05 01 00 02 00 00 00` sce 12 = TREPPE (Port
  RE15_AOT_TYPE_STAIR, p0 1) (-6050,-8400,3700,1400): vom Nordwest-Balkon des Rings hinab zum Steg-Landeplatz.
* Kameras: Cut 2 @0x000A0 (3883,-8101,-7091)->(-11568,-5157,-9778) = schaltschrank.png; Cut 4 @0x000E0
  (-4253,-9340,7013)->(-1841,-4606,2549) = Draufsicht auf Becken + Steg (Projektion des Stegs Bild x 100..210,
  y 73..221, tools/maske/geom.build_view) = Fahrt-Kamera; Cut 15 @0x00240 flag 1 = Schachtmitte oben auf die Ost-Tuer.
* Stelle Pult: Rueckprojektion des roten Markers in schaltschrank.png (Cut 2, Bild (45,128)) auf die Nordwand
  z = -10350 -> Welt (-89,-7079); Pultkanten Bild (30,60)/(20,180) -> x 125..706, y -8506..-6242 (Skript
  scratchpad proj2030.py, Kamera-Konvention gegen die Treppe geprueft: Zone NW (-6050,-5400,-8400) -> Bild (168,136),
  SE (-2350,-5400,-7000) -> (197,169), NW unten -> (168,202) = die Treppe im Bild).
* Stelle Ventil 13: der Tank rechts der Treppe = SCA Zelle 2 (Band 1), Landeplatz x -2550..-50, z -8500..-6900.
* Stelle Ventile (Platzhalter): Ost-Streifen x 5900..7250 an der Ost-Tuer; Cut 15 zeigt dort die Schilder 04/05/06.
* RVD (RDT+0x28, 20 B je Zone, from/to): 1->2 bei x -1500..-500 z -11000..-8500 (Nordstreifen), 2->3 bei x -4000..-3000
  z -9000..-6500 (Treppe), 3->4 bei z -5500..-4500 (Steg), 4->5 bei z 5000..6000.
* Sicherung: (3,108)=0 -> Slot 9 sce 3 Ereignis 6 @0x01BAA (-5900,6650,800,1000) **Band 1 = Suedlandeplatz des
  Stegs**; sub06 @0x01FE4 Frage msg 2, @0x01FF2 `22 03 6c 01`. => Die Bruecke oben sperrt den Weg zur Sicherung.
* Bank 5 in 2030: 0 Treffer `21 05`/`22 05` (frei fuer Marker). Bank-5-Wort 1 (Bits 0x20..0x3F) wird am Ende JEDES
  VM-Laufs geloescht (FUN_8003ebf4 @0x8003ec1c, scd_vm.c:862), Wort 0 beim Raumladen (FUN_8003ecec @0x8003ed74).

### 1.4 ROOM2050 — Hebel, Wasser, Schaltpult (Punkt 5)
* Hebel: sub00 (3,109)=0-Zweig: (3,104)=0 -> @0x01E28 `2c 06 03 31 03 00 5c 12 e2 d2 20 03 20 03 ff 00 18 04 00 00`
  Slot 6 sce 3 **Ereignis 4 = Hebel** (Rechteck (4700,-11550,800,800), Band 3), Slot 7 sce 0.
  (3,104)=1 -> Slot 7 sce 1 msg 1 "The water is blocking the entrance." @0x01EC6.
* sub04 @0x022E6 (Ereignis 4): `2b 00 ff ff` msg 0 "A lever to fill and drain water. Will you move it? ", `02`,
  @0x022EC Ifel_ck, @0x022F0 `21 0c 1f 00` Ja, Pad-Sperre `22 02 07 01` `22 01 1b 01` `22 02 02 01`, Sleep 1;
  @0x02304 Ifel_ck (3,104)=0: @0x0230C `22 03 68 01` Set(3,104)=1, obj 4 y -1400, Sleep 20, `29 04` Cut 4, BGM,
  @0x02324 `2f 01 ec ff` Speed_set(1,-20), @0x02328 `0d 00 06 00 b4 00` For 180 { Add_speed, Nop, Evt_next, Nop },
  obj 5 y -5000, Slot 7 -> sce 1 msg 1, Sca_id_set 0..4 0x32 ff; Else @0x0235A: Sleep 20, Cut 4, `22 03 68 00`,
  obj 4 `2f 01 14 00` Speed_set(1,+20) For 180, Member_set(1,-16200) obj 4/5, Slot 7 sce 0, Sca 0x32 00;
  @0x023AE Freigabe, `29 07` Cut 7, `3c 01`.
* Wasserobjekt obj 0 (Typ 0, Band 5): (3,109)=0 -> y -16200 @0x01D92 (ausser Sicht), (3,109)=1 -> y -3600
  @0x01F88. Kistenraetsel-Ende sub02 @0x02264 `2e 03 00` + @0x02268 `34 01 00 00` (y 0 = Beckengrund), dann
  @0x0227C `2f 01 ec ff` Speed_set(1,-20) + @0x02280 For 180 = Wasser steigt 180 Takte um 20 je Takt = 3600 -> y -3600
  (1800 unter dem Steg -5400). **= die FORM "Wasser steigt" (Objekt-Fahrt der Wasserflaeche).**
* Schaltpult: main00 @0x01D30 `06 00 3c 00` Ifel_ck + @0x01D34 `21 05 07 01` Ck(5,7)=1 -> @0x01D38 `2c 08 03 31
  03 00 a0 0f fe de 08 07 20 03 ff 00 18 09 00 00` Slot 8 sce 3 Ereignis 9 (Rechteck (4000,-8450,1800,800), Band 3)
  + @0x01D4C Cursor obj 8 `2d 08 04 00 00 00 00 01 00 00 9e b3 00 00 9c 58 ...` Typ 4 (-19554,0,22684) (= der Cursor
  der Cursor-Raetsel, re15_hebetisch_cursor.h). (5,7) wird in 2050 NIRGENDS gesetzt (grep `22 05 07`: 0) -> tot.
  sub09 @0x023F2 `29 0f` Cut 15 (2D-Pult), `46 08 00..` Slot 8 aus, `22 05 04 01` `22 05 05 01` `22 05 06 01`.
  sub01 @0x02182..@0x021E0: (5,4)=1 & Sce_key_ck 0x01/0x04/0x02/0x08 -> sub05..08 (Cursor obj 8 `2f 02 c8 00` /
  `2f 02 38 ff` / `2f 00 c8 00` / `2f 00 38 ff` + Add_speed); @0x021E2 (5,5)=1 & Sce_key_ck 0x40 -> sub10 (Ausstieg
  @0x0240C: Cursor parken @0x02414, Slot 8 wieder sce 3 Ereignis 9 @0x0241C, (5,4..6)=0, (2,0)/(2,2)=0, Cut_old,
  Cut_auto 1); @0x021FA (5,6)=1 -> Set(2,0)=1 Set(2,2)=1. KEINE Schalterzonen, keine Spannung, kein Ergebnis.
* Vorbild Schalterlogik ROOM2060 (Generator): Zonen `2c 03 05 44 ...` Slots 3..6 (sce 5, sat 0x44, 2050x2050)
  @0x00F34..@0x00F70; sub01: `21 05 01 01` + `2e 03 03` Cursor + `3e 00 0f 00 03 00` Member_cmp(0x0F == Zone 3) +
  `51 01 40 00` Sce_key_ck 0x40 (= SQUARE, pad_common.c vtbl Bit 6) -> Evt_exec sub08 @0x011D6; sub08 @0x014BA
  (5,1)=0, Hebel obj 5 `2f 03 c0 ff` For 16 / Else `2f 03 40 00`, Bit (5,6) kippt, (5,1)=1. Loesung @0x01296..
  @0x012AA `21 03 7e 00` `21 05 06 01` `21 05 07 00` `21 05 08 01` `21 05 09 00` -> Evt_exec sub13 + @0x012AE
  `22 03 7e 01`. Einstieg sub12 @0x015BA..@0x015FE (Frage msg 0, Ja, Slot 8 aus, `29 07`, (5,0..5),(5,12)=1).
* Vorbild Zielwert 80: ROOM11F0-Pult (include/re15_panel_zeiger.h; RE2 ROOM2130 sub04 @0x01752 `23 00 05 00 50 00`
  cmp(var5 == 80), Ruhe 30 Bilder @0x0171C, Deckel 100 @0x011C8, Bestaetigung se 0x0C @0x01762, Klick se 0x0A
  @0x01192). Texte ROOM11F0 msg 1 @0x18D8 "Will you push them? " (25 B), msg 2 @0x18F1 "Power supply OK." (24 B).
* Cut 15 @0x00240: (-19628,-17442,22616) -> (-19628,15928,22617) (Nadir, wie 11F0 Cut 10). Projektion (geom.build_view):
  Cursor (-19554,-900,22684) -> Bild (160.9,119.1); +200 x -> +2.515 px; +200 z -> -2.515 px (H 208, vz 16542).
  BSS-Bild ROOM20515.ppm (probe_bg_dump): drei grosse Hebel Mitten x 88/133/178, kleiner x 238, Koerper y 85..165;
  Skala Ziffernspalten (dunkle Pixel y 62..67): "0" 165.5, "90" 248.5, "100" 256.5 -> 0.91 px je Punkt; rotes Band
  (r > g+40, r > b+40) y 74 x 239..256 = 80..100; Laufschiene hell y 70..73, Grundlinie dunkel y 75.

### 1.5 ROOM2080 — der Rechner (Punkt 6)
* sub00 @0x00C02 `21 03 f5 00` Ck(3,245)=0: Slot 3 sce 1 msg 1, Slot 4 sce 3 Ereignis 2 @0x00C1A `2c 04 03 31 08 00
  ee d0 ae e3 e8 03 e8 03 ff 00 18 02 00 00` (Rechner, (-12050,-7250,1000,1000), **Band 8**), Wasser obj 0 @0x00C2E
  `2d 00 03 02 09 00 0a 00 1a 00 d8 dc d0 d5 e8 ea 00 00 00 04 ...` (Typ 3, Band 9, (-9000,-10800,-5400), rot_y 1024,
  Flags 0x000a|1, Attribut 0x001a), Tuer Slot 1 aus @0x00C50. Else @0x00C5E obj 0 bei y -3420 (Wiedereintritt).
* sub02 @0x00C86..@0x00CF1 (108 B, Ereignis 2): msg 0, Ja -> @0x00C94 `22 03 f5 01` Set(3,245)=1, Slot 4/3 aus,
  Tuer Slot 1 an @0x00CAC, Pad-Sperre, Sleep 20, `29 07` Cut 7, @0x00CC4 `2e 03 00` Wasser, @0x00CC8 `2f 01 32 00`
  Speed_set(1,+50), @0x00CCC `0d 00 06 00 94 00` For 148 { Add_speed, Evt_next x2 } (148 x 50 = 7400 -> y -3400),
  Sleep 20, Cut 3, Cut_auto 1, Freigabe, Plc_ret @0x00CEC, Endif @0x00CEE, Evt_end @0x00CF0. Ifel-Laenge @0x00C8E 0x60.
* Wasserflaeche obj 0: MD1 @0x000D98 4572 B (108 Vierecke, x 0..21600, y 0, z 0..16200, Ursprung in der Ecke),
  TIM @0x026408 16928 B.

### 1.6 Zuteilung Bank 9 (VERTRAG §1.1) — Benennung (k_port[] in test_r36_kartenbits.c eingetragen)
| Bit | Name | Bedeutung |
|---|---|---|
| 100 | RE15_R41C_BIT_LEITER_FREI | 2000/20B0: Leiter nach 2050 frei (Nutzer: "nach dem Steigen des Wassers") |
| 101 | RE15_R41C_BIT_VENTIL13 | 2030: Zulaufventil 13 geoeffnet |
| 102 | RE15_R41C_BIT_BRUECKE_UNTEN | 2030: Bruecke gesenkt |
| 103 | RE15_R41C_BIT_FLUT_GELAUFEN | 2030-Ventile geoeffnet und Raum verlassen -> 2000 wird 20B0 |
| 106 | RE15_R41C_BIT_PULT2050 | 2050: Schaltpult auf 80 V |
| 107 | RE15_R41C_BIT_2030_LEER | 2080-Rechner gelaufen -> 2030 wieder ohne Wasser |

## 2. Messung vorher (gebauter Stand master 116e8b0f, exe-Laeufe + Dumps)
* ROOM2030 per Debug-Sprung (Ost-Tuer, Cut 0): die Bruecke obj 0 steht bei y -1800 (Mittelsteg), begehbar; drei
  Spinnen greifen binnen Sekunden an (main00 Switch auf den Eintritts-Cut, Case 0 @0x01C12). Kein Pult-Platz, kein
  Ventil-Platz (AOT-Slots 12..14 frei), Cut 14/15 nie geschaltet (Befund Runde 40 bestaetigt).
* ROOM2000: die Leiter Slot 4 ist eine Tuer nach 2050; die drei Strahlen laufen immer (C_belege/leiter2_vergleich.png
  unten rechts zeigt die Wand mit Strahlen; vorher ohne Text).
* ROOM20B0 nur per Debug-Sprung erreichbar; (3,235)=1 zeigt die Strahlen (18 Effektplaetze, Riegel B).
* ROOM2050: Hebel sub04 immer bedienbar ((3,104) kippt); Pult-Zone Slot 8 nie angelegt ((5,7)=0), Cursor obj 8 nicht
  im Pool; Cut 15 nie geschaltet.
* ROOM2080: Rechner senkt nur das eigene Wasser; (3,245) ist ein Raum-Merkflag.
Bilder der BSS-Cuts (probe_bg_dump): scratchpad r41c/bg/sheet_203.png, sheet_205.png, sheet_200.png, sheet_20B.png.

## 3. Umsetzung (Dateien, Konstanten — jede mit @0x…/Byte-Offset im Kopf von re15_ventilkette_r41.h)

### 3.1 Punkt 1 — Leiter 2000 (`install_2000`)
Slot 4 (@0x0169E) wird bei Bit 100 = 0 zum Sperrtext 40 (re15_aot_retype sce 1, sat 0x31, Maske ffff; Slot-Band aus
door_params wie adaruf_1050.c:231, Form ROOM2030 @0x01B12). Text 40 = NUTZER-VORGABE woertlich
"I cannot climb up the ladder because of the water pressure." (gen/ventilkette_r41_texte.inc, Glyphen msg_common.c).
Mit Bit 100 bleibt Slot 4 die Tuer. Dasselbe in 20B0/20B1 (dort nur bei Debug-Sprung ohne Bit 100).

### 3.2 Punkt 2 — Bruecke oben, Pult, Ventil 13 (`install_2030`, Programme 2030_pult_*, 2030_v13_*)
* Bruecke: solange Bit 102 = 0 steht obj 0 bei y -5400 (PORT-WAHL: die Ringebene = die naechste Ebene ueber Band 1,
  Lift-Endlage @0x01F02 `34 01 e8 ea`), und die Steg-Zelle 12 ist in allen 5 Regionen u0 := ff gesperrt (Form Sca_id_set
  `37 0r 0c ff`, ROOM2050 @0x02346). Die Sicherung (Suedlandeplatz, Band 1) ist damit erst nach dem Senken erreichbar.
* Plaetze (Ereignis-Saetze in der Form ROOM2050 @0x01D38 `2c ss 03 31 bb 00 … ff 00 18 ee 00 00`, Port-Ereignisse
  24/25/26, Slots 12/13/14): Pult (-300,-10850,1300,1000) Band 3 (Nordwand, §1.3), Ventil 13 (-3050,-6900,1300,1000)
  Band 1 (Tank = Zelle 2, vom Landeplatz nach Westen), Ventile (4900,-2000,1400,3500) Band 3 (Ost-Streifen, Schilder
  04/05/06). Vorwaertspunkt 620 (FUN_80042bac) in den Riegeln mit den gemessenen Spielerlagen belegt.
* Programme (gen/ventilkette_r41_programme.inc, jede Zeile mit Vorbild): Pult ohne Bit 101 -> msg 40 ("gauge reads
  zero … Intake valve 13 is closed."); Ventil 13: Frage msg 41 -> Ja -> Set(9,101) + msg 42, danach Zustand msg 47
  (Klick = RE2-Panel-se 0x0A beim Kippen, Flanke von Bit 101 im Takt); Pult mit Bit 101: Frage msg 43 "… Lower the
  bridge?" (NUTZER-VORGABE) -> Ja -> `22 02 07 01` `3c 00` `29 04` (Cut 4 Draufsicht, PORT-WAHL als Fahrt-Kamera wie
  `29 0a` @0x01E60) `2e 03 00` `2f 01 78 00` For 30 { `30 02 02 00` } `34 01 f8 f8` (30 x 120 = 3600 in der Lift-Form
  sub03) + Sca_id_set Zelle 12 := 00 x5 + Set(9,102) + Sleep 20 + Cut_old + Cut_auto 1 + `22 02 07 00`; danach msg 44.
  Nein-Zweig = nichts (Ck(12,31) wie sub04 @0x022F0).

### 3.3 Punkt 3 — Ventilraetsel-Platzhalter, 60 s, Flut, Ertrinken (Programm 2030_ventile, C-Takt `flut_tick`)
* Platz an den Ventilen: Frage msg 45 (zwei Seiten, PORT-WAHL-Wortlaut) -> Ja -> Warnung msg 46 ("WARNING: The water
  will rise in one minute. Leave the room!", NUTZER-VORGABE sinngemaess) -> `22 05 1d 01` Marker -> **Sleep 1800**
  (`09 0a 08 07` = 60 s bei 30 SCD-Takten; Hintergrundfaden wie ROOM2000 sub07 @0x01B36, Spieler frei) -> Pad-Sperre
  `22 02 07 01` + `22 02 02 01` + `3c 00` + `29 04` -> zwei Wasserflaechen (obj 4/5 = ROOM2080 Prop 0, §1.5) per
  Pos_set auf den Beckengrund y 0 (x -10800, z -12400 / 3800; decken x -10800..10800, z -12400..20000) -> `22 05 1e 01`
  -> For 324 { Work_set(3,4) Speed_set(1,-50) Add_speed, Work_set(3,5) Speed_set(1,-50) Add_speed, Evt_next } (Rate
  des Rechners @0x00CC8, 324 x 50 = 16200 = bis zur Tuerebene) -> Sleep 60.
* C-Takt (game_step_common.c-Haken): Marker (5,0x1D) -> Zaehler-Latch je Raumstart, Ost-Tuer Slot 0 -> ROOM20B0
  Cut 11 (Ziel der 20B0-Szene @0x01F00/@0x01F08; Gegenweg der 20B0-Tuer @0x019C2), Wasserflaechen angelegt und bei
  z 60000 geparkt. Marker (5,0x1E) -> Flut: steht die Flaeche 900 ueber den Fuessen (RE15_R41C_ERTRINK_TIEFE, halbe
  Koerperhoehe = Typ-4-Anhebung -900 @0x8002c24c), dann hp := -1 (= RE15_KILL_AT main.c, Todeskette game_step_common.c
  :1413) + blaue Blende Kanal 0 (Form ROOM11F0 @0x174a `56 00 02 07 00 00` + `57 00 <level>`: ABR 2 = subtraktiv, der
  einzige Blendmodus der ausgelieferten Skripte (Zensus aller RDTs: nur 11F0/11F1/3080, immer `56 00 02 07`, Pulse
  `57 00 00 08`); hier Maske 0x6 = Rot+Gruen (FUN_800217b0 Config-Bits 2+1, fade_common.c), d. h. die Kachel zieht
  Rot und Gruen ab und Blau bleibt stehen = blauer Schleier; Level 0x5000 = Helligkeit 160 (level >> 7 @0x800218d0),
  PORT-WAHL. Erste Fassung "ABR 0 + Maske Blau" war ein Defekt, s. §4 "Blende")
  + EIN Spritzer am Spieler, wenn die steigende Flaeche seine Fuesse erreicht (`wy <= pl->y`, einmal je Flut;
  re15_esp_fx_spawn_rows mit dem Wasser-Schritt-Effekt 0x06050D00 = `lui a0,0x605` / `ori a0,a0,0xd00`
  @0x800456b8/bc in FUN_80045630, Gier `lh a1,106(a1)` @0x800456d0, `jal 0x80019700` @0x80045710). Das Original
  setzt diesen Effekt nur je Fuss-Aufsetzer (Bildwort-Bit 0x4000 `andi v0,v1,0x4000` @0x8003253c -> Fuss 7 - 3*Bit12
  @0x80032548-60 -> `jal 0x80045630` @0x8003255c, Wasserboden `andi v0,s0,0x80` @0x800456ac); waehrend der Flut steht
  der Spieler unter der Pad-Sperre, es gibt keinen Aufsetzer -> PORT-WAHL "ein Ereignis, keine Rate". Die erste
  Fassung "alle 6 Takte" (RE15_R41C_SPRITZER_TAKT 6) hatte keine Adresse und ist entfernt (ddb65cfc).
  Jeder Tod in 2030 (Ertrinken oder Spinnen: tick `if (hp < 0) s_timer = 0`) loescht den Zaehler, also setzt der
  naechste Raumstart (Titel -> LOAD GAME) keine Flut-Bits; der Raumstart loescht Marker, Blende, Zustand.
* Verlassen waehrend des Zaehlers (jede Tuer): der Installer des naechsten Raums setzt Bits 103 + 100 (Nutzer:
  "Verlaesst man den Raum fuehrt das zu ROOM 20B0"); ueber die Ost-Tuer landet man in 20B0 und dessen Original-Szene
  sub02/sub05 laeuft (Cut 11, Auswasch, Set(3,235)).

### 3.4 Punkt 4 — 20B0 statt 2000, Leiter frei (`tueren_umlenken`, `espr_unterdrueckt`)
Bei Bit 103 bekommt jede Tuer mit Ziel Stage 1 Raum 0 (= ROOM2000; 1260 Slots 1/4, 2010 Slot 0, 2050 Slot 0, 2070
Slots 0/3) dest_room 0x0B (ROOM20B0, Szenario-Nibble bleibt, aot_common.c dest_id) und den Zwillings-Cut
(re15_ventilkette_r41_cut_20b0: 0->0, 4->4, 10->9, 14->10, 13->7; Tabellen §1.2); 2030 Slot 0 -> 20B0 Cut 11 dauerhaft.
In 20B0/20B1 ueberspringt op_sce_espr_on bei Bit 100 genau die drei Sce_espr_on @0x01E96/@0x01EA6/@0x01EB6 (PC-Offset
gegen den RDT-Puffer; Haken scd_vm.c) -> "kein Wasser mehr runter laufen" (Bild C_belege/leiter2_vergleich.png).

### 3.5 Punkt 5 — Schaltpult 2050 sperrt den Hebel (`install_2050`, `panel_tick`, Programme 2050_*)
* Hebel (Ereignis 4) bei Bit 106 = 0 -> Sperrsatz msg 40 (Form ROOM2060 sub12 @0x015D0); mit Bit 106 das Original sub04.
* Pult: Slot 8 + Cursor obj 8 werden aus den Original-Saetzen @0x01D38/@0x01D4C angelegt (ohne (5,7)); Ereignis 9 ->
  Beschreibung msg 43 + "Will you push them? " (ROOM11F0 msg 1 Bytes) -> Ja -> sub09-Bytes `29 0f` `46 08 00…`
  `22 05 04 01` `22 05 06 01` OHNE `22 05 05 01` (sonst beendet SQUARE das Pult, sub01 @0x021E2). Die Original-Subs
  sub05..08 bewegen den Cursor (Riegel: RIGHT = sub07 @0x023DA).
* C-Takt bei (5,4)=1: Cursorlage gegen vier Hebelzonen (2400 x 4400 um die Hebelmitten, aus den Bildpixeln §1.4
  rueckgerechnet: x -26550/-22972/-19393/-14622, z 19800..24200); SQUARE-Flanke (virtuell 0x40) auf einer Zone kippt
  den Hebel + Klick se 0x0A; Gewichte **30/20/40 (gross) + 10 (klein) = PORT-WAHL**, einzige Loesung 30+40+10 = 80
  (Riegel zaehlt alle 16 Masken); 30 Ruhebilder (RE2 @0x0171C) -> Bit 106 + se 0x0C + "Power supply OK." (11F0 msg 2)
  -> Ausstieg = Original sub10 (Cursor parken, Slot 8 wieder Ereignis 9, Cut_old). CROSS (0x8000) = Abbruch (sub10).
  Danach ist Slot 8 ein Zustandssatz msg 44.
* Zeichner (main.c, neben dem 11F0-Zeiger): roter Laeufer x = 165.5 + 0.91 * Wert auf der Schiene y 70..73 (RE2
  ROOM2130: Zeiger unter der waagerechten Skala; Rot wie der 11F0-Zeiger) + gruene Marke 8x3 unter jedem EIN-Hebel
  (PORT-WAHL, Farbe der 11F0-Lampe). Bild C_belege/pult2050_sheet.png: F560 Hebel 3 EIN -> Zeiger auf 40, F620 Hebel 1
  dazu -> 70.

### 3.6 Punkt 6 — Rechner 2080 entleert 2030 (`install_2080`, Programme 2080_rechner_*)
Bei Bit 103 und nicht 107: Ereignis 2 = Original sub02 Byte fuer Byte (@0x00C86..@0x00CED) + im Ja-Zweig `22 09 6b 01`
Set(9,107) + msg 40 "The treatment pool is being drained as well." (Ifel-Laenge 0x60 -> 0x6A). War der Rechner
schon gelaufen ((3,245)=1), legt der Installer Slot 4 (@0x00C1A) erneut an; dann nur Frage msg 0 + Slot aus + Set +
Satz (kein zweites Senken). In 2030 gilt: Bit 103 ohne 107 = zwei Wasserflaechen bei y -7200 (1800 ueber dem Ring,
Abstand wie ROOM2050 Wasser -3600 zum Steg -5400 @0x01F88) und die oberen Lift-Rufzonen (Slots 4/6, sub00 @0x01D20/
@0x01DC2) = Original-Satz msg 1 "The elevator is not responding…" (Form sub01 @0x01E18); mit Bit 107 alles Original
ausser der Ost-Tuer (Slot 0 bleibt 20B0 Cut 11, Bit 103 — Nachbesserung 1, M7) und dem Ventil-Satz (Id 49, M8).

### 3.7 Gemeinsame Dateien (Haken, je 1-6 Zeilen, "Runde 41 Spur C")
scd_room_setup.c (Installer-Zeile am Blockende + include), scd_vm.c (Ereignis-Kette hinter nahaufnahmen_r40 +
op_sce_espr_on-Sprung + include), game_step_common.c (Takt hinter johnruf_r39), platform/pc/main.c (Boot-/CONTINUE-
Installer, Prop-Lader fuer obj 4/5, Zeichner-Block, include), tests/unit/test_r36_kartenbits.c (6 Zeilen k_port[]).
Neue Dateien: engine/src/ventilkette_r41.c, include/re15_ventilkette_r41.h, engine/src/gen/ventilkette_r41_*.inc,
tools/r41_ventilkette/{gen_r41_ventilkette.py,programme.py}, tests/unit/test_r41_ventilkette.c,
tests/unit/probes/r41_ventilkette.cmake, tests/integration/test_r41_ventilkette.cmake. Keine Aenderung an
shared_assets/PSX, pri/, synchro/, release/.

## 4. Messung nachher (exe, RE15_WINDOW_SCALE=1, RE15_FRAMEDUMP; Logs im scratchpad r41c/mess/*)
* Bruecke (C_belege/bruecke_vergleich.png, Cut 4 erzwungen, F300): frisch = Planke bei y -5400 ueber dem Becken (log
  "[ventilkette41] ROOM2030: Bruecke obj 0 oben y -5400, Zelle 12 gesperrt"); mit RE15_SET_FLAG=9:101,9:102 = Planke
  in der Original-Lage -1800 (im BSS ist dort nur das Becken gemalt -> die Fahrt ist sichtbar).
* Flut-Zustand (C_belege/flut_vergleich.png, Bits 101/102/103/100): Cut 4 = Wasserflaeche 2140 unter der Kamera
  (Differenz zum BSS 35710 px), Cut 13 (Tuerebene) = Flaeche 9000 tiefer (Differenz 1406 px unten rechts).
* Leiter (C_belege/leiter2_vergleich.png, Cut 9/10 erzwungen, RE15_PLAYER_POS -24500,10300): 20B0 (3,235)=1 ohne
  Bit 100 = Strahlen aus dem Gitterfenster (Original), mit Bit 100 = keine Strahlen (Differenz 3083 px, nur die
  Strahlen); 2000 Aktion an der Leiter = Text 40 im Bild (F340, Schreibmaschine "I cannot climb up the la").
* Pult 2050 (C_belege/pult2050_sheet.png, Eingabeskript A/W/R/L): F300 Aktion -> msg 43, F423 -> msg 41 "Will you push
  them?", F501 Ja -> Cut 15 + Cursor; RIGHT 7 Bilder -> Hebel 3, SQUARE F550 -> log "Hebel 3 -> EIN, Schalter 0x4,
  40 V" (Zeiger auf 40, gruene Marke); LEFT 36 Bilder -> Hebel 1, SQUARE F616 -> "Hebel 1 -> EIN, Schalter 0x5, 70 V".
  Lehre fuer die Eingabeskripte: ein Druck waehrend der Schreibmaschine spult nur vor; je Text ~3-4 s warten.
* Zaehler/Flut/Tod (Unit-Riegel, Takt fuer Takt, Spieler auf dem Ring y -5400): Warnung zu -> Marker -> Zaehler
  (Ost-Tuer Ziel 0x0B Cut 11, Wasser geparkt) -> Flut 1800 Takte nach dem Schliessen der Warnung (gemessen 1504 +
  600 Takte nach der Aktion, davon ~300 Takte Texte) -> Wasser 50/Takt -> Tod bei Wasser y -6300 (= -5400 - 900),
  125 Takte nach Flutbeginn -> Blende Kanal 0 (level 0x5000, ABR 2, Maske Rot+Gruen) -> Wasser endet bei -16200.
  D2 (Tuerebene y -16200): Tod mit der Endlage -16200 im Takt flut_bei + 323 (der erste der 324 Schritte laeuft im
  Marker-Takt: der For-Koerper laeuft bis zum ersten Evt_next, derselbe Takt sieht s_flut = 1).
* **Flut am Spiel** (Fortsetzung 22:00, exe-Kopie re15_pc_r41c_mess.exe, RE15_STATE_LOG, Bildnummern = Spielbilder ab
  dem Sprung; Reihe C_belege/flut_echt_sheet.png = F2600/2620/2640/2680/2720/2740/2760/2780/2820/2880/2940/2960):
  - Messmittel (kein Spielverhalten): die drei Spinnen des Eintritts-Cuts als Spielzustand "schon getoetet" ueber ihre
    Kill-Bits `RE15_SET_FLAG=7:155,7:156,7:157` = Sce_em_set pc[7] 0x9b/0x9c/0x9d (main00 @0x01C18/@0x01C2C/@0x01C40),
    EM-Status-Gate FUN_800420a0 @0x80042120-38 (`persist != 0xFF && flag(zone, persist)` -> kein Spawn), Zone 7 =
    STAGE2 (re15_em_status_zone, enemy_ai_common.c:14208); Zustandslog: 0 Gegner-Eintraege in 2030. Spieler per
    RE15_PLAYER_POS (6550,-100) Blick +z (3072; der Vorwaertspunkt 620 trifft keinen Platz), der Ventil-Platz per
    RE15_FIRE_AOT=14@330 (= derselbe Pfad wie die Aktionstaste, re15_aot_fire_slot -> scd_event_fire 26), Druecke alle
    4 s ab F450 (ein Druck nach dem Schliessen der Warnung trifft so keinen Platz mehr).
  - Vorbefund (21:52): der Debug-Sprung stellt den Spieler mit Blick +x = auf die Ost-Tuer; die Aktionstaste bei F330
    feuerte Tuer Slot 0 nach ROOM2070 (`[aot] DOOR FIRE slot=0 rect=(7350,-200,...)`), dort toeteten zwei Spinnen den
    stehenden Spieler (hp 100 -> -20 bei F605). Darum tragen integration A und der Flut-Lauf jetzt RE15_PLAYER_POS
    mit Blickrichtung.
  - Ablauf: F330 Ereignis 26 -> Frage 45 oeffnet F331 (Pause-Maske FFFF0007); Druecke F450/F573 spulen und blaettern,
    F696 = Ja -> 45 zu (F697), Warnung 46 auf (F699); F819 schliesst -> Marker (5,0x1D) -> `[ventilkette41] Zaehler
    laeuft` + Ost-Tuer -> 20B0 Cut 11 (F820) -> Sleep 1800 -> **F2621 Flut** (`FLUT: Wasser steigt`, Cut 0 -> 4,
    Pause-Maske 21000007 = Pad-Sperre) = 1800 Bilder nach dem Schliessen -> Wasser 50 je Takt ab y 0: Flaeche sichtbar
    unter der Bruecke (F2640 y -1000, F2680 -3000), Bruecke taucht unter (F2720 -5000), Fuesse (-5400) bei F2728,
    [Stand 22:00, alte Fassung: Spritzer alle 6 Takte ab F2733 (Zustandslog fx 1/2/3 bei F2733/2739/2745) — seit
    ddb65cfc EIN Spritzer bei F2728, s. "Spritzer nachgemessen"], **F2746 Tod** (`ERTRUNKEN: Wasser y
    -6300, Spieler y -5400 -> hp -1`, = 125 Bilder nach dem Flutbeginn, 126 Schritte x 50 = 6300 mit dem ersten im
    Marker-Takt), Blende (`config ch=0 abr=2 mask=6`, `kick ch=0 value=0x5000`), Todeskette (weisser Blitz F2820,
    pst 7 bei F2859, YOU DIED F2880..F2960), Ende F2960#2030. Ein spaeteres Ende (3500 ohne Raumfilter) lief in den
    Neustart nach dem Tod, dessen Bildzaehler die Framedumps 2400..3400 mit ROOM1170-Bildern ueberschrieb (erster
    Lauf, verworfen).
  - Blende (22:05): Bildmitte 100x60 F2740 (29,36,29) -> F2760 mit "ABR 0 + Maske Blau": (30,37,0) = Blau WEG
    (render_pc.c:1282 kennt nur ABR 1 additiv / 2 subtraktiv, 0/3 fallen auf subtraktiv zurueck -> "minus Blau");
    mit ABR 2 + Maske Rot+Gruen: (0,0,30) = Blau bleibt (C_belege/flut_blende_vergleich.png). Die Form ist auf der PSX
    dieselbe (GPU B-F je Kanal, Kachel-RGB = Helligkeit & Maske @0x800218d0).
    **Renderer-Einschraenkung (Nachbesserung 1, M4):** diese Farbe gilt fuer Renderer mit subtraktiver Blende
    (direct3d, opengl seit 81ee1efc, GLES2). SDLs Software-Renderer (`RE15_SOFTWARE_RENDER=1`, nur Mess-/Testbetrieb)
    kennt ABR 2 nicht und zeichnet die Kachel deckend (160,160,0) gelb (Abnahme 0 L3b); Riegel C misst die Farbe
    deshalb seit Nachbesserung 1 im beschleunigten Renderer (§0b M4, §5).
  - **Spritzer nachgemessen** (Vorgaenger 00:43-00:45, exe 00:41 = Stand ddb65cfc, gleicher Aufbau wie oben,
    Skript scratchpad r41c2/mess_flut.sh, Log r41c2/mess/flut_spritzer/debug.log + state.txt, von mir gelesen):
    `FLUT: Wasser steigt` (F2621) -> F2728 `[ventilkette41] SPRITZER: Wasser y -5400 erreicht die Fuesse (Spieler y
    -5400), Effekt 0x06050D00` GENAU EINMAL (Zustandslog fx=1 F2728..F2740, ab F2741 fx=0) -> `config ch=0 abr=2
    mask=6 step=0 bucket=7` + `kick ch=0 value=0x5000` -> F2746 `ERTRUNKEN: Wasser y -6300, Spieler y -5400 -> hp
    -1, blaue Blende` (Zustandslog hp 100 -> -1 bei F2746). Bildreihe **C_belege/flut_spritzer_sheet.png**.
* Echter Weg 2030 -> Ost-Tuer -> 20B0: integration_r41_ventilkette Lauf A (§5). Die Original-Szene in 20B0 (sub02 ->
  Zone Slot 7 -> sub05 -> Set(3,235)) laeuft im Port erst mit dem Haken der Spur A: §6 Querbezug A (Lauf kette_mit_a,
  Bild C_belege/querbezug_a_20b0_szene_sheet.png).
* **Eintritt in das geflutete 2030 nach der Flut** (NEU 03:03-03:05, exe 02:51, Kopie re15_pc_r41c3_mess.exe, Skript
  scratchpad r41c3/mess_ring.sh, Log r41c3/mess/ring_nach_flut/debug.log + state.txt): Zustand nach der Szene
  `RE15_SET_FLAG=3:235,9:103,9:100` (ohne 107), Sprung 20B0, `RE15_FIRE_AOT=3@200#20B0` = die Original-Ost-Tuer
  Slot 3 @0x019C2 (`[aot] DOOR FIRE slot=3 rect=(5950,-6400,hw=500,hh=1500) target_cut=0 spawn=(6550,-5400,-100)`)
  -> `[room] PC loaded room2030.rdt` -> `[ventilkette41] ROOM2030: unter Wasser (y -7200), Lift-Rufzonen 4/6 -> msg 1`
  + `Tuer Slot 0 -> ROOM20B0 Cut 11 (Ost-Tuer nach der Flut)`. Der Spieler steht auf dem Ring (y -5400), die
  Wasserflaeche 1800 ueber seinen Fuessen, und laeuft (Eingabe U2 ab F330: x 6782 -> 6368, mo 105); kein Ertrinken
  (der Ertrink-Takt laeuft nur waehrend der Flut, s_flut), eine Spinne greift im Wasser an (hp 100 -> 77 bis F460).
  Bild **C_belege/ring_nach_flut_sheet.png** (F60..F480, Cut 0: die Flaeche als Band in Brusthoehe vor Leon).
  Dieselbe Landung haben 2070 Slots 1/2 @0x00A5A/@0x00A7A (unbedingt in main00, Ziel ROOM2030 Cut 0, spawn
  (6550,-5400,-100) = dieselben Bytes wie 20B0 @0x019C2 ab pc[14]; Tuersaetze selbst dekodiert). Nicht gemessen: ob
  2070 nach der Flut erreichbar ist (2070 Slot 4 -> ROOM11A0, Slot 5 -> 2040; Nutzer: 2040 braucht den Aufzug mit
  Strom). Der Weg zum Rechner 2080 braucht den Ring NICHT (aus den Tuersaetzen, nicht gefahren): 2050 Slot 1 @0x01C6A
  -> 2020, 2020 Slot 1 (@0x0088A, bei (3,219) = 0 erst der Ereignis-Platz Slot 4 @0x00852 + Tuer sce 0 @0x00866) ->
  2030 Tuerebene (spawn y -16200, ueber dem Wasser) -> 2030 Slot 1 @0x01AEA -> 2080. -> §6 OFFEN, §7 Frage (f).
* **Punkt 2 am Spiel** (Vorgaenger 23:47-23:49, exe-Kopie re15_pc_r41c_mess.exe, Skript scratchpad r41c/mess_punkte.sh,
  Spinnen per Kill-Bits wie im Flut-Lauf; die Kamera bleibt beim Sprung-Cut 0, weil der Spieler nicht laeuft;
  Logs scratchpad r41c/mess/{v13,pult_bruecke}/debug.log, von mir gelesen):
  - Ventil 13 (`RE15_PLAYER_POS=-1500,-6400,2048,1`, Log `pl pos=(-1500,-1800,-6418) rot=2048` = Landeplatz Band 1
    am Tank): F330 Aktion -> `[ventilkette41] ROOM2030 Ereignis 25 -> Port-Programm 2030_v13_frage` ->
    `[msgtext] room=2030 id=41 "Intake valve 13. Will you open it?"` (Bild **C_belege/ventil13_frage.png** = F360,
    Schreibmaschine "Intake valve 13. / Wi"); F453 Ja -> `[msgtext] room=2030 id=42 "You've opened valve 13. The
    pressure is rising."` (Bit 101); F699 Aktion -> `Port-Programm 2030_v13_zustand` -> id 47 "Intake valve 13 is open."
    (die Programmwahl selbst belegt Bit 101 = 1, ventilkette_r41.c re15_ventilkette_r41_ereignis).
  - Pult mit offenem Ventil 13 (`RE15_SET_FLAG=...,9:101`, `RE15_PLAYER_POS=350,-9500,1024,3`, Log `pl pos=(350,-5400,
    -9500)` = Ring Band 3 vor der Nordwand): F330 Aktion -> `Ereignis 24 -> Port-Programm 2030_pult_senken` ->
    id 43 "The pressure gauge reads 80. Lower the bridge?"; F453 Ja -> `[scd F456] Cut_chg(4)` + `[szenenschutz] F456 an`
    -> `[prop-member F516] prop[0] obj=0x00 m=1 → val=-1800` (Endlage Member_set(1,-1800) nach 30 Schritten je 120
    jeden zweiten Takt: 456 + 60 = 516) -> Kamera zurueck (Cut_old, cut 0) -> `[scd F550] letterbox closed -> gameplay`;
    F576 Aktion -> `2030_pult_unten` -> id 44 "The bridge is down.". Bilderreihe **C_belege/bruecke_fahrt_sheet.png**:
    F345 Frage 43 (Schreibmaschine), F450 Frage komplett mit Yes/No, F465 Cut 4 (Draufsicht, Fahrt laeuft), F480/F495/
    F510 die Planke sinkt ueber dem Becken, F525 unten (Steg-Ebene), F600 zurueck an der Nordwand mit Text 44.
* **Punkt 6 am Spiel** (Vorgaenger 23:49-23:51, Bits 103 + 100 vorgesetzt, Log scratchpad r41c/mess/rechner2080b/):
  erster Versuch `RE15_PLAYER_POS=-11550,-6000,1024,8` verworfen (die Kollision schob den Spieler nach (-11185,-7206),
  Blick 1024 = -z, der Vorwaertspunkt traf das Rechner-Rechteck (-12050,-7250,1000,1000) nicht -> kein Ereignis);
  zweiter Lauf `(-11185,-7206)` Blick 3072 (+z, Vorwaertspunkt z -6586 im Rechteck), Log `[parity] JUMP-Spawn ersetzt
  durch RE15_PLAYER_POS (-11185,-7206) rot=3072 band=8`: F330 Aktion -> `ROOM2080 Ereignis 2 -> Port-Programm
  2080_rechner_erw` -> `[msgtext] room=2080 id=0 "From here maybe I can control the water level.Will you operate the
  computer?"` (Original-Text) -> F453 spult, F576 Ja -> `[scd F599] Cut_chg(7)` -> Wasser obj 0 sinkt (Original For 148
  @0x00CCC, 2 Evt_next je Schritt: 599 + 296 + Sleep 20 = 915) -> `[scd F915] Cut_chg(3)` -> `[msgtext] room=2080 id=40 "The treatment pool is being drained as well."`
  (Bit 107 steht im selben Ja-Zweig `22 09 6b 01`; das Zustandslog fuehrt Bank 9 nicht, das Bit misst Riegel G).
  Bilderreihe **C_belege/rechner2080_sheet.png**: F360 Frage (Sprung-Kamera), F600 Cut 7 Wasser sinkt, F750, F900
  (Becken leer bis auf den Grund), F930 Cut 3 Text 40 (Schreibmaschine), F1200 Text 40 komplett.
* **Luecken am Spiel (dritte Fortsetzung, 2026-10-24 00:26-00:29, scratchpad r41c2/mess/<lauf>/debug.log)** — was bis
  dahin nur der Unit-Riegel zeigte, jetzt mit der exe (Eingabeskript = Aktionstaste/Steuerkreuz, Bildnummern = Spielbilder):
  - **P5 Pult -> Hebel, ein Lauf** (`p5a_pult_loesung`, `RE15_PLAYER_POS=4900,-7100,1024` vor dem Pult wie pult2050c):
    F330 Aktion -> `ROOM2050 Ereignis 9 -> Port-Programm 2050_pult_ein` -> id 43 "A high voltage switchboard. The pump
    needs 80 volts." -> F453 -> id 41 "Will you push them?" -> F531 Ja -> `[scd F534] Cut_chg(15)`; F564 RIGHT (8 Bilder),
    F581 SQUARE -> `Pult 2050: Hebel 3 -> EIN, Schalter 0x4, 40 V`; F605 LEFT (36 Bilder), F650 SQUARE -> `Hebel 1 -> EIN,
    Schalter 0x5, 70 V`; F674 RIGHT (60 Bilder = 12000 bei 200 je Bild `2f 00 c8 00` sub07), F743 SQUARE -> `Hebel 4 ->
    EIN, Schalter 0xD, 80 V` -> nach den 30 Ruhebildern (RE2 @0x0171C) `Pult 2050 GELOEST: 80 V -> Bit 106, "Power supply
    OK."` + id 42 (= 11F0 msg 2) -> F836 zu -> `Pult 2050 verlassen (Schalter 0xD, 80 V)` (sub10 @0x0240C). Danach der
    Hebel-Platz Slot 6 (Ereignis 4) ueber `RE15_FIRE_AOT=6@900` (= re15_aot_fire_slot, derselbe Pfad wie die Aktionstaste;
    das Pult steht an der Saeule, der Hebel in der Nische dahinter — ein offenes Skript um die Wand herum waere geraten)
    -> KEIN Port-Programm mehr, das Original sub04: id 0 "A lever to fill and drain water. Will you move it?" -> F1022 Ja
    -> `[prop-member F1026] prop[5] obj=0x04 m=1 → val=-1400` (@0x0230C ff.) -> `[scd F1046] Cut_chg(4)` (Sleep 20) ->
    For 180 x Speed -20 (@0x02324/@0x02328) -> `[prop-member F1226] prop[6] obj=0x05 m=1 → val=-5000` + `Cut_chg(7)` ->
    `[scd F1240] letterbox closed -> gameplay`. Bildvergleich Cut 4: F1050..F1155 0 Pixel Unterschied, F1200 736, F1215
    1283 Pixel (Rahmen x 52..233, y 188..215 = die Wasserflaeche taucht unter dem Gitter ins Bild).
  - **P5 Hebel vorher, Aktionstaste** (`p5b_hebel_gesperrt`, `RE15_PLAYER_POS=4300,-11150,0,3` = Nische westlich des
    Hebels, Blick +x, Vorwaertspunkt x 4920 im Rechteck (4700,-11550,800,800) @0x01E28): F330 Aktion -> `ROOM2050
    Ereignis 4 -> Port-Programm 2050_hebel_sperre` -> id 40 "The pump has no power. The switchboard must be set first.";
    kein Cut_chg, kein Member_set. Bilderreihe **C_belege/pult2050_loesung_sheet.png** (F420 Hebel vorher, F540 Cut 15,
    F585 40 V, F660 70 V, F750 80 V mit drei gruenen Marken, F795 "Power supply OK.", F990 Original-Frage am Hebel,
    F1155 Cut 4, F1215 Wasser im Bild).
  - **P2 Pult ohne Ventil 13** (`p2a_pult_ohne`, Ring vor der Nordwand wie pult_bruecke, ohne Bit 101): F330 Aktion ->
    `Ereignis 24 -> Port-Programm 2030_pult_ohne` -> `[msgtext] room=2030 id=40 "A control panel for the bridge. The
    pressure gauge reads zero.Intake valve 13 is closed."` (zwei Seiten, F453 blaettert); kein Cut_chg(4), keine
    Fahrt. Bild **C_belege/pult2030_ohne_ventil.png** (F440 Seite 1, F560 Seite 2).
  - **P4 Leiter 20B0 -> 2050** (`p4a_leiter20b0`, Zustand nach der Szene: `RE15_SET_FLAG=3:235,9:103,9:100`, Spieler
    vor der Leiter wie integration B): F330 Aktion -> `[aot] DOOR FIRE slot=4 rect=(-27100,10300,hw=500,hh=500)
    target_cut=0 spawn=(9100,-5400,4100)` -> `[room] PC loaded room2050.rdt` (kein Sperrtext) -> in 2050 sofort
    `[ventilkette41] ROOM2050: Tuer Slot 0 -> ROOM20B0 Cut 9 (Ziel ROOM2000)`.
  - **P4 2050-Leiter hinab -> 20B0** (`p4b_leiter2050`, gleicher Zustand, `RE15_PLAYER_POS=9100,4300,3072` wie
    integration_r36_dachkiste echt2050): F330 Aktion -> `[aot] DOOR FIRE slot=0 rect=(9100,5700,hw=1000,hh=1000)
    target_cut=9 spawn=(-26450,-1800,10250)` -> `[room] PC loaded room20b0.rdt` (Original-Ziel ROOM2000 Cut 10 @0x01C4A,
    Zwilling Cut 9 laut §1.2).
  - **P6 2030 nach dem Rechner** (`p6b_leer_cut4`, Cut 4 erzwungen, Bits 101/102/103/100 + 107): kein `unter Wasser`-
    Satz im Log (nur `Tuer Slot 0 -> ROOM20B0 Cut 11 (Ost-Tuer nach der Flut)`); das Bild ist **bitgleich** (0 Pixel
    > 24) zum Stand ohne Flut (r41c/mess/bruecke_unten, nur 101+102) und unterscheidet sich vom Flut-Stand ohne 107
    (r41c/mess/flut_cut4) in 39271 Pixeln. Bild **C_belege/raum2030_leer_vergleich.png**.

## 5. Tests
* `unit_r41_ventilkette` (tests/unit/test_r41_ventilkette.c, registriert in probes/r41_ventilkette.cmake): A Scheiben
  Byte fuer Byte gegen 9 RDTs (Leon+Elza) + 2080-Programm = sub02 + Schwanz + Texte dekodiert + Gewichte (1 von 16
  Masken) + Cut-Zuordnung; B Leiter (Text, Aktion, Bit 100 = Tuer, 20B0 ohne Bit 100 = 18 Strahlen-Plaetze); C Bruecke
  oben/Zelle 12 zu, Plaetze 12/13/14, Pult msg 40, Ventil 13 Nein/Ja/Zustand + Bit 101, Pult Nein/Ja (Fahrt unter
  Cut 4, y -1800, Bit 102, Zelle offen, Rueckkehr, 60..200 Takte), Neustart mit 102; D Ventile Nein/Ja, Zaehler,
  Ost-Tuer, Wasser geparkt, Flut nach 1800 Takten, 50/Takt, Pad-Sperre, Cut 4, Tod bei 900 ueber den Fuessen, Blende,
  Wasser bis -16200, Neustart ohne Blende; E Verlassen -> Bits 103+100, 20B0 Leiter = Tuer + 0 Strahlen, Tueren
  2010/2050/2070/1260 -> 20B0 mit Cut, 2030 unter Wasser + Rufzone msg 1 + Zustandssatz, mit 107 leer; F 2050 Hebel
  ohne Strom, Pult Nein/Ja, Cursorfahrt sub07, Zonen, 30+40+10 = 80, Zeiger x(80)=238/x(0)=166, Marken, Bit 106 nach
  <= 32 Ruhebildern, Ausstieg sub10, Cursor geparkt, Hebel = Original (3,104)=1, Zustandssatz, CROSS = Abbruch;
  G 2080 Original / mit Flut (msg 0 + 40, Bit 107, Wasser -3400) / Rechner schon gelaufen / mit 107.
  D2 Tuerebene (Tod mit der Endlage -16200, flut_bei + 323). Ergebnis bis 00:41: PASS (233 Pruefungen), Leon + Elza
  (Fortsetzung: Blenden-Riegel = abr 2, Masken r/g 0xff, b 0, Helligkeit 160). Seit ddb65cfc zusaetzlich D: genau EIN
  Spritzer bei Wasser y -5400 (= Fuesse) vor dem Tod, D2: einer im Todestakt auf der Tuerebene.
  **Lauf 2026-10-24 02:56 (exe/Test-exe 02:51 = Stand 271b05ea): `PASS (239 Pruefungen)`.**
* `integration_r41_ventilkette` (tests/integration/test_r41_ventilkette.cmake): A echter Weg 2030 Ventile (Aktionstaste,
  Blick 2048) -> Frage -> Ja (4. Druck) -> Warnung -> zu (F777) -> Zaehler -> Ost-Tuer feuert (RE15_FIRE_AOT=0@900) ->
  ROOM20B0 geladen -> Bits 103 + 100, kein 2070; B 2000 Leiter (-26000) = Sperrtext `[msgtext] room=2000 id=40`, kein
  Raumwechsel nach 2050; C Flut am Spiel (Kill-Bits, FIRE_AOT 14, Zustandslog): Zaehler 1800 +- 3 Bilder, Flut unter
  Cut 4, Tod 125 +- 2 Bilder danach bei Wasser -6300 / Spieler -5400, Blende kick 0x5000, Framedump F2750, Ende
  F2960#2030. **`ctest --timeout 900 -R "^integration_r41_ventilkette$"` -> `100% tests passed, 0 tests failed out
  of 1` (213.97 s)** — davor zwei rote Laeufe: A/B mit den alten Takten (Tuer statt Ventile; Leiter nicht getroffen).
  Seit 271b05ea prueft C zusaetzlich `SPRITZER: Wasser y -5400 erreicht die Fuesse (Spieler y -5400)` VOR `ERTRUNKEN:`
  und genau EIN `SPRITZER:`-Satz (Ereignis, keine Rate). A prueft den Szenenverlauf in 20B0 NICHT (der haengt ohne den
  Haken der Spur A, §6) — der Riegel bleibt also mit und ohne diesen Haken gueltig.
* `integration_r41_ventilkette_b` (tests/integration/test_r41_ventilkette_b.cmake, Kopf-Praefix re15_pc_r41cb_): die
  Glieder, die A nicht faehrt — D Punkt 5 in einem Lauf (Pult 2050 -> Text 43 -> "Will you push them?" -> Cut 15 ->
  Cursor RIGHT 8 / LEFT 36 / RIGHT 60 -> 40/70/80 V -> 30 Ruhebilder -> Bit 106 + "Power supply OK." -> sub10 -> Hebel
  Slot 6 per FIRE_AOT = Original sub04 -> obj 4 y -1400 -> Cut 4 -> obj 5 y -5000); E Hebel vorher = 2050_hebel_sperre
  (Text 40, kein Cut 4, kein Member_set); F Bruecken-Pult ohne Ventil 13 = 2030_pult_ohne (Text 40, keine Fahrt);
  G 20B0 nach der Szene mit 103 + 100: Leiter Slot 4 = Tuer nach 2050, dort Slot 0 -> 20B0 Cut 9; H 2050-Leiter hinab
  -> 20B0 Cut 9; I 2030 mit 103 ohne 107 = Wasser y -7200 (Gegenprobe), mit 107 kein Wasser; J Elza 2051 = D bis
  "Power supply OK." + Original-Hebel; K Elza 2001 Leiter = Sperrtext 40 woertlich. Erste Fahrt (exe 22:17, 00:36-00:41,
  Protokoll scratchpad r41c2/ctest_b2.txt): `100% tests passed, 0 tests failed out of 1` (291.85 s).
* Beide r41-Integrationsriegel auf der exe 00:41 (= Quellstand ddb65cfc/271b05ea; 00:46:38-00:55:43, Protokoll
  scratchpad r41c2/ctest_integ_r41.txt, `exit=0`):
  `ctest --test-dir re15_port/build -R "^integration_r41_ventilkette(_b)?$" --timeout 1200`
  -> **`100% tests passed, 0 tests failed out of 2`** (`Total Test time (real) = 545.09 sec`; A/B/C 216.23 s,
  Teil B 328.81 s). Die exe 02:51 ist aus derselben Quelle gebaut (nur der temporaere Spur-A-Haken war dazwischen
  drin und wieder heraus) -> nicht wiederholt.
* Gezielte Gruppe (ctest, Regex woertlich):
  `ctest -j 6 --timeout 600 -R "^(unit_r41_ventilkette|unit_r36_kartenbits|unit_r37_fahrstuhl_lift2030|unit_r36_gegnerzahl_raum20b0|unit_r38_npckante_j2000|unit_r40_nahaufnahmen|unit_r40_untersuchen1120|unit_r34n_a_rolltor|unit_r36_kartenitems|unit_r38_wasser_.*|unit_r38_wasserzombie)$"`
  -> `100% tests passed, 0 tests failed out of 18` (18:35, vor dem Riegel D2).
  Fortsetzung, erweitert um die Gruppen der beruehrten Dateien (scd_room_setup.c, scd_vm.c, main.c, kartenbits):
  `ctest -j 6 --timeout 600 -R "^(unit_r41_ventilkette|unit_r36_kartenbits|unit_scd_.*|unit_aot_.*|unit_r40_nahaufnahmen|unit_r40_untersuchen1120|unit_r37_kistenregel_.*|unit_r36_dachkiste_.*|unit_r37_kiste.*|unit_r37_itemband|unit_r37_fahrstuhl_lift2030|unit_r36_gegnerzahl_raum20b0|unit_r38_npckante_j2000|unit_r34n_a_rolltor|unit_r36_kartenitems|unit_r38_wasser_.*|unit_r38_wasserzombie|unit_r38_spinnen)$"`
  -> 21:50 `97% tests passed, 1 tests failed out of 37` (unit_r41_ventilkette D2, Erwartung 324 statt 323) ->
  nach dem Riegel-Fix und dem Blenden-Fix (Lauf 23:46:55 auf der exe/den Test-exe vom 22:17, Protokoll scratchpad
  r41c/ctest_unit_r41c_final.txt): **`100% tests passed, 0 tests failed out of 37`** (`Total Test time (real) =
  6.24 sec`).
  **Abschluss-Lauf 2026-10-24 02:56** (gleicher Regex, Bau 02:51 = Stand 271b05ea ohne den temporaeren Haken,
  `--output-on-failure`, Protokoll scratchpad r41c3/ctest_unit.txt, `EXIT=0`):
  `ctest --test-dir re15_port/build -j 6 --timeout 600 --output-on-failure -R "^(unit_r41_ventilkette|unit_r36_kartenbits|unit_scd_.*|unit_aot_.*|unit_r40_nahaufnahmen|unit_r40_untersuchen1120|unit_r37_kistenregel_.*|unit_r36_dachkiste_.*|unit_r37_kiste.*|unit_r37_itemband|unit_r37_fahrstuhl_lift2030|unit_r36_gegnerzahl_raum20b0|unit_r38_npckante_j2000|unit_r34n_a_rolltor|unit_r36_kartenitems|unit_r38_wasser_.*|unit_r38_wasserzombie|unit_r38_spinnen)$"`
  -> **`100% tests passed, 0 tests failed out of 37`** (`Total Test time (real) =   8.32 sec`).
* Integrationsgruppe der beruehrten Raeume (seriell, exe): Regex
  `^(integration_r41_ventilkette|integration_r37_fahrstuhl_n1|integration_r37_itemband|integration_r37_itemband_lift|integration_r37_kiste|integration_r37_kiste2feld|integration_r37_kistenregel|integration_r36_gegnerzahl|integration_r36_dachkiste|integration_r38_spinnen_echtlauf|integration_r38_wasser|integration_r38_wasserzombie|integration_map_uebergang|integration_cross_stage|integration_item_name_census|integration_save_counter_pin)$`
  -> 19:03 (Vorgaenger, alte exe, abgebrochen vom Sitzungslimit nach 4 von 16): dachkiste rot (echt2050 Leiter — der
  Querbezug unten), gegnerzahl / fahrstuhl_n1 / itemband gruen (103 / 155 / 829 s).
  Fortsetzung (exe vom 22:17 = Stand aller Commits, `--timeout 1800`, weil dachkiste allein >1100 s braucht; Lauf
  ~22:24:39-23:46:27, Protokoll scratchpad r41c/ctest_integ_gruppe.txt, `exit=0`):
  **`100% tests passed, 0 tests failed out of 16`** (`Total Test time (real) = 4908.47 sec`; dachkiste 1425.21 s,
  itemband_lift 1379.32 s, itemband 824.22 s, kistenregel 284.60 s, r41_ventilkette 214.59 s, wasser 179.11 s,
  fahrstuhl_n1 132.01 s, wasserzombie 122.24 s, spinnen_echtlauf 115.02 s, gegnerzahl 90.71 s, kiste 63.03 s,
  kiste2feld 57.60 s, save_counter_pin 19.68 s, map_uebergang 0.69 s, item_name_census 0.31 s, cross_stage 0.02 s).
  Kein roter Fenster-Haken -> nichts einzeln nachzufahren. Seit diesem Lauf hat sich im Engine-Code nur flut_tick
  (Spritzer, ddb65cfc) geaendert; der Takt laeuft nur waehrend der Flut in 2030 (s_flut), die keine der 15 fremden
  Gruppen faehrt — die r41-Riegel selbst liefen auf der neuen exe (2/2 oben).

* Querbezuege (Memory "neue Regeln widerlegen alte Riegel", beim ersten Integrationslauf gefunden):
  - `integration_r36_dachkiste` echt2050/elza2051 fahren 2050 -> 2000 -> Leiter slot 4 @0x0169E -> 2050; die Leiter ist
    jetzt bis zur Flut gesperrt -> die zwei Laeufe setzen `RE15_SET_FLAG=9:100` (Leiter frei; Gegenstand des Riegels
    ist der Kisten-Record, nicht die Leiter). Lauf 1 der Gruppe fiel genau an "DOOR FIRE slot=4 rect=(-27100,10300"
    durch (1115 s unter Last).
  - `integration_r37_itemband` schrot2030 geht ueber den Steg (GEHE -1050 -7700 -> -1050 7100 = Zelle 12) zu den
    Schrotpatronen; der Steg ist bis zum Pult-Raetsel gesperrt -> Lauf setzt `RE15_SET_FLAG=9:101,9:102`.
  Beide Aenderungen sind Kommentar-belegt in den cmake-Dateien.

* **Nachbesserung 1 (2026-10-24; exe/Test-exe 05:45 = Engine-Stand def5ff47, danach nur Riegel/Dossier; ctest aus Git-Bash,
  Protokolle scratchpad `r41c_nb1/`):**
  - Unit-Gruppe (37 des Vorgaengers + Heliport 1171 + Kraehen wegen M3), Regex woertlich
    (`r41c_nb1/regex_unit.txt`):
    `ctest --test-dir re15_port/build -j 6 --timeout 600 --output-on-failure -R "^(unit_r41_ventilkette|unit_r36_kartenbits|unit_scd_.*|unit_aot_.*|unit_r40_nahaufnahmen|unit_r40_untersuchen1120|unit_r37_kistenregel_.*|unit_r36_dachkiste_.*|unit_r37_kiste.*|unit_r37_itemband|unit_r37_fahrstuhl_lift2030|unit_r36_gegnerzahl_raum20b0|unit_r38_npckante_j2000|unit_r34n_a_rolltor|unit_r36_kartenitems|unit_r38_wasser_.*|unit_r38_wasserzombie|unit_r38_spinnen|unit_r38_heliport1171_.*|unit_r39_heliport1171_.*|unit_crow_.*|unit_re2_crow_.*|probe_crow_kill|unit_r35_fenster_kraehe|unit_r36_kleidung_kraehe|unit_r36_schuss_kraehe)$"`
    -> **`100% tests passed, 0 tests failed out of 67`** (`Total Test time (real) =   7.83 sec`, EXIT=0);
    unit_r41_ventilkette `PASS (255 Pruefungen)` (vorher 239; neu: Scheibe @0x0201A, Text 49, Fall F Kisten,
    Fall E Text 49, Fall H Bank 5). Gegenprobe am alten Modul: `FAIL (255 Pruefungen)` mit genau den 6 neuen Pruefungen
    (M1 x2, M3 x2, M8 x2), Protokoll `r41c_nb1/unit_gegen.txt`.
  - **Endstand (nach C-E2, exe/Test-exe 06:37 = Stand 74fb9fae + Riegel-Korrektur):** Unit-Gruppe (Regex wie unten,
    `r41c_nb1/ctest_unit2.txt`) -> **`100% tests passed, 0 tests failed out of 67`** (`Total Test time (real) =   9.12 sec`),
    unit_r41_ventilkette `PASS (274 Pruefungen)`;
    `ctest --test-dir re15_port/build -R "^integration_r41_ventilkette(_b)?$" --timeout 1200 --output-on-failure` ->
    `integration_r41_ventilkette ...... Passed 230.63 sec`, `integration_r41_ventilkette_b ....***Failed 422.49 sec`
    (Lauf L: der Riegel suchte `[msgtext] room=20B0`, das Log schreibt die Raum-Id klein `room=20b0`; der Text stand
    da) -> Riegelzeile korrigiert -> `ctest ... -R "^integration_r41_ventilkette_b$"` -> `1/1 Test #1127:
    integration_r41_ventilkette_b ....   Passed  512.29 sec` / **`100% tests passed, 0 tests failed out of 1`**
    (EXIT=0, `r41c_nb1/ctest_integ_b3.txt`).
  - Erste Runde der Nachbesserung (vor C-E2, exe 05:45 = def5ff47) — r41-Integrationsriegel (C jetzt im beschleunigten
    Renderer mit Farbmessung, _b mit E2/E3/E4):
    `ctest --test-dir re15_port/build -R "^integration_r41_ventilkette(_b)?$" --timeout 1200 --output-on-failure`
    -> `1/2 Test #1126: integration_r41_ventilkette ......   Passed  213.05 sec`,
    `2/2 Test #1127: integration_r41_ventilkette_b ....   Passed  444.40 sec`,
    **`100% tests passed, 0 tests failed out of 2`** (`Total Test time (real) = 657.49 sec`, EXIT=0). Laufordner
    `build/tests/integration/r41_ventilkette/lauf_6b7e0e00_*`, `.../r41_ventilkette_b/lauf_5c01b5fb_*`: E2
    `[ventilkette41] ROOM2050 Ereignis 2 -> Port-Programm 2050_hebel_sperre` + `[msgtext] room=2050 id=40 ...`, 0x Cut_chg(4);
    E3 `[msg] room=2050 id=0`, `[prop-member F456] prop[2] obj=0x01 m=0 → val=-17700`, Cut_chg(4),
    `[prop-member F657] prop[6] obj=0x05 m=1 → val=-5000`; E4 `loaded room2051.rdt` + `ROOM2051 Ereignis 2 -> ...
    2050_hebel_sperre` + `[msgtext] room=2051 id=40 ...`.
  - Ketten-Riegel (M2, Entscheid C-E1): im C-Baum nicht registriert, Configure-Zeile `-- integration_r41_ventilkette_kette
    uebersprungen: Spur A fehlt (...)`; mit dem wortgleichen Haken auf dem Endstand (`cmake -P`) `rc=0` + Abschlusszeile
    (§0b M2). (Vorlaeufer `_szene` mit Laufzeit-SKIP: `***Skipped 61.97 sec`, ersetzt.)
  - Querbezug M3 (Heliport 1171, Bank-5-Bits der Kraehenszene):
    `ctest --test-dir re15_port/build -R "^integration_r3[89]_heliport1171_.*$" --timeout 900 --output-on-failure`
    -> `integration_r38_heliport1171_weg_re2 Passed 74.47 sec`, `_weg_re15 Passed 69.53 sec`, `_laden Passed 46.40 sec`,
    `integration_r39_heliport1171_weg Passed 64.24 sec`, `_laden Passed 46.18 sec` -> **`100% tests passed, 0 tests failed
    out of 5`** (`Total Test time (real) = 300.91 sec`, EXIT=0). Damit ist der Original-Zustand der Kraehenszene
    ((5,0x1E) bleibt im Init-Takt stehen) mit den Heliport-Riegeln vertraeglich (sie liefen auch mit dem Loeschen gruen,
    Abnahme 0 — das Loeschen war fuer sie unsichtbar, s. M3).

* **Nachbesserung 2 (2026-10-24; Spiel-Code unveraendert, exe 06:37 = 5ddd98c0; neu gebaut `local_build.sh build` ->
  `[2/3] Linking C executable tests\unit\test_r41_ventilkette.exe` / `=== LOCAL-BUILD-OK (build)`; Protokolle scratchpad
  `r41c_nb2/`):**
  - `unit_r41_ventilkette` direkt: `J1 Zensus: 206 Raeume, 16 Funde (Tueren nach 2080/2090/20A0 + Set(3,206)=1)`,
    `ROOM11A0 11A0 zu msgs: 2@Cut0`, `ROOM11A1 11A0 zu msgs: 2@Cut0`, **`PASS (286 Pruefungen)`** (vorher 274; neu: J1
    Zensus + Bytes 11A0/11A1, J2 Leon + Elza je Sperre/Aktion/Gegenprobe (3,206) -> Tuer + Raumwechsel nach 20A0).
  - Unit-Gruppe der Nachbesserung 1 (67 Tests, Regex woertlich wie oben unter "Nachbesserung 1"):
    **`100% tests passed, 0 tests failed out of 67`** (`Total Test time (real) =  11.61 sec`, `r41c_nb2/ctest_unit67.txt`).
  - Unit-Gruppe der Abnahme 1 (164 Tests, Regex `C41abn1/regex_unit.txt` = `r41c_nb2/regex_unit164.txt`):
    **`100% tests passed, 0 tests failed out of 164`** (`Total Test time (real) =  18.67 sec`, `r41c_nb2/ctest_unit164.txt`).
  - Neuer Riegel: `ctest --test-dir re15_port/build -R "^integration_r41_ventilkette_weg$" --timeout 1800
    --output-on-failure` -> `1/1 Test #1128: integration_r41_ventilkette_weg ...   Passed  439.47 sec`,
    **`100% tests passed, 0 tests failed out of 1`** (`Total Test time (real) = 439.52 sec`, EXIT=0, `r41c_nb2/ctest_weg2.txt`;
    Laufordner `build/tests/integration/r41_ventilkette_weg/lauf_48e0a9dc_{a..f}`): a `[msgtext] room=11a0 id=2 "It's locked
    from the other side."`, b `Spieler-Familie PL04` ... `[msgtext] room=11a1 id=2 ...`, beide `FLAG (3,206) = 0`, kein
    `loaded room20a`; c `DOOR FIRE slot=4 rect=(29000,1300,...)` -> `loaded room20a0.rdt`; d `ZELLE r0[51] ... u0 0x00` ->
    `loaded room2020.rdt`; e `unter Wasser (y -7200)` -> `DOOR FIRE slot=3 ...` -> `loaded room2060.rdt`; f
    `2080_rechner_erw` -> `id=40` -> `FLAG (9,107) = 1`. Der erste Lauf desselben Riegels (`lauf_baaaad3f_a`, ohne die
    Spinnen-Kill-Bits des 2070-Nordgangs) endete mit Leons Tod im Nordgang und wurde von mir abgebrochen (kein
    Ergebnis gewertet); danach Kill-Bits (7,98)/(7,99) in a/b (§0c "Gegner auf dem Weg").
  - Nicht erneut gefahren (Spiel-Code und exe unveraendert seit 06:37): integration_r41_ventilkette / _b (Abnahme 1 §3:
    2/2 Passed auf genau dieser exe), Heliport 5/5, Querbezuege dachkiste/itemband/kistenregel (Abnahme 0/1).

* **Nachbesserung 3 (2026-10-24; Bau `local_build.sh configure` + `build` -> `=== LOCAL-BUILD-OK`, exe 11:03 = Quellstand
  97fb1d18 ff., seitdem nur Dossier):**
  - `test_r41_ventilkette.exe` direkt: `PASS (376 Pruefungen)` (Leon + Elza; neu K Ventilraetsel erst mit Bruecke und
    Sicherung + Sicherung ueber Original sub06, L Zensus (3,126)/(3,108) + SCA-Belege, E Strom -> Relinghoehe / Treppe 48,
    G 2080 Original in drei Zustaenden, I Ring-Tueren mit Strom offen, A Netz der Relinghoehe + Texte 48/49).
  - Unit-Gruppe (Regex der Abnahme 2 woertlich, scratchpad `r41c_nb3/regex_unit.txt`, `ctest --test-dir re15_port/build -R
    "<regex>" --timeout 600 --output-on-failure -j 4`): **`100% tests passed, 0 tests failed out of 164`** (34.55 s).
  - Integration (`ctest --test-dir re15_port/build -R "^integration_r41_ventilkette(_b|_weg|_weg_elza|_sperre)?$"
    --timeout 2700 --output-on-failure`, 11:05-11:53, unter Fremdlast anderer Spuren):
    `1/5 Test #1126: integration_r41_ventilkette ............   Passed  257.45 sec`,
    `2/5 Test #1127: integration_r41_ventilkette_b ..........   Passed  505.60 sec`,
    `3/5 Test #1128: integration_r41_ventilkette_weg ........   Passed  897.27 sec`,
    `4/5 Test #1129: integration_r41_ventilkette_weg_elza ...   Passed  908.15 sec`,
    `5/5 Test #1130: integration_r41_ventilkette_sperre .....   Passed  344.98 sec`,
    **`100% tests passed, 0 tests failed out of 5`** (`Total Test time (real) = 2913.54 sec`).
  - `integration_r41_ventilkette_kette` weiter nur mit Spur A registriert (Configure: "uebersprungen: Spur A fehlt", C-E1).

## 6. OFFEN
* **Nachbesserung 3 (2026-10-24, Abnahme 2 N1-N4, Nachtraege 7-10):**
  - Lesart "Relinghoehe": umgesetzt ist y -3600 = Oberkante der UNTEREN Reling (Bildmessung +1800 ueber dem Landeplatz,
    Band 2 der Ring-Bodenzellen, ROOM2050 -3600). Moegliche andere Lesart "Wasser bis an die Ring-Reling" waere y -5400
    (Band der Ring-Reling-Zellen fl 0x33; Vorschau scratchpad `r41c_nb3/poly_bc.png` obere Reihe). Entscheidungsfrage §7 (k).
  - 2060: der Weg Tuer -> Sicherungskasten/Generator ist als Gang ueber x -4500 / x 6000 nur in EINER Richtung als ein Lauf
    gegangen (w3: Generator -> Tuer); die zwei Raetsel laufen ueber RE15_INPUT_SCRIPT ab der Stelle (g1/g2), weil der
    Wegplan-Treiber kein Steuerkreuz fuer Cursor kennt.
  - Die 20B0-Szene (Ankunft ueber die Ost-Tuer) braucht weiter den AUTO-Haken der Spur A (C-E1); w1 startet am Ende der
    Szene ((3,235)=1, Lage (-20620,-9705)).
  - Port-Texte 48/49 (neu) und alle frueheren bleiben Vorschlag bis zur Freigabe (§7).
* (erledigt mit Nachbesserung 3, Nachtrag 7) ~~Nachbesserung 2 (2026-10-24, Abnahme 1 Mangel N1, Entscheid C-E2b): der Weg zum Rechner 2080 nach der Flut
  existiert nicht~~ — blockiert an der ORIGINAL-Sperre ROOM11A0/11A1 Slot 4 ((3,206); main00 @0x00FEA/@0x00FEE; nur
  ROOM20A0/20A1 sub10 @0x01FF0 setzt das Bit), gemessen zu Fuss mit Leon und Elza (§0c Glied 7, Bilder nb2_11a0/nb2_11a1).
  Keine Port-Sperre der Runde 41 auf dem Weg -> nichts erfunden, kein Code geaendert. Entscheidungsfrage §7 (j) an den
  Orchestrator. Riegel unit_r41_ventilkette J + integration_r41_ventilkette_weg halten den Befund fest (sie fallen,
  sobald jemand die Sperre oder die Zugaenge aendert, und muessen dann mit dem Entscheid neu gefasst werden).
* **Nachbesserung 1 (2026-10-24):** M2 bleibt eine Integrationsabhaengigkeit von Spur A (Entscheid C-E1) — mit dem
  wortgleichen Haken laeuft die ganze Kette in EINEM exe-Lauf (§0b M2, `r41c_nb1/mess/kette3`, Riegel-Lauf auf dem
  Endstand); der Ketten-Riegel `integration_r41_ventilkette_kette` wird nur mit A's Modul registriert (C-Baum:
  Configure-Zeile "uebersprungen: Spur A fehlt"). M5 umgesetzt (C-E2), M6 angenommen (C-E3). Offen als Fragen: M7
  (Ost-Tuer nach Bit 107, Frage g) und H2 (Frage h); M1, M3, M4, M8, M9, H1 behoben (§0b). H3 (PSX) unten.
* **Querbezug A — die 20B0-Szene haengt ohne den Haken der Spur A, mit ihm laeuft sie** (ausgewertet 2026-10-24 02:52):
  - OHNE Haken (integration A, Lauf build/tests/integration/r41_ventilkette/lauf_912f1ca0_a/debug.log, 00:47, exe
    00:41 = Quellstand 271b05ea): `[aot] DOOR FIRE slot=0 rect=(7350,-200,hw=500,hh=1750) target_cut=11
    spawn=(5950,-1800,-6400)` -> `[room] PC loaded room20b0.rdt` -> sub00 Ck(3,235)=0 @0x01AAA -> Evt_exec sub02 @0x01E06
    -> `[scd F900] Cut_chg(11)` (@0x01F00) -> `Plc_dest(slot=0 mode=0x05 dest=(-15920,-6355) flag=0)` (@0x01F24) ->
    Lauf nach Westen (Cut 12 F30, Cut 13 F60/F90) -> ab F120 steht der Spieler bei (-15820,-1800,-6355) in Cut 14, mo 2,
    bis zum Ende F1040: im Rechteck der Zone Slot 7 (@0x01DF2, (-17400,-8700,2250,4600), Band 1, sce 3 Ereignis 5),
    aber sub05 startet nie (kein `thread start ... first_op=0x46` = Aot_reset @0x01F5C, kein Cut_chg(7), kein
    `letterbox closed`). Ursache im Port: re15_aot_scan ueberspringt waehrend einer Szene jeden Platz ausser CAM_SWITCH
    (aot_common.c:1169 `if (in_cinematic && a->type != RE15_AOT_TYPE_CAM_SWITCH) continue;`; in_cinematic @1077 =
    player_mode 2 oder Balken schliessen noch, player_mode 2 setzt main.c:5795 solange re15_cine_active() = (1,27) ||
    (2,7), game_state.c:256), und sub02 setzt (2,7)=1 @0x01EF8 (`22 02 07 01`) / (1,28)=1 @0x01EFC (`22 01 1c 01`;
    Nachbesserung 1: nicht (1,27)); erst sub05 loescht (2,7) @0x01FC0 und (1,27) @0x01FC4 (`22 01 1b 00`). Ohne den Haken bleibt der Spieler in 20B0 unter der Pad-Sperre stehen.
  - MIT Haken (temporaerer Nachbau, NICHT committet: AUTO_EVENT in 20B0/20B1 trotz in_cinematic, Patch scratchpad
    r41c2/temp_haken_spur_a_aot_common.patch; Lauf scratchpad r41c2/mess/kette_mit_a, 00:56-00:58, Aufbau wie
    integration A ohne Kill-Bits, Ende F700#20B0, Bild C_belege/querbezug_a_20b0_szene_sheet.png): 2030 F900 Ost-Tuer
    -> 20B0 F0 Cut 11 + Pos_set (2380,-1800,-6355) @0x01F08 (Zustandslog F1 `PL(2380,-6355)`) -> Plc_dest wie oben ->
    beim Eintritt in Zone Slot 7 `thread start slot=11 first_op=0x46` = sub05 @0x01F5C (zwischen F100 und F105; mit 200
    je Bild ab F90 x -12420 erreicht der Spieler die Zonenkante x -15150 in F104) -> F105
    Cut_chg(7) @0x01F6A -> Plc_dest (-20370,-7305) @0x01F80 / (-21020,-9355) @0x01F8E (Bild F120: Cut 7, die Tuer
    fliegt auf) -> F146 Cut_chg(15) @0x01F9C -> Plc_dest Modus 9 (-22820,-7555) @0x01F9E / Modus 7 (-20620,-9705)
    @0x01FA8 -> Sleep 60 @0x01FB2 -> F228 Cut_chg(6) @0x01FB6 + Plc_ret @0x01FB8 (Zustandslog pm 2 -> 1) -> Cut_auto 1
    -> **Set(3,235)=1 @0x01FBC** -> Set(2,7)=0 @0x01FC0 -> Set(1,27)=0 @0x01FC4 -> Sca_id_set x10 -> Evt_end @0x01FF0.
    Set(3,235) wird nicht eigens geloggt: sub05 ist ohne Verzweigung (scd_dump_room.py), die Flanke von (1,27)/(2,7)
    meldet main.c:5797-5816 nach den 15 Abblend-Takten als `[scd F242] letterbox closed -> gameplay (player_mode=0)`,
    also liefen Set(3,235) und die beiden Freigaben im VM-Lauf von F228. **Spieler frei**: pm 0 ab F242 (Zustandslog),
    danach die freien Steh-Varianten mo 200 -> 210 (F343) -> 211 (F359) -> 212 (F448) wie im freien 2030 (F119/F135/
    F303), Bilder F260/F700 Cut 6 ohne Balken. (Die Wasserwand EM024 ist darin noch die Port-Fassung — Spur A.)
  - Der echte Haken kommt mit Spur A (Zweig r41/wasser20b0: aot_common.c `&& !re15_wasser20b0_r41_auto_aot(
    g_current_room_id, a->type)`, AUTO-Pass FUN_800436a8 @0x8001ce1c ohne Szenen-Gate) bei der Integration; in diesem
    Baum NICHT uebernommen. integration_r41_ventilkette A prueft den Szenenverlauf nicht und bleibt mit dem Haken
    gueltig; die Bits 103 + 100 setzt schon der Raumwechsel (Installer), nicht die Szene.
* **Geflutetes 2030 ueber den Ring betretbar** (NEU 03:03, §4 "Eintritt in das geflutete 2030"): nach der Flut (103
  ohne 107) fuehren 20B0 Slot 3 @0x019C2 und 2070 Slots 1/2 @0x00A5A/@0x00A7A auf den Ring (y -5400); die Flaeche steht
  1800 darueber (y -7200, PORT-WAHL §3.6), der Spieler steht und laeuft im Wasser, die Spinnen greifen an; ertrunken
  wird nur waehrend der Flut (900 ueber den Fuessen, s_flut). Der Wortlaut ("Wasser in 2030: Ueber die Ringebene",
  "spaeter wieder frei ohne Wasser") ist erfuellt, er legt aber nicht fest, was der Ring-Eintritt in den gefluteten
  Raum tut. Innerhalb der Zuteilung versucht: (1) Original-Text als Sperrsatz — 20B0 hat keine Texttabelle (msg_sec 0),
  2070 nur msg 0 "You've opened the lock." (rdt_msgdump.py); (2) ein Tuer-Riegel ohne eigenen Text —
  re15_aot_door_params_t hat kein Schloss-Feld, die Port-Sperren (tuersperre_r39.c, sperre1050_r37.c) nutzen je Raum
  eine Port-Id; (3) Ertrinken auch nach der Flut — ein Sofort-Tod hinter einer Tuer ohne Warnung. Keins davon ist
  belegt oder vertraglich gedeckt -> nicht gebaut, Entscheidungsfrage §7 (f) mit Vorschlag.
* PSX-Ziel: die Wasserflaechen-Daten (gen/ventilkette_r41_wasser.inc = ROOM2080 MD1 @0x000D98 4572 B + TIM @0x026408
  16928 B) sind nur unter RE15_PLATFORM_PC eingebunden (ventilkette_r41.c, PSX-Laufzeitbudget); Bits, Zaehler, Flut-Takt
  und Tod sind plattformneutral. Der PSX-Bau ist in diesem Baum weder gebaut noch gemessen (keine PSn00bSDK-Kette).
* (erledigt 22:00) Die Flut als exe-Messung: gemessen mit den Spinnen-Kill-Bits als Spielzustand (§4 "Flut am Spiel",
  Riegel integration C). Offen bleibt nur die ORIGINAL-Seite: RE1.5/RE2 kennen kein Ertrinken, Form = PORT-WAHL.
* Der Level 0x5000 (Helligkeit 160) der Ertrink-Blende ist PORT-WAHL: die ausgelieferten Skripte nutzen die Blende nur
  als Lampenflackern (11F0/11F1/3080, `57 00 00 08` = Helligkeit 16); ein ausgelieferter Tint-Wert existiert nicht.
* Verlaesst der Spieler 2030 waehrend des Zaehlers NICHT ueber die Ost-Tuer (sondern per Lift nach 2080/2060/2020),
  gilt die Flut ebenfalls als gelaufen (Bits 103 + 100); die 20B0-Szene laeuft dann beim naechsten Eintritt in 2000
  (= 20B0), gleich von welcher Tuer (die Original-Szene sub02 setzt den Spieler an die Ost-Tuer). PORT-WAHL, nicht
  vom Nutzer entschieden.
* (⛔ BERICHTIGT in Nachbesserung 2, §0b M7 / §0c: 2070 = drei getrennte Gaenge, 2040 ohne Aufzug — Treppe + Rost-Tuer
  mit Zeitbombe; der folgende Satz der Nachbesserung 1 bleibt als Protokoll stehen.)
* 2070 (der Gang, mit der Tuer nach 11A0) ist nach der Flut NUR noch ueber 2040 (Aufzug mit Strom) erreichbar: 2030
  Slot 0 bleibt auch mit Bit 107 auf 20B0 umgelenkt, und 20B0 hat (wie im Original-20B0) keine Tuer nach 2070
  (Nachbesserung 1, M7; die fruehere Fassung "von 2030/2040 aus" war falsch). 2070 -> 2000 landet in 20B0 am Westtor
  (die Szene erwartet den Ost-Eintritt). Frage §7 (g).
* Die Wasserflaeche in 2030 ist das 2080-Modell (Ecke als Ursprung, zwei Flaechen); unter Cut 4 (nur ueber den Steg
  erreichbar, der unter Wasser steht) zeigt sich ein Rand der Flaeche (C_belege/flut_vergleich.png links).
* Die Hebel des 2050-Pults sind gemalt (keine Modelle wie in 2060/11F0): Zustand = gruene Marke + Zeiger.
* RE2-Ton fuer ein Ventilrad gibt es in RE2_SOUND_LISTE.md nicht (gesucht: valve/wheel/pipe/turn): Ventil 13 nutzt den
  RE2-Panel-Klick 0x0A wie lift2030_r37/hebetisch.

## 7. Fuer den Nutzer
* **Stand Nachbesserung 3 (Nachtraege 7-10):** So laeuft die Kette jetzt: 2030 Pult (zu wenig Druck) -> Treppe hinab ->
  Ventil 13 am Tank -> Pult "Lower the bridge?" -> Bruecke unten -> ueber den Steg zum Sued-Landeplatz -> dort liegt die
  Sicherung (aufheben!) -> die nummerierten Ventile 09/08 an der Suedwand (ohne Sicherung: "Numbered valves. If I turn
  them, this whole level will be flooded. I should look around here first.") -> Ventile drehen -> Warnung -> 60 s ->
  ueber Steg/Treppe/Ring zur Ost-Tuer -> 20B0 (Auswasch-Szene mit Spur A) -> Kanal -> Leiter -> 2050 (Pult 80 V, Kisten)
  -> 2020 (Kurbel) -> 2030 Nordgalerie (noch geflutet) -> 2060: Sicherung einsetzen + Generator (Schalter 1 und 3) ->
  zurueck: 2030 Nordgalerie (jetzt nur noch der untere Bereich unter Wasser) -> 2020 -> 2050 -> Leiter -> 20B0 -> Kanal ->
  Ost-Tuer (jetzt offen) -> 2030 Ring (trocken) -> Lift B -> Suedgalerie -> 2080 -> Treppe -> Rechner (Original) -> 2090.
  Zu Fuss gemessen mit Leon und Elza (§0d, Riegel integration_r41_ventilkette_weg / _weg_elza).
* Nach dem Strom bleibt der untere Bereich geflutet (Pegel y -3600 = Oberkante der unteren Reling, Bildbogen
  C_belege/nb3_pegel_vorher_nachher_sheet.png) und ist gesperrt: die Treppe sagt "I cannot go down the stairs because of
  the water." **Dort liegen und sind danach verloren:** die Sicherung (2030 Slot 9 @0x01BAA — wird vor der Flut
  zwingend aufgehoben, weil die Ventile erst danach gehen) und **Shotgun Shells x14** (2030 Slot 10 @0x01BC0, Item 0x16,
  Sued-Landeplatz Ostende), dazu Ventil 13 und die Bruecke. Wer die Patronen will, nimmt sie vor dem Ventilraetsel mit.
* Keine neuen Assets im Paket-Gate (alles eingebacken in gen/*.inc aus den vorhandenen RDTs; das Netz der Relinghoehe ist
  aus ROOM2080 Prop 0 abgeleitet); keine Sprachdateien.
* (Historie) Nach der Flut sind die Tueren aus 20B0 (Ost) und 2070 in den Ring von 2030 zu ("I cannot open the door
  because of the water pressure.") — seit Nachtrag 7 nur noch, bis der Strom an ist. **⛔ BERICHTIGT (Nachbesserung 2):** den frueher
  hier genannten Weg "2050 -> 2020 -> 2030 oben -> 2080" gibt es nicht (die Tuerebene von 2030 hat zwei getrennte
  Galerien), und auch der Umweg 2050 -> 2040 -> 2070 -> 11A0 -> 20A0 -> 2090 -> 2080 endet an der Original-Tuer in
  11A0: "It's locked from the other side." (§0c, Bilder C_belege/nb2_11a0_sperre_leon_sheet.png /
  nb2_11a1_sperre_elza_sheet.png). Nach der Flut ist der Rechner 2080 derzeit NICHT erreichbar -> Frage (j).
* Bedienung: 2030 Ring-Nordwand (Cut 1/2) = Pult; Treppe hinab, am Tank rechts (Zelle 2) = Ventil 13; ueber den Steg
  zum Sued-Landeplatz: Westende = Sicherung, Suedwand vor 09/08 = die Ventile (Platzhalter des spaeteren Raetsels;
  Nachbesserung 3, vorher am Ring-Ost-Streifen ueber den Schildern 04/05/06). 2050: Pult an der Saeule
  (Slot 8), Cursor mit dem Steuerkreuz, SQUARE kippt einen Hebel, CROSS verlaesst das Pult; Loesung 30 + 40 + 10 V.
* Port-Texte zur Freigabe (englisch wie das Spiel; NUTZER-VORGABE = Wortlaut aus dem Auftrag):
  2000/20B0 40 "I cannot climb up the ladder / because of the water pressure." (VORGABE)
  2030 40 "A control panel for the bridge. / The pressure gauge reads zero. | Intake valve 13 is closed."
  2030 41 "Intake valve 13. / Will you open it?" (Ja/Nein) · 42 "You've opened valve 13. / The pressure is rising."
  2030 43 "The pressure gauge reads 80. / Lower the bridge?" (VORGABE "Lower the bridge?") · 44 "The bridge is down."
  2030 45 "Numbered valves. They control the / flood gates of the treatment pool. | Will you turn them?"
  2030 46 "WARNING: The water will rise / in one minute. Leave the room!" (VORGABE sinngemaess)
  2030 47 "Intake valve 13 is open." · 48 "I cannot go down the stairs / because of the water." (NEU, Nachbesserung 3:
  Treppe nach dem Strom) · 49 "Numbered valves. If I turn them, / this whole level will be flooded. | I should look
  around here first." (NEU, Nachbesserung 3: Ventile vor Bruecke/Sicherung); waehrend des Zaehlers zeigen die Ventile die
  Warnung 46 erneut (die frueheren Saetze 48 "... flooded" / 49 "... drained" entfallen)
  2050 40 "The pump has no power. / The switchboard must be set first." · 41 = 11F0 "Will you push them? " ·
  42 = 11F0 "Power supply OK." · 43 "A high voltage switchboard. / The pump needs 80 volts." ·
  44 "The switchboard is set to 80 volts. / The pump is powered."
  (2080 40 entfaellt mit Nachtrag 7 — der Rechner ist wieder reines Original.)
  20B0 41 = 2070 40 "I cannot open the door / because of the water pressure." (NEU, Entscheid C-E2: Ring-Tueren in das
  geflutete 2030 bis zum Entleeren gesperrt)
* Entscheidungsfragen: (a) Hebelgewichte 30/20/40/10 (Loesung Hebel 1 + 3 + klein) — anderes Muster gewuenscht?
  (b) Bruecke "oben" = Ringebene y -5400 — hoeher gewuenscht? (c) Flut auch beim Verlassen ueber die oberen Tueren
  (jetzt: ja, siehe §6). (d) Lift-Rufzonen oben nutzen nach der Flut den Original-Satz 1 ("not responding") —
  eigener Satz ("The elevator is flooded.") gewuenscht? (e) Ertrink-Effekt = Tod + blaue Blende (subtraktiv Rot+Gruen,
  Blau bleibt) + Spritzer; ein eigener Ton fehlt (RE2-Liste ohne Ertrinken).
  -> Orchestrator-Entscheide 21:37 (AUFTRAG.md): a-e alle angenommen; die Texte bleiben Vorschlag bis zur Freigabe.
  **(f) NEU (2026-10-24, §6):** Nach der Flut fuehren die 20B0-Ost-Tuer (Slot 3) und 2070 (Slots 1/2) auf den
  gefluteten Ring von 2030; Leon steht dort bis zur Brust im Wasser (Bild C_belege/ring_nach_flut_sheet.png). Vorschlag:
  diese Tueren sperren, bis der Rechner 2080 2030 entleert hat (Bit 107), mit einem Satz in der Form der Leiter-Vorgabe,
  z. B. "I cannot open the door because of the water pressure." (Mechanik wie die Leiter: Sperrtext-Satz sce 1, Form
  ROOM2030 @0x01B12). Braucht Port-Id 41 in 20B0/20B1 und Id 40 in 2070/2071 — VERTRAG §1.2 teilt C dort nur 20B0 Id 40
  zu, deshalb nicht gebaut. Der Weg zum Rechner bleibt offen (2050 -> 2020 -> 2030 Tuerebene -> 2080, ueber dem Wasser).
  **⛔ BERICHTIGT (Nachbesserung 2, Abnahme 1 N1):** dieser Weg existiert nicht (Nord-/Suedgalerie getrennt, L9) — der
  Vorschlag (f) und damit C-E2 beruhten auf einer ungemessenen Behauptung; Befund und Revisionsfrage §0c / (j).
  Alternative: so lassen (Waten in Brusthoehe). -> **Orchestrator-Entscheid C-E2 (05:25): Vorschlag angenommen, Ids
  zugeteilt, umgesetzt** (§0b M5).
  **(g) NEU (Nachbesserung 1, M7):** 2030 Slot 0 (Ost-Tuer) bleibt auch nach dem Entleeren (Bit 107) auf 20B0 umgelenkt;
  damit ist 2070 (mit der Tuer nach 11A0) nach der Flut nur ueber 2040 (Aufzug mit Strom) erreichbar [⛔ BERICHTIGT
  Nachbesserung 2: die Ost-Tuer fuehrt in den 2070-MITTELgang (2000 <-> Ring), nicht zur 11A0-Tuer im Nordgang; 2040 hat
  keinen Aufzug, §0b M7]. Soll die Ost-Tuer
  mit Bit 107 wieder nach 2070 fuehren (Original-Ziel)? Ohne Entscheid bleibt 2030 <-> 20B0.
  **(h) NEU (Hinweis H2):** steht Leon waehrend der Flut an den Ventilen, liegt sein Spritzer ausserhalb der Flut-Kamera
  Cut 4; sichtbar sind Wasser, blauer Schleier, Tod. Kamera vor dem Tod auf Leons Zone zurueckschalten (neue Regel ohne
  Vorbild) oder so lassen?
  **(i) (Abnahme M6):** die kleinen Haken der Spur C in main.c / game_step_common.c und die Querbezug-Flags ->
  **Orchestrator-Entscheid C-E3: angenommen.**
  **(k) NEU (Nachbesserung 3, Nachtrag 8):** Der Ruhepegel nach dem Strom steht auf y -3600 = Oberkante der unteren
  Reling (Steg/Landeplaetze): der Ring ist trocken, alles darunter 1800 tief unter Wasser (Bildbogen
  C_belege/nb3_pegel_vorher_nachher_sheet.png). Gemeint "bis an die Ring-Reling" (y -5400, das Wasser stuende buendig mit
  dem Ring)? Dann nur die Konstante RE15_R41C_WASSER_Y_RELING (und der Riegel) zu aendern.
  **(l) NEU (Nachbesserung 3):** Texte 48 "I cannot go down the stairs because of the water." (Treppe nach dem Strom) und
  49 "Numbered valves. If I turn them, this whole level will be flooded. I should look around here first." (Ventile vor
  Bruecke/Sicherung) — Wortlaut freigeben oder aendern.
  **(j) ERLEDIGT (Nachtrag 7: "Nach der Aktivierung des Stroms ist 2030 leer gelaufen"; Weg gemessen §0d N1):**
  ~~(Nachbesserung 2, Abnahme 1 N1 / Entscheid C-E2b) — an den Orchestrator:~~ Nach der Flut (Bits 103+100
  ohne 107) erreicht der Spieler den Rechner 2080 NICHT; 2030 bleibt geflutet, 2080/2090/20A0 (Alligator, Kanister-
  Szene der Spuren F/G) sind unerreichbar. Gemessen (§0c): bis 11A0 ist alles mit den Original-Raetseln zu Fuss
  begehbar; der Weg nach 20A0 endet an der ORIGINAL-Sperre ROOM11A0/11A1 Slot 4 (main00 @0x00FEA Ck(3,206)=0 ->
  @0x00FEE msg 2 "It's locked from the other side."), und (3,206) setzt nur ROOM20A0/20A1 sub10 @0x01FF0 (von innen).
  Vor der Flut ist 2080 ebenfalls unerreichbar (Lift B braucht den Generator 2060, der hinter der gesperrten Leiter
  liegt) — die Lage tritt also in JEDER Spielreihenfolge ein. Moegliche Entscheide (nichts davon gebaut, C-E2b):
  (j1) die 11A0-Tuer nach 20A0 nach der Flut oeffnen (Port: (3,206)=1, z. B. zusammen mit Bit 103) — dann gilt der in
       C-E2b gedachte Weg 2050 -> 2020 -> 2030 Nordgalerie -> 2060 -> 2040 (Zeitbombe) -> 2070 Nordgang -> 11A0 -> 20A0
       -> 2090 (Becken) -> 2080 -> Rechner (Glieder 2-9 gemessen bis auf die Strecken IN 20A0 und 2090). Das ist ein
       Eingriff in eine Original-Sperre und stellt den Alligator VOR den Rechner.
  (j2) einen Ring-Zugang nach der Flut offen lassen (C-E2 teilweise zuruecknehmen): Waten in Brusthoehe (Pegel y -7200,
       Ring y -5400) zu Lift B unten (Slot 7) -> Suedgalerie -> 2080. Lift B braucht Strom (3,126) = Generator 2060
       (nach der Flut erreichbar) mit der Sicherung aus 2030 Slot 9 @0x01BAA (Steg-Suedlandeplatz Band 1, nach der Flut
       5400 unter der Wasserflaeche) — ob man die nach der Flut noch holen kann/soll, waere mitzuentscheiden.
  (j3) die Flut erst zulassen, wenn 2080 schon besucht ist, o. ae. (aendert die Reihenfolge des Nutzer-Designs).
  Ohne Entscheid bleibt Punkt 6 ("entleert danach auch 2030") im Spielablauf nach der Flut unerreichbar; die Mechanik des
  Rechners selbst ist gemessen (L6, §0c Glied 9).
* Sichtprobe fuer den Nutzer: 2030 am Ring vor den Ventilen die Aktionstaste, Ja, Warnung wegdruecken, 60 s stehen
  bleiben -> Draufsicht, Wasser steigt ueber die Bruecke und den Ring, Spritzer, Tod mit blauem Schleier
  (C_belege/flut_echt_sheet.png). Mit den drei Spinnen im Raum (ohne Kill-Bits) stirbt man vorher an den Spinnen;
  im Spiel also erst die Spinnen erledigen oder den Raum durch die Ost-Tuer verlassen (-> 20B0).
* Nach der Flut: in 20B0 laeuft die Auswasch-Szene des Originals erst mit dem Haken der Spur A bis zur Freigabe (§6
  Querbezug A); ohne ihn bleibt Leon im Westgang (Cut 14) stehen. Mit dem Haken (gemessen, ein Lauf, §0b M2): Szene
  -> frei im Mittelgang -> an der Kante zum Kanal die Aktionstaste (Sprung hinunter), durch den Kanal, an der anderen
  Kante die Aktionstaste (Aufstieg) -> Westgang -> Leiter -> 2050 (Bild C_belege/nb1_kette_ein_lauf_mit_haken_a_sheet.png).
  **⛔ BERICHTIGT (Nachbesserung 2):** "Zum Rechner 2080 geht es ueber 2050 -> 2020 -> die Tuerebene von 2030 -> 2080"
  war falsch; nach der Flut gibt es derzeit keinen Weg zum Rechner (§0c, Frage j). Der Rechner selbst senkt 2080 und leert
  2030 (gemessen per Sprung und von der 2090-Tuer aus: Bild C_belege/rechner2080_sheet.png, raum2030_leer_vergleich.png,
  §0c Glied 9).
* Neue Belegbilder dieser Fortsetzung: C_belege/querbezug_a_20b0_szene_sheet.png (Szene mit dem Haken der Spur A),
  C_belege/ring_nach_flut_sheet.png (Frage f). Nachbesserung 1: C_belege/nb1_kette_ein_lauf_mit_haken_a_sheet.png (die
  ganze Kette in einem Lauf mit dem Haken der Spur A), C_belege/nb1_kisten_hebel_sheet.png (Kisten-Hebel gesperrt),
  C_belege/nb1_blende_d3d.png (Ertrink-Blende im beschleunigten Renderer vor/nach dem Tod).
