/* probe_11f0_puzzle_pos.c — MESSSONDE: WO STEHT DER SPIELER WAEHREND/NACH DEM
 * GENERATOR-RAETSEL (ROOM11F0)?
 *
 * Nutzer-Beobachtung: "Nach dem Generator-Raetsel stand man nicht mehr VOR dem
 * Generator, sondern weiter weg davon. Bewegt man sich vielleicht auch PHYSISCH
 * innerhalb des Raetsels?"
 *
 * Der bisherige Fluchtlauf-Sweep (probe_11f0_escape_sweep) startete ab einem
 * ANGENOMMENEN Standpunkt. Diese Sonde MISST die Spur statt sie anzunehmen: echter
 * Spawn -> echter Anmarsch ueber re15_game_step (Tank-Pad) -> echter QUADRAT-Druck auf
 * den Panel-AOT -> echte Raetsel-Bedienung mit dem D-Pad -> Positions-Log.
 *
 * ORIGINAL-SEITE (selbst disassembliert, keine Annahme):
 *   ROOM11F0.RDT sub01 @0x108C laeuft JEDES BILD. Ab @0x12A4:
 *       0x12A4 Ifel_ck  { 0x12A8 Ck(5,0x0C)==1
 *                         0x12AC Set(2,0,1)
 *                         0x12B0 Set(2,2,1) }
 *   sub17 (Abbruch): 0x16E8 Set(2,0,0) / 0x16EC Set(2,2,0)
 *   sub18 (geloest): 0x172E Set(2,0,0) / 0x1732 Set(2,2,0) / 0x1736 Set(2,7,1)
 *                    ... 0x1784 Set(2,7,0)
 *   `Set` = LAB_8003fdd0 (SCD-Dispatch @0x80074530):
 *       @0x8003fdd8 lbu v1,1(v0)   = zone
 *       @0x8003fddc lbu a1,2(v0)   = bit
 *       @0x8003fde0 lbu a2,3(v0)   = op
 *       @0x8003fdec sll v1,v1,2
 *       @0x8003fdf8 lui at,0x8007 / @0x8003fdfc addiu at,at,0x4664 / @0x8003fe00 addu at,at,v1
 *       @0x8003fe04 lw  v1,0(at)   = PTR_DAT_80074664[zone]
 *       @0x8003fdd4 lui t0,0x8000  + @0x8003fe68 srlv v1,t0,a1  = 0x80000000 >> (bit&0x1f)
 *       op 1 -> or @0x8003fe6c / op 0 -> nor+and @0x8003fe54-58 / op 7 -> xor @0x8003fe80
 *   Zeiger-Tabelle @0x80074664: [0]=DAT_800aca38 [1]=DAT_800aca3c [2]=DAT_800aca40 ...
 *   => Zone 2 IST das Pause-Wort DAT_800aca40 (= g_re15_pauseflags):
 *      Set(2,0,*) = Bit 0x80000000 = RE15_PAUSE_PLAYER (FUN_80031c44 @0x80031c78 bltz)
 *      Set(2,2,*) = Bit 0x20000000 = RE15_PAUSE_AI     (z.B. FUN_80100424 @0x8010043c)
 *      Set(2,7,*) = Bit 0x01000000 = RE15_PAUSE_PAD    (FUN_80030444 @0x800304f8)
 *
 * Gemessen wird deshalb: bewegt sich der Spieler waehrend der Raetsel-Bedienung?
 */
#include "re15_rdt.h"
#include "re15_scd.h"
#include "re15_actor.h"
#include "re15_aot.h"
#include "re15_room.h"
#include "re15_player.h"
#include "re15_camera.h"
#include "re15_game_step.h"
#include "re15_collision.h"
#include "re15_inventory.h"
#include "re15_msg.h"
#include "re15_enemy_ai.h"
#include "re15_enemy.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#ifndef RE15_ASSET_PSX_DIR
#define RE15_ASSET_PSX_DIR "shared_assets/PSX"
#endif

extern scd_vm_t         g_scd;
extern re15_aot_state_t g_aot;
extern uint32_t         g_re15_pauseflags;

static re15_rdt_t         s_rdt;
static re15_camera_view_t s_cam;
static re15_game_ctx_t    s_ctx;
static uint8_t           *s_raw = NULL;
static size_t             s_rawsz = 0;

static uint8_t *slurp(const char *p, size_t *n)
{
    FILE *f = fopen(p, "rb"); if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    if (sz <= 0) { fclose(f); return NULL; }
    uint8_t *b = (uint8_t *)malloc((size_t)sz);
    if (b && fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); b = NULL; }
    fclose(f); if (b) *n = (size_t)sz; return b;
}

/* ===== ORAKEL — FUN_80014230 / FUN_80014324 / FUN_80014368 byte-exakt aus den RDT-Bytes
 * (identisch zu probe_11f0_escape_sweep, dort ueber 21 Raeume x 56,7 Mio. Punkte gegen
 * re15_aot_scan geprueft). Record = 0x14 B: +1 floor, +2 cam_from, +3 cam_to, +4.. 4 Ecken. */
static uint32_t g_rvd_off = 0;
static int      g_rvd_n   = 0;
static int16_t rs16(uint32_t off) { return (int16_t)(uint16_t)(s_raw[off] | (s_raw[off+1] << 8)); }

static int orig_point_in_quad(int32_t px, int32_t pz, uint32_t rec)   /* @0x80014368 */
{
    int32_t x0 = rs16(rec + 4),  z0 = rs16(rec + 6);
    int32_t a4 = pz - z0, a5 = rs16(rec + 8) - x0, a1 = px - x0;
    int32_t a2 = rs16(rec + 10) - z0, a6 = rs16(rec + 0x10) - x0, a7 = rs16(rec + 0x12) - z0;
    if (a5 * a4 <= a2 * a1) {                       /* @0x800143a4-b0 */
        if (a6 * a4 < a7 * a1) return 0;            /* @0x800143b4-c4 */
        int32_t b3 = rs16(rec + 0x0e) - z0, b8 = rs16(rec + 0x0c) - x0;
        if (((a2 - b3) * (a1 - b8) <= (a5 - b8) * (a4 - b3)) &&
            ((a6 - b8) * (a4 - b3) <= (a7 - b3) * (a1 - b8))) return 1;
    }
    return 0;
}
static long orig_group_ptr(int cut)                                   /* @0x80014324 */
{
    for (int i = 0; i < g_rvd_n; i++) {
        uint32_t rec = g_rvd_off + (uint32_t)i * 0x14u;
        if (s_raw[rec + 2] == (uint8_t)cut) return (long)rec;
    }
    return -1;
}
static int orig_rvd_scan(int cut, int32_t px, int32_t pz, int floor)  /* @0x80014230 */
{
    long grp = orig_group_ptr(cut);
    if (grp < 0) return -1;
    uint32_t s1 = (uint32_t)grp + 0x14, s0 = (uint32_t)grp + 0x16;
    if (s_raw[(uint32_t)grp + 0x16] != (uint8_t)cut) return -1;       /* @0x80014254-5c */
    for (;;) {
        uint8_t fl = s_raw[s0 - 1];                                   /* @0x8001426c rec+1 */
        if (fl == 0xFF || (int)fl == floor)
            if (orig_point_in_quad(px, pz, s1)) return s_raw[s0 + 1]; /* @0x800142a8 rec+3 */
        s0 += 0x14; s1 += 0x14;
        if (s0 + 0x14 > s_rawsz) return -1;
        if (s_raw[s0] != (uint8_t)cut) return -1;                     /* @0x800142c0-d0 */
    }
}

static int s_shown = 0;    /* == main.c s_last_cut_idx (der ANGEZEIGTE Cut) */

/* Ein Bild in der Reihenfolge des PC-Ports (main.c 3626 / 3751 / 3951 / 4810). */
static void frame(uint16_t held, uint16_t edge)
{
    const unsigned char *raw; int len, id;
    scd_vm_tick();                                         /* main.c:3626 */
    re15_msg_tick(&raw, &len, &id);                        /* main.c:3751 */
    if (re15_cam_present_tick()) s_shown = (int)g_scd.cam_id;  /* main.c:3951 */
    s_ctx.active_cut  = s_shown;                           /* main.c:3952 / gctx.active_cut */
    s_ctx.pad_current = held;
    s_ctx.pad_pressed = edge;
    re15_game_step(&s_ctx);                                /* main.c:4810 */
}

static void pstate(const char *tag, int f)
{
    const re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    printf("   %-10s f%-4d pos=(%7ld,%7ld) rot=%5d | pause=%08lX z2b0=%d z2b2=%d z2b7=%d "
           "| f5_0=%d f5_0C=%d cam=%u msg=%u\n",
           tag, f, (long)pl->x, (long)pl->z, (int)pl->rot_y,
           (unsigned long)g_re15_pauseflags,
           re15_game_flag_get(2, 0), re15_game_flag_get(2, 2), re15_game_flag_get(2, 7),
           re15_game_flag_get(5, 0), re15_game_flag_get(5, 0x0C),
           (unsigned)g_scd.cam_id, (unsigned)g_scd.message_active);
}

/* ---- Auto-Pilot: Tank-Fahrt zum Ziel, Pad-Bits werden protokolliert ------------------- */
static char  s_tok[16384];
static int   s_toklen = 0;
static uint16_t s_lastbits = 0xFFFF;
static int   s_runlen = 0;

static void tok_flush(void)
{
    if (s_lastbits == 0xFFFF || s_runlen == 0) return;
    char letters[8]; int n = 0;
    if (s_lastbits & RE15_PAD_BIT_CROSS) letters[n++] = 'X';
    if (s_lastbits & RE15_PAD_BIT_UP)    letters[n++] = 'U';
    if (s_lastbits & RE15_PAD_BIT_DOWN)  letters[n++] = 'D';
    if (s_lastbits & RE15_PAD_BIT_LEFT)  letters[n++] = 'L';
    if (s_lastbits & RE15_PAD_BIT_RIGHT) letters[n++] = 'R';
    if (s_lastbits & RE15_PAD_BIT_SQUARE)letters[n++] = 'A';
    if (n == 0) letters[n++] = 'W';
    letters[n] = 0;
    s_toklen += snprintf(s_tok + s_toklen, sizeof s_tok - (size_t)s_toklen,
                         "%s%s%.4f", s_toklen ? "," : "", letters, s_runlen / 30.0);
    s_runlen = 0;
}

static void tok_add(uint16_t bits)
{
    if (bits != s_lastbits) { tok_flush(); s_lastbits = bits; }
    s_runlen++;
}

static void frame_rec(uint16_t held, uint16_t edge)
{
    tok_add(held);
    frame(held, edge);
}

/* Ziel anlaufen: erst drehen (in-place, 96/Frame), dann rennen (X+U). */
static int drive_to(int32_t tx, int32_t tz, int max_frames, int32_t tol)
{
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    int stuck = 0;
    for (int i = 0; i < max_frames; i++) {
        int32_t dx = tx - pl->x, dz = tz - pl->z;
        if ((long long)dx*dx + (long long)dz*dz < (long long)tol*tol) return 1;
        double ang = atan2((double)(-dz), (double)dx) * 4096.0 / 6.283185307179586;
        int want = ((int)(ang + 0.5)) & 0x0FFF;
        int err  = (want - (int)(pl->rot_y & 0x0FFF) + 2048 + 4096) % 4096 - 2048;
        uint16_t bits;
        if (err > 60)       bits = RE15_PAD_BIT_RIGHT;
        else if (err < -60) bits = RE15_PAD_BIT_LEFT;
        else                bits = (uint16_t)(RE15_PAD_BIT_UP | RE15_PAD_BIT_CROSS);
        int32_t bx = pl->x, bz = pl->z; int br = pl->rot_y;
        frame_rec(bits, 0);
        if (pl->x == bx && pl->z == bz && pl->rot_y == br) { if (++stuck > 20) return 0; }
        else stuck = 0;
    }
    return 0;
}

static void room_boot_at(int32_t px, int32_t pz, int16_t rot)
{
    memset(&s_cam, 0, sizeof s_cam); memset(&s_ctx, 0, sizeof s_ctx);
    s_ctx.rdt = &s_rdt; s_ctx.rdt_ok = 1; s_ctx.cam_view = &s_cam; s_ctx.active_cut = 0;
    re15_actor_init(); re15_aot_init(); scd_vm_init();
    re15_enemy_reset(); re15_enemy_ai_set_paused(0);
    re15_player_cmd_reset();
    re15_pauseflags_clear();          /* @0x8001ca44 `sw zero,0x800aca40` (Raumwechsel) */
    g_current_room_id = 0x11F0; g_room_change.pending = 0;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100; pl->hit_react = 0;
    pl->state = 0; pl->motion = 0; pl->floor = 0; pl->y = 0;
    pl->x = px; pl->z = pz; pl->rot_y = rot;
    re15_collision_set_band(0);
    re15_inv_load_briefing();
    s_shown = 0;
    /* ⚠️ OHNE DAS IST DIE SONDE UNEHRLICH: der echte Raumwechsel laedt den MESSAGE-Block
     * (room_common.c:381 -> re15_msg_load_room_block, RDT+0x3c). Fehlt er, liefert
     * re15_msg_get_raw NULL, re15_dialog_step verwirft die Nachricht sofort
     * (msg_common.c:430) und sub16s `Evt_next` laeuft durch, OHNE dass jemand
     * bestaetigt hat. Genau daran ist die erste Fassung dieser Sonde vorbeigelaufen:
     * sie oeffnete das Raetsel mit einem einzigen QUADRAT, das echte Spiel nicht. */
    re15_msg_load_room_block(s_rdt.messages, s_rdt.messages_size);
    scd_room_reenter(&s_rdt, pl->x, pl->z, 0);
    for (int f = 0; f < 30; f++) frame(0, 0);
}

/* Spawn wie im ECHTEN Spiel gemessen (debug.log: "[walk] pl pos=(250,0,250) rot=1024"). */
static void room_boot(void) { room_boot_at(250, 250, 1024); }

/* QUADRAT auf dem Panel druecken, bis sub16 das Raetsel oeffnet (flag(5,0x0C)). */
static int open_puzzle(void)
{
    for (int f = 0; f < 400; f++) {
        uint16_t b = (f % 8 == 0) ? RE15_PAD_BIT_SQUARE : 0;
        frame(b, b);
        if (re15_game_flag_get(5, 0x0C)) return f;
    }
    return -1;
}

/* Win-Muster setzen und sub18 komplett durchlaufen lassen (Nachrichten wegbestaetigen). */
static void solve_puzzle(int frames)
{
    static const uint8_t pat[10] = { 1, 0, 1, 0, 1, 0, 1, 0, 1, 0 };
    for (int n = 0; n < 10; n++) re15_game_flag_set(5, (uint8_t)(0x0D + n), pat[n]);
    for (int f = 0; f < frames; f++) {
        uint16_t e = (g_scd.message_active && (f % 6) == 0) ? RE15_PAD_BIT_SQUARE : 0;
        frame(e, e);
    }
}

/* ---- Token-Replay: exakt das Format von RE15_INPUT_SCRIPT (input_pc.c script_parse_once).
 * Damit laesst sich ein Tank-Skript hier billig validieren, bevor es die echte Exe faehrt. */
static uint16_t tank_bit(char c)
{
    switch (c) {
    case 'U': return RE15_PAD_BIT_UP;    case 'D': return RE15_PAD_BIT_DOWN;
    case 'L': return RE15_PAD_BIT_LEFT;  case 'R': return RE15_PAD_BIT_RIGHT;
    case 'X': return RE15_PAD_BIT_CROSS; case 'A': return RE15_PAD_BIT_SQUARE;
    default:  return 0;
    }
}
static void tank_replay(const char *s)
{
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    const char *p = s;
    while (*p) {
        while (*p == ',' || *p == ' ') p++;
        if (!*p) break;
        uint16_t bits = 0;
        while (*p && ((*p >= 'A' && *p <= 'Z'))) { bits |= tank_bit(*p); p++; }
        char *end = NULL; double secs = strtod(p, &end);
        if (end == p) { p++; continue; }
        p = end;
        int n = (int)(secs * 30.0 + 0.5);
        static uint16_t prev = 0;
        for (int i = 0; i < n; i++) { frame(bits, (uint16_t)(bits & ~prev)); prev = bits; }
        printf("   Token bits=%04X %3d Bilder -> (%7ld,%7ld) rot=%5d cut=%d f5_0C=%d "
               "msg=%u/id%u fsm=%u pause=%08lX\n",
               bits, n, (long)pl->x, (long)pl->z, (int)pl->rot_y, s_shown,
               re15_game_flag_get(5, 0x0C), (unsigned)g_scd.message_active,
               (unsigned)g_scd.message_id, (unsigned)g_scd.message_fsm,
               (unsigned long)g_re15_pauseflags);
    }
}

int main(int argc, char **argv)
{
    char rp[600];
    snprintf(rp, sizeof rp, "%s/STAGE1/ROOM11F0.RDT", RE15_ASSET_PSX_DIR);
    s_raw = slurp(rp, &s_rawsz);
    if (!s_raw) { printf("SKIP: %s fehlt\n", rp); return 77; }
    if (re15_rdt_parse(s_raw, s_rawsz, &s_rdt) < 0) { printf("FAIL: RDT-Parse\n"); return 1; }

    /* Skript-Anker gegen stille Daten-Drift. */
    if (!(s_raw[0x12A4] == 0x06 && s_raw[0x12A8] == 0x21 && s_raw[0x12A9] == 0x05 &&
          s_raw[0x12AA] == 0x0C && s_raw[0x12AC] == 0x22 && s_raw[0x12AD] == 0x02 &&
          s_raw[0x12AE] == 0x00 && s_raw[0x12AF] == 0x01 &&
          s_raw[0x12B0] == 0x22 && s_raw[0x12B1] == 0x02 && s_raw[0x12B2] == 0x02 &&
          s_raw[0x12B3] == 0x01)) {
        printf("FAIL: sub01 @0x12A4 ist nicht `If Ck(5,0x0C) { Set(2,0,1); Set(2,2,1) }`\n");
        return 1;
    }
    printf("[anchor] sub01 @0x12A4 If Ck(5,0x0C)==1 -> @0x12AC Set(2,0,1) @0x12B0 Set(2,2,1)  OK\n");

    /* `-tank "<RE15_INPUT_SCRIPT-Tokens>"` = das Skript fuer den ECHTEN Lauf hier validieren. */
    if (argc >= 3 && strcmp(argv[1], "-tank") == 0) {
        room_boot();
        printf("[tank-replay] ab Spawn (250,250,rot=1024):\n");
        tank_replay(argv[2]);
        re15_actor_t *p2 = &g_actors[RE15_ACTOR_SLOT_PLAYER];
        printf("[tank-replay] ENDE pos=(%ld,%ld) rot=%d pause=%08lX f5_0C=%d\n",
               (long)p2->x, (long)p2->z, (int)p2->rot_y,
               (unsigned long)g_re15_pauseflags, re15_game_flag_get(5, 0x0C));
        free(s_raw); return 0;
    }

    room_boot();
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    printf("\n===== M1: Anmarsch zum Generator-Panel (AOT slot1 @0x0D64, "
           "Rect x[-2200..-1400] z[-15850..-14450]) =====\n");
    pstate("spawn", 0);
    /* Wegpunkte aus probe_11f0_escape_sweep P1 (dort per march() validiert). */
    int ok1 = drive_to(250,   -13350, 900, 400);
    pstate("wp1", ok1);
    int ok2 = drive_to(-530,  -16032, 900, 400);
    pstate("wp2", ok2);
    int ok3 = drive_to(-1800, -15150, 900, 300);
    pstate("panel", ok3);

    /* Auf das Panel ausrichten: Rect-Mitte, Blick nach -Z (rot 1024) — der FORWARD-620-Test
     * (sce_flags 0x31 = Pool 1 | ACTION 0x10 | FORWARD 0x20) prueft den Punkt 620 voraus. */
    for (int i = 0; i < 40; i++) {
        int err = (1024 - (int)(pl->rot_y & 0x0FFF) + 2048 + 4096) % 4096 - 2048;
        if (err > 40)       frame_rec(RE15_PAD_BIT_RIGHT, 0);
        else if (err < -40) frame_rec(RE15_PAD_BIT_LEFT, 0);
        else break;
    }
    for (int i = 0; i < 10; i++) frame_rec(0, 0);
    pstate("ausger.", 0);
    tok_flush();
    printf("[tank] Pad-Skript bis hierher: RE15_INPUT_SCRIPT=\"%s\"\n", s_tok);

    printf("\n===== M2: QUADRAT auf dem Panel -> sce-3 -> sub16 =====\n");
    int fired = open_puzzle();
    pstate("nach sub16", fired);
    if (!re15_game_flag_get(5, 0x0C)) {
        printf("   !! sub16 hat flag(5,0x0C) NICHT gesetzt — Raetsel nicht offen\n");
        printf("   (AOT slot1: active=%d type=%d flags=0x%02X ev=%u)\n",
               g_aot.slots[1].active, g_aot.slots[1].type, g_aot.slots[1].sce_flags,
               g_aot.slots[1].event_id);
        free(s_raw); return 1;
    }

    printf("\n===== M3: RAETSEL OFFEN — D-Pad halten (Cursor fahren) und POSITION messen =====\n");
    int32_t x0 = pl->x, z0 = pl->z;
    struct { uint16_t bit; const char *name; } DIRS[4] = {
        { RE15_PAD_BIT_UP,    "UP   " }, { RE15_PAD_BIT_DOWN,  "DOWN " },
        { RE15_PAD_BIT_LEFT,  "LEFT " }, { RE15_PAD_BIT_RIGHT, "RIGHT" },
    };
    for (int d = 0; d < 4; d++) {
        int32_t bx = pl->x, bz = pl->z;
        for (int f = 0; f < 60; f++) frame(DIRS[d].bit, f == 0 ? DIRS[d].bit : 0);
        printf("   60x %s: (%7ld,%7ld) -> (%7ld,%7ld)  d=(%ld,%ld)   pause=%08lX\n",
               DIRS[d].name, (long)bx, (long)bz, (long)pl->x, (long)pl->z,
               (long)(pl->x - bx), (long)(pl->z - bz), (unsigned long)g_re15_pauseflags);
    }
    /* Und einmal RENNEN wie ein Spieler, der die Richtungstaste mit CROSS haelt. */
    {
        int32_t bx = pl->x, bz = pl->z;
        for (int f = 0; f < 90; f++)
            frame((uint16_t)(RE15_PAD_BIT_UP | RE15_PAD_BIT_CROSS), 0);
        printf("   90x UP+X : (%7ld,%7ld) -> (%7ld,%7ld)  d=(%ld,%ld)\n",
               (long)bx, (long)bz, (long)pl->x, (long)pl->z,
               (long)(pl->x - bx), (long)(pl->z - bz));
    }
    printf("   GESAMT-DRIFT waehrend der Raetsel-Bedienung: (%ld,%ld)\n",
           (long)(pl->x - x0), (long)(pl->z - z0));
    pstate("raetsel", 0);

    printf("\n===== M4: Raetsel LOESEN (Win-Muster) -> sub18, Position nachher =====\n");
    {
        static const uint8_t pat[10] = { 1, 0, 1, 0, 1, 0, 1, 0, 1, 0 };
        for (int n = 0; n < 10; n++) re15_game_flag_set(5, (uint8_t)(0x0D + n), pat[n]);
    }
    for (int f = 0; f < 400; f++) {
        uint16_t edge = (g_scd.message_active && (f % 6) == 0) ? RE15_PAD_BIT_SQUARE : 0;
        frame(edge, edge);
        if (f % 50 == 0) pstate("sub18", f);
    }
    pstate("ENDE", 400);
    int32_t END_X = pl->x, END_Z = pl->z;
    printf("   DRIFT gegenueber dem Panel-Standpunkt: (%ld,%ld)\n",
           (long)(pl->x - x0), (long)(pl->z - z0));

    /* ================= M5: FLUCHTLAUF-SWEEP vom GEMESSENEN Standpunkt ==================
     * Der alte Sweep (probe_11f0_escape_sweep) startete bei (-1432,-15600) und bewegte den
     * Spieler mit einem NACHGEBAUTEN Walker. Hier wird von dem Punkt gestartet, an dem der
     * Spieler nach dem Raetsel WIRKLICH steht (M4), und ueber re15_game_step gelaufen —
     * also durch die echte Kommando-FSM, die echte Kollision und den echten AOT-Scan. */
    printf("\n===== M5: Fluchtlauf-Sweep ab dem GEMESSENEN Nach-Raetsel-Standpunkt "
           "(%ld,%ld) =====\n", (long)END_X, (long)END_Z);
    {
        const int NDIR = 32;
        struct { uint16_t bits; const char *n; } SPD[2] = {
            { (uint16_t)(RE15_PAD_BIT_UP | RE15_PAD_BIT_CROSS), "RENNEN" },
            { RE15_PAD_BIT_UP,                                  "GEHEN " },
        };
        int fails = 0, stucks = 0, runs = 0, switched = 0, nozone = 0;
        /* RVD-Tabelle fuer das Orakel (RDT+0x28), Records bis zum 0xFFFFFFFF-Terminator. */
        g_rvd_off = (uint32_t)(s_raw[0x28] | (s_raw[0x29]<<8) | (s_raw[0x2A]<<16) | (s_raw[0x2B]<<24));
        g_rvd_n = 0;
        for (uint32_t o = g_rvd_off; o + 20 <= s_rawsz; o += 20) {
            if (s_raw[o] == 0xFF && s_raw[o+1] == 0xFF && s_raw[o+2] == 0xFF && s_raw[o+3] == 0xFF) break;
            g_rvd_n++;
        }
        printf("   (Orakel: RVD @0x%04X, %d Records)\n", g_rvd_off, g_rvd_n);
        for (int sp = 0; sp < 2; sp++) {
            for (int d = 0; d < NDIR; d++) {
                int want = (d * 4096) / NDIR;
                room_boot_at(END_X, END_Z, 1024);
                if (open_puzzle() < 0) { printf("   !! sub16 oeffnete nicht (dir=%d)\n", want); continue; }
                solve_puzzle(400);
                /* Erst auf dem Fleck drehen (96/Bild, @0x80073ee5), dann loslaufen. */
                for (int t = 0; t < 60; t++) {
                    int err = (want - (int)(pl->rot_y & 0x0FFF) + 2048 + 4096) % 4096 - 2048;
                    if (err > 48)       frame(RE15_PAD_BIT_RIGHT, 0);
                    else if (err < -48) frame(RE15_PAD_BIT_LEFT, 0);
                    else break;
                }
                int cut0 = s_shown;
                int32_t bx = pl->x, bz = pl->z;
                int chg = 0;
                for (int t = 0; t < 240; t++) {
                    frame(SPD[sp].bits, 0);
                    if (s_shown != cut0) { chg = 1; break; }
                }
                runs++;
                int32_t mx = pl->x - bx, mz = pl->z - bz;
                int moved = ((long long)mx*mx + (long long)mz*mz > 800LL*800LL);
                if (chg) switched++;
                else if (moved) {
                    /* KEIN Wechsel — will das ORIGINAL an dieser Endstelle einen?
                     * (FUN_80014230 byte-exakt aus den RDT-Bytes, Etage 0 = DAT_800acad6) */
                    int want_cut = orig_rvd_scan(cut0, pl->x, pl->z, 0);
                    if (want_cut == cut0) want_cut = -1;
                    if (want_cut >= 0) {
                        fails++;
                        printf("   !! %s rot=%4d start_cut=%d KEIN Wechsel -> (%ld,%ld); "
                               "ORIGINAL wollte cut %d  ***PORTFEHLER***\n",
                               SPD[sp].n, want, cut0, (long)pl->x, (long)pl->z, want_cut);
                    } else {
                        nozone++;
                        printf("   -- %s rot=%4d start_cut=%d kein Wechsel -> (%ld,%ld); "
                               "ORIGINAL will dort AUCH keinen (Endstelle in keiner Zone)\n",
                               SPD[sp].n, want, cut0, (long)pl->x, (long)pl->z);
                    }
                } else stucks++;
            }
        }
        printf("   => %d/%d Laeufe mit Kamerawechsel | %d PORTFEHLER (Original wollte wechseln) "
               "| %d Endstellen ausserhalb jeder Zone (Original wechselt dort auch nicht) "
               "| %d ohne nennenswerte Bewegung\n", switched, runs, fails, nozone, stucks);
    }

    free(s_raw);
    return 0;
}
