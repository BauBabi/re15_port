# Runde 36 — Nachlauf-Liste des Orchestrators

Funde der Abnahmen, die ausserhalb des Wortlauts der jeweiligen Spur liegen, aber nach den Regeln
("Original oder nicht", 100%-Mandat, Beta -> Retail) gebaut oder dem Nutzer gemeldet werden muessen.
Fortlaufend gepflegt; Quelle je Eintrag = Abnahmebericht im Spurbaum.

| # | Quelle | Fund | Stand |
|---|---|---|---|
| 1 | A_abnahme_2.md Mangel 2 | Objekt-Fall Typ 4 fehlt im Port: geschobene Kisten kippen/fallen im Original ueber Kanten (Objekt-Pass @0x8002c0fc-c14c -> Typ-Tabelle @0x80073c70 [4] = FUN_8002c40c -> FUN_8002c444, schreibt obj+0x82). ROOM5020/5021 Lage (4,203)=1 und vermutlich 20A0/20A1 (3,204)=1 entstehen im Port nie; Latch 5020 @0x012F4-302 / 20A0 @0x01AD4-e2 | Folgelauf wf_fc40f428-68b (Nachbesserung 3, Entscheidung: portieren) |
| 2 | B_abnahme_1.md Hinweis 6 | RE2-Hund-Claim PL+0x1D3 \|= 0x80 (@0x801051f8/0x80105200, Pruefung @0x80104e5c-68) fehlt -> mehrere Hunde beissen gleichzeitig (drei Bisse in fuenf Bildern, F582/F585/F587) | offen — Beta->Retail: portieren |
| 3 | B_abnahme_1.md Hinweis 3 | Unter dem RE2-Gehirn spawnt jeder ueberlebte Griff jetzt zusaetzlich das RE1.5-Release-Blut (FUN_80019700 a0=0x1500 @0x8010a574) | dem Nutzer ankuendigen (RELEASE_NOTES) |
| 4 | B_abnahme_1.md Hinweis 4 | ROOM1190 Kampfbild: erstes Decal (Panel 4 = Westen-Rueckseite) erst sichtbar, wenn Leon der Kamera den Ruecken zeigt | dem Nutzer ankuendigen |
| 5 | B Dossier OFFEN | STAGE3-5 (0,0x32)-Stempel (FUN_80112784 / FUN_8010de24 / FUN_8010dfa4), Birkin-Stempel STAGE5, RE2-Kraehe ohne Dive/Strike-Zwilling | offen (vorbestehend) |
| 6 | F_abnahme_0.md Hinweis | Lebender Gorilla hat jetzt den grossen Schatten aus dem disassemblierten INIT (Record 1700/1800 statt 500x600) | dem Nutzer zum Spieltest nennen |
| 7 | D_abnahme_1.md Hinweis 6 | Granaten-Menge erreicht 0 jetzt erst im Spawnbild — Spur I ("Granaten bei 0 entfernen") muss dort entfernen | ERLEDIGT in der Integration (D+I, e147a4ac): integration_r36_granate + integration_r36_inventar gemeinsam gruen; Grenzfall an der exe gemessen (Ds Griff-Lauf mit RE15_GIVE=9:2): Abzuege F19/F105 (ac=11), Wurf 1 SPAWN F43 (mg 2->1), Griff F113 vor dem Spawnbild F129, Endmenge mg=1 — die letzte Granate bleibt, Is Entfernung greift nicht |
| 8 | C Dossier "Fuer den Nutzer" | Liegender Spawn-Zombie (EXEC[7]) ist weiter treffbar; Dossier stellt das als Frage — Regel "keine Wahlfragen": RE2-Verhalten belegen und bauen | offen |
| 9 | A_abnahme_0.md H1 | ROOM2060 Startlage: die Spinne schiebt Leon nach dem Aufstieg vom Tritt, er schwebt auf y=-1800 (Original-Reihenfolge Push vor Standtest @0x80031cbc/@0x80031ce8; Erreichbarkeit im Original unbelegt) | offen |
| 10 | A_abnahme_0.md H5 | Debug-Sprung mit Elza nach 3091 landet in 3090 | Spur I (Debug-Menue Elza-Raeume) |
| 11 | E_abnahme_0.md H4 | Veraltete Kommentare nach dem Fix: irons_tod_1150.c:196/:635, include/re15_irons_tod.h:172-173 ("Records 15..19 ... erscheinen aber nicht") | in der Integration berichtigen ("bis Runde 35, RE15_ACTOR_MAX 16") |
| 12 | E_abnahme_0.md H5 | PSX-Ziel: g_actors waechst um 5 x sizeof(re15_actor_t); RAM-Budget ungemessen (PSn00bSDK fehlt) | offen (reai-v2-psx-build-gap) |
| 13 | E_abnahme_0.md H6 | ROOM20B0/20B1: Original schreibt Slots 20..34 hinter sein 20er-Feld (bis 0x800b1087); Port verwirft sie (PORT-ENTSCHEIDUNG) | offen, ausserhalb des Nutzer-Punkts |
| 14 | E_abnahme_0.md H2 | CONTINUE in ROOM20B0: Kamera bleibt Cut 0 (alt = neu, Ursache offen) | offen |
| 16 | C Nachbesserung 3 OFFEN 13 | Fress-Tropfen des fressenden RE2-Zombies fehlen (@0x80103C8C, Blut Id 0, Skala 5072 @0x80103C80, Part 8 @0x80103C90, a3 (200,0,0) @0x80103C68-7C, nur bei Bild-Wort & 0x10000 @0x80103C50-64) | offen — Beta->Retail: portieren |
| 17 | C Nachbesserung 3 OFFEN 9 | Feuer-Emitter (Id 5, @0x80106180/220/254/440/470) tragen Welt-Versatz (0,200,0) statt Anker-Versatz | offen |
| 18 | C Nachbesserung 3 | Kriecher-Kopfabriss = Id 8 (Fleischbrocken, @0x80102AD4-DC), nicht Blut; in Raeumen ohne Raum-Id 5 erscheinen keine Brocken (gore-vollausbau §2.1) | Hinweis |
| 19 | L_elza.md §8 | Neues Asset fuer das Paket-Gate: re15_port/shared_assets/RE2/ELZASE.VBS (13144 B, md5 8e996ef238defe2188292e55af7f8ab2, tools/r36_l/re2_elza_se_cut.py --install); neue Quelldateien engine/src/elza_r36.c + platform/pc/src/elza_se_pc.c (Android-Configure frisch) | beim Paketbau: make_package.sh check_tree + Android-GLOB |
| 20 | L_elza.md §8 | main16.5 = ein einzelnes Stoehnen; Untertitel jetzt "Roy: groans" (wie Roys Original-Zeile msg 10), ROOM1011 Nachricht 24 (nicht 1101) | Frage an den Nutzer: anderer Wortlaut? |
| 21 | L_elza.md §8 / OFFEN 1 | Roy sackt in ROOM1021 beim Schuss nicht sichtbar zusammen | naechste Runde (Original messen) |
| 22 | Orchestrator | Elza-Sprachdateien des Nutzers waren unversioniert (synchro/STAGE1/room1011/1021/1031/1041/1141/11B1/1211) — versioniert auf master 565da62b | erledigt |
| 15 | A Nachbesserung 3 | Neu fallfaehig ohne exe-Lauf: ROOM1090/1091 obj 1 (Band 5 -> 3; Zweck der Kiste = Frage an Spur H), ROOM3050/3051 obj 1, ROOM2050/2051 obj 6; DuckStation-Gegenmessung 5020 nicht gefahren | in Abnahme 3 / Integration pruefen |
