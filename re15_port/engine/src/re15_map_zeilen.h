/* GENERIERT von tools/gen_marker_zeilen.py - HERGELEITETE MARKER-ZEILEN.
 *
 * Die Tabelle @0x800768b0 setzt die lokalen Koordinaten EINES Raums auf die
 * gemalte Karte ({x_off, y_off, x_scale, z_scale}; Formel FUN_800473f8
 * @0x8004741c-0x80047528). Von 106 Slots tragen nur 39 eine echte Zeile - die
 * uebrigen haben Massstab 1, und die Formel bildet dort JEDE Weltposition auf
 * denselben Punkt ab. Das Kartensystem des Prototyps ist unfertig.
 *
 * Diese Zeilen sind HERGELEITET, nicht ausgeliefert: eine Tuer ist derselbe Ort
 * in zwei raumlokalen Systemen, also ist die Zeile des Nachbarn ueber sie
 * ausrechenbar. Riegel ist die Auslassprobe - jede AUSGELIEFERTE Zeile einmal
 * versteckt und hergeleitet: Median 7 px, 16 von 26 innerhalb 8 px.
 * PORT-ERGAENZUNG, ausdruecklich keine Rekonstruktion. Wo das Original eine
 * echte Zeile hat, steht sie hier NICHT - dort gilt das Original. */
typedef struct {
    unsigned char  slot;
    short          ox;
    unsigned short oy, sx, sz;
} re15_map_zeile_t;

static const re15_map_zeile_t s_map_zeilen[] = {
    {   0,     91,   227,  2428,  2229 },   /* ROOM1000  trigger x3, Massstab x geliehen von 1050, z geliehen von 1050 */
    {   1,     84,   167,  2287,  2287 },   /* ROOM1010  trigger x2, Massstab x geliehen von 1020, z geliehen von 1020 */
    {   5,     63,   247,  2428,  2229 },   /* ROOM1050  trigger x1, Massstab x geliehen von 1030, z geliehen von 1030 */
    {   6,      1,   278,  2080,  2320 },   /* ROOM1060  trigger x1, Massstab x geliehen von 1040, z geliehen von 1040 */
    {   8,     78,   214,  2080,  2320 },   /* ROOM1080  einseitig x1, Massstab x geliehen von 1040, z geliehen von 1040 */
    {   9,    132,   304,  2428,  2229 },   /* ROOM1090  trigger x1, Massstab x geliehen von 1050, z geliehen von 1050 */
    {  10,     40,   329,  2428,  2229 },   /* ROOM10A0  trigger x1, Massstab x geliehen von 1050, z geliehen von 1050 */
    {  18,    101,   190,  2296,  2312 },   /* ROOM1120  trigger x1, Massstab x geliehen von 1130, z geliehen von 1130 */
    {  19,     90,   192,  2296,  2312 },   /* ROOM1130  trigger x1, Massstab x geliehen von 1150, z geliehen von 1150 */
    {  20,    108,   183,  2296,  2312 },   /* ROOM1140  trigger x1, Massstab x geliehen von 1130, z geliehen von 1130 */
    {  24,     89,   216,  2200,  2301 },   /* ROOM1180  trigger x2, Massstab x aus 1190/11B0, z geliehen von 1190 */
    {  28,    158,   195,  2272,  1954 },   /* ROOM11C0  trigger x1, Massstab x geliehen von 11B0, z geliehen von 11B0 */
    {  29,     79,   217,  2200,  2301 },   /* ROOM11D0  trigger x1, Massstab x geliehen von 1180, z geliehen von 1180 */
    {  31,     35,   180,  2304,  2240 },   /* ROOM11F0  trigger x1, Massstab x geliehen von 11E0, z geliehen von 11E0 */
    {  34,    166,   123,  2496,  2250 },   /* ROOM1220  trigger x5, Massstab x geliehen von 1210, z geliehen von 1210 */
    {  39,     31,   191,  2340,  2308 },   /* ROOM2010  trigger x1, Massstab x geliehen von 2000, z geliehen von 2000 */
    {  40,    -38,   168,  2340,  2308 },   /* ROOM2020  trigger x1, Massstab x geliehen von 2050, z geliehen von 2050 */
    {  41,     75,   238,  2340,  2308 },   /* ROOM2030  trigger x1, Massstab x geliehen von 2070, z geliehen von 2070 */
    {  42,    137,   223,  2340,  2308 },   /* ROOM2040  trigger x2, Massstab x geliehen von 2050, z geliehen von 2050 */
    {  43,     -1,   186,  2340,  2308 },   /* ROOM2050  trigger x1, Massstab x geliehen von 2000, z geliehen von 2000 */
    {  44,    108,   263,  2340,  2308 },   /* ROOM2060  trigger x2, Massstab x geliehen von 2030, z geliehen von 2030 */
    {  45,     79,   224,  2340,  2308 },   /* ROOM2070  trigger x2, Massstab x geliehen von 2000, z geliehen von 2000 */
    {  46,    111,   187,  2340,  2308 },   /* ROOM2080  trigger x1, Massstab x geliehen von 2030, z geliehen von 2030 */
    {  47,    110,   187,  2340,  2308 },   /* ROOM2090  trigger x1, Massstab x geliehen von 2080, z geliehen von 2080 */
    {  52,    156,   213,  2208,  2737 },   /* ROOM3020  trigger x1, Massstab x geliehen von 3010, z geliehen von 3010 */
    {  55,    177,   195,  2240,  2240 },   /* ROOM3050  trigger x2, Massstab x geliehen von 3040, z geliehen von 3040 */
    {  60,    233,   253,  2064,  2337 },   /* ROOM30A0  trigger x1, Massstab x geliehen von 3090, z geliehen von 3090 */
    {  61,    206,   206,  2064,  2337 },   /* ROOM30B0  trigger x1, Massstab x geliehen von 3090, z geliehen von 3090 */
    {  62,    178,   208,  1560,  2337 },   /* ROOM30C0  trigger x2, Massstab x aus 3090/3030, z geliehen von 3090 */
    {  63,    154,   213,  2497,  2716 },   /* ROOM30D0  trigger x3, Massstab x aus 3010/3090, z aus 3010/3030 */
    {  67,    122,    87,  1760,  1760 },   /* ROOM4020  einseitig x1, Massstab x geliehen von 4000, z geliehen von 4000 */
    {  69,    116,   164,  1281,  1288 },   /* ROOM4040  spawn x1, Massstab x geliehen von 4030, z geliehen von 4030 */
    {  70,     92,   129,  1281,  1288 },   /* ROOM4050  trigger x2, Massstab x geliehen von 4040, z geliehen von 4040 */
    {  72,    127,   164,  1281,  1288 },   /* ROOM4070  trigger x1, Massstab x geliehen von 4040, z geliehen von 4040 */
    {  73,    125,   170,  1793,  1589 },   /* ROOM4080  trigger x2, Massstab x geliehen von 40A0, z aus 40A0/4090 */
    {  76,    159,   186,  1793,  1589 },   /* ROOM40B0  trigger x1, Massstab x geliehen von 4080, z geliehen von 4080 */
    {  77,     99,   154,  1792,  1710 },   /* ROOM5000  trigger x3, Massstab x geliehen von 5030, z aus 5030/5010 */
    {  79,     85,   114,  1792,  1710 },   /* ROOM5020  trigger x1, Massstab x geliehen von 5000, z geliehen von 5000 */
    {  82,     78,   123,  1793,  1729 },   /* ROOM5050  trigger x1, Massstab x geliehen von 5040, z geliehen von 5040 */
    {  88,    167,   177,  1696,  1681 },   /* ROOM50B0  trigger x1, Massstab x geliehen von 5140, z geliehen von 5140 */
    {  90,    113,   160,  1700,  1648 },   /* ROOM50D0  trigger x1, Massstab x geliehen von 50C0, z geliehen von 50C0 */
    {  91,    153,   142,  1700,  1648 },   /* ROOM50E0  trigger x1, Massstab x geliehen von 50D0, z geliehen von 50D0 */
    {  98,    202,   158,  2161,  2304 },   /* ROOM6000  trigger x1, Massstab x geliehen von 6030, z geliehen von 6030 */
    {  99,    203,   159,  2161,  2304 },   /* ROOM6010  trigger x2, Massstab x geliehen von 6000, z geliehen von 6000 */
};

