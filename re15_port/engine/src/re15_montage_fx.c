/*
 * re15_montage_fx.c — die Praesentations-Schicht der ROOM1240-Standbild-Montage.
 * Alle Raten aus RE2s OPENING.BIN (analysis/preintro_re2/re2-bewegung.md,
 * re2-renderer.md); Bild-Reihenfolge, Standzeiten, Texte und Skip bleiben
 * byte-true RE1.5 (analysis/preintro_re2/re15-montage-inventar.md).
 */
#include "re15_montage_fx.h"
#include <stdlib.h>
#include <string.h>

/* --- RE2-Raten ------------------------------------------------------------- */
#define FX_LEVEL_MAX        0x60  /* Klemme @0x801c1bb8-bc (96,96,96)              */
#define FX_FADE_STEP        2     /* +/-0x00020202 @0x801c1b94 / @0x801c1be4       */
/* Puls-Takte: der Zaehler laeuft N -> 0 und feuert BEI 0 (dann Reload N), die
 * Periode ist also N+1. Reload 10 @0x801c1a60 => 11; Reload 3 @0x801c1aa4 => 4. */
#define FX_PAN_PERIOD       11
#define FX_PAN_STEP          1    /* Impuls -1 @0x801c1a6c -> 1 px @0x801c1c44     */
#define FX_ZOOM_PERIOD       4
#define FX_ZOOM_STEP         2    /* (impuls<<1) @0x801c1d90 / @0x801c1dd4         */
/* RE2 pannt ein ABSICHTLICH UEBERGROSSES Bild: die Standbild-Vierergruppe ist
 * 320x320 (4 Quads a 160x160) und faehrt im 240-px-Fenster durch — 80 px Ueberhang.
 * Unsere BSS sind bildschirmfuellend 320x240, also bekommt der Pan-Modus denselben
 * Ueberhang als Sockel, sonst liefe schwarzer Rand ins Bild. */
#define FX_PAN_OVERSCAN     80

/* --- Bewegungs-Modus je Montage-Bild ----------------------------------------
 * Was in RE2 tatsaechlich zu sehen ist (Nutzer-Vorgabe 2026-08-30, gegen die
 * Phasen-Timeline in re2-sequenzer.md §3.5 geprueft):
 *   1. Bild (Zombie)          = STANDBILD
 *   2. Bild (T-Virus)         = wandert vertikal            (RE2 {2..5}, 23 -> 27)
 *   3. Bild (S.T.A.R.S.-Logo) = zoomt HERAUS und laeuft ueber
 *                               das naechste Bild WEITER    (RE2 {10},   71 -> 75)
 *   4./5. Bild (Villa, Heli)  = STANDBILDER                 (RE2 {11..14}, 7 -> 11)
 *   letztes Bild (Umbrella)   = zoomt HINEIN, ebenfalls ueber
 *                               den Bildwechsel hinaus      (RE2 {19},   39 -> 43)
 * RE2 setzt in der ganzen Sequenz genau DREI Bewegungen; alles Uebrige blendet nur. */
/* Wartezeit, bis das Standbild UNTER dem zoomenden Logo erscheint.
 * RE2: das Logo {19} wird in Phase 4 bei t==580 gesetzt (@0x801c0a74-94), das
 * Standbild {20..23} erst in Phase 5 bei t==96 (@0x801c0ab4-c8) — dazwischen
 * liegen die 96 Frames des Phasenanfangs. */
#define FX_HOLD_DELAY  96

/* ⛔ DAS VORIGE STANDBILD BLEIBT STEHEN, das Logo zoomt DARUEBER.
 *
 * Nutzer 2026-08-30: "Das Standbild vom Labor blendet aus, dann blendet das
 * heranzoomende Umbrella Logo ein. Das ist im Original Resident Evil 2 nicht so.
 * Da hast du das Standbild des Labors und DARAUF zoomed das Umbrella logo ran."
 *
 * Am Code bestaetigt (Phase 5 @0x801c0aa0):
 *   t== 96 : Maske 0x00f00000 = {20..23} (Standbild), Zustand 7 = EINBLENDEN,
 *            a1 = 0 (@0x801c0ab4-c8) — hide_others ist NULL, das Standbild
 *            verdraengt also NICHTS.
 *   t==261 : Maske 0x00080000 = {19} (Logo), Zustand 43 = ausblenden + Zoom auf,
 *            a1 = 0 (@0x801c0b68-7c).
 * ⇒ Zwischen beiden liegen 165 Frames, in denen Standbild UND Logo gleichzeitig
 * voll sichtbar sind; das Logo zoomt in dieser Zeit ueber dem stehenden Bild.
 *
 * (Der harte Clear `0x801c1f64(0, 1)` @0x801c09e4-f4 existiert zwar, sitzt aber
 * am KAPITEL-Anfang in Phase 4 t==94 — nicht zwischen Standbild und Logo. Ihn
 * hierher zu ziehen war der Fehler der Vorversion.)
 *
 * Umsetzung: fuer diese Bilder blendet die vorige Ebene NICHT aus, sondern haelt
 * ihre Helligkeit, waehrend die neue darueber einblendet. */
static const uint8_t s_cut_hold_prev[RE15_MONTAGE_CUTS] = {
    0, 0, 0, 0, 0, 0, 0, 0,
    1,   /* 8 Umbrella-Logo — zoomt UEBER dem stehenden Labor-Standbild */
};

static const uint8_t s_cut_mode[RE15_MONTAGE_CUTS] = {
    RE15_MFX_NONE,     /* 0 Schwarz (160 T Opening)                              */
    RE15_MFX_NONE,     /* 1 Zombie-Kopf              — STANDBILD                 */
    RE15_MFX_PAN,      /* 2 T-Virus-Mikroskopbild    — wandert von oben nach unten */
    RE15_MFX_ZOOM_OUT, /* 3 S.T.A.R.S.-Abzeichen     — zoomt heraus, laeuft weiter */
    RE15_MFX_NONE,     /* 4 S.T.A.R.S. vor dem Villentor — STANDBILD             */
    RE15_MFX_NONE,     /* 5 Helikopter ueber der Explosion — STANDBILD           */
    RE15_MFX_NONE,     /* 6 Gruppe vor dem Helikopter                            */
    RE15_MFX_NONE,     /* 7 Umbrella-Laborinneres                                */
    RE15_MFX_ZOOM_IN,  /* 8 Umbrella-Logo            — zoomt hinein, laeuft weiter */
};

/* --- Eine Bild-EBENE. Jede traegt ihre EIGENE Bewegung und fuehrt sie ueber den
 * Bildwechsel hinweg fort — RE2s Bewegungs-Bit bleibt beim Umschalten von Ein- auf
 * Ausblenden gesetzt (71 -> 75, 39 -> 43, 23 -> 27). ------------------------- */
typedef struct {
    uint8_t mode;      /* RE15_MFX_*                              */
    int16_t level;     /* 0..FX_LEVEL_MAX                         */
    int8_t  dir;       /* +1 einblenden, -1 ausblenden, 0 haelt   */
    int16_t pan_y;
    int16_t zoom_px;
    uint8_t used;
    int16_t delay;     /* Frames, bis diese Ebene einblendet (0 = sofort)      */
} fx_layer_t;

static struct {
    int        active;
    int        stock;              /* -1 = Umgebung noch nicht gelesen */
    fx_layer_t cur, prev;
    int        pan_ctr, zoom_ctr;  /* GLOBALE Puls-Zaehler (RE2: ein Paar im
                                    * Renderer-Kopf @0x801c1a0c-abc, nicht je Element) */
} s_fx;

int re15_montage_fx_level_max(void) { return FX_LEVEL_MAX; }

int re15_montage_fx_stock(void)
{
    if (s_fx.stock < 0) {
        const char *v = getenv("RE15_MONTAGE_STOCK");
        s_fx.stock = (v && *v && *v != '0') ? 1 : 0;
    }
    return s_fx.stock;
}
void re15_montage_fx_stock_set(int v) { s_fx.stock = v; }

void re15_montage_fx_reset(void)
{
    int keep = s_fx.stock;
    memset(&s_fx, 0, sizeof s_fx);
    s_fx.stock    = keep;
    s_fx.pan_ctr  = FX_PAN_PERIOD;
    s_fx.zoom_ctr = FX_ZOOM_PERIOD;
}

void re15_montage_fx_set_active(int on)
{
    on = on ? 1 : 0;
    if (on == s_fx.active) return;
    re15_montage_fx_reset();
    s_fx.active = on;
}

int re15_montage_fx_active(void)
{
    return (s_fx.active && !re15_montage_fx_stock()) ? 1 : 0;
}

void re15_montage_fx_on_cut(int cut, int had_previous)
{
    if (!re15_montage_fx_active()) return;
    /* Die bisherige Ebene wandert nach "prev" und blendet AUS — ihre Bewegung
     * (Modus, Zoom, Pan) laeuft dabei UNVERAENDERT WEITER. Genau das ist RE2s
     * 71 -> 75 / 39 -> 43: nur das Blend-Bit kippt, das Bewegungs-Bit bleibt.
     * Ohne das sprang das Logo beim Bildwechsel auf seine Ausgangsgroesse
     * zurueck (Nutzer 2026-08-30: "Bei dir wird das Logo ploetzlich wieder gross"). */
    if (had_previous && s_fx.cur.used) {
        int holds = (cut >= 0 && cut < RE15_MONTAGE_CUTS) ? s_cut_hold_prev[cut] : 0;
        s_fx.prev = s_fx.cur;
        if (holds) {
            /* Nutzer 2026-08-31: "Es zoomed am Anfang in auf schwarzen hintergrund,
             * dann kommt das letzte Standbild im Labor und da zoomed es dann eben
             * weiter in." — genau RE2s Reihenfolge: das Logo erscheint ALLEIN auf
             * leerem Grund (Phase 4 t==580) und erst 96 Frames spaeter legt sich das
             * Standbild darunter (Phase 5 t==96, Zustand 7, hide_others = 0).
             * Also: die vorige Ebene wird ausgeblendet und kommt VERZOEGERT zurueck. */
            s_fx.prev.level = 0;
            s_fx.prev.dir   = 0;
            s_fx.prev.delay = FX_HOLD_DELAY;
        } else {
            s_fx.prev.dir = -1;    /* normale Kreuzblende */
        }
    } else {
        memset(&s_fx.prev, 0, sizeof s_fx.prev);
    }
    memset(&s_fx.cur, 0, sizeof s_fx.cur);
    s_fx.cur.used = 1;
    s_fx.cur.dir  = 1;
    s_fx.cur.mode = (cut >= 0 && cut < RE15_MONTAGE_CUTS)
                  ? s_cut_mode[cut] : (uint8_t)RE15_MFX_NONE;
}

static void layer_fade(fx_layer_t *l)
{
    if (!l->used) return;
    if (l->delay > 0) {              /* wartet noch auf seinen Auftritt */
        if (--l->delay == 0) l->dir = 1;
        return;
    }
    if (l->dir == 0) return;
    l->level = (int16_t)(l->level + l->dir * FX_FADE_STEP);
    if (l->level >= FX_LEVEL_MAX) { l->level = FX_LEVEL_MAX; l->dir = 0; }
    if (l->level <= 0) {
        l->level = 0;
        if (l->dir < 0) l->used = 0;    /* ausgeblendet = Ebene frei */
        l->dir = 0;
    }
}

static void layer_move(fx_layer_t *l, int pan_pulse, int zoom_pulse)
{
    if (!l->used) return;
    /* Bild wandert nach OBEN (RE2: Y += Puls, Puls = -1 @0x801c1c44) — der
     * sichtbare Ausschnitt faehrt damit von oben nach unten durchs Motiv. */
    if (pan_pulse && l->mode == RE15_MFX_PAN && l->pan_y > -FX_PAN_OVERSCAN)
        l->pan_y = (int16_t)(l->pan_y - FX_PAN_STEP);
    if (zoom_pulse) {
        if (l->mode == RE15_MFX_ZOOM_IN)  l->zoom_px = (int16_t)(l->zoom_px + FX_ZOOM_STEP);
        if (l->mode == RE15_MFX_ZOOM_OUT) l->zoom_px = (int16_t)(l->zoom_px - FX_ZOOM_STEP);
    }
}

void re15_montage_fx_tick(void)
{
    int pan_pulse = 0, zoom_pulse = 0;
    if (!re15_montage_fx_active()) return;

    layer_fade(&s_fx.cur);
    layer_fade(&s_fx.prev);

    if (--s_fx.pan_ctr  <= 0) { s_fx.pan_ctr  = FX_PAN_PERIOD;  pan_pulse  = 1; }
    if (--s_fx.zoom_ctr <= 0) { s_fx.zoom_ctr = FX_ZOOM_PERIOD; zoom_pulse = 1; }

    layer_move(&s_fx.cur,  pan_pulse, zoom_pulse);
    layer_move(&s_fx.prev, pan_pulse, zoom_pulse);   /* laeuft WEITER — der Kern-Fix */
}

/* Im Stock-Modus (byte-true Hartschnitt) meldet die Schicht durchgaengig NICHTS —
 * sonst koennte ein Restwert aus einem frueheren Lauf in den Zeichner lecken. */
int re15_montage_fx_level_new(void)
{ return re15_montage_fx_active() && s_fx.cur.used  ? s_fx.cur.level  : 0; }
int re15_montage_fx_level_prev(void)
{ return re15_montage_fx_active() && s_fx.prev.used ? s_fx.prev.level : 0; }
int re15_montage_fx_pan_y(void)       { return re15_montage_fx_active() ? s_fx.cur.pan_y   : 0; }
int re15_montage_fx_zoom_px(void)     { return re15_montage_fx_active() ? s_fx.cur.zoom_px : 0; }
int re15_montage_fx_prev_pan_y(void)  { return re15_montage_fx_active() ? s_fx.prev.pan_y  : 0; }
int re15_montage_fx_prev_zoom_px(void){ return re15_montage_fx_active() ? s_fx.prev.zoom_px: 0; }
int re15_montage_fx_mode(void)        { return s_fx.cur.mode; }
int re15_montage_fx_prev_mode(void)   { return s_fx.prev.used ? s_fx.prev.mode
                                                             : RE15_MFX_NONE; }

static int base_zoom_of(const fx_layer_t *l)
{
    return (l->used && l->mode == RE15_MFX_PAN) ? FX_PAN_OVERSCAN : 0;
}
int re15_montage_fx_base_zoom(void)      { return base_zoom_of(&s_fx.cur); }
int re15_montage_fx_prev_base_zoom(void) { return base_zoom_of(&s_fx.prev); }
