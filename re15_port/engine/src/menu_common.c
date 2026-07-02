/*
 * menu_common.c — Inventory / weapon-select menu LOGIC (Phase 8.20, 2026-07-02).
 *
 * The engine-side, headless-testable half of the weapon-select menu (the render half is
 * platform-side: re15_menu_render in the PC main loop). See re15_menu.h + RE15_INVENTORY_SUBSYSTEM.md.
 *
 * Byte-true: the open trigger (START edge), and the EQUIP commit — set DAT_800aca5d then re-prime the
 * weapon's ARMS SE bank (FUN_80043d8c), the exact order of the game's equip-commit (@0x80046688 +
 * @0x800466c4) and room-init (FUN_800314b0:42->54). FAITHFUL-LINE (flagged): the OWNED-weapon set is a
 * fixed test list — the byte-true inventory item-id -> weapon-id derivation runs through FUN_800c00a8,
 * which lives in the DEBUG.BIN overlay region (0x800C0000, all-?? in the EXE dump) and is not yet RE'd.
 */
#include "re15_menu.h"
#include "re15_player.h"   /* RE15_PAD_BIT_* */
#include "re15_damage.h"   /* re15_player_equipped_weapon / re15_player_set_equipped_weapon */
#include "re15_audio.h"    /* re15_audio_prime_weapon (re-load the ARMS SE bank on equip) */

/* Phase-1 FAITHFUL-LINE owned-weapon list: the weapons with known byte-true ARMS banks + damage/reach
 * rows. Weapon 1 (the handgun, ARMS01) is the byte-true ROOM1140 briefing weapon; 2/3 are test entries
 * (ARMS02/ARMS03, seen equipped in savestates). Phase 2 replaces this by scanning g_inv for weapon ids. */
typedef struct { int weapon; const char *name; } menu_weapon_t;
static const menu_weapon_t s_weapons[] = {
    { 1, "HANDGUN"  },   /* ARMS01 — 6 dmg / reach 1100; the byte-true briefing weapon */
    { 2, "WEAPON 2" },   /* ARMS02 — 24 dmg / reach 1000 */
    { 3, "WEAPON 3" },   /* ARMS03 — 5 dmg / reach 1000 */
};
#define MENU_N ((int)(sizeof s_weapons / sizeof s_weapons[0]))

static int s_open   = 0;
static int s_cursor = 0;

int  re15_menu_is_open(void) { return s_open; }
int  re15_menu_count(void)   { return MENU_N; }
int  re15_menu_cursor(void)  { return s_cursor; }
const char *re15_menu_entry_name(int i)  { return (i >= 0 && i < MENU_N) ? s_weapons[i].name   : ""; }
int         re15_menu_entry_weapon(int i){ return (i >= 0 && i < MENU_N) ? s_weapons[i].weapon : 0;  }

void re15_menu_toggle(void)
{
    s_open = !s_open;
    if (s_open) {
        /* snap the cursor to the currently-equipped weapon if it is in the list */
        int eq = re15_player_equipped_weapon();
        s_cursor = 0;
        for (int i = 0; i < MENU_N; i++)
            if (s_weapons[i].weapon == eq) { s_cursor = i; break; }
    }
}

void re15_menu_tick(uint16_t pad_pressed)
{
    if (!s_open) return;
    /* cursor nav (clamped, no wrap — Phase 2 gets the byte-true 2-col +-1/+-2 grid nav) */
    if ((pad_pressed & RE15_PAD_BIT_UP)   && s_cursor > 0)          s_cursor--;
    if ((pad_pressed & RE15_PAD_BIT_DOWN) && s_cursor < MENU_N - 1) s_cursor++;

    if (pad_pressed & RE15_PAD_BIT_SQUARE) {
        /* CONFIRM = EQUIP (byte-true: DAT_800aca5d = id @0x80046688, then FUN_80043d8c @0x800466c4) */
        int w = s_weapons[s_cursor].weapon;
        re15_player_set_equipped_weapon(w);
        re15_audio_prime_weapon(w);   /* re-load the equipped weapon's ARMS SE bank */
        s_open = 0;                   /* equip closes the menu */
    } else if (pad_pressed & RE15_PAD_BIT_CROSS) {
        s_open = 0;                   /* cancel (START is handled by re15_menu_toggle, not here) */
    }
}
