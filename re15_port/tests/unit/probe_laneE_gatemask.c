/* probe_laneE_gatemask.c — LANE E MESSUNG (kein Fix, keine Engine-Aenderung).
 *
 * FRAGE: Warum kommen im ORIGINAL nur KRIECHER unter dem ROOM1030-Tor durch?
 *
 * Original-Belege, die diese Sonde am PORT nachmisst:
 *   B1  Sca_id_set (Opcode 0x37) Handler @0x8004175c:
 *         lbu v1,1(a2)  = pc[1] = REGION
 *         lbu a1,2(a2)  = pc[2] = INDEX
 *         addiu v1,v1,1 / sll v1,2 / addu v1,SCAbase / lw v1,0(v1)   -> region ptr = SCAbase[region+1]
 *         sll v0,a1,1 / addu v0,v0,a1 / sll v0,v0,2                  -> index*12
 *         lbu a1,3(a2)  = pc[3] = VALUE
 *         sb  a1,9(v0)                                               -> record[+9] = VALUE (= u0)
 *   B2  ROOM1030.RDT Datei-Offsets 0x2000/0x2004 (sub00) und 0x278e/0x2792 (sub08):
 *         37 02 06 f7 / 37 03 06 f7  = Sca_id_set(rgn2,idx6,0xF7) + Sca_id_set(rgn3,idx6,0xF7)
 *   B3  Kollisions-Resolver FUN_8003b0a4:
 *         @0x8003b244 lhu v0,-2(s2) / sll 16 / sra 24   -> u0 = record[+9] (sign-ext)
 *         @0x8003b248 lbu v1,24(sp)                     -> die MASKE (Arg a2)
 *         @0x8003b254 and v1,v1,v0
 *         @0x8003b258 bne v1,zero,0x8003b2e0            -> !=0 => SOLID (push-out)
 *   B4  Maske pro Aktor (game-weiter Zensus aller 49 jal-0x8003b0a4-Aufrufe):
 *         Spieler          = 1  (@0x80031d74 / @0x800384c8  `ori a2,zero,0x1`)
 *         Standard-Gegner  = 4  (24x hart `ori a2,zero,0x4`)
 *         Zombie/ZGirl     = entity+0x1D7 (@0x80100624 / @0x8010aac8 `lbu a2,471(a0)`)
 *         entity+0x1D7 = 4 aufrecht (@0x801050b4), = 8 kriechend (@0x8010374c / @0x801050f4)
 *       => 0xF7 = 0xFF & ~0x08 laesst GENAU Maske 8 (Kriecher) durch.
 *
 * GEMESSEN wird NUR der Ist-Zustand des Ports. */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "re15_rdt.h"
#include "re15_scd.h"
#include "re15_actor.h"
#include "re15_player.h"
#include "re15_room.h"
#include "re15_collision.h"

#define RE15_STR(x)  #x
#define RE15_XSTR(x) RE15_STR(x)

extern scd_vm_t g_scd;

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

/* flat index of (region,index) exactly like the original's SCAbase[region+1] + index*12 */
static int flat_of(const re15_rdt_t *r, int rgn, int idx)
{
    int base = 0;
    for (int i = 0; i < rgn && i < 5; i++) base += r->sca_rgn[i];
    return base + idx;
}

static void dump_cell(const re15_rdt_t *r, const char *tag, int rgn, int idx)
{
    int f = flat_of(r, rgn, idx);
    if (f < 0 || f >= r->sca_count) { printf("  %-10s rgn%d idx%d -> flat %d AUSSERHALB\n", tag, rgn, idx, f); return; }
    const re15_sca_entry_t *e = &r->sca[f];
    printf("  %-10s rgn%d idx%2d -> flat%3d  type=%u u0=0x%02X u1=0x%02X floor=0x%02X band=%d "
           "rect X[%d..%d] Z[%d..%d]\n",
           tag, rgn, idx, f, e->type, e->u0, e->u1, e->floor, e->floor >> 4,
           (int)e->x, (int)e->x + (int)e->width, (int)e->z, (int)e->z + (int)e->density);
}

/* FUN_8003b068 quadrant, as the port implements it (re15_collision.c quadrant_of). */
static int quad(int32_t px, int32_t pz, int16_t ox, int16_t oz)
{
    unsigned zb = (unsigned)(pz - (int32_t)oz) & 0x80000000u;
    unsigned xb = (unsigned)(px - (int32_t)ox) & 0x80000000u;
    return (int)((zb | (xb >> 1)) >> 0x1e);
}

int main(void)
{
    const char *base = RE15_XSTR(RE15_ASSETS_PATH);
    char path[600]; size_t sz = 0;
    snprintf(path, sizeof path, "%s/STAGE1/ROOM1030.RDT", base);
    uint8_t *dat = read_file(path, &sz);
    if (!dat) { fprintf(stderr, "FAIL: %s nicht lesbar\n", path); return 1; }

    /* ---------- M0: ROH-BYTES der vier Sca_id_set-Stellen ---------- */
    printf("===== M0: Roh-Bytes der Sca_id_set-Stellen in ROOM1030.RDT =====\n");
    const uint32_t sites[4] = { 0x2000, 0x2004, 0x278e, 0x2792 };
    for (int i = 0; i < 4; i++) {
        uint32_t o = sites[i];
        printf("  @0x%04X: %02x %02x %02x %02x   -> Sca_id_set(rgn=%u, idx=%u, val=0x%02X)\n",
               o, dat[o], dat[o+1], dat[o+2], dat[o+3], dat[o+1], dat[o+2], dat[o+3]);
    }

    static re15_rdt_t rdt;
    if (re15_rdt_parse(dat, sz, &rdt) != 0) { fprintf(stderr, "FAIL: parse\n"); return 1; }
    printf("\nROOM1030: sca_count=%d rgn=[%d %d %d %d %d] origin=(%d,%d)\n",
           rdt.sca_count, rdt.sca_rgn[0], rdt.sca_rgn[1], rdt.sca_rgn[2], rdt.sca_rgn[3],
           rdt.sca_rgn[4], (int)(int16_t)rdt.ceiling_x, (int)(int16_t)rdt.ceiling_z);

    /* ---------- M1: die Tor-Zellen VOR jedem Skript ---------- */
    printf("\n===== M1: Tor-Zellen VOR dem Skript (Disc-Zustand) =====\n");
    dump_cell(&rdt, "TOR", 2, 6);
    dump_cell(&rdt, "TOR", 3, 6);

    const re15_sca_entry_t *gate = &rdt.sca[flat_of(&rdt, 2, 6)];
    int32_t gx0 = gate->x, gx1 = gate->x + gate->width;
    int32_t gz0 = gate->z, gz1 = gate->z + gate->density;
    int32_t cx  = (gx0 + gx1) / 2, cz = (gz0 + gz1) / 2;
    printf("  Tor-Mitte = (%d,%d); Quadrant der Mitte = %d\n",
           (int)cx, (int)cz, quad(cx, cz, (int16_t)rdt.ceiling_x, (int16_t)rdt.ceiling_z));
    printf("  Quadrant knapp WESTLICH (%d,%d) = %d   knapp OESTLICH (%d,%d) = %d\n",
           (int)(gx0 + 100), (int)cz, quad(gx0 + 100, cz, (int16_t)rdt.ceiling_x, (int16_t)rdt.ceiling_z),
           (int)(gx1 - 100), (int)cz, quad(gx1 - 100, cz, (int16_t)rdt.ceiling_x, (int16_t)rdt.ceiling_z));

    /* ---------- M2: Raum hochfahren, danach nochmal messen ---------- */
    re15_actor_init();
    scd_vm_init();
    g_current_room_id = 0x1030;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100;
    pl->x = -16900; pl->y = 0; pl->z = -4900; pl->rot_y = 0;
    scd_room_reenter(&rdt, pl->x, pl->z, 0);
    for (int i = 0; i < 4; i++) scd_vm_tick();

    printf("\n===== M2: nach main00+sub00 (+4 Ticks) =====\n");
    printf("  Flag Bank4 Bit15 (das Ck-Gate von sub00) = %d\n",
           re15_game_flag_get(4, 15));
    dump_cell(&rdt, "TOR", 2, 6);
    dump_cell(&rdt, "TOR", 3, 6);

    /* ---------- M3: sub08 erzwingen (die zweite, ungegatete Stelle) ---------- */
    if (rdt.sub_scd_count > 8 && rdt.sub_scd[8]) {
        printf("\n===== M3: erzwungenes sub08 =====\n");
        /* Slot 1 ist TABU: scd_vm_tick reseedet ihn JEDEN Frame mit sub01 (scd_vm.c:566
         * — der byte-true Per-Frame-Reseed FUN_8003f038). Ein dort gestartetes sub08 wird
         * sofort verdraengt (erste Messung: active=0, pc zeigte auf sub01 @+0x21A5). */
        const int SLOT = 5;
        g_scd.threads[SLOT].active = 0;
        scd_thread_start(SLOT, rdt.sub_scd[8]);
        for (int i = 0; i < 12; i++) {
            scd_vm_tick();
            const scd_thread_t *t = &g_scd.threads[SLOT];
            printf("   tick%-2d slot%d active=%d pc=+0x%04X (op 0x%02X)\n", i, SLOT, t->active,
                   t->pc ? (unsigned)(t->pc - dat) : 0u, t->pc ? t->pc[0] : 0);
        }
        dump_cell(&rdt, "TOR", 2, 6);
        dump_cell(&rdt, "TOR", 3, 6);
    }

    /* ---------- M4: die reine Masken-Arithmetik von @0x8003b254/58 ---------- */
    printf("\n===== M4: (Maske & u0) fuer u0=0xFF (zu) und u0=0xF7 (Tor offen) =====\n");
    const unsigned masks[3]   = { 1u, 4u, 8u };
    const char    *mnames[3]  = { "Spieler (1)", "Gegner aufrecht (4)", "Zombie kriechend (8)" };
    for (int u = 0; u < 2; u++) {
        unsigned u0 = u ? 0xF7u : 0xFFu;
        for (int m = 0; m < 3; m++)
            printf("   u0=0x%02X  Maske %-22s -> (m&u0)=0x%02X  => %s\n",
                   u0, mnames[m], masks[m] & u0,
                   (masks[m] & u0) ? "SOLID (push-out)" : "DURCHLAESSIG");
    }

    /* ---------- M5: was der Port tatsaechlich anlegt ---------- */
    printf("\n===== M5: Port-Clamp gegen das Tor (u0 = aktueller Wert) =====\n");
    {
        /* Testpunkt: aus dem Sueden EIN STUECK IN die Tor-Zelle hinein (kleiner Schritt —
         * der Resolver ist ein reiner Overlap-Test, kein Sweep; ein Schritt ueber die
         * ganze 1649-tiefe Zelle hinweg wuerde ihn korrekt VERFEHLEN). */
        int32_t startz = gz1 + 700, endz = (gz0 + gz1) / 2;
        int32_t tx = cx;
        struct { const char *tag; int enemy; } cases[2] = { { "PLAYER (Maske 1, r=450)", 0 },
                                                            { "ENEMY  (Maske 4, r=200)", 1 } };
        for (int c = 0; c < 2; c++) {
            int32_t x = tx, z = startz;
            re15_collision_reset_band();
            re15_collision_set_band(0);
            int32_t nx = x, nz = endz;
            if (cases[c].enemy)
                re15_collision_constrain_enemy(&rdt, x, z, &nx, &nz, 200, 0);
            else
                re15_collision_constrain(&rdt, x, z, &nx, &nz);
            printf("   %-26s (%d,%d)->soll(%d,%d) ist(%d,%d)  %s\n",
                   cases[c].tag, (int)x, (int)z, (int)tx, (int)endz, (int)nx, (int)nz,
                   (nz == endz && nx == tx) ? "DURCH" : "GEBLOCKT");
        }
        printf("   HINWEIS: der Port hat KEINEN Aufrufpfad mit Maske 8 —\n"
               "            re15_collision.c:617 uebergibt fuer JEDEN Gegner hart 4u.\n");
    }

    free(dat);
    return 0;
}
