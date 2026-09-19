/* probe_p3_gator_lunge.c — MESS-SONDE Phase 3 (2026-09-19), kein add_test.
 *
 * Offener Punkt 2 des Dossiers (gator-finisher-sounds.md §4 B2 / "Offen"):
 * Stufe B2 = LUNGE als RE2-Clip-Folge 2 -> 3 statt des heutigen 45-Bilder-Clip-4-
 * Schnapps. Der Auftrag verlangt ZUERST die Messung der heutigen Trefferfenster und
 * danach den Beleg, dass der Kampf gleich schwer bleibt.
 *
 * Diese Sonde misst BEIDE Varianten am ECHTEN Boss-Tick (kein Nachbau) auf dem
 * Biss-Raster von probe_gator_sweep (3 Leon-Verhalten x 4 Gator-Startpunkte x
 * 6 Leon-Standorte = 72 Faelle, je 2400 Bilder):
 *   - Treffer ja/nein je Fall, Bild des ersten Treffers
 *   - Zahl der LUNGE-Anlaeufe (Eintritt in den Angriffs-Clip) und wie viele davon
 *     in einem Treffer enden
 *   - Bilder vom Anlauf-Beginn bis zum Treffer (das eigentliche "Trefferfenster")
 *   - zurueckgelegte Strecke des Gators waehrend eines Anlaufs
 * Variante ueber die Umgebung: RE15_GB_LUNGE_B2=1 (Mess-Schalter im Boss, Default AUS).
 *
 * Teil 0 ist reine Datenmessung aus EM23.EMD: die Laengen der Clips 2/3/4 und das
 * Trefferfenster von B2 (Clip 3 f >= 114, @0x80100e8c `sltiu v0,v0,0x72`) in TICKS,
 * einmal mit einfachem und einmal mit doppeltem Clip-Vorlauf (@0x80100da4-dd0 /
 * @0x80100e18-e4c: bei +0x22C != 0 ruft das Original FUN_8001a330 ZWEIMAL je Tick,
 * und FUN_8001a330 erhoeht +0x14D genau um 1 je Ruf — `lbu v0,333(s2)` /
 * `addiu v0,v0,1` / `sb v0,333(s2)` @0x80029b28-34, Wrap-Rueckgabe 1 @0x80029b3c-4c).
 */
#include "re15_rdt.h"
#include "re15_actor.h"
#include "re15_aot.h"
#include "re15_room.h"
#include "re15_enemy_ai.h"
#include "re15_enemy.h"
#include "re15_ai_flavor.h"
#include "re15_skeleton.h"   /* re15_cos_q12/sin_q12 fuer die Maul-Distanz-Kopie */
#include "re15_player.h"
#include "re15_damage.h"
#include "re15_collision.h"
#include "re15_boss_gator.h"
#include "re15_emd.h"
#include "re2_ems.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef RE15_ASSET_PSX_DIR
#define RE15_ASSET_PSX_DIR "shared_assets/PSX"
#endif
#ifndef RE15_ASSET_RE2_DIR
#define RE15_ASSET_RE2_DIR "shared_assets/RE2"
#endif

static uint8_t *slurp(const char *path, size_t *n)
{
    FILE *f = fopen(path, "rb");
    if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    if (sz <= 0) { fclose(f); return NULL; }
    uint8_t *b = (uint8_t *)malloc((size_t)sz);
    if (b && fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); b = NULL; }
    fclose(f); if (b) *n = (size_t)sz;
    return b;
}

/* Kopie von gb_maul_dist (enemy_ai_boss_gator.c:483, dort static): Abstand Leons zum
 * Maul-Punkt 2600 Einheiten vor der Entity. Das Trefferkriterium beider Varianten ist
 * `<= 1500` (:1543 B2 / :1562 heute) — die Sonde misst damit, wie viele Ticks eines
 * Anlaufs das Maul AM ZIEL steht, ohne dass das Bissfenster offen ist ("Totzeit"). */
static int32_t p_maul_dist(const re15_actor_t *e, const re15_actor_t *pl)
{
    int32_t fc = re15_cos_q12((int)e->rot_y);
    int32_t fs = re15_sin_q12((int)e->rot_y);
    int32_t kx = e->x + (int32_t)(((int64_t)fc * 2600) >> 12);
    int32_t kz = e->z - (int32_t)(((int64_t)fs * 2600) >> 12);
    int64_t dx = pl->x - kx, dz = pl->z - kz;
    int64_t d2 = dx * dx + dz * dz;
    int32_t d = 0;
    while ((int64_t)d * d < d2 && d < 30000) d += 16;
    return d;
}

static const struct { int32_t x, z; const char *wo; } BSTART[] = {
    { -6900, -24700, "Lauerplatz" }, {  5000, -24000, "SO" },
    { -6000,  -8000, "NW" },         {  3000,  -7000, "NO" },
};
static const struct { int32_t x, z; const char *wo; } BLEON[] = {
    { -5500, -22000, "Sued-West" }, {  4000, -21000, "Sued-Ost" },
    { -4000,  -9500, "Nord-West" }, {  3500,  -8000, "Nord-Ost" },
    { -4500, -16000, "Westgasse" }, {  4700, -12000, "Ost-Nord" },
};

int main(void)
{
    size_t sz = 0, n = 0, es = 0;
    uint8_t *emd = NULL, *buf = NULL, *ems = NULL;
    re15_enemy_bank_t B;
    re15_actor_t *pl, *e;
    int slot, sz_i, gi, lj;
    int faelle = 0, treffer_n = 0, lunge_n = 0, lunge_treffer = 0;
    long sum_bis_treffer = 0, sum_fenster = 0, fenster_n = 0;
    int max_fenster = 0, min_fenster = 99999;
    long sum_strecke = 0; int strecke_n = 0;
    long sum_totzeit = 0; int totzeit_n = 0; int max_totzeit = 0;
    const char *b2 = getenv("RE15_GB_LUNGE_B2");

    /* ---- Teil 0: Clip-Laengen + B2-Fenster aus den Daten ---- */
    emd = slurp(RE15_ASSET_RE2_DIR "/EM23.EMD", &sz);
    if (!emd) { printf("FAIL EM23.EMD\n"); return 1; }
    memset(&B, 0, sizeof B);
    if (re2_emd_parse_bank(emd, sz, &B) != 0) { printf("FAIL parse EM23\n"); return 1; }
    printf("=== TEIL 0: Clip-Laengen (EM23.EMD EDD1) ===\n");
    {   int c;
        for (c = 0; c < B.anim.clip_count && c < 12; c++)
            printf("  Clip %2d: %d Bilder\n", c, B.anim.clips[c].frame_count);
    }
    {   int n2 = B.anim.clip_count > 2 ? B.anim.clips[2].frame_count : 0;
        int n3 = B.anim.clip_count > 3 ? B.anim.clips[3].frame_count : 0;
        int n4 = B.anim.clip_count > 4 ? B.anim.clips[4].frame_count : 0;
        printf("  HEUTE   (Clip 4): %d Bilder = %d Ticks; Fenster af 6..34 -> Treffer "
               "fruehestens Tick 6, spaetestens Tick 34\n", n4, n4);
        printf("  B2 einfach (Clip 2 + Clip 3, 1 Bild/Tick): %d + %d = %d Ticks; "
               "Fenster Clip3 f>=114 -> Tick %d..%d\n",
               n2, n3, n2 + n3, n2 + 114, n2 + n3 - 1);
        printf("  B2 DOPPEL  (+0x22C != 0, 2 Bilder/Tick):   %d + %d = %d Ticks; "
               "Fenster -> Tick %d..%d\n",
               (n2 + 1) / 2, (n3 + 1) / 2, (n2 + 1) / 2 + (n3 + 1) / 2,
               (n2 + 1) / 2 + 114 / 2, (n2 + 1) / 2 + (n3 - 1) / 2);
        printf("  Vortrieb im Anlauf (GB_LUNGE_SPEED 64/Tick): heute bis Treffer "
               "%d..%d Einheiten; B2-doppelt %d Einheiten bis Fensterbeginn\n",
               6 * 64, 34 * 64, ((n2 + 1) / 2 + 114 / 2) * 64);
    }

    /* ---- Teil 1: Biss-Raster am echten Boss-Tick ---- */
    printf("\n=== TEIL 1: Biss-Raster (72 Faelle a 2400 Bilder), RE15_GB_LUNGE_B2=%s ===\n",
           b2 ? b2 : "(aus)");
#ifdef _WIN32
    _putenv("RE15_GB_STUMM=1");
    _putenv("RE15_GB_TEST=1");     /* aggro sofort - identisch zu probe_gator_sweep.c:81,
                                    * damit ALLE 72 Faelle einen aktiven Boss haben */
#else
    setenv("RE15_GB_STUMM", "1", 1);
    setenv("RE15_GB_TEST", "1", 1);
#endif
    buf = slurp(RE15_ASSET_PSX_DIR "/STAGE2/ROOM2090.RDT", &n);
    if (!buf || re15_rdt_parse(buf, n, &g_room_rdt) != 0) { printf("FAIL RDT\n"); return 1; }
    g_room_rdt_ok = 1;
    re15_ai_flavor_set(RE15_AI_FLAVOR_RE2);
    re15_actor_init(); re15_aot_init();
    re15_enemy_reset(); re15_enemy_ai_set_paused(0);
    re15_player_cmd_reset();
    re15_damage_seed_rng(0x2545f491u);
    g_current_room_id = 0x2090;
    ems = slurp(RE15_ASSET_RE2_DIR "/CDEMD0.EMS", &es);
    if (ems) {
        re15_enemy_bank_t *eb = re15_enemy_alloc(0x23u);
        if (eb && re2_ems_load_bank(ems, es, 0x23, eb, NULL) == 0) { eb->buf = NULL; eb->ok = 1; }
    }
    pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    slot = RE15_ACTOR_MAX - 1; e = &g_actors[slot];

    for (sz_i = 0; sz_i < 3; sz_i++)
    for (gi = 0; gi < (int)(sizeof BSTART / sizeof BSTART[0]); gi++)
    for (lj = 0; lj < (int)(sizeof BLEON / sizeof BLEON[0]); lj++) {
        int32_t lx = BLEON[lj].x, lz = BLEON[lj].z;
        int f, treffer = 0, tf = -1;
        int in_lunge = 0, lunge_start = -1, lunge_case = 0, lunge_case_hit = 0;
        int totzeit = 0;   /* Ticks im Anlauf: Maul am Ziel (<=1500), Fenster noch zu */
        int32_t lstart_x = 0, lstart_z = 0;
        uint8_t prev_motion = 0;
        memset(e, 0, sizeof *e);
        e->active = 1; e->type = 0x23u;
        e->x = BSTART[gi].x; e->y = 0; e->z = BSTART[gi].z;
        e->grid_id = 0; e->state = 0; e->em_flag_id = 0xFF;
        re15_enemy_apply_hitbox(e, 0x23u);
        pl->active = 1; pl->type = 0; pl->hp = 100;
        pl->x = lx; pl->z = lz; pl->y = 0;
        re15_gator_boss_tick(slot);
        e->x = BSTART[gi].x; e->z = BSTART[gi].z; e->y = -1200;
        for (f = 0; f < 2400 && !treffer; f++) {
            int32_t ox, oz;
            pl->active = 1; pl->type = 0; pl->hp = 100; pl->hit_react = 0;
            pl->state = 0; pl->motion = 0;
            pl->x = lx; pl->z = lz; pl->y = 0; pl->floor = 0;
            ox = e->x; oz = e->z;
            re15_gator_boss_tick(slot);
            if ((e->x != ox || e->z != oz) && !re15_gator_boss_skip_clamp(e)) {
                int32_t nx = e->x, nz = e->z;
                re15_collision_constrain_enemy(&g_room_rdt, ox, oz, &nx, &nz,
                                               e->hit_radius_min, e->y, 4u);
                e->x = nx; e->z = nz;
            }
            /* Anlauf-Erkennung: Eintritt in den Angriffs-Clip (heute 4, B2 Clip 2). */
            if ((e->motion == 4 || e->motion == 2) && prev_motion != e->motion
                && e->anim_frame <= 1) {
                in_lunge = 1; lunge_start = f; lunge_case++; totzeit = 0;
                lstart_x = e->x; lstart_z = e->z;
            } else if (in_lunge && e->motion != 4 && e->motion != 2 && e->motion != 3) {
                in_lunge = 0;
                {   int64_t dx = e->x - lstart_x, dz = e->z - lstart_z;
                    int32_t d = 0; int64_t d2 = dx*dx + dz*dz;
                    while ((int64_t)d*d < d2 && d < 40000) d += 32;
                    sum_strecke += d; strecke_n++; }
            }
            prev_motion = e->motion;
            /* TOTZEIT: das Maul steht schon am Ziel (Trefferkriterium <=1500,
             * enemy_ai_boss_gator.c:1543/:1562), das Bissfenster ist aber noch zu. */
            if (in_lunge && p_maul_dist(e, pl) <= 1500) totzeit++;
            if (pl->hp < 100 || (pl->hit_react & 1)) {
                treffer = 1; tf = f;
                if (in_lunge && lunge_start >= 0) {
                    int w = f - lunge_start;
                    lunge_case_hit++;
                    sum_fenster += w; fenster_n++;
                    if (w > max_fenster) max_fenster = w;
                    if (w < min_fenster) min_fenster = w;
                    sum_totzeit += totzeit - 1; totzeit_n++;   /* -1: der Treffertick selbst */
                    if (totzeit - 1 > max_totzeit) max_totzeit = totzeit - 1;
                }
                break;
            }
            if (sz_i != 0) {                       /* Leon flieht (1) / kreist (2) */
                int64_t dx = (int64_t)lx - e->x, dz = (int64_t)lz - e->z;
                int64_t d2 = dx * dx + dz * dz;
                int32_t d = 0, vx, vz;
                while ((int64_t)d * d < d2 && d < 40000) d += 32;
                if (d < 1) d = 1;
                if (sz_i == 1) { vx = (int32_t)(dx * 55 / d); vz = (int32_t)(dz * 55 / d); }
                else { int dir = ((f / 240) & 1) ? 1 : -1;
                       vx = (int32_t)(-dz * 55 * dir / d); vz = (int32_t)( dx * 55 * dir / d); }
                lx += vx; lz += vz;
                if (lx < -8600) lx = -8600; else if (lx > 6900) lx = 6900;
                if (lz < -26700) lz = -26700; else if (lz > -5700) lz = -5700;
                if (lx > -1800 && lx < 1950 && lz > -20100 && lz < -12350) {
                    int32_t dw = lx + 1800, de_ = 1950 - lx;
                    int32_t ds = lz + 20100, dn = -12350 - lz;
                    int32_t m = dw; int k = 0;
                    if (de_ < m) { m = de_; k = 1; }
                    if (ds  < m) { m = ds;  k = 2; }
                    if (dn  < m) { m = dn;  k = 3; }
                    if (k == 0) lx = -1800; else if (k == 1) lx = 1950;
                    else if (k == 2) lz = -20100; else lz = -12350;
                }
                if (lx > 1750 && lz > -18200 && lz < -14400) {
                    int32_t ds = lz + 18200, dn = -14400 - lz, dw = lx - 1750;
                    if (dw <= ds && dw <= dn) lx = 1750;
                    else if (ds < dn) lz = -18200; else lz = -14400;
                }
            }
        }
        faelle++;
        lunge_n += lunge_case;
        lunge_treffer += lunge_case_hit;
        if (treffer) { treffer_n++; sum_bis_treffer += tf; }
        else printf("  OHNE TREFFER: sz=%d gator=%-11s leon=%-10s anlaeufe=%d "
                    "gator_ende=(%ld,%ld) leon_ende=(%ld,%ld)\n",
                    sz_i, BSTART[gi].wo, BLEON[lj].wo, lunge_case,
                    (long)e->x, (long)e->z, (long)lx, (long)lz);
        e->active = 0;
    }
    printf("\nERGEBNIS  Faelle=%d  Treffer=%d (%.1f%%)  Bilder bis Treffer im Mittel=%ld\n",
           faelle, treffer_n, faelle ? 100.0 * treffer_n / faelle : 0.0,
           treffer_n ? sum_bis_treffer / treffer_n : -1);
    printf("          Anlaeufe=%d  davon mit Treffer=%d (%.1f%%)  "
           "Anlauf->Treffer: min %d / Mittel %ld / max %d Ticks\n",
           lunge_n, lunge_treffer, lunge_n ? 100.0 * lunge_treffer / lunge_n : 0.0,
           fenster_n ? min_fenster : -1, fenster_n ? sum_fenster / fenster_n : -1,
           max_fenster);
    printf("          TOTZEIT im treffenden Anlauf (Maul <=1500 am Ziel, Fenster zu): "
           "Mittel %ld / max %d Ticks (n=%d)\n",
           totzeit_n ? sum_totzeit / totzeit_n : -1, max_totzeit, totzeit_n);
    printf("          Strecke je abgebrochenem Anlauf im Mittel=%ld Einheiten (n=%d)\n",
           strecke_n ? sum_strecke / strecke_n : -1, strecke_n);
    free(emd);
    return 0;
}
