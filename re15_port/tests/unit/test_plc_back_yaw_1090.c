/**
 * @file test_plc_back_yaw_1090.c
 * @brief PIN — Plc_dest-Modi 7/8 sind RUECKWAERTS-Modi (Blick 180 Grad gegen die Laufrichtung).
 *
 * NUTZER-BEFUND (ROOM1090, praezisiert): "Leon laeuft vor und dreht sich nach RECHTS um. Im
 * Original schaut er nach LINKS zum Transporter." Der WEG stimmte, die BLICKRICHTUNG am Ende
 * der Cutscene war falschherum.
 *
 * GEMESSEN VORHER (probe_1090_cutscene): Endstand (-76,-2001) yaw = 2203 — exakt die
 * LAUFRICHTUNG des letzten Wegpunkts. NACHHER: (-77,-1997) yaw = 111 ~ 2203 + 0x800.
 *
 * MECHANISMUS (selbst disassembliert, info/Re1.5/PSX.EXE):
 *   Der letzte Wegpunkt von sub02 @0x247C ist `Plc_dest(0, mode 8, dest=(-130,-1988))`.
 *   Plc_dest @0x80041be4 setzt nur +0x4=4 / +0x5=mode (@0x80041c14/18); den Yaw setzt der
 *   PRO-MODE-Handler aus der Spieler-Tabelle @0x80073e30:
 *     [4]=0x80030af0 WALK  [5]=0x80030d28 RUN  [7]=0x80031080  [8]=0x800311f0  [9]=0x80031360
 *   Handler Mode 8 @0x800311f0 (Mode 7 @0x80031080 ist identisch aufgebaut):
 *     80031250  jal 0x8001aac4              Yaw-Slew
 *     80031254  addiu a2,zero,-48           <<< RATE IST NEGATIV
 *     80031258  jal 0x800245d8              Translation
 *     8003125c  ori  a0,zero,0x800          <<< YAW-OFFSET 180 GRAD
 *   FUN_8001aac4 @0x8001aac4 wertet das Vorzeichen der Rate aus:
 *     8001aaf8  jal 0x8001a6d4              bearing = Peilung Aktor->Ziel
 *     8001ab08  bgez s0,0x8001ab1c          nur wenn rate < 0:
 *     8001ab10    subu s1,zero,v0             rate = -rate
 *     8001ab14    addiu v0,a0,2048            bearing += 0x800
 *     8001ab18    andi a0,v0,0xfff
 *   FUN_800245d8 @0x800245d8 addiert sein Argument auf den Yaw, bevor es dreht:
 *     80024658  lh   v0,106(entity)         (+0x6a Yaw)
 *     80024664  addu a0,v0,a0               RotMatrixY(yaw + a0)
 *   => Blick = bearing+0x800, Bewegung = Blick+0x800 = bearing. Der Aktor laeuft ZUM Ziel und
 *      schaut dabei DAVON WEG. Mode 4/5 rufen a0 = 0 (@0x80030c60 / @0x80030eb4) und eine
 *      POSITIVE Rate (0x30 @0x80030c58 / 0x48 @0x80030eac) — die sind vorwaerts.
 *
 * ZUSAETZLICH gepinnt: die Drehrichtungs-Entscheidung von FUN_8001aac4 ist NICHT der
 * symmetrische kuerzeste Weg —
 *     8001ab30-38  d = (bearing - yaw + rate) & 0xfff
 *     8001ab44/54  d <  2*rate  -> yaw = bearing        (SNAP)
 *     8001ab5c/60  d <= 0x800   -> yaw = yaw + rate     (positiv)
 *     8001ab58     sonst        -> yaw = yaw - rate     (negativ)
 * Positiv gedreht wird nur fuer eine Winkeldifferenz in [rate, 0x800-rate]; die letzten `rate`
 * Einheiten VOR exakt 180 Grad dreht das Original NEGATIV. Der alte Port-Clamp drehte dort
 * positiv — genau die Sorte Vorzeichenfehler, um die es hier geht.
 */
#include "re15_actor.h"
#include "re15_scd.h"
#include "re15_rdt.h"
#include "re15_aot.h"
#include "re15_room.h"
#include "re15_player.h"       /* RE15_PLAYER_MOTION_BACK_PL00 */
#include "re15_to_re2.h"       /* re15_to_re2_plc_dest_clip */
#include "re15_anim_select.h"  /* re15_actor_anim_select — die Bank-Aufloesung */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern scd_vm_t g_scd;
extern int  scd_thread_start(int slot, const uint8_t *pc);
extern int16_t re15_atan2_q12(int32_t dz, int32_t dx);
extern void re15_actors_anim_advance(void);
extern int  re15_player_event_reach_clip(void);   /* game_step_common.c — Sub 6 aktiv? */

static int s_fail = 0;

static void ok(int cond, const char *what, long got, long want)
{
    if (cond) { printf("  OK   %-58s got=%ld\n", what, got); }
    else      { printf("  FAIL %-58s got=%ld want=%ld\n", what, got, want); s_fail++; }
}

/* Signierte Kreis-Differenz in (-2048,+2048]. */
static int ydiff(int a, int b)
{
    int d = (b - a) & 0x0FFF;
    if (d > 2048) d -= 4096;
    return d;
}

static int iabs_(int v) { return v < 0 ? -v : v; }

/* Peilung Aktor->Ziel in der Port-Konvention (identisch zu der im Walker). */
static int bearing_to(int32_t px, int32_t pz, int32_t tx, int32_t tz)
{
    return (int)((re15_atan2_q12(tz - pz, tx - px) - 1024) & 0x0FFF);
}

static void reset_actor(re15_actor_t *a)
{
    memset(a, 0, sizeof *a);
    a->active = 1; a->type = 0; a->hp = 100;
}

/* Einen Plc_dest-Walk komplett durchfahren. Liefert die Tickzahl bis zur Ankunft. */
static int run_walk(re15_actor_t *a, uint8_t mode, int32_t dx, int32_t dz,
                    int16_t start_yaw, int32_t sx, int32_t sz, int max_ticks)
{
    a->x = sx; a->z = sz; a->rot_y = start_yaw;
    a->walk_dest_x = (int16_t)dx; a->walk_dest_z = (int16_t)dz;
    a->walk_mode = mode; a->walk_flag_bit = 0x20;
    a->walk_active = 1; a->walk_fsm = 0;
    int t = 0;
    while (a->walk_active && t < max_ticks) { re15_actor_step_walk(a); t++; }
    return t;
}

int main(void)
{
    re15_actor_init(); scd_vm_init(); re15_aot_init();
    re15_actor_t *a = &g_actors[RE15_ACTOR_SLOT_PLAYER];

    /* ===================================================================================
     * R1 — MECHANISMUS: Mode 8 = rueckwaerts. Startet man ihn BEREITS rueckwaerts
     * ausgerichtet, laeuft er eine gerade Linie: die Peilung bleibt konstant, also MUSS der
     * Endyaw exakt bearing+0x800 sein (@0x8001ab14) und die Strecke trotzdem zum Ziel fuehren
     * (@0x8003125c). ================================================================== */
    printf("R1 Mode 8 rueckwaerts (gerade Linie, Startyaw = bearing+0x800)\n");
    {
        const int32_t sx = 3000, sz = -600, tx = -1200, tz = 900;
        int bear = bearing_to(sx, sz, tx, tz);
        reset_actor(a);
        int ticks = run_walk(a, 0x08, tx, tz, (int16_t)((bear + 0x800) & 0xFFF), sx, sz, 400);
        int32_t ddx = tx - a->x, ddz = tz - a->z;
        ok(a->walk_active == 0, "Mode 8 kommt an", ticks, 0);
        ok(ddx * ddx + ddz * ddz < 100 * 100, "Mode 8 Endabstand < 100",
           (long)(ddx * ddx + ddz * ddz), 9999);
        /* Toleranz 0x20: die Schrittrichtung ist quantisiert (re15_cos/sin_q12), der Aktor
         * driftet um wenige Einheiten von der idealen Linie ab, also wandert die Peilung
         * waehrend des Laufs minimal mit. Die exakte Ein-Tick-Rechnung pinnt R2. */
        ok(iabs_(ydiff(a->rot_y, (bear + 0x800) & 0xFFF)) <= 0x20,
           "Mode 8 Endyaw == bearing+0x800 (@0x8001ab14)",
           iabs_(ydiff(a->rot_y, (bear + 0x800) & 0xFFF)), 0x20);
        ok(iabs_(ydiff(a->rot_y, bear)) > 0x600,
           "NEGATIV: Endyaw ist NICHT die Laufrichtung", iabs_(ydiff(a->rot_y, bear)), 0x600);
        printf("       bearing=%d  endyaw=%d  ticks=%d  end=(%ld,%ld)\n",
               bear, a->rot_y, ticks, (long)a->x, (long)a->z);
    }

    /* R1b — POSITIV-KONTROLLE: derselbe Weg als Mode 5 (RUN) haelt den Yaw AUF der
     * Laufrichtung (a0 = 0 @0x80030eb4, Rate positiv 0x48 @0x80030eac). */
    printf("R1b Positiv-Kontrolle Mode 5 (vorwaerts)\n");
    {
        const int32_t sx = 3000, sz = -600, tx = -1200, tz = 900;
        int bear = bearing_to(sx, sz, tx, tz);
        reset_actor(a);
        int ticks = run_walk(a, 0x05, tx, tz, (int16_t)bear, sx, sz, 400);
        ok(a->walk_active == 0, "Mode 5 kommt an", ticks, 0);
        ok(a->rot_y == bear, "Mode 5 Endyaw == bearing (unveraendert vorwaerts)",
           a->rot_y, bear);
    }

    /* R1c — Mode 7 verhaelt sich wie Mode 8 (@0x800310e4 rate -48, @0x800310ec a0=0x800). */
    printf("R1c Mode 7 ist ebenfalls rueckwaerts\n");
    {
        const int32_t sx = -2500, sz = 2200, tx = 700, tz = -1400;
        int bear = bearing_to(sx, sz, tx, tz);
        reset_actor(a);
        run_walk(a, 0x07, tx, tz, (int16_t)((bear + 0x800) & 0xFFF), sx, sz, 400);
        ok(iabs_(ydiff(a->rot_y, (bear + 0x800) & 0xFFF)) <= 0x20,
           "Mode 7 Endyaw == bearing+0x800",
           iabs_(ydiff(a->rot_y, (bear + 0x800) & 0xFFF)), 0x20);
    }

    /* R1d — DER FEHLERFALL: startet Mode 8 mit dem Blick IN die Laufrichtung (= was der Port
     * vorher als Endzustand hatte), muss er sich UMDREHEN und trotzdem ans Ziel laufen. */
    printf("R1d Mode 8 dreht sich aus der Laufrichtung heraus um\n");
    {
        const int32_t sx = 3000, sz = -600, tx = -1200, tz = 900;
        int bear = bearing_to(sx, sz, tx, tz);
        reset_actor(a);
        int ticks = run_walk(a, 0x08, tx, tz, (int16_t)bear, sx, sz, 400);
        int32_t ddx = tx - a->x, ddz = tz - a->z;
        ok(ddx * ddx + ddz * ddz < 100 * 100, "erreicht das Ziel trotzdem",
           (long)(ddx * ddx + ddz * ddz), 9999);
        int off = iabs_(ydiff(a->rot_y, bear));
        ok(off > 0x600, "Endyaw >135 Grad von der Laufrichtung entfernt", off, 0x600);
        printf("       start yaw=%d -> end yaw=%d (bearing %d), ticks=%d\n",
               bear, a->rot_y, bear, ticks);
    }

    /* ===================================================================================
     * R2 — DREHRICHTUNGS-BAND von FUN_8001aac4 (@0x8001ab44-0x8001ab80), gemessen ueber
     * Mode 9 (Drehen auf der Stelle, Rate 0x60 @0x80031440, kein 0x800245d8 -> keine
     * Positionsaenderung, ein Tick = eine reine Yaw-Rechnung). =========================== */
    printf("R2 Drehrichtung/SNAP-Fenster (rate 0x60)\n");
    {
        const int32_t sx = 0, sz = 0, tx = 2600, tz = -1900;
        const int rate = 0x60;
        int bear = bearing_to(sx, sz, tx, tz);
        struct { int delta_u; int want_step; const char *what; } cases[] = {
            /* d = (delta_u + rate) & 0xfff */
            { 0x800,          -rate, "delta 0x800 (exakt 180) -> NEGATIV (@0x8001ab58)"      },
            { 0x800 - rate,   +rate, "delta 0x800-rate        -> POSITIV (@0x8001ab7c)"      },
            { 0x800 - rate+1, -rate, "delta 0x800-rate+1      -> NEGATIV (Band, @0x8001ab58)"},
            { 0x400,          +rate, "delta 0x400             -> POSITIV"                    },
            { 0xC00,          -rate, "delta 0xC00             -> NEGATIV"                    },
        };
        for (unsigned i = 0; i < sizeof cases / sizeof cases[0]; i++) {
            reset_actor(a);
            a->x = sx; a->z = sz;
            a->rot_y = (int16_t)((bear - cases[i].delta_u) & 0x0FFF);
            int16_t before = a->rot_y;
            a->walk_dest_x = (int16_t)tx; a->walk_dest_z = (int16_t)tz;
            a->walk_mode = 0x09; a->walk_flag_bit = 0x20;
            a->walk_active = 1; a->walk_fsm = 2;   /* direkt in den Slew-Body */
            re15_actor_step_walk(a);
            ok(ydiff(before, a->rot_y) == cases[i].want_step, cases[i].what,
               ydiff(before, a->rot_y), cases[i].want_step);
            ok(a->x == sx && a->z == sz, "  Mode 9 bewegt sich nicht (kein 0x800245d8)",
               (long)a->x, (long)sx);
        }
        /* SNAP-Fenster: |delta| < rate -> yaw = bearing (@0x8001ab54). */
        int snaps[] = { rate - 1, -(rate - 1) };
        for (unsigned i = 0; i < 2; i++) {
            reset_actor(a);
            a->rot_y = (int16_t)((bear - snaps[i]) & 0x0FFF);
            a->walk_dest_x = (int16_t)tx; a->walk_dest_z = (int16_t)tz;
            a->walk_mode = 0x09; a->walk_flag_bit = 0x20;
            a->walk_active = 1; a->walk_fsm = 2;
            re15_actor_step_walk(a);
            ok(a->rot_y == bear, "SNAP innerhalb rate -> yaw = bearing", a->rot_y, bear);
        }
    }

    /* ===================================================================================
     * R3 — DER ECHTE ABLAUF: ROOM1090 sub02 als SCD-Thread. Regressionswache fuer Teleport,
     * Wegpunkte und Endposition + der eigentliche Pin auf die Blickrichtung. ============= */
    printf("R3 ROOM1090 sub02 (echter SCD-Thread)\n");
    {
        char rp[600];
        snprintf(rp, sizeof rp, "%s/STAGE1/ROOM1090.RDT", RE15_ASSET_PSX_DIR);
        FILE *f = fopen(rp, "rb");
        if (!f) { printf("  SKIP: %s fehlt\n", rp); return s_fail ? 1 : 77; }
        fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
        uint8_t *raw = (uint8_t *)malloc((size_t)sz);
        if (!raw || fread(raw, 1, (size_t)sz, f) != (size_t)sz) { fclose(f); printf("  FAIL: read\n"); return 1; }
        fclose(f);
        re15_rdt_t rdt;
        if (re15_rdt_parse(raw, (size_t)sz, &rdt) < 0) { printf("  FAIL: RDT-Parse\n"); return 1; }

        /* Wegpunkte roh — Regressionswache gegen ein Verrutschen der Offsets. */
        ok(raw[0x247C] == 0x40 && raw[0x247C + 2] == 0x08,
           "sub02 letzter Wegpunkt @0x247C ist Plc_dest mode 8", raw[0x247C + 2], 8);
        int16_t wp_x = (int16_t)(raw[0x247C + 4] | (raw[0x247C + 5] << 8));
        int16_t wp_z = (int16_t)(raw[0x247C + 6] | (raw[0x247C + 7] << 8));
        ok(wp_x == -130 && wp_z == -1988, "Wegpunkt @0x247C dest == (-130,-1988)", wp_x, -130);
        ok(raw[0x245E + 2] == 0x05, "Wegpunkt @0x245E ist mode 5 (RUN)", raw[0x245E + 2], 5);

        re15_actor_init(); scd_vm_init(); re15_aot_init();
        g_current_room_id = 0x1090; g_room_change.pending = 0;
        re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
        pl->active = 1; pl->type = 0; pl->hp = 100;
        pl->x = -10100; pl->y = -1800; pl->z = 4200; pl->rot_y = 3072;
        scd_room_reenter(&rdt, pl->x, pl->z, 0);
        for (int fr = 0; fr < 30; fr++) { scd_vm_tick(); re15_aot_scan(pl->x, pl->z, (uint8_t)g_scd.cam_id); }

        scd_thread_start(3, rdt.sub_scd[2]);
        int32_t tp_x = 0, tp_z = 0; int16_t tp_yaw = 0; int have_tp = 0;
        int32_t leg8_x = 0, leg8_z = 0; int have_leg8 = 0;
        for (int fr = 0; fr < 400; fr++) {
            scd_vm_tick();
            re15_actors_anim_advance();
            re15_actor_step_all_walkers();
            re15_aot_scan(pl->x, pl->z, (uint8_t)g_scd.cam_id);
            if (!have_tp && fr == 0) { tp_x = pl->x; tp_z = pl->z; tp_yaw = pl->rot_y; have_tp = 1; }
            if (!have_leg8 && pl->walk_active && pl->walk_mode == 0x08) {
                leg8_x = pl->x; leg8_z = pl->z; have_leg8 = 1;
            }
        }
        ok(tp_x == -3011 && tp_z == 4397 && tp_yaw == 4546,
           "Teleport-Startpose (-3011,4397) yaw 4546 unveraendert", (long)tp_x, -3011);
        ok(have_leg8, "der Mode-8-Abschnitt laeuft ueberhaupt", have_leg8, 1);

        int32_t ddx = -130 - pl->x, ddz = -1988 - pl->z;
        ok(ddx * ddx + ddz * ddz < 300 * 300, "Endposition am letzten Wegpunkt",
           (long)(ddx * ddx + ddz * ddz), 89999);

        /* Blickrichtung: der Aktor muss ENTGEGEN der zurueckgelegten Strecke des
         * Mode-8-Abschnitts schauen (@0x8001ab14 + @0x8003125c). */
        int travel = bearing_to(leg8_x, leg8_z, pl->x, pl->z);
        int off = iabs_(ydiff(pl->rot_y, travel));
        ok(off > 0x600, "Endblick >135 Grad gegen die Laufrichtung des Mode-8-Abschnitts",
           off, 0x600);
        ok(iabs_(ydiff(pl->rot_y, (travel + 0x800) & 0xFFF)) < 0x200,
           "Endblick liegt bei Laufrichtung+0x800",
           iabs_(ydiff(pl->rot_y, (travel + 0x800) & 0xFFF)), 0x200);
        ok(pl->rot_y != 2203, "NEGATIV: nicht mehr der alte Vorwaerts-Yaw 2203", pl->rot_y, 2203);

        /* Regressions-Pin auf den gemessenen Ist-Wert des byte-true Walkers (Messung
         * 2026-08-21 nach dem Fix; vorher 2203 bei (-76,-2001)). */
        ok(pl->rot_y == 111, "Regressions-Pin Endyaw == 111", pl->rot_y, 111);
        ok(pl->x == -77 && pl->z == -1997, "Regressions-Pin Endposition == (-77,-1997)",
           (long)pl->x, -77);
        printf("       Mode-8-Start (%ld,%ld) -> Ende (%ld,%ld) yaw=%d, Laufrichtung=%d\n",
               (long)leg8_x, (long)leg8_z, (long)pl->x, (long)pl->z, pl->rot_y, travel);

        /* ---- BANK-PIN: die Modi 7/8 posieren aus der COMMON-Bank, nicht aus der Waffenbank --
         * Nutzer-Befund 2026-08-23: "zum Schluss rennt Leon komisch, fast auf der Stelle."
         * Der Clip-INDEX 0 war richtig (`sb zero,=>DAT_800acae8` @0x800310bc / @0x8003122c),
         * die BANK nicht: die beiden Handler laden
         *   @0x80031134/3c und @0x800312a4/ac  `lw 0x800acad8` + `lw 0x800acbc0` = PL00-Paar
         *   (Schreiber @0x80031578 / @0x8003154c = PLD-Directory),
         * waehrend die Modi 4/5/9 @0x80030bec/f4, @0x80030ec0/c8, @0x80031488/90 das PLW-Paar
         * 0x800acbc4/0x800acbc8 laden (Schreiber @0x80036c04 / @0x80036be4).
         * Der Port bildete 7/8 auf den RUN-Sentinel 100 = PL00W01 Clip 0 ab: 22 Bilder statt
         * der 34 von PL00.EDD Clip 0 -> bei +0x8c = 70 (@0x800310a8 / @0x80031218) takteten die
         * Beine 34/22 = 1,55x zu schnell fuer die Bodengeschwindigkeit. */
        ok(re15_to_re2_plc_dest_clip(0x07, 1) == RE15_PLAYER_MOTION_BACK_PL00,
           "Mode 7 -> COMMON-Bank-Sentinel (PL00 Clip 0)",
           re15_to_re2_plc_dest_clip(0x07, 1), RE15_PLAYER_MOTION_BACK_PL00);
        ok(re15_to_re2_plc_dest_clip(0x08, 1) == RE15_PLAYER_MOTION_BACK_PL00,
           "Mode 8 -> COMMON-Bank-Sentinel (PL00 Clip 0)",
           re15_to_re2_plc_dest_clip(0x08, 1), RE15_PLAYER_MOTION_BACK_PL00);
        ok(re15_to_re2_plc_dest_clip(0x08, 0) == RE15_PLAYER_MOTION_BACK_PL00,
           "Mode 8 ist RBJ-unabhaengig (Handler laedt die Bank unbedingt)",
           re15_to_re2_plc_dest_clip(0x08, 0), RE15_PLAYER_MOTION_BACK_PL00);
        /* NEGATIV: die Modi 4/5/9 muessen die W01-Sentinels behalten (105 = Clip 5, 100 = Clip 0). */
        ok(re15_to_re2_plc_dest_clip(0x04, 1) == 105, "NEGATIV: Mode 4 bleibt W01 Clip 5",
           re15_to_re2_plc_dest_clip(0x04, 1), 105);
        ok(re15_to_re2_plc_dest_clip(0x05, 1) == 100, "NEGATIV: Mode 5 bleibt W01 Clip 0",
           re15_to_re2_plc_dest_clip(0x05, 1), 100);
        ok(re15_to_re2_plc_dest_clip(0x09, 1) == 105, "NEGATIV: Mode 9 bleibt W01 Clip 5",
           re15_to_re2_plc_dest_clip(0x09, 1), 105);
        /* Und der Sentinel MUSS in anim_select auf (PL00, Clip 0) aufloesen — sonst faellt er
         * als direkter Clipindex 236 % clip_count in eine fremde Bank. */
        {
            re15_anim_banks_t bk; memset(&bk, 0, sizeof bk);
            re15_emd_skeleton_t  sk_pl00, sk_w01, sk_def;
            re15_emd_animation_t an_pl00, an_w01, an_def;
            memset(&sk_pl00,0,sizeof sk_pl00); memset(&sk_w01,0,sizeof sk_w01);
            memset(&sk_def,0,sizeof sk_def);
            memset(&an_pl00,0,sizeof an_pl00); memset(&an_w01,0,sizeof an_w01);
            memset(&an_def,0,sizeof an_def);
            bk.def_skel = &sk_def; bk.def_anim = &an_def;
            bk.pl00_skel = &sk_pl00; bk.pl00_anim = &an_pl00; bk.pl00_ok = 1;
            bk.w01_skel  = &sk_w01;  bk.w01_anim  = &an_w01;  bk.w01_ok  = 1;
            re15_anim_view_t av; memset(&av, 0, sizeof av);
            re15_actor_t probe; memset(&probe, 0, sizeof probe);
            probe.motion = RE15_PLAYER_MOTION_BACK_PL00;
            re15_actor_anim_select(&probe, 1, &bk, &av);
            ok(av.anim == &an_pl00 && av.skel == &sk_pl00,
               "anim_select: Sentinel 236 -> PL00-Bank", (long)(av.anim == &an_pl00), 1);
            ok(av.clip_override == 0, "anim_select: Sentinel 236 -> Clip 0",
               av.clip_override, 0);
        }

        /* ===============================================================================
         * R4 — SCHRITTZAEHLUNG. Nutzer-Befund 2026-08-23: "Im Original macht er zum
         * Schluss noch EINEN Schritt zurueck, bei uns ZWEI."
         *
         * MECHANISMUS (selbst disassembliert, info/Re1.5/PSX.EXE): der Ankunfts-Zweig JEDES
         * Spieler-Walk-Handlers schreibt den SUB-INDEX auf 6 und die Phase auf 0 —
         *   Mode 4 @0x80030cf0 `sb 6,-13735(at)=>0x800aca59` + @0x80030cfc `sb zero,0(s0)`
         *   Mode 5 @0x80030f44 + @0x80030f50   Mode 7 @0x800311a8 + @0x800311b0
         *   Mode 8 @0x80031318 + @0x80031320   Mode 9 @0x800313ec + @0x800313f4
         * und der Plc-Executor dispatcht 0x800aca59 jeden Tick neu (@0x8003068c `lbu` /
         * @0x8003069c `addiu at,at,15920` = 0x80073e30 / @0x800306a4 `lw` / @0x800306ac
         * `jalr`). 0x80073e30[6] = 0x800517f0 = EVENT-REACH: +0x94=1 (@0x80051854),
         * +0x95=0 (@0x80051864), +0x8f=7 (@0x80051874), Clip 1 einmal -> Clip 2 als Loop
         * (@0x800518c8/@0x800518dc). Der Walk-Clip wird danach NIE wieder advanct — sein
         * einziger Advancer ist das `jal 0x8001f314` im Handler-Body (Mode 8 @0x800312b0).
         * Der Rueckfall `0x800acc18 & 4` (@0x8003133c-48 usw.) ist tot: DAT_800acc18 hat
         * genau zwei Schreiber, `xori 0x20`+`sh` @0x800306c4/c8 und `sh zero` @0x8003197c.
         *
         * GEMESSEN (echter Lauf ROOM1090 sub02, RE15_MOTRACE):
         *   VORHER  F589 mo=236 af=0 ... F613 mo=236 af=24 (Endposition schon ab F600)
         *   NACHHER F589 mo=236 af=0 ... F597 mo=236 af=8, F598 mo=1, F613 mo=2
         * PL00.EDD Clip 0 setzt den Fuss bei Frame 8 und 25 auf (Wort-Bit 0x4000, gelesen
         * @0x80031274; Seite ueber Bit 0x1000 @0x8003127c) => vorher 2 Beinzyklen, jetzt 1.
         * ================================================================== */
        printf("R4 Ankunft uebergibt an Sub 6 (Event-Reach) — Schrittzahl\n");
        {
            /* (a) DATEN-PIN: die Fussaufsetzer der drei beteiligten Clips, roh aus den EDDs. */
            struct { const char *file; int clip; } cs[3] = {
                { "PLD/PL00.EDD", 0 }, { "PLD/PL00W01.EDD", 1 }, { "PLD/PL00W01.EDD", 2 }
            };
            int fc_[3] = {0,0,0}, nfoot[3] = {0,0,0}, f_first[3] = {-1,-1,-1}, f_second[3] = {-1,-1,-1};
            for (int i = 0; i < 3; i++) {
                char ep[600]; snprintf(ep, sizeof ep, "%s/%s", RE15_ASSET_PSX_DIR, cs[i].file);
                FILE *ef = fopen(ep, "rb"); if (!ef) continue;
                static unsigned char eb[65536];
                size_t en = fread(eb, 1, sizeof eb, ef); fclose(ef);
                unsigned ent = (unsigned)cs[i].clip * 4u;
                if (ent + 4 > en) continue;
                int cfc = eb[ent] | (eb[ent+1] << 8);
                int coff = eb[ent+2] | (eb[ent+3] << 8);
                fc_[i] = cfc;
                for (int f = 0; f < cfc && (size_t)(coff + f*4 + 4) <= en; f++) {
                    unsigned w = (unsigned)(eb[coff+f*4] | (eb[coff+f*4+1] << 8));
                    if (w & 0x4000u) {
                        if (f_first[i] < 0) f_first[i] = f; else if (f_second[i] < 0) f_second[i] = f;
                        nfoot[i]++;
                    }
                }
            }
            ok(fc_[0] == 34 && nfoot[0] == 2 && f_first[0] == 8 && f_second[0] == 25,
               "PL00.EDD Clip 0: 34 Bilder, Fuss bei 8 und 25 (Bit 0x4000 @0x80031274)",
               nfoot[0], 2);
            ok(fc_[1] == 16 && nfoot[1] == 0,
               "PL00W01 Clip 1 (Sub-6 Phase 0, @0x80051854): 16 Bilder, KEIN Fussaufsetzer",
               nfoot[1], 0);
            ok(fc_[2] == 52 && nfoot[2] == 0,
               "PL00W01 Clip 2 (Sub-6 Idle-Loop, @0x800518c8): 52 Bilder, KEIN Fussaufsetzer",
               nfoot[2], 0);

            /* (b) ABLAUF: sub02 erneut fahren und den Mode-8-Abschnitt vermessen. */
            re15_actor_init(); scd_vm_init(); re15_aot_init();
            g_current_room_id = 0x1090; g_room_change.pending = 0;
            re15_actor_t *p2 = &g_actors[RE15_ACTOR_SLOT_PLAYER];
            p2->active = 1; p2->type = 0; p2->hp = 100;
            p2->x = -10100; p2->y = -1800; p2->z = 4200; p2->rot_y = 3072;
            scd_room_reenter(&rdt, p2->x, p2->z, 0);
            for (int fr = 0; fr < 30; fr++) { scd_vm_tick(); re15_aot_scan(p2->x, p2->z, (uint8_t)g_scd.cam_id); }
            scd_thread_start(3, rdt.sub_scd[2]);

            int in8 = 0, ticks8 = 0, af_max8 = -1, mo_after = -1, af_after_max = -1;
            int steps = 0, last_slot = -1, post = 0;
            for (int fr = 0; fr < 500; fr++) {
                /* Reihenfolge wie im echten Loop (platform/pc/main.c): scd_vm_tick ->
                 * re15_actor_step_all_walkers (@main.c:3708) -> re15_game_step, in dem
                 * re15_player_tick den Spieler-Frame advanct. */
                scd_vm_tick();
                re15_actors_anim_advance();
                re15_actor_step_all_walkers();
                if (re15_player_event_reach_clip() >= 0) {
                    /* Sub 6 besitzt +0x95 (player_common.c) — hier kein Advance. */
                } else if (p2->motion_init_delay > 0) p2->motion_init_delay--;
                else { p2->anim_frame++; if (p2->anim_frac > 0) p2->anim_frac--; }
                re15_aot_scan(p2->x, p2->z, (uint8_t)g_scd.cam_id);

                int on_back = (p2->motion == RE15_PLAYER_MOTION_BACK_PL00);
                if (p2->walk_active && p2->walk_mode == 0x08) {
                    in8 = 1; ticks8++;
                    if ((int)p2->anim_frame > af_max8) af_max8 = (int)p2->anim_frame;
                }
                /* Schrittzaehler: jeder NEU erreichte Fussaufsetzer-Slot von PL00 Clip 0. */
                if (on_back && fc_[0] > 0) {
                    int slot = (int)(p2->anim_frame % (unsigned)fc_[0]);
                    if (slot != last_slot && (slot == f_first[0] || slot == f_second[0])) steps++;
                    last_slot = slot;
                }
                if (in8 && !p2->walk_active) {          /* nach der Ankunft */
                    if (mo_after < 0) mo_after = (int)p2->motion;
                    if (on_back && (int)p2->anim_frame > af_after_max)
                        af_after_max = (int)p2->anim_frame;
                    if (++post >= 60) break;
                }
            }
            printf("       Mode-8: %d Ticks, af_max=%d; nach Ankunft motion=%d, "
                   "Rueckwaerts-Clip noch %d Bilder, SCHRITTE=%d\n",
                   ticks8, af_max8, mo_after, af_after_max < 0 ? 0 : af_after_max + 1, steps);
            ok(in8, "Mode-8-Abschnitt gefahren", in8, 1);
            ok(mo_after != RE15_PLAYER_MOTION_BACK_PL00,
               "NEGATIV: nach der Ankunft laeuft NICHT weiter der Rueckwaerts-Clip 236",
               mo_after, RE15_PLAYER_MOTION_BACK_PL00);
            ok(mo_after == 1,
               "Ankunft -> Sub 6 Phase 0: +0x94 = 1 (@0x80051854)", mo_after, 1);
            ok(af_after_max < 0,
               "kein einziges Bild Rueckwaerts-Clip nach der Ankunft", af_after_max, -1);
            ok(steps == 1, "GENAU EIN Schritt im Mode-8-Abschnitt (Fuss bei Frame 8)",
               steps, 1);
            ok(af_max8 <= f_second[0],
               "der Lauf erreicht den ZWEITEN Fussaufsetzer (Frame 25) nie",
               af_max8, f_second[0]);
        }

        /* R5 — DOPPEL-ADVANCE: waehrend Sub 6 laeuft, darf re15_player_tick den Frame-Zaehler
         * NICHT zusaetzlich hochzaehlen. Der Sub ruft anim_set genau einmal pro Tick
         * (@0x8005188c Phase 0/1, @0x800518f0 Phase 2/3), und anim_set macht +0x95 += 1
         * (@0x8001f618/1c) und +0x8f -= 1 (@0x8001f5b0-b4) — EINMAL.
         * GEMESSEN vorher (echter Lauf, RE15_MOTRACE, Plc_dest mode 6 in sub02):
         *   F61 mo=1 af=2 frac=5 | F62 af=4 frac=3 | F63 af=6 frac=1 | F64 af=8 frac=0
         * = +2 pro 30-Hz-Tick, der Halte-Clip lief doppelt so schnell. */
        printf("R5 Sub 6 besitzt +0x95 allein (kein Doppel-Advance)\n");
        {
            extern void re15_player_event_reach_begin(void);
            extern int  re15_player_event_reach_clip(void);
            re15_actor_t *p3 = &g_actors[RE15_ACTOR_SLOT_PLAYER];
            re15_player_event_reach_begin();
            ok(re15_player_event_reach_clip() == 1,
               "Sub 6 aktiv, Phase 0 -> Clip 1", re15_player_event_reach_clip(), 1);
            ok(p3->motion == 1 && p3->anim_frame == 0 && p3->anim_frac == 7,
               "Sub-6-Init: +0x94=1 (@0x80051854) +0x95=0 (@0x80051864) +0x8f=7 (@0x80051874)",
               (long)p3->anim_frac, 7);
            uint16_t af0 = p3->anim_frame; uint8_t fr0 = p3->anim_frac;
            re15_player_tick(NULL, 0);
            ok(p3->anim_frame == af0 && p3->anim_frac == fr0,
               "NEGATIV: re15_player_tick advanct waehrend Sub 6 NICHT mit",
               (long)p3->anim_frame, (long)af0);
            { extern void re15_player_event_reach_end(void); re15_player_event_reach_end(); }
            uint16_t af1 = p3->anim_frame;
            re15_player_tick(NULL, 0);
            ok(p3->anim_frame == (uint16_t)(af1 + 1),
               "Positiv-Kontrolle: ohne Sub 6 advanct player_tick wieder",
               (long)p3->anim_frame, (long)(af1 + 1));
        }
        free(raw);
    }

    printf(s_fail ? "\nFAIL: %d Pruefungen\n" : "\nOK: alle Pruefungen\n", s_fail);
    return s_fail ? 1 : 0;
}
