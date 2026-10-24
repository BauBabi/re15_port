/*
 * RE1.5 Rebuilt — Runde 41 Spur E "kleinkram_b", Nachbesserung 3 (E-E2): Renn-Tempo (ANIMATE 0x80032778 Fall 0/1,
 * FUN_800369f8 Modus 1) und Renn-Stopp (DECIDE 0x80032604, ANIMATE Fall 2/3). Herleitung, Messung und Port-Grenzen:
 * include/re15_rennstopp_r41.h; Dossier analysis/befunde_runde41/E_kleinkram_b.md ("Nachbesserung 3").
 */
#include "re15_rennstopp_r41.h"

#include <string.h>

#include "re15_player.h"     /* RE15_PAD_BIT_*, re15_player_w_skel / re15_player_w_anim */
#include "re15_skeleton.h"   /* re15_sin_q12 / re15_cos_q12 / re15_skel_euler_matrix (RotMatrix) */
#include "re15_math.h"       /* re15_squareroot0 = SquareRoot0 0x80065f60 */

#define RS_ABZUG     40      /* @0x8003293c `addiu v0,v0,-40` */
#define RS_MISCHUNG  7       /* @0x800327f4 / @0x80032908 `ori v0,zero,0x7` -> +0x8f (Fall 0 / Fall 2) */
#define RS_GEWICHT   0x200   /* @0x80032888 / @0x8003292c `ori a3,zero,0x200` = anim_set a3 (Gewicht je Zaehlerstufe) */
#define RS_SOCKEL    150     /* @0x800328ec `addiu v0,v0,150` */
#define RS_TEILE_MIN 8       /* Kette 0,1,2..4 / 5..7 (@0x80036a48-@0x80036a9c) */
#define RS_FALL3     3       /* @0x800328f8 `ori v0,zero,0x3` / @0x800328fc `sb v0,0(a1)`, a1 = 0x800aca5a (@0x8003277c/
                              * @0x80032780) -> Spieler +6 = 3 ab dem ersten Stopp-Bild (Nachbesserung 4, Uebernahme-Marke) */

/* ---------------------------------------------------------------------------------------------- Zustand */
static int     s_aktiv;      /* Stopp: +6 >= 2 im Unterzustand Rennen */
static int     s_takt_offen; /* NB4: der letzte ungesperrte Dispatcher-Durchlauf wartet auf den Kommando-1-Schritt */
static int     s_steh;       /* die Bewegung, die der Stopp gesetzt hat (Fremdeingriff erkennen) */
static int32_t s_tempo;      /* Spieler +0x8c (DAT_800acae0), int16 */
static int     s_tempo_ok;   /* ein Renn-Tempo wurde gerechnet bzw. aus Fall 0 gesetzt */
static int16_t s_winkel[RE15_EMD_MAX_BONES][3];   /* Teil+0x60 je Teil = letzte Pose aus anim_set (FUN_8001f3bc) */
static int32_t s_wurzel[3];                        /* Wurzel-Kopie +0x38/+0x3a/+0x3c (int16) */
static int     s_pose_ok;
static int     s_stand_neu;  /* Stopp endete im letzten Bild (Kommandowort 0x0001 @0x80032958): Stehen-Fall 0 steht an */
static int     s_pruef_aus;  /* NUR Sonden-Gegenprobe (re15_rennstopp_r41_pruef_aus), kein Spielweg */
static int     s_pruef_nb4;  /* NUR Riegel-Gegenprobe (re15_rennstopp_r41_pruef_uebernahme_aus): Uebernahme wie vor NB4 */

/* ---------------------------------------------------------------------------------------------- Fusskette */
typedef struct { int32_t m[9]; int32_t t[3]; } rs_mat_t;

static int32_t rs_ir(int64_t v)   /* IR1..3 bei lm = 0: -0x8000..0x7fff */
{
    return v < -32768 ? -32768 : (v > 32767 ? 32767 : (int32_t)v);
}

/* FUN_80022da0: Rotation je Spalte MVMVA sf=1 (@0x80022df0/@0x80022e38/@0x80022e84, IR gesaettigt), Translation
 * MVMVA mit TR (@0x80022eec): o.t = a.t + (a.m * (s16)b.t) >> 12 (MAC1..3 @0x80022ef8-@0x80022f00). */
static void rs_verkette(const rs_mat_t *a, const rs_mat_t *b, rs_mat_t *o)
{
    rs_mat_t r;
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++) {
            int64_t s = 0;
            for (int k = 0; k < 3; k++) s += (int64_t)a->m[i * 3 + k] * (int64_t)b->m[k * 3 + j];
            r.m[i * 3 + j] = rs_ir(s >> 12);
        }
    for (int i = 0; i < 3; i++) {
        int64_t s = 0;
        for (int k = 0; k < 3; k++) s += (int64_t)a->m[i * 3 + k] * (int64_t)(int16_t)b->t[k];
        r.t[i] = a->t[i] + (int32_t)(s >> 12);
    }
    *o = r;
}

/* Fusslage (Translation der Kette Entity * Teil 0 * Teil 1 * Teil 2+3f * Teil 3+3f * Teil 4+3f, @0x80036a30-@0x80036aa0).
 * Entity-Matrix = RotMatrix(0, Gier, 0) (FUN_80068098 @0x80036a34), Translation 0: die Lage kuerzt sich in der Differenz. */
static void rs_fuss(const re15_emd_skeleton_t *sk, int16_t (*w)[3], const int32_t wurzel[3], int fuss, int gier,
                    int32_t out[3])
{
    rs_mat_t M, L;
    re15_skel_euler_matrix(0, gier, 0, M.m);
    M.t[0] = M.t[1] = M.t[2] = 0;
    const int kette[5] = { 0, 1, 2 + 3 * fuss, 3 + 3 * fuss, 4 + 3 * fuss };
    for (int i = 0; i < 5; i++) {
        const int b = kette[i];
        re15_skel_euler_matrix(w[b][0], w[b][1], w[b][2], L.m);          /* RotMatrix(Teil+0x60) in FUN_8001f3bc */
        if (b == 0) { L.t[0] = wurzel[0]; L.t[1] = wurzel[1]; L.t[2] = wurzel[2]; }   /* Wurzel Teil 0 +0x2c..+0x34 */
        else {
            L.t[0] = sk->bone_relative_pos[b][0];
            L.t[1] = sk->bone_relative_pos[b][1];
            L.t[2] = sk->bone_relative_pos[b][2];
        }
        rs_verkette(&M, &L, &M);
    }
    out[0] = M.t[0]; out[1] = M.t[1]; out[2] = M.t[2];
}

/* FUN_80020510 + LoadAverageShort12 @0x800664e0: vorige &= 0xfff (@0x80020520/@0x80020524), kf auf +-0x800 um die vorige
 * gefaltet (@0x80020538-@0x8002055c), dann GPF (vorige * w) >> 12 (@0x80066508) + GPL (kf * (0x1000 - w)) >> 12
 * (@0x80066538) — zwei Abrundungen. */
static int16_t rs_winkel_misch(int vor, int kf, int w)
{
    vor &= 0xfff;
    const unsigned d = (unsigned)(uint16_t)((kf - vor) + 0x800);
    if (d > 0x1000u) kf = (int16_t)(kf + ((d & 0x8000u) ? 0x1000 : -0x1000));
    kf = (int16_t)kf;
    return (int16_t)(((vor * w) >> 12) + ((kf * (0x1000 - w)) >> 12));
}

/* anim_set -> FUN_8001f3bc fuer die Teile: Zaehler 0 -> kf-Pose uebernehmen; sonst Wurzel GPF12(kf, 0x1000-w) +
 * GPL12(vorige, w) und je Teil FUN_80020510 (w = 0x200 * Zaehler, Zaehler VOR dem Abzaehlen). */
static void rs_pose(const re15_emd_skeleton_t *sk, int kf, int zaehler)
{
    int16_t px = 0, py = 0, pz = 0;
    re15_emd_get_keyframe_position(sk, kf, &px, &py, &pz);
    const int32_t k[3] = { px, py, pz };
    const int w = RS_GEWICHT * zaehler;
    for (int a = 0; a < 3; a++)
        s_wurzel[a] = zaehler ? (int16_t)(((k[a] * (0x1000 - w)) >> 12) + (((int32_t)(int16_t)s_wurzel[a] * w) >> 12))
                              : k[a];
    for (int b = 0; b < sk->bone_count; b++) {
        int16_t ax = 0, ay = 0, az = 0;
        re15_emd_get_keyframe_angles(sk, kf, b, &ax, &ay, &az);
        if (!zaehler) { s_winkel[b][0] = ax; s_winkel[b][1] = ay; s_winkel[b][2] = az; }
        else {
            s_winkel[b][0] = rs_winkel_misch(s_winkel[b][0], ax, w);
            s_winkel[b][1] = rs_winkel_misch(s_winkel[b][1], ay, w);
            s_winkel[b][2] = rs_winkel_misch(s_winkel[b][2], az, w);
        }
    }
}

int re15_rennstopp_r41_fussrechnung(const re15_emd_skeleton_t *sk, const re15_emd_animation_t *an, int clip, int bild,
                                    int mischung, int gier_vorher, int gier, int neu,
                                    const int16_t (*vor_winkel)[3], const int32_t *vor_wurzel, int32_t *tempo, int *roh)
{
    if (!sk || !an || clip < 0 || clip >= an->clip_count) return 0;
    if (sk->bone_count < RS_TEILE_MIN || sk->bone_count > RE15_EMD_MAX_BONES || sk->keyframe_count <= 0) return 0;
    const int fc = an->clips[clip].frame_count;
    if (fc <= 0 || bild < 0) return 0;
    const uint32_t fe = re15_emd_get_frame_entry(an, clip, bild % fc);
    if (fe & 0x8000u) return 0;                     /* Tween-Bild (FUN_8001f8b4) — der Renn-Clip hat keins (Riegel) */
    int kf = (int)(fe & 0xfffu);
    if (kf >= sk->keyframe_count) kf = sk->keyframe_count - 1;
    if (neu || !s_pose_ok) {                        /* vorige Pose = das zuletzt gezeichnete Bild */
        if (vor_winkel && vor_wurzel) {
            for (int b = 0; b < sk->bone_count; b++) {
                s_winkel[b][0] = vor_winkel[b][0]; s_winkel[b][1] = vor_winkel[b][1]; s_winkel[b][2] = vor_winkel[b][2];
            }
            s_wurzel[0] = (int16_t)vor_wurzel[0]; s_wurzel[1] = (int16_t)vor_wurzel[1]; s_wurzel[2] = (int16_t)vor_wurzel[2];
        } else {
            rs_pose(sk, kf, 0);
        }
        s_pose_ok = 1;
    }
    const int fuss = (fe & 0x2000u) ? 0 : 1;        /* @0x800328b4 `andi 0x2000` -> a1 = 0 (@0x800328c4), sonst 1 (@0x800328c8) */
    int32_t f_vor[3], f_neu[3];
    rs_fuss(sk, s_winkel, s_wurzel, fuss, gier_vorher, f_vor);   /* Teil+0x40: Weltmatrix aus dem letzten Zeichnen */
    rs_pose(sk, kf, mischung);                                     /* anim_set @0x80032884 */
    rs_fuss(sk, s_winkel, s_wurzel, fuss, gier, f_neu);           /* FUN_800369f8 @0x80036a30-@0x80036aa0 */
    const int64_t dx = (int16_t)(f_neu[0] - f_vor[0]);           /* lhu/subu/sll 16/sra 16 @0x80036af0-@0x80036b04 */
    const int64_t dz = (int16_t)(f_neu[2] - f_vor[2]);           /* @0x80036b0c-@0x80036b24 */
    const int16_t r = (int16_t)re15_squareroot0((uint32_t)(dx * dx + dz * dz));   /* @0x80036b30, sh +0x8c @0x80036b44 */
    if (roh) *roh = r;
    if (tempo) *tempo = (int16_t)((r >> 2) + RS_SOCKEL);        /* @0x800328dc-@0x800328f4 */
    return 1;
}

int32_t re15_rennstopp_r41_lauftempo(re15_actor_t *p, int erstes_bild, int gier_vorher, int32_t tabelle)
{
    if (!p) return tabelle;
    if (s_pruef_aus) return tabelle;                /* Sonden-Gegenprobe: Stand vor Nachbesserung 3 */
    s_aktiv = 0;                                    /* Rennen = Unterzustand 1, kein Stopp */
    if (erstes_bild || !s_tempo_ok) { s_tempo = tabelle; s_tempo_ok = 1; }   /* Fall 0 @0x800327cc-@0x80032830 */
    int32_t t = 0;
    const int ok = re15_rennstopp_r41_fussrechnung(re15_player_w_skel(), re15_player_w_anim(), 0,
                                                   erstes_bild ? 0 : (int)p->anim_frame,
                                                   erstes_bild ? RS_MISCHUNG : (int)p->anim_frac,
                                                   gier_vorher, (int)p->rot_y, erstes_bild,
                                                   p->anim_prev_valid ? (const int16_t (*)[3])p->prev_angles : NULL,
                                                   p->anim_prev_valid ? p->prev_root : NULL, &t, NULL);
    if (ok) s_tempo = t;                            /* Fall 1 @0x800328cc-@0x800328f4 */
    return s_tempo;
}

/* ---------------------------------------------------------------------------------------------- Renn-Stopp */
static void vorschub(re15_actor_t *p)
{
    /* @0x8003295c `jal 0x800245d8` (a0 = 0): Vorschub um das Tempo in Blickrichtung — derselbe Weg wie Gehen/Rennen
     * im Port (player_common.c: x += cos*v/4096, z += -sin*v/4096). */
    const int32_t s = re15_sin_q12((int)p->rot_y), c = re15_cos_q12((int)p->rot_y);
    p->x += (c * s_tempo) / 4096;
    p->z += (-s * s_tempo) / 4096;
}

static int schritt(re15_actor_t *p, uint16_t pad_bits, int lauf_motion, int steh_motion, int32_t tabelle, int waffe)
{
    if (s_pruef_aus) return 0;                        /* Sonden-Gegenprobe: Stand vor Nachbesserung 3 (sofort Stehen) */
    s_takt_offen = 0;                                 /* NB4: dieser Durchlauf lief durch Kommando 1 (0x80031de8) */
    if (s_stand_neu) {
        /* Stehen-ANIMATE 0x80032038 Fall 0 im Bild NACH dem Stopp-Ende (@0x80032074-@0x8003209c): Clip 3 bleibt,
         * Bild +0x95 = 0, Mischung +0x8f = 7. Waehlt der Aufrufer in diesem Bild eine neue Bewegung (Stehen-DECIDE
         * 0x80031f38), setzt er Bild/Mischung ohnehin selbst. */
        s_stand_neu = 0;
        if (!s_aktiv && p->motion == s_steh && p->state == 1) {
            p->anim_frame        = 0;
            p->motion_init_delay = 0;
            p->anim_frac         = RS_MISCHUNG;       /* @0x8003208c/@0x8003209c `sb 7,+0x8f` */
        }
    }
    /* Skript/Treffer hat uebernommen. NB4: auch wenn ein anderer Zustand +6 neu geschrieben hat (Plc_ret @0x80041f98,
     * Plc_motion @0x80041bb4, Raumeintritt, Treffer, Flinch-Ende) — der Stopp lebt im Original nur in +5 = 2 / +6 = 3. */
    if (s_aktiv && (p->motion != s_steh || p->state != 1 || (p->sub_state_2 != RS_FALL3 && !s_pruef_nb4))) s_aktiv = 0;
    if (!s_aktiv) {
        if (p->motion != lauf_motion || p->state != 1) return 0;          /* nur aus dem Rennen (+6 < 2) */
        const int oben_rennen = (pad_bits & RE15_PAD_BIT_UP) && (pad_bits & RE15_PAD_BIT_CROSS);   /* & 0x201 == 0x201 */
        const int zielen      = (pad_bits & RE15_PAD_BIT_R1) && waffe;                                /* 0x100 + Waffe */
        if (oben_rennen && !zielen) return 0;                                                         /* weiter rennen */
        s_aktiv = 1;                                  /* @0x8003272c +6 = 2; ANIMATE Fall 2 im selben Bild: */
        if (!s_tempo_ok) { s_tempo = tabelle; s_tempo_ok = 1; }   /* sonst das letzte Renn-Tempo (+0x8c bleibt stehen) */
        s_steh  = steh_motion;
        p->motion            = (int16_t)steh_motion;  /* @0x80032900/@0x80032904 Clip 3 (+0x94) */
        p->anim_frame        = 0;
        p->motion_init_delay = 0;
        p->anim_frac         = RS_MISCHUNG;           /* @0x80032908/@0x80032910 +0x8f = 7 */
        p->sub_state_2       = RS_FALL3;              /* @0x800328fc +6 = 3 (NB4: Marke, die fremde Schreiber loeschen) */
    } else {
        /* @0x80032730-@0x80032760: OBEN -> Gehen 0x0101 (@0x80032738, auch mit RENNEN: Rennen erst aus dem Gehen-DECIDE
         * des Folgebilds @0x80032444-@0x80032454), UNTEN -> Rueckwaerts 0x0401 (@0x80032744), LINKS/RECHTS -> Drehen
         * 0x0301 (@0x80032750). Der Dispatcher liest +5 nach dem DECIDE neu (@0x80031efc): das neue ANIMATE laeuft in
         * DIESEM Bild, der Aufrufer waehlt die Bewegung jetzt. */
        if (pad_bits & RE15_PAD_BIT_UP) {
            s_aktiv = 0;
            p->sub_state_2 = 0;                       /* Wort 0x0101 @0x80032760 `sw`: +6 = 0 */
            return 2;                                 /* Gehen erzwingen */
        }
        if (pad_bits & (RE15_PAD_BIT_DOWN | RE15_PAD_BIT_LEFT | RE15_PAD_BIT_RIGHT)) {
            s_aktiv = 0;
            p->sub_state_2 = 0;                       /* Wort 0x0401/0x0301 @0x80032760 `sw`: +6 = 0 */
            return 0;
        }
    }
    s_tempo = (int16_t)(s_tempo - RS_ABZUG);          /* @0x80032930-@0x80032944 */
    if (s_tempo < 0) {                                /* @0x8003294c `bgez` / @0x80032958 Kommandowort 0x0001 -> Stehen */
        s_aktiv = 0; s_stand_neu = 1;
        p->sub_state_2 = 0;                           /* `sw 1` @0x80032958 = +4..+7 = 1,0,0,0 */
    }
    vorschub(p);                                      /* auch der letzte, negative Schritt */
    return 1;
}

int re15_rennstopp_r41_takt(re15_actor_t *p, uint16_t pad_bits, int lauf_motion, int steh_motion, int32_t tabelle,
                            int waffe)
{
    if (!p) return 0;
    return schritt(p, pad_bits, lauf_motion, steh_motion, tabelle, waffe);
}

int re15_rennstopp_r41_skript(re15_actor_t *p, int lauf_motion, int steh_motion, int32_t tabelle)
{
    if (!p) return 0;
    return schritt(p, 0, lauf_motion, steh_motion, tabelle, 0);
}

int re15_rennstopp_r41_unterzustand2(const re15_actor_t *p, int lauf_motion)
{
    if (!p || s_pruef_aus || p->state != 1) return 0;   /* nur Kommando 1 (0x80031de8) */
    if (s_aktiv && p->motion == s_steh) return 1;       /* Stopp: +6 >= 2 */
    return p->motion == lauf_motion;                    /* Rennen: +6 < 2 */
}

/* NB4 (Abnahme 3 Mangel 1, Entscheid E-E4): UEBERNAHME. Der Stopp lebt im Original ausschliesslich in den Spieler-Bytes
 * +5 = 2 / +6 = 3 unter Kommando 1 (DECIDE 0x80032604 @0x8003272c, ANIMATE 0x80032778 @0x800328fc). Jeder andere Zustand
 * schreibt sie neu, bevor er den Spieler zurueckgibt — die Treppe beim Start (sce 12 @0x80043500: `ori 0xb` + `sb v0,5(a1)`
 * @0x80043540/@0x80043544, +6 @0x80043594; sce 13 @0x800435cc: @0x8004360c/@0x80043610, +6 @0x80043660) und im Abschluss LAB_80038e50 (@0x80038ea8 `sb zero,0x800aca59`, @0x80038eb0
 * `sb zero,0x800aca5a`), Plc_ret (@0x80041f90-@0x80041f9c), Plc_motion (@0x80041bb0-@0x80041bc4), der Griff (STAGE1
 * FUN_80102548 @0x80102640 `sw v0,0x800aca58` = Wort Kommando 5; Freigabe z.B. @0x8011c38c `sw 1,0x800aca58`), Treffer/Tod
 * (@0x80012ef4-@0x80012efc). Der Port fuehrt diese Zustaende in eigenen
 * Zweigen des Spieler-Dispatchers (game_step_common.c: Klettern, Treppe, Tod, Griff, Knockdown, Flinch) bzw. im
 * Schiebe-FSM (Unterzustand 8, @0x80031340), die den Kommando-1-Schritt auslassen und die Bytes nicht schreiben. Darum:
 * laeuft ein UNGESPERRTER Durchlauf des Dispatchers ohne den Kommando-1-Schritt dieses Moduls, hatte ein anderer Zustand
 * den Spieler — der Stopp ist geloescht wie im Original. Gesperrte Durchlaeufe (`bltz` @0x80031c78 auf g_pauseflags,
 * Bit 0x80000000) lassen +4..+7 unberuehrt (Kurbel ROOM2020: Stopp im Aktionsbild, eingefroren waehrend der Fragen, Auslauf
 * nach Pos_set — o_renn_losl n94..n98, Abnahme 3 §3.2). Gerufen VOR der Zweig-Kette, laeuft = rdt_ok && kein Spieler-Bit. */
void re15_rennstopp_r41_dispatcher(re15_actor_t *p, int laeuft)
{
    if (!laeuft || s_pruef_aus || s_pruef_nb4) return;
    if (s_takt_offen) {
        s_aktiv = 0; s_stand_neu = 0;
        if (p && p->sub_state_2 == RS_FALL3) p->sub_state_2 = 0;   /* Port-Zweige schreiben +6 nicht selbst */
    }
    s_takt_offen = (s_aktiv || s_stand_neu);
}

/* NB4: Nachricht OHNE Spieler-Bit (player_common.c msg_block-Zweig): das Original laesst Kommando 1 dann weiterlaufen —
 * keine Uebernahme. Der Port haelt die Bewegung in diesem Zweig an (vorbestehend); der Stopp bleibt stehen wie vor NB4. */
void re15_rennstopp_r41_halten(void) { s_takt_offen = 0; }

void    re15_rennstopp_r41_pruef_aus(int aus) { s_pruef_aus = aus ? 1 : 0; s_aktiv = 0; s_stand_neu = 0; s_takt_offen = 0; }
void    re15_rennstopp_r41_pruef_uebernahme_aus(int aus) { s_pruef_nb4 = aus ? 1 : 0; s_takt_offen = 0; }
int     re15_rennstopp_r41_aktiv(void) { return s_aktiv; }
int32_t re15_rennstopp_r41_tempo(void) { return s_tempo; }
void    re15_rennstopp_r41_zuruecksetzen(void)
{
    s_aktiv = 0; s_tempo = 0; s_tempo_ok = 0; s_steh = 0; s_pose_ok = 0; s_stand_neu = 0; s_takt_offen = 0;
    memset(s_winkel, 0, sizeof s_winkel);
    memset(s_wurzel, 0, sizeof s_wurzel);
}
