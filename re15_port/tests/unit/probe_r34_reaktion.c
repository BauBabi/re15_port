/*
 * probe_r34_reaktion.c — Runde 34 (Granaten), Spur B: Gegnerreaktion aller Typen auf die
 * Explosion (Resolver Art 2/3/4 mit Punkt P) und das Bodenfeuer (re15_re2_gl_apply), gefahren
 * ueber den ECHTEN Spielschritt (re15_game_step: KI, Filter, HP-Stempel, Anim-Vorschub).
 *
 * Dossier: analysis/befunde_runde34_granaten/bau_b.md. Rueckgabe 0 = gruen, sonst die Nummer der
 * ersten fehlgeschlagenen Pruefung.
 *
 * Aufruf: probe_r34_reaktion [teil]   teil = zombie | hund | spinne | re15 | g5 | treppe |
 *                                            zensus | alle (Vorgabe)
 */
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
#include "re15_inventory.h"
#include "re15_msg.h"
#include "re15_tim.h"
#include "re2_ems.h"
#include "re15_skeleton.h"   /* re15_sin_q12 / re15_cos_q12 */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef RE15_ASSET_PSX_DIR
#define RE15_ASSET_PSX_DIR "shared_assets/PSX"
#endif

extern void re15_player_aim_reset(void);
extern void re15_player_set_aim_clip_len(int fc);

static int s_first_fail = 0, s_fails = 0;
#define CHECK(nr, c, ...) do { if (!(c)) { printf("FAIL %d: ", (nr)); printf(__VA_ARGS__); printf("\n"); \
    s_fails++; if (!s_first_fail) s_first_fail = (nr); } else { printf("ok   %d: ", (nr)); printf(__VA_ARGS__); printf("\n"); } } while (0)

static re15_rdt_t         s_rdt;
static uint8_t           *s_rdt_buf = NULL;
static int                s_room_id = 0;
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

/* ---- RE2-Baenke (pc_enemy_load-Spiegel, RE2-Zweig), damit Clip-Laengen echt sind ---- */
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
    return 0;
}

static void bringup(re15_ai_flavor_t flavor)
{
    re15_ai_flavor_set(flavor);
    re15_game_state_init();                /* Spielflags frisch: Todes-Flags frueherer Teile */
    re15_actor_init(); re15_aot_init(); scd_vm_init();
    re15_enemy_reset(); re15_enemy_ai_set_paused(0);
    re15_player_cmd_reset(); re15_player_aim_reset();
    extern void re15_esp_fx_reset(void);
    re15_esp_fx_reset();
    re15_damage_seed_rng(0x0badf00du);
    g_current_room_id = (uint16_t)s_room_id;
    g_room_rdt_ok = 0; g_room_change.pending = 0;   /* derselbe Stand wie im ersten Teil eines frischen
                                                     * Prozesses — kein veralteter Raum (bringup_hunde
                                                     * setzt ROOM11D0 mit Zeigern in einen spaeter
                                                     * freigegebenen Puffer) */
    { extern void scd_register_current_rdt(const re15_rdt_t *rdt);
      scd_register_current_rdt(NULL); }             /* sonst saet der VM-Tick Slot 1 je Bild mit
                                                     * sub01 des ALTEN Raums neu (scd_vm.c:674) */
    if (s_rdt.main_scd)   scd_thread_start(0, s_rdt.main_scd);
    if (s_rdt.sub_scd[0]) scd_thread_start(1, s_rdt.sub_scd[0]);
    g_scd.work_vars[10] = 0;
    for (int i = 0; i < 120; i++) scd_vm_tick();
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100; pl->hit_react = 0;
    pl->state = 0; pl->motion = 0; pl->floor = 0; pl->y = 0;
    re15_collision_set_band(0);
    re15_player_set_aim_clip_len(12);
    if (flavor != RE15_AI_FLAVOR_RE15)
        for (int s = 1; s < RE15_ACTOR_MAX; s++)
            if (g_actors[s].active && re15_re2_owns_type(g_actors[s].type)) load_re2_bank(g_actors[s].type);
}

static void zensus_print(const char *tag)
{
    printf("[%s] Raum %04X:", tag, s_room_id);
    for (int s = 1; s < RE15_ACTOR_MAX; s++) {
        re15_actor_t *e = &g_actors[s];
        if (!e->active) continue;
        printf(" [%d t=%02x st=%d/%d/%d hp=%d grid=%02x @(%d,%d,%d) r%d]", s, e->type, e->state,
               e->sub_state_1, e->sub_state_2, e->hp, e->grid_id, e->x, e->y, e->z, e->rot_y);
    }
    printf("\n");
}

static void explosion_bei(const re15_actor_t *e, int32_t dx, uint8_t art)
{
    re15_attack_box_t b;
    b.x = e->x + dx; b.y = e->y - 500; b.z = e->z; b.radius = 500;   /* P = (x, y-500, z) @0x800185a0-ac */
    re15_resolve_attack(&b, art, -1);
}

/* =========================================================================================
 * TEIL "zombie" — B5: Brand-/Saeure-DoT und Leichen-Ausblender (RE2-KI).
 * ========================================================================================= */
static void teil_zombie(void)
{
    printf("== zombie (B5)\n");
    if (room_load(0x1140, "STAGE1") != 0) { CHECK(100, 0, "ROOM1140 fehlt"); return; }
    const uint8_t art[2] = { 4, 3 };                 /* 4 = Brand (RE2-Zeile 10), 3 = Saeure (11) */
    for (int lauf = 0; lauf < 2; lauf++) {
        bringup(RE15_AI_FLAVOR_RE2);
        for (int f = 0; f < 60; f++) frame();
        if (lauf == 0) zensus_print("1140 RE2");
        /* Brad 0x11 (HP 250 @0x801008C8) aus der Fress-Pose holen: stehend, gehend (EXEC[1]).
         * Harness-Hilfe (Zustand gesetzt, Treffer + KI laufen echt); Arena = nur Brad. */
        int slot = -1;
        for (int s = 1; s < RE15_ACTOR_MAX; s++)
            if (g_actors[s].active && g_actors[s].type == 0x11) { slot = s; break; }
        if (slot < 0) { CHECK(100, 0, "kein Brad 0x11 in ROOM1140"); return; }
        for (int s = 1; s < RE15_ACTOR_MAX; s++) if (s != slot) g_actors[s].active = 0;
        re15_actor_t *e = &g_actors[slot];
        re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
        e->grid_id = 0; e->re2z_f10e = 0; e->re2z_flags21a &= (uint16_t)~0x12u; e->re2z_self1d3 = 0;
        e->hit_react = 0;
        re15_ai_set_state_word(e, 0x101);
        pl->x = e->x - 6000; pl->z = e->z; pl->y = e->y;
        for (int f = 0; f < 10; f++) frame();
        int16_t hp0 = e->hp;
        explosion_bei(e, 300, art[lauf]);
        CHECK(101 + 10 * lauf, e->hp == hp0 - 200 && e->state == 2 &&
                               e->sub_state_1 == (lauf == 0 ? 10 : 11) && e->re2z_hits1d2 == 0,
              "Brad %s: hp %d -> %d (-200), st=%d, +5=%d, 1D2=%d", lauf == 0 ? "Brand" : "Saeure",
              hp0, e->hp, e->state, e->sub_state_1, e->re2z_hits1d2);
        /* Verkohlung (+0x10E |= 0x80, +0x21A |= 0x800 — FUN_80106128 @0x8010613C-48 + @0x80105df0-fc)
         * bzw. Aetzung (+0x21A |= 0x1800 @0x8010632C-38) aus Stagger-P0 (@0x80105DC4-F18). */
        int el = 0;
        for (int f = 0; f < 5 && !el; f++) {
            frame();
            el = (lauf == 0) ? ((e->re2z_f10e & 0x80u) && (e->re2z_flags21a & 0x800u))
                             : ((e->re2z_flags21a & 0x1800u) == 0x1800u);
        }
        CHECK(102 + 10 * lauf, el, "Element-Bits nach dem Treffer: f10e=0x%04X 21a=0x%04X",
              e->re2z_f10e, e->re2z_flags21a);
        /* DoT: HP -1 genau dann, wenn beim Gang-Executor (+0x236 & 7) == 0 war; der Zaehler
         * steigt NACH dem Dispatch (@0x801004F8-508) -> nach dem Bild gilt (+0x236 & 7) == 1. */
        int drops = 0, bad = 0, walk_frames = 0, last_drop = -1, gap_bad = 0, died = -1;
        uint8_t tod_zeile = 0, tod_1d2 = 0, tod_1d3 = 0; uint16_t tod_21a = 0;
        for (int f = 0; f < 1600 && died < 0; f++) {
            int16_t hp_vor = e->hp; uint8_t st_vor = e->state; uint8_t sub_vor = e->sub_state_1;
            frame();
            int walk = (st_vor == 1 && (sub_vor == 1 || sub_vor == 2));
            if (walk) walk_frames++;
            else last_drop = -1;                 /* Abstand nur ueber durchgehendes Gehen messen */
            if (e->hp == hp_vor - 1 && walk) {
                drops++;
                if ((e->re2z_c236 & 7u) != 1u) bad++;
                if (last_drop >= 0 && f - last_drop != 8) gap_bad++;
                last_drop = f;
            }
            if (e->state == 3 && st_vor != 3) {
                died = f; tod_zeile = e->sub_state_1; tod_1d2 = e->re2z_hits1d2; tod_1d3 = e->re2z_self1d3;
                tod_21a = e->re2z_flags21a;
            }
        }
        CHECK(103 + 10 * lauf, drops >= 40 && bad == 0 && gap_bad == 0,
              "DoT: %d HP-Stufen in %d Gang-Bildern, Takt-Fehler %d, Abstand != 8: %d", drops,
              walk_frames, bad, gap_bad);
        CHECK(104 + 10 * lauf, died >= 0 && tod_zeile == (lauf == 0 ? 0x0A : 0x0B) && tod_1d2 == 4 &&
                               (tod_1d3 & 0x80u) && (tod_21a & 0x2000u),
              "Tod am Element: Bild %d, +5=0x%02X, 1D2=%d, 1D3=0x%02X, 21a=0x%04X", died, tod_zeile,
              tod_1d2, tod_1d3, tod_21a);
        /* Leichen-Ausblender (@0x8010a810-868, nur verkohlt): Part 0 R-Byte sinkt alle 4 Bilder
         * um 1, Stopp bei 16. */
        if (lauf == 0) {
            int corpse = -1;
            for (int f = 0; f < 600 && corpse < 0; f++) { frame(); if (e->state == 7) corpse = f; }
            uint32_t r0 = e->re2z_part_tint[0] & 0xffu;
            int stufen = 0, abstand_ok = 1, last = -1;
            for (int f = 0; f < 400; f++) {
                uint32_t vor = e->re2z_part_tint[0] & 0xffu;
                frame();
                uint32_t nach = e->re2z_part_tint[0] & 0xffu;
                if (nach == vor - 1u) { if (last >= 0 && f - last != 4) abstand_ok = 0; last = f; stufen++; }
            }
            uint32_t r1 = e->re2z_part_tint[0] & 0xffu;
            CHECK(105, corpse >= 0 && stufen > 0 && abstand_ok && (r1 == 16u || r1 < r0),
                  "Leichen-Ausblender: Leiche ab Bild %d, R %u -> %u in %d Stufen (je 4 Bilder: %d)",
                  corpse, r0, r1, stufen, abstand_ok);
            CHECK(106, r1 == 16u, "Ausblender stoppt bei R = 16 (@0x8010a830-34): R = %u", r1);
        }
    }
    /* (107) NEGATIV-KONTROLLE: ein gehender Brad OHNE Element-Bits verliert in 400 Bildern keine HP. */
    {
        bringup(RE15_AI_FLAVOR_RE2);
        for (int f = 0; f < 60; f++) frame();
        int slot = -1;
        for (int s = 1; s < RE15_ACTOR_MAX; s++)
            if (g_actors[s].active && g_actors[s].type == 0x11) { slot = s; break; }
        for (int s = 1; s < RE15_ACTOR_MAX; s++) if (s != slot) g_actors[s].active = 0;
        re15_actor_t *e = &g_actors[slot];
        e->grid_id = 0; e->re2z_f10e = 0; e->re2z_flags21a &= (uint16_t)~0x12u; e->re2z_self1d3 = 0;
        re15_ai_set_state_word(e, 0x101);
        g_actors[RE15_ACTOR_SLOT_PLAYER].x = e->x - 6000;
        int16_t hp0 = e->hp;
        for (int f = 0; f < 400; f++) frame();
        CHECK(107, e->hp == hp0, "ohne Element: hp %d -> %d", hp0, e->hp);
    }
}

/* =========================================================================================
 * TEIL "hund" — B6: RE2-Hund (EMD0G_MOD0.BIN), Tod Zeile 9/10/11 und HURT 9/10/11.
 * ROOM11D0 mit Flag 3:152 = 1 (freier Hunde-Satz, Hochfahren wie probe_r30_hund_tod.c:106-133).
 * ========================================================================================= */
static const int16_t k_box_op40[4] = { -600, 0, 300, 150 };   /* @0x80010910 (RE2-PSX.EXE) */
static int s_se7 = 0;
static void hund_se(int id, int flag2000) { (void)flag2000; if (id == 7) s_se7++; }

static void bringup_hunde(void)
{
    re15_ai_flavor_set(RE15_AI_FLAVOR_RE2);
    re15_game_state_init();
    re15_actor_init(); re15_aot_init(); scd_vm_init();
    re15_enemy_reset(); re15_enemy_ai_set_paused(0);
    load_re2_bank(0x20);                   /* NACH dem Reset, VOR dem Spawn (probe_r30_hund_tod.c:108-113) */
    re15_player_cmd_reset(); re15_player_aim_reset();
    extern void re15_esp_fx_reset(void);
    re15_esp_fx_reset();
    re15_damage_seed_rng(0x0badf00du);
    { extern void re15_re2z_rng_reset(void); re15_re2z_rng_reset(); }
    g_room_rdt = s_rdt; g_room_rdt_ok = 1;
    g_current_room_id = (uint16_t)s_room_id; g_room_change.pending = 0;
    re15_msg_load_room_block(s_rdt.messages, s_rdt.messages_size);
    scd_register_room_events(&s_rdt);
    re15_game_flag_set(3, 152, 1);          /* freier Hunde-Satz (Else-Zweig @Datei 0x13EE) */
    if (s_rdt.main_scd)   scd_thread_start(0, s_rdt.main_scd);
    if (s_rdt.sub_scd[0]) scd_thread_start(1, s_rdt.sub_scd[0]);
    for (int i = 0; i < 120; i++) scd_vm_tick();
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100; pl->hit_react = 0;
    pl->state = 0; pl->motion = 0; pl->floor = 0; pl->y = 0;
    re15_collision_set_band(0);
    re15_player_set_aim_clip_len(12);
    re15_re2dog_audio_hook(hund_se, NULL);
}

/* Arena: der erste Hund allein, stehend (Zustand 1), Spieler 8000 weit weg; hp > 0 setzt HP. */
static re15_actor_t *hund_arena(int16_t hp)
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
    pl->x = e->x - 8000; pl->z = e->z; pl->y = e->y;
    for (int f = 0; f < 3; f++) frame();
    e->re2z_self1d3 = 0; e->hit_react = 0;
    if (hp > 0) e->hp = hp;
    return e;
}

static int hund_gl(re15_actor_t *e, unsigned zeile, unsigned k)
{
    const int32_t P[3] = { e->x, e->y - 100, e->z };      /* Flammenpunkt y - 100 (@0x800207a4) */
    return re15_re2_gl_apply(P, 0, k_box_op40, (k << 28) | 0x20000u | zeile);
}

static const uint8_t k_teile[7] = { 2, 3, 4, 7, 8, 9, 10 };   /* @0x80105680 */
static int in_teile(int p) { for (int i = 0; i < 7; i++) if (k_teile[i] == p) return 1; return 0; }
static unsigned fxz(int fx) { return re15_re2dog_fx_zaehler(fx); }
static unsigned fx_summe(void) { unsigned s = 0; for (int i = 0; i < 16; i++) s += fxz(i); return s; }
static int tints_gleich(const re15_actor_t *e, int von, int bis, uint32_t w)
{
    for (int p = von; p < bis; p++) if (e->re2z_part_tint[p] != w) return 0;
    return 1;
}

static void teil_hund(void)
{
    printf("== hund (B6)\n");
    if (room_load(0x11D0, "STAGE1") != 0) { CHECK(120, 0, "ROOM11D0 fehlt"); return; }

    /* ---- Tod Zeile 9 (Granate, Art 2 -> +0x5 = 9): Router 0x80104610 / P0 0x80104694 ---- */
    {
        re15_actor_t *e = hund_arena(0);
        if (!e) { CHECK(120, 0, "kein Hund 0x20 in ROOM11D0"); return; }
        int16_t hp0 = e->hp;
        explosion_bei(e, 300, 2);
        CHECK(120, e->state == 3 && e->sub_state_1 == 9 && e->re2z_hits1d2 < 3 && e->re2z_self1d3 == 15,
              "HE: hp %d -> %d, st=%d, +5=%d (9), 1D2=%d (<3), 1D3=%d (15)", hp0, e->hp, e->state,
              e->sub_state_1, e->re2z_hits1d2, e->re2z_self1d3);
        re15_re2dog_fx_zaehler_reset(); s_se7 = 0;
        const int16_t gier = e->rot_y;
        frame();                                           /* DEATH-P0 */
        int fl_ok = 1, feld_ok = 1;
        for (int p = 0; p < 17; p++) {
            const int soll = in_teile(p);
            if (((e->re2z_part_flags[p] & 0x4Au) == 0x4Au) != soll) fl_ok = 0;
            if (soll && !(e->re2z_part_tint[p] == 0x00101040u && e->re2z_part_w9c[p] == 800 &&
                          e->re2z_part_w9a[p] == -150 && e->re2z_part_w9e[p] == 10 &&
                          e->re2z_part_wa4[p] == -100 && e->re2z_part_life[p] == 0 &&
                          e->re2z_part_yaw98[p] == gier)) feld_ok = 0;
        }
        CHECK(121, fl_ok, "Zeile 9: Flags |= 0x4A genau an Parts {2,3,4,7,8,9,10} (@0x80105680, @0x801044a4-a8)");
        CHECK(122, feld_ok, "Teile-Felder +0x9C 800/+0x9A -150/+0x9E 10/+0xA4 -100/+0xA0 0, Farbe 0x00101040, "
                            "+0x98 = Gier (@0x801044b4-cc)");
        const unsigned b0 = e->re2d_budget21f;
        CHECK(123, b0 == 15 && fxz(0) == 3 && fxz(1) + fxz(2) == 1 && fxz(7) <= 1 && s_se7 == 0,
              "P0-Bild: +0x21F = %u (18 - 3 Router-Effekte = 15), FX0 %u (Kern 1 + Router 2), FX1/2 %u (1), "
              "FX7 %u (<=1), SE7 %d (0, stumm @0x801046E8)", b0, fxz(0), fxz(1) + fxz(2), fxz(7), s_se7);
        int stufen_ok = 1; unsigned b = b0;
        for (int f = 0; f < 5; f++) {
            frame();
            if ((unsigned)e->re2d_budget21f + 3u != b) stufen_ok = 0;
            b = e->re2d_budget21f;
        }
        frame(); frame();
        CHECK(124, stufen_ok && e->re2d_budget21f == 0 && fxz(0) == 13 && fxz(1) + fxz(2) == 6,
              "Blut je Bild (@0x80104644-7C): Budget 15 -> 0 in 3er-Stufen %d, FX0 %u (13), FX1/2 %u (6)",
              stufen_ok, fxz(0), fxz(1) + fxz(2));
    }

    /* ---- Tod Zeile 10 (Brand, Art 4 -> Waffe 11 -> Zeile 10): 0x80104774 ---- */
    {
        re15_actor_t *e = hund_arena(0);
        explosion_bei(e, 300, 4);
        CHECK(130, e->state == 3 && e->sub_state_1 == 11 && e->re2z_hits1d2 < 3,
              "Brand: st=%d, +5=%d (11 -> Zeile 10), 1D2=%d", e->state, e->sub_state_1, e->re2z_hits1d2);
        re15_re2dog_fx_zaehler_reset(); s_se7 = 0;
        frame();
        CHECK(131, tints_gleich(e, 0, 17, 0x00202020u) && tints_gleich(e, 17, 20, 0u),
              "Zeile 10: Parts 0..16 +0x70 = 0x00202020 (@0x801047d8-800), 17..19 unberuehrt "
              "(p0 0x%08X p16 0x%08X p17 0x%08X)", e->re2z_part_tint[0], e->re2z_part_tint[16],
              e->re2z_part_tint[17]);
        CHECK(132, fxz(7) == 6 && fxz(0) == 1 && e->re2d_budget21f == 0 && s_se7 == 1,
              "Zeile 10: FX7 %u (6, @0x801047b4-d4), FX0 %u (Kern 1), Budget %u (0), SE7 %d (1, Kern)",
              fxz(7), fxz(0), e->re2d_budget21f, s_se7);
        const unsigned s0 = fx_summe();
        for (int f = 0; f < 10; f++) frame();
        CHECK(133, fx_summe() == s0, "Zeile 10 hat kein Blut je Bild (Router 0x80104118): %u -> %u", s0, fx_summe());
    }

    /* ---- Tod Zeile 11 (Saeure, Art 3 -> Waffe 10 -> Zeile 11): 0x8010481C ---- */
    {
        re15_actor_t *e = hund_arena(0);
        explosion_bei(e, 300, 3);
        CHECK(140, e->state == 3 && e->sub_state_1 == 10 && e->re2z_hits1d2 < 3,
              "Saeure: st=%d, +5=%d (10 -> Zeile 11), 1D2=%d", e->state, e->sub_state_1, e->re2z_hits1d2);
        re15_re2dog_fx_zaehler_reset(); s_se7 = 0;
        frame();
        CHECK(141, tints_gleich(e, 0, 17, 0x00003F2Fu) && tints_gleich(e, 17, 20, 0u),
              "Zeile 11: Parts 0..16 +0x70 = 0x00003F2F (@0x80104844-64) (p0 0x%08X p16 0x%08X p17 0x%08X)",
              e->re2z_part_tint[0], e->re2z_part_tint[16], e->re2z_part_tint[17]);
        CHECK(142, fxz(9) == 1 && fxz(10) == 1 && fxz(0) == 1 && e->re2d_budget21f == 0 && s_se7 == 1,
              "Zeile 11: FX9 %u (1), FX10 %u (1) (@0x8010486c-9c), FX0 %u (Kern 1), Budget %u, SE7 %d",
              fxz(9), fxz(10), fxz(0), e->re2d_budget21f, s_se7);
    }

    /* ---- Klammer-Tor (+0x1D2 >= 3): nur der Kern (Negativ-Kontrollen) ---- */
    {
        re15_actor_t *e = hund_arena(30);                  /* 30 < 50 (Klammer 1, @0x800A44D8) */
        int r = hund_gl(e, 10, 1);
        CHECK(150, r != 0 && e->state == 3 && e->sub_state_1 == 11 && e->re2z_hits1d2 == 3,
              "GL Zeile 10 Kl. 1: r=%d st=%d +5=%d 1D2=%d (3)", r, e->state, e->sub_state_1, e->re2z_hits1d2);
        re15_re2dog_fx_zaehler_reset(); s_se7 = 0;
        frame();
        CHECK(151, tints_gleich(e, 0, 17, 0u) && fxz(7) == 0 && s_se7 == 1,
              "Zeile 10 bei 1D2 >= 3 (`sltiu v0,v0,0x3` @0x80104794): keine Farbe, FX7 %u (0), SE7 %d (1)",
              fxz(7), s_se7);
        e = hund_arena(30);
        r = hund_gl(e, 9, 1);
        re15_re2dog_fx_zaehler_reset(); s_se7 = 0;
        frame();
        int keine = 1;
        for (int p = 0; p < 17; p++) if (e->re2z_part_flags[p] & 0x4Au) keine = 0;
        for (int f = 0; f < 6; f++) frame();
        CHECK(152, r != 0 && e->sub_state_1 == 9 && keine && s_se7 == 1 && fxz(0) == 1 && fxz(1) + fxz(2) == 0,
              "Zeile 9 bei 1D2 >= 3 (@0x801046b8-cc): nur der Kern, Schrei SE7 %d (1), kein Teile-Wurf %d, "
              "kein Blut (FX0 %u = Kern, FX1/2 %u)", s_se7, keine, fxz(0), fxz(1) + fxz(2));
        /* (153) Zeile 16 (Waffe 14) ignoriert die Klammer (`bne v1,v0(=16)` @0x801047a0-a8):
         *       Treffer-Stempel wie der Applier (Wort-`sw` 3 + `sb` Zeile), 1D2 = 3. */
        e = hund_arena(30);
        e->hp = -1; e->state = 3; e->sub_state_1 = 14; e->sub_state_2 = 0; e->sub_state_3 = 0;
        e->re2z_hits1d2 = 3;
        re15_re2dog_fx_zaehler_reset(); s_se7 = 0;
        frame();
        CHECK(153, tints_gleich(e, 0, 17, 0x00202020u) && fxz(7) == 6,
              "Zeile 16 bei 1D2 = 3: Farbe %d, FX7 %u (6)", tints_gleich(e, 0, 17, 0x00202020u), fxz(7));
    }

    /* ---- HURT: FX-8-Schleife ceil(n/2) (@0x80103d30-5c / @0x80103de8-e14), +0x5 := 1 ---- */
    {
        static const struct { unsigned zeile, k, n, fx8; } hn[4] = {
            { 9, 1, 1, 1 }, { 9, 0, 2, 1 }, { 10, 1, 3, 2 }, { 10, 0, 4, 2 } };
        for (int i = 0; i < 4; i++) {
            re15_actor_t *e = hund_arena(1000);
            int r = hund_gl(e, hn[i].zeile, hn[i].k);
            re15_re2dog_fx_zaehler_reset();
            frame();
            const int sperre = (e->re2z_self1d3 & 0x80u) != 0;
            CHECK(160 + i, r != 0 && e->state == 2 && fxz(8) == hn[i].fx8 && fxz(0) == 1 && e->sub_state_1 == 1 &&
                           sperre == (hn[i].zeile == 10u),
                  "HURT Zeile %u Kl. %u (n = %u): FX8 %u (%u), FX0 %u (P0 1), +5 = %d (1), 1D3 Bit 0x80 %d",
                  hn[i].zeile, hn[i].k, hn[i].n, fxz(8), hn[i].fx8, fxz(0), e->sub_state_1, sperre);
        }
        /* (164) HURT 11 (0x80103E60): EIN Part rand&0xF := 0x00003F2F, FX9, +0x5 := 1. */
        re15_actor_t *e = hund_arena(1000);
        int r = hund_gl(e, 11, 0);
        re15_re2dog_fx_zaehler_reset();
        frame();
        int n3f = 0;
        for (int p = 0; p < 20; p++) if (e->re2z_part_tint[p] == 0x00003F2Fu) n3f++;
        CHECK(164, r != 0 && e->state == 2 && n3f == 1 && fxz(9) == 1 && e->sub_state_1 == 1,
              "HURT 11: %d Part(s) 0x00003F2F (1, @0x80103ebc-c0), FX9 %u (1), +5 = %d (1)", n3f, fxz(9),
              e->sub_state_1);
    }

    /* ---- Sperre: nach HURT 10 kein Applier-Treffer, solange +0x1D3 & 0x80 steht ---- */
    {
        int bild[2] = { -1, -1 }, bit_bilder[2] = { 0, 0 }, frei_ok[2] = { 1, 1 };
        for (int lauf = 0; lauf < 2; lauf++) {
            re15_actor_t *e = hund_arena(1000);
            hund_gl(e, lauf == 0 ? 9u : 10u, 0);
            for (int f = 0; f < 600 && bild[lauf] < 0; f++) {
                frame();
                const uint8_t sperre = e->re2z_self1d3;
                const int16_t hp_vor = e->hp;
                if (hund_gl(e, 10, 2)) {                   /* Klammer 2 = 5 Schaden (@0x800A44D8) */
                    bild[lauf] = f;
                    if (sperre != 0u || e->hp != hp_vor - 5) frei_ok[lauf] = 0;
                } else if (sperre == 0u) frei_ok[lauf] = 0;   /* frei, aber nicht getroffen */
                if (sperre & 0x80u) bit_bilder[lauf]++;
            }
        }
        CHECK(165, bild[0] == 14 && frei_ok[0] && bit_bilder[0] == 0,
              "nach HURT 9 (ohne Bit 0x80): wieder treffbar in Bild %d (14 = Sperre 15 abgelaufen), sauber %d",
              bild[0], frei_ok[0]);
        CHECK(166, bild[1] > bild[0] && frei_ok[1] && bit_bilder[1] >= bild[1],
              "nach HURT 10 (Bit 0x80 @0x80103e34-3c): erst in Bild %d wieder treffbar, Bit stand %d Bilder, "
              "sauber %d", bild[1], bit_bilder[1], frei_ok[1]);
    }
}

/* =========================================================================================
 * TEIL "spinne" — B7: RE2-Spinne 0x25 (EMS25.BIN), Tod Zeile 10 (Brand) / 11 (Saeure).
 * Arena: ROOM1140-Kontext, alle Raum-Gegner aus, EINE Spinne im Slot 1 (Bank EM025 vor dem INIT,
 * Aufbau wie test_adult_spider_ai.c:34-37), Spieler 8000 weit weg.
 * ========================================================================================= */
static re15_actor_t *spinne_arena(void)
{
    if (room_load(0x1140, "STAGE1") != 0) return NULL;
    bringup(RE15_AI_FLAVOR_RE2);
    for (int s = 1; s < RE15_ACTOR_MAX; s++) g_actors[s].active = 0;
    load_re2_bank(0x25);
    re15_actor_t *e = &g_actors[1];
    memset(e, 0, sizeof *e);
    e->active = 1; e->type = 0x25; e->state = 0;
    e->x = 0; e->y = 0; e->z = 0; e->rot_y = 0;
    re15_enemy_apply_hitbox(e, 0x25);
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->x = e->x - 8000; pl->z = e->z; pl->y = 0;
    for (int f = 0; f < 5; f++) frame();
    e->re2z_self1d3 = 0; e->hit_react = 0; e->re2z_f10e = 0;
    return e;
}

static void teil_spinne(void)
{
    printf("== spinne (B7)\n");
    /* (170) Brand (Art 4 -> Waffe 11 -> Zeile 10): 20 Parts 0x00202F2F (FUN_8010609C @0x80104B44-4C). */
    {
        re15_actor_t *e = spinne_arena();
        if (!e) { CHECK(170, 0, "ROOM1140 fehlt"); return; }
        e->hp = 50;                                          /* < 130 (Zeile 10 Kl. 0 @0x800A4C44) */
        explosion_bei(e, 300, 4);
        const int st = e->state, s1 = e->sub_state_1;
        frame();
        CHECK(170, st == 3 && s1 == 11 && tints_gleich(e, 0, 20, 0x00202F2Fu),
              "Brand-Tod: st=%d +5=%d (11 -> Zeile 10), Parts 0..19 0x00202F2F %d (p0 0x%08X p19 0x%08X)",
              st, s1, tints_gleich(e, 0, 20, 0x00202F2Fu), e->re2z_part_tint[0], e->re2z_part_tint[19]);
        CHECK(171, !(e->re2z_part_flags[19] & 0x10u) && e->re2s_dead239 == 0,
              "Brand: Part 19 fliegt NICHT (Flags 0x%04X), +0x239 %d (0)", e->re2z_part_flags[19], e->re2s_dead239);
    }
    /* (172) Saeure (Art 3 -> Waffe 10 -> Zeile 11): 20 Parts 0x00101F3F + Part 19 fliegt + +0x239 = 1. */
    {
        re15_actor_t *e = spinne_arena();
        e->hp = 50;                                          /* < 90 (Zeile 11 Kl. 0 @0x800A4C58) */
        explosion_bei(e, 300, 3);
        const int st = e->state, s1 = e->sub_state_1;
        frame();
        CHECK(172, st == 3 && s1 == 10 && tints_gleich(e, 0, 20, 0x00101F3Fu),
              "Saeure-Tod: st=%d +5=%d (10 -> Zeile 11), Parts 0..19 0x00101F3F %d (p0 0x%08X p19 0x%08X)",
              st, s1, tints_gleich(e, 0, 20, 0x00101F3Fu), e->re2z_part_tint[0], e->re2z_part_tint[19]);
        CHECK(173, (e->re2z_part_flags[19] & 0x10u) && e->re2z_part_w9e[19] == 90 && e->re2z_part_yaw98[19] == 0 &&
                   e->re2z_part_w9a[19] == 0 && (uint16_t)e->re2z_part_w9c[19] == 0x6464u && e->re2s_dead239 == 1,
              "Part 19 (@0x80104bc8-f4): Flags 0x%04X (|0x10), +0x9E %d (90), +0x98 %d, +0x9A %d, +0x9C 0x%04X "
              "(0x6464 = Bytes 100/100), +0x239 %d (1)", e->re2z_part_flags[19], e->re2z_part_w9e[19],
              e->re2z_part_yaw98[19], e->re2z_part_w9a[19], (uint16_t)e->re2z_part_w9c[19], e->re2s_dead239);
    }
    /* (174) NEGATIV: HE (Art 2 -> Zeile 9) faerbt nicht (@0x8010493C ruft FUN_8010609C nicht). */
    {
        re15_actor_t *e = spinne_arena();
        e->hp = 30;                                          /* < 60 (Zeile 9 Kl. 0 @0x800A4C30) */
        explosion_bei(e, 300, 2);
        const int st = e->state, s1 = e->sub_state_1;
        frame();
        CHECK(174, st == 3 && s1 == 9 && tints_gleich(e, 0, 20, 0u),
              "HE-Tod: st=%d +5=%d, keine Part-Farbe %d", st, s1, tints_gleich(e, 0, 20, 0u));
    }
}

/* =========================================================================================
 * TEIL "re15" — B8: RE1.5-KI-Typen 0x29 / 0x2b / 0x23 / 0x27 und der liegende Fresser 0x16.
 * Arena ohne Raum (wie test_adult_spider_ai.c): Flavor RE15, EIN Gegner im Slot 1, Spieler weit
 * weg, KI ueber re15_enemy_ai_run_all (der echte Dispatch), Treffer ueber re15_resolve_attack.
 * ========================================================================================= */
extern int g_test_room_se_log[2048];
extern int g_test_room_se_n;
static int se_zahl(int id) { int n = 0; for (int i = 0; i < g_test_room_se_n; i++) if (g_test_room_se_log[i] == id) n++; return n; }

static re15_actor_t *re15_arena(uint8_t type, uint8_t grid)
{
    re15_ai_flavor_set(RE15_AI_FLAVOR_RE15);
    re15_actor_init(); re15_enemy_reset(); re15_enemy_ai_set_paused(0);
    re15_damage_seed_rng(0x0badf00du);
    g_current_room_id = 0x3000;                          /* kein Sonderraum (2090/5090) */
    g_room_rdt_ok = 0;                                   /* keine Raumgeometrie (Arena) */
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    memset(pl, 0, sizeof *pl);
    pl->active = 1; pl->type = 0; pl->hp = 100; pl->x = 0; pl->z = 30000;
    re15_actor_t *e = &g_actors[1];
    memset(e, 0, sizeof *e);
    e->active = 1; e->type = type; e->state = 0; e->grid_id = grid;
    e->x = 0; e->y = 0; e->z = 0; e->rot_y = 0;
    re15_enemy_apply_hitbox(e, type);
    re15_enemy_ai_run_all(0);                            /* INIT */
    e->hit_react = 0;
    g_test_room_se_n = 0;
    return e;
}

/* Explosion vor (dx > 0 in Blickrichtung) oder hinter der Figur; Gier 0 = Blick nach +x. */
static void explosion_rel(re15_actor_t *e, int32_t dx, uint8_t art)
{
    re15_attack_box_t b;
    const int32_t c = re15_cos_q12(e->rot_y), s = re15_sin_q12(e->rot_y);
    b.x = e->x + (int32_t)((c * dx) >> 12); b.z = e->z - (int32_t)((s * dx) >> 12);
    b.y = e->y - 500; b.radius = 500;
    re15_resolve_attack(&b, art, -1);
}

static void teil_re15(void)
{
    printf("== re15 (B8)\n");
    /* ---- 0x29 Kakerlake: Explosions-Tod 0x801154b4 (vorn / hinten) ---- */
    for (int hinten = 0; hinten < 2; hinten++) {
        re15_actor_t *e = re15_arena(0x29, 0);
        e->sub_state_1 = 0; e->sub_state_2 = 0; e->sub_state_3 = 0;   /* ruhend, kein Flug */
        const int32_t x0 = e->x, z0 = e->z;
        explosion_rel(e, hinten ? -300 : 300, 2);
        const int st = e->state, z5 = e->sub_state_1, hr = e->hit_react;
        re15_enemy_ai_run_all(0);                        /* DEATH-P0 + Phase 1 im selben Bild */
        const int clip = e->motion, sp = e->crow_speed;
        const int32_t dx1 = e->x - x0, dz1 = e->z - z0;
        int tod7 = -1, halt_ok = 1; int32_t fr_letzt = -1;
        for (int f = 0; f < 400 && tod7 < 0; f++) { re15_enemy_ai_run_all(0); if (e->state == 7) tod7 = f; }
        fr_letzt = e->anim_frame;
        for (int f = 0; f < 30; f++) { re15_enemy_ai_run_all(0); if (e->anim_frame != fr_letzt) halt_ok = 0; }
        const int soll_clip = (hr & 0x80) ? 11 : 10;
        CHECK(180 + hinten * 2, st == 3 && z5 == 9 && clip == soll_clip && (e->hit_react & 2) && sp == 80 &&
                                se_zahl(7) == 1 && tod7 >= 0,
              "0x29 %s: st=%d +5=%d hr=0x%02X Clip %d (%d), +0x8c %d (80), SE7 %d (1), Leiche nach %d Bildern",
              hinten ? "hinten" : "vorn", st, z5, hr, clip, soll_clip, sp, se_zahl(7), tod7);
        const int64_t weg = (int64_t)dx1 * dx1 + (int64_t)dz1 * dz1;
        CHECK(181 + hinten * 2, halt_ok && ((hr & 0x80) ? (weg <= 4) : (weg >= 70 * 70)),
              "0x29 %s: Rueckstoss im ersten Bild (%d,%d) (%s), letztes Bild in der Leiche gehalten %d",
              hinten ? "hinten" : "vorn", dx1, dz1, (hr & 0x80) ? "netto ~0 bei 0x80" : ">= 70 nach hinten", halt_ok);
    }
    /* ---- 0x29: Flaechenfeuer (Art 5, +0x5 = 14) -> Zucken-Spur Clip 7, NICHT Explosion ---- */
    {
        re15_actor_t *e = re15_arena(0x29, 0);
        e->sub_state_1 = 0; e->sub_state_2 = 0; e->sub_state_3 = 0;
        const int16_t hp0 = e->hp;
        explosion_rel(e, 300, 5);
        const int st = e->state, z5 = e->sub_state_1;
        re15_enemy_ai_run_all(0);
        CHECK(184, st == 2 && z5 == 14 && e->hp == hp0 - 50 && e->motion == 7 && (e->hit_react & 2),
              "0x29 Art 5: st=%d +5=%d hp %d -> %d (-50 @0x8006f422), Clip %d (7, Spur @0x8011ed84[14])",
              st, z5, hp0, e->hp, e->motion);
    }
    /* ---- 0x2b Tyrant: Phase 2 Clip 0xa / 0xb nur bei +0x93 & 0x80 ---- */
    for (int hinten = 0; hinten < 2; hinten++) {
        re15_actor_t *e = re15_arena(0x2b, 0);
        explosion_rel(e, hinten ? -300 : 300, 2);
        const int hr = e->hit_react;
        re15_enemy_ai_run_all(0);
        const int clip0 = e->motion, hr2 = (e->hit_react & 2) != 0;
        int se7_bild = -1, clip2 = -1, tod7 = -1;
        for (int f = 0; f < 600 && tod7 < 0; f++) {
            const int n7 = se_zahl(7);
            re15_enemy_ai_run_all(0);
            if (se7_bild < 0 && se_zahl(7) > n7) se7_bild = e->anim_frame;
            if (clip2 < 0 && e->sub_state_3 >= 3) clip2 = e->motion;
            if (e->state == 7) tod7 = f;
        }
        const int soll0 = (hr & 0x80) ? 9 : 8, soll2 = (hr & 0x80) ? 0x0b : 0x0a;
        CHECK(185 + hinten, clip0 == soll0 && hr2 && clip2 == soll2 && tod7 >= 0,
              "0x2b %s: hr=0x%02X, Ph.0 Clip %d (%d) + Bit 2 %d, Ph.2 Clip 0x%x (0x%x, @0x80114f34), Leiche %d",
              hinten ? "hinten" : "vorn", hr, clip0, soll0, hr2, clip2, soll2, tod7);
    }
    /* ---- 0x23 Alligator (Land, grid ungerade): Clip 13, Leiche nach Bild 20 ---- */
    {
        re15_actor_t *e = re15_arena(0x23, 1);
        explosion_rel(e, 300, 2);
        const int st = e->state;
        int clip = -1, bild_vor_leiche = -1;
        for (int f = 0; f < 200 && e->state != 7; f++) {
            const int32_t fr = e->anim_frame;
            re15_enemy_ai_run_all(0);
            if (clip < 0) clip = e->motion;
            if (e->state == 7) bild_vor_leiche = fr;
        }
        CHECK(187, st == 3 && clip == 13 && e->state == 7 && bild_vor_leiche == 20 && (e->hit_react & 2),
              "0x23: st=%d Clip %d (13 @0x8010eaa4), Leiche nach Bild %d (20 @0x8010eba8), Bit 2 %d",
              st, clip, bild_vor_leiche, (e->hit_react & 2) != 0);
    }
    /* ---- 0x27 Made/Gorilla: Explosion (Zeile 9) unveraendert Crash-Tod, Zeile 14 jetzt Boden-Tod ---- */
    {
        re15_actor_t *e = re15_arena(0x27, 0);
        explosion_rel(e, 300, 2);
        re15_enemy_ai_run_all(0);
        const int clip9 = e->motion, z9 = e->sub_state_1;
        e = re15_arena(0x27, 0);
        e->hp = 40;                                      /* < 50: Art 5 toetet */
        explosion_rel(e, 300, 5);
        const int st14 = e->state, z14 = e->sub_state_1;
        re15_enemy_ai_run_all(0);
        CHECK(188, z9 == 9 && (clip9 == 0x0a || clip9 == 0x0b) && st14 == 3 && z14 == 14 && e->motion == 0x0e,
              "0x27: Zeile 9 Clip 0x%x (0xa/0xb Crash @0x8011bc10), Art 5 st=%d +5=%d Clip 0x%x (0xe Boden-Tod, "
              "Spur @0x80121500[14])", clip9, st14, z14, e->motion);
    }
    /* ---- 0x16 liegender Fresser (RE1.5-KI, ROOM1140): +0x93 = 1 in Ruhe -> nur |= 2, kein Schaden ---- */
    {
        if (room_load(0x1140, "STAGE1") != 0) { CHECK(189, 0, "ROOM1140 fehlt"); return; }
        bringup(RE15_AI_FLAVOR_RE15);
        for (int f = 0; f < 60; f++) frame();
        zensus_print("1140 RE15");
        int n = 0, ok = 1;
        for (int s = 1; s < RE15_ACTOR_MAX; s++) {
            re15_actor_t *e = &g_actors[s];
            if (!e->active || e->type != 0x16) continue;
            const int16_t hp0 = e->hp; const uint8_t hr0 = e->hit_react;
            explosion_bei(e, 300, 2);
            printf("   Fresser Slot %d: +0x93 0x%02X -> 0x%02X, hp %d -> %d, st %d\n", s, hr0, e->hit_react,
                   hp0, e->hp, e->state);
            if (!((hr0 & 1) && e->hp == hp0 && (e->hit_react & 2))) ok = 0;
            n++;
        }
        CHECK(189, n > 0 && ok, "0x16 liegend (RE1.5): %d Fresser, alle +0x93 Bit 0 in Ruhe, kein Schaden, |= 2 "
                               "(@0x80012fbc-cc)", n);
    }
}

/* =========================================================================================
 * TEIL "g5" — B9: Endkampf-G5 (ROOM5090, RE2-Modul em36): Schaden E16, Sperre, Akku, Fenster.
 * Aufbau wie probe_g5_boss.c:52-101 (Bank EM036, Slot 2, Kampfstart grid 0x13).
 * ========================================================================================= */
extern void re15_g5_boss_tick(int slot);
extern void re15_g5_flinch_zustand(int *akku, int *takt, int *fenster, int *sub);

static int s_g5_arena_slot = 2;
static re15_actor_t *g5_arena(void)
{
    re15_ai_flavor_set(RE15_AI_FLAVOR_RE2);
    re15_game_state_init();
    re15_actor_init(); re15_enemy_reset(); re15_enemy_ai_set_paused(0);
    g_room_rdt_ok = 0;
    g_current_room_id = 0x5090;
    load_re2_bank(0x36);
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    memset(pl, 0, sizeof *pl);
    pl->active = 1; pl->type = 0; pl->hp = 100; pl->x = 13699; pl->z = -23400; pl->y = 0;
    /* Das Modul setzt seinen Zustand nur bei einem SLOT-Wechsel neu auf (enemy_ai_boss_g5.c
     * `s_g5_slot != slot`) — jede Arena nimmt deshalb einen anderen Slot. */
    static int s_n = 0;
    s_g5_arena_slot = 2 + (s_n++ % 8);
    re15_actor_t *e = &g_actors[s_g5_arena_slot];
    memset(e, 0, sizeof *e);
    e->active = 1; e->type = 0x36; e->flags = 1; e->hp = 600;
    e->x = 1200; e->z = -23350; e->grid_id = 0x33;
    re15_enemy_apply_hitbox(e, 0x36);
    re15_g5_boss_tick(s_g5_arena_slot);                 /* Ctor (HP 600 @0x801003fc) */
    e->grid_id = 0x13;                                  /* Kampfstart (Member_set @0x130A) */
    for (int f = 0; f < 3; f++) { re15_g5_boss_tick(s_g5_arena_slot); pl->hp = 100; pl->hit_react = 0; }
    return e;
}

static void g5_bild(re15_actor_t *e)
{
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    re15_g5_boss_tick(s_g5_arena_slot);
    pl->hp = 100; pl->hit_react = 0; pl->state = 0;
    (void)e;
}

static void teil_g5(void)
{
    printf("== g5 (B9)\n");
    int akku, takt, fenster, sub;
    /* (190) Granate Zeile 9: HP 600 - 80 (E16, Record @0x800A5F7C), Sperre 15 (w1 @0x800A5F80). */
    re15_actor_t *e = g5_arena();
    const int16_t hp0 = e->hp;
    explosion_bei(e, 300, 2);
    CHECK(190, hp0 == 600 && e->hp == 520 && e->re2z_self1d3 == 15 && (e->hit_react & 1u) && e->sub_state_1 == 9,
          "G5 HE: hp %d -> %d (600 - 80), +0x1D3 %d (15), +0x93 0x%02X, +0x5 %d (9)", hp0, e->hp,
          e->re2z_self1d3, e->hit_react, e->sub_state_1);
    g5_bild(e);
    re15_g5_flinch_zustand(&akku, &takt, &fenster, &sub);
    CHECK(191, akku == 14 && fenster == 1 && takt == 15 && sub != 0xF && e->re2z_self1d3 == 14,
          "nach dem Bild: Akku %d (14 @0x801056BC), Fenster %d, Zaehler %d (15 @0x80102a48), Sub 0x%x (kein "
          "STAGGER), Sperre %d (14 @0x801000ec)", akku, fenster, takt, sub, e->re2z_self1d3);
    /* (192) Waehrend der Sperre: zweite Explosion -> kein Schaden, nur |= 2 (Resolver-Riegel). */
    {
        const int16_t hp1 = e->hp;
        explosion_bei(e, 300, 2);
        CHECK(192, e->hp == hp1 && (e->hit_react & 3u) == 3u,
              "Sperre: 2. Explosion im Bild 1 -> hp %d -> %d (unveraendert), +0x93 0x%02X (Bit 0|2)", hp1, e->hp,
              e->hit_react);
    }
    /* (193) Sperre laeuft ab -> Bit 0 frei genau nach 15 Bildern; der Akku ist dann 14 (Zerfall erst nach
     *       16 Bildern im Fenster). */
    int frei = -1;
    for (int f = 2; f <= 20 && frei < 0; f++) { e->hit_react &= (uint8_t)~2u; g5_bild(e); if (!(e->hit_react & 1u)) frei = f; }
    re15_g5_flinch_zustand(&akku, &takt, &fenster, &sub);
    CHECK(193, frei == 15 && akku == 14 && sub != 0xF,
          "Sperre frei in Bild %d (15), Akku %d (14), Sub 0x%x", frei, akku, sub);
    /* (194) zweite Granate innerhalb des Zerfalls -> STAGGER (Akku >= 15 @0x80102a58). */
    explosion_bei(e, 300, 2);
    g5_bild(e);
    re15_g5_flinch_zustand(&akku, &takt, &fenster, &sub);
    CHECK(194, sub == 0xF && akku == 0 && fenster == 0 && e->hp == 440,
          "2. Treffer: Sub 0x%x (0xF STAGGER), Akku %d / Fenster %d (zurueckgesetzt @0x80102a84-90), hp %d (440)",
          sub, akku, fenster, e->hp);
    /* (195) NEGATIV: ein einzelner Treffer taumelt nie; der Akku zerfaellt je 16 Bilder, das Fenster schliesst
     *       bei 0 (@0x80100150). */
    e = g5_arena();
    explosion_bei(e, 300, 2);
    int stagger = 0, bild_13 = -1, bild_0 = -1;
    for (int f = 1; f <= 260; f++) {
        g5_bild(e);
        re15_g5_flinch_zustand(&akku, &takt, &fenster, &sub);
        if (sub == 0xF && !stagger)
            printf("   STAGGER in Bild %d: akku=%d takt=%d fenster=%d hr=0x%02X lock=%d hp=%d\n", f, akku, takt,
                   fenster, e->hit_react, e->re2z_self1d3, e->hp);
        if (f < 40 && getenv("G5DBG"))
            printf("   DBG %d: akku=%d takt=%d fen=%d sub=%x hr=0x%02X lock=%d hp=%d\n", f, akku, takt, fenster,
                   sub, e->hit_react, e->re2z_self1d3, e->hp);
        if (sub == 0xF) stagger = 1;
        if (bild_13 < 0 && akku == 13) bild_13 = f;
        if (bild_0 < 0 && akku == 0) bild_0 = f;
    }
    CHECK(195, !stagger && bild_13 == 17 && bild_0 == 1 + 14 * 16 && fenster == 0,
          "Einzeltreffer: kein STAGGER %d, Akku 13 in Bild %d (17 = 1 + 16), 0 in Bild %d (%d), Fenster %d",
          !stagger, bild_13, bild_0, 1 + 14 * 16, fenster);
    /* (196) Bodenfeuer (Op 40, Hitcode 0x2002000A = Zeile 10 Klammer 2) am G5: RE2-Kandidat mit dem
     *       em36-Kasten -> HP -5 (w0 0x00511846 >> 20 @0x800A5F90), Sperre 15, Akku +14 (Zeile 10). */
    e = g5_arena();
    {
        const int32_t P[3] = { e->x - 2000, e->y - 100, e->z };   /* Mitte +0x94 = -2000 (@0x80100534) */
        const int16_t hpa = e->hp;
        const int r = re15_re2_gl_apply(P, 0, k_box_op40, 0x2002000Au);
        const int lock = e->re2z_self1d3;
        const int r2 = re15_re2_gl_apply(P, 0, k_box_op40, 0x2002000Au);   /* Gate 2: gesperrt */
        g5_bild(e);
        re15_g5_flinch_zustand(&akku, &takt, &fenster, &sub);
        CHECK(196, r != 0 && r2 == 0 && e->hp == hpa - 5 && lock == 15 && akku == 14 && fenster == 1,
              "G5 Bodenfeuer: r=%d, 2. Aufruf r=%d (Gate 2), hp %d -> %d (-5), Sperre %d (15), Akku %d (14)",
              r, r2, hpa, e->hp, lock, akku);
    }
}

int main(int argc, char **argv)
{
    const char *teil = (argc > 1) ? argv[1] : "alle";
    int alle = (strcmp(teil, "alle") == 0);
    if (alle || strstr(teil, "zombie")) teil_zombie();   /* Teile auch als Liste "a,b" */
    if (alle || strstr(teil, "hund")) teil_hund();
    if (alle || strstr(teil, "spinne")) teil_spinne();
    if (alle || strstr(teil, "re15")) teil_re15();
    if (alle || strstr(teil, "g5")) teil_g5();
    printf("probe_r34_reaktion %s: %d Fehler (erste Pruefung %d)\n", teil, s_fails, s_first_fail);
    return s_first_fail;
}
