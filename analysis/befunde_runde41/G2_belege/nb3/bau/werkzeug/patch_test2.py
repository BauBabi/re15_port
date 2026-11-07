# -*- coding: utf-8 -*-
"""NB3-Bau: obenlog zaehlt ab dem Anhaengen des Kanisters (RE2 Zielmodus: der Alligator holt erst den Kanister, kein Angriff auf die Figur oben)."""
import io, sys
P = r"C:/workspace/git/reAi_v2/.claude/worktrees/r41_gatorfix/re15_port/tests/unit/test_r41_gatorfix.c"
s = io.open(P, encoding="utf-8", newline="").read()
crlf = "\r\n" in s
s = s.replace("\r\n", "\n")
def rep(old, new, n=1):
    global s
    c = s.count(old)
    if c != n:
        print("FUNDSTELLE %d statt %d:\n%s" % (c, n, old[:300])); sys.exit(1)
    s = s.replace(old, new)

rep("""    static int8_t ph[40000]; static uint8_t ramm[40000];
    nb3_phasen_lesen(dir, ph, 40000); memset(ramm, 0, sizeof ramm); nb3_boss_zeilen(dir, "RAMMEN", ramm, 40000);
    int oben_n = 0, erst_f = -1, letzt_f = -1, treffer = 0, tr_oben = 0, je[5] = { 0, 0, 0, 0, 0 };
    int erst_offs = -1, erst_tf = -1, pause_max = 0, abschn = 0, verlassen_max = 0, a = -1, letzt_tr = -1;
    const char *erst_art = "-";
    for (int i = 0; i <= n; i++) {
        const int ob = i < n && nb3_oben_grund(&s_nb3_st[i]);""",
"""    static int8_t ph[40000]; static uint8_t ramm[40000];
    nb3_phasen_lesen(dir, ph, 40000); memset(ramm, 0, sizeof ramm); nb3_boss_zeilen(dir, "RAMMEN", ramm, 40000);
    /* RE2 Zielmodus (+0x1C0 & 4 @0x80100a34-5c, Port re15_k2080_r41_anschwimmen): liegt der geworfene Kanister im Becken, holt der Alligator erst ihn -
     * keine Belagerung, kein Angriff (exe probe4: Figur ab F83 oben, Kanister angehaengt F1014, dazwischen CHASE zum Kanister im Nordwesten, Pause 856 Bilder).
     * Das Mass "Figur oben nicht sicher" zaehlt darum ab dem Anhaengen ("KANISTER angehaengt F<n>" in gator_boss.log, wie traglog). */
    int auf = 0;
    {   char pf[1024]; static char zl[4096]; snprintf(pf, sizeof pf, "%s/gator_boss.log", dir);
        FILE *fb = fopen(pf, "r");
        if (fb) { while (fgets(zl, sizeof zl, fb)) { if (strstr(zl, "KANISTER angehaengt")) { const char *q = strrchr(zl, 'F'); if (q && auf == 0) auf = atoi(q + 1); } } fclose(fb); } }
    int oben_n = 0, erst_f = -1, letzt_f = -1, treffer = 0, tr_oben = 0, je[5] = { 0, 0, 0, 0, 0 };
    int erst_offs = -1, erst_tf = -1, pause_max = 0, abschn = 0, verlassen_max = 0, a = -1, letzt_tr = -1, oben_vor_auf = 0;
    const char *erst_art = "-";
    for (int i = 0; i <= n; i++) {
        const int ob_roh = i < n && nb3_oben_grund(&s_nb3_st[i]);
        if (ob_roh && s_nb3_st[i].f < auf) oben_vor_auf++;
        const int ob = ob_roh && s_nb3_st[i].f >= auf;""")
rep("""    printf("obenlog: %d Bilder (F%d..F%d); Figur oben %d Bilder (erstes F%d, letztes F%d, %d Abschnitte); Treffer gesamt %d, oben %d (Rammen %d, CROSS-Biss %d, Hochbiss %d, Biss %d, sonst %d); "
           "erster Treffer oben nach %d Bildern (%s F%d); laengste Pause ohne Treffer oben %d Bilder; ohne Treffer verlassener Abschnitt bis %d Bilder -> %s (Schranke %d)\\n",
           n, s_nb3_st[0].f, s_nb3_st[n - 1].f, oben_n, erst_f, letzt_f, abschn, treffer, tr_oben, je[1], je[2], je[3], je[4], je[0], erst_offs, erst_art, erst_tf, pause_max, verlassen_max,
           ok ? "ok" : "VERFEHLT", NB3_PAUSE_MAX);""",
"""    printf("obenlog: %d Bilder (F%d..F%d); Kanister angehaengt F%d (gezaehlt ab dann, davor %d Bilder oben); Figur oben %d Bilder (erstes F%d, letztes F%d, %d Abschnitte); Treffer gesamt %d, "
           "oben %d (Rammen %d, CROSS-Biss %d, Hochbiss %d, Biss %d, sonst %d); erster Treffer oben nach %d Bildern (%s F%d); laengste Pause ohne Treffer oben %d Bilder; "
           "ohne Treffer verlassener Abschnitt bis %d Bilder -> %s (Schranke %d)\\n",
           n, s_nb3_st[0].f, s_nb3_st[n - 1].f, auf, oben_vor_auf, oben_n, erst_f, letzt_f, abschn, treffer, tr_oben, je[1], je[2], je[3], je[4], je[0], erst_offs, erst_art, erst_tf, pause_max,
           verlassen_max, ok ? "ok" : "VERFEHLT", NB3_PAUSE_MAX);""")
io.open(P, "w", encoding="utf-8", newline="").write(s.replace("\n", "\r\n") if crlf else s)
print("OK crlf=%d" % crlf)
