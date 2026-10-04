/* =============================================================================================
 * RE1.5 Rebuilt — Android: Asset-Liste lesen und gegen "zuletzt entpackt" abgleichen
 * (Runde 34a, N1 "Geraete-Entpacker update-sicher", 2026-09-30)
 *
 * PORT-WAHL, kein Originalverhalten: die PSX las von CD, der PC-Port liest den Paketordner.
 * Nur die Android-App muss ihre Assets aus der APK in den App-Speicherordner entpacken
 * (android_glue.c re15_android_bootstrap_assets). Diese Datei ist der reine C-Teil davon -
 * ohne SDL, ohne Android (nur C und fuer re15_abgleich_waisen POSIX-Dateifunktionen) -, damit ein
 * PC-Unit-Test (tests/unit/test_r34a_asset_abgleich.c, Sonde probes/r34a_android.cmake) GENAU diesen
 * Code uebersetzt und prueft.
 *
 * WARUM (Befund N1a): bis v0.8.19 trug die Liste nur "<bytes>\t<pfad>", und der Marker
 * re15_assets_ok.txt war ein FNV-1a ueber diese Liste. Aenderte ein Update eine Datei bei
 * GLEICHER Groesse (P07G.DO2 in Runde 33 dreimal bei 55908 B), blieb die Liste bytegleich, das
 * Entpacken wurde ganz uebersprungen, und die alte Datei blieb auf dem Geraet.
 *
 * FORMAT v2 (Schreiber: platform/android/app/build.gradle writeAssetManifest; Pruefer:
 * release/apk_asset_gate.py manifest_lesen - liest nach DENSELBEN Regeln wie re15_abgleich_lesen):
 *     # re15 assets v2 <anzahl> <bytes>\n              Kopfzeile, IMMER die erste Zeile
 *     <bytes>\t<sha256, 64 Zeichen 0-9a-f>\t<pfad>\n  je Datei (Schreiber sortiert nach Pfad)
 *   - Zeilen an '\n' getrennt; angehaengte '\r' werden abgeschnitten, leere Zeilen uebersprungen.
 *   - Kopfzeile: genau "# re15 assets v2 " + 1-18 Ziffern + ' ' + 1-18 Ziffern. Die alte Kopfzeile
 *     "# re15 assets <n> <b>" (v1, bis v0.8.19) wird erkannt und ABGELEHNT (eigene Rueckgabe).
 *   - Datenzeile: 1-18 Ziffern, Tab, 64 Zeichen [0-9a-f] (Grossbuchstaben NICHT), Tab, Pfad.
 *   - Pfad: 1-512 Bytes, relativ, mindestens ein '/' (Assets liegen nie direkt im Speicherordner -
 *     dort liegen Logs, Spielstand und diese Listen), NUR druckbares ASCII 0x20-0x7e (also keine
 *     Steuerzeichen, kein 0x7f, kein Byte >= 0x80), kein '\\', jedes Segment 1-251 Bytes und nicht
 *     '.'/'..', KEIN Segment endet auf ".neu" (ASCII, Gross/klein egal: Endung der Zwischendatei; bis Runde 35 nur
 *     der ganze Pfad - Regel R1, siehe re15_abgleich_weg_frei unten).
 *   - kein Pfad ist zugleich Ordner eines anderen ("a/q" und "a/q/c", ASCII-Gross/klein egal; Regel R2, Runde 35).
 *     Nachbesserung R4-1 (Gegenpruefung H5/U3): bis dahin war jedes wohlgeformte UTF-8 erlaubt - der
 *     App-Speicher faltet aber Unicode-Gross/klein, NFC/NFD und 'ss'/U+00DF (Emulator API 36:
 *     Kelvin-Zeichen/K je EINE Datei), NTFS nicht; ein solches Paar wurde auf dem Geraet still eine
 *     Datei mit falschem Inhalt. Und Namen > 255 B legt das Geraet nicht an, '.neu' haengt 4 B an.
 *   - weitere '#'-Zeilen, doppelte Pfade (auch nur in ASCII-Gross/klein verschieden: der
 *     App-Speicher ist case-insensitiv; mit reinem ASCII ist diese Faltung vollstaendig), NUL-Bytes,
 *     keine Datei, Kopfzeile passt nicht zu den Zeilen, Liste > 64 MiB -> die GANZE Liste ist
 *     ungueltig (fail closed: lieber gar nicht entpacken als still mit einem Loch im Asset-Baum).
 * ============================================================================================= */
#ifndef RE15_ASSET_ABGLEICH_H
#define RE15_ASSET_ABGLEICH_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define RE15_ABGLEICH_KOPF        "# re15 assets v2 "   /* Versionskennung v2 */
#define RE15_ABGLEICH_KOPF_V1     "# re15 assets "      /* bis v0.8.19: ohne Pruefsumme */
#define RE15_ABGLEICH_LISTE_MAX   (64u << 20)           /* wie bisher read_apk_asset: > 64 MiB -> abgelehnt */
#define RE15_ABGLEICH_ZAHL_MAX    18                    /* Ziffern je Zahl (< 2^63, kein Ueberlauf) */
#define RE15_ABGLEICH_PFAD_MAX    512                   /* Bytes je Pfad (Puffer im Entpacker: PATH_MAX) */
#define RE15_ABGLEICH_SEGMENT_MAX 251                   /* Bytes je Segment: Geraet Name <= 255 B, dazu ".neu" */
#define RE15_ABGLEICH_NEU_ENDUNG  ".neu"                /* Zwischendatei <ziel>.neu, danach rename() */

/* ---------------------------------------------------------------------------- SHA-256 (FIPS 180-4) */
typedef struct {
    uint32_t h[8];
    uint64_t bytes;
    unsigned char blk[64];
    size_t fill;
} re15_sha256_t;

void re15_sha256_start(re15_sha256_t *c);
void re15_sha256_dazu(re15_sha256_t *c, const void *daten, size_t n);
void re15_sha256_ende(re15_sha256_t *c, char hex[65]);      /* 64 Zeichen 0-9a-f + NUL */

/* sha256 einer Datei (stdio). 0 = ok (hex + *groesse gesetzt), -1 = nicht lesbar. */
int  re15_sha256_datei(const char *pfad, char hex[65], long long *groesse);

/* ---------------------------------------------------------------------------- Liste */
typedef struct {
    long long groesse;
    char sha[65];
    const char *pfad;           /* zeigt in liste->puffer */
} re15_abgleich_eintrag_t;

typedef struct {
    char *puffer;               /* eigene Kopie des Textes (Zeilenenden durch NUL ersetzt) */
    re15_abgleich_eintrag_t *e; /* nach Pfad sortiert (strcmp, Bytes) */
    size_t n;
    long long summe;
} re15_abgleich_liste_t;

/* 0 = gueltig. Sonst < 0, l ist leer (re15_abgleich_frei trotzdem erlaubt) und fehler traegt den
 * Grund ("Zeile 7: ..."). RE15_ABGLEICH_ALTES_FORMAT = Kopfzeile v1 (bis v0.8.19). */
#define RE15_ABGLEICH_UNGUELTIG     (-1)
#define RE15_ABGLEICH_ALTES_FORMAT  (-2)
#define RE15_ABGLEICH_KEIN_SPEICHER (-3)
int  re15_abgleich_lesen(re15_abgleich_liste_t *l, const char *text, size_t len,
                         char *fehler, size_t fehler_n);
void re15_abgleich_frei(re15_abgleich_liste_t *l);
const re15_abgleich_eintrag_t *re15_abgleich_suchen(const re15_abgleich_liste_t *l, const char *pfad);
int  re15_abgleich_pfad_ok(const char *p, size_t n);      /* 1 = zulaessig (Regeln oben) */

/* ---------------------------------------------------------------------------- Abgleich */
enum {
    RE15_ABGLEICH_BEHALTEN  = 0,   /* in "zuletzt entpackt" mit gleicher Groesse UND Summe */
    RE15_ABGLEICH_GEAENDERT = 1,   /* in "zuletzt entpackt", aber Groesse oder Summe anders */
    RE15_ABGLEICH_NEU       = 2,   /* steht nicht in "zuletzt entpackt" */
    RE15_ABGLEICH_PRUEFEN   = 3    /* es gibt keine gueltige "zuletzt entpackt"-Liste */
};

typedef struct {
    unsigned char *aktion;      /* je Eintrag von neu, gleicher Index wie neu->e */
    size_t n_behalten, n_geaendert, n_neu, n_pruefen;
    const char **weg;           /* Pfade, die nur in alt stehen (zeigen in alt->puffer), sortiert */
    size_t n_weg;
} re15_abgleich_plan_t;

/* alt == NULL: keine gueltige "zuletzt entpackt"-Liste (Erstinstallation, v0.8.19-Marker, Liste
 * unlesbar, abgebrochener Lauf) -> jede Datei RE15_ABGLEICH_PRUEFEN, nichts zu loeschen.
 * 0 = ok, < 0 = kein Speicher (p ist dann leer). */
int  re15_abgleich_planen(re15_abgleich_plan_t *p, const re15_abgleich_liste_t *neu,
                          const re15_abgleich_liste_t *alt);
void re15_abgleich_plan_frei(re15_abgleich_plan_t *p);

/* Was mit EINER Datei geschieht, nachdem der Speicherordner befragt wurde.
 * groesse_ist < 0: Datei fehlt (oder ist keine regulaere Datei). */
enum {
    RE15_TUN_NICHTS        = 0,    /* liegt aktuell da */
    RE15_TUN_ENTPACKEN     = 1,    /* aus der APK neu schreiben */
    RE15_TUN_SUMME_PRUEFEN = 2     /* gleich gross, Stand unbekannt: sha256 der Datei gegen die Liste */
};
int  re15_abgleich_tun(int aktion, long long groesse_ist, long long groesse_soll);

/* ---------------------------------------------------------------------------- Waisen (Nachbesserung R4-1, U2/E1) */
/* Unter <wurzel>/<baum> (je Eintrag von baeume) jede Datei, deren Pfad relativ zu <wurzel> NICHT (bytegenau) in l
 * steht, loeschen - auch .neu-Reste und Symlinks (nie gefolgt) -, danach leere Unterordner (die Baeume selbst bleiben).
 * Gedacht fuer die Baeume, die allein dem Entpacker gehoeren (shared_assets, synchro: die Engine schreibt dort nichts).
 * Aufgerufen, wenn es keine gueltige "zuletzt entpackt"-Liste gibt (Erstinstallation, Uebergang v0.8.19, abgebrochener
 * Lauf) - dann weiss sonst niemand, was frueher entpackt wurde, und Dateien, die die neue Liste nicht mehr fuehrt,
 * blieben fuer immer liegen (Gegenpruefung R4-1: halbe CDEMD0.EMS.neu bzw. ganze CDEMD0.EMS als Waise).
 * melde(ctx, rel, ok) je Datei (ok 1 = geloescht, 0 = nicht loeschbar; melde darf NULL sein).
 * Rueckgabe: Zahl der geloeschten Dateien; *n_fehler (darf NULL sein) = nicht loeschbare Dateien + unlesbare Ordner. */
long re15_abgleich_waisen(const char *wurzel, const char *const *baeume, size_t n_baeume,
                          const re15_abgleich_liste_t *l,
                          void (*melde)(void *ctx, const char *rel, int ok), void *ctx, long *n_fehler);

/* ---------------------------------------------------------------------------- Weg frei (Runde 35 Spur N, F-Y4/H8) */
/* Datei <-> Ordner gleichen Namens im SELBEN Start heilen (Dossier analysis/befunde_runde35/N_android.md Punkt 2).
 * Bis dahin brach ein Update ab, wenn auf dem Weg einer Datei etwas mit falschem Typ lag (Ordner auf dem Zielnamen,
 * Datei auf einem Ordnernamen, Ordner auf dem Namen der Zwischendatei <ziel>.neu), und erst der naechste Start (ohne
 * Liste -> Waisen-Lauf) stellte den Stand her - bei <ziel>.neu nie.
 * Sicher, weil der Leser zwei Regeln erzwingt (re15_abgleich_lesen, gleich im Gate und in build.gradle):
 *   R1  kein Segment endet auf ".neu" (bis dahin nur der ganze Pfad) -> ein Ordner <ziel>.neu traegt nie Gelistetes;
 *   R2  kein Pfad ist zugleich Ordner eines anderen (ASCII-Gross/klein egal) -> ein Ordner auf einem Dateinamen und eine
 *       Datei auf einem Ordnernamen tragen nie Gelistetes.
 * re15_abgleich_weg_frei: vor dem Schreiben von <wurzel>/<rel> (rel nach re15_abgleich_pfad_ok)
 *   - ein Elternsegment, das es gibt, das aber kein echter Ordner ist (Datei, Symlink, ...)  -> unlink
 *   - <rel>.neu ist ein echter Ordner                                                       -> samt Inhalt loeschen
 *   - <rel> ist ein echter Ordner                                                           -> samt Inhalt loeschen
 *   Symlinks werden nie verfolgt (nur der Link selbst geloescht). melde(ctx, rel_des_konflikts, art, n_dateien, ok) je
 *   Konflikt (darf NULL sein). 0 = Weg frei, -1 = ein Konflikt liess sich nicht raeumen (oder rel unzulaessig). */
enum {
    RE15_KONFLIKT_ELTER_DATEI = 1,   /* Datei (o.ae.) liegt auf einem Ordnernamen des Pfads */
    RE15_KONFLIKT_ZIEL_ORDNER = 2,   /* Ordner liegt auf dem Dateinamen */
    RE15_KONFLIKT_NEU_ORDNER  = 3    /* Ordner liegt auf dem Namen der Zwischendatei <rel>.neu */
};
int  re15_abgleich_weg_frei(const char *wurzel, const char *rel,
                            void (*melde)(void *ctx, const char *rel, int art, long n_dateien, int ok), void *ctx);

/* Nach dem Loeschen von <wurzel>/<rel>: leer gewordene Elternordner entfernen (rmdir, nur wenn leer), hoechstens bis
 * unter das erste Segment (der Baum selbst bleibt, wie bei re15_abgleich_waisen). Rueckgabe: Zahl entfernter Ordner. */
long re15_abgleich_leere_eltern(const char *wurzel, const char *rel);

#ifdef __cplusplus
}
#endif
#endif /* RE15_ASSET_ABGLEICH_H */
