/* Runde 41 Spur G "kanister2080", Nachbesserung 1 (Abnahme 0, M2 / Nachtrag 23): wo muss der Kanister im 2090-Becken liegen, damit
 * der Alligator ihn im weiteren Kampf aufnimmt? Treibt den ECHTEN Boss-Tick (re15_gator_boss_tick + Dispatcher-Klemme, Aufbau wie
 * tests/unit/test_r41_re2gator.c setup(0): KEIN RE15_GB_TEST, kein RE15_GATOR_KANISTER) im Zustand NACH der Kanister-Szene: frischer
 * Aktor, untergetaucht am Lauerplatz (B9), Auftauchen bei Annaeherung < 9000 (GB_AGGRO, Nutzer-Entscheid N22.2), RE2-Intro, dann
 * Verfolgung. Leon steht (Statue, hp 100) an einer Lage P; der Kanister liegt (Spur-B-API re15_gr2_kanister_setzen, wie
 * kanister2080_r41.c) an einer Kandidaten-Stelle S. Gemessen je (P,S): Bild der Aufnahme (re15_gr2_kanister_stufe >= 1) oder nie.
 * Aufruf: probe_liegestelle <shared_assets/PSX> <shared_assets/RE2> [bilder] [sx sz]   (ohne sx/sz: Raster ueber den Westteil)
 *         probe_liegestelle <PSX> <RE2> <bilder> raster <x0> <x1> <z0> <z1> <schritt>   (Raster in einem Ausschnitt)
 * Ausgabe: je Stelle Zahl der Leon-Lagen mit Aufnahme, Mittel/Max der Bilder nach dem Auftauchen; mit sx sz je Leon-Lage eine Zeile.
 * Grenze des Modells: Leon steht still (ein Spieler bewegt sich und schiesst) - das Raster ordnet die Stellen, die exe-Laeufe
 * (Dossier 8.2) messen den echten Ablauf. */
#include "re15_rdt.h"
#include "re15_scd.h"
#include "re15_actor.h"
#include "re15_aot.h"
#include "re15_room.h"
#include "re15_enemy_ai.h"
#include "re15_enemy.h"
#include "re15_ai_flavor.h"
#include "re15_player.h"
#include "re15_damage.h"
#include "re15_collision.h"
#include "re15_boss_gator.h"
#include "re15_gator_re2_r41.h"
#include "re2_ems.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

static uint8_t *slurp(const char *path, size_t *n)
{
    FILE *f = fopen(path, "rb"); if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    uint8_t *b = (uint8_t *)malloc((size_t)sz);
    if (b && fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); b = NULL; }
    fclose(f); *n = (size_t)sz; return b;
}
static uint8_t *s_rdt; static size_t s_rdt_n;
static uint8_t *s_ems; static size_t s_ems_n;
static int slot; static re15_actor_t *e, *pl;
static void se_nop(int id, int f) { (void)id; (void)f; }
static void bank_nop(int b) { (void)b; }
static int setup(void)
{
#ifdef _WIN32
    _putenv("RE15_GB_TEST="); _putenv("RE15_GB_STUMM=1"); _putenv("RE15_GATOR_KANISTER=");
#else
    unsetenv("RE15_GB_TEST"); setenv("RE15_GB_STUMM", "1", 1); unsetenv("RE15_GATOR_KANISTER");
#endif
    if (re15_rdt_parse(s_rdt, s_rdt_n, &g_room_rdt) != 0) return 0;
    g_room_rdt_ok = 1;
    re15_ai_flavor_set(RE15_AI_FLAVOR_RE15);
    re15_actor_init(); re15_aot_init();
    re15_enemy_reset(); re15_enemy_ai_set_paused(0);
    re15_player_cmd_reset();
    re15_damage_seed_rng(0x2545f491u);
    g_current_room_id = 0x2090;
    re15_enemy_bank_t *eb = re15_enemy_find(0x23u);
    if (!eb || !eb->ok) {
        if (!eb) eb = re15_enemy_alloc(0x23u);
        if (!eb || re2_ems_load_bank(s_ems, s_ems_n, 0x23, eb, NULL) != 0) return 0;
        eb->buf = NULL; eb->ok = 1;
    }
    pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    slot = RE15_ACTOR_MAX - 1;
    e = &g_actors[slot];
    memset(e, 0, sizeof *e);
    e->active = 1; e->type = 0x23u;
    e->x = -5300; e->y = 0; e->z = -16300;            /* Port-Spawn (main.c, dritte Wasserzone), vor dem Boss-INIT */
    e->grid_id = 0; e->state = 0; e->em_flag_id = 0xFF;
    re15_enemy_apply_hitbox(e, 0x23u);
    pl->active = 1; pl->type = 0; pl->hp = 100;
    re15_gator_audio_hook(se_nop, bank_nop);
    re15_gr2_raum_se_hook = NULL;
    return 1;
}
static void tick(void)
{
    int32_t ox = e->x, oz = e->z;
    re15_gator_boss_tick(slot);
    if ((e->x != ox || e->z != oz) && !re15_gator_boss_skip_clamp(e)) {
        int32_t nx = e->x, nz = e->z;
        re15_collision_constrain_enemy(&g_room_rdt, ox, oz, &nx, &nz, e->hit_radius_min, e->y, 4u);
        e->x = nx; e->z = nz;
    }
}
static void statue(int32_t x, int32_t z)
{
    pl->x = x; pl->z = z; pl->y = 0; pl->floor = 0; pl->state = 0; pl->motion = 0; pl->hit_react = 0; pl->hp = 100;
}
/* 1 = Aufnahme; *bild = Bild seit Raumeintritt; *auf = Bild des Auftauchens (-1 nie) */
static int lauf(int32_t px, int32_t pz, int32_t sx, int32_t sz, int bilder, int *bild, int *auf)
{
    if (!setup()) return -1;
    statue(px, pz);
    re15_gr2_kanister_setzen(sx, -1200, sz);           /* wie kanister2080_r41.c beim Raumeintritt (Bit 116) */
    *auf = -1; *bild = -1;
    for (int f = 0; f < bilder; f++) {
        statue(px, pz);
        tick();
        if (*auf < 0 && !re15_gator_boss_untergetaucht(e)) *auf = f;
        if (re15_gr2_kanister_stufe() >= 1) { *bild = f; break; }
    }
    return *bild >= 0;
}
static int wasser(int32_t x, int32_t z, int32_t m)
{
    if (x < -8900 + m || x > 7200 - m || z < -27000 + m || z > -5400 - m) return 0;
    if (x > -1700 - m && x < 1850 + m && z > -20000 - m && z < -12450 + m) return 0;    /* Plattform SCA [4] */
    if (x > 1850 - m && x < 7200 && z > -18100 - m && z < -14500 + m) return 0;          /* Rampe SCA [5] */
    return 1;
}
int main(int argc, char **argv)
{
    if (argc < 3) { fprintf(stderr, "Aufruf: %s <PSX-Dir> <RE2-Dir> [bilder] [sx sz]\n", argv[0]); return 2; }
    const int bilder = argc > 3 ? atoi(argv[3]) : 1500;
    char p[512];
    snprintf(p, sizeof p, "%s/STAGE2/ROOM2090.RDT", argv[1]); s_rdt = slurp(p, &s_rdt_n);
    snprintf(p, sizeof p, "%s/CDEMD0.EMS", argv[2]); s_ems = slurp(p, &s_ems_n);
    if (!s_rdt || !s_ems) { fprintf(stderr, "Daten fehlen\n"); return 1; }
    /* Leon-Lagen: Wasser (Rand 600), naeher als 9000 am Lauerplatz (-7200,-9200) = nur dort taucht er nach der Szene auf */
    int32_t L[64][2]; int nl = 0;
    for (int32_t z = -18000; z <= -6000 && nl < 64; z += 1500)
        for (int32_t x = -8300; x <= 5200 && nl < 64; x += 1500) {
            int64_t dx = x + 7200, dz = z + 9200;
            if (dx * dx + dz * dz >= (int64_t)9000 * 9000 || !wasser(x, z, 600)) continue;
            L[nl][0] = x; L[nl][1] = z; nl++;
        }
    int32_t gx0 = -8000, gx1 = -2500, gz0 = -24000, gz1 = -9000, gst = 500, gstz = 1000;
    if (argc > 9 && !strcmp(argv[4], "raster")) {
        gx0 = atoi(argv[5]); gx1 = atoi(argv[6]); gz0 = atoi(argv[7]); gz1 = atoi(argv[8]); gst = gstz = atoi(argv[9]);
    } else if (argc > 5) {                             /* eine Stelle, je Leon-Lage eine Zeile */
        const int32_t sx = atoi(argv[4]), sz = atoi(argv[5]);
        int n = 0;
        for (int i = 0; i < nl; i++) {
            int b = -1, a = -1;
            int r = lauf(L[i][0], L[i][1], sx, sz, bilder, &b, &a);
            printf("Leon (%6d,%6d): Auftauchen F%d, %s", (int)L[i][0], (int)L[i][1], a, r > 0 ? "Aufnahme " : "keine Aufnahme");
            if (r > 0) { printf("F%d (%d Bilder nach dem Auftauchen)", b, b - a); n++; }
            printf("\n");
        }
        printf("Stelle (%d,%d): %d von %d Leon-Lagen mit Aufnahme in %d Bildern\n", (int)sx, (int)sz, n, nl, bilder);
        return 0;
    }
    printf("%d Leon-Lagen, %d Bilder je Lauf\n", nl, bilder);
    for (int32_t sz = gz0; sz <= gz1; sz += gstz)
        for (int32_t sx = gx0; sx <= gx1; sx += gst) {
            if (!wasser(sx, sz, 300)) continue;
            int n = 0, summe = 0, maxb = 0;
            for (int i = 0; i < nl; i++) {
                int b = -1, a = -1;
                if (lauf(L[i][0], L[i][1], sx, sz, bilder, &b, &a) > 0) { n++; summe += b - a; if (b - a > maxb) maxb = b - a; }
            }
            printf("Stelle (%6d,%6d): %2d/%d Aufnahmen, mittel %4d, max %4d Bilder nach dem Auftauchen\n", (int)sx, (int)sz, n, nl,
                   n ? summe / n : -1, maxb);
            fflush(stdout);
        }
    return 0;
}
