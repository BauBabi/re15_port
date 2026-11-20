/*
 * RE1.5 Rebuilt — Runde 42 "story" (v0.8.31): VERTRAG der Story-Ressourcen. NUR Konstanten (Bank-9-Bits und Nachrichten-
 * Ids), noch KEIN Spielcode. Grundlage: Story-Konzept C:/workspace/git/reAi_v2_entscheidung_2026-11-06/11_story_v0831/
 * KONZEPT.md (Nutzer 08.11.: "Ja, rein story technisch ist der Rest fantastisch."; Korrekturen K-01..K-08 bindend).
 * Vertrag mit Zensus, Belegen und Sprechern: analysis/befunde_runde42/VERTRAG.md; Pruefwerkzeug + Ausgabe:
 * tools/r42_story/zensus_r42.py -> analysis/befunde_runde42/belege/zensus_r42.txt.
 *
 * PORT-STORY: alles hier ist neue Story nach dem Konzept (Lueckenfueller nach 80-%-Material und Nutzer), KEIN Original-
 * Verhalten. Ausnahme ist RE15_R42_BIT_ENZYM = das Disc-Bit der 40-%-Disc (Beleg dort).
 *
 * ZENSUS Bank 9 (g_game.flags, Original DAT_800b1078 = Tabelle 0x80074664[9] @0x80074688; gespeichert mit dem Spielstand):
 *   Original (Mod-Baum shared_assets/PSX, tools/r36_kartenbits/bank9_zensus.py original, 2026-11-14): 85 Bits belegt,
 *     frei u.a. 23..51, 100..103, 106..107, 110..133 (Bank 9 nur ueber RDTs: Teil exe findet keinen EXE-/Overlay-Zugriff).
 *   40-%-Disc (Version_40%/PSX, derselbe Zensus): 15 Bits belegt (2 6 10..22), 23..255 frei.
 *   Port (bank9_zensus.py port + test_r36_kartenbits.c k_port[], dieser Baum = r41/integration + Gorilla-Filme):
 *     53..84, 87..89, 93..97, 100..103, 106..107, 110..117, 125, 128, 133, 134, 236, 238, 243.
 *   Offene r41-Zweige (git diff HEAD...<zweig>, 2026-11-14): nur r41/moosgang 126/127; form/sichel_szene/sichel_v8/
 *     integration/master ohne neue Bank-9-Bits.
 *   -> frei in allen drei: 118..124, 129..132, 23..51 (Auszug). Die Zuteilung unten nimmt NUR daraus (spaeter: 137 Strang finale,
 *      139 Strang fabrikplan, je mit eigenem Zensus).
 *   Jedes Bit hier steht in tests/unit/test_r36_kartenbits.c k_port[] (Ausnahme Lageplan-Bits: die gehoeren beim Bau in
 *   k_r36k_lage, sonst meldet teil_p "doppelt").
 */
#ifndef RE15_STORY_R42_H
#define RE15_STORY_R42_H

#define RE15_R42_BANK                 9

/* ---- Block B (v0.8.31): STAGE2/STAGE3 ---------------------------------------------------------------------------------- */
#define RE15_R42_BIT_2080_SZENE       118  /* L1/E1: Nachgespraech 2080/2081 gesehen (Konzept Anhang B "Bit 118", frueher Spur G) */
#define RE15_R42_BIT_2080_TOR         119  /* L1 K-01: Tor in 2080/2081 mit dem Schluessel geoeffnet (Rest der Spur-G-Reserve)    */
#define RE15_R42_BIT_LEITER           120  /* L3/E2 K-07: Szene vor der Leiter 11A0/11A1 gesehen                                   */
#define RE15_R42_BIT_TUER_11A         121  /* L1b K-02: Tuer 11A0/11A1 <-> 2070/2071 von der 11A-Seite geoeffnet                  */
#define RE15_R42_BIT_TUER_2040        122  /* L1b K-03: Tuer 2040/2041 <-> 2070/2071 aus dem Belohnungsraum geoeffnet             */
#define RE15_R42_BIT_3020_SCHUESSE    123  /* L5 Punkt 2: Leon hoert in 3020 die Schuesse (nach dem Rueckwechsel) - gesehen       */
#define RE15_R42_BIT_3060_UEBERGABE   124  /* L5-L7: Wiedersehen 3060 mit Uebergabe gesehen (oeffnet die Kartentuer 3010, L-10)  */

/* ---- Block A (v0.8.31): Finale --------------------------------------------------------------------------------------- */
#define RE15_R42_BIT_FINALE_ENDE      129  /* T-07: Finale beendet -> Abspann frei (eigenes Kennzeichen, Konzept Abschnitt 5)     */
/* Strang finale Stufe 3 (Plan S5/D3): Taken-Bit des geworfenen Raketenwerfers (Item_aot_set pc[18], RE2 Flag 0x28 @0x01216).
 * zensus_r42.py 2026-11-18: frei in Original/Port/Disc ("danach noch frei ... 137, 139, ..."); 23..51 sind voll (r42/gegenstaende
 * 40..51, Hangar 38/39), 170 nimmt r42/gegenstaende. 5090 und 5091 teilen das Bit (getrennte Spielstaende, VERTRAG 1.2 Regel). */
#define RE15_R42_BIT_WERFER_FINALE    137  /* L22/E-20 K-13: Raketenwerfer aus der Wurfszene 5090/5091 aufgehoben                 */

/* ---- Block C, Texte (v0.8.31) ---------------------------------------------------------------------------------------- */
#define RE15_R42_BIT_ZUG_ELZA         130  /* E9: Zugstart 6031 (Sherry startet) gesehen; Leons Gegenstueck = RE15_AN_ZUG_BIT 133 */

/* ---- Block C (v0.8.32, reserviert) ---------------------------------------------------------------------------------- */
#define RE15_R42_BIT_ALARM            131  /* L13/E4: Alarm + Durchsage 5060/5061 gelaufen (Ausbruch)                              */
#define RE15_R42_BIT_IMPFUNG          132  /* L13/E4: Begleiter geimpft (4010 Marvin / 5011 Sherry; getrennte Spielstaende)       */
#define RE15_R42_BIT_SCHUTZRAUM       23   /* L18/E5: Schutzraum-Szene 6020/6021 gesehen                                           */
#define RE15_R42_BIT_HANGAR           24   /* L19/E5: Hangar-Tor 5140/5141 geoeffnet                                               */
#define RE15_R42_BIT_IDKARTE          25   /* L14/E-11: Research Center ID Card genommen (50E0/50F1, Taken-Bit des neuen Items)    */
#define RE15_R42_BIT_ENZYM_DRIN       26   /* L16: Enzym in der Truhe 4090/4091                                                    */
#define RE15_R42_BIT_KARTE_OK         27   /* L16: ID-Karte am Tastenfeld 4090/4091 angenommen                                     */
#define RE15_R42_BIT_CODE_OK          28   /* L16/L17: Code eingegeben 4090/4091                                                   */
#define RE15_R42_BIT_PROBE            29   /* L16: Virusprobe (T/G) genommen 4090/4091 (Taken-Bit)                                 */
#define RE15_R42_BIT_PROBE_DRIN       30   /* L16: Probe im Synthesegeraet 5060/5061                                               */
#define RE15_R42_BIT_ZETTEL           31   /* L17: Laborzettel mit dem Code 40B0/40B1 genommen                                     */
#define RE15_R42_BIT_DOK_P4           32   /* E3: Dokument "P-4 Lab Incident Report" 5021 genommen                                 */
#define RE15_R42_BIT_DOK_VAKZIN       33   /* E3: Dokument "Vaccine Synthesis" 5011 erhalten                                       */
/* Enzym: das Disc-Bit selbst. Version_40%/PSX/STAGE4/ROOM40A1.RDT Datei @0x00ED0
 * `50 0a 09 31 00 00 48 9f 42 96 68 06 7a 03 3c 00 01 00 11 00 ff 00` = Item_aot_set Slot 10 Item 0x3C x1, Taken-Bit u16 pc[18]
 * = 0x0011 (Leser @0x80040680/@0x800406d8); Freigabe bei Licht @0x00FE0 `46 0a 09 31 3c 00 01 00 11 00`. Im Mod-Baum und im
 * Port frei (die Treffer "Bit 17" im Port-Zensus sind Bildbytes in gen/ventilkette_r41_plakat.inc, kein Flag-Zugriff). */
#define RE15_R42_BIT_ENZYM            17
/* Lageplaene (6b, F-18/F-25 nach Empfehlung): beim Bau in k_r36k_lage (kartenitems_r36) eintragen, NICHT in k_port[]. */
#define RE15_R42_BIT_PLAN_KANAL       34   /* Wandplan 2000/2001                                                                   */
#define RE15_R42_BIT_PLAN_FABRIK      35   /* Leons Besitz: Uebergabe 3060 (L6) oder selbst genommen in 30C0; Elza: Wandplan 30C1  */
/* Strang fabrikplan (20.11.2026, Konzept K-16): NUTZERWUNSCH woertlich "Im Story Zweig - room 3060 übergibt Marvin ein plan of the
 * plant. Das macht natürlich nur sinn wenn er ihn auch wirklich irgendwo aufnimmt...". Marvin nimmt den Wandplan in 30C0 (Zeile 30C0/
 * Leon in k_r36k_lage, begleit_bit) - das gehoert zu MARVINS Seite, nicht zu Leons Besitz (Bit 35): Marvins Karte zeigt Blatt 7,
 * Leons erst nach der Uebergabe 3060 (story3a_r42.c geben setzt 35 und loescht dieses Bit). PORT-WAHL aus dem Zensus
 * (zensus_r42.py 20.11.2026): frei im Original (Mod-Baum, bank9_zensus.py original), im Port (bank9_zensus.py port), auf der 40-%-Disc
 * und in keinem der 232 Zweige des Repos (git grep "#define RE15_*BIT* 139" je Zweig: kein Treffer). */
#define RE15_R42_BIT_PLAN_MARVIN      139  /* Marvin traegt den Fabrikplan (aus 30C0, noch nicht uebergeben)                       */
#define RE15_R42_BIT_PLAN_FORSCHUNG   36   /* Wandplan 4010/4011                                                                   */
#define RE15_R42_BIT_PLAN_UBAHN       37   /* Wandplan 6010/6011 (oder 6000/6001, misst der Bau; Ids dort ebenfalls frei)         */

/* ---- Nachrichten-Ids (je Raum; Regel und Zensus: VERTRAG.md Abschnitt 2) ----------------------------------------------
 * Stimme jeder Zeile: synchro/STAGE<n>/room<RAUM>/main<Id>.wav (audio_pc.c, VOICE_MAX_MSG 64). Jede neue Zeile wird mit
 * re15_msg_install_text UND re15_msg_install_durations(re15_msg_compute_duration(..)) abgelegt (Form story_r36.c ablegen /
 * kanister2080_r41.c:1190): fehlt die Aufnahme, laeuft die Zeile mit der Textdauer wie jede RDT-Zeile ohne Ton; liegt sie
 * da, wartet op_message_on auf die echte Restlaenge (scd_vm.c RE15_VOICE_NOTBREMSE). */
/* 2080/2081 (Konzept-Nummern 43..51, Anhang B; 40/41 frei seit Spur C Nachbesserung 3, 42 = Spur G Kanister) */
#define RE15_R42_MSG_2080_ERSTE       43   /* Leon 43..48, Elza 43..51 */
#define RE15_R42_MSG_2080_LETZTE_LEON 48
#define RE15_R42_MSG_2081_LETZTE_ELZA 51
#define RE15_R42_MSG_2080_LOCH        52   /* "Next to the computer... there's a keyhole." (K-04) */
#define RE15_R42_MSG_2080_TOR_ZU      53   /* "The gate is locked. It needs a key." */
#define RE15_R42_MSG_2080_TOR_AUF     54   /* "Leon used the key." / "Elza used the key." */
/* 2090/2091 (Spur F 42/43 belegt, 44..46 an F reserviert) */
#define RE15_R42_MSG_2090_UHR         47   /* K-08: "There's something else lying next to it..." */
/* 11A0/11A1 (RDT 0..2) */
#define RE15_R42_MSG_11A_ERSTE        3    /* Leon 3..6, Elza 3..10 (6..9 nur mit Uhr) */
#define RE15_R42_MSG_11A0_LETZTE      6
#define RE15_R42_MSG_11A1_LETZTE      10
#define RE15_R42_MSG_11A_AUFGESPERRT  11   /* L1b: "Leon/Elza unlocked the door." */
/* 2070/2071 (RDT 0, Spur C 40) und 2040/2041 (RDT 0..5) */
#define RE15_R42_MSG_2070_ANDERE_SEITE 1   /* L1b: "It's locked from the other side." (beide Tueren) */
#define RE15_R42_MSG_2040_AUFGESPERRT 6    /* K-03: "Leon/Elza unlocked the door." */
/* STAGE3 */
#define RE15_R42_MSG_3020_ERSTE       7    /* 7..9 (RDT 0..6) */
#define RE15_R42_MSG_3060_ERSTE       11   /* 11..27 (RDT 0..10) */
#define RE15_R42_MSG_3060_LETZTE      27
#define RE15_R42_MSG_3070_ADA         6    /* L10 (RDT 0..5) */
#define RE15_R42_MSG_30E0_LOBBY       16   /* L8 (RDT 0..15) */
#define RE15_R42_MSG_30E0_ADA_KOMMT   17   /* L10 optional */
#define RE15_R42_MSG_30E0_LEON_BEIN   3    /* L7: Neutext "He's back in the lobby. Some dogs got his leg..." (RDT 3 @0x1163) */
/* STAGE6 Zug (Block C Texte, v0.8.31) */
#define RE15_R42_MSG_6031_ERSTE       12   /* 12..17 (RDT 0..11); 6030 nur Neutexte vorhandener Ids 2..5, 21, 22 */
#define RE15_R42_MSG_6031_LETZTE      17

#endif /* RE15_STORY_R42_H */
