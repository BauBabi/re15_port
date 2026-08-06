/* probe_laneF_floor_1060.c — LANE F DIAGNOSE (kein ctest).
 *
 * FRAGE: Wie unterscheidet ROOM1060 die vier UEBEREINANDER gestapelten Treppen im
 * Kamera-Zonen-Scan? Gibt es eine Etagen-Dimension (Floor-Byte / Band / Y)?
 *
 * Belegte Original-Mechanik (disassembliert, siehe Report):
 *   FUN_80014230 @0x80014230  — RVD-Zonen-Scan, Stride 20, Feldzugriffe:
 *     @0x80014254 lbu v1,22(a0)   -> rec[2] = cam_from (Gruppen-Terminator)
 *     @0x8001426c lbu v1,-1(s0)   -> rec[1] = FLOOR-Byte
 *     @0x80014274 beq v1,0xff     -> 0xFF = jede Etage
 *     @0x80014280 lbu 0x800acad6  -> sonst MUSS == player+0x82 (Band)
 *     @0x80014298 jal 0x80014368  -> Punkt-in-Quad(player XZ, rec+4..rec+0x13)
 *     @0x800142a8 lbu a0,1(s0)    -> rec[3] = cam_to  -> FUN_800142f4, RETURN
 *   Es gibt KEIN Y-Feld und KEINEN Y-Test.  Die einzige Etagen-Dimension ist
 *   rec[1] (Floor) — plus implizit der aktuell ANGEZEIGTE Cut (rec[2]).
 *
 * Diese Sonde MISST:
 *   (F1) alle RVD-Floor-Bytes in ROOM1060/ROOM10A0 (ist das Gate inert?)
 *   (F2) ob je zwei Zonen verschiedener Cuts im XZ ueberlappen -> dann ist der Cut
 *        der EINZIGE Etagen-Diskriminator, jeder verpasste Uebergang ist kumulativ
 *   (F3) den IDEALEN geometrischen Abstiegs-Sweep (Punkt laeuft die Treppenachse
 *        entlang, Zonen-Scan pro Schritt) -> die Cut-Kette, die das Original
 *        erzeugen MUSS, wenn der Spieler die Treppe durchlaeuft
 *   (F4) dieselbe Kette mit dem ECHTEN Port-Treppencode (re15_stair_*) und der
 *        geometrisch RICHTIGEN Blickrichtung (die Treppe hinunter)
 *   (F5) das Band waehrend der Treppe (Original setzt es @0x80038ce4 auf 7)
 *   (F6) den FINALIZE-Versatz des Ports (stair_common.c:162, 48x100u)
 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "re15_rdt.h"
#include "re15_scd.h"
#include "re15_actor.h"
#include "re15_room.h"
#include "re15_player.h"
#include "re15_collision.h"
#include "re15_stair.h"
#include "re15_aot.h"
#include "re15_emd.h"

#define RE15_STR(x)  #x
#define RE15_XSTR(x) RE15_STR(x)

static re15_rdt_t g_rdt;
static re15_emd_skeleton_t  g_skel;
static re15_emd_animation_t g_anim;
static int g_skel_ok = 0;
static int g_cut = 0;
static unsigned g_frame = 0;

static uint8_t *read_file(const char *path, size_t *out_size)
{
    FILE *f = fopen(path, "rb");
    if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    if (sz <= 0) { fclose(f); return NULL; }
    uint8_t *buf = (uint8_t *)malloc((size_t)sz);
    if (!buf) { fclose(f); return NULL; }
    size_t rd = fread(buf, 1, (size_t)sz, f);
    fclose(f);
    if (rd != (size_t)sz) { free(buf); return NULL; }
    *out_size = (size_t)sz;
    return buf;
}

/* --- ein RVD-Scan-Frame auf einem beliebigen Punkt (ohne Actor) ------------ */
static int scan_point(int32_t x, int32_t z, const char *tag, int verbose)
{
    g_scd.cut_auto_enabled = 1;
    re15_aot_scan(x, z, (uint8_t)g_cut);
    int changed = 0;
    if (g_scd.cam_change_pending) {
        int nc = (int)g_scd.cam_id;
        g_scd.cam_change_pending = 0;
        if (nc != g_cut) {
            if (verbose)
                printf("      F%-4u CUT %d -> %-2d  @(%ld,%ld)   [%s]\n",
                       g_frame, g_cut, nc, (long)x, (long)z, tag);
            g_cut = nc; changed = 1;
        }
    }
    g_frame++;
    return changed;
}

/* --- (F2) XZ-Ueberlappung von Zonen verschiedener cam_from ---------------- */
static int aabb_overlap(const re15_rdt_zone_t *a, const re15_rdt_zone_t *b)
{
    int32_t ax0 = a->cx - a->half_w, ax1 = a->cx + a->half_w;
    int32_t az0 = a->cz - a->half_h, az1 = a->cz + a->half_h;
    int32_t bx0 = b->cx - b->half_w, bx1 = b->cx + b->half_w;
    int32_t bz0 = b->cz - b->half_h, bz1 = b->cz + b->half_h;
    return !(ax1 < bx0 || bx1 < ax0 || az1 < bz0 || bz1 < az0);
}

static void report_zones(const char *room)
{
    printf("\n=== (F1) RVD-Tabelle %s — %d Zonen ===\n", room, g_rdt.zone_count);
    int nonff = 0;
    for (int i = 0; i < g_rdt.zone_count; i++) {
        const re15_rdt_zone_t *z = &g_rdt.zones[i];
        int anchor = (i == 0 || g_rdt.zones[i-1].cam_from != z->cam_from);
        printf("  [%2d]%s from=%-2d to=%-2d FLOOR=0x%02x  x[%6d..%6d] z[%6d..%6d]\n",
               i, anchor ? " ANKER" : "      ", (int)z->cam_from, (int)z->cam_to,
               (int)z->floor,
               (int)(z->cx - z->half_w), (int)(z->cx + z->half_w),
               (int)(z->cz - z->half_h), (int)(z->cz + z->half_h));
        if (z->floor != 0xFF) nonff++;
    }
    printf("  -> Floor-Bytes != 0xFF: %d von %d  => Floor-Gate @0x80014280 %s\n",
           nonff, g_rdt.zone_count, nonff ? "AKTIV" : "INERT");

    printf("\n=== (F2) XZ-Ueberlappung von Trigger-Zonen VERSCHIEDENER cam_from ===\n");
    int pairs = 0;
    for (int i = 0; i < g_rdt.zone_count; i++) {
        if (i == 0 || g_rdt.zones[i-1].cam_from != g_rdt.zones[i].cam_from) continue; /* Anker */
        for (int j = i + 1; j < g_rdt.zone_count; j++) {
            if (j == 0 || g_rdt.zones[j-1].cam_from != g_rdt.zones[j].cam_from) continue;
            if (g_rdt.zones[i].cam_from == g_rdt.zones[j].cam_from) continue;
            if (!aabb_overlap(&g_rdt.zones[i], &g_rdt.zones[j])) continue;
            pairs++;
            if (pairs <= 24)
                printf("  [%2d](from=%d->%d) ueberlappt [%2d](from=%d->%d)\n",
                       i, g_rdt.zones[i].cam_from, g_rdt.zones[i].cam_to,
                       j, g_rdt.zones[j].cam_from, g_rdt.zones[j].cam_to);
        }
    }
    printf("  -> %d ueberlappende Paare. Ohne Floor-Byte ist der ANGEZEIGTE CUT der\n"
           "     einzige Diskriminator -> jeder verpasste Uebergang ist KUMULATIV.\n", pairs);
}

/* --- Treppen-AOT-Beschreibung -------------------------------------------- */
typedef struct {
    int slot; int chain; int axis; int32_t corner; int32_t extent; int side; int count;
    int32_t cx, cz, hw, hh;
} stair_t;

static int collect_stairs(stair_t *out, int max)
{
    int n = 0;
    for (int i = 0; i < RE15_AOT_MAX && n < max; i++) {
        const re15_aot_t *a = &g_aot.slots[i];
        if (!a->active || a->type != RE15_AOT_TYPE_STAIR) continue;
        const re15_aot_stair_params_t *sp = &g_aot.stair_params[i];
        out[n].slot = i;  out[n].chain = (int)a->band;
        out[n].axis = sp->axis; out[n].corner = sp->corner; out[n].extent = sp->extent;
        out[n].side = sp->side; out[n].count = sp->count & 7;
        out[n].cx = a->x; out[n].cz = a->z; out[n].hw = a->half_w; out[n].hh = a->half_h;
        n++;
    }
    return n;
}

int main(void)
{
    const char *base = RE15_XSTR(RE15_ASSETS_PATH);
    {
        char p2[600]; size_t s1 = 0, s2 = 0;
        snprintf(p2, sizeof p2, "%s/PLD/PL00.EDD", base);
        uint8_t *edd = read_file(p2, &s1);
        snprintf(p2, sizeof p2, "%s/PLD/PL00.EMR", base);
        uint8_t *emr = read_file(p2, &s2);
        if (edd && emr && re15_emd_parse_animation(edd, s1, &g_anim) == 0 &&
            re15_emd_parse_skeleton(emr, s2, &g_skel) == 0) g_skel_ok = 1;
        printf("PL00 %s\n", g_skel_ok ? "geladen" : "FEHLT (FK-Foot-Lock inaktiv!)");
    }

    char path[600]; size_t size = 0;
    snprintf(path, sizeof path, "%s/STAGE1/ROOM1060.RDT", base);
    uint8_t *data = read_file(path, &size);
    if (!data) { printf("FAIL: %s\n", path); return 1; }
    if (re15_rdt_parse(data, size, &g_rdt) != 0) { printf("FAIL parse\n"); return 1; }

    report_zones("ROOM1060");

    /* Raum hochfahren (RVD -> AOT-Slots, SCD main00 -> Treppen-AOTs). */
    re15_actor_init();
    scd_vm_init();
    g_current_room_id = 0x1060;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100;
    pl->x = 25150; pl->z = 24000; pl->y = -8 * 0x708;
    scd_register_room_events(&g_rdt);
    scd_room_reenter(&g_rdt, 0, 0, 0);
    for (int f = 0; f < 10; f++) scd_vm_tick();

    stair_t st[16];
    int nst = collect_stairs(st, 16);
    printf("\n=== Treppen-AOTs (SCD Aot_set, RDT @0x0d72..) ===\n");
    for (int i = 0; i < nst; i++)
        printf("  slot=%2d chain(Band)=%d axis=%d corner=%ld extent=%ld side=%d count=%d"
               "  rect x[%ld..%ld] z[%ld..%ld]\n",
               st[i].slot, st[i].chain, st[i].axis, (long)st[i].corner, (long)st[i].extent,
               st[i].side, st[i].count,
               (long)(st[i].cx - st[i].hw), (long)(st[i].cx + st[i].hw),
               (long)(st[i].cz - st[i].hh), (long)(st[i].cz + st[i].hh));

    /* Fuer jede Treppe: welches Rect-Ende ist OBEN?  ASCEND <=> (low_half && side==0)
     * || (!low_half && side==1)   [LAB_800435cc @0x800435dc-0x80043594]
     * => side==0: LOW-Ende = unten, HIGH-Ende = OBEN
     *    side==1: HIGH-Ende = unten, LOW-Ende = OBEN                                */
    printf("\n=== (F3) IDEALER geometrischer Abstiegs-Sweep (Punkt laeuft die Achse) ===\n");
    printf("    Reihenfolge = Band 8 -> 6 -> 4 -> 2 -> 0; pro Treppe vom OBEREN Rect-Ende\n"
           "    zum UNTEREN, Schrittweite 10u, auf der Rect-Mittelachse.\n");
    g_cut = 0; g_frame = 0;
    for (int i = 0; i < RE15_AOT_MAX; i++) g_aot.slots[i].was_inside = 0;
    int bands[5] = { 8, 6, 4, 2, 0 };
    for (int b = 0; b < 4; b++) {
        /* Treppen-Record fuer dieses Band mit dem passenden Abstiegs-Ende suchen:
         * das ist der Record, dessen chain == Band ist UND dessen count-Schritt zum
         * naechsten Band fuehrt. In ROOM1060 gibt es je Band zwei (auf/ab) — wir
         * nehmen den, dessen OBERES Ende zum aktuellen Band gehoert. Praktisch:
         * beide Records eines Bandes haben verschiedene Rects; der ABSTIEG geht auf
         * dem Rect, dessen "oberes Ende" wir gerade erreicht haben. Wir probieren
         * beide und nehmen den, der die meisten Cut-Wechsel erzeugt — nein: wir
         * nehmen die AUS DER GEOMETRIE eindeutige Kette, indem wir die Treppe
         * waehlen, deren Rect NICHT die des vorigen Abstiegs war.                 */
        static int32_t prev_cx = -1;
        const stair_t *pick = NULL;
        for (int i = 0; i < nst; i++) {
            if (st[i].chain != bands[b]) continue;
            if (prev_cx >= 0 && st[i].cx == prev_cx) continue;   /* nicht dieselbe Treppe */
            pick = &st[i]; break;
        }
        if (!pick) { printf("   Band %d: kein Record\n", bands[b]); continue; }
        prev_cx = pick->cx;

        int32_t lo = pick->corner;
        int32_t hi = pick->corner + pick->extent;
        int32_t top = (pick->side == 0) ? hi : lo;      /* OBEN */
        int32_t bot = (pick->side == 0) ? lo : hi;      /* UNTEN */
        int32_t step = (bot > top) ? 10 : -10;
        int32_t axis_fixed = (pick->axis == 13) ? pick->cx : pick->cz;
        printf("\n   -- Treppe Band %d -> %d  (slot %d, Rect-Mitte %ld, %s %ld -> %ld) cut=%d\n",
               bands[b], bands[b] - pick->count, pick->slot, (long)axis_fixed,
               pick->axis == 13 ? "Z" : "X", (long)top, (long)bot, g_cut);
        for (int32_t p = top; (step > 0) ? (p <= bot) : (p >= bot); p += step) {
            if (pick->axis == 13) scan_point(axis_fixed, p, "sweep", 1);
            else                  scan_point(p, axis_fixed, "sweep", 1);
        }
        printf("      -> Ende Sweep: cut=%d\n", g_cut);
    }
    printf("\n   IDEALE END-CUT-KETTE = %d\n", g_cut);

    /* --- (F4) mit dem echten Port-Treppencode, Blickrichtung = die Treppe hinab -- */
    printf("\n=== (F4) ECHTER Port-Treppencode, Blickrichtung geometrisch korrekt ===\n");
    g_cut = 0; g_frame = 0;
    for (int i = 0; i < RE15_AOT_MAX; i++) g_aot.slots[i].was_inside = 0;
    re15_stair_reset();
    {
        int32_t prev_cx = -1;
        for (int b = 0; b < 4; b++) {
            const stair_t *pick = NULL;
            for (int i = 0; i < nst; i++) {
                if (st[i].chain != bands[b]) continue;
                if (prev_cx >= 0 && st[i].cx == prev_cx) continue;
                pick = &st[i]; break;
            }
            if (!pick) { printf("   Band %d: kein Record\n", bands[b]); continue; }
            prev_cx = pick->cx;

            int32_t lo = pick->corner, hi = pick->corner + pick->extent;
            int32_t top = (pick->side == 0) ? hi : lo;
            int down_is_plus = (pick->side != 0);        /* side=1: oben=lo -> abwaerts +Z */
            /* Port-Konvention: x += cos(rot), z -= sin(rot).  rot 0x400 -> -Z, 0xC00 -> +Z */
            int rot = down_is_plus ? 3072 : 1024;
            /* Startpunkt: 200u INNERHALB des oberen Endes, auf der Achsenmitte. */
            int32_t sz = top + (down_is_plus ? +200 : -200);
            pl->x = pick->cx; pl->z = sz;
            pl->y = -(int32_t)bands[b] * 0x708;
            pl->rot_y = (int16_t)rot;
            re15_collision_set_band(bands[b]);
            re15_stair_reset();
            printf("\n   -- Treppe Band %d: Start @(%ld,%ld) rot=%d cut=%d\n",
                   bands[b], (long)pl->x, (long)pl->z, rot, g_cut);
            if (!re15_stair_try_start(&g_rdt, 1)) { printf("      KEIN Stair-Start!\n"); continue; }
            printf("      Richtung=%s  Band(waehrend Treppe)=%d  [Original setzt hier 7 @0x80038ce4]\n",
                   pl->motion == (int16_t)RE15_PLAYER_MOTION_STAIR_DOWN ? "DOWN" : "UP",
                   re15_collision_debug_band());
            int32_t z0 = pl->z, x0 = pl->x, zprev = pl->z, xprev = pl->x;
            int n = 0; int32_t maxjump = 0; int jumpframe = -1;
            while (re15_stair_active() && n < 900) {
                re15_stair_tick(&g_rdt, g_skel_ok ? &g_skel : NULL, g_skel_ok ? &g_anim : NULL);
                int32_t dx = pl->x - xprev, dz = pl->z - zprev;
                int32_t d  = (dx < 0 ? -dx : dx) + (dz < 0 ? -dz : dz);
                if (d > maxjump) { maxjump = d; jumpframe = n; }
                xprev = pl->x; zprev = pl->z;
                scan_point(pl->x, pl->z, "stair", 1);
                n++;
            }
            printf("      Ende nach %d Frames @(%ld,%ld) y=%ld band=%d cut=%d  dz=%+ld dx=%+ld\n",
                   n, (long)pl->x, (long)pl->z, (long)pl->y,
                   re15_collision_debug_band(), g_cut,
                   (long)(pl->z - z0), (long)(pl->x - x0));
            printf("      (F6) groesster Ein-Tick-Versatz = %ld u @Tick %d  "
                   "[Original LAB_80038e50 @0x80038e50-eec hat KEINEN XZ-Store]\n",
                   (long)maxjump, jumpframe);
        }
    }
    printf("\n   PORT END-CUT-KETTE = %d\n", g_cut);
    return 0;
}
