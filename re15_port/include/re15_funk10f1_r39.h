/*
 * RE1.5 Rebuilt — Runde 39 Spur I "funk10f1": die Funkszene in ROOM10F1 (Elza am Kommunikationssystem).
 *
 * NUTZER-VORGABE (woertlich, analysis/befunde_runde39/AUFTRAG.md):
 *   "In Room 10F1 möchte ich, das wenn Elza ihn betritt eine Cutscene läuft wo sie Rechts zum Kommunikationssystem geht
 *    und von der Animation her drauf drückt und sagt "Hello, anyone, can you hear me? We Need help!" / Dann kommt "..." /
 *    Dann "Uhh, this Thing is broken!". / Suche die dir passenden Animationen dafür heraus."
 * Dossier mit Messungen und Belegen: analysis/befunde_runde39/I_funk10f1.md.
 *
 * ⛔ PORT-WAHL AUF NUTZERWUNSCH. ROOM10F1 hat im Auslieferungsstand keine Szene (sub-Tabelle @0x10D4: 2 Subs, sub00 =
 * Tritt/Flag (3,102), sub01 = Member_cmp; main00 @0x00F32 nur Tueren/Texte/Items/Props) und keinen Animationsblock
 * (RDT+0x5C = 0). Gebaut wie die Szene ROOM10F0 (Runde 35 K, cut_10f0.c): ein portseitig eingespielter SCD-Bytecode aus
 * ORIGINAL-Opcodes (gen/funk10f1_r39_szene.inc, Generator tools/r39_funk10f1/szene_bauen.py mit Vorbild je Zeile), von der
 * vorhandenen VM ausgefuehrt — kein Asset-Patch. Die FORM ist Elzas eigene Szene ROOM1031 sub13 @0x02982..@0x02A58.
 *
 * ABLAUF: Raumaufbau (scd_room_setup.c, nach dem Init-Lauf) -> re15_funk10f1_r39_install setzt die drei Nachrichten ein
 * und feuert Ereignis 27, solange (9,70) = 0 -> scd_event_fire -> re15_funk10f1_r39_ereignis liefert das Programm. Das
 * Programm setzt (9,70) als ERSTES Opcode (Einmal-Riegel). Beim zweiten Betreten steht (9,70) -> keine Szene.
 * Gesten: ROOM10F1 hat keinen Block; solange die Szene aussteht, leiht der Raumlader den Block von ROOM1031 (Elzas
 * Bibliothek, platform/pc/src/cut10f0_pc.c re15_cut10f0_pc_rbj_leihen, Quelle re15_funk10f1_r39_rbj_quelle).
 */
#ifndef RE15_FUNK10F1_R39_H
#define RE15_FUNK10F1_R39_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Der Raum: ROOM10F1 = Elzas Communications Room (DEBUG.BIN "COMMUNIC. ROOM", Raum-Byte 0x0F; die niedrigste Hex-Ziffer
 * ist die Spielervariante, @0x800397e4 `srl a0,a0,31` + @0x800397ec `addu`). Leons ROOM10F0 bleibt unberuehrt. */
#define RE15_FUNK10F1_RAUM            0x10F1

/* "Szene gesehen" — VERTRAG Runde 39 §1.1: Spur I = Bank-9-Bit 70 (frei in Original und Port, Riegel
 * tests/unit/test_r36_kartenbits.c k_port[]). Bank 9 = 0x800b1078 (Flag-Tabelle 0x80074664[9]), gespeichert mit g_game.flags. */
#define RE15_FUNK10F1_GESEHEN_BANK    9
#define RE15_FUNK10F1_GESEHEN_BIT     70

/* Ereignis-Nummer: < RE15_RDT_MAX_SUB_SCD (32, Schranke in scd_event_fire); in keinem Port-Haken vergeben (benutzt: 2, 3, 4,
 * 5, 6, 13, 20..26, 28, 29 — grep RE15_*EREIGNIS in include/), ROOM10F1 hat nur sub00/sub01 (@0x10D4 `04 00`). */
#define RE15_FUNK10F1_EREIGNIS        27

/* Portseitige Nachrichten (VERTRAG Runde 39 §1.2: Spur I = 10F1 Ids 40..44). ROOM10F1-Nachrichtentabelle @0x1168:
 * 6 Eintraege (Id 0..5). Sprachdateien (Nutzer-Produktion, automatisch eingebunden, synchro/README.md):
 * synchro/STAGE1/room10F1/main40.wav, main41.wav, main42.wav. */
#define RE15_FUNK10F1_MSG_HALLO       40   /* "Elza: Hello, anyone, can you hear me? / We Need help!" */
#define RE15_FUNK10F1_MSG_PUNKTE      41   /* "Elza: ..."                                              */
#define RE15_FUNK10F1_MSG_KAPUTT      42   /* "Elza: Uhh, this Thing is broken!"                       */

/* Gestenbank: Record 0 (Marker Bit 0 = Spieler, Binder FUN_8001b3f8) des Blocks von ROOM1031 (RDT+0x5C = 0x307C,
 * 26760 B, 1 Record, 13 Clips [20,30,30,20,30,25,30,50,18,24,20,2,50], rbj_zensus.py). Daraus: Clip 11 (Haltepose "Arm
 * vorn", 2 Bilder) + Plc_flg 4 = Druck, Clip 0 rueckwaerts = Arm senken (ROOM1031 sub13 @0x029A4/@0x029A8/@0x029BE/
 * @0x029C2: Elza drueckt den Rolltor-Notschalter), Clip 9 = Hand an die Huefte (@0x02A32), Clip 12 = Hand an die Stirn
 * (@0x02A00). Dossier §2.3/§2.4. */
#define RE15_FUNK10F1_RBJ_RAUM        0x1031

/* Weg und Platz (PORT-WAHL nach dem Wortlaut "Rechts zum Kommunikationssystem", Dossier §2.1/§3.1):
 *   Eintritt  ROOM10D1 main00 @0x01174 Door_aot_set Slot 0 -> (8400,0,-350) Gierung 2048 (Blick -X); rechts von ihr = +Z.
 *   Konsole   Aot_set Slot 2 @0x00F72 msg 0 "A large communications device" x 7000..7800 z 2600..12300; Konsolenfuss =
 *             SCA-Zelle 10 x 7400..9300 z 2500..14750; Tische davor Zelle 13 (x 6800..8300 z 3800..5300), 12 (x 6600..8100
 *             z 7500..9000).
 *   Weg       (6250,2900) -> (6250,5900) -> Platz (6900,6400) zwischen den Tischen 13 und 12, Blick auf (7400,6400). */
#define RE15_FUNK10F1_PLATZ_X         6900
#define RE15_FUNK10F1_PLATZ_Z         6400
#define RE15_FUNK10F1_KONSOLE_X       7400

/* HAKEN scd_room_setup.c (nach dem Init-Lauf von main00, Ende des Installer-Blocks): ROOM10F1 mit (9,70) = 0 -> Texte
 * einsetzen und Ereignis 27 feuern; sonst nichts. */
void re15_funk10f1_r39_install(uint16_t room_id);
/* HAKEN scd_vm.c scd_event_fire: das Programm fuer (ROOM10F1, Ereignis 27), solange (9,70) = 0; sonst NULL. */
const uint8_t *re15_funk10f1_r39_ereignis(uint16_t room_id, uint8_t event_id);
/* HAKEN platform/pc/src/cut10f0_pc.c (Raumaufbau ohne eigenen Animationsblock): 0x1031, solange die Szene in ROOM10F1
 * aussteht; sonst 0. */
unsigned re15_funk10f1_r39_rbj_quelle(unsigned room_id);

/* Pruefhaken (Riegel): Programm, Nachrichten, Zustand. */
const uint8_t *re15_funk10f1_r39_programm(int *out_len);
const uint8_t *re15_funk10f1_r39_meldung(int msg_id, int *out_len);
int re15_funk10f1_r39_programm_laeuft(void);

#ifdef __cplusplus
}
#endif

#endif /* RE15_FUNK10F1_R39_H */
