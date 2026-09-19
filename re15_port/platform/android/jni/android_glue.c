/* =============================================================================================
 * RE1.5 Rebuilt — Android-Bootstrap (2026-09-19)
 *
 * Zwei Aufgaben, beide aus main() (platform/pc/main.c, nur unter __ANDROID__):
 *
 *  1. re15_android_bootstrap_paths()  — ALLERERSTER Aufruf in main():
 *     Der Port kennt nur "neben der exe" und "im Arbeitsverzeichnis". Beides gibt es auf
 *     Android nicht (/proc/self/exe = app_process64, cwd = "/"). Deshalb wird der
 *     App-Speicherordner zum Anker: extern (<sdcard>/Android/data/de.re15.port/files —
 *     per adb/USB einsehbar, dort liegen dann befund.log + debug.log + re15_card.mcr), sonst
 *     intern. chdir() dorthin + re15_pc_set_exe_dir() -> alle Log-/Save-Pfade des Ports
 *     landen dort, und die Asset-Wurzel-Aufloesung findet <anker>/shared_assets/PSX.
 *
 *  2. re15_android_bootstrap_assets() — nach freopen(debug.log), VOR der Wurzel-Aufloesung:
 *     Die Assets liegen unkomprimiert in der APK (assets/, Gradle-Task stageAssets +
 *     writeAssetManifest). Die Liste assets/re15_assets.txt ("<bytes>\t<pfad>" je Zeile,
 *     Kopfzeile "# re15 assets <anzahl> <bytes>") wird per SDL_RWFromFile (= AAssetManager)
 *     gelesen und jede Datei nach <anker>/<pfad> kopiert, sofern sie dort nicht schon in
 *     derselben Groesse liegt. Ein Marker <anker>/re15_assets_ok.txt traegt den FNV-1a-Hash
 *     der Liste: stimmt er, wird gar nichts geprueft (Kaltstart ~0 ms). Fortschritt als
 *     Balken + Text ueber den SDL-Renderer (dafuer wird re15_render_init() hier schon
 *     aufgerufen; der spaetere Aufruf in main() ist ein No-op).
 * ============================================================================================= */
#include <SDL.h>
#include <SDL_system.h>
#include <android/log.h>
#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#include "re15_engine.h"
#include "asset_root_pc.h"
#include "touch_overlay_pc.h"

#define LOGI(...) __android_log_print(ANDROID_LOG_INFO,  "re15", __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, "re15", __VA_ARGS__)

extern SDL_Renderer *re15_render_pc_renderer(void);   /* render_pc.c (nur __ANDROID__) */

static char s_root[512];

const char *re15_android_root(void) { return s_root; }

/* ------------------------------------------------------------------------------ 1. Pfade */

void re15_android_bootstrap_paths(void)
{
    const char *p = NULL;
    int st = SDL_AndroidGetExternalStorageState();
    if (st & SDL_ANDROID_EXTERNAL_STORAGE_WRITE) p = SDL_AndroidGetExternalStoragePath();
    if (!p || !p[0]) p = SDL_AndroidGetInternalStoragePath();
    if (!p || !p[0]) p = "/data/local/tmp";
    snprintf(s_root, sizeof s_root, "%s", p);
    mkdir(s_root, 0770);
    if (chdir(s_root) != 0)
        LOGE("[android] chdir(%s) fehlgeschlagen: %s", s_root, strerror(errno));
    re15_pc_set_exe_dir(s_root);
    LOGI("[android] Speicherordner (exe-Anker + cwd): %s (extern=%d)", s_root,
         (st & SDL_ANDROID_EXTERNAL_STORAGE_WRITE) ? 1 : 0);
}

/* ------------------------------------------------------------------------------ Werkzeug */

static uint64_t fnv1a64(const char *s, size_t n)
{
    uint64_t h = 1469598103934665603ULL;
    for (size_t i = 0; i < n; i++) { h ^= (unsigned char)s[i]; h *= 1099511628211ULL; }
    return h;
}

static char *read_apk_asset(const char *name, size_t *len)
{
    SDL_RWops *rw = SDL_RWFromFile(name, "rb");
    if (!rw) return NULL;
    Sint64 sz = SDL_RWsize(rw);
    if (sz < 0 || sz > (Sint64)(64u << 20)) { SDL_RWclose(rw); return NULL; }
    char *b = (char *)malloc((size_t)sz + 1);
    if (!b) { SDL_RWclose(rw); return NULL; }
    size_t got = SDL_RWread(rw, b, 1, (size_t)sz);
    SDL_RWclose(rw);
    b[got] = '\0';
    if (len) *len = got;
    return b;
}

static void mkdirs_for(const char *path)
{
    char t[1024];
    snprintf(t, sizeof t, "%s", path);
    for (char *q = t + 1; *q; q++) {
        if (*q == '/') { *q = '\0'; mkdir(t, 0770); *q = '/'; }
    }
}

static long long file_size(const char *path)
{
    struct stat sb;
    if (stat(path, &sb) != 0 || !S_ISREG(sb.st_mode)) return -1;
    return (long long)sb.st_size;
}

/* ------------------------------------------------------------------------------ Anzeige */

static void draw_progress(SDL_Renderer *r, const char *l1, const char *l2, double frac)
{
    if (!r) return;
    int W = 0, H = 0, lw = 0, lh = 0;
    SDL_GetRendererOutputSize(r, &W, &H);
    if (W <= 0 || H <= 0) return;
    SDL_RenderGetLogicalSize(r, &lw, &lh);
    SDL_RenderSetLogicalSize(r, 0, 0);
    SDL_SetRenderDrawBlendMode(r, SDL_BLENDMODE_NONE);
    SDL_SetRenderDrawColor(r, 0, 0, 0, 255);
    SDL_RenderClear(r);

    int u  = H / 12; if (u < 8) u = 8;
    int fs = u / 9;  if (fs < 2) fs = 2;
    int tw1 = (int)strlen(l1) * 6 * fs;
    re15_touch_pc_text(r, (W - tw1) / 2, H / 2 - 2 * u, fs, l1, 230, 230, 230, 255);

    int bx = W / 6, bw = W * 2 / 3, by = H / 2 - u / 2, bh = u;
    SDL_SetRenderDrawBlendMode(r, SDL_BLENDMODE_NONE);
    SDL_SetRenderDrawColor(r, 110, 110, 110, 255);
    { SDL_Rect o[4] = { { bx, by, bw, 3 }, { bx, by + bh - 3, bw, 3 }, { bx, by, 3, bh }, { bx + bw - 3, by, 3, bh } };
      for (int i = 0; i < 4; i++) SDL_RenderFillRect(r, &o[i]); }
    if (frac < 0) frac = 0; if (frac > 1) frac = 1;
    SDL_SetRenderDrawColor(r, 200, 40, 40, 255);
    { SDL_Rect f = { bx + 5, by + 5, (int)((double)(bw - 10) * frac), bh - 10 };
      if (f.w > 0) SDL_RenderFillRect(r, &f); }

    int fs2 = (fs > 2) ? fs - 1 : fs;
    int tw2 = (int)strlen(l2) * 6 * fs2;
    re15_touch_pc_text(r, (W - tw2) / 2, by + bh + u / 2, fs2, l2, 200, 200, 200, 255);

    SDL_RenderPresent(r);
    SDL_RenderSetLogicalSize(r, lw, lh);
    SDL_PumpEvents();
}

/* ------------------------------------------------------------------------------ 2. Assets */

void re15_android_bootstrap_assets(void)
{
    re15_render_init();                                  /* Fenster + Renderer (idempotent) */
    SDL_Renderer *r = re15_render_pc_renderer();

    size_t mlen = 0;
    char *man = read_apk_asset("re15_assets.txt", &mlen);
    if (!man || mlen == 0) {
        fprintf(stderr, "[android] FEHLER: assets/re15_assets.txt fehlt in der APK — keine Assets.\n");
        LOGE("[android] assets/re15_assets.txt fehlt in der APK");
        for (int i = 0; i < 90; i++) { draw_progress(r, "RE1.5 PORT", "FEHLER: ASSET-LISTE FEHLT IN DER APK", 0); SDL_Delay(33); }
        free(man);
        return;
    }

    uint64_t hash = fnv1a64(man, mlen);
    long count = 0; long long total = 0;
    sscanf(man, "# re15 assets %ld %lld", &count, &total);

    char marker[600], want[128];
    snprintf(marker, sizeof marker, "%s/re15_assets_ok.txt", s_root);
    snprintf(want, sizeof want, "%016llx %ld %lld", (unsigned long long)hash, count, total);
    {
        FILE *mf = fopen(marker, "rb");
        if (mf) {
            char have[128] = {0};
            size_t n = fread(have, 1, sizeof have - 1, mf);
            fclose(mf);
            while (n > 0 && (have[n-1] == '\n' || have[n-1] == '\r')) have[--n] = '\0';
            if (strcmp(have, want) == 0) {
                fprintf(stderr, "[android] Assets bereits entpackt (%ld Dateien, %lld Bytes) unter %s\n", count, total, s_root);
                LOGI("[android] Assets aktuell (%s)", want);
                free(man);
                return;
            }
            fprintf(stderr, "[android] Asset-Marker weicht ab ('%s' != '%s') -> pruefe/entpacke neu\n", have, want);
        }
    }

    fprintf(stderr, "[android] Entpacke %ld Dateien (%lld Bytes) aus der APK nach %s\n", count, total, s_root);
    LOGI("[android] Entpacke %ld Dateien (%lld Bytes) nach %s", count, total, s_root);

    const size_t BUF = 1u << 20;
    char *buf = (char *)malloc(BUF);
    long done_n = 0, copied_n = 0, failed_n = 0;
    long long done_b = 0;
    Uint32 last_draw = 0;
    char l2[160];

    char *line = man, *next;
    for (; line && *line; line = next) {
        next = strchr(line, '\n');
        if (next) { *next = '\0'; next++; }
        size_t L = strlen(line);
        while (L > 0 && line[L-1] == '\r') line[--L] = '\0';
        if (L == 0 || line[0] == '#') continue;
        char *tab = strchr(line, '\t');
        if (!tab) continue;
        *tab = '\0';
        long long sz = atoll(line);
        const char *rel = tab + 1;

        char dst[1024];
        snprintf(dst, sizeof dst, "%s/%s", s_root, rel);
        if (file_size(dst) != sz) {
            mkdirs_for(dst);
            SDL_RWops *rw = SDL_RWFromFile(rel, "rb");
            FILE *f = rw ? fopen(dst, "wb") : NULL;
            long long got = 0;
            if (rw && f && buf) {
                for (;;) {
                    size_t n = SDL_RWread(rw, buf, 1, BUF);
                    if (n == 0) break;
                    if (fwrite(buf, 1, n, f) != n) { got = -1; break; }
                    got += (long long)n;
                }
            }
            if (f) fclose(f);
            if (rw) SDL_RWclose(rw);
            if (got != sz) {
                failed_n++;
                fprintf(stderr, "[android] FEHLER beim Entpacken: %s (%lld/%lld Bytes, rw=%p f=%p)\n",
                        rel, got, sz, (void *)rw, (void *)f);
                LOGE("[android] Entpacken fehlgeschlagen: %s (%lld/%lld)", rel, got, sz);
            } else {
                copied_n++;
            }
        }
        done_n++; done_b += sz;

        Uint32 now = SDL_GetTicks();
        if (now - last_draw > 80 || done_n == count) {
            last_draw = now;
            snprintf(l2, sizeof l2, "%ld / %ld DATEIEN  (%d%%)  %lld MB", done_n, count,
                     total > 0 ? (int)(done_b * 100 / total) : 0, done_b >> 20);
            draw_progress(r, "RE1.5 PORT - ASSETS WERDEN ENTPACKT", l2, total > 0 ? (double)done_b / (double)total : 0.0);
        }
    }
    free(buf);
    free(man);

    fprintf(stderr, "[android] Entpacken fertig: %ld geprueft, %ld kopiert, %ld Fehler\n", done_n, copied_n, failed_n);
    LOGI("[android] Entpacken fertig: %ld geprueft, %ld kopiert, %ld Fehler", done_n, copied_n, failed_n);
    if (failed_n == 0) {
        FILE *mf = fopen(marker, "wb");
        if (mf) { fprintf(mf, "%s\n", want); fclose(mf); }
    } else {
        snprintf(l2, sizeof l2, "%ld DATEIEN KONNTEN NICHT ENTPACKT WERDEN - SIEHE DEBUG.LOG", failed_n);
        for (int i = 0; i < 90; i++) { draw_progress(r, "RE1.5 PORT", l2, 1.0); SDL_Delay(33); }
    }
}
