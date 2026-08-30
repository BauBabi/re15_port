/* test_montage_fx.c — PIN fuer die Pre-Intro-Praesentation (Nutzer-Auftrag 2026-08-30:
 * RE2s Pre-Intro uebernehmen; Nachbesserungen "Es gibt bei den einen Bild auch eine
 * Bewegung von oben nach unten" und "es muss bis in das naechste Standbild zoomen.
 * Bei dir wird das Logo ploetzlich wieder gross").
 *
 * Geprueft werden die aus RE2s OPENING.BIN uebernommenen RATEN und das ZWEI-EBENEN-
 * MODELL (Element-Renderer 0x801c1a0c; analysis/preintro_re2/re2-bewegung.md):
 *   - Einblend-Rampe +2/Frame bis 0x60 (@0x801c1b94-98, Klemme @0x801c1bb8-bc)
 *   - Ausblend-Rampe -2/Frame (@0x801c1be4-ec), UEBERLAPPEND = Kreuzblende
 *   - vertikales Wandern 1 px je 11 Frames (Reload 10 @0x801c1a60, Y += Puls @0x801c1c44)
 *   - zentrierter Zoom 2 px je 4 Frames (Reload 3 @0x801c1aa4, W/H += Puls<<1 @0x801c1d94)
 *   - DIE BEWEGUNG LAEUFT UEBER DEN BILDWECHSEL WEITER: RE2s Zustand ist ein Bitfeld,
 *     beim Umschalten von Ein- auf Ausblenden kippt nur das Blend-Bit, das
 *     Bewegungs-Bit bleibt (71 -> 75, 39 -> 43, 23 -> 27)
 *   - RE15_MONTAGE_STOCK=1 = byte-true RE1.5-Hartschnitt (Schicht komplett aus)
 */
#include "re15_montage_fx.h"

#include <stdio.h>

static int g_fail = 0;
#define CHECK(name, cond) do { if (!(cond)) { printf("  FAIL: %s\n", name); g_fail = 1; } \
                               else printf("  PASS: %s\n", name); } while (0)

static void ticks(int n) { for (int i = 0; i < n; i++) re15_montage_fx_tick(); }

int main(void)
{
    printf("=== Pre-Intro-Montage: RE2-Praesentation ===\n");
    re15_montage_fx_stock_set(0);

    /* --- (1) Einblend-Rampe: +2/Frame, Klemme 0x60 = genau 48 Frames --- */
    re15_montage_fx_set_active(1);
    re15_montage_fx_on_cut(1, 0);
    CHECK("Start bei 0 (Bild glueht aus dem Schwarz auf)", re15_montage_fx_level_new() == 0);
    ticks(1);
    CHECK("nach 1 Frame Helligkeit 2 (+0x00020202 @0x801c1b94)",
          re15_montage_fx_level_new() == 2);
    ticks(47);
    CHECK("nach 48 Frames exakt 0x60 (Klemme @0x801c1bb8-bc)",
          re15_montage_fx_level_new() == re15_montage_fx_level_max());
    ticks(30);
    CHECK("bleibt auf 0x60 stehen (kein Ueberlauf)",
          re15_montage_fx_level_new() == re15_montage_fx_level_max());

    /* --- (2) DIE BILD-ZUORDNUNG (Nutzer-Vorgabe, gegen RE2s Timeline geprueft) --- */
    re15_montage_fx_on_cut(1, 0);
    CHECK("Bild 1 (Zombie) ist ein STANDBILD", re15_montage_fx_mode() == RE15_MFX_NONE);
    re15_montage_fx_on_cut(2, 0);
    CHECK("Bild 2 (T-Virus) WANDERT", re15_montage_fx_mode() == RE15_MFX_PAN);
    re15_montage_fx_on_cut(3, 0);
    CHECK("Bild 3 (S.T.A.R.S.-Logo) zoomt HERAUS", re15_montage_fx_mode() == RE15_MFX_ZOOM_OUT);
    re15_montage_fx_on_cut(4, 0);
    CHECK("Bild 4 (Villa) ist ein STANDBILD", re15_montage_fx_mode() == RE15_MFX_NONE);
    re15_montage_fx_on_cut(5, 0);
    CHECK("Bild 5 (Helikopter) ist ein STANDBILD", re15_montage_fx_mode() == RE15_MFX_NONE);
    re15_montage_fx_on_cut(8, 0);
    CHECK("Bild 8 (Umbrella-Logo) zoomt HINEIN", re15_montage_fx_mode() == RE15_MFX_ZOOM_IN);

    /* --- (3) Wandern: 1 px je 11 Frames, mit dem RE2-Ueberhang als Sockel --- */
    re15_montage_fx_set_active(0);
    re15_montage_fx_set_active(1);
    re15_montage_fx_on_cut(2, 0);
    CHECK("Wander-Bild startet ohne Versatz", re15_montage_fx_pan_y() == 0);
    CHECK("Wander-Bild traegt den RE2-Ueberhang (80 px)",
          re15_montage_fx_base_zoom() == 80);
    ticks(10);
    CHECK("nach 10 Frames noch kein Schritt (Periode 11)", re15_montage_fx_pan_y() == 0);
    ticks(1);
    CHECK("Schritt 1 bei Frame 11 (Reload 10 @0x801c1a60)", re15_montage_fx_pan_y() == -1);
    ticks(11);
    CHECK("Schritt 2 bei Frame 22", re15_montage_fx_pan_y() == -2);
    ticks(11 * 200);
    CHECK("Wandern endet am Ueberhang (-80), laeuft nicht aus dem Bild",
          re15_montage_fx_pan_y() == -80);

    /* --- (4) Zoom: 2 px je 4 Frames --- */
    re15_montage_fx_set_active(0);
    re15_montage_fx_set_active(1);
    re15_montage_fx_on_cut(8, 0);
    ticks(3);
    CHECK("nach 3 Frames noch kein Schritt (Periode 4)", re15_montage_fx_zoom_px() == 0);
    ticks(1);
    CHECK("Schritt bei Frame 4: +2 px (Reload 3 @0x801c1aa4)", re15_montage_fx_zoom_px() == 2);
    ticks(40);
    CHECK("nach 44 Frames: 11 Schritte = +22 px", re15_montage_fx_zoom_px() == 22);

    /* --- (5) ⛔ DER KERN-FIX: die Bewegung LAEUFT UEBER DEN BILDWECHSEL WEITER.
     *      Genau hier sah der Nutzer "das Logo wird ploetzlich wieder gross". --- */
    re15_montage_fx_set_active(0);
    re15_montage_fx_set_active(1);
    re15_montage_fx_on_cut(3, 0);                 /* Logo, zoomt heraus */
    ticks(40);                                    /* 10 Schritte = -20 px */
    {
        int z_before = re15_montage_fx_zoom_px();
        CHECK("Logo hat vor dem Wechsel herausgezoomt", z_before == -20);
        re15_montage_fx_on_cut(4, 1);             /* naechstes Bild (Standbild) */
        CHECK("das Logo behaelt beim Wechsel SEINEN Zoom (springt NICHT zurueck)",
              re15_montage_fx_prev_zoom_px() == z_before);
        CHECK("das Logo behaelt seinen Bewegungs-Modus (RE2: 71 -> 75, Bit bleibt)",
              re15_montage_fx_prev_mode() == RE15_MFX_ZOOM_OUT);
        CHECK("das neue Standbild bewegt sich selbst nicht",
              re15_montage_fx_mode() == RE15_MFX_NONE);
        ticks(8);                                 /* 2 weitere Zoom-Schritte */
        CHECK("und es zoomt WEITER, waehrend das naechste Bild schon da ist",
              re15_montage_fx_prev_zoom_px() == z_before - 4);
        CHECK("das neue Bild bleibt dabei unbewegt",
              re15_montage_fx_zoom_px() == 0 && re15_montage_fx_pan_y() == 0);
        CHECK("beide Ebenen sind gleichzeitig sichtbar (Kreuzblende)",
              re15_montage_fx_level_new() > 0 && re15_montage_fx_level_prev() > 0);
    }

    /* --- (6) dasselbe fuer das Wandern (RE2 23 -> 27) --- */
    re15_montage_fx_set_active(0);
    re15_montage_fx_set_active(1);
    re15_montage_fx_on_cut(2, 0);                 /* Wander-Bild */
    ticks(33);                                    /* 3 Schritte */
    {
        int p_before = re15_montage_fx_pan_y();
        CHECK("Wander-Bild hat sich bewegt", p_before == -3);
        re15_montage_fx_on_cut(3, 1);
        CHECK("es behaelt seinen Wander-Stand", re15_montage_fx_prev_pan_y() == p_before);
        CHECK("und seinen Ueberhang-Sockel", re15_montage_fx_prev_base_zoom() == 80);
        ticks(11);
        CHECK("und wandert weiter, waehrend das naechste Bild laeuft",
              re15_montage_fx_prev_pan_y() == p_before - 1);
    }

    /* --- (7) STOCK-Schalter: byte-true Hartschnitt --- */
    re15_montage_fx_stock_set(1);
    CHECK("RE15_MONTAGE_STOCK=1 schaltet die Schicht ab", re15_montage_fx_active() == 0);
    re15_montage_fx_on_cut(6, 1);
    ticks(20);
    CHECK("im Stock-Modus laufen keine Rampen", re15_montage_fx_level_new() == 0);
    re15_montage_fx_stock_set(0);

    /* --- (8) Raum verlassen setzt zurueck --- */
    re15_montage_fx_set_active(1);
    re15_montage_fx_on_cut(2, 0);
    ticks(20);
    re15_montage_fx_set_active(0);
    CHECK("ausserhalb des Montage-Raums inaktiv", re15_montage_fx_active() == 0);
    re15_montage_fx_set_active(1);
    CHECK("Wiedereintritt startet sauber", re15_montage_fx_level_new() == 0
          && re15_montage_fx_pan_y() == 0 && re15_montage_fx_zoom_px() == 0
          && re15_montage_fx_prev_zoom_px() == 0);

    if (g_fail) { printf("FAIL\n"); return 1; }
    printf("OK\n");
    return 0;
}
