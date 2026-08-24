/* Siehe asset_root_pc.h fuer das WARUM und die Prioritaetenkette. */

/* MUSS vor JEDEM System-Header stehen. Die Targets bauen mit -std=c11 (strikt); glibc blendet
 * darunter alles aus, was nicht ISO-C ist — darunter readlink() aus <unistd.h>. Ohne dieses
 * Makro brach der Linux-/Steam-Deck-Build hart ab (gemessen 2026-08-24 im docker_linux_build:
 * "error: 'S_IFMT' undeclared" + "implicit declaration of readlink"), waehrend mingw beides
 * ohnehin sichtbar macht — deshalb fiel es am Windows-Cross-Build NICHT auf. */
#if !defined(_WIN32) && !defined(_DEFAULT_SOURCE)
#  define _DEFAULT_SOURCE 1
#endif

#include "asset_root_pc.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#ifdef _WIN32
#  include <windows.h>
#  include <direct.h>
#  define re15_getcwd _getcwd
#else
#  include <unistd.h>
#  define re15_getcwd getcwd
#endif

#define RR_MAX   24
#define RR_PATH  512

static char s_exe_dir[RR_PATH];
static int  s_exe_done = 0;

static char s_cd[RR_MAX][RR_PATH]; static int s_cd_n = 0;
static char s_sh[RR_MAX][RR_PATH]; static int s_sh_n = 0;
static char s_ba[RR_MAX][RR_PATH]; static int s_ba_n = 0;
static int  s_init = 0;

/* ---------------------------------------------------------------------------- Pfad-Werkzeug */

/* Kopieren, '\' -> '/', abschliessende Trenner strippen. */
static void rr_norm(char *dst, size_t cap, const char *src)
{
    size_t n = 0;
    dst[0] = '\0';
    if (!src) return;
    while (src[n] && n + 1 < cap) {
        char c = src[n];
        dst[n] = (c == '\\') ? '/' : c;
        n++;
    }
    dst[n] = '\0';
    while (n > 1 && dst[n-1] == '/') {
        /* "C:/" und "/" behalten ihren Trenner nicht — beide Formen sind als Praefix ok */
        dst[--n] = '\0';
    }
}

static int rr_dir_exists(const char *p)
{
    struct stat st;
    if (!p || !p[0]) return 0;
    if (stat(p, &st) != 0) return 0;
    return S_ISDIR(st.st_mode) ? 1 : 0;   /* S_ISDIR ist ISO/POSIX und auf beiden Targets da */
}

static void rr_add(char (*arr)[RR_PATH], int *n, const char *p)
{
    char t[RR_PATH];
    if (!p || !p[0] || *n >= RR_MAX) return;
    rr_norm(t, sizeof t, p);
    if (!t[0]) return;
    for (int i = 0; i < *n; i++) if (strcmp(arr[i], t) == 0) return;
    snprintf(arr[*n], RR_PATH, "%s", t);
    (*n)++;
}

/* dir + bis zu `levels` Elternverzeichnisse eintragen (durch Abschneiden am letzten '/',
 * daher bleiben die Pfade sauber und lassen sich zuverlaessig deduplizieren). */
static void rr_add_ancestors(char (*arr)[RR_PATH], int *n, const char *dir, int levels)
{
    char t[RR_PATH];
    rr_norm(t, sizeof t, dir);
    for (int L = 0; L <= levels && t[0]; L++) {
        rr_add(arr, n, t);
        char *slash = strrchr(t, '/');
        if (!slash || slash == t) break;                 /* "/x" oder kein Trenner -> Ende  */
        *slash = '\0';
        if (t[1] == ':' && t[2] == '\0') break;          /* "C:" -> Laufwerkswurzel, Ende   */
    }
}

/* -------------------------------------------------------------------------- exe-Verzeichnis */

const char *re15_pc_exe_dir(void)
{
    if (s_exe_done) return s_exe_dir;
    s_exe_done = 1;
    s_exe_dir[0] = '\0';
    {
        char buf[RR_PATH];
        buf[0] = '\0';
#ifdef _WIN32
        /* ⛔ NICHT GetModuleFileNameA (gemessen 2026-08-24, Skeptiker-Befund): liegt das Paket
         * in einem Ordner, dessen Name in der ANSI-Codepage der Maschine NICHT darstellbar ist
         * (kyrillischer Ordner auf CP1252, oder umgekehrt "Täst" auf einer CJK-Maschine),
         * liefert die A-Variante Fragezeichen. Der exe-Anker faellt dann KOMPLETT weg und wir
         * sind zurueck beim 0.3.19-Fehler: keine Schrift, keine Untertitel — still.
         * Deshalb: breit lesen, verlustfrei umwandeln; geht das nicht, den 8.3-Kurznamen
         * nehmen (reines ASCII); geht auch das nicht, LAUT scheitern statt stumm. */
        wchar_t wbuf[RR_PATH];
        const int WN = (int)(sizeof wbuf / sizeof wbuf[0]);
        DWORD wgot = GetModuleFileNameW(NULL, wbuf, (DWORD)WN);
        buf[0] = '\0';
        if (wgot > 0 && wgot < (DWORD)WN) {
            /* Bei ACP=UTF-8 (Windows 10+ Opt-in) MUSS lpUsedDefaultChar NULL sein, sonst
             * scheitert die Umwandlung mit ERROR_INVALID_PARAMETER — und verlustbehaftet
             * kann sie dort ohnehin nicht sein. */
            int acp_utf8 = (GetACP() == CP_UTF8);
            BOOL lossy = FALSE;
            int n = WideCharToMultiByte(CP_ACP, acp_utf8 ? 0 : WC_NO_BEST_FIT_CHARS,
                                        wbuf, -1, buf, (int)sizeof buf,
                                        NULL, acp_utf8 ? NULL : &lossy);
            if (n <= 0 || (!acp_utf8 && lossy)) {
                wchar_t wshort[RR_PATH];
                DWORD sg = GetShortPathNameW(wbuf, wshort, (DWORD)WN);
                buf[0] = '\0';
                if (sg > 0 && sg < (DWORD)WN) {
                    lossy = FALSE;
                    n = WideCharToMultiByte(CP_ACP, acp_utf8 ? 0 : WC_NO_BEST_FIT_CHARS,
                                            wshort, -1, buf, (int)sizeof buf,
                                            NULL, acp_utf8 ? NULL : &lossy);
                    if (n <= 0 || (!acp_utf8 && lossy)) buf[0] = '\0';
                }
                if (!buf[0])
                    fprintf(stderr,
                            "[asset] WARNUNG: der Pfad dieser exe laesst sich in der ANSI-Codepage "
                            "dieses Systems nicht darstellen und 8.3-Kurznamen sind abgeschaltet. "
                            "Die Assets neben der exe koennen deshalb nicht gefunden werden — bitte "
                            "den Ordner ohne Sonderzeichen benennen.\n");
            }
        }
#else
        ssize_t got = readlink("/proc/self/exe", buf, sizeof buf - 1);
        if (got > 0) buf[got] = '\0'; else buf[0] = '\0';
#endif
        if (buf[0]) {
            char t[RR_PATH];
            rr_norm(t, sizeof t, buf);
            char *slash = strrchr(t, '/');
            if (slash) { *slash = '\0'; snprintf(s_exe_dir, sizeof s_exe_dir, "%s", t); }
        }
    }
    return s_exe_dir;
}

/* -------------------------------------------------------------------------- Listen aufbauen */

static void rr_init(void)
{
    if (s_init) return;
    s_init = 1;

    /* (a) Basis-Kandidaten: exe-Verzeichnis + Vorfahren, dann cwd + Vorfahren, dann die
     *     Vorfahren des einkompilierten Defaults (letzterer ZULETZT — ein Paket soll seine
     *     eigenen Assets nehmen, nicht heimlich ein Repo auf der Entwickler-Maschine). */
    static char cand[RR_MAX][RR_PATH]; int cn = 0;
    rr_add_ancestors(cand, &cn, re15_pc_exe_dir(), 4);
    {
        char cwd[RR_PATH];
        if (re15_getcwd(cwd, (int)sizeof cwd)) rr_add_ancestors(cand, &cn, cwd, 4);
    }
#ifdef RE15_ASSET_ROOT_DEFAULT
    rr_add_ancestors(cand, &cn, RE15_ASSET_ROOT_DEFAULT, 4);
#endif
#ifdef RE15_CD_ROOT_DEFAULT
    rr_add_ancestors(cand, &cn, RE15_CD_ROOT_DEFAULT, 4);
#endif

    /* (b) filtern: nur Verzeichnisse, die wirklich einen Asset-Baum tragen. Haelt die Liste
     *     kurz (typisch 1-3), damit ein Fehlschlag nicht Dutzende fopen kostet. */
    for (int i = 0; i < cn; i++) {
        char p[RR_PATH];
        snprintf(p, sizeof p, "%s/shared_assets", cand[i]);
        if (rr_dir_exists(p)) { rr_add(s_ba, &s_ba_n, cand[i]); continue; }
        snprintf(p, sizeof p, "%s/synchro", cand[i]);
        if (rr_dir_exists(p)) rr_add(s_ba, &s_ba_n, cand[i]);
    }

    /* (c) CD-Wurzeln: env zuerst, dann <basis>/shared_assets/PSX, dann die Compile-Defaults. */
    {
        const char *e;
        e = getenv("RE15_CD_ROOT");    if (e && e[0]) rr_add(s_cd, &s_cd_n, e);
        e = getenv("RE15_ASSET_ROOT"); if (e && e[0]) rr_add(s_cd, &s_cd_n, e);
    }
    for (int i = 0; i < s_ba_n; i++) {
        char p[RR_PATH];
        snprintf(p, sizeof p, "%s/shared_assets/PSX", s_ba[i]);
        if (rr_dir_exists(p)) rr_add(s_cd, &s_cd_n, p);
    }
#ifdef RE15_CD_ROOT_DEFAULT
    rr_add(s_cd, &s_cd_n, RE15_CD_ROOT_DEFAULT);
#endif
#ifdef RE15_ASSET_ROOT_DEFAULT
    rr_add(s_cd, &s_cd_n, RE15_ASSET_ROOT_DEFAULT);
#endif

    /* (d) Shared-Wurzeln = Eltern der CD-Wurzeln (dort liegen RE2/ und extracted_fx/).
     *     Die env-Varianten behalten dadurch automatisch ihre Vorrangstellung; genau das war
     *     bisher der "<cdroot>/../<rel>"-Geschwisterpfad in pc_read_shared(). */
    for (int i = 0; i < s_cd_n; i++) {
        char t[RR_PATH];
        rr_norm(t, sizeof t, s_cd[i]);
        char *slash = strrchr(t, '/');
        if (slash && slash != t) { *slash = '\0'; rr_add(s_sh, &s_sh_n, t); }
    }
    for (int i = 0; i < s_ba_n; i++) {
        char p[RR_PATH];
        snprintf(p, sizeof p, "%s/shared_assets", s_ba[i]);
        if (rr_dir_exists(p)) rr_add(s_sh, &s_sh_n, p);
    }
}

const char *re15_pc_cd_root(int i)     { rr_init(); return (i >= 0 && i < s_cd_n) ? s_cd[i] : NULL; }
const char *re15_pc_shared_root(int i) { rr_init(); return (i >= 0 && i < s_sh_n) ? s_sh[i] : NULL; }
const char *re15_pc_base_root(int i)   { rr_init(); return (i >= 0 && i < s_ba_n) ? s_ba[i] : NULL; }

int re15_pc_cd_path(int i, const char *rel, char *buf, unsigned bufsz)
{
    const char *r = re15_pc_cd_root(i);
    if (!r || !buf || bufsz == 0) return 0;
    snprintf(buf, bufsz, "%s/%s", r, rel ? rel : "");
    return 1;
}

int re15_pc_base_path(int i, const char *rel, char *buf, unsigned bufsz)
{
    const char *r = re15_pc_base_root(i);
    if (!r || !buf || bufsz == 0) return 0;
    snprintf(buf, bufsz, "%s/%s", r, rel ? rel : "");
    return 1;
}

/* ------------------------------------------------------------------------------ Datei lesen */

/* BEWUSST ein eigenes fopen und NICHT re15_asset_read_file(): dieses Modul liegt UNTER dem
 * Asset-Leser (der ruft hier herein, wenn sein literaler Versuch fehlschlaegt). Ein Aufruf
 * zurueck nach oben waere eine Rekursion. Deshalb hier auch keine Fehlermeldung — ein Miss
 * ist im Wurzel-Sweep der Normalfall. */
static uint8_t *rr_read(const char *path, int *size)
{
    FILE *f = fopen(path, "rb");
    if (!f) return NULL;
    fseek(f, 0, SEEK_END);
    long sz = ftell(f);
    fseek(f, 0, SEEK_SET);
    if (sz <= 0) { fclose(f); return NULL; }
    uint8_t *b = (uint8_t *)malloc((size_t)sz);
    if (!b) { fclose(f); return NULL; }
    if (fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); fclose(f); return NULL; }
    fclose(f);
    if (size) *size = (int)sz;
    return b;
}

static uint8_t *rr_read_list(const char *(*root)(int), const char *rel, int *size)
{
    char p[RR_PATH];
    if (!rel || !rel[0]) return NULL;
    for (int i = 0;; i++) {
        const char *r = root(i);
        if (!r) break;
        snprintf(p, sizeof p, "%s/%s", r, rel);
        uint8_t *b = rr_read(p, size);
        if (b) return b;
    }
    return NULL;
}

uint8_t *re15_pc_read_cd(const char *rel, int *size)     { rr_init(); return rr_read_list(re15_pc_cd_root, rel, size); }
uint8_t *re15_pc_read_shared(const char *rel, int *size) { rr_init(); return rr_read_list(re15_pc_shared_root, rel, size); }
uint8_t *re15_pc_read_base(const char *rel, int *size)   { rr_init(); return rr_read_list(re15_pc_base_root, rel, size); }

uint8_t *re15_pc_read_re2(const char *rel, int *size)
{
    char p[RR_PATH];
    uint8_t *b = NULL;
    const char *envroot;
    rr_init();
    if (!rel || !rel[0]) return NULL;
    envroot = getenv("RE15_RE2_ASSET_ROOT");
    if (envroot && envroot[0]) {
        snprintf(p, sizeof p, "%s/%s", envroot, rel);
        b = rr_read(p, size);
        if (b) return b;
    }
    for (int i = 0;; i++) {
        const char *r = re15_pc_shared_root(i);
        if (!r) break;
        snprintf(p, sizeof p, "%s/RE2/%s", r, rel);
        b = rr_read(p, size);
        if (b) return b;
    }
    return NULL;
}

uint8_t *re15_pc_read_any(const char *rel, int *size)
{
    uint8_t *b;
    if (!rel || !rel[0]) return NULL;
    rr_init();
    b = rr_read_list(re15_pc_cd_root, rel, size);      if (b) return b;   /* "DATA/TEX.TIM"           */
    b = rr_read_list(re15_pc_shared_root, rel, size);  if (b) return b;   /* "extracted_fx/x.tim"     */
    b = rr_read_list(re15_pc_base_root, rel, size);    if (b) return b;   /* "synchro/…", "shared_assets/…" */
    return NULL;
}

void re15_pc_asset_roots_report(void)
{
    static int done = 0;
    if (done) return;
    done = 1;
    rr_init();
    fprintf(stderr, "[asset] exe-dir: %s\n", re15_pc_exe_dir()[0] ? re15_pc_exe_dir() : "(unbekannt)");
    if (s_cd_n == 0) {
        fprintf(stderr, "[asset] WARNUNG: keine Asset-Wurzel gefunden — es wird nichts laden.\n");
    } else {
        fprintf(stderr, "[asset] cd-root: %s%s\n", s_cd[0], (s_cd_n > 1) ? " (+Fallbacks)" : "");
    }
    if (getenv("RE15_ASSET_DEBUG")) {
        for (int i = 0; i < s_cd_n; i++) fprintf(stderr, "[asset]   cd[%d]     = %s\n", i, s_cd[i]);
        for (int i = 0; i < s_sh_n; i++) fprintf(stderr, "[asset]   shared[%d] = %s\n", i, s_sh[i]);
        for (int i = 0; i < s_ba_n; i++) fprintf(stderr, "[asset]   base[%d]   = %s\n", i, s_ba[i]);
    }
}
