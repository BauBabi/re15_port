/* probe_r36_elza_mess.c — Runde 36 Spur L: MESS-SONDE (kein Riegel) fuer die Elza-Szenen.
 *
 * Laedt die ausgelieferte RDT, baut den Raum ueber scd_room_reenter auf (derselbe Einhaengepunkt wie im
 * Spiel), faehrt Bilder in der Reihenfolge von platform/pc/main.c (Muster test_r35_cut1150.c) und
 * schreibt je Bild, an welcher RDT-Datei-Stelle die Faeden stehen, plus Clip/Bild der Akteure.
 * Damit wird die Zeitlinie jeder Szene am echten VM gemessen, bevor ein Ton eingehaengt wird.
 *
 * Aufruf: probe_r36_elza_mess <raum-hex> <x> <z> <rot> <cut> <ereignis|-1> <bilder> [bank:bit ...]
 *   z.B. probe_r36_elza_mess 1211 -21567 -19911 0 7 2 1200 4:0xEC
 * R36_BILDER=<slot>: zusaetzlich je Bild Clip/Bild/Phase dieses Aktors.
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
#include "re15_msg.h"
#include "re15_enemy_ai.h"
#include "re15_enemy.h"
#include "re15_fade.h"
#include "re15_ems.h"
#include "re15_emd.h"
#include "re15_tim.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void re15_actor_step_all_walkers(void);

static uint8_t           *s_raw = NULL;
static size_t             s_rawsz = 0;
static re15_rdt_t         s_rdt;
static re15_camera_view_t s_cam;
static re15_game_ctx_t    s_ctx;
static int                s_shown = 0;
static int                s_cine_was = 0;

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

static uint8_t *s_ems = NULL; static size_t s_ems_sz = 0;
static uint8_t  s_bank_buf[4][0x60000];
static void bank_laden(uint8_t type, int buf)
{
    if (!s_ems) {
        char p2[600]; snprintf(p2, sizeof p2, "%s/EMD/CDEMD0.EMS", RE15_ASSET_PSX_DIR);
        s_ems = slurp(p2, &s_ems_sz);
    }
    if (!s_ems) return;
    int idx = re15_ems_index_for_type(type);
    size_t off = 0, len = 0;
    if (idx < 0 || re15_ems_get_entry(s_ems, s_ems_sz, idx, &off, &len) != 0) return;
    re15_enemy_bank_t *eb = re15_enemy_alloc(type);
    if (!eb || len > sizeof s_bank_buf[buf]) return;
    memcpy(s_bank_buf[buf], s_ems + off, len);
    re15_tim_t tim = (re15_tim_t){0};
    if (re15_emd_parse_container(s_bank_buf[buf], len, &eb->md1, &eb->skel, &eb->anim, &tim) != 0) return;
    eb->ok = 1; eb->buf = NULL;
    eb->loco_ok   = (re15_emd_parse_loco_bank(s_bank_buf[buf], len, &eb->skel_loco, &eb->anim_loco) == 0);
    eb->victim_ok = (re15_emd_parse_victim_bank(s_bank_buf[buf], len, &eb->skel_victim, &eb->anim_victim) == 0);
    eb->own_ok    = (re15_emd_parse_own_bank(s_bank_buf[buf], len, &eb->skel_own, &eb->anim_own) == 0);
}

static void frame(void)
{
    const unsigned char *raw; int len, id;
    scd_vm_tick();
    re15_actor_step_all_walkers();
    re15_letterbox_tick(re15_game_flag_get(1, 27));
    {
        int cine = re15_game_flag_get(1, 27) || re15_game_flag_get(2, 7);
        if (cine) { g_scd.player_mode = 2; g_scd.letterbox_countdown = -1; }
        else if (s_cine_was) g_scd.letterbox_countdown = 15;
        s_cine_was = cine;
        if (g_scd.letterbox_countdown > 0 && --g_scd.letterbox_countdown == 0) {
            g_scd.player_mode = 0;
            re15_aot_settle_at(g_actors[RE15_ACTOR_SLOT_PLAYER].x, g_actors[RE15_ACTOR_SLOT_PLAYER].z);
        }
    }
    re15_msg_tick(&raw, &len, &id);
    if (re15_cam_present_tick()) s_shown = (int)g_scd.cam_id;
    s_ctx.active_cut  = s_shown;
    s_ctx.pad_current = 0;
    s_ctx.pad_pressed = 0;
    re15_game_step(&s_ctx);
    scd_audio_event_t e;
    while (scd_audio_queue_pop(&e))
        printf("    audio kind=%d id=%d\n", (int)e.kind, (int)e.sample_id);
}

int main(int argc, char **argv)
{
    if (argc < 8) { printf("Aufruf: raum x z rot cut ereignis bilder [bank:bit ...]\n"); return 2; }
    uint16_t room = (uint16_t)strtol(argv[1], NULL, 16);
    int32_t px = atoi(argv[2]), pz = atoi(argv[3]); int16_t rot = (int16_t)atoi(argv[4]);
    uint8_t cut = (uint8_t)atoi(argv[5]); int ev = atoi(argv[6]); int n = atoi(argv[7]);
    re15_game_state_init();
    for (int i = 8; i < argc; i++) {
        char *c = strchr(argv[i], ':'); if (!c) continue;
        int b = (int)strtol(argv[i], NULL, 0), bit = (int)strtol(c + 1, NULL, 0);
        re15_game_flag_set((uint8_t)b, (uint8_t)bit, 1);
    }
    char rp[600];
    snprintf(rp, sizeof rp, "%s/STAGE%u/ROOM%04X.RDT", RE15_ASSET_PSX_DIR, (unsigned)(room >> 12), (unsigned)room);
    s_raw = slurp(rp, &s_rawsz);
    if (!s_raw || re15_rdt_parse(s_raw, s_rawsz, &s_rdt) < 0) { printf("RDT fehlt: %s\n", rp); return 1; }
    memset(&s_cam, 0, sizeof s_cam); memset(&s_ctx, 0, sizeof s_ctx);
    s_ctx.rdt = &s_rdt; s_ctx.rdt_ok = 1; s_ctx.cam_view = &s_cam; s_ctx.active_cut = cut;
    re15_game_state_t flags_vorher = g_game;
    re15_actor_init(); re15_aot_init(); scd_vm_init();
    g_game = flags_vorher;
    re15_enemy_reset(); re15_enemy_ai_set_paused(0);
    bank_laden(0x47, 0); bank_laden(0x49, 1); bank_laden(0x10, 2); bank_laden(0x45, 3);
    re15_player_cmd_reset();
    re15_pauseflags_clear();
    g_letterbox_level = 0;
    g_current_room_id = room; g_room_change.pending = 0;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100; pl->hit_react = 0;
    pl->state = 0; pl->motion = 0; pl->floor = 0; pl->y = 0;
    pl->x = px; pl->z = pz; pl->rot_y = rot;
    re15_collision_set_band(0);
    s_shown = cut; s_cine_was = 0;
    g_scd.player_mode = 0; g_scd.letterbox_countdown = 0;
    re15_msg_load_room_block(s_rdt.messages, s_rdt.messages_size);
    re15_rbj_bind_room(s_rdt.animation, s_rdt.animation_size);
    scd_room_reenter(&s_rdt, pl->x, pl->z, cut);
    re15_msg_load_room_block(s_rdt.messages, s_rdt.messages_size);
    for (int s = 1; s < RE15_ACTOR_MAX; s++)
        if (g_actors[s].active && g_actors[s].type)
            printf("Aktor %d Typ 0x%02X (%d,%d,%d) mo=%d\n", s, g_actors[s].type, (int)g_actors[s].x, (int)g_actors[s].y,
                   (int)g_actors[s].z, g_actors[s].motion);
    if (ev >= 0) { int r = scd_event_fire((uint8_t)ev); printf("scd_event_fire(%d) -> %d\n", ev, r); }
    long last[SCD_THREAD_COUNT]; for (int i = 0; i < SCD_THREAD_COUNT; i++) last[i] = -2;
    int last_mo[RE15_ACTOR_MAX]; for (int i = 0; i < RE15_ACTOR_MAX; i++) last_mo[i] = -1;
    int last_msg = -1;
    const char *bild_env = getenv("R36_BILDER");
    for (int f = 0; f < n; f++) {
        frame();
        for (int i = 2; i < SCD_THREAD_COUNT; i++) {
            const scd_thread_t *t = &g_scd.threads[i];
            long off = (t->active && t->pc) ? (long)(t->pc - s_rdt.raw) : -1;
            if (off != last[i]) {
                if (off >= 0 && off < (long)s_rdt.raw_size)
                    printf("F%4d faden %2d @0x%05lX op %02X\n", f, i, off, s_rdt.raw[off]);
                else if (off == -1 && last[i] >= 0) printf("F%4d faden %2d ENDE\n", f, i);
                last[i] = off;
            }
        }
        for (int s = 0; s < RE15_ACTOR_MAX; s++) {
            const re15_actor_t *a = &g_actors[s];
            if (!a->active) continue;
            int key = a->motion * 1000 + a->sub_state_1 * 10 + ((a->anim_flags & 0x80) ? 1 : 0);
            if (key != last_mo[s]) {
                printf("F%4d aktor %d typ %02X clip %d sub %d fl %02X (%d,%d) bild %d\n", f, s, a->type, a->motion,
                       a->sub_state_1, a->anim_flags, (int)a->x, (int)a->z, a->anim_frame);
                last_mo[s] = key;
            }
        }
        if (bild_env) {
            const re15_actor_t *a = &g_actors[atoi(bild_env)];
            printf("F%4d   [%d] clip %d bild %d ph %d st %d/%d pos (%d,%d,%d) aktiv %d\n", f, atoi(bild_env), a->motion,
                   a->anim_frame, a->sub_state_2, a->state, a->sub_state_1, (int)a->x, (int)a->y, (int)a->z, a->active);
        }
        int mid = g_scd.message_active ? (int)g_scd.message_id : -1;
        if (mid != last_msg) { printf("F%4d nachricht %d\n", f, mid); last_msg = mid; }
    }
    return 0;
}
