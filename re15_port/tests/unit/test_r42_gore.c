/*
 * test_r42_gore.c — Runde 42 Strang "gore", Stufe 1 (BAUPLAN gore_matrix/BAUPLAN.md U1, U10, U17; Nutzerentscheide
 * NUTZERENTSCHEIDE_1116.md vom 16./17.11.2026).
 *
 *   [mess]  Messung (kein Riegel): Clip 1 der Unterhaelfte (Bank B, re15_re2z_lower_kf) Bild fuer Bild - Hoehe von Becken
 *           und Knien, damit die Kippzeit der Granatwerfer-Beine aus dem Clip kommt und nicht geraten ist; dazu die
 *           Knochen-Eltern des RE2-Zombies.
 *   [u1]    Granatwerfer (echter Applier re15_re2_gl_apply, Hitcode 0x10020009 = RE2 Op 47 @0x80020d54-58) toetet einen
 *           STEHENDEN Zombie jedes Typs 0x10/0x11/0x12/0x13/0x16/0x18 im RE2-Gehirn: keine Zerteilung, der Oberkoerper
 *           wird RE2-Kriecher (+0x10E Bit 0, HP 10 @0x80106ACC-E8), ein Pistolentreffer toetet ihn; die Beine bleiben am
 *           Todesort, stehen die ersten Bilder (Clip 1 Bild 0), kippen danach und frieren ein (part[1] |= 0x40).
 *           Handgranate (Art 2) zerteilt weiter; liegender Zombie + Granatwerfer wird zerteilt.
 *           RE1.5-Gehirn: Granatwerfer (+0x5 = 15) -> Oberkoerperzweig (Kriecher 30, @0x80108970), Handgranate (+0x5 = 9)
 *           -> Zerteilen, Saeure-Granate (+0x5 = 10) -> Standard-Tod (keine Gore-Zelle mehr).
 *   [u10]   Handgranate: jeder Zombie-Typ mit seiner hoechsten Port-HP stirbt am RAND des Wirkbereichs und wird zerteilt;
 *           D_G = hoechste Zombie-HP + 1 (Todesregel hp < 0).
 *   [u17]   Schrot-Kriecher 66FC: die Beine bleiben am Platz (kein Weg-Schub @0x8010B970-9C / @0x8010BA4C).
 *   [aus]   eigener Prozess mit RE15_KEIN_80PROZENT_GORE=1: Granatwerfer wie vor Runde 42 (keine Abtrennung), Schrot-Beine
 *           gleiten wie RE2.
 * Rueckgabe 0 = gruen, sonst die Nummer der ersten fehlgeschlagenen Pruefung.
 * Aufruf: test_r42_gore [mess|u1|u10|u17|alle|aus]
 *
 * GEGENPROBE: die Riegel lesen nur Felder und Funktionen, die es schon auf aaea5f8c2 gibt, damit derselbe Test gegen die
 * Engine VOR Runde 42 baut und dort rot ist.
 */
#if !defined(_WIN32) && !defined(_DEFAULT_SOURCE)
#define _DEFAULT_SOURCE 1
#endif
#include "re15_esp.h"
#include "re15_granate_r35.h"
#include "re15_actor.h"
#include "re15_damage.h"
#include "re15_scd.h"
#include "re15_rdt.h"
#include "re15_aot.h"
#include "re15_room.h"
#include "re15_enemy_ai.h"
#include "re15_enemy.h"
#include "re15_ai_flavor.h"
#include "re15_player.h"
#include "re15_camera.h"
#include "re15_game_step.h"
#include "re15_collision.h"
#include "re15_msg.h"
#include "re15_tim.h"
#include "re15_teile_r15.h"
#include "re15_skeleton.h"
#include "re2_ems.h"
#include "re2_fx.h"
#include "re15_emd.h"
#include "re15_math.h"
#include "re15_brandzombie.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef RE15_ASSET_PSX_DIR
#define RE15_ASSET_PSX_DIR "shared_assets/PSX"
#endif

extern void re15_player_aim_reset(void);
extern void re15_player_set_aim_clip_len(int fc);
extern int  re15_re2z_last_death_handler(void);
extern unsigned re15_re2z_granate_zerfetzt_zahl(void);
extern void re15_re2_damage_model_set(int on);
extern int  re15_enemy_waffen_schaden(const re15_actor_t *e, int weapon_id);
extern int  re15_re2_gl_apply(const int32_t p[3], int16_t gier, const int16_t box_in[4], uint32_t hitcode);

static int s_erste = 0, s_fehler = 0;
#define PRUEF(nr, cond, ...) do { if (!(cond)) { \
        printf("FAIL %d: ", (nr)); printf(__VA_ARGS__); printf("\n"); \
        if (!s_erste) s_erste = (nr); s_fehler++; } else { printf("ok   %d: ", (nr)); printf(__VA_ARGS__); printf("\n"); } } while (0)

static re15_esp_t s_core;
static uint8_t *slurp(const char *p, size_t *n)
{
    FILE *f = fopen(p, "rb"); if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    if (sz <= 0) { fclose(f); return NULL; }
    uint8_t *b = (uint8_t *)malloc((size_t)sz);
    if (b && fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); b = NULL; }
    fclose(f); if (b) *n = (size_t)sz; return b;
}

/* ---- Raum / Spielschritt (Muster test_r41_granate.c) ---------------------------------------------------------------- */
static re15_rdt_t         s_rdt;
static uint8_t           *s_rdt_buf = NULL;
static re15_camera_view_t s_cam;
static re15_game_ctx_t    s_ctx;
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
    memset(&s_ctx, 0, sizeof s_ctx);
    s_ctx.rdt = &s_rdt; s_ctx.rdt_ok = 1; s_ctx.cam_view = &s_cam; s_ctx.active_cut = 0;
    g_current_room_id = (uint16_t)room_id;
    return 0;
}
/* Ein Zombie `quelle` aus ROOM1140 (0x10 / 0x11 / 0x16), wahlweise umgetypt auf `typ`, alle anderen Gegner aus. */
static re15_actor_t *zombie_arena(re15_ai_flavor_t fl, uint8_t quelle, uint8_t typ)
{
    re15_ai_flavor_set(fl);
    re15_game_state_init();
    re15_actor_init(); re15_aot_init(); scd_vm_init();
    re15_enemy_reset(); re15_enemy_ai_set_paused(0);
    re15_player_cmd_reset(); re15_player_aim_reset();
    re15_esp_fx_reset();
    re15_teile_reset_alle();
    re15_damage_seed_rng(0x0badf00du);
    g_room_rdt_ok = 0; g_room_change.pending = 0;
    { extern void scd_register_current_rdt(const re15_rdt_t *rdt); scd_register_current_rdt(NULL); }
    if (s_rdt.main_scd)   scd_thread_start(0, s_rdt.main_scd);
    if (s_rdt.sub_scd[0]) scd_thread_start(1, s_rdt.sub_scd[0]);
    g_scd.work_vars[10] = 0;
    for (int i = 0; i < 120; i++) scd_vm_tick();
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100; pl->hit_react = 0;
    pl->state = 0; pl->motion = 0; pl->floor = 0; pl->y = 0;
    re15_collision_set_band(0);
    re15_player_set_aim_clip_len(12);
    for (int s = 1; s < RE15_ACTOR_MAX; s++)
        if (g_actors[s].active && re15_re2_owns_type(g_actors[s].type)) load_re2_bank(g_actors[s].type);
    load_re2_bank(typ);
    for (int f = 0; f < 60; f++) frame();
    int slot = -1;
    for (int s = 1; s < RE15_ACTOR_MAX; s++)
        if (g_actors[s].active && g_actors[s].type == quelle && g_actors[s].state != 7) { slot = s; break; }
    if (slot < 0) return NULL;
    for (int s = 1; s < RE15_ACTOR_MAX; s++) if (s != slot) g_actors[s].active = 0;
    re15_actor_t *e = &g_actors[slot];
    e->type = typ;
    e->grid_id = 0; e->re2z_f10e = 0; e->re2z_flags21a &= (uint16_t)~0x12u; e->re2z_self1d3 = 0;
    e->hit_react = 0;
    re15_ai_set_state_word(e, 0x101);
    pl->x = e->x - 6000; pl->z = e->z; pl->y = e->y;
    for (int f = 0; f < 10; f++) frame();
    e->hit_react = 0;
    return e;
}

/* Die Pose der Unterhaelfte (Bank B, re15_re2z_lower_kf) -> Hoehe ueber dem Boden von Becken (1) und dem tiefsten /
 * hoechsten Beinknochen 2..6. y < 0 = oben (PSX). */
static int unterhaelfte_hoehe(const re15_actor_t *e, int *becken, int *knie_max)
{
    const re15_emd_skeleton_t *sk = NULL;
    int kf = re15_re2z_lower_kf(e, &sk);
    if (!sk) return -1;
    static re15_skel_pose_t P[RE15_EMD_MAX_BONES];
    if (re15_skel_compute_pose(sk, kf, P) != 0) return -1;
    int fix = re15_skel_root_y_fix(sk, kf);
    *becken = -(P[1].trans[1] + fix);
    int km = -100000;
    for (int b = 2; b <= 6; b++) { int h = -(P[b].trans[1] + fix); if (h > km) km = h; }
    *knie_max = km;
    return kf;
}

/* ================================================================================================
 * [mess] Clip 1 der Unterhaelfte Bild fuer Bild
 * ================================================================================================ */
static void abschnitt_mess(void)
{
    if (room_load(0x1140, "STAGE1") != 0) return;
    re15_actor_t *e = zombie_arena(RE15_AI_FLAVOR_RE2, 0x10, 0x10);
    if (!e) { printf("kein Zombie\n"); return; }
    const re15_enemy_bank_t *b = re15_enemy_find(e->type);
    if (!b) return;
    printf("Bank 0x%02x: skel %d Knochen, own_ok %d loco_ok %d\n", e->type, b->skel.bone_count, b->own_ok, b->loco_ok);
    printf("Eltern:");
    for (int i = 0; i < b->skel.bone_count; i++) printf(" %d:%d", i, (int)b->skel.bone_parent[i]);
    printf("\n");
    const re15_emd_animation_t *A = b->own_ok ? &b->anim_own : &b->anim;
    for (int clip = 0; clip <= 1; clip++) {
        int len = (clip < A->clip_count) ? A->clips[clip].frame_count : 0;
        printf("Clip %d (Bank B): %d Bilder\n", clip, len);
        for (int f = 0; f < len; f++) {
            e->re2z_low_state = 3u; e->re2z_low_clip = (uint8_t)clip; e->re2z_low_c1 = (uint8_t)f;
            int hb = 0, hk = 0;
            int kf = unterhaelfte_hoehe(e, &hb, &hk);
            printf("  Bild %2d kf %4d Becken %5d hoechster Beinknochen %5d\n", f, kf, hb, hk);
        }
    }
}

/* Der Granatwerfer: RE2 Op 47 Sub 12 (GL-Runde) ruft den Applier mit Hitcode 0x10020009 (`lui a3,0x1002 / ori a3,a3,0x9`
 * @0x80020d54-58) und der Box @0x80010918 = {-2000,0,1000,500}; Port re2_fx.c:1390. Hier derselbe Aufruf direkt. */
static int werfer(const re15_actor_t *e)
{
    const int16_t box[4] = RE15_GRANATE_R35_BOX_EXPLOSION;
    const int32_t p[3] = { e->x + 300, e->y - 500, e->z };
    return re15_re2_gl_apply(p, 0, box, 0x10020009u);
}
static int handgranate(const re15_actor_t *e, uint8_t art)
{
    const int32_t p[3] = { e->x + 300, e->y - 500, e->z };
    return re15_granate_r35_explosion(p, 0, art);
}
static int abstand(int32_t ax, int32_t az, int32_t bx, int32_t bz)
{
    int64_t dx = ax - bx, dz = az - bz;
    return (int)re15_squareroot0((uint32_t)(dx * dx + dz * dz));
}

/* ================================================================================================
 * [u1] Granatwerfer trennt den Oberkoerper, Handgranate zerteilt
 * ================================================================================================ */
static const struct { uint8_t quelle, typ; } k_zombies[] = {
    { 0x10, 0x10 }, { 0x11, 0x11 }, { 0x16, 0x16 }, { 0x10, 0x12 }, { 0x10, 0x13 }, { 0x10, 0x18 },
};
#define N_ZOMBIES ((int)(sizeof k_zombies / sizeof k_zombies[0]))
static void abschnitt_u1(void)
{
    if (room_load(0x1140, "STAGE1") != 0) { PRUEF(10, 0, "ROOM1140 fehlt"); return; }
    int tot = 0, nicht_zerteilt = 0, h66 = 0, steht = 0, kippt = 0, liegt = 0, am_ort = 0, friert = 0, kriecher = 0,
        pistole = 0, rumpf_weg = 0;
    for (int k = 0; k < N_ZOMBIES; k++) {
        re15_actor_t *e = zombie_arena(RE15_AI_FLAVOR_RE2, k_zombies[k].quelle, k_zombies[k].typ);
        if (!e) { PRUEF(10, 0, "kein Zombie 0x%02x in ROOM1140", k_zombies[k].quelle); return; }
        e->hp = 1;                                   /* jeder Treffer toetet: hier geht es um das Bild, nicht den Schaden */
        const int32_t ox = e->x, oz = e->z;          /* Todesort (Lage beim Treffer) */
        unsigned z0 = re15_re2z_granate_zerfetzt_zahl();
        re15_damage_seed_rng(0x4200u + (uint32_t)k * 7919u);
        int n = werfer(e);
        if (n >= 1 && e->state == 3 && e->sub_state_1 == 9u) tot++;
        frame();                                     /* erster DEATH-Takt */
        int hdl = re15_re2z_last_death_handler();
        if (re15_re2z_granate_zerfetzt_zahl() == z0) nicht_zerteilt++;
        if (hdl == 4) h66++;                         /* RE2ZD_66FC */
        /* Stehen: 17 Bilder lang Clip 1 Bild 0 (Becken-Hoehe >= 1900, gemessen Bild 0..3 = 1976..1979) */
        int st_ok = 1, kp_bild = -1, lg_ok = 1, ort_ok = 1, frz = 0, kr_bild = -1;
        for (int f = 0; f < 400; f++) {
            int hb = 0, hk = 0;
            int kf = (e->re2z_rag_anchor_on) ? unterhaelfte_hoehe(e, &hb, &hk) : -1;
            if (f < 16 && (kf < 0 || hb < 1900)) st_ok = 0;
            if (kp_bild < 0 && kf >= 0 && hb < 300) kp_bild = f;
            if (kp_bild >= 0 && f > kp_bild && f < kp_bild + 60 && hb > 500) lg_ok = 0;   /* bleibt unten */
            if (e->re2z_rag_anchor_on && e->re2z_low_state != 0u &&
                abstand(e->re2z_low_x, e->re2z_low_z, ox, oz) > 0) ort_ok = 0;
            if (e->re2z_part_flags[1] & 0x40u) frz = 1;
            if (getenv("R42_DBG") && k == 0 && f < 120)
                printf("      f%3d st %u clip %u c1 %u Becken %d Bein %d low (%d,%d) e (%d,%d) frz %d\n", f, e->re2z_low_state,
                       e->re2z_low_clip, e->re2z_low_c1, hb, hk, e->re2z_low_x, e->re2z_low_z, e->x, e->z,
                       (e->re2z_part_flags[1] & 0x40u) ? 1 : 0);
            if (kr_bild < 0 && (e->re2z_f10e & 1u) && e->state == 1) kr_bild = f;
            frame();
        }
        if (st_ok) steht++;
        if (kp_bild >= 16 && kp_bild <= 20) kippt++;  /* 17 Bilder Stand + 3 Bilder Kippen (0:18,45-0:19,10) */
        if (kp_bild >= 0 && lg_ok) liegt++;
        if (ort_ok && e->re2z_rag_anchor_on) am_ort++;
        if (frz) friert++;
        if (kr_bild >= 0 && e->hp == 10 && (e->re2z_f10e & 1u)) kriecher++;
        if (re15_enemy_waffen_schaden(e, 3) > e->hp) pistole++;   /* w3 = Handfeuerwaffe (RE2 Zeile 3 @0x800A4154) */
        int weg = abstand(e->x, e->z, e->re2z_low_x, e->re2z_low_z);
        if (weg > 200) rumpf_weg++;
        printf("   Typ 0x%02x: Treffer %d Zeile %u Handler %d Zerteilung %u | Beine: Stand %d Kippbild %d liegen %d am Ort %d "
               "eingefroren %d | Kriecher ab Bild %d HP %d f10e 0x%04x Pistole %d | Rumpf-Beine %d\n",
               k_zombies[k].typ, n, e->sub_state_1, hdl, re15_re2z_granate_zerfetzt_zahl() - z0, st_ok, kp_bild, lg_ok,
               ort_ok, frz, kr_bild, e->hp, (unsigned)e->re2z_f10e, re15_enemy_waffen_schaden(e, 3), weg);
    }
    PRUEF(10, tot == N_ZOMBIES, "Granatwerfer (Applier 0x10020009) toetet jeden Zombie-Typ (Zustand 3, Zeile 9) %d/%d", tot, N_ZOMBIES);
    PRUEF(11, nicht_zerteilt == N_ZOMBIES, "Granatwerfer zerteilt NICHT (Nutzerentscheid F1) %d/%d", nicht_zerteilt, N_ZOMBIES);
    PRUEF(12, h66 == N_ZOMBIES, "Oberkoerper laeuft durch den Ragdoll 66FC (Handler 4) %d/%d", h66, N_ZOMBIES);
    PRUEF(13, steht == N_ZOMBIES, "die Beine STEHEN die ersten 16 Bilder (Clip 1 Bild 0, Becken >= 1900) %d/%d", steht, N_ZOMBIES);
    PRUEF(14, kippt == N_ZOMBIES, "die Beine liegen nach 17 + 3 Bildern (80 %% 0:18,45-0:19,10) %d/%d", kippt, N_ZOMBIES);
    PRUEF(15, liegt == N_ZOMBIES, "die Beine bleiben liegen (Becken <= 500 in den 60 Bildern danach) %d/%d", liegt, N_ZOMBIES);
    PRUEF(16, am_ort == N_ZOMBIES, "die Beine bleiben am Todesort (kein Weg-Schub, 400 Bilder) %d/%d", am_ort, N_ZOMBIES);
    PRUEF(17, friert == N_ZOMBIES, "die Beine frieren ein (part[1] |= 0x40 @0x8010BA88-9C) %d/%d", friert, N_ZOMBIES);
    PRUEF(18, kriecher == N_ZOMBIES, "der Oberkoerper wird RE2-Kriecher mit HP 10 (@0x80106ACC-E8, +0x10E Bit 0 @0x80106B0C) %d/%d",
          kriecher, N_ZOMBIES);
    PRUEF(19, pistole == N_ZOMBIES, "ein Pistolentreffer toetet den Kriecher (RE2-Schaden > 10) %d/%d", pistole, N_ZOMBIES);
    PRUEF(20, rumpf_weg == N_ZOMBIES, "Oberkoerper und Beine liegen getrennt (> 200) %d/%d", rumpf_weg, N_ZOMBIES);

    /* Handgranate zerteilt weiter (RE2-Gehirn), liegender Zombie + Granatwerfer wird zerteilt */
    {
        re15_actor_t *e = zombie_arena(RE15_AI_FLAVOR_RE2, 0x10, 0x10);
        if (!e) { PRUEF(21, 0, "kein Zombie"); return; }
        e->hp = 143;
        unsigned z0 = re15_re2z_granate_zerfetzt_zahl();
        int n = handgranate(e, 2);
        frame();
        for (int f = 0; f < 60; f++) frame();
        PRUEF(21, n == 1 && re15_re2z_granate_zerfetzt_zahl() == z0 + 1u && !(e->re2z_f10e & 1u) && !e->re2z_rag_anchor_on,
              "Handgranate (Art 2) zerteilt den Zombie (Zerteilung +%u, kein Kriecher, kein Aushang)",
              re15_re2z_granate_zerfetzt_zahl() - z0);
    }
    {
        re15_actor_t *e = zombie_arena(RE15_AI_FLAVOR_RE2, 0x10, 0x10);
        if (!e) { PRUEF(22, 0, "kein Zombie"); return; }
        e->hp = 1;
        e->re2z_flags21a |= 0x10u;                   /* liegt (Zweig 2 @0x801083EC-F0) */
        unsigned z0 = re15_re2z_granate_zerfetzt_zahl();
        int n = werfer(e);
        frame();
        PRUEF(22, n >= 1 && re15_re2z_granate_zerfetzt_zahl() == z0 + 1u && !e->re2z_rag_anchor_on,
              "liegender Zombie + Granatwerfer: zerteilt (keine stehenden Beine, BAUPLAN U1 Weg 4), Zerteilung +%u",
              re15_re2z_granate_zerfetzt_zahl() - z0);
    }

    /* RE1.5-Gehirn (Import-Option an = Vorgabe) */
    {
        re15_actor_t *e = zombie_arena(RE15_AI_FLAVOR_RE15, 0x10, 0x10);
        if (!e) { PRUEF(23, 0, "kein Zombie (RE1.5)"); return; }
        e->hp = 1;
        unsigned z0 = re15_re2z_granate_zerfetzt_zahl();
        int n = werfer(e);
        uint8_t w = e->sub_state_1;
        int kr = -1;
        for (int f = 0; f < 300 && kr < 0; f++) { frame(); if (e->grid_id == 0x81 && e->state == 1) kr = f; }
        PRUEF(23, n >= 1 && w == 15u && kr >= 0 && e->hp == 0x1e && re15_re2z_granate_zerfetzt_zahl() == z0,
              "RE1.5-KI Granatwerfer (+0x5 = %u): Oberkoerperzweig, Kriecher +9 = 0x81 ab Bild %d, HP %d (0x1e @0x80108970), "
              "Zerteilung +%u", w, kr, e->hp, re15_re2z_granate_zerfetzt_zahl() - z0);
    }
    {
        re15_actor_t *e = zombie_arena(RE15_AI_FLAVOR_RE15, 0x10, 0x10);
        if (!e) { PRUEF(24, 0, "kein Zombie (RE1.5)"); return; }
        e->hp = 1;
        int n = handgranate(e, 3);                   /* Saeure-Granate, +0x5 = 10 (DAT_8006f430[3] @0x8006f433) */
        uint8_t w = e->sub_state_1;
        int kr = 0;
        for (int f = 0; f < 200; f++) { frame(); if (e->grid_id == 0x81) kr = 1; }
        PRUEF(24, n == 1 && w == 10u && !kr && e->state == 7,
              "RE1.5-KI Saeure-Granate (+0x5 = %u): Standard-Tod (Zelle 0 @0x8011fff0), kein Kriecher (%d), Leiche (Zustand %u)",
              w, kr, e->state);
    }
}

/* ================================================================================================
 * [u10] Handgranate: jeder Zombie im Wirkbereich stirbt und wird zerteilt, auch am Rand
 * ================================================================================================ */
/* Die hoechste Zombie-HP im Port (Regel 2 des Nutzers: RE2-Werte). Quelle je Wert (info/re2leon/COMMON/BIN/EMOVL10_S0.BIN):
 *   0x11: 250 fest (`addiu v1,zero,250` @0x801008C8 / `sh v1,342(s2)` @0x801008CC)
 *   sonst: Tabelle @0x8010C670 (Maximum 128 @0x8010C674) + 15 bei weniger als 4 Gegnern (@0x80100810-14) = 143.
 * Todesregel hp < 0 (`bgez v1` @0x80013014 RE1.5 / @0x80047284 RE2): genau ausreichend = hoechste HP + 1 = 251. */
#define U10_MAX_HP   250
#define U10_DG_SOLL  (U10_MAX_HP + 1)
static void abschnitt_u10(void)
{
    if (room_load(0x1140, "STAGE1") != 0) { PRUEF(30, 0, "ROOM1140 fehlt"); return; }
    static const struct { re15_ai_flavor_t fl; const char *name; } kis[2] = {
        { RE15_AI_FLAVOR_RE2, "RE2-KI" }, { RE15_AI_FLAVOR_RE15, "RE1.5-KI" } };
    int rand_tot = 0, rand_zerteilt = 0, aussen_heil = 0, faelle = 0;
    for (int ki = 0; ki < 2; ki++)
        for (int k = 0; k < N_ZOMBIES; k++) {
            re15_actor_t *e = zombie_arena(kis[ki].fl, k_zombies[k].quelle, k_zombies[k].typ);
            if (!e) { PRUEF(30, 0, "kein Zombie 0x%02x", k_zombies[k].quelle); return; }
            const int16_t hp0 = (k_zombies[k].typ == 0x11) ? 250 : 143;
            /* Rand suchen: Explosionspunkt P immer gleich, Zombie entlang +x verschoben, bis der Kandidatentest
             * (dieselben Tore wie die Explosion, re15_re2_gl_kandidat) nicht mehr trifft. */
            const int32_t ex = e->x, ez = e->z;
            const int32_t p[3] = { ex, e->y - 500, ez };
            int rand_dx = -1;
            for (int dx = 0; dx <= 6000; dx += 10) {
                int16_t box[4] = RE15_GRANATE_R35_BOX_EXPLOSION;
                int16_t box2[4] = RE15_GRANATE_R35_BOX_EXPLOSION;
                int32_t q[3] = { p[0], p[1] + RE15_GRANATE_R35_EXPLOSION_DY, p[2] };
                e->x = ex + dx; e->hit_react = 0; e->re2z_self1d3 = 0;
                if (re15_re2_gl_kandidat(e, p, 0, box) || re15_re2_gl_kandidat(e, q, 0, box2)) rand_dx = dx;
            }
            faelle++;
            if (rand_dx < 0) { printf("   %s 0x%02x: kein Rand gefunden\n", kis[ki].name, k_zombies[k].typ); continue; }
            /* am Rand */
            e->x = ex + rand_dx; e->hp = hp0; e->hit_react = 0; e->re2z_self1d3 = 0;
            unsigned z0 = re15_re2z_granate_zerfetzt_zahl();
            int n = re15_granate_r35_explosion(p, 0, 2);
            int16_t hp1 = e->hp;
            frame();
            if (n == 1 && hp1 == hp0 - RE15_GRANATE_DG && hp1 < 0 && e->state == 3) rand_tot++;
            if (re15_re2z_granate_zerfetzt_zahl() == z0 + 1u) rand_zerteilt++;
            printf("   %s 0x%02x: Rand dx %d, HP %d -> %d, Zustand %u, Zerteilung +%u\n", kis[ki].name, k_zombies[k].typ,
                   rand_dx, hp0, hp1, e->state, re15_re2z_granate_zerfetzt_zahl() - z0);
            /* knapp ausserhalb: nichts */
            re15_actor_t *f = zombie_arena(kis[ki].fl, k_zombies[k].quelle, k_zombies[k].typ);
            if (!f) continue;
            f->x = f->x + rand_dx + 10; f->hp = hp0;
            const int32_t pf[3] = { f->x - rand_dx - 10, f->y - 500, f->z };
            int nf = re15_granate_r35_explosion(pf, 0, 2);
            if (nf == 0 && f->hp == hp0) aussen_heil++;
        }
    PRUEF(30, rand_tot == faelle, "Handgranate am RAND des Wirkbereichs: jeder Zombie-Typ aus seiner hoechsten HP tot (RE2-KI und "
          "RE1.5-KI) %d/%d", rand_tot, faelle);
    PRUEF(31, rand_zerteilt == faelle, "... und zerteilt %d/%d", rand_zerteilt, faelle);
    PRUEF(32, aussen_heil == faelle, "10 Einheiten ausserhalb: kein Treffer (kein Schadensabfall, kein Rand-Rest) %d/%d", aussen_heil, faelle);
    PRUEF(33, RE15_GRANATE_DG == U10_DG_SOLL, "D_G = %d = hoechste Zombie-HP %d (0x11 @0x801008C8) + 1 (Todesregel @0x80013014)",
          RE15_GRANATE_DG, U10_MAX_HP);
    {   /* Minimalitaet: 0x11 mit 250 stirbt "gerade so" (-1); weniger Schaden liesse ihn mit 0 leben (`bgez`) */
        re15_actor_t *e = zombie_arena(RE15_AI_FLAVOR_RE2, 0x11, 0x11);
        if (e) {
            e->hp = U10_MAX_HP;
            (void)handgranate(e, 2);
            PRUEF(34, e->hp == -1 && e->state == 3, "0x11 mit 250 HP: Handgranate -> HP %d (soll -1: genau so viel wie noetig)", e->hp);
        }
    }
}

/* ================================================================================================
 * [u17] Schrot aus der Naehe, Oberkoerper-Kriecher: die Beine bleiben am Platz
 * ================================================================================================ */
/* Gibt die Strecke zurueck, die die Unterhaelfte zwischen Maschinenstart und Bild 400 zuruecklegt; -1 = kein 66FC-Kriecher. */
static int u17_strecke(uint32_t seed, int *kriecher)
{
    re15_actor_t *e = zombie_arena(RE15_AI_FLAVOR_RE2, 0x10, 0x10);
    if (!e) return -1;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->x = e->x - 1500;
    re15_damage_seed_rng(seed);
    e->hp = -1; e->hit_react = 0x01;               /* kein Krit (+0x93 & 0x40) -> kein Zerfetzen */
    re15_ai_set_state_word(e, 0x0703);             /* Zustand 3, Zeile 7 (Schrot) */
    e->sub_state_2 = 0; e->sub_state_3 = 0; e->re2z_hits1d2 = 1;   /* Spalte 1 = Rumpf nah -> 66FC */
    int32_t sx = 0, sz = 0; int start = 0;
    *kriecher = 0;
    for (int f = 0; f < 400; f++) {
        frame();
        if (f == 0 && (re15_re2z_last_death_handler() != 4 || e->re2z_rag231 != 0)) return -1;
        if (!start && e->re2z_low_state != 0u) { start = 1; sx = e->re2z_low_x; sz = e->re2z_low_z; }
        if ((e->re2z_f10e & 1u) && e->state == 1) *kriecher = 1;
    }
    if (!start) return -1;
    return abstand(e->re2z_low_x, e->re2z_low_z, sx, sz);
}
static void abschnitt_u17(int soll_bleiben)
{
    if (room_load(0x1140, "STAGE1") != 0) { PRUEF(40, 0, "ROOM1140 fehlt"); return; }
    int faelle = 0, bleibt = 0, kr = 0;
    for (uint32_t seed = 1; seed < 200 && faelle < 4; seed++) {
        int k = 0;
        int s = u17_strecke(seed * 2654435761u, &k);
        if (s < 0) continue;
        faelle++;
        kr += k;
        if (soll_bleiben ? (s == 0) : (s > 300)) bleibt++;
        printf("   Seed %u: Strecke der Beine %d, Kriecher %d\n", seed, s, k);
    }
    if (soll_bleiben) {
        PRUEF(40, faelle == 4 && bleibt == 4, "Schrot-Kriecher 66FC: die Beine bleiben am Platz (Strecke 0, Disc @0x80108820-98) %d/%d",
              bleibt, faelle);
        PRUEF(41, kr == faelle, "der Oberkoerper kriecht wie vorher (RE2 66FC Phase 2) %d/%d", kr, faelle);
    } else {
        PRUEF(42, faelle == 4 && bleibt == 4, "Schalter aus: die Beine gleiten wie RE2 (> 300, @0x8010B970-9C / @0x8010BA4C) %d/%d",
              bleibt, faelle);
    }
}

/* ================================================================================================
 * [aus] RE15_KEIN_80PROZENT_GORE=1: Granatwerfer ohne Abtrennung (RE2 wie vor Runde 41)
 * ================================================================================================ */
static void abschnitt_aus(void)
{
    if (room_load(0x1140, "STAGE1") != 0) { PRUEF(50, 0, "ROOM1140 fehlt"); return; }
    re15_actor_t *e = zombie_arena(RE15_AI_FLAVOR_RE2, 0x10, 0x10);
    if (!e) { PRUEF(50, 0, "kein Zombie"); return; }
    e->hp = 1;
    int n = werfer(e);
    frame();
    int anker = e->re2z_rag_anchor_on, hdl = re15_re2z_last_death_handler();
    for (int f = 0; f < 200; f++) frame();
    PRUEF(50, n >= 1 && !anker && hdl != 4 && !(e->re2z_f10e & 1u),
          "Schalter aus: Granatwerfer-Tod ohne Abtrennung (Handler %d, Aushang %d, Kriecher %d)", hdl, anker,
          (int)(e->re2z_f10e & 1u));
    abschnitt_u17(0);
}

/* ================================================================================================
 * [u11mess] Messung (kein Riegel, BAUPLAN U11 Weg 1): brennender Zombie in ROOM5120, Tod durch die Pistole - laufen die
 *           acht Flammen (Effekt 8 Sub 3 am Aktor, re15_brand_scharf) an der Leiche weiter, und wie lange?
 * ================================================================================================ */
static re15_esp_t s_bank5120;
static int flammen_an(int slot, int *r18)
{
    int n = 0; *r18 = 0;
    for (int i = 0; i < 256; i++) {
        const re15_esp_fx_t *f = re15_esp_fx_get(i);
        if (!f || f->effect_id != 8 || f->sub_index != 3 || f->follow_slot != slot) continue;
        n++;
        if ((f->row[0] | f->row[1] << 8) == 18) (*r18)++;
    }
    return n;
}
static uint32_t rd32(const uint8_t *p, size_t o) { return (uint32_t)p[o] | (uint32_t)p[o+1] << 8 | (uint32_t)p[o+2] << 16 | (uint32_t)p[o+3] << 24; }
static int s_u11_min = 99, s_u11_sicht_min = 99, s_u11_naehe = 1, s_u11_leiche = 0;
static void abschnitt_u11mess(int modus)
{
    if (room_load(0x5120, "STAGE5") != 0) return;
    {   size_t n = 0; uint8_t *raw = slurp(RE15_ASSET_PSX_DIR "/STAGE5/ROOM5120.RDT", &n);
        if (raw && re15_esp_parse(raw, n, rd32(raw, 0x4C), rd32(raw, 0x50), rd32(raw, 0x54), rd32(raw, 0x58), &s_bank5120) == 0)
            re15_esp_set_room_bank(&s_bank5120); }
    re15_ai_flavor_set(RE15_AI_FLAVOR_RE2);
    re15_game_state_init();
    re15_game_flag_set(1, 31, 1);                    /* brennende Zombies (re15_brand_init: Zone 1 Bit 31 @0x801008f4-904) */
    re15_actor_init(); re15_aot_init(); scd_vm_init();
    re15_enemy_reset(); re15_enemy_ai_set_paused(0);
    re15_player_cmd_reset(); re15_player_aim_reset();
    re15_esp_fx_reset(); re15_teile_reset_alle();
    re15_damage_seed_rng(0x0badf00du);
    g_room_rdt_ok = 0; g_room_change.pending = 0;
    { extern void scd_register_current_rdt(const re15_rdt_t *rdt); scd_register_current_rdt(NULL); }
    if (s_rdt.main_scd)   scd_thread_start(0, s_rdt.main_scd);
    if (s_rdt.sub_scd[0]) scd_thread_start(1, s_rdt.sub_scd[0]);
    for (int i = 0; i < 120; i++) scd_vm_tick();
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100; pl->state = 0; pl->y = 0;
    for (int s = 1; s < RE15_ACTOR_MAX; s++)
        if (g_actors[s].active && re15_re2_owns_type(g_actors[s].type)) load_re2_bank(g_actors[s].type);
    for (int f = 0; f < 30; f++) { frame(); re15_esp_fx_tick(re15_esp_room_bank()); }
    int slot = -1;
    for (int s = 1; s < RE15_ACTOR_MAX; s++)
        if (g_actors[s].active && (g_actors[s].ai_flags & 0x100u)) { slot = s; break; }
    if (slot < 0) { printf("kein brennender Zombie in ROOM5120\n"); return; }
    for (int s = 1; s < RE15_ACTOR_MAX; s++) if (s != slot) g_actors[s].active = 0;
    re15_actor_t *e = &g_actors[slot];
    pl->x = e->x - 6000; pl->z = e->z; pl->y = e->y;
    int r18 = 0;
    printf("Slot %d Typ 0x%02x Zustand %u Zeitzaehler %d Flammen %d\n", slot, e->type, e->state, e->ai_attack_timer, flammen_an(slot, &r18));
    if (modus == 0) {                                 /* Tod durch Pistole: Zeile 3 (Handfeuerwaffe), Spalte 1 */
        e->hp = -1; re15_ai_set_state_word(e, 0x0303); e->re2z_hits1d2 = 1; e->hit_react = 1;
    } else {                                          /* Verbrennen nach Zeit */
        e->ai_attack_timer = 2;
    }
    for (int f = 0; f <= 1800; f++) {
        frame(); re15_esp_fx_tick(re15_esp_room_bank());
        {   int n0 = flammen_an(slot, &r18), sicht = 0;
            for (int i = 0; i < 256; i++) {
                const re15_esp_fx_t *q = re15_esp_fx_get(i);
                if (!q || q->effect_id != 8 || q->sub_index != 3 || q->follow_slot != slot) continue;
                if (re15_esp_fx_visible(q)) sicht++;
                if (abstand(q->x, q->z, e->x, e->z) > 2500) s_u11_naehe = 0;   /* an der Leiche, nicht im Raum verstreut */
            }
            if (f >= 30) { if (n0 < s_u11_min) s_u11_min = n0; if (sicht < s_u11_sicht_min) s_u11_sicht_min = sicht; }
            if (e->state == 7) s_u11_leiche = 1;
        }
        if (f % 60 == 0 || f < 5) {
            int n = flammen_an(slot, &r18);
            printf("  Bild %4d Zustand %u/%u aktiv %d Flammen %d (Routine 18: %d) fx gesamt %d\n", f, e->state, e->sub_state_1,
                   e->active, n, r18, re15_esp_fx_count());
        }
    }
}

int main(int argc, char **argv)
{
    const char *teil = (argc > 1) ? argv[1] : "alle";
    if (strcmp(teil, "aus") == 0) {
#ifdef _WIN32
        _putenv("RE15_KEIN_80PROZENT_GORE=1");
#else
        setenv("RE15_KEIN_80PROZENT_GORE", "1", 1);
#endif
    }
    size_t n = 0;
    uint8_t *buf = slurp(RE15_ASSET_PSX_DIR "/DATA/CORE00.ESP", &n);
    if (!buf) { fprintf(stderr, "CORE00.ESP fehlt\n"); return 1; }
    if (re15_esp_parse_global(buf, n, &s_core) != 0) { fprintf(stderr, "CORE00.ESP Parse\n"); return 1; }
    re15_esp_set_global_bank(&s_core);
    re15_esp_set_room_bank(NULL);
    re15_re2_damage_model_set(1);

    if (strcmp(teil, "mess") == 0) { abschnitt_mess(); free(buf); return 0; }
    if (strcmp(teil, "u11mess") == 0) { abschnitt_u11mess(0); free(buf); return 0; }
    if (strcmp(teil, "u11zeit") == 0) { abschnitt_u11mess(1); free(buf); return 0; }
    int alle = (strcmp(teil, "alle") == 0);
    if (alle || strcmp(teil, "u1") == 0)  { printf("[u1] Granatwerfer / Handgranate\n"); abschnitt_u1(); }
    if (alle || strcmp(teil, "u10") == 0) { printf("[u10] Handgranate am Rand\n"); abschnitt_u10(); }
    if (alle || strcmp(teil, "u17") == 0) { printf("[u17] Schrot-Kriecher\n"); abschnitt_u17(1); }
    if (alle || strcmp(teil, "u11") == 0) {
        printf("[u11] brennende Leiche (Pin, gemessen)\n");
        abschnitt_u11mess(0);
        PRUEF(60, s_u11_leiche && s_u11_min == 8 && s_u11_sicht_min == 8 && s_u11_naehe,
              "Tod durch die Pistole: alle acht Flammen (Effekt 8 Sub 3, re15_brand_scharf @0x80100954ff) laufen an der Leiche "
              "1800 Bilder weiter, sichtbar und an ihr (min %d, sichtbar %d, nahe %d, Leiche %d)", s_u11_min, s_u11_sicht_min,
              s_u11_naehe, s_u11_leiche);
    }
    if (strcmp(teil, "aus") == 0)         { printf("[aus] Schalter aus\n"); abschnitt_aus(); }

    free(buf);
    if (s_fehler) { printf("test_r42_gore: %d Fehler, erste Pruefung %d\n", s_fehler, s_erste); return s_erste; }
    printf("test_r42_gore: ALLE PRUEFUNGEN GRUEN\n");
    return 0;
}
