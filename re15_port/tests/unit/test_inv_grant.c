/* test_inv_grant.c — byte-true world-item pickup INSERT (FUN_8004dc4c, audit wf_6eea7fa1).
 *
 * The world pickup does NOT stack onto an existing same-id slot and does NOT clamp to max_stack
 * (those live only in the menu combine/reload paths). Weapons 0x0e..0x13 are 2-cell "wide" items
 * that front-shift; everything else takes the first free slot.
 */
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "re15_inventory.h"

static int g_fail = 0;
#define CHECK(name, cond) do { if (!(cond)) { printf("  FAIL: %s\n", name); g_fail = 1; } \
                               else printf("  PASS: %s\n", name); } while (0)

int main(void)
{
    printf("=== byte-true inventory world-pickup INSERT (wf_6eea7fa1) ===\n");

    /* --- (1) NO auto-stack: the same ammo picked up twice occupies TWO slots (not one merged) --- */
    {
        re15_inv_init();
        re15_inv_grant(0x15, 30);          /* H.GUN BULLETS x30 */
        re15_inv_grant(0x15, 20);          /* another x20 — must NOT merge into the first slot */
        CHECK("no-stack: slot0 = {0x15, 30}", g_inv.slots[0].id == 0x15 && g_inv.slots[0].qty == 30);
        CHECK("no-stack: slot1 = {0x15, 20} (separate slot, NOT merged to 50)",
              g_inv.slots[1].id == 0x15 && g_inv.slots[1].qty == 20);
    }

    /* --- (2) first-free-slot placement + flags 0 --- */
    {
        re15_inv_init();
        g_inv.slots[0].id = 0x03; g_inv.slots[0].qty = 15;   /* occupy slot 0 (handgun) */
        int rc = re15_inv_grant(0x22, 1);                    /* a key -> first FREE slot = 1 */
        CHECK("first-free: grant returns 0", rc == 0);
        CHECK("first-free: placed in slot 1, flags 0",
              g_inv.slots[1].id == 0x22 && g_inv.slots[1].qty == 1 && g_inv.slots[1].flags == 0);
    }

    /* --- (3) WIDE weapon (0x0e..0x13) front-shift: goes to slots 0/1 with flags 1/2, others +2 --- */
    {
        re15_inv_init();
        g_inv.slots[0].id = 0x01; g_inv.slots[0].qty = 0;    /* knife */
        g_inv.slots[1].id = 0x03; g_inv.slots[1].qty = 15;   /* handgun */
        re15_inv_grant(0x12, 5);                             /* ROCKET LAUNCHER (wide weapon) */
        CHECK("wide: slot0 = {0x12, flags 1}",  g_inv.slots[0].id == 0x12 && g_inv.slots[0].flags == 1);
        CHECK("wide: slot1 = {0x12, flags 2}",  g_inv.slots[1].id == 0x12 && g_inv.slots[1].flags == 2);
        CHECK("wide: old slot0 knife shifted to slot 2",  g_inv.slots[2].id == 0x01);
        CHECK("wide: old slot1 handgun shifted to slot 3", g_inv.slots[3].id == 0x03 && g_inv.slots[3].qty == 15);
    }

    /* --- (4) full inventory refuses the pickup --- */
    {
        re15_inv_init();
        for (int i = 0; i < RE15_INV_MAX_SLOTS; i++) { g_inv.slots[i].id = 0x22; g_inv.slots[i].qty = 1; }
        CHECK("full: grant returns -1", re15_inv_grant(0x15, 10) == -1);
    }

    /* --- (5) reject id 0 / amount 0 --- */
    {
        re15_inv_init();
        CHECK("reject id 0",     re15_inv_grant(0, 5) == -1);
        CHECK("reject amount 0", re15_inv_grant(0x15, 0) == -1);
    }

    if (g_fail) { printf("INV-GRANT: FAIL\n"); return 1; }
    printf("INV-GRANT: all checks passed\n");
    return 0;
}
