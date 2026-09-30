/* =============================================================================================
 * RE1.5 Rebuilt — Android-Bootstrap (2026-09-19, Entpacker update-sicher seit Runde 34a N1)
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
 *     writeAssetManifest) und werden in den Speicherordner entpackt. PORT-WAHL, kein
 *     Originalverhalten (die PSX las von CD).
 *
 * ENTPACKER (Runde 34a, N1 - Dossier analysis/befunde_runde34_android/android_entpacker_n1.md).
 * Bis v0.8.19 zwei Fehler:
 *   N1a  Die Liste trug nur "<bytes>\t<pfad>", der Marker re15_assets_ok.txt war ein FNV-1a ueber
 *        die Liste, und je Datei wurde nur die Groesse verglichen: eine gleich grosse Aenderung
 *        (P07G.DO2 in Runde 33 dreimal bei 55908 B) blieb nach dem Update ALT auf dem Geraet.
 *   N1b  Quelle war SDL_RWFromFile(<relativer pfad>), und SDL 2.28.5 oeffnet relative Pfade
 *        ZUERST unter dem internen Speicher (SDL_rwops.c:541-557) - lag der Speicherordner intern,
 *        las der Entpacker die eigene, gleichzeitig mit fopen(dst,"wb") geleerte Zieldatei.
 * Jetzt:
 *   - Quelle ist AUSSCHLIESSLICH der AAssetManager der APK (Activity.getAssets() per JNI ->
 *     AAssetManager_fromJava, globale Referenz), auch fuer die Liste re15_assets.txt.
 *   - Liste Format v2 (asset_abgleich.h): "<bytes>\t<sha256>\t<pfad>", strenge Pfadregeln,
 *     jede Abweichung -> gar nicht entpacken (fail closed, Meldung auf dem Schirm).
 *   - Ziel wird als <ziel>.neu geschrieben, beim Schreiben gehasht, und nur wenn Groesse UND
 *     sha256 der Liste entsprechen per rename() an seinen Platz gesetzt.
 *   - Nach vollstaendigem Erfolg: sync(), dann eine Kopie der Liste als re15_assets_entpackt.txt
 *     ("zuletzt entpackt"). Ein Lauf, der etwas aendert, loescht diese Kopie ZUERST (und macht das
 *     per fsync des Ordners haltbar) - ein abgebrochener Lauf hinterlaesst also nie eine Liste, die
 *     mehr verspricht, als auf der Platte liegt.
 *   - Start: Liste bytegleich der zuletzt entpackten -> schneller Weg (nur stat(): Groesse je
 *     Datei). Sonst je Datei (re15_abgleich_planen/_tun): in der alten Liste mit gleicher Groesse
 *     und Summe -> behalten, wenn die Groesse stimmt; geaendert/neu -> entpacken; Pfade nur in
 *     der alten Liste -> loeschen. Ohne gueltige alte Liste (Erstinstallation, v0.8.19-Geraet mit
 *     nur dem alten Marker, abgebrochener Lauf) -> jede gleich grosse Datei per SHA-256 pruefen,
 *     sonst entpacken (Aufwand gemessen: Dossier Abschnitt 3).
 * ============================================================================================= */
#include <SDL.h>
#include <SDL_system.h>
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>
#include <android/log.h>
#include <jni.h>
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#include "re15_engine.h"
#include "asset_root_pc.h"
#include "touch_overlay_pc.h"
#include "asset_abgleich.h"

#define LOGI(...) __android_log_print(ANDROID_LOG_INFO,  "re15", __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, "re15", __VA_ARGS__)

#define LISTE_APK       "re15_assets.txt"            /* assets/ der APK (build.gradle writeAssetManifest) */
#define LISTE_ENTPACKT  "re15_assets_entpackt.txt"   /* Speicherordner: Kopie der Liste nach vollem Erfolg */
#define MARKER_V1       "re15_assets_ok.txt"         /* bis v0.8.19: FNV-1a der v1-Liste */

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

/* AAssetManager der APK. Die Java-Seite (Activity.getAssets()) haelt eine globale Referenz fest:
 * der native Zeiger aus AAssetManager_fromJava gilt nur, solange das Java-Objekt lebt. */
static AAssetManager *s_am;
static jobject s_am_ref;

static AAssetManager *apk_assets(void)
{
    if (s_am) return s_am;
    JNIEnv *env = (JNIEnv *)SDL_AndroidGetJNIEnv();
    jobject act = (jobject)SDL_AndroidGetActivity();          /* lokale Referenz */
    if (!env || !act) return NULL;
    jclass cls = (*env)->GetObjectClass(env, act);
    jmethodID mid = cls ? (*env)->GetMethodID(env, cls, "getAssets", "()Landroid/content/res/AssetManager;") : NULL;
    jobject am = mid ? (*env)->CallObjectMethod(env, act, mid) : NULL;
    if ((*env)->ExceptionCheck(env)) { (*env)->ExceptionClear(env); am = NULL; }
    if (am) {
        s_am_ref = (*env)->NewGlobalRef(env, am);
        (*env)->DeleteLocalRef(env, am);
    }
    if (cls) (*env)->DeleteLocalRef(env, cls);
    (*env)->DeleteLocalRef(env, act);
    if (s_am_ref) s_am = AAssetManager_fromJava(env, s_am_ref);
    return s_am;
}

/* Ganze Datei aus der APK (assets/<name>); NULL = fehlt/zu gross/Lesefehler. */
static char *apk_datei_lesen(AAssetManager *am, const char *name, size_t max, size_t *len)
{
    AAsset *a = AAssetManager_open(am, name, AASSET_MODE_STREAMING);
    if (!a) return NULL;
    off64_t n = AAsset_getLength64(a);
    if (n < 0 || (uint64_t)n > (uint64_t)max) { AAsset_close(a); return NULL; }
    char *b = (char *)malloc((size_t)n + 1);
    size_t got = 0;
    while (b && got < (size_t)n) {
        size_t k = (size_t)n - got;
        if (k > (1u << 20)) k = 1u << 20;
        int r = AAsset_read(a, b + got, k);
        if (r <= 0) break;
        got += (size_t)r;
    }
    AAsset_close(a);
    if (!b || got != (size_t)n) { free(b); return NULL; }
    b[n] = '\0';
    *len = (size_t)n;
    return b;
}

/* Ganze lokale Datei; NULL = fehlt/zu gross/Lesefehler. */
static char *datei_lesen(const char *pfad, size_t max, size_t *len)
{
    FILE *f = fopen(pfad, "rb");
    if (!f) return NULL;
    char *b = NULL;
    long n = -1;
    if (fseek(f, 0, SEEK_END) == 0) n = ftell(f);
    if (n >= 0 && (unsigned long)n <= max && fseek(f, 0, SEEK_SET) == 0) {
        b = (char *)malloc((size_t)n + 1);
        if (b && fread(b, 1, (size_t)n, f) != (size_t)n) { free(b); b = NULL; }
    }
    fclose(f);
    if (!b) return NULL;
    b[n] = '\0';
    *len = (size_t)n;
    return b;
}

/* <s_root>/<rel><endung> nach out; 0 = passt, -1 = zu lang. */
static int pfad_bauen(char *out, size_t n, const char *rel, const char *endung)
{
    int k = snprintf(out, n, "%s/%s%s", s_root, rel, endung ? endung : "");
    return (k > 0 && (size_t)k < n) ? 0 : -1;
}

static void mkdirs_for(const char *path)
{
    char t[PATH_MAX];
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

static void ordner_haltbar(const char *ordner)
{
    int fd = open(ordner, O_RDONLY | O_DIRECTORY | O_CLOEXEC);
    if (fd >= 0) { fsync(fd); close(fd); }
}

static int alles_schreiben(int fd, const char *p, size_t n)
{
    while (n > 0) {
        ssize_t w = write(fd, p, n);
        if (w < 0) { if (errno == EINTR) continue; return -1; }
        p += w; n -= (size_t)w;
    }
    return 0;
}

/* Eine Datei aus der APK als <ziel>.neu schreiben, dabei hashen; nur bei Groesse UND sha256 wie
 * in der Liste per rename() an ihren Platz. 0 = ok. */
static int entpacken(AAssetManager *am, const re15_abgleich_eintrag_t *e, const char *dst, const char *tmp,
                     char *buf, size_t bufn)
{
    mkdirs_for(dst);
    AAsset *a = AAssetManager_open(am, e->pfad, AASSET_MODE_STREAMING);
    if (!a) {
        LOGE("[android] Entpacken: %s fehlt in der APK", e->pfad);
        fprintf(stderr, "[android] FEHLER beim Entpacken: %s fehlt in der APK\n", e->pfad);
        return -1;
    }
    off64_t n = AAsset_getLength64(a);
    if (n != (off64_t)e->groesse) {
        AAsset_close(a);
        LOGE("[android] Entpacken: %s hat in der APK %lld B, die Liste nennt %lld B", e->pfad, (long long)n, e->groesse);
        fprintf(stderr, "[android] FEHLER beim Entpacken: %s APK %lld B != Liste %lld B\n", e->pfad, (long long)n, e->groesse);
        return -1;
    }
    int fd = open(tmp, O_WRONLY | O_CREAT | O_TRUNC | O_CLOEXEC, 0660);
    if (fd < 0) {
        int err = errno;
        AAsset_close(a);
        LOGE("[android] Entpacken: %s nicht anlegbar: %s", tmp, strerror(err));
        fprintf(stderr, "[android] FEHLER beim Entpacken: %s nicht anlegbar: %s\n", tmp, strerror(err));
        return -1;
    }
    re15_sha256_t c;
    re15_sha256_start(&c);
    long long got = 0;
    int kaputt = 0;
    for (;;) {
        int r = AAsset_read(a, buf, bufn);
        if (r < 0) { kaputt = 1; break; }
        if (r == 0) break;
        if (alles_schreiben(fd, buf, (size_t)r) != 0) { kaputt = 2; break; }
        re15_sha256_dazu(&c, buf, (size_t)r);
        got += r;
    }
    AAsset_close(a);
    int err_close = close(fd) != 0 ? errno : 0;
    char hex[65];
    re15_sha256_ende(&c, hex);
    if (kaputt || err_close || got != e->groesse || strcmp(hex, e->sha) != 0) {
        unlink(tmp);
        LOGE("[android] Entpacken fehlgeschlagen: %s (%lld/%lld B, sha256 %.16s.. Liste %.16s.., Grund %d/%d)",
             e->pfad, got, e->groesse, hex, e->sha, kaputt, err_close);
        fprintf(stderr, "[android] FEHLER beim Entpacken: %s (%lld/%lld Bytes, sha256 %s, Liste %s)\n",
                e->pfad, got, e->groesse, hex, e->sha);
        return -1;
    }
    if (rename(tmp, dst) != 0) {
        int err = errno;
        unlink(tmp);
        LOGE("[android] Entpacken: rename(%s) fehlgeschlagen: %s", dst, strerror(err));
        fprintf(stderr, "[android] FEHLER beim Entpacken: rename %s: %s\n", dst, strerror(err));
        return -1;
    }
    return 0;
}

/* Liste als <ziel>.neu, fsync, rename, Ordner fsync. 0 = ok. */
static int liste_schreiben(const char *ziel, const char *text, size_t len)
{
    char tmp[PATH_MAX];
    if (snprintf(tmp, sizeof tmp, "%s%s", ziel, RE15_ABGLEICH_NEU_ENDUNG) >= (int)sizeof tmp) return -1;
    int fd = open(tmp, O_WRONLY | O_CREAT | O_TRUNC | O_CLOEXEC, 0660);
    if (fd < 0) return -1;
    int ok = alles_schreiben(fd, text, len) == 0 && fsync(fd) == 0;
    if (close(fd) != 0) ok = 0;
    if (!ok || rename(tmp, ziel) != 0) { unlink(tmp); return -1; }
    ordner_haltbar(s_root);
    return 0;
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

static void fehler_zeigen(SDL_Renderer *r, const char *text)
{
    for (int i = 0; i < 90; i++) { draw_progress(r, "RE1.5 PORT", text, 0); SDL_Delay(33); }
}

/* ------------------------------------------------------------------------------ 2. Assets */

void re15_android_bootstrap_assets(void)
{
    re15_render_init();                                  /* Fenster + Renderer (idempotent) */
    SDL_Renderer *r = re15_render_pc_renderer();
    Uint32 t0 = SDL_GetTicks();
    char fehler[256];

    AAssetManager *am = apk_assets();
    if (!am) {
        fprintf(stderr, "[android] FEHLER: kein AAssetManager (Activity.getAssets) - keine Assets.\n");
        LOGE("[android] kein AAssetManager (Activity.getAssets)");
        fehler_zeigen(r, "FEHLER: KEIN ZUGRIFF AUF DIE APK-ASSETS");
        return;
    }

    size_t mlen = 0;
    char *man = apk_datei_lesen(am, LISTE_APK, RE15_ABGLEICH_LISTE_MAX, &mlen);
    if (!man) {
        fprintf(stderr, "[android] FEHLER: assets/%s fehlt in der APK (oder > 64 MiB) - keine Assets.\n", LISTE_APK);
        LOGE("[android] assets/%s fehlt in der APK (oder > 64 MiB)", LISTE_APK);
        fehler_zeigen(r, "FEHLER: ASSET-LISTE FEHLT IN DER APK");
        return;
    }
    re15_abgleich_liste_t neu;
    int rc = re15_abgleich_lesen(&neu, man, mlen, fehler, sizeof fehler);
    if (rc != 0) {
        fprintf(stderr, "[android] FEHLER: assets/%s ungueltig (%s) - es wird NICHTS entpackt.\n", LISTE_APK, fehler);
        LOGE("[android] assets/%s ungueltig: %s", LISTE_APK, fehler);
        fehler_zeigen(r, rc == RE15_ABGLEICH_ALTES_FORMAT ? "FEHLER: ASSET-LISTE IM ALTEN FORMAT"
                                                          : "FEHLER: ASSET-LISTE DER APK UNGUELTIG");
        free(man);
        return;
    }

    char pf_liste[PATH_MAX], pf_marker[PATH_MAX], pf_liste_neu[PATH_MAX];
    pfad_bauen(pf_liste, sizeof pf_liste, LISTE_ENTPACKT, NULL);
    pfad_bauen(pf_liste_neu, sizeof pf_liste_neu, LISTE_ENTPACKT, RE15_ABGLEICH_NEU_ENDUNG);
    pfad_bauen(pf_marker, sizeof pf_marker, MARKER_V1, NULL);

    /* "zuletzt entpackt" */
    size_t alen = 0;
    char *alt_text = datei_lesen(pf_liste, RE15_ABGLEICH_LISTE_MAX, &alen);
    int gleich = alt_text && alen == mlen && memcmp(alt_text, man, mlen) == 0;
    re15_abgleich_liste_t alt;
    memset(&alt, 0, sizeof alt);
    int alt_ok = 0;
    if (alt_text && !gleich) {
        char f2[256];
        alt_ok = re15_abgleich_lesen(&alt, alt_text, alen, f2, sizeof f2) == 0;
        if (!alt_ok) {
            fprintf(stderr, "[android] zuletzt-entpackt-Liste unlesbar (%s) -> jede Datei pruefen\n", f2);
            LOGI("[android] zuletzt-entpackt-Liste unlesbar (%s) -> jede Datei pruefen", f2);
        }
    }
    free(alt_text);
    int v1_marker = access(pf_marker, F_OK) == 0;

    re15_abgleich_plan_t plan;
    if (re15_abgleich_planen(&plan, &neu, gleich ? &neu : (alt_ok ? &alt : NULL)) != 0) {
        LOGE("[android] kein Speicher fuer den Abgleich");
        fehler_zeigen(r, "FEHLER: KEIN SPEICHER");
        re15_abgleich_frei(&alt); re15_abgleich_frei(&neu); free(man);
        return;
    }

    char dst[PATH_MAX], tmp[PATH_MAX];

    /* ---- schneller Weg: Liste bytegleich der zuletzt entpackten -> nur Groessen pruefen */
    if (gleich) {
        size_t falsch = 0;
        for (size_t i = 0; i < neu.n; i++) {
            if (pfad_bauen(dst, sizeof dst, neu.e[i].pfad, NULL) != 0 ||
                re15_abgleich_tun(plan.aktion[i], file_size(dst), neu.e[i].groesse) != RE15_TUN_NICHTS)
                falsch++;
        }
        if (falsch == 0) {
            fprintf(stderr, "[android] Assets aktuell (schneller Weg): %zu Dateien, %lld Bytes unter %s (%u ms)\n",
                    neu.n, neu.summe, s_root, (unsigned)(SDL_GetTicks() - t0));
            LOGI("[android] Assets aktuell (schneller Weg): Liste = zuletzt entpackt, %zu Dateien, Groessen geprueft, %u ms",
                 neu.n, (unsigned)(SDL_GetTicks() - t0));
            re15_abgleich_plan_frei(&plan); re15_abgleich_frei(&neu); free(man);
            return;
        }
        fprintf(stderr, "[android] Liste = zuletzt entpackt, aber %zu Dateien fehlen/falsche Groesse -> neu entpacken\n", falsch);
        LOGI("[android] Liste = zuletzt entpackt, aber %zu Dateien fehlen/falsche Groesse -> neu entpacken", falsch);
    }

    /* ---- Lauf, der etwas aendern kann: "zuletzt entpackt" ZUERST weg (haltbar) */
    const char *modus = gleich ? "Groessen-Nachlauf" : alt_ok ? "Update" : v1_marker ? "Uebergang v0.8.19" : "ohne Liste";
    unlink(pf_liste);
    unlink(pf_liste_neu);
    ordner_haltbar(s_root);
    fprintf(stderr, "[android] Abgleich (%s): %zu Dateien (%lld Bytes) - behalten %zu, geaendert %zu, neu %zu, pruefen %zu, weg %zu\n",
            modus, neu.n, neu.summe, plan.n_behalten, plan.n_geaendert, plan.n_neu, plan.n_pruefen, plan.n_weg);
    LOGI("[android] Abgleich (%s): %zu Dateien (%lld Bytes) - behalten %zu, geaendert %zu, neu %zu, pruefen %zu, weg %zu",
         modus, neu.n, neu.summe, plan.n_behalten, plan.n_geaendert, plan.n_neu, plan.n_pruefen, plan.n_weg);
    const char *titel = (plan.n_pruefen && v1_marker) ? "RE1.5 PORT - ASSETS WERDEN EINMALIG GEPRUEFT"
                                                      : "RE1.5 PORT - ASSETS WERDEN ENTPACKT";
    const int einzeln = gleich || alt_ok;                 /* je Datei protokollieren (Update, nicht Erstinstallation) */

    const size_t BUF = 1u << 20;
    char *buf = (char *)malloc(BUF);
    long n_kopiert = 0, n_summe = 0, n_summe_neu = 0, n_fehler = 0, n_reste = 0, n_weg = 0;
    long long b_kopiert = 0, b_summe = 0, done_b = 0;
    Uint32 ms_summe = 0, ms_kopie = 0, last_draw = 0;
    char l2[160];
    if (!buf) n_fehler++;

    for (size_t i = 0; buf && i < neu.n; i++) {
        const re15_abgleich_eintrag_t *e = &neu.e[i];
        if (pfad_bauen(dst, sizeof dst, e->pfad, NULL) != 0 ||
            pfad_bauen(tmp, sizeof tmp, e->pfad, RE15_ABGLEICH_NEU_ENDUNG) != 0) {
            n_fehler++;
            LOGE("[android] Pfad zu lang: %s/%s", s_root, e->pfad);
            continue;
        }
        if (unlink(tmp) == 0) n_reste++;                  /* Rest eines abgebrochenen Laufs */
        long long ist = file_size(dst);
        int tun = re15_abgleich_tun(plan.aktion[i], ist, e->groesse);
        if (tun == RE15_TUN_SUMME_PRUEFEN) {
            char hex[65];
            long long g = -1;
            Uint32 ts = SDL_GetTicks();
            int ok = re15_sha256_datei(dst, hex, &g) == 0 && g == e->groesse && strcmp(hex, e->sha) == 0;
            ms_summe += SDL_GetTicks() - ts;
            n_summe++;
            b_summe += e->groesse;
            if (ok) {
                tun = RE15_TUN_NICHTS;
            } else {
                tun = RE15_TUN_ENTPACKEN;
                n_summe_neu++;
                fprintf(stderr, "[android] Summe weicht ab -> neu: %s\n", e->pfad);
                LOGI("[android] Summe weicht ab -> neu: %s", e->pfad);
            }
        } else if (tun == RE15_TUN_ENTPACKEN && einzeln) {
            static const char *grund[] = { "Groesse falsch/fehlt", "geaendert", "neu", "fehlt/andere Groesse" };
            fprintf(stderr, "[android] entpacke %s (%s)\n", e->pfad, grund[plan.aktion[i] & 3]);
            LOGI("[android] entpacke %s (%s)", e->pfad, grund[plan.aktion[i] & 3]);
        }
        if (tun == RE15_TUN_ENTPACKEN) {
            Uint32 tk = SDL_GetTicks();
            if (entpacken(am, e, dst, tmp, buf, BUF) == 0) { n_kopiert++; b_kopiert += e->groesse; }
            else n_fehler++;
            ms_kopie += SDL_GetTicks() - tk;
        }
        done_b += e->groesse;

        Uint32 now = SDL_GetTicks();
        if (now - last_draw > 80 || i + 1 == neu.n) {
            last_draw = now;
            snprintf(l2, sizeof l2, "%zu / %zu DATEIEN  (%d%%)  %lld MB", i + 1, neu.n,
                     neu.summe > 0 ? (int)(done_b * 100 / neu.summe) : 0, done_b >> 20);
            draw_progress(r, titel, l2, neu.summe > 0 ? (double)done_b / (double)neu.summe : 1.0);
        }
    }
    free(buf);

    /* Pfade, die nur in der alten Liste stehen (Regeln wie jede Listenzeile: nie ausserhalb s_root) */
    for (size_t j = 0; j < plan.n_weg; j++) {
        if (pfad_bauen(dst, sizeof dst, plan.weg[j], NULL) != 0) continue;
        if (unlink(dst) == 0) {
            n_weg++;
            fprintf(stderr, "[android] entfernt (nicht mehr in der Liste): %s\n", plan.weg[j]);
            LOGI("[android] entfernt (nicht mehr in der Liste): %s", plan.weg[j]);
        }
    }

    if (n_fehler == 0) {
        sync();                                           /* erst alle Dateien haltbar, dann die Liste */
        if (liste_schreiben(pf_liste, man, mlen) != 0) {
            n_fehler++;
            LOGE("[android] %s nicht schreibbar: %s", pf_liste, strerror(errno));
        } else if (v1_marker) {
            unlink(pf_marker);                            /* alter Marker ist ab jetzt bedeutungslos */
        }
    }
    Uint32 ms = SDL_GetTicks() - t0;
    fprintf(stderr, "[android] Entpacken fertig (%s): %zu geprueft, %ld kopiert (%lld B, %u ms), %ld per SHA-256 geprueft "
                    "(%lld B, %u ms, %ld abweichend), %ld entfernt, %ld .neu-Reste, %ld Fehler, %u ms\n",
            modus, neu.n, n_kopiert, b_kopiert, (unsigned)ms_kopie, n_summe, b_summe, (unsigned)ms_summe, n_summe_neu,
            n_weg, n_reste, n_fehler, (unsigned)ms);
    LOGI("[android] Entpacken fertig (%s): %zu geprueft, %ld kopiert (%lld B, %u ms), %ld per SHA-256 geprueft "
         "(%lld B, %u ms, %ld abweichend), %ld entfernt, %ld .neu-Reste, %ld Fehler, %u ms",
         modus, neu.n, n_kopiert, b_kopiert, (unsigned)ms_kopie, n_summe, b_summe, (unsigned)ms_summe, n_summe_neu,
         n_weg, n_reste, n_fehler, (unsigned)ms);
    if (n_fehler) {
        snprintf(l2, sizeof l2, "%ld DATEIEN KONNTEN NICHT ENTPACKT WERDEN - SIEHE DEBUG.LOG", n_fehler);
        for (int i = 0; i < 90; i++) { draw_progress(r, "RE1.5 PORT", l2, 1.0); SDL_Delay(33); }
    }
    re15_abgleich_plan_frei(&plan);
    re15_abgleich_frei(&alt);
    re15_abgleich_frei(&neu);
    free(man);
}
