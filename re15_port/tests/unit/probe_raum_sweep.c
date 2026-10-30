/* probe_raum_sweep.c — MESSSONDE (kein ctest): EIN Raum, echter re15_game_step, 600 Takte ohne Eingabe.
 *
 * Teil des Vergleichs Original <-> Port (analysis/vergleich_original_port/). Ausgabe EINE Zeile:
 *   RAUM <hex> spawns=<n> typen=<t:anz,...> aktiv_ende=<n> tot=<n> threads=<n> spieler_hp=<hp>
 * Ein Absturz/Haenger betrifft nur diesen Aufruf (das Treiber-Skript ruft je Raum neu auf).
 * Banken: RE1.5-EMS je gespawntem Typ (wie test_zombie_gore.c), RE2-KI ist Standard. */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "re15_rdt.h"
#include "re15_scd.h"
#include "re15_actor.h"
#include "re15_aot.h"
#include "re15_room.h"
#include "re15_enemy_ai.h"
#include "re15_enemy.h"
#include "re15_emd.h"
#include "re15_ems.h"
#include "re15_player.h"
#include "re15_damage.h"
#include "re15_camera.h"
#include "re15_game_step.h"
#include "re15_collision.h"
#include "re15_msg.h"
#include "re15_esp.h"

static re15_rdt_t         s_rdt;
static re15_camera_view_t s_cam;
static re15_game_ctx_t    s_ctx;

static uint8_t *slurp(const char *p, size_t *n)
{
    FILE *f = fopen(p, "rb"); if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    if (sz <= 0) { fclose(f); return NULL; }
    uint8_t *b = (uint8_t *)malloc((size_t)sz);
    if (b && fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); b = NULL; }
    fclose(f); if (b) *n = (size_t)sz; return b;
}

static uint8_t *s_ems; static size_t s_ems_n;
static void bank(uint8_t type)
{
    if (!s_ems) s_ems = slurp(RE15_ASSET_PSX_DIR "/EMD/CDEMD0.EMS", &s_ems_n);
    if (!s_ems || re15_enemy_find(type)) return;
    size_t off = 0, len = 0;
    if (re15_ems_get_entry(s_ems, s_ems_n, re15_ems_index_for_type(type), &off, &len) != 0) return;
    uint8_t *blob = (uint8_t *)malloc(len); if (!blob) return;
    memcpy(blob, s_ems + off, len);
    re15_enemy_bank_t *eb = re15_enemy_alloc(type); if (!eb) { free(blob); return; }
    re15_tim_t tim = (re15_tim_t){0};
    if (re15_emd_parse_container(blob, len, &eb->md1, &eb->skel, &eb->anim, &tim) != 0) { eb->type = 0; free(blob); return; }
    eb->ok = 1; eb->buf = NULL;
    eb->loco_ok = (re15_emd_parse_loco_bank(blob, len, &eb->skel_loco, &eb->anim_loco) == 0);
    eb->own_ok  = (re15_emd_parse_own_bank(blob, len, &eb->skel_own, &eb->anim_own) == 0);
}

int main(int argc, char **argv)
{
    unsigned room = (argc > 1) ? (unsigned)strtoul(argv[1], NULL, 16) : 0x1140u;
    char path[600];
    snprintf(path, sizeof path, RE15_ASSET_PSX_DIR "/STAGE%u/ROOM%04X.RDT", (room >> 12) & 0xF, room);
    size_t sz = 0; uint8_t *buf = slurp(path, &sz);
    if (!buf) { printf("RAUM %04X FEHLT\n", room); return 2; }
    if (re15_rdt_parse(buf, sz, &s_rdt) != 0) { printf("RAUM %04X PARSE-FEHLER\n", room); return 1; }
    /* Effekt-Baenke wie main.c: Raum-ESP (RDT+0x4C..0x58) + CORE00.ESP */
    static re15_esp_t rb, gb;
    {   uint32_t o[4]; for (int i = 0; i < 4; i++) o[i] = (uint32_t)buf[0x4C + 4*i] | (uint32_t)buf[0x4D + 4*i] << 8 | (uint32_t)buf[0x4E + 4*i] << 16 | (uint32_t)buf[0x4F + 4*i] << 24;
        if (re15_esp_parse(buf, sz, o[0], o[1], o[2], o[3], &rb) == 0) re15_esp_set_room_bank(&rb);
        size_t gn = 0; uint8_t *g = slurp(RE15_ASSET_PSX_DIR "/DATA/CORE00.ESP", &gn);
        if (g && re15_esp_parse_global(g, gn, &gb) == 0) re15_esp_set_global_bank(&gb); }
    memset(&s_cam, 0, sizeof s_cam); memset(&s_ctx, 0, sizeof s_ctx);
    s_ctx.rdt = &s_rdt; s_ctx.rdt_ok = 1; s_ctx.cam_view = &s_cam; s_ctx.active_cut = 0;

    re15_actor_init(); re15_aot_init(); scd_vm_init();
    re15_enemy_reset(); re15_enemy_ai_set_paused(0);
    re15_player_cmd_reset();
    re15_damage_seed_rng(0x1234567u);
    g_current_room_id = (uint16_t)room;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100; pl->state = 0; pl->motion = 0; pl->y = 0;
    re15_collision_set_band(0);
    re15_msg_load_room_block(s_rdt.messages, s_rdt.messages_size);
    scd_register_room_events(&s_rdt);
    scd_room_reenter(&s_rdt, pl->x, pl->z, 0);

    int spawns = 0; int seen[256] = {0};
    int takte = getenv("RAUM_TAKTE") ? atoi(getenv("RAUM_TAKTE")) : 600;
    for (int f = 0; f < takte; f++) {
        for (int s = 1; s < RE15_ACTOR_MAX; s++)
            if (g_actors[s].active && !seen[g_actors[s].type]) { seen[g_actors[s].type] = 1; bank(g_actors[s].type); }
        const unsigned char *raw; int len, id;
        re15_msg_tick(&raw, &len, &id);
        s_ctx.pad_current = 0; s_ctx.pad_pressed = (f % 40 == 20) ? 0x0040 : 0;   /* gelegentlich bestaetigen (Texte) */
        re15_game_step(&s_ctx);
    }
    if (getenv("RAUM_FX")) {
        for (int s = 1; s < RE15_ACTOR_MAX; s++) if (g_actors[s].active)
            printf("  Aktor %d Typ %02x st %d/%d pos (%d,%d,%d) +0x1d8 %04x +0x1da %d hp %d\n", s, g_actors[s].type, g_actors[s].state,
                   g_actors[s].sub_state_1, g_actors[s].x, g_actors[s].y, g_actors[s].z, g_actors[s].ai_flags, g_actors[s].ai_attack_timer, g_actors[s].hp);
        for (int i = 0; i < 256; i++) { const re15_esp_fx_t *f = re15_esp_fx_get(i);
            if (f && f->follow_slot > 0) printf("  fx %d id %d sub %d an Aktor %d Teil %d pos (%d,%d,%d) Routine %d\n", i, f->effect_id, f->sub_index,
                   f->follow_slot, f->follow_part, f->x, f->y, f->z, f->row[0] | f->row[1] << 8); }
    }
    int aktiv = 0, tot = 0; char typen[400] = ""; int cnt[256] = {0};
    for (int s = 1; s < RE15_ACTOR_MAX; s++) {
        if (!g_actors[s].active) continue;
        aktiv++; spawns++; cnt[g_actors[s].type]++;
        if (g_actors[s].state == 3 || g_actors[s].state == 7) tot++;
    }
    for (int t = 0; t < 256; t++) if (cnt[t]) { char b[24]; snprintf(b, sizeof b, "%02x:%d,", t, cnt[t]); strcat(typen, b); }
    printf("RAUM %04X aktiv=%d typen=%s tot=%d spieler_hp=%d spieler_state=%d\n",
           room, aktiv, typen[0] ? typen : "-", tot, pl->hp, pl->state);
    return 0;
}
