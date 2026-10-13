/* test_r37_kiste2feld.c — Runde 37 Spur R "kiste2feld" (Orchestrator-Auftrag R zu Nachtrag 9 des Nutzers).
 * Dossier analysis/befunde_runde37/R_kiste2feld.md.
 *
 *   Nachtrag 9: "In der Kiste muessen die Icons offenbar so weit wie moeglich rechts liegen, damit sie nicht vom
 *   Text ueberdeckt werden." (Lage: Spur M) — Auftrag R: die 2-Feld-Symbole (49 px, Waffen) in der Kisten-Liste
 *   VOLLSTAENDIG wie RE2 zeichnen (vorher: nur die 40er-Kachel, Spalten 40..48 = Spalte 39), auch im Inventar-Gitter.
 *
 * Engine-Riegel (der Pixel-Riegel an der exe ist integration_r37_kiste2feld):
 *   (A) ZEILEN-OP-VERTRAG fuer ALLE Item-Ids 1..255 auf der Auswahlzeile der Kisten-Liste (echte Kisten-FSM, echte
 *       Op-Liste re15_inv_screen_build Block 7b): das Symbol ist EIN ITEMTILE-SPRT mit v = Id und u = QUELL-SPANNE
 *       wie RE2s POLY_FT4 u1 - u0 (FUN_800710dc, u0 = u2 `addiu t0,zero,40` @0x800717b4):
 *         2-Feld (Size 3)  49 x 19, Spanne 80 (u1 = u3 `addiu t1,zero,120` @0x8007183c, `sb t1,-14(s0)` @0x80071848)
 *         schmal           25 x 19, Spanne 40 (`addiu t1,zero,80` @0x80071844)
 *       Vorher trug die Op u = 0 -> der Zeichner nahm die 40er-Kachel 1:1 und wiederholte Spalte 39.
 *   (B) GITTER-ZUORDNUNG: jede Inventar-Zelle einer 2-Feld-Waffe traegt ihre EIGENE Haelfte des 80x30-Breitbilds —
 *       Kopf (Flag 1) Spalten 0..39, Schwanz (Flag 2) 40..79 (Original: LoadImage 80x30 ueber Zelle 0/1, FUN_8004dc4c
 *       @0x8004de44-48 -> FUN_800492b8 @0x8004df08; danach MoveImage je 40x30-Zelle FUN_80049390 @0x8004945c) — fuer
 *       Kopfzellen in JEDER Speicher-Spalte (0, 2, 5, 8). Jede Zelle, fuer die der Symbol-Speicher das Breitbild
 *       verlangt (re15_inv_screen_cache_tile == -2), hat eine Zuordnung; schmale/leere Zellen keine.
 *   (C) GPU-ABTASTUNG (re15_kiste2feld_texel): fuer 80 -> 49, 30 -> 19, 40 -> 25 Pixel fuer Pixel gleich der
 *       geschlossenen Form (p*n_tex + r)/n_px aus dem Dossier §2.3 (DuckStation-Rasterizer, Fixkomma 12, Start + 1/2);
 *       die Spalten 80 -> 49 und die Zeilen 30 -> 19 steigen STRENG (keine wiederholte Spalte/Zeile), alle Texel im
 *       Quellbild; n -> n ist die Identitaet (alter 1:1-Pfad des 80er-Breitbilds unveraendert).
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
#include "re15_kiste_r37.h"
#include "re15_kiste2feld_r37.h"

static int fails = 0;
#define CHECK(cond, ...) do { if (!(cond)) { fprintf(stderr, "FAIL: " __VA_ARGS__); \
    fprintf(stderr, "\n"); fails++; } } while (0)

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

static int ist_breit(uint8_t id) { return (uint8_t)(id - 0x0e) < 6; }   /* RE1.5 `addiu -14; sltiu 6` @0x8004dc74-78 */

static void inv_set(int k, uint8_t id, uint8_t qty, uint8_t kind)
{
    g_inv.slots[k].id = id; g_inv.slots[k].qty = qty; g_inv.slots[k].flags = kind;
}

int main(void)
{
    int id, k, p;
    printf("=== Runde 37 Spur R: 2-Feld-Symbole wie RE2 (Kisten-Liste + Inventar-Gitter) ===\n");

    /* ---------------- (A) Zeilen-Op-Vertrag, alle Ids ---------------- */
    re15_inv_init();
    re15_itembox_init();
    re15_inv_set_equipped_slot(0x80);
    re15_inv_set_prev_equip_slot(0x80);
    inv_set(0, 0x03, 15, 0);
    inv_set(1, 0x15, 50, 0);
    if (re15_menu_is_open()) { re15_menu_toggle(); idle(2); }
    re15_menu_toggle_box();
    idle(1);
    press(RE15_PAD_BIT_SQUARE);                       /* Bestaetigen -> Box-Seite (Zustand 1) */
    CHECK(re15_menu_is_open() && g_inv_screen.box_mode == 1 && re15_itembox_screen_state() == 1,
          "Kiste offen + Box-Seite (open %d box_mode %d Zustand %d)", re15_menu_is_open(), g_inv_screen.box_mode,
          re15_itembox_screen_state());
    {
        int n_breit = 0, n_schmal = 0;
        for (id = 1; id <= 255; id++) {
            int i, n, gefunden = 0;
            re15_itembox_init();
            g_itembox.slots[RE15_BOX_PICK_ROW].id = (uint8_t)id;
            g_itembox.slots[RE15_BOX_PICK_ROW].qty = 7;
            g_itembox.slots[RE15_BOX_PICK_ROW].flags = ist_breit((uint8_t)id) ? RE15_BOX_KIND_WIDE : 0;
            n = re15_inv_screen_build(&g_inv_screen, s_ops, RE15_INV_MAX_OPS);
            for (i = 0; i < n; i++) {
                const re15_inv_op_t *o = &s_ops[i];
                if (o->kind != RE15_INV_OP_SPRT || o->page != RE15_INV_PAGE_ITEMTILE) continue;
                gefunden++;
                CHECK(o->v == id, "(A) Id 0x%02X: Symbol-Op v %d", id, o->v);
                if (ist_breit((uint8_t)id)) {
                    CHECK(o->w == 2 * RE15_BOX_ICON_W - 1 && o->h == RE15_BOX_ICON_H &&
                          o->u == RE15_KISTE2FELD_SPANNE_BREIT && RE15_KISTE2FELD_SPANNE_BREIT == 120 - 40,
                          "(A) 2-Feld-Id 0x%02X: Symbol %dx%d Spanne u %d (erwartet 49x19 Spanne 80 = RE2 u 40..120 "
                          "@0x800717b4/@0x8007183c)", id, o->w, o->h, o->u);
                } else {
                    CHECK(o->w == RE15_BOX_ICON_W && o->h == RE15_BOX_ICON_H &&
                          o->u == RE15_KISTE2FELD_SPANNE_SCHMAL && RE15_KISTE2FELD_SPANNE_SCHMAL == 80 - 40,
                          "(A) Id 0x%02X: Symbol %dx%d Spanne u %d (erwartet 25x19 Spanne 40 = RE2 u 40..80 "
                          "@0x800717b4/@0x80071844)", id, o->w, o->h, o->u);
                }
            }
            CHECK(gefunden == 1, "(A) Id 0x%02X: %d Symbol-Ops statt 1", id, gefunden);
            if (ist_breit((uint8_t)id)) n_breit++; else n_schmal++;
        }
        printf("  (A) %d Ids geprueft: %d 2-Feld (49x19, Spanne 80 = RE2 u 40..120), %d schmal (25x19, Spanne 40)\n",
               n_breit + n_schmal, n_breit, n_schmal);
        CHECK(n_breit == 6, "(A) %d 2-Feld-Ids statt 6 (0x0e..0x13)", n_breit);
    }
    if (re15_menu_is_open()) { re15_menu_toggle(); idle(2); }

    /* ---------------- (B) Gitter: Haelfte je Zelle, Kopf in jeder Speicher-Spalte ---------------- */
    {
        /* Belegung wie probe_r37_kiste2feld_karte "alle": Kopf 0/2/5/8 = Speicher-Spalte 0/2/2/2 (Zelle k bei
         * u (k%3)*40, RE1.5 Tabelle @0x80076244) */
        static const struct { int kopf; uint8_t id; } k_breit[4] = { { 0, 0x0F }, { 2, 0x12 }, { 5, 0x13 }, { 8, 0x0E } };
        int n_kopf = 0, n_schwanz = 0;
        re15_inv_init();
        for (k = 0; k < RE15_INV_MAX_SLOTS; k++) inv_set(k, 0, 0, 0);
        for (k = 0; k < 4; k++) {
            inv_set(k_breit[k].kopf, k_breit[k].id, 9, 1);
            inv_set(k_breit[k].kopf + 1, k_breit[k].id, 9, 2);
        }
        inv_set(4, 0x03, 15, 0);
        inv_set(7, 0x15, 50, 0);
        for (k = 0; k < 10; k++) {
            uint8_t zid = 0;
            int sp = -1, r = re15_kiste2feld_zelle(k, &zid, &sp);
            int erwartet = (g_inv.slots[k].flags == 1 || g_inv.slots[k].flags == 2);
            CHECK(r == erwartet, "(B) Zelle %d (Id 0x%02X Flag %d): Zuordnung %d, erwartet %d", k, g_inv.slots[k].id,
                  g_inv.slots[k].flags, r, erwartet);
            CHECK((re15_inv_screen_cache_tile(k) == -2) == (r == 1),
                  "(B) Zelle %d: Symbol-Speicher verlangt Breitbild (%d) ohne passende Zuordnung (%d)", k,
                  re15_inv_screen_cache_tile(k), r);
            if (r) {
                CHECK(zid == g_inv.slots[k].id, "(B) Zelle %d: Id 0x%02X statt 0x%02X", k, zid, g_inv.slots[k].id);
                CHECK(sp == (g_inv.slots[k].flags == 1 ? 0 : RE15_KISTE2FELD_HAELFTE_W),
                      "(B) Zelle %d (Flag %d): erste Breitbild-Spalte %d — Kopf 0..39, Schwanz 40..79", k,
                      g_inv.slots[k].flags, sp);
                if (g_inv.slots[k].flags == 1) n_kopf++; else n_schwanz++;
            }
        }
        CHECK(RE15_KISTE2FELD_HAELFTE_W * 2 == RE15_KISTE2FELD_BREIT_W && RE15_KISTE2FELD_BREIT_W == 0x28 * 2 &&
              RE15_KISTE2FELD_HAELFTE_W == 0x14 * 2 && RE15_KISTE2FELD_BREIT_H == 0x1e,
              "(B) Masse: Breitbild 0x28 hw x 0x1e (@0x80049314/@0x8004931c), Zelle 0x14 hw (@0x800493ac)");
        CHECK(re15_kiste2feld_zelle(-1, NULL, NULL) == 0 && re15_kiste2feld_zelle(10, NULL, NULL) == 0,
              "(B) Zellen ausserhalb 0..9 ohne Zuordnung");
        printf("  (B) Gitter: %d Kopfzellen (Spalten 0..39) + %d Schwanzzellen (Spalten 40..79), Kopf in Speicher-"
               "Spalte 0 und 2; schmale/leere Zellen ohne Breitbild\n", n_kopf, n_schwanz);
        CHECK(n_kopf == 4 && n_schwanz == 4, "(B) %d Koepfe / %d Schwaenze statt 4/4", n_kopf, n_schwanz);
    }

    /* ---------------- (C) GPU-Abtastung ---------------- */
    {
        static const struct { int n_tex, n_px, r; const char *was; } k_fall[3] = {
            { 80, 49, 24, "2-Feld-Spalten 80 -> 49" }, { 30, 19, 9, "Zeilen 30 -> 19" }, { 40, 25, 12, "schmal 40 -> 25" }
        };
        int f;
        for (f = 0; f < 3; f++) {
            int vorher = -1, streng = 1;
            for (p = 0; p < k_fall[f].n_px; p++) {
                int t = re15_kiste2feld_texel(k_fall[f].n_tex, k_fall[f].n_px, p);
                int soll = (p * k_fall[f].n_tex + k_fall[f].r) / k_fall[f].n_px;
                CHECK(t == soll, "(C) %s: Pixel %d -> Texel %d, erwartet %d", k_fall[f].was, p, t, soll);
                CHECK(t >= 0 && t < k_fall[f].n_tex, "(C) %s: Pixel %d -> Texel %d ausserhalb", k_fall[f].was, p, t);
                if (t <= vorher) streng = 0;
                vorher = t;
            }
            if (k_fall[f].n_tex > k_fall[f].n_px)
                CHECK(streng, "(C) %s: wiederholte Spalte/Zeile", k_fall[f].was);
            printf("  (C) %s: %d Pixel = (p*%d+%d)/%d, streng steigend %s\n", k_fall[f].was, k_fall[f].n_px,
                   k_fall[f].n_tex, k_fall[f].r, k_fall[f].n_px, streng ? "ja" : "nein");
        }
        for (p = 0; p < 80; p++)
            CHECK(re15_kiste2feld_texel(80, 80, p) == p && (p >= 40 || re15_kiste2feld_texel(40, 40, p) == p),
                  "(C) Identitaet n -> n verletzt bei %d", p);
    }

    if (fails) { printf("=== %d FEHLER ===\n", fails); return 1; }
    printf("=== OK ===\n");
    return 0;
}
