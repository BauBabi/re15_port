/* probe_wallstep_gegen.c — GEGENPROBE zu probe_wallstep.c (reine Messsonde, kein Fix).
 *
 * probe_wallstep behauptet fuer ROOM1030-Zelle #30 (x -15518..-12566, z -24134..-22694, r=400):
 *   "Schritt 5..1120 -> Klemmung auf z = -24552 ... die andere Achse bleibt VOLL frei
 *    (Gleiten: bei Schritt (900,900) kommen die 900 in X komplett an)"
 * und schreibt den Auswurf-DURCH-die-Wand ausschliesslich der SCHMALEN Zelle #19 zu.
 *
 * Diese Sonde tastet den DIAGONALEN Anlauf lueckenlos ab (dx = dz = s, s = 1..1120) statt nur
 * bei s = 21/35/70/212/900 zu stichproben.
 *
 * Original-Belege (selbst nachgelesen):
 *   @0x8003b41c-78  Broadphase (Mass + 2r), EINZELPUNKT  -> kein Streckentest
 *   @0x8003bcb0-d40 pen = min(|+Kante|,|-Kante|), Saum +-18 (0x12)
 *   @0x8003bdf0-e30 MINIMAL-ACHSE: ax<az -> X-Push, sonst Z-Push
 *   @0x8003be8c-a0  code==0 -> Ruecksetzung auf die Vorposition, return 0
 */
#include "re15_rdt.h"
#include "re15_collision.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>

static re15_rdt_t       s_rdt;
static re15_sca_entry_t s_cell[1];

static void mk_room(int16_t cx, int16_t cz, uint16_t w, uint16_t d)
{
    memset(&s_rdt, 0, sizeof s_rdt);
    s_cell[0].width = w; s_cell[0].density = d;
    s_cell[0].x = cx;    s_cell[0].z = cz;
    s_cell[0].type = 1;  s_cell[0].u0 = 0xff; s_cell[0].u1 = 0; s_cell[0].floor = 0x03;
    s_rdt.sca = s_cell; s_rdt.sca_count = 1;
    s_rdt.sca_rgn[0] = 1;
    s_rdt.ceiling_x = (uint16_t)(int16_t)-32000;
    s_rdt.ceiling_z = (uint16_t)(int16_t)-32000;
}

int main(void)
{
    const int32_t R = 400;
    const int16_t CX = -15518, CZ = -24134;
    const uint16_t W = 2952, D = 1440;
    mk_room(CX, CZ, W, D);
    re15_collision_set_band(0);

    const int32_t x0 = CX + W / 2;      /* -14042 */
    const int32_t z0 = CZ - R - 1;      /* -24535, 1 Einheit vor dem Fangfenster */

    int32_t first_bad = -1, last_ok = -1;
    int32_t worst_over = 0, worst_step = 0;
    for (int32_t s = 1; s <= 1120; s++) {
        int32_t nx = x0 + s, nz = z0 + s; uint8_t c = 0; uint16_t a = 0;
        re15_collision_constrain_contact(&s_rdt, x0, z0, &nx, &nz, R, 0, 4u, &c, &a);
        int32_t ax = nx - x0;
        if (ax == s) { last_ok = s; continue; }
        if (first_bad < 0) first_bad = s;
        if (ax - s > worst_over) { worst_over = ax - s; worst_step = s; }
    }
    printf("DIAGONAL (dx=dz=s) an Zelle #30, s = 1..1120:\n");
    printf("  letzter Schritt mit VOLL freier X-Achse : %ld\n", (long)last_ok);
    printf("  erster Schritt mit NICHT freier X-Achse : %ld\n", (long)first_bad);
    printf("  groesster UEBERSCHUSS in X (netto - befohlen): +%ld bei Schritt %ld\n",
           (long)worst_over, (long)worst_step);

    printf("\n  Einzelwerte um die Schwelle:\n");
    for (int32_t s = 936; s <= 944; s++) {
        int32_t nx = x0 + s, nz = z0 + s; uint8_t c = 0; uint16_t a = 0;
        int hit = re15_collision_constrain_contact(&s_rdt, x0, z0, &nx, &nz, R, 0, 4u, &c, &a);
        printf("    s=%4ld -> (%7ld,%7ld) netto(%6ld,%6ld) hit=%d   X %s  | Z %s Zelle\n",
               (long)s, (long)nx, (long)nz, (long)(nx - x0), (long)(nz - z0), hit,
               (nx - x0 == s) ? "frei" : "GEKLEMMT/AUSGEWORFEN",
               (nz > CZ && nz < CZ + D) ? "IN" : "ausserhalb");
    }
    printf("\nSONDE-OK\n");
    return 0;
}
