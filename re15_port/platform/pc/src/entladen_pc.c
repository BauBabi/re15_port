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
/* room_pc.c */
extern int  re15_room_pc_belegt(unsigned *gen);
extern void re15_room_pc_entladen(void);

const char *const re15_entladen_fachname[RE15_FACH_ANZAHL] = {
    "pri_masken", "pri_atlas", "sld", "msk", "tim", "gegner",
    "esp_bank", "esp_fx", "esp_pool", "re2fx", "rdt"
};

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
        int fr = am_ereignis && (i == RE15_FACH_ESP_FX || i == RE15_FACH_ESP_POOL || i == RE15_FACH_RE2FX)
               ? z->belegt[i] : z->fremd[i];
        fprintf(f, " %s=%d", re15_entladen_fachname[i], fr);
    }
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
    /* Zeile nur bei Befund und gedrosselt (erstes Bild + jedes 30.), sonst waere das Log riesig. */
    if ((z.fremd_summe > 0 || masken_fremd > 0) &&
        (s_bilder_fremd + s_bilder_masken_fremd == 1 || (s_bilder % 30u) == 0u)) {
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
static void alles_entladen(void)
{
    /* (Schritt 2 der Runde — siehe Dossier "Umsetzung") */
}

void re15_entladen_ereignis(const char *anlass)
{
    FILE *f = log_oeffnen();
    if (f) {   /* Bilanz der Strecke, die jetzt endet — VOR dem Generationswechsel */
        re15_entladen_zensus_t z0;
        re15_entladen_zensus(&z0);
        fprintf(f, "VORHER %s gen=%u raum=%04X", anlass, g_re15_entladen_gen, g_current_room_id);
        zensus_schreiben(f, &z0, 0);
        fputc('\n', f);
        summe_schreiben(f);
    }
    re15_entladen_gen_weiter();
    alles_entladen();
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
