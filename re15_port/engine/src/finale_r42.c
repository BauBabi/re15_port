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

#include "gen/finale_r42_texte.inc"   /* k_fi_5090_msg02, k_fi_5091_msg02 (tools/r42_finale/texte_finale.py) */
#include "re15_room.h"                 /* g_current_room_id */

int re15_player_gunbone_matrix(int32_t rot[9], int32_t t[3]);   /* re15_damage.c */

#define FI_RAKETE   0x020D1000u   /* Wiederholung 1/2: `lui a0,0x20d / ori a0,a0,0x1000` @0x8010384c-50 / @0x80103950-54 */
#define FI_WIED3    0x270A1000u   /* Wiederholung 3: `lui a0,0x270a / ori 0x1000` @0x80103a54-58 (Bank 0x27 = Raum-ESP R702) */
#define FI_KNALL    0x031F5000u   /* Einschlag + Nachknalle: `lui a0,0x31f / ori a0,a0,0x5000` @0x80103ae0-e4 / @0x80103ce4-e8 */
#define FI_KNALL_Y  (-1800)       /* `addiu v0,zero,-0x708 / sh v0,0x12(sp)` @0x80103b24-2c */
#define FI_NACH_Y   (-2500)       /* `addiu v1,v1,-0x9c4` @0x80103cd8 */
#define FI_NACH_R      20         /* r * 5 * 4: `sll v1,v0,2 / addu v1,v1,v0 / sll v1,v1,2` @0x80103ca8-b4 */

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
    int      xa_rest;         /* Phase 8: Bilder bis zum Ende von Xa 0x03 */
    int32_t  pl_x, pl_z; int16_t pl_gier; int cut;   /* gemerkt @0x8010369c-cc (0x80104d54/58/5c/60) */
    int32_t  fo_x, fo_z;                             /* +0x230/+0x232 @0x80103610-14 */
    unsigned takt;            /* Spieltakte seit Raumstart (Messschiene) */
    unsigned t_ausl, t_frei;  /* T und U (Messschiene) */
    uint8_t  mtx[32];         /* Quell-Matrix der Wiederholungen (Platz +0x6C) */
} s_fi = { .form = -1, .uhr = -1 };

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

/* ---- Raum ------------------------------------------------------------------------------------------------------------- */
int re15_finale_r42_raum(void) { return re15_bf_bahnsteig_aktiv(); }

void re15_finale_r42_reset(void)
{
    const uint16_t r = s_fi.raum;
    memset(&s_fi, 0, sizeof s_fi);
    s_fi.raum = r; s_fi.form = -1; s_fi.uhr = -1;
}

void re15_finale_r42_install(uint16_t room_id)
{
    /* Selbst-Tueren (Wagenwechsel, scd_room_reenter) bauen denselben Raum neu auf: Zustand behalten. */
    if (s_fi.raum != room_id) { s_fi.raum = room_id; re15_finale_r42_reset(); }
    if (room_id == 0x5090u) {
        re15_msg_install_text(RE15_FI_MSG_ZEILE_LEON, k_fi_5090_msg02, sizeof k_fi_5090_msg02);
        const int d = re15_msg_compute_duration(k_fi_5090_msg02, sizeof k_fi_5090_msg02, 0);
        if (d > 0 && d < 65535) re15_msg_install_durations(RE15_FI_MSG_ZEILE_LEON, d);
    }
    if (room_id == 0x5091u) {
        re15_msg_install_text(RE15_FI_MSG_ZEILE_ELZA, k_fi_5091_msg02, sizeof k_fi_5091_msg02);
        const int d = re15_msg_compute_duration(k_fi_5091_msg02, sizeof k_fi_5091_msg02, 0);
        if (d > 0 && d < 65535) re15_msg_install_durations(RE15_FI_MSG_ZEILE_ELZA, d);
    }
}

int re15_finale_r42_ende(void) { return re15_game_flag_get(RE15_R42_BANK, RE15_R42_BIT_FINALE_ENDE) != 0; }
int re15_finale_r42_phase(void) { return s_fi.phase; }
int re15_finale_r42_genug(void) { return s_fi.genug; }
int re15_finale_r42_passend(void) { return s_fi.passend; }
int re15_finale_r42_uhr(void) { return s_fi.uhr; }
int re15_finale_r42_wurf_faellig(void) { return s_fi.wurf; }
int re15_finale_r42_besitzt(int slot) { return s_fi.phase != RE15_FI_AUS && s_fi.phase < RE15_FI_ENDE && slot == s_fi.form; }
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
    if (s_fi.uhr < 0 && kampf_laeuft(slot)) { s_fi.uhr = RE15_FI_UHR; protokoll("Kampf laeuft, Uhr %d Bilder", RE15_FI_UHR, 0, 0); }
    else if (s_fi.uhr > 0 && --s_fi.uhr == 0) { s_fi.uhr_ab = 1; protokoll("Uhr abgelaufen (Set(5,0x0F))", 0, 0, 0); }
    /* sub11 @0x017DE-0x01818, von sub01 @0x0134A je Bild gerufen solange Ck(5,0x0D)=0:
     *   if (+0x1D8 == 1 && +0x1DA == 1) or (Ck(5,0x0F) == 1 && +0x1DA == 1)  ->  Set(5,0x0D,1), Evt_exec sub12 (Wurf). */
    if (!s_fi.wurf && s_fi.uhr >= 0 && s_fi.phase == RE15_FI_AUS && s_fi.passend && (s_fi.genug || s_fi.uhr_ab)) {
        s_fi.wurf = 1;
        protokoll("Wurf faellig (genug %d, Uhr ab %d, Abstand %d) - Wurfszene folgt in Stufe 3", s_fi.genug, s_fi.uhr_ab, dist);
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

/* Quell-Matrix der Wiederholung. RE2: Teil 11 des Spielers (+0x7AC, `addiu a2,s0,0x7ac` @0x80103858) mit Versatz (600,0,400)
 * (@0x80103828-3c). PORT-ZUORDNUNG (offen, Messung im RE2-Savestate): der Port hat den Waffenknochen der echten Schuesse
 * (werfer_r35.c mtx_bauen fuer Id 18, ohne Rahmen) - die Wiederholung startet an derselben Muendung wie der echte Schuss
 * ({0,1100,0}, Raketen-Handler @0x800455a8-d0). */
static int quelle_bauen(void)
{
    int32_t r[9], t[3];
    if (!re15_player_gunbone_matrix(r, t)) return 0;
    for (int k = 0; k < 9; k++) {
        int32_t v = r[k];
        if (v > 32767) v = 32767; else if (v < -32768) v = -32768;
        s_fi.mtx[2 * k] = (uint8_t)v; s_fi.mtx[2 * k + 1] = (uint8_t)(v >> 8);
    }
    s_fi.mtx[18] = 0; s_fi.mtx[19] = 0;
    for (int k = 0; k < 3; k++) {
        s_fi.mtx[20 + 4 * k] = (uint8_t)t[k]; s_fi.mtx[21 + 4 * k] = (uint8_t)(t[k] >> 8);
        s_fi.mtx[22 + 4 * k] = (uint8_t)(t[k] >> 16); s_fi.mtx[23 + 4 * k] = (uint8_t)(t[k] >> 24);
    }
    return 1;
}

static void wiederholung(uint32_t a0, uint8_t besitzer)
{
    static const int16_t OM[4] = { 0, 1100, 0, 0 };   /* PORT-ZUORDNUNG s. quelle_bauen */
    const re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    int i = -1;
    if (quelle_bauen()) {
        re2fx_boden_basis_setzen(pl->y);
        i = re2fx_spawn_besitzer(a0, besitzer, (int16_t)pl->rot_y, s_fi.mtx, OM);   /* FUN_8001c8c4 @0x8010385c / @0x80103960 / @0x80103a60 */
        if (i >= 0 && i < RE2FX_PLAETZE) re2fx_r35_schuetze(i, pl->x, pl->z);
    }
    re15_audio_re2_arms_se(0x11, 0);                  /* Se_on 0x01000001 @0x80103864-74 (ARMS11 Satz 0, wie werfer_r35.c) */
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
    0x09, 0x0a, LE16(RE15_FI_SCHLAF_ZEILE),       /* @0x01A30 `09 0a 0f 00` */
    0x2b, RE15_FI_MSG_ZEILE_LEON, 0x00, 0x00,     /* Xa_on(0,2) @0x01A3C + Message_on 7 @0x01A40 -> K-11: Message_on mit Stimme
                                                   * (synchro main02.wav), Untertitel-Maske 0 wie ROOM6030 sub02 @0x00DC2 */
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

/* Phase 0 @0x801035c0-0x80103700 */
static void phase0(void)
{
    re15_actor_t *e = &g_actors[s_fi.form];
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    /* Lagen: RE2 stellt Gegner (-14547,-14636) Gier 1432 und Leon (-19025,200,-20861) Gier 4433 fest (@0x801035cc-e0 /
     * @0x80103638-74); Leon steht damit 7668 vor dem Gegner, Gegnergier = Peilung zu Leon (1432 gegen gerechnet 1430).
     * PORT-ZUORDNUNG bis U3 (Fixpunkte im Wagen C): beide bleiben stehen; die Form schaut den Spieler an wie dort. */
    s_fi.fo_x = e->x; s_fi.fo_z = e->z;                                   /* +0x230/+0x232 @0x80103610-14 */
    e->rot_y = (int16_t)peilung(e->x, e->z, pl->x, pl->z);
    form_halten(e);                                                       /* +0x14C = 0x0A05 @0x801035e8-ec (D11) */
    s_fi.pl_x = pl->x; s_fi.pl_z = pl->z; s_fi.pl_gier = pl->rot_y;       /* @0x801036a4-b0 */
    s_fi.cut = (int)g_scd.work_vars[0x0A];                                /* @0x8010369c / @0x801036cc */
    re15_game_flag_set(2, 0, 1);                                          /* Bank 2 |= 0x80000000 @0x80103678-8c (Spieler) */
    re15_game_flag_set(2, 7, 1);                                          /*          |= 0x01000000 (Pad) */
    re15_game_flag_set(1, 27, 1);                                         /* Bank 1 |= 0x10 @0x801036b8-c4 (Kinobalken) */
    g_scd.cut_auto_enabled = 0;                                           /* Bank 1 |= 0x100 @0x801036bc-d4 (1,0x17): Zonenwechsel
                                                                           * aus (PORT-ZUORDNUNG wie bf_kamera) */
    /* Schnitt 11 (FUN_8002c7ac(11) @0x801036d8): "Game over"-Schnitt = Rolle RE2 11 (Spieler im Bild), U3 offen ->
     * PORT-ZUORDNUNG: der Schnitt, der den Spieler gerade zeigt, bleibt. */
    kamera(s_fi.cut);
    s_fi.phase = RE15_FI_P1; s_fi.t = 0; s_fi.z16a = 0; s_fi.frei = 0;   /* +0x1D6 = 1, +0x158 = 0, Phase 1 @0x801036f4-700 */
    const int slot = programm_starten();                                  /* RE2: +0x1D6 = 1 -> sub01/sub14 -> sub15 */
    s_fi.t_ausl = s_fi.takt;
    protokoll("Ausloeser: Abschuss (Form hp %d), Programm Slot %d", e->hp, slot, 0);
}

static void loesche_rauch_und(uint8_t besitzer)
{
    re15_game_flag_set(2, 3, 0);                                          /* Bank 2 &= 0xefffffff @0x801038c8-f0 / @0x801039cc-f4 */
    re2fx_loesche_bank_sub(3, 0x0A, NULL);                                /* FUN_8001cefc(3,10,0) @0x801038ec / @0x801039f0 */
    re2fx_loesche_besitzer(besitzer);                                     /* FUN_8001cfbc @0x801038f4 / @0x801039f8 */
}

static void wiederholungs_schnitt(int32_t abstand, const char *name)
{
    re15_actor_t *e = &g_actors[s_fi.form];
    spieler_vor(e, abstand);
    /* Schnitt 8/9/10 @0x801037dc / @0x801038fc / @0x80103a00: die RE2-Schnitte zeigen den Bahnsteig (ROOM702.BSS), Plan D6 ->
     * PORT-ZUORDNUNG bis U3: der Schnitt bleibt der des Ausloesers (Rollenwahl je Wiederholung ist die Messung U3). */
    kamera(s_fi.cut);
    protokoll(name[1] == '1' ? "W1: Spieler %d vor der Form" : name[1] == '2' ? "W2: Spieler %d vor der Form"
                                                               : "W3: Spieler %d vor der Form", abstand, 0, 0);
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
    e->active = 0; e->no_draw = 1;                                        /* PORT-WAHL: der Gegner ist fort (RE2: Teile + Flag) */
    protokoll("Kill: (5,0x1C) + Bit 129, Spieler zurueck", 0, 0, 0);
}

static void sequenz(uint16_t *pad)
{
    re15_actor_t *e = &g_actors[s_fi.form];
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
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
        if (!s_fi.frei) { *pad = (uint16_t)(RE15_PAD_BIT_R1 | RE15_PAD_BIT_SQUARE); s_fi.z16a = 1; }   /* 0x140 @0x80103730-4c */
        else *pad = RE15_PAD_BIT_R1;                                      /* 0x100 @0x80103714-2c */
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
        wiederholungs_schnitt(RE15_FI_W1_ABSTAND, "W1");
        s_fi.phase = RE15_FI_P2; s_fi.t = 0;                              /* @0x80103818-24, weiter im selben Bild */
        /* fallthrough */
    case RE15_FI_P2:                                                      /* @0x80103828 */
        if (s_fi.t == 1) wiederholung(FI_RAKETE, 0x80);
        if (s_fi.t == RE15_FI_W1_HALT) { re15_game_flag_set(2, 3, 1); protokoll("W1 eingefroren", 0, 0, 0); }
        if (++s_fi.t == RE15_FI_W1_DAUER) s_fi.phase = RE15_FI_P2 + 1;    /* Phase 3 im naechsten Bild (j 0x80103e5c) */
        break;
    case RE15_FI_P2 + 1:                                                  /* Phase 3 @0x801038c8 */
        loesche_rauch_und(0x80);
        wiederholungs_schnitt(RE15_FI_W2_ABSTAND, "W2");
        s_fi.phase = RE15_FI_P4; s_fi.t = 0;
        /* fallthrough */
    case RE15_FI_P4:                                                      /* @0x8010392c */
        if (s_fi.t == 1) wiederholung(FI_RAKETE, 0x81);
        if (s_fi.t == RE15_FI_W2_HALT) { re15_game_flag_set(2, 3, 1); protokoll("W2 eingefroren", 0, 0, 0); }
        if (++s_fi.t == RE15_FI_W2_DAUER) s_fi.phase = RE15_FI_P4 + 1;
        break;
    case RE15_FI_P4 + 1:                                                  /* Phase 5 @0x801039cc */
        loesche_rauch_und(0x81);
        wiederholungs_schnitt(RE15_FI_W3_ABSTAND, "W3");
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
        /* Se_on 0x010a0001 (ARMS11 Satz 0x0A leer = stumm, Dossier A 3.6) und 0x020f0001 (Raumbank Satz 0x0F, R702.EDT: S1 offen)
         * @0x80103b40-60 */
        for (int b = 0x90; b <= 0x9e; b++) re2fx_loesche_besitzer((uint8_t)b);   /* @0x80103b74-be8 */
        (void)re15_audio_re2_xa(ist_elza() ? RE15_FI_XA03_DATEI_ELZA : RE15_FI_XA03_DATEI,
                                ist_elza() ? RE15_FI_XA03_LESE_ELZA : RE15_FI_XA03_LESE);   /* Xa 0x03 @0x80103bec-f4 */
        s_fi.xa_rest = ist_elza() ? RE15_FI_XA03_BILDER_ELZA : RE15_FI_XA03_BILDER;
        /* Koerper zerplatzt (FUN_80100108 @0x80103c04, Brocken FUN_8010158c Teile 0/3/5/7/13/20 @0x80103c0c-50): U5 offen.
         * PLATZHALTER bis U5: die Form wird im Einschlagbild unsichtbar (die Explosion deckt die Stelle). */
        e->no_draw = 1;
        s_fi.getoetet = 1;                                                /* +0x10E |= 0x4000 @0x80103c58-68 */
        s_fi.phase = RE15_FI_P8; s_fi.t = 0;
        protokoll("Einschlag: VSync 5, Explosion Platz %d, Xa 0x03 (%d Sektoren)", k, RE15_FI_XA03_LESE, 0);
    }   /* fallthrough: Phase 8 im selben Bild (@0x80103c6c-70) */
        /* fallthrough */
    case RE15_FI_P8: {                                                    /* @0x80103c70 */
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
        }
        /* XA-Bit @0x80103d64-74: im Startbild (Phase 7/8 desselben Bilds) steht es noch; nach n Bildern ist es aus, sobald
         * n / 30 >= 6,84 s -> n = 206 = RE15_FI_XA03_BILDER. */
        if (s_fi.xa_rest-- > 0) break;
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
    if (s_fi.phase == RE15_FI_AUS) {
        if (!ausloeser(pad, pressed)) return 0;
        phase0();
        return 1;                                                         /* das verbrauchte Feuer gilt in diesem Bild */
    }
    if (s_fi.phase >= RE15_FI_ENDE) return 0;
    if (s_fi.form < 0 || s_fi.form >= RE15_ACTOR_MAX) { s_fi.phase = RE15_FI_ENDE; return 0; }
    uint16_t p = *pad;
    sequenz(&p);
    if (s_fi.phase == RE15_FI_P1) { *pad = p; if (pressed) *pressed = (uint16_t)(p & RE15_PAD_BIT_SQUARE); return 1; }
    return 0;
}
