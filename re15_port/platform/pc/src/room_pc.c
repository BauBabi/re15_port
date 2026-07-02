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
    /* Asset-Pfad-Konsolidierung (2026-07-02): der Raum-RDT kommt aus der EINEN Asset-
     * Wurzel shared_assets/PSX, CD-Layout STAGE{N}/ROOM%04X.RDT. Die erste Hex-Ziffer der
     * Room-ID = Stage (verifiziert: 80/32/32/32/48/16 = 240 RDTs unter STAGE1..6). Die alten
     * assets_shared/RDT-Flach- + re15_reborn-Ketten sind entfernt — es gibt nur noch diese
     * eine Wurzel (env RE15_ASSET_ROOT / RE15_CD_ROOT übersteuern; sonst der Compile-Default). */
    char path[300];
    uint8_t *buf = NULL;
    int size = 0;
    unsigned stage = (room_id >> 12) & 0xF;

    const char *roots[3]; int nroots = 0;
    const char *er = getenv("RE15_ASSET_ROOT"); if (er && er[0]) roots[nroots++] = er;
    const char *cr = getenv("RE15_CD_ROOT");    if (cr && cr[0]) roots[nroots++] = cr;
#ifdef RE15_ASSET_ROOT_DEFAULT
    roots[nroots++] = RE15_ASSET_ROOT_DEFAULT;
#endif
    for (int i = 0; i < nroots && !buf && stage >= 1 && stage <= 6; i++) {
        snprintf(path, sizeof path, "%s/STAGE%u/ROOM%04X.RDT", roots[i], stage, room_id);
        buf = re15_asset_read_file(path, &size);
    }
    /* cwd-relative Fallbacks (CTest/headless aus unterschiedlichen Arbeitsverzeichnissen). */
    if (!buf && stage >= 1 && stage <= 6) {
        static const char *rel[] = { "shared_assets/PSX/", "../shared_assets/PSX/",
                                     "../../shared_assets/PSX/", "../../../shared_assets/PSX/", NULL };
        for (int i = 0; rel[i] && !buf; i++) {
            snprintf(path, sizeof path, "%sSTAGE%u/ROOM%04X.RDT", rel[i], stage, room_id);
            buf = re15_asset_read_file(path, &size);
        }
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
