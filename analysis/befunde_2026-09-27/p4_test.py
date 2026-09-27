import sys, os
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from elza_ed import rep

T = "re15_port/tests/unit/test_savedata.c"
rep(T, [
(
"""    g_gameflow.character = 1;""",
"""    /* ELZA = 4, nicht 1 — DAT_800ACA5C traegt den PLD-Index (@0x801024cc
     * `ori v0,zero,0x4` + @0x801024d4 `sb`). Der Haken bleibt derselbe
     * Rundlauf-Pin, prueft ihn aber jetzt mit einem Wert, den das Spiel
     * wirklich annimmt. */
    g_gameflow.character = 4;"""
),
(
"""    if (g_gameflow.character != 1) { fprintf(stderr, "FAIL(char)\\n"); fail = 1; }""",
"""    if (g_gameflow.character != 4) { fprintf(stderr, "FAIL(char)\\n"); fail = 1; }"""
),
])
