/*
 * test_r38_wasser.c — Runde 38 Spur N "wasser": der Wasser-Effekt beim Gehen durch flaches Wasser.
 *
 * Nutzer (AUFTRAG Runde 38): "In Room 1260 und folgende fehlt noch der Wasser Effekt, wenn sich Leon oder Elza
 * im flachen Wasser bewegen." Dossier analysis/befunde_runde38/N_wasser.md, Code engine/src/wasser_r38.c.
 *
 * Gemessen wird die MECHANIK am echten Raum (RDT + ESP-Bank + PL00/PL00W01-Baenke), ohne exe:
 *   pfuetze    ROOM1260 FLR-Fleck P0 (Typ 0x97): Schritt -> Effekt 6 Sub 5 (0x06050D00 @0x800456b8/bc), Flags 3,
 *              Skala 0x0D00, Anker = Fuss-Part * (0,100,0) + T = der im ORIGINAL gemessene Wert (DuckStation-GDB,
 *              Haltepunkt @0x80045710: Part 4 (-7555,-1873,-7385), Part 7 (-7595,-1887,-7100)), Tonsatz 0x17.
 *   zeitfolge  der Schritt prueft das Bildwort des GEZEICHNETEN Bilds (*(+0x168), anim_set des Vorbilds
 *              @0x8001f36c): gezeichnet 22 / aktuell 23 -> Schritt; gezeichnet 21 / aktuell 22 -> KEIN Schritt
 *              (der Stand vor Runde 38 schaltete genau umgekehrt, einen Takt zu frueh).
 *   trocken    Beton (Typ 0x1d) -> Schritt-Ton 0x1d, KEIN Effekt.
 *   stempel    ROOM2090 Becken, +0x88 = -1620 (sce-8-Stempel des Vorbilds): Effekt 6 Sub 3 (0x06032000 @0x800456f0/f4)
 *              an Part 6 (a1-1, `addiu a2,a2,-108` @0x800456f8), Gier 0x800 (@0x800456e8), Watsatz 0x1a (0x1d - 3
 *              @0x800456ec); Anker relativ zu Leon wie im Original (-233,-836,+174); +0x88 danach 0 (@0x80031da4).
 *   lauf       Echtzeit-Lauf a0 = 1 (`ori a0,zero,0x1` @0x80032850) -> Tonsatz 0x18; Plc-Lauf (Plc_dest) a0 = 0.
 *   aotbreite  sce-8-Zone ROOM1260 main00 @0x11ca (Breite 38950 > 32767): Stempel -900 fuer eine Figur in der Zone
 *              (`lhu`/`sltu` @0x80042b70-94 = vorzeichenlos); vorher las op_aot_set die Breite als s16 -> nie gestempelt.
 *   zensus     FLR-Typ an den sechs Fleckmitten von ROOM1260 = 0x97, daneben 0x1d.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "re15_actor.h"
#include "re15_aot.h"
#include "re15_anim_select.h"
#include "re15_emd.h"
#include "re15_esp.h"
#include "re15_game_step.h"
#include "re15_rdt.h"
#include "re15_scd.h"
#include "re15_wasser_r38.h"

extern re15_actor_t g_actors[];

static int s_fails = 0;
#define CHECK(c, ...) do { if (!(c)) { printf("FAIL: "); printf(__VA_ARGS__); printf("\n"); s_fails++; } \
                           else { printf("ok   "); printf(__VA_ARGS__); printf("\n"); } } while (0)

static uint8_t *slurp(const char *rel, size_t *n)
{
    char p[1024];
    snprintf(p, sizeof p, "%s/%s", RE15_ASSET_PSX_DIR, rel);
    FILE *f = fopen(p, "rb");
    if (!f) { printf("Datei fehlt: %s\n", p); return NULL; }
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    uint8_t *b = (sz > 0) ? (uint8_t *)malloc((size_t)sz) : NULL;
    if (b && fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); b = NULL; }
    fclose(f);
    if (n) *n = b ? (size_t)sz : 0;
    return b;
}

static uint32_t rd32(const uint8_t *p) { return (uint32_t)p[0] | ((uint32_t)p[1] << 8) | ((uint32_t)p[2] << 16) | ((uint32_t)p[3] << 24); }

/* ---- Raum + Baenke --------------------------------------------------------------------------- */
static re15_rdt_t           s_rdt;
static uint8_t             *s_rdt_buf = NULL;
static size_t               s_rdt_n = 0;
static re15_esp_t           s_esp;
static re15_emd_skeleton_t  s_pl00_sk, s_w01_sk;
static re15_emd_animation_t s_pl00_an, s_w01_an;
static re15_game_ctx_t      s_ctx;

static int raum_laden(const char *rel)
{
    free(s_rdt_buf);
    s_rdt_buf = slurp(rel, &s_rdt_n);
    if (!s_rdt_buf) return -1;
    memset(&s_rdt, 0, sizeof s_rdt);
    if (re15_rdt_parse(s_rdt_buf, s_rdt_n, &s_rdt) != 0) return -2;
    memset(&s_esp, 0, sizeof s_esp);
    if (re15_esp_parse(s_rdt_buf, s_rdt_n, rd32(s_rdt_buf + 0x4c), rd32(s_rdt_buf + 0x50),
                       rd32(s_rdt_buf + 0x54), rd32(s_rdt_buf + 0x58), &s_esp) != 0) return -3;
    re15_esp_set_room_bank(&s_esp);
    re15_esp_fx_reset();
    s_ctx.rdt = &s_rdt; s_ctx.rdt_ok = 1;
    return 0;
}

static int baenke_laden(void)
{
    size_t ne = 0, nm = 0;
    uint8_t *e = slurp("PLD/PL00.EDD", &ne), *m = slurp("PLD/PL00.EMR", &nm);
    if (!e || !m) return -1;
    if (re15_emd_parse_animation(e, ne, &s_pl00_an) != 0 || re15_emd_parse_skeleton(m, nm, &s_pl00_sk) != 0) return -2;
    e = slurp("PLD/PL00W01.EDD", &ne); m = slurp("PLD/PL00W01.EMR", &nm);
    if (!e || !m) return -3;
    re15_emd_skeleton_t roh; memset(&roh, 0, sizeof roh);
    if (re15_emd_parse_animation(e, ne, &s_w01_an) != 0 || re15_emd_parse_skeleton(m, nm, &roh) != 0) return -4;
    /* Zusammengesetzte W01-Bank wie der Zeichner (main.c "[w01-composite]"): PL00-Knochen + Grundpose,
     * W01-Keyframe-Pool. */
    s_w01_sk = s_pl00_sk;
    s_w01_sk.keyframe_data       = roh.keyframe_data;
    s_w01_sk.keyframe_data_size  = roh.keyframe_data_size;
    s_w01_sk.keyframe_count      = roh.keyframe_count;
    s_w01_sk.keyframe_size_bytes = roh.keyframe_size_bytes;
    s_ctx.pl00_skel = &s_pl00_sk; s_ctx.pl00_anim = &s_pl00_an;
    s_ctx.w01_anim  = &s_w01_an;
    s_ctx.w_skel    = &s_w01_sk;  s_ctx.w_anim    = &s_w01_an;
    return 0;
}

/* Spieler in Geh-/Lauf-Bewegung aufstellen: Lage, Blick, Bewegung, GEZEICHNETES Bild. */
static re15_actor_t *spieler(int32_t x, int32_t y, int32_t z, int16_t gier, int motion, int bild, int skriptlauf)
{
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    memset(pl, 0, sizeof *pl);
    pl->active = 1;
    pl->x = x; pl->y = y; pl->z = z; pl->rot_y = gier;
    pl->motion = (int16_t)motion;
    pl->anim_frame = (uint16_t)bild;
    pl->anim_flags = 0x04;                 /* LOOP */
    pl->walk_active = (uint8_t)skriptlauf;
    pl->floor = (uint8_t)((y <= -1800) ? 1 : 0);
    return pl;
}

/* Ein Spielschritt: Bildanfang (Schnappschuss = gezeichnetes Bild), Vorschub des Takts, Fusstakt. */
static void takt(re15_actor_t *pl)
{
    re15_wasser_r38_bildanfang(&s_ctx);
    pl->anim_frame++;                      /* re15_player_tick: anim_frame++ (player_common.c) */
    re15_wasser_r38_fusstakt(&s_ctx);
}

static const re15_esp_fx_t *effekt(uint8_t id, uint8_t sub)
{
    for (int i = 0; i < RE15_ESP_FX_MAX; i++) {
        const re15_esp_fx_t *f = re15_esp_fx_get(i);
        if (f && f->effect_id == id && f->sub_index == sub) return f;
    }
    return NULL;
}

static int nah(int32_t a, int32_t b, int32_t tol) { int32_t d = a - b; return d >= -tol && d <= tol; }

/* ---- Faelle ---------------------------------------------------------------------------------- */
static void fall_pfuetze(void)
{
    printf("== pfuetze (ROOM1260 P0, FLR 0x97) ==\n");
    if (raum_laden("STAGE1/ROOM1260.RDT") != 0 || baenke_laden() != 0) { CHECK(0, "Raum/Baenke laden"); return; }
    re15_wasser_r38_reset();
    /* Part 4: gezeichnetes Bild 22 (Bildwort 0x507c im Original), Leon an der Wand (-7068,-1800,-7246) Blick 0. */
    re15_actor_t *pl = spieler(-7068, -1800, -7246, 0, 105, 22, 0);
    takt(pl);
    const re15_wasser_r38_mess_t *m = re15_wasser_r38_mess();
    const re15_esp_fx_t *f = effekt(6, 5);
    CHECK(m->boden == 1, "Zweig Boden genau einmal (boden=%u stempel=%u trocken=%u)", m->boden, m->stempel, m->trocken);
    CHECK(f != NULL, "Effekt 6 Sub 5 im Pool (0x06050D00 @0x800456b8/bc)");
    if (f) {
        CHECK(f->scale16 == 0x0D00, "Skala 0x0D00 (@0x800456bc), ist %#x", f->scale16);
        CHECK(f->flags == 0x03, "Start-Flags 3 (FUN_80019700 @0x800197b4), ist %#x", f->flags);
        CHECK(f->param == 0, "Gier +0x2e = Blick 0 (`lh a1,106(a1)` @0x800456d0), ist %d", f->param);
    }
    CHECK(m->part == 4, "Part 4 (7 - 3*Bit12, Bildwort 0x507c), ist %d", m->part);
    CHECK(nah(m->anker[0], -7555, 3) && nah(m->anker[1], -1873, 3) && nah(m->anker[2], -7385, 3),
          "Anker = Original (-7555,-1873,-7385) +-3, ist (%d,%d,%d)", m->anker[0], m->anker[1], m->anker[2]);
    CHECK(m->edt == 0x17, "Tonsatz 0x17 = 0 + (0x97 & 0x7f) (@0x80045718-24), ist %#x", m->edt);

    /* Part 7: gezeichnetes Bild 7 (Bildwort 0x606d). */
    re15_esp_fx_reset();
    re15_wasser_r38_reset();
    pl = spieler(-7068, -1800, -7246, 0, 105, 7, 0);
    takt(pl);
    CHECK(m->boden == 1 && m->part == 7, "Part 7 im Bild 7 (boden=%u part=%d)", m->boden, m->part);
    CHECK(nah(m->anker[0], -7595, 3) && nah(m->anker[1], -1887, 3) && nah(m->anker[2], -7100, 3),
          "Anker = Original (-7595,-1887,-7100) +-3, ist (%d,%d,%d)", m->anker[0], m->anker[1], m->anker[2]);
}

static void fall_zeitfolge(void)
{
    printf("== zeitfolge (Bildwort des gezeichneten Bilds, @0x8001f36c) ==\n");
    if (raum_laden("STAGE1/ROOM1260.RDT") != 0 || baenke_laden() != 0) { CHECK(0, "Raum/Baenke laden"); return; }
    re15_wasser_r38_reset();
    re15_actor_t *pl = spieler(-7068, -1800, -7246, 0, 105, 21, 0);
    takt(pl);                              /* gezeichnet 21 (kein Bit 0x4000), aktuell 22 (Bit 0x4000) */
    const re15_wasser_r38_mess_t *m = re15_wasser_r38_mess();
    CHECK(m->boden + m->stempel + m->trocken == 0, "gezeichnet 21 / aktuell 22: KEIN Schritt (alt: Schritt einen Takt zu frueh)");
    takt(pl);                              /* gezeichnet 22, aktuell 23 */
    CHECK(m->boden == 1, "gezeichnet 22 / aktuell 23: Schritt (alt: kein Schritt), boden=%u", m->boden);
    takt(pl);                              /* gezeichnet 23: kein weiterer */
    CHECK(m->boden == 1, "naechster Takt ohne Schritt (boden=%u)", m->boden);
}

static void fall_trocken(void)
{
    printf("== trocken (ROOM1260 Beton 0x1d) ==\n");
    if (raum_laden("STAGE1/ROOM1260.RDT") != 0 || baenke_laden() != 0) { CHECK(0, "Raum/Baenke laden"); return; }
    re15_wasser_r38_reset();
    re15_actor_t *pl = spieler(-8404, -1800, -7246, 0, 105, 22, 0);
    takt(pl);
    const re15_wasser_r38_mess_t *m = re15_wasser_r38_mess();
    CHECK(m->trocken == 1 && m->boden == 0 && m->stempel == 0, "trockener Schritt (trocken=%u)", m->trocken);
    CHECK(re15_esp_fx_count() == 0, "kein Effekt im Pool (%d)", re15_esp_fx_count());
    CHECK(m->edt == 0x1d, "Tonsatz 0x1d (Beton), ist %#x", m->edt);
}

static void fall_stempel(void)
{
    printf("== stempel (ROOM2090, sce-8 +0x88 = -1620) ==\n");
    if (raum_laden("STAGE2/ROOM2090.RDT") != 0 || baenke_laden() != 0) { CHECK(0, "Raum/Baenke laden"); return; }
    re15_wasser_r38_reset();
    re15_actor_t *pl = spieler(-8433, 0, -25503, 0, 105, 7, 0);
    pl->water_y = -1620;                   /* Stempel des Vorbild-Scans (Handler 0x8004330c `sh v0,136`) */
    takt(pl);
    const re15_wasser_r38_mess_t *m = re15_wasser_r38_mess();
    const re15_esp_fx_t *f = effekt(6, 3);
    CHECK(pl->water_y == 0, "Spieler+0x88 nach dem Bildanfang 0 (`sh zero` @0x80031da4), ist %d", pl->water_y);
    CHECK(m->stempel == 1, "Zweig Stempel (stempel=%u boden=%u)", m->stempel, m->boden);
    CHECK(f != NULL, "Effekt 6 Sub 3 im Pool (0x06032000 @0x800456f0/f4)");
    if (f) {
        CHECK(f->scale16 == 0x2000, "Skala 0x2000, ist %#x", f->scale16);
        CHECK(f->param == 0x800, "Gier 0x800 (`ori a1,zero,0x800` @0x800456e8), ist %#x", f->param);
    }
    CHECK(m->part == 6, "Part 6 = a1 - 1 (`addiu a2,a2,-108` @0x800456f8), ist %d", m->part);
    CHECK(nah(m->anker[0] - pl->x, -233, 3) && nah(m->anker[1] - pl->y, -836, 3) && nah(m->anker[2] - pl->z, 174, 3),
          "Anker relativ zu Leon = Original (-233,-836,174) +-3, ist (%d,%d,%d)",
          m->anker[0] - pl->x, m->anker[1] - pl->y, m->anker[2] - pl->z);
    CHECK(m->edt == 0x1a, "Watsatz 0x1a = 0x1d - 3 (@0x800456ec), ist %#x", m->edt);
}

static int lauf_schrittbild(void)
{
    /* erstes Bild des Lauf-Clips (W01 Clip 0) mit Bit 0x4000 */
    re15_actor_t probe; memset(&probe, 0, sizeof probe); probe.anim_flags = 0x04; probe.motion = 100;
    if (s_w01_an.clip_count <= 0) return -1;
    for (int b = 0; b < s_w01_an.clips[0].frame_count; b++)
        if (re15_actor_footstep(&s_w01_an, &probe, 0, (uint32_t)b)) return b;
    return -1;
}

static void fall_lauf(void)
{
    printf("== lauf (a0 = 1 @0x80032850) ==\n");
    if (raum_laden("STAGE1/ROOM1260.RDT") != 0 || baenke_laden() != 0) { CHECK(0, "Raum/Baenke laden"); return; }
    int b = lauf_schrittbild();
    CHECK(b >= 0, "Lauf-Clip hat ein Schrittbild (%d)", b);
    if (b < 0) return;
    re15_wasser_r38_reset();
    re15_actor_t *pl = spieler(-7068, -1800, -7246, 0, 100, b, 0);
    takt(pl);
    const re15_wasser_r38_mess_t *m = re15_wasser_r38_mess();
    CHECK(m->boden == 1 && m->edt == 0x18, "Echtzeit-Lauf in der Pfuetze: Tonsatz 0x18 (a0 = 1), ist %#x", m->edt);
    re15_esp_fx_reset(); re15_wasser_r38_reset();
    pl = spieler(-7068, -1800, -7246, 0, 100, b, 1);
    takt(pl);
    CHECK(m->boden == 1 && m->edt == 0x17, "Plc-Lauf (Plc_dest): Tonsatz 0x17 (a0 = 0 @0x80030e18), ist %#x", m->edt);
    CHECK(re15_wasser_r38_a0(105, 0) == 0 && re15_wasser_r38_a0(100, 0) == 1 && re15_wasser_r38_a0(100, 1) == 0,
          "a0: Gehen 0, Lauf 1, Plc-Lauf 0");
}

static void fall_aotbreite(void)
{
    printf("== aotbreite (ROOM1260 main00 sce-8, Breite 38950) ==\n");
    if (raum_laden("STAGE1/ROOM1260.RDT") != 0) { CHECK(0, "Raum laden"); return; }
    /* Satz aus der Datei suchen: Aot_set 0x2C, sce 8, Breite > 32767 (main00 @0x11ca) */
    const uint8_t *satz = NULL;
    for (size_t o = 0; o + 20 <= s_rdt_n; o++) {
        const uint8_t *p = s_rdt_buf + o;
        if (p[0] == 0x2c && p[2] == 0x08 && p[3] == 0x47 && (p[11] & 0x80) && (uint16_t)(p[10] | (p[11] << 8)) == 38950)
            { satz = p; break; }
    }
    CHECK(satz != NULL, "sce-8-Satz mit Breite 38950 in ROOM1260.RDT gefunden");
    if (!satz) return;
    uint8_t bc[21];
    memcpy(bc, satz, 20); bc[20] = 0x02;   /* Evt_next */
    scd_vm_init();
    re15_aot_init();
    CHECK(scd_thread_start(0, bc) == 0, "SCD-Faden gestartet");
    scd_vm_tick();
    int slot = satz[1];
    CHECK(g_aot.slots[slot].type == RE15_AOT_TYPE_WATER, "Satz als WATER installiert (Typ %d)", g_aot.slots[slot].type);
    int16_t x0 = (int16_t)(satz[6] | (satz[7] << 8)), z0 = (int16_t)(satz[8] | (satz[9] << 8));
    re15_actor_t *pl = spieler(0, 0, 0, 0, 105, 0, 0);
    pl->x = x0 + 30000; pl->z = z0 + 1500; pl->floor = 0; pl->water_y = 0;   /* weit hinter x+32767 */
    re15_aot_scan(pl->x, pl->z, 0);
    CHECK(pl->water_y == -900, "Figur bei x0+30000 in der Zone: Stempel -900 (vorzeichenlos @0x80042b70), ist %d", pl->water_y);
    pl->water_y = 0; pl->x = x0 - 100;
    re15_aot_scan(pl->x, pl->z, 0);
    CHECK(pl->water_y == 0, "Figur vor der Zone: kein Stempel (%d)", pl->water_y);
}

static void fall_zensus(void)
{
    printf("== zensus (ROOM1260 FLR) ==\n");
    if (raum_laden("STAGE1/ROOM1260.RDT") != 0) { CHECK(0, "Raum laden"); return; }
    static const int32_t mitte[6][2] = { {-7200,-7900}, {-8550,550}, {25850,12000}, {20600,-5600}, {-20900,-16050}, {-20150,-8450} };
    for (int i = 0; i < 6; i++)
        CHECK(re15_rdt_floor_sound(&s_rdt, mitte[i][0], mitte[i][1]) == 0x97, "Fleck P%d (%d,%d) = 0x97", i, mitte[i][0], mitte[i][1]);
    CHECK(re15_rdt_floor_sound(&s_rdt, -8404, -7246) == 0x1d, "Gang neben P0 = 0x1d");
}

int main(int argc, char **argv)
{
    const char *fall = (argc > 1) ? argv[1] : "alle";
    int alle = strcmp(fall, "alle") == 0;
    if (alle || !strcmp(fall, "pfuetze"))   fall_pfuetze();
    if (alle || !strcmp(fall, "zeitfolge")) fall_zeitfolge();
    if (alle || !strcmp(fall, "trocken"))   fall_trocken();
    if (alle || !strcmp(fall, "stempel"))   fall_stempel();
    if (alle || !strcmp(fall, "lauf"))      fall_lauf();
    if (alle || !strcmp(fall, "aotbreite")) fall_aotbreite();
    if (alle || !strcmp(fall, "zensus"))    fall_zensus();
    printf("%s: %d Fehler\n", fall, s_fails);
    return s_fails ? 1 : 0;
}
