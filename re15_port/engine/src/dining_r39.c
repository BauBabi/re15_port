/*
 * RE1.5 Rebuilt — Runde 39 Spur J "dining1141": Phase H von ROOM1141 nach den Nutzer-Punkten 1-8 (Punkt 9 = Haken in
 * audio_pc.c). Herleitung und Konstanten: include/re15_dining_r39.h, Programme mit Listing:
 * engine/src/gen/dining_r39_programme.inc (tools/r39_dining/programme_bauen.py), Dossier
 * analysis/befunde_runde39/J_dining1141.md. Baut auf Runde 38 Spur F auf (dining_r38.c ruft die Haken unten).
 */
#include "re15_dining_r39.h"
#include "re15_dining_r38.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "re15_scd.h"
#include "re15_msg.h"
#include "re15_room.h"
#include "re15_actor.h"
#include "re15_engine.h"           /* g_engine.frame_count (Messschiene) */

#include "gen/dining_r39_programme.inc"

void (*re15_dining_r39_se_hook)(int satz) = NULL;
unsigned g_re15_r39j_se_zaehler[3] = { 0, 0, 0 };
int      g_re15_r39j_hilfe_letzte  = 0;

/* ============================ Programme und Texte ============================ */
const uint8_t *re15_dining_r39_haemmern(void)  { return k_r39j_p_haemmern; }
const uint8_t *re15_dining_r39_befreiung(void) { return k_r39j_p_befreiung; }

const uint8_t *re15_dining_r39_programm(int nr, int *out_len)
{
    static const struct { const uint8_t *b; int n; } p[2] = {
        { k_r39j_p_haemmern,  (int)sizeof k_r39j_p_haemmern  },
        { k_r39j_p_befreiung, (int)sizeof k_r39j_p_befreiung },
    };
    if (nr < 0 || nr >= 2) { if (out_len) *out_len = 0; return NULL; }
    if (out_len) *out_len = p[nr].n;
    return p[nr].b;
}

const uint8_t *re15_dining_r39_text(int nr, int *out_len)
{
    static const struct { const uint8_t *b; int n; } t[3] = {
        { k_r39j_msg50, (int)sizeof k_r39j_msg50 },
        { k_r39j_msg51, (int)sizeof k_r39j_msg51 },
        { k_r39j_msg52, (int)sizeof k_r39j_msg52 },
    };
    if (nr < 0 || nr >= 3) { if (out_len) *out_len = 0; return NULL; }
    if (out_len) *out_len = t[nr].n;
    return t[nr].b;
}

const uint8_t *re15_dining_r39_klopfton(int *out_len)
{
    if (out_len) *out_len = (int)sizeof k_r39j_klopfton;
    return k_r39j_klopfton;
}

static void meldung(int id, const uint8_t *b, size_t n)
{
    re15_msg_install_text((unsigned char)id, b, n);
    int d = re15_msg_compute_duration(b, n, 0);
    if (d > 0 && d < 65535) re15_msg_install_durations((unsigned char)id, d);
}

/* ============================ Raumaufbau ============================
 * Ende von re15_dining_r38_install: die drei Port-Texte; in Phase H steht John NICHT im Dachgang (Runde 38), sondern ist
 * auf dem Dach = geparkt (Punkt 2: "wenn John durch die Tür kommt, statt durch das Tor"). BEFREIUNG39 setzt ihn erst
 * hinter der Kamera von Cut 7 innen vor die Dachtuer (Pos_set), waehrend der Tuer-Ton laeuft. */
void re15_dining_r39_install(uint16_t room_id, int phase_h)
{
    if (room_id != RE15_R38F_RAUM_1141) return;
    meldung(RE15_R39J_MSG_REAKTION, k_r39j_msg50, sizeof k_r39j_msg50);
    meldung(RE15_R39J_MSG_PUH,      k_r39j_msg51, sizeof k_r39j_msg51);
    meldung(RE15_R39J_MSG_FOLGEN,   k_r39j_msg52, sizeof k_r39j_msg52);
    if (!phase_h) return;
    re15_actor_t *j = &g_actors[RE15_R38F_JOHN_SCRIPT_SLOT + 1];   /* SCRIPT_SLOT_TO_ACTOR = +1 (scd_vm.c) */
    if (j->active && j->type == RE15_R38F_JOHN_TYP) {
        j->x = RE15_R39J_PARK_X;
        j->y = 0;
        j->z = RE15_R39J_PARK_Z;
    }
#ifdef RE15_PLATFORM_PC
    fprintf(stderr, "[dining39] F%u ROOM1141 Phase H: John auf dem Dach (geparkt %d,%d), Klopfen + help01..05 statt "
                    "Zombie-Stoehnen/\"Help me out.\"\n", (unsigned)g_engine.frame_count, RE15_R39J_PARK_X, RE15_R39J_PARK_Z);
#endif
}

/* ============================ Ton ============================ */
int re15_dining_r39_se_on(unsigned bank, int satz)
{
    if (bank != RE15_R39J_SE_BANK) return 0;
    if (satz == RE15_R39J_SATZ_KLOPFEN) g_re15_r39j_se_zaehler[0]++;
    else if (satz == RE15_R39J_SATZ_GLAS) g_re15_r39j_se_zaehler[1]++;
    else if (satz >= RE15_R39J_SATZ_HILFE0 && satz < RE15_R39J_SATZ_HILFE0 + RE15_R39J_HILFE_N) {
        g_re15_r39j_se_zaehler[2]++;
        g_re15_r39j_hilfe_letzte = satz - RE15_R39J_SATZ_HILFE0 + 1;
    }
    if (re15_dining_r39_se_hook) re15_dining_r39_se_hook(satz);
    return 1;
}

/* Punkt 6: "Vor Elzas 'What was that?' ... Scheiben zerspringen". Die Szene zeigt msg 5 (John) mit Sprachzeile; der Port
 * oeffnet die naechste Nachricht erst, wenn die Sprachzeile zu Ende ist (op_message_on Stimmen-Riegel). Damit der Glasbruch
 * unmittelbar VOR Elzas Zeile klingt und nicht mitten in Johns Satz, wartet BEFREIUNG39 mit `2b 3a 00 00` (msg 58) auf
 * das Ende der laufenden Sprachzeile: der Riegel in op_message_on parkt, danach verbraucht dieser Haken die Zeile (kein
 * Text). Ohne Sprachzeile laeuft sie sofort durch. */
int re15_dining_r39_message_on(const uint8_t *pc)
{
    return pc && (g_current_room_id & 0xFFFFu) == RE15_R38F_RAUM_1141 && pc[1] == RE15_R39J_MSG_STIMME_WARTEN;
}

/* ROOM1141 Sprach-Id 53..57 -> synchro/STAGE1/room1141/help01..05.wav (Nutzer-Dateien, unveraendert; Zuordnung im Code,
 * VERTRAG 1.2). Jede andere (Raum, Id) bleibt beim Stamm "mainNN". */
void re15_dining_r39_stamm(unsigned room_id, int voice_id, char *stamm, size_t n)
{
    if ((room_id & 0xFFFFu) != RE15_R38F_RAUM_1141 || !stamm || n < 7) return;
    if (voice_id < RE15_R39J_HILFE_ID0 || voice_id >= RE15_R39J_HILFE_ID0 + RE15_R39J_HILFE_N) return;
    snprintf(stamm, n, "help%02d", voice_id - RE15_R39J_HILFE_ID0 + 1);
}

/* ============================ Messschiene (env-gegatet, kein Spielverhalten) ============================
 * RE15_R39J_LOG=<datei>: je Bild in ROOM1141 eine Zeile mit Kamera (angefordert/angezeigt), Johns Lage/Blick/Zustand,
 * Elzas Lage/Blick, Nachricht und den Flags (3,0x6A) (4,0xF2) (9,94) (9,67) — fuer die Punkte 2/5/7/8. */
void re15_dining_r39_messhaken(void)
{
#ifdef RE15_PLATFORM_PC
    static int s_init = 0;
    static FILE *s_f = NULL;
    if (!s_init) {
        s_init = 1;
        const char *p = getenv("RE15_R39J_LOG");
        if (p && *p) s_f = fopen(p, "w");
    }
    if (!s_f || g_current_room_id != RE15_R38F_RAUM_1141) return;
    const re15_actor_t *j = NULL;
    for (int s = 1; s < RE15_ACTOR_MAX; s++)
        if (g_actors[s].active && g_actors[s].type == RE15_R38F_JOHN_TYP) { j = &g_actors[s]; break; }
    const re15_actor_t *e = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    /* Blick Johns zu Elza: Abweichung in der Konvention von re15_ai_arc_test (re15_damage.c: ausgerichtet, wenn
     * atan2_q12(dz,dx) == rot_y + 1024), auf -2048..2047 gefaltet. */
    int blick = 0;
    if (j) blick = (((int)re15_atan2_q12(e->z - j->z, e->x - j->x) - ((int)j->rot_y + 1024)) % 4096 + 4096 + 2048) % 4096 - 2048;
    fprintf(s_f, "F%u cam=%d anz=%d msg=%d/%d john=%d (%d,%d) rot=%d st=%d s1=%d s2=%d mo=%d elza=(%d,%d) rot=%d "
                 "6a=%d f2=%d b94=%d b67=%d se=%u/%u/%u hilfe=%d blick=%d\n",
            (unsigned)g_engine.frame_count, (int)g_scd.cam_id, (int)g_scd.work_vars[0x0A],
            (int)g_scd.message_active, (int)g_scd.message_id,
            j ? 1 : 0, j ? (int)j->x : 0, j ? (int)j->z : 0, j ? (int)(j->rot_y & 0xFFF) : 0,
            j ? (int)j->state : 0, j ? (int)j->sub_state_1 : 0, j ? (int)j->sub_state_2 : 0, j ? (int)j->motion : 0,
            (int)e->x, (int)e->z, (int)(e->rot_y & 0xFFF),
            re15_game_flag_get(RE15_R38F_6A_BANK, RE15_R38F_6A_BIT), re15_game_flag_get(RE15_R38F_F2_BANK, RE15_R38F_F2_BIT),
            re15_game_flag_get(9, RE15_R39J_BIT_REAKTION), re15_game_flag_get(9, 67),
            g_re15_r39j_se_zaehler[0], g_re15_r39j_se_zaehler[1], g_re15_r39j_se_zaehler[2], g_re15_r39j_hilfe_letzte, blick);
    fflush(s_f);
#endif
}
