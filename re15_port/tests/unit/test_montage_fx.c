/* test_montage_fx.c — PIN (Nutzer-Auftrag 2026-08-30: „In Resident Evil 2 gibt es mehr
 * oder weniger exakt das gleiche Pre-Intro wie wir im Port mit statischen Bildern haben,
 * nur besser. uebernimm mir das." + „Es gibt bei den einen Bild auch eine Bewegung von
 * oben nach unten.").
 *
 * Geprueft werden die aus RE2s OPENING.BIN uebernommenen RATEN (Element-Renderer
 * 0x801c1a0c; Herleitung analysis/preintro_re2/re2-bewegung.md + re2-renderer.md):
 *   - Einblend-Rampe +2/Frame bis 0x60 (@0x801c1b94-98, Klemme @0x801c1bb8-bc) = 48 Frames
 *   - Ausblend-Rampe -2/Frame (@0x801c1be4-ec) = 48 Frames, UEBERLAPPEND (Kreuzblende:
 *     RE2 setzt Aus- und Einblendung im selben Frame, hide_others=0)
 *   - vertikales Wandern 1 px je 11 Frames (Zaehler-Reload 10 @0x801c1a60, Y += Puls
 *     @0x801c1c44), begrenzt auf den RE2-Ueberhang von 80 px
 *   - zentrierter Zoom 2 px je 4 Frames (Reload 3 @0x801c1aa4, W/H += Puls<<1 @0x801c1d94)
 *   - RE15_MONTAGE_STOCK=1 = byte-true RE1.5-Hartschnitt (Schicht komplett aus)
 */
#include "re15_montage_fx.h"

#include <stdio.h>
#include <string.h>

static int g_fail = 0;
#define CHECK(name, cond) do { if (!(cond)) { printf("  FAIL: %s\n", name); g_fail = 1; } \
                               else printf("  PASS: %s\n", name); } while (0)

static void ticks(int n) { for (int i = 0; i < n; i++) re15_montage_fx_tick(); }

int main(void)
{
    printf("=== Pre-Intro-Montage: RE2-Praesentationsraten ===\n");
    re15_montage_fx_stock_set(0);

    /* --- (1) Einblend-Rampe: +2/Frame, Klemme 0x60, also GENAU 48 Frames --- */
    re15_montage_fx_set_active(1);
    re15_montage_fx_on_cut(1, 0);                 /* erstes Bild, kein Vorgaenger */
    CHECK("Start bei 0 (Bild glueht aus dem Schwarz auf)", re15_montage_fx_level_new() == 0);
    ticks(1);
    CHECK("nach 1 Frame Helligkeit 2 (+0x00020202 @0x801c1b94)",
          re15_montage_fx_level_new() == 2);
    ticks(46);                                    /* insgesamt 47 */
    CHECK("nach 47 Frames noch unter der Klemme", re15_montage_fx_level_new() == 94);
    ticks(1);                                     /* 48 */
    CHECK("nach 48 Frames exakt 0x60 (Klemme @0x801c1bb8-bc)",
          re15_montage_fx_level_new() == re15_montage_fx_level_max());
    ticks(30);
    CHECK("bleibt auf 0x60 stehen (kein Ueberlauf)",
          re15_montage_fx_level_new() == re15_montage_fx_level_max());

    /* --- (2) Kreuzblende: der naechste Cut blendet das alte Bild AUS, waehrend das
     *         neue AUFBLENDET — beide Ebenen gleichzeitig sichtbar (RE2 hide_others=0) --- */
    re15_montage_fx_on_cut(2, 1);
    CHECK("neues Bild startet bei 0", re15_montage_fx_level_new() == 0);
    CHECK("altes Bild uebernimmt seinen Stand 0x60",
          re15_montage_fx_level_prev() == re15_montage_fx_level_max());
    ticks(24);
    CHECK("Mitte der Blende: BEIDE Ebenen sichtbar (Ueberlappung)",
          re15_montage_fx_level_new() == 48 && re15_montage_fx_level_prev() == 48);
    ticks(24);
    CHECK("nach 48 Frames: neu voll, alt weg",
          re15_montage_fx_level_new() == re15_montage_fx_level_max()
          && re15_montage_fx_level_prev() == 0);

    /* --- (3) Die BEWEGUNG (Nutzer-Beobachtung): 1 px je 11 Frames, Cut 5 = Pan-Bild --- */
    re15_montage_fx_on_cut(5, 1);
    CHECK("Pan-Bild startet ohne Versatz", re15_montage_fx_pan_y() == 0);
    CHECK("Pan-Bild traegt den RE2-Ueberhang als Sockel (80 px)",
          re15_montage_fx_base_zoom() == 80);
    ticks(10);
    CHECK("nach 10 Frames noch kein Schritt (Periode 11)", re15_montage_fx_pan_y() == 0);
    ticks(1);
    CHECK("Schritt 1 bei Frame 11 (Zaehler-Reload 10 @0x801c1a60)",
          re15_montage_fx_pan_y() == -1);
    ticks(11);
    CHECK("Schritt 2 bei Frame 22", re15_montage_fx_pan_y() == -2);
    ticks(11 * 200);
    CHECK("Wandern endet am Ueberhang (-80), laeuft nicht aus dem Bild",
          re15_montage_fx_pan_y() == -80);

    /* --- (4) Zoom: 2 px je 4 Frames, zentriert (Cut 1 = Zoom-Bild) --- */
    re15_montage_fx_on_cut(1, 1);
    CHECK("Zoom startet bei 0", re15_montage_fx_zoom_px() == 0);
    CHECK("Zoom-Bild hat keinen Pan-Sockel", re15_montage_fx_base_zoom() == 0);
    ticks(3);
    CHECK("nach 3 Frames noch kein Schritt (Periode 4)", re15_montage_fx_zoom_px() == 0);
    ticks(1);
    CHECK("Schritt bei Frame 4: +2 px (Reload 3 @0x801c1aa4, Puls<<1 @0x801c1d94)",
          re15_montage_fx_zoom_px() == 2);
    ticks(4 * 10);
    CHECK("nach 44 Frames: 11 Schritte = +22 px", re15_montage_fx_zoom_px() == 22);
    CHECK("Pan bleibt beim Zoom-Bild aus", re15_montage_fx_pan_y() == 0);

    /* --- (5) Bilder ohne Bewegung (Logos): nur die Blende --- */
    re15_montage_fx_on_cut(3, 1);                  /* S.T.A.R.S.-Abzeichen */
    ticks(60);
    CHECK("Logo-Bild ohne Wandern/Zoom",
          re15_montage_fx_pan_y() == 0 && re15_montage_fx_zoom_px() == 0);
    CHECK("blendet trotzdem auf", re15_montage_fx_level_new() == re15_montage_fx_level_max());

    /* --- (6) STOCK-Schalter: byte-true Hartschnitt, Schicht komplett aus --- */
    re15_montage_fx_stock_set(1);
    CHECK("RE15_MONTAGE_STOCK=1 schaltet die Schicht ab", re15_montage_fx_active() == 0);
    re15_montage_fx_on_cut(6, 1);
    ticks(20);
    CHECK("im Stock-Modus laufen keine Rampen", re15_montage_fx_level_new() == 0);
    re15_montage_fx_stock_set(0);

    /* --- (7) Raum verlassen setzt zurueck --- */
    re15_montage_fx_set_active(1);
    re15_montage_fx_on_cut(2, 0);
    ticks(10);
    re15_montage_fx_set_active(0);
    CHECK("ausserhalb des Montage-Raums inaktiv", re15_montage_fx_active() == 0);
    re15_montage_fx_set_active(1);
    CHECK("Wiedereintritt startet sauber bei 0", re15_montage_fx_level_new() == 0
          && re15_montage_fx_pan_y() == 0 && re15_montage_fx_zoom_px() == 0);

    if (g_fail) { printf("FAIL\n"); return 1; }
    printf("OK\n");
    return 0;
}
