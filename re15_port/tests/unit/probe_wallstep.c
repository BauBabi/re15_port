/* probe_wallstep.c — MESSSONDE (kein Fix): was macht die ORIGINAL-KLEMME bei einem GROSSEN
 * Bewegungsschritt?
 *
 * Gemessen wird der byte-true Port von RE1.5 FUN_8003b0a4 (re15_collision_constrain_contact,
 * Handler FUN_8003bca8 = push_rect) an EINER isolierten Zelle, damit kein Nachbar das
 * Ergebnis verwischt. Masse aus ROOM1030 Zelle #30 (rgn3 idx8, typ 1, u0=0xFF):
 *      x -15518 .. -12566 (w = 2952)   z -24134 .. -22694 (d = 1440)
 * Radius r = 400 = box[6] des Kriechers (STAGE1.BIN Datei 0x1f778 = {0,-1440,0,400,1440,400}).
 *
 * Belegstellen im Original:
 *   Broad-Phase        @0x8003b284-94 : (u32)(pos - (cell - r)) < cell.extent + 2r
 *   push_rect Eindring @0x8003bd0c..  : pen = min(|+Kante|,|-Kante|) mit Saum 0x12
 *   Richtungs-Code     (pen ^ (pos-prev)) >> 31 | ((pen ^ (pos-prev)) >> 30 & 2)
 *   code==0-Zweig      : *param_2 = *param_3 ; param_2[2] = param_3[2] ; return 0
 *                        (= RUECKSTELLUNG auf die Vorposition, @0x8003be9c-Block)
 */
#include "re15_rdt.h"
#include "re15_collision.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static re15_rdt_t        s_rdt;
static re15_sca_entry_t  s_cell[1];

static void mk_room(int16_t cx, int16_t cz, uint16_t w, uint16_t d)
{
    memset(&s_rdt, 0, sizeof s_rdt);
    s_cell[0].width = w; s_cell[0].density = d;
    s_cell[0].x = cx;    s_cell[0].z = cz;
    s_cell[0].type = 1;  s_cell[0].u0 = 0xff; s_cell[0].u1 = 0; s_cell[0].floor = 0x03;
    s_rdt.sca = s_cell; s_rdt.sca_count = 1;
    /* alle 5 Quadranten-Partitionen sehen dieselbe Zelle -> Quadrantenwahl egal */
    s_rdt.sca_rgn[0] = 1; s_rdt.sca_rgn[1] = 0; s_rdt.sca_rgn[2] = 0;
    s_rdt.sca_rgn[3] = 0; s_rdt.sca_rgn[4] = 0;
    s_rdt.ceiling_x = (uint16_t)(int16_t)-32000;  /* alle Testpunkte liegen in Quadrant 0 */
    s_rdt.ceiling_z = (uint16_t)(int16_t)-32000;
}

static void one(int32_t x0, int32_t z0, int32_t dx, int32_t dz, int32_t r)
{
    int32_t nx = x0 + dx, nz = z0 + dz;
    uint8_t contact = 0; uint16_t attr = 0;
    int hit = re15_collision_constrain_contact(&s_rdt, x0, z0, &nx, &nz, r, 0, 4u, &contact, &attr);
    int32_t ax = nx - x0, az = nz - z0;
    const char *v = (nx == x0 && nz == z0) ? "RUECKSTELLUNG auf Vorposition"
                  : (ax == dx && az == dz) ? "DURCH (kein Kontakt / kein Push)"
                                           : "geklemmt/geglitten";
    printf("  Start(%7ld,%7ld) Schritt(%6ld,%6ld) -> Ziel(%7ld,%7ld) netto(%6ld,%6ld) hit=%d  %s\n",
           (long)x0, (long)z0, (long)dx, (long)dz, (long)nx, (long)nz,
           (long)ax, (long)az, hit, v);
}

int main(void)
{
    const int32_t R = 400;
    const int16_t CX = -15518, CZ = -24134;
    const uint16_t W = 2952, D = 1440;
    mk_room(CX, CZ, W, D);
    re15_collision_set_band(0);

    printf("EINZELZELLE  x %d..%d (w=%u)   z %d..%d (d=%u)   r=%ld\n",
           CX, CX + W, W, CZ, CZ + D, D, (long)R);
    printf("Fangfenster: X %ld..%ld (w+2r=%ld)   Z %ld..%ld (d+2r=%ld)\n\n",
           (long)(CX - R), (long)(CX + W + R), (long)(W + 2*R),
           (long)(CZ - R), (long)(CZ + D + R), (long)(D + 2*R));

    const int32_t mid_x = CX + W/2;                 /* mittig in der Zellenbreite */
    const int32_t outside_z = CZ - R - 1;           /* 1 Einheit vor dem Fangfenster */

    printf("[A] Frontalanlauf von SUEDEN (dz>0) aus z=%ld — Schrittweiten des RE2-Zombies\n"
           "    (Gang 1-Keyframe max 35 / 52, mit uebersprungenem Keyframe max 70 / 102;\n"
           "     Sprint-Clip PAIR2/1 max 212):\n", (long)outside_z);
    { static const int32_t st[] = {5,11,21,35,52,70,102,212,400,800,1200,1800,2239,2240,2241,2600};
      for (unsigned i=0;i<sizeof st/sizeof st[0];i++) one(mid_x, outside_z, 0, st[i], R); }

    printf("\n[B] Derselbe Anlauf, aber der Aktor steht schon AN der Kante (z = CZ - r):\n");
    { static const int32_t st[] = {5,11,21,35,52,70,102,212,400,800,1200,1800,2600};
      for (unsigned i=0;i<sizeof st/sizeof st[0];i++) one(mid_x, CZ - R, 0, st[i], R); }

    printf("\n[C] SCHRAEG (Gleit-Test): gleicher Betrag in +X und +Z:\n");
    { static const int32_t st[] = {21,35,70,212,900,2600};
      for (unsigned i=0;i<sizeof st/sizeof st[0];i++) one(mid_x, outside_z, st[i], st[i], R); }

    printf("\n[D] DURCHTUNNEL-SCHWELLE, fein abgetastet (Start z=%ld):\n", (long)outside_z);
    for (int32_t s = 2200; s <= 2280; s += 10) one(mid_x, outside_z, 0, s, R);

    printf("\n[E] Anlauf von WESTEN gegen die schmale Seite (Fenster %ld):\n", (long)(W + 2*R));
    { static const int32_t st[] = {35,102,212,1000,2000,3000,3751,3752,3760,4000};
      for (unsigned i=0;i<sizeof st/sizeof st[0];i++)
          one(CX - R - 1, CZ + D/2, st[i], 0, R); }

    printf("\n[F] Gegenprobe an der SCHMALSTEN ROOM1030-Zelle (#19: w=358, d=1093):\n");
    mk_room((int16_t)-10255, (int16_t)-24036, 358, 1093);
    { static const int32_t st[] = {35,70,102,212,400,1000,1157,1158,1159,1200};
      for (unsigned i=0;i<sizeof st/sizeof st[0];i++)
          one((int32_t)-10255 - R - 1, (int32_t)-24036 + 1093/2, st[i], 0, R); }

    printf("\n[G] SCHWELLEN, exakt abgetastet (Zelle #30, Anlauf von Sueden, x mittig):\n");
    mk_room(CX, CZ, W, D);
    {
        int32_t lo = -1, hi = -1, tun = -1;
        for (int32_t st = 1; st <= 3000; st++) {
            int32_t nx = mid_x, nz = outside_z + st; uint8_t c = 0; uint16_t a = 0;
            int hit = re15_collision_constrain_contact(&s_rdt, mid_x, outside_z, &nx, &nz,
                                                       R, 0, 4u, &c, &a);
            int reset = (nx == mid_x && nz == outside_z);
            int freerun = (!hit && nz == outside_z + st);
            if (reset && lo < 0) lo = st;
            if (reset) hi = st;
            if (freerun && st > 100 && tun < 0) tun = st;
        }
        printf("    erste RUECKSTELLUNG bei Schritt %ld  (Zellmitte cz+d/2 = %ld)\n",
               (long)lo, (long)(CZ + D / 2));
        printf("    letzte RUECKSTELLUNG bei %ld, erster DURCHTUNNEL bei %ld (Fangfenster d+2r = %ld)\n",
               (long)hi, (long)tun, (long)(D + 2 * R));
    }
    printf("\nSONDE-OK\n");
    return 0;
}
