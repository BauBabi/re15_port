/* test_1090_stay_on_crate.c — PIN: OBEN AUF DER KISTE BLEIBEN (ROOM1090).
 *
 * Nutzer-Report 2026-08-22: "Ich kann auf die Box klettern, aber bin dann oben nicht auf die
 * Box beschraenkt — ich kann in der Luft rumlaufen" und "ich komme nicht mehr von der Box
 * runter". Im ECHTEN Spiel gemessen (RE15_DEBUG_JUMP=1090@120 + RE15_INPUT_SCRIPT): einen
 * Frame nachdem der Kletter-FSM freigibt, sprang der Spieler von z=4266 auf z=3403 —
 * das ist exakt `kiste.z - box_hz - Spielerradius` = 4753 - 900 - 450, also die
 * AUSSCHIEBUNG aus genau der Kiste, auf der er steht. Damit stand er neben der Kiste in der
 * Luft (y blieb -3600) und DAT_800ac788 fiel auf -1, wodurch auch der Abwaerts-Zweig der
 * Sonde (@0x8002d6f4 verlangt `DAT_800aca3c & 0x4000`) nie mehr greifen konnte.
 *
 * Der Original-Mechanismus (selbst disassembliert):
 *
 *   FUN_8002bd44 @0x8002beec-0x8002bf0c — der Objekt-Pass des Spielers:
 *     8002bef0  lw   v0, 0x0(s6)=>DAT_800ac788    ; das Standobjekt
 *     8002bef8  bne  s2, v0, LAB_8002bf14         ; NUR wenn s2 DAS Standobjekt ist:
 *     8002bf00  addiu a0, s6, 0x2cc               ;   a0 = Spieler (0x800aca54)
 *     8002bf04  jal  FUN_8002cfd4                 ;   a1 = s2 -> EINGRENZUNG
 *     8002bf0c  j    LAB_8002c0ec                 ;   und der Push-Out (jal FUN_8002cabc
 *                                                 ;   @0x8002c0e4) wird UEBERSPRUNGEN
 *
 *   FUN_8002cfd4 @0x8002cfd4 — die Eingrenzung (a3 = Spieler, t2 = Objekt):
 *     8002cfe0  andi v0, player[0x00], 0x40   -> gesetzt: return   (@0x8002cfe4 bne)
 *     8002cff4  andi v0, obj[0x00],    0x2    -> gesetzt: return   (@0x8002cff8 bne)
 *     8002d008  t1 = obj[0x34]                8002d014  t3 = obj[0x3c]
 *     8002d028  a0 = *(u16*)(player[0x78]+6)  8002d034  a0 >>= 1      ; 450 >> 1 = 225
 *     8002d038  t0 = obj_hx - a0              8002d044  v1 = obj_hz - a0  <<< DERSELBE a0
 *     8002d01c  t8 = t1 - player[0x34]        8002d054  t7 = t3 - player[0x3c]
 *     8002d05c/64  AUSSERHALB = `(u32)(2*h) < (u32)(d + h)`  Bit0 = X, Bit1 = Z
 *     8002d06c  beide drin -> return
 *     8002d074/88  dieselben Tests mit der VORposition player[0x40]/[0x44] (`lh` = SIGNIERT)
 *     8002d0ac-b0  a0 = (vorher ^ jetzt) & jetzt   = nur die GERADE verlassenen Achsen
 *     8002d0c0-cc  X: dx>=0 -> obj.x - t0 sonst obj.x + t0   (`sw 0x34(a3)`)
 *     8002d0d0-f4  Z: dz>=0 -> obj.z - v1 sonst obj.z + v1   (`sw 0x3c(a3)`)
 *
 *   Der Standobjekt-Test selbst, FUN_80031c44 @0x80031ce8-0x80031d20:
 *     FUN_8002d100(player, 0x12) und `obj[0x82] + 1 == DAT_800acad6`.
 *
 * ROOM1090 obj0: pos=(-8136,-1800,4753) box h=(900,900,900) band=1
 *   -> Schrumpfmass 900 - 225 = 675, Eingrenzung x[-8811,-7461] z[4078,5428].
 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "re15_rdt.h"
#include "re15_scd.h"
#include "re15_actor.h"
#include "re15_player.h"
#include "re15_room.h"
#include "re15_climb.h"
#include "re15_collision.h"
#include "re15_skeleton.h"

extern scd_vm_t g_scd;

static int g_fail = 0;
static void chk(const char *what, long got, long want)
{
    if (got != want) { printf("FAIL %-48s got=%ld want=%ld\n", what, got, want); g_fail++; }
    else               printf("ok   %-48s %ld\n", what, got);
}

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

static re15_emd_animation_t s_pl00, s_w01;
static int load_banks(void)
{
    char p[600]; size_t sz = 0;
    snprintf(p, sizeof p, "%s/PLD/PL00.EDD", RE15_ASSET_PSX_DIR);
    uint8_t *a = read_file(p, &sz);
    if (!a || re15_emd_parse_animation(a, sz, &s_pl00) != 0) return 0;
    snprintf(p, sizeof p, "%s/PLD/PL00W01.EDD", RE15_ASSET_PSX_DIR);
    uint8_t *b = read_file(p, &sz);
    if (!b || re15_emd_parse_animation(b, sz, &s_w01) != 0) return 0;
    return 1;
}

static re15_rdt_t    s_rdt;
static re15_actor_t *s_pl;

static void reset_room(void)
{
    re15_actor_init();
    scd_vm_init();
    re15_climb_reset();
    g_current_room_id = 0x1090;
    g_room_change.pending = 0;
    s_pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    s_pl->active = 1; s_pl->type = 0; s_pl->hp = 100;
    s_pl->x = -10100; s_pl->y = -1800; s_pl->z = 4200; s_pl->rot_y = 0;
    scd_room_reenter(&s_rdt, s_pl->x, s_pl->z, 0);
    re15_collision_set_band(re15_collision_band_from_y(s_pl->y));
}

/* Genau der Zustand, den der ECHTE Lauf nach dem Aufstieg zeigte (Messreihe t7.txt, T311):
 * auf dem Kistendeckel, Band 2, posS auf die Kistenmitte gesnappt (@0x800382bc-d8). */
static void stand_on_crate(int32_t x, int32_t z)
{
    s_pl->x = x; s_pl->z = z;
    s_pl->y = (int32_t)g_scd.props[0].y - 2 * (int32_t)g_scd.props[0].box_hy;   /* -3600 */
    s_pl->flags = 0;
    s_pl->pos_s_x = (uint16_t)(int32_t)g_scd.props[0].x;
    s_pl->pos_s_z = (uint16_t)(int32_t)g_scd.props[0].z;
    re15_collision_set_band((int)g_scd.props[0].band + 1);                      /* 2 */
}

int main(void)
{
    size_t sz = 0;
    uint8_t *raw = read_file(RE15_ASSET_PSX_DIR "/STAGE1/ROOM1090.RDT", &sz);
    if (!raw) { printf("SKIP: ROOM1090.RDT fehlt\n"); return 77; }
    if (re15_rdt_parse(raw, sz, &s_rdt) < 0) { printf("SKIP: RDT-Parse\n"); return 77; }
    if (!load_banks()) { printf("SKIP: PL00/PL00W01-Baenke fehlen\n"); return 77; }
    reset_room();

    const int32_t ox = (int32_t)g_scd.props[0].x;          /* -8136 */
    const int32_t oz = (int32_t)g_scd.props[0].z;          /*  4753 */
    const int32_t sx = (int32_t)g_scd.props[0].box_hx - (450 >> 1);   /* @0x8002d038 -> 675 */
    const int32_t szk= (int32_t)g_scd.props[0].box_hz - (450 >> 1);   /* @0x8002d044 -> 675 */
    chk("C0 Schrumpfmass X (obj_hx - PR/2)", (long)sx, 675);
    chk("C0 Schrumpfmass Z (obj_hz - PR/2)", (long)szk, 675);

    /* ---- C1: Standobjekt-Test @0x80031ce8-0x80031d20 -------------------------------- */
    stand_on_crate(ox + 113, oz - 487);                    /* = (-8023, 4266), im Kasten */
    chk("C1 standing_probe auf dem Deckel", re15_climb_standing_probe(s_pl->x, s_pl->z), 0);
    /* Auf dem BODEN (Band 1) ist `obj[0x82]+1 == DAT_800acad6` falsch -> kein Standobjekt. */
    re15_collision_set_band(1);
    chk("C1 standing_probe auf dem Boden (Band 1)",
        re15_climb_standing_probe(s_pl->x, s_pl->z), -1);

    /* ---- C2: der gemeldete Bug — KEINE Ausschiebung aus der eigenen Kiste ------------ */
    stand_on_crate(ox + 113, oz - 487);
    {
        int32_t nx = s_pl->x, nz = s_pl->z;
        re15_collision_objects(&nx, &nz);
        chk("C2 X bleibt (kein Push-Out, @0x8002bf0c)", (long)nx, (long)s_pl->x);
        chk("C2 Z bleibt (kein Push-Out, @0x8002bf0c)", (long)nz, (long)s_pl->z);
        /* Die REGRESSION war exakt dieser Wert (im echten Spiel gemessen): */
        chk("C2 NICHT der alte Ausschiebe-Wert 3403",
            (long)(nz == oz - (int32_t)g_scd.props[0].box_hz - 450), 0);
    }

    /* ---- C3: die Eingrenzung klemmt auf die Kante (@0x8002d0c0-cc / @0x8002d0d0-f4) -- */
    stand_on_crate(ox + sx + 200, oz);            /* 200 ueber die +X-Kante hinaus */
    {
        int32_t nx = s_pl->x, nz = s_pl->z;
        re15_collision_objects(&nx, &nz);
        chk("C3 +X ueber die Kante -> obj.x + sx", (long)nx, (long)(ox + sx));
        chk("C3 Z unangetastet", (long)nz, (long)oz);
    }
    stand_on_crate(ox - sx - 200, oz);            /* 200 ueber die -X-Kante hinaus */
    {
        int32_t nx = s_pl->x, nz = s_pl->z;
        re15_collision_objects(&nx, &nz);
        chk("C3 -X ueber die Kante -> obj.x - sx", (long)nx, (long)(ox - sx));
    }
    stand_on_crate(ox, oz + szk + 200);
    {
        int32_t nx = s_pl->x, nz = s_pl->z;
        re15_collision_objects(&nx, &nz);
        chk("C3 +Z ueber die Kante -> obj.z + sz", (long)nz, (long)(oz + szk));
        chk("C3 X unangetastet", (long)nx, (long)ox);
    }
    stand_on_crate(ox, oz - szk - 200);
    {
        int32_t nx = s_pl->x, nz = s_pl->z;
        re15_collision_objects(&nx, &nz);
        chk("C3 -Z ueber die Kante -> obj.z - sz", (long)nz, (long)(oz - szk));
    }

    /* ---- C4: INNERHALB -> die Eingrenzung fasst nicht an (@0x8002d06c beq) ----------- */
    stand_on_crate(ox + sx - 1, oz + szk - 1);
    {
        int32_t nx = s_pl->x, nz = s_pl->z;
        re15_collision_objects(&nx, &nz);
        chk("C4 knapp INNEN: X unveraendert", (long)nx, (long)(ox + sx - 1));
        chk("C4 knapp INNEN: Z unveraendert", (long)nz, (long)(oz + szk - 1));
    }

    /* ---- N1: `(vorher ^ jetzt) & jetzt` @0x8002d0ac-b0 — war er auf der Achse SCHON
     *          draussen, wird NICHT geklemmt. --------------------------------------- */
    stand_on_crate(ox + sx + 200, oz);
    s_pl->pos_s_x = (uint16_t)(int32_t)(ox + sx + 100);    /* Vorposition ebenfalls draussen */
    {
        int32_t nx = s_pl->x, nz = s_pl->z;
        re15_collision_objects(&nx, &nz);
        chk("N1 vorher schon draussen -> KEINE Klemmung", (long)nx, (long)(ox + sx + 200));
        (void)nz;
    }

    /* ---- N2: Spieler-Flag 0x40 (@0x8002cfe0-e4) und Objekt-Flag 0x2 (@0x8002cff4-f8) - */
    stand_on_crate(ox + sx + 200, oz);
    s_pl->flags = 0x40;
    {
        int32_t nx = s_pl->x, nz = s_pl->z;
        re15_collision_objects(&nx, &nz);
        chk("N2 player[0x00]&0x40 -> keine Eingrenzung", (long)nx, (long)(ox + sx + 200));
    }
    stand_on_crate(ox + sx + 200, oz);
    {
        uint16_t keep = g_scd.props[0].flags;
        g_scd.props[0].flags = (uint16_t)(keep | 2u);
        int32_t nx = s_pl->x, nz = s_pl->z;
        re15_collision_objects(&nx, &nz);
        chk("N2 obj[0x00]&0x2 -> keine Eingrenzung", (long)nx, (long)(ox + sx + 200));
        g_scd.props[0].flags = keep;
    }

    /* ---- N3: KEIN Standobjekt -> die normale Ausschiebung MUSS weiter greifen -------
     * (sonst waere die Kiste vom Boden aus durchlaufbar — die Gegenprobe zum Fix.) */
    reset_room();
    s_pl->y = -1800;
    s_pl->x = ox - (int32_t)g_scd.props[0].box_hx - 200;   /* 250 IN die Kiste hinein */
    s_pl->z = oz;
    s_pl->flags = 0;
    re15_collision_set_band(1);
    chk("N3 kein Standobjekt am Boden", re15_climb_standing_probe(s_pl->x, s_pl->z), -1);
    {
        int32_t nx = s_pl->x, nz = s_pl->z;
        re15_collision_objects(&nx, &nz);
        chk("N3 Ausschiebung aktiv (x auf box_hx+450)",
            (long)nx, (long)(ox - (int32_t)g_scd.props[0].box_hx - 450));
    }

    /* ---- E1: End-to-end im FSM — aufsteigen, oben bleiben, absteigen ---------------- */
    reset_room();
    s_pl->x = ox - (int32_t)g_scd.props[0].box_hx - 450;
    s_pl->z = oz;
    s_pl->y = -1800;
    s_pl->rot_y = 0;
    re15_collision_set_band(1);
    re15_climb_standing_tick();
    chk("E1 Aufstieg startet", re15_climb_try_start(&s_rdt, 1), 1);
    for (int f = 0; f < 200 && re15_climb_active(); f++) {
        re15_climb_tick(&s_rdt, NULL, &s_pl00, &s_w01);
        re15_climb_standing_tick();
    }
    chk("E1 oben, Substate 0", re15_climb_dbg_sub(), 0);
    chk("E1 Y == Kistendeckel", (long)s_pl->y,
        (long)((int32_t)g_scd.props[0].y - 2 * (int32_t)g_scd.props[0].box_hy));
    chk("E1 steht auf Objekt 0", re15_climb_dbg_standing(), 0);

    /* 60 Schritte vorwaerts (+X) mit dem Objekt-Pass wie im Frame: er DARF den Kasten
     * nicht verlassen und muss Standobjekt bleiben. */
    {
        int left = 0, lost = 0;
        for (int f = 0; f < 60; f++) {
            int32_t nx = s_pl->x + 60, nz = s_pl->z;
            re15_collision_objects(&nx, &nz);
            s_pl->x = nx; s_pl->z = nz;
            re15_climb_standing_tick();
            if (s_pl->x > ox + sx || s_pl->x < ox - sx ||
                s_pl->z > oz + szk || s_pl->z < oz - szk) left = 1;
            if (re15_climb_dbg_standing() != 0) lost = 1;
        }
        chk("E1 nie ausserhalb des Kastens gelaufen", left, 0);
        chk("E1 Standobjekt nie verloren", lost, 0);
        chk("E1 an der +X-Kante geklemmt", (long)s_pl->x, (long)(ox + sx));
    }

    /* Abstieg nach -X — im echten Spiel bei T466 gemessen (KIND 2 @0x8002d740). */
    s_pl->x = ox - sx;
    s_pl->z = oz;
    s_pl->rot_y = 2048;
    re15_climb_standing_tick();
    chk("E1 Abstieg startet", re15_climb_try_start(&s_rdt, 1), 1);
    chk("E1 KIND == 2 (@0x8002d740)", re15_climb_dbg_kind(), 2);
    {
        int saw10 = 0;
        for (int f = 0; f < 400 && re15_climb_active(); f++) {
            re15_climb_tick(&s_rdt, NULL, &s_pl00, &s_w01);
            re15_climb_standing_tick();
            if (re15_climb_dbg_sub() == 10) saw10 = 1;
        }
        chk("E1 Substate 10 erreicht (@0x8003811c)", saw10, 1);
        chk("E1 wieder auf Band 1 gelandet", re15_collision_debug_band(), 1);
        chk("E1 Y wieder Bodenhoehe", (long)s_pl->y,
            (long)re15_collision_room_coll(&s_rdt, s_pl->x, s_pl->z, 1, 8, 0x100));
    }

    free(raw);
    printf(g_fail ? "\nFEHLER: %d\n" : "\nALLE PINS OK (%d Fehler)\n", g_fail);
    return g_fail ? 1 : 0;
}
