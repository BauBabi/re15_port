/*
 * RE1.5 Rebuilt — Runde 36 Spur Q "schluessel": Parking Lot Key (Leon, ROOM10F0) + Parkplatz-Tuer
 * ROOM11B0 -> ROOM11C0, Prison Key (Elza, ROOM10F1) statt Roy in ROOM1011.
 * Herleitung, Belege und alle Konstanten: include/re15_schluessel_r36.h und
 * analysis/befunde_runde36/Q_schluessel.md. Vorbilder: inhalt_r35.c (Item-Prop + Zone),
 * tuer1120_1130.c (Tuer-Umwidmung), sicherung_itembild.c (Icon/Bild im geladenen Puffer).
 */
#include "re15_schluessel_r36.h"

#include <stddef.h>
#include <string.h>

#include "re15_scd.h"
#include "re15_aot.h"
#include "re15_msg.h"
#include "re15_inventory.h"
#include "re15_lock_se.h"
#include "re15_audio.h"
#include "re15_item_discard.h"
#ifdef RE15_PLATFORM_PC
#include <stdio.h>
#endif

#include "gen/re2_door_bank.inc"     /* RE2_DOOR_SE_AUF_KEY (TUERSE.VBS Satz 4 = RE2 ROOM1140 Satz 0x25) */
#include "gen/schluessel_r36.inc"    /* RE2-Bytes: Modelle, Icons, Item-Bilder (Werkzeug-Kopf)        */

extern void re15_scd_show_message(uint8_t index, uint32_t pause_mask);

/* ============================ Texte (Glyphen der Spielschrift) ============================
 * Glyphen = re15_msg_glyph (msg_common.c): ' '=00, A..Z=1D.., a..z=3D.., '.'=57, '\''=3A,
 * Zeilenumbruch 08, Seitenwechsel 02 00, Ende 01 00 (Raummeldung) bzw. 07 (Namensblob).
 * ⛔ NEUE TEXTE = PORT-WAHL; belegt ist ihre FORM und jeder Glyphenlauf:
 *
 * NAMEN (Namensblob-Form wie DEBUG.BIN @0x800c4a28, 0x07-terminiert, z.B. "Chris' Diary"
 *   @0x800c4e04 `1f 44 4e 45 4f 3a 00 20 45 3d 4e 55 07`). "Prison Key" = der Original-Name aus
 *   ROOM1011 msg 20 @0x12BF "You've taken the Prison key." in Titel-Schreibung wie "Water Key"
 *   (DEBUG.BIN 0x45). "Parking Lot Key" = Nutzer-Wort ("Parking Lot Schluessel").
 * CHECK-TEXTE (Beschreibungsbank-Form @0x800c50de, 01 00-terminiert, ohne Kopf). FORM = die
 *   RE2-Beschreibung des jeweiligen MODELLS (RE2 PSX.EXE):
 *     P. Room Key @0x800A1DA4 "It looks unique.<FC>Now I can access<FC>the power room."
 *       -> "It looks unique. / Now I can access / the parking lot."
 *     Cabin Key   @0x800A1C6E "I can access the<FC>cabin in the rear<FC>with this."
 *       -> "I can access the / prison cells / with this."
 *   (RE2 0xFC = Zeilenumbruch = RE1.5 0x08.)
 * TUER-MELDUNGEN (Raummeldung, Kopf 04 02 wie ROOM11B0 msg 13/14/15 @0x1BFE/@0x1C37/@0x1C62):
 *   msg 28 = RE2 key_type-0x4C-Text des Modells P. Room Key (0x5F -> Index 0x13 @0x8009F2BB)
 *     "<FC>It's locked.<FD> The door is marked<FC>{09}Power Room{0A}." mit dem Ort getauscht;
 *     RE2 <FD> (Seite) = RE1.5 `02 00` (ROOM10F0 msg 5 @0x12B6 "It's a small office." 02 00
 *     "It's locked."), Anfuehrung RE1.5 0x61 ... 0x19 mit Punkt INNEN wie DEBUG.BIN-Text 0x45
 *     (`61 2f 41 53 ... 49 57 19 01 00` "Sewer Mainteinance Room.").
 *     Laeufe: "It's locked." ROOM10F0 @0x12CE `25 50 3a 4f 00 48 4b 3f 47 41 40 57`;
 *     "The door is " ROOM11B1 msg 12 @0x179F `30 44 41 00 40 4b 4b 4e 00 45 4f 00`.
 *   msg 29 = RE1.5-Form der Schluessel-Benutzung ROOM11E0 msg 12 @0x238C `04 02 35 4b 51 3a 52 41
 *     00 51 4f 41 40 00 50 44 41 00 05 01 2c 48 45 41 4e 4f 05 00 57 01 00` ("You've used the
 *     Pliers.", Name in Farbe 1) mit dem Namen getauscht = RE2 Index 5 @0x8009F0E5
 *     "You have used the <Name>." an @0x8005164c. */
static const uint8_t k_name_park[16] = {          /* "Parking Lot Key" */
    0x2c,0x3d,0x4e,0x47,0x45,0x4a,0x43,0x00,0x28,0x4b,0x50,0x00,0x27,0x41,0x55,0x07,
};
static const uint8_t k_name_prison[11] = {        /* "Prison Key" */
    0x2c,0x4e,0x45,0x4f,0x4b,0x4a,0x00,0x27,0x41,0x55,0x07,
};
static const uint8_t k_desc_park[52] = {          /* "It looks unique. / Now I can access / the parking lot." */
    0x25,0x50,0x00,0x48,0x4b,0x4b,0x47,0x4f,0x00,0x51,0x4a,0x45,0x4d,0x51,0x41,0x57,
    0x08,0x2a,0x4b,0x53,0x00,0x25,0x00,0x3f,0x3d,0x4a,0x00,0x3d,0x3f,0x3f,0x41,0x4f,
    0x4f,0x08,0x50,0x44,0x41,0x00,0x4c,0x3d,0x4e,0x47,0x45,0x4a,0x43,0x00,0x48,0x4b,
    0x50,0x57,0x01,0x00,
};
static const uint8_t k_desc_prison[42] = {        /* "I can access the / prison cells / with this." */
    0x25,0x00,0x3f,0x3d,0x4a,0x00,0x3d,0x3f,0x3f,0x41,0x4f,0x4f,0x00,0x50,0x44,0x41,
    0x08,0x4c,0x4e,0x45,0x4f,0x4b,0x4a,0x00,0x3f,0x41,0x48,0x48,0x4f,0x08,0x53,0x45,
    0x50,0x44,0x00,0x50,0x44,0x45,0x4f,0x57,0x01,0x00,
};
static const uint8_t k_msg_zu[51] = {             /* "It's locked." | "The door is marked / "Parking Lot."" */
    0x04,0x02,0x25,0x50,0x3a,0x4f,0x00,0x48,0x4b,0x3f,0x47,0x41,0x40,0x57,0x02,0x00,
    0x30,0x44,0x41,0x00,0x40,0x4b,0x4b,0x4e,0x00,0x45,0x4f,0x00,0x49,0x3d,0x4e,0x47,
    0x41,0x40,0x08,0x61,0x2c,0x3d,0x4e,0x47,0x45,0x4a,0x43,0x00,0x28,0x4b,0x50,0x57,
    0x19,0x01,0x00,
};
static const uint8_t k_msg_benutzt[40] = {        /* "You've used the Parking Lot Key." */
    0x04,0x02,0x35,0x4b,0x51,0x3a,0x52,0x41,0x00,0x51,0x4f,0x41,0x40,0x00,0x50,0x44,
    0x41,0x00,0x05,0x01,0x2c,0x3d,0x4e,0x47,0x45,0x4a,0x43,0x00,0x28,0x4b,0x50,0x00,
    0x27,0x41,0x55,0x05,0x00,0x57,0x01,0x00,
};

/* Leeres Ereignis-Programm: Evt_end (Opcode 0x01, 2 Byte; RE15_SCD_OPCODES_REFERENCE.md). Der Druck
 * auf die gesperrte Tuer erledigt alles in C (wie RE2s Tuer-Handler LAB_80051514 ohne Skript). */
static const uint8_t k_evt_end[2] = { 0x01, 0x00 };

unsigned g_re15_r36q_zu_zaehler      = 0;
unsigned g_re15_r36q_benutzt_zaehler = 0;

static uint8_t s_gesperrt;
static int16_t s_tuer_next[3];   /* Door_aot_set-Nutzlast +0/+2/+4 (next_pos) fuer das Zurueckwidmen */

static void log_q(const char *s)
{
#ifdef RE15_PLATFORM_PC
    fprintf(stderr, "[schluessel] %s\n", s);
#else
    (void)s;
#endif
}

/* ============================ Texte installieren ============================ */
static void meldung_installieren(int id, const uint8_t *b, size_t n)
{
    re15_msg_install_text((unsigned char)id, b, n);
    {
        int d = re15_msg_compute_duration(b, n, 0);
        if (d > 0 && d < 65535) re15_msg_install_durations((unsigned char)id, d);
    }
}

const uint8_t *re15_schluessel_r36_meldung(int msg_id, int *out_len)
{
    if (msg_id == RE15_R36Q_MSG_ZU)      { if (out_len) *out_len = (int)sizeof k_msg_zu;      return k_msg_zu; }
    if (msg_id == RE15_R36Q_MSG_BENUTZT) { if (out_len) *out_len = (int)sizeof k_msg_benutzt; return k_msg_benutzt; }
    if (out_len) *out_len = 0;
    return NULL;
}

int re15_schluessel_r36_tuer_gesperrt(void) { return s_gesperrt; }

/* ============================ Raumaufbau ============================ */
static int slot_von_obj_id(uint8_t obj_id)
{
    for (int k = 0; k < (int)g_scd.prop_count; k++)
        if (g_scd.props[k].obj_id == obj_id) return k;
    return -1;
}

/* 10F0 -> Parking Lot Key (Leon), 10F1 -> Prison Key (Elza); sonst 0. */
static uint8_t item_des_raums(uint16_t room_id, uint8_t *bit)
{
    if (room_id == 0x10F0) { if (bit) *bit = RE15_R36Q_PARK_BIT;   return RE15_R36Q_PARK_ITEM; }
    if (room_id == 0x10F1) { if (bit) *bit = RE15_R36Q_PRISON_BIT; return RE15_R36Q_PRISON_ITEM; }
    return 0;
}

static void schluessel_anlegen(uint16_t room_id)
{
    uint8_t bit = 0, item = item_des_raums(room_id, &bit);
    if (!item) return;
    /* schon genommen -> weder Prop noch Zone (Item_aot_set @0x800406dc-718; inhalt_r35.c) */
    if (re15_game_flag_get(9, bit)) return;

    /* PROP — Felder wie ein Obj_model_set (LAB_80040914), Muster inhalt_r35.c */
    if (slot_von_obj_id(RE15_R36Q_OBJ) < 0 && g_scd.prop_count < RE15_SCD_MAX_PROPS) {
        int i = (int)g_scd.prop_count++;
        memset(&g_scd.props[i], 0, sizeof g_scd.props[i]);
        g_scd.props[i].active     = 1;
        g_scd.props[i].obj_id     = RE15_R36Q_OBJ;
        g_scd.props[i].obj_type   = 0;                       /* pc[2] -> pool+8 @0x8004095c   */
        g_scd.props[i].band       = RE15_R36Q_PROP_BAND;     /* pc[4] -> pool+130 @0x80040974 */
        g_scd.props[i].parent_obj = -1;                      /* pc[5] = 0x00 = Weltraum       */
        g_scd.props[i].x = RE15_R36Q_X;
        g_scd.props[i].y = RE15_R36Q_Y;
        g_scd.props[i].z = RE15_R36Q_Z;
        g_scd.props[i].rot_y = RE15_R36Q_ROT_Y;
        g_scd.props[i].flags = RE15_R36Q_PROP_FLAGS;         /* 0x000A | 1 @0x80040998         */
    }
    /* ZONE — wie op_item_aot_set: Mitte + halbe Ausdehnung, sat (+3), Etage (+4) 0. Nur frei. */
    if (!g_aot.slots[RE15_R36Q_SLOT].active) {
        re15_aot_set_item_tk_prop(RE15_R36Q_SLOT,
                                  RE15_R36Q_RECT_X + RE15_R36Q_RECT_W / 2,
                                  RE15_R36Q_RECT_Z + RE15_R36Q_RECT_D / 2,
                                  RE15_R36Q_RECT_W / 2, RE15_R36Q_RECT_D / 2,
                                  item, RE15_R36Q_MENGE, bit, RE15_R36Q_OBJ);
        g_aot.slots[RE15_R36Q_SLOT].sce_flags = RE15_R36Q_AOT_SAT;
        g_aot.slots[RE15_R36Q_SLOT].band      = 0;
    }
#ifdef RE15_PLATFORM_PC
    fprintf(stderr, "[schluessel] ROOM%04X: %s (Item 0x%02X, Bit (9,%u)) Prop obj %d bei (%d,%d,%d), "
                    "Zone Slot %d\n", (unsigned)room_id, re15_schluessel_r36_name_ascii(item),
            (unsigned)item, (unsigned)bit, RE15_R36Q_OBJ, RE15_R36Q_X, RE15_R36Q_Y, RE15_R36Q_Z,
            RE15_R36Q_SLOT);
#endif
}

static void tuer_sperren(uint16_t room_id)
{
    if (room_id != RE15_R36Q_TUER_RAUM) return;
    if (re15_game_flag_get(9, RE15_R36Q_TUER_BIT)) return;          /* schon aufgeschlossen */
    /* Ohne Strom widmet sub01 den Slot in JEDEM Bild zum Text msg 15 um: @0x011F2 If, @0x011F6
     * `21 04 f3 00` Ck(4,0xF3)==0, @0x01216 `46 01 01 31 0f 00 ff ff 00 00` Aot_reset(1, sce 1, 0x31,
     * msg 15) ("It's too dark to see anything..."). Dann bleibt alles RE1.5 — gesperrt wird nur bei
     * Strom (4,0xF3)=1, wenn Slot 1 die Tuer bleibt. */
    if (!re15_game_flag_get(RE15_R36Q_STROM_BANK, RE15_R36Q_STROM_BIT)) return;
    /* Nur den echten Tuer-Satz umwidmen (main00 @0x00FAA). */
    const re15_aot_t *a = &g_aot.slots[RE15_R36Q_TUER_SLOT];
    if (!a->active || a->type != RE15_AOT_TYPE_DOOR) return;

    meldung_installieren(RE15_R36Q_MSG_ZU, k_msg_zu, sizeof k_msg_zu);
    meldung_installieren(RE15_R36Q_MSG_BENUTZT, k_msg_benutzt, sizeof k_msg_benutzt);
    /* Nutzlast merken: Aot_reset schreibt beim Zurueckwidmen +0/+2/+4 (@0x80040788-a8), Ziel/Cut/
     * Richtung bleiben (re15_aot_retype case 2). */
    s_tuer_next[0] = (int16_t)g_aot.door_params[RE15_R36Q_TUER_SLOT].spawn_x;
    s_tuer_next[1] = (int16_t)g_aot.door_params[RE15_R36Q_TUER_SLOT].spawn_y;
    s_tuer_next[2] = (int16_t)g_aot.door_params[RE15_R36Q_TUER_SLOT].spawn_z;
    /* Band: der Port haelt das Tuer-Band in door_params, der Ereignis-Platz liest a->band
     * (tuer1120_1130.c, gleiche Begruendung). */
    g_aot.slots[RE15_R36Q_TUER_SLOT].band = g_aot.door_params[RE15_R36Q_TUER_SLOT].band;
    /* sce 3 Ereignis (Payload +3 = Sub, @0x80043100; re15_aot_retype case 3), sat 0x31 = Aktion. */
    re15_aot_retype(RE15_R36Q_TUER_SLOT, 3, RE15_R36Q_TUER_FLAGS, 0,
                    (uint16_t)(RE15_R36Q_EREIGNIS << 8), 0);
    s_gesperrt = 1;
#ifdef RE15_PLATFORM_PC
    fprintf(stderr, "[schluessel] ROOM%04X Slot %d (Tuer -> ROOM11C0) gesperrt: Ereignis %d, "
                    "Flag (9,%d) = 0\n", (unsigned)room_id, RE15_R36Q_TUER_SLOT, RE15_R36Q_EREIGNIS,
            RE15_R36Q_TUER_BIT);
#endif
}

void re15_schluessel_r36_install(uint16_t room_id)
{
    s_gesperrt = 0;
    schluessel_anlegen(room_id);
    tuer_sperren(room_id);
}

/* ============================ Ereignisse ============================ */
uint8_t re15_schluessel_r36_umleiten(uint16_t room_id, uint8_t event_id)
{
    if (room_id == RE15_R36Q_ROY_RAUM && event_id == RE15_R36Q_ROY_SUB_SCHL) {
        log_q("ROOM1011: Roy -> sub07 (Zettel 0513) statt sub06 (Prison key)");
        return RE15_R36Q_ROY_SUB_ZETTEL;
    }
    return event_id;
}

const uint8_t *re15_schluessel_r36_ereignis(uint16_t room_id, uint8_t event_id)
{
    if (room_id != RE15_R36Q_TUER_RAUM || event_id != RE15_R36Q_EREIGNIS || !s_gesperrt) return NULL;
    /* RE2 LAB_80051514: Besitz-Test im INVENTAR (`jal FUN_800696cc` @0x80051628, `bltz` @0x80051634). */
    if (re15_inv_find_item(RE15_R36Q_PARK_ITEM) >= 0) {
        /* gefunden: Text (@0x8005164c), Aufschliess-Ton (@0x80051654/58 Se_on(2,0x25)), Wegwerf-
         * Fortsetzung (@0x80051670), Schlossbit (@0x80051690 FUN_8007730c) — KEIN Durchgang bei
         * diesem Druck (v0 = 0 @0x80051698); der Platz wird wieder die Original-Tuer, der naechste
         * Druck geht hindurch. Die Wegwerf-Kette des Ports in RE2s Reihenfolge (re15_item_discard.h):
         * Besitz VOR dem Oeffnen (@0x80051628/34), Fortsetzung NACH dem Oeffnen (@0x80051670). */
        (void)re15_discard_besitz_vor_nachricht(room_id, RE15_R36Q_MSG_BENUTZT);
        re15_scd_show_message(RE15_R36Q_MSG_BENUTZT, RE15_R36Q_MASKE << 16);
        re15_audio_re2_door_se(RE2_DOOR_SE_AUF_KEY);
        re15_discard_notice_message(room_id, RE15_R36Q_MSG_BENUTZT);
        re15_game_flag_set(9, RE15_R36Q_TUER_BIT, 1);
        g_aot.slots[RE15_R36Q_TUER_SLOT].band = g_aot.door_params[RE15_R36Q_TUER_SLOT].band;
        re15_aot_retype(RE15_R36Q_TUER_SLOT, 2, RE15_R36Q_TUER_FLAGS,
                        (uint16_t)s_tuer_next[0], (uint16_t)s_tuer_next[1], (uint16_t)s_tuer_next[2]);
        g_aot.slots[RE15_R36Q_TUER_SLOT].was_inside = 1;
        s_gesperrt = 0;
        g_re15_r36q_benutzt_zaehler++;
        log_q("ROOM11B0: Parking Lot Key benutzt -> Tuer offen, Flag (9,93) = 1");
    } else {
        /* fehlt: Verschlossen-Ton (@0x800516a0/a4 Se_on(2,0x16), a1 = 0) und Text (@0x800516b8).
         * Satz = Art M (mechanisch) der Mini-Bank TUERSE.VBS (re15_lock_se.h). */
        re15_audio_re2_door_se(re15_lock_se_satz_fuer_art(RE15_LOCK_SE_ART_M));
        re15_scd_show_message(RE15_R36Q_MSG_ZU, RE15_R36Q_MASKE << 16);
        g_re15_r36q_zu_zaehler++;
        log_q("ROOM11B0: Parkplatz-Tuer verschlossen (kein Parking Lot Key im Inventar)");
    }
    return k_evt_end;
}

int re15_schluessel_r36_discard_stelle(unsigned room_id, uint8_t msg_id, uint8_t *out_item)
{
    if ((uint16_t)room_id != RE15_R36Q_TUER_RAUM || msg_id != RE15_R36Q_MSG_BENUTZT) return 0;
    if (out_item) *out_item = RE15_R36Q_PARK_ITEM;
    return 1;
}

void re15_schluessel_r36_genommen(uint8_t item_id, uint8_t taken_bit)
{
    if (item_id != RE15_R36Q_PARK_ITEM && item_id != RE15_R36Q_PRISON_ITEM) return;
#ifdef RE15_PLATFORM_PC
    fprintf(stderr, "[schluessel] %s genommen: Item 0x%02X, Bit (9,%u)\n",
            re15_schluessel_r36_name_ascii(item_id), (unsigned)item_id, (unsigned)taken_bit);
#else
    (void)taken_bit;
#endif
    if (item_id != RE15_R36Q_PRISON_ITEM) return;
    /* wie ROOM1011 sub06 @0x00E2C `22 04 ec 01` */
    re15_game_flag_set(RE15_R36Q_ZELLE_BANK, RE15_R36Q_ZELLE_BIT, 1);
    log_q("Prison Key genommen -> Flag (4,0xEC) = 1 (Zelle ROOM1211)");
}

/* ============================ Namen / Texte ============================ */
const uint8_t *re15_schluessel_r36_name(uint8_t item_id)
{
    if (item_id == RE15_R36Q_PARK_ITEM)   return k_name_park;
    if (item_id == RE15_R36Q_PRISON_ITEM) return k_name_prison;
    return NULL;
}

const uint8_t *re15_schluessel_r36_desc(uint8_t item_id)
{
    if (item_id == RE15_R36Q_PARK_ITEM)   return k_desc_park;
    if (item_id == RE15_R36Q_PRISON_ITEM) return k_desc_prison;
    return NULL;
}

const char *re15_schluessel_r36_name_ascii(uint8_t item_id)
{
    if (item_id == RE15_R36Q_PARK_ITEM)   return "PARKING LOT KEY";
    if (item_id == RE15_R36Q_PRISON_ITEM) return "PRISON KEY";
    return NULL;
}

/* ============================ Icon / Item-Bild ============================
 * ITEMALL.PIX: Tile == Id, 1200 B (40x30 8 bpp; item_icon_common.c ITEMALL_TILE_BYTES).
 * ITPS.ITP:    Block == Id, 0x3000 B (Lader LAB_8001e404 `ori v0,zero,0x3000` @0x8001e414). */
void re15_schluessel_r36_icon_einsetzen(uint8_t *itemall, int size)
{
    if (!itemall) return;
    if ((RE15_R36Q_PARK_ITEM + 1) * 1200 <= size)
        memcpy(itemall + RE15_R36Q_PARK_ITEM * 1200, re15_r36q_park_icon, 1200);
    if ((RE15_R36Q_PRISON_ITEM + 1) * 1200 <= size)
        memcpy(itemall + RE15_R36Q_PRISON_ITEM * 1200, re15_r36q_prison_icon, 1200);
}

void re15_schluessel_r36_bild_einsetzen(uint8_t *itps, int size)
{
    if (!itps) return;
    if ((RE15_R36Q_PARK_ITEM + 1) * 0x3000 <= size)
        memcpy(itps + RE15_R36Q_PARK_ITEM * 0x3000, re15_r36q_park_itps, 0x3000);
    if ((RE15_R36Q_PRISON_ITEM + 1) * 0x3000 <= size)
        memcpy(itps + RE15_R36Q_PRISON_ITEM * 0x3000, re15_r36q_prison_itps, 0x3000);
}

/* ============================ Plattform-Lader ============================ */
int re15_schluessel_r36_obj_id(uint16_t room_id)
{
    return item_des_raums(room_id, NULL) ? RE15_R36Q_OBJ : -1;
}

const uint8_t *re15_schluessel_r36_md1_bytes(uint16_t room_id, int *out_size)
{
    uint8_t item = item_des_raums(room_id, NULL);
    if (out_size) *out_size = 0;
    if (item == RE15_R36Q_PARK_ITEM) {
        if (out_size) *out_size = (int)sizeof re15_r36q_park_md1;
        return re15_r36q_park_md1;
    }
    if (item == RE15_R36Q_PRISON_ITEM) {
        if (out_size) *out_size = (int)sizeof re15_r36q_prison_md1;
        return re15_r36q_prison_md1;
    }
    return NULL;
}

const uint8_t *re15_schluessel_r36_tim_bytes(uint16_t room_id, int *out_size)
{
    uint8_t item = item_des_raums(room_id, NULL);
    if (out_size) *out_size = 0;
    if (item == RE15_R36Q_PARK_ITEM) {
        if (out_size) *out_size = (int)sizeof re15_r36q_park_tim;
        return re15_r36q_park_tim;
    }
    if (item == RE15_R36Q_PRISON_ITEM) {
        if (out_size) *out_size = (int)sizeof re15_r36q_prison_tim;
        return re15_r36q_prison_tim;
    }
    return NULL;
}
