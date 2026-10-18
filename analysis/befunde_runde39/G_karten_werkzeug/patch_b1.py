"""Runde 39 Spur G: B1 (Blatt 0) als Schema-Zeichnung - Zeilen in re15_map_zones.h einsetzen."""
import sys
P = sys.argv[1]
s = open(P, encoding='utf-8').read()

def ersetze(alt, neu, n=1):
    global s
    if s.count(alt) != n:
        raise SystemExit('Anker %d-mal statt %d: %r' % (s.count(alt), n, alt[:120]))
    s = s.replace(alt, neu)

B1_KOPF = """    /* ==== Runde 39 Spur G: B1 (Blatt 0) NEU als Schema-Zeichnung aus Kollision + Tueren ====================
     * Nutzer 2026-10-17: "Der Room B1 den habe ich nicht gemacht. Da muss du generell noch einmal schauen, da
     * passt nichts so richtig zusammen mit dem wie man sich bewegt im Vergleich zu der Karte."
     * GEMESSEN (Dossier analysis/befunde_runde39/G_karten.md B1): die gemalten Kacheln von Blatt 0 (MAP01.PIX,
     * Liste @0x800762a0) sind ein SCHEMA - der Gang als Ring, Abschnitte mit sx/sy bis 4950/408 auf Streifen
     * gestreckt (Runde 35), ROOM1160 (Zwinger ohne Strom) ganz ohne Zeile, ROOM11C0 (Ring um eine Insel) als
     * Rechteck. Das Original selbst zeigt hier keinen Marker: die Massstabszeilen von 1180/11D0/1230 sind der Stub
     * {0,0,1,1} (@0x80076970 / @0x80076998 / @0x800769c8), 11C0 = (174,196,1,1) @0x80076990.
     * JETZT (PORT-ERGAENZUNG, Nutzer-Modell "Kollisionsboxen pro Raum, alle Tueren und wo sie hinfuehren, alle
     * Treppen ... der naechste Raum schliesst EXAKT an die Wand an", Memory reai-v2-zu-kompliziert):
     *  - jeder Ort = wenige Rechtecke ueber seiner begehbaren Flaeche, gemessen mit dem ECHTEN Kollisionsloeser
     *    (probe_r39_karten flut, Port von FUN_8003b0a4), Zellen in s_map_synth_cells (Index 3..18);
     *  - Lage je Tuerpaar: Wand auf Wand, Tuermitten laengs der Wand deckungsgleich (alle Paare gegenlaeufig, keine
     *    Drehung); 11C0 ohne Gang-Tuer mit der Westwand auf der von 11B0 - wie im Original r3/r4 beide x 168;
     *  - EIN Massstab fuer das Blatt: RE2 Retail FUN_8006e120 x = (x+28000)/450 (`lui v0,0x91a2` @0x8006e1dc,
     *    `ori v0,v0,0xb3c5` @0x8006e1e8, `sra v1,v1,8` @0x8006e20c, z negiert `subu v0,zero,v0` @0x8006e268) als
     *    Affine 65536/450 -> 146; der Marker rechnet mit DERSELBEN Affine (re15_map_zone_abbildung), also steht er
     *    in jeder begehbaren Lage in der Zeichnung seines Orts (Selbstprobe b1_plan.py: 0 von 23548 daneben);
     *  - Wandlinien zweier Orte bis 3 px Abstand werden EINE Linie ("Keine Doppelwaende").
     * Rechnung: analysis/befunde_runde39/G_karten_werkzeug/b1_plan.py (Lagen, Zellen, Affinen, Marken).
     * Weltboxen = Zonen-Zugehoerigkeit (je Raum EINE Zone, ausser 1190 Halle/Ostkammer). */
"""

# ---- Gang 1180/1230: fuenf Abschnitte -> EINE Zone mit Schema-Zeichnung 4 ----------------------------------
alt_gang = s[s.index('    { 0x1180,   3690,  -2697,   5514,  13804,  0,   0, 0,  25,'):]
alt_gang = alt_gang[:alt_gang.index('    { 0x1231,   2190,  16804,   5514,  31058,  0,   0, 4, 109,    98,    86,  1915,   408, 0, 0,   0, 0 },\n') +
                    len('    { 0x1231,   2190,  16804,   5514,  31058,  0,   0, 4, 109,    98,    86,  1915,   408, 0, 0,   0, 0 },\n')]
neu_gang = B1_KOPF + """    /* Gang 1180 = 1230 (Varianten, Runde 35): EINE Zone statt fuenf gestreckter Abschnitte; Weltbox = Huelle der
     * begehbaren Flaeche (Flut r 0: x -9492..5508, z -16377..31023, +-50 Rasterhalbschritt). Die Zusatzbits der
     * frueheren Abschnitte 2..4 (s_zone_zusatzbit) bleiben reserviert und ungenutzt. */
    { 0x1180,  -9542, -16427,   5558,  31073,  0, 255, 0,  25,     0,     0,     0,     0, 0, 0,   4, 0 },
    { 0x1181,  -9542, -16427,   5558,  31073,  0, 255, 0,  25,     0,     0,     0,     0, 0, 0,   4, 0 },
    { 0x1230,  -9542, -16427,   5558,  31073,  0, 255, 0,  25,     0,     0,     0,     0, 0, 0,   4, 0 },
    { 0x1231,  -9542, -16427,   5558,  31073,  0, 255, 0,  25,     0,     0,     0,     0, 0, 0,   4, 0 },
"""
ersetze(alt_gang, neu_gang)

# ---- 1190 Halle / Ostkammer -------------------------------------------------------------------------------
ersetze("""    { 0x1190, -24536, -26498,   4624, -10280,  0,   1, 0,  26,    75,   127,  1920,  2301, 0, 0,   0, 0 },
    { 0x1191, -24536, -26498,   4624, -10280,  0,   1, 0,  26,    75,   127,  1920,  2301, 0, 0,   0, 0 },
    { 0x1190,  13998, -25472,  21360, -11432,  0,   2, 1, 110,     0,     0,     0,     0, 0, 0,   0, 0 },
    { 0x1191,  13998, -25472,  21360, -11432,  0,   2, 1, 110,     0,     0,     0,     0, 0, 0,   0, 0 },
""", """    /* Runde 39 Spur G: Halle -> Schema-Zeichnung 5, Ostkammer -> 6 (B1-Kopf oben; die Weltboxen bleiben). Die
     * ausgelieferte Zeile @0x80076978 (75,127,1920,2301) gilt fuer die gemalte Kachel r1, die nicht mehr gezeichnet
     * wird; Halle und Ostkammer liegen im 1190-Rahmen 8286/3900 Einheiten anders zueinander als am Gang (zwei
     * getrennte Modelle in EINEM Raum) - deshalb je eigene Affine aus der eigenen Tuer. */
    { 0x1190, -24536, -26498,   4624, -10280,  0, 255, 0,  26,     0,     0,     0,     0, 0, 0,   5, 0 },
    { 0x1191, -24536, -26498,   4624, -10280,  0, 255, 0,  26,     0,     0,     0,     0, 0, 0,   5, 0 },
    { 0x1190,  13998, -25472,  21360, -11432,  0, 255, 1, 110,     0,     0,     0,     0, 0, 0,   6, 0 },
    { 0x1191,  13998, -25472,  21360, -11432,  0, 255, 1, 110,     0,     0,     0,     0, 0, 0,   6, 0 },
""")

# ---- 11B0 / 11C0 -------------------------------------------------------------------------------------------
ersetze("""    { 0x11B0, -29100, -29100,   3800, -13200,  0,   3, 0,  28,   159,   110,  2272,  1954, 0, 0,   0, 0 },
    { 0x11B1, -29100, -29100,   3800, -13200,  0,   3, 0,  28,   159,   110,  2272,  1954, 0, 0,   0, 0 },
    { 0x11C0, -32200, -31100,  21640,  25999,  0,   4, 0,  29,   158,   195,  2272,  1954, 0, 0,   0, 0 },
    { 0x11C1, -32200, -31100,  21640,  25999,  0,   4, 0,  29,   158,   195,  2272,  1954, 0, 0,   0, 0 },
""", """    /* Runde 39 Spur G: 11B0 -> Schema 7 (Streifen + Werkstatt als EIN Raum: die Trennwand @0x00EC8 wird nach
     * "Reservestrom an" per Sca_id_set 37 03 0c 00 @0x0118C nicht-fest), 11C0 -> Schema 8 (Ring um die Insel).
     * Die gemalten r3/r4 und ihre Zeilen (@0x80076988 bzw. 11B0s Zeile fuer 11C0) entfallen. */
    { 0x11B0, -29100, -29100,   3800, -13200,  0, 255, 0,  28,     0,     0,     0,     0, 0, 0,   7, 0 },
    { 0x11B1, -29100, -29100,   3800, -13200,  0, 255, 0,  28,     0,     0,     0,     0, 0, 0,   7, 0 },
    { 0x11C0, -32200, -31100,  21640,  25999,  0, 255, 0,  29,     0,     0,     0,     0, 0, 0,   8, 0 },
    { 0x11C1, -32200, -31100,  21640,  25999,  0, 255, 0,  29,     0,     0,     0,     0, 0, 0,   8, 0 },
""")

# ---- 11D0 + NEU 1160 ---------------------------------------------------------------------------------------
ersetze("""    { 0x11D0, -11950, -24050,   7750, -16500,  0,   5, 0,  30,     0,     0,     0,     0, 0, 0,   0, 0 },
    { 0x11D1, -11950, -24050,   7750, -16500,  0,   5, 0,  30,     0,     0,     0,     0, 0, 0,   0, 0 },
""", """    /* Runde 39 Spur G: 11D0 -> Schema 9 (Weltbox = Huelle der begehbaren Flaeche; die Streckungs-Box von Runde 36
     * entfaellt mit der Streckung). NEU ROOM1160: derselbe Zwinger OHNE Strom - ROOM1180 main00 waehlt fuer
     * Tuer-Slot 0 per Ck(4,243) @0x009EA zwischen Ziel 0x16 (= 1160, Strom aus, @0x009EE) und 0x1d (= 11D0,
     * @0x00A12); beide Tueren -> Gang liegen auf (-1300,-17000,2000,1000) (1160 @0x007C6, 11D0 @0x011E2) = derselbe
     * Rahmen. Bisher hatte 1160 KEINE Zeile: weder Marker noch Zeichnung, solange der Strom aus ist. */
    { 0x11D0, -11500, -23100,   5000, -11500,  0, 255, 0,  30,     0,     0,     0,     0, 0, 0,   9, 0 },
    { 0x11D1, -11500, -23100,   5000, -11500,  0, 255, 0,  30,     0,     0,     0,     0, 0, 0,   9, 0 },
    { 0x1160,  -8550, -23100,   5050, -11500,  0, 255, 0,  30,     0,     0,     0,     0, 0, 0,   9, 0 },
    { 0x1161,  -8550, -23100,   5050, -11500,  0, 255, 0,  30,     0,     0,     0,     0, 0, 0,   9, 0 },
""")

# ---- 10A0 B1-Absatz (Gast-Zeile Blatt 0) -------------------------------------------------------------------
ersetze("""    { 0x10A0,  17850,  15550,  29100,  28300,  0,   6, 0,  10,     0,     0,     0,     0, 0, 0,   0, 1 },
    { 0x10A1,  17850,  15550,  29100,  28300,  0,   6, 0,  10,     0,     0,     0,     0, 0, 0,   0, 1 },
""", """    /* Runde 39 Spur G: B1-Absatz -> Schema 10 (Absatz Band 4 samt beiden Treppenlaeufen, Aot_set sce 13 Band 4
     * @0x00E7C / @0x00E90); die gemalte r6 (153,114,16,24) entfaellt, die Etagenzeile Band 4 zeigt auf rect 255. */
    { 0x10A0,  17850,  15550,  29100,  28300,  0, 255, 0,  10,     0,     0,     0,     0, 0, 0,  10, 1 },
    { 0x10A1,  17850,  15550,  29100,  28300,  0, 255, 0,  10,     0,     0,     0,     0, 0, 0,  10, 1 },
""")
ersetze("""    { 0x10A0, 0,  4,  0,  6 },
    { 0x10A1, 0,  4,  0,  6 },
""", """    { 0x10A0, 0,  4,  0, 255 },   /* Runde 39 Spur G: Schema statt r6 (sonst malte rect_state die alte Kachel) */
    { 0x10A1, 0,  4,  0, 255 },
""")

# ---- Schema-Zeichnungen + Zellen ---------------------------------------------------------------------------
ersetze("""    { 207, 139, 15, 15, 2, 1, -152, 0, 211, 0, 145, 135 },  /* ROOM1000 Sued-WC */
};""", """    { 207, 139, 15, 15, 2, 1, -152, 0, 211, 0, 145, 135 },  /* ROOM1000 Sued-WC */
    /* Runde 39 Spur G: B1 (Blatt 0), b1_plan.py - Affine mx = (146*x)>>16 + c, my = (-146*z)>>16 + f je Ort
     * (RE2-Massstab 1/450, s. B1-Kopf in s_map_zones), Kasten = Huelle der Zellen. */
    { 124,  67,  35, 107,  3, 5, 146, 0, 146, 0, -146, 137 },  /* synth 4:  Gang 1180/1230 */
    {  69, 115,  64,  37,  8, 1, 146, 0, 121, 0, -146,  93 },  /* synth 5:  1190 Halle */
    { 132, 109,  19,  32,  9, 1, 146, 0, 103, 0, -146,  84 },  /* synth 6:  1190 Ostkammer */
    { 158,  69,  65,  26, 10, 1, 146, 0, 219, 0, -146,  35 },  /* synth 7:  11B0 */
    { 158,  94,  90,  78, 11, 4, 146, 0, 220, 0, -146, 134 },  /* synth 8:  11C0 Ring */
    { 110, 162,  38,  27, 15, 3, 146, 0, 136, 0, -146, 137 },  /* synth 9:  11D0 = 1160 */
    { 141, 153,  18,  16, 18, 1, 146, 0,  98, 0, -146, 212 },  /* synth 10: 10A0 B1-Absatz */
};""")
ersetze("""    { 207,  89, 15, 33 }, { 207, 123, 15, 16 }, { 207, 139, 15, 15 },
};""", """    { 207,  89, 15, 33 }, { 207, 123, 15, 16 }, { 207, 139, 15, 15 },
    /* Runde 39 Spur G: B1-Zellen (b1_plan.py), x/y = linke obere Wandlinie, w/h inklusive beider Wandlinien */
    { 150,  67,   9,  80 }, { 124, 102,  35,   8 }, { 124, 109,   9,   7 }, { 132, 146,  27,   8 }, { 133, 153,   9,  21 },
    {  69, 115,  64,  37 },
    { 132, 109,  19,  32 },
    { 158,  69,  65,  26 },
    { 158,  94,  90,  22 }, { 180, 115,  14,  36 }, { 230, 115,  18,  36 }, { 180, 150,  68,  22 },
    { 110, 162,  16,  27 }, { 117, 181,  22,   8 }, { 132, 173,  16,   9 },
    { 141, 153,  18,  16 },
};""")

# ---- Marken Blatt 0 ----------------------------------------------------------------------------------------
for alt in ("""    {  0,  2,  151,   69, 0, 110, 106, 1 },   /* Runde 35 Spur G: gepaart mit Gang 1180/1230 */
    {  0,  3,  168,   82, 3,  28, 255, 0 },
""", """    {  0,  3,  173,  100, 2,  28,  29, 1 },
    {  0,  3,  233,   88, 1,  28, 255, 0 },
    {  0,  5,  129,  124, 0,  30, 108, 1 },   /* Runde 35 Spur G: gepaart mit Gang 1180/1230 */
    {  0,  5,  137,  172, 2,  30, 255, 0 },
""", """    {  0,  0,  139,   76, 2, 106,  26, 1 },
    {  0,  0,  157,  113, 2, 107, 255, 0 },
""", """    {  0,  6,  158,  128, 4,  10, 255, 0 },
    {  0,  6,  163,  125, 4,  10, 255, 0 },
"""):
    ersetze(alt, "    /* Runde 39 Spur G: alte Blatt-0-Marke(n) hier entfernt - B1 neu, Marken am Tabellenende */\n")
ersetze("""};

/* INNENWAENDE in Karten-Pixeln""", """    /* Runde 39 Spur G: B1-Marken der Schema-Zeichnung (b1_plan.py). Tuer = auf der GEMEINSAMEN Wandlinie, laengs
     * Mittel der beiden Trigger-Mitten, 3 px von jeder Ecke (Balken +-2); zid = Gang, wo der Gang beteiligt ist
     * (der Gang ist auf B1 immer zuerst betreten). Tuer-Datensaetze: 1180 slot 2 @0x00A54 <-> 1190 slot 4 @0x028B2,
     * 1230 slot 4 @0x00CE6 (Ruhe-Satz slot 6 @0x00BC4 sce 0) <-> 1190 slot 6 @0x02B1C, 1180 slot 3 @0x00A74 <->
     * 11B0 slot 0 @0x00F64, 1180 slot 1 @0x00A34 <-> 10A0 slot 1 @0x00DDA/@0x00DFE, 1180 slot 0 @0x009EE/@0x00A12
     * <-> 11D0 slot 0 @0x011E2 / 1160 slot 0 @0x007C6, 11B0 slot 1 @0x00FAA <-> 11C0 slot 0 @0x01712; 11D0 -> 1260
     * slot 1 @0x01228 in der Ostwand des Sued-Gangs (@0x008C0). Treppen = Aot_set
     * sce 13 Band 4 @0x00E7C / @0x00E90 (ROOM10A0.RDT), in den Innenraum des Absatzes geklemmt. Der Kanaldeckel
     * 11B0 -> 1260 (slot 5 @0x01010, Trigger = Kreiszelle @0x00EA4 mitten im Raum) ist keine Wandtuer und bekommt
     * keine Marke. */
    {  0, 255,  127,  115, 2,  25,  26, 0 },   /* Gang <-> 1190 Halle */
    {  0, 255,  146,  109, 2,  25, 110, 0 },   /* Gang (1230) <-> 1190 Ostkammer */
    {  0, 255,  158,   72, 1,  25,  28, 0 },   /* Gang <-> 11B0 */
    {  0, 255,  145,  153, 2,  25,  10, 0 },   /* Gang <-> 10A0 B1-Absatz */
    {  0, 255,  136,  173, 2,  25,  30, 0 },   /* Gang <-> 11D0 / 1160 */
    {  0, 255,  163,   94, 2,  28,  29, 0 },   /* 11B0 <-> 11C0 */
    {  0, 255,  138,  185, 1,  30, 255, 0 },   /* 11D0 -> 1260 */
    {  0, 255,  154,  162, 4,  10, 255, 0 },   /* 10A0 Treppe 6<->4 (1F)  @0x00E7C */
    {  0, 255,  145,  165, 4,  10, 255, 0 },   /* 10A0 Treppe 4<->3 (B2)  @0x00E90 */
};

/* INNENWAENDE in Karten-Pixeln""")
open(P, 'w', encoding='utf-8', newline='\n').write(s)
print('OK')
