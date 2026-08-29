/* test_writher_abtauchen.c — PIN: die Gitter-Arme lassen sich abschiessen und kommen
 * danach nicht wieder.
 *
 * NUTZER-AUFTRAG 2026-08-26: "Ich wuerde die Haende auch gerne anschiessen koennen, das die
 * danach nicht mehr rauskommen nach 2 Schuessen oder so."
 *
 * BYTE-BELEGT (STAGE1.BIN, alles selbst disassembliert):
 *   Trefferbudget +0x1D2 im INIT:
 *       8010c3f8  jal   0x8001af20      ; rng
 *       8010c400  andi  v0,v0,0x3
 *       8010c404  addiu v0,v0,1         ; -> 1..4
 *       8010c41c  sb    v0,466(v1)
 *   Kosten je Waffe, Zeigertabelle @0x80120c40 (Aufruf `sll v0,v0,2` @0x8010d2a4 /
 *   `addiu at,at,3136` @0x8010d2ac / `jalr` @0x8010d2bc), drei Blaetter:
 *       0x8010d0b0  jr ra                 ->  0
 *       0x8010d0b8  addiu v0,v0,-1        -> -1   (@0x8010d0cc)
 *       0x8010d0d8  addiu v0,v0,-2        -> -2   (@0x8010d0ec)
 *   Auswertung im Zuck-Blatt:
 *       8010d144  lb    v0,466(v1)        ; SIGNED
 *       8010d14c  bgez  v0,0x8010d178     ; >= 0 -> normal weiter
 *       8010d154  sb    1,4(v1)           ; state = 1
 *       8010d164  sb    4,5(v1)           ; ABTAUCHEN
 *   HP-Zeile des Typs: Basis 0x8011f034, Zeile Typ*0x20 = @0x8011f374, Spalte rng&0xf
 *   (@0x8010daf4/@0x8010dafc), Store @0x8010db14 =
 *   {72,82,96,82,83,96,74,84,99,76,88,86,87,82,80,90}.
 *
 * ⛔ NACHRUESTUNG ist zweierlei, beides benannt: (a) DASS der Arm-INIT die HP-Routine
 * ueberhaupt ruft (im Original kein Store auf +0x9a im ganzen Arm-Baum), und (b) dass das
 * Wiederauftauchen WEGGELASSEN wird — das Original wartet (rng&0xff)+30 Bilder
 * (@0x8010d044-54) und geht dann zurueck auf Sub 2 (@0x8010d088-8c). Genau dieser Uebergang
 * wird nicht gebaut; erfunden wird nichts.
 *
 * DIE WACHE IST NICHT VAKUANT: sie schiesst mit der PISTOLE (Kosten -1), zaehlt die
 * Schuesse bis zum Abtauchen, verlangt den Bereich 2..5 (Budget 1..4 plus der Treffer, der
 * unter null drueckt) — und prueft danach, dass der Arm auch bei einem erneuten
 * Naeherungs-Tor NICHT mehr ausfaehrt. Dazu die Gegenprobe, dass ein UNBESCHOSSENER Arm
 * sehr wohl ausfaehrt; ohne die waere die Zusage wertlos.
 */
#include "re15_rdt.h"
#include "re15_scd.h"
#include "re15_actor.h"
#include "re15_aot.h"
#include "re15_room.h"
#include "re15_enemy.h"
#include "re15_enemy_ai.h"
#include "re15_ems.h"
#include "re15_emd.h"
#include "re15_md1.h"
#include "re15_collision.h"
#include "re15_msg.h"
#include "re15_game_step.h"
#include "re15_camera.h"
#include "re15_damage.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef RE15_ASSET_PSX_DIR
#define RE15_ASSET_PSX_DIR "shared_assets/PSX"
#endif

static int fails = 0;
#define CHECK(c, ...) do { if (!(c)) { printf("FAIL: " __VA_ARGS__); printf("\n"); fails++; } \
                           else { printf("  PASS: " __VA_ARGS__); printf("\n"); } } while (0)

static uint8_t s_blob[0x80000];

static uint8_t *slurp(const char *p, size_t *n)
{
    FILE *f = fopen(p, "rb"); if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    if (sz <= 0) { fclose(f); return NULL; }
    uint8_t *b = (uint8_t *)malloc((size_t)sz);
    if (b && fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); b = NULL; }
    fclose(f); if (b) *n = (size_t)sz; return b;
}

static int load_bank(uint8_t type)
{
    size_t n = 0;
    uint8_t *ems = slurp(RE15_ASSET_PSX_DIR "/EMD/CDEMD0.EMS", &n);
    if (!ems) return 0;
    int idx = re15_ems_index_for_type(type);
    size_t off = 0, len = 0;
    int ok = 0;
    if (idx >= 0 && re15_ems_get_entry(ems, n, idx, &off, &len) == 0 && len <= sizeof s_blob) {
        memcpy(s_blob, ems + off, len);
        re15_enemy_bank_t *eb = re15_enemy_find(type);
        if (!eb) eb = re15_enemy_alloc(type);
        if (eb) {
            re15_tim_t tim; memset(&tim, 0, sizeof tim);
            if (re15_emd_parse_container(s_blob, len, &eb->md1, &eb->skel, &eb->anim, &tim) == 0) {
                eb->ok = 1; eb->buf = NULL;
                eb->loco_ok = (re15_emd_parse_loco_bank(s_blob, len,
                                                        &eb->skel_loco, &eb->anim_loco) == 0);
                ok = 1;
            }
        }
    }
    free(ems);
    return ok;
}

/* Einen Arm frisch aufsetzen und seinen INIT laufen lassen. */
static re15_actor_t *arm_spawn(int slot, int32_t x, int32_t z)
{
    re15_actor_t *e = &g_actors[slot];
    memset(e, 0, sizeof *e);
    e->active = 1; e->type = 0x1a; e->state = 0;
    e->x = x; e->y = 0; e->z = z; e->rot_y = 0;
    re15_enemy_apply_hitbox(e, 0x1a);
    re15_enemy_ai_run_all(0);            /* INIT */
    return e;
}

int main(void)
{
    printf("=== Gitter-Arme: anschiessen und abtauchen ===\n");

    re15_actor_init(); re15_aot_init(); scd_vm_init();
    re15_enemy_reset(); re15_enemy_ai_set_paused(0);
    g_current_room_id = 0x1210;
    re15_damage_seed_rng(1234u);

    CHECK(load_bank(0x1a), "EM01A-Bank geladen — ohne sie ist clip_len 0 und die Wache blind");

    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100;
    /* ⛔ NAH GENUG UND MIT BLICK AUF DEN ARM. Der erste Anlauf setzte den Spieler auf
     * (4091,900) — Abstand rund 4190, und dort traf KEIN Schuss (20 Versuche, Budget
     * unveraendert). Treffer-Tests fangen beim ZIELEN an, nicht beim Schaden: der
     * Waffenpfad braucht ein Ziel in Reichweite und im Kegel. */
    pl->x = 1200; pl->y = 0; pl->z = 0;
    pl->rot_y = 2048;                          /* Blick nach -x, also auf den Arm bei (0,0) */

    /* --- (1) UNBESCHOSSEN: der Arm faehrt aus. Ohne diese Gegenprobe waere (3) wertlos. --- */
    {
        /* Fuer die Ausfahr-Gegenprobe muss der Spieler am HANDPUNKT stehen (Heimat + 4091
         * entlang +0x6a), sonst geht das Naeherungs-Tor gar nicht auf. Zum Schiessen steht
         * er danach wieder nah genug fuer die Zielerfassung. */
        int32_t sx = pl->x, sz = pl->z; int16_t sr = pl->rot_y;
        pl->x = 4091; pl->z = 900;
        re15_actor_t *e = arm_spawn(1, 0, 0);
        int came_out = 0;
        for (int f = 0; f < 200; f++) {
            re15_enemy_ai_run_all(0);
            if (e->x != 0) { came_out = 1; break; }
        }
        printf("  (1) unbeschossen: Arm auf x=%ld nach dem Lauf\n", (long)e->x);
        CHECK(came_out,
              "ein UNBESCHOSSENER Arm faehrt aus (x = %ld) — sonst koennte (3) nicht zeigen, "
              "dass Beschuss ihn davon abhaelt", (long)e->x);
        e->active = 0;
        pl->x = sx; pl->z = sz; pl->rot_y = sr;
    }

    /* --- (2) BESCHUSS mit der Pistole: wie viele Treffer bis zum Abtauchen? ------------- */
    int schuesse = 0, budget0 = 0;
    re15_actor_t *e = arm_spawn(1, 0, 0);
    budget0 = e->writher_hits;
    printf("  (2) Startbudget %d, HP %d\n", budget0, e->hp);
    CHECK(budget0 >= 1 && budget0 <= 4,
          "das Startbudget liegt in 1..4 (%d) — (rng & 3) + 1 @0x8010c400/@0x8010c404",
          budget0);

    for (int f = 0; f < 400 && e->sub_state_1 != 7; f++) {
        if (e->state == 1 && (f % 20) == 0) {
            /* FIXTURE-KORREKTUR 2026-08-29: nur REGISTRIERTE Treffer zaehlen (Rueckgabe
             * slot+1), nicht Versuche — seit der Flinch byte-true die volle Clip-2-Laenge
             * spielt (+0x95=0 @0x8010d210) und +0x9c=0 (@0x8010d26c) die Lunge sofort
             * re-armt, kann ein Versuch den gerade AUSGEFAHRENEN Arm (2420 an der
             * Spieler-Position vorbei) verfehlen. Die Wache behauptet "Treffer" — jetzt
             * misst sie sie. */
            if (re15_player_weapon_fire(3) != 0)   /* Waffen-Id 3 = Pistole, Kosten -1 */
                schuesse++;
        }
        re15_enemy_ai_run_all(0);
    }
    printf("  (2) nach %d Pistolentreffern: Sub %u, HP %d, Budget %d\n",
           schuesse, e->sub_state_1, e->hp, (int8_t)e->writher_hits);
    CHECK(e->sub_state_1 == 7,
          "der Arm ist nach %d Pistolentreffern VERGRABEN (Sub %u) — das Budget lief unter "
          "null (`lb +0x1D2` @0x8010d144 / `bgez` @0x8010d14c)", schuesse, e->sub_state_1);
    CHECK(schuesse >= 2 && schuesse <= 5,
          "und es waren %d Treffer, also im Bereich 2..5 — Budget 1..4 plus der Treffer, der "
          "unter null drueckt, bei Pistolenkosten -1 (Tabelle @0x80120c40[3] -> 0x8010d0b8)",
          schuesse);
    CHECK(e->hp > 0,
          "die HP sind dabei NICHT aufgebraucht (%d von %d) — der Zaehler gewinnt vor dem "
          "Schaden, genau wie im Original", e->hp, budget0 ? e->hp : 0);

    /* --- (3) UND ER KOMMT NICHT WIEDER --------------------------------------------------- */
    {
        int32_t x0 = e->x;
        int came_back = 0;
        for (int f = 0; f < 600; f++) {
            re15_enemy_ai_run_all(0);
            if (e->sub_state_1 != 7 || e->x != x0) { came_back = 1; break; }
        }
        printf("  (3) 600 weitere Bilder: Sub %u, x=%ld\n", e->sub_state_1, (long)e->x);
        CHECK(!came_back,
              "er bleibt vergraben (Sub %u) und faehrt NICHT wieder aus — das Original "
              "taucht nach (rng&0xff)+30 Bildern wieder auf (@0x8010d044-54 -> @0x8010d088), "
              "genau dieser Uebergang ist bewusst weggelassen", e->sub_state_1);
    }

    /* --- (4) und er ist kein Ziel mehr --------------------------------------------------- */
    printf("  (4) hit_react = 0x%02x\n", e->hit_react);
    CHECK((e->hit_react & 1u) != 0,
          "der vergrabene Arm traegt das Treffer-Bit dauerhaft (0x%02x) — damit faellt er "
          "aus der Zielauswahl des Aufloesers", e->hit_react);

    if (fails) { printf("\nABTAUCHEN: FAIL (%d)\n", fails); return 1; }
    printf("\nABTAUCHEN: die Arme lassen sich wegschiessen und bleiben weg\n");
    return 0;
}
