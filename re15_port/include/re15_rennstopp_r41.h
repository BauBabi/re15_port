/*
 * RE1.5 Rebuilt — Runde 41 Spur E "kleinkram_b", Nachbesserung 2 (Abnahme 1 Mangel 1/2, master-AUFTRAG E-N2): der
 * RENN-STOPP. Laesst der Spieler beim Rennen los, laeuft Leon im Original mit abnehmendem Tempo aus; der Port blieb
 * sofort stehen.
 *
 * GEMESSEN (DuckStation-GDB, Dossier "Nachbesserung 2", Lage +0x34/+0x3c, Tempo +0x8c = DAT_800acae0, Mischzaehler +0x8f):
 *   freie Strecke (o_rennstopp.txt): 12 Bilder Rennen nach +x, loslassen -> `cmd 1/2/3/0` Clip 3 Bild 0, Schritte
 *   157, 117, 77, 37, -3 (Tempo 197 -> 157 -> ... -> -3), dann `cmd 1/0/0/0` / `1/0/1/0` Stehen.
 *   Kurbel ROOM2020 im Rennen (o_renn2.txt): nach dem Ja Pos_set @0x0A8C (3650,-2925), dann im Spieler-Takt desselben
 *   Bilds der Stopp: 174, 134, 94, 54, 14, -26 (Tempo 214 -> -26), Leon kurbelt bei (3650,-3369).
 * ORIGINAL (selbst disassembliert): Kommando 1 (0x80073f90[1] = 0x80031de8), Unterzustand 2 = Rennen,
 *   DECIDE 0x80073fb0[2] = 0x80032604: +6 < 2 (@0x800326e0-@0x800326ec) und NICHT (gehalten & 0x201) == 0x201
 *   (OBEN + RENNEN, @0x800326f4-@0x80032700) oder Zielen 0x100 mit Waffe (@0x80032708-@0x80032720) ->
 *   @0x8003272c `sb 2,+6`. +6 >= 2 (@0x80032730-@0x80032760): OBEN -> 0x0101 (Gehen), UNTEN -> 0x0401 (Rueckwaerts),
 *   LINKS/RECHTS -> 0x0301 (Drehen) per @0x80032760 `sw DAT_800aca58`, sonst weiter auslaufen.
 *   ANIMATE 0x80073ff0[2] = 0x80032778: Fall 2 @0x800328f8-@0x80032910 +6 = 3, Clip 3 (+0x94), +0xe3 = 7 (Mischung) und
 *   weiter in Fall 3 @0x80032918: anim_set, @0x80032930-@0x80032944 Tempo -= 40, @0x8003294c `bgez` / @0x80032958
 *   `sw 1,DAT_800aca58` (Stehen) bei Tempo < 0, @0x8003295c `jal 0x800245d8` Vorschub um das Tempo — auch den letzten,
 *   negativen Schritt.
 * PORT: Das Tempo des Rennens ist im Port konstant RUN_SPEED_PER_FRAME (player_common.c, Tabelle @0x80073f24 = 200);
 *   das Original fuehrt es je Bild ueber den Clip (gemessen 159..214). Der Stopp beginnt darum mit dem Port-Tempo
 *   (200 -> Schritte 160, 120, 80, 40, 0, -40) — Abzug, Ende, Abbruch und Clip sind die des Originals. Waehrend des
 *   Stopps kein Zielen (R1 wird erst im Stehen wieder gelesen, wie im Original).
 *   Skript-Modus (Port player_mode 2, Pad gesperrt — Kurbel nach Set(2,7)/(1,27) @0x0A80/@0x0A84): das Original
 *   laesst den Spieler-Takt mit leerem Pad laufen, der Stopp laeuft durch; der Port setzte das Rennen sofort auf Stehen.
 */
#ifndef RE15_RENNSTOPP_R41_H
#define RE15_RENNSTOPP_R41_H

#include <stdint.h>
#include "re15_actor.h"

/* Bewegungs-Block des Spielers, je Spieler-Takt, VOR der Bewegungswahl: 1 = dieses Bild gehoert dem Renn-Stopp
 * (Leon ist um das Tempo verschoben, Bewegung = steh_motion), dann waehlt der Aufrufer Stehen und zielt nicht.
 * lauf_motion/steh_motion = die Port-Bewegungen fuer Rennen/Stehen, lauf_tempo = Port-Renntempo, waffe = Waffe in
 * der Hand (DAT_800aca5d != 0). */
int re15_rennstopp_r41_takt(re15_actor_t *p, uint16_t pad_bits, int lauf_motion, int steh_motion,
                            int32_t lauf_tempo, int waffe);

/* Skript-Zweig (player_mode 2, ohne Pad): derselbe Stopp mit leerem Pad. 1 = Stopp aktiv (nichts weiter tun). */
int re15_rennstopp_r41_skript(re15_actor_t *p, int lauf_motion, int steh_motion, int32_t lauf_tempo);

/* Pruefhaken */
int     re15_rennstopp_r41_aktiv(void);
int32_t re15_rennstopp_r41_tempo(void);
void    re15_rennstopp_r41_zuruecksetzen(void);

#endif /* RE15_RENNSTOPP_R41_H */
