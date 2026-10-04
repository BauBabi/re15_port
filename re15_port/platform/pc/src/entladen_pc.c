/*
 * entladen_pc.c — Runde 35 Spur I: ALLE Raum-Assets des PC-Ports entladen + Zensus.
 *
 * Herleitung, Original-Adressen und Messungen: include/re15_entladen.h und
 * analysis/befunde_runde35/I_entladen.md. Kurz: im Original liegt alles Raum-Eigene in EINER
 * Arena, die der Raumlader FUN_800396fc bei JEDEM Aufruf auf die Basis zuruecksetzt
 * (@0x80039738/40/48) — und er laeuft beim Tuerwechsel (@0x8001d988) UND beim Spielstart
 * (@0x8001d5ac, Spielmodul-Init; dort zusaetzlich @0x8001d590-a0). Gezeichnet wird eine
 * Raum-Maske nur im Spielmodul (@0x8001ce54). Der PC haelt dieselben Daten in Caches mit
 * Prozess-Lebensdauer; re15_entladen_ereignis() leert sie an denselben Grenzen.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "re15_entladen.h"
#include "re15_enemy.h"
#include "re15_esp.h"
#include "re2_fx.h"
#include "re15_room.h"
#include "re15_light.h"
#include "re15_msg.h"
#include "re15_gameflow.h"
#include "asset_root_pc.h"

/* render_pc.c */
extern unsigned re15_render_pc_pri_gen(void);
extern int  re15_render_pc_pri_belegt(unsigned *rects_gen, int *atlas, unsigned *atlas_gen);
extern int  re15_render_pc_tim_slot_raum(int slot);
extern int  re15_render_pc_tim_slot_belegt(int slot, unsigned *gen);
extern int  re15_render_pc_tim_slot_anzahl(void);
extern void re15_render_pc_entladen_raum(void);
/* bg_pc.c */
extern int  re15_pri_sld_belegt(unsigned *gen);
extern void re15_pri_sld_entladen(void);
extern int  re15_bg_belegt(unsigned *gen);
extern void re15_bg_invalidate(void);
extern void re15_bg_prev_invalidate(void);
/* room_pc.c */
extern int  re15_room_pc_belegt(unsigned *gen);
extern void re15_room_pc_entladen(void);
/* audio_pc.c */
extern int  re15_audio_raum_belegt(void);

const char *const re15_entladen_fachname[RE15_FACH_ANZAHL] = {
    "pri_masken", "pri_atlas", "sld", "msk", "tim", "gegner",
    "esp_bank", "esp_fx", "esp_pool", "re2fx", "rdt", "ton", "bg", "stimme", "figur",
    "rbj", "bg_prev", "re2ton"
};

/* ---- Raum-Animationsbank (Nachbesserung 2, Abnahme 1 M1) -------------------------------------
 * Vorher hielt main.c den Dateipuffer selbst (`static uint8_t *s_room_rbj`, frei nur, wenn der
 * NAECHSTE Raum wieder einen Block hatte; Boot-Puffer `rbj_buf` nie). Original: Block = RDT+0x5C
 * (`lw a2,92(v0)` @0x8001b404, v0 = RDT-Zeiger 0x800ac778 @0x8001b3fc), RDT ab der Arena-Basis
 * (`jal 0x80013b60` @0x800397e8) -> mit dem Arena-Reset @0x80039738 weg, bei JEDEM Raumladen. */
static uint8_t *s_rbj      = NULL;
static int      s_rbj_size = 0;
static unsigned s_rbj_raum = 0;
static unsigned s_rbj_gen  = 0;

void re15_entladen_rbj_halten(uint8_t *buf, int size, unsigned raum)
{
    if (buf != s_rbj) free(s_rbj);
    s_rbj      = buf;
    s_rbj_size = buf ? size : 0;
    s_rbj_raum = buf ? raum : 0;
    s_rbj_gen  = g_re15_entladen_gen;
}
int re15_entladen_rbj_belegt(unsigned *gen)
{
    if (gen) *gen = s_rbj_gen;
    return s_rbj != NULL;
}

/* ---- Nachgezeichnete Masken (vorher main.c-Cache, Schluessel nur der Raum) ------------------ */
static unsigned char *s_msk      = NULL;
static int            s_msk_size = 0;
static unsigned       s_msk_room = 0xFFFFu;
static unsigned       s_msk_gen  = 0;

const unsigned char *re15_entladen_msk(unsigned room_id, int *out_size)
{
    if (s_msk_room != room_id) {
        char rel[64];
        free(s_msk); s_msk = NULL; s_msk_size = 0;
        s_msk_room = room_id;
        snprintf(rel, sizeof rel, "MASKS/ROOM%04X.MSK", room_id);
        s_msk = re15_pc_read_cd(rel, &s_msk_size);
        if (!s_msk) s_msk_size = 0;
        s_msk_gen = g_re15_entladen_gen;
    }
    if (out_size) *out_size = s_msk_size;
    return s_msk;
}

/* ---- Raum-Effektbank: Generation beim Binden (pc_load_room_esp) ------------------------------ */
static unsigned s_esp_bank_gen = 0;
void re15_entladen_esp_bank_merken(void) { s_esp_bank_gen = g_re15_entladen_gen; }

/* ---- Zensus -------------------------------------------------------------------------------- */
static int re2fx_lebend(void)
{
    int n = 0;
    for (int i = 0; i < RE2FX_PLAETZE; i++) {
        const uint8_t *p = re2fx_platz(i);
        if (p && (p[0x18] | p[0x19])) n++;          /* +0x18 u16 STATUS (re2_fx.h) */
    }
    return n;
}

void re15_entladen_zensus(re15_entladen_zensus_t *z)
{
    const unsigned g = g_re15_entladen_gen;
    memset(z, 0, sizeof *z);

    unsigned rg = 0, ag = 0; int atlas = 0;
    int nr = re15_render_pc_pri_belegt(&rg, &atlas, &ag);
    z->belegt[RE15_FACH_PRI_MASKEN] = nr;
    z->fremd [RE15_FACH_PRI_MASKEN] = (nr > 0 && rg != g) ? nr : 0;
    z->belegt[RE15_FACH_PRI_ATLAS]  = atlas;
    z->fremd [RE15_FACH_PRI_ATLAS]  = (atlas && ag != g) ? 1 : 0;

    unsigned sg = 0;
    int sld = re15_pri_sld_belegt(&sg);
    z->belegt[RE15_FACH_SLD] = sld ? 1 : 0;
    z->fremd [RE15_FACH_SLD] = (sld && sg != g) ? 1 : 0;

    z->belegt[RE15_FACH_MSK] = s_msk ? 1 : 0;
    z->fremd [RE15_FACH_MSK] = (s_msk && s_msk_gen != g) ? 1 : 0;

    for (int s = 0; s < re15_render_pc_tim_slot_anzahl(); s++) {
        unsigned tg = 0;
        if (!re15_render_pc_tim_slot_raum(s) || !re15_render_pc_tim_slot_belegt(s, &tg)) continue;
        z->belegt[RE15_FACH_TIM]++;
        if (tg != g) z->fremd[RE15_FACH_TIM]++;
    }

    for (int b = 0; b < RE15_ENEMY_MAX; b++) {
        if (!g_enemy[b].ok) continue;
        z->belegt[RE15_FACH_GEGNER]++;
        if (re15_entladen_gegner_gen(b) != g) z->fremd[RE15_FACH_GEGNER]++;
    }

    int bank = re15_esp_room_bank() != NULL;
    z->belegt[RE15_FACH_ESP_BANK] = bank;
    z->fremd [RE15_FACH_ESP_BANK] = (bank && s_esp_bank_gen != g) ? 1 : 0;

    /* Laufende Instanzen tragen keine Generation — sie sind ausschliesslich am EREIGNIS selbst
     * zu werten (dort ist jede noch lebende Instanz eine des Raums davor). */
    z->belegt[RE15_FACH_ESP_FX]   = re15_esp_fx_count();
    z->belegt[RE15_FACH_ESP_POOL] = re15_esp_pool_count();
    z->belegt[RE15_FACH_RE2FX]    = re2fx_lebend();

    unsigned dg = 0;
    int rdt = re15_room_pc_belegt(&dg);
    z->belegt[RE15_FACH_RDT] = rdt ? 1 : 0;
    z->fremd [RE15_FACH_RDT] = (rdt && dg != g) ? 1 : 0;

    z->belegt[RE15_FACH_TON] = re15_audio_raum_belegt();   /* ohne Generation: nur am Ereignis */

    unsigned bgg = 0;
    int bg = re15_bg_belegt(&bgg);
    z->belegt[RE15_FACH_BG] = bg ? 1 : 0;
    z->fremd [RE15_FACH_BG] = (bg && bgg != g) ? 1 : 0;

    /* Nachbesserung 1: Raum-Stimmen (M1) und Elliot (M2), beide mit Generation. */
    unsigned vg = 0;
    int st = re15_audio_stimmen_belegt(&vg, NULL);
    z->belegt[RE15_FACH_STIMME] = st;
    z->fremd [RE15_FACH_STIMME] = (st > 0 && vg != g) ? st : 0;
    unsigned eg = 0;
    int fig = re15_elliot_pc_belegt(&eg);
    z->belegt[RE15_FACH_FIGUR] = fig ? 1 : 0;
    z->fremd [RE15_FACH_FIGUR] = (fig && eg != g) ? 1 : 0;

    /* Nachbesserung 2: Raum-Animationsbank (eigener Dateipuffer + Leihe Spur K), Montage-
     * Schnappschuss, RE2-Raumbank-Ergaenzungen (ohne Generation: nur am Ereignis, wie "ton"). */
    unsigned rg1 = 0, rg2 = 0;
    int rb1 = re15_entladen_rbj_belegt(&rg1), rb2 = re15_cut10f0_pc_rbj_belegt(&rg2);
    z->belegt[RE15_FACH_RBJ] = rb1 + rb2;
    z->fremd [RE15_FACH_RBJ] = (rb1 && rg1 != g) + (rb2 && rg2 != g);
    unsigned pg = 0;
    int bp = re15_bg_prev_belegt(&pg);
    z->belegt[RE15_FACH_BG_PREV] = bp ? 1 : 0;
    z->fremd [RE15_FACH_BG_PREV] = (bp && pg != g) ? 1 : 0;
    z->belegt[RE15_FACH_RE2TON] = re15_audio_re2_raumbaenke_belegt();

    for (int f = 0; f < RE15_FACH_ANZAHL; f++) {
        z->belegt_summe += z->belegt[f];
        z->fremd_summe  += z->fremd[f];
    }
}

/* ---- Messschiene ---------------------------------------------------------------------------- */
static FILE *log_oeffnen(void)
{
    static int s_init = 0; static const char *s_pfad = NULL;
    if (!s_init) { s_init = 1; s_pfad = getenv("RE15_ENTLADEN_LOG"); if (s_pfad && !*s_pfad) s_pfad = NULL; }
    return s_pfad ? fopen(s_pfad, "ab") : NULL;
}

static void zensus_schreiben(FILE *f, const re15_entladen_zensus_t *z, int am_ereignis)
{
    fprintf(f, " | belegt");
    for (int i = 0; i < RE15_FACH_ANZAHL; i++) fprintf(f, " %s=%d", re15_entladen_fachname[i], z->belegt[i]);
    fprintf(f, " | fremd");
    for (int i = 0; i < RE15_FACH_ANZAHL; i++) {
        /* am Ereignis ist JEDE noch lebende Instanz fremd (s.o.) */
        int fr = am_ereignis && (i == RE15_FACH_ESP_FX || i == RE15_FACH_ESP_POOL || i == RE15_FACH_RE2FX ||
                                 i == RE15_FACH_TON || i == RE15_FACH_RE2TON)
               ? z->belegt[i] : z->fremd[i];
        fprintf(f, " %s=%d", re15_entladen_fachname[i], fr);
    }
    if (z->belegt[RE15_FACH_TIM] > 0) {   /* welche Raum-Slots (f = fremd) */
        fprintf(f, " | tim_slots");
        for (int s = 0; s < re15_render_pc_tim_slot_anzahl(); s++) {
            unsigned tg = 0;
            if (re15_render_pc_tim_slot_raum(s) && re15_render_pc_tim_slot_belegt(s, &tg))
                fprintf(f, " %d%s", s, (am_ereignis || tg != g_re15_entladen_gen) ? "f" : "");
        }
    }
    if (s_rbj)   /* Nachbesserung 2: welcher Raum, wie gross (Abnahme 1: ROOM1170.RBJ, 55060 B) */
        fprintf(f, " | rbj_datei raum=%04X bytes=%d gen=%u", s_rbj_raum, s_rbj_size, s_rbj_gen);
}

/* Laufende Summen seit dem letzten Ereignis (fuer die SUMME-Zeile, die der Pin liest). */
static unsigned s_bilder = 0, s_bilder_fremd = 0, s_bilder_masken_fremd = 0, s_masken_fremd_max = 0;
static unsigned s_bilder_masken = 0;
static int      s_fremd_max = 0;
static char     s_letzter_anlass[24] = "prozess";

static void summe_schreiben(FILE *f)
{
    fprintf(f, "SUMME seit=%s bilder=%u bilder_mit_masken=%u bilder_fremd_belegt=%u fremd_belegt_max=%d "
               "bilder_fremde_masken_gezeichnet=%u fremde_masken_max=%u\n",
            s_letzter_anlass, s_bilder, s_bilder_masken, s_bilder_fremd, s_fremd_max,
            s_bilder_masken_fremd, s_masken_fremd_max);
    s_bilder = s_bilder_fremd = s_bilder_masken_fremd = s_masken_fremd_max = s_bilder_masken = 0;
    s_fremd_max = 0;
}

void re15_entladen_bild(int masken_gezeichnet, unsigned masken_gen)
{
    static int s_an = -1;
    if (s_an < 0) { const char *p = getenv("RE15_ENTLADEN_LOG"); s_an = (p && *p) ? 1 : 0; }
    if (!s_an) return;

    re15_entladen_zensus_t z;
    re15_entladen_zensus(&z);
    int masken_fremd = (masken_gezeichnet > 0 && masken_gen != g_re15_entladen_gen) ? masken_gezeichnet : 0;
    s_bilder++;
    if (masken_gezeichnet > 0) s_bilder_masken++;
    if (z.fremd_summe > 0) s_bilder_fremd++;
    if (z.fremd_summe > s_fremd_max) s_fremd_max = z.fremd_summe;
    if (masken_fremd > 0) {
        s_bilder_masken_fremd++;
        if ((unsigned)masken_fremd > s_masken_fremd_max) s_masken_fremd_max = (unsigned)masken_fremd;
    }
    /* Bildbeleg (RE15_ENTLADEN_SHOT=<praefix>, RE15_ENTLADEN_SHOT_BILD=<n>[,<n>...], Vorgabe 5):
     * die genannten Bilder JE Ereignis-Strecke werden komplett komponiert zurueckgelesen
     * (render_pc: vor SDL_RenderPresent, wie FRAMEDUMP) — auch in Titel/Auswahl, wo der
     * FRAMEDUMP-Haken der Spielschleife nicht hinkommt. Reiner Messhaken. */
    { static const char *s_shot = NULL, *s_shot_bild = "5"; static int s_shot_init = 0;
      static unsigned s_shot_gen = 0, s_shot_n = 0;
      if (!s_shot_init) { s_shot_init = 1; s_shot = getenv("RE15_ENTLADEN_SHOT"); if (s_shot && !*s_shot) s_shot = NULL;
                          const char *b = getenv("RE15_ENTLADEN_SHOT_BILD"); if (b && *b) s_shot_bild = b; }
      if (s_shot) {
          if (s_shot_gen != g_re15_entladen_gen) { s_shot_gen = g_re15_entladen_gen; s_shot_n = 0; }
          ++s_shot_n;
          for (const char *q = s_shot_bild; q && *q; ) {
              if ((unsigned)atoi(q) == s_shot_n) {
                  extern void re15_render_pc_request_readback(const char *path);
                  char p[300];
                  snprintf(p, sizeof p, "%s_gen%u_b%03u_raum%04X_modus%d.ppm", s_shot, g_re15_entladen_gen,
                           s_shot_n, g_current_room_id, (int)re15_gameflow_mode());
                  re15_render_pc_request_readback(p);
                  break;
              }
              q = strchr(q, ','); if (q) q++;
          }
      } }
    /* Zeile nur bei Befund und gedrosselt (erstes Befund-Bild je Strecke + jedes 30.). */
    static unsigned s_zeilen_strecke = 0xFFFFFFFFu, s_zeile_gen = 0;
    if (s_zeile_gen != g_re15_entladen_gen) { s_zeile_gen = g_re15_entladen_gen; s_zeilen_strecke = 0; }
    if ((z.fremd_summe > 0 || masken_fremd > 0) &&
        (s_zeilen_strecke++ == 0 || (s_bilder % 30u) == 0u)) {
        FILE *f = log_oeffnen();
        if (f) {
            fprintf(f, "BILD %u gen=%u raum=%04X modus=%d masken_gezeichnet=%d (gen %u) fremd_gezeichnet=%d",
                    s_bilder, g_re15_entladen_gen, g_current_room_id, (int)re15_gameflow_mode(),
                    masken_gezeichnet, masken_gen, masken_fremd);
            zensus_schreiben(f, &z, 0);
            fputc('\n', f);
            fclose(f);
        }
    }
}

/* ---- Entladen ------------------------------------------------------------------------------- */
/* Das Gegenstueck zum Arena-Reset: ALLES, was einem Raum gehoert, faellt an der Grenze — auch das,
 * was der neue Raum gleich wieder fuellt (das Fuellen ist dann eindeutig "neu", Generation g).
 * Jeder Schritt ist idempotent; der Raumwechsel-Teardown (room_pc.c re15_room_reset_render_pc,
 * room_common.c re15_room_apply_pending) laeuft danach unveraendert weiter. */
static void alles_entladen(const char *anlass)
{
    /* (1) Masken, Atlas, Raum-TIM-Slots (Props 4..9/26..35/45, Gegner 10..18 + Gore 46..49,
     *     Tuersequenz 24/25, Raum-ESP 36..43). Original: Masken-Tabelle in der Arena
     *     (`jal 0x80039270` @0x800399cc), Objekt-Slots per Flagwort genullt (FUN_8003ea7c
     *     @0x8003eab0-acc), Gegner-Entities per Flagwort (FUN_8001a4c0 @0x8001a4e8). */
    re15_render_pc_entladen_raum();
    /* (2) SLD-Auszug + nachgezeichnete Masken (beides Bytes des Raums). */
    re15_pri_sld_entladen();
    free(s_msk); s_msk = NULL; s_msk_size = 0; s_msk_room = 0xFFFFu;
    /* (3) Gegnerbanken (Modelle/Animationen, PC-Puffer frei). */
    re15_enemy_reset();
    /* (4) Effekte: 96 Plaetze nullen + Id-Maps -1 (FUN_80019354 @0x80019378/@0x80019388-e4,
     *     einziger Aufrufer @0x8003996c im Raumlader), Raum-Bank weg, Row-Pool, RE2-FX-Pool. */
    re15_esp_fx_reset();
    re15_esp_set_room_bank(NULL);
    re15_esp_pool_reset();
    re2fx_reset();
    /* (5) Lichtset + Nachrichtentabelle des Raums (beide aus der RDT reloziert, @0x800397fc-834). */
    g_re15_room_lights_ok = 0;
    re15_msg_clear_room_block();
    /* (6) Raum-Tonbaenke: im Original schliesst der Raumlader die Vorgaengerbank (SsVabClose in
     *     FUN_80043eac/FUN_80043fb0, @0x80039974/@0x8003997c). */
    { extern void re15_audio_raum_entladen(void); re15_audio_raum_entladen(); }
    /* (7) RDT-Bytes des Raums davor. Beim Raumwechsel liegt die NEUE RDT zu diesem Zeitpunkt nur
     *     geparst in re15_room_load bereit und wird direkt danach installiert (== Laden ab der
     *     Arena-Basis @0x800397e8 NACH dem Reset @0x80039738); an Spielstart/-ende folgt der
     *     Boot-Block bzw. der Titel (Spielmodul-Init @0x8001d590-a0). */
    (void)anlass;
    re15_room_pc_entladen();
    /* (8) Hintergrundbild des Cuts. Original: der Raumlader schaltet zuerst auf Modus-2-Schwarz
     *     (`jal 0x80021634` a0=2 @0x8001d620-28 Boot / @0x8001d830-34 Tuer, Spielmodul-Init
     *     @0x8001d248-50) und gibt das neue Bild erst nach dem Laden frei (@0x8001dadc-ec). Der
     *     Port laedt das Eintrittsbild direkt danach (room_common.c Schritt 9 / Boot-Preload);
     *     schlaegt das fehl, bleibt es schwarz statt beim Bild des Raums davor.
     *     Nachbesserung 2 (Abnahme 1 H1): auch der Montage-Schnappschuss (s_bg_prev) faellt an
     *     JEDER Grenze. Die fruehere Ausnahme "raum" (laufende Ueberblendung) traf nicht zu: die
     *     Montage ist nur in ROOM1240 aktiv (main.c re15_montage_fx_set_active(Raum == 0x1240)),
     *     der Schnappschuss wird nur dort beim Cut-Wechsel neu genommen (re15_bg_snapshot_prev)
     *     und nur vom Montage-Blit gelesen; hinter der Tuer liest ihn niemand mehr. */
    re15_bg_invalidate();
    re15_bg_prev_invalidate();
    /* (9) Nachbesserung 1, M1: dekodierte Raum-Stimmen + Loesen des Stimm-Stroms. RE2-Raumlader
     *     FUN_80049e48 liest die neue RDT `jal 0x80012fb8` @0x8004a1c4 -> Pause @0x800130d4,
     *     Setmode 0xA0 (XA-ADPCM-Bit 6 = 0) @0x800130f0, ReadN @0x80013140: keine Stimme
     *     erreicht danach die SPU; dekodiert im RAM lag in RE2 nie etwas (CD-XA -> SPU). */
    re15_audio_stimmen_entladen();
    /* (10) Nachbesserung 1, M2: Elliot (Typ 0x47) — im Original ein Sce_em_set-Modell in der
     *     Arena (`jal 0x80022300` @0x80042328 mit dem Kopf 0x800ac77c @0x800422c4), also mit dem
     *     Arena-Reset @0x80039738 weg. Neu geladen beim naechsten Spawn eines 0x47-Aktors. */
    re15_elliot_pc_entladen();
    /* (11) Nachbesserung 2, M1: Raum-Animationsbank. Der eigene Dateipuffer (RBJ/ROOM%04X.RBJ,
     *     Tuer- und Boot-Weg in main.c) und die Leihe der Spur K (ganze ROOM11B0-RDT fuer
     *     ROOM10F0) fallen. Original: Block = RDT+0x5C (@0x8001b404), die RDT liegt in der Arena
     *     (@0x800397e8) -> mit dem Reset @0x80039738 weg; der Binder `jal 0x8001b3f8` @0x80039a08
     *     liest bei JEDEM Raumladen den Block der NEUEN RDT. Leons/Elliots Overlay-Zeiger stellt
     *     main.c direkt danach im selben Raumaufbau neu (Overlay aus der neuen Bank bzw. PL00-Basis)
     *     — dasselbe Fenster wie beim RDT-Alias, dessen Bytes Schritt (7) schon freigibt. */
    re15_entladen_rbj_halten(NULL, 0, 0);
    re15_cut10f0_pc_rbj_freigeben();
    /* (12) Nachbesserung 2 (Abnahme 1 H2/H3): RE2-Raumbank-Ergaenzungen. RE2-Raumlader
     *     FUN_80049e48 `jal 0x8005a09c` @0x8004a33c: dort schliesst FUN_8005a09c die ENEMSE-Bank
     *     des Raums davor (Handle 0x800d4c4b != -1 -> `jal 0x80084ec0` @0x8005a108, `sb -1`
     *     @0x8005a114) und laedt die per FUN_80052b38 (@0x80053610 im Raum-Setup, `jal 0x80053528`
     *     @0x8004a334) neu bestimmte. ELEVSE/HINTSE/TUERSE/PANEL2130 sind in RE2 Saetze der
     *     RAUMBANK (Bank 2 = SND0 der RDT, re15_audio.h). Die Tuersequenz-Baenke (TORSE + je
     *     Archiv) bleiben: Tuerbank @0x3DC50 ausserhalb des Key-Off-Bereichs (Runde 31, O2). */
    re15_audio_re2_raumbaenke_entladen();
}

void re15_entladen_ereignis(const char *anlass)
{
    FILE *f = log_oeffnen();
    if (f) {   /* Bilanz der Strecke, die jetzt endet — VOR dem Generationswechsel */
        re15_entladen_zensus_t z0;
        re15_entladen_zensus(&z0);
        fprintf(f, "VORHER %s gen=%u raum=%04X", anlass, g_re15_entladen_gen, g_current_room_id);
        zensus_schreiben(f, &z0, 0);
        { int laeuft = 0; (void)re15_audio_stimmen_belegt(NULL, &laeuft);   /* Nachbesserung 1 */
          fprintf(f, " | stimme_laeuft=%d", laeuft); }
        fputc('\n', f);
        summe_schreiben(f);
    }
    re15_entladen_gen_weiter();
    alles_entladen(anlass);
    snprintf(s_letzter_anlass, sizeof s_letzter_anlass, "%s", anlass ? anlass : "?");
    if (f) {
        re15_entladen_zensus_t z;
        re15_entladen_zensus(&z);
        fprintf(f, "EREIGNIS %s gen=%u raum=%04X", anlass, g_re15_entladen_gen, g_current_room_id);
        zensus_schreiben(f, &z, 1);
        fputc('\n', f);
        fclose(f);
    }
}
