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
#include "re15_inventory.h"        /* g_inv / re15_item_is_weapon / re15_inv_equipped_slot (Waffe heben) */
#include "re15_messer.h"           /* RE15_MESSER_ID / RE15_MESSER_NICHTS */
#include "re15_inventar_r36.h"     /* RE15_R36_GRANATE_ERSTE/_LETZTE */
#include "re15_audio.h"            /* re15_audio_prime_weapon */
#include "re15_damage.h"           /* re15_player_equipped_weapon / _set_equipped_weapon (aca5d) */

#include "gen/dining_r39_programme.inc"

void (*re15_dining_r39_se_hook)(int satz) = NULL;
unsigned g_re15_r39j_se_zaehler[3] = { 0, 0, 0 };
int      g_re15_r39j_hilfe_letzte  = 0;

/* ============================ Programme und Texte ============================ */
const uint8_t *re15_dining_r39_haemmern(void)   { return k_r39j_p_haemmern; }
const uint8_t *re15_dining_r39_entriegeln(void) { return k_r39j_p_entriegeln; }
const uint8_t *re15_dining_r39_rolltor(void)    { return k_r39j_p_rolltor; }

const uint8_t *re15_dining_r39_programm(int nr, int *out_len)
{
    static const struct { const uint8_t *b; int n; } p[3] = {
        { k_r39j_p_haemmern,   (int)sizeof k_r39j_p_haemmern   },
        { k_r39j_p_entriegeln, (int)sizeof k_r39j_p_entriegeln },
        { k_r39j_p_rolltor,    (int)sizeof k_r39j_p_rolltor    },
    };
    if (nr < 0 || nr >= 3) { if (out_len) *out_len = 0; return NULL; }
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

/* ============================ Spieler-Plc_motion(2,c) = W-Bank-Clip (Nachbesserung 1 N4.2) ============================
 * Original: der Handler @0x80041b90 schreibt +0x5 = pc[1] (@0x80041bc4); der Spieler-Executor 0x80030660 liest +0x5
 * (`lbu v0,-13735(v0)` 0x800aca59 @0x8003068c) und springt ueber die Tabelle 0x80073e30 (@0x8003069c) - Eintrag [2] =
 * 0x80050f00, dieselbe Spiel-einmal-und-halten-FSM wie bei den NPCs (re15_npc_sub_motion) auf dem Kanal +0x170/+0x174;
 * fuer den Spieler ist das die W-Bank (Waffen-Lader @0x80036be4 `sw v0,-13368(at)` = 0x800acbc8, @0x80036c04
 * `sw v0,-13372(at)` = 0x800acbc4). Der Port spielt fuer pc[1] != 0 bisher PL00 (scd_vm.c anim_use_pl00); in ROOM1141
 * spielt er den Clip jetzt aus der W-Bank (Zeichner: main.c-Haken wie der Modus-6-Override) und haelt ihn auf dem letzten
 * Bild (Phase 2). Rueckwaerts (Plc_flg 0x80) setzt der Zeichner ueber re15_actor_anim_reverse um. Gilt nur in 1141
 * (ROOM3060/3061 tragen dieselbe Form, sind aber nicht Teil dieser Spur - Dossier OFFEN). */
static int s_w_clip = -1;

void re15_dining_r39_w_beginn(int entity, int clip)
{
    if ((g_current_room_id & 0xFFFFu) != RE15_R38F_RAUM_1141 || entity != RE15_R39J_W_SUB || clip < 0) { s_w_clip = -1; return; }
    s_w_clip = clip;
#ifdef RE15_PLATFORM_PC
    fprintf(stderr, "[dining39] F%u Elza Plc_motion(2,%d) -> W-Bank-Clip %d (Sub 0x80073e30[2] = 0x80050f00, Waffe %d)\n",
            (unsigned)g_engine.frame_count, clip, clip, re15_player_equipped_weapon());
#endif
}

void re15_dining_r39_w_tick(void)
{
    if (s_w_clip < 0) return;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    /* Ein anderer Plc-Befehl hat Kommandowort oder Clip ersetzt (Plc_motion @0x80041ba8/@0x80041bb0, Plc_dest @0x80041c14):
     * die W-Bank-Phase ist vorbei. */
    if ((g_current_room_id & 0xFFFFu) != RE15_R38F_RAUM_1141 || pl->state != 4 || (int)pl->motion != s_w_clip) {
        s_w_clip = -1;
        return;
    }
    extern int re15_player_wbank_clip_fc(int clip);   /* player_common.c: W-Bank-EDD-Laengen */
    const int fc = re15_player_wbank_clip_fc(s_w_clip);
    if (fc >= 1 && (int)pl->anim_frame > fc - 1) pl->anim_frame = (uint16_t)(fc - 1);   /* halten: Phase 2 (@0x80050fec) */
}

int re15_dining_r39_w_clip(void) { return s_w_clip; }

/* NUTZER-VORGABE Nachtrag 4 (woertlich): "Oder falls Elza keine Waffe ausgerüstet hat eben das Messer...." / "Ich meine mit
 * ausgerüstet - keine Waffe im Inventar hat" / "Falls sie irgendeine Waffe im Inventar hat, hebt sie halt diese."
 * Schusswaffe = Waffen-Item (re15_item_is_weapon, Id < 0x15) der Gun-Klasse (Id >= 3, Klassenweiche @0x80074030) ohne die
 * Wurf-Granaten 0x09..0x0B (inventar_r36). Keine ausgeruestet -> die erste im Inventar wird ausgeruestet (Setzer wie der
 * Statusschirm-Commit @0x80046688, re15_damage.c); keine im Inventar -> Messer = Id 1 bei Platz 0x80 (Rueckfall
 * `ori v0,zero,0x1` @0x8004666c, re15_messer.h). Die Waffe bleibt danach ausgeruestet (der Kampf folgt). */
static int ist_schusswaffe(int id)
{
    return id >= 3 && re15_item_is_weapon((uint8_t)id) && !(id >= RE15_R36_GRANATE_ERSTE && id <= RE15_R36_GRANATE_LETZTE);
}

int re15_dining_r39_waffe_waehlen(void)
{
    const int alt = re15_player_equipped_weapon();
    int neu = -1;
    if (ist_schusswaffe(alt) && re15_inv_find_item((uint8_t)alt) >= 0) neu = alt;
    for (int i = 0; neu < 0 && i < RE15_INV_MAX_SLOTS; i++)
        if (g_inv.slots[i].id && ist_schusswaffe(g_inv.slots[i].id)) neu = g_inv.slots[i].id;
    if (neu >= 0) {
        if (neu != alt) { re15_player_set_equipped_weapon(neu); re15_audio_prime_weapon(neu); }
    } else if (alt != RE15_MESSER_ID) {
        re15_player_set_equipped_weapon(RE15_MESSER_ID);
        re15_inv_set_equipped_slot(RE15_MESSER_NICHTS);
        re15_audio_prime_weapon(RE15_MESSER_ID);
        neu = RE15_MESSER_ID;
    } else {
        neu = RE15_MESSER_ID;
    }
#ifdef RE15_PLATFORM_PC
    fprintf(stderr, "[dining39] F%u Dachtuer entriegelt: Elza hebt Waffe %d (vorher %d, %s)\n", (unsigned)g_engine.frame_count,
            neu, alt, ist_schusswaffe(neu) ? "Schusswaffe" : "Messer");
#endif
    return neu;
}

/* ============================ Raumaufbau ============================
 * Ende von re15_dining_r38_install: die drei Port-Texte; in Phase H steht John NICHT im Dachgang (Runde 38), sondern ist
 * auf dem Dach = geparkt (Punkt 2: "wenn John durch die Tür kommt, statt durch das Tor"). ENTRIEGELN39 setzt ihn erst
 * hinter der Kamera von Cut 7 innen vor die Dachtuer (Pos_set), waehrend der Tuer-Ton laeuft. */
void re15_dining_r39_install(uint16_t room_id, int phase_h)
{
    s_w_clip = -1;
    if (room_id != RE15_R38F_RAUM_1141) return;
    re15_game_flag_set(RE15_R39J_MERKER_BANK, RE15_R39J_MERKER_BIT, 0);   /* kein Gemaelde-Programm beim Eintritt */
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
