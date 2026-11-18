/*
 * RE1.5 Rebuilt — Runde 42 Strang finale, Stufe 1 (Leon, ROOM5090). Kopf, Nutzerwortlaut und alle Konstanten:
 * include/re15_finale_r42.h. Adressen @0x8010xxxx = RE2-Tyrant-Overlay CDEMD0_EM2B_ai1.BIN (gegen 0x80100000), @0x800xxxxx =
 * RE2-Leon-EXE info/re2leon/PSX.EXE, @0x0xxxx = Datei-Offset in ROOM7020.RDT (RE2) bzw. ROOM5090.RDT (RE1.5).
 *
 * ⛔ PORT-ERGAENZUNG nach Nutzerentscheid, KEIN Auslieferungscode. Ablauf und Bildzahlen sind die des RE2-Endes; was der Zugwagen
 * anders braucht, ist als PORT-WAHL / PORT-ZUORDNUNG markiert und im Bericht als offen gefuehrt (Plan U3, U4, U5, U6).
 */
#include "re15_finale_r42.h"
#include "re15_story_r42.h"
#include "re15_birkin_formen.h"   /* re15_bf_bahnsteig_aktiv, re15_bb_bank_da */
#include "re15_annette_r41.h"     /* re15_annette_r41_besitzt: die Szene davor */
#include "re15_finale_wurf_r42.h"  /* Stufe 3: Wurfszene (Plan S5) */

#include <string.h>
#include <stdlib.h>
#ifdef RE15_PLATFORM_PC
#include <stdio.h>
#endif

#include "re15_actor.h"
#include "re15_damage.h"          /* re15_enemy_player_dist, re15_engine_rand8, re15_player_equipped_weapon */
#include "re15_inventory.h"       /* re15_ammo_mag_nonzero */
#include "re15_player.h"          /* re15_player_aim_ready, RE15_PAD_BIT_* */
#include "re15_scd.h"             /* g_scd, scd_thread_start, re15_game_flag_*, RE15_PAUSE_* */
#include "re15_msg.h"
#include "re15_engine.h"
#include "re15_math.h"            /* re15_catan */
#include "re15_door_seq.h"        /* re15_door_rotmatrix = RE2 RotMatrix @0x8008e1f4 */
#include "re15_audio.h"           /* re15_audio_re2_xa */
#include "re15_rumble.h"
#include "re15_werfer.h"          /* re15_audio_re2_arms_se */
#include "re2_fx.h"
#include "re2_buehne.h"
#include "re15_teile_r15.h"        /* U5: Teil-Schicht (Bit 0x40 eigene Weltmatrix, 0x08 Scherben) */

#include "gen/finale_r42_texte.inc"   /* k_fi_5090_msg02, k_fi_5091_msg02 (tools/r42_finale/texte_finale.py) */
#include "re15_room.h"                 /* g_current_room_id, g_room_rdt */
#include "re15_camera.h"               /* re15_camera_build_view: Messzeile Bildlage */

int re15_player_gunbone_matrix(int32_t rot[9], int32_t t[3]);   /* re15_damage.c */

#define FI_RAKETE   0x020D1000u   /* Wiederholung 1/2: `lui a0,0x20d / ori a0,a0,0x1000` @0x8010384c-50 / @0x80103950-54 */
#define FI_WIED3    0x270A1000u   /* Wiederholung 3: `lui a0,0x270a / ori 0x1000` @0x80103a54-58 (Bank 0x27 = Raum-ESP R702) */
#define FI_KNALL    0x031F5000u   /* Einschlag + Nachknalle: `lui a0,0x31f / ori a0,a0,0x5000` @0x80103ae0-e4 / @0x80103ce4-e8 */
#define FI_KNALL_Y  (-1800)       /* `addiu v0,zero,-0x708 / sh v0,0x12(sp)` @0x80103b24-2c */
#define FI_NACH_Y   (-2500)       /* `addiu v1,v1,-0x9c4` @0x80103cd8 */
#define FI_NACH_R      20         /* r * 5 * 4: `sll v1,v0,2 / addu v1,v1,v0 / sll v1,v1,2` @0x80103ca8-b4 */

/* U5: Tabelle @0x8010476c je Eintrag (+0x02 Gier, +0x04 v, +0x06 a, +0x08 vy, +0x0A Zaehler, +0x0C Dreh, +0x0E Dreh-a, +0x10 Seite)
 * und die Euler-Lage +0x68/+0x6A/+0x6C des Teils (FUN_8007746c). */
typedef struct {
    int16_t gier, v, a, vy, c, dreh, dreh_a, seite;
    int16_t eu[3];
    uint8_t eu_da;
} fi_flug_t;

/* ---- Zustand ---------------------------------------------------------------------------------------------------------- */
static struct {
    uint16_t raum;
    int      form;            /* Slot der Riesenform, -1 */
    /* Wurzel */
    uint8_t  genug;           /* +0x1D8 */
    uint8_t  passend;         /* +0x1DA */
    int      uhr;             /* sub10-Zaehler; -1 = nicht gestartet */
    uint8_t  uhr_ab;          /* Set(5,0x0F,1) @0x017D8 */
    uint8_t  wurf;            /* Set(5,0x0D,1) @0x017F2/@0x0180E (einmal) */
    /* Sequenz (Zustand 3) */
    int      phase;           /* RE15_FI_* */
    int      t;               /* +0x158 */
    int      z16a;            /* +0x16A */
    uint8_t  frei;            /* +0x15A */
    uint8_t  getoetet;        /* +0x10E |= 0x4000 (@0x80103c64-68): kein zweiter Ausloeser */
    uint32_t xa_vbl;          /* Phase 8: Austastungen seit dem Start von Xa 0x03 (R1, Echtzeit) */
    int32_t  pl_x, pl_z; int16_t pl_gier; int cut;   /* gemerkt @0x8010369c-cc (0x80104d54/58/5c/60) */
    int32_t  fo_x, fo_z;                             /* +0x230/+0x232 @0x80103610-14 */
    unsigned takt;            /* Spieltakte seit Raumstart (Messschiene) */
    unsigned t_ausl, t_frei;  /* T und U (Messschiene) */
    uint8_t  mtx[32];         /* Quell-Matrix der Wiederholungen (Platz +0x6C) */
    int      w_platz;         /* Messschiene: Platz der Wiederholungsrakete */
    uint8_t  tmtx[32];        /* U5: Teil-Matrix fuer 0x040E2000 (FUN_801005ac a2 = Teil + 0x48) */
    fi_flug_t fl[RE15_FI_TEILE_FLUG];
} s_fi = { .form = -1, .uhr = -1, .w_platz = -1 };

/* S1: Raumtonbank R702 (PC-Seite platform/pc/src/finale_r42_pc.c) */
static void (*s_se)(int satz) = NULL;
void re15_finale_r42_se_hook(void (*fn)(int satz)) { s_se = fn; }

/* ---- Messschiene ------------------------------------------------------------------------------------------------------ */
static void protokoll(const char *fmt, int a, int b, int c)
{
#ifdef RE15_PLATFORM_PC
    static int an = -1; static FILE *fp = NULL;
    if (an < 0) { const char *e = getenv("RE15_FINALE_LOG"); an = (e && *e && *e != '0') ? 1 : 0; if (an) fp = fopen("finale.log", "a"); }
    char z[200];
    snprintf(z, sizeof z, fmt, a, b, c);
    fprintf(stderr, "[finale] T%u F%u %s\n", s_fi.takt, (unsigned)g_engine.frame_count, z);
    if (fp) { fprintf(fp, "[finale] T%u F%u %s\n", s_fi.takt, (unsigned)g_engine.frame_count, z); fflush(fp); }
#else
    (void)fmt; (void)a; (void)b; (void)c;
#endif
}

static int bildlage(int32_t x, int32_t y, int32_t z, int *u, int *tiefe);   /* Messzeile, unten */

/* Stufe 3 (finale_wurf_r42.c): dieselbe Messschiene und dieselbe Raumtonbank. */
void re15_finale_r42_wurf_protokoll(const char *fmt, int a, int b, int c) { protokoll(fmt, a, b, c); }
void re15_finale_r42_se(int satz)
{
    if (s_se) s_se(satz);
    protokoll("Se_on Raumbank R702 Satz 0x%02X (PC-Haken %d)", satz, s_se != NULL, 0);
}

/* ---- Raum ------------------------------------------------------------------------------------------------------------- */
int re15_finale_r42_raum(void) { return re15_bf_bahnsteig_aktiv(); }

void re15_finale_r42_reset(void)
{
    const uint16_t r = s_fi.raum;
    memset(&s_fi, 0, sizeof s_fi);
    s_fi.raum = r; s_fi.form = -1; s_fi.uhr = -1; s_fi.w_platz = -1;
    re15_finale_wurf_reset();
}

void re15_finale_r42_install(uint16_t room_id)
{
    /* Selbst-Tueren (Wagenwechsel, scd_room_reenter) bauen denselben Raum neu auf: Zustand behalten. */
    if (s_fi.raum != room_id) { s_fi.raum = room_id; re15_finale_r42_reset(); }
    if (room_id == 0x5090u) {
        re15_msg_install_text(RE15_FI_MSG_ZEILE_LEON, k_fi_5090_msg02, sizeof k_fi_5090_msg02);
        const int d = re15_msg_compute_duration(k_fi_5090_msg02, sizeof k_fi_5090_msg02, 0);
        if (d > 0 && d < 65535) re15_msg_install_durations(RE15_FI_MSG_ZEILE_LEON, d);
        /* Stufe 3: Zuruf des Werfers (RE15_FW_MSG_ZURUF, tools/r42_finale/texte_finale.py) */
        re15_msg_install_text(RE15_FW_MSG_ZURUF, k_fi_5090_msg03, sizeof k_fi_5090_msg03);
        const int dz = re15_msg_compute_duration(k_fi_5090_msg03, sizeof k_fi_5090_msg03, 0);
        if (dz > 0 && dz < 65535) re15_msg_install_durations(RE15_FW_MSG_ZURUF, dz);
    }
    if (room_id == 0x5091u) {
        re15_msg_install_text(RE15_FI_MSG_ZEILE_ELZA, k_fi_5091_msg02, sizeof k_fi_5091_msg02);
        const int d = re15_msg_compute_duration(k_fi_5091_msg02, sizeof k_fi_5091_msg02, 0);
        if (d > 0 && d < 65535) re15_msg_install_durations(RE15_FI_MSG_ZEILE_ELZA, d);
        re15_msg_install_text(RE15_FW_MSG_ZURUF, k_fi_5091_msg03, sizeof k_fi_5091_msg03);
        const int dz = re15_msg_compute_duration(k_fi_5091_msg03, sizeof k_fi_5091_msg03, 0);
        if (dz > 0 && dz < 65535) re15_msg_install_durations(RE15_FW_MSG_ZURUF, dz);
    }
}

int re15_finale_r42_ende(void) { return re15_game_flag_get(RE15_R42_BANK, RE15_R42_BIT_FINALE_ENDE) != 0; }
int re15_finale_r42_phase(void) { return s_fi.phase; }
int re15_finale_r42_genug(void) { return s_fi.genug; }
int re15_finale_r42_passend(void) { return s_fi.passend; }
int re15_finale_r42_uhr(void) { return s_fi.uhr; }
int re15_finale_r42_wurf_faellig(void) { return s_fi.wurf; }
/* Nach dem Kill liegen die Teile (U5); die Form bleibt im Besitz des Finales bis zum Raumwechsel (RE2: +0x10E |= 0x4000 @0x80103c64-68,
 * die KI kommt nicht zurueck). */
int re15_finale_r42_besitzt(int slot)
{
    if (re15_finale_wurf_besitzt(slot)) return 1;                       /* Stufe 3: Form ruht in der Wurfszene (+0x10E = 0x8000) */
    return s_fi.phase != RE15_FI_AUS && (s_fi.phase < RE15_FI_ENDE || s_fi.getoetet) && slot == s_fi.form;
}
int re15_finale_r42_kein_ziel(const re15_actor_t *e)
{
    if (!e || !re15_finale_r42_raum() || e->type != 0x30u || !re15_bb_bank_da(e)) return 0;
    if (s_fi.phase != RE15_FI_AUS) return 1;                                   /* Phase 0 @0x8010361c-24 */
    return re15_player_equipped_weapon() == 18;                                /* Wurzel @0x801018d4-0x80101900 (RE2-Id 0x11) */
}

/* ---- Wurzel (S4) ------------------------------------------------------------------------------------------------------ */
int re15_finale_r42_start_hp(int bisher)
{
    return re15_finale_r42_raum() ? RE15_FI_HP_START : bisher;   /* `sh v0,0x156(s0)` @0x80101be0 */
}

/* Kampfbeginn = Ende von sub04 (5090 @0x01344/@0x01348 Set(1,27,0)/Set(2,7,0), danach Cut_auto/Plc_ret @0x0134C; 5091 entsprechend).
 * RE2 startet die Uhr beim Kampfbeginn (sub07 Evt_exec sub10 @0x016C0) und sperrt den Abschuss bis dahin ueber +0x10E & 0x4000
 * (gesetzt @0x80102f4c, geloescht @0x80102f8c). PORT-ZUORDNUNG: Kampf laeuft = (3,42) gesetzt (sub01 @0x0129A), Riegel (2,7) des
 * Skripts frei und die Annette-Szene nicht im Besitz der Form. */
static int kampf_laeuft(int slot)
{
    if (!re15_game_flag_get(3, 42)) return 0;
    if (re15_game_flag_get(2, 7)) return 0;
    if (re15_annette_r41_besitzt(slot)) return 0;
    return 1;
}

void re15_finale_r42_kampf_start_test(void) { if (s_fi.uhr < 0) s_fi.uhr = RE15_FI_UHR; }

void re15_finale_r42_form_takt(int slot, int gehen, int32_t dist)
{
    if (!re15_finale_r42_raum()) return;
    re15_actor_t *e = &g_actors[slot];
    s_fi.form = slot;
    /* +0x1DA: je Bild 0 (@0x80101a10), im Gehzustand ab Abstand 2801 auf 1 (@0x80101d98-db0) */
    s_fi.passend = (uint8_t)((gehen && dist >= RE15_FI_PASSEND_AB) ? 1 : 0);
    /* +0x1D8 und Untergrenze nach dem Zustand (@0x80101ab0-0x80101adc) */
    if (e->hp < RE15_FI_HP_GENUG && !s_fi.genug) { s_fi.genug = 1; protokoll("genug Schaden (hp %d < 20000)", e->hp, 0, 0); }
    if (e->hp < RE15_FI_HP_UNTEN) e->hp = RE15_FI_HP_UNTEN;
    /* Uhr sub10 */
    if (s_fi.uhr < 0 && kampf_laeuft(slot)) {
        s_fi.uhr = RE15_FI_UHR;
#ifdef RE15_PLATFORM_PC
        {   /* PRUEFHAKEN RE15_FINALE_UHR=<n> (Stufe 3, kein Spielverhalten): Uhr fuer die Integrationslaeufe der Wurfszene kuerzen;
             * der Wert im Spiel bleibt RE15_FI_UHR = 1800 (sub10 @0x017D4, Riegel unit_r42_finale). */
            const char *eu = getenv("RE15_FINALE_UHR");
            if (eu && *eu && atoi(eu) > 0) s_fi.uhr = atoi(eu);
        }
#endif
        protokoll("Kampf laeuft, Uhr %d Bilder", s_fi.uhr, 0, 0);
    }
    else if (s_fi.uhr > 0 && --s_fi.uhr == 0) { s_fi.uhr_ab = 1; protokoll("Uhr abgelaufen (Set(5,0x0F))", 0, 0, 0); }
    /* sub11 @0x017DE-0x01818, von sub01 @0x0134A je Bild gerufen solange Ck(5,0x0D)=0:
     *   if (+0x1D8 == 1 && +0x1DA == 1) or (Ck(5,0x0F) == 1 && +0x1DA == 1)  ->  Set(5,0x0D,1), Evt_exec sub12 (Wurf). */
    if (!s_fi.wurf && s_fi.uhr >= 0 && s_fi.phase == RE15_FI_AUS && s_fi.passend && (s_fi.genug || s_fi.uhr_ab)) {
        s_fi.wurf = 1;
        protokoll("Wurf faellig (genug %d, Uhr ab %d, Abstand %d)", s_fi.genug, s_fi.uhr_ab, dist);
        (void)re15_finale_wurf_start(slot);                             /* Evt_exec sub12 @0x017F6 / @0x01812 (Stufe 3) */
    }
}

/* ---- Hilfen ----------------------------------------------------------------------------------------------------------- */
/* FUN_800154ac @0x800154ac: Winkel von (x0,z0) nach (x1,z1) (`subu s0,a2,a0` @0x800154c8, `subu a0,a3,a1` @0x800154dc, catan
 * @0x80015520, Quadrant @0x80015530-38, `andi 0xfff` @0x80015540) - wie g5_catan2 (enemy_ai_boss_g5.c). */
static int32_t peilung(int32_t x0, int32_t z0, int32_t x1, int32_t z1)
{
    const int32_t dx = x1 - x0, dz = z1 - z0;
    if (dx == 0) return (dz > 0) ? 0xC00 : 0x400;
    const int32_t q = (dx < 0) ? 0x800 : 0;
    return (q - (int32_t)(int16_t)re15_catan((int32_t)(((int64_t)dz * 4096) / dx))) & 0xFFF;
}

/* FUN_801016cc(e, d): Spieler d vor den Gegner (`lh a0,0x76(s0)` Gier, FUN_80077384 = RotMatrix(0,gier,0) + ApplyMatrixSV auf
 * (d,0,0) @0x801016d8-f0, + Gegnerlage @0x801016f8-734), Spielergier = Peilung Spieler -> Gegner - 0x32 (@0x80101738-50). */
int32_t re15_finale_r42_peilung(int32_t x0, int32_t z0, int32_t x1, int32_t z1) { return peilung(x0, z0, x1, z1); }

static void spieler_vor(const re15_actor_t *e, int32_t d)
{
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    const uint16_t rot[3] = { 0, (uint16_t)(e->rot_y & 0xfff), 0 };
    int16_t m[9];
    re15_door_rotmatrix(rot, m);
    pl->x = (int16_t)(e->x + (int32_t)(((int64_t)m[0] * d) >> 12));   /* `sh a0,0x10(sp)` 16 Bit @0x80101708 */
    pl->z = (int16_t)(e->z + (int32_t)(((int64_t)m[6] * d) >> 12));
    pl->rot_y = (int16_t)((peilung(pl->x, pl->z, e->x, e->z) - 0x32) & 0xfff);
}

/* Kamera der Szene: wie Cut_chg (Zonen aus, Bit 0x100 @0x800402d4; Port cut_auto_enabled = 0, Muster birkin_formen.c bf_kamera). */
static void kamera(int cut)
{
    g_scd.cut_auto_enabled = 0;
    if ((int)g_scd.work_vars[0x0A] != cut || g_scd.cam_id != cut) { g_scd.cam_id = (uint8_t)cut; g_scd.cam_change_pending = 1; }
}

/* MATRIX{ short m[3][3]; long t[3]; } (32 Byte) aus einer Port-Weltmatrix (Q12) und Lage. */
static void matrix_packen(uint8_t out[32], const int32_t r[9], const int32_t t[3])
{
    for (int k = 0; k < 9; k++) {
        int32_t v = r[k];
        if (v > 32767) v = 32767; else if (v < -32768) v = -32768;
        out[2 * k] = (uint8_t)v; out[2 * k + 1] = (uint8_t)(v >> 8);
    }
    out[18] = 0; out[19] = 0;
    for (int k = 0; k < 3; k++) {
        out[20 + 4 * k] = (uint8_t)t[k]; out[21 + 4 * k] = (uint8_t)(t[k] >> 8);
        out[22 + 4 * k] = (uint8_t)(t[k] >> 16); out[23 + 4 * k] = (uint8_t)(t[k] >> 24);
    }
}

/* Quell-Matrix der Wiederholung: Teil 11 des Spielers (Kopf RE15_FI_QUELLE_*). Der Port stellt Knochen 11 jedes Bild, auch wenn der
 * Spieler unsichtbar ist (main.c: Pose "auch gecullt" -> re15_player_set_hand_world/_rot), wie RE2 die Teil-Matrizen weiterrechnet. */
static int quelle_bauen(void)
{
    int32_t r[9], t[3];
    if (!re15_player_gunbone_matrix(r, t)) return 0;
    matrix_packen(s_fi.mtx, r, t);
    return 1;
}

static void wiederholung(uint32_t a0, uint8_t besitzer)
{
    static const int16_t OM[4] = { RE15_FI_QUELLE_X, 0, RE15_FI_QUELLE_Z, 0 };   /* (600,0,400) @0x80103828-38 */
    const re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    int i = -1;
    int aus = 0;
#ifdef RE15_PLATFORM_PC
    {   /* MESSHAKEN RE15_FINALE_W_AUS=<Besitzer hex> (Runde 3, kein Spielverhalten): diese Wiederholung ohne Effekt - der Bildvergleich
         * mit einem Lauf mit Effekt zaehlt dessen sichtbare Bildpunkte (Pruefbericht s1 r2 R1: W3 war verdeckt). */
        const char *ev = getenv("RE15_FINALE_W_AUS");
        aus = (ev && *ev && (uint8_t)strtoul(ev, NULL, 16) == besitzer);
    }
#endif
    if (!aus && quelle_bauen()) {
        re2fx_boden_basis_setzen(pl->y);
        /* FUN_8001c8c4(a0, a1 = Besitzer, a2 = Teil 11, a3 = Versatz) @0x8010385c / @0x80103960 / @0x80103a60: +0x22 = a1 >> 16 = 0 */
        i = re2fx_spawn_besitzer(a0, besitzer, 0, s_fi.mtx, OM);
        s_fi.w_platz = i;
        if (i >= 0 && i < RE2FX_PLAETZE) re2fx_r35_schuetze(i, pl->x, pl->z);
    }
    re15_audio_re2_arms_se(0x11, 0);                  /* Se_on 0x01000001 @0x80103864-74 (ARMS11 Satz 0, wie werfer_r35.c) */
    {   /* Messzeilen: Quelle (Knochen 11 + Versatz) und Form, fuer die Flugrichtung gegen die Form */
        int32_t r[9], t[3];
        const re15_actor_t *e = &g_actors[s_fi.form];
        if (re15_player_gunbone_matrix(r, t))
            protokoll("Wiederholung Quelle (%d,%d,%d)", t[0] + (int32_t)(((int64_t)r[0] * OM[0] + (int64_t)r[2] * OM[2]) >> 12),
                      t[1] + (int32_t)(((int64_t)r[3] * OM[0] + (int64_t)r[5] * OM[2]) >> 12),
                      t[2] + (int32_t)(((int64_t)r[6] * OM[0] + (int64_t)r[8] * OM[2]) >> 12));
        protokoll("Wiederholung Form (%d,%d) Schnitt %d", e->x, e->z, (int)g_scd.cam_id);
    }
    protokoll("Wiederholung %08x Besitzer 0x%02x -> Platz %d", (int)a0, besitzer, i);
}

/* Pose der Form waehrend der Sequenz. RE2 setzt +0x14C = 0x0A05 (Clip 5 Bild 10, @0x801035e8-ec) und animiert bis Phase 7 nicht mehr;
 * dieselbe Pose setzt der Stehzustand @0x80101d38-58. PORT-ZUORDNUNG nach Rolle (Plan D11, Rolle U4 offen): Stehen-Clip der Riesenform
 * BB_C_STEHEN 2, Bild 0 = Ruhepose (birkin_bahnsteig.c). Der Port-Animator laeuft unabhaengig von der KI, darum je Takt halten. */
static void form_halten(re15_actor_t *e)
{
    e->motion = 2; e->anim_frame = 0; e->anim_frac = 0;
    e->anim_flags &= (uint16_t)~0x80u;
}

/* ---- Abschuss (S6) ---------------------------------------------------------------------------------------------------- */
/* Port-Programm = ROOM7020 sub15 @0x01A22-0x01A70, Befehl fuer Befehl in RE1.5-Opcodes. Gestartet im Ausloesebild T; die VM laeuft
 * vor dem Spielschritt, der erste Befehl also im Bild T+1 wie RE2 (sub01 -> sub14 -> sub15 im Folgebild, Dossier B 2.3). */
#define LE16(v) (uint8_t)((uint16_t)(v) & 0xff), (uint8_t)(((uint16_t)(v) >> 8) & 0xff)
static const uint8_t k_prog_leon[] = {
    0x54, 0x01, 0x02, 0x00, 0x00, 0x00,           /* Kampfmusik aus: RE2 `51 02 02` @0x01A22 (Deutung U10 offen); Port = 5090 sub01
                                                   * @0x012AE `54 01 02 00 00 00` SUB stoppen (scd_vm.c Sce_bgm_control) */
    0x22, 0x05, RE15_FI_SCHUETTEL_AUS, 0x01,      /* Set(5,0x14,1) @0x01A28 `22 05 14 01` (in 5090/5091 ohne Leser: Bit frei, Zensus) */
                                                  /* Set(1,4,0) @0x01A2C `22 01 04 00`: PORT-WAHL weggelassen - RE2-Bank 1 Bit 4 ist dort der
                                                   * Strom-/Lichtzustand (sub16 Set(1,4,1) mit Cut_replace 12->0), in der RE1.5-Bank 1 des Ports
                                                   * ist Index 4 anders belegt; im Wagen gibt es keinen Stromausfall. */
    0x09, 0x0a, LE16(RE15_FI_SCHLAF_ZEILE),       /* @0x01A30 `09 0a 0f 00` */
    0x2b, RE15_FI_MSG_ZEILE_LEON, 0x00, 0x00,     /* IfEl Ck(1,0) @0x01A34 waehlt Xa_on(0,2) @0x01A3C + Meldung 7 (Leon) bzw. @0x01A4A +
                                                   * Meldung 9; PORT-WAHL: je Raum ein Programm (5090 Leon, 5091 Elza), K-11: Message_on mit
                                                   * Stimme (synchro main02.wav), Untertitel-Maske 0 wie ROOM6030 sub02 @0x00DC2 */
    0x09, 0x0a, LE16(RE15_FI_ZEILE_BILDER),       /* Wsleep/Wsleeping @0x01A56: Lesefenster 162 Sektoren = 33 Bilder (Kopf) */
    0x22, 0x05, RE15_FI_SIG_FREI, 0x01,           /* Member_set(0x18,2) @0x01A5C */
    0x09, 0x0a, LE16(RE15_FI_SCHLAF_W1A),         /* @0x01A60 */
    0x09, 0x0a, LE16(RE15_FI_SCHLAF_W1B),         /* @0x01A64 */
    0x22, 0x05, RE15_FI_SIG_W1, 0x01,             /* Member_set(0x18,3) @0x01A6C */
    0x01, 0x00                                    /* Evt_end @0x01A70 */
};
/* Elza: ROOM7021 sub15 @0x019E8-0x01A38 ist Befehl fuer Befehl gleich (Sleep 15 @0x019F6, Xa_on @0x01A02/@0x01A10, Wsleep @0x01A1C,
 * Member_set(0x18,2) @0x01A22, Sleep 32 @0x01A26, Sleep 5 @0x01A2A, Member_set(0x18,3) @0x01A32; Dossier B 2.1); nur das Lesefenster
 * ist laenger (71 statt 33 Bilder, Kopf). Zeile "You lose, big guy!" (NUTZERWUNSCH 17.11., K-11 Claire). */
static const uint8_t k_prog_elza[] = {
    0x54, 0x01, 0x02, 0x00, 0x00, 0x00,           /* SUB stoppen wie 5090 @0x012AE / 5091 @0x01292 */
    0x22, 0x05, RE15_FI_SCHUETTEL_AUS, 0x01,      /* Set(5,0x14,1) wie Leon (ROOM7021 sub15 Befehl fuer Befehl gleich, Dossier B 2.1) */
    0x09, 0x0a, LE16(RE15_FI_SCHLAF_ZEILE),       /* @0x019F6 */
    0x2b, RE15_FI_MSG_ZEILE_ELZA, 0x00, 0x00,     /* Xa_on(0,2) @0x01A02 -> Message_on (K-11) */
    0x09, 0x0a, LE16(RE15_FI_ZEILE_BILDER_ELZA),  /* Wsleep @0x01A1C: 354 Sektoren = 71 Bilder */
    0x22, 0x05, RE15_FI_SIG_FREI, 0x01,           /* @0x01A22 */
    0x09, 0x0a, LE16(RE15_FI_SCHLAF_W1A),         /* @0x01A26 */
    0x09, 0x0a, LE16(RE15_FI_SCHLAF_W1B),         /* @0x01A2A */
    0x22, 0x05, RE15_FI_SIG_W1, 0x01,             /* @0x01A32 */
    0x01, 0x00
};
static int ist_elza(void) { return (g_current_room_id & 0xFFFFu) == 0x5091u; }

const uint8_t *re15_finale_r42_programm(int elza, int *out_len)
{
    if (out_len) *out_len = elza ? (int)sizeof k_prog_elza : (int)sizeof k_prog_leon;
    return elza ? k_prog_elza : k_prog_leon;
}

static int programm_starten(void)
{
    for (int slot = SCD_EVENT_SLOT_FIRST; slot <= SCD_EVENT_SLOT_LAST; slot++)
        if (!g_scd.threads[slot].active && scd_thread_start(slot, ist_elza() ? k_prog_elza : k_prog_leon) == 0) return slot;
    return -1;
}

/* Ausloeser der Tyrant-Wurzel (@0x80101904-0x80101a00), alle Bedingungen:
 *   Spieler zielt mit dem Raketenwerfer (Bank 1 Bit 0x40000, gesetzt im Zielzustand bei Waffe 17 @0x800437b8-cc; RE1.5-Id 18),
 *   Munition (FUN_80069f54 @0x8010193c), kein Sperrbit +0x10E & 0x4000 (@0x8010195c), Feuer gedrueckt (0x800ce30c & 0x40
 *   @0x801019b4-c4; RE1.5: SQUARE gehalten im Zielen, player_common.c), |Peilung Spieler -> Gegner - Gier| < 300 (@0x80101970-dc).
 * Treffer: Zustand 3 (@0x801019e4-e8) und das Feuer ist verbraucht (`addiu v1,zero,-0x41 / and / sw` @0x801019ec-a00). */
static int ausloeser(uint16_t *pad, uint16_t *pressed)
{
    if (s_fi.form < 0 || s_fi.getoetet || s_fi.uhr < 0) return 0;
    if (re15_finale_wurf_aktiv()) return 0;                             /* Stufe 3: Pad gesperrt (Set(2,7,1) @0x0181A), Form ruht */
    re15_actor_t *e = &g_actors[s_fi.form];
    const re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    if (!e->active || !re15_bb_bank_da(e)) return 0;
    if ((*pad & RE15_PAD_BIT_SQUARE) && (s_fi.takt % 15u) == 0u) {  /* Messzeile (alle 15 Takte bei Feuer): welche Bedingung fehlt */
        int32_t d0 = (peilung(pl->x, pl->z, e->x, e->z) - (int32_t)pl->rot_y) & 0xfff;
        if (d0 >= 0x801) d0 -= 0x1000;
        protokoll("Feuer-Flanke: Waffe %d, zielt %d, Munition %d", re15_player_equipped_weapon(),
                  re15_player_werfer_zielt() * 10 + ((*pad & RE15_PAD_BIT_R1) ? 1 : 0), re15_ammo_mag_nonzero());
        protokoll("Feuer-Flanke: Blick %d (Grenze 300), Abstand %d", d0, re15_enemy_player_dist(e, pl), 0);
    }
    if (re15_player_equipped_weapon() != 18) return 0;
    if (!re15_player_werfer_zielt() || !(*pad & RE15_PAD_BIT_R1)) return 0;
    if (!re15_ammo_mag_nonzero()) return 0;
    if (!(*pad & RE15_PAD_BIT_SQUARE)) return 0;
    int32_t d = (peilung(pl->x, pl->z, e->x, e->z) - (int32_t)pl->rot_y) & 0xfff;   /* @0x8010199c-a0 */
    if (d >= 0x801) d |= (int32_t)0xfffff000;                                        /* `ori a0,v1,0xf000` @0x801019b0 */
    if (d < 0) d = -d;                                                               /* @0x801019d0-d8 */
    if (d >= RE15_FI_BLICK) return 0;
    *pad = (uint16_t)(*pad & ~RE15_PAD_BIT_SQUARE);                                  /* Feuer verbraucht */
    if (pressed) *pressed = (uint16_t)(*pressed & ~RE15_PAD_BIT_SQUARE);
    return 1;
}

/* Phase 0, Teil Fixpunkte + Schnitt (@0x801035cc-0x801036dc). Eigene Funktion fuer den Riegel re15_finale_r42_schnitt_test (Runde 4,
 * Pruefbericht s1 r3 R1): Ausloesen in jeder Lauf-Kamera 11-14 muss einen Kamerawechsel anfordern, damit das Bild mit dem Versetzen
 * gehalten wird (kamerahalt_r41.c = RE2 @0x8002ba50/@0x8002bb58). Rueckgabe: der Schnitt vor dem Ausloesen. */
static int fixpunkte_und_schnitt(re15_actor_t *e, re15_actor_t *pl)
{
    /* Lagen: RE2 stellt Gegner (-14547,-14636) Gier 1432 und Leon (-19025,200,-20861) Gier 4433 fest (@0x801035cc-e0 /
     * @0x80103638-74); Leon steht damit 7668 vor dem Gegner, Gegnergier = Peilung zu Leon (1432 gegen gerechnet 1430).
     * U3 (Kopf RE15_FI_FORM_*): Form auf den Fixpunkt in Wagen C, Spieler 7668 davor auf ihrer Achse (Spielergier nach der Regel von
     * FUN_801016cc @0x80101738-50; die feste RE2-Gier 0x1151 gilt fuer ROOM7020-Koordinaten). */
    s_fi.fo_x = e->x; s_fi.fo_z = e->z;                                   /* +0x230/+0x232 @0x80103610-14 */
    s_fi.pl_x = pl->x; s_fi.pl_z = pl->z; s_fi.pl_gier = pl->rot_y;       /* @0x801036a4-b0 (vor dem Versetzen gemerkt) */
    e->x = RE15_FI_GO_FORM_X; e->z = RE15_FI_GO_FORM_Z; e->rot_y = RE15_FI_GO_FORM_GIER;   /* PORT-WAHL Runde 4: eigene "Game over"-
                                                                           * Lage auf Schnitt 15 (Kopf RE15_FI_GO_*) */
    form_halten(e);                                                       /* +0x14C = 0x0A05 @0x801035e8-ec (D11, U4) */
    spieler_vor(e, RE15_FI_T_ABSTAND);
    s_fi.cut = (int)g_scd.work_vars[0x0A];                                /* @0x8010369c / @0x801036cc */
    re15_game_flag_set(2, 0, 1);                                          /* Bank 2 |= 0x80000000 @0x80103678-8c (Spieler) */
    re15_game_flag_set(2, 7, 1);                                          /*          |= 0x01000000 (Pad) */
    re15_game_flag_set(1, 27, 1);                                         /* Bank 1 |= 0x10 @0x801036b8-c4 (Kinobalken) */
    g_scd.cut_auto_enabled = 0;                                           /* Bank 1 |= 0x100 @0x801036bc-d4 (1,0x17): Zonenwechsel
                                                                           * aus (PORT-ZUORDNUNG wie bf_kamera) */
    const int vorher = (int)g_scd.work_vars[0x0A];
    kamera(RE15_FI_CUT_ZEILE);                                            /* FUN_8002c7ac(11) @0x801036d8 -> U3 Runde 4: Schnitt 15 */
    return vorher;
}

/* Stufe 3 (Reaktions-Schnitt, re15_finale_wurf_r42.h): Leons "Game over"-Lage = 7668 vor der "Game over"-Lage der Form, wie
 * fixpunkte_und_schnitt (FUN_801016cc-Regel), ohne die Form zu bewegen. */
void re15_finale_r42_go_spieler(int32_t *x, int32_t *z)
{
    const uint16_t rot[3] = { 0, (uint16_t)(RE15_FI_GO_FORM_GIER & 0xfff), 0 };
    int16_t m[9];
    re15_door_rotmatrix(rot, m);
    *x = (int16_t)(RE15_FI_GO_FORM_X + (int32_t)(((int64_t)m[0] * RE15_FI_T_ABSTAND) >> 12));
    *z = (int16_t)(RE15_FI_GO_FORM_Z + (int32_t)(((int64_t)m[6] * RE15_FI_T_ABSTAND) >> 12));
}

int re15_finale_r42_schnitt_test(int form_slot)
{
    if (form_slot < 0 || form_slot >= RE15_ACTOR_MAX) return -1;
    s_fi.form = form_slot;
    return fixpunkte_und_schnitt(&g_actors[form_slot], &g_actors[RE15_ACTOR_SLOT_PLAYER]);
}

/* Phase 0 @0x801035c0-0x80103700 */
static void phase0(void)
{
    re15_actor_t *e = &g_actors[s_fi.form];
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    const int vorher = fixpunkte_und_schnitt(e, pl);
    s_fi.phase = RE15_FI_P1; s_fi.t = 0; s_fi.z16a = 0; s_fi.frei = 0;   /* +0x1D6 = 1, +0x158 = 0, Phase 1 @0x801036f4-700 */
    const int slot = programm_starten();                                  /* RE2: +0x1D6 = 1 -> sub01/sub14 -> sub15 */
    s_fi.t_ausl = s_fi.takt;
    protokoll("Ausloeser: Abschuss (Form hp %d), Programm Slot %d, Schnitt %d", e->hp, slot, RE15_FI_CUT_ZEILE);
    protokoll("Fixpunkte: Form (%d,%d), Spieler x %d", e->x, e->z, pl->x);
    protokoll("Ausloese-Schnitt: vorher %d, angefordert %d, Wechsel %d", vorher, (int)g_scd.cam_id, g_scd.cam_change_pending ? 1 : 0);
    {   int uf = -1, tf = 0, us = -1, ts = 0;
        const int df = bildlage(e->x, -1500, e->z, &uf, &tf), ds = bildlage(pl->x, -900, pl->z, &us, &ts);
        protokoll("Bildlage Game over: Form u %d Tiefe %d", df ? uf : -9999, tf, 0);
        protokoll("Bildlage Game over: Spieler u %d Tiefe %d", ds ? us : -9999, ts, 0); }
}

static void loesche_rauch_und(uint8_t besitzer)
{
    re15_game_flag_set(2, 3, 0);                                          /* Bank 2 &= 0xefffffff @0x801038c8-f0 / @0x801039cc-f4 */
    re2fx_loesche_bank_sub(3, 0x0A, NULL);                                /* FUN_8001cefc(3,10,0) @0x801038ec / @0x801039f0 */
    re2fx_loesche_besitzer(besitzer);                                     /* FUN_8001cfbc @0x801038f4 / @0x801039f8 */
}

static void wiederholungs_schnitt(int32_t abstand, int cut, const char *name)
{
    re15_actor_t *e = &g_actors[s_fi.form];
    e->x = RE15_FI_FORM_X; e->z = RE15_FI_FORM_Z; e->rot_y = RE15_FI_FORM_GIER;   /* PORT-WAHL Runde 4: W-Lage (U3 Schritt 1); RE2 hat
                                                                           * nur die eine Lage @0x801035cc-e0, umgesetzt im Schnittbild */
    spieler_vor(e, abstand);
    s_fi.w_platz = -1;
    kamera(cut);                                                          /* Schnitt 8/9/10 @0x801037dc/@0x801038fc/@0x80103a00 -> U3 */
    protokoll(name[1] == '1' ? "W1: Spieler %d vor der Form, Schnitt %d" : name[1] == '2' ? "W2: Spieler %d vor der Form, Schnitt %d"
                                                               : "W3: Spieler %d vor der Form, Schnitt %d", abstand, cut, 0);
}

static void kill(void)
{
    re15_actor_t *e = &g_actors[s_fi.form];
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    /* FUN_80018fb0 @0x80103d7c = Abschuss-Flag (Bank 7, +0x1CE). Port: das Ende des Finales = (5,0x1C) wie der G5-Tod bisher
     * (5090 sub01 @0x012A6 -> sub02 Abspann) + Bank-9-Bit 129 (T-07, Sperren in scd_vm.c / aot_common.c haengen daran). */
    re15_game_flag_set(RE15_R42_BANK, RE15_R42_BIT_FINALE_ENDE, 1);
    re15_game_flag_set(5, 0x1c, 1);
    /* Spieler zurueck auf die gemerkte Lage, y = 0 (@0x80103d84-db8) */
    pl->x = s_fi.pl_x; pl->z = s_fi.pl_z; pl->y = 0; pl->rot_y = s_fi.pl_gier;
    (void)e;                                                              /* die Teile bleiben liegen (U5); die Form gehoert dem Finale */
    protokoll("Kill: (5,0x1C) + Bit 129, Spieler zurueck (Xa-Fenster %d Austastungen)", (int)s_fi.xa_vbl, 0, 0);
}

/* ---- U5: Zerplatzen ---------------------------------------------------------------------------------------------------
 * PORT-ZUORDNUNG nach Rolle (Messung Strang-Scratchpad u5/u5_zuordnung.md, Gerippe gelesen aus build/extracted/re2_ems/
 * CDEMD0_EM2B_emd.EMD = CDEMD0.EMS ab 0x48A800 + EMD): RE2-Super-Tyrant 21 Teile, Eltern [-1,0,1,2,0,4,5,6,7,7,7,7,5,12,13,13,13,13,0,18,19]
 * = 0 Huefte; 1-2-3 Bein (z +385); 4 Rumpf; 5 Brust mit Kopf; 6-7 Arm (z +675) mit Fingern 8-11; 12-13 Arm (z -548) mit Klauen
 * 14-17; 18-19-20 Bein (z -385). Riesenform 32 Teile (analysis/befunde_runde41/form/gerippe.json: 0 huefte, 1-6 schwanz, 7 wirbel,
 * 8/9 oberschenkel l/r, 10 brust, 11-13 arm_l, 14/15/28 klaue_l, 16-18 arm_r, 19/20/29 klaue_r, 21 hals, 22 kopf, 23 kiefer, 24/25
 * Bein l unten, 26/27 Bein r unten, 30/31 Klauen). Jeder RE2-Teil -> der Form-Teil derselben Rolle; die Teile der Form ohne
 * RE2-Gegenstueck gehoeren zur Rolle ihres Elternteils: Schwanz 1-6 zur Huefte (RE2 0), Hals/Kopf/Kiefer 21-23 zur Brust (RE2 5,
 * deren Netz den Kopf traegt). 30/31 haengen an den Haenden und folgen ihnen. */
static const struct { uint8_t re2, form, marke; } k_flug[RE15_FI_TEILE_FLUG] = {
    /* Tabelle @0x8010476c, Eintrag i: Teil (+0x00) und Marke (+0x12), selbst gelesen (Datei @0x476C + 20*i) */
    { 1, 8, 1 }, { 2, 24, 1 }, { 4, 7, 1 }, { 6, 11, 1 }, { 8, 13, 0 }, { 9, 14, 0 }, { 10, 15, 0 }, { 11, 28, 0 },
    { 12, 16, 1 }, { 14, 18, 1 }, { 15, 19, 0 }, { 16, 20, 0 }, { 17, 29, 0 }, { 18, 9, 0 }, { 19, 26, 0 } };
#define FI_BROCKEN_N 15
static const struct { uint8_t re2, form; } k_brocken[FI_BROCKEN_N] = {      /* a1 @0x80103c14/20/2c/38/44/50 = 0, 3, 5, 7, 13, 0x14 */
    { 0, 0 }, { 0, 1 }, { 0, 2 }, { 0, 3 }, { 0, 4 }, { 0, 5 }, { 0, 6 },  /* Huefte + Schwanz */
    { 3, 25 }, { 5, 10 }, { 5, 21 }, { 5, 22 }, { 5, 23 },                 /* Fuss l; Brust + Hals/Kopf/Kiefer */
    { 7, 12 }, { 13, 17 }, { 0x14, 27 } };                                 /* Unterarm l, Unterarm r, Fuss r */
#define FI_FLUG_V      200       /* Tabelle +0x04 (Datei @0x4770 `c8 00`) */
#define FI_FLUG_VY   (-200)      /* Tabelle +0x08 (Datei @0x4774 `38 ff`) */
#define FI_BROCKEN_VY  (-10)     /* `addiu v0,zero,-0xa / sh v0,0x9a(v1)` @0x801015d0-d8 */
#define FI_BROCKEN_FARBE 0x202038u /* `lui a2,0x20 / ori a2,a2,0x2038 / sw a2,0x70(v1)` @0x8010158c/@0x801015b8/@0x801015e4 */
#define FI_FUNKE     0x040E2000u /* FUN_801005ac `lui a0,0x40e / ori a0,a0,0x2000` @0x8010061c-20 */

static int16_t teil_rand8(void) { return (int16_t)(int8_t)re15_engine_rand8(); }   /* FUN_80015fe8, unteres Byte */

/* FUN_8007746c(a0, a1 = +0x68, a2 = +0x48, a3 = 0): Euler (x, y, z) der Teil-Matrix fuer RotMatrix = Rx*Ry*Rz (re15_door_rotmatrix,
 * Zeile 0 = (cy*cz, -cy*sz, sy)). x = ratan2(-m12, m22) (@0x80077540-54), y = ratan2(m02', SquareRoot12(4096 - m02'^2)) mit
 * m02' = (m02 << 12) / (|Spalte 2|^2 >> 12) (@0x800774c8-80077580), z = ratan2(w.y, w.x) mit w = (RotMatrix(x,y,0))^T * Spalte 0
 * (@0x80077590-800775e0). ratan2 = PsyQ-Bibliothek (RE2 @0x8008f31c, RE1.5 @0x80065de0 = re15_ratan2); SquareRoot12 @0x8008d624 ist
 * hier SquareRoot0(x * 4096) (re15_squareroot0) - gleiche Rechnung, Rundung der Tabelle nicht bit-gleich geprueft (Praezision). */
static void euler_aus(const int32_t m[9], int16_t eu[3])
{
    const int32_t m02 = m[2], m12 = m[5], m22 = m[8];
    int32_t q = (int32_t)(((int64_t)m02 * m02 + (int64_t)m12 * m12 + (int64_t)m22 * m22) >> 12);
    if (q == 0) q = 1;
    const int32_t sy = (int32_t)(((int64_t)m02 << 12) / q);
    eu[0] = (int16_t)re15_ratan2(-m12, m22);
    int32_t k = 4096 - (int32_t)(((int64_t)sy * sy) >> 12);
    if (k < 0) k = 0;
    eu[1] = (int16_t)re15_ratan2(sy, (int32_t)re15_squareroot0((uint32_t)k << 12));
    const uint16_t xy0[3] = { (uint16_t)eu[0], (uint16_t)eu[1], 0 };
    int16_t r[9];
    re15_door_rotmatrix(xy0, r);
    const int32_t c0[3] = { m[0], m[3], m[6] };
    const int32_t wx = (int32_t)(((int64_t)r[0] * c0[0] + (int64_t)r[3] * c0[1] + (int64_t)r[6] * c0[2]) >> 12);   /* R^T */
    const int32_t wy = (int32_t)(((int64_t)r[1] * c0[0] + (int64_t)r[4] * c0[1] + (int64_t)r[7] * c0[2]) >> 12);
    eu[2] = (int16_t)re15_ratan2(wy, wx);
}

/* Phase 7: FUN_80100108 (15 Flugteile) und sechsmal FUN_8010158c (Brocken). */
static void zerplatzen_start(void)
{
    re15_teil_t *T = re15_teile(s_fi.form);
    if (!T) return;
    re15_teile_reset(s_fi.form);
    for (int i = 0; i < RE15_FI_TEILE_FLUG; i++) {
        const int k = k_flug[i].form;
        if (k >= RE15_TEILE_MAX) continue;
        T[k].w |= 0x40u;                                                  /* `ori v0,v0,0x40` @0x80100178-80 */
        s_fi.fl[i].gier = (int16_t)(((int)re15_engine_rand8() & 0xff) << 4);   /* `andi 0xff / sll 4 / sh 2(s1)` @0x80100184-90 */
        { const int16_t r = teil_rand8();                                  /* (s8 + Vorzeichen) >> 1, `srl` + `sh` (16 Bit) @0x80100194-b4 */
          s_fi.fl[i].v = (int16_t)(FI_FLUG_V + (int16_t)((r + (r < 0 ? 1 : 0)) >> 1)); }
        s_fi.fl[i].a = 0;                                                 /* Tabelle +0x06 = 0 */
        s_fi.fl[i].vy = (int16_t)(FI_FLUG_VY + teil_rand8());             /* @0x801001b8-cc */
        s_fi.fl[i].dreh = (int16_t)(teil_rand8() * 2);                    /* `sll 0x18 / sra 0x17` @0x801001d0-e8 */
        s_fi.fl[i].dreh_a = 0;                                            /* `sh zero,0xe(s1)` @0x801001ec */
        s_fi.fl[i].c = 0;                                                 /* `sh zero,0xa(s1)` @0x80100224 */
        s_fi.fl[i].eu_da = 0;                                             /* Euler + Seite (+0x10) beim ersten gezeichneten Bild */
    }
    for (int j = 0; j < FI_BROCKEN_N; j++) {
        const int k = k_brocken[j].form;
        if (k >= RE15_TEILE_MAX) continue;
        /* RE2 Bit 0x4A = 0x40 eigene Matrix + 0x08 Drift/Zerfall (FUN_80028DAC: vy +0x9A, Ende nach 0x1D Bildern) + 0x02.
         * Gegenstueck der RE1.5-Teilschicht: 0x40 + 0x08 Scherben (FUN_8001f1e0: vy = h94[1], += h94[3], Ende ab 0x1D). */
        T[k].w |= 0x48u;
        T[k].h94[1] = (uint16_t)FI_BROCKEN_VY;                            /* +0x9A = -10 */
        T[k].h94[3] = 0;                                                  /* +0x9E = 0 */
        T[k].h94[4] = 0;                                                  /* +0x9C = 0 */
        T[k].col = FI_BROCKEN_FARBE;                                      /* +0x70 = 0x00202038 */
    }
    re15_teile_markieren(s_fi.form);
}

/* Phase 8 je Bild: FUN_8010024c @0x8010024c-0x801005a8, auf der eigenen Weltmatrix der Teilschicht. Die Teilschicht uebernimmt die
 * Matrix beim ersten Zeichnen nach dem Setzen von 0x40 (re15_teil_stufe); bis dahin wartet der Teil (ein Bild, wie die Zombie-Teile). */
static void zerplatzen_takt(const re15_actor_t *e)
{
    re15_teil_t *T = re15_teile(s_fi.form);
    if (!T) return;
    for (int i = 0; i < RE15_FI_TEILE_FLUG; i++) {
        const int k = k_flug[i].form;
        if (k >= RE15_TEILE_MAX || !T[k].fest) continue;
        fi_flug_t *f = &s_fi.fl[i];
        if (!f->eu_da) {                                                  /* FUN_80100108 @0x801001f8-218 */
            euler_aus(T[k].rot, f->eu);
            f->eu[2] = (int16_t)(f->eu[2] & 0xfff);
            f->seite = (int16_t)((uint16_t)(f->eu[2] - 0x400) < 0x800u);
            f->eu_da = 1;
        }
        /* Fall: c++, vy += 98c^2/20 - 98(c-1)^2/20 (`*0x66666667 >> 3`, je auf 0 gerundet) @0x8010028c-0x8010032c */
        const int32_t c = (int16_t)(f->c + 1);
        f->c = (int16_t)c;
        const int32_t q1 = (c * (c * 98)) / 20, q0 = ((c - 1) * ((c - 1) * 98)) / 20;
        f->vy = (int16_t)(f->vy + (q1 - q0));
        /* Boden: Teil-y (+0x60) >= 0 (`bltz` @0x80100358) -> v /= 20, vy = -vy/28 (0x92492493, >>4), y = 0, c = 0,
         * Dreh-a +-10 zur naechsten Lage (@0x80100360-0x80100408). Port: y relativ zum Boden der Form (e->y), wie die Zombie-Teile
         * (enemy_ai_re2_zombie.c PORT-MAPPING (1) BODEN). */
        if (T[k].t[1] - e->y >= 0) {
            f->v = (int16_t)(f->v / 20);
            f->vy = (int16_t)-(f->vy / 28);                              /* (sign) - ((hi(vy*0x92492493) + vy) >> 4) = -(vy/28) */
            T[k].t[1] = e->y;
            f->c = 0;
            const int16_t rz = (int16_t)(f->eu[2] & 0xfff);
            f->eu[2] = rz;
            if ((uint16_t)(rz - 0x400) < 0x800u) f->dreh_a = (rz < 0x801) ? -10 : 10;
            else f->dreh_a = (rz < 0x400) ? 10 : -10;
        }
        /* Drehung: z += Dreh, Dreh += Dreh-a (@0x8010040c-0x80100494) */
        f->eu[2] = (int16_t)((f->eu[2] + f->dreh) & 0xfff);
        f->dreh = (int16_t)(f->dreh + f->dreh_a);
        {
            const int16_t seite = (int16_t)((uint16_t)(f->eu[2] - 0x400) < 0x800u);
            if (f->dreh_a == 0) f->seite = seite;                         /* @0x80100494 */
            else if (seite != f->seite) f->dreh = (int16_t)-(f->dreh / 20);   /* (sign) - (hi(d*0x66666667) >> 3) @0x80100468-90 */
        }
        if (f->dreh < 10 && (uint16_t)(f->eu[2] - 0x3f7) < 0x13u) { f->dreh = 0; f->dreh_a = 0; }   /* @0x80100498-c8 */
        if (f->dreh < 10 && (uint16_t)(f->eu[2] - 0xbf7) < 0x13u) { f->dreh = 0; f->dreh_a = 0; }   /* @0x801004cc-fc */
        /* Weg: (v, ., 0) um Gier der Form + Eintrag gedreht (FUN_80077384 @0x80100500-20), Lage += (dx, vy, dz) (@0x80100524-60) */
        {
            const uint16_t g[3] = { 0, (uint16_t)(e->rot_y + f->gier), 0 };
            int16_t m[9];
            re15_door_rotmatrix(g, m);
            T[k].t[0] += (int16_t)(((int32_t)m[0] * f->v) >> 12);
            T[k].t[1] += f->vy;
            T[k].t[2] += (int16_t)(((int32_t)m[6] * f->v) >> 12);
        }
        /* RotMatrix(+0x68 -> +0x48) @0x8010055c */
        {
            const uint16_t r3[3] = { (uint16_t)f->eu[0], (uint16_t)f->eu[1], (uint16_t)f->eu[2] };
            int16_t m[9];
            re15_door_rotmatrix(r3, m);
            for (int q = 0; q < 9; q++) T[k].rot[q] = m[q];
        }
        f->v = (int16_t)(f->v + f->a);                                    /* @0x80100564-74 */
    }
}

/* Phase 8 Bild 16: Brocken weg (`sw zero` auf Teil 0/3/5/7/13/20 @0x80103d48-60), FUN_801005ac: 0x040E2000 an jedem Teil mit Marke,
 * a1 = (Zufall & 0xff) << 4, Versatz 0 (@0x8010060c-34). */
static void zerplatzen_bild16(void)
{
    re15_teil_t *T = re15_teile(s_fi.form);
    if (!T) return;
    for (int j = 0; j < FI_BROCKEN_N; j++)
        if (k_brocken[j].form < RE15_TEILE_MAX) T[k_brocken[j].form].w = 0;
    static const int16_t null4[4] = { 0, 0, 0, 0 };
    int n = 0;
    for (int i = 0; i < RE15_FI_TEILE_FLUG; i++) {
        const int k = k_flug[i].form;
        if (!k_flug[i].marke || k >= RE15_TEILE_MAX) continue;
        matrix_packen(s_fi.tmtx, T[k].rot, T[k].t);
        if (re2fx_spawn_sofort(FI_FUNKE, (int16_t)(((int)re15_engine_rand8() & 0xff) << 4), s_fi.tmtx, null4) >= 0) n++;
    }
    protokoll("Bild 16: Brocken weg, %d x 0x040E2000", n, 0, 0);
}

int re15_finale_r42_xa_aus(uint32_t vbl, int sektoren)
{
    return vbl * (uint32_t)RE15_FI_CD_SEKTOREN_S >= (uint32_t)sektoren * (uint32_t)RE15_FI_VBL_S;
}

int re15_finale_r42_teil(int i, int *re2, int *form, int *marke)
{
    if (i >= 0 && i < RE15_FI_TEILE_FLUG) {
        if (re2) *re2 = k_flug[i].re2;
        if (form) *form = k_flug[i].form;
        if (marke) *marke = k_flug[i].marke;
        return 1;
    }
    i -= RE15_FI_TEILE_FLUG;
    if (i >= 0 && i < FI_BROCKEN_N) {
        if (re2) *re2 = k_brocken[i].re2;
        if (form) *form = k_brocken[i].form;
        if (marke) *marke = 0;
        return 1;
    }
    return 0;
}

/* Messzeile Bildlage (Runde 3, Pruefbericht s1 r2 R1): Weltpunkt -> Bild 320x240 mit der Kamera des aktuellen Schnitts (RID-Satz wie
 * der Renderer, re15_camera_build_view = FUN_80076cb0-Nachbau, u = 160 + H * x / z). Rueckgabe 0 = hinter dem Auge. Nur Messung. */
static int bildlage(int32_t x, int32_t y, int32_t z, int *u, int *tiefe)
{
    const int cut = (int)g_scd.cam_id;
    if (!g_room_rdt_ok || !g_room_rdt.cuts || cut < 0 || cut >= g_room_rdt.cut_count) return 0;
    re15_camera_view_t v;
    if (re15_camera_build_view(&g_room_rdt.cuts[cut], &v) != 0) return 0;
    const int64_t vx = (((int64_t)v.rot[0] * x + (int64_t)v.rot[1] * y + (int64_t)v.rot[2] * z) >> 12) + v.trans[0];
    const int64_t vz = (((int64_t)v.rot[6] * x + (int64_t)v.rot[7] * y + (int64_t)v.rot[8] * z) >> 12) + v.trans[2];
    *tiefe = (int)vz;
    if (vz <= 0) return 0;
    *u = (int)(160 + (int64_t)v.fov_screen_dist * vx / vz);
    return 1;
}

/* Messzeile: Weltlage der Wiederholungsrakete (+0x34/+0x36/+0x38, re2_fx.c) und Abstand zur Form, je Bild bis zum Einfrieren. */
static void rakete_messen(const re15_actor_t *e)
{
    if (s_fi.w_platz < 0 || s_fi.w_platz >= RE2FX_PLAETZE || s_fi.t < 1 || s_fi.t > 16) return;
    const uint8_t *b = re2fx_platz(s_fi.w_platz);
    if (!b || !(b[0x18] | b[0x19])) return;
    const int32_t x = (int16_t)(b[0x34] | b[0x35] << 8), y = (int16_t)(b[0x36] | b[0x37] << 8), z = (int16_t)(b[0x38] | b[0x39] << 8);
    const int32_t dx = x - e->x, dz = z - e->z;
    protokoll("Rakete (%d,%d) Abstand zur Form %d", x, z, (int)re15_squareroot0((uint32_t)((int64_t)dx * dx + (int64_t)dz * dz)));
    int u = -1, t = 0, uf = -1, tf = 0;
    const int da = bildlage(x, y, z, &u, &t), df = bildlage(e->x, -1500, e->z, &uf, &tf);
    const int w = s_fi.phase == RE15_FI_P2 ? 1 : s_fi.phase == RE15_FI_P4 ? 2 : 3;
    protokoll("Bildlage Rakete W%d u %d Tiefe %d", w, da ? u : -9999, t);
    protokoll("Bildlage Form W%d u %d Tiefe %d", w, df ? uf : -9999, tf);
}

static void sequenz(uint16_t *pad)
{
    re15_actor_t *e = &g_actors[s_fi.form];
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    if (s_fi.phase == RE15_FI_P2 || s_fi.phase == RE15_FI_P4 || s_fi.phase == RE15_FI_P6) rakete_messen(e);
    if (s_fi.phase < RE15_FI_P7) form_halten(e);
    if (e->hp < RE15_FI_HP_UNTEN) e->hp = RE15_FI_HP_UNTEN;              /* die echte Rakete kann treffen; Untergrenze wie die Wurzel */

    switch (s_fi.phase) {
    case RE15_FI_P1:                                                      /* @0x80103704 */
        {   /* Messzeile: Zustand des Spielers um T und U (Phase im Low-Nibble, Rueckstoss Bit 4, Clip-Bild) */
            extern int re15_player_aim_phase_debug(void);
            const unsigned dt = s_fi.takt - s_fi.t_ausl, du = s_fi.t_frei ? s_fi.takt - s_fi.t_frei : 999u;
            if (dt <= 3u || du <= 8u)
                protokoll("Spieler: Zielphase 0x%02x, Bild %d, Form-HP %d", re15_player_aim_phase_debug(), (int)pl->anim_frame, (int)e->hp);
        }
        /* Tastenwort ERGAENZT, nicht ersetzt (Pruefbericht s1 r1): +0x15A == 0 -> `ori v1,v1,0x140` @0x80103740, sonst
         * `ori v0,v0,0x100` @0x80103724 (Runde 3: Adressen berichtigt, Pruefbericht s1 r2) */
        if (!s_fi.frei) { *pad = (uint16_t)(*pad | RE15_PAD_BIT_R1 | RE15_PAD_BIT_SQUARE); s_fi.z16a = 1; }   /* 0x140 @0x80103730-4c */
        else *pad = (uint16_t)(*pad | RE15_PAD_BIT_R1);                   /* 0x100 @0x80103714-2c */
        if (s_fi.z16a == RE15_FI_RAKETE_WEG) { const int n = re2fx_loesche_bank_sub(2, 0x0D, NULL); protokoll("echte Rakete geloescht (%d Plaetze)", n, 0, 0); }
        if (s_fi.z16a > 0) s_fi.z16a++;                                   /* @0x8010376c-80 */
        if (re15_game_flag_get(5, RE15_FI_SIG_FREI)) {                    /* +0x1D6 == 2 @0x80103784-b4 */
            re15_game_flag_set(5, RE15_FI_SIG_FREI, 0);
            s_fi.frei = 1;
            re15_game_flag_set(2, 0, 0);                                  /* Bank 2 &= 0x7fffffff: Spieler frei, schiesst im selben Bild */
            /* PORT-ZUORDNUNG (gemessen: finale.log Lauf lauf1, Pause-Wort 0x81000007 im Bild nach U): die Schlusszeile ist im Port ein
             * Message_on (K-11), und das Schliessen einer RE1.5-Nachricht stellt das beim Oeffnen gemerkte Pause-Wort wieder her
             * (FUN_80028134: `lw a0,DAT_800b853c` / `sw a0,0x800aca40` @0x800285a4 / @0x800286cc / @0x8002871c). Gemerkt wurde es in
             * T+16 mit gesetztem (2,0) - der Spieler fror nach der Freigabe wieder ein, die echte Rakete flog nie. RE2 hat diesen Weg
             * nicht (Xa_on + leere Meldung 7, Dossier B 5): die Freigabe gilt darum auch fuer die gemerkte Kopie. */
            g_re15_pauseflags_saved &= ~RE15_PAUSE_PLAYER;
            s_fi.t_frei = s_fi.takt;
            protokoll("Freigabe: echter Schuss (U = T+%d)", (int)(s_fi.t_frei - s_fi.t_ausl), 0, 0);
        }
        if (!re15_game_flag_get(5, RE15_FI_SIG_W1)) break;                /* +0x1D6 == 3 @0x801037b8-c0 */
        re15_game_flag_set(5, RE15_FI_SIG_W1, 0);
        re15_game_flag_set(2, 0, 1);                                      /* @0x801037c8-dc */
        pl->no_draw = 1;                                                  /* 0x800cfbf8 |= 0x80400 @0x801037e4-0c: unsichtbar, ohne Schatten */
        wiederholungs_schnitt(RE15_FI_W1_ABSTAND, RE15_FI_CUT_W1, "W1");
        s_fi.phase = RE15_FI_P2; s_fi.t = 0;                              /* @0x80103818-24, weiter im selben Bild */
        /* fallthrough */
    case RE15_FI_P2:                                                      /* @0x80103828 */
        if (s_fi.t == 1) wiederholung(FI_RAKETE, 0x80);
        if (s_fi.t == RE15_FI_W1_HALT) { re15_game_flag_set(2, 3, 1); protokoll("W1 eingefroren", 0, 0, 0); }
        if (++s_fi.t == RE15_FI_W1_DAUER) s_fi.phase = RE15_FI_P2 + 1;    /* Phase 3 im naechsten Bild (j 0x80103e5c) */
        break;
    case RE15_FI_P2 + 1:                                                  /* Phase 3 @0x801038c8 */
        loesche_rauch_und(0x80);
        wiederholungs_schnitt(RE15_FI_W2_ABSTAND, RE15_FI_CUT_W2, "W2");
        s_fi.phase = RE15_FI_P4; s_fi.t = 0;
        /* fallthrough */
    case RE15_FI_P4:                                                      /* @0x8010392c */
        if (s_fi.t == 1) wiederholung(FI_RAKETE, 0x81);
        if (s_fi.t == RE15_FI_W2_HALT) { re15_game_flag_set(2, 3, 1); protokoll("W2 eingefroren", 0, 0, 0); }
        if (++s_fi.t == RE15_FI_W2_DAUER) s_fi.phase = RE15_FI_P4 + 1;
        break;
    case RE15_FI_P4 + 1:                                                  /* Phase 5 @0x801039cc */
        loesche_rauch_und(0x81);
        wiederholungs_schnitt(RE15_FI_W3_ABSTAND, RE15_FI_CUT_W3, "W3");
        s_fi.phase = RE15_FI_P6; s_fi.t = 0;
        /* fallthrough */
    case RE15_FI_P6:                                                      /* @0x80103a30 */
        if (s_fi.t == 1) wiederholung(FI_WIED3, 0x82);                   /* Bank 0x27 nur mit R702.ESP (S1/U6 offen): ohne Bank -1 */
        if (++s_fi.t != RE15_FI_W3_DAUER) break;
        s_fi.phase = RE15_FI_P7;                                          /* @0x80103a9c-aac */
        re2fx_loesche_besitzer(0x82);
        re15_rumble_small(6, 3);                                          /* FUN_8003947c(6,3) @0x80103ab0-b8 */
        re15_rumble_ramp(60, 250, 0, 0);                                  /* FUN_800395b8(60,250,0,0) @0x80103abc-cc */
        protokoll("Ruetteln", 0, 0, 0);
        break;
    case RE15_FI_P7: {                                                    /* @0x80103ad8 Einschlag */
        re2fx_loesche_besitzer(0x82);                                     /* @0x80103ad8 */
        re2b_vsync_setzen(RE15_FI_VSYNC_KNALL);                           /* @0x80103b00-08 */
        const int16_t ofs[4] = { (int16_t)e->x, FI_KNALL_Y, (int16_t)e->z, 0 };   /* `lw v1,0x38 / sh` @0x80103b18-3c */
        const int k = re2fx_spawn_sofort(FI_KNALL, 0, re2fx_einheitsmatrix, ofs); /* FUN_8001bf10 @0x80103b38, a2 = 0x8009db44 */
        /* Se_on 0x010a0001 (ARMS11 Satz 0x0A leer = stumm, Dossier A 3.6) @0x80103b40-4c und 0x020f0001 (Raumbank R702 Satz 0x0F)
         * @0x80103b54-5c */
        if (s_se) s_se(RE15_FI_SE_EINSCHLAG);
        protokoll("Se_on 0x020F0001: Raumbank R702 Satz 0x%02X (PC-Haken %d)", RE15_FI_SE_EINSCHLAG, s_se != NULL, 0);
        for (int b = 0x90; b <= 0x9e; b++) re2fx_loesche_besitzer((uint8_t)b);   /* @0x80103b74-be8 */
        (void)re15_audio_re2_xa(ist_elza() ? RE15_FI_XA03_DATEI_ELZA : RE15_FI_XA03_DATEI,
                                ist_elza() ? RE15_FI_XA03_LESE_ELZA : RE15_FI_XA03_LESE);   /* Xa 0x03 @0x80103bec-f4 */
        s_fi.xa_vbl = 0;
        zerplatzen_start();                                               /* FUN_80100108 @0x80103c04, FUN_8010158c @0x80103c0c-50 (U5) */
        s_fi.getoetet = 1;                                                /* +0x10E |= 0x4000 @0x80103c58-68 */
        s_fi.phase = RE15_FI_P8; s_fi.t = 0;
        protokoll("Einschlag: VSync 5, Explosion Platz %d, Xa 0x03 (%d Sektoren)", k, RE15_FI_XA03_LESE, 0);
    }   /* fallthrough: Phase 8 im selben Bild (@0x80103c6c-70) */
        /* fallthrough */
    case RE15_FI_P8: {                                                    /* @0x80103c70 */
        zerplatzen_takt(e);                                               /* `jal 0x8010024c` @0x80103c70 */
        re2b_schuettel_takt(s_fi.t < RE15_FI_BROCKEN_BILD ? RE2B_SCHUETTEL_LEICHT : 0u);   /* Bank 1 |= 4 @0x80103b0c-10 */
        const int alt = s_fi.t++;
        if (alt < RE15_FI_NACHKNALL) {
            const int32_t rx = (int8_t)re15_engine_rand8(), ry = (int8_t)re15_engine_rand8(), rz = (int8_t)re15_engine_rand8();
            const int16_t ofs[4] = { (int16_t)(e->x + rx * FI_NACH_R), (int16_t)(ry * FI_NACH_R + FI_NACH_Y),
                                     (int16_t)(e->z + rz * FI_NACH_R), 0 };      /* @0x80103c98-d1c */
            (void)re2fx_spawn_sofort(FI_KNALL, 0, re2fx_einheitsmatrix, ofs);
        }
        if (s_fi.t == RE15_FI_BROCKEN_BILD) {
            re2b_schuettel_takt(0);                                       /* Bank 1 &= ~4 @0x80103d30-44 */
            protokoll("Wackeln aus (Bild 16)", 0, 0, 0);
            zerplatzen_bild16();                                          /* @0x80103d48-60, `jal 0x801005ac` @0x80103d5c */
        }
        /* XA-Bit @0x80103d64-74 (Kopf RE15_FI_XA03_LESE): Echtzeit. Im Startbild steht das Bit noch (alt == 0); jedes weitere Bild
         * hat VSync-Wert Austastungen gedauert (VSync(DAT_800dfc1a) @0x8002b990-98). Aus, sobald vbl / 60 >= Sektoren / 150. */
        if (alt > 0) s_fi.xa_vbl += (uint32_t)re2b_vsync();
        if (!re15_finale_r42_xa_aus(s_fi.xa_vbl, ist_elza() ? RE15_FI_XA03_LESE_ELZA : RE15_FI_XA03_LESE)) break;
        kill();
        s_fi.phase = RE15_FI_P9;
        break;
    }
    case RE15_FI_P9:                                                      /* @0x80103dc8: RE2 wartet auf sub16 (+0x1D6 = 4); Port: im
                                                                           * Bild nach dem Kill, 5090 sub02 uebernimmt (PORT-WAHL) */
        re2b_vsync_setzen(RE2B_VSYNC_SPIEL);                              /* @0x80103dd4-dc */
        re2b_schuettel_takt(0);
        kamera(s_fi.cut);                                                 /* FUN_8002c7ac(0x80104d60) @0x80103e04 */
        g_scd.cut_auto_enabled = 1;
        re15_game_flag_set(2, 0, 0); re15_game_flag_set(2, 7, 0);        /* Bank 2 &= 0x7fffffff & 0xfeffffff @0x80103e0c-50 */
        pl->no_draw = 0;                                                  /* 0x800cfbf8 &= 0xfff7fbff @0x80103e1c-58 */
        s_fi.phase = RE15_FI_ENDE;
        protokoll("Phase 9: Sperren aus, VSync 2", 0, 0, 0);
        break;
    default: break;
    }
}

int re15_finale_r42_vor_spielzug(uint16_t *pad, uint16_t *pressed)
{
    if (!re15_finale_r42_raum()) return 0;
    s_fi.takt++;
    re15_finale_wurf_takt();                                            /* Stufe 3: Signale, Flug, Aufnahme-Messzeile */
    if (s_fi.phase == RE15_FI_AUS) {
        if (!ausloeser(pad, pressed)) return 0;
        phase0();
        return 1;                                                         /* das verbrauchte Feuer gilt in diesem Bild */
    }
    if (s_fi.phase >= RE15_FI_ENDE) return 0;
    if (s_fi.form < 0 || s_fi.form >= RE15_ACTOR_MAX) { s_fi.phase = RE15_FI_ENDE; return 0; }
    uint16_t p = *pad;
    sequenz(&p);
    /* Feuer-Flanke: der Port schiesst auf `pressed`; RE2 liest das Tastenwort 0x800ce30c (@0x80103724) - die Flanke wird darum mit dem
     * Quadrat-Bit ergaenzt, die uebrigen Flanken bleiben (die Pad-Sperre (2,7) haelt Menues zu). */
    if (s_fi.phase == RE15_FI_P1) { *pad = p; if (pressed) *pressed = (uint16_t)(*pressed | (p & RE15_PAD_BIT_SQUARE)); return 1; }
    return 0;
}
