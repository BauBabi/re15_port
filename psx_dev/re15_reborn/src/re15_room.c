/*
 * RE1.5 Rebuilt — per-room RDT loader, PSX (CD-stream) HALF.
 * See re15_room.h. RE2 mirror: the room RDT is CdRead from disc per room.
 * The cross-room STATE + transition LOGIC (g_room_rdt / g_room_change /
 * re15_room_request_change / re15_room_apply_pending) now live in the SHARED
 * room_common.c so both ports use them; this file is the PSX ARCH loader only
 * (re15_room_load via CD), registered as the PSX ctx->load_rdt callback.
 */
#include <stdint.h>
#include <stdio.h>          /* sprintf */
#include "re15_room.h"
#include "re15_rdt.h"
#include "re15_cdfs.h"

/* extern: defined in the shared room_common.c */
/* g_room_rdt / g_room_rdt_ok / g_current_room_id are declared in re15_room.h */

/* Resident buffer for the CURRENT room's RDT. Reused per room (room N overwrites
 * room N-1). Sized for the largest RDT we stream: ROOM1170 = ~296KB; 320KB gives
 * headroom for the bigger rooms (the 46-cut ROOM5011 is comparable, its extra
 * cuts are tiny camera/zone records vs the model/sound bulk). */
static unsigned char s_room_rdt[0x50000];   /* 320 KB */

int re15_room_load(unsigned room_id)
{
    char name[32];
    /* RE2-faithful disc layout: RDTs live in a \RDT\ subdirectory (RE2: \PL0\RDT\)
     * so the CD ROOT directory stays tiny — keeping the boot-critical files
     * (SYSTEM.CNF/EXE/first TIMs) in root directory sector 1. See
     * bugfix_iso_root_dir_sector2_bootfreeze. */
    sprintf(name, "\\RDT\\ROOM%04X.RDT;1", room_id);   /* 0x1170 -> "\RDT\ROOM1170.RDT;1" */

    int n = re15_cd_load_file(name, s_room_rdt, (int)sizeof s_room_rdt);
    if (n <= 0) { g_room_rdt_ok = 0; return -1; }
    if (re15_rdt_parse(s_room_rdt, (size_t)n, &g_room_rdt) != 0) {
        g_room_rdt_ok = 0;
        return -1;
    }
    g_current_room_id = room_id;
    g_room_rdt_ok = 1;
    return 0;
}
