/* probe_r29_1d3_doppelabzug.c — MESSSCHIENE Runde 29.
 *
 * FRAGE: Wie oft je Bild zieht der Port +0x1D3 (die RE2-Trefferpause) ab?
 * Das ORIGINAL hat GENAU EIN Dekrement je Bild, im Root-Prolog des jeweiligen
 * Gegner-Overlays — von mir selbst disassembliert:
 *   Zombie EMZ0.BIN        @0x80100484 lbu v1,467(s0) / @0x8010048c andi v0,v1,0x7f /
 *                          @0x80100490 beq / @0x80100494 addiu v0,v1,-1 / @0x80100498 sb
 *   Hund   EMD0G_MOD0.BIN  @0x80100028 lbu v1,467(s0) / @0x80100030 andi v0,v1,0x7f /
 *                          @0x80100034 beq / @0x80100038 addiu v0,v1,-1 / @0x8010003c sb
 *   Kraehe EMOVL21_S0.BIN  @0x80100160 lbu / @0x80100168 andi 0x7f / @0x8010016c beq /
 *                          @0x80100170 addiu -1 / @0x80100174 sb
 *   0x25   EMS25.BIN       @0x801000f4 andi 0x7f / @0x801000fc addiu -1 / @0x80100100 sb
 *   0x26   EMS26.BIN       @0x80100048 andi 0x7f / @0x80100050 addiu -1 / @0x80100054 sb
 * Kandidatenfilter: `lbu v0,467(s0)` @0x80047138 / `bne v0,zero,0x8004740c` @0x80047140.
 * Stempel: `lw v0,4(a1)` @0x80047338 / `srl v0,v0,9` @0x80047340 / `andi v0,v0,0x7f`
 * @0x80047344 / `sb a0,467(s1)` @0x8004734c; Zeile = 0x800A6A88[+0x8] + 0x14*(id-1)
 * (@0x80047218-40). Hund = 0x800A4424, Pistole (Zeile 3/4) => Stun 15.
 *
 * DREI DURCHGAENGE, alle ueber den ECHTEN Weg (re15_game_step + Pad):
 *  (1) RATE: +0x1D3 von aussen auf 40 gesetzt, danach OHNE Schuss weiterticken.
 *      Protokolliert wird das Byte Bild fuer Bild; ausgegeben Delta je Bild und die
 *      Bildzahl bis 0. Erwartung byte-true: 40 Bilder, Delta 1.
 *  (2) SCHUSS: ein echter Pistolentreffer; gemessen der gestempelte Wert und die
 *      Bildzahl bis 0. Soll = Stun-Zeile.
 *  (3) HUND-KETTE: nach dem Schuss die Hurt-Kette Bild fuer Bild (st/s1/s2 + Clip),
 *      damit die Pause mit der Hinfall-/Aufsteh-Animation verglichen werden kann.
 *
 * Reine Messschiene, wenn ohne Argument gestartet. Mit Argument "riegel" wird sie zum
 * Regressionstest: Delta je Bild MUSS 1 sein und das Byte MUSS 0 erreichen.
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

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef RE15_ASSET_PSX_DIR
#define RE15_ASSET_PSX_DIR "shared_assets/PSX"
#endif

extern void    re15_player_aim_reset(void);
extern void    re15_player_set_aim_clip_len(int fc);
extern int     re15_player_aim_ready(void);
extern int16_t re15_atan2_q12(int32_t dz, int32_t dx);
extern void    re15_esp_fx_reset(void);

static re15_rdt_t         s_rdt;
static re15_camera_view_t s_cam;
static re15_game_ctx_t    s_ctx;
static int                s_room_id = 0x1140;
static int                s_fire_sub = -1;
static uint8_t           *s_keep = NULL;

static uint8_t *slurp(const char *p, size_t *n)
{
    FILE *f = fopen(p, "rb"); if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    if (sz <= 0) { fclose(f); return NULL; }
    uint8_t *b = (uint8_t *)malloc((size_t)sz);
    if (b && fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); b = NULL; }
    fclose(f); if (b) *n = (size_t)sz; return b;
}

static void frame(uint16_t cur, uint16_t edge)
{
    const unsigned char *raw; int len, id;
    re15_msg_tick(&raw, &len, &id);
    s_ctx.pad_current = cur; s_ctx.pad_pressed = edge;
    re15_game_step(&s_ctx);
}

static void bringup(void)
{
    re15_actor_init(); re15_aot_init(); scd_vm_init();
    re15_enemy_reset(); re15_enemy_ai_set_paused(0);
    re15_player_cmd_reset(); re15_player_aim_reset();
    re15_esp_fx_reset();
    re15_damage_seed_rng(0x0badf00du);
    g_current_room_id = (uint16_t)s_room_id;
    if (s_rdt.main_scd)   scd_thread_start(0, s_rdt.main_scd);
    if (s_rdt.sub_scd[0]) scd_thread_start(1, s_rdt.sub_scd[0]);
    if (s_fire_sub >= 0 && s_rdt.sub_scd_count > s_fire_sub && s_rdt.sub_scd[s_fire_sub])
        scd_thread_start(2, s_rdt.sub_scd[s_fire_sub]);
    g_scd.work_vars[10] = 0;
    for (int i = 0; i < 120; i++) scd_vm_tick();
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100; pl->hit_react = 0;
    pl->state = 0; pl->motion = 0; pl->floor = 0; pl->y = 0;
    re15_collision_set_band(0);
    re15_player_set_aim_clip_len(12);
}

static int load_room(const char *sub, int room_id, int fire_sub)
{
    char path[600];
    const char *base = getenv("RE15_ASSET_DIR");
    snprintf(path, sizeof path, "%s/%s", (base && *base) ? base : RE15_ASSET_PSX_DIR, sub);
    size_t sz = 0; uint8_t *buf = slurp(path, &sz);
    if (!buf) return 0;
    if (re15_rdt_parse(buf, sz, &s_rdt) != 0) { free(buf); return 0; }
    s_room_id = room_id; s_fire_sub = fire_sub;
    if (s_keep) free(s_keep);
    s_keep = buf;
    return 1;
}

static void track(int slot, int32_t back)
{
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    re15_actor_t *e  = &g_actors[slot];
    int32_t dx = e->x - pl->x, dz = e->z - pl->z;
    int64_t q  = (int64_t)dx*dx + (int64_t)dz*dz;
    double  d  = q > 0 ? __builtin_sqrt((double)q) : 0.0;
    if (d > 1.0) {
        pl->x = e->x - (int32_t)((double)dx / d * back);
        pl->z = e->z - (int32_t)((double)dz / d * back);
    } else { pl->x = e->x - back; pl->z = e->z; }
    pl->y = e->y;
    pl->rot_y = (int16_t)(((int)re15_atan2_q12(e->z - pl->z, e->x - pl->x) - 0x400) & 0x0fff);
}

static uint16_t elev_pad_for(const re15_actor_t *e)
{
    if (e->type == 0x21) {
        if (e->aim_band == 4) return RE15_PAD_BIT_UP;
        if (e->aim_band == 1) return RE15_PAD_BIT_DOWN;
        return 0;
    }
    if (e->grid_id & 0x80) return RE15_PAD_BIT_DOWN;
    return 0;
}

static int find_type(uint8_t type)
{
    for (int s = 1; s < RE15_ACTOR_MAX; s++) {
        if (!g_actors[s].active || g_actors[s].type != type) continue;
        if (type >= 0x10 && type <= 0x18 && (g_actors[s].grid_id & 0x80)) continue;
        return s;
    }
    return -1;
}

static int setup_target(uint8_t type, int weapon, int baby_force)
{
    re15_ai_flavor_set(RE15_AI_FLAVOR_RE2);
    bringup();
    re15_inv_load_briefing();
    re15_player_set_equipped_weapon(weapon);
    {   int es = re15_inv_equipped_slot();
        if (es >= 0 && es < RE15_INV_MAX_SLOTS) g_inv.slots[es].qty = 250; }
    if (baby_force) {
        for (int s = 1; s < RE15_ACTOR_MAX; s++)
            if (g_actors[s].active && g_actors[s].type == 0x26)
                g_actors[s].re2s_baby_spawned = 1;
    }
    for (int f = 0; f < 60; f++) {
        g_actors[RE15_ACTOR_SLOT_PLAYER].hp = 100;
        if (baby_force)
            for (int s = 1; s < RE15_ACTOR_MAX; s++)
                if (g_actors[s].active && g_actors[s].type == 0x26)
                    g_actors[s].re2s_baby_spawned = 1;
        frame(0, 0);
    }
    int slot = find_type(type);
    if (slot < 0) return -1;
    for (int s = 1; s < RE15_ACTOR_MAX; s++) if (s != slot) g_actors[s].active = 0;
    if (type == 0x20 && g_actors[slot].state == 4 && g_actors[slot].grid_id == 0x40)
        g_actors[slot].grid_id = 0x43;
    return slot;
}

static void aim_up(int slot, int32_t back)
{
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    re15_actor_t *e  = &g_actors[slot];
    pl->hp = 100;
    re15_player_cmd_reset(); re15_player_aim_reset(); re15_player_set_aim_clip_len(12);
    track(slot, back);
    for (int f = 0; f < 60 && !re15_player_aim_ready(); f++) {
        pl->hp = 100; track(slot, back);
        frame((uint16_t)(RE15_PAD_BIT_R1 | elev_pad_for(e)), 0);
    }
}


/* ===================== Durchgang 1: ABZUGSRATE ===========================================
 * +0x1D3 von aussen auf START gesetzt (Messaenderung, nicht der Fix), danach laeuft der
 * ECHTE Weg ohne weiteren Schuss. Gemessen: Delta je Bild und Bildzahl bis 0. */
typedef struct {
    const char *tag;
    int  ok;            /* Lauf gueltig (Gegner gefunden, >0 Bilder) */
    int  seen;          /* Abdeckung */
    int  bis_null;      /* Bild, in dem low-7 zuerst 0 war (-1 = nie) */
    int  dmin, dmax;    /* kleinstes / groesstes Delta je Bild (nur fallende Bilder) */
    int  d1, d2, dn;    /* Bilder mit Delta 1 / Delta 2 / Delta >2 */
    int  start;
    unsigned char ende;
} rate_t;

static void pass_rate(rate_t *r, const char *tag, uint8_t type, const char *room, int room_id,
                      int fire_sub, int weapon, int baby_force, int start, int budget, int verbose)
{
    memset(r, 0, sizeof *r); r->tag = tag; r->bis_null = -1; r->dmin = 999; r->start = start;
    if (!load_room(room, room_id, fire_sub)) {
        printf("  [%s] FEHLLAUF: Raum %s fehlt - sagt NICHTS\n", tag, room); return; }
    int slot = setup_target(type, weapon, baby_force);
    if (slot < 0) {
        printf("  [%s] FEHLLAUF: kein Gegner Typ 0x%02X in %04X - sagt NICHTS\n", tag, type, room_id);
        return; }
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    re15_actor_t *e  = &g_actors[slot];
    const int32_t back = (weapon < 3) ? 900 : 2000;
    aim_up(slot, back);
    printf("      [start] slot=%d active=%d type=0x%02X hp=%d grid=0x%02X st=%d/%u/%u\n",
           slot, e->active, e->type, e->hp, (unsigned)e->grid_id,
           e->state, e->sub_state_1, e->sub_state_2);
    e->re2z_self1d3 = (unsigned char)start;
    int prev = start;
    for (int f = 0; f < budget; f++) {
        pl->hp = 100;
        if (e->hp < 400) e->hp = 30000;      /* am Leben halten: gemessen wird die Pause */
        if (!e->active) break;
        track(slot, back);
        frame((uint16_t)(RE15_PAD_BIT_R1 | elev_pad_for(e)), 0);   /* nur zielen, NICHT schiessen */
        r->seen++;
        int now = e->re2z_self1d3 & 0x7f;
        int d   = prev - now;
        if (d > 0) {
            if (d < r->dmin) r->dmin = d;
            if (d > r->dmax) r->dmax = d;
            if (d == 1) r->d1++; else if (d == 2) r->d2++; else r->dn++;
        }
        if (verbose && f < 24)
            printf("      f%-3d 1D3=0x%02X low7=%2d delta=%d  st=%d/%u/%u\n",
                   f, e->re2z_self1d3, now, d, e->state, e->sub_state_1, e->sub_state_2);
        prev = now;
        if (now == 0 && r->bis_null < 0) { r->bis_null = f + 1; break; }
    }
    r->ende = e->re2z_self1d3;
    r->ok = (r->seen > 0);
    if (r->dmin == 999) r->dmin = 0;
    printf("  [%s] Abdeckung %d Bilder | Start low7=%d -> 0 nach %d Bildern | "
           "Delta je Bild: min %d max %d (1x:%d 2x:%d >2:%d) | Endbyte 0x%02X\n",
           tag, r->seen, start, r->bis_null, r->dmin, r->dmax, r->d1, r->d2, r->dn, r->ende);
    if (r->bis_null < 0)
        printf("      => ERGEBNIS: erreichte 0 NICHT in %d Bildern - die Pause friert ein\n", budget);
    else if (r->dmax > 1)
        printf("      => ERGEBNIS: %d Abzuege je Bild - die Pause ist um Faktor %d zu KURZ\n",
               r->dmax, r->dmax);
    else
        printf("      => ERGEBNIS: EIN Abzug je Bild (byte-true zum Root-Prolog)\n");
}

/* ===================== Durchgang 2: ECHTER SCHUSS ======================================== */
typedef struct {
    const char *tag; int ok, stempel, bis_null, seen, soll;
} shot_t;

static void pass_shot(shot_t *r, const char *tag, uint8_t type, const char *room, int room_id,
                      int fire_sub, int weapon, int baby_force, int soll, int budget, int verbose)
{
    memset(r, 0, sizeof *r); r->tag = tag; r->bis_null = -1; r->soll = soll;
    if (!load_room(room, room_id, fire_sub)) {
        printf("  [%s] FEHLLAUF: Raum fehlt - sagt NICHTS\n", tag); return; }
    int slot = setup_target(type, weapon, baby_force);
    if (slot < 0) { printf("  [%s] FEHLLAUF: kein Gegner - sagt NICHTS\n", tag); return; }
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    re15_actor_t *e  = &g_actors[slot];
    const int32_t back = (weapon < 3) ? 900 : 2000;
    aim_up(slot, back);
    e->re2z_self1d3 = 0;
    int getroffen = -1;
    for (int f = 0; f < budget; f++) {
        pl->hp = 100;
        if (e->hp < 400) e->hp = 30000;
        if (!e->active) break;
        track(slot, back);
        int schuss = (getroffen < 0);
        frame((uint16_t)(RE15_PAD_BIT_R1 | elev_pad_for(e) |
                         (schuss ? RE15_PAD_BIT_SQUARE : 0u)),
              (uint16_t)((schuss && (f % 6) == 0) ? RE15_PAD_BIT_SQUARE : 0u));
        r->seen++;
        int low = e->re2z_self1d3 & 0x7f;
        if (getroffen < 0 && low > 0) { getroffen = f; r->stempel = low; }
        else if (getroffen >= 0) {
            if (verbose && (f - getroffen) < 20)
                printf("      +%-2d 1D3=0x%02X low7=%2d  st=%d/%u/%u clip=%d frame=%d\n",
                       f - getroffen, e->re2z_self1d3, low, e->state, e->sub_state_1,
                       e->sub_state_2, e->motion, e->anim_frame);
            if (low == 0) { r->bis_null = f - getroffen; break; }
        }
    }
    r->ok = (getroffen >= 0);
    if (!r->ok) { printf("  [%s] FEHLLAUF: kein Treffer in %d Bildern - sagt NICHTS\n", tag, r->seen); return; }
    printf("  [%s] Abdeckung %d Bilder | Stempel low7=%d (SOLL %d) | 0 erreicht nach %d Bildern "
           "(SOLL %d)\n", tag, r->seen, r->stempel, soll, r->bis_null, soll);
}

/* ===================== Durchgang 3: HUND - Pause gegen die Aufsteh-Kette ================= */
static void pass_dog_chain(int budget, int *pause_out, int *hurt_out)
{
    *pause_out = -1; *hurt_out = -1;
    if (!load_room("STAGE1/ROOM1190.RDT", 0x1190, 13)) { printf("  FEHLLAUF: ROOM1190 fehlt\n"); return; }
    int slot = setup_target(0x20, 3, 0);
    if (slot < 0) { printf("  FEHLLAUF: kein Hund 0x20 in 1190 - sagt NICHTS\n"); return; }
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    re15_actor_t *e  = &g_actors[slot];
    aim_up(slot, 2000);
    e->re2z_self1d3 = 0;
    int getroffen = -1, pause_bis = -1, hurt_bis = -1;
    for (int f = 0; f < budget; f++) {
        pl->hp = 100;
        if (e->hp < 400) e->hp = 30000;
        if (!e->active) break;
        track(slot, 2000);
        int schuss = (getroffen < 0);
        frame((uint16_t)(RE15_PAD_BIT_R1 | (schuss ? RE15_PAD_BIT_SQUARE : 0u)),
              (uint16_t)((schuss && (f % 6) == 0) ? RE15_PAD_BIT_SQUARE : 0u));
        int low = e->re2z_self1d3 & 0x7f;
        if (getroffen < 0) { if (low > 0) { getroffen = f;
                printf("      TREFFER f%d: 1D3=0x%02X st=%d/%u/%u clip=%d\n",
                       f, e->re2z_self1d3, e->state, e->sub_state_1, e->sub_state_2, e->motion); }
            continue; }
        int k = f - getroffen;
        if (k < 70)
            printf("      +%-2d 1D3=0x%02X low7=%2d | st=%d/%u/%u | clip=%2d frame=%2d%s%s\n",
                   k, e->re2z_self1d3, low, e->state, e->sub_state_1, e->sub_state_2,
                   e->motion, e->anim_frame,
                   (low == 0 && pause_bis < 0) ? "   <== PAUSE ENDE" : "",
                   (e->state != 2 && hurt_bis < 0) ? "   <== WIEDER ACTIVE" : "");
        if (low == 0 && pause_bis < 0) pause_bis = k;
        if (e->state != 2 && hurt_bis < 0) hurt_bis = k;
        if (pause_bis >= 0 && hurt_bis >= 0) break;
    }
    *pause_out = pause_bis; *hurt_out = hurt_bis;
    printf("  HUND: Pause endete nach %d Bildern, HURT-Kette endete nach %d Bildern\n",
           pause_bis, hurt_bis);
    if (pause_bis >= 0 && hurt_bis >= 0) {
        if (pause_bis >= hurt_bis)
            printf("      => die Pause reicht BIS/UEBER das Ende der Aufsteh-Kette\n");
        else
            printf("      => der Hund ist %d Bilder lang treffbar, WAEHREND er noch liegt\n",
                   hurt_bis - pause_bis);
    }
}

/* ======================================================================================== */
static const struct { const char *sub; int id; } SPINNE_KAND[] = {
    { "STAGE1/ROOM1090.RDT", 0x1090 }, { "STAGE2/ROOM2000.RDT", 0x2000 },
    { "STAGE2/ROOM2010.RDT", 0x2010 }, { "STAGE2/ROOM2020.RDT", 0x2020 },
    { "STAGE2/ROOM2040.RDT", 0x2040 }, { "STAGE2/ROOM2050.RDT", 0x2050 },
    { "STAGE2/ROOM2060.RDT", 0x2060 }, { "STAGE1/ROOM10D0.RDT", 0x10d0 },
    { "STAGE1/ROOM1260.RDT", 0x1260 }, { "STAGE3/ROOM3010.RDT", 0x3010 } };

int main(int argc, char **argv)
{
    const int riegel = (argc > 1 && strcmp(argv[1], "riegel") == 0);
    setvbuf(stdout, NULL, _IONBF, 0);
    memset(&s_cam, 0, sizeof s_cam); memset(&s_ctx, 0, sizeof s_ctx);
    s_ctx.rdt = &s_rdt; s_ctx.rdt_ok = 1; s_ctx.cam_view = &s_cam; s_ctx.active_cut = 0;

    printf("=== DURCHGANG 1: ABZUGSRATE (+0x1D3 = 40 gesetzt, danach nur zielen) ===\n");
    rate_t rz, rd, rc, r25, r26;
    pass_rate(&rz,  "ZOMBIE 0x10 R1140", 0x10, "STAGE1/ROOM1140.RDT", 0x1140, -1, 3, 0, 40, 200, 1);
    pass_rate(&rd,  "HUND   0x20 R1190", 0x20, "STAGE1/ROOM1190.RDT", 0x1190, 13, 3, 0, 40, 200, 1);
    pass_rate(&rc,  "KRAEHE 0x21 R10C0", 0x21, "STAGE1/ROOM10C0.RDT", 0x10c0, -1, 3, 0, 40, 200, 1);
    pass_rate(&r26, "BABY   0x26 R1090", 0x26, "STAGE1/ROOM1090.RDT", 0x1090, -1, 3, 1, 40, 200, 1);
    const char *sp = NULL; int spid = 0;
    for (unsigned i = 0; i < sizeof SPINNE_KAND / sizeof SPINNE_KAND[0]; i++) {
        if (!load_room(SPINNE_KAND[i].sub, SPINNE_KAND[i].id, -1)) continue;
        if (setup_target(0x25, 3, 0) >= 0) { sp = SPINNE_KAND[i].sub; spid = SPINNE_KAND[i].id; break; }
    }
    if (sp) pass_rate(&r25, "SPINNE 0x25", 0x25, sp, spid, -1, 3, 0, 40, 200, 1);
    else { memset(&r25, 0, sizeof r25); r25.tag = "SPINNE 0x25";
           printf("  [SPINNE 0x25] FEHLLAUF: kein Raum mit lebender 0x25 - sagt NICHTS\n"); }

    printf("\n=== DURCHGANG 2: ECHTER PISTOLENTREFFER (Stun-Zeile 0x800A6A88[typ]) ===\n");
    shot_t sz, sd;
    pass_shot(&sz, "ZOMBIE 0x10 R1140", 0x10, "STAGE1/ROOM1140.RDT", 0x1140, -1, 3, 0, 15, 400, 1);
    pass_shot(&sd, "HUND   0x20 R1190", 0x20, "STAGE1/ROOM1190.RDT", 0x1190, 13, 3, 0, 15, 400, 1);

    printf("\n=== DURCHGANG 3: HUND - Pause gegen die Hinfall-/Aufsteh-Kette ===\n");
    int pause = -1, hurt = -1;
    pass_dog_chain(600, &pause, &hurt);

    if (!riegel) { printf("\nMESSSCHIENE GELAUFEN (kein Riegel angefordert).\n"); return 0; }

    int rot = 0;
    const rate_t *alle[5] = { &rz, &rd, &rc, &r26, &r25 };
    for (int i = 0; i < 5; i++) {
        const rate_t *r = alle[i];
        if (!r->ok) { printf("RIEGEL: [%s] FEHLLAUF - kein Urteil\n", r->tag ? r->tag : "?"); rot = 1; continue; }
        if (r->bis_null < 0) {
            printf("RIEGEL ROT: [%s] +0x1D3 erreichte 0 NICHT (Endbyte 0x%02X)\n", r->tag, r->ende);
            rot = 1; }
        else if (r->dmax != 1) {
            printf("RIEGEL ROT: [%s] Delta je Bild max %d (SOLL 1)\n", r->tag, r->dmax); rot = 1; }
        else if (r->bis_null != r->start) {
            printf("RIEGEL ROT: [%s] 0 nach %d Bildern (SOLL %d)\n", r->tag, r->bis_null, r->start);
            rot = 1; }
        else printf("RIEGEL GRUEN: [%s] %d Bilder, EIN Abzug je Bild, 0 erreicht\n",
                    r->tag, r->bis_null);
    }
    /* GEGEN-RIEGEL gegen Ueberkorrektur: die Pause darf den Sollwert nicht UEBERschreiten. */
    const shot_t *ss[2] = { &sz, &sd };
    for (int i = 0; i < 2; i++) {
        if (!ss[i]->ok) { printf("RIEGEL: [%s] kein Treffer - kein Urteil\n", ss[i]->tag); rot = 1; continue; }
        if (ss[i]->bis_null != ss[i]->soll) {
            printf("RIEGEL ROT: [%s] Pause %d Bilder (SOLL %d)\n",
                   ss[i]->tag, ss[i]->bis_null, ss[i]->soll); rot = 1; }
        else printf("RIEGEL GRUEN: [%s] Pause genau %d Bilder\n", ss[i]->tag, ss[i]->bis_null);
    }
    printf(rot ? "\nRIEGEL: ROT\n" : "\nRIEGEL: GRUEN\n");
    return rot;
}
