# -*- coding: utf-8 -*-
"""NB3-Bau Patch 6: Band-Regel nur im CHASE-Zweig 7 (Figur im Band), gb_kanten_umweg(direkt) statt Zielpruefung; Zweig 7 ohne Vorhalte."""
import io, sys
P = r"C:/workspace/git/reAi_v2/.claude/worktrees/r41_gatorfix/re15_port/engine/src/enemy_ai_boss_gator.c"
s = io.open(P, encoding="utf-8", newline="").read()
crlf = "\r\n" in s
s = s.replace("\r\n", "\n")
def rep(old, new, n=1):
    global s
    c = s.count(old)
    if c != n:
        print("FUNDSTELLE %d statt %d:\n%s" % (c, n, old[:300])); sys.exit(1)
    s = s.replace(old, new)

rep("""static int gb_kanten_umweg(const re15_actor_t *e, int32_t *tx, int32_t *tz)
{""",
"""static int gb_kanten_umweg(const re15_actor_t *e, int32_t *tx, int32_t *tz, int direkt)
{""")
rep("""    /* NB3 (Vorgabe B): Ziel im Koerper-Band vor einer Blockkante -> die Strecken ZUM ZIEL gegen den Block selbst (m 0) statt
     * "Punkt innen" (s. gb_punkt_im_band); Start-/Eckenstrecken behalten GB_KOERPER_M. */
    const int32_t mz = (!s_gb_nb3_alt && gb_punkt_im_band(*tx, *tz)) ? 0 : GB_KOERPER_M;
    if (gb_seg_frei(e->x, e->z, *tx, *tz, mz)) return -1;
    for (i = 0; i < 4; i++) { kx[i] = gb_ecke[i][0]; kz[i] = gb_ecke[i][1]; }
    kx[4] = *tx; kz[4] = *tz;
    for (i = 0; i < NK; i++) {
        fest[i] = 0; prev[i] = -2;
        if (gb_seg_frei(e->x, e->z, kx[i], kz[i], i == 4 ? mz : GB_KOERPER_M)) {
""",
"""    /* NB3 (Vorgabe B): direkt = der CHASE-Zweig 7 hat die Figur im Koerper-Band vor einer Blockkante und die Strecke gegen den Block
     * selbst frei befunden - kein Umweg ("Punkt innen" der 1800er-Pruefung war der Haenger, s. gb_punkt_im_band). Nur dort: die
     * Pruefung am ZIEL statt an der Figur liess den VORHALTE-Punkt (Figur vom Koerper geschoben, 20 Bilder Vorhalte) im Band greifen
     * und wich in der Westkanal-Lage (-6000,-16000) von NB5 ab (Riegel gang, erster Bau: 1944 von 2700 Bildern). */
    if (direkt) return -1;
    if (gb_seg_frei(e->x, e->z, *tx, *tz, GB_KOERPER_M)) return -1;
    for (i = 0; i < 4; i++) { kx[i] = gb_ecke[i][0]; kz[i] = gb_ecke[i][1]; }
    kx[4] = *tx; kz[4] = *tz;
    for (i = 0; i < NK; i++) {
        fest[i] = 0; prev[i] = -2;
        if (gb_seg_frei(e->x, e->z, kx[i], kz[i], GB_KOERPER_M)) {
""")
rep("""            if (fest[j] || !gb_seg_frei(kx[u], kz[u], kx[j], kz[j], j == 4 ? mz : GB_KOERPER_M))
                continue;
""",
"""            if (fest[j] || !gb_seg_frei(kx[u], kz[u], kx[j], kz[j], GB_KOERPER_M))
                continue;
""")
# Zweig 7: ohne Vorhalte
rep("""                if (!s_gb_nb3_alt && gb_punkt_im_band(zp->x, zp->z) && gb_seg_frei(e->x, e->z, zp->x, zp->z, 0))
                    { g->dbg_zweig = 7; }   /* NB3 (Vorgabe B): Figur im Band vor der Kante, Strecke gegen den Block frei -> direkt */""",
"""                if (!s_gb_nb3_alt && gb_punkt_im_band(zp->x, zp->z) && gb_seg_frei(e->x, e->z, zp->x, zp->z, 0))
                    { tx = zp->x; tz = zp->z; g->dbg_zweig = 7; }   /* NB3 (Vorgabe B): Figur im Band vor der Kante, Strecke gegen den Block frei -> direkt,
                                                                      * ohne Vorhalte (der Vorhalte-Punkt laege im Band oder im Block) */""")
# Aufrufer
rep("""            int ubi = gb_kanten_umweg(e, &tx, &tz);
            if (ubi >= 0) g->dbg_zweig = (int8_t)(10 + ubi);""",
"""            int ubi = gb_kanten_umweg(e, &tx, &tz, g->dbg_zweig == 7);   /* NB3: Zweig 7 = direkt (s. gb_kanten_umweg) */
            if (ubi >= 0) g->dbg_zweig = (int8_t)(10 + ubi);""")
rep("""                int uh = gb_kanten_umweg(e, &gx, &gz);""",
"""                int uh = gb_kanten_umweg(e, &gx, &gz, 0);""")
rep("""                        int bi2 = gb_kanten_umweg(e, &hx, &hz);""",
"""                        int bi2 = gb_kanten_umweg(e, &hx, &hz, 0);""")
rep("""{   (void)gb_kanten_umweg(e, tx, tz);""",
"""{   (void)gb_kanten_umweg(e, tx, tz, 0);""")
assert s.count("gb_kanten_umweg(") == 5, s.count("gb_kanten_umweg(")
io.open(P, "w", encoding="utf-8", newline="").write(s.replace("\n", "\r\n") if crlf else s)
print("OK crlf=%d" % crlf)
