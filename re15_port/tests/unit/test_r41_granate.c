/*
 * test_r41_granate.c — Runde 41 Spur GR: die Handgranate zerfetzt Zombies (auch den dicken 0x11) und ist stark genug.
 *
 * Dossier: analysis/befunde_runde41/GR_granate.md. Code: re15_damage.c (Art 2 an der Zombie-Familie = 1000
 * @0x8006f41c), enemy_ai_re2_zombie.c re2z_granate_zerfetzen (RE2-Gehirn), zombiegore_r15.c z_granate (RE1.5-Gehirn).
 *
 *   [1] schaden  — Explosion (re15_granate_r35_explosion, Art 2) an der Zombie-Familie 0x10/0x11/0x12/0x13/0x16/0x18
 *                  mit der hoechsten RE2-HP (Brad 0x11 = 250 @0x801008C8, sonst 128 + 15) und der RE1.5-HP: Tod.
 *                  Saeure/Brand (Art 3/4) bleiben bei E4 (200). JEDER Gegner (Zombies, Hund, Kraehe, Spinne,
 *                  RE2-Babyspinne, kleiner Affe, Tyrant, Zellenarm) bekommt unter RE2-KI, RE1.5-KI und MIXED denselben Wert.
 *                  Runde 41 Spur WS (Nutzer 2026-11-07): der Wert ist D_G = RE15_GRANATE_DG = 250 + 1 (vorher 1000 @0x8006f41c).
 *   [6] boss     — je Boss und Form ueber den echten Hitscan, die echte Explosion und den echten Raketen-Applier gemessen:
 *                  Form-HP, Granatenschaden == D_G, Granaten und Raketen bis zum Formende (Spur WS, Pruefblatt Tabelle d;
 *                  die Boss-Regel der Spur GR "Granate + N Schuss" ist ueberholt). Riesenform 699/398/97/-204, G5 400/200/0/-200.
 *   [8] ws       — Spur WS: D_G toetet 0x11 (250 -> -1) und jeden Standard-Typ, Dummy mit 251 HP lebt mit 0; Rakete je
 *                  Port-Typ unter drei KIs = RE2 Zeile 17 K0; Gegenprobe: die Werte des alten Stands sind rot.
 *   [2] zerfetzt — RE2-Gehirn (Standard-KI), Zombies aus ROOM1140 (0x10, 0x11 dick, 0x16) und umgetypt 0x12/0x13/0x18
 *                  (Nachbesserung 1, Video CD2 19,25-21,1 s + Nutzer-Entscheid "Teile bleiben liegen"): die Zerfetzung
 *                  laeuft genau einmal, sieben Stuecke (Rumpf, Becken, Kopf, Oberarme, Oberschenkel) fliegen mit dem Wurf
 *                  0x1062, die Kinder bleiben dran, keine RE1.5-Scherben, Sturz 0x80108530, Leiche; mit Zeichengang-
 *                  Nachbau (Physik am Zeichner): alle Stuecke liegen nach 150 Bildern am Boden, Scheitel 1800..3200,
 *                  Ruhe nach <= 45 Bildern, gestreut, alles weiter gezeichnet, Rauch-Nachlauf 6 Kinder.
 *   [3] r15      — RE1.5-Gehirn (Flavor re15, Import-Option an): z_granate -> dieselben sieben Stuecke, liegen, Leiche,
 *                  kein Kriecher.
 *   [5] hund     — RE2- und RE1.5-Hund (ROOM11D0): 7 Parts @0x80105680 fliegen 0x1062, Farbe 0x101040, liegen.
 *   [4] aus      — eigener Prozess mit RE15_KEIN_80PROZENT_GORE=1: keine Zerfetzung (RE2 0x80108BEC wie vorher), der
 *                  Schaden bleibt D_G (Staerke haengt nicht am Gore-Schalter).
 * Rueckgabe 0 = gruen, sonst die Nummer der ersten fehlgeschlagenen Pruefung.
 *   [7] g3080    — NACHBESSERUNG 3: ROOM3080 gemessen (Szene oder Kampf, Spieler-Kontrolle, Schaden jeder Waffe am 0x36).
 * Aufruf: test_r41_granate [schaden|zerfetzt|r15|hund|boss|g3080|ws|alle|aus]
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
#include "re15_boss_gator.h"
#include "re15_gator_re2_r41.h"
#include "re15_birkin_formen.h"
#include "re15_ems.h"
#include "re15_emd.h"
#include "re15_fade.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Spur WS: nur fuer den Vorher-Lauf (Gegenprobe) gegen die Engine VOR der Aenderung, dort fehlt das Makro aus re15_damage.h. */
#ifndef RE15_GRANATE_DG
#define RE15_GRANATE_DG (250 + 1)
#endif

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
extern int  re15_re2z_gore_active(const re15_actor_t *e);

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
/* Explosion 300 neben dem Gegner. Die Hoehe P.y probiert die Lagen durch, bis der Kandidatentest (Band
 * @0x8004716c-a4, re15_re2_gl_kandidat) den Gegner annimmt: die Kaesten liegen je Typ verschieden hoch (Kraehe unter
 * RE1.5-KI, RE2-Babyspinne, G5 lagen bei P.y = -500 ausserhalb). Geprueft wird der SCHADEN, nicht die Trefferflaeche;
 * ein Fehlversuch aendert nichts am Gegner. Rueckgabe: Zahl der Treffer. */
static int granate_treffer_art(re15_actor_t *e, uint8_t art)
{
    static const int32_t dy[] = { -500, 0, -1000, -1500, -2000, -3000, 500, -250, -750 };
    /* Kastenmitte: 300 neben der Wurzel; G5 +0x94 = -2000 (`addiu v0,zero,-2000` @0x80100534, bei Gier 0 auf x) */
    static const int32_t dxz[][2] = { { 300, 0 }, { -2000, 0 }, { 0, -2000 } };
    for (unsigned j = 0; j < sizeof dxz / sizeof dxz[0]; j++)
        for (unsigned i = 0; i < sizeof dy / sizeof dy[0]; i++) {
            const int32_t p[3] = { e->x + dxz[j][0], e->y + dy[i], e->z + dxz[j][1] };
            int n = re15_granate_r35_explosion(p, 0, art);
            if (n) return n;
        }
    return 0;
}
static int granate_treffer(re15_actor_t *e) { return granate_treffer_art(e, 2); }
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
        int gut = (n == 1 && h == (int16_t)(zk[i].hp - RE15_GRANATE_DG) && st == 3);   /* Spur WS: 251 (vorher 1000) */
        printf("   RE2-KI %s: Treffer %d HP %d -> %d Zustand %u\n", zk[i].name, n, zk[i].hp, h, st);
        if (gut) ok++;
    }
    PRUEF(1, ok == 6, "Handgranate (Art 2) an der Zombie-Familie unter RE2-Modell: Schaden D_G %d (250 @0x801008C8 + 1), Tod %d/6",
          RE15_GRANATE_DG, ok);
    /* RE1.5-Gehirn mit RE1.5-HP (Import-Option aus waere 1058 fuer 0x18; hier die RE1.5-KI mit Modell): Tod */
    {
        int16_t h; uint8_t st;
        int n = schaden_an(RE15_AI_FLAVOR_RE15, 0x11, 107, 2, &h, &st);
        PRUEF(2, n == 1 && h == 107 - RE15_GRANATE_DG && st == 3, "RE1.5-KI dicker Zombie 0x11 (RE1.5-HP max 107 @0x8011f254): %d -> %d "
              "Zustand %u (107 - 251 = -144)", 107, h, st);
    }
    /* Saeure/Brand bleiben bei E4 (Zombie Zeile 11/10 K0 = 200 @0x800A41F4/@0x800A41E0) */
    {
        int16_t h3, h4; uint8_t s3, s4;
        schaden_an(RE15_AI_FLAVOR_RE2, 0x11, 250, 3, &h3, &s3);
        schaden_an(RE15_AI_FLAVOR_RE2, 0x11, 250, 4, &h4, &s4);
        PRUEF(3, h3 == 50 && h4 == 50, "Saeure/Brand an 0x11 unveraendert 200 (E4): HP %d / %d (50)", h3, h4);
    }
    /* NACHBESSERUNG 2 (Nutzer 2026-11-02 "die Granate hat halt einen Schaden, sind die Gegner damit tot, sind sie tot"):
     * JEDER normale Gegner bekommt unter JEDER KI denselben Wert - vorher unter dem RE2-Modell die Werferzeile 9
     * (Hund 300 @0x800A44C4, Kraehe 60 @0x800A4640, Spinne 60 @0x800A4C30). Spur WS: der Wert ist D_G 251 (vorher 1000
     * @0x8006f41c). HP so gross, dass der Schaden sichtbar wird. */
    static const struct { uint8_t typ; const char *name; } nz[] = {
        { 0x10, "Zombie 0x10" }, { 0x11, "dicker Zombie 0x11" }, { 0x16, "Zombie 0x16" },
        { 0x20, "Hund 0x20 (RE2 vorher 300)" }, { 0x21, "Kraehe 0x21 (RE2 vorher 60)" },
        { 0x25, "Spinne 0x25 (RE2 vorher 60)" }, { 0x26, "Babyspinne 0x26 (RE2-Baby, vorher 60)" },
        { 0x29, "kleiner Affe 0x29" }, { 0x2B, "Tyrant 0x2B" }, { 0x1A, "Zellenarm 0x1A" },
    };
    static const struct { re15_ai_flavor_t fl; const char *name; } kis[3] = {
        { RE15_AI_FLAVOR_RE2, "RE2-KI" }, { RE15_AI_FLAVOR_RE15, "RE1.5-KI" }, { RE15_AI_FLAVOR_MIXED, "MIXED" } };
    const unsigned NNZ = sizeof nz / sizeof nz[0];
    int nok = 0;
    for (unsigned k = 0; k < 3; k++) {
        for (unsigned i = 0; i < NNZ; i++) {
            int16_t h; uint8_t st;
            re15_ai_flavor_set(kis[k].fl);
            re15_actor_init(); re15_enemy_reset();
            re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
            pl->active = 1; pl->type = 0; pl->hp = 100; pl->x = 30000; pl->y = 0; pl->z = 30000;
            re15_player_apply_hitbox(pl);
            g_room_rdt_ok = 0; g_current_room_id = 0x1140;
            re15_actor_t *e = dummy(1, nz[i].typ, 2000);
            if (nz[i].typ == 0x26u) e->re2s_baby_spawned = 1;          /* RE2-Baby nach Herkunft (re15_re2spider_baby_owns) */
            int n = granate_treffer(e);
            h = e->hp; st = e->state; (void)st;
            int d = 2000 - h;
            printf("   %s %s: Treffer %d Schaden %d (soll D_G %d)\n", kis[k].name, nz[i].name, n, d, RE15_GRANATE_DG);
            if (n == 1 && d == RE15_GRANATE_DG) nok++;
        }
    }
    PRUEF(4, nok == (int)(3 * NNZ), "Handgranate an allen normalen Gegnern D_G %d unter RE2-KI, RE1.5-KI und MIXED: %d/%u",
          RE15_GRANATE_DG, nok, 3 * NNZ);
}

/* ================================================================================================
 * [6] BOSSE (Nachbesserung 2, Nutzer 2026-11-02 "Gorilla, Handgranate und ein paar Schuss, und ja auch Birkin und
 *     Aligator, wobei die an sich schon staerker sein muessen als der Gorilla"): je Boss und Form gemessen ueber den
 *     ECHTEN Hitscan (re15_player_weapon_fire) und die ECHTE Explosion (re15_granate_r35_explosion, Art 2):
 *     Pistolenschaden (Waffe 3), Schrotflintenschaden (Waffe 8), Form-HP aus dem INIT der KI, Schuesse ohne Granate,
 *     Schuesse nach EINER Granate. Pruefungen: keine Form stirbt an einer Granate, Granate + N Schuesse toeten,
 *     Gorilla N kleiner als Birkin/Alligator N; G5 unveraendert.
 *     NACHBESSERUNG 3 (Nutzer F1/F2/F4): Birkin-HP-Skala x 2 (re15_birkin_formen.h RE15_BIRKIN_HP_*), Bahnsteig-/Riesenform
 *     1000 (G4-Ctor @0x8010067c) ueber den ECHTEN Eintritt re15_bb_tick (G4-Bank wie der Plattform-Haken).
 *     RUNDE 41 SPUR WS (Nutzer 2026-11-07): die Boss-Regel (Rest R, danach N Schuss) und die Pruefungen 90/91 (Gorilla
 *     schwaecher als Birkin, Pistolensummen) ENTFALLEN. Neu je Fall: Granatenschaden == D_G, Zahl der Granaten und der
 *     Raketen (echter Applier, Hitcode 0x30011) bis zum Formende nach Pruefblatt Tabelle d; Gator-Boss 25240 (H3).
 * ================================================================================================ */
extern int     re15_player_weapon_fire(int weapon_id);
extern int16_t re15_atan2_q12(int32_t, int32_t);

static void boss_welt(uint16_t raum)
{
    re15_ai_flavor_set(RE15_AI_FLAVOR_RE2);                          /* Vorgabe-KI; die Bosse sind unter jeder KI RE1.5 */
    re15_actor_init(); re15_enemy_reset(); re15_bf_reset();
    re15_enemy_ai_set_paused(0);
    re15_player_cmd_reset(); re15_player_aim_reset();
    re15_esp_fx_reset();
    g_room_rdt_ok = 0; g_room_change.pending = 0; g_current_room_id = raum;
    re15_damage_seed_rng(0x1234u);
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100; pl->x = 0; pl->y = 0; pl->z = 20000; pl->floor = 0;
    re15_player_apply_hitbox(pl);
}
/* Boss in Slot 1 mit dem INIT seiner KI (Spawn-Wurzelaufruf wie Sce_em_set @0x8004259c bzw. ein KI-Takt). */
static re15_actor_t *boss_neu(uint8_t typ, uint8_t grid)
{
    re15_actor_t *e = &g_actors[1];
    memset(e, 0, sizeof *e);
    e->active = 1; e->type = typ; e->grid_id = grid; e->state = 0; e->em_flag_id = 0xFF;
    e->x = 0; e->y = 0; e->z = 0; e->rot_y = 0;
    re15_enemy_apply_hitbox(e, typ);
    if (typ == 0x27u || typ == 0x30u) re15_enemy_spawn_root(1);
    else if (typ == 0x23u && !re15_gator_boss_active(e)) re15_enemy_ai_run_all(1);   /* RE1.5-Alligator INIT */
    e->hit_react = 0;
    return e;
}
/* Ein Schuss ueber den echten Hitscan: Spieler 2000 vor dem Boss, Blick auf ihn, Gorilla am Boden (Band LEVEL, wie der
 * Stempel des ACTIVE-Schwanzes @0x80117380-e8). Der Treffer-Riegel +0x93 Bit 0 wird vorher frei gemacht (im Spiel loest ihn
 * der Ausgang der Trefferreaktion). Rueckgabe: abgezogene HP, -1 = kein Treffer. */
static int boss_schuss(re15_actor_t *e, int waffe)
{
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->x = e->x - 2000; pl->z = e->z; pl->y = e->y;
    pl->rot_y = (int16_t)(((int)re15_atan2_q12(e->z - pl->z, e->x - pl->x) - 0x400) & 0xfff);
    e->hit_react = (uint8_t)(e->hit_react & ~3u);
    if (e->type == 0x27u) { e->aim_band = 2; e->mag_airborne = 0; }
    int16_t vor = e->hp;
    int r = re15_player_weapon_fire(waffe);
    return (r == (int)(e - g_actors) + 1) ? (int)vor - (int)e->hp : -1;
}
static int boss_granate(re15_actor_t *e)
{
    e->hit_react = (uint8_t)(e->hit_react & ~3u);
    e->re2_gl_sperre = 0; e->re2z_self1d3 = 0;     /* Spur WS (mehrere Granaten je Form): die RE2-Sperre nach dem Treffer (15 Bilder,
                                                    * @0x80047340-44) loest im Spiel der Bildtakt; der G5 ist RE2-Kandidat (Gate 2
                                                    * @0x80047138-40) */
    int16_t vor = e->hp;
    int n = granate_treffer(e);                                         /* P.y = Liegestelle - 500 @0x800185a8 zuerst */
    return n ? (int)vor - (int)e->hp : -1;
}
/* Pistolenschuesse bis zum Ende der Form (hp < ende); Rueckgabe Zahl, -1 = Fehlschuss, 999 = kein Ende nach 400. */
static int boss_schuesse_bis(re15_actor_t *e, int ende)
{
    for (int n = 1; n <= 400; n++) {
        if (boss_schuss(e, 3) < 0) return -1;
        if (e->hp < ende) return n;
    }
    return 999;
}

/* G4-Bank der Bahnsteig-Form wie der Plattform-Haken (platform/pc/main.c, RE2/BAHNSTEIG_G4.EMD + Opfer-Bank von EM30), damit
 * der ECHTE Eintritt von re15_bb_tick laeuft (HP-Zuweisung RE15_BB_FORM_HP). Muster: test_birkin_bahnsteig.c form_bank. */
static uint8_t *s_g4_emd = NULL, *s_em30 = NULL;
static size_t   s_g4_emd_n = 0, s_em30_n = 0;
static void g4_dateien(void)
{
    static int versucht = 0;
    if (versucht) return;
    versucht = 1;
    s_g4_emd = slurp(RE15_ASSET_PSX_DIR "/../RE2/BAHNSTEIG_G4.EMD", &s_g4_emd_n);
    size_t esz = 0, off = 0, len = 0;
    uint8_t *ems = slurp(RE15_ASSET_PSX_DIR "/EMD/CDEMD0.EMS", &esz);
    if (ems && re15_ems_get_entry(ems, esz, re15_ems_index_for_type(0x30), &off, &len) == 0) {
        s_em30 = (uint8_t *)malloc(len); memcpy(s_em30, ems + off, len); s_em30_n = len;
    }
    free(ems);
}
static void g4_haken(uint8_t type)
{
    if (type != 0x30u || !s_g4_emd || !s_em30) return;
    re15_enemy_bank_t *eb = re15_enemy_find(0x30u);
    if (!eb) eb = re15_enemy_alloc(0x30u);
    if (!eb) return;
    { uint8_t typ = eb->type; memset(eb, 0, sizeof *eb); eb->type = typ; }
    if (re2_emd_parse_bank(s_g4_emd, s_g4_emd_n, eb) != 0) { eb->type = 0; return; }
    eb->re2_rig = 1; eb->ok = 1; eb->buf = NULL;
    eb->victim_ok = re15_emd_parse_victim_bank(s_em30, s_em30_n, &eb->skel_victim, &eb->anim_victim) == 0;
}

typedef enum { BF_GORILLA, BF_ALLIGATOR, BF_BIRKIN, BF_FORM1, BF_MUTATION, BF_NACH_MUT, BF_BAHNSTEIG, BF_G5, BF_GATOR } bform_t;
typedef struct { const char *name; uint16_t raum; uint8_t typ, grid; bform_t art; int ende; } bfall_t;
/* Den Boss in seine Form bringen: Form 1 -> Mutation ueber das echte Wurzel-Tor (hp < RE15_BIRKIN_HP_MUT_TOR = `slti 100`
 * @0x8011667c x 2 -> 0x601) und die echte Mutation (HP RE15_BIRKIN_HP_MUTATION = 0x96 @0x80118094 x 2); nach der Mutation bis
 * zu ihrem Ende (sub 6 Phase 2 @0x8011888c). Bahnsteig-Form: am Mutationsende laedt der Haken die G4-Bank, der naechste
 * KI-Takt laeuft durch den Eintritt von re15_bb_tick. */
static re15_actor_t *boss_aufstellen(const bfall_t *f)
{
    boss_welt(f->raum);
    re15_bf_modell_hook(f->art == BF_BAHNSTEIG ? g4_haken : NULL);
    re15_actor_t *e = boss_neu(f->typ, f->grid);
    if (f->art == BF_G5)    { e->hp = 600; }                           /* Ctor @0x801003fc (enemy_ai_boss_g5.c) */
    if (f->art == BF_GATOR) { e->hp = RE15_GR2_HP_INIT; }              /* 25240 @0x80100518-580 (Spur WS H3, gator_re2_r41.c) */
    if (f->art == BF_MUTATION || f->art == BF_NACH_MUT || f->art == BF_BAHNSTEIG) {
        e->state = 1; e->sub_state_1 = 1; e->sub_state_2 = 0; e->sub_state_3 = 0;
        e->hp = RE15_BIRKIN_HP_MUT_TOR - 10;                            /* unter das Tor */
        int n = 0;
        while (n++ < 400) {
            re15_enemy_ai_run_all(1);
            if (f->art == BF_MUTATION && (e->birkin_flags & 0x8)) break;
            if (f->art != BF_MUTATION && (e->grid_id & 0x10) && !(e->birkin_flags & 0x8) && e->sub_state_1 != 6) break;
        }
        if (f->art == BF_BAHNSTEIG) re15_enemy_ai_run_all(1);           /* erster Formen-Takt: Eintritt re15_bb_tick */
        e->hit_react = 0;
        if (f->art != BF_BAHNSTEIG) e->state = 1;
    }
    re15_bf_modell_hook(NULL);
    return e;
}
/* Runde 41 Spur WS: eine Rakete ueber den echten Applier re15_re2_gl_apply (FUN_800470C0-Zwilling) mit dem Hitcode des
 * Gegnerkontakts 0x00030011 (Zeile 17, K0, alle; `lui a3,0x3` @0x8001f7b0 / `ori a3,a3,0x11` @0x8001f7c0) und der Flugbox
 * {-800,0,400,200} (`read 0x80010908`). Der Pruefpunkt probiert Lagen um den Gegner durch, bis der Kandidatentest ihn annimmt
 * (wie granate_treffer_art); ein Fehlversuch aendert nichts am Gegner. Riegel werden vorher frei gemacht (im Spiel loest sie
 * die Trefferreaktion bzw. die Sperre 15 @0x80047340-44). Rueckgabe: Zahl der Treffer. */
#define WS_HIT_RAKETE 0x00030011u
static int rakete_treffer(re15_actor_t *e)
{
    static const int16_t box[4] = { -800, 0, 400, 200 };            /* @0x80010908 */
    static const int32_t dy[] = { -500, 0, -1000, -1500, -2000, -3000, 500, -250, -750 };
    static const int32_t dxz[][2] = { { 300, 0 }, { -300, 0 }, { 0, 300 }, { 0, -300 }, { -2000, 0 }, { 0, -2000 }, { 0, 0 } };
    for (unsigned j = 0; j < sizeof dxz / sizeof dxz[0]; j++)
        for (unsigned i = 0; i < sizeof dy / sizeof dy[0]; i++) {
            e->hit_react = (uint8_t)(e->hit_react & ~3u);
            e->re2_gl_sperre = 0; e->re2z_self1d3 = 0;
            const int32_t p[3] = { e->x + dxz[j][0], e->y + dy[i], e->z + dxz[j][1] };
            int n = re15_re2_gl_apply(p, 0, box, WS_HIT_RAKETE);
            if (n) return 1;
        }
    return 0;
}
static int boss_rakete(re15_actor_t *e)
{
    int16_t vor = e->hp;
    return rakete_treffer(e) ? (int)vor - (int)e->hp : -1;
}
/* Treffer (Granate bzw. Rakete) bis zum Ende der Form (hp < ende bzw. Zustand 3). Waehrend der Mutation laeuft nach jedem
 * Treffer ein KI-Takt (Revive-Waechter @0x8011a43c). Rueckgabe Zahl, -1 = kein Treffer, 999 = kein Ende nach 12. Die HP nach
 * jedem Treffer landen in hp_folge[0..11]. */
static int boss_treffer_bis(re15_actor_t *e, const bfall_t *f, int rakete, int *hp_folge)
{
    for (int n = 1; n <= 12; n++) {
        int d = rakete ? boss_rakete(e) : boss_granate(e);
        if (d < 0) return -1;
        if (f->art == BF_MUTATION) { re15_enemy_ai_run_all(1); if (e->state == 3 || e->hp <= 0) return n; e->state = 1; }
        if (hp_folge) hp_folge[n - 1] = e->hp;
        if (f->art != BF_MUTATION && (e->state == 3 || e->hp < f->ende)) return n;
        if (f->art != BF_BAHNSTEIG) e->state = 1;
    }
    return 999;
}
static void abschnitt_boss(void)
{
    g4_dateien();
    /* Soll (Pruefblatt WS_ENTSCHEIDUNG.md Tabelle d, Gator-Boss nach H3 mit 25240):
     *   granaten = ceil-Rechnung mit D_G 251 und der Todesregel hp < 0 bzw. dem Formende hp < ende
     *   raketen  = dasselbe mit dem RE2-Wert Zeile 17 K0 (re15_rakete_schaden) */
    static bfall_t fall[] = {
        { "Gorilla 0x27 (1141)",                         0x1141, 0x27, 0x10, BF_GORILLA,   0 },
        { "Alligator 0x23 RE1.5-KI (ausserhalb 2090)",   0x2080, 0x23, 0x00, BF_ALLIGATOR, 0 },
        { "Birkin 0x30 Mensch 3070 (grid 0x10)",         0x3070, 0x30, 0x10, BF_BIRKIN,    0 },
        { "Birkin 0x30 5080 (grid 0x13)",                0x5080, 0x30, 0x13, BF_BIRKIN,    0 },
        { "Birkin 0x30 Hangar/Sichel 50A0 (grid 0x10)",  0x50A0, 0x30, 0x10, BF_BIRKIN,    0 },
        { "Birkin 0x30 Labor 50E0 (grid 0x10)",          0x50E0, 0x30, 0x10, BF_BIRKIN,    0 },
        { "Birkin 0x30 Form 1 3071 (grid 0x01)",         0x3071, 0x30, 0x01, BF_FORM1,    -1 },
        { "Birkin 0x30 Form 1 5090 (grid 0x13)",         0x5090, 0x30, 0x13, BF_FORM1,    -1 },
        { "Birkin 0x30 waehrend der Mutation (3071)",    0x3071, 0x30, 0x01, BF_MUTATION,  0 },
        { "Birkin 0x30 nach der Mutation 3071",          0x3071, 0x30, 0x01, BF_NACH_MUT,  0 },
        { "Bahnsteig-/Riesenform 5090 (G4-Eintritt)",    0x5090, 0x30, 0x13, BF_BAHNSTEIG, 0 },
        { "G5 0x36 5090",                                0x5090, 0x36, 0x13, BF_G5,        0 },
        { "Gator-Boss 0x23 2090 (bis zur Schwaeche)",    0x2090, 0x23, 0x00, BF_GATOR,    -2 },
    };
    /* Soll je Fall: Form-HP, Raketenschaden, Granaten bis zum Formende, Raketen bis zum Formende (999 = nie). */
    static const struct { int hp0, d_rak, n_gr, n_rak; } soll[] = {
        { 360,   200, 2, 2 },     /* Gorilla 180 @0x8011f514 x 2; Rakete Licker @0x800A485C: 360-251=109, -142 / 160, -40 */
        { 300,   110, 2, 3 },     /* RE1.5-Alligator @0x801175dc; Rakete @0x800A4B54: 49, -202 / 190, 80, -30 */
        { 600,   200, 3, 4 },     /* 0x12c @0x8011690c x 2; G1 @0x800A58B0: 349, 98, -153 / 400, 200, 0, -200 */
        { 600,   200, 3, 4 },
        { 600,   200, 3, 4 },
        { 600,   200, 3, 4 },
        { 600,   200, 2, 3 },     /* Form 1 bis hp < 200 (`slti 100` @0x8011667c x 2): 349, 98 / 400, 200, 0 */
        { 600,   200, 2, 3 },
        { 300,   200, 999, 999 }, /* waehrend der Mutation: ein toedlicher Treffer setzt 100 (Revive @0x8011a530 x 2) */
        { 300,   200, 2, 2 },     /* nach der Mutation 0x96 @0x80118094 x 2: 49, -202 / 100, -100 */
        { 1000,  301, 4, 4 },     /* G4-Ctor @0x80100678; G4 @0x800A5D24: 749, 498, 247, -4 / 699, 398, 97, -204 */
        { 600,   200, 3, 4 },     /* G5-Ctor @0x801003fc; G5 @0x800A601C: 349, 98, -153 / 400, 200, 0, -200 */
        { 25240, 110, 1, 3 },     /* H3: 25000 + 240 (@0x801045ec, Bit 0x40 @0x8002b4ac); Schwaeche < 25000 @0x80101ec4:
                                   * 24989 / 25130, 25020, 24910 */
    };
    const unsigned NF = sizeof fall / sizeof fall[0];
    for (unsigned i = 0; i < NF; i++) {
        if (fall[i].ende == -1) fall[i].ende = RE15_BIRKIN_HP_MUT_TOR;  /* `slti v0,v0,100` @0x8011667c x 2 */
        if (fall[i].ende == -2) fall[i].ende = RE15_GR2_HP_SOCKEL;      /* `slti v0,v0,25000` @0x80101ec4 */
    }
    printf("   %-44s | Form-HP | Ende  | Pistole | Granate | Rakete | Granaten bis Ende (HP-Folge)      | Raketen bis Ende (HP-Folge)\n", "Boss");
    for (unsigned i = 0; i < NF; i++) {
        const bfall_t *f = &fall[i];
        re15_actor_t *e = boss_aufstellen(f);
        int hp0 = e->hp;
        int dp = boss_schuss(e, 3);
        e = boss_aufstellen(f);
        int g = boss_granate(e);
        e = boss_aufstellen(f);
        int r = boss_rakete(e);
        int fg[12], fr[12];
        for (int k = 0; k < 12; k++) { fg[k] = fr[k] = -99999; }
        e = boss_aufstellen(f);
        int ng = boss_treffer_bis(e, f, 0, fg);
        e = boss_aufstellen(f);
        int nr = boss_treffer_bis(e, f, 1, fr);
        printf("   %-44s | %7d | %5d | %7d | %7d | %6d | %3d (%d %d %d %d) | %3d (%d %d %d %d)\n", f->name, hp0, f->ende, dp, g, r,
               ng, fg[0], fg[1], fg[2], fg[3], nr, fr[0], fr[1], fr[2], fr[3]);
        PRUEF(70 + (int)i, hp0 == soll[i].hp0 && g == RE15_GRANATE_DG && ng == soll[i].n_gr,
              "%s: Form-HP %d (soll %d), Granate %d (soll D_G %d), %d Granaten bis zum Formende (soll %d)", f->name, hp0,
              soll[i].hp0, g, RE15_GRANATE_DG, ng, soll[i].n_gr);
        PRUEF(110 + (int)i, r == soll[i].d_rak && nr == soll[i].n_rak,
              "%s: Rakete %d (soll %d = RE2 Zeile 17 K0), %d Raketen bis zum Formende (soll %d)", f->name, r, soll[i].d_rak,
              nr, soll[i].n_rak);
        if (f->art == BF_BAHNSTEIG)
            PRUEF(130, fr[0] == 699 && fr[1] == 398 && fr[2] == 97 && fr[3] == -204,
                  "Riesenform: HP nach jeder Rakete %d, %d, %d, %d (soll 699, 398, 97, -204; nach 3 Raketen lebt sie mit 97)",
                  fr[0], fr[1], fr[2], fr[3]);
        if (f->art == BF_G5)
            PRUEF(131, fr[0] == 400 && fr[1] == 200 && fr[2] == 0 && fr[3] == -200,
                  "G5: HP nach jeder Rakete %d, %d, %d, %d (soll 400, 200, 0, -200; mit 0 HP lebt er, `bgez` @0x80047284)",
                  fr[0], fr[1], fr[2], fr[3]);
        if (f->art == BF_GATOR)
            PRUEF(132, fg[0] == RE15_GR2_HP_SOCKEL + 240 - RE15_GRANATE_DG && fg[0] < RE15_GR2_HP_SOCKEL,
                  "Gator-Boss: eine Granate 25240 -> %d < 25000 (Schwaeche @0x80101ec4, H3)", fg[0]);
    }
}

/* ================================================================================================
 * [7] ROOM3080 (Nachbesserung 3, Nutzer F3 "huh, wie jetzt das ist doch ein Boss fight. Warum soll die Granate nicht
 *     wirken?"): MESSUNG, was im Port in 3080 passiert - Raum laden, SCD (main00 + sub00) hochfahren wie
 *     probe_r30_birkin_frost, Spieler auf das Tuerziel (ROOM3070 Door_aot_set @0x03302), Bilder wie der Hauptlauf
 *     (Szenen-Rahmen (1,27)/(2,7) -> player_mode 2). Gemessen: Spawn 0x36 grid 0x24 (sub00 @0x007D4), Szenenmodus nach
 *     Member_set(0x0c,4) @0x00934, ob der Spieler in IRGENDEINEM Bild mit dem 0x36 Kontrolle hat, ob der 0x36 angreift, wann
 *     die Tuer (Aot_on @0x00A22) den Raum verlaesst; dazu am lebenden 0x36 der Schaden JEDER Spielerwaffe (Hitscan, Zeile
 *     @0x8006f360) und jeder Explosionsart 2..9 (DAT_8006f418).
 * ================================================================================================ */
static int s3080_cine_war = 0, s3080_gezeigt = 0;
static void frame3080(uint16_t pad)
{
    const unsigned char *raw; int len, id;
    scd_vm_tick();
    re15_actor_step_all_walkers();
    {
        int cine = re15_game_flag_get(1, 27) || re15_game_flag_get(2, 7);
        re15_letterbox_tick(re15_game_flag_get(1, 27));
        if (cine) { g_scd.player_mode = 2; g_scd.letterbox_countdown = -1; }
        else if (s3080_cine_war) { g_scd.letterbox_countdown = 15; }
        s3080_cine_war = cine;
        if (g_scd.letterbox_countdown > 0 && --g_scd.letterbox_countdown == 0) g_scd.player_mode = 0;
    }
    re15_msg_tick(&raw, &len, &id);
    if (re15_cam_present_tick()) s3080_gezeigt = (int)g_scd.cam_id;
    s_ctx.active_cut = s3080_gezeigt;
    s_ctx.pad_current = pad; s_ctx.pad_pressed = pad;
    re15_game_step(&s_ctx);
    s_bild++;
}
static int lade_re15_bank(uint8_t typ)
{
    size_t n = 0, off = 0, len = 0;
    uint8_t *ems = slurp(RE15_ASSET_PSX_DIR "/EMD/CDEMD0.EMS", &n);
    if (!ems) return 0;
    int idx = re15_ems_index_for_type(typ), ok = 0;
    re15_enemy_bank_t *eb = re15_enemy_find(typ);
    if (!eb) eb = re15_enemy_alloc(typ);
    if (eb && idx >= 0 && re15_ems_get_entry(ems, n, idx, &off, &len) == 0) {
        uint8_t *kopie = (uint8_t *)malloc(len);
        memcpy(kopie, ems + off, len);
        re15_tim_t tim; memset(&tim, 0, sizeof tim);
        if (re15_emd_parse_container(kopie, len, &eb->md1, &eb->skel, &eb->anim, &tim) == 0) { eb->buf = NULL; eb->ok = 1; ok = 1; }
    }
    free(ems);
    return ok;
}
static void start3080(re15_ai_flavor_t fl)
{
    re15_ai_flavor_set(fl);
    memset(&s_cam, 0, sizeof s_cam); memset(&s_ctx, 0, sizeof s_ctx);
    s_ctx.rdt = &s_rdt; s_ctx.rdt_ok = 1; s_ctx.cam_view = &s_cam; s_ctx.active_cut = 0;
    re15_game_state_init();
    re15_actor_init(); re15_aot_init(); scd_vm_init();
    re15_enemy_reset(); re15_enemy_ai_set_paused(0); re15_bf_reset();
    re15_player_cmd_reset(); re15_player_aim_reset(); re15_esp_fx_reset();
    re15_damage_seed_rng(0x0badf00du);
    g_room_rdt = s_rdt; g_room_rdt_ok = 1; g_current_room_id = 0x3080; g_room_change.pending = 0;
    lade_re15_bank(0x36);
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100; pl->hit_react = 0; pl->state = 0; pl->motion = 0; pl->floor = 0;
    pl->x = -25850; pl->y = 0; pl->z = -25900; pl->rot_y = 930;          /* Tuerziel ROOM3070 Door_aot_set @0x03302 */
    re15_player_apply_hitbox(pl);
    re15_collision_set_band(0);
    re15_msg_load_room_block(s_rdt.messages, s_rdt.messages_size);
    scd_register_room_events(&s_rdt);
    scd_room_reenter(&s_rdt, pl->x, pl->z, 0);
    g_scd.cut_auto_enabled = 1;
    s3080_cine_war = 0; s3080_gezeigt = 0;
}
static int finde36(void)
{
    for (int s = 1; s < RE15_ACTOR_MAX; s++) if (g_actors[s].active && g_actors[s].type == 0x36u) return s;
    return -1;
}
/* Schaden am 0x36 ohne Nebenwirkung auf den Ablauf: Zustand sichern, schiessen/sprengen, zurueck. */
static int schaden36(int slot, int waffe, int art)
{
    re15_actor_t sich_e = g_actors[slot], sich_p = g_actors[RE15_ACTOR_SLOT_PLAYER];
    re15_actor_t *e = &g_actors[slot];
    e->hit_react = 0;
    int d;
    if (waffe > 0) d = boss_schuss(e, waffe);
    else { int16_t vor = e->hp; int n = granate_treffer_art(e, (uint8_t)art); d = n ? (int)vor - (int)e->hp : -1; }
    g_actors[slot] = sich_e; g_actors[RE15_ACTOR_SLOT_PLAYER] = sich_p;
    return d;
}
static void abschnitt_3080(void)
{
    if (room_load(0x3080, "STAGE3") != 0) { PRUEF(100, 0, "ROOM3080 fehlt"); return; }
    static const struct { re15_ai_flavor_t fl; const char *name; } kis[2] = {
        { RE15_AI_FLAVOR_RE2, "RE2-KI (Vorgabe)" }, { RE15_AI_FLAVOR_RE15, "RE1.5-KI" } };
    for (int k = 0; k < 2; k++) {
        start3080(kis[k].fl);
        s_bild = 0;
        int slot = -1, spawn_bild = -1, szene_bild = -1, max_phase = -1, grid0 = -1, hp0 = -1, bank = 0;
        int mit36 = 0, kontrolle = 0, angriff = 0, tuer = -1, schaden_bild = -1;
        int dmg_waffe[22], dmg_art[10];
        for (int i = 0; i < 22; i++) dmg_waffe[i] = -9;
        for (int i = 0; i < 10; i++) dmg_art[i] = -9;
        for (int f = 0; f < 3000 && tuer < 0; f++) {
            uint16_t pad = (g_scd.message_active && (f & 7) == 0) ? 0x8000u : 0u;   /* Texte bestaetigen */
            frame3080(pad);
            if (slot < 0) {
                slot = finde36();
                if (slot >= 0) { spawn_bild = (int)s_bild; grid0 = g_actors[slot].grid_id; hp0 = g_actors[slot].hp;
                                 bank = re15_enemy_find(0x36u) != NULL; }
            }
            if (g_room_change.pending) { tuer = (int)s_bild; break; }
            if (slot < 0 || !g_actors[slot].active) continue;
            re15_actor_t *e = &g_actors[slot];
            mit36++;
            if (g_scd.player_mode != 2) kontrolle++;
            if (g_actors[RE15_ACTOR_SLOT_PLAYER].hp < 100) angriff++;
            /* Freigabe: Member_set(0x0c,4) @0x00934 loescht das Frost-Bit 0x20 (grid 0x24 -> 0x04, Wurzel-Tor @0x80116274) */
            if (e->grid_id == 0x04 && szene_bild < 0) szene_bild = (int)s_bild;
            if (szene_bild >= 0 && (int)e->sub_state_2 > max_phase) max_phase = e->sub_state_2;
            if (szene_bild >= 0 && schaden_bild < 0 && (int)s_bild == szene_bild + 30) {
                schaden_bild = (int)s_bild;
                for (int w = 1; w < 22; w++) dmg_waffe[w] = schaden36(slot, w, 0);
                for (int a = 2; a <= 9; a++) dmg_art[a] = schaden36(slot, 0, a);
            }
        }
        printf("   %s: 0x36 Slot %d gespawnt Bild %d grid 0x%02x HP %d (Bank %d); Freigabe grid 0x04 (Szenenmodus Nibble 4) Bild %d, "
               "hoechste Phase %d; %d Bilder mit dem 0x36, davon %d mit Spieler-Kontrolle (player_mode != 2), %d mit "
               "Spieler-HP < 100; Tuer/Raumwechsel Bild %d\n", kis[k].name, slot, spawn_bild, grid0, hp0, bank,
               szene_bild, max_phase, mit36, kontrolle, angriff, tuer);
        printf("   %s: Schaden am 0x36 im Bild %d - Waffen (Hitscan, Zeile @0x8006f360, -1 = kein Treffer):", kis[k].name, schaden_bild);
        for (int w = 1; w < 22; w++) printf(" w%d=%d", w, dmg_waffe[w]);
        printf("\n   %s: Explosionsarten (DAT_8006f418):", kis[k].name);
        for (int a = 2; a <= 9; a++) printf(" art%d=%d", a, dmg_art[a]);
        printf("\n");
        int waffen_null = 1, waffen_treffer = 0;
        for (int w = 1; w < 22; w++) { if (dmg_waffe[w] > 0) waffen_null = 0; if (dmg_waffe[w] >= 0) waffen_treffer++; }
        PRUEF(100 + 3 * k, slot >= 0 && grid0 == 0x24 && szene_bild > 0 && tuer > szene_bild && max_phase == 14,
              "%s: 3080 ist eine Szene - 0x36 grid 0x24 (sub00 @0x007D4), Freigabe Bild %d (Member_set(0x0c,4) @0x00934), "
              "Szene laeuft bis Phase %d (ACT 0x80118d00, [14] @0x8011935c steht), Raum endet ueber die Tuer Bild %d (Aot_on @0x00A22)",
              kis[k].name, szene_bild, max_phase, tuer);
        PRUEF(101 + 3 * k, mit36 > 0 && kontrolle == 0 && angriff == 0,
              "%s: kein Kampf - in %d Bildern mit dem 0x36 hat der Spieler %d-mal Kontrolle (Rahmen (1,27)/(2,7) sub02 "
              "@0x007FE/@0x00802 bis zum Raumwechsel), %d Bilder mit Treffer am Spieler", kis[k].name, mit36, kontrolle, angriff);
        PRUEF(102 + 3 * k, waffen_null && waffen_treffer > 0 && dmg_art[2] == RE15_GRANATE_DG,
              "%s: alle Spielerwaffen 0 Schaden (Zeile 0x36 @0x8006f360 = 22 x 0, %d Waffen trafen), Handgranate D_G %d "
              "(Spur WS, vorher 1000 @0x8006f41c), Werfer-Arten 6..9 = %d/%d/%d/%d (DAT_8006f418; Art 9 = Rakete 200 @0x800A601C)",
              kis[k].name, waffen_treffer, dmg_art[2], dmg_art[6], dmg_art[7], dmg_art[8], dmg_art[9]);
    }
}

/* ================================================================================================
 * [2] ZERFETZT — RE2-Gehirn (Nachbesserung 1: der ganze Zombie zerfliegt, die Stuecke bleiben liegen)
 * ================================================================================================ */
extern unsigned re15_esp_granate_rauch_kinder(void);
static const int k_stueck[7] = { 0, 1, 2, 5, 8, 9, 12 };   /* Rumpf, Becken, Oberschenkel, Kopf, Oberarme */
#define K_STUECK_MASKE 0x1327u
/* Liegen alle geworfenen Stuecke (Maske) nach `bilder` Bildern am Boden? Zeichengang je Bild (Physik haengt am Zeichner).
 * Gemessen: hoechster Scheitel ueber dem Boden, Bilder bis zum Liegen, Streuung (waagrechter Abstand vom Koerper). */
typedef struct { int liegen, scheitel_min, scheitel_max, bilder_max, gestreut, gezeichnet_alle, kaskade_weg, ziel_fehl; } flug_t;
static flug_t flug_messen(re15_actor_t *e, uint16_t maske, int bilder, int n_teile)
{
    flug_t r; memset(&r, 0, sizeof r);
    r.scheitel_min = 1 << 30;
    int32_t hoch[16], h0[16]; int ruh[16];
    for (int p = 0; p < 16; p++) { hoch[p] = 0; ruh[p] = -1; h0[p] = -1; }

    int16_t ziel[16];
    for (int p = 0; p < 16; p++) ziel[p] = e->re2z_part_w9a[p];          /* Zielhoehe beim Wurf abgelegt (+0x9A) */
    uint8_t gz[16] = { 0 }; int nb = 0;
    for (int f = 0; f < bilder; f++) {
        frame();
        re15_esp_fx_tick(NULL);                                          /* ESP-Takt wie fx_plattform_pc.c (Rauch-Nachlauf) */
        zeichengang(e, gz, &nb);
        for (int p = 0; p < 16; p++) {
            if (!((maske >> p) & 1u)) continue;
            if (!(e->re2z_part_seeded & (1u << p))) continue;
            int32_t h = (int32_t)e->y - e->re2z_part_t[p][1];
            if (h0[p] < 0) h0[p] = h;                                    /* Lage beim Einfrieren der Matrix */
            if (h > hoch[p]) hoch[p] = h;
            if (ruh[p] < 0 && e->re2z_part_st86[p] == 1) ruh[p] = f;
        }
    }
    r.liegen = 1; r.gestreut = 0; r.gezeichnet_alle = 1; r.kaskade_weg = 0;
    for (int p = 0; p < 16; p++) {
        if (!((maske >> p) & 1u)) continue;
        if (ruh[p] < 0 || !(e->re2z_part_flags[p] & 1u) || e->re2z_part_t[p][1] != e->y) r.liegen = 0;
        if (hoch[p] < r.scheitel_min) r.scheitel_min = hoch[p];
        {   /* Soll-Scheitel = max(Zielhoehe, Startlage + GR_STEIG_MIN 600); der Wurf faellt diskret um ~v/2 kuerzer */
            int32_t soll = ziel[p]; if (h0[p] + 600 > soll) soll = h0[p] + 600;
            if (hoch[p] < soll - 350 || hoch[p] > soll + 50) r.ziel_fehl |= 1 << p;
        }
        if (hoch[p] > r.scheitel_max) r.scheitel_max = hoch[p];
        if (ruh[p] > r.bilder_max) r.bilder_max = ruh[p];
        int32_t dx = e->re2z_part_t[p][0] - e->x, dz = e->re2z_part_t[p][2] - e->z;
        if (dx * dx + dz * dz > 300 * 300) r.gestreut++;
    }
    for (int i = 0; i < n_teile && i < nb; i++) if (!gz[i]) { r.gezeichnet_alle = 0; r.kaskade_weg |= 1 << i; }
    return r;
}
static void abschnitt_zerfetzt(void)
{
    if (room_load(0x1140, "STAGE1") != 0) { PRUEF(20, 0, "ROOM1140 fehlt"); return; }
    static const struct { uint8_t quelle, typ; } fall[] = {
        { 0x10, 0x10 }, { 0x11, 0x11 }, { 0x16, 0x16 }, { 0x10, 0x12 }, { 0x10, 0x13 }, { 0x10, 0x18 },
    };
    const int N = (int)(sizeof fall / sizeof fall[0]);
    int tot = 0, einmal = 0, wurf = 0, kinder = 0, keine_scherben = 0, sturz = 0, leiche = 0, liegen = 0, hoehe = 0,
        zeit = 0, streu = 0, alle = 0, rauch = 0, krieche = 0;
    for (int k = 0; k < N; k++) {
        re15_actor_t *e = zombie_arena(RE15_AI_FLAVOR_RE2, fall[k].quelle, fall[k].typ);
        if (!e) { PRUEF(20, 0, "kein Zombie 0x%02x in ROOM1140", fall[k].quelle); return; }
        int slot = (int)(e - g_actors);
        e->hp = (fall[k].typ == 0x11) ? 250 : 143;
        unsigned z0 = re15_re2z_granate_zerfetzt_zahl();
        unsigned r0 = re15_esp_granate_rauch_kinder();
        re15_damage_seed_rng(0x4100u + (uint32_t)k * 7919u);
        const int32_t p[3] = { e->x + 300, e->y - 500, e->z };
        int n = re15_granate_r35_explosion(p, 0, 2);
        if (n == 1 && e->state == 3 && e->sub_state_1 == 9u) tot++;
        frame();                                                   /* erster DEATH-Takt */
        int hdl = re15_re2z_last_death_handler();
        if (re15_re2z_granate_zerfetzt_zahl() == z0 + 1u) einmal++;
        int w_ok = (e->re2z_part_gr == K_STUECK_MASKE);
        for (int i = 0; i < 7; i++) w_ok &= (e->re2z_part_flags[k_stueck[i]] & 0x1062u) == 0x1062u;
        if (w_ok) wurf++;
        static const int kind[8] = { 3, 4, 6, 7, 10, 11, 13, 14 };
        int c_ok = 1;
        for (int i = 0; i < 8; i++) c_ok &= (e->re2z_part_flags[kind[i]] == 1u);
        if (c_ok) kinder++;
        re15_teil_t *t = re15_teile(slot);
        int s_ok = 1;                                              /* keine RE1.5-Scherben (Riesendreiecke der 1. Messung) */
        for (int i = 0; t && re15_teile_an(slot) && i < 15; i++) if (t[i].w & 0x08u) s_ok = 0;
        if (s_ok) keine_scherben++;
        if (hdl == 1) sturz++;
        flug_t fl = flug_messen(e, K_STUECK_MASKE, 150, 15);
        if (e->state == 7) leiche++;
        if (e->grid_id == 0x81 || (e->re2z_f10e & 1u)) krieche++;
        if (fl.liegen) liegen++;
        if (fl.ziel_fehl == 0 && fl.scheitel_min >= 1800) hoehe++;
        if (fl.bilder_max <= 45) zeit++;
        if (fl.gestreut >= 5) streu++;
        if (fl.gezeichnet_alle) alle++;
        if (re15_esp_granate_rauch_kinder() - r0 == 6u) rauch++;
        printf("   Typ 0x%02x (Quelle 0x%02x, Slot %d): Treffer %d HP %d Zeile %u Handler %d Zerfetzung %u Maske 0x%04x "
               "Kinder %d Scherben-frei %d | liegen %d Scheitel %d..%d Bilder bis Ruhe %d gestreut %d/7 alle gezeichnet %d "
               "(fehlend 0x%04x) Rauch %u Zustand %d\n",
               fall[k].typ, fall[k].quelle, slot, n, e->hp, e->sub_state_1, hdl, re15_re2z_granate_zerfetzt_zahl() - z0,
               (unsigned)e->re2z_part_gr, c_ok, s_ok, fl.liegen, fl.scheitel_min, fl.scheitel_max, fl.bilder_max,
               fl.gestreut, fl.gezeichnet_alle, (unsigned)fl.kaskade_weg, re15_esp_granate_rauch_kinder() - r0, e->state);
    }
    PRUEF(21, tot == N, "Explosion toetet jeden Zombie (Zustand 3, Zeile 9) %d/%d", tot, N);
    PRUEF(22, einmal == N, "Zerfetzung genau einmal je Tod %d/%d", einmal, N);
    PRUEF(23, wurf == N, "sieben Stuecke (Rumpf 0, Becken 1, Kopf 8, Oberarme 9/12, Oberschenkel 2/5) fliegen mit dem Wurf "
                         "0x1062 (FUN_80028AD8, `ori 0x1062` @0x80108E5C), Liegen-Maske 0x1327 %d/%d", wurf, N);
    PRUEF(24, kinder == N, "Unterarme/Haende/Unterschenkel/Fuesse bleiben am Eltern-Stueck (Flagwort 1) %d/%d", kinder, N);
    PRUEF(25, keine_scherben == N, "keine RE1.5-Scherben (0x08) am Zombie %d/%d", keine_scherben, N);
    PRUEF(26, sturz == N, "danach der normale Sturz 0x80108530 (Handler 1) %d/%d", sturz, N);
    PRUEF(27, leiche == N && krieche == 0, "Leiche (Zustand 7) %d/%d, kein Kriecher (%d)", leiche, N, krieche);
    PRUEF(28, liegen == N, "alle sieben Stuecke liegen nach 150 Bildern AM BODEN (st 1, Bit 0 steht, y = Boden) %d/%d", liegen, N);
    PRUEF(29, hoehe == N, "Scheitel = max(Zielhoehe 2400..2910, Startlage + 600) -350..+50 (GR_DECKE, CD2 19,55-19,69 s), "
                          "mindestens 1800 ueber dem Boden %d/%d", hoehe, N);
    PRUEF(30, zeit == N, "in Ruhe nach hoechstens 45 Bildern (CD2: Landung 20,02 s / letztes Teil 20,62 s) %d/%d", zeit, N);
    PRUEF(31, streu == N, "mindestens 5 von 7 Stuecken > 300 vom Koerper gestreut %d/%d", streu, N);
    PRUEF(32, alle == N, "alle 15 Teile werden weiter gezeichnet (nichts verschwindet, kein heiler Koerper am Ort) %d/%d", alle, N);
    PRUEF(33, rauch == N, "Rauch-Nachlauf: 6 Kinder 0x030B5800 (@0x80018698-a8) je Zombie %d/%d", rauch, N);
}

/* ================================================================================================
 * [3] R15 — RE1.5-Gehirn (Import-Option an = Vorgabe: derselbe RE2-Modellblock)
 * ================================================================================================ */
static void abschnitt_r15(void)
{
    if (room_load(0x1140, "STAGE1") != 0) { PRUEF(40, 0, "ROOM1140 fehlt"); return; }
    static const uint8_t typen[2] = { 0x10, 0x11 };
    int ok = 0, leiche = 0, krieche = 0, liegen = 0;
    for (int k = 0; k < 2; k++) {
        re15_actor_t *e = zombie_arena(RE15_AI_FLAVOR_RE15, typen[k], typen[k]);
        if (!e) { PRUEF(40, 0, "kein Zombie 0x%02x (RE1.5)", typen[k]); return; }
        e->hp = 107;
        const int32_t p[3] = { e->x + 300, e->y - 500, e->z };
        int n = re15_granate_r35_explosion(p, 0, 2);
        frame();
        int w_ok = (e->re2z_part_gr == K_STUECK_MASKE);
        for (int i = 0; i < 7; i++) w_ok &= (e->re2z_part_flags[k_stueck[i]] & 0x1062u) == 0x1062u;
        flug_t fl = flug_messen(e, K_STUECK_MASKE, 150, 15);
        printf("   RE1.5 Typ 0x%02x: Treffer %d Waffe %u Maske 0x%04x liegen %d Scheitel %d..%d Zustand %d grid 0x%02x\n",
               typen[k], n, e->sub_state_1, (unsigned)e->re2z_part_gr, fl.liegen, fl.scheitel_min, fl.scheitel_max,
               e->state, e->grid_id);
        if (n == 1 && w_ok) ok++;
        if (fl.liegen) liegen++;
        if (e->state == 7) leiche++;
        if (e->grid_id == 0x81) krieche++;
    }
    PRUEF(41, ok == 2, "RE1.5-KI (Import an): dieselben sieben Stuecke fliegen (z_granate -> re15_re2z_granate_teile_r15) %d/2", ok);
    PRUEF(42, liegen == 2, "RE1.5-KI: die Stuecke liegen nach 150 Bildern am Boden %d/2", liegen);
    PRUEF(43, leiche == 2 && krieche == 0, "RE1.5-KI: Leiche %d/2, kein Kriecher-Uebergang (%d)", leiche, krieche);
}

/* ================================================================================================
 * [5] HUND — RE2-Gehirn und RE1.5-Gehirn, ROOM11D0 (freier Hunde-Satz Flag 3:152, wie probe_r34_reaktion)
 * ================================================================================================ */
extern unsigned re15_re2d_granate_zahl(void);
static re15_actor_t *hund_arena(re15_ai_flavor_t fl)
{
    re15_ai_flavor_set(fl);
    re15_game_state_init();
    re15_actor_init(); re15_aot_init(); scd_vm_init();
    re15_enemy_reset(); re15_enemy_ai_set_paused(0);
    load_re2_bank(0x20);
    re15_player_cmd_reset(); re15_player_aim_reset();
    re15_esp_fx_reset();
    re15_teile_reset_alle();
    re15_damage_seed_rng(0x0badf00du);
    g_room_rdt_ok = 0; g_room_change.pending = 0;
    { extern void scd_register_current_rdt(const re15_rdt_t *rdt); scd_register_current_rdt(NULL); }
    re15_game_flag_set(3, 152, 1);
    if (s_rdt.main_scd)   scd_thread_start(0, s_rdt.main_scd);
    if (s_rdt.sub_scd[0]) scd_thread_start(1, s_rdt.sub_scd[0]);
    for (int i = 0; i < 120; i++) scd_vm_tick();
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100; pl->hit_react = 0;
    pl->state = 0; pl->motion = 0; pl->floor = 0; pl->y = 0;
    re15_collision_set_band(0);
    re15_player_set_aim_clip_len(12);
    for (int f = 0; f < 30; f++) frame();
    int slot = -1;
    for (int s = 1; s < RE15_ACTOR_MAX; s++)
        if (g_actors[s].active && g_actors[s].type == 0x20) { slot = s; break; }
    if (slot < 0) return NULL;
    for (int s = 1; s < RE15_ACTOR_MAX; s++) if (s != slot) g_actors[s].active = 0;
    re15_actor_t *e = &g_actors[slot];
    if (e->state != 1) re15_ai_set_state_word(e, 0x100);
    e->grid_id = 0; e->re2z_f10e = 0; e->re2z_self1d3 = 0; e->hit_react = 0;
    pl->x = e->x - 8000; pl->z = e->z; pl->y = e->y;
    for (int f = 0; f < 3; f++) frame();
    e->re2z_self1d3 = 0; e->hit_react = 0;
    e->hp = 60;                                                   /* jede Explosivgranate toetet (RE2 300 / RE1.5 1000) */
    return e;
}
static void abschnitt_hund(void)
{
    if (room_load(0x11D0, "STAGE1") != 0) { PRUEF(50, 0, "ROOM11D0 fehlt"); return; }
    static const re15_ai_flavor_t fls[2] = { RE15_AI_FLAVOR_RE2, RE15_AI_FLAVOR_RE15 };
    static const char *namen[2] = { "RE2-KI", "RE1.5-KI" };
    int ok = 0, liegen = 0, aktiv = 0;
    for (int k = 0; k < 2; k++) {
        re15_actor_t *e = hund_arena(fls[k]);
        if (!e) { PRUEF(50, 0, "kein Hund 0x20 in ROOM11D0 (%s)", namen[k]); return; }
        unsigned h0 = re15_re2d_granate_zahl();
        const int32_t p[3] = { e->x + 300, e->y - 500, e->z };
        int n = re15_granate_r35_explosion(p, 0, 2);
        frame();
        int w_ok = (e->re2z_part_gr == 0x079Cu) && re15_re2d_granate_zahl() == h0 + 1u;
        static const int teile[7] = { 2, 3, 4, 7, 8, 9, 10 };      /* @0x80105680 */
        for (int i = 0; i < 7; i++) w_ok &= (e->re2z_part_flags[teile[i]] & 0x1062u) == 0x1062u &&
                                           e->re2z_part_tint[teile[i]] == 0x00101040u;
        int g_ok = re15_re2z_gore_active(e);
        flug_t fl = flug_messen(e, 0x079Cu, 150, 17);
        printf("   %s Hund Slot %d: Treffer %d Zustand %d Waffe %u Maske 0x%04x Bruecke %d liegen %d Scheitel %d..%d "
               "Bilder %d gestreut %d/7\n", namen[k], (int)(e - g_actors), n, e->state, e->sub_state_1,
               (unsigned)e->re2z_part_gr, g_ok, fl.liegen, fl.scheitel_min, fl.scheitel_max, fl.bilder_max, fl.gestreut);
        if (n == 1 && w_ok) ok++;
        if (fl.liegen) liegen++;
        if (g_ok) aktiv++;
    }
    PRUEF(51, ok == 2, "Hund zerfetzt unter beiden KIs: 7 Parts {2,3,4,7,8,9,10} (@0x80105680) fliegen 0x1062, Farbe 0x101040 "
                       "(@0x801044c8), einmal je Tod %d/2", ok);
    PRUEF(52, aktiv == 2, "Part-Bruecke aktiv (RE2-KI und RE1.5-KI nach dem Wurf) %d/2", aktiv);
    PRUEF(53, liegen == 2, "die Hund-Teile liegen nach 150 Bildern am Boden %d/2", liegen);
}

/* ================================================================================================
 * [4] AUS — Gegenprobe RE15_KEIN_80PROZENT_GORE=1 (eigener Prozess, der Schalter wird einmal gelesen)
 * ================================================================================================ */
static void abschnitt_aus(void)
{
    if (room_load(0x1140, "STAGE1") != 0) { PRUEF(60, 0, "ROOM1140 fehlt"); return; }
    re15_actor_t *e = zombie_arena(RE15_AI_FLAVOR_RE2, 0x11, 0x11);
    if (!e) { PRUEF(60, 0, "kein Zombie 0x11"); return; }
    e->hp = 250;
    unsigned z0 = re15_re2z_granate_zerfetzt_zahl();
    const int32_t p[3] = { e->x + 300, e->y - 500, e->z };
    int n = re15_granate_r35_explosion(p, 0, 2);
    frame();
    int hdl = re15_re2z_last_death_handler();
    PRUEF(61, n == 1 && e->hp == 250 - RE15_GRANATE_DG, "Schalter aus: Schaden bleibt D_G %d (HP %d, soll 250 - 251 = -1)",
          RE15_GRANATE_DG, e->hp);
    PRUEF(62, re15_re2z_granate_zerfetzt_zahl() == z0 && hdl == 5, "Schalter aus: keine Zerfetzung, RE2-Zelle 0x80108BEC (Handler %d)", hdl);
}

/* ================================================================================================
 * [8] WS — Runde 41 Spur WS (Nutzerentscheid 2026-11-07 "mach wie resident evil 2. Auch bei der Granate. mach sie so stark,
 *     dass sie gerade so die Standard Gegner mit einer Granate toeten kann"; Pruefblatt WS_ENTSCHEIDUNG.md):
 *     (G) Handgranate D_G = 250 + 1: toetet 0x11 (250 -> -1) und jeden Standard-Typ aus seiner hoechsten HP; ein Dummy mit
 *         251 HP lebt danach mit 0 (Zustand 2) = Minimalitaet und Todesregel (`bgez` @0x80013014 / @0x80047284) zugleich.
 *     (R) Rakete je Port-Typ ueber den echten Applier (Hitcode 0x30011) unter RE2-KI, RE1.5-KI und MIXED = RE2 Zeile 17 K0.
 *     Gegenprobe: dieselben Praedikate mit den Werten VOR WS (Granate 1000/Boss-Regel, Rakete 1000, G5 80) muessen rot sein.
 *     Die Sollwerte stehen hier als eigene Tabelle mit Adresse (nicht aus der Engine gelesen), damit der Abschnitt auch gegen
 *     eine Engine OHNE die Aenderung baut (Vorher-Lauf, Muster GR N2.1).
 * ================================================================================================ */
#define WS_DG_SOLL (250 + 1)          /* `addiu v1,zero,250` @0x801008C8 (EMOVL10_S0.BIN) + Todesregel hp < 0 */
/* n Treffer mit Schaden d aus hp0: lebt nach n-1 (hp >= 0 bzw. >= ende), tot nach n. */
static int ws_treffer_bis(int hp0, int d, int ende)
{
    if (d <= 0) return 999;
    int hp = hp0, n = 0;
    while (hp >= ende && n < 999) { hp -= d; n++; }
    return n;
}
static void abschnitt_ws(void)
{
    re15_re2_damage_model_set(1);
    static const struct { re15_ai_flavor_t fl; const char *name; } kis[3] = {
        { RE15_AI_FLAVOR_RE2, "RE2-KI" }, { RE15_AI_FLAVOR_RE15, "RE1.5-KI" }, { RE15_AI_FLAVOR_MIXED, "MIXED" } };

    /* ---- (G) Granate: Standard-Typen aus ihrer hoechsten HP (Pruefblatt 1.3 / Tabelle d) ---- */
    static const struct { uint8_t typ; int16_t hp; const char *name; } st[] = {
        { 0x10, 143, "Zombie 0x10 (128 @0x8010C674 + 15 @0x80100810)" },
        { 0x11, 250, "dicker Zombie 0x11 (250 @0x801008C8)" },
        { 0x12, 143, "Zombie 0x12" }, { 0x13, 143, "Zombie 0x13" }, { 0x16, 143, "Zombie 0x16" }, { 0x18, 143, "Zombie 0x18" },
        { 0x20, 132, "Hund 0x20 (RE2 132 / RE1.5 111)" }, { 0x21, 10, "Kraehe 0x21" },
        { 0x25, 136, "Spinne 0x25 (RE1.5 136 @0x80117634)" }, { 0x26, 1, "RE2-Babyspinne 0x26" },
        { 0x29, 121, "kleiner Affe 0x29 (@0x8011d700)" }, { 0x2B, 126, "Tyrant 0x2B (@0x80118b1a)" },
        { 0x2D, 100, "Ivy 0x2D" },
    };
    const unsigned NST = sizeof st / sizeof st[0];
    int tot = 0;
    for (unsigned k = 0; k < 3; k++)
        for (unsigned i = 0; i < NST; i++) {
            re15_ai_flavor_set(kis[k].fl);
            re15_actor_init(); re15_enemy_reset();
            re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
            pl->active = 1; pl->type = 0; pl->hp = 100; pl->x = 30000; pl->y = 0; pl->z = 30000;
            re15_player_apply_hitbox(pl);
            g_room_rdt_ok = 0; g_current_room_id = 0x1140;
            re15_damage_seed_rng(0x1234u);
            re15_actor_t *e = dummy(1, st[i].typ, st[i].hp);
            if (st[i].typ == 0x26u) e->re2s_baby_spawned = 1;
            int n = granate_treffer(e);
            int gut = (n == 1 && e->hp == st[i].hp - WS_DG_SOLL && e->state == 3);
            if (!gut) printf("   %s %s: Treffer %d HP %d -> %d Zustand %u (soll %d, Zustand 3)\n", kis[k].name, st[i].name, n,
                             st[i].hp, e->hp, e->state, st[i].hp - WS_DG_SOLL);
            tot += gut;
        }
    PRUEF(140, tot == (int)(3 * NST), "eine Granate (D_G %d) toetet jeden Standard-Typ aus seiner hoechsten HP unter RE2-KI, "
          "RE1.5-KI und MIXED: %d/%u", WS_DG_SOLL, tot, 3 * NST);
    {
        int16_t h; uint8_t s;
        schaden_an(RE15_AI_FLAVOR_RE2, 0x11, 250, 2, &h, &s);
        PRUEF(141, h == -1 && s == 3, "0x11 mit 250 HP: eine Granate -> HP %d Zustand %u (soll -1, 3: 'gerade so')", h, s);
        schaden_an(RE15_AI_FLAVOR_RE2, 0x11, WS_DG_SOLL, 2, &h, &s);
        PRUEF(142, h == 0 && s == 2, "Dummy mit D_G = 251 HP: eine Granate -> HP %d Zustand %u (soll 0, 2: lebt mit 0 HP, "
              "`bgez` @0x80013014 - kleinerer Wert als 251 toetete den 0x11 nicht)", h, s);
        PRUEF(143, RE15_GRANATE_DG == WS_DG_SOLL, "RE15_GRANATE_DG = %d (soll 250 + 1)", RE15_GRANATE_DG);
    }

    /* ---- (R) Rakete je Port-Typ (Pruefblatt Tabelle b), Dummy-HP 2000 ---- */
    static const struct { uint8_t typ; uint16_t raum; int soll; const char *name; } rk[] = {
        { 0x10, 0x1140, 900, "Zombie 0x10 (@0x800A426C)" }, { 0x11, 0x1140, 900, "Zombie 0x11" },
        { 0x12, 0x1140, 900, "Zombie 0x12" }, { 0x13, 0x1140, 900, "Zombie 0x13" },
        { 0x16, 0x1140, 900, "Zombie 0x16 (@0x800A43E8)" }, { 0x18, 0x1140, 900, "Zombie 0x18" },
        { 0x1A, 0x1210, 40, "Gitterhaende 0x1A (RE2 0x2D @0x800A52C0)" },
        { 0x20, 0x1140, 300, "Hund 0x20 (@0x800A4564)" }, { 0x21, 0x1140, 300, "Kraehe 0x21 (@0x800A46E0)" },
        { 0x23, 0x2080, 110, "Alligator 0x23 (@0x800A4B54)" },
        { 0x25, 0x1140, 200, "Spinne 0x25 (@0x800A4CD0)" }, { 0x26, 0x1140, 200, "Babyspinne 0x26 (@0x800A4CD0)" },
        { 0x27, 0x1141, 200, "Gorilla 0x27 (Licker @0x800A485C)" }, { 0x29, 0x1140, 300, "kleiner Affe 0x29 (@0x800A5144)" },
        { 0x2B, 0x1140, 500, "Tyrant 0x2B (@0x800A55B8)" }, { 0x2D, 0x1140, 300, "Ivy 0x2D (RE2 0x2E @0x800A5734)" },
        { 0x30, 0x3070, 200, "Birkin 0x30 (G1 @0x800A58B0)" }, { 0x36, 0x5090, 200, "G5 0x36 5090 (@0x800A601C)" },
        { 0x36, 0x3080, 200, "0x36 ausserhalb 5090" }, { 0x14, 0x1140, 900, "Vorgabe 0x14 (Zeiger @0x800A6AD8 -> 0x800A412C)" },
    };
    const unsigned NRK = sizeof rk / sizeof rk[0];
    int rok = 0;
    for (unsigned k = 0; k < 3; k++)
        for (unsigned i = 0; i < NRK; i++) {
            re15_ai_flavor_set(kis[k].fl);
            re15_actor_init(); re15_enemy_reset(); re15_bf_reset();
            re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
            pl->active = 1; pl->type = 0; pl->hp = 100; pl->x = 30000; pl->y = 0; pl->z = 30000;
            re15_player_apply_hitbox(pl);
            g_room_rdt_ok = 0; g_current_room_id = rk[i].raum;
            re15_damage_seed_rng(0x1234u);
            re15_actor_t *e = dummy(1, rk[i].typ, 2000);
            if (rk[i].typ == 0x26u) e->re2s_baby_spawned = 1;
            if (rk[i].typ == 0x30u) e->grid_id = 0x10;                 /* Form-2-Bit vom Spawn (3070): 600-Form, nicht Riesenform */
            int n = rakete_treffer(e);
            int d = 2000 - e->hp;
            int gut = (n == 1 && d == rk[i].soll);
            printf("   %s %s: Treffer %d Schaden %d (soll %d)%s\n", kis[k].name, rk[i].name, n, d, rk[i].soll, gut ? "" : "  <- FALSCH");
            rok += gut;
        }
    PRUEF(144, rok == (int)(3 * NRK), "Rakete je Port-Typ = RE2 Zeile 17 K0 unter RE2-KI, RE1.5-KI und MIXED: %d/%u", rok, 3 * NRK);

    /* ---- Gegenprobe: die Praedikate muessen den alten Stand verwerfen ---- */
    {
        int rot = 0;
        rot += ws_treffer_bis(1000, 1000, 0) != 4;   /* Riesenform alt: 1000 -> 0 lebt, 2. Rakete toetet (statt 4) */
        rot += ws_treffer_bis(600, 80, 0) != 3;      /* G5-Granate alt 80 @0x800A5F7C: 8 Granaten (statt 3) */
        rot += ws_treffer_bis(360, 1000, 0) != 2;    /* Gorilla-Rakete alt 1000: 1 Rakete (statt 2) */
        rot += 320 != WS_DG_SOLL;                    /* Gorilla-Granate alt 320 (Boss-Regel) != D_G */
        rot += ws_treffer_bis(25300, 250, 25000) != 1; /* Gator alt: 25300, 250 -> 2 Granaten bis zur Schwaeche (statt 1) */
        rot += ws_treffer_bis(600, 1000, 0) != 4;    /* Birkin-Rakete alt 1000: 1 Rakete (statt 4) */
        rot += 1000 != WS_DG_SOLL;                   /* Standard-Granate alt 1000 (@0x8006f41c) != D_G */
        rot += ws_treffer_bis(250, 250, 0) != 1;     /* D_G 250 waere zu klein: 0x11 lebt mit 0 */
        PRUEF(145, rot == 8, "Gegenprobe: alle 8 Werte des alten Stands (Riesenform 1000, G5 80, Gorilla 320, 1000 an Standard, "
              "Gorilla-Rakete 1000, Gator 25300/250, Birkin-Rakete 1000) und D_G 250 verfehlen die Sollzahlen: %d/8", rot);
        PRUEF(146, ws_treffer_bis(1000, 301, 0) == 4 && ws_treffer_bis(600, 200, 0) == 4 && ws_treffer_bis(250, 251, 0) == 1 &&
                   ws_treffer_bis(25240, 251, 25000) == 1 && ws_treffer_bis(25240, 110, 25000) == 3,
              "Rechnung neu: Riesenform 4 Raketen, G5 4 Raketen, 0x11 1 Granate, Gator 1 Granate / 3 Raketen bis zur Schwaeche");
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
    if (alle || strstr(teil, "hund"))     { printf("[5] Hund\n");               abschnitt_hund(); }
    if (alle || strstr(teil, "boss"))     { printf("[6] Bosse\n");              abschnitt_boss(); }
    if (alle || strstr(teil, "g3080"))    { printf("[7] ROOM3080\n");           abschnitt_3080(); }
    if (alle || strstr(teil, "ws"))       { printf("[8] Waffenschaden (Spur WS)\n"); abschnitt_ws(); }
    if (strcmp(teil, "aus") == 0)         { printf("[4] Schalter aus\n");       abschnitt_aus(); }

    free(buf);
    if (s_fehler) { printf("test_r41_granate: %d Fehler, erste Pruefung %d\n", s_fehler, s_erste); return s_erste; }
    printf("test_r41_granate: ALLE PRUEFUNGEN GRUEN\n");
    return 0;
}
