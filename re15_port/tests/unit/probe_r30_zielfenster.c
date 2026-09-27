/* probe_r30_zielfenster.c — MESSSCHIENE Runde 30 (Welle 1: MESSEN, kein Verhalten geaendert).
 *
 * FRAGE: Das FUENFTE Tor des RE2-Kandidatenfilters FUN_800470C0 ist ein SENKRECHTES
 * ZIELFENSTER. Der Port hat es nicht. Was traegt der Port heute an dessen drei Eingaengen,
 * und wen wuerde das Tor sperren, wenn man es einbaut?
 *
 * DAS TOR, selbst disassembliert (info/re2leon/PSX.EXE, re2_disasm.py dis 0x800470c0):
 *   8004716c  lhu v1,464(s0)      ; +0x1D0
 *   80047170  lh  a0,152(s0)      ; +0x98  b  (SIGNED halfword)
 *   80047174  lw  v0,60(s0)       ; +0x3C  eY (Gegner-Y; +0x24 ist die MATRIX, t[1] = +0x3C)
 *   80047178  andi v1,v1,0xff00
 *   8004717c  addu v0,v0,a0
 *   80047180  addiu v0,v0,100
 *   80047184  sh  v1,464(s0)
 *   80047188  lhu v1,158(s0)      ; +0x9E  h  (UNSIGNED halfword)
 *   8004718c  lw  a0,4(s4)        ; ZIELHOEHE  = s4->vy
 *   80047190  addu v0,v0,v1
 *   80047194  subu v0,v0,a0
 *   80047198  addiu v1,v1,100
 *   8004719c  sll v1,v1,1
 *   800471a0  sltu v0,v0,v1       ; UNSIGNED
 *   800471a4  beq v0,zero,0x8004740c   ; -> Kandidatenschleife (addiu s2,s2,4 @0x8004740c,
 *                                      ;    bne s2,v0,0x8004711c @0x80047418)
 * ALSO:  DURCH  <=>  (uint32)(eY + b + 100 + h - zielY) < (uint32)(2*(h+100))
 * Mit Hgun := eY - zielY (positiv = Ziel LIEGT UEBER dem Gegner, PSX-Y zeigt nach unten):
 *        DURCH  <=>  -(b+h+100) <= Hgun < (h - b + 100)
 *
 * s4 AUFGELOEST (Aufrufstelle @0x80042f94, Funktion @0x80042c64):
 *   80042e60  addiu s0,sp,32      ; MATRIX (PsyQ: 3x3 shorts + 2 Pad = 20 B, dann t[3])
 *   80042e64/74/84/94  jal 0x8002ce94  ; vier Verkettungen ueber die Arm-/Waffen-Bones
 *                                      ; (s2+24, s2+1572, s2+1744, s2+1916; s2 = lw 408(s1))
 *   80042f8c  addiu a0,sp,52      ; a0 = &MATRIX.t[0]  =>  s4+4 = sp+56 = t[1] = MUENDUNGS-Y
 *   80042f60-6c  fuer Waffen-Id (+0x14D)-7 in [0,4]:  t[1] += 200 vor dem Aufruf,
 *   80042fac-b8                    t[1] -= 200 danach.
 * Die Zielhoehe ist also die WELT-Y-Koordinate der MUENDUNG (Bone-Kette der Waffenhand),
 * NICHT die Fusshoehe des Spielers.
 *
 * DIE ORIGINAL-WERTE VON +0x98/+0x9E je Typ (Vollscan aller `sh rt,152/158(rs)` je Overlay):
 *   0x10 Zombie EMZ0.BIN        INIT  -1500/1500 @0x8010095C/64 ; Kriecher -350/350
 *                               @0x80100B14-20, @0x80103460-6C, @0x80106B28-34,
 *                               @0x801077F8-818, @0x80108998-9A4 ; zurueck -1500/1500
 *                               @0x80103710-20, @0x80104A1C-28, @0x80107E8C-98
 *   0x20 Hund   EMD0G_MOD0.BIN  INIT  -1000/1000 @0x8010028C-9C ; STAUCHUNG FUN_80104088:
 *                               a1==0 -> -500/500 @0x80104098-A8, a1!=0 -> -1000/1000
 *                               @0x801040B8-C4
 *   0x21 Kraehe EMOVL21_S0.BIN  INIT  -350/530   @0x801003B8-DC
 *   0x25 Spinne EMS25.BIN       Tabelle @0x801063A0+20/24 -> -1400/1400, danach +0x98 = 0
 *                               (`sh zero,152(s0)` @0x8010276C) => b=0, h=1400
 *   0x26 Baby   EMS26.BIN       INIT  -10/10     @0x80100168-78   (EINZIGER Schreiber)
 *
 * WAS DIESE SONDE MISST (kein Nachbau des Tores, keine Verhaltensaenderung):
 *  (1) FELD-INVENTUR: fuehrt der Port +0x98/+0x9E ueberhaupt? Und welche senkrechte Groesse
 *      benutzt der heutige Trefferpfad statt der Muendungshoehe?
 *  (2) ZIELHOEHE: ist im Port eine Muendungs-Weltposition ueberhaupt verfuegbar
 *      (re15_player_gunbone_world) — auf dem ECHTEN Weg, nicht in der Theorie?
 *  (3) TOR-RECHNUNG: mit dem gemessenen Hgun je Typ — haelt das Tor oder sperrt es?
 *      Zusaetzlich die Schwelle je Typ als Zahl.
 *  (4) HUND-KETTE mit geladener RE2-Bank: Laenge und die Stelle, an der das Original
 *      FUN_80104088(1) ruft (@0x801036F0 in HURT-P2 bei +0x7==2 nach Clip-Ende,
 *      @0x801037C0 in HURT-P3 nach Clip-Ende).
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
#include "re2_ems.h"

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
extern int     re15_player_gunbone_world(int32_t ox, int32_t oy, int32_t oz, int32_t out[3]);

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
    /* GLEICHE EBENE wie in probe_r29: der Spieler wird auf die Bodenhoehe des Gegners
     * gesetzt. OHNE diese Zeile bleibt pl->y auf 0, waehrend die RE2-Gegner je nach Raum
     * auf -530..-5400 stehen — dann misst die Sonde den Teleport-Versatz der Sonde selbst
     * und nicht die Spielgroesse (erster Lauf 2026-09-27 lief genau da hinein). */
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

/* RE2-Bank (CDEMD0.EMS) — OHNE sie haben alle Clips Laenge 0 und jede Animation ist nach
 * EINEM Bild "zu Ende" (Falle aus Runde 29). */
static uint8_t *s_ems2 = NULL; static size_t s_ems2_n = 0;
static re15_enemy_bank_t *load_re2_bank(uint8_t type)
{
    re15_enemy_bank_t *eb = re15_enemy_find(type);
    if (eb && eb->ok) return eb;
    if (!eb) eb = re15_enemy_alloc(type);
    if (!eb) return NULL;
    if (!s_ems2) s_ems2 = slurp(RE15_ASSET_PSX_DIR "/../RE2/CDEMD0.EMS", &s_ems2_n);
    if (s_ems2 && re2_ems_load_bank(s_ems2, s_ems2_n, (int)type, eb, NULL) == 0) {
        eb->buf = NULL; eb->ok = 1; return eb;
    }
    eb->type = 0; return NULL;
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
    load_re2_bank(type);
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

/* ===== Das Tor als reine Rechnung (kein Einbau) ========================================== */
/* DURCH <=> (uint32)(eY + b + 100 + h - zielY) < (uint32)(2*(h+100))   @0x8004717C-A4 */
static int tor(int32_t eY, int b, int h, int32_t zielY)
{
    uint32_t lhs = (uint32_t)(eY + b + 100 + h - zielY);
    uint32_t rhs = (uint32_t)(2 * (h + 100));
    return lhs < rhs;
}

typedef struct { const char *name; uint8_t type; int b, h; const char *quelle; } box_t;
static const box_t BOXEN[] = {
  { "ZOMBIE 0x10 stehend", 0x10, -1500, 1500, "EMZ0 INIT @0x8010095C/64" },
  { "ZOMBIE 0x10 Kriecher",0x10,  -350,  350, "EMZ0 @0x80100B14-20 / @0x80103460-6C" },
  { "HUND   0x20 stehend", 0x20, -1000, 1000, "EMD0G INIT @0x8010028C-9C / F80104088(1) @0x801040B8-C4" },
  { "HUND   0x20 liegend", 0x20,  -500,  500, "FUN_80104088(0) @0x80104098-A8" },
  { "KRAEHE 0x21",         0x21,  -350,  530, "EMOVL21_S0 @0x801003B8-DC" },
  { "SPINNE 0x25",         0x25,     0, 1400, "EMS25 Tab @0x801063A0+20/24, +0x98=0 @0x8010276C" },
  { "BABY   0x26",         0x26,   -10,   10, "EMS26 @0x80100168-78 (einziger Schreiber)" },
};

/* ===== Durchgang 1: FELD-INVENTUR + senkrechte Groessen auf dem echten Weg ================ */
typedef struct {
    const char *tag; int ok, seen;
    int32_t eY_min, eY_max, plY_min, plY_max;
    int32_t hg_min, hg_max;          /* Hgun mit ZIELHOEHE = pl->y (heutiger Port-Stand) */
    int     gun_valid;               /* wie oft lieferte re15_player_gunbone_world() 1 */
    int32_t gunY_min, gunY_max;
    int     lieg;                    /* Bilder mit grid&0x80 */
} inv_t;

static void pass_inv(inv_t *r, const char *tag, uint8_t type, const char *room, int room_id,
                     int fire_sub, int weapon, int baby_force, int budget)
{
    memset(r, 0, sizeof *r); r->tag = tag;
    r->eY_min = r->plY_min = r->hg_min = r->gunY_min =  0x7fffffff;
    r->eY_max = r->plY_max = r->hg_max = r->gunY_max = -0x7fffffff;
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
    for (int f = 0; f < budget; f++) {
        pl->hp = 100;
        if (e->hp < 400) e->hp = 30000;
        if (!e->active) break;
        track(slot, back);
        /* Schuss ab Bild 20, damit auch der getroffene/liegende Zustand in die Messung faellt */
        int schuss = (f >= 20);
        frame((uint16_t)(RE15_PAD_BIT_R1 | elev_pad_for(e) | (schuss ? RE15_PAD_BIT_SQUARE : 0u)),
              (uint16_t)((schuss && (f % 6) == 0) ? RE15_PAD_BIT_SQUARE : 0u));
        r->seen++;
        if (e->y < r->eY_min) r->eY_min = e->y;
        if (e->y > r->eY_max) r->eY_max = e->y;
        if (pl->y < r->plY_min) r->plY_min = pl->y;
        if (pl->y > r->plY_max) r->plY_max = pl->y;
        int32_t hg = e->y - pl->y;                       /* Hgun, wenn Zielhoehe = pl->y */
        if (hg < r->hg_min) r->hg_min = hg;
        if (hg > r->hg_max) r->hg_max = hg;
        int32_t g[3];
        if (re15_player_gunbone_world(0, 0, 0, g)) {
            r->gun_valid++;
            if (g[1] < r->gunY_min) r->gunY_min = g[1];
            if (g[1] > r->gunY_max) r->gunY_max = g[1];
        }
        if (e->grid_id & 0x80) r->lieg++;
    }
    r->ok = (r->seen > 0);
    printf("  [%s] %d Bilder | eY %d..%d | plY %d..%d | Hgun(Ziel=plY) %d..%d | "
           "Muendung gueltig %d/%d%s | liegend %d\n",
           tag, r->seen, r->eY_min, r->eY_max, r->plY_min, r->plY_max, r->hg_min, r->hg_max,
           r->gun_valid, r->seen,
           r->gun_valid ? "" : " (Renderer hat Bone 11 nie gestellt)", r->lieg);
}

/* ===== Durchgang 2: HUND-KETTE mit Bank, Tor-Urteil je Bild ==============================
 * Nebenher der RIEGEL fuer die Hitbox-Stauchung FUN_80104088 (@0x80104090-D8):
 *   vor dem Treffer  -1000/1000   (INIT @0x8010028C-9C)
 *   nach dem Treffer  -500/500    (FUN_80104088(0) @0x80103458 / @0x8010352C)
 *   nach der Kette   -1000/1000   (FUN_80104088(1) @0x801036F0 / @0x801037C0) */
typedef struct { int ok, b_vor, h_vor, b_lieg, h_lieg, b_nach, h_nach, kette, pause; } box_t2;
static box_t2 s_box;

static void pass_dog_chain(int budget)
{
    memset(&s_box, 0, sizeof s_box);
    if (!load_room("STAGE1/ROOM1190.RDT", 0x1190, 13)) { printf("  FEHLLAUF: ROOM1190 fehlt\n"); return; }
    int slot = setup_target(0x20, 3, 0);
    if (slot < 0) { printf("  FEHLLAUF: kein Hund 0x20 in 1190 - sagt NICHTS\n"); return; }
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    re15_actor_t *e  = &g_actors[slot];
    aim_up(slot, 2000);
    e->re2z_self1d3 = 0;
    s_box.b_vor = e->re2_hit_b98; s_box.h_vor = (int)e->re2_hit_h9e;
    s_box.b_lieg = 0x7fff; s_box.h_lieg = 0x7fff;
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
        if (getroffen < 0) {
            if (low > 0) { getroffen = f;
                printf("      TREFFER f%d: 1D3=%d st=%d/%u/%u/%u clip=%d eY=%d plY=%d\n",
                       f, low, e->state, e->sub_state_1, e->sub_state_2, e->sub_state_3,
                       e->motion, e->y, pl->y); }
            continue; }
        int k = f - getroffen;
        if (e->state == 2) {   /* solange HURT laeuft: die gestauchte Box mitschreiben */
            s_box.b_lieg = e->re2_hit_b98; s_box.h_lieg = (int)e->re2_hit_h9e;
        }
        s_box.b_nach = e->re2_hit_b98; s_box.h_nach = (int)e->re2_hit_h9e;
        if (k < 90)
            printf("      +%-2d 1D3=%2d | st=%d/%u/%u/%u | clip=%2d f=%2d | Box %d/%d | "
                   "Tor(liegend)=%s Tor(stehend)=%s%s%s\n",
                   k, low, e->state, e->sub_state_1, e->sub_state_2, e->sub_state_3,
                   e->motion, e->anim_frame, e->re2_hit_b98, (int)e->re2_hit_h9e,
                   tor(e->y, -500, 500, pl->y) ? "DURCH " : "SPERRT",
                   tor(e->y, -1000, 1000, pl->y) ? "DURCH " : "SPERRT",
                   (low == 0 && pause_bis < 0) ? "   <== PAUSE ENDE" : "",
                   (e->state != 2 && hurt_bis < 0) ? "   <== WIEDER ACTIVE" : "");
        if (low == 0 && pause_bis < 0) pause_bis = k;
        if (e->state != 2 && hurt_bis < 0) hurt_bis = k;
        if (pause_bis >= 0 && hurt_bis >= 0) break;
    }
    printf("  HUND: Pause endete nach %d Bildern, HURT-Kette endete nach %d Bildern",
           pause_bis, hurt_bis);
    if (pause_bis >= 0 && hurt_bis >= 0 && hurt_bis > pause_bis)
        printf("  => %d Bilder treffbar im Liegen\n", hurt_bis - pause_bis);
    else printf("\n");
    s_box.kette = hurt_bis; s_box.pause = pause_bis; s_box.ok = (getroffen >= 0);
    printf("  HITBOX: vor dem Treffer %d/%d | waehrend HURT %d/%d | nach der Kette %d/%d\n",
           s_box.b_vor, s_box.h_vor, s_box.b_lieg, s_box.h_lieg, s_box.b_nach, s_box.h_nach);
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

    printf("=== TEIL 0: DAS TOR ALS ZAHL (reine Rechnung aus @0x8004717C-A4) ===\n");
    printf("  DURCH  <=>  -(b+h+100) <= Hgun < (h-b+100)      [Hgun = eY - Zielhoehe]\n");
    for (unsigned i = 0; i < sizeof BOXEN / sizeof BOXEN[0]; i++) {
        const box_t *B = &BOXEN[i];
        printf("  %-22s b=%6d h=%5d  =>  Fenster Hgun in [%6d , %6d)   %s\n",
               B->name, B->b, B->h, -(B->b + B->h + 100), B->h - B->b + 100, B->quelle);
    }

    printf("\n=== TEIL 1: FELD-INVENTUR IM PORT ===\n");
    printf("  re15_actor_t traegt WEDER +0x98 (Unterkante) NOCH +0x9E (Halbhoehe) als\n");
    printf("  Trefferzonen-Felder. Vorhanden ist nur der NACHBAR +0x9A: `re2z_rad9a`\n");
    printf("  (re15_actor.h:299) — die XZ-Halbbreite, und die nur fuer die Zombie-Familie.\n");
    printf("  Der heutige RE2-Trefferpfad (re15_re2_gun_probe, re15_damage.c:1520) rechnet\n");
    printf("  senkrecht mit `dy = e->y - pl->y` gegen die Waffen-Fenster — die Zielhoehe ist\n");
    printf("  dort also die FUSSHOEHE des Spielers, nicht die Muendung.\n\n");

    inv_t iz, id_, ic, i25, i26;
    pass_inv(&iz,  "ZOMBIE 0x10 R1140", 0x10, "STAGE1/ROOM1140.RDT", 0x1140, -1, 3, 0, 240);
    pass_inv(&id_, "HUND   0x20 R1190", 0x20, "STAGE1/ROOM1190.RDT", 0x1190, 13, 3, 0, 240);
    pass_inv(&ic,  "KRAEHE 0x21 R10C0", 0x21, "STAGE1/ROOM10C0.RDT", 0x10c0, -1, 3, 0, 240);
    pass_inv(&i26, "BABY   0x26 R1090", 0x26, "STAGE1/ROOM1090.RDT", 0x1090, -1, 3, 1, 240);
    const char *sp = NULL; int spid = 0;
    for (unsigned i = 0; i < sizeof SPINNE_KAND / sizeof SPINNE_KAND[0]; i++) {
        if (!load_room(SPINNE_KAND[i].sub, SPINNE_KAND[i].id, -1)) continue;
        if (setup_target(0x25, 3, 0) >= 0) { sp = SPINNE_KAND[i].sub; spid = SPINNE_KAND[i].id; break; }
    }
    if (sp) pass_inv(&i25, "SPINNE 0x25", 0x25, sp, spid, -1, 3, 0, 240);
    else { memset(&i25, 0, sizeof i25); i25.tag = "SPINNE 0x25";
           printf("  [SPINNE 0x25] FEHLLAUF: kein Raum mit lebender 0x25 - sagt NICHTS\n"); }

    printf("\n=== TEIL 2: TOR-URTEIL MIT DEM GEMESSENEN Hgun (Zielhoehe = pl->y, Port-Stand) ===\n");
    {   const inv_t *I[5] = { &iz, &id_, &ic, &i25, &i26 };
        const uint8_t T[5] = { 0x10, 0x20, 0x21, 0x25, 0x26 };
        for (int k = 0; k < 5; k++) {
            if (!I[k]->ok) { printf("  [%s] kein Lauf\n", I[k]->tag ? I[k]->tag : "?"); continue; }
            for (unsigned i = 0; i < sizeof BOXEN / sizeof BOXEN[0]; i++) {
                if (BOXEN[i].type != T[k]) continue;
                int lo = tor(0, BOXEN[i].b, BOXEN[i].h, -I[k]->hg_min);
                int hi = tor(0, BOXEN[i].b, BOXEN[i].h, -I[k]->hg_max);
                printf("  %-22s Hgun gemessen %d..%d  =>  Tor %s / %s\n",
                       BOXEN[i].name, I[k]->hg_min, I[k]->hg_max,
                       lo ? "DURCH" : "SPERRT", hi ? "DURCH" : "SPERRT");
            }
        }
    }

    printf("\n=== TEIL 2b: DIE DREI ZIELHOEHEN-KANDIDATEN DES PORTS ===\n");
    {   /* A = heutiger Stand, B = der einzige gemessene Hand-Bone-Wert im Port,
         *  C = die einzige echte Muendungsquelle. */
        static const struct { const char *nm; int hg; const char *quelle; } K[3] = {
          { "A pl->y (heutiger Trefferpfad)",  0,
            "re15_re2_gun_probe: `dy = e->y - pl->y`, re15_damage.c:1520" },
          { "B pl->y - 2083 (Hand-Bone b13)", 2083,
            "game_step_common.c:1762 `pl->y - 2083`, Quelle shots/pose_aim.txt.leon b13 y=-2083" },
          { "C re15_player_gunbone_world",    -1,
            "re15_damage.c:1134 — braucht s_hand_world/s_hand_rot vom PC-Renderer" },
        };
        for (int k = 0; k < 3; k++) {
            if (K[k].hg < 0) {
                printf("  %-32s : headless NIE verfuegbar (gemessen 0/240 je Typ) — %s\n",
                       K[k].nm, K[k].quelle);
                continue;
            }
            printf("  %-32s : Hgun = %4d  =>", K[k].nm, K[k].hg);
            for (unsigned i = 0; i < sizeof BOXEN / sizeof BOXEN[0]; i++)
                printf(" %s=%s", BOXEN[i].name + 0,
                       tor(0, BOXEN[i].b, BOXEN[i].h, -K[k].hg) ? "ja" : "NEIN");
            printf("\n      (%s)\n", K[k].quelle);
        }
    }

    printf("\n=== TEIL 3: SCHWELLEN-SWEEP — ab welcher Muendungshoehe sperrt das Tor? ===\n");
    printf("  (Hgun = Hoehe der Muendung UEBER dem Gegner-Ursprung, PSX-Y zeigt nach unten)\n");
    printf("  Hgun :");
    for (int h = 0; h <= 2400; h += 200) printf(" %5d", h);
    printf("\n");
    for (unsigned i = 0; i < sizeof BOXEN / sizeof BOXEN[0]; i++) {
        printf("  %-22s", BOXEN[i].name);
        for (int h = 0; h <= 2400; h += 200)
            printf(" %5s", tor(0, BOXEN[i].b, BOXEN[i].h, -h) ? "  ja " : " NEIN");
        printf("\n");
    }

    printf("\n=== TEIL 4: HUND-KETTE MIT RE2-BANK ===\n");
    {   re15_enemy_bank_t *db = load_re2_bank(0x20);
        if (db && db->ok) {
            static const int cl[4] = { 17, 18, 7, 22 };
            static const char *const nm[4] = {
                "P0 Treffer-Zucken @0x80103460 (Wort 0x00070011)",
                "P2 +0x7=0 Hinfallen @0x801036B0 (addiu v1,v1,18)",
                "P2 +0x7=1 Aufstehen @0x801036D0 (Wort 0x000F0007)",
                "P3 weiche Landung @0x8010378C (Wort 0x00030F16)" };
            printf("  EM020-Bank (RE2 CDEMD0.EMS): %d Clips.\n", db->anim.clip_count);
            int summe = 0;
            for (int i = 0; i < 4; i++) {
                int fc = (cl[i] < db->anim.clip_count) ? db->anim.clips[cl[i]].frame_count : -1;
                printf("    Clip %2d  %-48s = %d Bilder\n", cl[i], nm[i], fc);
                if (i < 3 && fc > 0) summe += fc;
            }
            printf("    => P0+P2.0+P2.1 = %d Clip-Bilder\n", summe);
            printf("    FUN_80104088(1) ruft das Original an GENAU ZWEI Stellen:\n");
            printf("      @0x801036F0  HURT-P2, Zweig +0x7 == 2 (`beq v1,v0,0x801036f0`\n");
            printf("                   @0x80103698), und NUR wenn der Clip-Vorschub\n");
            printf("                   `jal 0x8002959c` @0x8010365C != 0 zurueckgab —\n");
            printf("                   also im Bild NACH dem Ende des Aufsteh-Clips 7.\n");
            printf("      @0x801037C0  HURT-P3, nach `jal 0x8002959c` @0x801037B0 != 0,\n");
            printf("                   gefolgt von `sw 0x201,4(s0)` @0x801037C8-D0 = zurueck\n");
            printf("                   nach ACTIVE/Sub 2.\n");
            printf("    Gestaucht wird mit FUN_80104088(0) @0x80103458 (HURT-P0) und\n");
            printf("      @0x8010352C (HURT-P1).\n");
        } else printf("  ACHTUNG: EM020-Bank FEHLT - die Clip-Laengen sagen NICHTS\n");
    }
    pass_dog_chain(600);

    if (!riegel) { printf("\nMESSSCHIENE GELAUFEN (kein Riegel angefordert).\n"); return 0; }

    /* ===== RIEGEL: die Hitbox-Stauchung FUN_80104088, byte-true =========================
     * Der Riegel prueft NUR die Felder +0x98/+0x9E des Hundes. Das fuenfte Gate ist NICHT
     * gebaut (ihm fehlt die Muendungshoehe) und wird hier bewusst NICHT verriegelt. */
    int rot = 0;
    if (!s_box.ok) { printf("RIEGEL: kein Treffer - kein Urteil\n"); rot = 1; }
    else {
        struct { const char *w; int b, h, sb, sh; } P[3] = {
            { "vor dem Treffer (INIT @0x8010028C-9C)", -1000, 1000, s_box.b_vor,  s_box.h_vor  },
            { "waehrend HURT (F80104088(0) @0x80104098-A8)", -500, 500, s_box.b_lieg, s_box.h_lieg },
            { "nach der Kette (F80104088(1) @0x801040B8-C4)", -1000, 1000, s_box.b_nach, s_box.h_nach },
        };
        for (int i = 0; i < 3; i++) {
            if (P[i].sb != P[i].b || P[i].sh != P[i].h) {
                printf("RIEGEL ROT: %s -> %d/%d (SOLL %d/%d)\n",
                       P[i].w, P[i].sb, P[i].sh, P[i].b, P[i].h); rot = 1;
            } else printf("RIEGEL GRUEN: %s -> %d/%d\n", P[i].w, P[i].sb, P[i].sh);
        }
        /* Gegen-Riegel gegen ein EINGEFROREN-Gestaucht (die Runde-13/14-Falle in ihrer
         * Hitbox-Variante): die Kette MUSS enden und die Box MUSS wieder aufgehen. */
        if (s_box.kette < 0) { printf("RIEGEL ROT: die HURT-Kette endete nicht\n"); rot = 1; }
        else printf("RIEGEL GRUEN: HURT-Kette endete nach %d Bildern (Pause %d)\n",
                    s_box.kette, s_box.pause);
    }
    printf(rot ? "\nRIEGEL: ROT\n" : "\nRIEGEL: GRUEN\n");
    return rot;
}
