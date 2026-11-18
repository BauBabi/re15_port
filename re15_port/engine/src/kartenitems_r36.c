/*
 * RE1.5 Rebuilt — Lageplaene zum Aufsammeln (Runde 36 Spur N; Runde 42 Strang lageplaene: EIN Revierplan +
 * EIN Kellerplan wie RE2 room20B0). Herleitung, Belege und alle Konstanten: include/re15_kartenitems_r36.h
 * und analysis/befunde_runde36/N_kartenitems.md.
 * Vorbild der Datei-Form: inhalt_r35.c (Prop + Zone beim Raumstart), rolltor_1050.c (Port-Programm
 * mit Ja/Nein-Frage, Texte beim Ausloesen). Licht: KEIN Haken mehr — die Karte traegt das Flach-Attribut
 * RE15_R36K_PROP_ATTR und wird vom Prop-Zeichner wie jedes Objekt mit Pool+0x0c-Bit 0x10 gezeichnet
 * (FUN_8002c18c @0x8002c2f4 -> FUN_80022f0c, 0x808080; Nachbesserung 1).
 */
#include "re15_kartenitems_r36.h"

#include <stddef.h>
#include <string.h>

#include "re15_scd.h"
#include "re15_aot.h"
#include "re15_msg.h"
#ifdef RE15_PLATFORM_PC
#include <stdio.h>
#endif

#include "gen/kartenitems_r36.inc"

#define N_ZEILEN ((int)(sizeof k_r36k_lage / sizeof k_r36k_lage[0]))

/* Runde 42 Stufe 2: eine Zeile gilt nur in ihrer Fassung des Raums (variante; Fabrikplan nur Elzas 30C1). */
static int fassung_passt(const re15_kartenitem_r36_t *z, uint16_t room_id)
{
    if (z->variante == RE15_R36K_VARIANTE_LEON) return (room_id & 1u) == 0;
    if (z->variante == RE15_R36K_VARIANTE_ELZA) return (room_id & 1u) != 0;
    return 1;
}

static int index_des_raums(uint16_t room_id)
{
    for (int k = 0; k < N_ZEILEN; k++)
        if ((uint16_t)(room_id & 0xFFFEu) == k_r36k_lage[k].raum && fassung_passt(&k_r36k_lage[k], room_id)) return k;
    return -1;
}

int re15_kartenitems_r36_anzahl(void) { return N_ZEILEN; }

const re15_kartenitem_r36_t *re15_kartenitems_r36_zeile(int i)
{
    return (i >= 0 && i < N_ZEILEN) ? &k_r36k_lage[i] : NULL;
}

const re15_kartenitem_r36_t *re15_kartenitems_r36_des_raums(uint16_t room_id)
{
    int k = index_des_raums(room_id);
    return k >= 0 ? &k_r36k_lage[k] : NULL;
}

/* Runde 42: genommen = ALLE Bits des Plans gesetzt. Fehlt eines (frischer Stand, oder alter Spielstand mit
 * einem einzelnen Etagenplan der Runde 36), haengt der Plan und setzt bei der Aufnahme alle. */
/* Zeile ohne Gruppe (n_blaetter 0, z.B. eine spaeter von Hand eingetragene Zeile im alten 13-Felder-Format) zaehlt als
 * Ein-Blatt-Plan mit blatt/bit — sonst hinge sie nie (leere Gruppe = "alle genommen"). */
static int n_von(const re15_kartenitem_r36_t *z)
{
    return z->n_blaetter < 1 ? 1 : (z->n_blaetter > RE15_R36K_MAX_BLAETTER ? RE15_R36K_MAX_BLAETTER : z->n_blaetter);
}
static uint8_t bit_von(const re15_kartenitem_r36_t *z, int i)   { return z->n_blaetter < 1 ? z->bit : z->bits[i]; }
static uint8_t blatt_von(const re15_kartenitem_r36_t *z, int i) { return z->n_blaetter < 1 ? z->blatt : z->blaetter[i]; }

int re15_kartenitems_r36_genommen(const re15_kartenitem_r36_t *z)
{
    if (!z) return 0;
    for (int i = 0; i < n_von(z); i++)
        if (!re15_game_flag_get(RE15_R36K_BANK, bit_von(z, i))) return 0;
    return 1;
}

static int slot_von_obj_id(uint8_t obj_id)
{
    for (int k = 0; k < (int)g_scd.prop_count; k++)
        if (g_scd.props[k].obj_id == obj_id) return k;
    return -1;
}

/* Ist der Satz in `slot` schon unsere Aufhebe-Zone? (Idempotenz: beide Raumstart-Wege koennen
 * hintereinander laufen; ein zweites Verdraengen wuerde den verdraengten Satz ueberschreiben.) */
static int ist_unsere_zone(int slot)
{
    const re15_aot_t *a = &g_aot.slots[slot];
    return a->active && a->type == RE15_AOT_TYPE_GENERIC && a->event_id == RE15_R36K_EREIGNIS;
}

/* Den Satz aus `von` unveraendert nach `nach` tragen (alle parallelen Felder, wie sie der Scan liest). */
static void satz_umziehen(int von, int nach)
{
    g_aot.slots[nach]       = g_aot.slots[von];
    g_aot.door_params[nach] = g_aot.door_params[von];
    g_aot.item_params[nach] = g_aot.item_params[von];
    g_aot.flag_params[nach] = g_aot.flag_params[von];
    g_aot.env_params[nach]  = g_aot.env_params[von];
    g_aot.stair_params[nach] = g_aot.stair_params[von];
    g_aot.slots[nach].was_inside = 0;
}

void re15_kartenitems_r36_install(uint16_t room_id)
{
    int k = index_des_raums(room_id);
    if (k < 0) return;
    const re15_kartenitem_r36_t *g = &k_r36k_lage[k];
    /* schon genommen -> weder Prop noch Zone (RE2 room20B0: das Weltobjekt bleibt bei (20000,..)
     * geparkt, RE1.5 ROOM5030 sub00 @0x01086 Ck(3,115,0) legt den Ausloeser nur ohne Bit an). */
    if (re15_kartenitems_r36_genommen(g)) return;

    /* PROP — Felder wie ein Obj_model_set (LAB_80040914), Muster inhalt_r35.c */
    if (slot_von_obj_id(RE15_R36K_OBJ) < 0 && g_scd.prop_count < RE15_SCD_MAX_PROPS) {
        int i = (int)g_scd.prop_count++;
        memset(&g_scd.props[i], 0, sizeof g_scd.props[i]);
        g_scd.props[i].active     = 1;
        g_scd.props[i].obj_id     = RE15_R36K_OBJ;
        g_scd.props[i].obj_type   = 0;                     /* pc[2] -> pool+8 @0x8004095c   */
        /* pc[4] -> pool+130 @0x80040974. Runde 42 Stufe 2: eine Etage UEBER dem Standort = PORT-WAHL AUS MESSUNG, keine
         * Disc-Regel (Kopf re15_kartenitems_r36.h PROP-BAND; die frueher hier als Vorbild genannten Wandobjekte ROOM2000
         * @0x016E6.., ROOM6000 @0x00E16, ROOM1200 @0x007DA zeigen keine +1-Regel). Im Band des Spielers waere das Plakat fuer
         * die Klettersonde ein Objekt (FUN_8002d2c0 @0x8002d364-70 Band-Gate obj+0x82 == Spieler-Band, Kasten FUN_8002da4c
         * mit Rand -112 trifft fast ueberall): GEGENPROBE B gemessen in 2000 - Aktion vor der Wand -> Klettern auf y -3600
         * statt Ereignis 28 (unit kanal + integration_r42_lageplaene2_weg rot). Stufe 1 (Band 0): 1 wie bisher. */
        g_scd.props[i].band       = (uint8_t)(g->band + RE15_R36K_PROP_BAND);
        g_scd.props[i].parent_obj = -1;                    /* pc[5] = 0x00 = Weltraum        */
        g_scd.props[i].x = g->x;
        g_scd.props[i].y = g->y;
        g_scd.props[i].z = g->z;
        g_scd.props[i].rot_x = g->rot_x;
        g_scd.props[i].rot_y = g->rot_y;
        g_scd.props[i].rot_z = g->rot_z;
        g_scd.props[i].flags = RE15_R36K_PROP_FLAGS;       /* 0x000A | 1 @0x80040998         */
        g_scd.props[i].attr  = RE15_R36K_PROP_ATTR;        /* 0x0010 Flach-Bit (Pool+0x0c @0x800409b0):
                                                            * RE2 room20B0 @0x03158 / room2130 @0x00FDC,
                                                            * Zeichner FUN_8002c18c @0x8002c2f4         */
    }

    /* ZONE — EVENT-Satz wie ROOM5030 sub00 @0x0108A (sce 3, sat 0x31, p0 0x00FF, p1 = Sub<<8|0x18). */
    if (g->aot_slot >= RE15_AOT_MAX || ist_unsere_zone(g->aot_slot)) return;
    if (g->verdraengt_nach < RE15_AOT_MAX) {
        /* Umzug eines Original-Satzes (seit Strang lageplaene_nach2 von KEINER Zeile benutzt: der Revierplan liegt jetzt
         * in Slot 21 links der Tuer, die Notfall-Tafel bleibt in Slot 9). Fruehere Belegung:
         * ROOM1030/1031 (Nacharbeit): Slot 9 ist die Notfall-Tafel (main00 Aot_set @0x01D42, sce 1, msg 4,
         * Rechteck x 480..2864 z -15769..-14139 vor der Ostwand). Der Aktions-Scan nimmt je Druck den
         * KLEINSTEN treffenden Slot (FUN_80042bac, aot_common.c "one action handler per press") — die
         * Karte in einem hohen Slot waere dort nie erreichbar. Der Satz zieht deshalb unveraendert in
         * den freien Slot verdraengt_nach (39) um, die Karte nimmt seinen Platz; ausserhalb ihres Rechtecks
         * antwortet die Tafel wie bisher, nach der Aufnahme (Aot_reset(9,0)) ueberall. (Bis zur Nacharbeit
         * war es Slot 8 = Muenztelefon @0x01D2E an der alten Stelle der Rueckwand.) */
        if (g_aot.slots[g->aot_slot].active) {
            if (g_aot.slots[g->verdraengt_nach].active) return;   /* kein freier Platz: nichts anlegen */
            satz_umziehen(g->aot_slot, g->verdraengt_nach);
        }
    } else if (g_aot.slots[g->aot_slot].active) {
        return;                                              /* fremder Satz im Slot: nichts anlegen */
    }
    re15_aot_set(g->aot_slot, RE15_AOT_TYPE_GENERIC, RE15_R36K_EREIGNIS,
                 g->rect_x + RE15_R36K_RECT / 2, g->rect_z + RE15_R36K_RECT / 2,
                 RE15_R36K_RECT / 2, RE15_R36K_RECT / 2);
    g_aot.slots[g->aot_slot].sce_flags = 0;
    g_aot.slots[g->aot_slot].band      = g->band;           /* Aot_set pc[4]: 0 wie ROOM5030 (Stufe 1); Stufe 2 das Band
                                                            * des Standorts (Gehweg 2000 = 1, Absatz 6000 = 2)      */
    re15_aot_retype(g->aot_slot, RE15_R36K_AOT_SCE, RE15_R36K_AOT_SAT, RE15_R36K_AOT_P0,
                    (uint16_t)((RE15_R36K_EREIGNIS << 8) | 0x18), 0);
#ifdef RE15_PLATFORM_PC
    {
        char umzug[48] = "";
        if (g->verdraengt_nach < RE15_AOT_MAX)
            snprintf(umzug, sizeof umzug, " (verdraengter Satz -> Slot %d)", (int)g->verdraengt_nach);
        fprintf(stderr, "[kartenitems] ROOM%04X: Karte Blatt %d (Bit 9,%d) Prop obj %d bei (%ld,%ld,%ld), "
                        "Zone Slot %d%s\n", (unsigned)room_id, g->blatt, g->bit, RE15_R36K_OBJ,
                (long)g->x, (long)g->y, (long)g->z, g->aot_slot, umzug);
    }
#endif
}

/* ---- Das Port-Programm. Jede Zeile mit ihrem Vorbild. ----------------------------------------------
 * Ifel_ck-Laenge nach ROOM5030 sub02 @0x010B2 `06 00 1e 00`: Blockende = Ifel_ck + 4 + Laenge = hinter
 * dem Endif (0x010B2 + 4 + 0x1E = 0x010D4). Vorlage mit EINEM Set: +0x06 + 4 + 0x2C = +0x36.
 * Runde 42: je weiteres Blatt des Plans ein weiteres `22 09 bb 01` direkt hinter dem ersten (wie RE2 room20B0
 * Set(33,2) @0x037DA, Set(33,3) @0x037DE, Set(33,4) @0x037E2 hintereinander); jedes verlaengert den
 * Ifel-Block um 4 Byte. Revierplan (Nacharbeit, mit Dach): 4 Sets -> 68 Byte, Ifel-Laenge 0x38; Kellerplan: 2 Sets -> 60 Byte, 0x30. */
static const uint8_t k_programm[] = {
    /* +00 */ 0x2b, RE15_R36K_MSG_FRAGE, 0xff, 0xff,   /* Message_on 30 (Frage), Maske 0xFFFF  = ROOM5030 sub02 @0x010AC */
    /* +04 */ 0x02, 0x00,                              /* Evt_next + Nop                       = @0x010B0                 */
    /* +06 */ 0x06, 0x00, 0x2c, 0x00,                  /* Ifel_ck, Blockende +0x36             = @0x010B2                 */
    /* +0A */ 0x21, 0x0c, 0x1f, 0x00,                  /* Ck(12,31,0) = Ja                     = @0x010B6                 */
    /* +0E */ 0x46, 0xEE, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                                       /* Aot_reset(Slot,0) Zone weg           = @0x010BA (Slot = Zeile)  */
    /* +18 */ 0x09, 0x0a, 0x01, 0x00,                  /* Sleep 1                              = @0x010C4                 */
    /* +1C */ 0x22, RE15_R36K_BANK, 0xEE, 0x01,        /* Set(9,Bit,1) Besitz + Etagenwahl     = Rolle @0x010C8; RE2
                                                        * Set(33,n) @0x037DA + Set(35,n) @0x037E6 in EINEM Bit   */
    /* +20 */ 0x2e, 0x03, RE15_R36K_OBJ,               /* Work_set(3 = Objekt, obj 15)         = RE2 @0x037F2 `2e 04 08`
                                                        * (RE1.5 Art 3 = Objekt: scd_vm.c scd_work_bind)          */
    /* +23 */ 0x00,                                    /* Nop                                  = RE2 @0x037F5             */
    /* +24 */ 0x32, 0x00, 0x20, 0x4e, 0x20, 0x4e, 0x20, 0x4e,
                                                       /* Pos_set(20000,20000,20000) Modell weg = RE2 @0x037F6           */
    /* +2C */ 0x02, 0x00,                              /* Evt_next + Nop                       = RE2 @0x037FE             */
    /* +2E */ 0x2b, RE15_R36K_MSG_GENOMMEN, 0xff, 0xff,/* Message_on 31 "You've taken the ..." = @0x010CC / RE2 @0x03800  */
    /* +32 */ 0x02, 0x00,                              /* Evt_next + Nop                       = @0x010D0                 */
    /* +34 */ 0x08, 0x00,                              /* Endif                                = @0x010D2                 */
    /* +36 */ 0x01, 0x00,                              /* Evt_end                              = @0x010D4                 */
};
#define PRG_SLOT 0x0F   /* Offset des Slot-Bytes im Aot_reset */
#define PRG_SET  0x1C   /* Offset des ersten Set              */
#define PRG_BIT  0x1E   /* Offset des Bit-Bytes im Set        */
#define PRG_IFEL 0x08   /* Offset des Laengen-Bytes im Ifel_ck */

static uint8_t s_programm[sizeof k_programm + 4 * (RE15_R36K_MAX_BLAETTER - 1)];

const uint8_t *re15_kartenitems_r36_programm(uint16_t room_id, int *out_len)
{
    int k = index_des_raums(room_id);
    if (out_len) *out_len = 0;
    if (k < 0) return NULL;
    const re15_kartenitem_r36_t *z = &k_r36k_lage[k];
    int n = n_von(z);
    int extra = 4 * (n - 1);
    /* Kopf bis einschliesslich erstem Set, dann die weiteren Sets, dann der Rest der Vorlage. */
    memcpy(s_programm, k_programm, PRG_SET + 4);
    s_programm[PRG_SLOT] = z->aot_slot;
    s_programm[PRG_BIT]  = bit_von(z, 0);
    for (int i = 1; i < n; i++) {
        uint8_t *q = s_programm + PRG_SET + 4 * i;
        q[0] = 0x22; q[1] = RE15_R36K_BANK; q[2] = bit_von(z, i); q[3] = 0x01;   /* Set(9,Bit,1) = RE2 @0x037DE/@0x037E2 */
    }
    memcpy(s_programm + PRG_SET + 4 + extra, k_programm + PRG_SET + 4, sizeof k_programm - (PRG_SET + 4));
    s_programm[PRG_IFEL] = (uint8_t)(k_programm[PRG_IFEL] + extra);           /* Blockende wandert mit */
    if (out_len) *out_len = (int)sizeof k_programm + extra;
    return s_programm;
}

const uint8_t *re15_kartenitems_r36_meldung(uint16_t room_id, int welche, int *out_len)
{
    int k = index_des_raums(room_id);
    if (out_len) *out_len = 0;
    if (k < 0 || welche < 0 || welche > 1) return NULL;
    if (out_len) *out_len = welche ? k_r36k_genommen_len[k] : k_r36k_frage_len[k];
    return welche ? k_r36k_genommen[k] : k_r36k_frage[k];
}

static void text_einsetzen(unsigned char id, const uint8_t *m, int len)
{
    re15_msg_install_text(id, m, (size_t)len);
    /* Dauer-Eintrag wie fuer jede RDT-Nachricht (re15_msg_load_room_block; Muster rolltor_1050.c). */
    int d = re15_msg_compute_duration(m, (size_t)len, 0);
    if (d > 0 && d < 65535) re15_msg_install_durations(id, d);
}

const uint8_t *re15_kartenitems_r36_ereignis(uint16_t room_id, uint8_t event_id)
{
    int k = index_des_raums(room_id);
    if (k < 0 || event_id != RE15_R36K_EREIGNIS) return NULL;
    if (re15_kartenitems_r36_genommen(&k_r36k_lage[k])) return NULL;
    /* Texte beim Ausloesen einsetzen (Muster rolltor_1050.c): gilt an jedem Raumstart-Weg, der
     * Raum-Teardown loescht sie wieder (re15_msg_clear_room_block). */
    text_einsetzen(RE15_R36K_MSG_FRAGE, k_r36k_frage[k], k_r36k_frage_len[k]);
    text_einsetzen(RE15_R36K_MSG_GENOMMEN, k_r36k_genommen[k], k_r36k_genommen_len[k]);
#ifdef RE15_PLATFORM_PC
    fprintf(stderr, "[kartenitems] ROOM%04X Ereignis %u -> Port-Programm Karte Blatt %d\n",
            (unsigned)room_id, (unsigned)event_id, k_r36k_lage[k].blatt);
#endif
    return re15_kartenitems_r36_programm(room_id, NULL);
}

int re15_kartenitems_r36_obj_id(uint16_t room_id)
{
    return index_des_raums(room_id) >= 0 ? RE15_R36K_OBJ : -1;
}

const uint8_t *re15_kartenitems_r36_md1_bytes(uint16_t room_id, int *out_size)
{
    int k = index_des_raums(room_id);
    if (out_size) *out_size = 0;
    if (k < 0) return NULL;
    if (out_size) *out_size = (int)sizeof k_r36k_md1_0;
    return k_r36k_md1[k];
}

const uint8_t *re15_kartenitems_r36_tim_bytes(uint16_t room_id, int *out_size)
{
    if (out_size) *out_size = 0;
    if (index_des_raums(room_id) < 0) return NULL;
    if (out_size) *out_size = (int)sizeof k_r36k_tim;
    return k_r36k_tim;
}

/* Runde 42: Besitz je BLATT aus seinem eigenen Bit (blaetter[i] <-> bits[i] aller Plaene) — damit zaehlen auch
 * die Einzelbits alter Spielstaende (Runde 36: 3F-Plan 1140 = Bit 87 usw.) unveraendert. */
int re15_kartenitems_r36_blatt_besitz(unsigned page)
{
    for (int k = 0; k < N_ZEILEN; k++)
        for (int i = 0; i < n_von(&k_r36k_lage[k]); i++)
            if (blatt_von(&k_r36k_lage[k], i) == page && re15_game_flag_get(RE15_R36K_BANK, bit_von(&k_r36k_lage[k], i)))
                return 1;
    return 0;
}

uint32_t re15_kartenitems_r36_besitz_bits(void)
{
    uint32_t m = 0;
    for (int k = 0; k < N_ZEILEN; k++)
        for (int i = 0; i < n_von(&k_r36k_lage[k]); i++)
            if (blatt_von(&k_r36k_lage[k], i) < 32 && re15_game_flag_get(RE15_R36K_BANK, bit_von(&k_r36k_lage[k], i)))
                m |= (uint32_t)1u << blatt_von(&k_r36k_lage[k], i);
    return m;
}
