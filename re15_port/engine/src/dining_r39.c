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
#include "re15_damage.h"           /* re15_player_equipped_weapon / _set_equipped_weapon (aca5d), re15_ai_arc_test */
#include "re15_elza_r36.h"         /* re15_elza_se_hook / RE15_ELZA_TON_SCHUSS (Johns Schuss-Ton, Runde 36 L / 38 H) */
#include "re15_emd.h"              /* re15_emd_parse_rbj_record (Heranwinken-Leihe ROOM1171) */
#include "re15_enemy.h"            /* re15_enemy_find (Johns Skelett als Basis-Hierarchie) */
#include "re15_wclip_r41.h"         /* Runde 41 Spur J: Spieler-Plc_motion(2,c) = W-Bank in jedem Raum */

#include "gen/dining_r39_programme.inc"

void (*re15_dining_r39_se_hook)(int satz) = NULL;
unsigned g_re15_r39j_se_zaehler[3] = { 0, 0, 0 };
int      g_re15_r39j_hilfe_letzte  = 0;

/* ============================ Programme und Texte ============================ */
const uint8_t *re15_dining_r39_haemmern(void)   { return k_r39j_p_haemmern; }
const uint8_t *re15_dining_r39_entriegeln(void) { return k_r39j_p_entriegeln; }
const uint8_t *re15_dining_r39_rolltor(void)    { return k_r39j_p_rolltor; }

/* SCHUETZE39 liegt beschreibbar: der Anker ZIEL traegt vor dem Plc_dest die Lage des naechsten Gorillas ein. */
static uint8_t s_schuetze[sizeof k_r39j_p_schuetze];
int      g_re15_r39j_anker_n = 0;
unsigned g_re15_r39j_schuss_zaehler = 0, g_re15_r39j_treffer_zaehler = 0;
const uint8_t *re15_dining_r39_schuetze(void) { return s_schuetze; }

const uint8_t *re15_dining_r39_programm(int nr, int *out_len)
{
    static const struct { const uint8_t *b; int n; } p[4] = {
        { k_r39j_p_haemmern,   (int)sizeof k_r39j_p_haemmern   },
        { k_r39j_p_entriegeln, (int)sizeof k_r39j_p_entriegeln },
        { k_r39j_p_rolltor,    (int)sizeof k_r39j_p_rolltor    },
        { k_r39j_p_schuetze,   (int)sizeof k_r39j_p_schuetze   },
    };
    if (nr < 0 || nr >= 4) { if (out_len) *out_len = 0; return NULL; }
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
static int s_w_ton = 0;   /* Messer-Zieh-Ton dieser Hebung schon gespielt */

void re15_dining_r39_w_beginn(int entity, int clip)
{
    re15_wclip_r41_beginn((g_current_room_id & 0xFFFFu) != RE15_R38F_RAUM_1141 ? entity : -1, clip);   /* Runde 41 Spur J: (2,c) = W-Bank-Clip in jedem Raum */
    if ((g_current_room_id & 0xFFFFu) != RE15_R38F_RAUM_1141 || entity != RE15_R39J_W_SUB || clip < 0) { s_w_clip = -1; return; }
    s_w_clip = clip;
    s_w_ton = 0;
#ifdef RE15_PLATFORM_PC
    fprintf(stderr, "[dining39] F%u Elza Plc_motion(2,%d) -> W-Bank-Clip %d (Sub 0x80073e30[2] = 0x80050f00, Waffe %d)\n",
            (unsigned)g_engine.frame_count, clip, clip, re15_player_equipped_weapon());
#endif
}

/* ============================ Nachbesserung 3 (Abnahme 2 M1): Waffe senken unter der Pad-Sperre ============================
 * Befund (Abnahme 2 Lauf R/R2): zielte Elza noch, als ENTRIEGELN39 nach dem Kampf auf Cut 8 schnitt, stand sie dort nach
 * Pos_set + Member_set(4) mit Blick zu John 11 Bilder lang mit erhobener Waffe (state.txt F6296..F6306 pst=1 mo=215), bis
 * Plc_dest TURN das Kommandowort ersetzte. Ursache im Port: im Skript-Modus (player_mode 2, main.c cine_active = (1,27) oder
 * (2,7)) ueberspringt re15_player_tick den ganzen Pad-Zweig samt Ziel-FSM - die Waffe bleibt einfach oben.
 * Original (selbst disassembliert, info/Re1.5/PSX.EXE): Set(2,7)=1 setzt Bit 0x01000000 des Pausen-Worts 0x800aca40
 * (Set-Tabelle @0x80074664[2], game_state.c). Der Pad-Umsetzer FUN_80030444 (Aufrufe im Task-Takt @0x80029dd8/@0x80029e14,
 * VOR der Spiel-Task) kappt dann das virtuelle Held-Wort: `lw v0,-13760(v0)` @0x800304f4, `lui v1,0x100` @0x800304f8,
 * `andi v0,v0,0xf000` @0x80030514, `sw v0,-14488(at)` @0x8003051c (0x800ac768). Der Spieler-Dispatcher haelt nur bei Bit 31
 * (`bltz a0` @0x80031c78) und laeuft also weiter; die Ziel-FSM liest genau dieses Wort und senkt ohne Bit 0x100:
 *   Schusswaffe HOLD  `lw v1,-14488(v1)` @0x800331e4 / `andi v0,v1,0x100` @0x800331ec / `ori v0,zero,0x3` @0x800331f8 /
 *                     `sh v0,-13734(at)` @0x80033200 (aca5a = 3 = SENKEN)
 *   Messer HOLD       @0x80035128 / @0x80035130 / @0x8003513c / @0x80035144
 *   Rueckstoss        `lw a0,-14488(a0)` @0x80033560 / `andi v0,a0,0x100` @0x80033600, ab Bild > Byte 2 der Waffenzeile
 *                     @0x80074092 + (w-1)*5 (@0x8003362c) -> @0x8003364c
 * Wer beim Szenenbeginn zielt, senkt also die Waffe (Clip 6 rueckwaerts) und steht danach neutral; ENTRIEGELN39 setzt die
 * Sperre darum wie ROOM11C0 sub03 (@0x01B06/@0x01B0A, Sleep 30 @0x01B14) 30 Bilder VOR dem Schnitt @0x01B18.
 * Port: re15_player_tick laesst die Ziel-FSM in ROOM1141 Phase H unter der Sperre mit gekapptem Pad laufen (Haken
 * player_common.c). Der Sperr-Stand wird hier am Ende des Spielschritts gemerkt (re15_dining_r39_tick laeuft nach dem
 * Spieler-Takt) und gilt im naechsten Bild - wie im Original, wo der Umsetzer vor der Spiel-Task laeuft und das Skript das Bit
 * in der Spiel-Task setzt. Nur ROOM1141 Phase H (die anderen Raeume behalten den bisherigen Port-Stand, Dossier OFFEN). */
static int s_phase_h = 0;
static int s_sperre_gesehen = 0;

int re15_dining_r39_ziel_senken(void)
{
    return s_phase_h && (g_current_room_id & 0xFFFFu) == RE15_R38F_RAUM_1141 && s_sperre_gesehen;
}

/* ============================ N4.3: John schiesst (Faden SCHUETZE39) ============================
 * Original-Form: ROOM1021 sub02 @0x020C6 Plc_motion(2,6) Zielen, @0x020CA Sleep 110, @0x020CE/@0x020D6 Plc_motion(2,7)
 * Feuer-Clip (eigene Bank Clip 7) im Abstand Sleep 30 (@0x020D2), @0x020EA/@0x020EE (2,6) rueckwaerts Senken. Der Ton
 * kommt ein Bild nach dem Feuer-Clip (Verzug 1 wie elza_r36.c P4, RE2 ROOM1010 sub30 @0x02676/sub31 @0x0268E) aus
 * RE15_ELZA_TON_SCHUSS (RE2 ROOM4040 Satz 0x0A = Johns Schuss in 1021, Runde 38 H). Treffer = Schaden der Handfeuerwaffe
 * auf den Gorilla, mit den Schreibern des Schuss-Treffers (re15_damage.c re15_player_weapon_fire: +0x9a -= dmg, +0x93 |= 1,
 * +0x7 = 0 @0x80012428, +0x4 = 2/3 @0x80012520, +0x5 = Waffen-Id @0x800124bc); danach kriecht der Gorilla eine Weile auf John
 * zu (Aufmerksamkeit, Lenkziel im Gorilla-B[3] statt Elza). */
static int     s_ziel_slot = -1;
static int     s_schuss_offen = 0;
static int     s_zielen = 0;                      /* Nachbesserung 2 (b): Anker ZIELEN..SENKEN = John fuehrt die Waffe nach */
static int16_t s_aufmerksam[RE15_ACTOR_MAX];
static int32_t s_auf_abstand[RE15_ACTOR_MAX];   /* Abstand zu John am letzten Prueftakt (Fortschritt der Jagd) */
static uint8_t s_zugewandt[RE15_ACTOR_MAX];     /* Nachbesserung 2 (c): der Gorilla hat sich John zugewandt (Kegel ZUGEWANDT) */
static uint8_t s_auf_takt[RE15_ACTOR_MAX];      /* Bilder seit der Zuwendung bzw. seit dem letzten Prueftakt */
extern void re15_enemy_steer_point(re15_actor_t *e, int32_t tx, int32_t tz, int slew);   /* enemy_ai_common.c = FUN_8001a8f8 */

static re15_actor_t *john_aktor(void)
{
    for (int s = 1; s < RE15_ACTOR_MAX; s++)
        if (g_actors[s].active && g_actors[s].type == RE15_R38F_JOHN_TYP) return &g_actors[s];
    return NULL;
}

static int gorilla_lebt(const re15_actor_t *g)
{
    return g->active && g->type == RE15_R38F_GORILLA_TYP && g->hp >= 0 && g->state != 3;
}

/* Ein Gorilla mitten im Angriff (Zustand 1, Sub 5 Biss / 6 Wucht / 7 Sprung / 9 / 15 Aufbaeumen-Griff mit laufender
 * Phase +0x6 != 0) oder in der Luft (+0x1e0) wird weder getroffen noch umgelenkt: das Original kennt keinen Dritten, der
 * einen Angriff auf den Spieler unterbricht (Griff/Opferzustand des Spielers, re15_player_victim_*). */
static int gorilla_im_angriff(const re15_actor_t *g)
{
    if (g->mag_airborne) return 1;
    if (g->state != 1 || g->sub_state_2 == 0) return 0;
    return g->sub_state_1 == 5 || g->sub_state_1 == 6 || g->sub_state_1 == 7 || g->sub_state_1 == 9 || g->sub_state_1 == 15;
}

/* Nachbesserung 2 (a): Johns Ziel = der naechste lebende Gorilla, der NICHT gerade angreift - haelt einer Elza (Sub 15) oder
 * beisst/springt er, nimmt John den anderen. Greifen alle an, bleibt der naechste lebende (der Schuss geht dann daneben,
 * s.o.: einen Angriff auf den Spieler unterbricht im Original kein Dritter). Abstand wie bisher |dx|+|dz|. Die Wahl ist
 * die Form des Nachzielens der Pistole (Gun-FSM Sub 5 @0x80033eec: Zielsuche FUN_80037250(0x7530) beim Eintritt, dann
 * Nachdrehen @0x80033f94) - John waehlt neu, wenn sein Ziel tot ist oder angreift. */
static int ziel_waehlen(const re15_actor_t *j)
{
    int beste = -1, frei = -1;
    long d_beste = 0x7fffffffL, d_frei = 0x7fffffffL;
    for (int s = 1; j && s < RE15_ACTOR_MAX; s++) {
        const re15_actor_t *g = &g_actors[s];
        if (!gorilla_lebt(g)) continue;
        const long d = labs((long)g->x - j->x) + labs((long)g->z - j->z);
        if (d < d_beste) { d_beste = d; beste = s; }
        if (!gorilla_im_angriff(g) && d < d_frei) { d_frei = d; frei = s; }
    }
    return frei > 0 ? frei : beste;
}

static int ziel_gilt(int slot)
{
    return slot > 0 && slot < RE15_ACTOR_MAX && gorilla_lebt(&g_actors[slot]) && !gorilla_im_angriff(&g_actors[slot]);
}

/* ============================ Heranwinken aus ROOM1171 (M2/N4.4, Nutzer-Nachtrag 7) ============================ */
const uint8_t *(*re15_dining_r39_rbj_hook)(int *size) = NULL;
static int                  s_leihe_ok = 0, s_leihe_an = 0, s_leihe_slot = -1;
static re15_emd_skeleton_t  s_leihe_skel;
static re15_emd_animation_t s_leihe_anim;

static void leihe_starten(void)
{
    const re15_actor_t *j = john_aktor();
    if (!j) return;
    if (!s_leihe_ok && re15_dining_r39_rbj_hook) {
        int n = 0;
        const uint8_t *rbj = re15_dining_r39_rbj_hook(&n);
        re15_enemy_bank_t *eb = re15_enemy_find(RE15_R38F_JOHN_TYP);   /* Basis-Hierarchie = Johns eigene Bank (wie */
        if (rbj && n > 0 && eb && eb->ok &&                             /* enemy_common.c rbj_resolve_slot)         */
            re15_emd_parse_rbj_record(rbj, (size_t)n, RE15_R39J_WINK_REC, &eb->skel, &s_leihe_skel, &s_leihe_anim) == 0 &&
            s_leihe_anim.clip_count > RE15_R39J_WINK_CLIP)
            s_leihe_ok = 1;
    }
    if (!s_leihe_ok) return;
    s_leihe_an = 1;
    s_leihe_slot = (int)(j - g_actors);
#ifdef RE15_PLATFORM_PC
    fprintf(stderr, "[dining39] F%u John winkt heran: RBJ ROOM%04X Record %d Clip %d geliehen (%d Bilder, %d Clips)\n",
            (unsigned)g_engine.frame_count, RE15_R39J_WINK_RAUM, RE15_R39J_WINK_REC, RE15_R39J_WINK_CLIP,
            s_leihe_anim.clips[RE15_R39J_WINK_CLIP].frame_count, s_leihe_anim.clip_count);
#endif
}

static int leihe_gilt(int slot)
{
    if (!s_leihe_an || slot != s_leihe_slot || slot <= 0 || slot >= RE15_ACTOR_MAX) return 0;
    const re15_actor_t *a = &g_actors[slot];
    return (g_current_room_id & 0xFFFFu) == RE15_R38F_RAUM_1141 && a->active && a->state == 4 && a->sub_state_1 == 0 &&
           !a->walk_active && (int)a->motion == RE15_R39J_WINK_CLIP;
}

const void *re15_dining_r39_rbj_anim(int slot) { return leihe_gilt(slot) ? (const void *)&s_leihe_anim : NULL; }
const void *re15_dining_r39_rbj_skel(int slot) { return leihe_gilt(slot) ? (const void *)&s_leihe_skel : NULL; }
int re15_dining_r39_leihe_aktiv(void) { return leihe_gilt(s_leihe_slot); }
int re15_dining_r39_off_wink(void) { return RE15_R39J_ENTRIEGELN_OFF_WINK; }   /* Riegel */
int re15_dining_r39_schuetze_anker(int k)   /* Riegel: 0 ZIEL, 1 ZIELEN, 2 FEUER1, 3 FEUER2, 4 SENKEN (Nachbesserung 2) */
{
    static const int a[5] = { RE15_R39J_SCHUETZE_OFF_ZIEL, RE15_R39J_SCHUETZE_OFF_ZIELEN, RE15_R39J_SCHUETZE_OFF_FEUER1,
                              RE15_R39J_SCHUETZE_OFF_FEUER2, RE15_R39J_SCHUETZE_OFF_SENKEN };
    return (k >= 0 && k < 5) ? a[k] : -1;
}

void re15_dining_r39_pc(const uint8_t *pc)
{
    if (pc == k_r39j_p_entriegeln + RE15_R39J_ENTRIEGELN_OFF_WINK) { leihe_starten(); return; }
    if (!pc || pc < s_schuetze || pc >= s_schuetze + sizeof s_schuetze) return;
    const int off = (int)(pc - s_schuetze);
    if (off == RE15_R39J_SCHUETZE_OFF_ZIEL) {
        const int beste = ziel_waehlen(john_aktor());
        s_ziel_slot = beste;
        if (beste > 0) {   /* Plc_dest-Operand x @+4 / z @+6 (scd_vm.c op_plc_dest scd_read_le_s16(&pc[4]/&pc[6])) */
            const int16_t x = (int16_t)g_actors[beste].x, z = (int16_t)g_actors[beste].z;
            s_schuetze[off + 4] = (uint8_t)(x & 0xFF); s_schuetze[off + 5] = (uint8_t)((uint16_t)x >> 8);
            s_schuetze[off + 6] = (uint8_t)(z & 0xFF); s_schuetze[off + 7] = (uint8_t)((uint16_t)z >> 8);
        }
    } else if (off == RE15_R39J_SCHUETZE_OFF_ZIELEN) {
        s_zielen = 1;         /* Nachbesserung 2 (b): ab dem Zielen fuehrt John die Waffe nach (re15_dining_r39_tick) */
    } else if (off == RE15_R39J_SCHUETZE_OFF_SENKEN) {
        s_zielen = 0;
    } else if (off == RE15_R39J_SCHUETZE_OFF_FEUER1 || off == RE15_R39J_SCHUETZE_OFF_FEUER2) {
        s_schuss_offen = 1;   /* Verzug 1: Ton + Treffer im naechsten Bild (re15_dining_r39_tick) */
    }
}

/* Trefferbedingung (Riegel): 0 = Treffer, sonst Grund 1 kein Ziel / 2 tot / 3 im Angriff / 4 nicht im Kegel. Nachbesserung 2 (a):
 * KEINE Sperre mehr ueber Elzas Opferzustand (re15_player_victim_state) - die traf auch den Gorilla, der Elza NICHT haelt
 * (Abnahme 1: 5 von 16 Salven). Ausgenommen bleibt nur der Gorilla im Angriff (gorilla_im_angriff: der Greifer in Sub 15, Biss,
 * Sprung): der Wurf des Spielers laeuft im Spieler-Takt (Opfer-Handler 0x8011c118 als Kommando 5, jalr v0 @0x80031cb4) und
 * endet nur in P7 (0x8011c384) - einen Abbruch durch einen Dritten hat das Original nicht. */
int re15_dining_r39_trifft(const re15_actor_t *j, const re15_actor_t *g)
{
    if (!j || !g) return 1;
    if (!gorilla_lebt(g)) return 2;
    if (gorilla_im_angriff(g)) return 3;
    if (re15_ai_arc_test(j, g->x, g->z, RE15_R39J_TREFFER_KEGEL) != 0) return 4;
    return 0;
}

static void schuss(void)
{
    extern int re15_enemy_waffen_schaden(const re15_actor_t *e, int weapon_id);   /* re15_damage.c, Haken */
    g_re15_r39j_schuss_zaehler++;
    if (re15_elza_se_hook) re15_elza_se_hook(RE15_ELZA_TON_SCHUSS);
    re15_actor_t *j = john_aktor();
    re15_actor_t *g = (s_ziel_slot > 0 && s_ziel_slot < RE15_ACTOR_MAX) ? &g_actors[s_ziel_slot] : NULL;
    int treffer = 0;
    static const char *const k_grund[5] = { "", "kein Ziel", "tot", "im Angriff", "nicht im Kegel" };
    const int tr = re15_dining_r39_trifft(j, g);
    const char *grund = k_grund[(tr >= 0 && tr < 5) ? tr : 1];
    if (tr == 0) {
        const int dmg = re15_enemy_waffen_schaden(g, RE15_R39J_WAFFE_JOHN);
        g->hp = (int16_t)(g->hp - dmg);                        /* +0x9a -= dmg */
        g->hit_react |= 0x1;                                   /* +0x93 |= 1 */
        if (((((int32_t)g->rot_y - (int32_t)j->rot_y) + 0x400) & 0xfff) < 0x800)
            g->hit_react |= 0x80;                              /* Ruecken-Treffer wie beim Spieler, Peilung von John */
        g->sub_state_3 = 0;                                    /* +0x7 = 0 (@0x80012428) */
        g->state = (uint8_t)((g->hp >= 0) ? 2 : 3);            /* +0x4 HURT/DEATH (@0x80012520) */
        g->anim_flags &= (uint16_t)~0x80u;
        g->sub_state_2 = 1;                                    /* waagerecht (Zielhoehe 0) */
        g->sub_state_1 = (uint8_t)RE15_R39J_WAFFE_JOHN;        /* +0x5 = Waffen-Id (@0x800124bc) */
        if (s_aufmerksam[s_ziel_slot] <= 0) s_zugewandt[s_ziel_slot] = 0;   /* neue Zuwendung: erst wieder drehen */
        s_auf_takt[s_ziel_slot] = 0;                                          /* Fortschritts-Takt ab diesem Treffer */
        s_aufmerksam[s_ziel_slot] = RE15_R39J_AUFMERKSAM_BILDER;
        s_auf_abstand[s_ziel_slot] = (int32_t)(labs((long)g->x - j->x) + labs((long)g->z - j->z));
        g_re15_r39j_treffer_zaehler++;
        treffer = 1;
    }
#ifdef RE15_PLATFORM_PC
    fprintf(stderr, "[dining39] F%u John schiesst (Schuss %u) auf Gorilla Slot %d: %s%s\n", (unsigned)g_engine.frame_count,
            g_re15_r39j_schuss_zaehler, s_ziel_slot, treffer ? "Treffer, Aufmerksamkeit" : "daneben - ", treffer ? "" : grund);
#endif
    (void)grund;
}

/* Lenkziel im Gorilla-B[3] (enemy_ai_common.c, Haken): 1 = Johns Lage ersetzt das Ziel. Nachbesserung 2 (c): der Haken
 * steuert dann OHNE die Umweg-/Pad-Suche der Runde 36 F (re15_r36_affen_padsuche_ziel) - die plant den Weg zu ELZA (r36_uw_route
 * mit dem Spieler) und hielt den Gorilla am Westrand x -6982 auf Elzas Umweg (Abnahme 1 L2 Gorilla 3: Drehung 3157 -> 3163). */
int re15_dining_r39_affe_ziel(int slot, int32_t *x, int32_t *z)
{
    if (slot <= 0 || slot >= RE15_ACTOR_MAX || s_aufmerksam[slot] <= 0 || !x || !z) return 0;
    if ((g_current_room_id & 0xFFFFu) != RE15_R38F_RAUM_1141) { s_aufmerksam[slot] = 0; return 0; }
    const re15_actor_t *j = john_aktor();
    if (!j || j->x <= -29000) return 0;
    *x = j->x;
    *z = j->z;
    return 1;
}

int re15_dining_r39_ziel_slot(void) { return s_ziel_slot; }
int re15_dining_r39_zielt(void) { return s_zielen; }

int re15_dining_r39_aufmerksam(int slot) { return (slot > 0 && slot < RE15_ACTOR_MAX) ? s_aufmerksam[slot] : 0; }

/* Gorilla-Hirn (enemy_ai_common.c, Kopf von Zustand 1, Haken): waehrend der Aufmerksamkeit kriecht der getroffene Gorilla
 * im Jagd-Ablauf B[3] auf John zu (Lenkziel oben) und trifft keine Angriffs-Entscheidung gegen Elza (A[3]/A[4]: Biss,
 * Aufbaeumen, Sprung - die greifen den SPIELER, re15_player_*). Steht er John naeher als RE15_R39J_AUFMERKSAM_NAH, ist die
 * Aufmerksamkeit vorbei und das Original-Hirn uebernimmt wieder (Elza). PORT-WAHL "Aufmerksamkeit erhalten".
 */
int re15_dining_r39_affe_jagt_john(int slot)
{
    if (slot <= 0 || slot >= RE15_ACTOR_MAX || s_aufmerksam[slot] <= 0) return 0;
    const re15_actor_t *g = &g_actors[slot];
    const re15_actor_t *j = john_aktor();
    if ((g_current_room_id & 0xFFFFu) != RE15_R38F_RAUM_1141 || !j || j->x <= -29000 || g->type != RE15_R38F_GORILLA_TYP) {
        s_aufmerksam[slot] = 0;
        return 0;
    }
    const long dx = (long)g->x - j->x, dz = (long)g->z - j->z;
    if (dx * dx + dz * dz < (long)RE15_R39J_AUFMERKSAM_NAH * RE15_R39J_AUFMERKSAM_NAH) { s_aufmerksam[slot] = 0; return 0; }
    if (gorilla_im_angriff(g)) return 0;   /* laufender Angriff/Sprung spielt zu Ende, danach Jagd zu John */
    return 1;
}

/* Der gezeichnete W-Bank-Clip. Schusswaffe: Clip 6 (RAISE, Gun-FSM sub0) = die Original-Form ROOM3060 @0x010BE. Messer
 * (Waffen-Id < 3, Klassenweiche @0x80074030): ein Messer, das noch nicht in der Hand ist, kommt im Original ueber DRAW
 * (Melee sub4 @0x80035538, Clip 0x0D, Ton 0x1080001 @0x800355f8) in die Bereitschaft - Clip 6 allein hebt die leere Hand
 * (Bild n1_hand.png, Lauf n1 F1756). Senken ist fuer beide Clip 6 rueckwaerts (LOWER @0x80033c74 / @0x80035424). */
int re15_dining_r39_w_clip(void)
{
    if (s_w_clip < 0) return re15_wclip_r41_clip();   /* Runde 41 Spur J: ausserhalb ROOM1141 die allgemeine Regel */
    const re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    if (s_w_clip == RE15_R39J_W_CLIP_HEBEN && re15_player_equipped_weapon() < 3 && !(pl->anim_flags & 0x80))
        return RE15_R39J_W_CLIP_MESSER;
    return s_w_clip;
}

static void w_halten(void)
{
    re15_wclip_r41_halten();   /* Runde 41 Spur J: Phase 2 der allgemeinen W-Bank-Regel (@0x80050fec) */
    if (s_w_clip < 0) return;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    /* Ein anderer Plc-Befehl hat Kommandowort oder Clip ersetzt (Plc_motion @0x80041ba8/@0x80041bb0, Plc_dest @0x80041c14):
     * die W-Bank-Phase ist vorbei. */
    if ((g_current_room_id & 0xFFFFu) != RE15_R38F_RAUM_1141 || pl->state != 4 || (int)pl->motion != s_w_clip) {
        s_w_clip = -1;
        return;
    }
    extern int re15_player_wbank_clip_fc(int clip);   /* player_common.c: W-Bank-EDD-Laengen */
    const int clip = re15_dining_r39_w_clip();
    if (clip == RE15_R39J_W_CLIP_MESSER && !s_w_ton) {
        extern void re15_audio_weapon_se(int idx);
        re15_audio_weapon_se(8);   /* Messer ziehen, SE 0x1080001 (@0x800355f8; player_common.c DRAW-Eintritt) */
        s_w_ton = 1;
    }
    const int fc = re15_player_wbank_clip_fc(clip);
    if (fc >= 1 && (int)pl->anim_frame > fc - 1) pl->anim_frame = (uint16_t)(fc - 1);   /* halten: Phase 2 (@0x80050fec) */
}

void re15_dining_r39_tick(void)
{
    s_sperre_gesehen = (g_re15_pauseflags & RE15_PAUSE_PAD) ? 1 : 0;   /* Nachbesserung 3: gilt im naechsten Bild (s. oben) */
    w_halten();
    if (s_leihe_an && !leihe_gilt(s_leihe_slot) && s_leihe_slot >= 0 &&
        ((g_current_room_id & 0xFFFFu) != RE15_R38F_RAUM_1141 || (int)g_actors[s_leihe_slot].motion != RE15_R39J_WINK_CLIP ||
         g_actors[s_leihe_slot].sub_state_1 != 0))
        s_leihe_an = 0;   /* der naechste Befehl (Abgang RUN) hat Clip/Sub ersetzt: die Leihe ist vorbei */
    if ((g_current_room_id & 0xFFFFu) != RE15_R38F_RAUM_1141) { s_schuss_offen = 0; s_zielen = 0; return; }
    re15_actor_t *jz = john_aktor();
    /* Nachbesserung 2 (b): Zielen (Anker ZIELEN..SENKEN) = John dreht die Waffe dem Ziel je Bild um hoechstens 0xC8 nach
     * (FUN_8001a8f8-Kern, Form Gun-FSM Sub 5 @0x80033f90/@0x80033f94); ist sein Ziel tot oder im Angriff, waehlt er neu (a).
     * Abnahme 1 L2 F4137: Richtung nur vor den 110 Zielbildern bestimmt, das Ziel lief aus dem Kegel (rel -599/-612). */
    if (s_zielen && jz) {
        if (!ziel_gilt(s_ziel_slot)) { const int neu = ziel_waehlen(jz); if (neu > 0) s_ziel_slot = neu; }
        if (s_ziel_slot > 0 && gorilla_lebt(&g_actors[s_ziel_slot]))
            re15_enemy_steer_point(jz, g_actors[s_ziel_slot].x, g_actors[s_ziel_slot].z, RE15_R39J_NACHFUEHREN);
    }
    if (s_schuss_offen) { s_schuss_offen = 0; schuss(); }
    const re15_actor_t *j = jz;
    for (int s = 1; s < RE15_ACTOR_MAX; s++) {
        if (s_aufmerksam[s] <= 0) continue;
        s_aufmerksam[s]--;
        if (!j) continue;
        const re15_actor_t *g = &g_actors[s];
        const int32_t d = (int32_t)(labs((long)g->x - j->x) + labs((long)g->z - j->z));
        /* Nachbesserung 2 (c): erst dreht sich der Gorilla John zu (B[3]-Lenkschritt, Haken enemy_ai_common.c); der Fortschritt
         * zaehlt ab der Zuwendung (Kegel RE15_R39J_ZUGEWANDT = A[3]-Biss-Kegel @0x80117a60-74). Vorher endete die Aufmerksamkeit
         * am ersten Prueftakt, bevor er sich drehen konnte (Abnahme 1 L2: 8 Treffer, je 29 Bilder, Drehung 3157 -> 3163). */
        if (!s_zugewandt[s]) {
            if (re15_ai_arc_test(g, j->x, j->z, RE15_R39J_ZUGEWANDT) == 0) {
                s_zugewandt[s] = 1; s_auf_takt[s] = 0; s_auf_abstand[s] = d;
#ifdef RE15_PLATFORM_PC
                fprintf(stderr, "[dining39] F%u Gorilla Slot %d John zugewandt (Abstand %d, noch %d Bilder)\n",
                        (unsigned)g_engine.frame_count, s, (int)d, (int)s_aufmerksam[s]);
#endif
            }
            continue;
        }
        /* Kein Fortschritt (Tisch, zweiter Gorilla, Wand): alle RE15_R39J_AUFMERKSAM_TAKT Bilder muss der Abstand zu John um
         * RE15_R39J_AUFMERKSAM_SCHRITT kleiner geworden sein, sonst ist die Aufmerksamkeit vorbei (kein Kriechen auf der
         * Stelle) - PORT-WAHL, gemessen Lauf auf2: festgeklemmt 180 Bilder bei Abstand 6600 +-80. */
        if (s_aufmerksam[s] > 0 && ++s_auf_takt[s] >= RE15_R39J_AUFMERKSAM_TAKT) {
            s_auf_takt[s] = 0;
            if (d > s_auf_abstand[s] - RE15_R39J_AUFMERKSAM_SCHRITT) {
#ifdef RE15_PLATFORM_PC
                fprintf(stderr, "[dining39] F%u Gorilla Slot %d: Aufmerksamkeit vorbei - festgeklemmt (Abstand %d, vorher %d)\n",
                        (unsigned)g_engine.frame_count, s, (int)d, (int)s_auf_abstand[s]);
#endif
                s_aufmerksam[s] = 0;
            }
            else s_auf_abstand[s] = d;
        }
    }
}


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
    s_phase_h = 0; s_sperre_gesehen = 0;   /* Nachbesserung 3: Senken unter der Sperre nur in ROOM1141 Phase H */
    s_leihe_an = 0; s_leihe_slot = -1;
    s_leihe_ok = 0;   /* auch die geparste Leihe faellt: der Record haengt an Johns Bank (Basis-Hierarchie, re15_enemy_find),
                       * die mit dem Raum neu geladen wird - leihe_starten() parst ihn je Raum neu (Abnahme 2 H7) */
    s_schuss_offen = 0;
    s_zielen = 0;
    s_ziel_slot = -1;
    memset(s_aufmerksam, 0, sizeof s_aufmerksam);
    memset(s_zugewandt, 0, sizeof s_zugewandt);
    memset(s_auf_takt, 0, sizeof s_auf_takt);
    g_re15_r39j_anker_n = 0;
    if (room_id != RE15_R38F_RAUM_1141) return;
    memcpy(s_schuetze, k_r39j_p_schuetze, sizeof s_schuetze);   /* Ziel-Operand frisch */
    g_re15_r39j_anker_n = 1;
    re15_game_flag_set(RE15_R39J_MERKER_BANK, RE15_R39J_MERKER_BIT, 0);   /* kein Gemaelde-Programm beim Eintritt */
    /* msg 50 (Elzas Reaktion) wird in 1141 nicht mehr gezeigt (Nutzer-Nachtrag 10: die Zeile zieht nach ROOM1041, Spur P);
     * Text und Zugriff re15_dining_r39_text(0) bleiben fuer den Riegel und die Weiterverwendung. */
    meldung(RE15_R39J_MSG_PUH,      k_r39j_msg51, sizeof k_r39j_msg51);
    meldung(RE15_R39J_MSG_FOLGEN,   k_r39j_msg52, sizeof k_r39j_msg52);
    if (!phase_h) return;
    s_phase_h = 1;
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
    if (satz == RE15_R39J_SATZ_KLOPFEN || satz == RE15_R39J_SATZ_TUERTON) g_re15_r39j_se_zaehler[0]++;   /* [0] = Klopfen */
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
    if (voice_id == RE15_R39J_TUERTON_ID && n >= 11) { snprintf(stamm, n, "door_sound"); return; }   /* Nachtrag 17 Punkt 5 */
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
    const re15_actor_t *ga = NULL, *gb = NULL;   /* die beiden Gorillas (Lage, Energie, Aufmerksamkeit) */
    for (int s = 1; s < RE15_ACTOR_MAX; s++)
        if (g_actors[s].active && g_actors[s].type == RE15_R38F_GORILLA_TYP) { if (!ga) ga = &g_actors[s]; else if (!gb) gb = &g_actors[s]; }
    /* Blick Johns zu Elza: Abweichung in der Konvention von re15_ai_arc_test (re15_damage.c: ausgerichtet, wenn
     * atan2_q12(dz,dx) == rot_y + 1024), auf -2048..2047 gefaltet. */
    int blick = 0;
    if (j) blick = (((int)re15_atan2_q12(e->z - j->z, e->x - j->x) - ((int)j->rot_y + 1024)) % 4096 + 4096 + 2048) % 4096 - 2048;
    fprintf(s_f, "F%u cam=%d anz=%d msg=%d/%d john=%d (%d,%d) rot=%d st=%d s1=%d s2=%d mo=%d elza=(%d,%d) rot=%d "
                 "6a=%d f2=%d b94=%d b67=%d se=%u/%u/%u hilfe=%d pm=%d c3=%d w=%d sch=%u/%u g=(%d,%d,%d,%d)/(%d,%d,%d,%d) zl=%d/%d blick=%d\n",
            (unsigned)g_engine.frame_count, (int)g_scd.cam_id, (int)g_scd.work_vars[0x0A],
            (int)g_scd.message_active, (int)g_scd.message_id,
            j ? 1 : 0, j ? (int)j->x : 0, j ? (int)j->z : 0, j ? (int)(j->rot_y & 0xFFF) : 0,
            j ? (int)j->state : 0, j ? (int)j->sub_state_1 : 0, j ? (int)j->sub_state_2 : 0, j ? (int)j->motion : 0,
            (int)e->x, (int)e->z, (int)(e->rot_y & 0xFFF),
            re15_game_flag_get(RE15_R38F_6A_BANK, RE15_R38F_6A_BIT), re15_game_flag_get(RE15_R38F_F2_BANK, RE15_R38F_F2_BIT),
            re15_game_flag_get(9, RE15_R39J_BIT_REAKTION), re15_game_flag_get(9, 67),
            g_re15_r39j_se_zaehler[0], g_re15_r39j_se_zaehler[1], g_re15_r39j_se_zaehler[2], g_re15_r39j_hilfe_letzte,
            (int)g_scd.player_mode, re15_game_flag_get(4, 0xC3), re15_dining_r39_w_clip(), g_re15_r39j_schuss_zaehler,
            g_re15_r39j_treffer_zaehler,
            ga ? (int)ga->x : 0, ga ? (int)ga->z : 0, ga ? (int)ga->hp : 0, ga ? s_aufmerksam[ga - g_actors] : 0,
            gb ? (int)gb->x : 0, gb ? (int)gb->z : 0, gb ? (int)gb->hp : 0, gb ? s_aufmerksam[gb - g_actors] : 0,
            s_zielen, s_ziel_slot, blick);
    fflush(s_f);
#endif
}
