/* kleidung_r36.c — Runde 36 Spur B "kleidung": Leons zerrissene/blutige Kleidung unter jedem
 * Gegner-Gehirn. Belege und Adressen: include/re15_kleidung_r36.h, Dossier
 * analysis/befunde_runde36/B_kleidung.md §2. Die Wund-Tabelle selbst (FUN_80037edc) lebt in
 * re15_damage.c (re15_wound_add / re15_wound_release_stamp), die Anwendung auf die Textur im
 * Plattform-Renderer (main.c Blut-Decal-Sync, render_pc.c re15_render_pc_wound_blit). */
#include "re15_kleidung_r36.h"
#include "re15_damage.h"
#include "re15_esp.h"
#include "re15_engine.h"   /* g_engine.frame_count (nur Messhaken) */
#include <stdio.h>
#include <stdlib.h>

extern void re15_player_victim_bone_pos_pub(int bone, int32_t out[3]);   /* enemy_ai_common.c (Spur J r35) */

/* MESSHAKEN RE15_KLEIDUNG_LOG=1 (env-gegatet, kein Verhalten): je Stempel-Ereignis eine Zeile
 * mit Bild, Quelle, Richtung, Spieler-HP und der Wund-Tabelle (Level/Akku je Panel) DANACH —
 * belegt auf dem echten Spielweg, welcher Griff/Biss welche Panels fuellt (Dossier §5.3). */
static void kleid_log(const char *quelle, int typ, int dir)
{
    static int s_log = -1;
    if (s_log < 0) s_log = getenv("RE15_KLEIDUNG_LOG") ? 1 : 0;
    if (!s_log) return;
    fprintf(stderr, "[kleidung] F%u %s typ=0x%02x dir=%d hp=%d gen=%d panels=",
            (unsigned)g_engine.frame_count, quelle, typ, dir,
            (int)g_actors[RE15_ACTOR_SLOT_PLAYER].hp, re15_wound_generation());
    for (int p = 0; p < 8; p++)
        fprintf(stderr, "%d/%d%s", re15_wound_level(p), re15_wound_acc(p), (p < 7) ? "," : "\n");
}

/* STAGE1-Registrierung FUN_8011e864 (@0x8011e9c0-0x8011eac8, player_hit_chain.md §1):
 * Hook A @0x800ac758+Typ*4 = LAB_8010a28c fuer 0x10,0x11,0x12,0x16,0x18,0x1c..0x1f; 0x13 ->
 * LAB_8010c16c (-> 0x8010a2cc). 0x1A = PORT-WAHL (RE2-Gitterarm der ROOM1210, Opfer-Spender 0x10,
 * re15_victim_donor_set(0x1A,0x10) in main.c). 0x20/0x21/0x27 = eigene Hooks (kein generischer
 * Stempel: Hund LAB_80111944 stempelt selbst nur Panel 0 @0x80111b64, Kraehe LAB_8011597c und
 * Gorilla LAB_8011c118 stempeln beim Release gar nicht). */
int re15_kleidung_generischer_griff(uint8_t typ)
{
    switch (typ) {
        case 0x10: case 0x11: case 0x12: case 0x13: case 0x16: case 0x18:
        case 0x1A: case 0x1C: case 0x1D: case 0x1E: case 0x1F:
            return 1;
        default:
            return 0;
    }
}

/* Panel-Ziel-LUT @0x80074208 (PSX.EXE Datei-Offset 0x64a08), 8 Zeilen x 8 (x,y)-Halfword-Paare.
 * Builder FUN_80037c1c indiziert mit dem CHARAKTER-Byte DAT_800aca5c (`lbu v0,-120(s3)` @0x80037c8c
 * = 0x800acad4-0x78 = aca5c, `sll v0,v0,4` @0x80037c94 -> Zeile = char): Zeilen 0..3 = Leon-Block
 * (PL00..PL03), 4..7 = Elza-Block (PL04..PL07; lying_devour.md: Bit 2 trennt die Bloecke). dstX =
 * 448 + x (hw, `sh v0,19(s0)` @0x80037cb4), dstY = 256 + y (`addiu v0,v0,256` @0x80037cdc).
 * Rohbytes (bytes 0x80074208 128):
 *   43 23 2b 2c 1f c0 42 af 16 74 10 1a 06 66 00 00 | 43 23 2b 2c 1f c0 42 af 17 63 07 22 07 73 18 06
 *   43 23 2b 2c 1f c0 5d 71 10 75 00 28 00 6d 10 0b | 43 23 30 2c 1f c0 16 86 42 af 10 1a 06 66 00 00
 *   46 26 28 77 30 2f 42 a2 09 4f 0b 0f 1b be 2b c0 | 46 26 28 77 30 2f 42 a2 09 4f 0b 0f 1b be 2b c0
 *   46 26 28 77 30 2f 42 a2 09 4f 04 0f 1b be 2b c0 | 43 23 30 2c 1f c0 16 86 42 af 10 1a 06 66 00 00
 * Der Port nahm fuer Elza bisher Zeile 1 (= Leon-Variante PL01) statt Zeile 4 (PL04) — die Decals
 * sassen neben ihren UV-Inseln (blaue Saeume, Dossier §5.3). */
static const uint8_t s_panel_lut[8][16] = {
    { 0x43,0x23, 0x2b,0x2c, 0x1f,0xc0, 0x42,0xaf, 0x16,0x74, 0x10,0x1a, 0x06,0x66, 0x00,0x00 },
    { 0x43,0x23, 0x2b,0x2c, 0x1f,0xc0, 0x42,0xaf, 0x17,0x63, 0x07,0x22, 0x07,0x73, 0x18,0x06 },
    { 0x43,0x23, 0x2b,0x2c, 0x1f,0xc0, 0x5d,0x71, 0x10,0x75, 0x00,0x28, 0x00,0x6d, 0x10,0x0b },
    { 0x43,0x23, 0x30,0x2c, 0x1f,0xc0, 0x16,0x86, 0x42,0xaf, 0x10,0x1a, 0x06,0x66, 0x00,0x00 },
    { 0x46,0x26, 0x28,0x77, 0x30,0x2f, 0x42,0xa2, 0x09,0x4f, 0x0b,0x0f, 0x1b,0xbe, 0x2b,0xc0 },
    { 0x46,0x26, 0x28,0x77, 0x30,0x2f, 0x42,0xa2, 0x09,0x4f, 0x0b,0x0f, 0x1b,0xbe, 0x2b,0xc0 },
    { 0x46,0x26, 0x28,0x77, 0x30,0x2f, 0x42,0xa2, 0x09,0x4f, 0x04,0x0f, 0x1b,0xbe, 0x2b,0xc0 },
    { 0x43,0x23, 0x30,0x2c, 0x1f,0xc0, 0x16,0x86, 0x42,0xaf, 0x10,0x1a, 0x06,0x66, 0x00,0x00 },
};
int re15_kleidung_panel_ziel(int charakter, int panel, int *tim_x, int *tim_y)
{
    int zeile = charakter & 0x0F;                       /* DAT_800aca5c (PL00..PL0F) */
    if (zeile > 7 || panel < 0 || panel > 7) return 0;  /* nur 8 Zeilen belegt (@0x80074208-87) */
    if (tim_x) *tim_x = (int)s_panel_lut[zeile][panel * 2] * 2;   /* Halfword -> 8bpp-Texel */
    if (tim_y) *tim_y = (int)s_panel_lut[zeile][panel * 2 + 1];   /* dstY-256 = TIM-Zeile */
    return 1;
}

void re15_kleidung_abwurf(uint8_t greifer_typ, int dir)
{
    if (!re15_kleidung_generischer_griff(greifer_typ)) return;
    if (dir < 0 || dir > 3) return;                      /* Tabelle 0x801201b8 hat 4 Eintraege */
    /* Release-Blut @0x8010a574 (FUN_80019700, a0=0x1500 @0x8010a524/@0x8010a550): Anker =
     * Spieler-Part 8 (`addiu a2,a2,1440` = +0x5A0 @0x8010a570) fuer dir<2 (`sltiu v0,v0,0x2`
     * @0x8010a518), sonst Part 3 (`addiu a2,a2,580` = +0x244 @0x8010a54c); Yaw = Spieler +0x6a. */
    {
        int32_t anker[3];
        re15_player_victim_bone_pos_pub((dir < 2) ? 8 : 3, anker);
        re15_esp_fx_spawn_ex(re15_esp_room_bank(), 0, 0, 0x1500,
                             anker[0], anker[1], anker[2],
                             (int16_t)g_actors[RE15_ACTOR_SLOT_PLAYER].rot_y);
    }
    /* Wund-Helper `lbu aca59 / jalr 0x801201b8[aca59]` @0x8010a580-5b0 (Tabelle re15_damage.c
     * s_wound_helper = 0x8010a1cc / 0x8010a208 / 0x8010a244 / 0x8010a268, selbst disassembliert). */
    re15_wound_release_stamp(dir);
    kleid_log("abwurf", greifer_typ, dir);
}

void re15_kleidung_hundebiss(const re15_actor_t *hund, const re15_actor_t *spieler)
{
    if (!hund || !spieler) return;
    /* aca59 = FUN_8001a780(Spieler)+2 (@0x8010f310-324): a780 = ((Spieler.yaw - self.yaw + 0x400)
     * & 0xfff) < 0x800 (@0x8001a788-a4) -> 1 = gleichgerichtet (Ruecken zum Hund). */
    int gleich = ((((int32_t)spieler->rot_y - (int32_t)hund->rot_y) + 0x400) & 0xfff) < 0x800 ? 1 : 0;
    /* rng = FUN_8001af20 @0x8010f334 (Engine-Strom), `andi v0,v0,0x1` @0x8010f34c; panel =
     * byte[0x80121010 + aca59*2 + (rng&1)] (@0x8010f33c-5c), Bytes @0x80121014 = 05 07 04 06. */
    static const uint8_t panel[4] = { 5, 7, 4, 6 };     /* @0x80121014 */
    int idx = (gleich ? 2 : 0) + (re15_engine_rand8() & 1);
    re15_wound_add(panel[idx], 0x32);                   /* `ori a1,zero,0x32` @0x8010f364 */
    kleid_log("hundebiss", hund->type, idx);
}

void re15_kleidung_kraehe(const re15_actor_t *kraehe, const re15_actor_t *spieler)
{
    if (!kraehe || !spieler) return;
    /* +0x1ec = Spieler.y - Kraehe.y (@0x801124cc / @0x80114f40), gelesen `lh v1,492(v0)`
     * @0x801161f8; Baender `slti 1500` @0x80116200, `slti 3000` @0x80116220, `slti 3600`
     * @0x80116254. Hier LIVE aus denselben Groessen gebildet (PORT-MAPPING fuer die RE2-Kraehe). */
    int32_t vert = (int32_t)(int16_t)(spieler->y - kraehe->y);
    if      (vert < 1500) { re15_wound_add(1, 0x0a); re15_wound_add(2, 0x0a); }   /* @0x8011620c/14-1c/70 */
    else if (vert < 3000) { re15_wound_add(3, 0x0a); re15_wound_add(4, 0x28);     /* @0x8011622c/38 */
                            re15_wound_add(6, 0x28); re15_wound_add(5, 0x28); }   /* @0x80116244/50-70 */
    else if (vert < 3600) { re15_wound_add(0, 0x08); re15_wound_add(7, 0x28); }   /* @0x80116260/68-70 */
    /* >= 3600: nichts (beq @0x80116258 -> Epilog) */
    kleid_log("kraehe", kraehe->type, (int)vert);
}
