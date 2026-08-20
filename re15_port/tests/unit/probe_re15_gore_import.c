/* probe_re15_gore_import.c — MESSUNG: reisst die PISTOLE im RE1.5-KI-Modus wirklich ein Bein ab?
 *
 * NUTZER-AUFTRAG 2026-08-20 (woertlich): "Bei RE2 AI und RE2 AI + Model ist es so, dass auch mit
 * der Handfeuerwaffe Gliedmassen abgeschossen werden koennen. Bei RE1.5 AI noch nicht. Und genau
 * das ... moechte ich auch in RE1.5 AI haben."
 *
 * Diese Sonde misst NICHT synthetisch: sie faehrt game_step() mit Pad-Eingaben (R1 halten ->
 * SQUARE) in ROOM1140 mit den echten Sce_em_set-Spawns und protokolliert JEDEN Treffer mit
 *   Frame | Trefferzahl | HP | Zonen-Reserve +0x152 | Trefferrichtung +0x1D0 | +0x21A
 *   | Part-Flagwoerter 9..14 | Stumpf-Mesh | Zaehlerstand des RE2-PRNG (re15_re2_rand_draws)
 * und laesst am Ende die ANZEIGE-BRUECKE (re15_re2z_gore_resolve, Zwilling FUN_80027160
 * @0x80027160) sagen, welche Meshes der Renderer danach noch zeichnet.
 *
 * ---- WARUM DER SPIELER UM DEN ZOMBIE HERUMGESETZT WIRD ---------------------------------------
 * Das Dreifach-Gate des Zerlegers @0x80105288-B0 verlangt einen SEITENTREFFER:
 *     +0x1D0 & 0xC0 != 0   (@0x801052A4-B0)
 * und +0x1D0 entsteht aus `d = Peilung(Spieler->Zombie) - Zombie-Yaw` (@0x80041A08):
 *     Bit 0x20  ((d+1024)&0xfff) < 2048   = RUECKEN
 *     Bit 0x40  ((d+1536)&0xfff) < 1024   = SEITE A     (@0x80041A30-58)
 *     Bit 0x80  ((d- 512)&0xfff) < 1024   = SEITE B     (@0x80041A5C-84)
 * Ein Zombie, der den Spieler ANSIEHT, liefert d == 2048 und damit KEIN einziges Bit — frontal
 * kann per Konstruktion nie ein Bein abgehen, in RE2 wie im Port. Die Sonde stellt den Spieler
 * deshalb jeden Frame auf die FLANKE (Suche ueber 64 Winkel, dieselbe Peilungsformel wie der
 * Applier) — Harness-Positionierung wie das track() in probe_re2_hp_model.c; der SCHUSS-WEG
 * (Aim-FSM + game_step + re15_player_weapon_fire) bleibt echt.
 *
 * Kein add_test — reine Messsonde (die Zahlen wandern in den Report/PIN).
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
#include "re15_ems.h"
#include "re2_ems.h"
#include "re15_skeleton.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef RE15_ASSET_PSX_DIR
#define RE15_ASSET_PSX_DIR "shared_assets/PSX"
#endif

extern void     re15_player_aim_reset(void);
extern void     re15_player_set_aim_clip_len(int fc);
extern int16_t  re15_atan2_q12(int32_t dz, int32_t dx);
extern uint32_t re15_re2_rand_draws(void);   /* Port-Diagnose: kumulativer RE2-PRNG-Wurfzaehler */

static re15_rdt_t         s_rdt;
static int                s_room = 0x1140;
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

/* Die ECHTE RE1.5-Gegner-Bank aus EMD/CDEMD0.EMS (headless gibt es keinen pc_enemy_load) —
 * damit die Anzeige-Bruecke unten gegen die RICHTIGE Knochenzahl/Elternkette laeuft und die
 * Stumpf-Grenze (RE1.5-MD1 hat kein Mesh 15) messbar statt behauptet ist. */
static uint8_t *s_emd_blob[0x40];
static re15_enemy_bank_t *load_re15_bank(const char *base, uint8_t type)
{
    re15_enemy_bank_t *eb = re15_enemy_find(type);
    if (eb && eb->ok) return eb;
    char emsp[600]; size_t ems_size = 0;
    snprintf(emsp, sizeof emsp, "%s/EMD/CDEMD0.EMS", base);
    uint8_t *ems = slurp(emsp, &ems_size);
    if (!ems) return NULL;
    int idx = re15_ems_index_for_type(type);
    size_t off = 0, len = 0;
    if (idx < 0 || re15_ems_get_entry(ems, ems_size, idx, &off, &len) != 0) { free(ems); return NULL; }
    uint8_t *blob = (uint8_t *)malloc(len);
    if (!blob) { free(ems); return NULL; }
    memcpy(blob, ems + off, len);
    free(ems);
    if (type < 0x40) { free(s_emd_blob[type]); s_emd_blob[type] = blob; }
    if (!eb) eb = re15_enemy_alloc(type);
    if (!eb) return NULL;
    if (re15_emd_parse_container(blob, len, &eb->md1, &eb->skel, &eb->anim, NULL) != 0) return NULL;
    eb->ok = 1; eb->buf = NULL;
    return eb;
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
    re15_damage_seed_rng(0x0badf00du);
    g_current_room_id = (uint16_t)s_room;
    if (s_rdt.main_scd)   scd_thread_start(0, s_rdt.main_scd);
    if (s_rdt.sub_scd[0]) scd_thread_start(1, s_rdt.sub_scd[0]);
    /* ROOM1190 stellt seine Hunde ueber einen Sub-Thread auf (wie probe_re2_hp_model). */
    if (s_room == 0x1190 && s_rdt.sub_scd_count > 13 && s_rdt.sub_scd[13])
        scd_thread_start(2, s_rdt.sub_scd[13]);
    g_scd.work_vars[10] = 0;
    for (int i = 0; i < 120; i++) scd_vm_tick();
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100; pl->hit_react = 0;
    pl->state = 0; pl->motion = 0; pl->floor = 0; pl->y = 0;
    re15_collision_set_band(0);
    re15_player_set_aim_clip_len(12);
}

/* Peilung Spieler->Zombie in derselben Rechnung wie der Applier (@0x800419D8-A08). */
static int bearing_to(const re15_actor_t *e, const re15_actor_t *pl)
{
    return ((int)re15_atan2_q12(e->z - pl->z, e->x - pl->x) - 1024) & 0xfff;
}

/* Stellt den Spieler so um den Zombie, dass +0x1D0 das gewuenschte Bit bekommt.
 * want: 0x40/0x80 = Flanke (Zerleger-Gate), 0x20 = Ruecken, 0 = frontal (Gegenprobe).
 * ⚠ Es wird die MITTE des jeweiligen Fensters angepeilt, nicht der erste passende Winkel: der
 * Zombie dreht sich zwischen Platzierung und Schuss weiter, und am Fensterrand kippt das Bit
 * dann wieder heraus (in der ersten Messung genau so passiert). Fenstermitten aus
 * @0x80041A0C-84: 0x20 -> d = 0, 0x40 -> d = 3072, 0x80 -> d = 1024, frontal -> d = 2048. */
static void place(int slot, int32_t back, unsigned want)
{
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    re15_actor_t *e  = &g_actors[slot];
    int target = (want == 0x40u) ? 3072 : (want == 0x80u) ? 1024 : (want == 0x20u) ? 0 : 2048;
    int best = 0, best_err = 1 << 30;
    for (int k = 0; k < 64; k++) {
        int a = (k * 64) & 0xfff;
        int32_t px = e->x + (int32_t)(((int64_t)re15_cos_q12(a) * back) >> 12);
        int32_t pz = e->z + (int32_t)(((int64_t)re15_sin_q12(a) * back) >> 12);
        re15_actor_t probe = *pl; probe.x = px; probe.z = pz;
        int d   = (bearing_to(e, &probe) - ((int)e->rot_y & 0xfff)) & 0xfff;
        int err = (d - target) & 0xfff;
        if (err > 2048) err = 4096 - err;
        if (err < best_err) { best_err = err; best = a; }
    }
    pl->x = e->x + (int32_t)(((int64_t)re15_cos_q12(best) * back) >> 12);
    pl->z = e->z + (int32_t)(((int64_t)re15_sin_q12(best) * back) >> 12);
    pl->y = e->y;
    pl->rot_y = (int16_t)bearing_to(e, pl);
}

static int pick_enemy(int want_type)
{
    for (int s = 1; s < RE15_ACTOR_MAX; s++)
        if (g_actors[s].active && g_actors[s].type >= 0x10 && g_actors[s].type < 0x40
            && !(g_actors[s].grid_id & 0x80)
            && (want_type < 0 || g_actors[s].type == (uint8_t)want_type)) return s;
    return -1;
}

static void dump_parts(const re15_actor_t *e, const char *tag)
{
    printf("      %-10s flags:", tag);
    for (int i = 0; i < 15; i++) printf(" %d:%04X", i, (unsigned)e->re2z_part_flags[i]);
    printf("\n      %-10s mesh :", "");
    for (int i = 0; i < 15; i++) printf(" %d:%2u", i, (unsigned)e->re2z_part_mesh[i]);
    printf("\n");
}

/* EIN Lauf: Waffe `weapon`, Trefferrichtung `want`, Gegnertyp `want_type`. */
static void run(int flavor, int import_on, int weapon, unsigned want, int want_type,
                const char *tag)
{
    re15_ai_flavor_set((re15_ai_flavor_t)flavor);
    re15_re15_re2z_import_set(import_on);
    re15_re2_damage_model_set(1);
    bringup();
    re15_inv_load_briefing();
    if (weapon >= 3) { g_inv.slots[1].id = (uint8_t)weapon; g_inv.slots[1].qty = 250; }
    re15_player_set_equipped_weapon(weapon);

    for (int f = 0; f < 60; f++) { g_actors[RE15_ACTOR_SLOT_PLAYER].hp = 100; frame(0, 0); }
    int slot = pick_enemy(want_type);
    if (slot < 0) { printf("  %s: kein Gegner\n", tag); return; }
    for (int s = 1; s < RE15_ACTOR_MAX; s++) if (s != slot) g_actors[s].active = 0;

    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    re15_actor_t *e  = &g_actors[slot];
    const int32_t back = (weapon < 3) ? 900 : 2600;
    pl->hp = 100;
    re15_player_cmd_reset(); re15_player_aim_reset(); re15_player_set_aim_clip_len(12);
    place(slot, back, want);

    printf("\n  --- %s (typ 0x%02X, Waffe %d, Richtung 0x%02X) ---\n", tag, e->type, weapon, want);
    printf("      Start: hp=%d  +0x152=%d  +0x1D0=0x%04X  +0x21A=0x%04X  gore_active=%d\n",
           e->hp, (int)e->re2z_pool152, (unsigned)e->re2z_hitdir1d0,
           (unsigned)e->re2z_flags21a, re15_re2z_gore_active(e));

    for (int f = 0; f < 40 && !re15_player_aim_ready(); f++) {
        pl->hp = 100; place(slot, back, want); frame(RE15_PAD_BIT_R1, 0);
    }

    int hits = 0, hp_last = e->hp, dismember_frame = -1, dismember_shin = -1;
    uint32_t draws_before_dismember = 0, draws_at_dismember = 0;
    for (int f = 0; f < 2500; f++) {
        pl->hp = 100;
        place(slot, back, want);
        uint16_t f21a_before = e->re2z_flags21a;
        uint32_t draws0 = re15_re2_rand_draws();
        int on = ((f >> 1) & 1) == 0;
        frame((uint16_t)(RE15_PAD_BIT_R1 | (on ? RE15_PAD_BIT_SQUARE : 0)),
              (uint16_t)(on && ((f & 3) == 0) ? RE15_PAD_BIT_SQUARE : 0));
        if (e->hp < hp_last) {
            hits++;
            printf("      Treffer %2d @F%4d: hp=%4d  +0x152=%3d  +0x1D0=0x%04X  +0x21A=0x%04X"
                   "  RNG-Wuerfe=%u\n",
                   hits, f, e->hp, (int)e->re2z_pool152, (unsigned)e->re2z_hitdir1d0,
                   (unsigned)e->re2z_flags21a, (unsigned)(re15_re2_rand_draws() - draws0));
        }
        if (dismember_frame < 0 && ((e->re2z_flags21a & 0x60u) != (f21a_before & 0x60u))) {
            dismember_frame = f;
            dismember_shin  = (e->re2z_flags21a & 0x40u) ? 13 : 10;
            draws_before_dismember = draws0;
            draws_at_dismember     = re15_re2_rand_draws();
            printf("      >>> ZERLEGER @0x80105288 hat gezuendet, Frame %d, Treffer %d\n", f, hits);
            printf("          Seite: +0x21A |= 0x%02X -> Oberschenkel Part %d, Schienbein Part %d\n",
                   (e->re2z_flags21a & 0x40u) ? 0x40 : 0x20,
                   (e->re2z_flags21a & 0x40u) ? 12 : 9, dismember_shin);
            printf("          RNG: Wuerfe %u..%u in diesem Frame (der Zerleger macht 3: "
                   "Stumpf-Wurf @0x801052F8, 2x Emitter-Winkel @0x80105354/@0x801053BC)\n",
                   (unsigned)draws_before_dismember, (unsigned)draws_at_dismember);
            dump_parts(e, "nach");
        }
        hp_last = e->hp;
        if (e->hp < 0 || e->state == 3 || e->state == 7) break;
        if (dismember_frame >= 0 && f > dismember_frame + 200) break;
    }
    if (dismember_frame < 0) {
        printf("      KEIN Abriss in 2500 Frames (Treffer: %d, +0x152=%d, +0x1D0=0x%04X)\n",
               hits, (int)e->re2z_pool152, (unsigned)e->re2z_hitdir1d0);
        return;
    }

    /* --- was sieht der RENDERER? (die Bruecke, Zwilling FUN_80027160) ------------------------ */
    {
        const char *envb2 = getenv("RE15_ASSET_DIR");
        (void)load_re15_bank((envb2 && *envb2) ? envb2 : RE15_ASSET_PSX_DIR, e->type);
    }
    re15_enemy_bank_t *b = re15_enemy_find(e->type);
    if (b && b->ok && b->skel.bone_count > 0) {
        uint8_t draw[RE15_EMD_MAX_BONES]; uint32_t tint[RE15_EMD_MAX_BONES];
        uint8_t mesh[RE15_EMD_MAX_BONES];
        int nb = b->skel.bone_count < b->md1.mesh_count ? b->skel.bone_count : b->md1.mesh_count;
        printf("      RE1.5-Bank typ 0x%02X: %d Knochen, %d Meshes; Elternkette:",
               e->type, b->skel.bone_count, b->md1.mesh_count);
        for (int i = 0; i < b->skel.bone_count; i++)
            printf(" %d<-%d", i, (int)b->skel.bone_parent[i]);
        printf("\n");
        /* ⚠ NUMMERNRAEUME: `dismember_shin` ist eine RE2-PART-Nummer, der Renderer (und damit
         * re15_re2z_gore_part_matrix) indiziert mit BONE-SLOTS der geladenen Bank. Im
         * RE1.5-Modus uebersetzt die Hybrid-Permutation re2_hybrid_perm (RE2-Slot ->
         * RE1.5-Index); im RE2-Modus ist sie die Identitaet. */
        const int8_t *perm = NULL;
        int np = (re15_ai_flavor() == RE15_AI_FLAVOR_RE15)
               ? re2_hybrid_perm((int)e->type, &perm) : 0;
        int shin_bone = (np > dismember_shin && perm && perm[dismember_shin] >= 0)
                      ? (int)perm[dismember_shin] : dismember_shin;
        printf("      abgerissenes Teil: RE2-Part %d -> Bank-Bone %d (Perm %s)\n",
               dismember_shin, shin_bone, np ? "k_perm_zombie" : "Identitaet");
        /* Ein paar Frames weiter laufen lassen, damit die Flugphysik FUN_80028AD8 den zweiten
         * Bodenkontakt erreicht (`and v0,v0,-2` @0x80028CA0 = die Kaskade). */
        for (int f = 0; f < 240; f++) {
            pl->hp = 100; place(slot, back, want); frame(RE15_PAD_BIT_R1, 0);
            int32_t rot[9], tr[3];
            for (int k = 0; k < 9; k++) rot[k] = (k % 4 == 0) ? 4096 : 0;
            tr[0] = e->x; tr[1] = e->y; tr[2] = e->z;
            (void)re15_re2z_gore_part_matrix(e, shin_bone, (uint32_t)(1000 + f), rot, tr);
        }
        int on2 = re15_re2z_gore_resolve(e, b->skel.bone_parent, nb, draw, tint, mesh);
        printf("      Bruecke aktiv=%d -> gezeichnet:", on2);
        for (int i = 0; i < nb; i++) printf(" %d:%s", i, draw[i] ? "ja" : "NEIN");
        printf("\n");
        dump_parts(e, "final");
    }
}

static int load_room(const char *base, const char *sub, int room_id)
{
    char path[600];
    snprintf(path, sizeof path, "%s/%s", base, sub);
    size_t sz = 0; uint8_t *buf = slurp(path, &sz);
    if (!buf) { printf("RDT fehlt: %s\n", path); return 0; }
    if (re15_rdt_parse(buf, sz, &s_rdt) != 0) { printf("RDT-Parse: %s\n", path); return 0; }
    s_room = room_id;
    return 1;
}

int main(void)
{
    const char *envb = getenv("RE15_ASSET_DIR");
    const char *base = (envb && *envb) ? envb : RE15_ASSET_PSX_DIR;
    memset(&s_cam, 0, sizeof s_cam); memset(&s_ctx, 0, sizeof s_ctx);
    s_ctx.rdt = &s_rdt; s_ctx.rdt_ok = 1; s_ctx.cam_view = &s_cam; s_ctx.active_cut = 0;

    if (load_room(base, "STAGE1/ROOM1140.RDT", 0x1140)) {
        printf("\n===== RE1.5-KI-MODUS, Port-Option AN — PISTOLE (Waffe 3), FLANKE =====\n");
        run(RE15_AI_FLAVOR_RE15, 1, 3, 0x40, 0x10, "RE1.5 + Import");

        printf("\n===== GEGENPROBE 1: RE1.5-KI-Modus, Port-Option AUS =====\n");
        run(RE15_AI_FLAVOR_RE15, 0, 3, 0x40, 0x10, "RE1.5 ohne Import");

        printf("\n===== GEGENPROBE 2: RE1.5 + Import, aber FRONTAL (kein 0xC0-Bit) =====\n");
        run(RE15_AI_FLAVOR_RE15, 1, 3, 0x00, 0x10, "RE1.5 frontal");

        printf("\n===== VERGLEICH: RE2-KI-Modus, PISTOLE, FLANKE =====\n");
        run(RE15_AI_FLAVOR_RE2, 1, 3, 0x40, 0x10, "RE2-Modus");
    }
    if (load_room(base, "STAGE1/ROOM1190.RDT", 0x1190)) {
        printf("\n===== REGRESSIONSWACHE: HUND 0x20 im RE1.5-Modus mit Import AN =====\n");
        run(RE15_AI_FLAVOR_RE15, 1, 3, 0x40, 0x20, "RE1.5 Hund");
    }
    return 0;
}
