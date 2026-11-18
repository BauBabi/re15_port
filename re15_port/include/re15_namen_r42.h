/*
 * RE1.5 Rebuilt — Runde 42 Strang "namen", Stufe 2: Mod-Fortsetzungszeilen in ROOM10D0/1150/2000 am Satzende der Aufnahme geteilt,
 * jeder Untertitel mit Sprechernamen.
 *
 * NUTZER 18.11.2026 (woertlich): "Du hast bei den übernommenen Sätzen aus Resident Evil 2 nicht den Namen in den Dialog mit davor
 * geschrieben wie bei den rest auch...." Memory reai-v2-sprechername-vor-satz: jeder gesprochene Satz traegt den Sprechernamen wie
 * die uebrigen Untertitel, Systemtexte nicht.
 *
 * BEFUND (Zensus scratchpad/sprechernamen/BERICHT.md Teil B): fuenf Mod-Saetze stehen auf zwei RDT-Nachrichten, nur die erste mit
 * Namen, der Schnitt mitten im Satz - die Aufnahmen synchro/STAGEn/roomXXXX/mainNN.wav (eine je Nachricht, Nutzer-Produktion)
 * trennen am Satzende:
 *   ROOM10D0 msg 18/19 Marvin (@0x02212/@0x02262), msg 24/25 Leon (@0x0241A/@0x02472)
 *   ROOM1150 msg 12/13 Irons  (@0x016B9/@0x0170E)
 *   ROOM2000 msg 4/5 Leon     (@0x01C9A/@0x01CEB), msg 7/8 Ada (@0x01D59/@0x01DA6)
 * NEU: jede Nachricht zeigt genau den Satz ihrer Aufnahme und beginnt mit dem Namenskopf der ersten (`04 00 05 <Farbe> <Name> 16
 * 05 00 00`). Wortlaut der Mod Glyphe fuer Glyphe; an der Trennstelle Satzende der Aufnahme + Grossbuchstabe (PORT-WAHL mit Zensus,
 * Kopf von tools/r42_namen/fortsetzung_bauen.py). Zeilenwechsel der Mod bleiben, Schwanz `04 01 01 N` je Nachricht aus dem RDT.
 *
 * WIE: re15_namen_r42_texte() setzt die zehn Texte (gen/namen_r42_texte.inc) samt Dauer nach dem Laden des Nachrichtenblocks ein
 * (Haken in msg_common.c re15_msg_load_room_block, wie story6_r42.c). Die RDTs bleiben bytegleich. Ablauf, Stimmen und Sleeps der
 * Skripte sind unberuehrt (10D0 sub21 Message_on 18 @0x01B5C / 19 @0x01B78, 24 @0x01C8E / 25 @0x01CBE; 1150 sub08 12 @0x01252 /
 * 13 @0x01262; 2000 sub02 4 @0x018C2 / 5 @0x018EC, 7 @0x01926 / 8 @0x0197C).
 * Elza-Raeume 10D1/1151/2001: keine dieser Nachrichten, keine Fortsetzungszeile (geprueft, Kopf des Werkzeugs).
 */
#ifndef RE15_NAMEN_R42_H
#define RE15_NAMEN_R42_H

#include <stdint.h>

/* Haken in re15_msg_load_room_block NACH den RDT-Texten: setzt in ROOM10D0/1150/2000 die neu geteilten Zeilen ein. */
void re15_namen_r42_texte(uint16_t room_id);

/* Text einer neu geteilten Zeile (fuer Riegel), sonst NULL. */
const uint8_t *re15_namen_r42_text(uint16_t room_id, uint8_t id, int *out_len);

#endif
