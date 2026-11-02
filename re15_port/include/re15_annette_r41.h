/*
 * RE1.5 Rebuilt — Runde 41 Spur AN: Annette-Szene im hinteren Zugwagen (ROOM5090, Leon).
 *
 * Herleitung, Belege und alle Konstanten: analysis/befunde_runde41/AN_annette.md.
 *
 * Nutzerentscheid (Wortlaut im Dossier): sobald Boss am Bahnsteig, Generator und Tor erledigt sind, steht die Frau im Kittel
 * nicht mehr im Fuehrerstand 603, sondern im hinteren Wagen 509; dort spricht sie den mutierten William an und er toetet sie
 * (Vorbild RE2), danach der Kampf gegen die Riesenform wie bisher, dann der G5.
 *
 * Was die Disc schon traegt (Dossier 2.1/2.2): die drei Bedingungen sind zusammen (3,89) (ROOM6000 sub07 @0x011B4 Set(3,89,1),
 * setzt (3,49) aus ROOM5080 sub01 @0x00796 voraus); ROOM6030 sub07 @0x010F8 spawnt sie ab (3,89)=1 nicht mehr; 5090 ist vorher
 * nicht erreichbar (ROOM6030 sub06 @0x0106A Tuer Slot 3 = msg 10). ROOM5090 sub00 @0x0122A spawnt sie unbedingt.
 *
 * Was der Port dazu tut (alles PORT-WAHL, im Code markiert):
 *   - Spawn @0x0122A nur bei (3,89)=1 und (3,42)=0 (Szene noch nicht gelaufen = Kampf noch nicht begonnen);
 *   - Szene vor dem Kampfstart: ROOM5090 sub01 @0x01296 Evt_exec sub04 -> Port-Programm (Bytecode aus Original-Opcodes),
 *     das am Ende selbst Evt_exec sub04 ruft; der Kampfstart laeuft danach unveraendert;
 *   - Takt der Figuren (Riesenform, Annette) in re15_annette_r41_tick, Signale ueber Bank 5 (Raum-Scratch);
 *   - Aussehen: RE2-Annette (CDEMD0.EMS kind 0x47/0x49) auf dem 0x4D-Gerippe, shared_assets/RE2/ANNETTE_4D*.EMD
 *     (tools/r41_annette/annette_modell.py).
 */
#ifndef RE15_ANNETTE_R41_H
#define RE15_ANNETTE_R41_H

#include <stdint.h>

#define RE15_AN_RAUM          0x5090u   /* nur Leon: 5091/6031 haben keine 0x4D (Dossier 2.2) */
#define RE15_AN_TYP           0x4Du     /* Frau im Kittel, ROOM5090 sub00 @0x0122A `44 00 4d 40` */
#define RE15_AN_FORM_TYP      0x30u     /* Riesenform, ROOM5090 sub00 @0x0124A `44 01 30 33` */
#define RE15_AN_SUB_KAMPF     4u        /* ROOM5090 sub01 @0x01296 `04 ff 18 04` */

/* Flags (Dossier 2.1, 6) */
#define RE15_AN_TOR_BANK      3u
#define RE15_AN_TOR_BIT       89u       /* (3,0x59): ROOM6000 sub07 @0x011B4 `22 03 59 01` */
#define RE15_AN_KAMPF_BANK    3u
#define RE15_AN_KAMPF_BIT     42u       /* (3,0x2A): ROOM5090 sub01 @0x0129A `22 03 2a 01` */

/* Nachrichten (VERTRAG 1.2: Port-Ids ab 40); Stimmen synchro/STAGE5/room5090/main40..42.wav */
#define RE15_AN_MSG_SATZ      40        /* "Annette: W-William! You're alive!" (RE2 woertlich) */
#define RE15_AN_MSG_SCHREI    41        /* "Annette: Aaaah!" (PORT-WAHL) */
#define RE15_AN_MSG_LEON      42        /* "Leon: Annette!" (PORT-WAHL) */

/* Signale Programm <-> Takt, Bank 5 = Raum-Scratch (wie RE15_GSZ_SIG_*, gatorszene_r41.h). In 5090 belegt das Original
 * (5,28) (sub01 @0x012A6) und die Plc_dest-Ankunftsbits (5,32)/(5,33); 0x10-0x12 sind frei. */
#define RE15_AN_SIG_SCHLAG    0x10u     /* Programm -> Takt: Annette weicht, die Form dreht sich und schlaegt */
#define RE15_AN_SIG_TREFFER   0x11u     /* Takt -> Programm: der Hieb hat getroffen (Schrei) */
#define RE15_AN_SIG_LIEGT     0x12u     /* Takt -> Programm: sie liegt */

/* Phasen des Takts */
enum {
    RE15_AN_AUS = 0, RE15_AN_DROHEN = 1, RE15_AN_WEICHEN = 2, RE15_AN_HIEB = 3, RE15_AN_FLUG = 4, RE15_AN_KIPPEN = 5,
    RE15_AN_LEICHE = 6, RE15_AN_FREI = 7
};

/* Haken (je eine Zeile, "Runde 41 Spur AN") */
int  re15_annette_r41_spawn_weg(uint16_t room_id, uint8_t type);                                    /* scd_vm.c op_sce_em_set */
const uint8_t *re15_annette_r41_evt_exec(uint16_t room_id, uint8_t sub_id, const uint8_t *target);  /* scd_vm.c op_evt_exec */
void re15_annette_r41_install(uint16_t room_id);                                                    /* scd_room_setup.c */
void re15_annette_r41_tick(void);                                                                   /* enemy_ai_common.c run_all */
int  re15_annette_r41_besitzt(int slot);                                                            /* enemy_ai_common.c run_all */
const char *re15_annette_r41_emd_datei(uint8_t type);                                               /* platform/pc/main.c */
int  re15_annette_r41_volle_drehung(const void *actor);                                             /* platform/pc/main.c Zeichner */
void re15_annette_r41_modell_hook(void (*fn)(uint8_t type));                                        /* platform/pc/main.c */
void re15_annette_r41_audio_hook(void (*se_fn)(int se_id, int flag2000), void (*bank_fn)(int bank)); /* platform/pc/main.c */

/* Fuer Tests */
const uint8_t *re15_annette_r41_programm(int *out_len);
const uint8_t *re15_annette_r41_meldung(int msg_id, int *out_len);
int  re15_annette_r41_phase(void);
int  re15_annette_r41_slot(int form);           /* 1 = Riesenform, 0 = Annette; -1 = keiner */
int  re15_annette_r41_blut(void);               /* Blutspawns seit Szenenstart */
void re15_annette_r41_reset(void);

#endif
