/*
 * test_r41_granate.c — Runde 41 Spur GR: die Handgranate zerfetzt Zombies (auch den dicken 0x11) und ist stark genug.
 *
 * Dossier: analysis/befunde_runde41/GR_granate.md. Code: re15_damage.c (Art 2 an der Zombie-Familie = 1000
 * @0x8006f41c), enemy_ai_re2_zombie.c re2z_granate_zerfetzen (RE2-Gehirn), zombiegore_r15.c z_granate (RE1.5-Gehirn).
 *
 *   [1] schaden  — Explosion (re15_granate_r35_explosion, Art 2) an der Zombie-Familie 0x10/0x11/0x12/0x13/0x16/0x18
 *                  mit der hoechsten RE2-HP (Brad 0x11 = 250 @0x801008C8, sonst 128 + 15) und der RE1.5-HP: Tod,
 *                  Schaden 1000. Saeure/Brand (Art 3/4) bleiben bei E4 (200). Nicht-Zombies unveraendert (Hund 300,
 *                  Kraehe 60, Spinne 60, G5 80, Alligator/Gorilla/Kakerlake 1000).
 *   [2] zerfetzt — RE2-Gehirn (Standard-KI), Zombies aus ROOM1140 (0x10, 0x11 dick, 0x16) und umgetypt 0x12/0x13/0x18:
 *                  Explosion -> die Zerfetzung laeuft genau einmal, Kopf/Oberarme/Oberschenkel fliegen (0x4A/0x1062),
 *                  Unterarme/Unterschenkel/Fuesse platzen (RE1.5-Teilwort 0x4a), Haende weg, Handler = Sturz 0x80108530,
 *                  Leiche (Zustand 7). Mit Zeichengang-Nachbau (Teilstufe + Part-Matrix je Bild, wie main.c): nach dem
 *                  Ausflug bleiben nur Rumpf (0) und Becken (1) gezeichnet.
 *   [3] r15      — RE1.5-Gehirn (Flavor re15): Waffe 9 -> z_granate: Kopf/Arme/Unterschenkel/Fuesse platzen, kein
 *                  Kriecher (grid 0x81 nie), Leiche.
 *   [4] aus      — eigener Prozess mit RE15_KEIN_80PROZENT_GORE=1: keine Zerfetzung (RE2 0x80108BEC wie vorher), der
 *                  Schaden bleibt 1000 (PORT-WAHL Staerke haengt nicht am Gore-Schalter).
 * Rueckgabe 0 = gruen, sonst die Nummer der ersten fehlgeschlagenen Pruefung.
 * Aufruf: test_r41_granate [schaden|zerfetzt|r15|alle|aus]
 */
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
#include "re2_ems.h"
#include "re2_fx.h"

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
extern int  re15_re2z_gore_resolve(const re15_actor_t *e, const int8_t *bone_parent, int n,
                                   uint8_t *out_draw, uint32_t *out_tint, uint8_t *out_mesh);
extern int  re15_re2z_gore_part_matrix(re15_actor_t *e, int part, uint32_t frame, int32_t rot[9], int32_t trans[3]);
extern void re15_enemy_bone_world_pos(const re15_actor_t *e, int bone, int32_t out[3]);
extern void re15_re2_damage_model_set(int on);

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

/* ---- Raum / Spielschritt (Muster test_r35_granate.c [4]) --------------------------------------------------------- */
static re15_rdt_t         s_rdt;
static uint8_t           *s_rdt_buf = NULL;
static re15_camera_view_t s_cam;
static re15_game_ctx_t    s_ctx;
static uint32_t           s_bild = 0;
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
/* Der Zeichengang von main.c fuer EINEN Gegner, ohne Rasterung: Bruecke (Bit 0 + Kaskade) -> Teilstufe (RE1.5-Teilwort,
 * Scherbenzaehler) -> Part-Matrix (RE2-Flugphysik). Physik und Ablauf haengen im Original am ZEICHNER (FUN_80027434 /
 * FUN_8001e9ec), deshalb muss der Test ihn je Bild nachfahren. Matrix = Einheitsmatrix an der Knochen-Weltlage. */
static void zeichengang(re15_actor_t *e, uint8_t *gezeichnet, int *n_out)
{
    int slot = (int)(e - g_actors);
    const re15_enemy_bank_t *b = re15_enemy_find(e->type);
    int n = (b && b->skel.bone_count > 0) ? b->skel.bone_count : 15;
    if (n > 16) n = 16;
    uint8_t draw[RE15_EMD_MAX_BONES]; uint32_t tint[RE15_EMD_MAX_BONES]; uint8_t mesh[RE15_EMD_MAX_BONES];
    for (int i = 0; i < RE15_EMD_MAX_BONES; i++) { draw[i] = 1; tint[i] = 0x808080u; mesh[i] = (uint8_t)i; }
    int gore_on = re15_re2z_gore_resolve(e, b ? b->skel.bone_parent : NULL, n, draw, tint, mesh);
    re15_teil_t *t = re15_teile_an(slot) ? re15_teile(slot) : NULL;
    for (int i = 0; i < n; i++) {
        gezeichnet[i] = 0;
        if (gore_on && !draw[i]) continue;
        int32_t rot[9] = { 4096, 0, 0, 0, 4096, 0, 0, 0, 4096 }, tr[3];
        re15_enemy_bone_world_pos(e, i, tr);
        if (t) {
            int par = (b && i < b->skel.bone_count) ? (int)b->skel.bone_parent[i] : -1;
            int rc = re15_teil_stufe(slot, i, par, rot, tr, (int32_t)e->dog_floor_y, 1);
            if (rc == 0) continue;
            if (rc == 2) re15_teil_scherben_nach(slot, i, 1);
        }
        if (gore_on) (void)re15_re2z_gore_part_matrix(e, i, s_bild, rot, tr);
        gezeichnet[i] = 1;
    }
    *n_out = n;
}
static void frame(void)
{
    const unsigned char *raw; int len, id;
    re15_msg_tick(&raw, &len, &id);
    s_ctx.pad_current = 0; s_ctx.pad_pressed = 0;
    g_actors[RE15_ACTOR_SLOT_PLAYER].hp = 100;
    re15_game_step(&s_ctx);
    s_bild++;
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
/* Ein Zombie `quelle` aus ROOM1140 (0x10 / 0x11 / 0x16), wahlweise umgetypt auf `typ` (gleiches Gehirn, gleiche
 * RE2-Slots; Bank des Zieltyps wird geladen), alle anderen Gegner aus. */
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
    re15_ai_set_state_word(e, (fl == RE15_AI_FLAVOR_RE2) ? 0x101 : 0x101);
    pl->x = e->x - 6000; pl->z = e->z; pl->y = e->y;
    for (int f = 0; f < 10; f++) frame();
    e->hit_react = 0;
    return e;
}

/* ================================================================================================
 * [1] SCHADEN
 * ================================================================================================ */
static re15_actor_t *dummy(int slot, uint8_t typ, int16_t hp)
{
    re15_actor_t *e = &g_actors[slot];
    memset(e, 0, sizeof *e);
    e->active = 1; e->type = typ; e->hp = hp; e->state = 1; e->hit_react = 0;
    e->x = 0; e->y = 0; e->z = 0; e->rot_y = 0; e->em_flag_id = 0xFF;
    re15_enemy_apply_hitbox(e, typ);
    return e;
}
static int schaden_an(re15_ai_flavor_t fl, uint8_t typ, int16_t hp, uint8_t art, int16_t *hp_nach, uint8_t *st_nach)
{
    re15_ai_flavor_set(fl);
    re15_actor_init(); re15_enemy_reset();
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100; pl->x = 30000; pl->y = 0; pl->z = 30000;
    re15_player_apply_hitbox(pl);
    g_room_rdt_ok = 0; g_current_room_id = 0x1140;
    re15_damage_seed_rng(0x1234u);
    re15_actor_t *e = dummy(1, typ, hp);
    const int32_t p[3] = { 300, -500, 0 };                      /* Liegestelle 300 daneben, P.y = -500 @0x800185a8 */
    int n = re15_granate_r35_explosion(p, 0, art);
    *hp_nach = e->hp; *st_nach = e->state;
    return n;
}
static void abschnitt_schaden(void)
{
    re15_re2_damage_model_set(1);
    static const struct { uint8_t typ; int16_t hp; const char *name; } zk[] = {
        { 0x10, 143, "Zombie 0x10 (RE2-HP max 128+15 @0x8010C670/@0x80100810)" },
        { 0x11, 250, "DICKER Zombie 0x11 (Brad, HP 250 @0x801008C8)" },
        { 0x12, 143, "Zombie 0x12" }, { 0x13, 143, "Zombie 0x13" },
        { 0x16, 143, "Zombie 0x16 (eigene RE2-Zeile, vorher 80 @0x800A4348)" },
        { 0x18, 143, "Zombie 0x18" },
    };
    int ok = 0;
    for (unsigned i = 0; i < sizeof zk / sizeof zk[0]; i++) {
        int16_t h; uint8_t st;
        int n = schaden_an(RE15_AI_FLAVOR_RE2, zk[i].typ, zk[i].hp, 2, &h, &st);
        int gut = (n == 1 && h == (int16_t)(zk[i].hp - 1000) && st == 3);
        printf("   RE2-KI %s: Treffer %d HP %d -> %d Zustand %u\n", zk[i].name, n, zk[i].hp, h, st);
        if (gut) ok++;
    }
    PRUEF(1, ok == 6, "Handgranate (Art 2) an der Zombie-Familie unter RE2-Modell: Schaden 1000 (@0x8006f41c), Tod %d/6", ok);
    /* RE1.5-Gehirn mit RE1.5-HP (Import-Option aus waere 1058 fuer 0x18; hier die RE1.5-KI mit Modell): Tod */
    {
        int16_t h; uint8_t st;
        int n = schaden_an(RE15_AI_FLAVOR_RE15, 0x11, 107, 2, &h, &st);
        PRUEF(2, n == 1 && h == 107 - 1000 && st == 3, "RE1.5-KI dicker Zombie 0x11 (RE1.5-HP max 107 @0x8011f254): %d -> %d Zustand %u", 107, h, st);
    }
    /* Saeure/Brand bleiben bei E4 (Zombie Zeile 11/10 K0 = 200 @0x800A41F4/@0x800A41E0) */
    {
        int16_t h3, h4; uint8_t s3, s4;
        schaden_an(RE15_AI_FLAVOR_RE2, 0x11, 250, 3, &h3, &s3);
        schaden_an(RE15_AI_FLAVOR_RE2, 0x11, 250, 4, &h4, &s4);
        PRUEF(3, h3 == 50 && h4 == 50, "Saeure/Brand an 0x11 unveraendert 200 (E4): HP %d / %d (50)", h3, h4);
    }
    /* Nicht-Zombies: unveraendert (RE2-Records Zeile 9 K0 bzw. RE1.5 1000) */
    static const struct { uint8_t typ; int16_t hp; int16_t soll; const char *name; } nz[] = {
        { 0x20, 2000, 300,  "Hund 0x20 (Z9 K0 300 @0x800A44C4)" },
        { 0x21, 2000, 60,   "Kraehe 0x21 (Z9 K0 60 @0x800A4640)" },
        { 0x25, 2000, 60,   "Spinne 0x25 (Z9 K0 60 @0x800A4C30)" },
        { 0x27, 2000, 1000, "Gorilla/Made 0x27 (RE1.5 1000 @0x8006f41c)" },
        { 0x29, 2000, 1000, "Kakerlake 0x29 (RE1.5 1000)" },
        { 0x23, 2000, 1000, "Alligator 0x23 (RE1.5 1000)" },
    };
    int nok = 0;
    for (unsigned i = 0; i < sizeof nz / sizeof nz[0]; i++) {
        int16_t h; uint8_t st;
        int n = schaden_an(RE15_AI_FLAVOR_RE2, nz[i].typ, nz[i].hp, 2, &h, &st);
        int d = nz[i].hp - h;
        printf("   %s: Treffer %d Schaden %d (soll %d)\n", nz[i].name, n, d, nz[i].soll);
        if (n == 1 && d == nz[i].soll) nok++;
    }
    PRUEF(4, nok == 6, "Nicht-Zombies: Granatenschaden unveraendert %d/6", nok);
}

/* ================================================================================================
 * [2] ZERFETZT — RE2-Gehirn
 * ================================================================================================ */
static void abschnitt_zerfetzt(void)
{
    if (room_load(0x1140, "STAGE1") != 0) { PRUEF(20, 0, "ROOM1140 fehlt"); return; }
    static const struct { uint8_t quelle, typ; } fall[] = {
        { 0x10, 0x10 }, { 0x11, 0x11 }, { 0x16, 0x16 }, { 0x10, 0x12 }, { 0x10, 0x13 }, { 0x10, 0x18 },
    };
    const int N = (int)(sizeof fall / sizeof fall[0]);
    int tot = 0, einmal = 0, flug = 0, platzt = 0, weg = 0, sturz = 0, leiche = 0, rest = 0, krieche = 0;
    for (int k = 0; k < N; k++) {
        re15_actor_t *e = zombie_arena(RE15_AI_FLAVOR_RE2, fall[k].quelle, fall[k].typ);
        if (!e) { PRUEF(20, 0, "kein Zombie 0x%02x in ROOM1140", fall[k].quelle); return; }
        int slot = (int)(e - g_actors);
        e->hp = (fall[k].typ == 0x11) ? 250 : 143;
        unsigned z0 = re15_re2z_granate_zerfetzt_zahl();
        re15_damage_seed_rng(0x4100u + (uint32_t)k * 7919u);
        const int32_t p[3] = { e->x + 300, e->y - 500, e->z };
        int n = re15_granate_r35_explosion(p, 0, 2);
        if (n == 1 && e->state == 3 && e->sub_state_1 == 9u) tot++;
        frame();                                                   /* erster DEATH-Takt */
        int hdl = re15_re2z_last_death_handler();
        if (re15_re2z_granate_zerfetzt_zahl() == z0 + 1u) einmal++;
        int f_ok = (e->re2z_part_flags[8] & 0x4Au) == 0x4Au;
        static const int wurf[4] = { 2, 5, 9, 12 };
        for (int i = 0; i < 4; i++) f_ok &= (e->re2z_part_flags[wurf[i]] & 0x1062u) == 0x1062u;
        if (f_ok) flug++;
        re15_teil_t *t = re15_teile(slot);
        static const int pl_[6] = { 3, 4, 6, 7, 10, 13 };
        int p_ok = re15_teile_an(slot) && t;
        for (int i = 0; p_ok && i < 6; i++) p_ok &= (t[pl_[i]].w & 0x4au) == 0x4au;
        if (p_ok) platzt++;
        if (t && t[11].w == 0 && t[14].w == 0) weg++;
        if (hdl == 1) sturz++;
        /* Zeichengang je Bild, bis 150 Bilder (Leiche + Ausflug der Teile) */
        uint8_t gz[16] = { 0 }; int nb = 0;
        int war7 = 0;
        for (int f = 0; f < 150; f++) {
            frame();
            zeichengang(e, gz, &nb);
            if (e->state == 7) war7 = 1;
            if (e->grid_id == 0x81 || (e->re2z_f10e & 1u)) krieche++;
        }
        if (war7) leiche++;
        int r_ok = gz[0] && gz[1];
        int sonst = 0;
        for (int i = 2; i < nb; i++) if (gz[i]) sonst |= (1 << i);
        if (r_ok && !sonst) rest++;
        printf("   Typ 0x%02x (Quelle 0x%02x, Slot %d): Treffer %d HP %d Zeile %u Handler %d Zerfetzung %u Flug %d "
               "Platzen %d Haende %d Leiche %d | gezeichnet Rumpf %d Becken %d sonst 0x%04x (n %d)\n",
               fall[k].typ, fall[k].quelle, slot, n, e->hp, e->sub_state_1, hdl,
               re15_re2z_granate_zerfetzt_zahl() - z0, f_ok, p_ok, t ? (t[11].w == 0 && t[14].w == 0) : -1,
               war7, gz[0], gz[1], (unsigned)sonst, nb);
    }
    PRUEF(21, tot == N, "Explosion toetet jeden Zombie (Zustand 3, Zeile 9) %d/%d", tot, N);
    PRUEF(22, einmal == N, "Zerfetzung genau einmal je Tod %d/%d", einmal, N);
    PRUEF(23, flug == N, "Kopf (0x4A, @0x80108DD4-E3C) + Oberarme/Oberschenkel (0x1062, @0x80108E54 / @0x80105384) fliegen %d/%d", flug, N);
    PRUEF(24, platzt == N, "Unterarme/Unterschenkel/Fuesse platzen (RE1.5 0x4a, FUN_80108d04) %d/%d", platzt, N);
    PRUEF(25, weg == N, "Haende weg (Teilwort 0 @0x801081d0/@0x8010826c) %d/%d", weg, N);
    PRUEF(26, sturz == N, "danach der normale Sturz 0x80108530 (Handler 1) %d/%d", sturz, N);
    PRUEF(27, leiche == N && krieche == 0, "Leiche (Zustand 7) %d/%d, kein Kriecher (%d Bilder)", leiche, N, krieche);
    PRUEF(28, rest == N, "nach dem Ausflug bleiben nur Rumpf + Becken gezeichnet %d/%d", rest, N);
}

/* ================================================================================================
 * [3] R15 — RE1.5-Gehirn
 * ================================================================================================ */
static void abschnitt_r15(void)
{
    if (room_load(0x1140, "STAGE1") != 0) { PRUEF(30, 0, "ROOM1140 fehlt"); return; }
    static const uint8_t typen[2] = { 0x10, 0x11 };
    int ok = 0, leiche = 0, krieche = 0;
    for (int k = 0; k < 2; k++) {
        re15_actor_t *e = zombie_arena(RE15_AI_FLAVOR_RE15, typen[k], typen[k]);
        if (!e) { PRUEF(30, 0, "kein Zombie 0x%02x (RE1.5)", typen[k]); return; }
        int slot = (int)(e - g_actors);
        e->hp = 107;
        const int32_t p[3] = { e->x + 300, e->y - 500, e->z };
        int n = re15_granate_r35_explosion(p, 0, 2);
        frame();
        re15_teil_t *t = re15_teile(slot);
        int g = 14;                                                  /* Gore-Teil 0x10/0x11 (@0x8011f794) */
        int t_ok = t && re15_teile_an(slot) && (t[g].w & 0x4au) == 0x4au
                   && (t[2].w & 0x4au) == 0x4au && (t[3].w & 0x4au) == 0x4au
                   && (t[5].w & 0x4au) == 0x4au && (t[6].w & 0x4au) == 0x4au
                   && t[10].w == 0 && t[13].w == 0;
        int war7 = 0;
        for (int f = 0; f < 150; f++) {
            frame();
            if (e->state == 7) war7 = 1;
            if (e->grid_id == 0x81) krieche++;
        }
        printf("   RE1.5 Typ 0x%02x: Treffer %d Zeile %u Teile ok %d Leiche %d\n", typen[k], n, e->sub_state_1, t_ok, war7);
        if (n == 1 && t_ok) ok++;
        if (war7) leiche++;
    }
    PRUEF(31, ok == 2, "RE1.5-KI: Kopf/Unterschenkel/Fuesse platzen, Haende weg (z_granate) %d/2", ok);
    PRUEF(32, leiche == 2 && krieche == 0, "RE1.5-KI: Leiche %d/2, kein Kriecher-Uebergang (%d)", leiche, krieche);
}

/* ================================================================================================
 * [4] AUS — Gegenprobe RE15_KEIN_80PROZENT_GORE=1 (eigener Prozess, der Schalter wird einmal gelesen)
 * ================================================================================================ */
static void abschnitt_aus(void)
{
    if (room_load(0x1140, "STAGE1") != 0) { PRUEF(40, 0, "ROOM1140 fehlt"); return; }
    re15_actor_t *e = zombie_arena(RE15_AI_FLAVOR_RE2, 0x11, 0x11);
    if (!e) { PRUEF(40, 0, "kein Zombie 0x11"); return; }
    e->hp = 250;
    unsigned z0 = re15_re2z_granate_zerfetzt_zahl();
    const int32_t p[3] = { e->x + 300, e->y - 500, e->z };
    int n = re15_granate_r35_explosion(p, 0, 2);
    frame();
    int hdl = re15_re2z_last_death_handler();
    PRUEF(41, n == 1 && e->hp == 250 - 1000, "Schalter aus: Schaden bleibt 1000 (HP %d)", e->hp);
    PRUEF(42, re15_re2z_granate_zerfetzt_zahl() == z0 && hdl == 5, "Schalter aus: keine Zerfetzung, RE2-Zelle 0x80108BEC (Handler %d)", hdl);
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
    int alle = (strcmp(teil, "alle") == 0);
    size_t n = 0;
    uint8_t *buf = slurp(RE15_ASSET_PSX_DIR "/DATA/CORE00.ESP", &n);
    if (!buf) { fprintf(stderr, "CORE00.ESP fehlt\n"); return 1; }
    if (re15_esp_parse_global(buf, n, &s_core) != 0) { fprintf(stderr, "CORE00.ESP Parse\n"); return 1; }
    re15_esp_set_global_bank(&s_core);
    re15_esp_set_room_bank(NULL);

    if (alle || strstr(teil, "schaden"))  { printf("[1] Schaden\n");            abschnitt_schaden(); }
    if (alle || strstr(teil, "zerfetzt")) { printf("[2] Zerfetzt (RE2-KI)\n");  abschnitt_zerfetzt(); }
    if (alle || strstr(teil, "r15"))      { printf("[3] RE1.5-KI\n");           abschnitt_r15(); }
    if (strcmp(teil, "aus") == 0)         { printf("[4] Schalter aus\n");       abschnitt_aus(); }

    free(buf);
    if (s_fehler) { printf("test_r41_granate: %d Fehler, erste Pruefung %d\n", s_fehler, s_erste); return s_erste; }
    printf("test_r41_granate: ALLE PRUEFUNGEN GRUEN\n");
    return 0;
}
