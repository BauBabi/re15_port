/*
 * RE1.5 Rebuilt — Zielscheiben-Texte im Schiessstand ROOM1190/1191 (Runde 36 Spur H; Erstbau Runde 35).
 *
 * NUTZER-VORGABE (AUFTRAG.md Runde 36, woertlich): "Die Texte: 'This target has a surprisingly large
 * number of bullet holes.' und 'This target does not have many bullet holes' sollen sich hinten bei den
 * Zielscheiben befinden, wenn man sie anklickt, nicht vorne bei den Knoepfen, die die Zielscheiben
 * bewegen lassen... Bei Elza sind es genau die anderen zwei Zielscheiben, deshalb muessen die Texte da
 * genau vertauscht sein."  Dossier: analysis/befunde_runde36/H_ziel1190.md (Punkt 1).
 *
 * RUNDE 35 ZURUECKGENOMMEN: die Knoepfe (main00 Aot_set slot 0..3 @0x02226.., sce 5 -> sub02..05 msg 0
 * bzw. sce 1 msg 2) laufen wieder byte-true ueber den Original-Weg — kein Haken mehr in op_message_on.
 *
 * DIE SCHEIBEN: Obj 0..3 (sub13 Obj_model_set @RDT 0x0297E/0x029A0/0x029C2/0x029E4, in 1190 und 1191
 * bytegleich), x=-21340 (hinten), z=-24516/-20916/-17316/-13716. Die Knoepfe fahren Obj n bis x=-5220
 * vor den Tresen (sub06/sub08 Add_speed, sub00 Member_set @0x02336.. beim Betreten mit (4,n)=1).
 * Die Bahn ist begehbar (Durchgang am Tresenende, SCA r0[0] @0x01C1C endet bei z=-11504, Suedwand
 * r0[1] @0x01C28 bei z=-10280; gemessen: Leon steht vor S0..S2 bei x=-20845, vor S3 bei x=-20792).
 *
 * WELCHE SCHEIBE WELCHE IST (Kamera, Runde 35): von links nach rechts Obj 0,1,2,3 (Cut 5/6/7/9).
 *   Leon (ROOM1190): Obj 0 + 2 = "viele" (Nutzer "Scheibe 1 und 3"), Obj 1 + 3 = "wenige".
 *   Elza (ROOM1191): genau vertauscht. Das Raetsel bestaetigt es: Loesung A (Leon, (3,111)=0) = Obj 0+2
 *   vorn (sub01 @0x02438-0x02468), Loesung B (Elza, (3,112)=1 aus ROOM1241 @0x0055E) = Obj 1+3 vorn
 *   (sub01 @0x02478-0x024A8). Elza spielt die "1"-RDT: `addu a0,a0,v0` @0x800397ec.
 *
 * FORM (belegt) — vier Text-Plaetze, die an ihrer Scheibe HAENGEN:
 *   * sce 1 MESSAGE wie das Vorbild im selben Raum, main00 @0x0227A
 *     `2c 00 01 31 00 00 d0 ee 9c 9b 58 02 60 09 02 00 ff ff 00 00`: sat 0x31 (Aktion + Vorwaertspunkt
 *     620, FUN_80042bac `ori v0,zero,0x26c` @0x80042bd0), Band 0, w=600 (`58 02`), d=2400 (`60 09`),
 *     Nutzlast msg-Nummer + Pausenmaske 0xffff (Handler LAB_80043084, @0x80043098/@0x800430a4).
 *   * ANHAENGEN an Obj n = das "super"-Byte des Records (Aot_set pc[5]) mit Bit 0x80: FUN_80042bac
 *     `lbu v1,3(s0)` @0x80042cd4, Objekt-Pool 0x800b3f98 Stride 0x94, Versatz = Objekt-x/z
 *     (`lhu` 0x800b3fcc @0x80042d7c / 0x800b3fd4 @0x80042db4), Rechteck-Ecke = Versatz + rec x/z
 *     (@0x80042e60-0x80042e84). Der Port fuehrt dieses Byte fuer Plaetze nicht (kein ausgelieferter
 *     Record nutzt es, Zensus 240 RDT) — re15_ziel1190_folgen() rechnet DIESELBE Summe vor jedem Scan
 *     fuer genau diese vier Plaetze: Rechteck relativ zum Objekt = Mitte auf der Scheibe, +-300/+-1200.
 *   * Text: Kopf `04 02` + Satz (Zeilenumbruch 0x08) + Ende `01 00` = Form von ROOM1190 msg 2
 *     @0x2E5E ("I have nothing else to do" 08 "here." 01 00). Glyphen re15_msg_glyph.
 *
 * PORT-WAHL (gekennzeichnet): Platznummern 17..20 (erste freie nach dem letzten Raumplatz 16, sub14
 * @0x02D90; die RVD-Kamerazonen belegen die Plaetze von 63 abwaerts); Nachrichten-Ids 6/7 (VERTRAG 1.2);
 * Satzende "." auch beim zweiten Satz (Gleichlauf, Runde 35); Stimme synchro/STAGE1/room<Raum>/main06.wav
 * bzw. main07.wav (Muster leiche_1110_1230.c) — fehlt die Datei, stumm mit Untertitel.
 */
#ifndef RE15_ZIEL1190_H
#define RE15_ZIEL1190_H

#include <stdint.h>

#define RE15_ZIEL1190_RAUM        0x1190u  /* RE15_ROOM_BASE: 1190 (Leon) und 1191 (Elza)          */
#define RE15_ZIEL1190_MSG_VIELE    6u      /* Port-Id (VERTRAG 1.2: 1190/1191 -> 6..11)              */
#define RE15_ZIEL1190_MSG_WENIGE   7u      /* Port-Id                                               */
#define RE15_ZIEL1190_SLOT0       17       /* PORT-WAHL: Plaetze 17..20 = Scheibe 0..3              */
#define RE15_ZIEL1190_HALB_X      300      /* w=600 `58 02` main00 @0x02284 (Vorbild @0x0227A) / 2  */
#define RE15_ZIEL1190_HALB_Z      1200     /* d=2400 `60 09` main00 @0x02286 / 2                    */
#define RE15_ZIEL1190_SAT         0x31u    /* sat main00 @0x0227D                                   */
#define RE15_ZIEL1190_BAND        0u       /* floor main00 @0x0227E                                 */
#define RE15_ZIEL1190_PAUSE       0xFFFFu  /* Nutzlast u16@+2 main00 @0x0228A `ff ff`               */

/* 1 = Scheibe obj (0..3) traegt im Raum room_id "viele Einschuesse", 0 = "wenige", -1 = kein Fall. */
int re15_ziel1190_viele(unsigned room_id, int obj);

/* Baut den Text (Kopf 04 02 + Satz + 01 00). viele = 1/0. Liefert die Laenge, 0 bei zu kleinem cap. */
int re15_ziel1190_text(int viele, uint8_t *out, int cap);

/* Raumaufbau (scd_room_setup.c Installer-Block + main.c Boot-/CONTINUE-Weg): legt die vier
 * Text-Plaetze an ihre Scheiben. Tut in jedem anderen Raum nichts. */
void re15_ziel1190_install(uint16_t room_id);

/* Vor jedem AOT-Scan (aot_common.c re15_aot_scan): die vier Plaetze auf die aktuelle Lage ihrer
 * Scheibe setzen (super-Anbindung @0x80042cd4-0x80042e84). Tut in jedem anderen Raum nichts. */
void re15_ziel1190_folgen(void);

/* Haken in re15_scd_show_message (sce-1-Weg): Text 6/7 ablegen, Stimme, oeffnen. 1 = gezeigt. */
int re15_ziel1190_show(uint8_t index, uint32_t pause_mask);

#endif /* RE15_ZIEL1190_H */
