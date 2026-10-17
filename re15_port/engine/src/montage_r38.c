/*
 * Runde 38 Spur O "montage" — Montage nach Irons' Tod: die durchbrechenden Zombies gehen in jedem Schnitt auf die
 * Kamera zu (Form des Rolltor-Schnitts). Herleitung, Belege, Konstanten: include/re15_montage_r38.h, Dossier
 * analysis/befunde_runde38/O_montage.md. Die Listen (Durchbruch-Form) und das Umparken in Cut 12 stehen im Programm
 * (irons_tod_1150.c); hier der Einzelstopp hinter der Kamera (Baustein 3), die Kriech-Regel am aufgebrochenen Tor
 * (Baustein 4), das Warten der Zombies hinter dem Tor auf ihren Schnitt (Baustein 5) und die Messung je Schnitt.
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
/* Abstand je Spur im 16-Bilder-Raster ab bild0 (Mitte der Sichtzeit = dm); 48 Proben = 768 Bilder (der laengste Montage-
 * Schnitt, das Rolltor, dauert 405). uint16: Abstaende in einem Raum < 65536. */
#define R38_PROBE_TAKT 16
#define R38_PROBEN     48
static uint16_t s_probe[R38_MAX_SPUREN][R38_PROBEN];
static int s_n_spur = 0;
static const re15_r38_kamera_t *s_offen = NULL;   /* laufender Abschnitt */
static int s_offen_von = 0;                        /* erster Spur-Index des Abschnitts */
static int s_bild = 0;                             /* Abschnitts-Bild */
#define R38_MAX_ABSCHNITTE 8
static re15_r38_abschnitt_t s_ab[R38_MAX_ABSCHNITTE];
static int s_n_ab = 0;
static re15_r38_abschnitt_t s_ab_offen;            /* laufender Abschnitt (Kopf) */
static int s_fremde_kamera = 0;                    /* Bilder eines Montage-Schritts auf einer Kamera ausserhalb der Tabelle */
#define R38_PFAD_TAKT 7                            /* FUN_80039e7c +0x91: `ori v0,v1,0x7` @0x80039eb8 */
static uint8_t s_ziel_lauf[RE15_ACTOR_MAX];        /* Bilder in Folge mit fremdem Navigator-Ziel je Aktor */
static uint8_t s_wartet[RE15_ACTOR_MAX];           /* Baustein 5: von DIESER Regel angehalten (nur diese gibt sie frei) */

void re15_r38_montage_reset(void)
{
    s_n_spur = 0; s_offen = NULL; s_offen_von = 0; s_bild = 0; s_n_ab = 0; s_fremde_kamera = 0;
    memset(&s_ab_offen, 0, sizeof s_ab_offen);
    memset(s_ziel_lauf, 0, sizeof s_ziel_lauf);
    memset(s_wartet, 0, sizeof s_wartet);
}

/* Baustein 5 (re15_montage_r38.h): im Montage-Schritt ROOM1030 halten die Zombies HINTER DEM TOR (BLK-Zonen 0 und 10)
 * still, solange ein anderer Schnitt als das Tor (Cut 12) zu sehen ist; mit dem ersten Bild von Cut 12 laufen sie los.
 * Form ROOM1141 sub02: `34 0c 30 00` (Member_set(12,0x30) = entity+0x9 Bit 0x20) @0x00E70/@0x00E78 unmittelbar vor
 * `29 00` (Cut_chg 0) @0x00E80, Freigabe `34 0c 10 00` @0x00F64/@0x00F74; Wirkung @0x80100450-5c (Takt uebersprungen). */
static void tor_warten(const re15_r38_kamera_t *k)
{
    const int tor_schnitt = (k && k->cut == RE15_R38_KRIECH_CUT);
    for (int i = 1; i < RE15_ACTOR_MAX; i++) {
        re15_actor_t *a = &g_actors[i];
        if (!a->active || !a->type || !re15_re2z_owns_type(a->type)) { s_wartet[i] = 0; continue; }
        if (tor_schnitt) {
            if (!s_wartet[i]) continue;
            a->grid_id = (uint8_t)(a->grid_id & ~RE15_AI_GRID_SKIP);              /* Freigabe @0x00F64 */
            s_wartet[i] = 0;
#ifdef RE15_PLATFORM_PC
            fprintf(stderr, "[montage-r38] ROOM%04X Cut %d: Aktor %d hinter dem Tor freigegeben (entity+0x9 &= ~0x20) bei (%d,%d)\n",
                    (unsigned)RE15_IT_RAUM_1030, RE15_R38_KRIECH_CUT, i, (int)a->x, (int)a->z);
#endif
            continue;
        }
        if (s_wartet[i] || (a->grid_id & RE15_AI_GRID_SKIP)) continue;
        const uint8_t zone = re15_nav_zone_from_pos((int16_t)a->x, (int16_t)a->z);
        if (zone != RE15_R38_ZONE_HINTER_TOR_A && zone != RE15_R38_ZONE_HINTER_TOR_B) continue;
        a->grid_id = (uint8_t)(a->grid_id | RE15_AI_GRID_SKIP);                   /* @0x00E70: Bit 0x20 */
        s_wartet[i] = 1;
#ifdef RE15_PLATFORM_PC
        fprintf(stderr, "[montage-r38] ROOM%04X Cut %d: Aktor %d hinter dem Tor (Zone %d) wartet auf Cut %d (entity+0x9 |= 0x20)"
                " bei (%d,%d)\n", (unsigned)RE15_IT_RAUM_1030, k ? (int)k->cut : -1, i, zone, RE15_R38_KRIECH_CUT, (int)a->x, (int)a->z);
#endif
    }
}

/* Den laufenden Abschnitt schliessen (Riegel im Prozess: der Lauf endet mit der Tuer-Anforderung, bevor der naechste
 * Raum ihn schliesst; in der exe schliesst ihn der Raumwechsel). */
static void abschnitt_schliessen(void);
void re15_r38_abschnitt_beenden(void) { abschnitt_schliessen(); }

int re15_r38_fremde_kamera(void) { return s_fremde_kamera; }

int re15_r38_spuren(const re15_r38_spur_t **out)
{
    if (out) *out = s_spur;
    return s_n_spur;
}

int re15_r38_abschnitte(const re15_r38_abschnitt_t **out)
{
    if (out) *out = s_ab;
    return s_n_ab;
}

int re15_r38_abschnitt_bilder(uint16_t raum, uint8_t cut)
{
    int n = 0;
    for (int i = 0; i < s_n_ab; i++) if (s_ab[i].raum == raum && s_ab[i].cut == cut) n = s_ab[i].bilder;
    if (s_offen && s_offen->raum == raum && s_offen->cut == cut) n = s_bild;
    return n;
}

static void abschnitt_schliessen(void)
{
    if (!s_offen) return;
    s_ab_offen.bilder = s_bild;
    for (int i = s_offen_von; i < s_n_spur; i++) {          /* Mitte der eigenen Sichtzeit (16er-Raster, abgerundet) */
        re15_r38_spur_t *p = &s_spur[i];
        int k = ((p->bild1 - p->bild0) / 2) / R38_PROBE_TAKT;
        if (k >= R38_PROBEN) k = R38_PROBEN - 1;
        p->dm = (int32_t)s_probe[i][k];
    }
    if (s_n_ab < R38_MAX_ABSCHNITTE) s_ab[s_n_ab++] = s_ab_offen;
#ifdef RE15_PLATFORM_PC
    for (int i = s_offen_von; i < s_n_spur; i++) {
        const re15_r38_spur_t *p = &s_spur[i];
        fprintf(stderr, "[montage-r38] ROOM%04X Cut %d Bild %d: Aktor %d Typ %02x Bit %d Abstand zur Kamera %d -> %d -> %d"
                " (Tiefe %d -> %d, Bild %d..%d) angehalten %d vorher %d Kriech %d/%d Warte %d Lage (%d,%d)\n", (unsigned)p->raum,
                p->cut, s_bild, p->aktor, p->typ, p->bit, (int)p->d0, (int)p->dm, (int)p->d1, (int)p->t0, (int)p->t1, p->bild0,
                p->bild1, p->angehalten, p->halt0, p->kriech0, p->kriecht, p->warte, (int)p->x1, (int)p->z1);
    }
    fprintf(stderr, "[montage-r38] ROOM%04X Cut %d Ende Bild %d: Spieler (%d,%d) HP min %d, Navigator-Ziel fremd %d, Kriech-Regel %d,"
            " KI %s\n", (unsigned)s_ab_offen.raum, s_ab_offen.cut, s_bild, (int)s_ab_offen.px, (int)s_ab_offen.pz, s_ab_offen.hp_min,
            s_ab_offen.ziel_fremd, s_ab_offen.kriech, s_ab_offen.ki_re2 ? "RE2" : "RE1.5");
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
    {   /* Messung: Kamera ausserhalb der Montage-Schnitte waehrend eines Montage-Schritts (z.B. RVD-Wechsel Cut 7 -> 8) */
        static int s_fremd_alt = -1;
        const int fremd = (raum_soll >= 0 && (int)(uint16_t)g_current_room_id == raum_soll && !k) ? (int)g_scd.cam_id : -1;
        if (fremd >= 0) s_fremde_kamera++;
#ifdef RE15_PLATFORM_PC
        if (fremd >= 0 && fremd != s_fremd_alt)
            fprintf(stderr, "[montage-r38] ROOM%04X: Kamera Cut %d ausserhalb der Montage-Schnitte\n", (unsigned)raum_soll, fremd);
#endif
        s_fremd_alt = fremd;
    }
    /* Baustein 5: vor der Messung, damit das erste Bild von Cut 12 die Freigegebenen als laufend sieht */
    if (s_an && raum_soll == RE15_IT_RAUM_1030 && (int)(uint16_t)g_current_room_id == raum_soll) tor_warten(k);
    else memset(s_wartet, 0, sizeof s_wartet);
    if (k != s_offen) {                                   /* Schnitt- oder Raumwechsel: Abschnitt wechseln */
        abschnitt_schliessen();
        if (k) {
            s_offen = k; s_offen_von = s_n_spur; s_bild = 0;
            memset(&s_ab_offen, 0, sizeof s_ab_offen);
            memset(s_ziel_lauf, 0, sizeof s_ziel_lauf);
            s_ab_offen.raum = k->raum; s_ab_offen.cut = k->cut; s_ab_offen.hp_min = 1 << 30;
            s_ab_offen.ki_re2 = (uint8_t)(re15_ai_re2_for_type(0x10) ? 1 : 0);   /* Zombie-Familie 0x10/0x11/0x16 */
        }
    }
    if (!k) return;
    s_bild++;
    {   /* Messung je Abschnitt: Spielerlage + HP; Navigator-Ziel der Zombies in der Zone des Spielers. Ein Zonenwechsel
         * laesst das Ziel bis zum naechsten KI-Takt auf dem Uebergang stehen (gemessen: 1 Bild); "fremd" ist erst ein Ziel
         * ungleich der Spielerlage, das laenger haelt als ein Takt der Pfadsuche (+0x91 `ori v0,v1,0x7` @0x80039eb8 /
         * `addiu v1,v1,-1` @0x80039ed4 = alle 7 Bilder) — wie das veraltete Ziel aus ROOM1040 (Dossier "Zwischenmessung"). */
        const re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
        s_ab_offen.px = pl->x; s_ab_offen.pz = pl->z;
        if (pl->hp < s_ab_offen.hp_min) s_ab_offen.hp_min = pl->hp;
        const uint8_t zpl = re15_nav_zone_from_pos((int16_t)pl->x, (int16_t)pl->z);
        int fremd = 0;
        for (int i = 1; i < RE15_ACTOR_MAX; i++) {
            const re15_actor_t *a = &g_actors[i];
            int abw = zpl != 0xff && a->active && a->type && re15_re2z_owns_type(a->type) && !(a->grid_id & RE15_AI_GRID_SKIP) &&
                      a->state == 1 && re15_nav_zone_from_pos((int16_t)a->x, (int16_t)a->z) == zpl &&
                      (a->steer_x != (int16_t)pl->x || a->steer_z != (int16_t)pl->z);
            s_ziel_lauf[i] = abw ? (uint8_t)(s_ziel_lauf[i] < 255 ? s_ziel_lauf[i] + 1 : 255) : 0;
            if (s_ziel_lauf[i] > R38_PFAD_TAKT) fremd = 1;
        }
        s_ab_offen.ziel_fremd += fremd;
    }

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
            p->x1 = a->x; p->z1 = a->z;
            if (p->bild0 == s_bild) { p->d0 = p->d1; p->t0 = p->t1; }
            {   const int rel = s_bild - p->bild0;
                if (rel % R38_PROBE_TAKT == 0 && rel / R38_PROBE_TAKT < R38_PROBEN)
                    s_probe[p - s_spur][rel / R38_PROBE_TAKT] = (uint16_t)(p->d1 < 65535 ? p->d1 : 65535); }
            if (!p->warte && a->member_0b == RE15_R38_WARTE_STEMPEL && !(a->anim_flags & RE15_R38_KRIECH_BIT))
                p->warte = s_bild;                                /* aufrecht im Warte-Rechteck (@0x024E0) */
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
    if (s_an && k->raum == RE15_IT_RAUM_1030 && k->cut == RE15_R38_KRIECH_CUT) s_ab_offen.kriech += kriech_regel(k);
}
