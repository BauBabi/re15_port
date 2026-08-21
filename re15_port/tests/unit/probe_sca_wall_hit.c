/* probe_sca_wall_hit.c — MESSUNG + PIN: der RUECKGABEWERT des SCA-Klemmpfads
 * (entity+0x1d6 Maggot / +0x1da Hund + Adult-Spider / RE2s +0x110 Bit 0).
 *
 * ============================================================================================
 * DER SCHREIBER — FUN_8003b0a4 liefert ZWEI Ausgaben, nicht eine
 * ============================================================================================
 * (1) entity+0x90, das Kontakt-BYTE (Richtungs-Nibble | 8 | cell.u1 & 3) — @0x8003b4c0-dc,
 *     gepinnt in probe_sca_contact90.c.
 * (2) DER RUECKGABEWERT — davon voellig unabhaengig, disassembliert aus info/Re1.5/PSX.EXE:
 *       8003b124  addu s7,zero,zero    ; s7 = 0 beim Eintritt
 *       8003b500  ori  s7,s7,0x1       ; bei JEDEM Broadphase-Treffer (derselbe Block wie
 *                                        der +0x90-Write @0x8003b4dc, also dieselbe Bedingung)
 *       8003b520  andi v0,s7,0xff      ; return
 *     Ein voller `dis 0x8003b0a4 300 | grep s7` findet GENAU diese drei Beruehrungen von s7
 *     (plus save/restore @0x8003b0c0/@0x8003b52c) -> der Rueckgabewert ist strikt 0 oder 1.
 *
 * DIE VIER ROOTS, DIE IHN SPEICHERN (jeder in SEIN eigenes Feld, alle UNBEDINGT, alle mit
 * denselben Argumenten a0 = entity+0x34 / a1 = dim[+6] / a2 = 4):
 *   Maggot 0x27   Root 0x80116db8 (STAGE1.BIN)
 *       80116e64 lw v0,120(a0) / 80116e68 ori a2,zero,0x4 / 80116e6c lhu a1,6(v0)
 *       80116e70 jal 0x8003b0a4 / 80116e74 addiu a0,a0,52
 *       80116e84 sh v0,470(v1)          ; +0x1d6   [STORE HALFWORD]
 *   Hund 0x20     Root 0x8010d7f8 (STAGE1.BIN)
 *       8010d8a4-b4 identische Argumente / 8010d8c4 sh v0,474(v1)   ; +0x1da
 *   A-Spider 0x25 Root 0x801109e4 (STAGE2.BIN)
 *       80110a90-a0 identische Argumente / 80110ab0 sh v0,474(v1)   ; +0x1da
 *   Kraehe 0x21   Root 0x80112020 (STAGE1.BIN)
 *       801121f8-208 identische Argumente / 80112218 sb v0,465(v1)  ; +0x1d1  (NICHT portiert,
 *                                                                    siehe OPEN unten)
 *
 * DIE LESER
 * ---------
 *   Maggot +0x1d6 (Voll-Scan aller Lade-/Speicherbefehle mit Displacement 470 ueber STAGE1.BIN;
 *                  3 Leser, alle `lhu`):
 *     @0x80117448  if (+0x1d6) +0x1de += 1; else +0x1de = 0        (Steckenbleib-Zaehler)
 *     @0x80117b18  if (+0x1d6) skip; else if (+0x1e1) skip; else +0x5 = 0xf   (REAR-UP)
 *     @0x80118054  if (+0x1d6) skip; else if (+0x1d4 < 0x1771) skip; ...      (FERN-SPRUNG)
 *   Hund +0x1da (Voll-Scan ueber STAGE1.BIN; 2 Leser, `lh`):
 *     @0x8010dd80  if (+0x1da) +0x1dc += 1; else +0x1dc = 0
 *     @0x8010e840  if (+0x1da) ... (Reroute-Zweig)
 *   Adult Spider +0x1da (Voll-Scan ueber STAGE2.BIN; 1 Leser, `lh`):
 *     @0x80111008  if (+0x1da) +0x1dc += 1; else +0x1dc = 0
 *     (die zwei `sh t1,474(v0)` @0x80114428/@0x80114a08 adressieren ueber Stride-0x1f4-Arithmetik
 *      ein FREMDES Entity = Spawn-Pfad, kein Wandkontakt.)
 *   RE2 +0x110 (Voll-Scan `l* rt,272(rs)` ueber die RE2-Overlays, alle `lw`):
 *     Hund   EMD0G_MOD0.BIN @0x80100618 @0x801006D4 @0x80100D50 @0x80102BC8 @0x80102F00
 *     Zombie EMOVL10_S0.BIN @0x80101844 @0x801020AC @0x80102384 @0x8010B894
 *     Spinne EMS25.BIN      @0x80100EA8 @0x801018E8 @0x80101C8C @0x801028BC
 *     Kraehe EMOVL21_S0.BIN @0x80103104 @0x801033BC @0x801040AC (+ @0x80104108 liest +0x114)
 *     EMS26.BIN (Baby-Spinne): 0 Leser.
 *
 * ============================================================================================
 * WARUM DER ALTE PORT-ERSATZ BEWEISBAR TOT WAR
 * ============================================================================================
 * Der Port proxyte alle drei Felder ueber `(entity+0x90 & 3) != 0`. Aber +0x90 = (dir>>4) + 8 +
 * (u1 & 3), und 8 & 3 == 0 -> `+0x90 & 3` IST `cell.u1 & 3`. In einem Raum ohne u1&3-Zelle ist
 * der Ausdruck also IMMER 0, egal wie fest der Gegner an der Wand klebt. TEIL A misst genau das:
 * ROOM11C0 (Maggot) und ROOM2090 (Adult Spider) haben NULL Zellen mit u1&3.
 * ============================================================================================
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

/* ============================================================================================
 * TEIL A — DATENPIN: in den Raeumen der drei Gegner gibt es KEINE u1&3-Zelle,
 *          also war der alte Proxy `(+0x90 & 3) != 0` dort nachweislich immer 0.
 * ========================================================================================== */
static void part_a(const char *rel, int *out_cells, int *out_u13)
{
    char p[512]; snprintf(p, sizeof p, "%s/%s", RE15_ASSET_PSX_DIR, rel);
    size_t n = 0; uint8_t *b = slurp(p, &n);
    *out_cells = -1; *out_u13 = -1;
    if (!b) { printf("FAIL: %s nicht lesbar\n", rel); fails++; return; }
    re15_rdt_t r;
    if (re15_rdt_parse(b, n, &r) != 0) { printf("FAIL: Parse %s\n", rel); fails++; free(b); return; }
    int u13 = 0, solid = 0;
    for (int i = 0; i < r.sca_count; i++) {
        if (r.sca[i].u0 & 4u) solid++;
        if (r.sca[i].u1 & 3u) u13++;
    }
    printf("     %-22s  SCA-Zellen=%3d  gegner-solide(u0&4)=%3d  mit u1&3=%d\n",
           rel, r.sca_count, solid, u13);
    *out_cells = r.sca_count; *out_u13 = u13;
    free(b);
}

/* ============================================================================================
 * TEIL B — SCHREIBER-PIN: der Rueckgabewert ist 1, WAEHREND `+0x90 & 3` 0 bleibt.
 *          Genau die Konstellation, in der der alte Proxy versagte.
 * ========================================================================================== */
static void part_b(const char *rel, int32_t radius)
{
    char p[512]; snprintf(p, sizeof p, "%s/%s", RE15_ASSET_PSX_DIR, rel);
    size_t n = 0; uint8_t *b = slurp(p, &n);
    if (!b) { printf("FAIL: %s nicht lesbar\n", rel); fails++; return; }
    static re15_rdt_t r; if (re15_rdt_parse(b, n, &r) != 0) {
        printf("FAIL: Parse %s\n", rel); fails++; free(b); return; }

    const re15_sca_entry_t *c = NULL;
    for (int i = 0; i < r.sca_count; i++)
        if ((r.sca[i].u0 & 4u) && r.sca[i].type == 1) { c = &r.sca[i]; break; }
    if (!c) { printf("FAIL: %s hat keine gegner-solide Rechteck-Zelle\n", rel); fails++; free(b); return; }

    /* Ein-Zellen-RDT wie in probe_sca_contact90.c TEIL A: der Resolver hat kein `break`
     * (@0x8003b510-20), in einem echten Raum wuerde die Nachbarschaft mitmessen. */
    static re15_sca_entry_t cell; static re15_rdt_t one;
    cell = *c; memset(&one, 0, sizeof one);
    one.sca = &cell; one.sca_count = 1; one.sca_rgn[0] = 1;
    one.ceiling_x = 0x8000u; one.ceiling_z = 0x8000u;

    const int32_t X0 = (int32_t)cell.x, Z0 = (int32_t)cell.z;
    const int32_t X1 = X0 + (int32_t)cell.width, Z1 = Z0 + (int32_t)cell.density;
    const int32_t y  = -(int32_t)(cell.floor >> 4) * 0x708;

    uint8_t contact = 0; uint16_t attr = 0;
    int32_t nx = X0 - 100, nz = (Z0 + Z1) / 2;
    int hit = re15_collision_constrain_contact(&one, X0 - 900, nz, &nx, &nz,
                                               radius, y, 4u, &contact, &attr);
    printf("     %-22s  Zelle x[%ld..%ld] z[%ld..%ld] u1=0x%02x -> Rueckgabe=%d  +0x90=0x%02x  "
           "(+0x90&3)=%u\n", rel, (long)X0, (long)X1, (long)Z0, (long)Z1, cell.u1,
           hit, contact, contact & 3u);
    CHECK(hit == 1, "%s: FUN_8003b0a4-Rueckgabe %d statt 1 (@0x8003b500 `ori s7,s7,0x1`)", rel, hit);
    CHECK((contact & 8u) != 0, "%s: +0x90 Bit 3 fehlt", rel);
    if ((cell.u1 & 3) == 0)
        CHECK((contact & 3u) == 0, "%s: +0x90&3 = %u obwohl u1&3 == 0", rel, contact & 3u);
    free(b);
}

/* ============================================================================================
 * TEIL C — LIVE ueber den ECHTEN Weg: echte RDT in g_room_rdt, SCD-VM hochgefahren, echte
 *          Sce_em_set-Spawns, echte EMS-Bank, re15_game_step pro Frame. Der Wandkontakt wird
 *          NICHT von Hand gesetzt: der Gegner wird an eine ECHTE SCA-Zelle des Raums gestellt
 *          und der Spieler so platziert, dass die echte Nav ihn dagegen drueckt. Gemessen wird
 *            neu = e->sca_wall_hit      (der portierte FUN_8003b0a4-Rueckgabewert)
 *            alt = (e->ai_contact & 3)  (der Ersatz, den der Port vorher benutzte)
 *          plus je Gegner sein ECHTER Verbraucher. Jeder Positiv-Fall hat einen Negativ-Fall
 *          (derselbe Aufbau, nur weit weg von jeder Zelle).
 * ========================================================================================== */
typedef struct {
    int frames, hit_new, hit_old, first_new, first_old;
    int ref_hit;         /* UNABHAENGIGE Kontrolle: Klemmpfad noch einmal an der Frame-End-Pos */
    int consumer;        /* typ-spezifischer Verbraucher-Zaehler (s. Aufrufer) */
    int consumer_max;
} live_t;

/* Die erste gegner-solide (u0 & 4) Rechteck-Zelle des Raums. */
static const re15_sca_entry_t *first_solid(const re15_rdt_t *r)
{
    for (int i = 0; i < r->sca_count; i++)
        if ((r->sca[i].u0 & 4u) && r->sca[i].type == 1) return &r->sca[i];
    return NULL;
}

/* Ein "klebriger" Wandplatz: eine Stelle, an der der Klemmpfad auch NACH seinem eigenen Push
 * noch trifft — also eine Innenecke, aus der die Klemme den Aktor nicht heraus bekommt. Nur so
 * ist der Positiv-Fall unabhaengig davon, ob die jeweilige KI zufaellig in die Wand steuert.
 * Gesucht wird ueber die ECHTEN Zellen des Raums, nichts wird konstruiert. */
static int find_sticky(const re15_rdt_t *r, int32_t radius, int32_t y,
                       int32_t *out_x, int32_t *out_z)
{
    for (int i = 0; i < r->sca_count; i++) {
        const re15_sca_entry_t *c = &r->sca[i];
        if (!(c->u0 & 4u) || c->type != 1) continue;
        const int32_t X0 = (int32_t)c->x, Z0 = (int32_t)c->z;
        const int32_t X1 = X0 + (int32_t)c->width, Z1 = Z0 + (int32_t)c->density;
        const int32_t cand[4][2] = {
            { X0 + (int32_t)c->width / 2, Z0 + (int32_t)c->density / 2 },  /* mitten drin */
            { X0, Z0 }, { X1, Z0 }, { X0, Z1 },                            /* die Ecken */
        };
        for (int k = 0; k < 4; k++) {
            int32_t x = cand[k][0], z = cand[k][1];
            uint8_t cc; uint16_t aa;
            for (int it = 0; it < 8; it++) {                 /* bis zum Fixpunkt klemmen */
                int32_t nx = x, nz = z; cc = 0; aa = 0;
                re15_collision_constrain_contact(r, x, z, &nx, &nz, radius, y, 4u, &cc, &aa);
                if (nx == x && nz == z) break;
                x = nx; z = nz;
            }
            {   /* haelt der Fixpunkt den Kontakt? */
                int32_t nx = x, nz = z; cc = 0; aa = 0;
                if (re15_collision_constrain_contact(r, x, z, &nx, &nz, radius, y, 4u, &cc, &aa)
                    && nx == x && nz == z) { *out_x = x; *out_z = z; return i; }
            }
        }
    }
    return -1;
}

typedef struct { uint8_t state, sub1, sub2, sub3, grid; int re2; } setup_t;

static void live_run(const char *rdt_path, unsigned room_id, uint8_t etype, const setup_t *su,
                     int wall, int frames, int verbose, live_t *out)
{
    memset(out, 0, sizeof *out);
    out->first_new = -1; out->first_old = -1;

    size_t n = 0; uint8_t *buf = slurp(rdt_path, &n);
    if (!buf) { printf("FAIL: %s nicht lesbar\n", rdt_path); fails++; return; }
    if (re15_rdt_parse(buf, n, &g_room_rdt) != 0) {
        printf("FAIL: Parse %s\n", rdt_path); fails++; free(buf); return; }
    g_room_rdt_ok = 1; s_rdt = g_room_rdt;

    const re15_sca_entry_t *c = first_solid(&g_room_rdt);
    if (!c) { printf("FAIL: %s hat keine gegner-solide Rechteck-Zelle\n", rdt_path); fails++;
              g_room_rdt_ok = 0; free(buf); return; }
    const int32_t X0 = (int32_t)c->x, Z0 = (int32_t)c->z;
    const int32_t ZM = Z0 + (int32_t)c->density / 2;
    const int32_t ey = -(int32_t)(c->floor >> 4) * 0x708;

    memset(&s_cam, 0, sizeof s_cam); memset(&s_ctx, 0, sizeof s_ctx);
    s_ctx.rdt = &s_rdt; s_ctx.rdt_ok = 1; s_ctx.cam_view = &s_cam; s_ctx.active_cut = 0;
    re15_ai_flavor_set(su->re2 ? RE15_AI_FLAVOR_RE2 : RE15_AI_FLAVOR_RE15);
    re15_actor_init(); re15_aot_init(); scd_vm_init();
    re15_enemy_reset(); re15_enemy_ai_set_paused(0);
    re15_player_cmd_reset();
    re15_damage_seed_rng(0x2545f491u);
    g_current_room_id = room_id;

    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100; pl->hit_react = 0;
    pl->state = 0; pl->motion = 0; pl->y = ey; pl->floor = (uint8_t)(c->floor >> 4);
    re15_collision_set_band(pl->floor);
    re15_inv_load_briefing();

    /* WAND-Fall: der Gegner steht 300 Einheiten westlich der Westflanke der Zelle (also tief in
     * der um seinen Radius aufgeblasenen Broadphase @0x8003b284-94), der Spieler 2000 suedlich
     * UND 200 OESTLICH der Flanke — die echte Nav steuert damit dauerhaft mit einer Komponente
     * IN die Wand, der Gegner klebt an ihr.
     * FREI-Fall (Negativ-Kontrolle): identischer Aufbau, nur um 400000 verschoben -> keine Zelle
     * kann die Broadphase je bestehen. */
    const int32_t off = wall ? 0 : 400000;
    pl->x = X0 + 200 + off; pl->z = ZM + 2000;
    scd_room_reenter(&s_rdt, pl->x, pl->z, 0);
    (void)load_bank(etype);

    int es = -1;
    for (int s = 1; s < RE15_ACTOR_MAX; s++)
        if (g_actors[s].active && g_actors[s].type == etype) { es = s; break; }
    if (es < 0) { printf("FAIL: kein Gegner Typ 0x%02x in %s\n", etype, rdt_path); fails++;
                  g_room_rdt_ok = 0; free(buf); return; }
    re15_actor_t *e = &g_actors[es];
    e->x = X0 - 300 + off; e->z = ZM; e->y = ey; e->floor = (uint8_t)(c->floor >> 4);
    {   /* Wenn es im Raum eine Stelle gibt, an der die Klemme auch nach ihrem eigenen Push noch
         * trifft (Innenecke), dann dorthin — sonst haengt der Positiv-Fall daran, ob die KI
         * zufaellig in die Wand steuert. */
        int32_t sx = 0, sz = 0;
        if (find_sticky(&g_room_rdt, e->hit_radius_min, ey, &sx, &sz) >= 0) {
            e->x = sx + off; e->z = sz;
            pl->x = sx + 500 + off; pl->z = sz + 2000;
        }
    }
    e->state = su->state; e->sub_state_1 = su->sub1; e->sub_state_2 = su->sub2;
    e->sub_state_3 = su->sub3; e->grid_id = su->grid;
    e->rot_y = (int16_t)(((int)re15_atan2_q12(pl->z - e->z, pl->x - e->x) - 0x400) & 0xfff);

    if (verbose)
        printf("     %s: Slot %d Typ 0x%02x @(%ld,%ld) r=%d Band %d — Zelle x>=%ld z[%ld..%ld], "
               "Spieler @(%ld,%ld), Flavor %s\n", wall ? "WAND" : "FREI", es, etype,
               (long)e->x, (long)e->z, (int)e->hit_radius_min, (int)e->floor,
               (long)X0, (long)Z0, (long)(Z0 + c->density), (long)pl->x, (long)pl->z,
               su->re2 ? "RE2" : "RE1.5");

    for (int f = 0; f < frames; f++) {
        frame_step();
        if (!e->active) break;
        out->frames++;
        if (e->sca_wall_hit) { out->hit_new++; if (out->first_new < 0) out->first_new = f; }
        if (e->ai_contact & 3u) { out->hit_old++; if (out->first_old < 0) out->first_old = f; }
        {   /* UNABHAENGIGE Kontrolle, damit ein `neu == 0` nicht mit "steht gar nicht an der
             * Wand" verwechselt wird: derselbe Klemmpfad noch einmal an der Frame-End-Position. */
            int32_t nx = e->x, nz = e->z; uint8_t cc = 0; uint16_t aa = 0;
            if (re15_collision_constrain_contact(&g_room_rdt, e->x, e->z, &nx, &nz,
                                                 e->hit_radius_min, e->y, 4u, &cc, &aa))
                out->ref_hit++;
        }
        switch (etype) {
        case 0x27:  /* Maggot: REAR-UP (+0x5 == 15) — der Zustand, den @0x80117b18 sperrt */
            if (e->sub_state_1 == 15) out->consumer++;
            /* CHASE-Eintrittspose neu scharfschalten: NUR die Entry-Bedingungen des
             * Bite/Rear-up-Zweigs, NICHT der Kontakt — der entsteht echt aus der Geometrie. */
            if (e->state == 1 && e->sub_state_1 != 15) {
                e->sub_state_1 = 3; e->sub_state_2 = 0; e->sub_state_3 = 0;
                e->dog_blocked_ctr = 0; e->mag_pin_cd = 0; e->hit_react = 0; pl->hit_react = 0;
                e->rot_y = (int16_t)(((int)re15_atan2_q12(pl->z - e->z, pl->x - e->x) - 0x400) & 0xfff);
            }
            break;
        case 0x25:  /* Adult Spider: der Steckenbleib-Zaehler +0x1dc (@0x80111008) */
            out->consumer = (int)e->ai_target_x;
            if (out->consumer > out->consumer_max) out->consumer_max = out->consumer;
            break;
        case 0x20:  /* Hund: RE1.5 +0x1dc (dog_blocked_ctr), RE2 der +0x110-Zweig (+0x6) */
            if (su->re2) { if (e->sub_state_2 != 1) out->consumer++; }
            else {
                out->consumer = (int)e->dog_blocked_ctr;
                if (out->consumer > out->consumer_max) out->consumer_max = out->consumer;
            }
            break;
        default: break;
        }
        if (verbose && (f % 60 == 0 || f < 3))
            printf("       f%-4d pos=(%7ld,%7ld) yaw=%5d  neu=%u alt=%u +0x90=0x%02x  "
                   "+0x4=%u +0x5=%-2u +0x6=%-2u  verbr=%d\n",
                   f, (long)e->x, (long)e->z, (int)e->rot_y,
                   (unsigned)e->sca_wall_hit, (unsigned)(e->ai_contact & 3u), e->ai_contact,
                   e->state, e->sub_state_1, e->sub_state_2, out->consumer);
    }
    printf("     -> %s: %d Frames  neu=%d  alt=%d  ref=%d  erster neu=f%d erster alt=f%d  "
           "Verbraucher=%d (max %d)\n", wall ? "WAND" : "FREI",
           out->frames, out->hit_new, out->hit_old, out->ref_hit, out->first_new, out->first_old,
           out->consumer, out->consumer_max);
    g_room_rdt_ok = 0;
    free(buf);
}

int main(void)
{
    printf("== PIN: FUN_8003b0a4-RUECKGABEWERT (+0x1d6 / +0x1da / RE2 +0x110 Bit0) ==\n");

    printf("\n-- TEIL A: Datenpin — wie oft kann der ALTE Proxy `(+0x90 & 3)` ueberhaupt feuern? --\n");
    int cells, u13_11c0 = 0, u13_2090 = 0, u13_1190 = 0, u13_1230 = 0;
    part_a("STAGE1/ROOM11C0.RDT", &cells, &u13_11c0);       /* Maggot 0x27 */
    part_a("STAGE2/ROOM2090.RDT", &cells, &u13_2090);       /* Adult Spider 0x25 */
    part_a("STAGE1/ROOM1190.RDT", &cells, &u13_1190);       /* Hund 0x20 */
    part_a("STAGE1/ROOM1230.RDT", &cells, &u13_1230);       /* Hund 0x20 */
    CHECK(u13_11c0 == 0, "ROOM11C0 hat %d Zellen mit u1&3 — der alte Proxy waere dort NICHT tot",
          u13_11c0);
    CHECK(u13_1190 == 0 && u13_1230 == 0,
          "ROOM1190/1230 haben %d/%d Zellen mit u1&3", u13_1190, u13_1230);
    /* ROOM2090 HAT u1&3-Zellen (gemessen: 15) — der alte Proxy war dort nicht tot, meldete aber
     * `blockiert` nur, wenn ZUFAELLIG die zuletzt getroffene Zelle u1&3 trug. */

    printf("\n-- TEIL B: Schreiber — Rueckgabe 1 bei (+0x90 & 3) == 0 --\n");
    part_b("STAGE1/ROOM11C0.RDT", 1600);                    /* Maggot-Radius */
    part_b("STAGE2/ROOM2090.RDT", 1000);                    /* Adult-Spider-Radius */
    part_b("STAGE1/ROOM1190.RDT", 1000);                    /* Hund */

    printf("\n-- TEIL C: LIVE, je Gegner Positiv- (WAND) und Negativ-Fall (FREI) --\n");
    live_t w, fr;

    {   /* MAGGOT 0x27, ROOM11C0 — Verbraucher: das REAR-UP-Gate @0x80117b18
         * (`if (+0x1d6) skip; else if (+0x1e1) skip; else +0x5 = 0xf`). */
        setup_t su = { 1, 3, 0, 0, 0, 0 };
        printf("   [C1] ROOM11C0 Maggot 0x27 — REAR-UP-Gate @0x80117b18\n");
        live_run(RE15_ASSET_PSX_DIR "/STAGE1/ROOM11C0.RDT", 0x11C0, 0x27, &su, 1, 240, 1, &w);
        live_run(RE15_ASSET_PSX_DIR "/STAGE1/ROOM11C0.RDT", 0x11C0, 0x27, &su, 0, 240, 1, &fr);
        CHECK(w.ref_hit > 0,   "C1/WAND: die Kontroll-Messung sieht KEINEN Wandkontakt — der "
                               "Aufbau taugt nicht als Positiv-Fall");
        CHECK(w.hit_new > 0,   "C1/WAND: der Klemm-Rueckgabewert hat NIE gefeuert (ref=%d)", w.ref_hit);
        CHECK(fr.hit_new == 0, "C1/FREI: Rueckgabewert %d mal gesetzt ohne jede Zelle", fr.hit_new);
        CHECK(w.consumer == 0, "C1/WAND: REAR-UP feuerte %dx trotz Wandkontakt (@0x80117b18)",
              w.consumer);
        CHECK(fr.consumer > 0, "C1/FREI: REAR-UP feuerte NIE — der Positiv-Zweig ist unerreichbar, "
                               "der WAND-Nullwert waere dann kein Beweis");
    }

    {   /* ADULT SPIDER 0x25, ROOM2090 — Verbraucher: +0x1dc @0x80111008 (steigt bei Kontakt,
         * wird bei Kontaktfreiheit auf 0 gesetzt); Folge-Leser @0x801111d0/@0x8011144c
         * (`+0x1dc >= 31 -> +0x5=1, +0x6=0xa`). */
        setup_t su = { 1, 0, 0, 0, 1, 0 };
        printf("   [C2] ROOM2090 Adult Spider 0x25 — Steckenbleib-Zaehler +0x1dc @0x80111008\n");
        live_run(RE15_ASSET_PSX_DIR "/STAGE2/ROOM2090.RDT", 0x2090, 0x25, &su, 1, 240, 1, &w);
        live_run(RE15_ASSET_PSX_DIR "/STAGE2/ROOM2090.RDT", 0x2090, 0x25, &su, 0, 240, 1, &fr);
        CHECK(w.ref_hit > 0,   "C2/WAND: die Kontroll-Messung sieht KEINEN Wandkontakt");
        CHECK(w.hit_new > 0,   "C2/WAND: der Klemm-Rueckgabewert hat NIE gefeuert (ref=%d)", w.ref_hit);
        CHECK(fr.hit_new == 0, "C2/FREI: Rueckgabewert %d mal gesetzt ohne jede Zelle", fr.hit_new);
        CHECK(w.consumer_max > 0, "C2/WAND: +0x1dc blieb 0 trotz Wandkontakt (@0x80111008)");
        CHECK(fr.consumer_max == 0, "C2/FREI: +0x1dc stieg auf %d ohne jede Zelle", fr.consumer_max);
    }

    {   /* HUND 0x20, ROOM1190 — RE1.5: +0x1dc @0x8010dd80. */
        setup_t su = { 1, 0, 0, 0, 0, 0 };
        printf("   [C3] ROOM1190 Hund 0x20 RE1.5 — +0x1dc @0x8010dd80\n");
        live_run(RE15_ASSET_PSX_DIR "/STAGE1/ROOM1190.RDT", 0x1190, 0x20, &su, 1, 240, 1, &w);
        live_run(RE15_ASSET_PSX_DIR "/STAGE1/ROOM1190.RDT", 0x1190, 0x20, &su, 0, 240, 1, &fr);
        CHECK(w.ref_hit > 0,   "C3/WAND: die Kontroll-Messung sieht KEINEN Wandkontakt");
        CHECK(w.hit_new > 0,   "C3/WAND: der Klemm-Rueckgabewert hat NIE gefeuert (ref=%d)", w.ref_hit);
        CHECK(fr.hit_new == 0, "C3/FREI: Rueckgabewert %d mal gesetzt ohne jede Zelle", fr.hit_new);
    }

    {   /* HUND 0x20, ROOM1190 — RE2-Flavor: +0x110 Bit 0, Verbraucher der Idle-Wander-Zweig
         * @0x80100618-24 (`+0x110&1` -> +0x6 verlaesst 1). */
        setup_t su = { 1, 0, 1, 0, 0, 1 };
        printf("   [C4] ROOM1190 Hund 0x20 RE2 — +0x110&1 @0x80100618\n");
        live_run(RE15_ASSET_PSX_DIR "/STAGE1/ROOM1190.RDT", 0x1190, 0x20, &su, 1, 240, 1, &w);
        live_run(RE15_ASSET_PSX_DIR "/STAGE1/ROOM1190.RDT", 0x1190, 0x20, &su, 0, 240, 1, &fr);
        CHECK(w.ref_hit > 0,   "C4/WAND: die Kontroll-Messung sieht KEINEN Wandkontakt");
        CHECK(w.hit_new > 0,   "C4/WAND: der Klemm-Rueckgabewert hat NIE gefeuert (ref=%d)", w.ref_hit);
        CHECK(fr.hit_new == 0, "C4/FREI: Rueckgabewert %d mal gesetzt ohne jede Zelle", fr.hit_new);
    }

    printf("\n%s (%d Fehler)\n", fails ? "FEHLGESCHLAGEN" : "OK", fails);
    return fails ? 1 : 0;
}
