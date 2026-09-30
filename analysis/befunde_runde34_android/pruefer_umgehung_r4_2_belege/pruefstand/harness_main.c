/* Gegenpruefung R4-2 (Umgehung): Linux-Pruefstand fuer den ECHTEN Geraete-Entpacker
 * re15_port/platform/android/jni/android_glue.c (+ asset_abgleich.c), beide UNVERAENDERT mituebersetzt.
 * Attrappen (stub/): SDL (keine Anzeige), JNI (Activity.getAssets), AAssetManager (liest "APK-Assets" aus $H_APK/<name>),
 * android/log (-> stderr). Speicherordner = $H_ROOT (SDL_AndroidGetExternalStoragePath).
 * Steuerung per Umgebung:
 *   H_ROOT, H_APK             Pflicht
 *   H_KEIN_AM=1               CallObjectMethod(getAssets) liefert NULL (JNI-Fehlerpfad "kein AssetManager")
 *   H_KEIN_RENDERER=1         re15_render_pc_renderer() liefert NULL (Anzeige unmoeglich)
 *   H_ABBRUCH_NACH_BYTES=N    AAsset_read beendet den Prozess mit _exit(9), sobald insgesamt N Bytes gelesen sind
 *                             (= Abbruch mitten im Entpacken: force-stop/Absturz)
 * fehler_halten() (Meldung bleibt stehen) beendet sich nur ueber SDL_QUIT - die Attrappe liefert SDL_QUIT beim 3. Poll.
 * Ausgabe: stderr des Entpackers; am Ende "PRUEFSTAND: SPIELSTART" (bootstrap_assets kehrte zurueck) - sonst endet der
 * Prozess in fehler_halten mit exit(1) bzw. im Abbruch mit 9. */
#define _GNU_SOURCE 1
#include <SDL.h>
#include <jni.h>
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>
#include <android/log.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

void re15_android_bootstrap_paths(void);
void re15_android_bootstrap_assets(void);
SDL_Renderer *re15_render_pc_renderer(void);

/* ---- SDL */
Uint32 SDL_GetTicks(void) { struct timespec t; clock_gettime(CLOCK_MONOTONIC, &t); return (Uint32)(t.tv_sec * 1000 + t.tv_nsec / 1000000); }
void SDL_Delay(Uint32 ms) { usleep(ms * 100); }
static int s_polls;
int SDL_PollEvent(SDL_Event *e)
{
    if (++s_polls % 3 == 0) {
        e->type = SDL_QUIT;
        fprintf(stderr, "PRUEFSTAND: SDL_QUIT (App geschlossen) nach %d Polls\n", s_polls);
        return 1;
    }
    return 0;
}
void SDL_PumpEvents(void) {}
static int s_draws;
int SDL_GetRendererOutputSize(SDL_Renderer *r, int *w, int *h) { (void)r; *w = 1280; *h = 720; s_draws++; return 0; }
void SDL_RenderGetLogicalSize(SDL_Renderer *r, int *w, int *h) { (void)r; *w = 0; *h = 0; }
int SDL_RenderSetLogicalSize(SDL_Renderer *r, int w, int h) { (void)r; (void)w; (void)h; return 0; }
int SDL_SetRenderDrawBlendMode(SDL_Renderer *r, int m) { (void)r; (void)m; return 0; }
int SDL_SetRenderDrawColor(SDL_Renderer *r, int a, int b, int c, int d) { (void)r; (void)a; (void)b; (void)c; (void)d; return 0; }
int SDL_RenderClear(SDL_Renderer *r) { (void)r; return 0; }
int SDL_RenderFillRect(SDL_Renderer *r, const SDL_Rect *rc) { (void)r; (void)rc; return 0; }
void SDL_RenderPresent(SDL_Renderer *r) { (void)r; }
int SDL_AndroidGetExternalStorageState(void) { return SDL_ANDROID_EXTERNAL_STORAGE_WRITE | SDL_ANDROID_EXTERNAL_STORAGE_READ; }
const char *SDL_AndroidGetExternalStoragePath(void) { return getenv("H_ROOT"); }
const char *SDL_AndroidGetInternalStoragePath(void) { return getenv("H_ROOT"); }

/* ---- Engine-Teile, die android_glue.c ruft */
void re15_render_init(void) {}
static char s_fake_renderer;
SDL_Renderer *re15_render_pc_renderer(void) { return getenv("H_KEIN_RENDERER") ? NULL : (SDL_Renderer *)&s_fake_renderer; }
static char s_letzter_text[256];
int re15_touch_pc_text(SDL_Renderer *r, int x, int y, int scale, const char *s, int cr, int cg, int cb, int ca)
{
    (void)r; (void)x; (void)y; (void)scale; (void)cr; (void)cg; (void)cb; (void)ca;
    snprintf(s_letzter_text, sizeof s_letzter_text, "%s", s);
    return 0;
}
void re15_pc_set_exe_dir(const char *dir) { (void)dir; }

/* ---- android/log */
int __android_log_print(int prio, const char *tag, const char *fmt, ...)
{
    (void)prio; (void)tag; (void)fmt;
    return 0;                                   /* dieselben Texte gehen im Entpacker auch nach stderr */
}

/* ---- JNI */
static char s_act, s_cls, s_am_obj;
static jclass j_goc(JNIEnv *e, jobject o) { (void)e; (void)o; return &s_cls; }
static jmethodID j_gmi(JNIEnv *e, jclass c, const char *n, const char *s) { (void)e; (void)c; (void)s; return strcmp(n, "getAssets") == 0 ? (jmethodID)&s_cls : NULL; }
static jobject j_com(JNIEnv *e, jobject o, jmethodID m, ...) { (void)e; (void)o; (void)m; return getenv("H_KEIN_AM") ? NULL : &s_am_obj; }
static jboolean j_ec(JNIEnv *e) { (void)e; return 0; }
static void j_ecl(JNIEnv *e) { (void)e; }
static jobject j_ngr(JNIEnv *e, jobject o) { (void)e; return o; }
static void j_dlr(JNIEnv *e, jobject o) { (void)e; (void)o; }
static const struct JNINativeInterface s_jni = { j_goc, j_gmi, j_com, j_ec, j_ecl, j_ngr, j_dlr };
static JNIEnv s_env = &s_jni;
void *SDL_AndroidGetJNIEnv(void) { return &s_env; }
void *SDL_AndroidGetActivity(void) { return &s_act; }

/* ---- AAssetManager: $H_APK/<name> */
struct AAssetManager { int dummy; };
static struct AAssetManager s_am;
struct AAsset { FILE *f; long long n; };
static long long s_gelesen, s_abbruch = -1;
AAssetManager *AAssetManager_fromJava(JNIEnv *env, jobject am) { (void)env; return am ? &s_am : NULL; }
AAsset *AAssetManager_open(AAssetManager *m, const char *name, int mode)
{
    (void)m; (void)mode;
    char p[8192];
    snprintf(p, sizeof p, "%s/%s", getenv("H_APK"), name);
    FILE *f = fopen(p, "rb");
    if (!f) return NULL;
    AAsset *a = (AAsset *)calloc(1, sizeof *a);
    a->f = f;
    fseek(f, 0, SEEK_END); a->n = ftell(f); fseek(f, 0, SEEK_SET);
    return a;
}
off64_t AAsset_getLength64(AAsset *a) { return (off64_t)a->n; }
int AAsset_read(AAsset *a, void *buf, size_t n)
{
    if (s_abbruch >= 0 && s_gelesen + (long long)n > s_abbruch) {
        size_t k = (size_t)(s_abbruch - s_gelesen);
        size_t r = fread(buf, 1, k, a->f);
        s_gelesen += (long long)r;
        fprintf(stderr, "PRUEFSTAND: ABBRUCH nach %lld gelesenen Bytes (Prozess endet mitten im Entpacken)\n", s_gelesen);
        _exit(9);
    }
    size_t r = fread(buf, 1, n, a->f);
    s_gelesen += (long long)r;
    return (int)r;
}
void AAsset_close(AAsset *a) { if (a) { fclose(a->f); free(a); } }

static void beim_ende(void)
{
    fprintf(stderr, "PRUEFSTAND: Ende, %d Zeichenaufrufe, letzte Anzeige '%s'\n", s_draws, s_letzter_text);
}

int main(void)
{
    if (!getenv("H_ROOT") || !getenv("H_APK")) { fprintf(stderr, "H_ROOT/H_APK fehlen\n"); return 2; }
    const char *ab = getenv("H_ABBRUCH_NACH_BYTES");
    if (ab) s_abbruch = atoll(ab);
    atexit(beim_ende);
    re15_android_bootstrap_paths();
    re15_android_bootstrap_assets();
    fprintf(stderr, "PRUEFSTAND: SPIELSTART (bootstrap_assets kehrte zurueck, %d Zeichenaufrufe)\n", s_draws);
    return 0;
}
