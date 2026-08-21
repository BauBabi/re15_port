/* probe_11f0_escape_sweep.c — MESSSONDE (kein ctest, reine Diagnose)
 *
 * Nutzer-Befund, ZWEITE Meldung: "Nach Loesen des Generator-Raetsels und direktem
 * Weglaufen aendert sich die Kamera/das Bild TEILWEISE immer noch nicht korrekt."
 *
 * Gemessen wird eine TREFFERQUOTE ueber viele Fluchtlaeufe statt eines Einzelfalls:
 * fuer jeden erreichbaren Standpunkt vor dem Generator x 32 Richtungen x 2 Tempi.
 *
 * ORIGINAL-BELEG (selbst disassembliert, ghidra1_V2.txt):
 *   RVD-Zonen-Scan  FUN_80014230 @0x80014230 (Haupt-Loop @0x8001ccec, a0=0)
 *      @0x80014234 `lw a0,DAT_800ac794` (Gruppenzeiger)
 *      @0x8001423c `lbu v0,DAT_800afbb5` (ANGEFORDERTER Cut)
 *      @0x80014254 `lbu v1,0x16(a0)` / @0x8001425c `bne v1,v0` -> raus
 *      @0x8001426c Floor-Byte rec+1 (0xff oder == DAT_800acad6)
 *      @0x80014298 `jal FUN_80014368(&DAT_800aca88, rec)` = Punkt-in-Quad
 *      @0x800142a8 `lbu a0,1(s0)` (= rec+3 Ziel-Cut) / @0x800142ac `jal FUN_800142f4` / RETURN
 *   Cut_chg  LAB_800402a0: @0x800402c0 `lbu a1,DAT_800b0fe4` (ANGEZEIGTER Cut)
 *            @0x800402d4 `ori DAT_800aca3c,0x100` (Auto AUS)
 *            @0x800402e4 `sb a1,DAT_800b3f7b` @0x800402ec `sh a2,DAT_800b0fe8`
 *            @0x800402f4 `sb 1,DAT_800b5457` @0x800402fc `sh a0,DAT_800b0fe4`
 *            @0x80040300 `jal FUN_800142f4`
 *   Cut_old  LAB_8004032c: @0x8004033c `lbu a0,DAT_800b3f7b` ... @0x80040378-84 (Auto AN)
 *   Cut_auto LAB_800403ac (Flag 1 -> Bit 0x100 loeschen)
 *   Present  FUN_8002137c @0x800214f8/@0x80021500/@0x80021508/@0x80021514
 *
 * ROOM11F0-SKRIPT (aus der ausgelieferten RDT gelesen):
 *   sub16 @0x15A2  Raetsel OEFFNEN  (@0x15C0 Cut_chg(0x0A))
 *   sub17 @0x15FA  Raetsel ABBRECHEN (@0x16F0 Cut_old, @0x16F2 Cut_auto(1))
 *   sub18 @0x16F6  Raetsel GELOEST  (@0x173A Cut_chg(8), @0x1748 Cut_chg(13),
 *                  @0x1770 Cut_chg(14), @0x1776 Aot_reset(slot1,sce=1),
 *                  @0x1780 Cut_chg(8), @0x1782 Cut_auto(1))
 *   Der Spieler wird in KEINEM der drei versetzt.
 *
 * BEWEGUNGSMODELL = byte-true Walker FUN_800245d8 (actor_locomotion.c:343-344):
 *   x += (cos(rot_y)*speed)>>12 ; z -= (sin(rot_y)*speed)>>12 ; Gehen 75 / Rennen 200.
 * FRAME-REIHENFOLGE = die des Ports (main.c 3626 / 3951 / 4810 -> game_step_common.c:1189).
 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "re15_rdt.h"
#include "re15_scd.h"
#include "re15_actor.h"
#include "re15_aot.h"
#include "re15_room.h"
#include "re15_collision.h"
#include "re15_msg.h"
#include "re15_skeleton.h"     /* re15_sin_q12 / re15_cos_q12 */

extern scd_vm_t g_scd;
extern int scd_thread_start(int slot, const uint8_t *pc);

static re15_rdt_t  g_rdt;
static uint8_t    *g_raw   = NULL;
static size_t      g_rawsz = 0;
static int         g_shown = 0;          /* s_last_cut_idx aus main.c */
static int         g_trace = 0;

static uint8_t *read_file(const char *path, size_t *out_size)
{
    FILE *f = fopen(path, "rb");
    if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    if (sz <= 0) { fclose(f); return NULL; }
    uint8_t *buf = (uint8_t *)malloc((size_t)sz);
    if (!buf) { fclose(f); return NULL; }
    size_t rd = fread(buf, 1, (size_t)sz, f);
    fclose(f);
    if (rd != (size_t)sz) { free(buf); return NULL; }
    *out_size = (size_t)sz;
    return buf;
}

extern uint16_t g_scd_pad_edge;
extern uint16_t g_scd_pad_held;

/* EIN Bild in der Reihenfolge des Ports. `speed` 0 = stehen. */
static void frame(int16_t speed)
{
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    scd_vm_tick();                                              /* main.c:3626 */
    /* NACHRICHT WEGBESTAETIGEN wie ein Spieler — ohne das steht sub18 ewig an seinem
     * Message_on(2) @0x1742 (gemessen: Thread parkt auf 0x1747) und erreicht das
     * Cut_auto(1) @0x1782 NIE. Das Bestaetigungs-Bit ist virtuell 0x4000 = physisch
     * QUADRAT (Tabelle @0x80073dbc[14]); den FSM treibt re15_msg_tick (main.c:3751). */
    {
        static int msgphase = 0;
        g_scd_pad_edge = 0; g_scd_pad_held = 0;
        if (g_scd.message_active && ((msgphase++ % 6) == 0)) g_scd_pad_edge = 0x4000;
        const unsigned char *mraw = NULL; int mlen = 0, mid = 0;
        re15_msg_tick(&mraw, &mlen, &mid);
    }
    if (re15_cam_present_tick()) g_shown = (int)g_scd.cam_id;   /* main.c:3951 */
    if (speed) {                                                /* actor_locomotion.c:343-344 */
        int32_t c = re15_cos_q12(pl->rot_y & 0x0FFF);
        int32_t s = re15_sin_q12(pl->rot_y & 0x0FFF);
        int32_t ox = pl->x, oz = pl->z;
        int32_t nx = pl->x + (int32_t)((c * (int32_t)speed) >> 12);
        int32_t nz = pl->z - (int32_t)((s * (int32_t)speed) >> 12);
        re15_collision_constrain(&g_rdt, ox, oz, &nx, &nz);
        re15_collision_objects(&nx, &nz);
        pl->x = nx; pl->z = nz;
    }
    re15_aot_scan(pl->x, pl->z, (uint8_t)g_shown);              /* game_step_common.c:1189 */
}

static void trace_frames(const char *tag, int n)
{
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    int last = -999; long lastpc = -1;
    for (int f = 0; f < n; f++) {
        frame(0);
        int key = ((int)g_scd.cam_id << 16) | (g_shown << 8)
                | ((int)g_scd.cut_auto_enabled << 4) | (int)g_scd.cam_change_pending;
        long pc4 = g_scd.threads[4].active ? (long)(g_scd.threads[4].pc - g_raw) : -1;
        if (g_trace && (key != last || pc4 != lastpc)) {
            last = key; lastpc = pc4;
            printf("      [%s] f%-3d req=%-2u shown=%-2d wv0A=%-2d prev=%-2u pend=%u auto=%u "
                   "msg=%u thr4pc=0x%04lX pos=(%ld,%ld)\n",
                   tag, f, (unsigned)g_scd.cam_id, g_shown, (int)g_scd.work_vars[0x0A],
                   (unsigned)g_scd.cam_id_prev, (unsigned)g_scd.cam_change_pending,
                   (unsigned)g_scd.cut_auto_enabled, (unsigned)g_scd.message_active,
                   pc4, (long)pl->x, (long)pl->z);
        }
    }
}

static void room_reset(int32_t px, int32_t pz)
{
    re15_actor_init(); scd_vm_init(); re15_aot_init();
    re15_collision_reset_band();
    g_current_room_id = 0x11F0; g_room_change.pending = 0;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100;
    pl->x = px; pl->y = 0; pl->z = pz; pl->rot_y = 0;
    re15_collision_set_band(0);
    g_shown = 0;
    scd_room_reenter(&g_rdt, pl->x, pl->z, 0);
    for (int f = 0; f < 40; f++) frame(0);
}

/* Kollisions-constrainter Marsch mit ECHTER Laufgeschwindigkeit zu (tx,tz). */
static int march(int32_t tx, int32_t tz, int16_t speed, int max_frames)
{
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    int stuck = 0;
    for (int i = 0; i < max_frames; i++) {
        int32_t dx = tx - pl->x, dz = tz - pl->z;
        if ((long long)dx*dx + (long long)dz*dz < 300LL*300LL) return 1;
        double ang = atan2((double)(-dz), (double)dx) * 4096.0 / 6.283185307179586;
        pl->rot_y = (int16_t)((int)(ang + 0.5) & 0x0FFF);
        int32_t bx = pl->x, bz = pl->z;
        frame(speed);
        if (pl->x == bx && pl->z == bz) { if (++stuck > 3) return 0; } else stuck = 0;
    }
    return 0;
}

/* Ein kompletter Fluchtlauf: Raum neu, Standpunkt, Cut, Skript-sub, dann weglaufen.
 * Rueckgabe: 0 = Kamera wechselte, 1 = KEIN Wechsel trotz Bewegung, 2 = blockiert. */
static int escape_run(int32_t sx, int32_t sz, int start_cut,
                      int sub_open_off, int sub_close_off,
                      int16_t speed, int dir, int *out_start_cut, int32_t *ex, int32_t *ez)
{
    room_reset(sx, sz);
    g_scd.cam_id = (uint8_t)start_cut; g_scd.cam_change_pending = 1;
    for (int f = 0; f < 5; f++) frame(0);
    if (sub_open_off > 0) {
        scd_thread_start(3, g_raw + sub_open_off);
        for (int f = 0; f < 60; f++) frame(0);
    }
    if (sub_close_off > 0) {
        scd_thread_start(4, g_raw + sub_close_off);
        for (int f = 0; f < 220; f++) frame(0);
    } else {
        for (int f = 0; f < 220; f++) frame(0);
    }
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    int cut0 = g_shown;
    if (out_start_cut) *out_start_cut = cut0;
    int32_t bx = pl->x, bz = pl->z;
    pl->rot_y = (int16_t)dir;
    for (int f = 0; f < 240; f++) {
        frame(speed);
        if (g_shown != cut0) { if (ex) { *ex = pl->x; *ez = pl->z; } return 0; }
    }
    if (ex) { *ex = pl->x; *ez = pl->z; }
    int32_t mx = pl->x - bx, mz = pl->z - bz;
    return ((long long)mx*mx + (long long)mz*mz > 800LL*800LL) ? 1 : 2;
}

/* ===================================================================================
 * ORAKEL — FUN_80014230 / FUN_80014324 / FUN_80014368 byte-exakt aus den RDT-Bytes.
 * Record = 0x14 B: +0 ? +1 floor +2 cam_from +3 cam_to +4..0x13 = 4 Ecken (s16 x,z).
 * ================================================================================= */
static uint32_t g_rvd_off = 0;
static int      g_rvd_n   = 0;

static int16_t rs16(uint32_t off) { return (int16_t)(uint16_t)(g_raw[off] | (g_raw[off+1] << 8)); }

/* FUN_80014368 @0x80014368 — Punkt-in-Quad, exakt in der Original-Form. */
static int orig_point_in_quad(int32_t px, int32_t pz, uint32_t rec)
{
    int32_t x0 = rs16(rec + 4),  z0 = rs16(rec + 6);
    int32_t a4 = pz - z0;                        /* param_1[2] - z0 */
    int32_t a5 = rs16(rec + 8)  - x0;
    int32_t a1 = px - x0;
    int32_t a2 = rs16(rec + 10) - z0;
    int32_t a6 = rs16(rec + 0x10) - x0;
    int32_t a7 = rs16(rec + 0x12) - z0;
    if (a5 * a4 <= a2 * a1) {                    /* @0x800143a4-b0 */
        if (a6 * a4 < a7 * a1) return 0;         /* @0x800143b4-c4 */
        int32_t b3 = rs16(rec + 0x0e) - z0;
        int32_t b8 = rs16(rec + 0x0c) - x0;
        if (((a2 - b3) * (a1 - b8) <= (a5 - b8) * (a4 - b3)) &&
            ((a6 - b8) * (a4 - b3) <= (a7 - b3) * (a1 - b8)))
            return 1;
    }
    return 0;
}

/* FUN_80014324 @0x80014324 — erster Record mit +0x02 == cut (Gruppenzeiger). */
static long orig_group_ptr(int cut)
{
    for (int i = 0; i < g_rvd_n; i++) {
        uint32_t rec = g_rvd_off + (uint32_t)i * 0x14u;
        if (g_raw[rec + 2] == (uint8_t)cut) return (long)rec;
    }
    return -1;                                   /* im Original laeuft die Schleife weiter */
}

/* FUN_80014230 @0x80014230 — Rueckgabe = neuer Cut oder -1 (kein Wechsel). */
static int orig_rvd_scan(int cut, int32_t px, int32_t pz, int floor)
{
    long grp = orig_group_ptr(cut);
    if (grp < 0) return -1;
    uint32_t s1 = (uint32_t)grp + 0x14;          /* @0x80014260 s1 = a0+0x14 */
    uint32_t s0 = (uint32_t)grp + 0x16;          /* @0x80014268 s0 = a0+0x16 */
    if (g_raw[(uint32_t)grp + 0x16] != (uint8_t)cut) return -1;   /* @0x80014254-5c */
    for (;;) {
        uint8_t fl = g_raw[s0 - 1];              /* @0x8001426c rec+1 */
        if (fl == 0xFF || (int)fl == floor) {    /* @0x80014274 / @0x80014288 */
            if (orig_point_in_quad(px, pz, s1))  /* @0x80014298 */
                return g_raw[s0 + 1];            /* @0x800142a8 rec+3 */
        }
        s0 += 0x14; s1 += 0x14;                  /* @0x800142bc / @0x800142d4 */
        if (s0 + 0x14 > g_rawsz) return -1;
        if (g_raw[s0] != (uint8_t)cut) return -1;/* @0x800142c0-d0 */
    }
}

int main(int argc, char **argv)
{
    g_trace = (argc > 1 && strcmp(argv[1], "-v") == 0);
    const char *base = RE15_ASSET_PSX_DIR;
    char rp[600];
    snprintf(rp, sizeof rp, "%s/STAGE1/ROOM11F0.RDT", base);
    g_raw = read_file(rp, &g_rawsz);
    if (!g_raw) { printf("SKIP: %s fehlt\n", rp); return 77; }
    if (re15_rdt_parse(g_raw, g_rawsz, &g_rdt) < 0) { printf("FAIL: RDT-Parse\n"); return 1; }
    if (!(g_raw[0x15C0] == 0x29 && g_raw[0x15C1] == 0x0A)) {
        printf("FAIL: @0x15C0 != Cut_chg(0x0A)\n"); return 1; }
    if (!(g_raw[0x16F0] == 0x2A && g_raw[0x16F2] == 0x3C && g_raw[0x16F3] == 0x01)) {
        printf("FAIL: @0x16F0 != Cut_old+Cut_auto(1)\n"); return 1; }
    if (!(g_raw[0x1780] == 0x29 && g_raw[0x1781] == 0x08 &&
          g_raw[0x1782] == 0x3C && g_raw[0x1783] == 0x01)) {
        printf("FAIL: @0x1780 != Cut_chg(8)+Cut_auto(1)\n"); return 1; }

    /* ---------------- Phase 0: ORAKEL-Vergleich ueber den ganzen Raum ---------------- */
    g_rvd_off = (uint32_t)(g_raw[0x28] | (g_raw[0x29]<<8) | (g_raw[0x2A]<<16) | (g_raw[0x2B]<<24));
    g_rvd_n = 0;
    for (uint32_t o = g_rvd_off; o + 20 <= g_rawsz; o += 20) {
        if (g_raw[o] == 0xFF && g_raw[o+1] == 0xFF && g_raw[o+2] == 0xFF && g_raw[o+3] == 0xFF) break;
        g_rvd_n++;
    }
    printf("[P0] RVD @0x%04X, %d Records. ORAKEL (FUN_80014230) vs. Port (re15_aot_scan)\n",
           g_rvd_off, g_rvd_n);
    room_reset(250, 250);
    /* Nur die CAM_SWITCH-Slots stehen lassen — der Vergleich gilt dem Kamera-Scan;
     * andere AOT-Klassen wuerden feuern und den Scan vorzeitig beenden. */
    for (int i = 0; i < RE15_AOT_MAX; i++)
        if (g_aot.slots[i].type != RE15_AOT_TYPE_CAM_SWITCH) g_aot.slots[i].active = 0;
    {
        re15_actor_t *p0 = &g_actors[RE15_ACTOR_SLOT_PLAYER];
        long tested = 0, diff = 0;
        int  diff_by_cut[16]; memset(diff_by_cut, 0, sizeof diff_by_cut);
        int  shown_examples = 0;
        for (int cut = 0; cut < 15; cut++) {
            for (int32_t x = -4000; x <= 13500; x += 100) {
                for (int32_t z = -18000; z <= 6500; z += 100) {
                    int want = orig_rvd_scan(cut, x, z, 0);      /* floor 0 = DAT_800acad6 */
                    g_scd.cam_id             = (uint8_t)cut;
                    g_scd.cam_change_pending = 0;
                    g_scd.cut_auto_enabled   = 1;
                    p0->x = x; p0->z = z;
                    re15_aot_scan(x, z, (uint8_t)cut);
                    int got = ((int)g_scd.cam_id == cut) ? -1 : (int)g_scd.cam_id;
                    /* Das Original ruft FUN_800142f4 auch dann, wenn der Zielcut == Quellcut
                     * ist; fuer den Vergleich zaehlt nur "welcher Cut steht danach". */
                    if (want == cut) want = -1;
                    tested++;
                    if (want != got) {
                        diff++;
                        if (cut < 16) diff_by_cut[cut]++;
                        if (shown_examples < 20) {
                            shown_examples++;
                            printf("   !! cut=%2d (%6ld,%6ld): ORIGINAL -> %-3d, PORT -> %d\n",
                                   cut, (long)x, (long)z, want, got);
                        }
                    }
                }
            }
        }
        printf("[P0] %ld/%ld Gitterpunkte weichen ab\n", diff, tested);
        for (int c = 0; c < 15; c++)
            if (diff_by_cut[c]) printf("     cut %2d: %d Abweichungen\n", c, diff_by_cut[c]);
    }

    /* --- Phase 0b: TANK-SKRIPT validieren (dieselben Tokens wie RE15_INPUT_SCRIPT).
     * Der Port dreht mit LEFT/RIGHT (48/96 pro Bild) und laeuft mit UP(+CROSS).
     * Hier wird nur die reine Geometrie geprueft: Kurs setzen, N Bilder laufen. */
    {
        struct { int rot; int16_t spd; int frames; } SCRIPT[] = {
            { 1024, 200,  8 },   /* nach Norden anlaufen */
            { 1024, 200, 60 },
            { 1150, 200, 20 },
            { 1024, 200, 40 },
        };
        room_reset(250, 250);
        re15_actor_t *p = &g_actors[RE15_ACTOR_SLOT_PLAYER];
        printf("[P0b] Tank-Skript ab (250,250):\n");
        for (unsigned si = 0; si < sizeof SCRIPT / sizeof SCRIPT[0]; si++) {
            p->rot_y = (int16_t)SCRIPT[si].rot;
            for (int f = 0; f < SCRIPT[si].frames; f++) frame(SCRIPT[si].spd);
            printf("      Schritt %u rot=%4d -> (%6ld,%6ld) cut=%d\n",
                   si, SCRIPT[si].rot, (long)p->x, (long)p->z, g_shown);
        }
    }

    /* ---------------- Phase 1: echter Anmarsch bis vor den Generator ---------------- */
    room_reset(250, 250);
    printf("[P1] Spawn (250,250) cut=%u shown=%d auto=%d\n",
           (unsigned)g_scd.cam_id, g_shown, (int)g_scd.cut_auto_enabled);
    /* Wegpunkte aus dem in P0b validierten Tank-Lauf (gerade nach Norden, dann in die
     * Generator-Nische) — der frueher benutzte Direktmarsch blieb an der Nischenkante
     * bei z=-13982 haengen und hat den Standpunkt um ~2000 Einheiten verfehlt. */
    march(250,   -13350, 200, 400);
    march(-530,  -16032, 200, 400);
    march(-1800, -15600, 200, 400);
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    int32_t SX = pl->x, SZ = pl->z;
    int     SCUT = g_shown;
    printf("[P1] Standpunkt vor dem Generator: (%ld,%ld) cut=%d\n",
           (long)SX, (long)SZ, SCUT);

    /* Welche RVD-Zonen umschliessen diesen Punkt (unabhaengig vom cam_from-Gate)? */
    printf("[P1b] RVD-Zonen, die (%ld,%ld) enthalten:\n", (long)SX, (long)SZ);
    for (int i = 0; i < g_rdt.zone_count; i++) {
        const re15_rdt_zone_t *z = &g_rdt.zones[i];
        int anchor = (i == 0 || g_rdt.zones[i-1].cam_from != z->cam_from);
        if (SX >= z->cx - z->half_w && SX <= z->cx + z->half_w &&
            SZ >= z->cz - z->half_h && SZ <= z->cz + z->half_h)
            printf("      zone[%2d] %s from=%u to=%u\n", i, anchor ? "ANKER" : "     ",
                   (unsigned)z->cam_from, (unsigned)z->cam_to);
    }

    /* ---------------- Phase 2: Spur der beiden Skript-Wege ---------------- */
    printf("\n[P2] SPUR — Raetsel oeffnen (sub16) und LOESEN (sub18)\n");
    room_reset(SX, SZ);
    g_scd.cam_id = (uint8_t)SCUT; g_scd.cam_change_pending = 1;
    for (int f = 0; f < 5; f++) frame(0);
    printf("   Start: req=%u shown=%d wv0A=%d auto=%d\n", (unsigned)g_scd.cam_id, g_shown,
           (int)g_scd.work_vars[0x0A], (int)g_scd.cut_auto_enabled);
    scd_thread_start(3, g_raw + 0x15C0);
    trace_frames("sub16", 60);
    printf("   nach sub16: req=%u shown=%d wv0A=%d prev=%u auto=%d\n",
           (unsigned)g_scd.cam_id, g_shown, (int)g_scd.work_vars[0x0A],
           (unsigned)g_scd.cam_id_prev, (int)g_scd.cut_auto_enabled);
    scd_thread_start(4, g_raw + 0x16F6);   /* sub18 = GELOEST */
    trace_frames("sub18", 260);
    printf("   nach sub18: req=%u shown=%d wv0A=%d prev=%u auto=%d pos=(%ld,%ld)\n",
           (unsigned)g_scd.cam_id, g_shown, (int)g_scd.work_vars[0x0A],
           (unsigned)g_scd.cam_id_prev, (int)g_scd.cut_auto_enabled,
           (long)g_actors[RE15_ACTOR_SLOT_PLAYER].x,
           (long)g_actors[RE15_ACTOR_SLOT_PLAYER].z);

    /* ---------------- Phase 3: Fluchtlauf-Sweeps ---------------- */
    const int     NDIR   = 32;
    const int16_t SPD[2] = { 200, 75 };
    const char   *sname[2] = { "RENNEN", "GEHEN " };
    struct { const char *name; int open_off, close_off; } CASE[3] = {
        { "GELOEST   (sub16+sub18)", 0x15C0, 0x16F6 },
        { "ABGEBROCH (sub16+sub17)", 0x15C0, 0x15FA },
        { "KONTROLLE (kein Skript) ", 0,      0      },
    };
    for (int cs = 0; cs < 3; cs++) {
        int fails = 0, stucks = 0, runs = 0;
        printf("\n[P3] %s — Sweep ab (%ld,%ld) cut=%d\n",
               CASE[cs].name, (long)SX, (long)SZ, SCUT);
        for (int sp = 0; sp < 2; sp++) {
            for (int d = 0; d < NDIR; d++) {
                int sc = -1; int32_t ex = 0, ez = 0;
                int r = escape_run(SX, SZ, SCUT, CASE[cs].open_off, CASE[cs].close_off,
                                   SPD[sp], (d * 4096) / NDIR, &sc, &ex, &ez);
                runs++;
                if (r == 1) {
                    fails++;
                    printf("   !! %s dir=%4d start_cut=%d KEIN Wechsel -> (%ld,%ld)\n",
                           sname[sp], (d * 4096) / NDIR, sc, (long)ex, (long)ez);
                } else if (r == 2) stucks++;
            }
        }
        printf("   => %d/%d ohne Kamerawechsel trotz Bewegung (%d blockiert)\n",
               fails, runs, stucks);
    }

    free(g_raw);
    return 0;
}
