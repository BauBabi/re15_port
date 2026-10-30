/*
 * RE1.5 Rebuilt — Raumlicht (Kopf, Herkunft und Grenzen: include/re15_raumlicht.h).
 * ⛔ Port-Ergaenzung nach dem 80%-Video; Messwerte stehen bei den Tabellen.
 */
#include "re15_raumlicht.h"
#include <stdlib.h>

/* ROOM50B0 Cut 2: Rueckprojektion des Bandes (PSX-Bild x 5..95, y 182..171) durch die Kamera
 * (-12232,-1600,-21992) H 208 auf y = 0 (Spawns des Raums y = 0): (-28600,-13275) .. (-31450,-5975).
 * Rot-Ueberschuss im Video bis ~130 (R) bei kaum Gruen, etwas Blau = rosa-rot; Bandhoehe im Bild ~18 px
 * -> halbe Tiefe 2600 (die X-Ausdehnung laeuft hier in Blickrichtung). Blob-Mitte 96/255 x 200 = +75. */
static const re15_raumlicht_t s_container[] = {
    { -26800, 0, -13600, 3600, 2200, 210, 26, 66 },
    { -28000, 0, -10200, 3600, 2200, 210, 26, 66 },
    { -29200, 0,  -6800, 3600, 2200, 210, 26, 66 },
};

/* ROOM2000 Cut 2: Glanzpunkte (PSX-Bild (185,158) (161,148) (139,147) (124,137)) durch die Kamera
 * (1926,-4590,-25164) H 208 auf y = 0 (Zombie-Spawns im Kanal y = 0). Dazwischen aufgefuellt. */
static const int32_t s_glanz[][2] = {
    { -6300, -21450 }, { -7000, -21800 }, { -7725, -22200 }, { -8200, -23300 },
    { -9000, -23550 }, { -9900, -23875 },
};

static uint32_t rl_hash(uint32_t v)
{
    v ^= v >> 16; v *= 0x7feb352du; v ^= v >> 15; v *= 0x846ca68bu; v ^= v >> 16;
    return v;
}

int re15_raumlicht_liste(unsigned room, uint32_t frame, re15_raumlicht_t *out, int max)
{
    static int aus = -1;
    if (aus < 0) aus = getenv("RE15_KEIN_RAUMLICHT") ? 1 : 0;
    if (aus || !out || max <= 0) return 0;
    int n = 0;
    switch (room & 0xFFFEu) {
    case 0x50B0u:
        for (unsigned i = 0; i < sizeof s_container / sizeof s_container[0] && n < max; i++)
            out[n++] = s_container[i];
        break;
    case 0x2000u:
        /* Gestaltung: je Glanzpunkt ein 4-Bild-Fenster, in ~1 von 12 Fenstern leuchtet er (im Video
         * meist ein, selten zwei Punkte gleichzeitig, je wenige Bilder lang). */
        for (unsigned i = 0; i < sizeof s_glanz / sizeof s_glanz[0] && n < max; i++) {
            uint32_t fenster = frame / 4u;
            if (rl_hash(fenster * 31u + i * 977u) % 12u != 0) continue;
            re15_raumlicht_t l = { s_glanz[i][0], 0, s_glanz[i][1], 260, 260, 225, 230, 230 };
            out[n++] = l;
        }
        break;
    default: break;
    }
    return n;
}
