/*
 * RE1.5 Rebuilt — LAGEPLAENE ZUM AUFSAMMELN (Runde 36 Spur N): je Polizei-Etage ein RE2-Lageplan an
 * der Wand; Aufnehmen schaltet das Kartenblatt der Etage frei wie in Resident Evil 2.
 * Dossier mit allen Messungen: analysis/befunde_runde36/N_kartenitems.md.
 *
 * NUTZER-VORGABE (AUFTRAG.md Z. 73-77, woertlich): "Extrahiere mir Karte01 oder Karte02 ... Haenge sie
 *   mir beispielhaft an die Stelle von map3F.bmp, und schalte damit die Map fuer Etage 3F frei, wie das
 *   Kartensystem in Resident Evil 2 auch funktioniert. Auch mit den Text." + 2F ROOM10D0 (map2F.bmp),
 *   1F ROOM1030 (map1F.bmp), B2 ROOM1200 (mapB2.bmp), B1 ROOM11D0 (mapB1.bmp).
 *   ⛔ map3F.bmp ist ROOM1140 Cut 5 (Bildvergleich 8,5 gegen >= 113,8 fuer alle ROOM1150-Cuts), nicht
 *   ROOM1150 wie AUFTRAG.md annahm; Blatt 4 = 3F (re15_map_zones.h Zeile 0x1140).
 *
 * WAS DAS ORIGINAL VORGIBT (selbst disassembliert / aus den RDT-Bytes):
 *   FORM DER AUFNAHME — RE1.5 kennt sie selbst: ROOM5030 sub02 (Laborplan) @0x010AC
 *     `2b 00 ff ff` Frage, `02 00`, `06 00 1e 00` Ifel_ck, `21 0c 1f 00` Ck(12,31,0) = Ja,
 *     `46 05 ..` Aot_reset(5,0), `09 0a 01 00` Sleep 1, `22 03 73 01` Set, `2b 01 ff ff` "You've taken
 *     the laboratory map.", `02 00`, `08 00`, `01 00`; Ausloeser sub00 @0x0108A Aot_set Slot 5 sce 3
 *     sat 0x31 p0 0x00FF p1 0x0218.
 *   RE2 (Ziel, wo RE1.5 nichts hat): room20B0 sub10 @0x037BA..0x0381A — dieselbe Folge plus Besitz-
 *     Bits Set(33,2..4) @0x037DA-E2, Etagen-Wahl Set(35,2..4) @0x037E6-EE und das Wegraeumen des
 *     Weltmodells `2e 04 08` Work_set(Objekt 8) @0x037F2 + `32 00 20 4e 20 4e 20 4e` Pos_set
 *     (20000,20000,20000) @0x037F6, dann `02 00` @0x037FE und die Meldung @0x03800.
 *   ZEICHNER — RE2 FUN_8006E120: Bank 33 (0x800D4924, Zeigertabelle 0x800A78C8 + 33*4 = 0x800A794C)
 *     `jal 0x80077360` @0x8006E66C; mit Plan unbesuchte Raeume in CLUT-Zeile 498 (`li s5,0x1f2`
 *     @0x8006E71C), ohne Plan uebersprungen (`beq v0,zero,0x8006e768` @0x8006E744). Der Port hat das
 *     Gatter schon (re15_map_owned_page); diese Datei liefert die Fundstellen der Polizei-Blaetter.
 *   ETAGENWAHL — RE2 Bank 35 (0x800D4908): `jal 0x80077360` @0x8006D934 / @0x8006D998; die Aufnahme
 *     setzt sie (room20B0 @0x037E6-EE). Port: re15_map_blatt_waehlbar fragt den Besitz mit ab.
 *   TEXT — RE2 room2130 sub00 / sub12 (Bytes Dossier 2.4), RE1.5-Rahmen ROOM2030 msg 2/3.
 *   LICHT — RE1.5 FUN_8002c18c (Objekt-Zeichner): `lw v0,0xc(s1)` @0x8002c2ec, `andi v0,v0,0x10`
 *     @0x8002c2f4, `beq` @0x8002c2f8: Attribut-Bit 0x10 (Pool+0x0c = Obj_model_set pc[8..9],
 *     LAB_80040914 @0x800409a8/@0x800409b0) -> FLACH ohne Raumlicht ueber FUN_80022f0c @0x8002c320
 *     (a2 = obj+0x70 = 0x808080 aus FUN_8002b898.c:16 = Textur unveraendert), sonst NCCT. RE2 zeichnet
 *     seine Wandplaene genau so: room20B0 obj 8 @0x03158 und room2130 obj 0 @0x00FDC tragen das
 *     Attributwort 0x0010 (FUN_800363f0.c:41-54 `puVar4[4] & 0x10` -> FUN_8002d0e8/FUN_8002d244
 *     unbeleuchtet; FUN_80035710.c:16 0x808080). In RE1.5 tragen 260 Objekte in 46 Raeumen dasselbe
 *     Bit, darunter ROOM1200 obj 0/1 (@0x007DA/@0x00804) an derselben Wand wie die B2-Karte. Die
 *     Karte erhaelt deshalb attr = RE15_R36K_PROP_ATTR; der Prop-Zeichner (platform/pc/main.c) fuehrt
 *     den Flach-Zweig fuer JEDES Objekt mit dem Bit (Nachbesserung 1, Dossier Abschnitt 8).
 *
 * PORT-WAHL (Messung, Dossier 3): Ebene, Mitte, Massstab (Karte innerhalb der Marke, 3:2), Drehung,
 *   Aufhebe-Rechteck 1000 x 1000 um Mitte + 300 Einheiten zur Raumseite — alles erzeugt von
 *   re15_port/tools/r36_kartenitems/karten_export.py -> engine/src/gen/kartenitems_r36.inc.
 *
 * BANK-9-BITS "Plan genommen" (Nachtrag kartenbits, Dossier Abschnitt 9): 3F = 87, 2F = 88, 1F = 89,
 *   B2 = 77, B1 = 78. Bank 9 = 0x800b1078 (Flag-Tabelle 0x80074664[9] @0x80074688) ist im Original die Zone
 *   der Item-TAKEN-Bits: Item_aot_set @0x80040644 liest das Bit als u16 pc[18] (@0x80040680, Langform pc[26]
 *   @0x80040670) und legt den Satz still, wenn es steht (`jal 0x8004efe4` @0x800406dc, `sb zero,0(v0)`
 *   @0x800406f4); Ck @0x8003fcf4 / Set @0x8003fdd0 treffen ueber dieselbe Tabelle dasselbe Bit. Ein
 *   Lageplan-Bit darf deshalb KEIN Original-Bit sein. Zensus (analysis/befunde_runde36/N_belege/
 *   bank9_zensus.txt): 85 von 256 Bits belegt, frei u.a. 53..84 und 87..89. B2/B1 lagen bis zum Nachtrag
 *   auf 90/91 = ROOM2090/2091 main00 @0x00A12 Item 0x3F (Bit 0x5a) und ROOM2040/2041 main00 @0x01364
 *   Item 0x32 (Bit 0x5b) — wer den Plan nahm, fand diese Items in Stage 2 nicht. 77/78: im Original frei,
 *   in keiner Port-Version je benutzt (git log --all, Tags v0.8.21-23). NICHT frei, obwohl im VERTRAG 1.1
 *   als Reserve gefuehrt: 85 (Green Keycard ROOM3040 @0x014B2), 86 (Spark Plug ROOM4040 @0x00F66),
 *   92 (Super Redhawk ROOM3050 @0x01EC6). Riegel: unit_r36_kartenbits.
 */
#ifndef RE15_KARTENITEMS_R36_H
#define RE15_KARTENITEMS_R36_H

#include <stdint.h>

typedef struct {
    uint16_t raum;             /* Leon-Variante; Elza = raum | 1 (derselbe Ort)                        */
    uint8_t  blatt;            /* Kartenblatt (re15_map_zones.h): 4 = 3F, 3 = 2F, 2 = 1F, 1 = B2, 0 = B1 */
    uint8_t  bit;              /* Bank-9-Bit: 87/88/89/77/78 (Kopf BANK-9-BITS; nie ein Original-Bit)   */
    uint8_t  aot_slot;         /* Slot des Aufhebe-Ereignisses                                         */
    uint8_t  verdraengt_nach;  /* 0xFF = keiner; sonst: der Satz in aot_slot zieht dorthin um (Vorrang) */
    int32_t  x, y, z;          /* Prop-Ursprung (Welt)                                                  */
    int16_t  rot_x, rot_y, rot_z;   /* Pool +0x68/+0x6A/+0x6C (scd_vm.c Member 3..5)                    */
    int32_t  rect_x, rect_z;   /* Ecke des Aufhebe-Rechtecks, Groesse RE15_R36K_RECT                    */
} re15_kartenitem_r36_t;

#define RE15_R36K_ANZAHL       5
#define RE15_R36K_BANK         9         /* Bank-9-Bits (gespeichert mit g_game.flags)                   */
#define RE15_R36K_EREIGNIS     28        /* VERTRAG.md 1.3 Spur N                                       */
#define RE15_R36K_OBJ          15        /* Prop-Pool-Id; TIM-Slot RE15_TIM_SLOT_PROP(15) = 35; kein Raum
                                          * der fuenf hat nOmodel > 8, kein Port-Prop nutzt 8..16         */
#define RE15_R36K_MSG_FRAGE    30        /* VERTRAG.md 1.2 (ROOM1150-Liste: 28/29 belegt -> 30/31)      */
#define RE15_R36K_MSG_GENOMMEN 31
#define RE15_R36K_RECT         1000      /* r35-Muster: haeufigste Item-Groesse (69 von 162)            */
#define RE15_R36K_AOT_SCE      3         /* EVENT wie ROOM5030 sub00 @0x0108A (`2c 05 03 ..`)            */
#define RE15_R36K_AOT_SAT      0x31      /* Aktion+vorne+Spieler wie ROOM5030 (0xB1 = 0x31|Viereck)      */
#define RE15_R36K_AOT_P0       0x00FFu   /* ROOM5030 sub00 Payload +0 `ff 00`                            */
#define RE15_R36K_PROP_FLAGS   0x000B    /* 0x000A | 1 (@0x80040998), wie r35 RE15_R35I_PROP_FLAGS       */
#define RE15_R36K_PROP_ATTR    0x0010    /* Pool+0x0c Flach-Bit (Obj_model_set pc[8..9] @0x800409a8/b0):
                                          * RE2 room20B0 obj 8 @0x03158 / room2130 obj 0 @0x00FDC = 0x0010;
                                          * RE1.5 ROOM1200 obj 0/1 @0x007DA/@0x00804 = 0x0010; Zeichner
                                          * FUN_8002c18c @0x8002c2f4 -> FUN_80022f0c flach, 0x808080      */
#define RE15_R36K_PROP_BAND    1         /* wie r35 RE15_R35I_PROP_BAND (100 von 121 Item-Props)          */

/* Prop + Aufhebe-Zone beim Raumstart anlegen (nur solange das Bank-9-Bit fehlt). Gerufen an BEIDEN
 * Raumstart-Wegen (scd_room_setup.c, platform/pc/main.c Boot/CONTINUE) nach dem Init-Lauf von main00.
 * Idempotent; tut in jedem anderen Raum nichts. */
void re15_kartenitems_r36_install(uint16_t room_id);

/* HAKEN scd_event_fire: das Port-Programm fuer (Kartenraum, Ereignis 28), solange die Karte haengt;
 * setzt dabei die Nachrichten 30/31 des Raums. Sonst NULL. */
const uint8_t *re15_kartenitems_r36_ereignis(uint16_t room_id, uint8_t event_id);

/* Plattform-Lader: obj_id/MD1/TIM des Karten-Props in diesem Raum (beide Varianten), sonst -1/NULL. */
int            re15_kartenitems_r36_obj_id(uint16_t room_id);
const uint8_t *re15_kartenitems_r36_md1_bytes(uint16_t room_id, int *out_size);
const uint8_t *re15_kartenitems_r36_tim_bytes(uint16_t room_id, int *out_size);

/* HAKEN re15_map_owned.c: Besitz eines Kartenblatts aus den Bank-9-Bits 87/88/89/77/78 (Gegenstueck zu RE2
 * Bank 33 @0x8006E66C). */
int      re15_kartenitems_r36_blatt_besitz(unsigned page);
uint32_t re15_kartenitems_r36_besitz_bits(void);

/* Sonden/Riegel. */
int                          re15_kartenitems_r36_anzahl(void);
const re15_kartenitem_r36_t *re15_kartenitems_r36_zeile(int i);
const re15_kartenitem_r36_t *re15_kartenitems_r36_des_raums(uint16_t room_id);
const uint8_t *re15_kartenitems_r36_programm(uint16_t room_id, int *out_len);
/* welche: 0 = Frage (Nachricht 30), 1 = "You've taken ..." (31). */
const uint8_t *re15_kartenitems_r36_meldung(uint16_t room_id, int welche, int *out_len);

#endif /* RE15_KARTENITEMS_R36_H */
