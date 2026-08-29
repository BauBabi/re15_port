/* GENERIERT von tools/gen_map_tables.py — MARKER-REPARATUR (Nutzer-Auftrag
 * 2026-08-30: 'die Karte reparieren'). 65 der 120 Basis-Raeume tragen im
 * Auslieferungsstand nur die Platzhalter-Marker-Zeile {0,0,1,1} (der Stock-
 * Positionsmarker projiziert dort auf ~(0,0)); einzelne weitere Zeilen
 * verfehlen ihr Rect. Diese Tabelle liefert fuer solche Slots reparierte
 * Parameter derselben Formel FUN_800473f8 @0x8004741c-528, hergeleitet aus
 * dem Kollisions-Footprint des Raums (RDT-SCA) und seinem zugeordneten Rect:
 *   xscl=round(ax*2^20), xoff=bx-32000*ax, zscl=round(az*2^20), yoff=bz+32000*az.
 * RE15_MAP_STOCK=1 schaltet die Reparatur ab (byte-true Auslieferungsstand).
 * Typedef re15_map_row_fix_t: re15_room.h. */
static const re15_map_row_fix_t s_map_row_fixes[] = {
    {  13,   145,   149,  1387,   671 },  /* 0x10D0: Platzhalter-Zeile */
    {  14,    57,   257,  3261,  3203 },  /* 0x10E0: Platzhalter-Zeile */
    {  15,    50,   200,  2783,  2645 },  /* 0x10F0: Platzhalter-Zeile */
    {  16,   180,   170,  1401,  1671 },  /* 0x1100: Platzhalter-Zeile */
    {  17,   155,   142,  1634,  2200 },  /* 0x1110: Platzhalter-Zeile */
    {  18,    75,   205,  2185,  1997 },  /* 0x1120: Platzhalter-Zeile */
    {  19,    93,   189,  2290,  2199 },  /* 0x1130: Platzhalter-Zeile */
    {  20,   116,   179,  2151,  2139 },  /* 0x1140: Platzhalter-Zeile */
    {  23,   145,   161,  1130,  1260 },  /* 0x1170: EXE-Zeile verfehlt Rect (IoU<0.30, EXE=(100, 206, 2280, 2268)) */
    {  24,   124,   121,   968,   833 },  /* 0x1180: Platzhalter-Zeile */
    {  28,   168,   198,  1402,  1763 },  /* 0x11C0: Platzhalter-Zeile */
    {  31,   100,   174,   939,  1207 },  /* 0x11F0: Platzhalter-Zeile */
    {  34,   209,   122,   957,   878 },  /* 0x1220: Platzhalter-Zeile */
    {  40,    99,   180,  2202,  2257 },  /* 0x2020: Platzhalter-Zeile */
    {  44,    90,   107,   797,   809 },  /* 0x2060: Platzhalter-Zeile */
    {  46,    53,   169,  1748,  1737 },  /* 0x2080: Platzhalter-Zeile */
    {  52,   155,   203,  2681,  2185 },  /* 0x3020: Platzhalter-Zeile */
    {  60,   220,   307,  1751,  3478 },  /* 0x30A0: Platzhalter-Zeile */
    {  62,   155,   213,  2678,  2638 },  /* 0x30C0: Platzhalter-Zeile */
    {  64,   134,   111,  1097,  1118 },  /* 0x30E0: EXE-Zeile verfehlt Rect (IoU<0.30, EXE=(93, 207, 2056, 2432)) */
    {  68,   119,   145,  4916,  4191 },  /* 0x4030: EXE-Zeile verfehlt Rect (IoU<0.30, EXE=(150, 121, 1281, 1288)) */
    {  74,   101,   184,  2102,  1392 },  /* 0x4090: EXE-Zeile verfehlt Rect (IoU<0.30, EXE=(191, 143, 1540, 1844)) */
    {  77,   113,   132,  1789,  1571 },  /* 0x5000: Platzhalter-Zeile */
    {  84,   226,   113,   915,   740 },  /* 0x5070: Platzhalter-Zeile */
    {  85,    87,   124,  1137,  1136 },  /* 0x5080: Platzhalter-Zeile */
    {  86,   154,   176,   925,  1013 },  /* 0x5090: Platzhalter-Zeile */
    {  88,   126,   199,  1550,   903 },  /* 0x50B0: Platzhalter-Zeile */
    {  90,   118,   170,  1450,  1706 },  /* 0x50D0: Platzhalter-Zeile */
    {  91,   162,   144,  1495,  1200 },  /* 0x50E0: Platzhalter-Zeile */
    {  92,   161,   128,  1733,  1585 },  /* 0x50F0: Platzhalter-Zeile */
    {  98,    95,   171,  1948,  2360 },  /* 0x6000: Platzhalter-Zeile */
    { 101,    33,   140,  2257,  2331 },  /* 0x6030: EXE-Zeile verfehlt Rect (IoU<0.30, EXE=(243, 137, 2161, 2304)) */
};
