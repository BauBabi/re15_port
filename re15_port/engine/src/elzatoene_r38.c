/*
 * elzatoene_r38.c — Runde 38 Spur H "elzatoene": Schuss-Ton statt Platzhaltertext in ROOM1041 (P1) und Roys
 * Zombie-Stoehnen beim Zugehen in ROOM1021 (P2). P3 (Johns Schuesse) laeuft ueber die Anker der Runde 36
 * (engine/src/elza_r36.c) mit dem korrigierten Schuss-Satz der Mini-Bank ELZASE.VBS.
 * Herleitung, Belege und Port-Wahlen: include/re15_elzatoene_r38.h und analysis/befunde_runde38/H_elzatoene.md.
 */
#include "re15_elzatoene_r38.h"
#include "re15_elza_r36.h"          /* re15_elza_se_hook, RE15_ELZA_TON_SCHUSS (Mini-Bank ELZASE.VBS) */
#include "re15_scd.h"
#include "re15_engine.h"            /* g_engine.frame_count — nur fuer das Datei-Log */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern unsigned g_current_room_id;
extern void re15_audio_room_se(int se_id);   /* FUN_800453d0-Pfad (snd1-Raumbank) */

int g_re15_elzatoene_anker_n = 0;

/* ---- Messschiene (die GUI-exe hat kein stderr): dieselbe Datei wie elza_r36.c, RE15_ELZA_LOG=<pfad> ---------- */
static void logf_(const char *fmt, unsigned a, unsigned b, unsigned c)
{
    static int s_init = 0; static const char *s_pfad = NULL;
    if (!s_init) { s_init = 1; s_pfad = getenv("RE15_ELZA_LOG"); if (s_pfad && !*s_pfad) s_pfad = NULL; }
    if (!s_pfad) return;
    FILE *f = fopen(s_pfad, "a");
    if (!f) return;
    fprintf(f, "F%u ", (unsigned)g_engine.frame_count);
    fprintf(f, fmt, a, b, c);
    fputc('\n', f);
    fclose(f);
}

typedef struct {
    uint16_t raum;
    uint32_t off;          /* RDT-Datei-Offset des Anker-Opcodes                 */
    uint8_t  sig[8];       /* erwartete Bytes ab off (Anker-Opcode + Folge-Opcode) */
    uint8_t  aktion;
} anker_t;

static const anker_t k_anker[] = {
    /* P1 ROOM1041 sub11 @0x02200 `2b 03 00 00` Message_on 3 ("*gunshot*"), Folge @0x02204 `2e 01 00 00` Work_set(1,0). */
    { RE15_ELZATOENE_RAUM_1041, RE15_ELZATOENE_1041_OFF, { 0x2b,0x03,0x00,0x00, 0x2e,0x01,0x00,0x00 },
      RE15_ELZATOENE_AKT_SCHUSS_1041 },
    /* P2 ROOM1021 sub02 @0x020B2 `40 00 04 22 4e ef 42 e6` Plc_dest Roy -> (-4274,-6590) (8 Byte = der ganze Opcode). */
    { RE15_ELZATOENE_RAUM_1021, RE15_ELZATOENE_1021_OFF, { 0x40,0x00,0x04,0x22, 0x4e,0xef,0x42,0xe6 },
      RE15_ELZATOENE_AKT_ROY_STOEHNEN },
};
#define ANKER_N ((int)(sizeof k_anker / sizeof k_anker[0]))

static const unsigned char *s_anker_pc[ANKER_N];
static int                  s_anker_idx[ANKER_N];
static int                  s_zaehler[RE15_ELZATOENE_AKT_N];
static int                  s_takt[RE15_ELZATOENE_AKT_N];

int re15_elzatoene_r38_zaehler(int aktion)
{
    return (aktion > 0 && aktion < RE15_ELZATOENE_AKT_N) ? s_zaehler[aktion] : 0;
}
int re15_elzatoene_r38_letzter_takt(int aktion)
{
    return (aktion > 0 && aktion < RE15_ELZATOENE_AKT_N) ? s_takt[aktion] : -1;
}
unsigned re15_elzatoene_r38_anker_off(int i)
{
    return (i >= 0 && i < g_re15_elzatoene_anker_n) ? k_anker[s_anker_idx[i]].off : 0u;
}
void re15_elzatoene_r38_reset(void)
{
    memset(s_zaehler, 0, sizeof s_zaehler);
    for (int i = 0; i < RE15_ELZATOENE_AKT_N; i++) s_takt[i] = -1;
}

void re15_elzatoene_r38_raum_scan(const unsigned char *raw, int raw_size, unsigned room_id)
{
    g_re15_elzatoene_anker_n = 0;
    if (!raw || raw_size <= 0) return;
    for (int i = 0; i < ANKER_N; i++) {
        const anker_t *a = &k_anker[i];
        if (a->raum != (uint16_t)room_id) continue;
        if ((long)a->off + 8 > (long)raw_size) continue;
        if (memcmp(raw + a->off, a->sig, 8) != 0) {
            logf_("[elzatoene] ROOM%04X Anker @0x%05X: Signatur weicht ab - nicht armiert %u", room_id, a->off, 0);
            continue;                               /* falscher Stand/Patch: lieber stumm als falsch */
        }
        s_anker_pc[g_re15_elzatoene_anker_n]  = raw + a->off;
        s_anker_idx[g_re15_elzatoene_anker_n] = i;
        g_re15_elzatoene_anker_n++;
    }
    if (g_re15_elzatoene_anker_n)
        logf_("[elzatoene] ROOM%04X: %u Anker armiert %u", room_id, (unsigned)g_re15_elzatoene_anker_n, 0);
}

static void ausfuehren(int aktion, unsigned off)
{
    switch (aktion) {
    case RE15_ELZATOENE_AKT_SCHUSS_1041:
        /* Derselbe RE2-Skript-Pistolenschuss wie Johns Schuesse (ELZASE.VBS Satz RE15_ELZA_TON_SCHUSS =
         * RE2 ROOM4040 Satz 0x0A), ueber den Plattform-Haken der Runde 36. */
        if (re15_elza_se_hook) re15_elza_se_hook(RE15_ELZA_TON_SCHUSS);
        break;
    case RE15_ELZATOENE_AKT_ROY_STOEHNEN:
        re15_audio_room_se(RE15_ELZATOENE_ROY_STOEHN_SE);   /* @0x80102298 `ori a0,zero,0x4` + @0x8010229c jal 0x800453d0 */
        break;
    default: return;
    }
    s_zaehler[aktion]++;
    s_takt[aktion] = (int)g_scd.tick_count;
    logf_("[elzatoene] ROOM%04X Anker @0x%05X -> Aktion %u", (unsigned)g_current_room_id, off, (unsigned)aktion);
}

void re15_elzatoene_r38_pc(const unsigned char *pc)
{
    for (int k = 0; k < g_re15_elzatoene_anker_n; k++) {
        if (s_anker_pc[k] != pc) continue;
        const anker_t *a = &k_anker[s_anker_idx[k]];
        if (a->aktion == RE15_ELZATOENE_AKT_SCHUSS_1041) return;   /* der Text-Anker gehoert op_message_on */
        ausfuehren(a->aktion, a->off);
        return;
    }
}

int re15_elzatoene_r38_message_on(const unsigned char *pc)
{
    for (int k = 0; k < g_re15_elzatoene_anker_n; k++) {
        if (s_anker_pc[k] != pc) continue;
        const anker_t *a = &k_anker[s_anker_idx[k]];
        if (a->aktion != RE15_ELZATOENE_AKT_SCHUSS_1041) return 0;
        ausfuehren(a->aktion, a->off);
        return 1;                                   /* Text entfaellt: der Aufrufer setzt pc += 4 und laeuft weiter */
    }
    return 0;
}
