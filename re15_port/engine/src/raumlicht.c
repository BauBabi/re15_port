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
 * jeden Fall fehlerhaft und quatsch". Die weissen Wolken im 80%-Video (V1 2:06-2:15 = Complete Disc Video 2,
 * 5..21 s) sind Spritzer an den Fuessen watender Zombies, keine ortsfesten Lichter.
 * Was die Raumdaten im Kanal setzen (berichtigt in Nachbesserung 1, Dossier Abschnitt 2.1):
 *   - Effekt 0x0B an den Gitterstrahlen: main00 @0x0174C/@0x0175C/@0x0176C (Version_40%: @0x01568/@0x01578/@0x01588).
 *   - Effekt 6 Sub 0 ist ein FALLENDER DECKENTROPFEN mit Aufschlag-Spritzer, kein Licht. ESP-Zeilen (Repo @0x14A0C):
 *     Zeile 0 Routine 10 @0x800176b0, Zeile 1 Routine 3 @0x80017334 (9 Bilder), Zeile 2 Routine 13 @0x80017990 mit
 *     accel (0,10,0) @0x14A64 (Version_40%: @0x027B4); Routine 13 prueft den Boden (`jal 0x8001c6e8` @0x800179c4)
 *     und setzt dann die Aufschlag-Anim +0x6e @0x80017a28 (Repo 0x26, Version_40% 0x0A). Acht Punkte bei y -10000,
 *     585 Bilder je Umlauf: Repo sub07 @0x01B06..@0x01BAE (ROOM2001 sub08 @0x01D32).
 *   - Repo-Baum (Mod): main00 startet die Schleife unbedingt, @0x0177C `04 ff 18 07` (ROOM2001 `04 ff 18 08`). In Cut 2
 *     bestehen Punkt 3, 5 und 7 den Regions-Test @0x80053334 (RVD-Satz @0x00318); Punkt 5 (-14476,-23171) faellt in
 *     den Kanal, Bildlage bei y 0: (125,116).
 *   - Version_40% (echte Disc): dieselbe Schleife steht in sub03 @0x015B6..@0x0165E, wird aber NICHT gestartet.
 *     main00 endet @0x01598 mit `01 00`, im ganzen SCD steht kein Evt_exec und kein Gosub (kein `04 ?? 18 ??`), die
 *     drei Aot_set @0x014C6/@0x014DA/@0x014EE sind sce 8. Nur dort stimmt "kein Effekt auf dem Kanalwasser".
 *   - Port: die Tropfen haengen bei y -10018..-10000 (RE15_FX_LOG). Grund: Die Sammelklemme floor_y = Spawn-y in
 *     re15_esp.c esp_fx_spawn_rows_one haelt sie fest, und Routine 13 hat in esp_fx_dispatch keinen Fall (default).
 *   OFFEN 4 ist entschieden, Variante B wie Version_40%: tropfen_r41.c laesst den Evt_exec @0x0177C aus, und die Schleife
 *     startet nicht (include/re15_tropfen_r41.h). Mit dem Raumlicht haben die Tropfen nichts zu tun.
 * Die Wasserflaechen (Obj_model_set Typ 3 @0x016E6/@0x01708/@0x0172A) und der Wasserlauf der Spur N bleiben. */

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
