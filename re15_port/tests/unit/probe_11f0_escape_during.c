/* probe_11f0_escape_during.c — MESSSONDE (kein ctest, reine Diagnose).
 *
 * WARUM NEU: probe_11f0_escape_sweep.c laesst sub18 ERST KOMPLETT DURCHLAUFEN (220 Bilder)
 * und laeuft DANN weg -> 0/64 Ausfaelle. Der Nutzer laeuft aber "DIREKT" weg, also WAEHREND
 * sub18 noch laeuft. Genau in diesem Fenster ist die Auto-Kamera AUS:
 *   Cut_chg LAB_800402a0 @0x800402d4 `ori DAT_800aca3c,0x100`   = Auto-Scan AUS
 *   RVD-Gate @0x8001cce0 `andi v0,v0,0x100` / @0x8001cce4 `bne -> ueberspringen`
 *   Cut_auto(1) LAB_800403ac loescht das Bit wieder.
 * ROOM11F0-sub18 (SCD-Sub-Tabelle @0x0D34[18] -> Datei 0x16F6):
 *   0x173A Cut_chg(8) 0x1742 Message_on(2,0xffff) 0x1746 Evt_next
 *   0x1748 Cut_chg(13) 0x174A fade_set(Schritt 0) + 4x fade_start/Sleep (2/3/4/3)
 *   0x1770 Cut_chg(14) 0x1772 Sleep 0x28 0x1776 Aot_reset(1,sce=1) 0x1780 Cut_chg(8)
 *   0x1782 Cut_auto(1)  <- erst HIER geht der Zonen-Scan wieder an
 * Gemessen wird deshalb: Losrennen bei Verzoegerung k nach sub18-Start, 32 Richtungen,
 * 2 Tempi. Und fuer jeden Ausfall wird das ORAKEL (FUN_80014230 byte-exakt aus den
 * RDT-Bytes) gefragt, ob das ORIGINAL an derselben Endposition ebenfalls keine Zone
 * findet — nur dann ist "Kamera bleibt stehen" byte-true und KEIN Port-Fehler.
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


/* ===================================================================================
 * FLUCHTLAUF *WAEHREND* sub18: bei Verzoegerung `delay` Bilder nach Thread-Start
 * losrennen, dann `run` Bilder laufen. Rueckgabe der beobachteten Groessen.
 * ================================================================================= */
typedef struct {
    int  cut_at_move;      /* angezeigter Cut im Moment des Losrennens          */
    int  cut_end;          /* angezeigter Cut am Ende                            */
    int  changed_after;    /* hat sich der Cut NACH dem Losrennen je geaendert?  */
    int  auto_at_move;     /* cut_auto beim Losrennen                            */
    int  auto_end;
    int32_t ex, ez;
    long moved2;           /* quadrierte Strecke                                 */
    int  oracle_end;       /* was das ORIGINAL an (ex,ez) mit Gruppe req liefert  */
    int  req_end;
} during_res_t;

static void escape_during(int32_t sx, int32_t sz, int start_cut, int delay,
                          int16_t speed, int dir, int run_frames, during_res_t *o)
{
    room_reset(sx, sz);
    g_scd.cam_id = (uint8_t)start_cut; g_scd.cam_change_pending = 1;
    for (int f = 0; f < 5; f++) frame(0);
    scd_thread_start(3, g_raw + 0x15C0);      /* sub16 = Panel auf */
    for (int f = 0; f < 60; f++) frame(0);
    scd_thread_start(4, g_raw + 0x16F6);      /* sub18 = GELOEST */

    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    for (int f = 0; f < delay; f++) frame(0);  /* stehen bleiben bis `delay` */

    o->cut_at_move  = g_shown;
    o->auto_at_move = (int)g_scd.cut_auto_enabled;
    int32_t bx = pl->x, bz = pl->z;
    pl->rot_y = (int16_t)dir;
    o->changed_after = 0;
    for (int f = 0; f < run_frames; f++) {
        frame(speed);
        if (g_shown != o->cut_at_move) o->changed_after = 1;
    }
    o->cut_end  = g_shown;
    o->auto_end = (int)g_scd.cut_auto_enabled;
    o->ex = pl->x; o->ez = pl->z;
    int32_t mx = pl->x - bx, mz = pl->z - bz;
    o->moved2 = (long)((long long)mx*mx + (long long)mz*mz);
    o->req_end    = (int)g_scd.cam_id;
    o->oracle_end = orig_rvd_scan((int)g_scd.cam_id, pl->x, pl->z,
                                  re15_collision_debug_band());
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
    g_rvd_off = (uint32_t)(g_raw[0x28] | (g_raw[0x29]<<8) | (g_raw[0x2A]<<16) | (g_raw[0x2B]<<24));
    g_rvd_n = 0;
    for (uint32_t o = g_rvd_off; o + 20 <= g_rawsz; o += 20) {
        if (g_raw[o] == 0xFF && g_raw[o+1] == 0xFF && g_raw[o+2] == 0xFF && g_raw[o+3] == 0xFF) break;
        g_rvd_n++;
    }

    /* Standpunkt vor dem Generator (derselbe Anmarsch wie im Schwester-Probe). */
    room_reset(250, 250);
    march(250,   -13350, 200, 400);
    march(-530,  -16032, 200, 400);
    march(-1800, -15600, 200, 400);
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    int32_t SX = pl->x, SZ = pl->z; int SCUT = g_shown;
    printf("[D0] Standpunkt (%ld,%ld) cut=%d, RVD @0x%04X (%d Records)\n",
           (long)SX, (long)SZ, SCUT, g_rvd_off, g_rvd_n);

    /* D1: Zeitachse von sub18 — wann geht cut_auto wieder an? */
    {
        room_reset(SX, SZ);
        g_scd.cam_id = (uint8_t)SCUT; g_scd.cam_change_pending = 1;
        for (int f = 0; f < 5; f++) frame(0);
        scd_thread_start(3, g_raw + 0x15C0);
        for (int f = 0; f < 60; f++) frame(0);
        scd_thread_start(4, g_raw + 0x16F6);
        int on_at = -1, la = -1, ls = -1;
        printf("[D1] sub18-Zeitachse (nur Aenderungen):\n");
        for (int f = 0; f < 200; f++) {
            frame(0);
            if ((int)g_scd.cut_auto_enabled != la || g_shown != ls) {
                la = (int)g_scd.cut_auto_enabled; ls = g_shown;
                printf("     f%-3d req=%-2u shown=%-2d auto=%d msg=%u thr4pc=0x%04lX\n",
                       f, (unsigned)g_scd.cam_id, g_shown, la,
                       (unsigned)g_scd.message_active,
                       g_scd.threads[4].active ? (long)(g_scd.threads[4].pc - g_raw) : -1);
            }
            if (on_at < 0 && g_scd.cut_auto_enabled) on_at = f;
        }
        printf("[D1] cut_auto(1) erreicht bei Bild %d nach sub18-Start\n", on_at);
    }

    /* D2: Losrennen bei k = 0..150, 32 Richtungen, 2 Tempi. */
    const int NDIR = 32;
    const int16_t SPD[2] = { 200, 75 };
    const char *sname[2] = { "RENNEN", "GEHEN " };
    int total = 0, nochange = 0, oracle_agrees = 0, port_bug = 0;
    printf("\n[D2] Fluchtlauf WAEHREND sub18 (k = Bilder nach Thread-Start)\n");
    for (int k = 0; k <= 150; k += 5) {
        int kc = 0, ko = 0, kb = 0, kruns = 0;
        for (int sp = 0; sp < 2; sp++) {
            for (int d = 0; d < NDIR; d++) {
                during_res_t r; memset(&r, 0, sizeof r);
                escape_during(SX, SZ, SCUT, k, SPD[sp], (d * 4096) / NDIR, 240, &r);
                total++; kruns++;
                if (r.moved2 <= 800L*800L) continue;         /* blockiert, nicht gewertet */
                if (!r.changed_after) {
                    nochange++; kc++;
                    /* Sagt das ORIGINAL an derselben Endposition auch "keine Zone"? */
                    if (r.oracle_end < 0 || r.oracle_end == r.req_end) { oracle_agrees++; ko++; }
                    else {
                        port_bug++; kb++;
                        if (kb <= 3)
                            printf("   !! k=%-3d %s dir=%4d: PORT bleibt auf %d (req=%d), "
                                   "ORIGINAL wuerde auf %d schalten @(%ld,%ld) auto=%d\n",
                                   k, sname[sp], (d*4096)/NDIR, r.cut_end, r.req_end,
                                   r.oracle_end, (long)r.ex, (long)r.ez, r.auto_end);
                    }
                }
            }
        }
        printf("   k=%-3d  %2d/%2d ohne Wechsel  (Orakel bestaetigt %2d, PORT-FEHLER %2d)\n",
               k, kc, kruns, ko, kb);
    }
    printf("\n[D2] GESAMT %d Laeufe: %d ohne Kamerawechsel, davon %d byte-true (Orakel: auch "
           "das Original findet keine Zone), %d ECHTE PORT-FEHLER\n",
           total, nochange, oracle_agrees, port_bug);

    free(g_raw);
    return port_bug ? 1 : 0;
}
