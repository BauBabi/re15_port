# -*- coding: utf-8 -*-
"""NB3-Bau Patch 7: NB3-Regeln nur im Stand 'neu' (Vorschlag 4) - unter dem Pruefhaken v4_alt (Stand f942f56c0, Kanister als Koerper) wie unter nb3_alt aus."""
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
rep("""static int s_gb_nb3_alt = 0;
void re15_gator_boss_nb3_alt_setzen(int an) { s_gb_nb3_alt = an; }
int  re15_gator_boss_nb3_alt(void) { return s_gb_nb3_alt; }
""",
"""static int s_gb_nb3_alt = 0;
void re15_gator_boss_nb3_alt_setzen(int an) { s_gb_nb3_alt = an; }
int  re15_gator_boss_nb3_alt(void) { return s_gb_nb3_alt; }
/* NB3 aus = Stand a661eb8aa (Pruefhaken nb3_alt) ODER Stand f942f56c0 (Pruefhaken re15_gr2_v4_alt: der Kanister zaehlt als Koerper - Kopf-Klemme,
 * Trag-Lenkung, Huelle; die Riegel fuzz/klemm/alligator2 tragen/re2gator laufen so). Dort gilt die NB5-Wegwahl weiter: mit der Kanister-Huelle
 * lief die direkte Anfahrt (Vorgabe B) an die Pier-Westkante in die Kopf-Klemme und endete rueckwaerts an der Westwand (Riegel fuzz, Lage P-mitte:
 * 92 2-s-Fenster ab t673 bei (-7416,-15904), Clip 10). Im Spiel ist kein Pruefhaken gesetzt. */
static int gb_nb3_aus(void) { return s_gb_nb3_alt || re15_gr2_v4_alt(); }
""")
rep("    if (!s_gb_nb3_alt && gb_punkt_im_band(pl->x, pl->z)) {", "    if (!gb_nb3_aus() && gb_punkt_im_band(pl->x, pl->z)) {")
rep("        if (!s_gb_nb3_alt && gb_pl_nische(pl)) player_on_platform = 1;", "        if (!gb_nb3_aus() && gb_pl_nische(pl)) player_on_platform = 1;")
rep("                if (!s_gb_nb3_alt && gb_punkt_im_band(zp->x, zp->z) && gb_seg_frei(e->x, e->z, zp->x, zp->z, 0))",
    "                if (!gb_nb3_aus() && gb_punkt_im_band(zp->x, zp->z) && gb_seg_frei(e->x, e->z, zp->x, zp->z, 0))")
rep("        if (!s_gb_nb3_alt && gb_pl_nische(pl)) oben = 1;", "        if (!gb_nb3_aus() && gb_pl_nische(pl)) oben = 1;")
rep("                    if (s_gb_nb3_alt) {\n", "                    if (gb_nb3_aus()) {\n")
rep("        if ((re15_gr2_rw_traegt() || (!s_gb_nb3_alt && gb_pl_oben(pl)))", "        if ((re15_gr2_rw_traegt() || (!gb_nb3_aus() && gb_pl_oben(pl)))")
assert s.count("!s_gb_nb3_alt") == 0 and s.count("if (s_gb_nb3_alt)") == 0, (s.count("!s_gb_nb3_alt"), s.count("if (s_gb_nb3_alt)"))
# re15_gr2_v4_alt muss vor gb_nb3_aus deklariert sein
if s.find("int  re15_gr2_v4_alt(void);") < 0 and s.find("re15_gator_re2_r41.h") < 0:
    print("WARNUNG: keine Deklaration von re15_gr2_v4_alt gefunden")
io.open(P, "w", encoding="utf-8", newline="").write(s.replace("\n", "\r\n") if crlf else s)
print("OK crlf=%d" % crlf)
