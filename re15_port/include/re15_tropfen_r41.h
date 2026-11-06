/* re15_tropfen_r41.h — Runde 41 Spur W "wasserblink", OFFEN 4: Deckentropfen ROOM2000/2001 wie die echte 40%-Disc (Variante B).
 *
 * ENTSCHEID (Orchestrator 2026-11-01, Variante B): Die Tropfen-Schleife startet im Port nicht. Begruendung ist die Nutzerregel
 * "Mod-Inhalt nur mit 80%-Beleg". Dossier analysis/befunde_runde41/W_wasserblink.md §2.1 und OFFEN 4, Abnahme W_abnahme_2.md §4/§5 und H1.
 *
 * BYTES (selbst durchlaufen, Laengen = s_opcode_sizes in scd_vm.c; Werkzeug W_werkzeug/abnahme2/wa2_scd.py):
 *   Repo-Baum (shared_assets/PSX/STAGE2, = info/Re1.5 = Fan-Mod):
 *     ROOM2000 main00 @0x0161E..@0x01784. Nach den drei Gitterstrahlen 0x0B @0x0174C/@0x0175C/@0x0176C stehen
 *       @0x0177C `04 ff 18 07` = Evt_exec(cond 0xFF, sub07) und @0x01780 `01 00` = Evt_end.
 *     ROOM2001 main00 ist dieselbe Stelle mit @0x0177C `04 ff 18 08` (sub08).
 *     Ziel sub07 (2000) @0x01B06 bzw. sub08 (2001) @0x01D32 ist die Tropfen-Schleife. Erste Zeile `3a 00 06 00 00 00 00 10` = Sce_espr_on
 *       Effekt 6 Sub 0 Skala 0x1000, y 0xD8F0 = -10000. Acht Punkte, For 180 dreimal und For 45 einmal, Goto @0x01BAE zurueck
 *       (585 Bilder je Umlauf). Effekt 6 Sub 0 ist der Deckentropfen (Routine 13 @0x80017990, Dossier §2.1 (a)).
 *     Zensus Opcode 0x04 und 0x18 ueber alle Programme von 2000/2001: sub07/sub08 startet NUR diese eine Stelle. Weitere Evt_exec stehen
 *       in 2000 sub00 @0x0179C (sub02) und sub02 @0x01A32 (sub03), in 2001 sub00 @0x017C6/@0x0181E (sub02/sub05) und sub02 @0x018DA (sub03).
 *       Effekt 6 Sub 0 kommt ausserhalb von sub07/sub08 in keinem Sce_espr_on vor.
 *   Version_40% (echte Disc, ROOM2000 = ROOM2001 Byte fuer Byte): main00 @0x01426 endet nach denselben drei 0x0B @0x01568/@0x01578/@0x01588
 *     mit @0x01598 `01 00`. Im ganzen SCD steht kein Opcode 0x04 und kein 0x18. Dieselbe Schleife liegt als sub03 @0x015B6..@0x01664 da,
 *     wird aber nie gestartet.
 *   80%-Video: Im Abschnitt ROOM2000 Cut 2 (rund 9 s) faellt kein Tropfen (W_abnahme_1.md §3.1, W_abnahme_2.md §5). Das ist ein schwacher
 *     Gegenbeleg, aber jedenfalls kein 80%-Beleg.
 *
 * WIRKUNG: Der Haken laesst genau diesen einen Evt_exec aus. Der Zeiger geht wie beim Opcode um 4 weiter, danach endet main00 mit `01 00`.
 *   Das ist der Ablauf der 40%-Disc (dort @0x01598 `01 00` direkt hinter den drei 0x0B). Dadurch verschwindet auch der Port-Fehler,
 *   dass der haengende Tropfen als pulsierender Bildpunkt zu sehen war: Cut 2 bei (83,0..1), Cut 5 bei (193,1..2), Cut 7 bei (169,15)
 *   (W_abnahme_2.md H1). Er hing, weil re15_esp.c floor_y auf die Spawn-Hoehe klemmt und esp_fx_dispatch keine Routine 13 kennt. Das bleibt
 *   unberuehrt, denn ohne Start entsteht kein Effekt 6 Sub 0 mehr.
 * EINGRENZUNG: Der Haken greift nur, wenn alles stimmt:
 *   Raum 0x2000 mit sub07 oder 0x2001 mit sub08;
 *   pc liegt in main00, main00 beginnt bei 0x0161E, pc steht bei 0x0177C;
 *   die vier Bytes sind `04 ff 18 07` bzw. `04 ff 18 08`;
 *   das Ziel liegt bei 0x01B06 bzw. 0x01D32 und beginnt mit `3a 00 06 00 00 00 00 10`.
 *   Effekt 6 allgemein bleibt, ebenso die Gitterstrahlen 0x0B, der Wasserlauf der Spur N (wasserlauf_r41.c) und 20B0/20B1.
 *   Die 28 anderen Repo-RDTs mit Routine-13-Zeilen (Dossier §2.1 (e)) gehoeren nicht zu diesem Entscheid.
 */
#ifndef RE15_TROPFEN_R41_H
#define RE15_TROPFEN_R41_H

#include <stdint.h>
#include "re15_rdt.h"

#define RE15_R41W_RAUM_2000        0x2000u
#define RE15_R41W_RAUM_2001        0x2001u
#define RE15_R41W_MAIN00_OFF       0x0161Eu   /* main00 2000/2001 (Repo-Baum)                        */
#define RE15_R41W_EVT_OFF          0x0177Cu   /* `04 ff 18 07` (2000) / `04 ff 18 08` (2001)         */
#define RE15_R41W_SUB_2000         0x07u      /* sub07 @0x01B06                                      */
#define RE15_R41W_SUB_2001         0x08u      /* sub08 @0x01D32                                      */
#define RE15_R41W_SUB_OFF_2000     0x01B06u
#define RE15_R41W_SUB_OFF_2001     0x01D32u

/* op_evt_exec (scd_vm.c), VOR dem Vorruecken des Zeigers: 1 = diesen Evt_exec auslassen (pc += 4, kein Start), 0 = Original. */
int re15_tropfen_r41_evt_exec_gestrichen(uint16_t room_id, const uint8_t *pc, const re15_rdt_t *rdt);
/* Pruefhaken: wie oft der Haken seit dem letzten Zuruecksetzen gegriffen hat. */
unsigned re15_tropfen_r41_zaehler(void);
void re15_tropfen_r41_reset(void);

#endif /* RE15_TROPFEN_R41_H */
