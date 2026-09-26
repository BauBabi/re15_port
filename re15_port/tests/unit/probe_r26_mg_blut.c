/* probe_r26_mg_blut.c — RIEGEL Runde 26: der MG-/Gatling-Blutstoss gegen den Pistolenstoss.
 *
 * Nutzer 2026-09-26: "Das Maschinengewehr sorgt in Resident Evil 2 fuer viel viel mehr Blut
 * bei den Zombies bei treffern."
 * Dossiers: analysis/befunde_2026-09-26/mg-mehr-blut.md + pruefung-mg-mehr-blut.md.
 *
 * GEMESSEN WIRD, WAS VERHALTEN IST — nicht "sieht mehr aus". Jeder Fall vergleicht die
 * MG-Zeile 15 (Waffe 12 Ingram / 19 MC51 -> Handler 0x80107EF0) gegen die Pistolen-Zeile 3
 * (-> Handler 0x80105438), beide ueber die HURT-Wurzel FUN_80104F40 mit derselben Bank:
 *
 *  (1) BLUTMENGE je Treffer = belegte ESP-FX-Slots. Alter Stand: MG genau EIN Slot
 *      (re2z_blood_fx_at -> scale 0x1500 ohne @0x…, splatter 0).
 *  (2) DATENGETRIEBENE Slot-Zahl: das Original klont `uVar1` Slots aus dem SUB-Record
 *      (FUN_8001BF10), und der MG wechselt sub per RNG (`andi s0,s0,0x1` @0x80108044).
 *      Ueber viele Treffer muessen deshalb BEIDE Sub-Groessen vorkommen. Alter Stand: immer
 *      derselbe eine Slot.
 *  (3) RNG-ZUEGE: Phase 0 zieht SECHS Wuerfe — einen fuer die Clip-Wahl
 *      (`jal 0x80015FE8` @0x80107F80) und FUENF fuer den Stoss, in der Reihenfolge
 *      X (@0x80108000), Y (@0x80108010), Z (@0x80108024), sub (@0x80108034),
 *      scale (@0x8010803C). Alter Stand: NULL Wuerfe fuer den Stoss.
 *  (4) ANKER Part 0 bei Zone != 0 (`addiu a2,s1,72` @0x80108064); Part 1 gilt nur fuer
 *      Zone 0 (`addiu a2,s1,244` @0x801080C0, Weiche `beq a0,zero` @0x80107FF4). Alter
 *      Stand: fest Part 1.
 *  (5) VERSATZ/STREUUNG: X = 256-2r, Y = 312-4r, Z = 256-2r mit r = 0..255
 *      (`andi v0,v0,0xff` @0x80016004) -> X,Z in [-254,256], Y in [-708,312].
 *      Alter Stand: Versatz 0.
 *  (6) SKALIERUNG 7000..9040 (`sll v0,v0,3` @0x8010804C, `addiu v0,v0,7000` @0x80108050).
 *      Alter Stand: fest 5376.
 *  (7) Die PISTOLE bleibt unberuehrt (ererbter 8er-Faecher) und der Pool laeuft unter dem
 *      Dauerfeuer-Takt des Ports NICHT ueber — reine Messung, mit Zahlen im Protokoll.
 *
 * Alle Adressen in dieser Runde selbst gelesen aus info/re2leon/COMMON/BIN/EMZ0.BIN
 * (Ladebasis 0x80100000, kein Header) bzw. info/re2leon/PSX.EXE (RAM = 0x80010000+off-0x800).
 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "re15_actor.h"
#include "re15_esp.h"
#include "re15_ai_flavor.h"

extern re15_actor_t g_actors[];

void     re15_re2z_hurt_test(re15_actor_t *e, re15_actor_t *pl);
int      re15_re2z_last_fx_part(void);
void     re15_re2z_last_fx_pos(int32_t out[3]);
uint32_t re15_re2_rand_draws(void);
uint32_t re15_re2_rand(void);
void     re15_re2z_rng_reset(void);

#ifndef RE15_ASSET_PSX_DIR
#define RE15_ASSET_PSX_DIR "shared_assets/PSX"
#endif

static int g_fail = 0;
#define CHECK(cond, ...) do { if (!(cond)) { g_fail = 1; \
    fprintf(stderr, "FAIL: " __VA_ARGS__); fprintf(stderr, "\n"); } } while (0)

static uint8_t *slurp(const char *path, long *out_sz)
{
    FILE *f = fopen(path, "rb");
    if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    uint8_t *b = (uint8_t *)malloc((size_t)sz);
    if (b && fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); b = NULL; }
    fclose(f); *out_sz = sz; return b;
}

static uint32_t rd32(const uint8_t *p, size_t o)
{
    return (uint32_t)p[o] | ((uint32_t)p[o+1] << 8) | ((uint32_t)p[o+2] << 16) | ((uint32_t)p[o+3] << 24);
}

/* Slots AN EINER POSITION zaehlen. Der Blutstoss legt alle seine Slots auf denselben Punkt
 * (Anker + a3-Versatz), die uebrigen Emitter der HURT-Wurzel liegen woanders — so trennt die
 * Sonde den Blutstoss vom Rest, ohne die Engine zu instrumentieren. */
static int slots_bei(const int32_t pos[3], uint16_t *scale_out)
{
    int n = 0;
    for (int i = 0; i < RE15_ESP_FX_MAX; i++) {
        const re15_esp_fx_t *f = re15_esp_fx_get(i);
        if (!f) continue;
        if (f->x != pos[0] || f->y != pos[1] || f->z != pos[2]) continue;
        if (n == 0 && scale_out) *scale_out = f->scale16;
        n++;
    }
    return n;
}

/* Ein Treffer-Frame. row = Zeile der 2D-Tabelle @0x8010C940 (= +0x5, Applier-Stempel
 * `sb v1,5(t0)` @0x80041AB4), col = +0x1D2 (`sb v1,466(t0)` @0x80041A9C).
 * `vorwuerfe` dreht den RNG vorher weiter, damit aufeinanderfolgende Treffer nicht identisch
 * sind. Rueckgabe: belegte FX-Slots; *draws = RNG-Zuege dieses Frames. */
static int treffer(unsigned row, unsigned col, int vorwuerfe,
                   uint32_t *draws, int *part, int32_t pos[3])
{
    re15_actor_t *e  = &g_actors[1];
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];

    memset(e, 0, sizeof *e);
    memset(pl, 0, sizeof *pl);
    e->active = 1; e->type = 0x10; e->hp = 100;
    e->x = 1000; e->y = 0; e->z = 2000; e->rot_y = 0;
    pl->active = 1; pl->hp = 100;
    pl->x = 1000; pl->y = 0; pl->z = 0;      /* Spieler noerdlich -> Peilung != rot_y */

    /* Zustand 2 (HURT), Phase 0 — der Tick nach dem Applier-Wortstore `sw v0,4(s1)`
     * @0x80047288 (Delay-Slot des `bgez v1` @0x80047284, laeuft IMMER; Little-Endian
     * nullt er +0x5/+0x6/+0x7), danach Zeilen- und Spalten-Stempel. */
    e->state         = 2;
    e->sub_state_1   = (uint8_t)row;
    e->sub_state_2   = 0;
    e->re2z_hits1d2  = (uint8_t)col;
    e->re2z_res223   = 100;   /* Flinch-Tor @0x8010506C-78 zu halten */
    e->re2z_flag222  = 0;
    e->re2z_f10e     = 0;     /* kein Kriecher (@0x80104FE8), keine Flinch-Schwelle */
    e->re2z_flags21a = 0;     /* kein Aufsteher (@0x80105014), nicht liegend (@0x80105168) */

    re15_esp_fx_reset();
    re15_re2z_rng_reset();
    for (int i = 0; i < vorwuerfe; i++) (void)re15_re2_rand();
    uint32_t d0 = re15_re2_rand_draws();
    re15_re2z_hurt_test(e, pl);
    *draws = re15_re2_rand_draws() - d0;
    *part  = re15_re2z_last_fx_part();
    re15_re2z_last_fx_pos(pos);
    return re15_esp_fx_count();
}

int main(void)
{
    printf("=== RIEGEL r26: MG-Blutstoss (0x80107EF0) gegen Pistole (0x80105438) ===\n");

    /* Baenke laden — ohne sie faellt die Slot-Zahl aus den ESP-Daten weg. */
    long rsz = 0, gsz = 0;
    uint8_t *rdt  = slurp(RE15_ASSET_PSX_DIR "/STAGE1/ROOM1140.RDT", &rsz);
    uint8_t *core = slurp(RE15_ASSET_PSX_DIR "/DATA/CORE00.ESP", &gsz);
    static re15_esp_t esp, gesp;
    if (!rdt || re15_esp_parse(rdt, (size_t)rsz, rd32(rdt, 0x4C), rd32(rdt, 0x50),
                               rd32(rdt, 0x54), rd32(rdt, 0x58), &esp) != 0) {
        fprintf(stderr, "FAIL: ESP-Bank ROOM1140 nicht ladbar\n"); return 1;
    }
    re15_esp_set_room_bank(&esp);
    if (core && re15_esp_parse_global(core, (size_t)gsz, &gesp) == 0)
        re15_esp_set_global_bank(&gesp);

    /* Referenz: wie viele Stroeme traegt der Blut-Sub-Record? Das ist die Zahl, die das
     * Original als `uVar1` aus dem Sub-Record klont (FUN_8001BF10) — sie darf NICHT im
     * Code stehen, sondern kommt aus der Bank. */
    int str0, str1;
    re15_esp_fx_reset(); str0 = re15_esp_fx_spawn_rows(&esp, 0, 0, 8000, 0, 0, 0, 0, 0);
    re15_esp_fx_reset(); str1 = re15_esp_fx_spawn_rows(&esp, 0, 1, 8000, 0, 0, 0, 0, 0);
    printf("  Bank ROOM1140: Blut-Id 0 hat sub0=%d, sub1=%d Stroeme\n", str0, str1);
    CHECK(str0 > 0 && str1 > 0, "Bank liefert keine Blut-Stroeme (sub0=%d sub1=%d)", str0, str1);

    uint32_t d_mg = 0, d_pi = 0;
    int      p_mg = -1, p_pi = -1;
    int32_t  pos_mg[3] = {0,0,0}, pos_pi[3] = {0,0,0};

    /* Zeile 15 = SMG/Ingram, Zeile 18 = Gatling — die EINZIGEN beiden Zeilen der Tabelle
     * @0x8010C940, die auf 0x80107EF0 zeigen (eigener Dump @0x8010CB5C / @0x8010CBC8).
     * Spalte 1 = Grundzone RUMPF (`addiu v0,zero,1 / sb v0,466(s1)` @0x80047294-98). */
    int n_mg = treffer(15, 1, 0, &d_mg, &p_mg, pos_mg);
    int blut_mg = slots_bei(pos_mg, NULL);
    int n_pi = treffer( 3, 1, 0, &d_pi, &p_pi, pos_pi);
    int blut_pi = slots_bei(pos_pi, NULL);
    printf("  MG      Zeile 15: fx=%2d (Blut %2d)  rng=%u  part=%d  pos=(%d,%d,%d)\n",
           n_mg, blut_mg, (unsigned)d_mg, p_mg, (int)pos_mg[0], (int)pos_mg[1], (int)pos_mg[2]);
    printf("  Pistole Zeile  3: fx=%2d (Blut %2d)  rng=%u  part=%d  pos=(%d,%d,%d)\n",
           n_pi, blut_pi, (unsigned)d_pi, p_pi, (int)pos_pi[0], (int)pos_pi[1], (int)pos_pi[2]);

    /* (1) BLUTMENGE: mindestens die Stroeme des kleineren Sub-Records. Alter Stand: 1. */
    {
        int minstr = (str0 < str1) ? str0 : str1;
        CHECK(blut_mg >= minstr,
              "(1) MG-Blutstoss belegt %d Slots, die Bank traegt aber mindestens %d Stroeme je "
              "Spawner-Aufruf (FUN_8001BF10 klont uVar1 aus dem Sub-Record)", blut_mg, minstr);
        CHECK(blut_mg > 1, "(1) MG-Blutstoss ist %d Slot — das ist der alte 0x1500-Stand-in",
              blut_mg);
    }

    /* (2) RNG-WURFZAHL: gepinnt wird die DIFFERENZ zur Pistole, damit der Pin nicht an den
     *     uebrigen Wuerfen der HURT-Wurzel haengt. Fuenf eigene Zuege fuer den Stoss. */
    CHECK(d_mg >= d_pi + 5u,
          "(2) MG zieht %u RNG-Wuerfe, Pistole %u — der Stoss braucht FUENF eigene "
          "(X @0x80108000, Y @0x80108010, Z @0x80108024, sub @0x80108034, scale @0x8010803C)",
          (unsigned)d_mg, (unsigned)d_pi);

    /* (3) ANKER: Spalte 1 -> 1 % 3 != 0 -> Zweig A -> Part 0 (@0x80108064). */
    CHECK(p_mg == 0, "(3) MG-Anker ist Part %d, Zweig A des Originals = Part 0 (@0x80108064)",
          p_mg);

    /* (4)+(5)+(6): 64 Treffer mit fortlaufendem RNG-Stand. */
    {
        int32_t ank[3];
        {   /* Ankerposition OHNE Versatz: der Pistolenstoss am selben Aktor, gleicher Part. */
            uint32_t dd; int pp; int32_t p3[3];
            (void)treffer(3, 1, 0, &dd, &pp, p3);
            ank[0] = p3[0]; ank[1] = p3[1]; ank[2] = p3[2];
            CHECK(pp == p_mg, "(3) Pistole nimmt Part %d, MG Part %d — nicht vergleichbar",
                  pp, p_mg);
        }
        int aus_band = 0, gleich_anker = 0, wechsel = 0;
        int saw_str0 = 0, saw_str1 = 0, sc_min = 0x7fffffff, sc_max = 0, sc_aus = 0;
        int32_t prev[3] = {0,0,0};
        for (int i = 0; i < 64; i++) {
            uint32_t dd; int pp; int32_t p3[3];
            (void)treffer(15, 1, i, &dd, &pp, p3);
            uint16_t sc16 = 0;
            int n = slots_bei(p3, &sc16);
            if (n == str0) saw_str0 = 1;
            if (n == str1) saw_str1 = 1;
            if (i > 0 && (p3[0] != prev[0] || p3[1] != prev[1] || p3[2] != prev[2])) wechsel++;
            prev[0] = p3[0]; prev[1] = p3[1]; prev[2] = p3[2];
            int dx = (int)(p3[0] - ank[0]), dy = (int)(p3[1] - ank[1]), dz = (int)(p3[2] - ank[2]);
            if (dx == 0 && dy == 0 && dz == 0) gleich_anker++;
            if (dx < -254 || dx > 256 || dz < -254 || dz > 256 || dy < -708 || dy > 312) aus_band++;
            if (n > 0) {
                int sc = (int)sc16;
                if (sc < sc_min) sc_min = sc;
                if (sc > sc_max) sc_max = sc;
                if (sc < 7000 || sc > 9040) sc_aus++;
            }
        }
        printf("  64 Treffer: %d/64 auf dem Anker, %d/64 ausserhalb des Bandes, %d/63 Wechsel, "
               "scale %d..%d, %d ausserhalb 7000..9040\n",
               gleich_anker, aus_band, wechsel, sc_min, sc_max, sc_aus);
        printf("  Slot-Zahl datengetrieben: sub0(%d) gesehen=%d, sub1(%d) gesehen=%d\n",
               str0, saw_str0, str1, saw_str1);

        /* (4) Slot-Zahl wechselt mit dem sub-Wurf — das beweist, dass sie aus der Bank
         *     kommt und nicht aus einer Konstante. */
        CHECK(str0 == str1 || (saw_str0 && saw_str1),
              "(4) ueber 64 Treffer kam nur EINE Sub-Groesse vor (sub0=%d gesehen=%d, "
              "sub1=%d gesehen=%d) — der sub-Wurf @0x80108034 wirkt nicht",
              str0, saw_str0, str1, saw_str1);
        /* (5) Versatz + Streuung. */
        CHECK(gleich_anker == 0,
              "(5) %d/64 Stoesse sitzen ohne Versatz auf dem Anker — das Original setzt drei "
              "RNG-Offsets (@0x80108004/18/2C)", gleich_anker);
        CHECK(aus_band == 0,
              "(5) %d/64 Stoesse ausserhalb X/Z [-254,256], Y [-708,312]", aus_band);
        CHECK(wechsel >= 32, "(5) nur %d/63 Wechsel — der Stoss streut nicht", wechsel);
        /* (6) Skalierungsband. */
        CHECK(sc_aus == 0 && sc_min >= 7000 && sc_max <= 9040,
              "(6) scale %d..%d, %d Treffer ausserhalb 7000..9040 (@0x80108050 + rand<<3)",
              sc_min, sc_max, sc_aus);
        CHECK(sc_max > sc_min, "(6) scale ist konstant (%d) statt 7000+(rand<<3)", sc_min);
    }

    /* (7) MESSUNG: Pool-Belegung unter dem DAUERFEUER-TAKT DES PORTS
     *     ((anim_frame & 4) == 0 = 5 Schuss je 9 Bilder, player_common.c:423, @0x800349BC;
     *     RE2 verbraucht dagegen 1 Patrone je 8 Aufrufe, @0x8006A144-58). Der Pool haelt
     *     96 Slots (`addiu t2,zero,96` @0x8001BF10 = RE15_ESP_FX_MAX); laeuft er voll,
     *     fallen Muendungsfeuer/Huelsen still aus. */
    {
        re15_esp_fx_reset();
        re15_re2z_rng_reset();
        re15_actor_t *e  = &g_actors[1];
        re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
        int maxbel = 0, n_treffer = 0, voll = 0;
        for (int f = 0; f < 30; f++) {
            if ((f & 4) == 0) {
                e->state = 2; e->sub_state_1 = 15; e->sub_state_2 = 0;
                e->re2z_hits1d2 = 1; e->re2z_res223 = 100;
                re15_re2z_hurt_test(e, pl);
                n_treffer++;
            }
            re15_esp_fx_tick(&esp);
            int n = re15_esp_fx_count();
            if (n > maxbel) maxbel = n;
            if (n >= RE15_ESP_FX_MAX) voll++;
        }
        printf("  Dauerfeuer 30 Bilder (%d Treffer, Port-Takt): Spitze %d/%d Slots, "
               "%d Bilder am Anschlag\n", n_treffer, maxbel, RE15_ESP_FX_MAX, voll);
        CHECK(voll == 0,
              "(7) der FX-Pool stand %d von 30 Bildern am Anschlag (%d/%d) — dann fallen "
              "Muendungsfeuer und Huelsen still aus", voll, maxbel, RE15_ESP_FX_MAX);
    }

    printf(g_fail ? "=== RIEGEL r26: ROT ===\n" : "=== RIEGEL r26: GRUEN ===\n");
    return g_fail;
}
