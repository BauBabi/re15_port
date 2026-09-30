/* =============================================================================================
 * RE1.5 Rebuilt — Android: Asset-Liste lesen und gegen "zuletzt entpackt" abgleichen
 * (Runde 34a, N1 "Geraete-Entpacker update-sicher", 2026-09-30)
 *
 * PORT-WAHL, kein Originalverhalten: die PSX las von CD, der PC-Port liest den Paketordner.
 * Nur die Android-App muss ihre Assets aus der APK in den App-Speicherordner entpacken
 * (android_glue.c re15_android_bootstrap_assets). Diese Datei ist der reine C-Teil davon -
 * ohne SDL, ohne Android -, damit ein PC-Unit-Test (tests/unit/test_r34a_asset_abgleich.c,
 * Sonde probes/r34a_android.cmake) GENAU diesen Code uebersetzt und prueft.
 *
 * WARUM (Befund N1, Nachbesserung R2): bis v0.8.19 trug die Liste nur "<bytes>\t<pfad>", und der
 * Marker re15_assets_ok.txt war ein FNV-1a ueber diese Liste. Aenderte ein Update eine Datei bei
 * GLEICHER Groesse (P07G.DO2 in Runde 33 dreimal bei 55908 B), blieb die Liste bytegleich, das
 * Entpacken wurde ganz uebersprungen, und die alte Datei blieb auf dem Geraet.
 *
 * FORMAT v2 (Schreiber: platform/android/app/build.gradle writeAssetManifest; Pruefer:
 * release/apk_asset_gate.py manifest_pruefen - liest nach DENSELBEN Regeln wie re15_abgleich_lesen):
 *     # re15 assets v2 <anzahl> <bytes>\n              Kopfzeile, IMMER die erste Zeile
 *     <bytes>\t<sha256, 64 Zeichen 0-9a-f>\t<pfad>\n  je Datei (Schreiber sortiert nach Pfad)
 *   - Zeilen an '\n' getrennt; angehaengte '\r' werden abgeschnitten, leere Zeilen uebersprungen.
 *   - Kopfzeile: genau "# re15 assets v2 " + 1-18 Ziffern + ' ' + 1-18 Ziffern. Die alte Kopfzeile
 *     "# re15 assets <n> <b>" (v1, bis v0.8.19) wird erkannt und ABGELEHNT (eigene Meldung).
 *   - Datenzeile: 1-18 Ziffern, Tab, 64 Zeichen [0-9a-f] (Grossbuchstaben NICHT), Tab, Pfad.
 *   - Pfad: nicht leer, relativ, mindestens ein '/' (Assets liegen nie direkt im Speicherordner -
 *     dort liegen Logs, Spielstand und diese Listen), kein '\\', keine Steuerzeichen (< 0x20,
 *     0x7f), kein leeres/'.'/'..'-Segment, endet nicht auf ".neu" (Endung der Zwischendatei).
 *   - weitere '#'-Zeilen, doppelte Pfade, NUL-Bytes, Kopfzeile passt nicht zu den Zeilen,
 *     Liste > 64 MiB -> die GANZE Liste ist ungueltig (fail closed: lieber gar nicht starten als
 *     still mit einem Loch im Asset-Baum).
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
 * unlesbar) -> jede Datei RE15_ABGLEICH_PRUEFEN, nichts zu loeschen. 0 = ok, < 0 = kein Speicher. */
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

#ifdef __cplusplus
}
#endif
#endif /* RE15_ASSET_ABGLEICH_H */
