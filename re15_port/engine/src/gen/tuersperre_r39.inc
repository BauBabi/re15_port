/* GENERIERT von re15_port/tools/r39_johnruf/tuersperre_bauen.py - NICHT HAND-EDITIEREN.
 * Runde 39 Spur P, Nachbesserung 1-3: Tuersperren 1041/1051 (Panikphase) und 11B1 (mit Strom) - Texte, Szenen-Huelle 10B1,
 * Anker. Dossier: analysis/befunde_runde39/P_johnruf1041.md Abschnitt 8. */

#define RE15_R39P_TS_ANKER_T1041_OFF 0x010B8u
#define RE15_R39P_TS_ANKER_T1041_LEN 32u
#define RE15_R39P_TS_ANKER_T1041_FNV 0x6B57D50Cu
#define RE15_R39P_TS_ANKER_T1051_OFF 0x00B5Au
#define RE15_R39P_TS_ANKER_T1051_LEN 32u
#define RE15_R39P_TS_ANKER_T1051_FNV 0xC8625F8Bu
#define RE15_R39P_TS_ANKER_T10B1_OFF 0x016F2u
#define RE15_R39P_TS_ANKER_T10B1_LEN 32u
#define RE15_R39P_TS_ANKER_T10B1_FNV 0x1D16A550u
#define RE15_R39P_TS_ANKER_S10B1_OFF 0x01812u
#define RE15_R39P_TS_ANKER_S10B1_LEN 640u
#define RE15_R39P_TS_ANKER_S10B1_FNV 0x60D166C0u
#define RE15_R39P_TS_ANKER_T11B1_OFF 0x00F9Cu
#define RE15_R39P_TS_ANKER_T11B1_LEN 32u
#define RE15_R39P_TS_ANKER_T11B1_FNV 0xB93CFC61u
#define RE15_R39P_TS_ANKER_M11B1_OFF 0x0179Fu
#define RE15_R39P_TS_ANKER_M11B1_LEN 23u
#define RE15_R39P_TS_ANKER_M11B1_FNV 0x7B6A7A04u

/* k_r39p_ts_szene10b1: 22 Byte
 *   +0000 18 04                    Gosub sub04 = die Szene UNVERAENDERT aus dem RDT (@0x0195A..@0x01A90, msg 0..5); ihr Aot_on @0x01A8E `47 01` trifft den gesperrten Slot 1 -> Text msg 40 statt Raumwechsel (Form Gosub-Huelle Spur O / Runde 38 F, Gosub @0x8003fc04)
 *   +0002 02 00                    ROOM1051 sub02 @0x00CCC Evt_next + Nop: die VM parkt unter RE15_PAUSE_SCD, bis der Text zu ist (FUN_8003f038 @0x8003f040-4c, Maske 0xffff0000 @0x800430a4)
 *   +0004 22 02 07 00              ROOM1051 sub02 @0x00D90 Set(2,7)=0 Pad frei (sub04 @0x01966 setzt sie, loest sie nie)
 *   +0008 22 01 1b 00              ROOM1051 sub02 @0x00D94 Set(1,0x1B)=0 Balken weg (sub04 @0x0196A)
 *   +000C 3c 01                    ROOM1051 sub02 @0x00D98 Cut_auto 1
 *   +000E 2e 01 00 00              ROOM1051 sub02 @0x00D9A Work_set(1,0) Elza + Nop
 *   +0012 42 00                    ROOM1051 sub02 @0x00D9E Plc_ret + Nop (Elza wieder frei; sub04 @0x01986 Work_set(1,0))
 *   +0014 01 00                    ROOM1051 sub02 @0x00DA2 Evt_end
 */
static const uint8_t k_r39p_ts_szene10b1[22] = {
    0x18,0x04,0x02,0x00,0x22,0x02,0x07,0x00,0x22,0x01,0x1b,0x00,0x3c,0x01,0x2e,0x01,
    0x00,0x00,0x42,0x00,0x01,0x00,
};

/* ROOM1041 msg 41 / ROOM1051 msg 40: NUTZER-VORGABE "I have to rescue John! He is in danger!" (AUFTRAG.md Nachtrag 12), Erzaehlform
 * `04 02 .. 08 .. 01 00` wie die Tuersperren 1141/11C0 und ROOM1090 msg 7 @0x28E6  [Zeilenbreiten 150, 105 px <= 271] */
static const uint8_t k_r39p_ts_msg_rettung[43] = {
    0x04,0x02,0x25,0x00,0x44,0x3d,0x52,0x41,0x00,0x50,0x4b,0x00,0x4e,0x41,0x4f,0x3f,
    0x51,0x41,0x00,0x26,0x4b,0x44,0x4a,0x1a,0x08,0x24,0x41,0x00,0x45,0x4f,0x00,0x45,
    0x4a,0x00,0x40,0x3d,0x4a,0x43,0x41,0x4e,0x1a,0x01,0x00,
};

/* ROOM10B1 msg 40: RE2 ROOM21A0 msg 4 @0x02963 " / The lid is firmly closed.<fd>It's impossible for me / to open it." in RE1.5-Kodierung (Kopf des Werkzeugs)
 *   [Zeilenbreiten 160, 144, 68 px <= 271] */
static const uint8_t k_r39p_ts_msg_deckel[66] = {
    0x04,0x02,0x08,0x30,0x44,0x41,0x00,0x48,0x45,0x40,0x00,0x45,0x4f,0x00,0x42,0x45,
    0x4e,0x49,0x48,0x55,0x00,0x3f,0x48,0x4b,0x4f,0x41,0x40,0x57,0x02,0x00,0x25,0x50,
    0x3a,0x4f,0x00,0x45,0x49,0x4c,0x4b,0x4f,0x4f,0x45,0x3e,0x48,0x41,0x00,0x42,0x4b,
    0x4e,0x00,0x49,0x41,0x08,0x50,0x4b,0x00,0x4b,0x4c,0x41,0x4a,0x00,0x45,0x50,0x57,
    0x01,0x00,
};
