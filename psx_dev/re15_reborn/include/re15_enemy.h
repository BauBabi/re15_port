/*
 * RE1.5 Rebuilt — generic enemy-model registry (globalization 2026-06-13).
 *
 * Generalizes the em21 special-case: any SCD Sce_em_set(type) can load its
 * EM<type:02X>.EMD (mesh + skeleton + animation + texture) into a bank here,
 * and the shared anim selector (anim_select_common.c) routes an actor of that
 * type to its bank — no per-enemy code. The actual LOAD is per-port:
 *   PSX: CD-stream \PLD\EM%02X.EMD into a resident buffer + VRAM-alloc its TIM.
 *   PC : file-read EMD/EM%02X.EMD + upload its TIM to a render slot.
 * Both fill a bank via re15_enemy_alloc() then set ok=1.
 *
 * em21 (0x21) + Elliot (0x47) keep their existing special paths for now; the
 * generic registry handles every OTHER type. Reset on room change.
 */
#ifndef RE15_ENEMY_H
#define RE15_ENEMY_H

#include <stdint.h>
#include "re15_md1.h"
#include "re15_emd.h"

/* Resident enemy banks. PC can afford many (malloc); PSX (2 MB) only a few big
 * EMDs (~260 KB each) — the PSX loader caps itself, this is just the table size.
 * Each bank now carries a frames[1664] EDD (enemy MAIN anim banks are large), so the
 * table size directly costs ~bank_size each in BSS — kept to the most any one room needs
 * (ROOM1140 = 3 zombie types) plus headroom, to keep the PSX _end < 0x80200000. */
/* 4 banks. Was 5; reduced to 4 (2026-06-17) to reclaim ~9 KB BSS after the re-examine
 * (issue-2) work pushed _end (0x801f6cd4) past the PSX boot RAM ceiling — the boot-time
 * /RDT/ directory read (a 16384-byte transient alloc) + stack peak then collided with BSS
 * → `psxspu timeout 0x0805` → PC=0 crash (DuckStation-bisected: MAX=5 crashes, MAX=4 boots
 * with _end=0x801f4880, ~9 KB below the 6d-good 0x801f6ae4). No room needs >3 enemy types
 * (ROOM1140 = 3 zombies), so 4 keeps one spare. If a future room needs 5, free BSS elsewhere
 * rather than bumping this back — the PSX boot is RAM-critical. */
#define RE15_ENEMY_MAX 4

typedef struct {
    uint8_t type;                 /* Sce_em_set type byte; 0 = free slot */
    uint8_t ok;                   /* 1 = loaded + parsed, ready to render */
    re15_md1_t           md1;
    re15_emd_skeleton_t  skel;
    re15_emd_animation_t anim;
    int tpage, clut;              /* PSX VRAM handles (per-tri path: both -1) */
    int xshift, yshift;           /* PSX VRAM tpage-X / CLUT-row relocation shifts */
    int pc_tex_slot;              /* PC render texture slot (-1 if unused) */
    void *buf;                    /* resident EMD buffer; PC = malloc'd (freed on reset),
                                   * PSX = points at a static slot buffer (NULL = don't free) */
} re15_enemy_bank_t;

extern re15_enemy_bank_t g_enemy[RE15_ENEMY_MAX];

/* Loaded bank for `type`, or NULL. (type 0 = none.) */
re15_enemy_bank_t *re15_enemy_find(uint8_t type);

/* Claim a free bank for `type` (sets type, clears ok). Returns NULL if the table
 * is full. The port loader fills md1/skel/anim/texture then sets ok=1. */
re15_enemy_bank_t *re15_enemy_alloc(uint8_t type);

/* Drop all banks (call on room change). Frees any malloc'd buf (PC). */
void re15_enemy_reset(void);

/* Room-cinematic overlay. Given a room's RBJ buffer (loaded per-port) + the PRE-OVERLAY
 * base skeletons, re-overlays Leon (+ optionally Elliot) from their bases with the RBJ
 * keyframes, and rebinds the per-room cutscene enemy's animation from an RBJ record
 * (table-driven, e.g. ROOM1150 RECORD 1 = prone Irons onto em45). `ensure_enemy` = the
 * port's enemy loader; pass NULL to only rebind an already-loaded enemy. Any out-pointer
 * may be NULL to skip that overlay; base==out aliasing is safe. The CALLER supplies the
 * ~7 KB parse scratch (re15_emd_animation_t is huge — never a stack local / static here).
 *
 * ⚠️ PARITY NOTE: only the PC port CALLS this (its boot + cross-room reload). The PSX port
 * keeps a hand-tuned INLINE copy in asset_psx.c re15_load_room_cinematic — NOT a drift
 * oversight: the RAM-tight PSX boot stack/heap could not host this fn's call frame + scratch
 * (it froze the boot; see the memory). The two are a JUSTIFIED arch difference; keep the
 * overlay LOGIC (Leon/Elliot overlay + the ROOM→enemy rebind table) identical in both by
 * hand if you change one. */
void re15_apply_room_cinematic(const uint8_t *rbj, size_t rbj_size, unsigned room_id,
        const re15_emd_skeleton_t *pl00_base,
        re15_emd_skeleton_t *leon_skel, re15_emd_animation_t *leon_anim,
        const re15_emd_skeleton_t *elliot_base, int elliot_ok,
        re15_emd_skeleton_t *elliot_skel, re15_emd_animation_t *elliot_anim,
        re15_emd_skeleton_t *scratch_skel, re15_emd_animation_t *scratch_anim,
        void (*ensure_enemy)(uint8_t type));

#endif /* RE15_ENEMY_H */
