/*
 * RE1.5 Rebuilt — Beta-Birkin-Formen (Kopf, Herkunft und Abgrenzung: include/re15_birkin_formen.h).
 *
 * ⛔ PORT-ERGAENZUNG nach dem 80%-Video, KEIN Auslieferungscode. Belegte Werte tragen @0x…; die uebrigen
 * Zahlen sind aus Video 1 (Complete Disc, 4:36-4:40) zurueckprojiziert oder Gestaltung und sind so markiert.
 */
#include "re15_birkin_formen.h"
#include "re15_actor.h"
#include "re15_enemy.h"
#include "re15_damage.h"        /* re15_enemy_apply_hitbox, re15_enemy_bone_world_pos */
#include "re15_scd.h"           /* g_scd.work_vars[0x0A] = angezeigter Cut, re15_game_flag_set */
#include "re15_room.h"          /* g_current_room_id */
#include "re15_audio.h"        /* re15_audio_room_se */
#include "re15_g5auftritt_r41.h"  /* Runde 41: G5-Auftritt nach RE2 room7040 sub06, Schnittstelle re15_bf_g5_auftritt_start */
#include "re15_skeleton.h"     /* re15_skel_bone_to_world (Nahsicht-Wache) */
#include "re15_camera.h"       /* Nachbesserung 4: re15_camera_build_view (Bildlage je Kamera) */
#include "re15_aot.h"          /* Nachbesserung 4: re15_aot_point_in_quad (Ankerzone des Wagens) */
#include "re15_anim_select.h"  /* Nachbesserung 4: re15_compute_actor_kf (Pose der Form) */
#include "re15_engine.h"       /* Nachbesserung 4: g_engine.frame_count (Messschiene) */
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

void re15_g5_boss_spawn(int slot);
int  re15_g5_boss_zustand(int *sub, int *ph, int *timer);   /* enemy_ai_boss_g5.c */                       /* enemy_ai_boss_g5.c: Ctor faellig */
int  re15_r38_birkin_anim(re15_actor_t *e);              /* enemy_ai_common.c: Clip-Vorschub des Typs */
int  re15_r38_birkin_fc(const re15_actor_t *e);          /* enemy_ai_common.c: Clip-Laenge des Typs */
void re15_enemy_steer_point(re15_actor_t *e, int32_t tx, int32_t tz, int slew);   /* FUN_80015558-Zwilling */

/* ---- Modellwahl --------------------------------------------------------------------------- */
static int s_bf_mutiert = 0;   /* 5090: Original-Birkin hat mutiert -> Bahnsteig-Form */
const char *re15_bf_emd_datei(uint8_t type)
{
    /* Roter Affe (Video 1 4:11-4:19, tools/birkin_formen.py ROT_AFFE): nur die CLUTs von EM29 sind
     * umgefaerbt, Modell/Clips/KI bleiben die des Typs 0x29. RE15_KEIN_ROTER_AFFE=1 = Auslieferung. */
    if (type == 0x29u)
        return ((g_current_room_id & 0xFFFEu) == 0x4080u && !getenv("RE15_KEIN_ROTER_AFFE"))
               ? "EMD/EM29_ROT.EMD" : NULL;
    if (type != 0x30u) return NULL;
    const char *form = getenv("RE15_BIRKIN_FORM");
    if (form) {
        if (!strcmp(form, "original"))  return NULL;
        if (!strcmp(form, "hangar"))    return "EMD/EM30_HANGAR.EMD";
        if (!strcmp(form, "bahnsteig")) return NULL;     /* eigene Bank: re15_bf_bahnsteig_bank_faellig */
    }
    if (re15_bf_hangar_aktiv())    return "EMD/EM30_HANGAR.EMD";
    return NULL;
}

/* Riesenform (Bahnsteig-Form J, RE2/BAHNSTEIG_FORM.EMD, Lader platform/pc/main.c) in 5080/5081 und 5090/5091 ab der
 * Raumladung. PORT-WAHL nach den Nutzer-Entscheiden E5/E6/E17 (analysis/befunde_runde41/form/NUTZER_ENTSCHEIDE.md):
 * Bahnsteig Riesenform -> Flucht, Zug Riesenform -> aus dem Zug geschossen -> G5; die Mutation zur Riesenform liegt am
 * Ende des letzten Sichel-Form-Kampfs VOR dem Bahnsteig. Bis Stufe 3 (form/STUFE3.md) kaempfte in 5090 zuerst der EM30
 * und mutierte (Clip 0x14, sub 6 @0x80117f80); diese Runde gibt es in 5090 nicht mehr, re15_bf_mutation_offen ist 0. */
int re15_bf_riesenform_raum(void)
{
    const unsigned r = g_current_room_id & 0xFFFEu;
    return r == 0x5080u || re15_bf_bahnsteig_aktiv();
}

int re15_bf_flucht_raum(void) { return (g_current_room_id & 0xFFFEu) == 0x5080u; }

int re15_bf_bahnsteig_bank_faellig(uint8_t type)
{
    if (type != 0x30u) return 0;
    const char *form = getenv("RE15_BIRKIN_FORM");
    if (form) return !strcmp(form, "bahnsteig");
    return re15_bf_riesenform_raum();
}

int re15_bf_mutation_offen(void) { return 0; }

static void (*s_bf_modell_hook)(uint8_t) = 0;
void re15_bf_modell_hook(void (*fn)(uint8_t type)) { s_bf_modell_hook = fn; }

/* Runde 41 Spur GR, Nachbesserung 2: Merker "diese 0x30 hat ihre Mutation hinter sich" je Slot (auch ausserhalb 5090:
 * ROOM3071 gibt den Form-1-Birkin mit grid 0x21 -> 0x01 frei, Member_set(0x0C,1) @0x03645). Gesetzt am Mutationsende
 * (sub 6 Phase 2 @0x8011888c ruft re15_bf_mutation_fertig fuer jeden Raum), geloescht beim Raumwechsel (re15_bf_reset). */
static uint8_t s_bf_nach_mutation[RE15_ACTOR_MAX];

void re15_bf_mutation_fertig(int slot)
{
    if (slot >= 1 && slot < RE15_ACTOR_MAX) s_bf_nach_mutation[slot] = 1;
    if (!re15_bf_bahnsteig_aktiv() || s_bf_mutiert) return;
    s_bf_mutiert = 1;
    if (s_bf_modell_hook) s_bf_modell_hook(0x30u);      /* Plattform laedt die 0x30-Bank neu (jetzt Bahnsteig-Form) */
}

int re15_bf_bahnsteig_aktiv(void)
{
    return (g_current_room_id & 0xFFFEu) == 0x5090u && !getenv("RE15_ENDKAMPF_NUR_G5");
}

int re15_bf_hangar_aktiv(void)
{
    return (g_current_room_id & 0xFFFEu) == 0x50A0u && !getenv("RE15_KEIN_HANGAR_BIRKIN");
}

/* Runde 41 Spur GR, Nachbesserung 2 (Kopf: re15_birkin_formen.h): Start-HP und Ende der LAUFENDEN Form eines 0x30.
 * Runde 41 Spur WS: die Handgranate fragt die Form nicht mehr (D_G 251 fuer jeden Gegner, re15_granate_schaden); gebraucht wird
 * die Funktion als Riesenform-Praedikat der Rakete (form_hp == RE15_BB_FORM_HP -> RE2-G4 301 @0x800A5D24, re15_rakete_schaden)
 * und von den Tests (Formende).
 * (Zahlen nach Nachbesserung 3: x RE15_BIRKIN_HP_FAKTOR = 2, Bahnsteig-Form = G4-HP 1000, re15_birkin_formen.h)
 *   Bahnsteig-Form (G4-Bank oder 5090 nach der Mutation) 1000 bis hp < 0   (birkin_bahnsteig.c Eintritt RE15_BB_FORM_HP,
 *                                                                            G4-Ctor `sh v0,342(s1)` @0x8010067c)
 *   Mutation laeuft (+0x1dd & 8)                           300 bis hp < 0   (`ori v0,zero,0x96` @0x80118094; ein toedlicher
 *                                                                            Treffer wird hier ohnehin zu HP 100 = 0x32 @0x8011a534 x 2)
 *   nach der Mutation (Merker, ausserhalb 5090)            300 bis hp < 0   (dieselbe Zuweisung; der Merker gilt nur, solange
 *                                                                            hp <= 300 - ein neuer INIT schreibt 600)
 *   Form 1, Mutation offen (Wurzel-Tor wie unten)          600 bis hp < 200 (`ori v0,zero,0x12c` @0x8011690c, Tor
 *                                                                            `slti v0,v0,100` @0x8011667c -> 0x601)
 *   sonst (Form-2-Bit vom Spawn, z.B. 3070 grid 0x10)      600 bis hp < 0   (INIT @0x8011690c, Tod `bltz` am Applier)
 * Das Wurzel-Tor ist das von enemy_ai_common.c (re15_birkin_root, Abnahme 1 M3):
 * `!(grid & 0x10)` (`andi 0x10` @0x801164e8) oder 5090 vor dem Mutationsstart. */
/* Nachbesserung 3: die Zahlen laufen ueber die HP-Skala RE15_BIRKIN_HP_* (re15_birkin_formen.h, x RE15_BIRKIN_HP_FAKTOR). */
#define BF_HP_INIT      RE15_BIRKIN_HP_INIT       /* 0x12c @0x8011690c (STAGE3/STAGE5 INIT, unbedingt) */
#define BF_HP_MUTATION  RE15_BIRKIN_HP_MUTATION   /* 0x96 @0x80118094-98 */
#define BF_HP_MUT_TOR   RE15_BIRKIN_HP_MUT_TOR    /* slti 100 @0x8011667c */
void re15_bf_granate_form(const re15_actor_t *e, int *form_hp, int *ende)
{
    int slot = e ? (int)(e - g_actors) : -1;
    *form_hp = BF_HP_INIT; *ende = 0;
    if (!e) return;
    if (re15_bb_bank_da(e) || re15_bf_bahnsteig_bank_faellig(e->type)) { *form_hp = RE15_BB_FORM_HP; return; }
    if (e->birkin_flags & 0x8) { *form_hp = BF_HP_MUTATION; return; }
    if (slot >= 1 && slot < RE15_ACTOR_MAX && s_bf_nach_mutation[slot]) {
        if (e->hp <= BF_HP_MUTATION) { *form_hp = BF_HP_MUTATION; return; }
        s_bf_nach_mutation[slot] = 0;                    /* HP ueber 150 = neuer INIT im selben Slot */
    }
    if (!(e->grid_id & 0x10) || re15_bf_mutation_offen()) *ende = BF_HP_MUT_TOR;
}

/* ---- Bahnsteig: Uebergabe an den G5 -------------------------------------------------------
 * Aufruf aus der Birkin-Todesmaschine, Phase 2 (enemy_ai_common.c, @0x8011a690). Der Typwechsel genuegt,
 * weil 0x36 in 5090 eine eigene Bank (RE2-EM36, PC-Lader) und eine eigene KI-Wurzel hat:
 *   - re15_g5_boss_spawn = Sce_em_set-Ctor-Pfad (@0x800421e0 `sw zero,4(s0)` -> Ctor @0x801003CC, HP 600)
 *   - grid 0x13 = die Kampfstart-Armierung (ROOM5090 sub04 Member_set(0x0c,0x13) @0x130A), damit stellt
 *     sich der G5 im naechsten Tick selbst auf (-9000,-23400) (@0x801011d0/@0x801011d8) und spawnt seine
 *     vier 0x37-Arme. */
void re15_bf_morph_zu_g5(int slot)
{
    if (slot < 1 || slot >= RE15_ACTOR_MAX) return;
    re15_actor_t *e = &g_actors[slot];
    e->type = 0x36u;
    e->grid_id = 0x13u;
    e->state = 0; e->sub_state_1 = 0; e->sub_state_2 = 0; e->sub_state_3 = 0;
    e->motion = 0; e->anim_frame = 0; e->anim_frac = 0; e->hit_react = 0;
    e->anim_blend_rate = 0; e->anim_flags &= (uint16_t)~0x80u;   /* Rate und Richtung der Form gehoeren nicht dem G5 */
    e->hp = 600;                                         /* Ctor @0x801003fc, ueberschreibt er ohnehin */
    re15_enemy_apply_hitbox(e, 0x36u);
    re15_g5_boss_spawn(slot);
}

/* ---- Hangar-Szene --------------------------------------------------------------------------
 * Positionen: Rueckprojektion der Video-1-Bilder durch die RID-Kamera ROOM50A0 Cut 12
 * (pos (-13140,-3258,-26604), H 231; Eichung Video->PSX x=(vx-218)/1.64, y=(vy-146)/1.87, Korrelation
 * 0.96 gegen BG ROOM50A12). Fusspunkte im Video: Birkin (195,205), Spider-Man (25,185). */
#define BF_BIRKIN_X   (-8475)
#define BF_BIRKIN_Z  (-21675)
#define BF_SM_X      (-12575)
#define BF_SM_Z      (-16950)
#define BF_HANGAR_CUT    12          /* Video: die Szene laeuft vollstaendig in Cut 12 */

/* Clips. Birkin EM30: 0x0B = GRAB-THROW (ACT[5] @0x80117d30), Wurf bei Bild 0x2C. Spider-Man EM2B (Bank 0,
 * 30 Clips): 0x0C = ATTACK1-Satz (B[4] @0x801127bc), 7 = gekruemmte Reaktion (HURT @0x801148e8),
 * 0x0A = Rueckenlage (DEATH, @0x80114cb0-Kette). Die Rollen in der Szene sind Gestaltung. */
#define BF_CLIP_GRIFF    0x0B
#define BF_WURF_BILD     0x2C
#define BF_GREIF_BILD    12          /* Klaue (Knochen 15) erreicht den Hochpunkt ~ -3900 (gemessen) */
#define BF_KLAUE         15          /* rechte Klaue, EMR Knochen 13-15 */
#define BF_SM_SPRUNG     0x0C
#define BF_SM_GRIFF      7
#define BF_SM_LIEGT      0x0A
/* Gestaltung: Sprungdauer (Video: 276.0 -> 276.5 s = 15 Bilder bis ueber den Kopf), Haltehoehe des
 * Spider-Man unter der Klaue (Becken des Modells -1795 ueber dem Ursprung), Sprungbogen. */
#define BF_SPRUNG_TICKS  14
#define BF_HALTE_DY      2000
#define BF_BOGEN         2600

static struct {
    uint32_t raum;
    int      birkin, sm;           /* Slots, -1 = nicht gesetzt */
    int      phase;                /* s. re15_bf_hangar_phase */
    int      t;
    int32_t  sx, sz;               /* Sprungstart */
    int32_t  vy;                   /* Sturz */
    int      birkin_start;         /* Birkin-Clip laeuft */
} s_h = { 0, -1, -1, 0, 0, 0, 0, 0, 0 };
static int s_szene_gespielt = 0;
static struct { int slot, phase, t; } s_rs = { -1, 0, 0 };   /* Rueckstoss-Szene 5090, s. unten */
static int s_rs_kam_halt = 0;  /* Nachbesserung 4: Haltezeit nach einem Szenenschnitt */
#define BF_RS_KAM_HALT 30      /* Gestaltung: 1 s wie die Haltezeit der Nahsicht-Wache (Nachbesserung 1) */
static int s_kam_auftritt = 0, s_kam_wache = 0, s_kam_wache_halt = 0;   /* Nachbesserung 1 M4, s. bf_nahsicht_wache */
static void bf_nahsicht_wache(void);
static void bf_kamera(int cut);
static void bf_kamera_setzen(int cut);
static void bf_kamera_frei(void);
static void bf_zucken(re15_actor_t *e);

void re15_bf_reset(void)
{
    s_bf_mutiert = 0;
    memset(s_bf_nach_mutation, 0, sizeof s_bf_nach_mutation);   /* Runde 41 Spur GR NB2 */
    re15_g5a_reset();                                    /* Runde 41: G5-Auftritt (g5auftritt_r41.c) */
    s_kam_auftritt = 0; s_kam_wache = 0; s_kam_wache_halt = 0;
    re15_bb_reset();
    s_rs.slot = -1; s_rs.phase = 0; s_rs.t = 0; s_rs_kam_halt = 0;
    s_h.raum = 0; s_h.birkin = s_h.sm = -1; s_h.phase = 0; s_h.t = 0; s_h.birkin_start = 0;
}

int re15_bf_hangar_phase(void) { return s_h.phase; }
int re15_bf_hangar_slot(int birkin) { return birkin ? s_h.birkin : s_h.sm; }

static void bf_rueckstoss_tick(void);
int re15_bf_szene_besitzt(int slot)
{
    if (slot >= 0 && slot == s_rs.slot) return 1;       /* Rueckstoss-Szene 5090 */
    if (slot >= 0 && re15_bb_pose_aktiv() && g_actors[slot].type == 0x30u) return 1;   /* Messhaken: Pose steht fest */
    if (s_h.raum == 0 || slot < 0) return 0;
    if (slot == s_h.sm) return 1;                       /* der Spider-Man gehoert bis zum Raumwechsel der Szene */
    return slot == s_h.birkin && s_h.phase < 5;
}

static int16_t bf_blick(const re15_actor_t *e, int32_t tx, int32_t tz)
{
    /* dieselbe Peilung wie re15_enemy_steer_point (enemy_ai_common.c) */
    return (int16_t)(((int)re15_atan2_q12(tz - e->z, tx - e->x) - 0x400) & 0x0fff);
}

static int bf_neuer_aktor(uint8_t type, int32_t x, int32_t z, uint8_t grid)
{
    int frei = -1;
    for (int i = RE15_ACTOR_MAX - 1; i >= 1; i--)        /* rueckwaerts wie der Alligator (main.c) */
        if (!g_actors[i].active) { frei = i; break; }
    if (frei < 0) return -1;
    re15_actor_t *a = &g_actors[frei];
    memset(a, 0, sizeof *a);
    a->active = 1; a->type = type;
    a->x = x; a->y = 0; a->z = z;
    a->grid_id = grid;
    a->em_flag_id = 0xFF;                                /* kein Kill-Persist */
    re15_enemy_apply_hitbox(a, type);
    return frei;
}

static void bf_clip(re15_actor_t *e, uint8_t c) { e->motion = c; e->anim_frame = 0; e->anim_frac = 7; }

/* Clip-Vorschub mit der Bank des Typs; am Ende das letzte Bild halten statt umzulaufen. */
static int bf_anim_halt(re15_actor_t *e)
{
    int fc = re15_r38_birkin_fc(e);
    if (e->anim_frame + 1 >= fc) { e->anim_frame = (uint8_t)(fc - 1); return 1; }
    re15_r38_birkin_anim(e);
    return 0;
}

void re15_bf_raumtick(void)
{
    re15_bb_pose_tick();                                 /* Messhaken der Sichtpruefung, im Normalpfad stumm */
    /* MESSHAKEN RE15_BIRKIN_HP_AT=<n>: n KI-Ticks nach Raumeintritt in 5090 die HP des 0x30 auf 0 (Sichtpruefung
     * der Rueckstoss-Szene ohne Durchspielen). Der Debug-Sprung setzt keinen Tuer-Schnitt, die Zonen laufen dann
     * nie; RE15_BIRKIN_CUT=<n> stellt beim Raumeintritt den Schnitt (12 = Kampfstart sub01 @0x1290). Stumm im Normalpfad. */
    { static int at = -2; static unsigned raum = 0; static int t = 0;
      if (at == -2) { const char *v = getenv("RE15_BIRKIN_HP_AT"); at = v ? atoi(v) : -1; }
      if (at >= 0 && re15_bf_riesenform_raum()) {
          if (raum != g_current_room_id) {
              const char *c = getenv("RE15_BIRKIN_CUT");
              raum = g_current_room_id; t = 0;
              if (c) bf_kamera_setzen(atoi(c));          /* Zonen bleiben an wie nach dem echten Kampfstart */
          }
          if (++t == at || (t == 2 * at && s_bf_mutiert))
              for (int i = 1; i < RE15_ACTOR_MAX; i++)
                  if (g_actors[i].active && g_actors[i].type == 0x30u && g_actors[i].state == 1) {
                      g_actors[i].hp = 0; g_actors[i].state = 3; g_actors[i].sub_state_3 = 0; }   /* = Todes-Commit nach Treffer */
      } }
    bf_rueckstoss_tick();
    bf_nahsicht_wache();                                 /* Nachbesserung 1 M4: Riesenform 5090, Kamera */
    /* G5-AUFTRITT (Runde 41, RE2 room7040 sub06 ab der Armierung @0x018DE): Grossaufnahme der offenen Wagentuer (RE2-Schnitt 9),
     * zuerst die Tentakel ([T1]), dann zieht sich der Koerper herein ([T2]), danach RE2-Schnitte 10/5/6 bis zum Kampf.
     * Kamera, Spielersperre und Ablauf: g5auftritt_r41.c. (Hier stand bis Runde 41 die Szenenkamera Cut 11 bis [T2].) */
    re15_g5a_tick();
    if (!re15_bf_hangar_aktiv()) { if (s_h.raum) re15_bf_reset(); return; }
    if (s_h.raum != g_current_room_id) {
        re15_bf_reset();
        s_h.raum = g_current_room_id;
        /* Birkin: grid 0x10 = Form-2-Bit (@0x801164e8 sperrt die HP-Mutation), Nibble 0 -> INIT-HUB
         * (@0x80116880-8b8), also ein gewoehnlicher Angreifer, sobald die Szene ihn freigibt. */
        s_h.birkin = bf_neuer_aktor(0x30u, BF_BIRKIN_X, BF_BIRKIN_Z, 0x10u);
        if (!s_szene_gespielt)
            s_h.sm = bf_neuer_aktor(0x2Bu, BF_SM_X, BF_SM_Z, 0x00u);
        if (s_h.birkin < 0) { re15_bf_reset(); s_h.raum = g_current_room_id; return; }
        re15_actor_t *b = &g_actors[s_h.birkin];
        b->hp = RE15_BIRKIN_HP_INIT;                     /* INIT-Wert 0x12c @0x8011690c (x Faktor, GR NB3), laeuft nach der Szene erneut */
        if (s_h.sm >= 0) {
            re15_actor_t *m = &g_actors[s_h.sm];
            b->rot_y = bf_blick(b, m->x, m->z);
            m->rot_y = bf_blick(m, b->x, b->z);
            m->hp = 100;
            bf_clip(m, 0);
        } else {
            s_h.phase = 5;                               /* Szene schon gesehen: nur der Angreifer */
            b->state = 0;
        }
        bf_clip(b, 0);
        return;
    }
    if (s_h.birkin < 0 || s_h.phase >= 5) return;
    re15_actor_t *b = &g_actors[s_h.birkin];
    re15_actor_t *m = (s_h.sm >= 0) ? &g_actors[s_h.sm] : NULL;
    if (!b->active || b->type != 0x30u || !m || !m->active) { s_h.phase = 5; b->state = 0; return; }
    if (!re15_enemy_find(0x30u) || !re15_enemy_find(0x2Bu)) return;   /* Banken noch nicht geladen */

    int32_t k[3];
    switch (s_h.phase) {
    case 0:                                              /* warten, bis Cut 12 angezeigt wird */
        if ((int)g_scd.work_vars[0x0A] != BF_HANGAR_CUT) return;
        s_h.phase = 1; s_h.t = 0; s_h.sx = m->x; s_h.sz = m->z;
        bf_clip(m, BF_SM_SPRUNG);
        /* Birkins Griff so starten, dass die Klaue beim Eintreffen am Hochpunkt steht */
        s_h.birkin_start = 0;
        break;
    case 1: {                                            /* Sprung auf die Klaue */
        s_h.t++;
        if (!s_h.birkin_start && s_h.t >= BF_SPRUNG_TICKS - BF_GREIF_BILD) {
            bf_clip(b, BF_CLIP_GRIFF); b->anim_frac = 0xf; s_h.birkin_start = 1;
        } else if (s_h.birkin_start) re15_r38_birkin_anim(b);
        re15_enemy_bone_world_pos(b, BF_KLAUE, k);
        int32_t ziel_y = k[1] + BF_HALTE_DY;
        int t = s_h.t, n = BF_SPRUNG_TICKS;
        m->x = s_h.sx + (int32_t)((int64_t)(k[0] - s_h.sx) * t / n);
        m->z = s_h.sz + (int32_t)((int64_t)(k[2] - s_h.sz) * t / n);
        m->y = (int32_t)((int64_t)ziel_y * t / n) - (int32_t)((int64_t)BF_BOGEN * 4 * t * (n - t) / (n * n));
        bf_anim_halt(m);
        if (t >= n) { s_h.phase = 2; bf_clip(m, BF_SM_GRIFF); }
        break;
    }
    case 2:                                              /* im Griff, bis zum Wurf-Bild */
        re15_r38_birkin_anim(b);
        re15_enemy_bone_world_pos(b, BF_KLAUE, k);
        m->x = k[0]; m->z = k[2]; m->y = k[1] + BF_HALTE_DY;
        m->rot_y = bf_blick(m, b->x, b->z);
        re15_r38_birkin_anim(m);                         /* Zappeln: Clip 7 laeuft im Kreis */
        if (b->anim_frame >= BF_WURF_BILD) { s_h.phase = 3; s_h.vy = 0; }
        break;
    case 3:                                              /* geschleudert: freier Fall auf den Boden */
        re15_r38_birkin_anim(b);
        s_h.vy += 90;                                    /* Gestaltung: Fallbeschleunigung je Tick */
        m->y += s_h.vy;
        if (m->y >= 0) {
            m->y = 0; s_h.phase = 4;
            bf_clip(m, BF_SM_LIEGT);
            m->hp = 0;
        }
        break;
    case 4:                                              /* liegt (letztes Bild bleibt); Birkin spielt den Griff zu Ende */
        bf_anim_halt(m);
        if (bf_anim_halt(b)) {
            s_h.phase = 5;
            s_szene_gespielt = 1;
            b->state = 0;                                /* INIT -> HUB: Angriff auf den Spieler */
            b->sub_state_1 = b->sub_state_2 = b->sub_state_3 = 0;
        }
        break;
    default: break;
    }
}

/* ---- Bahnsteig: Rueckstoss-Szene Bahnsteig-Form -> G5 -------------------------------------
 * Nutzer 2026-10-30: "Man schiesst Birkin energiemaessig runter. Dann kommt eine Cutscene, wie Birkin mit
 * Schuessen nach hinten aus dem Zug rausgedraengt wird. Dann kommt er mit den Tentakeln in der finalen Form
 * wieder rein - genauso wie in RE2."
 * Ablauf (Gestaltung, keine Original-Szene; ausgeloest beim Todeseintritt des 0x30 in 5090/5091 statt der
 * Kollaps-Kette @0x8011a5d8):
 *   1 SCHIESSEN  der Spieler zielt und feuert selbst (Eingabe R1+Quadrat ueber re15_bf_pad, also der echte
 *                Schuss-Ablauf mit Animation/Ton/Munition), Blick auf Birkin; Birkin zuckt (Flinch-Clip 7,
 *                HURT @0x8011a2e0) und weicht Richtung Zugende zurueck.
 *   2 HINAUS     Birkin passiert das Westende des Wagens (Eckbloecke x -5683, Luecke z -24974..-21990 -
 *                derselbe Weg, auf dem der RE2-G5 hereinkommt, enemy_ai_boss_g5.c @0x801011d0) und
 *                verschwindet; Pause mit Gebruell.
 *   3 ZURUECK    Typwechsel zum G5 (re15_bf_morph_zu_g5): dessen RE2-Auftritt von (-9000,-23400) samt der
 *                vier Tentakel uebernimmt. */
#define BF_RS_AUSGANG_X  (-5683)      /* Eckbloecke Westende ROOM5090 (SCA [21]/[25]) */
#define BF_RS_AUSGANG_Z  (-23482)     /* Mitte der Luecke z -24974..-21990 */
#define BF_RS_MIN_TICKS  90           /* Gestaltung: mindestens 3 s Beschuss */
#define BF_RS_PAUSE      45           /* Gestaltung: Pause hinter dem Zugende */
/* Kameras ROOM5090 (RID): Cut 12 (18526,-24660)->(1496,-20370) = Kampfkamera (sub04 Cut_chg 0x0C @0x1342),
 * Cut 11 (7626,-25360)->(-1504,-21970) = Blick auf das Zugende (Westende x -5683; dieselbe Kamera wie das
 * Kreatur-Standbild V2 1:56.3, SIFT-Beleg analysis/befunde_runde41/R_bahnsteig.md). RE2 room7040 hat eigene Kameras, die der RE1.5-Wagen nicht teilt. */
#define BF_RS_ZUGENDE_KAM (-2500)     /* Gestaltung: ab hier schneidet die Szene aufs Zugende */
/* Zuckt die Form unter dem Beschuss: mit der G4-Bank Clip 8 ab Bild 5 (@0x80107224), sonst EM30 Clip 7 (HURT
 * @0x8011a2e0). */
static void bf_zucken(re15_actor_t *e)
{
    e->anim_flags &= (uint16_t)~0x80u;                   /* Form kann gerade rueckwaerts gelaufen sein */
    if (re15_bb_bank_da(e)) { e->motion = 8; e->anim_frame = 5; e->anim_frac = 7; e->anim_blend_rate = 0x200; }   /* a3 @0x80107244 */
    else { bf_clip(e, 7); e->anim_frac = 0xf; }
}

static void bf_kamera_setzen(int cut)
{
    if ((int)g_scd.work_vars[0x0A] != cut || g_scd.cam_id != cut) { g_scd.cam_id = (uint8_t)cut; g_scd.cam_change_pending = 1; }
}

/* Szenen-Kamera. ⛔ Abnahme 1 M9: ROOM5090 sub04 laesst die Zonen an (Cut_auto 01 @0x134C). Stand der Spieler in einer Zone,
 * deren Wechsel aus dem Szenen-Cut fuehrt (RVD @0x490 Cut 11 -> 12, x 2800..4200; @0x4b8 Cut 12 -> 11, x 1203..2703; @0x4cc
 * Cut 12 -> 13, x 14000..15400), setzte der Scan cam_id zurueck und die Szene wieder hin, je Bild: jedes Bild ein Apply,
 * Kamerahalt samt 6 Bilddauern, 4 Bilder je Sekunde, bis zu zwei Minuten Standbild. Wie Cut_chg schaltet die Szene die
 * Zonen ab (Bit 0x100 @0x800402d4, Scan-Gate @0x8001cce0-e4, Port cut_auto_enabled = 0, cut_chg_zwilling in
 * gatorszene_r41.c) und gibt sie am Ende wieder frei (FUN_8004032c @0x8004032c). */
static void bf_kamera(int cut)
{
    g_scd.cut_auto_enabled = 0;
    bf_kamera_setzen(cut);
}

static void bf_kamera_frei(void) { g_scd.cut_auto_enabled = 1; }

/* ---- Nachbesserung 1, M4: Kamera-Nahsicht der Riesenform in 5090/5091 (PORT-WAHL) ------------------------------------
 * Abnahme 1 M4: (a) Kampfstart, Cut 15 (RID-Auge (46,-1236,-24302) -> (5190,-2246,-20090), ROOM5090 sub04 Cut_chg 0x0f @0x0130E):
 * die Form steht 1150 vor der Kamera auf der Pos_set-Lage (1200,-23350) (sub04 @0x012FE), 1 s nur Texturflaeche. Offline gemessen
 * (form_buehne, Clip 10 Bild 30): x 1200 -> 89 % des Bildes, x 3000 -> 45 % und rechts abgeschnitten; die Blickachse von Cut 15 verlaesst
 * den Gang (z -21700) schon bei x 3220, eine Lage weiter weg gibt es im Wagen nicht. Deshalb ein eigener Schnitt: die Riesenform tritt in
 * Cut 11 auf (Auge (7626,-3256,-25360) -> (-1504,-986,-21970)), der Kamera von Bild 2 (form/kamera_bild2.json, Lage (941,-23776)) -
 * Clip 10 Bild 45-52 ist Pose Bild 2. Gehalten bis Clip 10 Bild BF_AUFTRITT_KAM_BIS, dann der Schnitt der Spielerlage
 * (re15_bf_schnitt_aus_lage) und die Zonen wieder frei, wie das Szenenende der Rueckstoss-Szene.
 * (b) Cut 13 (Auge x 11050, Blick +x) und Cut 12 (Auge x 18526, Blick -x) schauen den Gang entlang gegeneinander. Steht die Form
 * weniger als BF_NAH_ACHSE vor dem Auge und der Spieler dahinter (Abnahme: Leon x 15191, Form 8223 -> 13380, 8 s 60-90 % des Bildes),
 * schaltet die Wache auf die Gegenkamera, wenn der Spieler dort mindestens 1500 vor dem Auge steht und die Form hinter ihm. Haltezeit
 * 30 Bilder gegen Flattern; danach Zonen frei.
 * Nachbesserung 4 Runde 2 (Abnahme 3 N1/N2/E1): die Wache nach dem Bildmass (Runde 1) war im Kampf schlechter als diese Achsenregel
 * (gleiches Spiel je Bild, k-Weg 38 statt 7 Bilder > 60 % fast schwarz mit Kurzschnitten, v-Weg 28 Bilder dunkler Aussenblick Cut 14,
 * e-Weg Spielerruecken 37,9 %) und ist zurueckgenommen; die Achsenregel gilt wieder unveraendert (Stand 2e3713c74). Das Bildmass bleibt
 * nur fuer die Rueckstoss-Szene (bf_schnitt_waehlen). Rest M-B (Spieler 800-1300 vor dem Auge von Cut 12 nach dem Wurf, Abnahme 2
 * F1051-F1174): keine Kamera des Raums zeigt dort Spieler und Form im Pruefmass (Abnahme 3 Abschnitt 3.3, F1049-F1125 Cut 12 bis 48 %
 * Spielerflaeche), es bleibt die Original-Zonenkamera Cut 13 (PORT-WAHL des Orchestrators, kein Nutzerentscheid). */
#define BF_AUFTRITT_KAM_BIS 60
#define BF_NAH_ACHSE      4500
static const struct { uint8_t cut; int32_t auge_x; int8_t blick; } k_gang_kam[2] = {
    { 0x0C, 18526, -1 },   /* RID ROOM5090 Cut 12 */
    { 0x0D, 11050, +1 },   /* RID ROOM5090 Cut 13 */
};

static int bf_riesenform_slot(void)
{
    for (int i = 1; i < RE15_ACTOR_MAX; i++) {
        const re15_actor_t *e = &g_actors[i];
        if (e->active && !e->no_draw && e->type == 0x30u && re15_bb_bank_da(e) && !(e->grid_id & 0x20)) return i;
    }
    return -1;
}

/* ---- Nachbesserung 4 (Abnahme 2 M-A, M-B): Schnittwahl nach der Bildlage von Form und Spieler (PORT-WAHL) ----------------
 * Abnahme 2 M-A: die Rueckstoss-Szene hielt fest Cut 12 (Auge x 18526, Blick -x). Im echten Wagenweg stand die Form bei Szenenbeginn
 * 1240 vor dem Auge (16 Bilder 77-100 % fast schwarz) bzw. hinter dem Auge, waehrend der Spieler 711 davor stand (7,5 s nur sein Ruecken).
 * M-B: nach dem Wurf stand der Spieler 800-1300 vor dem Auge von Cut 12, die Wache schaltete nicht, Cut 13 zeigte 4 s die Form vor ihm.
 * Statt fester Schnitte prueft die Szene (und die Wache) je Bild, was jede Kamera des Wagens zeigt:
 *   Kameras des Wagens = jeder Cut, dessen Ankerzone (erste RVD-Zone seiner Gruppe, Such-Anker FUN_80014230/FUN_80014324,
 *   rdt_common.c) den Spieler enthaelt. ROOM5090/5091 Wagen C (Datei-Offsets, 20 Byte je Zone): Cut 11 @0x47c, 12 @0x4a4,
 *   13 @0x4e0 (je x -5600..28100, z -29700..-16975), 14 @0x51c (x 17000..28050), 15 @0x544 (wie 11). Augen/Ziele aus der RID
 *   (32 Byte je Cut ab 0x60): Cut 11 @0x1c0, 12 @0x1e0, 13 @0x200, 14 @0x220, 15 @0x240. Keine eigene Kamera.
 *   Original-Zonenkamera = re15_bf_schnitt_aus_lage (RVD-Streifen @0x490-@0x530, s. dort).
 *   Projektion wie der Zeichner: Blickmatrix re15_camera_build_view (FUN_80053ca4), H = fov >> 7, Bild 320 x 240, Mitte (160,120).
 *   Form = Kugeln um alle Knochen der Pose (re15_skel_compute_pose wie re15_enemy_part_point), Spieler = Kugeln y 0..-3000 (wie
 *   gatorszene_r41.c spieler_punkte, dort an der exe gemessen: ~3000 Einheiten hoch, Rumpf r 500).
 * Schranken = Pruefmass der Abnahme 2 (Abschnitt 5 M-A): kein Szenenbild mit mehr als 60 % Form (dort: Bildpunkte < 24), kein
 * Spielerkoerper ueber einem Drittel des Bildes; dazu Form nicht von der Kamera geschnitten und nicht hinter dem Spieler.
 * Das Kugelmodell misst Rahmen, nicht Bildpunkte; geeicht an den GEZEICHNETEN Dreiecken (Messschiene RE15_SLOTDECKUNG, render_pc.c,
 * Laeufe n2_w_leon/n2_v_leon, STUFE3.md Nachbesserung 4): Formrahmen / Formbildpunkte 1,65-2,6, Spielerrahmen / Spielerbildpunkte
 * 2,3-3,2, Modell-Verdeckung / gezeichnete Verdeckung 1,4-3,2 (bei Modell 443-500 gezeichnet 275-355 Promille). Die Modellschranken
 * unten halten damit die Bildpunkt-Schranken auch beim kleinsten gemessenen Verhaeltnis ein. */
#define BF_BM_FORM_R      700    /* PORT-WAHL: Kugel je Formknochen; Schulter +-1470, Haende +-1590 (am Asset, birkin_bahnsteig.c:723) */
#define BF_BM_PL_R        500    /* = GSZ_SPIELER_RADIUS (gatorszene_r41.c, an der exe gemessen) */
#define BF_BM_PL_N        5      /* = gatorszene_r41.c spieler_punkte: Kugeln y 0..-3000 in Vierteln (an der exe gemessen ~3000 hoch) */
#define BF_BM_PL_Y0       0
#define BF_BM_PL_DY       750
#define BF_BM_NAH         1000   /* PORT-WAHL: Formkugel naeher als das an der Bildebene (im Bild) = von der Kamera geschnitten; der
                                  * gezeigte Schnitt bleibt bis zur Bildebene selbst (Grenze 0), gegen Flattern (Laeufe n3: Cut 11/15) */
#define BF_BM_FORM_MAX    900    /* Promille Rahmen; / 1,65 = 545 Promille Bildpunkte < 60 % (Abnahme 2 M-A) */
#define BF_BM_PL_MAX      490    /* Promille Rahmen; kleinstes gemessenes Verhaeltnis 1,49 (n3_a F1077: 665 -> 447) -> <= 329 Promille
                                  * Bildpunkte < ein Drittel (Abnahme 2 M-A); dazu keine Spielerkugel an der Bildebene (pl_nah) */
#define BF_BM_DECK_MAX    650    /* Promille Rahmen; kleinstes gemessenes Verhaeltnis 1,41 (500 -> 355) -> <= 461 Promille verdeckt, Form zu mehr als der Haelfte sichtbar */
#define BF_BM_INNEN_MIN   500    /* PORT-WAHL: mindestens die Haelfte der Formknochen (Kugelmitten) im Bild */
#define BF_BM_PUNKTE      32
typedef struct {
    int ok;                       /* Kamera da (RID), Form projiziert */
    int nah;                      /* Formkugel an der Bildebene im Bild */
    int hinter;                   /* Formkugeln hinter der Kamera */
    int form;                     /* Promille: Formrahmen im Bild (geklemmt) */
    int innen;                    /* Promille: Formkugel-Mitten im Bild (von allen Formkugeln) */
    int mitte_in;                 /* Schwerpunkt der Formkugel-Mitten im Bild */
    int pl;                       /* Promille: Spielerrahmen im Bild */
    int pl_in;                    /* Spielerrahmen-Mitte im Bild */
    int pl_nah;                   /* Spielerkugel naeher als BF_BM_NAH an der Bildebene (im Bild): Rahmen unzuverlaessig, Spieler zu nah */
    int deck_form;                /* Promille der Formflaeche hinter dem naeheren Spieler */
    int deck_pl;                  /* Promille der Spielerflaeche hinter der naeheren Form */
} bf_bild_t;

static int bf_form_punkte(const re15_actor_t *e, int32_t pk[BF_BM_PUNKTE][3])
{
    re15_enemy_bank_t *b = re15_enemy_find(e->type);
    int n = 0;
    if (b && b->ok && b->skel.bone_count > 0) {
        re15_skel_pose_t poses[RE15_EMD_MAX_BONES];
        const int kf = re15_compute_actor_kf(&b->anim, &b->skel, e, -1, e->anim_frame);
        void *save = g_anim_pose_actor;
        int rv;
        g_anim_pose_actor = NULL;                        /* QUERY wie re15_enemy_part_point */
        rv = re15_skel_compute_pose(&b->skel, kf, poses);
        g_anim_pose_actor = save;
        if (rv == 0)
            for (int i = 0; i < b->skel.bone_count && n < BF_BM_PUNKTE; i++, n++)
                re15_skel_bone_to_world(poses[i].trans, e->rot_y, e->x, e->y, e->z, pk[n]);
    }
    if (n == 0) { pk[0][0] = e->x; pk[0][1] = e->y; pk[0][2] = e->z; n = 1; }
    return n;
}

/* Rahmen (x0,y0,x1,y1) der Kugeln im Bild; Tiefe = Mittel der vorderen. Rueckgabe Zahl der Kugeln vor der Kamera.
 * innen/sp: Zahl der Kugelmitten im Bild und Summe ihrer Bildlagen (Schwerpunkt). */
static int bf_rahmen(const re15_camera_view_t *v, int32_t pk[][3], int n, int32_t r, int64_t rm[4], int64_t *tief, int *hinter, int *nah,
                     int32_t nah_grenze, int *innen, int64_t sp[2])
{
    const int64_t h = v->fov_screen_dist;
    int vorn = 0;
    int64_t summe = 0;
    if (innen) *innen = 0;
    if (sp) sp[0] = sp[1] = 0;
    rm[0] = rm[1] = INT64_MAX; rm[2] = rm[3] = INT64_MIN;
    for (int i = 0; i < n; i++) {
        const int64_t px = pk[i][0], py = pk[i][1], pz = pk[i][2];
        const int32_t vx = (int32_t)(((int64_t)v->rot[0]*px + (int64_t)v->rot[1]*py + (int64_t)v->rot[2]*pz) >> 12) + v->trans[0];
        const int32_t vy = (int32_t)(((int64_t)v->rot[3]*px + (int64_t)v->rot[4]*py + (int64_t)v->rot[5]*pz) >> 12) + v->trans[1];
        const int32_t vz = (int32_t)(((int64_t)v->rot[6]*px + (int64_t)v->rot[7]*py + (int64_t)v->rot[8]*pz) >> 12) + v->trans[2];
        if (vz + r <= 0) { if (hinter) (*hinter)++; continue; }
        {
            const int64_t d = vz > r ? vz : r;
            const int64_t bx = 160 + (vx * h) / d, by = 120 + (vy * h) / d, br = (r * h) / d;
            if (bx - br < rm[0]) rm[0] = bx - br;
            if (by - br < rm[1]) rm[1] = by - br;
            if (bx + br > rm[2]) rm[2] = bx + br;
            if (by + br > rm[3]) rm[3] = by + br;
            if (nah && vz - r < nah_grenze && bx + br > 0 && bx - br < 320 && by + br > 0 && by - br < 240) *nah = 1;
            if (vz > 0 && bx >= 0 && bx < 320 && by >= 0 && by < 240) {
                if (innen) (*innen)++;
                if (sp) { sp[0] += bx; sp[1] += by; }
            }
        }
        summe += vz; vorn++;
    }
    *tief = vorn ? summe / vorn : 0;
    return vorn;
}

static int64_t bf_flaeche(const int64_t a[4]) { return (a[2] > a[0] && a[3] > a[1]) ? (a[2] - a[0]) * (a[3] - a[1]) : 0; }
static void bf_klemmen(const int64_t a[4], int64_t k[4])
{
    k[0] = a[0] < 0 ? 0 : a[0]; k[1] = a[1] < 0 ? 0 : a[1];
    k[2] = a[2] > 320 ? 320 : a[2]; k[3] = a[3] > 240 ? 240 : a[3];
}
static int64_t bf_schnitt(const int64_t a[4], const int64_t b[4])
{
    const int64_t s[4] = { a[0] > b[0] ? a[0] : b[0], a[1] > b[1] ? a[1] : b[1], a[2] < b[2] ? a[2] : b[2], a[3] < b[3] ? a[3] : b[3] };
    return bf_flaeche(s);
}

static int bf_bild_mass(int cut, int32_t pk[][3], int npk, int gezeigt, bf_bild_t *m)
{
    re15_camera_view_t v;
    int32_t sp[BF_BM_PL_N][3];
    int64_t fr[4], fk[4], pr[4], pkl[4], ft = 0, pt = 0;
    const re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    int nf, np, fin = 0;
    int64_t fsp[2];
    memset(m, 0, sizeof *m);
    if (!g_room_rdt_ok || !g_room_rdt.cuts || cut < 0 || cut >= g_room_rdt.cut_count) return 0;
    if (re15_camera_build_view(&g_room_rdt.cuts[cut], &v) != 0) return 0;
    nf = bf_rahmen(&v, pk, npk, BF_BM_FORM_R, fr, &ft, &m->hinter, &m->nah, gezeigt ? 0 : BF_BM_NAH, &fin, fsp);
    for (int i = 0; i < BF_BM_PL_N; i++) { sp[i][0] = pl->x; sp[i][1] = pl->y + BF_BM_PL_Y0 - BF_BM_PL_DY * i; sp[i][2] = pl->z; }
    np = pl->active ? bf_rahmen(&v, sp, BF_BM_PL_N, BF_BM_PL_R, pr, &pt, NULL, &m->pl_nah, BF_BM_NAH, NULL, NULL) : 0;
    m->ok = 1;
    if (nf > 0) {
        bf_klemmen(fr, fk);
        m->form = (int)(bf_flaeche(fk) * 1000 / (320 * 240));
        m->innen = fin * 1000 / npk;
        m->mitte_in = fin > 0 && fsp[0] / fin >= 0 && fsp[0] / fin < 320 && fsp[1] / fin >= 0 && fsp[1] / fin < 240;
    }
    if (np > 0) {
        bf_klemmen(pr, pkl);
        m->pl = (int)(bf_flaeche(pkl) * 1000 / (320 * 240));
        m->pl_in = (pr[0] + pr[2]) / 2 >= 0 && (pr[0] + pr[2]) / 2 < 320 && (pr[1] + pr[3]) / 2 >= 0 && (pr[1] + pr[3]) / 2 < 240;
        if (nf > 0) {
            const int64_t s = bf_schnitt(fk, pkl), af = bf_flaeche(fk), ap = bf_flaeche(pkl);
            if (pt < ft && af > 0) m->deck_form = (int)(s * 1000 / af);
            if (ft < pt && ap > 0) m->deck_pl = (int)(s * 1000 / ap);
        }
    }
    return 1;
}

/* Szene: die Form ist das Motiv. */
static int bf_bild_szene_gut(const bf_bild_t *m)
{
    return m->ok && !m->nah && !(m->hinter && m->form > 0) && m->mitte_in && m->innen >= BF_BM_INNEN_MIN
        && m->form <= BF_BM_FORM_MAX && m->pl <= BF_BM_PL_MAX && !m->pl_nah && m->deck_form <= BF_BM_DECK_MAX;
}

/* Kameras des Wagens fuer ein Motiv bei (x,z):
 * - Ankerzone (erste Zone der Gruppe, FUN_80014324 sucht sie) enthaelt das Motiv: ROOM5090/5091 Cut 11 @0x47c, 12 @0x4a4, 13 @0x4e0,
 *   15 @0x544 (je x -5600..28100), Cut 14 @0x51c (x 17000..28050).
 * - Steht das Auge AUSSERHALB der Wechselstreifen des Wagens (z-Bereich aller Nicht-Anker-Zonen der Kandidaten, Wagen C z -28225..-18700),
 *   blickt die Kamera durch die Aussenwand (Cut 14, Auge z -29130 @0x220); sie zeigt nur ihren Anzeigebereich: x-Spanne ihrer Wechselstreifen
 *   (Cut 14: 14->13 @0x530 x 18502..20252, 13->14 @0x508 x 20250..21750), zur Wagenseite hin offen, wenn sie die aeusserste ist. Gemessen:
 *   die Form bei x 12044-14764 (Lauf n1_v F1624-F1656) und Leon bei x 17714 (Lauf n3_a F1125-F1179) sind in Cut 14 nicht zu sehen. */
static int bf_wagen_kameras(int32_t x, int32_t z, int out[8])
{
    int n = 0, kand[8], nk = 0;
    int32_t zmin = INT32_MAX, zmax = INT32_MIN, lo[8], hi[8], lo_min = INT32_MAX, hi_max = INT32_MIN;
    if (!g_room_rdt_ok) return 0;
    for (int i = 0; i < g_room_rdt.zone_count && nk < 8; i++) {
        const re15_rdt_zone_t *zn = &g_room_rdt.zones[i];
        if (i > 0 && g_room_rdt.zones[i - 1].cam_from == zn->cam_from) continue;     /* nur der Anker */
        if (zn->cam_from >= g_room_rdt.cut_count) continue;
        if (re15_aot_point_in_quad(x, z, zn->xs, zn->zs)) kand[nk++] = zn->cam_from;
    }
    for (int c = 0; c < nk; c++) {
        lo[c] = INT32_MAX; hi[c] = INT32_MIN;
        for (int i = 1; i < g_room_rdt.zone_count; i++) {
            const re15_rdt_zone_t *zn = &g_room_rdt.zones[i];
            if (g_room_rdt.zones[i - 1].cam_from != zn->cam_from) continue;           /* Anker auslassen */
            if (zn->cam_from != kand[c] && zn->cam_to != kand[c]) continue;
            for (int q = 0; q < 4; q++) {
                if (zn->xs[q] < lo[c]) lo[c] = zn->xs[q];
                if (zn->xs[q] > hi[c]) hi[c] = zn->xs[q];
                if (zn->zs[q] < zmin) zmin = zn->zs[q];
                if (zn->zs[q] > zmax) zmax = zn->zs[q];
            }
        }
        if (lo[c] <= hi[c]) { if (lo[c] < lo_min) lo_min = lo[c]; if (hi[c] > hi_max) hi_max = hi[c]; }
    }
    for (int c = 0; c < nk; c++) {
        const int32_t az = g_room_rdt.cuts[kand[c]].pos_z;
        if (zmin <= zmax && (az < zmin || az > zmax)) {                                  /* Auge ausserhalb des Wagens */
            const int32_t l = lo[c] == lo_min ? INT32_MIN : lo[c], h = hi[c] == hi_max ? INT32_MAX : hi[c];
            if (lo[c] > hi[c] || x < l || x > h) continue;
        }
        out[n++] = kand[c];
    }
    return n;
}

/* MESSSCHIENE RE15_BF_KAMLOG=<datei>: je Bild der Szene die Masse aller Wagenkameras (kein Spielverhalten). */
static void bf_kamlog(const char *wer, int gewaehlt, const int *k, const bf_bild_t *m, int n, const re15_actor_t *e)
{
    static FILE *fp = NULL; static int init = 0;
    if (!init) { const char *p = getenv("RE15_BF_KAMLOG"); init = 1; if (p && *p) fp = fopen(p, "w"); }
    if (!fp) return;
    fprintf(fp, "F%u %s wahl=%d form=(%d,%d) pl=(%d,%d)", (unsigned)g_engine.frame_count, wer, gewaehlt, (int)e->x, (int)e->z,
            (int)g_actors[RE15_ACTOR_SLOT_PLAYER].x, (int)g_actors[RE15_ACTOR_SLOT_PLAYER].z);
    for (int i = 0; i < n; i++)
        fprintf(fp, " | c%d ok=%d nah=%d hi=%d f=%d in=%d mi=%d p=%d pi=%d df=%d dp=%d pn=%d sz=%d", k[i], m[i].ok, m[i].nah, m[i].hinter,
                m[i].form, m[i].innen, m[i].mitte_in, m[i].pl, m[i].pl_in, m[i].deck_form, m[i].deck_pl, m[i].pl_nah,
                bf_bild_szene_gut(&m[i]));
    fputc('\n', fp); fflush(fp);
}

/* Wahl je Bild der Rueckstoss-Szene (Motiv Form), in dieser Reihenfolge die erste gute Kamera:
 *   (1) Original-Zonenkamera der FORMLAGE (re15_bf_schnitt_aus_lage auf die Form: der Schnitt, den der Raum fuer diese Stelle des Wagens
 *   zeigt), (2) der aktuelle Schnitt, (3) die Original-Zonenkamera der Spielerlage, (4) die gute Kamera mit dem groessten Formbild;
 *   in den BF_RS_KAM_HALT Bildern nach einem Schnitt tauschen (1) und (2) (Haltezeit gegen Flattern).
 * Gibt es keine, gilt die Original-Zonenkamera der Formlage (Auftrag NB4: wo der Raum fuer eine Lage keinen passenden Schnitt hat).
 * Die Nahsicht-Wache im Kampf nutzt das Bildmass seit Nachbesserung 4 Runde 2 nicht mehr (Abnahme 3 N1, s. bf_nahsicht_wache). */
static int bf_schnitt_waehlen(const re15_actor_t *e, int halten, const char *wer)
{
    const re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    int32_t pk[BF_BM_PUNKTE][3];
    int k[8], n, npk, wahl = -1, best = -1;
    const int cur = g_scd.cam_change_pending ? (int)g_scd.cam_id : (int)g_scd.work_vars[0x0A];
    const int zone_pl = re15_bf_schnitt_aus_lage(pl->x), zone_form = re15_bf_schnitt_aus_lage(e->x);
    /* halten: in den BF_RS_KAM_HALT Bildern nach einem Szenenschnitt geht der aktuelle Schnitt vor (gegen Flattern) */
    const int vorzug[3] = { halten ? cur : zone_form, halten ? zone_form : cur, zone_pl };
    bf_bild_t m[8];
    n = bf_wagen_kameras(e->x, e->z, k);
    if (n == 0) return -1;
    npk = bf_form_punkte(e, pk);
    for (int i = 0; i < n; i++) bf_bild_mass(k[i], pk, npk, k[i] == cur, &m[i]);
    for (int v = 0; v < 3 && wahl < 0; v++)
        for (int i = 0; i < n && wahl < 0; i++)
            if (k[i] == vorzug[v] && bf_bild_szene_gut(&m[i])) wahl = k[i];
    for (int i = 0; i < n && wahl < 0; i++)
        if (bf_bild_szene_gut(&m[i]))
            if (best < 0 || m[i].form > m[best].form) best = i;
    if (wahl < 0) wahl = best >= 0 ? k[best] : zone_form;
    bf_kamlog(wer, wahl, k, m, n, e);
    return wahl;
}

static void bf_kamera_zurueck(void)
{
    bf_kamera_setzen(re15_bf_schnitt_aus_lage(g_actors[RE15_ACTOR_SLOT_PLAYER].x));
    bf_kamera_frei();
}

static void bf_nahsicht_wache(void)
{
    const re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    int s = (re15_bf_bahnsteig_aktiv() && s_rs.slot < 0 && !re15_g5a_laeuft() && !re15_bb_pose_aktiv()) ? bf_riesenform_slot() : -1;
    const re15_actor_t *e = s >= 0 ? &g_actors[s] : NULL;
    /* (a) Auftritt in Cut 11 */
    if (e && e->state == 1 && e->sub_state_1 == 0 /* BB_AUFTRITT */ && e->motion == 10 /* BB_C_DROHEN */
        && e->anim_frame < BF_AUFTRITT_KAM_BIS) {
        bf_kamera(0x0B);
        s_kam_auftritt = 1;
        return;
    }
    if (s_kam_auftritt) { s_kam_auftritt = 0; bf_kamera_zurueck(); }
    /* (b) Gegenkamera, wenn die Form zwischen Auge und Spieler die Sicht fuellt (Achsenregel, Stand 2e3713c74, s. oben) */
    if (!e || e->state != 1) {
        if (s_kam_wache) { s_kam_wache = 0; bf_kamera_zurueck(); }
        return;
    }
    {
        int cur = (int)g_scd.work_vars[0x0A], k, ziel = -1;
        int32_t mitte;
        {   /* Kastenmitte (hit_offset_x laengs der Gier) als Lage der Masse */
            const int32_t v[3] = { e->hit_offset_x, 0, 0 };
            int32_t w[3];
            re15_skel_bone_to_world(v, e->rot_y, e->x, e->y, e->z, w);
            mitte = w[0];
        }
        for (k = 0; k < 2; k++) {
            if (cur != k_gang_kam[k].cut && !(s_kam_wache && s_kam_wache == k_gang_kam[k ^ 1].cut + 1)) continue;
            {
                const int32_t form_achse = (mitte - k_gang_kam[k].auge_x) * k_gang_kam[k].blick;
                const int32_t pl_achse   = (pl->x - k_gang_kam[k].auge_x) * k_gang_kam[k].blick;
                const int32_t g_form = (mitte - k_gang_kam[k ^ 1].auge_x) * k_gang_kam[k ^ 1].blick;
                const int32_t g_pl   = (pl->x - k_gang_kam[k ^ 1].auge_x) * k_gang_kam[k ^ 1].blick;
                if (form_achse > -2000 && form_achse < BF_NAH_ACHSE && pl_achse > form_achse && g_pl >= 1500 && g_form > g_pl)
                    ziel = k_gang_kam[k ^ 1].cut;
            }
        }
        if (ziel >= 0) { bf_kamera(ziel); s_kam_wache = ziel + 1; s_kam_wache_halt = 30; return; }
        if (s_kam_wache) {
            if (s_kam_wache_halt > 0) { s_kam_wache_halt--; bf_kamera(s_kam_wache - 1); return; }
            s_kam_wache = 0; bf_kamera_zurueck();
        }
    }
}

/* Schnitt zur Spielerlage ROOM5090/5091, Abnahme 2. Die RVD-Zonen sind Streifen, sie loesen nur aus dem passenden Schnitt aus:
 * @0x490 11->12 x 2800..4200, @0x4b8 12->11 x 1203..2703, @0x4cc 12->13 x 14000..15400, @0x4f4 13->12 x 12249..13873, @0x508
 * 13->14 x 20250..21750, @0x530 14->13 x 18502..20252. Ein fester Cut 12 am Szenenende liess Leon oestlich von x 15400 ohne Zone
 * und ohne Bild (Lauf z_ende F2102). Cut_old (`lbu a0,0x3f7b(a0)` @0x8004033c, `sh a0,0xfe4(at)` @0x80040364) hilft hier
 * nicht: der Spieler bewegt sich waehrend der Szene frei, der gemerkte Schnitt passt dann nicht mehr. Die Grenzen liegen so, dass kein
 * Streifen des gewaehlten Schnitts sofort zurueckschaltet. */
int re15_bf_schnitt_aus_lage(int32_t x)
{
    if (x < 2750)  return 0x0B;
    if (x < 14000) return 0x0C;
    if (x < 20250) return 0x0D;
    return 0x0E;
}

int re15_bf_rueckstoss_phase(void) { return s_rs.slot >= 0 ? s_rs.phase : 0; }

void re15_bf_rueckstoss_start(int slot)
{
    if (s_rs.slot >= 0) return;
    s_rs.slot = slot; s_rs.phase = 1; s_rs.t = 0;
    re15_actor_t *e = &g_actors[slot];
    e->hp = 1;                                           /* bleibt "am Ende", stirbt aber nicht */
    if (re15_bb_bank_da(e)) re15_bb_taumeln_start(slot);  /* Nachbesserung 1 M5: Riesenform taumelt (Clip 13) */
    else bf_zucken(e);
    {   /* Nachbesserung 4 (M-A): Schnitt nach der Bildlage statt fest Cut 12 (bf_schnitt_waehlen); ohne RID/RVD wie bisher Cut 12 */
        const int w = re15_bb_bank_da(e) ? bf_schnitt_waehlen(e, 0, "rs") : -1;
        bf_kamera(w >= 0 ? w : 0x0C);
        s_rs_kam_halt = BF_RS_KAM_HALT;
    }
}

uint16_t re15_bf_pad(uint16_t pad, uint16_t *pressed)
{
    if (s_rs.slot < 0 || s_rs.phase != 1) return pad;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    re15_actor_t *e  = &g_actors[s_rs.slot];
    pl->rot_y = bf_blick(pl, e->x, e->z);                /* Blick auf Birkin */
    if (pressed) *pressed = (uint16_t)(((s_rs.t % 20) == 0) ? 0x8000u : 0u);   /* Quadrat-Flanke: Nachladen */
    return (uint16_t)(0x0800u | 0x8000u);                /* RE15_PAD_BIT_R1 | RE15_PAD_BIT_SQUARE, keine Richtung */
}

/* UEBERGABEPUNKT Riesenform -> G5 (Stufe 3, analysis/befunde_runde41/form/STUFE3.md Abschnitt 4).
 * Wird genau einmal aufgerufen, wenn die Form den Wagen verlassen hat: mit der Riesenform am Ende ihres Sturz-Clips 12, ohne sie
 * (EM30 per RE15_BIRKIN_FORM=original) am Wagenende. Danach ruft Phase 2 (case 2 in der Rueckstoss-Szene, ohne Pause) einmal
 * re15_bf_g5_auftritt_start (g5auftritt_r41.c, RE2 room7040 sub06 @0x018DE). Ab da besitzt der G5-Auftritt (Spur G5A) Kamera,
 * Typwechsel re15_bf_morph_zu_g5 und Spielersperre; die Nahsicht-Wache dieser Datei schweigt (Gate !re15_g5a_laeuft() in
 * bf_nahsicht_wache). Die fruehere Kamera-Halte s_kam_g5 gibt es seit Commit 6d8e65214 nicht mehr. */
static void bf_rs_form_ist_draussen(re15_actor_t *e, int mit_gebruell)
{
    s_rs.phase = 2; s_rs.t = 0;
    e->no_draw = 1;
    if (mit_gebruell) re15_audio_room_se(10);            /* Gebruell (Birkin-Todes-SE 10, @0x8011a744) */
}

/* Nachbesserung 1 (PORT-WAHL), Riesenform in der Rueckstoss-Szene, Phase 1:
 *  M5: kein Gleiten mehr (Abnahme 1: 147 je Bild auf dem stehenden Zucken-Clip 8). Die Form taumelt rueckwaerts: Clip 13 im Kreis,
 *      Vortrieb aus seiner Spur (-80 je Bild, Fuesse in der Welt verriegelt), Ruecken zum Wagenende - die Gier lenkt mit 64 je Bild
 *      auf den Punkt, der dem Ausgang gegenueberliegt (Gestaltung). Kamera Cut 12 (Kampfkamera, wie bisher).
 *  M1: der Sturz (Clip 12) beginnt IM Wagen bei Wurzel x <= BF_RS_STURZ_X, wo Cut 11 die Form ganz zeigt (Abnahme 1 M1, Messhaken in
 *      Cut 11: x -3000 ganz, -5000 halb, -6300 nicht sichtbar). Die Clip-Spur (Bild 0-30 zusammen -3270) traegt sie zur Tuer (-5683),
 *      dort kippt sie hinaus. Ist die Grenze vor BF_RS_MIN_TICKS erreicht, zuckt sie dort am Platz (Clip 8 ab Bild 5, @0x80107224). */
#define BF_RS_STURZ_X  (-2500)        /* Gestaltung = BF_RS_ZUGENDE_KAM */
#define BF_RS_TAUMEL_KAM_X  3000      /* Gestaltung: Cut 11 ab hier (Form >= 4600 vor dem Auge x 7626) */
static void bf_rs_riesenform_phase1(re15_actor_t *e, const re15_actor_t *pl)
{
    (void)pl;
    if (e->x <= BF_RS_STURZ_X) {
        bf_kamera(0x0B);
        if (s_rs.t >= BF_RS_MIN_TICKS) {
            s_rs.t = 0;
            s_rs.phase = 3;
            re15_bb_sturz_start(s_rs.slot);
            return;
        }
        if (e->motion != 8) bf_zucken(e);
        else if (re15_r38_birkin_anim(e)) bf_zucken(e);
        return;
    }
    {   /* Ruecken zum Ausgang: Ziel = Spiegelpunkt des Ausgangs an der Wurzel */
        const int32_t ax = BF_RS_AUSGANG_X - 900, az = BF_RS_AUSGANG_Z;
        re15_enemy_steer_point(e, 2 * e->x - ax, 2 * e->z - az, 64);
    }
    re15_bb_taumeln_tick(s_rs.slot);
    /* Kamera: bis Nachbesserung 3 fest Cut 12 (Kampfkamera, Auge x 18526; im Debug-Sprung-Weg die Form 12000-21000 entfernt, hinter Leon,
     * Lauf n1 l5090_feuer Bild 372-420; im echten Wagenweg nicht, Abnahme 2 M-A). Ab x <= BF_RS_TAUMEL_KAM_X zeigt Cut 11 (Auge x 7626, Blick -x, Kamera von Bild 2) sie von vorn, >= 4600 vor dem
     * Auge, wie sie zum Wagenende zurueckgedraengt wird (PORT-WAHL). */
    if (e->x <= BF_RS_TAUMEL_KAM_X) bf_kamera(0x0B);
    else {   /* Nachbesserung 4 (M-A): bis dahin der Schnitt nach der Bildlage (bf_schnitt_waehlen), ohne RID/RVD Cut 12 */
        const int cur = g_scd.cam_change_pending ? (int)g_scd.cam_id : (int)g_scd.work_vars[0x0A];
        const int w = bf_schnitt_waehlen(e, s_rs_kam_halt > 0, "rs");
        if (w >= 0 && w != cur) s_rs_kam_halt = BF_RS_KAM_HALT;
        else if (s_rs_kam_halt > 0) s_rs_kam_halt--;
        bf_kamera(w >= 0 ? w : 0x0C);
    }
}

static void bf_rueckstoss_tick(void)
{
    if (s_rs.slot < 0) return;
    re15_actor_t *e = &g_actors[s_rs.slot];
    if (!e->active || !re15_bf_bahnsteig_aktiv()) { s_rs.slot = -1; bf_kamera_frei(); return; }
    s_rs.t++;
    e->state = 2; e->hp = 1;                             /* Treffer aendern nichts mehr am Ablauf */
    switch (s_rs.phase) {
    case 1: {
        if (re15_bb_bank_da(e)) {                        /* Riesenform: Nachbesserung 1 M1/M5 */
            const re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
            bf_rs_riesenform_phase1(e, pl);
            break;
        }
        if (re15_r38_birkin_anim(e)) bf_zucken(e);       /* Zucken bei jedem Treffer: Clip im Kreis */
        int32_t dx = BF_RS_AUSGANG_X - 900 - e->x, dz = BF_RS_AUSGANG_Z - e->z;
        int32_t rest = (int32_t)(dx < 0 ? -dx : dx) + (dz < 0 ? -dz : dz);
        int32_t ticks = BF_RS_MIN_TICKS - s_rs.t; if (ticks < 20) ticks = 20;
        int32_t schritt = rest / ticks; if (schritt < 40) schritt = 40;    /* Gestaltung */
        if (rest > 0) {
            e->x += (int32_t)((int64_t)dx * schritt / rest);
            e->z += (int32_t)((int64_t)dz * schritt / rest);
        }
        bf_kamera(e->x < BF_RS_ZUGENDE_KAM ? 0x0B : 0x0C);
        /* rueckwaerts: Gesicht zum Spieler */
        e->rot_y = bf_blick(e, g_actors[RE15_ACTOR_SLOT_PLAYER].x, g_actors[RE15_ACTOR_SLOT_PLAYER].z);
        if (e->x <= BF_RS_AUSGANG_X - 600 && s_rs.t >= BF_RS_MIN_TICKS) {
            s_rs.t = 0;
            if (re15_bb_bank_da(e)) {                    /* Riesenform: faellt rueckwaerts aus dem Wagen (Clip 12, Stufe 3) */
                s_rs.phase = 3;
                re15_bb_sturz_start(s_rs.slot);
            } else bf_rs_form_ist_draussen(e, 1);
        }
        break;
    }
    case 3:                                              /* PORT-WAHL: Sturz-Clip am Zugende, Blick Cut 11 (= Bild 2) */
        bf_kamera(0x0B);
        if (re15_bb_sturz_tick(s_rs.slot)) bf_rs_form_ist_draussen(e, 0);   /* Gebruell traegt Clip 12 Bild 2 */
        break;
    case 2: {                                            /* die Form ist aus dem Zug: ab hier die G5-Szene (re15_g5auftritt_r41.h) */
        /* SCHNITTSTELLE zur Form-Spur: re15_bf_g5_auftritt_start = RE2 sub06 @0x018DE (Armierung). RE2 hat zwischen dem Verschwinden
         * der Masse und der Armierung keine Pause; die Pause BF_RS_PAUSE vor dem Typwechsel ist damit weg. */
        int slot = s_rs.slot;
        s_rs.slot = -1; s_rs.phase = 0;
        e->no_draw = 0;
        re15_bf_g5_auftritt_start(slot);
        break;
    }
    default: break;
    }
}
