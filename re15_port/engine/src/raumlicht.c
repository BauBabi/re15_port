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

/* ROOM2000/2001: KEIN Eintrag (Runde 41 Spur W, Dossier analysis/befunde_runde41/W_wasserblink.md).
 * Der Fall 0x2000 aus Commit 4100d2aed liess an sechs festen Kanalpunkten Glanzpunkte in 4-Bild-Fenstern per Hash
 * aufblitzen; Takt, Zahl, Groesse und Farbe hatten keine Adresse. Nutzer 2026-11-01: "Das blinken im Wasser ist auf
 * jeden Fall fehlerhaft und quatsch". Das Original setzt keinen Effekt auf das Kanalwasser: ROOM2000 main00
 * @0x0174C/@0x0175C/@0x0176C = Effekt 0x0B an den Gitterstrahlen, sub07 @0x01B06..@0x01B94 = Effekt 6 Sub 0 bei
 * y -10000 (Version_40%: @0x01568/@0x01578/@0x01588, sub03 @0x015B6..@0x01644). Die weissen Wolken im 80%-Video
 * (V1 2:06-2:15 = Complete Disc Video 2, 5..21 s) sind Spritzer an den Fuessen watender Zombies, keine ortsfesten
 * Lichter. Die Wasserflaechen (Obj_model_set Typ 3 @0x016E6/@0x01708/@0x0172A) und der Wasserlauf der Spur N bleiben. */

int re15_raumlicht_liste(unsigned room, uint32_t frame, re15_raumlicht_t *out, int max)
{
    static int aus = -1;
    (void)frame;
    if (aus < 0) aus = getenv("RE15_KEIN_RAUMLICHT") ? 1 : 0;
    if (aus || !out || max <= 0) return 0;
    int n = 0;
    switch (room & 0xFFFEu) {
    case 0x50B0u:
        for (unsigned i = 0; i < sizeof s_container / sizeof s_container[0] && n < max; i++)
            out[n++] = s_container[i];
        break;
    default: break;
    }
    return n;
}
