/*
 * RE1.5 Rebuilt — Elzas Vorspann (Runde 38 Spur E "elzaintro"): Stimme der Montage ROOM1241 und das
 * Erzaehler-Schwarz ROOM1031 Cut 13. Herleitung, Bytes und Messungen: include/re15_elzaintro_r38.h und
 * analysis/befunde_runde38/E_elzaintro.md.
 */
#include "re15_elzaintro_r38.h"

#include <stdio.h>

/* ---------------------------------------------------------------------------------------------------------
 * PUNKT 1 — Stimme. Eine Zeile je Raum, der die Aufnahmen eines anderen Raums spricht (NUTZER-VORGABE
 * "Es ist das gleiche wie bei Leon"; Form = byte-gleiche Nachrichten 0..5, Belege im Header).
 * ------------------------------------------------------------------------------------------------------- */
typedef struct {
    unsigned raum;        /* laufender Raum (Message_on-Ort)                         */
    unsigned quelle;      /* Raum, dessen synchro-Ordner spricht                      */
    int      erste, letzte;
} stimme_t;

static const stimme_t k_stimme[] = {
    /* ROOM1241 sub02 @0x0582..@0x05FE Message_on 0..5 = ROOM1240 sub02 @0x057E..@0x05FA, Texte byte-gleich
     * (ROOM1241 @0x63C.. / ROOM1240 @0x638..). */
    { RE15_ELZAINTRO_RAUM_ELZA, RE15_ELZAINTRO_RAUM_LEON, RE15_ELZAINTRO_MSG_ERSTE, RE15_ELZAINTRO_MSG_LETZTE },
};

unsigned re15_elzaintro_r38_stimme_raum(unsigned room_id, int msg_id)
{
    for (unsigned k = 0; k < sizeof k_stimme / sizeof k_stimme[0]; k++)
        if (k_stimme[k].raum == room_id && msg_id >= k_stimme[k].erste && msg_id <= k_stimme[k].letzte)
            return k_stimme[k].quelle;
    return room_id;
}

int re15_elzaintro_r38_stimme_pfad(unsigned room_id, int msg_id, const char *stamm, char *out, size_t n)
{
    unsigned q = re15_elzaintro_r38_stimme_raum(room_id, msg_id);
    if (q == room_id || !stamm || !out || n == 0) return 0;
    /* dieselbe Regel wie audio_pc.c: STAGE = Raum >> 12, Raum 4 Stellen Gross-Hex (synchro/README.md) */
    int w = snprintf(out, n, "synchro/STAGE%u/room%04X/%s.wav", q >> 12, q, stamm);
    return (w > 0 && (size_t)w < n) ? 1 : 0;
}

/* ---------------------------------------------------------------------------------------------------------
 * PUNKT 2 — Erzaehler-Schnitte des Vorspanns: Bild = schwarzes Standbild + Text, sonst nichts.
 * ------------------------------------------------------------------------------------------------------- */
typedef struct {
    unsigned raum;
    int      cut;
    unsigned scd_off;     /* Datei-Offset des einzigen Cut_chg auf diesen Schnitt */
} erzaehler_t;

static const erzaehler_t k_erzaehler[] = {
    /* Leon: ROOM1170 sub11 @0x016A6 `29 07` (einziges Cut_chg(7); RVD-Ziele 0..3/8..11). Bis Runde 37 stand
     * diese Bedingung woertlich in main.c (`g_current_room_id == 0x1170 && active_cut_idx == 7`). */
    { 0x1170u,  7, 0x016A6u },
    /* Elza: ROOM1031 sub15 @0x02A74 `29 0d` (einziges Cut_chg(13); RVD-Ziele 0..9). Gleiche Kamera
     * (@0x200 = ROOM1170 @0x140), gleiches Standbild (BG13 = ROOM1170 BG07), gleiche Zone (@0x4F8 = @0x370). */
    { 0x1031u, 13, 0x02A74u },
};

int re15_elzaintro_r38_erzaehler_schwarz(unsigned room_id, int cut)
{
    for (unsigned k = 0; k < sizeof k_erzaehler / sizeof k_erzaehler[0]; k++)
        if (k_erzaehler[k].raum == room_id && k_erzaehler[k].cut == cut) return 1;
    return 0;
}

int re15_elzaintro_r38_erzaehler_anzahl(void)
{
    return (int)(sizeof k_erzaehler / sizeof k_erzaehler[0]);
}

int re15_elzaintro_r38_erzaehler_zeile(int i, unsigned *room_id, int *cut, unsigned *scd_off)
{
    if (i < 0 || i >= re15_elzaintro_r38_erzaehler_anzahl()) return 0;
    if (room_id) *room_id = k_erzaehler[i].raum;
    if (cut)     *cut     = k_erzaehler[i].cut;
    if (scd_off) *scd_off = k_erzaehler[i].scd_off;
    return 1;
}
