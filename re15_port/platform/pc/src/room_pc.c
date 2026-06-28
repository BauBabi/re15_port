/*
 * room_pc.c — PC ARCH half of the per-room RDT loader.
 *
 * Mirror of the PSX re15_room.c (CD loader), but reads the room container from
 * a FILE (assets/room####.rdt) via the PC asset backend instead of CdRead. It
 * fills the SHARED g_room_rdt (room_common.c); the shared transition
 * (re15_room_apply_pending) calls it through the ctx->load_rdt callback.
 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "re15_room.h"
#include "re15_rdt.h"

extern uint8_t *re15_asset_read_file(const char *path, int *out_size);

/* Keeps the resident room's RDT bytes alive (g_room_rdt points into it). */
static unsigned char *s_pc_room_buf = NULL;

int re15_room_load(unsigned room_id)
{
    /* Globalization fix 2026-06-13: cross-room RDTs come from the SHARED tree
     * (assets_shared/RDT/ROOM%04X.RDT). The old re15_reborn/assets/ only held
     * test.rdt + room1140.rdt, so ANY other destination (e.g. door4 -> ROOM1130)
     * was not found and the room change silently aborted. Windows file lookup is
     * case-insensitive so ROOM%04X matches the tree. */
    static const char *prefixes[] = {
#ifdef RE15_ASSET_ROOT_DEFAULT
        /* Verschiebung nach re15_port: absolute Default-Wurzel zuerst (cwd-unabhängig,
         * enthält alle 240 RDTs). Gleicher Mechanismus wie pc_read_shared / bg_pc. */
        RE15_ASSET_ROOT_DEFAULT "/",
#endif
        "../../assets_shared/", "../assets_shared/", "../../../assets_shared/",
        "assets_shared/", "psx_dev/assets_shared/",
        "../re15_reborn/", "../../re15_reborn/", "../../../re15_reborn/",
        "../re15_reborn/assets/", "../../re15_reborn/assets/", "../../../re15_reborn/assets/",
        "psx_dev/re15_reborn/", "psx_dev/re15_reborn/assets/",
        "re15_reborn/", "re15_reborn/assets/"
    };
    char path[256];
    uint8_t *buf = NULL;
    int size = 0;
    for (int i = 0; i < (int)(sizeof prefixes / sizeof prefixes[0]); i++) {
        snprintf(path, sizeof path, "%sRDT/ROOM%04X.RDT", prefixes[i], room_id);
        buf = re15_asset_read_file(path, &size);
        if (buf) break;
    }
    for (int i = 0; !buf && i < (int)(sizeof prefixes / sizeof prefixes[0]); i++) {
        snprintf(path, sizeof path, "%sROOM%04X.RDT", prefixes[i], room_id);
        buf = re15_asset_read_file(path, &size);
    }
    for (int i = 0; !buf && i < (int)(sizeof prefixes / sizeof prefixes[0]); i++) {
        snprintf(path, sizeof path, "%sroom%04x.rdt", prefixes[i], room_id);
        buf = re15_asset_read_file(path, &size);
    }
    if (!buf) {
        fprintf(stderr, "[room] PC load FAILED: room%04x.rdt not found\n", room_id);
        g_room_rdt_ok = 0;
        return -1;
    }
    if (re15_rdt_parse(buf, (size_t)size, &g_room_rdt) != 0) {
        free(buf);
        g_room_rdt_ok = 0;
        return -1;
    }
    if (s_pc_room_buf) free(s_pc_room_buf);   /* free the previous room's bytes */
    s_pc_room_buf     = buf;
    g_current_room_id = room_id;
    g_room_rdt_ok     = 1;
    fprintf(stderr, "[room] PC loaded room%04x.rdt (%d bytes)\n", room_id, size);
    return 0;
}

/* PC cross-room render reset (re15_room_apply_pending ctx callback): the PC BG
 * loader re-reads each cut from file and there is no prop-VRAM bump allocator to
 * rewind, so nothing to reset here. (Kept as the per-port arch hook.) */
void re15_room_reset_render_pc(void) { }
