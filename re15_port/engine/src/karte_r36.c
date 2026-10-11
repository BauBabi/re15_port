/* karte_r36.c — Runde 36 Spur M "karte": Bit-Tabelle der Teilbereiche (Besucht-Latch je Teil).
 * Belege und Herleitung: include/re15_karte_r36.h, analysis/befunde_runde36/M_karte.md (P5). */
#include "re15_karte_r36.h"

/* ⛔ EXPLIZIT UND NUR ANHAENGEN (dieselbe Regel wie s_etage_bit / s_zone_zusatzbit in
 * re15_map_zones.c): das Bit steht im Spielstand. Eine laufende Nummer ueber s_map_teile wuerde
 * sich verschieben, sobald der Generator einen Teil dazunimmt - dann zeigte ein alter Stand fremde
 * Teile als besucht. Schluessel = (Blatt, Rechteck, Ausschnitt-Ursprung) aus s_map_teile.
 * Bit = RE15_KARTE_R36_TEIL_BASIS + Zeilenindex. */
static const struct { unsigned char page, rect; short tx, ty; unsigned char eingang; }
s_teil_bit[] = {
    /* ROOM1110 (Blatt 3 rect 5, zid 17). Eingang: ROOM1100 Tuer slot 1 @0x009BA (ROOM1101 ebenso)
     * -> Spawn (-2400,-5000) im Evidence Room; die kleinen Raeume erreicht man NUR ueber die
     * Selbst-Tueren ROOM1110 slot 1 @0x00A6E -> (-4550,-650) und slot 3 @0x00AAE -> (-4550,4350). */
    { 3, 5, 146, 114, 1 },   /* Bit  96  Evidence Room        Welt x -2909..14950        */
    { 3, 5, 189, 122, 0 },   /* Bit  97  kleiner Raum oben    Welt z -3806..1320         */
    { 3, 5, 189, 134, 0 },   /* Bit  98  kleiner Raum unten   Welt z  1961..7087         */
    /* ROOM5090 (Blatt 9 rect 7, zid 84). Eingang: ROOM6030 Tuer slot 3 @0x00C92 (ROOM6031 ebenso)
     * -> Spawn (27200,4900) in Teil 1; die uebrigen Teile nur ueber die Selbst-Tueren ROOM5090
     * slot 0/2/3/4 @0x0104E/@0x0108E/@0x010AE/@0x010CE. */
    { 9, 7, 173, 117, 0 },   /* Bit  99 */
    { 9, 7, 213,  86, 1 },   /* Bit 100  Eingangsteil (Spawn aus ROOM6030) */
    { 9, 7, 213, 144, 0 },   /* Bit 101 */
    { 9, 7, 213, 108, 0 },   /* Bit 102 */
    { 9, 7, 213, 136, 0 },   /* Bit 103 */
    /* NEUE ZEILEN HIER ANHAENGEN. Kapazitaet: Bit 96..127 = 32 Zeilen. */
};
#define TEIL_BIT_N ((int)(sizeof s_teil_bit / sizeof s_teil_bit[0]))

int re15_karte_r36_teil_bit(int page, int rect, int tx, int ty, int *eingang)
{
    int i;
    for (i = 0; i < TEIL_BIT_N; i++)
        if (s_teil_bit[i].page == page && s_teil_bit[i].rect == rect &&
            s_teil_bit[i].tx == tx && s_teil_bit[i].ty == ty) {
            if (eingang) *eingang = s_teil_bit[i].eingang;
            return RE15_KARTE_R36_TEIL_BASIS + i;
        }
    if (eingang) *eingang = 0;
    return -1;
}

int re15_karte_r36_teil_bit_count(void) { return TEIL_BIT_N; }
