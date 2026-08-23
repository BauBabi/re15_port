/**
 * @file test_ai_flavor_mixed.c
 * @brief PIN fuer die DRITTE KI-Stufe "AI MIXED" (RE15_AI_FLAVOR_MIXED, 2026-08-23).
 *
 * NUTZER-AUFTRAG (woertlich): „Und ich haette gerne eine weitere Option im Optionsmenue
 * **RE AI Mixed**, die beinhaltet im Prinzip **alles von RE 1.5 AI ausser die Hunde, die sollen
 * komplett von RE 2 AI stammen**."
 *
 * ⛔ KEIN byte-true Zustand, sondern eine Port-Option — dieser Test pinnt deshalb die AUSWAHL,
 * nicht die Original-Bytes. Die byte-true-Kampagnen darunter (RE1.5- und RE2-Verhalten) sind
 * unberuehrt und haben ihre eigenen PINs.
 *
 * ---------------------------------------------------------------------------------------------
 * ABSCHNITT 1 — der TYP-RESOLVER re15_ai_re2_for_type() als Wahrheitstabelle.
 *   Nach dem Umbau gibt es KEINEN globalen `flavor == RE2`-Vergleich mehr: alle vormals 30
 *   Vergleichsstellen fragen diese eine Funktion. Wer sie bricht, bricht alle drei Stufen —
 *   deshalb ist sie hier vollstaendig (3 Flavors x 11 Typen) festgenagelt.
 *
 * ABSCHNITT 2 — die Flavor-KLEMME (Fremdwerte fallen auf RE1.5, gueltige gehen 1:1 durch).
 *
 * ABSCHNITT 3 — POSITIVFALL, live in ROOM1190 (dem RE1.5-Hunderaum): im MIXED-Modus faehrt der
 *   HUND das RE2-Brain. Zwei unabhaengige Belege:
 *     (a) MECHANISMUS: e->re2d_stuck230 ist der Stuck-Zaehler des RE2-Root-Epilogs
 *         (@0x80100090-D8, enemy_ai_re2_dog.c:2127). KEIN anderer Codepfad im Port schreibt
 *         dieses Feld (Zensus: nur enemy_ai_re2_dog.c) — es ist damit der direkte Beweis, dass
 *         re15_re2dog_tick gelaufen ist.
 *     (b) AEQUIVALENZ: die vollstaendige Zustands-Spur des Raums ueber N Frames ist unter MIXED
 *         BIT-IDENTISCH zu RE2 und VERSCHIEDEN von RE1.5.
 *
 * ABSCHNITT 4 — NEGATIVFALL, live in ROOM1140 (dem Zombie-Raum): im MIXED-Modus faehrt der
 *   ZOMBIE das RE1.5-Brain. Spur unter MIXED BIT-IDENTISCH zu RE1.5 und VERSCHIEDEN von RE2.
 *
 * ABSCHNITT 5 — die uebrigen RE2-faehigen Typen (Kraehe 0x21, Spinnen 0x25/0x26) bleiben in
 *   MIXED auf RE1.5, und die Port-Option "RE2-Zombie-Uebernahme im RE1.5-Modus"
 *   (re15_re15_re2z_import_owns) gilt in MIXED genauso wie in RE1.5 — sonst waere MIXED fuer
 *   Zombies eben NICHT "alles wie RE1.5".
 */
#include "re15_rdt.h"
#include "re15_scd.h"
#include "re15_actor.h"
#include "re15_aot.h"
#include "re15_enemy_ai.h"
#include "re15_enemy.h"
#include "re15_ai_flavor.h"
#include "re15_player.h"    /* re15_actors_anim_advance */
#include "re15_damage.h"    /* re15_damage_seed_rng */
#include "re2_ems.h"        /* echte RE2-Bank fuer die Hunde-Laeufe */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef RE15_ASSET_PSX_DIR
#define RE15_ASSET_PSX_DIR "shared_assets/PSX"
#endif

static int fails = 0;
#define CHECK(c, ...) do { if (!(c)) { printf("FAIL: " __VA_ARGS__); printf("\n"); fails++; } } while (0)

static uint8_t *slurp(const char *path, long *out_sz)
{
    FILE *f = fopen(path, "rb");
    if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    if (sz <= 0) { fclose(f); return NULL; }
    uint8_t *b = (uint8_t *)malloc((size_t)sz);
    if (b && fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); b = NULL; }
    fclose(f);
    if (b) *out_sz = sz;
    return b;
}

/* ============================================================================================
 * ABSCHNITT 1 + 2 — Resolver + Klemme (reine Funktionen, kein Raum noetig)
 * ========================================================================================== */
static const char *flavor_name(re15_ai_flavor_t f)
{
    return f == RE15_AI_FLAVOR_RE15 ? "RE1.5" : f == RE15_AI_FLAVOR_RE2 ? "RE2" : "MIXED";
}

static void section_resolver(void)
{
    /* Repraesentative Typen: die ganze Zombie-Familie (re15_re2z_owns_type), Hund, Kraehe,
     * beide Spinnen und ein Typ ohne jedes RE2-Brain (Gorilla-Boss 0x27). */
    static const unsigned types[] = { 0x10, 0x11, 0x12, 0x13, 0x16, 0x18,
                                      0x20, 0x21, 0x25, 0x26, 0x27 };
    static const int n = (int)(sizeof types / sizeof types[0]);

    re15_ai_flavor_set(RE15_AI_FLAVOR_RE15);
    for (int i = 0; i < n; i++)
        CHECK(re15_ai_re2_for_type(types[i]) == 0,
              "RE1.5: KEIN Typ darf RE2 fahren, Typ 0x%02X lieferte 1", types[i]);

    re15_ai_flavor_set(RE15_AI_FLAVOR_RE2);
    for (int i = 0; i < n; i++)
        CHECK(re15_ai_re2_for_type(types[i]) == 1,
              "RE2: JEDER Typ muss RE2 fahren, Typ 0x%02X lieferte 0", types[i]);

    re15_ai_flavor_set(RE15_AI_FLAVOR_MIXED);
    for (int i = 0; i < n; i++) {
        int want = (types[i] == 0x20u);      /* der Auftrag: NUR die Hunde */
        CHECK(re15_ai_re2_for_type(types[i]) == want,
              "MIXED: Typ 0x%02X muss %s sein, lieferte %d",
              types[i], want ? "RE2" : "RE1.5", re15_ai_re2_for_type(types[i]));
    }
    printf("  [1] Resolver-Tabelle 3 Flavors x %d Typen geprueft\n", n);

    /* ABSCHNITT 2 — Klemme: gueltige Werte 1:1, alles andere auf den byte-true RE1.5-Zustand. */
    re15_ai_flavor_set(RE15_AI_FLAVOR_RE15);
    CHECK(re15_ai_flavor() == RE15_AI_FLAVOR_RE15, "set(RE15) muss RE15 liefern");
    re15_ai_flavor_set(RE15_AI_FLAVOR_RE2);
    CHECK(re15_ai_flavor() == RE15_AI_FLAVOR_RE2, "set(RE2) muss RE2 liefern");
    re15_ai_flavor_set(RE15_AI_FLAVOR_MIXED);
    CHECK(re15_ai_flavor() == RE15_AI_FLAVOR_MIXED, "set(MIXED) muss MIXED liefern");
    re15_ai_flavor_set((re15_ai_flavor_t)7);     /* Fremdwert (alter Stand / atoi-Muell) */
    CHECK(re15_ai_flavor() == RE15_AI_FLAVOR_RE15,
          "Fremdwert 7 muss auf RE1.5 klemmen, wurde %s", flavor_name(re15_ai_flavor()));
    re15_ai_flavor_set((re15_ai_flavor_t)-1);
    CHECK(re15_ai_flavor() == RE15_AI_FLAVOR_RE15,
          "Fremdwert -1 muss auf RE1.5 klemmen, wurde %s", flavor_name(re15_ai_flavor()));
    printf("  [2] Flavor-Klemme geprueft (0/1/2 durch, Fremdwert -> RE1.5)\n");
}

/* ============================================================================================
 * Live-Laeufe: EIN Raum, EIN deterministischer Spieler-Pfad, drei Flavors.
 * ========================================================================================== */
static void frame(void) { re15_enemy_ai_run_all(1); re15_actors_anim_advance(); }

/* FNV-1a ueber die spielrelevanten Aktor-Felder — die "Zustands-Spur" des Laufs. */
static uint64_t trace_mix(uint64_t h, const void *p, size_t n)
{
    const uint8_t *b = (const uint8_t *)p;
    for (size_t i = 0; i < n; i++) { h ^= b[i]; h *= 0x100000001b3ull; }
    return h;
}

typedef struct {
    uint64_t hash;        /* Spur ueber alle Frames */
    int      n_actors;
    int      stuck230;    /* MAXIMUM des RE2-Root-Stuck-Zaehlers ueber ALLE Frames.
                           * ⚠️ NICHT der Endwert: der Epilog @0x80100090-D8 NULLT den Zaehler in
                           * jedem Frame, in dem sich der Hund bewegt hat (enemy_ai_re2_dog.c:2128)
                           * — beim jagenden Hund steht am Ende deshalb 0, obwohl das RE2-Brain
                           * die ganze Zeit lief (erst gemessen, dann korrigiert). */
    int      max_sub;     /* hoechster erreichter ACTIVE-Substate (Diagnose) */
} run_result_t;

/* Bringup wie test_re2_room1190_ab.c (Skill re15-room-probe), plus expliziter RE2-RNG-Reset,
 * damit die drei Laeufe wirklich vergleichbar sind. */
static int bringup(const re15_rdt_t *rdt, int extra_sub, uint8_t want_type,
                   int slots[], int *n)
{
    re15_actor_init();
    re15_aot_init();
    scd_vm_init();
    re15_enemy_ai_set_paused(0);
    re15_damage_seed_rng(0x0badf00du);
    re15_re2z_rng_reset();            /* RE2-PRNG + 0x800CFBF4-Analog + One-Save-Latch */
    if (rdt->main_scd) scd_thread_start(0, rdt->main_scd);
    scd_thread_start(1, rdt->sub_scd[0]);
    if (extra_sub >= 0 && rdt->sub_scd_count > extra_sub && rdt->sub_scd[extra_sub])
        scd_thread_start(2, rdt->sub_scd[extra_sub]);
    g_scd.work_vars[10] = 0;
    for (int i = 0; i < 120; i++) scd_vm_tick();
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->x = 0; pl->y = 0; pl->z = 0;
    pl->hp = 1000;                    /* nicht sterben — der Pfad soll ueber alle Frames laufen */
    pl->hit_react = 0; pl->state = 0; pl->motion = 0; pl->floor = 0; pl->rot_y = 0;
    *n = 0;
    for (int s = 1; s < RE15_ACTOR_MAX; s++)
        if (g_actors[s].active && g_actors[s].type == want_type) slots[(*n)++] = s;
    return (*n >= 1) ? 0 : -1;
}

/* Die ECHTE RE2-Bank fuer `type` in die Registry legen — 1:1 wie platform/pc/main.c
 * pc_enemy_load_ex es unter RE2 UND (neu) unter MIXED fuer 0x20 tut. Ohne sie liefen die
 * Clip-Laengen aus der RE1.5-Bank und der Vergleich RE2<->MIXED waere nicht aussagekraeftig. */
static void load_re2_bank(uint8_t type)
{
    static uint8_t *ems = NULL; static long ems_sz = 0;
    if (!ems) ems = slurp(RE15_ASSET_PSX_DIR "/../RE2/CDEMD0.EMS", &ems_sz);
    if (!ems) return;
    re15_enemy_bank_t *eb = re15_enemy_find(type);
    if (!eb) eb = re15_enemy_alloc(type);
    if (eb && re2_ems_load_bank(ems, (size_t)ems_sz, type, eb, NULL) == 0) { eb->buf = NULL; eb->ok = 1; }
}

static run_result_t run_room(const re15_rdt_t *rdt, int extra_sub, uint8_t want_type,
                             re15_ai_flavor_t fl, int re2_bank, int frames)
{
    run_result_t r; memset(&r, 0, sizeof r);
    int slots[RE15_ACTOR_MAX], n = 0;
    re15_ai_flavor_set(fl);
    if (bringup(rdt, extra_sub, want_type, slots, &n) != 0) { r.n_actors = 0; return r; }
    if (re2_bank) load_re2_bank(want_type);
    r.n_actors = n;

    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    re15_actor_t *a0 = &g_actors[slots[0]];
    uint64_t h = 0xcbf29ce484222325ull;
    for (int f = 0; f < frames; f++) {
        /* DERSELBE deterministische Pfad in allen Laeufen: der Spieler steht rennend 1500
         * Einheiten neben dem ersten Gegner und wird jeden Frame nachgezogen. */
        pl->x = a0->x + 1500; pl->z = a0->z; pl->motion = 100; pl->floor = a0->floor;
        pl->hp = 1000;
        frame();
        for (int i = 0; i < n; i++) {
            const re15_actor_t *e = &g_actors[slots[i]];
            struct { int32_t x, z, y; int16_t rot, hp, motion; uint8_t st, s1, s2, s3; uint16_t fr; }
                snap = { e->x, e->z, e->y, e->rot_y, e->hp, (int16_t)e->motion,
                         (uint8_t)e->state, e->sub_state_1, e->sub_state_2, e->sub_state_3,
                         e->anim_frame };
            h = trace_mix(h, &snap, sizeof snap);
            if (e->state == 1 && e->sub_state_1 > r.max_sub) r.max_sub = e->sub_state_1;
            if ((int)e->re2d_stuck230 > r.stuck230) r.stuck230 = (int)e->re2d_stuck230;
        }
    }
    r.hash = h;
    return r;
}

int main(void)
{
    printf("=== AI-FLAVOR MIXED (RE1.5 ueberall, RE2 nur Hund 0x20) ===\n");

    section_resolver();

    /* ---------------------------------------------------------------------------------------
     * ABSCHNITT 3 — POSITIVFALL: ROOM1190, die Hunde
     * ------------------------------------------------------------------------------------- */
    {
        long sz = 0;
        uint8_t *buf = slurp(RE15_ASSET_PSX_DIR "/STAGE1/ROOM1190.RDT", &sz);
        re15_rdt_t rdt;
        if (!buf || re15_rdt_parse(buf, (size_t)sz, &rdt) != 0 || !rdt.sub_scd[0]) {
            printf("FAIL: ROOM1190.RDT nicht lesbar/parsebar\n"); return 1;
        }
        /* Hunde-Welle in sub13 (Sce_em_set 0x44/type 0x20 @RDT 0x2900/0x2914/0x2928) */
        const int FR = 240;
        run_result_t a15 = run_room(&rdt, 13, 0x20, RE15_AI_FLAVOR_RE15,  0, FR);
        run_result_t a2  = run_room(&rdt, 13, 0x20, RE15_AI_FLAVOR_RE2,   1, FR);
        run_result_t am  = run_room(&rdt, 13, 0x20, RE15_AI_FLAVOR_MIXED, 1, FR);
        printf("  [3] ROOM1190 Hunde: n=%d/%d/%d  stuck230=%d/%d/%d  maxsub=%d/%d/%d\n",
               a15.n_actors, a2.n_actors, am.n_actors,
               a15.stuck230, a2.stuck230, am.stuck230,
               a15.max_sub, a2.max_sub, am.max_sub);
        CHECK(am.n_actors >= 1, "ROOM1190 muss >=1 Hund spawnen (MIXED)");

        /* (a) MECHANISMUS: re2d_stuck230 schreibt AUSSCHLIESSLICH der RE2-Root-Epilog
         *     (@0x80100090-D8). > 0 == re15_re2dog_tick ist gelaufen. */
        CHECK(am.stuck230 > 0,
              "MIXED POSITIV: der Hund MUSS das RE2-Brain fahren — re2d_stuck230 "
              "(RE2-Root-Epilog @0x80100090-D8) blieb 0");
        CHECK(a15.stuck230 == 0,
              "RE1.5 NEGATIV-Gegenprobe: kein RE2-Root darf laufen, re2d_stuck230=%d",
              a15.stuck230);
        CHECK(a2.stuck230 > 0, "RE2-Referenz: re2d_stuck230 muss > 0 sein");

        /* (b) AEQUIVALENZ: MIXED == RE2 fuer den Hund, und != RE1.5. */
        CHECK(am.hash == a2.hash,
              "MIXED muss fuer den HUND Frame fuer Frame == RE2 sein "
              "(%016llx vs %016llx)", (unsigned long long)am.hash, (unsigned long long)a2.hash);
        CHECK(am.hash != a15.hash,
              "MIXED darf fuer den HUND NICHT == RE1.5 sein (Spur identisch: %016llx)",
              (unsigned long long)am.hash);
        free(buf);
    }

    /* ---------------------------------------------------------------------------------------
     * ABSCHNITT 4 — NEGATIVFALL: ROOM1140, die Zombies
     * ------------------------------------------------------------------------------------- */
    {
        long sz = 0;
        uint8_t *buf = slurp(RE15_ASSET_PSX_DIR "/STAGE1/ROOM1140.RDT", &sz);
        re15_rdt_t rdt;
        if (!buf || re15_rdt_parse(buf, (size_t)sz, &rdt) != 0 || !rdt.sub_scd[0]) {
            printf("FAIL: ROOM1140.RDT nicht lesbar/parsebar\n"); return 1;
        }
        /* KEINE RE2-Bank laden: unter MIXED laedt pc_enemy_load_ex fuer die Zombie-Familie die
         * RE1.5-Bank, der Lauf spiegelt also den echten Asset-Zustand. RE2 bekommt sie auch
         * nicht — dann liegt die Differenz garantiert am BRAIN und nicht an Clip-Laengen. */
        const int FR = 240;
        run_result_t z15 = run_room(&rdt, -1, 0x10, RE15_AI_FLAVOR_RE15,  0, FR);
        run_result_t z2  = run_room(&rdt, -1, 0x10, RE15_AI_FLAVOR_RE2,   0, FR);
        run_result_t zm  = run_room(&rdt, -1, 0x10, RE15_AI_FLAVOR_MIXED, 0, FR);
        printf("  [4] ROOM1140 Zombies: n=%d/%d/%d  maxsub=%d/%d/%d\n",
               z15.n_actors, z2.n_actors, zm.n_actors, z15.max_sub, z2.max_sub, zm.max_sub);
        CHECK(zm.n_actors >= 1, "ROOM1140 muss >=1 Zombie spawnen (MIXED)");
        CHECK(zm.hash == z15.hash,
              "MIXED muss fuer den ZOMBIE Frame fuer Frame == RE1.5 sein "
              "(%016llx vs %016llx)", (unsigned long long)zm.hash, (unsigned long long)z15.hash);
        CHECK(zm.hash != z2.hash,
              "MIXED darf fuer den ZOMBIE NICHT == RE2 sein (Spur identisch: %016llx)",
              (unsigned long long)zm.hash);
        free(buf);
    }

    /* ---------------------------------------------------------------------------------------
     * ABSCHNITT 5 — die uebrigen RE2-faehigen Typen + die RE1.5-Port-Option
     * ------------------------------------------------------------------------------------- */
    {
        /* Kraehe/Spinnen: in MIXED explizit RE1.5 (der Auftrag nennt NUR die Hunde). */
        re15_ai_flavor_set(RE15_AI_FLAVOR_MIXED);
        CHECK(re15_ai_re2_for_type(0x21) == 0, "MIXED: Kraehe 0x21 muss RE1.5 bleiben");
        CHECK(re15_ai_re2_for_type(0x25) == 0, "MIXED: Adult-Spinne 0x25 muss RE1.5 bleiben");
        CHECK(re15_ai_re2_for_type(0x26) == 0, "MIXED: 0x26 muss RE1.5 bleiben");

        /* Die Port-Option "RE2-Zombie-Uebernahme im RE1.5-Modus" (Zerleger + Schadensmodell,
         * Default AN) MUSS in MIXED genauso greifen wie in RE1.5 — sonst waere MIXED fuer
         * Zombies NICHT "alles wie RE1.5". Gegenprobe: in RE2 greift sie nie (dort laeuft das
         * RE2-Brain selbst), und der Hund gehoert ihr in keinem Modus. */
        re15_re15_re2z_import_set(1);
        re15_ai_flavor_set(RE15_AI_FLAVOR_RE15);
        int imp15 = re15_re15_re2z_import_owns(0x10);
        re15_ai_flavor_set(RE15_AI_FLAVOR_MIXED);
        int impmx = re15_re15_re2z_import_owns(0x10);
        int impdog = re15_re15_re2z_import_owns(0x20);
        re15_ai_flavor_set(RE15_AI_FLAVOR_RE2);
        int impre2 = re15_re15_re2z_import_owns(0x10);
        CHECK(imp15 == 1, "RE1.5: die Zombie-Import-Option muss greifen");
        CHECK(impmx == 1, "MIXED: die Zombie-Import-Option muss GENAUSO greifen wie in RE1.5");
        CHECK(impdog == 0, "MIXED: der Hund gehoert der Zombie-Import-Option nie");
        CHECK(impre2 == 0, "RE2: die RE1.5-Import-Option darf nie greifen");
        printf("  [5] Kraehe/Spinne bleiben RE1.5; Zombie-Import RE1.5==MIXED==1, RE2==0\n");
    }

    /* Testkontext wiederherstellen (test_support-Konvention: byte-true RE1.5). */
    re15_ai_flavor_set(RE15_AI_FLAVOR_RE15);

    if (fails) { printf("=== %d FAIL(s) ===\n", fails); return 1; }
    printf("=== alle MIXED-PINs gruen ===\n");
    return 0;
}
