/* AUTOMATISCH ERZEUGT von re15_port/tools/r41_ventilkette/programme.py - nicht von Hand aendern.
 * Port-Programme der Ventilkette (Runde 41 Spur C) als SCD-Bytecode; jede Zeile mit ihrem Vorbild.
 * Blocklaengen der Ifel_ck automatisch (Blockende = Ifel_ck + 4 + Laenge = hinter dem Endif). */

/* re15_r41c_prg_2030_pult_ohne (8 B) */
static const unsigned char re15_r41c_prg_2030_pult_ohne[8] = {
    /* +000 */ 0x2b,0x28,0xff,0xff, /* Message_on 40, Maske ffff - A control panel... gauge reads zero... valve 13 is closed. */
    /* +004 */ 0x02,0x00, /* Evt_next + Nop (ROOM2050 sub04 @0x022EA) */
    /* +006 */ 0x01,0x00, /* Evt_end */
};

/* re15_r41c_prg_2030_pult_unten (8 B) */
static const unsigned char re15_r41c_prg_2030_pult_unten[8] = {
    /* +000 */ 0x2b,0x2c,0xff,0xff, /* Message_on 44, Maske ffff - The bridge is down. */
    /* +004 */ 0x02,0x00, /* Evt_next + Nop (ROOM2050 sub04 @0x022EA) */
    /* +006 */ 0x01,0x00, /* Evt_end */
};

/* re15_r41c_prg_2030_pult_senken (86 B) */
static const unsigned char re15_r41c_prg_2030_pult_senken[86] = {
    /* +000 */ 0x2b,0x2b,0xff,0xff, /* Message_on 43, Maske ffff - The pressure gauge reads 80. Lower the bridge? (Frage, Ja/Nein) */
    /* +004 */ 0x02,0x00, /* Evt_next + Nop (ROOM2050 sub04 @0x022EA) */
    /* +006 */ 0x06,0x00,0x4a,0x00, /* Ifel_ck (Laenge automatisch) Ja-Zweig */
    /* +00A */ 0x21,0x0c,0x1f,0x00, /* Ck(12,31)=0 = Ja (ROOM2050 sub04 @0x022F0) */
    /* +00E */ 0x22,0x02,0x07,0x01, /* Set(2,7)=1 Pad-Sperre (ROOM2030 sub02 @0x01E30) */
    /* +012 */ 0x3c,0x00, /* Cut_auto 0 (ROOM2030 sub02 @0x01E34) */
    /* +014 */ 0x29,0x04, /* Cut_chg 4 Draufsicht (Fahrt-Kamera; Form ROOM2030 sub02 @0x01E60 `29 0a`) */
    /* +016 */ 0x2e,0x03,0x00,0x00, /* Work_set(3,0) Bruecke + Nop (Form sub03 @0x01ED2 `2e 03 01`) */
    /* +01A */ 0x2f,0x01,0x78,0x00, /* Speed_set(1,+120) (sub03 @0x01EF2 `2f 01 78 00`) */
    /* +01E */ 0x0d,0x00,0x06,0x00,0x1e,0x00, /* For 30 (Form sub03 @0x01EDA `0d 00 06 00 2d 00`; 30 x 120 = 3600) */
    /* +024 */ 0x30,0x02,0x02,0x00, /* Add_speed, Evt_next, Evt_next, Nop (sub03 @0x01EE0..@0x01EE3) */
    /* +028 */ 0x0e,0x00, /* Next (sub03 @0x01EE4) */
    /* +02A */ 0x34,0x01,0xf8,0xf8, /* Member_set(1,-1800) Endlage = Original-Lage @0x01B6C (Form sub03 @0x01EE6) */
    /* +02E */ 0x37,0x00,0x0c,0x00, /* Sca_id_set Region 0 Zelle 12 := 00 begehbar (Form ROOM2050 sub04 @0x02398 `37 00 32 00`) */
    /* +032 */ 0x37,0x01,0x0c,0x00, /* Region 1 */
    /* +036 */ 0x37,0x02,0x0c,0x00, /* Region 2 */
    /* +03A */ 0x37,0x03,0x0c,0x00, /* Region 3 */
    /* +03E */ 0x37,0x04,0x0c,0x00, /* Region 4 */
    /* +042 */ 0x22,0x09,0x66,0x01, /* Set(9,102)=1 Bruecke unten (Port-Bit, VERTRAG) */
    /* +046 */ 0x09,0x0a,0x14,0x00, /* Sleep 20 (ROOM2080 sub02 @0x00CD8 nach der Fahrt) */
    /* +04A */ 0x2a,0x00, /* Cut_old + Nop (ROOM11D0 sub02 @0x01884) */
    /* +04C */ 0x3c,0x01, /* Cut_auto 1 (ROOM2030 sub02 @0x01EC2) */
    /* +04E */ 0x22,0x02,0x07,0x00, /* Set(2,7)=0 (ROOM2030 sub02 @0x01EC4) */
    /* +052 */ 0x08,0x00, /* Endif  */
    /* +054 */ 0x01,0x00, /* Evt_end */
};

/* re15_r41c_prg_2030_v13_frage (28 B) */
static const unsigned char re15_r41c_prg_2030_v13_frage[28] = {
    /* +000 */ 0x2b,0x29,0xff,0xff, /* Message_on 41, Maske ffff - Intake valve 13. Will you open it? */
    /* +004 */ 0x02,0x00, /* Evt_next + Nop (ROOM2050 sub04 @0x022EA) */
    /* +006 */ 0x06,0x00,0x10,0x00, /* Ifel_ck (Laenge automatisch) Ja-Zweig */
    /* +00A */ 0x21,0x0c,0x1f,0x00, /* Ck(12,31)=0 = Ja (ROOM2050 sub04 @0x022F0) */
    /* +00E */ 0x22,0x09,0x65,0x01, /* Set(9,101)=1 Ventil 13 offen (Port-Bit) */
    /* +012 */ 0x2b,0x2a,0xff,0xff, /* Message_on 42, Maske ffff - You've opened valve 13. The pressure is rising. (Form ROOM2030 sub06 @0x02000 msg 3 nach dem Set) */
    /* +016 */ 0x02,0x00, /* Evt_next + Nop (ROOM2050 sub04 @0x022EA) */
    /* +018 */ 0x08,0x00, /* Endif  */
    /* +01A */ 0x01,0x00, /* Evt_end */
};

/* re15_r41c_prg_2030_v13_zustand (8 B) */
static const unsigned char re15_r41c_prg_2030_v13_zustand[8] = {
    /* +000 */ 0x2b,0x2f,0xff,0xff, /* Message_on 47, Maske ffff - Intake valve 13 is open. */
    /* +004 */ 0x02,0x00, /* Evt_next + Nop (ROOM2050 sub04 @0x022EA) */
    /* +006 */ 0x01,0x00, /* Evt_end */
};

/* re15_r41c_prg_2030_ventil_01 (22 B) */
static const unsigned char re15_r41c_prg_2030_ventil_01[22] = {
    /* +000 */ 0x2b,0x32,0xff,0xff, /* Message_on 50, Maske ffff - Valve 01 ... Will you turn it? (Frage, Ja/Nein) */
    /* +004 */ 0x02,0x00, /* Evt_next + Nop (ROOM2050 sub04 @0x022EA) */
    /* +006 */ 0x06,0x00,0x0a,0x00, /* Ifel_ck (Laenge automatisch) Ja-Zweig */
    /* +00A */ 0x21,0x0c,0x1f,0x00, /* Ck(12,31)=0 = Ja (ROOM2050 sub04 @0x022F0) */
    /* +00E */ 0x22,0x05,0x1b,0x01, /* Set(5,0x1B)=1 Ventil gedreht (Marker fuer den C-Takt; Bank 5 Wort 0 = Raum-Scratch, Raumladen wischt @0x8003ed74; Form der Schalter ROOM2060 sub06 @0x014D8 `22 05 06 01`) */
    /* +012 */ 0x08,0x00, /* Endif  */
    /* +014 */ 0x01,0x00, /* Evt_end */
};

/* re15_r41c_prg_2030_ventil_02 (22 B) */
static const unsigned char re15_r41c_prg_2030_ventil_02[22] = {
    /* +000 */ 0x2b,0x33,0xff,0xff, /* Message_on 51, Maske ffff - Valve 02 ... Will you turn it? (Frage, Ja/Nein) */
    /* +004 */ 0x02,0x00, /* Evt_next + Nop (ROOM2050 sub04 @0x022EA) */
    /* +006 */ 0x06,0x00,0x0a,0x00, /* Ifel_ck (Laenge automatisch) Ja-Zweig */
    /* +00A */ 0x21,0x0c,0x1f,0x00, /* Ck(12,31)=0 = Ja (ROOM2050 sub04 @0x022F0) */
    /* +00E */ 0x22,0x05,0x1b,0x01, /* Set(5,0x1B)=1 Ventil gedreht (Marker fuer den C-Takt; Bank 5 Wort 0 = Raum-Scratch, Raumladen wischt @0x8003ed74; Form der Schalter ROOM2060 sub06 @0x014D8 `22 05 06 01`) */
    /* +012 */ 0x08,0x00, /* Endif  */
    /* +014 */ 0x01,0x00, /* Evt_end */
};

/* re15_r41c_prg_2030_ventil_03 (22 B) */
static const unsigned char re15_r41c_prg_2030_ventil_03[22] = {
    /* +000 */ 0x2b,0x34,0xff,0xff, /* Message_on 52, Maske ffff - Valve 03 ... Will you turn it? (Frage, Ja/Nein) */
    /* +004 */ 0x02,0x00, /* Evt_next + Nop (ROOM2050 sub04 @0x022EA) */
    /* +006 */ 0x06,0x00,0x0a,0x00, /* Ifel_ck (Laenge automatisch) Ja-Zweig */
    /* +00A */ 0x21,0x0c,0x1f,0x00, /* Ck(12,31)=0 = Ja (ROOM2050 sub04 @0x022F0) */
    /* +00E */ 0x22,0x05,0x1b,0x01, /* Set(5,0x1B)=1 Ventil gedreht (Marker fuer den C-Takt; Bank 5 Wort 0 = Raum-Scratch, Raumladen wischt @0x8003ed74; Form der Schalter ROOM2060 sub06 @0x014D8 `22 05 06 01`) */
    /* +012 */ 0x08,0x00, /* Endif  */
    /* +014 */ 0x01,0x00, /* Evt_end */
};

/* re15_r41c_prg_2030_ventil_07 (22 B) */
static const unsigned char re15_r41c_prg_2030_ventil_07[22] = {
    /* +000 */ 0x2b,0x35,0xff,0xff, /* Message_on 53, Maske ffff - Valve 07 ... Will you turn it? (Frage, Ja/Nein) */
    /* +004 */ 0x02,0x00, /* Evt_next + Nop (ROOM2050 sub04 @0x022EA) */
    /* +006 */ 0x06,0x00,0x0a,0x00, /* Ifel_ck (Laenge automatisch) Ja-Zweig */
    /* +00A */ 0x21,0x0c,0x1f,0x00, /* Ck(12,31)=0 = Ja (ROOM2050 sub04 @0x022F0) */
    /* +00E */ 0x22,0x05,0x1b,0x01, /* Set(5,0x1B)=1 Ventil gedreht (Marker fuer den C-Takt; Bank 5 Wort 0 = Raum-Scratch, Raumladen wischt @0x8003ed74; Form der Schalter ROOM2060 sub06 @0x014D8 `22 05 06 01`) */
    /* +012 */ 0x08,0x00, /* Endif  */
    /* +014 */ 0x01,0x00, /* Evt_end */
};

/* re15_r41c_prg_2030_ventil_08 (22 B) */
static const unsigned char re15_r41c_prg_2030_ventil_08[22] = {
    /* +000 */ 0x2b,0x36,0xff,0xff, /* Message_on 54, Maske ffff - Valve 08 ... Will you turn it? (Frage, Ja/Nein) */
    /* +004 */ 0x02,0x00, /* Evt_next + Nop (ROOM2050 sub04 @0x022EA) */
    /* +006 */ 0x06,0x00,0x0a,0x00, /* Ifel_ck (Laenge automatisch) Ja-Zweig */
    /* +00A */ 0x21,0x0c,0x1f,0x00, /* Ck(12,31)=0 = Ja (ROOM2050 sub04 @0x022F0) */
    /* +00E */ 0x22,0x05,0x1b,0x01, /* Set(5,0x1B)=1 Ventil gedreht (Marker fuer den C-Takt; Bank 5 Wort 0 = Raum-Scratch, Raumladen wischt @0x8003ed74; Form der Schalter ROOM2060 sub06 @0x014D8 `22 05 06 01`) */
    /* +012 */ 0x08,0x00, /* Endif  */
    /* +014 */ 0x01,0x00, /* Evt_end */
};

/* re15_r41c_prg_2030_ventil_09 (22 B) */
static const unsigned char re15_r41c_prg_2030_ventil_09[22] = {
    /* +000 */ 0x2b,0x37,0xff,0xff, /* Message_on 55, Maske ffff - Valve 09 ... Will you turn it? (Frage, Ja/Nein) */
    /* +004 */ 0x02,0x00, /* Evt_next + Nop (ROOM2050 sub04 @0x022EA) */
    /* +006 */ 0x06,0x00,0x0a,0x00, /* Ifel_ck (Laenge automatisch) Ja-Zweig */
    /* +00A */ 0x21,0x0c,0x1f,0x00, /* Ck(12,31)=0 = Ja (ROOM2050 sub04 @0x022F0) */
    /* +00E */ 0x22,0x05,0x1b,0x01, /* Set(5,0x1B)=1 Ventil gedreht (Marker fuer den C-Takt; Bank 5 Wort 0 = Raum-Scratch, Raumladen wischt @0x8003ed74; Form der Schalter ROOM2060 sub06 @0x014D8 `22 05 06 01`) */
    /* +012 */ 0x08,0x00, /* Endif  */
    /* +014 */ 0x01,0x00, /* Evt_end */
};

/* re15_r41c_prg_2030_ventile_flut (96 B) */
static const unsigned char re15_r41c_prg_2030_ventile_flut[96] = {
    /* +000 */ 0x2b,0x2e,0xff,0xff, /* Message_on 46, Maske ffff - WARNING: The water will rise in one minute. Leave the room! */
    /* +004 */ 0x02,0x00, /* Evt_next + Nop (ROOM2050 sub04 @0x022EA) */
    /* +006 */ 0x22,0x05,0x1d,0x01, /* Set(5,0x1D)=1 Zaehler laeuft (Bank 5 Wort 0 = Raum-Scratch, Raumladen wischt @0x8003ed74; C-Takt liest) */
    /* +00A */ 0x09,0x0a,0x08,0x07, /* Sleep 1800 = 60 s bei 30 Takten (Hintergrundfaden wie ROOM2000 sub07 @0x01B36) */
    /* +00E */ 0x22,0x02,0x07,0x01, /* Set(2,7)=1 Pad-Sperre (ROOM2030 sub02 @0x01E30) */
    /* +012 */ 0x22,0x02,0x02,0x01, /* Set(2,2)=1 KI steht (ROOM2050 sub02 @0x0223C) */
    /* +016 */ 0x3c,0x00, /* Cut_auto 0 (ROOM2030 sub02 @0x01E34) */
    /* +018 */ 0x29,0x04, /* Cut_chg 4 Draufsicht */
    /* +01A */ 0x2e,0x03,0x04,0x00, /* Work_set(3,4) Wasser A + Nop */
    /* +01E */ 0x32,0x00,0xd0,0xd5,0x00,0x00,0x90,0xcf, /* Pos_set (-10800,0,-12400) Beckengrund (Form ROOM2000 sub02 @0x0180C) */
    /* +026 */ 0x2f,0x01,0xce,0xff, /* Speed_set(1,-50) aufwaerts (Rate ROOM2080 sub02 @0x00CC8 `2f 01 32 00`) */
    /* +02A */ 0x2e,0x03,0x05,0x00, /* Work_set(3,5) Wasser B + Nop */
    /* +02E */ 0x32,0x00,0xd0,0xd5,0x00,0x00,0xd8,0x0e, /* Pos_set (-10800,0,3800) */
    /* +036 */ 0x2f,0x01,0xce,0xff, /* Speed_set(1,-50) */
    /* +03A */ 0x22,0x05,0x1e,0x01, /* Set(5,0x1E)=1 Flut laeuft (Bank 5 Wort 0; C-Takt: Ertrinken) */
    /* +03E */ 0x0d,0x00,0x16,0x00,0x44,0x01, /* For 324, Block 0x16 = Koerper 20 + Next 2 (324 x 50 = 16200 bis zur Tuerebene; Form ROOM2080 @0x00CCC) */
    /* +044 */ 0x2e,0x03,0x04,0x00, /* Work_set(3,4) + Nop */
    /* +048 */ 0x2f,0x01,0xce,0xff, /* Speed_set(1,-50) (Form: Speed_set direkt vor Add_speed, sub03 @0x01ED6) */
    /* +04C */ 0x30, /* Add_speed A */
    /* +04D */ 0x2e,0x03,0x05,0x00, /* Work_set(3,5) + Nop */
    /* +051 */ 0x2f,0x01,0xce,0xff, /* Speed_set(1,-50) */
    /* +055 */ 0x30, /* Add_speed B */
    /* +056 */ 0x02,0x00, /* Evt_next + Nop (ROOM2050 sub04 @0x022EA) */
    /* +058 */ 0x0e,0x00, /* Next */
    /* +05A */ 0x09,0x0a,0x3c,0x00, /* Sleep 60 (Form ROOM20B0 sub05 @0x01FB2) */
    /* +05E */ 0x01,0x00, /* Evt_end */
};

/* re15_r41c_prg_2030_ventile_falsch (24 B) */
static const unsigned char re15_r41c_prg_2030_ventile_falsch[24] = {
    /* +000 */ 0x22,0x05,0x10,0x00, /* Set(5,0x10)=0 Drehung 1 zurueck (Form ROOM11F0 sub17 @0x016C0 `22 05 0d 00`) */
    /* +004 */ 0x22,0x05,0x11,0x00, /* Set(5,0x11)=0 */
    /* +008 */ 0x22,0x05,0x12,0x00, /* Set(5,0x12)=0 */
    /* +00C */ 0x22,0x05,0x13,0x00, /* Set(5,0x13)=0 */
    /* +010 */ 0x2b,0x2d,0xff,0xff, /* Message_on 45, Maske ffff - The valves turned back to their original position. */
    /* +014 */ 0x02,0x00, /* Evt_next + Nop (ROOM2050 sub04 @0x022EA) */
    /* +016 */ 0x01,0x00, /* Evt_end */
};

/* re15_r41c_prg_2030_ventile_laeuft (8 B) */
static const unsigned char re15_r41c_prg_2030_ventile_laeuft[8] = {
    /* +000 */ 0x2b,0x2e,0xff,0xff, /* Message_on 46, Maske ffff - WARNING: The water will rise in one minute. Leave the room! (Zustand waehrend des Zaehlers) */
    /* +004 */ 0x02,0x00, /* Evt_next + Nop (ROOM2050 sub04 @0x022EA) */
    /* +006 */ 0x01,0x00, /* Evt_end */
};

/* re15_r41c_prg_2030_ventile_vorher (8 B) */
static const unsigned char re15_r41c_prg_2030_ventile_vorher[8] = {
    /* +000 */ 0x2b,0x31,0xff,0xff, /* Message_on 49, Maske ffff - I should look around here first. */
    /* +004 */ 0x02,0x00, /* Evt_next + Nop (ROOM2050 sub04 @0x022EA) */
    /* +006 */ 0x01,0x00, /* Evt_end */
};

/* re15_r41c_prg_2030_plakat (14 B) */
static const unsigned char re15_r41c_prg_2030_plakat[14] = {
    /* +000 */ 0x22,0x02,0x07,0x01, /* Set(2,7)=1 Pad-Sperre (ROOM2030 sub02 @0x01E30) */
    /* +004 */ 0x22,0x02,0x02,0x01, /* Set(2,2)=1 KI steht (ROOM2050 sub02 @0x0223C) */
    /* +008 */ 0x22,0x05,0x1c,0x01, /* Set(5,0x1C)=1 Plakat-Nahaufnahme (Marker fuer den C-Takt, Bank 5 Wort 0) */
    /* +00C */ 0x01,0x00, /* Evt_end */
};

/* re15_r41c_prg_2050_hebel_sperre (8 B) */
static const unsigned char re15_r41c_prg_2050_hebel_sperre[8] = {
    /* +000 */ 0x2b,0x28,0xff,0xff, /* Message_on 40, Maske ffff - The pump has no power. The switchboard must be set first. (Form ROOM2060 sub12 @0x015D0) */
    /* +004 */ 0x02,0x00, /* Evt_next + Nop (ROOM2050 sub04 @0x022EA) */
    /* +006 */ 0x01,0x00, /* Evt_end */
};

/* re15_r41c_prg_2050_pult_ein (44 B) */
static const unsigned char re15_r41c_prg_2050_pult_ein[44] = {
    /* +000 */ 0x2b,0x2b,0xff,0xff, /* Message_on 43, Maske ffff - A high voltage switchboard. The pump needs 80 volts. */
    /* +004 */ 0x02,0x00, /* Evt_next + Nop (ROOM2050 sub04 @0x022EA) */
    /* +006 */ 0x2b,0x29,0xff,0xff, /* Message_on 41, Maske ffff - Will you push them? = ROOM11F0 msg 1 @0x18D8 (Form ROOM11F0 sub16 @0x015A8) */
    /* +00A */ 0x02,0x00, /* Evt_next + Nop (ROOM2050 sub04 @0x022EA) */
    /* +00C */ 0x06,0x00,0x1a,0x00, /* Ifel_ck (Laenge automatisch) Ja-Zweig */
    /* +010 */ 0x21,0x0c,0x1f,0x00, /* Ck(12,31)=0 = Ja (ROOM2050 sub04 @0x022F0) */
    /* +014 */ 0x29,0x0f, /* Cut_chg 15 = ROOM2050 sub09 @0x023F2 */
    /* +016 */ 0x46,0x08,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, /* Aot_reset Slot 8 sce 0 = sub09 @0x023F4 */
    /* +020 */ 0x22,0x05,0x04,0x01, /* Set(5,4)=1 Cursor-Tasten = sub09 @0x023FE */
    /* +024 */ 0x22,0x05,0x06,0x01, /* Set(5,6)=1 Spieler/KI stehen = sub09 @0x02406 (OHNE (5,5): SQUARE beendet sonst, sub01 @0x021E2) */
    /* +028 */ 0x08,0x00, /* Endif  */
    /* +02A */ 0x01,0x00, /* Evt_end */
};

/* re15_r41c_prg_2050_pult_zustand (8 B) */
static const unsigned char re15_r41c_prg_2050_pult_zustand[8] = {
    /* +000 */ 0x2b,0x2c,0xff,0xff, /* Message_on 44, Maske ffff - The switchboard is set to 80 volts. The pump is powered. */
    /* +004 */ 0x02,0x00, /* Evt_next + Nop (ROOM2050 sub04 @0x022EA) */
    /* +006 */ 0x01,0x00, /* Evt_end */
};

typedef struct { const char *name; const unsigned char *b; int n; } re15_r41c_programm_t;
static const re15_r41c_programm_t re15_r41c_programme[] = {
    { "2030_pult_ohne", re15_r41c_prg_2030_pult_ohne, 8 },
    { "2030_pult_unten", re15_r41c_prg_2030_pult_unten, 8 },
    { "2030_pult_senken", re15_r41c_prg_2030_pult_senken, 86 },
    { "2030_v13_frage", re15_r41c_prg_2030_v13_frage, 28 },
    { "2030_v13_zustand", re15_r41c_prg_2030_v13_zustand, 8 },
    { "2030_ventil_01", re15_r41c_prg_2030_ventil_01, 22 },
    { "2030_ventil_02", re15_r41c_prg_2030_ventil_02, 22 },
    { "2030_ventil_03", re15_r41c_prg_2030_ventil_03, 22 },
    { "2030_ventil_07", re15_r41c_prg_2030_ventil_07, 22 },
    { "2030_ventil_08", re15_r41c_prg_2030_ventil_08, 22 },
    { "2030_ventil_09", re15_r41c_prg_2030_ventil_09, 22 },
    { "2030_ventile_flut", re15_r41c_prg_2030_ventile_flut, 96 },
    { "2030_ventile_falsch", re15_r41c_prg_2030_ventile_falsch, 24 },
    { "2030_ventile_laeuft", re15_r41c_prg_2030_ventile_laeuft, 8 },
    { "2030_ventile_vorher", re15_r41c_prg_2030_ventile_vorher, 8 },
    { "2030_plakat", re15_r41c_prg_2030_plakat, 14 },
    { "2050_hebel_sperre", re15_r41c_prg_2050_hebel_sperre, 8 },
    { "2050_pult_ein", re15_r41c_prg_2050_pult_ein, 44 },
    { "2050_pult_zustand", re15_r41c_prg_2050_pult_zustand, 8 },
};
#define RE15_R41C_PROGRAMME_N ((int)(sizeof re15_r41c_programme / sizeof re15_r41c_programme[0]))
