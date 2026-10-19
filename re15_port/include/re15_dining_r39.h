/*
 * RE1.5 Rebuilt — Runde 39 Spur J "dining1141": Umbau der Phase H von ROOM1141 (Elza; baut auf Runde 38 Spur F auf,
 * include/re15_dining_r38.h). Dossier mit allen Messungen und Belegen: analysis/befunde_runde39/J_dining1141.md.
 *
 * NUTZER-VORGABEN (woertlich, analysis/befunde_runde39/AUFTRAG.md):
 *   1 "kommen komische Zombie Geräusche im Hintergrund, die will ich nicht. Es gibt ja keine Zombies."
 *   2 "Finde ich es doch besser, wenn John durch die Tür kommt, statt durch das Tor. [...] Kannst du nicht während des
 *      Tür öffnens die Kamera vorne auf Elza irgendwie machen, dann Tür schließen Sound und dann Kamera auf Tür, wo John
 *      bereits da ist?"
 *   3 "Du hast irgendeinen komischen shout voiceover für John eingebaut. Denn will ich nicht."
 *   4 "Ich möchte das du immer im Mix mit einigen Abständen die Voiceovers 1-5 stattdessen abspielst. Dazwischen möchte
 *      ich das du immer wieder ein mehrfachen Klopf gegen Tür Sound abspielst"
 *   5 Elza reagiert beim ersten Hoeren: "Oh, no, this is John! He is in Trouble, I have to help him!" (Nachtrag: John)
 *   6 "Vor Elzas "What was that?" Möchte ich das du einen Scheiben zerspringen Sound abspielst."
 *   7 "Bei Johns Text "It's some Kind of Gorilla...." schaut er ab da nicht mehr vernünftig zu Elza."
 *   8 "John: Puuuh, that was close! I thought we end up as Gorilla meal…." / "John: Elza, follow me, I think I might
 *      have found a way out of here!" — "Alles natürlich mit passenden Animationen."
 *   9 "Wenn man ROOM 1141 danach verlässt zu room 1171 kommt am anfang noch kurz die Panik Musik." (audio_pc.c-Haken)
 *
 * KEIN ASSET-PATCH: Programme = ausgelieferte Original-Bytes + eingefuegte Zeilen in Original-Opcode-Form
 * (engine/src/gen/dining_r39_programme.inc, Werkzeug tools/r39_dining/programme_bauen.py mit Listing je Zeile).
 */
#ifndef RE15_DINING_R39_H
#define RE15_DINING_R39_H

#include <stdint.h>
#include <stddef.h>
#include "re15_actor.h"   /* re15_actor_t (Nachbesserung 2: re15_dining_r39_trifft) */

/* ---- Ressourcen (analysis/befunde_runde39/VERTRAG.md 1.1/1.2) ----------------------------------------------------- */
#define RE15_R39J_BIT_REAKTION   94     /* Bank 9: Elza hat auf den ersten Hilferuf reagiert (Punkt 5). Seit Nutzer-Nachtrag
                                         * 10 nicht mehr gesetzt (Reaktion in 1141 entfaellt); bleibt reserviert (k_port[])  */
#define RE15_R39J_MSG_REAKTION   50     /* "Elza: Oh, no, this is John! / He is in trouble, I have to help him!" - seit
                                         * Nutzer-Nachtrag 10 nicht mehr in ROOM1141 (die Zeile kommt in ROOM1041, Spur P) */
#define RE15_R39J_MSG_PUH        51     /* "John: Puuuh, that was close! ..."                                         */
#define RE15_R39J_MSG_FOLGEN     52     /* "John: Elza, follow me, ..."                                                */
#define RE15_R39J_HILFE_ID0      53     /* Sprach-Ids 53..57 = synchro/STAGE1/room1141/help01..05.wav (ohne Text)     */
#define RE15_R39J_HILFE_N        5
#define RE15_R39J_MSG_STIMME_WARTEN 58  /* Port-Zeile ohne Text: op_message_on parkt, solange eine Sprachzeile laeuft
                                         * (scd_vm.c Stimmen-Riegel), dann verbraucht der Haken sie (Punkt 6)       */

/* ---- Nachbesserung 1 (J_dining1141.md Abschnitt 8) ------------------------------------------------------------- */
#define RE15_R39J_MERKER_BANK    5      /* (5,0x10) = Gemaelde-Programm ROLLTOR39 laeuft: dining_r38.c setzt ihn SYNCHRON
                                         * beim Ausloesen von Ereignis 6, ROLLTOR39 loescht ihn am Ende; Elzas Reaktion
                                         * wartet darauf (M1). Bank 5 = Skript-Merker (ROOM1141 sub03/sub04 @0x00FA4/
                                         * @0x00FB6 nutzen nur Bit 0/1)                                                  */
#define RE15_R39J_MERKER_BIT     0x10
#define RE15_R39J_MSG52_REST     40     /* nach Sleep 60 (ROOM1171 @0x014D2): 60 + 40 = 100 >= msg 52 (99 Bilder, Abnahme H3)
                                         * und >= Clip 96 Bilder - John laeuft erst danach los                            */
/* M2/N4.4 + Nutzer-Nachtrag 7 ("die soll so aussehen wie in room 1171"): Johns Heranwinken = ROOM1171 sub02 @0x014CE
 * `3f 00 02 00` Plc_motion(0,2), Clip 2 des RBJ-Records 1 (Marker 0x2 = Gegner 0 = John) von ROOM1171 (RBJ = RDT+0x5C
 * @0x188C, Lader FUN_8001b3f8 `lw a2,92(v0)` @0x8001b404; Record 1 EDD @+0x3E34, 3 Clips 20/30/96). ROOM1141 hat diesen
 * Clip nicht - er wird fuer die Geste geliehen (Muster Runde 35 Spur K / Runde 39 Spur I: RBJ-Leihe aus einer anderen
 * RDT, platform/pc/src/cut10f0_pc.c).                                                                                  */
#define RE15_R39J_WINK_RAUM      0x1171u
#define RE15_R39J_WINK_REC       1      /* Record-Marker 0x2 = Bit (1 + Gegner 0) = John (Binder @0x8001b480-90)         */
#define RE15_R39J_WINK_CLIP      2
#define RE15_R39J_WINK_BILDER    96     /* Bildzahl des Clips (EDD Record 1 @+0x3E34; Riegel prueft)                     */
#define RE15_R39J_EREIGNIS_RIEGEL 5     /* main00 @0x00A9A `2c 02 03 31 .. 18 05` -> sub05 = Dachtuer entriegeln (N4.2) */
#define RE15_R39J_W_SUB          2      /* Plc_motion pc[1] = 2 auf dem Spieler = Sub 0x80073e30[2] = 0x80050f00 auf dem
                                         * Kanal +0x170/+0x174 = W-Bank (@0x80036be4/@0x80036c04); Form ROOM3060 sub03
                                         * @0x010BE `3f 02 06 00`                                                        */
#define RE15_R39J_W_CLIP_HEBEN   6      /* W-Bank Clip 6 = RAISE (Gun-FSM sub0 / Melee @0x80034ee8)                       */
#define RE15_R39J_W_CLIP_MESSER  0x0D   /* Messer noch nicht in der Hand: DRAW (Melee sub4 @0x80035538, Clip 0xD)         */
/* N4.3 (NUTZER-VORGABE Nachtrag 4 "John sollte auch ab und an mal auf die Gorillas schiessen ... und dann ihre
 * Aufmerksamkeit erhalten"; Johns Zeile msg 11 "I'll try to distract them"): Faden SCHUETZE39 in Original-Form der Schuesse
 * auf Roy (ROOM1021 sub02 @0x020C6..@0x020F2). Treffer/Ton/Aufmerksamkeit am Anker FEUER (dining_r39.c).              */
#define RE15_R39J_SUB_SCHUETZE   30     /* Evt_exec-Nummer (< RE15_RDT_MAX_SUB_SCD 32; ROOM1141 hat sub00..sub06)          */
#define RE15_R39J_SCHUETZE_BANK  5      /* (5,0x11) = Salve laeuft; ENTRIEGELN39 wartet nach dem Kampf darauf              */
#define RE15_R39J_SCHUETZE_BIT   0x11
#define RE15_R39J_WAFFE_JOHN     3      /* Schaden = Spalte 3 (Handfeuerwaffe) der Zeile @0x8006e0d0 + Typ*0x58: Gorilla
                                         * @0x8006ee44 = 12 (Runde 39 Spur E)                                             */
#define RE15_R39J_TREFFER_KEGEL  0xC8   /* Treffer, wenn der Gorilla im Kegel +-0xC8 vor John steht = Auto-Zielfenster der
                                         * Pistole (Gun-FSM sub0 `ori a1,zero,0xc8` @0x80032fe0, FUN_8001a8f8) - PORT-WAHL */
#define RE15_R39J_AUFMERKSAM_BILDER 180 /* so lange kriecht der getroffene Gorilla auf John zu = eine Salvenpause (Sleep 90
                                         * + 90, Takt RE2 ROOM1060 sub05 +0x58/+0x5E) - PORT-WAHL "Aufmerksamkeit"           */
#define RE15_R39J_AUFMERKSAM_NAH 2000   /* naeher an John: Aufmerksamkeit vorbei = Gorilla-Kasten 1600 (@0x80121356, Runde 36 F)
                                         * + 400 Abstand, damit er John nicht schiebt - PORT-WAHL                     */
#define RE15_R39J_AUFMERKSAM_TAKT 30    /* Prueftakt der Jagd: alle 30 Bilder ...                                         */
#define RE15_R39J_AUFMERKSAM_SCHRITT 300 /* ... mindestens 300 naeher an John, sonst vorbei (festgeklemmt) - PORT-WAHL. Der
                                          * Takt zaehlt erst, wenn der Gorilla John zugewandt ist (Nachbesserung 2, unten)  */
/* ---- Nachbesserung 2 (Abnahme 1 Mangel M1, J_dining1141.md Abschnitt 9) ------------------------------------------ */
#define RE15_R39J_NACHFUEHREN    0xC8   /* (b) Waehrend des Zielens dreht John die Waffe dem Ziel nach, je Bild hoechstens 0xC8
                                         * = Form des Nachzielens der Pistole: Gun-FSM Sub 5 (Wechsel aufs naechste Ziel,
                                         * @0x80033eec) Phase >= 1 `ori a1,zero,0xc8` @0x80033f90 / `jal 0x8001a8f8` @0x80033f94
                                         * (a0 = Ziel+0x34 @0x80033f98), je Bild bis zur Ausrichtung (arc_test @0x80033fcc)   */
#define RE15_R39J_ZUGEWANDT      0x180  /* (c) Gorilla "John zugewandt" = Kegel +-0x180 des Gorilla-Hirns (A[3] Biss-Kegel
                                         * a804(0xbb8,0x180) @0x80117a60-74). Erst dann zaehlt der Fortschritts-Takt oben;
                                         * bis dahin dreht er sich mit dem B[3]-Lenkschritt 6..37 (@0x80117d1c-34) zu John   */
#define RE15_R39J_CUT_ENDE       0      /* Kamera nach Johns Abgang: RVD ROOM1141 (RDT+0x28 -> @0x01A8) Satz 0 `01 ff 00 00 fc d6
                                         * 64 9c ..` = Gruppe Cut 0 mit dem Viereck x -10500..3000 z -25500..-8500 um Elzas
                                         * Gespraechslage; die Gruppe Cut 8 hat nur ihren Kopf (Satz 22 `01 ff 08 00`), aus
                                         * Cut 8 fuehrt kein Wechsel. Form ROOM1141 sub02 @0x00E80 `29 00` vor @0x00F56
                                         * Cut_auto 1 (vorher Cut_old = zurueck auf Cut 8, die Kamera blieb stehen)          */

/* ---- Port-Bank 0x3E (Se_on pc[1]; frei: 0x0E/0x3A/0x3B/0x3C/0x3D sind vergeben, Dossier 3) ------------------------ */
#define RE15_R39J_SE_BANK        0x3E
#define RE15_R39J_SATZ_KLOPFEN   0x00   /* RE2 DOOR0F.DO2 Tonteil Satz 0 = Holzschlag (Treppe Holz), Archiv-Tabelle RE2
                                         * @0x8009A5D4 `88 2a cc 8b ..` Tonteil 0x2A88 B (gen/re2_tuer_tabelle.inc)        */
#define RE15_R39J_SATZ_GLAS      0x21   /* RE2 ROOM1090 sub15 @0x02BB0 `36 02 21 01 ..` = GLAS1090 Satz 0x21              */
#define RE15_R39J_SATZ_HILFE0    0x31   /* 0x31..0x35 -> Sprach-Id 53..57 (help01..05)                                    */
#define RE15_R39J_SATZ_TUERTON   0x36   /* Nachtrag 17 Punkt 5: synchro/STAGE1/room1141/door_sound.wav (Nutzer)          */
#define RE15_R39J_TUERTON_ID     59     /* Sprach-Id (1141: 40-42, 50-58 vergeben); Stamm "door_sound"                  */

/* ---- Lagen (PORT-WAHL mit Beleg, Dossier 2.5) --------------------------------------------------------------------- */
#define RE15_R39J_PARK_X         (-30000)   /* John auf dem Dach = geparkt (ROOM11B0 main00 @0x01080 `d0 8a`)          */
#define RE15_R39J_PARK_Z         (-30000)
#define RE15_R39J_JOHN_TUER_X    (-7350)    /* John innen vor der Dachtuer (Gang x -8600..-5750 SCA, Nordwand z >= -250,
                                             * Tuer-Rechteck main00 @0x00A7A z -750..250); Cut 6 Fuss (170,188)          */
#define RE15_R39J_JOHN_TUER_Z    (-1300)
#define RE15_R39J_JOHN_TUER_YAW  1024       /* Blick -Z in den Gang (pos_advance @0x800245d8 z -= sin)                 */
#define RE15_R39J_ELZA_VORNE_X   (-7800)    /* Elza im Bild von Cut 7 (Blick nach Sueden): Fuss (208,234), Kopf (215,147)*/
#define RE15_R39J_ELZA_VORNE_Z   (-12800)
#define RE15_R39J_ELZA_CUT7_Z    (-9775)    /* = RE15_R38F_CUT7_AUS_Z: suedlich davon ist Elza im Bild von Cut 7; dort schneidet
                                             * ENTRIEGELN39 auf Cut 7 (statt 3 Bilder Cut 0 an der Zonengrenze, Lauf n1)  */
#define RE15_R39J_ELZA_NACH_X    (-5800)    /* Elza beim Gespraech nach dem Kampf, 1750 oestlich von John: Bild von Cut 8
                                             * Elza (145,130..215) / John (194,135..200) nebeneinander, Elza ausserhalb
                                             * von Johns Bahn (BAHN_X_MAX -6450) und nicht vor ihm (Messung 4.2)        */
#define RE15_R39J_ELZA_NACH_Z    (-21500)
#define RE15_R39J_ECKE_X         (-7550)    /* Johns Platz im Kampf = sub02 @0x00F8E `40 00 05 01 82 e2 04 ac`         */
#define RE15_R39J_ECKE_Z         (-21500)

/* ---- Kameras (RID ROOM1141, Dossier 2.5) --------------------------------------------------------------------------- */
#define RE15_R39J_CUT_ELZA       7      /* (-7465,-3024,-9613) -> (-4265,2209,-27792): vom Gangende nach Sueden        */
#define RE15_R39J_CUT_TUER       6      /* (-8292,-838,-6606) -> (-6375,-3295,8211): Dachgang mit Dachtuer             */
#define RE15_R39J_CUT_GESPRAECH  8      /* (-2824,-1295,-18232) -> (-17127,-4112,-31804): Suedwest-Ecke               */

/* ---- Haken ------------------------------------------------------------------------------------------------------- */
/* Ende von re15_dining_r38_install (scd_room_setup.c Blockende + Boot/CONTINUE): Texte 50..52, Phase H: John parken. */
void re15_dining_r39_install(uint16_t room_id, int phase_h);
/* Phase-H-Programme statt HAEMMERN/BEFREIUNG der Runde 38 (dining_r38.c): Klopf-Schleife (Evt_exec sub02), Gemaelde
 * (Ereignis 6: nur Rolladen, danach frei laufen) und Riegel der Dachtuer (Ereignis 5: die Szene mit John). */
const uint8_t *re15_dining_r39_haemmern(void);
const uint8_t *re15_dining_r39_rolltor(void);
const uint8_t *re15_dining_r39_entriegeln(void);
/* Beim Ausloesen des Riegels (Ereignis 5): die Waffe, die Elza hebt (NUTZER-VORGABE Nachtrag 4: Schusswaffe aus dem
 * Inventar - die ausgeruestete, sonst die erste im Inventar, die dann ausgeruestet wird -, ohne Schusswaffe das Messer).
 * Rueckgabe = Waffen-Id (aca5d). */
int re15_dining_r39_waffe_waehlen(void);
/* Spieler-Plc_motion(2,c) in ROOM1141 (scd_vm.c op_plc_motion, Haken): W-Bank-Clip c einmal spielen und halten
 * (0x80050f00 auf +0x170/+0x174). _tick je Bild (game_step_common.c, Haken), _clip fuer den Zeichner (main.c, Haken):
 * -1 = aus. Endet, sobald ein anderer Plc-Befehl Clip oder Kommandowort des Spielers aendert. */
void re15_dining_r39_w_beginn(int entity, int clip);
void re15_dining_r39_tick(void);   /* je Bild: W-Clip halten, Schuss (Verzug 1), Aufmerksamkeit */
/* Nachbesserung 3 (Abnahme 2 M1): 1 = in ROOM1141 Phase H stand im letzten Spielschritt die Pad-Sperre (2,7) (Pausen-Wort
 * Bit 0x01000000, Set-Tabelle @0x80074664[2]). Dann laeuft die Ziel-FSM des Spielers mit gekapptem Pad weiter wie im Original
 * (FUN_80030444 `andi v0,v0,0xf000` @0x80030514 / `sw 0x800ac768` @0x8003051c; Dispatcher haelt nur bei Bit 31 @0x80031c78)
 * und senkt die Waffe (HOLD @0x800331e4-@0x80033200, Messer @0x80035128-@0x80035144, Rueckstoss @0x80033600/@0x8003364c).
 * Haken in player_common.c re15_player_tick; der Stand gilt ab dem Bild NACH dem Setzen (Umsetzer vor der Spiel-Task). */
int  re15_dining_r39_ziel_senken(void);
int  re15_dining_r39_w_clip(void);
/* N4.3: Faden SCHUETZE39 (Evt_exec sub 30 in ROOM1141, Haken dining_r38.c), Anker im VM-Takt (scd_vm.c, Haken),
 * Lenkziel eines Gorillas (enemy_ai_common.c Gorilla B[3], Haken): John statt Elza, solange er Johns Treffer
 * "beachtet". g_re15_r39j_anker_n != 0, solange der Faden in diesem Raum liegt (Wachposten des VM-Hakens). */
const uint8_t *re15_dining_r39_schuetze(void);
void re15_dining_r39_pc(const uint8_t *pc);
/* Lenkziel: 1 = John ersetzt das Ziel (dann OHNE Umweg-/Pad-Suche, die gilt Elza), 0 = unveraendert. */
int  re15_dining_r39_affe_ziel(int slot, int32_t *x, int32_t *z);
int  re15_dining_r39_affe_jagt_john(int slot);   /* 1 = Gorilla im Hirn-Kopf auf Jagd B[3] zu John */
int  re15_dining_r39_ziel_slot(void);            /* Johns aktuelles Ziel (Riegel/Messschiene), -1 = keins */
int  re15_dining_r39_zielt(void);                /* 1 = John fuehrt die Waffe gerade nach (Anker ZIELEN..SENKEN) */
/* Trefferbedingung von Johns Schuss (Riegel): 0 = Treffer, 1 kein Ziel, 2 tot, 3 im Angriff (Greifer/Biss/Sprung), 4 nicht im
 * Kegel RE15_R39J_TREFFER_KEGEL. Elzas Opferzustand spielt keine Rolle (Nachbesserung 2 (a)). */
int  re15_dining_r39_trifft(const re15_actor_t *john, const re15_actor_t *gorilla);
extern int      g_re15_r39j_anker_n;
extern unsigned g_re15_r39j_schuss_zaehler, g_re15_r39j_treffer_zaehler;
int  re15_dining_r39_aufmerksam(int slot);   /* Restbilder (Riegel) */
/* Heranwinken-Leihe: Plattform liefert den RBJ-Block von ROOM1171 (dining_r39_pc.c), die Engine parst Record 1 gegen
 * Johns Skelett und gibt ihn nur waehrend der Geste (John Zustand 4, Sub 0, Clip 2) als RBJ-Kanal heraus (Haken in
 * enemy_common.c re15_actor_rbj_anim/_skel). */
extern const uint8_t *(*re15_dining_r39_rbj_hook)(int *size);
const void *re15_dining_r39_rbj_anim(int slot);   /* const re15_emd_animation_t * oder NULL */
const void *re15_dining_r39_rbj_skel(int slot);   /* const re15_emd_skeleton_t *  oder NULL */
int  re15_dining_r39_leihe_aktiv(void);
int  re15_dining_r39_off_wink(void);   /* Anker WINK in ENTRIEGELN39 (Riegel) */
int  re15_dining_r39_schuetze_anker(int k);   /* Anker in SCHUETZE39: 0 ZIEL 1 ZIELEN 2 FEUER1 3 FEUER2 4 SENKEN (Riegel) */
/* Se_on-Zweig (dining_r38.c re15_dining_r38_se_on): 1 = Port-Bank 0x3E erledigt. */
int re15_dining_r39_se_on(unsigned bank, int satz);
/* audio_pc.c re15_voice_load_clip: ROOM1141 Id 53..57 -> Stamm "help01".."help05"; sonst unveraendert. */
void re15_dining_r39_stamm(unsigned room_id, int voice_id, char *stamm, size_t n);
/* op_message_on (scd_vm.c, nach dem Stimmen-Riegel): 1 = (ROOM1141, msg 58) verbraucht - eine reine Warte-Zeile. */
int re15_dining_r39_message_on(const uint8_t *pc);
/* Messschiene RE15_R39J_LOG=<datei> (env-gegatet, je Bild aus re15_dining_r38_messhaken). */
void re15_dining_r39_messhaken(void);
/* Plattform-Ton (platform/pc/src/dining_r39_pc.c): bank 0x3E, satz wie oben. */
extern void (*re15_dining_r39_se_hook)(int satz);

/* ---- Pruefhaken (Riegel) --------------------------------------------------------------------------------------- */
const uint8_t *re15_dining_r39_programm(int nr, int *out_len);   /* 0 HAEMMERN39 1 ENTRIEGELN39 2 ROLLTOR39 3 SCHUETZE39 */
const uint8_t *re15_dining_r39_text(int nr, int *out_len);       /* 0 msg 50 1 msg 51 2 msg 52 */
const uint8_t *re15_dining_r39_klopfton(int *out_len);           /* DOOR0F-Tonteil (TORSE-Aufbau) */
extern unsigned g_re15_r39j_se_zaehler[3];   /* Se_on der Bank 0x3E: [0] Klopfen [1] Glas [2] Hilferuf */
extern int      g_re15_r39j_hilfe_letzte;    /* zuletzt gerufene Hilferuf-Nummer 1..5 (0 = keine)       */

#endif /* RE15_DINING_R39_H */
