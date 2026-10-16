/*
 * RE1.5 Rebuilt — ELZAS VORSPANN (Runde 38 Spur E "elzaintro"). Dossier mit allen Messungen:
 * analysis/befunde_runde38/E_elzaintro.md.
 *
 * PUNKT 1 — "Bei Elzas Intro fehlt das Voiceover. Es ist das gleiche wie bei Leon." (AUFTRAG.md, Wortlaut)
 *   Port-Mechanismus: die Sprachausgabe sucht synchro/STAGE<n>/room<LAUFENDER RAUM>/main<ID>.wav
 *   (audio_pc.c re15_voice_load_clip). Elzas Montage ist ROOM1241 -> synchro/STAGE1/room1241/ gibt es nicht ->
 *   stumm. Kein Original-Mechanismus: RE1.5 hat keine englische Sprachausgabe, synchro/ ist eigene Produktion
 *   (scd_vm.c, Kopf von scd_queue_voice). Die Zuordnung ROOM1241 -> Leons Aufnahmen aus room1240 ist
 *   NUTZER-VORGABE; ihre FORM (Zeile fuer Zeile dieselbe Message-Id) belegen die Daten:
 *     - Nachrichtenblock ROOM1240 @0x62C / ROOM1241 @0x630 (RDT+0x3C), Offsettabelle beider
 *       [12,105,191,278,370,484], msg 0..5 byte-gleich (92/85/86/91/113/71 Byte bis 0x01-END),
 *       ROOM1240 msg0 @0x638 = ROOM1241 msg0 @0x63C = 04 00 1d 00 3e 45 56 3d ...
 *     - sub02 ruft in beiden Raeumen Message_on 0..5 in derselben Folge mit denselben Sleeps:
 *       ROOM1240 `2b 00 00 00` @0x057E, `2b 01` @0x0598, `2b 02` @0x05A0, `2b 03` @0x05BA, `2b 04` @0x05DA,
 *       `2b 05` @0x05FA; ROOM1241 je +4 (@0x0582 .. @0x05FE; sub02 beginnt dort mit `22 03 6f 01`
 *       `22 03 70 01` statt `22 03 8b 01`).
 *   Eigene Aufnahmen unter synchro/STAGE1/room1241/ haben Vorrang (Regel synchro/README.md "Datei da = sie
 *   spielt"); der Ersatz greift nur, wenn dort nichts liegt.
 *
 * PUNKT 2 — "Im blackscreen vom preintro Text, scheint die map durch vom Raum bei Elza"
 *   Gemessen: im Erzaehler-Schwarz (ROOM1031 sub15, Cut 13) steht der Lageplan 1F (Runde 36 Spur N,
 *   kartenitems_r36.c, Prop obj 15 bei (-1692,-2769,-23638)) als 8x6-Pixel-Feld x 91..98 / y 107..112 im Bild,
 *   F3..F440. Gegenprobe Bank-9-Bit 89 (Plan genommen -> kein Prop): Feld weg.
 *   ORIGINAL: der Erzaehler-Schnitt zeigt NUR das schwarze Standbild und den Text —
 *     - Cut 13 ist nur ueber `29 0d` ROOM1031 sub15 @0x02A74 erreichbar (einziges Cut_chg(13) im Raum; kein
 *       Cut_replace auf 13; keine RVD-Zone mit Ziel 13: Zonen ab RDT+0x28 = 0x228, Ziele 0..9);
 *     - Standbild BSS/ROOM1031/BG13.BSS = ROOM1170/BG07.BSS = ROOM1240/BG00.BSS (sha256 44cafcd7ff20b961...);
 *     - Kamera Cut 13 @0x200 pos (-12834,-3114,-9774) tgt (-7794,-2196,-22446) = Lobby-Kamera Cut 0 @0x60 =
 *       ROOM1170 Cut 7 @0x140; einzige Zone von Cut 13 @0x4F8 `01 ff 0d 00 31 c6 44 80 ...` byte-gleich mit
 *       ROOM1170 @0x370 (Cut 7) -> Cut 13 ist Elzas Gegenstueck zu Leons Erzaehler-Schnitt;
 *     - die Raum-Objekte entstehen in main00 nur im Zweig Ck(3,125)==1 (@0x01E50 `06 00 c0 01`,
 *       @0x01E54 `21 03 7d 01`: Sce_em_set x20 ab @0x01E5C, Obj_model_set @0x01FEC); der Erzaehler laeuft mit
 *       (3,125)==0 (@0x0207E `21 03 7d 00` -> Evt_exec sub15 @0x020A2), sub15 setzt (3,125) erst @0x02A68;
 *     - Elza steht auf dem Tuer-Ziel aus ROOM1241 main00 @0x051A (-26214,0,-3861) HINTER der Kamera
 *       (Blickrichtung (5040,918,-12672) * (Spieler - Kamera) < 0).
 *   Die Lobby-Kamera sieht den Port-Lageplan; das Original hat dort nichts. Der Port fuehrt fuer Leons
 *   Erzaehler-Schnitt (ROOM1170 Cut 7) bereits das Netz s_scene_black (main.c, render_pc.c: Vollbild-Schwarz
 *   UNTER dem Text). Dieselbe Regel gilt jetzt fuer Elzas Erzaehler-Schnitt ROOM1031 Cut 13 — dort NICHT
 *   redundant: sie deckt jeden Port-Prop der Lobby (Lageplan, kuenftige Gegenstaende) im Erzaehler ab.
 */
#ifndef RE15_ELZAINTRO_R38_H
#define RE15_ELZAINTRO_R38_H

#include <stddef.h>

#define RE15_ELZAINTRO_RAUM_ELZA   0x1241u   /* Elzas Montage (Tuerziel @0x0531 = 0x03 -> ROOM1031)  */
#define RE15_ELZAINTRO_RAUM_LEON   0x1240u   /* Leons Montage (Tuerziel @0x0531 = 0x17 -> ROOM1170)  */
#define RE15_ELZAINTRO_MSG_ERSTE   0         /* sub02 @0x0582 `2b 00 00 00`                         */
#define RE15_ELZAINTRO_MSG_LETZTE  5         /* sub02 @0x05FE `2b 05 00 00`; Offsettabelle: 6 Saetze */

/* Raum, dessen synchro-Ordner die Aufnahme fuer (room_id, msg_id) liefert, wenn room_id selbst keine hat:
 * ROOM1241 msg 0..5 -> ROOM1240. Sonst room_id. */
unsigned re15_elzaintro_r38_stimme_raum(unsigned room_id, int msg_id);

/* HAKEN audio_pc.c re15_voice_load_clip (nach den beiden eigenen Versuchen): schreibt den Ersatzpfad
 * "synchro/STAGE<n>/room<RAUM 4 Hex>/<stamm>.wav" nach out. 1 = es gibt einen Ersatz, 0 = keiner. */
int re15_elzaintro_r38_stimme_pfad(unsigned room_id, int msg_id, const char *stamm, char *out, size_t n);

/* HAKEN main.c (Netz s_scene_black): 1 = (Raum, Cut) ist ein Erzaehler-Schnitt des Vorspanns, dessen Bild
 * nur aus dem schwarzen Standbild und dem Text besteht — ROOM1170 Cut 7 (Leon) und ROOM1031 Cut 13 (Elza). */
int re15_elzaintro_r38_erzaehler_schwarz(unsigned room_id, int cut);

/* Pruefhaken: Zeilen der Erzaehler-Tabelle (Riegel unit_r38_elzaintro_*). */
int re15_elzaintro_r38_erzaehler_anzahl(void);
int re15_elzaintro_r38_erzaehler_zeile(int i, unsigned *room_id, int *cut, unsigned *scd_off);

#endif /* RE15_ELZAINTRO_R38_H */
