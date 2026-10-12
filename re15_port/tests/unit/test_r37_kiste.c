/* test_r37_kiste.c — Runde 37 Spur M "kiste" (Nachtrag 9 des Nutzers). Dossier analysis/befunde_runde37/M_kiste.md.
 *
 *   "In der Kiste muessen die Icons offenbar so weit wie moeglich rechts liegen, damit sie nicht vom Text
 *    ueberdeckt werden. [...] beim scrollen/bewegen in der Kiste [liegen] waehrenddessen die Items oberhalb des
 *    Kastens [...]. Der Kasten muss layertechnisch ueber den Items liegen."
 *
 * Riegel ueber die ECHTE Op-Liste des Kisten-Schirms (re15_inv_screen_build, Block 7b), Takt fuer Takt ueber die
 * echte Kisten-FSM (re15_menu_* wie test_itembox_screen):
 *   (A) M1 ZENSUS aller Item-Ids 1..255 in drei Spielzustaenden (Anfang; alle Flags gesetzt mit Leon; alle Flags
 *       mit Elza — erfasst zustandsabhaengige Namen anderer Spuren, z.B. "Empty Fire Extinguisher" nach der
 *       Integration): das rechteste GEMALTE Pixel des Namens (Glyphen aus DATA/TEX.TIM wie inv_render_pc.c) liegt
 *       links vom Symbol; das Symbol endet buendig am Fensterrand (RE15_KISTE_SYMBOL_RECHTS = boxX+0xc5 = 204,
 *       RE2 @0x80071238); die Mengen-Ziffern liegen IM Symbol unten links (RE2 FUN_80072c6c @0x800aae2c,
 *       Zeilenoberkante + 11) und UEBER ihm (RE2 Ziffern OT-Eintrag 3 ueber Symbol 6).
 *   (B) M2 GLEITEN: jedes Bild der Scroll-Animation RUNTER und HOCH (RE2 Zustaende 3/2, Versatz -+3 x 6): die vier
 *       Rahmen-Teile (Panel ohne Listenfenster) stehen in der Op-Liste VOR (= ueber) allen Zeilen-Ops, jede
 *       Zeilen-Op liegt innerhalb des Panels (dort deckt sie der Rahmen ausserhalb des Fensters), die Rahmen-Teile
 *       tasten das Panel-Bild 1:1 an derselben Stelle ab; die einlaufende Zeile (RE2: scroll-1 / scroll+5) ist da.
 *   (C) UNVERAENDERT: Auswahl-Band, Scrollgriff, Panel-Ops, Namens-x 16.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "re15_menu.h"
#include "re15_itembox.h"
#include "re15_inv_screen.h"
#include "re15_inventory.h"
#include "re15_player.h"
#include "re15_damage.h"
#include "re15_fade.h"
#include "re15_actor.h"
#include "re15_scd.h"
#include "re15_gameflow.h"
#include "re15_tim.h"
#include "re15_kiste_r37.h"

#ifndef RE15_ASSET_PSX_DIR
#define RE15_ASSET_PSX_DIR "shared_assets/PSX"
#endif

static int fails = 0;
#define CHECK(cond, ...) do { if (!(cond)) { fprintf(stderr, "FAIL: " __VA_ARGS__); \
    fprintf(stderr, "\n"); fails++; } } while (0)

/* Port-Geometrie der Zeilen (re15_inv_screen.c Block 7b): Zeile 0 bei RE2 y 41 + Versatz 12. */
#define ZEILE0_Y   (41 + (RE15_KISTE_PANEL_Y - 13))
#define NAME_X     16
#define FENSTER_X0 (RE15_KISTE_PANEL_X + RE15_KISTE_FENSTER_BX0)          /* 13  */
#define FENSTER_X1 (RE15_KISTE_PANEL_X + RE15_KISTE_FENSTER_BX1)          /* 203 */
#define FENSTER_Y0 (RE15_KISTE_PANEL_Y + RE15_KISTE_FENSTER_BY0)          /* 52  */
#define FENSTER_Y1 (RE15_KISTE_PANEL_Y + RE15_KISTE_FENSTER_BY1)          /* 152 */
#define PANEL_X1   (RE15_KISTE_PANEL_X + RE15_KISTE_PANEL_W - 1)          /* 219 */
#define PANEL_Y1   (RE15_KISTE_PANEL_Y + RE15_KISTE_PANEL_H - 1)          /* 180 */

/* ---- Glyphen des Namens: DATA/TEX.TIM Texel-Spalten 256..511, Zeilen 0..127, CLUT-Eintraege 0..15 (wie
 *      inv_render_pc.c s_font4 / s_clut[10]); Texel mit CLUT-Wert 0 ist durchsichtig. ---- */
static uint8_t  s_font4[128][256];
static uint16_t s_fclut[16];
static int font_laden(void)
{
    char pfad[512];
    FILE *f;
    long n;
    uint8_t *buf;
    re15_tim_t tim;
    snprintf(pfad, sizeof pfad, "%s/DATA/TEX.TIM", RE15_ASSET_PSX_DIR);
    f = fopen(pfad, "rb");
    if (!f) return 0;
    fseek(f, 0, SEEK_END); n = ftell(f); fseek(f, 0, SEEK_SET);
    buf = (uint8_t *)malloc((size_t)n);
    if (!buf || fread(buf, 1, (size_t)n, f) != (size_t)n) { fclose(f); free(buf); return 0; }
    fclose(f);
    if (re15_tim_parse(buf, (int)n, &tim) != 0 || tim.bpp != 4 || !tim.has_clut) { free(buf); return 0; }
    {
        const uint8_t *px = (const uint8_t *)tim.pixels;
        int pitch = tim.width / 2, y, u;
        for (y = 0; y < 128 && y < tim.height; y++)
            for (u = 0; u < 256 && 256 + u < tim.width; u++) {
                int vx = 256 + u;
                uint8_t bb = px[y * pitch + (vx >> 1)];
                s_font4[y][u] = (vx & 1) ? (bb >> 4) : (bb & 0xF);
            }
        memcpy(s_fclut, tim.clut, sizeof s_fclut);
    }
    free(buf);   /* tim zeigt in buf — alles Noetige ist kopiert */
    return 1;
}

/* rechtestes gemaltes Pixel einer FONT4-Glyphe (-1 = leer) */
static int glyph_rechts(const re15_inv_op_t *o)
{
    int r = -1, py, pxx;
    for (py = 0; py < o->h; py++) {
        int v = (o->v + py) & 255;
        for (pxx = 0; pxx < o->w; pxx++) {
            int u = (o->u + pxx) & 255;
            if (s_fclut[s_font4[v & 127][u]] != 0 && o->x + pxx > r) r = o->x + pxx;
        }
    }
    return r;
}

/* ---- Bild-Takt wie test_itembox_screen ---- */
static void frame(uint16_t pressed, uint16_t held)
{
    re15_menu_start_poll(pressed, 1);
    if (re15_menu_gameplay_frozen())
        re15_menu_fsm_tick(pressed, held);
    if (re15_menu_is_open())
        re15_inv_screen_ecg_tick();
    re15_fade_tick();
}
static void idle(int n) { while (n-- > 0) frame(0, 0); }
static void press(uint16_t bits) { frame(bits, bits); }

static re15_inv_op_t s_ops[RE15_INV_MAX_OPS];

static int ist_zeilen_op(const re15_inv_op_t *o)
{
    /* Zeilen-Ops der Kisten-Liste: Namensglyphen (FONT4), Symbol (ITEMTILE), Mengen-Ziffern (TEX4 8x8 v 0xf0),
     * alle links vom Inventar-Gitter (x < 215) und oberhalb der Textbox (y < Panel-Ende). Die Bildunterschrift
     * in der Textbox (y 193) und das Inventar rechts (x >= 215) zaehlen nicht dazu. */
    if (o->kind != RE15_INV_OP_SPRT) return 0;
    if (o->x >= 215 || o->y > PANEL_Y1 + 1) return 0;
    if (o->page == RE15_INV_PAGE_FONT4 || o->page == RE15_INV_PAGE_ITEMTILE) return 1;
    if (o->page == RE15_INV_PAGE_TEX4 && o->w == 8 && o->h == 8 && o->v == 0xf0) return 1;
    return 0;
}

static int ist_breit(uint8_t id) { return (uint8_t)(id - 0x0e) < 6; }   /* inventory_common.c (type-0x0e)<6 */

static void kiste_oeffnen_boxseite(void)
{
    if (re15_menu_is_open()) { re15_menu_toggle(); idle(2); }
    re15_menu_toggle_box();
    idle(1);
    press(RE15_PAD_BIT_SQUARE);                       /* virtual confirm 0x4000 -> Box-Seite (Zustand 1) */
}

/* ===================================================================================== (A) M1 */
typedef struct { int id, rechts, sym_x, sym_w, luecke; } zensus_t;

static int zensus(const char *zustand, zensus_t *best, int *n_namen)
{
    int id, sel = RE15_BOX_PICK_ROW;
    int ry = ZEILE0_Y + sel * RE15_BOX_ROW_PX;
    int min_luecke = 1000;
    *n_namen = 0;
    best->rechts = -1;
    for (id = 1; id <= 255; id++) {
        int i, n, sym_i = -1, rechts = -1, glyphen = 0, links = 9999;
        re15_itembox_init();
        g_itembox.slots[sel].id = (uint8_t)id;
        g_itembox.slots[sel].qty = 100;                          /* drei Ziffern, wo die Id Ziffern hat */
        g_itembox.slots[sel].flags = ist_breit((uint8_t)id) ? RE15_BOX_KIND_WIDE : 0;
        n = re15_inv_screen_build(&g_inv_screen, s_ops, RE15_INV_MAX_OPS);
        for (i = 0; i < n; i++) {
            const re15_inv_op_t *o = &s_ops[i];
            if (!ist_zeilen_op(o)) continue;
            if (o->page == RE15_INV_PAGE_ITEMTILE && o->y == ry) sym_i = i;
            if (o->page == RE15_INV_PAGE_FONT4 && o->y == ry + 2) {
                int r = glyph_rechts(o);
                glyphen++;
                if (o->x < links) links = o->x;
                if (r > rechts) rechts = r;
            }
        }
        CHECK(sym_i >= 0, "(A %s) Id 0x%02X: kein Symbol in Zeile %d", zustand, id, sel);
        if (sym_i < 0) continue;
        {
            const re15_inv_op_t *s = &s_ops[sym_i];
            int w_soll = ist_breit((uint8_t)id) ? (RE15_BOX_ICON_W * 2 - 1) : RE15_BOX_ICON_W;
            CHECK(s->w == w_soll && s->h == RE15_BOX_ICON_H,
                  "(A %s) Id 0x%02X: Symbolgroesse %dx%d, erwartet %dx%d", zustand, id, s->w, s->h, w_soll,
                  RE15_BOX_ICON_H);
            /* M1: so weit rechts wie moeglich = buendig am Fensterrand boxX+0xc5 (RE2 @0x80071238) */
            CHECK(s->x + s->w == RE15_KISTE_SYMBOL_RECHTS && s->x + s->w - 1 == FENSTER_X1,
                  "(A %s) Id 0x%02X: Symbol x %d..%d endet nicht am Fensterrand %d", zustand, id, s->x,
                  s->x + s->w - 1, FENSTER_X1);
            if (glyphen > 0) {
                int luecke = s->x - rechts - 1;
                (*n_namen)++;
                CHECK(links == NAME_X, "(A %s) Id 0x%02X: Name beginnt bei x %d statt %d (unveraendert)",
                      zustand, id, links, NAME_X);
                /* M1: kein gemaltes Namenspixel im Symbol */
                CHECK(rechts < s->x, "(A %s) Id 0x%02X: Name reicht bis x %d IN das Symbol ab x %d",
                      zustand, id, rechts, s->x);
                if (luecke < min_luecke) min_luecke = luecke;
                if (rechts > best->rechts) {
                    best->id = id; best->rechts = rechts; best->sym_x = s->x; best->sym_w = s->w;
                    best->luecke = luecke;
                }
            }
            /* Ziffern: im Symbol unten links und UEBER ihm (Op-Index kleiner = spaeter gerastert = oben) */
            for (i = 0; i < n; i++) {
                const re15_inv_op_t *o = &s_ops[i];
                if (!ist_zeilen_op(o) || o->page != RE15_INV_PAGE_TEX4) continue;
                if (o->y != ry + RE15_KISTE_ZIFFER_DY) {
                    CHECK(0, "(A %s) Id 0x%02X: Ziffer bei y %d statt Zeilenoberkante+11 = %d", zustand, id, o->y,
                          ry + RE15_KISTE_ZIFFER_DY);
                    continue;
                }
                CHECK(o->x >= s->x && o->x + o->w <= s->x + s->w && o->y + o->h <= s->y + s->h,
                      "(A %s) Id 0x%02X: Ziffer (%d,%d) %dx%d liegt nicht im Symbol (%d,%d) %dx%d", zustand, id,
                      o->x, o->y, o->w, o->h, s->x, s->y, s->w, s->h);
                CHECK(i < sym_i, "(A %s) Id 0x%02X: Ziffer-Op %d unter dem Symbol-Op %d", zustand, id, i, sym_i);
            }
            /* erste Ziffer an der linken Symbolkante (RE2: x = boxX+0x92 = Symbolkante) */
            for (i = 0; i < n; i++) {
                const re15_inv_op_t *o = &s_ops[i];
                if (!ist_zeilen_op(o) || o->page != RE15_INV_PAGE_TEX4) continue;
                CHECK(o->x >= s->x + RE15_KISTE_ZIFFER_DX, "(A %s) Id 0x%02X: Ziffer links der Symbolkante",
                      zustand, id);
                break;
            }
        }
    }
    return min_luecke;
}

/* ===================================================================================== (B) M2 */
static int s_gleit_bilder = 0, s_einlauf_hoch = 0, s_einlauf_runter = 0;

static void pruefe_bild(const char *wo, int bild)
{
    int n = re15_inv_screen_build(&g_inv_screen, s_ops, RE15_INV_MAX_OPS);
    int i, t, teil_i[RE15_KISTE_RAHMEN_TEILE], n_teil = 0, erste_zeile = -1, letzte_teil = -1;
    int band_max = -1, griff_i = -1, panel_min = 1 << 30;
    int pixoff = re15_itembox_screen_pixoff();
    int scroll = re15_itembox_screen_scroll();
    for (i = 0; i < n; i++) {
        const re15_inv_op_t *o = &s_ops[i];
        if (o->kind == RE15_INV_OP_PANELTEIL) {
            if (n_teil < RE15_KISTE_RAHMEN_TEILE) teil_i[n_teil] = i;
            n_teil++;
            letzte_teil = i;
        } else if (o->kind == RE15_INV_OP_PANEL) {
            if (i < panel_min) panel_min = i;
        } else if (o->kind == RE15_INV_OP_FILL && o->x == 12 && o->w == 192 && o->h == 2) {
            band_max = i;
        } else if (o->kind == RE15_INV_OP_FILL && o->x == 208 && o->w == 6) {
            griff_i = i;
        } else if (ist_zeilen_op(o) && erste_zeile < 0) {
            erste_zeile = i;
        }
    }
    CHECK(n_teil == RE15_KISTE_RAHMEN_TEILE, "(B %s Bild %d) %d Rahmen-Teile statt %d", wo, bild, n_teil,
          RE15_KISTE_RAHMEN_TEILE);
    if (n_teil != RE15_KISTE_RAHMEN_TEILE) return;
    /* die Rahmen-Teile tasten das Panel-Bild 1:1 an derselben Stelle ab und sind die Tabelle aus dem Header */
    {
        long flaeche = 0;
        for (t = 0; t < RE15_KISTE_RAHMEN_TEILE; t++) {
            const re15_inv_op_t *o = &s_ops[teil_i[t]];
            const re15_kiste_r37_teil_t *tl = &re15_kiste_r37_rahmen[t];
            CHECK(o->page == 0 && o->u == tl->u && o->v == tl->v && o->w == tl->w && o->h == tl->h &&
                  o->x == RE15_KISTE_PANEL_X + o->u && o->y == RE15_KISTE_PANEL_Y + o->v,
                  "(B %s Bild %d) Rahmen-Teil %d: (%d,%d)+(%d,%d) uv (%d,%d) passt nicht 1:1 aufs Panel", wo, bild,
                  t, o->x, o->y, o->w, o->h, o->u, o->v);
            flaeche += (long)o->w * o->h;
        }
        /* Teile + Fenster = ganzes Panel, ueberlappungsfrei (Flaechensumme; Tabelle ist achsparallel gestapelt) */
        CHECK(flaeche + (long)(FENSTER_X1 - FENSTER_X0 + 1) * (FENSTER_Y1 - FENSTER_Y0 + 1) ==
              (long)RE15_KISTE_PANEL_W * RE15_KISTE_PANEL_H,
              "(B %s Bild %d) Rahmen-Teile (%ld) + Fenster decken das Panel nicht genau", wo, bild, flaeche);
    }
    /* REIHENFOLGE (frueher = oben): Band < Griff < Rahmen-Teile < Zeilen < Panel-Grund */
    CHECK(band_max >= 0 && griff_i >= 0 && band_max < teil_i[0] && griff_i < teil_i[0],
          "(B %s Bild %d) Band/Griff nicht ueber dem Rahmen (Band %d, Griff %d, Rahmen %d)", wo, bild, band_max,
          griff_i, teil_i[0]);
    CHECK(panel_min > letzte_teil, "(B %s Bild %d) Panel-Grund-Op %d liegt UEBER einem Rahmen-Teil %d", wo, bild,
          panel_min, letzte_teil);
    for (i = 0; i < n; i++) {
        const re15_inv_op_t *o = &s_ops[i];
        int x1, y1;
        if (!ist_zeilen_op(o)) continue;
        /* M2: jede Zeilen-Op liegt UNTER allen Rahmen-Teilen ... */
        CHECK(i > letzte_teil, "(B %s Bild %d) Zeilen-Op %d (x %d y %d) liegt UEBER dem Rahmen (Teil %d)", wo,
              bild, i, o->x, o->y, letzte_teil);
        CHECK(i < panel_min, "(B %s Bild %d) Zeilen-Op %d unter dem Panel-Grund %d", wo, bild, i, panel_min);
        /* ... und innerhalb des Panels: ausserhalb des Fensters deckt sie damit ein Rahmen-Teil zu */
        x1 = o->x + o->w - 1; y1 = o->y + o->h - 1;
        CHECK(o->x >= RE15_KISTE_PANEL_X && x1 <= PANEL_X1 && o->y >= RE15_KISTE_PANEL_Y && y1 <= PANEL_Y1,
              "(B %s Bild %d) Zeilen-Op (%d,%d)..(%d,%d) ragt aus dem Panel (%d,%d)..(%d,%d)", wo, bild, o->x, o->y,
              x1, y1, RE15_KISTE_PANEL_X, RE15_KISTE_PANEL_Y, PANEL_X1, PANEL_Y1);
        /* Symbole bleiben waagerecht im Fenster (kein Rahmenpixel durch ein Symbol, auch in Ruhe) */
        if (o->page == RE15_INV_PAGE_ITEMTILE)
            CHECK(o->x >= FENSTER_X0 && x1 <= FENSTER_X1, "(B %s Bild %d) Symbol x %d..%d ausserhalb %d..%d", wo,
                  bild, o->x, x1, FENSTER_X0, FENSTER_X1);
    }
    /* EINLAUFENDE ZEILE (RE2 Zustand 2: scroll-1 ueber Zeile 0; Zustand 3: scroll+5 unter Zeile 4) */
    if (pixoff != 0) {
        int k = (pixoff > 0) ? RE15_KISTE_EINLAUF_HOCH : RE15_KISTE_EINLAUF_RUNTER;
        int slot = (scroll + k) & (RE15_BOX_SLOTS - 1);
        int ry = ZEILE0_Y + k * RE15_BOX_ROW_PX + pixoff, da = 0;
        for (i = 0; i < n; i++)
            if (ist_zeilen_op(&s_ops[i]) && s_ops[i].page == RE15_INV_PAGE_ITEMTILE && s_ops[i].y == ry &&
                s_ops[i].v == g_itembox.slots[slot].id)
                da = 1;
        if (g_itembox.slots[slot].id != 0) {
            CHECK(da, "(B %s Bild %d) Versatz %d: einlaufende Zeile (Platz %d, Id 0x%02X) fehlt bei y %d", wo, bild,
                  pixoff, slot, g_itembox.slots[slot].id, ry);
            if (da && pixoff > 0) s_einlauf_hoch++;
            if (da && pixoff < 0) s_einlauf_runter++;
        }
    }
    s_gleit_bilder++;
}

static void gleiten(uint16_t taste, const char *wo, int vorzeichen)
{
    int f, gesehen[7] = {0}, n_anim = 0;
    int scroll0 = re15_itembox_screen_scroll();
    press(taste);                                     /* Anstoss: Zustand 2/3, Versatz 0 */
    pruefe_bild(wo, 0);
    for (f = 1; f <= RE15_BOX_SCROLL_FRAMES + 1; f++) {
        frame(0, 0);
        {
            int p = re15_itembox_screen_pixoff();
            if (p != 0) {
                CHECK(p == vorzeichen * RE15_BOX_SCROLL_STEP_PX * f,
                      "(B %s) Bild %d: Versatz %d, erwartet %d (RE2 +-3 je Bild @0x80070118/@0x800702a4)", wo, f, p,
                      vorzeichen * RE15_BOX_SCROLL_STEP_PX * f);
                if (f <= 6) gesehen[f] = 1;
                n_anim++;
            }
        }
        pruefe_bild(wo, f);
    }
    CHECK(n_anim == RE15_BOX_SCROLL_FRAMES, "(B %s) %d Gleit-Bilder statt %d", wo, n_anim, RE15_BOX_SCROLL_FRAMES);
    CHECK(re15_itembox_screen_pixoff() == 0 && re15_itembox_screen_state() == 1 &&
          re15_itembox_screen_scroll() == ((scroll0 - vorzeichen) & (RE15_BOX_SLOTS - 1)),
          "(B %s) nach dem Commit: Versatz %d Zustand %d Scroll %d (vorher %d)", wo, re15_itembox_screen_pixoff(),
          re15_itembox_screen_state(), re15_itembox_screen_scroll(), scroll0);
    (void)gesehen;
}

int main(void)
{
    zensus_t best;
    int n_namen = 0, luecke;
    static const uint8_t k_fuellung[] = { 0x44, 0x0F, 0x43, 0x1D, 0x47, 0x46, 0x0B, 0x15, 0x5B, 0x59, 0x5A, 0x12 };
    printf("=== Runde 37 Spur M: Kisten-Liste — Symbol rechts, Rahmen ueber den Zeilen ===\n");
    if (!font_laden()) {
        fprintf(stderr, "FAIL: %s/DATA/TEX.TIM nicht lesbar (Glyphen fuer den Namens-Zensus)\n", RE15_ASSET_PSX_DIR);
        return 1;
    }

    re15_inv_init();
    re15_itembox_init();
    re15_inv_set_equipped_slot(0x80);
    re15_inv_set_prev_equip_slot(0x80);
    g_inv.slots[0].id = 0x03; g_inv.slots[0].qty = 15;
    g_inv.slots[1].id = 0x15; g_inv.slots[1].qty = 50;
    kiste_oeffnen_boxseite();
    CHECK(re15_menu_is_open() && g_inv_screen.box_mode == 1 && re15_itembox_screen_state() == 1,
          "Kiste offen + Box-Seite (open %d box_mode %d Zustand %d)", re15_menu_is_open(), g_inv_screen.box_mode,
          re15_itembox_screen_state());

    /* ---------------- (A) Zensus in drei Zustaenden ---------------- */
    {
        static const char *k_name[3] = { "Anfang", "alle Flags Leon", "alle Flags Elza" };
        int z;
        for (z = 0; z < 3; z++) {
            re15_game_state_init();
            g_gameflow.character = 0;
            if (z >= 1) memset(g_game.flags, 0xff, sizeof g_game.flags);
            if (z == 2) g_gameflow.character = 4;
            luecke = zensus(k_name[z], &best, &n_namen);
            printf("  (A) %-16s %3d Namen geprueft; laengster Name Id 0x%02X bis x %d, Symbol x %d..%d, "
                   "kleinste Luecke %d px\n", k_name[z], n_namen, best.id, best.rechts, best.sym_x,
                   best.sym_x + best.sym_w - 1, luecke);
            CHECK(n_namen >= 100, "(A %s) nur %d Namen gefunden — Namensbank nicht erreicht?", k_name[z], n_namen);
            CHECK(luecke >= 1, "(A %s) kleinste Luecke Name->Symbol %d px", k_name[z], luecke);
        }
        re15_game_state_init();
        g_gameflow.character = 0;
    }

    /* ---------------- (B) Gleiten, volle Kiste ---------------- */
    {
        int s, r;
        re15_itembox_init();
        for (s = 0; s < RE15_BOX_SLOTS; s++) {
            uint8_t id = k_fuellung[s % (int)sizeof k_fuellung];
            g_itembox.slots[s].id = id;
            g_itembox.slots[s].qty = (uint8_t)(1 + (s * 37) % 120);
            g_itembox.slots[s].flags = ist_breit(id) ? RE15_BOX_KIND_WIDE : 0;
        }
        g_inv_screen.box_scroll = 0;
        pruefe_bild("Ruhe", 0);
        for (r = 0; r < 3; r++) gleiten(RE15_PAD_BIT_DOWN, "RUNTER", -1);
        for (r = 0; r < 3; r++) gleiten(RE15_PAD_BIT_UP, "HOCH", +1);
        /* Ring-Naht: von 0 aus HOCH laeuft Platz 63 ein (RE2 (scroll-1)&0x3f) */
        gleiten(RE15_PAD_BIT_UP, "HOCH ueber die Naht", +1);
        printf("  (B) %d Bilder geprueft (Rahmen-Teile ueber allen Zeilen, Zeilen im Panel), einlaufende Zeile "
               "HOCH in %d / RUNTER in %d Gleit-Bildern\n", s_gleit_bilder, s_einlauf_hoch, s_einlauf_runter);
        CHECK(s_einlauf_hoch == 4 * RE15_BOX_SCROLL_FRAMES && s_einlauf_runter == 3 * RE15_BOX_SCROLL_FRAMES,
              "(B) einlaufende Zeile HOCH %d / RUNTER %d Bilder, erwartet %d / %d", s_einlauf_hoch,
              s_einlauf_runter, 4 * RE15_BOX_SCROLL_FRAMES, 3 * RE15_BOX_SCROLL_FRAMES);
    }

    /* ---------------- (C) unveraendert: Band, Griff, Panel-Ops ---------------- */
    {
        int n = re15_inv_screen_build(&g_inv_screen, s_ops, RE15_INV_MAX_OPS), i, nb = 0, ng = 0, np = 0;
        int sy0 = ZEILE0_Y + RE15_BOX_PICK_ROW * RE15_BOX_ROW_PX - 1;
        for (i = 0; i < n; i++) {
            const re15_inv_op_t *o = &s_ops[i];
            if (o->kind == RE15_INV_OP_FILL && o->x == 12 && o->w == 192 && o->h == 2) {
                CHECK((o->y == sy0 || o->y == sy0 + RE15_BOX_ROW_PX) && o->r == 200 && o->g == 48 && o->b == 48,
                      "(C) Auswahl-Band y %d rgb(%d,%d,%d) veraendert", o->y, o->r, o->g, o->b);
                nb++;
            } else if (o->kind == RE15_INV_OP_FILL && o->x == 208 && o->w == 6) {
                int gh = (100 * RE15_BOX_WINDOW) / RE15_BOX_SLOTS;
                CHECK(o->h == gh && o->y == 42 + (RE15_KISTE_PANEL_Y - 13) + (100 - gh) * g_inv_screen.box_scroll /
                      (RE15_BOX_SLOTS - 1) && o->r == 232 && o->g == 248 && o->b == 72,
                      "(C) Scrollgriff y %d h %d veraendert", o->y, o->h);
                ng++;
            } else if (o->kind == RE15_INV_OP_PANEL) {
                CHECK(o->x == RE15_KISTE_PANEL_X && o->w == RE15_KISTE_PANEL_W &&
                      ((o->v == 0 && o->y == RE15_KISTE_PANEL_Y && o->h == RE15_KISTE_PANEL_H) ||
                       (o->v == 1 && o->y == RE15_KISTE_PANEL_Y + RE15_KISTE_PANEL_H && o->h == 38)),
                      "(C) Panel-Op %d (%d,%d) %dx%d veraendert", o->v, o->x, o->y, o->w, o->h);
                np++;
            }
        }
        CHECK(nb == 2 && ng == 1 && np == 2, "(C) Band %d / Griff %d / Panel %d Ops (erwartet 2/1/2)", nb, ng, np);
        printf("  (C) Auswahl-Band, Scrollgriff, Panel-Ops unveraendert\n");
    }

    if (fails) { printf("=== %d FEHLER ===\n", fails); return 1; }
    printf("=== OK ===\n");
    return 0;
}
