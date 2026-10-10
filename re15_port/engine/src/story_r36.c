/*
 * RE1.5 Rebuilt — Story Leon/Elza (Runde 36, Spur P "story").
 *
 * Herleitung, Belege und alle Konstanten: include/re15_story_r36.h und
 * analysis/befunde_runde36/P_story.md. Kein Asset-Patch: die RDTs bleiben byte-true.
 */
#include "re15_story_r36.h"

#include "re15_aot.h"        /* g_aot, re15_aot_set, re15_aot_retype                    */
#include "re15_scd.h"        /* re15_game_flag_get, scd_prop_hide_by_obj_id, g_scd       */
#include "re15_msg.h"        /* re15_msg_install_text / _durations / _compute_duration   */
#ifdef RE15_PLATFORM_PC
#include <stdio.h>
#endif

#include "gen/story_r36_texte.inc"   /* k_s36_texte[] (tools/r36_p/texte_bauen.py) */

#define N_TEXTE ((int)(sizeof k_s36_texte / sizeof k_s36_texte[0]))

/* ---- Vorlagen der Text-Plaetze: die ORIGINAL-Saetze, nur Slot (pc[1]) und msg (Nutzlast u16@+0)
 * getauscht (re15_story_r36.h P2/P3). ------------------------------------------------------------ */

/* ROOM1211.RDT sub00 @0x01DEE, Slot 8 -> 9, msg 10 -> 20. */
static const uint8_t k_zelle_satz[20] = {
    0x2c, RE15_S36_ZELLE_SLOT, 0x01, 0x31, 0x00, 0x00,
    0x5c, 0xaa, 0x00, 0xa8, 0x0e, 0x03, 0xd4, 0x06,          /* (-21924,-22528) (782,1748) */
    RE15_S36_ZELLE_MSG, 0x00, 0xff, 0xff, 0x00, 0x00,        /* msg, Pausemaske 0xffff     */
};

/* ROOM1090.RDT sub00 @0x0230E, Slot 2 -> 5, msg 7 -> 20 (28 B, Flags 0xB1 = Viereck-Form). */
static const uint8_t k_schuppen_satz[28] = {
    0x2c, RE15_S36_SCHUPPEN_SLOT, 0x01, 0xb1, 0x01, 0x00,
    0x64, 0x00, 0x14, 0x05, 0x3a, 0x07, 0x66, 0x08,          /* (100,1300) (1850,2150)      */
    0xde, 0x0d, 0x30, 0xf8, 0x08, 0x07, 0x10, 0xf5,          /* (3550,-2000) (1800,-2800)   */
    RE15_S36_SCHUPPEN_MSG, 0x00, 0xff, 0xff, 0x00, 0x00,     /* msg, Pausemaske 0xffff      */
};

static int s_roy = -1;

static int16_t le_s16(const uint8_t *p) { return (int16_t)(uint16_t)(p[0] | (p[1] << 8)); }

/* Einen sce-1-Satz (Aot_set 0x2C) GENAU so ablegen, wie es op_aot_set (scd_vm.c) im sce-1-Zweig tut:
 *   kurz (pc[3]&0x80 == 0): Ecke pc[6..9] + Ausdehnung pc[10..13] -> Mitte/halbe Ausdehnung,
 *                           msg = pc[14], Pausemaske = u16 pc[16..17];
 *   lang (pc[3]&0x80):      Viereck pc[6..21] (FUN_80014368), AABB als Mitte, msg = pc[22],
 *                           Pausemaske = u16 pc[24..25], xs/zs + has_quad;
 *   immer: band = pc[4], sce_flags = pc[3] (record[1], FUN_80042bac-Pruefbits). */
static void text_platz(const uint8_t *pc)
{
    const int slot = (int)pc[1];
    if (slot < 0 || slot >= RE15_AOT_MAX) return;
    const int lang = (pc[3] & 0x80) != 0;
    int16_t vx[4] = {0, 0, 0, 0}, vz[4] = {0, 0, 0, 0};
    int32_t cx, cz, hw, hh;
    if (lang) {
        int32_t minx, maxx, minz, maxz;
        for (int i = 0; i < 4; i++) { vx[i] = le_s16(&pc[6 + i * 4]); vz[i] = le_s16(&pc[8 + i * 4]); }
        minx = maxx = vx[0]; minz = maxz = vz[0];
        for (int i = 1; i < 4; i++) {
            if (vx[i] < minx) minx = vx[i];
            if (vx[i] > maxx) maxx = vx[i];
            if (vz[i] < minz) minz = vz[i];
            if (vz[i] > maxz) maxz = vz[i];
        }
        cx = (minx + maxx) / 2; cz = (minz + maxz) / 2;
        hw = (maxx - minx) / 2; hh = (maxz - minz) / 2;
    } else {
        const int16_t rx = le_s16(&pc[6]), rz = le_s16(&pc[8]);
        const int16_t rw = le_s16(&pc[10]), rd = le_s16(&pc[12]);
        cx = (int32_t)rx + (int32_t)rw / 2;
        cz = (int32_t)rz + (int32_t)rd / 2;
        hw = (int32_t)(rw < 0 ? -rw : rw) / 2;
        hh = (int32_t)(rd < 0 ? -rd : rd) / 2;
    }
    const uint8_t  msg   = lang ? pc[22] : pc[14];
    const uint16_t maske = lang ? (uint16_t)(pc[24] | (pc[25] << 8)) : (uint16_t)(pc[16] | (pc[17] << 8));
    re15_aot_set(slot, RE15_AOT_TYPE_MESSAGE, msg, cx, cz, hw, hh);
    g_aot.slots[slot].pause_mask16 = maske;
    g_aot.slots[slot].band         = pc[4];
    g_aot.slots[slot].sce_flags    = pc[3];
    if (lang) {
        for (int k = 0; k < 4; k++) { g_aot.slots[slot].xs[k] = vx[k]; g_aot.slots[slot].zs[k] = vz[k]; }
        g_aot.slots[slot].has_quad = 1;
    }
}

/* Eine Tabellen-Nachricht ablegen (Text + Dauer, wie re15_msg_load_room_block). */
static void ablegen(const re15_story_r36_text_t *t)
{
    re15_msg_install_text(t->id, t->bytes, (size_t)t->len);
    {
        int d = re15_msg_compute_duration(t->bytes, (size_t)t->len, 0);
        if (d > 0 && d < 65535) re15_msg_install_durations(t->id, d);
    }
}

static const re15_story_r36_text_t *finde(uint16_t raum, uint8_t id)
{
    for (int i = 0; i < N_TEXTE; i++)
        if (k_s36_texte[i].raum == raum && k_s36_texte[i].id == id) return &k_s36_texte[i];
    return 0;
}

void re15_story_r36_install(uint16_t room_id)
{
    s_roy = -1;

    /* P1 — ROOM1020 (nur Leon): ohne Strom (4,0xF3) weder Leiche noch Anklick-Platz. */
    if (room_id == RE15_S36_ROY_RAUM) {
        if (re15_game_flag_get(RE15_S36_STROM_BANK, RE15_S36_STROM_BIT)) {
            s_roy = 1;                                    /* Original-Zustand: Roy liegt da */
        } else {
            scd_prop_hide_by_obj_id(RE15_S36_ROY_OBJ);    /* Zeichen-Bit weg (@0x80021fc8)  */
            /* Aot_reset(5, sce 0) = ROOM1020 sub02 @0x01F70 — nur den echten Ereignis-Satz. */
            if (g_aot.slots[RE15_S36_ROY_SLOT].active &&
                (g_aot.slots[RE15_S36_ROY_SLOT].type == RE15_AOT_TYPE_GENERIC ||
                 g_aot.slots[RE15_S36_ROY_SLOT].type == RE15_AOT_TYPE_AUTO_EVENT))
                re15_aot_retype(RE15_S36_ROY_SLOT, 0, 0x00, 0, 0, 0);
            s_roy = 0;
        }
#ifdef RE15_PLATFORM_PC
        fprintf(stderr, "[story36] ROOM1020: Strom (4,0xF3)=%d -> Roy %s\n",
                re15_game_flag_get(RE15_S36_STROM_BANK, RE15_S36_STROM_BIT),
                s_roy ? "liegt da (obj 6 + Slot 5 wie Original)" : "noch nicht da (obj 6 versteckt, Slot 5 sce 0)");
#endif
        return;
    }

    /* P2 — ROOM1210 (Leon): Text-Platz an der offenen Zelle, eigene Nachricht Id 20. */
    if (room_id == RE15_S36_ZELLE_RAUM) {
        const re15_story_r36_text_t *t = finde(RE15_S36_ZELLE_RAUM, RE15_S36_ZELLE_MSG);
        if (t && !g_aot.slots[RE15_S36_ZELLE_SLOT].active) {
            ablegen(t);
            text_platz(k_zelle_satz);
#ifdef RE15_PLATFORM_PC
            fprintf(stderr, "[story36] ROOM1210: Text-Platz Slot %d (Rechteck ROOM1211 @0x01DEE) -> msg %d\n",
                    RE15_S36_ZELLE_SLOT, RE15_S36_ZELLE_MSG);
#endif
        }
        return;
    }

    /* P3 — ROOM1091 (Elza): Text-Platz am ausgebrannten Schuppen, eigene Nachricht Id 20. */
    if (room_id == RE15_S36_SCHUPPEN_RAUM) {
        const re15_story_r36_text_t *t = finde(RE15_S36_SCHUPPEN_RAUM, RE15_S36_SCHUPPEN_MSG);
        if (t && !g_aot.slots[RE15_S36_SCHUPPEN_SLOT].active) {
            ablegen(t);
            text_platz(k_schuppen_satz);
#ifdef RE15_PLATFORM_PC
            fprintf(stderr, "[story36] ROOM1091: Text-Platz Slot %d (Viereck ROOM1090 @0x0230E) -> msg %d\n",
                    RE15_S36_SCHUPPEN_SLOT, RE15_S36_SCHUPPEN_MSG);
#endif
        }
        return;
    }
}

void re15_story_r36_texte(uint16_t room_id)
{
    int n = 0;
    for (int i = 0; i < N_TEXTE; i++) {
        const re15_story_r36_text_t *t = &k_s36_texte[i];
        if (t->raum != room_id || t->off_alt == 0) continue;   /* nur Overrides bestehender Ids */
        ablegen(t);
        n++;
    }
#ifdef RE15_PLATFORM_PC
    if (n) fprintf(stderr, "[story36] ROOM%04X: %d Nachrichten umgeschrieben (nie begegnet)\n",
                   (unsigned)room_id, n);
#else
    (void)n;
#endif
}

const re15_story_r36_text_t *re15_story_r36_tabelle(int *n)
{
    if (n) *n = N_TEXTE;
    return k_s36_texte;
}

int re15_story_r36_roy_sichtbar(void) { return s_roy; }
