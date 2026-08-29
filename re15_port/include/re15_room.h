/*
 * RE1.5 Rebuilt — per-room loader (Phase 4.7 multi-room foundation, 2026-06-02).
 *
 * RE2-canonical room model: a room's RDT (the container holding its cameras,
 * zones, SCD scripts, lights, prop models, sounds, messages) is STREAMED FROM CD
 * into a resident buffer on room-enter, replacing the previous room's. The RDT
 * stays resident for the room's lifetime (the parsed re15_rdt_t holds pointers
 * into it), and is overwritten when the next room loads — so ONE buffer serves
 * every room. This is the foundation for real room transitions (door AOT ->
 * re15_room_load(next)), replacing the single hardcoded incbin'd ROOM1170.
 */
#ifndef RE15_ROOM_H
#define RE15_ROOM_H

#include "re15_rdt.h"
#include "re15_camera.h"   /* re15_camera_cut_t / re15_camera_view_t for the apply ctx */

/* The current room's parsed RDT + validity flag. (Defined in the SHARED
 * room_common.c so BOTH ports link the cross-room state.) */
extern re15_rdt_t g_room_rdt;
extern int        g_room_rdt_ok;
extern unsigned   g_current_room_id;   /* room resident in g_room_rdt (per-room asset paths) */

/* RE2-KARTENSYSTEM (Port-Erweiterung, Nutzer-Auftrag 2026-08-30 — re15_map_visited.c):
 * 1 Besucht-Bit je re15_room_ids[]-Eintrag; markiert am Raum-Lade-Choke-Point
 * (scd_room_reenter), serialisiert als Save-v6-Block. */
void re15_map_visited_reset(void);
void re15_map_visited_mark(unsigned room_id);
int  re15_map_visited(unsigned room_id);
void re15_map_visited_export(uint8_t out[32]);
void re15_map_visited_import(const uint8_t in[32]);

/* Zustand eines Karten-Rects (Seite + Rect-Index der EXE-Rect-Liste @0x80076840):
 * staerkster Zustand aller dem Rect zugeordneten Raeume. */
enum {
    RE15_MAP_RECT_UNMAPPED  = 0,   /* keine Zuordnung -> Stock-Neutralton */
    RE15_MAP_RECT_UNVISITED = 1,   /* zugeordnet, keiner der Raeume besucht -> schwarz */
    RE15_MAP_RECT_VISITED   = 2,   /* mindestens ein Raum besucht -> gruen */
    RE15_MAP_RECT_CURRENT   = 3    /* enthaelt den aktuellen Raum -> rot */
};
int re15_map_rect_state(unsigned page, unsigned rect_idx);

/* MARKER-REPARATUR (Nutzer-Auftrag 2026-08-30): reparierte Parameter der Marker-
 * Formel FUN_800473f8 fuer Slots, deren EXE-Zeile @0x800768B0 Platzhalter ist oder
 * ihr Rect verfehlt (Tabelle: re15_map_row_fix.h, tools/gen_map_tables.py).
 * re15_map_stock_mode(): 1 wenn RE15_MAP_STOCK=1 (byte-true Auslieferungsstand,
 * Reparatur + RE2-Faerbung aus). */
typedef struct { unsigned char slot; short xoff; unsigned short yoff, xscl, zscl; } re15_map_row_fix_t;
const re15_map_row_fix_t *re15_map_row_fix_find(unsigned slot);
int re15_map_stock_mode(void);
void re15_map_stock_set(int v);   /* Tests: 0/1 erzwingen, -1 = Umgebung neu lesen */

/* Death/game-over presentation (game_step_common.c). g_death_fade = 0..255 fade-to-black over the
 * byte-true 0x78 death timer; g_gameover_active = 1 once it expires -> the PC loop shows YOU DIED. */
extern int g_death_fade;      /* BLACK exit fade 0..255 (game-over sub 6) */
extern int g_gameover_active;
extern int g_death_white;     /* ADDITIVE white overlay 0..255 (FUN_80021880 level>>7) */
extern int g_death_blackbg;   /* flat-black background mode (FUN_80021634(2,0)) */
extern int g_death_cam;       /* death-camera glide active */
extern int g_death_pool;      /* blood-pool growth ticks (500+12t x 600+12t) */
extern int g_death_flyin;     /* YOU DIED letter fly-in tick 0..50, -1 hidden */
extern int g_death_glow;      /* spotlight backdrop brightness (gradual decay) */

/* The room the game boots into (RE1.5 intro = the helipad, STAGE1/ROOM1170). The single
 * entry point; everything else is reached via doors (data-driven). */
#define RE15_BOOT_ROOM  0x1170

/* Load ROOM<id>.RDT into g_room_rdt and parse it. This is the ARCHITECTURE-
 * specific loader: the PSX build (re15_room.c) streams it from CD; the PC build
 * (room_pc.c) reads the assets/room####.rdt file. Returns 0 on success
 * (g_room_rdt_ok=1), -1 on failure. The shared transition (re15_room_apply_pending)
 * calls it through the per-port ctx->load_rdt callback, NOT directly. */
int re15_room_load(unsigned room_id);

/* Pending cross-room transition request. The AOT scan sets this when the
 * player enters a DOOR whose destination differs from the current room; the
 * main loop consumes it AFTER the scan (a room change reloads the RDT + resets
 * VRAM, which must not happen mid-scan). */
typedef struct {
    int       pending;      /* 1 = a transition is queued */
    unsigned  room_id;      /* destination ROOM####.RDT id (e.g. 0x1140) */
    int32_t   x, y, z;      /* player spawn in the destination room */
    int16_t   yaw_4096;
    int       target_cut;   /* entry camera cut in the destination (Door_aot_set dest_cut) */
} re15_room_change_t;
extern re15_room_change_t g_room_change;

/* Queue a cross-room transition (called from the DOOR AOT fire). The door's
 * Door_aot_set carries the destination spawn pos/yaw AND the entry camera cut.
 * SHARED (room_common.c) — both ports queue the same way. */
void re15_room_request_change(unsigned room_id, int32_t x, int32_t y, int32_t z,
                              int16_t yaw, int target_cut);

/* SHARED cross-room transition apply (room_common.c). The two main loops fill
 * this ctx with their per-port handles + the three ARCHITECTURE callbacks
 * (RDT load = CD vs file, render reset = VRAM/BG vs texcache, per-cut BG decode
 * = MDEC vs file) and call re15_room_apply_pending() once per frame AFTER the
 * AOT scan. It runs the IDENTICAL transition LOGIC (re-alias RDT, re-init
 * actors/SCD, set spawn + entry cut, per-room lights/messages/band/BGM, stair
 * reset) on both ports. Returns 1 if a transition was applied this frame. */
typedef struct {
    re15_rdt_t               *rdt;             /* &re15_test_rdt / &rdt (re-aliased to g_room_rdt) */
    int                      *rdt_ok;
    const re15_camera_cut_t **active_cuts;     /* re-pointed to the new room's cuts */
    int                      *active_cut_count;
    int                      *cam_active_cut;  /* set to the door's entry cut */
    re15_camera_view_t       *cam_view;        /* rebuilt for the entry cut */
    int  (*load_rdt)(unsigned room_id);        /* ARCH: CD (PSX) / file (PC) → fills g_room_rdt */
    void (*reset_render)(void);                /* ARCH: VRAM rewind + BG invalidate (PSX) / texcache (PC) */
    int  (*load_bg_cut)(int cut);              /* ARCH: decode the entry cut's BG (PSX MDEC / PC file); return ignored */
    void (*load_props)(unsigned room_id);      /* ARCH: load the dest room's Obj_model_set prop MD1s+TIMs (data-driven). NULL ok. */
    void (*load_cinematic)(unsigned room_id);  /* ARCH: load the dest room's cinematic rbj overlay + special NPC into the per-room arena (PSX). NULL ok. */
} re15_room_apply_ctx_t;

int re15_room_apply_pending(const re15_room_apply_ctx_t *ctx);

/*---------------------------------------------------------------------------
 * RAUM-TRANSITIONS-PRAESENTATION (Transitions-FSM FUN_8001c958, Zustands-Byte
 * DAT_800b5359, Tabelle @0x8001069c). Vollstaendige Adress-Belege stehen im
 * Kopfkommentar von room_common.c.
 *
 *   re15_room_transition_present()  = State-1-Tuerzweig + State-3-Rumpf
 *       (@0x8001ca44 Pause-Flags loeschen, @0x8001cbdc Spieler-Kommandowort,
 *        @0x8001cc00/@0x8001cc18 die -0x1800-Einblendung, @0x8001cc20-28 State 4)
 *   re15_room_transition_tick()     = State 4 (@0x8001cc34, Freeze 0xff000000)
 *        und State 5 (@0x8001cc70, Freigabe wenn die Blende fertig ist).
 *        MUSS am FRAME-ANFANG laufen, VOR allen Subsystemen — im Original
 *        dispatcht die FSM @0x8001c994, die Subsystem-Aufrufe folgen erst
 *        @0x8001cdec. Rueckgabe 1 = Transition laeuft (Logik eingefroren).
 *--------------------------------------------------------------------------*/
void re15_room_transition_present(void);
int  re15_room_transition_tick(void);
int  re15_room_transition_active(void);
void re15_room_transition_reset(void);

#endif /* RE15_ROOM_H */
