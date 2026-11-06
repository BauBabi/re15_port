/*
 * RE1.5 Rebuilt — Runde 41 Spur ST80-T "Story-Texte umsetzen": Szenen nach den Nutzerentscheiden aus dem Story-Abgleich.
 *
 * Dossier mit Zeilenliste und Stimmen-Pfaden: analysis/story80/TEXTE_UMSETZUNG.md.
 * Texte: tools/r36_p/texte_bauen.py -> engine/src/gen/story_r36_texte.inc (re15_story_r36.h, auch die neuen Ids 21..26).
 * Szenen: tools/r41_story80/szenen_bauen.py -> engine/src/gen/story80_szenen.inc (ORIGINAL-Bytes, Belege je Baustein).
 *
 * KEIN Asset-Patch: der Port tauscht nur das Ziel eines Evt_exec (op_evt_exec, scd_vm.c), wenn die ersten 16 Bytes des
 * Original-Subs der Signatur gleichen:
 *   ROOM30C1 sub00 @0x00C02 `04 ff 18 03` -> sub03 @0x00C2C  ->  kurze Fassung: msg 1..5, danach Marvins Abgang
 *                                                                (Original sub03 @0x00E16..@0x00E68) unveraendert
 *   ROOM6030 sub01 @0x00D70 `04 ff 18 02` -> sub02 @0x00D7A  ->  Wagen-Szene: msg 0, 1, 21, 2, 3, 22, 4, 5, 6, 7, 23, 24, 8
 *   ROOM6030 sub07 @0x010E8 `04 ff 18 08` -> sub08 @0x0113A  ->  Fuehrerstand: msg 11, 12, 25, 26, dann 13..19 wie bisher
 * Stimmen: synchro/STAGE<n>/room<ID>/main<Id>.wav (audio_pc.c, kein Eintrag in stimmen_r39.c noetig).
 *
 * Message-Ids ROOM6030 (VERTRAG Runde 36 §1.2: Port-Ids < 32 = PSX MSG_TABLE_N): RDT 0..20, ST80-T 21..26.
 * Frei fuer andere Spuren: 27..31 (Hinweis an r41/annette, Zugstart-Szene nach dem Tor).
 */
#ifndef RE15_STORY80_R41_H
#define RE15_STORY80_R41_H

#include <stdint.h>

#define RE15_S80_RAUM_30C1        0x30C1u  /* Elza (xxx1); Leon 30C0 hat keine Marvin-Szene                        */
#define RE15_S80_SUB_30C1         3        /* sub00 @0x00C02 `04 ff 18 03`                                          */
#define RE15_S80_RAUM_6030        0x6030u  /* Leon (xxx0)                                                           */
#define RE15_S80_SUB_WAGEN        2        /* sub01 @0x00D70 `04 ff 18 02`                                          */
#define RE15_S80_SUB_PULT         8        /* sub07 @0x010E8 `04 ff 18 08`                                          */
#define RE15_S80_ID_ERSTE_NEUE    21       /* ROOM6030: RDT-Nachrichten 0..20 (Sektion @0x01394)                    */
#define RE15_S80_ID_LETZTE_NEUE   26

enum { RE15_S80_P_30C1 = 0, RE15_S80_P_WAGEN = 1, RE15_S80_P_PULT = 2, RE15_S80_P_N = 3 };

/* HAKEN op_evt_exec (scd_vm.c): liefert das Programm statt des Original-Subs, sonst `orig`. */
const uint8_t *re15_story80_evt_exec(uint16_t room_id, uint8_t sub_id, const uint8_t *orig);

/* Tests/Abnahme: Programm-Bytes, Soll-Folge der Message-Ids, Zaehler der Umleitungen je Programm. */
const uint8_t *re15_story80_programm(int p, int *out_len);
const uint8_t *re15_story80_folge(int p, int *out_n);
extern unsigned g_re15_story80_umleitungen[RE15_S80_P_N];

#endif /* RE15_STORY80_R41_H */
