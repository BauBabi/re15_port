#!/usr/bin/env python3
"""r30_nb_alt_schiene.py - Runde 30 / Thema D, Nachbesserung: GEGENPROBE am Ausgangsstand.

Ruestet einen Baum auf dem Ausgangsstand master d98e9639 (Titel-Puls noch im Zeichner, ein
Schritt je Aufruf von re15_render_pc_title_menu) mit GENAU der Messinfrastruktur nach, die der
Integrationstest integration_r30_titel_puls (tests/integration/test_r30_titel_puls.cmake) braucht
- und mit NICHTS sonst. Das Verhalten des alten Standes bleibt unberuehrt:

  1. render_pc.c: zwei Lesefunktionen fuer den alten Pulszustand (s_tmoji_pulse_ctr/_val).
  2. main.c: die Messschiene RE15_TITLE_PULSE_LOG im selben Zeilenformat wie der neue Stand
       "<zeit_us> <zaehler> <pulswert> <faellige> <phase> <einblende_tick> <einblende_B>"
     - Titel-Schleife: je Bild nach re15_render_pc_title_menu (dort lief der Schritt), faellig = 1,
       Einblende-Tick = tblink >> 1 und B = 255 - tick*8 (genau die alte Rechnung).
     - Bestaetigungs-Fade: je Bild nach dem Neuzeichnen, Phase 1.
  3. main.c: der Zeit-Testhaken RE15_TITLE_CONFIRM_MS, wortgleich zum neuen Stand.

Aufruf:  r30_nb_alt_schiene.py <baum>        (bricht ab, wenn ein Anker nicht GENAU einmal passt)
Danach:  cmake --build <baum>/re15_port/<bauverz> --target re15_pc
         cmake -DRE15_PC_EXE=<exe> -DWORKDIR=<dir> -P tests/integration/test_r30_titel_puls.cmake
"""
import os, sys

def patch(path, pairs):
    src = open(path, encoding="utf-8", newline="").read()
    crlf = "\r\n" in src                      # Arbeitsbaum mit core.autocrlf: Zeilenende der Datei
    for anchor, repl in pairs:
        if crlf:
            anchor, repl = anchor.replace("\n", "\r\n"), repl.replace("\n", "\r\n")
        n = src.count(anchor)
        if n != 1:
            sys.exit("Anker %d-mal statt 1-mal in %s:\n%s" % (n, path, anchor))
        src = src.replace(anchor, repl)
    open(path, "w", encoding="utf-8", newline="").write(src)
    print("gepatcht:", path)

def main():
    baum = sys.argv[1]
    rp = os.path.join(baum, "re15_port", "platform", "pc", "src", "render_pc.c")
    mp = os.path.join(baum, "re15_port", "platform", "pc", "main.c")

    patch(rp, [(
        "void re15_render_pc_hide_title_menu(void) { s_tmoji_show = 0; }\n",
        "void re15_render_pc_hide_title_menu(void) { s_tmoji_show = 0; }\n"
        "/* R30-GEGENPROBE (Messinfrastruktur, kein Verhalten): alter Pulszustand lesen */\n"
        "int r30_alt_puls_ctr(void) { return s_tmoji_pulse_ctr; }\n"
        "int r30_alt_puls_val(void) { return s_tmoji_pulse_val; }\n",
    )])

    schiene = (
        "/* R30-GEGENPROBE (Messinfrastruktur, kein Verhalten): Messschiene RE15_TITLE_PULSE_LOG im\n"
        " * Zeilenformat des neuen Standes, am ALTEN Pulszustand. */\n"
        "static uint64_t r30_alt_now_us(void)\n"
        "{\n"
        "    uint64_t c = (uint64_t) SDL_GetPerformanceCounter();\n"
        "    uint64_t f = (uint64_t) SDL_GetPerformanceFrequency();\n"
        "    if (f == 0) return (uint64_t) SDL_GetTicks() * 1000ull;\n"
        "    return (c / f) * 1000000ull + ((c % f) * 1000000ull) / f;\n"
        "}\n"
        "static void r30_alt_log(int phase, unsigned tk, int B)\n"
        "{\n"
        "    extern int r30_alt_puls_ctr(void); extern int r30_alt_puls_val(void);\n"
        "    static FILE *lf; static int init;\n"
        "    if (!init) { init = 1; const char *p = getenv(\"RE15_TITLE_PULSE_LOG\"); if (p && *p) lf = fopen(p, \"w\"); }\n"
        "    if (!lf) return;\n"
        "    fprintf(lf, \"%llu %d %d %d %d %u %d\\n\", (unsigned long long) r30_alt_now_us(), r30_alt_puls_ctr(),\n"
        "            r30_alt_puls_val(), 1, phase, phase ? 0u : tk, phase ? 0 : B);\n"
        "    fflush(lf);\n"
        "}\n\n"
    )
    haken = (
        "            { static int s_tc_init, s_tc_done; static uint64_t s_tc_ms, s_tc_t0;\n"
        "              if (!s_tc_init) {\n"
        "                  const char *e = getenv(\"RE15_TITLE_CONFIRM_MS\");\n"
        "                  s_tc_init = 1;\n"
        "                  s_tc_ms   = (e && *e) ? (uint64_t) strtoull(e, NULL, 10) : 0;\n"
        "                  s_tc_done = (s_tc_ms == 0);\n"
        "                  s_tc_t0   = r30_alt_now_us();\n"
        "              }\n"
        "              if (!s_tc_done && r30_alt_now_us() - s_tc_t0 >= s_tc_ms * 1000ull) {\n"
        "                  s_tc_done = 1;\n"
        "                  pp |= RE15_PAD_BIT_CROSS;\n"
        "              } }\n"
    )
    patch(mp, [
        ("int main(int argc, char *argv[])\n{", schiene + "int main(int argc, char *argv[])\n{"),
        ("                if (tblink == 16) pp |= RE15_PAD_BIT_CROSS;  /* confirm LOAD GAME -> load screen */\n"
         "            }\n",
         "                if (tblink == 16) pp |= RE15_PAD_BIT_CROSS;  /* confirm LOAD GAME -> load screen */\n"
         "            }\n" + haken),
        ("            re15_render_pc_title_menu(&s_tmoji, cursor);\n\n",
         "            re15_render_pc_title_menu(&s_tmoji, cursor);\n"
         "            { unsigned tk = tblink >> 1; int B = 255 - (int) tk * 8; if (B < 0) B = 0;\n"
         "              r30_alt_log(0, tk, B); }\n\n"),
        ("re15_render_pc_title_menu(&s_tmoji, cursor);   /* Redraw @0x80102d10 */\n",
         "re15_render_pc_title_menu(&s_tmoji, cursor);   /* Redraw @0x80102d10 */\n"
         "                                r30_alt_log(1, 0, 0);\n"),
    ])

if __name__ == "__main__":
    main()
