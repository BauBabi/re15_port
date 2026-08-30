/*
 * re15_montage_fx.c — PRAESENTATIONS-SCHICHT fuer die Standbild-Montage (Pre-Intro).
 *
 * NUTZER-AUFTRAG 2026-08-30: „In Resident Evil 2 gibt es mehr oder weniger exakt das
 * gleiche Pre-Intro wie wir im Port mit statischen Bildern haben, nur besser. uebernimm
 * mir das." + Nachtrag: „Es gibt bei den einen Bild auch eine Bewegung von oben nach unten."
 *
 * WAS BLEIBT RE1.5-BYTE-TRUE: Bild-Reihenfolge, Standzeiten (SUB2-Sleep-Kette, Summe 1422
 * Ticks), Texte + Sprachausgabe, Skip (SQUARE gehalten -> SUB3), der Handoff nach ROOM1170.
 * Dieses Modul aendert AUSSCHLIESSLICH, WIE der Bildwechsel aussieht.
 *
 * WAS VON RE2 KOMMT (OPENING.BIN, geladen @0x801bfa18; Element-Renderer 0x801c1a0c —
 * selbst disassembliert, analysis/preintro_re2/re2-bewegung.md):
 *   Der Element-Zustand ist ein BITFELD: Bit2 = EINBLENDEN, Bit3 = AUSBLENDEN,
 *   Bit4 = vertikales Wandern, Bit5/6 = Zoom (die Sequenzer-„Codes" 7/11/23/27/39/43/71/75).
 *   - Helligkeits-Rampe: gepacktes RGB-Wort im Element-Record +6(s4),
 *       EIN  += 0x00020202/Frame (= +2 je Kanal), Klemme 0x0060605f -> 0x00606060
 *            (@0x801c1b94-bd0 Typ 0, @0x801c1ce4-d20 Typ 2)  => 0 -> 96 in 48 Frames
 *       AUS  += 0xfffdfdfe (= -2) @0x801c1be4-c0c (Typ 0)     => 96 -> 0 in 48 Frames
 *       AUS  += 0xfffefeff (= -1) @0x801c1d34-d5c (Typ 2)     => 96 -> 0 in 96 Frames
 *     Prim = Code 0x2E (POLY_FT4 SEMI-TRANSPARENT, `lui a0,0x2e00` @0x801c1c14/@0x801c1d64).
 *   - Schritt-Takte (Renderer-Kopf @0x801c1a0c-abc): Zaehler 0x801c2ee0 Reload 10 ->
 *     Impuls-Zelle 0x801c2edc, Zaehler 0x801c2ee2 Reload 3 -> Zelle 0x801c2ede; beide
 *     liefern alle N Frames genau EINEN Schritt (-1), sonst 0.
 *   - Vertikales Wandern (Bit4): `lhu 4(s4) + lhu 0x801c2edc -> sh 4(s4)` @0x801c1c34-48
 *     => 1 Pixel je 10 Frames.
 *   - Zoom (Bit5/6): `lbu 8(s5)/9(s5) -/+ (impuls<<1)` @0x801c1d84-df4 => 2 Pixel je 3
 *     Frames auf BREITE UND HOEHE, danach zentriert: `(320-w)/2`, `(240-h)/2`
 *     @0x801c1df8-e24.
 *
 * PORT-WAHL (klar benannt, weil RE1.5 dafuer keine Daten hat): WELCHES unserer 9 Bilder
 * welchen Bewegungs-Modus bekommt (Tabelle s_cut_mode unten). RE2 legt das je Element in
 * seiner Phasen-Timeline fest; unsere Montage hat keine solche Tabelle, also ist die
 * Zuordnung eine Gestaltungsentscheidung — die RATEN sind es NICHT, die sind RE2-belegt.
 *
 * RE15_MONTAGE_STOCK=1 schaltet die ganze Schicht ab (byte-true 1-Frame-Hartschnitt).
 */
#include <stdlib.h>
#include <string.h>
#include "re15_montage_fx.h"

/* ---- RE2-Konstanten (s. Kopf) ---- */
#define FX_LEVEL_MAX      0x60   /* Klemme 0x00606060 @0x801c1bb8-c0 */
#define FX_RAMP_IN           2   /* +0x00020202/Frame @0x801c1b94-98 */
#define FX_RAMP_OUT          2   /* -0x00020202/Frame @0x801c1be4-ec (Typ 0) */
#define FX_RAMP_OUT_SLOW     1   /* -0x00010101/Frame @0x801c1d34-3c (Typ 2) */
/* Puls-Takte: der Zaehler laeuft N -> 0 und feuert BEI 0 (dann Reload N), die Periode ist
 * also N+1 Frames. Reload 10 @0x801c1a60 => 11 Frames; Reload 3 @0x801c1aa4 => 4 Frames. */
#define FX_PAN_PERIOD       11   /* Zaehler-Reload 10 @0x801c1a60 (N+1) */
#define FX_PAN_STEP          1   /* Impuls -1 @0x801c1a6c -> 1 px je Puls @0x801c1c44 */
#define FX_ZOOM_PERIOD       4   /* Zaehler-Reload 3 @0x801c1aa4 (N+1) */
#define FX_ZOOM_STEP         2   /* (impuls<<1) @0x801c1d90/@0x801c1dd4 */
/* RE2 pannt ein ABSICHTLICH UEBERGROSSES Bild: die Standbild-Vierergruppe ist 320x320
 * (4 Quads a 160x160, Y=10/170) und faehrt im 240-px-Fenster nach oben durch — 80 px
 * Ueberhang. Unsere BSS sind bildschirmfuellend 320x240, also bekommt der Pan-Modus
 * denselben Ueberhang als Grund-Vergroesserung, sonst liefe schwarzer Rand ins Bild. */
#define FX_PAN_OVERSCAN     80

/* Bewegungs-Modus je Montage-Bild (PORT-WAHL — s. Kopf). Vorbild: RE2 setzt die Modi
 * sparsam, meist eine Bewegung je Kapitel (Nutzer sah sie „bei den einen Bild").
 * Cut 0 = Schwarzbild (kein Effekt), Logos statisch, Motive mit Tiefe bekommen Zoom,
 * das Panorama das vertikale Wandern. */
static const uint8_t s_cut_mode[RE15_MONTAGE_CUTS] = {
    RE15_MFX_NONE,   /* 0 Schwarz (160 T Opening) */
    RE15_MFX_ZOOM_IN,/* 1 Zombie-Kopf im Profil */
    RE15_MFX_NONE,   /* 2 T-Virus-Mikroskopaufnahme */
    RE15_MFX_NONE,   /* 3 S.T.A.R.S.-Abzeichen (Logo) */
    RE15_MFX_ZOOM_IN,/* 4 S.T.A.R.S.-Team vor dem Villentor */
    RE15_MFX_PAN,    /* 5 Helikopter ueber der Explosion */
    RE15_MFX_NONE,   /* 6 S.T.A.R.S.-Gruppe vor dem Heli */
    RE15_MFX_ZOOM_IN,/* 7 Umbrella-Laborinneres */
    RE15_MFX_NONE,   /* 8 Umbrella-Logo */
};

static struct {
    int      active;        /* Montage laeuft (Raum-gegated vom Aufrufer) */
    int      cut;           /* aktuelles Bild */
    int      level_new;     /* Helligkeit des AKTUELLEN Bildes 0..0x60 */
    int      level_prev;    /* Helligkeit des VORIGEN Bildes 0..0x60 (Kreuzblende) */
    int      have_prev;     /* voriges Bild im Snapshot-Puffer gueltig */
    int      pan_ctr;       /* Schritt-Takt 10 (@0x801c2ee0) */
    int      zoom_ctr;      /* Schritt-Takt 3 (@0x801c2ee2) */
    int      pan_y;         /* aufsummierter Wander-Offset (px) */
    int      zoom_px;       /* aufsummierte Groessenaenderung (px, Breite = Hoehe) */
    uint8_t  mode;          /* Modus des aktuellen Bildes */
} s_fx;

static int s_stock = -1;

int re15_montage_fx_stock(void)
{
    if (s_stock < 0) {
        const char *e = getenv("RE15_MONTAGE_STOCK");
        s_stock = (e && e[0] == '1') ? 1 : 0;
    }
    return s_stock;
}
void re15_montage_fx_stock_set(int v) { s_stock = v; }

void re15_montage_fx_reset(void)
{
    memset(&s_fx, 0, sizeof s_fx);
    s_fx.cut = -1;
    s_fx.pan_ctr  = FX_PAN_PERIOD;
    s_fx.zoom_ctr = FX_ZOOM_PERIOD;
}

void re15_montage_fx_set_active(int on)
{
    if (!on) { if (s_fx.active) re15_montage_fx_reset(); return; }
    if (!s_fx.active) { re15_montage_fx_reset(); s_fx.active = 1; }
}

int re15_montage_fx_active(void)
{
    return s_fx.active && !re15_montage_fx_stock();
}

void re15_montage_fx_on_cut(int cut, int had_previous)
{
    if (!re15_montage_fx_active()) return;
    if (cut == s_fx.cut) return;
    s_fx.cut       = cut;
    /* Das bisherige Bild uebernimmt seinen Helligkeitsstand und blendet von dort aus;
     * das neue startet bei 0 und rampt hoch (RE2: zwei Elemente, eines auf Bit3, das
     * neue auf Bit2 — die Ueberlappung IST die Kreuzblende). */
    s_fx.level_prev = had_previous ? (s_fx.level_new > 0 ? s_fx.level_new : FX_LEVEL_MAX) : 0;
    s_fx.have_prev  = had_previous ? 1 : 0;
    s_fx.level_new  = 0;
    s_fx.pan_y      = 0;
    s_fx.zoom_px    = 0;
    s_fx.pan_ctr    = FX_PAN_PERIOD;
    s_fx.zoom_ctr   = FX_ZOOM_PERIOD;
    s_fx.mode       = (cut >= 0 && cut < RE15_MONTAGE_CUTS) ? s_cut_mode[cut] : RE15_MFX_NONE;
}

void re15_montage_fx_tick(void)
{
    if (!re15_montage_fx_active()) return;

    /* Helligkeits-Rampen (@0x801c1b94-c0c): neues Bild auf, altes ab. */
    if (s_fx.level_new < FX_LEVEL_MAX) {
        s_fx.level_new += FX_RAMP_IN;
        if (s_fx.level_new > FX_LEVEL_MAX) s_fx.level_new = FX_LEVEL_MAX;
    }
    if (s_fx.have_prev && s_fx.level_prev > 0) {
        s_fx.level_prev -= FX_RAMP_OUT;
        if (s_fx.level_prev <= 0) { s_fx.level_prev = 0; s_fx.have_prev = 0; }
    }

    /* Schritt-Takte (@0x801c1a0c-abc): je Periode genau EIN Schritt. */
    if (--s_fx.pan_ctr <= 0) {
        s_fx.pan_ctr = FX_PAN_PERIOD;
        /* RE2 Bit4 @0x801c1c44: Y += Puls (-1) — das Bild wandert nach OBEN, der
         * sichtbare Ausschnitt also nach unten durchs Motiv. */
        if (s_fx.mode == RE15_MFX_PAN && s_fx.pan_y > -FX_PAN_OVERSCAN)
            s_fx.pan_y -= FX_PAN_STEP;
    }
    if (--s_fx.zoom_ctr <= 0) {
        s_fx.zoom_ctr = FX_ZOOM_PERIOD;
        /* RE2 Bit5 @0x801c1d94: W/H WACHSEN (+2) = hineinfahren;
         *     Bit6 @0x801c1dd8: W/H SCHRUMPFEN (-2) = herausfahren. */
        if (s_fx.mode == RE15_MFX_ZOOM_IN)  s_fx.zoom_px += FX_ZOOM_STEP;
        if (s_fx.mode == RE15_MFX_ZOOM_OUT) s_fx.zoom_px -= FX_ZOOM_STEP;
    }
}

int re15_montage_fx_base_zoom(void)
{
    /* Der Pan-Modus braucht den RE2-Ueberhang als Sockel (s. FX_PAN_OVERSCAN). */
    return (re15_montage_fx_active() && s_fx.mode == RE15_MFX_PAN) ? FX_PAN_OVERSCAN : 0;
}

/* Alle Abfragen liefern NUR im aktiven Zustand Werte: ist die Schicht aus (Stock-Modus
 * oder anderer Raum), darf kein Rest des letzten Laufs mehr durchschlagen. */
int re15_montage_fx_level_new(void)  { return re15_montage_fx_active() ? s_fx.level_new : 0; }
int re15_montage_fx_level_prev(void) { return (re15_montage_fx_active() && s_fx.have_prev) ? s_fx.level_prev : 0; }
int re15_montage_fx_pan_y(void)      { return re15_montage_fx_active() ? s_fx.pan_y : 0; }
int re15_montage_fx_zoom_px(void)    { return re15_montage_fx_active() ? s_fx.zoom_px : 0; }
int re15_montage_fx_level_max(void)  { return FX_LEVEL_MAX; }
