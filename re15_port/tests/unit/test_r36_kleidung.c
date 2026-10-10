/* test_r36_kleidung.c — Runde 36 Spur B "kleidung": Leons zerrissene/blutige Kleidung
 * (Blut-Decal-Wundsystem, analysis/blood_decals.md; Dossier analysis/befunde_runde36/B_kleidung.md).
 *
 * Nutzer-Wortlaut: "Aus irgendeinen Grund hat Leons Kleidung jetzt keine Blutflecken/Risse mehr,
 * wenn er auf Warning/Danger runter kommt."
 *
 * Original-Mechanik (RE1.5 PSX.EXE, blood_decals.md §2/§3, selbst gegengelesen):
 *   FUN_80037edc(panel, amount): Akku += amount (@0x80037f04-08); ab 0x78=120 (@0x80037f14 sltiu)
 *   Stempel: Level++ (@0x80037f2c), Clamp 2 (@0x80037f40/50), DR_MOVE-Blit in die OT (@0x80037f7c-fc4).
 *   Trigger beim Zombie-Griff = Phase 4 der SPIELER-Opfer-FSM (cmd 5, 0x8010a2cc): Dispatcher
 *   @0x8010a580-5b0 `lbu aca59 / jalr 0x801201b8[dir]` -> Helper 0x8010a1cc (dir 0: (0,10),(5,50),(7,50))
 *   / 0x8010a208 (dir 1: (0,10),(4,50),(7,50)) / 0x8010a244 (dir 2: (1,50)) / 0x8010a268 (dir 3: (2,50)).
 *
 * Teile:
 *   griff <re2|re15> [budget]  — ROOM1140 (Briefing-Raum) auf dem ECHTEN game_step-Weg: Leon steht
 *                                neben einem Fresser, wird gegriffen, ueberlebt (HP wird aufgefuellt),
 *                                der Griff wird abgeschuettelt. Gemessen: Griffe, Abwuerfe, Wund-
 *                                Tabelle (Level/Akku) nach jedem Abwurf.
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
#include "re2_ems.h"
#include "re15_ems.h"
#include "re15_esp.h"
#include "re15_kleidung_r36.h"

#ifndef RE15_ASSET_PSX_DIR
#define RE15_ASSET_PSX_DIR "shared_assets/PSX"
#endif

extern void re15_player_aim_reset(void);
extern void re15_player_set_aim_clip_len(int fc);
extern int  re15_player_victim_state(void);

static re15_rdt_t         s_rdt;
static re15_camera_view_t s_cam;
static re15_game_ctx_t    s_ctx;
static uint8_t           *s_ems = NULL; static size_t s_ems_n = 0;
static int                s_fail = 0;

#define CHECK(cond, ...) do { if (!(cond)) { printf("FAIL: "); printf(__VA_ARGS__); \
                              printf("\n"); s_fail++; } } while (0)

static uint8_t *slurp(const char *p, size_t *n)
{
    FILE *f = fopen(p, "rb"); if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    if (sz <= 0) { fclose(f); return NULL; }
    uint8_t *b = (uint8_t *)malloc((size_t)sz);
    if (b && fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); b = NULL; }
    fclose(f); if (b) *n = (size_t)sz; return b;
}

static void load_bank2(uint8_t type)
{
    if (!s_ems) s_ems = slurp(RE15_ASSET_PSX_DIR "/../RE2/CDEMD0.EMS", &s_ems_n);
    if (!s_ems) return;
    re15_enemy_bank_t *eb = re15_enemy_find(type);
    if (eb && eb->ok) return;
    if (!eb) eb = re15_enemy_alloc(type);
    if (!eb) return;
    if (re2_ems_load_bank(s_ems, s_ems_n, (int)type, eb, NULL) == 0) { eb->buf = NULL; eb->ok = 1; }
    else eb->type = 0;
}

/* RE1.5-Bank (EMD/CDEMD0.EMS des CD-Baums) wie probe_dogwound_zknock.c */
static uint8_t *s_blob15[64];
static void load_bank15(uint8_t type)
{
    const char *base = getenv("RE15_ASSET_DIR");
    char p[600]; size_t n = 0;
    snprintf(p, sizeof p, "%s/EMD/CDEMD0.EMS", (base && *base) ? base : RE15_ASSET_PSX_DIR);
    uint8_t *ems = slurp(p, &n);
    if (!ems) return;
    int idx = re15_ems_index_for_type(type);
    size_t off = 0, len = 0;
    if (idx < 0 || re15_ems_get_entry(ems, n, idx, &off, &len) != 0) { free(ems); return; }
    re15_enemy_bank_t *eb = re15_enemy_find(type);
    if (eb && eb->ok) { free(ems); return; }
    if (!eb) eb = re15_enemy_alloc(type);
    if (!eb) { free(ems); return; }
    uint8_t *blob = (uint8_t *)malloc(len); memcpy(blob, ems + off, len); free(ems);
    if (type < 64) { free(s_blob15[type]); s_blob15[type] = blob; }
    if (re15_emd_parse_container(blob, len, &eb->md1, &eb->skel, &eb->anim, NULL) != 0) return;
    eb->ok = 1; eb->buf = NULL;
    /* Opfer-Bank (Bank 2 = Leons Griff-Clips) wie der Plattform-Lader (main.c:1278) */
    eb->victim_ok = (re15_emd_parse_victim_bank(blob, len, &eb->skel_victim, &eb->anim_victim) == 0);
    eb->loco_ok   = (re15_emd_parse_loco_bank(blob, len, &eb->skel_loco, &eb->anim_loco) == 0);
}

static void frame(uint16_t cur, uint16_t edge)
{
    const unsigned char *raw; int len, id;
    re15_msg_tick(&raw, &len, &id);
    s_ctx.pad_current = cur; s_ctx.pad_pressed = edge;
    re15_game_step(&s_ctx);
}

static int is_zombie(uint8_t t) { return t == 0x10 || t == 0x11 || t == 0x16; }

static void print_wounds(const char *tag)
{
    printf("%s wunden:", tag);
    for (int p = 0; p < 8; p++) printf(" p%d=L%d", p, re15_wound_level(p));
    printf("  gen=%d\n", re15_wound_generation());
}

/* ---------------------------------------------------------------------------------------------- */
static int teil_griff(const char *flavor, int budget)
{
    const char *base = getenv("RE15_ASSET_DIR");
    char path[600];
    snprintf(path, sizeof path, "%s/STAGE1/ROOM1140.RDT",
             (base && *base) ? base : RE15_ASSET_PSX_DIR);
    size_t sz = 0; uint8_t *buf = slurp(path, &sz);
    if (!buf) { printf("SKIP: %s nicht lesbar\n", path); return 77; }
    if (re15_rdt_parse(buf, sz, &s_rdt) != 0) { printf("FAIL: RDT-Parse\n"); return 1; }

    int re2 = (flavor && flavor[0] == 'r' && flavor[2] == '2');
    memset(&s_cam, 0, sizeof s_cam); memset(&s_ctx, 0, sizeof s_ctx);
    s_ctx.rdt = &s_rdt; s_ctx.rdt_ok = 1; s_ctx.cam_view = &s_cam; s_ctx.active_cut = 0;

    re15_ai_flavor_set(re2 ? RE15_AI_FLAVOR_RE2 : RE15_AI_FLAVOR_RE15);
    re15_actor_init(); re15_aot_init(); scd_vm_init();
    re15_enemy_reset(); re15_enemy_ai_set_paused(0);
    re15_player_cmd_reset(); re15_player_aim_reset();
    re15_damage_seed_rng(0x0badf00du);
    re15_wound_reset();
    g_current_room_id = 0x1140;
    if (s_rdt.main_scd)   scd_thread_start(0, s_rdt.main_scd);
    if (s_rdt.sub_scd[0]) scd_thread_start(1, s_rdt.sub_scd[0]);
    g_scd.work_vars[10] = 0;
    for (int i = 0; i < 120; i++) scd_vm_tick();
    if (re2) {
        load_bank2(0x10); load_bank2(0x11); load_bank2(0x16);
        re15_enemy_bank_t *eb = re15_enemy_find(0x16);
        if (!eb || !eb->ok) { printf("SKIP: RE2-Bank EM016 fehlt\n"); return 77; }
    } else {
        load_bank15(0x10); load_bank15(0x11); load_bank15(0x16);
    }

    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100; pl->hit_react = 0;
    pl->state = 0; pl->motion = 0; pl->floor = 0; pl->y = 0;
    re15_collision_set_band(0);
    re15_player_set_aim_clip_len(12);
    re15_inv_load_briefing();

    int zs[RE15_ACTOR_MAX], nz = 0;
    for (int s = 1; s < RE15_ACTOR_MAX; s++)
        if (g_actors[s].active && is_zombie(g_actors[s].type)) zs[nz++] = s;
    if (nz == 0) { printf("FAIL: keine Zombies in ROOM1140 gespawnt\n"); return 1; }
    /* Greifer: der erste STEHENDE Zombie (Typ 0x10/0x11; Slot 1 ist der Liegende 0x16 des
     * Deskriptors 0x88, der im RE1.5-Flavor vom Boden aus beisst und frisst) */
    int zA = -1;
    for (int i = 0; i < nz; i++) {
        re15_actor_t *e = &g_actors[zs[i]];
        printf("  Zombie Slot %d Typ 0x%02X state %d sub %d pos (%d,%d,%d)\n", zs[i], e->type,
               e->state, e->sub_state_1, (int)e->x, (int)e->y, (int)e->z);
        if (zA < 0 && e->type != 0x16) zA = zs[i];
    }
    if (zA < 0) zA = zs[0];
    /* alle anderen Zombies schlafen legen, damit genau EIN Greifer misst */
    for (int i = 0; i < nz; i++) if (zs[i] != zA) g_actors[zs[i]].active = 0;
    re15_actor_t *z0 = &g_actors[zA];
    printf("ROOM1140 %s: %d Zombies, Greifer Slot %d Typ 0x%02X (state %d sub %d)\n",
           re2 ? "RE2" : "RE1.5", nz, zA, z0->type, z0->state, z0->sub_state_1);

    int grabs = 0, releases = 0, prev_v = 0, prev_s1 = -1, first_grab = -1;
    int min_hp = 100;
    int lv_nach[5][8]; memset(lv_nach, 0, sizeof lv_nach);   /* Level je Panel nach Abwurf 1..4 */
    int gen_nach[5]; memset(gen_nach, 0, sizeof gen_nach);
    for (int f = 0; f < budget; f++) {
        int v = re15_player_victim_state();
        if (v == 0 && !(pl->hit_react & 1)) {               /* frei: neben den Greifer stellen */
            pl->x = z0->x + 400; pl->z = z0->z; pl->y = z0->y; pl->floor = z0->floor;
            pl->rot_y = z0->rot_y;                            /* Ruecken zum Zombie -> Griff von hinten/vorn je Bahn */
        }
        if (pl->hp < min_hp) min_hp = pl->hp;
        if (pl->hp < 60) pl->hp = 100;                        /* ueberleben: kein Devour, kein One-Save */
        /* Befreiung wie der Nutzer: Knopf-Hammern im Griff (Kante alle 2 Takte; RE1.5 +0x9c -= 1+5*mash
         * @FUN_80037024, RE2 +0x158 -= 2+5*mash @0x80102868-7C) */
        uint16_t edge = (v == 1 && (f & 1)) ? (uint16_t)RE15_PAD_BIT_CROSS : 0u;
        frame(edge, edge);
        v = re15_player_victim_state();
        if (getenv("R36B_TRACE")) {
            static int lv = -1, ls = -1, lhp = -999, lm = -1;
            if (v != lv || pl->state != ls || pl->hp != lhp || pl->motion != lm) {
                printf("    F%d victim=%d pl.state=%d sub=%d hp=%d motion=%d hit_react=%02x | z0 state=%d sub=%d/%d\n",
                       f, v, pl->state, pl->sub_state_1, pl->hp, pl->motion, pl->hit_react,
                       z0->state, z0->sub_state_1, z0->sub_state_2);
                lv = v; ls = pl->state; lhp = pl->hp; lm = pl->motion;
            }
        }
        if (z0->state == 1 && z0->sub_state_1 == 3 && prev_s1 != 3) { grabs++; if (first_grab < 0) first_grab = f; }
        prev_s1 = (z0->state == 1) ? (int)z0->sub_state_1 : -1;
        if (v == 3 && prev_v != 3) {
            releases++;
            char tag[64]; snprintf(tag, sizeof tag, "  F%d Abwurf #%d (hp=%d)", f, releases, pl->hp);
            print_wounds(tag);
            if (releases <= 4) {
                for (int p = 0; p < 8; p++) lv_nach[releases][p] = re15_wound_level(p);
                gen_nach[releases] = re15_wound_generation();
            }
        }
        prev_v = v;
        if (releases >= 4) break;
    }
    printf("ERGEBNIS %s: Griffe=%d Abwuerfe=%d erster Griff F%d min_hp=%d\n",
           re2 ? "RE2" : "RE1.5", grabs, releases, first_grab, min_hp);
    print_wounds("  Ende");
    int any = 0; for (int p = 0; p < 8; p++) if (re15_wound_level(p) > 0) any = 1;
    printf("  Stempel sichtbar (ein Panel Level>0): %s\n", any ? "JA" : "NEIN");

    /* RIEGEL — die Mechanik des Originals (blood_decals.md §3.4, Savestate-verifiziert: drei
     * ueberlebte Griffe = 3x50 = 150 >= 120 auf den beiden 50er-Panels des Helpers, das 10er-Panel 0
     * bleibt bei 30): Abwurf 1 und 2 ohne Stempel, Abwurf 3 stempelt Panel 7 UND (4 oder 5) auf
     * Stufe 1 (Richtung = Helper 0x8010a1cc vorn (5,7) / 0x8010a208 hinten (4,7)), Panel 0 bleibt 0. */
    CHECK(releases >= 3, "weniger als 3 ueberlebte Griffe im Budget (%d)", releases);
    if (releases >= 3) {
        for (int p = 0; p < 8; p++) {
            CHECK(lv_nach[1][p] == 0, "Abwurf 1: Panel %d schon Stufe %d (2x50 < 120)", p, lv_nach[1][p]);
            CHECK(lv_nach[2][p] == 0, "Abwurf 2: Panel %d schon Stufe %d (2x50 < 120)", p, lv_nach[2][p]);
        }
        CHECK(lv_nach[3][7] == 1, "Abwurf 3: Panel 7 muss Stufe 1 sein (ist %d) — (7,+50) in 0x8010a1cc/0x8010a208", lv_nach[3][7]);
        CHECK((lv_nach[3][4] == 1) != (lv_nach[3][5] == 1),
              "Abwurf 3: genau EINES von Panel 4 (hinten) / 5 (vorn) Stufe 1 (ist 4=%d 5=%d)", lv_nach[3][4], lv_nach[3][5]);
        CHECK(lv_nach[3][0] == 0, "Abwurf 3: Panel 0 bleibt 0 (3x10 = 30 < 120), ist %d", lv_nach[3][0]);
        for (int p = 1; p <= 3; p++) CHECK(lv_nach[3][p] == 0, "Abwurf 3: Kriecher-Panel %d unberuehrt", p);
        CHECK(lv_nach[3][6] == 0, "Abwurf 3: Panel 6 unberuehrt (kein Helper schreibt es)");
        CHECK(gen_nach[3] > gen_nach[2], "Abwurf 3: Generation muss springen (Sync-Marke fuer den Renderer)");
    }
    re15_ai_flavor_set(RE15_AI_FLAVOR_RE15);
    return 0;
}

/* ---------------------------------------------------------------------------------------------- */
/* karte <datei> <raum-hex> <char 0|4> <x> <y> <z> <rot> <hp> <wundspec> [cut]
 * wundspec: "-" = keine Wunden | "alle:N" = alle 8 Panels Level N | "5:1,7:1" = je Panel:Level.
 * Schreibt Slot 0 einer PSX-Karte (v9-Spielstand mit wounds[8][2], re15_savedata.c:212) — der
 * Bild-Riegel laedt sie per CONTINUE und misst die Textur. */
#include "re15_savedata.h"
#include "re15_memcard.h"
#include "re15_gameflow.h"
static int teil_karte(int argc, char **argv)
{
    if (argc < 11) { printf("karte <datei> <raum-hex> <char> <x> <y> <z> <rot> <hp> <wundspec>\n"); return 2; }
    const char *path = argv[2];
    unsigned room = (unsigned)strtoul(argv[3], NULL, 16);
    int chr = atoi(argv[4]);
    scd_vm_init(); re15_actor_init(); re15_aot_init();
    re15_wound_reset();
    g_current_room_id = (int)room;
    g_gameflow.character = (uint8_t)chr;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0;
    pl->x = atoi(argv[5]); pl->y = atoi(argv[6]); pl->z = atoi(argv[7]);
    pl->rot_y = (int16_t)atoi(argv[8]); pl->hp = (int16_t)atoi(argv[9]);
    re15_inv_load_briefing();
    g_scd.cam_id = (uint8_t)((argc > 11) ? atoi(argv[11]) : 0);   /* Kamera-Cut des Spielstands */
    const char *w = argv[10];
    if (strncmp(w, "alle:", 5) == 0) {
        int lv = atoi(w + 5);
        for (int p = 0; p < 8; p++) for (int k = 0; k < lv; k++) re15_wound_add(p, 200);
    } else if (strcmp(w, "-") != 0) {
        const char *q = w;
        while (*q) {
            int p = atoi(q); const char *c = strchr(q, ':'); if (!c) break;
            int lv = atoi(c + 1);
            for (int k = 0; k < lv; k++) re15_wound_add(p, 200);
            const char *k2 = strchr(q, ','); if (!k2) break; q = k2 + 1;
        }
    }
    re15_savedata_t sd; re15_savedata_capture(&sd, 0, 1);
    if (re15_memcard_save(path, 0, &sd, chr ? "ELZA  KLEID" : "LEON  KLEID") != 0) {
        printf("FAIL: Karte %s nicht schreibbar\n", path); return 1;
    }
    printf("Karte %s: Raum %04X char %d pos (%d,%d,%d) rot %d hp %d wunden", path, room, chr,
           (int)pl->x, (int)pl->y, (int)pl->z, (int)pl->rot_y, (int)pl->hp);
    for (int p = 0; p < 8; p++) printf(" %u", sd.wounds[p][0]);
    printf("\n");
    return 0;
}

/* ---------------------------------------------------------------------------------------------- */
/* nullen <datei> [slot] — liest einen ECHTEN Spielstand (vom Spiel selbst geschrieben, z.B. nach
 * ueberlebten Griffen auf dem Spielweg), setzt NUR die Wund-Tabelle (v5 wounds[8][2]) auf 0,
 * rechnet die Pruefsumme neu und schreibt ihn in denselben Slot zurueck. Liefert die Referenz
 * "derselbe Spielstand ohne Wunden" fuer den Bildvergleich (Dossier §5.3): Position, Blickrichtung,
 * HP, Kamera-Cut und Inventar bleiben bytegleich. Gibt die gelesenen Wunden aus (Beleg, dass der
 * Spielweg sie erzeugt hat). Exit 0 = ok, 1 = Karte leer/unlesbar, 3 = keine Wunde im Spielstand. */
static int teil_nullen(int argc, char **argv)
{
    if (argc < 3) { printf("nullen <datei> [slot]\n"); return 2; }
    const char *path = argv[2];
    int slot = (argc > 3) ? atoi(argv[3]) : 0;
    re15_savedata_t sd;
    if (re15_memcard_load(path, slot, &sd) != 0) { printf("FAIL: Karte %s Slot %d nicht lesbar\n", path, slot); return 1; }
    int summe = 0;
    printf("Spielstand %s Slot %d: Raum %04X char %d pos (%d,%d,%d) rot %d hp %d cut %d wunden(level/akku)",
           path, slot, (unsigned)sd.room, (int)sd.character, (int)sd.player_x, (int)sd.player_y,
           (int)sd.player_z, (int)sd.player_rot, (int)sd.player_hp, (int)sd.camera_cut);
    for (int p = 0; p < 8; p++) { printf(" %u/%u", sd.wounds[p][0], sd.wounds[p][1]); summe += sd.wounds[p][0]; }
    printf("\n");
    memset(sd.wounds, 0, sizeof sd.wounds);
    sd.checksum = re15_savedata_checksum(&sd);
    if (re15_memcard_save(path, slot, &sd, "LEON  NULL ") != 0) { printf("FAIL: Karte %s nicht schreibbar\n", path); return 1; }
    printf("Karte %s Slot %d: Wunden genullt, Pruefsumme neu\n", path, slot);
    return summe ? 0 : 3;
}

/* ---------------------------------------------------------------------------------------------- */
/* bild <a.ppm> <b.ppm> <min_diff> <x0> <y0> <x1> <y1> — zaehlt die Pixel, die sich zwischen zwei
 * Framedumps (P6 320x240) unterscheiden. min_diff = 0: es darf KEIN Pixel abweichen (Determinismus);
 * sonst: mindestens min_diff Pixel weichen ab und ALLE Abweichungen liegen im Kasten [x0,x1)x[y0,y1)
 * (= die Figur; die Kulisse bleibt unberuehrt). Exit 0 = Bedingung erfuellt. */
static unsigned char *ppm_lesen(const char *path, int *w, int *h)
{
    size_t n = 0; unsigned char *d = slurp(path, &n);
    if (!d) return NULL;
    int maxv = 0, pos = 0;
    if (sscanf((const char *)d, "P6 %d %d %d%n", w, h, &maxv, &pos) != 3) { free(d); return NULL; }
    pos++;                                                /* das eine Whitespace nach maxval */
    if ((size_t)pos + (size_t)(*w) * (size_t)(*h) * 3 > n) { free(d); return NULL; }
    unsigned char *px = (unsigned char *)malloc((size_t)(*w) * (size_t)(*h) * 3);
    memcpy(px, d + pos, (size_t)(*w) * (size_t)(*h) * 3); free(d);
    return px;
}
static int teil_bild(int argc, char **argv)
{
    if (argc < 9) { printf("bild <a.ppm> <b.ppm> <min_diff> <x0> <y0> <x1> <y1>\n"); return 2; }
    int wa, ha, wb, hb;
    unsigned char *a = ppm_lesen(argv[2], &wa, &ha), *b = ppm_lesen(argv[3], &wb, &hb);
    if (!a || !b) { printf("FAIL: PPM nicht lesbar (%s / %s)\n", argv[2], argv[3]); return 1; }
    if (wa != wb || ha != hb) { printf("FAIL: Groesse %dx%d vs %dx%d\n", wa, ha, wb, hb); return 1; }
    int min_diff = atoi(argv[4]);
    int x0 = atoi(argv[5]), y0 = atoi(argv[6]), x1 = atoi(argv[7]), y1 = atoi(argv[8]);
    int diff = 0, aussen = 0, bx0 = wa, by0 = ha, bx1 = -1, by1 = -1;
    for (int y = 0; y < ha; y++) for (int x = 0; x < wa; x++) {
        size_t o = ((size_t)y * (size_t)wa + (size_t)x) * 3;
        if (a[o] != b[o] || a[o+1] != b[o+1] || a[o+2] != b[o+2]) {
            diff++;
            if (x < bx0) bx0 = x; if (x > bx1) bx1 = x; if (y < by0) by0 = y; if (y > by1) by1 = y;
            if (x < x0 || x >= x1 || y < y0 || y >= y1) aussen++;
        }
    }
    printf("bild: %d Pixel verschieden, Kasten (%d,%d)-(%d,%d), ausserhalb [%d,%d)x[%d,%d): %d\n",
           diff, bx0, by0, bx1, by1, x0, x1, y0, y1, aussen);
    free(a); free(b);
    if (min_diff == 0) { if (diff != 0) { printf("FAIL: %d Pixel weichen ab, erwartet 0\n", diff); return 1; } return 0; }
    if (diff < min_diff) { printf("FAIL: nur %d Pixel verschieden, mindestens %d erwartet\n", diff, min_diff); return 1; }
    if (aussen != 0) { printf("FAIL: %d Abweichungen ausserhalb der Figur\n", aussen); return 1; }
    return 0;
}

#include "test_r36_kleidung_teile.inc"   /* hund / kraehe / lut */

int main(int argc, char **argv)
{
    const char *teil = (argc > 1) ? argv[1] : "griff";
    if (strcmp(teil, "karte") == 0) return teil_karte(argc, argv);
    if (strcmp(teil, "bild") == 0)  return teil_bild(argc, argv);
    if (strcmp(teil, "nullen") == 0) return teil_nullen(argc, argv);
    if (strcmp(teil, "hund") == 0 || strcmp(teil, "kraehe") == 0 || strcmp(teil, "lut") == 0) {
        int budget = (argc > 2) ? atoi(argv[2]) : 3000;
        int rc = (teil[0] == 'h') ? teil_hund(budget) : (teil[0] == 'k') ? teil_kraehe(budget) : teil_lut();
        if (rc == 77) return 77;
        if (rc != 0) s_fail++;
        printf("test_r36_kleidung %s: %s\n", teil, s_fail ? "FAILURES" : "OK");
        return s_fail ? 1 : 0;
    }
    if (strcmp(teil, "griff") == 0) {
        const char *flavor = (argc > 2) ? argv[2] : "re2";
        int budget = (argc > 3) ? atoi(argv[3]) : 3000;
        int rc = teil_griff(flavor, budget);
        if (rc == 77) return 77;
        printf("test_r36_kleidung griff: %s\n", s_fail ? "FAILURES" : "OK");
        return s_fail ? 1 : 0;
    }
    printf("unbekannter Teil %s\n", teil);
    return 2;
}
