/* probe_sca_contact90.c — PIN: entity+0x90, das KONTAKT-BYTE des SCA-Klemmpfads.
 *
 * ============================================================================================
 * DER SCHREIBER — FUN_8003b0a4 (der SCA-Resolver), Disasm aus info/Re1.5/PSX.EXE
 * ============================================================================================
 * CLEAR, einmal pro Aufruf, VOR der Zellenschleife (@0x8003b1d0-ec):
 *      8003b1d0  lbu v1,144(a0)        ; a0 = g_entity(cur)
 *      8003b1d8  andi v1,v1,0xf0       ; obere Nibble BEHALTEN
 *      8003b1dc  sb  v1,144(a0)        ; entity+0x90 &= 0xf0     (STORE BYTE)
 *      8003b1ec  sw  zero,436(v1)      ; entity+0x1b4 = 0
 * WRITE, bei JEDEM Broadphase-Treffer, NACH dem Push-Handler (@0x8003b4b0-ec):
 *      8003b4b8  jal 0x8001bf04        ; a0 = Positions-Vektor NACH dem Push, a2 = Zelle
 *      8003b4c0  sra v0,v0,4
 *      8003b4c4  addiu v0,v0,8
 *      8003b4c8  lbu v1,0(s2)          ; s2 = cell+0x0a -> u1
 *      8003b4d4  andi v1,v1,0x3
 *      8003b4d8  addu v0,v0,v1
 *      8003b4dc  sb  v0,144(a0)        ; entity+0x90 = (dir>>4) + 8 + (u1 & 3)
 *      8003b4ec  sw  s4,436(v0)        ; entity+0x1b4 = &cell
 * Kein `break` in der Schleife -> die LETZTE treffende Zelle gewinnt; der Schreiber feuert
 * auch dann, wenn der Push die Position gar nicht bewegt hat.
 *
 * FUN_8001bf04 (@0x8001bf04) liefert NUR {0x000,0x400,0x800,0xc00,0xf00}:
 *      cx = signbit(px-(cell.x+cell.width))<<1 | signbit(px-cell.x)
 *      cx==3 (links)  -> 0x000 | cx==0 (rechts) -> 0x800
 *      cx==2 (X drin) -> cz==3 (vor cell.z) 0xc00 | cz==0 (dahinter) 0x400 | sonst 0xf00
 * => Bits 4-7 von +0x90 = Richtungs-Nibble, Bit 3 = "geklemmt", Bits 0-1 = cell.u1 & 3
 *    (8 & 3 == 0, also ist cell.u1 die EINZIGE Quelle der beiden unteren Bits).
 *
 * ============================================================================================
 * DIE LESER (14 Fundstellen, alle disassembliert)
 * ============================================================================================
 *  Zombie  @0x8010206c (decide[2] FUN_80102058), @0x80102f30 (decide[8]), @0x80105630
 *          (decide[0x13]):  `andi v0,a0,0x3` != 0 UND
 *          `(((+0x90&0xf0)<<4) - rot_y + 512) & 0xfff < 1024`
 *          -> `sw` von 0x901 bzw. 0xA01 nach entity+0x4 (Bit 0 waehlt 9 oder 10);
 *          das `sw` nullt dabei +0x6/+0x7 mit.
 *  Zombie  @0x80103478/@0x80103518 (animate[0xa] FUN_801033c8; decide[0xa] @0x801033c0 ist
 *          `jr ra`): Dreh-Schritt FUN_8001aa68((+0x90&0xf0)<<4, 0x10) bzw. Yaw-SNAP.
 *  ZGirl   @0x8010bca8/@0x8010bd58 — dieselben zwei in ihrem eigenen Fall-Zustand.
 *  Hund    @0x8010e260/@0x8010e634 (Gate wie Zombie), @0x8010e2b4/@0x8010e688
 *          (`+0x1e8 = +0x90 & 1` -> sub 13 bzw. 14), @0x80110384 (Dreh-Schritt),
 *          @0x8011040c (Yaw-Snap), @0x80110650 (`+0x9f = +0x90`).
 *  Dazu der Zeiger +0x1b4: `lw v0,436(a0)` / `lhu v0,10(v0)` / `sh v0,490(a0)`
 *          @0x8010e294-2a4 und @0x8010e668-78 (Hund, +0x1ea).
 *
 * ============================================================================================
 * WIRKUNGSBEREICH (selbst gemessen: 240 RDTs, 20874 SCA-Zellen)
 * ============================================================================================
 *  u1-Histogramm {0:19692, 1:764, 2:346, 6:30, 14:30, 16:4, 32:8}; `u1 & 0x80` = 0 in ALLEN
 *  Zellen (der zweite Zweig @0x8003b25c-60 ist beweisbar tot).
 *  `u1 & 3` ist in 40 RDTs (20 Raeume) gesetzt. ROOM1140/1030/1190/1230 haben u1 == 0 ->
 *  dort darf sich NICHTS aendern (Negativ-Kontrolle unten).
 *  In ROOM10B0 ist die einzige gegner-sichtbare Zelle (u0 & 4) die #6/#38/#70/#102/#134:
 *  typ 1, x[-6992..-350], z[-10126..-5892], u0 = 0xff, u1 = 0x01, floor = 0x01 -> Band 0.
 */
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

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef RE15_ASSET_PSX_DIR
#define RE15_ASSET_PSX_DIR "shared_assets/PSX"
#endif

static int fails = 0;
#define CHECK(c, ...) do { if (!(c)) { printf("FAIL: "); printf(__VA_ARGS__); printf("\n"); fails++; } } while (0)

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

static uint8_t *s_ems = NULL; static size_t s_ems_n = 0;
static uint8_t  s_blob[0x80000];
static int load_bank(uint8_t type)
{
    if (!s_ems) s_ems = slurp(RE15_ASSET_PSX_DIR "/EMD/CDEMD0.EMS", &s_ems_n);
    if (!s_ems) return 0;
    re15_enemy_bank_t *eb = re15_enemy_find(type);
    if (eb && eb->ok) return 1;
    int idx = re15_ems_index_for_type(type);
    size_t off = 0, len = 0;
    if (idx < 0 || re15_ems_get_entry(s_ems, s_ems_n, idx, &off, &len) != 0) return 0;
    if (len > sizeof s_blob) return 0;
    if (!eb) eb = re15_enemy_alloc(type);
    if (!eb) return 0;
    memcpy(s_blob, s_ems + off, len);
    re15_tim_t tim = (re15_tim_t){0};
    if (re15_emd_parse_container(s_blob, len, &eb->md1, &eb->skel, &eb->anim, &tim) != 0) {
        eb->type = 0; return 0;
    }
    eb->ok = 1; eb->buf = NULL;
    re15_emd_parse_own_bank(s_blob, len, &eb->skel_own, &eb->anim_own);
    eb->own_ok = (eb->anim_own.clip_count > 0);
    eb->loco_ok = (re15_emd_parse_loco_bank(s_blob, len, &eb->skel_loco, &eb->anim_loco) == 0);
    return 1;
}

static void frame_step(void)
{
    const unsigned char *raw; int len, id;
    re15_msg_tick(&raw, &len, &id);
    s_ctx.pad_current = 0; s_ctx.pad_pressed = 0;
    scd_vm_tick();
    re15_game_step(&s_ctx);
}

/* ---------------------------------------------------------------------------------------------
 * ZWEITE, UNABHAENGIGE IMPLEMENTIERUNG von FUN_8001bf04 (bewusst aus der Disasm-Regel
 * nachgeschrieben, NICHT aus dem Engine-Code kopiert) — damit der Test die Formel prueft und
 * nicht nur sich selbst.
 * ------------------------------------------------------------------------------------------ */
static int ref_dir_code(int32_t px, int32_t pz, const re15_sca_entry_t *c)
{
    int left_of_right = (px < (int32_t)c->x + (int32_t)c->width);   /* signbit(px-(x+w)) */
    int left_of_left  = (px <  (int32_t)c->x);                      /* signbit(px-x)     */
    int cx = (left_of_right << 1) | left_of_left;
    if (cx == 2) {
        int above_far  = (pz < (int32_t)c->z + (int32_t)c->density);
        int above_near = (pz <  (int32_t)c->z);
        int cz = (above_far << 1) | above_near;
        if (cz == 0) return 0x400;
        if (cz == 3) return 0xc00;
        return 0xf00;
    }
    if (cx == 0) return 0x800;
    if (cx == 3) return 0x000;
    return 0xf00;
}

/* ============================================================================================
 * TEIL A — der SCHREIBER, byte-exakt, gegen eine ECHTE Raum-Zelle mit u1 & 3
 * ============================================================================================ */
static const re15_sca_entry_t *find_cell(const re15_rdt_t *r, int want_u1_bits, unsigned mask)
{
    for (int i = 0; i < r->sca_count; i++) {
        const re15_sca_entry_t *c = &r->sca[i];
        if (!(mask & c->u0)) continue;
        if (want_u1_bits) { if ((c->u1 & 3) == 0) continue; }
        else              { if ((c->u1 & 3) != 0) continue; }
        if (c->type != 1) continue;
        return c;
    }
    return NULL;
}

static void part_a(const re15_rdt_t *src, const char *label, int expect_u1_bits)
{
    const re15_sca_entry_t *found = find_cell(src, expect_u1_bits, 4u);
    if (!found) { printf("  %s: keine passende Zelle gefunden\n", label); fails++; return; }

    /* Die ECHTEN Zellen-Bytes, aber ALLEIN in einem Ein-Zellen-RDT: der Original-Resolver hat
     * kein `break` (@0x8003b510-20) — die LETZTE treffende Zelle gewinnt. In einem echten Raum
     * ueberlappen an den Flanken mehrere Zellen, dann prueft der Test nicht mehr die Formel,
     * sondern die Nachbarschaft. ceiling = -32768/-32768 -> quadrant_of liefert immer 0
     * (FUN_8003b068), also ist die Partition rgn[0] mit genau dieser einen Zelle aktiv. */
    static re15_sca_entry_t s_cell;
    static re15_rdt_t       s_one;
    s_cell = *found;
    memset(&s_one, 0, sizeof s_one);
    s_one.sca = &s_cell; s_one.sca_count = 1; s_one.sca_rgn[0] = 1;
    s_one.ceiling_x = 0x8000u; s_one.ceiling_z = 0x8000u;
    const re15_rdt_t *rdt = &s_one;
    const re15_sca_entry_t *c = &s_cell;

    const int32_t X0 = (int32_t)c->x, Z0 = (int32_t)c->z;
    const int32_t X1 = X0 + (int32_t)c->width, Z1 = Z0 + (int32_t)c->density;
    const int32_t R  = 400;                       /* box[6] des Zombies (STAGE1.BIN 0x1f778) */
    const int32_t y  = -(int32_t)(c->floor >> 4) * 0x708;   /* band_from_y-Umkehrung */

    printf("  %s: Zelle typ=%d x[%ld..%ld] z[%ld..%ld] u0=0x%02x u1=0x%02x floor=0x%02x band=%d\n",
           label, c->type, (long)X0, (long)X1, (long)Z0, (long)Z1,
           c->u0, c->u1, c->floor, (int)(c->floor >> 4));

    struct { const char *n; int32_t sx, sz, tx, tz; } probe[4] = {
        { "von -X", X0 - 900, (Z0 + Z1) / 2, X0 - 100, (Z0 + Z1) / 2 },
        { "von +X", X1 + 900, (Z0 + Z1) / 2, X1 + 100, (Z0 + Z1) / 2 },
        { "von -Z", (X0 + X1) / 2, Z0 - 900, (X0 + X1) / 2, Z0 - 100 },
        { "von +Z", (X0 + X1) / 2, Z1 + 900, (X0 + X1) / 2, Z1 + 100 },
    };

    for (int p = 0; p < 4; p++) {
        uint8_t  contact   = 0xA5;                  /* Vorbelegung: der Clear muss 0xA0 lassen */
        uint16_t cell_attr = 0xBEEF;
        int32_t  nx = probe[p].tx, nz = probe[p].tz;
        int hit = re15_collision_constrain_contact(rdt, probe[p].sx, probe[p].sz, &nx, &nz,
                                                   R, y, 4u, &contact, &cell_attr);
        const int32_t dir = ref_dir_code(nx, nz, c);
        const uint8_t want = (uint8_t)((dir >> 4) + 8 + (c->u1 & 3));
        printf("     %s: (%ld,%ld)->(%ld,%ld) hit=%d +0x90=0x%02x (erwartet 0x%02x, dir=0x%03x) "
               "attr=0x%04x\n", probe[p].n, (long)probe[p].tx, (long)probe[p].tz,
               (long)nx, (long)nz, hit, contact, want, dir, cell_attr);
        CHECK(hit == 1, "%s/%s: FUN_8003b0a4-Rueckgabe %d statt 1", label, probe[p].n, hit);
        CHECK(contact == want,
              "%s/%s: +0x90 = 0x%02x, erwartet 0x%02x ((dir 0x%03x >> 4) + 8 + (u1 0x%02x & 3))",
              label, probe[p].n, contact, want, dir, c->u1);
        CHECK((contact & 3u) == (unsigned)(c->u1 & 3),
              "%s/%s: +0x90 & 3 = %u, aber cell.u1 & 3 = %u — die unteren Bits duerfen NUR "
              "aus u1 kommen", label, probe[p].n, contact & 3u, (unsigned)(c->u1 & 3));
        CHECK((contact & 8u) != 0, "%s/%s: Bit 3 (das +8 @0x8003b4c4) fehlt", label, probe[p].n);
        CHECK(cell_attr == (uint16_t)((uint16_t)c->u1 | ((uint16_t)c->floor << 8)),
              "%s/%s: +0x1b4-Attr 0x%04x != *(u16*)(cell+0x0a) 0x%04x", label, probe[p].n,
              cell_attr, (uint16_t)((uint16_t)c->u1 | ((uint16_t)c->floor << 8)));
    }

    /* CLEAR @0x8003b1d0-ec: ein kontaktfreier Lauf laesst NUR die obere Nibble stehen. */
    {
        uint8_t  contact   = 0xC9;                  /* Richtung 0xc + 8 + 1, wie nach einem Treffer */
        uint16_t cell_attr = 0x0101;
        int32_t  nx = X0 - 200000, nz = Z0 - 200000; /* weit weg, keine Zelle trifft */
        int hit = re15_collision_constrain_contact(rdt, nx, nz, &nx, &nz, R, y, 4u,
                                                   &contact, &cell_attr);
        printf("     Clear (kontaktfrei): +0x90 0xC9 -> 0x%02x, attr 0x0101 -> 0x%04x, hit=%d\n",
               contact, cell_attr, hit);
        CHECK(hit == 0, "%s: Rueckgabe %d obwohl keine Zelle getroffen", label, hit);
        CHECK(contact == 0xC0, "%s: Clear ergab 0x%02x statt 0xC0 (`andi 0xf0` @0x8003b1d8)",
              label, contact);
        CHECK(cell_attr == 0, "%s: +0x1b4 nicht genullt (0x%04x, `sw zero,436` @0x8003b1ec)",
              label, cell_attr);
    }
}

/* ============================================================================================
 * TEIL B/C — LIVE: der Gegner laeuft in die Zelle, der Leser feuert
 * ============================================================================================ */
typedef struct {
    int   contact_frames;    /* Frames mit +0x90 & 8 (Klemme aktiv)          */
    int   first_contact_frame;
    uint8_t first_contact_value;
    int   u1_frames;         /* Frames mit +0x90 & 3 (Ausweich-Bits gesetzt) */
    int   first_u1_frame;
    uint8_t first_u1_value;
    int   first_sub_frame;   /* erster Frame mit +0x5 in {9,10}              */
    uint8_t first_sub;
    int   sub_frames;
} live_t;

static void live_run(const char *rdt_path, unsigned room_id, uint8_t ztype,
                     int32_t zx, int32_t zz, int32_t zy,
                     int32_t px, int32_t pz, int frames, int verbose, live_t *out)
{
    memset(out, 0, sizeof *out);
    out->first_u1_frame = -1; out->first_sub_frame = -1; out->first_contact_frame = -1;

    size_t n = 0;
    uint8_t *buf = slurp(rdt_path, &n);
    if (!buf) { printf("FAIL: %s nicht lesbar\n", rdt_path); fails++; return; }
    if (re15_rdt_parse(buf, n, &g_room_rdt) != 0) { printf("FAIL: Parse %s\n", rdt_path); fails++; free(buf); return; }
    g_room_rdt_ok = 1; s_rdt = g_room_rdt;

    memset(&s_cam, 0, sizeof s_cam); memset(&s_ctx, 0, sizeof s_ctx);
    s_ctx.rdt = &s_rdt; s_ctx.rdt_ok = 1; s_ctx.cam_view = &s_cam; s_ctx.active_cut = 0;
    re15_ai_flavor_set(RE15_AI_FLAVOR_RE15);
    re15_actor_init(); re15_aot_init(); scd_vm_init();
    re15_enemy_reset(); re15_enemy_ai_set_paused(0);
    re15_player_cmd_reset();
    re15_damage_seed_rng(0x2545f491u);
    g_current_room_id = room_id;

    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100; pl->hit_react = 0;
    pl->state = 0; pl->motion = 0; pl->y = zy; pl->floor = (uint8_t)(-(zy / 0x708));
    pl->x = px; pl->z = pz;
    re15_collision_set_band(pl->floor);
    re15_inv_load_briefing();
    scd_room_reenter(&s_rdt, pl->x, pl->z, 0);
    (void)load_bank(ztype);

    /* Den ersten Gegner des gesuchten Typs an die Startposition setzen und in den ENGAGE-
     * Zustand versetzen (+0x4 = 1, +0x5 = 2 = decide FUN_80102058) — POSITIV-KONTROLLE:
     * Position + Wachzustand sind gestellt, ALLES danach (Nav, Klemme, +0x90, Leser) laeuft
     * ueber den echten Weg. */
    int zs = -1;
    for (int s = 1; s < RE15_ACTOR_MAX; s++)
        if (g_actors[s].active && g_actors[s].type == ztype) { zs = s; break; }
    if (zs < 0) { printf("FAIL: kein Gegner Typ 0x%02x in %s\n", ztype, rdt_path); fails++;
                  g_room_rdt_ok = 0; free(buf); return; }
    re15_actor_t *e = &g_actors[zs];
    e->x = zx; e->z = zz; e->y = zy; e->floor = (uint8_t)(-(zy / 0x708));
    e->state = 1; e->sub_state_1 = 2; e->sub_state_2 = 0; e->sub_state_3 = 0;
    e->hp = 100;

    if (verbose)
        printf("     Start: Slot %d Typ 0x%02x @(%ld,%ld,%ld) Band %d — Spieler @(%ld,%ld)\n",
               zs, ztype, (long)zx, (long)zy, (long)zz, (int)e->floor, (long)px, (long)pz);

    for (int f = 0; f < frames; f++) {
        uint8_t before = e->ai_contact;
        frame_step();
        if (!e->active) break;
        if (e->ai_contact & 8u) {
            out->contact_frames++;
            if (out->first_contact_frame < 0) {
                out->first_contact_frame = f; out->first_contact_value = e->ai_contact;
            }
        }
        if (e->ai_contact & 3u) {
            out->u1_frames++;
            if (out->first_u1_frame < 0) { out->first_u1_frame = f; out->first_u1_value = e->ai_contact; }
        }
        if (e->sub_state_1 == 9 || e->sub_state_1 == 10) {
            out->sub_frames++;
            if (out->first_sub_frame < 0) { out->first_sub_frame = f; out->first_sub = e->sub_state_1; }
        }
        if (verbose && (f % 40 == 0 || (out->first_u1_frame >= 0 && f >= out->first_u1_frame - 2 &&
                                  f <= out->first_u1_frame + 6)))
            printf("       f%-4d pos=(%6ld,%6ld) yaw=%5d mo=%02x d=%u  +0x90: 0x%02x -> 0x%02x  "
                   "+0x4=%u +0x5=%-2u +0x6=%-2u attr=0x%04x\n",
                   f, (long)e->x, (long)e->z, (int)e->rot_y, (unsigned)e->motion,
                   (unsigned)e->ai_dist, before, e->ai_contact,
                   e->state, e->sub_state_1, e->sub_state_2, e->coll_cell_attr);
    }
    g_room_rdt_ok = 0;
    free(buf);
}

int main(void)
{
    printf("== PIN: entity+0x90 — SCA-Kontakt-Byte (Schreiber @0x8003b4c0-dc / Leser 14x) ==\n");

    /* ---- TEIL A: Schreiber byte-exakt ------------------------------------------------------ */
    size_t n = 0;
    uint8_t *b10b0 = slurp(RE15_ASSET_PSX_DIR "/STAGE1/ROOM10B0.RDT", &n);
    if (!b10b0) { printf("SKIP: ROOM10B0.RDT fehlt\n"); return 77; }
    re15_rdt_t r10b0;
    if (re15_rdt_parse(b10b0, n, &r10b0) != 0) { printf("FAIL: Parse ROOM10B0\n"); return 1; }

    size_t n2 = 0;
    uint8_t *b1140 = slurp(RE15_ASSET_PSX_DIR "/STAGE1/ROOM1140.RDT", &n2);
    if (!b1140) { printf("SKIP: ROOM1140.RDT fehlt\n"); return 77; }
    re15_rdt_t r1140;
    if (re15_rdt_parse(b1140, n2, &r1140) != 0) { printf("FAIL: Parse ROOM1140\n"); return 1; }

    printf("\n-- TEIL A: Schreiber gegen eine ECHTE u1&3-Zelle (ROOM10B0) --\n");
    part_a(&r10b0, "ROOM10B0 (u1&3 != 0)", 1);

    printf("\n-- TEIL A': NEGATIV-KONTROLLE, Raum mit u1 == 0 (ROOM1140) --\n");
    /* Der Klemmpfad muss dort GENAU 8 in die untere Nibble schreiben — Kontakt ja,
     * Ausweich-Bits nein. Damit ist `(+0x90 & 3) == 0` und JEDER der 14 Leser faellt durch. */
    part_a(&r1140, "ROOM1140 (u1 == 0)", 0);

    /* Zusatz: Zensus der Negativ-Raeume, damit der Pin nicht auf einer einzelnen Zelle steht.
     * ROOM1190/1230 sind die HUNDE-Raeume: dort ist u1 ueberall 0, der Reroute-Gate
     * `andi v0,v1,0x3` @0x8010e268 kann also im ORIGINAL nie passen -> der Hunde-Reroute
     * (sub 13/14) feuert in seinen eigenen Raeumen NICHT. (Der frueher hier stehende
     * Port-Ersatzschreiber setzte Bit 0 bei JEDER Klemme und liess ihn staendig feuern.) */
    static const char *neg[] = { "STAGE1/ROOM1140.RDT", "STAGE1/ROOM1030.RDT",
                                 "STAGE1/ROOM1190.RDT", "STAGE1/ROOM1230.RDT" };
    for (unsigned k = 0; k < sizeof neg / sizeof neg[0]; k++) {
        char p[512]; snprintf(p, sizeof p, "%s/%s", RE15_ASSET_PSX_DIR, neg[k]);
        size_t nn = 0; uint8_t *bb = slurp(p, &nn);
        if (!bb) { printf("FAIL: %s nicht lesbar\n", neg[k]); fails++; continue; }
        re15_rdt_t rr;
        if (re15_rdt_parse(bb, nn, &rr) != 0) { printf("FAIL: Parse %s\n", neg[k]); fails++; free(bb); continue; }
        int bad = 0;
        for (int i = 0; i < rr.sca_count; i++) if (rr.sca[i].u1 & 3) bad++;
        printf("     %s: %d SCA-Zellen, davon %d mit u1&3\n", neg[k], rr.sca_count, bad);
        CHECK(bad == 0, "%s hat %d Zellen mit u1&3 — kein Negativ-Raum mehr", neg[k], bad);
        free(bb);
    }

    /* ---- TEIL B: LIVE, der Gegner laeuft in die u1&3-Zelle --------------------------------- */
    printf("\n-- TEIL B: LIVE in ROOM10B0 — Zombie 0x10 laeuft gegen die u1=0x01-Zelle --\n");
    /* Zelle #6: x[-6992..-350], z[-10126..-5892], Band 0. Der Zombie startet WESTLICH davon
     * auf Band 0, der Spieler steht OESTLICH dahinter -> der Zombie laeuft in die Westflanke.
     * (Positiv-Kontrolle: nur Startpose gestellt; Nav/Klemme/+0x90/Leser laufen echt.) */
    live_t lv;
    live_run(RE15_ASSET_PSX_DIR "/STAGE1/ROOM10B0.RDT", 0x10B0, 0x10,
             -7500, -8000, 0,          /* Zombie: westlich der Zelle, Band 0 */
             2000, -8000,              /* Spieler: oestlich dahinter        */
             900, 1, &lv);
    printf("     Ergebnis: Kontakt-Frames=%d  u1-Frames=%d (erster f%d, +0x90=0x%02x)  "
           "+0x5 in {9,10}: %d Frames (erster f%d, sub=%u)\n",
           lv.contact_frames, lv.u1_frames, lv.first_u1_frame, lv.first_u1_value,
           lv.sub_frames, lv.first_sub_frame, lv.first_sub);

    /* Gemessene Kette (Ausgabe oben): f68 Klemme an der u1=0x01-Zelle -> +0x90 = 0x09
     * (Richtungs-Nibble 0 = Aktor WESTLICH der Zelle, +8 Kontakt, +1 = u1 & 1), +0x1b4-Attr
     * 0x0101; f69 feuert der Leser @0x8010206c -> Zustandswort 0xA01, also +0x5 = 10 (Bit 0
     * gesetzt -> 10, nicht 9, `andi v0,a0,0x1` / `addiu v0,v0,9` @0x8010209c-a0). Danach laeuft
     * der Ausweich-Zustand: Yaw slewt auf die Fluchtrichtung 0x000 (Leser @0x80103478), Clip
     * 0x16 (Leser @0x80103518), Band += 1. */
    CHECK(lv.u1_frames > 0, "LIVE ROOM10B0: +0x90 & 3 hat NIE gefeuert — der Schreiber erreicht "
                            "die u1=0x01-Zelle nicht");
    CHECK(lv.first_u1_value == 0x09,
          "LIVE ROOM10B0: erstes +0x90 mit u1-Bits = 0x%02x, erwartet 0x09 "
          "(dir 0x000 >> 4) + 8 + (u1 0x01 & 3)", lv.first_u1_value);
    CHECK(lv.first_sub_frame == lv.first_u1_frame + 1,
          "LIVE ROOM10B0: +0x5 wechselte in Frame %d, der Kontakt kam in Frame %d — der Leser "
          "muss im FOLGE-Frame feuern (Resolver laeuft NACH dem AI-Dispatch @0x8010062c)",
          lv.first_sub_frame, lv.first_u1_frame);
    CHECK(lv.first_sub == 10,
          "LIVE ROOM10B0: +0x5 = %u statt 10 — u1 == 0x01 hat Bit 0 gesetzt, also 0xA01 "
          "(@0x8010209c-b0)", lv.first_sub);

    /* ---- TEIL C: REGRESSIONSWACHE, u1==0-Raum darf sich NICHT aendern ---------------------- */
    printf("\n-- TEIL C: REGRESSIONSWACHE ROOM1140 (u1 == 0 in allen Zellen) --\n");
    /* SCHARFE Negativ-Kontrolle: derselbe Aufbau wie Teil B, nur in einem u1==0-Raum. Die
     * Zombie-Wand-Zelle x[-10600..-8600] z[-25550..1850] wird von OSTEN angelaufen -> Richtungs-
     * Nibble 0x8, Winkel 0x800 = 2048, und der Zombie schaut beim Anlaufen genau dorthin, das
     * Winkel-Gate `(dir - yaw + 512) & 0xfff < 1024` waere also OFFEN. Es feuert trotzdem
     * nichts, weil `+0x90 & 3` == 0 bleibt — genau die geforderte Nullwirkung. */
    live_t lv0;
    live_run(RE15_ASSET_PSX_DIR "/STAGE1/ROOM1140.RDT", 0x1140, 0x10,
             -8300, -19600, 0,          /* Zombie: oestlich der Wandzelle */
             -15000, -19600,            /* Spieler: westlich dahinter     */
             900, 1, &lv0);
    printf("     Ergebnis: Kontakt-Frames=%d (erster f%d, +0x90=0x%02x)  u1-Frames=%d  "
           "+0x5 in {9,10}: %d Frames\n",
           lv0.contact_frames, lv0.first_contact_frame, lv0.first_contact_value,
           lv0.u1_frames, lv0.sub_frames);
    CHECK(lv0.contact_frames > 0,
          "REGRESSIONSWACHE: in ROOM1140 hat die Klemme nie gefeuert — dann ist die "
          "Negativ-Kontrolle wertlos (Positiv-Kontrolle fehlt)");
    CHECK((lv0.first_contact_value & 0x0fu) == 8u,
          "REGRESSIONSWACHE: +0x90 = 0x%02x — in einem u1==0-Raum MUSS die untere Nibble genau "
          "8 sein (nur das +8 @0x8003b4c4)", lv0.first_contact_value);
    CHECK(lv0.u1_frames == 0,
          "REGRESSIONSWACHE: +0x90 & 3 feuerte %d Frames in ROOM1140, obwohl u1 dort ueberall "
          "0 ist", lv0.u1_frames);
    CHECK(lv0.sub_frames == 0,
          "REGRESSIONSWACHE: +0x5 erreichte %d Frames lang 9/10 in ROOM1140 — dort darf sich "
          "durch den +0x90-Schreiber NICHTS aendern", lv0.sub_frames);

    free(b10b0); free(b1140);
    re15_ai_flavor_set(RE15_AI_FLAVOR_RE15);
    printf("\n== %s (%d Fehler) ==\n", fails ? "FEHLGESCHLAGEN" : "OK", fails);
    return fails ? 1 : 0;
}
