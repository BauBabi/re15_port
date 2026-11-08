# -*- coding: utf-8 -*-
"""NB3-Bau Patch 3: gb_quer_trifft = exakte Nachbildung der Belagerungs-Querung (gb_cross_begin + GBP_CROSS (b)), nur Koerperkontakt."""
import io, sys
P = r"C:/workspace/git/reAi_v2/.claude/worktrees/r41_gatorfix/re15_port/engine/src/enemy_ai_boss_gator.c"
s = io.open(P, encoding="utf-8", newline="").read()
crlf = "\r\n" in s
s = s.replace("\r\n", "\n")
a = s.index("/* Runde 41 Spur G2-RE2 NB3 (Vorgabe A): trifft die geplante Belagerungs-Querung ab Start-x cx0 die Figur?")
b_anchor = "    return treffer;\n}\n"
b = s.index(b_anchor, a) + len(b_anchor)
new = """/* Runde 41 Spur G2-RE2 NB3 (Vorgabe A): rammt die geplante Belagerungs-Querung ab Start-x cx0 die Figur? Nachbildung der Bahn von
 * gb_cross_begin + GBP_CROSS mit der Figur oben, Bild fuer Bild: Start (cx0 geklemmt auf [GB_RAMP_X0 + GB_RING_M .. GB_RAMP_X1 - 700],
 * Kante +- GB_BAHN_M), erstes Bahn-Ende x = Figur-x geklemmt auf [3350 .. GB_RAMP_X1 - 700] (Lande-Klemme), Dauer len/32 in [90..240],
 * Rampen-Fenster t_ein..t_aus (gb_seg_rect_span, Marge 300), im Fenster Bahn-Ende x = Figur-x geklemmt auf [GB_RAMP_X0 + 700 ..] (b),
 * Lage = cx0 + (cx1 - cx0) t / n, Blick = Sofort-Snap auf (cx1, cz1) (re15_enemy_steer_point 0x800); Treffer = gb_koerper_kontakt wie das
 * Rammen in GBP_CROSS. Gemessen (Harness nb3_oben, Lage R2318/-16300, erster und zweiter Bau): vom Ost-Ende des Kanten-Pendels (x 5625)
 * lief die Bahn 1757 seitlich an der Figur vorbei, und eine Vorpruefung mit dem Maulpunkt <= 1500 ueber die vereinfachte Bahn hielt bei
 * x 5230 einen Grenzfall (Abstand ~1500) fuer einen Treffer, den die echte Bahn (Ende-x 3350 vor dem Fenster, gb_maul_dist in 16er-
 * Schritten) verfehlte - erst die Querung ab dem Anlauf (x 4450) rammte (t603 / t682). Darum hier die Bahn selbst und nur der Koerper. */
static int gb_seg_rect_span(int32_t x0, int32_t z0, int32_t x1, int32_t z1, int32_t bx0, int32_t bz0, int32_t bx1, int32_t bz1, int32_t m, int32_t *t0, int32_t *t1);
static int gb_quer_trifft(int32_t cx0, int von_sued, const re15_actor_t *pl)
{
    const int32_t cz0 = von_sued ? GB_RAMP_Z0 - GB_BAHN_M : GB_RAMP_Z1 + GB_BAHN_M;
    const int32_t cz1 = von_sued ? GB_RAMP_Z1 + GB_BAHN_M : GB_RAMP_Z0 - GB_BAHN_M;
    int32_t cx1 = pl->x, cxf = pl->x, n, t_ein, t_aus;
    if (cx0 < GB_RAMP_X0 + GB_RING_M) cx0 = GB_RAMP_X0 + GB_RING_M;
    if (cx0 > GB_RAMP_X1 - 700)       cx0 = GB_RAMP_X1 - 700;
    if (cx1 < 3350)                   cx1 = 3350;                    /* gb_cross_begin: Lande-x-Klemme */
    if (cx1 > GB_RAMP_X1 - 700)       cx1 = GB_RAMP_X1 - 700;
    if (cxf < GB_RAMP_X0 + 700)       cxf = GB_RAMP_X0 + 700;        /* GBP_CROSS (b): im Rampen-Fenster die volle Breite */
    if (cxf > GB_RAMP_X1 - 700)       cxf = GB_RAMP_X1 - 700;
    {   const int64_t ldx = cx1 - cx0, ldz = cz1 - cz0, l2 = ldx * ldx + ldz * ldz;
        int32_t len = 64; while ((int64_t)len * len < l2 && len < 30000) len += 64;
        n = len / 32; if (n < 90) n = 90; if (n > 240) n = 240; }
    {   int32_t q0, q1;
        if (gb_seg_rect_span(cx0, cz0, cx1, cz1, GB_RAMP_X0, GB_RAMP_Z0, GB_RAMP_X1, GB_RAMP_Z1, 300, &q0, &q1)) {
            t_ein = (int32_t)(((int64_t)n * q0) >> 12); t_aus = (int32_t)(((int64_t)n * q1) >> 12);
        } else { t_ein = 0; t_aus = n; } }
    int treffer = 0; int32_t t_tr = -1;
    for (int32_t t = 1; t <= n; t++) {
        const int32_t ex = (t >= t_ein && t <= t_aus) ? cxf : cx1;
        const int32_t x = cx0 + (int32_t)((int64_t)(ex - cx0) * t / n), z = cz0 + (int32_t)((int64_t)(cz1 - cz0) * t / n);
        const int yaw = ((int)re15_atan2_q12(cz1 - z, ex - x) - 0x400) & 0x0fff;
        if (gb_koerper_kontakt(x, z, yaw, pl->x, pl->z)) { treffer = 1; t_tr = t; break; }
    }
    if (!s_gb_stumm) {   /* Diagnose (gator_boss.log): nur bei Wechsel von Start-x oder Ergebnis */
        static FILE *s_qp = NULL; static int32_t s_qx = -1; static int s_qr = -1;
        if (!s_qp) s_qp = fopen("gator_boss.log", "a");
        if (s_qp && (cx0 != s_qx || treffer != s_qr)) {
            fprintf(s_qp, "QUERPRUEF F%u cx0=%d ende=%d/%d cz0=%d cz1=%d n=%d fenster=%d..%d figur=(%d,%d) -> %d (t %d)\\n",
                    (unsigned)g_engine.frame_count, cx0, cx1, cxf, cz0, cz1, n, t_ein, t_aus, pl->x, pl->z, treffer, t_tr);
            fflush(s_qp); s_qx = cx0; s_qr = treffer;
        }
    }
    return treffer;
}
"""
s = s[:a] + new + s[b:]
io.open(P, "w", encoding="utf-8", newline="").write(s.replace("\n", "\r\n") if crlf else s)
print("OK")
