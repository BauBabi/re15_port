/*
 * RE1.5 Rebuilt — FEUERLOESCHER (Runde 37 Spur I, Nachtrag 4 des Nutzers, AUFTRAG.md):
 *   "Bitte erstelle mir zusaetzlich auch ein Modell des Feuerloeschers. Ich finde deine Idee, bei elza
 *    einen Leeren Feuerloescher am Boden zu haben gut. Der muesste dann bei Leon nach der Nutzung
 *    erstmalig auftauchen. Ausserdem finde ich den FEuerloescher im Spind bei Leon noch ungluecklich
 *    und wuerde ihn gerne woanders platzieren...."
 * Dossier mit allen Messungen: analysis/befunde_runde37/I_feuerloescher.md.
 *
 * WAS DAS ORIGINAL VORGIBT (selbst disassembliert, RDT-Bytes im Dossier):
 *   AUFNAHME  ROOM1000.RDT sub00 @0x00C24 `50 03 09 31 00 00 14 50 e0 fc 20 03 20 03 31 00 01 00 86 00 ff 00`
 *             = Item_aot_set Slot 3, sce 9, sat 0x31, Rechteck (20500,-800) 800x800, Item 0x31 x1,
 *             Taken-Bit (9,0x86) (pc[18], @0x80040680 `lhu a1,0x12(a2)`), tk_prop 0xFF = KEIN Modell
 *             (pc[20], @0x80040684 `lbu s1,0x14(a2)`).
 *   STORY     ROOM1000.RDT sub01 @0x00CFC: Ifel_ck Ck(9,0x86)==1 @0x00D00, Ifel_ck Ck(3,0x85)==0 @0x00D08,
 *             Set(3,0x85)=1 @0x00D0C. sub01 laeuft in JEDEM Spielbild (FUN_8003f038 @0x8003f064-84).
 *             ROOM1090 sub00 @0x02306 liest (3,0x85): =0 -> Slot 2 Text msg 7 @0x0230E, =1 -> Slot 2
 *             sce 3 -> sub06 (Loeschen) @0x02336 + Selbst-Tuer Slot 3 @0x02352.
 *   LOESCHEN  ROOM1090.RDT sub06 @0x02702: msg 7/8, Ifel_ck Ck(12,0x1F)==0 (= Ja) @0x0270E,
 *             Set(3,0x81)=1 @0x0271E, Set(3,0x84)=1 @0x02722, Aot_on 3 @0x02726 (Selbst-Tuer, Neuladen).
 *   ⛔ (3,0x84) IST EIN EINMAL-ZEICHEN: sub03 (Rettungsszene, von sub00 @0x022E0 gestartet) loescht es als
 *             ERSTES (@0x024CE `22 03 84 00`). DAUERHAFT ist (3,0x81) "Feuer aus": sub00 legt Feuer und
 *             brennende Kamerabilder nur bei Ck(3,0x81)==0 an (@0x021C8, @0x021EE, @0x022EA, @0x02332);
 *             Zensus aller 258 RDTs: Set(3,0x81) nur ROOM1090 sub06 (Leon) und ROOM10B1 (Elza-Variante,
 *             eigene Bedeutung). Der leere Loescher bei Leon haengt deshalb an (3,0x81), NICHT an (3,0x84).
 *   ELZA      ROOM1091 main00 @0x02202 obj 2 an Leons Nach-Rettungs-Lage, kein Feuer (Typ 0x26) — der
 *             Platz ist bei Elza schon geloescht; ROOM1001 Slot 3 traegt dort 30 Patronen (Item 0x15).
 *   MODELL    Item-Props des Originals: Obj_model_set Typ 0, Band 1, Flags 0x000A (|1 @0x80040998),
 *             Attribut pc[8..9] = 0x0000 -> NCCT-Raumlicht (FUN_8002c18c @0x8002c2f4 Bit 0x10 NICHT
 *             gesetzt). Belege: ROOM1010 @0x00930 (First Aid Spray), ROOM4050 @0x01E8A/@0x01F32
 *             (Antidote Spray), ROOM1000 @0x00C66/@0x00C88 (Medizinkoffer). Der Loescher bekommt dasselbe.
 *
 * PORT-WAHL (Dossier, Abschnitt "Zu entscheiden"):
 *   Modell (gen/feuerloescher_r37.inc, tools/r37_feuerloescher/feuerloescher_modell.py), Fundort
 *   ROOM1050 Cut 6 an der Wand rechts der Notausgangs-Tuer (zu ROOM1090), Lage des leeren Loeschers in
 *   ROOM1090/1091, Untersuchen-Text msg 29 "It's an empty fire extinguisher.", Spind ROOM1000 Slot 3
 *   -> der Raumtext msg 0 "Nothing unusual." (Satzform wie ROOM1000 sub00 @0x00CAA Slot 6).
 *   Leon ROOM1090: der leere Loescher liegt, sobald das Feuer aus ist (3,0x81) UND Leon ihn nicht mehr
 *   traegt. Die Port-Wegwerf-Abfrage nach msg 9 "You've used the Fire Extinguisher." (Runde 25,
 *   gen/discard_sites.inc Zeile ROOM1090/9/0x31) ist der Moment, in dem er ihn hinlegt; "Nein" =
 *   Leon behaelt ihn, dann liegt keiner da (sonst laege derselbe Loescher doppelt vor).
 */
#ifndef RE15_FEUERLOESCHER_R37_H
#define RE15_FEUERLOESCHER_R37_H

#include <stdint.h>

#define RE15_FL37_ITEM            0x31   /* ROOM1000.RDT @0x0C24 pc[14..15] `31 00`                       */
#define RE15_FL37_MENGE           1      /* @0x0C24 pc[16..17] `01 00`                                      */
#define RE15_FL37_TAKEN_BIT       0x86   /* @0x0C24 pc[18..19] `86 00` (Bank 9, Item_aot_set @0x80040680)   */
#define RE15_FL37_STORY_BANK      3
#define RE15_FL37_STORY_BIT       0x85   /* ROOM1000 sub01 @0x00D0C `22 03 85 01`                            */
#define RE15_FL37_FEUER_AUS_BIT   0x81   /* ROOM1090 sub06 @0x0271E `22 03 81 01` (dauerhaft, s.o.)         */
#define RE15_FL37_EINMAL_BIT      0x84   /* ROOM1090 sub06 @0x02722 Set =1, sub03 @0x024CE Set =0           */

/* Prop-Pool-Id: in allen drei Raeumen frei (ROOM1050 nOmodel 2 + Dokument obj 2; ROOM1090/1091
 * nOmodel 4 + Schrot obj 4 = RE15_R35I_SCHROT_OBJ); TIM-Slot RE15_TIM_SLOT_PROP(5) = 9. */
#define RE15_FL37_OBJ             5
#define RE15_FL37_PROP_BAND       1      /* wie die Item-Props des Originals (ROOM1010 @0x00930 pc[4])      */
#define RE15_FL37_PROP_FLAGS      0x000B /* 0x000A | 1 (@0x80040998), ROOM1010 @0x00930 pc[6..7] `0a 00`    */
#define RE15_FL37_PROP_ATTR       0x0000 /* NCCT-Raumlicht wie ROOM1010 @0x00930 pc[8..9] `00 00`           */

/* Nachrichten-Id (VERTRAG.md 1.2 Spur I: 29..31). */
#define RE15_FL37_MSG_LEER        29
/* AOT-Satzform der Zonen (Zensus 160 von 164 Item_aot_set / 427 von 436 sce-1-Zonen: sat 0x31). */
#define RE15_FL37_AOT_SAT         0x31
#define RE15_FL37_PAUSE           0xFFFFu /* sce-1-Pause-Halbwort (524/524 Saetze, scd_vm.c op_aot_set)     */

/* Spind ROOM1000 (nur Leon): Slot 3 der Original-Aufnahme -> Raumtext msg 0 "Nothing unusual." */
#define RE15_FL37_SPIND_RAUM      0x1000
#define RE15_FL37_SPIND_SLOT      3
#define RE15_FL37_SPIND_MSG       0      /* ROOM1000 msg 0 @0x0D22 = Text der Plaetze 6..9 (@0x00CAA..)     */

typedef enum { RE15_FL37_STEHEND = 0, RE15_FL37_LIEGEND = 1 } re15_fl37_art_t;
typedef enum { RE15_FL37_IMMER = 0, RE15_FL37_FEUER_AUS = 1, RE15_FL37_NICHT_GENOMMEN = 2 } re15_fl37_tor_t;

typedef struct {
    uint16_t raum;          /* GENAU dieser Raum (Leon xxx0, Elza xxx1)                                */
    uint8_t  art;           /* re15_fl37_art_t: welches der zwei Netze                                 */
    uint8_t  tor;           /* re15_fl37_tor_t: wann das Prop liegt                                    */
    uint8_t  slot;          /* AOT-Slot der Zone (Aufnahme bzw. Untersuchen)                           */
    uint8_t  zone_item;     /* 1 = Aufnahme (Item 0x31), 0 = Untersuchen (msg RE15_FL37_MSG_LEER)       */
    uint8_t  band;          /* Zonen-Etage (Aot_set pc[4])                                             */
    int32_t  x, y, z;       /* Prop-Ursprung (Welt)                                                    */
    int16_t  rot_y;         /* Pool +0x6A (scd_vm.c Member 4)                                          */
    int32_t  rect_x, rect_z, rect_w, rect_d;   /* Zone: Ecke + Groesse wie im Aot_set-Satz             */
} re15_fl37_lage_t;

/* Prop + Zone beim Raumstart (beide Raumstart-Wege, NACH dem Init-Lauf von main00/sub00) anlegen bzw.
 * den Spind in ROOM1000 umwidmen. Idempotent; tut in jedem anderen Raum nichts. */
void re15_feuerloescher_r37_install(uint16_t room_id);

/* HAKEN item_modal_common.c (Aufnahme bestaetigt, Taken-Bit gesetzt): Item 0x31 mit Bit 0x86 setzt
 * (3,0x85) wie ROOM1000 sub01 @0x00D0C — der Fundort liegt nicht mehr in ROOM1000. */
void re15_feuerloescher_r37_genommen(uint8_t item_type, uint8_t taken_bit);

/* HAKEN item_discard_common.c (Wegwerf-Abfrage "Ja", Slot geleert): Item 0x31 in ROOM1090 -> der leere
 * Loescher liegt ab jetzt am Boden (Installer mit offenem Tor). */
void re15_feuerloescher_r37_weggeworfen(uint8_t item_type);

/* Plattform-Lader: obj_id/MD1/TIM des Loescher-Props in diesem Raum, sonst -1/NULL. */
int            re15_feuerloescher_r37_obj_id(uint16_t room_id);
const uint8_t *re15_feuerloescher_r37_md1_bytes(uint16_t room_id, int *out_size);
const uint8_t *re15_feuerloescher_r37_tim_bytes(uint16_t room_id, int *out_size);

/* Sonden/Riegel. */
int                     re15_feuerloescher_r37_anzahl(void);
const re15_fl37_lage_t *re15_feuerloescher_r37_zeile(int i);
const re15_fl37_lage_t *re15_feuerloescher_r37_des_raums(uint16_t room_id);
const uint8_t          *re15_feuerloescher_r37_md1(int art, int *out_size);
const uint8_t          *re15_feuerloescher_r37_text(int *out_len);
/* Liegt das Prop im aktuellen Zustand (Tor der Zeile erfuellt)? -1 = kein Loescher-Raum. */
int                     re15_feuerloescher_r37_liegt(uint16_t room_id);

#endif /* RE15_FEUERLOESCHER_R37_H */
