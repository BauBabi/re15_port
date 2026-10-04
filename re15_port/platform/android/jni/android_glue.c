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
 *     der alten Liste -> loeschen, und zwar VOR dem Entpacken (der App-Speicher ist case-insensitiv:
 *     "a/X.BIN" alt, "a/x.bin" neu waere sonst nach dem Entpacken wieder geloescht). Ohne gueltige alte Liste (Erstinstallation, v0.8.19-Geraet mit
 *     nur dem alten Marker, abgebrochener Lauf) -> jede gleich grosse Datei per SHA-256 pruefen,
 *     sonst entpacken (Aufwand gemessen: Dossier Abschnitt 3).
 * Nachbesserung R4-1 (Dossier analysis/befunde_runde34_android/android_r4_nachbesserung.md):
 *   - U2/E1 Waisen: ohne gueltige alte Liste werden shared_assets/ und synchro/ vor dem Entpacken durchgegangen,
 *     alles, was nicht in der neuen Liste steht (auch .neu-Reste), wird geloescht (re15_abgleich_waisen) - vorher
 *     blieben nach Abbruch + Update bzw. beim Uebergang von v0.8.19 gestrichene Dateien fuer immer liegen.
 *   - U4 fail closed: jeder Fehler (kein AssetManager, Liste fehlt/ungueltig/v1, kein Speicher, Dateifehler) haelt
 *     die Meldung stehen, bis die App geschlossen wird - das Spiel startet nicht mit altem/gemischtem Baum.
 *   - U1: scheitert das Loeschen der "zuletzt entpackt"-Liste, laeuft kein Lauf.
 *   - H5/U3 (asset_abgleich.c): Pfade nur druckbares ASCII, Segment <= 251 B.
 * Runde 35 Spur N (Dossier analysis/befunde_runde35/N_android.md; gemessen im Pruefstand tests/unit/r35_android/):
 *   - E2-1 Fortschrittsanzeige: Schrift aus Hoehe UND Breite (text_block), auf 20:9/16:9 nichts mehr abgeschnitten.
 *   - F-Y4/H8 Datei <-> Ordner gleichen Namens im Update: re15_abgleich_weg_frei vor jedem Entpacken und leer gewordene
 *     Ordner nach dem Loeschen (re15_abgleich_leere_eltern) - das Update wird im SELBEN Start fertig, nicht erst im
 *     naechsten; F-Y6 nicht loeschbare weg-Pfade werden gemeldet statt verschluckt.
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

/* Runde 35 Spur N (Befund E2-1, Dossier analysis/befunde_runde35/N_android.md Punkt 1). PORT-WAHL, kein
 * Originalverhalten (die PSX hatte keinen Entpacker). Bis dahin kam die Schriftgroesse NUR aus der Hoehe (fs = H/108),
 * die Breite des Textes (Zeichen x 6 x fs) wurde nie gegen W geprueft: auf 2400x1080 stand der 44-Zeichen-Titel bei
 * x = -120 ("RE" und "FT" weg), die 57/58-Zeichen-Fehlertexte bei x = -339/-366 (gemessen im Pruefstand und auf dem
 * Emulator der Runde 34a). Jetzt:
 *   - seitlicher Rand 0.4u (u = H/12) - derselbe Randabstand wie die Bedienelemente des Ports
 *     (touch_overlay_pc.c:175 Schultertasten bei 0.4u, gleiche Einheit u = H/12, touch_overlay_pc.c:151);
 *   - Schrift = min(Hoehen-Groesse wie bisher, (W - 2 Rand) / (6 x Zeichen)): auf jedem Seitenverhaeltnis ganz im Bild;
 *   - passt der Text selbst mit Skalierung 1 nicht (sehr schmale Ausgabe), wird er an Leerzeichen umbrochen
 *     (Zeilenabstand 9 Pixel je Skalierung: Glyphe 7 hoch + 2 frei). Die Titelzeilen wachsen nach OBEN, die Zeile unter
 *     dem Balken nach UNTEN - der Balken bleibt frei. */
#define FORTSCHRITT_ZEILEN_MAX 6
#define FORTSCHRITT_ZEILE_MAX  160                           /* l2 (fehler_halten/Fortschritt) ist char[160] */

static void text_block(SDL_Renderer *r, int W, int rand, const char *s, int fs_max, int y, int nach_oben, Uint8 c)
{
    const int glyph = 6;                                     /* Vorschub je Zeichen in Skalierung 1 (touch_overlay_pc.c:483) */
    int avail = W - 2 * rand;
    int len = (int)strlen(s);
    if (len <= 0 || avail < glyph) return;
    int fs = avail / (glyph * len);
    if (fs > fs_max) fs = fs_max;
    if (fs >= 1) {
        re15_touch_pc_text(r, (W - len * glyph * fs) / 2, y, fs, s, c, c, c, 255);
        return;
    }
    int max_z = avail / glyph;                               /* Zeichen je Zeile bei Skalierung 1 */
    char zeile[FORTSCHRITT_ZEILEN_MAX][FORTSCHRITT_ZEILE_MAX + 1];
    int n = 0;
    const char *p = s;
    while (*p && n < FORTSCHRITT_ZEILEN_MAX) {
        while (*p == ' ') p++;
        int rest = (int)strlen(p);
        if (rest == 0) break;
        int k = rest <= max_z ? rest : max_z;
        if (k < rest) {                                      /* am letzten Leerzeichen innerhalb der Zeile umbrechen */
            int b = k;
            while (b > 0 && p[b] != ' ') b--;
            if (b > 0) k = b;
        }
        if (k > FORTSCHRITT_ZEILE_MAX) k = FORTSCHRITT_ZEILE_MAX;
        memcpy(zeile[n], p, (size_t)k);
        zeile[n][k] = '\0';
        n++;
        p += k;
    }
    for (int i = 0; i < n; i++) {
        int yy = nach_oben ? y - (n - 1 - i) * 9 : y + i * 9;
        re15_touch_pc_text(r, (W - (int)strlen(zeile[i]) * glyph) / 2, yy, 1, zeile[i], c, c, c, 255);
    }
}

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
    int rand = (int)(0.4f * (float)u);                       /* Runde 35 Spur N: Rand wie touch_overlay_pc.c:175 */
    text_block(r, W, rand, l1, fs, H / 2 - 2 * u, 1, 230);

    int bx = W / 6, bw = W * 2 / 3, by = H / 2 - u / 2, bh = u;
    SDL_SetRenderDrawBlendMode(r, SDL_BLENDMODE_NONE);
    SDL_SetRenderDrawColor(r, 110, 110, 110, 255);
    { SDL_Rect o[4] = { { bx, by, bw, 3 }, { bx, by + bh - 3, bw, 3 }, { bx, by, 3, bh }, { bx + bw - 3, by, 3, bh } };
      for (int i = 0; i < 4; i++) SDL_RenderFillRect(r, &o[i]); }
    if (frac < 0) frac = 0;
    if (frac > 1) frac = 1;
    SDL_SetRenderDrawColor(r, 200, 40, 40, 255);
    { SDL_Rect f = { bx + 5, by + 5, (int)((double)(bw - 10) * frac), bh - 10 };
      if (f.w > 0) SDL_RenderFillRect(r, &f); }

    int fs2 = (fs > 2) ? fs - 1 : fs;
    text_block(r, W, rand, l2, fs2, by + bh + u / 2, 0, 200);

    SDL_RenderPresent(r);
    SDL_RenderSetLogicalSize(r, lw, lh);
    SDL_PumpEvents();
}

/* Fail closed (Nachbesserung R4-1, Gegenpruefung U4): ohne vollstaendigen Asset-Baum startet das Spiel NICHT. Bis dahin
 * stand jede Fehlermeldung ~3 s (90 x 33 ms) auf dem Schirm, dann lief main() weiter - bei einem Update mit dem ALTEN
 * bzw. einem gemischten Baum (das Symptom von N1a, nur mit Hinweis). re15_android_bootstrap_assets() hat keine
 * Rueckgabe, und main.c (PC-Code) bleibt unberuehrt: deshalb haelt diese Funktion die Meldung stehen, bis die App
 * geschlossen wird (SDL_QUIT), und beendet dann den Prozess. Die Ursache steht in debug.log und logcat (Tag re15). */
static _Noreturn void fehler_halten(SDL_Renderer *r, const char *text)
{
    fprintf(stderr, "[android] ABBRUCH: %s - das Spiel startet nicht (die Meldung bleibt, bis die App geschlossen wird)\n",
            text);
    fflush(stderr);
    LOGE("[android] ABBRUCH: %s - das Spiel startet nicht (Meldung bleibt stehen)", text);
    for (;;) {
        draw_progress(r, "RE1.5 PORT - FEHLER", text, 0);
        SDL_Event ev;
        while (SDL_PollEvent(&ev)) {
            if (ev.type == SDL_QUIT || ev.type == SDL_APP_TERMINATING) {
                LOGI("[android] App geschlossen - Prozess endet ohne Spielstart");
                fflush(stderr);
                exit(1);
            }
        }
        SDL_Delay(100);
    }
}

/* re15_abgleich_waisen meldet je Datei (Nachbesserung R4-1, U2/E1). Nicht loeschbar = Warnung, kein Abbruch: eine Waise
 * steht in keiner Liste, die Engine oeffnet sie nie - sie kostet nur Platz. */
static void waise_melden(void *ctx, const char *rel, int ok)
{
    (void)ctx;
    if (ok) {
        fprintf(stderr, "[android] Waise entfernt (steht in keiner Liste): %s\n", rel);
        LOGI("[android] Waise entfernt (steht in keiner Liste): %s", rel);
    } else {
        int err = errno;
        fprintf(stderr, "[android] WARNUNG: Waise nicht loeschbar: %s (%s)\n", rel, strerror(err));
        LOGE("[android] Waise nicht loeschbar: %s (%s)", rel, strerror(err));
    }
}

/* Runde 35 Spur N (F-Y4/H8): re15_abgleich_weg_frei meldet jeden geraeumten Konflikt (Datei <-> Ordner gleichen Namens).
 * ctx zaehlt die geraeumten. Nicht raeumbar = Fehler des Laufs (fail closed wie jeder Dateifehler). */
static void konflikt_melden(void *ctx, const char *rel, int art, long n_dateien, int ok)
{
    static const char *const was[] = { "?", "eine Datei, wo ein Ordner hin muss",
                                       "ein Ordner, wo die Datei hin muss",
                                       "ein Ordner auf dem Namen der Zwischendatei" };
    const char *w = (art >= 1 && art <= 3) ? was[art] : was[0];
    if (ok) {
        char inhalt[48] = "";
        if (art != RE15_KONFLIKT_ELTER_DATEI) snprintf(inhalt, sizeof inhalt, " mit %ld Dateien darin", n_dateien);
        if (ctx) (*(long *)ctx)++;
        fprintf(stderr, "[android] Konflikt geraeumt: %s war %s%s - entfernt\n", rel, w, inhalt);
        LOGI("[android] Konflikt geraeumt: %s war %s%s - entfernt", rel, w, inhalt);
    } else {
        int err = errno;
        fprintf(stderr, "[android] FEHLER: Konflikt nicht raeumbar: %s ist %s (%s)\n", rel, w, strerror(err));
        LOGE("[android] Konflikt nicht raeumbar: %s ist %s (%s)", rel, w, strerror(err));
    }
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
        fehler_halten(r, "FEHLER: KEIN ZUGRIFF AUF DIE APK-ASSETS");
    }

    size_t mlen = 0;
    char *man = apk_datei_lesen(am, LISTE_APK, RE15_ABGLEICH_LISTE_MAX, &mlen);
    if (!man) {
        fprintf(stderr, "[android] FEHLER: assets/%s fehlt in der APK (oder > 64 MiB) - keine Assets.\n", LISTE_APK);
        LOGE("[android] assets/%s fehlt in der APK (oder > 64 MiB)", LISTE_APK);
        fehler_halten(r, "FEHLER: ASSET-LISTE FEHLT IN DER APK");
    }
    re15_abgleich_liste_t neu;
    int rc = re15_abgleich_lesen(&neu, man, mlen, fehler, sizeof fehler);
    if (rc != 0) {
        fprintf(stderr, "[android] FEHLER: assets/%s ungueltig (%s) - es wird NICHTS entpackt.\n", LISTE_APK, fehler);
        LOGE("[android] assets/%s ungueltig: %s", LISTE_APK, fehler);
        free(man);
        fehler_halten(r, rc == RE15_ABGLEICH_ALTES_FORMAT ? "FEHLER: ASSET-LISTE IM ALTEN FORMAT"
                                                          : "FEHLER: ASSET-LISTE DER APK UNGUELTIG");
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
        re15_abgleich_frei(&alt); re15_abgleich_frei(&neu); free(man);
        fehler_halten(r, "FEHLER: KEIN SPEICHER");
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

    /* ---- Lauf, der etwas aendern kann: "zuletzt entpackt" ZUERST weg (haltbar). Nachbesserung R4-1 (Gegenpruefung U1):
     *      scheitert das Loeschen (ausser "gibt es nicht"), laeuft KEIN Lauf - sonst koennte nach einem Abbruch die ALTE
     *      Liste stehen bleiben und mehr versprechen, als auf der Platte liegt. */
    const char *modus = gleich ? "Groessen-Nachlauf" : alt_ok ? "Update" : v1_marker ? "Uebergang v0.8.19" : "ohne Liste";
    if (unlink(pf_liste) != 0 && errno != ENOENT) {
        int err = errno;
        fprintf(stderr, "[android] FEHLER: %s nicht loeschbar (%s) - ohne das waere ein Abbruch nicht erkennbar\n",
                pf_liste, strerror(err));
        LOGE("[android] %s nicht loeschbar: %s", pf_liste, strerror(err));
        re15_abgleich_plan_frei(&plan); re15_abgleich_frei(&alt); re15_abgleich_frei(&neu); free(man);
        fehler_halten(r, "FEHLER: ALTE ASSET-LISTE NICHT LOESCHBAR - SIEHE DEBUG.LOG");
    }
    unlink(pf_liste_neu);
    ordner_haltbar(s_root);

    /* Waisen (Nachbesserung R4-1, Gegenpruefung U2 / echter Lauf E1): ohne gueltige "zuletzt entpackt"-Liste
     * (Erstinstallation, Uebergang v0.8.19, abgebrochener Lauf) weiss niemand, welche Dateien eines frueheren Stands
     * noch liegen - geloescht wurde bis dahin nur, was eine ALTE Liste nannte, und .neu-Reste nur fuer Pfade der NEUEN.
     * Abbruch + Update, das den abgebrochenen Pfad streicht, liess die halbe .neu bzw. die fertige Datei fuer immer
     * liegen. Jetzt: shared_assets/ und synchro/ (gehoeren allein dem Entpacker; die Engine schreibt dort nichts)
     * durchgehen und alles, was nicht in der neuen Liste steht, VOR dem Entpacken loeschen. */
    long n_waisen = 0, n_waisen_fehler = 0;
    if (!gleich && !alt_ok) {
        static const char *const baeume[] = { "shared_assets", "synchro" };
        n_waisen = re15_abgleich_waisen(s_root, baeume, sizeof baeume / sizeof *baeume, &neu, waise_melden, NULL,
                                        &n_waisen_fehler);
    }
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
    long n_konflikt = 0, n_weg_fehler = 0;                /* Runde 35 Spur N */
    long long b_kopiert = 0, b_summe = 0, done_b = 0;
    Uint32 ms_summe = 0, ms_kopie = 0, last_draw = 0;
    char l2[160];
    if (!buf) n_fehler++;

    /* Pfade, die nur in der alten Liste stehen, ZUERST loeschen (Regeln wie jede Listenzeile: nie ausserhalb
     * s_root). Erst danach entpacken: der App-Speicher ist case-insensitiv (Dossier N1, 1.3) - stuende "a/X.BIN"
     * nur in der alten und "a/x.bin" in der neuen Liste, loeschte ein unlink NACH dem Entpacken die frische
     * Datei (Dossier N1, 2.5). */
    /* Runde 35 Spur N: danach leer gewordene Ordner weg (H8: Ordner q/ mit c wird Datei q - der leere Ordner q/ liess
     * das rename scheitern), und ein Fehler wird nicht mehr verschluckt (F-Y6): liegt dort ein Ordner, raeumt ihn
     * re15_abgleich_weg_frei; sonst WARNUNG mit Grund (wie re15_abgleich_waisen - die Datei steht in keiner Liste). */
    for (size_t j = 0; j < plan.n_weg; j++) {
        if (pfad_bauen(dst, sizeof dst, plan.weg[j], NULL) != 0) continue;
        int ok = unlink(dst) == 0;
        if (!ok && errno != ENOENT) {
            int err = errno;
            ok = re15_abgleich_weg_frei(s_root, plan.weg[j], konflikt_melden, &n_konflikt) == 0 && access(dst, F_OK) != 0;
            if (!ok) {
                n_weg_fehler++;
                fprintf(stderr, "[android] WARNUNG: nicht mehr gelistet, aber nicht loeschbar: %s (%s)\n", plan.weg[j],
                        strerror(err));
                LOGE("[android] nicht mehr gelistet, aber nicht loeschbar: %s (%s)", plan.weg[j], strerror(err));
            }
        }
        if (ok) {
            n_weg++;
            re15_abgleich_leere_eltern(s_root, plan.weg[j]);
            fprintf(stderr, "[android] entfernt (nicht mehr in der Liste): %s\n", plan.weg[j]);
            LOGI("[android] entfernt (nicht mehr in der Liste): %s", plan.weg[j]);
        }
    }

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
            /* Runde 35 Spur N (F-Y4/H8): Datei <-> Ordner gleichen Namens auf dem Weg im SELBEN Start raeumen */
            if (re15_abgleich_weg_frei(s_root, e->pfad, konflikt_melden, &n_konflikt) != 0) n_fehler++;
            else if (entpacken(am, e, dst, tmp, buf, BUF) == 0) { n_kopiert++; b_kopiert += e->groesse; }
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
                    "(%lld B, %u ms, %ld abweichend), %ld entfernt, %ld Waisen entfernt (%ld nicht loeschbar), "
                    "%ld .neu-Reste, %ld Konflikte geraeumt, %ld nicht loeschbar, %ld Fehler, %u ms\n",
            modus, neu.n, n_kopiert, b_kopiert, (unsigned)ms_kopie, n_summe, b_summe, (unsigned)ms_summe, n_summe_neu,
            n_weg, n_waisen, n_waisen_fehler, n_reste, n_konflikt, n_weg_fehler, n_fehler, (unsigned)ms);
    LOGI("[android] Entpacken fertig (%s): %zu geprueft, %ld kopiert (%lld B, %u ms), %ld per SHA-256 geprueft "
         "(%lld B, %u ms, %ld abweichend), %ld entfernt, %ld Waisen entfernt (%ld nicht loeschbar), "
         "%ld .neu-Reste, %ld Konflikte geraeumt, %ld nicht loeschbar, %ld Fehler, %u ms",
         modus, neu.n, n_kopiert, b_kopiert, (unsigned)ms_kopie, n_summe, b_summe, (unsigned)ms_summe, n_summe_neu,
         n_weg, n_waisen, n_waisen_fehler, n_reste, n_konflikt, n_weg_fehler, n_fehler, (unsigned)ms);
    re15_abgleich_plan_frei(&plan);
    re15_abgleich_frei(&alt);
    re15_abgleich_frei(&neu);
    free(man);
    if (n_fehler) {                                       /* fail closed (R4-1, U4): kein Spielstart mit Loch im Baum */
        snprintf(l2, sizeof l2, "%ld DATEIEN KONNTEN NICHT ENTPACKT WERDEN - SIEHE DEBUG.LOG", n_fehler);
        fehler_halten(r, l2);
    }
}
