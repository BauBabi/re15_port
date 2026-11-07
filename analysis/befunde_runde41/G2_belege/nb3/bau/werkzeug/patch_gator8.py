# -*- coding: utf-8 -*-
"""NB3-Bau Patch 8: Figur IM Block-Rechteck auf Wasserhoehe (nur im Defektfall: Biss-Wegstoss in den Pier) = unerreichbar wie die Ostnische -> Belagerung."""
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
/* NB3 (Vorgabe B): die Figur steht IM Block-Rechteck (Plattform/Rampe) auf Wasserhoehe (y >= -900). Fuer die Figur ist das Rechteck nicht begehbar
 * (test_r41_gatorfix begehbar 2091 Band 0/1: Raster x <= 2200, z -20200..-12200 "weggeschoben"), beim Klettern springen y und z im selben Bild
 * (y 0 -> -1800, z -18750 -> -17602, exe probe4 F82) - der Zustand kommt nur im Defektfall vor: exe wand_elza (NB3) F7920 Biss-Wegstoss aus der
 * Taschen-Ecke (2318,-18568) in den Pier (1382,-16749) -> (1382,-17036), 488 Bilder bis zum Laufende; in 7 Laeufen vor/nach NB3 sonst 0 Bilder.
 * Dort gilt "Punkt innen" zu Recht, die NB5-Verfolgung pendelte an der SW-Ecke (2 Haenge-Fenster F8239/F8389, Kopf 9076-11182 von der Figur).
 * Unerreichbar wie die Ostnische -> Belagerung der naechsten Kante (GBP_GUARD). Der Wegstoss selbst (Spieler-Kollision) ist nicht Teil dieser Spur. */
static int gb_pl_im_block(const re15_actor_t *pl)
{
    return pl->y >= -900
        && ((pl->x >= GB_PLAT_X0 && pl->x <= GB_PLAT_X1 && pl->z >= GB_PLAT_Z0 && pl->z <= GB_PLAT_Z1)
            || (pl->x >= GB_RAMP_X0 && pl->x <= GB_RAMP_X1 && pl->z >= GB_RAMP_Z0 && pl->z <= GB_RAMP_Z1));
}
static int gb_pl_unerreichbar(const re15_actor_t *pl) { return gb_pl_nische(pl) || gb_pl_im_block(pl); }
""")
rep("""           || gb_pl_nische(pl);
}
""",
"""           || gb_pl_unerreichbar(pl);
}
""")
rep("        if (!gb_nb3_aus() && gb_pl_nische(pl)) player_on_platform = 1;   /* NB3 (Vorgabe B): Ostnische = belagern wie oben */",
    "        if (!gb_nb3_aus() && gb_pl_unerreichbar(pl)) player_on_platform = 1;   /* NB3 (Vorgabe B): Ostnische / Figur im Block = belagern wie oben */")
rep("        if (!gb_nb3_aus() && gb_pl_nische(pl)) oben = 1;   /* NB3 (Vorgabe B): Ostnische = belagern wie oben */",
    "        if (!gb_nb3_aus() && gb_pl_unerreichbar(pl)) oben = 1;   /* NB3 (Vorgabe B): Ostnische / Figur im Block = belagern wie oben */")
io.open(P, "w", encoding="utf-8", newline="").write(s.replace("\n", "\r\n") if crlf else s)
print("OK crlf=%d" % crlf)
