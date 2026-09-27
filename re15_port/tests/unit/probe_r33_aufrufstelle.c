/* ============================================================================================
 * probe_r33_aufrufstelle — Runde 33: die ZWEITE Aufrufstelle des Kandidatenfilters
 *
 * Aufgeloest wurde (Dossier analysis/befunde_2026-09-27/zweite-aufrufstelle.md):
 *   * Teil+0x5C..+0x64 ist die TRANSLATION t[0..2] der WELT-Matrix von Teil 20 (Teil 17 bei
 *     Entity-Typ 14) des Spieler-Teilepools +0x198 — ein PROJEKTIL-Arbeitsplatz, keine
 *     Muendung (`addiu s5,s3,92` @0x80046398; Integration `+= Schritt` @0x80046550/68/7C).
 *   * Aufrufstelle (A) @0x80042F94 gehoert zu WAFFE 1 = MESSER, (B) @0x800467C0 zu WAFFE 12.
 *     Jede SCHUSSWAFFE laeuft ueber FUN_800410CC (@0x80043AFC) und liest +0x98/+0x9E nie.
 *   * +0x14D ist die BILDNUMMER im Clip (`lbu/addiu 1/sb` @0x80029B28-34), kein Waffenfilter.
 *   * Der allgemeine Leser der Haltungsklasse (word0>>26)&7 ist FUN_800410CC @0x800413C4-D8.
 *
 * DIESE SONDE MISST ZWEIERLEI:
 *   TEIL 1  Die Haltungsklasse des RE2-Hundes, neu gebaut aus FUN_80104088 (EMD0G_MOD0.BIN,
 *           @0x80104090-D8): liegend genau 1, stehend genau 3.
 *   TEIL 2  Den Trefferzensus je Typ ueber 900 Bilder Dauerbeschuss — plus die KONTROLLE mit
 *           kuenstlich genullter Trefferbox. Faellt die Kontrolle nicht auf 0, misst die
 *           Sonde das fuenfte Tor gar nicht und ihre Zahlen sagen nichts.
 *
 * Gemessen auf dem ECHTEN Weg (re15_game_step + Pad, echte RDTs, RE2-KI-Geschmack,
 * RE2-Bank shared_assets/RE2/CDEMD0.EMS — ohne sie haben alle Clips Laenge 0, Falle Runde 29).
 * ============================================================================================ */
#include "re15_emd.h"
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
extern int     re15_player_muzzle_world(int32_t out[3]);
extern int16_t re15_atan2_q12(int32_t dz, int32_t dx);
extern void    re15_esp_fx_reset(void);

static re15_rdt_t         s_rdt;
static re15_camera_view_t s_cam;
static re15_emd_skeleton_t  s_pl00_skel;
static re15_emd_animation_t s_pl00_anim;
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
    pl->y = e->y;                       /* gleiche Ebene, wie in probe_r29/r30b begruendet */
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

static uint32_t s_last_gate5, s_last_class;
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

/* kriecher != 0 sucht die LIEGENDE Zombie-Variante (grid & 0x80), sonst die stehende. */
static int find_type(uint8_t type, int kriecher)
{
    for (int s = 1; s < RE15_ACTOR_MAX; s++) {
        if (!g_actors[s].active || g_actors[s].type != type) continue;
        if (type >= 0x10 && type <= 0x18) {
            int k = (g_actors[s].grid_id & 0x80) ? 1 : 0;
            if (k != (kriecher ? 1 : 0)) continue;
        }
        return s;
    }
    return -1;
}

static int setup_target(uint8_t type, int kriecher, int weapon, int baby_force)
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
    int slot = find_type(type, kriecher);
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

/* ===== TEIL 1: DIE HALTUNGSKLASSE DES HUNDES ==============================================
 * FUN_80104088 (EMD0G_MOD0.BIN, selbst disassembliert):
 *   a1 == 0: `lui a0,0xe7ff / ori a0,a0,0xffff` @0x80104090-94, `and v0,v0,a0` @0x801040B4,
 *            `lui v1,0x400` @0x801040AC, `or`/`sw` @0x801040D0-D8  -> (word0>>26)&7 == 1
 *   a1 != 0: `lui v1,0xc00` @0x801040CC OHNE Maske                 -> (word0>>26)&7 |= 3
 * Leser: FUN_800410CC @0x800413C4-D8 (`srl v0,v0,26` / `andi s6,v0,0x7` / `beq s6,zero`).
 * Der Port fuehrt das Feld als re15_actor_t.re2z_parts. */
static int teil1_haltung(int budget, int *saw_lying, int *saw_standing, int *bad)
{
    if (!load_room("STAGE1/ROOM1190.RDT", 0x1190, 13)) {
        printf("  FEHLLAUF: ROOM1190 fehlt - sagt NICHTS\n"); return -1; }
    int slot = setup_target(0x20, 0, 3, 0);
    if (slot < 0) { printf("  FEHLLAUF: kein Hund 0x20 in 1190 - sagt NICHTS\n"); return -1; }
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    re15_actor_t *e  = &g_actors[slot];
    aim_up(slot, 2000);
    *saw_lying = *saw_standing = *bad = 0;
    for (int f = 0; f < budget; f++) {
        pl->hp = 100; e->hp = 30000;
        if (!e->active) break;
        track(slot, 2000);
        frame((uint16_t)(RE15_PAD_BIT_R1 | RE15_PAD_BIT_SQUARE),
              (uint16_t)(((f % 6) == 0) ? RE15_PAD_BIT_SQUARE : 0u));
        if (!e->re2_hit_box_set) continue;
        unsigned k = (unsigned)e->re2z_parts & 7u;
        if (e->re2_hit_b98 == -500) {            /* gestauchte Box = a1 == 0 */
            (*saw_lying)++;  if (k != 1u) (*bad)++;
        } else if (e->re2_hit_b98 == -1000) {    /* volle Box = a1 != 0 */
            (*saw_standing)++; if ((k & 3u) != 3u) (*bad)++;
        }
    }
    printf("  Hund 0x20: gestauchte Box in %d Bildern (Klasse muss 1 sein), volle Box in %d\n"
           "             Bildern (Klasse muss 3 tragen) | Abweichungen: %d\n",
           *saw_lying, *saw_standing, *bad);
    return 0;
}

/* ===== TEIL 2: TREFFERZENSUS + KONTROLLEN ==================================================
 * ⛔ RUNDE 34 UMGESTELLT. Bis Runde 33 sperrte den liegenden Hund das FUENFTE TOR
 * (@0x8004716C-A4, +0x98/+0x9E gegen die Muendungshoehe), und die Kontrolle nullte genau
 * diese Box. Seit Runde 34 laeuft der Schuss ueber die HALTUNGSKLASSE (word0>>26)&7
 * (FUN_800410CC @0x800413C4-D8) gegen die Maskentabelle @0x800A6DB4; das fuenfte Tor
 * gehoert nur noch dem MESSER (@0x80042F94) und dem Bolzen (@0x800467C0).
 * Deshalb gibt es jetzt ZWEI Kontrollen, und beide messen den MECHANISMUS:
 *   null_box == 1  Trefferbox jedes Bild b=h=0 und gueltig. FRUEHER musste die Trefferzahl
 *                  darauf auf 0 fallen; JETZT darf sie sich NICHT MEHR AENDERN — genau das
 *                  ist der Beweis, dass das Tor nicht mehr im Schuss-Pfad liegt.
 *   null_box == 3  Haltungsklasse jedes Bild = 4 (nur Bit 2). Die EBEN-Zeile 3 der
 *                  Maskentabelle lautet `2,0,0` und fragt allein Bit 1 ab -> die Trefferzahl
 *                  MUSS bei jedem Applier-Typ auf 0 fallen. (Klasse 0 taugt als Kontrolle
 *                  nicht: fuer die Zombie-Familie faengt sie die Port-Ersatzmaske
 *                  "parts==0 -> 3" ab, gemessen 70 Treffer trotz Klasse 0.)
 * Jeder Lauf zaehlt zusaetzlich die Tor-Urteile und die Klassen-Lesungen mit. */
static int zensus(const char *tag, uint8_t type, int kriecher, const char *room, int room_id,
                  int fire_sub, int baby_force, int budget, int null_box, int *box_set_out)
{
    if (!load_room(room, room_id, fire_sub)) {
        printf("  [%-22s] FEHLLAUF: Raum %s fehlt - sagt NICHTS\n", tag, room); return -1; }
    int slot = setup_target(type, kriecher, 3, baby_force);
    if (slot < 0) {
        printf("  [%-22s] FEHLLAUF: kein Ziel Typ 0x%02X%s in %04X - sagt NICHTS\n",
               tag, type, kriecher ? " (Kriecher)" : "", room_id); return -1; }
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    re15_actor_t *e  = &g_actors[slot];
    aim_up(slot, 2000);
    re15_dmg_mech_reset();
    int treffer = 0, box_set = 0;
    for (int f = 0; f < budget; f++) {
        pl->hp = 100; e->hp = 30000;
        if (!e->active) break;
        track(slot, 2000);
        if (null_box == 1) { e->re2_hit_b98 = 0; e->re2_hit_h9e = 0; e->re2_hit_box_set = 1; }
        else if (null_box == 2) { /* KRIECHER-Box des RE2-Zombies, @0x80100B14-20 / @0x80103460-6C */
            e->re2_hit_b98 = -350; e->re2_hit_h9e = 350; e->re2_hit_box_set = 1; }
        else if (null_box == 3) { e->re2z_parts = 4u; }   /* nur Bit 2, s. Blockkopf */
        frame((uint16_t)(RE15_PAD_BIT_R1 | elev_pad_for(e) | RE15_PAD_BIT_SQUARE),
              (uint16_t)(((f % 6) == 0) ? RE15_PAD_BIT_SQUARE : 0u));
        if (e->re2_hit_box_set) box_set = 1;
        if (e->hp < 30000) treffer++;
    }
    if (box_set_out) *box_set_out = box_set;
    re15_dmg_mech_counts(&s_last_gate5, &s_last_class, NULL);
    printf("  [%-22s] Treffer %4d in %4d Bildern | box_set=%d | Tor5 %u / Klasse %u%s\n",
           tag, treffer, budget, box_set, s_last_gate5, s_last_class,
           null_box == 1 ? "  (KONTROLLE Box genullt — darf NICHTS aendern)" :
           null_box == 2 ? "  (Box erzwungen: Kriecher -350/350 @0x80100B14-20)" :
           null_box == 3 ? "  (KONTROLLE Klasse 4 — muss auf 0 fallen)" : "");
    return treffer;
}

static const struct { const char *sub; int id; } SPINNENRAUM[] = {
    { "STAGE1/ROOM1090.RDT", 0x1090 }, { "STAGE2/ROOM2000.RDT", 0x2000 },
    { "STAGE2/ROOM2010.RDT", 0x2010 }, { "STAGE2/ROOM2020.RDT", 0x2020 },
    { "STAGE2/ROOM2040.RDT", 0x2040 }, { "STAGE2/ROOM2050.RDT", 0x2050 },
    { "STAGE2/ROOM2060.RDT", 0x2060 }, { "STAGE1/ROOM10D0.RDT", 0x10d0 },
    { "STAGE1/ROOM1260.RDT", 0x1260 }, { "STAGE3/ROOM3010.RDT", 0x3010 } };

int main(int argc, char **argv)
{
    const int riegel = (argc > 1 && strcmp(argv[1], "riegel") == 0);
    setvbuf(stdout, NULL, _IONBF, 0);
    /* Ohne Leons eigene Bank liefert re15_player_muzzle_world 0 und das fuenfte Tor gatet
     * gar nicht - dann misst die KONTROLLE unten nichts (dieselbe Falle wie in r30b). */
    {   size_t esz = 0, rsz = 0;
        uint8_t *edd = slurp(RE15_ASSET_PSX_DIR "/PLD/PL00.EDD", &esz);
        uint8_t *emr = slurp(RE15_ASSET_PSX_DIR "/PLD/PL00.EMR", &rsz);
        if (!edd || !emr) { printf("SKIP: PL00.EDD/EMR fehlt\n"); return 77; }
        if (re15_emd_parse_animation(edd, esz, &s_pl00_anim) != 0 ||
            re15_emd_parse_skeleton (emr, rsz, &s_pl00_skel) != 0) {
            printf("FAIL: PL00-Parse\n"); return 1; } }
    memset(&s_cam, 0, sizeof s_cam); memset(&s_ctx, 0, sizeof s_ctx);
    s_ctx.rdt = &s_rdt; s_ctx.rdt_ok = 1; s_ctx.cam_view = &s_cam; s_ctx.active_cut = 0;
    s_ctx.pl00_skel = &s_pl00_skel; s_ctx.pl00_anim = &s_pl00_anim;

    printf("=== TEIL 1: HALTUNGSKLASSE (word0>>26)&7 DES RE2-HUNDES ===\n");
    int lie = 0, stand = 0, bad = 0;
    int t1 = teil1_haltung(400, &lie, &stand, &bad);

    printf("\n=== TEIL 2: TREFFERZENSUS, 900 BILDER DAUERBESCHUSS JE TYP ===\n");
    int tb[7], bs[7];
    tb[0] = zensus("ZOMBIE 0x10 stehend",  0x10, 0, "STAGE1/ROOM1140.RDT", 0x1140, -1, 0, 900, 0, &bs[0]);
    /* KRIECHER: in keinem der durchsuchten Raeume steht beim Laden ein LEBENDER 0x10 mit
     * grid & 0x80 (nachgesehen 2026-09-27: 1010/1220/1050/10F0/2000/2010 — alle 0 Treffer).
     * Gemessen wird deshalb der stehende Zombie mit der KRIECHER-BOX -350/350, die das
     * Original im Kriech-Tick setzt (`sh` @0x80100B14-20 und @0x80103460-6C, Vollscan
     * Runde 30). Das ist genau der Eingang, den das fuenfte Tor sieht — die Zeile misst das
     * TOR, nicht die Kriech-KI, und sagt das auch so. */
    tb[1] = zensus("ZOMBIE 0x10 Kriecherbox", 0x10, 0, "STAGE1/ROOM1140.RDT", 0x1140, -1, 0, 900, 2, &bs[1]);
    tb[2] = zensus("HUND 0x20",            0x20, 0, "STAGE1/ROOM1190.RDT", 0x1190, 13, 0, 900, 0, &bs[2]);
    tb[3] = zensus("KRAEHE 0x21",          0x21, 0, "STAGE1/ROOM10C0.RDT", 0x10c0, -1, 0, 900, 0, &bs[3]);
    const char *sp = NULL; int spid = 0;
    for (unsigned i = 0; i < sizeof SPINNENRAUM / sizeof SPINNENRAUM[0]; i++) {
        if (!load_room(SPINNENRAUM[i].sub, SPINNENRAUM[i].id, -1)) continue;
        if (setup_target(0x25, 0, 3, 0) >= 0) { sp = SPINNENRAUM[i].sub; spid = SPINNENRAUM[i].id; break; } }
    tb[4] = sp ? zensus("SPINNE 0x25", 0x25, 0, sp, spid, -1, 0, 900, 0, &bs[4]) : -1;
    if (!sp) printf("  [SPINNE 0x25          ] FEHLLAUF: kein Raum mit lebender 0x25 - sagt NICHTS\n");
    tb[5] = zensus("BABY 0x26",            0x26, 0, "STAGE1/ROOM1090.RDT", 0x1090, -1, 1, 900, 0, &bs[5]);
    tb[6] = -2;   /* Platzhalter, s. Kontrolle */

    printf("\n=== TEIL 3a: KONTROLLE — TREFFERBOX KUENSTLICH GENULLT ===\n");
    printf("  (Seit Runde 34 ist das die GEGENPROBE: die Zahlen muessen den Zeilen aus Teil 2\n"
           "   gleichen und Tor5 muss 0 sein — das Tor liegt nicht mehr im Schuss-Pfad.)\n");
    int kc_dummy = 0;
    int kz = zensus("ZOMBIE 0x10 stehend",  0x10, 0, "STAGE1/ROOM1140.RDT", 0x1140, -1, 0, 300, 1, &kc_dummy);
    uint32_t kz_g5 = s_last_gate5;
    int kh = zensus("HUND 0x20",            0x20, 0, "STAGE1/ROOM1190.RDT", 0x1190, 13, 0, 300, 1, &kc_dummy);
    uint32_t kh_g5 = s_last_gate5;
    printf("\n=== TEIL 3b: KONTROLLE — HALTUNGSKLASSE AUF 4 (nur Bit 2) ===\n");
    int kz4 = zensus("ZOMBIE 0x10 stehend",  0x10, 0, "STAGE1/ROOM1140.RDT", 0x1140, -1, 0, 300, 3, &kc_dummy);
    uint32_t kz4_cls = s_last_class;
    int kh4 = zensus("HUND 0x20",            0x20, 0, "STAGE1/ROOM1190.RDT", 0x1190, 13, 0, 300, 3, &kc_dummy);
    uint32_t kh4_cls = s_last_class;

    /* ===== TEIL 4: DER NUTZER-BEFUND ALS BILDERZAHL, JETZT UEBER DIE KLASSE ===============
     * "Im Original sind die Hunde erst wieder verwundbar, sobald sie stehen" (Runde 26).
     * Gemessen wird der volle Ablauf am STUECK: stehend treffen -> er geht nieder -> waehrend
     * er liegt darf KEIN Schaden fallen -> er steht auf -> es faellt wieder Schaden.
     * Dazu je Abschnitt die Haltungsklasse, damit die Ursache mitgemessen ist. */
    int b_stand_hits = 0, b_lie_frames = 0, b_lie_hits = 0, b_up_frames = 0, b_up_hits = 0;
    int b_kette = -1, b_cls_lie_bad = 0, b_cls_up_ok = 0;
    uint32_t b_g5 = 0, b_cls = 0;
    if (load_room("STAGE1/ROOM1190.RDT", 0x1190, 13)) {
        int slot = setup_target(0x20, 0, 3, 0);
        if (slot >= 0) {
            re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
            re15_actor_t *e  = &g_actors[slot];
            aim_up(slot, 2000);
            e->re2z_self1d3 = 0;
            re15_dmg_mech_reset();
            int nieder = -1, wieder = -1;
            for (int f = 0; f < 600; f++) {
                pl->hp = 100; e->hp = 30000;
                if (!e->active) break;
                track(slot, 2000);
                int schuss = (f >= 10);
                frame((uint16_t)(RE15_PAD_BIT_R1 | (schuss ? RE15_PAD_BIT_SQUARE : 0u)),
                      (uint16_t)((schuss && (f % 6) == 0) ? RE15_PAD_BIT_SQUARE : 0u));
                int hit = (e->hp < 30000);
                unsigned k = (unsigned)e->re2z_parts & 7u;
                if (nieder < 0) { if (hit) b_stand_hits++;
                                  if (e->state == 2) nieder = f;
                                  continue; }
                if (wieder < 0 && e->state != 2) wieder = f;
                if (wieder < 0) { b_lie_frames++; if (hit) b_lie_hits++;
                                  if (k != 1u) b_cls_lie_bad++; }
                else            { b_up_frames++;  if (hit) b_up_hits++;
                                  /* ⛔ NICHT "in JEDEM Bild Bit 1": der Hund wird hier weiter
                                   * beschossen und geht dabei erneut nieder — dann IST die
                                   * Klasse wieder 1 (`and 0xE7FFFFFF` @0x80104090-B4), und das
                                   * ist richtig. Gezaehlt wird, ob er nach dem Aufstehen
                                   * ueberhaupt wieder Bit 1 traegt. Die strenge Zuordnung
                                   * Box <-> Klasse prueft Teil 1 (0 Abweichungen/400 Bilder). */
                                  if (k & 2u) b_cls_up_ok++;
                                  if (b_up_frames >= 60) break; }
            }
            if (nieder >= 0 && wieder >= 0) b_kette = wieder - nieder;
            re15_dmg_mech_counts(&b_g5, &b_cls, NULL);
        }
    }
    printf("\n=== TEIL 4: DER NUTZER-BEFUND, GEMESSEN UEBER DIE HALTUNGSKLASSE ===\n");
    printf("  stehend getroffen: %d | Kette bis er wieder steht: %d Bilder\n"
           "  waehrend er LIEGT: %d Treffer in %d Bildern (Klasse != 1 in %d Bildern)\n"
           "  nach dem AUFSTEHEN: %d Treffer in %d Bildern (Klasse MIT Bit 1 in %d Bildern)\n"
           "  MECHANISMUS: Tor5-Urteile %u | Klassen-Lesungen %u\n",
           b_stand_hits, b_kette, b_lie_hits, b_lie_frames, b_cls_lie_bad,
           b_up_hits, b_up_frames, b_cls_up_ok, b_g5, b_cls);

    /* ===== TEIL 5: DAS MESSER — DAS TOR IST NICHT WEG, ES IST UMGEZOGEN ==================
     * Runde 34 nimmt das fuenfte Tor aus dem Schuss-Pfad. Damit es nicht STILL verschwindet,
     * wird hier die Gegenrichtung gemessen: mit dem MESSER (RE2-Waffe 1, Aufrufstelle
     * @0x80042F94) MUSS es Urteile faellen, und das Messer muss weiter treffen.
     * Reichweite 900 wie in probe_r31_boxen (Nahkampf-Kegel FUN_800127FC, Dispatch
     * @0x8006E548[1] = 0x800127FC). */
    int m_hits[2] = { -1, -1 }; uint32_t m_g5[2] = { 0, 0 };
    {   static const struct { const char *tag; uint8_t type; const char *room; int id; int sub; }
            MZ[2] = { { "ZOMBIE 0x10", 0x10, "STAGE1/ROOM1140.RDT", 0x1140, -1 },
                      { "HUND   0x20", 0x20, "STAGE1/ROOM1190.RDT", 0x1190, 13 } };
        printf("\n=== TEIL 5: MESSER (Waffe 1) — das Tor MUSS hier laufen ===\n");
        for (int i = 0; i < 2; i++) {
            if (!load_room(MZ[i].room, MZ[i].id, MZ[i].sub)) {
                printf("  [%s] FEHLLAUF: Raum fehlt - sagt NICHTS\n", MZ[i].tag); continue; }
            int slot = setup_target(MZ[i].type, 0, 1, 0);
            if (slot < 0) { printf("  [%s] FEHLLAUF: kein Ziel - sagt NICHTS\n", MZ[i].tag);
                            continue; }
            re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
            re15_actor_t *e  = &g_actors[slot];
            aim_up(slot, 900);
            re15_dmg_mech_reset();
            int hits = 0;
            for (int f = 0; f < 300; f++) {
                pl->hp = 100; e->hp = 30000;
                if (!e->active) break;
                track(slot, 900);
                frame((uint16_t)(RE15_PAD_BIT_R1 | RE15_PAD_BIT_SQUARE),
                      (uint16_t)(((f % 6) == 0) ? RE15_PAD_BIT_SQUARE : 0u));
                if (e->hp < 30000) hits++;
            }
            re15_dmg_mech_counts(&m_g5[i], &s_last_class, NULL);
            m_hits[i] = hits;
            printf("  [%s] Messer-Treffer %3d in 300 Bildern | Tor5-Urteile %u\n",
                   MZ[i].tag, hits, m_g5[i]);
        }
    }

    printf("\n=== URTEIL ===\n");
    printf("  Haltungsklasse Hund: %s\n",
           (t1 == 0 && bad == 0 && lie > 0 && stand > 0) ? "byte-true (liegend 1, stehend 3)"
                                                         : "NICHT vollstaendig gemessen");
    {   static const char *N[6] = { "ZOMBIE 0x10 stehend", "ZOMBIE 0x10 Kriecherbox (erzwungen)",
                                    "HUND 0x20", "KRAEHE 0x21", "SPINNE 0x25", "BABY 0x26" };
        for (int i = 0; i < 6; i++)
            printf("  %-38s %s\n", N[i],
                   tb[i] < 0 ? "FEHLLAUF" : (tb[i] > 0 ? "treffbar" : "⛔ NIE GETROFFEN")); }
    printf("  (Zeile 2 war bis Runde 33 eine WARNUNG: mit der Kriecher-Box -350/350 sperrte das\n"
           "   fuenfte Tor den Kandidaten dauerhaft. Seit Runde 34 liegt das Tor nicht mehr im\n"
           "   Schuss-Pfad, die Zeile trifft wieder und ist damit KEINE Warnung mehr — die\n"
           "   Kriech-Frage haengt jetzt an der Haltungsklasse, nicht an der Box.)\n");

    if (!riegel) return 0;

    int fail = 0;
    /* RIEGEL 1: die neue Haltungsklasse. Faellt sie, ist re2d_hitbox kaputt. */
    if (t1 != 0) { printf("RIEGEL-FAIL: Haltungsklasse nicht gemessen\n"); fail = 1; }
    if (lie <= 0) {
        printf("RIEGEL-FAIL: der Hund war in 400 Bildern nie gestaucht - der Riegel prueft nichts\n");
        fail = 1; }
    if (stand <= 0) {
        printf("RIEGEL-FAIL: der Hund war in 400 Bildern nie in der vollen Box\n"); fail = 1; }
    if (bad != 0) {
        printf("RIEGEL-FAIL: %d Bilder mit falscher Haltungsklasse (FUN_80104088 @0x80104090-D8)\n",
               bad); fail = 1; }
    /* RIEGEL 2: kein LIVE-Typ dauerhaft untreffbar (Runde-13/14-Falle). Zeile 1 ist die
     * erzwungene Kriecher-Box und wird unten getrennt geprueft. */
    {   static const int LIVE[5] = { 0, 2, 3, 4, 5 };
        for (int k = 0; k < 5; k++) {
            int i = LIVE[k];
            if (tb[i] == 0) { printf("RIEGEL-FAIL: Typ #%d ist in 900 Bildern NIE getroffen worden\n", i);
                              fail = 1; } } }
    /* ⛔ RIEGEL 2b GESTRICHEN (Runde 34), und hier steht warum statt einer angepassten Zahl:
     * er nagelte "erzwungene Kriecher-Box -350/350 -> 0 Treffer" fest. Diese 0 kam
     * AUSSCHLIESSLICH vom fuenften Tor (Fenster [-100,800) gegen Muendung ~1665). Das Tor
     * liegt nicht mehr im Schuss-Pfad (FUN_800470C0 hat nur die Aufrufstellen @0x80042F94
     * Messer und @0x800467C0 Bolzen; jede Schusswaffe geht ueber `jal 0x800410CC`
     * @0x80043AFC), also misst die Zeile nichts mehr ueber den Schuss. Gemessen steht sie
     * jetzt auf 82 Treffern = derselbe Wert wie die Zeile ohne erzwungene Box. Die Schranke
     * wurde NICHT auf 82 "nachgezogen" — sie ist ersatzlos weg und durch die Tor-Zaehler
     * unten ersetzt, die den MECHANISMUS pruefen statt eine Nebenwirkung.
     *
     * RIEGEL 3 (Runde 34 UMGEDREHT): die Box-Kontrolle beweist jetzt das Gegenteil. Das
     * kuenstliche Nullen von +0x98/+0x9E darf die Trefferzahl NICHT mehr veraendern, und das
     * Tor darf kein einziges Urteil faellen. */
    if (kz_g5 != 0u || kh_g5 != 0u) {
        printf("RIEGEL-FAIL: das fuenfte Tor faellte beim SCHUSS noch Urteile (Zombie %u, "
               "Hund %u) - es gehoert nur zum Messer (@0x80042F94)\n", kz_g5, kh_g5);
        fail = 1; }
    if (kz <= 0 || kh <= 0) {
        printf("RIEGEL-FAIL: mit genullter Box fiel gar kein Schaden mehr (Zombie %d, Hund %d)"
               " - dann haengt der Schuss doch am Tor\n", kz, kh);
        fail = 1; }
    /* RIEGEL 3b: DIE KLASSEN-KONTROLLE. Klasse 4 (nur Bit 2) faellt an der EBEN-Zeile 3
     * `2,0,0` @0x800A6DB4+9+3 durch -> beide Applier-Typen MUESSEN auf 0 fallen, und die
     * Klasse muss ueberhaupt gelesen worden sein. Faellt das nicht, laeuft der Applier an
     * der Messung vorbei und alle Zahlen oben sagen nichts. */
    if (kz4 != 0) {
        printf("RIEGEL-FAIL: KONTROLLE Zombie mit Klasse 4 traf %d mal - der Applier liegt "
               "nicht im gemessenen Pfad\n", kz4); fail = 1; }
    if (kh4 != 0) {
        printf("RIEGEL-FAIL: KONTROLLE Hund mit Klasse 4 traf %d mal - s.o.\n", kh4); fail = 1; }
    if (kz4_cls == 0u || kh4_cls == 0u) {
        printf("RIEGEL-FAIL: Haltungsklasse nie gelesen (Zombie %u, Hund %u)\n",
               kz4_cls, kh4_cls); fail = 1; }
    /* ===== RIEGEL 4: DER NUTZER-BEFUND, UEBER DEN NEUEN MECHANISMUS ======================
     * getroffen -> liegt -> NICHT treffbar -> steht auf -> wieder treffbar, als Bilderzahl. */
    if (b_kette < 0) {
        printf("RIEGEL-FAIL: die Hunde-Kette wurde nicht durchlaufen - Riegel 4 prueft nichts\n");
        fail = 1; }
    if (b_stand_hits <= 0) {
        printf("RIEGEL-FAIL: der STEHENDE Hund wurde nicht getroffen (Ueberkorrektur)\n");
        fail = 1; }
    if (b_lie_frames <= 0) {
        printf("RIEGEL-FAIL: der Hund lag in keinem Bild - Riegel 4 prueft nichts\n"); fail = 1; }
    if (b_lie_hits != 0) {
        printf("RIEGEL-FAIL: der LIEGENDE Hund wurde %d mal getroffen (erwartet 0)\n",
               b_lie_hits); fail = 1; }
    if (b_up_hits <= 0) {
        printf("RIEGEL-FAIL: nach dem Aufstehen faellt kein Schaden (%d in %d Bildern) - das "
               "waere die Dauersperre\n", b_up_hits, b_up_frames); fail = 1; }
    if (b_cls_lie_bad != 0) {
        printf("RIEGEL-FAIL: waehrend er lag trug die Klasse in %d Bildern nicht genau 1 "
               "(FUN_80104088(0) @0x80104090-B4)\n", b_cls_lie_bad); fail = 1; }
    if (b_cls_up_ok <= 0) {
        printf("RIEGEL-FAIL: nach dem Aufstehen trug die Klasse in keinem Bild Bit 1 "
               "(FUN_80104088(1) @0x801040CC) - dann traegt der Befund einen anderen Grund\n");
        fail = 1; }
    if (b_g5 != 0u) {
        printf("RIEGEL-FAIL: waehrend der Hunde-Kette faellte das fuenfte Tor %u Urteile\n",
               b_g5); fail = 1; }
    if (b_cls == 0u) {
        printf("RIEGEL-FAIL: waehrend der Hunde-Kette wurde die Klasse nie gelesen - der "
               "Befund haengt dann an einem anderen Mechanismus\n"); fail = 1; }
    /* ===== RIEGEL 5: das Tor ist UMGEZOGEN, nicht geloescht ==============================
     * Mit dem Messer MUSS es Urteile faellen (@0x80042F94), und das Messer muss treffen. */
    for (int i = 0; i < 2; i++) {
        if (m_hits[i] < 0) {
            printf("RIEGEL-FAIL: Messer-Lauf %d gar nicht gelaufen\n", i); fail = 1; continue; }
        if (m_hits[i] == 0) {
            printf("RIEGEL-FAIL: das MESSER traf Ziel %d in 300 Bildern nicht - der Keil "
                   "{-200,0,250,125} @0x8001101C bzw. die Reichweite ist kaputt\n", i);
            fail = 1; }
        if (m_g5[i] == 0u) {
            printf("RIEGEL-FAIL: beim MESSER faellte das fuenfte Tor kein Urteil (Ziel %d) - "
                   "es ist geloescht statt umgezogen (@0x80042F94)\n", i); fail = 1; }
    }
    printf(fail ? "PROBE-FAIL\n" : "PROBE-OK\n");
    return fail;
}
