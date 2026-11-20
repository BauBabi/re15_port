/* test_r42_ton_schleife.c — Strang "ton", Stufe 2 (2026-11-19): Schleifen-Geraeusche laufen, bis das Spiel sie beendet.
 *
 * Nutzer 19.11.2026: "Zum Beispiel beim öffnen des Rolltors im Flur. Klingt ganz leicht anders."
 * Messbericht U2: das Rolltor-Rumpeln (ROOM1130 snd0 Satz 10, Ton 7, VAG 4) ist eine SCHLEIFEN-Probe: Datei @0x4644,
 * Block 127 Flags 0x06 (Loop Start), Block 369 Flags 0x03 (Loop End + Repeat), psx-spx soundprocessingunitspu.md
 * "SPU ADPCM Samples" Z. 103-141. Original: 6,52 s Rumpeln bis zum Stopp-Ton; Port vorher 2,29 s, dann Stille.
 *
 * Der Riegel rechnet gegen die Bankdaten der Disc und linkt das ECHTE audio_pc.c (RE15_AUDIO_CAP_SYNC: 1470
 * Stereo-Bilder je re15_audio_tick, kein Tongeraet, Regler 100):
 *   (1) Schleifenpunkte aus den Flag-Bytes (re15_vag_loop_start, nur mit -DRE15_TON_S2_HAKEN): alle vier
 *       Schleifen-Proben des Spiels (Zensus 28 Schichten, alle snd0 Stimme 6):
 *       ROOM1130 VAG 4 -> Block 127 = Probe 3556, ROOM1150 VAG 7 -> Block 61 = 1708, ROOM1190 VAG 6 -> Block 197 = 5516,
 *       ROOM11A0 VAG 7 -> Block 127 = 3556; der Stopp-Ton ROOM1130 VAG 5 ist eine Einmal-Probe (-1).
 *   (2) Rumpeln laeuft weiter: Satz 10, 6,5 s (195 Spielbilder) ohne Stopp. In jedem 1/30-s-Fenster zwischen 2,4 s und
 *       6,4 s liegt die Spitze ueber -40 dBFS (vorher: digitale Stille ab 2,29 s).
 *   (3) Ende durch Key-On auf derselben Stimme: Satz 11 (Stimme 6 = SPU 22, Prio 7 > 6, Gate FUN_80045a18 @0x80045a34-58,
 *       gekeyt von der Frame-Pumpe FUN_800458d4) schneidet die Schleife ab; 1,5 s spaeter ist der Mitschnitt still.
 *   (4) Ende durch Raumwechsel: Satz 10 erneut, nach 3 s Raumbanken neu laden (re15_audio_load_room_banks -> Key-Off,
 *       Gegenstueck zu FUN_800443ec: SsUtKeyOffV 0x8005a0d8 fuer SPU 0x10..0x17 @0x800443f4-0x8004442c, gerufen aus dem
 *       Raumlader @0x80039790): ab dem naechsten Spielbild still.
 *   (5) ROOM1150 Satz 12 (Ton 6, VAG 7, 11025 Hz, Probe 1,18 s, Schleife ab Block 61), Abstand im Skript bis zum Stopp
 *       Satz 13: For 0x5b = 91 Spielbilder (ROOM1150.RDT @0x0FF6 `0d 00 04 00 5b 00`, Se_on @0x0FE2 / @0x1000).
 *   (6) ROOM11A0 Satz 10 (Ton 14, VAG 7, Rate 0x232, Probe 1,71 s, Schleife ab Block 127), bis zum Stopp Satz 11:
 *       For 0x46 + For 0x14 = 90 Spielbilder (ROOM11A0.RDT @0x1322 / @0x132C, Se_on @0x1312 / @0x133A).
 *       Fuer (5)/(6): kein Fenster ohne Ton bis zum Stopp (vorher Stille ab dem Probe-Ende), danach nach 1,5 s still.
 *
 *   (7) Runde 2 (Pruefung s2_r1 R1): der Rolladen im Speisesaal ROOM1140/1141 spielt dieselbe Rolltor-Schleife ueber die
 *       Port-Bank 0x3B (= ROOM1131 snd0, dining_r38_pc.c; Satz 0x0A @0x1478 `00 00 76 16` -> Prog 0 Ton 7 Prio 6 Stimme 6
 *       -> VAG 4; Stopp Satz 0x0B @0x147C `00 00 87 16` -> Ton 8 Prio 7 Stimme 6). Ablauf wie Programm ROLL1140
 *       (gen/dining_r38_programme.inc): Se_on 0x0C, Sleep 15, Se_on 0x0A, Sleep 15 + For 20 + For 10x10 + For 3 + For 3
 *       = 141 Spielbilder, Se_on 0x0B. Zwischen 2,4 s und 4,6 s nach 0x0A kein Fenster unter -40 dBFS (vorher: Stille ab
 *       2,29 s), 1,17..1,5 s nach 0x0B still.
 *
 * GEGENPROBE: derselbe Quelltext ohne -DRE15_TON_S2_HAKEN mit dem audio_pc.c von 71caea668 gebaut faellt in (2)
 * (Stille ab 2,29 s). (1) gibt es dort nicht (re15_vag_loop_start fehlt).
 */
#include "re15_audio.h"
#include "re15_engine.h"
#include "re15_vab.h"
#include "re15_rdt.h"
#include "re15_room.h"
#include "re15_actor.h"
#include "re15_scd.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#ifndef RE15_ASSET_PSX_DIR
#define RE15_ASSET_PSX_DIR "shared_assets/PSX"
#endif
#define RATE      44100
#define PER_TICK  (RATE / 30)

/* wie test_r42_ton_mischer.c: das ECHTE audio_pc.c, diese Symbole kommen sonst aus main.c */
re15_engine_state_t g_engine;
void re15_debug_text(int x, int y, int z, const char *text) { (void)x; (void)y; (void)z; (void)text; }
unsigned char *re15_asset_read_file(const char *path, int *out_size)
{
    char buf[600];
    const char *pfx = "shared_assets/PSX/";
    if (strncmp(path, pfx, strlen(pfx)) == 0) snprintf(buf, sizeof buf, "%s/%s", RE15_ASSET_PSX_DIR, path + strlen(pfx));
    else snprintf(buf, sizeof buf, "%s/%s", RE15_ASSET_PSX_DIR, path);
    FILE *f = fopen(buf, "rb");
    if (!f) { snprintf(buf, sizeof buf, "%s", path); f = fopen(buf, "rb"); }
    if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    if (sz <= 0) { fclose(f); return NULL; }
    unsigned char *b = (unsigned char *)malloc((size_t)sz);
    if (b && fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); b = NULL; }
    fclose(f);
    if (b && out_size) *out_size = (int)sz;
    return b;
}

static int fails = 0, checks = 0;
#define CHECK(c, ...) do { checks++; if (!(c)) { printf("FAIL: " __VA_ARGS__); printf("\n"); fails++; } } while (0)

static uint32_t rd32(const uint8_t *p) { return (uint32_t)p[0] | ((uint32_t)p[1] << 8) | ((uint32_t)p[2] << 16) | ((uint32_t)p[3] << 24); }

#ifdef RE15_TON_S2_HAKEN
/* Schleifenpunkt der VAG `vag0` (ab 0) der snd0-Bank eines Raums (RDT-Kopf +0x0c VH, +0x10 VB). */
static int schleife_von(const char *rel, int vag0)
{
    int n = 0;
    uint8_t *d = re15_asset_read_file(rel, &n);
    if (!d) return -2;
    static re15_vab_t vab;
    const uint32_t vh = rd32(d + 0x0c), vb = rd32(d + 0x10), vb_end = rd32(d + 0x14) > vb ? rd32(d + 0x14) : (uint32_t)n;
    int r = -3;
    if (re15_vab_parse(d + vh, vb - vh, &vab) == 0 && vag0 < vab.vag_count) {
        const uint32_t off = vab.samples[vag0].offset, sz = vab.samples[vag0].size;
        if (vb + off + sz <= vb_end) r = re15_vag_loop_start(d + vb + off, sz);
    }
    free(d);
    return r;
}
#endif

static void raum_laden(const char *rel)
{
    int n = 0;
    uint8_t *d = re15_asset_read_file(rel, &n);
    if (!d || re15_rdt_parse(d, (size_t)n, &g_room_rdt) != 0) { printf("FAIL: %s nicht ladbar\n", rel); fails++; return; }
    g_room_rdt_ok = 1;
    g_scd.work_vars[0x0A] = 9;
    memset(&g_actors[0], 0, sizeof g_actors[0]);
    g_actors[0].active = 1; g_actors[0].x = -1614; g_actors[0].z = 17282;
    re15_audio_load_room_banks();                          /* der RDT-Puffer bleibt bewusst bestehen (Baenke zeigen hinein) */
}

static const char *s_cap = "r42_ton_schleife.pcm";
static long s_takt = 0;
static long laufen(int ticks) { const long t0 = s_takt; for (int i = 0; i < ticks; i++) re15_audio_tick(); s_takt += ticks; return t0; }

static int16_t *s_pcm = NULL; static long s_pcm_n = 0;
/* Spitze (Betrag, beide Seiten) im Spielbild `takt` */
static int spitze(long takt)
{
    int pk = 0;
    const long a = takt * PER_TICK;
    if (!s_pcm || a + PER_TICK > s_pcm_n) return -1;
    for (long i = a * 2; i < (a + PER_TICK) * 2; i++) { const int v = abs(s_pcm[i]); if (v > pk) pk = v; }
    return pk;
}

/* (7) dining_r38_pc.c wird mitgelinkt; diese beiden Symbole kommen sonst aus render_pc.c / dining_r39_pc.c. */
void re15_render_pc_request_readback(const char *path) { (void)path; }
void re15_pc_dining_r39_init(void) { }
extern void re15_pc_dining_r38_init(void);
extern unsigned re15_pc_dining_r38_gespielt(int k);
extern void (*re15_dining_r38_se_hook)(int bank, int satz);

int main(void)
{
    static char capenv[1024];
    remove(s_cap);
    snprintf(capenv, sizeof capenv, "RE15_AUDIO_CAP_SYNC=%s", s_cap);
    putenv(capenv);
    static char noint[] = "RE15_NO_INTRO=1";
    putenv(noint);
    setvbuf(stdout, NULL, _IONBF, 0);
    printf("== Strang ton Stufe 2: Schleifen-Geraeusche laufen bis zum Ende durch das Spiel ==\n");

#ifdef RE15_TON_S2_HAKEN
    {   const struct { const char *raum; int vag0, soll; const char *was; } f[] = {
            { "STAGE1/ROOM1130.RDT", 3, 127 * 28, "Rolltor-Rumpeln 1130 Satz 10 (VAG 4, Block 127/369)" },
            { "STAGE1/ROOM1131.RDT", 3, 127 * 28, "Rolltor-Rumpeln 1131 (Elza) Satz 10" },
            { "STAGE1/ROOM1050.RDT", 3, 127 * 28, "Rolltor-Rumpeln 1050 Satz 10" },
            { "STAGE1/ROOM1150.RDT", 6,  61 * 28, "ROOM1150 Satz 12 (VAG 7, Block 61/464)" },
            { "STAGE1/ROOM1190.RDT", 5, 197 * 28, "ROOM1190 Satz 10 (VAG 6, Block 197/427)" },
            { "STAGE1/ROOM11A0.RDT", 6, 127 * 28, "ROOM11A0 Satz 10 (VAG 7, Block 127/369)" },
            { "STAGE1/ROOM1130.RDT", 4, -1,       "Rolltor-Stopp 1130 Satz 11 (VAG 5) = Einmal-Probe" },
        };
        for (size_t i = 0; i < sizeof f / sizeof f[0]; i++) {
            const int ist = schleife_von(f[i].raum, f[i].vag0);
            CHECK(ist == f[i].soll, "(1) %s: Schleifenpunkt %d, Soll %d (Flag-Bytes, psx-spx SPU ADPCM Samples)", f[i].was, ist, f[i].soll);
        }
    }
#endif

    re15_audio_init();
    raum_laden("STAGE1/ROOM1130.RDT");
    enum { N_LANG = 195, N_STOPP = 45, N_RAUM = 90, N_NACH = 15 };
    (void)laufen(4);
    re15_audio_room_se_snd0(10); const long t_rumpeln = laufen(N_LANG);      /* (2) 6,5 s ohne Stopp */
    re15_audio_room_se_snd0(11); const long t_stopp = laufen(N_STOPP);       /* (3) Stopp-Ton auf derselben Stimme */
    re15_audio_room_se_snd0(10); const long t_zwei = laufen(N_RAUM);         /* (4) erneut, dann Raumwechsel */
    raum_laden("STAGE1/ROOM1130.RDT");
    const long t_nach = laufen(N_NACH);
    raum_laden("STAGE1/ROOM1150.RDT");
    re15_audio_room_se_snd0(12); const long t_1150 = laufen(91);
    re15_audio_room_se_snd0(13); const long t_1150s = laufen(N_STOPP);
    raum_laden("STAGE1/ROOM11A0.RDT");
    re15_audio_room_se_snd0(10); const long t_11a0 = laufen(90);
    re15_audio_room_se_snd0(11); const long t_11a0s = laufen(N_STOPP);
    /* (7) Speisesaal: Port-Bank 0x3B */
    raum_laden("STAGE1/ROOM1140.RDT");
    re15_pc_dining_r38_init();
    long t_roll = -1, t_rolls = -1;
    if (re15_dining_r38_se_hook) {
        re15_dining_r38_se_hook(0x3B, 0x0C); (void)laufen(15);
        re15_dining_r38_se_hook(0x3B, 0x0A); t_roll = laufen(141);
        re15_dining_r38_se_hook(0x3B, 0x0B); t_rolls = laufen(N_STOPP);
    }
    re15_audio_shutdown();

    {   FILE *f = fopen(s_cap, "rb");
        if (f) {
            fseek(f, 0, SEEK_END); const long sz = ftell(f); fseek(f, 0, SEEK_SET);
            s_pcm = (int16_t *)malloc((size_t)sz);
            if (s_pcm && fread(s_pcm, 1, (size_t)sz, f) == (size_t)sz) s_pcm_n = sz / 4;
            fclose(f);
        }
        CHECK(s_pcm_n >= s_takt * PER_TICK, "Mitschnitt %s: %ld Stereo-Bilder, erwartet %ld", s_cap, s_pcm_n, s_takt * PER_TICK); }

    /* (2) -40 dBFS = 328 */
    {   int schwach = 0, min_pk = 99999; long min_t = -1;
        for (long k = 72; k < 192; k++) {                   /* 2,4 s .. 6,4 s nach dem Satz */
            const int p = spitze(t_rumpeln + k);
            if (p < min_pk) { min_pk = p; min_t = k; }
            if (p < 328) schwach++;
        }
        printf("  (2) Rumpeln 2,4..6,4 s: kleinste Fenster-Spitze %d (Spielbild %ld), Fenster unter -40 dBFS: %d von 120\n", min_pk, min_t, schwach);
        CHECK(schwach == 0, "(2) Rolltor-Rumpeln ROOM1130 Satz 10 verstummt nach der Probe (%d von 120 Fenstern zwischen 2,4 und 6,4 s "
              "unter -40 dBFS, kleinste Spitze %d): die Schleife (Block 369 Flags 0x03 -> Block 127) laeuft nicht", schwach, min_pk);
    }
    /* (3) nach dem Stopp-Ton (0,9 s) still */
    {   int laut = 0;
        for (long k = 35; k < N_STOPP; k++) if (spitze(t_stopp + k) != 0) laut++;
        printf("  (3) nach Satz 11: Fenster mit Ton zwischen 1,17 und 1,5 s: %d\n", laut);
        CHECK(laut == 0, "(3) Satz 11 auf Stimme 6 beendet die Schleife nicht (%d Fenster mit Ton 1,17..1,5 s nach dem Stopp)", laut);
        CHECK(spitze(t_stopp + 5) > 328, "(3) Stopp-Ton selbst nicht hoerbar (Spitze %d)", spitze(t_stopp + 5));
    }
    /* (4) Raumwechsel */
    {   int laut = 0;
        CHECK(spitze(t_zwei + N_RAUM - 2) > 328, "(4) Rumpeln vor dem Raumwechsel nicht hoerbar (Spitze %d)", spitze(t_zwei + N_RAUM - 2));
        for (long k = 1; k < N_NACH; k++) if (spitze(t_nach + k) != 0) laut++;
        printf("  (4) nach dem Raumwechsel: Fenster mit Ton: %d\n", laut);
        CHECK(laut == 0, "(4) Schleife ueberlebt den Raumwechsel (%d Fenster mit Ton; Original: SsUtKeyOffV 0x10..0x17 @0x800443f4)", laut);
    }

    /* (5)/(6) */
    {   const struct { const char *was; long t0, ts; int n; } f[] = {
            { "(5) ROOM1150 Satz 12 -> Satz 13 nach 91 Bildern", t_1150, t_1150s, 91 },
            { "(6) ROOM11A0 Satz 10 -> Satz 11 nach 90 Bildern", t_11a0, t_11a0s, 90 },
        };
        for (int i = 0; i < 2; i++) {
            int still = 0, erst = -1, laut = 0;
            for (long k = 2; k < f[i].n; k++) if (spitze(f[i].t0 + k) == 0) { still++; if (erst < 0) erst = (int)k; }
            for (long k = 45 - 10; k < 45; k++) if (spitze(f[i].ts + k) != 0) laut++;
            printf("  %s: stille Fenster vor dem Stopp %d (erstes Bild %d); nach dem Stopp 1,17..1,5 s mit Ton: %d\n",
                   f[i].was, still, erst, laut);
            CHECK(still == 0, "%s: %d stille Spielbilder vor dem Stopp (erstes %d) - die Schleife laeuft nicht", f[i].was, still, erst);
            CHECK(laut == 0, "%s: nach dem Stopp-Ton weiter Ton (%d Fenster) - die Schleife haengt", f[i].was, laut);
        }
    }

    /* (7) */
    CHECK(t_roll >= 0 && re15_pc_dining_r38_gespielt(0) == 3, "(7) Port-Bank 0x3B nicht gespielt (Haken %s, gespielt %u)",
          re15_dining_r38_se_hook ? "da" : "fehlt", re15_pc_dining_r38_gespielt(0));
    if (t_roll >= 0) {
        int schwach = 0, min_pk = 99999; long min_t = -1, laut = 0;
        for (long k = 72; k < 138; k++) {                   /* 2,4 s .. 4,6 s nach Satz 0x0A */
            const int p = spitze(t_roll + k);
            if (p < min_pk) { min_pk = p; min_t = k; }
            if (p < 328) schwach++;
        }
        for (long k = 35; k < N_STOPP; k++) if (spitze(t_rolls + k) != 0) laut++;
        printf("  (7) Speisesaal Port-Bank 0x3B Satz 0x0A 2,4..4,6 s: kleinste Spitze %d (Bild %ld), Fenster unter -40 dBFS: %d von 66;"
               " nach Satz 0x0B 1,17..1,5 s mit Ton: %ld\n", min_pk, min_t, schwach, laut);
        CHECK(schwach == 0, "(7) Rolladen 1140/1141 (Port-Bank 0x3B = ROOM1131 snd0 Satz 0x0A, VAG 4) verstummt nach der Probe "
              "(%d von 66 Fenstern zwischen 2,4 und 4,6 s unter -40 dBFS, kleinste Spitze %d): keine Schleife auf diesem Weg", schwach, min_pk);
        CHECK(laut == 0, "(7) Satz 0x0B (Stimme 6, Prio 7) beendet die Rolladen-Schleife nicht (%ld Fenster mit Ton)", laut);
        CHECK(spitze(t_rolls + 5) > 328, "(7) Stopp-Ton 0x0B selbst nicht hoerbar (Spitze %d)", spitze(t_rolls + 5));
    }

    free(s_pcm);
    remove(s_cap);
    printf("%s: %d Pruefungen, %d Fehler\n", fails ? "ROT" : "GRUEN", checks, fails);
    return fails ? 1 : 0;
}
