# -*- coding: utf-8 -*-
"""NB3 Bauer Patch 9 (so angewandt, CRLF-erhaltend): Zaehler oben_ohne nur in den Kampfphasen (gb_phase_kampf) - Querbezug integration_r41_re2gator Lauf belagerung."""
import io
p = r"C:/workspace/git/reAi_v2/.claude/worktrees/r41_gatorfix/re15_port/engine/src/enemy_ai_boss_gator.c"
raw = io.open(p, "rb").read()
crlf = b"\r\n" in raw
t = raw.decode("utf-8").replace("\r\n", "\n")
a1 = "int  re15_gator_boss_oben_ohne(const re15_actor_t *e)"
assert t.count(a1) == 1
f1 = ('/* NB3 (Vorgabe A): Phasen, in denen der Zaehler oben_ohne laeuft - nur im Kampf kann er die Figur oben angreifen. Nicht gezaehlt: OFF, LURK\n'
      ' * (Lauerstellung an der Leiter, Punkt 2), FRESSEN/DIE/DEAD/RE2_TOD und das RE2-Intro (GBP_RE2_INTRO, Durchbruch -> Schnapper, ~300 Bilder ab dem\n'
      ' * Betreten). Gemessen (exe-Riegel integration_r41_re2gator, Lauf belagerung: Figur ab F0 auf der Rampe (6300,-16250), Intro bis F305): mit dem\n'
      ' * Zaehler im Intro quert er im 71. Bild der Belagerung (QUERPRUEF F305 t 1, CROSS F376, Rammen F377) VOR dem ersten Hochbiss (a661eb8aa F417), die\n'
      ' * GIER-WACHE (300 Bildpaare CHASE/GUARD) wird nie erreicht und der Finisher kommt F751 statt F627; nur im Kampf gezaehlt laeuft die Belagerung\n'
      ' * wie im Stand a661eb8aa (Hochbiss F417/F522/F627 nullt den Zaehler). Harness (Lagen A1/B1) und exe ohne Intro-Anteil unveraendert. */\n'
      'static int gb_phase_kampf(int ph)\n'
      '{\n'
      '    return ph == GBP_CHASE || ph == GBP_LUNGE || ph == GBP_FLINCH || ph == GBP_GUARD || ph == GBP_CROSS_APPR || ph == GBP_CROSS\n'
      '        || ph == GBP_RE2_DASH;\n'
      '}\n')
t = t.replace(a1, f1 + a1)
a2 = ("        if (!gb_pl_oben(pl)) g->oben_ohne = 0;\n"
      "        else if (g->oben_ohne < 30000) g->oben_ohne++;\n")
assert t.count(a2) == 1
b2 = ("        if (!gb_pl_oben(pl) || !gb_phase_kampf(g->phase)) g->oben_ohne = 0;   /* nur im Kampf gezaehlt (s. gb_phase_kampf) */\n"
      "        else if (g->oben_ohne < 30000) g->oben_ohne++;\n")
t = t.replace(a2, b2)
a3 = "         * gb_biss_abschluss; nicht oben nullt. Ab GB_OBEN_OHNE quert die Belagerung ueber die Figur (GBP_GUARD). */\n"
assert t.count(a3) == 1
t = t.replace(a3, "         * gb_biss_abschluss; nicht oben und jede Nicht-Kampfphase (RE2-Intro, Lauern, Tod) nullt. Ab GB_OBEN_OHNE quert die Belagerung\n"
                  "         * ueber die Figur (GBP_GUARD). */\n")
out = t.replace("\n", "\r\n") if crlf else t
io.open(p, "wb").write(out.encode("utf-8"))
print("gepatcht crlf=%d" % crlf)
