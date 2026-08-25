/* ============================================================================
 *  NPC (Chief Irons, type 0x40, EM040) AI — Wave 1 port probe.
 *
 *  Byte-true 0x8011c5a0 family (RE15_NPC_AI.md). The STAGE1 NPCs are invulnerable
 *  cutscene actors (HP = -1) that idle-pose, walk, look-at, and dialogue. Seeds a
 *  type-0x40 actor and drives re15_enemy_ai_run_all, asserting:
 *   (1) INIT (0x8011c6dc): state 0 -> 1 (ACTIVE), HP = -1 (invulnerable), idle clip 2.
 *   (2) IDLE: the NPC holds its position and animates the idle pose (clip 2 loops).
 * ==========================================================================*/
#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include <stdlib.h>

#include "re15_actor.h"
#include "re15_enemy_ai.h"
#include "re15_enemy.h"
#include "re15_ems.h"
#include "re15_emd.h"
#include "re15_md1.h"

#ifndef RE15_ASSET_PSX_DIR
#define RE15_ASSET_PSX_DIR "shared_assets/PSX"
#endif

static uint8_t s_npc_blob[0x80000];

/* EM040 aus CDEMD0.EMS in die Registry legen. OHNE Bank ist re15_actor_clip_len 0 und
 * jede Aussage ueber Clip-Laengen/Bank waere vakuant. BANK 1 (dir[4]/dir[3], geladen von
 * FUN_80022300 @0x800224b8/@0x800224c8) ist die Bank, aus der die Eskorte posiert. */
static int npc_load_bank(uint8_t type)
{
    FILE *f = fopen(RE15_ASSET_PSX_DIR "/EMD/CDEMD0.EMS", "rb");
    if (!f) return 0;
    fseek(f, 0, SEEK_END); long fsz = ftell(f); fseek(f, 0, SEEK_SET);
    uint8_t *ems = (uint8_t *)malloc((size_t)fsz);
    if (!ems || fread(ems, 1, (size_t)fsz, f) != (size_t)fsz) { free(ems); fclose(f); return 0; }
    fclose(f);
    int idx = re15_ems_index_for_type(type);
    size_t off = 0, len = 0; int ok = 0;
    if (idx >= 0 && re15_ems_get_entry(ems, (size_t)fsz, idx, &off, &len) == 0
        && len <= sizeof s_npc_blob) {
        memcpy(s_npc_blob, ems + off, len);
        re15_enemy_bank_t *eb = re15_enemy_find(type);
        if (!eb) eb = re15_enemy_alloc(type);
        if (eb) {
            re15_tim_t tim = (re15_tim_t){0};
            if (re15_emd_parse_container(s_npc_blob, len, &eb->md1, &eb->skel, &eb->anim, &tim) == 0) {
                eb->ok = 1; eb->buf = NULL;
                eb->loco_ok = (re15_emd_parse_loco_bank(s_npc_blob, len,
                                                        &eb->skel_loco, &eb->anim_loco) == 0);
                eb->own_ok  = (re15_emd_parse_own_bank(s_npc_blob, len,
                                                       &eb->skel_own, &eb->anim_own) == 0);
                ok = 1;
            }
        }
    }
    free(ems);
    return ok;
}

int main(void)
{
    int fail = 0;
    memset(g_actors, 0, sizeof g_actors);

    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->x = 0; pl->y = 0; pl->z = 3000; pl->hp = 100;

    const int NS = 1;
    re15_actor_t *n = &g_actors[NS];
    n->active = 1; n->type = 0x40; n->state = 0; n->x = 1000; n->y = 0; n->z = 1000;

    printf("=== NPC (Chief Irons, type 0x40) Wave-1 AI ===\n");
    re15_enemy_reset();
    int bank_ok = npc_load_bank(0x40);
    re15_enemy_bank_t *nb = re15_enemy_find(0x40);
    printf("  EM040-Bank: %s (own_ok=%d, Bank-1-Clips=%d)\n", bank_ok ? "ok" : "FEHLT",
           nb ? nb->own_ok : -1, (nb && nb->own_ok) ? nb->anim_own.clip_count : -1);
    if (!bank_ok || !nb || !nb->own_ok) {
        fprintf(stderr, "FAIL(0): ohne EM040-BANK 1 misst (3) nichts\n");
        fail = 1;
    }

    /* (1) INIT */
    re15_enemy_ai_run_all(0);
    if (n->state != 1) { fprintf(stderr, "FAIL(1): INIT->ACTIVE expected state 1, got %d\n", n->state); fail = 1; }
    if (n->hp != -1)   { fprintf(stderr, "FAIL(1): NPC must be invulnerable (HP=-1), got %d\n", n->hp); fail = 1; }
    if (n->motion != 2){ fprintf(stderr, "FAIL(1): idle clip 2 expected, got %d\n", n->motion); fail = 1; }
    printf("  (1) INIT: state->1, hp=%d (invulnerable), idle clip=%d\n", n->hp, n->motion);

    /* (2) ESKORTE — bis 2026-08-27 stand hier "idle NPC must stay put".
     *
     * Das war eine Zusicherung auf einen NICHT IMPLEMENTIERTEN Zweig: NPC-State 1 lief im Port
     * in `case 1: default:` und hielt nur die Idle-Pose. Das Original hat dort eine vollstaendige
     * ESKORTE-Maschine (Wurzel 0x8011ce54, siehe re15_npc_escort_tick): Ziel = ein Punkt 1500
     * Einheiten hinter dem Spieler (@0x8011ce78 `ori a0,zero,0x5dc`, Winkel ±0x800 @0x8011ceb0),
     * losgehen bei Distanz >= 1501 (@0x8004f118 `sltiu v0,v0,0x5dd`), stehenbleiben bei < 500
     * (@0x8004f3bc `sltiu v0,v0,0x1f4`), wieder los bei >= 1001 (@0x8004f818 `sltiu v0,v0,0x3e9`).
     * Der Nutzer-Befund war genau das: "ausserdem folgt mir ada nicht nach der cutscene".
     *
     * Die Wache prueft deshalb jetzt die SCHLEIFE statt der Regungslosigkeit — das ist schaerfer:
     * sie faellt sowohl, wenn die NPC gar nicht laeuft, als auch, wenn sie durch den Spieler
     * hindurchlaeuft oder nie zur Ruhe kommt. */
    int32_t nx0 = n->x, nz0 = n->z;
    int saw_advance = 0; uint8_t last = n->anim_frame;
    for (int f = 0; f < 120; f++) {
        re15_enemy_ai_run_all(0);
        if (n->anim_frame != last) saw_advance = 1;
        last = n->anim_frame;
    }
    int32_t moved = (n->x - nx0) * (n->x - nx0) + (n->z - nz0) * (n->z - nz0);
    if (moved == 0) {
        fprintf(stderr, "FAIL(2): die NPC muss dem Spieler FOLGEN (Eskorte 0x8011ce54), "
                        "blieb aber bei (%d,%d) stehen\n", n->x, n->z);
        fail = 1;
    }
    /* Sie muss die Schleife auch SCHLIESSEN: nach dem Anlaufen steht sie im Halte-Band.
     * Sub 1 -> Sub 3 bei Distanz < 500 (@0x8004f3c8), Sub 3 -> Sub 1 erst wieder ab 1001. */
    if (n->sub_state_1 != 3) {
        fprintf(stderr, "FAIL(2): die Eskorte kommt nicht zur Ruhe — Sub %u statt 3 "
                        "(Ankunft @0x8004f3c8), Distanz %u\n", n->sub_state_1, n->ai_dist);
        fail = 1;
    }
    /* ⛔ KORREKTUR 2026-08-28: hier stand `>= 0x1f4` (Halte-PUNKT). Das war eine Folge des
     * alten Port-Sub-3, der STEHEN BLIEB. Byte-true ist Sub 3 aber ein Gehen mit HALBEM
     * Tempo (`lbu v0,0(at)` aus 0x80076c00 / `srl v0,v0,1` / `sh v0,140(v1)` @0x8004fa7c-88),
     * und verlassen wird er erst wieder ab 1001 (`sltiu v0,v0,0x3e9` @0x8004f818). Die NPC
     * pendelt also im BAND 500..1000, statt auf 500 stehen zu bleiben. Gemessen 884. */
    if (n->ai_dist >= 0x3e9u) {
        fprintf(stderr, "FAIL(2): Halte-BAND nicht erreicht - +0x1d0 = %u, erwartet < 1001 "
                        "(@0x8004f3bc)\n", n->ai_dist);
        fail = 1;
    }
    if (!saw_advance)               { fprintf(stderr, "FAIL(2): idle pose must animate (anim_frame advances)\n"); fail = 1; }
    if (n->hp != -1)                { fprintf(stderr, "FAIL(2): NPC must stay invulnerable, hp=%d\n", n->hp); fail = 1; }
    printf("  (2) ESKORTE: (%d,%d) -> (%d,%d), Sub %u, +0x1d0 = %u, hp=%d\n",
           nx0, nz0, n->x, n->z, n->sub_state_1, n->ai_dist, n->hp);

    /* (3) DIE ANIMATION DER ESKORTE (Nutzer-Report 2026-08-28: "Adas Lauf- und Idle
     * Animation beim Folgen sind noch falsch").
     *
     * Byte-true schreibt jeder Exec-Sub seinen Clip in PHASE 0 (+0x6 == 0) und laesst ihn
     * danach nur noch laufen:
     *     Stehen  Sub 0: `ori v0,zero,0x2` / `sb v0,148(v1)` @0x8004f350-54   -> Clip 2
     *     Gehen   Sub 1: `ori v0,zero,0x5` / `sb v0,148(v1)` @0x8004f520-24   -> Clip 5
     *     Laufen  Sub 5: `sb zero,148(v0)` @0x8004fe88                        -> Clip 0
     *     Nah     Sub 3: `ori v0,zero,0x5` / `sb v0,148(v1)` @0x8004fa2c-30   -> Clip 5
     * Und ALLE posieren aus dem Kanal-Paar +0x170/+0x174 = BANK 1 (`lw a0,368(v0)` /
     * `lw a1,372(v0)` unmittelbar vor `jal 0x8001f314`: @0x8004f384-88, @0x8004f5c0-c4,
     * @0x8004f7bc-c0, @0x8004fb14, @0x8004ff68).
     *
     * Zwei Defekte, die genau hier haengen geblieben waren:
     *   a) das Phasen-Tor fehlte -> Clip/Bild/Blend wurden in JEDEM Bild neu gesetzt, der
     *      Bildzaehler stand still (gemessen: anim_frame konstant 1, anim_frac konstant 6).
     *   b) der globale Advancer taktete die NPC in State 1 ZUSAETZLICH (gemessen +2 Bilder
     *      pro Bild) — im Original ist anim_set der einzige Stepper (`lbu v0,149(v1)` /
     *      `addiu v0,v0,1` / `sb v0,149(v1)` @0x8001f60c-1c).
     * Die Wache misst deshalb den FORTSCHRITT des Bildzaehlers, nicht nur den Clip. */
    {
        /* Weit weg stellen -> Sub 1 (Gehen). */
        pl->x = 0; pl->z = 0;
        n->x = 6000; n->z = 0; n->sub_state_1 = 0; n->sub_state_2 = 0;
        for (int f = 0; f < 6; f++) re15_enemy_ai_run_all(0);
        if (n->sub_state_1 != 1 && n->sub_state_1 != 5) {
            fprintf(stderr, "FAIL(3): bei grossem Abstand muss die Eskorte gehen (Sub 1) oder "
                            "laufen (Sub 5, ab 3001 @0x8004f434), Sub %u\n", n->sub_state_1);
            fail = 1;
        }
        uint8_t want = (n->sub_state_1 == 5) ? 0 : 5;
        if (n->motion != want) {
            fprintf(stderr, "FAIL(3): Sub %u muss Clip %u spielen (@0x8004f520-24 bzw. "
                            "@0x8004fe88), gemessen %u\n", n->sub_state_1, want, n->motion);
            fail = 1;
        }
        /* Der Bildzaehler muss um GENAU 1 pro Bild steigen (kein Stillstand, kein Doppel-Tick). */
        int steps_ok = 1;
        for (int f = 0; f < 8; f++) {
            uint8_t before = n->anim_frame;
            int len = re15_actor_clip_len(n);
            re15_enemy_ai_run_all(0);
            uint8_t after = n->anim_frame;
            uint8_t expect = (uint8_t)((before + 1) % (len > 0 ? len : 1));
            if (after != expect) {
                fprintf(stderr, "FAIL(3): anim_frame muss um genau 1 steigen (anim_set ist der "
                                "einzige Stepper, @0x8001f60c-1c): %u -> %u, erwartet %u "
                                "(Clip-Laenge %d)\n", before, after, expect, len);
                steps_ok = 0; fail = 1; break;
            }
        }
        /* Gegenprobe zur Writher-Wache: die NPC-Familie MUSS in der Eskorte die eigene
         * Bank 1 nehmen - dieselbe Regel, dasselbe Praedikat. */
        if (!re15_actor_uses_own_bank(n)) {
            fprintf(stderr, "FAIL(3): die Eskorte MUSS aus BANK 1 posieren (+0x170/+0x174, f314-Loads \n0x8004f5c0-c4 u.a.), Praedikat sagt nein\n");
            fail = 1;
        }
        if (steps_ok)
            printf("  (3) LAUF-ANIM: Sub %u, Clip %u, anim_frame steigt um genau 1 pro Bild "
                   "(Clip-Laenge %d aus BANK 1)\n",
                   n->sub_state_1, n->motion, re15_actor_clip_len(n));

        /* Nahe stellen -> Sub 0 (Stehen), Clip 2. */
        n->x = 200; n->z = 0; n->sub_state_1 = 0; n->sub_state_2 = 0;
        for (int f = 0; f < 4; f++) re15_enemy_ai_run_all(0);
        if (n->sub_state_1 == 0 && n->motion != 2) {
            fprintf(stderr, "FAIL(3): der Steh-Sub muss Clip 2 spielen (@0x8004f350-54), "
                            "gemessen %u\n", n->motion);
            fail = 1;
        }
        printf("  (3) IDLE-ANIM: Sub %u, Clip %u\n", n->sub_state_1, n->motion);
    }

    if (fail) { printf("NPC WAVE-1: FAIL\n"); return 1; }
    printf("NPC WAVE-1: all checks passed\n");
    return 0;
}
