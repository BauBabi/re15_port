/* probe_re2_crow_10c0.c - DIAGNOSE (kein ctest): Nutzer-Report 2026-09-07, RE2-Modus:
 *   (1) "Sie bleiben an Raum Blockern haengen und finden dann den Raum nicht mehr."
 *   (2) "Haben sie Leon einmal gehackt, fliegen sie nur noch an ihn ran, haben keine
 *        Animation mehr und greifen nicht mehr an. erschiessen kann man sie dann auch nicht."
 *
 * Beide bestehenden Tests (test_re2_crow_ai.c, test_re2_room10c0_ab.c) laufen OHNE
 * g_room_rdt_ok - Wand, LOS und Kontakt am ECHTEN Raum sind darin ungetestet, und genau
 * dort verortet diag_crow_stuck.md die Ursachen. Diese Sonde laedt ROOM10C0.RDT wirklich.
 *
 * DIE BANK MUSS GELADEN SEIN. Ohne sie meldet re15_re2z_victim_begin "keine Victim-Bank"
 * (enemy_ai_common.c:2372) und der Griff rastet NIE ein - der erste Lauf dieser Sonde
 * zeigte genau das und sah aus wie der gesuchte Defekt. Gemessen (probe_re2_crow_bank):
 * Typ 0x21 aus CDEMD0.EMS hat victim_ok=1. Also Artefakt der Sonde, kein Befund.
 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "re15_rdt.h"
#include "re15_scd.h"
#include "re15_actor.h"
#include "re15_room.h"
#include "re15_enemy_ai.h"
#include "re15_enemy.h"
#include "re15_player.h"
#include "re15_collision.h"
#include "re15_damage.h"
#include "re15_ai_flavor.h"
#include "re2_ems.h"

#define RE15_STR(x)  #x
#define RE15_XSTR(x) RE15_STR(x)
#define FENSTER 300
#define TICKS   3600

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

static void bank_laden(void)
{
    size_t esz = 0;
    uint8_t *ems = read_file(RE15_XSTR(RE15_ASSETS_PATH) "/../RE2/CDEMD0.EMS", &esz);
    if (!ems) { printf("  [Bank] CDEMD0.EMS fehlt - der Griff kann nicht einrasten!\n"); return; }
    /* re15_enemy_alloc liefert ab dem zweiten Lauf NULL (die Registry ueberlebt
     * re15_actor_init) - dann stand victim_ok auf -1 und die Laeufe 2..n massen
     * wieder den bankfreien Artefakt-Zustand. Vorhandene Bank wiederverwenden. */
    re15_enemy_bank_t *eb = re15_enemy_find(0x21);
    if (!eb) eb = re15_enemy_alloc(0x21);
    re15_tim_t tim = {0};
    if (eb && !eb->victim_ok && re2_ems_load_bank(ems, esz, 0x21, eb, &tim) == 0) eb->ok = 1;
    printf("  [Bank] Typ 0x21: victim_ok=%d\n", eb ? eb->victim_ok : -1);
}

static void lauf(const re15_rdt_t *rdt, int32_t px, int32_t pz, const char *wo, int gehackt)
{
    re15_actor_init();
    scd_vm_init();
    g_current_room_id = 0x10c0;
    re15_ai_flavor_set(RE15_AI_FLAVOR_RE2);
    bank_laden();

    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->x = px; pl->y = 0; pl->z = pz;
    pl->hp = 100; pl->rot_y = 0; pl->hit_react = 0;
    memcpy((void *)&g_room_rdt, rdt, sizeof(g_room_rdt));
    g_room_rdt_ok = 1;
    re15_collision_set_band(0);
    scd_register_room_events(rdt);
    scd_room_reenter((re15_rdt_t *)rdt, 0, 0, 0);

    int32_t lx[RE15_ACTOR_MAX], lz[RE15_ACTOR_MAX], ly[RE15_ACTOR_MAX];
    long weg[RE15_ACTOR_MAX], weg_v[RE15_ACTOR_MAX], stall[RE15_ACTOR_MAX], stallmax[RE15_ACTOR_MAX];
    long frames[RE15_ACTOR_MAX], claim[RE15_ACTOR_MAX], grab[RE15_ACTOR_MAX];
    unsigned long subs[RE15_ACTOR_MAX], subs_ng[RE15_ACTOR_MAX];
    uint8_t lf[RE15_ACTOR_MAX]; int hatte[RE15_ACTOR_MAX];
    long grab_ohne_opfer = 0;
    memset(weg,0,sizeof weg); memset(weg_v,0,sizeof weg_v); memset(stall,0,sizeof stall);
    memset(stallmax,0,sizeof stallmax); memset(frames,0,sizeof frames);
    memset(claim,0,sizeof claim); memset(grab,0,sizeof grab);
    memset(subs,0,sizeof subs); memset(subs_ng,0,sizeof subs_ng);
    memset(lf,0,sizeof lf); memset(hatte,0,sizeof hatte);
    for (int i = 0; i < RE15_ACTOR_MAX; i++) { lx[i]=g_actors[i].x; lz[i]=g_actors[i].z; ly[i]=g_actors[i].y; }

    printf("=== ROOM10C0, RE2-KI, Spieler %s ===\n", wo);
    for (int f = 0; f < TICKS; f++) {
        if (!gehackt) pl->hit_react = 0;
        scd_vm_tick(); re15_enemy_ai_run_all(1); re15_actors_anim_advance();
        for (int i = 1; i < RE15_ACTOR_MAX; i++) {
            re15_actor_t *e = &g_actors[i];
            if (!e->active || e->type != 0x21) continue;
            int32_t dx = e->x - lx[i], dz = e->z - lz[i], dy = e->y - ly[i];
            weg[i] += (dx<0?-dx:dx) + (dz<0?-dz:dz) + (dy<0?-dy:dy);
            lx[i]=e->x; lz[i]=e->z; ly[i]=e->y;
            if (e->state == 1 && e->sub_state_1 < 32) subs[i] |= (1UL << e->sub_state_1);
            if (e->re2c_flags22a & 0x4u) claim[i]++;
            if (e->state == 1 && e->sub_state_1 == 14) {
                grab[i]++; hatte[i] = 1;
                if (re15_player_victim_state() == 0) grab_ohne_opfer++;
            }
            if (e->anim_frame != lf[i]) { frames[i]++; lf[i] = e->anim_frame; }
            if (hatte[i] && e->state == 1 && e->sub_state_1 < 32)
                subs_ng[i] |= (1UL << e->sub_state_1);
        }
        if ((f % FENSTER) == FENSTER-1) {
            /* NUR WACHE Kraehen: die schlafenden (Sub 0, dist>1800 @0x80100704) stehen
             * naturgemaess still - sie mitzuzaehlen machte die Zahl wertlos. */
            for (int i = 1; i < RE15_ACTOR_MAX; i++) {
                re15_actor_t *e = &g_actors[i];
                if (!e->active || e->type != 0x21 || subs[i] == 1UL) continue;
                if (weg[i] - weg_v[i] < 300) {
                    stall[i]++;
                    if (stall[i] > stallmax[i]) stallmax[i] = stall[i];
                } else stall[i] = 0;
                weg_v[i] = weg[i];
            }
        }
    }

    for (int i = 1; i < RE15_ACTOR_MAX; i++) {
        re15_actor_t *e = &g_actors[i];
        if (!e->active || e->type != 0x21 || subs[i] == 1UL) continue;   /* nur wache */
        printf("  slot %2d: Weg=%7ld Sub=0x%05lx Claim=%4ld Grab=%4ld Frames=%5ld "
               "laengste Stille=%ld Fenster  nach dem Griff Sub=0x%05lx  jetzt ss=%d\n",
               i, weg[i], subs[i], claim[i], grab[i], frames[i], stallmax[i],
               subs_ng[i], e->sub_state_1);
    }
    printf("  Grab-Ticks OHNE Opfer-FSM=%ld   Opfer-FSM am Ende: state=%d typ=0x%02x\n",
           grab_ohne_opfer, re15_player_victim_state(), (unsigned)re15_player_victim_type());

    /* Nutzer: "erschiessen kann man sie dann auch nicht" - GEZIELT im Griff testen. */
    int tgt = -1;
    for (int i = 1; i < RE15_ACTOR_MAX; i++)
        if (g_actors[i].active && g_actors[i].type == 0x21 && subs[i] != 1UL) { tgt = i; break; }
    if (tgt > 0) {
        re15_actor_t *e = &g_actors[tgt];
        int wart = 0;
        for (; wart < 900; wart++) {
            if (e->state == 1 && (e->sub_state_1 == 13 || e->sub_state_1 == 14)) break;
            if (!gehackt) pl->hit_react = 0;
            scd_vm_tick(); re15_enemy_ai_run_all(1); re15_actors_anim_advance();
        }
        pl->rot_y = (int16_t)((re15_atan2_q12(e->z - pl->z, e->x - pl->x) - 0x400) & 0xfff);
        int ss0 = e->sub_state_1, hp0 = e->hp;
        int hit = re15_player_weapon_fire(3);
        printf("  Schuss slot %d nach %d Ticks Warten (ss=%d hp=%d, dy=%ld): fire=%d -> hp=%d st=%d\n",
               tgt, wart, ss0, hp0, (long)(pl->y - e->y), hit, (int)e->hp, e->state);
    }
    g_room_rdt_ok = 0; re15_collision_reset_band();
    printf("\n");
}

int main(void)
{
    const char *base = RE15_XSTR(RE15_ASSETS_PATH);
    char path[600];
    snprintf(path, sizeof path, "%s/STAGE1/ROOM10C0.RDT", base);
    size_t size = 0;
    uint8_t *data = read_file(path, &size);
    if (!data) { fprintf(stderr, "FAIL: %s nicht lesbar\n", path); return 1; }
    re15_rdt_t rdt;
    if (re15_rdt_parse(data, size, &rdt) != 0) { fprintf(stderr, "FAIL: parse\n"); return 1; }
    /* Der Weck-Test ist dist<=1800 (@0x80100704) - der Spieler MUSS neben einer Kraehe
     * stehen, sonst sitzen alle drei 3600 Ticks in Sub 0. */
    lauf(&rdt, 1300, 300,   "neben Kraehe 1 (2000,300)", 0);
    lauf(&rdt, 3400, -3900, "neben Kraehe 2 (4100,-3900)", 0);
    lauf(&rdt, 4400, 3600,  "neben Kraehe 3 (5100,3600)", 0);
    /* Nutzer: "Haben sie Leon einmal GEHACKT..." - die Laeufe oben loeschen
     * hit_react jeden Tick (Original-cmd-0-Prolog @0x80031964) und verhindern
     * damit genau den Zustand, von dem er spricht. Diese drei lassen ihn stehen. */
    lauf(&rdt, 1300, 300,   "neben Kraehe 1 (2000,300)", 1);
    lauf(&rdt, 3400, -3900, "neben Kraehe 2 (4100,-3900)", 1);
    lauf(&rdt, 4400, 3600,  "neben Kraehe 3 (5100,3600)", 1);
    free(data);
    return 0;
}
