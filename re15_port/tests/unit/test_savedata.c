/* test_savedata.c — FE-4 save/load round-trip + byte-true .mcr format.
 *
 * (1) capture live game-state -> save to a .mcr slot -> clobber state -> load ->
 *     restore -> the state must come back identical.
 * (2) the .mcr must be a byte-true PSX card image: block-0 "MC" header, the used
 *     directory frame (0x51 alloc, filename "BISLPS-0022202", valid XOR checksum),
 *     and the file block's "SC" title frame.
 * (3) empty slots load as -1; list() reports exactly the used slots + titles.
 */
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "re15_savedata.h"
#include "re15_memcard.h"
#include "re15_actor.h"
#include "re15_room.h"
#include "re15_gameflow.h"
#include "re15_collision.h"   /* band re-derive on restore */
#include "re15_damage.h"      /* equipped weapon id round-trip */

int main(void)
{
    int fail = 0;
    const char *mcr = "test_re15_card.mcr";
    remove(mcr);
    printf("=== FE-4 save/load round-trip + .mcr format ===\n");

    /* 1. distinctive live game-state (Y = -3600 -> collision band 2, non-default) */
    memset(&g_actors[0], 0, sizeof g_actors[0]);
    g_actors[0].x = 12345; g_actors[0].y = -3600; g_actors[0].z = 9999;
    g_actors[0].rot_y = 0x0400; g_actors[0].hp = 87; g_actors[0].status_flags = 0x2;
    g_current_room_id = 0x1150;
    g_gameflow.character = 1;
    memset(&g_inv, 0, sizeof g_inv);
    g_inv.slots[0].id = 0x03; g_inv.slots[0].qty = 15;   /* BROWNING HP */
    g_inv.slots[1].id = 0x21; g_inv.slots[1].qty = 2;    /* MEMORY CARD */
    re15_player_set_equipped_weapon(0x03);               /* weapon id 3 + slot 0 */
    memset(&g_game, 0, sizeof g_game);
    re15_game_flag_set(3, 17, 1);
    re15_game_flag_set(5, 200, 1);
    int want_band = re15_collision_band_from_y(-3600);   /* = 2 */

    /* 2. capture + save to slot 2 */
    re15_savedata_t sd;
    re15_savedata_capture(&sd, 3600, 1);
    if (re15_memcard_save(mcr, 2, &sd, "BIOHAZARD 1.5") != 0) {
        fprintf(stderr, "FAIL(save): write failed\n"); fail = 1;
    }

    /* 3. clobber the live state (incl. weapon id + collision band) */
    memset(&g_actors[0], 0, sizeof g_actors[0]);
    g_current_room_id = 0; g_gameflow.character = 0;
    memset(&g_inv, 0, sizeof g_inv);
    memset(&g_game, 0, sizeof g_game);
    re15_player_set_equipped_weapon(0x01);   /* knife (the stale default a bad restore leaves) */
    re15_collision_set_band(7);              /* wrong band */

    /* 4. load + restore */
    re15_savedata_t ld;
    uint16_t room = 0;
    if (re15_memcard_load(mcr, 2, &ld) != 0) { fprintf(stderr, "FAIL(load)\n"); fail = 1; }
    else if (re15_savedata_restore(&ld, &room) != 0) { fprintf(stderr, "FAIL(restore)\n"); fail = 1; }

    /* 5. verify the restored state */
    if (g_actors[0].x != 12345 || g_actors[0].y != -3600 || g_actors[0].z != 9999 ||
        g_actors[0].rot_y != 0x0400 || g_actors[0].hp != 87 || g_actors[0].status_flags != 2) {
        fprintf(stderr, "FAIL(player): got x=%d hp=%d rot=%d\n",
                g_actors[0].x, g_actors[0].hp, g_actors[0].rot_y); fail = 1;
    }
    if (room != 0x1150) { fprintf(stderr, "FAIL(room): %04x\n", room); fail = 1; }
    if (g_gameflow.character != 1) { fprintf(stderr, "FAIL(char)\n"); fail = 1; }
    if (g_inv.slots[0].id != 0x03 || g_inv.slots[0].qty != 15 ||
        g_inv.slots[1].id != 0x21 || g_inv.slots[1].qty != 2) { fprintf(stderr, "FAIL(inv)\n"); fail = 1; }
    if (!re15_game_flag_get(3, 17) || !re15_game_flag_get(5, 200)) { fprintf(stderr, "FAIL(flags)\n"); fail = 1; }
    if (re15_player_equipped_weapon() != 0x03) {
        fprintf(stderr, "FAIL(weapon): got %d, want 3\n", re15_player_equipped_weapon()); fail = 1; }
    if (re15_inv_equipped_slot() != 0) {
        fprintf(stderr, "FAIL(equip-slot): got %d\n", re15_inv_equipped_slot()); fail = 1; }
    if (re15_collision_debug_band() != want_band) {
        fprintf(stderr, "FAIL(band): got %d, want %d\n", re15_collision_debug_band(), want_band); fail = 1; }
    if (!fail) printf("  round-trip: player/room/char/inv/flags/weapon/band all restored\n");

    /* 6. empty slot -> -1 */
    if (re15_memcard_load(mcr, 0, &ld) != -1) { fprintf(stderr, "FAIL(empty): slot 0 should be empty\n"); fail = 1; }

    /* 7. list */
    int used[RE15_SAVE_SLOTS]; char titles[RE15_SAVE_SLOTS][RE15_MC_TITLE_LEN];
    int nu = re15_memcard_list(mcr, used, titles);
    if (nu != 1 || !used[2] || strncmp(titles[2], "BIOHAZARD 1.5", 13) != 0) {
        fprintf(stderr, "FAIL(list): nu=%d used2=%d title='%s'\n", nu, used[2], titles[2]); fail = 1;
    } else printf("  list: 1 used slot, title '%s'\n", titles[2]);

    /* 8. byte-true PSX card image checks */
    {
        static uint8_t img[128 * 1024];
        FILE *f = fopen(mcr, "rb");
        size_t n = f ? fread(img, 1, sizeof img, f) : 0;
        if (f) fclose(f);
        if (n != sizeof img) { fprintf(stderr, "FAIL(mcr): size %zu\n", n); fail = 1; }
        else {
            if (img[0] != 'M' || img[1] != 'C') { fprintf(stderr, "FAIL(mcr): block0 magic\n"); fail = 1; }
            const uint8_t *dir = img + 3 * 0x80;             /* slot 2 -> block 3 -> dir frame 3 */
            if (dir[0] != 0x51) { fprintf(stderr, "FAIL(mcr): dir alloc %02x\n", dir[0]); fail = 1; }
            if (strcmp((const char *)dir + 0x0A, "BISLPS-0022202") != 0) {
                fprintf(stderr, "FAIL(mcr): filename '%s'\n", dir + 0x0A); fail = 1; }
            uint8_t x = 0; for (int i = 0; i < 0x7F; i++) x ^= dir[i];
            if (x != dir[0x7F]) { fprintf(stderr, "FAIL(mcr): dir XOR checksum\n"); fail = 1; }
            const uint8_t *b = img + 3 * 0x2000;             /* the file block */
            if (b[0] != 'S' || b[1] != 'C') { fprintf(stderr, "FAIL(mcr): SC title magic\n"); fail = 1; }
            if (!fail) printf("  .mcr: byte-true PSX card (MC hdr, 0x51 dir, BISLPS-0022202, XOR ok, SC block)\n");
        }
    }

    remove(mcr);
    if (fail) { printf("SAVEDATA: FAIL\n"); return 1; }
    printf("SAVEDATA: all checks passed\n");
    return 0;
}
