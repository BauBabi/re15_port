/*
 * RE1.5 Rebuilt — Runde 41 Spur E "kleinkram_b", Nachbesserung 2: Renn-Stopp (Kommando 1 Unterzustand 2, DECIDE
 * 0x80032604 @0x8003272c / @0x80032730-@0x80032760, ANIMATE 0x80032778 Fall 2 @0x800328f8-@0x80032910, Fall 3
 * @0x80032918-@0x8003295c). Herleitung und Messung: include/re15_rennstopp_r41.h; Dossier
 * analysis/befunde_runde41/E_kleinkram_b.md ("Nachbesserung 2").
 */
#include "re15_rennstopp_r41.h"

#include "re15_player.h"     /* RE15_PAD_BIT_* */
#include "re15_skeleton.h"   /* re15_sin_q12 / re15_cos_q12 */

#define RS_ABZUG 40          /* @0x8003293c `addiu v0,v0,-40` */

static int     s_aktiv;      /* +6 >= 2 im Unterzustand Rennen */
static int32_t s_tempo;      /* Spieler +0x8c (DAT_800acae0) */
static int     s_steh;       /* die Bewegung, die der Stopp gesetzt hat (Fremdeingriff erkennen) */

static void vorschub(re15_actor_t *p)
{
    /* @0x8003295c `jal 0x800245d8` (a0 = 0): Vorschub um das Tempo in Blickrichtung — derselbe Weg wie Gehen/Rennen
     * im Port (player_common.c: x += cos*v, z += -sin*v). */
    const int32_t s = re15_sin_q12((int)p->rot_y), c = re15_cos_q12((int)p->rot_y);
    p->x += (c * s_tempo) / 4096;
    p->z += (-s * s_tempo) / 4096;
}

static int schritt(re15_actor_t *p, uint16_t pad_bits, int lauf_motion, int steh_motion, int32_t lauf_tempo,
                   int waffe)
{
    if (s_aktiv && (p->motion != s_steh || p->state != 1)) s_aktiv = 0;   /* Skript/Treffer/Treppe hat uebernommen */
    if (!s_aktiv) {
        if (p->motion != lauf_motion || p->state != 1) return 0;          /* nur aus dem Rennen (+6 < 2) */
        const int oben_rennen = (pad_bits & RE15_PAD_BIT_UP) && (pad_bits & RE15_PAD_BIT_CROSS);   /* & 0x201 == 0x201 */
        const int zielen      = (pad_bits & RE15_PAD_BIT_R1) && waffe;                                /* 0x100 + Waffe */
        if (oben_rennen && !zielen) return 0;                                                         /* weiter rennen */
        s_aktiv = 1;                                  /* @0x8003272c +6 = 2; ANIMATE Fall 2 im selben Bild: */
        s_tempo = lauf_tempo;
        s_steh  = steh_motion;
        p->motion            = (int16_t)steh_motion;  /* @0x80032900/@0x80032904 Clip 3 (+0x94) */
        p->anim_frame        = 0;
        p->motion_init_delay = 0;
        p->anim_frac         = 7;                     /* @0x80032908/@0x80032910 +0xe3 = 7 */
    } else {
        /* @0x80032730-@0x80032760: OBEN -> Gehen, UNTEN -> Rueckwaerts, LINKS/RECHTS -> Drehen (Stopp endet,
         * der Aufrufer waehlt die neue Bewegung in diesem Bild). */
        if (pad_bits & (RE15_PAD_BIT_UP | RE15_PAD_BIT_DOWN | RE15_PAD_BIT_LEFT | RE15_PAD_BIT_RIGHT)) {
            s_aktiv = 0;
            return 0;
        }
    }
    s_tempo -= RS_ABZUG;                              /* @0x80032930-@0x80032944 */
    if (s_tempo < 0) s_aktiv = 0;                     /* @0x8003294c `bgez` / @0x80032958 -> Stehen */
    vorschub(p);                                      /* auch der letzte, negative Schritt */
    return 1;
}

int re15_rennstopp_r41_takt(re15_actor_t *p, uint16_t pad_bits, int lauf_motion, int steh_motion,
                            int32_t lauf_tempo, int waffe)
{
    if (!p) return 0;
    return schritt(p, pad_bits, lauf_motion, steh_motion, lauf_tempo, waffe);
}

int re15_rennstopp_r41_skript(re15_actor_t *p, int lauf_motion, int steh_motion, int32_t lauf_tempo)
{
    if (!p) return 0;
    return schritt(p, 0, lauf_motion, steh_motion, lauf_tempo, 0);
}

int     re15_rennstopp_r41_aktiv(void)       { return s_aktiv; }
int32_t re15_rennstopp_r41_tempo(void)       { return s_tempo; }
void    re15_rennstopp_r41_zuruecksetzen(void) { s_aktiv = 0; s_tempo = 0; s_steh = 0; }
