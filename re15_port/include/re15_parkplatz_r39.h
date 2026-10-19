/*
 * RE1.5 Rebuilt — Runde 39 Spur O "parkplatz11c0": ROOM11C0 (Parkplatz, Leon) — solange der Gorilla-Kampf laeuft,
 * zeigt die Tuer den Text "I cannot leave Ada alone here with those monsters!" statt Leon hinauszulassen.
 *
 * NUTZER-VORGABE (AUFTRAG.md Nachtrag 9, woertlich): "bei room11C0, während der Kampf gegen die Gorillas noch läuft, soll
 * bei Leon der Text kommen "I cannot leave Ada alone here with those monsters!". Wenn Leon versucht durch die Tür aus dem
 * Raum raus zu gehen." Dossier: analysis/befunde_runde39/O_parkplatz11c0.md (Messung vorher: die Tuer ging im Kampf).
 *
 * BELEGT IST DIE FORM, NICHT DER WUNSCH (RE1.5 hat diese Sperre nicht):
 *   - "Kampf laeuft" = die Original-Bedingung des Raums, kein Port-Zaehler, kein Bank-9-Bit:
 *       Beginn  sub01 @0x01820 Ck(4,64)==0 -> @0x01824 Evt_exec sub02; sub02 @0x0184E Set(4,64)=1 ... @0x01ACE Set(2,7)=0
 *       Ende    sub01 @0x0182E Ck(3,67)==0 && @0x01836 Ck(7,96) && @0x0183E Ck(7,97) -> @0x01842 Evt_exec sub03;
 *               sub03 @0x01B02 Set(3,67)=1 ... @0x01C08 Aot_on(0) (die Szene geht selbst durch die Tuer)
 *       Wiedereintritt im Kampf: sub00 @0x017C8 Else + @0x017D0 Ck(3,67)==0 -> (4,64)==1 && (3,67)==0
 *       Kill-Flags 0x60/0x61 = Sce_em_set pc[7] (@0x80042120/@0x80042170 -> Entity+0x1C6), gesetzt beim Gorilla-Tod
 *       STAGE1.BIN @0x8011b8e8-f4 (`jal 0x8004ef90`, a0 = 0x800b1038), Bank 7 = re15_em_status_zone() (Stage 1).
 *   - Sperre = RE2s Pflichtkampf-Form, ROOM7040 sub06 @0x0185A `46 00 04 31 01 00 00 00 ff ff` (Tuer-Slot -> MESSAGE in
 *     der Kampf-Beginn-Szene, vor der Rueckgabe der Steuerung) und Ruecknahme sub07 @0x01ACC `46 00 01 31 f4 98 00 00
 *     c9 a5` (Tuer zurueck mit dem eigenen Ziel in der Kampf-Ende-Szene); RE1.5-Zeilenform ROOM11B0 sub01 @0x01216
 *     `46 01 01 31 0f 00 ff ff 00 00` / ROOM1141 sub05 @0x00FDC `46 01 02 31 ..`; dasselbe Muster wie Runde 38 F 1141
 *     msg 41 (tools/r38_dining/programme_bauen.py:322-325/:352).
 *   - Aot_reset LAB_80040738 (@0x80040764 sce, @0x80040768-78 Flags, @0x80040788-a8 Nutzlast) tauscht nur sce + 3
 *     Halbworte; Ziel-Raum/Cut der Tuer bleiben im Satz. MESSAGE-Handler PTR_8007469c[1] = 0x80043084 (@0x800430a0
 *     `jal 0x80027e68`, Maske @0x800430a4), TUER-Handler [2] = 0x800430bc.
 *   - Aot_on LAB_800407bc @0x8004082c `jalr` ruft den Handler, den der Slot GERADE hat -> die Ruecknahme steht VOR
 *     sub03 @0x01C08 (Programm ENDE: Aot_reset, dann Gosub sub03).
 *   - Text in Erzaehlform `04 02 .. 08 .. 01 00` (ROOM1090 msg 7 @0x28E6, RE2 ROOM7040 msg 1 @0x01D1A `fa 02 ..`).
 *
 * ABLAUF IM PORT
 *   Erster Besuch:  sub01 @0x01824 Evt_exec sub02 -> Programm SZENE (gen/parkplatz_r39_programme.inc):
 *                   Aot_reset Slot 0 -> MESSAGE msg 40, Gosub sub02 (Szene unveraendert aus dem RDT) — nur bei
 *                   (3,67)==0: die einzige Freigabe sub01 @0x0182E Ck(3,67)==0 -> @0x01842 muss noch greifen koennen
 *                   (Abnahme 0 M1; bei (3,67)=1 und (4,64)=0 = Spielstand/Debug-Flags: Original-sub02, keine Sperre).
 *   Wiedereintritt: (4,64)==1 && (3,67)==0 nach dem Init-Lauf -> derselbe Aot_reset aus C (re15_aot_retype).
 *   Kampf-Ende:     sub01 @0x01842 Evt_exec sub03 -> Programm ENDE: Aot_reset Slot 0 -> DOOR (Nutzlast main00
 *                   @0x01712+14), Gosub sub03 (Szene unveraendert; Aot_on(0) @0x01C08 findet die Tuer).
 *   Elza ROOM11C1: keine Gorillas, kein Begleiter (sub00 = 5 Hunde) -> nichts.
 *   Stimme: keine - einfacher Raumtext wie Leons Tuertexte (Nachtrag 13).
 */
#ifndef RE15_PARKPLATZ_R39_H
#define RE15_PARKPLATZ_R39_H

#include <stdint.h>

#define RE15_R39O_RAUM            0x11C0u  /* ROOM11C0 = Leon (Elza ROOM11C1, @0x800397e4 Variante +1)            */
#define RE15_R39O_TUER_SLOT       0        /* main00 @0x01712 `3b 00 ..` pc[1] — die einzige Tuer des Raums        */
#define RE15_R39O_ZIEL_RAUM       0x1Bu    /* main00 @0x01712+23 = ROOM11B0                                       */
#define RE15_R39O_MSG_SPERRE      40       /* VERTRAG 1.2: 11C0 Id 40 (Original-Ids 0..9, Port 10..14 irons_tod)  */
#define RE15_R39O_SAT             0x31u    /* main00 @0x01715 pc[3] = ROOM11B0 sub01 @0x01219 pc[3]               */
#define RE15_R39O_MASKE           0xFFFFu  /* ROOM11B0 sub01 @0x01216 Nutzlast +2 (Maske des sce-1-Satzes)         */
#define RE15_R39O_SCE_MESSAGE     1        /* PTR_8007469c[1] = 0x80043084                                        */
#define RE15_R39O_SCE_TUER        2        /* PTR_8007469c[2] = 0x800430bc                                        */
#define RE15_R39O_SUB_SZENE       2        /* sub01 @0x01824 `04 0a 18 02`                                        */
#define RE15_R39O_SUB_ENDE        3        /* sub01 @0x01842 `04 ff 18 03`                                        */
#define RE15_R39O_ADA_BANK        4        /* sub02 @0x0184E `22 04 40 01` / sub01 @0x01820 `21 04 40 00`          */
#define RE15_R39O_ADA_BIT         64
#define RE15_R39O_ENDE_BANK       3        /* sub03 @0x01B02 `22 03 43 01` / sub01 @0x0182E / sub00 @0x017D0      */
#define RE15_R39O_ENDE_BIT        67
#define RE15_R39O_KILL_BANK       7        /* sub01 @0x01836/@0x0183E `21 07 ..` = re15_em_status_zone() Stage 1    */
#define RE15_R39O_KILL_1          0x60     /* sub00 @0x01784+7 / sub01 @0x01836                                   */
#define RE15_R39O_KILL_2          0x61     /* sub00 @0x01798+7 / sub01 @0x0183E                                   */
#define RE15_R39O_GORILLA_TYP     0x27u    /* sub00 @0x01784 pc[2]                                                */
#define RE15_R39O_ZIEL_X          (-25552) /* main00 @0x01712+14 `30 9c` (Nutzlast der Ruecknahme)                */
#define RE15_R39O_ZIEL_Y          0        /* main00 @0x01712+16 `00 00`                                          */
#define RE15_R39O_ZIEL_Z          (-25331) /* main00 @0x01712+18 `0d 9d`                                          */

/* Kampf laeuft: (4,64) gesetzt (Ada-Szene begonnen, sub02 @0x0184E) und (3,67) frei (Ende-Szene nicht begonnen). */
int re15_parkplatz_r39_kampf(void);
/* 1 = das geladene RDT traegt die erwarteten Bytes (FNV-Anker main00-Tuer + sub00..sub03), sonst bleibt alles Original. */
int re15_parkplatz_r39_anker_ok(void);

/* Raumaufbau NACH dem Init-Lauf (scd_room_setup.c Blockende, platform/pc/main.c Boot-/CONTINUE-Weg): Text 40 einsetzen;
 * bei Wiedereintritt im Kampf Slot 0 -> MESSAGE 40 (derselbe Aot_reset wie Programm SZENE). */
void re15_parkplatz_r39_install(uint16_t room_id);
/* op_evt_exec-Haken: sub02 -> Programm SZENE (nur bei (3,67)==0), sub03 -> Programm ENDE (nur ROOM11C0 mit gueltigen
 * Ankern), sonst orig. */
const uint8_t *re15_parkplatz_r39_evt_exec(uint16_t room_id, uint8_t sub_id, const uint8_t *orig);
/* Text-Bytes (Riegel): msg 40. */
const uint8_t *re15_parkplatz_r39_text(int *out_len);
/* Programm-Bytes (Riegel): 0 = SZENE, 1 = ENDE. */
const uint8_t *re15_parkplatz_r39_programm(int nr, int *out_len);

/* Messschiene (Riegel/Integration): wie oft die Programme gestartet, der Wiedereintritt gesperrt und die Stimme
 * eingereiht wurde. */
extern unsigned g_re15_r39o_szene_zaehler;
extern unsigned g_re15_r39o_ende_zaehler;
extern unsigned g_re15_r39o_eintritt_zaehler;

/* MESSHAKEN (env-gegatet, kein Spielverhalten; Muster Runde 38 F re15_dining_r38_messhaken), je Spielbild aus main.c:
 *   RE15_R39O_GORILLAS_STARR=1  im Kampf (Spieler frei) die Gorillas einfrieren (grid_id | 0x20 = Member_set 0x0C=0x30,
 *                               @0x800410b8; 11C0 sub02 @0x01A58 `34 0c 30 00` friert sie so in der Szene), damit ein
 *                               Messlauf zu Fuss lebend an die Tuer kommt (Lauf vor1: ohne das stirbt Leon unterwegs).
 *   RE15_R39O_GORILLAS_TOT=<n>  nach n freien Kampfbildern die Gorillas herausnehmen und GENAU ihre Kill-Bits setzen
 *                               ((7,0x60)/(7,0x61), STAGE1.BIN @0x8011b8e8-f4) -> sub01 startet sub03 wie im Original. */
void re15_parkplatz_r39_messhaken(void);

#endif
