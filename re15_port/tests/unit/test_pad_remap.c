/*
 * test_pad_remap.c — wave-6 finding 4: the VIRTUAL (config-remapped) pad-word builder
 * re15_pad_virtual_word (pad_common.c) against the MZD default preset-0 table
 * @0x80073dbc, applied by FUN_80030444 @0x800304b8-e4:
 *   for i in 15..0: if (raw & table[i]) virtual |= 1<<i
 * Table bytes (PSX.EXE, re-read this pass):
 *   00 10 00 20 00 40 00 80 00 10 00 40 80 00 80 00
 *   08 00 40 00 08 00 04 00 00 80 00 20 80 00 40 00
 * = u16[16] { UP, RIGHT, DOWN, LEFT, UP, DOWN, SQ, SQ, R1, CROSS, R1, L1,
 *             LEFT, RIGHT, SQ, CROSS } in the PSX raw layout (dpad 0x1000/0x2000/
 * 0x4000/0x8000, SQUARE 0x0080, CROSS 0x0040, R1 0x0008, L1 0x0004). The port
 * table is the same mapping in RE15_PAD_BIT layout. Key facts under test:
 *   virtual CONFIRM 0x4000 <- RAW SQUARE  (entry 14)
 *   virtual CANCEL  0x8000 <- RAW CROSS   (entry 15)
 *   virtual menu-L/R 0x1000/0x2000 <- RAW d-pad LEFT/RIGHT (entries 12/13)
 */
#include <stdio.h>
#include "re15_engine.h"
#include "re15_player.h"

static int fails = 0;
#define CHECK(cond, ...) do { \
    if (!(cond)) { fails++; printf("FAIL: " __VA_ARGS__); printf("\n"); } \
} while (0)

int main(void)
{
    /* single-button probes: expected virtual word per the @0x80073dbc entries */
    struct { uint16_t phys; uint16_t virt; const char *name; } cases[] = {
        { RE15_PAD_BIT_UP,     0x0001 | 0x0010,          "UP -> virt 0x0011"         },
        { RE15_PAD_BIT_RIGHT,  0x0002 | 0x2000,          "RIGHT -> virt 0x2002"      },
        { RE15_PAD_BIT_DOWN,   0x0004 | 0x0020,          "DOWN -> virt 0x0024"       },
        { RE15_PAD_BIT_LEFT,   0x0008 | 0x1000,          "LEFT -> virt 0x1008"       },
        { RE15_PAD_BIT_SQUARE, 0x0040 | 0x0080 | 0x4000, "SQUARE -> virt 0x40c0"     },
        { RE15_PAD_BIT_CROSS,  0x0200 | 0x8000,          "CROSS -> virt 0x8200"      },
        { RE15_PAD_BIT_R1,     0x0100 | 0x0400,          "R1 -> virt 0x0500"         },
        { RE15_PAD_BIT_L1,     0x0800,                   "L1 -> virt 0x0800"         },
        { RE15_PAD_BIT_TRIANGLE, 0x0000,                 "TRIANGLE -> no virt bit"   },
        { RE15_PAD_BIT_CIRCLE,   0x0000,                 "CIRCLE -> no virt bit"     },
        { RE15_PAD_BIT_START,    0x0000,                 "START -> no virt bit (raw-only)" },
        { 0,                     0x0000,                 "no input -> 0"             },
    };
    unsigned i;
    for (i = 0; i < sizeof cases / sizeof cases[0]; i++) {
        uint16_t got = re15_pad_virtual_word(cases[i].phys);
        CHECK(got == cases[i].virt, "%s: got 0x%04x want 0x%04x",
              cases[i].name, got, cases[i].virt);
    }

    /* the load-bearing finding-4 bits */
    CHECK((re15_pad_virtual_word(RE15_PAD_BIT_SQUARE) & 0x4000) != 0,
          "virtual CONFIRM 0x4000 must come from RAW SQUARE (@0x80073dbc[14])");
    CHECK((re15_pad_virtual_word(RE15_PAD_BIT_CROSS) & 0x4000) == 0,
          "RAW CROSS must NOT set virtual confirm 0x4000");
    CHECK((re15_pad_virtual_word(RE15_PAD_BIT_CROSS) & 0x8000) != 0,
          "virtual CANCEL 0x8000 must come from RAW CROSS (@0x80073dbc[15])");
    CHECK((re15_pad_virtual_word(RE15_PAD_BIT_SQUARE) & 0x8000) == 0,
          "RAW SQUARE must NOT set virtual cancel 0x8000");
    CHECK((re15_pad_virtual_word((uint16_t)(RE15_PAD_BIT_LEFT | RE15_PAD_BIT_RIGHT)) & 0x3000) == 0x3000,
          "YES/NO toggle mask 0x3000 = raw d-pad LEFT|RIGHT (@0x80073dbc[12..13])");
    CHECK((re15_pad_virtual_word((uint16_t)(RE15_PAD_BIT_TRIANGLE | RE15_PAD_BIT_CIRCLE)) & 0x3000) == 0,
          "TRIANGLE/CIRCLE must NOT toggle (old mislabel)");

    /* union linearity: builder is bitwise-OR of single-button results (all table
     * entries are single physical bits — the edge-word equivalence argument) */
    {
        uint16_t all = (uint16_t)(RE15_PAD_BIT_SQUARE | RE15_PAD_BIT_CROSS |
                                  RE15_PAD_BIT_UP | RE15_PAD_BIT_LEFT);
        uint16_t sum = (uint16_t)(re15_pad_virtual_word(RE15_PAD_BIT_SQUARE) |
                                  re15_pad_virtual_word(RE15_PAD_BIT_CROSS)  |
                                  re15_pad_virtual_word(RE15_PAD_BIT_UP)     |
                                  re15_pad_virtual_word(RE15_PAD_BIT_LEFT));
        CHECK(re15_pad_virtual_word(all) == sum, "builder must be union-linear");
    }

    if (fails) { printf("%d FAILURES\n", fails); return 1; }
    printf("test_pad_remap: all checks passed\n");
    return 0;
}
