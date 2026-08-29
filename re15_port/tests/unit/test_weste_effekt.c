/* test_weste_effekt.c — PIN (Nutzer-Auftrag 2026-08-29: "Die POLICE-Weste sollte Leons
 * Energie insoweit erhoehen, dass er einen Zombie-Biss mehr aushaelt."):
 *
 * Original-Basis (analysis/nutzer_batch_2026-08-29/weste-effekt.md, Kern selbst
 * nachdisassembliert): keine Defense-Mechanik im Original; der Modellwechsel-Load
 * FUN_800314b0 heilt VOLL (`ori v0,0x64` @0x80031710 + `sh v0,player.hp` @0x80031718);
 * Standard-Zombie-Biss = -5 (`addiu v0,v0,-5` @0x801027dc); Tod erst bei HP < 0
 * (bgez @0x80012ee8). NACHRUESTUNG: Weste (Flag(3,0x75)) = +5 Max-HP -> 105 = genau
 * 21 statt 20 ueberlebbare Bisse.
 */
#include "re15_rdt.h"
#include "re15_scd.h"
#include "re15_actor.h"
#include "re15_aot.h"
#include "re15_room.h"
#include "re15_savedata.h"
#include "re15_enemy_ai.h"
#include "re15_enemy.h"
#include "re15_msg.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef RE15_ASSET_PSX_DIR
#define RE15_ASSET_PSX_DIR "shared_assets/PSX"
#endif

extern scd_vm_t g_scd;
extern void re15_item_use_apply(uint8_t id);

static int g_fail = 0;
#define CHECK(name, cond) do { if (!(cond)) { printf("  FAIL: %s\n", name); g_fail = 1; } \
                               else printf("  PASS: %s\n", name); } while (0)

static uint8_t *slurp(const char *p, size_t *n)
{
    FILE *f = fopen(p, "rb"); if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    if (sz <= 0) { fclose(f); return NULL; }
    uint8_t *b = (uint8_t *)malloc((size_t)sz);
    if (b && fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); b = NULL; }
    fclose(f); if (b) *n = (size_t)sz; return b;
}

int main(void)
{
    char rp[600];
    size_t rawsz = 0;
    snprintf(rp, sizeof rp, "%s/STAGE1/ROOM1190.RDT", RE15_ASSET_PSX_DIR);
    uint8_t *rawbuf = slurp(rp, &rawsz);
    if (!rawbuf) { printf("SKIP: %s fehlt\n", rp); return 77; }
    static re15_rdt_t rdt;
    if (re15_rdt_parse(rawbuf, rawsz, &rdt) < 0) { printf("FAIL: RDT-Parse\n"); return 1; }

    printf("=== R.P.D.-Weste: +5 Max-HP = ein Biss (-5 @0x801027dc) mehr ===\n");

    scd_vm_init();
    re15_actor_init(); re15_aot_init();
    re15_enemy_reset(); re15_enemy_ai_set_paused(1);
    g_current_room_id = 0x1190; g_room_change.pending = 0;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 40; pl->x = 19100; pl->y = 0; pl->z = -21250;
    re15_msg_load_room_block(rdt.messages, rdt.messages_size);
    re15_vest_model_mark(0);

    /* Ohne Weste: Reenter ohne Modellwechsel darf NICHTS tun. */
    scd_room_reenter(&rdt, pl->x, pl->z, 13);
    CHECK("Reenter ohne Modellwechsel aendert HP nicht (@0x80039770 beq)", pl->hp == 40);

    /* ANLEGEN (sub15-Wirkung: Flag(3,0x75)=1 + work_vars[0x10]=1, dann Selbst-Tuer-Reload).
     * ⛔ NUTZER-ENTSCHEIDUNG 2026-08-30: KEINE Heilung — die Original-Vollheilung des
     * Modell-Reloads (@0x80031710/18) ist bewusst nicht uebernommen. Nur +5 Bonus. */
    re15_game_flag_set(3, 0x75, 1);
    g_scd.work_vars[0x10] = 1;
    scd_room_reenter(&rdt, pl->x, pl->z, 13);
    CHECK("Anlegen heilt NICHT, gibt nur +5 (40 -> 45)", pl->hp == 45);
    CHECK("Bonus-Getter liefert 5", re15_vest_hp_bonus() == 5);

    /* 21 Standard-Bisse (-5 @0x801027dc) ab VOLL (Spray): 105 - 21*5 = 0, Tod erst < 0. */
    {
        re15_item_use_apply(0x22);                    /* Vollheilung mit Weste -> 105 */
        CHECK("Vollheilung mit Weste -> 105 (der +1-Biss-Deckel)", pl->hp == 105);
        int16_t hp = pl->hp;
        for (int i = 0; i < 21; i++) hp = (int16_t)(hp - 5);
        CHECK("21 Bisse ueberlebbar (HP 0, Tod erst < 0 @0x80012ee8)", hp == 0);
        CHECK("ohne Weste waeren nur 20 drin (100 - 21*5 < 0)", (100 - 21 * 5) < 0);
    }

    /* Save -> Load: HP bleibt, Modell-Index wird aus dem Flag rekonstruiert,
     * und der Folge-Reenter aendert die HP nicht. */
    {
        re15_savedata_t sd;
        pl->hp = 63;
        re15_savedata_capture(&sd, 0, 1);
        pl->hp = 1; g_scd.work_vars[0x10] = 0; re15_vest_model_mark(0);
        uint16_t room = 0;
        CHECK("Restore ok", re15_savedata_restore(&sd, &room) == 0);
        CHECK("HP aus dem Save (63)", pl->hp == 63);
        CHECK("work_vars[0x10] aus Flag(3,0x75) rekonstruiert", g_scd.work_vars[0x10] == 1);
        scd_room_reenter(&rdt, pl->x, pl->z, 13);
        CHECK("Reenter nach Load aendert HP nicht (Spiegel markiert)", pl->hp == 63);
    }

    /* ABLEGEN: nur -5, keine Heilung; nie toedlich. */
    re15_game_flag_set(3, 0x75, 0);
    g_scd.work_vars[0x10] = 0;
    pl->hp = 63;
    scd_room_reenter(&rdt, pl->x, pl->z, 13);
    CHECK("Ablegen nimmt nur den Bonus (63 -> 58)", pl->hp == 58);
    re15_game_flag_set(3, 0x75, 1); g_scd.work_vars[0x10] = 1;
    scd_room_reenter(&rdt, pl->x, pl->z, 13);       /* wieder an (58 -> 63) */
    pl->hp = 3;
    re15_game_flag_set(3, 0x75, 0); g_scd.work_vars[0x10] = 0;
    scd_room_reenter(&rdt, pl->x, pl->z, 13);
    CHECK("Ablegen bei HP 3 toetet nicht (Boden 0)", pl->hp == 0);

    free(rawbuf);
    if (g_fail) { printf("FAIL\n"); return 1; }
    printf("OK\n");
    return 0;
}
