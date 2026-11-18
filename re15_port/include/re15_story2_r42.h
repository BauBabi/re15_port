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
/* Spieler am Schluesselloch (Lauf e2 "angekommen" (-11160,-7316), [leon-root] Bildpunkt x 230,6 in Cut 3). PORT-WAHL der Inszenierung
 * (Pruefung 1, Befund 1 "Begleiter steht mit dem Ruecken zur Kamera direkt vor der Spielfigur"; Befund 4 "Sherry kommt nach dem
 * 2030-Tuerton von Sueden aus dem Vordergrund"). Gemessen mit der Cut-3-Projektion (Auge/Ziel RID @0x0C0, sd 208 wie das Log;
 * Werkzeug scratchpad story/proj.py, gegen [leon-root] 230,6 / 84,7 auf 3 Bildpunkte genau) und der SCA ROOM2080 @0x4BC Band 8:
 *  - Sichtbarer Boden in Cut 3 nur zwischen dem Wandpfeiler links (Bildpunkt ~120) und dem Ostrahmen rechts (~235, Vordergrund).
 *  - Zwiegespraech QUER zur Blickachse: Spieler P rechts (Bildpunkt 217, Tiefe 8055), Begleiter N links (188, 8031), Abstand 1140
 *    (Form der RE2-Zweierszenen: beide im Profil). Der Spieler geht dafuer vom Rechner nach P (Plc_dest Modus 4 GEHEN, Form ROOM1021
 *    sub02 @0x020B2 `40 00 04 22`), kein Hindernis auf dem Weg (Kreis @0x600 (-12200,-7500) r 600 >= 1000 entfernt).
 *  - Begleiter von Suedwesten (aus dem Gang von der Leiter, wo er den Kanister geworfen hat), ueber den Zwischenpunkt W1 an der
 *    Wand @0x5E8 (x <= -12700, z -14450..-13050) vorbei (>= 715) und am Kreis @0x5F4 (-12700,-10000) r 600 vorbei (Rand 570):
 *    er tritt bei Bildpunkt 6 unten links in einer Tiefe von 4750 ins Bild (vorher (-10700,-20000): Tiefe 640 = direkt unter der
 *    Linse).
 *  - Sherry (E1 Punkt 3 "kommt vom Treppenabsatz die Treppe hinauf"): Tuer 2030 (-24400,-17700) Band 5, Treppe x -22700..-16400,
 *    Absatz oben = Nische SCA @0x51C x -16650..-14300 z -18300..-14750 (Boden fl 120 wie der Gang). Sie erscheint in der Nische
 *    (Bildpunkt -46, ausserhalb), laeuft durch deren Oeffnung bei x -14300 (z -18300..-14750, Weg z -15400) und ueber W2 an der Wand
 *    @0x5E8 vorbei (Abstand 950) vor die beiden. Der RUN haelt 300 vor dem Ziel (d < 300 @0x80051764): Lauf k_elza5 (gdigrab Bild
 *    831) mit dem Ziel (-10600,-11200) hielt Sherry bei (-10738,-11451) = Bildpunkt 194 genau vor John (185) und verdeckte seine
 *    Beine. Ziel jetzt (-10350,-10900): Halt ~(-10501,-11159) = Bildpunkt 205 zwischen John (185) und Elza (217), Tiefe 6899 (vor
 *    dem Paar, ein Kind - Kopf in Hueftenhoehe der Erwachsenen); Ziel zu P 1250, zu N 1346. */
#define RE15_S2_SPIELER_X        (-10700)   /* P */
#define RE15_S2_SPIELER_Z        (-9700)
#define RE15_S2_NPC_START_X      (-13400)   /* Start, ausserhalb des Bildes (Bildpunkt -65, Kopf y 211 unter dem Untertitelband) */
#define RE15_S2_NPC_START_Z      (-17200)
#define RE15_S2_NPC_ZW_X         (-12000)   /* W1 */
#define RE15_S2_NPC_ZW_Z         (-14600)
#define RE15_S2_NPC_X            (-11600)   /* N */
#define RE15_S2_NPC_Z            (-10400)
#define RE15_S2_SHERRY_START_X   (-15800)   /* Nische oben an der Treppe */
#define RE15_S2_SHERRY_START_Z   (-16400)
#define RE15_S2_SHERRY_ZW_X      (-12100)   /* W2 */
#define RE15_S2_SHERRY_ZW_Z      (-13900)
#define RE15_S2_SHERRY_X         (-10350)   /* S (Ziel; Halt 300 davor) */
#define RE15_S2_SHERRY_Z         (-10900)
#define RE15_S2_TREPPE_X         (-16400)   /* Blickziel beim Tuerton: oberes Treppenende (Treppe x -22700..-16400) */
#define RE15_S2_TREPPE_Z         (-16500)
/* Gier beim Erscheinen (Sce_em_set pc[16..17]) = Richtung zum ersten Wegpunkt, Konvention dir(Gier) = (cos, -sin) (0 = +X, 1024 = -Z,
 * 3072 = +Z; dachkiste_r36.c wp_yaw_zu, gemessen Punkt 1.2): round(atan2(-dz, dx) / 2pi * 4096) & 4095. Beim Wiederbetreten (K-07,
 * k_warten_*) die Gier zum Gespraechspartner der Szene. */
#define RE15_S2_GIER_NPC_START   3394       /* Start -> W1 */
#define RE15_S2_GIER_SHERRY_START 3709      /* Nische -> W2 */
#define RE15_S2_GIER_NPC_WARTEN  3665       /* N -> P */
#define RE15_S2_GIER_SHERRY_WARTEN 2296     /* S -> N (Sherry bleibt bei John) */
/* Weg vom Tuerton bis Sherry in der Nische: Tuer (-24400,-17700) -> Treppenfuss (-22700,-16500) 2081, Treppe bis -16400 6300,
 * bis zur Nische 608 = 8989 Einheiten; Bilder = Weg / Laufgeschwindigkeit Sherry (Plc_dest Modus 5 = NPC-Sub 5 RUN @0x80051484).
 * GEMESSEN (Lauf s2b/k_elza3, debug.log "[story2] ... Sherry erscheint" / "Ankunft Sherry"): erscheint F1192 bei (-15800,-16400),
 * Plc_dest ab F1193 (Sleep 1), Ankunft an W2 F1236 bei (-12173,-14091) = 4300 Einheiten in 43 Bildern = 100 je Bild aus dem
 * Stand (zweiter Abschnitt W2 -> S mit Wende: 3005 in 25 = 120). 8989 / 100 = 89 Bilder. PORT-WAHL: die Treppe selbst laeuft
 * Sherry nicht (Band-Wechsel 5 -> 8 im NPC-Gang nicht gebaut), sie erscheint, wenn sie oben waere. */
#define RE15_S2_TREPPE_WEG       8989
#define RE15_S2_SHERRY_V         100       /* gemessen, s. o. */
#define RE15_S2_SHERRY_LAUF_BILDER (RE15_S2_TREPPE_WEG / RE15_S2_SHERRY_V)
#define RE15_S2_NPC_SLOT         8        /* Skript-Slot wie RE15_K2080_NPC_SLOT (Aktor 9)                                  */
#define RE15_S2_SHERRY_SLOT      9
#define RE15_S2_TYP_MARVIN       0x40     /* ROOM2000 sub @0x017B4 `44 01 40 40 ..`                                         */
#define RE15_S2_TYP_JOHN         0x49     /* ROOM2001 @0x0179E `44 00 49 40 ..`                                             */
#define RE15_S2_TYP_SHERRY       0x4B     /* ROOM2001 @0x0180A `44 00 4b 40 ..`                                             */
#define RE15_S2_NPC_GRID         0x40     /* stehender NPC (ROOM2000/2001 pc[3] = 0x40)                                     */
/* Runde 42 rest Stufe 3, U-07: "Sherry wartet im Pausenraum 2011" - Set Mod ROOM2001 sub05 @0x1A92 `22 03 4a 01`, einziger Leser Mod
 * ROOM2011 sub00 @0x08BC `21 03 4a 01` (-> Sce_em_set Typ 0x4B @0x08C0). Elzas Szene 2081 loescht ihn, sobald Sherry dort erscheint. */
#define RE15_S2_SHERRY_2011_BANK 3
#define RE15_S2_SHERRY_2011_BIT  74
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
/* PORT-WAHL: das Modell liegt 450 neben dem Schluessel (Schluessel-Quadrat 270 = 180 x 3/2, RE15_GSZ_KEY_SKALA_Q12); seit Pruefung 1
 * Befund 6 nicht mehr laengs x (nach echtem Tod lag es bei Bildpunkt 287 am Plattformrand, Kamera 0), sondern in der Richtung, die in
 * der Einstellung des Schluessels sichtbar ist (re15_gatorszene_r41_neben_lage), und es wird so gross gezeichnet wie der Schluessel
 * (groesste Ausdehnung 144 -> 270, Skala 7680/4096, re15_story2_r42_scheitel; gemessen in Originalgroesse ~3 x 1,5 Bildpunkte, Lauf
 * s2b/k_elza5 Bild 512).
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
int  re15_story2_r42_spawn_weg(uint16_t room_id, uint8_t type);
#include "re15_md1.h"
const re15_md1_vertex_t *re15_story2_r42_scheitel(uint16_t room_id, int obj_id, const re15_md1_vertex_t *v, int n, int quad);   /* main.c Props: Uhr gross (Befund 6) */              /* scd_vm.c Sce_em_set: 11A0/11A1 Befund 8  */
int  re15_story2_r42_tor2090_nimmt_schluessel(void);                         /* gatorszene_r41.c tor_druck: 0 seit K-01       */
void re15_story2_r42_tor2090_alt_setzen(int an);                             /* Pruefhaken: 1 = Stand Runde 41 (Riegel)       */

/* Pruefhaken / Messzeilen */
const uint8_t *re15_story2_r42_text(uint16_t room_id, uint8_t id, int *out_len);
const uint8_t *re15_story2_r42_programm(int elza, int *out_len);             /* Szene 2080 (0 Leon) / 2081 (1 Elza)           */
int  re15_story2_r42_szene_laeuft(void);
extern unsigned g_re15_s2_zaehler[8];  /* 0 Hinweis 52, 1 Loch zu (53), 2 Loch auf (54), 3 andere Seite (1), 4 aufgeschlossen
                                        * 11A0, 5 aufgeschlossen 2040, 6 Uhr-Hinweis 47, 7 Szenen-Start */

#endif /* RE15_STORY2_R42_H */
