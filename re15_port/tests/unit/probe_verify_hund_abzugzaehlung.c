/* probe_verify_hund_abzugzaehlung.c — GEGENPRUEFUNG des Befundes
 * "RE2-Hund + Tiefzielen = 255 Abzuege, 0 Treffer".
 *
 * Der Original-Befund (probe_hund_ersttreffer2.c mess_D) zaehlt als "Abzug"
 * JEDES BILD, in dem re15_player_aim_ready() == 1 war:
 *
 *     if (rdy) { abzug++; ... }
 *
 * Das ist KEIN Schusszaehler. Diese Sonde misst denselben Ablauf, zaehlt aber
 * zusaetzlich die ECHTEN Entladungen ueber den einzigen byte-true Beleg, den
 * eine Entladung hinterlaesst: FUN_8004eae4 @0x8004eae4 dekrementiert das
 * Magazin-Byte des ausgeruesteten Slots (+1 in DAT_800b10ac; Port:
 * re15_ammo_consume -> g_inv.slots[s].qty--). Ausserdem wird protokolliert,
 * wie viele der gezaehlten "Abzuege" auf ein LEERES Magazin fallen — dort
 * feuert game_step_common.c:1427-1440 gar nicht (Leer-Zweig reagiert NUR auf
 * die Druckflanke @0x80033338).
 *
 * Reine Messsonde. Kein Engine-Eingriff.
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
#include "re15_emd.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef RE15_ASSET_PSX_DIR
#define RE15_ASSET_PSX_DIR "shared_assets/PSX"
#endif

extern int  re15_player_aim_ready(void);
extern void re15_player_aim_reset(void);
extern void re15_player_set_aim_clip_lens(const uint16_t *fcs, int n);
extern int  re15_player_aim_elevation(void);
extern int  re15_player_aim_phase_debug(void);

static re15_rdt_t         s_rdt;
static int                s_room_id = 0x1190;
static int                s_fire_sub = 13;
static re15_camera_view_t s_cam;
static re15_game_ctx_t    s_ctx;
static const char        *s_base = RE15_ASSET_PSX_DIR;

static uint8_t *slurp(const char *p, size_t *n)
{
    FILE *f = fopen(p, "rb"); if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    if (sz <= 0) { fclose(f); return NULL; }
    uint8_t *b = (uint8_t *)malloc((size_t)sz);
    if (b && fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); b = NULL; }
    fclose(f); if (b) *n = (size_t)sz; return b;
}

static int load_real_gun_cliplens(void)
{
    char p[600]; snprintf(p, sizeof p, "%s/PLD/PL00W03.PLW", s_base);
    size_t sz = 0; uint8_t *plw = slurp(p, &sz);
    uint32_t diroff, de[4]; int k, i, n;
    re15_emd_animation_t an;
    if (!plw || sz < 16) return 0;
    diroff = (uint32_t)(plw[0] | (plw[1]<<8) | (plw[2]<<16) | ((uint32_t)plw[3]<<24));
    if (diroff + 16 > (uint32_t)sz) { free(plw); return 0; }
    for (k = 0; k < 4; k++)
        de[k] = (uint32_t)(plw[diroff+4*k] | (plw[diroff+4*k+1]<<8) |
                           (plw[diroff+4*k+2]<<16) | ((uint32_t)plw[diroff+4*k+3]<<24));
    memset(&an, 0, sizeof an);
    if (!(de[0] < de[1] && de[1] <= (uint32_t)sz) ||
        re15_emd_parse_animation(plw + de[0], (int)(de[1] - de[0]), &an) != 0) { free(plw); return 0; }
    {
        uint16_t fcs[16]; n = (an.clip_count < 16) ? an.clip_count : 16;
        for (i = 0; i < n; i++) fcs[i] = (uint16_t)an.clips[i].frame_count;
        re15_player_set_aim_clip_lens(fcs, n);
    }
    free(plw);
    return 1;
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
    re15_actor_t *pl; int i;
    re15_actor_init(); re15_aot_init(); scd_vm_init();
    re15_enemy_reset(); re15_enemy_ai_set_paused(0);
    re15_player_cmd_reset(); re15_player_aim_reset();
    re15_damage_seed_rng(0x0badf00du);
    g_current_room_id = (uint16_t)s_room_id;
    if (s_rdt.main_scd)   scd_thread_start(0, s_rdt.main_scd);
    if (s_rdt.sub_scd[0]) scd_thread_start(1, s_rdt.sub_scd[0]);
    if (s_fire_sub >= 0 && s_rdt.sub_scd_count > s_fire_sub && s_rdt.sub_scd[s_fire_sub])
        scd_thread_start(2, s_rdt.sub_scd[s_fire_sub]);
    g_scd.work_vars[10] = 0;
    for (i = 0; i < 120; i++) scd_vm_tick();
    pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100; pl->hit_react = 0;
    pl->state = 0; pl->motion = 0; pl->floor = 0; pl->y = 0;
    re15_collision_set_band(0);
    load_real_gun_cliplens();
}

static int first_enemy(unsigned want_type)
{
    int s;
    for (s = 1; s < RE15_ACTOR_MAX; s++)
        if (g_actors[s].active && g_actors[s].type == (uint8_t)want_type) return s;
    return -1;
}

/* Magazin-Byte des ausgeruesteten Slots (inv_resolve_slot-Zwilling, FUN_8004ea6c). */
static int mag_qty(void)
{
    int s = re15_inv_equipped_slot();
    if (s == 0x80 || s < 0 || s >= RE15_INV_MAX_SLOTS) return -1;
    if (g_inv.slots[s].flags == 2 && s > 0) s--;
    return (int)g_inv.slots[s].qty;
}

static void lauf(const char *tag, int flavor, unsigned type, int weapon, uint16_t elev_bit,
                 int verbose)
{
    int slot, f, raise = 0;
    int abzug_rdyframes = 0;     /* die Zaehlweise DES BEFUNDES                    */
    int abzug_leer      = 0;     /* davon: Magazin LEER -> es feuert gar nichts    */
    int schuesse        = 0;     /* ECHTE Entladungen (Magazin-Dekrement)          */
    int treffer         = 0;
    int prev_mag;
    re15_actor_t *e, *pl;
    uint16_t pad;

    printf("\n--- %s (flavor=%s, typ=0x%02X, w=%d, elev=%s) ---\n", tag,
           flavor == RE15_AI_FLAVOR_RE2 ? "RE2" : "RE1.5", type, weapon,
           elev_bit == RE15_PAD_BIT_DOWN ? "UNTEN" : elev_bit == RE15_PAD_BIT_UP ? "OBEN" : "eben");

    re15_ai_flavor_set((re15_ai_flavor_t)flavor);
    bringup();
    re15_inv_load_briefing();
    re15_player_set_equipped_weapon(weapon);
    for (f = 0; f < 60; f++) { g_actors[RE15_ACTOR_SLOT_PLAYER].hp = 100; frame(0, 0); }
    slot = first_enemy(type);
    if (slot < 0) { printf("  kein Gegner\n"); return; }
    for (f = 1; f < RE15_ACTOR_MAX; f++) if (f != slot) g_actors[f].active = 0;
    re15_ai_set_state_word(&g_actors[slot], 0x201);
    g_actors[slot].sub_state_2 = 0; g_actors[slot].sub_state_3 = 0;

    e = &g_actors[slot]; pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->x = e->x - 3000; pl->z = e->z; pl->y = e->y;
    pl->rot_y = (int16_t)(((int)re15_atan2_q12(e->z - pl->z, e->x - pl->x) - 0x400) & 0xfff);
    pl->hp = 100;
    re15_player_cmd_reset(); re15_player_aim_reset(); load_real_gun_cliplens();
    pad = (uint16_t)(RE15_PAD_BIT_R1 | elev_bit);
    for (f = 0; f < 60 && !re15_player_aim_ready(); f++) { pl->hp = 100; frame(pad, 0); raise++; }

    printf("  Start: equip-Slot=%d slot-id=0x%02X mag=%d | Reserve-Slot=%d\n",
           re15_inv_equipped_slot(),
           (re15_inv_equipped_slot() < RE15_INV_MAX_SLOTS)
               ? g_inv.slots[re15_inv_equipped_slot()].id : 0xff,
           mag_qty(), re15_ammo_reserve_slot());

    prev_mag = mag_qty();
    for (f = 0; f < 600; f++) {
        int rdy = re15_player_aim_ready();
        int hpb = e->hp, magb = mag_qty();
        pl->hp = 100;
        frame((uint16_t)(pad | RE15_PAD_BIT_SQUARE),
              (uint16_t)((f == 0) ? RE15_PAD_BIT_SQUARE : 0));
        if (rdy) { abzug_rdyframes++; if (magb == 0) abzug_leer++; }
        { int magn = mag_qty();
          if (magn >= 0 && magb > magn) schuesse += (magb - magn); }
        if (e->hp < hpb) treffer++;
        if (verbose && f < 60)
            printf("    f%03d rdy=%d phase=0x%02X mag=%d hp=%d\n",
                   f, rdy, re15_player_aim_phase_debug(), mag_qty(), e->hp);
        prev_mag = mag_qty();
        if (e->hp < 0) { printf("  (tot in f%03d)\n", f); break; }
    }
    (void)prev_mag;
    printf("  ERGEBNIS %s:\n", tag);
    printf("    Zaehlweise DES BEFUNDES (aim_ready-BILDER) : %d\n", abzug_rdyframes);
    printf("    davon mit LEEREM Magazin (es feuert nichts): %d\n", abzug_leer);
    printf("    ECHTE Entladungen (Magazin-Dekrement)      : %d\n", schuesse);
    printf("    Treffer                                    : %d\n", treffer);
    printf("    Endstand Magazin                           : %d\n", mag_qty());
}


/* ---------------------------------------------------------------------------
 * KONTROLL-EXPERIMENT am ECHTEN Engine-Pfad (keine Nachbildung):
 * re15_damage.c:1557-1567 (generischer else-Zweig, den ein RE2-eigener Hund
 * nimmt, weil re15_damage.c:1469 auf `!re15_ai_re2_for_type(0x20)` gattert):
 *     if (e->grid_id & 0x80) eband = (bdist < 0x1388u) ? 0x20000000u : 0u;
 *     else                   eband = 0x40000000u;
 * Wenn wirklich DAS Hoehen-Band verwirft, muss ein erzwungenes grid_id|0x80
 * bei Distanz < 0x1388 den TIEF-Schuss schlagartig treffen lassen - und der
 * EBEN-Schuss muss dann umgekehrt ins Leere gehen.
 * Der Spieler haengt dabei auf fester Distanz (Geometrie konstant).
 * ------------------------------------------------------------------------- */
static void kontrolle(const char *tag, uint16_t elev_bit, int force_grid80)
{
    int slot, f, schuesse = 0, treffer = 0, magb, magn;
    re15_actor_t *e, *pl;
    uint16_t pad;
    printf("\n--- KONTROLLE %s (elev=%s, grid|0x80 erzwungen=%d) ---\n", tag,
           elev_bit == RE15_PAD_BIT_DOWN ? "UNTEN" : elev_bit == RE15_PAD_BIT_UP ? "OBEN" : "eben",
           force_grid80);
    re15_ai_flavor_set(RE15_AI_FLAVOR_RE2);
    bringup();
    re15_inv_load_briefing();
    re15_player_set_equipped_weapon(3);
    for (f = 0; f < 60; f++) { g_actors[RE15_ACTOR_SLOT_PLAYER].hp = 100; frame(0, 0); }
    slot = first_enemy(0x20);
    if (slot < 0) { printf("  kein Gegner\n"); return; }
    for (f = 1; f < RE15_ACTOR_MAX; f++) if (f != slot) g_actors[f].active = 0;
    re15_ai_set_state_word(&g_actors[slot], 0x201);
    g_actors[slot].sub_state_2 = 0; g_actors[slot].sub_state_3 = 0;
    e = &g_actors[slot]; pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    re15_player_cmd_reset(); re15_player_aim_reset(); load_real_gun_cliplens();
    pad = (uint16_t)(RE15_PAD_BIT_R1 | elev_bit);
    for (f = 0; f < 60 && !re15_player_aim_ready(); f++) {
        pl->x = e->x - 2600; pl->z = e->z; pl->y = e->y;
        pl->rot_y = (int16_t)(((int)re15_atan2_q12(e->z - pl->z, e->x - pl->x) - 0x400) & 0xfff);
        pl->hp = 100; frame(pad, 0);
    }
    magb = mag_qty();
    for (f = 0; f < 400; f++) {
        int hpb;
        pl->x = e->x - 2600; pl->z = e->z; pl->y = e->y;   /* Distanz 2600 < 0x1388 = 5000 */
        pl->rot_y = (int16_t)(((int)re15_atan2_q12(e->z - pl->z, e->x - pl->x) - 0x400) & 0xfff);
        pl->hp = 100;
        if (force_grid80) e->grid_id = (uint8_t)(e->grid_id | 0x80u);
        hpb = e->hp;
        frame((uint16_t)(pad | RE15_PAD_BIT_SQUARE),
              (uint16_t)((f == 0) ? RE15_PAD_BIT_SQUARE : 0));
        magn = mag_qty();
        if (magn >= 0 && magb > magn) schuesse += (magb - magn);
        magb = magn;
        if (e->hp < hpb) treffer++;
        if (e->hp < 0) break;
        if (magn == 0) break;                              /* Magazin leer -> Ende */
    }
    printf("  grid_id=0x%02X  ECHTE Schuesse=%d  Treffer=%d  (hp %d)\n",
           e->grid_id, schuesse, treffer, e->hp);
}

static int load_room(const char *sub, int room_id, int fsub)
{
    char path[600]; size_t sz = 0; uint8_t *b;
    snprintf(path, sizeof path, "%s/%s", s_base, sub);
    b = slurp(path, &sz);
    if (!b) { printf("RDT fehlt: %s\n", path); return 0; }
    if (re15_rdt_parse(b, sz, &s_rdt) != 0) { printf("RDT-Parse %s\n", sub); return 0; }
    s_ctx.rdt = &s_rdt; s_ctx.rdt_ok = 1;
    s_room_id = room_id; s_fire_sub = fsub;
    return 1;
}

int main(int argc, char **argv)
{
    const char *base = getenv("RE15_ASSET_DIR");
    int verbose = (argc > 1 && argv[1][0] == 'v');
    if (base && *base) s_base = base;
    memset(&s_cam, 0, sizeof s_cam); memset(&s_ctx, 0, sizeof s_ctx);
    s_ctx.cam_view = &s_cam; s_ctx.active_cut = 0;

    if (!load_room("STAGE1/ROOM1190.RDT", 0x1190, 13)) return 1;
    lauf("HUND-RE2-eben", RE15_AI_FLAVOR_RE2, 0x20, 3, 0, verbose);
    lauf("HUND-RE2-tief", RE15_AI_FLAVOR_RE2, 0x20, 3, RE15_PAD_BIT_DOWN, verbose);
    lauf("HUND-RE2-hoch", RE15_AI_FLAVOR_RE2, 0x20, 3, RE15_PAD_BIT_UP, verbose);

    kontrolle("eben / grid unveraendert", 0, 0);
    kontrolle("TIEF / grid unveraendert", RE15_PAD_BIT_DOWN, 0);
    kontrolle("TIEF / grid|0x80",         RE15_PAD_BIT_DOWN, 1);
    kontrolle("eben / grid|0x80",         0, 1);
    return 0;
}
