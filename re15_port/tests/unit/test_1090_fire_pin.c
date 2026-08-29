/* test_1090_fire_pin.c — PIN fuer den Nutzer-Befund "Aussen fehlt noch Feuer" (ROOM1090).
 *
 * ===========================================================================================
 * WAS DER RAUM IST
 * ===========================================================================================
 * ROOM1090 ist der Hinterhof/Parkplatz mit dem verunfallten "RACCOON POLICE"-Transporter.
 * ROOM109.BSS traegt nCut = 16 = 8 Kamerawinkel x 2 Beleuchtungen: Frames 0..7 sind die
 * normalen Winkel, Frames 8..15 DIESELBEN Winkel in kraeftigem orangen FEUERSCHEIN. Der Raum
 * brennt also im Original — der Nutzer-Report ist datenseitig belegt.
 *
 * ===========================================================================================
 * SOLLSEITE — Roh-Scan von STAGE1/ROOM1090.RDT (Datei-Offsets, Bytes selbst gedumpt)
 * ===========================================================================================
 * RDT+0x4C effectStart = 0x00011010, erste 8 Bytes:  05 07 09 10 FF FF FF FF
 * RDT+0x50 EFF-Zeigertabellen-ENDE = 0x000118CC (abwaerts, FUN_8001945c):
 *     Id 0x05 Eintrag 0x0008 -> Body 0x11018 (ca=9,  cb=29)
 *     Id 0x07 Eintrag 0x01D8 -> Body 0x111E8 (ca=11, cb=9)
 *     Id 0x09 Eintrag 0x0480 -> Body 0x11490 (ca=9,  cb=8)
 *     Id 0x10 Eintrag 0x0664 -> Body 0x11674 (ca=11, cb=26)   <-- die FLAMMEN
 * RDT+0x54 TIM-Basis = 0x0002EEB8, RDT+0x58 TIM-Zeigerende = 0x00039BD8 (abwaerts,
 * FUN_800194f8): Eintraege 0x0000 / 0x2440 / 0x3480 / 0x64C0 ->
 *     Id 0x05 TIM 0x2EEB8 (4bpp 256x72)   = Gore/Blut-Brocken
 *     Id 0x07 TIM 0x312F8 (4bpp 256x32)   = glimmende Schwelbrocken
 *     Id 0x09 TIM 0x32338 (4bpp 256x96)   = FUNKEN
 *     Id 0x10 TIM 0x35378 (4bpp 256x144)  = FLAMMEN (8 grosse Frames + 2x 3x3-Kachelsaeule)
 * sub00 @0x2214: 7x Sce_em_set (Opcode 0x44, 20 B) — die brennenden Truemmer:
 *     44 00 26 00 | 44 01 26 01 | 44 02 26 02 | 44 03 26 04 | 44 04 26 03 | 44 05 26 03
 *     | 44 06 26 04      (Typ 0x26, Spawn-Byte pc[3] = grid_id = 0,1,2,4,3,3,4)
 *     Positionen (pc[8..13] LE, alle Y=-1800): (2052,-1334) (2375,-2333) (3104,-3498)
 *     (2974,-860) (2874,440) (2774,2040) (1374,1340)  [X,Z]
 *   Typ 0x26 kommt in KEINEM der uebrigen 239 RDTs vor (Voll-Zensus).
 *
 * ===========================================================================================
 * ORIGINAL-MECHANISMUS (disassembliert, Adressen belegt)
 * ===========================================================================================
 * EXE-Dispatch 0x80072bac[0x26] = 0x80116288 (STAGE1-Registrierung @0x8011e8f4).
 * Root 0x80116288 dispatcht entity[+0x4] ueber @0x80121268.
 *
 * FUNKEN (INIT-Zustand 0x801164b0, Schwanz @0x801166c4-e8):
 *     801166c4  andi v0,v0,0x80        ; Gate: grid_id & 0x80 -> KEIN Spawn
 *     801166cc  bne  v0,zero,<skip>
 *     801166d4  lui  a3,0x8012 / addiu a3,a3,4680     ; a3 = 0x80121248 (Null-SVECTOR)
 *     801166e0  lui  a0,0x903  / ori a0,a0,0x1800     ; a0 = 0x09031800
 *     801166e4  jal  0x80019700                        ; Id 0x09, sub 3, scale 0x1800
 *
 * FLAMME (FUN_80116d00, gerufen aus Zustand 1 / Substate 0 @0x801167a4 bzw. @0x801168bc):
 *     80116d10  lbu  v0,9(v0)          ; entity[+0x9] = grid_id (Sce_em_set pc[3])
 *     80116d18  andi v1,v0,0x7f
 *     80116d1c  sltiu v0,v1,0x5
 *     80116d20  beq  v0,zero,0x80116d8c ; variant >= 5 -> KEIN Spawn
 *     80116d28  lui  at,0x8010 / addiu at,at,868       ; Sprungtabelle @0x80100364
 *     80116d3c  jr   v0
 *        [0] 0x80116d44: lui v1,0x803   -> Effekt-Id 0x08 (globale CORE00-Bank)
 *        [1] 0x80116d5c: lui v1,0x1003  -> Effekt-Id 0x10 (Raum-Bank)   @0x80116d6c
 *        [2] 0x80116d5c -> 0x10   [3] 0x80116d44 -> 0x08   [4] 0x80116d5c -> 0x10
 *     80116d70  lbu  a0,464(v0)        ; entity[+0x1D0] = Budget/Phase
 *     80116d74  lh   a1,106(v0)        ; entity[+0x6A] = rot_y
 *     80116d78  lw   a2,392(v0)        ; entity[+0x188] = Anker-Matrix
 *     80116d7c  sll  a0,a0,8
 *     80116d80  or   a0,a0,v1          ; a0 = (phase<<8) | (id<<24) | (3<<16)
 *     80116d84  jal  0x80019700
 *     80116d88  addiu a2,a2,64         ; Anker = Matrix + 64
 *   entity[+0x1D0] steht im Delay-Slot des Aufrufs schon: `ori v0,zero,0x28` @0x80116784
 *   (Behavior A, Varianten 0..2) bzw. `0x2c` @0x8011689c (Behavior B, 3..4), gespeichert per
 *   `sb v0,464(a0)` @0x801167a8 / @0x801168c0 -> scale16 = 0x2800 bzw. 0x2C00.
 *
 * VOLLSTAENDIGKEIT: ein Slice ueber ALLE 702 `jal`-Stellen zu 0x80019700 / 0x800199d4 /
 * 0x80019d50 / 0x80019ca8 in PSX.EXE + STAGE1..STAGE5.BIN loest cat 0x10 NUR im Bereich
 * 0x80116bb4-0x80116d88 auf (dieser Emitter, 4 Stellen). Es gibt keinen zweiten Feuer-
 * Spawner im Spiel — und `lui *,0x10xx` existiert game-weit genau einmal (@0x80116d6c).
 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "re15_rdt.h"
#include "re15_scd.h"
#include "re15_actor.h"
#include "re15_esp.h"
#include "re15_tim.h"
#include "re15_room.h"
#include "re15_aot.h"
#include "re15_enemy_ai.h"
#include "re15_ai_flavor.h"
#include "re15_player.h"    /* re15_actors_anim_advance */

extern scd_vm_t g_scd;

static int g_fail = 0;
#define CHECK(cond, ...) do { if (!(cond)) { printf("  FAIL: "); printf(__VA_ARGS__); \
                                             printf("\n"); g_fail++; } } while (0)

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

static uint32_t rd32(const uint8_t *p, uint32_t o)
{
    return (uint32_t)p[o] | ((uint32_t)p[o+1] << 8) | ((uint32_t)p[o+2] << 16) | ((uint32_t)p[o+3] << 24);
}

/* Ein 4bpp-Texel eines geparsten TIM -> "wird gezeichnet?" nach der byte-true GPU-Regel
 * (re15_tim_texel_argb ist genau die Regel, die der PC-Upload anwendet). */
static int tim4_opaque(const re15_tim_t *t, int x, int y)
{
    if (t->bpp != 4 || !t->clut || x < 0 || y < 0 || x >= t->width || y >= t->height) return 0;
    const uint8_t *row = (const uint8_t *)t->pixels + (size_t)y * ((size_t)t->width / 2u);
    uint8_t byte = row[x >> 1];
    int idx = (x & 1) ? (byte >> 4) : (byte & 0x0f);
    if (idx >= t->clut_entries) return 0;
    return re15_tim_texel_argb(t->clut[idx]) != 0u;   /* die eine GPU-Regel, re15_tim.h */
}

int main(void)
{
    size_t sz = 0;
    uint8_t *raw = read_file(RE15_ASSET_PSX_DIR "/STAGE1/ROOM1090.RDT", &sz);
    if (!raw) { printf("FEHLT: ROOM1090.RDT\n"); return 77; }

    /* -------------------------------------------------------------------------------------
     * A) Die Raum-ESP-Bank traegt die FLAMMEN (Id 0x10) — byte-true geparst.
     * ----------------------------------------------------------------------------------- */
    printf("A) ROOM1090 ESP-Bank (RDT+0x4C/0x50/0x54/0x58)\n");
    re15_esp_t bank;
    int rc = re15_esp_parse(raw, sz, rd32(raw, 0x4C), rd32(raw, 0x50),
                            rd32(raw, 0x54), rd32(raw, 0x58), &bank);
    CHECK(rc == 0, "re15_esp_parse rc=%d (erwartet 0)", rc);
    if (rc != 0) { free(raw); return 1; }

    CHECK(bank.id_count == 4, "id_count=%d (erwartet 4)", bank.id_count);
    static const uint8_t want_ids[4] = { 0x05, 0x07, 0x09, 0x10 };
    for (int i = 0; i < 4 && i < bank.id_count; i++)
        CHECK(bank.eff[i].effect_id == want_ids[i],
              "eff[%d].effect_id=0x%02X (erwartet 0x%02X)", i, bank.eff[i].effect_id, want_ids[i]);

    int fi = re15_esp_find_id(&bank, 0x10);
    CHECK(fi == 3, "Feuer-Id 0x10 -> eff_idx %d (erwartet 3)", fi);
    if (fi >= 0) {
        printf("   eff[%d] id=0x%02X body=0x%05X ca=%u cb=%u tim=0x%05X\n", fi,
               bank.eff[fi].effect_id, bank.eff[fi].eff_start,
               bank.eff[fi].count_a, bank.eff[fi].count_b, bank.eff[fi].tim_off);
        CHECK(bank.eff[fi].eff_start == 0x11674u, "Body 0x%05X (erwartet 0x11674)", bank.eff[fi].eff_start);
        CHECK(bank.eff[fi].count_a == 11, "count_a=%u (erwartet 11 Anim-Records)", bank.eff[fi].count_a);
        CHECK(bank.eff[fi].count_b == 26, "count_b=%u (erwartet 26 Coord-Zellen)", bank.eff[fi].count_b);
        CHECK(bank.eff[fi].tim_off == 0x35378u, "TIM 0x%05X (erwartet 0x35378)", bank.eff[fi].tim_off);

        /* Die Flammen-TIM: 4bpp, 256x144 (FUN_800194f8 laedt jede Effekt-TIM einzeln). */
        re15_tim_t tim;
        int trc = re15_tim_parse(raw + bank.eff[fi].tim_off, (int)(sz - bank.eff[fi].tim_off), &tim);
        CHECK(trc == 0, "TIM-Parse rc=%d", trc);
        if (trc == 0) {
            printf("   Flammen-TIM: %dbpp %dx%d clut=%d\n", tim.bpp, tim.width, tim.height, tim.clut_entries);
            CHECK(tim.bpp == 4,       "TIM bpp=%d (erwartet 4)", tim.bpp);
            CHECK(tim.width == 256,   "TIM w=%d (erwartet 256)", tim.width);
            CHECK(tim.height == 144,  "TIM h=%d (erwartet 144)", tim.height);
        }

        /* Anim-Records @0x1167C, 8 B je Eintrag — die Flammen-Sequenz (Zellen 0..4, zwei
         * 9er-Kachelgruppen ab Zelle 5 bzw. 14, dann 23/24/25). Erster Record roh:
         * `00 01 01 40` -> desc = 0x0100, param = 0x4001. */
        re15_esp_anim_t a0, a5;
        CHECK(re15_esp_anim(&bank, fi, 0, &a0) == 0, "anim[0] nicht lesbar");
        CHECK(re15_esp_anim(&bank, fi, 5, &a5) == 0, "anim[5] nicht lesbar");
        CHECK(a0.desc == 0x0100, "anim[0].desc=0x%04X (erwartet 0x0100)", a0.desc);
        CHECK(a5.desc == 0x0905, "anim[5].desc=0x%04X (erwartet 0x0905 = Zelle 5, 9 Kacheln)", a5.desc);
        /* Row-Block @0x1173C: Sub-Offset-Tabelle 0004 001A 0030 0046 -> vier Subs (0..3). */
        int st = re15_esp_row_streams(&bank, fi, 3);
        printf("   Row-Streams sub3 = %d\n", st);
        CHECK(st >= 1, "sub 3 hat %d Streams (erwartet >=1)", st);
    }

    /* -------------------------------------------------------------------------------------
     * B) Die Varianten-Tabelle @0x80100364 — Id 0x08 (global) vs 0x10 (Raum-Bank).
     * ----------------------------------------------------------------------------------- */
    printf("B) Varianten-Tabelle @0x80100364\n");
    static const int want_tab[5] = { 0x08, 0x10, 0x10, 0x08, 0x10 };
    for (uint8_t v = 0; v < 5; v++)
        CHECK(re15_esp_type26_flame_id(v) == want_tab[v],
              "flame_id(%u)=0x%02X (erwartet 0x%02X)", v, re15_esp_type26_flame_id(v), want_tab[v]);
    /* NEGATIV: `sltiu v0,v1,0x5` @0x80116d1c — Variante >= 5 spawnt gar nichts. */
    CHECK(re15_esp_type26_flame_id(5)    == -1, "Variante 5 muesste -1 liefern");
    CHECK(re15_esp_type26_flame_id(0x7f) == -1, "Variante 0x7F muesste -1 liefern");
    /* Das `andi v1,v0,0x7f` maskiert die Flag-Bits weg: 0x80|1 ist weiter Variante 1. */
    CHECK(re15_esp_type26_flame_id(0x81) == 0x10, "grid 0x81 muesste wie Variante 1 wirken");

    /* Die sieben Records des Raums: 0,1,2,4,3,3,4 -> 0x08,0x10,0x10,0x10,0x08,0x08,0x10 */
    static const uint8_t room_variants[7] = { 0, 1, 2, 4, 3, 3, 4 };
    int room_bank_users = 0;
    for (int i = 0; i < 7; i++)
        if (re15_esp_type26_flame_id(room_variants[i]) == 0x10) room_bank_users++;
    printf("   %d von 7 Truemmern ziehen die RAUM-Flamme 0x10, %d die globale 0x08\n",
           room_bank_users, 7 - room_bank_users);
    CHECK(room_bank_users == 4, "erwartet 4 Raum-Flammen (Varianten 1,2,4,4), gezaehlt %d", room_bank_users);

    /* -------------------------------------------------------------------------------------
     * C) Der byte-true Spawn: Id/sub/scale aus FUN_80116d00 bzw. dem INIT-Schwanz.
     * ----------------------------------------------------------------------------------- */
    printf("C) Spawn-Parameter\n");
    re15_esp_fx_reset();
    re15_esp_set_room_bank(&bank);
    re15_esp_set_global_bank(NULL);      /* Id 0x08 liegt in CORE00.ESP; hier nicht gebunden */

    /* Behavior A (Varianten 0..2): Budget 0x28 @0x80116784 -> scale16 0x2800. */
    re15_esp_fx_t *f = re15_esp_type26_flame(&bank, /*grid*/1, /*phase*/0x28, 2375, -1800, -2333, 1024, -1);
    CHECK(f != NULL, "Flammen-Spawn (Variante 1) lieferte NULL");
    if (f) {
        printf("   Flamme: id=0x%02X sub=%u scale=0x%04X eff_idx=%d\n",
               f->effect_id, f->sub_index, f->scale16, f->eff_idx);
        CHECK(f->effect_id == 0x10, "effect_id=0x%02X (erwartet 0x10)", f->effect_id);
        CHECK(f->sub_index == 3,    "sub=%u (erwartet 3)", f->sub_index);
        CHECK(f->scale16 == 0x2800, "scale16=0x%04X (erwartet 0x2800)", f->scale16);
        CHECK(f->eff_idx == 3,      "eff_idx=%d (erwartet 3 = die Flammen-TIM)", f->eff_idx);
        CHECK(f->bank == &bank,     "Flamme haengt nicht an der Raum-Bank");
    }
    /* Behavior B (Varianten 3..4): Budget 0x2C @0x8011689c -> scale16 0x2C00. */
    re15_esp_fx_t *fb = re15_esp_type26_flame(&bank, /*grid*/4, /*phase*/0x2c, 1374, -1800, 1340, 1024, -1);
    CHECK(fb && fb->scale16 == 0x2C00, "Behavior-B scale16=0x%04X (erwartet 0x2C00)",
          fb ? fb->scale16 : 0);
    CHECK(fb && fb->effect_id == 0x10, "Behavior-B id=0x%02X (erwartet 0x10)", fb ? fb->effect_id : 0);

    /* FUNKEN beim Erscheinen: a0 = 0x09031800. */
    re15_esp_fx_t *fs = re15_esp_type26_emerge(&bank, /*grid*/1, 2375, -1800, -2333, 1024, -1);
    CHECK(fs != NULL, "Funken-Spawn lieferte NULL");
    if (fs) {
        printf("   Funken: id=0x%02X sub=%u scale=0x%04X eff_idx=%d\n",
               fs->effect_id, fs->sub_index, fs->scale16, fs->eff_idx);
        CHECK(fs->effect_id == 0x09, "Funken id=0x%02X (erwartet 0x09)", fs->effect_id);
        CHECK(fs->sub_index == 3,    "Funken sub=%u (erwartet 3)", fs->sub_index);
        CHECK(fs->scale16 == 0x1800, "Funken scale=0x%04X (erwartet 0x1800)", fs->scale16);
        CHECK(fs->eff_idx == 2,      "Funken eff_idx=%d (erwartet 2 = Id 0x09)", fs->eff_idx);
    }
    /* NEGATIV: Gate `andi v0,v0,0x80` @0x801166c4 — grid-Bit 0x80 unterdrueckt die Funken. */
    CHECK(re15_esp_type26_emerge(&bank, 0x81, 0, 0, 0, 0, -1) == NULL,
          "grid 0x81 duerfte KEINE Funken spawnen (Gate grid&0x80)");
    /* NEGATIV: Variante >= 5 spawnt keine Flamme. */
    CHECK(re15_esp_type26_flame(&bank, 5, 0x28, 0, 0, 0, 0, -1) == NULL,
          "Variante 5 duerfte KEINE Flamme spawnen");

    /* -------------------------------------------------------------------------------------
     * C2) ROW-VM (Nutzer-Report 2026-08-29 "Zeug ueber den Feuern"): Routine 17
     *     (FUN_80017c00 — flags:=row[0x0e]=0x17 @0x80017c10/18, tpage|=row[0x16]=0x20
     *     @0x80017c28-3c = ABR1 ADDITIV) -> Routine 18 (FUN_80017c8c defW/defH-Oszillator),
     *     Follow-Bit 0x04 reitet auf dem Emitter (@0x80019f44-f94).
     * ----------------------------------------------------------------------------------- */
    printf("C2) Row-VM: Routine 17/18 + Follow\n");
    {
        extern void re15_actor_init(void);
        re15_actor_init();
        re15_esp_fx_reset();
        g_actors[1].active = 1;
        g_actors[1].x = 2052; g_actors[1].y = -1800; g_actors[1].z = -1334;
        re15_esp_fx_t *fr = re15_esp_type26_flame(&bank, 1, 0x28, 2052, -1800, -1334, 1024, 1);
        CHECK(fr && fr->rows_base != NULL,
              "Flamme laeuft ueber den Row-VM-Pfad (sub-3-Stream @RDT 0x1161c)");
        if (fr && fr->rows_base) {
            re15_esp_fx_tick(&bank);              /* Tick 1: Routine 17 armiert + advance */
            CHECK(fr->flags == 0x17,
                  "Routine 17: flags == 0x17 (aktiv|sichtbar|FOLLOW|ABE), ist 0x%02X", fr->flags);
            CHECK((fr->tpage & 0x60) == 0x20,
                  "Routine 17: TPAGE-ABR == 1 (ADDITIV, |=0x20 @0x80017c28-3c), tpage=0x%04X",
                  fr->tpage);
            uint16_t w0 = (uint16_t)(fr->row[0x04] | (fr->row[0x05] << 8));
            g_actors[1].y = -2740;                /* Heim-Pin des Emitters faehrt hoch
                                                   * (@0x80116444-98, Savestate: -2740) */
            re15_esp_fx_tick(&bank);              /* Tick 2: Oszillator + Follow */
            CHECK(fr->y == -2740,
                  "FOLLOW: fx reitet auf dem Emitter (y=-2740), ist %ld", (long)fr->y);
            uint16_t w1 = (uint16_t)(fr->row[0x04] | (fr->row[0x05] << 8));
            CHECK(w1 != w0,
                  "Routine 18: defW oszilliert (Schritt -20 @0x80017c6c), %u -> %u", w0, w1);
        }
        g_actors[1].active = 0;
        re15_esp_fx_reset();
    }

    /* -------------------------------------------------------------------------------------
     * D) NEGATIV-Kontrolle: ein Raum OHNE Feuer-Bank kann Id 0x10 nicht aufloesen.
     * ----------------------------------------------------------------------------------- */
    printf("D) Negativ-Kontrolle ROOM1140 (Bank `05 07`)\n");
    size_t sz2 = 0;
    uint8_t *raw2 = read_file(RE15_ASSET_PSX_DIR "/STAGE1/ROOM1140.RDT", &sz2);
    if (raw2) {
        re15_esp_t b2;
        if (re15_esp_parse(raw2, sz2, rd32(raw2, 0x4C), rd32(raw2, 0x50),
                           rd32(raw2, 0x54), rd32(raw2, 0x58), &b2) == 0) {
            CHECK(re15_esp_find_id(&b2, 0x10) < 0, "ROOM1140 duerfte KEINE Feuer-Id 0x10 tragen");
            CHECK(re15_esp_find_id(&b2, 0x05) >= 0, "ROOM1140 muesste Id 0x05 tragen");
        }
        free(raw2);
    }

    /* -------------------------------------------------------------------------------------
     * E) Port-Seite: setzt der Raum die 7 Feuer-Entities ueberhaupt?
     * ----------------------------------------------------------------------------------- */
    printf("E) Sce_em_set-Belegung nach main00+sub00\n");
    re15_rdt_t rdt;
    if (re15_rdt_parse(raw, sz, &rdt) >= 0) {
        re15_actor_init();
        scd_vm_init();
        g_current_room_id = 0x1090;
        g_room_change.pending = 0;
        re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
        pl->active = 1; pl->type = 0; pl->hp = 100;
        pl->x = -10100; pl->y = -1800; pl->z = 4200; pl->rot_y = 3072;
        scd_room_reenter(&rdt, pl->x, pl->z, /*entry_scenario=*/0);

        int n26 = 0;
        for (int i = 0; i < RE15_ACTOR_MAX; i++) {
            if (!g_actors[i].active || g_actors[i].type != 0x26) continue;
            n26++;
            printf("   actor[%d] typ=0x26 grid=0x%02X pos=(%ld,%ld,%ld) -> Flamme 0x%02X\n",
                   i, g_actors[i].grid_id, (long)g_actors[i].x, (long)g_actors[i].y,
                   (long)g_actors[i].z, re15_esp_type26_flame_id(g_actors[i].grid_id));
        }
        CHECK(n26 == 7, "%d Typ-0x26-Entities gespawnt (erwartet 7)", n26);
    }

    /* -------------------------------------------------------------------------------------
     * F) Die Flamme BRENNT DAUERHAFT: die 11 Anim-Records von Id 0x10 enden auf
     *    `00 01 ff 40` (Record 10, dur = param&0xff = 0xFF = Loop-Marker, Ziel =
     *    desc&0xff = 0) -> der Frame-Timer FUN_80019e20 springt zurueck auf Record 0.
     *    Ein 10-Frame-Loop mit je 1 Bild Standzeit = das lodernde Feuer. Negativprobe:
     *    ein Terminator-Record (dur==0 && loop==0) wuerde despawnen — passiert hier nie.
     * ----------------------------------------------------------------------------------- */
    printf("F) Loop-Test der Flammen-Sequenz (60 Ticks)\n");
    re15_esp_fx_reset();
    re15_esp_set_room_bank(&bank);
    re15_esp_fx_t *fl = re15_esp_type26_flame(&bank, 1, 0x28, 2375, -1800, -2333, 1024, -1);
    CHECK(fl != NULL, "Loop-Test: Spawn lieferte NULL");
    if (fl) {
        int max_frame = 0, wraps = 0, prev = fl->frame;
        for (int t = 0; t < 60; t++) {
            re15_esp_fx_tick(&bank);
            if (!fl->active) break;
            if (fl->frame > max_frame) max_frame = fl->frame;
            if (fl->frame < prev) wraps++;
            prev = fl->frame;
        }
        printf("   aktiv=%u max_frame=%d wraps=%d\n", fl->active, max_frame, wraps);
        CHECK(fl->active == 1, "Flamme ist nach 60 Ticks erloschen (erwartet: brennt weiter)");
        /* Von aussen sichtbar sind nur die Records 0..9: Record 10 IST der Loop-Marker und
         * wird im selben Tick konsumiert (frame++ -> 10, dur==0xFF -> frame := desc&0xff = 0).
         * Jeder Record haelt 1 Bild (param&0xff == 1) -> 10 Bilder je Runde, 6 Runden in 60. */
        CHECK(max_frame == 9, "hoechster sichtbarer Anim-Record %d (erwartet 9)", max_frame);
        CHECK(wraps == 6, "%d Loop-Rueckspruenge in 60 Ticks (erwartet 6 = 10-Bilder-Loop)", wraps);
        re15_esp_anim_t a10;
        CHECK(re15_esp_anim(&bank, fi, 10, &a10) == 0, "anim[10] nicht lesbar");
        CHECK((a10.param & 0xff) == 0xff, "anim[10].param&0xff = 0x%02X (erwartet 0xFF = Loop)",
              (unsigned)(a10.param & 0xff));
        CHECK((a10.desc & 0xff) == 0x00, "anim[10].desc&0xff = 0x%02X (erwartet 0 = Loop-Ziel)",
              (unsigned)(a10.desc & 0xff));
    }

    /* -------------------------------------------------------------------------------------
     * G) DIE ANDEREN DREI TRUEMMER — Effekt-Id 0x08 aus der GLOBALEN Bank CORE00.ESP.
     *
     * Varianten 0 und 3 (Records 0, 4 und 5 = drei der sieben Truemmer) ziehen Id 0x08 statt
     * 0x10. Diese Bank traegt KEINE eingebettete TIM: ihre EFF-Header nennen nur ein
     * `word1 = (tpage<<16)|clut`-Paar (u32 @eff_start+4), das auf eine beim Boot hochgeladene
     * VRAM-Seite zeigt. Id 0x08 @CORE00.ESP 0x628 hat word1 = 0x001E7911
     * -> tpage 0x001E = VRAM(896,256) 4bpp, clut 0x7911 = VRAM(272,484).
     *
     * QUELLE DER TEXTUR = die DATEI `DATA/TEX.TIM` (kein VRAM-Dump noetig). Belegt:
     * TEX.TIM ist 4bpp mit CLUT-Block @Datei 0x14 (32x24 -> VRAM(256,480)) und Bildblock
     * @Datei 0x620 (320 Halfwords x 256 Zeilen -> VRAM(0,0)). Der Boot-Installer blittet dessen
     * Halfword-Spalten 192..319 unveraendert nach VRAM(896,256): alle 32768 Halfwords dieses
     * Rechtecks stimmen mit dem ShowVRAM-Abzug ueberein (extracted_fx/vram_view_ground_truth.png).
     * Damit ist Id 0x08 = TEX.TIM-Spalten 192..255, Pixel @Datei 0x7A0 (Stride 640 B), CLUT
     * @Datei 0x134. Erzeugt von tools/tex_tim_effect_slice.py nach extracted_fx/effect8_fire.tim.
     *
     * WAS DIE SEITE ZEIGT: oben Explosions-/Feuerball-Wolken, unten zwei Reihen zuengelnder
     * FLAMMEN — und genau die unteren Reihen adressieren die 10 Coord-Zellen von Id 0x08
     * (v=168 bzw. v=208, S=40). Dieselbe Seite mit clut 0x7811 (Id 0x03) ist die GRAUE
     * Rauch-Variante: eine Sheet, zwei CLUTs.
     * ----------------------------------------------------------------------------------- */
    printf("G) Globale Bank CORE00.ESP — Id 0x08 (die Varianten-0/3-Flamme)\n");
    size_t gsz = 0;
    uint8_t *graw = read_file(RE15_ASSET_PSX_DIR "/DATA/CORE00.ESP", &gsz);
    CHECK(graw != NULL, "DATA/CORE00.ESP nicht lesbar");
    re15_esp_t gbank;
    int gfi = -1;
    if (graw) {
        CHECK(re15_esp_parse_global(graw, gsz, &gbank) == 0, "re15_esp_parse_global fehlgeschlagen");
        CHECK(gbank.id_count == 5, "CORE00.ESP id_count=%d (erwartet 5: 03 08 00 02 04)", gbank.id_count);
        gfi = re15_esp_find_id(&gbank, 0x08);
        CHECK(gfi == 1, "Id 0x08 -> eff_idx %d (erwartet 1; Datei-Reihenfolge 03 08 00 02 04)", gfi);
        if (gfi >= 0) {
            printf("   eff[%d] id=0x08 body=0x%05X ca=%u cb=%u tim=0x%05X\n", gfi,
                   gbank.eff[gfi].eff_start, gbank.eff[gfi].count_a, gbank.eff[gfi].count_b,
                   gbank.eff[gfi].tim_off);
            CHECK(gbank.eff[gfi].eff_start == 0x628u, "Body 0x%05X (erwartet 0x628)",
                  gbank.eff[gfi].eff_start);
            CHECK(gbank.eff[gfi].count_a == 11, "count_a=%u (erwartet 11)", gbank.eff[gfi].count_a);
            CHECK(gbank.eff[gfi].count_b == 10, "count_b=%u (erwartet 10 Flammen-Zellen)",
                  gbank.eff[gfi].count_b);
            /* KEINE eingebettete TIM — das ist der ganze Grund fuer den Datei-Slice. */
            CHECK(gbank.eff[gfi].tim_off == 0u,
                  "CORE00.ESP duerfte KEINE eingebettete TIM haben (tim_off=0x%05X)",
                  gbank.eff[gfi].tim_off);
            /* word1 @eff_start+4 = (tpage<<16)|clut. */
            uint32_t w1 = rd32(graw, gbank.eff[gfi].eff_start + 4);
            printf("   word1 = 0x%08X -> tpage 0x%04X VRAM(%d,%d) | clut 0x%04X VRAM(%d,%d)\n",
                   w1, w1 >> 16, ((w1 >> 16) & 0xf) * 64, (((w1 >> 16) >> 4) & 1) * 256,
                   w1 & 0xffff, ((w1 & 0xffff) & 0x3f) * 16, (w1 & 0xffff) >> 6);
            CHECK(w1 == 0x001E7911u, "word1=0x%08X (erwartet 0x001E7911)", w1);
        }
    }

    /* Die 5 Sheets der globalen Bank: jede benutzte Id muss einen Sheet-Index haben, sonst
     * findet pc_draw_effects keinen Render-Slot. */
    printf("H) Sheet-Index-Abbildung der globalen Bank\n");
    static const uint8_t want_sheet_ids[RE15_ESP_GLOBAL_SHEETS] = { 0x00, 0x02, 0x03, 0x04, 0x08 };
    for (int i = 0; i < RE15_ESP_GLOBAL_SHEETS; i++)
        CHECK(re15_esp_global_sheet_index(want_sheet_ids[i]) == i,
              "sheet_index(0x%02X)=%d (erwartet %d)", want_sheet_ids[i],
              re15_esp_global_sheet_index(want_sheet_ids[i]), i);
    if (graw)
        for (int i = 0; i < gbank.id_count; i++)
            CHECK(re15_esp_global_sheet_index(gbank.eff[i].effect_id) >= 0,
                  "CORE00.ESP-Id 0x%02X hat KEINEN Sheet-Index -> kein Render-Slot",
                  gbank.eff[i].effect_id);
    /* NEGATIV: eine Id, die NICHT in CORE00.ESP steht, darf keinen Sheet liefern. */
    CHECK(re15_esp_global_sheet_index(0x10) == -1, "Id 0x10 ist die RAUM-Bank, kein globales Sheet");
    CHECK(re15_esp_global_sheet_index(0x09) == -1, "Id 0x09 (Funken) ist raum-lokal, kein globales Sheet");
    CHECK(re15_esp_global_sheet_index(0x05) == -1, "Id 0x05 ist raum-lokal, kein globales Sheet");

    /* -------------------------------------------------------------------------------------
     * I) DIE TEXTUR SELBST: effect8_fire.tim muss die 10 Zellen von Id 0x08 wirklich fuellen.
     *    Das ist der Test, der die REGRESSION festnagelt: bis 2026-08-21 band der Port fuer
     *    Id 0x08 mangels Sheet das BLUT-Sheet (Slot 20), und dort sind 9 der 10 Zellen
     *    vollstaendig transparent -> die drei Truemmer blieben unsichtbar.
     * ----------------------------------------------------------------------------------- */
    printf("I) effect8_fire.tim deckt die 10 Coord-Zellen ab\n");
    size_t fsz = 0;
    uint8_t *ftim_raw = read_file(RE15_ASSET_PSX_DIR "/../extracted_fx/effect8_fire.tim", &fsz);
    CHECK(ftim_raw != NULL, "extracted_fx/effect8_fire.tim fehlt (tools/tex_tim_effect_slice.py)");
    size_t bsz2 = 0;
    uint8_t *btim_raw = read_file(RE15_ASSET_PSX_DIR "/../extracted_fx/effect0_blood.tim", &bsz2);

    if (ftim_raw && graw && gfi >= 0) {
        re15_tim_t ftim;
        int frc = re15_tim_parse(ftim_raw, (int)fsz, &ftim);
        CHECK(frc == 0, "effect8_fire.tim Parse rc=%d", frc);
        if (frc == 0) {
            printf("   Feuer-TIM: %dbpp %dx%d clut=%d\n", ftim.bpp, ftim.width, ftim.height,
                   ftim.clut_entries);
            CHECK(ftim.bpp == 4,          "bpp=%d (erwartet 4)", ftim.bpp);
            CHECK(ftim.width == 256,      "w=%d (erwartet 256)", ftim.width);
            CHECK(ftim.height == 256,     "h=%d (erwartet 256)", ftim.height);
            CHECK(ftim.clut_entries == 16, "clut=%d (erwartet 16)", ftim.clut_entries);
        }

        re15_tim_t btim; int brc = btim_raw ? re15_tim_parse(btim_raw, (int)bsz2, &btim) : -1;

        /* Jede der 11 Anim-Records nennt ihre Startzelle + die Kachelgroesse S (param>>8). */
        int cells_ok = 0, cells_seen = 0, blood_empty = 0;
        for (int i = 0; i < (int)gbank.eff[gfi].count_a; i++) {
            re15_esp_anim_t a;
            if (re15_esp_anim(&gbank, gfi, i, &a) != 0) { CHECK(0, "anim[%d] nicht lesbar", i); continue; }
            int cs = a.desc & 0xff, S = (a.param >> 8) & 0xff;
            CHECK(S == 40, "anim[%d].S=%d (erwartet 40 = 0x28)", i, S);
            if (i == 10) {   /* Loop-Marker wie bei Id 0x10: dur 0xFF, Ziel Zelle 0 */
                CHECK((a.param & 0xff) == 0xff, "anim[10] ist kein Loop-Marker (param&0xff=0x%02X)",
                      (unsigned)(a.param & 0xff));
                CHECK(cs == 0, "anim[10] Loop-Ziel=%d (erwartet 0)", cs);
            }
            re15_esp_coord_t c;
            if (re15_esp_coord(&gbank, gfi, cs, &c) != 0) { CHECK(0, "coord[%d] fehlt", cs); continue; }
            if (i == 10) continue;                     /* Zelle 0 schon ueber anim[0] gezaehlt */
            cells_seen++;
            /* Opake Texel in (u,v)..(u+S,v+S) — ein Frame ohne opake Texel ist unsichtbar. */
            int fop = 0, bop = 0;
            for (int y = c.v; y < c.v + S && y < ftim.height; y++)
                for (int x = c.u; x < c.u + S && x < ftim.width; x++) {
                    if (frc == 0 && tim4_opaque(&ftim, x, y)) fop++;
                    if (brc == 0 && tim4_opaque(&btim, x, y)) bop++;
                }
            printf("   Zelle %2d uv=(%3d,%3d) S=%2d  Feuer %4d opak | Blut %4d opak\n",
                   cs, c.u, c.v, S, fop, bop);
            /* Die Zellen liegen in den beiden unteren Flammen-Reihen der Seite. */
            CHECK(c.v == 168 || c.v == 208, "Zelle %d v=%d (erwartet 168 oder 208)", cs, c.v);
            CHECK(fop > 0, "Zelle %d ist auf dem FEUER-Sheet leer (%d opake Texel)", cs, fop);
            if (fop > 0) cells_ok++;
            if (brc == 0 && bop == 0) blood_empty++;
        }
        CHECK(cells_seen == 10, "%d Zellen geprueft (erwartet 10)", cells_seen);
        CHECK(cells_ok == 10, "%d von 10 Zellen tragen Flammen-Texel (erwartet 10)", cells_ok);
        /* NEGATIV-KONTROLLE = die Regression selbst: auf dem Blut-Sheet, das der Port vor dem
         * Fix band, sind 9 der 10 Zellen restlos leer. Genau das war "nur 4 der 7 Feuer". */
        if (brc == 0) {
            printf("   Negativ-Kontrolle: %d von 10 Zellen sind auf dem BLUT-Sheet leer\n", blood_empty);
            CHECK(blood_empty == 9,
                  "erwartet 9 leere Zellen auf dem Blut-Sheet (die Regression), gezaehlt %d", blood_empty);
        }
    }

    /* -------------------------------------------------------------------------------------
     * J) ALLE SIEBEN TRUEMMER haben jetzt eine aufloesbare Textur: 4x Raum-Bank (Id 0x10)
     *    + 3x globale Bank (Id 0x08). Vorher waren es 4.
     * ----------------------------------------------------------------------------------- */
    printf("J) Alle 7 Truemmer aufloesbar\n");
    if (graw) {
        re15_esp_fx_reset();
        re15_esp_set_room_bank(&bank);
        re15_esp_set_global_bank(&gbank);
        static const uint8_t variants[7] = { 0, 1, 2, 4, 3, 3, 4 };
        static const uint8_t phases[7]   = { 0x28, 0x28, 0x28, 0x2c, 0x2c, 0x2c, 0x2c };
        int resolved = 0, from_global = 0, from_room = 0;
        for (int i = 0; i < 7; i++) {
            re15_esp_fx_t *ff = re15_esp_type26_flame(&bank, variants[i], phases[i],
                                                      1000 + i * 100, -1800, 500, 1024, -1);
            CHECK(ff != NULL, "Truemmer %d (Variante %u) spawnte nicht", i, variants[i]);
            if (!ff) continue;
            int sheet = (ff->bank == &gbank) ? re15_esp_global_sheet_index(ff->effect_id) : -1;
            printf("   Truemmer %d Variante %u -> id 0x%02X bank=%s eff_idx=%d sheet=%d\n",
                   i, variants[i], ff->effect_id,
                   (ff->bank == &gbank) ? "GLOBAL" : (ff->bank == &bank ? "RAUM" : "?"),
                   ff->eff_idx, sheet);
            CHECK(ff->eff_idx >= 0, "Truemmer %d: Effekt-Id 0x%02X in KEINER Bank aufloesbar",
                  i, ff->effect_id);
            if (ff->eff_idx < 0) continue;
            resolved++;
            if (ff->bank == &gbank) {
                from_global++;
                /* DAS IST DER PIN: die Varianten-0/3-Emitter liefern einen gueltigen Slot. */
                CHECK(sheet == 4, "Truemmer %d: Id 0x08 -> Sheet %d (erwartet 4 = FEUER-Slot)",
                      i, sheet);
            } else {
                from_room++;
            }
        }
        CHECK(resolved == 7, "%d von 7 Truemmern haben eine Textur (erwartet 7)", resolved);
        CHECK(from_global == 3, "%d Truemmer aus der GLOBALEN Bank (erwartet 3: Varianten 0,3,3)",
              from_global);
        CHECK(from_room == 4, "%d Truemmer aus der RAUM-Bank (erwartet 4: Varianten 1,2,4,4)",
              from_room);
        re15_esp_set_global_bank(NULL);
    }

    /* -------------------------------------------------------------------------------------
     * K) ⛔ KI-FLAVOR-PIN (Nutzer-Befund 2026-08-22): "Bei RE2-KI ist im ROOM1090 der
     *    Flammen-Effekt nicht da, stattdessen gibt es komische Dreiecke, die da rumschwirren."
     *
     * URSACHE (Welle F): das Besitz-Gate des RE2-Spinnen-Brains war TYP-fest und hat den
     * ganzen Typ 0x26 beansprucht. Die sieben ROOM1090-Records sind aber KEINE Spinnen —
     * Dispatch-Registrierung STAGE1.BIN (selbst disassembliert):
     *     8011E8F0  lui   v0,0x8011
     *     8011E8F4  addiu v0,v0,25224      ; 0x80116288 = der FEUER-Emitter
     *     8011E8F8  lui   at,0x8007
     *     8011E8FC  sw    v0,11332(at)     ; 0x80072BAC + 0x26*4 = 0x80072C44
     * Gegenprobe derselben Schleife: [0x20] = 0x8010D7F8 (Hund), [0x21] = 0x80112020 (Kraehe),
     * [0x27] = 0x80116DB8 (Gorilla) — alle drei decken sich mit den verifizierten Roots.
     * In RE2 ist kind 0x26 dagegen die BABY-SPINNE, die die Adult ausstoesst:
     *     80105DE4  jal   0x8001ad3c
     *     80105DE8  addiu a0,zero,38       ; kind 38 = 0x26      (EMS25.BIN)
     * Zensus ueber alle sechs Stage-Overlays: 0x26 ist NUR in STAGE1 registriert, die
     * Adult-Spinne 0x25 NUR in STAGE2 (0x801109E4) — sie koennen gar nicht verwandt sein.
     *
     * GEMESSEN VOR DEM FIX (echte EXE, ROOM1090 ueber das Debug-Menue, RE15_STATE_LOG):
     *   RE15_AI_FLAVOR=re15 -> fx=14, alle 7 Aktoren ueber 83 Frames auf ihren RDT-Positionen.
     *   RE15_AI_FLAVOR=re2  -> fx=0, 5 von 7 laufen weg (Slot 4 (2974,-860) -> (-2025,-2)).
     * NACH DEM FIX ist der RE2-Lauf pixel-identisch zum RE1.5-Lauf (Frame 300, 0 von 691200
     * Pixeln verschieden) und die FX-Draw-Logs sind Zeile fuer Zeile gleich.
     *
     * DIESER PIN faehrt beide Flavors durch dieselbe Raum-Simulation und verlangt:
     *   (a) kein Aktor wandert,  (b) der Flammen-Emitter feuert (Effekt-Id 0x08/0x10),
     *   (c) das RE2-Brain beansprucht keinen der sieben.
     * ----------------------------------------------------------------------------------- */
    printf("K) KI-Flavor-Pin: RE1.5 vs RE2 im selben Raum\n");
    if (graw) {
        re15_rdt_t rdt2;
        if (re15_rdt_parse(raw, sz, &rdt2) >= 0) {
            static const int k_flav[2] = { RE15_AI_FLAVOR_RE15, RE15_AI_FLAVOR_RE2 };
            static const char *k_name[2] = { "RE1.5", "RE2" };
            int32_t ref_x[8], ref_z[8]; int16_t ref_r[8]; int ref_n = 0;
            for (int fl = 0; fl < 2; fl++) {
                re15_ai_flavor_set((re15_ai_flavor_t)k_flav[fl]);
                re15_actor_init(); re15_aot_init(); scd_vm_init();
                re15_enemy_ai_set_paused(0);
                re15_esp_fx_reset();
                re15_esp_set_room_bank(&bank);
                re15_esp_set_global_bank(&gbank);
                g_current_room_id = 0x1090;
                g_room_change.pending = 0;
                re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
                pl->active = 1; pl->type = 0; pl->hp = 100;
                pl->x = -10100; pl->y = -1800; pl->z = 4200; pl->rot_y = 3072;
                scd_room_reenter(&rdt2, pl->x, pl->z, 0);

                int slots[8], n = 0;
                for (int i = 1; i < RE15_ACTOR_MAX && n < 8; i++)
                    if (g_actors[i].active && g_actors[i].type == 0x26u) slots[n++] = i;
                CHECK(n == 7, "%s: %d Typ-0x26-Aktoren (erwartet 7)", k_name[fl], n);

                int32_t x0[8], z0[8];
                for (int i = 0; i < n; i++) { x0[i] = g_actors[slots[i]].x; z0[i] = g_actors[slots[i]].z; }

                /* (c) Besitz: KEINER der sieben darf dem RE2-Brain gehoeren. */
                for (int i = 0; i < n; i++)
                    CHECK(re15_re2spider_owns(&g_actors[slots[i]]) == 0,
                          "%s: RDT-0x26 Slot %d gehoert dem RE2-Spinnen-Brain (Feuer-Emitter!)",
                          k_name[fl], slots[i]);

                for (int f = 0; f < 200; f++) { re15_enemy_ai_run_all(1); re15_actors_anim_advance(); }

                /* (a) niemand wandert. */
                int moved = 0;
                for (int i = 0; i < n; i++)
                    if (g_actors[slots[i]].x != x0[i] || g_actors[slots[i]].z != z0[i]) moved++;
                CHECK(moved == 0, "%s: %d von %d Feuer-Emittern sind gewandert (byte-true: 0 — "
                      "der Root 0x80116288 hat KEINE Lokomotion)", k_name[fl], moved, n);

                /* (b) die Flammen brennen: Effekt-Id 0x08 / 0x10 im FX-Pool. */
                int n_fire = 0, n_room = 0, n_glob = 0;
                for (int i = 0; i < RE15_ESP_FX_MAX; i++) {
                    const re15_esp_fx_t *ff = re15_esp_fx_get(i);
                    if (!ff || !ff->active) continue;
                    if (ff->effect_id == 0x10) { n_fire++; n_room++; }
                    else if (ff->effect_id == 0x08) { n_fire++; n_glob++; }
                }
                printf("   %-5s: aktive Flammen id0x10=%d id0x08=%d (gesamt %d), gewandert=%d\n",
                       k_name[fl], n_room, n_glob, n_fire, moved);
                CHECK(n_fire >= 7, "%s: nur %d Flammen aktiv (erwartet >= 7 — der Nutzer-Befund "
                      "war GENAU 0 im RE2-Modus)", k_name[fl], n_fire);
                CHECK(n_room >= 4, "%s: nur %d Raum-Flammen 0x10 (Varianten 1,2,4,4 @0x80100364)",
                      k_name[fl], n_room);
                CHECK(n_glob >= 3, "%s: nur %d globale Flammen 0x08 (Varianten 0,3,3 @0x80100364)",
                      k_name[fl], n_glob);

                /* Beide Flavors muessen DENSELBEN Endzustand liefern (Negativ-Test: der
                 * RE1.5-Pfad darf sich durch den Fix nicht geaendert haben). */
                if (fl == 0) {
                    ref_n = n;
                    for (int i = 0; i < n; i++) {
                        ref_x[i] = g_actors[slots[i]].x; ref_z[i] = g_actors[slots[i]].z;
                        ref_r[i] = g_actors[slots[i]].rot_y;
                    }
                } else {
                    CHECK(n == ref_n, "RE2 stellt %d statt %d Emitter auf", n, ref_n);
                    for (int i = 0; i < n && i < ref_n; i++)
                        CHECK(g_actors[slots[i]].x == ref_x[i] && g_actors[slots[i]].z == ref_z[i]
                              && g_actors[slots[i]].rot_y == ref_r[i],
                              "RE2 Slot %d bei (%ld,%ld,r%d), RE1.5 bei (%ld,%ld,r%d)",
                              slots[i], (long)g_actors[slots[i]].x, (long)g_actors[slots[i]].z,
                              (int)g_actors[slots[i]].rot_y,
                              (long)ref_x[i], (long)ref_z[i], (int)ref_r[i]);
                }
            }
            re15_ai_flavor_set(RE15_AI_FLAVOR_RE15);
            re15_esp_set_global_bank(NULL);
        }
    }

    free(ftim_raw);
    free(btim_raw);
    free(graw);
    re15_esp_fx_reset();
    re15_esp_set_room_bank(NULL);
    free(raw);

    printf(g_fail ? "\nFEHLER: %d\n" : "\nOK (%d Fehler)\n", g_fail);
    return g_fail ? 1 : 0;
}
