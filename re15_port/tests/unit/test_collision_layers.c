/* test_collision_layers.c — the byte-true per-actor SCA wall clamp (audit COLL-DISPATCH-MASK / radius).
 *
 * FUN_8003b0a4 is parameterized on (radius, solid-mask, band): a cell is solid to an actor iff
 * (mask & cell.u0); the player passes mask 1, enemies pass mask 4 (entity+0x1d7). The port had baked
 * the player mask/radius (1, 450) into re15_collision_constrain for ALL actors, so enemies collided
 * against the wrong cells (ROOM11C0 maggot through its u0=0x04 enemy-solid wall) at the wrong radius.
 * re15_collision_constrain_enemy now passes the enemy's radius + mask 4. Also: re15_body_push uses the
 * FULL 32-bit delta (FUN_8002aec4 subu), not an s16-truncated one. Asserts:
 *   (1) an ENEMY-solid cell (u0=0x04) pushes the ENEMY but the PLAYER passes through.
 *   (2) a PLAYER-solid cell (u0=0xfb) pushes the PLAYER but the ENEMY passes through.
 *   (3) a bigger enemy radius pushes further (r=1600 vs the player r=450).
 *   (4) re15_body_push rejects a far pair (full delta), does not s16-wrap into a false push.
 */
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "re15_rdt.h"
#include "re15_collision.h"
#include "re15_actor.h"
#include "re15_enemy_ai.h"

static void make_cell(re15_rdt_t *rdt, re15_sca_entry_t *e, uint8_t u0)
{
    memset(rdt, 0, sizeof *rdt);
    rdt->sca = e; rdt->sca_count = 1; rdt->sca_rgn[0] = 1;
    rdt->ceiling_x = 0x8000u; rdt->ceiling_z = 0x8000u;   /* -> quadrant 0 covers entry 0 */
    memset(e, 0, sizeof *e);
    e->type = 1; e->x = -1000; e->z = -1000; e->width = 2000; e->density = 2000;   /* rect (-1000..1000)^2 */
    e->u0 = u0; e->floor = 0;                              /* band 0 */
    re15_collision_set_band(0);                            /* player band 0 (matches the cell) */
}

int main(void)
{
    int fail = 0;
    printf("=== per-actor SCA wall clamp: solid-mask + radius (FUN_8003b0a4 args) ===\n");

    /* came from slightly toward the centre -> push ejects toward +x. */
    const int32_t PX = 800, PZ = 0, OX = 740, OZ = 0;

    /* (1) enemy-solid cell (u0=0x04, player-transparent): player passes, enemy pushed. */
    {
        re15_rdt_t rdt; re15_sca_entry_t e; make_cell(&rdt, &e, 0x04);
        int32_t nx = PX, nz = PZ;
        re15_collision_constrain(&rdt, OX, OZ, &nx, &nz);          /* player mask 1: 1&0x04=0 -> skip */
        if (nx != PX || nz != PZ) { fprintf(stderr, "FAIL(1a): player must PASS an enemy-solid cell (u0=0x04), moved to (%d,%d)\n", nx, nz); fail = 1; }
        nx = PX; nz = PZ;
        re15_collision_constrain_enemy(&rdt, OX, OZ, &nx, &nz, 450, 0);   /* enemy mask 4: 4&0x04=4 -> push */
        if (nx == PX && nz == PZ) { fprintf(stderr, "FAIL(1b): enemy must be PUSHED out of the enemy-solid cell\n"); fail = 1; }
        if (!fail) printf("  (1) enemy-solid u0=0x04: player passes, enemy pushed out\n");
    }

    /* (2) player-solid cell (u0=0xfb, enemy-transparent): player pushed, enemy passes. */
    {
        re15_rdt_t rdt; re15_sca_entry_t e; make_cell(&rdt, &e, 0xfb);
        int32_t nx = PX, nz = PZ;
        re15_collision_constrain(&rdt, OX, OZ, &nx, &nz);          /* player: 1&0xfb=1 -> push */
        if (nx == PX && nz == PZ) { fprintf(stderr, "FAIL(2a): player must be PUSHED out of a player-solid cell (u0=0xfb)\n"); fail = 1; }
        nx = PX; nz = PZ;
        re15_collision_constrain_enemy(&rdt, OX, OZ, &nx, &nz, 450, 0);   /* enemy: 4&0xfb=0 -> skip */
        if (nx != PX || nz != PZ) { fprintf(stderr, "FAIL(2b): enemy must PASS a player-solid cell (u0=0xfb), moved to (%d,%d)\n", nx, nz); fail = 1; }
        if (!fail) printf("  (2) player-solid u0=0xfb: player pushed out, enemy passes\n");
    }

    /* (3) a bigger radius reaches further: a point at x=1500 is OUTSIDE the cell (face +x=1000) but
     *     inside the r=1600 broad-phase (face+1600=2600) while OUTSIDE the r=450 one (face+450=1450).
     *     So the r=1600 enemy is pushed out (standoff 1600), the r=450 player is left alone. */
    {
        re15_rdt_t rdt; re15_sca_entry_t e; make_cell(&rdt, &e, 0x05);   /* solid to both (bit0 + bit2) */
        const int32_t QX = 1500, QOX = 1440;                            /* just outside the +x face */
        int32_t sx = QX, sz = 0; re15_collision_constrain(&rdt, QOX, 0, &sx, &sz);              /* r=450 */
        int32_t bx = QX, bz = 0; re15_collision_constrain_enemy(&rdt, QOX, 0, &bx, &bz, 1600, 0); /* r=1600 */
        if (sx != QX) { fprintf(stderr, "FAIL(3a): r=450 player must NOT reach x=1500 (broad-phase 1450), moved to %d\n", sx); fail = 1; }
        if (bx == QX) { fprintf(stderr, "FAIL(3b): r=1600 enemy must be CAUGHT at x=1500 (broad-phase 2600), stayed %d\n", bx); fail = 1; }
        if (!fail) printf("  (3) larger radius reaches further: r450 leaves x=1500 alone, r1600 catches+moves it (->%d)\n", bx);
    }

    /* (4) re15_body_push: a far pair (65100 on x) must reject via the full 32-bit delta, not s16-wrap. */
    {
        re15_actor_t A, B; memset(&A, 0, sizeof A); memset(&B, 0, sizeof B);
        A.x = 0; A.z = 0; B.x = 65100; B.z = 0;      /* true |dx|=65100 >> R=850 -> reject */
        int pushed = re15_body_push(&A, 450, &B, 400);
        if (pushed != 0 || B.x != 65100) { fprintf(stderr, "FAIL(4a): far pair (dx=65100) must NOT push (got pushed=%d, B.x=%d)\n", pushed, B.x); fail = 1; }
        /* control: a close pair still pushes. */
        B.x = 500; B.z = 0;
        pushed = re15_body_push(&A, 450, &B, 400);
        if (pushed != 1) { fprintf(stderr, "FAIL(4b): close pair (dx=500 < R=850) must push\n"); fail = 1; }
        if (!fail) printf("  (4) body-push: far pair rejected (full delta), close pair pushed\n");
    }

    if (fail) { printf("COLLISION-LAYERS: FAIL\n"); return 1; }
    printf("COLLISION-LAYERS: all checks passed\n");
    return 0;
}
