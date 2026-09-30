/* Gegenpruefung R4-2 (Umgehung), Y5/Y9: re15_abgleich_waisen (asset_abgleich.c HEAD, UNVERAENDERT mituebersetzt) unter
 * ECHTEM POSIX (Linux-Container, lstat/Symlinks/Rechte) - der Projekt-Unit-Test laeuft nur unter mingw, wo
 * re15_lstat = stat und RE15_IST_LINK = 0 ist: die Symlink-Regel des Loesch-Codes lief dort nie.
 * Je Fall ein frischer Ordner <basis>/<fall>/{wurzel,aussen}; geprueft wird, was danach noch existiert.
 * Ausgabe je Pruefung "ok"/"FEHLER"; Rueckgabe = Zahl der Fehler. Aufruf: u2_waisen_test <basis-ordner> */
#define _DEFAULT_SOURCE 1
#include "asset_abgleich.h"
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

static int s_fehler, s_ok;
static char s_basis[1024];

static void pruef(int bed, const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    printf("%s ", bed ? "ok    " : "FEHLER");
    vprintf(fmt, ap);
    printf("\n");
    va_end(ap);
    if (bed) s_ok++; else s_fehler++;
}

static void datei(const char *p, const char *inhalt)
{
    FILE *f = fopen(p, "wb");
    if (!f) { printf("SETUP: %s: %s\n", p, strerror(errno)); exit(99); }
    fputs(inhalt, f);
    fclose(f);
}

static void ordner(const char *p) { if (mkdir(p, 0775) != 0 && errno != EEXIST) { printf("SETUP mkdir %s: %s\n", p, strerror(errno)); exit(99); } }
static int da(const char *p) { struct stat sb; return lstat(p, &sb) == 0; }

static char s_listen_text[1 << 16];
/* Liste v2 aus Pfaden (Groesse 1, Summe 64 x 'a' - fuer re15_abgleich_waisen zaehlt nur der Pfad) */
static int liste_bauen(re15_abgleich_liste_t *l, const char *const *pfade, size_t n)
{
    size_t o = (size_t)snprintf(s_listen_text, sizeof s_listen_text, "# re15 assets v2 %zu %zu\n", n, n);
    for (size_t i = 0; i < n; i++)
        o += (size_t)snprintf(s_listen_text + o, sizeof s_listen_text - o,
                              "1\taaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa\t%s\n", pfade[i]);
    char f[256];
    int rc = re15_abgleich_lesen(l, s_listen_text, o, f, sizeof f);
    if (rc) printf("SETUP Liste: %d %s\n", rc, f);
    return rc;
}

static long s_melde_ok, s_melde_nein;
static void melde(void *ctx, const char *rel, int ok) { (void)ctx; (void)rel; if (ok) s_melde_ok++; else s_melde_nein++; }

static long laufen(const char *wurzel, const re15_abgleich_liste_t *l, long *nf)
{
    static const char *const baeume[] = { "shared_assets", "synchro" };
    s_melde_ok = s_melde_nein = 0;
    return re15_abgleich_waisen(wurzel, baeume, 2, l, melde, NULL, nf);
}

static void fall(const char *name, char *w, char *a, size_t n)
{
    snprintf(w, n, "%s/%s/wurzel", s_basis, name);
    snprintf(a, n, "%s/%s/aussen", s_basis, name);
    char t[2048];
    snprintf(t, sizeof t, "%s/%s", s_basis, name);
    ordner(t);
    ordner(w);
    ordner(a);
    printf("--- %s\n", name);
}

#define P(...) (snprintf(pb, sizeof pb, __VA_ARGS__), pb)

int main(int argc, char **argv)
{
    if (argc < 2) return 2;
    snprintf(s_basis, sizeof s_basis, "%s", argv[1]);
    ordner(s_basis);
    char w[1024], a[1024], pb[8192];
    re15_abgleich_liste_t l;
    long nf = 0, nw;
    const char *gelistet[] = { "shared_assets/PSX/DATA/TEX.TIM", "shared_assets/PSX/A.BIN", "synchro/STAGE1/m.wav" };

    /* W1 Baum selbst ist Symlink nach aussen: nichts darf ausserhalb geloescht werden */
    fall("W1_baum_symlink", w, a, sizeof w);
    datei(P("%s/fremd.bin", a), "x");
    ordner(P("%s/PSX", a));
    datei(P("%s/PSX/fremd2.bin", a), "x");
    if (symlink(a, P("%s/shared_assets", w)) != 0) printf("SETUP symlink: %s\n", strerror(errno));
    liste_bauen(&l, gelistet, 3);
    nw = laufen(w, &l, &nf);
    pruef(da(P("%s/fremd.bin", a)) && da(P("%s/PSX/fremd2.bin", a)), "W1 Ziel des Baum-Symlinks unberuehrt (geloescht %ld, Fehler %ld)", nw, nf);
    pruef(da(P("%s/shared_assets", w)), "W1 Baum-Symlink selbst bleibt (kein Baum -> uebersprungen)");
    re15_abgleich_frei(&l);

    /* W2 Unterordner ist Symlink nach aussen: nur der Link weg, nie hineingehen */
    fall("W2_unterordner_symlink", w, a, sizeof w);
    ordner(P("%s/shared_assets", w));
    datei(P("%s/innen.bin", a), "x");
    if (symlink(a, P("%s/shared_assets/PSX", w)) != 0) printf("SETUP symlink: %s\n", strerror(errno));
    liste_bauen(&l, gelistet, 3);
    nw = laufen(w, &l, &nf);
    pruef(da(P("%s/innen.bin", a)), "W2 Ziel des Unterordner-Symlinks unberuehrt");
    pruef(!da(P("%s/shared_assets/PSX", w)), "W2 der Link selbst ist weg (Waise, geloescht %ld, Fehler %ld)", nw, nf);
    re15_abgleich_frei(&l);

    /* W3 Datei-Symlinks: gelisteter Name bleibt (gewollt), ungelisteter Link weg, Ziel bleibt */
    fall("W3_datei_symlink", w, a, sizeof w);
    ordner(P("%s/shared_assets", w));
    ordner(P("%s/shared_assets/PSX", w));
    datei(P("%s/ziel.bin", a), "x");
    { char z0[1100], l0[1100]; snprintf(z0, sizeof z0, "%s/ziel.bin", a); snprintf(l0, sizeof l0, "%s/shared_assets/PSX/A.BIN", w);
      if (symlink(z0, l0) != 0) printf("SETUP symlink %s\n", strerror(errno)); }
    { char z[1100]; snprintf(z, sizeof z, "%s/ziel.bin", a);
      if (symlink(z, P("%s/shared_assets/PSX/fremd.lnk", w)) != 0) printf("SETUP symlink\n"); }
    liste_bauen(&l, gelistet, 3);
    nw = laufen(w, &l, &nf);
    pruef(da(P("%s/shared_assets/PSX/A.BIN", w)), "W3 gelisteter Symlink bleibt (Entpacker: stat folgt, rename ersetzt nur den Link)");
    pruef(!da(P("%s/shared_assets/PSX/fremd.lnk", w)) && da(P("%s/ziel.bin", a)), "W3 ungelisteter Link weg, Ziel bleibt (geloescht %ld)", nw);
    re15_abgleich_frei(&l);

    /* W4 Ordnertiefe > 64 (WAISEN_TIEFE): unterhalb nichts loeschen, Fehler zaehlen */
    fall("W4_tiefe_70", w, a, sizeof w);
    {
        size_t o = (size_t)snprintf(pb, sizeof pb, "%s/shared_assets", w);
        ordner(pb);
        char tief64[8192] = "", tief70[8192] = "";
        for (int i = 1; i <= 70; i++) {
            o += (size_t)snprintf(pb + o, sizeof pb - o, "/d");
            ordner(pb);
            if (i == 63) { snprintf(tief64, sizeof tief64, "%s/waise63.bin", pb); datei(tief64, "x"); }
            if (i == 70) { snprintf(tief70, sizeof tief70, "%s/waise70.bin", pb); datei(tief70, "x"); }
        }
        liste_bauen(&l, gelistet, 3);
        nw = laufen(w, &l, &nf);
        pruef(!da(tief64), "W4 Waise in Tiefe 63 geloescht");
        pruef(da(tief70) && nf > 0, "W4 Waise in Tiefe 70 bleibt, Fehler gezaehlt (%ld) - kein Loeschen jenseits der Grenze", nf);
        re15_abgleich_frei(&l);
    }

    /* W5 Pfad > 4096 B (WAISEN_PFAD): kein Loeschen eines abgeschnittenen Pfads */
    fall("W5_pfad_lang", w, a, sizeof w);
    {
        char seg[201];
        memset(seg, 'x', 200); seg[200] = '\0';
        snprintf(pb, sizeof pb, "%s/shared_assets", w);
        ordner(pb);
        if (chdir(pb) != 0) return 98;
        /* Koeder: eine gelistete Datei, deren Pfad ein PRAEFIX der tiefen Kette ist */
        int tiefe = 0;
        for (tiefe = 0; tiefe < 24; tiefe++) {        /* 24 x 201 B = 4824 B unter dem Baum */
            if (mkdir(seg, 0775) != 0 && errno != EEXIST) { printf("SETUP mkdir tief %d: %s\n", tiefe, strerror(errno)); break; }
            if (chdir(seg) != 0) break;
            if (tiefe == 17 || tiefe == 23) datei("waise.bin", "x");
        }
        if (chdir(s_basis) != 0) return 98;
        liste_bauen(&l, gelistet, 3);
        nw = laufen(w, &l, &nf);
        /* nachsehen per chdir-Kette */
        snprintf(pb, sizeof pb, "%s/shared_assets", w);
        int noch17 = -1, noch23 = -1;
        if (chdir(pb) == 0) {
            for (int t = 0; t < 24; t++) {
                if (chdir(seg) != 0) break;
                if (t == 17) noch17 = access("waise.bin", F_OK) == 0;
                if (t == 23) noch23 = access("waise.bin", F_OK) == 0;
            }
        }
        if (chdir(s_basis) != 0) return 98;
        printf("   (Pfadlaenge Tiefe 17: %zu B, Tiefe 23: %zu B unter %s)\n", strlen(w) + 15 + 18 * 201 + 10,
               strlen(w) + 15 + 24 * 201 + 10, w);
        pruef(noch17 == 0, "W5 Waise mit Pfad < 4096 B geloescht (noch da: %d)", noch17);
        pruef(noch23 == 1 && nf > 0, "W5 Waise mit Pfad > 4096 B bleibt, Fehler %ld (noch da: %d, geloescht gesamt %ld)", nf, noch23, nw);
        re15_abgleich_frei(&l);
    }

    /* W6 Ordner traegt den Namen einer gelisteten DATEI; Datei traegt den Namen eines gelisteten ORDNERS */
    fall("W6_typ_vertauscht", w, a, sizeof w);
    ordner(P("%s/shared_assets", w));
    ordner(P("%s/shared_assets/PSX", w));
    ordner(P("%s/shared_assets/PSX/A.BIN", w));
    datei(P("%s/shared_assets/PSX/A.BIN/innen.bin", w), "x");
    datei(P("%s/shared_assets/PSX/DATA", w), "x");
    liste_bauen(&l, gelistet, 3);
    nw = laufen(w, &l, &nf);
    pruef(!da(P("%s/shared_assets/PSX/A.BIN", w)), "W6 Ordner 'A.BIN' (gelistet als Datei) samt Inhalt weg -> Entpacker kann schreiben");
    pruef(!da(P("%s/shared_assets/PSX/DATA", w)), "W6 Datei 'DATA' (gelistet als Ordner) weg (geloescht %ld, Fehler %ld)", nw, nf);
    re15_abgleich_frei(&l);

    /* W7 nicht lesbarer / nicht beschreibbarer Ordner (nur ohne root aussagekraeftig) */
    fall("W7_rechte", w, a, sizeof w);
    ordner(P("%s/shared_assets", w));
    ordner(P("%s/shared_assets/zu", w));
    datei(P("%s/shared_assets/zu/waise.bin", w), "x");
    ordner(P("%s/shared_assets/nurlesen", w));
    datei(P("%s/shared_assets/nurlesen/waise.bin", w), "x");
    chmod(P("%s/shared_assets/zu", w), 0000);
    chmod(P("%s/shared_assets/nurlesen", w), 0555);
    liste_bauen(&l, gelistet, 3);
    nw = laufen(w, &l, &nf);
    chmod(P("%s/shared_assets/zu", w), 0775);
    chmod(P("%s/shared_assets/nurlesen", w), 0775);
    printf("   (uid %d; geloescht %ld, Fehler %ld, melde ok %ld / nicht loeschbar %ld)\n", (int)getuid(), nw, nf, s_melde_ok, s_melde_nein);
    if (getuid() != 0) {
        pruef(da(P("%s/shared_assets/zu/waise.bin", w)) && nf >= 1, "W7 unlesbarer Ordner: Inhalt bleibt, als Fehler gezaehlt (nur Warnung)");
        pruef(da(P("%s/shared_assets/nurlesen/waise.bin", w)) && s_melde_nein >= 1, "W7 nicht loeschbare Waise gemeldet (ok=0), kein Abbruch");
    } else {
        printf("   (root: Rechte-Faelle nicht aussagekraeftig, uebersprungen)\n");
    }
    re15_abgleich_frei(&l);

    /* W8 .neu-Reste, Sonderdateien, Namen mit Nicht-ASCII/ungueltigem UTF-8, Gross/klein (Linux: getrennte Dateien) */
    fall("W8_reste", w, a, sizeof w);
    ordner(P("%s/shared_assets", w));
    ordner(P("%s/shared_assets/PSX", w));
    ordner(P("%s/shared_assets/PSX/DATA", w));
    datei(P("%s/shared_assets/PSX/DATA/TEX.TIM", w), "x");
    datei(P("%s/shared_assets/PSX/DATA/TEX.TIM.neu", w), "x");
    datei(P("%s/shared_assets/PSX/a.bin", w), "x");            /* Gross/klein-Variante von A.BIN */
    datei(P("%s/shared_assets/PSX/\xff\xfe.bin", w), "x");
    datei(P("%s/shared_assets/.nomedia", w), "");
    mkfifo(P("%s/shared_assets/PSX/fifo", w), 0664);
    liste_bauen(&l, gelistet, 3);
    nw = laufen(w, &l, &nf);
    pruef(da(P("%s/shared_assets/PSX/DATA/TEX.TIM", w)), "W8 gelistete Datei bleibt");
    pruef(!da(P("%s/shared_assets/PSX/DATA/TEX.TIM.neu", w)) && !da(P("%s/shared_assets/PSX/a.bin", w))
          && !da(P("%s/shared_assets/PSX/\xff\xfe.bin", w)) && !da(P("%s/shared_assets/.nomedia", w))
          && !da(P("%s/shared_assets/PSX/fifo", w)), "W8 .neu-Rest, Gross/klein-Variante, Nicht-UTF-8-Name, .nomedia, FIFO weg (geloescht %ld, Fehler %ld)", nw, nf);
    nw = laufen(w, &l, &nf);
    pruef(nw == 0 && nf == 0, "W8/Y9 zweiter Lauf (= Neustart nach Abbruch) loescht nichts mehr (%ld/%ld)", nw, nf);
    re15_abgleich_frei(&l);

    /* W9 Wurzel mit '/' am Ende, Baum fehlt, Spielstand in der Wurzel */
    fall("W9_wurzel", w, a, sizeof w);
    datei(P("%s/re15_card.mcr", w), "save");
    datei(P("%s/re15_assets_entpackt.txt", w), "x");
    ordner(P("%s/synchro", w));
    datei(P("%s/synchro/alt.wav", w), "x");
    { char w2[1100]; snprintf(w2, sizeof w2, "%s/", w);
      liste_bauen(&l, gelistet, 3);
      nw = laufen(w2, &l, &nf); }
    pruef(da(P("%s/re15_card.mcr", w)) && da(P("%s/re15_assets_entpackt.txt", w)), "W9 Wurzel-Dateien unberuehrt");
    pruef(!da(P("%s/synchro/alt.wav", w)) && da(P("%s/synchro", w)), "W9 Waise in synchro weg, Baum bleibt; shared_assets fehlt = kein Fehler (%ld/%ld)", nw, nf);
    re15_abgleich_frei(&l);

    printf("ERGEBNIS: %d ok, %d FEHLER\n", s_ok, s_fehler);
    return s_fehler;
}
