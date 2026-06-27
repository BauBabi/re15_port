/*
 * Unit tests for screen-space backface detection (Phase 4.5.4).
 *
 * Mirrors the IS_BACKFACE macro used in re15_reborn_pc/main.c and the
 * gte_nclip equivalent on PSX. Both targets MUST agree on which winding
 * is "front" or culling-driven visual diff would surface.
 *
 * Math: 2D cross product Z component
 *        z = (b.x - a.x) * (c.y - a.y) - (b.y - a.y) * (c.x - a.x)
 *   z > 0  → counter-clockwise → front-facing (standard convention)
 *   z <= 0 → clockwise or degenerate → back-facing
 *
 * The macro in main.c is `<= 0` so we replicate that here.
 */
#include <stdio.h>

#define TEST(cond, msg)                                                \
    do {                                                                \
        if (!(cond)) {                                                  \
            fprintf(stderr, "FAIL %s:%d %s — %s\n",                     \
                    __FILE__, __LINE__, __func__, msg);                 \
            return 1;                                                   \
        }                                                               \
    } while (0)

/* Mirror the macro from re15_reborn_pc/main.c. `long long` is required
 * because Windows MSVC `long` is only 32 bits and a 30000×60000 product
 * already exceeds s32 range — overflow would mis-classify large meshes. */
static int is_backface(int ax, int ay, int bx, int by, int cx, int cy)
{
    long long cross_z = (long long)(bx - ax) * (cy - ay)
                      - (long long)(by - ay) * (cx - ax);
    return cross_z <= 0;
}

static int test_ccw_is_front(void)
{
    /* Counter-clockwise triangle in screen coords (Y grows downward) =
     * actually appears CLOCKWISE to a viewer. PSX convention treats CW
     * winding (positive cross) as front. Pick winding accordingly. */
    /* Triangle: (10, 10) → (30, 10) → (20, 30) — clockwise in screen-y-down */
    TEST(!is_backface(10, 10, 30, 10, 20, 30), "clockwise screen-y-down = front");
    return 0;
}

static int test_cw_reversed_is_back(void)
{
    /* Same triangle reversed: (10, 10) → (20, 30) → (30, 10) — back-facing */
    TEST(is_backface(10, 10, 20, 30, 30, 10), "reversed winding = back");
    return 0;
}

static int test_degenerate_is_back(void)
{
    /* All three points on the same line → zero area → cross == 0 → treated
     * as back (we cull it, since it'd render as a sliver). */
    TEST(is_backface(0, 0, 10, 0, 20, 0), "collinear is culled");
    TEST(is_backface(5, 5, 5, 5, 5, 5), "single point is culled");
    return 0;
}

static int test_large_coords_no_overflow(void)
{
    /* MD1 verts can project to large screen offsets if the mesh is close.
     * Verify a deliberately huge triangle doesn't overflow s32 cross product
     * (we use `long` which is at least 32-bit; on Windows long is exactly 32).
     * 30000 * 30000 = 9e8, fits in s32. 30000 * 60000 = 1.8e9, still fits. */
    TEST(!is_backface(-30000, -30000, 30000, -30000, 0, 30000),
         "large coords still classify correctly");
    return 0;
}

int main(void)
{
    int fails = 0;
    fails += test_ccw_is_front();
    fails += test_cw_reversed_is_back();
    fails += test_degenerate_is_back();
    fails += test_large_coords_no_overflow();

    if (fails == 0) {
        printf("test_backface: all 4 tests PASSED\n");
        return 0;
    } else {
        printf("test_backface: %d test(s) FAILED\n", fails);
        return 1;
    }
}
