/*
 * RE1.5 Rebuilt — Runde 39 Spur P "johnruf1041", Nachbesserung 1: Tuersperren fuer Elza (1041 -> 1031 und 1051 -> 10A1 in der
 * Panikphase, der Kanaldeckel in 10B1 immer). Herleitung und alle Konstanten: include/re15_tuersperre_r39.h; Texte, Szenen-
 * Huelle und Anker mit Listing: engine/src/gen/tuersperre_r39.inc (tools/r39_johnruf/tuersperre_bauen.py); Dossier
 * analysis/befunde_runde39/P_johnruf1041.md Abschnitt 8. Muster: parkplatz_r39.c (Spur O), tuer1060_1040.c (Runde 35 L).
 * Nachbesserung 2 (Orchestrator): die Saetze sind einfache Raumtexte ohne Stimme (Nachtrag 13); der Kanaldeckel 10B1 ist
 * zurueckgestellt (k_sperren, Abnahme 1 M1).
 */
#include "re15_tuersperre_r39.h"

#include <stddef.h>
#include <string.h>

#include "re15_scd.h"
#include "re15_aot.h"
#include "re15_msg.h"
#include "re15_room.h"
#include "re15_engine.h"           /* g_engine.frame_count (Messschrieb) */
#include "re15_johnruf_r39.h"      /* Bank 9 Bit 97 = Panikphase */
#ifdef RE15_PLATFORM_PC
#include <stdio.h>
#endif

#include "gen/tuersperre_r39.inc"   /* Texte, Programm SZENE10B1, Anker (tuersperre_bauen.py) */

static unsigned s_zaehler[3];       /* 0 Sperren gesetzt, 1 Szenen-Huellen, 2 frei (Stimme entfiel, Nachtrag 13) */

#ifdef RE15_PLATFORM_PC
#define TS_LOG(...) do { fprintf(stderr, "[tuersperre] F%u ", (unsigned)g_engine.frame_count); \
                         fprintf(stderr, __VA_ARGS__); fputc('\n', stderr); } while (0)
#else
#define TS_LOG(...) do { } while (0)
#endif

/* ============================ Tabelle der Sperren ============================ */
typedef struct {
    uint16_t raum;
    uint8_t  slot;
    uint8_t  msg;
    uint8_t  immer;                 /* 1 = ohne Bedingung (10B1), 0 = nur in der Panikphase (Bit 97) */
    const uint8_t *text;
    size_t   text_len;
    uint32_t anker_off, anker_len, anker_fnv;
} sperre_t;

static const sperre_t k_sperren[] = {
    { RE15_R39P_TS_RAUM_1041, RE15_R39P_TS_SLOT_1041, RE15_R39P_TS_MSG_1041, 0, k_r39p_ts_msg_rettung,
      sizeof k_r39p_ts_msg_rettung,
      RE15_R39P_TS_ANKER_T1041_OFF, RE15_R39P_TS_ANKER_T1041_LEN, RE15_R39P_TS_ANKER_T1041_FNV },
    { RE15_R39P_TS_RAUM_1051, RE15_R39P_TS_SLOT_1051, RE15_R39P_TS_MSG_1051, 0, k_r39p_ts_msg_rettung,
      sizeof k_r39p_ts_msg_rettung,
      RE15_R39P_TS_ANKER_T1051_OFF, RE15_R39P_TS_ANKER_T1051_LEN, RE15_R39P_TS_ANKER_T1051_FNV },
    /* ROOM10B1 Kanaldeckel (Nachtrag 12c, immer = 1) ZURUECKGESTELLT nach Abnahme 1 M1: der Abstieg vom Dach ist einseitig
     * (ROOM10B1 SCA @0x00EE4: Zellen 31 @0x01070 und 18 @0x00FD4 nur "hinab", keine Band-1-Zelle "hinauf"; Aufstieg nur mit
     * Bit 0 @0x8002d544 `andi v0,s0,0x1` / @0x8002d548 `beq v0,zero,0x8002d634`), und Slot 1 ist der einzige Ausgang des
     * unteren Teils -> ein immer geschlossener Deckel sperrt Elza ein. Ohne Eintrag laeuft 10B1 wie ausgeliefert (sub04
     * @0x01A8E `47 01`); die Huelle SZENE10B1 bleibt fuer die Entscheidung des Nutzers im Generator (90e50a2a). */
};
#define N_SPERREN ((int)(sizeof k_sperren / sizeof k_sperren[0]))

static const sperre_t *sperre_fuer(unsigned room_id)
{
    for (int i = 0; i < N_SPERREN; i++)
        if (k_sperren[i].raum == (uint16_t)(room_id & 0xFFFFu)) return &k_sperren[i];
    return NULL;
}

/* ============================ Anker (Quellbereiche im GELADENEN RDT, FNV-1a wie parkplatz_r39.c) ============================ */
static uint32_t fnv(const uint8_t *p, uint32_t n)
{
    uint32_t h = 0x811C9DC5u;
    for (uint32_t i = 0; i < n; i++) { h ^= p[i]; h *= 0x01000193u; }
    return h;
}

static int bereich_ok(uint32_t off, uint32_t len, uint32_t soll)
{
    const uint8_t *raw = g_room_rdt.raw;
    if (!raw || (uint32_t)g_room_rdt.raw_size < off + len) return 0;
    return fnv(raw + off, len) == soll;
}

int re15_tuersperre_r39_anker_ok(unsigned room_id)
{
    const sperre_t *s = sperre_fuer(room_id);
    if (!s || (unsigned)g_current_room_id != (unsigned)s->raum) return 0;
    if (!bereich_ok(s->anker_off, s->anker_len, s->anker_fnv)) return 0;
    if (s->raum == RE15_R39P_TS_RAUM_10B1 &&
        !bereich_ok(RE15_R39P_TS_ANKER_S10B1_OFF, RE15_R39P_TS_ANKER_S10B1_LEN, RE15_R39P_TS_ANKER_S10B1_FNV)) return 0;
    return 1;
}

static int panik(void) { return re15_game_flag_get(RE15_R39P_BIT_BANK, RE15_R39P_BIT_PANIK) != 0; }

int re15_tuersperre_r39_gesperrt(unsigned room_id)
{
    const sperre_t *s = sperre_fuer(room_id);
    if (!s || (unsigned)g_current_room_id != (unsigned)s->raum) return 0;
    const re15_aot_t *a = &g_aot.slots[s->slot];
    return a->active && a->type == RE15_AOT_TYPE_MESSAGE && a->event_id == s->msg;
}

/* Die Sperre setzen, wenn ihre Bedingung gilt und der Slot noch die ausgelieferte Tuer ist. */
static void sperren(const sperre_t *s)
{
    if (!s->immer && !panik()) return;                 /* 1041/1051: nur in der Panikphase */
    const re15_aot_t *a = &g_aot.slots[s->slot];
    if (!a->active || a->type != RE15_AOT_TYPE_DOOR) return;   /* schon Text oder vom Skript geaendert: nichts */
    if (!re15_tuersperre_r39_anker_ok(s->raum)) {
        TS_LOG("ROOM%04X: RDT weicht von den Ankern ab -> keine Sperre (Slot %u bleibt Tuer)", (unsigned)s->raum,
               (unsigned)s->slot);
        return;
    }
    /* = Aot_reset(slot, 1, 0x31, msg, 0xffff, 0): ROOM11B1 sub01 @0x011C0 / ROOM11A1 main00 @0x00FBC (Bytes im Kopf) */
    re15_aot_retype(s->slot, RE15_R39P_TS_SCE_MESSAGE, RE15_R39P_TS_SAT, (uint16_t)s->msg, (uint16_t)RE15_R39P_TS_MASKE, 0);
    s_zaehler[0]++;
    if (s->immer) TS_LOG("ROOM%04X: Kanaldeckel Slot %u -> Text msg %u (immer zu)", (unsigned)s->raum, (unsigned)s->slot,
                         (unsigned)s->msg);
    else          TS_LOG("ROOM%04X: Panikphase (Bit 97) -> Tuer Slot %u -> Text msg %u", (unsigned)s->raum,
                         (unsigned)s->slot, (unsigned)s->msg);
}

/* ============================ Raumaufbau / je Bild ============================ */
void re15_tuersperre_r39_install(uint16_t room_id)
{
    const sperre_t *s = sperre_fuer(room_id);
    if (!s) return;                                    /* Leon (1040/1050/10B0) und alle anderen Raeume: nichts */
    re15_msg_install_text(s->msg, s->text, s->text_len);   /* gilt an jedem Raumstart-Weg; der Teardown loescht Port-Ids */
    {
        int d = re15_msg_compute_duration(s->text, s->text_len, 0);
        if (d > 0 && d < 65535) re15_msg_install_durations(s->msg, d);
    }
    sperren(s);
}

void re15_tuersperre_r39_tick(void)
{
    const sperre_t *s = sperre_fuer((unsigned)g_current_room_id);
    if (s) sperren(s);                                 /* Bit 97 steigt in 1041 (RUF1041) -> im naechsten Bild VOR dem Scan */
}

/* ============================ Evt_exec-Haken (10B1 sub04) ============================ */
const uint8_t *re15_tuersperre_r39_evt_exec(uint16_t room_id, uint8_t sub_id, const uint8_t *orig)
{
    if (room_id != RE15_R39P_TS_RAUM_10B1 || sub_id != RE15_R39P_TS_SUB_10B1 || !orig) return orig;
    if (!re15_tuersperre_r39_gesperrt(RE15_R39P_TS_RAUM_10B1) || !re15_tuersperre_r39_anker_ok(RE15_R39P_TS_RAUM_10B1))
        return orig;                                   /* Deckel nicht gesperrt: Original-Szene mit Raumwechsel */
    s_zaehler[1]++;
    TS_LOG("ROOM%04X sub01 @0x01824 Evt_exec sub%02u -> Programm SZENE10B1 (Szene, Deckel bleibt zu, Steuerung zurueck)",
           (unsigned)RE15_R39P_TS_RAUM_10B1, (unsigned)sub_id);
    return k_r39p_ts_szene10b1;
}

/* ============================ Pruefhaken ============================ */
const uint8_t *re15_tuersperre_r39_text(unsigned room_id, int *out_len)
{
    const sperre_t *s = sperre_fuer(room_id);
    if (out_len) *out_len = s ? (int)s->text_len : 0;
    return s ? s->text : NULL;
}

const uint8_t *re15_tuersperre_r39_szene(int *out_len)
{
    if (out_len) *out_len = (int)sizeof k_r39p_ts_szene10b1;
    return k_r39p_ts_szene10b1;
}

unsigned re15_tuersperre_r39_zaehler(int k) { return (k >= 0 && k < 3) ? s_zaehler[k] : 0u; }
