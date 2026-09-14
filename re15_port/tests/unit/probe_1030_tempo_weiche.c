/* probe_1030_tempo_weiche.c — ROOM1030 / Runde 12: WO genau faellt die Weiche, seit der
 * RE2-Init-Setzer B (@0x801008D8-0x80100950) jedem dritten Zombie das Gangtempo-Bit
 * +0x21A |= 0x8000 gibt?
 *
 * Diese Sonde MISST nur (sie aendert keinen Engine-Code) und faehrt drei Laeufe:
 *   (A) RE2 wie ausgeliefert            — Setzer B scharf
 *   (B) RE2, Tempo-Bit VOR jedem Tick geloescht — dieselbe Zufallsfolge (beide Zuege
 *       @0x801008F0/F8 und der dritte @0x8010093C werden weiterhin gezogen, nur die
 *       WIRKUNG des Bits ist weg), also eine saubere Ein-Variablen-Probe
 *   (C) RE1.5-Geschmack                 — dieselbe Stelle unter der Original-KI
 *
 * Gemessen wird je Zombie:
 *   +0x21A & 0x8000 (Tempo), +0x223 (Flinch-Reserve), Start (x,z,yaw,clip),
 *   erster Frame mit Stempel +0x0B == 5 (AOT-Zone 5 = der Streifen SUEDLICH des Tores,
 *   Aot_set id 5 @Datei 0x1cf2: x[-12900..-3700] z[-25300..-24200]),
 *   erster Frame mit +0x1C4 & 0x1000 (= der Kriech-Befehl, den sub07 @Datei 0x2754 bzw.
 *   sub09 @Datei 0x2804-16 schreibt), erster Frame mit SCA-Maske 8, Endposition.
 * Dazu die Skript-Budget-Zaehler work_vars[7] (sub03 zaehlt Member15==6) und
 * work_vars[5], plus die Flag-Bank 5. */
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

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef RE15_ASSET_PSX_DIR
#define RE15_ASSET_PSX_DIR "shared_assets/PSX"
#endif

#define GATE_Z_NORTH (-22771)      /* Oberkante Torband #28: -24420 + 1649 */
#define MAXF 3000

static re15_rdt_t         s_rdt;
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

static uint8_t *s_re2_ems = NULL; static size_t s_re2_n = 0;
static int load_bank_re2(uint8_t type)
{
    if (!s_re2_ems) s_re2_ems = slurp(RE15_ASSET_PSX_DIR "/../RE2/CDEMD0.EMS", &s_re2_n);
    if (!s_re2_ems) return 0;
    re15_enemy_bank_t *eb = re15_enemy_find(type);
    if (eb && eb->ok) return 1;
    if (!eb) eb = re15_enemy_alloc(type);
    if (!eb) return 0;
    if (re2_ems_load_bank(s_re2_ems, s_re2_n, (int)type, eb, NULL) == 0) {
        eb->buf = NULL; eb->ok = 1; return 1;
    }
    eb->type = 0; return 0;
}

static uint8_t *s_re15_ems = NULL; static size_t s_re15_n = 0;
static uint8_t  s_blob[0x80000];
static int load_bank_re15(uint8_t type)
{
    if (!s_re15_ems) s_re15_ems = slurp(RE15_ASSET_PSX_DIR "/EMD/CDEMD0.EMS", &s_re15_n);
    if (!s_re15_ems) return 0;
    re15_enemy_bank_t *eb = re15_enemy_find(type);
    if (eb && eb->ok) return 1;
    int idx = re15_ems_index_for_type(type);
    size_t off = 0, len = 0;
    if (idx < 0 || re15_ems_get_entry(s_re15_ems, s_re15_n, idx, &off, &len) != 0) return 0;
    if (len > sizeof s_blob) return 0;
    if (!eb) eb = re15_enemy_alloc(type);
    if (!eb) return 0;
    memcpy(s_blob, s_re15_ems + off, len);
    re15_tim_t tim = (re15_tim_t){0};
    if (re15_emd_parse_container(s_blob, len, &eb->md1, &eb->skel, &eb->anim, &tim) != 0) {
        eb->type = 0; return 0;
    }
    eb->ok = 1; eb->buf = NULL;
    re15_emd_parse_own_bank(s_blob, len, &eb->skel_own, &eb->anim_own);
    eb->own_ok = (eb->anim_own.clip_count > 0);
    eb->loco_ok = (re15_emd_parse_loco_bank(s_blob, len, &eb->skel_loco, &eb->anim_loco) == 0);
    return 1;
}

static void frame_step(void)
{
    const unsigned char *raw; int len, id;
    re15_msg_tick(&raw, &len, &id);
    s_ctx.pad_current = 0; s_ctx.pad_pressed = 0;
    scd_vm_tick();
    re15_game_step(&s_ctx);
}

static int is_zombie(const re15_actor_t *e)
{
    return e->active && (e->type == 0x10 || e->type == 0x11 || e->type == 0x12 ||
                         e->type == 0x16 || e->type == 0x18);
}

typedef struct {
    uint16_t tempo;       /* +0x21A & 0x8000 nach dem INIT-Tick */
    int8_t   res223;
    int32_t  x0, z0; int16_t ry0; int16_t clip0;
    int      f_zone5;     /* erster Frame Stempel +0x0B == 5   */
    int      f_zone6;     /* erster Frame Stempel +0x0B == 6   */
    int      f_arm;       /* erster Frame +0x1C4 & 0x1000      */
    int      f_mask8;     /* erster Frame SCA-Maske 8          */
    int      f_geo5;      /* erster Frame geometrisch im AOT-5-Rechteck */
    int32_t  xe, ze;
    int      crossed;
    double   path;
    int      n_zone5;
} slotinfo_t;

static slotinfo_t s_si[RE15_ACTOR_MAX];
static int        s_live[RE15_ACTOR_MAX];

/* Zeitleiste: wann sprang work_vars[7] (= sub03-Zaehlung Member15==6)? */
static int16_t s_w7[MAXF], s_w5[MAXF];
static uint8_t s_armcount[MAXF];

static int  s_burn = 0;        /* k Zuege VOR dem ersten Tick verbrennen = Strom verschieben */
static int  s_armslot = -1;    /* Not-Scharfschaltung: diesem Slot +0x1C4 |= 0x1000 geben */
static int  s_armframe = -1;
static int32_t s_nudge_x = 0;   /* zusaetzlich: im selben Frame nach OSTEN setzen */
static int  s_quiet = 0;
static int  s_mask_zone6_from = -1;   /* ab diesem Frame: Stempel +0x0B==6 NACH dem Tick auf 0
                                       * setzen -> sub03 zaehlt sie nicht mehr, work_vars[7]
                                       * faellt unter 4, das Tor `Cmp(work7,<,4)` @Datei 0x21cc
                                       * geht wieder auf. Testet GENAU dieses Tor. */
static int  s_out_stuck = -1, s_out_armed = 0, s_out_crossed = 0;
static int  s_out_order[8];

static void run(re15_ai_flavor_t fl, int kill_tempo, const char *name, int frames)
{
    memset(&s_cam, 0, sizeof s_cam); memset(&s_ctx, 0, sizeof s_ctx);
    s_ctx.rdt = &s_rdt; s_ctx.rdt_ok = 1; s_ctx.cam_view = &s_cam; s_ctx.active_cut = 0;

    re15_ai_flavor_set(fl);
    re15_actor_init(); re15_aot_init(); scd_vm_init();
    re15_enemy_reset(); re15_enemy_ai_set_paused(0);
    re15_player_cmd_reset();
    re15_damage_seed_rng(0x2545f491u);
    g_current_room_id = 0x1030;

    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100; pl->hit_react = 0;
    pl->state = 0; pl->motion = 0; pl->floor = 0; pl->y = 0;
    pl->x = -18050; pl->z = -8300;
    re15_collision_set_band(0);
    re15_inv_load_briefing();
    scd_room_reenter(&s_rdt, pl->x, pl->z, 0);

    if (fl == RE15_AI_FLAVOR_RE2) {
        (void)load_bank_re2(0x10); (void)load_bank_re2(0x11); (void)load_bank_re2(0x16);
    } else {
        (void)load_bank_re15(0x10); (void)load_bank_re15(0x11); (void)load_bank_re15(0x16);
    }

    for (int b = 0; b < s_burn; b++) (void)re15_re2_rand();

    memset(s_live, 0, sizeof s_live);
    memset(s_si, 0, sizeof s_si);
    for (int s = 1; s < RE15_ACTOR_MAX; s++) {
        if (!is_zombie(&g_actors[s])) continue;
        s_live[s] = 1;
        s_si[s].x0 = g_actors[s].x; s_si[s].z0 = g_actors[s].z;
        s_si[s].ry0 = g_actors[s].rot_y; s_si[s].clip0 = g_actors[s].motion;
        s_si[s].f_zone5 = s_si[s].f_zone6 = s_si[s].f_arm = s_si[s].f_mask8 = s_si[s].f_geo5 = -1;
    }

    if (!s_quiet) printf("\n=== %s ===\n", name);
    if (frames > MAXF) frames = MAXF;

    int32_t lx[RE15_ACTOR_MAX], lz[RE15_ACTOR_MAX];
    for (int s = 1; s < RE15_ACTOR_MAX; s++) { lx[s] = s_si[s].x0; lz[s] = s_si[s].z0; }

    for (int f = 0; f < frames; f++) {
        /* (B): das Tempo-Bit VOR dem Tick loeschen — ab Frame 1, damit der INIT-Zug
         * (@0x801008F0/F8/@0x8010093C) unveraendert stattfindet und nur die WIRKUNG fehlt. */
        if (kill_tempo && f > 0)
            for (int s = 1; s < RE15_ACTOR_MAX; s++)
                if (s_live[s]) g_actors[s].re2z_flags21a &= (uint16_t)~0x8000u;

        if (s_armslot > 0 && f == s_armframe && s_live[s_armslot]) {
            g_actors[s_armslot].anim_flags |= 0x1000u;
            if (s_nudge_x) g_actors[s_armslot].x = s_nudge_x;
        }

        frame_step();

        if (f == 0)
            for (int s = 1; s < RE15_ACTOR_MAX; s++)
                if (s_live[s]) {
                    s_si[s].tempo  = (uint16_t)(g_actors[s].re2z_flags21a & 0x8000u);
                    s_si[s].res223 = g_actors[s].re2z_res223;
                    s_si[s].clip0  = g_actors[s].motion;
                }

        s_w7[f] = g_scd.work_vars[7];
        s_w5[f] = g_scd.work_vars[5];
        int na = 0;
        for (int s = 1; s < RE15_ACTOR_MAX; s++) {
            if (!s_live[s]) continue;
            re15_actor_t *e = &g_actors[s];
            slotinfo_t *si = &s_si[s];
            double dx = (double)(e->x - lx[s]), dz = (double)(e->z - lz[s]);
            si->path += (dx < 0 ? -dx : dx) + (dz < 0 ? -dz : dz);
            lx[s] = e->x; lz[s] = e->z;
            if (e->member_0b == 5) { if (si->f_zone5 < 0) si->f_zone5 = f; si->n_zone5++; }
            if (e->member_0b == 6 && si->f_zone6 < 0) si->f_zone6 = f;
            if ((e->anim_flags & 0x1000u) && si->f_arm   < 0) si->f_arm   = f;
            if (e->sca_mask == 8            && si->f_mask8 < 0) si->f_mask8 = f;
            if (e->x >= -12900 && e->x <= -3700 && e->z >= -25300 && e->z <= -24200 &&
                si->f_geo5 < 0) si->f_geo5 = f;
            if (e->anim_flags & 0x1000u) na++;
            si->xe = e->x; si->ze = e->z;
            if (si->z0 <= GATE_Z_NORTH && e->z > GATE_Z_NORTH) si->crossed = 1;
        }
        s_armcount[f] = (uint8_t)na;
        if (s_mask_zone6_from >= 0 && f >= s_mask_zone6_from)
            for (int s = 1; s < RE15_ACTOR_MAX; s++)
                if (s_live[s] && g_actors[s].member_0b == 6) g_actors[s].member_0b = 0;
    }

    s_out_stuck = -1; s_out_armed = 0; s_out_crossed = 0;
    for (int s = 1; s < RE15_ACTOR_MAX; s++) {
        if (!s_live[s]) continue;
        if (s_si[s].f_arm >= 0) s_out_armed++;
        if (s_si[s].crossed) s_out_crossed++;
        else if (s_si[s].z0 <= GATE_Z_NORTH && s_out_stuck < 0) s_out_stuck = s;
    }
    {   /* Reihenfolge der Ankuenfte in AOT-5 (Stempel +0x0B == 5) */
        int n = 0;
        for (int rank = 0; rank < 6; rank++) {
            int best = -1;
            for (int s = 1; s < RE15_ACTOR_MAX; s++) {
                if (!s_live[s] || s_si[s].f_zone5 < 0) continue;
                int seen = 0;
                for (int i = 0; i < n; i++) if (s_out_order[i] == s) seen = 1;
                if (seen) continue;
                if (best < 0 || s_si[s].f_zone5 < s_si[best].f_zone5) best = s;
            }
            if (best < 0) break;
            s_out_order[n++] = best;
        }
        for (; n < 8; n++) s_out_order[n] = 0;
    }
    if (s_quiet) return;

    printf("  %-4s %-7s %-5s %-22s %-5s %-5s | %-7s %-7s %-7s %-7s %-7s | %-9s %s\n",
           "slot", "tempo", "+223", "start (x,z)", "yaw", "clip",
           "geo>Z5", "st.Z5", "st.Z6", "ARM", "maske8", "Weg", "Ende / Riegel");
    for (int s = 1; s < RE15_ACTOR_MAX; s++) {
        if (!s_live[s]) continue;
        slotinfo_t *si = &s_si[s];
        printf("  z%-3d %-7s %-5d (%7ld,%8ld) %-5d %-5d | %-7d %-7d %-7d %-7d %-7d | %-9.0f (%ld,%ld) %s\n",
               s, si->tempo ? "SCHNELL" : "normal", (int)si->res223,
               (long)si->x0, (long)si->z0, (int)si->ry0, (int)si->clip0,
               si->f_geo5, si->f_zone5, si->f_zone6, si->f_arm, si->f_mask8,
               si->path, (long)si->xe, (long)si->ze,
               si->crossed ? "DURCH" : (si->z0 <= GATE_Z_NORTH ? "*** STECKT ***" : "(nordstart)"));
    }

    printf("  Zeitleiste (nur Aenderungen): frame | armed | work7 (sub03: Member15==6) | work5\n");
    int pa = -1, p7 = -12345, p5 = -12345;
    for (int f = 0; f < frames; f++) {
        if (s_armcount[f] != pa || s_w7[f] != p7 || s_w5[f] != p5) {
            printf("    f%-5d armed=%d work7=%d work5=%d\n", f, s_armcount[f], s_w7[f], s_w5[f]);
            pa = s_armcount[f]; p7 = s_w7[f]; p5 = s_w5[f];
        }
        if (f > 1400) break;
    }
    printf("  Flag-Bank5: bits0..5=%d%d%d%d%d%d  0x14=%d 0x20=%d 0x21=%d 0x22=%d\n",
           re15_game_flag_get(5,0), re15_game_flag_get(5,1), re15_game_flag_get(5,2),
           re15_game_flag_get(5,3), re15_game_flag_get(5,4), re15_game_flag_get(5,5),
           re15_game_flag_get(5,0x14), re15_game_flag_get(5,0x20),
           re15_game_flag_get(5,0x21), re15_game_flag_get(5,0x22));
}

int main(void)
{
    printf("== ROOM1030: wo faellt die Tempo-Weiche? ==\n");
    size_t n = 0;
    uint8_t *buf = slurp(RE15_ASSET_PSX_DIR "/STAGE1/ROOM1030.RDT", &n);
    if (!buf) { printf("FAIL: ROOM1030.RDT nicht lesbar\n"); return 1; }
    if (re15_rdt_parse(buf, n, &g_room_rdt) != 0) { printf("FAIL: RDT-Parse\n"); return 1; }
    g_room_rdt_ok = 1;
    s_rdt = g_room_rdt;

    run(RE15_AI_FLAVOR_RE2,  0, "(A) RE2 wie ausgeliefert (Setzer B scharf)", 2000);
    re15_enemy_reset();
    run(RE15_AI_FLAVOR_RE2,  1, "(B) RE2, Tempo-Bit vor jedem Tick geloescht", 2000);
    re15_enemy_reset();
    run(RE15_AI_FLAVOR_RE15, 0, "(C) RE1.5-Geschmack", 2000);

    /* ---- (D) GEGENPROBE: den Nachzuegler von Hand scharfschalten ---------------------------
     * Wenn die Klemme NUR daran haengt, dass er nie den Kriech-Befehl bekam, muss er mit
     * +0x1C4 |= 0x1000 (genau das, was sub07 @Datei 0x2758 schreibt) durchkommen. */
    re15_enemy_reset();
    s_armslot = 3; s_armframe = 700;
    run(RE15_AI_FLAVOR_RE2, 0, "(D) RE2 + Not-Scharfschaltung von z3 in Frame 700", 3000);
    s_armslot = -1; s_armframe = -1;

    /* ---- (G) BEIDE FEHLER ZUSAMMEN AUFGEHOBEN: scharfschalten UND einmalig oestlich der
     * Wandkante -12166 setzen (Broad-Phase FUN_8003b0a4 @0x8003b284-94, r=400). Kommt er
     * dann durch, sind es GENAU zwei Ursachen: (1) das Skript-Budget, (2) die 29 Einheiten
     * zwischen Nav-Kreuzung -12195 und Wandkante -12166. */
    re15_enemy_reset();
    s_armslot = 3; s_armframe = 700; s_nudge_x = -12100;
    run(RE15_AI_FLAVOR_RE2, 0, "(G) RE2 + Scharfschaltung UND Ostversatz von z3 in Frame 700", 3000);
    s_armslot = -1; s_armframe = -1; s_nudge_x = 0;

    /* ---- (F) TOR-TEST: haelt wirklich `Cmp(work7,<,4)` (@Datei 0x21cc) den Nachzuegler
     * draussen? Ab Frame 560 wird der Zonenstempel +0x0B==6 nach jedem Tick auf 0 gesetzt;
     * sub03 (@Datei 0x220e, Member_cmp(15,==,6)) zaehlt dann niemanden mehr, work_vars[7]
     * faellt unter 4 und sub06 muss den spaeter ankommenden z3 scharfschalten. */
    re15_enemy_reset();
    s_mask_zone6_from = 560;
    run(RE15_AI_FLAVOR_RE2, 0, "(F) RE2, Budget-Tor kuenstlich offengehalten (work7 gedrueckt)", 2000);
    s_mask_zone6_from = -1;

    /* ---- (E) IST DIE IDENTITAET DES NACHZUEGLERS EINE EIGENSCHAFT DES ZUFALLSSTROMS? -------
     * Der Setzer B schiebt pro Zombie ZWEI (im Trefferzweig DREI) Zuege in den Strom. Hier
     * wird derselbe Effekt sauber isoliert: k Zuege VOR dem ersten Tick verbrennen, sonst
     * alles identisch. Aendert sich dabei, WER haengenbleibt, dann ist die Identitaet des
     * Nachzueglers eine Eigenschaft des Zufallsstroms — nicht des Tempo-Bits. */
    printf("\n=== (E) Strom-Verschiebung: k Vorab-Zuege (RE2) ===\n");
    printf("  %-3s %-6s %-8s %-9s %s\n", "k", "scharf", "durch", "steckt", "Ankunftsreihenfolge in AOT-5 (Slot@Frame)");
    for (int k = 0; k < 12; k++) {
        re15_enemy_reset();
        s_burn = k; s_quiet = 1;
        run(RE15_AI_FLAVOR_RE2, 0, "sweep", 1200);
        s_quiet = 0; s_burn = 0;
        printf("  %-3d %-6d %-8d z%-8d", k, s_out_armed, s_out_crossed, s_out_stuck);
        for (int i = 0; i < 6 && s_out_order[i]; i++)
            printf(" z%d@%d", s_out_order[i], s_si[s_out_order[i]].f_zone5);
        printf("\n");
    }

    free(buf);
    g_room_rdt_ok = 0;
    re15_ai_flavor_set(RE15_AI_FLAVOR_RE15);
    printf("\n== MESSUNG FERTIG ==\n");
    return 0;
}
