/*
 * RE1.5 Rebuilt — Runde 42 "story" (v0.8.31), Stufe STAGE2: Schluesselloch + Nachgespraech 2080/2081, Tueren "von der anderen
 * Seite" 2070/11A0/2040, Taschenuhr neben dem Schluessel 2090/2091. Code: engine/src/story2_r42.c, Texte:
 * tools/r42_story/texte_stage2.py -> engine/src/gen/story2_r42_texte.inc.
 *
 * PORT-STORY (Konzept C:/workspace/git/reAi_v2_entscheidung_2026-11-06/11_story_v0831/KONZEPT.md, vom Nutzer am 08.11.
 * freigegeben; Korrekturen K-01..K-08 Abschnitt 0 bindend): 3.2 L1 + K-01 (Hinweis beim Ablassen, Schluessel in 2080, Szene
 * automatisch), L1b + K-02/K-03 (finale Korridortuer und Tuer zum Belohnungsraum "locked from the other side"), 4.2 E1 + K-07 +
 * F-03 (Elza/John/Sherry, John bleibt mit Sherry zurueck), 6b + K-08 + F-19 (Uhr neben dem Schluessel, Hinweis, Gravur bei Leon).
 * Bits und Nachrichten-Ids NUR aus dem VERTRAG analysis/befunde_runde42/VERTRAG.md (Werte in re15_story_r42.h).
 *
 * ---- MESSUNGEN (Belege, scd_dump_room.py auf shared_assets/PSX; 2081/2091/2071/2041/11A1 bytegleich bzw. main00 gleich) ------
 * K-01 "in 2080 das Tor oeffnen": ROOM2080 hat KEINEN Tor-Satz - main00 Door_aot_set Slot 0 @0x0BA2 -> 2030, Slot 1 @0x0BC2
 *   Leiter -> 2090. Das einzige Tor = ROOM2090 Slot 1 @0x009B6 -> 20A0 (Runde 41 Nachtrag 21, Bit 113). 2080 ist die obere Ebene
 *   derselben Halle (kanister2080: x/z geteilt, RVD @0x188 = 2090 @0x220) - Cut 7 RID @0x140 blickt vom Rechner in das Becken.
 *   -> Das Schluesselloch "next to the computer" (Hinweistext K-04) oeffnet das Tor 2090<->20A0 (Bit 113); L-02 "danach zurueck
 *   an den Fuss der Leiter in 2090". Der Tor-Handler in 2090 nimmt den Schluessel seitdem nicht mehr (gatorszene_r41.c tor_druck).
 * Rechner: ROOM2080 sub00 @0x0C1A `2c 04 03 31 08 00 ee d0 ae e3 e8 03 e8 03 ff 00 18 02 00 00` = Aot_set Slot 4 sce 3, Band 8,
 *   Rechteck x -12050..-11050 z -7250..-6250, Ereignis 2; nur solange Ck(3,245)=0 @0x0C02 (danach Else-Zweig @0x0C5A ohne Slot 4).
 *   sub02 @0x0C86..0x0CF2 = Ablassen: Message_on 0 `2b 00 ff ff` @0x0C86, Ifel_ck `06 00 60 00` @0x0C8C (Ja-Zweig Ck(12,31)=0),
 *   Set(3,245) @0x0C94, ..., Work_set(1,0) @0x0CE8 + Plc_ret @0x0CEC, Endif `08 00` @0x0CEE, Evt_end @0x0CF0.
 *   Spieler am Rechner: (-11160,-7316) Blick +Z, Kamera Cut 3 (Lauf e2, WEGPLAN ab der 2030-Tuer, state.log cam=3); Cut 3 = RID
 *   @0x0C0 Auge (-8446,-18889,-16448) -> Ziel (-11806,-16085,-11908).
 *   Gang zum Rechner (SCA ROOM2080 @0x4BC, Band 8): Westwand @0x558 x -14300..-12400 (z -13050..-6750), Ostwand @0x570 x
 *   -9200..-8200 (z -16550..-4750), Nordwand @0x564 z -6750..-4750; Kreise @0x5F4 x -13300..-12100 z -10600..-9400 und @0x600
 *   x -12800..-11600 z -8100..-6900 (Stuhl). Ostwand @0x57C x -9200..-8200 z -28900..-18600 (Gang nach Sueden zur Leiter).
 * K-02/K-03 (Korridor ROOM2070/2071 Nordgang, analysis/befunde_runde41/C_ventilkette.md: SCA ROOM2070 @0x714 drei Gaenge):
 *   2070 main00 Ifel_ck Ck(3,151)=0 @0x0ABE: Aot_set Slot 7 sce 3 @0x0AC2 -> sub02 @0x0CDA (Set(3,151) @0x0CE4, msg 0 "You've
 *   opened the lock." @0x0CE8, Aot_reset Slot 4 Tuer @0x0CEE) - der Riegel sitzt im ORIGINAL auf der 2070-Seite.
 *   11A0 main00 Ifel_ck Ck(3,151)=0 @0x00F3E: Aot_set Slot 0 msg 2 "It's locked from the other side." @0x00F42, sonst Door_aot_set
 *   Slot 0 @0x00F5A -> 2070 (-18650,0,-19300) Cut 12.
 *   2070 Slot 5 @0x0B1C -> 2040 und 2040 Slot 4 @0x0131C (Galerie, Band 5) -> 2070: ohne Schloss. 2040 = Belohnungsraum hinter der
 *   Rost-Tuer (Zeitbombe (3,53) @0x01624).
 *   UMKEHR (K-02/K-03): auf der Korridorseite "It's locked from the other side." (Text = 11A0 msg 2 bytegleich), auf der anderen
 *   Seite schliesst der Druck auf (Form 2070 sub02: Text + Riegel, die Tuer bleibt bei diesem Druck zu, danach Abkuerzung).
 * K-08 (Uhr): ROOM2090 main00 @0x00A12 Item_aot_set Slot 5 Item 0x3F x1 Taken-Bit 90, Rechteck (5350,-16700) 1000 x 1000 Band 1,
 *   Modell obj 0 @0x00A28 (5750,-1800,-16300). Heute (2090 und 2091 bytegleich) liegt sie unauffaellig am Tor, ohne Hinweis.
 */
#ifndef RE15_STORY2_R42_H
#define RE15_STORY2_R42_H

#include <stdint.h>
#include "re15_story_r42.h"

/* ---- Raeume (Raum-Basis & 0xFFFE; Elza = +1, @0x800397ec Elza-Variante) ----------------------------------------------- */
#define RE15_S2_RAUM_2080        0x2080u
#define RE15_S2_RAUM_2090        0x2090u
#define RE15_S2_RAUM_2070        0x2070u
#define RE15_S2_RAUM_2040        0x2040u
#define RE15_S2_RAUM_11A0        0x11A0u

/* ---- Port-Ereignis (scd_event_fire) ------------------------------------------------------------------------------------
 * 30: Zensus der RE15_*_EREIGNIS in include/ - 30 nur in 1041/1061/1031 (elzazombies_r39) und 1121 (hund1121_r39); in
 * 2080 (Sub-Tabelle @0x0BF8 `06 00` = 3 Subs), 2070 (@0x0CD0, 3 Subs), 2040 (@0x1404, 9 Subs), 11A0 (@0x1178, 6 Subs) frei;
 * < RE15_RDT_MAX_SUB_SCD 32. */
#define RE15_S2_EREIGNIS         30
#define RE15_S2_SAT              0x31  /* Aot_set pc[3] aller hier umgewidmeten Saetze: @0x0C1D / @0x0AC5 / @0x00F45 `31`    */

/* ---- 2080: Rechner / Schluesselloch ---------------------------------------------------------------------------------- */
#define RE15_S2_RECHNER_OFF      0x0C1A   /* sub00 Aot_set Slot 4 (20 B), Bytes oben                                       */
#define RE15_S2_RECHNER_SLOT     4
#define RE15_S2_RECHNER_EREIGNIS 2        /* Nutzlast `ff 00 18 02` -> sub02                                                */
#define RE15_S2_ABLASS_OFF       0x0C86   /* sub02 Anfang                                                                   */
#define RE15_S2_ABLASS_IFEL      0x0C8C   /* Ifel_ck `06 00 60 00`: Blocklaenge 0x60 ab @0x0C90                              */
#define RE15_S2_ABLASS_ENDIF     0x0CEE   /* Endif `08 00`                                                                  */
#define RE15_S2_ABLASS_ENDE      0x0CF2   /* nach Evt_end `01 00` @0x0CF0                                                    */
#define RE15_S2_WASSER_BANK      3
#define RE15_S2_WASSER_BIT       245      /* Ck(3,245) @0x0C02 / Set @0x0C94                                                */
#define RE15_S2_KEY_ITEM         0x34     /* Water Key = RE15_GSZ_KEY_ITEM (Runde 41 Spur F)                                */
#define RE15_S2_TOR_BIT          113      /* = RE15_GSZ_BIT_TOR: Tor 2090 <-> 20A0 aufgeschlossen                            */
#define RE15_S2_GATOR_TOT_BIT    111      /* = RE15_GSZ_BIT_LIEGT                                                            */
#define RE15_S2_KEY_GENOMMEN_BIT 112      /* = RE15_GSZ_BIT_GENOMMEN                                                         */

/* ---- Szene 2080/2081 (PORT-WAHL der Inszenierung, gemessen am Gang oben; Lauf e2/e3 der Stufe) ------------------------- */
#define RE15_S2_CUT_SZENE        3        /* RID @0x0C0: blickt vom Treppenabsatz den Gang hinauf auf den Rechner           */
#define RE15_S2_Y_OBEN           (-14400) /* Band 8 = -8 * 1800 (Sce_em_set +0x1ba @0x800421d4, RE15_K2080_Y)               */
#define RE15_S2_BAND_OBEN        8
/* Spieler am Schluesselloch (Lauf e2 "angekommen" (-11160,-7316), Bildpunkt x 233 in Cut 3 laut [leon-root]). Der Begleiter
 * kommt vom Sueden (von der Leiter, wo er den Kanister geworfen hat) durch den Gang (x -12400..-9200) und bleibt vor dem Spieler
 * stehen. Start hinter der Bildebene von Cut 3 (Auge (-8446,-16448), Blick (-3360,4540): (p - Auge) . Blick < 0). Lagen im Bild
 * gemessen (Projektion mit dem Cut-3-Auge und H 208 = sd des Logs; Laeufe t1/t1e): bei Bildpunkt 243 verdeckte der Ostrahmen im
 * Vordergrund den NPC (Lauf t1, (-10400,-9000)), Sherry hinter John (Lauf t1e, (-11700,-8700) Bildpunkt 211 hinter John 222).
 * Jetzt: Spieler 236 (Tiefe 8955), John/Marvin (-10750,-9600) 227 (6875), Sherry (-11800,-9500) 199 (7580). Abstaende: Kreise
 * @0x5F4 (Mitte (-12700,-10000) r 600) 1030 und @0x600 ((-12200,-7500) r 600) 2040, John-Sherry 1055 (> zwei Koerperradien).
 * Sherry laeuft ueber einen Zwischenpunkt (-11300,-11000), damit ihr Weg am Kreis @0x5F4 >= 1177 vorbeifuehrt. */
#define RE15_S2_NPC_START_X      (-10700)
#define RE15_S2_NPC_START_Z      (-20000)
#define RE15_S2_NPC_X            (-10750)
#define RE15_S2_NPC_Z            (-9600)
#define RE15_S2_SHERRY_START_X   (-11300)
#define RE15_S2_SHERRY_START_Z   (-20000)
#define RE15_S2_SHERRY_ZW_X      (-11300)
#define RE15_S2_SHERRY_ZW_Z      (-11000)
#define RE15_S2_SHERRY_X         (-11800)
#define RE15_S2_SHERRY_Z         (-9500)
#define RE15_S2_NPC_SLOT         8        /* Skript-Slot wie RE15_K2080_NPC_SLOT (Aktor 9)                                  */
#define RE15_S2_SHERRY_SLOT      9
#define RE15_S2_TYP_MARVIN       0x40     /* ROOM2000 sub @0x017B4 `44 01 40 40 ..`                                         */
#define RE15_S2_TYP_JOHN         0x49     /* ROOM2001 @0x0179E `44 00 49 40 ..`                                             */
#define RE15_S2_TYP_SHERRY       0x4B     /* ROOM2001 @0x0180A `44 00 4b 40 ..`                                             */
#define RE15_S2_NPC_GRID         0x40     /* stehender NPC (ROOM2000/2001 pc[3] = 0x40)                                     */
/* Signale Bank 5 (Raum-Scratch, geloescht beim Raum-Init FUN_8003ecec @0x8003ed74; ROOM2080/2081-Skripte ohne Bank 5, Zensus
 * kanister2080 Kopf; Kanister-Szene 0x17..0x1F nur in ihrer Phase vor dem Tod des Alligators). */
#define RE15_S2_SIG_FREI         0x14     /* Takt -> Programm: keine Zeile, keine Stimme mehr (Form RE15_K2080_SIG_WSLEEP)   */
#define RE15_S2_SIG_TUER         0x15     /* Programm -> Takt: Tuer-Ton 2030 (DOOR06 Satz 1, re15_k2080_tuerton_hook)       */
#define RE15_S2_AN_SPIELER       0x20     /* Plc_dest-Ankunftsbit Spieler (Form ROOM2040 sub02 @0x01648 `40 00 09 20`)      */
#define RE15_S2_AN_NPC           0x21     /* Plc_dest-Ankunftsbit NPC (Form ROOM6030 sub08 @0x01160 `40 00 09 21`)          */
#define RE15_S2_AN_SHERRY        0x22     /* Plc_dest-Ankunftsbit Sherry (Form ROOM1021 sub02 @0x020B2 `40 00 04 22`)       */

/* ---- Tueren K-02/K-03 ---------------------------------------------------------------------------------------------------- */
#define RE15_S2_RIEGEL_BANK      3
#define RE15_S2_RIEGEL_BIT       151      /* (3,151): Ck @0x0ABE (2070) / @0x00F3E (11A0), Set 2070 sub02 @0x0CE4             */
#define RE15_S2_2070_RIEGEL_SLOT 7        /* 2070 main00 Aot_set @0x0AC2 (sce 3 -> sub02)                                    */
#define RE15_S2_2070_ZU2040_SLOT 5        /* 2070 main00 Door_aot_set @0x0B1C -> 2040                                        */
#define RE15_S2_11A0_TUER_SLOT   0        /* 11A0 main00 Aot_set msg 2 @0x00F42 / Door_aot_set @0x00F5A                      */
#define RE15_S2_11A0_TUER_OFF    0x00F5A  /* Door_aot_set (32 B) Slot 0 -> 2070 (-18650,0,-19300) Cut 12                     */
#define RE15_S2_2040_TUER_SLOT   4        /* 2040 main00 Door_aot_set @0x0131C -> 2070                                       */
#define RE15_S2_2040_TUER_OFF    0x0131C

/* ---- Uhr K-08 ------------------------------------------------------------------------------------------------------------ */
#define RE15_S2_UHR_ITEM         0x3F     /* ROOM2090 @0x00A12 pc[14]                                                       */
#define RE15_S2_UHR_SLOT         5        /* pc[1]                                                                          */
#define RE15_S2_UHR_BIT          90       /* pc[18] Taken-Bit                                                               */
#define RE15_S2_UHR_OBJ          0        /* pc[20] -> Obj_model_set obj 0 @0x00A28                                         */
#define RE15_S2_UHR_HALB         500      /* Rechteck 1000 x 1000 wie @0x00A12 `e8 03 e8 03`                                */
/* PORT-WAHL: das Modell liegt 450 neben dem Schluessel (laengs x; Schluessel-Quadrat 270 = 180 x 3/2, RE15_GSZ_KEY_SKALA_Q12).
 * Die Item-Zone ist die des Schluessels (Platz 16 hinter dessen Platz 15, gatorszene GSZ_KEY_AOT; 2090 main00 nutzt 0..5,
 * Kamerazonen 48..63, Montage-Tuer 20, Kanister-Tuer 21): Lauf t5 zeigte, dass der Spieler vor der Leiche bei x -1050
 * klemmt - eine um 450 versetzte Zone (x -220..780) erreichte er nicht. */
#define RE15_S2_UHR_PLATZ        16
#define RE15_S2_UHR_VERSATZ      450

/* ---- Haken -------------------------------------------------------------------------------------------------------------- */
void re15_story2_r42_install(uint16_t room_id);                               /* scd_room_setup.c (Ende des Installer-Blocks) */
const uint8_t *re15_story2_r42_ereignis(uint16_t room_id, uint8_t event_id); /* scd_vm.c scd_event_fire                        */
void re15_story2_r42_tick(void);                                             /* scd_vm.c scd_vm_tick (Gameplay-Lauf)          */
void re15_story2_r42_schluessel_liegt(uint16_t room_id);                     /* gatorszene_r41.c key_bei_leiche               */
int  re15_story2_r42_discard_stelle(unsigned room_id, uint8_t msg_id, uint8_t *out_item);   /* item_discard_common.c        */
const uint8_t *re15_story2_r42_desc(uint8_t item_id);                        /* re15_inv_screen.c re15_inv_desc_entry         */
int  re15_story2_r42_tor2090_nimmt_schluessel(void);                         /* gatorszene_r41.c tor_druck: 0 seit K-01       */
void re15_story2_r42_tor2090_alt_setzen(int an);                             /* Pruefhaken: 1 = Stand Runde 41 (Riegel)       */

/* Pruefhaken / Messzeilen */
const uint8_t *re15_story2_r42_text(uint16_t room_id, uint8_t id, int *out_len);
const uint8_t *re15_story2_r42_programm(int elza, int *out_len);             /* Szene 2080 (0 Leon) / 2081 (1 Elza)           */
int  re15_story2_r42_szene_laeuft(void);
extern unsigned g_re15_s2_zaehler[8];  /* 0 Hinweis 52, 1 Loch zu (53), 2 Loch auf (54), 3 andere Seite (1), 4 aufgeschlossen
                                        * 11A0, 5 aufgeschlossen 2040, 6 Uhr-Hinweis 47, 7 Szenen-Start */

#endif /* RE15_STORY2_R42_H */
