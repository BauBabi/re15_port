/*
 * RE1.5 Rebuilt — Runde 38 Spur F "dining1141": Elzas Weg ROOM1021 -> 1011 -> 1131 -> 1141 (Logikloch John) und der
 * Rolladen-Ton in ROOM1140/1141. Dossier mit allen Messungen und Belegen: analysis/befunde_runde38/F_dining1141.md.
 *
 * NUTZER-VORGABEN (woertlich, analysis/befunde_runde38/AUFTRAG.md + analysis/befunde_runde37/AUFTRAG.md Nachtrag 14):
 *   "Als Hinweis machen wir: „There's an empty key tag on his belt. / It reads "Dining Room".“ und das "Help" ist
 *    einfach Teil der Audio files, genau wie das klopfen. Das soll nicht als Text erscheinen."
 *   "Der Raum 1141 ist vorher abgeschlossen, Roy hat den Schlüssel. Nachdem er zum Zombie geworden ist, hat er den
 *    Schlüssel an der Stelle gedropped, wo er vorher am Boden lag in ROOM 1011. [...] ohne das John, wait und das
 *    Sorry.... Einfach nur direkt - are you okay? what's happened? [...] immer wieder ein panisches Klopfen/Hämmern und
 *    ein help ein regelmäßigen abständen, bis man den geheimen Schalter gefunden hat um die Tür zu öffnen, mit panischer
 *    Musik. Wenn Die Monkeys da sind, und Elza eine Tür anklickt, muss, so lange die Monkeys noch nicht tot sind kommen
 *    "I cannot leave him alone with those monsters"."
 *   "In Room 1140 hat das Rolltor keinen Sound beim hochfahren."
 *
 * KEIN ASSET-PATCH: alle RDTs bleiben byte-true. Neue Programme = Original-Bytes + eingefuegte Zeilen in ausgelieferter
 * Opcode-Form (engine/src/gen/dining_r38_programme.inc, Werkzeug tools/r38_dining/programme_bauen.py mit Listing).
 *
 * ABLAUF (nur Elza-RDTs 1011/1021/1131/1141; Leon nur der Rolladen-Ton in 1140):
 *   1021  Roys Leiche (sub06, Ereignis 6): msg 9 -> Port-Nachricht 40 = msg 9 + Seite + Hinweis.
 *   1011  nach der 1021-Szene ((3,0x5F) = 1, ROOM1021 sub02 @0x01FE4) liegt der Dining Room Key an Roys Liegestelle.
 *   1131  Tuer Slot 0 -> 1141 verschlossen, bis der Schluessel benutzt ist (RE2-Tuer-Handler wie Runde 36 Spur Q).
 *   1141  Phase H ((4,0xF0) = 1 und Riegel zu (4,0xC3) = 0): beim Betreten statt der Szene sub02 die Klopf-Schleife
 *         (Klopfen + "Help me out." + Raum-Titel MAIN16); John und die Gorillas werden nicht gezeigt (er sitzt auf dem
 *         Dach). Gemaelde (sub06) hebt den Rolladen MIT Ton; der Riegel (sub05) beendet die Schleife, John bricht durch
 *         die Dachtuer herein, Szene sub02 ab msg 2 (msg 2 ohne "Same here..."), danach Tuersperre (msg 41) solange die
 *         Gorillas leben, dann geht John durch die Dachtuer voraus ((9,67)) und die Tueren sind wieder frei.
 */
#ifndef RE15_DINING_R38_H
#define RE15_DINING_R38_H

#include <stdint.h>

/* ---- Raeume ------------------------------------------------------------------------------------------------- */
#define RE15_R38F_RAUM_1011   0x1011u
#define RE15_R38F_RAUM_1021   0x1021u
#define RE15_R38F_RAUM_1131   0x1131u
#define RE15_R38F_RAUM_1140   0x1140u
#define RE15_R38F_RAUM_1141   0x1141u

/* ---- der Gegenstand ----------------------------------------------------------------------------------------------
 * Item-Id 0x43 (ausgeliefert "Communications Card"): Zensus tools/r38_dining/zensus.py ueber alle RDTs — 0
 * Item_aot_set (`50 slot 09`, Id u16 @+14), 0 Aot_reset sce 9, 0 Keep_Item_ck (0x5E); Eigenschaftssatz
 * @0x80074DA8 + 0x43*12 = `01 00 00 00 88 4c 07 80 00 00 00 00` (Obergrenze 1, wie alle Schluesselkarten 0x36..0x47);
 * Klasse "Schluessel" (Id >= 0x22, `sltiu id,0x22` @0x80049124). Freie Ids derselben Familie 0x36..0x47: nur 0x43
 * (0x40/0x42/0x45 vergibt der Port schon, 0x36-0x39/0x41/0x44/0x46/0x47 sind platziert). Icon-Tile/ITPS-Block liegen
 * in den 72er-Dateien (Id < 0x48). Name, CHECK-Text, Icon und Item-Bild ersetzt der Port.
 * Modell/Icon/Bild = RE2 Item 0x5B (Herz-Schluessel, mesh066): gen/dining_r38_schluessel.inc
 * (tools/r38_dining/re2_export.py; ROOM2020.RDT MD1 @0x003474 / TIM @0x01A160, ITEMALL Tile 91, ITPS Block 91). */
#define RE15_R38F_ITEM          0x43
#define RE15_R38F_MENGE         1        /* jede Schluessel-Ausgabe des Originals (discard_sites.inc: Menge 1) */
/* Bank 9 (VERTRAG Runde 38 1.1: F = 64, 66, 67; Zensus: in keinem Item_aot_set pc[18] und keinem Ck/Set der Bank 9). */
#define RE15_R38F_BIT_GENOMMEN  64       /* Dining Room Key aufgenommen (ROOM1011)                            */
#define RE15_R38F_BIT_TUER      66       /* Tuer 1131 -> 1141 aufgeschlossen (eigenes Schlossbit wie RE2
                                          * FUN_8007730c @0x80051690, Runde 36 Spur Q Bit 93)                 */
#define RE15_R38F_BIT_JOHN_WEG  67       /* John hat 1141 durch die Dachtuer verlassen                        */

/* ---- ROOM1011: der Schluessel an Roys Liegestelle ------------------------------------------------------------------
 * Roys Spawn sub00 @0x009F6 / @0x00A3E `44 00 47 40 .. 30 f8 00 00 1a 1d` = (-2000, 0, 7450). Sichtbar ab dem Ende der
 * 1021-Szene: (3,0x5F) wird NUR von ROOM1021 sub02 @0x01FE4 gesetzt (Zensus flag_xref.txt) und nie geloescht.
 * Prop obj 4 (RDT-Kopf Byte 2 nOmodel = 3, obj 0..2 belegt @0x008EE/@0x00910/@0x00932; obj 3 bleibt frei fuer die
 * Parallel-Spur G (Roys Tagebuch, VERTRAG 1.5) — der Prop-Lader ist raum-, nicht phasengebunden); TIM-Slot
 * RE15_TIM_SLOT_PROP(4) = 8. Zone Slot 14 (main00 belegt 0..8; Abstand zu G aus demselben Grund). */
#define RE15_R38F_1011_OBJ      4
#define RE15_R38F_1011_SLOT     14
#define RE15_R38F_ROY_BANK      3
#define RE15_R38F_ROY_BIT       0x5F
#define RE15_R38F_KEY_X         (-2000)
#define RE15_R38F_KEY_Y         0
#define RE15_R38F_KEY_Z         7450
#define RE15_R38F_KEY_ROT_Y     512
/* Aufhebe-Rechteck 1000 x 1000 um die Liegestelle (haeufigste Item-Groesse, 69 von 162; RE15_R35I_KARTE_RECT_*). */
#define RE15_R38F_KEY_RECT_X    (-2500)
#define RE15_R38F_KEY_RECT_Z    6950
#define RE15_R38F_KEY_RECT_W    1000
#define RE15_R38F_KEY_RECT_D    1000
/* Prop-/Zonen-Felder wie die Item-Props des Originals (Runde 36 Spur Q, re15_schluessel_r36.h). */
#define RE15_R38F_PROP_FLAGS    0x000B   /* pc[6..7] 0x000A | 1 @0x80040998 */
#define RE15_R38F_PROP_BAND     1        /* pc[4] -> pool+130 @0x80040974   */
#define RE15_R38F_AOT_SAT       0x31     /* 160 von 164 Item_aot_set        */

/* ---- ROOM1021: Roys Leiche ----------------------------------------------------------------------------------------
 * Leiche = main00 @0x01E24 Ifel Ck(3,0x5F)==1 -> @0x01E2C Aot_set Slot 6 sce 3 -> sub06 (Ereignis 6) + @0x01E40 Prop.
 * sub06 @0x021EC..0x02221: Message_on 9 @0x02202 `2b 09 ff ff` ("He's dead.[Seite]Poor Roy...", @0x024FB). */
#define RE15_R38F_1021_EREIGNIS     6
#define RE15_R38F_MSG_1021_LEICHE   40   /* VERTRAG 1.2: 1021 40-44 */

/* ---- ROOM1131: die Tuer zum Besprechungsraum ----------------------------------------------------------------------
 * main00 @0x0088E `3b 00 02 31 00 00 e0 fc e4 c1 e8 03 a0 0f 50 e2 00 00 40 bb 00 00 00 14 ..` = Door_aot_set Slot 0,
 * sce 2, sat 0x31 -> Stage 0 Raum 0x14 (ROOM1141), Spawn (-7600,0,-17600), Cut 0 — ohne jede Bedingung.
 * Sperre = Muster Runde 36 Spur Q (schluessel_r36.c tuer_sperren / RE2 LAB_80051514): Slot -> sce-3-Ereignis. */
#define RE15_R38F_1131_TUER_SLOT    0
#define RE15_R38F_1131_EREIGNIS     29   /* Port-Ereignis (< RE15_RDT_MAX_SUB_SCD; in 1131 frei: sub00..sub02) */
#define RE15_R38F_TUER_FLAGS        0x31 /* sat des Tuer-Satzes selbst (pc[3] @0x00891) */
#define RE15_R38F_MSG_1131_ZU       40   /* "It's locked. / The door is marked "Dining Room"." (VERTRAG 1.2: 1131 40-44) */
#define RE15_R38F_MSG_1131_BENUTZT  41   /* "You've used the Dining Room Key."                                       */
#define RE15_R38F_MASKE             0xffffu   /* Pausemaske aller 524 ausgelieferten sce-1-Saetze */

/* ---- ROOM1140/1141: Rolladen-Ton ----------------------------------------------------------------------------------
 * Port-Bank 0x3B = die snd0-Bank von ROOM1131 (EDT @0x1450 / VH @0x14D0 / VB @0x2F70 = RDT-Kopf +0x08/+0x0C/+0x10):
 * nur dort liegen die Rolladen-Wellen (Satz 0x0A -> VAG 4, 0x0B -> VAG 5, 0x0C -> VAG 8; die Baenke von 1140/1141
 * tragen auf denselben Saetzen andere Wellen, sha1-Vergleich Dossier 2.1). Programme ROLL1140/ROLL1141 = sub03/sub06
 * mit den drei Se_on + Sleeps von ROOM1131 sub02 @0x00A48/@0x00A54/@0x00A58/@0x00A64/@0x00AB2/@0x00ABE. */
#define RE15_R38F_1140_EREIGNIS     3    /* Gemaelde Leon (Strom an; scd_vm.c s_power_gates leitet sonst auf 5) */
#define RE15_R38F_1141_EREIGNIS_GEMAELDE 6
#define RE15_R38F_1141_EREIGNIS_RIEGEL   5
#define RE15_R38F_SE_BANK_ROLLADEN  0x3B
/* ROOM1131.RDT im CD-Baum (Lader der Plattform) und die drei Saetze. */
#define RE15_R38F_ROLLADEN_RDT      "STAGE1/ROOM1131.RDT"

/* ---- ROOM1141: Phase H, Szene, Sperre ------------------------------------------------------------------------------
 * Phase H = (4,0xF0) == 1 (ROOM1021 sub02 @0x01FE0 setzt, ROOM1141 sub02 @0x00C18 loescht) UND Riegel zu (4,0xC3) == 0
 * (ROOM1141 sub05 @0x00FD2 setzt). Beim Betreten startet sub00 @0x00B02 `04 ff 18 02` die Szene sub02 — in Phase H
 * startet stattdessen das Programm HAEMMERN; die Spawns sub00 @0x00B06 (John) / @0x00B1A / @0x00B2E (Gorillas,
 * geparkt) nimmt der Installer wieder heraus, das Programm RIEGEL setzt sie beim Einbruch neu. */
#define RE15_R38F_SUB_SZENE         2
#define RE15_R38F_F0_BANK           4
#define RE15_R38F_F0_BIT            0xF0
#define RE15_R38F_C3_BANK           4
#define RE15_R38F_C3_BIT            0xC3
#define RE15_R38F_F2_BANK           4
#define RE15_R38F_F2_BIT            0xF2   /* Gorillas leben (sub02 @0x00C1C setzt, sub01 @0x00C0C loescht) */
#define RE15_R38F_JOHN_SCRIPT_SLOT  0      /* sub00 @0x00B06 `44 00 49 ..` */
#define RE15_R38F_JOHN_TYP          0x49
#define RE15_R38F_GORILLA_TYP       0x27   /* sub00 @0x00B1A/@0x00B2E `44 01 27 ..` / `44 02 27 ..` */
#define RE15_R38F_MSG_1141_FRAGE    40     /* msg 2 ohne "Same here..." (VERTRAG 1.2: 1141 40-49) */
#define RE15_R38F_MSG_1141_SPERRE   41     /* "I cannot leave him alone with those monsters."        */
/* Port-Baenke der Klopf-Schleife (Se_on pc[1]; RE1.5 kennt Bank 0..5, FUN_80045024 `sltiu 0x6` @0x80045094):
 *   0x3C = KLOPF1060 = RE2 ROOM1060 snd0 (shared_assets/RE2/KLOPF1060.*), Satz 0x0A/0x0B = das Schlagen
 *          (RE2 ROOM1060 sub05 @0x0F26 +0x22/+0x32/+0x4C);
 *   0x3D = Johns Hilferuf synchro/STAGE1/room1141/john_help.wav ("Help me out.", Schnitt aus room1171/main01.wav). */
#define RE15_R38F_SE_BANK_KLOPFEN   0x3C
#define RE15_R38F_SE_BANK_HILFE     0x3D
#define RE15_R38F_KLOPF_DATEI       "KLOPF1060"
#define RE15_R38F_HILFE_DATEI       "synchro/STAGE1/room1141/john_help.wav"
/* Choreografie des Einbruchs/Abgangs (PORT-WAHL; Formen und Begruendung im Werkzeug/Dossier 3.6). */
#define RE15_R38F_ELZA_SEITE_X      (-6450)
#define RE15_R38F_ELZA_SEITE_Z      (-2900)
#define RE15_R38F_ELZA_SEITE_YAW    3072
#define RE15_R38F_JOHN_TUER_X       (-7350)
#define RE15_R38F_JOHN_TUER_Z       (-400)
#define RE15_R38F_JOHN_TUER_YAW     1024
#define RE15_R38F_GANG_X            (-7350)
#define RE15_R38F_GANG_Z            (-7600)
#define RE15_R38F_TUER_INNEN_X      (-7350)
#define RE15_R38F_TUER_INNEN_Z      (-700)

/* ---- Haken ------------------------------------------------------------------------------------------------------- */
/* Raumaufbau NACH dem Init-Lauf (scd_room_setup.c Blockende, platform/pc/main.c Boot/CONTINUE). */
void re15_dining_r38_install(uint16_t room_id);
/* op_evt_exec (scd_vm.c): Ziel-Programm eines Evt_exec. `orig` = sub_scd[sub_id]; liefert orig oder ein Port-Programm. */
const uint8_t *re15_dining_r38_evt_exec(uint16_t room_id, uint8_t sub_id, const uint8_t *orig);
/* scd_event_fire (scd_vm.c), Programm-Kette: Port-Programm fuer (Raum, Ereignis) oder NULL. */
const uint8_t *re15_dining_r38_ereignis(uint16_t room_id, uint8_t event_id);
/* audio_pc.c Se_on-Zweig vor der Bank-Weiche: 1 = Port-Bank dieser Spur (0x3B/0x3C/0x3D) erledigt. */
int re15_dining_r38_se_on(unsigned bank, int satz);
/* Plattform-Ton (platform/pc/src/dining_r38_pc.c setzt ihn): bank = 0x3B/0x3C/0x3D, satz = EDT-Satz. */
extern void (*re15_dining_r38_se_hook)(int bank, int satz);

/* Gegenstand 0x43 (Namen/Texte/Bilder an den Ladestellen; Muster Runde 36 Spur Q). NULL/0 fuer jede andere Id. */
const uint8_t *re15_dining_r38_name(uint8_t item_id);
const uint8_t *re15_dining_r38_desc(uint8_t item_id);
int  re15_dining_r38_ist_schluessel(uint8_t item_id);   /* Wegwerf-Text "key" (item_prompt_common.c) */
void re15_dining_r38_icon_einsetzen(uint8_t *itemall, int size);
void re15_dining_r38_bild_einsetzen(uint8_t *itps, int size);
int  re15_dining_r38_discard_stelle(unsigned room_id, uint8_t msg_id, uint8_t *out_item);
/* Plattform-Lader des Props in ROOM1011. */
int re15_dining_r38_obj_id(uint16_t room_id);
const uint8_t *re15_dining_r38_md1_bytes(uint16_t room_id, int *out_size);
const uint8_t *re15_dining_r38_tim_bytes(uint16_t room_id, int *out_size);

/* ---- Pruefhaken (Riegel) --------------------------------------------------------------------------------------- */
const uint8_t *re15_dining_r38_programm(int nr, int *out_len);   /* 0 ROLL1140 1 ROLL1141 2 LEICHE 3 HAEMMERN 4 RIEGEL */
const uint8_t *re15_dining_r38_text(int nr, int *out_len);       /* 0 1021/40 1 1131/40 2 1131/41 3 1141/40 4 1141/41 */
int  re15_dining_r38_phase_h(void);         /* 1 = aktueller Raumaufbau 1141 in Phase H                          */
int  re15_dining_r38_tuer_gesperrt(void);   /* 1 = 1131 Slot 0 ist umgewidmet                                     */
int  re15_dining_r38_anker_ok(void);        /* Bitmaske der geprueften Quellbereiche (1 = bytegleich)             */
extern unsigned g_re15_r38f_zu_zaehler;       /* Druecke auf die verschlossene Tuer ohne Schluessel              */
extern unsigned g_re15_r38f_benutzt_zaehler;  /* Druecke mit Schluessel                                          */
extern unsigned g_re15_r38f_schleife_beendet; /* wie oft der Riegel die Klopf-Schleife beendet hat               */
extern unsigned g_re15_r38f_se_zaehler[3];    /* Se_on der drei Port-Baenke 0x3B/0x3C/0x3D (seit Prozessstart)   */

#endif /* RE15_DINING_R38_H */
