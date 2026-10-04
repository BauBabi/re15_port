/* =============================================================================================
 * Runde 35 Spur N "android" — Pruefstand fuer den ECHTEN Geraete-Entpacker
 * re15_port/platform/android/jni/android_glue.c (+ asset_abgleich.c), beide UNVERAENDERT mituebersetzt.
 * Vorbild: Linux-Pruefstand der Gegenpruefung R4-2 (analysis/befunde_runde34_android/pruefer_umgehung_r4_2_belege/
 * pruefstand/harness_main.c). Neu: laeuft auch unter mingw (kompat_win.h), Displaygroesse waehlbar, und jede
 * gezeichnete Textzeile/jedes Rechteck wird gegen die Displaygrenzen gemessen (Punkt 1 "Fortschrittsanzeige seitlich
 * abgeschnitten"). Dossier: analysis/befunde_runde35/N_android.md. PORT-WAHL-Werkzeug, kein Originalverhalten.
 *
 * Attrappen: SDL (keine Anzeige - Zeichenaufrufe werden vermessen), JNI (Activity.getAssets), AAssetManager (liest
 * die "APK-Assets" aus $H_APK/<name>), android/log (verworfen: dieselben Texte gehen im Entpacker auch nach stderr).
 * Speicherordner = $H_ROOT (SDL_AndroidGetExternalStoragePath).
 * Umgebung:
 *   H_ROOT, H_APK   Pflicht
 *   H_W, H_H        Ausgabegroesse des Renderers (SDL_GetRendererOutputSize), Standard 1280x720
 * fehler_halten() (Meldung bleibt stehen) endet nur ueber SDL_QUIT - die Attrappe liefert SDL_QUIT beim 3. Poll.
 * Ausgabe (stderr):
 *   je SDL_RenderPresent:  PRUEFSTAND-BILD W=.. H=.. zeilen=n ausserhalb=k ueberlappung=m text='<zeilen, mit ' ' verbunden>'
 *   je Verstoss:          PRUEFSTAND-AUSSERHALB ... / PRUEFSTAND-UEBERLAPPUNG ...
 *   am Ende:              PRUEFSTAND: SPIELSTART (bootstrap_assets kehrte zurueck) - sonst exit(1) aus fehler_halten
 * ============================================================================================= */
#if !defined(_WIN32) && !defined(_GNU_SOURCE)
#  define _GNU_SOURCE 1
#endif
#include <SDL.h>
#include <jni.h>
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>
#include <android/log.h>
#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#if defined(_WIN32)
#  include <windows.h>
#  include <direct.h>
#  include <io.h>
#  include <fcntl.h>
#else
#  include <time.h>
#  include <unistd.h>
#endif

void re15_android_bootstrap_paths(void);
void re15_android_bootstrap_assets(void);
SDL_Renderer *re15_render_pc_renderer(void);

/* ------------------------------------------------------------------------------ mingw: POSIX-Ersatz (kompat_win.h) */
#if defined(_WIN32)
int r35_mkdir(const char *p, int mode) { (void)mode; return _mkdir(p); }
int r35_rename(const char *a, const char *b)
{
    if (MoveFileExA(a, b, MOVEFILE_REPLACE_EXISTING)) return 0;
    DWORD e = GetLastError();
    struct stat sb;
    if (stat(b, &sb) == 0 && S_ISDIR(sb.st_mode)) errno = EISDIR;          /* POSIX: rename(Datei, Ordner) -> EISDIR */
    else if (e == ERROR_FILE_NOT_FOUND || e == ERROR_PATH_NOT_FOUND) errno = ENOENT;
    else errno = EACCES;
    return -1;
}
int r35_fsync(int fd) { return _commit(fd); }
void r35_sync(void) {}
#endif

/* ------------------------------------------------------------------------------ SDL */
Uint32 SDL_GetTicks(void)
{
#if defined(_WIN32)
    return (Uint32)GetTickCount();
#else
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return (Uint32)(t.tv_sec * 1000 + t.tv_nsec / 1000000);
#endif
}
void SDL_Delay(Uint32 ms) { (void)ms; }
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

static int s_w = 1280, s_h = 720;
int SDL_GetRendererOutputSize(SDL_Renderer *r, int *w, int *h) { (void)r; *w = s_w; *h = s_h; return 0; }
void SDL_RenderGetLogicalSize(SDL_Renderer *r, int *w, int *h) { (void)r; *w = 0; *h = 0; }
int SDL_RenderSetLogicalSize(SDL_Renderer *r, int w, int h) { (void)r; (void)w; (void)h; return 0; }
int SDL_SetRenderDrawBlendMode(SDL_Renderer *r, int m) { (void)r; (void)m; return 0; }
int SDL_SetRenderDrawColor(SDL_Renderer *r, Uint8 a, Uint8 b, Uint8 c, Uint8 d) { (void)r; (void)a; (void)b; (void)c; (void)d; return 0; }

/* Ein Bild: gezeichnete Textzeilen (Kasten wie re15_touch_pc_text: Glyphe 5x7, Vorschub 6*scale -> Breite
 * (6*len - 1)*scale, Hoehe 7*scale; touch_overlay_pc.c:466-486) und Rechtecke (Balken). */
typedef struct { int x0, y0, x1, y1; } kasten_t;   /* halboffen [x0,x1) x [y0,y1) */
#define MAX_ZEILEN 32
#define MAX_RECHTECKE 32
static kasten_t s_zk[MAX_ZEILEN];
static char s_zt[MAX_ZEILEN][256];
static int s_zn;
static kasten_t s_rk[MAX_RECHTECKE];
static int s_rn;
static int s_bilder, s_bilder_ausserhalb, s_bilder_ueberlappung;
static char s_letzter_text[2048];

int SDL_RenderClear(SDL_Renderer *r) { (void)r; s_zn = 0; s_rn = 0; return 0; }
int SDL_RenderFillRect(SDL_Renderer *r, const SDL_Rect *rc)
{
    (void)r;
    if (rc && s_rn < MAX_RECHTECKE) { kasten_t k = { rc->x, rc->y, rc->x + rc->w, rc->y + rc->h }; s_rk[s_rn++] = k; }
    return 0;
}

int re15_touch_pc_text(SDL_Renderer *r, int x, int y, int scale, const char *s,
                       uint8_t cr, uint8_t cg, uint8_t cb, uint8_t ca)
{
    (void)r; (void)cr; (void)cg; (void)cb; (void)ca;
    if (!s) return 0;
    if (scale < 1) scale = 1;                                  /* wie das Original touch_overlay_pc.c:470 */
    int len = (int)strlen(s);
    if (s_zn < MAX_ZEILEN) {
        kasten_t k = { x, y, x + (len > 0 ? (6 * len - 1) * scale : 0), y + 7 * scale };
        s_zk[s_zn] = k;
        snprintf(s_zt[s_zn], sizeof s_zt[s_zn], "%s", s);
        s_zn++;
    }
    return 6 * len * scale;
}

static int schneidet(const kasten_t *a, const kasten_t *b)
{
    return a->x0 < b->x1 && b->x0 < a->x1 && a->y0 < b->y1 && b->y0 < a->y1;
}

void SDL_RenderPresent(SDL_Renderer *r)
{
    (void)r;
    int aus = 0, ueber = 0;
    s_letzter_text[0] = '\0';
    for (int i = 0; i < s_zn; i++) {
        const kasten_t *k = &s_zk[i];
        if (k->x0 < 0 || k->y0 < 0 || k->x1 > s_w || k->y1 > s_h) {
            aus++;
            fprintf(stderr, "PRUEFSTAND-AUSSERHALB Text W=%d H=%d x=%d..%d y=%d..%d '%s'\n", s_w, s_h, k->x0, k->x1,
                    k->y0, k->y1, s_zt[i]);
        }
        for (int j = 0; j < i; j++)
            if (schneidet(k, &s_zk[j])) {
                ueber++;
                fprintf(stderr, "PRUEFSTAND-UEBERLAPPUNG Text/Text '%s' / '%s'\n", s_zt[j], s_zt[i]);
            }
        for (int j = 0; j < s_rn; j++)
            if (schneidet(k, &s_rk[j])) {
                ueber++;
                fprintf(stderr, "PRUEFSTAND-UEBERLAPPUNG Text/Balken '%s' x=%d..%d y=%d..%d\n", s_zt[i], s_rk[j].x0,
                        s_rk[j].x1, s_rk[j].y0, s_rk[j].y1);
            }
        size_t l = strlen(s_letzter_text);
        snprintf(s_letzter_text + l, sizeof s_letzter_text - l, "%s%s", i ? " " : "", s_zt[i]);
    }
    for (int j = 0; j < s_rn; j++) {
        const kasten_t *k = &s_rk[j];
        if (k->x0 < 0 || k->y0 < 0 || k->x1 > s_w || k->y1 > s_h) {
            aus++;
            fprintf(stderr, "PRUEFSTAND-AUSSERHALB Rechteck W=%d H=%d x=%d..%d y=%d..%d\n", s_w, s_h, k->x0, k->x1, k->y0,
                    k->y1);
        }
    }
    /* Geometrie je Zeile, wenn sich der Bildinhalt gegenueber dem vorigen Bild aendert (Beleg der gewaehlten Skalierung) */
    static char s_vorher[2048];
    if (strcmp(s_vorher, s_letzter_text) != 0) {
        for (int i = 0; i < s_zn; i++)
            fprintf(stderr, "PRUEFSTAND-ZEILE W=%d H=%d x=%d..%d y=%d..%d s=%d '%s'\n", s_w, s_h, s_zk[i].x0, s_zk[i].x1,
                    s_zk[i].y0, s_zk[i].y1, (s_zk[i].y1 - s_zk[i].y0) / 7, s_zt[i]);
        snprintf(s_vorher, sizeof s_vorher, "%s", s_letzter_text);
    }
    s_bilder++;
    if (aus) s_bilder_ausserhalb++;
    if (ueber) s_bilder_ueberlappung++;
    fprintf(stderr, "PRUEFSTAND-BILD W=%d H=%d zeilen=%d ausserhalb=%d ueberlappung=%d text='%s'\n", s_w, s_h, s_zn, aus,
            ueber, s_letzter_text);
}

int SDL_AndroidGetExternalStorageState(void) { return SDL_ANDROID_EXTERNAL_STORAGE_WRITE | SDL_ANDROID_EXTERNAL_STORAGE_READ; }
const char *SDL_AndroidGetExternalStoragePath(void) { return getenv("H_ROOT"); }
const char *SDL_AndroidGetInternalStoragePath(void) { return getenv("H_ROOT"); }

/* ------------------------------------------------------------------------------ Engine-Teile, die android_glue.c ruft */
void re15_render_init(void) {}
static char s_fake_renderer;
SDL_Renderer *re15_render_pc_renderer(void) { return (SDL_Renderer *)&s_fake_renderer; }
void re15_pc_set_exe_dir(const char *dir) { (void)dir; }

/* ------------------------------------------------------------------------------ android/log */
int __android_log_print(int prio, const char *tag, const char *fmt, ...) { (void)prio; (void)tag; (void)fmt; return 0; }

/* ------------------------------------------------------------------------------ JNI */
static char s_act, s_cls, s_am_obj;
static jclass j_goc(JNIEnv *e, jobject o) { (void)e; (void)o; return &s_cls; }
static jmethodID j_gmi(JNIEnv *e, jclass c, const char *n, const char *s)
{
    (void)e; (void)c; (void)s;
    return strcmp(n, "getAssets") == 0 ? (jmethodID)&s_cls : NULL;
}
static jobject j_com(JNIEnv *e, jobject o, jmethodID m, ...) { (void)e; (void)o; (void)m; return &s_am_obj; }
static jboolean j_ec(JNIEnv *e) { (void)e; return 0; }
static void j_ecl(JNIEnv *e) { (void)e; }
static jobject j_ngr(JNIEnv *e, jobject o) { (void)e; return o; }
static void j_dlr(JNIEnv *e, jobject o) { (void)e; (void)o; }
static const struct JNINativeInterface s_jni = { j_goc, j_gmi, j_com, j_ec, j_ecl, j_ngr, j_dlr };
static JNIEnv s_env = &s_jni;
void *SDL_AndroidGetJNIEnv(void) { return &s_env; }
void *SDL_AndroidGetActivity(void) { return &s_act; }

/* ------------------------------------------------------------------------------ AAssetManager: $H_APK/<name> */
struct AAssetManager { int dummy; };
static struct AAssetManager s_am;
struct AAsset { FILE *f; long long n; };
AAssetManager *AAssetManager_fromJava(JNIEnv *env, jobject am) { (void)env; return am ? &s_am : NULL; }
AAsset *AAssetManager_open(AAssetManager *m, const char *name, int mode)
{
    (void)m; (void)mode;
    char p[4096];
    snprintf(p, sizeof p, "%s/%s", getenv("H_APK"), name);
    struct stat sb;
    if (stat(p, &sb) != 0 || !S_ISREG(sb.st_mode)) return NULL;  /* die APK kennt keine Ordner als Asset */
    FILE *f = fopen(p, "rb");
    if (!f) return NULL;
    AAsset *a = (AAsset *)calloc(1, sizeof *a);
    if (!a) { fclose(f); return NULL; }
    a->f = f;
    a->n = (long long)sb.st_size;
    return a;
}
off64_t AAsset_getLength64(AAsset *a) { return (off64_t)a->n; }
int AAsset_read(AAsset *a, void *buf, size_t n) { return (int)fread(buf, 1, n, a->f); }
void AAsset_close(AAsset *a) { if (a) { fclose(a->f); free(a); } }

/* ------------------------------------------------------------------------------ main */
static void beim_ende(void)
{
    fprintf(stderr, "PRUEFSTAND: Ende, %d Bilder (%d mit Text/Rechteck ausserhalb, %d mit Ueberlappung), letzte Anzeige '%s'\n",
            s_bilder, s_bilder_ausserhalb, s_bilder_ueberlappung, s_letzter_text);
    fflush(stderr);
}

int main(void)
{
#if defined(_WIN32)
    _set_fmode(_O_BINARY);                 /* open()/fopen() ohne 'b' sonst im Textmodus (CRLF) - Geraet: immer binaer */
#endif
    if (!getenv("H_ROOT") || !getenv("H_APK")) { fprintf(stderr, "H_ROOT/H_APK fehlen\n"); return 2; }
    if (getenv("H_W")) s_w = atoi(getenv("H_W"));
    if (getenv("H_H")) s_h = atoi(getenv("H_H"));
    setvbuf(stderr, NULL, _IONBF, 0);
    atexit(beim_ende);
    re15_android_bootstrap_paths();
    re15_android_bootstrap_assets();
    fprintf(stderr, "PRUEFSTAND: SPIELSTART (bootstrap_assets kehrte zurueck)\n");
    return 0;
}
