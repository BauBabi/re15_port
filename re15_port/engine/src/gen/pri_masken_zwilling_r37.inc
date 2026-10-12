/* ERZEUGT von re15_port/tools/maske/zwilling_zensus.py - NICHT von Hand aendern.
 * Runde 37 Spur A, Nachbesserung 1 (VERTRAG.md 1.7): Elza-Raum (ungerade Id) -> Leon-Variante, deren
 * Port-Masken (MASKS/ROOM%04X.MSK + _PRI%02d.TIM) gelten, weil Kamerasaetze (RDT+0x24, Satz+0x00..+0x1B je
 * Cut; fov @0x80021e68, pos/Ziel FUN_80053ca4 @0x80053d1c..d48) und Hintergrund je Cut (@0x80021c88..cd8, Raum
 * ohne Varianten-Ziffer)
 * nachweislich gleich sind und die Elza-Variante keine eigene MSK hat. Riegel: unit_r37_pri1100_zwilling. */
{ 0x1001, 0x1000 },   /* RDT verschieden, SCA gleich */
{ 0x1011, 0x1010 },   /* RDT verschieden, SCA gleich */
{ 0x1051, 0x1050 },   /* RDT verschieden, SCA gleich */
{ 0x1061, 0x1060 },   /* RDT bytegleich, SCA gleich */
{ 0x10A1, 0x10A0 },   /* RDT bytegleich, SCA gleich */
{ 0x10C1, 0x10C0 },   /* RDT bytegleich, SCA gleich */
{ 0x10D1, 0x10D0 },   /* RDT verschieden, SCA gleich */
{ 0x10E1, 0x10E0 },   /* RDT bytegleich, SCA gleich */
{ 0x10F1, 0x10F0 },   /* RDT bytegleich, SCA gleich */
{ 0x1101, 0x1100 },   /* RDT bytegleich, SCA gleich */
{ 0x1111, 0x1110 },   /* RDT bytegleich, SCA gleich */
{ 0x1121, 0x1120 },   /* RDT bytegleich, SCA gleich */
{ 0x1131, 0x1130 },   /* RDT verschieden, SCA gleich */
{ 0x1141, 0x1140 },   /* RDT verschieden, SCA gleich */
{ 0x1161, 0x1160 },   /* RDT bytegleich, SCA gleich */
{ 0x1181, 0x1180 },   /* RDT bytegleich, SCA gleich */
{ 0x11B1, 0x11B0 },   /* RDT verschieden, SCA gleich */
{ 0x11D1, 0x11D0 },   /* RDT bytegleich, SCA gleich */
{ 0x11F1, 0x11F0 },   /* RDT bytegleich, SCA gleich */
{ 0x1221, 0x1220 },   /* RDT bytegleich, SCA gleich */
{ 0x1231, 0x1230 },   /* RDT bytegleich, SCA gleich */
{ 0x2091, 0x2090 },   /* RDT bytegleich, SCA gleich */
