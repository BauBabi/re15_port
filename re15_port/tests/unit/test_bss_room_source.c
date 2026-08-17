/**
 * @file test_bss_room_source.c
 * @brief DATEN-PIN: jeder BSS-Hintergrund-Chunk stammt aus dem Stage-Container SEINES Raums.
 *
 * ANLASS (Nutzer-Report 15, ROOM1100 "Hintergrund gehoert nicht in den Raum"): die 13 Dateien
 * unter shared_assets/PSX/BSS/ROOM1100/ waren KOPIEN der ROOM1170-Frames. Der Fix hat sie neu
 * aus STAGE1/ROOM110.BSS geschnitten. Diese Reparatur lag danach in KEINEM Test — dieselbe
 * Regression (falscher Raum beim Neu-Extrahieren, versehentliches Kopieren, halber Lauf)
 * konnte still zurueckkehren. Genau das pint diese Datei.
 *
 * BELEG (selbst nachgerechnet, 2026-08-17, roher Byte-Vergleich):
 *   - STAGE1/ROOM110.BSS = 851968 B = 13 x 65536 B, Rest 0.
 *   - NEU: BSS/ROOM1100/BG<i>.BSS == ROOM110.BSS[i*65536 .. +65536) fuer alle i in 0..12,
 *          und identisch zu BSS/ROOM1101/BG<i>.BSS (dieselbe Referenz, andere Spielfigur).
 *   - ALT (git HEAD, vor dem Fix): BSS/ROOM1100/BG<i>.BSS war fuer alle 13 i byte-identisch
 *          mit BSS/ROOM1170/BG<i>.BSS (und ROOM1171) — der belegte Fehlzustand.
 *
 * FORMAT/KONVENTION (aus dem ausgelieferten Baum abgeleitet, 156 Raum-Verzeichnisse geprueft):
 *   Verzeichnis  BSS/ROOM<S><RR><V>/BG<nn>.BSS     (S = Stage-Ziffer, RR = Raum, V = Figur)
 *   Quelle       STAGE<S>/ROOM<S><RR>.BSS, aufgeteilt in 65536-B-Chunks
 *   Chunk nn des Containers == BG<nn>.BSS. 65536 = die BSS-Frame-Chunk-Groesse des
 *   MDEC-Hintergrund-Streams (CD-Sektor-Vielfaches, 32 x 2048).
 *
 * Der Test liest AUSSCHLIESSLICH den Asset-Baum (kein Netz, kein Emulator, keine Engine).
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define RE15_STR(x)  #x
#define RE15_XSTR(x) RE15_STR(x)

#define CHUNK 65536u

static int fails = 0;
#define CHECK(cond, ...) do { if (!(cond)) { fprintf(stderr, "FAIL: " __VA_ARGS__); \
    fprintf(stderr, "\n"); fails++; } } while (0)

static long file_size(const char *path)
{
    FILE *f = fopen(path, "rb");
    if (!f) return -1;
    fseek(f, 0, SEEK_END);
    long sz = ftell(f);
    fclose(f);
    return sz;
}

/* Vergleicht BG<idx>.BSS eines Raum-Verzeichnisses mit Chunk <idx> des Stage-Containers.
 * Rueckgabe: 1 = identisch, 0 = Abweichung, -1 = Datei fehlt. */
static int compare_chunk(const char *dir, int idx, const char *src, long src_size)
{
    char p[700];
    snprintf(p, sizeof p, "%s/BG%02d.BSS", dir, idx);
    FILE *a = fopen(p, "rb");
    if (!a) return -1;
    FILE *b = fopen(src, "rb");
    if (!b) { fclose(a); return 0; }
    long off = (long)idx * (long)CHUNK;
    if (off + (long)CHUNK > src_size) { fclose(a); fclose(b); return 0; }
    fseek(b, off, SEEK_SET);
    static uint8_t ba[16384], bb[16384];
    int same = 1;
    for (unsigned done = 0; done < CHUNK; done += (unsigned)sizeof ba) {
        size_t ra = fread(ba, 1, sizeof ba, a);
        size_t rb = fread(bb, 1, sizeof bb, b);
        if (ra != sizeof ba || rb != sizeof bb || memcmp(ba, bb, ra) != 0) { same = 0; break; }
    }
    if (same) {   /* die Chunk-Datei darf auch nicht LAENGER als der Chunk sein */
        if (fgetc(a) != EOF) same = 0;
    }
    fclose(a); fclose(b);
    return same;
}

int main(void)
{
    const char *base = RE15_XSTR(RE15_ASSETS_PATH);
    printf("== BSS-Raum-Quellen-Pin (Report 15) ==\n");

    int rooms = 0, chunks_ok = 0, chunks_bad = 0, missing = 0, extra = 0, short_rooms = 0;
    char short_list[512]; short_list[0] = 0;

    /* Voll-Sweep: Stage 1..6 x Raum 0x00..0xFF x Figur 0..1. Kein dirent noetig — es wird
     * schlicht probiert zu oeffnen; nicht vorhandene Verzeichnisse fallen durch. */
    for (int s = 1; s <= 6; s++) {
        for (int rr = 0; rr <= 0xFF; rr++) {
            for (int v = 0; v <= 1; v++) {
                char dir[700], src[700], probe[700];
                snprintf(dir, sizeof dir, "%s/BSS/ROOM%X%02X%X", base, s, rr, v);
                snprintf(probe, sizeof probe, "%s/BG00.BSS", dir);
                if (file_size(probe) < 0) continue;              /* Raum nicht ausgeliefert */
                snprintf(src, sizeof src, "%s/STAGE%d/ROOM%X%02X.BSS", base, s, s, rr);
                long src_size = file_size(src);
                CHECK(src_size > 0, "ROOM%X%02X%X: Stage-Container %s fehlt", s, rr, v, src);
                if (src_size <= 0) continue;
                CHECK((src_size % (long)CHUNK) == 0,
                      "ROOM%X%02X%X: %s ist kein Vielfaches von 65536 (%ld)", s, rr, v, src,
                      src_size);
                int nch = (int)(src_size / (long)CHUNK);
                rooms++;
                int present = 0;
                for (int i = 0; i < nch; i++) {
                    int r = compare_chunk(dir, i, src, src_size);
                    if (r < 0) { missing++; continue; }
                    present++;
                    if (r == 1) chunks_ok++;
                    else {
                        chunks_bad++;
                        CHECK(0, "ROOM%X%02X%X/BG%02d.BSS != %s Chunk %d — der Hintergrund "
                                 "stammt NICHT aus dem Container dieses Raums (genau der "
                                 "ROOM1100-Fehlzustand: dort standen ROOM1170-Frames)",
                              s, rr, v, i, src, i);
                    }
                }
                /* Ein BG jenseits der Container-Laenge kann nur aus einem FREMDEN Raum stammen. */
                {   char p2[700];
                    snprintf(p2, sizeof p2, "%s/BG%02d.BSS", dir, nch);
                    if (file_size(p2) >= 0) {
                        extra++;
                        CHECK(0, "ROOM%X%02X%X: BG%02d.BSS existiert, %s hat aber nur %d Chunks",
                              s, rr, v, nch, src, nch);
                    }
                }
                if (present != nch) {
                    short_rooms++;
                    char tmp[64];
                    snprintf(tmp, sizeof tmp, "ROOM%X%02X%X(%d/%d) ", s, rr, v, present, nch);
                    if (strlen(short_list) + strlen(tmp) < sizeof short_list) strcat(short_list, tmp);
                }
            }
        }
    }

    printf("  Raeume: %d | Chunks identisch: %d | abweichend: %d | fehlend: %d | ueberzaehlig: %d\n",
           rooms, chunks_ok, chunks_bad, missing, extra);
    printf("  unvollstaendige Raeume: %d [%s]\n", short_rooms, short_list);

    /* ⛔ ABDECKUNGS-WAECHTER: ohne ihn waere jede Aussage oben ein vacuous pass, sobald der
     * Asset-Baum fehlt oder umbenannt wird (dann faende die Schleife 0 Raeume und der Test
     * waere gruen). Stand 2026-08-17: 156 Raum-Verzeichnisse, 1688 Chunk-Dateien
     * (Summe der Container-Chunks = 1692, davon fehlen 4 in ROOM5001, siehe unten). */
    CHECK(rooms >= 156, "Abdeckung: mindestens 156 BSS-Raum-Verzeichnisse erwartet, %d gefunden "
                        "(Asset-Baum unvollstaendig -> der Pin misst nichts)", rooms);
    CHECK(chunks_ok >= 1688, "Abdeckung: mindestens 1688 verglichene Chunks erwartet, %d",
          chunks_ok);
    CHECK(chunks_bad == 0, "%d Chunk(s) stammen aus einem fremden Raum", chunks_bad);
    CHECK(extra == 0, "%d ueberzaehlige BG-Datei(en)", extra);
    /* Bekannte, VORBESTEHENDE Unvollstaendigkeit (kein falscher Raum, nur weniger Frames):
     * ROOM5001 liefert 5 von 9 Chunks, alle 5 korrekt. Ein zweiter kurzer Raum waere neu. */
    CHECK(short_rooms <= 1, "unerwartet viele unvollstaendige Raeume (%d): %s", short_rooms,
          short_list);

    /* --- Der namentliche Report-15-Pin: ROOM1100 --------------------------------------- */
    {
        char dir[700], src[700];
        snprintf(dir, sizeof dir, "%s/BSS/ROOM1100", base);
        snprintf(src, sizeof src, "%s/STAGE1/ROOM110.BSS", base);
        long ss = file_size(src);
        CHECK(ss == 851968, "ROOM110.BSS muss 851968 B (13 Chunks) haben, ist %ld", ss);
        int ok13 = 1;
        for (int i = 0; i < 13; i++) if (compare_chunk(dir, i, src, ss) != 1) ok13 = 0;
        CHECK(ok13, "ROOM1100: alle 13 BG-Chunks muessen aus STAGE1/ROOM110.BSS stammen");

        /* Gegen-Pin auf den BELEGTEN Fehlzustand: vor dem Fix waren alle 13 Dateien
         * byte-identisch mit BSS/ROOM1170/ (git HEAD nachgerechnet). Faellt jemand darauf
         * zurueck, schlaegt DIESER Pin an — auch dann, wenn ROOM110.BSS mal fehlen sollte. */
        char d1170[700];
        snprintf(d1170, sizeof d1170, "%s/BSS/ROOM1170", base);
        char pa[700], pb[700];
        snprintf(pa, sizeof pa, "%s/BG00.BSS", dir);
        snprintf(pb, sizeof pb, "%s/BG00.BSS", d1170);
        FILE *fa = fopen(pa, "rb"), *fb = fopen(pb, "rb");
        CHECK(fa && fb, "ROOM1100/BG00.BSS und ROOM1170/BG00.BSS muessen beide existieren");
        if (fa && fb) {
            static uint8_t ba[CHUNK], bb[CHUNK];
            size_t ra = fread(ba, 1, CHUNK, fa), rb = fread(bb, 1, CHUNK, fb);
            CHECK(!(ra == rb && ra > 0 && memcmp(ba, bb, ra) == 0),
                  "ROOM1100/BG00.BSS ist wieder byte-identisch mit ROOM1170/BG00.BSS — genau der "
                  "gemeldete Fehlzustand (Report 15)");
        }
        if (fa) fclose(fa);
        if (fb) fclose(fb);

        /* ROOM1101 (Elliot-Variante desselben Raums) teilt denselben Container. */
        char d1101[700];
        snprintf(d1101, sizeof d1101, "%s/BSS/ROOM1101", base);
        int same01 = 1;
        for (int i = 0; i < 13; i++) if (compare_chunk(d1101, i, src, ss) != 1) same01 = 0;
        CHECK(same01, "ROOM1101 muss denselben Container STAGE1/ROOM110.BSS tragen");
    }

    if (fails) { fprintf(stderr, "\n%d Pin(s) FEHLGESCHLAGEN\n", fails); return 1; }
    printf("\nBSS-RAUM-QUELLEN: all checks passed\n");
    return 0;
}
