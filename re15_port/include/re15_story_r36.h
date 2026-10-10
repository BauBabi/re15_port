/*
 * RE1.5 Rebuilt — Story Leon/Elza (Runde 36, Spur P "story", Bau-Teil).
 *
 * NUTZER-VORGABEN (woertlich, analysis/befunde_runde36/AUFTRAG.md Nachtrag 14:00):
 *   P1 "Ja, wenn es mehr sinn macht, das Roy erst da liegt, wenn der Strom eingeschaltet wurde, dann
 *      machen wir das so."
 *   P2 "bei Leon ein Text an Elzas Zelle (The cell door is open...) - die leere Zelle als Spur."
 *   P3 "ROOM1090. Elzas 1091 ist bytegleich Leons Zustand nach der Rettung - Leon war zuerst dort.
 *      Hinnehmen und eine Spur ergaenzen (burnt-out shed)"
 *   P4 "Die duerfen sich niemals begegnet sein, das heisst - alle Textstellen, die darauf hindeuten
 *      das sie sich begegnet waeren, muessen wir umschreiben."
 * Dossier mit allen Belegen und der alt->neu-Tabelle: analysis/befunde_runde36/P_story.md.
 *
 * KEIN Asset-Patch: die RDTs bleiben byte-true. Der Port
 *   - versteckt nach dem Init-Lauf von main00 Roys Leiche und legt ihren Anklick-Platz still (P1),
 *   - legt in ROOM1210/ROOM1091 je einen Text-Platz (sce 1) mit eigener Nachricht Id 20 an (P2/P3),
 *   - ersetzt nach dem Laden des Nachrichtenblocks einzelne Nachrichten (P4).
 * Wortlaut aller Texte = PORT-WAHL (Story); die FORM (Kopf/Farbe/Schwanz, Glyphen, Umbruch) ist aus
 * den Original-Nachrichten uebernommen (tools/r36_p/texte_bauen.py -> gen/story_r36_texte.inc).
 */
#ifndef RE15_STORY_R36_H
#define RE15_STORY_R36_H

#include <stdint.h>
#include <stddef.h>

/* ---- P1: Roys Leiche in Leons ROOM1020 erst mit Strom ------------------------------------------
 * ROOM1020.RDT main00 @0x01DEE `2d 06 00 00 01 00 0a 00 00 00 78 ee 00 00 44 d6 00 00 00 0c ...`
 *   = Obj_model_set obj 6 (-4488,0,-10684) rot y 3072, UNBEDINGT (erster Ifel_ck erst @0x01E10).
 * ROOM1020.RDT main00 @0x01DDA `2c 05 03 31 00 00 0e ed 8c d8 e8 03 e8 03 ff 00 18 02 00 00`
 *   = Aot_set Slot 5, sce 3 -> sub02 (@0x01F86 `2b 00 ff ff` = msg 0 "It's the RPD officer, Roy...").
 * Strom = (4,0xF3): einziger Setzer ROOM11F0/11F1 sub18 @0x016F6 `22 04 f3 01` (Generator); der Port
 *   fuehrt dasselbe Bit (gemessen: unit_r31_generator "sub18 (4:243) im Bild 50").
 * Stilllegen = genau das Aot_reset, mit dem das Original den Platz waehrend der Szene stilllegt:
 *   ROOM1020 sub02 @0x01F70 `46 05 00 00 00 00 00 00 00 00` (Slot 5, sce 0 = Handler[0] inert).
 * Verstecken = Zeichen-Bit des Pool-Eintrags loeschen (scd_prop_hide_by_obj_id, @0x80021fc8).
 * NUR Leon (0x1020): Elzas ROOM1021 legt dieselbe Leiche (@0x01E40) selbst unter Ck(3,0x5F) an. */
#define RE15_S36_ROY_RAUM          0x1020u
#define RE15_S36_ROY_OBJ           6        /* @0x01DEE pc[1] */
#define RE15_S36_ROY_SLOT          5        /* @0x01DDA pc[1] */
#define RE15_S36_STROM_BANK        4        /* @0x016F6 pc[1] */
#define RE15_S36_STROM_BIT         0xF3     /* @0x016F6 pc[2..3] = 0x00F3 */

/* ---- P2: Leon ROOM1210 — Text an der leeren Zelle ----------------------------------------------
 * Vorlage = Elzas Zellentuer-Platz ROOM1211.RDT sub00 @0x01DEE
 *   `2c 08 01 31 00 00 5c aa 00 a8 0e 03 d4 06 0a 00 ff ff 00 00` (Slot 8, sce 1, Flags 0x31, Band 0,
 *   Rechteck (-21924,-22528,782,1748), msg 10, Pausemaske 0xffff) — dort nur, solange (4,0xEC)==0.
 * Uebernommen: alles ausser Slot (8 -> 9 = erster freier Slot nach dem Raumstart von ROOM1210:
 *   0..8 belegt, 49..63 Kamerazonen; gemessen probe_r30_irons-diary-welt aots 1210) und msg
 *   (10 -> 20 = Port-Id, VERTRAG 1.2 "ROOM1210 (Leon, RDT-Zahl 0) | P | 20..21").
 * ROOM1210.RDT hat keinen Nachrichtenblock (RDT+0x3C = 0) -> Id 20 kollidiert mit nichts. */
#define RE15_S36_ZELLE_RAUM        0x1210u
#define RE15_S36_ZELLE_SLOT        9
#define RE15_S36_ZELLE_MSG         20

/* ---- P3: Elza ROOM1091 — Text am ausgebrannten Schuppen ----------------------------------------
 * Vorlage = Leons Feuer-Platz ROOM1090.RDT sub00 @0x0230E (28 B, Viereck-Form, Flags 0xB1):
 *   `2c 02 01 b1 01 00 64 00 14 05 3a 07 66 08 de 0d 30 f8 08 07 10 f5 07 00 ff ff 00 00`
 *   (Slot 2, sce 1, Band 1, Viereck (100,1300) (1850,2150) (3550,-2000) (1800,-2800), msg 7
 *   "I must hurry up and get something to put out this fire to save that woman!").
 * Uebernommen: alles ausser Slot (2 -> 5: ROOM1091 belegt 0/1/2 + 4 (r35 Spur F Shotgun Shells),
 *   Kamerazonen ab 39; gemessen aots 1091) und msg (7 -> 20, VERTRAG 1.2 "ROOM1091 | P | 20..21").
 * ROOM1091.RDT hat keinen Nachrichtenblock. KEIN leerer Feuerloescher (Nachtrag 14:00). */
#define RE15_S36_SCHUPPEN_RAUM     0x1091u
#define RE15_S36_SCHUPPEN_SLOT     5
#define RE15_S36_SCHUPPEN_MSG      20

/* ---- P4: Text-Overrides (Tabelle gen/story_r36_texte.inc) ------------------------------------ */
typedef struct {
    uint16_t       raum;      /* g_current_room_id (Variante im letzten Nibble: xxx0 Leon, xxx1 Elza) */
    uint8_t        id;        /* Nachrichten-Id (Message_on pc[1] bzw. sce-1-Nutzlast)               */
    const uint8_t *bytes;     /* .msg-Rohbytes                                                         */
    uint8_t        len;
    uint32_t       off_alt;   /* RDT-Byte-Offset der ersetzten Original-Nachricht (0 = neue Id)        */
    const char    *alt;       /* dekodierter Original-Wortlaut ("" = neue Id)                          */
    const char    *neu;       /* dekodierter neuer Wortlaut (re15_msg_decode_text-Form)                */
} re15_story_r36_text_t;

/* P1-P3 beim Raumaufbau, NACH dem Init-Lauf von main00 (Leiche/Platz stehen dann im Pool).
 * Gerufen am Ende des Installer-Blocks von scd_room_reenter (scd_room_setup.c) und im Boot-/
 * CONTINUE-Block von platform/pc/main.c (der geht nicht durch scd_room_reenter). Tut in jedem
 * anderen Raum nichts. Legt die eigenen Nachrichten (Id 20) selbst ab. */
void re15_story_r36_install(uint16_t room_id);

/* P4 am ENDE von re15_msg_load_room_block (msg_common.c): ersetzt die Nachrichten des Raums laut
 * Tabelle (Text + Dauer). Muss NACH dem Laden laufen — der Tuer-Weg laedt die Tabelle erst nach
 * scd_room_reenter (room_common.c Schritt 7 vs. 11) und ueberschreibt dabei die Ids 0..n-1. */
void re15_story_r36_texte(uint16_t room_id);

/* Tabelle (Tests/Abnahme). */
const re15_story_r36_text_t *re15_story_r36_tabelle(int *n);

/* Letzte Entscheidung von P1 (Tests/Log): 1 = Roy liegt da (Strom an), 0 = versteckt, -1 = anderer Raum. */
int re15_story_r36_roy_sichtbar(void);

#endif /* RE15_STORY_R36_H */
