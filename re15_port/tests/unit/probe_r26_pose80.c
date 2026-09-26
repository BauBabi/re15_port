/* probe_r26_pose80.c - MESSSCHIENE (2026-09-26, Auftrag "Hunde-Posenriegel: trifft er den
 * Fall des Nutzers, und wen macht er unverwundbar?").
 *
 * FRAGE: Wird +0x1D3 Bit 0x80 (der Pose-Riegel, Original-Gate `lbu v0,467(s0)` @0x80047138 /
 * `bne v0,zero` @0x80047140 UNMASKIERT) auf dem ECHTEN Pfad je Typ wieder 0 - oder bleibt es
 * stehen? Ein Gate auf einen nie geloeschten Riegel sperrt den Gegner fuer immer
 * unverwundbar (Falle aus Runde 13/14).
 *
 * MESSVERFAHREN - drei Durchgaenge, alle ueber game_step + Pad (R1 zielen, SQUARE feuern),
 * KEIN nachgebauter Filter:
 *  (A) NATUR / DAUERFEUER: N Bilder lang zielen+feuern, jedes Bild das GANZE Byte +0x1D3
 *      protokollieren. Ausgegeben werden: Bilder gesehen (Abdeckung), Bilder mit Bit 0x80,
 *      Anzahl 0->1 (Setzer feuerte) und 1->0 (Freigeber feuerte), laengster ununterbrochener
 *      Riegel-Lauf, Endwert.
 *  (B) ZWANGSRIEGEL: Bit 0x80 wird EINMAL von aussen gesetzt (Messaenderung, nicht der Fix),
 *      danach laeuft der echte Pfad OHNE weitere Schuesse. Gemessen: nach wieviel Bildern
 *      der Riegel faellt - oder dass er nie faellt. Das ist die Erreichbarkeits-Probe der
 *      Freigeber.
 *  (C) HUND-SPRUNG vs. HUND-KUGEL: dieselbe Messung getrennt fuer den gewoehnlichen
 *      Pistolentreffer und fuer den Sprung-/Satz-Zustand - das entscheidet, ob der
 *      Nutzerfall ("erst verwundbar, sobald sie wieder stehen") ueberhaupt getroffen wird.
 *
 * Reine Messschiene: bestanden = gelaufen. KEINE Engine-Aenderung, kein Fix.
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

/* ------------------------------------------------------------------ Pass A: Dauerfeuer */
typedef struct {
    int seen;        /* Abdeckung: tatsaechlich getickte Bilder */
    int locked;      /* Bilder mit Bit 0x80 */
    int rise;        /* 0 -> 1 Uebergaenge */
    int fall;        /* 1 -> 0 Uebergaenge */
    int longest;     /* laengster ununterbrochener Riegel-Lauf */
    int tail;        /* Riegel-Lauf am Ende des Fensters (0 = am Ende frei) */
    int hits;        /* Bilder, in denen low-7 neu gestempelt wurde */
    unsigned char last;
} run_t;

static void passA(const char *tag, uint8_t type, const char *room, int room_id, int fire_sub,
                  int weapon, int baby_force, int budget)
{
    run_t r; memset(&r, 0, sizeof r);
    if (!load_room(room, room_id, fire_sub)) { printf("  [%s] FEHLLAUF: Raum %s fehlt - sagt NICHTS\n", tag, room); return; }
    int slot = setup_target(type, weapon, baby_force);
    if (slot < 0) { printf("  [%s] FEHLLAUF: kein Gegner Typ 0x%02X in %04X - sagt NICHTS\n", tag, type, room_id); return; }
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    re15_actor_t *e  = &g_actors[slot];
    const int32_t back = (weapon < 3) ? 900 : 2000;
    aim_up(slot, back);
    e->re2z_self1d3 = 0;
    int prev80 = 0, run = 0, prevlow = 0;
    int first_rise = -1, first_fall_after = -1;
    for (int f = 0; f < budget; f++) {
        pl->hp = 100;
        if (e->hp < 400) e->hp = 30000;      /* am Leben halten: wir messen den Riegel, nicht den Tod */
        if (!e->active) break;
        track(slot, back);
        frame((uint16_t)(RE15_PAD_BIT_R1 | RE15_PAD_BIT_SQUARE | elev_pad_for(e)),
              (uint16_t)((f == 0) ? RE15_PAD_BIT_SQUARE : 0));
        r.seen++;
        unsigned char v = e->re2z_self1d3;
        int now80 = (v & 0x80u) ? 1 : 0;
        int low = v & 0x7f;
        if (low > prevlow) r.hits++;
        prevlow = low;
        if (now80) {
            r.locked++; run++;
            if (run > r.longest) r.longest = run;
            if (!prev80) { r.rise++; if (first_rise < 0) first_rise = f; }
        } else {
            if (prev80) { r.fall++; if (first_rise >= 0 && first_fall_after < 0) first_fall_after = f; }
            run = 0;
        }
        prev80 = now80;
        r.last = v;
    }
    r.tail = run;
    printf("  [%s] Abdeckung %d Bilder, Treffer-Stempel %dx | Bit0x80: %d Bilder, %dx gesetzt, "
           "%dx freigegeben, laengster Lauf %d, Endlauf %d, Endwert 0x%02X\n",
           tag, r.seen, r.hits, r.locked, r.rise, r.fall, r.longest, r.tail, r.last);
    if (r.seen == 0) { printf("      FEHLLAUF (0 Bilder) - sagt NICHTS\n"); return; }
    if (r.rise == 0)
        printf("      => Bit 0x80 wurde in diesem Fenster NIE gesetzt (Riegel spielt hier keine Rolle)\n");
    else if (r.fall == 0)
        printf("      => ⛔ %dx gesetzt, NIE freigegeben - ein Ganz-Byte-Gate sperrt diesen Typ DAUERHAFT\n", r.rise);
    else
        printf("      => gesetzt und freigegeben (erstes Setzen f%d, erste Freigabe f%d)\n",
               first_rise, first_fall_after);
}

/* --------------------------------------------------------- Pass B: Zwangsriegel von aussen */
static void passB(const char *tag, uint8_t type, const char *room, int room_id, int fire_sub,
                  int weapon, int baby_force, int budget)
{
    if (!load_room(room, room_id, fire_sub)) { printf("  [%s] FEHLLAUF: Raum fehlt - sagt NICHTS\n", tag); return; }
    int slot = setup_target(type, weapon, baby_force);
    if (slot < 0) { printf("  [%s] FEHLLAUF: kein Gegner Typ 0x%02X - sagt NICHTS\n", tag, type); return; }
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    re15_actor_t *e  = &g_actors[slot];
    const int32_t back = (weapon < 3) ? 900 : 2000;
    aim_up(slot, back);
    /* MESSAENDERUNG (nicht der Fix): Riegel von aussen setzen, low-7 = 0, damit nur der
     * Pose-Riegel offen ist und das Root-Dekrement nichts zu tun hat. */
    e->re2z_self1d3 = 0x80u;
    int seen = 0, cleared = -1;
    int st0 = e->state, s10 = e->sub_state_1;
    for (int f = 0; f < budget; f++) {
        pl->hp = 100;
        if (e->hp < 400) e->hp = 30000;
        if (!e->active) break;
        track(slot, back);
        frame((uint16_t)(RE15_PAD_BIT_R1 | elev_pad_for(e)), 0);
        seen++;
        if (!(e->re2z_self1d3 & 0x80u)) { cleared = f; break; }
    }
    if (seen == 0) { printf("  [%s] FEHLLAUF (0 Bilder) - sagt NICHTS\n", tag); return; }
    if (cleared >= 0)
        printf("  [%s] Abdeckung %d Bilder (Start st=%d/%d): Riegel fiel bei Bild %d "
               "(st=%d/%d, Byte 0x%02X)\n",
               tag, seen, st0, s10, cleared, e->state, e->sub_state_1, e->re2z_self1d3);
    else
        printf("  [%s] Abdeckung %d Bilder (Start st=%d/%d): ⛔ Riegel fiel NIE "
               "(Ende st=%d/%d, Byte 0x%02X) - Ganz-Byte-Gate = DAUERHAFT unverwundbar\n",
               tag, seen, st0, s10, e->state, e->sub_state_1, e->re2z_self1d3);
}

/* ------------------------------------------------- Pass C: Hund - Kugel vs. Sprung/Satz */
static void passC_dog(int budget)
{
    if (!load_room("STAGE1/ROOM1190.RDT", 0x1190, 13)) { printf("  FEHLLAUF: ROOM1190 fehlt\n"); return; }
    int slot = setup_target(0x20, 3, 0);
    if (slot < 0) { printf("  FEHLLAUF: kein Hund 0x20 in 1190 - sagt NICHTS\n"); return; }
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    re15_actor_t *e  = &g_actors[slot];
    aim_up(slot, 2000);
    e->re2z_self1d3 = 0;
    /* Ein EINZELNER Treffer, danach Bild fuer Bild beobachten. */
    int fired = -1;
    for (int f = 0; f < 240 && fired < 0; f++) {
        pl->hp = 100; if (e->hp < 400) e->hp = 30000;
        track(slot, 2000);
        frame((uint16_t)(RE15_PAD_BIT_R1 | RE15_PAD_BIT_SQUARE),
              (uint16_t)((f == 0) ? RE15_PAD_BIT_SQUARE : 0));
        if (e->re2z_self1d3 & 0x7fu) fired = f;
    }
    if (fired < 0) { printf("  [HUND-KUGEL] FEHLLAUF: kein Treffer im Fenster - sagt NICHTS\n"); return; }
    printf("  [HUND-KUGEL] Treffer bei f%d: Byte 0x%02X (low7=%d bit80=%d), st=%d/%d/%d\n",
           fired, e->re2z_self1d3, e->re2z_self1d3 & 0x7f,
           (e->re2z_self1d3 & 0x80u) ? 1 : 0, e->state, e->sub_state_1, e->sub_state_2);
    int seen = 0, lock = 0, zero_at = -1, lock_at_zero = -1;
    for (int f = 1; f <= budget; f++) {
        pl->hp = 100; if (e->hp < 400) e->hp = 30000;
        track(slot, 2000);
        frame(RE15_PAD_BIT_R1, 0);
        seen++;
        if (e->re2z_self1d3 & 0x80u) lock++;
        if (zero_at < 0 && (e->re2z_self1d3 & 0x7fu) == 0) {
            zero_at = f; lock_at_zero = (e->re2z_self1d3 & 0x80u) ? 1 : 0;
        }
    }
    printf("  [HUND-KUGEL] Abdeckung %d Bilder nach dem Treffer: low7 erreichte 0 bei f%d "
           "(Bit0x80 dort = %d), Bit0x80 stand in %d von %d Bildern, Endbyte 0x%02X "
           "(st=%d/%d/%d)\n",
           seen, zero_at, lock_at_zero, lock, seen, e->re2z_self1d3,
           e->state, e->sub_state_1, e->sub_state_2);
}

/* ---------------------------------------------- Pass D: Zustand erzwingen, dann beobachten */
static void passD_one(const char *tag, int slot, int st, int s1, int s2, int budget)
{
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    re15_actor_t *e  = &g_actors[slot];
    e->active = 1;
    e->state = (uint8_t)st; e->sub_state_1 = (uint8_t)s1; e->sub_state_2 = (uint8_t)s2;
    e->sub_state_3 = 0;
    e->re2z_self1d3 = 0;
    e->hp = 30000;
    int seen = 0, set_at = -1, clr_at = -1, locked = 0;
    for (int f = 0; f < budget; f++) {
        pl->hp = 100;
        if (e->hp < 400) e->hp = 30000;
        if (!e->active) break;
        track(slot, 2000);
        frame(RE15_PAD_BIT_R1, 0);
        seen++;
        int now = (e->re2z_self1d3 & 0x80u) ? 1 : 0;
        if (now) { locked++; if (set_at < 0) set_at = f; }
        else if (set_at >= 0 && clr_at < 0) clr_at = f;
    }
    if (seen == 0) { printf("      %-34s FEHLLAUF (0 Bilder) - sagt NICHTS\n", tag); return; }
    printf("      %-34s Abdeckung %3d Bilder | Bit0x80: gesetzt ab f%-4d %s, %d Bilder "
           "verriegelt, Ende st=%d/%d Byte 0x%02X\n",
           tag, seen, set_at,
           (set_at < 0) ? "(NIE gesetzt)   " : (clr_at >= 0 ? "freigegeben " : "NIE freigegeben"),
           locked, e->state, e->sub_state_1, e->re2z_self1d3);
    if (set_at >= 0 && clr_at >= 0)
        printf("      %-34s   -> Freigabe bei f%d (Dauer %d Bilder)\n", "", clr_at, clr_at - set_at);
}

static void passD(void)
{
    if (load_room("STAGE1/ROOM1190.RDT", 0x1190, 13)) {
        int slot = setup_target(0x20, 3, 0);
        if (slot > 0) {
            aim_up(slot, 2000);
            printf("  HUND 0x20 (ROOM1190):\n");
            passD_one("ACTIVE sub 14 (Sprung @0x80102C28)",  slot, 1, 14, 0, 300);
            passD_one("ACTIVE sub 15 (Satz  @0x80102E58)",   slot, 1, 15, 0, 300);
            passD_one("ACTIVE sub 16 (      @0x801030FC)",   slot, 1, 16, 0, 300);
            passD_one("HURT Zeile 10 (Feuer @0x80103D9C)",   slot, 2, 10, 0, 300);
            passD_one("HURT Zeile 16 (Feuer @0x80103D9C)",   slot, 2, 16, 0, 300);
            passD_one("HURT Zeile  1 (Kugel @0x80103308)",   slot, 2,  1, 0, 300);
            passD_one("HURT Zeile  0 (      @0x8010321C)",   slot, 2,  0, 0, 300);
        } else printf("  HUND: FEHLLAUF, kein 0x20 - sagt NICHTS\n");
    }
    if (load_room("STAGE1/ROOM10C0.RDT", 0x10c0, -1)) {
        int slot = setup_target(0x21, 3, 0);
        if (slot > 0) {
            aim_up(slot, 2000);
            printf("  KRAEHE 0x21 (ROOM10C0):\n");
            passD_one("state 7 (Leichen-Router)",            slot, 7, 0, 0, 300);
            passD_one("state 7 sub 1 (GIB)",                 slot, 7, 1, 0, 300);
            passD_one("state 7 sub 2 (Wand-Splat)",          slot, 7, 2, 0, 300);
            passD_one("state 7 sub 3 (Launch)",              slot, 7, 3, 0, 300);
            passD_one("state 3 (DEATH)",                     slot, 3, 0, 0, 300);
        } else printf("  KRAEHE: FEHLLAUF - sagt NICHTS\n");
    }
    if (load_room("STAGE1/ROOM1090.RDT", 0x1090, -1)) {
        int slot = setup_target(0x26, 3, 1);
        if (slot > 0) {
            aim_up(slot, 2000);
            printf("  BABY 0x26 (ROOM1090):\n");
            passD_one("state 1 sub 0",                       slot, 1, 0, 0, 300);
            passD_one("state 2 (HURT)",                      slot, 2, 0, 0, 300);
            passD_one("state 3 (DEATH)",                     slot, 3, 0, 0, 300);
            passD_one("state 7 (CORPSE)",                    slot, 7, 0, 0, 300);
        } else printf("  BABY: FEHLLAUF - sagt NICHTS\n");
    }
}

int main(void)
{
    memset(&s_cam, 0, sizeof s_cam); memset(&s_ctx, 0, sizeof s_ctx);
    s_ctx.rdt = &s_rdt; s_ctx.rdt_ok = 1; s_ctx.cam_view = &s_cam; s_ctx.active_cut = 0;

    printf("\n############ PASS A - DAUERFEUER, ganzes Byte +0x1D3 beobachtet ############\n");
    passA("HUND   0x20 R1190", 0x20, "STAGE1/ROOM1190.RDT", 0x1190, 13, 3, 0, 900);
    passA("KRAEHE 0x21 R10C0", 0x21, "STAGE1/ROOM10C0.RDT", 0x10c0, -1, 3, 0, 900);
    passA("BABY   0x26 R1090", 0x26, "STAGE1/ROOM1090.RDT", 0x1090, -1, 3, 1, 900);
    passA("FEUER  0x26 R1090 (ohne Baby)", 0x26, "STAGE1/ROOM1090.RDT", 0x1090, -1, 3, 0, 900);
    {   static const struct { const char *sub; int id; } cand[] = {
            { "STAGE1/ROOM1090.RDT", 0x1090 }, { "STAGE2/ROOM2000.RDT", 0x2000 },
            { "STAGE2/ROOM2010.RDT", 0x2010 }, { "STAGE2/ROOM2020.RDT", 0x2020 },
            { "STAGE2/ROOM2040.RDT", 0x2040 }, { "STAGE2/ROOM2050.RDT", 0x2050 },
            { "STAGE2/ROOM2060.RDT", 0x2060 }, { "STAGE1/ROOM10D0.RDT", 0x10d0 },
            { "STAGE1/ROOM1260.RDT", 0x1260 }, { "STAGE3/ROOM3010.RDT", 0x3010 } };
        const char *sp = NULL; int spid = 0;
        for (unsigned i = 0; i < sizeof cand / sizeof cand[0]; i++) {
            if (!load_room(cand[i].sub, cand[i].id, -1)) continue;
            re15_ai_flavor_set(RE15_AI_FLAVOR_RE2); bringup();
            for (int f = 0; f < 30; f++) frame(0, 0);
            if (find_type(0x25) >= 0) { sp = cand[i].sub; spid = cand[i].id; break; }
        }
        if (sp) passA("SPINNE 0x25", 0x25, sp, spid, -1, 3, 0, 900);
        else    printf("  [SPINNE 0x25] FEHLLAUF: kein Raum mit lebender 0x25 - sagt NICHTS\n");
        if (sp) { printf("\n"); }
        printf("\n########### PASS B - ZWANGSRIEGEL: faellt Bit 0x80 auf dem echten Pfad? ####\n");
        passB("HUND   0x20 R1190", 0x20, "STAGE1/ROOM1190.RDT", 0x1190, 13, 3, 0, 600);
        passB("KRAEHE 0x21 R10C0", 0x21, "STAGE1/ROOM10C0.RDT", 0x10c0, -1, 3, 0, 600);
        passB("BABY   0x26 R1090", 0x26, "STAGE1/ROOM1090.RDT", 0x1090, -1, 3, 1, 600);
        passB("FEUER  0x26 R1090 (ohne Baby)", 0x26, "STAGE1/ROOM1090.RDT", 0x1090, -1, 3, 0, 600);
        if (sp) passB("SPINNE 0x25", 0x25, sp, spid, -1, 3, 0, 600);
        else    printf("  [SPINNE 0x25] FEHLLAUF: kein Raum - sagt NICHTS\n");
    }

    printf("\n########### PASS C - HUND: gewoehnlicher Kugeltreffer #####################\n");
    passC_dog(240);

    printf("\n########### PASS D - ZUSTAND ERZWUNGEN: setzt der Port Bit 0x80? ##########\n");
    passD();

    printf("\nPROBE-OK (Messschiene gelaufen)\n");
    return 0;
}
