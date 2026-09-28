/* probe_r30_n_room5080.c — Runde 30, Nachschliff room5080: der ROOM5080-Birkin VOR und NACH der
 * Generator-Folge (Dossier analysis/befunde_runde30/nachschliff-room5080.md, Abschnitte 8.1 und 9).
 *
 * BEFUND (Echtlauf des Gegenpruefers): der Spieler steht im Westgang, die Generator-Folge ist NICHT
 * ausgeloest. Im Port lief Birkin (Typ 0x30, grid 0x33) ab der Raumladung von seiner Spawnlage
 * (-18100,-5600,200) los, ging durch die Nordwand in den Rauten-Block und biss den Spieler.
 *
 * ⛔ KORRIGIERT (Runde 30, Nachbesserung 2): die erste Fassung dieses Riegels verlangte, dass Birkin
 * vor der Folge LOSLAEUFT (EMERGENCE, >= 5000 Einheiten) und nur an der Nordwand GEKLEMMT wird —
 * "RE1.5 parkt nicht, sondern klemmt". FALSCH. Die Birkin-Wurzel friert bei grid & 0x20 ein und
 * erreicht weder den Dispatch noch die Klemme (selbst disassembliert, Dossier 9.1):
 *   STAGE5 @0x80116a7c lbu v0,9(a0) / @0x80116a84 andi v0,v0,0x20 / @0x80116a88 bne -> 0x80116eb8,
 *   dort nur jal 0x8001b064 (Schatten) @0x80116ecc und jr ra.
 * Sce_em_set ruft die Wurzel beim Spawn EINMAL mit geloeschtem Bit (@0x8004256c-@0x80042608):
 * INIT -> Zustand 1, Sub 9, +0x95 = 0x10 (STAGE3 @0x80116880/@0x80116890, STAGE5 +0x814).
 * Erst Member_set(0x0C,0x13) @ROOM5080 Datei 0x0083E (sub03) loescht das Bit.
 *
 * RIEGEL (Standard): ROOM5080 laden, SCD hochfahren (sub00 spawnt Birkin), der Spieler steht bei
 * (-23350,-18200) (die Lage aus dem Echtlauf).
 *  Teil 1 — 2400 Bilder OHNE Folge:
 *   A1 Birkin gespawnt wie der Record (grid 0x33, y -5600, floor 0).
 *   A2 in JEDEM Bild eingefroren: Lage (-18100,-5600,200), grid 0x33, Zustand 1 / Sub 9, Clip 0,
 *      +0x95 = 0x10 (der INIT lief schon beim Spawn).
 *   A3 er bewegt sich nicht (max. Abstand zur Spawnlage 0).
 *   B  nie in der Rohflaeche einer Band-0-Zelle, nie im Raum (z < -6654).
 *   C  Spieler-hp bleibt 100, kein Griff.
 *   D  die Folge lief nicht (Bank 5 Bit 32 / (3,48) bleiben 0).
 *  Teil 2 — die Folge (Generator-AOT Platz 1 @0x0072E, Ja auf msg 0):
 *   E1 die Freigabe kommt vom Member_set @0x0083E (grid 0x33 -> 0x13).
 *   E2 danach ZUERST Sub 9 EMERGENCE (Clip 0x10), erst dann WALK (Sub 1).
 *   E3 nach dem Fall steht er am Boden (y 0: Speed_set(1,400) @0x0082E x For 14 @0x00832).
 *   E4 auch nach der Freigabe nie in einer Band-0-Zelle.
 * Negativ-Kontrolle: vor dem Bau der Schranke ROT (A2/A3/B/C).
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
#include "re15_skeleton.h"
#include "re15_tim.h"
#include "re15_fade.h"
#include "re15_math.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef RE15_ASSET_PSX_DIR
#define RE15_ASSET_PSX_DIR "shared_assets/PSX"
#endif

static re15_rdt_t         s_rdt;
static re15_camera_view_t s_cam;
static re15_game_ctx_t    s_ctx;
static int                s_shown = 0;
static int                s_fail = 0;
static uint16_t           s_pad = 0;

#define PIN(cond, ...) do { if (!(cond)) { s_fail++; printf("  RIEGEL FEHLT: " __VA_ARGS__); printf("\n"); } \
                            else { printf("  RIEGEL ok: " __VA_ARGS__); printf("\n"); } } while (0)

static uint8_t *slurp(const char *p, size_t *n)
{
    FILE *f = fopen(p, "rb"); if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    if (sz <= 0) { fclose(f); return NULL; }
    uint8_t *b = (uint8_t *)malloc((size_t)sz);
    if (b && fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); b = NULL; }
    fclose(f); if (b) *n = (size_t)sz; return b;
}

/* EM030 wie zur Laufzeit (platform/pc/main.c pc_enemy_read_re15_emd): der Record aus dem
 * RE1.5-CDEMD0.EMS, damit Cliplaengen (EMERGENCE, Laufzyklus) die echten sind. */
static int load_em030(void)
{
    size_t n = 0, off = 0, len = 0;
    uint8_t *ems = slurp(RE15_ASSET_PSX_DIR "/EMD/CDEMD0.EMS", &n);
    if (!ems) return 0;
    int idx = re15_ems_index_for_type(0x30);
    if (idx < 0 || re15_ems_get_entry(ems, n, idx, &off, &len) != 0) return 0;
    re15_enemy_bank_t *eb = re15_enemy_find(0x30);
    if (!eb) eb = re15_enemy_alloc(0x30);
    if (!eb) return 0;
    re15_tim_t tim; memset(&tim, 0, sizeof tim);
    if (re15_emd_parse_container(ems + off, len, &eb->md1, &eb->skel, &eb->anim, &tim) != 0) return 0;
    eb->buf = NULL; eb->ok = 1;               /* aliast in ems (bleibt resident) */
    return 1;
}

static int s_cine_was_active = 0;
static void frame(void)
{
    const unsigned char *raw; int len, id;
    scd_vm_tick();
    re15_actor_step_all_walkers();
    {
        int cine_active = re15_game_flag_get(1, 27) || re15_game_flag_get(2, 7);
        re15_letterbox_tick(re15_game_flag_get(1, 27));
        if (cine_active) { g_scd.player_mode = 2; g_scd.letterbox_countdown = -1; }
        else if (s_cine_was_active) { g_scd.letterbox_countdown = 15; }
        s_cine_was_active = cine_active;
        if (g_scd.letterbox_countdown > 0 && --g_scd.letterbox_countdown == 0) {
            g_scd.player_mode = 0;
            re15_aot_settle_at(g_actors[RE15_ACTOR_SLOT_PLAYER].x,
                               g_actors[RE15_ACTOR_SLOT_PLAYER].z);
        }
    }
    re15_msg_tick(&raw, &len, &id);
    if (re15_cam_present_tick()) s_shown = (int)g_scd.cam_id;
    s_ctx.active_cut  = s_shown;
    s_ctx.pad_current = s_pad;
    s_ctx.pad_pressed = s_pad;
    re15_game_step(&s_ctx);
}

/* Rohflaeche einer Zelle (ohne Radius): steht der Birkin-Ursprung IN der Wand? */
static int in_zelle_band0(int32_t x, int32_t z, int *zelle)
{
    for (int i = 0; i < s_rdt.sca_count; i++) {
        const re15_sca_entry_t *c = &s_rdt.sca[i];
        if ((c->floor >> 4) != 0) continue;
        if (x >= c->x && x <= c->x + (int32_t)c->width &&
            z >= c->z && z <= c->z + (int32_t)c->density) { if (zelle) *zelle = i; return 1; }
    }
    return 0;
}

int main(int argc, char **argv)
{
    int nbilder = 2400, nfolge = 3000;
    for (int i = 1; i < argc; i++) {
        if (strncmp(argv[i], "n=", 2) == 0) nbilder = atoi(argv[i] + 2);
        else if (strncmp(argv[i], "folge=", 6) == 0) nfolge = atoi(argv[i] + 6);
    }

    size_t rsz = 0;
    uint8_t *raw = slurp(RE15_ASSET_PSX_DIR "/STAGE5/ROOM5080.RDT", &rsz);
    if (!raw) { printf("FEHLT: ROOM5080.RDT\n"); return 77; }
    if (re15_rdt_parse(raw, rsz, &s_rdt) != 0) { printf("FEHLT: RDT-Parse\n"); return 77; }

    printf("=== ROOM5080 — Birkin vor und nach der Generator-Folge (Frost-Schranke, Band +0x82) ===\n");
    printf("  @0x00746 Sce_em_set:");
    for (int i = 0; i < 14; i++) printf(" %02x", raw[0x746 + i]);
    printf("\n  -> Typ 0x%02x grid 0x%02x pc[4](+0x82)=%u y=%d\n", raw[0x748], raw[0x749], raw[0x74A],
           (int)(int16_t)(raw[0x750] | (raw[0x751] << 8)));
    printf("  @0x0083E Member_set: %02x %02x %02x %02x\n", raw[0x83E], raw[0x83F], raw[0x840], raw[0x841]);
    if (raw[0x746] != 0x44 || raw[0x748] != 0x30 || raw[0x749] != 0x33 || raw[0x74A] != 0 ||
        raw[0x83E] != 0x34 || raw[0x83F] != 0x0c || raw[0x840] != 0x13 || raw[0x841] != 0) {
        printf("FEHLT: Records nicht wie erwartet\n"); return 77;
    }

    memset(&s_cam, 0, sizeof s_cam);
    memset(&s_ctx, 0, sizeof s_ctx);
    s_ctx.rdt = &s_rdt; s_ctx.rdt_ok = 1; s_ctx.cam_view = &s_cam; s_ctx.active_cut = 4;

    re15_actor_init(); re15_aot_init(); scd_vm_init();
    re15_enemy_reset(); re15_enemy_ai_set_paused(0);
    re15_player_victim_reset();
    re15_damage_seed_rng(0x0badf00du);
    g_room_rdt = s_rdt; g_room_rdt_ok = 1;
    g_current_room_id = 0x5080; g_room_change.pending = 0;
    if (!load_em030()) printf("  (EM030-Bank fehlt — Cliplaengen fallen auf 40)\n");

    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100; pl->hit_react = 0;
    pl->state = 0; pl->motion = 0; pl->floor = 0;
    pl->x = -23350; pl->y = 0; pl->z = -18200; pl->rot_y = 2048;   /* Echtlauf-Standlage */
    re15_collision_set_band(0);

    re15_msg_load_room_block(s_rdt.messages, s_rdt.messages_size);
    scd_register_room_events(&s_rdt);
    scd_room_reenter(&s_rdt, pl->x, pl->z, /*entry cut*/ 4);
    g_scd.cut_auto_enabled = 1;
    s_shown = 4;

    /* ---- Teil 1: ohne Folge ---- */
    int bslot = -1, spawn_ok = 0, frost_verletzt = 0, frost_erstes = -1, wand_bilder = 0, raum_bilder = 0;
    int erste_wand = -1, wand_zelle = -1, hp_min = 100, griff = 0, folge = 0;
    int32_t sx = 0, sz = 0, zmin = 0x7fffffff, dmax = 0;
    for (int f = 0; f < nbilder; f++) {
        frame();
        if (bslot < 0) {
            for (int s = 1; s < RE15_ACTOR_MAX; s++)
                if (g_actors[s].active && g_actors[s].type == 0x30) { bslot = s; break; }
            if (bslot >= 0) {
                re15_actor_t *b = &g_actors[bslot];
                sx = b->x; sz = b->z;
                spawn_ok = (b->grid_id == 0x33 && b->y == -5600 && b->floor == 0);
                printf("  Bild %d: Birkin Slot %d grid 0x%02x pos=(%d,%d,%d) floor=%u st=%d sub=%d +0x95=0x%02x\n",
                       f, bslot, (unsigned)b->grid_id, (int)b->x, (int)b->y, (int)b->z, (unsigned)b->floor,
                       b->state, b->sub_state_1, (unsigned)b->anim_frame);
            }
        }
        if (bslot >= 0) {
            re15_actor_t *b = &g_actors[bslot];
            int eingefroren = (b->x == -18100 && b->y == -5600 && b->z == 200 && b->grid_id == 0x33 &&
                               b->state == 1 && b->sub_state_1 == 9 && b->motion == 0 && b->anim_frame == 0x10);
            if (!eingefroren) { if (frost_verletzt++ == 0) frost_erstes = f; }
            int32_t dx = b->x - sx, dz = b->z - sz;
            int32_t d = (int32_t)re15_squareroot0((uint32_t)(dx * dx + dz * dz));
            if (d > dmax) dmax = d;
            if (b->z < zmin) zmin = b->z;
            int zi = -1;
            if (in_zelle_band0(b->x, b->z, &zi)) {
                wand_bilder++;
                if (erste_wand < 0) { erste_wand = f; wand_zelle = zi; }
            }
            if (b->z < -6654) raum_bilder++;
            if ((f % 600) == 0 || f == nbilder - 1)
                printf("  Bild %4d: Birkin pos=(%d,%d,%d) grid 0x%02x st=%d sub %d clip %d | Spieler (%d,%d) hp %d\n",
                       f, (int)b->x, (int)b->y, (int)b->z, (unsigned)b->grid_id, b->state, b->sub_state_1,
                       (int)b->motion, (int)pl->x, (int)pl->z, (int)pl->hp);
        }
        if (pl->hp < hp_min) hp_min = pl->hp;
        if (re15_player_is_grabbed()) griff++;
        if (re15_game_flag_get(5, 32) || re15_game_flag_get(3, 48)) folge = 1;
    }
    printf("\n  Teil 1: Bilder nicht eingefroren %d (erstes %d), max. Abstand zur Spawnlage %d, kleinstes z %d,\n"
           "          Bilder in Band-0-Zelle %d (erstes %d, Zelle #%d), Bilder im Raum (z < -6654) %d\n",
           frost_verletzt, frost_erstes, (int)dmax, (int)zmin, wand_bilder, erste_wand, wand_zelle, raum_bilder);
    printf("  Spieler: kleinstes hp %d, Griff-Bilder %d; Folge gelaufen %d\n\n", hp_min, griff, folge);

    PIN(bslot >= 0 && spawn_ok, "A1 Birkin gespawnt wie der Record (grid 0x33, y -5600, floor 0)");
    PIN(bslot >= 0 && frost_verletzt == 0,
        "A2 in jedem Bild eingefroren: (-18100,-5600,200), grid 0x33, Zustand 1/Sub 9, +0x95 0x10 (%d Bilder verletzt)",
        frost_verletzt);
    PIN(dmax == 0, "A3 er bewegt sich nicht (max. Abstand %d)", (int)dmax);
    PIN(wand_bilder == 0, "B1 nie in der Rohflaeche einer Band-0-Zelle (%d Bilder)", wand_bilder);
    PIN(raum_bilder == 0, "B2 nie im Raum, z < -6654 (%d Bilder)", raum_bilder);
    PIN(hp_min == 100 && griff == 0, "C  Spieler unversehrt (hp min %d, Griff %d)", hp_min, griff);
    PIN(!folge, "D  Generator-Folge nicht gelaufen (Bank 5 Bit 32 / (3,48) = 0)");

    /* ---- Teil 2: die Folge ---- */
    if (bslot < 0) { printf("\n  ERGEBNIS: ROT (kein Birkin)\n"); return 1; }
    re15_actor_t *b = &g_actors[bslot];
    pl->x = -26900; pl->z = -18050;                    /* Mitte des Generator-Rechtecks @0x0072E */
    re15_aot_fire_slot(1);                             /* Aot_set Platz 1 -> sub02 */
    int frei_bild = -1, frei_ok = 0, emerg_bild = -1, walk_bild = -1, wand2 = 0;
    int32_t y_nach = 0x7fffffff;
    /* N1-Messung (Abschnitt 7/9.6, NICHT gebaut): Griff-Laeufe nach der Freigabe, Spieler ohne Eingabe */
    int griff_an = 0, griff_start = -1, erste_hp = -1, nlauf = 0;
    int lauf_start[8], lauf_len[8], frei_len[8];
    uint8_t grid_vor = b->grid_id;
    const uint8_t *ziel = raw + 0x83E;
    for (int f = 0; f < nfolge; f++) {
        const uint8_t *pc_vor[SCD_THREAD_COUNT]; uint8_t akt_vor[SCD_THREAD_COUNT];
        for (int i = 0; i < SCD_THREAD_COUNT; i++) { pc_vor[i] = g_scd.threads[i].pc; akt_vor[i] = g_scd.threads[i].active; }
        s_pad = (g_scd.message_active && ((f & 7) == 0)) ? 0x8000 : 0;   /* Quadrat = Bestaetigen */
        frame();
        if (frei_bild < 0 && (grid_vor & 0x20) && !(b->grid_id & 0x20)) {
            frei_bild = f;
            for (int i = 0; i < SCD_THREAD_COUNT; i++)
                if (akt_vor[i] && pc_vor[i] && pc_vor[i] <= ziel &&
                    (ziel < g_scd.threads[i].pc || !g_scd.threads[i].active) && ziel < pc_vor[i] + 0x100)
                    frei_ok = (b->grid_id == 0x13);
            printf("  Folge Bild %d: Freigabe grid 0x%02x -> 0x%02x, Birkin (%d,%d,%d) st=%d sub=%d\n", f,
                   (unsigned)grid_vor, (unsigned)b->grid_id, (int)b->x, (int)b->y, (int)b->z, b->state, b->sub_state_1);
        }
        grid_vor = b->grid_id;
        if (frei_bild >= 0) {
            if (emerg_bild < 0 && b->state == 1 && b->sub_state_1 == 9 && b->motion == 0x10) emerg_bild = f;
            if (walk_bild < 0 && b->state == 1 && b->sub_state_1 == 1) walk_bild = f;
            if (f == frei_bild + 5) y_nach = b->y;
            if (in_zelle_band0(b->x, b->z, NULL)) wand2++;
            if (erste_hp < 0 && pl->hp < 100) erste_hp = f;
            int g = re15_player_is_grabbed();
            if (g && !griff_an) {
                if (nlauf > 0 && nlauf <= 8) frei_len[nlauf - 1] = f - (lauf_start[nlauf - 1] + lauf_len[nlauf - 1]);
                griff_start = f; griff_an = 1;
            } else if (!g && griff_an) {
                if (nlauf < 8) { lauf_start[nlauf] = griff_start; lauf_len[nlauf] = f - griff_start; frei_len[nlauf] = -1; }
                nlauf++; griff_an = 0;
            }
        }
    }
    printf("  Folge: Freigabe Bild %d, EMERGENCE Bild %d, WALK Bild %d, y nach dem Fall %d, Band-0-Zellbilder %d\n\n",
           frei_bild, emerg_bild, walk_bild, (int)y_nach, wand2);
    printf("  N1-Messung (Spieler ohne Eingabe): erster Treffer Bild %d, Griff-Laeufe %d%s", erste_hp, nlauf,
           griff_an ? " (letzter Lauf dauert am Ende noch an)" : "");
    for (int i = 0; i < nlauf && i < 8; i++)
        printf("%s[Bild %d: %d Bilder gegriffen, danach %d frei]", i ? " " : ": ", lauf_start[i], lauf_len[i], frei_len[i]);
    printf("\n\n");
    PIN(frei_bild >= 0 && frei_ok, "E1 Freigabe durch Member_set @0x0083E (grid 0x33 -> 0x13)");
    PIN(emerg_bild >= 0 && walk_bild > emerg_bild, "E2 zuerst EMERGENCE (Bild %d), dann WALK (Bild %d)",
        emerg_bild, walk_bild);
    PIN(y_nach == 0, "E3 nach dem Fall am Boden (y %d)", (int)y_nach);
    PIN(wand2 == 0, "E4 nach der Freigabe nie in einer Band-0-Zelle (%d Bilder)", wand2);
    printf("\n  ERGEBNIS: %s (%d Riegel verletzt)\n", s_fail ? "ROT" : "GRUEN", s_fail);
    return s_fail ? 1 : 0;
}
