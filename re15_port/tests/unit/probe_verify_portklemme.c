/* probe_verify_portklemme.c — GEGENPRUEFUNG des Befundes "port-klemme" (2026-09-14).
 *
 * Misst die im Befund BEHAUPTETEN Schritte der Port-Klemme direkt an
 * collision_constrain_impl (re15_collision.c:696) statt sie zu glauben:
 *   A) Broadphase AUSSCHLIESSLICH auf der NEUEN Position (:729-730) -> Tunneln moeglich?
 *   B) +0x12-Haut in push_rect (FUN_8003bca8 @0x8003bce0/@0x8003bce8) -> Klemmabstand r+18?
 *   C) KEIN break: gewinnt wirklich die LETZTE treffende Zelle (@0x8003b510-20)?
 *   D) Kontakt-Clear *contact &= 0xf0 (@0x8003b1d0-dc) -> ueberlebt das obere Nibble?
 *   E) Quadranten-Partition: wird wirklich nur EINE Region gescannt — und ist
 *      Region 4 ueberhaupt erreichbar (FUN_8003b068 liefert nur 2 Bit)?
 *   F) Band strikt == (:726)
 *   G) ROOM1030 LIVE: welcher Gegner-Radius steht wirklich in den Aktoren?
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

static re15_sca_entry_t s_cells[8];
static re15_rdt_t       s_syn;

static void syn(int n, const int *rgn, int cx, int cz)
{
    memset(&s_syn, 0, sizeof s_syn);
    s_syn.sca = s_cells; s_syn.sca_count = n;
    for (int i = 0; i < 5; i++) s_syn.sca_rgn[i] = rgn[i];
    s_syn.ceiling_x = (uint16_t)(int16_t)cx;
    s_syn.ceiling_z = (uint16_t)(int16_t)cz;
}
static void cell(int i, int x, int z, int w, int d, int type, int u0, int u1, int floor)
{
    s_cells[i].x = (int16_t)x; s_cells[i].z = (int16_t)z;
    s_cells[i].width = (uint16_t)w; s_cells[i].density = (uint16_t)d;
    s_cells[i].type = (uint8_t)type; s_cells[i].u0 = (uint8_t)u0;
    s_cells[i].u1 = (uint8_t)u1; s_cells[i].floor = (uint8_t)floor;
}

/* Band 0 => enemy_y 0 (band_from_y(0) = 0). */
static int clamp0(int32_t ox, int32_t oz, int32_t *x, int32_t *z, int32_t r,
                  uint8_t *c, uint16_t *a)
{
    return re15_collision_constrain_contact(&s_syn, ox, oz, x, z, r, 0, 4u, c, a);
}

static uint8_t *slurp(const char *p, size_t *n)
{
    FILE *f = fopen(p, "rb"); if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    if (sz <= 0) { fclose(f); return NULL; }
    uint8_t *b = (uint8_t *)malloc((size_t)sz);
    if (b && fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); b = NULL; }
    fclose(f); if (b) *n = (size_t)sz; return b;
}

static re15_rdt_t         s_rdt;
static re15_camera_view_t s_cam;
static re15_game_ctx_t    s_ctx;

int main(void)
{
    const int R1[5] = {1,0,0,0,0};
    int32_t x, z; uint8_t c; uint16_t a; int hit;

    printf("=== A) Broadphase nur auf der NEUEN Position? ===\n");
    syn(1, R1, 0, 0);
    cell(0, 0, 0, 1000, 1000, 1, 0xff, 0, 0x00);
    x = 6000; z = 500; c = 0; a = 0;
    hit = clamp0(-5000, 500, &x, &z, 400, &c, &a);
    printf("  Schritt (-5000,500) -> (6000,500) quer durch die Zelle x[0..1000]: "
           "hit=%d Ergebnis=(%d,%d)  %s\n", hit, (int)x, (int)z,
           (x == 6000) ? "DURCHGETUNNELT (nur Endpunkt getestet)" : "geklemmt");

    printf("=== B) +0x12-Haut: Klemmabstand zur Zellflaeche ===\n");
    for (int r = 400; r <= 450; r += 50) {
        x = 200; z = 500; c = 0; a = 0;
        hit = clamp0(-5000, 500, &x, &z, r, &c, &a);
        printf("  r=%d: Ziel x=200 -> x=%d (erwartet -(r+18) = %d) hit=%d contact=0x%02x attr=0x%04x\n",
               r, (int)x, -(r + 18), hit, c, a);
        x = -218; z = 500; c = 0; a = 0;
        (void)clamp0(-5000, 500, &x, &z, r, &c, &a);
        printf("     flacher Zweig (Ziel x=-218): x=%d\n", (int)x);
    }

    printf("=== C) Letzte treffende Zelle gewinnt (kein break)? ===\n");
    {
        const int R2[5] = {2,0,0,0,0};
        /* C1: zwei DECKUNGSGLEICHE Zellen — die erste schiebt heraus, die zweite
         *     faellt danach durch die Broadphase (die auf der bereits GESCHOBENEN
         *     Position laeuft). */
        syn(2, R2, 0, 0);
        cell(0, 0, 0, 1000, 1000, 1, 0xff, 1, 0x00);   /* attr 0x0001 */
        cell(1, 0, 0, 1000, 1000, 1, 0xff, 2, 0x03);   /* attr 0x0302 */
        x = 200; z = 500; c = 0; a = 0;
        hit = clamp0(-5000, 500, &x, &z, 400, &c, &a);
        printf("  C1 deckungsgleich (attr 0x0001 / 0x0302): x=%d attr=0x%04x contact=0x%02x hit=%d -> %s\n",
               (int)x, a, c, hit, (a == 0x0302) ? "LETZTE gewinnt" : "ERSTE gewinnt (2. Zelle faellt aus der Broadphase)");
        /* C2: zweite Zelle so gross, dass sie auch NACH dem ersten Schub noch trifft. */
        syn(2, R2, 0, 0);
        cell(0,     0, 0, 1000, 1000, 1, 0xff, 1, 0x00);   /* attr 0x0001 */
        cell(1, -5000, 0,10000, 1000, 1, 0xff, 2, 0x03);   /* attr 0x0302, umschliesst alles */
        x = 200; z = 500; c = 0; a = 0;
        hit = clamp0(-5000, 500, &x, &z, 400, &c, &a);
        printf("  C2 zweite Zelle trifft auch nach dem Schub: x=%d attr=0x%04x contact=0x%02x hit=%d -> %s\n",
               (int)x, a, c, hit, (a == 0x0302) ? "LETZTE gewinnt" : "ERSTE gewinnt");
        /* C3: Reihenfolge umgedreht. */
        syn(2, R2, 0, 0);
        cell(0, -5000, 0,10000, 1000, 1, 0xff, 2, 0x03);
        cell(1,     0, 0, 1000, 1000, 1, 0xff, 1, 0x00);
        x = 200; z = 500; c = 0; a = 0;
        hit = clamp0(-5000, 500, &x, &z, 400, &c, &a);
        printf("  C3 umgekehrte Reihenfolge: x=%d attr=0x%04x contact=0x%02x hit=%d\n",
               (int)x, a, c, hit);
        /* C4: Masken-Test (mask & u0) — u0 ohne Bit 2 darf NICHT klemmen. */
        syn(1, R1, 0, 0);
        cell(0, 0, 0, 1000, 1000, 1, 0xf3, 0, 0x00);
        x = 200; z = 500; c = 0; a = 0;
        hit = clamp0(-5000, 500, &x, &z, 400, &c, &a);
        printf("  C4 u0=0xF3 gegen Maske 4: hit=%d x=%d (erwartet 0 / 200)\n", hit, (int)x);
    }

    printf("=== D) Kontakt-Clear &= 0xf0 ===\n");
    {
        syn(1, R1, 0, 0);
        cell(0, 0, 0, 1000, 1000, 1, 0xff, 0, 0x00);
        x = 30000; z = 30000; c = 0xA5; a = 0x1234;
        hit = clamp0(30000, 30000, &x, &z, 400, &c, &a);
        printf("  kontaktfreier Frame: contact 0xA5 -> 0x%02x (erwartet 0xA0), attr 0x1234 -> 0x%04x, hit=%d\n",
               c, a, hit);
    }

    printf("=== E) Quadranten-Partition / Region 4 ===\n");
    {
        /* Vier identische Sperr-Zellen, eine je Region. Der Aktor steht immer an
         * derselben Stelle; nur die ceiling-Referenz waehlt den Quadranten. */
        const int R4[5] = {1,1,1,1,0};
        for (int q = 0; q < 4; q++) {
            syn(4, R4, 0, 0);
            for (int i = 0; i < 4; i++) cell(i, 0, 0, 1000, 1000, 1, 0xff, (uint8_t)(i + 1), 0x00);
            /* q = 2*(pz < ceilz) + (px < ceilx); Aktor bei (200,500) */
            int cx = (q & 1) ? 30000 : -30000;
            int cz = (q & 2) ? 30000 : -30000;
            s_syn.ceiling_x = (uint16_t)(int16_t)cx;
            s_syn.ceiling_z = (uint16_t)(int16_t)cz;
            x = 200; z = 500; c = 0; a = 0;
            hit = clamp0(-5000, 500, &x, &z, 400, &c, &a);
            printf("  ceiling=(%6d,%6d) -> getroffene Zelle u1=%u (erwartet %d) hit=%d x=%d\n",
                   cx, cz, (unsigned)(a & 0xff), q + 1, hit, (int)x);
        }
        /* Region 4 allein bestueckt: erreichbar? */
        const int R5[5] = {0,0,0,0,1};
        syn(1, R5, -30000, -30000);
        cell(0, 0, 0, 1000, 1000, 1, 0xff, 9, 0x00);
        x = 200; z = 500; c = 0; a = 0;
        hit = clamp0(-5000, 500, &x, &z, 400, &c, &a);
        printf("  NUR Region 4 bestueckt: hit=%d x=%d -> Region 4 ist %s\n",
               hit, (int)x, hit ? "erreichbar" : "NIE erreichbar (q hat nur 2 Bit)");
    }

    printf("=== F) Band strikt == ===\n");
    {
        syn(1, R1, 0, 0);
        cell(0, 0, 0, 1000, 1000, 1, 0xff, 0, 0x10);   /* floor 0x10 -> Band 1 */
        x = 200; z = 500; c = 0; a = 0;
        hit = clamp0(-5000, 500, &x, &z, 400, &c, &a);
        printf("  Zelle Band 1, Aktor Band 0: hit=%d x=%d\n", hit, (int)x);
    }

    printf("=== G) ROOM1030 LIVE: Gegner-Radien ===\n");
    {
        size_t n = 0;
        uint8_t *d = slurp(RE15_ASSET_PSX_DIR "/STAGE1/ROOM1030.RDT", &n);
        if (!d || re15_rdt_parse(d, n, &s_rdt) != 0) { printf("  RDT fehlt\n"); return 0; }
        memset(&s_cam, 0, sizeof s_cam); memset(&s_ctx, 0, sizeof s_ctx);
        s_ctx.rdt = &s_rdt; s_ctx.rdt_ok = 1; s_ctx.cam_view = &s_cam; s_ctx.active_cut = 0;
        re15_actor_init(); re15_aot_init(); scd_vm_init();
        re15_enemy_reset(); re15_enemy_ai_set_paused(0);
        re15_player_cmd_reset(); re15_damage_seed_rng(0x2545f491u);
        g_current_room_id = 0x1030;
        re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
        pl->active = 1; pl->type = 0; pl->hp = 100; pl->x = -12100; pl->z = -26000;
        re15_collision_set_band(0);
        scd_room_reenter(&s_rdt, pl->x, pl->z, 0);
        printf("  SCA counts=%d/%d/%d/%d/%d total=%d ceiling=(%d,%d)\n",
               s_rdt.sca_rgn[0], s_rdt.sca_rgn[1], s_rdt.sca_rgn[2], s_rdt.sca_rgn[3],
               s_rdt.sca_rgn[4], s_rdt.sca_count,
               (int)(int16_t)s_rdt.ceiling_x, (int)(int16_t)s_rdt.ceiling_z);
        for (int s = 1; s < RE15_ACTOR_MAX; s++) {
            re15_actor_t *e = &g_actors[s];
            if (!e->active) continue;
            printf("  Slot %2d typ=0x%02x r_min=%u r_max=%u h=%u floor=%u pos=(%d,%d)\n",
                   s, e->type, (unsigned)e->hit_radius_min, (unsigned)e->hit_radius_max,
                   (unsigned)e->hit_height, (unsigned)e->floor, (int)e->x, (int)e->z);
        }
        printf("  Spieler r_min=%u\n", (unsigned)pl->hit_radius_min);
    }
    return 0;
}
