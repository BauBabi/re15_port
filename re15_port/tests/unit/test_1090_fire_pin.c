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
    re15_esp_fx_t *f = re15_esp_type26_flame(&bank, /*grid*/1, /*phase*/0x28, 2375, -1800, -2333, 1024);
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
    re15_esp_fx_t *fb = re15_esp_type26_flame(&bank, /*grid*/4, /*phase*/0x2c, 1374, -1800, 1340, 1024);
    CHECK(fb && fb->scale16 == 0x2C00, "Behavior-B scale16=0x%04X (erwartet 0x2C00)",
          fb ? fb->scale16 : 0);
    CHECK(fb && fb->effect_id == 0x10, "Behavior-B id=0x%02X (erwartet 0x10)", fb ? fb->effect_id : 0);

    /* FUNKEN beim Erscheinen: a0 = 0x09031800. */
    re15_esp_fx_t *fs = re15_esp_type26_emerge(&bank, /*grid*/1, 2375, -1800, -2333, 1024);
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
    CHECK(re15_esp_type26_emerge(&bank, 0x81, 0, 0, 0, 0) == NULL,
          "grid 0x81 duerfte KEINE Funken spawnen (Gate grid&0x80)");
    /* NEGATIV: Variante >= 5 spawnt keine Flamme. */
    CHECK(re15_esp_type26_flame(&bank, 5, 0x28, 0, 0, 0, 0) == NULL,
          "Variante 5 duerfte KEINE Flamme spawnen");

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
    re15_esp_fx_t *fl = re15_esp_type26_flame(&bank, 1, 0x28, 2375, -1800, -2333, 1024);
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

    re15_esp_fx_reset();
    re15_esp_set_room_bank(NULL);
    free(raw);

    printf(g_fail ? "\nFEHLER: %d\n" : "\nOK (%d Fehler)\n", g_fail);
    return g_fail ? 1 : 0;
}
