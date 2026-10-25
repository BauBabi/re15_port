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

/* re15_r41c_prg_2030_ventil_01 (184 B) */
static const unsigned char re15_r41c_prg_2030_ventil_01[184] = {
    /* +000 */ 0x2b,0x32,0xff,0xff, /* Message_on 50, Maske ffff - Valve 01 ... Will you turn it? (Frage, Ja/Nein) */
    /* +004 */ 0x02,0x00, /* Evt_next + Nop (ROOM2050 sub04 @0x022EA) */
    /* +006 */ 0x06,0x00,0xac,0x00, /* Ifel_ck (Laenge automatisch) Ja-Zweig */
    /* +00A */ 0x21,0x0c,0x1f,0x00, /* Ck(12,31)=0 = Ja (ROOM2050 sub04 @0x022F0) */
    /* +00E */ 0x22,0x02,0x07,0x01, /* Set(2,7)=1 Pad-Sperre (ROOM2030 sub02 @0x01E30) */
    /* +012 */ 0x22,0x02,0x02,0x01, /* Set(2,2)=1 KI steht (ROOM2050 sub02 @0x0223C) */
    /* +016 */ 0x2e,0x01,0x00,0x00, /* Work_set(1,0) Spieler + Nop (RE2 sub08 @0x0210A `2e 01 00`) */
    /* +01A */ 0x32,0x00,0xba,0xfa,0xf8,0xf8,0xfc,0xe1, /* Pos_set (-1350,-1800,-7684) vor Rad 01 (Form RE2 sub08 @0x02116 `32 00 30 b2 00 00 fe ac`) */
    /* +022 */ 0x34,0x04,0x00,0x04, /* Member_set(4 = rot_y, 1024) Blick zur Wand (Form RE2 @0x0212C `34 0f 00 08`) */
    /* +026 */ 0x09,0x0a,0x14,0x00, /* Sleep 20 (RE2 @0x02130) */
    /* +02A */ 0x3f,0x00,0x0f,0x00, /* Plc_motion(0,15,0) Hand zum Rad (RE2 @0x0213C; RBJ Clip 15 = 30 Bilder) */
    /* +02E */ 0x09,0x0a,0x1e,0x00, /* Sleep 30 (RE2 @0x0214A) */
    /* +032 */ 0x3f,0x00,0x10,0x00, /* Plc_motion(0,16,0) Rad fassen (RE2 @0x02156; Clip 16 = 50 Bilder) */
    /* +036 */ 0x09,0x0a,0x1c,0x00, /* Sleep 28 (RE2 @0x02164) */
    /* +03A */ 0x22,0x05,0x1a,0x01, /* Set(5,0x1A)=1 Ton: statt RE2 se_on(2,5) `36 02 05 01 01 00 70 17 38 ff 00 00` (C-Takt spielt RE2 ROOM1080 Satz 5, Bank VENTIL1080) */
    /* +03E */ 0x09,0x0a,0x16,0x00, /* Sleep 22 (RE2 @0x02174) */
    /* +042 */ 0x3f,0x00,0x11,0x00, /* Plc_motion(0,17,0) Drehtakt 1/5 (RE2 @0x02186/@0x021B4/@0x021CE) */
    /* +046 */ 0x09,0x0a,0x12,0x00, /* Sleep 18 (RE2 @0x02194/@0x021B8/@0x021D2) */
    /* +04A */ 0x22,0x05,0x1a,0x01, /* Set(5,0x1A)=1 Ton: statt RE2 se_on(2,5) `36 02 05 01 01 00 70 17 38 ff 00 00` (C-Takt spielt RE2 ROOM1080 Satz 5, Bank VENTIL1080) */
    /* +04E */ 0x09,0x0a,0x0c,0x00, /* Sleep 12 (RE2 @0x021A4/@0x021C8) */
    /* +052 */ 0x3f,0x00,0x11,0x00, /* Plc_motion(0,17,0) Drehtakt 2/5 (RE2 @0x02186/@0x021B4/@0x021CE) */
    /* +056 */ 0x09,0x0a,0x12,0x00, /* Sleep 18 (RE2 @0x02194/@0x021B8/@0x021D2) */
    /* +05A */ 0x22,0x05,0x1a,0x01, /* Set(5,0x1A)=1 Ton: statt RE2 se_on(2,5) `36 02 05 01 01 00 70 17 38 ff 00 00` (C-Takt spielt RE2 ROOM1080 Satz 5, Bank VENTIL1080) */
    /* +05E */ 0x09,0x0a,0x0c,0x00, /* Sleep 12 (RE2 @0x021A4/@0x021C8) */
    /* +062 */ 0x3f,0x00,0x11,0x00, /* Plc_motion(0,17,0) Drehtakt 3/5 (RE2 @0x02186/@0x021B4/@0x021CE) */
    /* +066 */ 0x09,0x0a,0x12,0x00, /* Sleep 18 (RE2 @0x02194/@0x021B8/@0x021D2) */
    /* +06A */ 0x22,0x05,0x1a,0x01, /* Set(5,0x1A)=1 Ton: statt RE2 se_on(2,5) `36 02 05 01 01 00 70 17 38 ff 00 00` (C-Takt spielt RE2 ROOM1080 Satz 5, Bank VENTIL1080) */
    /* +06E */ 0x09,0x0a,0x0c,0x00, /* Sleep 12 (RE2 @0x021A4/@0x021C8) */
    /* +072 */ 0x3f,0x00,0x11,0x00, /* Plc_motion(0,17,0) Drehtakt 4/5 (RE2 @0x02186/@0x021B4/@0x021CE) */
    /* +076 */ 0x09,0x0a,0x12,0x00, /* Sleep 18 (RE2 @0x02194/@0x021B8/@0x021D2) */
    /* +07A */ 0x22,0x05,0x1a,0x01, /* Set(5,0x1A)=1 Ton: statt RE2 se_on(2,5) `36 02 05 01 01 00 70 17 38 ff 00 00` (C-Takt spielt RE2 ROOM1080 Satz 5, Bank VENTIL1080) */
    /* +07E */ 0x09,0x0a,0x0c,0x00, /* Sleep 12 (RE2 @0x021A4/@0x021C8) */
    /* +082 */ 0x3f,0x00,0x11,0x00, /* Plc_motion(0,17,0) Drehtakt 5/5 (RE2 @0x02186/@0x021B4/@0x021CE) */
    /* +086 */ 0x09,0x0a,0x12,0x00, /* Sleep 18 (RE2 @0x02194/@0x021B8/@0x021D2) */
    /* +08A */ 0x22,0x05,0x1a,0x01, /* Set(5,0x1A)=1 Ton: statt RE2 se_on(2,5) `36 02 05 01 01 00 70 17 38 ff 00 00` (C-Takt spielt RE2 ROOM1080 Satz 5, Bank VENTIL1080) */
    /* +08E */ 0x09,0x0a,0x0f,0x00, /* Sleep 15 (RE2 @0x021E2) */
    /* +092 */ 0x09,0x0a,0x0c,0x00, /* Sleep 12 (RE2 @0x021E6) */
    /* +096 */ 0x3f,0x00,0x10,0xc0, /* Plc_motion(0,16,0xC0) loslassen rueckwaerts, ohne Ueberblendung (RE2 @0x021EC) */
    /* +09A */ 0x09,0x0a,0x32,0x00, /* Sleep 50 (RE2 @0x021F0) */
    /* +09E */ 0x3f,0x00,0x0f,0xc0, /* Plc_motion(0,15,0xC0) Hand zurueck (RE2 @0x021F6) */
    /* +0A2 */ 0x09,0x0a,0x1e,0x00, /* Sleep 30 (RE2 @0x021FA) */
    /* +0A6 */ 0x42,0x00, /* Plc_ret + Nop (RE2 @0x0221E `42` `00`) */
    /* +0A8 */ 0x22,0x02,0x02,0x00, /* Set(2,2)=0 (ROOM11F0 sub17 @0x016EC) */
    /* +0AC */ 0x22,0x02,0x07,0x00, /* Set(2,7)=0 (ROOM2030 sub02 @0x01EC4) */
    /* +0B0 */ 0x22,0x05,0x1b,0x01, /* Set(5,0x1B)=1 Ventil gedreht (Marker fuer den C-Takt; Bank 5 Wort 0 = Raum-Scratch, Raumladen wischt @0x8003ed74; Form der Schalter ROOM2060 sub06 @0x014D8 `22 05 06 01`) */
    /* +0B4 */ 0x08,0x00, /* Endif  */
    /* +0B6 */ 0x01,0x00, /* Evt_end */
};

/* re15_r41c_prg_2030_ventil_02 (184 B) */
static const unsigned char re15_r41c_prg_2030_ventil_02[184] = {
    /* +000 */ 0x2b,0x33,0xff,0xff, /* Message_on 51, Maske ffff - Valve 02 ... Will you turn it? (Frage, Ja/Nein) */
    /* +004 */ 0x02,0x00, /* Evt_next + Nop (ROOM2050 sub04 @0x022EA) */
    /* +006 */ 0x06,0x00,0xac,0x00, /* Ifel_ck (Laenge automatisch) Ja-Zweig */
    /* +00A */ 0x21,0x0c,0x1f,0x00, /* Ck(12,31)=0 = Ja (ROOM2050 sub04 @0x022F0) */
    /* +00E */ 0x22,0x02,0x07,0x01, /* Set(2,7)=1 Pad-Sperre (ROOM2030 sub02 @0x01E30) */
    /* +012 */ 0x22,0x02,0x02,0x01, /* Set(2,2)=1 KI steht (ROOM2050 sub02 @0x0223C) */
    /* +016 */ 0x2e,0x01,0x00,0x00, /* Work_set(1,0) Spieler + Nop (RE2 sub08 @0x0210A `2e 01 00`) */
    /* +01A */ 0x32,0x00,0x96,0x00,0xf8,0xf8,0xfc,0xe1, /* Pos_set (150,-1800,-7684) vor Rad 02 (Form RE2 sub08 @0x02116 `32 00 30 b2 00 00 fe ac`) */
    /* +022 */ 0x34,0x04,0x00,0x04, /* Member_set(4 = rot_y, 1024) Blick zur Wand (Form RE2 @0x0212C `34 0f 00 08`) */
    /* +026 */ 0x09,0x0a,0x14,0x00, /* Sleep 20 (RE2 @0x02130) */
    /* +02A */ 0x3f,0x00,0x0f,0x00, /* Plc_motion(0,15,0) Hand zum Rad (RE2 @0x0213C; RBJ Clip 15 = 30 Bilder) */
    /* +02E */ 0x09,0x0a,0x1e,0x00, /* Sleep 30 (RE2 @0x0214A) */
    /* +032 */ 0x3f,0x00,0x10,0x00, /* Plc_motion(0,16,0) Rad fassen (RE2 @0x02156; Clip 16 = 50 Bilder) */
    /* +036 */ 0x09,0x0a,0x1c,0x00, /* Sleep 28 (RE2 @0x02164) */
    /* +03A */ 0x22,0x05,0x1a,0x01, /* Set(5,0x1A)=1 Ton: statt RE2 se_on(2,5) `36 02 05 01 01 00 70 17 38 ff 00 00` (C-Takt spielt RE2 ROOM1080 Satz 5, Bank VENTIL1080) */
    /* +03E */ 0x09,0x0a,0x16,0x00, /* Sleep 22 (RE2 @0x02174) */
    /* +042 */ 0x3f,0x00,0x11,0x00, /* Plc_motion(0,17,0) Drehtakt 1/5 (RE2 @0x02186/@0x021B4/@0x021CE) */
    /* +046 */ 0x09,0x0a,0x12,0x00, /* Sleep 18 (RE2 @0x02194/@0x021B8/@0x021D2) */
    /* +04A */ 0x22,0x05,0x1a,0x01, /* Set(5,0x1A)=1 Ton: statt RE2 se_on(2,5) `36 02 05 01 01 00 70 17 38 ff 00 00` (C-Takt spielt RE2 ROOM1080 Satz 5, Bank VENTIL1080) */
    /* +04E */ 0x09,0x0a,0x0c,0x00, /* Sleep 12 (RE2 @0x021A4/@0x021C8) */
    /* +052 */ 0x3f,0x00,0x11,0x00, /* Plc_motion(0,17,0) Drehtakt 2/5 (RE2 @0x02186/@0x021B4/@0x021CE) */
    /* +056 */ 0x09,0x0a,0x12,0x00, /* Sleep 18 (RE2 @0x02194/@0x021B8/@0x021D2) */
    /* +05A */ 0x22,0x05,0x1a,0x01, /* Set(5,0x1A)=1 Ton: statt RE2 se_on(2,5) `36 02 05 01 01 00 70 17 38 ff 00 00` (C-Takt spielt RE2 ROOM1080 Satz 5, Bank VENTIL1080) */
    /* +05E */ 0x09,0x0a,0x0c,0x00, /* Sleep 12 (RE2 @0x021A4/@0x021C8) */
    /* +062 */ 0x3f,0x00,0x11,0x00, /* Plc_motion(0,17,0) Drehtakt 3/5 (RE2 @0x02186/@0x021B4/@0x021CE) */
    /* +066 */ 0x09,0x0a,0x12,0x00, /* Sleep 18 (RE2 @0x02194/@0x021B8/@0x021D2) */
    /* +06A */ 0x22,0x05,0x1a,0x01, /* Set(5,0x1A)=1 Ton: statt RE2 se_on(2,5) `36 02 05 01 01 00 70 17 38 ff 00 00` (C-Takt spielt RE2 ROOM1080 Satz 5, Bank VENTIL1080) */
    /* +06E */ 0x09,0x0a,0x0c,0x00, /* Sleep 12 (RE2 @0x021A4/@0x021C8) */
    /* +072 */ 0x3f,0x00,0x11,0x00, /* Plc_motion(0,17,0) Drehtakt 4/5 (RE2 @0x02186/@0x021B4/@0x021CE) */
    /* +076 */ 0x09,0x0a,0x12,0x00, /* Sleep 18 (RE2 @0x02194/@0x021B8/@0x021D2) */
    /* +07A */ 0x22,0x05,0x1a,0x01, /* Set(5,0x1A)=1 Ton: statt RE2 se_on(2,5) `36 02 05 01 01 00 70 17 38 ff 00 00` (C-Takt spielt RE2 ROOM1080 Satz 5, Bank VENTIL1080) */
    /* +07E */ 0x09,0x0a,0x0c,0x00, /* Sleep 12 (RE2 @0x021A4/@0x021C8) */
    /* +082 */ 0x3f,0x00,0x11,0x00, /* Plc_motion(0,17,0) Drehtakt 5/5 (RE2 @0x02186/@0x021B4/@0x021CE) */
    /* +086 */ 0x09,0x0a,0x12,0x00, /* Sleep 18 (RE2 @0x02194/@0x021B8/@0x021D2) */
    /* +08A */ 0x22,0x05,0x1a,0x01, /* Set(5,0x1A)=1 Ton: statt RE2 se_on(2,5) `36 02 05 01 01 00 70 17 38 ff 00 00` (C-Takt spielt RE2 ROOM1080 Satz 5, Bank VENTIL1080) */
    /* +08E */ 0x09,0x0a,0x0f,0x00, /* Sleep 15 (RE2 @0x021E2) */
    /* +092 */ 0x09,0x0a,0x0c,0x00, /* Sleep 12 (RE2 @0x021E6) */
    /* +096 */ 0x3f,0x00,0x10,0xc0, /* Plc_motion(0,16,0xC0) loslassen rueckwaerts, ohne Ueberblendung (RE2 @0x021EC) */
    /* +09A */ 0x09,0x0a,0x32,0x00, /* Sleep 50 (RE2 @0x021F0) */
    /* +09E */ 0x3f,0x00,0x0f,0xc0, /* Plc_motion(0,15,0xC0) Hand zurueck (RE2 @0x021F6) */
    /* +0A2 */ 0x09,0x0a,0x1e,0x00, /* Sleep 30 (RE2 @0x021FA) */
    /* +0A6 */ 0x42,0x00, /* Plc_ret + Nop (RE2 @0x0221E `42` `00`) */
    /* +0A8 */ 0x22,0x02,0x02,0x00, /* Set(2,2)=0 (ROOM11F0 sub17 @0x016EC) */
    /* +0AC */ 0x22,0x02,0x07,0x00, /* Set(2,7)=0 (ROOM2030 sub02 @0x01EC4) */
    /* +0B0 */ 0x22,0x05,0x1b,0x01, /* Set(5,0x1B)=1 Ventil gedreht (Marker fuer den C-Takt; Bank 5 Wort 0 = Raum-Scratch, Raumladen wischt @0x8003ed74; Form der Schalter ROOM2060 sub06 @0x014D8 `22 05 06 01`) */
    /* +0B4 */ 0x08,0x00, /* Endif  */
    /* +0B6 */ 0x01,0x00, /* Evt_end */
};

/* re15_r41c_prg_2030_ventil_03 (184 B) */
static const unsigned char re15_r41c_prg_2030_ventil_03[184] = {
    /* +000 */ 0x2b,0x34,0xff,0xff, /* Message_on 52, Maske ffff - Valve 03 ... Will you turn it? (Frage, Ja/Nein) */
    /* +004 */ 0x02,0x00, /* Evt_next + Nop (ROOM2050 sub04 @0x022EA) */
    /* +006 */ 0x06,0x00,0xac,0x00, /* Ifel_ck (Laenge automatisch) Ja-Zweig */
    /* +00A */ 0x21,0x0c,0x1f,0x00, /* Ck(12,31)=0 = Ja (ROOM2050 sub04 @0x022F0) */
    /* +00E */ 0x22,0x02,0x07,0x01, /* Set(2,7)=1 Pad-Sperre (ROOM2030 sub02 @0x01E30) */
    /* +012 */ 0x22,0x02,0x02,0x01, /* Set(2,2)=1 KI steht (ROOM2050 sub02 @0x0223C) */
    /* +016 */ 0x2e,0x01,0x00,0x00, /* Work_set(1,0) Spieler + Nop (RE2 sub08 @0x0210A `2e 01 00`) */
    /* +01A */ 0x32,0x00,0x2e,0x09,0xf8,0xf8,0xfc,0xe1, /* Pos_set (2350,-1800,-7684) vor Rad 03 (Form RE2 sub08 @0x02116 `32 00 30 b2 00 00 fe ac`) */
    /* +022 */ 0x34,0x04,0x00,0x04, /* Member_set(4 = rot_y, 1024) Blick zur Wand (Form RE2 @0x0212C `34 0f 00 08`) */
    /* +026 */ 0x09,0x0a,0x14,0x00, /* Sleep 20 (RE2 @0x02130) */
    /* +02A */ 0x3f,0x00,0x0f,0x00, /* Plc_motion(0,15,0) Hand zum Rad (RE2 @0x0213C; RBJ Clip 15 = 30 Bilder) */
    /* +02E */ 0x09,0x0a,0x1e,0x00, /* Sleep 30 (RE2 @0x0214A) */
    /* +032 */ 0x3f,0x00,0x10,0x00, /* Plc_motion(0,16,0) Rad fassen (RE2 @0x02156; Clip 16 = 50 Bilder) */
    /* +036 */ 0x09,0x0a,0x1c,0x00, /* Sleep 28 (RE2 @0x02164) */
    /* +03A */ 0x22,0x05,0x1a,0x01, /* Set(5,0x1A)=1 Ton: statt RE2 se_on(2,5) `36 02 05 01 01 00 70 17 38 ff 00 00` (C-Takt spielt RE2 ROOM1080 Satz 5, Bank VENTIL1080) */
    /* +03E */ 0x09,0x0a,0x16,0x00, /* Sleep 22 (RE2 @0x02174) */
    /* +042 */ 0x3f,0x00,0x11,0x00, /* Plc_motion(0,17,0) Drehtakt 1/5 (RE2 @0x02186/@0x021B4/@0x021CE) */
    /* +046 */ 0x09,0x0a,0x12,0x00, /* Sleep 18 (RE2 @0x02194/@0x021B8/@0x021D2) */
    /* +04A */ 0x22,0x05,0x1a,0x01, /* Set(5,0x1A)=1 Ton: statt RE2 se_on(2,5) `36 02 05 01 01 00 70 17 38 ff 00 00` (C-Takt spielt RE2 ROOM1080 Satz 5, Bank VENTIL1080) */
    /* +04E */ 0x09,0x0a,0x0c,0x00, /* Sleep 12 (RE2 @0x021A4/@0x021C8) */
    /* +052 */ 0x3f,0x00,0x11,0x00, /* Plc_motion(0,17,0) Drehtakt 2/5 (RE2 @0x02186/@0x021B4/@0x021CE) */
    /* +056 */ 0x09,0x0a,0x12,0x00, /* Sleep 18 (RE2 @0x02194/@0x021B8/@0x021D2) */
    /* +05A */ 0x22,0x05,0x1a,0x01, /* Set(5,0x1A)=1 Ton: statt RE2 se_on(2,5) `36 02 05 01 01 00 70 17 38 ff 00 00` (C-Takt spielt RE2 ROOM1080 Satz 5, Bank VENTIL1080) */
    /* +05E */ 0x09,0x0a,0x0c,0x00, /* Sleep 12 (RE2 @0x021A4/@0x021C8) */
    /* +062 */ 0x3f,0x00,0x11,0x00, /* Plc_motion(0,17,0) Drehtakt 3/5 (RE2 @0x02186/@0x021B4/@0x021CE) */
    /* +066 */ 0x09,0x0a,0x12,0x00, /* Sleep 18 (RE2 @0x02194/@0x021B8/@0x021D2) */
    /* +06A */ 0x22,0x05,0x1a,0x01, /* Set(5,0x1A)=1 Ton: statt RE2 se_on(2,5) `36 02 05 01 01 00 70 17 38 ff 00 00` (C-Takt spielt RE2 ROOM1080 Satz 5, Bank VENTIL1080) */
    /* +06E */ 0x09,0x0a,0x0c,0x00, /* Sleep 12 (RE2 @0x021A4/@0x021C8) */
    /* +072 */ 0x3f,0x00,0x11,0x00, /* Plc_motion(0,17,0) Drehtakt 4/5 (RE2 @0x02186/@0x021B4/@0x021CE) */
    /* +076 */ 0x09,0x0a,0x12,0x00, /* Sleep 18 (RE2 @0x02194/@0x021B8/@0x021D2) */
    /* +07A */ 0x22,0x05,0x1a,0x01, /* Set(5,0x1A)=1 Ton: statt RE2 se_on(2,5) `36 02 05 01 01 00 70 17 38 ff 00 00` (C-Takt spielt RE2 ROOM1080 Satz 5, Bank VENTIL1080) */
    /* +07E */ 0x09,0x0a,0x0c,0x00, /* Sleep 12 (RE2 @0x021A4/@0x021C8) */
    /* +082 */ 0x3f,0x00,0x11,0x00, /* Plc_motion(0,17,0) Drehtakt 5/5 (RE2 @0x02186/@0x021B4/@0x021CE) */
    /* +086 */ 0x09,0x0a,0x12,0x00, /* Sleep 18 (RE2 @0x02194/@0x021B8/@0x021D2) */
    /* +08A */ 0x22,0x05,0x1a,0x01, /* Set(5,0x1A)=1 Ton: statt RE2 se_on(2,5) `36 02 05 01 01 00 70 17 38 ff 00 00` (C-Takt spielt RE2 ROOM1080 Satz 5, Bank VENTIL1080) */
    /* +08E */ 0x09,0x0a,0x0f,0x00, /* Sleep 15 (RE2 @0x021E2) */
    /* +092 */ 0x09,0x0a,0x0c,0x00, /* Sleep 12 (RE2 @0x021E6) */
    /* +096 */ 0x3f,0x00,0x10,0xc0, /* Plc_motion(0,16,0xC0) loslassen rueckwaerts, ohne Ueberblendung (RE2 @0x021EC) */
    /* +09A */ 0x09,0x0a,0x32,0x00, /* Sleep 50 (RE2 @0x021F0) */
    /* +09E */ 0x3f,0x00,0x0f,0xc0, /* Plc_motion(0,15,0xC0) Hand zurueck (RE2 @0x021F6) */
    /* +0A2 */ 0x09,0x0a,0x1e,0x00, /* Sleep 30 (RE2 @0x021FA) */
    /* +0A6 */ 0x42,0x00, /* Plc_ret + Nop (RE2 @0x0221E `42` `00`) */
    /* +0A8 */ 0x22,0x02,0x02,0x00, /* Set(2,2)=0 (ROOM11F0 sub17 @0x016EC) */
    /* +0AC */ 0x22,0x02,0x07,0x00, /* Set(2,7)=0 (ROOM2030 sub02 @0x01EC4) */
    /* +0B0 */ 0x22,0x05,0x1b,0x01, /* Set(5,0x1B)=1 Ventil gedreht (Marker fuer den C-Takt; Bank 5 Wort 0 = Raum-Scratch, Raumladen wischt @0x8003ed74; Form der Schalter ROOM2060 sub06 @0x014D8 `22 05 06 01`) */
    /* +0B4 */ 0x08,0x00, /* Endif  */
    /* +0B6 */ 0x01,0x00, /* Evt_end */
};

/* re15_r41c_prg_2030_ventil_07 (184 B) */
static const unsigned char re15_r41c_prg_2030_ventil_07[184] = {
    /* +000 */ 0x2b,0x35,0xff,0xff, /* Message_on 53, Maske ffff - Valve 07 ... Will you turn it? (Frage, Ja/Nein) */
    /* +004 */ 0x02,0x00, /* Evt_next + Nop (ROOM2050 sub04 @0x022EA) */
    /* +006 */ 0x06,0x00,0xac,0x00, /* Ifel_ck (Laenge automatisch) Ja-Zweig */
    /* +00A */ 0x21,0x0c,0x1f,0x00, /* Ck(12,31)=0 = Ja (ROOM2050 sub04 @0x022F0) */
    /* +00E */ 0x22,0x02,0x07,0x01, /* Set(2,7)=1 Pad-Sperre (ROOM2030 sub02 @0x01E30) */
    /* +012 */ 0x22,0x02,0x02,0x01, /* Set(2,2)=1 KI steht (ROOM2050 sub02 @0x0223C) */
    /* +016 */ 0x2e,0x01,0x00,0x00, /* Work_set(1,0) Spieler + Nop (RE2 sub08 @0x0210A `2e 01 00`) */
    /* +01A */ 0x32,0x00,0x28,0x0a,0xf8,0xf8,0xac,0x1b, /* Pos_set (2600,-1800,7084) vor Rad 07 (Form RE2 sub08 @0x02116 `32 00 30 b2 00 00 fe ac`) */
    /* +022 */ 0x34,0x04,0x00,0x0c, /* Member_set(4 = rot_y, 3072) Blick zur Wand (Form RE2 @0x0212C `34 0f 00 08`) */
    /* +026 */ 0x09,0x0a,0x14,0x00, /* Sleep 20 (RE2 @0x02130) */
    /* +02A */ 0x3f,0x00,0x0f,0x00, /* Plc_motion(0,15,0) Hand zum Rad (RE2 @0x0213C; RBJ Clip 15 = 30 Bilder) */
    /* +02E */ 0x09,0x0a,0x1e,0x00, /* Sleep 30 (RE2 @0x0214A) */
    /* +032 */ 0x3f,0x00,0x10,0x00, /* Plc_motion(0,16,0) Rad fassen (RE2 @0x02156; Clip 16 = 50 Bilder) */
    /* +036 */ 0x09,0x0a,0x1c,0x00, /* Sleep 28 (RE2 @0x02164) */
    /* +03A */ 0x22,0x05,0x1a,0x01, /* Set(5,0x1A)=1 Ton: statt RE2 se_on(2,5) `36 02 05 01 01 00 70 17 38 ff 00 00` (C-Takt spielt RE2 ROOM1080 Satz 5, Bank VENTIL1080) */
    /* +03E */ 0x09,0x0a,0x16,0x00, /* Sleep 22 (RE2 @0x02174) */
    /* +042 */ 0x3f,0x00,0x11,0x00, /* Plc_motion(0,17,0) Drehtakt 1/5 (RE2 @0x02186/@0x021B4/@0x021CE) */
    /* +046 */ 0x09,0x0a,0x12,0x00, /* Sleep 18 (RE2 @0x02194/@0x021B8/@0x021D2) */
    /* +04A */ 0x22,0x05,0x1a,0x01, /* Set(5,0x1A)=1 Ton: statt RE2 se_on(2,5) `36 02 05 01 01 00 70 17 38 ff 00 00` (C-Takt spielt RE2 ROOM1080 Satz 5, Bank VENTIL1080) */
    /* +04E */ 0x09,0x0a,0x0c,0x00, /* Sleep 12 (RE2 @0x021A4/@0x021C8) */
    /* +052 */ 0x3f,0x00,0x11,0x00, /* Plc_motion(0,17,0) Drehtakt 2/5 (RE2 @0x02186/@0x021B4/@0x021CE) */
    /* +056 */ 0x09,0x0a,0x12,0x00, /* Sleep 18 (RE2 @0x02194/@0x021B8/@0x021D2) */
    /* +05A */ 0x22,0x05,0x1a,0x01, /* Set(5,0x1A)=1 Ton: statt RE2 se_on(2,5) `36 02 05 01 01 00 70 17 38 ff 00 00` (C-Takt spielt RE2 ROOM1080 Satz 5, Bank VENTIL1080) */
    /* +05E */ 0x09,0x0a,0x0c,0x00, /* Sleep 12 (RE2 @0x021A4/@0x021C8) */
    /* +062 */ 0x3f,0x00,0x11,0x00, /* Plc_motion(0,17,0) Drehtakt 3/5 (RE2 @0x02186/@0x021B4/@0x021CE) */
    /* +066 */ 0x09,0x0a,0x12,0x00, /* Sleep 18 (RE2 @0x02194/@0x021B8/@0x021D2) */
    /* +06A */ 0x22,0x05,0x1a,0x01, /* Set(5,0x1A)=1 Ton: statt RE2 se_on(2,5) `36 02 05 01 01 00 70 17 38 ff 00 00` (C-Takt spielt RE2 ROOM1080 Satz 5, Bank VENTIL1080) */
    /* +06E */ 0x09,0x0a,0x0c,0x00, /* Sleep 12 (RE2 @0x021A4/@0x021C8) */
    /* +072 */ 0x3f,0x00,0x11,0x00, /* Plc_motion(0,17,0) Drehtakt 4/5 (RE2 @0x02186/@0x021B4/@0x021CE) */
    /* +076 */ 0x09,0x0a,0x12,0x00, /* Sleep 18 (RE2 @0x02194/@0x021B8/@0x021D2) */
    /* +07A */ 0x22,0x05,0x1a,0x01, /* Set(5,0x1A)=1 Ton: statt RE2 se_on(2,5) `36 02 05 01 01 00 70 17 38 ff 00 00` (C-Takt spielt RE2 ROOM1080 Satz 5, Bank VENTIL1080) */
    /* +07E */ 0x09,0x0a,0x0c,0x00, /* Sleep 12 (RE2 @0x021A4/@0x021C8) */
    /* +082 */ 0x3f,0x00,0x11,0x00, /* Plc_motion(0,17,0) Drehtakt 5/5 (RE2 @0x02186/@0x021B4/@0x021CE) */
    /* +086 */ 0x09,0x0a,0x12,0x00, /* Sleep 18 (RE2 @0x02194/@0x021B8/@0x021D2) */
    /* +08A */ 0x22,0x05,0x1a,0x01, /* Set(5,0x1A)=1 Ton: statt RE2 se_on(2,5) `36 02 05 01 01 00 70 17 38 ff 00 00` (C-Takt spielt RE2 ROOM1080 Satz 5, Bank VENTIL1080) */
    /* +08E */ 0x09,0x0a,0x0f,0x00, /* Sleep 15 (RE2 @0x021E2) */
    /* +092 */ 0x09,0x0a,0x0c,0x00, /* Sleep 12 (RE2 @0x021E6) */
    /* +096 */ 0x3f,0x00,0x10,0xc0, /* Plc_motion(0,16,0xC0) loslassen rueckwaerts, ohne Ueberblendung (RE2 @0x021EC) */
    /* +09A */ 0x09,0x0a,0x32,0x00, /* Sleep 50 (RE2 @0x021F0) */
    /* +09E */ 0x3f,0x00,0x0f,0xc0, /* Plc_motion(0,15,0xC0) Hand zurueck (RE2 @0x021F6) */
    /* +0A2 */ 0x09,0x0a,0x1e,0x00, /* Sleep 30 (RE2 @0x021FA) */
    /* +0A6 */ 0x42,0x00, /* Plc_ret + Nop (RE2 @0x0221E `42` `00`) */
    /* +0A8 */ 0x22,0x02,0x02,0x00, /* Set(2,2)=0 (ROOM11F0 sub17 @0x016EC) */
    /* +0AC */ 0x22,0x02,0x07,0x00, /* Set(2,7)=0 (ROOM2030 sub02 @0x01EC4) */
    /* +0B0 */ 0x22,0x05,0x1b,0x01, /* Set(5,0x1B)=1 Ventil gedreht (Marker fuer den C-Takt; Bank 5 Wort 0 = Raum-Scratch, Raumladen wischt @0x8003ed74; Form der Schalter ROOM2060 sub06 @0x014D8 `22 05 06 01`) */
    /* +0B4 */ 0x08,0x00, /* Endif  */
    /* +0B6 */ 0x01,0x00, /* Evt_end */
};

/* re15_r41c_prg_2030_ventil_08 (184 B) */
static const unsigned char re15_r41c_prg_2030_ventil_08[184] = {
    /* +000 */ 0x2b,0x36,0xff,0xff, /* Message_on 54, Maske ffff - Valve 08 ... Will you turn it? (Frage, Ja/Nein) */
    /* +004 */ 0x02,0x00, /* Evt_next + Nop (ROOM2050 sub04 @0x022EA) */
    /* +006 */ 0x06,0x00,0xac,0x00, /* Ifel_ck (Laenge automatisch) Ja-Zweig */
    /* +00A */ 0x21,0x0c,0x1f,0x00, /* Ck(12,31)=0 = Ja (ROOM2050 sub04 @0x022F0) */
    /* +00E */ 0x22,0x02,0x07,0x01, /* Set(2,7)=1 Pad-Sperre (ROOM2030 sub02 @0x01E30) */
    /* +012 */ 0x22,0x02,0x02,0x01, /* Set(2,2)=1 KI steht (ROOM2050 sub02 @0x0223C) */
    /* +016 */ 0x2e,0x01,0x00,0x00, /* Work_set(1,0) Spieler + Nop (RE2 sub08 @0x0210A `2e 01 00`) */
    /* +01A */ 0x32,0x00,0x90,0x01,0xf8,0xf8,0xac,0x1b, /* Pos_set (400,-1800,7084) vor Rad 08 (Form RE2 sub08 @0x02116 `32 00 30 b2 00 00 fe ac`) */
    /* +022 */ 0x34,0x04,0x00,0x0c, /* Member_set(4 = rot_y, 3072) Blick zur Wand (Form RE2 @0x0212C `34 0f 00 08`) */
    /* +026 */ 0x09,0x0a,0x14,0x00, /* Sleep 20 (RE2 @0x02130) */
    /* +02A */ 0x3f,0x00,0x0f,0x00, /* Plc_motion(0,15,0) Hand zum Rad (RE2 @0x0213C; RBJ Clip 15 = 30 Bilder) */
    /* +02E */ 0x09,0x0a,0x1e,0x00, /* Sleep 30 (RE2 @0x0214A) */
    /* +032 */ 0x3f,0x00,0x10,0x00, /* Plc_motion(0,16,0) Rad fassen (RE2 @0x02156; Clip 16 = 50 Bilder) */
    /* +036 */ 0x09,0x0a,0x1c,0x00, /* Sleep 28 (RE2 @0x02164) */
    /* +03A */ 0x22,0x05,0x1a,0x01, /* Set(5,0x1A)=1 Ton: statt RE2 se_on(2,5) `36 02 05 01 01 00 70 17 38 ff 00 00` (C-Takt spielt RE2 ROOM1080 Satz 5, Bank VENTIL1080) */
    /* +03E */ 0x09,0x0a,0x16,0x00, /* Sleep 22 (RE2 @0x02174) */
    /* +042 */ 0x3f,0x00,0x11,0x00, /* Plc_motion(0,17,0) Drehtakt 1/5 (RE2 @0x02186/@0x021B4/@0x021CE) */
    /* +046 */ 0x09,0x0a,0x12,0x00, /* Sleep 18 (RE2 @0x02194/@0x021B8/@0x021D2) */
    /* +04A */ 0x22,0x05,0x1a,0x01, /* Set(5,0x1A)=1 Ton: statt RE2 se_on(2,5) `36 02 05 01 01 00 70 17 38 ff 00 00` (C-Takt spielt RE2 ROOM1080 Satz 5, Bank VENTIL1080) */
    /* +04E */ 0x09,0x0a,0x0c,0x00, /* Sleep 12 (RE2 @0x021A4/@0x021C8) */
    /* +052 */ 0x3f,0x00,0x11,0x00, /* Plc_motion(0,17,0) Drehtakt 2/5 (RE2 @0x02186/@0x021B4/@0x021CE) */
    /* +056 */ 0x09,0x0a,0x12,0x00, /* Sleep 18 (RE2 @0x02194/@0x021B8/@0x021D2) */
    /* +05A */ 0x22,0x05,0x1a,0x01, /* Set(5,0x1A)=1 Ton: statt RE2 se_on(2,5) `36 02 05 01 01 00 70 17 38 ff 00 00` (C-Takt spielt RE2 ROOM1080 Satz 5, Bank VENTIL1080) */
    /* +05E */ 0x09,0x0a,0x0c,0x00, /* Sleep 12 (RE2 @0x021A4/@0x021C8) */
    /* +062 */ 0x3f,0x00,0x11,0x00, /* Plc_motion(0,17,0) Drehtakt 3/5 (RE2 @0x02186/@0x021B4/@0x021CE) */
    /* +066 */ 0x09,0x0a,0x12,0x00, /* Sleep 18 (RE2 @0x02194/@0x021B8/@0x021D2) */
    /* +06A */ 0x22,0x05,0x1a,0x01, /* Set(5,0x1A)=1 Ton: statt RE2 se_on(2,5) `36 02 05 01 01 00 70 17 38 ff 00 00` (C-Takt spielt RE2 ROOM1080 Satz 5, Bank VENTIL1080) */
    /* +06E */ 0x09,0x0a,0x0c,0x00, /* Sleep 12 (RE2 @0x021A4/@0x021C8) */
    /* +072 */ 0x3f,0x00,0x11,0x00, /* Plc_motion(0,17,0) Drehtakt 4/5 (RE2 @0x02186/@0x021B4/@0x021CE) */
    /* +076 */ 0x09,0x0a,0x12,0x00, /* Sleep 18 (RE2 @0x02194/@0x021B8/@0x021D2) */
    /* +07A */ 0x22,0x05,0x1a,0x01, /* Set(5,0x1A)=1 Ton: statt RE2 se_on(2,5) `36 02 05 01 01 00 70 17 38 ff 00 00` (C-Takt spielt RE2 ROOM1080 Satz 5, Bank VENTIL1080) */
    /* +07E */ 0x09,0x0a,0x0c,0x00, /* Sleep 12 (RE2 @0x021A4/@0x021C8) */
    /* +082 */ 0x3f,0x00,0x11,0x00, /* Plc_motion(0,17,0) Drehtakt 5/5 (RE2 @0x02186/@0x021B4/@0x021CE) */
    /* +086 */ 0x09,0x0a,0x12,0x00, /* Sleep 18 (RE2 @0x02194/@0x021B8/@0x021D2) */
    /* +08A */ 0x22,0x05,0x1a,0x01, /* Set(5,0x1A)=1 Ton: statt RE2 se_on(2,5) `36 02 05 01 01 00 70 17 38 ff 00 00` (C-Takt spielt RE2 ROOM1080 Satz 5, Bank VENTIL1080) */
    /* +08E */ 0x09,0x0a,0x0f,0x00, /* Sleep 15 (RE2 @0x021E2) */
    /* +092 */ 0x09,0x0a,0x0c,0x00, /* Sleep 12 (RE2 @0x021E6) */
    /* +096 */ 0x3f,0x00,0x10,0xc0, /* Plc_motion(0,16,0xC0) loslassen rueckwaerts, ohne Ueberblendung (RE2 @0x021EC) */
    /* +09A */ 0x09,0x0a,0x32,0x00, /* Sleep 50 (RE2 @0x021F0) */
    /* +09E */ 0x3f,0x00,0x0f,0xc0, /* Plc_motion(0,15,0xC0) Hand zurueck (RE2 @0x021F6) */
    /* +0A2 */ 0x09,0x0a,0x1e,0x00, /* Sleep 30 (RE2 @0x021FA) */
    /* +0A6 */ 0x42,0x00, /* Plc_ret + Nop (RE2 @0x0221E `42` `00`) */
    /* +0A8 */ 0x22,0x02,0x02,0x00, /* Set(2,2)=0 (ROOM11F0 sub17 @0x016EC) */
    /* +0AC */ 0x22,0x02,0x07,0x00, /* Set(2,7)=0 (ROOM2030 sub02 @0x01EC4) */
    /* +0B0 */ 0x22,0x05,0x1b,0x01, /* Set(5,0x1B)=1 Ventil gedreht (Marker fuer den C-Takt; Bank 5 Wort 0 = Raum-Scratch, Raumladen wischt @0x8003ed74; Form der Schalter ROOM2060 sub06 @0x014D8 `22 05 06 01`) */
    /* +0B4 */ 0x08,0x00, /* Endif  */
    /* +0B6 */ 0x01,0x00, /* Evt_end */
};

/* re15_r41c_prg_2030_ventil_09 (184 B) */
static const unsigned char re15_r41c_prg_2030_ventil_09[184] = {
    /* +000 */ 0x2b,0x37,0xff,0xff, /* Message_on 55, Maske ffff - Valve 09 ... Will you turn it? (Frage, Ja/Nein) */
    /* +004 */ 0x02,0x00, /* Evt_next + Nop (ROOM2050 sub04 @0x022EA) */
    /* +006 */ 0x06,0x00,0xac,0x00, /* Ifel_ck (Laenge automatisch) Ja-Zweig */
    /* +00A */ 0x21,0x0c,0x1f,0x00, /* Ck(12,31)=0 = Ja (ROOM2050 sub04 @0x022F0) */
    /* +00E */ 0x22,0x02,0x07,0x01, /* Set(2,7)=1 Pad-Sperre (ROOM2030 sub02 @0x01E30) */
    /* +012 */ 0x22,0x02,0x02,0x01, /* Set(2,2)=1 KI steht (ROOM2050 sub02 @0x0223C) */
    /* +016 */ 0x2e,0x01,0x00,0x00, /* Work_set(1,0) Spieler + Nop (RE2 sub08 @0x0210A `2e 01 00`) */
    /* +01A */ 0x32,0x00,0xa6,0xf5,0xf8,0xf8,0xac,0x1b, /* Pos_set (-2650,-1800,7084) vor Rad 09 (Form RE2 sub08 @0x02116 `32 00 30 b2 00 00 fe ac`) */
    /* +022 */ 0x34,0x04,0x00,0x0c, /* Member_set(4 = rot_y, 3072) Blick zur Wand (Form RE2 @0x0212C `34 0f 00 08`) */
    /* +026 */ 0x09,0x0a,0x14,0x00, /* Sleep 20 (RE2 @0x02130) */
    /* +02A */ 0x3f,0x00,0x0f,0x00, /* Plc_motion(0,15,0) Hand zum Rad (RE2 @0x0213C; RBJ Clip 15 = 30 Bilder) */
    /* +02E */ 0x09,0x0a,0x1e,0x00, /* Sleep 30 (RE2 @0x0214A) */
    /* +032 */ 0x3f,0x00,0x10,0x00, /* Plc_motion(0,16,0) Rad fassen (RE2 @0x02156; Clip 16 = 50 Bilder) */
    /* +036 */ 0x09,0x0a,0x1c,0x00, /* Sleep 28 (RE2 @0x02164) */
    /* +03A */ 0x22,0x05,0x1a,0x01, /* Set(5,0x1A)=1 Ton: statt RE2 se_on(2,5) `36 02 05 01 01 00 70 17 38 ff 00 00` (C-Takt spielt RE2 ROOM1080 Satz 5, Bank VENTIL1080) */
    /* +03E */ 0x09,0x0a,0x16,0x00, /* Sleep 22 (RE2 @0x02174) */
    /* +042 */ 0x3f,0x00,0x11,0x00, /* Plc_motion(0,17,0) Drehtakt 1/5 (RE2 @0x02186/@0x021B4/@0x021CE) */
    /* +046 */ 0x09,0x0a,0x12,0x00, /* Sleep 18 (RE2 @0x02194/@0x021B8/@0x021D2) */
    /* +04A */ 0x22,0x05,0x1a,0x01, /* Set(5,0x1A)=1 Ton: statt RE2 se_on(2,5) `36 02 05 01 01 00 70 17 38 ff 00 00` (C-Takt spielt RE2 ROOM1080 Satz 5, Bank VENTIL1080) */
    /* +04E */ 0x09,0x0a,0x0c,0x00, /* Sleep 12 (RE2 @0x021A4/@0x021C8) */
    /* +052 */ 0x3f,0x00,0x11,0x00, /* Plc_motion(0,17,0) Drehtakt 2/5 (RE2 @0x02186/@0x021B4/@0x021CE) */
    /* +056 */ 0x09,0x0a,0x12,0x00, /* Sleep 18 (RE2 @0x02194/@0x021B8/@0x021D2) */
    /* +05A */ 0x22,0x05,0x1a,0x01, /* Set(5,0x1A)=1 Ton: statt RE2 se_on(2,5) `36 02 05 01 01 00 70 17 38 ff 00 00` (C-Takt spielt RE2 ROOM1080 Satz 5, Bank VENTIL1080) */
    /* +05E */ 0x09,0x0a,0x0c,0x00, /* Sleep 12 (RE2 @0x021A4/@0x021C8) */
    /* +062 */ 0x3f,0x00,0x11,0x00, /* Plc_motion(0,17,0) Drehtakt 3/5 (RE2 @0x02186/@0x021B4/@0x021CE) */
    /* +066 */ 0x09,0x0a,0x12,0x00, /* Sleep 18 (RE2 @0x02194/@0x021B8/@0x021D2) */
    /* +06A */ 0x22,0x05,0x1a,0x01, /* Set(5,0x1A)=1 Ton: statt RE2 se_on(2,5) `36 02 05 01 01 00 70 17 38 ff 00 00` (C-Takt spielt RE2 ROOM1080 Satz 5, Bank VENTIL1080) */
    /* +06E */ 0x09,0x0a,0x0c,0x00, /* Sleep 12 (RE2 @0x021A4/@0x021C8) */
    /* +072 */ 0x3f,0x00,0x11,0x00, /* Plc_motion(0,17,0) Drehtakt 4/5 (RE2 @0x02186/@0x021B4/@0x021CE) */
    /* +076 */ 0x09,0x0a,0x12,0x00, /* Sleep 18 (RE2 @0x02194/@0x021B8/@0x021D2) */
    /* +07A */ 0x22,0x05,0x1a,0x01, /* Set(5,0x1A)=1 Ton: statt RE2 se_on(2,5) `36 02 05 01 01 00 70 17 38 ff 00 00` (C-Takt spielt RE2 ROOM1080 Satz 5, Bank VENTIL1080) */
    /* +07E */ 0x09,0x0a,0x0c,0x00, /* Sleep 12 (RE2 @0x021A4/@0x021C8) */
    /* +082 */ 0x3f,0x00,0x11,0x00, /* Plc_motion(0,17,0) Drehtakt 5/5 (RE2 @0x02186/@0x021B4/@0x021CE) */
    /* +086 */ 0x09,0x0a,0x12,0x00, /* Sleep 18 (RE2 @0x02194/@0x021B8/@0x021D2) */
    /* +08A */ 0x22,0x05,0x1a,0x01, /* Set(5,0x1A)=1 Ton: statt RE2 se_on(2,5) `36 02 05 01 01 00 70 17 38 ff 00 00` (C-Takt spielt RE2 ROOM1080 Satz 5, Bank VENTIL1080) */
    /* +08E */ 0x09,0x0a,0x0f,0x00, /* Sleep 15 (RE2 @0x021E2) */
    /* +092 */ 0x09,0x0a,0x0c,0x00, /* Sleep 12 (RE2 @0x021E6) */
    /* +096 */ 0x3f,0x00,0x10,0xc0, /* Plc_motion(0,16,0xC0) loslassen rueckwaerts, ohne Ueberblendung (RE2 @0x021EC) */
    /* +09A */ 0x09,0x0a,0x32,0x00, /* Sleep 50 (RE2 @0x021F0) */
    /* +09E */ 0x3f,0x00,0x0f,0xc0, /* Plc_motion(0,15,0xC0) Hand zurueck (RE2 @0x021F6) */
    /* +0A2 */ 0x09,0x0a,0x1e,0x00, /* Sleep 30 (RE2 @0x021FA) */
    /* +0A6 */ 0x42,0x00, /* Plc_ret + Nop (RE2 @0x0221E `42` `00`) */
    /* +0A8 */ 0x22,0x02,0x02,0x00, /* Set(2,2)=0 (ROOM11F0 sub17 @0x016EC) */
    /* +0AC */ 0x22,0x02,0x07,0x00, /* Set(2,7)=0 (ROOM2030 sub02 @0x01EC4) */
    /* +0B0 */ 0x22,0x05,0x1b,0x01, /* Set(5,0x1B)=1 Ventil gedreht (Marker fuer den C-Takt; Bank 5 Wort 0 = Raum-Scratch, Raumladen wischt @0x8003ed74; Form der Schalter ROOM2060 sub06 @0x014D8 `22 05 06 01`) */
    /* +0B4 */ 0x08,0x00, /* Endif  */
    /* +0B6 */ 0x01,0x00, /* Evt_end */
};

/* re15_r41c_prg_2030_ventile_flut (96 B) */
static const unsigned char re15_r41c_prg_2030_ventile_flut[96] = {
    /* +000 */ 0x2b,0x2e,0xff,0xff, /* Message_on 46, Maske ffff - WARNING: The water will rise in one minute. Please leave the room immediately! (Nachtrag 19) */
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
    /* +000 */ 0x2b,0x2e,0xff,0xff, /* Message_on 46, Maske ffff - WARNING: ... Please leave the room immediately! (Zustand waehrend des Zaehlers) */
    /* +004 */ 0x02,0x00, /* Evt_next + Nop (ROOM2050 sub04 @0x022EA) */
    /* +006 */ 0x01,0x00, /* Evt_end */
};

/* re15_r41c_prg_2030_ventile_vorher (8 B) */
static const unsigned char re15_r41c_prg_2030_ventile_vorher[8] = {
    /* +000 */ 0x2b,0x31,0xff,0xff, /* Message_on 49, Maske ffff - It is locked. = Original ROOM20A0 msg 1 @0x203C */
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

/* re15_r41c_prg_2010_kasten_leer (8 B) */
static const unsigned char re15_r41c_prg_2010_kasten_leer[8] = {
    /* +000 */ 0x2b,0x28,0xff,0xff, /* Message_on 40, Maske ffff - A fuse box for the valve controls. | One of the fuses is missing... (ROOM2060 msg 3) */
    /* +004 */ 0x02,0x00, /* Evt_next + Nop (ROOM2050 sub04 @0x022EA) */
    /* +006 */ 0x01,0x00, /* Evt_end */
};

/* re15_r41c_prg_2010_kasten_frage (66 B) */
static const unsigned char re15_r41c_prg_2010_kasten_frage[66] = {
    /* +000 */ 0x2b,0x28,0xff,0xff, /* Message_on 40, Maske ffff - A fuse box ... | One of the fuses is missing... */
    /* +004 */ 0x02,0x00, /* Evt_next + Nop (ROOM2050 sub04 @0x022EA) */
    /* +006 */ 0x2b,0x29,0xff,0xff, /* Message_on 41, Maske ffff - Will you use the Fuse? = ROOM2060 msg 4 @0x1855 (Form sub18 @0x0167E) */
    /* +00A */ 0x02,0x00, /* Evt_next + Nop (ROOM2050 sub04 @0x022EA) */
    /* +00C */ 0x06,0x00,0x30,0x00, /* Ifel_ck (Laenge automatisch) Ja-Zweig */
    /* +010 */ 0x21,0x0c,0x1f,0x00, /* Ck(12,31)=0 = Ja (ROOM2050 sub04 @0x022F0) */
    /* +014 */ 0x22,0x02,0x07,0x01, /* Set(2,7)=1 Pad-Sperre (ROOM2030 sub02 @0x01E30) */
    /* +018 */ 0x22,0x02,0x02,0x01, /* Set(2,2)=1 KI steht (ROOM2050 sub02 @0x0223C) */
    /* +01C */ 0x22,0x09,0x7d,0x01, /* Set(9,125)=1 Sicherung im Ventilkasten (Form ROOM2060 sub19 @0x0169E `22 03 90 01`) */
    /* +020 */ 0x22,0x05,0x1f,0x01, /* Set(5,0x1F)=1 Standbild der Ventile + Aufschliess-Ton (Marker fuer den C-Takt; Bank 5 Wort 0, ROOM2010 benutzt Bank 5 nicht) */
    /* +024 */ 0x0f,0x04,0x08,0x00, /* While (Form ROOM2000 @0x01AD4): solange das Standbild offen ist */
    /* +028 */ 0x21,0x05,0x1f,0x01, /* Ck(5,0x1F)=1 */
    /* +02C */ 0x02,0x00, /* Evt_next + Nop (ROOM2050 sub04 @0x022EA) */
    /* +02E */ 0x10,0x00, /* Ewhile */
    /* +030 */ 0x2b,0x2a,0xff,0xff, /* Message_on 42, Maske ffff - You've used the Fuse. = ROOM2060 msg 5 @0x1875 (sub19 @0x016C8) */
    /* +034 */ 0x02,0x00, /* Evt_next + Nop (ROOM2050 sub04 @0x022EA) */
    /* +036 */ 0x22,0x02,0x02,0x00, /* Set(2,2)=0 (ROOM11F0 sub17 @0x016EC) */
    /* +03A */ 0x22,0x02,0x07,0x00, /* Set(2,7)=0 (ROOM2030 sub02 @0x01EC4) */
    /* +03E */ 0x08,0x00, /* Endif  */
    /* +040 */ 0x01,0x00, /* Evt_end */
};

/* re15_r41c_prg_2010_kasten_drin (28 B) */
static const unsigned char re15_r41c_prg_2010_kasten_drin[28] = {
    /* +000 */ 0x2b,0x2b,0xff,0xff, /* Message_on 43, Maske ffff - The fuse is in place. (ROOM2060 msg 6) | Will you take the Fuse? (ROOM2030 msg 2) */
    /* +004 */ 0x02,0x00, /* Evt_next + Nop (ROOM2050 sub04 @0x022EA) */
    /* +006 */ 0x06,0x00,0x10,0x00, /* Ifel_ck (Laenge automatisch) Ja-Zweig */
    /* +00A */ 0x21,0x0c,0x1f,0x00, /* Ck(12,31)=0 = Ja (ROOM2050 sub04 @0x022F0) */
    /* +00E */ 0x22,0x09,0x7d,0x00, /* Set(9,125)=0 Sicherung wieder genommen (Form ROOM2030 sub06 @0x01FF2 `22 03 6c 01`) */
    /* +012 */ 0x2b,0x2c,0xff,0xff, /* Message_on 44, Maske ffff - You've taken the Fuse. = ROOM2030 msg 3 @0x20AC (sub06 @0x02000) */
    /* +016 */ 0x02,0x00, /* Evt_next + Nop (ROOM2050 sub04 @0x022EA) */
    /* +018 */ 0x08,0x00, /* Endif  */
    /* +01A */ 0x01,0x00, /* Evt_end */
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
    { "2030_ventil_01", re15_r41c_prg_2030_ventil_01, 184 },
    { "2030_ventil_02", re15_r41c_prg_2030_ventil_02, 184 },
    { "2030_ventil_03", re15_r41c_prg_2030_ventil_03, 184 },
    { "2030_ventil_07", re15_r41c_prg_2030_ventil_07, 184 },
    { "2030_ventil_08", re15_r41c_prg_2030_ventil_08, 184 },
    { "2030_ventil_09", re15_r41c_prg_2030_ventil_09, 184 },
    { "2030_ventile_flut", re15_r41c_prg_2030_ventile_flut, 96 },
    { "2030_ventile_falsch", re15_r41c_prg_2030_ventile_falsch, 24 },
    { "2030_ventile_laeuft", re15_r41c_prg_2030_ventile_laeuft, 8 },
    { "2030_ventile_vorher", re15_r41c_prg_2030_ventile_vorher, 8 },
    { "2030_plakat", re15_r41c_prg_2030_plakat, 14 },
    { "2010_kasten_leer", re15_r41c_prg_2010_kasten_leer, 8 },
    { "2010_kasten_frage", re15_r41c_prg_2010_kasten_frage, 66 },
    { "2010_kasten_drin", re15_r41c_prg_2010_kasten_drin, 28 },
    { "2050_hebel_sperre", re15_r41c_prg_2050_hebel_sperre, 8 },
    { "2050_pult_ein", re15_r41c_prg_2050_pult_ein, 44 },
    { "2050_pult_zustand", re15_r41c_prg_2050_pult_zustand, 8 },
};
#define RE15_R41C_PROGRAMME_N ((int)(sizeof re15_r41c_programme / sizeof re15_r41c_programme[0]))
