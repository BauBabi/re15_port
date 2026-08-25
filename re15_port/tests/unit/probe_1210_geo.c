/* probe_1210_geo.c - MESSUNG (kein Test, kein add_test), Auftrag 2026-08-29:
 *   (A) Wie nah kann ein Spieler in ROOM1210 wirklich an jeden Arm?
 *       - begehbarer Boden  (re15_collision_on_floor  = FUN_8003b7f0)
 *       - BESETZBARE Punkte (re15_collision_constrain = FUN_8003b0a4, Radius 450
 *         @0x80073e9a, Maske 1 @0x80031d74) + Objekt-Pass (re15_collision_objects)
 *       - ERREICHBARE Punkte = Flutfuellung ueber die besetzbaren, vom Flur aus
 *       - echter Laufweg mit Gehtempo 75/Bild gegen 5 und 1 (Schrittweiten-Artefakt?)
 *   (B) Wie weit ragt das MESH (nicht der Bone) nach vorn? Alle Posen der Clips 0/1/2,
 *       Renderer-Transform: v_world = R_y(yaw)*(pose.rot*v + pose.trans) + pos.
 *   (C) Kontakt-Geometrie des Originals: Radius Spieler 450 + Writher 300 = 750
 *       (FUN_8002aec4, Aufruf @0x8010c2f8 mit a0 = 0x800aca54 = Spieler).
 */
#include "re15_rdt.h"
#include "re15_scd.h"
#include "re15_actor.h"
#include "re15_aot.h"
#include "re15_room.h"
#include "re15_enemy.h"
#include "re15_enemy_ai.h"
#include "re15_ems.h"
#include "re15_emd.h"
#include "re15_md1.h"
#include "re15_collision.h"
#include "re15_msg.h"
#include "re15_game_step.h"
#include "re15_camera.h"
#include "re15_damage.h"
#include "re15_skeleton.h"
#include "re15_math.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef RE15_ASSET_PSX_DIR
#define RE15_ASSET_PSX_DIR "shared_assets/PSX"
#endif

static re15_rdt_t         s_rdt;
static re15_camera_view_t s_cam;
static re15_game_ctx_t    s_ctx;
static uint8_t            s_blob1a[0x80000];

static uint8_t *slurp(const char *p, size_t *n)
{
    FILE *f = fopen(p, "rb"); if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    if (sz <= 0) { fclose(f); return NULL; }
    uint8_t *b = (uint8_t *)malloc((size_t)sz);
    if (b && fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); b = NULL; }
    fclose(f); if (b) *n = (size_t)sz; return b;
}

static int load_bank(uint8_t type, uint8_t *blob, size_t blobcap)
{
    size_t n = 0;
    uint8_t *ems = slurp(RE15_ASSET_PSX_DIR "/EMD/CDEMD0.EMS", &n);
    if (!ems) return 0;
    int idx = re15_ems_index_for_type(type);
    size_t off = 0, len = 0; int ok = 0;
    if (idx >= 0 && re15_ems_get_entry(ems, n, idx, &off, &len) == 0 && len <= blobcap) {
        memcpy(blob, ems + off, len);
        re15_enemy_bank_t *eb = re15_enemy_find(type);
        if (!eb) eb = re15_enemy_alloc(type);
        if (eb) {
            re15_tim_t tim = (re15_tim_t){0};
            if (re15_emd_parse_container(blob, len, &eb->md1, &eb->skel, &eb->anim, &tim) == 0) {
                eb->ok = 1; eb->buf = NULL;
                eb->loco_ok = (re15_emd_parse_loco_bank(blob, len, &eb->skel_loco, &eb->anim_loco) == 0);
                ok = 1;
            }
        }
    }
    free(ems);
    return ok;
}

static void frame_step(void)
{
    const unsigned char *raw; int len, id;
    re15_msg_tick(&raw, &len, &id);
    s_ctx.pad_current = 0; s_ctx.pad_pressed = 0;
    scd_vm_tick();
    re15_game_step(&s_ctx);
}

/* BESETZBAR = die Broadphase des Aufloesers greift an diesem Punkt fuer KEINE Zelle.
 * Das ist woertlich der Overlap-Test aus collision_constrain_impl (Quadranten-Auswahl
 * FUN_8003b068, Band-Gleichheit, (mask & u0), Rechteck um r=450 aufgeblasen).
 * ⛔ WARUM NICHT "constrain veraendert den Punkt nicht": push_rect hat einen
 * Rueckschnapp-Zweig (code==0, `*lx = prevx; *lz = prevz;`). Faellt bei prev==pos die
 * Bedingung `*lx==prevx && *lz==prevz` durch (weil eine FRUEHERE Zelle schon geschoben
 * hat), schnappt der Punkt exakt auf sich selbst zurueck und sieht frei aus, obwohl er
 * blockiert ist. Gemessen an (-12107,-5897): Zelle #5 schiebt z auf -5996, danach
 * schnappt Zelle #12 auf die Ausgangslage zurueck -> "unveraendert" trotz Blockade. */
static int occupiable(int32_t x, int32_t z)
{
    const int32_t r = 450;                    /* @0x80073e9a via lhu a1,6(v0) @0x80031d6c */
    unsigned zb = (unsigned)(z - (int32_t)(int16_t)s_rdt.ceiling_z) & 0x80000000u;
    unsigned xb = (unsigned)(x - (int32_t)(int16_t)s_rdt.ceiling_x) & 0x80000000u;
    int q = (int)((zb | (xb >> 1)) >> 30);
    int st = 0; for (int i = 0; i < q && i < 5; i++) st += s_rdt.sca_rgn[i];
    int en = st + (q < 5 ? s_rdt.sca_rgn[q] : 0);
    if (en > s_rdt.sca_count) en = s_rdt.sca_count;
    for (int i = st; i < en; i++) {
        const re15_sca_entry_t *e = &s_rdt.sca[i];
        if ((e->floor >> 4) != 0) continue;                 /* Spieler-Band 0 */
        if ((1u & e->u0) == 0) continue;                    /* ori a2,zero,0x1 @0x80031d74 */
        if ((unsigned)(x - ((int32_t)e->x - r)) < (unsigned)((int32_t)e->width   + r*2) &&
            (unsigned)(z - ((int32_t)e->z - r)) < (unsigned)((int32_t)e->density + r*2))
            return 0;
    }
    { int32_t nx = x, nz = z; re15_collision_objects(&nx, &nz);
      if (nx != x || nz != z) return 0; }
    return 1;
}

/* ---- Flutfuellung ueber die besetzbaren Punkte -------------------------------------- */
#define GX0 (-34000)
#define GX1 (-4000)
#define GZ0 (-26000)
#define GZ1 (-2000)
#define GS  25
#define GW  (((GX1)-(GX0))/GS + 1)
#define GH  (((GZ1)-(GZ0))/GS + 1)
static uint8_t g_occ[GW*GH];
static uint8_t g_rea[GW*GH];
static int     g_stack[GW*GH];

static int32_t idist(int32_t dx, int32_t dz)
{ return (int32_t)re15_squareroot0((uint32_t)((int64_t)dx*dx + (int64_t)dz*dz)); }

int main(void)
{
    char path[600];
    snprintf(path, sizeof path, RE15_ASSET_PSX_DIR "/STAGE1/ROOM1210.RDT");
    size_t sz = 0; uint8_t *buf = slurp(path, &sz);
    if (!buf) { printf("FAIL: %s nicht lesbar\n", path); return 1; }
    if (re15_rdt_parse(buf, sz, &s_rdt) != 0) { printf("FAIL: RDT-Parse\n"); return 1; }
    memset(&s_cam, 0, sizeof s_cam); memset(&s_ctx, 0, sizeof s_ctx);
    s_ctx.rdt = &s_rdt; s_ctx.rdt_ok = 1; s_ctx.cam_view = &s_cam; s_ctx.active_cut = 0;

    re15_actor_init(); re15_aot_init(); scd_vm_init();
    re15_enemy_reset(); re15_enemy_ai_set_paused(0);
    re15_player_victim_reset();
    re15_damage_seed_rng(0x0badf00du);
    g_current_room_id = 0x1210;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100; pl->y = 0;
    re15_collision_set_band(0);
    scd_register_room_events(&s_rdt);
    scd_room_reenter(&s_rdt, 0, 0, 0);
    printf("EM01A-Bank: %s\n", load_bank(0x1A, s_blob1a, sizeof s_blob1a) ? "ok" : "FEHLT");

    for (int f = 0; f < 8; f++) frame_step();

    int arms[16]; int na = 0;
    for (int s = 1; s < RE15_ACTOR_MAX && na < 16; s++)
        if (g_actors[s].active && g_actors[s].type == 0x1A) arms[na++] = s;
    printf("Arme: %d | Spieler-Band %d\n", na, re15_collision_debug_band());

    /* ---------- (0) SCA-Zellen ---------- */
    printf("\n=== (0) SCA-Zellen (sca_count=%d, rgn=%d/%d/%d/%d/%d, ceil=(%d,%d)) ===\n",
           s_rdt.sca_count, s_rdt.sca_rgn[0], s_rdt.sca_rgn[1], s_rdt.sca_rgn[2],
           s_rdt.sca_rgn[3], s_rdt.sca_rgn[4],
           (int)(int16_t)s_rdt.ceiling_x, (int)(int16_t)s_rdt.ceiling_z);
    for (int i = 0; i < s_rdt.sca_count; i++) {
        const re15_sca_entry_t *e = &s_rdt.sca[i];
        printf("  #%3d typ=%d band=%d u0=0x%02x u1=0x%02x  x %7d..%7d  z %7d..%7d  (w=%u d=%u)\n",
               i, e->type, e->floor >> 4, e->u0, e->u1,
               (int)e->x, (int)e->x + (int)e->width, (int)e->z, (int)e->z + (int)e->density,
               (unsigned)e->width, (unsigned)e->density);
    }

    /* ---------- (0b) EINZELPUNKT-KONTROLLE des Klemmers ---------- */
    printf("\n=== (0b) Einzelpunkte: was macht re15_collision_constrain? (Band %d) ===\n",
           re15_collision_debug_band());
    {
        const int32_t pts[8][2] = {
            {-19500,-14000},{-12107,-5897},{-14900,-5897},{-14025,-5897},
            {-20640,-8847},{-20641,-8847},{-18147,-5897},{-18146,-5897}
        };
        for (int k = 0; k < 8; k++) {
            int32_t x = pts[k][0], z = pts[k][1];
            int32_t nx = x, nz = z;
            re15_collision_constrain(&s_rdt, x, z, &nx, &nz);
            int32_t ox2 = nx, oz2 = nz;
            re15_collision_objects(&ox2, &oz2);
            printf("  (%7ld,%7ld) -> Klemmer (%7ld,%7ld) -> Objekte (%7ld,%7ld) | on_floor=%d\n",
                   (long)x,(long)z,(long)nx,(long)nz,(long)ox2,(long)oz2,
                   re15_collision_on_floor(&s_rdt, x, z));
        }
    }

    /* ---------- (0c) Welche Zellen sieht der Klemmer an einem Punkt? ---------- */
    {
        const int32_t pp[3][2] = {{-12107,-5897},{-14900,-5897},{-19500,-14000}};
        for (int k = 0; k < 3; k++) {
            int32_t px = pp[k][0], pz = pp[k][1];
            unsigned zb = (unsigned)(pz - (int32_t)(int16_t)s_rdt.ceiling_z) & 0x80000000u;
            unsigned xb = (unsigned)(px - (int32_t)(int16_t)s_rdt.ceiling_x) & 0x80000000u;
            int q = (int)((zb | (xb >> 1)) >> 30);
            int st = 0; for (int i2 = 0; i2 < q && i2 < 5; i2++) st += s_rdt.sca_rgn[i2];
            int en = st + (q < 5 ? s_rdt.sca_rgn[q] : 0);
            printf("  (0c) (%ld,%ld): Quadrant %d -> Zellen %d..%d:", (long)px,(long)pz,q,st,en-1);
            for (int i2 = st; i2 < en && i2 < s_rdt.sca_count; i2++) {
                const re15_sca_entry_t *e2 = &s_rdt.sca[i2];
                if ((e2->floor >> 4) != 0) continue;
                if ((unsigned)(px - ((int32_t)e2->x - 450)) < (unsigned)((int32_t)e2->width + 900) &&
                    (unsigned)(pz - ((int32_t)e2->z - 450)) < (unsigned)((int32_t)e2->density + 900))
                    printf(" #%d", i2);
            }
            printf("\n");
        }
    }

    /* ---------- (1) Gitter: BESETZBAR = der Klemmer verschiebt den Punkt NICHT ----------
     * (Die SCA-Zellen dieses Raums sind WAENDE: der Spieler laeuft im Komplement,
     *  siehe Kopfkommentar re15_collision.c. on_floor==1 heisst hier: IN einer Zelle.) */
    for (int gz = 0; gz < GH; gz++)
        for (int gx = 0; gx < GW; gx++)
            g_occ[gz*GW+gx] = (uint8_t)occupiable(GX0 + gx*GS, GZ0 + gz*GS);

    int32_t seedz = -14000, seedx = -19500;
    int seeded = occupiable(seedx, seedz);
    printf("\nSaat (%ld,%ld): %s\n", (long)seedx, (long)seedz, seeded ? "besetzbar" : "NICHT besetzbar");

    memset(g_rea, 0, sizeof g_rea);
    if (seeded) {
        int sp = 0;
        int sx = (int)((seedx - GX0)/GS), sz2 = (int)((seedz - GZ0)/GS);
        g_rea[sz2*GW+sx] = 1; g_stack[sp++] = sz2*GW+sx;
        const int dxs[4] = {1,-1,0,0}, dzs[4] = {0,0,1,-1};
        while (sp > 0) {
            int c = g_stack[--sp]; int cx = c % GW, cz = c / GW;
            for (int k = 0; k < 4; k++) {
                int nx = cx + dxs[k], nz = cz + dzs[k];
                if (nx < 0 || nx >= GW || nz < 0 || nz >= GH) continue;
                if (g_rea[nz*GW+nx] || !g_occ[nz*GW+nx]) continue;
                g_rea[nz*GW+nx] = 1; g_stack[sp++] = nz*GW+nx;
            }
        }
    }
    { int no=0,nr=0; int32_t rx0=0x7fffffff,rx1=-0x7fffffff,rz0=0x7fffffff,rz1=-0x7fffffff;
      for (int gz=0;gz<GH;gz++) for(int gx=0;gx<GW;gx++){
          no+=g_occ[gz*GW+gx];
          if (g_rea[gz*GW+gx]) { nr++; int32_t X=GX0+gx*GS,Z=GZ0+gz*GS;
              if(X<rx0)rx0=X; if(X>rx1)rx1=X; if(Z<rz0)rz0=Z; if(Z>rz1)rz1=Z; } }
      printf("Gitter %dx%d (Schritt %d): besetzbar %d, erreichbar %d, Huelle x %ld..%ld z %ld..%ld\n",
             GW,GH,GS,no,nr,(long)rx0,(long)rx1,(long)rz0,(long)rz1); }

    /* ---------- (2) pro Arm: INTERVALLE auf der Arm-z-Linie ---------- */
    printf("\n=== (2) Pro Arm-z-Linie: Intervalle von on_floor==1 (= IN einer SCA-Zelle)\n");
    printf("        und von BESETZBAR (Klemmer r=450 @0x80073e9a, Maske 1 @0x80031d74) ===\n");
    for (int i = 0; i < na; i++) {
        re15_actor_t *e = &g_actors[arms[i]];
        printf("  slot %2d @(%7ld,%7ld) yaw=%4d\n", arms[i], (long)e->x, (long)e->z, (int)e->rot_y);
        for (int mode = 0; mode < 2; mode++) {
            printf("       %-10s:", mode ? "besetzbar" : "on_floor");
            int inrun = 0; int32_t r0 = 0, prev = 0; int nprint = 0;
            for (int32_t x = -30000; x <= -8000; x += 1) {
                int v = mode ? occupiable(x, e->z) : re15_collision_on_floor(&s_rdt, x, e->z);
                if (v && !inrun) { inrun = 1; r0 = x; }
                if (!v && inrun) { inrun = 0; if (nprint++ < 8) printf("  [%ld..%ld]", (long)r0, (long)prev); }
                prev = x;
            }
            if (inrun && nprint++ < 8) printf("  [%ld..%ld]", (long)r0, (long)prev);
            printf("\n");
        }
    }

    /* ---------- (3) pro Arm: kleinste Distanz vom ERREICHBAREN Raum ---------- */
    printf("\n=== (3) Kleinste Distanz vom ERREICHBAREN Spielerraum zu Spawn und Landepunkt ===\n");
    printf("    Landepunkt = Spawn + 2420 entlang +0x6a (netto der Lunge FUN_8010c714)\n");
    printf("    Original-Koerperkontakt FUN_8002aec4: 450 (@0x80073e9a) + 300 (@0x80120922) = 750\n");
    for (int i = 0; i < na; i++) {
        re15_actor_t *e = &g_actors[arms[i]];
        int t = (int)e->rot_y & 0xfff;
        int32_t lx = e->x + (int32_t)(((int64_t)2420 * re15_cos_q12(t)) >> 12);
        int32_t lz = e->z + (int32_t)(((int64_t)(-2420) * re15_sin_q12(t)) >> 12);
        int32_t bs = 0x7fffffff, bsx=0, bsz=0, bl = 0x7fffffff, blx=0, blz=0;
        for (int gz = 0; gz < GH; gz++) for (int gx = 0; gx < GW; gx++) {
            if (!g_rea[gz*GW+gx]) continue;
            int32_t px = GX0+gx*GS, pz = GZ0+gz*GS;
            int32_t d = idist(px-e->x, pz-e->z); if (d<bs){bs=d;bsx=px;bsz=pz;}
            d = idist(px-lx, pz-lz);             if (d<bl){bl=d;blx=px;blz=pz;}
        }
        printf("  slot %2d Spawn(%7ld,%7ld) -> naechster erreichbarer (%7ld,%7ld) d=%6ld %s\n",
               arms[i], (long)e->x, (long)e->z, (long)bsx, (long)bsz, (long)bs,
               bs < 750 ? "<750 KONTAKT" : "");
        printf("            Lande(%7ld,%7ld) -> naechster erreichbarer (%7ld,%7ld) d=%6ld %s%s\n",
               (long)lx, (long)lz, (long)blx, (long)blz, (long)bl,
               bl < 750 ? "<750 KONTAKT " : "", bl < 1200 ? "<1200 RE2-GRIFFTOR" : "");
    }

    /* ---------- (3b) Wer klemmt? Die Zelle an der Grenze ---------- */
    printf("\n=== (3b) Welche SCA-Zelle setzt die Grenze? (Zelle inflatiert um r=450 + 0x12 Haut) ===\n");
    for (int i = 0; i < na; i++) {
        re15_actor_t *e = &g_actors[arms[i]];
        int dir = (e->x < -19500) ? -1 : 1;
        int32_t lim = 0; int have = 0;
        for (int32_t x = -19500; x >= -30000 && x <= -8000; x += dir) {
            if (!occupiable(x, e->z)) break;
            lim = x; have = 1;
        }
        int32_t nx = lim + dir;
        int best = -1;
        for (int c = 0; c < s_rdt.sca_count; c++) {
            const re15_sca_entry_t *sc = &s_rdt.sca[c];
            if ((sc->floor >> 4) != 0) continue;
            if ((unsigned)(nx - ((int32_t)sc->x - 450)) < (unsigned)((int32_t)sc->width + 900) &&
                (unsigned)(e->z - ((int32_t)sc->z - 450)) < (unsigned)((int32_t)sc->density + 900))
                best = c;
        }
        printf("  slot %2d z=%7ld Richtung %+d: letzter besetzbarer x=%7ld%s | blockende Zelle #%d",
               arms[i], (long)e->z, dir, (long)lim, have?"":" (KEINER)", best);
        if (best >= 0) {
            const re15_sca_entry_t *sc = &s_rdt.sca[best];
            printf(" typ=%d x %d..%d z %d..%d", sc->type, (int)sc->x, (int)sc->x+(int)sc->width,
                   (int)sc->z, (int)sc->z+(int)sc->density);
        }
        printf(" | Abstand zum Arm %ld\n", (long)(e->x > lim ? e->x - lim : lim - e->x));
    }

    /* ---------- (4) echter Laufweg ---------- */
    printf("\n=== (4) Laufweg von der Flurmitte zur Arm-Seite: Klemmer + Objekte, Schritt 75/5/1 ===\n");
    for (int i = 0; i < na; i++) {
        re15_actor_t *e = &g_actors[arms[i]];
        int32_t startx = -19500;
        int dir = (e->x > startx) ? 1 : -1;
        const int steps[3] = {75, 5, 1};
        printf("  slot %2d z=%7ld Start x=%7ld Richtung %+d:", arms[i], (long)e->z, (long)startx, dir);
        for (int k = 0; k < 3; k++) {
            int32_t px = startx, pz = e->z;
            for (int f = 0; f < 40000; f++) {
                int32_t nx = px + dir*steps[k], nz = pz;
                re15_collision_constrain(&s_rdt, px, pz, &nx, &nz);
                re15_collision_objects(&nx, &nz);
                if (nx == px && nz == pz) break;
                px = nx; pz = nz;
                if (dir > 0 ? px > e->x + 4000 : px < e->x - 4000) break;
            }
            int32_t d = idist(px - e->x, pz - e->z);
            printf("  [%2d/Bild -> x=%7ld z=%7ld d=%5ld]", steps[k], (long)px, (long)pz, (long)d);
        }
        printf("\n");
    }

    /* ---------- (4b) HARTE OBERGRENZE der Arm-Bewegung: sein EIGENER Wandklemmer ----------
     * Wurzel-Tick @0x8010c308-24:  lw v0,120(a0) (+0x78) ; ori a2,zero,0x4 ; lhu a1,6(v0) ;
     * jal 0x8003b0a4 ; addiu a0,a0,52   -> Radius = *(u16*)(box+6) = 300 @0x80120922
     * (Box-Zeiger 0x8012091c aus 0x80120934, gesetzt @0x8010c3c4), Solid-Maske 4 @0x8010c318.
     * Frage: wie weit kaeme der Arm, wenn die Translation UNBEGRENZT waere? */
    printf("\n=== (4b) Arm mit UNBEGRENZTER Vorwaertsbewegung, eigener Klemmer r=300 Maske 4 ===\n");
    for (int i = 0; i < na; i++) {
        re15_actor_t *e = &g_actors[arms[i]];
        int t = (int)e->rot_y & 0xfff;
        int32_t px = e->x, pz = e->z;
        int stuck = 0;
        for (int f = 0; f < 4000 && stuck < 20; f++) {
            int32_t nx = px + (int32_t)(((int64_t)20 * re15_cos_q12(t)) >> 12);
            int32_t nz = pz + (int32_t)(((int64_t)(-20) * re15_sin_q12(t)) >> 12);
            re15_collision_constrain_enemy(&s_rdt, px, pz, &nx, &nz, 300, e->y, 4u);
            if (nx == px && nz == pz) stuck++; else stuck = 0;
            px = nx; pz = nz;
        }
        /* Spieler-Grenze auf dieser z-Linie, in Richtung des Arms */
        int dir = (e->x < -19500) ? -1 : 1;
        int32_t lim = -19500;
        for (int32_t x = -19500; x >= -30000 && x <= -8000; x += dir) {
            if (!occupiable(x, e->z)) break;
            lim = x;
        }
        int32_t tip = px + (int32_t)(((int64_t)1671 * re15_cos_q12(t)) >> 12);
        int32_t dorg = lim > px ? lim - px : px - lim;
        int32_t dtip = lim > tip ? lim - tip : tip - lim;
        printf("  slot %2d: Klemm-Endpunkt x=%7ld (byte-true Lunge waere %7ld) | Spieler-Grenze %7ld"
               " | Ursprungs-Abstand %5ld | Mesh-Spitze %7ld -> Abstand %5ld\n",
               arms[i], (long)px,
               (long)(e->x + (int32_t)(((int64_t)2420 * re15_cos_q12(t)) >> 12)),
               (long)lim, (long)dorg, (long)tip, (long)dtip);
    }

    /* ---------- (5) MESH-Reichweite ---------- */
    printf("\n=== (5) EM01A-MESH: groesste Auslenkung ueber alle Posen (yaw 0 -> vorn = +X) ===\n");
    {
        re15_enemy_bank_t *ab = re15_enemy_find(0x1A);
        if (!ab || !ab->ok) printf("  (Bank fehlt)\n");
        else {
            printf("  Meshes %d, Bones %d, Clips %d\n", ab->md1.mesh_count, ab->skel.bone_count, ab->anim.clip_count);
            int32_t gmaxx = -0x7fffffff; int gc=-1,gf=-1,gb=-1;
            int32_t gmaxr = 0;
            for (int c = 0; c < ab->anim.clip_count && c < 8; c++) {
                int32_t cmaxx = -0x7fffffff, cminx = 0x7fffffff, cmaxr = 0;
                int32_t cmaxy = -0x7fffffff, cminy = 0x7fffffff;
                int cfx=-1, cbx=-1;
                for (int f = 0; f < ab->anim.clips[c].frame_count; f++) {
                    int kf = (int)(ab->anim.frames[ab->anim.clips[c].first_frame + f] & 0xfffu);
                    static re15_skel_pose_t poses[RE15_EMD_MAX_BONES];
                    if (re15_skel_compute_pose(&ab->skel, kf, poses) != 0) continue;
                    for (int b = 0; b < ab->skel.bone_count && b < ab->md1.mesh_count; b++) {
                        const re15_skel_pose_t *p = &poses[b];
                        const re15_md1_mesh_t *m = &ab->md1.meshes[b];
                        for (int pass = 0; pass < 2; pass++) {
                            const re15_md1_vertex_t *vs = pass ? m->quad_vertices : m->tri_vertices;
                            int nv = pass ? m->quad_vertex_count : m->tri_vertex_count;
                            if (!vs) continue;
                            for (int v = 0; v < nv; v++) {
                                int used = 0;
                                if (!pass) {
                                    for (int ti = 0; ti < m->triangle_count && !used; ti++)
                                        if (m->triangles[ti].v0 == (uint16_t)v ||
                                            m->triangles[ti].v1 == (uint16_t)v ||
                                            m->triangles[ti].v2 == (uint16_t)v) used = 1;
                                } else {
                                    for (int qi = 0; qi < m->quad_count && !used; qi++)
                                        if (m->quads[qi].v0 == (uint16_t)v ||
                                            m->quads[qi].v1 == (uint16_t)v ||
                                            m->quads[qi].v2 == (uint16_t)v ||
                                            m->quads[qi].v3 == (uint16_t)v) used = 1;
                                }
                                if (!used) continue;
                                int64_t wx = (int64_t)p->rot[0]*vs[v].x + (int64_t)p->rot[1]*vs[v].y + (int64_t)p->rot[2]*vs[v].z;
                                int64_t wy = (int64_t)p->rot[3]*vs[v].x + (int64_t)p->rot[4]*vs[v].y + (int64_t)p->rot[5]*vs[v].z;
                                int64_t wz = (int64_t)p->rot[6]*vs[v].x + (int64_t)p->rot[7]*vs[v].y + (int64_t)p->rot[8]*vs[v].z;
                                int32_t X = (int32_t)(wx >> 12) + p->trans[0];
                                int32_t Y = (int32_t)(wy >> 12) + p->trans[1];
                                int32_t Z = (int32_t)(wz >> 12) + p->trans[2];
                                if (X > cmaxx) { cmaxx = X; cfx = f; cbx = b; }
                                if (X < cminx) cminx = X;
                                if (Y > cmaxy) cmaxy = Y;
                                if (Y < cminy) cminy = Y;
                                int32_t r = idist(X, Z);
                                if (r > cmaxr) cmaxr = r;
                            }
                        }
                    }
                }
                printf("  Clip %d (%3d Bilder): X %7ld..%7ld (VORN %ld, Bild %d Mesh %d) | Y %7ld..%7ld | max XZ-Radius %ld\n",
                       c, ab->anim.clips[c].frame_count, (long)cminx, (long)cmaxx, (long)cmaxx, cfx, cbx,
                       (long)cminy, (long)cmaxy, (long)cmaxr);
                if (c < 3 && cmaxx > gmaxx) { gmaxx = cmaxx; gc = c; gf = cfx; gb = cbx; }
                if (c < 3 && cmaxr > gmaxr) gmaxr = cmaxr;
            }
            printf("  >> Clips 0/1/2: groesste Vorwaerts-Auslenkung %ld (Clip %d, Bild %d, Mesh %d), groesster XZ-Radius %ld\n",
                   (long)gmaxx, gc, gf, gb, (long)gmaxr);
            printf("  Clip-2-Profil (Vorwaerts-Reichweite je Bild):");
            for (int f = 0; f < ab->anim.clips[2].frame_count; f++) {
                int kf = (int)(ab->anim.frames[ab->anim.clips[2].first_frame + f] & 0xfffu);
                static re15_skel_pose_t poses[RE15_EMD_MAX_BONES];
                if (re15_skel_compute_pose(&ab->skel, kf, poses) != 0) continue;
                int32_t mx = -0x7fffffff;
                for (int b = 0; b < ab->skel.bone_count && b < ab->md1.mesh_count; b++) {
                    const re15_skel_pose_t *p = &poses[b];
                    const re15_md1_mesh_t *m = &ab->md1.meshes[b];
                    for (int pass = 0; pass < 2; pass++) {
                        const re15_md1_vertex_t *vs = pass ? m->quad_vertices : m->tri_vertices;
                        int nv = pass ? m->quad_vertex_count : m->tri_vertex_count;
                        if (!vs) continue;
                        for (int v = 0; v < nv; v++) {
                            int64_t wx = (int64_t)p->rot[0]*vs[v].x + (int64_t)p->rot[1]*vs[v].y + (int64_t)p->rot[2]*vs[v].z;
                            int32_t X = (int32_t)(wx >> 12) + p->trans[0];
                            if (X > mx) mx = X;
                        }
                    }
                }
                printf(" %ld", (long)mx);
            }
            printf("\n");
        }
    }

    free(buf);
    printf("\nprobe_1210_geo: MESSUNG FERTIG\n");
    return 0;
}
