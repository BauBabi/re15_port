# Runde 39 — UEBERGABE (laufend; neueste Eintraege oben)

Fuer jede naechste Sitzung/jeden anderen Account geschrieben. Nach JEDEM Schritt hier eintragen und committen.
Pflichtlektuere: `CLAUDE.md`, `analysis/befunde_runde39/AUFTRAG.md` (Wortlaut), `VERTRAG.md` (Spuren, Wellen, Ressourcen).
Verfahren wie Runde 38 (`analysis/befunde_runde38/UEBERGABE.md` §2-§4), Release-Ziel **v0.8.27**.
Zeiten IMMER aus der Uhr (Get-Date/date) oder der Commit-Zeit, nie schaetzen.

## ✅ Fortschritt (neueste zuerst)
* 2026-10-18 19:58: Spur I funk10f1 BESTANDEN (Abnahme 0 ohne Maengel; Fix c95c4976): Szene beim ersten Betreten 10F1 durch die
  Kartenleser-Tuer (Ereignis 27, Bit (9,70) in k_port[]), Gesten aus ROOM1031 sub13 (@0x029A4-c2, @0x02A32, @0x02A00), msg 40-42,
  98+3 Tests. In die Integration gemergt (68bfe620, konfliktfrei; Integration enthaelt jetzt auch E2, nicht in master).
  FUER DEN NUTZER: Sprachdateien synchro/STAGE1/room10F1/main40.wav ("Hello, anyone, ..."), main41.wav ("...", optional),
  main42.wav ("Uhh, this Thing is broken!") fehlen; Schreibweise "We Need help!"/"this Thing" woertlich - Kleinschreibung?
* 2026-10-18 19:31: Nutzer-Nachtrag 5 (44b4e5a3): Schluessel 10F0/10F1 hoechstens halb so gross -> neue Spur M schluessel2
  (r39_kopf.js/VERTRAG f70f6ec4, Baum r39_schluessel2 auf f70f6ec4), Lauf wf_6a2be5eb-568, args `{"welle": ["M"], "slots": 1,
  "max_nach": 1, "basis": "f70f6ec4"}`.
* 2026-10-18 19:30: Integration mit E-Nachbesserung 2 + Logschalter-Fix (3fe9e358): 842/842 gruen (alle unit_/prop_/probe_ +
  Gorilla-/1141-/Zustand-Gift-Integrationen, ctest -j 4, 1438 s). NICHT nach master vor E-Nachbesserung 3 (Befund E: Wand-
  Durchstoss in 1141, laeuft wf_676d6740-b2f) - doppelte Energie verlaengert den Kampf = mehr Stoss-Gelegenheiten. Nutzer-
  Nachtrag 4 (1141: Rolltor frei, Szene erst beim Entriegeln, Elza hebt Waffe aus dem Inventar sonst Messer, John schiesst
  auf die Gorillas + zieht Aufmerksamkeit, Heranwinken am Ende) in AUFTRAG.md (master + Baum r39_dining1141) -> Spur J
  Nachbesserung, sobald deren Abnahme 0 (wf_7cea9b87-5d9) fertig ist.
* 2026-10-18 19:04: Spur E Nachbesserung 2 BESTANDEN (Abnahme 2; Fix b963e6bb): Gorilla-Regel fuer alle 0x27 (Zensus: nur
  ROOM11C0 + ROOM1141), 1141 mit Elza: 360 Energie, Flinch bei akku 36, 31. Treffer toetet, Szene bis Johns Abgang;
  unit_maggot_ai (Original-Riegel) misst den Original-INIT mit Haken aus. Integration: ff auf master 3640d6c1, Merge
  58b8591f, Logschalter-Kollision E/F getrennt (RE15_R39_GEGNER_LOG, 3fe9e358). Bau + alle schnellen Tests + Gorilla-/1141-
  Integrationen laufen (Regex scratchpad/integ_regex_e2.txt). ⛔ BEFUND E (vor dem Release klaeren): in 1141 kann der
  Original-Koerperstoss (Gorilla gegen Gorilla, R 3200, Kasten @0x80121350; Wandklemme FUN_8003bca8 nimmt die naehere Kante
  @0x8003bcec-cfc) einen Gorilla durch die 2000 dicke Westwand schieben -> Kampf endet nie (Versuch 5: x -12218); einmal
  Elza im Rear-up-Pin hinter der Suedwand (z -26068). Doppelte Energie macht es wahrscheinlicher. Naechster Weg: RE2-
  Gegenstueck (Koerperstoss + Wandklemme) lesen, Pin-Fall bildgenau messen.
* 2026-10-18 18:57: master-Regression gefunden (von Spur I gemeldet) und behoben: unit_r38_itemorte_raum seit Welle 2 rot
  - Spur H stellte das Loescher-Band auf Spielerband(y)+1 um, der Runde-38-K-Riegel schrieb fuer 1030 noch Band 1 fest;
  Verhalten richtig (integration_r38_itemorte / r37_feuerloescher / r39_elza_loescher gruen), Riegel auf Band 2 (96ab793c).
  Danach ALLE schnellen Tests in master: unit_/prop_/probe_ 822/822 (ctest -j 6, 673 s). Neue Regel (Memory
  reai-v2-gezielte-integration-querbezug): vor jedem master-Merge zusaetzlich alle 822 schnellen Tests im Integrationsbaum.
* 2026-10-18 17:06: MONATS-/SITZUNGSLIMIT (~14:58, Reset 17:00) beendete G/I/J/K und die E-Nachbesserung 2 nach ~35 min.
  Gesichert: karten-Reste 716eda1d (UNGEPRUEFT); K hatte einen Probe-Merge von r39/integration (MERGE_HEAD 4e2177bd) offen
  -> abgebrochen; master (Welle 1+2) in alle fuenf Baeume gemergt (dining1141: AUFTRAG.md-Konflikt, beide Seiten behalten).
  Fortsetzung 17:05 mit stand je Spur (args woertlich in orchestrator/args_welle3_forts.json): G wf_23e44d81-e5a,
  I wf_fcd48678-b0e, J wf_7cea9b87-5d9, K wf_4e57956b-fad, E2 wf_0f038a8e-3ac (modus nachbesserung, runde0 1).
* 2026-10-18 14:53: WELLE 2 IN MASTER: merge 82fc487d (D+E+F+H; Integration 238/238 + E-Gruppe 85/85, ctest -j 3), Hauptbaum
  configure+build gruen, exe 14:51:13, Startlauf exit 0 (direct3d, ABR2 ok). Baeume r39_{treffer,zustand,elza} entfernt;
  r39_gegner bleibt (E-Nachbesserung 2 laeuft), r39_integration bleibt fuer Welle 3.
* 2026-10-18 14:38: Integration Welle 2 Stufe H+F+D (Bau 14:03, 6de53176): 238/238 gezielte Tests gruen (ctest -j 3, 2016 s).
  Jetzt Bau mit E (4e2177bd) + E-Gruppe (Regex scratchpad/integ_regex_e.txt); danach Welle 2 -> master + Hauptbaum-exe,
  E-Nachbesserung 2 (Gorillas generell) als eigener Folge-Merge.
* 2026-10-18 14:27: Nutzer-Nachtrag 3 (dc330054): Aufsteher RE2 bleibt, EKG bleibt, Spind 15 bleibt, Stapeln auch Kiste =
  schon erfuellt (re15_itembox.c:102 nutzt re15_inventar_r39_stapelt); GORILLAS GENERELL -> Spur E Nachbesserung 2
  wf_6101b819-29c (args woertlich in orchestrator/args_e_nach2.json; r39/gegner vorher auf master gemergt 2486a028).
  Hinweis an Spur J in deren Baum-AUFTRAG committet (Gorilla-Tod in 1141 nicht an Schusszahl binden). Danach r39/gegner
  erneut in die Integration.
* 2026-10-18 14:20: Fable-MONATSLIMIT ("monthly spend limit") beendete die vier Welle-3-Bau-Agenten nach ~30 s (keine Commits,
  Baeume sauber). Nutzer stellte das Modell auf Opus 5.5 (default). Welle 3 NEU gestartet (gleiche args, basis 1038dc15):
  G karten wf_96165e9e-aa5, I funk10f1 wf_02f632c1-c4e, J dining1141 wf_78e6cc10-215, K heliport1171 wf_2e231bdd-63d.
  Commit-Attribution ab jetzt "Claude Opus 5.5". Integrationstest H+F+D (238) laeuft seit 14:04 in r39_integration.
* 2026-10-18 14:19: WELLE 3 GESTARTET auf master 1038dc15 (= Welle 1 + A1; Welle 2 liegt nur in r39_integration 4e2177bd,
  deren H+F+D-Tests laufen dort noch, E-Tests danach). Baeume r39_{karten,funk10f1,dining1141,heliport1171}, je Spur ein
  Lauf, Resume nur mit args `{"welle": ["<X>"], "slots": 1, "max_nach": 1, "basis": "1038dc15"}`: G karten wf_3fcc8bff-38e,
  I funk10f1 wf_2873e9d0-624, J dining1141 wf_af6323a2-e39, K heliport1171 wf_dcbbfeac-cdc. Danach: Welle 2 -> master +
  Hauptbaum-exe, Welle-2-Baeume entfernen; Welle 3 in die Integration, volle Suite, Release v0.8.27.
* 2026-10-18 14:14: Spur E gegner BESTANDEN nach Nachbesserung 1 (Fix f20734fa): Parking-Lot-Gorillas ROOM11C0 HP 360
  (Saat 180 @0x8011f514 x2) + Flinch erst ab 36 Energie (= 3 Pistolentreffer a 12 @0x8006ee44; NUTZER-VORGABE), Ein-
  Angreifer-Riegel der RE2-Hunde (ori 0x80 @0x801051f8; 11C1 Doppelbiss F1008/F1019 weg), Kriecher 0x81 in 1010/1011 +
  1220/1221 (Wecker sltiu 0xbb8 @0x80104718, echte Tuer), Gorilla-Schatten im Sprung auf +0x1ba (Original-Lage, Schwanz
  @0x80116f20-38 auf jedem Pfad; der Root-Skip-Haken der Abnahme 0 war falsch und ist raus); 49/49 Tests. In die
  Integration gemergt (4e2177bd, konfliktfrei; H+F+D-Tests laufen noch auf dem Bau davor) -> ALLE VIER Spuren der Welle 2 drin. FUER DEN NUTZER: HP x2 / 3-Treffer auch fuer die 1141-Gorillas
  (Wortlaut "beim Parking Lot" -> nur 11C0)? NACHLAUF: Gorilla-Schattengroesse Original 1700/1800, Port 500/600.
* 2026-10-18 14:03: Spur D treffer BESTANDEN nach Nachbesserung 1 (Abnahme 1 f0eec254; Fix bd0f0471): (1) Aufsteher = RE2
  byte-true - Sturz-Aufsteher bis Clip-Bild 55 nur Beine-Maske (@0x801032b8-fc, @0x801036d0-f0, Maskentor @0x800413c4-d8),
  EBEN-Schuss trifft erst ab Bild 55, TIEF jederzeit; Port war schon so, Riegel misst jetzt (17 CHECKs); (2) Schuss ueber
  Moebel: RE2-Flag-Praxis (FUN_80050858 @0x800508bc-c8), Moebel-Liste PORT-WAHL (218 Zellen, 11 Leon-Raeume + Elza),
  ROOM1250 0/6 -> 6/6 Treffer; (3) Zielhilfe nur bei Sichtlinie (ROOM1180: Ziel hinter Wand -> sichtbarer Zombie); 28/28
  Tests. In die Integration gemergt (6de53176, konfliktfrei). FUER DEN NUTZER (Entscheidung): Punkt 1 ist in RE2 genau so
  (Nutzer-Annahme "in RE2 nicht so" widerlegt) - RE2 behalten oder NUTZER-VORGABE "Aufsteher auch in Normalhoehe treffbar"
  (Nutzerwert, kein RE-Wert); Granate prallt weiter am Tisch ab; Kleinzellen (Lampe/Stativ/Loescher) durchlaessig.
* 2026-10-18 13:46: Spur F zustand BESTANDEN nach Nachbesserung 1 (Abnahme 1 07312f9a; Fix 8c9c104e): volle Heilung = ganze
  Kleidung (Wund-Tabelle genullt + Spielerhaut neu wie FUN_800314b0, Schwelle 100 @0x8004ae14 / Weste 105; dabei Heap-
  Absturz bei Vollheilung MIT Weste (free auf Container-Schnittzeiger) gefunden und behoben), Gift -1 HP alle 31 Bilder wie
  RE2 (Takt @0x8003c154/64), EKG magenta "Poison"; 3 + 39 Tests. In die Integration gemergt (108cc2d7, konfliktfrei).
  H6 (Zitat-Fehler in Kommentaren: Haut-Upload ist @0x800315b0/@0x80031814, Builder @0x800316c8) vom Orchestrator in der
  Integration nachgezogen. FUER DEN NUTZER: EKG-Welle unter Gift ist die RE1.5-Welle in RE2-Farbe - die RE2-POISON-Welle
  passt nicht ohne erfundene Skalierung in den RE1.5-Monitor (Entscheidungsfrage, Dossier F 5).
* 2026-10-18 13:33: Spur H elza BESTANDEN (Abnahme 0 086a75dd; Fix 564810c0): 1031 Hand einmal (Latch @0x80041bb0 +
  Lauf-Clip am Walker-Sub-Init @0x800313a4/@0x80030b58/@0x80030d90 - gilt fuer JEDE Spieler-Plc_dest-Ausrichtung), Karte
  sofort nach Roys Tagebuch (Faden parkt auf Message_on @0x00E64; RE2 Opcode 0x84 @0x800591C4), Loescher-Prop 1090/1091
  auf Spielerband+1 (Zensus 558 Null-Boxen, Vorgaenger-Behauptung berichtigt); 144+3+9 Tests. In die Integration gemergt
  (66d351f1, konfliktfrei). Hinweise: 5 Bilder Rueckblende vor der Karte (RE2-Form); 1 Bild Sub-6-Pose zwischen TURN und
  WALK (vorher ebenso, eigener RE-Punkt); Sichtpruefung anderer Szenen durch die Walker-Aenderung in der Integration.
* 2026-10-18 13:02: Integrationsbaum 9e7bab74 (A1+B+C+L) gebaut, A-Tests 94/94 gruen (ctest -j 3, 725 s) - Merge der
  A-Nachbesserung verifiziert. Welle 2 laeuft (D/E/F/H).
* 2026-10-18 12:52: Hauptbaum auf master 6926409c neu gebaut (exe 12:50:47, Startlauf exit 0, EXIT_AT erreicht). Welle-1-Baeume
  r39_{toene,inventar,optik,stimmen} + Zweige entfernt (alle in master; 141 GB frei). Integrationsbaum r39_integration
  (9e7bab74) bleibt fuer Welle 2; dort laeuft die Gegenprobe der 94 A-Tests nach dem A1-Merge.
* 2026-10-18 12:50: Spur A Nachbesserung 1 BESTANDEN (Abnahme 1 dfbcf43b; Fix ad16c58f): Stufe-2-Lautheits-Boden
  stimmen_pegel_r39.c (T=985 R=2040 C=21759 aus dem Zensus aller 216 synchro-Dateien; nur Irons' 10 Dateien 6,9-12,6x,
  206 bitgleich; Irons in der Sterbeszene -0,1/-4,5/-4,9/+17,1 dB statt -10..-12 dB, 0 geklemmt), 1150 msg 29 <- main22,
  11B0-Text belegt; 94/94 Tests. Zwei Doku-Maengel vom Orchestrator nachgezogen (82640397). Merges: Integration 9e7bab74,
  master 6926409c; Hauptbaum-Bau laeuft. FUER DEN NUTZER: main19/main20 ("not going to make it" / "one last favor") bleiben 4,5-4,9 dB
  unter der BGM, weil je ein 0,2-0,5-ms-Knall die Anhebung ohne Clipping begrenzt - Neuexport dieser zwei Dateien waere der Weg.
* 2026-10-18 12:06: SITZUNGSLIMIT (~11:08, Reset 12:00) beendete die vier Bau-Agenten der Welle 2 und die Abnahme der
  A-Nachbesserung (deren Bau-Agent war fertig: ad16c58f, Stufe-2-Lautheits-Boden T=985/R=2040/C=21759 aus dem Zensus der
  216 synchro-Dateien, 1150 msg 29 <- main22, 94/94 Tests). Reste gesichert: treffer ff1649c1, gegner 9ddf0bbc, zustand
  87180698 (UNGEPRUEFT), elza sauber (08f3094d). Fortsetzung 12:05: A Abnahme 1 (modus abnahme, runde0 1) wf_63fe7405-030;
  D wf_6e5db63e-eef, E wf_413d6880-3a4, F wf_d11dbab6-b20, H wf_3b62557a-59b mit stand-Texten (args woertlich in
  orchestrator/args_welle2_forts.json). Waechter neu.
* 2026-10-18 10:25: WELLE 1 IN MASTER: merge c71fa144 (A+B+C+L, 237/237 gezielte Tests in der Integration), Hauptbaum
  configure+build gruen, exe re15_port/build/platform/pc/re15_pc.exe 10:20:06, Startlauf exit 0. Spur A Nachbesserung 1
  (wf_45b12abc-f7d, Irons-Pegel + main22) laeuft noch -> eigener Merge spaeter. WELLE 2 GESTARTET auf master c71fa144
  (Baeume r39_{treffer,gegner,zustand,elza}), je Spur ein Lauf, Resume nur mit args `{"welle": ["<X>"], "slots": 1,
  "max_nach": 1, "basis": "c71fa144"}`: D treffer wf_a1200483-99d, E gegner wf_8c3955b1-9ff, F zustand wf_5a65713f-27b,
  H elza wf_7e1608d3-5a9. Welle-1-Baeume (toene/inventar/optik) koennen nach dem A-Nachtrag entfernt werden (stimmen
  bleibt bis zum zweiten Merge).
* 2026-10-18 10:19: Integration Welle 1 KOMPLETT (fc7a0231 = A+B+C+L): Bau gruen, A-Tests 93/93 (ctest -j 3, 749 s), zusammen
  mit B+C+L 237/237 ohne Wiederholung. Spur A Nachbesserung 1 (wf_45b12abc-f7d) laeuft noch - nach ihrer Abnahme erneut
  mergen, bauen, A-Tests. Naechster Schritt: Integration -> master, Hauptbaum-exe (nur wenn der Nutzer nicht spielt).
* 2026-10-18 10:06: Integration Welle 1, Stufe B+C+L (Bau 09:32): gezielte Tests 144/144 gruen (ctest -j 3, 1954 s, keine
  Wiederholung noetig; Regex in scratchpad/integ_regex_bcl.txt der Sitzung). Jetzt: Bau mit A (fc7a0231) + A-Tests (93).
* 2026-10-18 09:49: Nutzer-Nachtrag 2 (AUFTRAG.md 09:48, 32ab1f97): Irons lauter = Port darf ueber 4x anheben (Form messen);
  1150 msg 29 <- neue Datei main22.wav; 11B0 "unwanted followers" steht schon so (szene11b0_bauen.py:286) - nichts zu
  aendern; 1171 passt. master in r39/stimmen gemergt (275a322c), Spur A NACHBESSERUNG 1 gestartet: wf_45b12abc-f7d
  (args woertlich in orchestrator/args_a_nach1.json: modus nachbesserung, abn = die drei Nachtrag-Punkte als Maengel).
  Danach r39/stimmen erneut in die Integration mergen. Welle-1-Integration (B+C+L gebaut 09:32, A gemergt fc7a0231,
  noch nicht gebaut): B/C/L-Tests laufen seit 09:33 (-j 3).
* 2026-10-18 09:34: Spur A stimmen BESTANDEN (Abnahme 0, bb285672; Bau 90d6395e): Ursache = Lader sucht main<MSG-ID>.wav,
  Nutzer legte mit der naechsten freien Nummer ab; Tabelle (Raum, Nachricht) -> Datei (stimmen_r39.c): 1050 msg 22-24 <-
  main09-11, 1150 msg 22-28 <- main15-21, 11B0 msg 20/21 <- main13/14, 1171 msg 3 <- main02 (Form RE2 xa_on @0x80058028),
  93/93 gezielte Tests. In die Integration gemergt (fc7a0231, konfliktfrei) -> ALLE VIER Spuren der Welle 1 ohne
  Nachbesserung bestanden. FUER DEN NUTZER: (1) Irons' Zeilen 1150 main18-21 sind -47 dBFS (22 dB leiser als Leon), der
  Port hebt max. 4x -> Irons liegt 10-12 dB unter der BGM: lauter exportieren oder Port-Anhebung >4x (Port-Wahl)?
  (2) 1150 msg 29 "Leon: SIR, Sir?!" hat keine Aufnahme (main29.wav wuerde sofort spielen). (3) 11B0 msg 21 spricht
  "unwanted followers" (Szenentext = Aufnahme), nicht "unexpected visitors". (4) 1171 msg 2 "C'mon, give me a hand!"
  zeigt das Original nie. Integration: B/C/L-Tests laufen (-j 3), danach Bau mit A + A-Tests.
* 2026-10-18 09:30: Spur L optik BESTANDEN (Abnahme 0, 9c149629; Bau 9726b606): Westwand-Masken ROOM1100/1101 Cut 1/2 enden
  an der GEMALTEN Ecke (Original hat dort keine Masken, pri @0x60C/@0x610 FF FF FF FF; Vorbild ROOM1030 Cut 3 Maske 2 endet an
  der gemalten Kante; Nutzer-Marke F741: Streifen 134 -> 1 Punkt), Schluessel 10F0/10F1 auf RE2-Bildgroesse (oberes Quartil
  L_nat*H/vz der RE2-Platzierungen; Leon 491 -> 69 px, Elza 217 -> 63 px bei 960x720), 64/64 gezielte Tests. In die
  Integration gemergt (5ba0cc96, konfliktfrei; r37_pri1100-/r38_itemorte-Riegel = gewollte Anpassungen). FUER DEN NUTZER:
  H1 Prison Key jetzt 16 px breit (Hebel RE2_QUANTIL in tools/r38_itemorte/itemorte_export.py, Max 1,0 = 26 px); H2 bei Elza
  bleiben 37 Punkte auf der Sockel-Kantenlinie 171/172 (Wandbasis, Streuung der Original-Masken); H3 zweite Marke
  befund_1090_F1357 = Kartenbildschirm (Positionsmarke ausserhalb) -> Spur G. Integrationsbaum: configure+build gestartet.
* 2026-10-18 09:02: Spur C inventar BESTANDEN (Abnahme 0, 103347cc; Bau a78ee047): Spind 1000 Munition 0x15 x15 Bit (9,69),
  Stapeln aller Mengen-Items (sltiu 0x22 @0x80049124, Obergrenze 250 @0x80074DA8, Aktion 5 @0x8004e664), Elza-Status
  STPIC_04 (DEBUG.BIN FUN_800c01c4), Yellow Keycard: Abfrage nach dem Code (Gate (3,139) @0x01A8A, Generator-E5 je
  Szenario-Variante); Alt-Riegel r21/r26/r37 nachgezogen. In die Integration gemergt (905e0f6d, konfliktfrei). FUER DEN
  NUTZER (Zu entscheiden): Spind-Munition 15 (Port-Halbierung 2026-09-20) oder 30 wie der Original-Satz ROOM1001; Granaten
  0x09..0x0B stapeln jetzt auch (Regel "alles mit Zahl") - gewollt?; "discard"-Abfrage gab es fuer die gelbe Karte vorher
  nicht - jetzt fragt ROOM11E1 nach dem richtigen Code (RE2-Form, einmal).
* 2026-10-18 08:10: Spur B toene BESTANDEN (Abnahme 0, 52b39720; Bau 66aea987): Schublade = ROOM11F0/11F1 (RE2 ROOM1130
  0x28), Gitter + Schrank = ROOM1190/1191 (RE1.5 ROOM2060 0x0E/0x0D, RE2 ROOM1120 0x0C), 31/31 gezielte Tests. Maengel M1/M2
  (Kommentare) vom Orchestrator nachgezogen. Integrationsbaum .claude/worktrees/r39_integration (r39/integration von master
  6d4abe35), B per --no-ff gemergt. Paket-Gate TOENE39.VBS in release/make_package.sh (H6). FUER DEN NUTZER (Release Notes
  "Zu entscheiden"): H1 Raumnummern (genannt 10F0/1090, gemessen 11F0/1190 - 10F0/1090 haben keine Schublade/keinen
  Schrank), H2 Regal-Welle klingt ~1 s nach dem Halt aus (RE2-Form), H3 Zu-Ton am Gitter zusaetzlich, H4 Schubladen-Ton bei
  jedem Oeffnen.
* 2026-10-18 07:20: WOCHENLIMIT beendete um ~03:10 alle vier Bau-Agenten der Welle 1 ("weekly limit, resets Oct 23";
  danach Kontowechsel, Modell Fable 5.1). Dossiers + wip-Commits lagen vor (A 3, B 4, C 3, L 3 Commits); Reste als
  UNGEPRUEFT gesichert (inventar dfde7745, optik 9bd581fe). Nutzer-Nachtrag (cb7d7855): 1141 Punkt 5 = JOHN, nicht Roy;
  NEUER Punkt "Gorilla-Schatten beim Sprung" -> Spur E (VERTRAG, r39_kopf.js). Welle 1 FORTGESETZT mit stand-Texten,
  neue Laeufe (args woertlich in orchestrator/args_welle1_forts.json): A wf_04388b3a-a3c, B wf_59e6e9bb-274,
  C wf_d5013a11-cfa, L wf_ccdd741a-c8e. Waechter neu geschaltet.
* 2026-10-18 02:36: WELLE 1 GESTARTET auf master 3e49ff87 (nach Release v0.8.26 754f67ad), je Spur ein eigener Lauf von
  orchestrator/r39.js (Waechter je Lauf). Resume nur mit IDENTISCHEN args `{"welle": ["<X>"], "slots": 1, "max_nach": 1,
  "basis": "3e49ff87"}`: A stimmen wf_28cb3826-7cd, B toene wf_6b21d705-6d7, C inventar wf_5fada65b-ca3,
  L optik wf_844e8fa6-7d9. Baeume .claude/worktrees/r39_{stimmen,toene,inventar,optik}.
* 2026-10-18 01:50: Vorbereitung waehrend des Release-Laufs 3 von v0.8.26 (keine Agenten, keine Last): VERTRAG.md (12 Spuren
  A-L, 3 Wellen zu je 4), orchestrator/r39_kopf.js (Spuren + Wortlaut + Einstiege) ->
  mk_r39.py -> r39.js (Rumpf aus r38.js, Syntax geprueft), mk_wt39.sh, nutzer/ (map*_correct.png, F9-Marken 1100 21:49 und
  1090 23:31 + befund.log-Auszug). Offene Nutzerfrage: J Punkt 5 "this is Roy!" — Roy ist in 1141 schon tot, es klopft
  John (Antwort in AUFTRAG.md nachtragen, Spur J liest sie dort).

## 1. Start einer Welle
1. Release v0.8.26 muss stehen (Tag v0.8.26 auf master). Basis der Baeume = master.
2. Baeume: `bash analysis/befunde_runde39/orchestrator/mk_wt39.sh <thema>` je Spur der Welle; `git worktree list` pruefen
   (Memory reai-v2-worktree-veralteter-basisstand).
3. Workflow: scriptPath `analysis/befunde_runde39/orchestrator/r39.js`, args `{"welle": ["A","B","C","L"], "slots": 4,
   "max_nach": 1}` (Welle 2: D,E,F,H; Welle 3: G,I,J,K). Waechter (Agent-Haenger, Platte) wie Runde 38.
4. Nach der Welle: Integration (Runde 38 UEBERGABE §3), gezielte Tests, master, Hauptbaum-exe bauen, Baeume entfernen.

## 2. Bei einem Abbruch
Wie Runde 38 UEBERGABE §2 (Resume mit identischen args aus dem Sitzungs-JSONL; Reste als wip committen).
