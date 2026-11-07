# -*- coding: utf-8 -*-
"""NB3-Bau Patch 4: Vorpruefung bis Bild n-1 (GBP_CROSS endet bei t >= cframes vor dem Rammen-Test); Anlauf der Zaehler-Querung ohne Kanten-Pendel."""
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

rep("""    int treffer = 0; int32_t t_tr = -1;
    for (int32_t t = 1; t <= n; t++) {
        const int32_t ex = (t >= t_ein && t <= t_aus) ? cxf : cx1;""",
"""    int treffer = 0; int32_t t_tr = -1;
    for (int32_t t = 1; t < n; t++) {   /* GBP_CROSS endet bei t >= cframes VOR dem Rammen-Test: das letzte Bild zaehlt nicht */
        const int32_t ex = (t >= t_ein && t <= t_aus) ? cxf : cx1;""")

rep("""                int pend_z = 0;   /* Pendel-Achse folgt der ZIELART (Sweep-Beleg""",
"""                int quer_anlauf = 0;   /* NB3 (Vorgabe A): Anlauf der Zaehler-Querung ohne Kanten-Pendel (s. u.) */
                int pend_z = 0;   /* Pendel-Achse folgt der ZIELART (Sweep-Beleg""")

rep("""                        gx = rx; gz = rz_ein;   /* Anlauf; Weg via Umweg unten */
                        g->umlauf = 0;          /* kein West-Commit noetig */""",
"""                        gx = rx; gz = rz_ein;   /* Anlauf; Weg via Umweg unten */
                        g->umlauf = 0;          /* kein West-Commit noetig */
                        quer_anlauf = quer_frei;   /* NB3: der Anlauf der Zaehler-Querung ohne das Kanten-Pendel - mit ihm stand das Ziel bis 90 Bilder
                                                    * 1500 oestlich des Anlaufs (Harness R2318/-16300: t300 Zaehler, t378 Start) */""")

rep("""                        gx += g->pend_dir * 1500;
                        if (auf_rampe) {""",
"""                        if (!quer_anlauf) gx += g->pend_dir * 1500;   /* NB3: s. quer_anlauf */
                        if (auf_rampe) {""")

io.open(P, "w", encoding="utf-8", newline="").write(s.replace("\n", "\r\n") if crlf else s)
print("OK crlf=%d" % crlf)
