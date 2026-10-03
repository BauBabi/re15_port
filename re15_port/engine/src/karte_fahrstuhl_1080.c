/* karte_fahrstuhl_1080.c — Runde 35 Spur G: Kartenblatt der Fahrstuhlkabine ROOM1080.
 * Belege und Herleitung: include/re15_karte_fahrstuhl.h, analysis/befunde_runde35/G_karte.md B5. */
#include "re15_karte_fahrstuhl.h"
#include "re15_scd.h"   /* re15_game_flag_get */

/* Spiegel von DAT_800b0fe6 (alter Raum): das Original sichert beim Raumwechsel den
 * bisherigen Raum, bevor es den neuen schreibt - FUN_8001d600 Tuer-Zweig @0x8001d92c
 * `lhu v0,0x0fe2(0x800b)` -> @0x8001d938 `sh v0,0x0fe6(0x800b)` -> @0x8001d95c
 * `sh v0,0x0fe2`. Der Port fuehrt diese Zelle nicht; sie wird hier aus dem Raum
 * nachgefuehrt, den re15_map_zone_update je Bild bzw. am Raumlade-Punkt meldet. */
static unsigned s_raum_jetzt = 0, s_raum_vorher = 0;

void re15_karte_raum_gesehen(unsigned room)
{
    if (room != s_raum_jetzt) { s_raum_vorher = s_raum_jetzt; s_raum_jetzt = room; }
}

/* ⛔ PORT-WAHL (Abnahme 0, G2): das ORIGINAL waehlt das Blatt der Kabine NICHT nach dem
 * Vorraum. Der Seiten-Setzer FUN_8004b568 schickt Raumindex 8 (ROOM1080) ueber die Fall-Kette
 * 0..11 IMMER auf Blatt 2 (`ori v0,zero,0x2` @0x8004b684, `sb v0,0x260e` @0x8004b88c), und die
 * Karte liest DAT_800b0fe6 nicht. Belegt sind hier nur die Konstanten (Blatt 2/3/4 der
 * Etagenraeume aus demselben Setzer, die Bits 54/55/56 aus den RDTs) - die WAHL "Blatt der
 * Etage, aus der man einstieg" ist eine Port-Ergaenzung, weil die Kabine auf drei Blaettern
 * gemalt ist und das Original auf 2F/3F die Kabine des falschen Stockwerks zeigen wuerde. */
int re15_karte_fahrstuhl_blatt(unsigned room)
{
    unsigned vor;
    if ((room & ~1u) != 0x1080u) return -1;
    /* (1) Man betritt die Kabine IMMER aus einem Etagenraum - dort steht sie also. */
    vor = s_raum_vorher & ~1u;
    if (s_raum_jetzt == room) {
        if (vor == 0x1040u) return 2;   /* Seiten-Setzer @0x8004b684: Raum 4  -> Blatt 2 */
        if (vor == 0x10C0u) return 3;   /*                @0x8004b6f8: Raum 12 -> Blatt 3 */
        if (vor == 0x1120u) return 4;   /*                @0x8004b758: Raum 18 -> Blatt 4 */
    }
    /* (2) Sonst die Etage, die das Spiel selbst fuehrt (Bank 3 Bit 54/55/56; genau eines
     * gesetzt - ROOM10C0 loescht 54/56 @0x1008/@0x1022, ROOM1120 54/55 @0x0D86/@0x0DA0). */
    if (re15_game_flag_get(3, 54)) return 2;   /* ROOM1040 @0x15D6 `22 03 36 01` */
    if (re15_game_flag_get(3, 55)) return 3;   /* ROOM10C0 @0x0FEE `22 03 37 01` */
    if (re15_game_flag_get(3, 56)) return 4;   /* ROOM1120 @0x0D6C `22 03 38 01` */
    return -1;
}

/* Klemmfenster des Spieler-Markers in der Kabine (Nachbesserung 1, Abnahme 0 M1).
 * Das ORIGINAL klemmt den Marker NICHT: RE1.5 FUN_800473f8 setzt das Quad x-4..x+4 und haengt
 * es direkt ein (AddPrim @0x800475d8), RE2 Retail FUN_8006e120 schreibt x0/y0 (@0x8006e2d8 /
 * @0x8006e2f4) und ruft AddPrim (@0x8006e2f0) ohne Grenze. Die Rand-Reserve 4 des Kartenschirms
 * (re15_inv_screen.c) ist eine Port-Zutat fuer Kunst-Rechtecke, deren Rand blosser Rahmen ist.
 * In der Kabine fuellt die Kunst aber nur die linke obere 10x10-Ecke des 16x16-Rechtecks
 * (Kachel uv(168,40), MAP03.PIX/MAP05.PIX ab Datei-Byte 0x1454: Index 4 = Wand in Spalte/Zeile
 * 0 und 9, Index 1 = Innen in 1..8) - die Reserve schnitt vom gemalten Innenraum 3 px links und
 * oben ab. Fenster jetzt = gemalter Innenraum, um den Glyph-Versatz verschoben: der Ring des
 * Markers liegt im 8x8-Quad ab uv(224,128) auf uv 225..229 / 129..133 (DATA/TEX.TIM ab
 * Datei-Byte 0x14910, Zeile v=129), seine Mitte also bei Quad-Ursprung+3 = (mx-1, my-1).
 * Im RE2-Massstab (re15_map_zones.h, Zeilen 0x1080) erreicht der Spieler genau diesen
 * Innenraum; das Fenster greift nur fuer Lagen ausserhalb der Kollision (Spawn in der Wand). */
#define KF_GLYPH_MITTE 1   /* TEX.TIM @0x14910: Ring-Mitte = mx - 1 / my - 1 */
#define KF_INNEN_LO    1   /* MAP03/05.PIX @0x1454: Innen ab Spalte/Zeile 1 ...  */
#define KF_INNEN_HI    8   /*                      ... bis 8 (Wand 0 und 9)      */
int re15_karte_fahrstuhl_fenster(unsigned room, int rx, int ry,
                                 int *lo_x, int *hi_x, int *lo_y, int *hi_y)
{
    if ((room & ~1u) != 0x1080u) return 0;
    *lo_x = rx + KF_INNEN_LO + KF_GLYPH_MITTE; *hi_x = rx + KF_INNEN_HI + KF_GLYPH_MITTE;
    *lo_y = ry + KF_INNEN_LO + KF_GLYPH_MITTE; *hi_y = ry + KF_INNEN_HI + KF_GLYPH_MITTE;
    return 1;
}
