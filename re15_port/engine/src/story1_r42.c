/*
 * RE1.5 Rebuilt — Runde 42 "story" (v0.8.31), Stufe STAGE1: Szene vor der Leiter 11A0/11A1. Belege, Formen und Konstanten:
 * include/re15_story1_r42.h.
 *
 * PORT-STORY nach dem freigegebenen Konzept (3.2 L3, 4.2 E2, 6b; K-07, K-08, F-26). Gebaut wird nur aus Original-Formen, wie
 * story2_r42.c: die Szene ist SCD-Bytecode aus Original-Opcodes, ausgefuehrt von der vorhandenen VM (Sce_em_set, Work_set,
 * Plc_dest, Message_on, Cut_chg/Cut_auto, Plc_ret), die Abblende ist die Blende des Raumwechsels, das Hinaufsteigen der Begleiter
 * die NPC-Kantensonde des Originals (npckante_r38.c). Jede Zeile hat Text UND Dauer (re15_msg_install_text +
 * re15_msg_install_durations): fehlt die Aufnahme synchro/STAGE1/room11A0/mainNN.wav bzw. room11A1, laeuft die Zeile mit der
 * Textdauer wie jede RDT-Zeile ohne Ton (VERTRAG 2.1).
 */
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "re15_story1_r42.h"
#include "re15_story_r42.h"
#include "re15_scd.h"
#include "re15_room.h"
#include "re15_msg.h"
#include "re15_actor.h"
#include "re15_engine.h"
#include "re15_inventory.h"
#include "re15_collision.h"
#include "re15_fade.h"
#include "re15_gameflow.h"          /* re15_char_variant: 0 Leon, 1 Elza */
#include "re15_npckante_r38.h"       /* re15_npckante_aktiv: Kletter-Sub des Begleiters */
#include "re15_climb.h"             /* re15_climb_active, RE15_PLAYER_MOTION_CLIMB_UP/_TURN (@0x80038134/@0x80038030) */

#include "gen/story1_r42_texte.inc"

extern unsigned g_current_room_id;
extern int g_re15_voice_laeuft, g_re15_voice_restbilder;

#define S1_LOG(...) do { fprintf(stderr, "[story1] F%u ", (unsigned)g_engine.frame_count); fprintf(stderr, __VA_ARGS__); \
                         fputc('\n', stderr); } while (0)

unsigned g_re15_s1_zaehler[4];

static int basis_ist(uint16_t room, uint16_t b) { return (uint16_t)(room & 0xFFFEu) == b; }
static int elza(void) { return re15_char_variant() != 0; }
static int bit9(int b) { return re15_game_flag_get(RE15_R42_BANK, (uint8_t)b) ? 1 : 0; }

/* ==== Texte ============================================================================================================= */
const uint8_t *re15_story1_r42_text(uint16_t room_id, uint8_t id, int *out_len)
{
    for (int i = 0; i < S1_TEXTE_N; i++)
        if (k_s1_texte[i].raum == room_id && k_s1_texte[i].id == id) {
            if (out_len) *out_len = k_s1_texte[i].n;
            return k_s1_texte[i].b;
        }
    if (out_len) *out_len = 0;
    return NULL;
}

static void texte_einsetzen(uint16_t room_id)
{
    for (int i = 0; i < S1_TEXTE_N; i++) {
        if (k_s1_texte[i].raum != room_id) continue;
        re15_msg_install_text(k_s1_texte[i].id, k_s1_texte[i].b, k_s1_texte[i].n);
        const int d = re15_msg_compute_duration(k_s1_texte[i].b, k_s1_texte[i].n, 0);   /* wie re15_msg_load_room_block */
        if (d > 0 && d < 65535) re15_msg_install_durations(k_s1_texte[i].id, d);
    }
}

/* ==== Opcode-Formen (Satzbreiten = s_opcode_sizes, scd_vm.c; Vorbilder wie story2_r42.c) ===================================
 *   Set `22 bb ii vv`            ROOM11A0 sub04 @0x01304 (Set(1,27,1)) / @0x01308 (Set(2,7,1)) - derselbe Riegel wie 2080
 *   Cut_chg `29 cc`              ROOM11A0 sub04 @0x0130C;  Cut_auto `3c 01` @0x01360
 *   Work_set `2e kk ii` + Nop    ROOM11A0 sub04 @0x01362 (`2e 01 00` `00`)
 *   Plc_ret `42` + Nop           ROOM11A0 sub04 @0x01366;  Riegel los Set(1,27,0)/Set(2,7,0) @0x01368/@0x0136C
 *   Sce_em_set (20 B)            ROOM11A0 sub00 @0x0118C (Marvin), ROOM11A1 @0x0118C (John) / @0x011A0 (Sherry); Band = pc[4]
 *   Plc_dest `40 00 mm bb x z`   Modus 4 gehen ROOM1021 sub02 @0x020B2, 5 laufen ROOM2040 sub02 @0x0165C, 9 drehen @0x01648
 *   Ankunfts-Poll `11 00 08 00 02 00 12 04 21 05 bb 00`   ROOM2040 sub02 @0x01650..@0x01658 (Do {Evt_next} Edwhile Ck(5,bb)=0)
 *   Message_on `2b id 00 00`     ROOM3000 sub03 @0x016B6 (Maske 0 = Untertitel)
 *   Sleep `09 0a nn nn`          ROOM2080 sub02 @0x0CBE
 *   Evt_end `01 00`              ROOM11A0 sub04 @0x01370 */
#define LE16(v)            (uint8_t)((uint16_t)(v) & 0xff), (uint8_t)(((uint16_t)(v) >> 8) & 0xff)
#define OP_SET(b,i,v)      0x22, (b), (i), (v)
#define OP_CUT(c)          0x29, (c)
#define OP_CUT_AUTO        0x3c, 0x01
#define OP_WORK(k,i)       0x2e, (k), (i), 0x00
#define OP_PLC_RET         0x42, 0x00
#define OP_DEST(m,bit,x,z) 0x40, 0x00, (m), (bit), LE16(x), LE16(z)
#define OP_WARTE(bit)      0x11, 0x00, 0x08, 0x00, 0x02, 0x00, 0x12, 0x04, 0x21, 0x05, (bit), 0x00
#define OP_SLEEP(n)        0x09, 0x0a, LE16(n)
#define OP_END             0x01, 0x00
#define OP_EM(slot,typ,band,x,z,gier) 0x44, (slot), (typ), RE15_S1_NPC_GRID, (band), 0x00, 0x00, 0xff, \
                           LE16(x), LE16(-(band) * 1800), LE16(z), 0x00, 0x00, LE16(gier), 0x00, 0x00
/* Bewegung mit festgehaltener Ankunft: Haltebit loeschen, dann Plc_dest; der Takt setzt das Haltebit, sobald das Ankunftsbit
 * (Wort 1, nur ein Bild lang) kommt (re15_story1_r42.h "Signale"). Damit duerfen zwei Figuren gleichzeitig laufen. */
#define OP_GEH(m,an,halt,x,z) OP_SET(5, (halt), 0), OP_DEST((m), (an), (x), (z))
/* Eine Zeile: wie story2_r42.c OP_ZEILE (Sleep 2 dort begruendet: die Stimme startet erst im Audio-Takt). */
#define OP_ZEILE(id)       OP_SET(5, RE15_S1_SIG_FREI, 0), 0x2b, (id), 0x00, 0x00, OP_SLEEP(2), OP_WARTE(RE15_S1_SIG_FREI)
#define OP_RIEGEL          OP_SET(1, 27, 1), OP_SET(2, 7, 1)
#define OP_RIEGEL_LOS      OP_SET(1, 27, 0), OP_SET(2, 7, 0)
/* Abgang: der Takt blendet ab, sobald der vorangehende Begleiter an der Kante hinaufzusteigen beginnt (Kletter-Sub 0x12/0x16,
 * npckante_r38.c) - der Aufstieg selbst gelingt in 11A0 nicht (re15_story1_r42.h "Abgang", Lauf s1/l2). */
#define OP_ABBLENDE        OP_SET(5, RE15_S1_SIG_HELL, 0), OP_SET(5, RE15_S1_SIG_ABBLENDE, 1), OP_WARTE(RE15_S1_SIG_HELL)
#define M_GEHEN  0x04      /* WALK: ROOM1021 sub02 @0x020B2 `40 00 04 22` (NPC-Sub 4 0x80051148, Ankunft d < 100)           */
#define M_LAUF   0x05      /* RUN: ROOM2040 sub02 @0x0165C `40 00 05 20` (NPC-Sub 5 0x80051484, Ankunft d < 300 @0x80051764) */
#define M_DREH   0x09      /* drehen: ROOM2040 sub02 @0x01648 `40 00 09 20`                                                  */
#define OP_DESTV(...)      OP_GEH(__VA_ARGS__)    /* Lage-Makros erst nach dem Einsetzen zerlegen */
#define S1_P     RE15_S1_SPIELER_X, RE15_S1_SPIELER_Z
#define S1_N     RE15_S1_NPC_X, RE15_S1_NPC_Z
#define S1_S     RE15_S1_SHERRY_X, RE15_S1_SHERRY_Z
#define S1_L     RE15_S1_LEITER_X, RE15_S1_LEITER_Z
#define HS       RE15_S1_HALT_SPIELER
#define HN       RE15_S1_HALT_NPC
#define HSH      RE15_S1_HALT_SHERRY
#define AS       RE15_S1_AN_SPIELER
#define AN       RE15_S1_AN_NPC
#define ASH      RE15_S1_AN_SHERRY

/* Anfang: Riegel, Kamera auf die Oeffnung (Cut 3), der Ruf von unten (Zeile 3), der Spieler tritt an die Kante (P) und dreht sich
 * nach unten zum Gang (N). */
#define S1_ANFANG \
    OP_RIEGEL, OP_CUT(RE15_S1_CUT_SZENE)
#define S1_SPIELER_AN_DIE_KANTE \
    OP_WORK(1, 0), OP_DESTV(M_GEHEN, AS, HS, S1_P), OP_WARTE(HS), \
    OP_DESTV(M_DREH, AS, HS, S1_N), OP_WARTE(HS)
/* Ende: Abblende (Takt: Begleiter weg, Bit 120), Bits, Kamera und Spieler frei (Form ROOM11A0 sub04 @0x0135E..@0x01370). Bit 118
 * mit: wer hier war, hat Marvin/John nach dem Alligator getroffen - die Szene in 2080/2081 darf danach nicht mehr kommen (Altstand
 * v0.8.30 ohne Bit 118, re15_story2_r42 install_2080). */
#define S1_ENDE \
    OP_ABBLENDE, \
    OP_SET(RE15_R42_BANK, RE15_R42_BIT_2080_SZENE, 1), OP_SET(RE15_R42_BANK, RE15_R42_BIT_LEITER, 1), \
    OP_CUT_AUTO, OP_RIEGEL_LOS, \
    OP_WORK(1, 0), OP_PLC_RET, \
    OP_END

/* Leon (L3): Marvin ruft, kommt im unteren Gang angelaufen, drei Zeilen, geht als Erster zur Kante (Richtung Leiter), Abblende. */
static const uint8_t k_prog_leon[] = {
    S1_ANFANG,
    OP_EM(RE15_S1_NPC_SLOT, RE15_S1_TYP_MARVIN, RE15_S1_BAND_UNTEN, RE15_S1_NPC_START_X, RE15_S1_NPC_START_Z, RE15_S1_GIER_NPC_START),
    OP_SLEEP(1),                                                      /* Abschnittsende (story2_r42.c S2_ANKUNFT) */
    OP_ZEILE(3),                                                      /* "Leon, wait up!" (Marvin noch ausserhalb des Bildes) */
    S1_SPIELER_AN_DIE_KANTE,
    OP_WORK(2, RE15_S1_NPC_SLOT), OP_DESTV(M_LAUF, AN, HN, S1_N), OP_WARTE(HN),
    OP_DESTV(M_DREH, AN, HN, S1_P), OP_WARTE(HN),
    OP_ZEILE(4), OP_ZEILE(5), OP_ZEILE(6),
    OP_WORK(2, RE15_S1_NPC_SLOT), OP_DESTV(M_GEHEN, AN, HN, S1_L),
    S1_ENDE
};

/* Elza (E2): Sherry ruft, John und Sherry kommen zusammen angelaufen, Zeilen 4/5, [Uhr 6..9], John 10, John geht als Erster
 * zur Kante, Sherry "right behind me" an Johns Platz hinter ihm, Abblende. */
#define S1_ELZA_ANKUNFT \
    S1_ANFANG, \
    OP_EM(RE15_S1_NPC_SLOT, RE15_S1_TYP_JOHN, RE15_S1_BAND_UNTEN, RE15_S1_NPC_START_X, RE15_S1_NPC_START_Z, RE15_S1_GIER_NPC_START), \
    OP_EM(RE15_S1_SHERRY_SLOT, RE15_S1_TYP_SHERRY, RE15_S1_BAND_UNTEN, RE15_S1_SHERRY_START_X, RE15_S1_SHERRY_START_Z, \
          RE15_S1_GIER_SHERRY_START), \
    OP_SLEEP(1), \
    OP_ZEILE(3),                                                      /* "Elza! Wait for us!" */ \
    S1_SPIELER_AN_DIE_KANTE, \
    OP_WORK(2, RE15_S1_NPC_SLOT), OP_DESTV(M_LAUF, AN, HN, S1_N), \
    OP_WORK(2, RE15_S1_SHERRY_SLOT), OP_DESTV(M_LAUF, ASH, HSH, S1_S), \
    OP_WARTE(HN), OP_WORK(2, RE15_S1_NPC_SLOT), OP_DESTV(M_DREH, AN, HN, S1_P), \
    OP_WARTE(HSH), OP_WORK(2, RE15_S1_SHERRY_SLOT), OP_DESTV(M_DREH, ASH, HSH, S1_P), \
    OP_WARTE(HN), OP_WARTE(HSH), \
    OP_ZEILE(4), OP_ZEILE(5)
#define S1_ELZA_ABGANG \
    OP_ZEILE(10), \
    OP_WORK(2, RE15_S1_NPC_SLOT), OP_DESTV(M_GEHEN, AN, HN, S1_L), \
    OP_WORK(2, RE15_S1_SHERRY_SLOT), OP_DESTV(M_GEHEN, ASH, HSH, S1_N), \
    S1_ENDE
static const uint8_t k_prog_elza[] = { S1_ELZA_ANKUNFT, S1_ELZA_ABGANG };
/* Mit Uhr (6b, K-08, F-26): Elza wendet sich Sherry zu, Zeile 6, Uebergabe (Takt nimmt die Uhr aus dem Inventar), 7..9, zurueck zu
 * John. */
static const uint8_t k_prog_elza_uhr[] = {
    S1_ELZA_ANKUNFT,
    OP_WORK(1, 0), OP_DESTV(M_DREH, AS, HS, S1_S), OP_WARTE(HS),
    OP_ZEILE(6), OP_SET(5, RE15_S1_SIG_UHR, 1), OP_ZEILE(7), OP_ZEILE(8), OP_ZEILE(9),
    OP_WORK(1, 0), OP_DESTV(M_DREH, AS, HS, S1_N), OP_WARTE(HS),
    S1_ELZA_ABGANG
};

const uint8_t *re15_story1_r42_programm(int f, int *out_len)
{
    const uint8_t *p = f == 0 ? k_prog_leon : f == 1 ? k_prog_elza : k_prog_elza_uhr;
    if (out_len) *out_len = f == 0 ? (int)sizeof k_prog_leon : f == 1 ? (int)sizeof k_prog_elza : (int)sizeof k_prog_elza_uhr;
    return p;
}

static int starten(const uint8_t *p)
{
    for (int slot = SCD_EVENT_SLOT_FIRST; slot <= SCD_EVENT_SLOT_LAST; slot++)
        if (!g_scd.threads[slot].active && scd_thread_start(slot, p) == 0) return slot;
    return -1;
}

/* ==== Zustand ============================================================================================================= */
static int s_laeuft = 0;          /* 1 = Programm laeuft */
static int s_blende = 0;          /* 0 ruhig, 1 aus, 2 halten, 3 ein */
static int s_halten = 0;
static uint8_t s_halt_alt = 0;    /* Ankunftsbits im Vorbild (Messzeile) */
static uint32_t s_start_bild = 0;
int re15_story1_r42_szene_laeuft(void) { return s_laeuft; }

void re15_story1_r42_install(uint16_t room_id)
{
    s_laeuft = 0; s_blende = 0; s_halten = 0; s_halt_alt = 0;
    if (!basis_ist(room_id, RE15_S1_RAUM_11A0)) return;
    texte_einsetzen(room_id);
    if (bit9(RE15_R42_BIT_LEITER))
        S1_LOG("ROOM%04X: Szene an der Leiter schon gesehen (Bit %d = 1)", (unsigned)room_id, RE15_R42_BIT_LEITER);
    else
        S1_LOG("ROOM%04X: Ausloesezone Band %d x %d..%d z %d..%d (SCA @0x0830/@0x0824, Kante @0x07E8, RVD @0x02D4)", (unsigned)room_id,
               RE15_S1_ZONE_BAND, RE15_S1_ZONE_X0, RE15_S1_ZONE_X1, RE15_S1_ZONE_Z0, RE15_S1_ZONE_Z1);
}

static int ist_begleiter(uint8_t t) { return t == RE15_S1_TYP_MARVIN || t == RE15_S1_TYP_JOHN || t == RE15_S1_TYP_SHERRY; }

static int in_zone(void)
{
    const re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    return pl->x >= RE15_S1_ZONE_X0 && pl->x <= RE15_S1_ZONE_X1 && pl->z >= RE15_S1_ZONE_Z0 && pl->z <= RE15_S1_ZONE_Z1 &&
           re15_collision_debug_band() == RE15_S1_ZONE_BAND;
}

static void szene_starten(uint16_t room)
{
    const re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    int f = 0;
    if (elza()) f = re15_inv_find_item(RE15_S1_UHR_ITEM) >= 0 ? 2 : 1;
    int n = 0;
    const uint8_t *p = re15_story1_r42_programm(f, &n);
    const int t = starten(p);
    if (t < 0) { S1_LOG("ROOM%04X: kein freier Faden - Szene wartet", (unsigned)room); return; }
    s_laeuft = 1; s_blende = 0; s_halt_alt = 0; s_start_bild = g_engine.frame_count;
    g_re15_s1_zaehler[0]++;
    S1_LOG("ROOM%04X: Spieler betritt den Abschnitt bei (%d,%d,%d) Band %d -> Szene %s (Faden %d, %d B)", (unsigned)room,
           (int)pl->x, (int)pl->y, (int)pl->z, re15_collision_debug_band(),
           f == 0 ? "L3 (Marvin)" : f == 1 ? "E2 (John, Sherry) ohne Uhr" : "E2 (John, Sherry) mit Uhr (6b)", t, n);
}

static void uhr_uebergeben(uint16_t room)
{
    const int s = re15_inv_find_item(RE15_S1_UHR_ITEM);
    if (s < 0) { S1_LOG("ROOM%04X: Uhr nicht mehr im Inventar", (unsigned)room); return; }
    re15_inv_remove_slot(s);            /* wie ein verbrauchter Gegenstand: Platz leeren @0x8004aef0 ... */
    re15_inv_compact();                 /* ... und zusammenschieben FUN_8004dadc */
    g_re15_s1_zaehler[1]++;
    S1_LOG("ROOM%04X: Taschenuhr (Item 0x%02X, Platz %d) an Sherry uebergeben", (unsigned)room, RE15_S1_UHR_ITEM, s);
}

static void begleiter_weg(uint16_t room)
{
    for (int i = 1; i < RE15_ACTOR_MAX; i++)
        if (g_actors[i].active && ist_begleiter(g_actors[i].type)) {
            S1_LOG("ROOM%04X: NPC 0x%02X Aktor %d bei (%d,%d,%d) Band %d verlaesst den Raum (Abblende)", (unsigned)room,
                   g_actors[i].type, i, (int)g_actors[i].x, (int)g_actors[i].y, (int)g_actors[i].z, (int)g_actors[i].floor);
            re15_actor_free(i);
            g_re15_s1_zaehler[3]++;
        }
}

/* Abgang (re15_story1_r42.h): der vorangehende Begleiter (Marvin/John) hat die Kante erreicht = Kletter-Sub 0x12/0x16 begonnen
 * (Sonde FUN_8002d7d8 trifft die Kletterkante 800 voraus), oder er ist (falls die Sonde nie trifft) an seinem Ziel angekommen. */
static int abgang_an_der_kante(uint16_t room)
{
    for (int i = 1; i < RE15_ACTOR_MAX; i++) {
        const re15_actor_t *e = &g_actors[i];
        if (!e->active || (e->type != RE15_S1_TYP_MARVIN && e->type != RE15_S1_TYP_JOHN)) continue;
        if (re15_npckante_aktiv(e)) {
            S1_LOG("ROOM%04X: NPC 0x%02X an der Kante bei (%d,%d,%d) Sub 0x%02X -> Abblende", (unsigned)room, e->type, (int)e->x,
                   (int)e->y, (int)e->z, e->sub_state_1);
            return 1;
        }
    }
    if (re15_game_flag_get(5, RE15_S1_HALT_NPC)) {
        S1_LOG("ROOM%04X: Begleiter am Ziel ohne Kante -> Abblende", (unsigned)room);
        return 1;
    }
    return 0;
}

/* Abblende (re15_story1_r42.h): aus -> halten (Begleiter weg) -> ein -> Signal HELL. Der Takt laeuft vor dem Zeichnen; im Bild, in
 * dem die Rampe fertig ist (Bit 15 nach dem letzten gezeichneten Bild), setzt er SOFORT das statische Schwarz - kein helles Bild
 * dazwischen (wie die Tuer: Stufe 2 fragt FUN_8002178c ab, bevor gezeichnet wird). */
static void blende_takt(uint16_t room)
{
    switch (s_blende) {
    case 1:
        if (!re15_fade_done(RE15_S1_BLENDE_KANAL)) return;
        re15_fade_config(RE15_S1_BLENDE_KANAL, RE15_S1_BLENDE_ABR, RE15_S1_BLENDE_MASKE, 0, 0);       /* @0x80046620-50 */
        re15_fade_kick(RE15_S1_BLENDE_KANAL, RE15_S1_BLENDE_SCHWARZ);
        begleiter_weg(room);
        s_halten = RE15_S1_BLENDE_HALTEN; s_blende = 2;
        S1_LOG("ROOM%04X: Abblende schwarz, %d Bilder halten", (unsigned)room, RE15_S1_BLENDE_HALTEN);
        return;
    case 2:
        if (s_halten-- > 0) return;
        re15_fade_config(RE15_S1_BLENDE_KANAL, RE15_S1_BLENDE_ABR, RE15_S1_BLENDE_MASKE, (int16_t)RE15_S1_BLENDE_EIN, 0); /* @0x8001cc00 */
        re15_fade_kick(RE15_S1_BLENDE_KANAL, 0);                                                                         /* @0x8001cc18 */
        s_blende = 3;
        return;
    case 3:
        if (!re15_fade_done(RE15_S1_BLENDE_KANAL)) return;
        s_blende = 0;
        re15_game_flag_set(5, RE15_S1_SIG_HELL, 1);
        S1_LOG("ROOM%04X: Einblende fertig", (unsigned)room);
        return;
    default:
        return;
    }
}

void re15_story1_r42_tick(void)
{
    const uint16_t room = (uint16_t)g_current_room_id;
    if (!basis_ist(room, RE15_S1_RAUM_11A0)) return;
    if (!s_laeuft) {
        const int16_t mo = g_actors[RE15_ACTOR_SLOT_PLAYER].motion;
        if (bit9(RE15_R42_BIT_LEITER) || g_scd.message_active || g_room_change.pending || re15_climb_active() ||
            mo == RE15_PLAYER_MOTION_CLIMB_UP || mo == RE15_PLAYER_MOTION_CLIMB_TURN) return;   /* Kletterbewegung zu Ende (Kopf) */
        if (in_zone()) szene_starten(room);
        return;
    }
    /* Zeile/Stimme fertig */
    const int zeile  = g_scd.message_active != 0;
    const int stimme = g_re15_voice_laeuft && g_re15_voice_restbilder > 0;
    if (!zeile && !stimme) re15_game_flag_set(5, RE15_S1_SIG_FREI, 1);
    /* Ankuenfte festhalten (Wort 1 lebt nur ein Bild) + Messzeilen */
    static const char *const wer[3] = { "Spieler", "Begleiter", "Sherry" };
    for (int b = 0; b < 3; b++) {
        const int an = re15_game_flag_get(5, (uint8_t)(RE15_S1_AN_SPIELER + b)) ? 1 : 0;
        if (an) re15_game_flag_set(5, (uint8_t)(RE15_S1_HALT_SPIELER + b), 1);
        if (an && !((s_halt_alt >> b) & 1)) {
            const re15_actor_t *a = NULL;
            if (b == 0) a = &g_actors[RE15_ACTOR_SLOT_PLAYER];
            else for (int i = 1; i < RE15_ACTOR_MAX; i++)
                if (g_actors[i].active && (b == 2 ? g_actors[i].type == RE15_S1_TYP_SHERRY
                                                  : (g_actors[i].type == RE15_S1_TYP_MARVIN || g_actors[i].type == RE15_S1_TYP_JOHN)))
                    { a = &g_actors[i]; break; }
            if (a) S1_LOG("ROOM%04X: Ankunft %s bei (%d,%d,%d) Band %d Gier %d (Bild %u der Szene)", (unsigned)room, wer[b],
                          (int)a->x, (int)a->y, (int)a->z, (int)a->floor, (int)a->rot_y,
                          (unsigned)(g_engine.frame_count - s_start_bild));
        }
        s_halt_alt = (uint8_t)((s_halt_alt & ~(1u << b)) | ((unsigned)an << b));
    }
    if (re15_game_flag_get(5, RE15_S1_SIG_UHR)) { re15_game_flag_set(5, RE15_S1_SIG_UHR, 0); uhr_uebergeben(room); }
    if (re15_game_flag_get(5, RE15_S1_SIG_ABBLENDE) && s_blende == 0 && abgang_an_der_kante(room)) {
        re15_game_flag_set(5, RE15_S1_SIG_ABBLENDE, 0);
        re15_fade_config(RE15_S1_BLENDE_KANAL, RE15_S1_BLENDE_ABR, RE15_S1_BLENDE_MASKE, (int16_t)RE15_S1_BLENDE_AUS, 0);   /* @0x8001ca64 */
        re15_fade_kick(RE15_S1_BLENDE_KANAL, 0);                                                                          /* @0x8001ca88 */
        s_blende = 1;
        g_re15_s1_zaehler[2]++;
        S1_LOG("ROOM%04X: Abblende (Blende des Raumwechsels, Schritt +0x%X)", (unsigned)room, RE15_S1_BLENDE_AUS);
    }
    blende_takt(room);
    if (bit9(RE15_R42_BIT_LEITER) && s_blende == 0 && !g_scd.message_active) {
        s_laeuft = 0;
        S1_LOG("ROOM%04X: Szene %s fertig nach %u Bildern, Bit %d = 1, Begleiter vorausgegangen", (unsigned)room,
               elza() ? "E2" : "L3", (unsigned)(g_engine.frame_count - s_start_bild), RE15_R42_BIT_LEITER);
    }
}
