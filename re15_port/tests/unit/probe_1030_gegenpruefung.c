/* probe_1030_gegenpruefung.c — UNABHAENGIGE Nachmessung des Befundes
 * "room1030-fall: Nav-Tuer und SCA-Tuer nicht deckungsgleich, es fehlen 29 Einheiten".
 * Misst SELBST: (a) die SCA-u0-Bytes VOR und NACH dem Raum-Skript, (b) die Nav-Kreuzung
 * 0->9, (c) das tatsaechlich befahrbare x-Fenster per echtem Klemmpfad (kein Nachbau). */
#include "re15_rdt.h"
#include "re15_scd.h"
#include "re15_actor.h"
#include "re15_aot.h"
#include "re15_room.h"
#include "re15_enemy_ai.h"
#include "re15_enemy.h"
#include "re15_ai_flavor.h"
#include "re15_player.h"
#include "re15_damage.h"
#include "re15_camera.h"
#include "re15_game_step.h"
#include "re15_collision.h"
#include "re15_inventory.h"
#include "re15_msg.h"
#include "re15_emd.h"
#include "re15_ems.h"
#include "re2_ems.h"

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

static uint8_t *slurp(const char *p, size_t *n)
{
    FILE *f = fopen(p, "rb"); if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    if (sz <= 0) { fclose(f); return NULL; }
    uint8_t *b = (uint8_t *)malloc((size_t)sz);
    if (b && fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); b = NULL; }
    fclose(f); if (b) *n = (size_t)sz; return b;
}

static uint8_t *s_re2_ems = NULL; static size_t s_re2_n = 0;
static int load_bank_re2(uint8_t type)
{
    if (!s_re2_ems) s_re2_ems = slurp(RE15_ASSET_PSX_DIR "/../RE2/CDEMD0.EMS", &s_re2_n);
    if (!s_re2_ems) return 0;
    re15_enemy_bank_t *eb = re15_enemy_find(type);
    if (eb && eb->ok) return 1;
    if (!eb) eb = re15_enemy_alloc(type);
    if (!eb) return 0;
    if (re2_ems_load_bank(s_re2_ems, s_re2_n, (int)type, eb, NULL) == 0) {
        eb->buf = NULL; eb->ok = 1; return 1;
    }
    eb->type = 0; return 0;
}

static void dump_rgn3(const char *tag)
{
    int base = 0; for (int i = 0; i < 3; i++) base += g_room_rdt.sca_rgn[i];
    printf("  [%s] rgn3 = flach %d..%d\n", tag, base, base + g_room_rdt.sca_rgn[3] - 1);
    for (int i = base; i < base + g_room_rdt.sca_rgn[3]; i++) {
        const re15_sca_entry_t *e = &g_room_rdt.sca[i];
        printf("    #%02d typ=%d u0=0x%02X u1=0x%02X floor=0x%02X band=%d  x[%d..%d] z[%d..%d]"
               "  m1=%d m4=%d m8=%d\n",
               i, e->type, e->u0, e->u1, e->floor, e->floor >> 4,
               (int)e->x, (int)e->x + (int)e->width,
               (int)e->z, (int)e->z + (int)e->density,
               (1u & e->u0) ? 1 : 0, (4u & e->u0) ? 1 : 0, (8u & e->u0) ? 1 : 0);
    }
}

/* Freies x-Fenster bei festem z: der Punkt gilt als FREI, wenn der echte Klemmpfad
 * ihn nicht verschiebt UND keine Zelle die Broadphase besteht. */
static void sweep(int32_t z, int32_t r, unsigned mask, int band, const char *tag)
{
    int32_t lo = 0, hi = 0; int have = 0, runs = 0;
    printf("  [%s] z=%ld r=%ld mask=%u band=%d ->", tag, (long)z, (long)r, mask, band);
    for (int32_t x = -16500; x <= -8500; x++) {
        int32_t nx = x, nz = z;
        int hit = re15_collision_constrain_contact_band(&g_room_rdt, x, z, &nx, &nz,
                                                        r, band, mask, NULL, NULL);
        int free_here = (!hit && nx == x && nz == z);
        if (free_here) { if (!have) { lo = x; have = 1; } hi = x; }
        else if (have) { printf(" [%ld..%ld]", (long)lo, (long)hi); have = 0; runs++; }
    }
    if (have) { printf(" [%ld..%ld]", (long)lo, (long)hi); runs++; }
    if (!runs) printf(" (kein freies x)");
    printf("\n");
}

int main(void)
{
    printf("== ROOM1030 GEGENPRUEFUNG ==\n");
    size_t n = 0;
    uint8_t *buf = slurp(RE15_ASSET_PSX_DIR "/STAGE1/ROOM1030.RDT", &n);
    if (!buf) { printf("FAIL: RDT nicht lesbar\n"); return 1; }
    if (re15_rdt_parse(buf, n, &g_room_rdt) != 0) { printf("FAIL: Parse\n"); return 1; }
    g_room_rdt_ok = 1; s_rdt = g_room_rdt;

    printf("\n-- SCA rgn-Zaehler: %d/%d/%d/%d/%d  Decke (%d,%d) --\n",
           g_room_rdt.sca_rgn[0], g_room_rdt.sca_rgn[1], g_room_rdt.sca_rgn[2],
           g_room_rdt.sca_rgn[3], g_room_rdt.sca_rgn[4],
           (int)(int16_t)g_room_rdt.ceiling_x, (int)(int16_t)g_room_rdt.ceiling_z);

    printf("\n-- A) SCA VOR dem Raum-Skript (reine Datei-Bytes) --\n");
    dump_rgn3("Datei");

    printf("\n-- B) Nav-Zonen (BLK), %d Knoten --\n", g_room_rdt.block_count);
    for (int i = 0; i < g_room_rdt.block_count; i++) {
        const uint8_t *nd = g_room_rdt.blocks + i * 12;
        printf("    Zone %2d x[%d..%d] z[%d..%d] links=0x%04X\n", i,
               (int)(int16_t)(nd[0] | (nd[1] << 8)), (int)(int16_t)(nd[4] | (nd[5] << 8)),
               (int)(int16_t)(nd[2] | (nd[3] << 8)), (int)(int16_t)(nd[6] | (nd[7] << 8)),
               (unsigned)(nd[10] | (nd[11] << 8)));
    }

    /* Raum hochfahren, damit die SCD-Setzer (Sca_id_set 0x37) wirklich laufen. */
    memset(&s_cam, 0, sizeof s_cam); memset(&s_ctx, 0, sizeof s_ctx);
    s_ctx.rdt = &s_rdt; s_ctx.rdt_ok = 1; s_ctx.cam_view = &s_cam; s_ctx.active_cut = 0;
    re15_ai_flavor_set(RE15_AI_FLAVOR_RE2);
    re15_actor_init(); re15_aot_init(); scd_vm_init();
    re15_enemy_reset(); re15_enemy_ai_set_paused(0);
    re15_player_cmd_reset();
    re15_damage_seed_rng(0x2545f491u);
    g_current_room_id = 0x1030;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100; pl->state = 0; pl->floor = 0; pl->y = 0;
    pl->x = -18050; pl->z = -8300;
    re15_collision_set_band(0);
    re15_inv_load_briefing();
    scd_room_reenter(&s_rdt, pl->x, pl->z, 0);
    (void)load_bank_re2(0x10); (void)load_bank_re2(0x11); (void)load_bank_re2(0x16);
    int f28 = -1, f30 = -1;
    for (int f = 0; f < 6000; f++) {
        const unsigned char *raw; int len, id;
        re15_msg_tick(&raw, &len, &id);
        s_ctx.pad_current = 0; s_ctx.pad_pressed = 0;
        scd_vm_tick();
        re15_game_step(&s_ctx);
        if (f28 < 0 && g_room_rdt.sca[28].u0 != 0xFF) f28 = f;
        if (f30 < 0 && g_room_rdt.sca[30].u0 != 0xFF) f30 = f;
    }
    printf("\n-- C) SCA NACH 6000 Frames Raum-Skript (u0(#28) geaendert ab Frame %d, "
           "u0(#30) ab Frame %d) --\n", f28, f30);
    dump_rgn3("Laufzeit");

    /* Welche Gegner stehen da, mit welchem Radius/welcher Maske? */
    printf("\n-- C2) Aktoren --\n");
    for (int s = 1; s < RE15_ACTOR_MAX; s++) {
        const re15_actor_t *e = &g_actors[s];
        if (!e->active) continue;
        printf("    Slot %d typ=0x%02X r_min=%d floor=%d sca_mask=%u pos=(%ld,%ld)\n",
               s, e->type, (int)e->hit_radius_min, (int)e->floor, (unsigned)e->sca_mask,
               (long)e->x, (long)e->z);
    }

    printf("\n-- D) Nav-Kreuzung Zone 0 -> Zone 9 (FUN_8003a31c-Arithmetik) --\n");
    {
        const uint8_t *n0 = g_room_rdt.blocks + 0 * 12, *n9 = g_room_rdt.blocks + 9 * 12;
        int ax1 = (int16_t)(n0[0] | (n0[1] << 8)), ax2 = (int16_t)(n0[4] | (n0[5] << 8));
        int az1 = (int16_t)(n0[2] | (n0[3] << 8)), az2 = (int16_t)(n0[6] | (n0[7] << 8));
        int bx1 = (int16_t)(n9[0] | (n9[1] << 8)), bx2 = (int16_t)(n9[4] | (n9[5] << 8));
        int bz1 = (int16_t)(n9[2] | (n9[3] << 8)), bz2 = (int16_t)(n9[6] | (n9[7] << 8));
        int zadj = (az1 == bz2 || az2 == bz1);
        int lo = ax1 > bx1 ? ax1 : bx1, hi = ax2 < bx2 ? ax2 : bx2;
        int mid = (lo + hi) >> 1;
        int cz = (az1 == bz2) ? az1 : az2;
        printf("    Z-benachbart=%d  lo=%d hi=%d  Mitte=%d  cz=%d  (az1=%d az2=%d bz1=%d bz2=%d)\n",
               zadj, lo, hi, mid, cz, az1, az2, bz1, bz2);
        printf("    Vorzugsfenster (2*r=800): (%d .. %d)\n", lo + 800, hi - 800);
    }

    printf("\n-- E) Befahrbares x-Fenster (echter Klemmpfad) --\n");
    sweep(-23860, 400, 8u, 0, "Kriecher mask8 @cz     ");
    sweep(-23640, 400, 8u, 0, "Kriecher mask8 @Z9Mitte");
    sweep(-23860, 400, 4u, 0, "aufrecht mask4 @cz     ");
    sweep(-23860, 450, 1u, 0, "Spieler  mask1 r450    ");

    {
        const re15_sca_entry_t *c30 = &g_room_rdt.sca[30];
        const re15_sca_entry_t *c31 = &g_room_rdt.sca[31];
        int limit = (int)c30->x + (int)c30->width + 400;
        int east  = (int)c31->x - 400;
        const uint8_t *n0 = g_room_rdt.blocks + 0 * 12, *n9 = g_room_rdt.blocks + 9 * 12;
        int ax1 = (int16_t)(n0[0] | (n0[1] << 8)), ax2 = (int16_t)(n0[4] | (n0[5] << 8));
        int bx1 = (int16_t)(n9[0] | (n9[1] << 8)), bx2 = (int16_t)(n9[4] | (n9[5] << 8));
        int lo = ax1 > bx1 ? ax1 : bx1, hi = ax2 < bx2 ? ax2 : bx2;
        int mid = (lo + hi) >> 1;
        printf("\n-- F) Fehlbetrag --\n");
        printf("    #30-Ostkante+r = %d, #31-Westkante-r = %d, Nav-Ziel = %d, Differenz = %d\n",
               limit, east, mid, limit - mid);
        printf("    Nav-Tuer Breite = %d, SCA-Tuer Breite = %d\n",
               bx2 - bx1, (int)c31->x - ((int)c30->x + (int)c30->width));
        printf("    Ueberlapp Nav/SCA = [%d..%d], davon nutzbar [%d..%d] = %d\n",
               (int)c30->x + (int)c30->width > bx1 ? (int)c30->x + (int)c30->width : bx1,
               hi, limit, hi, hi - limit);
    }

    free(buf); g_room_rdt_ok = 0;
    return 0;
}
