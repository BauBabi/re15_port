# -*- coding: utf-8 -*-
"""NB3-Bau Patch 5: Helfer gb_pl_oben (Figur oben auf Rampe/Plattform oder in der Ostnische); Rammen ohne Kanister nur dann."""
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

rep("""static int gb_pl_nische(const re15_actor_t *pl)
{
    return pl->x > GB_RAMP_X1 && pl->z >= GB_RAMP_Z0 && pl->z <= GB_RAMP_Z1;
}
""",
"""static int gb_pl_nische(const re15_actor_t *pl)
{
    return pl->x > GB_RAMP_X1 && pl->z >= GB_RAMP_Z0 && pl->z <= GB_RAMP_Z1;
}
/* NB3: die Figur ist OBEN (Rampe/Plattform mit y < -900 wie player_on_platform im CHASE) oder in der Ostnische - der Zaehler der Vorgabe A
 * und das Rammen ohne Kanister gelten nur ihr; eine Figur im Wasser wird in der Verfolgungs-Querung wie in NB5 nur vom Maul getroffen. */
static int gb_pl_oben(const re15_actor_t *pl)
{
    return ((pl->y < -900)
            && ((pl->x >= GB_PLAT_X0 && pl->x <= GB_PLAT_X1 && pl->z >= GB_PLAT_Z0 && pl->z <= GB_PLAT_Z1)
                || (pl->x >= GB_RAMP_X0 && pl->x <= GB_RAMP_X1 && pl->z >= GB_RAMP_Z0 && pl->z <= GB_RAMP_Z1)))
           || gb_pl_nische(pl);
}
""")

rep("""        const int oben_jetzt = ((pl->y < -900)
                                && ((pl->x >= GB_PLAT_X0 && pl->x <= GB_PLAT_X1 && pl->z >= GB_PLAT_Z0 && pl->z <= GB_PLAT_Z1)
                                    || (pl->x >= GB_RAMP_X0 && pl->x <= GB_RAMP_X1 && pl->z >= GB_RAMP_Z0 && pl->z <= GB_RAMP_Z1)))
                               || gb_pl_nische(pl);
        if (!oben_jetzt) g->oben_ohne = 0;""",
"""        if (!gb_pl_oben(pl)) g->oben_ohne = 0;""")

rep("""        if ((re15_gr2_rw_traegt() || !s_gb_nb3_alt)   /* NB3 (Vorgabe A): Rammen auch ohne Kanister - "den Spieler oben auf der Rampe auch rammen koennen" */""",
"""        if ((re15_gr2_rw_traegt() || (!s_gb_nb3_alt && gb_pl_oben(pl)))   /* NB3 (Vorgabe A): Rammen auch ohne Kanister, wenn die Figur OBEN steht - "den Spieler oben auf
                                                                             * der Rampe auch rammen koennen"; Figur im Wasser: wie NB5 nur der CROSS-Biss unten */""")

io.open(P, "w", encoding="utf-8", newline="").write(s.replace("\n", "\r\n") if crlf else s)
print("OK crlf=%d" % crlf)
