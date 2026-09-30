/* probe_r34n_f_leiche.c — MESS-SONDE (Runde 34 Nacht, Spur F): die zwei Leichen-Ereignisse
 * ROOM1110 (Slot 5 -> sub02) und ROOM1230 (Slot 18 -> sub21) im ECHTEN Bild-Ablauf des Ports.
 *
 * ⛔ WERKZEUG DES RE-SCHRITTS, VOR DEM BAU. Seit dem Bau (leiche_1110_1230.c, Haken in
 * op_message_on und re15_game_step) laufen die echten Haken im Engine-Code mit: der Modus "ist"
 * misst deshalb NICHT mehr den Auslieferungsstand, und die "soll"-Simulation liegt auf den echten
 * Haken drauf. Die Messung des gebauten Stands ist der Riegel test_r34n_f_leiche.
 *
 * Kein Pin, kein Spielverhalten, KEIN Port-Code — misst den Ist-Stand und fuehrt den BAUPLAN
 * als Simulation IN DER SONDE vor (analysis/befunde_runde34_nacht/F_leichen.md, 2/4/5/6):
 *
 *   ist        der ausgelieferte Ablauf: Bild des Drucks, Nachricht (Id, Freeze), Seiten,
 *              Schliessen, Programmzaehler des Ereignis-Fadens (Datei-Offset), Modal (nie),
 *              Platz am Ende wieder scharf.
 *   soll-nein  SIMULATION des Bauplans: der neue lange Text liegt (nur in der Sonde) auf der
 *              Leichen-Nachricht; sobald er zu ist, oeffnet die Sonde das Aufnahme-Modal genau
 *              so, wie es re15_leiche_tick tun soll: re15_item_modal_start(0x15, 15, Bit, -1, 0xFF),
 *              an derselben Stelle im Bild (vor re15_game_step). Antwort "No" -> Bit bleibt 0,
 *              Inventar unveraendert, Faden steht waehrend des Modals, zweites Untersuchen bietet
 *              wieder an.
 *   soll-ja    wie oben, Antwort "Yes" -> Bit gesetzt, H. Gun Bullets +7 (15 halbiert,
 *              re15_pickup_menge_nutzer), gestapelt; zweites Untersuchen -> kurzer Text, KEIN Modal.
 *   soll-voll  Inventar voll ohne Handgun-Munition -> "can't carry" (Zustand 8), Bit bleibt 0.
 *
 * Bild-Reihenfolge wie main.c: [SCD-Takt ausser bei laufendem Modal, main.c:5343] ->
 * re15_msg_tick (main.c:5503) -> (simulierter Haken) -> re15_game_step (main.c:7358) ->
 * re15_item_modal_tick (main.c:7380).
 *
 * Aufruf: probe_r34n_f_leiche [1110|1111|1230|1231|alle] [ist|soll-nein|soll-ja|soll-voll|alle]
 */
#include "re15_rdt.h"
#include "re15_scd.h"
#include "re15_actor.h"
#include "re15_aot.h"
#include "re15_room.h"
#include "re15_player.h"
#include "re15_camera.h"
#include "re15_game_step.h"
#include "re15_collision.h"
#include "re15_inventory.h"
#include "re15_msg.h"
#include "re15_enemy_ai.h"
#include "re15_enemy.h"
#include "re15_skeleton.h"      /* re15_sin_q12 / re15_cos_q12 */
#include "re15_item_modal.h"
#include "re15_engine.h"        /* re15_pad_virtual_word */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef RE15_ASSET_PSX_DIR
#define RE15_ASSET_PSX_DIR "shared_assets/PSX"
#endif

extern uint32_t g_re15_pauseflags;

/* ---- die neuen Texte (Bauplan 5.3; jedes Byte aus dem Auslieferungsstand belegt) ---- */
static const uint8_t k_1110_lang[] = {
    0x04, 0x02,
    0x25,0x50,0x3a,0x4f,0x00,0x3d,0x00,0x4c,0x4b,0x48,0x45,0x3f,0x41,0x00,0x4b,0x42,0x42,0x45,0x3f,
    0x41,0x4e,0x18,0x00,0x44,0x41,0x3a,0x4f,0x00,0x40,0x41,0x3d,0x40,0x57,   /* ROOM1110 @0x0D6A */
    0x02, 0x00,                                                               /* ROOM1110 @0x0D8B */
    0x24,0x41,0x00,0x45,0x4f,0x00,0x44,0x4b,0x48,0x40,0x45,0x4a,0x43,        /* ROOM1110 @0x0D8D */
    0x00,0x4f,0x4b,0x49,0x41,0x50,0x44,0x45,0x4a,0x43,                      /* ROOM1110 @0x0EC4 */
    0x57,                                                                     /* ROOM1110 @0x0DA1 */
    0x01, 0x00,
};
static const uint8_t k_1110_kurz[] = {
    0x04, 0x02,
    0x25,0x50,0x3a,0x4f,0x00,0x3d,0x00,0x4c,0x4b,0x48,0x45,0x3f,0x41,0x00,0x4b,0x42,0x42,0x45,0x3f,
    0x41,0x4e,0x18,0x00,0x44,0x41,0x3a,0x4f,0x00,0x40,0x41,0x3d,0x40,0x57,
    0x01, 0x00,
};
static const uint8_t k_1230_lang[] = {
    0x04, 0x02,
    0x1d,0x00,0x49,0x45,0x4f,0x41,0x4e,0x3d,0x3e,0x48,0x41,0x00,0x40,0x41,0x3d,0x50,0x44,
    0x57,0x57,0x57,                                                           /* ROOM1230 @0x16F6 */
    0x02, 0x00,                                                               /* ROOM1230 @0x170A */
    0x24,0x41,0x00,0x45,0x4f,0x00,0x44,0x4b,0x48,0x40,0x45,0x4a,0x43,        /* ROOM1230 @0x170C */
    0x00,0x4f,0x4b,0x49,0x41,0x50,0x44,0x45,0x4a,0x43,                      /* ROOM1110 @0x0EC4 */
    0x57,                                                                     /* ROOM1230 @0x1720 */
    0x01, 0x00,
};
static const uint8_t k_1230_kurz[] = {
    0x04, 0x02,
    0x1d,0x00,0x49,0x45,0x4f,0x41,0x4e,0x3d,0x3e,0x48,0x41,0x00,0x40,0x41,0x3d,0x50,0x44,
    0x57,0x57,0x57,
    0x01, 0x00,
};

#define SIM_ITEM   0x15   /* H. Gun Bullets */
#define SIM_MENGE  15     /* Modus RE1.5 22/38, RE2 37/44 (Dossier 3.6) */

static uint8_t           *s_raw = NULL;
static size_t             s_rawsz = 0;
static re15_rdt_t         s_rdt;
static re15_camera_view_t s_cam;
static re15_game_ctx_t    s_ctx;
static int                s_shown = 0;
static int                s_fehler = 0;

/* simulierter Haken (= geplantes re15_leiche_message_on / re15_leiche_tick) */
static int     s_sim = 0, s_sim_warte = 0, s_sim_scharf = 0, s_sim_msg = -1;
static uint8_t s_sim_bit = 0;

#define PRUEF(c, ...) do { if (!(c)) { printf("  FEHLER: "); printf(__VA_ARGS__); printf("\n"); s_fehler++; } \
                           else { printf("  ok: "); printf(__VA_ARGS__); printf("\n"); } } while (0)

static uint8_t *slurp(const char *p, size_t *n)
{
    FILE *f = fopen(p, "rb");
    if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    uint8_t *b = (uint8_t *)malloc((size_t)sz);
    if (b && fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); b = NULL; }
    fclose(f);
    if (n) *n = (size_t)sz;
    return b;
}

static int s_modal_starts = 0;
static void frame(uint16_t held, uint16_t edge)
{
    const unsigned char *raw; int len, id;
    if (!re15_item_modal_active()) scd_vm_tick();        /* main.c:5343 Modal-Freeze des SCD-Takts */
    re15_msg_tick(&raw, &len, &id);
    if (re15_cam_present_tick()) s_shown = (int)g_scd.cam_id;
    /* SIMULIERTER Message_on-HAKEN: die Leichen-Nachricht ist aufgegangen -> Angebot scharf
     * (Bauplan 5.1, re15_leiche_message_on setzt es beim Oeffnen, NUR ohne Bit) */
    if (s_sim && s_sim_warte && g_scd.message_active && (int)g_scd.message_id == s_sim_msg) {
        s_sim_warte = 0;
        s_sim_scharf = !re15_game_flag_get(9, s_sim_bit);
    }
    /* SIMULIERTER TICK-HAKEN: Text der Leiche zu -> Aufnahme-Modal (Bauplan 5.1, re15_leiche_tick) */
    if (s_sim && s_sim_scharf && !g_scd.message_active && !re15_item_modal_active()) {
        s_sim_scharf = 0;
        if (!re15_game_flag_get(9, s_sim_bit)) {
            re15_item_modal_start(SIM_ITEM, SIM_MENGE, s_sim_bit, -1, 0xFF);
            s_modal_starts++;
        }
    }
    s_ctx.active_cut  = s_shown;
    s_ctx.pad_current = held;
    s_ctx.pad_pressed = edge;
    re15_game_step(&s_ctx);
    if (re15_item_modal_active())
        re15_item_modal_tick(re15_pad_virtual_word(edge), re15_pad_virtual_word(held));
    scd_audio_event_t e;
    while (scd_audio_queue_pop(&e)) { }
}

static int room_boot(uint16_t room)
{
    char rp[600];
    snprintf(rp, sizeof rp, "%s/STAGE%u/ROOM%04X.RDT", RE15_ASSET_PSX_DIR,
             (unsigned)(room >> 12), (unsigned)room);
    free(s_raw); s_raw = slurp(rp, &s_rawsz);
    if (!s_raw || s_rawsz < 0x100 || re15_rdt_parse(s_raw, s_rawsz, &s_rdt) < 0) {
        printf("  FEHLER: ROOM%04X nicht ladbar\n", room); s_fehler++; return -1;
    }
    memset(&s_cam, 0, sizeof s_cam); memset(&s_ctx, 0, sizeof s_ctx);
    s_ctx.rdt = &s_rdt; s_ctx.rdt_ok = 1; s_ctx.cam_view = &s_cam; s_ctx.active_cut = 0;
    re15_game_state_t flags_vorher = g_game;
    re15_actor_init(); re15_aot_init(); scd_vm_init();
    g_game = flags_vorher;
    re15_enemy_reset(); re15_enemy_ai_set_paused(0);
    re15_player_cmd_reset();
    re15_pauseflags_clear();
    g_current_room_id = room; g_room_change.pending = 0;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100; pl->hit_react = 0;
    pl->state = 0; pl->motion = 0; pl->floor = 0; pl->y = 0;
    pl->x = 0; pl->z = 0; pl->rot_y = 0;
    re15_collision_set_band(0);
    s_shown = 0;
    re15_msg_load_room_block(s_rdt.messages, s_rdt.messages_size);
    scd_room_reenter(&s_rdt, pl->x, pl->z, 0);
    re15_msg_load_room_block(s_rdt.messages, s_rdt.messages_size);   /* wie room_common.c:392 */
    for (int f = 0; f < 30; f++) frame(0, 0);
    return 0;
}

static int vorwaerts_trifft(const re15_aot_t *a, int32_t px, int32_t pz, int yaw)
{
    int32_t c = re15_cos_q12(yaw), s = re15_sin_q12(yaw);
    int32_t fx = px + (int32_t)((620 * c) >> 12);
    int32_t fz = pz - (int32_t)((620 * s) >> 12);
    long dx = (long)fx - (long)a->x, dz = (long)fz - (long)a->z;
    if (dx < 0) dx = -dx;
    if (dz < 0) dz = -dz;
    return dx <= a->half_w && dz <= a->half_h;
}

static int stellen(int slot)
{
    re15_aot_t *a = &g_aot.slots[slot];
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    static const int dxf[5] = { 0, 1, -1, 0, 0 }, dzf[5] = { 0, 0, 0, 1, -1 };
    for (int k = 0; k < 5; k++) {
        long zx = a->x + dxf[k] * (long)(a->half_w / 2);
        long zz = a->z + dzf[k] * (long)(a->half_h / 2);
        for (int d = 0; d < 16; d++) {
            int yaw = d * 256;
            int32_t c = re15_cos_q12(yaw), s = re15_sin_q12(yaw);
            int32_t px = (int32_t)zx - (int32_t)((620 * c) >> 12);
            int32_t pz = (int32_t)zz + (int32_t)((620 * s) >> 12);
            pl->rot_y = (int16_t)yaw; pl->x = px; pl->z = pz;
            frame(0, 0);
            g_room_change.pending = 0;
            if (pl->x == px && pl->z == pz && (int)pl->rot_y == yaw && vorwaerts_trifft(a, px, pz, yaw))
                return 1;
        }
    }
    return 0;
}

static long faden_pc(int *out_slot)
{
    for (int s = SCD_EVENT_SLOT_FIRST; s <= SCD_EVENT_SLOT_LAST; s++) {
        const scd_thread_t *t = &g_scd.threads[s];
        if (t->active && t->pc && t->pc >= s_raw && t->pc < s_raw + s_rawsz) {
            if (out_slot) *out_slot = s;
            return (long)(t->pc - s_raw);
        }
    }
    return -1;
}

typedef struct {
    int nachrichten, erste_id, seiten, zu_bild, modal_auf, modal_zu, modal_je, scharf, ende_bild;
    long pc_bei_zu, pc_min_modal, pc_max_modal;
    int raw_len;
    unsigned char raw_kopf[4];
} ablauf_t;

/* EIN Untersuchen: Druck, Text blaettern, Modal beantworten (antwort 0 = Yes, 1 = No). */
static ablauf_t untersuchen(int slot, int antwort, int laut)
{
    ablauf_t r; memset(&r, 0, sizeof r);
    r.erste_id = -1; r.zu_bild = -1; r.modal_auf = -1; r.modal_zu = -1; r.scharf = -1;
    r.pc_bei_zu = -1; r.pc_min_modal = 0x7fffffff; r.pc_max_modal = -1; r.ende_bild = -1;
    long pc_alt = -2; int msg_alt = 0, fsm_alt = -1, modal_alt = 0, gewaehlt = 0;
    s_modal_starts = 0;
    for (int f = 0; f < 900; f++) {
        uint16_t e = 0;
        if (f == 1) e = RE15_PAD_BIT_SQUARE;
        else if (g_scd.message_active && g_scd.message_fsm == 1 && (f % 10) == 0) e = RE15_PAD_BIT_SQUARE;
        else if (g_scd.message_active && g_scd.message_parse > 0 && (f % 10) == 0) {
            int rl = 0; re15_msg_get_raw((int)g_scd.message_id, &rl);
            if (g_scd.message_parse >= rl - 2) e = RE15_PAD_BIT_SQUARE;
        } else if (re15_item_modal_active() && re15_item_modal_prompt_ready()) {
            uint8_t ty; int ch = 0;
            int pr = re15_item_modal_prompt(&ty, &ch);
            if (pr == 2 && (f % 10) == 0) e = RE15_PAD_BIT_SQUARE;              /* can't carry: weg */
            else if (pr == 1 && ch != antwort && (f % 10) == 0) e = RE15_PAD_BIT_RIGHT; /* Ja/Nein */
            else if (pr == 1 && ch == antwort && (f % 10) == 5 && !gewaehlt) { e = RE15_PAD_BIT_SQUARE; gewaehlt = 1; }
        }
        frame(e, e);

        int sl = -1; long pc = faden_pc(&sl);
        if (laut && pc != pc_alt) printf("    F%03d Faden slot %d pc @0x%05lX%s\n", f, sl, pc, pc < 0 ? " (beendet)" : "");
        pc_alt = pc;
        if (g_scd.message_active && !msg_alt) {
            r.nachrichten++;
            if (r.erste_id < 0) {
                r.erste_id = (int)g_scd.message_id;
                const unsigned char *rw = re15_msg_get_raw(r.erste_id, &r.raw_len);
                if (rw) memcpy(r.raw_kopf, rw, 4);
            }
            if (laut) printf("    F%03d Nachricht AUF id=%d Freeze=0x%08X\n", f, (int)g_scd.message_id,
                             (unsigned)g_re15_pauseflags);
        }
        if (g_scd.message_active && (int)g_scd.message_fsm != fsm_alt) {
            if (g_scd.message_fsm == 1) { r.seiten++; if (laut) printf("    F%03d Seite %d fertig\n", f, r.seiten); }
            fsm_alt = (int)g_scd.message_fsm;
        }
        if (!g_scd.message_active && msg_alt) {
            if (r.zu_bild < 0) { r.zu_bild = f; r.pc_bei_zu = pc; }
            if (laut) printf("    F%03d Nachricht ZU, Faden @0x%05lX\n", f, pc);
            fsm_alt = -1;
        }
        msg_alt = g_scd.message_active;
        int mo = re15_item_modal_active();
        if (mo && !modal_alt) { r.modal_je = 1; r.modal_auf = f; if (laut) printf("    F%03d MODAL AUF (Zustand %d)\n", f, re15_item_modal_state()); }
        if (mo) { if (pc < r.pc_min_modal) r.pc_min_modal = pc; if (pc > r.pc_max_modal) r.pc_max_modal = pc; }
        if (!mo && modal_alt) { r.modal_zu = f; if (laut) printf("    F%03d MODAL ZU\n", f); }
        modal_alt = mo;
        if (r.zu_bild >= 0 && pc < 0 && !mo && r.scharf < 0) {
            r.scharf = g_aot.slots[slot].active ? 1 : 0;
            r.ende_bild = f;
            if (laut) printf("    F%03d Ereignis beendet, Platz %d aktiv=%d\n", f, slot, g_aot.slots[slot].active);
            break;
        }
    }
    return r;
}

static int inv_menge(uint8_t id)
{
    int n = 0;
    for (int i = 0; i < 10; i++) if (g_inv.slots[i].id == id) n += g_inv.slots[i].qty;
    return n;
}

static void lauf(uint16_t room, const char *modus)
{
    int slot = ((room & 0xFFF0u) == 0x1110u) ? 5 : 18;
    int msg  = ((room & 0xFFF0u) == 0x1110u) ? 0 : 10;
    uint8_t bit = ((room & 0xFFF0u) == 0x1110u) ? 61 : 62;
    const uint8_t *lang = ((room & 0xFFF0u) == 0x1110u) ? k_1110_lang : k_1230_lang;
    size_t lang_n = ((room & 0xFFF0u) == 0x1110u) ? sizeof k_1110_lang : sizeof k_1230_lang;
    const uint8_t *kurz = ((room & 0xFFF0u) == 0x1110u) ? k_1110_kurz : k_1230_kurz;
    size_t kurz_n = ((room & 0xFFF0u) == 0x1110u) ? sizeof k_1110_kurz : sizeof k_1230_kurz;

    printf("\n=== ROOM%04X, Modus %s (Platz %d, Nachricht %d, Bit (9,%d)) ===\n", room, modus, slot, msg, bit);
    re15_game_state_init();
    /* soll-voll: Inventar voll OHNE Handgun-Munition (10 Plaetze Ids 0x22..0x2B), s.u. */
    s_sim = strcmp(modus, "ist") != 0;
    s_sim_bit = bit; s_sim_msg = msg; s_sim_scharf = 0; s_sim_warte = 0;
    if (room_boot(room) != 0) return;
    re15_inv_load_briefing();                 /* NACH dem Raumaufbau (Inventar = Spielstand) */
    if (!strcmp(modus, "soll-voll"))
        for (int i = 0; i < 10; i++) { g_inv.slots[i].id = (uint8_t)(0x22 + i); g_inv.slots[i].qty = 1; g_inv.slots[i].flags = 0; }
    if (!stellen(slot)) { printf("  FEHLER: kein Standplatz vor Platz %d\n", slot); s_fehler++; return; }
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    printf("  Standplatz (%d,%d) Blick %d, Munition 0x15 vorher %d\n", (int)pl->x, (int)pl->z,
           (int)pl->rot_y, inv_menge(SIM_ITEM));

    if (!s_sim) {
        ablauf_t r = untersuchen(slot, 1, 1);
        printf("  ERGEBNIS ist: Nachricht id %d, %d Seitenstopps, zu F%d, Faden bei ZU @0x%05lX, Modal je %d, scharf %d\n",
               r.erste_id, r.seiten, r.zu_bild, r.pc_bei_zu, r.modal_je, r.scharf);
        PRUEF(r.erste_id == msg, "Ist: Original-Nachricht %d", msg);
        PRUEF(r.modal_je == 0, "Ist: kein Item-Modal");
        PRUEF(r.scharf == 1, "Ist: Platz %d danach wieder scharf", slot);
        return;
    }

    int antwort = !strcmp(modus, "soll-ja") ? 0 : 1;
    for (int runde = 1; runde <= 2; runde++) {
        /* Simulierter Message_on-Haken: Text je nach Bit, Angebot nur ohne Bit */
        int genommen = re15_game_flag_get(9, bit);
        re15_msg_install_text((unsigned char)msg, genommen ? kurz : lang, genommen ? kurz_n : lang_n);
        s_sim_warte = 1; s_sim_scharf = 0;
        int vorher = inv_menge(SIM_ITEM);
        ablauf_t r = untersuchen(slot, antwort, runde == 1);
        int nachher = inv_menge(SIM_ITEM);
        printf("  Runde %d: Text %s (%d B), %d Seitenstopps, zu F%d, Modal auf F%d zu F%d (Starts %d), "
               "Faden waehrend Modal @0x%05lX..@0x%05lX, Ende F%d, Bit=%d, Munition %d -> %d, scharf %d\n",
               runde, genommen ? "kurz" : "lang", r.raw_len, r.seiten, r.zu_bild, r.modal_auf, r.modal_zu,
               s_modal_starts, r.pc_min_modal, r.pc_max_modal, r.ende_bild, re15_game_flag_get(9, bit),
               vorher, nachher, r.scharf);
        PRUEF(r.scharf == 1, "Runde %d: Platz %d am Ende wieder scharf", runde, slot);
        if (!genommen) {
            PRUEF(r.modal_je == 1 && r.modal_auf == r.zu_bild, "Runde %d: Modal geht im Bild auf, in dem der Text zugeht (F%d/F%d)",
                  runde, r.modal_auf, r.zu_bild);
            PRUEF(r.pc_min_modal == r.pc_bei_zu && r.pc_max_modal == r.pc_bei_zu,
                  "Runde %d: Faden steht waehrend des Modals auf @0x%05lX (nach Evt_next, vor der 2. Plc_motion)",
                  runde, r.pc_bei_zu);
            if (antwort == 0 && strcmp(modus, "soll-voll") != 0) {
                PRUEF(re15_game_flag_get(9, bit) == 1, "Runde %d Yes: Bit (9,%d) gesetzt", runde, bit);
                PRUEF(nachher == vorher + SIM_MENGE / 2, "Runde %d Yes: +%d H. Gun Bullets (15 halbiert)", runde, SIM_MENGE / 2);
            } else {
                PRUEF(re15_game_flag_get(9, bit) == 0, "Runde %d %s: Bit (9,%d) bleibt 0", runde,
                      strcmp(modus, "soll-voll") ? "No" : "voll", bit);
                PRUEF(nachher == vorher, "Runde %d: Inventar unveraendert", runde);
            }
        } else {
            PRUEF(r.modal_je == 0, "Runde %d nach Yes: kein Modal mehr", runde);
            PRUEF(r.seiten == 0, "Runde %d nach Yes: kurzer Text ohne Seitenstopp", runde);
        }
    }
}

int main(int argc, char **argv)
{
    const char *rw = argc > 1 ? argv[1] : "alle";
    const char *mw = argc > 2 ? argv[2] : "alle";
    static const uint16_t raeume[4] = { 0x1110, 0x1111, 0x1230, 0x1231 };
    static const char *const modi[4] = { "ist", "soll-nein", "soll-ja", "soll-voll" };
    for (int i = 0; i < 4; i++) {
        char rn[8]; snprintf(rn, sizeof rn, "%04X", raeume[i]);
        if (strcmp(rw, "alle") && strcmp(rw, rn)) continue;
        for (int m = 0; m < 4; m++)
            if (!strcmp(mw, "alle") || !strcmp(mw, modi[m])) lauf(raeume[i], modi[m]);
    }
    printf("\nprobe_r34n_f_leiche: %d FEHLER\n", s_fehler);
    return s_fehler ? 1 : 0;
}
