/*
 * RE1.5 Rebuilt — Rolltor ROOM1050/1051: erst die Sicherung einsetzen, dann laeuft das Tor.
 *
 * Runde 34 Nacht, Spur A. NUTZER-VORGABE (woertlich, analysis/befunde_runde34_nacht/AUFTRAG.md):
 *   "Ich moechte das das Tor in ROOM 1050 nicht mehr einfach so geoeffnet werden kann, sondern
 *    die CUT und der Text mit der Sicherung dort aktiviert werden soll. Also das Tor soll erst
 *    geoeffnet werden koennen, wenn die Sicherung eingesetzt werden. Fuer die Sicherung und
 *    Nachricht soll es eine Nahansicht geben."
 * Dossier mit allen Belegen: analysis/befunde_runde34_nacht/A_rolltor.md (§3 Mechanismus, §5 Bauplan,
 * §9 Umsetzung); Gegenpruefung A_rolltor.gegenpruefung.md.
 *
 * WAS AUSGELIEFERT IST (ROOM1050.RDT, Datei-Offsets):
 *   sub00 @0x0C1E `21 03 79 00` Ck(3,121,0) -> @0x0C22 Aot_set Slot 7 sce 3 sat 0x31,
 *         Rechteck (16800,-8950,800,800), Nutzlast `ff 00 18 02` = Ereignis 2 (sub02).
 *   sub02 @0x0CAC `2b 00 80 ff` Frage "It's a shutter switch. / Will you push it?" ->
 *         @0x0CB6 `21 0c 1f 00` Ja -> @0x0CBA `22 03 79 01` Tor offen -> Fahrt mit Ton.
 *   msg 2 @0x0ED2 "I need a fuse to run the shutter." — im SCD NIE aufgerufen (kein `2b 02`).
 *   Kamera Cut 7 @0x140 / Cut 8 @0x160: dieselbe Kamera, Bild Cut 7 = Sockel leer + rote
 *   Leuchte, Cut 8 = Sicherung drin + beide gruen; per RVD nie erreichbar (nur Anker-Saetze
 *   @0x32C/@0x340 auf einem Blindrechteck) — reine Skript-Cuts, heute unbenutzt.
 * Das Sicherungs-Raetsel ist hier HERAUSGESCHNITTEN (RE1.5 unfertig), in ROOM2060 aber
 * VOLLSTAENDIG ausgeliefert — dessen Form wird uebernommen (siehe rolltor_1050.c).
 *
 * WIE ES GEBAUT IST: KEIN Asset-Patch. Loest der Schalter Ereignis 2 aus, startet scd_event_fire
 * (EIN Haken, scd_vm.c) statt sub02 ein portseitiges Programm aus ORIGINAL-Opcodes, das die
 * vorhandene VM mit der Original-Zeitsemantik ausfuehrt. Ist die Sicherung eingesetzt
 * (Bank 9 Bit 63), laeuft wieder der ausgelieferte sub02 — unveraendert. Dazu EIN Port-Opcode:
 * 0x62 = RE2 Retail Sce_item_lost (Tabelle 0x800a74c8[0x62] -> 0x800585e4), nur innerhalb des
 * Port-Programms wirksam (PC-Schranke); fuer RDT-Bytecode bleibt 0x62 das, was es war
 * (op_unknown, Satzbreite 1, s_opcode_sizes[0x62]).
 */
#ifndef RE15_ROLLTOR_H
#define RE15_ROLLTOR_H

#include <stdint.h>
#include "re15_scd.h"

/* Die zwei Raeume. ROOM1051 (Elza) traegt dieselben Daten: sub02 @0x0CC8..0x0DA4 bytegleich zu
 * ROOM1050 sub02 @0x0CAC..0x0D88, Schalter-Zone @0x0C4C == @0x0C22, msg 0 @0x0DEC / msg 2
 * @0x0E68 gleich, Kameratabelle (10 Cuts), RVD und SCA gleich (tools/r34n_a/belege.py). */
#define RE15_ROLLTOR_RAUM_LEON        0x1050
#define RE15_ROLLTOR_RAUM_ELZA        0x1051

/* Das Ereignis des Schalters: Slot-7-Nutzlast `ff 00 18 02` (sub00 @0x0C30..@0x0C33, Byte 3 =
 * sub 2). sce-3-Handler 0x800430f0: @0x80043100 `lbu a1,3(v0)` = sub, @0x80043104 `jal 0x8003ee3c`. */
#define RE15_ROLLTOR_EREIGNIS         2

/* "Tor offen": sub00 @0x0C1E `21 03 79 00` (installiert den Schalter nur, solange 0),
 * sub02 @0x0CBA `22 03 79 01` (setzt es beim Oeffnen). */
#define RE15_ROLLTOR_OFFEN_BANK       3
#define RE15_ROLLTOR_OFFEN_BIT        121

/* "Sicherung eingesetzt" — PORT-WAHL (VERTRAG Runde 34 Nacht §1.1: Spur A = Bank-9-Bits 63/64;
 * RE1.5 kennt in STAGE1 keine Sicherung). Bank 9 = 0x800b1078 (Flag-Tabelle 0x80074664[9]),
 * liegt in g_game.flags und wird komplett gespeichert (re15_savedata.c capture/restore memcpy).
 * Rolle wie ROOM2060 (3,144): sub19 @0x0169E `22 03 90 01`. Bit 64 bleibt Reserve. */
#define RE15_ROLLTOR_EINGESETZT_BANK  9
#define RE15_ROLLTOR_EINGESETZT_BIT   63

/* Portseitige Nachrichten (VERTRAG §1.3: Spur A = 20/21; ROOM1050/1051 haben msg 0..8). */
#define RE15_ROLLTOR_MSG_FRAGE        20   /* = ROOM2060 msg 4 @0x1855 "Will you use the Fuse?" */
#define RE15_ROLLTOR_MSG_BENUTZT      21   /* = ROOM2060 msg 5 @0x1875 "You've used the Fuse."  */

/* Port-Opcode: RE2 Retail Sce_item_lost, 2 Byte [0x62, Item-Id] (@0x800585fc `lbu a0,1(v0)`,
 * @0x80058644 `addiu v1,v1,2`). */
#define RE15_ROLLTOR_OP_ITEM_LOST     0x62

/* HAKEN scd_event_fire: liefert das Port-Programm, das statt sub_scd[ereignis] laufen soll, oder
 * NULL (dann laeuft das ausgelieferte Unterprogramm). Nicht-NULL nur in ROOM1050/1051 fuer
 * Ereignis 2, solange (9,63)=0 und (3,121)=0. Setzt dabei die Texte 20/21 ein. */
const uint8_t *re15_rolltor_ereignis(uint16_t raum, uint8_t ereignis);

/* HAKEN register_opcodes: Handler fuer Opcode 0x62. */
int re15_rolltor_op_item_lost(scd_thread_t *t);

/* Zugaenge fuer Riegel/Sonde (kein Spielverhalten). mit = 1: Fassung "Sicherung im Inventar". */
const uint8_t *re15_rolltor_programm(int mit, int *len);
const uint8_t *re15_rolltor_meldung(uint8_t id, int *len);

#endif /* RE15_ROLLTOR_H */
