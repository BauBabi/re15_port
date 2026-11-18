/*
 * RE1.5 Rebuilt — Runde 42 Strang "rest", Stufe 3: ungenutzte Capcom-/80-%-Inhalte STAGE2-6 (U-05 501 Rechner, U-01 3091 Pochen
 * + D309 #21/#24/#25, U-02 3091 "There's a power supply.", U-11 402 Wandplan -> Blatt 8; Stufe 5: U-08 Elzas Tuer 4001 -> 401;
 * Stufe 7: Pochen, Blicke und Zielhaltungen in 3091 auf die Lobby-Tuer, durch die Elza am Ende geht).
 * Herleitung, Belege und Formen: include/re15_ungenutzt_r42.h. Kein Asset-Patch: die Programme sind Original-Befehle; 3091 sub07
 * wird zur Laufzeit aus den RDT-Bytes kopiert, nachdem die Pruefbytes stimmen (sonst laeuft das Original).
 */
#include "re15_ungenutzt_r42.h"

#include "re15_scd.h"
#include "re15_rdt.h"
#include "re15_room.h"
#include "re15_msg.h"
#include "re15_engine.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "gen/ungenutzt_r42_texte.inc"

extern unsigned g_current_room_id;
extern int g_re15_voice_laeuft, g_re15_voice_restbilder;

#define U42_LOG(...) do { fprintf(stderr, "[ungenutzt42] F%u ", (unsigned)g_engine.frame_count); fprintf(stderr, __VA_ARGS__); \
                          fputc('\n', stderr); } while (0)

unsigned g_re15_u42_zaehler[6];

#define LE16(v)            (uint8_t)((uint16_t)(v) & 0xff), (uint8_t)(((uint16_t)(v) >> 8) & 0xff)
#define OP_SET(b,i,v)      0x22, (b), (i), (v)
#define OP_SLEEP(n)        0x09, 0x0a, LE16(n)
#define OP_WARTE(bit)      0x11, 0x00, 0x08, 0x00, 0x02, 0x00, 0x12, 0x04, 0x21, 0x05, (bit), 0x00   /* ROOM2040 sub02 @0x01650 */

static int basis(uint16_t room, uint16_t b) { return (uint16_t)(room & 0xFFFEu) == b; }

static const uint8_t *rdt_bytes(int off, const uint8_t *soll, int n)
{
    const re15_rdt_t *r = &g_room_rdt;
    if (!r->raw || off < 0 || off + n > (int)r->raw_size) return NULL;
    if (soll && memcmp(r->raw + off, soll, (size_t)n) != 0) return NULL;
    return r->raw + off;
}

/* ==== U-05: 501 Rechner (Disc #40 vor dem Speicher-Text) ================================================================= */
static const uint8_t k_rechner_5010[] = {
    0x29, 0x02,                          /* Cut_chg 2 Rechner           = ROOM5010 sub03 @0x004F6                            */
    0x2b, RE15_U42_5010_SATZ, 0xff, 0xff,/* Message_on 2 = Disc #40     Form @0x004F8; Text Mod msg 2 (Slot 3 @0x003E2 tot)  */
    0x02, 0x00,                          /* Evt_next                    Form @0x004FC                                        */
    0x2b, RE15_U42_5010_SPEICHER, 0xff, 0xff, /* Message_on 6 Speichern = @0x004F8                                          */
    0x02, 0x00,                          /*                             = @0x004FC                                           */
    0x2a, 0x00, 0x3c, 0x01, 0x01, 0x00,  /* Cut_old, Cut_auto 1, Evt_end = @0x004FE..@0x00503                                */
};
static const uint8_t k_rechner_5011[] = {
    0x29, 0x02,                          /* Cut_chg 2 Rechner           = ROOM5011 sub08 @0x00E34                            */
    0x2b, RE15_U42_5011_SATZ, 0xff, 0xff,/* Message_on 40 = Disc #40   Form @0x00E36; Text Mod msg 40 (Slot 4 @0x003F6 tot) */
    0x02, 0x00,
    0x2b, RE15_U42_5011_SPEICHER, 0xff, 0xff, /* Message_on 45 Speichern = @0x00E36                                         */
    0x02, 0x00,
    0x2a, 0x00, 0x3c, 0x01, 0x01, 0x00,  /* = @0x00E3C..@0x00E41                                                             */
};
static const uint8_t k_sub03_5010[14] = { 0x29, 0x02, 0x2b, 0x06, 0xff, 0xff, 0x02, 0x00, 0x2a, 0x00, 0x3c, 0x01, 0x01, 0x00 };
static const uint8_t k_sub08_5011[14] = { 0x29, 0x02, 0x2b, 0x2d, 0xff, 0xff, 0x02, 0x00, 0x2a, 0x00, 0x3c, 0x01, 0x01, 0x00 };

/* ==== U-11: 402 Wandplan -> Blatt 8 ======================================================================================= */
static const uint8_t k_plan[] = {
    0x29, 0x03,                          /* Cut_chg 3                   = ROOM4020 sub11 @0x00B06                            */
    0x2b, 0x00, 0xff, 0xff,              /* Message_on 0 "There's a map of the entire area." = @0x00B08                       */
    0x02, 0x00,                          /* Evt_next                    = @0x00B0C                                           */
    0x06, 0x00, 0x10, 0x00,              /* Ifel_ck (Ck 4 + Set 4 + Msg 4 + Evt_next 2 + Endif 2; Form ROOM3091 sub06 @0x016B4) */
    0x21, RE15_R42_BANK, RE15_R42_BIT_PLAN_FORSCHUNG, 0x00,   /* Ck(9,36)=0  Form ROOM5030 sub00 @0x01086 `21 03 73 00`      */
    0x22, RE15_R42_BANK, RE15_R42_BIT_PLAN_FORSCHUNG, 0x01,   /* Set(9,36)   Form ROOM5030 sub02 @0x010C8 `22 03 73 01`      */
    0x2b, RE15_U42_PLAN_MSG, 0xff, 0xff, /* Message_on 2 = ROOM5030 msg 1 Form @0x010CC `2b 01 ff ff`                         */
    0x02, 0x00,                          /*                             Form @0x010D0                                        */
    0x08, 0x00,                          /* Endif                       Form @0x010D2                                        */
    0x2a, 0x00, 0x3c, 0x01, 0x01, 0x00,  /* Cut_old, Cut_auto 1, Evt_end = ROOM4020 sub11 @0x00B0E..@0x00B13                 */
};
static const uint8_t k_sub11_4020[14] = { 0x29, 0x03, 0x2b, 0x00, 0xff, 0xff, 0x02, 0x00, 0x2a, 0x00, 0x3c, 0x01, 0x01, 0x00 };

/* ==== U-02: 3091 sub04 Schalter mit "There's a power supply." ============================================================ */
static const uint8_t k_schalter[] = {
    0x06, 0x00, 0x0c, 0x00,              /* Ifel_ck (Ck 4 + Msg 4 + Evt_next 2 + Endif 2)                                     */
    0x21, 0x04, 0x08, 0x01,              /* Ck(4,8)=1 Strom an          = ROOM3091 sub04 @0x015CE                            */
    0x2b, 0x02, 0xff, 0xff, 0x02, 0x00,  /* msg 2 "This breaker switch is flipped down..." = @0x015D2..@0x015D7               */
    0x08, 0x00,                          /* Endif                       Form ROOM3091 sub06 @0x016C6                         */
    0x06, 0x00, 0x12, 0x00,              /* Ifel_ck (Ck 4 + Msg 4 + 2 + Msg 4 + 2 + Endif 2)                                  */
    0x21, 0x04, 0x08, 0x00,              /* Ck(4,8)=0 kein Strom (Sherry allein, sub05 @0x016AA setzt (4,8))                   */
    0x2b, RE15_U42_MSG_STROM, 0xff, 0xff, 0x02, 0x00,   /* msg 35 "There's a power supply." (80 % T25; PORT-WAHL Stelle: am
                                                         * Schalter-Platz Slot 5, das 80-%-Foto zeigt eher Cut 6), Form @0x015DC */
    0x2b, 0x03, 0xff, 0xff, 0x02, 0x00,  /* msg 3 "I can't reach the breaker switch..." = @0x015DC..@0x015E1                  */
    0x08, 0x00,
    0x01, 0x00,                          /* Evt_end                     = @0x015E4                                           */
};
static const uint8_t k_sub04_3091[28] = { 0x06, 0x00, 0x0e, 0x00, 0x21, 0x04, 0x08, 0x01, 0x2b, 0x02, 0xff, 0xff, 0x02, 0x00,
                                          0x07, 0x00, 0x0c, 0x00, 0x2b, 0x03, 0xff, 0xff, 0x02, 0x00, 0x00, 0x00, 0x01, 0x00 };

/* ==== U-01: 3091 sub07 mit D309 #21/#24/#25, Pochen + Blicke an der Lobby-Tuer (Stufe 7) ================================= */
/* Pruefbytes der Stellen, die geaendert oder eingefuegt werden (Mod ROOM3091). */
static const uint8_t k_neck_lobby[10] = { 0x41, 0x01, LE16(RE15_U42_LOBBY_X), 0x00, 0x00, LE16(RE15_U42_LOBBY_Z), 0x64, 0x00 };
static const uint8_t k_dest_e[8]  = { 0x40, 0x00, 0x09, 0x20, LE16(RE15_U42_LOBBY_X), LE16(RE15_U42_LOBBY_Z) };   /* @0x1AB0 */
static const uint8_t k_dest_j[8]  = { 0x40, 0x00, 0x09, 0x21, LE16(RE15_U42_LOBBY_X), LE16(RE15_U42_LOBBY_Z) };   /* @0x1ACA */
static const uint8_t k_schlag[12] = { 0x36, 0x02, 0x0d, 0x01, 0x00, 0x00, LE16(RE15_U42_HOF_X), 0x00, 0x00, LE16(RE15_U42_HOF_Z) };
static const uint8_t k_vor_25[8]  = { 0x09, 0x0a, 0x1e, 0x00, 0x2b, 0x19, 0x00, 0x00 };   /* @0x1BBE Sleep 30, @0x1BC2 msg 25 */
static const uint8_t k_vor_29[8]  = { 0x09, 0x0a, 0x46, 0x00, 0x2e, 0x02, 0x01, 0x00 };   /* @0x1C60 Sleep 70, @0x1C64 Work 2,1 */
static const uint8_t k_kopf07[8]  = { 0x2b, 0x20, 0xff, 0xff, 0x02, 0x00, 0x22, 0x04 };   /* @0x16CA msg 32, Evt_next, Set(4,..) */
static const uint8_t k_ende07[4]  = { 0x47, 0x00, 0x01, 0x00 };                           /* @0x1DAE Aot_on 0, Evt_end */
static const uint8_t k_tuer0_rect[8] = { LE16(RE15_U42_TUER0_RX), LE16(RE15_U42_TUER0_RZ), LE16(RE15_U42_TUER0_RW),
                                         LE16(RE15_U42_TUER0_RD) };                       /* @0x1338 / @0x135C Slot 0 Rechteck */
static const int     k_schlag_off[4] = { 0x1A62, 0x1A72, 0x1A82, 0x1A92 };              /* Mod Se_on 0x0D (Disc @0x1778..@0x17A8) */
static const uint8_t k_geste_j0[4] = { 0x3f, 0x00, 0x04, 0x00 };                          /* @0x1B2A Plc_motion 4 (John nach msg 23) */
static const uint8_t k_geste_e0[4] = { 0x3f, 0x00, 0x05, 0x00 };                          /* @0x1BCA Plc_motion 5 (Elza nach msg 25) */
static const uint8_t k_geste_j1[4] = { 0x3f, 0x00, 0x03, 0x00 };                          /* @0x1BF6 Plc_motion 3 (John nach msg 26) */

#define U42_GESTE_J_OFF  0x1B2A   /* [@0x1B2A, @0x1B5A): Johns Gesten nach msg 23 (4,4,0,0,8; 25+25+25+25+20 Bilder) */
#define U42_GESTE_J_ENDE 0x1B5A
#define U42_GESTE_E_OFF  0x1BCA   /* [@0x1BCA, @0x1BEE): Elzas Gesten nach msg 25 (5,5,2,9; 25+25+50+20 Bilder) */
#define U42_GESTE_E_ENDE 0x1BEE
#define U42_GESTE_J2_OFF  0x1BF6  /* [@0x1BF6, @0x1C0A): Johns Gesten nach msg 26 "Will do!" (3,3; 50+51 Bilder) - fuer msg 36 */
#define U42_GESTE_J2_ENDE 0x1C0A
#define U42_EIN_JOHN     0x1BC2   /* vor msg 25 */
#define U42_EIN_ELZA     0x1C64   /* vor Work 2,1 / msg 29 */

static uint8_t s_szene[(RE15_U42_SUB07_ENDE - RE15_U42_SUB07_OFF) + 256];
static int     s_szene_n;

static void put(int *k, const void *b, int n) { if (*k + n <= (int)sizeof s_szene) { memcpy(s_szene + *k, b, (size_t)n); } *k += n; }

static int pruefen07(void)
{
    return rdt_bytes(RE15_U42_SUB07_OFF, k_kopf07, 8) && rdt_bytes(RE15_U42_SUB07_ENDE - 4, k_ende07, 4) &&
           rdt_bytes(k_schlag_off[0], k_schlag, 12) && rdt_bytes(k_schlag_off[1], k_schlag, 12) &&
           rdt_bytes(k_schlag_off[2], k_schlag, 12) && rdt_bytes(k_schlag_off[3], k_schlag, 12) &&
           rdt_bytes(0x1332 + 6, k_tuer0_rect, 8) && rdt_bytes(0x1356 + 6, k_tuer0_rect, 8) &&
           rdt_bytes(0x1AA6, k_neck_lobby, 10) && rdt_bytes(0x1AB0, k_dest_e, 8) &&
           rdt_bytes(0x1AC0, k_neck_lobby, 10) && rdt_bytes(0x1ACA, k_dest_j, 8) &&
           rdt_bytes(U42_EIN_JOHN - 4, k_vor_25, 8) && rdt_bytes(U42_EIN_ELZA - 4, k_vor_29, 8) &&
           rdt_bytes(U42_GESTE_J_OFF, k_geste_j0, 4) && rdt_bytes(U42_GESTE_E_OFF, k_geste_e0, 4) &&
           rdt_bytes(U42_GESTE_J2_OFF, k_geste_j1, 4);
}

/* Eine neue Zeile in der Form der Mod-Zeilen (Work_set des Sprechers, Message_on Maske 0, Gestenblock) und danach WARTEN, bis
 * Zeile und Stimme zu Ende sind (Signal RE15_U42_SIG_FREI vom Takt; Form story3b_r42.c OP_ZEILE). */
static void zeile(int *k, uint8_t work_idx, uint8_t msg, int geste_off, int geste_ende)
{
    const uint8_t *r = g_room_rdt.raw;
    const uint8_t kopf[] = { OP_SET(5, RE15_U42_SIG_FREI, 0),
                             0x2e, 0x02, work_idx, 0x00,          /* Work_set (2,i) + Nop, Form @0x01B16 / @0x01C64          */
                             0x2b, msg, 0x00, 0x00 };             /* Message_on Maske 0 (Untertitel), Form @0x01B26           */
    const uint8_t fuss[] = { OP_SLEEP(2), OP_WARTE(RE15_U42_SIG_FREI) };
    put(k, kopf, (int)sizeof kopf);
    put(k, r + geste_off, geste_ende - geste_off);
    put(k, fuss, (int)sizeof fuss);
}

static int szene_bauen(void)
{
    s_szene_n = 0;
    if (!pruefen07()) return 0;
    const uint8_t *r = g_room_rdt.raw;
    int k = 0;
    /* [@0x16CA, @0x1BC2) Byte fuer Byte; die Blick-/Drehziele @0x1AA6/@0x1AB0/@0x1AC0/@0x1ACA bleiben das Mod-Ziel der Lobby-Tuer
     * (-28481,-155) (Stufe 7, NUTZERWUNSCH 18.11. "warum zielen sie denn jetzt auf die falsche Tür?"). Nur der Ort der vier
     * Schlaege pc[6..11] wandert in die Mitte des Rechtecks der Lobby-Tuer Slot 0 (PORT-WAHL, re15_ungenutzt_r42.h (a2)). */
    const int a = RE15_U42_SUB07_OFF;
    put(&k, r + a, U42_EIN_JOHN - a);
    const uint8_t ort[6] = { LE16(RE15_U42_POCH_X), LE16(RE15_U42_POCH_Y), LE16(RE15_U42_POCH_Z) };
    for (int i = 0; i < 4; i++) memcpy(s_szene + (k_schlag_off[i] - a) + 6, ort, sizeof ort);
    /* John msg 33 (D309 #21) vor Elzas msg 25 */
    zeile(&k, 0x00, RE15_U42_MSG_JOHN, U42_GESTE_J_OFF, U42_GESTE_J_ENDE);
    /* [@0x1BC2, @0x1C64) */
    put(&k, r + U42_EIN_JOHN, U42_EIN_ELZA - U42_EIN_JOHN);
    /* Elza msg 34 (D309 #24) nach Sherrys msg 28 */
    zeile(&k, 0x01, RE15_U42_MSG_ELZA, U42_GESTE_E_OFF, U42_GESTE_E_ENDE);
    /* John msg 36 (D309 #25 「任せとけ！」, Disc-Antwort auf #24) direkt danach, Gesten wie zu "Will do!" (G2 Pruefung s3_r1) */
    zeile(&k, 0x00, RE15_U42_MSG_JOHN2, U42_GESTE_J2_OFF, U42_GESTE_J2_ENDE);
    /* [@0x1C64, @0x1DB2) */
    put(&k, r + U42_EIN_ELZA, RE15_U42_SUB07_ENDE - U42_EIN_ELZA);
    if (k > (int)sizeof s_szene) return 0;
    s_szene_n = k;
    return 1;
}

/* ==== U-08: 4001 Tuer Slot 1 nach 401 fuer Elza ========================================================================== */
/* Mod ROOM4001 main00 @0x1084, 32 Byte (Rechteckform, sat 0x31). Disc ROOM4001 @0x1012 ist bis auf pc[2] (0x02) derselbe Satz. */
static const uint8_t k_tuer401_mod[32] = {
    0x3b, RE15_U42_TUER401_SLOT, 0x00, 0x31, 0x00, 0x00,
    LE16(-17100), LE16(-4600), LE16(1600), LE16(2200),          /* Rechteck @0x108A..@0x1091 `34 bd 08 ee 40 06 98 08`       */
    LE16(-21044), LE16(0), LE16(-24000), LE16(-2048),           /* Ankunft in 401 @0x1092..@0x1099 `cc ad 00 00 40 a2 00 f8` */
    0x03, 0x01, 0x00,                                           /* Stufe 3, Raum 01, Cut 0 @0x109A..@0x109C                  */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,                   /* @0x109D..@0x10A3                                          */
};

uint8_t re15_ungenutzt_r42_tuer_sce(uint16_t room_id, const uint8_t *pc)
{
    const uint8_t sce = pc[2];
    if (room_id != RE15_U42_RAUM_4001) return sce;
    const re15_rdt_t *r = &g_room_rdt;
    if (!r->raw || pc < r->raw || pc + sizeof k_tuer401_mod > r->raw + r->raw_size) return sce;
    if ((int)(pc - r->raw) != RE15_U42_TUER401_OFF) return sce;
    if (memcmp(pc, k_tuer401_mod, sizeof k_tuer401_mod) != 0) {
        g_re15_u42_zaehler[4]++;
        U42_LOG("ROOM4001 @0x%04X: Tuersatz anders als die Mod - unveraendert", RE15_U42_TUER401_OFF);
        return sce;
    }
    g_re15_u42_zaehler[5]++;
    U42_LOG("ROOM4001 Slot %d @0x%04X: Tuer nach 401 offen (sce %d wie Disc @0x1012 statt %d), U-08", RE15_U42_TUER401_SLOT,
            RE15_U42_TUER401_OFF, RE15_U42_TUER401_SCE, sce);
    return RE15_U42_TUER401_SCE;
}

/* ==== Texte ============================================================================================================= */
const uint8_t *re15_ungenutzt_r42_text(uint16_t room_id, uint8_t id, int *out_len)
{
    for (int i = 0; i < U42_TEXTE_N; i++)
        if (k_u42_texte[i].raum == room_id && k_u42_texte[i].id == id) {
            if (out_len) *out_len = k_u42_texte[i].n;
            return k_u42_texte[i].b;
        }
    if (out_len) *out_len = 0;
    return NULL;
}

void re15_ungenutzt_r42_texte(uint16_t room_id)
{
    int n = 0;
    for (int i = 0; i < U42_TEXTE_N; i++)
        if (k_u42_texte[i].raum == room_id) {
            re15_msg_install_text(k_u42_texte[i].id, k_u42_texte[i].b, (size_t)k_u42_texte[i].n);
            const int d = re15_msg_compute_duration(k_u42_texte[i].b, k_u42_texte[i].n, 0);   /* wie re15_msg_load_room_block */
            if (d > 0 && d < 65535) re15_msg_install_durations(k_u42_texte[i].id, d);
            n++;
        }
    if (n) U42_LOG("ROOM%04X: %d Nachrichten eingesetzt (nach den RDT-Texten)", (unsigned)room_id, n);
}

/* ==== Haken ============================================================================================================== */
const uint8_t *re15_ungenutzt_r42_ereignis(uint16_t room_id, uint8_t event_id)
{
    if (room_id == RE15_U42_RAUM_5010 && event_id == RE15_U42_5010_EREIGNIS) {
        if (!rdt_bytes(RE15_U42_5010_SUB_OFF, k_sub03_5010, 14)) { g_re15_u42_zaehler[4]++; return NULL; }
        g_re15_u42_zaehler[0]++;
        U42_LOG("ROOM5010 Ereignis 3: Rechner - erst Disc #40 (msg 2), dann Speichern (msg 6), U-05");
        return k_rechner_5010;
    }
    if (room_id == RE15_U42_RAUM_5011 && event_id == RE15_U42_5011_EREIGNIS) {
        if (!rdt_bytes(RE15_U42_5011_SUB_OFF, k_sub08_5011, 14)) { g_re15_u42_zaehler[4]++; return NULL; }
        g_re15_u42_zaehler[0]++;
        U42_LOG("ROOM5011 Ereignis 8: Rechner - erst Disc #40 (msg 40), dann Speichern (msg 45), U-05");
        return k_rechner_5011;
    }
    if (basis(room_id, RE15_U42_RAUM_4020) && event_id == RE15_U42_PLAN_EREIGNIS) {
        if (!rdt_bytes(RE15_U42_PLAN_SUB_OFF, k_sub11_4020, 14)) { g_re15_u42_zaehler[4]++; return NULL; }
        if (!re15_game_flag_get(RE15_R42_BANK, RE15_R42_BIT_PLAN_FORSCHUNG)) g_re15_u42_zaehler[3]++;
        U42_LOG("ROOM%04X Ereignis 11: Wandplan - Blatt %d (Bit %d = %d), U-11", (unsigned)room_id, RE15_U42_PLAN_BLATT,
                RE15_R42_BIT_PLAN_FORSCHUNG, re15_game_flag_get(RE15_R42_BANK, RE15_R42_BIT_PLAN_FORSCHUNG));
        return k_plan;
    }
    return NULL;
}

const uint8_t *re15_ungenutzt_r42_evt_exec(uint16_t room_id, uint8_t sub, const uint8_t *pc)
{
    if (room_id != RE15_U42_RAUM_3091) return pc;
    if (sub == RE15_U42_3091_SUB_SZENE) {
        /* MESSHAKEN RE15_U42_ORIGINAL (env, KEIN Spielverhalten): die ausgelieferte Mod-Szene unveraendert laufen lassen - fuer
         * integration_r41_stage3szenen_b_3091, das die Clipfolge der Mod-Szene gegen den DuckStation-GDB-Lauf prueft (die neue
         * Fassung pruefen unit_r42_ungenutzt_szene/_bytes). */
        if (getenv("RE15_U42_ORIGINAL")) { U42_LOG("ROOM3091 sub07: Messhaken RE15_U42_ORIGINAL -> Mod-Szene"); return pc; }
        if (!szene_bauen()) { g_re15_u42_zaehler[4]++; U42_LOG("ROOM3091 sub07: Pruefbytes anders - Original"); return pc; }
        g_re15_u42_zaehler[1]++;
        U42_LOG("ROOM3091 sub07: Pochen an der Lobby-Tuer (%d,%d,%d), Blick zur Lobby-Tuer (%d,%d), Zeilen %d (John), %d (Elza), "
                "%d (John), Programm %d B - U-01/Stufe 7", RE15_U42_POCH_X, RE15_U42_POCH_Y, RE15_U42_POCH_Z, RE15_U42_LOBBY_X,
                RE15_U42_LOBBY_Z, RE15_U42_MSG_JOHN, RE15_U42_MSG_ELZA, RE15_U42_MSG_JOHN2, s_szene_n);
        return s_szene;
    }
    if (sub == RE15_U42_3091_SUB_SCHALTER) {
        if (!rdt_bytes(RE15_U42_SUB04_OFF, k_sub04_3091, (int)sizeof k_sub04_3091)) { g_re15_u42_zaehler[4]++; return pc; }
        if (!re15_game_flag_get(4, 8)) g_re15_u42_zaehler[2]++;
        return k_schalter;
    }
    return pc;
}

void re15_ungenutzt_r42_tick(void)
{
    if (g_current_room_id != RE15_U42_RAUM_3091) return;
    const int zeile_offen = g_scd.message_active != 0;
    const int stimme = g_re15_voice_laeuft && g_re15_voice_restbilder > 0;
    if (!zeile_offen && !stimme) re15_game_flag_set(5, RE15_U42_SIG_FREI, 1);
}

int re15_ungenutzt_r42_blatt(unsigned page)
{
    return page == RE15_U42_PLAN_BLATT && re15_game_flag_get(RE15_R42_BANK, RE15_R42_BIT_PLAN_FORSCHUNG);
}

const uint8_t *re15_ungenutzt_r42_programm(uint16_t room_id, int welches, int *out_len)
{
    const uint8_t *p = NULL; int n = 0;
    if (room_id == RE15_U42_RAUM_5010) { p = k_rechner_5010; n = (int)sizeof k_rechner_5010; }
    else if (room_id == RE15_U42_RAUM_5011) { p = k_rechner_5011; n = (int)sizeof k_rechner_5011; }
    else if (basis(room_id, RE15_U42_RAUM_4020)) { p = k_plan; n = (int)sizeof k_plan; }
    else if (room_id == RE15_U42_RAUM_3091 && welches == 0) { if (szene_bauen()) { p = s_szene; n = s_szene_n; } }
    else if (room_id == RE15_U42_RAUM_3091 && welches == 1) { p = k_schalter; n = (int)sizeof k_schalter; }
    if (out_len) *out_len = n;
    return p;
}
