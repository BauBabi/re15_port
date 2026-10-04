/* =============================================================================================
 * Runde 35 Spur N "android" (2026-10-03): Unit-Test der neuen Teile von
 * re15_port/platform/android/jni/asset_abgleich.c - Leser-Regeln R1/R2 und das Raeumen von Datei<->Ordner-Konflikten
 * (re15_abgleich_weg_frei, re15_abgleich_leere_eltern) auf einem echten Temp-Ordner.
 * Dossier: analysis/befunde_runde35/N_android.md Punkt 2. PORT-WAHL, kein Originalverhalten (die PSX las von CD).
 * Der Ablauf im ECHTEN android_glue.c (Update im selben Start) wird im Pruefstand gemessen
 * (r35_android/test_r35_android_entpacker.cmake, ctest unit_r35_android_konflikt).
 * ============================================================================================= */
#if !defined(_WIN32) && !defined(_DEFAULT_SOURCE)
#  define _DEFAULT_SOURCE 1
#endif
#include "asset_abgleich.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#if defined(_WIN32)
#  include <direct.h>
#  include <io.h>
#  define ordner_neu(p) _mkdir(p)
#else
#  include <unistd.h>
#  define ordner_neu(p) mkdir((p), 0700)
#endif

static int s_pruef = 0, s_fehl = 0;
#define PRUEFE(bed, ...) do {                                                   \
        s_pruef++;                                                              \
        if (!(bed)) {                                                           \
            s_fehl++;                                                           \
            printf("FEHLER %s:%d: %s -- ", __FILE__, __LINE__, #bed);            \
            printf(__VA_ARGS__);                                                \
            printf("\n");                                                       \
        }                                                                       \
    } while (0)

#define SA "0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef"

/* ------------------------------------------------------------------------------ Leser: R1 + R2 (+ F-Y1-Proben) */
static void erwarte(const char *titel, const char *text, int soll_rc, const char *soll_grund)
{
    re15_abgleich_liste_t l;
    char f[256];
    int rc = re15_abgleich_lesen(&l, text, strlen(text), f, sizeof f);
    PRUEFE(rc == soll_rc, "%s: rc %d, soll %d (%s)", titel, rc, soll_rc, f);
    if (soll_grund) PRUEFE(strstr(f, soll_grund) != NULL, "%s: Grund '%s', soll '%s'", titel, f, soll_grund);
    re15_abgleich_frei(&l);
}

static void test_leser(void)
{
    const int U = RE15_ABGLEICH_UNGUELTIG;
    /* R1: kein Segment endet auf .neu (Gross/klein egal) - dieselben Proben wie apk_asset_gate.py _MANIFEST_PROBEN */
    erwarte("R1 Ordner X.neu", "# re15 assets v2 1 5\n5\t" SA "\ta/X.neu/B\n", U, "unzulaessiger Pfad");
    erwarte("R1 Ordner X.NEU", "# re15 assets v2 1 5\n5\t" SA "\ta/X.NEU/B\n", U, "unzulaessiger Pfad");
    erwarte("R1 erstes Segment .Neu", "# re15 assets v2 1 5\n5\t" SA "\tx.Neu/b\n", U, "unzulaessiger Pfad");
    erwarte("R1 Segment nur .neu", "# re15 assets v2 1 5\n5\t" SA "\ta/.neu/b\n", U, "unzulaessiger Pfad");
    erwarte("R1 x.neux/b erlaubt", "# re15 assets v2 1 5\n5\t" SA "\ta/x.neux/b\n", 0, NULL);
    erwarte("R1 neu/b erlaubt", "# re15 assets v2 1 5\n5\t" SA "\ta/neu/b\n", 0, NULL);
    PRUEFE(re15_abgleich_pfad_ok("a/b.neu/c", 9) == 0, "pfad_ok a/b.neu/c");
    PRUEFE(re15_abgleich_pfad_ok("a/bneu/c", 8) == 1, "pfad_ok a/bneu/c");
    PRUEFE(re15_abgleich_pfad_ok("a/.neux/c", 9) == 1, "pfad_ok a/.neux/c");
    /* R2: kein Pfad ist zugleich Ordner eines anderen */
    erwarte("R2 Datei + Ordner", "# re15 assets v2 2 10\n5\t" SA "\ta/q\n5\t" SA "\ta/q/c\n", U, "Datei und Ordner gleichen Namens");
    erwarte("R2 Gross/klein", "# re15 assets v2 2 10\n5\t" SA "\ta/Q\n5\t" SA "\tA/q/c\n", U, "Datei und Ordner gleichen Namens");
    erwarte("R2 tief", "# re15 assets v2 2 10\n5\t" SA "\ta/q/c/d/e\n5\t" SA "\ta/q/c\n", U, "Datei und Ordner gleichen Namens");
    erwarte("R2 erstes Segment", "# re15 assets v2 2 10\n5\t" SA "\tsynchro/x\n5\t" SA "\tSYNCHRO\n", U, NULL);
    erwarte("R2 a/q + a/qc erlaubt", "# re15 assets v2 2 10\n5\t" SA "\ta/q\n5\t" SA "\ta/qc/c\n", 0, NULL);
    erwarte("R2 a/q + a/q-x/c erlaubt", "# re15 assets v2 3 15\n5\t" SA "\ta/q\n5\t" SA "\ta/q-x/c\n5\t" SA "\ta/q.b/c\n", 0, NULL);
    /* F-Y1 (Gegenpruefung R4-2: Gate-Mutanten D06/D08/D13/D15 ueberlebten, weil diese Proben fehlten) - der
     * Geraete-Leser lehnt jede ab; das Gate muss es auch (apk_asset_gate.py _MANIFEST_PROBEN, Runde 35) */
    erwarte("F-Y1 Dublette Z/z", "# re15 assets v2 2 10\n5\t" SA "\ta/Z\n5\t" SA "\ta/z\n", U, "Gross/klein");
    erwarte("F-Y1 Kopf-Anzahl 19 Ziffern", "# re15 assets v2 0000000000000000001 5\n5\t" SA "\ta/b\n", U, "Kopfzeile");
    erwarte("F-Y1 CR vor Kopfzeile", "\r# re15 assets v2 1 5\n5\t" SA "\ta/b\n", U, "Kopfzeile");
    erwarte("F-Y1 CR vor Datenzeile", "# re15 assets v2 1 5\n\r5\t" SA "\ta/b\n", U, "Groesse");
    erwarte("F-Y1 Zeilenende LF-CR", "# re15 assets v2 1 5\n\r5\t" SA "\ta/b\n\r", U, NULL);
}

/* ------------------------------------------------------------------------------ weg_frei / leere_eltern */
static void datei_neu(const char *p, const char *inhalt)
{
    FILE *f = fopen(p, "wb");
    if (f) { fputs(inhalt, f); fclose(f); }
    PRUEFE(f != NULL, "anlegen %s", p);
}
static int ist_ordner(const char *p) { struct stat sb; return stat(p, &sb) == 0 && S_ISDIR(sb.st_mode); }
static int ist_datei(const char *p) { struct stat sb; return stat(p, &sb) == 0 && S_ISREG(sb.st_mode); }
static int gibt_es(const char *p) { struct stat sb; return stat(p, &sb) == 0; }

typedef struct { int n, art[8]; long dateien[8]; int ok[8]; char rel[8][128]; } meldungen_t;
static void melde(void *ctx, const char *rel, int art, long n_dateien, int ok)
{
    meldungen_t *m = (meldungen_t *)ctx;
    if (m->n < 8) {
        m->art[m->n] = art;
        m->dateien[m->n] = n_dateien;
        m->ok[m->n] = ok;
        snprintf(m->rel[m->n], sizeof m->rel[m->n], "%s", rel);
        m->n++;
    }
}

#define WR "r35_android_weg"   /* im Arbeitsordner des Tests (ctest: Build-Ordner) */

static void test_weg_frei(void)
{
    meldungen_t m;
    /* Grundgeruest */
    static const char *const ordner[] = { WR, WR "/shared_assets", WR "/shared_assets/PSX", WR "/shared_assets/PSX/q",
        WR "/shared_assets/PSX/q/x", WR "/shared_assets/PSX/q/x/y", WR "/shared_assets/PSX/X.neu",
        WR "/shared_assets/PSX/X.neu/u" };
    for (size_t i = 0; i < sizeof ordner / sizeof *ordner; i++) (void)ordner_neu(ordner[i]);
    datei_neu(WR "/shared_assets/PSX/q/x/y/rest", "r");
    datei_neu(WR "/shared_assets/PSX/q/z", "z");
    datei_neu(WR "/shared_assets/PSX/X.neu/u/v", "v");
    datei_neu(WR "/shared_assets/PSX/f", "f");
    datei_neu(WR "/shared_assets/PSX/ok.bin", "ok");

    /* 1. nichts im Weg: kein Eingriff, keine Meldung */
    memset(&m, 0, sizeof m);
    PRUEFE(re15_abgleich_weg_frei(WR, "shared_assets/PSX/ok.bin", melde, &m) == 0 && m.n == 0, "ok.bin: %d Meldungen", m.n);
    PRUEFE(ist_datei(WR "/shared_assets/PSX/ok.bin"), "ok.bin bleibt (die Datei selbst ist kein Konflikt)");
    memset(&m, 0, sizeof m);
    PRUEFE(re15_abgleich_weg_frei(WR, "shared_assets/NEU/a/b.bin", melde, &m) == 0 && m.n == 0, "fehlende Ordner: kein Konflikt");

    /* 2. Ordner (mit Unterordnern) auf dem Dateinamen q -> samt Inhalt weg, Meldung ZIEL_ORDNER mit 2 Dateien */
    memset(&m, 0, sizeof m);
    PRUEFE(re15_abgleich_weg_frei(WR, "shared_assets/PSX/q", melde, &m) == 0, "q frei");
    PRUEFE(!gibt_es(WR "/shared_assets/PSX/q"), "Ordner q weg");
    PRUEFE(m.n == 1 && m.art[0] == RE15_KONFLIKT_ZIEL_ORDNER && m.dateien[0] == 2 && m.ok[0] &&
           strcmp(m.rel[0], "shared_assets/PSX/q") == 0, "Meldung q: n=%d art=%d dateien=%ld rel=%s", m.n, m.art[0],
           m.dateien[0], m.rel[0]);

    /* 3. Ordner auf dem Namen der Zwischendatei X.neu -> weg (NEU_ORDNER), X selbst gibt es nicht */
    memset(&m, 0, sizeof m);
    PRUEFE(re15_abgleich_weg_frei(WR, "shared_assets/PSX/X", melde, &m) == 0, "X frei");
    PRUEFE(!gibt_es(WR "/shared_assets/PSX/X.neu"), "Ordner X.neu weg");
    PRUEFE(m.n == 1 && m.art[0] == RE15_KONFLIKT_NEU_ORDNER && m.dateien[0] == 1, "Meldung X.neu: n=%d art=%d", m.n, m.art[0]);

    /* 4. Datei f auf einem Ordnernamen (f/g/h.bin) -> Datei weg (ELTER_DATEI), danach legt der Entpacker f/g an */
    memset(&m, 0, sizeof m);
    PRUEFE(re15_abgleich_weg_frei(WR, "shared_assets/PSX/f/g/h.bin", melde, &m) == 0, "f/g/h.bin frei");
    PRUEFE(!gibt_es(WR "/shared_assets/PSX/f"), "Datei f weg");
    PRUEFE(m.n == 1 && m.art[0] == RE15_KONFLIKT_ELTER_DATEI && strcmp(m.rel[0], "shared_assets/PSX/f") == 0,
           "Meldung f: n=%d art=%d rel=%s", m.n, m.art[0], m.rel[0]);
    PRUEFE(ist_datei(WR "/shared_assets/PSX/ok.bin"), "Nachbar ok.bin unberuehrt");

    /* 5. unzulaessige Pfade raeumen NIE (nie ausserhalb der Wurzel) */
    datei_neu(WR "/shared_assets/PSX/schutz", "s");
    static const char *const schlecht[] = { "", "../x", "shared_assets/../shared_assets/PSX/schutz", "/abs/x",
                                            "shared_assets/PSX/schutz.neu", "ohne_schraegstrich" };
    for (size_t i = 0; i < sizeof schlecht / sizeof *schlecht; i++) {
        memset(&m, 0, sizeof m);
        PRUEFE(re15_abgleich_weg_frei(WR, schlecht[i], melde, &m) == -1 && m.n == 0, "unzulaessig '%s'", schlecht[i]);
    }
    PRUEFE(ist_datei(WR "/shared_assets/PSX/schutz"), "schutz bleibt");

    /* 6. leere_eltern: nach dem Loeschen von a/b/c/d.bin werden c, b entfernt, a (erstes Segment unter der Wurzel... hier
     *    shared_assets) bleibt; ein nicht leerer Ordner stoppt */
    (void)ordner_neu(WR "/shared_assets/L");
    (void)ordner_neu(WR "/shared_assets/L/b");
    (void)ordner_neu(WR "/shared_assets/L/b/c");
    datei_neu(WR "/shared_assets/L/b/bleibt", "x");
    PRUEFE(re15_abgleich_leere_eltern(WR, "shared_assets/L/b/c/d.bin") == 1, "nur c (leer) entfernt");
    PRUEFE(!gibt_es(WR "/shared_assets/L/b/c") && ist_ordner(WR "/shared_assets/L/b"), "c weg, b (nicht leer) bleibt");
    remove(WR "/shared_assets/L/b/bleibt");
    PRUEFE(re15_abgleich_leere_eltern(WR, "shared_assets/L/b/bleibt") == 2, "b und L entfernt");
    PRUEFE(ist_ordner(WR "/shared_assets"), "der Baum shared_assets selbst bleibt");
    PRUEFE(re15_abgleich_leere_eltern(WR, "../x/y") == 0, "unzulaessig: nichts");
}

int main(void)
{
    test_leser();
    test_weg_frei();
    printf("%s: %d Pruefungen, %d Fehler\n", s_fehl ? "FEHLER" : "OK", s_pruef, s_fehl);
    return s_fehl ? 1 : 0;
}
