/* probe_r30_n_room5080.c — Runde 30, Nachschliff room5080: der ROOM5080-Birkin VOR der
 * Generator-Folge (Dossier analysis/befunde_runde30/nachschliff-room5080.md, Abschnitt 8.1).
 *
 * BEFUND (Echtlauf des Gegenpruefers, vom Agenten bitgleich nachgefahren): der Spieler steht im
 * Westgang, die Generator-Folge ist NICHT ausgeloest. Birkin (Typ 0x30, grid 0x33) laeuft ab der
 * Raumladung von seiner Spawnlage (-18100,-5600,200) los, geht durch die Nordwand SCA @0x00320
 * in den Rauten-Block SCA @0x003C8 und beisst den Spieler (hp 100 -> 10).
 *
 * ORIGINAL-MECHANISMUS (selbst disassembliert):
 *   - Birkin-Wurzel STAGE5 @0x80116cc0-cdc: FUN_8003b0a4(entity+0x34, box[+6], Maske 4), jedes Bild.
 *   - FUN_8003b0a4 nimmt das Band aus +0x82: @0x8003b234 `lbu v1,130(a3)`,
 *     @0x8003b23c `bne v1,v0` gegen floor>>4 der Zelle.
 *   - +0x82 = Spawn-Byte pc[4] (Sce_em_set @0x800421c8 `lbu v0,2(s2)`, @0x800421d0
 *     `sb v0,130(s0)`); ROOM5080 Datei 0x0074A = 0x00 -> Band 0. Alle SCA-Zellen des Raums
 *     tragen floor 3 -> Band 0.
 *   - Die Sperre +0x0&8 (@0x8003b12c-13c) setzt Birkins INIT nur fuer grid&0xf==1
 *     (STAGE5 @0x801170e4); grid 0x33 hat Nibble 3.
 *   => Im Original klemmt die Nordwand den Birkin (y bleibt -5600, das Modul schreibt +0x38
 *      und +0x82 nie). Der Port nahm das Band aus der Hoehe: band_from_y(-5600) = 3 -> keine Zelle.
 *
 * RIEGEL (Modus "riegel", Standard): ROOM5080 laden, SCD hochfahren (sub00 spawnt Birkin),
 * Spieler steht bei (-23350,-18200) (die Lage aus dem Echtlauf), 2400 Bilder ohne Eingabe.
 *   A  Abdeckung: Birkin gespawnt (Typ 0x30, grid 0x33, y -5600, floor 0), EMERGENCE gelaufen
 *      (grid 0x30), er hat sich >= 5000 von der Spawnlage entfernt und kam der Wand bis auf
 *      1100 nahe (z <= -5554) — sonst prueft der Riegel nichts.
 *   B  Birkin steht in KEINEM Bild in der Rohflaeche einer Band-0-Zelle und nie im Raum
 *      (z < -6654 = Suedkante der Nordwand @0x00320).
 *   C  Spieler-hp bleibt 100, kein Griff.
 *   D  Die Folge lief nicht (Bank 5 Bit 32 = Plc_dest-Ankunft bleibt 0, (3,48) bleibt 0).
 * Vor dem Bau: ROT (B und C).
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
    s_ctx.pad_current = 0;
    s_ctx.pad_pressed = 0;
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
    int nbilder = 2400;
    for (int i = 1; i < argc; i++)
        if (strncmp(argv[i], "n=", 2) == 0) nbilder = atoi(argv[i] + 2);

    size_t rsz = 0;
    uint8_t *raw = slurp(RE15_ASSET_PSX_DIR "/STAGE5/ROOM5080.RDT", &rsz);
    if (!raw) { printf("FEHLT: ROOM5080.RDT\n"); return 77; }
    if (re15_rdt_parse(raw, rsz, &s_rdt) != 0) { printf("FEHLT: RDT-Parse\n"); return 77; }

    printf("=== ROOM5080 — Birkin vor der Generator-Folge (Wandklemme Band +0x82) ===\n");
    printf("  @0x00746 Sce_em_set:");
    for (int i = 0; i < 14; i++) printf(" %02x", raw[0x746 + i]);
    printf("\n  -> Typ 0x%02x grid 0x%02x pc[4](+0x82)=%u y=%d\n", raw[0x748], raw[0x749], raw[0x74A],
           (int)(int16_t)(raw[0x750] | (raw[0x751] << 8)));
    if (raw[0x746] != 0x44 || raw[0x748] != 0x30 || raw[0x749] != 0x33 || raw[0x74A] != 0) {
        printf("FEHLT: Spawn-Record nicht wie erwartet\n"); return 77;
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

    int bslot = -1, spawn_ok = 0, emerg_ok = 0, wand_bilder = 0, raum_bilder = 0, erste_wand = -1;
    int wand_zelle = -1, hp_min = 100, griff = 0, folge = 0;
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
                printf("  Bild %d: Birkin Slot %d grid 0x%02x pos=(%d,%d,%d) floor=%u\n", f, bslot,
                       (unsigned)b->grid_id, (int)b->x, (int)b->y, (int)b->z, (unsigned)b->floor);
            }
        }
        if (bslot >= 0) {
            re15_actor_t *b = &g_actors[bslot];
            if (b->grid_id == 0x30) emerg_ok = 1;
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
            if ((f % 300) == 0 || f == nbilder - 1)
                printf("  Bild %4d: Birkin pos=(%d,%d,%d) grid 0x%02x sub %d | Spieler (%d,%d) hp %d\n",
                       f, (int)b->x, (int)b->y, (int)b->z, (unsigned)b->grid_id, b->sub_state_1,
                       (int)pl->x, (int)pl->z, (int)pl->hp);
        }
        if (pl->hp < hp_min) hp_min = pl->hp;
        if (re15_player_is_grabbed()) griff++;
        if (re15_game_flag_get(5, 32) || re15_game_flag_get(3, 48)) folge = 1;
    }
    printf("\n  Birkin: max. Abstand zur Spawnlage %d, kleinstes z %d, Bilder in Band-0-Zelle %d "
           "(erstes %d, Zelle #%d), Bilder im Raum (z < -6654) %d\n",
           (int)dmax, (int)zmin, wand_bilder, erste_wand, wand_zelle, raum_bilder);
    printf("  Spieler: kleinstes hp %d, Griff-Bilder %d; Folge gelaufen %d\n\n", hp_min, griff, folge);

    PIN(bslot >= 0 && spawn_ok, "A1 Birkin gespawnt wie der Record (grid 0x33, y -5600, floor 0)");
    PIN(emerg_ok, "A2 EMERGENCE gelaufen (grid 0x33 -> 0x30, grid&=0xfc)");
    PIN(dmax >= 5000 && zmin <= -5554,
        "A3 Birkin laeuft los und erreicht die Nordwand (Abstand %d >= 5000, z %d <= -5554)",
        (int)dmax, (int)zmin);
    PIN(wand_bilder == 0, "B1 nie in der Rohflaeche einer Band-0-Zelle (%d Bilder)", wand_bilder);
    PIN(raum_bilder == 0, "B2 nie im Raum, z < -6654 (%d Bilder)", raum_bilder);
    PIN(hp_min == 100 && griff == 0, "C  Spieler unversehrt (hp min %d, Griff %d)", hp_min, griff);
    PIN(!folge, "D  Generator-Folge nicht gelaufen (Bank 5 Bit 32 / (3,48) = 0)");
    printf("\n  ERGEBNIS: %s (%d Riegel verletzt)\n", s_fail ? "ROT" : "GRUEN", s_fail);
    return s_fail ? 1 : 0;
}
