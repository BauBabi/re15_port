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
 *                                                                (nur bei (3,22)=1, Wagen-Szene gelaufen)
 * Nachbesserung 1 (Abnahme 1 M1, Entscheid Orchestrator Variante a): der Fuehrerstand ist VOR dem Wagen erreichbar
 * (Treppe main00 @0x00CB2, Tuer Slot 1 main00 @0x00C52 -> Cut 0 -> sub00 @0x00D2C -> sub07; sub07 @0x010D0 fragt nur
 * Ck(4,23)=0). Wagen-Flag (3,22): Schreiber sub02 @0x00D7A `22 03 16 01`, Leser sub01 @0x00D62 `21 03 16 00`.
 *   ROOM6030 sub07 @0x010E8 -> sub08 bei (3,22)=0       ->  Fuehrerstand vorne zuerst: msg 11, 31, dann 13..19;
 *                                                           setzt (9,128) "Sherry-Nachtrag offen"
 *   ROOM6030 sub07 @0x0110C `04 ff 18 09` -> sub09 @0x01376 bei (9,128)=1 und (3,22)=1 (Zweig @0x010EC Else_ck (4,23)=1,
 *                                                           @0x010F4 Ck(3,89)=0 Annette da) -> Nachtrag msg 40, 25;
 *                                                           loescht (9,128)
 * Stimmen: synchro/STAGE<n>/room<ID>/main<Id>.wav (audio_pc.c, kein Eintrag in stimmen_r39.c noetig).
 *
 * Message-Ids ROOM6030 (VERTRAG Runde 36 §1.2: Port-Ids < 32 = PSX MSG_TABLE_N): RDT 0..20, ST80-T 21..26.
 * 27..30 Spur AN (r41/annette, Zugstart-Szene nach dem Tor, Aufnahmen main27..30). Nachbesserung 1: 31 und 40 (40 =
 * VERTRAG Runde 41 1.2 "Port-Ids ab 40", nur PC: PSX MSG_TABLE_N 32 zeigt sie nicht). Beide ohne Aufnahme (stumm).
 */
#ifndef RE15_STORY80_R41_H
#define RE15_STORY80_R41_H

#include <stdint.h>

#define RE15_S80_RAUM_30C1        0x30C1u  /* Elza (xxx1); Leon 30C0 hat keine Marvin-Szene                        */
#define RE15_S80_SUB_30C1         3        /* sub00 @0x00C02 `04 ff 18 03`                                          */
#define RE15_S80_RAUM_6030        0x6030u  /* Leon (xxx0)                                                           */
#define RE15_S80_SUB_WAGEN        2        /* sub01 @0x00D70 `04 ff 18 02`                                          */
#define RE15_S80_SUB_PULT         8        /* sub07 @0x010E8 `04 ff 18 08`                                          */
#define RE15_S80_SUB_PULTRUHE     9        /* sub07 @0x0110C `04 ff 18 09` (Annette am Pult, (4,23)=1, (3,89)=0)    */
#define RE15_S80_WAGEN_BANK       3        /* (3,22): sub02 @0x00D7A `22 03 16 01`, sub01 @0x00D62 `21 03 16 00`    */
#define RE15_S80_WAGEN_BIT        22
#define RE15_S80_ID_VORNE         31       /* "Everyone we could find. Can you start this thing?" (NB1)             */
#define RE15_S80_ID_NACHTRAG      40       /* "There's a little girl in the back... Sherry." (NB1)                  */
/* Port-Bit "Sherry-Nachtrag offen" (Nachbesserung 1): Bank 9 (mit g_game.flags gespeichert). Zensus 2026-11-07
 * (tools/r36_kartenbits/bank9_zensus.py): Original frei (Block 110..133), Port frei (Integrationsbaum: 110-117, 125;
 * r41-Zweige: moosgang 126/127, annette 133). Eingetragen in tests/unit/test_r36_kartenbits.c k_port[]. */
#define RE15_S80_BIT_BANK         9
#define RE15_S80_BIT_NACHTRAG     128
#define RE15_S80_ID_ERSTE_NEUE    21       /* ROOM6030: RDT-Nachrichten 0..20 (Sektion @0x01394)                    */
#define RE15_S80_ID_LETZTE_NEUE   26

enum { RE15_S80_P_30C1 = 0, RE15_S80_P_WAGEN = 1, RE15_S80_P_PULT = 2, RE15_S80_P_PULT_VORNE = 3,
       RE15_S80_P_NACHTRAG = 4, RE15_S80_P_N = 5 };

/* HAKEN op_evt_exec (scd_vm.c): liefert das Programm statt des Original-Subs, sonst `orig`. */
const uint8_t *re15_story80_evt_exec(uint16_t room_id, uint8_t sub_id, const uint8_t *orig);

/* Tests/Abnahme: Programm-Bytes, Soll-Folge der Message-Ids, Zaehler der Umleitungen je Programm. */
const uint8_t *re15_story80_programm(int p, int *out_len);
const uint8_t *re15_story80_folge(int p, int *out_n);
extern unsigned g_re15_story80_umleitungen[RE15_S80_P_N];

#endif /* RE15_STORY80_R41_H */
