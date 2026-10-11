/*
 * RE1.5 Rebuilt — ZELLEN-AUSSCHIEBUNG der Objekte: Ruhe-Zweig LAB_8002c0b8 des Objekt-Passes FUN_8002bd44,
 * FUN_8003b558 mit Ausschiebung FUN_8003da78. Runde 36 Spur R "dachkiste", Nachbesserung 3 (P1, P4).
 *
 * Herleitung und Adressen: include/re15_zellschub_r36.h und analysis/befunde_runde36/R_dachkiste.md
 * ("Nachbesserung 3"). Jede Konstante traegt die Adresse ihrer Instruktion in info/Re1.5/PSX.EXE.
 *
 * Gemessener Befund (Abnahme 2, A7/O3): der Tritt obj 2 in ROOM2060 steht im Original nach dem Raumstart bei
 * x -3143 (Record @0x00EA6 -3125; Box c(50,-900,0) h(675,900,600) beruehrt die Zelle r0[9] x -2400..3150 an der
 * Kante, halboffener Test = Treffer -> FUN_8003da78 schiebt ihn um Eindringtiefe 0 + 18 nach Westen), danach
 * ist der Nord-Schub frei (orig_2060b_abn2: 19x SCHUB-OK). Der Port liess ihn bei -3125 an der Kante und
 * sperrte jeden Schub (2060echt2: F669 SCHUB-GESPERRT ziel=(-3125,2001), TREFFER r0[9]).
 */
#include <stdint.h>
#include <string.h>

#include "re15_zellschub_r36.h"
#include "re15_scd.h"
#include "re15_actor.h"
#include "re15_climb.h"         /* re15_climb_standing_probe = DAT_800ac788 (Stand-Objekt) */
#include "re15_objfall_r36.h"   /* re15_objfall_info: obj+4 (faellt) -> Spiegel = Lage (@0x8002c6ac-b0) */

extern scd_vm_t g_scd;

/* ---- Spiegel obj+0x40/+0x44 je Pool-Platz (u16, gelesen als s16 @0x8003db14/@0x8003dc38) ---------- */
static uint16_t s_spiegel_x[RE15_SCD_MAX_PROPS];
static uint16_t s_spiegel_z[RE15_SCD_MAX_PROPS];
static int      s_pass_lief;
/* Messschrieb (RE15_PUSH_LOG, dachkiste_r36.c): je Treffer von FUN_8003b558 im Ruhe-Zweig 8 Werte —
 * Zellenindex, x0, x1, z0, z1, r, Lage x/z NACH FUN_8003da78. Nur Diagnose. */
static int32_t  s_log[8 * 8];
static int      s_log_n;

void re15_zellschub_install(uint16_t room_id)
{
    (void)room_id;
    /* PORT-WAHL (belegt im Header): Startwert = Lage beim Raumaufbau. Der Installer LAB_80040914 schreibt den
     * Spiegel nicht, im Original steht dort der Altspeicher des Pool-Platzes. */
    for (int p = 0; p < RE15_SCD_MAX_PROPS; p++) {
        s_spiegel_x[p] = (uint16_t)g_scd.props[p].x;
        s_spiegel_z[p] = (uint16_t)g_scd.props[p].z;
    }
    s_pass_lief = 0;
}

void re15_zellschub_spiegel(int p)
{
    if (p < 0 || p >= RE15_SCD_MAX_PROPS) return;
    s_spiegel_x[p] = (uint16_t)g_scd.props[p].x;      /* @0x8002c0f0 lhu 52 -> @0x8002c100 sh 64 */
    s_spiegel_z[p] = (uint16_t)g_scd.props[p].z;      /* @0x8002c0f4 lhu 60 -> @0x8002c10c sh 68 */
}

int re15_zellschub_spiegel_lesen(int p, int32_t *sx, int32_t *sz)
{
    if (p < 0 || p >= RE15_SCD_MAX_PROPS || !sx || !sz) return -1;
    *sx = (int32_t)(int16_t)s_spiegel_x[p];
    *sz = (int32_t)(int16_t)s_spiegel_z[p];
    return 0;
}

/* ---- FUN_8003b068 — Quadrant (2 Bit) der Sonde; identisch zu quadrant_of in re15_collision.c ------------
 *   8003b068  lh v0,4(a1) / lw v1,8(a0) / addu / subu v0,v0,a3   ; (cz + obj.z) - ceil_z
 *   8003b084  and v0,v0,0x80000000                                ; Bit 31 = z < ceil_z
 *   8003b080  lh v1,0(a1) / lw a0,0(a0) / addu / subu v1,v1,a2   ; (cx + obj.x) - ceil_x
 *   8003b090  and v1,0x80000000 / srl v1,v1,1                     ; Bit 30 = x < ceil_x
 *   8003b098  or v0,v0,v1 / srl v0,v0,30                           ; Quadrant 0..3 */
static int quadrant_8003b068(int32_t cxw, int32_t czw, int16_t ceil_x, int16_t ceil_z)
{
    uint32_t v0 = (uint32_t)(czw - (int32_t)ceil_z) & 0x80000000u;
    uint32_t v1 = ((uint32_t)(cxw - (int32_t)ceil_x) & 0x80000000u) >> 1;
    return (int)((v0 | v1) >> 30);
}

static int32_t betrag(int32_t v) { return v < 0 ? -v : v; }   /* bgez / subu v0,zero,v0 */

/* ---- FUN_8003da78(a0 = Zelle, a1 = obj, a2 = r) --------------------------------------------------------- */
int re15_zellschub_8003da78(int32_t cell_x, int32_t cell_w, int32_t cell_z, int32_t cell_d,
                            int32_t box_cx, int32_t box_cz, int32_t r,
                            int32_t spiegel_x, int32_t spiegel_z, int32_t *x, int32_t *z)
{
    const int32_t t2  = *x;                                   /* @0x8003da88 lw t2,52(a1)             */
    const int32_t cxw = box_cx + t2;                          /* @0x8003daa0-a8 lh 0(t1) + t2          */
    int32_t t0 = (cell_x + cell_w + r) - cxw + 18;            /* @0x8003da98-ab0 (x0+w+r)-cxw, +18     */
    int32_t a0 = (cell_x - r) - cxw - 18;                     /* @0x8003daa4/@0x8003dab4-b8            */
    if (-a0 < t0) t0 = a0;                                    /* @0x8003dabc-acc slt v0,-a0,t0 -> t0=a0 */

    const int32_t t1  = *z;                                   /* @0x8003dadc lw t1,60(a1)              */
    const int32_t czw = box_cz + t1;                          /* @0x8003dad8-ae8 lh 4(t1) + obj.z      */
    int32_t a3 = (cell_z + cell_d + r) - czw + 18;            /* @0x8003dad0-af0                       */
    int32_t az = (cell_z - r) - czw - 18;                     /* @0x8003daf4-afc                       */
    if (-az < a3) a3 = az;                                    /* @0x8003db00-b10                       */

    const int32_t dx = t2 - spiegel_x;                        /* @0x8003db14/@0x8003db1c lh 64 (s16)   */
    const int32_t dz = t1 - spiegel_z;                        /* @0x8003db18/@0x8003db28 lh 68 (s16)   */
    const unsigned code = (unsigned)(((uint32_t)(t0 ^ dx)) >> 31)             /* @0x8003db20-24 */
                        | (unsigned)((((uint32_t)(a3 ^ dz)) >> 30) & 2u);     /* @0x8003db2c-38 */

    if (code == 2u) {                                         /* @0x8003db3c-40 beq v1,2 -> 0x8003dba0 */
        const int32_t b = betrag(a3);                         /* @0x8003dba0-a8                        */
        if ((uint32_t)(uint16_t)(b + 400) < 0x321u) {         /* @0x8003dbac-b8 sltiu (b+400)&0xffff,801 */
            *z = a3 + t1;                                     /* @0x8003dbbc/@0x8003dbc4 sw 60(a1)     */
            return 1;                                         /* @0x8003db9c ori v0,zero,1             */
        }
        /* -> 0x8003dbc8 */
    } else if (code == 1u) {                                  /* @0x8003db44-50 slti 3 / beq v1,1 -> db74 */
        const int32_t b = betrag(t0);                         /* @0x8003db74-7c                        */
        if ((uint32_t)(uint16_t)(b + 400) < 0x321u) {         /* @0x8003db80-8c                        */
            *x = t0 + t2;                                     /* @0x8003db90-94 sw 52(a1)              */
            return 1;
        }
        /* -> 0x8003dbc8 */
    } else if (code == 0u) {                                  /* @0x8003db58 j 0x8003dc2c              */
        if (t2 != spiegel_x || t1 != spiegel_z) {             /* @0x8003dc38-58 lh 64/68 == lw 52/60 ? */
            *x = spiegel_x;                                   /* @0x8003dca4/@0x8003dcb0 sw a0,52(v1)  */
            *z = spiegel_z;                                   /* @0x8003dca8/@0x8003dcb4 sw a1,60(v1)  */
            return 0;                                         /* @0x8003dcac addu v0,zero,zero         */
        }
        if (betrag(t0) < betrag(a3)) *x = t0 + t2;            /* @0x8003dc60-88 slt |t0|,|a3| -> sw 52 */
        else                         *z = a3 + t1;            /* @0x8003dc8c-94 sw 60                  */
        return 1;
    }
    /* Code 3 (@0x8003db60-64 beq v1,3) und die Ueberlaeufe von Code 1/2: 0x8003dbc8 kleinerer Betrag */
    if (betrag(t0) < betrag(a3)) *x = t0 + t2;                /* @0x8003dbc8-e4 / @0x8003dbec-c08 sw 52 */
    else                         *z = a3 + t1;                /* @0x8003dc0c-28 sw 60                  */
    return 1;                                                 /* @0x8003db98-9c                        */
}

/* ---- FUN_8003b558(a0 = obj, a1 = Maske) -------------------------------------------------------------- */
int re15_zellschub_8003b558(const re15_rdt_t *rdt, int p, int32_t *x, int32_t *z, unsigned mask)
{
    if (!rdt || !rdt->sca || rdt->sca_count <= 0 || !x || !z) return 0;
    if (p < 0 || p >= RE15_SCD_MAX_PROPS) return 0;
    const int32_t cx = (int32_t)g_scd.props[p].box_cx;            /* @0x8003b5b8 lhu 0(s3) -> Sonde      */
    const int32_t cz = (int32_t)g_scd.props[p].box_cz;            /* @0x8003b5c4 lhu 4(s3)               */
    const int32_t hx = (int32_t)(uint16_t)g_scd.props[p].box_hx;  /* @0x8003b684 lhu v1,6(a3)            */
    const int32_t hz = (int32_t)(uint16_t)g_scd.props[p].box_hz;  /* @0x8003b688 lhu v0,10(a3)           */
    const int     band = (int)g_scd.props[p].band;                /* @0x8003b638 lbu v1,130(s1)          */

    /* Spiegel fuer FUN_8003da78: waehrend eines Falls hat FUN_8002c444 ihn gerade auf die Lage gesetzt
     * (@0x8002c6ac-b0), sonst steht der Wert des letzten Bildendes (@0x8002c0f0-c10c) darin. */
    int32_t sx = (int32_t)(int16_t)s_spiegel_x[p];
    int32_t sz = (int32_t)(int16_t)s_spiegel_z[p];
    { re15_objfall_info_t o;
      if (re15_objfall_info(p, &o) == 0 && o.fall) { sx = *x; sz = *z; } }

    /* Quadrant der Sonde (Lage + Box-Mitte) @0x8003b5dc-5f0, Zellenliste des Quadranten @0x8003b5f4-624 */
    const int q = quadrant_8003b068(*x + cx, *z + cz, (int16_t)rdt->ceiling_x, (int16_t)rdt->ceiling_z);
    int start = 0; for (int i = 0; i < q && i < 5; i++) start += rdt->sca_rgn[i];
    int end = start + (q < 5 ? rdt->sca_rgn[q] : 0);
    if (end > rdt->sca_count) end = rdt->sca_count;
    if (start < 0) start = 0;

    int s5 = 0;                                                   /* @0x8003b5c8 addu s5,zero,zero       */
    const int m = (int)(mask & 0xffu);                            /* @0x8003b628 andi s6,s0,0xff         */
    for (int i = start; i < end; i++) {                           /* @0x8003b634 .. @0x8003b7b8          */
        const re15_sca_entry_t *e = &rdt->sca[i];
        /* @0x8003b634-644: (s16)(cell+10) >> 12 == obj+0x82 — Bandnibble vorzeichenbehaftet */
        const int cband = ((int32_t)(int16_t)(uint16_t)(((unsigned)e->floor << 8) | e->u1)) >> 12;
        if (cband != band) continue;
        /* @0x8003b64c-660: ((s8)u0 & Maske) == 0 -> naechste Zelle */
        if ((((int32_t)(int8_t)e->u0) & m) == 0) continue;
        if (g_scd.props[p].flags & 0x8u) continue;                /* @0x8003b668-674 obj+0 & 8          */

        int32_t r = hx;                                           /* @0x8003b68c lhu t0,6(a3)            */
        if (hx != hz) {                                           /* @0x8003b690 beq v1,v0 -> 0x8003b738 */
            const int32_t cxw = cx + *x;                          /* @0x8003b698-6ac                     */
            const int32_t x0 = (int32_t)e->x, x1 = x0 + (int32_t)e->width;
            const unsigned xcode = (unsigned)(((((uint32_t)(cxw - x1)) & 0x80000000u)
                                              | ((((uint32_t)(cxw - x0)) & 0x80000000u) >> 1)) >> 30);
            const int32_t czw = cz + *z;                          /* @0x8003b6d0-6dc                     */
            const int32_t z0 = (int32_t)e->z, z1 = z0 + (int32_t)e->density;
            const unsigned zcode = (unsigned)(((((uint32_t)(czw - z1)) & 0x80000000u)
                                              | ((((uint32_t)(czw - z0)) & 0x80000000u) >> 1)) >> 30);
            /* @0x8003b6f8-6fc xcode != 2 -> r = hx; @0x8003b704-708 zcode == xcode -> r = hx;
             * @0x8003b70c-734: zcode 3 oder 0 -> r = hz (`lhu t0,10(a3)` @0x8003b734), zcode 1 unmoeglich */
            if (xcode == 2u && zcode != 2u && (zcode == 3u || zcode == 0u)) r = hz;
        }
        /* @0x8003b738-768: (u32)((cx + obj.x) - (x0 - r)) < (u32)(w + 2r) — obj.x JE ZELLE neu gelesen */
        if (!((uint32_t)((cx + *x) - ((int32_t)e->x - r)) < (uint32_t)((int32_t)e->width + 2 * r))) continue;
        /* @0x8003b770-794: dito z */
        if (!((uint32_t)((cz + *z) - ((int32_t)e->z - r)) < (uint32_t)((int32_t)e->density + 2 * r))) continue;
        /* @0x8003b79c jal 0x8003da78 (a0 = Zelle, a1 = obj, a2 = r); @0x8003b7a4 s5 |= 1 */
        (void)re15_zellschub_8003da78((int32_t)e->x, (int32_t)e->width, (int32_t)e->z, (int32_t)e->density,
                                      cx, cz, r, sx, sz, x, z);
        s5 |= 1;
        if (s_log_n < 8) {                                        /* Messschrieb */
            int32_t *l = &s_log[8 * s_log_n++];
            l[0] = i; l[1] = e->x; l[2] = (int32_t)e->x + (int32_t)e->width; l[3] = e->z;
            l[4] = (int32_t)e->z + (int32_t)e->density; l[5] = r; l[6] = *x; l[7] = *z;
        }
    }
    return s5;                                                    /* @0x8003b7bc andi v0,s5,0xff         */
}

/* ---- Ruhe-Zweig LAB_8002c0b8 fuer ein Objekt: FUN_8003b558(obj, 2) @0x8002c0d0, Rueckgabe ungeprueft -- */
void re15_zellschub_ruhe(const re15_rdt_t *rdt, int p)
{
    if (p < 0 || p >= RE15_SCD_MAX_PROPS || !g_scd.props[p].active) return;
    int32_t x = g_scd.props[p].x, z = g_scd.props[p].z;
    const int32_t ox = x, oz = z;
    s_log_n = 0;
    (void)re15_zellschub_8003b558(rdt, p, &x, &z, 2u);           /* @0x8002c0b8 ori a1,zero,2           */
    g_scd.props[p].x = x;                                         /* FUN_8003da78 sw 52(a1)              */
    g_scd.props[p].z = z;                                         /* FUN_8003da78 sw 60(a1)              */
    if (x != ox || z != oz) {                                     /* Messschrieb RE15_PUSH_LOG (Spur R)  */
        extern void re15_dachkiste_ruhe_log(int, int32_t, int32_t, int32_t, int32_t, int, const int32_t *);
        re15_dachkiste_ruhe_log(p, ox, oz, x, z, s_log_n, s_log);
    }
}

void re15_zellschub_pass_lief(void) { s_pass_lief = 1; }

/* Bilder ohne Spieler-Schub (Klettern, Treppe, Szene, Griff): der Objekt-Pass FUN_8002bd44 laeuft im Original
 * JEDES Bild (@0x8001ce14); hier der Ruhe-Zweig fuer jeden aktiven Eintrag ausser dem Stand-Objekt
 * (@0x8002bef8 -> @0x8002c0ec ohne FUN_8003b558), rueckwaerts wie @0x8002bdb0-c8, danach der Spiegel. */
void re15_zellschub_nach_pass(const re15_rdt_t *rdt)
{
    if (s_pass_lief) { s_pass_lief = 0; return; }
    const re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    const int stand = re15_climb_standing_probe(pl->x, pl->z);   /* DAT_800ac788 */
    for (int p = (int)g_scd.prop_count - 1; p >= 0; p--) {
        if (p >= RE15_SCD_MAX_PROPS || !g_scd.props[p].active) continue;   /* @0x8002bdcc-d8 */
        if (p != stand) re15_zellschub_ruhe(rdt, p);
        re15_zellschub_spiegel(p);                                /* @0x8002c0f0-c10c */
    }
}
