/* test_n2_griff_start.c — Riegel Nachbesserung 2 (Form, Zusatzpunkt Koordinator; Nebenbefund 1 der GR-Abnahme 2):
 * "START waehrend des Riesenform-Griffs -> Statusschirm, danach frieren Form und Leon ein".
 *
 * Original (Dossier analysis/befunde_runde41/form/stufe3/nachbesserung2_griff_start.md):
 *   RE1.5 PSX.EXE: `lw a0,0x800aca3c` @0x8001ccfc, `andi v0,a0,0x40` / `bne v0,zero,0x8001cdec` @0x8001cd04-08 springt ueber den
 *   ganzen START-Block, auch ueber die Vormerkung aca3c |= 0x8000 @0x8001cd80-90. Der Birkin-Spieler-Haken (STAGE5 0x8011afd8) setzt
 *   aca3c |= 0x40 beim Packen ([0] @0x8011b0b4/c0, [1] @0x8011b4a0/ac, [2] @0x8011b770/7c) und loescht es beim Loslassen
 *   ([0] @0x8011b3e0-ec, [1] @0x8011b6b8-c4, [2] @0x8011b95c-68). RE2: `lbu 0x800cfdcb` (Spieler+0x93) / `bne -> 0x80026540`
 *   @0x80026438-40 vor der START-Flanke. In beiden Spielen wird START im Griff VERWORFEN: kein Schirm, auch nicht danach.
 *
 * Echter re15_game_step in ROOM5090/5091 mit der Bahnsteig-Form (BAHNSTEIG_FORM.EMD + Opfer-Bank EM30 wie pc_enemy_load_bahnsteig):
 *   1. Griff erzwingen (Sub 15), bis der Haken packt (re15_bk_opfer_cmd).
 *   2. Riegel: START-Flanken in jeder Haken-Phase; der Schirm darf weder im Griff noch danach aufgehen.
 *   3. Kontrolle: START im ersten freien Bild nach dem Loslassen oeffnet den Schirm normal; Waffenwechsel im Schirm, START schliesst.
 *   4. Immer: Form verlaesst Clip 27 und tickt weiter, Spieler frei und laeuft, nichts bleibt eingefroren.
 * Gegenprobe ohne START: derselbe Ablauf endet ebenso. Faelle: Gangmitte (Haken [0] 12 Bilder) und Nordwand wie Lauf b5 der
 * GR-Abnahme (Rueckstoss gegen die Wand z -20340 -> Ringkampf, Haken 64 Bilder).
 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "re15_rdt.h"
#include "re15_scd.h"
#include "re15_actor.h"
#include "re15_aot.h"
#include "re15_room.h"
#include "re15_enemy.h"
#include "re15_enemy_ai.h"
#include "re15_ems.h"
#include "re15_emd.h"
#include "re15_collision.h"
#include "re15_msg.h"
#include "re15_game_step.h"
#include "re15_camera.h"
#include "re15_damage.h"
#include "re15_math.h"
#include "re15_player.h"
#include "re15_menu.h"
#include "re15_fade.h"
#include "re15_birkin_opfer.h"
#include "re15_inventory.h"
#include "re2_ems.h"

#ifndef RE15_ASSET_PSX_DIR
#define RE15_ASSET_PSX_DIR "shared_assets/PSX"
#endif
#define RE2_DIR RE15_ASSET_PSX_DIR "/../RE2"

int re15_bb_bank_da(const re15_actor_t *e);

static int fails = 0;
#define CHECK(cond, ...) do { if (!(cond)) { fprintf(stderr, "FAIL: " __VA_ARGS__); \
    fprintf(stderr, "\n"); fails++; } } while (0)

static re15_rdt_t         s_rdt;
static re15_camera_view_t s_cam;
static re15_game_ctx_t    s_ctx;
static int                s_shown = 0;
static uint16_t           s_pad_alt = 0;

static uint8_t *slurp(const char *p, size_t *n)
{
    FILE *f = fopen(p, "rb");
    if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    uint8_t *b = (uint8_t *)malloc((size_t)sz);
    if (b && fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); b = NULL; }
    fclose(f);
    if (b && n) *n = (size_t)sz;
    return b;
}

static re15_emd_animation_t s_pl00_anim; static re15_emd_skeleton_t s_pl00_skel;
static int pl00_laden(void)
{
    size_t esz = 0, rsz = 0;
    uint8_t *edd = slurp(RE15_ASSET_PSX_DIR "/PLD/PL00.EDD", &esz);
    uint8_t *emr = slurp(RE15_ASSET_PSX_DIR "/PLD/PL00.EMR", &rsz);
    if (!edd || !emr) return 0;
    if (re15_emd_parse_animation(edd, esz, &s_pl00_anim) != 0 || re15_emd_parse_skeleton(emr, rsz, &s_pl00_skel) != 0) return 0;
    re15_player_set_pl00_banks(&s_pl00_skel, &s_pl00_anim);
    return 1;
}

static uint8_t *s_emd, *s_em30;
static size_t s_emd_n, s_em30_n;

/* Bank wie pc_enemy_load_bahnsteig (und test_birkin_bahnsteig.c form_bank): Riesenform + Opfer-Bank von EM30. */
static int form_bank(void)
{
    re15_enemy_bank_t *eb = re15_enemy_find(0x30u);
    if (!eb) eb = re15_enemy_alloc(0x30u);
    if (!eb) return 0;
    { uint8_t typ = eb->type; memset(eb, 0, sizeof *eb); eb->type = typ; }
    if (re2_emd_parse_bank(s_emd, s_emd_n, eb) != 0) return 0;
    eb->re2_rig = 1; eb->ok = 1; eb->buf = NULL; eb->bb_form = 1;
    eb->victim_ok = re15_emd_parse_victim_bank(s_em30, s_em30_n, &eb->skel_victim, &eb->anim_victim) == 0;
    return eb->victim_ok;
}

static void bild(uint16_t pad)
{
    const unsigned char *raw; int len, id;
    scd_vm_tick();
    re15_actor_step_all_walkers();
    re15_msg_tick(&raw, &len, &id);
    if (re15_cam_present_tick()) s_shown = (int)g_scd.cam_id;
    s_ctx.active_cut  = s_shown;
    s_ctx.pad_current = pad;
    s_ctx.pad_pressed = (uint16_t)(pad & (uint16_t)~s_pad_alt);
    s_pad_alt = pad;
    re15_game_step(&s_ctx);
    re15_fade_tick();
}

static int raum_boot(const char *rel, unsigned id)
{
    size_t rsz = 0; char p[512];
    snprintf(p, sizeof p, RE15_ASSET_PSX_DIR "/%s", rel);
    uint8_t *raw = slurp(p, &rsz);
    if (!raw || re15_rdt_parse(raw, rsz, &s_rdt) != 0) return 0;
    memset(&s_cam, 0, sizeof s_cam);
    memset(&s_ctx, 0, sizeof s_ctx);
    s_ctx.rdt = &s_rdt; s_ctx.rdt_ok = 1; s_ctx.cam_view = &s_cam; s_ctx.active_cut = 11;
    s_ctx.pl00_skel = &s_pl00_skel; s_ctx.pl00_anim = &s_pl00_anim;
    re15_actor_init(); re15_aot_init(); scd_vm_init();
    re15_enemy_reset(); re15_enemy_ai_set_paused(0);
    re15_player_victim_reset();
    re15_bk_opfer_reset();
    re15_damage_seed_rng(0x0badf00du);
    g_current_room_id = (uint16_t)id; g_room_change.pending = 0;
    g_re15_pauseflags = 0;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100; pl->hit_react = 0;
    pl->state = 1; pl->motion = 0; pl->floor = 0;
    pl->x = 8000; pl->y = 0; pl->z = -23400; pl->rot_y = 2048;
    re15_collision_reset_band();
    re15_collision_set_band(0);
    return 1;
}

/* Ein Lauf. start: 0 = kein START, 1 = START im Griff (alle 7 Bilder, solange der Haken laeuft: jede Phase getroffen),
 * 2 = Kontrolle: START im fuenften freien Bild nach dem Loslassen (der Schirm muss dann normal aufgehen). waffe: im Schirm die
 * angelegte Waffe wechseln (Commit beim Schliessen @0x80046654-66c8, wie Lauf b5), danach mit START schliessen. */
static void lauf(const char *rel, unsigned id, int start, int32_t bx, int32_t bz, int32_t dx, int32_t dz, const char *wo, int waffe)
{
    static const char *namen[3] = { "ohne START", "START im Griff", "START nach dem Loslassen" };
    char art[128];
    snprintf(art, sizeof art, "%s, %s%s", namen[start], wo, waffe ? ", Waffenwechsel im Schirm" : "");
    if (!raum_boot(rel, id)) { CHECK(0, "%s nicht ladbar", rel); return; }
    if (!form_bank()) { CHECK(0, "Form-Bank"); return; }
    /* Inventar: Messer (Platz 0) angelegt, Pistole (Platz 1) zum Wechsel im Schirm */
    memset(&g_inv, 0, sizeof g_inv);
    g_inv.slots[0].id = 1; g_inv.slots[0].qty = 1; g_inv.slots[1].id = 2; g_inv.slots[1].qty = 15;
    re15_inv_set_equipped_slot(0);
    int bs = re15_actor_alloc(0x30u);
    CHECK(bs > 0, "kein Aktor");
    if (bs <= 0) return;
    re15_actor_t *b = &g_actors[bs], *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    b->active = 1; b->type = 0x30u; b->x = bx; b->y = 0; b->z = bz; b->rot_y = 0; b->hp = 150; b->em_flag_id = 0xFF;
    b->grid_id = 0x13; b->no_draw = 0;
    re15_enemy_apply_hitbox(b, 0x30u);
    CHECK(re15_bb_bank_da(b), "%s %s: Form-Bank nicht erkannt", rel, art);
    /* Spieler (dx,dz) neben der Form; der Griff dreht ihn selbst zur Form (bb_griff) */
    pl->x = b->x + dx; pl->z = b->z + dz;
    bild(0);                                           /* Eintritt des Ticks (s_bb init) */
    b->x = bx; b->z = bz; b->rot_y = 0;
    pl->x = b->x + dx; pl->z = b->z + dz; pl->hp = 100; pl->hit_react = 0;
    b->state = 1; b->sub_state_1 = 15; b->sub_state_2 = 0; b->sub_state_3 = 0;   /* BB_GRIFF */
    int f, gepackt = -1;
    for (f = 0; f < 40; f++) {
        bild(0);
        if (re15_bk_opfer_cmd()) { gepackt = f; break; }
    }
    CHECK(gepackt >= 0 && b->motion == 27, "%s %s: Griff packt nicht (Clip %d Bild %d, opfer %d)", rel, art, b->motion, b->anim_frame,
          re15_bk_opfer_cmd());
    if (gepackt < 0) return;
    int offen_im_griff = 0, offen_nach = 0, geschlossen = 0, auf_bild = -1, haken_los = -1, n_start = 0, start_frei = -1;
    for (f = 0; f < 500; f++) {
        const int im_griff = re15_bk_opfer_cmd() != 0 || re15_player_is_grabbed();
        const int offen = re15_menu_is_open() || re15_menu_stage() != 0;
        uint16_t pad = 0;
        if (offen && im_griff) offen_im_griff++;
        if (!re15_bk_opfer_cmd() && haken_los < 0) haken_los = f;      /* aca58 zurueck (loesen) */
        if (getenv("N2_PROT") && f < 120) {
            extern int re15_inv_open_allowed(void);
            fprintf(stderr, "  f%3d haken %d ph %3d gr %d frei %d hr %02x mo %3d ms %d form %d/%d\n", f, re15_bk_opfer_cmd(),
                    re15_bk_opfer_phase(), re15_player_is_grabbed(), re15_inv_open_allowed(), pl->hit_react, pl->motion,
                    re15_menu_stage(), b->motion, b->anim_frame);
        }
        if (start == 1 && re15_bk_opfer_cmd() && (f % 7) == 2) { pad = RE15_PAD_BIT_START; n_start++; }
        if (start == 2 && haken_los >= 0 && f == haken_los + 5 && !offen) { pad = RE15_PAD_BIT_START; n_start++; start_frei = f; }
        if (re15_menu_is_open() && re15_menu_phase() == 1) {
            if (auf_bild < 0) auf_bild = f;
            offen_nach++;
            if (offen_nach == 10 && waffe) re15_inv_set_equipped_slot(1);
            if (offen_nach == 20) { pad = RE15_PAD_BIT_START; geschlossen = 1; }
        }
        bild(pad);
        if (pad) bild(0);                              /* Flanke wieder loslassen */
        if (haken_los >= 0 && f > haken_los + 40 && !re15_menu_gameplay_frozen() && b->motion != 27 && (start != 2 || geschlossen)) break;
    }
    fprintf(stderr, "%s %s: gepackt Bild %d, START-Flanken %d, Haken los nach %d, Schirm auf nach %d (%d Bilder offen), offen im Griff %d, "
                    "danach Form Clip %d Bild %d Sub %d, Spieler st %d mo %d gr %d, Waffe %d, Menue eingefroren %d\n",
            rel, art, gepackt, n_start, haken_los, auf_bild, offen_nach, offen_im_griff, b->motion, b->anim_frame,
            b->sub_state_1, pl->state, pl->motion, re15_player_is_grabbed(), re15_player_equipped_weapon(), re15_menu_gameplay_frozen());
    CHECK(haken_los >= 0, "%s %s: Haken endet nicht", rel, art);
    CHECK(offen_im_griff == 0, "%s %s: Statusschirm im Griff offen (%d Bilder)", rel, art, offen_im_griff);
    if (start == 1) {
        CHECK(n_start >= 2, "%s %s: zu wenige START-Flanken im Griff (%d)", rel, art, n_start);
        /* Original: aca3c & 0x40 (Haken @0x8011b0b4-c0) -> START-Block @0x8001cd04-08 uebersprungen, keine Vormerkung (RE2 @0x80026438-40) */
        CHECK(auf_bild < 0, "%s %s: START im Griff vorgemerkt, Schirm ging %d Bilder nach dem Loslassen auf (Original verwirft: @0x8001cd04-08)",
              rel, art, auf_bild - haken_los);
    }
    if (start == 2) {
        CHECK(auf_bild >= 0 && geschlossen, "%s %s: Kontrolle: Schirm geht nach dem Loslassen nicht auf (START bei %d)", rel, art, start_frei);
        if (waffe) CHECK(re15_player_equipped_weapon() == 2, "%s %s: Waffenwechsel nicht uebernommen (Waffe %d)", rel, art,
                         re15_player_equipped_weapon());
    }
    CHECK(!re15_menu_gameplay_frozen(), "%s %s: Spiel bleibt eingefroren (Menue-Stufe %d)", rel, art, re15_menu_stage());
    /* Griff endet: Form verlaesst Clip 27 und tickt weiter (danach weit weg gestellt, damit sie nicht neu angreift) */
    {
        int weiter = 0, bild0 = b->anim_frame, clip0 = b->motion;
        for (f = 0; f < 200 && b->motion == 27; f++) bild(0);
        CHECK(b->motion != 27, "%s %s: Form haengt in Clip 27 (Bild %d)", rel, art, b->anim_frame);
        b->x = -12000; b->z = -23400; b->sub_state_1 = 3; b->sub_state_2 = 0; b->state = 1;
        for (f = 0; f < 200 && (re15_bk_opfer_cmd() || re15_player_is_grabbed() || re15_player_hit_flinch_active()); f++) { pl->hp = 100; bild(0); }
        for (f = 0; f < 30; f++) { const int a = b->anim_frame, m = b->motion; bild(0); if (b->anim_frame != a || b->motion != m) weiter++; }
        CHECK(weiter >= 20, "%s %s: Form tickt nicht weiter (%d von 30 Bildern, vorher Clip %d Bild %d)", rel, art, weiter, clip0, bild0);
    }
    /* Spieler frei und steuerbar: Form weit weg stellen, UP 40 Bilder je Richtung */
    CHECK(!re15_player_is_grabbed() && !re15_bk_opfer_cmd(), "%s %s: Spieler bleibt gepackt", rel, art);
    {
        b->x = -12000; b->z = -23400; b->sub_state_1 = 3; b->sub_state_2 = 0; b->state = 1;
        for (f = 0; f < 60 && re15_player_hit_flinch_active(); f++) { pl->hp = 100; bild(0); }
        const int32_t sx = pl->x, sz = pl->z;
        int best = 0;
        for (int k = 0; k < 4 && best < 300; k++) {
            pl->rot_y = (int16_t)(k * 1024);
            const int32_t ax = pl->x, az = pl->z;
            for (f = 0; f < 40; f++) { pl->hp = 100; bild(RE15_PAD_BIT_UP); }
            const int32_t w = abs((int)(pl->x - ax)) + abs((int)(pl->z - az));
            if (w > best) best = w;
        }
        CHECK(best >= 300, "%s %s: Spieler laeuft nicht (Weg %d ab (%d,%d), st %d mo %d)", rel, art, best, (int)sx, (int)sz, pl->state, pl->motion);
    }
    CHECK(!re15_menu_is_open() && re15_menu_stage() == 0, "%s %s: Schirm nach dem Lauf offen", rel, art);
}

int main(void)
{
    if (!pl00_laden()) { printf("FEHLT: PL00\n"); return 77; }
    s_emd = slurp(RE2_DIR "/BAHNSTEIG_FORM.EMD", &s_emd_n);
    {
        size_t esz = 0, off = 0, len = 0;
        uint8_t *ems = slurp(RE15_ASSET_PSX_DIR "/EMD/CDEMD0.EMS", &esz);
        if (ems && re15_ems_get_entry(ems, esz, re15_ems_index_for_type(0x30), &off, &len) == 0) {
            s_em30 = (uint8_t *)malloc(len); memcpy(s_em30, ems + off, len); s_em30_n = len;
        }
        free(ems);
    }
    if (!s_emd || !s_em30) { fprintf(stderr, "SKIP: BAHNSTEIG_FORM.EMD / CDEMD0.EMS fehlt\n"); return 77; }
    /* Riegel: START im Griff. Gegenprobe: ohne START. Kontrolle: START nach dem Loslassen (+ Waffenwechsel). */
    lauf("STAGE5/ROOM5090.RDT", 0x5090u, 1, 5000, -23400, 2600, 0, "Gangmitte", 0);
    lauf("STAGE5/ROOM5090.RDT", 0x5090u, 1, 10054, -24751, 0, 2600, "Nordwand", 0);
    lauf("STAGE5/ROOM5091.RDT", 0x5091u, 1, 10054, -24751, 0, 2600, "Nordwand", 0);
    lauf("STAGE5/ROOM5090.RDT", 0x5090u, 0, 5000, -23400, 2600, 0, "Gangmitte", 0);
    lauf("STAGE5/ROOM5090.RDT", 0x5090u, 0, 10054, -24751, 0, 2600, "Nordwand", 0);
    lauf("STAGE5/ROOM5090.RDT", 0x5090u, 2, 10054, -24751, 0, 2600, "Nordwand", 1);
    lauf("STAGE5/ROOM5091.RDT", 0x5091u, 2, 5000, -23400, 2600, 0, "Gangmitte", 1);
    if (fails) { fprintf(stderr, "test_n2_griff_start: %d FEHLER\n", fails); return 1; }
    fprintf(stderr, "test_n2_griff_start: OK\n");
    return 0;
}
