/* =============================================================================================
 * Runde 34a, N1 (2026-09-30): PC-Unit-Test fuer den Android-Entpacker-Kern
 * re15_port/platform/android/jni/asset_abgleich.c (Format v2 der Asset-Liste, SHA-256, Abgleich
 * gegen "zuletzt entpackt"). Uebersetzt GENAU die ausgelieferte Datei (Sonde probes/r34a_android.cmake).
 * PORT-WAHL, kein Originalverhalten (Kopf von asset_abgleich.h).
 *
 * SHA-256-Sollwerte: FIPS 180-2 Anhang B (""/"abc"/448 Bit/896 Bit/10^6 x 'a'); die Kette ueber die
 * Laengen 0..300 ist mit Python hashlib gerechnet (Dossier analysis/befunde_runde34_android/
 * android_entpacker_n1.md, Abschnitt 2).
 * ============================================================================================= */
#include "asset_abgleich.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

/* ------------------------------------------------------------------------------ SHA-256 */
static void sha_text(const void *d, size_t n, char hex[65])
{
    re15_sha256_t c;
    re15_sha256_start(&c);
    re15_sha256_dazu(&c, d, n);
    re15_sha256_ende(&c, hex);
}

static void test_sha256(void)
{
    static const struct { const char *m; const char *soll; } v[] = {
        { "", "e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855" },
        { "abc", "ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad" },
        { "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq",
          "248d6a61d20638b8e5c026930c3e6039a33ce45964ff2167f6ecedd419db06c1" },
        { "abcdefghbcdefghicdefghijdefghijkefghijklfghijklmghijklmnhijklmnoijklmnopjklmnopqklmnopqrlmnopqrsmnopqrstnopqrstu",
          "cf5b16a778af8380036ce59e7b0492370b249b11e8f07a51afac45037afee9d1" },
    };
    char hex[65];
    for (size_t i = 0; i < sizeof v / sizeof v[0]; i++) {
        sha_text(v[i].m, strlen(v[i].m), hex);
        PRUEFE(strcmp(hex, v[i].soll) == 0, "Vektor %zu: %s", i, hex);
    }

    /* 10^6 x 'a' in verschiedenen Stueckelungen (Blockpuffer an allen Grenzen) */
    static unsigned char a[1000000];
    memset(a, 'a', sizeof a);
    static const size_t stueck[] = { 1000000, 1, 55, 56, 63, 64, 65, 1000, 4097 };
    for (size_t s = 0; s < sizeof stueck / sizeof stueck[0]; s++) {
        re15_sha256_t c;
        re15_sha256_start(&c);
        for (size_t o = 0; o < sizeof a; o += stueck[s]) {
            size_t k = sizeof a - o < stueck[s] ? sizeof a - o : stueck[s];
            re15_sha256_dazu(&c, a + o, k);
        }
        re15_sha256_ende(&c, hex);
        PRUEFE(strcmp(hex, "cdc76e5c9914fb9281a1c7e284d73e67f1809a48a497200e046d39ccc7112cd0") == 0,
               "10^6 x 'a' in Stuecken zu %zu: %s", stueck[s], hex);
    }

    /* Kette ueber die Laengen 0..300 (Byte i = i*7+3): jede Auffuell-Grenze 55/56/63/64/119/120/... */
    unsigned char muster[300];
    for (int i = 0; i < 300; i++) muster[i] = (unsigned char)((i * 7 + 3) & 0xff);
    static char kette[301 * 64 + 1];
    for (int n = 0; n <= 300; n++) {
        sha_text(muster, (size_t)n, hex);
        memcpy(kette + 64 * n, hex, 64);
    }
    sha_text(kette, 301 * 64, hex);
    PRUEFE(strcmp(hex, "9ab015b3431ba48c0f99e81c5bb7d903f6bfea343d7d7b190120d07cc51e653b") == 0, "Kette 0..300: %s", hex);

    /* Datei */
    const char *pfad = "r34a_asset_abgleich_probe.bin";
    FILE *f = fopen(pfad, "wb");
    PRUEFE(f != NULL, "Probedatei nicht anlegbar");
    if (f) {
        fwrite("abc", 1, 3, f);
        fclose(f);
        long long g = -5;
        int rc = re15_sha256_datei(pfad, hex, &g);
        PRUEFE(rc == 0 && g == 3 && strcmp(hex, v[1].soll) == 0, "Datei 'abc': rc %d, %lld B, %s", rc, g, hex);
        remove(pfad);
    }
    long long g2 = 77;
    PRUEFE(re15_sha256_datei("r34a_gibt_es_nicht.bin", hex, &g2) == -1 && g2 == 77, "fehlende Datei");
}

/* ------------------------------------------------------------------------------ Liste */
#define SA "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
#define SB "0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef"
#define SC "fedcba9876543210fedcba9876543210fedcba9876543210fedcba9876543210"
#define SD "dddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddd"

static int lesen(const char *text, size_t len, re15_abgleich_liste_t *l, char *fehler, size_t fn)
{
    return re15_abgleich_lesen(l, text, len, fehler, fn);
}

/* rc einer Liste (Text als C-Zeichenkette) und optional ein Teilstring der Fehlermeldung */
static void erwarte(const char *titel, const char *text, int soll_rc, const char *soll_fehler)
{
    re15_abgleich_liste_t l;
    char fehler[256];
    int rc = lesen(text, strlen(text), &l, fehler, sizeof fehler);
    PRUEFE(rc == soll_rc, "%s: rc %d, soll %d (%s)", titel, rc, soll_rc, fehler);
    if (soll_rc != 0)
        PRUEFE(l.n == 0 && l.e == NULL && l.puffer == NULL, "%s: Liste nach Fehler nicht leer", titel);
    if (soll_fehler)
        PRUEFE(strstr(fehler, soll_fehler) != NULL, "%s: Meldung '%s' ohne '%s'", titel, fehler, soll_fehler);
    re15_abgleich_frei(&l);
}

static void test_lesen_gut(void)
{
    const char *t = "# re15 assets v2 3 13\n"
                    "5\t" SB "\tshared_assets/PSX/b.bin\n"
                    "0\t" SA "\tshared_assets/PSX/a.bin\n"
                    "8\t" SC "\tsynchro/STAGE1/x.wav\n";
    re15_abgleich_liste_t l;
    char fehler[256];
    int rc = lesen(t, strlen(t), &l, fehler, sizeof fehler);
    PRUEFE(rc == 0, "gute Liste: rc %d (%s)", rc, fehler);
    PRUEFE(l.n == 3 && l.summe == 13, "gute Liste: n %zu, summe %lld", l.n, l.summe);
    if (rc == 0 && l.n == 3) {
        PRUEFE(strcmp(l.e[0].pfad, "shared_assets/PSX/a.bin") == 0 && l.e[0].groesse == 0 && strcmp(l.e[0].sha, SA) == 0,
               "sortiert [0]: %s", l.e[0].pfad);
        PRUEFE(strcmp(l.e[1].pfad, "shared_assets/PSX/b.bin") == 0 && l.e[1].groesse == 5 && strcmp(l.e[1].sha, SB) == 0,
               "sortiert [1]: %s", l.e[1].pfad);
        PRUEFE(strcmp(l.e[2].pfad, "synchro/STAGE1/x.wav") == 0 && l.e[2].groesse == 8, "sortiert [2]: %s", l.e[2].pfad);
        const re15_abgleich_eintrag_t *x = re15_abgleich_suchen(&l, "synchro/STAGE1/x.wav");
        PRUEFE(x && strcmp(x->sha, SC) == 0, "suchen x.wav");
        PRUEFE(re15_abgleich_suchen(&l, "synchro/STAGE1/y.wav") == NULL, "suchen y.wav (fehlt)");
        PRUEFE(re15_abgleich_suchen(&l, "SHARED_ASSETS/PSX/a.bin") == NULL, "suchen ist bytegenau");
    }
    re15_abgleich_frei(&l);
    re15_abgleich_frei(&l);                                       /* doppelt freigeben: erlaubt */

    erwarte("CRLF", "# re15 assets v2 1 5\r\n5\t" SB "\ta/b\r\n", 0, NULL);
    erwarte("mehrere \\r am Zeilenende", "# re15 assets v2 1 5\r\r\n5\t" SB "\ta/b\r\r\r\n", 0, NULL);
    erwarte("Leerzeilen", "# re15 assets v2 1 5\n\n\r\n5\t" SB "\ta/b\n\n\n", 0, NULL);
    erwarte("letzte Zeile ohne \\n", "# re15 assets v2 1 5\n5\t" SB "\ta/b", 0, NULL);
    erwarte("Nullen vorn", "# re15 assets v2 01 005\n005\t" SB "\ta/b\n", 0, NULL);
    erwarte("18 Ziffern", "# re15 assets v2 1 999999999999999999\n999999999999999999\t" SB "\ta/b\n", 0, NULL);
    erwarte("UTF-8 2 Byte", "# re15 assets v2 1 1\n1\t" SB "\ta/\xc3\x84.bin\n", 0, NULL);
    erwarte("UTF-8 4 Byte", "# re15 assets v2 1 1\n1\t" SB "\ta/\xf0\x9f\x98\x80\n", 0, NULL);
    erwarte("Leerzeichen im Pfad", "# re15 assets v2 1 1\n1\t" SB "\ta/b c.bin\n", 0, NULL);
    erwarte("Punkt im Segment", "# re15 assets v2 1 1\n1\t" SB "\ta/.b/..c/x.neux\n", 0, NULL);
}

static void test_lesen_kopf(void)
{
    erwarte("v1-Liste (bis v0.8.19)", "# re15 assets 2 10\n5\ta/b\n5\ta/c\n", RE15_ABGLEICH_ALTES_FORMAT, "alten Format v1");
    erwarte("v1-Kopf, CRLF", "# re15 assets 2 10\r\n5\ta/b\r\n", RE15_ABGLEICH_ALTES_FORMAT, NULL);
    erwarte("v1-Kopf mit v2-Zeilen", "# re15 assets 1 5\n5\t" SB "\ta/b\n", RE15_ABGLEICH_ALTES_FORMAT, NULL);
    erwarte("Kopf fehlt", "5\t" SB "\ta/b\n", RE15_ABGLEICH_UNGUELTIG, "Kopfzeile fehlt/unlesbar");
    erwarte("Kopf v3", "# re15 assets v3 1 5\n5\t" SB "\ta/b\n", RE15_ABGLEICH_UNGUELTIG, "Kopfzeile fehlt/unlesbar");
    erwarte("Kopf mit Leerzeichen dahinter", "# re15 assets v2 1 5 \n5\t" SB "\ta/b\n", RE15_ABGLEICH_UNGUELTIG, "Kopfzeile");
    erwarte("Kopf mit Tab dahinter", "# re15 assets v2 1 5\t\n5\t" SB "\ta/b\n", RE15_ABGLEICH_UNGUELTIG, "Kopfzeile");
    erwarte("Kopf mit Leerzeichen davor", " # re15 assets v2 1 5\n5\t" SB "\ta/b\n", RE15_ABGLEICH_UNGUELTIG, "Kopfzeile");
    erwarte("Kopf nach Leerzeile", "\n# re15 assets v2 1 5\n5\t" SB "\ta/b\n", RE15_ABGLEICH_UNGUELTIG, "Kopfzeile");
    erwarte("Kopf mit BOM", "\xef\xbb\xbf# re15 assets v2 1 5\n5\t" SB "\ta/b\n", RE15_ABGLEICH_UNGUELTIG, "Kopfzeile");
    erwarte("Kopf 19 Ziffern", "# re15 assets v2 1 0000000000000000005\n5\t" SB "\ta/b\n", RE15_ABGLEICH_UNGUELTIG, "Kopfzeile");
    erwarte("Kopf zwei Leerzeichen", "# re15 assets v2 1  5\n5\t" SB "\ta/b\n", RE15_ABGLEICH_UNGUELTIG, "Kopfzeile");
    erwarte("Kopf ohne Bytes", "# re15 assets v2 1\n5\t" SB "\ta/b\n", RE15_ABGLEICH_UNGUELTIG, "Kopfzeile");
    erwarte("Kopf mit Vorzeichen", "# re15 assets v2 +1 5\n5\t" SB "\ta/b\n", RE15_ABGLEICH_UNGUELTIG, "Kopfzeile");
    erwarte("Kopf Anzahl falsch", "# re15 assets v2 2 5\n5\t" SB "\ta/b\n", RE15_ABGLEICH_UNGUELTIG, "Kopfzeile nennt 2 Dateien / 5 Bytes");
    erwarte("Kopf Bytes falsch", "# re15 assets v2 1 6\n5\t" SB "\ta/b\n", RE15_ABGLEICH_UNGUELTIG, "die Zeilen ergeben 1 / 5");
    erwarte("keine Datei", "# re15 assets v2 0 0\n", RE15_ABGLEICH_UNGUELTIG, "ohne Dateien");
    erwarte("leerer Text", "", RE15_ABGLEICH_UNGUELTIG, "Kopfzeile");
    erwarte("Kommentarzeile", "# re15 assets v2 1 5\n# Notiz\n5\t" SB "\ta/b\n", RE15_ABGLEICH_UNGUELTIG, "Zeile 2: unerwartete Kommentarzeile");
}

static void test_lesen_zeilen(void)
{
    erwarte("Groesse fehlt", "# re15 assets v2 1 5\n\t" SB "\ta/b\n", RE15_ABGLEICH_UNGUELTIG, "Zeile 2: Groesse");
    erwarte("Groesse 19 Ziffern", "# re15 assets v2 1 5\n0000000000000000005\t" SB "\ta/b\n", RE15_ABGLEICH_UNGUELTIG, "Groesse");
    erwarte("Groesse mit Vorzeichen", "# re15 assets v2 1 5\n+5\t" SB "\ta/b\n", RE15_ABGLEICH_UNGUELTIG, "Groesse");
    erwarte("Groesse mit Leerzeichen", "# re15 assets v2 1 5\n5 \t" SB "\ta/b\n", RE15_ABGLEICH_UNGUELTIG, "Groesse");
    erwarte("Summe fehlt (v1-Zeile)", "# re15 assets v2 1 5\n5\ta/b\n", RE15_ABGLEICH_UNGUELTIG, "SHA-256");
    erwarte("Summe Grossbuchstaben", "# re15 assets v2 1 5\n5\t" "0123456789ABCDEF0123456789abcdef0123456789abcdef0123456789abcdef" "\ta/b\n",
            RE15_ABGLEICH_UNGUELTIG, "SHA-256");
    erwarte("Summe 63 Zeichen", "# re15 assets v2 1 5\n5\t" "0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcde" "\ta/b\n",
            RE15_ABGLEICH_UNGUELTIG, "SHA-256");
    erwarte("Summe 65 Zeichen", "# re15 assets v2 1 5\n5\t" SB "0\ta/b\n", RE15_ABGLEICH_UNGUELTIG, "SHA-256");
    erwarte("Summe mit Leerzeichen dahinter", "# re15 assets v2 1 5\n5\t" SB " \ta/b\n", RE15_ABGLEICH_UNGUELTIG, "SHA-256");
    erwarte("Summe mit Leerzeichen davor", "# re15 assets v2 1 5\n5\t " SB "\ta/b\n", RE15_ABGLEICH_UNGUELTIG, "SHA-256");
    erwarte("Summe mit 'g'", "# re15 assets v2 1 5\n5\t" "g123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef" "\ta/b\n",
            RE15_ABGLEICH_UNGUELTIG, "SHA-256");
    erwarte("Pfad fehlt", "# re15 assets v2 1 5\n5\t" SB "\t\n", RE15_ABGLEICH_UNGUELTIG, "unzulaessiger Pfad");
    erwarte("Pfad fehlt ohne Tab", "# re15 assets v2 1 5\n5\t" SB "\n", RE15_ABGLEICH_UNGUELTIG, "SHA-256");
    erwarte("Pfad mit Tab", "# re15 assets v2 1 5\n5\t" SB "\ta/b\tc\n", RE15_ABGLEICH_UNGUELTIG, "unzulaessiger Pfad");
    erwarte("Pfad mit Tab am Ende", "# re15 assets v2 1 5\n5\t" SB "\ta/b\t\n", RE15_ABGLEICH_UNGUELTIG, "unzulaessiger Pfad");
    erwarte("Pfad '..'", "# re15 assets v2 1 5\n5\t" SB "\tshared_assets/../../boese.bin\n", RE15_ABGLEICH_UNGUELTIG, "unzulaessiger Pfad");
    erwarte("Pfad '..' am Ende", "# re15 assets v2 1 5\n5\t" SB "\ta/..\n", RE15_ABGLEICH_UNGUELTIG, "unzulaessiger Pfad");
    erwarte("Pfad '.'", "# re15 assets v2 1 5\n5\t" SB "\ta/./b\n", RE15_ABGLEICH_UNGUELTIG, "unzulaessiger Pfad");
    erwarte("Pfad absolut", "# re15 assets v2 1 5\n5\t" SB "\t/data/b\n", RE15_ABGLEICH_UNGUELTIG, "unzulaessiger Pfad");
    erwarte("Pfad Backslash", "# re15 assets v2 1 5\n5\t" SB "\ta\\b\n", RE15_ABGLEICH_UNGUELTIG, "unzulaessiger Pfad");
    erwarte("Pfad Backslash + Slash", "# re15 assets v2 1 5\n5\t" SB "\ta/..\\..\\b\n", RE15_ABGLEICH_UNGUELTIG, "unzulaessiger Pfad");
    erwarte("Pfad ohne '/'", "# re15 assets v2 1 5\n5\t" SB "\tre15_card.mcr\n", RE15_ABGLEICH_UNGUELTIG, "unzulaessiger Pfad");
    erwarte("Pfad '//'", "# re15 assets v2 1 5\n5\t" SB "\ta//b\n", RE15_ABGLEICH_UNGUELTIG, "unzulaessiger Pfad");
    erwarte("Pfad '/' am Ende", "# re15 assets v2 1 5\n5\t" SB "\ta/b/\n", RE15_ABGLEICH_UNGUELTIG, "unzulaessiger Pfad");
    erwarte("Pfad Steuerzeichen 0x01", "# re15 assets v2 1 5\n5\t" SB "\ta/b\x01\n", RE15_ABGLEICH_UNGUELTIG, "unzulaessiger Pfad");
    erwarte("Pfad Steuerzeichen 0x1f", "# re15 assets v2 1 5\n5\t" SB "\ta/\x1f" "b\n", RE15_ABGLEICH_UNGUELTIG, "unzulaessiger Pfad");
    erwarte("Pfad DEL", "# re15 assets v2 1 5\n5\t" SB "\ta/b\x7f\n", RE15_ABGLEICH_UNGUELTIG, "unzulaessiger Pfad");
    erwarte("Pfad VT", "# re15 assets v2 1 5\n5\t" SB "\ta/b\x0b\n", RE15_ABGLEICH_UNGUELTIG, "unzulaessiger Pfad");
    erwarte("Pfad '\\r' mitten drin", "# re15 assets v2 1 5\n5\t" SB "\ta/\rb\n", RE15_ABGLEICH_UNGUELTIG, "unzulaessiger Pfad");
    erwarte("Pfad endet auf .neu", "# re15 assets v2 1 5\n5\t" SB "\ta/b.neu\n", RE15_ABGLEICH_UNGUELTIG, "unzulaessiger Pfad");
    erwarte("Pfad endet auf .NEU", "# re15 assets v2 1 5\n5\t" SB "\ta/b.NEU\n", RE15_ABGLEICH_UNGUELTIG, "unzulaessiger Pfad");
    erwarte("Pfad endet auf .Neu", "# re15 assets v2 1 5\n5\t" SB "\ta/b.Neu\n", RE15_ABGLEICH_UNGUELTIG, "unzulaessiger Pfad");
    erwarte("Pfad ist .neu-Segment", "# re15 assets v2 1 5\n5\t" SB "\ta/.neu\n", RE15_ABGLEICH_UNGUELTIG, "unzulaessiger Pfad");
    erwarte("UTF-8 kaputt 0xff", "# re15 assets v2 1 5\n5\t" SB "\ta/b\xff\n", RE15_ABGLEICH_UNGUELTIG, "unzulaessiger Pfad");
    erwarte("UTF-8 ueberlang", "# re15 assets v2 1 5\n5\t" SB "\ta/\xc0\xaf\n", RE15_ABGLEICH_UNGUELTIG, "unzulaessiger Pfad");
    erwarte("UTF-8 ueberlang 3 Byte", "# re15 assets v2 1 5\n5\t" SB "\ta/\xe0\x80\xaf\n", RE15_ABGLEICH_UNGUELTIG, "unzulaessiger Pfad");
    erwarte("UTF-8 Surrogat", "# re15 assets v2 1 5\n5\t" SB "\ta/\xed\xa0\x80\n", RE15_ABGLEICH_UNGUELTIG, "unzulaessiger Pfad");
    erwarte("UTF-8 ueber U+10FFFF", "# re15 assets v2 1 5\n5\t" SB "\ta/\xf4\x90\x80\x80\n", RE15_ABGLEICH_UNGUELTIG, "unzulaessiger Pfad");
    erwarte("UTF-8 abgeschnitten", "# re15 assets v2 1 5\n5\t" SB "\ta/\xe2\x82\n", RE15_ABGLEICH_UNGUELTIG, "unzulaessiger Pfad");
    erwarte("UTF-8 Folgebyte allein", "# re15 assets v2 1 5\n5\t" SB "\ta/\x80\n", RE15_ABGLEICH_UNGUELTIG, "unzulaessiger Pfad");
    erwarte("Pfad doppelt", "# re15 assets v2 2 10\n5\t" SB "\ta/b\n5\t" SC "\ta/b\n", RE15_ABGLEICH_UNGUELTIG, "Pfad doppelt: a/b");
    erwarte("Pfad nur in Gross/klein verschieden", "# re15 assets v2 2 10\n5\t" SB "\ta/B\n5\t" SC "\ta/b\n",
            RE15_ABGLEICH_UNGUELTIG, "nur in Gross/klein verschieden");
    erwarte("Pfad Gross/klein im Ordner", "# re15 assets v2 3 15\n5\t" SB "\tPSX/x\n5\t" SC "\tb/y\n5\t" SA "\tpsx/X\n",
            RE15_ABGLEICH_UNGUELTIG, "nur in Gross/klein verschieden");

    /* Pfadlaenge: 512 Bytes gut, 513 abgelehnt */
    {
        static char t[2048], p[600];
        memset(p, 'x', sizeof p);
        p[0] = 'a'; p[1] = '/';
        p[512] = '\0';
        snprintf(t, sizeof t, "# re15 assets v2 1 5\n5\t" SB "\t%s\n", p);
        erwarte("Pfad 512 Bytes", t, 0, NULL);
        p[512] = 'x'; p[513] = '\0';
        snprintf(t, sizeof t, "# re15 assets v2 1 5\n5\t" SB "\t%s\n", p);
        erwarte("Pfad 513 Bytes", t, RE15_ABGLEICH_UNGUELTIG, "unzulaessiger Pfad");
    }
    /* NUL-Byte (Laenge explizit) */
    {
        static const char t[] = "# re15 assets v2 1 5\n5\t" SB "\ta/b\0c\n";
        re15_abgleich_liste_t l;
        char fehler[256];
        int rc = lesen(t, sizeof t - 1, &l, fehler, sizeof fehler);
        PRUEFE(rc == RE15_ABGLEICH_UNGUELTIG && strstr(fehler, "NUL-Byte"), "NUL-Byte: rc %d (%s)", rc, fehler);
        re15_abgleich_frei(&l);
    }
    /* Summe der Groessen laeuft ueber (10 x 999999999999999999 > 2^63-1) */
    {
        static char t[4096];
        size_t o = (size_t)snprintf(t, sizeof t, "# re15 assets v2 10 999999999999999999\n");
        for (int i = 0; i < 10; i++)
            o += (size_t)snprintf(t + o, sizeof t - o, "999999999999999999\t" SB "\ta/%d\n", i);
        erwarte("Summe laeuft ueber", t, RE15_ABGLEICH_UNGUELTIG, "laeuft ueber");
    }
    /* > 64 MiB */
    {
        size_t n = (size_t)RE15_ABGLEICH_LISTE_MAX + 1;
        char *t = (char *)malloc(n);
        PRUEFE(t != NULL, "64 MiB + 1 nicht anlegbar");
        if (t) {
            memset(t, '\n', n);
            memcpy(t, "# re15 assets v2 1 5\n5\t" SB "\ta/b\n", strlen("# re15 assets v2 1 5\n5\t" SB "\ta/b\n"));
            re15_abgleich_liste_t l;
            char fehler[256];
            int rc = lesen(t, n, &l, fehler, sizeof fehler);
            PRUEFE(rc == RE15_ABGLEICH_UNGUELTIG && strstr(fehler, "64 MiB"), "64 MiB + 1: rc %d (%s)", rc, fehler);
            re15_abgleich_frei(&l);
            rc = lesen(t, n - 1, &l, fehler, sizeof fehler);          /* genau 64 MiB: Leerzeilen, gut */
            PRUEFE(rc == 0 && l.n == 1, "genau 64 MiB: rc %d (%s)", rc, fehler);
            re15_abgleich_frei(&l);
            free(t);
        }
    }
}

static void test_pfad_ok(void)
{
    PRUEFE(re15_abgleich_pfad_ok("a/b", 3) == 1, "a/b");
    PRUEFE(re15_abgleich_pfad_ok("shared_assets/RE15DOOR/P07G.DO2", 31) == 1, "P07G.DO2");
    PRUEFE(re15_abgleich_pfad_ok("a/b", 0) == 0, "Laenge 0");
    PRUEFE(re15_abgleich_pfad_ok(NULL, 3) == 0, "NULL");
    PRUEFE(re15_abgleich_pfad_ok("a/b.ne", 6) == 1, ".ne");
    PRUEFE(re15_abgleich_pfad_ok("a/bneu", 6) == 1, "bneu (ohne Punkt)");
    PRUEFE(re15_abgleich_pfad_ok("a/b.neu/c", 9) == 1, ".neu nur am Ende verboten");
    PRUEFE(re15_abgleich_pfad_ok("../a", 4) == 0, "../a");
    PRUEFE(re15_abgleich_pfad_ok("./a/b", 5) == 0, "./a/b");
    PRUEFE(re15_abgleich_pfad_ok("a/...", 5) == 1, "'...' ist ein Name");
}

/* ------------------------------------------------------------------------------ Abgleich */
static void liste(re15_abgleich_liste_t *l, const char *text)
{
    char fehler[256];
    int rc = re15_abgleich_lesen(l, text, strlen(text), fehler, sizeof fehler);
    if (rc != 0) { printf("FEHLER: Test-Liste ungueltig: %s\n", fehler); exit(1); }
}

static void test_planen(void)
{
    re15_abgleich_liste_t alt, neu, gleich;
    liste(&alt, "# re15 assets v2 4 20\n"
                "5\t" SA "\ta/bleibt\n"
                "5\t" SB "\ta/gleich_gross\n"
                "5\t" SC "\ta/waechst\n"
                "5\t" SD "\ta/weg\n");
    liste(&neu, "# re15 assets v2 4 26\n"
                "5\t" SA "\ta/bleibt\n"
                "5\t" SD "\ta/gleich_gross\n"      /* gleiche Groesse, andere Summe (Befund N1a) */
                "6\t" SC "\ta/waechst\n"           /* andere Groesse, gleiche Summe */
                "10\t" SB "\tb/neu\n");
    liste(&gleich, "# re15 assets v2 4 20\n"
                   "5\t" SD "\ta/weg\n"
                   "5\t" SC "\ta/waechst\n"
                   "5\t" SB "\ta/gleich_gross\n"
                   "5\t" SA "\ta/bleibt\n");

    re15_abgleich_plan_t p;
    PRUEFE(re15_abgleich_planen(&p, &neu, &alt) == 0, "planen");
    /* neu->e ist sortiert: a/bleibt, a/gleich_gross, a/waechst, b/neu */
    PRUEFE(p.aktion[0] == RE15_ABGLEICH_BEHALTEN, "a/bleibt: %d", p.aktion[0]);
    PRUEFE(p.aktion[1] == RE15_ABGLEICH_GEAENDERT, "a/gleich_gross: %d", p.aktion[1]);
    PRUEFE(p.aktion[2] == RE15_ABGLEICH_GEAENDERT, "a/waechst: %d", p.aktion[2]);
    PRUEFE(p.aktion[3] == RE15_ABGLEICH_NEU, "b/neu: %d", p.aktion[3]);
    PRUEFE(p.n_behalten == 1 && p.n_geaendert == 2 && p.n_neu == 1 && p.n_pruefen == 0,
           "Zaehler %zu/%zu/%zu/%zu", p.n_behalten, p.n_geaendert, p.n_neu, p.n_pruefen);
    PRUEFE(p.n_weg == 1 && strcmp(p.weg[0], "a/weg") == 0, "weg: %zu", p.n_weg);
    re15_abgleich_plan_frei(&p);
    re15_abgleich_plan_frei(&p);

    /* gleiche Liste (andere Zeilenfolge): alles BEHALTEN, nichts weg */
    PRUEFE(re15_abgleich_planen(&p, &alt, &gleich) == 0, "planen gleich");
    PRUEFE(p.n_behalten == 4 && p.n_geaendert == 0 && p.n_neu == 0 && p.n_weg == 0, "gleiche Liste: %zu behalten, %zu weg",
           p.n_behalten, p.n_weg);
    re15_abgleich_plan_frei(&p);

    /* keine gueltige "zuletzt entpackt"-Liste: alles PRUEFEN, nichts weg */
    PRUEFE(re15_abgleich_planen(&p, &neu, NULL) == 0, "planen ohne alt");
    PRUEFE(p.n_pruefen == 4 && p.n_weg == 0 && p.weg == NULL, "ohne alt: %zu pruefen", p.n_pruefen);
    for (size_t i = 0; i < neu.n; i++) PRUEFE(p.aktion[i] == RE15_ABGLEICH_PRUEFEN, "ohne alt [%zu]", i);
    re15_abgleich_plan_frei(&p);

    /* neue Datei allein / entfernte Datei allein */
    re15_abgleich_liste_t eins, zwei;
    liste(&eins, "# re15 assets v2 1 5\n5\t" SA "\ta/x\n");
    liste(&zwei, "# re15 assets v2 2 10\n5\t" SA "\ta/x\n5\t" SB "\ta/y\n");
    PRUEFE(re15_abgleich_planen(&p, &zwei, &eins) == 0 && p.n_neu == 1 && p.aktion[1] == RE15_ABGLEICH_NEU && p.n_weg == 0,
           "neue Datei");
    re15_abgleich_plan_frei(&p);
    PRUEFE(re15_abgleich_planen(&p, &eins, &zwei) == 0 && p.n_behalten == 1 && p.n_weg == 1 && strcmp(p.weg[0], "a/y") == 0,
           "entfernte Datei");
    re15_abgleich_plan_frei(&p);

    re15_abgleich_frei(&eins);
    re15_abgleich_frei(&zwei);
    re15_abgleich_frei(&alt);
    re15_abgleich_frei(&neu);
    re15_abgleich_frei(&gleich);
}

static void test_tun(void)
{
    PRUEFE(re15_abgleich_tun(RE15_ABGLEICH_BEHALTEN, 5, 5) == RE15_TUN_NICHTS, "behalten, gleich gross");
    PRUEFE(re15_abgleich_tun(RE15_ABGLEICH_BEHALTEN, 4, 5) == RE15_TUN_ENTPACKEN, "behalten, kleiner");
    PRUEFE(re15_abgleich_tun(RE15_ABGLEICH_BEHALTEN, 6, 5) == RE15_TUN_ENTPACKEN, "behalten, groesser");
    PRUEFE(re15_abgleich_tun(RE15_ABGLEICH_BEHALTEN, -1, 5) == RE15_TUN_ENTPACKEN, "behalten, fehlt");
    PRUEFE(re15_abgleich_tun(RE15_ABGLEICH_BEHALTEN, -1, 0) == RE15_TUN_ENTPACKEN, "behalten, 0-B-Datei fehlt");
    PRUEFE(re15_abgleich_tun(RE15_ABGLEICH_BEHALTEN, 0, 0) == RE15_TUN_NICHTS, "behalten, 0-B-Datei da");
    PRUEFE(re15_abgleich_tun(RE15_ABGLEICH_GEAENDERT, 5, 5) == RE15_TUN_ENTPACKEN, "geaendert, gleich gross (N1a)");
    PRUEFE(re15_abgleich_tun(RE15_ABGLEICH_GEAENDERT, -1, 5) == RE15_TUN_ENTPACKEN, "geaendert, fehlt");
    PRUEFE(re15_abgleich_tun(RE15_ABGLEICH_NEU, 5, 5) == RE15_TUN_ENTPACKEN, "neu, gleich gross");
    PRUEFE(re15_abgleich_tun(RE15_ABGLEICH_NEU, -1, 5) == RE15_TUN_ENTPACKEN, "neu, fehlt");
    PRUEFE(re15_abgleich_tun(RE15_ABGLEICH_PRUEFEN, 5, 5) == RE15_TUN_SUMME_PRUEFEN, "pruefen, gleich gross");
    PRUEFE(re15_abgleich_tun(RE15_ABGLEICH_PRUEFEN, 4, 5) == RE15_TUN_ENTPACKEN, "pruefen, andere Groesse");
    PRUEFE(re15_abgleich_tun(RE15_ABGLEICH_PRUEFEN, -1, 5) == RE15_TUN_ENTPACKEN, "pruefen, fehlt");
    PRUEFE(re15_abgleich_tun(RE15_ABGLEICH_PRUEFEN, -1, 0) == RE15_TUN_ENTPACKEN, "pruefen, 0-B-Datei fehlt");
    PRUEFE(re15_abgleich_tun(99, 5, 5) == RE15_TUN_ENTPACKEN, "unbekannte Aktion: entpacken");
}

int main(void)
{
    test_sha256();
    test_lesen_gut();
    test_lesen_kopf();
    test_lesen_zeilen();
    test_pfad_ok();
    test_planen();
    test_tun();
    printf("== r34a asset_abgleich: %d Pruefungen, %d Fehler ==\n", s_pruef, s_fehl);
    return s_fehl ? 1 : 0;
}
