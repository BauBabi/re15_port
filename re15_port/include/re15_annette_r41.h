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
 *   - Spawn @0x0122A nur bei (3,89)=1, Zugstart gesehen (9,133)=1 und (3,42)=0 (Szene noch nicht gelaufen);
 *   - Szene vor dem Kampfstart: ROOM5090 sub01 @0x01296 Evt_exec sub04 -> Port-Programm (Bytecode aus Original-Opcodes),
 *     das am Ende selbst Evt_exec sub04 ruft; der Kampfstart laeuft danach unveraendert. Ablauf, Bildzahlen und Toene
 *     nach der echten RE2-Szene (Claire-Disc SLES-10972 ROOM6141 MAIN sub02 @0x01024..@0x0118E, Emulator-Aufnahme
 *     analysis/befunde_runde41/AN_belege/re2_claire/), Kameras auf die RE1.5-Schnitte 12/15/11/13 von 5090 abgebildet;
 *   - Nachbesserung 1: Zugstart-Szene im Fuehrerstand 6030 nach dem Tor (Nutzer-Idee): ROOM6030 sub07 @0x01132
 *     Evt_exec sub05 -> Port-Programm, einmalig ueber (9,133); solange sie nicht lief, bleibt die Wagentuer nach 5090
 *     beim Original-Text msg 10 (Form von sub06 @0x0106A) und der Ausgang Slot 2 die Tuer aus main00 @0x00C72
 *     (Nachbesserung 2: msg 20 "You can't get off the train right now." @0x01098 erst, wenn der Zug faehrt);
 *   - Nachbesserung 2: Blut beim Hieb = der Emitter der RE2-Szene (Birkin-Overlay FUN_80104C24 @0x80104CD4-60, 15 Bilder
 *     Effekt 0/1/0x2000 an zwei Klauen), Belege in annette_r41.c AN_BLUT_*;
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

/* Flags (Dossier 2.1, 6, Nachbesserung 1) */
#define RE15_AN_TOR_BANK      3u
#define RE15_AN_TOR_BIT       89u       /* (3,0x59): ROOM6000 sub07 @0x011B4 `22 03 59 01` */
#define RE15_AN_KAMPF_BANK    3u
#define RE15_AN_KAMPF_BIT     42u       /* (3,0x2A): ROOM5090 sub01 @0x0129A `22 03 2a 01` */
/* Port-Bit "Zugstart-Szene 6030 gesehen" (Nachbesserung 1): Bank 9 (mit g_game.flags gespeichert). Zensus
 * tools/r36_kartenbits/bank9_zensus.py 2026-11-02: 133 im Original frei (Block 110..133, 240 RDTs, 0 Desync), in keinem
 * r41-Zweig vergeben; VERTRAG Runde 41 1.1: 125-133 vergibt der Orchestrator -> bestaetigt 2026-11-02 (Nachbesserung 2),
 * eingetragen in tests/unit/test_r36_kartenbits.c k_port[] und VERTRAG.md 1.1. */
#define RE15_AN_ZUG_BANK      9u
#define RE15_AN_ZUG_BIT       133u

/* Nachrichten (VERTRAG 1.2: Port-Ids ab 40); Stimmen synchro/STAGE5/room5090/main<Id>.wav */
#define RE15_AN_MSG_SATZ      40        /* "Annette: W-William! You're alive!" (RE2 Xa 0x31 woertlich) */
#define RE15_AN_MSG_SCHREI    41        /* nur Ton, KEIN Untertitel (Nutzer F9): synchro/STAGE5/room5090/main41.wav */
#define RE15_AN_MSG_LEON      42        /* "Leon: Annette!" (PORT-WAHL, Nutzer F8) */
#define RE15_AN_MSG_WILLIAM   43        /* "Annette: William." (RE2 Xa 0x30 woertlich) */

/* ROOM6030 Zugstart (Nachbesserung 1). Ids 27-30 = Absprache mit Spur ST80-T (21-26 dort; < 32 = PSX MSG_TABLE_N);
 * Stimmen synchro/STAGE6/room6030/main27..main30.wav. Wortlaut PORT-WAHL (Vorschlag), tools/r41_annette/texte_bauen.py. */
#define RE15_AN_ZUG_RAUM      0x6030u
#define RE15_AN_ZUG_SUB       5u        /* ROOM6030 sub07 @0x01132 / sub06 @0x010C4 `04 ff 18 05` (Licht-Schleife sub05) */
#define RE15_AN_MSG_FAEHRT    27        /* Annette: There! The train's moving. */
#define RE15_AN_MSG_WAS       28        /* Annette: What was that? / ...Sherry is back there! */
#define RE15_AN_MSG_PRUEFE    29        /* Leon: I'll check it out. */
#define RE15_AN_MSG_BLEIB     30        /* Annette: No. She's my daughter. / Stay here and keep an eye on / the controls. */

/* Signale Programm -> Takt, Bank 5 = Raum-Scratch (wie RE15_GSZ_SIG_*, gatorszene_r41.h). Zensus Nachbesserung 1
 * (tools/scd_dump_room.py): 5090 belegt (5,28) (sub01 @0x012A6) und (5,32); 6030 belegt (5,32)/(5,33) (Plc_dest-
 * Ankunft). 0x10-0x13 sind in beiden frei; (5,0x21) nutzt das Programm wie 6030 sub08 @0x01160 als Ankunft Annettes. */
#define RE15_AN_SIG_SCHLAG    0x10u     /* 5090: Hieb (RE2 @0x01142 Member_set 0x17=2); 6030: Zug-an-Ton */
#define RE15_AN_SIG_STURZ     0x11u     /* 5090: Form faellt herein (RE2 @0x010CA/@0x010CE); 6030: Krach-Ton */
#define RE15_AN_SIG_LIEGT     0x12u     /* 5090: Schnitt weg, sie liegt, Schrei als Ton (RE2 @0x01150/@0x01162) */
#define RE15_AN_SIG_BRUELL    0x13u     /* 5090: Bruellen mit Cut 11 (RE2 @0x01104 Se_on(2,0x0C)) */
#define RE15_AN_SIG_ZUG_AN    RE15_AN_SIG_SCHLAG
#define RE15_AN_SIG_KRACH     RE15_AN_SIG_STURZ

/* Phasen des Takts (5090) */
enum {
    RE15_AN_AUS = 0, RE15_AN_WARTEN = 1, RE15_AN_STURZ = 2, RE15_AN_DROHEN = 3, RE15_AN_HIEB = 4, RE15_AN_REAKTION = 5,
    RE15_AN_LEICHE = 6, RE15_AN_FREI = 7,
    RE15_AN_RUHE = 8      /* Nachbesserung 3 (M8): Wiedereintritt nach der Szene, Leiche liegt (RE2 MAIN sub00 @0x00D16) */
};

/* Haken (je eine Zeile, "Runde 41 Spur AN") */
int  re15_annette_r41_spawn_weg(uint16_t room_id, uint8_t type);                                    /* scd_vm.c op_sce_em_set */
const uint8_t *re15_annette_r41_evt_exec(uint16_t room_id, uint8_t sub_id, const uint8_t *target);  /* scd_vm.c op_evt_exec */
void re15_annette_r41_install(uint16_t room_id);                                                    /* scd_room_setup.c */
void re15_annette_r41_tick(void);                                                                   /* enemy_ai_common.c run_all */
int  re15_annette_r41_besitzt(int slot);                                                            /* enemy_ai_common.c run_all; Annette ab dem Trefferbild (Stufe 8) */
const char *re15_annette_r41_emd_datei(uint8_t type);                                               /* platform/pc/main.c */
int  re15_annette_r41_volle_drehung(const void *actor);                                             /* platform/pc/main.c Zeichner */
/* Nachbesserung 4 (Abnahme 2 N3): 1 = dieser Aktor ist Annettes Leiche (hp -1) und kein Schuss-Kandidat - wie die liegende
 * RE2-Annette (Claire RAM lauf3 e23..e30 +0x156 = -1), die RE2s Kandidatenfilter verwerfen: Pistole FUN_800410CC (`lh v0,342(s2)`
 * @0x80041280 / `bltz v0,0x80041300` @0x80041288), Messer/Bolzen FUN_800470C0 (`lh v0,342(s0)` / `bltz v0,0x8004740c`
 * @0x80047148-50). Seit NB5 (Nutzerentscheid 2026-11-03, RE2-Retail-Regel fuer alle Begleiter 0x40..0x4D mit hp < 0 im
 * Resolver) ist das ein Teilfall der allgemeinen Regel; der Haken bleibt als Zustandsabfrage der Szene. */
int  re15_annette_r41_kein_ziel(const void *actor);                                                /* re15_damage.c Waffen-Resolver */
void re15_annette_r41_modell_hook(void (*fn)(uint8_t type));                                        /* platform/pc/main.c */
void re15_annette_r41_audio_hook(void (*se_fn)(int se_id, int flag2000), void (*bank_fn)(int bank)); /* platform/pc/main.c */

/* Toene der Szenen 5090/6030 (Nachbesserung 1): Mini-Bank shared_assets/RE2/ANNETTE_SE.VBS, gespielt von
 * platform/pc/src/annette_se_pc.c (re15_pc_annette_se). Herkunft je Satz: Werkzeug tools/r41_annette/an_se_cut.py. */
enum {
    RE15_AN_SE_KRACH  = 0,   /* RE2 Claire ROOM6141 sub02 @0x01040 Se_on(3,0x0B): Krachen vor "William." */
    RE15_AN_SE_STURZ  = 1,   /* RE2 Claire ROOM6141 sub02 @0x010CE Se_on(2,0x0A): William faellt herein */
    RE15_AN_SE_BRUELL = 2,   /* RE2 Claire ROOM6141 sub02 @0x01104 Se_on(2,0x0C): mit Cut 0x0A (Anstarren) */
    RE15_AN_SE_ZUG_AN = 3,   /* RE2 Zug faehrt an (ROOM702.SAP "Train Powering Up", Satz im Werkzeug belegt) */
    RE15_AN_SE_N      = 4
};
void re15_annette_r41_se_hook(void (*fn)(int satz));                                                /* platform/pc/main.c */
/* Runde 42 Story STAGE6 (E9): Elzas Zugstart 6031 spielt dieselben Saetze (Zug an, Krachen) - re15_story6_r42.h */
void re15_annette_r41_ton_spielen(int satz);
void re15_annette_r41_stimme_hook(void (*fn)(uint16_t room, int voice_id));                         /* platform/pc/main.c */
/* NB4 N4: laedt den Tonteil von DOOR2A in die RE2-Tuerbank (Port-Bank 0x0E); 1 = geladen (annette_se_pc.c) */
void re15_annette_r41_tuerbank_hook(int (*fn)(void));                                               /* platform/pc/main.c */

/* Fuer Tests */
const uint8_t *re15_annette_r41_programm(int *out_len);
const uint8_t *re15_annette_r41_programm_zug(int *out_len);     /* 6030 Fuehrerstand */
const uint8_t *re15_annette_r41_programm_tuer(int *out_len);    /* 6030 Wagen: Ausgang Tuer, Tuer nach 5090 zu (msg 10) */
const uint8_t *re15_annette_r41_meldung_raum(uint16_t room_id, int msg_id, int *out_len);
int  re15_annette_r41_zug_szene(void);           /* Zugstart-Programme seit Raumeintritt (Testzaehler) */
int  re15_annette_r41_ton(int satz);             /* gespielte Saetze seit reset (Testzaehler) */
int  re15_annette_r41_schrei(void);              /* Schrei-Stimmen seit reset (Testzaehler) */
const uint8_t *re15_annette_r41_meldung(int msg_id, int *out_len);
int  re15_annette_r41_phase(void);
int  re15_annette_r41_slot(int form);           /* 1 = Riesenform, 0 = Annette; -1 = keiner */
int  re15_annette_r41_blut(void);               /* Blut-Aufrufe (RE2 FUN_801058BC, 2 je Bild x 15) seit Szenenstart */
int  re15_annette_r41_blut_plaetze(void);       /* davon belegte ESP-Plaetze (Zeilen je Aufruf) */
int  re15_annette_r41_blut_skala(void);         /* Skala der Blutzeilen (NB3: Bildgroesse wie RE2 Schnitt 0x0A) */
void re15_annette_r41_reset(void);

#endif
