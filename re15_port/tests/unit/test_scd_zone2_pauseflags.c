/* test_scd_zone2_pauseflags.c — PIN: SCD-Flag-ZONE 2 IST das Pause-Wort DAT_800aca40.
 *
 * Der `Set`-Opcode (0x22) loest seine Zone NICHT in ein privates Flag-Array auf, sondern
 * ueber eine Zeiger-Tabelle in ein Engine-Global.  LAB_8003fdd0 (SCD-Dispatch @0x80074530):
 *     @0x8003fdd8 lbu  v1,0x1(v0)                  v1 = zone
 *     @0x8003fddc lbu  a1,0x2(v0)                  a1 = bit
 *     @0x8003fde0 lbu  a2,0x3(v0)                  a2 = op
 *     @0x8003fdec sll  v1,v1,0x2
 *     @0x8003fdf8 lui  at,0x8007
 *     @0x8003fdfc addiu at,at,0x4664
 *     @0x8003fe00 addu at,at,v1
 *     @0x8003fe04 lw   v1,0x0(at)   =>  PTR_DAT_80074664[zone]
 *     @0x8003fdf0 sra  v0,a1,0x5 / @0x8003fdf4 sll v0,v0,0x2      = Wort-Offset
 *     @0x8003fe0c addu a3,v0,v1                                    = &bank[wort]
 *     @0x8003fdd4 lui  t0,0x8000 + @0x8003fe4c/68/7c srlv v1,t0,a1 = 0x80000000 >> (bit&0x1f)
 *     op 1 -> or @0x8003fe6c | op 0 -> nor+and @0x8003fe54-58 | op 7 -> xor @0x8003fe80
 * Tabelle @0x80074664:  [0] DAT_800aca38  [1] DAT_800aca3c  [2] DAT_800aca40  [3] DAT_800b0ff8 ...
 * DAT_800aca40 == g_re15_pauseflags, also:
 *     Set(2,0,*) -> 0x80000000 RE15_PAUSE_PLAYER (FUN_80031c44 @0x80031c78 `bltz`)
 *     Set(2,2,*) -> 0x20000000 RE15_PAUSE_AI     (FUN_80100424 @0x8010043c)
 *     Set(2,7,*) -> 0x01000000 RE15_PAUSE_PAD    (FUN_80030444 @0x800304f8)
 *
 * ANWENDUNGSFALL, an dem der Defekt aufgefallen ist (Nutzer 2026-08-22, ROOM11F0-Generator):
 * sub01 @0x108C laeuft jedes Bild; ab @0x12A4 steht
 *     @0x12A4 Ifel_ck { @0x12A8 Ck(5,0x0C)==1 -> @0x12AC Set(2,0,1) ; @0x12B0 Set(2,2,1) }
 * Das ist der Raetsel-Freeze: der Spieler steht still, waehrend die SCD-VM (nicht gefroren,
 * Bit 0x02000000 bleibt aus) und der Pad (Bit 0x01000000 bleibt aus) den Cursor weiterfahren.
 * Ohne die Bindung lief der Spieler mit dem Cursor-D-Pad MIT und stand nach dem Raetsel
 * woanders — gemessen im echten Spiel bis zu ~3900 Einheiten neben dem Panel.
 */
#include "re15_rdt.h"
#include "re15_scd.h"
#include "re15_actor.h"
#include "re15_aot.h"
#include "re15_room.h"
#include "re15_msg.h"
#include "re15_collision.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef RE15_ASSET_PSX_DIR
#define RE15_ASSET_PSX_DIR "shared_assets/PSX"
#endif

extern scd_vm_t g_scd;
extern uint32_t g_re15_pauseflags;

static int g_fail = 0;
#define CHECK(cond, ...) do { if (!(cond)) { printf("FAIL: "); printf(__VA_ARGS__); \
                              printf("\n"); g_fail = 1; } } while (0)

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
    /* ---- T1: die drei Bits einzeln, ueber den gemeinsamen Flag-Schreiber ---- */
    re15_pauseflags_clear();
    re15_game_flag_set(2, 0, 1);
    CHECK(g_re15_pauseflags == 0x80000000u,
          "Set(2,0,1) -> pauseflags 0x%08lX, erwartet 0x80000000 (RE15_PAUSE_PLAYER)",
          (unsigned long)g_re15_pauseflags);
    re15_game_flag_set(2, 2, 1);
    CHECK(g_re15_pauseflags == 0xA0000000u,
          "Set(2,2,1) -> pauseflags 0x%08lX, erwartet 0xA0000000 (+RE15_PAUSE_AI)",
          (unsigned long)g_re15_pauseflags);
    re15_game_flag_set(2, 7, 1);
    CHECK(g_re15_pauseflags == 0xA1000000u,
          "Set(2,7,1) -> pauseflags 0x%08lX, erwartet 0xA1000000 (+RE15_PAUSE_PAD)",
          (unsigned long)g_re15_pauseflags);
    re15_game_flag_set(2, 0, 0);
    re15_game_flag_set(2, 2, 0);
    re15_game_flag_set(2, 7, 0);
    CHECK(g_re15_pauseflags == 0u,
          "Clear -> pauseflags 0x%08lX, erwartet 0", (unsigned long)g_re15_pauseflags);

    /* Andere Zonen duerfen das Pause-Wort NICHT anfassen (Tabelle: [5]=0x800b1028 usw.). */
    re15_game_flag_set(5, 0, 1);
    re15_game_flag_set(3, 2, 1);
    re15_game_flag_set(9, 7, 1);
    CHECK(g_re15_pauseflags == 0u,
          "Zone 3/5/9 haben das Pause-Wort veraendert (0x%08lX)",
          (unsigned long)g_re15_pauseflags);
    /* Zone 2, Bit >= 32 liegt hinter DAT_800aca40 und wird vom Port nicht modelliert —
     * kein ausgeliefertes Skript benutzt es (Zensus: nur Bit 0, 2 und 7). */
    re15_game_flag_set(2, 32, 1);
    CHECK(g_re15_pauseflags == 0u,
          "Zone 2, Bit 32 (Wort 1) haette das Pause-Wort nicht anfassen duerfen (0x%08lX)",
          (unsigned long)g_re15_pauseflags);
    re15_game_flag_set(2, 32, 0);
    re15_pauseflags_clear();

    /* ---- T2: das ECHTE ROOM11F0-Skript, echte VM ---- */
    char rp[600];
    snprintf(rp, sizeof rp, "%s/STAGE1/ROOM11F0.RDT", RE15_ASSET_PSX_DIR);
    size_t sz = 0; uint8_t *raw = slurp(rp, &sz);
    if (!raw) { printf("SKIP: %s fehlt\n", rp); return 77; }
    static re15_rdt_t rdt;
    if (re15_rdt_parse(raw, sz, &rdt) < 0) { printf("FAIL: RDT-Parse\n"); free(raw); return 1; }

    /* Byte-Anker: sub01 @0x12A4 `Ifel_ck { Ck(5,0x0C)==1 ; Set(2,0,1) ; Set(2,2,1) }` */
    CHECK(raw[0x12A4] == 0x06, "@0x12A4 ist kein Ifel_ck (%02X)", raw[0x12A4]);
    CHECK(raw[0x12A8] == 0x21 && raw[0x12A9] == 0x05 && raw[0x12AA] == 0x0C && raw[0x12AB] == 0x01,
          "@0x12A8 ist kein Ck(5,0x0C,1) (%02X %02X %02X %02X)",
          raw[0x12A8], raw[0x12A9], raw[0x12AA], raw[0x12AB]);
    CHECK(raw[0x12AC] == 0x22 && raw[0x12AD] == 0x02 && raw[0x12AE] == 0x00 && raw[0x12AF] == 0x01,
          "@0x12AC ist kein Set(2,0,1) (%02X %02X %02X %02X)",
          raw[0x12AC], raw[0x12AD], raw[0x12AE], raw[0x12AF]);
    CHECK(raw[0x12B0] == 0x22 && raw[0x12B1] == 0x02 && raw[0x12B2] == 0x02 && raw[0x12B3] == 0x01,
          "@0x12B0 ist kein Set(2,2,1) (%02X %02X %02X %02X)",
          raw[0x12B0], raw[0x12B1], raw[0x12B2], raw[0x12B3]);
    /* sub17 @0x16E8/@0x16EC und sub18 @0x172E/@0x1732 loeschen dieselben Bits wieder. */
    CHECK(raw[0x16E8] == 0x22 && raw[0x16E9] == 0x02 && raw[0x16EA] == 0x00 && raw[0x16EB] == 0x00 &&
          raw[0x16EC] == 0x22 && raw[0x16ED] == 0x02 && raw[0x16EE] == 0x02 && raw[0x16EF] == 0x00,
          "sub17 @0x16E8/@0x16EC ist nicht Set(2,0,0)+Set(2,2,0)");
    CHECK(raw[0x172E] == 0x22 && raw[0x172F] == 0x02 && raw[0x1730] == 0x00 && raw[0x1731] == 0x00 &&
          raw[0x1732] == 0x22 && raw[0x1733] == 0x02 && raw[0x1734] == 0x02 && raw[0x1735] == 0x00,
          "sub18 @0x172E/@0x1732 ist nicht Set(2,0,0)+Set(2,2,0)");

    re15_actor_init(); re15_aot_init(); scd_vm_init();
    re15_pauseflags_clear();
    g_current_room_id = 0x11F0; g_room_change.pending = 0;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100;
    pl->x = -1437; pl->z = -15282; pl->rot_y = 1504; pl->y = 0;
    re15_collision_set_band(0);
    scd_room_reenter(&rdt, pl->x, pl->z, 0);
    for (int f = 0; f < 20; f++) scd_vm_tick();
    CHECK(g_re15_pauseflags == 0u,
          "vor dem Raetsel steht schon ein Freeze (0x%08lX)", (unsigned long)g_re15_pauseflags);

    /* Raetsel-Zustand herstellen wie sub16 @0x15C2..0x15F2 (Set(5,0x00..0x0C)=1). */
    for (int b = 0; b <= 0x0C; b++) re15_game_flag_set(5, (uint8_t)b, 1);
    for (int f = 0; f < 4; f++) scd_vm_tick();
    CHECK((g_re15_pauseflags & 0x80000000u) != 0,
          "sub01 @0x12AC hat RE15_PAUSE_PLAYER nicht gesetzt (0x%08lX)",
          (unsigned long)g_re15_pauseflags);
    CHECK((g_re15_pauseflags & 0x20000000u) != 0,
          "sub01 @0x12B0 hat RE15_PAUSE_AI nicht gesetzt (0x%08lX)",
          (unsigned long)g_re15_pauseflags);
    /* Der Pad darf NICHT eingefroren sein — sonst koennte sub01 den Cursor nicht mehr
     * fahren (Sce_key_ck liest DAT_800ac768, das FUN_80030444 @0x800304f8 bei Bit
     * 0x01000000 auf 0xf000 maskiert). Genauso die SCD-VM (Bit 0x02000000). */
    CHECK((g_re15_pauseflags & 0x01000000u) == 0,
          "der Pad ist waehrend des Raetsels eingefroren (0x%08lX) — Cursor unfahrbar",
          (unsigned long)g_re15_pauseflags);
    CHECK((g_re15_pauseflags & 0x02000000u) == 0,
          "die SCD-VM ist waehrend des Raetsels eingefroren (0x%08lX)",
          (unsigned long)g_re15_pauseflags);

    /* Selbstheilung: das Original stellt beim Text-Dismiss den Schnappschuss wieder her
     * (@0x800285a4 `sw a0,0x800aca40`) — sub01 setzt die Bits im naechsten Bild neu. */
    re15_pauseflags_clear();
    scd_vm_tick();
    CHECK((g_re15_pauseflags & 0xA0000000u) == 0xA0000000u,
          "sub01 stellt den Freeze nach einem Wisch nicht wieder her (0x%08lX)",
          (unsigned long)g_re15_pauseflags);

    /* Raetsel-Ende wie sub17/sub18: die Bits fallen. */
    for (int b = 0; b <= 0x0C; b++) re15_game_flag_set(5, (uint8_t)b, 0);
    re15_game_flag_set(2, 0, 0);
    re15_game_flag_set(2, 2, 0);
    for (int f = 0; f < 4; f++) scd_vm_tick();
    CHECK(g_re15_pauseflags == 0u,
          "nach dem Raetsel bleibt ein Freeze stehen (0x%08lX)", (unsigned long)g_re15_pauseflags);

    free(raw);
    printf(g_fail ? "test_scd_zone2_pauseflags: ABWEICHUNG\n" : "test_scd_zone2_pauseflags: OK\n");
    return g_fail;
}
