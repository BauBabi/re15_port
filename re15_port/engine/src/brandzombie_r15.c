/*
 * RE1.5 Rebuilt — BRENNENDE ZOMBIES (Kopf: include/re15_brandzombie.h). Alle Adressen STAGE1.BIN (Overlay roh
 * @0x80100000), selbst disassembliert 2026-10-30; die STAGE5-Zwillinge (die Raeume 5120/5140 laden STAGE5) sind
 * byte-gleich bis auf die Verschiebung: INIT-Arm `ori 0x100` @0x80100a54, `== 300` @0x801013b4, Teiletabelle
 * @0x8011e834 (= STAGE1 @0x8011f7a4, 16 Bytes gleich).
 */
#include "re15_brandzombie.h"
#include "re15_esp.h"
#include "re15_damage.h"       /* re15_engine_rand8, re15_enemy_part_point */
#include "re15_enemy_ai.h"     /* re15_ai_set_state_word */
#include "re15_ai_flavor.h"    /* re15_ai_re2_for_type, re15_re2z_owns_type, re15_re2z_footlock */
#include "re15_audio.h"
#include "re15_scd.h"          /* re15_game_flag_get / _set, re15_em_status_zone */
#include "re2_ems.h"           /* re2_hybrid_perm */

int re15_enemy_clip_done_pub(const re15_actor_t *e);   /* enemy_ai_common.c */
uint8_t re15_em_status_zone(void);                     /* enemy_ai_common.c */

/* Teiletabelle u8 @0x8011f7a4 (16 Bytes, `read 0x8011f7a4 16`): Variante 0 / 1 je acht RE1.5-Teile. */
static const uint8_t s_teile[16] = { 0, 7, 8, 14, 2, 4, 9, 13,   0, 7, 8, 14, 1, 5, 10, 11 };
/* Die acht a0-Woerter (Effekt 8, Sub 3, Skala) der INIT-Aufrufe @0x80100954-58, @0x801009c8-cc, @0x80100a1c-20,
 * @0x80100a78-7c, @0x80100acc-d0, @0x80100b20-24, @0x80100b74-78, @0x80100bc8-cc. */
static const uint16_t s_skala[8] = { 0x1800, 0x2000, 0x1000, 0x1000, 0x1500, 0x1500, 0x1000, 0x1000 };
/* a3 je Aufruf: s2 = sp+40 <- Woerter @0x80100014 (0,100,0) (@0x801006cc-f4), s3 = sp+24 <- @0x80100004
 * (-100,-600,0) (@0x801006a0-c8), s0 = @0x8011f7d4 (0,0,0) (@0x80100a24-28). Reihenfolge a3 = s2, s3, s0, s0,
 * s0, s3, s0, s0 (@0x8010095c / @0x801009d0 / @0x80100a2c / @0x80100a80 / @0x80100ad4 / @0x80100b28 / ...). */
static const int32_t s_ofs[8][3] = {
    { 0, 100, 0 }, { -100, -600, 0 }, { 0, 0, 0 }, { 0, 0, 0 },
    { 0, 0, 0 },   { -100, -600, 0 }, { 0, 0, 0 }, { 0, 0, 0 } };

/* RE1.5-Teilnummer -> Knochen der geladenen Bank. Im RE2-Gehirn ist das das RE2-Rig (re2_ems.h: RE2-Slot ->
 * RE1.5-Index), die Umkehrung trifft dasselbe Glied. */
static int knochen(const re15_actor_t *e, int k15)
{
    if (re15_ai_re2_for_type(e->type) && re15_re2z_owns_type(e->type)) {
        const int8_t *perm = NULL; int n = re2_hybrid_perm((int)e->type, &perm);
        for (int i = 0; perm && i < n; i++) if (perm[i] == k15) return i;
    }
    return k15;
}

void re15_brand_init(re15_actor_t *e, int slot)
{
    if (!e || !re15_game_flag_get(1, 31)) return;                /* lw aca3c / andi 1 / beq @0x801008f4-904 */
    re15_brand_scharf(e, slot);
}

void re15_brand_scharf(re15_actor_t *e, int slot)
{
    if (!e) return;
    e->ai_flags |= 0x100u;                                        /* ori 0x100 / sh 472 @0x80100918-28 */
    unsigned v = re15_engine_rand8() & 1u;                        /* jal rng @0x80100924, andi s1,v0,1 @0x80100934 */
    e->ai_flags |= (uint16_t)(v << 9);                            /* sll 9 / or / sh 472 @0x8010093c-48 */
    unsigned r1 = re15_engine_rand8();                            /* jal rng @0x80100944 -> s0 (Delay @0x80100950) */
    unsigned r2 = re15_engine_rand8();                            /* jal rng @0x8010094c -> v0 */
    e->ai_attack_timer = (int16_t)((r1 & 0xffu) + 600u + (r2 & 0xffu));   /* andi/addiu 600/andi/addu, sh 474
                                                                   * @0x80100960-7c */
    const re15_esp_t *bank = re15_esp_room_bank();
    for (int k = 0; k < 8; k++) {                                 /* acht jal 0x80019700 @0x801009c0 ... @0x80100c14 */
        int b = knochen(e, s_teile[v * 8u + (unsigned)k]);        /* @0x8011f7a4[s1*8 + k] (sll s1,3 @0x80100978) */
        int32_t p[3]; re15_enemy_part_point(e, b, s_ofs[k], p);
        /* a1 = +0x6a, a2 = Teil-Matrix (`lw v0,392` + Teil*0xac + 0x40, z.B. @0x801009a8-c4), a3 = Versatz */
        (void)re15_esp_fx_spawn_teil(bank, 8, 3, s_skala[k], p[0], p[1], p[2], (int16_t)e->rot_y, slot, b, s_ofs[k]);
    }
}

/* FUN_80019d50(8,3,0x16, Teil) fuer die acht Teile `t` (Aufflammen: Routine 22 -> 23 -> 18). */
static void aufflammen(const re15_actor_t *e, int slot, const uint8_t t[8])
{
    for (int k = 0; k < 8; k++) re15_esp_fx_routine_setzen(8, 3, 22, slot, knochen(e, t[k]));
}

void re15_brand_aufflammen_300(re15_actor_t *e, int slot)
{
    /* ⛔ ORIGINAL-FEHLER, byte-true uebernommen: der Index `andi s0,s0,0xff / sll s0,s0,3` @0x80101290-94 liest
     * ein in FUN_80101224 nie gesetztes s0 — beim Aufruf aus der Wurzel ist es immer playerX 0x800aca88
     * (`lui/addiu s0` @0x8010051c-20 vor `jalr` @0x80100588), also Index 0x440 -> Bytes @0x8011fbe4..eb = 0.
     * STAGE5 identisch (@0x8011e834 + 0x440 = @0x8011ec74, acht Nullen). Ergebnis: nur die Flamme an Teil 0
     * flammt bei 300 auf. */
    static const uint8_t null8[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
    aufflammen(e, slot, null8);
}

static void kill_flag(const re15_actor_t *e)
{
    if (e->em_flag_id != 0xFF) re15_game_flag_set(re15_em_status_zone(), e->em_flag_id, 1);   /* jal 0x8004ef90 @0x80109100 */
}

int re15_brand_tod(re15_actor_t *e, int slot)
{
    if (!e || e->sub_state_1 != 0x15) return 0;                   /* DEATH[21][0] @0x8012014c = 0x80108d74 */
    switch (e->sub_state_3) {                                     /* lbu v1,7 @0x80108d88 */
    case 0: {                                                     /* @0x80108dc4 */
        e->hit_react |= 1;                                        /* +0x93 |= 1 @0x80108dc4-d0 */
        e->sub_state_3 = 1;                                       /* +0x7 = 1 @0x80108de0 */
        e->anim_frac = 0xf;                                       /* +0x8f = 0xf @0x80108df0 */
        e->anim_frame = 0;                                        /* +0x95 = 0 @0x80108e00 */
        re15_audio_room_se((re15_engine_rand8() & 1u) ? 5 : 8);   /* rng/andi 1/beq, a0 = 8 bzw. 5, jal 0x800453d0
                                                                   * @0x80108dfc-e14 */
        /* +0x1b8 = 1 @0x80108e28: Todes-Byte, im Port ohne Leser (wie zombiegore_r15.c). */
        unsigned v = (e->ai_flags >> 6) & 8u;                     /* lhu 472 / srl 6 / andi 8 @0x80108e38-44 */
        aufflammen(e, slot, &s_teile[v]);                         /* 8x jal 0x80019d50(8,3,0x16) @0x80108e80-0x801090b0 */
        kill_flag(e);
    }   /* faellt in Phase 1 (@0x80109108) */
    /* fallthrough */
    case 1:
        e->sub_state_3 = (uint8_t)(e->sub_state_3 + re15_enemy_clip_done_pub(e));   /* f314(+0x170,+0x174,0,0x100),
                                                                   * +0x7 += v0 @0x8010911c-40 */
        break;
    default:                                                      /* Phase 2 @0x80109144 */
        e->flags |= 2;                                            /* word0 |= 2 @0x8010914c */
        re15_ai_set_state_word(e, 7u);                            /* sh 7,4 @0x8010915c-60 -> Leiche */
        break;
    }
    if (e->anim_frac == 0) re15_re2z_footlock(slot, e);           /* +0x8f == 0 -> FUN_8010939c(0,0) @0x80109170-80 */
    return 1;
}

/* ⛔ GESTALTUNG (Nutzer-Auftrag "RE2-KI ist Standard"): RE2 hat keine brennenden Zombies. Derselbe Zeitzaehler
 * +0x1da wie im RE1.5-ACTIVE (@0x80101260-6c, nur im aufrechten Zustand 1), bei 300 dasselbe Aufflammen, bei 0
 * stirbt der Zombie ueber die RE2-BRAND-Zeile 10 (Spalte 1): deren Sturz 0x80108530 traegt das RE2-eigene
 * Verkohlen FUN_80106128 (enemy_ai_re2_zombie.c re2z_dismember_row) — der RE2-Weg, verbrannt umzufallen; dazu
 * flammen alle acht Flammen auf wie in 0x80108d74. */
void re15_brand_re2_takt(re15_actor_t *e, int slot)
{
    if (!e || !(e->ai_flags & 0x100u) || e->state != 1 || e->hp < 0) return;
    e->ai_attack_timer = (int16_t)(e->ai_attack_timer - 1);
    if (e->ai_attack_timer == 300) re15_brand_aufflammen_300(e, slot);
    if (e->ai_attack_timer == 0) {
        aufflammen(e, slot, &s_teile[(e->ai_flags >> 6) & 8u]);
        e->hp = -1;
        re15_ai_set_state_word(e, 0x0a03u);                       /* Zustand 3, Zeile 10 (Brand) */
        e->re2z_hits1d2 = 1;                                      /* Spalte 1 = Rumpf */
        kill_flag(e);
    }
}
