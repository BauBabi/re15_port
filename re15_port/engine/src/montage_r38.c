/*
 * Runde 38 Spur O "montage" — Montage nach Irons' Tod: die durchbrechenden Zombies gehen in jedem Schnitt auf die
 * Kamera zu (Form des Rolltor-Schnitts). Herleitung, Belege, Konstanten: include/re15_montage_r38.h, Dossier
 * analysis/befunde_runde38/O_montage.md. Die Listen (Durchbruch-Form) und das Umparken in Cut 12 stehen im Programm
 * (irons_tod_1150.c); hier der Einzelstopp hinter der Kamera und die Messung je Schnitt.
 */
#include "re15_montage_r38.h"
#include "re15_irons_tod.h"
#include "re15_actor.h"
#include "re15_ai_flavor.h"   /* re15_re2z_owns_type: die Zombie-Familie */
#include "re15_enemy_ai.h"    /* RE15_AI_GRID_SKIP = entity+0x9 Bit 0x20 */
#include "re15_scd.h"         /* g_scd.cam_id */
#include <math.h>
#include <string.h>
#ifdef RE15_PLATFORM_PC
#include <stdio.h>
#endif

extern unsigned g_current_room_id;

/* RID-Saetze (RDT camera_start 0x60 + Cut * 32): pos_x @+0x04, pos_z @+0x0C, target_x @+0x10, target_z @+0x18. */
static const re15_r38_kamera_t k_kameras[] = {
    { 0x1130,  0, 1, 0x060,  -2386,  -7451,    876, -18973 },   /* ROOM1130 Cut 0: Kopien aus 1140 */
    { 0x1040,  1, 0, 0x080, -26262, -10692, -25020,  -1818 },   /* ROOM1040 Cut 1: Rolltor (nur Messung) */
    { 0x1030,  7, 1, 0x140, -26820,  -2574, -15858,  -5040 },   /* ROOM1030 Cut 7: Kopien aus 1070 */
    { 0x1030, 12, 1, 0x1E0, -11790, -15282,  -8640, -26478 },   /* ROOM1030 Cut 12 (= Cut 6, Tor aufgebrochen) */
};
#define R38_N_KAMERAS ((int)(sizeof k_kameras / sizeof k_kameras[0]))

int re15_r38_kameras(const re15_r38_kamera_t **out)
{
    if (out) *out = k_kameras;
    return R38_N_KAMERAS;
}

static int s_an = 1;
void re15_r38_montage_enable(int an) { s_an = an ? 1 : 0; }
int  re15_r38_montage_enabled(void)  { return s_an; }

/* Welcher Montage-Schritt gehoert zu welchem Raum (re15_irons_tod.h Zustaende). */
static int schritt_raum(int zustand)
{
    switch (zustand) {
    case RE15_IT_S1130: return RE15_IT_RAUM_1130;
    case RE15_IT_S1040: return RE15_IT_RAUM_1040;
    case RE15_IT_S1030: return RE15_IT_RAUM_1030;
    default:            return -1;
    }
}

static const re15_r38_kamera_t *kamera(uint16_t raum, int cut)
{
    for (int i = 0; i < R38_N_KAMERAS; i++)
        if (k_kameras[i].raum == raum && (int)k_kameras[i].cut == cut) return &k_kameras[i];
    return NULL;
}

/* ---- Messung ------------------------------------------------------------------------------------------------------ */
#define R38_MAX_SPUREN 128
static re15_r38_spur_t s_spur[R38_MAX_SPUREN];
static int s_n_spur = 0;
static const re15_r38_kamera_t *s_offen = NULL;   /* laufender Abschnitt */
static int s_offen_von = 0;                        /* erster Spur-Index des Abschnitts */
static int s_bild = 0;                             /* Abschnitts-Bild */
static struct { uint16_t raum; uint8_t cut; int bilder; } s_laenge[8];
static int s_n_laenge = 0;

void re15_r38_montage_reset(void)
{
    s_n_spur = 0; s_offen = NULL; s_offen_von = 0; s_bild = 0; s_n_laenge = 0;
}

int re15_r38_spuren(const re15_r38_spur_t **out)
{
    if (out) *out = s_spur;
    return s_n_spur;
}

int re15_r38_abschnitt_bilder(uint16_t raum, uint8_t cut)
{
    int n = 0;
    for (int i = 0; i < s_n_laenge; i++) if (s_laenge[i].raum == raum && s_laenge[i].cut == cut) n = s_laenge[i].bilder;
    if (s_offen && s_offen->raum == raum && s_offen->cut == cut) n = s_bild;
    return n;
}

static void abschnitt_schliessen(void)
{
    if (!s_offen) return;
    if (s_n_laenge < (int)(sizeof s_laenge / sizeof s_laenge[0])) {
        s_laenge[s_n_laenge].raum = s_offen->raum; s_laenge[s_n_laenge].cut = s_offen->cut;
        s_laenge[s_n_laenge].bilder = s_bild; s_n_laenge++;
    }
#ifdef RE15_PLATFORM_PC
    for (int i = s_offen_von; i < s_n_spur; i++) {
        const re15_r38_spur_t *p = &s_spur[i];
        fprintf(stderr, "[montage-r38] ROOM%04X Cut %d Bild %d: Aktor %d Typ %02x Bit %d Abstand zur Kamera %d -> %d"
                " (Tiefe %d -> %d, Bild %d..%d) angehalten %d%s\n", (unsigned)p->raum, p->cut, s_bild, p->aktor, p->typ,
                p->bit, (int)p->d0, (int)p->d1, (int)p->t0, (int)p->t1, p->bild0, p->bild1, p->angehalten,
                p->halt0 ? " (schon vorher)" : "");
    }
#endif
    s_offen = NULL;
}

static re15_r38_spur_t *spur(const re15_actor_t *a, int aktor)
{
    for (int i = s_offen_von; i < s_n_spur; i++)
        if (s_spur[i].aktor == aktor && s_spur[i].typ == a->type && s_spur[i].bit == a->em_flag_id) return &s_spur[i];
    if (s_n_spur >= R38_MAX_SPUREN) return NULL;
    re15_r38_spur_t *p = &s_spur[s_n_spur++];
    memset(p, 0, sizeof *p);
    p->raum = s_offen->raum; p->cut = s_offen->cut; p->aktor = (uint8_t)aktor; p->typ = a->type; p->bit = a->em_flag_id;
    p->bild0 = s_bild;
    p->halt0 = (a->grid_id & RE15_AI_GRID_SKIP) ? 1 : 0;
    p->kriech0 = (a->anim_flags & RE15_R38_KRIECH_BIT) ? 1 : 0;
    return p;
}

/* Baustein 4 (re15_montage_r38.h): die Regel von ROOM1030 sub06 @0x024C8.. fuer jeden Gegner-Index i = 0..0x13 (Aktor
 * i + 1, Work_set(2,i) = scd_work_bind kind 2): noch nicht geschickt (Ck(5,i) = 0) und im Warte-Rechteck (member 0x0F ==
 * 5) -> sub07 (member 0x10 |= 0x1000) und Set(5,i) = 1. Liefert die Zahl der neu geschickten. */
static int kriech_regel(const re15_r38_kamera_t *k)
{
    int n = 0;
    for (int i = 0; i < RE15_R38_KRIECH_N && i + 1 < RE15_ACTOR_MAX; i++) {
        re15_actor_t *a = &g_actors[i + 1];
        if (!a->active) continue;                                                    /* Work_set bindet nur aktive */
        if (re15_game_flag_get(RE15_R38_KRIECH_BANK, (uint8_t)i)) continue;         /* Ck(5,i) @0x024D4.. */
        if (a->member_0b != RE15_R38_WARTE_STEMPEL) continue;                        /* @0x024E0 */
        a->anim_flags = (uint16_t)(a->anim_flags | RE15_R38_KRIECH_BIT);             /* sub07 @0x02754-5E */
        re15_game_flag_set(RE15_R38_KRIECH_BANK, (uint8_t)i, 1);                    /* @0x024E8 */
        re15_r38_spur_t *p = spur(a, i + 1);
        if (p && !p->kriecht) p->kriecht = s_bild;
        n++;
#ifdef RE15_PLATFORM_PC
        fprintf(stderr, "[montage-r38] ROOM%04X Cut %d Bild %d: Aktor %d im Warte-Rechteck -> kriecht (member 0x10 |= 0x1000,"
                " Set(5,%d))\n", (unsigned)k->raum, k->cut, s_bild, i + 1, i);
#endif
    }
    return n;
}

/* ---- Takt --------------------------------------------------------------------------------------------------------- */
void re15_r38_montage_takt(void)
{
    int zustand = re15_irons_tod_zustand();
    int raum_soll = schritt_raum(zustand);
    const re15_r38_kamera_t *k = NULL;
    if (raum_soll >= 0 && (int)(uint16_t)g_current_room_id == raum_soll)
        k = kamera((uint16_t)raum_soll, (int)g_scd.cam_id);
    if (k != s_offen) {                                   /* Schnitt- oder Raumwechsel: Abschnitt wechseln */
        abschnitt_schliessen();
        if (k) { s_offen = k; s_offen_von = s_n_spur; s_bild = 0; }
    }
    if (!k) return;
    s_bild++;

    /* Blickrichtung xz des Schnitts (RID target - pos); Vorzeichen der Tiefe ganzzahlig, Betrag nur fuer die Messung. */
    const int64_t fx = (int64_t)k->zx - k->kx, fz = (int64_t)k->zz - k->kz;
    const double  fn = sqrt((double)fx * (double)fx + (double)fz * (double)fz);
    for (int i = 1; i < RE15_ACTOR_MAX; i++) {
        re15_actor_t *a = &g_actors[i];
        if (!a->active || !a->type || !re15_re2z_owns_type(a->type)) continue;
        const int64_t dx = (int64_t)a->x - k->kx, dz = (int64_t)a->z - k->kz;
        const int64_t tiefe = dx * fx + dz * fz;
        re15_r38_spur_t *p = spur(a, i);
        if (p) {
            p->bild1 = s_bild;
            p->d1 = (int32_t)lround(sqrt((double)dx * (double)dx + (double)dz * (double)dz));
            p->t1 = (int32_t)lround((double)tiefe / fn);
            if (p->bild0 == s_bild) { p->d0 = p->d1; p->t0 = p->t1; }
        }
        /* Baustein 3: Einzelstopp hinter der Bildebene (entity+0x9 |= 0x20, @0x80100450-5c) — nur in den Schnitten mit
         * halt = 1 (der Rolltor-Schnitt behaelt seine Regel in irons_tod_1150.c bilanz_takt). */
        if (s_an && k->halt && tiefe < 0 && !(a->grid_id & RE15_AI_GRID_SKIP)) {
            a->grid_id |= RE15_AI_GRID_SKIP;
            if (p) p->angehalten = s_bild;
#ifdef RE15_PLATFORM_PC
            fprintf(stderr, "[montage-r38] ROOM%04X Cut %d Bild %d: Aktor %d hinter der Kamera angehalten (entity+0x9 |= 0x20)\n",
                    (unsigned)k->raum, k->cut, s_bild, i);
#endif
        }
    }
    /* Baustein 4: durch das aufgebrochene Tor nur kriechend — jeder, der im Warte-Rechteck steht (sub06-Regel). */
    if (s_an && k->raum == RE15_IT_RAUM_1030 && k->cut == RE15_R38_KRIECH_CUT) (void)kriech_regel(k);
}
