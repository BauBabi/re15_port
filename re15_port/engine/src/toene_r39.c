/*
 * toene_r39.c — Runde 39 Spur B "toene": Schubladen-Ton ROOM11F0/11F1 (P1), Lueftungsgitter (P2) und Schrank (P3) in
 * der Raetsel-Szene ROOM1190/1191. Herleitung, Adressen und Port-Wahlen: include/re15_toene_r39.h und
 * analysis/befunde_runde39/B_toene.md. Muster: engine/src/elzatoene_r38.c (Anker = Datei-Offset + 8-Byte-Signatur).
 */
#include "re15_toene_r39.h"
#include "re15_scd.h"
#include "re15_engine.h"            /* g_engine.frame_count — nur fuer die Logzeile */
#include <string.h>
#ifdef RE15_PLATFORM_PC
#include <stdio.h>
#endif

extern unsigned g_current_room_id;

void (*re15_toene39_se_hook)(int ton) = NULL;
int g_re15_toene39_anker_n = 0;

typedef struct {
    uint16_t raum;
    uint32_t off;          /* RDT-Datei-Offset des Anker-Opcodes                     */
    uint8_t  sig[8];       /* erwartete Bytes ab off (Anker-Opcode + Folge-Opcodes)   */
    uint8_t  ton;          /* RE15_TOENE39_TON_*                                      */
} anker_t;

/* Signaturen = die ausgelieferten Bytes (scd_dump_room.py, Dossier §2). ROOM11F1 == ROOM11F0 (cmp), ROOM1191 traegt
 * sub10/sub11 bytegleich zu ROOM1190 (Abweichungen nur @0x2D22.. in sub14). */
static const anker_t k_anker[] = {
    /* P1 sub19 @0x017B6 `29 0b` Cut_chg 11, Folge @0x017B8 `22 02 07 01` Set(2,7), @0x017BC `2e 01` Work_set(1,0). */
    { 0x11F0, RE15_TOENE39_SCHUBLADE_OFF, { 0x29,0x0b, 0x22,0x02,0x07,0x01, 0x2e,0x01 }, RE15_TOENE39_TON_SCHUBLADE },
    { 0x11F1, RE15_TOENE39_SCHUBLADE_OFF, { 0x29,0x0b, 0x22,0x02,0x07,0x01, 0x2e,0x01 }, RE15_TOENE39_TON_SCHUBLADE },
    /* P3 sub10 @0x02796 `2e 03 05 00` Work_set(3,5) + Nop, @0x0279A `2f 00 9c ff` Speed_set(0,-100). */
    { 0x1190, RE15_TOENE39_SCHRANK_OFF,   { 0x2e,0x03,0x05,0x00, 0x2f,0x00,0x9c,0xff }, RE15_TOENE39_TON_REGAL },
    { 0x1191, RE15_TOENE39_SCHRANK_OFF,   { 0x2e,0x03,0x05,0x00, 0x2f,0x00,0x9c,0xff }, RE15_TOENE39_TON_REGAL },
    /* P2 sub11 @0x0286A `0d 00 06 00 0a 00` For 10, Rumpf @0x02870 `30` Add_speed / @0x02871 `31` Add_aspeed. */
    { 0x1190, RE15_TOENE39_LUKE_AUF_OFF,  { 0x0d,0x00,0x06,0x00,0x0a,0x00, 0x30,0x31 }, RE15_TOENE39_TON_GITTER_AUF },
    { 0x1191, RE15_TOENE39_LUKE_AUF_OFF,  { 0x0d,0x00,0x06,0x00,0x0a,0x00, 0x30,0x31 }, RE15_TOENE39_TON_GITTER_AUF },
    /* P2 sub11 @0x0288C `34 05 00 00` Member_set(5,0), @0x02890 `01 00` Evt_end, @0x02892 `2b 00` (sub12). */
    { 0x1190, RE15_TOENE39_LUKE_ZU_OFF,   { 0x34,0x05,0x00,0x00, 0x01,0x00, 0x2b,0x00 }, RE15_TOENE39_TON_GITTER_ZU },
    { 0x1191, RE15_TOENE39_LUKE_ZU_OFF,   { 0x34,0x05,0x00,0x00, 0x01,0x00, 0x2b,0x00 }, RE15_TOENE39_TON_GITTER_ZU },
};
#define ANKER_N ((int)(sizeof k_anker / sizeof k_anker[0]))

static const unsigned char *s_anker_pc[ANKER_N];
static int                  s_anker_idx[ANKER_N];
static int                  s_zaehler[RE15_TOENE39_TON_N];
static int                  s_takt[RE15_TOENE39_TON_N];

int re15_toene_r39_zaehler(int ton)
{
    return (ton >= 0 && ton < RE15_TOENE39_TON_N) ? s_zaehler[ton] : 0;
}
int re15_toene_r39_letzter_takt(int ton)
{
    return (ton >= 0 && ton < RE15_TOENE39_TON_N) ? s_takt[ton] : -1;
}
unsigned re15_toene_r39_anker_off(int i)
{
    return (i >= 0 && i < g_re15_toene39_anker_n) ? k_anker[s_anker_idx[i]].off : 0u;
}
void re15_toene_r39_reset(void)
{
    memset(s_zaehler, 0, sizeof s_zaehler);
    for (int i = 0; i < RE15_TOENE39_TON_N; i++) s_takt[i] = -1;
}

void re15_toene_r39_raum_scan(const unsigned char *raw, int raw_size, unsigned room_id)
{
    g_re15_toene39_anker_n = 0;
    if (!raw || raw_size <= 0) return;
    for (int i = 0; i < ANKER_N; i++) {
        const anker_t *a = &k_anker[i];
        if (a->raum != (uint16_t)room_id) continue;
        if ((long)a->off + 8 > (long)raw_size) continue;
        if (memcmp(raw + a->off, a->sig, 8) != 0) {
#ifdef RE15_PLATFORM_PC
            fprintf(stderr, "[toene39] ROOM%04X Anker @0x%05X: Signatur weicht ab - nicht armiert\n",
                    room_id, (unsigned)a->off);
#endif
            continue;                               /* anderer Stand/Patch: lieber stumm als falsch */
        }
        s_anker_pc[g_re15_toene39_anker_n]  = raw + a->off;
        s_anker_idx[g_re15_toene39_anker_n] = i;
        g_re15_toene39_anker_n++;
    }
#ifdef RE15_PLATFORM_PC
    if (g_re15_toene39_anker_n)
        fprintf(stderr, "[toene39] ROOM%04X: %d Anker armiert\n", room_id, g_re15_toene39_anker_n);
#endif
}

void re15_toene_r39_pc(const unsigned char *pc)
{
    for (int k = 0; k < g_re15_toene39_anker_n; k++) {
        if (s_anker_pc[k] != pc) continue;
        const anker_t *a = &k_anker[s_anker_idx[k]];
        if (re15_toene39_se_hook) re15_toene39_se_hook(a->ton);
        s_zaehler[a->ton]++;
        s_takt[a->ton] = (int)g_scd.tick_count;
#ifdef RE15_PLATFORM_PC
        fprintf(stderr, "[toene39] F%u ROOM%04X Anker @0x%05X -> Ton %d\n", (unsigned)g_engine.frame_count,
                (unsigned)g_current_room_id, (unsigned)a->off, (int)a->ton);
#endif
        return;
    }
}
