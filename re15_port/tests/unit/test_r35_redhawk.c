/*
 * test_r35_redhawk.c — Runde 35 Spur D "redhawk".
 *
 * Nutzer (AUFTRAG.md Z.17): "Wenn ich mit der Super Redhawk auf die Hunde schiesse bleiben die
 * Fleisch Effekte die sich rausloesen permanent da in loop."
 *
 * Dossier: analysis/befunde_runde35/D_redhawk.md.
 *
 * Aufruf: test_r35_redhawk [teil]
 *   zensus   Routinen-Zensus aller Raum-ESP-Baenke (STAGE1..6): je (Raum, Effekt-Id, Sub, Stream,
 *            Zeile) die Routinen-Waehler A (+0x00) und B (+0x02), Flags (+0x0e), +0x16, +0x1e,
 *            +0x26 und die Anim-Records (Dauer/Schleife). Reines Messwerkzeug, immer 0.
 */
#include "re15_esp.h"
#include "re15_rdt.h"
#include "re15_scd.h"
#include "re15_actor.h"
#include "re15_aot.h"
#include "re15_room.h"
#include "re15_enemy_ai.h"
#include "re15_enemy.h"
#include "re15_ai_flavor.h"
#include "re15_player.h"
#include "re15_damage.h"
#include "re15_camera.h"
#include "re15_game_step.h"
#include "re15_collision.h"
#include "re15_msg.h"
#include "re15_tim.h"
#include "re2_ems.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef RE15_ASSET_PSX_DIR
#define RE15_ASSET_PSX_DIR "shared_assets/PSX"
#endif

static uint8_t *slurp(const char *p, size_t *n)
{
    FILE *f = fopen(p, "rb"); if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    if (sz <= 0) { fclose(f); return NULL; }
    uint8_t *b = (uint8_t *)malloc((size_t)sz);
    if (b && fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); b = NULL; }
    fclose(f); if (b) *n = (size_t)sz; return b;
}
static uint32_t u32le(const uint8_t *b, size_t o)
{
    return (uint32_t)b[o] | ((uint32_t)b[o + 1] << 8) | ((uint32_t)b[o + 2] << 16) | ((uint32_t)b[o + 3] << 24);
}
static uint16_t u16le(const uint8_t *b, int o) { return (uint16_t)(b[o] | (b[o + 1] << 8)); }

/* Raum-ESP parsen wie pc_load_room_esp (platform/pc/main.c): RDT+0x4C/50/54/58. */
static int room_esp(const uint8_t *rdt, size_t n, re15_esp_t *out)
{
    if (n < 0x5C) return -1;
    return re15_esp_parse(rdt, n, u32le(rdt, 0x4C), u32le(rdt, 0x50), u32le(rdt, 0x54),
                          u32le(rdt, 0x58), out);
}

/* ===================================== ZENSUS ============================================ */
static void zensus_bank(const char *tag, const re15_esp_t *esp, unsigned seenA[64], unsigned seenB[64])
{
    for (int ei = 0; ei < esp->id_count; ei++) {
        const re15_esp_eff_t *e = &esp->eff[ei];
        printf("%s id=%u count_a=%u count_b=%u anim:", tag, e->effect_id, e->count_a, e->count_b);
        for (int k = 0; k < e->count_a && k < 40; k++) {
            re15_esp_anim_t a;
            if (re15_esp_anim(esp, ei, k, &a) != 0) break;
            printf(" [%d d%04x p%04x]", k, a.desc, a.param);
        }
        printf("\n");
        for (int sub = 0; sub < 8; sub++) {
            int ns = re15_esp_row_streams(esp, ei, sub);
            if (ns <= 0 || ns > 32) continue;
            for (int s = 0; s < ns; s++) {
                int nr = 0;
                const uint8_t *r = re15_esp_row_stream(esp, ei, sub, s, &nr);
                if (!r) { printf("%s id=%u sub=%d st=%d: (kein Stream)\n", tag, e->effect_id, sub, s); continue; }
                for (int k = 0; k < nr && k < 32; k++) {
                    const uint8_t *row = r + k * 40;
                    unsigned A = u16le(row, 0x00), B = u16le(row, 0x02);
                    if (A < 64) seenA[A]++;
                    if (B < 64) seenB[B]++;
                    printf("%s id=%u sub=%d st=%d/%d row=%d/%d A=%u B=%u acc=(%d,%d,%d) f0e=%02x vel=(%d,%d,%d)"
                           " p16=%u p1e=%u g26=%u\n",
                           tag, e->effect_id, sub, s, ns, k, nr, A, B,
                           (int16_t)u16le(row, 0x08), (int16_t)u16le(row, 0x0a), (int16_t)u16le(row, 0x0c),
                           row[0x0e],
                           (int16_t)u16le(row, 0x10), (int16_t)u16le(row, 0x12), (int16_t)u16le(row, 0x14),
                           u16le(row, 0x16), u16le(row, 0x1e), u16le(row, 0x26));
                }
            }
        }
    }
}

static int teil_zensus(void)
{
    static unsigned seenA[64], seenB[64];
    memset(seenA, 0, sizeof seenA); memset(seenB, 0, sizeof seenB);
    int rooms = 0;
    for (int st = 1; st <= 6; st++) {
        for (int r = 0; r < 0x100; r++) {
            char path[512];
            int id = (st << 12) | (r << 4);                  /* ROOMs000..sFF0 */
            snprintf(path, sizeof path, RE15_ASSET_PSX_DIR "/STAGE%d/ROOM%04X.RDT", st, id);
            size_t n = 0; uint8_t *b = slurp(path, &n);
            if (!b) continue;
            re15_esp_t esp; memset(&esp, 0, sizeof esp);
            if (room_esp(b, n, &esp) == 0) {
                char tag[32]; snprintf(tag, sizeof tag, "R%04X", id);
                printf("%s ids:", tag);
                for (int ei = 0; ei < esp.id_count; ei++) printf(" %u", esp.eff[ei].effect_id);
                printf("\n");
                zensus_bank(tag, &esp, seenA, seenB);
                rooms++;
            }
            free(b);
        }
    }
    printf("ZENSUS %d Raeume mit ESP. Routinen A:", rooms);
    for (int i = 0; i < 64; i++) if (seenA[i]) printf(" %d(%u)", i, seenA[i]);
    printf("\nZENSUS Routinen B:");
    for (int i = 0; i < 64; i++) if (seenB[i]) printf(" %d(%u)", i, seenB[i]);
    printf("\n");
    return 0;
}

/* ===================================== HUND-ARENA ========================================
 * Aufbau wie probe_r34_reaktion.c bringup_hunde/hund_arena (ROOM11D0, Flag 3:152 = 1 = freier
 * Hunde-Satz), aber MIT gebundener Raum-ESP-Bank (re15_esp_set_room_bank — die Sonde B6 hatte
 * keine, dort liefen die Spawns ins Leere) und der globalen CORE00.ESP. Der ESP-Takt laeuft wie
 * auf der Plattform hinter dem Spielschritt (fx_plattform_pc.c: re15_esp_fx_tick nach
 * re15_game_step = RE1.5 @0x8001ce2c hinter Gegnern @0x8001ce04 und Spieler @0x8001ce0c). */
static re15_rdt_t         s_rdt;
static uint8_t           *s_rdt_buf = NULL;
static int                s_room_id = 0;
static re15_camera_view_t s_cam;
static re15_game_ctx_t    s_ctx;
static re15_esp_t         s_room_esp, s_global_esp;
static uint8_t           *s_core = NULL;

static const uint8_t *re2_ems_blob(size_t *sz)
{
    static uint8_t *b = NULL; static size_t s = 0; static int tried = 0;
    if (!tried) { tried = 1; b = slurp(RE15_ASSET_PSX_DIR "/../RE2/CDEMD0.EMS", &s); }
    *sz = s; return b;
}
static void load_re2_bank(uint8_t type)
{
    if (re15_enemy_find(type)) return;
    re15_enemy_bank_t *eb = re15_enemy_alloc(type);
    if (!eb) return;
    size_t sz = 0; const uint8_t *ems = re2_ems_blob(&sz);
    re15_tim_t tim; memset(&tim, 0, sizeof tim);
    if (ems && re2_ems_load_bank(ems, sz, type, eb, &tim) == 0) { eb->buf = NULL; eb->ok = 1; return; }
    eb->type = 0;
}

static void frame(void)
{
    const unsigned char *raw; int len, id;
    re15_msg_tick(&raw, &len, &id);
    s_ctx.pad_current = 0; s_ctx.pad_pressed = 0;
    g_actors[RE15_ACTOR_SLOT_PLAYER].hp = 100;
    re15_game_step(&s_ctx);
    re15_esp_fx_tick(re15_esp_room_bank());
}

static int room_load(int room_id, const char *stage)
{
    char path[512];
    snprintf(path, sizeof path, RE15_ASSET_PSX_DIR "/%s/ROOM%04X.RDT", stage, room_id);
    free(s_rdt_buf); s_rdt_buf = NULL;
    size_t n = 0;
    s_rdt_buf = slurp(path, &n);
    if (!s_rdt_buf || re15_rdt_parse(s_rdt_buf, n, &s_rdt) != 0) { printf("RDT %s fehlt\n", path); return -1; }
    s_room_id = room_id;
    memset(&s_ctx, 0, sizeof s_ctx);
    s_ctx.rdt = &s_rdt; s_ctx.rdt_ok = 1; s_ctx.cam_view = &s_cam; s_ctx.active_cut = 0;
    memset(&s_room_esp, 0, sizeof s_room_esp);
    if (room_esp(s_rdt_buf, n, &s_room_esp) != 0) { printf("ROOM%04X ohne ESP\n", room_id); return -1; }
    if (!s_core) {
        size_t cs = 0; s_core = slurp(RE15_ASSET_PSX_DIR "/DATA/CORE00.ESP", &cs);
        if (s_core && re15_esp_parse_global(s_core, cs, &s_global_esp) == 0) re15_esp_set_global_bank(&s_global_esp);
    }
    return 0;
}

static void bringup_hunde(void)
{
    re15_ai_flavor_set(RE15_AI_FLAVOR_RE2);
    re15_game_state_init();
    re15_actor_init(); re15_aot_init(); scd_vm_init();
    re15_enemy_reset(); re15_enemy_ai_set_paused(0);
    load_re2_bank(0x20);
    re15_player_cmd_reset();
    re15_esp_fx_reset();
    re15_esp_set_room_bank(&s_room_esp);
    re15_damage_seed_rng(0x0badf00du);
    { extern void re15_re2z_rng_reset(void); re15_re2z_rng_reset(); }
    g_room_rdt = s_rdt; g_room_rdt_ok = 1;
    g_current_room_id = (uint16_t)s_room_id; g_room_change.pending = 0;
    re15_msg_load_room_block(s_rdt.messages, s_rdt.messages_size);
    scd_register_room_events(&s_rdt);
    re15_game_flag_set(3, 152, 1);          /* freier Hunde-Satz (probe_r34_reaktion.c:426) */
    if (s_rdt.main_scd)   scd_thread_start(0, s_rdt.main_scd);
    if (s_rdt.sub_scd[0]) scd_thread_start(1, s_rdt.sub_scd[0]);
    for (int i = 0; i < 120; i++) scd_vm_tick();
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100; pl->hit_react = 0;
    pl->state = 0; pl->motion = 0; pl->floor = 0; pl->y = 0;
    re15_collision_set_band(0);
}

/* Erster Hund allein, Spieler `abstand` vor ihm (Blick +x = rot 0, re2z_thrust-Konvention). */
static re15_actor_t *hund_arena(int32_t abstand)
{
    bringup_hunde();
    for (int f = 0; f < 30; f++) frame();
    int slot = -1;
    for (int s = 1; s < RE15_ACTOR_MAX; s++)
        if (g_actors[s].active && g_actors[s].type == 0x20) { slot = s; break; }
    if (slot < 0) return NULL;
    for (int s = 1; s < RE15_ACTOR_MAX; s++) if (s != slot) g_actors[s].active = 0;
    re15_actor_t *e = &g_actors[slot];
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    if (e->state != 1) re15_ai_set_state_word(e, 0x100);
    e->grid_id = 0; e->re2z_f10e = 0; e->re2z_self1d3 = 0; e->hit_react = 0;
    pl->x = e->x - abstand; pl->z = e->z; pl->y = e->y; pl->rot_y = 0;
    for (int f = 0; f < 3; f++) frame();
    e->re2z_self1d3 = 0; e->hit_react = 0;
    re15_esp_fx_reset();
    return e;
}

/* Zaehlt lebende Plaetze einer Effekt-Id (aktiv UND, bei Zeilen-VM, Flags-Bit 0). */
static int lebend(int id, int nur_sichtbar)
{
    int n = 0;
    for (int i = 0; i < RE15_ESP_FX_MAX; i++) {
        const re15_esp_fx_t *f = re15_esp_fx_get(i);
        if (!f || f->effect_id != id) continue;
        if (f->rows_base && !(f->flags & 0x01)) continue;
        if (nur_sichtbar && !re15_esp_fx_visible(f)) continue;
        n++;
    }
    return n;
}

static void dump_id(int id, const char *tag)
{
    for (int i = 0; i < RE15_ESP_FX_MAX; i++) {
        const re15_esp_fx_t *f = re15_esp_fx_get(i);
        if (!f || f->effect_id != id) continue;
        printf("  %s slot=%d id=%u sub=%u A=%u B=%u fl=%02x anim=%d timer=%d row=%d/%d y=%d xlat_y=%d vel=(%d,%d,%d)"
               " wpos=(%d,%d,%d) f1e=%d\n",
               tag, i, f->effect_id, f->sub_index, u16le(f->row, 0), u16le(f->row, 2), f->flags, f->frame,
               f->timer, f->row_cursor, f->row_count, f->y, f->xlat_y, f->drift_x, f->drift_y, f->drift_z,
               f->wpos[0], f->wpos[1], f->wpos[2], (int16_t)u16le(f->row, 0x1e));
    }
}

/* MESSUNG: Redhawk-Schuss (re15_player_weapon_fire(7) = FUN_80011f50-Kern, Waffe 7) auf den Hund,
 * danach Bild fuer Bild die lebenden Raum-Id-7-Plaetze (die Fleisch-Brocken). */
static int teil_messung(int room, const char *stage, int bilder)
{
    if (room_load(room, stage) != 0) return 1;
    re15_actor_t *e = hund_arena(2000);
    if (!e) { printf("kein Hund 0x20 in ROOM%04X\n", room); return 1; }
    int16_t hp0 = e->hp;
    int r = re15_player_weapon_fire(7);
    printf("ROOM%04X Schuss Waffe 7: Treffer=%d hp %d -> %d st=%d +5=%d\n", room, r, hp0, e->hp, e->state, e->sub_state_1);
    int spitze = 0, letztes = -1;
    for (int f = 1; f <= bilder; f++) {
        frame();
        int n = lebend(7, 0);
        if (n > spitze) spitze = n;
        if (n) letztes = f;
        if (f <= 3 || f == 10 || f == 30 || f == 60 || f == 90 || f == 150 || f == 300 || f == bilder) {
            printf("Bild %4d: id7 lebend %d (sichtbar %d), id0 %d, Hund st=%d/%d/%d\n", f, n, lebend(7, 1),
                   lebend(0, 0), e->state, e->sub_state_1, e->sub_state_2);
            if (f == 30 || f == bilder) dump_id(7, "  ");
        }
    }
    printf("ERGEBNIS ROOM%04X: Spitze id7 %d, letztes Bild mit id7 %d von %d\n", room, spitze, letztes, bilder);
    return 0;
}

int main(int argc, char **argv)
{
    const char *teil = argc > 1 ? argv[1] : "alle";
    if (!strcmp(teil, "zensus")) return teil_zensus();
    if (!strcmp(teil, "messung")) return teil_messung(0x11D0, "STAGE1", argc > 2 ? atoi(argv[2]) : 900);
    printf("unbekannter Teil %s\n", teil);
    return 2;
}
