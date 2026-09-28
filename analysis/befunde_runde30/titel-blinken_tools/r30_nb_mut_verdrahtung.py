#!/usr/bin/env python3
"""r30_nb_mut_verdrahtung.py - Runde 30 / Thema D, Nachbesserung: MUTATIONSPROBE.

Nimmt in platform/pc/main.c (Stand nach dem Bau) NUR die Verdrahtung zurueck - ein Pulsschritt je
BILD, Einblende-Tick aus tblink >> 1, Pulsschritt auch im Bestaetigungs-Fade - und laesst
engine/src/title_pulse.c unveraendert. Damit laesst sich zeigen:
  r30_titel_blinken           (bindet nur re15_engine)   -> bleibt GRUEN  (der Mangel)
  integration_r30_titel_puls  (echte re15_pc.exe)        -> wird ROT
NIE committen; danach: git checkout -- re15_port/platform/pc/main.c
Aufruf: r30_nb_mut_verdrahtung.py <pfad zu main.c>
"""
import sys
p = sys.argv[1]
s = open(p, encoding="utf-8", newline="").read()
nl = "\r\n" if "\r\n" in s else "\n"
def rep(a, b):
    global s
    a = a.replace("\n", nl); b = b.replace("\n", nl)
    assert s.count(a) == 1, a
    s = s.replace(a, b)
rep("""              if (tpass0) {
                  tpass0 = 0;
                  re15_title_clock_start(&tclock, t_now);
                  re15_title_pulse_step();                 /* Durchgang 0 */
                  due = 1;
              } else {
                  due = re15_title_clock_poll(&tclock, t_now);
                  re15_title_pulse_advance(due);
                  tfade_tick += due;
              }
""", """              if (tpass0) { tpass0 = 0; re15_title_clock_start(&tclock, t_now); }
              re15_title_pulse_step(); due = 1; tfade_tick = tblink >> 1;   /* MUTATION */
""")
rep("""                                pc_title_pulse_log(pc_now_us(), 0, 1, 0);
""", """                                re15_title_pulse_step();   /* MUTATION */
                                pc_title_pulse_log(pc_now_us(), 0, 1, 0);
""")
open(p, "w", encoding="utf-8", newline="").write(s)
print("mutiert")
