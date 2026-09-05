/* test_maggot_zone_leap.c — PIN (S5-Fix 2026-09-05): der Gorilla-ZONEN-LEAP (A[4] Path A).
 *
 * Nutzer-Report: "Die Gorillas beim Parking lot koennen kaum angreifen, da sie immer
 * zwischen den Autos haengen bleiben." (ROOM11C0, Typ 0x27, RE1.5-Brain in beiden AI-Modi.)
 *
 * ORIGINAL (STAGE1.BIN + RE1.5-PSX.EXE, diag_gorilla_stuck.md, adversarial bestaetigt in
 * verify_gorilla.md): A[4]-Decide @0x80117ec8-0x8011802c fragt per FUN_8003b93c
 * (Quadranten-Suche, Match u1==attr && (floor>>4)==+0x82, Radius box[6]+100, schreibt
 * entity+0x90=(dir>>4)+8+(u1&3) @0x8003ba8c, return 1 beim ersten Treffer) die
 * SCA-MARKER-Pads ab (ROOM11C0: idx36/47 u1=0x10, idx4/31/19/55 u1=0x20, alle u0=0) und
 * committet bei Blick ±45 Grad auf die Pad-Richtung +0x5=7 mit +0x7=1 (0x10) bzw. 3
 * (0x20 blind, nur ohne LOS-Latch). B[7] snapt den Yaw und setzt +0x82=1 (@0x80118af0),
 * die Landung 0 (@0x80118ca4) — der Flug klemmt an Band 1 statt an den Band-0-Autozellen.
 *
 * PIN 1: re15_collision_zone_query findet die 11C0-Pads (Band/Attr/Broadphase/Richtung).
 * PIN 2: A[4] committet den Zonen-Leap (sub 7, +0x7=1) bei Frontalstellung am Pad —
 *        und NICHT bei abgewandtem Blick (Yaw-Fenster) oder falschem Band.
 * PIN 3: der Zonen-Launch traegt floor=1 (Band-Wechsel), die Landung floor=0. */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "re15_rdt.h"
#include "re15_actor.h"
#include "re15_enemy.h"
#include "re15_enemy_ai.h"
#include "re15_collision.h"
#include "re15_room.h"
#include "re15_damage.h"

#define RE15_STR(x)  #x
#define RE15_XSTR(x) RE15_STR(x)

static int fails = 0;
#define CHECK(c, ...) do { if (!(c)) { printf("FAIL: " __VA_ARGS__); printf("\n"); fails++; } } while (0)

static uint8_t *read_file(const char *path, size_t *out_size)
{
    FILE *f = fopen(path, "rb");
    if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    if (sz <= 0) { fclose(f); return NULL; }
    uint8_t *buf = (uint8_t *)malloc((size_t)sz);
    if (!buf) { fclose(f); return NULL; }
    size_t rd = fread(buf, 1, (size_t)sz, f);
    fclose(f);
    if (rd != (size_t)sz) { free(buf); return NULL; }
    *out_size = (size_t)sz;
    return buf;
}

int main(void)
{
    const char *base = RE15_XSTR(RE15_ASSETS_PATH);
    char path[600]; size_t sz = 0;
    snprintf(path, sizeof path, "%s/STAGE1/ROOM11C0.RDT", base);
    uint8_t *dat = read_file(path, &sz);
    if (!dat) { fprintf(stderr, "FAIL: %s nicht lesbar\n", path); return 1; }
    static re15_rdt_t rdt;
    if (re15_rdt_parse(dat, sz, &rdt) != 0) { fprintf(stderr, "FAIL: parse\n"); return 1; }

    /* ===== PIN 1: Zonen-Abfrage auf den echten 11C0-Pads =============================== */
    uint8_t c = 0;
    /* Pad idx36 (u1=0x10): x[4900..9500] z[-6080..-2180] — der Ost-Spalt. Punkt westlich
     * davor (x=4000 innerhalb Broadphase r=1700), Band 0. Richtung: westlich der Zelle
     * -> dir 0x000 -> contact (0>>4)+8+(0x10&3)=8. */
    CHECK(re15_collision_zone_query(&rdt, 4000, -4000, 1700, 0, 0x10u, &c) == 1,
          "Pad idx36 (0x10) muss am Ost-Spalt gefunden werden");
    CHECK(c == 8, "Richtungs-Code westlich des Pads: (0x000>>4)+8+0 = 8, ist %d", (int)c);
    /* dieselbe Stelle, falsches Band -> MISS (@0x8003ba08 bne a2) */
    CHECK(re15_collision_zone_query(&rdt, 4000, -4000, 1700, 1, 0x10u, &c) == 0,
          "Band 1 darf das Band-0-Pad nicht finden");
    /* dieselbe Stelle, Attr 0x20 -> MISS (u1 EXAKT, @0x8003ba0c-14) */
    CHECK(re15_collision_zone_query(&rdt, 4000, -4000, 1700, 0, 0x20u, &c) == 0,
          "Attr 0x20 darf das 0x10-Pad nicht matchen");
    /* Ost-Tasche: Pad idx4/31 (u1=0x20) x[7500..10700] z[-200..8201]; Punkt x=6418
     * (gemessene Klemm-Linie der Sonde) liegt in der Broadphase. Richtung West -> 8+0. */
    CHECK(re15_collision_zone_query(&rdt, 6418, 5900, 1700, 0, 0x20u, &c) == 1,
          "Pad idx4/31 (0x20) muss in der Ost-Tasche gefunden werden");
    /* weit weg -> MISS */
    CHECK(re15_collision_zone_query(&rdt, -25000, -20000, 1700, 0, 0x10u, &c) == 0,
          "fern der Pads darf nichts matchen");

    /* ===== PIN 2+3: A[4]-Commit + Band-Wechsel ueber das echte Brain =================== */
    g_room_rdt = rdt; g_room_rdt_ok = 1;
    re15_actor_init();
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->hp = 100; pl->x = -14000; pl->z = 6000; pl->y = 0;
    re15_actor_t *m = &g_actors[2];
    memset(m, 0, sizeof *m);
    m->active = 1; m->type = 0x27; m->hp = 300;
    m->state = 1; m->sub_state_1 = 4; m->sub_state_2 = 1;      /* SELECTOR (A[4]) */
    m->mag_boost = 4;                                          /* +0x1e2=4 @0x80117098-9c */
    m->hit_radius_min = 1600;                                  /* box[6] @0x80121350 */
    m->floor = 0;                                              /* +0x82 Spawn pc[4]=0 */
    m->dog_flags = 0;                                          /* LOS-Latch frei */
    m->x = 4000; m->z = -4000; m->rot_y = 0;                   /* frontal auf Pad 36 (Ost) */
    {   re15_enemy_bank_t *eb = re15_enemy_find(0x27);
        if (!eb) eb = re15_enemy_alloc(0x27);
        if (eb) eb->ok = 1;                                    /* Brain-Gate (test_maggot_ai-Muster) */
    }

    re15_enemy_ai_run_all(1);
    CHECK(m->sub_state_1 == 7 && m->sub_state_3 == 1,
          "PIN 2: frontal am 0x10-Pad -> Zonen-Leap-Commit (+0x5=7/+0x7=1 @0x80117f50-74), "
          "ist s1=%d s3=%d", m->sub_state_1, m->sub_state_3);
    CHECK((int8_t)m->dog_aux9f == 8, "PIN 2: +0x9f = +0x90-Latch (@0x80117f40), ist %d",
          (int)m->dog_aux9f);

    /* Launch-Frames ticken bis floor=1 (B[7] Frame 10 @0x80118ab4-af0) */
    int saw_floor1 = 0, saw_land0 = 0, leaps = 0;
    for (int f = 0; f < 400; f++) {
        pl->hp = 100; pl->hit_react = 0;
        re15_enemy_ai_run_all(1);
        if (m->sub_state_1 == 7 && m->floor == 1) saw_floor1 = 1;
        if (saw_floor1 && m->floor == 0 && m->sub_state_1 != 7) { saw_land0 = 1; break; }
    }
    CHECK(saw_floor1, "PIN 3: Zonen-Launch setzt floor=1 (+0x82=1 @0x80118af0)");
    CHECK(saw_land0,  "PIN 3: Landung setzt floor=0 zurueck (@0x80118ca4)");
    (void)leaps;

    /* Negativ: abgewandter Blick (yaw 0x800 = West, Pad im Osten) -> KEIN Commit */
    m->state = 1; m->sub_state_1 = 4; m->sub_state_2 = 1; m->sub_state_3 = 0;
    m->x = 4000; m->z = -4000; m->rot_y = 0x800; m->floor = 0; m->mag_airborne = 0;
    m->dog_flags = 0; m->dog_blocked_ctr = 1;   /* HEAVY-Gate zu, damit nur Path A/B entscheiden */
    re15_enemy_ai_run_all(1);
    CHECK(!(m->sub_state_1 == 7 && m->sub_state_3 != 0),
          "Negativ: Yaw-Fenster (slti 1024 @0x80117f34) muss den abgewandten Leap sperren, "
          "ist s1=%d s3=%d", m->sub_state_1, m->sub_state_3);

    printf(fails ? "test_maggot_zone_leap: %d FAIL(s)\n" : "test_maggot_zone_leap: OK\n", fails);
    free(dat);
    return fails ? 1 : 0;
}
