# -*- coding: utf-8 -*-
"""NB3-Bau Patch 2: (1) gb_wand_dazwischen im Band = Strecke Wurzel->Figur, wenn die Figur naeher als der Maulpunkt ist;
(2) gb_quer_trifft: die Belagerungs-Querung der Vorgabe A startet erst, wenn die geplante Bahn die Figur rammt oder beisst."""
import io, sys
P = r"C:/workspace/git/reAi_v2/.claude/worktrees/r41_gatorfix/re15_port/engine/src/enemy_ai_boss_gator.c"
s = io.open(P, encoding="utf-8", newline="").read()
crlf = "\r\n" in s
s = s.replace("\r\n", "\n")
orig_len = len(s)
def rep(old, new, n=1):
    global s
    c = s.count(old)
    if c != n:
        print("FUNDSTELLE %d statt %d:\n%s" % (c, n, old[:300])); sys.exit(1)
    s = s.replace(old, new)

# (1) gb_wand_dazwischen - gb_punkt_im_band ist erst spaeter definiert -> Vorwaertsdeklaration davor
rep("""static int gb_wand_dazwischen(const re15_actor_t *e, const re15_actor_t *pl)
{
    /* Prueflinie ab MAULPUNKT (Zentrum+2600 voraus) statt ab Zentrum
     * (Biss-Sweep 2026-09-11): die lange Zentrum-Linie streifte schon bei
     * Ecken-Beruehrung die Plattform-Box und blockte legitime Bisse. */
    int32_t fc = re15_cos_q12((int)e->rot_y);
    int32_t fs = re15_sin_q12((int)e->rot_y);
    int32_t kx = e->x + (int32_t)(((int64_t)fc * 2600) >> 12);
    int32_t kz = e->z - (int32_t)(((int64_t)fs * 2600) >> 12);
    return gb_seg_hits_platform(kx, kz, pl->x, pl->z, 0)
        || gb_seg_hits_ramp(kx, kz, pl->x, pl->z, 0);
}
""",
"""static int gb_punkt_im_band(int32_t x, int32_t z);   /* fwd (NB3, definiert bei gb_seg_frei) */
static int gb_wand_dazwischen(const re15_actor_t *e, const re15_actor_t *pl)
{
    /* Prueflinie ab MAULPUNKT (Zentrum+2600 voraus) statt ab Zentrum
     * (Biss-Sweep 2026-09-11): die lange Zentrum-Linie streifte schon bei
     * Ecken-Beruehrung die Plattform-Box und blockte legitime Bisse. */
    int32_t fc = re15_cos_q12((int)e->rot_y);
    int32_t fs = re15_sin_q12((int)e->rot_y);
    int32_t kx = e->x + (int32_t)(((int64_t)fc * 2600) >> 12);
    int32_t kz = e->z - (int32_t)(((int64_t)fs * 2600) >> 12);
    if (!s_gb_nb3_alt && gb_punkt_im_band(pl->x, pl->z)) {
        /* Runde 41 Spur G2-RE2 NB3 (Vorgabe B): steht die Figur im Koerper-Band vor einer Blockkante und naeher an der Wurzel als der
         * Maulpunkt (2600 voraus), liegt der Maulpunkt HINTER ihr im Block ("Punkt innen") und sperrte den Biss fuer immer - gemessen
         * (Harness nb3_haengt, erster Bau): W1000/-20700 Wurzel still an der Klemmlinie (1000,-21484), 784 vor der Figur, 104 s ohne
         * Treffer; W4073/W4200/W3000 96 / 96 / 87 s. Die Wand-Frage ist dann die Strecke Wurzel -> Figur gegen den Block selbst (ein
         * Biss DURCH den Block bleibt unmoeglich); GBP_LUNGE zieht rueckwaerts, bis das Maul wieder vor der Figur liegt (dort seit dem
         * Biss-Sweep 2026-09-11), und trifft mit Maul <= 1500. */
        const int64_t dx = (int64_t)pl->x - e->x, dz = (int64_t)pl->z - e->z;
        if (dx * dx + dz * dz < (int64_t)2600 * 2600)
            return gb_seg_hits_platform(e->x, e->z, pl->x, pl->z, 0)
                || gb_seg_hits_ramp(e->x, e->z, pl->x, pl->z, 0);
    }
    return gb_seg_hits_platform(kx, kz, pl->x, pl->z, 0)
        || gb_seg_hits_ramp(kx, kz, pl->x, pl->z, 0);
}
""")

# (2) gb_quer_trifft nach gb_koerper_kontakt / re15_gator_boss_koerper_kontakt
rep("""int re15_gator_boss_koerper_kontakt(const re15_actor_t *e, int32_t px, int32_t pz) { return e ? gb_koerper_kontakt(e->x, e->z, (int)e->rot_y, px, pz) : 0; }
""",
"""int re15_gator_boss_koerper_kontakt(const re15_actor_t *e, int32_t px, int32_t pz) { return e ? gb_koerper_kontakt(e->x, e->z, (int)e->rot_y, px, pz) : 0; }
/* Runde 41 Spur G2-RE2 NB3 (Vorgabe A): trifft die geplante Belagerungs-Querung ab Start-x cx0 die Figur? Bahn wie gb_cross_begin / GBP_CROSS (b)
 * mit der Figur oben: Start (cx0 geklemmt auf [GB_RAMP_X0 + GB_RING_M .. GB_RAMP_X1 - 700], Kante +- GB_BAHN_M), Ende ueber der Rampe auf der
 * Spur der Figur (x geklemmt auf [GB_RAMP_X0 + 700 .. GB_RAMP_X1 - 700]), Dauer len/32 in [90..240], Blick in Bahnrichtung; Treffer = Rammen
 * (gb_koerper_kontakt, Koerperkreise + 450) oder CROSS-Biss (Maulpunkt 2600 voraus <= 1500). Gemessen (Harness nb3_oben, erster Bau, Lage
 * R2318/-16300): die Querung startete vom Ost-Ende des Kanten-Pendels (x 5625) und lief 1757 seitlich an der Figur vorbei - kein Treffer in
 * 240 Bildern, erst die zweite Querung ab dem Anlauf (x 4450) rammte bei t603; mit dieser Vorpruefung wartet er am Anlauf, bis die Bahn trifft. */
static int gb_quer_trifft(int32_t cx0, int von_sued, const re15_actor_t *pl)
{
    const int32_t cz0 = von_sued ? GB_RAMP_Z0 - GB_BAHN_M : GB_RAMP_Z1 + GB_BAHN_M;
    const int32_t cz1 = von_sued ? GB_RAMP_Z1 + GB_BAHN_M : GB_RAMP_Z0 - GB_BAHN_M;
    int32_t cx1 = pl->x;
    if (cx0 < GB_RAMP_X0 + GB_RING_M) cx0 = GB_RAMP_X0 + GB_RING_M;
    if (cx0 > GB_RAMP_X1 - 700)       cx0 = GB_RAMP_X1 - 700;
    if (cx1 < GB_RAMP_X0 + 700)       cx1 = GB_RAMP_X0 + 700;
    if (cx1 > GB_RAMP_X1 - 700)       cx1 = GB_RAMP_X1 - 700;
    int32_t n;
    {   const int64_t ldx = cx1 - cx0, ldz = cz1 - cz0, l2 = ldx * ldx + ldz * ldz;
        int32_t len = 64; while ((int64_t)len * len < l2 && len < 30000) len += 64;
        n = len / 32; if (n < 90) n = 90; if (n > 240) n = 240; }
    const int yaw = ((int)re15_atan2_q12(cz1 - cz0, cx1 - cx0) - 0x400) & 0x0fff;
    const int32_t fc = re15_cos_q12(yaw), fs = re15_sin_q12(yaw);
    for (int32_t t = 0; t <= n; t += 4) {
        const int32_t x = cx0 + (int32_t)((int64_t)(cx1 - cx0) * t / n), z = cz0 + (int32_t)((int64_t)(cz1 - cz0) * t / n);
        if (gb_koerper_kontakt(x, z, yaw, pl->x, pl->z)) return 1;
        {   const int64_t mx = (int64_t)pl->x - (x + (int32_t)(((int64_t)fc * 2600) >> 12)), mz = (int64_t)pl->z - (z - (int32_t)(((int64_t)fs * 2600) >> 12));
            if (mx * mx + mz * mz <= (int64_t)1500 * 1500) return 1; }
    }
    return 0;
}
""")

# (3) GUARD: Zaehler-Querung nur, wenn die Bahn ab dem Anlauf trifft; Start erst, wenn sie ab der aktuellen Lage trifft
rep("""                        if (g->oben_ohne >= GB_OBEN_OHNE) { zielzone = !zg2; quer_frei = 1; }
                        else if (g->cross_cd > 0) zielzone = zg2;
                    }
""",
"""                        int32_t rxa = pl->x;   /* Anlauf-x der QUER-CROSS (s. u.) = bester Start */
                        if (rxa < GB_RAMP_X0 + GB_RING_M) rxa = GB_RAMP_X0 + GB_RING_M;
                        if (rxa > GB_RAMP_X1 - 700)       rxa = GB_RAMP_X1 - 700;
                        if (g->oben_ohne >= GB_OBEN_OHNE && gb_quer_trifft(rxa, zg2 == 0, pl)) { zielzone = !zg2; quer_frei = 1; }
                        else if (g->cross_cd > 0) zielzone = zg2;
                    }
""")
rep("""                        {
                            int64_t adx = e->x - rx, adz = e->z - rz_ein;
                            if (adx * adx + adz * adz < (int64_t)1400 * 1400) {
                                gb_cross_begin(e, g, zg2 == 0, pl->x);
                                g->cross_oben = 1;   /* Belagerungs-Querung */
""",
"""                        {
                            int64_t adx = e->x - rx, adz = e->z - rz_ein;
                            if (adx * adx + adz * adz < (int64_t)1400 * 1400
                                && (!quer_frei || gb_quer_trifft(e->x, zg2 == 0, pl))) {   /* NB3 (Vorgabe A): die Zaehler-Querung erst, wenn die Bahn von hier trifft */
                                gb_cross_begin(e, g, zg2 == 0, pl->x);
                                g->cross_oben = 1;   /* Belagerungs-Querung */
""")

assert len(s) > orig_len, (len(s), orig_len)
io.open(P, "w", encoding="utf-8", newline="").write(s.replace("\n", "\r\n") if crlf else s)
print("OK", orig_len, "->", len(s))
