/*
 * RE1.5 Rebuilt — PSX asset / VRAM upload (Phase 4.3, 2026-05-18).
 *
 * Parse + upload TIM (CLUT + pixel data) to VRAM.
 * Exposes tpage/clut handles to render.c for sprite rendering.
 */
#include <stdint.h>
#include <psxgpu.h>
#include "re15_engine.h"
#include "re15_tim.h"
#include "re15_vram.h"   /* structured VRAM allocator for dynamic textures */
#include "re15_md1.h"
#include "re15_emd.h"
#include "re15_rdt.h"
#include "re15_light.h"   /* #1c: per-cut NCCT lighting */
#include "re15_msg.h"     /* text/subtitles: decode + duration provider */
#include "re15_cdfs.h"    /* Phase 4.7: CD-stream prop TIMs into shared staging */
#include "re15_room.h"    /* Phase 4.7: per-room RDT CD-load (multi-room foundation) */
#include "shadow_blob_psx.h"   /* RE1.5 char shadow blob as a 16bpp+STP texture */
#include "font_atlas_psx.h"    /* RE1.5 subtitle glyph font (TEX.TIM region) + CLUT */
#include "re15_enemy.h"        /* generic enemy-model registry (Step B PSX loader) */
#include <stdio.h>             /* sprintf — CD path building (codebase idiom) */
#include <string.h>            /* memcpy — copy enemy model out of shared staging */

extern const uint8_t test_tim[];
extern const uint32_t test_tim_size;
extern const uint8_t test_md1[];
extern const uint32_t test_md1_size;
extern const uint8_t test_edd[];
extern const uint32_t test_edd_size;
extern const uint8_t test_emr[];
extern const uint32_t test_emr_size;
/* (test_rdt incbin extern removed 2026-06-13: assets/test.rdt is gone — the room RDT
 * is CD-streamed as ROOM%04X.RDT; this extern was dead, never incbin'd nor used.) */
/* (room1170_rbj incbin removed 2026-06-14g — the cinematic rbj overlay is CD-streamed
 * per-room from \RBJ\ROOM1170.RBJ into the per-room arena; see re15_assets_init.) */
/* (room1170_light + room1170_msgs incbin removed 2026-06-14i — both are RDT-sliced per
 * room now: g_room_rdt.lights @ RDT+0x2c, g_room_rdt.messages @ RDT+0x3c. The incbin were
 * dead (light a never-hit fallback, msgs unreferenced).) */
/* (Phase 2, 2026-06-13: ALL room props — obj00..05 (incl. obj02 the helicopter) +
 * room1140 obj00 — are now sliced from the parsed RDT (g_room_rdt.prop_md1/tim[]),
 * not incbin'd/CD-streamed. The heli (prop index 2) loads from prop[2] in
 * re15_assets_init; the prop loop draws indices != 2. No more room1170_obj* /
 * room1140_obj* / OBJ0N.TIM assets in the EXE or on the disc.) */
/* (Elliot's MD1/EDD/EMR incbin removed 2026-06-14g — his mesh+skeleton are CD-streamed
 * per-room from \PLD\ELLIOT.MD1/EMR into the per-room arena; ELLIOT.EDD is not loaded
 * (the rbj overlay replaces his animation). His TIM is CD-streamed in re15_assets_init.) */
/* PL00W01 weapon-track: real walk/run cycle used by any actor in walk/run
 * motion (sentinels 105/100). */
extern const uint8_t pl00w01_edd[]; extern const uint32_t pl00w01_edd_size;
extern const uint8_t pl00w01_emr[]; extern const uint32_t pl00w01_emr_size;

/* Phase 4.5: parsed MD1, exposed for main.c diagnostics */
re15_md1_t re15_test_md1;
int        re15_test_md1_ok = 0;

/* Phase 4.5.7: parsed EDD (animation table) + EMR (skeleton + keyframes).
 * The asset file globals back the PL00 (Leon) player model. */
re15_emd_animation_t re15_test_anim;
re15_emd_skeleton_t  re15_test_skel;
int                  re15_test_skel_ok = 0;

/* PL00W01 walk-source (AD-round port from PC). Composite: PL00 bind pose +
 * W01 keyframes. Selected whenever an actor's motion is the walk/run sentinel
 * (105 = W01 clip 5 Walk_Forward, 100 = W01 clip 0 Run). The rbj (test) and
 * PL05 (elliot) anim sources have no real walk cycle, so both Leon and Elliot
 * source walking from here; the bone hierarchy is identical (15 bones), so the
 * pose-source swap is safe. */
re15_emd_animation_t re15_w01_anim;
re15_emd_skeleton_t  re15_w01_skel;
int                  re15_w01_ok = 0;

/* PL00 BASE animation (PL00.edd + PL00 bind) — the player's non-weapon track.
 * Holds IDLE (clip 0, standing loop) and BACK-WALK (clip 1, baked reverse gait),
 * which the W01 weapon set does NOT have. Saved before the rbj overlay overwrites
 * re15_test_anim/skel. (RE'd 2026-06-05: FSM BACK=bank0x16c clip1, IDLE=clip0.) */
re15_emd_animation_t re15_pl00_anim;
re15_emd_skeleton_t  re15_pl00_skel;
int                  re15_pl00_ok = 0;

/* Phase 4.5.9: parsed RDT (room container). */
re15_rdt_t re15_test_rdt;
int        re15_test_rdt_ok = 0;

/* #2: helicopter body MD1 (non-skeletal prop, actor type 0x47). */
re15_md1_t re15_heli_md1;
int        re15_heli_md1_ok = 0;
/* #2: heli TIM uploaded to a relocated VRAM slot; tpage/clut handles for the
 * prop renderer (the heli MD1's baked coords collide with FB + debug font). */
int        re15_heli_tim_tpage = 0;
int        re15_heli_tim_clut  = 0;
int        re15_heli_tim_ok    = 0;
/* Elliot (NPC type 0x47): own skeletal model (PL05). The skeletal renderer
 * takes per-actor md1/skel + a tpage/clut override (Elliot's relocated TIM). */
re15_md1_t           re15_elliot_md1;
re15_emd_skeleton_t  re15_elliot_skel;
re15_emd_animation_t re15_elliot_anim;
int                  re15_elliot_tpage = 0;
int                  re15_elliot_clut  = 0;
int                  re15_elliot_clut_yshift = 0;   /* #7: relocated_y - baked_y, for per-tri CLUT remap */
int                  re15_elliot_tpage_xshift = 0;  /* #7: relocated_x/64; his texture spans 2 tpages (body X0 / face X1) */
int                  re15_elliot_ok    = 0;
#define RE15_ELLIOT_VRAM_X  832   /* between Leon's TIM (ends x832) + font (960) */
#define RE15_ELLIOT_VRAM_Y  0
#define RE15_ELLIOT_CLUT_X  0     /* below Leon's CLUT (rows 480-482) */
#define RE15_ELLIOT_CLUT_Y  483

/* (em21 / type 0x21 = the 7 helipad crows) is NO LONGER a special case here — it folds
 * into the GENERIC per-room enemy arena (re15_enemy_load → \EMD\EM21.EMD), lazy-loaded
 * when a crow first spawns and freed on room change like every other enemy. The old
 * dedicated 73 KB s_em21_emd buffer + re15_em21_* globals + boot load were room1170-only
 * ballast and have been removed (2026-06-14f). */

/* #2 prop loop: per-obj_id model + texture table (obj_id 0..5; index 2 = heli,
 * left empty since the heli renders via the actor 0x47 path). Populated from
 * the bundled obj MD1/TIM in re15_assets_init via a small VRAM bump allocator. */
re15_md1_t re15_obj_md1[6];
int        re15_obj_tpage[6];
int        re15_obj_clut[6];
int        re15_obj_ok[6] = {0,0,0,0,0,0};

/* Multi-room: reclaim ALL per-room dynamic VRAM (prop/NPC/enemy texture slots +
 * CLUT rows) so the next room's set re-packs collision-free. Now just delegates to
 * the structured re15_vram allocator (which replaced the old per-texture bump
 * bookkeeping). Call at boot + on every room-enter BEFORE loading that room's
 * props/NPCs/enemies. */
/* RE2-style enemy MODEL-slot bank (Step B, 2026-06-14b).
 *
 * The released RE2 engine keeps a small FIXED bank of enemy MODEL slots at
 * 0x80100000, uploads each enemy's TIM to VRAM (the RAM TIM copy is transient), and
 * funnels every CD load through ONE shared staging buffer — it does NOT keep whole
 * EMD containers resident, and the slots are NOT shared with the room/RDT heap. We
 * mirror that exactly:
 *   1. Stage the full EM<NN>.EMD into the SHARED staging buffer (re15_psx_staging_buf,
 *      the same buffer the MDEC background decode uses — they never run at the same
 *      instant), so no second permanent load buffer is reserved.
 *   2. Upload its 8bpp TIM to VRAM (per-tri tpage+CLUT → xshift/yshift, like em21),
 *      then DISCARD the TIM bytes — only the model is kept.
 *   3. memcpy the MODEL section [0, off_tim) into a fixed slot and RE-PARSE from the
 *      slot, so the resident pointers (md1/skel/anim) alias the slot, not the staging.
 * Per the EMD measurements, the model is <=196 KB (EM2B) vs the 262 KB full container,
 * so each slot is SLOT_SIZE; the TIM (33-100 KB) lives only in VRAM.
 *
 * SLOT COUNT: RE2 ran 2 slots because its RETAIL enemies are ~53 KB. RE1.5's models
 * are 150-196 KB, and our baseline still incbin's the player/Elliot/BGM globals
 * (~450 KB the real engine would stream), so within the same 2 MB only ONE model slot
 * fits today (1 slot → ~148 KB stack headroom; a 2nd 196 KB slot would overflow). To
 * raise RE15_ENEMY_SLOTS, first reclaim baseline BSS the RE2 way: consolidate
 * re15_cd_staging into this shared buffer (-100 KB) and/or stream the incbin'd globals.
 * em21 (0x21, the crows) keeps its own resident buffer + special path for now; folding
 * it into this bank would free 72 KB and yield a true 2-type bank — a follow-up. */
/* The enemy models live in ONE bump ARENA (RE1.5-style, not RE2's fixed equal slots):
 * each enemy's model (TIM stripped, so off_tim + the 36-byte directory) is bump-allocated
 * by its ACTUAL size, so several SMALL enemies pack where one fixed 196 KB slot would sit.
 * This fits MULTIPLE enemy types/room (the common case is two ~150 KB models) at far less
 * cost than N×max-size slots. Reset per room. Sized to fit two ~148 KB models (e.g.
 * EM10+EM11); a single enemy up to EM2B (196 KB) also fits. Bounded also by the 8-bank
 * registry. Grow only with `_end` headroom to spare (no PSX heap — headroom is the stack). */
/* The per-room arena holds the CURRENT room's model data: enemy models (any room) AND
 * room1170's intro-cinematic assets — the rbj animation overlay + Elliot's mesh/skeleton
 * (2026-06-14g). They never coexist with a heavy enemy set (room1170's only enemies are the
 * crows, ~36 KB), so sharing one region is collision-free. Sized for the worst case (a room
 * with several big enemies). */
#define RE15_ENEMY_ARENA_SIZE  0x76000   /* 483328 B — grew after em21 + Elliot + rbj folded in */
static unsigned char s_enemy_arena[RE15_ENEMY_ARENA_SIZE] __attribute__((aligned(4)));
static int           s_enemy_arena_used = 0;

/* Copy `n` bytes into the arena (4-aligned bump); returns the resident pointer or NULL if
 * full. Used by the enemy loader AND the boot room1170 cinematic load (rbj + Elliot). */
static unsigned char *re15_arena_put(const void *src, size_t n)
{
    size_t a = (n + 3u) & ~(size_t)3u;
    if ((size_t)s_enemy_arena_used + a > RE15_ENEMY_ARENA_SIZE) return NULL;
    unsigned char *p = s_enemy_arena + (size_t)s_enemy_arena_used;
    memcpy(p, src, n);
    s_enemy_arena_used += (int)a;
    return p;
}

void re15_assets_reset_vram_allocators(void)
{
    re15_vram_reset_room();
    /* Free the per-room arena: the shared re15_enemy_reset() (called right after this on a
     * room change) clears the enemy banks, so the next room's models re-pack from 0. */
    s_enemy_arena_used = 0;
    /* CRITICAL lifetime fix (room1170 cinematic now lives in the arena): re15_test_skel/anim
     * (Leon, after the rbj overlay) and re15_elliot_* point INTO the arena we just freed.
     * Re-point Leon's skeleton back to the PERMANENT PL00 base (re15_pl00_*, which aliases the
     * always-resident incbin'd Leon model) and mark Elliot unloaded, so nothing dereferences
     * a dangling arena pointer in the next room. Guarded on pl00_ok so the boot path (before
     * the player is parsed) is a no-op. Leon's cinematic overlay is only needed in room1170
     * (re-loaded at boot); gameplay uses the W01/PL00 banks, so dropping it here is safe. */
    if (re15_pl00_ok) {
        re15_test_skel = re15_pl00_skel;
        re15_test_anim = re15_pl00_anim;
    }
    re15_elliot_ok = 0;
}

static uint32_t re15_u32le(const unsigned char *p)
{
    return (uint32_t)p[0] | ((uint32_t)p[1] << 8)
         | ((uint32_t)p[2] << 16) | ((uint32_t)p[3] << 24);
}

/* PSX generic enemy loader (Step B): RE2-faithful — stage \EMD\EM<NN>.EMD in the
 * shared buffer, TIM → VRAM, MODEL → fixed slot (TIM stripped). Triggered once when an
 * actor of `type` first appears. em21 (0x21) + Elliot (0x47) keep their special paths
 * and are NOT handled here. Returns 1 on success.
 *
 * NEGATIVE CACHE (2026-06-14c, fixes the framerate collapse): a bank is allocated up
 * front as a FAILED MARKER (type set, ok=0); ANY failure path returns WITHOUT clearing
 * it, so the same type is never re-attempted this room. Without this, a failing load
 * (e.g. VRAM full, missing file) re-issued a blocking ~262 KB CD read on EVERY frame
 * the actor was alive → framerate in the basement + the model stayed Leon. Markers
 * (and the slots) clear on room change via re15_enemy_reset / reset_vram_allocators.
 * Failure reasons are printed to TTY (visible in DuckStation's console log) so we can
 * see WHY a model didn't load even though the build can't be run here. */
int re15_enemy_load(uint8_t type)
{
    if (type == 0 || type == 0x47) return 0;   /* 0x47 = Elliot (own PL05 path, not yet folded) */

    /* Already loaded (ok=1) OR already failed this room (ok=0 marker) → do not retry. */
    for (int i = 0; i < RE15_ENEMY_MAX; i++)
        if (g_enemy[i].type == type) return g_enemy[i].ok ? 1 : 0;

    re15_enemy_bank_t *eb = re15_enemy_alloc(type);      /* FAILED MARKER until ok=1 below */
    if (!eb) return 0;                                   /* registry table full (8) */

    /* (1) Stage the full EMD container in the SHARED staging buffer. */
    unsigned char *stg = (unsigned char *) re15_psx_staging_buf;
    char path[24];
    sprintf(path, "\\EMD\\EM%02X.EMD;1", type);
    int n = re15_cd_load_file(path, stg, RE15_PSX_STAGING_SIZE);
    if (n <= 0) { printf("[enemy] EM%02X.EMD CD load FAILED (n=%d)\n", (unsigned)type, n); return 0; }

    /* Section directory: em<NN> container — index 8 = TIM (the tail); model = before it. */
    if ((size_t)n < 12) { printf("[enemy] EM%02X too small (%d)\n", (unsigned)type, n); return 0; }
    uint32_t begin = re15_u32le(stg + 0);
    uint32_t count = re15_u32le(stg + 4);
    if (count < 9 || (size_t)begin + count * 4 > (size_t)n) {
        printf("[enemy] EM%02X bad dir (begin=%u count=%u n=%d)\n", (unsigned)type, begin, count, n);
        return 0;
    }
    uint32_t off_tim   = re15_u32le(stg + begin + 8 * 4);   /* idx 8 = TIM = model end */
    uint32_t dir_bytes = count * 4u;                         /* the 9-entry section directory */
    /* EMD file layout is [model][TIM][directory] — the directory sits at the TAIL
     * (begin = filesize - dir_bytes), AFTER the TIM. We keep the model + the directory
     * resident and drop only the TIM (it's in VRAM), so the resident size is
     * off_tim + dir_bytes. */
    if (off_tim == 0 || off_tim > (uint32_t)n) {
        printf("[enemy] EM%02X bad off_tim=%u (n=%d)\n", (unsigned)type, off_tim, n);
        return 0;
    }
    /* Bump-allocate the resident model (model + appended directory, 4-aligned) from the
     * shared enemy arena. If it won't fit (the room's earlier enemies filled it), leave
     * the failed marker (ok=0) so this type isn't re-attempted every frame → Leon. */
    uint32_t resident = ((off_tim + dir_bytes) + 3u) & ~3u;
    if (resident > RE15_ENEMY_ARENA_SIZE) {
        printf("[enemy] EM%02X model too big (%u > arena %u)\n",
                (unsigned)type, resident, (unsigned)RE15_ENEMY_ARENA_SIZE);
        return 0;
    }
    if ((uint32_t)s_enemy_arena_used + resident > RE15_ENEMY_ARENA_SIZE) {
        printf("[enemy] EM%02X arena full (%d used + %u > %u) -> Leon\n",
                (unsigned)type, s_enemy_arena_used, resident, (unsigned)RE15_ENEMY_ARENA_SIZE);
        return 0;
    }

    /* (2) Parse the TIM from staging and upload it to VRAM; keep only VRAM handles. */
    re15_tim_t t;
    {
        re15_md1_t           tmd1;   re15_emd_skeleton_t tskel;   re15_emd_animation_t tanim;
        if (re15_emd_parse_container(stg, (size_t)n, &tmd1, &tskel, &tanim, &t) != 0
            || t.bpp != 8) { printf("[enemy] EM%02X parse/bpp fail (bpp=%d)\n", (unsigned)type, t.bpp); return 0; }
    }
    if (t.has_clut) {
        int rows = (t.clut_entries >= 256) ? (t.clut_entries / 256) : 1;
        if (rows < 1) rows = 1;
        int cy = re15_vram_alloc_clut(rows);             /* structured CLUT alloc */
        if (cy >= 0) {
            RECT cr = { 0, (short)cy, 256, (short)rows };
            LoadImage(&cr, (uint32_t *) t.clut);
            eb->clut   = getClut(0, cy);
            eb->yshift = cy - (int)t.clut_y;             /* per-tri CLUT-row relocation */
        }
    }
    int mx, my;
    if (!re15_vram_alloc_tex(t.width, /*need_y0*/1, &mx, &my)) {
        printf("[enemy] EM%02X VRAM tex alloc FAILED (w=%d) -> Leon\n", (unsigned)type, t.width);
        return 0;
    }
    RECT pr = { (short)mx, (short)my, (short)(t.width / 2), (short)t.height };
    LoadImage(&pr, (uint32_t *) t.pixels);
    eb->tpage  = getTPage(1, 0, mx, my);
    eb->xshift = mx / 64;                                /* per-tri tpage-X relocation */

    /* (3) Copy the MODEL [0, off_tim) into the fixed slot, then APPEND the 36-byte
     * directory (from the file tail at `begin`) right after it, and repoint the
     * container's directory pointer (bytes 0..3 = `begin`) at the relocated copy. The
     * TIM region between them is skipped → no TIM bytes resident (it's in VRAM). The
     * sub-section offsets (EDD/EMR/MD1 at idx 1/2/7) all point inside [0,off_tim) which
     * is copied; idx 8 (TIM) is never parsed here (out_tim = NULL). Re-parsing now
     * succeeds because the directory is present. */
    unsigned char *slot = s_enemy_arena + s_enemy_arena_used;
    memcpy(slot,           stg,         off_tim);       /* [model] */
    memcpy(slot + off_tim, stg + begin, dir_bytes);     /* [directory] (was at the tail) */
    slot[0] = (unsigned char)(off_tim        & 0xff);   /* repoint begin -> relocated dir */
    slot[1] = (unsigned char)((off_tim >>  8) & 0xff);
    slot[2] = (unsigned char)((off_tim >> 16) & 0xff);
    slot[3] = (unsigned char)((off_tim >> 24) & 0xff);
    if (re15_emd_parse_container(slot, (size_t)off_tim + dir_bytes,
                                 &eb->md1, &eb->skel, &eb->anim, NULL) != 0) {
        printf("[enemy] EM%02X slot reparse FAILED\n", (unsigned)type);
        return 0;
    }
    eb->buf = NULL;     /* arena-owned, static — must NOT be freed by re15_enemy_reset */
    eb->ok  = 1;
    s_enemy_arena_used += (int)resident;
    printf("[enemy] EM%02X OK: model=%uB (arena %d/%u)\n",
            (unsigned)type, off_tim, s_enemy_arena_used, (unsigned)RE15_ENEMY_ARENA_SIZE);
    return 1;
}

/* Load one room prop: parse its 8bpp MD1 + TIM (both pointers into resident memory
 * — RDT slices, see re15_load_room_props) and upload the texture to a structured
 * VRAM slot (one tex + CLUT row per prop, collision-free). The MD1 stays resident
 * (re15_md1_t references its bytes per frame). (Until Phase 2 the prop TIM was
 * CD-streamed via a `load_obj_prop` sibling; now everything comes from the RDT.) */
static void load_obj_prop_mem(int id,
                              const uint8_t *md1d, uint32_t md1sz,
                              const uint8_t *timd, uint32_t timsz)
{
    if (id < 0 || id >= 6) return;
    if (re15_md1_parse(md1d, (size_t)md1sz, &re15_obj_md1[id]) != 0) return;
    re15_tim_t t;
    if (re15_tim_parse(timd, (int)timsz, &t) != 0 || t.bpp != 8) return;
    if (t.has_clut) {
        int cy = re15_vram_alloc_clut(1);
        if (cy >= 0) {
            RECT cr = { 0, (short)cy, 256, 1 };
            LoadImage(&cr, (uint32_t *) t.clut);
            re15_obj_clut[id] = getClut(0, cy);
        }
    }
    int tx, ty;
    if (!re15_vram_alloc_tex(t.width, /*need_y0*/0, &tx, &ty)) return;
    RECT pr = { (short)tx, (short)ty, (short)(t.width / 2), (short)t.height };
    LoadImage(&pr, (uint32_t *) t.pixels);
    re15_obj_tpage[id] = getTPage(1, 0, tx, ty);
    re15_obj_ok[id]    = 1;
}

/* Multi-room data-driven prop loading: load the Obj_model_set prop MD1s+TIMs for
 * `room_id` into the re15_obj_* slots. Resets the prop-loop slots (0,1,3,4,5; slot
 * 2 = the heli ACTOR, loaded once at boot + cleared via re15_actor_init) so a
 * cross-room change drops the previous room's props. Call at boot (current room)
 * and from the room-change consume (the destination room). Replaces the old boot-
 * only hardcoded ROOM1170 obj00-05 block (audit #6). VRAM is reset separately by
 * re15_assets_reset_vram_allocators BEFORE this on a room change. */
void re15_load_room_props(unsigned room_id)
{
    (void) room_id;   /* props are now sourced from the resident room's RDT, not a per-id table */

    for (int i = 0; i < 6; i++)
        if (i != 2) re15_obj_ok[i] = 0;

    /* DATA-DRIVEN (globalization Phase 2, 2026-06-13): the room's prop MD1/TIM are
     * sliced out of the parsed RDT (re15_rdt_parse → prop_md1[]/prop_tim[], the
     * 0x30 model-pointer table) — pointers alias the resident RDT buffer, byte-true
     * (verified: room1170 obj00/01/03/04/05 + room1140 obj00 slices == the old
     * incbin/CD bytes). Replaces the hardcoded `if room_id==0x1170 …` block + the
     * per-room incbin/OBJ0N.TIM CD loads. Slot 2 is the helicopter ACTOR (loaded
     * once at boot, cleared via re15_actor_init) — NOT a prop-loop slot, so skip it.
     * Works for ANY room (boot + cross-room consume both re-parse g_room_rdt). */
    if (!g_room_rdt_ok) return;
    const re15_rdt_t *rdt = &g_room_rdt;
    int n = rdt->prop_count < 6 ? rdt->prop_count : 6;
    for (int k = 0; k < n; k++) {
        if (k == 2) continue;                                 /* heli actor slot */
        if (!rdt->prop_md1[k] || !rdt->prop_tim[k]) continue; /* prop absent in RDT */
        load_obj_prop_mem(k, rdt->prop_md1[k], (uint32_t)rdt->prop_md1_size[k],
                          rdt->prop_tim[k], (uint32_t)rdt->prop_tim_size[k]);
    }
}

/* Relocated heli VRAM slot. The big free block is x 320..640 (between the
 * framebuffers at x<320 and Leon's TIM + BG-MDEC cache at x>=640 — the BG
 * cache occupies (640,256)..(960,496), which an earlier (640,256) placement
 * collided with → the heli sampled striped BG data). CLUT sits beside Leon's
 * (Leon CLUT = x 0..256 @ y480; heli CLUT = x 256..512 @ y480). */
#define RE15_HELI_VRAM_X  320
#define RE15_HELI_VRAM_Y  256
/* CLUTs MUST avoid the pixel blocks. x256 row 480 overlapped the heli pixel
 * block (320,256)-(384,512) at y480 → corrupted CLUT entries 64-127 → white
 * heli parts rendered red. Park all relocated CLUTs at x0 (free below Leon's
 * CLUT rows 480-482; x0-256 has NO pixel block at y483+). */
#define RE15_HELI_CLUT_X  0
#define RE15_HELI_CLUT_Y  485

/* RE1.5 character-shadow blob (FUN_8001b064): 16bpp+STP texture uploaded to a
 * page-aligned VRAM slot, drawn as a subtractive (ABR=2) POLY_FT4 floor quad.
 * tpage carries the ABR-2 (subtractive) bits; UV origin is the page (0,0). */
int  re15_shadow_tpage = -1;
int  re15_shadow_ok    = 0;
/* page-aligned slot: x=960 (=64·15) below the FntLoad font (960,0)-(1024,256)
 * and right of the BG cache (…,256)-(960,496); 26×30 16bpp fits at (960,256). */
#define RE15_SHADOW_VRAM_X 960
#define RE15_SHADOW_VRAM_Y 256

/* RE1.5 subtitle GLYPH FONT (FUN_80028868): the TEX.TIM 4bpp font region uploaded
 * to a relocated free slot (the disc's (256,0) origin is inside our framebuffer),
 * + the 32×24 sub-CLUT. re15_render_msg_text emits one POLY_FT4 per glyph. CLUT
 * row per 0x05 speaker attr = (N&3)*2+(N>>2). 8 clut ids precomputed at upload. */
int re15_font_tpage = -1;
int re15_font_ok    = 0;
int re15_font_clut_id[8];
/* free page-aligned 4bpp slot (320,0)-(384,96): x320 < obj props (384), y0 <
 * heli (256) → clear. 64 VRAM units = 256px wide; 96 tall = glyph rows 0-5. */
#define RE15_FONT_VRAM_X 320
#define RE15_FONT_VRAM_Y 0
#define RE15_FONT_CLUT_X 256   /* below framebuffer-2 (ends y480); 32 wide × 24 */
#define RE15_FONT_CLUT_Y 480

/* Public sprite handle (consumed by render.c) */
int  re15_test_tim_loaded = 0;
int  re15_test_tim_tpage  = 0;
int  re15_test_tim_clut   = 0;
int  re15_test_tim_w      = 0;
int  re15_test_tim_h      = 0;

/* VRAM layout reminder (PSX has 1024x512 16-bit VRAM):
 *   (0,   0)..(320, 240)  Framebuffer #1
 *   (0, 240)..(320, 480)  Framebuffer #2
 *   (960, 0)..(1024, 256) PSn00bSDK debug font (FntLoad)
 *
 * Phase 4.5.7.5 placement:
 *   - Pixel data: (640, 0) — overrides TIM's baked-in data_x/y (which on
 *     PL00.tim wants (0,0) and would collide with Framebuffer 1).
 *   - CLUT: at TIM's BAKED clut_x/y. Most RE1.5 character TIMs target
 *     (0, 480)+ which is free (Framebuffer 1 ends at y=479). By honoring
 *     the TIM-baked CLUT coords, the MD1's per-triangle `uv->clut` field
 *     (= the original RE1.5 CLUT word) routes straight to the right
 *     CLUT row without any remap math.
 *
 * Phase 4.7+ will introduce a proper VRAM allocator. */
#define RE15_TIM_VRAM_X 640
#define RE15_TIM_VRAM_Y 0

/* Per-room cinematic + special-NPC loader (2026-06-14h) — DATA-DRIVEN per-room residency.
 * Streams the room's cinematic rbj overlay (\RBJ\ROOM%04X.RBJ, if the room HAS one) + its
 * special NPC (currently only Elliot, room1170) into the per-room arena, and applies the rbj
 * gesture overlay to Leon + that NPC. Called at boot for the initial room AND on EVERY room
 * enter (so a future room with its own cinematic/NPC works automatically; room1170 re-entry
 * re-loads them). The arena + Leon's skeleton were just reset to the clean PL00 base by
 * re15_assets_reset_vram_allocators, so we always overlay from the base. No-op for ordinary
 * rooms (no \RBJ\ file, no special NPC) — they keep the plain PL00 motions. */
void re15_load_room_cinematic(unsigned room_id)
{
    if (!re15_pl00_ok) return;   /* Leon's base must be parsed first (boot orders it before this) */

    /* (1) rbj cinematic overlay — DATA-DRIVEN by file presence (works for any room).
     * NOTE: kept INLINE on PSX (NOT the shared re15_apply_room_cinematic) — that shared
     * fn's parse-scratch path froze the RAM-tight PSX boot; this proven inline (scoped
     * stack locals the compiler reuses within one frame) fits. PC uses the shared fn. */
    const uint8_t *room_rbj = 0; uint32_t room_rbj_size = 0;
    char path[24];
    sprintf(path, "\\RBJ\\ROOM%04X.RBJ;1", room_id);
    int rn = re15_cd_load_file(path, re15_psx_staging_buf, RE15_PSX_STAGING_SIZE);
    if (rn > 0) {
        unsigned char *p = re15_arena_put(re15_psx_staging_buf, (size_t)rn);
        if (p) { room_rbj = p; room_rbj_size = (uint32_t)rn; }
    }
    if (room_rbj) {
        re15_emd_skeleton_t  base     = re15_pl00_skel;   /* clean PL00 bind + hierarchy */
        re15_emd_skeleton_t  rbj_skel = {0};
        re15_emd_animation_t rbj_anim = {0};
        if (re15_emd_parse_rbj(room_rbj, (size_t)room_rbj_size, &base, &rbj_skel, &rbj_anim) == 0) {
            re15_test_skel = rbj_skel;   /* Leon's cinematic-gesture overlay (aliases the arena) */
            re15_test_anim = rbj_anim;
        }
    }

    /* (2) Special NPC per room — DATA-DRIVEN by a small manifest (room_id → PLD basename of
     * the type-0x47 NPC). Add a room's NPC = a table row, NOT a code change. His mesh+skel
     * stream into the arena; his TIM goes to a relocated VRAM slot (baked CLUT collides with
     * Leon's); his EDD is not loaded (the rbj overlay replaces his animation). One NPC slot
     * (re15_elliot_*) — one special NPC per room, freed on room change. */
    static const struct { unsigned room; const char *npc; } s_room_npc[] = {
        { 0x1170, "ELLIOT" },   /* helipad intro */
    };
    const char *npc_name = 0;
    for (unsigned k = 0; k < sizeof(s_room_npc)/sizeof(s_room_npc[0]); k++)
        if (s_room_npc[k].room == room_id) { npc_name = s_room_npc[k].npc; break; }
    if (npc_name) {
        char npath[28];
        unsigned char *el_md1 = 0, *el_emr = 0;
        sprintf(npath, "\\PLD\\%s.MD1;1", npc_name);
        int el_md1_n = re15_cd_load_file(npath, re15_psx_staging_buf, RE15_PSX_STAGING_SIZE);
        if (el_md1_n > 0) el_md1 = re15_arena_put(re15_psx_staging_buf, (size_t)el_md1_n);
        sprintf(npath, "\\PLD\\%s.EMR;1", npc_name);
        int el_emr_n = re15_cd_load_file(npath, re15_psx_staging_buf, RE15_PSX_STAGING_SIZE);
        if (el_emr_n > 0) el_emr = re15_arena_put(re15_psx_staging_buf, (size_t)el_emr_n);
        if (el_md1 && el_emr &&
            re15_md1_parse(el_md1, el_md1_n, &re15_elliot_md1) == 0 &&
            re15_emd_parse_skeleton(el_emr, (size_t)el_emr_n, &re15_elliot_skel) == 0) {
            if (room_rbj) {   /* Elliot's dialog gestures = rbj clips 15/16/17/20/22/23 */
                re15_emd_animation_t e_rbj_anim = {0};
                re15_emd_skeleton_t  e_rbj_skel = {0};
                if (re15_emd_parse_rbj(room_rbj, (size_t)room_rbj_size,
                                       &re15_elliot_skel, &e_rbj_skel, &e_rbj_anim) == 0) {
                    re15_elliot_skel = e_rbj_skel;
                    re15_elliot_anim = e_rbj_anim;
                }
            }
            re15_tim_t et;
            sprintf(npath, "\\PLD\\%s.TIM;1", npc_name);
            int el_tn = re15_cd_load_file(npath, re15_cd_staging, RE15_CD_STAGING_SIZE);
            if (el_tn > 0 && re15_tim_parse(re15_cd_staging, el_tn, &et) == 0 && et.bpp == 8) {
                if (et.has_clut) {
                    int rows = (et.clut_entries >= 256) ? (et.clut_entries / 256) : 1;
                    if (rows < 1) rows = 1;
                    int cy = re15_vram_alloc_clut(rows);
                    if (cy >= 0) {
                        RECT cr = { 0, (short)cy, 256, (short)rows };
                        LoadImage(&cr, (uint32_t *) et.clut);
                        re15_elliot_clut       = getClut(0, cy);
                        re15_elliot_clut_yshift = cy - (int)et.clut_y;
                    }
                }
                int ex, ey;
                if (re15_vram_alloc_tex(et.width, /*need_y0*/1, &ex, &ey)) {
                    RECT pr = { (short)ex, (short)ey, (short)(et.width / 2), (short)et.height };
                    LoadImage(&pr, (uint32_t *) et.pixels);
                    re15_elliot_tpage       = getTPage(1, 0, ex, ey);
                    re15_elliot_tpage_xshift = ex / 64;
                    re15_elliot_ok          = 1;
                }
            }
        }
    }

    /* (3) Per-room RBJ → ENEMY binding: a NON-idle RBJ record (RECORD 1+) overrides a named
     * entity's animation. ROOM1150 RECORD 1 = prone wounded-breathing IRONS onto em45 (its own
     * clips are all upright). Keep the em45 MESH+EMR bind; replace keyframes+EDD. Data-driven. */
    if (room_rbj) {
        static const struct { unsigned room; uint8_t type; int record; } s_room_rbj_enemy[] = {
            { 0x1150, 0x45, 1 },   /* Irons: animation.rbj RECORD 1 = prone wounded-breathing */
        };
        for (unsigned k = 0; k < sizeof(s_room_rbj_enemy)/sizeof(s_room_rbj_enemy[0]); k++) {
            if (s_room_rbj_enemy[k].room != room_id) continue;
            uint8_t etype = s_room_rbj_enemy[k].type;
            if (!re15_enemy_find(etype)) re15_enemy_load(etype);   /* force-load em45 before rebind */
            re15_enemy_bank_t *eb = re15_enemy_find(etype);
            if (eb) {
                re15_emd_skeleton_t  r_skel = {0};
                re15_emd_animation_t r_anim = {0};
                if (re15_emd_parse_rbj_record(room_rbj, (size_t)room_rbj_size,
                        s_room_rbj_enemy[k].record, &eb->skel, &r_skel, &r_anim) == 0) {
                    eb->skel = r_skel;
                    eb->anim = r_anim;
                }
            }
        }
    }
}

void re15_assets_init(void)
{
    /* Phase 4.7: CD-stream Leon's TIM (PL00 player texture, ~99KB) into the shared
     * staging buffer → parse + VRAM upload → buffer freed. (Renamed disc file
     * TEST.TIM→PL00.TIM in the shared tree — globalization Phase 3-B 2026-06-13;
     * "test" was a dev placeholder, the asset is PL00.PLD's extracted texture.) */
    re15_tim_t tim;
    int leon_tn = re15_cd_load_file("\\PLD\\PL00.TIM;1", re15_cd_staging, RE15_CD_STAGING_SIZE);
    if (leon_tn <= 0 || re15_tim_parse(re15_cd_staging, leon_tn, &tim) != 0) {
        return;
    }

    /* Upload CLUT(s) at the TIM's BAKED VRAM coords. PL00 has 3 CLUTs
     * stacked vertically (head/body/limbs); honoring the baked coords
     * means each MD1 triangle's `uv->clut` field selects the right one
     * without any remap math.
     *
     * CLUT row width comes from the bpp (8bpp → 256 entries/row, 4bpp →
     * 16). Height = total_entries / row_width. */
    if (tim.has_clut) {
        int row_width = (tim.bpp == 4) ? 16 : 256;
        int row_count = row_width > 0 ? (tim.clut_entries / row_width) : 1;
        if (row_count < 1) row_count = 1;
        RECT clut_rect = { (short)tim.clut_x, (short)tim.clut_y,
                           (short)row_width, (short)row_count };
        LoadImage(&clut_rect, (uint32_t *) tim.clut);

        /* Fallback global CLUT — points at row 0. Per-triangle clut from
         * the MD1 overrides this for skeletal meshes; sprite paths still
         * use the global. */
        re15_test_tim_clut = getClut(tim.clut_x, tim.clut_y);
    }

    /* Upload pixel data — VRAM width in 16-bit units depends on bpp */
    int bpp_mode;
    int vram_w;
    switch (tim.bpp) {
        case 4:  bpp_mode = 0; vram_w = tim.width / 4; break;
        case 8:  bpp_mode = 1; vram_w = tim.width / 2; break;
        case 16: bpp_mode = 2; vram_w = tim.width;     break;
        default: return;
    }
    RECT data_rect = { RE15_TIM_VRAM_X, RE15_TIM_VRAM_Y,
                       (short)vram_w, (short)tim.height };
    LoadImage(&data_rect, (uint32_t *) tim.pixels);

    re15_test_tim_tpage = getTPage(bpp_mode, 0, RE15_TIM_VRAM_X, RE15_TIM_VRAM_Y);
    re15_test_tim_w     = tim.width;
    re15_test_tim_h     = tim.height;
    re15_test_tim_loaded = 1;

    DrawSync(0);

    /* Phase 4.5: parse test MD1 (display in Phase 4.5.2 via GTE).
     * test.md1 is now the PL00 (Leon) multi-mesh model with 17 sub-meshes
     * — one per bone of the skeleton parsed below. */
    if (re15_md1_parse(test_md1, (int)test_md1_size, &re15_test_md1) == 0) {
        re15_test_md1_ok = 1;
    }

    /* Phase 4.5.7: parse EDD + EMR. Skeleton holds 15 bones + ~711
     * keyframes; animation table holds 24 clips. Pose computation in
     * Phase 4.5.7.2 turns those into per-bone transformations the
     * skeletal mesh renderer (Phase 4.5.7.3) uses. */
    if (re15_emd_parse_animation(test_edd, (size_t)test_edd_size,
                                  &re15_test_anim) == 0 &&
        re15_emd_parse_skeleton (test_emr, (size_t)test_emr_size,
                                  &re15_test_skel) == 0)
    {
        re15_test_skel_ok = 1;

        /* AD-round port: build the PL00W01 walk-source composite NOW, while
         * re15_test_skel is still the CLEAN PL00 bind pose (the rbj overlay
         * below overwrites it). W01.EMR has a different bind (gun-raised
         * stance) than the PL00.MD1 mesh expects, so we keep PL00's bind +
         * hierarchy and only graft in W01's keyframe data (the walk/run
         * motion). Mirrors PC main.c AE-round (w01_skel = skel; copy kf_*). */
        re15_emd_skeleton_t  w01_raw  = {0};
        re15_emd_animation_t w01_anim = {0};
        if (re15_emd_parse_animation(pl00w01_edd, (size_t)pl00w01_edd_size,
                                     &w01_anim) == 0 &&
            re15_emd_parse_skeleton (pl00w01_emr, (size_t)pl00w01_emr_size,
                                     &w01_raw) == 0)
        {
            re15_w01_skel = re15_test_skel;   /* PL00 bind + hierarchy */
            re15_w01_skel.keyframe_data       = w01_raw.keyframe_data;
            re15_w01_skel.keyframe_data_size  = w01_raw.keyframe_data_size;
            re15_w01_skel.keyframe_count      = w01_raw.keyframe_count;
            re15_w01_skel.keyframe_size_bytes = w01_raw.keyframe_size_bytes;
            re15_w01_anim = w01_anim;
            re15_w01_ok   = 1;
        }

        /* Keep the PL00 BASE animation (idle/back) BEFORE the rbj overlay below
         * overwrites re15_test_anim/skel. At this point re15_test_skel is still the
         * PL00 bind + PL00 keyframes and re15_test_anim is PL00.edd's clip table. */
        re15_pl00_skel = re15_test_skel;
        re15_pl00_anim = re15_test_anim;
        re15_pl00_ok   = 1;
        /* (Leon's rbj cinematic overlay now happens in re15_load_room_cinematic, called at the
         * end of init + on every room enter — data-driven per-room. re15_test_skel stays the
         * clean PL00 base here; the W01 composite above was built from it before any overlay.) */
    }

    /* Phase 4.7 (multi-room foundation): the room RDT (ROOM1170, ~296KB) is no
     * longer incbin'd — it's STREAMED FROM CD via re15_room_load() into a resident
     * room buffer, parsed into g_room_rdt. We alias re15_test_rdt to it so the rest
     * of the engine is unchanged. This is the per-room load entry point: a door
     * transition will later just call re15_room_load(next_room_id). */
    if (re15_room_load(RE15_BOOT_ROOM) == 0) {
        re15_test_rdt    = g_room_rdt;     /* struct copy: pointers into the resident room buffer */
        re15_test_rdt_ok = 1;
    }

    /* #1c port (2026-06-01): parse the per-cut NCCT lighting set + apply cut 0,
     * mirroring PC main.c (BE-round). Without this g_re15_room_lights_ok stays
     * 0 and mesh_psx falls back to flat-neutral.
     * Multi-room (2026-06-05): the light set is parsed PER-ROOM from the RDT
     * (g_room_rdt.lights @ RDT+0x2c), so every room is lit correctly. (2026-06-14i:
     * the incbin'd ROOM1170 light FALLBACK was removed — dead, the RDT always has it.) */
    if (g_room_rdt.lights &&
        re15_light_parse(g_room_rdt.lights, (size_t)g_room_rdt.lights_size,
                         &g_re15_room_lights) == 0) {
        g_re15_room_lights_ok = 1;
        re15_light_apply_cut(&g_re15_room_lights, 0);
    }

    /* Text/subtitles (2026-06-05): interpret the room's MESSAGE table GENERICALLY
     * from its RDT (g_room_rdt.messages @ RDT+0x3c) — no longer the incbin'd
     * room1170 blob. The engine is an interpreter of the room data: every room's
     * Message_on(N) resolves against its own RDT message table. (Verified: this
     * yields the same intro subtitles the old incbin did — room1170 msg0 is
     * byte-identical.) */
    re15_msg_load_room_block(g_room_rdt.messages, g_room_rdt.messages_size);

    /* #2: helicopter body MD1 + TIM (rendered for actor type 0x47). Sliced from the
     * RDT prop table at index 2 — globalization Phase 2 (2026-06-13). Was incbin MD1
     * (room1170_obj02) + CD-streamed \PLD\OBJ02.TIM; both now come from g_room_rdt
     * (verified byte-true: prop[2] == room1170_obj02.md1/.tim). The prop LOOP skips
     * index 2 (it's this actor), so the heli is loaded here once at boot. */
    if (g_room_rdt_ok && g_room_rdt.prop_md1[2] &&
        re15_md1_parse(g_room_rdt.prop_md1[2], (size_t)g_room_rdt.prop_md1_size[2],
                       &re15_heli_md1) == 0) {
        re15_heli_md1_ok = 1;
    }

    /* #2: upload the heli TIM to its RELOCATED VRAM slot (baked coords collide
     * with the framebuffers + debug font). Unlike Leon (CLUT honored at baked
     * coords), the heli's CLUT is relocated too, so the prop renderer overrides
     * both tpage and clut with these handles instead of the MD1's baked words.
     * The TIM aliases the resident RDT buffer (no staging copy — LoadImage DMAs
     * its pixels/CLUT straight from there). */
    re15_tim_t htim;
    if (re15_heli_md1_ok && g_room_rdt.prop_tim[2] &&
        re15_tim_parse(g_room_rdt.prop_tim[2], g_room_rdt.prop_tim_size[2], &htim) == 0 &&
        htim.bpp == 8) {
        if (htim.has_clut) {
            int rows = (htim.clut_entries >= 256) ? (htim.clut_entries / 256) : 1;
            if (rows < 1) rows = 1;
            int cy = re15_vram_alloc_clut(rows);   /* structured CLUT alloc (was hardcoded 485) */
            if (cy >= 0) {
                RECT cr = { 0, (short)cy, 256, (short)rows };
                LoadImage(&cr, (uint32_t *) htim.clut);
                re15_heli_tim_clut = getClut(0, cy);
            }
        }
        RECT pr = { RE15_HELI_VRAM_X, RE15_HELI_VRAM_Y,
                    (short)(htim.width / 2), (short)htim.height };
        LoadImage(&pr, (uint32_t *) htim.pixels);
        re15_heli_tim_tpage = getTPage(1, 0, RE15_HELI_VRAM_X, RE15_HELI_VRAM_Y);
        re15_heli_tim_ok    = 1;
        /* The heli is PROP obj_id 2 (Obj_model_set / g_scd.props), NOT actor
         * type 0x47 (that's Elliot — PC dispatch is_elliot = type==0x47). Wire
         * the heli into the prop table at index 2 so the prop loop draws it. */
        re15_obj_md1[2]   = re15_heli_md1;
        re15_obj_tpage[2] = re15_heli_tim_tpage;
        re15_obj_clut[2]  = re15_heli_tim_clut;
        re15_obj_ok[2]    = 1;
    }

    /* #2 prop loop: load the current room's Obj_model_set props (obj02 = heli,
     * done above as an actor). Data-driven per-room now (audit #6) — boots ROOM1170;
     * the room-change consume re-calls re15_load_room_props(dest) for cross-room. */
    re15_load_room_props(RE15_BOOT_ROOM);

    /* RE1.5 character shadow blob (FUN_8001b064/af5c): upload the 26×30 16bpp
     * blob (RGB555 + per-texel STP) to the page-aligned slot. The page is 16-bit
     * (mode 2) with ABR=2 → semi-transparent POLY_FT4 darkens the floor by the
     * texel value (dst-=texel); zero texels are GPU-transparent (soft edge). */
    {
        RECT sh_rect = { RE15_SHADOW_VRAM_X, RE15_SHADOW_VRAM_Y,
                         RE15_SHADOW_BLOB_W, RE15_SHADOW_BLOB_H };
        LoadImage(&sh_rect, (uint32_t *) re15_shadow_blob_psx);
        DrawSync(0);
        /* mode 2 = 16bpp direct, abr 2 = subtractive (B - F). Slot is page-aligned
         * (x=64·15, y=256) so UV origin is the page (0,0). */
        re15_shadow_tpage = getTPage(2, 2, RE15_SHADOW_VRAM_X, RE15_SHADOW_VRAM_Y);
        re15_shadow_ok    = 1;
    }

    /* RE1.5 subtitle glyph font: upload the TEX.TIM 4bpp font region + sub-CLUT.
     * 4bpp page (mode 0), opaque (glyph bg = CLUT entry 0 = 0x0000 → transparent).
     * Precompute the 8 speaker CLUT-row ids (0x05 attr → row (N&3)*2+(N>>2)). */
    {
        RECT fr = { RE15_FONT_VRAM_X, RE15_FONT_VRAM_Y,
                    RE15_FONT_ATLAS_VRAMW, RE15_FONT_ATLAS_H };
        LoadImage(&fr, (uint32_t *) re15_font_atlas4);
        RECT cr = { RE15_FONT_CLUT_X, RE15_FONT_CLUT_Y,
                    RE15_FONT_CLUT_W, RE15_FONT_CLUT_H };
        LoadImage(&cr, (uint32_t *) re15_font_clut);
        DrawSync(0);
        re15_font_tpage = getTPage(0, 0, RE15_FONT_VRAM_X, RE15_FONT_VRAM_Y);
        for (int n = 0; n < 8; n++) {
            int row = ((n & 3) * 2) + ((n >> 2) & 1);
            re15_font_clut_id[n] = getClut(RE15_FONT_CLUT_X, RE15_FONT_CLUT_Y + row);
        }
        re15_font_ok = 1;
    }

    /* Per-room cinematic + special NPC (rbj overlay + Elliot) for the BOOT room — data-driven,
     * shared with the per-room-enter path (re15_load_room_cinematic). Leon is parsed + pl00 is
     * saved above, the room RDT is loaded (g_current_room_id set), so this is the right moment.
     * On later DOOR transitions the same function runs from re15_room_apply_pending. */
    re15_load_room_cinematic(g_current_room_id);

    /* (em21 boot-load removed 2026-06-14f — the crows now lazy-load via the generic
     * per-room enemy arena, re15_enemy_load(0x21) → \EMD\EM21.EMD, on first spawn.) */
}
