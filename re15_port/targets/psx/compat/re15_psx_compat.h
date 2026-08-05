/*
 * re15_psx_compat.h — Hosted-C-Luecken der PSX schliessen, OHNE die Basis anzufassen.
 * =============================================================================
 * Diese Datei wird im PSX-Build per -include VOR jede Uebersetzungseinheit
 * gezogen (siehe targets/psx/CMakeLists.txt). Sie existiert, weil die
 * gemeinsamen Engine-Quellen an einigen Stellen hosted-C benutzen, das eine
 * PlayStation nicht hat:
 *
 *   getenv()   — Debug-Schalter (RE15_*-Umgebungsvariablen)
 *   fopen()    — Datei-Diagnosen (Pose-Traces, Dumps)
 *   fprintf()  — Diagnose-Ausgaben nach stderr
 *
 * PSn00bSDK bringt printf/sprintf/snprintf mit, aber KEIN FILE, stderr, fopen,
 * fclose, fflush, fread, fwrite und kein getenv — das ist richtig so, denn ein
 * PSX-Programm hat weder Dateisystem-Schicht in der libc noch eine Umgebung.
 *
 * Semantik der Ersatz-Implementierungen (bewusst gewaehlt, nicht bloss Attrappen):
 *
 *   getenv()  -> immer NULL   => JEDER env-gesteuerte Debug-Pfad ist auf der
 *                               PSX aus. Genau das gewuenschte Verhalten.
 *   fopen()   -> immer NULL   => Datei-Diagnosen nehmen ihren Fehlerzweig; die
 *                               Aufrufer in der Basis pruefen alle auf NULL.
 *   fprintf() -> printf()     => Diagnosen gehen auf die PSX-TTY (sichtbar im
 *                               Emulator-Konsolenfenster), der Stream-Parameter
 *                               wird verworfen. Damit bleiben die Meldungen
 *                               erhalten statt lautlos zu verschwinden.
 *   fclose/fflush/fread/fwrite -> Nulloperationen mit plausiblem Rueckgabewert.
 *
 * WICHTIG: Hier wird NICHTS deklariert, was PSn00bSDK schon hat (printf,
 * sprintf, snprintf, vsnprintf, vsprintf) — sonst gaebe es Konflikte.
 */
#ifndef RE15_PSX_COMPAT_H
#define RE15_PSX_COMPAT_H

#include <stdarg.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Undurchsichtiger Stream-Typ. Die Basis benutzt FILE* nur als Handle und
 * vergleicht gegen NULL — mehr braucht es nicht. */
typedef struct re15_psx_FILE re15_psx_FILE;
#define FILE re15_psx_FILE

/* Die Standard-Streams existieren als eindeutige, aber nie dereferenzierte
 * Adressen. fprintf() verwirft sie ohnehin. */
extern FILE *const re15_psx_stdout;
extern FILE *const re15_psx_stderr;
#define stdout re15_psx_stdout
#define stderr re15_psx_stderr

/* Kein Dateisystem: liefert immer NULL, der Aufrufer nimmt den Fehlerzweig. */
FILE *re15_psx_fopen(const char *path, const char *mode);
#define fopen  re15_psx_fopen

int  re15_psx_fclose(FILE *f);
#define fclose re15_psx_fclose

int  re15_psx_fflush(FILE *f);
#define fflush re15_psx_fflush

unsigned int re15_psx_fread (void *p, unsigned int sz, unsigned int n, FILE *f);
#define fread  re15_psx_fread

unsigned int re15_psx_fwrite(const void *p, unsigned int sz, unsigned int n, FILE *f);
#define fwrite re15_psx_fwrite

/* Leitet auf printf um (Stream wird verworfen) -> Diagnosen auf der TTY. */
int  re15_psx_fprintf(FILE *f, const char *fmt, ...);
#define fprintf re15_psx_fprintf

/* Keine Umgebung: alle RE15_*-Debug-Schalter sind auf der PSX aus. */
char *re15_psx_getenv(const char *name);
#define getenv re15_psx_getenv

#ifdef __cplusplus
}
#endif

#endif /* RE15_PSX_COMPAT_H */
