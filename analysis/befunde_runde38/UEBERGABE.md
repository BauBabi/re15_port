# Runde 38 — UEBERGABE (laufend; neueste Eintraege oben)

Fuer jede naechste Sitzung/jeden anderen Account geschrieben. Nach JEDEM Schritt hier eintragen und committen.
Pflichtlektuere: `CLAUDE.md`, `analysis/befunde_runde38/AUFTRAG.md` (Wortlaut), `VERTRAG.md` (Spuren, Ressourcen).
Verfahren wie Runde 37 (`analysis/befunde_runde37/UEBERGABE.md` §2-§4), Release-Ziel **v0.8.26**.
Zeiten IMMER aus der Uhr (PowerShell Get-Date) oder der Commit-Zeit, nie schaetzen.

## ✅ Fortschritt (neueste zuerst)
* 2026-10-17 12:41: U wasserzombie BESTANDEN (Nachbesserung 1 + Abnahme 1, wf_ff1d4812-6d4). Merge nach dem laufenden W-Test.
* 2026-10-17 12:41: W rolltor20a0 BESTANDEN (Nachbesserung 1 + Abnahme 1, wf_75bbfb7e-d5f; N1 = halbtransparente Raum-Objekte ABE/ABR/STP), gemergt (4c3bf9d2, main.c Includes beide Seiten; schluessel_r36 bleibt Ks Seite, da W nur As alten Stand trug), configure + Bau OK, gezielte Tests laufen (W-Auswahl 81 + Cursor/Itemorte/Zopf/Masken/1141-Sicht/Spinnen/Dachkiste). Alle tri_alpha-Setzer setzen auf 255 zurueck (geprueft). Hinweis Nutzer: gruene Flecken 20A0 (aktivierte schlafende Effekt-Schleife unter dem jetzt subtraktiven Wasser) -> Release Notes. Fuer N: Wasser-Spritzer gegen die subtraktiven Wasserflaechen pruefen.
* 2026-10-17 12:25: Integration S gruen: Riegel szenenschutz_alle umgestellt (741e3269 + Kommentar 447f7260; Fall "kleiner Affe 0x29" Biss aus o4 F137 + schwer aus o6 F1006, Teil 2(d) Szene mitten im Angriff; Gegenprobe Schutz aus -> 8 gehaltene Checks rot, Tore B[5]/B[6] aus -> T2d rot mit netz>0; unabhaengige Pruefung bestanden). Kein Engine-Befund: alle 0x29-Angriffswege lesen +0x93 an den Original-Stellen. Baum r38_affen3 entfernt.
* 2026-10-17 11:48: Integration S: 139/140 gezielt; ROT unit_r38_szenenschutz_alle - Fall "Kakerlake 0x29" (Spur B) setzt den Affen ohne EM029-Bank und ohne Raum auf Abstand 900; Ss neues Affen-Gehirn greift dort nie an -> Kontrolle "frei greift an" faellt, gehaltener Teil prueft nichts. Integrations-Workflow wf_9fe64976-04d (Fix-Agent + unabhaengige Pruefung): Fall auf das S-Gehirn umstellen, Gegenprobe (Schutz aus -> gehalten rot), Tor statt Netz pruefen.
* 2026-10-17 11:25: S affen3 BESTANDEN (Nachbesserung 1 + Abnahme 1, wf_d92ecd29-8d1), gemergt (f1511069; AUFTRAG.md = Integrationsseite, der wortgleiche Nachtrag stand schon drin), configure + Bau OK, gezielte Tests laufen (S-Auswahl 107 + 13 exe + Spiderman/Griffe/Spinnen/Szenenschutz).
* 2026-10-17 11:22: Integration O gruen (122/122: O-Auswahl + Zombies L + Szenenschutz + Irons/1150/1030/1040), Baum r38_montage entfernt.
* 2026-10-17 11:10: O montage BESTANDEN (Abnahme 0, wf_ec75d894-03c), gemergt (127100ef, konfliktfrei; enthaelt den L-Merge), configure + Bau OK, gezielte Tests laufen (O-Auswahl + Zombies L + Szenenschutz).
* 2026-10-17 11:04: Zwischenpruefung Integration (18 Spuren, HEAD 72653ac1): alle unit_/prop_/probe_ 755/755 gruen (-j3, 574 s); Suite jetzt 919 Tests.
* 2026-10-17 10:51: Integration D gruen (114/114: D-Auswahl + alle Elza-/Masken-/Skelett-/Cursor-/Tagebuch-Riegel), Baum r38_zopf entfernt.
* 2026-10-17 10:30: D zopf BESTANDEN (Abnahme 0, wf_3c7f69c5-a0f), gemergt (72653ac1, main.c Include beide Seiten), configure + Bau OK, gezielte Tests laufen (D-Auswahl + alle Elza-/Masken-/Skelett-Riegel). N5 (Zopf ueber P-Kasten) -> Grenzen + NACHLAUF #16.
* 2026-10-17 10:16: Integration R gruen (122/122: R-Auswahl + Spiderman/Griffe/1171-Szene), Baum r38_spinnen entfernt.
* 2026-10-17 10:12: R spinnen BESTANDEN (Nachbesserung 1 + Abnahme 1, wf_40f9724b-eb5), gemergt (078b62af; nur AUFTRAG.md-Nachtraege beide Seiten), configure + Bau OK, gezielte Tests laufen (R-Auswahl 109 + Spiderman/Griffe). N fortgesetzt wf_a76ae9bd-445 (r38_w5.js, stand; ⛔ N aendert op_aot_set Breite/Tiefe vorzeichenlos -> bei der Integration breite Tuer-/AOT-/Item-Auswahl).
* 2026-10-17 10:07: Integration M gruen (117/117: M-Nebenwirkungsauswahl + 1141/1171/11B0/Szenenschutz), Baeume r38_npckante und r38_dining1141 entfernt.
* 2026-10-17 09:55: M npckante BESTANDEN (Abnahme 0, wf_bd5754a4-75e), gemergt (81f90f2e, enemy_ai_common.c Include beide Seiten), configure + Bau OK, gezielte Tests laufen (M-Nebenwirkungsauswahl + 1141/1171/11B0/Szenenschutz).
* 2026-10-17 09:55: Integration F gruen (142/142: F-Auswahl + H/G/I), Herz-Schluessel ROOM1011 unter NCLIP gemessen: Integrations-exe und F-exe (ohne Verwurf) zeigen dieselben 2 Pixel (x 114-115, y 156-157, Cut 3, Skala 1) -> Verwurf unschaedlich; Schluessel ist aber nur ein Punkt (Release Notes Grenzen).
* 2026-10-17 09:55: Fortsetzungen nach dem Limit (≤6 parallel): D Abnahme 0 (RESUME wf_3c7f69c5-a0f, Bau aus Cache), W Nachbesserung 1 (RESUME wf_75bbfb7e-d5f), R Abnahme 1 (RESUME wf_40f9724b-eb5), S Nachbesserung 1 (RESUME wf_d92ecd29-8d1), U Abnahme 1 (RESUME wf_ff1d4812-6d4), O Bau-Abschluss neu wf_ec75d894-03c (stand). Warten: N. Args fuer Resumes aus dem Sitzungsprotokoll (scratchpad/run_args.json, ueber "Run ID" im tool_result zugeordnet).
* 2026-10-17 09:55: ⛔ DRITTES SITZUNGSLIMIT ~09:25 (Reset 09:50) brach D (Abnahme 0), N (Bau), R (Abnahme 1), S (Nachbesserung 1), U (Abnahme 1), W (Nachbesserung 1), O (Bau) ab; M war 09:33 bestanden. Reste gesichert: R/U Teilberichte (*_teil_limit.md), S/W/O Code-Reste wip UNGEPRUEFT. Lehre: 8 parallel erschoepfen das 5-h-Fenster in ~4,5 h -> jetzt hoechstens 6.
* 2026-10-17 09:00: ⛔ ZWEI AGENT-HAENGER entdeckt (Agent-Protokoll ohne Schreibzugriff, letzter Werkzeug-Aufruf kehrte nie zurueck, keine exe mehr aktiv): D (Bau, seit 06:29, Bau+Messlauf in EINEM Aufruf) und U (Abnahme 1, seit 07:32, exe-Lauf 2050->2020). D: TaskStop w1yf8bxpj, Reste 447c0e35 UNGEPRUEFT, neu wf_3c7f69c5-a0f (r38_w4.js, stand + Werkzeug-Regel). U: TaskStop wqqtr5607, Teilbericht 76b6fa12, RESUME derselben Run-ID wf_ff1d4812-6d4 mit identischen args (Bau/Abnahme 0/Nachbesserung 1 aus dem Cache, Abnahme 1 neu). Waechter (scratchpad/waechter.sh, Monitor): Platte < 35 GB + Agent > 75 min still; aktive Laeufe in scratchpad/aktive_runs.txt.
* 2026-10-17 08:49: Integration A gruen (202/202 gezielt, inkl. integration_r38_cursor E/F mit K-Schluessellage), Baum r38_cursor entfernt. F gemergt (f50f9eda): Include-/Haken-Zeilen beide Seiten; probe_r26_inventar.c B4 von Hand vereint (G: genaue Pruefung unter Basis 0x48; F: dritter Port-Name 0x43 -> 9 x (0x48 - 3)); configure + Bau OK; gezielte Tests laufen (F-Auswahl + H/G/I). Danach: Herz-Schluessel 1011 unter NCLIP an der exe pruefen.
* 2026-10-17 08:44: F dining1141 BESTANDEN (Nachbesserung 1 + Abnahme 1, wf_356a3960-7df). Merge nach dem laufenden A-Test. Integration F: neue Assets shared_assets/RE2/KLOPF1060.EDT/.VH/.VB (Paket + Android-Gate pruefen), ROOM1021 Ereignis 2 jetzt Programm SZENE1021 (Gosub sub02) -> gemeinsamer Lauf mit H (PC-Anker @0x020B2) fahren; Herz-Schluessel mit NCLIP-Verwurf (A) in ROOM1011 an der exe pruefen (A_belege/port_modelle_sicht.py). O fortgesetzt wf_8204134c-a42 (r38_w5.js, stand: r37/re2zombie zuerst mergen).
* 2026-10-17 08:02: A cursor BESTANDEN (Abnahme 0, wf_283a8270-fcf), gemergt (f27076e4; schluessel_r36.c/.h = Seite K wie A-Dossier §8, A-Aenderungen lagen nur in den Konflikt-Abschnitten), configure + Bau OK, gezielte Tests laufen (A-Auswahl + itemorte). N fortgesetzt wf_7044a5d9-64e (r38_w5.js, stand). REGEL fuer die Integration (A §8): jedes Port-Modell in der Prop-Schleife muss mit dem NCLIP-Verwurf sichtbar sein (raeumlich wie das Original gewickelt, flach mit der Vorderseite zur Kamera) - Pruefwerkzeug A_belege/port_modelle_sicht.py; betrifft F (Herz-Schluessel), W (Rolladen 20A0).
* 2026-10-17 07:42: Integration B gruen (466/466 gezielt, inkl. 1171-Kraehe RE2/RE1.5 und T-Griffe), Baum r38_szenenschutz entfernt.
* 2026-10-17 07:36: B szenenschutz BESTANDEN (Nachbesserung 1 + Abnahme 1, wf_2545408e-a2b), gemergt (8877e863, enemy_ai_common.c Include beide Seiten), configure + Bau OK, gezielte Tests laufen (438 Unit aus B_belege + Kraehe/Hund/Griff/1171/Spiderman). T liest das Tor (hit_react==0 vor GRAB2), RE2-Spinne liest re15_player_is_grabbed -> B greift dort. Nachtrag "Spieler-Tor" in AUFTRAG.md von R (54f4aeaa) und S (0686d425). M fortgesetzt wf_bd5754a4-75e (r38_w4.js, stand). NACHLAUF (B, nicht beauftragt): Bewegungs-Gate des Ports ignoriert Spieler+0x4 (Original @0x80031c8c -> Executor @0x80073f90[4]); ROOM1230 sub21 Leon laeuft 30 Bilder - eigene Spur mit Zensus aller Plc-Subs.
* 2026-10-17 06:30: Integration K gruen (53/53 gezielt: itemorte, Feuerloescher, Schluessel, Kistenregel, Kartenitems/-bits, 10F0, 1030, Tagebuch), Baum r38_itemorte entfernt.
* 2026-10-17 06:16: Integration G gruen (148/148 gezielt), Baum r38_tagebuch entfernt. K gemergt (97490964, konfliktfrei); Bau scheiterte zuerst am Linker (neue Datei itemorte_r38.c -> GLOB) -> configure + build OK, gezielte K-Tests laufen.
* 2026-10-17 05:56: D fortgesetzt wf_76712d27-884 (r38_w4.js). Warten: M, N, O.
* 2026-10-17 05:55: K itemorte BESTANDEN (Nachbesserung 1 + Abnahme 1, wf_02f9bc41-f8f; Prison Key = RE2 Small Key). Merge nach dem G-Test.
* 2026-10-17 05:52: G tagebuch BESTANDEN (Abnahme 1, wf_fef4a19f-77f), gemergt (5f5d0089, Includes/Haken beide Seiten), Bau OK, Tests laufen. S fortgesetzt wf_d92ecd29-8d1.
* 2026-10-17 05:51: Integration T gruen (220/220 gezielt: Gegner/Griffe/Raeume), Baum r38_spiderman entfernt.
* 2026-10-17 05:36: T spiderman BESTANDEN (Abnahme 1, wf_7931fdbe-adb), gemergt (96be43ad, Includes beide Seiten), Bau OK, Tests laufen. R fortgesetzt wf_40f9724b-eb5.
* 2026-10-17 05:30: Integration V gruen (9/9: r38_releaseskript + Android-Pruefketten), Baum r38_releaseskript entfernt.
* 2026-10-17 05:11: W fortgesetzt wf_75bbfb7e-d5f (r38_w3.js) mit Auftrag, r38/cursor zu mergen: A und W bauen beide den NCLIP-Rueckseiten-Verwurf fuer Raum-Props (A wickelt Port-Modelle um, W zeichnet sie zweiseitig) -> EIN Weg im W-Zweig.
* 2026-10-17 05:11: V releaseskript BESTANDEN (Abnahme 1, wf_9f166cb7-bd0), in r38_integration gemergt (4e3a8f60), Tests laufen. ⛔ FUND der V-Abnahme: C:\workspace\Re15Data (Paketarchiv v0.8.7-v0.8.14 + v0.8.25, ~7,5 GB, und weitere Daten) existiert seit 16.10. 17:19-18:26 nicht mehr (C:\workspace mtime 18:26); kein Agent hat schreibend darauf zugegriffen (Transkripte), Papierkorb leer. Alle Paketversionen liegen vollstaendig in den Git-Tags (je 6 Dateien) -> wiederherstellbar; Nutzer informieren.
* 2026-10-17 04:55: ⛔ ZWEITES SITZUNGSLIMIT ~23:45 (Reset 02:00) brach alle Laeufe ab; DuckStation PID 33976 beendet, EnableGDBServer zurueck auf false (BOM entfernt). Reste gesichert (wip UNGEPRUEFT: affen3, itemorte, montage, npckante, rolltor20a0, spinnen, zopf; Abnahme-1-Teilberichte V, T). Lehre: 15 parallele Laeufe erschoepfen das Fenster in ~2,5 h, die meisten Bauten brechen ab -> jetzt hoechstens 8 gleichzeitig. Neu gestartet: V wf_9f166cb7-bd0 / G wf_fef4a19f-77f / T wf_7931fdbe-adb (Resume, Abnahme 1), U wf_ff1d4812-6d4 (Abnahme 0), K wf_02f9bc41-f8f / B wf_2545408e-a2b (Nachbesserung 1 mit stand), F wf_356a3960-7df / A wf_283a8270-fcf (Bau-Abschluss). Warten: W, R, S, D, M, N, O.
* 2026-10-16 23:42: Zeitstempel 22:20-23:38 nach den Commit-Zeiten berichtigt (vorher geschaetzt, bis zu 1,5 h daneben).
* 2026-10-16 23:38: Integration H/I gruen (168/168 gezielt), Baeume elzatoene/heliport1171 entfernt.
* 2026-10-16 23:20: Integration P gruen (53/53), Baum r37_masken36 entfernt. H gemergt (scd_vm.c beide Seiten, 2a8b9abf), I gemergt (main.c Include beide Seiten, e10d9efb), Bau OK, gezielte Tests H/I laufen.
* 2026-10-16 23:05: H elzatoene BESTANDEN (Nachbesserung 1 + Abnahme 1, wf_10922d64-fbc). Merge nach dem P-Test (mit I).
* 2026-10-16 22:58: I heliport1171 BESTANDEN (Abnahme 0, wf_88be68ba-db7, Bericht e0b6ed35). Merge nach dem P-Test (Konflikte erwartet: enemy_ai_common.c Include mit B, scd_room_setup.c mit F).
* 2026-10-16 22:56: Welle 5 (r38_w5.js, BASIS d70c234e): N wasser wf_9fc9967b-c4b, O montage wf_83f7268d-331. Damit laufen ALLE 21 Spuren (bzw. sind fertig).
* 2026-10-16 22:51: Integration Q/E/J gruen (105/105 gezielt), Baeume kleinkram/elzaintro/rekorder entfernt. P (r37/masken36) gemergt (87ff0737, konfliktfrei), Bau + Masken-Tests laufen. Welle 5: N, O.
* 2026-10-16 22:41: Welle 4 (r38_w4.js, BASIS f587d34e): D zopf wf_08abf2fa-3ac, M npckante wf_d2a07ce1-679. Platte 67 GB. Offen zu starten: N, O.
* 2026-10-16 22:37: P (r37 masken36) BESTANDEN (Abnahme 2, wf_6fbf718a-cd3, Bericht 0ce11f5f). Merge nach dem laufenden Q/E/J-Test.
* 2026-10-16 22:32: Integration: L gruen (210 Unit + 7 Integration), r37_re2zombie-Baum entfernt. Q, E gemergt (konfliktfrei), J gemergt (Konflikte scd_vm.c/audio_pc.c = Haken nebeneinander, beide Seiten via konflikt_beide.py, 63ea4222). Bau + gezielte Tests Q/E/J laufen.
* 2026-10-16 22:20: J rekorder BESTANDEN (Abnahme 0, wf_fe613bfa-201). S fortgesetzt wf_350d90f9-b55 (r38_w2b.js).
* 2026-10-16 22:12: Bau-Fortsetzung R wf_ad7a778e-ca5 (r38.js), U wf_dc602f1a-a19 (r38_w2b.js). Warten: S (stand), D, M, N, O (neu).
* 2026-10-16 22:10: E elzaintro BESTANDEN (Abnahme 0, wf_4385667f-bb0, Bericht f6b23e6d). Merge nach dem laufenden L-Test (mit Q).
* 2026-10-16 22:05: Q kleinkram BESTANDEN (Abnahme 0, wf_a6f0cd46-193, 150/150 gezielt). Merge nach dem laufenden L-Test.
* 2026-10-16 21:55: L (r37 re2zombie) BESTANDEN (Abnahme 2, wf_232032c3-de2, Bericht ca4f0879): Erholungsleiter P3 wie RE2, Texte richtig. In r38_integration gemergt, Bau + Zombie-Tests laufen.
* 2026-10-16 21:40: Nutzer-Nachtrag 2 (AUFTRAG): John darf in 1141 nicht hereinploppen. F-Lauf wf_9ab3a238-dfd GESTOPPT (TaskStop, Mess-exe PID 46072 beendet), Reste 32754a59 UNGEPRUEFT, master (Nachtrag 2) in r38/dining1141 gemergt (0a680a7d). Neue Loesung: John steht schon beim Betreten im Dachgang hinter dem Rolladen, sichtbar erst beim Hochfahren. F neu gestartet mit r38_f.js (Punkt 4 = Nachtrag 2, Hinweise angepasst): wf_0cca805e-279.
* 2026-10-16 21:15: ⛔ SITZUNGSLIMIT ~18:18 (Reset 21:00) brach ALLE Laeufe ab. Reste gesichert: wip UNGEPRUEFT in r38_cursor/dining1141/heliport1171/itemorte/spinnen, Abnahme-TEILBERICHTE in elzaintro/elzatoene/rekorder/spiderman/tagebuch und r37 L/P. Neuer Nutzer-Befund -> AUFTRAG Nachtrag 1 / Spur W (ROOM20A0 Rolladen vor der Kamera), r38_w3.js (BASIS df08d83a). Fortsetzung 21:05-21:15: Abnahme per Resume (Bau aus dem Cache) E wf_4385667f-bb0, H wf_10922d64-fbc, J wf_fe613bfa-201, Q wf_a6f0cd46-193, G wf_fef4a19f-77f, T wf_7931fdbe-adb, V wf_9f166cb7-bd0; L wf_232032c3-de2 / P wf_6fbf718a-cd3 (modus abnahme, runde0 2, max_nach 1); Bau-Fortsetzung mit stand: F wf_9ab3a238-dfd, B wf_74e3029b-16f, I wf_88be68ba-db7, A wf_9a07eaab-cd0, K wf_239fbc09-716; neu W wf_3d75f890-088. Warten: R, S, U (Bau mit stand), D, M, N, O (neu).
* 2026-10-16 18:30: Integration C gruen 27/27 (r38_werkstatt11b0 + szenenleon + Ton/Dialog), Baum r38_werkstatt11b0 entfernt. CPU ~80 %, RAM 12,9 GB frei, Platte 83 GB -> Welle 2b nur teilweise: S wf_8071033e-d91, U wf_7508e561-3ed (r38_w2b.js, BASIS cb782bda). D, M, N, O folgen, wenn weitere Spuren fertig sind.
* 2026-10-16 18:15: C werkstatt11b0 BESTANDEN (Abnahme 0, wf_55628571-c21): "But, hold on...", Schiebe-Ton aus ROOM1171 Satz 7 (11B0 Satz 7 @0xE4A0 leer), keine Fall-Pose (Bank folgt dem Clip). Integrationsbaum r38_integration angelegt (98d35560), C gemergt (869b9374), Bau + gezielte Tests laufen. Offen fuer die Integration: Spur I gegen szenenleon_r36.c (Sub 0x13 auch in 1171), Android-Configure wegen werkstatt11b0_pc.c.
* 2026-10-16 16:40: Welle 2a laeuft (r38_w2.js, BASIS 1bcbd0ff, Baeume 16:28-16:39): Q wf_a6f0cd46-193, C wf_55628571-c21, K wf_f51a7d4d-ccc, E wf_4385667f-bb0, J wf_fe613bfa-201, A wf_539a5d00-022. Damit 16 Laeufe gleichzeitig. Platte 111 GB frei (vor den Builds) -> Welle 2b (D, M, N, O, S, U) erst, wenn erste Spuren integriert und ihre Baeume entfernt sind.
* 2026-10-16 16:30: Welle 1 laeuft (je Spur ein Workflow, max_nach 2): L wf_64c0b503-94f (r37_l_forts.js), P wf_4979f9b2-77a (r37_p_forts.js; beide Zweige vorher mit master gemergt: L 44721dca, P 2bb1bbe3, Konflikte nur AUFTRAG/VERTRAG -> master), F wf_27c697b6-a0e, G wf_fef4a19f-77f, B wf_904f6df5-9b8, T wf_7931fdbe-adb, R wf_679cb64c-b33, H wf_10922d64-fbc, I wf_43b458d6-510, V wf_9f166cb7-bd0 (alle r38.js, BASIS e2cc7673). Baeume Welle 1 angelegt 16:17-16:27, Platte danach 143 GB frei. Welle 2 (A, C, D, E, J, K, M, N, O, Q, S, U) folgt nach Last/Platte.
* 2026-10-16 16:10: Runde 38 angelegt (Auftrag woertlich, Vertrag mit 20 Spuren A-K, M-O, Q-V + Fortsetzung L/P der
  Runde 37). Orchestrator `orchestrator/r38.js` aus `mk_r38.py` (Vorlage `befunde_runde37/orchestrator/r37_r.js`), je
  Spur ein eigener Workflow-Lauf `args {"welle": ["X"], "max_nach": 2}`; L/P mit `befunde_runde37/orchestrator/r37_l.js`
  / `r37_p.js` im Modus `nachbesserung` (Stand aus den Journalen wf_8e797ea4-b39 / wf_62590deb-1b8, runde0 1).
  Neue Sprachdateien des Nutzers (synchro/) und die Nutzer-Bilder (nutzer/) committet.

## 1. Spuren (Status)

| Spur | Baum | Thema | Status |
|---|---|---|---|
| A | r38_cursor | Cursor-Schatten 1150/11F0 | BESTANDEN (A0), integriert |
| B | r38_szenenschutz | kein Angriff in Szenen ohne Bewegung (1190/11D0), Ausnahme 1171 | BESTANDEN (A1), integriert |
| C | r38_werkstatt11b0 | 11B0: "But, hold on...", Schiebe-Ton, keine Fall-Animation | BESTANDEN (A0), integriert |
| D | r38_zopf | Elzas Zopf wie Claire (RE2) | BESTANDEN (A0), integriert |
| E | r38_elzaintro | Elza-Intro Voiceover, Pre-Intro-Schwarz ohne Raum | BESTANDEN (A0), integriert |
| F | r38_dining1141 | John 1021 -> 1141 (Dining Room Key, Dach, Haemmern, Gorilla-Sperre) + Rolltor-Ton 1140 | BESTANDEN (A1), integriert |
| G | r38_tagebuch | Roys Tagebuch 1011 + Kartenmarken Comms/Prison | BESTANDEN (A1), integriert |
| H | r38_elzatoene | Schuss 1041, Roy-Stoehnen + John-Schuss 1021 | BESTANDEN (A1), integriert |
| I | r38_heliport1171 | Elza haengt in 1171 | BESTANDEN (A0), integriert |
| J | r38_rekorder | Rekorder: WAV -> Piep + Tuer + Meldung | BESTANDEN (A0), integriert |
| K | r38_itemorte | Loescher 1030, Schluessel an die Pinnwand-Marke | BESTANDEN (A1), integriert |
| M | r38_npckante | NPC-Kantensprung (John 10B1/1261, Marvin/Ada) | BESTANDEN (A0), integriert |
| N | r38_wasser | Wasser-Effekt 1260+ | offen |
| O | r38_montage | Montage-Zombies Richtung Kamera | BESTANDEN (A0), integriert |
| Q | r38_kleinkram | Inventar-Ton, Elza 1151 Irons, 30E0 Flammenwerfer | BESTANDEN (A0), integriert |
| R | r38_spinnen | Spinnen Stage 2 (Schaechte, Gift, Minispinnen) | BESTANDEN (A1), integriert |
| S | r38_affen3 | kleine Affen Stage 3 treffen | BESTANDEN (A1), integriert |
| T | r38_spiderman | Spiderman Stage 4 + Spieler bleibt auf der Karte (alle Gegner) | BESTANDEN (A1), integriert |
| U | r38_wasserzombie | Zombies ziehen sich am Wasserrand hoch | BESTANDEN (A1) |
| V | r38_releaseskript | Release-Skript alle Plattformen + Version + Archiv | BESTANDEN (A1), integriert |
| W | r38_rolltor20a0 | Nachtrag 1: ROOM20A0 Rolladen vor der Kamera (+ halbtransparente Raum-Objekte) | BESTANDEN (A1), gemergt |
| L (r37) | r37_re2zombie | Nachbesserung 2: Texte + Inversion re2z_hit_main case 3 | BESTANDEN (A2), integriert |
| P (r37) | r37_masken36 | Nachbesserung 2: Cut 15 3050/3051, Kopfoberkante | BESTANDEN (A2), integriert |

## 2. Bei einem Abbruch (Sitzungs-/Wochen-/Monatslimit)
1. Verwaiste Prozesse beenden (`tasklist`; nur per PID, nie /IM), DuckStation `settings.ini` EnableGDBServer = false pruefen.
2. Je Baum: `git -C .claude/worktrees/r38_<x> status --short` — Reste als `wip(r38-<x>): ... UNGEPRUEFT` committen.
3. Journal des Workflows lesen (`~/.claude/projects/c--workspace-git-reAi-v2/<sitzung>/subagents/workflows/<run>/journal.jsonl`),
   je Spur feststellen: Bau fertig? Abnahme Nr.? Dann Rest-Lauf mit args: unfertiger Bau -> `stand.X` (Text: Commits,
   was fehlt); Abnahme offen -> `modus.X='abnahme'` + `bau.X`; Nachbesserung -> `modus.X='nachbesserung'` + `abn.X` +
   `runde0.X`. Muster: `analysis/befunde_runde36/orchestrator/args_rest*.json`.

## 3. Integration (wenn Spuren bestanden sind)
Integrationsbaum `.claude/worktrees/r38_integration` von master; je bestandene Spur `git merge --no-ff r38/<x>` (L/P:
`r37/re2zombie`, `r37/masken36`); Konflikte (meist Installer-/Include-Zeilen, k_port[]) mit
`analysis/befunde_runde36/orchestrator/konflikt_beide.py <dateien>` (beide Seiten), AUFTRAG/VERTRAG/UEBERGABE vom master;
dann `bash re15_port/tools/local_build.sh configure && ... build`; gezielte Tests je Spur; ⛔ neue gemeinsame Regeln
koennen ALTE Riegel widerlegen (Runde 37: Q-Kistenregel gegen 3 Alt-Riegel) — nach dem Merge alle Riegel greppen, die
dieselben Bits/Raeume setzen.

## 4. Release v0.8.26
master in die Integration, volle Suite einmal seriell ohne Agenten (`local_build.sh all`, ~3,6 h, 788+ Tests),
RE15_MIN_TESTS anheben (Verlaufszeile), Release Notes (Entwurf `orchestrator/RELEASE_NOTES_v0.8.26_ENTWURF.md`), master ff,
Hauptbaum-exe bauen, dann das neue Release-Skript der Spur V (Windows + Linux/Deck + Android, Paket, Archiv, alte Version
aus dem Archiv) — oder bei Problemen die Einzelschritte wie Runde 37 §4; Release-Commit (`git commit -F`), Tag, Push,
Baeume entfernen, Memory, Abschlussmeldung.
