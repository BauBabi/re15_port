/* ============================================================================================
 * G5-TENTAKEL (Typ 0x37, EM037) — die vier Arme des Endkampf-Birkin.
 * --------------------------------------------------------------------------------------------
 * Nutzer 2026-09-13: "Birkin hat immer noch - keine Tentakel". Im Original sind sie KEINE
 * Teile des Boss-Modells, sondern vier eigene Entities, die G5 per Routine-Wort kommandiert.
 * QUELLE: analysis/befunde_runde7_2026-09-13/em037-tentakel.md (Vollzensus des EM037-Overlays,
 * CDEMD0.EMS Sektor 0xE04, gelinkt @0x80100000) - jede Zahl hier traegt ihre @0x-Adresse.
 *
 * ARCHITEKTUR DES ORIGINALS (uebernommen):
 *   - Per-Frame-Main 0x80100178; Routine-Tabelle @0x80105688; 16 Sub-Zustaende @0x801056F0.
 *   - G5 schreibt ein GANZES Wort nach +0x04 (Sender 0x80104E9C @0x80104E9C-B0):
 *         wort = (ph << 16) | (sub << 8) | routine
 *     0x301 Peitsche, 0xD01 Spiess, 0xB01/0x70B01/0x90B01 Zug, 0x901 Speer, 0xA01/0x40A01
 *     Wedeln, 0x601 Einziehen, 0xE01 Tod, +0x06-Treiber fuer den Austritt (sub8).
 *   - ANKER: die Tentakel haengen JEDEN FRAME an G5s Part 2 (der MASSE, nicht am Kriecher),
 *     Tabelle @0x80105668; Modus +0x219: 0 = voller Anker, 1 = frei, 2 = halber Anker
 *     (@0x801001E4-388).
 *   - HP = -1 @0x80100530: die Tentakel sind unverwundbar.
 *
 * PORT-ENTSCHEIDUNGEN (dokumentiert, nicht geraten):
 *   1. SPAWN: ROOM5090 kennt keine 0x37 (sub00 hat nur 0x4D und 0x30) - das Modul spawnt die
 *      vier Aktoren beim Kampfstart selbst. Im RE2-Raum stehen sie ebenfalls nicht in der RDT
 *      (room7040 parkt alle fuenf auf (-32000,0,-32000)); die Platzierung macht auch dort der
 *      Code ueber die Ankertabelle (em037-tentakel.md 4.2/4.4).
 *   2. STRECKUNG: das Original faehrt die Tentakel ueber scaleX (0..4096) aus der Masse heraus.
 *      Die EM037-Kette liegt komplett auf der X-Achse (rel. Positionen 0/3750/3003/1508,
 *      Laenge 8261) - der uniforme render_scale_q12 des Ports (main.c:8200, Original-Zwilling
 *      FUN_8001e8c8) bildet die Laengenstreckung damit ab; die Dicke skaliert mit (Abweichung).
 *   3. TREFFER: das Original prueft ueber ein unsichtbares KIND-Entity (+0x10E=8) und meldet
 *      Bits in +0x220. Der Port prueft stattdessen die SPITZE (Kettenlaenge * Streckung entlang
 *      der Tentakel-Achse) gegen den Spieler - dieselbe Geometrie, ein Entity weniger.
 *   4. Die Per-Part-Einrollwinkel (sub9 ph5/7) und die Spitzensonde gegen Raumgrenzen sind
 *      NICHT gebaut; die betroffenen Zustaende laufen ueber ihre Timer.
 * ========================================================================================== */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "re15_actor.h"
#include "re15_enemy.h"
#include "re15_enemy_ai.h"
#include "re15_damage.h"
#include "re15_room.h"
#include "re15_skeleton.h"

#define TENT_N 4

/* Ankertabelle @0x80105668 (4 x 8 B: s16 x, y, z, pad). Zwei je Seite, je einer hoch/tief. */
static const int16_t s_tent_anker[TENT_N][3] = {
    {  965, -2243, -1933 },   /* vorn, hoch, links        */
    { 1615,  -706, -2440 },   /* weiter vorn, tief, links */
    { 1409,  -683,  2495 },   /* weiter vorn, tief, rechts*/
    {  957, -2426,  1958 },   /* vorn, hoch, rechts       */
};

/* Austritts-Clips je Index (Byte-Tabelle @0x80100048): 14 / 14 / 16 / 15. */
static const uint8_t s_tent_clip_austritt[TENT_N] = { 14, 14, 16, 15 };

/* Peitschen-Fenster (Tabelle @0x80105730, zwei Varianten a 4 s16):
 * Variante 0 (Clip 7) {30,55,60,70}, Variante 1 (Clip 8) {38,50,52,60}. */
static const int16_t s_tent_fenster[2][4] = {
    { 30, 55, 60, 70 },
    { 38, 50, 52, 60 },
};

/* RotX je Index beim Peitschen-Start (u16-Tabelle @0x80105738). */
static const int16_t s_tent_peitsch_rotx[TENT_N] = { 896, 1088, 2976, 3200 };

typedef struct {
    int      slot;          /* Aktor-Slot (-1 = nicht gespawnt)                    */
    uint8_t  sub, ph;       /* +0x05 / +0x06                                        */
    uint8_t  ankermodus;    /* +0x219: 0 voll / 1 frei / 2 halb                     */
    uint8_t  variante;      /* +0x16B (Peitschen-Variante)                          */
    int32_t  timer;         /* +0x158                                               */
    int32_t  t;             /* Phasenzaehler (sub8 ph4/ph6)                         */
    int32_t  scale_x;       /* 0..4096                                              */
    int32_t  rate;          /* +0x15A                                               */
    int16_t  rot_z;         /* +0x78                                                */
    int      getroffen;     /* Treffer-Sperre je Angriff                            */
} tent_t;

static tent_t s_tent[TENT_N];
static int    s_tent_bereit = 0;
static uint8_t s_tent_maske = 0;       /* G5 +0x228: Bit i+4 = beschaeftigt */

/* SE-Hook (dieselbe ENEMSE-Bank wie G5, Paar-Zeile 25). */
static void (*s_tent_se_fn)(int, int) = 0;
void re15_tentakel_audio_hook(void (*se)(int, int)) { s_tent_se_fn = se; }
static void tent_se(int id) { if (s_tent_se_fn) s_tent_se_fn(id, 0); }

uint8_t re15_g5_tentakel_maske(void) { return s_tent_maske; }

/* ---- Kommando-Eingang: G5s Sender 0x80104E9C schreibt ein ganzes Wort nach +0x04 ---- */
void re15_g5_tentakel_cmd(int idx, uint32_t wort)
{
    tent_t *t;
    if (idx < 0 || idx >= TENT_N) return;
    t = &s_tent[idx];
    t->sub = (uint8_t)((wort >> 8) & 0xFFu);
    t->ph  = (uint8_t)((wort >> 16) & 0xFFu);
    t->timer = 0; t->t = 0; t->getroffen = 0;
}

void re15_g5_tentakel_broadcast(uint32_t wort)
{
    int i;
    for (i = 0; i < TENT_N; i++) re15_g5_tentakel_cmd(i, wort);
}

/* ---- Spawn beim Kampfstart (Port-Entscheidung 1) ---------------------------------------- */
void re15_g5_tentakel_spawn(const re15_actor_t *g5)
{
    int i;
    if (s_tent_bereit) return;
    for (i = 0; i < TENT_N; i++) {
        int slot = re15_actor_alloc(0x37u);
        s_tent[i].slot = slot;
        s_tent[i].sub = 8; s_tent[i].ph = 0;      /* Ctor startet in sub8 (Austritt) */
        s_tent[i].ankermodus = 0;
        s_tent[i].scale_x = 0; s_tent[i].rate = 0;
        s_tent[i].rot_z = 0; s_tent[i].timer = 0; s_tent[i].t = 0;
        if (slot >= 0) {
            re15_actor_t *e = &g_actors[slot];
            e->type = 0x37u; e->active = 1; e->flags = 1;
            e->hp = -1;                            /* unverwundbar @0x80100530 */
            e->motion = (int16_t)s_tent_clip_austritt[i];
            e->anim_frame = 0;
            e->rot_y = g5 ? g5->rot_y : 0;
            e->render_scale_q12 = 1;               /* 0 = kein Skalieren; s. Port-Entscheidung 2 */
            e->grid_id = 0;
        }
    }
    s_tent_maske = 0;
    s_tent_bereit = 1;
}

void re15_g5_tentakel_reset(void)
{
    int i;
    for (i = 0; i < TENT_N; i++) {
        int slot = s_tent[i].slot;
        /* ⛔ NUR EINEN ECHTEN ARM ABSCHALTEN (Nutzer-Befund 2026-09-13: "es taucht immer
         * noch kein Birkin auf, + ich haenge in der cutscene fest").
         *
         * s_tent[] ist als `static` NULL-initialisiert - `slot` ist vor dem ersten Spawn
         * also 0, und Slot 0 ist per Definition der SPIELER (re15_actor.h:23
         * RE15_ACTOR_SLOT_PLAYER). Die alte Schleife hat beim ersten Reset viermal
         * `g_actors[0].active = 0` geschrieben und damit Leon abgeschaltet: der Raum fror
         * ein (befund.log 2026-09-13: Cut 12 bzw. 15, Position konstant ueber 110-575
         * Messzeilen), und weil der Boss-Tick den toten Spieler-Aktor las, erschien auch
         * Birkin nicht. Aufgefallen ist es erst, als der Reset ueberhaupt gerufen wurde -
         * bis zum selben Tag stand er nur im Test (das war der Runde-8-Fix).
         *
         * Zwei Riegel statt einem: Slot 0 ist NIE ein Arm, und der Aktor muss auch
         * wirklich noch der 0x37-Arm sein - sonst gehoert der Platz inzwischen jemand
         * anderem und das Abschalten waere genau der Fehler, den der Reset verhindern
         * soll. */
        if (slot > 0 && slot < RE15_ACTOR_MAX && g_actors[slot].type == 0x37u)
            g_actors[slot].active = 0;
        s_tent[i].slot = -1;
    }
    s_tent_bereit = 0; s_tent_maske = 0;
}

/* ---- Anker: Position aus G5s Part-2-(Blob-)Matrix (@0x801001E4-388) --------------------- */
static void tent_anker(tent_t *t, int idx, const re15_actor_t *g5, re15_actor_t *e)
{
    int32_t v[3];
    int32_t cs, sn;
    if (t->ankermodus == 1u) return;               /* frei - keine Nachfuehrung */
    v[0] = s_tent_anker[idx][0];
    v[1] = s_tent_anker[idx][1];
    v[2] = s_tent_anker[idx][2];
    if (t->ankermodus == 2u) {                     /* halber Anker @0x80100290-33C */
        v[1] = s_tent_anker[idx][1] / 2 - ((idx == 1 || idx == 2) ? 1500 : 1300);
        v[2] = s_tent_anker[idx][2] / 2;
    }
    /* Part 2 = die Masse: Bind (1800,4500,0) am WURZELKNOCHEN des Kriechers
     * (main.c-Komposition, g5-optik.md @0x80100670-740). Die Weltlage ist also
     * WURZELPOSE + Rot(yaw)*(Bind + Anker) - und die Wurzel haengt 3,1-4,5 m
     * UEBER dem Entity-Ursprung (alle EM036-Keyframes tragen rootY -4536..-3105).
     * ⛔ GEMESSEN 2026-09-13 (Nutzer-Marke R5090 F550): ohne diese Wurzelhoehe
     * landeten die vier Arme bei y = +2057..+3816, also 2-4 m UNTER dem Boden.
     * re15_enemy_bone_world_pos liefert die gerenderte Wurzel samt rootY. */
    {
        int32_t w[3];
        re15_enemy_bone_world_pos(g5, 0, w);
        v[0] += 1800; v[1] += 4500;
        cs = re15_cos_q12((int)g5->rot_y);
        sn = re15_sin_q12((int)g5->rot_y);
        e->x = w[0] + (int32_t)(((int64_t)cs * v[0] + (int64_t)sn * v[2]) >> 12);
        e->z = w[2] + (int32_t)((-(int64_t)sn * v[0] + (int64_t)cs * v[2]) >> 12);
        e->y = w[1] + v[1];
    }
    e->rot_y = g5->rot_y;
}

/* Spitze der Tentakel-Kette in Weltkoordinaten (Port-Entscheidung 3):
 * Kettenlaenge 8261 (rel. Positionen 0/3750/3003/1508, alle entlang +X) mal Streckung. */
static void tent_spitze(const tent_t *t, const re15_actor_t *e, int32_t out[3])
{
    int32_t len = (int32_t)(((int64_t)8261 * t->scale_x) >> 12);
    int32_t cs = re15_cos_q12((int)e->rot_y), sn = re15_sin_q12((int)e->rot_y);
    out[0] = e->x + (int32_t)(((int64_t)cs * len) >> 12);
    out[2] = e->z - (int32_t)(((int64_t)sn * len) >> 12);
    out[1] = e->y;
}

/* Diagnose/Test: Weltposition der Spitze eines Tentakels (-1 = kein Tentakel). */
int re15_g5_tentakel_spitze(int idx, int32_t out[3])
{
    if (idx < 0 || idx >= TENT_N) return -1;
    if (s_tent[idx].slot < 0 || s_tent[idx].slot >= RE15_ACTOR_MAX) return -1;
    tent_spitze(&s_tent[idx], &g_actors[s_tent[idx].slot], out);
    return 0;
}

static int tent_trifft(const tent_t *t, const re15_actor_t *e, const re15_actor_t *pl)
{
    int32_t s[3];
    int64_t dx, dz;
    tent_spitze(t, e, s);
    dx = (int64_t)pl->x - s[0];
    dz = (int64_t)pl->z - s[2];
    return (dx * dx + dz * dz) < (int64_t)1500 * 1500;
}

static int tent_anim(re15_actor_t *e)
{
    extern int re15_actor_clip_len(const re15_actor_t *a);
    int fc = re15_actor_clip_len(e);
    if (fc <= 0) fc = 1;
    if ((int)e->anim_frame + 1 >= fc) { e->anim_frame = 0; return 1; }
    e->anim_frame++;
    return 0;
}

/* ---- Ein Tentakel je Tick ---------------------------------------------------------------- */
static void tent_tick(int idx, const re15_actor_t *g5, re15_actor_t *pl)
{
    tent_t *t = &s_tent[idx];
    re15_actor_t *e;
    if (t->slot < 0 || t->slot >= RE15_ACTOR_MAX) return;
    e = &g_actors[t->slot];
    if (!e->active) return;

    switch (t->sub) {
    case 0:                                        /* IDLE 0x80100A44 */
        if (t->ph == 0) {
            e->motion = 0; e->anim_frame = 0;
            t->ph = 1;
            t->timer = 60 + (re15_engine_rand8() & 0x3F);   /* @0x80100A5C-9C */
            t->scale_x = 0;                                  /* Modell aus (parts.flags &= ~1) */
        }
        break;

    case 8:                                        /* AUSTRITT/BEBEN @0x80101D38ff */
        switch (t->ph) {
        case 0:
            e->motion = (int16_t)s_tent_clip_austritt[idx];
            e->anim_frame = 0;
            t->scale_x = 0;
            if (idx == 1) t->ankermodus = 2;       /* @0x80101D60-78 */
            break;                                  /* G5 treibt +0x06 weiter */
        case 1:
            tent_se(0); t->ph = 2; t->ankermodus = 1;   /* @0x80101D90 */
            break;
        case 2:                                     /* @0x80101DAC: 16 Frames heraus */
            t->scale_x += 256;
            if (t->scale_x > 4096) t->scale_x = 4096;
            if (tent_anim(e)) t->ph = 3;
            break;
        case 3:
            e->motion = 0; e->anim_frame = 0;      /* Clip-Wort 0x00070000 */
            t->timer = 0; t->ph = 4; tent_se(2);
            break;
        case 4:                                     /* @0x80101E24 */
            if (t->t < 5)               e->x -= t->t * 10;
            else if (t->t <= 34)        e->x += 10;
            if (t->t == 36) t->ph = 5;
            t->t++;
            tent_anim(e);
            break;
        case 5: t->timer = 0; break;               /* warten - G5 treibt */
        case 6: {                                   /* Zittern @0x80101E94 */
            int32_t z = (int32_t)(re15_engine_rand8() & 0xF) * (1 - 2 * (t->t & 1));
            if (t->t < 20) e->x -= t->t;
            if (t->t >= 16 && t->t <= 179) { e->x += z; e->y += z; e->z += z; }
            if (t->t >= 161) e->x += 5;
            if (t->t == 180) t->ph = 7;
            t->t++;
            tent_anim(e);
            break;
        }
        case 8:                                     /* Einzug @0x80101FA8 */
            t->scale_x -= 64;
            e->rot_y = (int16_t)((e->rot_y + ((idx < 2) ? -16 : 16)) & 0xfff);
            if (t->scale_x < 2048) { t->scale_x = 0; tent_se(0); t->ph = 9; }
            tent_anim(e);
            break;
        default: break;                             /* 7/9: warten bzw. verborgen */
        }
        break;

    case 3:                                        /* PEITSCHE (0x301) @0x80101404ff */
        switch (t->ph) {
        case 0:
            e->motion = 0; e->anim_frame = 0;
            t->rot_z = -576;                        /* @0x8010146C */
            e->rot_x = s_tent_peitsch_rotx[idx];
            t->scale_x = 0; t->ph = 1;
            s_tent_maske |= (uint8_t)(1u << (idx + 4));   /* +0x228 belegt */
            tent_se(0);
            break;
        case 1:
            t->scale_x += 64;
            if (t->scale_x >= 2025 && t->rot_z < 32) t->rot_z = (int16_t)(t->rot_z + 32);
            if (t->scale_x >= 3073) {               /* @0x801014B8-D4 */
                t->variante = (uint8_t)(re15_engine_rand8() & 1u);
                e->motion = (int16_t)(7 + t->variante);
                e->anim_frame = 0;
                t->ph = 2;
            }
            tent_anim(e);
            break;
        case 2:
        case 3: {
            const int16_t *F = s_tent_fenster[t->variante & 1u];
            int fr = (int)e->anim_frame;
            if (t->ph == 2) {
                t->scale_x += 64;
                if (t->scale_x >= 4097) {           /* @0x80101544-68 */
                    t->scale_x = 4096; t->ph = 3;
                    t->timer = (re15_engine_rand8() & 0xF) + 10;
                }
            } else if (tent_anim(e)) {              /* @0x801015D8-E8 */
                e->motion = 0; e->anim_frame = 0; t->ph = 4;
                break;
            }
            if (fr > F[0] && fr < F[1]) t->rot_z = (int16_t)(t->rot_z + 8);
            if (fr > F[2] && fr < F[3]) {           /* TREFFER-FENSTER @0x80101640-94 */
                t->rot_z = (int16_t)(t->rot_z + 4);
                if (!t->getroffen && pl->hit_react == 0 && g5->hp >= 0 &&
                    tent_trifft(t, e, pl)) {
                    t->getroffen = 1;
                    pl->hp = (int16_t)(pl->hp - 15);         /* 15 @0x801016D0 */
                    if (pl->hp < 0) pl->hp = -1;
                    pl->hit_react |= 1;
                    tent_se(3);                              /* @0x80101790 */
                }
            }
            if (t->ph == 2) tent_anim(e);
            break;
        }
        default:                                    /* ph4: einziehen @0x801017BC */
            t->scale_x -= 64;
            if (t->scale_x < 0) {
                t->scale_x = 0; t->sub = 0; t->ph = 0;
                s_tent_maske &= (uint8_t)~(1u << (idx + 4));
            }
            tent_anim(e);
            break;
        }
        break;

    case 13:                                       /* SPIESS (0xD01) @0x801035BCff */
        switch (t->ph) {
        case 0:
            e->motion = (int16_t)((idx == 0 || idx == 3) ? 23 : 21);
            e->anim_frame = 0;
            t->scale_x = 0; t->ph = 1;
            s_tent_maske |= (uint8_t)(1u << (idx + 4));
            tent_se(0);
            break;
        case 1:
            t->scale_x += 128;
            if (t->scale_x > 4096) t->scale_x = 4096;
            {   int fr = (int)e->anim_frame;
                if (fr > 26 && fr < 32 && !t->getroffen && pl->hit_react == 0 &&
                    g5->hp >= 0 && tent_trifft(t, e, pl)) {
                    t->getroffen = 1;
                    pl->hp = (int16_t)(pl->hp - 15);          /* 15 @0x80103730 */
                    if (pl->hp < 0) pl->hp = -1;
                    pl->hit_react |= 1;
                    tent_se(3);
                }
            }
            if (tent_anim(e)) t->ph = 5;
            break;
        default:                                    /* ph5: fertig @0x801038E0 */
            t->scale_x -= 96;
            if (t->scale_x < 0) {
                t->scale_x = 0; t->sub = 0; t->ph = 0;
                s_tent_maske &= (uint8_t)~(1u << (idx + 4));
            }
            tent_anim(e);
            break;
        }
        break;

    case 11:                                       /* ZUG-TENTAKEL (0xB01/0x70B01/0x90B01) */
        /* Die Zugstraenge bleiben "frei" (die Maske wird hier GELOESCHT, @0x80102B40-4C):
         * sie ziehen G5 heran, blockieren aber keinen Angriffsplatz. */
        s_tent_maske &= (uint8_t)~(1u << (idx + 4));
        if (t->ph == 0) {
            e->motion = 0; e->anim_frame = 0; t->scale_x = 0; t->ph = 1;
            tent_se(0);
        } else if (t->scale_x < 4096) {
            t->scale_x += 128;
            if (t->scale_x > 4096) t->scale_x = 4096;
            tent_anim(e);
        } else {
            if (tent_anim(e)) { t->sub = 0; t->ph = 0; }
        }
        break;

    case 6:                                        /* EINZIEHEN (0x601) @0x80101B80 */
        if (t->ph == 0) { e->motion = 0; e->anim_frame = 0; t->ph = 1; }
        else {
            t->scale_x -= 64;
            if (t->scale_x < 0) { t->scale_x = 0; t->sub = 0; t->ph = 0; }
            tent_anim(e);
        }
        break;

    case 14:                                       /* TOD (0xE01) @0x801039B0 */
        if (t->ph == 0) {
            s_tent_maske |= (uint8_t)(1u << (idx + 4));   /* bleibt fuer immer gesetzt */
            t->ph = 1;
        }
        if (t->scale_x > 0) {
            t->scale_x -= 32;
            if (t->scale_x < 0) t->scale_x = 0;
        }
        tent_anim(e);
        break;

    default:                                       /* 9/10/12: ueber Timer laufen lassen */
        if (t->scale_x < 4096) t->scale_x += 96;
        if (tent_anim(e)) { t->sub = 0; t->ph = 0; }
        break;
    }

    /* Anker + Streckung an den Renderer. */
    tent_anker(t, idx, g5, e);
    if (t->scale_x <= 0) {
        e->render_scale_q12 = 1;                   /* praktisch unsichtbar (in der Masse) */
    } else {
        e->render_scale_q12 = (int16_t)t->scale_x; /* uniform, s. Port-Entscheidung 2 */
    }
    e->rot_z = t->rot_z;
}

/* ---- Haupteinstieg: G5 ruft das je Frame nach seinem eigenen Tick ------------------------ */
void re15_g5_tentakel_tick(const re15_actor_t *g5)
{
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    int i;
    if (!s_tent_bereit || !g5) return;
    for (i = 0; i < TENT_N; i++) tent_tick(i, g5, pl);
}
