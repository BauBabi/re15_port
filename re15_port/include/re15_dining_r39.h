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

/* ---- Ressourcen (analysis/befunde_runde39/VERTRAG.md 1.1/1.2) ----------------------------------------------------- */
#define RE15_R39J_BIT_REAKTION   94     /* Bank 9: Elza hat auf den ersten Hilferuf reagiert (Punkt 5)                */
#define RE15_R39J_MSG_REAKTION   50     /* "Elza: Oh, no, this is John! / He is in Trouble, I have to help him!"     */
#define RE15_R39J_MSG_PUH        51     /* "John: Puuuh, that was close! ..."                                         */
#define RE15_R39J_MSG_FOLGEN     52     /* "John: Elza, follow me, ..."                                                */
#define RE15_R39J_HILFE_ID0      53     /* Sprach-Ids 53..57 = synchro/STAGE1/room1141/help01..05.wav (ohne Text)     */
#define RE15_R39J_HILFE_N        5

/* ---- Port-Bank 0x3E (Se_on pc[1]; frei: 0x0E/0x3A/0x3B/0x3C/0x3D sind vergeben, Dossier 3) ------------------------ */
#define RE15_R39J_SE_BANK        0x3E
#define RE15_R39J_SATZ_KLOPFEN   0x00   /* RE2 DOOR0F.DO2 Tonteil Satz 0 = Holzschlag (Treppe Holz), Archiv-Tabelle RE2
                                         * @0x8009A5D4 `88 2a cc 8b ..` Tonteil 0x2A88 B (gen/re2_tuer_tabelle.inc)        */
#define RE15_R39J_SATZ_GLAS      0x21   /* RE2 ROOM1090 sub15 @0x02BB0 `36 02 21 01 ..` = GLAS1090 Satz 0x21              */
#define RE15_R39J_SATZ_HILFE0    0x31   /* 0x31..0x35 -> Sprach-Id 53..57 (help01..05)                                    */

/* ---- Lagen (PORT-WAHL mit Beleg, Dossier 2.5) --------------------------------------------------------------------- */
#define RE15_R39J_PARK_X         (-30000)   /* John auf dem Dach = geparkt (ROOM11B0 main00 @0x01080 `d0 8a`)          */
#define RE15_R39J_PARK_Z         (-30000)
#define RE15_R39J_JOHN_TUER_X    (-7350)    /* John innen vor der Dachtuer (Gang x -8600..-5750 SCA, Nordwand z >= -250,
                                             * Tuer-Rechteck main00 @0x00A7A z -750..250); Cut 6 Fuss (170,188)          */
#define RE15_R39J_JOHN_TUER_Z    (-1300)
#define RE15_R39J_JOHN_TUER_YAW  1024       /* Blick -Z in den Gang (pos_advance @0x800245d8 z -= sin)                 */
#define RE15_R39J_ELZA_VORNE_X   (-7800)    /* Elza im Bild von Cut 7 (Blick nach Sueden): Fuss (208,234), Kopf (215,147)*/
#define RE15_R39J_ELZA_VORNE_Z   (-12800)
#define RE15_R39J_ELZA_NACH_X    (-6000)    /* Elza beim Gespraech nach dem Kampf, Bild von Cut 8: Fuss (217,232)       */
#define RE15_R39J_ELZA_NACH_Z    (-20000)
#define RE15_R39J_ECKE_X         (-7550)    /* Johns Platz im Kampf = sub02 @0x00F8E `40 00 05 01 82 e2 04 ac`         */
#define RE15_R39J_ECKE_Z         (-21500)

/* ---- Kameras (RID ROOM1141, Dossier 2.5) --------------------------------------------------------------------------- */
#define RE15_R39J_CUT_ELZA       7      /* (-7465,-3024,-9613) -> (-4265,2209,-27792): vom Gangende nach Sueden        */
#define RE15_R39J_CUT_TUER       6      /* (-8292,-838,-6606) -> (-6375,-3295,8211): Dachgang mit Dachtuer             */
#define RE15_R39J_CUT_GESPRAECH  8      /* (-2824,-1295,-18232) -> (-17127,-4112,-31804): Suedwest-Ecke               */

/* ---- Haken ------------------------------------------------------------------------------------------------------- */
/* Ende von re15_dining_r38_install (scd_room_setup.c Blockende + Boot/CONTINUE): Texte 50..52, Phase H: John parken. */
void re15_dining_r39_install(uint16_t room_id, int phase_h);
/* Phase-H-Programme statt HAEMMERN/BEFREIUNG der Runde 38 (dining_r38.c). */
const uint8_t *re15_dining_r39_haemmern(void);
const uint8_t *re15_dining_r39_befreiung(void);
/* Se_on-Zweig (dining_r38.c re15_dining_r38_se_on): 1 = Port-Bank 0x3E erledigt. */
int re15_dining_r39_se_on(unsigned bank, int satz);
/* audio_pc.c re15_voice_load_clip: ROOM1141 Id 53..57 -> Stamm "help01".."help05"; sonst unveraendert. */
void re15_dining_r39_stamm(unsigned room_id, int voice_id, char *stamm, size_t n);
/* Messschiene RE15_R39J_LOG=<datei> (env-gegatet, je Bild aus re15_dining_r38_messhaken). */
void re15_dining_r39_messhaken(void);
/* Plattform-Ton (platform/pc/src/dining_r39_pc.c): bank 0x3E, satz wie oben. */
extern void (*re15_dining_r39_se_hook)(int satz);

/* ---- Pruefhaken (Riegel) --------------------------------------------------------------------------------------- */
const uint8_t *re15_dining_r39_programm(int nr, int *out_len);   /* 0 HAEMMERN39 1 BEFREIUNG39 */
const uint8_t *re15_dining_r39_text(int nr, int *out_len);       /* 0 msg 50 1 msg 51 2 msg 52 */
const uint8_t *re15_dining_r39_klopfton(int *out_len);           /* DOOR0F-Tonteil (TORSE-Aufbau) */
extern unsigned g_re15_r39j_se_zaehler[3];   /* Se_on der Bank 0x3E: [0] Klopfen [1] Glas [2] Hilferuf */
extern int      g_re15_r39j_hilfe_letzte;    /* zuletzt gerufene Hilferuf-Nummer 1..5 (0 = keine)       */

#endif /* RE15_DINING_R39_H */
