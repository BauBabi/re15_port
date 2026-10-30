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
#include <stdlib.h>
#include <string.h>

void re15_g5_boss_spawn(int slot);                       /* enemy_ai_boss_g5.c: Ctor faellig */
int  re15_r38_birkin_anim(re15_actor_t *e);              /* enemy_ai_common.c: Clip-Vorschub des Typs */
int  re15_r38_birkin_fc(const re15_actor_t *e);          /* enemy_ai_common.c: Clip-Laenge des Typs */

/* ---- Modellwahl --------------------------------------------------------------------------- */
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
        if (!strcmp(form, "bahnsteig")) return "EMD/EM30_BAHNSTEIG.EMD";
    }
    if (re15_bf_hangar_aktiv())    return "EMD/EM30_HANGAR.EMD";
    if ((g_current_room_id & 0xFFFEu) == 0x5090u) return "EMD/EM30_BAHNSTEIG.EMD";
    return NULL;
}

int re15_bf_bahnsteig_aktiv(void)
{
    return (g_current_room_id & 0xFFFEu) == 0x5090u && !getenv("RE15_ENDKAMPF_NUR_G5");
}

int re15_bf_hangar_aktiv(void)
{
    return (g_current_room_id & 0xFFFEu) == 0x50A0u && !getenv("RE15_KEIN_HANGAR_BIRKIN");
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
static struct { int slot, phase, t; } s_rs = { -1, 0, 0 };   /* Rueckstoss-Szene 5090, s. unten */   /* einmal je Sitzung; Birkin selbst steht bei jedem Betreten */

void re15_bf_reset(void)
{
    s_rs.slot = -1; s_rs.phase = 0; s_rs.t = 0;
    s_h.raum = 0; s_h.birkin = s_h.sm = -1; s_h.phase = 0; s_h.t = 0; s_h.birkin_start = 0;
}

int re15_bf_hangar_phase(void) { return s_h.phase; }
int re15_bf_hangar_slot(int birkin) { return birkin ? s_h.birkin : s_h.sm; }

static void bf_rueckstoss_tick(void);
int re15_bf_szene_besitzt(int slot)
{
    if (slot >= 0 && slot == s_rs.slot) return 1;       /* Rueckstoss-Szene 5090 */
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
    /* MESSHAKEN RE15_BIRKIN_HP_AT=<n>: n KI-Ticks nach Raumeintritt in 5090 die HP des 0x30 auf 0 (Sichtpruefung
     * der Rueckstoss-Szene ohne Durchspielen). Im Normalpfad stumm. */
    { static int at = -2; static unsigned raum = 0; static int t = 0;
      if (at == -2) { const char *v = getenv("RE15_BIRKIN_HP_AT"); at = v ? atoi(v) : -1; }
      if (at >= 0 && re15_bf_bahnsteig_aktiv()) {
          if (raum != g_current_room_id) { raum = g_current_room_id; t = 0; }
          if (++t == at)
              for (int i = 1; i < RE15_ACTOR_MAX; i++)
                  if (g_actors[i].active && g_actors[i].type == 0x30u && g_actors[i].state == 1) {
                      g_actors[i].hp = 0; g_actors[i].state = 3; g_actors[i].sub_state_3 = 0; }   /* = Todes-Commit nach Treffer */
      } }
    bf_rueckstoss_tick();
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
        b->hp = 300;                                     /* INIT-Wert @0x8011690c, laeuft nach der Szene erneut */
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

/* ---- Bahnsteig: Rueckstoss-Szene Vierbein -> G5 -------------------------------------------
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

int re15_bf_rueckstoss_phase(void) { return s_rs.slot >= 0 ? s_rs.phase : 0; }

void re15_bf_rueckstoss_start(int slot)
{
    if (s_rs.slot >= 0) return;
    s_rs.slot = slot; s_rs.phase = 1; s_rs.t = 0;
    re15_actor_t *e = &g_actors[slot];
    e->hp = 1;                                           /* bleibt "am Ende", stirbt aber nicht */
    bf_clip(e, 7); e->anim_frac = 0xf;
    /* Kampfkamera: ROOM5090 sub04 Cut_chg(0x0C) @0x1342 - sieht Spieler und Zugende in einem Bild */
    g_scd.cam_id = 0x0C; g_scd.cam_change_pending = 1;
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

static void bf_rueckstoss_tick(void)
{
    if (s_rs.slot < 0) return;
    re15_actor_t *e = &g_actors[s_rs.slot];
    if (!e->active || !re15_bf_bahnsteig_aktiv()) { s_rs.slot = -1; return; }
    s_rs.t++;
    e->state = 2; e->hp = 1;                             /* Treffer aendern nichts mehr am Ablauf */
    switch (s_rs.phase) {
    case 1: {
        if (re15_r38_birkin_anim(e)) bf_clip(e, 7);      /* Zucken bei jedem Treffer: Clip 7 im Kreis */
        int32_t dx = BF_RS_AUSGANG_X - 900 - e->x, dz = BF_RS_AUSGANG_Z - e->z;
        int32_t rest = (int32_t)(dx < 0 ? -dx : dx) + (dz < 0 ? -dz : dz);
        int32_t ticks = BF_RS_MIN_TICKS - s_rs.t; if (ticks < 20) ticks = 20;
        int32_t schritt = rest / ticks; if (schritt < 40) schritt = 40;    /* Gestaltung */
        if (rest > 0) {
            e->x += (int32_t)((int64_t)dx * schritt / rest);
            e->z += (int32_t)((int64_t)dz * schritt / rest);
        }
        if ((int)g_scd.work_vars[0x0A] != 0x0C) { g_scd.cam_id = 0x0C; g_scd.cam_change_pending = 1; }
        /* rueckwaerts: Gesicht zum Spieler */
        e->rot_y = bf_blick(e, g_actors[RE15_ACTOR_SLOT_PLAYER].x, g_actors[RE15_ACTOR_SLOT_PLAYER].z);
        if (e->x <= BF_RS_AUSGANG_X - 600 && s_rs.t >= BF_RS_MIN_TICKS) {
            s_rs.phase = 2; s_rs.t = 0;
            e->no_draw = 1;
            re15_audio_room_se(10);                      /* Gebruell (Birkin-Todes-SE 10, @0x8011a744) */
        }
        break;
    }
    case 2:
        if (s_rs.t >= BF_RS_PAUSE) {
            int slot = s_rs.slot;
            s_rs.slot = -1; s_rs.phase = 0;
            e->no_draw = 0;
            re15_bf_morph_zu_g5(slot);
        }
        break;
    default: break;
    }
}
