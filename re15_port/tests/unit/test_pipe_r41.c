/* test_pipe_r41.c — Runde 41 Spur "pipe" (Nutzer 2026-11-16): die PIPE (Item 2) als Nahkampfwaffe wie das Messer,
 * mit mehr Reichweite, ohne Anzahl. Herleitung/Belege: include/re15_pipe_r41.h, tools/pipe_r41/gen_pipe_plw.py.
 *
 * Der Riegel ruft KEINE neue Port-Funktion — er misst nur Verhalten und Asset-Bytes, damit dieselbe Datei gegen
 * master gebaut werden kann (Gegenprobe: dort ROT in (1), (2), (4)).
 *   (1) BANK: PL00W02/PL04W02.PLW tragen EDD/EMR/TIM der Messerbank W01 byte-gleich, aber ein EIGENES Netz, dessen
 *       Spitze weiter reicht als die Messerspitze (das Rohr). master: W02 == W01 (Mod-Kopie) -> ROT.
 *   (2) REICHWEITE: echter Weg (re15_game_step + Pad, ROOM1140), KI angehalten, Zombie auf festen Abstand, R1+SQUARE:
 *       Messer trifft wie bisher bis 1400..1500 (Kegel 1100 @0x8006E5A4 + Radius 400); das Rohr trifft WEITER,
 *       insbesondere auf dem ersten Abstand, auf dem das Messer NICHT mehr trifft. Die Rohr-Grenze muss zur aus den
 *       Netz-Bytes nachgerechneten Eichung passen (dieselbe Formel wie der Generator). Beide KI-Arten (RE2/RE1.5).
 *   (3) SCHADEN: Zombies tragen im Port die RE2-Zeile (Port-Option 2026-08-20) -> Rohr = Messer = 3 (@0x800A412C,
 *       Zuordnung w2 -> RE2-Id 1); Nicht-Zombies die RE1.5-Zeile, z.B. Hund Messer 5 / Rohr 20 (@0x8006EBD4/D8).
 *   (4) KEINE ANZAHL: Inventar-Gitter mit Rohr x255 + Browning x15 zeigt nur die 2 Browning-Ziffern; das
 *       Ausruest-Feld mit angelegtem Rohr zeigt keine Ziffer; die Kisten-Zeile mit Rohr x100 keine Ziffer.
 *       Gegenprobe: Browning behaelt seine Ziffern (Original-Gate `sltiu id,0x22` @0x80049124).
 *   (5) SPEICHERN/LADEN: Rohr im Inventar + angelegt -> capture -> loeschen -> restore: Platz, Waffe 2, keine Zahl.
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
#include "re15_itembox.h"
#include "re15_inv_screen.h"
#include "re15_menu.h"
#include "re15_fade.h"
#include "re15_msg.h"
#include "re15_savedata.h"
#include "re2_ems.h"

#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef RE15_ASSET_PSX_DIR
#define RE15_ASSET_PSX_DIR "shared_assets/PSX"
#endif

extern void re15_player_aim_reset(void);
extern void re15_player_set_aim_clip_lens(const uint16_t *fcs, int n);
extern int  re15_player_aim_ready(void);
extern int  re15_player_aim_melee_dbg(void);
extern void re15_re2z_onesave_reset(void);

static int fails = 0;
#define CHECK(cond, ...) do { if (!(cond)) { printf("FAIL: " __VA_ARGS__); printf("\n"); fails++; } } while (0)

static uint8_t *slurp(const char *p, size_t *n)
{
    FILE *f = fopen(p, "rb"); if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    if (sz <= 0) { fclose(f); return NULL; }
    uint8_t *b = (uint8_t *)malloc((size_t)sz);
    if (b && fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); b = NULL; }
    fclose(f); if (b) *n = (size_t)sz; return b;
}
static uint32_t rd32(const uint8_t *p) { return (uint32_t)p[0] | ((uint32_t)p[1] << 8) | ((uint32_t)p[2] << 16) | ((uint32_t)p[3] << 24); }
static int16_t  rd16s(const uint8_t *p) { return (int16_t)(p[0] | (p[1] << 8)); }

/* ================================================================================ (1) BANK */
typedef struct { uint8_t *buf; size_t n; uint32_t off[4], len[4]; } plw_t;
static int plw_lesen(const char *name, plw_t *w)
{
    char pfad[512];
    snprintf(pfad, sizeof pfad, "%s/PLD/%s", RE15_ASSET_PSX_DIR, name);
    memset(w, 0, sizeof *w);
    w->buf = slurp(pfad, &w->n);
    if (!w->buf || w->n < 24) return 0;
    uint32_t dir = rd32(w->buf), cnt = rd32(w->buf + 4);
    if (cnt != 4 || dir + 16 > w->n) return 0;
    for (int i = 0; i < 4; i++) w->off[i] = rd32(w->buf + dir + 4 * i);
    for (int i = 0; i < 4; i++) {
        uint32_t ende = (i < 3) ? w->off[i + 1] : dir;
        if (ende < w->off[i] || ende > w->n) return 0;
        w->len[i] = ende - w->off[i];
    }
    return 1;
}
/* groesster Punktabstand zum Bone-11-Ursprung eines MD1-Netzes (mesh m), Punkte von Tri- und Quad-Teil. */
static double md1_spitze(const uint8_t *m, size_t n, int mesh)
{
    const uint8_t *h = m + 12 + mesh * 56;
    double best = 0;
    if ((size_t)(12 + mesh * 56 + 56) > n) return -1;
    for (int teil = 0; teil < 2; teil++) {
        uint32_t vo = rd32(h + (teil ? 28 : 0)), vc = rd32(h + (teil ? 32 : 4));
        for (uint32_t k = 0; k < vc; k++) {
            const uint8_t *p = m + 12 + vo + 8 * k;
            if ((size_t)(p - m) + 8 > n) return -1;
            double x = rd16s(p), y = rd16s(p + 2), z = rd16s(p + 4);
            double d = sqrt(x * x + y * y + z * z);
            if (d > best) best = d;
        }
    }
    return best;
}
static double hand_spitze(const char *fam)
{
    char pfad[512]; size_t n = 0;
    snprintf(pfad, sizeof pfad, "%s/PLD/%s.MD1", RE15_ASSET_PSX_DIR, fam);
    uint8_t *b = slurp(pfad, &n);
    if (!b) return -1;
    double s = md1_spitze(b, n, 11);   /* Netz 11 = rechte Hand (Bone 11) */
    free(b);
    return s;
}

/* Eichung wie tools/pipe_r41/gen_pipe_plw.py: Messer 1100 (@0x8006E5A4) gegen Grund 1000 (@0x8006E5A0). */
static int s_reach_eichung = -1;

static void pruefe_bank(void)
{
    static const char *fam[2] = { "PL00", "PL04" };
    for (int f = 0; f < 2; f++) {
        char n1[32], n2[32];
        snprintf(n1, sizeof n1, "%sW01.PLW", fam[f]);
        snprintf(n2, sizeof n2, "%sW02.PLW", fam[f]);
        plw_t m, r;
        int ok1 = plw_lesen(n1, &m), ok2 = plw_lesen(n2, &r);
        CHECK(ok1 && ok2, "(1) %s/%s nicht lesbar", n1, n2);
        if (!ok1 || !ok2) { free(m.buf); free(r.buf); continue; }
        for (int i = 0; i < 4; i++) {
            if (i == 2) continue;
            CHECK(m.len[i] == r.len[i] && memcmp(m.buf + m.off[i], r.buf + r.off[i], m.len[i]) == 0,
                  "(1) %s Abschnitt %d (%s) ist nicht der der Messerbank %s (PORT-WAHL Vorlage Messer)", n2, i,
                  i == 0 ? "EDD" : i == 1 ? "EMR" : "TIM", n1);
        }
        int netz_gleich = m.len[2] == r.len[2] && memcmp(m.buf + m.off[2], r.buf + r.off[2], m.len[2]) == 0;
        CHECK(!netz_gleich, "(1) %s traegt das MESSER-Netz (Mod-Kopie von W01) — kein Rohr in der Hand", n2);
        double sm = md1_spitze(m.buf + m.off[2], m.len[2], 0);
        double sr = md1_spitze(r.buf + r.off[2], r.len[2], 0);
        double sh = hand_spitze(fam[f]);
        printf("(1) %s: Spitze Hand %.1f | Messer %.1f | Rohr %.1f\n", fam[f], sh, sm, sr);
        CHECK(sr > 1.5 * sm, "(1) %s: Rohrspitze %.1f reicht nicht ueber das Messer %.1f hinaus", n2, sr, sm);
        if (f == 0 && sh > 0 && sm > sh) {
            double k = (1100.0 - 1000.0) / (sm - sh);
            s_reach_eichung = (int)lround(1100.0 + k * (sr - sm));
            printf("(1) Eichung Leon: k = 100/(%.1f-%.1f) = %.4f -> Rohr-Reichweite %d\n", sm, sh, k, s_reach_eichung);
        }
        free(m.buf); free(r.buf);
    }
}

/* ================================================================================ (2)(3) REICHWEITE/SCHADEN */
static re15_rdt_t         s_rdt;
static re15_camera_view_t s_cam;
static re15_game_ctx_t    s_ctx;
/* Messerbank W01 = Rohrbank W02 (EDD byte-gleich, Riegel (1)): 14 Clips, Schlag 7/9/11 = 25/20/20 Bilder. */
static const uint16_t FC_W01[14] = {22,16,52,1,50,30,10,25,1,20,1,20,1,15};

static void frame(uint16_t cur, uint16_t edge)
{
    const unsigned char *raw; int len, id;
    re15_msg_tick(&raw, &len, &id);
    s_ctx.pad_current = cur; s_ctx.pad_pressed = edge;
    re15_game_step(&s_ctx);
}

static uint8_t *s_ems = NULL; static long s_ems_sz = 0;
static re15_enemy_bank_t *load_re2_bank(uint8_t type)
{
    if (!s_ems) { size_t n = 0; s_ems = slurp(RE15_ASSET_PSX_DIR "/../RE2/CDEMD0.EMS", &n); s_ems_sz = (long)n; }
    if (!s_ems) return NULL;
    re15_enemy_bank_t *eb = re15_enemy_find(type);
    if (eb && eb->ok) return eb;
    if (!eb) eb = re15_enemy_alloc(type);
    if (!eb) return NULL;
    if (re2_ems_load_bank(s_ems, (size_t)s_ems_sz, (int)type, eb, NULL) == 0) { eb->buf = NULL; eb->ok = 1; return eb; }
    eb->type = 0; return NULL;
}

static void bringup(void)
{
    re15_actor_init(); re15_aot_init(); scd_vm_init();
    re15_enemy_reset(); re15_enemy_ai_set_paused(0);
    re15_player_cmd_reset(); re15_player_aim_reset();
    re15_damage_seed_rng(0x0badf00du);
    re15_re2z_rng_reset();
    re15_re2z_onesave_reset();
    g_current_room_id = 0x1140;
    if (s_rdt.main_scd)   scd_thread_start(0, s_rdt.main_scd);
    if (s_rdt.sub_scd[0]) scd_thread_start(1, s_rdt.sub_scd[0]);
    g_scd.work_vars[10] = 0;
    for (int i = 0; i < 120; i++) scd_vm_tick();
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100; pl->hit_react = 0;
    pl->state = 0; pl->motion = 0; pl->floor = 0; pl->y = 0;
    re15_collision_set_band(0);
    re15_player_set_aim_clip_lens(FC_W01, 14);
}

static int standing_zombie(void)
{
    for (int s = 1; s < RE15_ACTOR_MAX; s++)
        if (g_actors[s].active && g_actors[s].type >= 0x10 && g_actors[s].type <= 0x18 && !(g_actors[s].grid_id & 0x80))
            return s;
    return -1;
}

/* Ein Schlag aus dem Stand auf Abstand d. Rueckgabe: HP-Abzug (0 = kein Treffer), -1 = Fehllauf. */
typedef struct { int slot; int melee; } sweep_ctx_t;
static int schlag(sweep_ctx_t *sc, int d)
{
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    re15_actor_t *e  = &g_actors[sc->slot];
    pl->x = e->x - d; pl->hp = 100;
    re15_player_cmd_reset(); re15_player_aim_reset();
    e->hp = 200; e->hit_react = 0; e->re2z_self1d3 = 0;
    e->state = 1; e->sub_state_1 = 1; e->sub_state_2 = 1;
    for (int w = 0; w < 120 && !re15_player_aim_ready(); w++) frame(RE15_PAD_BIT_R1, 0);
    sc->melee = re15_player_aim_melee_dbg();
    int hp0 = e->hp;
    for (int k = 0; k < 40; k++) {
        e->hit_react = 0; e->re2z_self1d3 = 0;
        frame((uint16_t)(RE15_PAD_BIT_R1 | RE15_PAD_BIT_SQUARE), (uint16_t)(k == 0 ? RE15_PAD_BIT_SQUARE : 0));
        if (e->hp < hp0) return hp0 - e->hp;
    }
    return 0;
}

typedef struct { int letzter, erster, schaden_nah, melee; uint8_t treffer[40]; } sweep_t;
static void sweep(int waffe, int re2, sweep_t *out)
{
    memset(out, 0, sizeof *out); out->letzter = -1; out->erster = -1;
    re15_ai_flavor_set(re2 ? RE15_AI_FLAVOR_RE2 : RE15_AI_FLAVOR_RE15);
    bringup();
    re15_inv_load_briefing();
    re15_player_set_equipped_weapon(waffe);
    for (int f = 0; f < 60; f++) { g_actors[RE15_ACTOR_SLOT_PLAYER].hp = 100; frame(0, 0); }
    load_re2_bank(0x10);
    sweep_ctx_t sc; sc.slot = standing_zombie(); sc.melee = 0;
    if (sc.slot < 0) { printf("FEHLLAUF: kein stehender Zombie\n"); fails++; return; }
    for (int s = 1; s < RE15_ACTOR_MAX; s++) if (s != sc.slot) g_actors[s].active = 0;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    re15_actor_t *e  = &g_actors[sc.slot];
    re15_enemy_ai_set_paused(1);
    pl->rot_y = 1024; pl->y = e->y; pl->z = e->z;
    for (int i = 0, d = 400; d <= 3600; d += 100, i++) {
        int dmg = schlag(&sc, d);
        out->treffer[i] = (uint8_t)(dmg > 0);
        if (dmg > 0) { if (out->erster < 0) { out->erster = d; out->schaden_nah = dmg; } out->letzter = d; }
    }
    out->melee = sc.melee;
    re15_enemy_ai_set_paused(0);
    printf("(2) Waffe %d %s: Treffer %d..%d, Schaden nah %d, Nahkampf-Klasse %d\n", waffe, re2 ? "RE2-KI  " : "RE1.5-KI",
           out->erster, out->letzter, out->schaden_nah, out->melee);
}

static void pruefe_reichweite(void)
{
    for (int re2 = 1; re2 >= 0; re2--) {
        sweep_t m, r;
        sweep(1, re2, &m);
        sweep(2, re2, &r);
        const char *ki = re2 ? "RE2-KI" : "RE1.5-KI";
        CHECK(m.melee == 1 && r.melee == 1, "(2 %s) Klassen-Latch: Messer %d Rohr %d (beide Nahkampf @0x80074034/38)",
              ki, m.melee, r.melee);
        CHECK(m.letzter >= 1400 && m.letzter <= 1500, "(2 %s) Messer unveraendert erwartet 1400..1500 (Kegel 1100 "
              "@0x8006E5A4 + Radius 400), ist %d", ki, m.letzter);
        CHECK(r.erster == 400, "(2 %s) Rohr trifft aus der Naehe nicht (erster Treffer %d)", ki, r.erster);
        CHECK(r.letzter > m.letzter, "(2 %s) Rohr reicht %d, Messer %d — keine groessere Reichweite", ki, r.letzter,
              m.letzter);
        int i_m = (m.letzter + 100 - 400) / 100;
        CHECK(m.letzter >= 0 && i_m < 40 && !m.treffer[i_m] && r.treffer[i_m],
              "(2 %s) auf d=%d trifft das Messer NICHT und das Rohr %s", ki, m.letzter + 100,
              (i_m < 40 && r.treffer[i_m]) ? "schon" : "auch nicht");
        if (s_reach_eichung > 0) {
            /* Kegel: Treffer bei dist < Reichweite + Radius 400 (FUN_800127FC, strikt). Der Sweep-Raster ist 100. */
            int grenze = s_reach_eichung + 400;
            CHECK(r.letzter < grenze && r.letzter + 100 >= grenze,
                  "(2 %s) Rohr-Grenze %d passt nicht zur Netz-Eichung %d + Radius 400 = %d", ki, r.letzter,
                  s_reach_eichung, grenze);
        }
        /* Zombies tragen im Port in BEIDEN KI-Arten die RE2-Schadenszeile (Port-Option 2026-08-20, re15_damage.c
         * re15_enemy_dmg_row); dort steht das Rohr auf der Messer-Zeile r1 = 3 (@0x800A412C, PORT-ZUORDNUNG
         * enemy_ai_re2_zombie.c). Gemessen am echten Treffer: Rohr == Messer. */
        CHECK(r.schaden_nah == m.schaden_nah && m.schaden_nah == 3,
              "(3 %s) Zombie-Schaden Rohr %d / Messer %d, erwartet beide 3 (RE2-Zeile @0x800A412C)", ki,
              r.schaden_nah, m.schaden_nah);
    }
    /* (3b) Nicht-Zombies behalten die RE1.5-Zeile: Hund 0x20 @0x8006EBD0 = {0, 5, 20, ..} -> Messer 5, Rohr 20. */
    {
        extern int re15_enemy_waffen_schaden(const re15_actor_t *e, int weapon_id);
        re15_actor_t hund; memset(&hund, 0, sizeof hund); hund.type = 0x20;
        re15_ai_flavor_set(RE15_AI_FLAVOR_RE15);
        int s1 = re15_enemy_waffen_schaden(&hund, 1), s2 = re15_enemy_waffen_schaden(&hund, 2);
        printf("(3) Hund RE1.5-Zeile: Messer %d, Rohr %d\n", s1, s2);
        CHECK(s1 == 5 && s2 == 20, "(3) Hund: Messer %d / Rohr %d statt 5 / 20 (@0x8006EBD4 / @0x8006EBD8)", s1, s2);
    }
}

/* ================================================================================ (4)(5) KEINE ANZAHL */
static void mframe(uint16_t pressed, uint16_t held)
{
    re15_menu_start_poll(pressed, 1);
    if (re15_menu_gameplay_frozen()) re15_menu_fsm_tick(pressed, held);
    if (re15_menu_is_open()) re15_inv_screen_ecg_tick();
    re15_fade_tick();
}
static void midle(int n) { while (n-- > 0) mframe(0, 0); }
static re15_inv_op_t s_ops[RE15_INV_MAX_OPS];
static int ist_ziffer(const re15_inv_op_t *o)
{
    return o->kind == RE15_INV_OP_SPRT && o->page == RE15_INV_PAGE_TEX4 && o->w == 8 && o->h == 8 && o->v == 0xf0;
}
static int ziffern_zaehlen(int x_min, int x_max)
{
    int n = re15_inv_screen_build(&g_inv_screen, s_ops, RE15_INV_MAX_OPS), z = 0;
    for (int i = 0; i < n; i++)
        if (ist_ziffer(&s_ops[i]) && s_ops[i].x >= x_min && s_ops[i].x <= x_max) z++;
    return z;
}

static void inventar_belegen(void)
{
    re15_inv_init();
    g_inv.slots[0].id = 0x02; g_inv.slots[0].qty = 255;   /* Rohr wie aus dem Item-Debug (Menge 0xff @0x8004a1f8) */
    g_inv.slots[1].id = 0x03; g_inv.slots[1].qty = 15;    /* Browning x15 (Starttabelle @0x80074bd0[1] = 0x0f) */
}

static void pruefe_anzahl(void)
{
    /* (4a) Gitter, nichts angelegt. */
    inventar_belegen();
    re15_inv_set_equipped_slot(0x80); re15_inv_set_prev_equip_slot(0x80);
    re15_menu_toggle();
    mframe(RE15_PAD_BIT_SQUARE, RE15_PAD_BIT_SQUARE);   /* Reiter ITEM */
    midle(8);
    int z = ziffern_zaehlen(0, 400);
    printf("(4) Gitter Rohr x255 + Browning x15: %d Ziffern\n", z);
    CHECK(z == 2, "(4) Gitter zeigt %d Ziffern; erwartet 2 (nur Browning '15', das Rohr ohne Zahl)", z);
    re15_menu_toggle(); midle(2);

    /* (4b) Rohr angelegt: Ausruest-Feld ohne Zahl. */
    inventar_belegen();
    re15_inv_set_equipped_slot(0); re15_inv_set_prev_equip_slot(0x80);
    re15_menu_toggle();
    mframe(RE15_PAD_BIT_SQUARE, RE15_PAD_BIT_SQUARE);
    midle(8);
    re15_inv_screen_sync_equip();
    z = ziffern_zaehlen(0, 400);
    printf("(4) Rohr angelegt: %d Ziffern (Gitter + Ausruest-Feld)\n", z);
    CHECK(z == 2, "(4) mit angelegtem Rohr %d Ziffern; erwartet 2 (nur Browning)", z);
    re15_menu_toggle(); midle(2);

    /* (4c) Gegenprobe: Browning angelegt -> Ausruest-Feld zeigt seine 2 Ziffern zusaetzlich. */
    inventar_belegen();
    re15_inv_set_equipped_slot(1); re15_inv_set_prev_equip_slot(0x80);
    re15_menu_toggle();
    mframe(RE15_PAD_BIT_SQUARE, RE15_PAD_BIT_SQUARE);
    midle(8);
    re15_inv_screen_sync_equip();
    z = ziffern_zaehlen(0, 400);
    printf("(4) Gegenprobe Browning angelegt: %d Ziffern\n", z);
    CHECK(z == 4, "(4) Gegenprobe: Browning angelegt zeigt %d Ziffern; erwartet 4 (Gitter + Ausruest-Feld)", z);
    re15_menu_toggle(); midle(2);

    /* (4d) Kisten-Zeile: Rohr x100 in der Kiste ohne Zahl, Browning x100 mit 3 Ziffern. */
    for (int welches = 0; welches < 2; welches++) {
        re15_inv_init();
        re15_itembox_init();
        g_itembox.slots[RE15_BOX_PICK_ROW].id  = welches ? 0x03 : 0x02;
        g_itembox.slots[RE15_BOX_PICK_ROW].qty = 100;
        re15_menu_toggle_box();
        midle(1);
        mframe(RE15_PAD_BIT_SQUARE, RE15_PAD_BIT_SQUARE);
        midle(4);
        z = ziffern_zaehlen(0, 214);
        printf("(4) Kiste %s x100: %d Ziffern\n", welches ? "Browning" : "Rohr", z);
        if (welches) CHECK(z == 3, "(4) Gegenprobe Kiste Browning x100: %d Ziffern statt 3", z);
        else         CHECK(z == 0, "(4) Kiste Rohr x100 zeigt %d Ziffern; erwartet keine", z);
        if (re15_menu_is_open()) re15_menu_toggle();
        midle(4);
    }
}

static void pruefe_speichern(void)
{
    inventar_belegen();
    re15_player_set_equipped_weapon(2);
    re15_inv_set_equipped_slot(0);
    re15_savedata_t sd;
    re15_savedata_capture(&sd, 0, 1);
    re15_inv_init();
    re15_player_set_equipped_weapon(1);
    re15_inv_set_equipped_slot(0x80);
    uint16_t raum = 0;
    int rc = re15_savedata_restore(&sd, &raum);
    printf("(5) nach Laden: Platz 0 = Item 0x%02X x%u, Waffe %d, Ausruest-Platz 0x%02X\n", g_inv.slots[0].id,
           g_inv.slots[0].qty, re15_player_equipped_weapon(), re15_inv_equipped_slot());
    CHECK(rc == 0, "(5) restore rc=%d", rc);
    CHECK(g_inv.slots[0].id == 0x02, "(5) Rohr nach dem Laden nicht in Platz 0 (Item 0x%02X)", g_inv.slots[0].id);
    CHECK(re15_player_equipped_weapon() == 2, "(5) Waffe nach dem Laden %d statt 2", re15_player_equipped_weapon());
    CHECK(re15_inv_equipped_slot() == 0, "(5) Ausruest-Platz 0x%02X statt 0", re15_inv_equipped_slot());
    re15_menu_toggle();
    mframe(RE15_PAD_BIT_SQUARE, RE15_PAD_BIT_SQUARE);
    midle(8);
    re15_inv_screen_sync_equip();
    int z = ziffern_zaehlen(0, 400);
    CHECK(z == 2, "(5) nach dem Laden %d Ziffern statt 2 (Rohr ohne Zahl)", z);
    re15_menu_toggle(); midle(2);
}

int main(void)
{
    char path[600];
    const char *base = getenv("RE15_ASSET_DIR");
    pruefe_bank();
    snprintf(path, sizeof path, "%s/STAGE1/ROOM1140.RDT", (base && *base) ? base : RE15_ASSET_PSX_DIR);
    size_t sz = 0; uint8_t *buf = slurp(path, &sz);
    if (!buf || re15_rdt_parse(buf, sz, &s_rdt) != 0) { printf("FAIL: %s\n", path); return 1; }
    memset(&s_cam, 0, sizeof s_cam); memset(&s_ctx, 0, sizeof s_ctx);
    s_ctx.rdt = &s_rdt; s_ctx.rdt_ok = 1; s_ctx.cam_view = &s_cam; s_ctx.active_cut = 0;
    pruefe_reichweite();
    pruefe_anzahl();
    pruefe_speichern();
    free(buf);
    if (fails) { printf("RIEGEL-ROT: %d Fehler\n", fails); return 1; }
    printf("RIEGEL-GRUEN: Rohr = Nahkampf mit groesserer Reichweite, ohne Anzahl, speicherbar\n");
    return 0;
}
