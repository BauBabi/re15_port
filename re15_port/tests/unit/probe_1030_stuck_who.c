/* probe_1030_stuck_who.c — PIN: WELCHER der sechs ROOM1030-Zombies bleibt beim Kriechen unter dem
 * Tor haengen, und WARUM? (Nutzer-Report 2026-08-21: "einer von sechs bleibt haengen" +
 * "manchmal haben sie eine Laufanimation, aber laufen nicht")
 *
 * ⛔ ERGEBNIS: BEIDES ist EIN Mechanismus, und beides ist im ORIGINAL genauso.
 *
 * Messung (echter Weg: echte ROOM1030.RDT in g_room_rdt, echte Sce_em_set-Spawns, echte SCD-VM
 * VOR re15_game_step, geladene RE2/RE1.5-Baenke inkl. LOCO; 6000 Frames):
 *   RE1.5 : 6 gespawnt, 5 scharfgeschaltet, 5 durch. Slot 6 haengt bei (-12685,-24838).
 *   RE2   : 6 gespawnt, 6 scharfgeschaltet, 5 durch. Slot 2 haengt bei (-12211,-24552),
 *           Weg 44096 gegen Netto 2630, ab f840 EXAKT periodisch (Periode 100 Frames).
 *
 * (1) WARUM RE1.5 nur 5 scharfschaltet — SKRIPT-BUDGET, kein Port-Fehler.
 *     sub02 ruft die Scharfschalt-Schleife sub06 nur unter `Cmp(work7, <, 4)`
 *     (`23 00 07 03 04 00` @Datei 0x21cc; Operator 3 = `<`, Original `slt v0,a0,a1`
 *     @0x8003ffe8). work7 ist die von sub03 (@Datei 0x220e, `Member_cmp(15,==,6)`) gezaehlte
 *     Zahl der Zombies in AOT-Zone 6 (der Lobby). Wer erst ankommt, nachdem vier drin sind,
 *     wird NIE mehr scharfgeschaltet: der Nachzuegler traegt den AOT-Stempel +0x0B == 5
 *     ueber 5301 Frames — und bekommt +0x1C4 & 0x1000 trotzdem nie.
 *
 * (2) WARUM einer nicht durchkommt — NAV-ZIEL LIEGT IN DER WAND.
 *     Nav-Kreuzung Zone0 -> Zone9 (die westliche Tor-Tuer), FUN_8003a31c:
 *       lo = max(-13470,-13070) = -13070, hi = min(-3660,-11320) = -11320   (@0x8003a3a0-dc)
 *       Mitte = (lo+hi)>>1 = -12195                                          (@0x8003a3f0-f8)
 *       Entity-Preference (2*box[6] Rand) liefert im Fenster (-12270,-12120) dasselbe.
 *     SCA-Zelle #30 (rgn3 idx8): typ 1, u0=0xFF (auch fuer Kriech-Maske 8 solide),
 *       x[-15518..-12566] z[-24134..-22694] — sie deckt die WESTLICHE Haelfte der Tuer ab.
 *     Broad-Phase FUN_8003b0a4 @0x8003b284-94 mit r = box[6] = 400 (@0x80100628 `lhu a1,6(v0)`;
 *     Box @STAGE1.BIN Datei 0x1f778 = {0,-1440,0,400,1440,400}) gibt erst ab x >= -12166 frei.
 *     => Das Nav-Ziel liegt 29 Einheiten INNERHALB der Wand und ist nie erreichbar.
 *     Der Aktor laeuft ewig dagegen: push_rect haelt ihn auf z = -24134-400-0x12 = -24552.
 *
 * (3) Warum das ORIGINAL nicht ausweicht: der einzige Ausweich-Zweig des Zombies ist
 *     FUN_80102058 @0x8010206c (`lbu a0,144(a1)` / `andi v0,a0,0x3` @0x80102074 /
 *     `beq -> normal` @0x80102078 -> sonst Zustandswort 0x901/0xA01 @0x801020a0-b0).
 *     Der Schreiber @0x8003b4c0-dc setzt +0x90 = ((FUN_8001bf04(...)>>4) + 8 + (cell.u1 & 3));
 *     FUN_8001bf04 liefert nur {0x000,0x400,0x800,0xc00,0xf00}, die Bits 0-1 kommen also
 *     AUSSCHLIESSLICH aus cell.u1 & 3. In ROOM1030 ist u1 == 0x00 in ALLEN 35 Zellen
 *     -> der Reroute kann auch im Original nicht feuern.
 *
 *     ⚠️ SEPARATE, HIER NICHT WIRKSAME LUECKE (Zensus ueber alle 240 RDTs / 20874 SCA-Zellen):
 *     `u1 & 3` ist in 40 RDTs (= 20 Raeumen) gesetzt, u.a. STAGE1 ROOM1090/10B0/1170/11A0/
 *     1200/1260 und STAGE2 2000-20B0. Dort IST der Ausweich-Zweig echter Inhalt — und der Port
 *     schreibt +0x90 im SCA-Klemmpfad gar nicht (collision_constrain_impl setzt nur die
 *     Position). Betroffene Original-Leser: Zombie @0x8010206c/@0x80102f30/@0x80103478/
 *     @0x80103518/@0x80105630, ZGirl @0x8010bca8/@0x8010bd58, Hund @0x8010e260/@0x8010e2b4/
 *     @0x8010e634/@0x8010e688/@0x80110384/@0x8011040c/@0x80110650, @0x80117f18/@0x80117fc8.
 *     Das ist ein EIGENER Befund fuer eine eigene Welle — er aendert ROOM1030 nachweislich
 *     nicht (u1 == 0) und wurde daher hier NICHT mitgefixt.
 *     Gegenprobe zum zweiten dort fehlenden Zweig (`u1 & 0x80` @0x8003b25c-60): dieses Bit ist
 *     in ALLEN 20874 Zellen des ausgelieferten Spiels 0 -> beweisbar toter Code, korrekt ignoriert.
 *
 * POSITIV-KONTROLLE (unten): derselbe Kriecher einmalig auf x = -12100 (oestlich der Kante)
 * -> 6/6 durch. Kriech-Gehirn, Torzelle (u0 = 0xF7 via `37 02 06 f7`/`37 03 06 f7`,
 * Handler LAB_8004175c `sb a1,9(v0)` @0x8004179c) und SCA-Maske 8 sind also in Ordnung;
 * es klemmt ausschliesslich die Geometrie.
 *
 * Riegel: SCA rgn2/idx6 == rgn3/idx6, Rechteck (-20144,-24420, 24264 x 1649) -> Oberkante
 * z = -24420 + 1649 = -22771. Noerdlich (Lobby) heisst z > -22771.
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

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef RE15_ASSET_PSX_DIR
#define RE15_ASSET_PSX_DIR "shared_assets/PSX"
#endif

#define GATE_Z_NORTH (-22771)
#define MAXF 6000

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
    return e->active && re15_re2z_owns_type(e->type);
}

typedef struct {
    int32_t x, z;
    int16_t sx, sz, ry;
    uint8_t sca, st, s1, s2, cflags, motion, afr;
    int8_t  cslot;
    uint16_t f10e;
    uint16_t a1c4;
    uint8_t  m0b;
    uint8_t  inaot5;
} rec_t;

static rec_t s_rec[RE15_ACTOR_MAX][MAXF];
static int   s_live[RE15_ACTOR_MAX];

/* ---- POSITIV-KONTROLLE ----------------------------------------------------------------------
 * Setzt EINMAL, in Frame `s_nudge_frame`, die X des Slots `s_nudge_slot` auf `s_nudge_x`.
 * Damit wird bewiesen, dass NUR die Geometrie klemmt: derselbe Kriecher, dasselbe Gehirn,
 * dieselbe Torzelle — nur oestlich der Wandkante -12166 statt westlich davon. */
static int     s_nudge_slot  = -1;
static int     s_nudge_frame = -1;
static int32_t s_nudge_x     = 0;

/* Ergebnis je Slot, damit main() pinnen kann. */
typedef struct {
    int spawned;
    int armed;        /* je Zombie: hat +0x1C4 & 0x1000 gesehen  */
    int crossed;      /* hat den Riegel nach Norden ueberquert   */
    int stuck_slot;   /* der suedlich gestartete Nicht-Ueberquerer (-1 = keiner) */
    double stuck_path, stuck_net;
    int stuck_stamp5; /* Frames mit +0x0B == 5 beim Steckengebliebenen */
    int stuck_armedf; /* Frame des ersten 0x1000 (-1 = nie)            */
} out_t;

static void run(re15_ai_flavor_t fl, const char *name, int32_t px, int32_t pz, int frames,
                out_t *out)
{
    if (out) memset(out, 0, sizeof *out);
    if (out) out->stuck_slot = -1;
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
    pl->x = px; pl->z = pz;
    re15_collision_set_band(0);
    re15_inv_load_briefing();
    scd_room_reenter(&s_rdt, pl->x, pl->z, 0);

    if (fl == RE15_AI_FLAVOR_RE2) {
        (void)load_bank_re2(0x10); (void)load_bank_re2(0x11); (void)load_bank_re2(0x16);
    } else {
        (void)load_bank_re15(0x10); (void)load_bank_re15(0x11); (void)load_bank_re15(0x16);
    }

    memset(s_live, 0, sizeof s_live);
    int32_t z0[RE15_ACTOR_MAX], x0[RE15_ACTOR_MAX];
    for (int s = 1; s < RE15_ACTOR_MAX; s++) {
        if (!is_zombie(&g_actors[s])) continue;
        s_live[s] = 1; z0[s] = g_actors[s].z; x0[s] = g_actors[s].x;
    }

    printf("\n=== %s :: Spieler (%ld,%ld) ===\n", name, (long)px, (long)pz);
    printf("  Startpositionen:");
    for (int s = 1; s < RE15_ACTOR_MAX; s++)
        if (s_live[s]) printf("  z%d(t%02x)=(%ld,%ld)", s, g_actors[s].type, (long)x0[s], (long)z0[s]);
    printf("\n");

    if (out) for (int s = 1; s < RE15_ACTOR_MAX; s++) if (s_live[s]) out->spawned++;

    if (frames > MAXF) frames = MAXF;
    for (int f = 0; f < frames; f++) {
        if (s_nudge_slot > 0 && f == s_nudge_frame && s_live[s_nudge_slot]) {
            printf("  [Positiv-Kontrolle] f%d: Slot %d von x=%ld auf x=%ld gesetzt "
                   "(oestlich der Wandkante -12166)\n", f, s_nudge_slot,
                   (long)g_actors[s_nudge_slot].x, (long)s_nudge_x);
            g_actors[s_nudge_slot].x = s_nudge_x;
        }
        frame_step();
        for (int s = 1; s < RE15_ACTOR_MAX; s++) {
            if (!s_live[s]) continue;
            const re15_actor_t *e = &g_actors[s];
            rec_t *r = &s_rec[s][f];
            r->x = e->x; r->z = e->z; r->sx = e->steer_x; r->sz = e->steer_z; r->ry = e->rot_y;
            r->sca = e->sca_mask; r->st = e->state; r->s1 = e->sub_state_1; r->s2 = e->sub_state_2;
            r->cflags = e->contact_flags; r->cslot = e->contact_slot;
            r->motion = (uint8_t)e->motion; r->afr = (uint8_t)e->anim_frame;
            r->f10e = e->re2z_f10e; r->a1c4 = e->anim_flags;
            r->m0b = e->member_0b;
            /* AOT-5 (Aot_set id 5 @Datei 0x1cf2): x[-12900..-3700] z[-25300..-24200] */
            r->inaot5 = (e->x >= -12900 && e->x <= -3700 &&
                         e->z >= -25300 && e->z <= -24200) ? 1u : 0u;
        }
    }

    /* --- Auswertung: Weglaenge vs. Netto, Ueberquerung ------------------------------------- */
    printf("  %-4s %-8s %-22s %-22s %-9s %-9s %-6s %s\n",
           "slot", "typ", "start (x,z)", "ende  (x,z)", "Weg", "Netto", "sca", "Riegel");
    for (int s = 1; s < RE15_ACTOR_MAX; s++) {
        if (!s_live[s]) continue;
        double path = 0.0;
        int32_t lx = x0[s], lz = z0[s];
        int seen8 = 0, seenflag = 0, seencrawl = 0;
        for (int f = 0; f < frames; f++) {
            rec_t *r = &s_rec[s][f];
            double dx = (double)(r->x - lx), dz = (double)(r->z - lz);
            path += (dx < 0 ? -dx : dx) + (dz < 0 ? -dz : dz);
            lx = r->x; lz = r->z;
            if (r->sca == 8) seen8 = 1;
            if (r->a1c4 & 0x1000u) seenflag = 1;
            if (fl == RE15_AI_FLAVOR_RE2) { if (r->f10e & 1u) seencrawl = 1; }
        }
        int f_aot5 = -1, f_stamp5 = -1, f_flag = -1, n_stamp5 = 0;
        for (int f = 0; f < frames; f++) {
            rec_t *r = &s_rec[s][f];
            if (r->inaot5 && f_aot5 < 0) f_aot5 = f;
            if (r->m0b == 5) { if (f_stamp5 < 0) f_stamp5 = f; n_stamp5++; }
            if ((r->a1c4 & 0x1000u) && f_flag < 0) f_flag = f;
        }
        printf("        -> erster Frame in AOT5=%d, erster Stempel +0x0B==5 =%d (%d Frames), "
               "erster 0x1000 =%d\n", f_aot5, f_stamp5, n_stamp5, f_flag);
        if (s == 1)
            printf("        Bank5 am Ende: Slot-Flags 0..5 = %d%d%d%d%d%d | 0x14=%d 0x20=%d "
                   "0x21=%d 0x22=%d\n",
                   re15_game_flag_get(5,0), re15_game_flag_get(5,1), re15_game_flag_get(5,2),
                   re15_game_flag_get(5,3), re15_game_flag_get(5,4), re15_game_flag_get(5,5),
                   re15_game_flag_get(5,0x14), re15_game_flag_get(5,0x20),
                   re15_game_flag_get(5,0x21), re15_game_flag_get(5,0x22));
        int32_t ex = s_rec[s][frames-1].x, ez = s_rec[s][frames-1].z;
        double net = (double)(ex - x0[s]); if (net < 0) net = -net;
        double netz = (double)(ez - z0[s]); if (netz < 0) netz = -netz;
        int crossed = (z0[s] <= GATE_Z_NORTH && ez > GATE_Z_NORTH);
        if (out) {
            out->armed += seenflag;
            out->crossed += crossed;
            if (!crossed && z0[s] <= GATE_Z_NORTH && out->stuck_slot < 0) {
                out->stuck_slot   = s;
                out->stuck_path   = path;
                out->stuck_net    = net + netz;
                out->stuck_stamp5 = n_stamp5;
                out->stuck_armedf = f_flag;
            }
        }
        printf("  z%-3d 0x%02x     (%7ld,%8ld)  (%7ld,%8ld)  %8.0f  %8.0f  %d/%d/%d  %s\n",
               s, g_actors[s].type, (long)x0[s], (long)z0[s], (long)ex, (long)ez,
               path, net + netz, seenflag, seencrawl, seen8,
               crossed ? "DURCH" : (z0[s] > GATE_Z_NORTH ? "(startete noerdlich)" : "*** STECKT ***"));
    }

    /* --- Wer klemmt? Alle SCA-Zellen, die den Steckenden am Ende beruehren ------------------ */
    for (int s = 1; s < RE15_ACTOR_MAX; s++) {
        if (!s_live[s]) continue;
        if (z0[s] > GATE_Z_NORTH) continue;
        if (s_rec[s][frames-1].z > GATE_Z_NORTH) continue;
        const re15_actor_t *e = &g_actors[s];
        int band = re15_collision_band_from_y(e->y);
        int32_t r = (int32_t)e->hit_radius_min;
        unsigned mask = e->sca_mask ? e->sca_mask : 4u;
        printf("\n  === SCA-Umgebung von Slot %d: pos=(%ld,%ld) y=%ld band=%d r=%ld maske=%u ===\n",
               s, (long)e->x, (long)e->z, (long)e->y, band, (long)r, mask);
        unsigned zb = (unsigned)(e->z - (int32_t)g_room_rdt.ceiling_z) & 0x80000000u;
        unsigned xb = (unsigned)(e->x - (int32_t)g_room_rdt.ceiling_x) & 0x80000000u;
        int qd = (int)((zb | (xb >> 1)) >> 0x1e);
        int st = 0; for (int i = 0; i < qd && i < 5; i++) st += g_room_rdt.sca_rgn[i];
        int en = st + (qd < 5 ? g_room_rdt.sca_rgn[qd] : 0);
        printf("  Quadrant %d -> Zellen %d..%d (ceil=(%d,%d))\n", qd, st, en - 1,
               (int)g_room_rdt.ceiling_x, (int)g_room_rdt.ceiling_z);
        for (int i = st; i < en && i < g_room_rdt.sca_count; i++) {
            const re15_sca_entry_t *c = &g_room_rdt.sca[i];
            int hitband = (band == (c->floor >> 4));
            int hitmask = ((mask & c->u0) != 0);
            int over = ((unsigned)(e->x - ((int32_t)c->x - r)) < (unsigned)((int32_t)c->width   + r * 2) &&
                        (unsigned)(e->z - ((int32_t)c->z - r)) < (unsigned)((int32_t)c->density + r * 2));
            printf("    %s Zelle#%-3d (rgn-idx %-2d) typ=%-2d band=%d u0=0x%02x rect=(%d,%d, %dx%d) "
                   , over ? "UEBERLAPPT" : "          ",
                   "-> band%s maske%s %s\n",
                   i, i - st, c->type, (int)(c->floor >> 4), (unsigned)c->u0,
                   (int)c->x, (int)c->z, (int)c->width, (int)c->density,
                   hitband ? "OK" : "NEIN", hitmask ? "OK" : "NEIN",
                   (hitband && hitmask) ? "*** KLEMMT ***" : "");
        }
        /* Was macht die Klemme bei einem Schritt nach Norden? (mit Zell-Trace) */
        putenv((char *)"RE15_COLL_TRACE=1");
        for (int step = 25; step <= 400; step *= 2) {
            int32_t nx = e->x, nz = e->z + step;
            re15_collision_constrain_enemy(&g_room_rdt, e->x, e->z, &nx, &nz, r, e->y, mask);
            printf("    Schritt +%dz: (%ld,%ld) -> geklemmt (%ld,%ld)  dz=%ld\n",
                   step, (long)e->x, (long)(e->z + step), (long)nx, (long)nz, (long)(nz - e->z));
            fflush(stdout); fflush(stderr);
        }
        putenv((char *)"RE15_COLL_TRACE=");
    }

    /* --- Zustand der Torzellen (die sub07 auf 0xF7 setzt) ----------------------------------- */
    {
        int base = 0;
        for (int q = 0; q < 5; q++) {
            if (base + 6 < g_room_rdt.sca_count) {
                const re15_sca_entry_t *c = &g_room_rdt.sca[base + 6];
                printf("  Torzelle rgn%d/idx6 = Zelle#%d: typ=%d band=%d u0=0x%02x rect=(%d,%d,%dx%d)\n",
                       q, base + 6, c->type, (int)(c->floor >> 4), (unsigned)c->u0,
                       (int)c->x, (int)c->z, (int)c->width, (int)c->density);
            }
            base += g_room_rdt.sca_rgn[q];
        }
    }

    /* --- Frame-Auszug fuer jeden, der suedlich startete und NICHT durchkam ------------------ */
    for (int s = 1; s < RE15_ACTOR_MAX; s++) {
        if (!s_live[s]) continue;
        if (z0[s] > GATE_Z_NORTH) continue;
        if (s_rec[s][frames-1].z > GATE_Z_NORTH) continue;
        printf("\n  --- STECKENGEBLIEBEN: Slot %d (typ 0x%02x) — Frame-Auszug alle 20 Frames ---\n",
               s, g_actors[s].type);
        printf("  %-6s %-9s %-9s %-6s %-6s %-6s %-4s %-4s %-4s %-5s %-4s %-5s %-6s %s\n",
               "f", "x", "z", "steerX", "steerZ", "ry", "sca", "st", "s1", "s2", "clip", "fr",
               "cflg", "cslot/f10e/1c4 m0b/aot5");
        for (int f = 0; f < frames; f += 100) {
            rec_t *r = &s_rec[s][f];
            printf("  %-6d %-9ld %-9ld %-6d %-6d %-6d %-4u %-4u %-4u %-5u %-4u %-5u %-6u %d/%04x/%04x\n",
                   f, (long)r->x, (long)r->z, r->sx, r->sz, r->ry, r->sca, r->st, r->s1, r->s2,
                   r->motion, r->afr, r->cflags, r->cslot, r->f10e, r->a1c4, r->m0b, r->inaot5);
        }
    }
}

static int fails = 0;
#define CHECK(c, ...) do { if (!(c)) { printf("FAIL: "); printf(__VA_ARGS__); printf("\n"); fails++; } } while (0)

/* ---- Die DATEN-Geometrie, aus der das Klemmen folgt (aus der RDT gelesen, nicht geraten) ---- */
static void pin_geometry(void)
{
    printf("\n--- Daten-Pin: Nav-Tuer 9 gegen SCA-Wand #30 ---\n");
    /* Nav-Knoten (RDT+0x38, tbl+4 + 12*i, Felder +0/+2/+4/+6 = x1/z1/x2/z2 — Original liest
     * sie als 4(t1)/6(t1)/8(t1)/10(t1), FUN_8003a0fc @0x8003a138-78 / FUN_8003a31c). */
    const uint8_t *nd0 = g_room_rdt.blocks + 0 * 12;
    const uint8_t *nd9 = g_room_rdt.blocks + 9 * 12;
    int a_x1 = (int16_t)(nd0[0] | (nd0[1] << 8)), a_x2 = (int16_t)(nd0[4] | (nd0[5] << 8));
    int b_x1 = (int16_t)(nd9[0] | (nd9[1] << 8)), b_x2 = (int16_t)(nd9[4] | (nd9[5] << 8));
    int lo = a_x1 > b_x1 ? a_x1 : b_x1;
    int hi = a_x2 < b_x2 ? a_x2 : b_x2;
    int mid = (lo + hi) >> 1;                 /* FUN_8003a31c @0x8003a3f0-f8 `addu`+`sra 1` */
    printf("  Zone0 x[%d..%d]  Zone9 x[%d..%d]  -> lo=%d hi=%d  Kreuzung x=%d\n",
           a_x1, a_x2, b_x1, b_x2, lo, hi, mid);
    CHECK(mid == -12195, "Nav-Kreuzung 0->9 ist %d, erwartet -12195", mid);

    const re15_sca_entry_t *c30 = &g_room_rdt.sca[30];
    int cx = c30->x, cw = c30->width;
    printf("  SCA-Zelle #30: typ=%d u0=0x%02x u1=0x%02x band=%d x[%d..%d] z[%d..%d]\n",
           c30->type, c30->u0, c30->u1, c30->floor >> 4, cx, cx + cw,
           (int)c30->z, (int)c30->z + (int)c30->density);
    CHECK(c30->u0 == 0xFF, "Wand #30 u0 = 0x%02x, erwartet 0xFF (auch fuer Maske 8 solide)", c30->u0);
    CHECK(c30->u1 == 0x00, "Wand #30 u1 = 0x%02x, erwartet 0x00 — waere u1&3 gesetzt, koennte der "
                           "Original-Reroute @0x80102074 feuern", c30->u1);
    /* Broad-Phase FUN_8003b0a4 @0x8003b284-94: (center.x - (cell.x - r)) < (width + 2r), r=400. */
    int limit = cx + cw + 400;                /* ab hier greift die Zelle NICHT mehr */
    printf("  Radius 400 (Box @STAGE1.BIN 0x1f778) -> erreichbar erst ab x >= %d; "
           "Nav zielt auf %d  => %d Einheiten INNERHALB der Wand\n", limit, mid, limit - mid);
    CHECK(mid < limit, "die Nav-Kreuzung laege AUSSERHALB der Wand — Geometrie hat sich geaendert");

    /* Torzelle: der Kriecher-Durchlass, den sub07/sub08 auf 0xF7 setzen (`37 02 06 f7` @0x2000/
     * 0x278E, `37 03 06 f7` @0x2004/0x2792; Handler LAB_8004175c `sb a1,9(v0)` @0x8004179c). */
    CHECK((g_room_rdt.sca[28].u0 & 8u) == 0,
          "Torzelle #28 u0 = 0x%02x — Bit 3 muss fuer Kriecher frei sein", g_room_rdt.sca[28].u0);
    CHECK((g_room_rdt.sca[18].u0 & 8u) == 0,
          "Torzelle #18 u0 = 0x%02x — Bit 3 muss fuer Kriecher frei sein", g_room_rdt.sca[18].u0);
}

int main(void)
{
    printf("== ROOM1030: WER bleibt beim Kriechen haengen, und WARUM? ==\n");
    size_t n = 0;
    uint8_t *buf = slurp(RE15_ASSET_PSX_DIR "/STAGE1/ROOM1030.RDT", &n);
    if (!buf) { printf("FAIL: ROOM1030.RDT nicht lesbar\n"); return 1; }
    if (re15_rdt_parse(buf, n, &g_room_rdt) != 0) { printf("FAIL: RDT-Parse\n"); return 1; }
    g_room_rdt_ok = 1;
    s_rdt = g_room_rdt;

    out_t r15, r2, ctl;

    run(RE15_AI_FLAVOR_RE15, "PHASE A / RE1.5", -18050, -8300, 6000, &r15);
    re15_enemy_reset();
    run(RE15_AI_FLAVOR_RE2,  "PHASE A / RE2",   -18050, -8300, 6000, &r2);

    /* ---- POSITIV-KONTROLLE: derselbe Lauf, aber der Steckengebliebene wird EINMAL
     * oestlich der Wandkante gesetzt (x = -12100 > limit -12166). Kommt er dann durch,
     * ist bewiesen: Kriech-Gehirn, Torzelle und SCA-Maske sind in Ordnung — es klemmt
     * AUSSCHLIESSLICH die Geometrie (Nav-Ziel im radius-aufgeblasenen Wand-Rechteck). */
    printf("\n--- POSITIV-KONTROLLE (RE2, Slot %d einmalig nach x=-12100) ---\n", r2.stuck_slot);
    s_nudge_slot = r2.stuck_slot; s_nudge_frame = 900; s_nudge_x = -12100;
    re15_enemy_reset();
    run(RE15_AI_FLAVOR_RE2, "KONTROLLE / RE2", -18050, -8300, 6000, &ctl);
    s_nudge_slot = -1;

    pin_geometry();

    printf("\n== ERGEBNIS ==\n");
    printf("  RE1.5   : %d Zombies, %d scharfgeschaltet, %d durch   (Steckenbleiber Slot %d, "
           "Weg %.0f / Netto %.0f, +0x0B==5 fuer %d Frames, scharf ab Frame %d)\n",
           r15.spawned, r15.armed, r15.crossed, r15.stuck_slot, r15.stuck_path, r15.stuck_net,
           r15.stuck_stamp5, r15.stuck_armedf);
    printf("  RE2     : %d Zombies, %d scharfgeschaltet, %d durch   (Steckenbleiber Slot %d, "
           "Weg %.0f / Netto %.0f)\n",
           r2.spawned, r2.armed, r2.crossed, r2.stuck_slot, r2.stuck_path, r2.stuck_net);
    printf("  KONTROLLE: %d durch\n", ctl.crossed);

    /* ---- PINS ------------------------------------------------------------------------------
     * 1) RE1.5: 5 von 6 werden scharfgeschaltet. Das ist KEIN Port-Fehler, sondern das
     *    Skript-Budget: sub02 ruft die Scharfschalt-Schleife sub06 nur unter
     *    `Cmp(work7, <, 4)` (`23 00 07 03 04 00` @Datei 0x21cc; Operator 3 = `<`, Original
     *    `slt v0,a0,a1` @0x8003ffe8), und work7 ist die von sub03 (@Datei 0x220e,
     *    `Member_cmp(15,==,6)`) gezaehlte Zahl der Zombies in AOT-Zone 6 (der Lobby).
     *    Wer erst nach den ersten vier am Tor ankommt, wird nie mehr scharfgeschaltet —
     *    er traegt den Zonenstempel +0x0B==5 tausende Frames lang vergeblich. */
    CHECK(r15.spawned == 6, "RE1.5: %d statt 6 Zombies gespawnt", r15.spawned);
    CHECK(r15.armed == 5, "RE1.5: %d statt 5 scharfgeschaltet (Skript-Budget "
                          "`Cmp(work7,<,4)` @0x21cc)", r15.armed);
    CHECK(r15.crossed == 5, "RE1.5: %d statt 5 durch das Tor", r15.crossed);
    CHECK(r15.stuck_armedf == -1 && r15.stuck_stamp5 > 1000,
          "RE1.5-Steckenbleiber: erwartet 'gestempelt aber nie scharf' "
          "(Stempel-Frames %d, scharf ab %d)", r15.stuck_stamp5, r15.stuck_armedf);

    /* 2) RE2: alle sechs werden scharf (die RE2-Gangart bringt sie vor der 4er-Schwelle in
     *    AOT-5), aber einer laeuft in die Wand #30 und kommt nie an. Weglaenge >> Netto =
     *    der Grenzzyklus, den der Nutzer als "Laufanimation, aber keine Bewegung" sieht. */
    CHECK(r2.armed == 6, "RE2: %d statt 6 scharfgeschaltet", r2.armed);
    CHECK(r2.crossed == 5, "RE2: %d statt 5 durch das Tor", r2.crossed);
    CHECK(r2.stuck_slot > 0 && r2.stuck_path > 10.0 * r2.stuck_net,
          "RE2-Steckenbleiber: Weg %.0f ist nicht >> Netto %.0f — der Grenzzyklus ist weg "
          "(Slot %d)", r2.stuck_path, r2.stuck_net, r2.stuck_slot);

    /* 3) Positiv-Kontrolle: oestlich der Wandkante kommen ALLE sechs durch. */
    CHECK(ctl.crossed == 6,
          "POSITIV-KONTROLLE: nur %d von 6 durch, obwohl der Kriecher oestlich der Wandkante "
          "-12166 gesetzt wurde — dann liegt es NICHT nur an der Geometrie", ctl.crossed);

    free(buf);
    g_room_rdt_ok = 0;
    re15_ai_flavor_set(RE15_AI_FLAVOR_RE15);
    printf("\n== %s (%d Fehler) ==\n", fails ? "FEHLGESCHLAGEN" : "OK", fails);
    return fails ? 1 : 0;
}
