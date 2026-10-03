/*
 * test_r35_granate.c — Runde 35 Spur A: Handgranate gegen Waende/Gegner im Flug, RE2-Reichweite der
 * Explosion, RE2-Explosionston, Brutalitaet an der RE2-Zombie-Familie, Explosion an Birkin und Alligator.
 *
 * Dossier: analysis/befunde_runde35/A_granate.md. Jede Pruefung misst die MECHANIK (Werte, Zustaende,
 * Bildfolge) an der echten Engine (CORE00.ESP als globale Bank, re15_esp_fx_tick, ROOM1140-Geometrie,
 * Spielschritt fuer die RE2-Zombie-Arena, Boss-Module G5/Gator).
 *
 * Rueckgabe 0 = gruen, sonst die Nummer der ersten fehlgeschlagenen Pruefung.
 * Aufruf: test_r35_granate [wand|reichweite|sound|gore|bosse|strecke|alle]
 */
#include "re15_esp.h"
#include "re15_granate_r35.h"
#include "re15_actor.h"
#include "re15_damage.h"
#include "re15_scd.h"
#include "re15_rdt.h"
#include "re15_aot.h"
#include "re15_room.h"
#include "re15_enemy_ai.h"
#include "re15_enemy.h"
#include "re15_ai_flavor.h"
#include "re15_player.h"
#include "re15_camera.h"
#include "re15_game_step.h"
#include "re15_collision.h"
#include "re15_inventory.h"
#include "re15_msg.h"
#include "re15_tim.h"
#include "re2_ems.h"
#include "re2_fx.h"
#include "re15_skeleton.h"
#include "re15_boss_gator.h"
#include "fx_plattform_pc.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef RE15_ASSET_PSX_DIR
#define RE15_ASSET_PSX_DIR "shared_assets/PSX"
#endif

extern void re15_player_acaec_override_for_test(int on, uint16_t w);
extern void re15_player_aim_reset(void);
extern void re15_player_set_aim_clip_len(int fc);
extern int  re15_re2z_last_death_handler(void);
extern int  re15_re2z_death_cell(unsigned row, unsigned col);
extern void re15_g5_boss_tick(int slot);
extern void re15_g5_flinch_zustand(int *akku, int *takt, int *fenster, int *sub);
extern int  re15_g5_routine(void);

/* Spion fuer die Zusatzbank (die echte Funktion steht in audio_pc.c/SDL, das die Sonde nicht linkt;
 * Muster probe_r34_plattform.c). */
static int s_zusatz_id = -1, s_zusatz_satz = -1, s_zusatz_n = 0;
void re15_audio_arms_zusatz_se(int arms_id, int satz)
{
    s_zusatz_id = arms_id; s_zusatz_satz = satz; s_zusatz_n++;
}

static int s_erste = 0, s_fehler = 0;
#define PRUEF(nr, cond, ...) do { if (!(cond)) { \
        printf("FAIL %d: ", (nr)); printf(__VA_ARGS__); printf("\n"); \
        if (!s_erste) s_erste = (nr); s_fehler++; } else { printf("ok   %d: ", (nr)); printf(__VA_ARGS__); printf("\n"); } } while (0)

static re15_esp_t s_core;

static uint8_t *slurp(const char *p, size_t *n)
{
    FILE *f = fopen(p, "rb"); if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    if (sz <= 0) { fclose(f); return NULL; }
    uint8_t *b = (uint8_t *)malloc((size_t)sz);
    if (b && fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); b = NULL; }
    fclose(f); if (b) *n = (size_t)sz; return b;
}

/* ---- Spione fuer die Tonhaken ------------------------------------------------------------- */
#define MAX_EV 64
static int      s_bild = 0;
static int      s_se_n = 0;       static uint32_t s_se_code[MAX_EV];  static int s_se_bild[MAX_EV];
static int      s_re2se_n = 0;    static uint32_t s_re2se_code[MAX_EV]; static int s_re2se_bild[MAX_EV];
static int32_t  s_re2se_pos[MAX_EV][3];
static void spy_se(uint32_t code, const int32_t pos[3])
{
    (void)pos;
    if (s_se_n < MAX_EV) { s_se_code[s_se_n] = code; s_se_bild[s_se_n] = s_bild; s_se_n++; }
}
static void spy_re2se(uint32_t code, const int32_t pos[3])
{
    if (s_re2se_n < MAX_EV) {
        s_re2se_code[s_re2se_n] = code; s_re2se_bild[s_re2se_n] = s_bild;
        s_re2se_pos[s_re2se_n][0] = pos[0]; s_re2se_pos[s_re2se_n][1] = pos[1]; s_re2se_pos[s_re2se_n][2] = pos[2];
        s_re2se_n++;
    }
}
static void spione_reset(void) { s_se_n = 0; s_re2se_n = 0; s_bild = 0; }
static int zaehl_re2se(uint32_t code) { int n = 0; for (int i = 0; i < s_re2se_n; i++) if (s_re2se_code[i] == code) n++; return n; }
static int zaehl_se(uint32_t code)    { int n = 0; for (int i = 0; i < s_se_n; i++) if (s_se_code[i] == code) n++; return n; }

static uint16_t ru16(const re15_esp_fx_t *f, int off) { return (uint16_t)(f->row[off] | (f->row[off + 1] << 8)); }
static int slot_von(const re15_esp_fx_t *g)
{
    for (int i = 0; i < RE15_ESP_FX_MAX; i++) if (re15_esp_fx_get(i) == g) return i;
    return -1;
}

/* ---- Welt --------------------------------------------------------------------------------- */
static void welt_leer(void)
{
    re15_ai_flavor_set(RE15_AI_FLAVOR_RE15);
    re15_actor_init(); re15_enemy_reset();
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100; pl->hit_react = 0;
    re15_player_apply_hitbox(pl);
    pl->x = 30000; pl->y = 0; pl->z = 30000;
    g_re15_pauseflags = 0;
    g_room_rdt_ok = 0;
    g_current_room_id = 0x1140;
    re15_damage_seed_rng(0x0badf00du);
    re15_granate_r35_zaehler_reset();
}
static re15_actor_t *dummy(int slot, uint8_t typ, int16_t hp, int32_t x, int32_t y, int32_t z)
{
    re15_actor_t *e = &g_actors[slot];
    memset(e, 0, sizeof *e);
    e->active = 1; e->type = typ; e->hp = hp; e->state = 1; e->hit_react = 0;
    e->x = x; e->y = y; e->z = z; e->rot_y = 0; e->em_flag_id = 0xFF;
    re15_enemy_apply_hitbox(e, typ);
    return e;
}

/* ROOM1140.RDT als Raumgeometrie in g_room_rdt (SCA-Zellen fuer den Wandtest). */
static uint8_t *s_rdt1140 = NULL;
static int raum1140(void)
{
    if (!s_rdt1140) {
        size_t n = 0;
        s_rdt1140 = slurp(RE15_ASSET_PSX_DIR "/STAGE1/ROOM1140.RDT", &n);
        if (!s_rdt1140 || re15_rdt_parse(s_rdt1140, n, &g_room_rdt) != 0) { printf("ROOM1140.RDT fehlt\n"); return -1; }
    }
    g_room_rdt_ok = 1;
    g_current_room_id = 0x1140;
    return 0;
}

/* ================================================================================================
 * [1] WAND — Nutzer: "Granaten sollen nicht durch die Wand fliegen"
 * ROOM1140: solide Zelle [x8350 z-20750 w4100 d1150 typ01 u0ff] (gemessen W1, Dossier §2.1). MITTE-Wurf
 * (acaec 0x4000: v=(280,-50,24), acc_x -1 @0x800184bc-dc) ab x 7000 in +x (Gier 0) — der 5. Tick bringt
 * die Granate bei x >= 8350 in die Zelle. RE2 @0x8001ef84-8c: Wand -> Rueckzug (@0x8001ef90-0cc) ->
 * Explosion im selben Bild (@0x8001f0e0-104).
 * ================================================================================================ */
static void abschnitt_wand(void)
{
    welt_leer();
    if (raum1140() != 0) { PRUEF(100, 0, "ROOM1140 laden"); return; }
    re15_esp_fx_reset(); spione_reset();
    re15_player_acaec_override_for_test(1, 0x4000);
    const int32_t x0 = 7000, z0 = -19800, h = -1500;
    /* Werfer 900 hinter der Hand (Nachbesserung 1: im Wurfbild wird die Strecke Werfer -> Hand getestet). */
    g_actors[RE15_ACTOR_SLOT_PLAYER].x = x0 - 900; g_actors[RE15_ACTOR_SLOT_PLAYER].z = z0;
    re15_esp_fx_t *g = re15_esp_granate_spawn(&s_core, 2, x0, h, z0, 0);
    PRUEF(101, g != NULL, "Granate gespawnt (MITTE, Gier 0, Anker (%d,%d,%d))", x0, h, z0);
    if (!g) return;
    g->granate_boden = 0;
    const int slot = slot_von(g);
    unsigned res0 = re15_esp_granate_resolver_calls();
    int bild_wand = -1, bild_frei = -1;
    int16_t wpos_vor[3] = { 0, 0, 0 }, wpos_expl[3] = { 0, 0, 0 }, wpos_vw[3] = { 0, 0, 0 };
    uint16_t z_nach = 0; uint8_t fl_nach = 0; uint16_t A_nach = 0;
    int kinder_expl = 0;
    for (int k = 0; k < 40; k++) {
        s_bild = k;
        const re15_esp_fx_t *f0 = re15_esp_fx_get(slot);
        if (f0 && f0->granate_art) { wpos_vor[0] = f0->wpos[0]; wpos_vor[1] = f0->wpos[1]; wpos_vor[2] = f0->wpos[2]; }
        re15_esp_fx_tick(NULL);
        const re15_esp_fx_t *f = re15_esp_fx_get(slot);
        if (f && f->granate_art && bild_wand < 0 && re15_esp_granate_resolver_calls() > res0) {
            bild_wand = k;
            wpos_vw[0] = wpos_vor[0]; wpos_vw[1] = wpos_vor[1]; wpos_vw[2] = wpos_vor[2];
            wpos_expl[0] = f->wpos[0]; wpos_expl[1] = f->wpos[1]; wpos_expl[2] = f->wpos[2];
            z_nach = ru16(f, 0x1e); fl_nach = f->flags; A_nach = ru16(f, 0x00);
            for (int i = 0; i < RE15_ESP_FX_MAX; i++) {
                const re15_esp_fx_t *c = re15_esp_fx_get(i);
                if (c && c->effect_id == 3 && c->sub_index == 0x19) kinder_expl++;
            }
        }
        if (!(f && f->granate_art) && bild_wand >= 0 && bild_frei < 0) bild_frei = k;
        if (bild_frei >= 0) break;
    }
    unsigned nw = 0, nk = 0, ne = 0; re15_granate_r35_zaehler(&nw, &nk, &ne);
    PRUEF(102, bild_wand >= 0 && bild_wand <= 8,
          "Explosion durch die Wand im Bild %d (Zelle ab x 8350, 5 Ticks a ~280)", bild_wand);
    PRUEF(103, nw == 1 && nk == 0, "Ausloeser: Wand %u, Kontakt %u (soll 1/0)", nw, nk);
    PRUEF(104, wpos_expl[0] < 8350 && wpos_expl[0] > 7000,
          "Rueckzug (RE2 @0x8001ef90-0cc): wpos.x %d liegt wieder VOR der Zelle (x < 8350, > 7000)", (int)wpos_expl[0]);
    PRUEF(105, z_nach == 6 && A_nach == 31 && fl_nach == 0x61,
          "im Wandbild gezuendet: Zuender %u (7->6 @0x8001867c-84), A %u (31), Flags %02x (0x61 @0x80018580-84)",
          z_nach, A_nach, fl_nach);
    PRUEF(106, kinder_expl == 1, "Feuerball 0x03195000 im Wandbild: %d (1)", kinder_expl);
    PRUEF(107, bild_frei == bild_wand + 7, "Platz frei im Bild %d (Wand %d + 7: Zuender 6..0 @0x80018568/@0x800186b0)",
          bild_frei, bild_wand);
    PRUEF(108, zaehl_re2se(RE15_GRANATE_R35_SE_EXPLOSION) == 1,
          "RE2-SE 0x01110001 genau einmal (%d)", zaehl_re2se(RE15_GRANATE_R35_SE_EXPLOSION));
    /* Explosionspunkt der Wandzuendung = Rueckzugspunkt (RE2 Op 47 liest die Lage ohne Versatz @0x80020cdc-fc):
     * der RE2-SE traegt P; P.y liegt auf der Flugbahn (hoechstens einen Schritt neben der Lage des Vorbilds),
     * nicht 500 darueber (Routine-31-Versatz @0x800185a8 gilt der liegenden Granate). */
    { int32_t py = 0, px = 0; for (int i = 0; i < s_re2se_n; i++) if (s_re2se_code[i] == RE15_GRANATE_R35_SE_EXPLOSION) { py = s_re2se_pos[i][1]; px = s_re2se_pos[i][0]; }
      int32_t dy = py - (int32_t)wpos_vw[1]; if (dy < 0) dy = -dy;
      PRUEF(113, px == wpos_expl[0] && py == (int32_t)wpos_expl[1] - RE15_GRANATE_R35_R31_VERSATZ && dy < 150,
            "Explosionspunkt P=(%d,%d) = Rueckzugspunkt: Flugbahn-y im Vorbild %d (Abstand %d < 150; mit Routine-31-Versatz waeren es ~500)",
            (int)px, (int)py, (int)wpos_vw[1], (int)dy); }

    /* Negativ-Kontrolle: OHNE Raumgeometrie (g_room_rdt_ok = 0) fliegt derselbe Wurf weiter (R29 unveraendert). */
    g_room_rdt_ok = 0;
    re15_esp_fx_reset(); spione_reset(); re15_granate_r35_zaehler_reset();
    g = re15_esp_granate_spawn(&s_core, 2, x0, h, z0, 0);
    const int slot2 = slot_von(g);
    int xmax = 0;
    for (int k = 0; k < 12; k++) { re15_esp_fx_tick(NULL); const re15_esp_fx_t *f = re15_esp_fx_get(slot2);
        if (f && f->granate_art && f->wpos[0] > xmax) xmax = f->wpos[0]; }
    re15_granate_r35_zaehler(&nw, &nk, &ne);
    PRUEF(109, nw == 0 && ne == 0 && xmax > 8350, "ohne Zellen: kein Ausloeser (Wand %u, Explosionen %u), x bis %d (> 8350)",
          nw, ne, xmax);

    /* RE2-Flugkontakt (Box @0x80010900 {-1400,0,350,250}, Pruefhoehen y+-1000): als GEOMETRIE belegt, im Flug
     * NICHT verdrahtet (gemessen n1/n2: Luftzuendung ueber dem ueberflogenen Zombie; granate_r35.c). Ein Gegner
     * 0x27 600 vor dem Spawn liegt im gefegten Volumen (Box hinter dem Geschoss + r/4 = 400 vorwaerts) -> der
     * Geometrietest meldet 1, der Flug geht weiter (kein Kontakt-Ausloeser), die Granate explodiert an der Wand
     * und trifft ihn ueber die Reichweite (Wandpunkt ~8020 - 7600 = ~420 < 2000). */
    g_room_rdt_ok = 1;
    re15_esp_fx_reset(); spione_reset(); re15_granate_r35_zaehler_reset();
    re15_actor_t *d = dummy(1, 0x27, 180, x0 + 600, 0, z0);
    g = re15_esp_granate_spawn(&s_core, 2, x0, h, z0, 0);
    const int slot3 = slot_von(g);
    res0 = re15_esp_granate_resolver_calls();
    re15_esp_fx_tick(NULL);                                   /* Tick 0: Weltlage steht */
    const re15_esp_fx_t *f3 = re15_esp_fx_get(slot3);
    int geo = f3 ? re15_granate_r35_kontakt(f3) : -1;
    int bild_x3 = -1;
    for (int k = 1; k < 12; k++) { re15_esp_fx_tick(NULL);
        if (bild_x3 < 0 && re15_esp_granate_resolver_calls() > res0) bild_x3 = k; }
    re15_granate_r35_zaehler(&nw, &nk, &ne);
    PRUEF(110, geo == 1, "RE2-Kontaktgeometrie (@0x80010900, Band @0x8004716c-a4) meldet den Gegner 600 vor dem Spawn: %d (1)", geo);
    PRUEF(111, nw == 1 && ne == 1 && bild_x3 >= 3,
          "Flug geht weiter bis zur Wand: Wand %u, Explosionen %u, Explosionsbild %d (>= 3, nicht beim Kontakt)", nw, ne, bild_x3);
    PRUEF(112, d->hp == 180 - 1000 && d->state == 3 && d->sub_state_1 == 9,
          "Gegner 0x27 ueber die Reichweite der Wandexplosion getroffen: hp %d (-820), Zustand %u (3), +0x5 %u (9)",
          d->hp, d->state, d->sub_state_1);
    g_actors[1].active = 0;
    g_room_rdt_ok = 0;
    re15_player_acaec_override_for_test(0, 0);
}

/* ================================================================================================
 * [2] REICHWEITE — Nutzer: "Range viel zu niedrig" — RE2 Op 47 Box @0x80010918 {-2000,0,1000,500}
 * an P und P+900 (@0x80020d98); Zombie-Kasten r 400 -> Puffer +100 (@0x800471bc-ec): vorwaerts bis
 * 2400, rueckwaerts 2000, seitlich +-2400. Vorher: Zylinder 900 (500 + 400).
 * ================================================================================================ */
static int expl_an(int32_t px, int32_t py, int32_t pz)
{
    const int32_t p[3] = { px, py, pz };
    return re15_granate_r35_explosion(p, 0, 2);
}
static void abschnitt_reichweite(void)
{
    welt_leer();
    const int32_t xl = 0, zl = 0, yl = 20;            /* Liegestelle (y 20 wie W2), P.y = -480 */
    struct { int32_t dx, dy, dz; int soll; const char *was; } faelle[] = {
        {  1200, 0,    0, 1, "vorn 1200 (vorher 900 -> verfehlt, Nutzer-Fall)" },
        {  1900, 0,    0, 1, "vorn 1900" },
        {  2300, 0,    0, 1, "vorn 2300 (2000 + 400/4*4)" },
        {  2600, 0,    0, 0, "vorn 2600 (ausserhalb)" },
        { -1900, 0,    0, 1, "hinten 1900" },
        { -2100, 0,    0, 0, "hinten 2100 (Ecke -2000 @0x80010918)" },
        {     0, 0, 2300, 1, "seitlich 2300" },
        {     0, 0,-2300, 1, "seitlich -2300" },
        {     0, 0, 2600, 0, "seitlich 2600 (ausserhalb)" },
        {   500, -1800, 0, 0, "eine Etage HOEHER (Band @0x8004716c-a4: P.y nicht <= y+100)" },
        {   500,  1800, 0, 1, "eine Etage TIEFER (zweite Pruefhoehe P+900 @0x80020d98 / Band)" },
    };
    const int nf = (int)(sizeof faelle / sizeof faelle[0]);
    int nr = 120;
    for (int i = 0; i < nf; i++, nr++) {
        welt_leer();
        re15_actor_t *d = dummy(1, 0x10, 100, xl + faelle[i].dx, faelle[i].dy, zl + faelle[i].dz);
        int n = expl_an(xl, yl - 500, zl);
        int getroffen = (d->hp != 100);
        PRUEF(nr, getroffen == faelle[i].soll && n == faelle[i].soll,
              "%s: getroffen %d, Rueckgabe %d (soll %d); hp %d", faelle[i].was, getroffen, n, faelle[i].soll, d->hp);
        if (getroffen)
            /* Zombie 0x10 steht unter dem RE2-Schadensmodell (E4): Zeile 9 K0 = 200 (@0x800A41CC), nicht 1000. */
            PRUEF(nr, d->hp == 100 - 200 && d->state == 3 && d->sub_state_1 == 9,
                  "   Schaden E4 (RE2-Record Zeile 9 K0 = 200 @0x800A41CC): hp %d, Zustand %u, +0x5 %u (soll -100/3/9)",
                  d->hp, d->state, d->sub_state_1);
    }
    /* Mehrere Gegner in einer Explosion: alle im Quadrat werden getroffen (Hitcode-Bit 0x10000 "alle"). */
    welt_leer();
    re15_actor_t *a = dummy(1, 0x10, 100, xl + 800, 0, zl);
    re15_actor_t *b = dummy(2, 0x10, 100, xl - 800, 0, zl + 900);
    re15_actor_t *c = dummy(3, 0x10, 100, xl + 1500, 0, zl - 1500);
    /* RE2-Eigenheit (@0x800471bc-ec / Ruecknahme nur im Nicht-Treffer-Zweig @0x800473dc-408): der Puffer waechst
     * je getroffenem Kandidaten um r/4 = 100 (Kante +400) -> nach drei Treffern reicht das Quadrat vorwaerts bis
     * -2000 + 4*(1000 + 3*100) = 3200; der vierte Gegner steht deshalb bei 4000. */
    re15_actor_t *w = dummy(4, 0x10, 100, xl + 4000, 0, zl);
    int n = expl_an(xl, yl - 500, zl);
    PRUEF(nr++, n == 3 && a->hp < 0 && b->hp < 0 && c->hp < 0 && w->hp == 100,
          "drei Gegner im Quadrat getroffen (%d), der vierte bei 4000 nicht (hp %d)", n, w->hp);
    /* und die Eigenheit selbst gemessen: bei 3000 wird er NACH drei Treffern erreicht (Puffer gewachsen) */
    welt_leer();
    dummy(1, 0x10, 100, xl + 800, 0, zl); dummy(2, 0x10, 100, xl - 800, 0, zl + 900); dummy(3, 0x10, 100, xl + 1500, 0, zl - 1500);
    re15_actor_t *w2 = dummy(4, 0x10, 100, xl + 3000, 0, zl);
    n = expl_an(xl, yl - 500, zl);
    PRUEF(nr++, n == 4 && w2->hp < 0, "Puffer-Wachstum: vierter Gegner bei 3000 nach drei Treffern erreicht (%d, hp %d)", n, w2->hp);
    /* Spieler: KEIN Eigenschaden (RE2 FUN_800470C0 nur Gegnerliste @0x800470c4-0x8004740c) — auch bei 300. */
    welt_leer();
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->x = xl + 300; pl->y = 0; pl->z = zl;
    n = expl_an(xl, yl - 500, zl);
    PRUEF(nr++, pl->hp == 100 && pl->state == 0 && n == 0,
          "Spieler 300 neben der Explosion: hp %d Zustand %u (100/0, kein Spielerzweig), Treffer %d", pl->hp, pl->state, n);
    /* Tote Gegner (HP < 0, Gate 3 @0x80047148-50) sind keine Kandidaten. */
    welt_leer();
    re15_actor_t *t = dummy(1, 0x10, -5, xl + 500, 0, zl); t->state = 7;
    n = expl_an(xl, yl - 500, zl);
    PRUEF(nr++, n == 0 && t->hp == -5, "Leiche (hp -5) kein Kandidat: Treffer %d, hp %d", n, t->hp);
}

/* ================================================================================================
 * [3] SOUND — Nutzer: "falscher Explosionssound" — RE2 Op 47 0x01110001 (@0x80020d40-48) statt RE1.5
 * 0x04080001 (@0x800185e4-ec); Plattform-Weiche -> ARMS0F Satz 10 (RE1.5 ARMS0F.EDH @0x28 `00 00 33 20`
 * = RE2 ARMS09.EDH @0x44, .VB bytegleich).
 * ================================================================================================ */
static void abschnitt_sound(void)
{
    welt_leer();
    re15_esp_fx_reset(); spione_reset();
    re15_player_acaec_override_for_test(1, 0x4000);
    re15_esp_fx_t *g = re15_esp_granate_spawn(&s_core, 2, -6851, -2474, -18279, 0);
    const int slot = slot_von(g);
    int bild_x = -1;
    int16_t wpos_x[3] = { 0, 0, 0 };
    unsigned res0 = re15_esp_granate_resolver_calls();
    for (int k = 0; k < 160; k++) {
        s_bild = k;
        re15_esp_fx_tick(NULL);
        const re15_esp_fx_t *f = re15_esp_fx_get(slot);
        if (bild_x < 0 && re15_esp_granate_resolver_calls() > res0) {
            bild_x = k;
            if (f) { wpos_x[0] = f->wpos[0]; wpos_x[1] = f->wpos[1]; wpos_x[2] = f->wpos[2]; }
        }
        if (bild_x >= 0 && !(f && f->granate_art)) break;
    }
    int n_re2 = zaehl_re2se(RE15_GRANATE_R35_SE_EXPLOSION);
    int bild_se = -1; int32_t pos_se[3] = { 0, 0, 0 };
    for (int i = 0; i < s_re2se_n; i++) if (s_re2se_code[i] == RE15_GRANATE_R35_SE_EXPLOSION) {
        bild_se = s_re2se_bild[i]; pos_se[0] = s_re2se_pos[i][0]; pos_se[1] = s_re2se_pos[i][1]; pos_se[2] = s_re2se_pos[i][2]; }
    PRUEF(150, bild_x == 109, "MITTE gesund: Explosion im Bild %d (109, BAUPLAN 1.1 unveraendert)", bild_x);
    PRUEF(151, n_re2 == 1 && bild_se == bild_x, "RE2-SE 0x01110001 genau einmal (%d) im Explosionsbild (%d)", n_re2, bild_se);
    PRUEF(152, zaehl_se(0x04080001u) == 0, "RE1.5-SE 0x04080001 NICHT mehr (%d)", zaehl_se(0x04080001u));
    PRUEF(153, s_se_n == 8 && (s_se_code[0] & 0xffff00ffu) == 0x010A0001u,
          "Abprall-/Liegen-SEs ueber den ESP-Haken unveraendert: %d (8 x 0x010Axx01 @0x80018410-28; die Explosion geht ueber re2fx_se_hook)", s_se_n);
    /* SE-Lage = P = (Welt-x, Welt-y - 500, Welt-z) der liegenden Granate im Explosionsbild
     * (`lh v0,40(v1)` @0x80018594, `addiu v0,v0,-500` @0x800185a8, `lh v0,44(v1)` @0x800185b0). */
    PRUEF(154, n_re2 == 1 && pos_se[0] == wpos_x[0] && pos_se[1] == wpos_x[1] - 500 && pos_se[2] == wpos_x[2]
               && wpos_x[1] >= 0 && wpos_x[1] < 60,
          "SE-Lage (%d,%d,%d) = Liegestelle (%d,%d,%d) mit y - 500 (@0x800185a8); Liegestelle auf der Ebene (0 <= y < 60)",
          (int)pos_se[0], (int)pos_se[1], (int)pos_se[2], (int)wpos_x[0], (int)wpos_x[1], (int)wpos_x[2]);
    /* Plattform-Weiche (fx_plattform_pc.c): 0x01110001 -> ARMS0F Satz 10 */
    int arms = 0, satz = 0;
    int ok = re15_pc_re2fx_se_weiche(RE15_GRANATE_R35_SE_EXPLOSION, &arms, &satz);
    PRUEF(155, ok == 1 && arms == RE15_PC_ARMS_EXPLOSIV && satz == RE15_PC_ARMS_AUFSCHLAG_SATZ,
          "Weiche: 0x01110001 -> ok %d ARMS%02X Satz %d (soll 1 / ARMS0F / 10)", ok, arms, satz);
    PRUEF(156, re15_pc_re2fx_se_weiche(0x01140001u, &arms, &satz) == 0,
          "Weiche: 0x01140001 (Sub-13-Variante, RE2 stumm) -> 0");
    /* Der gebundene Plattform-Haken (re15_pc_re2fx_se) ruft die Zusatzbank ARMS0F Satz 10 genau einmal. */
    { int32_t p0[3] = { 0, 0, 0 }; s_zusatz_n = 0; s_zusatz_id = -1; s_zusatz_satz = -1;
      re15_pc_re2fx_se(RE15_GRANATE_R35_SE_EXPLOSION, p0);
      PRUEF(159, s_zusatz_n == 1 && s_zusatz_id == 0x0F && s_zusatz_satz == 10,
            "re15_pc_re2fx_se(0x01110001): Zusatzbank %d-mal, ARMS%02X Satz %d (1 / ARMS0F / 10)",
            s_zusatz_n, s_zusatz_id, s_zusatz_satz); }
    /* Datei-Belege: ARMS0F.EDH Record 0x0A @0x28 = 00 00 33 20 (= RE2 ARMS09.EDH Record 0x11 @0x44), VB 30416 B. */
    size_t n_edh = 0, n_vb = 0;
    uint8_t *edh = slurp(RE15_ASSET_PSX_DIR "/SOUND/ARMS0F.EDH", &n_edh);
    uint8_t *vb  = slurp(RE15_ASSET_PSX_DIR "/SOUND/ARMS0F.VB", &n_vb);
    PRUEF(157, edh && n_edh >= 0x2c && edh[0x28] == 0x00 && edh[0x29] == 0x00 && edh[0x2a] == 0x33 && edh[0x2b] == 0x20,
          "ARMS0F.EDH @0x28 = %02x %02x %02x %02x (00 00 33 20 = Ton 3 -> VAG 3)",
          edh ? edh[0x28] : 0, edh ? edh[0x29] : 0, edh ? edh[0x2a] : 0, edh ? edh[0x2b] : 0);
    PRUEF(158, vb && n_vb == 30416, "ARMS0F.VB %u B (30416 = RE2 ARMS09.VB, md5 786ad691...)", (unsigned)n_vb);
    free(edh); free(vb);
    re15_player_acaec_override_for_test(0, 0);
}

/* ================================================================================================
 * [4] GORE — Nutzer: "mehr Brutalitaet (Zerplatzen, abplatzende Beine, Arme)" — RE2 DEATH[9][1]
 * @0x8010CD68+4 = 0x80108BEC Zerreissen (bzw. Zeile-9-Wuerfe -> 0x80109610 Wegschleudern). Vorher K1 ->
 * Spalte 3 -> 0x80108530 Sturz (gemessen W2: Clip 1). Arena: echter ROOM1140-Zombie 0x10 unter RE2-KI
 * (Spielschritt), Explosion 300 vor ihm.
 * ================================================================================================ */
static re15_rdt_t         s_rdt;
static uint8_t           *s_rdt_buf = NULL;
static re15_camera_view_t s_cam;
static re15_game_ctx_t    s_ctx;
static const uint8_t *re2_ems_blob(size_t *sz)
{
    static uint8_t *b = NULL; static size_t s = 0; static int tried = 0;
    if (!tried) { tried = 1; b = slurp(RE15_ASSET_PSX_DIR "/../RE2/CDEMD0.EMS", &s); }
    *sz = s; return b;
}
static void load_re2_bank(uint8_t type)
{
    if (re15_enemy_find(type)) return;
    re15_enemy_bank_t *eb = re15_enemy_alloc(type);
    if (!eb) return;
    size_t sz = 0; const uint8_t *ems = re2_ems_blob(&sz);
    re15_tim_t tim; memset(&tim, 0, sizeof tim);
    if (ems && re2_ems_load_bank(ems, sz, type, eb, &tim) == 0) { eb->buf = NULL; eb->ok = 1; return; }
    eb->type = 0;
}
static void frame(void)
{
    const unsigned char *raw; int len, id;
    re15_msg_tick(&raw, &len, &id);
    s_ctx.pad_current = 0; s_ctx.pad_pressed = 0;
    g_actors[RE15_ACTOR_SLOT_PLAYER].hp = 100;
    re15_game_step(&s_ctx);
}
static int room_load(int room_id, const char *stage)
{
    char path[512];
    snprintf(path, sizeof path, RE15_ASSET_PSX_DIR "/%s/ROOM%04X.RDT", stage, room_id);
    free(s_rdt_buf); s_rdt_buf = NULL;
    size_t n = 0;
    s_rdt_buf = slurp(path, &n);
    if (!s_rdt_buf || re15_rdt_parse(s_rdt_buf, n, &s_rdt) != 0) { printf("RDT %s fehlt\n", path); return -1; }
    memset(&s_ctx, 0, sizeof s_ctx);
    s_ctx.rdt = &s_rdt; s_ctx.rdt_ok = 1; s_ctx.cam_view = &s_cam; s_ctx.active_cut = 0;
    g_current_room_id = (uint16_t)room_id;
    return 0;
}
static re15_actor_t *zombie_arena(uint8_t typ)
{
    re15_ai_flavor_set(RE15_AI_FLAVOR_RE2);
    re15_game_state_init();
    re15_actor_init(); re15_aot_init(); scd_vm_init();
    re15_enemy_reset(); re15_enemy_ai_set_paused(0);
    re15_player_cmd_reset(); re15_player_aim_reset();
    re15_esp_fx_reset();
    re15_damage_seed_rng(0x0badf00du);
    g_room_rdt_ok = 0; g_room_change.pending = 0;
    { extern void scd_register_current_rdt(const re15_rdt_t *rdt); scd_register_current_rdt(NULL); }
    if (s_rdt.main_scd)   scd_thread_start(0, s_rdt.main_scd);
    if (s_rdt.sub_scd[0]) scd_thread_start(1, s_rdt.sub_scd[0]);
    g_scd.work_vars[10] = 0;
    for (int i = 0; i < 120; i++) scd_vm_tick();
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100; pl->hit_react = 0;
    pl->state = 0; pl->motion = 0; pl->floor = 0; pl->y = 0;
    re15_collision_set_band(0);
    re15_player_set_aim_clip_len(12);
    for (int s = 1; s < RE15_ACTOR_MAX; s++)
        if (g_actors[s].active && re15_re2_owns_type(g_actors[s].type)) load_re2_bank(g_actors[s].type);
    for (int f = 0; f < 60; f++) frame();
    int slot = -1;
    for (int s = 1; s < RE15_ACTOR_MAX; s++)
        if (g_actors[s].active && g_actors[s].type == typ) { slot = s; break; }
    if (slot < 0) return NULL;
    for (int s = 1; s < RE15_ACTOR_MAX; s++) if (s != slot) g_actors[s].active = 0;
    re15_actor_t *e = &g_actors[slot];
    e->grid_id = 0; e->re2z_f10e = 0; e->re2z_flags21a &= (uint16_t)~0x12u; e->re2z_self1d3 = 0;
    e->hit_react = 0;
    re15_ai_set_state_word(e, 0x101);
    pl->x = e->x - 6000; pl->z = e->z; pl->y = e->y;
    for (int f = 0; f < 10; f++) frame();
    return e;
}
static void abschnitt_gore(void)
{
    if (room_load(0x1140, "STAGE1") != 0) { PRUEF(170, 0, "ROOM1140 fehlt"); return; }
    PRUEF(170, re15_re2z_death_cell(9, 1) == 5 && re15_re2z_death_cell(9, 2) == 5 && re15_re2z_death_cell(9, 3) == 1,
          "DEATH-Tabelle Zeile 9 @0x8010CD68: Spalte 1/2 = 5 (0x80108BEC Zerreissen), Spalte 3 = 1 (0x80108530 Sturz)");
    int handler[6] = { 0, 0, 0, 0, 0, 0 };
    int gore_laeufe = 0, leichen = 0, wiederbelebt = 0, spalte_ok = 0, teile_ok = 0;
    const int LAEUFE = 6;
    for (int lauf = 0; lauf < LAEUFE; lauf++) {
        re15_actor_t *e = zombie_arena(0x10);
        if (!e) { PRUEF(171, 0, "kein Zombie 0x10 in ROOM1140"); return; }
        re15_damage_seed_rng(0x1000u + (uint32_t)lauf * 7919u);
        const int32_t p[3] = { e->x + 300, e->y - 500, e->z };
        int n = re15_granate_r35_explosion(p, 0, 2);
        if (n != 1 || e->state != 3) { PRUEF(171, 0, "Lauf %d: Treffer %d Zustand %u (1/3)", lauf, n, e->state); return; }
        if (e->re2z_hits1d2 == 1u) spalte_ok++;
        int ausgang = -1;
        for (int f = 0; f < 400; f++) {
            frame();
            if (e->state == 7) { ausgang = 7; break; }
            if (e->state != 3) { ausgang = 1; break; }
        }
        int hdl = re15_re2z_last_death_handler();
        if (hdl >= 0 && hdl < 6) handler[hdl]++;
        if (hdl == 5 || hdl == 6) gore_laeufe++;
        if (ausgang == 7) leichen++;
        if (ausgang == 1) wiederbelebt++;
        int teile = 0;
        for (int pp = 0; pp < 20; pp++) if (e->re2z_part_flags[pp] & 0x4Au) teile++;
        if (teile >= 1) teile_ok++;
        printf("   Lauf %d: Spalte +0x1D2 %u, Handler %d, Ausgang %d, fliegende Teile %d\n",
               lauf, e->re2z_hits1d2, hdl, ausgang, teile);
    }
    PRUEF(172, spalte_ok == LAEUFE, "RE2-Stempel Spalte 1 (K0 + Zone 1, PORT-WAHL) in %d/%d Laeufen", spalte_ok, LAEUFE);
    PRUEF(173, gore_laeufe == LAEUFE, "Todes-Handler 0x80108BEC/0x80109610 (5/6) in %d/%d Laeufen (Sturz 1: %d)",
          gore_laeufe, LAEUFE, handler[1]);
    PRUEF(174, leichen == LAEUFE && wiederbelebt == 0, "Leiche (Zustand 7) in %d/%d, Wiederbelebung %d (0)", leichen, LAEUFE, wiederbelebt);
    PRUEF(175, teile_ok == LAEUFE, "abplatzende Teile (Part-Flags 0x4A) in %d/%d Laeufen", teile_ok, LAEUFE);
}

/* ================================================================================================
 * [5] BOSSE — Nutzer: "Tests bei Birkin und Alligator" — Explosion an Birkin 0x30 (RE1.5-KI, STAGE3),
 * 0x36@3080, G5 0x36@5090 (RE2-Modul), Alligator 0x23 (RE1.5-KI) und Gator-Boss 0x23@2090 (Modul):
 * Treffer bei 300 UND bei 1500 (neue Reichweite), Reaktion (HP, Zustand), KEIN Haenger (Zustand 2/3
 * wird verlassen), Modul-Zustaende (G5: Akku/Routine; Gator: Modul setzt +0x4 zurueck).
 * ================================================================================================ */
static void zensus_bild(void)
{
    extern void re15_re2z_hit_filter_apply(int slot);
    extern void re15_re2_pause_filter_apply(int slot);
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    re15_actors_anim_advance();
    re15_enemy_ai_run_all(0);
    for (int s = 1; s < RE15_ACTOR_MAX; s++)
        if (g_actors[s].active && re15_ai_re2_for_type(g_actors[s].type)) {
            if (re15_re2z_owns_type(g_actors[s].type)) re15_re2z_hit_filter_apply(s);
            else                                       re15_re2_pause_filter_apply(s);
        }
    re15_re2_hp_sync();
    pl->hp = 100; pl->hit_react = 0; pl->state = 0;
}
typedef struct { uint8_t typ; uint16_t raum; uint8_t grid; const char *name; } boss_t;
static const boss_t k_bosse[] = {
    { 0x30, 0x3080, 0x00, "Birkin 0x30 (RE1.5-KI)" },
    { 0x36, 0x3080, 0x00, "Birkin 0x36@3080" },
    { 0x36, 0x5090, 0x33, "G5 0x36@5090 (RE2-Modul)" },
    { 0x23, 0x2000, 0x01, "Alligator 0x23 (RE1.5-KI)" },
    { 0x23, 0x2090, 0x01, "Gator-Boss 0x23@2090 (Modul)" },
};
static void abschnitt_bosse(void)
{
    static const int32_t abstaende[2] = { 300, 1500 };
    int nr = 180;
    for (size_t bi = 0; bi < sizeof k_bosse / sizeof k_bosse[0]; bi++) {
        const boss_t *b = &k_bosse[bi];
        for (int ai = 0; ai < 2; ai++, nr++) {
            for (int fl = 0; fl < 2; fl++) {
                const re15_ai_flavor_t flavor = fl ? RE15_AI_FLAVOR_RE2 : RE15_AI_FLAVOR_RE15;
                re15_ai_flavor_set(flavor);
                re15_game_state_init();
                re15_actor_init(); re15_enemy_reset(); re15_enemy_ai_set_paused(0);
                re15_esp_fx_reset(); spione_reset();
                re15_damage_seed_rng(0x0badf00du);
                g_room_rdt_ok = 0;
                g_current_room_id = b->raum;
                if (flavor == RE15_AI_FLAVOR_RE2 || b->raum == 0x5090) load_re2_bank(b->typ);
                re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
                memset(pl, 0, sizeof *pl);
                pl->active = 1; pl->type = 0; pl->hp = 100; pl->x = 0; pl->z = 40000;
                const int slot = 1 + (int)((bi * 4u + (size_t)ai * 2u + (size_t)fl) % 8u);   /* G5-Modul: Slot-Wechsel */
                re15_actor_t *e = &g_actors[slot];
                memset(e, 0, sizeof *e);
                e->active = 1; e->type = b->typ; e->state = 0; e->grid_id = b->grid;
                e->x = 0; e->y = 0; e->z = 0; e->rot_y = 0; e->em_flag_id = 0xFF;
                re15_enemy_apply_hitbox(e, b->typ);
                for (int f = 0; f < 3; f++) zensus_bild();
                if (b->raum == 0x5090) { e->grid_id = 0x13; for (int f = 0; f < 3; f++) zensus_bild(); }
                const int16_t hp_vor = e->hp;
                /* Granate liegt `abstand` VOR dem Boss auf dem Boden (Gator-Boss im Wasser y -1200: Steg y 0). */
                int32_t p[3] = { e->x + abstaende[ai], (b->raum == 0x2090 ? 0 : e->y) - 500, e->z };
                int n = re15_granate_r35_explosion(p, 0, 2);
                const int st_hit = e->state;
                const int getroffen = (b->raum == 0x5090) ? (e->hp != hp_vor) : (st_hit == 2 || st_hit == 3);
                int ok_reaktion = 0, verlassen = -1, ende_st = -1;
                if (getroffen) {
                    if (b->raum == 0x5090) {
                        for (int f = 0; f < 60 && !ok_reaktion; f++) {
                            zensus_bild();
                            int akku, takt, fen, sub;
                            re15_g5_flinch_zustand(&akku, &takt, &fen, &sub);
                            ok_reaktion = (akku > 0 || sub == 0xF || re15_g5_routine() == 3);
                        }
                    } else {
                        for (int f = 0; f < 1500 && verlassen < 0; f++) {
                            zensus_bild();
                            if (!e->active || e->state != st_hit) verlassen = f;
                        }
                        for (int f = 0; f < 1500 && e->active && e->state != 7; f++) zensus_bild();
                        ok_reaktion = (verlassen >= 0);
                        ende_st = e->active ? e->state : -2;
                    }
                }
                PRUEF(nr, n >= 1 && getroffen,
                      "%s %s, Granate %d vor ihm: Treffer %d, hp %d -> %d, Zustand %d", fl ? "RE2" : "RE15", b->name,
                      (int)abstaende[ai], n, hp_vor, e->hp, st_hit);
                PRUEF(nr, !getroffen || ok_reaktion,
                      "   %s %s: kein Haenger (Zustand %d verlassen in Bild %d, Ende %d)", fl ? "RE2" : "RE15", b->name,
                      st_hit, verlassen, ende_st);
                PRUEF(nr, zaehl_re2se(RE15_GRANATE_R35_SE_EXPLOSION) == 0,
                      "   (Sonde: kein Routine-31-Ton ohne Granatenplatz, %d)", zaehl_re2se(RE15_GRANATE_R35_SE_EXPLOSION));
            }
        }
    }
}

/* ================================================================================================
 * [6] NACHBESSERUNG 1 — Zellform, Quadrantenliste, Strecke statt Punkt, Wurfbild
 *   (a) Form: RE1.5-Handler 0x800b2858[1..9] (FUN_8003aea0 @0x8003af04-84) — je Typ ein Punkt IN der
 *       Flaeche (blockiert) und einer im Rechteck, aber AUSSERHALB der Flaeche (frei). RE2 prueft die
 *       Form ebenfalls (Tabelle 0x80011104 @0x8004fe34-54).
 *   (b) ROOM11C0 (Abnahme 0, Lauf t11c0c): Hand (-6720,-13266) liegt im Rechteck der Raute
 *       x[-15400..4799] z[-13266..4921], ausserhalb der Raute -> frei (vorher: Explosion in der Hand).
 *   (c) ROOM1220 (Abnahme 0, Lauf t1220h2): Zelle x[-21825..-21550] (275 dick) wird vom HOCH-Schritt
 *       380 (@0x80018494) je nach Phase uebersprungen -> Strecke haelt in JEDER Phase.
 *   (d) Wurfbild: duenne Zelle zwischen Werfer und Hand -> Explosion ueber dem Werfer, kein Flug dahinter.
 * ================================================================================================ */
static int raum_laden(unsigned rid)
{
    static uint8_t *buf[4]; static unsigned id[4]; static size_t len[4]; static int nbuf = 0;
    int i;
    for (i = 0; i < nbuf; i++) if (id[i] == rid) break;
    if (i == nbuf) {
        char pfad[256];
        if (nbuf >= 4) return -1;
        snprintf(pfad, sizeof pfad, RE15_ASSET_PSX_DIR "/STAGE%u/ROOM%04X.RDT", rid >> 12, rid);
        buf[i] = slurp(pfad, &len[i]);
        if (!buf[i]) { printf("%s fehlt\n", pfad); return -1; }
        id[i] = rid; nbuf++;
    }
    if (re15_rdt_parse(buf[i], len[i], &g_room_rdt) != 0) return -1;
    g_room_rdt_ok = 1; g_current_room_id = rid;
    return 0;
}
static void abschnitt_strecke(void)
{
    /* ---- (a) Form je Typ: eine Zelle x[1000..3000] z[2000..3000] (w 2000, d 1000) in Quadrant 0 ---- */
    {
        static re15_sca_entry_t z1;
        re15_rdt_t r; memset(&r, 0, sizeof r);
        r.sca = &z1; r.sca_count = 1; r.sca_rgn[0] = 1;
        r.ceiling_x = (uint16_t)(int16_t)-30000; r.ceiling_z = (uint16_t)(int16_t)-30000;
        struct { uint8_t typ; int32_t ix, iz, ax, az; const char *was; } f[] = {
            { 1, 1100, 2100, 3100, 2500, "Rechteck: innen (1100,2100), aussen x 3100" },
            { 2, 2000, 2500, 1100, 2100, "Raute LAB_8003d00c: Mitte innen, Ecke (1100,2100) aussen" },
            { 3, 1500, 2500, 1050, 2050, "Kreis FUN_8003d6a8 (Mitte (2000,3000), r 1000): (1500,2500) innen, Ecke (1050,2050) aussen" },
            { 4, 2800, 2900, 1200, 2100, "Dreieck LAB_8003beb0 (rechter Winkel x+w,z+d): (2800,2900) innen, (1200,2100) aussen" },
            { 5, 1200, 2900, 2800, 2100, "Dreieck LAB_8003c734 (rechter Winkel x,z+d): (1200,2900) innen, (2800,2100) aussen" },
            { 6, 2800, 2100, 1200, 2900, "Dreieck LAB_8003cb9c (rechter Winkel x+w,z): (2800,2100) innen, (1200,2900) aussen" },
            { 7, 1200, 2100, 2800, 2900, "Dreieck LAB_8003c2cc (rechter Winkel x,z): (1200,2100) innen, (2800,2900) aussen" },
            { 8, 2000, 2500, 1020, 2020, "Kapsel x LAB_8003d7e8 (Kappen r 500): Mitte innen, Ecke (1020,2020) aussen" },
        };
        int nr = 200;
        for (unsigned i = 0; i < sizeof f / sizeof f[0]; i++, nr++) {
            z1.width = 2000; z1.density = 1000; z1.x = 1000; z1.z = 2000; z1.type = f[i].typ; z1.u0 = 0xff; z1.u1 = 0; z1.floor = 0x03;
            int in  = re15_granate_r35_punkt(&r, f[i].ix, f[i].iz, 0, 1);
            int aus = re15_granate_r35_punkt(&r, f[i].ax, f[i].az, 0, 1);
            int box = re15_collision_box_blocked(&r, f[i].ax, f[i].az, 0, 0, 1);
            /* Gegenprobe am Port-Zwilling des RE1.5-Handlers (Aufloeser FUN_8003b0a4, Radius 0, Maske 1): von
             * aussen nach innen bewegt -> der Handler schiebt; am Aussenpunkt stehend -> er schiebt nicht. */
            int32_t qx = f[i].ix, qz = f[i].iz;
            re15_collision_constrain_contact_band(&r, f[i].ax, f[i].az, &qx, &qz, 0, 0, 1u, NULL, NULL);
            int schiebt_in = (qx != f[i].ix || qz != f[i].iz);
            qx = f[i].ax; qz = f[i].az;
            re15_collision_constrain_contact_band(&r, f[i].ax, f[i].az, &qx, &qz, 0, 0, 1u, NULL, NULL);
            int schiebt_aus = (qx != f[i].ax || qz != f[i].az);
            PRUEF(nr, in == 1 && aus == 0 && (f[i].typ == 1 || box == 1) && schiebt_in == 1 && schiebt_aus == 0,
                  "Typ %u %s: innen %d (1), aussen %d (0); Rechteck-Test aussen %d; RE1.5-Handler schiebt innen %d (1) / aussen %d (0)",
                  f[i].typ, f[i].was, in, aus, box, schiebt_in, schiebt_aus);
        }
        /* Typ 9: Kapsel in z, Zelle w 1000 d 2000 */
        z1.width = 1000; z1.density = 2000; z1.x = 1000; z1.z = 2000; z1.type = 9;
        PRUEF(nr, re15_granate_r35_punkt(&r, 1500, 3000, 0, 1) == 1 && re15_granate_r35_punkt(&r, 1020, 2020, 0, 1) == 0,
              "Typ 9 Kapsel z LAB_8003d930 (Kappen r 500): Mitte innen, Ecke (1020,2020) aussen");
        nr++;
        /* Strecke ueber die leere Rautenecke (frei) und quer durch die Raute (blockiert) */
        z1.width = 2000; z1.density = 1000; z1.x = 1000; z1.z = 2000; z1.type = 2;
        PRUEF(nr, re15_granate_r35_strecke(&r, 900, 2200, 1300, 1900, 0, 1) == 0 &&
                  re15_granate_r35_strecke(&r, 900, 2500, 3100, 2500, 0, 1) == 1,
              "Raute: Strecke ueber die leere Ecke frei, Strecke durch die Mitte blockiert");
        nr++;
        /* Band und Klasse */
        z1.type = 1;
        z1.floor = 0x13;
        int b1 = re15_granate_r35_punkt(&r, 1100, 2100, 0, 1);
        z1.floor = 0x03; z1.u0 = 0x04;
        int k4 = re15_granate_r35_punkt(&r, 1100, 2100, 0, 1);
        PRUEF(nr, b1 == 0 && k4 == 0, "Zelle in Band 1 haelt Band 0 nicht (%d), Zelle u0=04 haelt Maske 1 nicht (%d)", b1, k4);
        nr++;
        /* Quadrantenliste: Deckenpunkt (0,0); Zelle x[100..300] z[-200..200] NUR in Liste 0 (x >= 0, z >= 0) */
        r.ceiling_x = 0; r.ceiling_z = 0; r.sca_rgn[0] = 1;
        z1.width = 200; z1.density = 400; z1.x = 100; z1.z = -200; z1.type = 1; z1.u0 = 0xff; z1.floor = 0x03;
        int oben  = re15_granate_r35_strecke(&r, 0,  100, 400,  100, 0, 1);   /* z >= 0: Liste 0 */
        int unten = re15_granate_r35_strecke(&r, 0, -100, 400, -100, 0, 1);   /* z < 0: Liste 2 (leer) */
        int quer  = re15_granate_r35_strecke(&r, 200, -150, 200, 150, 0, 1);  /* kreuzt z = 0 in der Zelle */
        PRUEF(nr, oben == 1 && unten == 0 && quer == 1,
              "Quadrantenliste (FUN_8003b068): z >= 0 blockiert %d (1), z < 0 frei %d (0), Querung %d (1)", oben, unten, quer);
    }

    /* ---- (b) ROOM11C0: der Punkt aus Lauf t11c0c ---- */
    welt_leer();
    if (raum_laden(0x11C0) != 0) { PRUEF(220, 0, "ROOM11C0 laden"); return; }
    {
        int rechteck = re15_collision_box_blocked(&g_room_rdt, -6720, -13266, 0, 0, 1);
        int form     = re15_granate_r35_punkt(&g_room_rdt, -6720, -13266, 0, 1);
        int mitte    = re15_granate_r35_punkt(&g_room_rdt, -5301, -4173, 0, 1);
        int leon     = re15_granate_r35_punkt(&g_room_rdt, -7157, -12355, 0, 1);
        PRUEF(220, rechteck == 1 && form == 0,
              "ROOM11C0 Hand (-6720,-13266): Rechteck der Raute %d (1 = Vorher-Befund), Form %d (0 = frei)", rechteck, form);
        PRUEF(221, mitte == 1 && leon == 0, "ROOM11C0: Rautenmitte (-5301,-4173) blockiert %d (1), Leons Standpunkt frei %d (0)", mitte, leon);
        /* der Wurf selbst: MITTE aus Leons Hand, Gier 467 (Lauf t11c0c) -> KEIN Ausloeser im Wurfbild */
        re15_esp_fx_reset(); spione_reset(); re15_granate_r35_zaehler_reset();
        re15_player_acaec_override_for_test(1, 0x4000);
        g_actors[RE15_ACTOR_SLOT_PLAYER].x = -7157; g_actors[RE15_ACTOR_SLOT_PLAYER].z = -12355;
        re15_esp_fx_t *g = re15_esp_granate_spawn(&s_core, 2, -6720, -2474, -13266, 467);
        const int slot = g ? slot_von(g) : -1;
        unsigned nw = 0, nk = 0, ne = 0; int flugbilder = 0;
        for (int k = 0; k < 6 && g; k++) {
            re15_esp_fx_tick(NULL);
            const re15_esp_fx_t *f = re15_esp_fx_get(slot);
            re15_granate_r35_zaehler(&nw, &nk, &ne);
            if (f && f->granate_art && ne == 0) flugbilder++;
        }
        PRUEF(222, g != NULL && flugbilder >= 3,
              "ROOM11C0 MITTE-Wurf aus der Hand: %d Flugbilder ohne Explosion (>= 3; vorher 0 = Explosion im Wurfbild)", flugbilder);
        re15_player_acaec_override_for_test(0, 0);
    }

    /* ---- (c) ROOM1220: duenne Zellenfront x[-21825..-21550], HOCH (380/Bild), alle Phasen ---- */
    welt_leer();
    if (raum_laden(0x1220) != 0) { PRUEF(230, 0, "ROOM1220 laden"); return; }
    {
        const int32_t zz = -10677;
        int p_vor = re15_granate_r35_punkt(&g_room_rdt, -21503, zz, 0, 1);
        int p_hin = re15_granate_r35_punkt(&g_room_rdt, -21873, zz, 0, 1);
        int p_in  = re15_granate_r35_punkt(&g_room_rdt, -21603, zz, 0, 1);
        int st    = re15_granate_r35_strecke(&g_room_rdt, -21503, zz, -21873, zz - 21, 0, 1);
        PRUEF(230, p_vor == 0 && p_hin == 0 && p_in == 1 && st == 1,
              "Lauf t1220h2: Punkt vor %d / hinter %d der Zelle frei, Punkt in der Zelle %d, STRECKE -21503 -> -21873 blockiert %d",
              p_vor, p_hin, p_in, st);
        int phasen = 0, gehalten = 0, punkt_haette_verfehlt = 0; int32_t x_min_expl = 0, x_max_expl = -40000;
        re15_player_acaec_override_for_test(1, 0x8000);              /* HOCH */
        for (int off = 0; off < 380; off += 10) {
            re15_esp_fx_reset(); spione_reset(); re15_granate_r35_zaehler_reset();
            const int32_t xa = -20000 - off;
            g_actors[RE15_ACTOR_SLOT_PLAYER].x = xa + 900; g_actors[RE15_ACTOR_SLOT_PLAYER].y = 0;
            g_actors[RE15_ACTOR_SLOT_PLAYER].z = zz;
            re15_esp_fx_t *g = re15_esp_granate_spawn(&s_core, 2, xa, -3600, zz, 2048);
            if (!g) continue;
            g->granate_boden = 0;
            const int slot = slot_von(g);
            unsigned res0 = re15_esp_granate_resolver_calls();
            int expl = 0, in = 0;
            for (int k = 0; k < 20 && !expl; k++) {
                re15_esp_fx_tick(NULL);
                const re15_esp_fx_t *f = re15_esp_fx_get(slot);
                if (!f) break;
                if (re15_esp_granate_resolver_calls() > res0) {
                    expl = 1;
                    if (f->wpos[0] > -21550 && f->wpos[0] < xa + 900) gehalten++;
                    if (f->wpos[0] < x_min_expl) x_min_expl = f->wpos[0];
                    if (f->wpos[0] > x_max_expl) x_max_expl = f->wpos[0];
                }
            }
            /* haette der Punkttest je Bild diese Phase verfehlt? Bahn in -x: Schritt 380, 378, ... (@0x80018494,
             * acc -2 @0x800184b0): kein Bildpunkt in x[-21825..-21550) */
            { int32_t x = xa, v = 380;
              for (int k = 0; k < 8; k++) { x -= v; v -= 2; if (x >= -21825 && x < -21550) in = 1; } }
            if (!in) punkt_haette_verfehlt++;
            phasen++;
        }
        PRUEF(231, phasen == 38 && gehalten == phasen,
              "ROOM1220 HOCH, %d Phasen (Wurfstelle je 10 versetzt): %d halten VOR der Zellenfront (x > -21550); Explosion x in [%d..%d]",
              phasen, gehalten, (int)x_min_expl, (int)x_max_expl);
        PRUEF(232, punkt_haette_verfehlt >= 5,
              "davon %d Phasen, in denen KEIN Bildpunkt in der 275 dicken Zelle liegt (der Punkttest je Bild verfehlte sie)", punkt_haette_verfehlt);

        /* HOCH-Wandzuendung trifft den stehenden Gegner am Boden: P = Rueckzugspunkt (Anker y -3171 wie Lauf d_a ->
         * y ~ -3500), zweite Pruefhoehe P + 900 (@0x80020d98) liegt im Band des Zombies (b98 -1500, h9e 1500
         * @0x8004716c-a4: Pruefhoehe in (-3100, 100]). Mit dem Routine-31-Versatz (P.y - 500 @0x800185a8) laege auch
         * sie darueber (gemessen exe-Lauf d_a vor der Aenderung: P.y -4025). */
        re15_esp_fx_reset(); spione_reset(); re15_granate_r35_zaehler_reset();
        { const int32_t xa = -19723;
          g_actors[RE15_ACTOR_SLOT_PLAYER].x = xa + 900; g_actors[RE15_ACTOR_SLOT_PLAYER].z = zz;
          re15_actor_t *d = dummy(1, 0x10, 100, -20800, 0, zz);
          re15_esp_fx_t *g = re15_esp_granate_spawn(&s_core, 2, xa, -3171, zz, 2048);
          if (g) g->granate_boden = 0;
          unsigned r0 = re15_esp_granate_resolver_calls(); int32_t py = 0;
          for (int k = 0; k < 20 && re15_esp_granate_resolver_calls() == r0; k++) re15_esp_fx_tick(NULL);
          for (int i = 0; i < s_re2se_n; i++) if (s_re2se_code[i] == RE15_GRANATE_R35_SE_EXPLOSION) py = s_re2se_pos[i][1];
          PRUEF(234, d->hp == 100 - 200 && py < -3100 && py > -4000,
                "HOCH-Wandzuendung bei P.y %d (ueber der ersten Pruefhoehe -3100, unter -4000): stehender Zombie 700 vor der Wand getroffen, hp %d (-100)",
                (int)py, d->hp);
          g_actors[1].active = 0; }

        /* ---- (d) Wurfbild: Werfer 468 vor der Zellenfront, Hand 1000 weiter = HINTER der duennen Zelle ---- */
        re15_esp_fx_reset(); spione_reset(); re15_granate_r35_zaehler_reset();
        re15_player_acaec_override_for_test(1, 0x4000);              /* MITTE */
        const int32_t wx = -21550 + 468;
        g_actors[RE15_ACTOR_SLOT_PLAYER].x = wx; g_actors[RE15_ACTOR_SLOT_PLAYER].z = zz;
        int hand_frei = (re15_granate_r35_punkt(&g_room_rdt, wx - 1000, zz, 0, 1) == 0);
        re15_esp_fx_t *g = re15_esp_granate_spawn(&s_core, 2, wx - 1000, -2474, zz, 2048);
        const int slot = g ? slot_von(g) : -1;
        unsigned res0 = re15_esp_granate_resolver_calls();
        re15_esp_fx_tick(NULL);
        const re15_esp_fx_t *f = (slot >= 0) ? re15_esp_fx_get(slot) : NULL;
        unsigned nw = 0, nk = 0, ne = 0; re15_granate_r35_zaehler(&nw, &nk, &ne);
        PRUEF(233, hand_frei && f && re15_esp_granate_resolver_calls() == res0 + 1 && nw == 1 &&
                   f->wpos[0] == (int16_t)wx && f->wpos[2] == (int16_t)zz,
              "Wurfbild: Hand hinter der duennen Zelle (Punkt frei %d) -> Explosion im Wurfbild (Wand %u) ueber dem Werfer: x %d (%d), z %d (%d)",
              hand_frei, nw, f ? (int)f->wpos[0] : 0, (int)wx, f ? (int)f->wpos[2] : 0, (int)zz);
        re15_player_acaec_override_for_test(0, 0);
    }
    g_room_rdt_ok = 0;
}

int main(int argc, char **argv)
{
    const char *teil = (argc > 1) ? argv[1] : "alle";
    int alle = (strcmp(teil, "alle") == 0);
    size_t n = 0;
    uint8_t *buf = slurp(RE15_ASSET_PSX_DIR "/DATA/CORE00.ESP", &n);
    if (!buf) { fprintf(stderr, "CORE00.ESP fehlt\n"); return 1; }
    if (re15_esp_parse_global(buf, n, &s_core) != 0) { fprintf(stderr, "CORE00.ESP Parse\n"); return 1; }
    re15_esp_set_global_bank(&s_core);
    re15_esp_set_room_bank(NULL);
    re15_esp_se_hook = spy_se;
    re2fx_se_hook    = spy_re2se;
    re15_esp_aufschlag_hook = NULL;

    if (alle || strstr(teil, "wand"))       { printf("[1] Wand / Kontakt\n");  abschnitt_wand(); }
    if (alle || strstr(teil, "reichweite")) { printf("[2] Reichweite\n");      abschnitt_reichweite(); }
    if (alle || strstr(teil, "sound"))      { printf("[3] Sound\n");           abschnitt_sound(); }
    if (alle || strstr(teil, "gore"))       { printf("[4] Gore\n");            abschnitt_gore(); }
    if (alle || strstr(teil, "bosse"))      { printf("[5] Bosse\n");           abschnitt_bosse(); }
    if (alle || strstr(teil, "strecke"))    { printf("[6] Form / Strecke (Nachbesserung 1)\n"); abschnitt_strecke(); }

    re15_esp_se_hook = NULL; re2fx_se_hook = NULL;
    free(buf);
    if (s_fehler) { printf("test_r35_granate: %d Fehler, erste Pruefung %d\n", s_fehler, s_erste); return s_erste; }
    printf("test_r35_granate: ALLE PRUEFUNGEN GRUEN\n");
    return 0;
}
