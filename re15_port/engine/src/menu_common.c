/*
 * menu_common.c — Inventory menu LOGIC (Phase 8.20/8.21, 2026-07-02).
 *
 * The g_inv-driven inventory screen's engine-side, headless-testable half (the render half is
 * platform-side: re15_menu_render in the PC main loop). See re15_menu.h + RE15_INVENTORY_SUBSYSTEM.md.
 *
 * Byte-true: the open trigger (START edge); the byte-true 2-col column-major cursor nav (FUN_800487b0
 * ±1-parity / ±2, no wrap); the EQUIP commit (DAT_800aca5d = item id then re-prime the ARMS SE bank —
 * the order of @0x80046688 + @0x800466c4), gated to WEAPONS (id < 0x15, the byte-true id-range gate).
 * The display list = the occupied g_inv slots compacted into the grid (matching FUN_8004dc4c).
 */
#include "re15_menu.h"
#include "re15_player.h"     /* RE15_PAD_BIT_* */
#include "re15_damage.h"     /* re15_player_equipped_weapon / re15_player_set_equipped_weapon */
#include "re15_audio.h"      /* re15_audio_prime_weapon (re-load the ARMS SE bank on equip) */
#include "re15_inventory.h"  /* g_inv, re15_item_is_weapon */

#define MENU_MAX_CELLS 10    /* 2 cols × 5 rows (DAT_80076274) */

static int     s_open   = 0;
static int     s_cursor = 0;
static int     s_count  = 0;
static uint8_t s_disp_slot[MENU_MAX_CELLS];   /* display index -> g_inv slot index */

int     re15_menu_is_open(void) { return s_open; }
int     re15_menu_count(void)   { return s_count; }
int     re15_menu_cursor(void)  { return s_cursor; }
uint8_t re15_menu_disp_id(int i)  { return (i >= 0 && i < s_count) ? g_inv.slots[s_disp_slot[i]].id  : 0; }
uint8_t re15_menu_disp_qty(int i) { return (i >= 0 && i < s_count) ? g_inv.slots[s_disp_slot[i]].qty : 0; }

/* Snapshot the occupied g_inv slots into the compacted display list (matches the game's compaction). */
static void rebuild_display(void)
{
    s_count = 0;
    for (int i = 0; i < RE15_INV_MAX_SLOTS && s_count < MENU_MAX_CELLS; i++)
        if (g_inv.slots[i].id != 0)
            s_disp_slot[s_count++] = (uint8_t)i;
}

void re15_menu_toggle(void)
{
    s_open = !s_open;
    if (s_open) {
        rebuild_display();
        /* snap the cursor to the currently-equipped weapon if it is displayed */
        int eq = re15_player_equipped_weapon();
        s_cursor = 0;
        for (int i = 0; i < s_count; i++)
            if (g_inv.slots[s_disp_slot[i]].id == eq) { s_cursor = i; break; }
    }
}

void re15_menu_tick(uint16_t pad_pressed)
{
    if (!s_open || s_count <= 0) { if (s_open && s_count <= 0 && (pad_pressed & (RE15_PAD_BIT_START|RE15_PAD_BIT_CROSS))) s_open = 0; return; }

    /* byte-true column-major 2-wide grid nav (FUN_800487b0), no wrap. even = left col, odd = right col. */
    if ((pad_pressed & RE15_PAD_BIT_RIGHT) && (s_cursor & 1) == 0 && s_cursor < s_count - 1) s_cursor++;
    if ((pad_pressed & RE15_PAD_BIT_LEFT)  && (s_cursor & 1) != 0 && s_cursor > 0)           s_cursor--;
    if ((pad_pressed & RE15_PAD_BIT_DOWN)  && s_cursor + 2 <  s_count)                       s_cursor += 2;
    if ((pad_pressed & RE15_PAD_BIT_UP)    && s_cursor >= 2)                                  s_cursor -= 2;

    if (pad_pressed & RE15_PAD_BIT_SQUARE) {
        uint8_t id = g_inv.slots[s_disp_slot[s_cursor]].id;
        if (re15_item_is_weapon(id)) {                 /* only weapons are equippable (id < 0x15) */
            re15_player_set_equipped_weapon(id);       /* DAT_800aca5d = id (@0x80046688)          */
            re15_audio_prime_weapon(id);               /* re-load its ARMS SE bank (@0x800466c4)   */
            s_open = 0;                                /* equip closes the menu                    */
        }
        /* SQUARE on a non-weapon (ammo/key) is a no-op here (USE/COMBINE = deferred sub-actions) */
    } else if (pad_pressed & RE15_PAD_BIT_CROSS) {
        s_open = 0;                                    /* cancel (START handled by re15_menu_toggle) */
    }
}
