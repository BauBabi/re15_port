/* Erzeugt von tests/unit/test_r37_masken36.c (Modus "erzeuge_tiefe") - NICHT von Hand aendern.
 * Runde 41 Spur M: Generator-Rechtecke, die an einem erreichbaren Stehplatz einen Figurpunkt ohne solide Zelle
 * davor verdecken wuerden, bekommen die Tiefe max bucket(solcher Punkte) (include/re15_masken36_r41.h).
 * Spalten: Raum der MSK-Datei, Cut, Rechteck, neue Tiefe. */
static const re15_masken36_tiefe_zeile_t re15_masken36_tiefe_zeilen[] = {
    { 0x3040,  0,   0,  108 },   /* alt 100, falsch 733804 (max Bucket 108), echt 0 (min Bucket -1) */
    { 0x3040,  0,   1,  106 },   /* alt 103, falsch 23654 (max Bucket 106), echt 0 (min Bucket -1) */
    { 0x3040,  0,   2,  104 },   /* alt 100, falsch 359802 (max Bucket 104), echt 0 (min Bucket -1) */
    { 0x3040,  0,   3,  102 },   /* alt 100, falsch 130202 (max Bucket 102), echt 0 (min Bucket -1) */
    { 0x3040,  0,   4,  100 },   /* alt 98, falsch 82133 (max Bucket 100), echt 0 (min Bucket -1) */
    { 0x3040,  0,   5,  100 },   /* alt 98, falsch 16815 (max Bucket 100), echt 0 (min Bucket -1) */
    { 0x3040,  0,   6,  104 },   /* alt 103, falsch 8779 (max Bucket 104), echt 0 (min Bucket -1) */
    { 0x3040,  0,   8,  108 },   /* alt 100, falsch 553087 (max Bucket 108), echt 0 (min Bucket -1) */
    { 0x3040,  0,  10,  100 },   /* alt 97, falsch 16827 (max Bucket 100), echt 0 (min Bucket -1) */
    { 0x3040,  0,  12,  102 },   /* alt 100, falsch 285385 (max Bucket 102), echt 0 (min Bucket -1) */
    { 0x3040,  0,  13,  102 },   /* alt 100, falsch 85619 (max Bucket 102), echt 0 (min Bucket -1) */
    { 0x3040,  0,  24,   91 },   /* alt 0, falsch 3979 (max Bucket 91), echt 0 (min Bucket -1) */
    { 0x3040,  0,  25,   91 },   /* alt 0, falsch 1823 (max Bucket 91), echt 0 (min Bucket -1) */
    { 0x3040,  0,  27,   99 },   /* alt 97, falsch 1467 (max Bucket 99), echt 0 (min Bucket -1) */
    { 0x3040,  0,  28,  107 },   /* alt 0, falsch 4944 (max Bucket 107), echt 0 (min Bucket -1) */
    { 0x3040,  0,  36,   98 },   /* alt 97, falsch 4182 (max Bucket 98), echt 0 (min Bucket -1) */
    { 0x3040,  1,   0,  186 },   /* alt 172, falsch 406978 (max Bucket 186), echt 0 (min Bucket -1) */
    { 0x3040,  1,   1,  186 },   /* alt 172, falsch 48907 (max Bucket 186), echt 711 (min Bucket 205) */
    { 0x3040,  1,   2,  177 },   /* alt 172, falsch 13685 (max Bucket 177), echt 0 (min Bucket -1) */
    { 0x3040,  1,   3,  183 },   /* alt 172, falsch 14183 (max Bucket 183), echt 0 (min Bucket -1) */
    { 0x3040,  1,   4,  175 },   /* alt 172, falsch 10684 (max Bucket 175), echt 0 (min Bucket -1) */
    { 0x3040,  1,   5,  176 },   /* alt 0, falsch 1829820 (max Bucket 176), echt 13832 (min Bucket 207) */
    { 0x3040,  1,   6,   64 },   /* alt 0, falsch 151257 (max Bucket 64), echt 0 (min Bucket -1) */
    { 0x3040,  1,   7,  173 },   /* alt 0, falsch 655 (max Bucket 173), echt 77 (min Bucket 205) */
    { 0x3040,  1,   8,  178 },   /* alt 172, falsch 1351 (max Bucket 178), echt 150 (min Bucket 205) */
    { 0x3040,  1,   9,  187 },   /* alt 172, falsch 3721 (max Bucket 187), echt 203 (min Bucket 205) */
    { 0x3040,  1,  10,  187 },   /* alt 172, falsch 1555 (max Bucket 187), echt 11 (min Bucket 216) */
    { 0x3040,  1,  11,  187 },   /* alt 172, falsch 803 (max Bucket 187), echt 0 (min Bucket -1) */
    { 0x3040,  1,  12,  187 },   /* alt 172, falsch 1623 (max Bucket 187), echt 0 (min Bucket -1) */
    { 0x3040,  1,  13,  186 },   /* alt 172, falsch 16869 (max Bucket 186), echt 0 (min Bucket -1) */
    { 0x3040,  1,  14,  173 },   /* alt 0, falsch 3925 (max Bucket 173), echt 74 (min Bucket 206) */
    { 0x3040,  1,  15,  178 },   /* alt 172, falsch 9485 (max Bucket 178), echt 71 (min Bucket 206) */
    { 0x3040,  1,  16,  184 },   /* alt 172, falsch 2379 (max Bucket 184), echt 0 (min Bucket -1) */
    { 0x3040,  1,  17,  174 },   /* alt 172, falsch 1353 (max Bucket 174), echt 0 (min Bucket -1) */
    { 0x3040,  1,  18,  175 },   /* alt 172, falsch 2004 (max Bucket 175), echt 0 (min Bucket -1) */
    { 0x3040,  3,   9,  135 },   /* alt 88, falsch 66 (max Bucket 135), echt 0 (min Bucket -1) */
    { 0x3040,  3,  10,  155 },   /* alt 90, falsch 14241 (max Bucket 155), echt 0 (min Bucket -1) */
    { 0x3040,  3,  13,  152 },   /* alt 93, falsch 4208 (max Bucket 152), echt 0 (min Bucket -1) */
    { 0x3040,  3,  20,  157 },   /* alt 88, falsch 1772 (max Bucket 157), echt 0 (min Bucket -1) */
    { 0x3040,  3,  21,  153 },   /* alt 76, falsch 1410 (max Bucket 153), echt 0 (min Bucket -1) */
    { 0x3040,  3,  55,  154 },   /* alt 150, falsch 938 (max Bucket 154), echt 0 (min Bucket -1) */
    { 0x3040,  3,  56,  153 },   /* alt 152, falsch 246 (max Bucket 153), echt 0 (min Bucket -1) */
    { 0x3040,  3,  57,  153 },   /* alt 88, falsch 1633 (max Bucket 153), echt 0 (min Bucket -1) */
    { 0x3040,  3,  58,  150 },   /* alt 89, falsch 335 (max Bucket 150), echt 0 (min Bucket -1) */
    { 0x3040,  3,  59,  150 },   /* alt 92, falsch 310 (max Bucket 150), echt 0 (min Bucket -1) */
    { 0x3040,  3,  61,  150 },   /* alt 95, falsch 226 (max Bucket 150), echt 0 (min Bucket -1) */
    { 0x3040,  3,  65,  152 },   /* alt 88, falsch 802 (max Bucket 152), echt 0 (min Bucket -1) */
    { 0x3040,  3,  67,  151 },   /* alt 102, falsch 602 (max Bucket 151), echt 0 (min Bucket -1) */
    { 0x3040,  3,  68,  152 },   /* alt 76, falsch 645 (max Bucket 152), echt 0 (min Bucket -1) */
    { 0x3040,  3,  69,  155 },   /* alt 88, falsch 523 (max Bucket 155), echt 0 (min Bucket -1) */
    { 0x3040,  3,  70,  158 },   /* alt 150, falsch 1053 (max Bucket 158), echt 0 (min Bucket -1) */
    { 0x3040,  3,  71,  157 },   /* alt 152, falsch 526 (max Bucket 157), echt 0 (min Bucket -1) */
    { 0x3040,  3,  72,  157 },   /* alt 88, falsch 1064 (max Bucket 157), echt 0 (min Bucket -1) */
    { 0x3040,  3,  73,  151 },   /* alt 88, falsch 494 (max Bucket 151), echt 0 (min Bucket -1) */
    { 0x3040,  3,  74,  151 },   /* alt 76, falsch 322 (max Bucket 151), echt 0 (min Bucket -1) */
    { 0x3040,  3,  77,  144 },   /* alt 88, falsch 170 (max Bucket 144), echt 0 (min Bucket -1) */
    { 0x3040,  5,  49,  157 },   /* alt 76, falsch 119 (max Bucket 157), echt 0 (min Bucket -1) */
    { 0x3040,  7,  42,   12 },   /* alt 0, falsch 168 (max Bucket 12), echt 0 (min Bucket -1) */
    { 0x3040,  8,   4,  114 },   /* alt 106, falsch 143557 (max Bucket 114), echt 0 (min Bucket -1) */
    { 0x3040,  8,   5,  114 },   /* alt 105, falsch 55356 (max Bucket 114), echt 14 (min Bucket 107), echt teils nicht mehr verdeckt */
    { 0x3040,  8,  19,  114 },   /* alt 106, falsch 9615 (max Bucket 114), echt 3 (min Bucket 107), echt teils nicht mehr verdeckt */
    { 0x3040,  8,  26,  114 },   /* alt 106, falsch 6413 (max Bucket 114), echt 2 (min Bucket 107), echt teils nicht mehr verdeckt */
    { 0x3040,  8,  36,   92 },   /* alt 80, falsch 636212 (max Bucket 92), echt 0 (min Bucket -1) */
    { 0x3040,  8,  38,   80 },   /* alt 75, falsch 52934 (max Bucket 80), echt 0 (min Bucket -1) */
    { 0x3040,  8,  39,   75 },   /* alt 73, falsch 13152 (max Bucket 75), echt 0 (min Bucket -1) */
    { 0x3040,  8,  40,   89 },   /* alt 83, falsch 14357 (max Bucket 89), echt 0 (min Bucket -1) */
    { 0x3040,  8,  43,   84 },   /* alt 81, falsch 6964 (max Bucket 84), echt 0 (min Bucket -1) */
    { 0x3040,  8,  51,   34 },   /* alt 0, falsch 244 (max Bucket 34), echt 0 (min Bucket -1) */
    { 0x3040,  8,  59,  119 },   /* alt 0, falsch 1001 (max Bucket 119), echt 0 (min Bucket -1) */
    { 0x3040,  8,  63,   69 },   /* alt 0, falsch 16863 (max Bucket 69), echt 0 (min Bucket -1) */
    { 0x3040,  8,  75,  108 },   /* alt 104, falsch 6229 (max Bucket 108), echt 0 (min Bucket -1) */
    { 0x3040,  8,  76,  110 },   /* alt 106, falsch 3816 (max Bucket 110), echt 0 (min Bucket -1) */
    { 0x3040,  8,  79,  119 },   /* alt 0, falsch 1615 (max Bucket 119), echt 0 (min Bucket -1) */
    { 0x3040,  8,  90,  113 },   /* alt 107, falsch 1242 (max Bucket 113), echt 0 (min Bucket -1) */
    { 0x3040,  8,  91,   83 },   /* alt 82, falsch 2357 (max Bucket 83), echt 0 (min Bucket -1) */
    { 0x3040,  8,  92,   90 },   /* alt 88, falsch 725 (max Bucket 90), echt 0 (min Bucket -1) */
    { 0x3040,  8,  93,   90 },   /* alt 89, falsch 395 (max Bucket 90), echt 0 (min Bucket -1) */
    { 0x3040,  8,  95,   86 },   /* alt 85, falsch 374 (max Bucket 86), echt 0 (min Bucket -1) */
    { 0x3040,  8,  99,   77 },   /* alt 76, falsch 939 (max Bucket 77), echt 0 (min Bucket -1) */
    { 0x3040,  8, 103,   74 },   /* alt 73, falsch 1202 (max Bucket 74), echt 0 (min Bucket -1) */
    { 0x3040,  9,   0,   99 },   /* alt 83, falsch 691891 (max Bucket 99), echt 19 (min Bucket 169) */
    { 0x3040,  9,   3,  126 },   /* alt 99, falsch 98933 (max Bucket 126), echt 3785 (min Bucket 117), echt teils nicht mehr verdeckt */
    { 0x3040,  9,  23,  122 },   /* alt 84, falsch 801648 (max Bucket 122), echt 3162 (min Bucket 116), echt teils nicht mehr verdeckt */
    { 0x3040,  9,  24,   82 },   /* alt 77, falsch 91050 (max Bucket 82), echt 0 (min Bucket -1) */
    { 0x3040,  9,  25,   90 },   /* alt 87, falsch 25898 (max Bucket 90), echt 0 (min Bucket -1) */
    { 0x3040,  9,  26,   78 },   /* alt 76, falsch 17162 (max Bucket 78), echt 0 (min Bucket -1) */
    { 0x3040,  9,  29,   86 },   /* alt 85, falsch 7151 (max Bucket 86), echt 0 (min Bucket -1) */
    { 0x3040,  9,  30,  100 },   /* alt 96, falsch 7754 (max Bucket 100), echt 0 (min Bucket -1) */
    { 0x3040,  9,  53,   45 },   /* alt 0, falsch 3737 (max Bucket 45), echt 0 (min Bucket -1) */
    { 0x3040,  9,  62,   47 },   /* alt 0, falsch 565 (max Bucket 47), echt 0 (min Bucket -1) */
    { 0x3040,  9,  83,  101 },   /* alt 97, falsch 2599 (max Bucket 101), echt 1 (min Bucket 119) */
    { 0x3040,  9,  84,  104 },   /* alt 99, falsch 1555 (max Bucket 104), echt 10 (min Bucket 118) */
    { 0x3040,  9,  86,  120 },   /* alt 101, falsch 389 (max Bucket 120), echt 5 (min Bucket 118), echt teils nicht mehr verdeckt */
    { 0x3040,  9,  91,  101 },   /* alt 99, falsch 745 (max Bucket 101), echt 0 (min Bucket -1) */
    { 0x3040,  9, 103,   77 },   /* alt 76, falsch 1080 (max Bucket 77), echt 0 (min Bucket -1) */
    { 0x3040,  9, 104,   76 },   /* alt 75, falsch 3185 (max Bucket 76), echt 0 (min Bucket -1) */
    { 0x3040, 10,   6,   32 },   /* alt 0, falsch 629 (max Bucket 32), echt 0 (min Bucket -1) */
    { 0x3041,  0,   0,  108 },   /* alt 100, falsch 733804 (max Bucket 108), echt 0 (min Bucket -1) */
    { 0x3041,  0,   1,  106 },   /* alt 103, falsch 23654 (max Bucket 106), echt 0 (min Bucket -1) */
    { 0x3041,  0,   2,  104 },   /* alt 100, falsch 359802 (max Bucket 104), echt 0 (min Bucket -1) */
    { 0x3041,  0,   3,  102 },   /* alt 100, falsch 130202 (max Bucket 102), echt 0 (min Bucket -1) */
    { 0x3041,  0,   4,  100 },   /* alt 98, falsch 82133 (max Bucket 100), echt 0 (min Bucket -1) */
    { 0x3041,  0,   5,  100 },   /* alt 98, falsch 16815 (max Bucket 100), echt 0 (min Bucket -1) */
    { 0x3041,  0,   6,  104 },   /* alt 103, falsch 8779 (max Bucket 104), echt 0 (min Bucket -1) */
    { 0x3041,  0,   8,  108 },   /* alt 100, falsch 553087 (max Bucket 108), echt 0 (min Bucket -1) */
    { 0x3041,  0,  10,  100 },   /* alt 97, falsch 16827 (max Bucket 100), echt 0 (min Bucket -1) */
    { 0x3041,  0,  12,  102 },   /* alt 100, falsch 285385 (max Bucket 102), echt 0 (min Bucket -1) */
    { 0x3041,  0,  13,  102 },   /* alt 100, falsch 85619 (max Bucket 102), echt 0 (min Bucket -1) */
    { 0x3041,  0,  24,   91 },   /* alt 0, falsch 3979 (max Bucket 91), echt 0 (min Bucket -1) */
    { 0x3041,  0,  25,   91 },   /* alt 0, falsch 1823 (max Bucket 91), echt 0 (min Bucket -1) */
    { 0x3041,  0,  27,   99 },   /* alt 97, falsch 1467 (max Bucket 99), echt 0 (min Bucket -1) */
    { 0x3041,  0,  28,  107 },   /* alt 0, falsch 4944 (max Bucket 107), echt 0 (min Bucket -1) */
    { 0x3041,  0,  36,   98 },   /* alt 97, falsch 4182 (max Bucket 98), echt 0 (min Bucket -1) */
    { 0x3041,  1,   0,  186 },   /* alt 172, falsch 406978 (max Bucket 186), echt 0 (min Bucket -1) */
    { 0x3041,  1,   1,  186 },   /* alt 172, falsch 48907 (max Bucket 186), echt 711 (min Bucket 205) */
    { 0x3041,  1,   2,  177 },   /* alt 172, falsch 13685 (max Bucket 177), echt 0 (min Bucket -1) */
    { 0x3041,  1,   3,  183 },   /* alt 172, falsch 14183 (max Bucket 183), echt 0 (min Bucket -1) */
    { 0x3041,  1,   4,  175 },   /* alt 172, falsch 10684 (max Bucket 175), echt 0 (min Bucket -1) */
    { 0x3041,  1,   5,  176 },   /* alt 0, falsch 1829820 (max Bucket 176), echt 13832 (min Bucket 207) */
    { 0x3041,  1,   6,   64 },   /* alt 0, falsch 151257 (max Bucket 64), echt 0 (min Bucket -1) */
    { 0x3041,  1,   7,  173 },   /* alt 0, falsch 655 (max Bucket 173), echt 77 (min Bucket 205) */
    { 0x3041,  1,   8,  178 },   /* alt 172, falsch 1351 (max Bucket 178), echt 150 (min Bucket 205) */
    { 0x3041,  1,   9,  187 },   /* alt 172, falsch 3721 (max Bucket 187), echt 203 (min Bucket 205) */
    { 0x3041,  1,  10,  187 },   /* alt 172, falsch 1555 (max Bucket 187), echt 11 (min Bucket 216) */
    { 0x3041,  1,  11,  187 },   /* alt 172, falsch 803 (max Bucket 187), echt 0 (min Bucket -1) */
    { 0x3041,  1,  12,  187 },   /* alt 172, falsch 1623 (max Bucket 187), echt 0 (min Bucket -1) */
    { 0x3041,  1,  13,  186 },   /* alt 172, falsch 16869 (max Bucket 186), echt 0 (min Bucket -1) */
    { 0x3041,  1,  14,  173 },   /* alt 0, falsch 3925 (max Bucket 173), echt 74 (min Bucket 206) */
    { 0x3041,  1,  15,  178 },   /* alt 172, falsch 9485 (max Bucket 178), echt 71 (min Bucket 206) */
    { 0x3041,  1,  16,  184 },   /* alt 172, falsch 2379 (max Bucket 184), echt 0 (min Bucket -1) */
    { 0x3041,  1,  17,  174 },   /* alt 172, falsch 1353 (max Bucket 174), echt 0 (min Bucket -1) */
    { 0x3041,  1,  18,  175 },   /* alt 172, falsch 2004 (max Bucket 175), echt 0 (min Bucket -1) */
    { 0x3041,  3,   9,  135 },   /* alt 88, falsch 66 (max Bucket 135), echt 0 (min Bucket -1) */
    { 0x3041,  3,  10,  155 },   /* alt 90, falsch 14241 (max Bucket 155), echt 0 (min Bucket -1) */
    { 0x3041,  3,  13,  152 },   /* alt 93, falsch 4208 (max Bucket 152), echt 0 (min Bucket -1) */
    { 0x3041,  3,  20,  157 },   /* alt 88, falsch 1772 (max Bucket 157), echt 0 (min Bucket -1) */
    { 0x3041,  3,  21,  153 },   /* alt 76, falsch 1410 (max Bucket 153), echt 0 (min Bucket -1) */
    { 0x3041,  3,  55,  154 },   /* alt 150, falsch 938 (max Bucket 154), echt 0 (min Bucket -1) */
    { 0x3041,  3,  56,  153 },   /* alt 152, falsch 246 (max Bucket 153), echt 0 (min Bucket -1) */
    { 0x3041,  3,  57,  153 },   /* alt 88, falsch 1633 (max Bucket 153), echt 0 (min Bucket -1) */
    { 0x3041,  3,  58,  150 },   /* alt 89, falsch 335 (max Bucket 150), echt 0 (min Bucket -1) */
    { 0x3041,  3,  59,  150 },   /* alt 92, falsch 310 (max Bucket 150), echt 0 (min Bucket -1) */
    { 0x3041,  3,  61,  150 },   /* alt 95, falsch 226 (max Bucket 150), echt 0 (min Bucket -1) */
    { 0x3041,  3,  65,  152 },   /* alt 88, falsch 802 (max Bucket 152), echt 0 (min Bucket -1) */
    { 0x3041,  3,  67,  151 },   /* alt 102, falsch 602 (max Bucket 151), echt 0 (min Bucket -1) */
    { 0x3041,  3,  68,  152 },   /* alt 76, falsch 645 (max Bucket 152), echt 0 (min Bucket -1) */
    { 0x3041,  3,  69,  155 },   /* alt 88, falsch 523 (max Bucket 155), echt 0 (min Bucket -1) */
    { 0x3041,  3,  70,  158 },   /* alt 150, falsch 1053 (max Bucket 158), echt 0 (min Bucket -1) */
    { 0x3041,  3,  71,  157 },   /* alt 152, falsch 526 (max Bucket 157), echt 0 (min Bucket -1) */
    { 0x3041,  3,  72,  157 },   /* alt 88, falsch 1064 (max Bucket 157), echt 0 (min Bucket -1) */
    { 0x3041,  3,  73,  151 },   /* alt 88, falsch 494 (max Bucket 151), echt 0 (min Bucket -1) */
    { 0x3041,  3,  74,  151 },   /* alt 76, falsch 322 (max Bucket 151), echt 0 (min Bucket -1) */
    { 0x3041,  3,  77,  144 },   /* alt 88, falsch 170 (max Bucket 144), echt 0 (min Bucket -1) */
    { 0x3041,  5,  49,  157 },   /* alt 76, falsch 119 (max Bucket 157), echt 0 (min Bucket -1) */
    { 0x3041,  7,  42,   12 },   /* alt 0, falsch 168 (max Bucket 12), echt 0 (min Bucket -1) */
    { 0x3041,  8,   4,  114 },   /* alt 106, falsch 143557 (max Bucket 114), echt 0 (min Bucket -1) */
    { 0x3041,  8,   5,  114 },   /* alt 105, falsch 55356 (max Bucket 114), echt 14 (min Bucket 107), echt teils nicht mehr verdeckt */
    { 0x3041,  8,  19,  114 },   /* alt 106, falsch 9615 (max Bucket 114), echt 3 (min Bucket 107), echt teils nicht mehr verdeckt */
    { 0x3041,  8,  26,  114 },   /* alt 106, falsch 6413 (max Bucket 114), echt 2 (min Bucket 107), echt teils nicht mehr verdeckt */
    { 0x3041,  8,  36,   92 },   /* alt 80, falsch 636212 (max Bucket 92), echt 0 (min Bucket -1) */
    { 0x3041,  8,  38,   80 },   /* alt 75, falsch 52934 (max Bucket 80), echt 0 (min Bucket -1) */
    { 0x3041,  8,  39,   75 },   /* alt 73, falsch 13152 (max Bucket 75), echt 0 (min Bucket -1) */
    { 0x3041,  8,  40,   89 },   /* alt 83, falsch 14357 (max Bucket 89), echt 0 (min Bucket -1) */
    { 0x3041,  8,  43,   84 },   /* alt 81, falsch 6964 (max Bucket 84), echt 0 (min Bucket -1) */
    { 0x3041,  8,  51,   34 },   /* alt 0, falsch 244 (max Bucket 34), echt 0 (min Bucket -1) */
    { 0x3041,  8,  59,  119 },   /* alt 0, falsch 1001 (max Bucket 119), echt 0 (min Bucket -1) */
    { 0x3041,  8,  63,   69 },   /* alt 0, falsch 16863 (max Bucket 69), echt 0 (min Bucket -1) */
    { 0x3041,  8,  75,  108 },   /* alt 104, falsch 6229 (max Bucket 108), echt 0 (min Bucket -1) */
    { 0x3041,  8,  76,  110 },   /* alt 106, falsch 3816 (max Bucket 110), echt 0 (min Bucket -1) */
    { 0x3041,  8,  79,  119 },   /* alt 0, falsch 1615 (max Bucket 119), echt 0 (min Bucket -1) */
    { 0x3041,  8,  90,  113 },   /* alt 107, falsch 1242 (max Bucket 113), echt 0 (min Bucket -1) */
    { 0x3041,  8,  91,   83 },   /* alt 82, falsch 2357 (max Bucket 83), echt 0 (min Bucket -1) */
    { 0x3041,  8,  92,   90 },   /* alt 88, falsch 725 (max Bucket 90), echt 0 (min Bucket -1) */
    { 0x3041,  8,  93,   90 },   /* alt 89, falsch 395 (max Bucket 90), echt 0 (min Bucket -1) */
    { 0x3041,  8,  95,   86 },   /* alt 85, falsch 374 (max Bucket 86), echt 0 (min Bucket -1) */
    { 0x3041,  8,  99,   77 },   /* alt 76, falsch 939 (max Bucket 77), echt 0 (min Bucket -1) */
    { 0x3041,  8, 103,   74 },   /* alt 73, falsch 1202 (max Bucket 74), echt 0 (min Bucket -1) */
    { 0x3041,  9,   0,   99 },   /* alt 83, falsch 691891 (max Bucket 99), echt 19 (min Bucket 169) */
    { 0x3041,  9,   3,  126 },   /* alt 99, falsch 98933 (max Bucket 126), echt 3785 (min Bucket 117), echt teils nicht mehr verdeckt */
    { 0x3041,  9,  23,  122 },   /* alt 84, falsch 801648 (max Bucket 122), echt 3162 (min Bucket 116), echt teils nicht mehr verdeckt */
    { 0x3041,  9,  24,   82 },   /* alt 77, falsch 91050 (max Bucket 82), echt 0 (min Bucket -1) */
    { 0x3041,  9,  25,   90 },   /* alt 87, falsch 25898 (max Bucket 90), echt 0 (min Bucket -1) */
    { 0x3041,  9,  26,   78 },   /* alt 76, falsch 17162 (max Bucket 78), echt 0 (min Bucket -1) */
    { 0x3041,  9,  29,   86 },   /* alt 85, falsch 7151 (max Bucket 86), echt 0 (min Bucket -1) */
    { 0x3041,  9,  30,  100 },   /* alt 96, falsch 7754 (max Bucket 100), echt 0 (min Bucket -1) */
    { 0x3041,  9,  53,   45 },   /* alt 0, falsch 3737 (max Bucket 45), echt 0 (min Bucket -1) */
    { 0x3041,  9,  62,   47 },   /* alt 0, falsch 565 (max Bucket 47), echt 0 (min Bucket -1) */
    { 0x3041,  9,  83,  101 },   /* alt 97, falsch 2599 (max Bucket 101), echt 1 (min Bucket 119) */
    { 0x3041,  9,  84,  104 },   /* alt 99, falsch 1555 (max Bucket 104), echt 10 (min Bucket 118) */
    { 0x3041,  9,  86,  120 },   /* alt 101, falsch 389 (max Bucket 120), echt 5 (min Bucket 118), echt teils nicht mehr verdeckt */
    { 0x3041,  9,  91,  101 },   /* alt 99, falsch 745 (max Bucket 101), echt 0 (min Bucket -1) */
    { 0x3041,  9, 103,   77 },   /* alt 76, falsch 1080 (max Bucket 77), echt 0 (min Bucket -1) */
    { 0x3041,  9, 104,   76 },   /* alt 75, falsch 3185 (max Bucket 76), echt 0 (min Bucket -1) */
    { 0x3041, 10,   6,   32 },   /* alt 0, falsch 629 (max Bucket 32), echt 0 (min Bucket -1) */
    { 0x3050,  1,  16,  177 },   /* alt 158, falsch 199805 (max Bucket 177), echt 3604 (min Bucket 143), echt teils nicht mehr verdeckt */
    { 0x3050,  1,  17,  209 },   /* alt 178, falsch 119464 (max Bucket 209), echt 704 (min Bucket 143), echt teils nicht mehr verdeckt */
    { 0x3050,  1,  19,  187 },   /* alt 175, falsch 31787 (max Bucket 187), echt 0 (min Bucket -1) */
    { 0x3050,  1,  26,  191 },   /* alt 173, falsch 11139 (max Bucket 191), echt 128 (min Bucket 143), echt teils nicht mehr verdeckt */
    { 0x3050,  1,  43,   58 },   /* alt 0, falsch 546952 (max Bucket 58), echt 114792 (min Bucket 241) */
    { 0x3050,  1,  44,   50 },   /* alt 0, falsch 74582 (max Bucket 50), echt 0 (min Bucket -1) */
    { 0x3050,  1,  50,   49 },   /* alt 0, falsch 4202 (max Bucket 49), echt 15068 (min Bucket 79) */
    { 0x3050,  1,  51,  182 },   /* alt 173, falsch 2169 (max Bucket 182), echt 0 (min Bucket -1) */
    { 0x3050,  1,  52,  177 },   /* alt 167, falsch 4183 (max Bucket 177), echt 128 (min Bucket 143), echt teils nicht mehr verdeckt */
    { 0x3050,  1,  56,  205 },   /* alt 190, falsch 3315 (max Bucket 205), echt 0 (min Bucket -1) */
    { 0x3050,  1,  57,  196 },   /* alt 184, falsch 2479 (max Bucket 196), echt 0 (min Bucket -1) */
    { 0x3050,  1,  58,  191 },   /* alt 178, falsch 620 (max Bucket 191), echt 0 (min Bucket -1) */
    { 0x3050,  2,  17,  130 },   /* alt 125, falsch 7717 (max Bucket 130), echt 80 (min Bucket 146) */
    { 0x3050,  2,  37,  157 },   /* alt 156, falsch 114659 (max Bucket 157), echt 0 (min Bucket -1) */
    { 0x3050,  2,  39,  161 },   /* alt 156, falsch 27656 (max Bucket 161), echt 0 (min Bucket -1) */
    { 0x3050,  2,  63,  161 },   /* alt 155, falsch 2889 (max Bucket 161), echt 0 (min Bucket -1) */
    { 0x3050,  2,  76,  129 },   /* alt 125, falsch 17057 (max Bucket 129), echt 10980 (min Bucket 121), echt teils nicht mehr verdeckt */
    { 0x3050,  2,  98,  160 },   /* alt 157, falsch 6976 (max Bucket 160), echt 0 (min Bucket -1) */
    { 0x3050,  2, 101,  134 },   /* alt 125, falsch 15836 (max Bucket 134), echt 12 (min Bucket 145) */
    { 0x3050,  3,   3,   92 },   /* alt 86, falsch 9730 (max Bucket 92), echt 0 (min Bucket -1) */
    { 0x3050,  3,   7,   89 },   /* alt 86, falsch 3400 (max Bucket 89), echt 0 (min Bucket -1) */
    { 0x3050,  3,  12,   90 },   /* alt 89, falsch 2458 (max Bucket 90), echt 0 (min Bucket -1) */
    { 0x3050,  3,  65,   93 },   /* alt 89, falsch 1330 (max Bucket 93), echt 0 (min Bucket -1) */
    { 0x3050,  3,  66,   91 },   /* alt 88, falsch 1070 (max Bucket 91), echt 0 (min Bucket -1) */
    { 0x3050,  3,  67,   91 },   /* alt 87, falsch 1279 (max Bucket 91), echt 0 (min Bucket -1) */
    { 0x3050,  3,  68,   90 },   /* alt 86, falsch 1042 (max Bucket 90), echt 0 (min Bucket -1) */
    { 0x3050,  4,   5,  124 },   /* alt 114, falsch 47966 (max Bucket 124), echt 24319 (min Bucket 83), echt teils nicht mehr verdeckt */
    { 0x3050,  4,  64,   73 },   /* alt 0, falsch 1887 (max Bucket 73), echt 0 (min Bucket -1) */
    { 0x3050,  4,  94,  122 },   /* alt 114, falsch 3850 (max Bucket 122), echt 3092 (min Bucket 84), echt teils nicht mehr verdeckt */
    { 0x3050,  4,  95,  120 },   /* alt 118, falsch 1564 (max Bucket 120), echt 652 (min Bucket 143) */
    { 0x3050,  4,  96,  143 },   /* alt 122, falsch 8708 (max Bucket 143), echt 4057 (min Bucket 140), echt teils nicht mehr verdeckt */
    { 0x3050,  4, 103,  121 },   /* alt 114, falsch 3450 (max Bucket 121), echt 120 (min Bucket 83), echt teils nicht mehr verdeckt */
    { 0x3050,  4, 104,  121 },   /* alt 118, falsch 910 (max Bucket 121), echt 0 (min Bucket -1) */
    { 0x3050,  5,   0,  137 },   /* alt 136, falsch 2797364 (max Bucket 137), echt 774770 (min Bucket 161) */
    { 0x3050,  5,   1,  133 },   /* alt 120, falsch 255608 (max Bucket 133), echt 38965 (min Bucket 162) */
    { 0x3050,  5,   3,  139 },   /* alt 107, falsch 107136 (max Bucket 139), echt 62905 (min Bucket 160) */
    { 0x3050,  5,  16,  230 },   /* alt 105, falsch 1249342 (max Bucket 230), echt 9607 (min Bucket 156), echt teils nicht mehr verdeckt */
    { 0x3050,  5,  17,  274 },   /* alt 105, falsch 223020 (max Bucket 274), echt 6007 (min Bucket 157), echt teils nicht mehr verdeckt */
    { 0x3050,  5,  18,  141 },   /* alt 107, falsch 51914 (max Bucket 141), echt 0 (min Bucket -1) */
    { 0x3050,  5,  19,  117 },   /* alt 105, falsch 24259 (max Bucket 117), echt 0 (min Bucket -1) */
    { 0x3050,  5,  20,  319 },   /* alt 107, falsch 56142 (max Bucket 319), echt 6614 (min Bucket 157), echt teils nicht mehr verdeckt */
    { 0x3050,  5,  21,  139 },   /* alt 107, falsch 17960 (max Bucket 139), echt 4695 (min Bucket 159) */
    { 0x3050,  5,  22,  268 },   /* alt 107, falsch 23392 (max Bucket 268), echt 165 (min Bucket 290) */
    { 0x3050,  5,  23,  262 },   /* alt 105, falsch 22756 (max Bucket 262), echt 19 (min Bucket 298) */
    { 0x3050,  5,  24,  214 },   /* alt 105, falsch 872403 (max Bucket 214), echt 229 (min Bucket 243) */
    { 0x3050,  5,  25,  210 },   /* alt 105, falsch 73155 (max Bucket 210), echt 0 (min Bucket -1) */
    { 0x3050,  5,  26,  216 },   /* alt 105, falsch 63231 (max Bucket 216), echt 794 (min Bucket 238) */
    { 0x3050,  5,  27,  216 },   /* alt 105, falsch 20569 (max Bucket 216), echt 480 (min Bucket 236) */
    { 0x3050,  5,  28,  213 },   /* alt 105, falsch 16125 (max Bucket 213), echt 0 (min Bucket -1) */
    { 0x3050,  5,  39,   43 },   /* alt 0, falsch 82992 (max Bucket 43), echt 47047 (min Bucket 238) */
    { 0x3050,  5,  40,  197 },   /* alt 0, falsch 71621 (max Bucket 197), echt 0 (min Bucket -1) */
    { 0x3050,  5,  41,   33 },   /* alt 0, falsch 277 (max Bucket 33), echt 0 (min Bucket -1) */
    { 0x3050,  5,  42,   41 },   /* alt 0, falsch 27832 (max Bucket 41), echt 27482 (min Bucket 233) */
    { 0x3050,  5,  45,   90 },   /* alt 0, falsch 617052 (max Bucket 90), echt 0 (min Bucket -1) */
    { 0x3050,  5,  47,  198 },   /* alt 0, falsch 22055 (max Bucket 198), echt 0 (min Bucket -1) */
    { 0x3050,  5,  73,  141 },   /* alt 107, falsch 55965 (max Bucket 141), echt 36475 (min Bucket 160) */
    { 0x3050,  5,  75,   50 },   /* alt 0, falsch 2325 (max Bucket 50), echt 1150 (min Bucket 71) */
    { 0x3050,  5,  76,   45 },   /* alt 0, falsch 359 (max Bucket 45), echt 0 (min Bucket -1) */
    { 0x3050,  5,  77,   49 },   /* alt 0, falsch 1529 (max Bucket 49), echt 714 (min Bucket 71) */
    { 0x3050,  5,  78,  215 },   /* alt 105, falsch 14193 (max Bucket 215), echt 21 (min Bucket 242) */
    { 0x3050,  5,  79,   47 },   /* alt 0, falsch 655 (max Bucket 47), echt 0 (min Bucket -1) */
    { 0x3050,  5,  81,  143 },   /* alt 107, falsch 64798 (max Bucket 143), echt 51380 (min Bucket 158) */
    { 0x3050,  5,  82,  320 },   /* alt 107, falsch 9984 (max Bucket 320), echt 2784 (min Bucket 157), echt teils nicht mehr verdeckt */
    { 0x3050,  5,  83,  256 },   /* alt 105, falsch 14664 (max Bucket 256), echt 0 (min Bucket -1) */
    { 0x3050,  5,  84,  215 },   /* alt 105, falsch 12141 (max Bucket 215), echt 48 (min Bucket 235) */
    { 0x3050,  5,  85,  214 },   /* alt 105, falsch 11047 (max Bucket 214), echt 9 (min Bucket 241) */
    { 0x3050,  5,  89,  136 },   /* alt 110, falsch 7169 (max Bucket 136), echt 1341 (min Bucket 161) */
    { 0x3050,  5,  90,  141 },   /* alt 107, falsch 12361 (max Bucket 141), echt 2062 (min Bucket 158) */
    { 0x3050,  5,  97,  119 },   /* alt 107, falsch 3410 (max Bucket 119), echt 0 (min Bucket -1) */
    { 0x3050,  6,   0,  170 },   /* alt 95, falsch 904938 (max Bucket 170), echt 8298 (min Bucket 119), echt teils nicht mehr verdeckt */
    { 0x3050,  6,   1,  215 },   /* alt 95, falsch 258409 (max Bucket 215), echt 761 (min Bucket 189), echt teils nicht mehr verdeckt */
    { 0x3050,  6,   2,  170 },   /* alt 95, falsch 44217 (max Bucket 170), echt 344 (min Bucket 190) */
    { 0x3050,  6,   3,  102 },   /* alt 94, falsch 27344 (max Bucket 102), echt 0 (min Bucket -1) */
    { 0x3050,  6,   4,  156 },   /* alt 94, falsch 19596 (max Bucket 156), echt 1923 (min Bucket 119), echt teils nicht mehr verdeckt */
    { 0x3050,  6,   5,  219 },   /* alt 95, falsch 20303 (max Bucket 219), echt 0 (min Bucket -1) */
    { 0x3050,  6,   6,  211 },   /* alt 95, falsch 19760 (max Bucket 211), echt 9 (min Bucket 222) */
    { 0x3050,  6,   7,   28 },   /* alt 0, falsch 60702 (max Bucket 28), echt 107534 (min Bucket 152) */
    { 0x3050,  6,   8,  216 },   /* alt 95, falsch 2353 (max Bucket 216), echt 0 (min Bucket -1) */
    { 0x3050,  6,   9,  208 },   /* alt 95, falsch 5292 (max Bucket 208), echt 0 (min Bucket -1) */
    { 0x3050,  6,  10,  209 },   /* alt 95, falsch 6204 (max Bucket 209), echt 0 (min Bucket -1) */
    { 0x3050,  6,  11,  221 },   /* alt 95, falsch 3105 (max Bucket 221), echt 0 (min Bucket -1) */
    { 0x3050,  6,  12,  169 },   /* alt 95, falsch 5396 (max Bucket 169), echt 489 (min Bucket 190) */
    { 0x3050,  6,  13,  156 },   /* alt 94, falsch 4434 (max Bucket 156), echt 157 (min Bucket 118), echt teils nicht mehr verdeckt */
    { 0x3050,  6,  14,  171 },   /* alt 95, falsch 6367 (max Bucket 171), echt 0 (min Bucket -1) */
    { 0x3050,  6,  15,  102 },   /* alt 94, falsch 6887 (max Bucket 102), echt 1429 (min Bucket 120) */
    { 0x3050,  6,  16,  103 },   /* alt 94, falsch 4705 (max Bucket 103), echt 17 (min Bucket 121) */
    { 0x3050,  6,  17,   97 },   /* alt 94, falsch 10611 (max Bucket 97), echt 0 (min Bucket -1) */
    { 0x3050,  6,  18,  103 },   /* alt 94, falsch 4869 (max Bucket 103), echt 0 (min Bucket -1) */
    { 0x3050,  7,   0,  113 },   /* alt 59, falsch 3029600 (max Bucket 113), echt 0 (min Bucket -1) */
    { 0x3050,  7,   1,  117 },   /* alt 59, falsch 88737 (max Bucket 117), echt 0 (min Bucket -1) */
    { 0x3050,  7,   2,  119 },   /* alt 59, falsch 70213 (max Bucket 119), echt 0 (min Bucket -1) */
    { 0x3050,  7,   3,  131 },   /* alt 59, falsch 44623 (max Bucket 131), echt 0 (min Bucket -1) */
    { 0x3050,  7,   4,  116 },   /* alt 59, falsch 26459 (max Bucket 116), echt 0 (min Bucket -1) */
    { 0x3050,  7,   5,  113 },   /* alt 74, falsch 13746 (max Bucket 113), echt 0 (min Bucket -1) */
    { 0x3050,  7,   6,  131 },   /* alt 59, falsch 16334 (max Bucket 131), echt 0 (min Bucket -1) */
    { 0x3050,  7,   7,  108 },   /* alt 59, falsch 13370 (max Bucket 108), echt 0 (min Bucket -1) */
    { 0x3050,  7,   8,   77 },   /* alt 59, falsch 7085 (max Bucket 77), echt 0 (min Bucket -1) */
    { 0x3050,  7,   9,  116 },   /* alt 59, falsch 11558 (max Bucket 116), echt 0 (min Bucket -1) */
    { 0x3050,  7,  10,  104 },   /* alt 59, falsch 9290 (max Bucket 104), echt 0 (min Bucket -1) */
    { 0x3050,  7,  11,  121 },   /* alt 59, falsch 9918 (max Bucket 121), echt 0 (min Bucket -1) */
    { 0x3050,  7,  12,  115 },   /* alt 59, falsch 8708 (max Bucket 115), echt 0 (min Bucket -1) */
    { 0x3050,  7,  28,  102 },   /* alt 59, falsch 18975 (max Bucket 102), echt 0 (min Bucket -1) */
    { 0x3050,  7,  29,  100 },   /* alt 59, falsch 16096 (max Bucket 100), echt 0 (min Bucket -1) */
    { 0x3050,  7,  41,   75 },   /* alt 0, falsch 3951 (max Bucket 75), echt 0 (min Bucket -1) */
    { 0x3050,  7,  54,   99 },   /* alt 59, falsch 456 (max Bucket 99), echt 0 (min Bucket -1) */
    { 0x3050,  7,  55,  101 },   /* alt 59, falsch 5872 (max Bucket 101), echt 0 (min Bucket -1) */
    { 0x3050,  7,  63,  102 },   /* alt 59, falsch 5038 (max Bucket 102), echt 0 (min Bucket -1) */
    { 0x3050,  7,  69,  131 },   /* alt 74, falsch 1200 (max Bucket 131), echt 0 (min Bucket -1) */
    { 0x3050,  7,  70,  103 },   /* alt 59, falsch 6878 (max Bucket 103), echt 0 (min Bucket -1) */
    { 0x3050,  7,  72,  133 },   /* alt 59, falsch 4722 (max Bucket 133), echt 0 (min Bucket -1) */
    { 0x3050,  7,  73,  129 },   /* alt 59, falsch 3513 (max Bucket 129), echt 0 (min Bucket -1) */
    { 0x3050,  7,  76,  115 },   /* alt 59, falsch 4123 (max Bucket 115), echt 0 (min Bucket -1) */
    { 0x3050,  7,  77,  122 },   /* alt 59, falsch 4469 (max Bucket 122), echt 0 (min Bucket -1) */
    { 0x3050,  7,  78,  116 },   /* alt 59, falsch 541 (max Bucket 116), echt 0 (min Bucket -1) */
    { 0x3050,  7,  79,  110 },   /* alt 59, falsch 514 (max Bucket 110), echt 0 (min Bucket -1) */
    { 0x3050,  7,  80,  110 },   /* alt 59, falsch 1489 (max Bucket 110), echt 0 (min Bucket -1) */
    { 0x3050,  7,  81,  118 },   /* alt 59, falsch 3217 (max Bucket 118), echt 0 (min Bucket -1) */
    { 0x3050,  7,  82,  117 },   /* alt 59, falsch 2676 (max Bucket 117), echt 0 (min Bucket -1) */
    { 0x3050,  7,  83,  107 },   /* alt 59, falsch 3852 (max Bucket 107), echt 0 (min Bucket -1) */
    { 0x3050,  7,  85,  115 },   /* alt 59, falsch 1576 (max Bucket 115), echt 0 (min Bucket -1) */
    { 0x3050,  9,  24,  306 },   /* alt 302, falsch 9884 (max Bucket 306), echt 27618 (min Bucket 242), echt teils nicht mehr verdeckt */
    { 0x3050, 13,   1,  235 },   /* alt 195, falsch 5748 (max Bucket 235), echt 176731 (min Bucket 53), echt teils nicht mehr verdeckt */
    { 0x3050, 13,  16,  207 },   /* alt 200, falsch 754 (max Bucket 207), echt 13847 (min Bucket 58), echt teils nicht mehr verdeckt */
    { 0x3050, 13,  20,  223 },   /* alt 214, falsch 4793 (max Bucket 223), echt 6328 (min Bucket 66), echt teils nicht mehr verdeckt */
    { 0x3050, 13,  37,  231 },   /* alt 198, falsch 924 (max Bucket 231), echt 5064 (min Bucket 59), echt teils nicht mehr verdeckt */
    { 0x3050, 13,  40,  232 },   /* alt 210, falsch 3018 (max Bucket 232), echt 1524 (min Bucket 69), echt teils nicht mehr verdeckt */
    { 0x3050, 13,  67,  234 },   /* alt 209, falsch 2626 (max Bucket 234), echt 3142 (min Bucket 63), echt teils nicht mehr verdeckt */
    { 0x3050, 13,  69,  225 },   /* alt 211, falsch 1306 (max Bucket 225), echt 1293 (min Bucket 64), echt teils nicht mehr verdeckt */
    { 0x3050, 13,  71,  227 },   /* alt 214, falsch 1985 (max Bucket 227), echt 2029 (min Bucket 66), echt teils nicht mehr verdeckt */
    { 0x3050, 13,  72,  222 },   /* alt 216, falsch 1232 (max Bucket 222), echt 982 (min Bucket 68), echt teils nicht mehr verdeckt */
    { 0x3050, 13,  73,  219 },   /* alt 218, falsch 1103 (max Bucket 219), echt 862 (min Bucket 68), echt teils nicht mehr verdeckt */
    { 0x3050, 13,  75,  233 },   /* alt 212, falsch 1536 (max Bucket 233), echt 723 (min Bucket 67), echt teils nicht mehr verdeckt */
    { 0x3051,  1,  16,  177 },   /* alt 158, falsch 199805 (max Bucket 177), echt 3604 (min Bucket 143), echt teils nicht mehr verdeckt */
    { 0x3051,  1,  17,  209 },   /* alt 178, falsch 119464 (max Bucket 209), echt 704 (min Bucket 143), echt teils nicht mehr verdeckt */
    { 0x3051,  1,  19,  187 },   /* alt 175, falsch 31787 (max Bucket 187), echt 0 (min Bucket -1) */
    { 0x3051,  1,  26,  191 },   /* alt 173, falsch 11139 (max Bucket 191), echt 128 (min Bucket 143), echt teils nicht mehr verdeckt */
    { 0x3051,  1,  43,   58 },   /* alt 0, falsch 546952 (max Bucket 58), echt 114792 (min Bucket 241) */
    { 0x3051,  1,  44,   50 },   /* alt 0, falsch 74582 (max Bucket 50), echt 0 (min Bucket -1) */
    { 0x3051,  1,  50,   49 },   /* alt 0, falsch 4202 (max Bucket 49), echt 15068 (min Bucket 79) */
    { 0x3051,  1,  51,  182 },   /* alt 173, falsch 2169 (max Bucket 182), echt 0 (min Bucket -1) */
    { 0x3051,  1,  52,  177 },   /* alt 167, falsch 4183 (max Bucket 177), echt 128 (min Bucket 143), echt teils nicht mehr verdeckt */
    { 0x3051,  1,  56,  205 },   /* alt 190, falsch 3315 (max Bucket 205), echt 0 (min Bucket -1) */
    { 0x3051,  1,  57,  196 },   /* alt 184, falsch 2479 (max Bucket 196), echt 0 (min Bucket -1) */
    { 0x3051,  1,  58,  191 },   /* alt 178, falsch 620 (max Bucket 191), echt 0 (min Bucket -1) */
    { 0x3051,  2,  17,  130 },   /* alt 125, falsch 7717 (max Bucket 130), echt 80 (min Bucket 146) */
    { 0x3051,  2,  37,  157 },   /* alt 156, falsch 114659 (max Bucket 157), echt 0 (min Bucket -1) */
    { 0x3051,  2,  39,  161 },   /* alt 156, falsch 27656 (max Bucket 161), echt 0 (min Bucket -1) */
    { 0x3051,  2,  63,  161 },   /* alt 155, falsch 2889 (max Bucket 161), echt 0 (min Bucket -1) */
    { 0x3051,  2,  76,  129 },   /* alt 125, falsch 17057 (max Bucket 129), echt 10980 (min Bucket 121), echt teils nicht mehr verdeckt */
    { 0x3051,  2,  98,  160 },   /* alt 157, falsch 6976 (max Bucket 160), echt 0 (min Bucket -1) */
    { 0x3051,  2, 101,  134 },   /* alt 125, falsch 15836 (max Bucket 134), echt 12 (min Bucket 145) */
    { 0x3051,  3,   3,   92 },   /* alt 86, falsch 9730 (max Bucket 92), echt 0 (min Bucket -1) */
    { 0x3051,  3,   7,   89 },   /* alt 86, falsch 3400 (max Bucket 89), echt 0 (min Bucket -1) */
    { 0x3051,  3,  12,   90 },   /* alt 89, falsch 2458 (max Bucket 90), echt 0 (min Bucket -1) */
    { 0x3051,  3,  65,   93 },   /* alt 89, falsch 1330 (max Bucket 93), echt 0 (min Bucket -1) */
    { 0x3051,  3,  66,   91 },   /* alt 88, falsch 1070 (max Bucket 91), echt 0 (min Bucket -1) */
    { 0x3051,  3,  67,   91 },   /* alt 87, falsch 1279 (max Bucket 91), echt 0 (min Bucket -1) */
    { 0x3051,  3,  68,   90 },   /* alt 86, falsch 1042 (max Bucket 90), echt 0 (min Bucket -1) */
    { 0x3051,  4,   5,  124 },   /* alt 114, falsch 47966 (max Bucket 124), echt 24319 (min Bucket 83), echt teils nicht mehr verdeckt */
    { 0x3051,  4,  64,   73 },   /* alt 0, falsch 1887 (max Bucket 73), echt 0 (min Bucket -1) */
    { 0x3051,  4,  94,  122 },   /* alt 114, falsch 3850 (max Bucket 122), echt 3092 (min Bucket 84), echt teils nicht mehr verdeckt */
    { 0x3051,  4,  95,  120 },   /* alt 118, falsch 1564 (max Bucket 120), echt 652 (min Bucket 143) */
    { 0x3051,  4,  96,  143 },   /* alt 122, falsch 8708 (max Bucket 143), echt 4057 (min Bucket 140), echt teils nicht mehr verdeckt */
    { 0x3051,  4, 103,  121 },   /* alt 114, falsch 3450 (max Bucket 121), echt 120 (min Bucket 83), echt teils nicht mehr verdeckt */
    { 0x3051,  4, 104,  121 },   /* alt 118, falsch 910 (max Bucket 121), echt 0 (min Bucket -1) */
    { 0x3051,  5,   0,  137 },   /* alt 136, falsch 2797364 (max Bucket 137), echt 774770 (min Bucket 161) */
    { 0x3051,  5,   1,  133 },   /* alt 120, falsch 255608 (max Bucket 133), echt 38965 (min Bucket 162) */
    { 0x3051,  5,   3,  139 },   /* alt 107, falsch 107136 (max Bucket 139), echt 62905 (min Bucket 160) */
    { 0x3051,  5,  16,  230 },   /* alt 105, falsch 1249342 (max Bucket 230), echt 9607 (min Bucket 156), echt teils nicht mehr verdeckt */
    { 0x3051,  5,  17,  274 },   /* alt 105, falsch 223020 (max Bucket 274), echt 6007 (min Bucket 157), echt teils nicht mehr verdeckt */
    { 0x3051,  5,  18,  141 },   /* alt 107, falsch 51914 (max Bucket 141), echt 0 (min Bucket -1) */
    { 0x3051,  5,  19,  117 },   /* alt 105, falsch 24259 (max Bucket 117), echt 0 (min Bucket -1) */
    { 0x3051,  5,  20,  319 },   /* alt 107, falsch 56142 (max Bucket 319), echt 6614 (min Bucket 157), echt teils nicht mehr verdeckt */
    { 0x3051,  5,  21,  139 },   /* alt 107, falsch 17960 (max Bucket 139), echt 4695 (min Bucket 159) */
    { 0x3051,  5,  22,  268 },   /* alt 107, falsch 23392 (max Bucket 268), echt 165 (min Bucket 290) */
    { 0x3051,  5,  23,  262 },   /* alt 105, falsch 22756 (max Bucket 262), echt 19 (min Bucket 298) */
    { 0x3051,  5,  24,  214 },   /* alt 105, falsch 872403 (max Bucket 214), echt 229 (min Bucket 243) */
    { 0x3051,  5,  25,  210 },   /* alt 105, falsch 73155 (max Bucket 210), echt 0 (min Bucket -1) */
    { 0x3051,  5,  26,  216 },   /* alt 105, falsch 63231 (max Bucket 216), echt 794 (min Bucket 238) */
    { 0x3051,  5,  27,  216 },   /* alt 105, falsch 20569 (max Bucket 216), echt 480 (min Bucket 236) */
    { 0x3051,  5,  28,  213 },   /* alt 105, falsch 16125 (max Bucket 213), echt 0 (min Bucket -1) */
    { 0x3051,  5,  39,   43 },   /* alt 0, falsch 82992 (max Bucket 43), echt 47047 (min Bucket 238) */
    { 0x3051,  5,  40,  197 },   /* alt 0, falsch 71621 (max Bucket 197), echt 0 (min Bucket -1) */
    { 0x3051,  5,  41,   33 },   /* alt 0, falsch 277 (max Bucket 33), echt 0 (min Bucket -1) */
    { 0x3051,  5,  42,   41 },   /* alt 0, falsch 27832 (max Bucket 41), echt 27482 (min Bucket 233) */
    { 0x3051,  5,  45,   90 },   /* alt 0, falsch 617052 (max Bucket 90), echt 0 (min Bucket -1) */
    { 0x3051,  5,  47,  198 },   /* alt 0, falsch 22055 (max Bucket 198), echt 0 (min Bucket -1) */
    { 0x3051,  5,  73,  141 },   /* alt 107, falsch 55965 (max Bucket 141), echt 36475 (min Bucket 160) */
    { 0x3051,  5,  75,   50 },   /* alt 0, falsch 2325 (max Bucket 50), echt 1150 (min Bucket 71) */
    { 0x3051,  5,  76,   45 },   /* alt 0, falsch 359 (max Bucket 45), echt 0 (min Bucket -1) */
    { 0x3051,  5,  77,   49 },   /* alt 0, falsch 1529 (max Bucket 49), echt 714 (min Bucket 71) */
    { 0x3051,  5,  78,  215 },   /* alt 105, falsch 14193 (max Bucket 215), echt 21 (min Bucket 242) */
    { 0x3051,  5,  79,   47 },   /* alt 0, falsch 655 (max Bucket 47), echt 0 (min Bucket -1) */
    { 0x3051,  5,  81,  143 },   /* alt 107, falsch 64798 (max Bucket 143), echt 51380 (min Bucket 158) */
    { 0x3051,  5,  82,  320 },   /* alt 107, falsch 9984 (max Bucket 320), echt 2784 (min Bucket 157), echt teils nicht mehr verdeckt */
    { 0x3051,  5,  83,  256 },   /* alt 105, falsch 14664 (max Bucket 256), echt 0 (min Bucket -1) */
    { 0x3051,  5,  84,  215 },   /* alt 105, falsch 12141 (max Bucket 215), echt 48 (min Bucket 235) */
    { 0x3051,  5,  85,  214 },   /* alt 105, falsch 11047 (max Bucket 214), echt 9 (min Bucket 241) */
    { 0x3051,  5,  89,  136 },   /* alt 110, falsch 7169 (max Bucket 136), echt 1341 (min Bucket 161) */
    { 0x3051,  5,  90,  141 },   /* alt 107, falsch 12361 (max Bucket 141), echt 2062 (min Bucket 158) */
    { 0x3051,  5,  97,  119 },   /* alt 107, falsch 3410 (max Bucket 119), echt 0 (min Bucket -1) */
    { 0x3051,  6,   0,  170 },   /* alt 95, falsch 904938 (max Bucket 170), echt 8298 (min Bucket 119), echt teils nicht mehr verdeckt */
    { 0x3051,  6,   1,  215 },   /* alt 95, falsch 258409 (max Bucket 215), echt 761 (min Bucket 189), echt teils nicht mehr verdeckt */
    { 0x3051,  6,   2,  170 },   /* alt 95, falsch 44217 (max Bucket 170), echt 344 (min Bucket 190) */
    { 0x3051,  6,   3,  102 },   /* alt 94, falsch 27344 (max Bucket 102), echt 0 (min Bucket -1) */
    { 0x3051,  6,   4,  156 },   /* alt 94, falsch 19596 (max Bucket 156), echt 1923 (min Bucket 119), echt teils nicht mehr verdeckt */
    { 0x3051,  6,   5,  219 },   /* alt 95, falsch 20303 (max Bucket 219), echt 0 (min Bucket -1) */
    { 0x3051,  6,   6,  211 },   /* alt 95, falsch 19760 (max Bucket 211), echt 9 (min Bucket 222) */
    { 0x3051,  6,   7,   28 },   /* alt 0, falsch 60702 (max Bucket 28), echt 107534 (min Bucket 152) */
    { 0x3051,  6,   8,  216 },   /* alt 95, falsch 2353 (max Bucket 216), echt 0 (min Bucket -1) */
    { 0x3051,  6,   9,  208 },   /* alt 95, falsch 5292 (max Bucket 208), echt 0 (min Bucket -1) */
    { 0x3051,  6,  10,  209 },   /* alt 95, falsch 6204 (max Bucket 209), echt 0 (min Bucket -1) */
    { 0x3051,  6,  11,  221 },   /* alt 95, falsch 3105 (max Bucket 221), echt 0 (min Bucket -1) */
    { 0x3051,  6,  12,  169 },   /* alt 95, falsch 5396 (max Bucket 169), echt 489 (min Bucket 190) */
    { 0x3051,  6,  13,  156 },   /* alt 94, falsch 4434 (max Bucket 156), echt 157 (min Bucket 118), echt teils nicht mehr verdeckt */
    { 0x3051,  6,  14,  171 },   /* alt 95, falsch 6367 (max Bucket 171), echt 0 (min Bucket -1) */
    { 0x3051,  6,  15,  102 },   /* alt 94, falsch 6887 (max Bucket 102), echt 1429 (min Bucket 120) */
    { 0x3051,  6,  16,  103 },   /* alt 94, falsch 4705 (max Bucket 103), echt 17 (min Bucket 121) */
    { 0x3051,  6,  17,   97 },   /* alt 94, falsch 10611 (max Bucket 97), echt 0 (min Bucket -1) */
    { 0x3051,  6,  18,  103 },   /* alt 94, falsch 4869 (max Bucket 103), echt 0 (min Bucket -1) */
    { 0x3051,  7,   0,  113 },   /* alt 59, falsch 3029600 (max Bucket 113), echt 0 (min Bucket -1) */
    { 0x3051,  7,   1,  117 },   /* alt 59, falsch 88737 (max Bucket 117), echt 0 (min Bucket -1) */
    { 0x3051,  7,   2,  119 },   /* alt 59, falsch 70213 (max Bucket 119), echt 0 (min Bucket -1) */
    { 0x3051,  7,   3,  131 },   /* alt 59, falsch 44623 (max Bucket 131), echt 0 (min Bucket -1) */
    { 0x3051,  7,   4,  116 },   /* alt 59, falsch 26459 (max Bucket 116), echt 0 (min Bucket -1) */
    { 0x3051,  7,   5,  113 },   /* alt 74, falsch 13746 (max Bucket 113), echt 0 (min Bucket -1) */
    { 0x3051,  7,   6,  131 },   /* alt 59, falsch 16334 (max Bucket 131), echt 0 (min Bucket -1) */
    { 0x3051,  7,   7,  108 },   /* alt 59, falsch 13370 (max Bucket 108), echt 0 (min Bucket -1) */
    { 0x3051,  7,   8,   77 },   /* alt 59, falsch 7085 (max Bucket 77), echt 0 (min Bucket -1) */
    { 0x3051,  7,   9,  116 },   /* alt 59, falsch 11558 (max Bucket 116), echt 0 (min Bucket -1) */
    { 0x3051,  7,  10,  104 },   /* alt 59, falsch 9290 (max Bucket 104), echt 0 (min Bucket -1) */
    { 0x3051,  7,  11,  121 },   /* alt 59, falsch 9918 (max Bucket 121), echt 0 (min Bucket -1) */
    { 0x3051,  7,  12,  115 },   /* alt 59, falsch 8708 (max Bucket 115), echt 0 (min Bucket -1) */
    { 0x3051,  7,  28,  102 },   /* alt 59, falsch 18975 (max Bucket 102), echt 0 (min Bucket -1) */
    { 0x3051,  7,  29,  100 },   /* alt 59, falsch 16096 (max Bucket 100), echt 0 (min Bucket -1) */
    { 0x3051,  7,  41,   75 },   /* alt 0, falsch 3951 (max Bucket 75), echt 0 (min Bucket -1) */
    { 0x3051,  7,  54,   99 },   /* alt 59, falsch 456 (max Bucket 99), echt 0 (min Bucket -1) */
    { 0x3051,  7,  55,  101 },   /* alt 59, falsch 5872 (max Bucket 101), echt 0 (min Bucket -1) */
    { 0x3051,  7,  63,  102 },   /* alt 59, falsch 5038 (max Bucket 102), echt 0 (min Bucket -1) */
    { 0x3051,  7,  69,  131 },   /* alt 74, falsch 1200 (max Bucket 131), echt 0 (min Bucket -1) */
    { 0x3051,  7,  70,  103 },   /* alt 59, falsch 6878 (max Bucket 103), echt 0 (min Bucket -1) */
    { 0x3051,  7,  72,  133 },   /* alt 59, falsch 4722 (max Bucket 133), echt 0 (min Bucket -1) */
    { 0x3051,  7,  73,  129 },   /* alt 59, falsch 3513 (max Bucket 129), echt 0 (min Bucket -1) */
    { 0x3051,  7,  76,  115 },   /* alt 59, falsch 4123 (max Bucket 115), echt 0 (min Bucket -1) */
    { 0x3051,  7,  77,  122 },   /* alt 59, falsch 4469 (max Bucket 122), echt 0 (min Bucket -1) */
    { 0x3051,  7,  78,  116 },   /* alt 59, falsch 541 (max Bucket 116), echt 0 (min Bucket -1) */
    { 0x3051,  7,  79,  110 },   /* alt 59, falsch 514 (max Bucket 110), echt 0 (min Bucket -1) */
    { 0x3051,  7,  80,  110 },   /* alt 59, falsch 1489 (max Bucket 110), echt 0 (min Bucket -1) */
    { 0x3051,  7,  81,  118 },   /* alt 59, falsch 3217 (max Bucket 118), echt 0 (min Bucket -1) */
    { 0x3051,  7,  82,  117 },   /* alt 59, falsch 2676 (max Bucket 117), echt 0 (min Bucket -1) */
    { 0x3051,  7,  83,  107 },   /* alt 59, falsch 3852 (max Bucket 107), echt 0 (min Bucket -1) */
    { 0x3051,  7,  85,  115 },   /* alt 59, falsch 1576 (max Bucket 115), echt 0 (min Bucket -1) */
    { 0x3051,  9,  24,  306 },   /* alt 302, falsch 9884 (max Bucket 306), echt 27618 (min Bucket 242), echt teils nicht mehr verdeckt */
    { 0x3051, 13,   1,  235 },   /* alt 195, falsch 5748 (max Bucket 235), echt 176731 (min Bucket 53), echt teils nicht mehr verdeckt */
    { 0x3051, 13,  16,  207 },   /* alt 200, falsch 754 (max Bucket 207), echt 13847 (min Bucket 58), echt teils nicht mehr verdeckt */
    { 0x3051, 13,  20,  223 },   /* alt 214, falsch 4793 (max Bucket 223), echt 6328 (min Bucket 66), echt teils nicht mehr verdeckt */
    { 0x3051, 13,  37,  231 },   /* alt 198, falsch 924 (max Bucket 231), echt 5064 (min Bucket 59), echt teils nicht mehr verdeckt */
    { 0x3051, 13,  40,  232 },   /* alt 210, falsch 3018 (max Bucket 232), echt 1524 (min Bucket 69), echt teils nicht mehr verdeckt */
    { 0x3051, 13,  67,  234 },   /* alt 209, falsch 2626 (max Bucket 234), echt 3142 (min Bucket 63), echt teils nicht mehr verdeckt */
    { 0x3051, 13,  69,  225 },   /* alt 211, falsch 1306 (max Bucket 225), echt 1293 (min Bucket 64), echt teils nicht mehr verdeckt */
    { 0x3051, 13,  71,  227 },   /* alt 214, falsch 1985 (max Bucket 227), echt 2029 (min Bucket 66), echt teils nicht mehr verdeckt */
    { 0x3051, 13,  72,  222 },   /* alt 216, falsch 1232 (max Bucket 222), echt 982 (min Bucket 68), echt teils nicht mehr verdeckt */
    { 0x3051, 13,  73,  219 },   /* alt 218, falsch 1103 (max Bucket 219), echt 862 (min Bucket 68), echt teils nicht mehr verdeckt */
    { 0x3051, 13,  75,  233 },   /* alt 212, falsch 1536 (max Bucket 233), echt 723 (min Bucket 67), echt teils nicht mehr verdeckt */
    { 0x30A0,  0,   0,  107 },   /* alt 83, falsch 710767 (max Bucket 107), echt 134494 (min Bucket 93), echt teils nicht mehr verdeckt */
    { 0x30A0,  0,   4,  103 },   /* alt 83, falsch 417025 (max Bucket 103), echt 179414 (min Bucket 92), echt teils nicht mehr verdeckt */
    { 0x30A0,  0,   5,  101 },   /* alt 89, falsch 37 (max Bucket 101), echt 7080 (min Bucket 109) */
    { 0x30A0,  0,  19,  163 },   /* alt 122, falsch 703116 (max Bucket 163), echt 27076 (min Bucket 136), echt teils nicht mehr verdeckt */
    { 0x30A0,  0,  20,  154 },   /* alt 134, falsch 1025 (max Bucket 154), echt 0 (min Bucket -1) */
    { 0x30A0,  0,  21,  154 },   /* alt 141, falsch 12 (max Bucket 154), echt 0 (min Bucket -1) */
    { 0x30A0,  0,  22,  143 },   /* alt 113, falsch 297260 (max Bucket 143), echt 14111 (min Bucket 139), echt teils nicht mehr verdeckt */
    { 0x30A0,  0,  23,  122 },   /* alt 106, falsch 71397 (max Bucket 122), echt 1 (min Bucket 142) */
    { 0x30A0,  0,  24,  165 },   /* alt 143, falsch 101676 (max Bucket 165), echt 0 (min Bucket -1) */
    { 0x30A0,  0,  25,  158 },   /* alt 137, falsch 26092 (max Bucket 158), echt 0 (min Bucket -1) */
    { 0x30A0,  0,  26,  132 },   /* alt 118, falsch 20566 (max Bucket 132), echt 0 (min Bucket -1) */
    { 0x30A0,  0,  27,  115 },   /* alt 103, falsch 17602 (max Bucket 115), echt 0 (min Bucket -1) */
    { 0x30A0,  0,  28,  127 },   /* alt 115, falsch 6104 (max Bucket 127), echt 0 (min Bucket -1) */
    { 0x30A0,  0,  29,  110 },   /* alt 101, falsch 5226 (max Bucket 110), echt 0 (min Bucket -1) */
    { 0x30A0,  0,  30,  150 },   /* alt 134, falsch 6870 (max Bucket 150), echt 0 (min Bucket -1) */
    { 0x30A0,  0,  31,   75 },   /* alt 62, falsch 42501 (max Bucket 75), echt 0 (min Bucket -1) */
    { 0x30A0,  0,  32,   77 },   /* alt 62, falsch 16431 (max Bucket 77), echt 0 (min Bucket -1) */
    { 0x30A0,  0,  33,   72 },   /* alt 62, falsch 1042 (max Bucket 72), echt 0 (min Bucket -1) */
    { 0x30A0,  0,  34,   79 },   /* alt 62, falsch 6670 (max Bucket 79), echt 0 (min Bucket -1) */
    { 0x30A0,  0,  35,   80 },   /* alt 62, falsch 1975 (max Bucket 80), echt 0 (min Bucket -1) */
    { 0x30A0,  0,  36,   79 },   /* alt 62, falsch 1610 (max Bucket 79), echt 0 (min Bucket -1) */
    { 0x30A0,  0,  37,   76 },   /* alt 62, falsch 833 (max Bucket 76), echt 0 (min Bucket -1) */
    { 0x30A0,  0,  38,   89 },   /* alt 80, falsch 165531 (max Bucket 89), echt 1515 (min Bucket 97) */
    { 0x30A0,  0,  39,   97 },   /* alt 87, falsch 36296 (max Bucket 97), echt 0 (min Bucket -1) */
    { 0x30A0,  0,  40,   86 },   /* alt 78, falsch 22079 (max Bucket 86), echt 0 (min Bucket -1) */
    { 0x30A0,  0,  41,   83 },   /* alt 76, falsch 9543 (max Bucket 83), echt 0 (min Bucket -1) */
    { 0x30A0,  0,  42,  103 },   /* alt 93, falsch 10754 (max Bucket 103), echt 0 (min Bucket -1) */
    { 0x30A0,  0,  43,   94 },   /* alt 86, falsch 7800 (max Bucket 94), echt 0 (min Bucket -1) */
    { 0x30A0,  0,  51,  163 },   /* alt 157, falsch 7244 (max Bucket 163), echt 0 (min Bucket -1) */
    { 0x30A0,  0,  75,  165 },   /* alt 157, falsch 5429 (max Bucket 165), echt 0 (min Bucket -1) */
    { 0x30A0,  0,  78,  166 },   /* alt 151, falsch 2355 (max Bucket 166), echt 0 (min Bucket -1) */
    { 0x30A0,  0,  79,  163 },   /* alt 146, falsch 7101 (max Bucket 163), echt 0 (min Bucket -1) */
    { 0x30A0,  0,  81,  155 },   /* alt 140, falsch 1609 (max Bucket 155), echt 0 (min Bucket -1) */
    { 0x30A0,  0,  82,  148 },   /* alt 134, falsch 2981 (max Bucket 148), echt 0 (min Bucket -1) */
    { 0x30A0,  0,  83,  141 },   /* alt 127, falsch 1751 (max Bucket 141), echt 0 (min Bucket -1) */
    { 0x30A0,  0,  84,  136 },   /* alt 125, falsch 5355 (max Bucket 136), echt 0 (min Bucket -1) */
    { 0x30A0,  0,  87,  131 },   /* alt 119, falsch 2508 (max Bucket 131), echt 0 (min Bucket -1) */
    { 0x30A0,  0,  88,  125 },   /* alt 112, falsch 2637 (max Bucket 125), echt 0 (min Bucket -1) */
    { 0x30A0,  0,  89,  118 },   /* alt 108, falsch 4534 (max Bucket 118), echt 0 (min Bucket -1) */
    { 0x30A0,  0,  91,  113 },   /* alt 104, falsch 2156 (max Bucket 113), echt 0 (min Bucket -1) */
    { 0x30A0,  0,  92,  108 },   /* alt 100, falsch 1300 (max Bucket 108), echt 0 (min Bucket -1) */
    { 0x30A0,  0,  93,  105 },   /* alt 97, falsch 3821 (max Bucket 105), echt 81 (min Bucket 96), echt teils nicht mehr verdeckt */
    { 0x30A0,  0,  95,  101 },   /* alt 92, falsch 4942 (max Bucket 101), echt 0 (min Bucket -1) */
    { 0x30A0,  0,  96,   96 },   /* alt 89, falsch 1812 (max Bucket 96), echt 0 (min Bucket -1) */
    { 0x30A0,  0,  98,   93 },   /* alt 85, falsch 4507 (max Bucket 93), echt 0 (min Bucket -1) */
    { 0x30A0,  0,  99,   89 },   /* alt 81, falsch 4995 (max Bucket 89), echt 0 (min Bucket -1) */
    { 0x30A0,  0, 100,   84 },   /* alt 78, falsch 7305 (max Bucket 84), echt 0 (min Bucket -1) */
    { 0x30A0,  0, 101,   82 },   /* alt 62, falsch 2930 (max Bucket 82), echt 0 (min Bucket -1) */
    { 0x30A0,  0, 102,   77 },   /* alt 62, falsch 736 (max Bucket 77), echt 0 (min Bucket -1) */
    { 0x30A0,  0, 103,   76 },   /* alt 62, falsch 559 (max Bucket 76), echt 0 (min Bucket -1) */
    { 0x30A0,  0, 104,   74 },   /* alt 62, falsch 882 (max Bucket 74), echt 0 (min Bucket -1) */
    { 0x30A0,  1,   0,   94 },   /* alt 72, falsch 1249851 (max Bucket 94), echt 58664 (min Bucket 106) */
    { 0x30A0,  1,   1,   97 },   /* alt 92, falsch 25661 (max Bucket 97), echt 1391 (min Bucket 111) */
    { 0x30A0,  1,   2,   65 },   /* alt 62, falsch 8705 (max Bucket 65), echt 0 (min Bucket -1) */
    { 0x30A0,  1,   3,   94 },   /* alt 91, falsch 7874 (max Bucket 94), echt 2864 (min Bucket 108) */
    { 0x30A0,  1,   4,   88 },   /* alt 71, falsch 478118 (max Bucket 88), echt 63621 (min Bucket 102) */
    { 0x30A0,  1,   7,   76 },   /* alt 65, falsch 345744 (max Bucket 76), echt 0 (min Bucket -1) */
    { 0x30A0,  1,   8,   85 },   /* alt 77, falsch 105538 (max Bucket 85), echt 0 (min Bucket -1) */
    { 0x30A0,  1,   9,   68 },   /* alt 62, falsch 81082 (max Bucket 68), echt 0 (min Bucket -1) */
    { 0x30A0,  1,  10,   80 },   /* alt 75, falsch 27590 (max Bucket 80), echt 0 (min Bucket -1) */
    { 0x30A0,  1,  11,   91 },   /* alt 84, falsch 28974 (max Bucket 91), echt 0 (min Bucket -1) */
    { 0x30A0,  1,  12,   65 },   /* alt 60, falsch 20561 (max Bucket 65), echt 0 (min Bucket -1) */
    { 0x30A0,  1,  13,   71 },   /* alt 67, falsch 21838 (max Bucket 71), echt 0 (min Bucket -1) */
    { 0x30A0,  1,  14,   79 },   /* alt 74, falsch 8609 (max Bucket 79), echt 0 (min Bucket -1) */
    { 0x30A0,  1,  15,   73 },   /* alt 69, falsch 8086 (max Bucket 73), echt 0 (min Bucket -1) */
    { 0x30A0,  1,  16,   89 },   /* alt 83, falsch 8387 (max Bucket 89), echt 0 (min Bucket -1) */
    { 0x30A0,  1,  17,   63 },   /* alt 60, falsch 6056 (max Bucket 63), echt 0 (min Bucket -1) */
    { 0x30A0,  1,  18,   83 },   /* alt 78, falsch 6025 (max Bucket 83), echt 0 (min Bucket -1) */
    { 0x30A0,  1,  19,   93 },   /* alt 89, falsch 5977 (max Bucket 93), echt 0 (min Bucket -1) */
    { 0x30A0,  1,  20,   66 },   /* alt 63, falsch 4330 (max Bucket 66), echt 0 (min Bucket -1) */
    { 0x30A0,  1,  28,   76 },   /* alt 72, falsch 82413 (max Bucket 76), echt 0 (min Bucket -1) */
    { 0x30A0,  1,  29,   73 },   /* alt 71, falsch 16521 (max Bucket 73), echt 0 (min Bucket -1) */
    { 0x30A0,  1,  31,   76 },   /* alt 73, falsch 17373 (max Bucket 76), echt 0 (min Bucket -1) */
    { 0x30A0,  1,  32,   76 },   /* alt 72, falsch 14353 (max Bucket 76), echt 0 (min Bucket -1) */
    { 0x30A0,  1,  46,   94 },   /* alt 92, falsch 4176 (max Bucket 94), echt 1369 (min Bucket 109) */
    { 0x30A0,  1,  52,   97 },   /* alt 93, falsch 4431 (max Bucket 97), echt 1 (min Bucket 113) */
    { 0x30A0,  1,  53,   94 },   /* alt 90, falsch 1437 (max Bucket 94), echt 0 (min Bucket -1) */
    { 0x30A0,  1,  54,   95 },   /* alt 91, falsch 1804 (max Bucket 95), echt 0 (min Bucket -1) */
    { 0x30A0,  1,  55,   95 },   /* alt 92, falsch 689 (max Bucket 95), echt 0 (min Bucket -1) */
    { 0x30A0,  1,  56,   72 },   /* alt 71, falsch 1479 (max Bucket 72), echt 0 (min Bucket -1) */
    { 0x30A0,  1,  58,   74 },   /* alt 72, falsch 4306 (max Bucket 74), echt 0 (min Bucket -1) */
    { 0x30A0,  1,  60,   91 },   /* alt 88, falsch 1030 (max Bucket 91), echt 0 (min Bucket -1) */
    { 0x30A0,  1,  61,   93 },   /* alt 89, falsch 661 (max Bucket 93), echt 0 (min Bucket -1) */
    { 0x30A0,  1,  64,   88 },   /* alt 85, falsch 1344 (max Bucket 88), echt 0 (min Bucket -1) */
    { 0x30A0,  1,  65,   90 },   /* alt 86, falsch 1691 (max Bucket 90), echt 0 (min Bucket -1) */
    { 0x30A0,  1,  66,   90 },   /* alt 87, falsch 654 (max Bucket 90), echt 0 (min Bucket -1) */
    { 0x30A0,  1,  68,   76 },   /* alt 72, falsch 2253 (max Bucket 76), echt 0 (min Bucket -1) */
    { 0x30A0,  1,  69,   76 },   /* alt 73, falsch 1824 (max Bucket 76), echt 0 (min Bucket -1) */
    { 0x30A0,  1,  70,   86 },   /* alt 82, falsch 1309 (max Bucket 86), echt 0 (min Bucket -1) */
    { 0x30A0,  1,  71,   86 },   /* alt 83, falsch 1658 (max Bucket 86), echt 0 (min Bucket -1) */
    { 0x30A0,  1,  72,   87 },   /* alt 84, falsch 640 (max Bucket 87), echt 0 (min Bucket -1) */
    { 0x30A0,  1,  73,   76 },   /* alt 74, falsch 1208 (max Bucket 76), echt 0 (min Bucket -1) */
    { 0x30A0,  1,  75,   76 },   /* alt 75, falsch 3881 (max Bucket 76), echt 0 (min Bucket -1) */
    { 0x30A0,  1,  76,   83 },   /* alt 80, falsch 961 (max Bucket 83), echt 0 (min Bucket -1) */
    { 0x30A0,  1,  77,   84 },   /* alt 81, falsch 630 (max Bucket 84), echt 0 (min Bucket -1) */
    { 0x30A0,  1,  79,   82 },   /* alt 78, falsch 2498 (max Bucket 82), echt 0 (min Bucket -1) */
    { 0x30A0,  1,  80,   81 },   /* alt 79, falsch 619 (max Bucket 81), echt 0 (min Bucket -1) */
    { 0x30A0,  1,  83,   79 },   /* alt 75, falsch 1223 (max Bucket 79), echt 0 (min Bucket -1) */
    { 0x30A0,  1,  84,   80 },   /* alt 76, falsch 1834 (max Bucket 80), echt 0 (min Bucket -1) */
    { 0x30A0,  1,  85,   76 },   /* alt 72, falsch 1477 (max Bucket 76), echt 0 (min Bucket -1) */
    { 0x30A0,  1,  86,   76 },   /* alt 73, falsch 2076 (max Bucket 76), echt 0 (min Bucket -1) */
    { 0x30A0,  1,  87,   77 },   /* alt 74, falsch 1205 (max Bucket 77), echt 0 (min Bucket -1) */
    { 0x30A0,  1,  88,   74 },   /* alt 72, falsch 923 (max Bucket 74), echt 0 (min Bucket -1) */
    { 0x30A0,  1,  89,   74 },   /* alt 73, falsch 706 (max Bucket 74), echt 0 (min Bucket -1) */
    { 0x30A0,  1,  91,   73 },   /* alt 70, falsch 1150 (max Bucket 73), echt 0 (min Bucket -1) */
    { 0x30A0,  1,  92,   75 },   /* alt 71, falsch 1755 (max Bucket 75), echt 0 (min Bucket -1) */
    { 0x30A0,  1,  93,   75 },   /* alt 71, falsch 2647 (max Bucket 75), echt 0 (min Bucket -1) */
    { 0x30A0,  1,  94,   72 },   /* alt 68, falsch 2222 (max Bucket 72), echt 0 (min Bucket -1) */
    { 0x30A0,  1,  95,   72 },   /* alt 69, falsch 1688 (max Bucket 72), echt 0 (min Bucket -1) */
    { 0x30A0,  1,  96,   69 },   /* alt 65, falsch 1615 (max Bucket 69), echt 0 (min Bucket -1) */
    { 0x30A0,  1,  97,   69 },   /* alt 66, falsch 3217 (max Bucket 69), echt 0 (min Bucket -1) */
    { 0x30A0,  1,  98,   70 },   /* alt 67, falsch 1653 (max Bucket 70), echt 0 (min Bucket -1) */
    { 0x30A0,  1,  99,   68 },   /* alt 64, falsch 2353 (max Bucket 68), echt 0 (min Bucket -1) */
    { 0x30A0,  1, 100,   65 },   /* alt 62, falsch 1066 (max Bucket 65), echt 0 (min Bucket -1) */
    { 0x30A0,  1, 101,   66 },   /* alt 63, falsch 814 (max Bucket 66), echt 0 (min Bucket -1) */
    { 0x30A0,  1, 102,   65 },   /* alt 61, falsch 2005 (max Bucket 65), echt 0 (min Bucket -1) */
    { 0x30A0,  1, 103,   62 },   /* alt 59, falsch 1739 (max Bucket 62), echt 0 (min Bucket -1) */
    { 0x30A0,  1, 104,   63 },   /* alt 60, falsch 1021 (max Bucket 63), echt 0 (min Bucket -1) */
    { 0x30A0,  3,  20,   80 },   /* alt 60, falsch 555270 (max Bucket 80), echt 427 (min Bucket 107) */
    { 0x30A0,  3,  21,   94 },   /* alt 89, falsch 127930 (max Bucket 94), echt 13199 (min Bucket 109) */
    { 0x30A0,  3,  27,   72 },   /* alt 57, falsch 447036 (max Bucket 72), echt 0 (min Bucket -1) */
    { 0x30A0,  3,  28,   98 },   /* alt 81, falsch 265627 (max Bucket 98), echt 4373 (min Bucket 117) */
    { 0x30A0,  3,  29,   89 },   /* alt 76, falsch 83801 (max Bucket 89), echt 0 (min Bucket -1) */
    { 0x30A0,  3,  30,   72 },   /* alt 57, falsch 21924 (max Bucket 72), echt 0 (min Bucket -1) */
    { 0x30A0,  3,  31,   70 },   /* alt 58, falsch 9395 (max Bucket 70), echt 0 (min Bucket -1) */
    { 0x30A0,  3,  32,   71 },   /* alt 62, falsch 6478 (max Bucket 71), echt 0 (min Bucket -1) */
    { 0x30A0,  3,  33,   93 },   /* alt 90, falsch 7491 (max Bucket 93), echt 0 (min Bucket -1) */
    { 0x30A0,  3,  34,   57 },   /* alt 52, falsch 4417 (max Bucket 57), echt 0 (min Bucket -1) */
    { 0x30A0,  3,  35,   71 },   /* alt 70, falsch 3848 (max Bucket 71), echt 0 (min Bucket -1) */
    { 0x30A0,  3,  36,   60 },   /* alt 52, falsch 238117 (max Bucket 60), echt 0 (min Bucket -1) */
    { 0x30A0,  3,  37,   65 },   /* alt 59, falsch 94506 (max Bucket 65), echt 0 (min Bucket -1) */
    { 0x30A0,  3,  38,   76 },   /* alt 67, falsch 70868 (max Bucket 76), echt 0 (min Bucket -1) */
    { 0x30A0,  3,  39,   52 },   /* alt 50, falsch 28783 (max Bucket 52), echt 0 (min Bucket -1) */
    { 0x30A0,  3,  40,   66 },   /* alt 60, falsch 37134 (max Bucket 66), echt 0 (min Bucket -1) */
    { 0x30A0,  3,  41,   85 },   /* alt 70, falsch 42415 (max Bucket 85), echt 0 (min Bucket -1) */
    { 0x30A0,  3,  42,   54 },   /* alt 50, falsch 23661 (max Bucket 54), echt 0 (min Bucket -1) */
    { 0x30A0,  3,  43,   57 },   /* alt 52, falsch 20365 (max Bucket 57), echt 0 (min Bucket -1) */
    { 0x30A0,  3,  44,   73 },   /* alt 65, falsch 21973 (max Bucket 73), echt 0 (min Bucket -1) */
    { 0x30A0,  3,  45,   69 },   /* alt 58, falsch 3200 (max Bucket 69), echt 0 (min Bucket -1) */
    { 0x30A0,  3,  78,   99 },   /* alt 97, falsch 5857 (max Bucket 99), echt 80 (min Bucket 117) */
    { 0x30A0,  3,  81,   75 },   /* alt 70, falsch 4781 (max Bucket 75), echt 0 (min Bucket -1) */
    { 0x30A0,  3,  82,   94 },   /* alt 93, falsch 2512 (max Bucket 94), echt 0 (min Bucket -1) */
    { 0x30A0,  3,  83,   92 },   /* alt 89, falsch 3657 (max Bucket 92), echt 0 (min Bucket -1) */
    { 0x30A0,  3,  84,   89 },   /* alt 85, falsch 386 (max Bucket 89), echt 0 (min Bucket -1) */
    { 0x30A0,  3,  85,   88 },   /* alt 83, falsch 984 (max Bucket 88), echt 0 (min Bucket -1) */
    { 0x30A0,  3,  86,   84 },   /* alt 81, falsch 945 (max Bucket 84), echt 0 (min Bucket -1) */
    { 0x30A0,  3,  87,   82 },   /* alt 77, falsch 1757 (max Bucket 82), echt 0 (min Bucket -1) */
    { 0x30A0,  3,  88,   77 },   /* alt 73, falsch 896 (max Bucket 77), echt 0 (min Bucket -1) */
    { 0x30A0,  3,  89,   88 },   /* alt 82, falsch 2434 (max Bucket 88), echt 0 (min Bucket -1) */
    { 0x30A0,  3,  91,   85 },   /* alt 81, falsch 385 (max Bucket 85), echt 0 (min Bucket -1) */
    { 0x30A0,  3,  92,   84 },   /* alt 77, falsch 8378 (max Bucket 84), echt 0 (min Bucket -1) */
    { 0x30A0,  3,  93,   81 },   /* alt 75, falsch 11475 (max Bucket 81), echt 0 (min Bucket -1) */
    { 0x30A0,  3,  94,   78 },   /* alt 73, falsch 12561 (max Bucket 78), echt 0 (min Bucket -1) */
    { 0x30A0,  3,  95,   76 },   /* alt 69, falsch 6853 (max Bucket 76), echt 0 (min Bucket -1) */
    { 0x30A0,  3,  96,   72 },   /* alt 67, falsch 4039 (max Bucket 72), echt 0 (min Bucket -1) */
    { 0x30A0,  3,  97,   69 },   /* alt 63, falsch 13641 (max Bucket 69), echt 0 (min Bucket -1) */
    { 0x30A0,  3,  98,   66 },   /* alt 62, falsch 7700 (max Bucket 66), echt 0 (min Bucket -1) */
    { 0x30A0,  3,  99,   59 },   /* alt 54, falsch 5672 (max Bucket 59), echt 0 (min Bucket -1) */
    { 0x30A0,  3, 100,   58 },   /* alt 52, falsch 25146 (max Bucket 58), echt 0 (min Bucket -1) */
    { 0x30A0,  3, 101,   54 },   /* alt 50, falsch 12926 (max Bucket 54), echt 0 (min Bucket -1) */
    { 0x30A0,  3, 102,   70 },   /* alt 65, falsch 6169 (max Bucket 70), echt 0 (min Bucket -1) */
    { 0x30A0,  3, 103,   64 },   /* alt 58, falsch 15119 (max Bucket 64), echt 0 (min Bucket -1) */
    { 0x30A0,  3, 104,   61 },   /* alt 56, falsch 24633 (max Bucket 61), echt 0 (min Bucket -1) */
    { 0x30A0,  4,   8,   49 },   /* alt 47, falsch 13654 (max Bucket 49), echt 0 (min Bucket -1) */
    { 0x30A0,  4,  10,   49 },   /* alt 47, falsch 1512 (max Bucket 49), echt 0 (min Bucket -1) */
    { 0x30A0,  4,  12,   50 },   /* alt 47, falsch 1320 (max Bucket 50), echt 0 (min Bucket -1) */
    { 0x30A0,  4,  13,   49 },   /* alt 47, falsch 1211 (max Bucket 49), echt 0 (min Bucket -1) */
    { 0x30A0,  4,  34,   48 },   /* alt 47, falsch 695 (max Bucket 48), echt 0 (min Bucket -1) */
    { 0x30A0,  4,  36,   49 },   /* alt 47, falsch 621 (max Bucket 49), echt 0 (min Bucket -1) */
    { 0x30A0,  4,  37,   49 },   /* alt 47, falsch 688 (max Bucket 49), echt 0 (min Bucket -1) */
    { 0x30A0,  4,  38,   48 },   /* alt 47, falsch 747 (max Bucket 48), echt 0 (min Bucket -1) */
    { 0x30A0,  4,  39,   50 },   /* alt 48, falsch 878 (max Bucket 50), echt 0 (min Bucket -1) */
    { 0x30A0,  4,  40,   50 },   /* alt 49, falsch 646 (max Bucket 50), echt 0 (min Bucket -1) */
    { 0x30A1,  0,   0,  107 },   /* alt 83, falsch 710767 (max Bucket 107), echt 134494 (min Bucket 93), echt teils nicht mehr verdeckt */
    { 0x30A1,  0,   4,  103 },   /* alt 83, falsch 417025 (max Bucket 103), echt 179414 (min Bucket 92), echt teils nicht mehr verdeckt */
    { 0x30A1,  0,   5,  101 },   /* alt 89, falsch 37 (max Bucket 101), echt 7080 (min Bucket 109) */
    { 0x30A1,  0,  19,  163 },   /* alt 122, falsch 703116 (max Bucket 163), echt 27076 (min Bucket 136), echt teils nicht mehr verdeckt */
    { 0x30A1,  0,  20,  154 },   /* alt 134, falsch 1025 (max Bucket 154), echt 0 (min Bucket -1) */
    { 0x30A1,  0,  21,  154 },   /* alt 141, falsch 12 (max Bucket 154), echt 0 (min Bucket -1) */
    { 0x30A1,  0,  22,  143 },   /* alt 113, falsch 297260 (max Bucket 143), echt 14111 (min Bucket 139), echt teils nicht mehr verdeckt */
    { 0x30A1,  0,  23,  122 },   /* alt 106, falsch 71397 (max Bucket 122), echt 1 (min Bucket 142) */
    { 0x30A1,  0,  24,  165 },   /* alt 143, falsch 101676 (max Bucket 165), echt 0 (min Bucket -1) */
    { 0x30A1,  0,  25,  158 },   /* alt 137, falsch 26092 (max Bucket 158), echt 0 (min Bucket -1) */
    { 0x30A1,  0,  26,  132 },   /* alt 118, falsch 20566 (max Bucket 132), echt 0 (min Bucket -1) */
    { 0x30A1,  0,  27,  115 },   /* alt 103, falsch 17602 (max Bucket 115), echt 0 (min Bucket -1) */
    { 0x30A1,  0,  28,  127 },   /* alt 115, falsch 6104 (max Bucket 127), echt 0 (min Bucket -1) */
    { 0x30A1,  0,  29,  110 },   /* alt 101, falsch 5226 (max Bucket 110), echt 0 (min Bucket -1) */
    { 0x30A1,  0,  30,  150 },   /* alt 134, falsch 6870 (max Bucket 150), echt 0 (min Bucket -1) */
    { 0x30A1,  0,  31,   75 },   /* alt 62, falsch 42501 (max Bucket 75), echt 0 (min Bucket -1) */
    { 0x30A1,  0,  32,   77 },   /* alt 62, falsch 16431 (max Bucket 77), echt 0 (min Bucket -1) */
    { 0x30A1,  0,  33,   72 },   /* alt 62, falsch 1042 (max Bucket 72), echt 0 (min Bucket -1) */
    { 0x30A1,  0,  34,   79 },   /* alt 62, falsch 6670 (max Bucket 79), echt 0 (min Bucket -1) */
    { 0x30A1,  0,  35,   80 },   /* alt 62, falsch 1975 (max Bucket 80), echt 0 (min Bucket -1) */
    { 0x30A1,  0,  36,   79 },   /* alt 62, falsch 1610 (max Bucket 79), echt 0 (min Bucket -1) */
    { 0x30A1,  0,  37,   76 },   /* alt 62, falsch 833 (max Bucket 76), echt 0 (min Bucket -1) */
    { 0x30A1,  0,  38,   89 },   /* alt 80, falsch 165531 (max Bucket 89), echt 1515 (min Bucket 97) */
    { 0x30A1,  0,  39,   97 },   /* alt 87, falsch 36296 (max Bucket 97), echt 0 (min Bucket -1) */
    { 0x30A1,  0,  40,   86 },   /* alt 78, falsch 22079 (max Bucket 86), echt 0 (min Bucket -1) */
    { 0x30A1,  0,  41,   83 },   /* alt 76, falsch 9543 (max Bucket 83), echt 0 (min Bucket -1) */
    { 0x30A1,  0,  42,  103 },   /* alt 93, falsch 10754 (max Bucket 103), echt 0 (min Bucket -1) */
    { 0x30A1,  0,  43,   94 },   /* alt 86, falsch 7800 (max Bucket 94), echt 0 (min Bucket -1) */
    { 0x30A1,  0,  51,  163 },   /* alt 157, falsch 7244 (max Bucket 163), echt 0 (min Bucket -1) */
    { 0x30A1,  0,  75,  165 },   /* alt 157, falsch 5429 (max Bucket 165), echt 0 (min Bucket -1) */
    { 0x30A1,  0,  78,  166 },   /* alt 151, falsch 2355 (max Bucket 166), echt 0 (min Bucket -1) */
    { 0x30A1,  0,  79,  163 },   /* alt 146, falsch 7101 (max Bucket 163), echt 0 (min Bucket -1) */
    { 0x30A1,  0,  81,  155 },   /* alt 140, falsch 1609 (max Bucket 155), echt 0 (min Bucket -1) */
    { 0x30A1,  0,  82,  148 },   /* alt 134, falsch 2981 (max Bucket 148), echt 0 (min Bucket -1) */
    { 0x30A1,  0,  83,  141 },   /* alt 127, falsch 1751 (max Bucket 141), echt 0 (min Bucket -1) */
    { 0x30A1,  0,  84,  136 },   /* alt 125, falsch 5355 (max Bucket 136), echt 0 (min Bucket -1) */
    { 0x30A1,  0,  87,  131 },   /* alt 119, falsch 2508 (max Bucket 131), echt 0 (min Bucket -1) */
    { 0x30A1,  0,  88,  125 },   /* alt 112, falsch 2637 (max Bucket 125), echt 0 (min Bucket -1) */
    { 0x30A1,  0,  89,  118 },   /* alt 108, falsch 4534 (max Bucket 118), echt 0 (min Bucket -1) */
    { 0x30A1,  0,  91,  113 },   /* alt 104, falsch 2156 (max Bucket 113), echt 0 (min Bucket -1) */
    { 0x30A1,  0,  92,  108 },   /* alt 100, falsch 1300 (max Bucket 108), echt 0 (min Bucket -1) */
    { 0x30A1,  0,  93,  105 },   /* alt 97, falsch 3821 (max Bucket 105), echt 81 (min Bucket 96), echt teils nicht mehr verdeckt */
    { 0x30A1,  0,  95,  101 },   /* alt 92, falsch 4942 (max Bucket 101), echt 0 (min Bucket -1) */
    { 0x30A1,  0,  96,   96 },   /* alt 89, falsch 1812 (max Bucket 96), echt 0 (min Bucket -1) */
    { 0x30A1,  0,  98,   93 },   /* alt 85, falsch 4507 (max Bucket 93), echt 0 (min Bucket -1) */
    { 0x30A1,  0,  99,   89 },   /* alt 81, falsch 4995 (max Bucket 89), echt 0 (min Bucket -1) */
    { 0x30A1,  0, 100,   84 },   /* alt 78, falsch 7305 (max Bucket 84), echt 0 (min Bucket -1) */
    { 0x30A1,  0, 101,   82 },   /* alt 62, falsch 2930 (max Bucket 82), echt 0 (min Bucket -1) */
    { 0x30A1,  0, 102,   77 },   /* alt 62, falsch 736 (max Bucket 77), echt 0 (min Bucket -1) */
    { 0x30A1,  0, 103,   76 },   /* alt 62, falsch 559 (max Bucket 76), echt 0 (min Bucket -1) */
    { 0x30A1,  0, 104,   74 },   /* alt 62, falsch 882 (max Bucket 74), echt 0 (min Bucket -1) */
    { 0x30A1,  1,   0,   94 },   /* alt 72, falsch 1249851 (max Bucket 94), echt 58664 (min Bucket 106) */
    { 0x30A1,  1,   1,   97 },   /* alt 92, falsch 25661 (max Bucket 97), echt 1391 (min Bucket 111) */
    { 0x30A1,  1,   2,   65 },   /* alt 62, falsch 8705 (max Bucket 65), echt 0 (min Bucket -1) */
    { 0x30A1,  1,   3,   94 },   /* alt 91, falsch 7874 (max Bucket 94), echt 2864 (min Bucket 108) */
    { 0x30A1,  1,   4,   88 },   /* alt 71, falsch 478118 (max Bucket 88), echt 63621 (min Bucket 102) */
    { 0x30A1,  1,   7,   76 },   /* alt 65, falsch 345744 (max Bucket 76), echt 0 (min Bucket -1) */
    { 0x30A1,  1,   8,   85 },   /* alt 77, falsch 105538 (max Bucket 85), echt 0 (min Bucket -1) */
    { 0x30A1,  1,   9,   68 },   /* alt 62, falsch 81082 (max Bucket 68), echt 0 (min Bucket -1) */
    { 0x30A1,  1,  10,   80 },   /* alt 75, falsch 27590 (max Bucket 80), echt 0 (min Bucket -1) */
    { 0x30A1,  1,  11,   91 },   /* alt 84, falsch 28974 (max Bucket 91), echt 0 (min Bucket -1) */
    { 0x30A1,  1,  12,   65 },   /* alt 60, falsch 20561 (max Bucket 65), echt 0 (min Bucket -1) */
    { 0x30A1,  1,  13,   71 },   /* alt 67, falsch 21838 (max Bucket 71), echt 0 (min Bucket -1) */
    { 0x30A1,  1,  14,   79 },   /* alt 74, falsch 8609 (max Bucket 79), echt 0 (min Bucket -1) */
    { 0x30A1,  1,  15,   73 },   /* alt 69, falsch 8086 (max Bucket 73), echt 0 (min Bucket -1) */
    { 0x30A1,  1,  16,   89 },   /* alt 83, falsch 8387 (max Bucket 89), echt 0 (min Bucket -1) */
    { 0x30A1,  1,  17,   63 },   /* alt 60, falsch 6056 (max Bucket 63), echt 0 (min Bucket -1) */
    { 0x30A1,  1,  18,   83 },   /* alt 78, falsch 6025 (max Bucket 83), echt 0 (min Bucket -1) */
    { 0x30A1,  1,  19,   93 },   /* alt 89, falsch 5977 (max Bucket 93), echt 0 (min Bucket -1) */
    { 0x30A1,  1,  20,   66 },   /* alt 63, falsch 4330 (max Bucket 66), echt 0 (min Bucket -1) */
    { 0x30A1,  1,  28,   76 },   /* alt 72, falsch 82413 (max Bucket 76), echt 0 (min Bucket -1) */
    { 0x30A1,  1,  29,   73 },   /* alt 71, falsch 16521 (max Bucket 73), echt 0 (min Bucket -1) */
    { 0x30A1,  1,  31,   76 },   /* alt 73, falsch 17373 (max Bucket 76), echt 0 (min Bucket -1) */
    { 0x30A1,  1,  32,   76 },   /* alt 72, falsch 14353 (max Bucket 76), echt 0 (min Bucket -1) */
    { 0x30A1,  1,  46,   94 },   /* alt 92, falsch 4176 (max Bucket 94), echt 1369 (min Bucket 109) */
    { 0x30A1,  1,  52,   97 },   /* alt 93, falsch 4431 (max Bucket 97), echt 1 (min Bucket 113) */
    { 0x30A1,  1,  53,   94 },   /* alt 90, falsch 1437 (max Bucket 94), echt 0 (min Bucket -1) */
    { 0x30A1,  1,  54,   95 },   /* alt 91, falsch 1804 (max Bucket 95), echt 0 (min Bucket -1) */
    { 0x30A1,  1,  55,   95 },   /* alt 92, falsch 689 (max Bucket 95), echt 0 (min Bucket -1) */
    { 0x30A1,  1,  56,   72 },   /* alt 71, falsch 1479 (max Bucket 72), echt 0 (min Bucket -1) */
    { 0x30A1,  1,  58,   74 },   /* alt 72, falsch 4306 (max Bucket 74), echt 0 (min Bucket -1) */
    { 0x30A1,  1,  60,   91 },   /* alt 88, falsch 1030 (max Bucket 91), echt 0 (min Bucket -1) */
    { 0x30A1,  1,  61,   93 },   /* alt 89, falsch 661 (max Bucket 93), echt 0 (min Bucket -1) */
    { 0x30A1,  1,  64,   88 },   /* alt 85, falsch 1344 (max Bucket 88), echt 0 (min Bucket -1) */
    { 0x30A1,  1,  65,   90 },   /* alt 86, falsch 1691 (max Bucket 90), echt 0 (min Bucket -1) */
    { 0x30A1,  1,  66,   90 },   /* alt 87, falsch 654 (max Bucket 90), echt 0 (min Bucket -1) */
    { 0x30A1,  1,  68,   76 },   /* alt 72, falsch 2253 (max Bucket 76), echt 0 (min Bucket -1) */
    { 0x30A1,  1,  69,   76 },   /* alt 73, falsch 1824 (max Bucket 76), echt 0 (min Bucket -1) */
    { 0x30A1,  1,  70,   86 },   /* alt 82, falsch 1309 (max Bucket 86), echt 0 (min Bucket -1) */
    { 0x30A1,  1,  71,   86 },   /* alt 83, falsch 1658 (max Bucket 86), echt 0 (min Bucket -1) */
    { 0x30A1,  1,  72,   87 },   /* alt 84, falsch 640 (max Bucket 87), echt 0 (min Bucket -1) */
    { 0x30A1,  1,  73,   76 },   /* alt 74, falsch 1208 (max Bucket 76), echt 0 (min Bucket -1) */
    { 0x30A1,  1,  75,   76 },   /* alt 75, falsch 3881 (max Bucket 76), echt 0 (min Bucket -1) */
    { 0x30A1,  1,  76,   83 },   /* alt 80, falsch 961 (max Bucket 83), echt 0 (min Bucket -1) */
    { 0x30A1,  1,  77,   84 },   /* alt 81, falsch 630 (max Bucket 84), echt 0 (min Bucket -1) */
    { 0x30A1,  1,  79,   82 },   /* alt 78, falsch 2498 (max Bucket 82), echt 0 (min Bucket -1) */
    { 0x30A1,  1,  80,   81 },   /* alt 79, falsch 619 (max Bucket 81), echt 0 (min Bucket -1) */
    { 0x30A1,  1,  83,   79 },   /* alt 75, falsch 1223 (max Bucket 79), echt 0 (min Bucket -1) */
    { 0x30A1,  1,  84,   80 },   /* alt 76, falsch 1834 (max Bucket 80), echt 0 (min Bucket -1) */
    { 0x30A1,  1,  85,   76 },   /* alt 72, falsch 1477 (max Bucket 76), echt 0 (min Bucket -1) */
    { 0x30A1,  1,  86,   76 },   /* alt 73, falsch 2076 (max Bucket 76), echt 0 (min Bucket -1) */
    { 0x30A1,  1,  87,   77 },   /* alt 74, falsch 1205 (max Bucket 77), echt 0 (min Bucket -1) */
    { 0x30A1,  1,  88,   74 },   /* alt 72, falsch 923 (max Bucket 74), echt 0 (min Bucket -1) */
    { 0x30A1,  1,  89,   74 },   /* alt 73, falsch 706 (max Bucket 74), echt 0 (min Bucket -1) */
    { 0x30A1,  1,  91,   73 },   /* alt 70, falsch 1150 (max Bucket 73), echt 0 (min Bucket -1) */
    { 0x30A1,  1,  92,   75 },   /* alt 71, falsch 1755 (max Bucket 75), echt 0 (min Bucket -1) */
    { 0x30A1,  1,  93,   75 },   /* alt 71, falsch 2647 (max Bucket 75), echt 0 (min Bucket -1) */
    { 0x30A1,  1,  94,   72 },   /* alt 68, falsch 2222 (max Bucket 72), echt 0 (min Bucket -1) */
    { 0x30A1,  1,  95,   72 },   /* alt 69, falsch 1688 (max Bucket 72), echt 0 (min Bucket -1) */
    { 0x30A1,  1,  96,   69 },   /* alt 65, falsch 1615 (max Bucket 69), echt 0 (min Bucket -1) */
    { 0x30A1,  1,  97,   69 },   /* alt 66, falsch 3217 (max Bucket 69), echt 0 (min Bucket -1) */
    { 0x30A1,  1,  98,   70 },   /* alt 67, falsch 1653 (max Bucket 70), echt 0 (min Bucket -1) */
    { 0x30A1,  1,  99,   68 },   /* alt 64, falsch 2353 (max Bucket 68), echt 0 (min Bucket -1) */
    { 0x30A1,  1, 100,   65 },   /* alt 62, falsch 1066 (max Bucket 65), echt 0 (min Bucket -1) */
    { 0x30A1,  1, 101,   66 },   /* alt 63, falsch 814 (max Bucket 66), echt 0 (min Bucket -1) */
    { 0x30A1,  1, 102,   65 },   /* alt 61, falsch 2005 (max Bucket 65), echt 0 (min Bucket -1) */
    { 0x30A1,  1, 103,   62 },   /* alt 59, falsch 1739 (max Bucket 62), echt 0 (min Bucket -1) */
    { 0x30A1,  1, 104,   63 },   /* alt 60, falsch 1021 (max Bucket 63), echt 0 (min Bucket -1) */
    { 0x30A1,  3,  20,   80 },   /* alt 60, falsch 555270 (max Bucket 80), echt 427 (min Bucket 107) */
    { 0x30A1,  3,  21,   94 },   /* alt 89, falsch 127930 (max Bucket 94), echt 13199 (min Bucket 109) */
    { 0x30A1,  3,  27,   72 },   /* alt 57, falsch 447036 (max Bucket 72), echt 0 (min Bucket -1) */
    { 0x30A1,  3,  28,   98 },   /* alt 81, falsch 265627 (max Bucket 98), echt 4373 (min Bucket 117) */
    { 0x30A1,  3,  29,   89 },   /* alt 76, falsch 83801 (max Bucket 89), echt 0 (min Bucket -1) */
    { 0x30A1,  3,  30,   72 },   /* alt 57, falsch 21924 (max Bucket 72), echt 0 (min Bucket -1) */
    { 0x30A1,  3,  31,   70 },   /* alt 58, falsch 9395 (max Bucket 70), echt 0 (min Bucket -1) */
    { 0x30A1,  3,  32,   71 },   /* alt 62, falsch 6478 (max Bucket 71), echt 0 (min Bucket -1) */
    { 0x30A1,  3,  33,   93 },   /* alt 90, falsch 7491 (max Bucket 93), echt 0 (min Bucket -1) */
    { 0x30A1,  3,  34,   57 },   /* alt 52, falsch 4417 (max Bucket 57), echt 0 (min Bucket -1) */
    { 0x30A1,  3,  35,   71 },   /* alt 70, falsch 3848 (max Bucket 71), echt 0 (min Bucket -1) */
    { 0x30A1,  3,  36,   60 },   /* alt 52, falsch 238117 (max Bucket 60), echt 0 (min Bucket -1) */
    { 0x30A1,  3,  37,   65 },   /* alt 59, falsch 94506 (max Bucket 65), echt 0 (min Bucket -1) */
    { 0x30A1,  3,  38,   76 },   /* alt 67, falsch 70868 (max Bucket 76), echt 0 (min Bucket -1) */
    { 0x30A1,  3,  39,   52 },   /* alt 50, falsch 28783 (max Bucket 52), echt 0 (min Bucket -1) */
    { 0x30A1,  3,  40,   66 },   /* alt 60, falsch 37134 (max Bucket 66), echt 0 (min Bucket -1) */
    { 0x30A1,  3,  41,   85 },   /* alt 70, falsch 42415 (max Bucket 85), echt 0 (min Bucket -1) */
    { 0x30A1,  3,  42,   54 },   /* alt 50, falsch 23661 (max Bucket 54), echt 0 (min Bucket -1) */
    { 0x30A1,  3,  43,   57 },   /* alt 52, falsch 20365 (max Bucket 57), echt 0 (min Bucket -1) */
    { 0x30A1,  3,  44,   73 },   /* alt 65, falsch 21973 (max Bucket 73), echt 0 (min Bucket -1) */
    { 0x30A1,  3,  45,   69 },   /* alt 58, falsch 3200 (max Bucket 69), echt 0 (min Bucket -1) */
    { 0x30A1,  3,  78,   99 },   /* alt 97, falsch 5857 (max Bucket 99), echt 80 (min Bucket 117) */
    { 0x30A1,  3,  81,   75 },   /* alt 70, falsch 4781 (max Bucket 75), echt 0 (min Bucket -1) */
    { 0x30A1,  3,  82,   94 },   /* alt 93, falsch 2512 (max Bucket 94), echt 0 (min Bucket -1) */
    { 0x30A1,  3,  83,   92 },   /* alt 89, falsch 3657 (max Bucket 92), echt 0 (min Bucket -1) */
    { 0x30A1,  3,  84,   89 },   /* alt 85, falsch 386 (max Bucket 89), echt 0 (min Bucket -1) */
    { 0x30A1,  3,  85,   88 },   /* alt 83, falsch 984 (max Bucket 88), echt 0 (min Bucket -1) */
    { 0x30A1,  3,  86,   84 },   /* alt 81, falsch 945 (max Bucket 84), echt 0 (min Bucket -1) */
    { 0x30A1,  3,  87,   82 },   /* alt 77, falsch 1757 (max Bucket 82), echt 0 (min Bucket -1) */
    { 0x30A1,  3,  88,   77 },   /* alt 73, falsch 896 (max Bucket 77), echt 0 (min Bucket -1) */
    { 0x30A1,  3,  89,   88 },   /* alt 82, falsch 2434 (max Bucket 88), echt 0 (min Bucket -1) */
    { 0x30A1,  3,  91,   85 },   /* alt 81, falsch 385 (max Bucket 85), echt 0 (min Bucket -1) */
    { 0x30A1,  3,  92,   84 },   /* alt 77, falsch 8378 (max Bucket 84), echt 0 (min Bucket -1) */
    { 0x30A1,  3,  93,   81 },   /* alt 75, falsch 11475 (max Bucket 81), echt 0 (min Bucket -1) */
    { 0x30A1,  3,  94,   78 },   /* alt 73, falsch 12561 (max Bucket 78), echt 0 (min Bucket -1) */
    { 0x30A1,  3,  95,   76 },   /* alt 69, falsch 6853 (max Bucket 76), echt 0 (min Bucket -1) */
    { 0x30A1,  3,  96,   72 },   /* alt 67, falsch 4039 (max Bucket 72), echt 0 (min Bucket -1) */
    { 0x30A1,  3,  97,   69 },   /* alt 63, falsch 13641 (max Bucket 69), echt 0 (min Bucket -1) */
    { 0x30A1,  3,  98,   66 },   /* alt 62, falsch 7700 (max Bucket 66), echt 0 (min Bucket -1) */
    { 0x30A1,  3,  99,   59 },   /* alt 54, falsch 5672 (max Bucket 59), echt 0 (min Bucket -1) */
    { 0x30A1,  3, 100,   58 },   /* alt 52, falsch 25146 (max Bucket 58), echt 0 (min Bucket -1) */
    { 0x30A1,  3, 101,   54 },   /* alt 50, falsch 12926 (max Bucket 54), echt 0 (min Bucket -1) */
    { 0x30A1,  3, 102,   70 },   /* alt 65, falsch 6169 (max Bucket 70), echt 0 (min Bucket -1) */
    { 0x30A1,  3, 103,   64 },   /* alt 58, falsch 15119 (max Bucket 64), echt 0 (min Bucket -1) */
    { 0x30A1,  3, 104,   61 },   /* alt 56, falsch 24633 (max Bucket 61), echt 0 (min Bucket -1) */
    { 0x30A1,  4,   8,   49 },   /* alt 47, falsch 13654 (max Bucket 49), echt 0 (min Bucket -1) */
    { 0x30A1,  4,  10,   49 },   /* alt 47, falsch 1512 (max Bucket 49), echt 0 (min Bucket -1) */
    { 0x30A1,  4,  12,   50 },   /* alt 47, falsch 1320 (max Bucket 50), echt 0 (min Bucket -1) */
    { 0x30A1,  4,  13,   49 },   /* alt 47, falsch 1211 (max Bucket 49), echt 0 (min Bucket -1) */
    { 0x30A1,  4,  34,   48 },   /* alt 47, falsch 695 (max Bucket 48), echt 0 (min Bucket -1) */
    { 0x30A1,  4,  36,   49 },   /* alt 47, falsch 621 (max Bucket 49), echt 0 (min Bucket -1) */
    { 0x30A1,  4,  37,   49 },   /* alt 47, falsch 688 (max Bucket 49), echt 0 (min Bucket -1) */
    { 0x30A1,  4,  38,   48 },   /* alt 47, falsch 747 (max Bucket 48), echt 0 (min Bucket -1) */
    { 0x30A1,  4,  39,   50 },   /* alt 48, falsch 878 (max Bucket 50), echt 0 (min Bucket -1) */
    { 0x30A1,  4,  40,   50 },   /* alt 49, falsch 646 (max Bucket 50), echt 0 (min Bucket -1) */
    { 0x30E0,  6,   0,   87 },   /* alt 57, falsch 522963 (max Bucket 87), echt 0 (min Bucket -1) */
    { 0x30E0,  6,   1,   73 },   /* alt 56, falsch 142129 (max Bucket 73), echt 0 (min Bucket -1) */
    { 0x30E0,  6,   2,   63 },   /* alt 56, falsch 43239 (max Bucket 63), echt 0 (min Bucket -1) */
    { 0x30E0,  6,   3,   92 },   /* alt 57, falsch 58271 (max Bucket 92), echt 0 (min Bucket -1) */
    { 0x30E0,  6,   4,   63 },   /* alt 56, falsch 23937 (max Bucket 63), echt 0 (min Bucket -1) */
    { 0x30E0,  6,   5,   74 },   /* alt 56, falsch 22932 (max Bucket 74), echt 0 (min Bucket -1) */
    { 0x30E0,  6,   6,   63 },   /* alt 57, falsch 11780 (max Bucket 63), echt 0 (min Bucket -1) */
    { 0x30E0,  6,   7,   93 },   /* alt 57, falsch 15903 (max Bucket 93), echt 0 (min Bucket -1) */
    { 0x30E0,  6,   8,   82 },   /* alt 56, falsch 9843 (max Bucket 82), echt 0 (min Bucket -1) */
    { 0x30E0,  6,   9,   68 },   /* alt 56, falsch 5651 (max Bucket 68), echt 0 (min Bucket -1) */
    { 0x30E0,  6,  10,   74 },   /* alt 56, falsch 5825 (max Bucket 74), echt 0 (min Bucket -1) */
    { 0x30E0,  6,  11,  132 },   /* alt 84, falsch 574268 (max Bucket 132), echt 0 (min Bucket -1) */
    { 0x30E0,  6,  12,  109 },   /* alt 57, falsch 90556 (max Bucket 109), echt 0 (min Bucket -1) */
    { 0x30E0,  6,  13,   94 },   /* alt 58, falsch 20554 (max Bucket 94), echt 0 (min Bucket -1) */
    { 0x30E0,  6,  14,  111 },   /* alt 57, falsch 21336 (max Bucket 111), echt 0 (min Bucket -1) */
    { 0x30E0,  6,  15,   88 },   /* alt 56, falsch 7651 (max Bucket 88), echt 0 (min Bucket -1) */
    { 0x30E0,  6,  16,  115 },   /* alt 57, falsch 966 (max Bucket 115), echt 0 (min Bucket -1) */
    { 0x30E0,  6,  17,  111 },   /* alt 57, falsch 7226 (max Bucket 111), echt 0 (min Bucket -1) */
    { 0x30E0,  6,  18,  102 },   /* alt 57, falsch 3797 (max Bucket 102), echt 0 (min Bucket -1) */
    { 0x30E0,  6,  19,   99 },   /* alt 58, falsch 1232 (max Bucket 99), echt 0 (min Bucket -1) */
    { 0x30E0,  6,  20,   96 },   /* alt 58, falsch 802 (max Bucket 96), echt 0 (min Bucket -1) */
    { 0x30E0,  6,  21,   94 },   /* alt 58, falsch 6069 (max Bucket 94), echt 0 (min Bucket -1) */
    { 0x30E0,  6,  22,   88 },   /* alt 56, falsch 1803 (max Bucket 88), echt 0 (min Bucket -1) */
    { 0x30E0,  6,  23,   85 },   /* alt 56, falsch 356 (max Bucket 85), echt 0 (min Bucket -1) */
    { 0x30E0,  6,  24,   82 },   /* alt 56, falsch 1003 (max Bucket 82), echt 0 (min Bucket -1) */
    { 0x30E0,  6,  25,   93 },   /* alt 57, falsch 3713 (max Bucket 93), echt 0 (min Bucket -1) */
    { 0x30E0,  6,  26,   92 },   /* alt 83, falsch 2764 (max Bucket 92), echt 0 (min Bucket -1) */
    { 0x30E0,  6,  27,   84 },   /* alt 56, falsch 696 (max Bucket 84), echt 0 (min Bucket -1) */
    { 0x30E0,  6,  28,   78 },   /* alt 56, falsch 2808 (max Bucket 78), echt 0 (min Bucket -1) */
    { 0x30E0,  6,  29,   72 },   /* alt 56, falsch 586 (max Bucket 72), echt 0 (min Bucket -1) */
    { 0x30E0,  6,  30,   71 },   /* alt 56, falsch 854 (max Bucket 71), echt 0 (min Bucket -1) */
    { 0x30E0,  6,  31,   68 },   /* alt 56, falsch 829 (max Bucket 68), echt 0 (min Bucket -1) */
    { 0x30E0,  6,  32,   66 },   /* alt 56, falsch 2345 (max Bucket 66), echt 0 (min Bucket -1) */
    { 0x30E0,  6,  33,   63 },   /* alt 56, falsch 4468 (max Bucket 63), echt 0 (min Bucket -1) */
    { 0x30E0,  6,  34,   61 },   /* alt 56, falsch 3091 (max Bucket 61), echt 0 (min Bucket -1) */
    { 0x30E0,  7,   0,   76 },   /* alt 41, falsch 863578 (max Bucket 76), echt 0 (min Bucket -1) */
    { 0x30E0,  7,   1,   75 },   /* alt 41, falsch 460947 (max Bucket 75), echt 0 (min Bucket -1) */
    { 0x30E0,  7,   2,   99 },   /* alt 41, falsch 523508 (max Bucket 99), echt 0 (min Bucket -1) */
    { 0x30E0,  7,   3,   77 },   /* alt 41, falsch 114666 (max Bucket 77), echt 0 (min Bucket -1) */
    { 0x30E0,  7,   4,   79 },   /* alt 41, falsch 74862 (max Bucket 79), echt 0 (min Bucket -1) */
    { 0x30E0,  7,   5,   77 },   /* alt 49, falsch 53897 (max Bucket 77), echt 0 (min Bucket -1) */
    { 0x30E0,  7,   6,   76 },   /* alt 41, falsch 45867 (max Bucket 76), echt 0 (min Bucket -1) */
    { 0x30E0,  7,   7,  103 },   /* alt 41, falsch 49854 (max Bucket 103), echt 0 (min Bucket -1) */
    { 0x30E0,  7,   8,   82 },   /* alt 41, falsch 31922 (max Bucket 82), echt 0 (min Bucket -1) */
    { 0x30E0,  7,   9,   77 },   /* alt 41, falsch 17982 (max Bucket 77), echt 0 (min Bucket -1) */
    { 0x30E0,  7,  10,  100 },   /* alt 41, falsch 18544 (max Bucket 100), echt 0 (min Bucket -1) */
    { 0x30E0,  7,  11,   76 },   /* alt 41, falsch 14536 (max Bucket 76), echt 0 (min Bucket -1) */
    { 0x30E0,  7,  12,   75 },   /* alt 41, falsch 8172 (max Bucket 75), echt 0 (min Bucket -1) */
    { 0x30E0,  7,  13,   67 },   /* alt 41, falsch 8184 (max Bucket 67), echt 0 (min Bucket -1) */
    { 0x30E0,  7,  14,   60 },   /* alt 41, falsch 6312 (max Bucket 60), echt 0 (min Bucket -1) */
    { 0x30E0,  7,  15,   77 },   /* alt 56, falsch 7558 (max Bucket 77), echt 0 (min Bucket -1) */
    { 0x30E0,  7,  16,  106 },   /* alt 41, falsch 10884 (max Bucket 106), echt 0 (min Bucket -1) */
    { 0x30E0,  7,  17,   77 },   /* alt 41, falsch 6299 (max Bucket 77), echt 0 (min Bucket -1) */
    { 0x30E0,  7,  18,  101 },   /* alt 41, falsch 9426 (max Bucket 101), echt 0 (min Bucket -1) */
    { 0x30E0,  7,  19,   56 },   /* alt 41, falsch 4093 (max Bucket 56), echt 0 (min Bucket -1) */
    { 0x30E0,  7,  20,   77 },   /* alt 41, falsch 5770 (max Bucket 77), echt 0 (min Bucket -1) */
    { 0x30E0,  7,  21,   75 },   /* alt 41, falsch 4738 (max Bucket 75), echt 0 (min Bucket -1) */
    { 0x30E0,  7,  22,  167 },   /* alt 91, falsch 576465 (max Bucket 167), echt 0 (min Bucket -1) */
    { 0x30E0,  7,  23,  179 },   /* alt 91, falsch 67187 (max Bucket 179), echt 0 (min Bucket -1) */
    { 0x30E0,  7,  24,  135 },   /* alt 41, falsch 44435 (max Bucket 135), echt 0 (min Bucket -1) */
    { 0x30E0,  7,  25,  103 },   /* alt 93, falsch 16232 (max Bucket 103), echt 0 (min Bucket -1) */
    { 0x30E0,  7,  26,  101 },   /* alt 91, falsch 6774 (max Bucket 101), echt 0 (min Bucket -1) */
    { 0x30E0,  7,  27,  145 },   /* alt 41, falsch 3621 (max Bucket 145), echt 0 (min Bucket -1) */
    { 0x30E0,  7,  28,  146 },   /* alt 48, falsch 655 (max Bucket 146), echt 0 (min Bucket -1) */
    { 0x30E0,  7,  29,  148 },   /* alt 49, falsch 663 (max Bucket 148), echt 0 (min Bucket -1) */
    { 0x30E0,  7,  30,  167 },   /* alt 123, falsch 5076 (max Bucket 167), echt 0 (min Bucket -1) */
    { 0x30E0,  7,  31,  132 },   /* alt 41, falsch 1146 (max Bucket 132), echt 0 (min Bucket -1) */
    { 0x30E0,  7,  32,   92 },   /* alt 41, falsch 3011 (max Bucket 92), echt 0 (min Bucket -1) */
    { 0x30E0,  7,  33,  103 },   /* alt 41, falsch 1318 (max Bucket 103), echt 0 (min Bucket -1) */
    { 0x30E0,  7,  34,  105 },   /* alt 41, falsch 1338 (max Bucket 105), echt 0 (min Bucket -1) */
    { 0x30E0,  7,  35,   86 },   /* alt 41, falsch 3882 (max Bucket 86), echt 0 (min Bucket -1) */
    { 0x30E0,  7,  36,  103 },   /* alt 41, falsch 3116 (max Bucket 103), echt 0 (min Bucket -1) */
    { 0x30E0,  7,  37,  103 },   /* alt 41, falsch 492 (max Bucket 103), echt 0 (min Bucket -1) */
    { 0x30E0,  7,  38,  103 },   /* alt 48, falsch 495 (max Bucket 103), echt 0 (min Bucket -1) */
    { 0x30E0,  7,  39,  103 },   /* alt 49, falsch 2678 (max Bucket 103), echt 0 (min Bucket -1) */
    { 0x30E0,  7,  40,  103 },   /* alt 50, falsch 500 (max Bucket 103), echt 0 (min Bucket -1) */
    { 0x30E0,  7,  41,   88 },   /* alt 41, falsch 424 (max Bucket 88), echt 0 (min Bucket -1) */
    { 0x30E0,  7,  42,   82 },   /* alt 41, falsch 1376 (max Bucket 82), echt 0 (min Bucket -1) */
    { 0x30E0,  7,  43,   79 },   /* alt 41, falsch 1339 (max Bucket 79), echt 0 (min Bucket -1) */
    { 0x30E0,  7,  44,   76 },   /* alt 41, falsch 3444 (max Bucket 76), echt 0 (min Bucket -1) */
    { 0x30E0,  7,  45,   73 },   /* alt 41, falsch 1854 (max Bucket 73), echt 0 (min Bucket -1) */
    { 0x30E0,  7,  46,   70 },   /* alt 41, falsch 2333 (max Bucket 70), echt 0 (min Bucket -1) */
    { 0x30E0,  7,  47,   68 },   /* alt 41, falsch 2802 (max Bucket 68), echt 0 (min Bucket -1) */
    { 0x30E0,  7,  48,   65 },   /* alt 41, falsch 2951 (max Bucket 65), echt 0 (min Bucket -1) */
    { 0x30E0,  7,  49,   64 },   /* alt 54, falsch 1441 (max Bucket 64), echt 0 (min Bucket -1) */
    { 0x30E0,  7,  50,   55 },   /* alt 48, falsch 650 (max Bucket 55), echt 0 (min Bucket -1) */
    { 0x30E0,  7,  51,   55 },   /* alt 49, falsch 1850 (max Bucket 55), echt 0 (min Bucket -1) */
    { 0x30E0,  7,  52,   51 },   /* alt 41, falsch 2601 (max Bucket 51), echt 0 (min Bucket -1) */
    { 0x30E0,  8,   0,   91 },   /* alt 55, falsch 277373 (max Bucket 91), echt 0 (min Bucket -1) */
    { 0x30E0,  8,   1,   73 },   /* alt 57, falsch 21837 (max Bucket 73), echt 0 (min Bucket -1) */
    { 0x30E0,  8,   2,   61 },   /* alt 57, falsch 427 (max Bucket 61), echt 0 (min Bucket -1) */
    { 0x30E0,  8,   3,   73 },   /* alt 57, falsch 7986 (max Bucket 73), echt 0 (min Bucket -1) */
    { 0x30E0,  8,   4,   94 },   /* alt 55, falsch 13701 (max Bucket 94), echt 0 (min Bucket -1) */
    { 0x30E0,  8,   5,   73 },   /* alt 57, falsch 2471 (max Bucket 73), echt 0 (min Bucket -1) */
    { 0x30E0,  8,   7,   66 },   /* alt 57, falsch 834 (max Bucket 66), echt 0 (min Bucket -1) */
    { 0x30E0,  8,   8,   61 },   /* alt 57, falsch 156 (max Bucket 61), echt 0 (min Bucket -1) */
    { 0x30E0,  8,   9,   91 },   /* alt 56, falsch 4273 (max Bucket 91), echt 0 (min Bucket -1) */
    { 0x30E0,  8,  10,  124 },   /* alt 55, falsch 142067 (max Bucket 124), echt 0 (min Bucket -1) */
    { 0x30E0,  8,  11,   99 },   /* alt 55, falsch 28372 (max Bucket 99), echt 0 (min Bucket -1) */
    { 0x30E0,  8,  12,  128 },   /* alt 55, falsch 30240 (max Bucket 128), echt 0 (min Bucket -1) */
    { 0x30E0,  8,  13,   99 },   /* alt 55, falsch 9296 (max Bucket 99), echt 0 (min Bucket -1) */
    { 0x30E0,  8,  14,   83 },   /* alt 55, falsch 3701 (max Bucket 83), echt 0 (min Bucket -1) */
    { 0x30E0,  8,  15,  110 },   /* alt 55, falsch 5844 (max Bucket 110), echt 0 (min Bucket -1) */
    { 0x30E0,  8,  16,  121 },   /* alt 55, falsch 2945 (max Bucket 121), echt 0 (min Bucket -1) */
    { 0x30E0,  8,  17,  122 },   /* alt 56, falsch 352 (max Bucket 122), echt 0 (min Bucket -1) */
    { 0x30E0,  8,  18,  127 },   /* alt 84, falsch 1470 (max Bucket 127), echt 0 (min Bucket -1) */
    { 0x30E0,  8,  19,  110 },   /* alt 55, falsch 1143 (max Bucket 110), echt 0 (min Bucket -1) */
    { 0x30E0,  8,  20,  104 },   /* alt 55, falsch 2271 (max Bucket 104), echt 0 (min Bucket -1) */
    { 0x30E0,  8,  21,   99 },   /* alt 55, falsch 2420 (max Bucket 99), echt 0 (min Bucket -1) */
    { 0x30E0,  8,  22,   92 },   /* alt 55, falsch 3619 (max Bucket 92), echt 0 (min Bucket -1) */
    { 0x30E0,  8,  23,   83 },   /* alt 55, falsch 1422 (max Bucket 83), echt 0 (min Bucket -1) */
    { 0x30E0,  8,  24,   78 },   /* alt 55, falsch 456 (max Bucket 78), echt 0 (min Bucket -1) */
    { 0x30E0,  8,  25,   79 },   /* alt 55, falsch 844 (max Bucket 79), echt 0 (min Bucket -1) */
    { 0x30E0,  8,  26,   94 },   /* alt 55, falsch 438 (max Bucket 94), echt 0 (min Bucket -1) */
    { 0x30E0,  8,  27,   75 },   /* alt 55, falsch 285 (max Bucket 75), echt 0 (min Bucket -1) */
    { 0x30E0,  8,  28,   73 },   /* alt 57, falsch 1122 (max Bucket 73), echt 0 (min Bucket -1) */
    { 0x30E0,  8,  29,   69 },   /* alt 57, falsch 507 (max Bucket 69), echt 0 (min Bucket -1) */
    { 0x30E0,  8,  30,   66 },   /* alt 57, falsch 310 (max Bucket 66), echt 0 (min Bucket -1) */
    { 0x30E0,  8,  31,   64 },   /* alt 57, falsch 186 (max Bucket 64), echt 0 (min Bucket -1) */
    { 0x30E0,  8,  32,   61 },   /* alt 57, falsch 65 (max Bucket 61), echt 0 (min Bucket -1) */
    { 0x30E0,  8,  33,   58 },   /* alt 57, falsch 15 (max Bucket 58), echt 0 (min Bucket -1) */
    { 0x30E0,  9,   0,   89 },   /* alt 71, falsch 80665 (max Bucket 89), echt 0 (min Bucket -1) */
    { 0x30E0,  9,   1,   95 },   /* alt 75, falsch 100557 (max Bucket 95), echt 0 (min Bucket -1) */
    { 0x30E0,  9,   2,   81 },   /* alt 71, falsch 4793 (max Bucket 81), echt 0 (min Bucket -1) */
    { 0x30E0,  9,   3,   92 },   /* alt 72, falsch 30366 (max Bucket 92), echt 0 (min Bucket -1) */
    { 0x30E0,  9,   4,  102 },   /* alt 76, falsch 17864 (max Bucket 102), echt 0 (min Bucket -1) */
    { 0x30E0,  9,   5,   81 },   /* alt 72, falsch 5127 (max Bucket 81), echt 0 (min Bucket -1) */
    { 0x30E0,  9,   6,   93 },   /* alt 73, falsch 9738 (max Bucket 93), echt 0 (min Bucket -1) */
    { 0x30E0,  9,   7,   90 },   /* alt 71, falsch 2757 (max Bucket 90), echt 0 (min Bucket -1) */
    { 0x30E0,  9,   8,   84 },   /* alt 74, falsch 7018 (max Bucket 84), echt 0 (min Bucket -1) */
    { 0x30E0,  9,   9,  108 },   /* alt 72, falsch 187499 (max Bucket 108), echt 0 (min Bucket -1) */
    { 0x30E0,  9,  10,  109 },   /* alt 74, falsch 61121 (max Bucket 109), echt 0 (min Bucket -1) */
    { 0x30E0,  9,  11,   96 },   /* alt 71, falsch 15451 (max Bucket 96), echt 0 (min Bucket -1) */
    { 0x30E0,  9,  13,   91 },   /* alt 71, falsch 1198 (max Bucket 91), echt 0 (min Bucket -1) */
    { 0x30E0,  9,  14,  102 },   /* alt 71, falsch 6445 (max Bucket 102), echt 0 (min Bucket -1) */
    { 0x30E0,  9,  15,   96 },   /* alt 73, falsch 9572 (max Bucket 96), echt 0 (min Bucket -1) */
    { 0x30E0,  9,  16,  108 },   /* alt 75, falsch 11628 (max Bucket 108), echt 0 (min Bucket -1) */
    { 0x30E0,  9,  17,  108 },   /* alt 75, falsch 10857 (max Bucket 108), echt 0 (min Bucket -1) */
    { 0x30E0,  9,  18,  106 },   /* alt 74, falsch 7289 (max Bucket 106), echt 0 (min Bucket -1) */
    { 0x30E0,  9,  20,   92 },   /* alt 71, falsch 1826 (max Bucket 92), echt 0 (min Bucket -1) */
    { 0x30E0,  9,  21,   85 },   /* alt 71, falsch 156100 (max Bucket 85), echt 0 (min Bucket -1) */
    { 0x30E0,  9,  22,   88 },   /* alt 71, falsch 28972 (max Bucket 88), echt 0 (min Bucket -1) */
    { 0x30E0,  9,  23,   80 },   /* alt 71, falsch 15317 (max Bucket 80), echt 0 (min Bucket -1) */
    { 0x30E0,  9,  24,   73 },   /* alt 71, falsch 8896 (max Bucket 73), echt 0 (min Bucket -1) */
    { 0x30E0,  9,  25,   87 },   /* alt 71, falsch 8737 (max Bucket 87), echt 0 (min Bucket -1) */
    { 0x30E0,  9,  26,   84 },   /* alt 71, falsch 6343 (max Bucket 84), echt 0 (min Bucket -1) */
    { 0x30E0,  9,  27,   78 },   /* alt 71, falsch 4965 (max Bucket 78), echt 0 (min Bucket -1) */
    { 0x30E0,  9,  28,   85 },   /* alt 71, falsch 5417 (max Bucket 85), echt 0 (min Bucket -1) */
    { 0x30E0,  9,  29,   80 },   /* alt 71, falsch 3908 (max Bucket 80), echt 0 (min Bucket -1) */
    { 0x30E0,  9,  30,   79 },   /* alt 71, falsch 3818 (max Bucket 79), echt 0 (min Bucket -1) */
    { 0x30E0,  9,  31,  107 },   /* alt 75, falsch 1485 (max Bucket 107), echt 0 (min Bucket -1) */
    { 0x30E0,  9,  32,  105 },   /* alt 74, falsch 1166 (max Bucket 105), echt 0 (min Bucket -1) */
    { 0x30E0,  9,  33,  103 },   /* alt 73, falsch 3329 (max Bucket 103), echt 0 (min Bucket -1) */
    { 0x30E0,  9,  34,  106 },   /* alt 74, falsch 893 (max Bucket 106), echt 0 (min Bucket -1) */
    { 0x30E0,  9,  35,  102 },   /* alt 72, falsch 562 (max Bucket 102), echt 0 (min Bucket -1) */
    { 0x30E0,  9,  36,  101 },   /* alt 72, falsch 3180 (max Bucket 101), echt 0 (min Bucket -1) */
    { 0x30E0,  9,  37,  100 },   /* alt 71, falsch 3229 (max Bucket 100), echt 0 (min Bucket -1) */
    { 0x30E0,  9,  38,   97 },   /* alt 71, falsch 483 (max Bucket 97), echt 0 (min Bucket -1) */
    { 0x30E0,  9,  41,  103 },   /* alt 76, falsch 2157 (max Bucket 103), echt 0 (min Bucket -1) */
    { 0x30E0,  9,  42,  102 },   /* alt 75, falsch 608 (max Bucket 102), echt 0 (min Bucket -1) */
    { 0x30E0,  9,  43,  102 },   /* alt 75, falsch 4171 (max Bucket 102), echt 0 (min Bucket -1) */
    { 0x30E0,  9,  44,  100 },   /* alt 75, falsch 5769 (max Bucket 100), echt 0 (min Bucket -1) */
    { 0x30E0,  9,  45,   96 },   /* alt 74, falsch 846 (max Bucket 96), echt 0 (min Bucket -1) */
    { 0x30E0,  9,  46,   96 },   /* alt 74, falsch 3842 (max Bucket 96), echt 0 (min Bucket -1) */
    { 0x30E0,  9,  48,   95 },   /* alt 74, falsch 2403 (max Bucket 95), echt 0 (min Bucket -1) */
    { 0x30E0,  9,  49,   94 },   /* alt 73, falsch 1552 (max Bucket 94), echt 0 (min Bucket -1) */
    { 0x30E0,  9,  50,   94 },   /* alt 73, falsch 768 (max Bucket 94), echt 0 (min Bucket -1) */
    { 0x30E0,  9,  51,   94 },   /* alt 72, falsch 2402 (max Bucket 94), echt 0 (min Bucket -1) */
    { 0x30E0,  9,  52,   93 },   /* alt 72, falsch 2555 (max Bucket 93), echt 0 (min Bucket -1) */
    { 0x30E0,  9,  53,   82 },   /* alt 71, falsch 2937 (max Bucket 82), echt 0 (min Bucket -1) */
    { 0x30E0,  9,  54,   92 },   /* alt 71, falsch 439 (max Bucket 92), echt 0 (min Bucket -1) */
    { 0x30E0,  9,  55,   92 },   /* alt 71, falsch 827 (max Bucket 92), echt 0 (min Bucket -1) */
    { 0x30E0,  9,  56,   91 },   /* alt 71, falsch 1485 (max Bucket 91), echt 0 (min Bucket -1) */
    { 0x30E0,  9,  57,   91 },   /* alt 71, falsch 561 (max Bucket 91), echt 0 (min Bucket -1) */
    { 0x30E0,  9,  58,   88 },   /* alt 71, falsch 1529 (max Bucket 88), echt 0 (min Bucket -1) */
    { 0x30E0,  9,  62,   88 },   /* alt 71, falsch 3536 (max Bucket 88), echt 0 (min Bucket -1) */
    { 0x30E0,  9,  63,   76 },   /* alt 71, falsch 942 (max Bucket 76), echt 0 (min Bucket -1) */
    { 0x30E0,  9,  65,   73 },   /* alt 71, falsch 920 (max Bucket 73), echt 0 (min Bucket -1) */
    { 0x30E0,  9,  66,   86 },   /* alt 71, falsch 3950 (max Bucket 86), echt 0 (min Bucket -1) */
    { 0x30E0,  9,  67,   72 },   /* alt 71, falsch 3821 (max Bucket 72), echt 0 (min Bucket -1) */
    { 0x30E0,  9,  68,   84 },   /* alt 76, falsch 987 (max Bucket 84), echt 0 (min Bucket -1) */
    { 0x30E0,  9,  69,   84 },   /* alt 75, falsch 2148 (max Bucket 84), echt 0 (min Bucket -1) */
    { 0x30E0,  9,  70,   82 },   /* alt 74, falsch 2444 (max Bucket 82), echt 0 (min Bucket -1) */
    { 0x30E0,  9,  71,   82 },   /* alt 71, falsch 1641 (max Bucket 82), echt 0 (min Bucket -1) */
    { 0x30E0,  9,  73,   81 },   /* alt 73, falsch 2197 (max Bucket 81), echt 0 (min Bucket -1) */
    { 0x30E0,  9,  74,   81 },   /* alt 72, falsch 1618 (max Bucket 81), echt 0 (min Bucket -1) */
    { 0x30E0,  9,  75,   80 },   /* alt 71, falsch 967 (max Bucket 80), echt 0 (min Bucket -1) */
    { 0x30E0,  9,  76,   79 },   /* alt 71, falsch 1189 (max Bucket 79), echt 0 (min Bucket -1) */
    { 0x30E0,  9,  77,   79 },   /* alt 72, falsch 615 (max Bucket 79), echt 0 (min Bucket -1) */
    { 0x30E0,  9,  78,   79 },   /* alt 71, falsch 984 (max Bucket 79), echt 0 (min Bucket -1) */
    { 0x30E0,  9,  79,   78 },   /* alt 71, falsch 1169 (max Bucket 78), echt 0 (min Bucket -1) */
    { 0x30E1,  6,   0,   87 },   /* alt 57, falsch 522963 (max Bucket 87), echt 0 (min Bucket -1) */
    { 0x30E1,  6,   1,   73 },   /* alt 56, falsch 142129 (max Bucket 73), echt 0 (min Bucket -1) */
    { 0x30E1,  6,   2,   63 },   /* alt 56, falsch 43239 (max Bucket 63), echt 0 (min Bucket -1) */
    { 0x30E1,  6,   3,   92 },   /* alt 57, falsch 58271 (max Bucket 92), echt 0 (min Bucket -1) */
    { 0x30E1,  6,   4,   63 },   /* alt 56, falsch 23937 (max Bucket 63), echt 0 (min Bucket -1) */
    { 0x30E1,  6,   5,   74 },   /* alt 56, falsch 22932 (max Bucket 74), echt 0 (min Bucket -1) */
    { 0x30E1,  6,   6,   63 },   /* alt 57, falsch 11780 (max Bucket 63), echt 0 (min Bucket -1) */
    { 0x30E1,  6,   7,   93 },   /* alt 57, falsch 15903 (max Bucket 93), echt 0 (min Bucket -1) */
    { 0x30E1,  6,   8,   82 },   /* alt 56, falsch 9843 (max Bucket 82), echt 0 (min Bucket -1) */
    { 0x30E1,  6,   9,   68 },   /* alt 56, falsch 5651 (max Bucket 68), echt 0 (min Bucket -1) */
    { 0x30E1,  6,  10,   74 },   /* alt 56, falsch 5825 (max Bucket 74), echt 0 (min Bucket -1) */
    { 0x30E1,  6,  11,  132 },   /* alt 84, falsch 574268 (max Bucket 132), echt 0 (min Bucket -1) */
    { 0x30E1,  6,  12,  109 },   /* alt 57, falsch 90556 (max Bucket 109), echt 0 (min Bucket -1) */
    { 0x30E1,  6,  13,   94 },   /* alt 58, falsch 20554 (max Bucket 94), echt 0 (min Bucket -1) */
    { 0x30E1,  6,  14,  111 },   /* alt 57, falsch 21336 (max Bucket 111), echt 0 (min Bucket -1) */
    { 0x30E1,  6,  15,   88 },   /* alt 56, falsch 7651 (max Bucket 88), echt 0 (min Bucket -1) */
    { 0x30E1,  6,  16,  115 },   /* alt 57, falsch 966 (max Bucket 115), echt 0 (min Bucket -1) */
    { 0x30E1,  6,  17,  111 },   /* alt 57, falsch 7226 (max Bucket 111), echt 0 (min Bucket -1) */
    { 0x30E1,  6,  18,  102 },   /* alt 57, falsch 3797 (max Bucket 102), echt 0 (min Bucket -1) */
    { 0x30E1,  6,  19,   99 },   /* alt 58, falsch 1232 (max Bucket 99), echt 0 (min Bucket -1) */
    { 0x30E1,  6,  20,   96 },   /* alt 58, falsch 802 (max Bucket 96), echt 0 (min Bucket -1) */
    { 0x30E1,  6,  21,   94 },   /* alt 58, falsch 6069 (max Bucket 94), echt 0 (min Bucket -1) */
    { 0x30E1,  6,  22,   88 },   /* alt 56, falsch 1803 (max Bucket 88), echt 0 (min Bucket -1) */
    { 0x30E1,  6,  23,   85 },   /* alt 56, falsch 356 (max Bucket 85), echt 0 (min Bucket -1) */
    { 0x30E1,  6,  24,   82 },   /* alt 56, falsch 1003 (max Bucket 82), echt 0 (min Bucket -1) */
    { 0x30E1,  6,  25,   93 },   /* alt 57, falsch 3713 (max Bucket 93), echt 0 (min Bucket -1) */
    { 0x30E1,  6,  26,   92 },   /* alt 83, falsch 2764 (max Bucket 92), echt 0 (min Bucket -1) */
    { 0x30E1,  6,  27,   84 },   /* alt 56, falsch 696 (max Bucket 84), echt 0 (min Bucket -1) */
    { 0x30E1,  6,  28,   78 },   /* alt 56, falsch 2808 (max Bucket 78), echt 0 (min Bucket -1) */
    { 0x30E1,  6,  29,   72 },   /* alt 56, falsch 586 (max Bucket 72), echt 0 (min Bucket -1) */
    { 0x30E1,  6,  30,   71 },   /* alt 56, falsch 854 (max Bucket 71), echt 0 (min Bucket -1) */
    { 0x30E1,  6,  31,   68 },   /* alt 56, falsch 829 (max Bucket 68), echt 0 (min Bucket -1) */
    { 0x30E1,  6,  32,   66 },   /* alt 56, falsch 2345 (max Bucket 66), echt 0 (min Bucket -1) */
    { 0x30E1,  6,  33,   63 },   /* alt 56, falsch 4468 (max Bucket 63), echt 0 (min Bucket -1) */
    { 0x30E1,  6,  34,   61 },   /* alt 56, falsch 3091 (max Bucket 61), echt 0 (min Bucket -1) */
    { 0x30E1,  7,   0,   76 },   /* alt 41, falsch 863578 (max Bucket 76), echt 0 (min Bucket -1) */
    { 0x30E1,  7,   1,   75 },   /* alt 41, falsch 460947 (max Bucket 75), echt 0 (min Bucket -1) */
    { 0x30E1,  7,   2,   99 },   /* alt 41, falsch 523508 (max Bucket 99), echt 0 (min Bucket -1) */
    { 0x30E1,  7,   3,   77 },   /* alt 41, falsch 114666 (max Bucket 77), echt 0 (min Bucket -1) */
    { 0x30E1,  7,   4,   79 },   /* alt 41, falsch 74862 (max Bucket 79), echt 0 (min Bucket -1) */
    { 0x30E1,  7,   5,   77 },   /* alt 49, falsch 53897 (max Bucket 77), echt 0 (min Bucket -1) */
    { 0x30E1,  7,   6,   76 },   /* alt 41, falsch 45867 (max Bucket 76), echt 0 (min Bucket -1) */
    { 0x30E1,  7,   7,  103 },   /* alt 41, falsch 49854 (max Bucket 103), echt 0 (min Bucket -1) */
    { 0x30E1,  7,   8,   82 },   /* alt 41, falsch 31922 (max Bucket 82), echt 0 (min Bucket -1) */
    { 0x30E1,  7,   9,   77 },   /* alt 41, falsch 17982 (max Bucket 77), echt 0 (min Bucket -1) */
    { 0x30E1,  7,  10,  100 },   /* alt 41, falsch 18544 (max Bucket 100), echt 0 (min Bucket -1) */
    { 0x30E1,  7,  11,   76 },   /* alt 41, falsch 14536 (max Bucket 76), echt 0 (min Bucket -1) */
    { 0x30E1,  7,  12,   75 },   /* alt 41, falsch 8172 (max Bucket 75), echt 0 (min Bucket -1) */
    { 0x30E1,  7,  13,   67 },   /* alt 41, falsch 8184 (max Bucket 67), echt 0 (min Bucket -1) */
    { 0x30E1,  7,  14,   60 },   /* alt 41, falsch 6312 (max Bucket 60), echt 0 (min Bucket -1) */
    { 0x30E1,  7,  15,   77 },   /* alt 56, falsch 7558 (max Bucket 77), echt 0 (min Bucket -1) */
    { 0x30E1,  7,  16,  106 },   /* alt 41, falsch 10884 (max Bucket 106), echt 0 (min Bucket -1) */
    { 0x30E1,  7,  17,   77 },   /* alt 41, falsch 6299 (max Bucket 77), echt 0 (min Bucket -1) */
    { 0x30E1,  7,  18,  101 },   /* alt 41, falsch 9426 (max Bucket 101), echt 0 (min Bucket -1) */
    { 0x30E1,  7,  19,   56 },   /* alt 41, falsch 4093 (max Bucket 56), echt 0 (min Bucket -1) */
    { 0x30E1,  7,  20,   77 },   /* alt 41, falsch 5770 (max Bucket 77), echt 0 (min Bucket -1) */
    { 0x30E1,  7,  21,   75 },   /* alt 41, falsch 4738 (max Bucket 75), echt 0 (min Bucket -1) */
    { 0x30E1,  7,  22,  167 },   /* alt 91, falsch 576465 (max Bucket 167), echt 0 (min Bucket -1) */
    { 0x30E1,  7,  23,  179 },   /* alt 91, falsch 67187 (max Bucket 179), echt 0 (min Bucket -1) */
    { 0x30E1,  7,  24,  135 },   /* alt 41, falsch 44435 (max Bucket 135), echt 0 (min Bucket -1) */
    { 0x30E1,  7,  25,  103 },   /* alt 93, falsch 16232 (max Bucket 103), echt 0 (min Bucket -1) */
    { 0x30E1,  7,  26,  101 },   /* alt 91, falsch 6774 (max Bucket 101), echt 0 (min Bucket -1) */
    { 0x30E1,  7,  27,  145 },   /* alt 41, falsch 3621 (max Bucket 145), echt 0 (min Bucket -1) */
    { 0x30E1,  7,  28,  146 },   /* alt 48, falsch 655 (max Bucket 146), echt 0 (min Bucket -1) */
    { 0x30E1,  7,  29,  148 },   /* alt 49, falsch 663 (max Bucket 148), echt 0 (min Bucket -1) */
    { 0x30E1,  7,  30,  167 },   /* alt 123, falsch 5076 (max Bucket 167), echt 0 (min Bucket -1) */
    { 0x30E1,  7,  31,  132 },   /* alt 41, falsch 1146 (max Bucket 132), echt 0 (min Bucket -1) */
    { 0x30E1,  7,  32,   92 },   /* alt 41, falsch 3011 (max Bucket 92), echt 0 (min Bucket -1) */
    { 0x30E1,  7,  33,  103 },   /* alt 41, falsch 1318 (max Bucket 103), echt 0 (min Bucket -1) */
    { 0x30E1,  7,  34,  105 },   /* alt 41, falsch 1338 (max Bucket 105), echt 0 (min Bucket -1) */
    { 0x30E1,  7,  35,   86 },   /* alt 41, falsch 3882 (max Bucket 86), echt 0 (min Bucket -1) */
    { 0x30E1,  7,  36,  103 },   /* alt 41, falsch 3116 (max Bucket 103), echt 0 (min Bucket -1) */
    { 0x30E1,  7,  37,  103 },   /* alt 41, falsch 492 (max Bucket 103), echt 0 (min Bucket -1) */
    { 0x30E1,  7,  38,  103 },   /* alt 48, falsch 495 (max Bucket 103), echt 0 (min Bucket -1) */
    { 0x30E1,  7,  39,  103 },   /* alt 49, falsch 2678 (max Bucket 103), echt 0 (min Bucket -1) */
    { 0x30E1,  7,  40,  103 },   /* alt 50, falsch 500 (max Bucket 103), echt 0 (min Bucket -1) */
    { 0x30E1,  7,  41,   88 },   /* alt 41, falsch 424 (max Bucket 88), echt 0 (min Bucket -1) */
    { 0x30E1,  7,  42,   82 },   /* alt 41, falsch 1376 (max Bucket 82), echt 0 (min Bucket -1) */
    { 0x30E1,  7,  43,   79 },   /* alt 41, falsch 1339 (max Bucket 79), echt 0 (min Bucket -1) */
    { 0x30E1,  7,  44,   76 },   /* alt 41, falsch 3444 (max Bucket 76), echt 0 (min Bucket -1) */
    { 0x30E1,  7,  45,   73 },   /* alt 41, falsch 1854 (max Bucket 73), echt 0 (min Bucket -1) */
    { 0x30E1,  7,  46,   70 },   /* alt 41, falsch 2333 (max Bucket 70), echt 0 (min Bucket -1) */
    { 0x30E1,  7,  47,   68 },   /* alt 41, falsch 2802 (max Bucket 68), echt 0 (min Bucket -1) */
    { 0x30E1,  7,  48,   65 },   /* alt 41, falsch 2951 (max Bucket 65), echt 0 (min Bucket -1) */
    { 0x30E1,  7,  49,   64 },   /* alt 54, falsch 1441 (max Bucket 64), echt 0 (min Bucket -1) */
    { 0x30E1,  7,  50,   55 },   /* alt 48, falsch 650 (max Bucket 55), echt 0 (min Bucket -1) */
    { 0x30E1,  7,  51,   55 },   /* alt 49, falsch 1850 (max Bucket 55), echt 0 (min Bucket -1) */
    { 0x30E1,  7,  52,   51 },   /* alt 41, falsch 2601 (max Bucket 51), echt 0 (min Bucket -1) */
    { 0x30E1,  8,   0,   91 },   /* alt 55, falsch 277373 (max Bucket 91), echt 0 (min Bucket -1) */
    { 0x30E1,  8,   1,   73 },   /* alt 57, falsch 21837 (max Bucket 73), echt 0 (min Bucket -1) */
    { 0x30E1,  8,   2,   61 },   /* alt 57, falsch 427 (max Bucket 61), echt 0 (min Bucket -1) */
    { 0x30E1,  8,   3,   73 },   /* alt 57, falsch 7986 (max Bucket 73), echt 0 (min Bucket -1) */
    { 0x30E1,  8,   4,   94 },   /* alt 55, falsch 13701 (max Bucket 94), echt 0 (min Bucket -1) */
    { 0x30E1,  8,   5,   73 },   /* alt 57, falsch 2471 (max Bucket 73), echt 0 (min Bucket -1) */
    { 0x30E1,  8,   7,   66 },   /* alt 57, falsch 834 (max Bucket 66), echt 0 (min Bucket -1) */
    { 0x30E1,  8,   8,   61 },   /* alt 57, falsch 156 (max Bucket 61), echt 0 (min Bucket -1) */
    { 0x30E1,  8,   9,   91 },   /* alt 56, falsch 4273 (max Bucket 91), echt 0 (min Bucket -1) */
    { 0x30E1,  8,  10,  124 },   /* alt 55, falsch 142067 (max Bucket 124), echt 0 (min Bucket -1) */
    { 0x30E1,  8,  11,   99 },   /* alt 55, falsch 28372 (max Bucket 99), echt 0 (min Bucket -1) */
    { 0x30E1,  8,  12,  128 },   /* alt 55, falsch 30240 (max Bucket 128), echt 0 (min Bucket -1) */
    { 0x30E1,  8,  13,   99 },   /* alt 55, falsch 9296 (max Bucket 99), echt 0 (min Bucket -1) */
    { 0x30E1,  8,  14,   83 },   /* alt 55, falsch 3701 (max Bucket 83), echt 0 (min Bucket -1) */
    { 0x30E1,  8,  15,  110 },   /* alt 55, falsch 5844 (max Bucket 110), echt 0 (min Bucket -1) */
    { 0x30E1,  8,  16,  121 },   /* alt 55, falsch 2945 (max Bucket 121), echt 0 (min Bucket -1) */
    { 0x30E1,  8,  17,  122 },   /* alt 56, falsch 352 (max Bucket 122), echt 0 (min Bucket -1) */
    { 0x30E1,  8,  18,  127 },   /* alt 84, falsch 1470 (max Bucket 127), echt 0 (min Bucket -1) */
    { 0x30E1,  8,  19,  110 },   /* alt 55, falsch 1143 (max Bucket 110), echt 0 (min Bucket -1) */
    { 0x30E1,  8,  20,  104 },   /* alt 55, falsch 2271 (max Bucket 104), echt 0 (min Bucket -1) */
    { 0x30E1,  8,  21,   99 },   /* alt 55, falsch 2420 (max Bucket 99), echt 0 (min Bucket -1) */
    { 0x30E1,  8,  22,   92 },   /* alt 55, falsch 3619 (max Bucket 92), echt 0 (min Bucket -1) */
    { 0x30E1,  8,  23,   83 },   /* alt 55, falsch 1422 (max Bucket 83), echt 0 (min Bucket -1) */
    { 0x30E1,  8,  24,   78 },   /* alt 55, falsch 456 (max Bucket 78), echt 0 (min Bucket -1) */
    { 0x30E1,  8,  25,   79 },   /* alt 55, falsch 844 (max Bucket 79), echt 0 (min Bucket -1) */
    { 0x30E1,  8,  26,   94 },   /* alt 55, falsch 438 (max Bucket 94), echt 0 (min Bucket -1) */
    { 0x30E1,  8,  27,   75 },   /* alt 55, falsch 285 (max Bucket 75), echt 0 (min Bucket -1) */
    { 0x30E1,  8,  28,   73 },   /* alt 57, falsch 1122 (max Bucket 73), echt 0 (min Bucket -1) */
    { 0x30E1,  8,  29,   69 },   /* alt 57, falsch 507 (max Bucket 69), echt 0 (min Bucket -1) */
    { 0x30E1,  8,  30,   66 },   /* alt 57, falsch 310 (max Bucket 66), echt 0 (min Bucket -1) */
    { 0x30E1,  8,  31,   64 },   /* alt 57, falsch 186 (max Bucket 64), echt 0 (min Bucket -1) */
    { 0x30E1,  8,  32,   61 },   /* alt 57, falsch 65 (max Bucket 61), echt 0 (min Bucket -1) */
    { 0x30E1,  8,  33,   58 },   /* alt 57, falsch 15 (max Bucket 58), echt 0 (min Bucket -1) */
    { 0x30E1,  9,   0,   89 },   /* alt 71, falsch 80665 (max Bucket 89), echt 0 (min Bucket -1) */
    { 0x30E1,  9,   1,   95 },   /* alt 75, falsch 100557 (max Bucket 95), echt 0 (min Bucket -1) */
    { 0x30E1,  9,   2,   81 },   /* alt 71, falsch 4793 (max Bucket 81), echt 0 (min Bucket -1) */
    { 0x30E1,  9,   3,   92 },   /* alt 72, falsch 30366 (max Bucket 92), echt 0 (min Bucket -1) */
    { 0x30E1,  9,   4,  102 },   /* alt 76, falsch 17864 (max Bucket 102), echt 0 (min Bucket -1) */
    { 0x30E1,  9,   5,   81 },   /* alt 72, falsch 5127 (max Bucket 81), echt 0 (min Bucket -1) */
    { 0x30E1,  9,   6,   93 },   /* alt 73, falsch 9738 (max Bucket 93), echt 0 (min Bucket -1) */
    { 0x30E1,  9,   7,   90 },   /* alt 71, falsch 2757 (max Bucket 90), echt 0 (min Bucket -1) */
    { 0x30E1,  9,   8,   84 },   /* alt 74, falsch 7018 (max Bucket 84), echt 0 (min Bucket -1) */
    { 0x30E1,  9,   9,  108 },   /* alt 72, falsch 187499 (max Bucket 108), echt 0 (min Bucket -1) */
    { 0x30E1,  9,  10,  109 },   /* alt 74, falsch 61121 (max Bucket 109), echt 0 (min Bucket -1) */
    { 0x30E1,  9,  11,   96 },   /* alt 71, falsch 15451 (max Bucket 96), echt 0 (min Bucket -1) */
    { 0x30E1,  9,  13,   91 },   /* alt 71, falsch 1198 (max Bucket 91), echt 0 (min Bucket -1) */
    { 0x30E1,  9,  14,  102 },   /* alt 71, falsch 6445 (max Bucket 102), echt 0 (min Bucket -1) */
    { 0x30E1,  9,  15,   96 },   /* alt 73, falsch 9572 (max Bucket 96), echt 0 (min Bucket -1) */
    { 0x30E1,  9,  16,  108 },   /* alt 75, falsch 11628 (max Bucket 108), echt 0 (min Bucket -1) */
    { 0x30E1,  9,  17,  108 },   /* alt 75, falsch 10857 (max Bucket 108), echt 0 (min Bucket -1) */
    { 0x30E1,  9,  18,  106 },   /* alt 74, falsch 7289 (max Bucket 106), echt 0 (min Bucket -1) */
    { 0x30E1,  9,  20,   92 },   /* alt 71, falsch 1826 (max Bucket 92), echt 0 (min Bucket -1) */
    { 0x30E1,  9,  21,   85 },   /* alt 71, falsch 156100 (max Bucket 85), echt 0 (min Bucket -1) */
    { 0x30E1,  9,  22,   88 },   /* alt 71, falsch 28972 (max Bucket 88), echt 0 (min Bucket -1) */
    { 0x30E1,  9,  23,   80 },   /* alt 71, falsch 15317 (max Bucket 80), echt 0 (min Bucket -1) */
    { 0x30E1,  9,  24,   73 },   /* alt 71, falsch 8896 (max Bucket 73), echt 0 (min Bucket -1) */
    { 0x30E1,  9,  25,   87 },   /* alt 71, falsch 8737 (max Bucket 87), echt 0 (min Bucket -1) */
    { 0x30E1,  9,  26,   84 },   /* alt 71, falsch 6343 (max Bucket 84), echt 0 (min Bucket -1) */
    { 0x30E1,  9,  27,   78 },   /* alt 71, falsch 4965 (max Bucket 78), echt 0 (min Bucket -1) */
    { 0x30E1,  9,  28,   85 },   /* alt 71, falsch 5417 (max Bucket 85), echt 0 (min Bucket -1) */
    { 0x30E1,  9,  29,   80 },   /* alt 71, falsch 3908 (max Bucket 80), echt 0 (min Bucket -1) */
    { 0x30E1,  9,  30,   79 },   /* alt 71, falsch 3818 (max Bucket 79), echt 0 (min Bucket -1) */
    { 0x30E1,  9,  31,  107 },   /* alt 75, falsch 1485 (max Bucket 107), echt 0 (min Bucket -1) */
    { 0x30E1,  9,  32,  105 },   /* alt 74, falsch 1166 (max Bucket 105), echt 0 (min Bucket -1) */
    { 0x30E1,  9,  33,  103 },   /* alt 73, falsch 3329 (max Bucket 103), echt 0 (min Bucket -1) */
    { 0x30E1,  9,  34,  106 },   /* alt 74, falsch 893 (max Bucket 106), echt 0 (min Bucket -1) */
    { 0x30E1,  9,  35,  102 },   /* alt 72, falsch 562 (max Bucket 102), echt 0 (min Bucket -1) */
    { 0x30E1,  9,  36,  101 },   /* alt 72, falsch 3180 (max Bucket 101), echt 0 (min Bucket -1) */
    { 0x30E1,  9,  37,  100 },   /* alt 71, falsch 3229 (max Bucket 100), echt 0 (min Bucket -1) */
    { 0x30E1,  9,  38,   97 },   /* alt 71, falsch 483 (max Bucket 97), echt 0 (min Bucket -1) */
    { 0x30E1,  9,  41,  103 },   /* alt 76, falsch 2157 (max Bucket 103), echt 0 (min Bucket -1) */
    { 0x30E1,  9,  42,  102 },   /* alt 75, falsch 608 (max Bucket 102), echt 0 (min Bucket -1) */
    { 0x30E1,  9,  43,  102 },   /* alt 75, falsch 4171 (max Bucket 102), echt 0 (min Bucket -1) */
    { 0x30E1,  9,  44,  100 },   /* alt 75, falsch 5769 (max Bucket 100), echt 0 (min Bucket -1) */
    { 0x30E1,  9,  45,   96 },   /* alt 74, falsch 846 (max Bucket 96), echt 0 (min Bucket -1) */
    { 0x30E1,  9,  46,   96 },   /* alt 74, falsch 3842 (max Bucket 96), echt 0 (min Bucket -1) */
    { 0x30E1,  9,  48,   95 },   /* alt 74, falsch 2403 (max Bucket 95), echt 0 (min Bucket -1) */
    { 0x30E1,  9,  49,   94 },   /* alt 73, falsch 1552 (max Bucket 94), echt 0 (min Bucket -1) */
    { 0x30E1,  9,  50,   94 },   /* alt 73, falsch 768 (max Bucket 94), echt 0 (min Bucket -1) */
    { 0x30E1,  9,  51,   94 },   /* alt 72, falsch 2402 (max Bucket 94), echt 0 (min Bucket -1) */
    { 0x30E1,  9,  52,   93 },   /* alt 72, falsch 2555 (max Bucket 93), echt 0 (min Bucket -1) */
    { 0x30E1,  9,  53,   82 },   /* alt 71, falsch 2937 (max Bucket 82), echt 0 (min Bucket -1) */
    { 0x30E1,  9,  54,   92 },   /* alt 71, falsch 439 (max Bucket 92), echt 0 (min Bucket -1) */
    { 0x30E1,  9,  55,   92 },   /* alt 71, falsch 827 (max Bucket 92), echt 0 (min Bucket -1) */
    { 0x30E1,  9,  56,   91 },   /* alt 71, falsch 1485 (max Bucket 91), echt 0 (min Bucket -1) */
    { 0x30E1,  9,  57,   91 },   /* alt 71, falsch 561 (max Bucket 91), echt 0 (min Bucket -1) */
    { 0x30E1,  9,  58,   88 },   /* alt 71, falsch 1529 (max Bucket 88), echt 0 (min Bucket -1) */
    { 0x30E1,  9,  62,   88 },   /* alt 71, falsch 3536 (max Bucket 88), echt 0 (min Bucket -1) */
    { 0x30E1,  9,  63,   76 },   /* alt 71, falsch 942 (max Bucket 76), echt 0 (min Bucket -1) */
    { 0x30E1,  9,  65,   73 },   /* alt 71, falsch 920 (max Bucket 73), echt 0 (min Bucket -1) */
    { 0x30E1,  9,  66,   86 },   /* alt 71, falsch 3950 (max Bucket 86), echt 0 (min Bucket -1) */
    { 0x30E1,  9,  67,   72 },   /* alt 71, falsch 3821 (max Bucket 72), echt 0 (min Bucket -1) */
    { 0x30E1,  9,  68,   84 },   /* alt 76, falsch 987 (max Bucket 84), echt 0 (min Bucket -1) */
    { 0x30E1,  9,  69,   84 },   /* alt 75, falsch 2148 (max Bucket 84), echt 0 (min Bucket -1) */
    { 0x30E1,  9,  70,   82 },   /* alt 74, falsch 2444 (max Bucket 82), echt 0 (min Bucket -1) */
    { 0x30E1,  9,  71,   82 },   /* alt 71, falsch 1641 (max Bucket 82), echt 0 (min Bucket -1) */
    { 0x30E1,  9,  73,   81 },   /* alt 73, falsch 2197 (max Bucket 81), echt 0 (min Bucket -1) */
    { 0x30E1,  9,  74,   81 },   /* alt 72, falsch 1618 (max Bucket 81), echt 0 (min Bucket -1) */
    { 0x30E1,  9,  75,   80 },   /* alt 71, falsch 967 (max Bucket 80), echt 0 (min Bucket -1) */
    { 0x30E1,  9,  76,   79 },   /* alt 71, falsch 1189 (max Bucket 79), echt 0 (min Bucket -1) */
    { 0x30E1,  9,  77,   79 },   /* alt 72, falsch 615 (max Bucket 79), echt 0 (min Bucket -1) */
    { 0x30E1,  9,  78,   79 },   /* alt 71, falsch 984 (max Bucket 79), echt 0 (min Bucket -1) */
    { 0x30E1,  9,  79,   78 },   /* alt 71, falsch 1169 (max Bucket 78), echt 0 (min Bucket -1) */
    { 0x4040,  4,   0,  154 },   /* alt 142, falsch 972924 (max Bucket 154), echt 103353 (min Bucket 203) */
    { 0x4040,  4,   1,  172 },   /* alt 156, falsch 225405 (max Bucket 172), echt 120303 (min Bucket 186) */
    { 0x4040,  4,   2,  150 },   /* alt 142, falsch 19272 (max Bucket 150), echt 0 (min Bucket -1) */
    { 0x4040,  4,   4,  165 },   /* alt 152, falsch 10820 (max Bucket 165), echt 0 (min Bucket -1) */
    { 0x4040,  4,   5,  165 },   /* alt 142, falsch 414665 (max Bucket 165), echt 147398 (min Bucket 183) */
    { 0x4040,  4,   7,  171 },   /* alt 163, falsch 599 (max Bucket 171), echt 0 (min Bucket -1) */
    { 0x4040,  4,   8,  169 },   /* alt 160, falsch 1164 (max Bucket 169), echt 0 (min Bucket -1) */
    { 0x4040,  4,   9,  167 },   /* alt 157, falsch 1737 (max Bucket 167), echt 0 (min Bucket -1) */
    { 0x4040,  4,  10,  158 },   /* alt 152, falsch 535 (max Bucket 158), echt 0 (min Bucket -1) */
    { 0x4040,  4,  11,  156 },   /* alt 149, falsch 1066 (max Bucket 156), echt 0 (min Bucket -1) */
    { 0x4040,  4,  12,  154 },   /* alt 147, falsch 1565 (max Bucket 154), echt 0 (min Bucket -1) */
    { 0x4040,  5,   9,  358 },   /* alt 243, falsch 11630 (max Bucket 358), echt 9136 (min Bucket 290), echt teils nicht mehr verdeckt */
    { 0x4040,  5,  14,   91 },   /* alt 82, falsch 736067 (max Bucket 91), echt 189822 (min Bucket 107) */
    { 0x4040,  5,  15,   93 },   /* alt 87, falsch 17401 (max Bucket 93), echt 9714 (min Bucket 106) */
    { 0x4040,  5,  16,   88 },   /* alt 82, falsch 711104 (max Bucket 88), echt 3662 (min Bucket 110) */
    { 0x4040,  5,  17,   94 },   /* alt 86, falsch 70198 (max Bucket 94), echt 2804 (min Bucket 107) */
    { 0x4040,  5,  18,   90 },   /* alt 84, falsch 6417 (max Bucket 90), echt 0 (min Bucket -1) */
    { 0x4040,  5,  19,   88 },   /* alt 82, falsch 24404 (max Bucket 88), echt 98129 (min Bucket 104) */
    { 0x4040,  5,  20,   90 },   /* alt 87, falsch 1407 (max Bucket 90), echt 4814 (min Bucket 103) */
    { 0x4040,  5,  21,   88 },   /* alt 82, falsch 185530 (max Bucket 88), echt 167269 (min Bucket 106) */
    { 0x4040,  5,  22,   90 },   /* alt 86, falsch 18877 (max Bucket 90), echt 19238 (min Bucket 106) */
    { 0x4040,  5,  27,    5 },   /* alt 0, falsch 228 (max Bucket 5), echt 0 (min Bucket -1) */
    { 0x4040,  5,  29,    5 },   /* alt 0, falsch 228 (max Bucket 5), echt 0 (min Bucket -1) */
    { 0x4040,  5,  34,   86 },   /* alt 84, falsch 583 (max Bucket 86), echt 577 (min Bucket 105) */
    { 0x4040,  5,  35,   88 },   /* alt 85, falsch 401 (max Bucket 88), echt 288 (min Bucket 105) */
    { 0x4040,  5,  36,   88 },   /* alt 86, falsch 591 (max Bucket 88), echt 592 (min Bucket 105) */
    { 0x4040,  5,  37,   87 },   /* alt 84, falsch 2001 (max Bucket 87), echt 1986 (min Bucket 106) */
    { 0x4040,  5,  39,   91 },   /* alt 87, falsch 2241 (max Bucket 91), echt 2192 (min Bucket 105) */
    { 0x4040,  5,  40,   91 },   /* alt 88, falsch 1086 (max Bucket 91), echt 934 (min Bucket 105) */
    { 0x4040,  5,  43,   92 },   /* alt 87, falsch 1442 (max Bucket 92), echt 1228 (min Bucket 105) */
    { 0x4040,  5,  44,   92 },   /* alt 88, falsch 1142 (max Bucket 92), echt 928 (min Bucket 105) */
    { 0x4040,  5,  47,   92 },   /* alt 87, falsch 1174 (max Bucket 92), echt 950 (min Bucket 106) */
    { 0x4040,  5,  48,   92 },   /* alt 88, falsch 1171 (max Bucket 92), echt 939 (min Bucket 106) */
    { 0x4040,  5,  50,   92 },   /* alt 87, falsch 546 (max Bucket 92), echt 305 (min Bucket 106) */
    { 0x4040,  5,  55,   92 },   /* alt 87, falsch 1277 (max Bucket 92), echt 0 (min Bucket -1) */
    { 0x4040,  5,  56,   88 },   /* alt 83, falsch 1587 (max Bucket 88), echt 0 (min Bucket -1) */
    { 0x4040,  5,  57,   88 },   /* alt 84, falsch 1924 (max Bucket 88), echt 0 (min Bucket -1) */
    { 0x4040,  5,  58,   90 },   /* alt 85, falsch 578 (max Bucket 90), echt 0 (min Bucket -1) */
    { 0x4040,  6,   4,  187 },   /* alt 173, falsch 1025389 (max Bucket 187), echt 431514 (min Bucket 206) */
    { 0x4040,  6,   5,  196 },   /* alt 182, falsch 80537 (max Bucket 196), echt 37518 (min Bucket 206) */
    { 0x4040,  6,   6,  181 },   /* alt 173, falsch 30013 (max Bucket 181), echt 0 (min Bucket -1) */
    { 0x4040,  6,   7,  200 },   /* alt 185, falsch 17350 (max Bucket 200), echt 4756 (min Bucket 211) */
    { 0x4040,  6,   8,  372 },   /* alt 365, falsch 530592 (max Bucket 372), echt 233239 (min Bucket 205), echt teils nicht mehr verdeckt */
    { 0x4040,  6,  26,  191 },   /* alt 185, falsch 776 (max Bucket 191), echt 2047 (min Bucket 206) */
    { 0x4040,  6,  27,  192 },   /* alt 185, falsch 311 (max Bucket 192), echt 299 (min Bucket 207) */
    { 0x4040,  6,  28,  194 },   /* alt 185, falsch 4489 (max Bucket 194), echt 2995 (min Bucket 207) */
    { 0x4040,  6,  29,  194 },   /* alt 185, falsch 2217 (max Bucket 194), echt 1231 (min Bucket 209) */
    { 0x4040,  6,  30,  195 },   /* alt 185, falsch 1172 (max Bucket 195), echt 605 (min Bucket 210) */
    { 0x4040,  6,  32,  189 },   /* alt 179, falsch 699 (max Bucket 189), echt 0 (min Bucket -1) */
    { 0x4040,  7,   2,  234 },   /* alt 223, falsch 891111 (max Bucket 234), echt 262935 (min Bucket 130), echt teils nicht mehr verdeckt */
    { 0x4040,  7,   3,  257 },   /* alt 235, falsch 102621 (max Bucket 257), echt 42157 (min Bucket 271) */
    { 0x4040,  7,   4,  261 },   /* alt 250, falsch 20627 (max Bucket 261), echt 5945 (min Bucket 274) */
    { 0x4040,  7,  11,  251 },   /* alt 238, falsch 955 (max Bucket 251), echt 0 (min Bucket -1) */
    { 0x4040,  7,  12,  245 },   /* alt 233, falsch 1847 (max Bucket 245), echt 0 (min Bucket -1) */
    { 0x4040,  7,  13,  241 },   /* alt 228, falsch 2700 (max Bucket 241), echt 0 (min Bucket -1) */
    { 0x4040,  9,   1,  165 },   /* alt 147, falsch 19304 (max Bucket 165), echt 10802 (min Bucket 195) */
    { 0x4040,  9,   2,  152 },   /* alt 147, falsch 2165 (max Bucket 152), echt 6763 (min Bucket 188) */
    { 0x4040,  9,   3,  176 },   /* alt 157, falsch 857799 (max Bucket 176), echt 40458 (min Bucket 196) */
    { 0x4040,  9,   4,  188 },   /* alt 163, falsch 78477 (max Bucket 188), echt 19054 (min Bucket 200) */
    { 0x4040,  9,   5,  178 },   /* alt 160, falsch 7773 (max Bucket 178), echt 7676 (min Bucket 195) */
    { 0x4040,  9,   6,  191 },   /* alt 174, falsch 14685 (max Bucket 191), echt 3073 (min Bucket 203) */
    { 0x4040,  9,   7,  180 },   /* alt 164, falsch 1222 (max Bucket 180), echt 2276 (min Bucket 195) */
    { 0x4040,  9,  29,  184 },   /* alt 169, falsch 906 (max Bucket 184), echt 699 (min Bucket 197) */
    { 0x4040,  9,  30,  182 },   /* alt 164, falsch 1665 (max Bucket 182), echt 863 (min Bucket 198) */
    { 0x4040,  9,  31,  184 },   /* alt 169, falsch 1367 (max Bucket 184), echt 686 (min Bucket 198) */
    { 0x4040,  9,  32,  157 },   /* alt 147, falsch 1401 (max Bucket 157), echt 1685 (min Bucket 193) */
    { 0x4040,  9,  33,  189 },   /* alt 174, falsch 1888 (max Bucket 189), echt 723 (min Bucket 201) */
    { 0x4040,  9,  34,  167 },   /* alt 157, falsch 760 (max Bucket 167), echt 518 (min Bucket 194) */
    { 0x4040,  9,  35,  177 },   /* alt 169, falsch 1083 (max Bucket 177), echt 824 (min Bucket 195) */
    { 0x4040,  9,  36,  173 },   /* alt 169, falsch 4046 (max Bucket 173), echt 769 (min Bucket 198) */
    { 0x4040,  9,  37,  179 },   /* alt 159, falsch 2842 (max Bucket 179), echt 0 (min Bucket -1) */
    { 0x4040,  9,  38,  181 },   /* alt 162, falsch 2244 (max Bucket 181), echt 0 (min Bucket -1) */
    { 0x4040,  9,  39,  183 },   /* alt 164, falsch 1668 (max Bucket 183), echt 0 (min Bucket -1) */
    { 0x4040, 10,   0,  233 },   /* alt 216, falsch 756279 (max Bucket 233), echt 7175 (min Bucket 279) */
    { 0x4040, 10,   1,  257 },   /* alt 233, falsch 97866 (max Bucket 257), echt 18227 (min Bucket 276) */
    { 0x4040, 10,   4,  260 },   /* alt 246, falsch 5765 (max Bucket 260), echt 1315 (min Bucket 276) */
    { 0x4040, 10,  11,  238 },   /* alt 216, falsch 1034044 (max Bucket 238), echt 7046 (min Bucket 282) */
    { 0x4040, 10,  12,  268 },   /* alt 233, falsch 122280 (max Bucket 268), echt 18547 (min Bucket 278) */
    { 0x4040, 10,  22,  237 },   /* alt 221, falsch 1196 (max Bucket 237), echt 156 (min Bucket 281) */
    { 0x4040, 10,  23,  243 },   /* alt 227, falsch 1220 (max Bucket 243), echt 185 (min Bucket 281) */
    { 0x4040, 10,  24,  252 },   /* alt 233, falsch 2359 (max Bucket 252), echt 454 (min Bucket 279) */
    { 0x4040, 10,  25,  258 },   /* alt 239, falsch 1296 (max Bucket 258), echt 255 (min Bucket 279) */
    { 0x4040, 10,  26,  242 },   /* alt 221, falsch 4479 (max Bucket 242), echt 0 (min Bucket -1) */
    { 0x4040, 10,  27,  248 },   /* alt 227, falsch 3467 (max Bucket 248), echt 0 (min Bucket -1) */
    { 0x4040, 10,  28,  258 },   /* alt 233, falsch 4706 (max Bucket 258), echt 0 (min Bucket -1) */
    { 0x4040, 10,  29,  262 },   /* alt 239, falsch 1319 (max Bucket 262), echt 0 (min Bucket -1) */
    { 0x4040, 11,  13,  153 },   /* alt 148, falsch 753179 (max Bucket 153), echt 8485 (min Bucket 287) */
    { 0x4040, 11,  14,  153 },   /* alt 148, falsch 20418 (max Bucket 153), echt 0 (min Bucket -1) */
    { 0x4040, 11,  15,  145 },   /* alt 144, falsch 609359 (max Bucket 145), echt 8674 (min Bucket 274) */
    { 0x4040, 11,  16,  145 },   /* alt 144, falsch 38763 (max Bucket 145), echt 395 (min Bucket 165) */
    { 0x4040, 11,  26,   16 },   /* alt 0, falsch 501 (max Bucket 16), echt 0 (min Bucket -1) */
    { 0x4040, 11,  27,   17 },   /* alt 0, falsch 905 (max Bucket 17), echt 0 (min Bucket -1) */
    { 0x4040, 11,  32,   17 },   /* alt 0, falsch 443 (max Bucket 17), echt 0 (min Bucket -1) */
    { 0x4040, 11,  33,   17 },   /* alt 0, falsch 674 (max Bucket 17), echt 0 (min Bucket -1) */
    { 0x4040, 11,  34,   17 },   /* alt 0, falsch 425 (max Bucket 17), echt 0 (min Bucket -1) */
    { 0x4040, 11,  36,   24 },   /* alt 0, falsch 600 (max Bucket 24), echt 0 (min Bucket -1) */
    { 0x4040, 11,  37,   24 },   /* alt 0, falsch 1893 (max Bucket 24), echt 0 (min Bucket -1) */
    { 0x4040, 11,  38,   24 },   /* alt 0, falsch 1191 (max Bucket 24), echt 0 (min Bucket -1) */
    { 0x4040, 11,  41,   38 },   /* alt 0, falsch 853 (max Bucket 38), echt 0 (min Bucket -1) */
    { 0x4040, 11,  42,   44 },   /* alt 0, falsch 1019 (max Bucket 44), echt 0 (min Bucket -1) */
    { 0x4040, 11,  93,  145 },   /* alt 144, falsch 8721 (max Bucket 145), echt 65 (min Bucket 168) */
    { 0x4040, 12,  71,  155 },   /* alt 154, falsch 324 (max Bucket 155), echt 0 (min Bucket -1) */
    { 0x4040, 12,  72,  157 },   /* alt 153, falsch 1066 (max Bucket 157), echt 0 (min Bucket -1) */
    { 0x4041,  4,   0,  154 },   /* alt 142, falsch 972924 (max Bucket 154), echt 103353 (min Bucket 203) */
    { 0x4041,  4,   1,  172 },   /* alt 156, falsch 225405 (max Bucket 172), echt 120303 (min Bucket 186) */
    { 0x4041,  4,   2,  150 },   /* alt 142, falsch 19272 (max Bucket 150), echt 0 (min Bucket -1) */
    { 0x4041,  4,   4,  165 },   /* alt 152, falsch 10820 (max Bucket 165), echt 0 (min Bucket -1) */
    { 0x4041,  4,   5,  165 },   /* alt 142, falsch 414665 (max Bucket 165), echt 147398 (min Bucket 183) */
    { 0x4041,  4,   7,  171 },   /* alt 163, falsch 599 (max Bucket 171), echt 0 (min Bucket -1) */
    { 0x4041,  4,   8,  169 },   /* alt 160, falsch 1164 (max Bucket 169), echt 0 (min Bucket -1) */
    { 0x4041,  4,   9,  167 },   /* alt 157, falsch 1737 (max Bucket 167), echt 0 (min Bucket -1) */
    { 0x4041,  4,  10,  158 },   /* alt 152, falsch 535 (max Bucket 158), echt 0 (min Bucket -1) */
    { 0x4041,  4,  11,  156 },   /* alt 149, falsch 1066 (max Bucket 156), echt 0 (min Bucket -1) */
    { 0x4041,  4,  12,  154 },   /* alt 147, falsch 1565 (max Bucket 154), echt 0 (min Bucket -1) */
    { 0x4041,  5,   9,  358 },   /* alt 243, falsch 11630 (max Bucket 358), echt 9136 (min Bucket 290), echt teils nicht mehr verdeckt */
    { 0x4041,  5,  14,   91 },   /* alt 82, falsch 736067 (max Bucket 91), echt 189822 (min Bucket 107) */
    { 0x4041,  5,  15,   93 },   /* alt 87, falsch 17401 (max Bucket 93), echt 9714 (min Bucket 106) */
    { 0x4041,  5,  16,   88 },   /* alt 82, falsch 711104 (max Bucket 88), echt 3662 (min Bucket 110) */
    { 0x4041,  5,  17,   94 },   /* alt 86, falsch 70198 (max Bucket 94), echt 2804 (min Bucket 107) */
    { 0x4041,  5,  18,   90 },   /* alt 84, falsch 6417 (max Bucket 90), echt 0 (min Bucket -1) */
    { 0x4041,  5,  19,   88 },   /* alt 82, falsch 24404 (max Bucket 88), echt 98129 (min Bucket 104) */
    { 0x4041,  5,  20,   90 },   /* alt 87, falsch 1407 (max Bucket 90), echt 4814 (min Bucket 103) */
    { 0x4041,  5,  21,   88 },   /* alt 82, falsch 185530 (max Bucket 88), echt 167269 (min Bucket 106) */
    { 0x4041,  5,  22,   90 },   /* alt 86, falsch 18877 (max Bucket 90), echt 19238 (min Bucket 106) */
    { 0x4041,  5,  27,    5 },   /* alt 0, falsch 228 (max Bucket 5), echt 0 (min Bucket -1) */
    { 0x4041,  5,  29,    5 },   /* alt 0, falsch 228 (max Bucket 5), echt 0 (min Bucket -1) */
    { 0x4041,  5,  34,   86 },   /* alt 84, falsch 583 (max Bucket 86), echt 577 (min Bucket 105) */
    { 0x4041,  5,  35,   88 },   /* alt 85, falsch 401 (max Bucket 88), echt 288 (min Bucket 105) */
    { 0x4041,  5,  36,   88 },   /* alt 86, falsch 591 (max Bucket 88), echt 592 (min Bucket 105) */
    { 0x4041,  5,  37,   87 },   /* alt 84, falsch 2001 (max Bucket 87), echt 1986 (min Bucket 106) */
    { 0x4041,  5,  39,   91 },   /* alt 87, falsch 2241 (max Bucket 91), echt 2192 (min Bucket 105) */
    { 0x4041,  5,  40,   91 },   /* alt 88, falsch 1086 (max Bucket 91), echt 934 (min Bucket 105) */
    { 0x4041,  5,  43,   92 },   /* alt 87, falsch 1442 (max Bucket 92), echt 1228 (min Bucket 105) */
    { 0x4041,  5,  44,   92 },   /* alt 88, falsch 1142 (max Bucket 92), echt 928 (min Bucket 105) */
    { 0x4041,  5,  47,   92 },   /* alt 87, falsch 1174 (max Bucket 92), echt 950 (min Bucket 106) */
    { 0x4041,  5,  48,   92 },   /* alt 88, falsch 1171 (max Bucket 92), echt 939 (min Bucket 106) */
    { 0x4041,  5,  50,   92 },   /* alt 87, falsch 546 (max Bucket 92), echt 305 (min Bucket 106) */
    { 0x4041,  5,  55,   92 },   /* alt 87, falsch 1277 (max Bucket 92), echt 0 (min Bucket -1) */
    { 0x4041,  5,  56,   88 },   /* alt 83, falsch 1587 (max Bucket 88), echt 0 (min Bucket -1) */
    { 0x4041,  5,  57,   88 },   /* alt 84, falsch 1924 (max Bucket 88), echt 0 (min Bucket -1) */
    { 0x4041,  5,  58,   90 },   /* alt 85, falsch 578 (max Bucket 90), echt 0 (min Bucket -1) */
    { 0x4041,  6,   4,  187 },   /* alt 173, falsch 1025389 (max Bucket 187), echt 431514 (min Bucket 206) */
    { 0x4041,  6,   5,  196 },   /* alt 182, falsch 80537 (max Bucket 196), echt 37518 (min Bucket 206) */
    { 0x4041,  6,   6,  181 },   /* alt 173, falsch 30013 (max Bucket 181), echt 0 (min Bucket -1) */
    { 0x4041,  6,   7,  200 },   /* alt 185, falsch 17350 (max Bucket 200), echt 4756 (min Bucket 211) */
    { 0x4041,  6,   8,  372 },   /* alt 365, falsch 530592 (max Bucket 372), echt 233239 (min Bucket 205), echt teils nicht mehr verdeckt */
    { 0x4041,  6,  26,  191 },   /* alt 185, falsch 776 (max Bucket 191), echt 2047 (min Bucket 206) */
    { 0x4041,  6,  27,  192 },   /* alt 185, falsch 311 (max Bucket 192), echt 299 (min Bucket 207) */
    { 0x4041,  6,  28,  194 },   /* alt 185, falsch 4489 (max Bucket 194), echt 2995 (min Bucket 207) */
    { 0x4041,  6,  29,  194 },   /* alt 185, falsch 2217 (max Bucket 194), echt 1231 (min Bucket 209) */
    { 0x4041,  6,  30,  195 },   /* alt 185, falsch 1172 (max Bucket 195), echt 605 (min Bucket 210) */
    { 0x4041,  6,  32,  189 },   /* alt 179, falsch 699 (max Bucket 189), echt 0 (min Bucket -1) */
    { 0x4041,  7,   2,  234 },   /* alt 223, falsch 891111 (max Bucket 234), echt 262935 (min Bucket 130), echt teils nicht mehr verdeckt */
    { 0x4041,  7,   3,  257 },   /* alt 235, falsch 102621 (max Bucket 257), echt 42157 (min Bucket 271) */
    { 0x4041,  7,   4,  261 },   /* alt 250, falsch 20627 (max Bucket 261), echt 5945 (min Bucket 274) */
    { 0x4041,  7,  11,  251 },   /* alt 238, falsch 955 (max Bucket 251), echt 0 (min Bucket -1) */
    { 0x4041,  7,  12,  245 },   /* alt 233, falsch 1847 (max Bucket 245), echt 0 (min Bucket -1) */
    { 0x4041,  7,  13,  241 },   /* alt 228, falsch 2700 (max Bucket 241), echt 0 (min Bucket -1) */
    { 0x4041,  9,   1,  165 },   /* alt 147, falsch 19304 (max Bucket 165), echt 10802 (min Bucket 195) */
    { 0x4041,  9,   2,  152 },   /* alt 147, falsch 2165 (max Bucket 152), echt 6763 (min Bucket 188) */
    { 0x4041,  9,   3,  176 },   /* alt 157, falsch 857799 (max Bucket 176), echt 40458 (min Bucket 196) */
    { 0x4041,  9,   4,  188 },   /* alt 163, falsch 78477 (max Bucket 188), echt 19054 (min Bucket 200) */
    { 0x4041,  9,   5,  178 },   /* alt 160, falsch 7773 (max Bucket 178), echt 7676 (min Bucket 195) */
    { 0x4041,  9,   6,  191 },   /* alt 174, falsch 14685 (max Bucket 191), echt 3073 (min Bucket 203) */
    { 0x4041,  9,   7,  180 },   /* alt 164, falsch 1222 (max Bucket 180), echt 2276 (min Bucket 195) */
    { 0x4041,  9,  29,  184 },   /* alt 169, falsch 906 (max Bucket 184), echt 699 (min Bucket 197) */
    { 0x4041,  9,  30,  182 },   /* alt 164, falsch 1665 (max Bucket 182), echt 863 (min Bucket 198) */
    { 0x4041,  9,  31,  184 },   /* alt 169, falsch 1367 (max Bucket 184), echt 686 (min Bucket 198) */
    { 0x4041,  9,  32,  157 },   /* alt 147, falsch 1401 (max Bucket 157), echt 1685 (min Bucket 193) */
    { 0x4041,  9,  33,  189 },   /* alt 174, falsch 1888 (max Bucket 189), echt 723 (min Bucket 201) */
    { 0x4041,  9,  34,  167 },   /* alt 157, falsch 760 (max Bucket 167), echt 518 (min Bucket 194) */
    { 0x4041,  9,  35,  177 },   /* alt 169, falsch 1083 (max Bucket 177), echt 824 (min Bucket 195) */
    { 0x4041,  9,  36,  173 },   /* alt 169, falsch 4046 (max Bucket 173), echt 769 (min Bucket 198) */
    { 0x4041,  9,  37,  179 },   /* alt 159, falsch 2842 (max Bucket 179), echt 0 (min Bucket -1) */
    { 0x4041,  9,  38,  181 },   /* alt 162, falsch 2244 (max Bucket 181), echt 0 (min Bucket -1) */
    { 0x4041,  9,  39,  183 },   /* alt 164, falsch 1668 (max Bucket 183), echt 0 (min Bucket -1) */
    { 0x4041, 10,   0,  233 },   /* alt 216, falsch 756279 (max Bucket 233), echt 7175 (min Bucket 279) */
    { 0x4041, 10,   1,  257 },   /* alt 233, falsch 97866 (max Bucket 257), echt 18227 (min Bucket 276) */
    { 0x4041, 10,   4,  260 },   /* alt 246, falsch 5765 (max Bucket 260), echt 1315 (min Bucket 276) */
    { 0x4041, 10,  11,  238 },   /* alt 216, falsch 1034044 (max Bucket 238), echt 7046 (min Bucket 282) */
    { 0x4041, 10,  12,  268 },   /* alt 233, falsch 122280 (max Bucket 268), echt 18547 (min Bucket 278) */
    { 0x4041, 10,  22,  237 },   /* alt 221, falsch 1196 (max Bucket 237), echt 156 (min Bucket 281) */
    { 0x4041, 10,  23,  243 },   /* alt 227, falsch 1220 (max Bucket 243), echt 185 (min Bucket 281) */
    { 0x4041, 10,  24,  252 },   /* alt 233, falsch 2359 (max Bucket 252), echt 454 (min Bucket 279) */
    { 0x4041, 10,  25,  258 },   /* alt 239, falsch 1296 (max Bucket 258), echt 255 (min Bucket 279) */
    { 0x4041, 10,  26,  242 },   /* alt 221, falsch 4479 (max Bucket 242), echt 0 (min Bucket -1) */
    { 0x4041, 10,  27,  248 },   /* alt 227, falsch 3467 (max Bucket 248), echt 0 (min Bucket -1) */
    { 0x4041, 10,  28,  258 },   /* alt 233, falsch 4706 (max Bucket 258), echt 0 (min Bucket -1) */
    { 0x4041, 10,  29,  262 },   /* alt 239, falsch 1319 (max Bucket 262), echt 0 (min Bucket -1) */
    { 0x4041, 11,  13,  153 },   /* alt 148, falsch 753179 (max Bucket 153), echt 8485 (min Bucket 287) */
    { 0x4041, 11,  14,  153 },   /* alt 148, falsch 20418 (max Bucket 153), echt 0 (min Bucket -1) */
    { 0x4041, 11,  15,  145 },   /* alt 144, falsch 609359 (max Bucket 145), echt 8674 (min Bucket 274) */
    { 0x4041, 11,  16,  145 },   /* alt 144, falsch 38763 (max Bucket 145), echt 395 (min Bucket 165) */
    { 0x4041, 11,  26,   16 },   /* alt 0, falsch 501 (max Bucket 16), echt 0 (min Bucket -1) */
    { 0x4041, 11,  27,   17 },   /* alt 0, falsch 905 (max Bucket 17), echt 0 (min Bucket -1) */
    { 0x4041, 11,  32,   17 },   /* alt 0, falsch 443 (max Bucket 17), echt 0 (min Bucket -1) */
    { 0x4041, 11,  33,   17 },   /* alt 0, falsch 674 (max Bucket 17), echt 0 (min Bucket -1) */
    { 0x4041, 11,  34,   17 },   /* alt 0, falsch 425 (max Bucket 17), echt 0 (min Bucket -1) */
    { 0x4041, 11,  36,   24 },   /* alt 0, falsch 600 (max Bucket 24), echt 0 (min Bucket -1) */
    { 0x4041, 11,  37,   24 },   /* alt 0, falsch 1893 (max Bucket 24), echt 0 (min Bucket -1) */
    { 0x4041, 11,  38,   24 },   /* alt 0, falsch 1191 (max Bucket 24), echt 0 (min Bucket -1) */
    { 0x4041, 11,  41,   38 },   /* alt 0, falsch 853 (max Bucket 38), echt 0 (min Bucket -1) */
    { 0x4041, 11,  42,   44 },   /* alt 0, falsch 1019 (max Bucket 44), echt 0 (min Bucket -1) */
    { 0x4041, 11,  93,  145 },   /* alt 144, falsch 8721 (max Bucket 145), echt 65 (min Bucket 168) */
    { 0x4041, 12,  71,  155 },   /* alt 154, falsch 324 (max Bucket 155), echt 0 (min Bucket -1) */
    { 0x4041, 12,  72,  157 },   /* alt 153, falsch 1066 (max Bucket 157), echt 0 (min Bucket -1) */
    { 0x4080,  1,   3,  174 },   /* alt 157, falsch 1731952 (max Bucket 174), echt 151184 (min Bucket 195) */
    { 0x4080,  1,   5,  171 },   /* alt 162, falsch 12627 (max Bucket 171), echt 0 (min Bucket -1) */
    { 0x4080,  1,   6,   11 },   /* alt 0, falsch 46850 (max Bucket 11), echt 0 (min Bucket -1) */
    { 0x4080,  1,  12,  173 },   /* alt 167, falsch 1603 (max Bucket 173), echt 0 (min Bucket -1) */
    { 0x4080,  1,  13,  173 },   /* alt 171, falsch 864 (max Bucket 173), echt 0 (min Bucket -1) */
    { 0x4080,  1,  14,  163 },   /* alt 160, falsch 2158 (max Bucket 163), echt 0 (min Bucket -1) */
    { 0x4080,  2,   0,  137 },   /* alt 79, falsch 1772804 (max Bucket 137), echt 1423282 (min Bucket 89), echt teils nicht mehr verdeckt */
    { 0x4080,  2,   1,  147 },   /* alt 80, falsch 125699 (max Bucket 147), echt 46361 (min Bucket 95), echt teils nicht mehr verdeckt */
    { 0x4080,  2,   2,  128 },   /* alt 83, falsch 35382 (max Bucket 128), echt 3842 (min Bucket 99), echt teils nicht mehr verdeckt */
    { 0x4080,  2,   3,  142 },   /* alt 79, falsch 29394 (max Bucket 142), echt 21478 (min Bucket 91), echt teils nicht mehr verdeckt */
    { 0x4080,  2,   4,  114 },   /* alt 85, falsch 8396 (max Bucket 114), echt 145 (min Bucket 101), echt teils nicht mehr verdeckt */
    { 0x4080,  2,   5,  139 },   /* alt 79, falsch 5936 (max Bucket 139), echt 2428 (min Bucket 90), echt teils nicht mehr verdeckt */
    { 0x4080,  2,   6,  146 },   /* alt 81, falsch 8213 (max Bucket 146), echt 3692 (min Bucket 94), echt teils nicht mehr verdeckt */
    { 0x4080,  2,   7,  149 },   /* alt 83, falsch 7920 (max Bucket 149), echt 2760 (min Bucket 97), echt teils nicht mehr verdeckt */
    { 0x4080,  2,  12,  274 },   /* alt 226, falsch 1106588 (max Bucket 274), echt 105574 (min Bucket 278) */
    { 0x4080,  2,  14,  256 },   /* alt 226, falsch 64904 (max Bucket 256), echt 0 (min Bucket -1) */
    { 0x4080,  2,  19,  140 },   /* alt 79, falsch 4292 (max Bucket 140), echt 370 (min Bucket 91), echt teils nicht mehr verdeckt */
    { 0x4080,  2,  20,  144 },   /* alt 81, falsch 4538 (max Bucket 144), echt 884 (min Bucket 93), echt teils nicht mehr verdeckt */
    { 0x4080,  2,  21,  256 },   /* alt 232, falsch 1580 (max Bucket 256), echt 0 (min Bucket -1) */
    { 0x4080,  2,  22,  143 },   /* alt 83, falsch 4631 (max Bucket 143), echt 664 (min Bucket 98), echt teils nicht mehr verdeckt */
    { 0x4080,  2,  23,   95 },   /* alt 86, falsch 3410 (max Bucket 95), echt 0 (min Bucket -1) */
    { 0x4080,  2,  24,   91 },   /* alt 83, falsch 1506 (max Bucket 91), echt 0 (min Bucket -1) */
    { 0x4080,  2,  25,   89 },   /* alt 81, falsch 1491 (max Bucket 89), echt 0 (min Bucket -1) */
    { 0x4080,  2,  26,   89 },   /* alt 80, falsch 1478 (max Bucket 89), echt 0 (min Bucket -1) */
    { 0x4080,  2,  27,   86 },   /* alt 79, falsch 3167 (max Bucket 86), echt 0 (min Bucket -1) */
    { 0x4080,  3,   4,  175 },   /* alt 140, falsch 1172739 (max Bucket 175), echt 54132 (min Bucket 198) */
    { 0x4080,  3,   5,  153 },   /* alt 140, falsch 173581 (max Bucket 153), echt 0 (min Bucket -1) */
    { 0x4080,  3,   6,  175 },   /* alt 168, falsch 30088 (max Bucket 175), echt 7107 (min Bucket 196) */
    { 0x4080,  3,   7,  167 },   /* alt 148, falsch 14721 (max Bucket 167), echt 0 (min Bucket -1) */
    { 0x4080,  3,   8,  173 },   /* alt 140, falsch 924557 (max Bucket 173), echt 29614 (min Bucket 195) */
    { 0x4080,  3,  17,  173 },   /* alt 161, falsch 1682 (max Bucket 173), echt 0 (min Bucket -1) */
    { 0x4080,  3,  18,  169 },   /* alt 158, falsch 1665 (max Bucket 169), echt 0 (min Bucket -1) */
    { 0x4080,  3,  19,  163 },   /* alt 151, falsch 885 (max Bucket 163), echt 0 (min Bucket -1) */
    { 0x4080,  3,  20,  161 },   /* alt 148, falsch 2921 (max Bucket 161), echt 0 (min Bucket -1) */
    { 0x4080,  3,  21,  157 },   /* alt 146, falsch 2228 (max Bucket 157), echt 0 (min Bucket -1) */
    { 0x4080,  3,  22,  155 },   /* alt 143, falsch 2846 (max Bucket 155), echt 0 (min Bucket -1) */
    { 0x4080,  5,   1,  126 },   /* alt 113, falsch 221145 (max Bucket 126), echt 87845 (min Bucket 154) */
    { 0x4080,  5,   2,  137 },   /* alt 121, falsch 44035 (max Bucket 137), echt 22799 (min Bucket 156) */
    { 0x4080,  5,   4,  133 },   /* alt 118, falsch 14017 (max Bucket 133), echt 3348 (min Bucket 155) */
    { 0x4080,  5,   5,  141 },   /* alt 129, falsch 11378 (max Bucket 141), echt 3804 (min Bucket 159) */
    { 0x4080,  5,   6,  124 },   /* alt 118, falsch 3749645 (max Bucket 124), echt 192287 (min Bucket 164) */
    { 0x4080,  5,   7,  142 },   /* alt 129, falsch 466259 (max Bucket 142), echt 133958 (min Bucket 163) */
    { 0x4080,  5,   9,  147 },   /* alt 140, falsch 91801 (max Bucket 147), echt 29856 (min Bucket 163) */
    { 0x4080,  5,  11,  150 },   /* alt 145, falsch 22151 (max Bucket 150), echt 7488 (min Bucket 166) */
    { 0x4080,  5,  13,  132 },   /* alt 124, falsch 8883 (max Bucket 132), echt 0 (min Bucket -1) */
    { 0x4080,  5,  23,  152 },   /* alt 148, falsch 8654 (max Bucket 152), echt 1378 (min Bucket 168) */
    { 0x4080,  5,  24,  143 },   /* alt 139, falsch 879 (max Bucket 143), echt 0 (min Bucket -1) */
    { 0x4080,  5,  25,  139 },   /* alt 134, falsch 863 (max Bucket 139), echt 0 (min Bucket -1) */
    { 0x4080,  5,  26,  137 },   /* alt 132, falsch 2593 (max Bucket 137), echt 0 (min Bucket -1) */
    { 0x4080,  5,  27,  132 },   /* alt 129, falsch 1933 (max Bucket 132), echt 0 (min Bucket -1) */
    { 0x4080,  5,  28,  130 },   /* alt 125, falsch 822 (max Bucket 130), echt 0 (min Bucket -1) */
    { 0x4080,  5,  29,  128 },   /* alt 123, falsch 1348 (max Bucket 128), echt 0 (min Bucket -1) */
    { 0x4080,  5,  30,  126 },   /* alt 120, falsch 1853 (max Bucket 126), echt 0 (min Bucket -1) */
    { 0x4080,  5,  31,  123 },   /* alt 118, falsch 1371 (max Bucket 123), echt 0 (min Bucket -1) */
    { 0x4080,  6,   0,  193 },   /* alt 187, falsch 1157586 (max Bucket 193), echt 90617 (min Bucket 257) */
    { 0x4080,  6,   1,  241 },   /* alt 211, falsch 243160 (max Bucket 241), echt 117044 (min Bucket 253) */
    { 0x4080,  6,   2,  230 },   /* alt 204, falsch 59862 (max Bucket 230), echt 3164 (min Bucket 266) */
    { 0x4080,  6,   4,  210 },   /* alt 196, falsch 13131 (max Bucket 210), echt 0 (min Bucket -1) */
    { 0x4080,  6,   6,  284 },   /* alt 183, falsch 151577 (max Bucket 284), echt 97426 (min Bucket 233), echt teils nicht mehr verdeckt */
    { 0x4080,  6,   8,  294 },   /* alt 183, falsch 49616 (max Bucket 294), echt 12716 (min Bucket 242), echt teils nicht mehr verdeckt */
    { 0x4080,  6,  11,  287 },   /* alt 183, falsch 10140 (max Bucket 287), echt 5142 (min Bucket 236), echt teils nicht mehr verdeckt */
    { 0x4080,  6,  22,   15 },   /* alt 0, falsch 274 (max Bucket 15), echt 0 (min Bucket -1) */
    { 0x4080,  6,  25,  291 },   /* alt 183, falsch 6343 (max Bucket 291), echt 2578 (min Bucket 240), echt teils nicht mehr verdeckt */
    { 0x4080,  6,  26,  298 },   /* alt 198, falsch 10123 (max Bucket 298), echt 1339 (min Bucket 247), echt teils nicht mehr verdeckt */
    { 0x4080,  6,  27,  239 },   /* alt 236, falsch 6055 (max Bucket 239), echt 441 (min Bucket 266) */
    { 0x4080,  6,  28,  233 },   /* alt 230, falsch 6824 (max Bucket 233), echt 438 (min Bucket 266) */
    { 0x4080,  6,  29,  256 },   /* alt 215, falsch 3125 (max Bucket 256), echt 21 (min Bucket 249), echt teils nicht mehr verdeckt */
    { 0x4080,  6,  30,  224 },   /* alt 215, falsch 1191 (max Bucket 224), echt 0 (min Bucket -1) */
    { 0x4080,  6,  31,  218 },   /* alt 211, falsch 2010 (max Bucket 218), echt 0 (min Bucket -1) */
    { 0x4080,  6,  32,  214 },   /* alt 206, falsch 2888 (max Bucket 214), echt 0 (min Bucket -1) */
    { 0x4080,  6,  33,  206 },   /* alt 198, falsch 1101 (max Bucket 206), echt 0 (min Bucket -1) */
    { 0x4080,  6,  34,  202 },   /* alt 194, falsch 1884 (max Bucket 202), echt 0 (min Bucket -1) */
    { 0x4080,  6,  35,  198 },   /* alt 190, falsch 2629 (max Bucket 198), echt 0 (min Bucket -1) */
    { 0x4080,  7,   7,  125 },   /* alt 123, falsch 651660 (max Bucket 125), echt 3098 (min Bucket 134) */
    { 0x4080,  7,   9,  125 },   /* alt 123, falsch 45083 (max Bucket 125), echt 819 (min Bucket 144) */
    { 0x4080,  7,  12,  125 },   /* alt 123, falsch 10513 (max Bucket 125), echt 157 (min Bucket 146) */
    { 0x4080,  7,  15,  132 },   /* alt 126, falsch 780176 (max Bucket 132), echt 0 (min Bucket -1) */
    { 0x4080,  7,  17,  132 },   /* alt 126, falsch 14871 (max Bucket 132), echt 0 (min Bucket -1) */
    { 0x4080,  7,  29,  126 },   /* alt 124, falsch 4394 (max Bucket 126), echt 3 (min Bucket 149) */
    { 0x4080,  8,   0,  132 },   /* alt 113, falsch 625 (max Bucket 132), echt 78326 (min Bucket 83), echt teils nicht mehr verdeckt */
    { 0x4080,  8,  24,  120 },   /* alt 113, falsch 1568 (max Bucket 120), echt 0 (min Bucket -1) */
    { 0x4080,  8,  60,  119 },   /* alt 113, falsch 194 (max Bucket 119), echt 0 (min Bucket -1) */
    { 0x4081,  1,   3,  174 },   /* alt 157, falsch 1731952 (max Bucket 174), echt 151184 (min Bucket 195) */
    { 0x4081,  1,   5,  171 },   /* alt 162, falsch 12627 (max Bucket 171), echt 0 (min Bucket -1) */
    { 0x4081,  1,   6,   11 },   /* alt 0, falsch 46850 (max Bucket 11), echt 0 (min Bucket -1) */
    { 0x4081,  1,  12,  173 },   /* alt 167, falsch 1603 (max Bucket 173), echt 0 (min Bucket -1) */
    { 0x4081,  1,  13,  173 },   /* alt 171, falsch 864 (max Bucket 173), echt 0 (min Bucket -1) */
    { 0x4081,  1,  14,  163 },   /* alt 160, falsch 2158 (max Bucket 163), echt 0 (min Bucket -1) */
    { 0x4081,  2,   0,  137 },   /* alt 79, falsch 1772804 (max Bucket 137), echt 1423282 (min Bucket 89), echt teils nicht mehr verdeckt */
    { 0x4081,  2,   1,  147 },   /* alt 80, falsch 125699 (max Bucket 147), echt 46361 (min Bucket 95), echt teils nicht mehr verdeckt */
    { 0x4081,  2,   2,  128 },   /* alt 83, falsch 35382 (max Bucket 128), echt 3842 (min Bucket 99), echt teils nicht mehr verdeckt */
    { 0x4081,  2,   3,  142 },   /* alt 79, falsch 29394 (max Bucket 142), echt 21478 (min Bucket 91), echt teils nicht mehr verdeckt */
    { 0x4081,  2,   4,  114 },   /* alt 85, falsch 8396 (max Bucket 114), echt 145 (min Bucket 101), echt teils nicht mehr verdeckt */
    { 0x4081,  2,   5,  139 },   /* alt 79, falsch 5936 (max Bucket 139), echt 2428 (min Bucket 90), echt teils nicht mehr verdeckt */
    { 0x4081,  2,   6,  146 },   /* alt 81, falsch 8213 (max Bucket 146), echt 3692 (min Bucket 94), echt teils nicht mehr verdeckt */
    { 0x4081,  2,   7,  149 },   /* alt 83, falsch 7920 (max Bucket 149), echt 2760 (min Bucket 97), echt teils nicht mehr verdeckt */
    { 0x4081,  2,  12,  274 },   /* alt 226, falsch 1106588 (max Bucket 274), echt 105574 (min Bucket 278) */
    { 0x4081,  2,  14,  256 },   /* alt 226, falsch 64904 (max Bucket 256), echt 0 (min Bucket -1) */
    { 0x4081,  2,  19,  140 },   /* alt 79, falsch 4292 (max Bucket 140), echt 370 (min Bucket 91), echt teils nicht mehr verdeckt */
    { 0x4081,  2,  20,  144 },   /* alt 81, falsch 4538 (max Bucket 144), echt 884 (min Bucket 93), echt teils nicht mehr verdeckt */
    { 0x4081,  2,  21,  256 },   /* alt 232, falsch 1580 (max Bucket 256), echt 0 (min Bucket -1) */
    { 0x4081,  2,  22,  143 },   /* alt 83, falsch 4631 (max Bucket 143), echt 664 (min Bucket 98), echt teils nicht mehr verdeckt */
    { 0x4081,  2,  23,   95 },   /* alt 86, falsch 3410 (max Bucket 95), echt 0 (min Bucket -1) */
    { 0x4081,  2,  24,   91 },   /* alt 83, falsch 1506 (max Bucket 91), echt 0 (min Bucket -1) */
    { 0x4081,  2,  25,   89 },   /* alt 81, falsch 1491 (max Bucket 89), echt 0 (min Bucket -1) */
    { 0x4081,  2,  26,   89 },   /* alt 80, falsch 1478 (max Bucket 89), echt 0 (min Bucket -1) */
    { 0x4081,  2,  27,   86 },   /* alt 79, falsch 3167 (max Bucket 86), echt 0 (min Bucket -1) */
    { 0x4081,  3,   4,  175 },   /* alt 140, falsch 1172739 (max Bucket 175), echt 54132 (min Bucket 198) */
    { 0x4081,  3,   5,  153 },   /* alt 140, falsch 173581 (max Bucket 153), echt 0 (min Bucket -1) */
    { 0x4081,  3,   6,  175 },   /* alt 168, falsch 30088 (max Bucket 175), echt 7107 (min Bucket 196) */
    { 0x4081,  3,   7,  167 },   /* alt 148, falsch 14721 (max Bucket 167), echt 0 (min Bucket -1) */
    { 0x4081,  3,   8,  173 },   /* alt 140, falsch 924557 (max Bucket 173), echt 29614 (min Bucket 195) */
    { 0x4081,  3,  17,  173 },   /* alt 161, falsch 1682 (max Bucket 173), echt 0 (min Bucket -1) */
    { 0x4081,  3,  18,  169 },   /* alt 158, falsch 1665 (max Bucket 169), echt 0 (min Bucket -1) */
    { 0x4081,  3,  19,  163 },   /* alt 151, falsch 885 (max Bucket 163), echt 0 (min Bucket -1) */
    { 0x4081,  3,  20,  161 },   /* alt 148, falsch 2921 (max Bucket 161), echt 0 (min Bucket -1) */
    { 0x4081,  3,  21,  157 },   /* alt 146, falsch 2228 (max Bucket 157), echt 0 (min Bucket -1) */
    { 0x4081,  3,  22,  155 },   /* alt 143, falsch 2846 (max Bucket 155), echt 0 (min Bucket -1) */
    { 0x4081,  5,   1,  126 },   /* alt 113, falsch 221145 (max Bucket 126), echt 87845 (min Bucket 154) */
    { 0x4081,  5,   2,  137 },   /* alt 121, falsch 44035 (max Bucket 137), echt 22799 (min Bucket 156) */
    { 0x4081,  5,   4,  133 },   /* alt 118, falsch 14017 (max Bucket 133), echt 3348 (min Bucket 155) */
    { 0x4081,  5,   5,  141 },   /* alt 129, falsch 11378 (max Bucket 141), echt 3804 (min Bucket 159) */
    { 0x4081,  5,   6,  124 },   /* alt 118, falsch 3749645 (max Bucket 124), echt 192287 (min Bucket 164) */
    { 0x4081,  5,   7,  142 },   /* alt 129, falsch 466259 (max Bucket 142), echt 133958 (min Bucket 163) */
    { 0x4081,  5,   9,  147 },   /* alt 140, falsch 91801 (max Bucket 147), echt 29856 (min Bucket 163) */
    { 0x4081,  5,  11,  150 },   /* alt 145, falsch 22151 (max Bucket 150), echt 7488 (min Bucket 166) */
    { 0x4081,  5,  13,  132 },   /* alt 124, falsch 8883 (max Bucket 132), echt 0 (min Bucket -1) */
    { 0x4081,  5,  23,  152 },   /* alt 148, falsch 8654 (max Bucket 152), echt 1378 (min Bucket 168) */
    { 0x4081,  5,  24,  143 },   /* alt 139, falsch 879 (max Bucket 143), echt 0 (min Bucket -1) */
    { 0x4081,  5,  25,  139 },   /* alt 134, falsch 863 (max Bucket 139), echt 0 (min Bucket -1) */
    { 0x4081,  5,  26,  137 },   /* alt 132, falsch 2593 (max Bucket 137), echt 0 (min Bucket -1) */
    { 0x4081,  5,  27,  132 },   /* alt 129, falsch 1933 (max Bucket 132), echt 0 (min Bucket -1) */
    { 0x4081,  5,  28,  130 },   /* alt 125, falsch 822 (max Bucket 130), echt 0 (min Bucket -1) */
    { 0x4081,  5,  29,  128 },   /* alt 123, falsch 1348 (max Bucket 128), echt 0 (min Bucket -1) */
    { 0x4081,  5,  30,  126 },   /* alt 120, falsch 1853 (max Bucket 126), echt 0 (min Bucket -1) */
    { 0x4081,  5,  31,  123 },   /* alt 118, falsch 1371 (max Bucket 123), echt 0 (min Bucket -1) */
    { 0x4081,  6,   0,  193 },   /* alt 187, falsch 1157586 (max Bucket 193), echt 90617 (min Bucket 257) */
    { 0x4081,  6,   1,  241 },   /* alt 211, falsch 243160 (max Bucket 241), echt 117044 (min Bucket 253) */
    { 0x4081,  6,   2,  230 },   /* alt 204, falsch 59862 (max Bucket 230), echt 3164 (min Bucket 266) */
    { 0x4081,  6,   4,  210 },   /* alt 196, falsch 13131 (max Bucket 210), echt 0 (min Bucket -1) */
    { 0x4081,  6,   6,  284 },   /* alt 183, falsch 151577 (max Bucket 284), echt 97426 (min Bucket 233), echt teils nicht mehr verdeckt */
    { 0x4081,  6,   8,  294 },   /* alt 183, falsch 49616 (max Bucket 294), echt 12716 (min Bucket 242), echt teils nicht mehr verdeckt */
    { 0x4081,  6,  11,  287 },   /* alt 183, falsch 10140 (max Bucket 287), echt 5142 (min Bucket 236), echt teils nicht mehr verdeckt */
    { 0x4081,  6,  22,   15 },   /* alt 0, falsch 274 (max Bucket 15), echt 0 (min Bucket -1) */
    { 0x4081,  6,  25,  291 },   /* alt 183, falsch 6343 (max Bucket 291), echt 2578 (min Bucket 240), echt teils nicht mehr verdeckt */
    { 0x4081,  6,  26,  298 },   /* alt 198, falsch 10123 (max Bucket 298), echt 1339 (min Bucket 247), echt teils nicht mehr verdeckt */
    { 0x4081,  6,  27,  239 },   /* alt 236, falsch 6055 (max Bucket 239), echt 441 (min Bucket 266) */
    { 0x4081,  6,  28,  233 },   /* alt 230, falsch 6824 (max Bucket 233), echt 438 (min Bucket 266) */
    { 0x4081,  6,  29,  256 },   /* alt 215, falsch 3125 (max Bucket 256), echt 21 (min Bucket 249), echt teils nicht mehr verdeckt */
    { 0x4081,  6,  30,  224 },   /* alt 215, falsch 1191 (max Bucket 224), echt 0 (min Bucket -1) */
    { 0x4081,  6,  31,  218 },   /* alt 211, falsch 2010 (max Bucket 218), echt 0 (min Bucket -1) */
    { 0x4081,  6,  32,  214 },   /* alt 206, falsch 2888 (max Bucket 214), echt 0 (min Bucket -1) */
    { 0x4081,  6,  33,  206 },   /* alt 198, falsch 1101 (max Bucket 206), echt 0 (min Bucket -1) */
    { 0x4081,  6,  34,  202 },   /* alt 194, falsch 1884 (max Bucket 202), echt 0 (min Bucket -1) */
    { 0x4081,  6,  35,  198 },   /* alt 190, falsch 2629 (max Bucket 198), echt 0 (min Bucket -1) */
    { 0x4081,  7,   7,  125 },   /* alt 123, falsch 651660 (max Bucket 125), echt 3098 (min Bucket 134) */
    { 0x4081,  7,   9,  125 },   /* alt 123, falsch 45083 (max Bucket 125), echt 819 (min Bucket 144) */
    { 0x4081,  7,  12,  125 },   /* alt 123, falsch 10513 (max Bucket 125), echt 157 (min Bucket 146) */
    { 0x4081,  7,  15,  132 },   /* alt 126, falsch 780176 (max Bucket 132), echt 0 (min Bucket -1) */
    { 0x4081,  7,  17,  132 },   /* alt 126, falsch 14871 (max Bucket 132), echt 0 (min Bucket -1) */
    { 0x4081,  7,  29,  126 },   /* alt 124, falsch 4394 (max Bucket 126), echt 3 (min Bucket 149) */
    { 0x4081,  8,   0,  132 },   /* alt 113, falsch 625 (max Bucket 132), echt 78326 (min Bucket 83), echt teils nicht mehr verdeckt */
    { 0x4081,  8,  24,  120 },   /* alt 113, falsch 1568 (max Bucket 120), echt 0 (min Bucket -1) */
    { 0x4081,  8,  60,  119 },   /* alt 113, falsch 194 (max Bucket 119), echt 0 (min Bucket -1) */
    { 0x5020,  0,   0,  111 },   /* alt 99, falsch 869212 (max Bucket 111), echt 0 (min Bucket -1) */
    { 0x5020,  0,   1,  133 },   /* alt 123, falsch 297282 (max Bucket 133), echt 21077 (min Bucket 146) */
    { 0x5020,  0,   3,  122 },   /* alt 110, falsch 210760 (max Bucket 122), echt 4474 (min Bucket 151) */
    { 0x5020,  0,  13,  105 },   /* alt 102, falsch 1305869 (max Bucket 105), echt 0 (min Bucket -1) */
    { 0x5020,  0,  15,  105 },   /* alt 99, falsch 59413 (max Bucket 105), echt 0 (min Bucket -1) */
    { 0x5020,  0,  18,  105 },   /* alt 97, falsch 17156 (max Bucket 105), echt 0 (min Bucket -1) */
    { 0x5020,  0,  24,  125 },   /* alt 107, falsch 907059 (max Bucket 125), echt 12966 (min Bucket 152) */
    { 0x5020,  0,  25,  136 },   /* alt 124, falsch 268083 (max Bucket 136), echt 45035 (min Bucket 147) */
    { 0x5020,  0,  26,  114 },   /* alt 101, falsch 46969 (max Bucket 114), echt 0 (min Bucket -1) */
    { 0x5020,  0,  27,  126 },   /* alt 119, falsch 31400 (max Bucket 126), echt 4105 (min Bucket 150) */
    { 0x5020,  0,  28,  129 },   /* alt 119, falsch 22297 (max Bucket 129), echt 0 (min Bucket -1) */
    { 0x5020,  0,  29,  175 },   /* alt 127, falsch 19373 (max Bucket 175), echt 3421 (min Bucket 150), echt teils nicht mehr verdeckt */
    { 0x5020,  0,  30,  119 },   /* alt 108, falsch 14337 (max Bucket 119), echt 0 (min Bucket -1) */
    { 0x5020,  0,  31,  109 },   /* alt 99, falsch 9936 (max Bucket 109), echt 0 (min Bucket -1) */
    { 0x5020,  0,  57,  123 },   /* alt 117, falsch 12943 (max Bucket 123), echt 218 (min Bucket 152) */
    { 0x5020,  0,  69,  126 },   /* alt 117, falsch 9787 (max Bucket 126), echt 959 (min Bucket 152) */
    { 0x5020,  0,  79,  136 },   /* alt 125, falsch 1799 (max Bucket 136), echt 0 (min Bucket -1) */
    { 0x5020,  0,  80,  133 },   /* alt 123, falsch 2389 (max Bucket 133), echt 0 (min Bucket -1) */
    { 0x5020,  0,  81,  130 },   /* alt 121, falsch 3459 (max Bucket 130), echt 0 (min Bucket -1) */
    { 0x5020,  0,  82,  125 },   /* alt 115, falsch 1128 (max Bucket 125), echt 0 (min Bucket -1) */
    { 0x5020,  0,  83,  123 },   /* alt 113, falsch 3244 (max Bucket 123), echt 0 (min Bucket -1) */
    { 0x5020,  0,  84,  121 },   /* alt 111, falsch 3187 (max Bucket 121), echt 0 (min Bucket -1) */
    { 0x5020,  0,  87,  116 },   /* alt 108, falsch 1053 (max Bucket 116), echt 0 (min Bucket -1) */
    { 0x5020,  0,  88,  115 },   /* alt 107, falsch 2051 (max Bucket 115), echt 0 (min Bucket -1) */
    { 0x5020,  0,  93,  112 },   /* alt 104, falsch 1011 (max Bucket 112), echt 0 (min Bucket -1) */
    { 0x5020,  0,  94,  111 },   /* alt 102, falsch 1951 (max Bucket 111), echt 0 (min Bucket -1) */
    { 0x5020,  0,  97,  107 },   /* alt 99, falsch 967 (max Bucket 107), echt 0 (min Bucket -1) */
    { 0x5020,  0,  98,  105 },   /* alt 98, falsch 1914 (max Bucket 105), echt 0 (min Bucket -1) */
    { 0x5020,  0,  99,  105 },   /* alt 97, falsch 3621 (max Bucket 105), echt 0 (min Bucket -1) */
    { 0x5020,  6,   0,   97 },   /* alt 92, falsch 2561238 (max Bucket 97), echt 0 (min Bucket -1) */
    { 0x5020,  6,   1,   91 },   /* alt 86, falsch 62005 (max Bucket 91), echt 0 (min Bucket -1) */
    { 0x5020,  6,   2,  144 },   /* alt 103, falsch 92339 (max Bucket 144), echt 725 (min Bucket 116), echt teils nicht mehr verdeckt */
    { 0x5020,  6,   3,  131 },   /* alt 96, falsch 56444 (max Bucket 131), echt 0 (min Bucket -1) */
    { 0x5020,  6,   4,   89 },   /* alt 85, falsch 29076 (max Bucket 89), echt 0 (min Bucket -1) */
    { 0x5020,  6,   5,  140 },   /* alt 92, falsch 33772 (max Bucket 140), echt 430 (min Bucket 116), echt teils nicht mehr verdeckt */
    { 0x5020,  6,   7,  140 },   /* alt 106, falsch 20972 (max Bucket 140), echt 1176 (min Bucket 116), echt teils nicht mehr verdeckt */
    { 0x5020,  6,   8,   86 },   /* alt 84, falsch 12218 (max Bucket 86), echt 0 (min Bucket -1) */
    { 0x5020,  6,   9,  141 },   /* alt 106, falsch 19491 (max Bucket 141), echt 615 (min Bucket 116), echt teils nicht mehr verdeckt */
    { 0x5020,  6,  10,  142 },   /* alt 101, falsch 15457 (max Bucket 142), echt 392 (min Bucket 116), echt teils nicht mehr verdeckt */
    { 0x5020,  6,  11,   97 },   /* alt 94, falsch 11656 (max Bucket 97), echt 5 (min Bucket 138) */
    { 0x5020,  6,  12,  151 },   /* alt 146, falsch 14469 (max Bucket 151), echt 0 (min Bucket -1) */
    { 0x5020,  6,  13,  118 },   /* alt 103, falsch 9527 (max Bucket 118), echt 3 (min Bucket 116), echt teils nicht mehr verdeckt */
    { 0x5020,  6,  14,   96 },   /* alt 91, falsch 8311 (max Bucket 96), echt 0 (min Bucket -1) */
    { 0x5020,  6,  25,  154 },   /* alt 107, falsch 1647381 (max Bucket 154), echt 7276 (min Bucket 116), echt teils nicht mehr verdeckt */
    { 0x5020,  6,  26,  138 },   /* alt 103, falsch 63171 (max Bucket 138), echt 381 (min Bucket 116), echt teils nicht mehr verdeckt */
    { 0x5020,  6,  28,  151 },   /* alt 146, falsch 23531 (max Bucket 151), echt 0 (min Bucket -1) */
    { 0x5020,  6,  29,  152 },   /* alt 151, falsch 21951 (max Bucket 152), echt 0 (min Bucket -1) */
    { 0x5020,  6,  30,  129 },   /* alt 106, falsch 13183 (max Bucket 129), echt 82 (min Bucket 116), echt teils nicht mehr verdeckt */
    { 0x5020,  6,  31,   97 },   /* alt 94, falsch 12500 (max Bucket 97), echt 29 (min Bucket 138) */
    { 0x5020,  6,  32,  138 },   /* alt 96, falsch 16229 (max Bucket 138), echt 0 (min Bucket -1) */
    { 0x5020,  6,  33,  153 },   /* alt 151, falsch 14020 (max Bucket 153), echt 0 (min Bucket -1) */
    { 0x5020,  6,  34,  155 },   /* alt 146, falsch 721928 (max Bucket 155), echt 0 (min Bucket -1) */
    { 0x5020,  6,  35,  155 },   /* alt 151, falsch 328250 (max Bucket 155), echt 0 (min Bucket -1) */
    { 0x5020,  6,  36,  143 },   /* alt 103, falsch 92620 (max Bucket 143), echt 0 (min Bucket -1) */
    { 0x5020,  6,  37,  144 },   /* alt 103, falsch 58350 (max Bucket 144), echt 1025 (min Bucket 116), echt teils nicht mehr verdeckt */
    { 0x5020,  6,  38,  150 },   /* alt 141, falsch 71767 (max Bucket 150), echt 0 (min Bucket -1) */
    { 0x5020,  6,  39,  147 },   /* alt 99, falsch 47076 (max Bucket 147), echt 0 (min Bucket -1) */
    { 0x5020,  6,  40,  142 },   /* alt 106, falsch 31183 (max Bucket 142), echt 623 (min Bucket 116), echt teils nicht mehr verdeckt */
    { 0x5020,  6,  41,  144 },   /* alt 109, falsch 14937 (max Bucket 144), echt 0 (min Bucket -1) */
    { 0x5020,  6,  43,  142 },   /* alt 101, falsch 8457 (max Bucket 142), echt 0 (min Bucket -1) */
    { 0x5020,  6,  44,   97 },   /* alt 92, falsch 314844 (max Bucket 97), echt 15 (min Bucket 138) */
    { 0x5020,  6,  46,   97 },   /* alt 96, falsch 10953 (max Bucket 97), echt 48 (min Bucket 135) */
    { 0x5020,  6,  47,   96 },   /* alt 92, falsch 10010 (max Bucket 96), echt 0 (min Bucket -1) */
    { 0x5020,  6,  48,   88 },   /* alt 85, falsch 69061 (max Bucket 88), echt 0 (min Bucket -1) */
    { 0x5020,  6,  49,   87 },   /* alt 83, falsch 21126 (max Bucket 87), echt 0 (min Bucket -1) */
    { 0x5020,  6,  50,   89 },   /* alt 85, falsch 29449 (max Bucket 89), echt 0 (min Bucket -1) */
    { 0x5020,  6,  51,   88 },   /* alt 85, falsch 13843 (max Bucket 88), echt 0 (min Bucket -1) */
    { 0x5020,  6,  52,   89 },   /* alt 85, falsch 13280 (max Bucket 89), echt 0 (min Bucket -1) */
    { 0x5020,  6,  53,   87 },   /* alt 83, falsch 8934 (max Bucket 87), echt 0 (min Bucket -1) */
    { 0x5020,  6,  54,   87 },   /* alt 83, falsch 4751 (max Bucket 87), echt 0 (min Bucket -1) */
    { 0x5020,  6,  56,   86 },   /* alt 83, falsch 4127 (max Bucket 86), echt 0 (min Bucket -1) */
    { 0x5020,  6,  64,   97 },   /* alt 94, falsch 9561 (max Bucket 97), echt 20 (min Bucket 135) */
    { 0x5020,  6,  66,   97 },   /* alt 96, falsch 435 (max Bucket 97), echt 0 (min Bucket -1) */
    { 0x5020,  6,  67,  129 },   /* alt 109, falsch 1108 (max Bucket 129), echt 0 (min Bucket -1) */
    { 0x5020,  6,  68,  129 },   /* alt 104, falsch 2127 (max Bucket 129), echt 0 (min Bucket -1) */
    { 0x5020,  6,  69,  129 },   /* alt 101, falsch 3012 (max Bucket 129), echt 0 (min Bucket -1) */
    { 0x5020,  6,  74,  146 },   /* alt 101, falsch 35480 (max Bucket 146), echt 235 (min Bucket 122), echt teils nicht mehr verdeckt */
    { 0x5020,  6,  75,  140 },   /* alt 0, falsch 931 (max Bucket 140), echt 12 (min Bucket 128), echt teils nicht mehr verdeckt */
    { 0x5020,  6,  76,   97 },   /* alt 94, falsch 4340 (max Bucket 97), echt 10 (min Bucket 138) */
    { 0x5020,  6,  78,  146 },   /* alt 101, falsch 34606 (max Bucket 146), echt 378 (min Bucket 122), echt teils nicht mehr verdeckt */
    { 0x5020,  6,  79,  144 },   /* alt 103, falsch 19665 (max Bucket 144), echt 258 (min Bucket 116), echt teils nicht mehr verdeckt */
    { 0x5020,  6,  82,  149 },   /* alt 96, falsch 19628 (max Bucket 149), echt 0 (min Bucket -1) */
    { 0x5020,  6,  83,  145 },   /* alt 112, falsch 23623 (max Bucket 145), echt 0 (min Bucket -1) */
    { 0x5020,  6,  84,  145 },   /* alt 109, falsch 13199 (max Bucket 145), echt 0 (min Bucket -1) */
    { 0x5020,  6,  85,  141 },   /* alt 106, falsch 5992 (max Bucket 141), echt 276 (min Bucket 116), echt teils nicht mehr verdeckt */
    { 0x5020,  6,  86,  151 },   /* alt 146, falsch 4830 (max Bucket 151), echt 0 (min Bucket -1) */
    { 0x5020,  6,  87,  149 },   /* alt 137, falsch 3905 (max Bucket 149), echt 0 (min Bucket -1) */
    { 0x5020,  6,  90,   97 },   /* alt 90, falsch 7542 (max Bucket 97), echt 1 (min Bucket 138) */
    { 0x5020,  6,  91,   89 },   /* alt 83, falsch 14719 (max Bucket 89), echt 0 (min Bucket -1) */
    { 0x5020,  6,  94,  149 },   /* alt 96, falsch 45387 (max Bucket 149), echt 0 (min Bucket -1) */
    { 0x5020,  6,  95,  154 },   /* alt 151, falsch 9097 (max Bucket 154), echt 0 (min Bucket -1) */
    { 0x5020,  6,  96,  147 },   /* alt 94, falsch 6019 (max Bucket 147), echt 0 (min Bucket -1) */
    { 0x5020,  6,  97,  151 },   /* alt 146, falsch 710 (max Bucket 151), echt 0 (min Bucket -1) */
    { 0x5020,  6,  98,  149 },   /* alt 137, falsch 1964 (max Bucket 149), echt 0 (min Bucket -1) */
    { 0x5020,  6,  99,  143 },   /* alt 94, falsch 6389 (max Bucket 143), echt 0 (min Bucket -1) */
    { 0x5020,  7,  15,   64 },   /* alt 44, falsch 446185 (max Bucket 64), echt 0 (min Bucket -1) */
    { 0x5020,  7,  27,  118 },   /* alt 108, falsch 331618 (max Bucket 118), echt 28992 (min Bucket 137) */
    { 0x5020,  7,  31,  118 },   /* alt 108, falsch 46745 (max Bucket 118), echt 0 (min Bucket -1) */
    { 0x5020,  7,  32,  121 },   /* alt 114, falsch 41544 (max Bucket 121), echt 6268 (min Bucket 137) */
    { 0x5020,  7,  35,  123 },   /* alt 114, falsch 24711 (max Bucket 123), echt 480 (min Bucket 141) */
    { 0x5020,  7,  36,  114 },   /* alt 108, falsch 18827 (max Bucket 114), echt 0 (min Bucket -1) */
    { 0x5020,  7,  37,   64 },   /* alt 44, falsch 246568 (max Bucket 64), echt 0 (min Bucket -1) */
    { 0x5020,  7,  45,   49 },   /* alt 44, falsch 193435 (max Bucket 49), echt 0 (min Bucket -1) */
    { 0x5020,  7,  47,   55 },   /* alt 53, falsch 12132 (max Bucket 55), echt 0 (min Bucket -1) */
    { 0x5020,  7,  76,  114 },   /* alt 108, falsch 70235 (max Bucket 114), echt 3971 (min Bucket 137) */
    { 0x5020,  7,  77,  121 },   /* alt 113, falsch 16756 (max Bucket 121), echt 2441 (min Bucket 137) */
    { 0x5020,  7,  79,  155 },   /* alt 119, falsch 29337 (max Bucket 155), echt 2276 (min Bucket 137), echt teils nicht mehr verdeckt */
    { 0x5020,  7,  97,  120 },   /* alt 111, falsch 2834 (max Bucket 120), echt 0 (min Bucket -1) */
    { 0x5020,  7, 101,   54 },   /* alt 52, falsch 6191 (max Bucket 54), echt 0 (min Bucket -1) */
    { 0x5020, 11,   0,  235 },   /* alt 177, falsch 380887 (max Bucket 235), echt 0 (min Bucket -1) */
    { 0x5020, 11,   1,  235 },   /* alt 177, falsch 667719 (max Bucket 235), echt 657 (min Bucket 168), echt teils nicht mehr verdeckt */
    { 0x5020, 11,  26,  206 },   /* alt 177, falsch 46 (max Bucket 206), echt 0 (min Bucket -1) */
    { 0x5020, 11,  27,  210 },   /* alt 177, falsch 449 (max Bucket 210), echt 0 (min Bucket -1) */
    { 0x5020, 11,  31,  212 },   /* alt 177, falsch 195 (max Bucket 212), echt 0 (min Bucket -1) */
    { 0x5020, 11,  43,  220 },   /* alt 177, falsch 4427 (max Bucket 220), echt 0 (min Bucket -1) */
    { 0x5020, 11,  47,  216 },   /* alt 177, falsch 620 (max Bucket 216), echt 0 (min Bucket -1) */
    { 0x5020, 11,  48,  212 },   /* alt 177, falsch 134 (max Bucket 212), echt 0 (min Bucket -1) */
    { 0x5020, 11,  84,  217 },   /* alt 177, falsch 1112 (max Bucket 217), echt 0 (min Bucket -1) */
    { 0x5020, 12,   0,   73 },   /* alt 59, falsch 78917 (max Bucket 73), echt 0 (min Bucket -1) */
    { 0x5020, 12,   6,   60 },   /* alt 59, falsch 754 (max Bucket 60), echt 0 (min Bucket -1) */
    { 0x5020, 12,  16,   72 },   /* alt 0, falsch 1686 (max Bucket 72), echt 0 (min Bucket -1) */
    { 0x5020, 12,  17,   61 },   /* alt 0, falsch 193 (max Bucket 61), echt 0 (min Bucket -1) */
    { 0x5021,  0,   0,  111 },   /* alt 99, falsch 869212 (max Bucket 111), echt 0 (min Bucket -1) */
    { 0x5021,  0,   1,  133 },   /* alt 123, falsch 297282 (max Bucket 133), echt 21077 (min Bucket 146) */
    { 0x5021,  0,   3,  122 },   /* alt 110, falsch 210760 (max Bucket 122), echt 4474 (min Bucket 151) */
    { 0x5021,  0,  13,  105 },   /* alt 102, falsch 1305869 (max Bucket 105), echt 0 (min Bucket -1) */
    { 0x5021,  0,  15,  105 },   /* alt 99, falsch 59413 (max Bucket 105), echt 0 (min Bucket -1) */
    { 0x5021,  0,  18,  105 },   /* alt 97, falsch 17156 (max Bucket 105), echt 0 (min Bucket -1) */
    { 0x5021,  0,  24,  125 },   /* alt 107, falsch 907059 (max Bucket 125), echt 12966 (min Bucket 152) */
    { 0x5021,  0,  25,  136 },   /* alt 124, falsch 268083 (max Bucket 136), echt 45035 (min Bucket 147) */
    { 0x5021,  0,  26,  114 },   /* alt 101, falsch 46969 (max Bucket 114), echt 0 (min Bucket -1) */
    { 0x5021,  0,  27,  126 },   /* alt 119, falsch 31400 (max Bucket 126), echt 4105 (min Bucket 150) */
    { 0x5021,  0,  28,  129 },   /* alt 119, falsch 22297 (max Bucket 129), echt 0 (min Bucket -1) */
    { 0x5021,  0,  29,  175 },   /* alt 127, falsch 19373 (max Bucket 175), echt 3421 (min Bucket 150), echt teils nicht mehr verdeckt */
    { 0x5021,  0,  30,  119 },   /* alt 108, falsch 14337 (max Bucket 119), echt 0 (min Bucket -1) */
    { 0x5021,  0,  31,  109 },   /* alt 99, falsch 9936 (max Bucket 109), echt 0 (min Bucket -1) */
    { 0x5021,  0,  57,  123 },   /* alt 117, falsch 12943 (max Bucket 123), echt 218 (min Bucket 152) */
    { 0x5021,  0,  69,  126 },   /* alt 117, falsch 9787 (max Bucket 126), echt 959 (min Bucket 152) */
    { 0x5021,  0,  79,  136 },   /* alt 125, falsch 1799 (max Bucket 136), echt 0 (min Bucket -1) */
    { 0x5021,  0,  80,  133 },   /* alt 123, falsch 2389 (max Bucket 133), echt 0 (min Bucket -1) */
    { 0x5021,  0,  81,  130 },   /* alt 121, falsch 3459 (max Bucket 130), echt 0 (min Bucket -1) */
    { 0x5021,  0,  82,  125 },   /* alt 115, falsch 1128 (max Bucket 125), echt 0 (min Bucket -1) */
    { 0x5021,  0,  83,  123 },   /* alt 113, falsch 3244 (max Bucket 123), echt 0 (min Bucket -1) */
    { 0x5021,  0,  84,  121 },   /* alt 111, falsch 3187 (max Bucket 121), echt 0 (min Bucket -1) */
    { 0x5021,  0,  87,  116 },   /* alt 108, falsch 1053 (max Bucket 116), echt 0 (min Bucket -1) */
    { 0x5021,  0,  88,  115 },   /* alt 107, falsch 2051 (max Bucket 115), echt 0 (min Bucket -1) */
    { 0x5021,  0,  93,  112 },   /* alt 104, falsch 1011 (max Bucket 112), echt 0 (min Bucket -1) */
    { 0x5021,  0,  94,  111 },   /* alt 102, falsch 1951 (max Bucket 111), echt 0 (min Bucket -1) */
    { 0x5021,  0,  97,  107 },   /* alt 99, falsch 967 (max Bucket 107), echt 0 (min Bucket -1) */
    { 0x5021,  0,  98,  105 },   /* alt 98, falsch 1914 (max Bucket 105), echt 0 (min Bucket -1) */
    { 0x5021,  0,  99,  105 },   /* alt 97, falsch 3621 (max Bucket 105), echt 0 (min Bucket -1) */
    { 0x5021,  6,   0,   97 },   /* alt 92, falsch 2561238 (max Bucket 97), echt 0 (min Bucket -1) */
    { 0x5021,  6,   1,   91 },   /* alt 86, falsch 62005 (max Bucket 91), echt 0 (min Bucket -1) */
    { 0x5021,  6,   2,  144 },   /* alt 103, falsch 92339 (max Bucket 144), echt 725 (min Bucket 116), echt teils nicht mehr verdeckt */
    { 0x5021,  6,   3,  131 },   /* alt 96, falsch 56444 (max Bucket 131), echt 0 (min Bucket -1) */
    { 0x5021,  6,   4,   89 },   /* alt 85, falsch 29076 (max Bucket 89), echt 0 (min Bucket -1) */
    { 0x5021,  6,   5,  140 },   /* alt 92, falsch 33772 (max Bucket 140), echt 430 (min Bucket 116), echt teils nicht mehr verdeckt */
    { 0x5021,  6,   7,  140 },   /* alt 106, falsch 20972 (max Bucket 140), echt 1176 (min Bucket 116), echt teils nicht mehr verdeckt */
    { 0x5021,  6,   8,   86 },   /* alt 84, falsch 12218 (max Bucket 86), echt 0 (min Bucket -1) */
    { 0x5021,  6,   9,  141 },   /* alt 106, falsch 19491 (max Bucket 141), echt 615 (min Bucket 116), echt teils nicht mehr verdeckt */
    { 0x5021,  6,  10,  142 },   /* alt 101, falsch 15457 (max Bucket 142), echt 392 (min Bucket 116), echt teils nicht mehr verdeckt */
    { 0x5021,  6,  11,   97 },   /* alt 94, falsch 11656 (max Bucket 97), echt 5 (min Bucket 138) */
    { 0x5021,  6,  12,  151 },   /* alt 146, falsch 14469 (max Bucket 151), echt 0 (min Bucket -1) */
    { 0x5021,  6,  13,  118 },   /* alt 103, falsch 9527 (max Bucket 118), echt 3 (min Bucket 116), echt teils nicht mehr verdeckt */
    { 0x5021,  6,  14,   96 },   /* alt 91, falsch 8311 (max Bucket 96), echt 0 (min Bucket -1) */
    { 0x5021,  6,  25,  154 },   /* alt 107, falsch 1647381 (max Bucket 154), echt 7276 (min Bucket 116), echt teils nicht mehr verdeckt */
    { 0x5021,  6,  26,  138 },   /* alt 103, falsch 63171 (max Bucket 138), echt 381 (min Bucket 116), echt teils nicht mehr verdeckt */
    { 0x5021,  6,  28,  151 },   /* alt 146, falsch 23531 (max Bucket 151), echt 0 (min Bucket -1) */
    { 0x5021,  6,  29,  152 },   /* alt 151, falsch 21951 (max Bucket 152), echt 0 (min Bucket -1) */
    { 0x5021,  6,  30,  129 },   /* alt 106, falsch 13183 (max Bucket 129), echt 82 (min Bucket 116), echt teils nicht mehr verdeckt */
    { 0x5021,  6,  31,   97 },   /* alt 94, falsch 12500 (max Bucket 97), echt 29 (min Bucket 138) */
    { 0x5021,  6,  32,  138 },   /* alt 96, falsch 16229 (max Bucket 138), echt 0 (min Bucket -1) */
    { 0x5021,  6,  33,  153 },   /* alt 151, falsch 14020 (max Bucket 153), echt 0 (min Bucket -1) */
    { 0x5021,  6,  34,  155 },   /* alt 146, falsch 721928 (max Bucket 155), echt 0 (min Bucket -1) */
    { 0x5021,  6,  35,  155 },   /* alt 151, falsch 328250 (max Bucket 155), echt 0 (min Bucket -1) */
    { 0x5021,  6,  36,  143 },   /* alt 103, falsch 92620 (max Bucket 143), echt 0 (min Bucket -1) */
    { 0x5021,  6,  37,  144 },   /* alt 103, falsch 58350 (max Bucket 144), echt 1025 (min Bucket 116), echt teils nicht mehr verdeckt */
    { 0x5021,  6,  38,  150 },   /* alt 141, falsch 71767 (max Bucket 150), echt 0 (min Bucket -1) */
    { 0x5021,  6,  39,  147 },   /* alt 99, falsch 47076 (max Bucket 147), echt 0 (min Bucket -1) */
    { 0x5021,  6,  40,  142 },   /* alt 106, falsch 31183 (max Bucket 142), echt 623 (min Bucket 116), echt teils nicht mehr verdeckt */
    { 0x5021,  6,  41,  144 },   /* alt 109, falsch 14937 (max Bucket 144), echt 0 (min Bucket -1) */
    { 0x5021,  6,  43,  142 },   /* alt 101, falsch 8457 (max Bucket 142), echt 0 (min Bucket -1) */
    { 0x5021,  6,  44,   97 },   /* alt 92, falsch 314844 (max Bucket 97), echt 15 (min Bucket 138) */
    { 0x5021,  6,  46,   97 },   /* alt 96, falsch 10953 (max Bucket 97), echt 48 (min Bucket 135) */
    { 0x5021,  6,  47,   96 },   /* alt 92, falsch 10010 (max Bucket 96), echt 0 (min Bucket -1) */
    { 0x5021,  6,  48,   88 },   /* alt 85, falsch 69061 (max Bucket 88), echt 0 (min Bucket -1) */
    { 0x5021,  6,  49,   87 },   /* alt 83, falsch 21126 (max Bucket 87), echt 0 (min Bucket -1) */
    { 0x5021,  6,  50,   89 },   /* alt 85, falsch 29449 (max Bucket 89), echt 0 (min Bucket -1) */
    { 0x5021,  6,  51,   88 },   /* alt 85, falsch 13843 (max Bucket 88), echt 0 (min Bucket -1) */
    { 0x5021,  6,  52,   89 },   /* alt 85, falsch 13280 (max Bucket 89), echt 0 (min Bucket -1) */
    { 0x5021,  6,  53,   87 },   /* alt 83, falsch 8934 (max Bucket 87), echt 0 (min Bucket -1) */
    { 0x5021,  6,  54,   87 },   /* alt 83, falsch 4751 (max Bucket 87), echt 0 (min Bucket -1) */
    { 0x5021,  6,  56,   86 },   /* alt 83, falsch 4127 (max Bucket 86), echt 0 (min Bucket -1) */
    { 0x5021,  6,  64,   97 },   /* alt 94, falsch 9561 (max Bucket 97), echt 20 (min Bucket 135) */
    { 0x5021,  6,  66,   97 },   /* alt 96, falsch 435 (max Bucket 97), echt 0 (min Bucket -1) */
    { 0x5021,  6,  67,  129 },   /* alt 109, falsch 1108 (max Bucket 129), echt 0 (min Bucket -1) */
    { 0x5021,  6,  68,  129 },   /* alt 104, falsch 2127 (max Bucket 129), echt 0 (min Bucket -1) */
    { 0x5021,  6,  69,  129 },   /* alt 101, falsch 3012 (max Bucket 129), echt 0 (min Bucket -1) */
    { 0x5021,  6,  74,  146 },   /* alt 101, falsch 35480 (max Bucket 146), echt 235 (min Bucket 122), echt teils nicht mehr verdeckt */
    { 0x5021,  6,  75,  140 },   /* alt 0, falsch 931 (max Bucket 140), echt 12 (min Bucket 128), echt teils nicht mehr verdeckt */
    { 0x5021,  6,  76,   97 },   /* alt 94, falsch 4340 (max Bucket 97), echt 10 (min Bucket 138) */
    { 0x5021,  6,  78,  146 },   /* alt 101, falsch 34606 (max Bucket 146), echt 378 (min Bucket 122), echt teils nicht mehr verdeckt */
    { 0x5021,  6,  79,  144 },   /* alt 103, falsch 19665 (max Bucket 144), echt 258 (min Bucket 116), echt teils nicht mehr verdeckt */
    { 0x5021,  6,  82,  149 },   /* alt 96, falsch 19628 (max Bucket 149), echt 0 (min Bucket -1) */
    { 0x5021,  6,  83,  145 },   /* alt 112, falsch 23623 (max Bucket 145), echt 0 (min Bucket -1) */
    { 0x5021,  6,  84,  145 },   /* alt 109, falsch 13199 (max Bucket 145), echt 0 (min Bucket -1) */
    { 0x5021,  6,  85,  141 },   /* alt 106, falsch 5992 (max Bucket 141), echt 276 (min Bucket 116), echt teils nicht mehr verdeckt */
    { 0x5021,  6,  86,  151 },   /* alt 146, falsch 4830 (max Bucket 151), echt 0 (min Bucket -1) */
    { 0x5021,  6,  87,  149 },   /* alt 137, falsch 3905 (max Bucket 149), echt 0 (min Bucket -1) */
    { 0x5021,  6,  90,   97 },   /* alt 90, falsch 7542 (max Bucket 97), echt 1 (min Bucket 138) */
    { 0x5021,  6,  91,   89 },   /* alt 83, falsch 14719 (max Bucket 89), echt 0 (min Bucket -1) */
    { 0x5021,  6,  94,  149 },   /* alt 96, falsch 45387 (max Bucket 149), echt 0 (min Bucket -1) */
    { 0x5021,  6,  95,  154 },   /* alt 151, falsch 9097 (max Bucket 154), echt 0 (min Bucket -1) */
    { 0x5021,  6,  96,  147 },   /* alt 94, falsch 6019 (max Bucket 147), echt 0 (min Bucket -1) */
    { 0x5021,  6,  97,  151 },   /* alt 146, falsch 710 (max Bucket 151), echt 0 (min Bucket -1) */
    { 0x5021,  6,  98,  149 },   /* alt 137, falsch 1964 (max Bucket 149), echt 0 (min Bucket -1) */
    { 0x5021,  6,  99,  143 },   /* alt 94, falsch 6389 (max Bucket 143), echt 0 (min Bucket -1) */
    { 0x5021,  7,  15,   64 },   /* alt 44, falsch 446185 (max Bucket 64), echt 0 (min Bucket -1) */
    { 0x5021,  7,  27,  118 },   /* alt 108, falsch 331618 (max Bucket 118), echt 28992 (min Bucket 137) */
    { 0x5021,  7,  31,  118 },   /* alt 108, falsch 46745 (max Bucket 118), echt 0 (min Bucket -1) */
    { 0x5021,  7,  32,  121 },   /* alt 114, falsch 41544 (max Bucket 121), echt 6268 (min Bucket 137) */
    { 0x5021,  7,  35,  123 },   /* alt 114, falsch 24711 (max Bucket 123), echt 480 (min Bucket 141) */
    { 0x5021,  7,  36,  114 },   /* alt 108, falsch 18827 (max Bucket 114), echt 0 (min Bucket -1) */
    { 0x5021,  7,  37,   64 },   /* alt 44, falsch 246568 (max Bucket 64), echt 0 (min Bucket -1) */
    { 0x5021,  7,  45,   49 },   /* alt 44, falsch 193435 (max Bucket 49), echt 0 (min Bucket -1) */
    { 0x5021,  7,  47,   55 },   /* alt 53, falsch 12132 (max Bucket 55), echt 0 (min Bucket -1) */
    { 0x5021,  7,  76,  114 },   /* alt 108, falsch 70235 (max Bucket 114), echt 3971 (min Bucket 137) */
    { 0x5021,  7,  77,  121 },   /* alt 113, falsch 16756 (max Bucket 121), echt 2441 (min Bucket 137) */
    { 0x5021,  7,  79,  155 },   /* alt 119, falsch 29337 (max Bucket 155), echt 2276 (min Bucket 137), echt teils nicht mehr verdeckt */
    { 0x5021,  7,  97,  120 },   /* alt 111, falsch 2834 (max Bucket 120), echt 0 (min Bucket -1) */
    { 0x5021,  7, 101,   54 },   /* alt 52, falsch 6191 (max Bucket 54), echt 0 (min Bucket -1) */
    { 0x5021, 11,   0,  235 },   /* alt 177, falsch 380887 (max Bucket 235), echt 0 (min Bucket -1) */
    { 0x5021, 11,   1,  235 },   /* alt 177, falsch 667719 (max Bucket 235), echt 657 (min Bucket 168), echt teils nicht mehr verdeckt */
    { 0x5021, 11,  26,  206 },   /* alt 177, falsch 46 (max Bucket 206), echt 0 (min Bucket -1) */
    { 0x5021, 11,  27,  210 },   /* alt 177, falsch 449 (max Bucket 210), echt 0 (min Bucket -1) */
    { 0x5021, 11,  31,  212 },   /* alt 177, falsch 195 (max Bucket 212), echt 0 (min Bucket -1) */
    { 0x5021, 11,  43,  220 },   /* alt 177, falsch 4427 (max Bucket 220), echt 0 (min Bucket -1) */
    { 0x5021, 11,  47,  216 },   /* alt 177, falsch 620 (max Bucket 216), echt 0 (min Bucket -1) */
    { 0x5021, 11,  48,  212 },   /* alt 177, falsch 134 (max Bucket 212), echt 0 (min Bucket -1) */
    { 0x5021, 11,  84,  217 },   /* alt 177, falsch 1112 (max Bucket 217), echt 0 (min Bucket -1) */
    { 0x5021, 12,   0,   73 },   /* alt 59, falsch 78917 (max Bucket 73), echt 0 (min Bucket -1) */
    { 0x5021, 12,   6,   60 },   /* alt 59, falsch 754 (max Bucket 60), echt 0 (min Bucket -1) */
    { 0x5021, 12,  16,   72 },   /* alt 0, falsch 1686 (max Bucket 72), echt 0 (min Bucket -1) */
    { 0x5021, 12,  17,   61 },   /* alt 0, falsch 193 (max Bucket 61), echt 0 (min Bucket -1) */
    { 0x5050,  0,   3,   58 },   /* alt 57, falsch 8262 (max Bucket 58), echt 0 (min Bucket -1) */
    { 0x5050,  0,   8,   58 },   /* alt 56, falsch 2161 (max Bucket 58), echt 0 (min Bucket -1) */
    { 0x5050,  0,   9,   58 },   /* alt 57, falsch 1667 (max Bucket 58), echt 0 (min Bucket -1) */
    { 0x5050,  0,  10,   58 },   /* alt 57, falsch 884 (max Bucket 58), echt 0 (min Bucket -1) */
    { 0x5050,  0,  16,   62 },   /* alt 0, falsch 28669 (max Bucket 62), echt 0 (min Bucket -1) */
    { 0x5050,  0,  17,   58 },   /* alt 0, falsch 6888 (max Bucket 58), echt 0 (min Bucket -1) */
    { 0x5050,  1,   0,  150 },   /* alt 86, falsch 116044 (max Bucket 150), echt 0 (min Bucket -1) */
    { 0x5050,  1,   1,  150 },   /* alt 86, falsch 11521 (max Bucket 150), echt 0 (min Bucket -1) */
    { 0x5050,  1,   2,  143 },   /* alt 86, falsch 7297 (max Bucket 143), echt 0 (min Bucket -1) */
    { 0x5050,  1,   3,  143 },   /* alt 86, falsch 2215 (max Bucket 143), echt 0 (min Bucket -1) */
    { 0x5050,  1,   4,  148 },   /* alt 86, falsch 2694 (max Bucket 148), echt 0 (min Bucket -1) */
    { 0x5050,  1,   6,   85 },   /* alt 68, falsch 147 (max Bucket 85), echt 0 (min Bucket -1) */
    { 0x5050,  1,   7,   80 },   /* alt 68, falsch 148 (max Bucket 80), echt 0 (min Bucket -1) */
    { 0x5050,  1,   8,   85 },   /* alt 68, falsch 22 (max Bucket 85), echt 0 (min Bucket -1) */
    { 0x5050,  1,   9,   77 },   /* alt 68, falsch 104 (max Bucket 77), echt 0 (min Bucket -1) */
    { 0x5050,  1,  10,   87 },   /* alt 68, falsch 74 (max Bucket 87), echt 0 (min Bucket -1) */
    { 0x5050,  1,  11,   83 },   /* alt 68, falsch 1367 (max Bucket 83), echt 213104 (min Bucket 50), echt teils nicht mehr verdeckt */
    { 0x5050,  1,  13,   78 },   /* alt 68, falsch 2638 (max Bucket 78), echt 13662 (min Bucket 53), echt teils nicht mehr verdeckt */
    { 0x5050,  1,  22,  150 },   /* alt 68, falsch 15495 (max Bucket 150), echt 35575 (min Bucket 114), echt teils nicht mehr verdeckt */
    { 0x5050,  1,  43,  142 },   /* alt 86, falsch 519 (max Bucket 142), echt 0 (min Bucket -1) */
    { 0x5050,  1,  45,  143 },   /* alt 86, falsch 1011 (max Bucket 143), echt 0 (min Bucket -1) */
    { 0x5050,  1,  47,  146 },   /* alt 86, falsch 1169 (max Bucket 146), echt 0 (min Bucket -1) */
    { 0x5050,  1,  56,  148 },   /* alt 68, falsch 264 (max Bucket 148), echt 711 (min Bucket 114), echt teils nicht mehr verdeckt */
    { 0x5050,  1,  85,   84 },   /* alt 68, falsch 2 (max Bucket 84), echt 0 (min Bucket -1) */
    { 0x5050,  1,  88,   87 },   /* alt 68, falsch 11 (max Bucket 87), echt 0 (min Bucket -1) */
    { 0x5050,  1,  90,   86 },   /* alt 68, falsch 53 (max Bucket 86), echt 0 (min Bucket -1) */
    { 0x5050,  1,  95,   83 },   /* alt 68, falsch 115 (max Bucket 83), echt 0 (min Bucket -1) */
    { 0x5050,  1,  97,   79 },   /* alt 68, falsch 53 (max Bucket 79), echt 0 (min Bucket -1) */
    { 0x5050,  1, 102,   77 },   /* alt 68, falsch 44 (max Bucket 77), echt 0 (min Bucket -1) */
    { 0x5050,  2,   0,   87 },   /* alt 63, falsch 175895 (max Bucket 87), echt 0 (min Bucket -1) */
    { 0x5050,  2,   1,   94 },   /* alt 63, falsch 93733 (max Bucket 94), echt 0 (min Bucket -1) */
    { 0x5050,  2,   2,   82 },   /* alt 63, falsch 64102 (max Bucket 82), echt 0 (min Bucket -1) */
    { 0x5050,  2,   3,   87 },   /* alt 63, falsch 49797 (max Bucket 87), echt 0 (min Bucket -1) */
    { 0x5050,  2,   4,   75 },   /* alt 63, falsch 35820 (max Bucket 75), echt 0 (min Bucket -1) */
    { 0x5050,  2,   5,   83 },   /* alt 63, falsch 25849 (max Bucket 83), echt 0 (min Bucket -1) */
    { 0x5050,  2,   6,   90 },   /* alt 63, falsch 26578 (max Bucket 90), echt 0 (min Bucket -1) */
    { 0x5050,  2,   7,   87 },   /* alt 63, falsch 24328 (max Bucket 87), echt 0 (min Bucket -1) */
    { 0x5050,  2,   8,   93 },   /* alt 63, falsch 23626 (max Bucket 93), echt 0 (min Bucket -1) */
    { 0x5050,  2,   9,   77 },   /* alt 63, falsch 12983 (max Bucket 77), echt 0 (min Bucket -1) */
    { 0x5050,  2,  10,   86 },   /* alt 63, falsch 12115 (max Bucket 86), echt 0 (min Bucket -1) */
    { 0x5050,  2,  11,   92 },   /* alt 63, falsch 12578 (max Bucket 92), echt 0 (min Bucket -1) */
    { 0x5050,  2,  12,   98 },   /* alt 63, falsch 12515 (max Bucket 98), echt 0 (min Bucket -1) */
    { 0x5050,  2,  13,   87 },   /* alt 63, falsch 10257 (max Bucket 87), echt 0 (min Bucket -1) */
    { 0x5050,  2,  14,   74 },   /* alt 63, falsch 6795 (max Bucket 74), echt 0 (min Bucket -1) */
    { 0x5050,  2,  15,   91 },   /* alt 67, falsch 9745 (max Bucket 91), echt 0 (min Bucket -1) */
    { 0x5050,  2,  16,   82 },   /* alt 63, falsch 7455 (max Bucket 82), echt 0 (min Bucket -1) */
    { 0x5050,  2,  17,   73 },   /* alt 63, falsch 6019 (max Bucket 73), echt 0 (min Bucket -1) */
    { 0x5050,  2,  18,   82 },   /* alt 63, falsch 7108 (max Bucket 82), echt 0 (min Bucket -1) */
    { 0x5050,  2,  19,   75 },   /* alt 63, falsch 5915 (max Bucket 75), echt 0 (min Bucket -1) */
    { 0x5050,  2,  20,   88 },   /* alt 63, falsch 7464 (max Bucket 88), echt 0 (min Bucket -1) */
    { 0x5050,  2,  21,   76 },   /* alt 63, falsch 5596 (max Bucket 76), echt 0 (min Bucket -1) */
    { 0x5050,  2,  22,   70 },   /* alt 63, falsch 4931 (max Bucket 70), echt 0 (min Bucket -1) */
    { 0x5050,  2,  23,   93 },   /* alt 63, falsch 7690 (max Bucket 93), echt 0 (min Bucket -1) */
    { 0x5050,  2,  24,   77 },   /* alt 63, falsch 5179 (max Bucket 77), echt 0 (min Bucket -1) */
    { 0x5050,  2,  25,   73 },   /* alt 63, falsch 4376 (max Bucket 73), echt 0 (min Bucket -1) */
    { 0x5050,  2,  26,   79 },   /* alt 63, falsch 199552 (max Bucket 79), echt 0 (min Bucket -1) */
    { 0x5050,  2,  27,   71 },   /* alt 63, falsch 62618 (max Bucket 71), echt 0 (min Bucket -1) */
    { 0x5050,  2,  28,   75 },   /* alt 63, falsch 42774 (max Bucket 75), echt 0 (min Bucket -1) */
    { 0x5050,  2,  29,   82 },   /* alt 63, falsch 27034 (max Bucket 82), echt 0 (min Bucket -1) */
    { 0x5050,  2,  30,   71 },   /* alt 63, falsch 13367 (max Bucket 71), echt 0 (min Bucket -1) */
    { 0x5050,  2,  31,   72 },   /* alt 63, falsch 9666 (max Bucket 72), echt 0 (min Bucket -1) */
    { 0x5050,  2,  32,   70 },   /* alt 63, falsch 5527 (max Bucket 70), echt 0 (min Bucket -1) */
    { 0x5050,  2,  33,   77 },   /* alt 63, falsch 5911 (max Bucket 77), echt 0 (min Bucket -1) */
    { 0x5050,  2,  34,   81 },   /* alt 63, falsch 5603 (max Bucket 81), echt 0 (min Bucket -1) */
    { 0x5050,  2,  35,   74 },   /* alt 63, falsch 5020 (max Bucket 74), echt 0 (min Bucket -1) */
    { 0x5050,  2,  36,   71 },   /* alt 63, falsch 4467 (max Bucket 71), echt 0 (min Bucket -1) */
    { 0x5050,  2,  37,   70 },   /* alt 63, falsch 3878 (max Bucket 70), echt 0 (min Bucket -1) */
    { 0x5050,  2,  38,   84 },   /* alt 63, falsch 4959 (max Bucket 84), echt 0 (min Bucket -1) */
    { 0x5050,  2,  39,   71 },   /* alt 63, falsch 3314 (max Bucket 71), echt 0 (min Bucket -1) */
    { 0x5050,  2,  40,   70 },   /* alt 63, falsch 2813 (max Bucket 70), echt 0 (min Bucket -1) */
    { 0x5050,  2,  55,   99 },   /* alt 63, falsch 10749 (max Bucket 99), echt 0 (min Bucket -1) */
    { 0x5050,  2,  56,   98 },   /* alt 64, falsch 330 (max Bucket 98), echt 0 (min Bucket -1) */
    { 0x5050,  2,  57,   97 },   /* alt 63, falsch 42211 (max Bucket 97), echt 0 (min Bucket -1) */
    { 0x5050,  2,  58,   93 },   /* alt 63, falsch 303 (max Bucket 93), echt 0 (min Bucket -1) */
    { 0x5050,  2,  59,   93 },   /* alt 63, falsch 20696 (max Bucket 93), echt 0 (min Bucket -1) */
    { 0x5050,  2,  60,   91 },   /* alt 64, falsch 823 (max Bucket 91), echt 0 (min Bucket -1) */
    { 0x5050,  2,  61,   91 },   /* alt 67, falsch 284 (max Bucket 91), echt 0 (min Bucket -1) */
    { 0x5050,  2,  62,   91 },   /* alt 68, falsch 2460 (max Bucket 91), echt 0 (min Bucket -1) */
    { 0x5050,  2,  63,   90 },   /* alt 63, falsch 11768 (max Bucket 90), echt 0 (min Bucket -1) */
    { 0x5050,  2,  65,   87 },   /* alt 63, falsch 6046 (max Bucket 87), echt 0 (min Bucket -1) */
    { 0x5050,  2,  66,   87 },   /* alt 63, falsch 7098 (max Bucket 87), echt 0 (min Bucket -1) */
    { 0x5050,  2,  67,   88 },   /* alt 67, falsch 1057 (max Bucket 88), echt 0 (min Bucket -1) */
    { 0x5050,  2,  68,   88 },   /* alt 63, falsch 1344 (max Bucket 88), echt 0 (min Bucket -1) */
    { 0x5050,  2,  69,   84 },   /* alt 63, falsch 240 (max Bucket 84), echt 0 (min Bucket -1) */
    { 0x5050,  2,  70,   84 },   /* alt 63, falsch 10019 (max Bucket 84), echt 0 (min Bucket -1) */
    { 0x5050,  2,  71,   83 },   /* alt 63, falsch 48658 (max Bucket 83), echt 0 (min Bucket -1) */
    { 0x5050,  2,  72,   83 },   /* alt 63, falsch 5287 (max Bucket 83), echt 0 (min Bucket -1) */
    { 0x5050,  2,  73,   82 },   /* alt 63, falsch 14429 (max Bucket 82), echt 0 (min Bucket -1) */
    { 0x5050,  2,  75,   82 },   /* alt 63, falsch 975 (max Bucket 82), echt 0 (min Bucket -1) */
    { 0x5050,  2,  77,   81 },   /* alt 63, falsch 3055 (max Bucket 81), echt 0 (min Bucket -1) */
    { 0x5050,  2,  80,   80 },   /* alt 63, falsch 2192 (max Bucket 80), echt 0 (min Bucket -1) */
    { 0x5050,  2,  81,   80 },   /* alt 63, falsch 237 (max Bucket 80), echt 0 (min Bucket -1) */
    { 0x5050,  2,  82,   79 },   /* alt 63, falsch 4272 (max Bucket 79), echt 0 (min Bucket -1) */
    { 0x5050,  2,  83,   77 },   /* alt 63, falsch 2285 (max Bucket 77), echt 0 (min Bucket -1) */
    { 0x5050,  2,  84,   77 },   /* alt 63, falsch 1998 (max Bucket 77), echt 0 (min Bucket -1) */
    { 0x5050,  2,  85,   77 },   /* alt 64, falsch 1369 (max Bucket 77), echt 0 (min Bucket -1) */
    { 0x5050,  2,  86,   76 },   /* alt 63, falsch 1977 (max Bucket 76), echt 0 (min Bucket -1) */
    { 0x5050,  2,  87,   75 },   /* alt 63, falsch 1295 (max Bucket 75), echt 0 (min Bucket -1) */
    { 0x5050,  2,  88,   75 },   /* alt 63, falsch 2355 (max Bucket 75), echt 0 (min Bucket -1) */
    { 0x5050,  2,  89,   75 },   /* alt 63, falsch 4998 (max Bucket 75), echt 0 (min Bucket -1) */
    { 0x5050,  2,  90,   75 },   /* alt 67, falsch 793 (max Bucket 75), echt 0 (min Bucket -1) */
    { 0x5050,  2,  91,   74 },   /* alt 64, falsch 1308 (max Bucket 74), echt 0 (min Bucket -1) */
    { 0x5050,  2,  92,   75 },   /* alt 63, falsch 790 (max Bucket 75), echt 0 (min Bucket -1) */
    { 0x5050,  2,  93,   74 },   /* alt 63, falsch 9092 (max Bucket 74), echt 0 (min Bucket -1) */
    { 0x5050,  2,  94,   73 },   /* alt 63, falsch 826 (max Bucket 73), echt 0 (min Bucket -1) */
    { 0x5050,  2,  95,   72 },   /* alt 63, falsch 3055 (max Bucket 72), echt 0 (min Bucket -1) */
    { 0x5050,  2,  96,   72 },   /* alt 63, falsch 4884 (max Bucket 72), echt 0 (min Bucket -1) */
    { 0x5050,  2,  97,   72 },   /* alt 63, falsch 405 (max Bucket 72), echt 0 (min Bucket -1) */
    { 0x5050,  2,  98,   71 },   /* alt 63, falsch 2578 (max Bucket 71), echt 0 (min Bucket -1) */
    { 0x5050,  2,  99,   71 },   /* alt 63, falsch 1009 (max Bucket 71), echt 0 (min Bucket -1) */
    { 0x5050,  2, 100,   71 },   /* alt 63, falsch 2512 (max Bucket 71), echt 0 (min Bucket -1) */
    { 0x5050,  2, 101,   70 },   /* alt 63, falsch 1988 (max Bucket 70), echt 0 (min Bucket -1) */
    { 0x5050,  2, 102,   70 },   /* alt 63, falsch 2270 (max Bucket 70), echt 0 (min Bucket -1) */
    { 0x5050,  2, 103,   70 },   /* alt 63, falsch 178 (max Bucket 70), echt 0 (min Bucket -1) */
    { 0x5050,  2, 104,   70 },   /* alt 63, falsch 3024 (max Bucket 70), echt 0 (min Bucket -1) */
    { 0x5050,  3,  86,   73 },   /* alt 72, falsch 3065 (max Bucket 73), echt 0 (min Bucket -1) */
    { 0x5050,  3,  91,   72 },   /* alt 71, falsch 3011 (max Bucket 72), echt 0 (min Bucket -1) */
    { 0x5050,  3,  93,   71 },   /* alt 70, falsch 2866 (max Bucket 71), echt 0 (min Bucket -1) */
    { 0x5051,  0,   3,   58 },   /* alt 57, falsch 8262 (max Bucket 58), echt 0 (min Bucket -1) */
    { 0x5051,  0,   8,   58 },   /* alt 56, falsch 2161 (max Bucket 58), echt 0 (min Bucket -1) */
    { 0x5051,  0,   9,   58 },   /* alt 57, falsch 1667 (max Bucket 58), echt 0 (min Bucket -1) */
    { 0x5051,  0,  10,   58 },   /* alt 57, falsch 884 (max Bucket 58), echt 0 (min Bucket -1) */
    { 0x5051,  0,  16,   62 },   /* alt 0, falsch 28669 (max Bucket 62), echt 0 (min Bucket -1) */
    { 0x5051,  0,  17,   58 },   /* alt 0, falsch 6888 (max Bucket 58), echt 0 (min Bucket -1) */
    { 0x5051,  1,   0,  150 },   /* alt 86, falsch 116044 (max Bucket 150), echt 0 (min Bucket -1) */
    { 0x5051,  1,   1,  150 },   /* alt 86, falsch 11521 (max Bucket 150), echt 0 (min Bucket -1) */
    { 0x5051,  1,   2,  143 },   /* alt 86, falsch 7297 (max Bucket 143), echt 0 (min Bucket -1) */
    { 0x5051,  1,   3,  143 },   /* alt 86, falsch 2215 (max Bucket 143), echt 0 (min Bucket -1) */
    { 0x5051,  1,   4,  148 },   /* alt 86, falsch 2694 (max Bucket 148), echt 0 (min Bucket -1) */
    { 0x5051,  1,   6,   85 },   /* alt 68, falsch 147 (max Bucket 85), echt 0 (min Bucket -1) */
    { 0x5051,  1,   7,   80 },   /* alt 68, falsch 148 (max Bucket 80), echt 0 (min Bucket -1) */
    { 0x5051,  1,   8,   85 },   /* alt 68, falsch 22 (max Bucket 85), echt 0 (min Bucket -1) */
    { 0x5051,  1,   9,   77 },   /* alt 68, falsch 104 (max Bucket 77), echt 0 (min Bucket -1) */
    { 0x5051,  1,  10,   87 },   /* alt 68, falsch 74 (max Bucket 87), echt 0 (min Bucket -1) */
    { 0x5051,  1,  11,   83 },   /* alt 68, falsch 1367 (max Bucket 83), echt 213104 (min Bucket 50), echt teils nicht mehr verdeckt */
    { 0x5051,  1,  13,   78 },   /* alt 68, falsch 2638 (max Bucket 78), echt 13662 (min Bucket 53), echt teils nicht mehr verdeckt */
    { 0x5051,  1,  22,  150 },   /* alt 68, falsch 15495 (max Bucket 150), echt 35575 (min Bucket 114), echt teils nicht mehr verdeckt */
    { 0x5051,  1,  43,  142 },   /* alt 86, falsch 519 (max Bucket 142), echt 0 (min Bucket -1) */
    { 0x5051,  1,  45,  143 },   /* alt 86, falsch 1011 (max Bucket 143), echt 0 (min Bucket -1) */
    { 0x5051,  1,  47,  146 },   /* alt 86, falsch 1169 (max Bucket 146), echt 0 (min Bucket -1) */
    { 0x5051,  1,  56,  148 },   /* alt 68, falsch 264 (max Bucket 148), echt 711 (min Bucket 114), echt teils nicht mehr verdeckt */
    { 0x5051,  1,  85,   84 },   /* alt 68, falsch 2 (max Bucket 84), echt 0 (min Bucket -1) */
    { 0x5051,  1,  88,   87 },   /* alt 68, falsch 11 (max Bucket 87), echt 0 (min Bucket -1) */
    { 0x5051,  1,  90,   86 },   /* alt 68, falsch 53 (max Bucket 86), echt 0 (min Bucket -1) */
    { 0x5051,  1,  95,   83 },   /* alt 68, falsch 115 (max Bucket 83), echt 0 (min Bucket -1) */
    { 0x5051,  1,  97,   79 },   /* alt 68, falsch 53 (max Bucket 79), echt 0 (min Bucket -1) */
    { 0x5051,  1, 102,   77 },   /* alt 68, falsch 44 (max Bucket 77), echt 0 (min Bucket -1) */
    { 0x5051,  2,   0,   87 },   /* alt 63, falsch 175895 (max Bucket 87), echt 0 (min Bucket -1) */
    { 0x5051,  2,   1,   94 },   /* alt 63, falsch 93733 (max Bucket 94), echt 0 (min Bucket -1) */
    { 0x5051,  2,   2,   82 },   /* alt 63, falsch 64102 (max Bucket 82), echt 0 (min Bucket -1) */
    { 0x5051,  2,   3,   87 },   /* alt 63, falsch 49797 (max Bucket 87), echt 0 (min Bucket -1) */
    { 0x5051,  2,   4,   75 },   /* alt 63, falsch 35820 (max Bucket 75), echt 0 (min Bucket -1) */
    { 0x5051,  2,   5,   83 },   /* alt 63, falsch 25849 (max Bucket 83), echt 0 (min Bucket -1) */
    { 0x5051,  2,   6,   90 },   /* alt 63, falsch 26578 (max Bucket 90), echt 0 (min Bucket -1) */
    { 0x5051,  2,   7,   87 },   /* alt 63, falsch 24328 (max Bucket 87), echt 0 (min Bucket -1) */
    { 0x5051,  2,   8,   93 },   /* alt 63, falsch 23626 (max Bucket 93), echt 0 (min Bucket -1) */
    { 0x5051,  2,   9,   77 },   /* alt 63, falsch 12983 (max Bucket 77), echt 0 (min Bucket -1) */
    { 0x5051,  2,  10,   86 },   /* alt 63, falsch 12115 (max Bucket 86), echt 0 (min Bucket -1) */
    { 0x5051,  2,  11,   92 },   /* alt 63, falsch 12578 (max Bucket 92), echt 0 (min Bucket -1) */
    { 0x5051,  2,  12,   98 },   /* alt 63, falsch 12515 (max Bucket 98), echt 0 (min Bucket -1) */
    { 0x5051,  2,  13,   87 },   /* alt 63, falsch 10257 (max Bucket 87), echt 0 (min Bucket -1) */
    { 0x5051,  2,  14,   74 },   /* alt 63, falsch 6795 (max Bucket 74), echt 0 (min Bucket -1) */
    { 0x5051,  2,  15,   91 },   /* alt 67, falsch 9745 (max Bucket 91), echt 0 (min Bucket -1) */
    { 0x5051,  2,  16,   82 },   /* alt 63, falsch 7455 (max Bucket 82), echt 0 (min Bucket -1) */
    { 0x5051,  2,  17,   73 },   /* alt 63, falsch 6019 (max Bucket 73), echt 0 (min Bucket -1) */
    { 0x5051,  2,  18,   82 },   /* alt 63, falsch 7108 (max Bucket 82), echt 0 (min Bucket -1) */
    { 0x5051,  2,  19,   75 },   /* alt 63, falsch 5915 (max Bucket 75), echt 0 (min Bucket -1) */
    { 0x5051,  2,  20,   88 },   /* alt 63, falsch 7464 (max Bucket 88), echt 0 (min Bucket -1) */
    { 0x5051,  2,  21,   76 },   /* alt 63, falsch 5596 (max Bucket 76), echt 0 (min Bucket -1) */
    { 0x5051,  2,  22,   70 },   /* alt 63, falsch 4931 (max Bucket 70), echt 0 (min Bucket -1) */
    { 0x5051,  2,  23,   93 },   /* alt 63, falsch 7690 (max Bucket 93), echt 0 (min Bucket -1) */
    { 0x5051,  2,  24,   77 },   /* alt 63, falsch 5179 (max Bucket 77), echt 0 (min Bucket -1) */
    { 0x5051,  2,  25,   73 },   /* alt 63, falsch 4376 (max Bucket 73), echt 0 (min Bucket -1) */
    { 0x5051,  2,  26,   79 },   /* alt 63, falsch 199552 (max Bucket 79), echt 0 (min Bucket -1) */
    { 0x5051,  2,  27,   71 },   /* alt 63, falsch 62618 (max Bucket 71), echt 0 (min Bucket -1) */
    { 0x5051,  2,  28,   75 },   /* alt 63, falsch 42774 (max Bucket 75), echt 0 (min Bucket -1) */
    { 0x5051,  2,  29,   82 },   /* alt 63, falsch 27034 (max Bucket 82), echt 0 (min Bucket -1) */
    { 0x5051,  2,  30,   71 },   /* alt 63, falsch 13367 (max Bucket 71), echt 0 (min Bucket -1) */
    { 0x5051,  2,  31,   72 },   /* alt 63, falsch 9666 (max Bucket 72), echt 0 (min Bucket -1) */
    { 0x5051,  2,  32,   70 },   /* alt 63, falsch 5527 (max Bucket 70), echt 0 (min Bucket -1) */
    { 0x5051,  2,  33,   77 },   /* alt 63, falsch 5911 (max Bucket 77), echt 0 (min Bucket -1) */
    { 0x5051,  2,  34,   81 },   /* alt 63, falsch 5603 (max Bucket 81), echt 0 (min Bucket -1) */
    { 0x5051,  2,  35,   74 },   /* alt 63, falsch 5020 (max Bucket 74), echt 0 (min Bucket -1) */
    { 0x5051,  2,  36,   71 },   /* alt 63, falsch 4467 (max Bucket 71), echt 0 (min Bucket -1) */
    { 0x5051,  2,  37,   70 },   /* alt 63, falsch 3878 (max Bucket 70), echt 0 (min Bucket -1) */
    { 0x5051,  2,  38,   84 },   /* alt 63, falsch 4959 (max Bucket 84), echt 0 (min Bucket -1) */
    { 0x5051,  2,  39,   71 },   /* alt 63, falsch 3314 (max Bucket 71), echt 0 (min Bucket -1) */
    { 0x5051,  2,  40,   70 },   /* alt 63, falsch 2813 (max Bucket 70), echt 0 (min Bucket -1) */
    { 0x5051,  2,  55,   99 },   /* alt 63, falsch 10749 (max Bucket 99), echt 0 (min Bucket -1) */
    { 0x5051,  2,  56,   98 },   /* alt 64, falsch 330 (max Bucket 98), echt 0 (min Bucket -1) */
    { 0x5051,  2,  57,   97 },   /* alt 63, falsch 42211 (max Bucket 97), echt 0 (min Bucket -1) */
    { 0x5051,  2,  58,   93 },   /* alt 63, falsch 303 (max Bucket 93), echt 0 (min Bucket -1) */
    { 0x5051,  2,  59,   93 },   /* alt 63, falsch 20696 (max Bucket 93), echt 0 (min Bucket -1) */
    { 0x5051,  2,  60,   91 },   /* alt 64, falsch 823 (max Bucket 91), echt 0 (min Bucket -1) */
    { 0x5051,  2,  61,   91 },   /* alt 67, falsch 284 (max Bucket 91), echt 0 (min Bucket -1) */
    { 0x5051,  2,  62,   91 },   /* alt 68, falsch 2460 (max Bucket 91), echt 0 (min Bucket -1) */
    { 0x5051,  2,  63,   90 },   /* alt 63, falsch 11768 (max Bucket 90), echt 0 (min Bucket -1) */
    { 0x5051,  2,  65,   87 },   /* alt 63, falsch 6046 (max Bucket 87), echt 0 (min Bucket -1) */
    { 0x5051,  2,  66,   87 },   /* alt 63, falsch 7098 (max Bucket 87), echt 0 (min Bucket -1) */
    { 0x5051,  2,  67,   88 },   /* alt 67, falsch 1057 (max Bucket 88), echt 0 (min Bucket -1) */
    { 0x5051,  2,  68,   88 },   /* alt 63, falsch 1344 (max Bucket 88), echt 0 (min Bucket -1) */
    { 0x5051,  2,  69,   84 },   /* alt 63, falsch 240 (max Bucket 84), echt 0 (min Bucket -1) */
    { 0x5051,  2,  70,   84 },   /* alt 63, falsch 10019 (max Bucket 84), echt 0 (min Bucket -1) */
    { 0x5051,  2,  71,   83 },   /* alt 63, falsch 48658 (max Bucket 83), echt 0 (min Bucket -1) */
    { 0x5051,  2,  72,   83 },   /* alt 63, falsch 5287 (max Bucket 83), echt 0 (min Bucket -1) */
    { 0x5051,  2,  73,   82 },   /* alt 63, falsch 14429 (max Bucket 82), echt 0 (min Bucket -1) */
    { 0x5051,  2,  75,   82 },   /* alt 63, falsch 975 (max Bucket 82), echt 0 (min Bucket -1) */
    { 0x5051,  2,  77,   81 },   /* alt 63, falsch 3055 (max Bucket 81), echt 0 (min Bucket -1) */
    { 0x5051,  2,  80,   80 },   /* alt 63, falsch 2192 (max Bucket 80), echt 0 (min Bucket -1) */
    { 0x5051,  2,  81,   80 },   /* alt 63, falsch 237 (max Bucket 80), echt 0 (min Bucket -1) */
    { 0x5051,  2,  82,   79 },   /* alt 63, falsch 4272 (max Bucket 79), echt 0 (min Bucket -1) */
    { 0x5051,  2,  83,   77 },   /* alt 63, falsch 2285 (max Bucket 77), echt 0 (min Bucket -1) */
    { 0x5051,  2,  84,   77 },   /* alt 63, falsch 1998 (max Bucket 77), echt 0 (min Bucket -1) */
    { 0x5051,  2,  85,   77 },   /* alt 64, falsch 1369 (max Bucket 77), echt 0 (min Bucket -1) */
    { 0x5051,  2,  86,   76 },   /* alt 63, falsch 1977 (max Bucket 76), echt 0 (min Bucket -1) */
    { 0x5051,  2,  87,   75 },   /* alt 63, falsch 1295 (max Bucket 75), echt 0 (min Bucket -1) */
    { 0x5051,  2,  88,   75 },   /* alt 63, falsch 2355 (max Bucket 75), echt 0 (min Bucket -1) */
    { 0x5051,  2,  89,   75 },   /* alt 63, falsch 4998 (max Bucket 75), echt 0 (min Bucket -1) */
    { 0x5051,  2,  90,   75 },   /* alt 67, falsch 793 (max Bucket 75), echt 0 (min Bucket -1) */
    { 0x5051,  2,  91,   74 },   /* alt 64, falsch 1308 (max Bucket 74), echt 0 (min Bucket -1) */
    { 0x5051,  2,  92,   75 },   /* alt 63, falsch 790 (max Bucket 75), echt 0 (min Bucket -1) */
    { 0x5051,  2,  93,   74 },   /* alt 63, falsch 9092 (max Bucket 74), echt 0 (min Bucket -1) */
    { 0x5051,  2,  94,   73 },   /* alt 63, falsch 826 (max Bucket 73), echt 0 (min Bucket -1) */
    { 0x5051,  2,  95,   72 },   /* alt 63, falsch 3055 (max Bucket 72), echt 0 (min Bucket -1) */
    { 0x5051,  2,  96,   72 },   /* alt 63, falsch 4884 (max Bucket 72), echt 0 (min Bucket -1) */
    { 0x5051,  2,  97,   72 },   /* alt 63, falsch 405 (max Bucket 72), echt 0 (min Bucket -1) */
    { 0x5051,  2,  98,   71 },   /* alt 63, falsch 2578 (max Bucket 71), echt 0 (min Bucket -1) */
    { 0x5051,  2,  99,   71 },   /* alt 63, falsch 1009 (max Bucket 71), echt 0 (min Bucket -1) */
    { 0x5051,  2, 100,   71 },   /* alt 63, falsch 2512 (max Bucket 71), echt 0 (min Bucket -1) */
    { 0x5051,  2, 101,   70 },   /* alt 63, falsch 1988 (max Bucket 70), echt 0 (min Bucket -1) */
    { 0x5051,  2, 102,   70 },   /* alt 63, falsch 2270 (max Bucket 70), echt 0 (min Bucket -1) */
    { 0x5051,  2, 103,   70 },   /* alt 63, falsch 178 (max Bucket 70), echt 0 (min Bucket -1) */
    { 0x5051,  2, 104,   70 },   /* alt 63, falsch 3024 (max Bucket 70), echt 0 (min Bucket -1) */
    { 0x5051,  3,  86,   73 },   /* alt 72, falsch 3065 (max Bucket 73), echt 0 (min Bucket -1) */
    { 0x5051,  3,  91,   72 },   /* alt 71, falsch 3011 (max Bucket 72), echt 0 (min Bucket -1) */
    { 0x5051,  3,  93,   71 },   /* alt 70, falsch 2866 (max Bucket 71), echt 0 (min Bucket -1) */
    { 0x5090, 11,   0,   91 },   /* alt 58, falsch 307 (max Bucket 91), echt 18717 (min Bucket 61), echt teils nicht mehr verdeckt */
    { 0x5090, 11,   5,   91 },   /* alt 58, falsch 309 (max Bucket 91), echt 0 (min Bucket -1) */
    { 0x5090, 11,   7,   71 },   /* alt 69, falsch 243835 (max Bucket 71), echt 0 (min Bucket -1) */
    { 0x5090, 11,  15,   74 },   /* alt 68, falsch 219571 (max Bucket 74), echt 0 (min Bucket -1) */
    { 0x5090, 11,  16,   67 },   /* alt 65, falsch 33896 (max Bucket 67), echt 0 (min Bucket -1) */
    { 0x5090, 11,  17,   72 },   /* alt 70, falsch 29783 (max Bucket 72), echt 0 (min Bucket -1) */
    { 0x5090, 11,  19,   70 },   /* alt 69, falsch 11469 (max Bucket 70), echt 0 (min Bucket -1) */
    { 0x5090, 11,  21,   75 },   /* alt 73, falsch 5936 (max Bucket 75), echt 0 (min Bucket -1) */
    { 0x5090, 11,  24,   97 },   /* alt 58, falsch 318 (max Bucket 97), echt 41 (min Bucket 90), echt teils nicht mehr verdeckt */
    { 0x5090, 11,  25,   97 },   /* alt 58, falsch 348 (max Bucket 97), echt 297 (min Bucket 89), echt teils nicht mehr verdeckt */
    { 0x5090, 11,  32,   90 },   /* alt 58, falsch 310 (max Bucket 90), echt 447 (min Bucket 87), echt teils nicht mehr verdeckt */
    { 0x5090, 11,  35,   95 },   /* alt 58, falsch 312 (max Bucket 95), echt 29 (min Bucket 93), echt teils nicht mehr verdeckt */
    { 0x5090, 11,  55,   95 },   /* alt 58, falsch 310 (max Bucket 95), echt 19 (min Bucket 93), echt teils nicht mehr verdeckt */
    { 0x5090, 11,  82,   97 },   /* alt 58, falsch 312 (max Bucket 97), echt 21 (min Bucket 94), echt teils nicht mehr verdeckt */
    { 0x5090, 11,  87,   74 },   /* alt 73, falsch 2854 (max Bucket 74), echt 0 (min Bucket -1) */
    { 0x5090, 11,  89,   95 },   /* alt 58, falsch 308 (max Bucket 95), echt 5 (min Bucket 91), echt teils nicht mehr verdeckt */
    { 0x5090, 11,  94,   76 },   /* alt 74, falsch 1080 (max Bucket 76), echt 0 (min Bucket -1) */
    { 0x5090, 11,  96,   73 },   /* alt 72, falsch 1783 (max Bucket 73), echt 0 (min Bucket -1) */
    { 0x5090, 12,  19,   77 },   /* alt 76, falsch 128540 (max Bucket 77), echt 0 (min Bucket -1) */
    { 0x5090, 12,  22,   80 },   /* alt 79, falsch 38143 (max Bucket 80), echt 0 (min Bucket -1) */
    { 0x5090, 12,  31,  124 },   /* alt 118, falsch 261208 (max Bucket 124), echt 0 (min Bucket -1) */
    { 0x5090, 12,  44,   79 },   /* alt 77, falsch 39710 (max Bucket 79), echt 0 (min Bucket -1) */
    { 0x5090, 12,  47,   88 },   /* alt 81, falsch 8667 (max Bucket 88), echt 0 (min Bucket -1) */
    { 0x5090, 12,  51,  195 },   /* alt 192, falsch 323145 (max Bucket 195), echt 7878 (min Bucket 257) */
    { 0x5090, 12,  53,  217 },   /* alt 207, falsch 49882 (max Bucket 217), echt 6664 (min Bucket 239) */
    { 0x5090, 12,  63,   66 },   /* alt 0, falsch 2283 (max Bucket 66), echt 0 (min Bucket -1) */
    { 0x5090, 12,  69,  209 },   /* alt 207, falsch 3023 (max Bucket 209), echt 227 (min Bucket 239) */
    { 0x5090, 12,  70,  204 },   /* alt 202, falsch 3813 (max Bucket 204), echt 203 (min Bucket 254) */
    { 0x5090, 12,  71,  200 },   /* alt 197, falsch 4738 (max Bucket 200), echt 185 (min Bucket 256) */
    { 0x5090, 12,  85,   82 },   /* alt 80, falsch 8967 (max Bucket 82), echt 0 (min Bucket -1) */
    { 0x5090, 12, 104,   78 },   /* alt 77, falsch 6193 (max Bucket 78), echt 0 (min Bucket -1) */
    { 0x5090, 13,   0,  143 },   /* alt 128, falsch 5551 (max Bucket 143), echt 63593 (min Bucket 134), echt teils nicht mehr verdeckt */
    { 0x5090, 13,   6,  149 },   /* alt 130, falsch 3128 (max Bucket 149), echt 4297 (min Bucket 138), echt teils nicht mehr verdeckt */
    { 0x5090, 13,  10,  150 },   /* alt 130, falsch 1973 (max Bucket 150), echt 1059 (min Bucket 144), echt teils nicht mehr verdeckt */
    { 0x5090, 13,  12,  153 },   /* alt 127, falsch 37893 (max Bucket 153), echt 82182 (min Bucket 138), echt teils nicht mehr verdeckt */
    { 0x5090, 13,  14,  157 },   /* alt 131, falsch 6080 (max Bucket 157), echt 760 (min Bucket 146), echt teils nicht mehr verdeckt */
    { 0x5090, 13,  18,  154 },   /* alt 130, falsch 2283 (max Bucket 154), echt 1008 (min Bucket 148), echt teils nicht mehr verdeckt */
    { 0x5090, 13,  20,   65 },   /* alt 61, falsch 211843 (max Bucket 65), echt 0 (min Bucket -1) */
    { 0x5090, 13,  21,   64 },   /* alt 61, falsch 124525 (max Bucket 64), echt 0 (min Bucket -1) */
    { 0x5090, 13,  25,   63 },   /* alt 61, falsch 24048 (max Bucket 63), echt 0 (min Bucket -1) */
    { 0x5090, 13,  26,   65 },   /* alt 61, falsch 16111 (max Bucket 65), echt 0 (min Bucket -1) */
    { 0x5090, 13,  29,   63 },   /* alt 62, falsch 10210 (max Bucket 63), echt 0 (min Bucket -1) */
    { 0x5090, 13,  42,  168 },   /* alt 140, falsch 476428 (max Bucket 168), echt 34458 (min Bucket 154), echt teils nicht mehr verdeckt */
    { 0x5090, 13,  44,  174 },   /* alt 140, falsch 997117 (max Bucket 174), echt 40627 (min Bucket 158), echt teils nicht mehr verdeckt */
    { 0x5090, 13,  57,   11 },   /* alt 0, falsch 315 (max Bucket 11), echt 0 (min Bucket -1) */
    { 0x5090, 13,  61,  151 },   /* alt 130, falsch 814 (max Bucket 151), echt 333 (min Bucket 191) */
    { 0x5090, 13,  80,  157 },   /* alt 131, falsch 2265 (max Bucket 157), echt 427 (min Bucket 201) */
    { 0x5090, 13,  97,   62 },   /* alt 61, falsch 47971 (max Bucket 62), echt 0 (min Bucket -1) */
    { 0x5090, 14,  54,  173 },   /* alt 128, falsch 10651 (max Bucket 173), echt 66766 (min Bucket 109), echt teils nicht mehr verdeckt */
    { 0x5090, 14,  56,  165 },   /* alt 136, falsch 5440 (max Bucket 165), echt 3213 (min Bucket 118), echt teils nicht mehr verdeckt */
    { 0x5090, 14,  58,  174 },   /* alt 130, falsch 7535 (max Bucket 174), echt 3876 (min Bucket 107), echt teils nicht mehr verdeckt */
    { 0x5090, 14,  95,  173 },   /* alt 132, falsch 7232 (max Bucket 173), echt 0 (min Bucket -1) */
    { 0x5090, 14,  99,  162 },   /* alt 138, falsch 3429 (max Bucket 162), echt 245 (min Bucket 123), echt teils nicht mehr verdeckt */
    { 0x5091, 11,   0,   91 },   /* alt 58, falsch 307 (max Bucket 91), echt 18717 (min Bucket 61), echt teils nicht mehr verdeckt */
    { 0x5091, 11,   5,   91 },   /* alt 58, falsch 309 (max Bucket 91), echt 0 (min Bucket -1) */
    { 0x5091, 11,   7,   71 },   /* alt 69, falsch 243835 (max Bucket 71), echt 0 (min Bucket -1) */
    { 0x5091, 11,  15,   74 },   /* alt 68, falsch 219571 (max Bucket 74), echt 0 (min Bucket -1) */
    { 0x5091, 11,  16,   67 },   /* alt 65, falsch 33896 (max Bucket 67), echt 0 (min Bucket -1) */
    { 0x5091, 11,  17,   72 },   /* alt 70, falsch 29783 (max Bucket 72), echt 0 (min Bucket -1) */
    { 0x5091, 11,  19,   70 },   /* alt 69, falsch 11469 (max Bucket 70), echt 0 (min Bucket -1) */
    { 0x5091, 11,  21,   75 },   /* alt 73, falsch 5936 (max Bucket 75), echt 0 (min Bucket -1) */
    { 0x5091, 11,  24,   97 },   /* alt 58, falsch 318 (max Bucket 97), echt 41 (min Bucket 90), echt teils nicht mehr verdeckt */
    { 0x5091, 11,  25,   97 },   /* alt 58, falsch 348 (max Bucket 97), echt 297 (min Bucket 89), echt teils nicht mehr verdeckt */
    { 0x5091, 11,  32,   90 },   /* alt 58, falsch 310 (max Bucket 90), echt 447 (min Bucket 87), echt teils nicht mehr verdeckt */
    { 0x5091, 11,  35,   95 },   /* alt 58, falsch 312 (max Bucket 95), echt 29 (min Bucket 93), echt teils nicht mehr verdeckt */
    { 0x5091, 11,  55,   95 },   /* alt 58, falsch 310 (max Bucket 95), echt 19 (min Bucket 93), echt teils nicht mehr verdeckt */
    { 0x5091, 11,  82,   97 },   /* alt 58, falsch 312 (max Bucket 97), echt 21 (min Bucket 94), echt teils nicht mehr verdeckt */
    { 0x5091, 11,  87,   74 },   /* alt 73, falsch 2854 (max Bucket 74), echt 0 (min Bucket -1) */
    { 0x5091, 11,  89,   95 },   /* alt 58, falsch 308 (max Bucket 95), echt 5 (min Bucket 91), echt teils nicht mehr verdeckt */
    { 0x5091, 11,  94,   76 },   /* alt 74, falsch 1080 (max Bucket 76), echt 0 (min Bucket -1) */
    { 0x5091, 11,  96,   73 },   /* alt 72, falsch 1783 (max Bucket 73), echt 0 (min Bucket -1) */
    { 0x5091, 12,  19,   77 },   /* alt 76, falsch 128540 (max Bucket 77), echt 0 (min Bucket -1) */
    { 0x5091, 12,  22,   80 },   /* alt 79, falsch 38143 (max Bucket 80), echt 0 (min Bucket -1) */
    { 0x5091, 12,  31,  124 },   /* alt 118, falsch 261208 (max Bucket 124), echt 0 (min Bucket -1) */
    { 0x5091, 12,  44,   79 },   /* alt 77, falsch 39710 (max Bucket 79), echt 0 (min Bucket -1) */
    { 0x5091, 12,  47,   88 },   /* alt 81, falsch 8667 (max Bucket 88), echt 0 (min Bucket -1) */
    { 0x5091, 12,  51,  195 },   /* alt 192, falsch 323145 (max Bucket 195), echt 7878 (min Bucket 257) */
    { 0x5091, 12,  53,  217 },   /* alt 207, falsch 49882 (max Bucket 217), echt 6664 (min Bucket 239) */
    { 0x5091, 12,  63,   66 },   /* alt 0, falsch 2283 (max Bucket 66), echt 0 (min Bucket -1) */
    { 0x5091, 12,  69,  209 },   /* alt 207, falsch 3023 (max Bucket 209), echt 227 (min Bucket 239) */
    { 0x5091, 12,  70,  204 },   /* alt 202, falsch 3813 (max Bucket 204), echt 203 (min Bucket 254) */
    { 0x5091, 12,  71,  200 },   /* alt 197, falsch 4738 (max Bucket 200), echt 185 (min Bucket 256) */
    { 0x5091, 12,  85,   82 },   /* alt 80, falsch 8967 (max Bucket 82), echt 0 (min Bucket -1) */
    { 0x5091, 12, 104,   78 },   /* alt 77, falsch 6193 (max Bucket 78), echt 0 (min Bucket -1) */
    { 0x5091, 13,   0,  143 },   /* alt 128, falsch 5551 (max Bucket 143), echt 63593 (min Bucket 134), echt teils nicht mehr verdeckt */
    { 0x5091, 13,   6,  149 },   /* alt 130, falsch 3128 (max Bucket 149), echt 4297 (min Bucket 138), echt teils nicht mehr verdeckt */
    { 0x5091, 13,  10,  150 },   /* alt 130, falsch 1973 (max Bucket 150), echt 1059 (min Bucket 144), echt teils nicht mehr verdeckt */
    { 0x5091, 13,  12,  153 },   /* alt 127, falsch 37893 (max Bucket 153), echt 82182 (min Bucket 138), echt teils nicht mehr verdeckt */
    { 0x5091, 13,  14,  157 },   /* alt 131, falsch 6080 (max Bucket 157), echt 760 (min Bucket 146), echt teils nicht mehr verdeckt */
    { 0x5091, 13,  18,  154 },   /* alt 130, falsch 2283 (max Bucket 154), echt 1008 (min Bucket 148), echt teils nicht mehr verdeckt */
    { 0x5091, 13,  20,   65 },   /* alt 61, falsch 211843 (max Bucket 65), echt 0 (min Bucket -1) */
    { 0x5091, 13,  21,   64 },   /* alt 61, falsch 124525 (max Bucket 64), echt 0 (min Bucket -1) */
    { 0x5091, 13,  25,   63 },   /* alt 61, falsch 24048 (max Bucket 63), echt 0 (min Bucket -1) */
    { 0x5091, 13,  26,   65 },   /* alt 61, falsch 16111 (max Bucket 65), echt 0 (min Bucket -1) */
    { 0x5091, 13,  29,   63 },   /* alt 62, falsch 10210 (max Bucket 63), echt 0 (min Bucket -1) */
    { 0x5091, 13,  42,  168 },   /* alt 140, falsch 476428 (max Bucket 168), echt 34458 (min Bucket 154), echt teils nicht mehr verdeckt */
    { 0x5091, 13,  44,  174 },   /* alt 140, falsch 997117 (max Bucket 174), echt 40627 (min Bucket 158), echt teils nicht mehr verdeckt */
    { 0x5091, 13,  57,   11 },   /* alt 0, falsch 315 (max Bucket 11), echt 0 (min Bucket -1) */
    { 0x5091, 13,  61,  151 },   /* alt 130, falsch 814 (max Bucket 151), echt 333 (min Bucket 191) */
    { 0x5091, 13,  80,  157 },   /* alt 131, falsch 2265 (max Bucket 157), echt 427 (min Bucket 201) */
    { 0x5091, 13,  97,   62 },   /* alt 61, falsch 47971 (max Bucket 62), echt 0 (min Bucket -1) */
    { 0x5091, 14,  54,  173 },   /* alt 128, falsch 10651 (max Bucket 173), echt 66766 (min Bucket 109), echt teils nicht mehr verdeckt */
    { 0x5091, 14,  56,  165 },   /* alt 136, falsch 5440 (max Bucket 165), echt 3213 (min Bucket 118), echt teils nicht mehr verdeckt */
    { 0x5091, 14,  58,  174 },   /* alt 130, falsch 7535 (max Bucket 174), echt 3876 (min Bucket 107), echt teils nicht mehr verdeckt */
    { 0x5091, 14,  95,  173 },   /* alt 132, falsch 7232 (max Bucket 173), echt 0 (min Bucket -1) */
    { 0x5091, 14,  99,  162 },   /* alt 138, falsch 3429 (max Bucket 162), echt 245 (min Bucket 123), echt teils nicht mehr verdeckt */
    { 0x50B0,  1,  32,   75 },   /* alt 0, falsch 328023 (max Bucket 75), echt 0 (min Bucket -1) */
    { 0x50B0,  1,  33,   49 },   /* alt 0, falsch 5215 (max Bucket 49), echt 0 (min Bucket -1) */
    { 0x50B0,  1,  39,   53 },   /* alt 0, falsch 6859 (max Bucket 53), echt 0 (min Bucket -1) */
    { 0x50B0,  1,  48,   59 },   /* alt 0, falsch 10374 (max Bucket 59), echt 0 (min Bucket -1) */
    { 0x50B0,  1,  49,   99 },   /* alt 0, falsch 67020 (max Bucket 99), echt 0 (min Bucket -1) */
    { 0x50B0,  2,  61,   96 },   /* alt 0, falsch 65893 (max Bucket 96), echt 0 (min Bucket -1) */
    { 0x50B0,  2,  87,  200 },   /* alt 0, falsch 103393 (max Bucket 200), echt 0 (min Bucket -1) */
    { 0x50B0,  3,  15,  111 },   /* alt 103, falsch 406176 (max Bucket 111), echt 0 (min Bucket -1) */
    { 0x50B0,  3,  17,  114 },   /* alt 107, falsch 29840 (max Bucket 114), echt 0 (min Bucket -1) */
    { 0x50B0,  3,  20,  167 },   /* alt 159, falsch 385551 (max Bucket 167), echt 0 (min Bucket -1) */
    { 0x50B0,  3,  21,  166 },   /* alt 159, falsch 107051 (max Bucket 166), echt 0 (min Bucket -1) */
    { 0x50B0,  3,  23,  166 },   /* alt 159, falsch 38182 (max Bucket 166), echt 0 (min Bucket -1) */
    { 0x50B0,  3,  24,  166 },   /* alt 159, falsch 24109 (max Bucket 166), echt 0 (min Bucket -1) */
    { 0x50B0,  3,  25,  162 },   /* alt 157, falsch 15873 (max Bucket 162), echt 0 (min Bucket -1) */
    { 0x50B0,  3,  26,  166 },   /* alt 156, falsch 13762 (max Bucket 166), echt 0 (min Bucket -1) */
    { 0x50B0,  3,  29,  114 },   /* alt 103, falsch 641257 (max Bucket 114), echt 0 (min Bucket -1) */
    { 0x50B0,  3,  30,  117 },   /* alt 108, falsch 67782 (max Bucket 117), echt 0 (min Bucket -1) */
    { 0x50B0,  3,  31,  112 },   /* alt 103, falsch 24058 (max Bucket 112), echt 0 (min Bucket -1) */
    { 0x50B0,  3,  32,  108 },   /* alt 102, falsch 21338 (max Bucket 108), echt 0 (min Bucket -1) */
    { 0x50B0,  3,  48,    5 },   /* alt 0, falsch 2493 (max Bucket 5), echt 0 (min Bucket -1) */
    { 0x50B0,  3,  59,    5 },   /* alt 0, falsch 1939 (max Bucket 5), echt 0 (min Bucket -1) */
    { 0x50B0,  3,  63,    8 },   /* alt 0, falsch 12249 (max Bucket 8), echt 0 (min Bucket -1) */
    { 0x50B0,  3,  73,    9 },   /* alt 0, falsch 1853 (max Bucket 9), echt 0 (min Bucket -1) */
    { 0x50B0,  3,  78,  113 },   /* alt 105, falsch 4273 (max Bucket 113), echt 0 (min Bucket -1) */
    { 0x50B0,  3,  79,  167 },   /* alt 159, falsch 11478 (max Bucket 167), echt 0 (min Bucket -1) */
    { 0x50B0,  3,  80,  166 },   /* alt 162, falsch 3383 (max Bucket 166), echt 0 (min Bucket -1) */
    { 0x50B0,  3,  81,  166 },   /* alt 156, falsch 42042 (max Bucket 166), echt 0 (min Bucket -1) */
    { 0x50B0,  3,  89,  114 },   /* alt 105, falsch 2678 (max Bucket 114), echt 0 (min Bucket -1) */
    { 0x50B0,  3,  91,  106 },   /* alt 101, falsch 3980 (max Bucket 106), echt 0 (min Bucket -1) */
    { 0x50B0,  3,  92,  111 },   /* alt 103, falsch 3642 (max Bucket 111), echt 0 (min Bucket -1) */
    { 0x50B0,  3,  94,  115 },   /* alt 108, falsch 2362 (max Bucket 115), echt 0 (min Bucket -1) */
    { 0x50B0,  3,  98,  166 },   /* alt 159, falsch 23408 (max Bucket 166), echt 0 (min Bucket -1) */
    { 0x50B0,  3, 103,  111 },   /* alt 104, falsch 7404 (max Bucket 111), echt 0 (min Bucket -1) */
    { 0x50B0,  3, 104,  106 },   /* alt 100, falsch 6321 (max Bucket 106), echt 0 (min Bucket -1) */
    { 0x50B1,  1,  32,   75 },   /* alt 0, falsch 328023 (max Bucket 75), echt 0 (min Bucket -1) */
    { 0x50B1,  1,  33,   49 },   /* alt 0, falsch 5215 (max Bucket 49), echt 0 (min Bucket -1) */
    { 0x50B1,  1,  39,   53 },   /* alt 0, falsch 6859 (max Bucket 53), echt 0 (min Bucket -1) */
    { 0x50B1,  1,  48,   59 },   /* alt 0, falsch 10374 (max Bucket 59), echt 0 (min Bucket -1) */
    { 0x50B1,  1,  49,   99 },   /* alt 0, falsch 67020 (max Bucket 99), echt 0 (min Bucket -1) */
    { 0x50B1,  2,  61,   96 },   /* alt 0, falsch 65893 (max Bucket 96), echt 0 (min Bucket -1) */
    { 0x50B1,  2,  87,  200 },   /* alt 0, falsch 103393 (max Bucket 200), echt 0 (min Bucket -1) */
    { 0x50B1,  3,  15,  111 },   /* alt 103, falsch 406176 (max Bucket 111), echt 0 (min Bucket -1) */
    { 0x50B1,  3,  17,  114 },   /* alt 107, falsch 29840 (max Bucket 114), echt 0 (min Bucket -1) */
    { 0x50B1,  3,  20,  167 },   /* alt 159, falsch 385551 (max Bucket 167), echt 0 (min Bucket -1) */
    { 0x50B1,  3,  21,  166 },   /* alt 159, falsch 107051 (max Bucket 166), echt 0 (min Bucket -1) */
    { 0x50B1,  3,  23,  166 },   /* alt 159, falsch 38182 (max Bucket 166), echt 0 (min Bucket -1) */
    { 0x50B1,  3,  24,  166 },   /* alt 159, falsch 24109 (max Bucket 166), echt 0 (min Bucket -1) */
    { 0x50B1,  3,  25,  162 },   /* alt 157, falsch 15873 (max Bucket 162), echt 0 (min Bucket -1) */
    { 0x50B1,  3,  26,  166 },   /* alt 156, falsch 13762 (max Bucket 166), echt 0 (min Bucket -1) */
    { 0x50B1,  3,  29,  114 },   /* alt 103, falsch 641257 (max Bucket 114), echt 0 (min Bucket -1) */
    { 0x50B1,  3,  30,  117 },   /* alt 108, falsch 67782 (max Bucket 117), echt 0 (min Bucket -1) */
    { 0x50B1,  3,  31,  112 },   /* alt 103, falsch 24058 (max Bucket 112), echt 0 (min Bucket -1) */
    { 0x50B1,  3,  32,  108 },   /* alt 102, falsch 21338 (max Bucket 108), echt 0 (min Bucket -1) */
    { 0x50B1,  3,  48,    5 },   /* alt 0, falsch 2493 (max Bucket 5), echt 0 (min Bucket -1) */
    { 0x50B1,  3,  59,    5 },   /* alt 0, falsch 1939 (max Bucket 5), echt 0 (min Bucket -1) */
    { 0x50B1,  3,  63,    8 },   /* alt 0, falsch 12249 (max Bucket 8), echt 0 (min Bucket -1) */
    { 0x50B1,  3,  73,    9 },   /* alt 0, falsch 1853 (max Bucket 9), echt 0 (min Bucket -1) */
    { 0x50B1,  3,  78,  113 },   /* alt 105, falsch 4273 (max Bucket 113), echt 0 (min Bucket -1) */
    { 0x50B1,  3,  79,  167 },   /* alt 159, falsch 11478 (max Bucket 167), echt 0 (min Bucket -1) */
    { 0x50B1,  3,  80,  166 },   /* alt 162, falsch 3383 (max Bucket 166), echt 0 (min Bucket -1) */
    { 0x50B1,  3,  81,  166 },   /* alt 156, falsch 42042 (max Bucket 166), echt 0 (min Bucket -1) */
    { 0x50B1,  3,  89,  114 },   /* alt 105, falsch 2678 (max Bucket 114), echt 0 (min Bucket -1) */
    { 0x50B1,  3,  91,  106 },   /* alt 101, falsch 3980 (max Bucket 106), echt 0 (min Bucket -1) */
    { 0x50B1,  3,  92,  111 },   /* alt 103, falsch 3642 (max Bucket 111), echt 0 (min Bucket -1) */
    { 0x50B1,  3,  94,  115 },   /* alt 108, falsch 2362 (max Bucket 115), echt 0 (min Bucket -1) */
    { 0x50B1,  3,  98,  166 },   /* alt 159, falsch 23408 (max Bucket 166), echt 0 (min Bucket -1) */
    { 0x50B1,  3, 103,  111 },   /* alt 104, falsch 7404 (max Bucket 111), echt 0 (min Bucket -1) */
    { 0x50B1,  3, 104,  106 },   /* alt 100, falsch 6321 (max Bucket 106), echt 0 (min Bucket -1) */
    { 0x5100,  2,  12,   86 },   /* alt 75, falsch 934035 (max Bucket 86), echt 228586 (min Bucket 128) */
    { 0x5100,  2,  13,   72 },   /* alt 69, falsch 124759 (max Bucket 72), echt 0 (min Bucket -1) */
    { 0x5100,  2,  16,   88 },   /* alt 84, falsch 33829 (max Bucket 88), echt 27830 (min Bucket 127) */
    { 0x5100,  3,   3,  139 },   /* alt 114, falsch 700228 (max Bucket 139), echt 83588 (min Bucket 257) */
    { 0x5100,  3,   4,  118 },   /* alt 107, falsch 229085 (max Bucket 118), echt 0 (min Bucket -1) */
    { 0x5100,  3,   5,  139 },   /* alt 134, falsch 126599 (max Bucket 139), echt 0 (min Bucket -1) */
    { 0x5100,  3,   6,  100 },   /* alt 99, falsch 51159 (max Bucket 100), echt 0 (min Bucket -1) */
    { 0x5100,  3,   7,  119 },   /* alt 110, falsch 35532 (max Bucket 119), echt 0 (min Bucket -1) */
    { 0x5100,  3,   8,  104 },   /* alt 100, falsch 24522 (max Bucket 104), echt 0 (min Bucket -1) */
    { 0x5100,  3,  15,  192 },   /* alt 169, falsch 425583 (max Bucket 192), echt 160441 (min Bucket 246) */
    { 0x5100,  3,  16,  191 },   /* alt 169, falsch 127456 (max Bucket 191), echt 11403 (min Bucket 246) */
    { 0x5100,  3,  17,  167 },   /* alt 163, falsch 40454 (max Bucket 167), echt 0 (min Bucket -1) */
    { 0x5100,  3,  18,  192 },   /* alt 179, falsch 35922 (max Bucket 192), echt 13243 (min Bucket 246) */
    { 0x5100,  3,  22,   21 },   /* alt 0, falsch 8229 (max Bucket 21), echt 0 (min Bucket -1) */
    { 0x5100,  3,  23,   21 },   /* alt 0, falsch 11539 (max Bucket 21), echt 0 (min Bucket -1) */
    { 0x5100,  4,   0,  100 },   /* alt 77, falsch 652948 (max Bucket 100), echt 0 (min Bucket -1) */
    { 0x5100,  4,   1,   86 },   /* alt 76, falsch 87004 (max Bucket 86), echt 0 (min Bucket -1) */
    { 0x5100,  4,   2,  104 },   /* alt 90, falsch 62518 (max Bucket 104), echt 0 (min Bucket -1) */
    { 0x5100,  4,   4,  100 },   /* alt 90, falsch 28920 (max Bucket 100), echt 0 (min Bucket -1) */
    { 0x5100,  4,   5,   92 },   /* alt 80, falsch 21045 (max Bucket 92), echt 0 (min Bucket -1) */
    { 0x5100,  4,   7,   96 },   /* alt 83, falsch 266 (max Bucket 96), echt 0 (min Bucket -1) */
    { 0x5100,  4,   9,  101 },   /* alt 91, falsch 9339 (max Bucket 101), echt 0 (min Bucket -1) */
    { 0x5100,  4,  10,   83 },   /* alt 76, falsch 8974 (max Bucket 83), echt 0 (min Bucket -1) */
    { 0x5100,  4,  11,  104 },   /* alt 93, falsch 9776 (max Bucket 104), echt 0 (min Bucket -1) */
    { 0x5100,  4,  12,   89 },   /* alt 78, falsch 6206 (max Bucket 89), echt 0 (min Bucket -1) */
    { 0x5100,  4,  14,   95 },   /* alt 84, falsch 5163 (max Bucket 95), echt 0 (min Bucket -1) */
    { 0x5100,  4,  30,  157 },   /* alt 112, falsch 380365 (max Bucket 157), echt 89817 (min Bucket 180) */
    { 0x5100,  4,  31,  147 },   /* alt 115, falsch 186532 (max Bucket 147), echt 18 (min Bucket 187) */
    { 0x5100,  4,  32,  116 },   /* alt 98, falsch 119880 (max Bucket 116), echt 0 (min Bucket -1) */
    { 0x5100,  4,  33,  129 },   /* alt 109, falsch 48191 (max Bucket 129), echt 0 (min Bucket -1) */
    { 0x5100,  4,  34,  159 },   /* alt 135, falsch 57025 (max Bucket 159), echt 1279 (min Bucket 185) */
    { 0x5100,  4,  35,  112 },   /* alt 96, falsch 28363 (max Bucket 112), echt 0 (min Bucket -1) */
    { 0x5100,  4,  36,  155 },   /* alt 123, falsch 3102 (max Bucket 155), echt 7302 (min Bucket 180) */
    { 0x5100,  4,  37,  145 },   /* alt 123, falsch 16678 (max Bucket 145), echt 1398 (min Bucket 187) */
    { 0x5100,  4,  38,  122 },   /* alt 106, falsch 11753 (max Bucket 122), echt 21 (min Bucket 191) */
    { 0x5100,  4,  39,  138 },   /* alt 119, falsch 13619 (max Bucket 138), echt 0 (min Bucket -1) */
    { 0x5100,  4,  40,  123 },   /* alt 106, falsch 12055 (max Bucket 123), echt 0 (min Bucket -1) */
    { 0x5100,  4,  41,  110 },   /* alt 95, falsch 10818 (max Bucket 110), echt 0 (min Bucket -1) */
    { 0x5100,  4,  42,  114 },   /* alt 99, falsch 7959 (max Bucket 114), echt 0 (min Bucket -1) */
    { 0x5100,  4,  43,  155 },   /* alt 132, falsch 650 (max Bucket 155), echt 3706 (min Bucket 180) */
    { 0x5100,  4,  44,  104 },   /* alt 93, falsch 6148 (max Bucket 104), echt 0 (min Bucket -1) */
    { 0x5100,  4,  53,    7 },   /* alt 0, falsch 255 (max Bucket 7), echt 0 (min Bucket -1) */
    { 0x5100,  4,  54,    7 },   /* alt 0, falsch 255 (max Bucket 7), echt 0 (min Bucket -1) */
    { 0x5100,  4,  60,  155 },   /* alt 140, falsch 264 (max Bucket 155), echt 240 (min Bucket 180) */
    { 0x5100,  4,  61,  153 },   /* alt 138, falsch 809 (max Bucket 153), echt 439 (min Bucket 180) */
    { 0x5100,  4,  62,  141 },   /* alt 124, falsch 3556 (max Bucket 141), echt 204 (min Bucket 183) */
    { 0x5100,  4,  63,  134 },   /* alt 120, falsch 2975 (max Bucket 134), echt 150 (min Bucket 183) */
    { 0x5100,  4,  65,  119 },   /* alt 109, falsch 262 (max Bucket 119), echt 10 (min Bucket 186) */
    { 0x5100,  4,  66,  119 },   /* alt 107, falsch 2658 (max Bucket 119), echt 4 (min Bucket 186) */
    { 0x5100,  4,  67,  117 },   /* alt 103, falsch 4510 (max Bucket 117), echt 0 (min Bucket -1) */
    { 0x5100,  4,  68,  113 },   /* alt 101, falsch 5315 (max Bucket 113), echt 0 (min Bucket -1) */
    { 0x5100,  4,  70,  102 },   /* alt 91, falsch 1064 (max Bucket 102), echt 0 (min Bucket -1) */
    { 0x5100,  4,  71,   97 },   /* alt 86, falsch 2145 (max Bucket 97), echt 0 (min Bucket -1) */
    { 0x5100,  4,  73,   86 },   /* alt 77, falsch 5202 (max Bucket 86), echt 0 (min Bucket -1) */
    { 0x5100,  4,  80,  130 },   /* alt 115, falsch 4281 (max Bucket 130), echt 150 (min Bucket 190) */
    { 0x5100,  4,  81,  126 },   /* alt 113, falsch 3623 (max Bucket 126), echt 41 (min Bucket 190) */
    { 0x5100,  4,  82,  102 },   /* alt 92, falsch 5118 (max Bucket 102), echt 0 (min Bucket -1) */
    { 0x5100,  4,  86,  110 },   /* alt 97, falsch 2995 (max Bucket 110), echt 0 (min Bucket -1) */
    { 0x5100,  4,  87,  107 },   /* alt 94, falsch 9489 (max Bucket 107), echt 0 (min Bucket -1) */
    { 0x5100,  4,  88,  102 },   /* alt 90, falsch 5237 (max Bucket 102), echt 0 (min Bucket -1) */
    { 0x5100,  4,  89,  153 },   /* alt 135, falsch 2838 (max Bucket 153), echt 0 (min Bucket -1) */
    { 0x5100,  4,  90,  150 },   /* alt 131, falsch 4749 (max Bucket 150), echt 0 (min Bucket -1) */
    { 0x5100,  4,  92,  141 },   /* alt 127, falsch 1715 (max Bucket 141), echt 0 (min Bucket -1) */
    { 0x5100,  4,  95,  132 },   /* alt 116, falsch 4166 (max Bucket 132), echt 0 (min Bucket -1) */
    { 0x5100,  4,  98,  125 },   /* alt 110, falsch 3146 (max Bucket 125), echt 0 (min Bucket -1) */
    { 0x5100,  4, 100,  112 },   /* alt 99, falsch 3978 (max Bucket 112), echt 0 (min Bucket -1) */
    { 0x5100,  4, 101,  107 },   /* alt 95, falsch 2390 (max Bucket 107), echt 0 (min Bucket -1) */
    { 0x5100,  4, 102,   94 },   /* alt 83, falsch 2107 (max Bucket 94), echt 0 (min Bucket -1) */
    { 0x5100,  4, 103,   91 },   /* alt 81, falsch 2017 (max Bucket 91), echt 0 (min Bucket -1) */
    { 0x5100,  4, 104,   89 },   /* alt 79, falsch 2878 (max Bucket 89), echt 0 (min Bucket -1) */
    { 0x5100,  5,   3,  272 },   /* alt 179, falsch 1000110 (max Bucket 272), echt 83887 (min Bucket 301) */
    { 0x5100,  5,   4,  202 },   /* alt 179, falsch 183681 (max Bucket 202), echt 0 (min Bucket -1) */
    { 0x5100,  5,   5,  349 },   /* alt 248, falsch 81449 (max Bucket 349), echt 11579 (min Bucket 301), echt teils nicht mehr verdeckt */
    { 0x5100,  5,   6,  232 },   /* alt 195, falsch 39775 (max Bucket 232), echt 0 (min Bucket -1) */
    { 0x5100,  5,   7,    8 },   /* alt 0, falsch 2416 (max Bucket 8), echt 0 (min Bucket -1) */
    { 0x5100,  5,   8,    8 },   /* alt 0, falsch 2344 (max Bucket 8), echt 0 (min Bucket -1) */
    { 0x5100,  5,  10,  271 },   /* alt 245, falsch 1411 (max Bucket 271), echt 0 (min Bucket -1) */
    { 0x5100,  5,  11,  257 },   /* alt 232, falsch 1295 (max Bucket 257), echt 0 (min Bucket -1) */
    { 0x5100,  5,  12,  253 },   /* alt 226, falsch 2368 (max Bucket 253), echt 0 (min Bucket -1) */
    { 0x5100,  5,  13,  247 },   /* alt 220, falsch 3452 (max Bucket 247), echt 0 (min Bucket -1) */
    { 0x5100,  5,  14,  242 },   /* alt 214, falsch 4373 (max Bucket 242), echt 0 (min Bucket -1) */
    { 0x5100,  5,  15,  236 },   /* alt 209, falsch 5367 (max Bucket 236), echt 0 (min Bucket -1) */
    { 0x5100,  5,  16,  221 },   /* alt 199, falsch 2126 (max Bucket 221), echt 0 (min Bucket -1) */
    { 0x5100,  5,  17,  218 },   /* alt 195, falsch 2068 (max Bucket 218), echt 0 (min Bucket -1) */
    { 0x5100,  5,  18,  214 },   /* alt 191, falsch 2964 (max Bucket 214), echt 0 (min Bucket -1) */
    { 0x5100,  5,  19,  209 },   /* alt 186, falsch 3697 (max Bucket 209), echt 0 (min Bucket -1) */
    { 0x5100,  5,  20,  206 },   /* alt 183, falsch 4642 (max Bucket 206), echt 0 (min Bucket -1) */
    { 0x5100,  6,  15,  208 },   /* alt 134, falsch 1473896 (max Bucket 208), echt 200387 (min Bucket 225) */
    { 0x5100,  6,  16,  149 },   /* alt 115, falsch 211173 (max Bucket 149), echt 0 (min Bucket -1) */
    { 0x5100,  6,  17,  128 },   /* alt 108, falsch 58527 (max Bucket 128), echt 0 (min Bucket -1) */
    { 0x5100,  6,  18,  174 },   /* alt 147, falsch 56509 (max Bucket 174), echt 0 (min Bucket -1) */
    { 0x5100,  6,  19,  148 },   /* alt 116, falsch 30775 (max Bucket 148), echt 0 (min Bucket -1) */
    { 0x5100,  6,  20,  135 },   /* alt 120, falsch 14995 (max Bucket 135), echt 0 (min Bucket -1) */
    { 0x5100,  6,  21,  121 },   /* alt 106, falsch 12643 (max Bucket 121), echt 0 (min Bucket -1) */
    { 0x5100,  6,  22,  163 },   /* alt 141, falsch 14875 (max Bucket 163), echt 0 (min Bucket -1) */
    { 0x5100,  6,  23,  152 },   /* alt 137, falsch 11175 (max Bucket 152), echt 0 (min Bucket -1) */
    { 0x5100,  6,  24,  120 },   /* alt 106, falsch 8575 (max Bucket 120), echt 0 (min Bucket -1) */
    { 0x5100,  6,  25,  184 },   /* alt 166, falsch 13230 (max Bucket 184), echt 0 (min Bucket -1) */
    { 0x5100,  6,  40,   37 },   /* alt 0, falsch 164548 (max Bucket 37), echt 0 (min Bucket -1) */
    { 0x5100,  6,  46,   78 },   /* alt 0, falsch 9334 (max Bucket 78), echt 0 (min Bucket -1) */
    { 0x5100,  6,  47,  121 },   /* alt 107, falsch 1106 (max Bucket 121), echt 0 (min Bucket -1) */
    { 0x5100,  6,  48,  122 },   /* alt 109, falsch 1131 (max Bucket 122), echt 0 (min Bucket -1) */
    { 0x5100,  6,  49,  123 },   /* alt 110, falsch 1685 (max Bucket 123), echt 0 (min Bucket -1) */
    { 0x5100,  6,  50,  122 },   /* alt 112, falsch 577 (max Bucket 122), echt 0 (min Bucket -1) */
    { 0x5100,  6,  51,  122 },   /* alt 114, falsch 1165 (max Bucket 122), echt 0 (min Bucket -1) */
    { 0x5100,  6,  52,  129 },   /* alt 116, falsch 2320 (max Bucket 129), echt 0 (min Bucket -1) */
    { 0x5100,  6,  53,  152 },   /* alt 147, falsch 1469 (max Bucket 152), echt 0 (min Bucket -1) */
    { 0x5100,  6,  54,  164 },   /* alt 150, falsch 2253 (max Bucket 164), echt 0 (min Bucket -1) */
    { 0x5100,  6,  55,  167 },   /* alt 153, falsch 1511 (max Bucket 167), echt 0 (min Bucket -1) */
    { 0x5100,  6,  57,  176 },   /* alt 160, falsch 2393 (max Bucket 176), echt 0 (min Bucket -1) */
    { 0x5100,  6,  58,  177 },   /* alt 164, falsch 1589 (max Bucket 177), echt 0 (min Bucket -1) */
    { 0x5100,  6,  81,  118 },   /* alt 106, falsch 4739 (max Bucket 118), echt 0 (min Bucket -1) */
    { 0x5100,  6,  82,  115 },   /* alt 102, falsch 7130 (max Bucket 115), echt 0 (min Bucket -1) */
    { 0x5100,  6,  83,  116 },   /* alt 104, falsch 3151 (max Bucket 116), echt 0 (min Bucket -1) */
    { 0x5100,  6,  85,  188 },   /* alt 172, falsch 2612 (max Bucket 188), echt 0 (min Bucket -1) */
    { 0x5100,  6,  86,  191 },   /* alt 176, falsch 1737 (max Bucket 191), echt 0 (min Bucket -1) */
    { 0x5100,  6,  87,  197 },   /* alt 180, falsch 1806 (max Bucket 197), echt 0 (min Bucket -1) */
    { 0x5100,  6,  88,  150 },   /* alt 136, falsch 2716 (max Bucket 150), echt 0 (min Bucket -1) */
    { 0x5100,  6,  89,  154 },   /* alt 138, falsch 4175 (max Bucket 154), echt 0 (min Bucket -1) */
    { 0x5100,  6,  90,  155 },   /* alt 141, falsch 1416 (max Bucket 155), echt 0 (min Bucket -1) */
    { 0x5100,  6,  91,  158 },   /* alt 144, falsch 715 (max Bucket 158), echt 0 (min Bucket -1) */
    { 0x5100,  6,  92,  136 },   /* alt 124, falsch 2441 (max Bucket 136), echt 0 (min Bucket -1) */
    { 0x5100,  6,  93,  140 },   /* alt 126, falsch 3805 (max Bucket 140), echt 0 (min Bucket -1) */
    { 0x5100,  6,  94,  142 },   /* alt 128, falsch 1289 (max Bucket 142), echt 0 (min Bucket -1) */
    { 0x5100,  6,  95,  143 },   /* alt 131, falsch 651 (max Bucket 143), echt 0 (min Bucket -1) */
    { 0x5100,  6,  96,  130 },   /* alt 118, falsch 1183 (max Bucket 130), echt 0 (min Bucket -1) */
    { 0x5100,  6,  97,  133 },   /* alt 120, falsch 1203 (max Bucket 133), echt 0 (min Bucket -1) */
    { 0x5100,  6,  98,  124 },   /* alt 112, falsch 569 (max Bucket 124), echt 0 (min Bucket -1) */
    { 0x5100,  6,  99,  118 },   /* alt 107, falsch 1061 (max Bucket 118), echt 0 (min Bucket -1) */
    { 0x5100,  7,   4,   84 },   /* alt 66, falsch 776507 (max Bucket 84), echt 292555 (min Bucket 108) */
    { 0x5100,  7,   5,   83 },   /* alt 70, falsch 29868 (max Bucket 83), echt 24651 (min Bucket 108) */
    { 0x5100,  7,   6,   71 },   /* alt 64, falsch 8301 (max Bucket 71), echt 4823 (min Bucket 114) */
    { 0x5100,  7,   7,   83 },   /* alt 73, falsch 5928 (max Bucket 83), echt 4749 (min Bucket 108) */
    { 0x5100,  7,   8,   79 },   /* alt 64, falsch 244841 (max Bucket 79), echt 0 (min Bucket -1) */
    { 0x5100,  7,   9,   70 },   /* alt 64, falsch 57406 (max Bucket 70), echt 0 (min Bucket -1) */
    { 0x5100,  7,  10,   85 },   /* alt 73, falsch 62271 (max Bucket 85), echt 0 (min Bucket -1) */
    { 0x5100,  7,  11,   75 },   /* alt 66, falsch 14581 (max Bucket 75), echt 0 (min Bucket -1) */
    { 0x5100,  7,  12,   82 },   /* alt 72, falsch 9001 (max Bucket 82), echt 0 (min Bucket -1) */
    { 0x5100,  7,  13,   73 },   /* alt 65, falsch 5241 (max Bucket 73), echt 0 (min Bucket -1) */
    { 0x5100,  7,  14,   83 },   /* alt 76, falsch 783 (max Bucket 83), echt 520 (min Bucket 108) */
    { 0x5100,  7,  15,   76 },   /* alt 69, falsch 715 (max Bucket 76), echt 513 (min Bucket 112) */
    { 0x5100,  7,  16,   77 },   /* alt 70, falsch 1033 (max Bucket 77), echt 797 (min Bucket 111) */
    { 0x5100,  7,  17,   77 },   /* alt 71, falsch 427 (max Bucket 77), echt 265 (min Bucket 111) */
    { 0x5100,  7,  18,   79 },   /* alt 72, falsch 420 (max Bucket 79), echt 268 (min Bucket 111) */
    { 0x5100,  7,  19,   70 },   /* alt 64, falsch 951 (max Bucket 70), echt 520 (min Bucket 114) */
    { 0x5100,  7,  20,   71 },   /* alt 65, falsch 957 (max Bucket 71), echt 582 (min Bucket 114) */
    { 0x5100,  7,  21,   71 },   /* alt 66, falsch 407 (max Bucket 71), echt 191 (min Bucket 114) */
    { 0x5100,  7,  23,   66 },   /* alt 64, falsch 1897 (max Bucket 66), echt 646 (min Bucket 118) */
    { 0x5100,  7,  28,   82 },   /* alt 74, falsch 1099 (max Bucket 82), echt 0 (min Bucket -1) */
    { 0x5100,  7,  29,   83 },   /* alt 75, falsch 771 (max Bucket 83), echt 0 (min Bucket -1) */
    { 0x5100,  7,  30,   83 },   /* alt 76, falsch 777 (max Bucket 83), echt 0 (min Bucket -1) */
    { 0x5100,  7,  31,   79 },   /* alt 71, falsch 1368 (max Bucket 79), echt 0 (min Bucket -1) */
    { 0x5100,  7,  32,   80 },   /* alt 72, falsch 2326 (max Bucket 80), echt 0 (min Bucket -1) */
    { 0x5100,  7,  33,   80 },   /* alt 73, falsch 432 (max Bucket 80), echt 0 (min Bucket -1) */
    { 0x5100,  7,  34,   75 },   /* alt 68, falsch 1311 (max Bucket 75), echt 0 (min Bucket -1) */
    { 0x5100,  7,  35,   76 },   /* alt 69, falsch 2242 (max Bucket 76), echt 0 (min Bucket -1) */
    { 0x5100,  7,  36,   76 },   /* alt 70, falsch 420 (max Bucket 76), echt 0 (min Bucket -1) */
    { 0x5100,  7,  37,   73 },   /* alt 66, falsch 689 (max Bucket 73), echt 0 (min Bucket -1) */
    { 0x5100,  7,  38,   73 },   /* alt 67, falsch 408 (max Bucket 73), echt 0 (min Bucket -1) */
    { 0x5100,  7,  39,   72 },   /* alt 64, falsch 968 (max Bucket 72), echt 0 (min Bucket -1) */
    { 0x5100,  7,  40,   72 },   /* alt 65, falsch 1538 (max Bucket 72), echt 0 (min Bucket -1) */
    { 0x5101,  2,  12,   86 },   /* alt 75, falsch 934035 (max Bucket 86), echt 228586 (min Bucket 128) */
    { 0x5101,  2,  13,   72 },   /* alt 69, falsch 124759 (max Bucket 72), echt 0 (min Bucket -1) */
    { 0x5101,  2,  16,   88 },   /* alt 84, falsch 33829 (max Bucket 88), echt 27830 (min Bucket 127) */
    { 0x5101,  3,   3,  139 },   /* alt 114, falsch 700228 (max Bucket 139), echt 83588 (min Bucket 257) */
    { 0x5101,  3,   4,  118 },   /* alt 107, falsch 229085 (max Bucket 118), echt 0 (min Bucket -1) */
    { 0x5101,  3,   5,  139 },   /* alt 134, falsch 126599 (max Bucket 139), echt 0 (min Bucket -1) */
    { 0x5101,  3,   6,  100 },   /* alt 99, falsch 51159 (max Bucket 100), echt 0 (min Bucket -1) */
    { 0x5101,  3,   7,  119 },   /* alt 110, falsch 35532 (max Bucket 119), echt 0 (min Bucket -1) */
    { 0x5101,  3,   8,  104 },   /* alt 100, falsch 24522 (max Bucket 104), echt 0 (min Bucket -1) */
    { 0x5101,  3,  15,  192 },   /* alt 169, falsch 425583 (max Bucket 192), echt 160441 (min Bucket 246) */
    { 0x5101,  3,  16,  191 },   /* alt 169, falsch 127456 (max Bucket 191), echt 11403 (min Bucket 246) */
    { 0x5101,  3,  17,  167 },   /* alt 163, falsch 40454 (max Bucket 167), echt 0 (min Bucket -1) */
    { 0x5101,  3,  18,  192 },   /* alt 179, falsch 35922 (max Bucket 192), echt 13243 (min Bucket 246) */
    { 0x5101,  3,  22,   21 },   /* alt 0, falsch 8229 (max Bucket 21), echt 0 (min Bucket -1) */
    { 0x5101,  3,  23,   21 },   /* alt 0, falsch 11539 (max Bucket 21), echt 0 (min Bucket -1) */
    { 0x5101,  4,   0,  100 },   /* alt 77, falsch 652948 (max Bucket 100), echt 0 (min Bucket -1) */
    { 0x5101,  4,   1,   86 },   /* alt 76, falsch 87004 (max Bucket 86), echt 0 (min Bucket -1) */
    { 0x5101,  4,   2,  104 },   /* alt 90, falsch 62518 (max Bucket 104), echt 0 (min Bucket -1) */
    { 0x5101,  4,   4,  100 },   /* alt 90, falsch 28920 (max Bucket 100), echt 0 (min Bucket -1) */
    { 0x5101,  4,   5,   92 },   /* alt 80, falsch 21045 (max Bucket 92), echt 0 (min Bucket -1) */
    { 0x5101,  4,   7,   96 },   /* alt 83, falsch 266 (max Bucket 96), echt 0 (min Bucket -1) */
    { 0x5101,  4,   9,  101 },   /* alt 91, falsch 9339 (max Bucket 101), echt 0 (min Bucket -1) */
    { 0x5101,  4,  10,   83 },   /* alt 76, falsch 8974 (max Bucket 83), echt 0 (min Bucket -1) */
    { 0x5101,  4,  11,  104 },   /* alt 93, falsch 9776 (max Bucket 104), echt 0 (min Bucket -1) */
    { 0x5101,  4,  12,   89 },   /* alt 78, falsch 6206 (max Bucket 89), echt 0 (min Bucket -1) */
    { 0x5101,  4,  14,   95 },   /* alt 84, falsch 5163 (max Bucket 95), echt 0 (min Bucket -1) */
    { 0x5101,  4,  30,  157 },   /* alt 112, falsch 380365 (max Bucket 157), echt 89817 (min Bucket 180) */
    { 0x5101,  4,  31,  147 },   /* alt 115, falsch 186532 (max Bucket 147), echt 18 (min Bucket 187) */
    { 0x5101,  4,  32,  116 },   /* alt 98, falsch 119880 (max Bucket 116), echt 0 (min Bucket -1) */
    { 0x5101,  4,  33,  129 },   /* alt 109, falsch 48191 (max Bucket 129), echt 0 (min Bucket -1) */
    { 0x5101,  4,  34,  159 },   /* alt 135, falsch 57025 (max Bucket 159), echt 1279 (min Bucket 185) */
    { 0x5101,  4,  35,  112 },   /* alt 96, falsch 28363 (max Bucket 112), echt 0 (min Bucket -1) */
    { 0x5101,  4,  36,  155 },   /* alt 123, falsch 3102 (max Bucket 155), echt 7302 (min Bucket 180) */
    { 0x5101,  4,  37,  145 },   /* alt 123, falsch 16678 (max Bucket 145), echt 1398 (min Bucket 187) */
    { 0x5101,  4,  38,  122 },   /* alt 106, falsch 11753 (max Bucket 122), echt 21 (min Bucket 191) */
    { 0x5101,  4,  39,  138 },   /* alt 119, falsch 13619 (max Bucket 138), echt 0 (min Bucket -1) */
    { 0x5101,  4,  40,  123 },   /* alt 106, falsch 12055 (max Bucket 123), echt 0 (min Bucket -1) */
    { 0x5101,  4,  41,  110 },   /* alt 95, falsch 10818 (max Bucket 110), echt 0 (min Bucket -1) */
    { 0x5101,  4,  42,  114 },   /* alt 99, falsch 7959 (max Bucket 114), echt 0 (min Bucket -1) */
    { 0x5101,  4,  43,  155 },   /* alt 132, falsch 650 (max Bucket 155), echt 3706 (min Bucket 180) */
    { 0x5101,  4,  44,  104 },   /* alt 93, falsch 6148 (max Bucket 104), echt 0 (min Bucket -1) */
    { 0x5101,  4,  53,    7 },   /* alt 0, falsch 255 (max Bucket 7), echt 0 (min Bucket -1) */
    { 0x5101,  4,  54,    7 },   /* alt 0, falsch 255 (max Bucket 7), echt 0 (min Bucket -1) */
    { 0x5101,  4,  60,  155 },   /* alt 140, falsch 264 (max Bucket 155), echt 240 (min Bucket 180) */
    { 0x5101,  4,  61,  153 },   /* alt 138, falsch 809 (max Bucket 153), echt 439 (min Bucket 180) */
    { 0x5101,  4,  62,  141 },   /* alt 124, falsch 3556 (max Bucket 141), echt 204 (min Bucket 183) */
    { 0x5101,  4,  63,  134 },   /* alt 120, falsch 2975 (max Bucket 134), echt 150 (min Bucket 183) */
    { 0x5101,  4,  65,  119 },   /* alt 109, falsch 262 (max Bucket 119), echt 10 (min Bucket 186) */
    { 0x5101,  4,  66,  119 },   /* alt 107, falsch 2658 (max Bucket 119), echt 4 (min Bucket 186) */
    { 0x5101,  4,  67,  117 },   /* alt 103, falsch 4510 (max Bucket 117), echt 0 (min Bucket -1) */
    { 0x5101,  4,  68,  113 },   /* alt 101, falsch 5315 (max Bucket 113), echt 0 (min Bucket -1) */
    { 0x5101,  4,  70,  102 },   /* alt 91, falsch 1064 (max Bucket 102), echt 0 (min Bucket -1) */
    { 0x5101,  4,  71,   97 },   /* alt 86, falsch 2145 (max Bucket 97), echt 0 (min Bucket -1) */
    { 0x5101,  4,  73,   86 },   /* alt 77, falsch 5202 (max Bucket 86), echt 0 (min Bucket -1) */
    { 0x5101,  4,  80,  130 },   /* alt 115, falsch 4281 (max Bucket 130), echt 150 (min Bucket 190) */
    { 0x5101,  4,  81,  126 },   /* alt 113, falsch 3623 (max Bucket 126), echt 41 (min Bucket 190) */
    { 0x5101,  4,  82,  102 },   /* alt 92, falsch 5118 (max Bucket 102), echt 0 (min Bucket -1) */
    { 0x5101,  4,  86,  110 },   /* alt 97, falsch 2995 (max Bucket 110), echt 0 (min Bucket -1) */
    { 0x5101,  4,  87,  107 },   /* alt 94, falsch 9489 (max Bucket 107), echt 0 (min Bucket -1) */
    { 0x5101,  4,  88,  102 },   /* alt 90, falsch 5237 (max Bucket 102), echt 0 (min Bucket -1) */
    { 0x5101,  4,  89,  153 },   /* alt 135, falsch 2838 (max Bucket 153), echt 0 (min Bucket -1) */
    { 0x5101,  4,  90,  150 },   /* alt 131, falsch 4749 (max Bucket 150), echt 0 (min Bucket -1) */
    { 0x5101,  4,  92,  141 },   /* alt 127, falsch 1715 (max Bucket 141), echt 0 (min Bucket -1) */
    { 0x5101,  4,  95,  132 },   /* alt 116, falsch 4166 (max Bucket 132), echt 0 (min Bucket -1) */
    { 0x5101,  4,  98,  125 },   /* alt 110, falsch 3146 (max Bucket 125), echt 0 (min Bucket -1) */
    { 0x5101,  4, 100,  112 },   /* alt 99, falsch 3978 (max Bucket 112), echt 0 (min Bucket -1) */
    { 0x5101,  4, 101,  107 },   /* alt 95, falsch 2390 (max Bucket 107), echt 0 (min Bucket -1) */
    { 0x5101,  4, 102,   94 },   /* alt 83, falsch 2107 (max Bucket 94), echt 0 (min Bucket -1) */
    { 0x5101,  4, 103,   91 },   /* alt 81, falsch 2017 (max Bucket 91), echt 0 (min Bucket -1) */
    { 0x5101,  4, 104,   89 },   /* alt 79, falsch 2878 (max Bucket 89), echt 0 (min Bucket -1) */
    { 0x5101,  5,   3,  272 },   /* alt 179, falsch 1000110 (max Bucket 272), echt 83887 (min Bucket 301) */
    { 0x5101,  5,   4,  202 },   /* alt 179, falsch 183681 (max Bucket 202), echt 0 (min Bucket -1) */
    { 0x5101,  5,   5,  349 },   /* alt 248, falsch 81449 (max Bucket 349), echt 11579 (min Bucket 301), echt teils nicht mehr verdeckt */
    { 0x5101,  5,   6,  232 },   /* alt 195, falsch 39775 (max Bucket 232), echt 0 (min Bucket -1) */
    { 0x5101,  5,   7,    8 },   /* alt 0, falsch 2416 (max Bucket 8), echt 0 (min Bucket -1) */
    { 0x5101,  5,   8,    8 },   /* alt 0, falsch 2344 (max Bucket 8), echt 0 (min Bucket -1) */
    { 0x5101,  5,  10,  271 },   /* alt 245, falsch 1411 (max Bucket 271), echt 0 (min Bucket -1) */
    { 0x5101,  5,  11,  257 },   /* alt 232, falsch 1295 (max Bucket 257), echt 0 (min Bucket -1) */
    { 0x5101,  5,  12,  253 },   /* alt 226, falsch 2368 (max Bucket 253), echt 0 (min Bucket -1) */
    { 0x5101,  5,  13,  247 },   /* alt 220, falsch 3452 (max Bucket 247), echt 0 (min Bucket -1) */
    { 0x5101,  5,  14,  242 },   /* alt 214, falsch 4373 (max Bucket 242), echt 0 (min Bucket -1) */
    { 0x5101,  5,  15,  236 },   /* alt 209, falsch 5367 (max Bucket 236), echt 0 (min Bucket -1) */
    { 0x5101,  5,  16,  221 },   /* alt 199, falsch 2126 (max Bucket 221), echt 0 (min Bucket -1) */
    { 0x5101,  5,  17,  218 },   /* alt 195, falsch 2068 (max Bucket 218), echt 0 (min Bucket -1) */
    { 0x5101,  5,  18,  214 },   /* alt 191, falsch 2964 (max Bucket 214), echt 0 (min Bucket -1) */
    { 0x5101,  5,  19,  209 },   /* alt 186, falsch 3697 (max Bucket 209), echt 0 (min Bucket -1) */
    { 0x5101,  5,  20,  206 },   /* alt 183, falsch 4642 (max Bucket 206), echt 0 (min Bucket -1) */
    { 0x5101,  6,  15,  208 },   /* alt 134, falsch 1473896 (max Bucket 208), echt 200387 (min Bucket 225) */
    { 0x5101,  6,  16,  149 },   /* alt 115, falsch 211173 (max Bucket 149), echt 0 (min Bucket -1) */
    { 0x5101,  6,  17,  128 },   /* alt 108, falsch 58527 (max Bucket 128), echt 0 (min Bucket -1) */
    { 0x5101,  6,  18,  174 },   /* alt 147, falsch 56509 (max Bucket 174), echt 0 (min Bucket -1) */
    { 0x5101,  6,  19,  148 },   /* alt 116, falsch 30775 (max Bucket 148), echt 0 (min Bucket -1) */
    { 0x5101,  6,  20,  135 },   /* alt 120, falsch 14995 (max Bucket 135), echt 0 (min Bucket -1) */
    { 0x5101,  6,  21,  121 },   /* alt 106, falsch 12643 (max Bucket 121), echt 0 (min Bucket -1) */
    { 0x5101,  6,  22,  163 },   /* alt 141, falsch 14875 (max Bucket 163), echt 0 (min Bucket -1) */
    { 0x5101,  6,  23,  152 },   /* alt 137, falsch 11175 (max Bucket 152), echt 0 (min Bucket -1) */
    { 0x5101,  6,  24,  120 },   /* alt 106, falsch 8575 (max Bucket 120), echt 0 (min Bucket -1) */
    { 0x5101,  6,  25,  184 },   /* alt 166, falsch 13230 (max Bucket 184), echt 0 (min Bucket -1) */
    { 0x5101,  6,  40,   37 },   /* alt 0, falsch 164548 (max Bucket 37), echt 0 (min Bucket -1) */
    { 0x5101,  6,  46,   78 },   /* alt 0, falsch 9334 (max Bucket 78), echt 0 (min Bucket -1) */
    { 0x5101,  6,  47,  121 },   /* alt 107, falsch 1106 (max Bucket 121), echt 0 (min Bucket -1) */
    { 0x5101,  6,  48,  122 },   /* alt 109, falsch 1131 (max Bucket 122), echt 0 (min Bucket -1) */
    { 0x5101,  6,  49,  123 },   /* alt 110, falsch 1685 (max Bucket 123), echt 0 (min Bucket -1) */
    { 0x5101,  6,  50,  122 },   /* alt 112, falsch 577 (max Bucket 122), echt 0 (min Bucket -1) */
    { 0x5101,  6,  51,  122 },   /* alt 114, falsch 1165 (max Bucket 122), echt 0 (min Bucket -1) */
    { 0x5101,  6,  52,  129 },   /* alt 116, falsch 2320 (max Bucket 129), echt 0 (min Bucket -1) */
    { 0x5101,  6,  53,  152 },   /* alt 147, falsch 1469 (max Bucket 152), echt 0 (min Bucket -1) */
    { 0x5101,  6,  54,  164 },   /* alt 150, falsch 2253 (max Bucket 164), echt 0 (min Bucket -1) */
    { 0x5101,  6,  55,  167 },   /* alt 153, falsch 1511 (max Bucket 167), echt 0 (min Bucket -1) */
    { 0x5101,  6,  57,  176 },   /* alt 160, falsch 2393 (max Bucket 176), echt 0 (min Bucket -1) */
    { 0x5101,  6,  58,  177 },   /* alt 164, falsch 1589 (max Bucket 177), echt 0 (min Bucket -1) */
    { 0x5101,  6,  81,  118 },   /* alt 106, falsch 4739 (max Bucket 118), echt 0 (min Bucket -1) */
    { 0x5101,  6,  82,  115 },   /* alt 102, falsch 7130 (max Bucket 115), echt 0 (min Bucket -1) */
    { 0x5101,  6,  83,  116 },   /* alt 104, falsch 3151 (max Bucket 116), echt 0 (min Bucket -1) */
    { 0x5101,  6,  85,  188 },   /* alt 172, falsch 2612 (max Bucket 188), echt 0 (min Bucket -1) */
    { 0x5101,  6,  86,  191 },   /* alt 176, falsch 1737 (max Bucket 191), echt 0 (min Bucket -1) */
    { 0x5101,  6,  87,  197 },   /* alt 180, falsch 1806 (max Bucket 197), echt 0 (min Bucket -1) */
    { 0x5101,  6,  88,  150 },   /* alt 136, falsch 2716 (max Bucket 150), echt 0 (min Bucket -1) */
    { 0x5101,  6,  89,  154 },   /* alt 138, falsch 4175 (max Bucket 154), echt 0 (min Bucket -1) */
    { 0x5101,  6,  90,  155 },   /* alt 141, falsch 1416 (max Bucket 155), echt 0 (min Bucket -1) */
    { 0x5101,  6,  91,  158 },   /* alt 144, falsch 715 (max Bucket 158), echt 0 (min Bucket -1) */
    { 0x5101,  6,  92,  136 },   /* alt 124, falsch 2441 (max Bucket 136), echt 0 (min Bucket -1) */
    { 0x5101,  6,  93,  140 },   /* alt 126, falsch 3805 (max Bucket 140), echt 0 (min Bucket -1) */
    { 0x5101,  6,  94,  142 },   /* alt 128, falsch 1289 (max Bucket 142), echt 0 (min Bucket -1) */
    { 0x5101,  6,  95,  143 },   /* alt 131, falsch 651 (max Bucket 143), echt 0 (min Bucket -1) */
    { 0x5101,  6,  96,  130 },   /* alt 118, falsch 1183 (max Bucket 130), echt 0 (min Bucket -1) */
    { 0x5101,  6,  97,  133 },   /* alt 120, falsch 1203 (max Bucket 133), echt 0 (min Bucket -1) */
    { 0x5101,  6,  98,  124 },   /* alt 112, falsch 569 (max Bucket 124), echt 0 (min Bucket -1) */
    { 0x5101,  6,  99,  118 },   /* alt 107, falsch 1061 (max Bucket 118), echt 0 (min Bucket -1) */
    { 0x5101,  7,   4,   84 },   /* alt 66, falsch 776507 (max Bucket 84), echt 292555 (min Bucket 108) */
    { 0x5101,  7,   5,   83 },   /* alt 70, falsch 29868 (max Bucket 83), echt 24651 (min Bucket 108) */
    { 0x5101,  7,   6,   71 },   /* alt 64, falsch 8301 (max Bucket 71), echt 4823 (min Bucket 114) */
    { 0x5101,  7,   7,   83 },   /* alt 73, falsch 5928 (max Bucket 83), echt 4749 (min Bucket 108) */
    { 0x5101,  7,   8,   79 },   /* alt 64, falsch 244841 (max Bucket 79), echt 0 (min Bucket -1) */
    { 0x5101,  7,   9,   70 },   /* alt 64, falsch 57406 (max Bucket 70), echt 0 (min Bucket -1) */
    { 0x5101,  7,  10,   85 },   /* alt 73, falsch 62271 (max Bucket 85), echt 0 (min Bucket -1) */
    { 0x5101,  7,  11,   75 },   /* alt 66, falsch 14581 (max Bucket 75), echt 0 (min Bucket -1) */
    { 0x5101,  7,  12,   82 },   /* alt 72, falsch 9001 (max Bucket 82), echt 0 (min Bucket -1) */
    { 0x5101,  7,  13,   73 },   /* alt 65, falsch 5241 (max Bucket 73), echt 0 (min Bucket -1) */
    { 0x5101,  7,  14,   83 },   /* alt 76, falsch 783 (max Bucket 83), echt 520 (min Bucket 108) */
    { 0x5101,  7,  15,   76 },   /* alt 69, falsch 715 (max Bucket 76), echt 513 (min Bucket 112) */
    { 0x5101,  7,  16,   77 },   /* alt 70, falsch 1033 (max Bucket 77), echt 797 (min Bucket 111) */
    { 0x5101,  7,  17,   77 },   /* alt 71, falsch 427 (max Bucket 77), echt 265 (min Bucket 111) */
    { 0x5101,  7,  18,   79 },   /* alt 72, falsch 420 (max Bucket 79), echt 268 (min Bucket 111) */
    { 0x5101,  7,  19,   70 },   /* alt 64, falsch 951 (max Bucket 70), echt 520 (min Bucket 114) */
    { 0x5101,  7,  20,   71 },   /* alt 65, falsch 957 (max Bucket 71), echt 582 (min Bucket 114) */
    { 0x5101,  7,  21,   71 },   /* alt 66, falsch 407 (max Bucket 71), echt 191 (min Bucket 114) */
    { 0x5101,  7,  23,   66 },   /* alt 64, falsch 1897 (max Bucket 66), echt 646 (min Bucket 118) */
    { 0x5101,  7,  28,   82 },   /* alt 74, falsch 1099 (max Bucket 82), echt 0 (min Bucket -1) */
    { 0x5101,  7,  29,   83 },   /* alt 75, falsch 771 (max Bucket 83), echt 0 (min Bucket -1) */
    { 0x5101,  7,  30,   83 },   /* alt 76, falsch 777 (max Bucket 83), echt 0 (min Bucket -1) */
    { 0x5101,  7,  31,   79 },   /* alt 71, falsch 1368 (max Bucket 79), echt 0 (min Bucket -1) */
    { 0x5101,  7,  32,   80 },   /* alt 72, falsch 2326 (max Bucket 80), echt 0 (min Bucket -1) */
    { 0x5101,  7,  33,   80 },   /* alt 73, falsch 432 (max Bucket 80), echt 0 (min Bucket -1) */
    { 0x5101,  7,  34,   75 },   /* alt 68, falsch 1311 (max Bucket 75), echt 0 (min Bucket -1) */
    { 0x5101,  7,  35,   76 },   /* alt 69, falsch 2242 (max Bucket 76), echt 0 (min Bucket -1) */
    { 0x5101,  7,  36,   76 },   /* alt 70, falsch 420 (max Bucket 76), echt 0 (min Bucket -1) */
    { 0x5101,  7,  37,   73 },   /* alt 66, falsch 689 (max Bucket 73), echt 0 (min Bucket -1) */
    { 0x5101,  7,  38,   73 },   /* alt 67, falsch 408 (max Bucket 73), echt 0 (min Bucket -1) */
    { 0x5101,  7,  39,   72 },   /* alt 64, falsch 968 (max Bucket 72), echt 0 (min Bucket -1) */
    { 0x5101,  7,  40,   72 },   /* alt 65, falsch 1538 (max Bucket 72), echt 0 (min Bucket -1) */
    { 0x6000,  3,   7,  141 },   /* alt 0, falsch 69199 (max Bucket 141), echt 0 (min Bucket -1) */
    { 0x6000,  3,   8,  118 },   /* alt 0, falsch 1877 (max Bucket 118), echt 0 (min Bucket -1) */
    { 0x6000,  5,   0,  199 },   /* alt 155, falsch 866683 (max Bucket 199), echt 447184 (min Bucket 218) */
    { 0x6000,  5,   1,  183 },   /* alt 155, falsch 104538 (max Bucket 183), echt 67336 (min Bucket 216) */
    { 0x6000,  5,   3,  176 },   /* alt 166, falsch 47911 (max Bucket 176), echt 0 (min Bucket -1) */
    { 0x6000,  5,  13,  189 },   /* alt 129, falsch 699892 (max Bucket 189), echt 194533 (min Bucket 77), echt teils nicht mehr verdeckt */
    { 0x6000,  5,  14,  165 },   /* alt 129, falsch 74118 (max Bucket 165), echt 524 (min Bucket 93), echt teils nicht mehr verdeckt */
    { 0x6000,  5,  15,  154 },   /* alt 129, falsch 27227 (max Bucket 154), echt 0 (min Bucket -1) */
    { 0x6000,  5,  16,  174 },   /* alt 129, falsch 22932 (max Bucket 174), echt 75 (min Bucket 92), echt teils nicht mehr verdeckt */
    { 0x6000,  5,  17,  181 },   /* alt 129, falsch 12101 (max Bucket 181), echt 0 (min Bucket -1) */
    { 0x6000,  5,  18,  159 },   /* alt 129, falsch 9929 (max Bucket 159), echt 0 (min Bucket -1) */
    { 0x6000,  5,  19,  234 },   /* alt 129, falsch 370258 (max Bucket 234), echt 329166 (min Bucket 70), echt teils nicht mehr verdeckt */
    { 0x6000,  5,  20,  209 },   /* alt 129, falsch 56856 (max Bucket 209), echt 4708 (min Bucket 80), echt teils nicht mehr verdeckt */
    { 0x6000,  5,  21,  200 },   /* alt 129, falsch 17836 (max Bucket 200), echt 0 (min Bucket -1) */
    { 0x6000,  5,  22,  219 },   /* alt 129, falsch 19815 (max Bucket 219), echt 0 (min Bucket -1) */
    { 0x6000,  5,  23,  201 },   /* alt 129, falsch 4231 (max Bucket 201), echt 13679 (min Bucket 87), echt teils nicht mehr verdeckt */
    { 0x6000,  5,  25,  215 },   /* alt 129, falsch 6259 (max Bucket 215), echt 5317 (min Bucket 87), echt teils nicht mehr verdeckt */
    { 0x6000,  5,  29,  215 },   /* alt 129, falsch 5177 (max Bucket 215), echt 23 (min Bucket 87), echt teils nicht mehr verdeckt */
    { 0x6000,  5,  33,  236 },   /* alt 129, falsch 107814 (max Bucket 236), echt 158620 (min Bucket 70), echt teils nicht mehr verdeckt */
    { 0x6000,  5,  34,  237 },   /* alt 129, falsch 47969 (max Bucket 237), echt 1792 (min Bucket 72), echt teils nicht mehr verdeckt */
    { 0x6000,  5,  35,  161 },   /* alt 129, falsch 4231 (max Bucket 161), echt 3813 (min Bucket 70), echt teils nicht mehr verdeckt */
    { 0x6000,  5,  36,  238 },   /* alt 129, falsch 17696 (max Bucket 238), echt 0 (min Bucket -1) */
    { 0x6000,  5,  37,  227 },   /* alt 129, falsch 11822 (max Bucket 227), echt 0 (min Bucket -1) */
    { 0x6000,  5,  38,  256 },   /* alt 212, falsch 244162 (max Bucket 256), echt 106978 (min Bucket 144), echt teils nicht mehr verdeckt */
    { 0x6000,  5,  41,  172 },   /* alt 143, falsch 102504 (max Bucket 172), echt 0 (min Bucket -1) */
    { 0x6000,  5,  65,  215 },   /* alt 129, falsch 4831 (max Bucket 215), echt 0 (min Bucket -1) */
    { 0x6000,  5,  66,  239 },   /* alt 212, falsch 8034 (max Bucket 239), echt 5146 (min Bucket 143), echt teils nicht mehr verdeckt */
    { 0x6000,  5,  70,  222 },   /* alt 129, falsch 5895 (max Bucket 222), echt 0 (min Bucket -1) */
    { 0x6000,  5,  73,  227 },   /* alt 129, falsch 6631 (max Bucket 227), echt 0 (min Bucket -1) */
    { 0x6000,  5,  76,  233 },   /* alt 129, falsch 7320 (max Bucket 233), echt 0 (min Bucket -1) */
    { 0x6000,  5,  80,  173 },   /* alt 171, falsch 6998 (max Bucket 173), echt 177 (min Bucket 238) */
    { 0x6000,  5,  82,  238 },   /* alt 129, falsch 9363 (max Bucket 238), echt 0 (min Bucket -1) */
    { 0x6000,  5,  83,  230 },   /* alt 129, falsch 11715 (max Bucket 230), echt 0 (min Bucket -1) */
    { 0x6000,  5,  84,  216 },   /* alt 129, falsch 7625 (max Bucket 216), echt 0 (min Bucket -1) */
    { 0x6000,  5,  86,  206 },   /* alt 129, falsch 7433 (max Bucket 206), echt 0 (min Bucket -1) */
    { 0x6000,  5,  88,  197 },   /* alt 129, falsch 10669 (max Bucket 197), echt 0 (min Bucket -1) */
    { 0x6000,  5,  89,  175 },   /* alt 168, falsch 4545 (max Bucket 175), echt 0 (min Bucket -1) */
    { 0x6000,  5,  90,  177 },   /* alt 167, falsch 5218 (max Bucket 177), echt 0 (min Bucket -1) */
    { 0x6000,  5,  91,  179 },   /* alt 165, falsch 5897 (max Bucket 179), echt 0 (min Bucket -1) */
    { 0x6000,  5,  92,  181 },   /* alt 163, falsch 6600 (max Bucket 181), echt 0 (min Bucket -1) */
    { 0x6000,  5,  93,  181 },   /* alt 162, falsch 6944 (max Bucket 181), echt 0 (min Bucket -1) */
    { 0x6000,  5,  94,  186 },   /* alt 129, falsch 6755 (max Bucket 186), echt 0 (min Bucket -1) */
    { 0x6000,  5,  95,  178 },   /* alt 129, falsch 6513 (max Bucket 178), echt 0 (min Bucket -1) */
    { 0x6000,  5,  96,  178 },   /* alt 159, falsch 4927 (max Bucket 178), echt 0 (min Bucket -1) */
    { 0x6000,  5,  97,  178 },   /* alt 158, falsch 5276 (max Bucket 178), echt 0 (min Bucket -1) */
    { 0x6000,  5,  98,  178 },   /* alt 157, falsch 5927 (max Bucket 178), echt 0 (min Bucket -1) */
    { 0x6000,  5,  99,  178 },   /* alt 155, falsch 17128 (max Bucket 178), echt 0 (min Bucket -1) */
    { 0x6000,  5, 100,  172 },   /* alt 129, falsch 8806 (max Bucket 172), echt 0 (min Bucket -1) */
    { 0x6000,  5, 101,  163 },   /* alt 129, falsch 6038 (max Bucket 163), echt 0 (min Bucket -1) */
    { 0x6000,  5, 102,  157 },   /* alt 129, falsch 5934 (max Bucket 157), echt 0 (min Bucket -1) */
    { 0x6000,  5, 103,  152 },   /* alt 130, falsch 7098 (max Bucket 152), echt 0 (min Bucket -1) */
    { 0x6000,  5, 104,  150 },   /* alt 131, falsch 5928 (max Bucket 150), echt 0 (min Bucket -1) */
    { 0x6000,  7,   0,  207 },   /* alt 124, falsch 533925 (max Bucket 207), echt 0 (min Bucket -1) */
    { 0x6000,  7,   1,  160 },   /* alt 124, falsch 41005 (max Bucket 160), echt 0 (min Bucket -1) */
    { 0x6000,  7,   2,  211 },   /* alt 150, falsch 87606 (max Bucket 211), echt 0 (min Bucket -1) */
    { 0x6000,  7,   4,  154 },   /* alt 124, falsch 38309 (max Bucket 154), echt 0 (min Bucket -1) */
    { 0x6000,  7,   5,  211 },   /* alt 124, falsch 12531 (max Bucket 211), echt 0 (min Bucket -1) */
    { 0x6000,  7,   7,  179 },   /* alt 145, falsch 22436 (max Bucket 179), echt 0 (min Bucket -1) */
    { 0x6000,  7,   8,  211 },   /* alt 170, falsch 21565 (max Bucket 211), echt 0 (min Bucket -1) */
    { 0x6000,  7,  11,  154 },   /* alt 124, falsch 10534 (max Bucket 154), echt 0 (min Bucket -1) */
    { 0x6000,  7,  12,  150 },   /* alt 125, falsch 9178 (max Bucket 150), echt 0 (min Bucket -1) */
    { 0x6000,  7,  13,  145 },   /* alt 124, falsch 204956 (max Bucket 145), echt 94381 (min Bucket 98), echt teils nicht mehr verdeckt */
    { 0x6000,  7,  16,  143 },   /* alt 124, falsch 39877 (max Bucket 143), echt 0 (min Bucket -1) */
    { 0x6000,  7,  22,  147 },   /* alt 124, falsch 8480 (max Bucket 147), echt 774 (min Bucket 98), echt teils nicht mehr verdeckt */
    { 0x6000,  7,  28,  150 },   /* alt 124, falsch 15470 (max Bucket 150), echt 10439 (min Bucket 99), echt teils nicht mehr verdeckt */
    { 0x6000,  7,  42,  149 },   /* alt 124, falsch 281276 (max Bucket 149), echt 0 (min Bucket -1) */
    { 0x6000,  7,  43,  152 },   /* alt 124, falsch 31606 (max Bucket 152), echt 0 (min Bucket -1) */
    { 0x6000,  7,  44,  147 },   /* alt 124, falsch 41171 (max Bucket 147), echt 0 (min Bucket -1) */
    { 0x6000,  7,  46,  152 },   /* alt 124, falsch 21975 (max Bucket 152), echt 0 (min Bucket -1) */
    { 0x6000,  7,  48,  153 },   /* alt 124, falsch 6710 (max Bucket 153), echt 0 (min Bucket -1) */
    { 0x6000,  7,  49,  281 },   /* alt 148, falsch 264781 (max Bucket 281), echt 46171 (min Bucket 224), echt teils nicht mehr verdeckt */
    { 0x6000,  7,  50,  277 },   /* alt 178, falsch 69020 (max Bucket 277), echt 35698 (min Bucket 175), echt teils nicht mehr verdeckt */
    { 0x6000,  7,  51,  209 },   /* alt 124, falsch 3709 (max Bucket 209), echt 0 (min Bucket -1) */
    { 0x6000,  7,  52,  276 },   /* alt 209, falsch 59501 (max Bucket 276), echt 0 (min Bucket -1) */
    { 0x6000,  7,  54,  266 },   /* alt 158, falsch 3396 (max Bucket 266), echt 8297 (min Bucket 177), echt teils nicht mehr verdeckt */
    { 0x6000,  7,  55,  213 },   /* alt 147, falsch 13462 (max Bucket 213), echt 0 (min Bucket -1) */
    { 0x6000,  7,  74,  262 },   /* alt 209, falsch 6332 (max Bucket 262), echt 251 (min Bucket 176), echt teils nicht mehr verdeckt */
    { 0x6000,  7,  75,  263 },   /* alt 198, falsch 13462 (max Bucket 263), echt 278 (min Bucket 177), echt teils nicht mehr verdeckt */
    { 0x6000,  7,  76,  275 },   /* alt 234, falsch 24719 (max Bucket 275), echt 257 (min Bucket 175), echt teils nicht mehr verdeckt */
    { 0x6000,  7,  79,  267 },   /* alt 262, falsch 311 (max Bucket 267), echt 0 (min Bucket -1) */
    { 0x6000,  7,  80,  273 },   /* alt 249, falsch 5332 (max Bucket 273), echt 0 (min Bucket -1) */
    { 0x6000,  7,  84,  211 },   /* alt 180, falsch 7220 (max Bucket 211), echt 0 (min Bucket -1) */
    { 0x6000,  7,  86,  194 },   /* alt 167, falsch 8119 (max Bucket 194), echt 0 (min Bucket -1) */
    { 0x6000,  7,  91,  179 },   /* alt 152, falsch 8164 (max Bucket 179), echt 0 (min Bucket -1) */
    { 0x6000,  7,  94,  165 },   /* alt 141, falsch 10173 (max Bucket 165), echt 0 (min Bucket -1) */
    { 0x6000,  7,  96,  154 },   /* alt 124, falsch 19112 (max Bucket 154), echt 0 (min Bucket -1) */
    { 0x6000,  7,  98,  157 },   /* alt 124, falsch 20265 (max Bucket 157), echt 0 (min Bucket -1) */
    { 0x6000,  7,  99,  149 },   /* alt 124, falsch 14072 (max Bucket 149), echt 5158 (min Bucket 99), echt teils nicht mehr verdeckt */
    { 0x6000,  7, 100,  148 },   /* alt 124, falsch 7562 (max Bucket 148), echt 3914 (min Bucket 99), echt teils nicht mehr verdeckt */
    { 0x6000,  7, 104,  147 },   /* alt 127, falsch 13304 (max Bucket 147), echt 0 (min Bucket -1) */
    { 0x6000,  8,  10,  202 },   /* alt 159, falsch 241779 (max Bucket 202), echt 0 (min Bucket -1) */
    { 0x6000,  8,  11,  206 },   /* alt 159, falsch 47782 (max Bucket 206), echt 817 (min Bucket 234) */
    { 0x6000,  8,  12,  208 },   /* alt 164, falsch 71865 (max Bucket 208), echt 0 (min Bucket -1) */
    { 0x6000,  8,  13,  182 },   /* alt 159, falsch 81995 (max Bucket 182), echt 0 (min Bucket -1) */
    { 0x6000,  8,  14,  191 },   /* alt 159, falsch 72154 (max Bucket 191), echt 0 (min Bucket -1) */
    { 0x6000,  8,  15,  186 },   /* alt 159, falsch 5545 (max Bucket 186), echt 1832 (min Bucket 231) */
    { 0x6000,  8,  16,  213 },   /* alt 180, falsch 35172 (max Bucket 213), echt 0 (min Bucket -1) */
    { 0x6000,  8,  17,  204 },   /* alt 159, falsch 41933 (max Bucket 204), echt 0 (min Bucket -1) */
    { 0x6000,  8,  18,  199 },   /* alt 159, falsch 15243 (max Bucket 199), echt 0 (min Bucket -1) */
    { 0x6000,  8,  19,  168 },   /* alt 159, falsch 12808 (max Bucket 168), echt 0 (min Bucket -1) */
    { 0x6000,  8,  22,  163 },   /* alt 159, falsch 6756 (max Bucket 163), echt 0 (min Bucket -1) */
    { 0x6000,  8,  23,  207 },   /* alt 175, falsch 16798 (max Bucket 207), echt 0 (min Bucket -1) */
    { 0x6000,  8,  24,  183 },   /* alt 159, falsch 17091 (max Bucket 183), echt 0 (min Bucket -1) */
    { 0x6000,  8,  25,  212 },   /* alt 193, falsch 12370 (max Bucket 212), echt 0 (min Bucket -1) */
    { 0x6000,  8,  26,  188 },   /* alt 162, falsch 14219 (max Bucket 188), echt 0 (min Bucket -1) */
    { 0x6000,  8,  27,  185 },   /* alt 159, falsch 17449 (max Bucket 185), echt 0 (min Bucket -1) */
    { 0x6000,  8,  28,  184 },   /* alt 159, falsch 14737 (max Bucket 184), echt 0 (min Bucket -1) */
    { 0x6000,  8,  29,  191 },   /* alt 162, falsch 4997 (max Bucket 191), echt 327191 (min Bucket 205) */
    { 0x6000,  8,  37,  179 },   /* alt 159, falsch 666939 (max Bucket 179), echt 8117 (min Bucket 153), echt teils nicht mehr verdeckt */
    { 0x6000,  8,  38,  184 },   /* alt 159, falsch 106996 (max Bucket 184), echt 0 (min Bucket -1) */
    { 0x6000,  8,  40,  196 },   /* alt 165, falsch 50621 (max Bucket 196), echt 4700 (min Bucket 154), echt teils nicht mehr verdeckt */
    { 0x6000,  8,  41,  183 },   /* alt 159, falsch 33091 (max Bucket 183), echt 0 (min Bucket -1) */
    { 0x6000,  8,  44,  167 },   /* alt 159, falsch 21256 (max Bucket 167), echt 0 (min Bucket -1) */
    { 0x6000,  8,  45,  187 },   /* alt 159, falsch 12992 (max Bucket 187), echt 969 (min Bucket 165), echt teils nicht mehr verdeckt */
    { 0x6000,  8,  46,  186 },   /* alt 159, falsch 14300 (max Bucket 186), echt 0 (min Bucket -1) */
    { 0x6000,  8,  47,  192 },   /* alt 178, falsch 13377 (max Bucket 192), echt 0 (min Bucket -1) */
    { 0x6000,  8,  50,  234 },   /* alt 172, falsch 135131 (max Bucket 234), echt 99493 (min Bucket 133), echt teils nicht mehr verdeckt */
    { 0x6000,  8,  51,  233 },   /* alt 190, falsch 48111 (max Bucket 233), echt 29864 (min Bucket 129), echt teils nicht mehr verdeckt */
    { 0x6000,  8,  52,  243 },   /* alt 203, falsch 51692 (max Bucket 243), echt 4839 (min Bucket 185), echt teils nicht mehr verdeckt */
    { 0x6000,  8,  54,  213 },   /* alt 176, falsch 16529 (max Bucket 213), echt 232 (min Bucket 151), echt teils nicht mehr verdeckt */
    { 0x6000,  8,  57,  239 },   /* alt 220, falsch 18354 (max Bucket 239), echt 0 (min Bucket -1) */
    { 0x6000,  8,  58,  234 },   /* alt 198, falsch 16120 (max Bucket 234), echt 0 (min Bucket -1) */
    { 0x6000,  8,  75,  187 },   /* alt 172, falsch 5890 (max Bucket 187), echt 1182 (min Bucket 128), echt teils nicht mehr verdeckt */
    { 0x6000,  8,  76,  188 },   /* alt 180, falsch 7152 (max Bucket 188), echt 755 (min Bucket 128), echt teils nicht mehr verdeckt */
    { 0x6000,  8,  77,  222 },   /* alt 195, falsch 9506 (max Bucket 222), echt 917 (min Bucket 193), echt teils nicht mehr verdeckt */
    { 0x6000,  8,  79,  234 },   /* alt 205, falsch 9745 (max Bucket 234), echt 168 (min Bucket 193), echt teils nicht mehr verdeckt */
    { 0x6000,  8,  84,  234 },   /* alt 229, falsch 7731 (max Bucket 234), echt 0 (min Bucket -1) */
    { 0x6000,  8,  88,  211 },   /* alt 202, falsch 7081 (max Bucket 211), echt 67 (min Bucket 230) */
    { 0x6000,  8,  91,  208 },   /* alt 172, falsch 11379 (max Bucket 208), echt 174 (min Bucket 245) */
    { 0x6000,  8,  92,  188 },   /* alt 167, falsch 7426 (max Bucket 188), echt 4 (min Bucket 253) */
    { 0x6000,  8,  93,  201 },   /* alt 159, falsch 69307 (max Bucket 201), echt 1923 (min Bucket 234) */
    { 0x6000,  8,  94,  242 },   /* alt 216, falsch 9417 (max Bucket 242), echt 0 (min Bucket -1) */
    { 0x6000,  8,  96,  160 },   /* alt 159, falsch 30913 (max Bucket 160), echt 678 (min Bucket 152), echt teils nicht mehr verdeckt */
    { 0x6000,  8,  97,  214 },   /* alt 192, falsch 8989 (max Bucket 214), echt 0 (min Bucket -1) */
    { 0x6000,  8,  98,  191 },   /* alt 167, falsch 10139 (max Bucket 191), echt 44 (min Bucket 154), echt teils nicht mehr verdeckt */
    { 0x6000,  8,  99,  191 },   /* alt 186, falsch 6824 (max Bucket 191), echt 0 (min Bucket -1) */
    { 0x6000,  8, 100,  196 },   /* alt 175, falsch 10260 (max Bucket 196), echt 0 (min Bucket -1) */
    { 0x6000,  8, 101,  207 },   /* alt 182, falsch 9508 (max Bucket 207), echt 0 (min Bucket -1) */
    { 0x6000,  8, 102,  188 },   /* alt 185, falsch 5390 (max Bucket 188), echt 0 (min Bucket -1) */
    { 0x6000,  8, 103,  185 },   /* alt 159, falsch 42878 (max Bucket 185), echt 0 (min Bucket -1) */
    { 0x6000,  8, 104,  191 },   /* alt 163, falsch 15210 (max Bucket 191), echt 12 (min Bucket 166), echt teils nicht mehr verdeckt */
    { 0x6000,  9,   0,  369 },   /* alt 238, falsch 1882003 (max Bucket 369), echt 178825 (min Bucket 275), echt teils nicht mehr verdeckt */
    { 0x6000,  9,   1,  280 },   /* alt 238, falsch 353507 (max Bucket 280), echt 0 (min Bucket -1) */
    { 0x6000,  9,   2,  273 },   /* alt 238, falsch 119697 (max Bucket 273), echt 10948 (min Bucket 337) */
    { 0x6000,  9,   3,  282 },   /* alt 238, falsch 54599 (max Bucket 282), echt 0 (min Bucket -1) */
    { 0x6000,  9,   4,  273 },   /* alt 238, falsch 31897 (max Bucket 273), echt 145 (min Bucket 343) */
    { 0x6000,  9,   5,  265 },   /* alt 238, falsch 22154 (max Bucket 265), echt 7085 (min Bucket 333) */
    { 0x6000,  9,   6,  360 },   /* alt 238, falsch 22159 (max Bucket 360), echt 3215 (min Bucket 288), echt teils nicht mehr verdeckt */
    { 0x6000,  9,   7,  256 },   /* alt 238, falsch 18185 (max Bucket 256), echt 4636 (min Bucket 325) */
    { 0x6000,  9,   8,  281 },   /* alt 238, falsch 16986 (max Bucket 281), echt 0 (min Bucket -1) */
    { 0x6000,  9,   9,  271 },   /* alt 238, falsch 214386 (max Bucket 271), echt 67487 (min Bucket 329) */
    { 0x6000,  9,  10,  263 },   /* alt 238, falsch 32341 (max Bucket 263), echt 12437 (min Bucket 328) */
    { 0x6000,  9,  12,  285 },   /* alt 238, falsch 433310 (max Bucket 285), echt 154 (min Bucket 342) */
    { 0x6000,  9,  13,  275 },   /* alt 238, falsch 110784 (max Bucket 275), echt 22374 (min Bucket 338) */
    { 0x6000,  9,  14,  278 },   /* alt 238, falsch 86842 (max Bucket 278), echt 0 (min Bucket -1) */
    { 0x6000,  9,  15,  278 },   /* alt 238, falsch 44689 (max Bucket 278), echt 0 (min Bucket -1) */
    { 0x6000,  9,  16,  268 },   /* alt 238, falsch 25492 (max Bucket 268), echt 10134 (min Bucket 337) */
    { 0x6000,  9,  17,  280 },   /* alt 238, falsch 15530 (max Bucket 280), echt 0 (min Bucket -1) */
    { 0x6000,  9,  22,  266 },   /* alt 238, falsch 12460 (max Bucket 266), echt 4339 (min Bucket 335) */
    { 0x6000,  9,  23,  266 },   /* alt 238, falsch 11049 (max Bucket 266), echt 3316 (min Bucket 336) */
    { 0x6000,  9,  24,  270 },   /* alt 238, falsch 10565 (max Bucket 270), echt 1666 (min Bucket 341) */
    { 0x6000,  9,  25,  270 },   /* alt 238, falsch 11999 (max Bucket 270), echt 1379 (min Bucket 341) */
    { 0x6000,  9,  26,  275 },   /* alt 238, falsch 16199 (max Bucket 275), echt 0 (min Bucket -1) */
    { 0x6000,  9,  27,  275 },   /* alt 238, falsch 18124 (max Bucket 275), echt 0 (min Bucket -1) */
    { 0x6000,  9,  28,  273 },   /* alt 238, falsch 4861 (max Bucket 273), echt 0 (min Bucket -1) */
    { 0x6000,  9,  29,  279 },   /* alt 238, falsch 9201 (max Bucket 279), echt 0 (min Bucket -1) */
    { 0x6000,  9,  30,  279 },   /* alt 238, falsch 5570 (max Bucket 279), echt 0 (min Bucket -1) */
    { 0x6000,  9,  31,  282 },   /* alt 238, falsch 9187 (max Bucket 282), echt 0 (min Bucket -1) */
    { 0x6000,  9,  32,  281 },   /* alt 238, falsch 5731 (max Bucket 281), echt 0 (min Bucket -1) */
    { 0x6000,  9,  33,  278 },   /* alt 238, falsch 18029 (max Bucket 278), echt 0 (min Bucket -1) */
    { 0x6000,  9,  34,  278 },   /* alt 238, falsch 12602 (max Bucket 278), echt 0 (min Bucket -1) */
    { 0x6001,  3,   7,  141 },   /* alt 0, falsch 69199 (max Bucket 141), echt 0 (min Bucket -1) */
    { 0x6001,  3,   8,  118 },   /* alt 0, falsch 1877 (max Bucket 118), echt 0 (min Bucket -1) */
    { 0x6001,  5,   0,  199 },   /* alt 155, falsch 866683 (max Bucket 199), echt 447184 (min Bucket 218) */
    { 0x6001,  5,   1,  183 },   /* alt 155, falsch 104538 (max Bucket 183), echt 67336 (min Bucket 216) */
    { 0x6001,  5,   3,  176 },   /* alt 166, falsch 47911 (max Bucket 176), echt 0 (min Bucket -1) */
    { 0x6001,  5,  13,  189 },   /* alt 129, falsch 699892 (max Bucket 189), echt 194533 (min Bucket 77), echt teils nicht mehr verdeckt */
    { 0x6001,  5,  14,  165 },   /* alt 129, falsch 74118 (max Bucket 165), echt 524 (min Bucket 93), echt teils nicht mehr verdeckt */
    { 0x6001,  5,  15,  154 },   /* alt 129, falsch 27227 (max Bucket 154), echt 0 (min Bucket -1) */
    { 0x6001,  5,  16,  174 },   /* alt 129, falsch 22932 (max Bucket 174), echt 75 (min Bucket 92), echt teils nicht mehr verdeckt */
    { 0x6001,  5,  17,  181 },   /* alt 129, falsch 12101 (max Bucket 181), echt 0 (min Bucket -1) */
    { 0x6001,  5,  18,  159 },   /* alt 129, falsch 9929 (max Bucket 159), echt 0 (min Bucket -1) */
    { 0x6001,  5,  19,  234 },   /* alt 129, falsch 370258 (max Bucket 234), echt 329166 (min Bucket 70), echt teils nicht mehr verdeckt */
    { 0x6001,  5,  20,  209 },   /* alt 129, falsch 56856 (max Bucket 209), echt 4708 (min Bucket 80), echt teils nicht mehr verdeckt */
    { 0x6001,  5,  21,  200 },   /* alt 129, falsch 17836 (max Bucket 200), echt 0 (min Bucket -1) */
    { 0x6001,  5,  22,  219 },   /* alt 129, falsch 19815 (max Bucket 219), echt 0 (min Bucket -1) */
    { 0x6001,  5,  23,  201 },   /* alt 129, falsch 4231 (max Bucket 201), echt 13679 (min Bucket 87), echt teils nicht mehr verdeckt */
    { 0x6001,  5,  25,  215 },   /* alt 129, falsch 6259 (max Bucket 215), echt 5317 (min Bucket 87), echt teils nicht mehr verdeckt */
    { 0x6001,  5,  29,  215 },   /* alt 129, falsch 5177 (max Bucket 215), echt 23 (min Bucket 87), echt teils nicht mehr verdeckt */
    { 0x6001,  5,  33,  236 },   /* alt 129, falsch 107814 (max Bucket 236), echt 158620 (min Bucket 70), echt teils nicht mehr verdeckt */
    { 0x6001,  5,  34,  237 },   /* alt 129, falsch 47969 (max Bucket 237), echt 1792 (min Bucket 72), echt teils nicht mehr verdeckt */
    { 0x6001,  5,  35,  161 },   /* alt 129, falsch 4231 (max Bucket 161), echt 3813 (min Bucket 70), echt teils nicht mehr verdeckt */
    { 0x6001,  5,  36,  238 },   /* alt 129, falsch 17696 (max Bucket 238), echt 0 (min Bucket -1) */
    { 0x6001,  5,  37,  227 },   /* alt 129, falsch 11822 (max Bucket 227), echt 0 (min Bucket -1) */
    { 0x6001,  5,  38,  256 },   /* alt 212, falsch 244162 (max Bucket 256), echt 106978 (min Bucket 144), echt teils nicht mehr verdeckt */
    { 0x6001,  5,  41,  172 },   /* alt 143, falsch 102504 (max Bucket 172), echt 0 (min Bucket -1) */
    { 0x6001,  5,  65,  215 },   /* alt 129, falsch 4831 (max Bucket 215), echt 0 (min Bucket -1) */
    { 0x6001,  5,  66,  239 },   /* alt 212, falsch 8034 (max Bucket 239), echt 5146 (min Bucket 143), echt teils nicht mehr verdeckt */
    { 0x6001,  5,  70,  222 },   /* alt 129, falsch 5895 (max Bucket 222), echt 0 (min Bucket -1) */
    { 0x6001,  5,  73,  227 },   /* alt 129, falsch 6631 (max Bucket 227), echt 0 (min Bucket -1) */
    { 0x6001,  5,  76,  233 },   /* alt 129, falsch 7320 (max Bucket 233), echt 0 (min Bucket -1) */
    { 0x6001,  5,  80,  173 },   /* alt 171, falsch 6998 (max Bucket 173), echt 177 (min Bucket 238) */
    { 0x6001,  5,  82,  238 },   /* alt 129, falsch 9363 (max Bucket 238), echt 0 (min Bucket -1) */
    { 0x6001,  5,  83,  230 },   /* alt 129, falsch 11715 (max Bucket 230), echt 0 (min Bucket -1) */
    { 0x6001,  5,  84,  216 },   /* alt 129, falsch 7625 (max Bucket 216), echt 0 (min Bucket -1) */
    { 0x6001,  5,  86,  206 },   /* alt 129, falsch 7433 (max Bucket 206), echt 0 (min Bucket -1) */
    { 0x6001,  5,  88,  197 },   /* alt 129, falsch 10669 (max Bucket 197), echt 0 (min Bucket -1) */
    { 0x6001,  5,  89,  175 },   /* alt 168, falsch 4545 (max Bucket 175), echt 0 (min Bucket -1) */
    { 0x6001,  5,  90,  177 },   /* alt 167, falsch 5218 (max Bucket 177), echt 0 (min Bucket -1) */
    { 0x6001,  5,  91,  179 },   /* alt 165, falsch 5897 (max Bucket 179), echt 0 (min Bucket -1) */
    { 0x6001,  5,  92,  181 },   /* alt 163, falsch 6600 (max Bucket 181), echt 0 (min Bucket -1) */
    { 0x6001,  5,  93,  181 },   /* alt 162, falsch 6944 (max Bucket 181), echt 0 (min Bucket -1) */
    { 0x6001,  5,  94,  186 },   /* alt 129, falsch 6755 (max Bucket 186), echt 0 (min Bucket -1) */
    { 0x6001,  5,  95,  178 },   /* alt 129, falsch 6513 (max Bucket 178), echt 0 (min Bucket -1) */
    { 0x6001,  5,  96,  178 },   /* alt 159, falsch 4927 (max Bucket 178), echt 0 (min Bucket -1) */
    { 0x6001,  5,  97,  178 },   /* alt 158, falsch 5276 (max Bucket 178), echt 0 (min Bucket -1) */
    { 0x6001,  5,  98,  178 },   /* alt 157, falsch 5927 (max Bucket 178), echt 0 (min Bucket -1) */
    { 0x6001,  5,  99,  178 },   /* alt 155, falsch 17128 (max Bucket 178), echt 0 (min Bucket -1) */
    { 0x6001,  5, 100,  172 },   /* alt 129, falsch 8806 (max Bucket 172), echt 0 (min Bucket -1) */
    { 0x6001,  5, 101,  163 },   /* alt 129, falsch 6038 (max Bucket 163), echt 0 (min Bucket -1) */
    { 0x6001,  5, 102,  157 },   /* alt 129, falsch 5934 (max Bucket 157), echt 0 (min Bucket -1) */
    { 0x6001,  5, 103,  152 },   /* alt 130, falsch 7098 (max Bucket 152), echt 0 (min Bucket -1) */
    { 0x6001,  5, 104,  150 },   /* alt 131, falsch 5928 (max Bucket 150), echt 0 (min Bucket -1) */
    { 0x6001,  7,   0,  207 },   /* alt 124, falsch 533925 (max Bucket 207), echt 0 (min Bucket -1) */
    { 0x6001,  7,   1,  160 },   /* alt 124, falsch 41005 (max Bucket 160), echt 0 (min Bucket -1) */
    { 0x6001,  7,   2,  211 },   /* alt 150, falsch 87606 (max Bucket 211), echt 0 (min Bucket -1) */
    { 0x6001,  7,   4,  154 },   /* alt 124, falsch 38309 (max Bucket 154), echt 0 (min Bucket -1) */
    { 0x6001,  7,   5,  211 },   /* alt 124, falsch 12531 (max Bucket 211), echt 0 (min Bucket -1) */
    { 0x6001,  7,   7,  179 },   /* alt 145, falsch 22436 (max Bucket 179), echt 0 (min Bucket -1) */
    { 0x6001,  7,   8,  211 },   /* alt 170, falsch 21565 (max Bucket 211), echt 0 (min Bucket -1) */
    { 0x6001,  7,  11,  154 },   /* alt 124, falsch 10534 (max Bucket 154), echt 0 (min Bucket -1) */
    { 0x6001,  7,  12,  150 },   /* alt 125, falsch 9178 (max Bucket 150), echt 0 (min Bucket -1) */
    { 0x6001,  7,  13,  145 },   /* alt 124, falsch 204956 (max Bucket 145), echt 94381 (min Bucket 98), echt teils nicht mehr verdeckt */
    { 0x6001,  7,  16,  143 },   /* alt 124, falsch 39877 (max Bucket 143), echt 0 (min Bucket -1) */
    { 0x6001,  7,  22,  147 },   /* alt 124, falsch 8480 (max Bucket 147), echt 774 (min Bucket 98), echt teils nicht mehr verdeckt */
    { 0x6001,  7,  28,  150 },   /* alt 124, falsch 15470 (max Bucket 150), echt 10439 (min Bucket 99), echt teils nicht mehr verdeckt */
    { 0x6001,  7,  42,  149 },   /* alt 124, falsch 281276 (max Bucket 149), echt 0 (min Bucket -1) */
    { 0x6001,  7,  43,  152 },   /* alt 124, falsch 31606 (max Bucket 152), echt 0 (min Bucket -1) */
    { 0x6001,  7,  44,  147 },   /* alt 124, falsch 41171 (max Bucket 147), echt 0 (min Bucket -1) */
    { 0x6001,  7,  46,  152 },   /* alt 124, falsch 21975 (max Bucket 152), echt 0 (min Bucket -1) */
    { 0x6001,  7,  48,  153 },   /* alt 124, falsch 6710 (max Bucket 153), echt 0 (min Bucket -1) */
    { 0x6001,  7,  49,  281 },   /* alt 148, falsch 264781 (max Bucket 281), echt 46171 (min Bucket 224), echt teils nicht mehr verdeckt */
    { 0x6001,  7,  50,  277 },   /* alt 178, falsch 69020 (max Bucket 277), echt 35698 (min Bucket 175), echt teils nicht mehr verdeckt */
    { 0x6001,  7,  51,  209 },   /* alt 124, falsch 3709 (max Bucket 209), echt 0 (min Bucket -1) */
    { 0x6001,  7,  52,  276 },   /* alt 209, falsch 59501 (max Bucket 276), echt 0 (min Bucket -1) */
    { 0x6001,  7,  54,  266 },   /* alt 158, falsch 3396 (max Bucket 266), echt 8297 (min Bucket 177), echt teils nicht mehr verdeckt */
    { 0x6001,  7,  55,  213 },   /* alt 147, falsch 13462 (max Bucket 213), echt 0 (min Bucket -1) */
    { 0x6001,  7,  74,  262 },   /* alt 209, falsch 6332 (max Bucket 262), echt 251 (min Bucket 176), echt teils nicht mehr verdeckt */
    { 0x6001,  7,  75,  263 },   /* alt 198, falsch 13462 (max Bucket 263), echt 278 (min Bucket 177), echt teils nicht mehr verdeckt */
    { 0x6001,  7,  76,  275 },   /* alt 234, falsch 24719 (max Bucket 275), echt 257 (min Bucket 175), echt teils nicht mehr verdeckt */
    { 0x6001,  7,  79,  267 },   /* alt 262, falsch 311 (max Bucket 267), echt 0 (min Bucket -1) */
    { 0x6001,  7,  80,  273 },   /* alt 249, falsch 5332 (max Bucket 273), echt 0 (min Bucket -1) */
    { 0x6001,  7,  84,  211 },   /* alt 180, falsch 7220 (max Bucket 211), echt 0 (min Bucket -1) */
    { 0x6001,  7,  86,  194 },   /* alt 167, falsch 8119 (max Bucket 194), echt 0 (min Bucket -1) */
    { 0x6001,  7,  91,  179 },   /* alt 152, falsch 8164 (max Bucket 179), echt 0 (min Bucket -1) */
    { 0x6001,  7,  94,  165 },   /* alt 141, falsch 10173 (max Bucket 165), echt 0 (min Bucket -1) */
    { 0x6001,  7,  96,  154 },   /* alt 124, falsch 19112 (max Bucket 154), echt 0 (min Bucket -1) */
    { 0x6001,  7,  98,  157 },   /* alt 124, falsch 20265 (max Bucket 157), echt 0 (min Bucket -1) */
    { 0x6001,  7,  99,  149 },   /* alt 124, falsch 14072 (max Bucket 149), echt 5158 (min Bucket 99), echt teils nicht mehr verdeckt */
    { 0x6001,  7, 100,  148 },   /* alt 124, falsch 7562 (max Bucket 148), echt 3914 (min Bucket 99), echt teils nicht mehr verdeckt */
    { 0x6001,  7, 104,  147 },   /* alt 127, falsch 13304 (max Bucket 147), echt 0 (min Bucket -1) */
    { 0x6001,  8,  10,  202 },   /* alt 159, falsch 241779 (max Bucket 202), echt 0 (min Bucket -1) */
    { 0x6001,  8,  11,  206 },   /* alt 159, falsch 47782 (max Bucket 206), echt 817 (min Bucket 234) */
    { 0x6001,  8,  12,  208 },   /* alt 164, falsch 71865 (max Bucket 208), echt 0 (min Bucket -1) */
    { 0x6001,  8,  13,  182 },   /* alt 159, falsch 81995 (max Bucket 182), echt 0 (min Bucket -1) */
    { 0x6001,  8,  14,  191 },   /* alt 159, falsch 72154 (max Bucket 191), echt 0 (min Bucket -1) */
    { 0x6001,  8,  15,  186 },   /* alt 159, falsch 5545 (max Bucket 186), echt 1832 (min Bucket 231) */
    { 0x6001,  8,  16,  213 },   /* alt 180, falsch 35172 (max Bucket 213), echt 0 (min Bucket -1) */
    { 0x6001,  8,  17,  204 },   /* alt 159, falsch 41933 (max Bucket 204), echt 0 (min Bucket -1) */
    { 0x6001,  8,  18,  199 },   /* alt 159, falsch 15243 (max Bucket 199), echt 0 (min Bucket -1) */
    { 0x6001,  8,  19,  168 },   /* alt 159, falsch 12808 (max Bucket 168), echt 0 (min Bucket -1) */
    { 0x6001,  8,  22,  163 },   /* alt 159, falsch 6756 (max Bucket 163), echt 0 (min Bucket -1) */
    { 0x6001,  8,  23,  207 },   /* alt 175, falsch 16798 (max Bucket 207), echt 0 (min Bucket -1) */
    { 0x6001,  8,  24,  183 },   /* alt 159, falsch 17091 (max Bucket 183), echt 0 (min Bucket -1) */
    { 0x6001,  8,  25,  212 },   /* alt 193, falsch 12370 (max Bucket 212), echt 0 (min Bucket -1) */
    { 0x6001,  8,  26,  188 },   /* alt 162, falsch 14219 (max Bucket 188), echt 0 (min Bucket -1) */
    { 0x6001,  8,  27,  185 },   /* alt 159, falsch 17449 (max Bucket 185), echt 0 (min Bucket -1) */
    { 0x6001,  8,  28,  184 },   /* alt 159, falsch 14737 (max Bucket 184), echt 0 (min Bucket -1) */
    { 0x6001,  8,  29,  191 },   /* alt 162, falsch 4997 (max Bucket 191), echt 327191 (min Bucket 205) */
    { 0x6001,  8,  37,  179 },   /* alt 159, falsch 666939 (max Bucket 179), echt 8117 (min Bucket 153), echt teils nicht mehr verdeckt */
    { 0x6001,  8,  38,  184 },   /* alt 159, falsch 106996 (max Bucket 184), echt 0 (min Bucket -1) */
    { 0x6001,  8,  40,  196 },   /* alt 165, falsch 50621 (max Bucket 196), echt 4700 (min Bucket 154), echt teils nicht mehr verdeckt */
    { 0x6001,  8,  41,  183 },   /* alt 159, falsch 33091 (max Bucket 183), echt 0 (min Bucket -1) */
    { 0x6001,  8,  44,  167 },   /* alt 159, falsch 21256 (max Bucket 167), echt 0 (min Bucket -1) */
    { 0x6001,  8,  45,  187 },   /* alt 159, falsch 12992 (max Bucket 187), echt 969 (min Bucket 165), echt teils nicht mehr verdeckt */
    { 0x6001,  8,  46,  186 },   /* alt 159, falsch 14300 (max Bucket 186), echt 0 (min Bucket -1) */
    { 0x6001,  8,  47,  192 },   /* alt 178, falsch 13377 (max Bucket 192), echt 0 (min Bucket -1) */
    { 0x6001,  8,  50,  234 },   /* alt 172, falsch 135131 (max Bucket 234), echt 99493 (min Bucket 133), echt teils nicht mehr verdeckt */
    { 0x6001,  8,  51,  233 },   /* alt 190, falsch 48111 (max Bucket 233), echt 29864 (min Bucket 129), echt teils nicht mehr verdeckt */
    { 0x6001,  8,  52,  243 },   /* alt 203, falsch 51692 (max Bucket 243), echt 4839 (min Bucket 185), echt teils nicht mehr verdeckt */
    { 0x6001,  8,  54,  213 },   /* alt 176, falsch 16529 (max Bucket 213), echt 232 (min Bucket 151), echt teils nicht mehr verdeckt */
    { 0x6001,  8,  57,  239 },   /* alt 220, falsch 18354 (max Bucket 239), echt 0 (min Bucket -1) */
    { 0x6001,  8,  58,  234 },   /* alt 198, falsch 16120 (max Bucket 234), echt 0 (min Bucket -1) */
    { 0x6001,  8,  75,  187 },   /* alt 172, falsch 5890 (max Bucket 187), echt 1182 (min Bucket 128), echt teils nicht mehr verdeckt */
    { 0x6001,  8,  76,  188 },   /* alt 180, falsch 7152 (max Bucket 188), echt 755 (min Bucket 128), echt teils nicht mehr verdeckt */
    { 0x6001,  8,  77,  222 },   /* alt 195, falsch 9506 (max Bucket 222), echt 917 (min Bucket 193), echt teils nicht mehr verdeckt */
    { 0x6001,  8,  79,  234 },   /* alt 205, falsch 9745 (max Bucket 234), echt 168 (min Bucket 193), echt teils nicht mehr verdeckt */
    { 0x6001,  8,  84,  234 },   /* alt 229, falsch 7731 (max Bucket 234), echt 0 (min Bucket -1) */
    { 0x6001,  8,  88,  211 },   /* alt 202, falsch 7081 (max Bucket 211), echt 67 (min Bucket 230) */
    { 0x6001,  8,  91,  208 },   /* alt 172, falsch 11379 (max Bucket 208), echt 174 (min Bucket 245) */
    { 0x6001,  8,  92,  188 },   /* alt 167, falsch 7426 (max Bucket 188), echt 4 (min Bucket 253) */
    { 0x6001,  8,  93,  201 },   /* alt 159, falsch 69307 (max Bucket 201), echt 1923 (min Bucket 234) */
    { 0x6001,  8,  94,  242 },   /* alt 216, falsch 9417 (max Bucket 242), echt 0 (min Bucket -1) */
    { 0x6001,  8,  96,  160 },   /* alt 159, falsch 30913 (max Bucket 160), echt 678 (min Bucket 152), echt teils nicht mehr verdeckt */
    { 0x6001,  8,  97,  214 },   /* alt 192, falsch 8989 (max Bucket 214), echt 0 (min Bucket -1) */
    { 0x6001,  8,  98,  191 },   /* alt 167, falsch 10139 (max Bucket 191), echt 44 (min Bucket 154), echt teils nicht mehr verdeckt */
    { 0x6001,  8,  99,  191 },   /* alt 186, falsch 6824 (max Bucket 191), echt 0 (min Bucket -1) */
    { 0x6001,  8, 100,  196 },   /* alt 175, falsch 10260 (max Bucket 196), echt 0 (min Bucket -1) */
    { 0x6001,  8, 101,  207 },   /* alt 182, falsch 9508 (max Bucket 207), echt 0 (min Bucket -1) */
    { 0x6001,  8, 102,  188 },   /* alt 185, falsch 5390 (max Bucket 188), echt 0 (min Bucket -1) */
    { 0x6001,  8, 103,  185 },   /* alt 159, falsch 42878 (max Bucket 185), echt 0 (min Bucket -1) */
    { 0x6001,  8, 104,  191 },   /* alt 163, falsch 15210 (max Bucket 191), echt 12 (min Bucket 166), echt teils nicht mehr verdeckt */
    { 0x6001,  9,   0,  369 },   /* alt 238, falsch 1882003 (max Bucket 369), echt 178825 (min Bucket 275), echt teils nicht mehr verdeckt */
    { 0x6001,  9,   1,  280 },   /* alt 238, falsch 353507 (max Bucket 280), echt 0 (min Bucket -1) */
    { 0x6001,  9,   2,  273 },   /* alt 238, falsch 119697 (max Bucket 273), echt 10948 (min Bucket 337) */
    { 0x6001,  9,   3,  282 },   /* alt 238, falsch 54599 (max Bucket 282), echt 0 (min Bucket -1) */
    { 0x6001,  9,   4,  273 },   /* alt 238, falsch 31897 (max Bucket 273), echt 145 (min Bucket 343) */
    { 0x6001,  9,   5,  265 },   /* alt 238, falsch 22154 (max Bucket 265), echt 7085 (min Bucket 333) */
    { 0x6001,  9,   6,  360 },   /* alt 238, falsch 22159 (max Bucket 360), echt 3215 (min Bucket 288), echt teils nicht mehr verdeckt */
    { 0x6001,  9,   7,  256 },   /* alt 238, falsch 18185 (max Bucket 256), echt 4636 (min Bucket 325) */
    { 0x6001,  9,   8,  281 },   /* alt 238, falsch 16986 (max Bucket 281), echt 0 (min Bucket -1) */
    { 0x6001,  9,   9,  271 },   /* alt 238, falsch 214386 (max Bucket 271), echt 67487 (min Bucket 329) */
    { 0x6001,  9,  10,  263 },   /* alt 238, falsch 32341 (max Bucket 263), echt 12437 (min Bucket 328) */
    { 0x6001,  9,  12,  285 },   /* alt 238, falsch 433310 (max Bucket 285), echt 154 (min Bucket 342) */
    { 0x6001,  9,  13,  275 },   /* alt 238, falsch 110784 (max Bucket 275), echt 22374 (min Bucket 338) */
    { 0x6001,  9,  14,  278 },   /* alt 238, falsch 86842 (max Bucket 278), echt 0 (min Bucket -1) */
    { 0x6001,  9,  15,  278 },   /* alt 238, falsch 44689 (max Bucket 278), echt 0 (min Bucket -1) */
    { 0x6001,  9,  16,  268 },   /* alt 238, falsch 25492 (max Bucket 268), echt 10134 (min Bucket 337) */
    { 0x6001,  9,  17,  280 },   /* alt 238, falsch 15530 (max Bucket 280), echt 0 (min Bucket -1) */
    { 0x6001,  9,  22,  266 },   /* alt 238, falsch 12460 (max Bucket 266), echt 4339 (min Bucket 335) */
    { 0x6001,  9,  23,  266 },   /* alt 238, falsch 11049 (max Bucket 266), echt 3316 (min Bucket 336) */
    { 0x6001,  9,  24,  270 },   /* alt 238, falsch 10565 (max Bucket 270), echt 1666 (min Bucket 341) */
    { 0x6001,  9,  25,  270 },   /* alt 238, falsch 11999 (max Bucket 270), echt 1379 (min Bucket 341) */
    { 0x6001,  9,  26,  275 },   /* alt 238, falsch 16199 (max Bucket 275), echt 0 (min Bucket -1) */
    { 0x6001,  9,  27,  275 },   /* alt 238, falsch 18124 (max Bucket 275), echt 0 (min Bucket -1) */
    { 0x6001,  9,  28,  273 },   /* alt 238, falsch 4861 (max Bucket 273), echt 0 (min Bucket -1) */
    { 0x6001,  9,  29,  279 },   /* alt 238, falsch 9201 (max Bucket 279), echt 0 (min Bucket -1) */
    { 0x6001,  9,  30,  279 },   /* alt 238, falsch 5570 (max Bucket 279), echt 0 (min Bucket -1) */
    { 0x6001,  9,  31,  282 },   /* alt 238, falsch 9187 (max Bucket 282), echt 0 (min Bucket -1) */
    { 0x6001,  9,  32,  281 },   /* alt 238, falsch 5731 (max Bucket 281), echt 0 (min Bucket -1) */
    { 0x6001,  9,  33,  278 },   /* alt 238, falsch 18029 (max Bucket 278), echt 0 (min Bucket -1) */
    { 0x6001,  9,  34,  278 },   /* alt 238, falsch 12602 (max Bucket 278), echt 0 (min Bucket -1) */
    { 0x6010,  2,  12,  107 },   /* alt 106, falsch 693 (max Bucket 107), echt 0 (min Bucket -1) */
    { 0x6010,  6,   0,  102 },   /* alt 100, falsch 419709 (max Bucket 102), echt 113739 (min Bucket 212) */
    { 0x6010,  6,   1,   99 },   /* alt 96, falsch 67274 (max Bucket 99), echt 0 (min Bucket -1) */
    { 0x6010,  6,   2,   98 },   /* alt 96, falsch 64634 (max Bucket 98), echt 37175 (min Bucket 230) */
    { 0x6010,  6,   3,   99 },   /* alt 98, falsch 18500 (max Bucket 99), echt 4164 (min Bucket 245) */
    { 0x6010,  6,   4,   95 },   /* alt 94, falsch 14683 (max Bucket 95), echt 0 (min Bucket -1) */
    { 0x6010,  6,   5,  103 },   /* alt 100, falsch 10970 (max Bucket 103), echt 0 (min Bucket -1) */
    { 0x6010,  6,   6,   99 },   /* alt 97, falsch 9571 (max Bucket 99), echt 5443 (min Bucket 232) */
    { 0x6010,  6,   7,   95 },   /* alt 94, falsch 8448 (max Bucket 95), echt 4897 (min Bucket 229) */
    { 0x6010,  6,   8,   99 },   /* alt 98, falsch 7100 (max Bucket 99), echt 3803 (min Bucket 244) */
    { 0x6010,  6,   9,   99 },   /* alt 98, falsch 6987 (max Bucket 99), echt 0 (min Bucket -1) */
    { 0x6010,  6,  10,   95 },   /* alt 94, falsch 1306 (max Bucket 95), echt 735 (min Bucket 229) */
    { 0x6010,  6,  11,   99 },   /* alt 98, falsch 917 (max Bucket 99), echt 478 (min Bucket 245) */
    { 0x6010,  6,  12,   99 },   /* alt 98, falsch 1349 (max Bucket 99), echt 717 (min Bucket 245) */
    { 0x6010,  6,  14,   99 },   /* alt 98, falsch 2686 (max Bucket 99), echt 1424 (min Bucket 244) */
    { 0x6010,  6,  15,   99 },   /* alt 98, falsch 5732 (max Bucket 99), echt 0 (min Bucket -1) */
    { 0x6010,  6,  16,  102 },   /* alt 100, falsch 502 (max Bucket 102), echt 0 (min Bucket -1) */
    { 0x6010,  6,  18,   95 },   /* alt 94, falsch 466 (max Bucket 95), echt 0 (min Bucket -1) */
    { 0x6010,  6,  19,  100 },   /* alt 98, falsch 3607 (max Bucket 100), echt 0 (min Bucket -1) */
    { 0x6010,  6,  20,   97 },   /* alt 96, falsch 6014 (max Bucket 97), echt 0 (min Bucket -1) */
    { 0x6010, 11,  13,   13 },   /* alt 0, falsch 37701 (max Bucket 13), echt 710 (min Bucket 130) */
    { 0x6010, 11,  49,   12 },   /* alt 0, falsch 2073 (max Bucket 12), echt 13126 (min Bucket 136) */
    { 0x6010, 11,  52,   12 },   /* alt 0, falsch 606 (max Bucket 12), echt 1935 (min Bucket 150) */
    { 0x6010, 12,  53,   39 },   /* alt 0, falsch 118710 (max Bucket 39), echt 0 (min Bucket -1) */
    { 0x6010, 12,  57,   38 },   /* alt 0, falsch 2631 (max Bucket 38), echt 0 (min Bucket -1) */
    { 0x6010, 12,  61,   48 },   /* alt 0, falsch 15525 (max Bucket 48), echt 0 (min Bucket -1) */
    { 0x6010, 12,  62,   48 },   /* alt 0, falsch 11776 (max Bucket 48), echt 0 (min Bucket -1) */
    { 0x6010, 12,  64,   45 },   /* alt 0, falsch 549 (max Bucket 45), echt 0 (min Bucket -1) */
    { 0x6010, 13,  43,   35 },   /* alt 0, falsch 110090 (max Bucket 35), echt 0 (min Bucket -1) */
    { 0x6010, 13,  54,   35 },   /* alt 0, falsch 644 (max Bucket 35), echt 0 (min Bucket -1) */
    { 0x6010, 13,  56,   36 },   /* alt 0, falsch 1432 (max Bucket 36), echt 0 (min Bucket -1) */
    { 0x6010, 13,  57,   41 },   /* alt 0, falsch 11023 (max Bucket 41), echt 0 (min Bucket -1) */
    { 0x6010, 13,  58,   43 },   /* alt 0, falsch 14870 (max Bucket 43), echt 0 (min Bucket -1) */
    { 0x6010, 13,  59,   46 },   /* alt 0, falsch 4880 (max Bucket 46), echt 0 (min Bucket -1) */
    { 0x6010, 13,  60,   47 },   /* alt 0, falsch 22851 (max Bucket 47), echt 0 (min Bucket -1) */
    { 0x6010, 13,  67,   49 },   /* alt 0, falsch 79066 (max Bucket 49), echt 2908 (min Bucket 170) */
    { 0x6010, 13,  76,   46 },   /* alt 0, falsch 3714 (max Bucket 46), echt 3689 (min Bucket 167) */
    { 0x6010, 13,  77,   46 },   /* alt 0, falsch 1008 (max Bucket 46), echt 830 (min Bucket 165) */
    { 0x6011,  2,  12,  107 },   /* alt 106, falsch 693 (max Bucket 107), echt 0 (min Bucket -1) */
    { 0x6011,  6,   0,  102 },   /* alt 100, falsch 419709 (max Bucket 102), echt 113739 (min Bucket 212) */
    { 0x6011,  6,   1,   99 },   /* alt 96, falsch 67274 (max Bucket 99), echt 0 (min Bucket -1) */
    { 0x6011,  6,   2,   98 },   /* alt 96, falsch 64634 (max Bucket 98), echt 37175 (min Bucket 230) */
    { 0x6011,  6,   3,   99 },   /* alt 98, falsch 18500 (max Bucket 99), echt 4164 (min Bucket 245) */
    { 0x6011,  6,   4,   95 },   /* alt 94, falsch 14683 (max Bucket 95), echt 0 (min Bucket -1) */
    { 0x6011,  6,   5,  103 },   /* alt 100, falsch 10970 (max Bucket 103), echt 0 (min Bucket -1) */
    { 0x6011,  6,   6,   99 },   /* alt 97, falsch 9571 (max Bucket 99), echt 5443 (min Bucket 232) */
    { 0x6011,  6,   7,   95 },   /* alt 94, falsch 8448 (max Bucket 95), echt 4897 (min Bucket 229) */
    { 0x6011,  6,   8,   99 },   /* alt 98, falsch 7100 (max Bucket 99), echt 3803 (min Bucket 244) */
    { 0x6011,  6,   9,   99 },   /* alt 98, falsch 6987 (max Bucket 99), echt 0 (min Bucket -1) */
    { 0x6011,  6,  10,   95 },   /* alt 94, falsch 1306 (max Bucket 95), echt 735 (min Bucket 229) */
    { 0x6011,  6,  11,   99 },   /* alt 98, falsch 917 (max Bucket 99), echt 478 (min Bucket 245) */
    { 0x6011,  6,  12,   99 },   /* alt 98, falsch 1349 (max Bucket 99), echt 717 (min Bucket 245) */
    { 0x6011,  6,  14,   99 },   /* alt 98, falsch 2686 (max Bucket 99), echt 1424 (min Bucket 244) */
    { 0x6011,  6,  15,   99 },   /* alt 98, falsch 5732 (max Bucket 99), echt 0 (min Bucket -1) */
    { 0x6011,  6,  16,  102 },   /* alt 100, falsch 502 (max Bucket 102), echt 0 (min Bucket -1) */
    { 0x6011,  6,  18,   95 },   /* alt 94, falsch 466 (max Bucket 95), echt 0 (min Bucket -1) */
    { 0x6011,  6,  19,  100 },   /* alt 98, falsch 3607 (max Bucket 100), echt 0 (min Bucket -1) */
    { 0x6011,  6,  20,   97 },   /* alt 96, falsch 6014 (max Bucket 97), echt 0 (min Bucket -1) */
    { 0x6011, 11,  13,   13 },   /* alt 0, falsch 37701 (max Bucket 13), echt 710 (min Bucket 130) */
    { 0x6011, 11,  49,   12 },   /* alt 0, falsch 2073 (max Bucket 12), echt 13126 (min Bucket 136) */
    { 0x6011, 11,  52,   12 },   /* alt 0, falsch 606 (max Bucket 12), echt 1935 (min Bucket 150) */
    { 0x6011, 12,  53,   39 },   /* alt 0, falsch 118710 (max Bucket 39), echt 0 (min Bucket -1) */
    { 0x6011, 12,  57,   38 },   /* alt 0, falsch 2631 (max Bucket 38), echt 0 (min Bucket -1) */
    { 0x6011, 12,  61,   48 },   /* alt 0, falsch 15525 (max Bucket 48), echt 0 (min Bucket -1) */
    { 0x6011, 12,  62,   48 },   /* alt 0, falsch 11776 (max Bucket 48), echt 0 (min Bucket -1) */
    { 0x6011, 12,  64,   45 },   /* alt 0, falsch 549 (max Bucket 45), echt 0 (min Bucket -1) */
    { 0x6011, 13,  43,   35 },   /* alt 0, falsch 110090 (max Bucket 35), echt 0 (min Bucket -1) */
    { 0x6011, 13,  54,   35 },   /* alt 0, falsch 644 (max Bucket 35), echt 0 (min Bucket -1) */
    { 0x6011, 13,  56,   36 },   /* alt 0, falsch 1432 (max Bucket 36), echt 0 (min Bucket -1) */
    { 0x6011, 13,  57,   41 },   /* alt 0, falsch 11023 (max Bucket 41), echt 0 (min Bucket -1) */
    { 0x6011, 13,  58,   43 },   /* alt 0, falsch 14870 (max Bucket 43), echt 0 (min Bucket -1) */
    { 0x6011, 13,  59,   46 },   /* alt 0, falsch 4880 (max Bucket 46), echt 0 (min Bucket -1) */
    { 0x6011, 13,  60,   47 },   /* alt 0, falsch 22851 (max Bucket 47), echt 0 (min Bucket -1) */
    { 0x6011, 13,  67,   49 },   /* alt 0, falsch 79066 (max Bucket 49), echt 2908 (min Bucket 170) */
    { 0x6011, 13,  76,   46 },   /* alt 0, falsch 3714 (max Bucket 46), echt 3689 (min Bucket 167) */
    { 0x6011, 13,  77,   46 },   /* alt 0, falsch 1008 (max Bucket 46), echt 830 (min Bucket 165) */
    { 0x6030, 12,   0,   84 },   /* alt 71, falsch 3951 (max Bucket 84), echt 0 (min Bucket -1) */
    { 0x6030, 12,   1,   82 },   /* alt 71, falsch 686 (max Bucket 82), echt 0 (min Bucket -1) */
    { 0x6030, 12,   2,   86 },   /* alt 71, falsch 2525 (max Bucket 86), echt 0 (min Bucket -1) */
    { 0x6030, 12,   4,   83 },   /* alt 71, falsch 660 (max Bucket 83), echt 0 (min Bucket -1) */
    { 0x6030, 12,   5,   86 },   /* alt 71, falsch 832 (max Bucket 86), echt 0 (min Bucket -1) */
    { 0x6030, 12,   6,   85 },   /* alt 71, falsch 789 (max Bucket 85), echt 0 (min Bucket -1) */
    { 0x6030, 12,   7,   85 },   /* alt 71, falsch 670 (max Bucket 85), echt 0 (min Bucket -1) */
    { 0x6030, 12,   9,   83 },   /* alt 71, falsch 659 (max Bucket 83), echt 0 (min Bucket -1) */
    { 0x6031, 12,   0,   84 },   /* alt 71, falsch 3951 (max Bucket 84), echt 0 (min Bucket -1) */
    { 0x6031, 12,   1,   82 },   /* alt 71, falsch 686 (max Bucket 82), echt 0 (min Bucket -1) */
    { 0x6031, 12,   2,   86 },   /* alt 71, falsch 2525 (max Bucket 86), echt 0 (min Bucket -1) */
    { 0x6031, 12,   4,   83 },   /* alt 71, falsch 660 (max Bucket 83), echt 0 (min Bucket -1) */
    { 0x6031, 12,   5,   86 },   /* alt 71, falsch 832 (max Bucket 86), echt 0 (min Bucket -1) */
    { 0x6031, 12,   6,   85 },   /* alt 71, falsch 789 (max Bucket 85), echt 0 (min Bucket -1) */
    { 0x6031, 12,   7,   85 },   /* alt 71, falsch 670 (max Bucket 85), echt 0 (min Bucket -1) */
    { 0x6031, 12,   9,   83 },   /* alt 71, falsch 659 (max Bucket 83), echt 0 (min Bucket -1) */
    { 0x0000, 0xFF, 0xFF, 0 }   /* Ende */
};
