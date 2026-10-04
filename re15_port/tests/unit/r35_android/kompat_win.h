/* Runde 35 Spur N: POSIX-Ersatz, damit der ECHTE platform/android/jni/android_glue.c auch unter mingw (Windows-Suite)
 * uebersetzt und laeuft. Nur fuer den Pruefstand, per "-include" vor android_glue.c gezogen (probes/r35_android.cmake).
 * Unter Linux/macOS leer - dort gilt POSIX unveraendert.
 *   mkdir(p, mode)   mingw kennt nur mkdir(p)
 *   rename(a, b)     POSIX ersetzt ein vorhandenes Ziel (Android: f2fs/ext4/FUSE), msvcrt bricht mit EEXIST ab -> MoveFileEx
 *                    mit MOVEFILE_REPLACE_EXISTING; ein ORDNER als Ziel scheitert wie unter POSIX (EISDIR)
 *   fsync / sync     _commit bzw. nichts (Haltbarkeit ist auf dem Pruefstand ohne Belang)
 *   O_CLOEXEC, O_DIRECTORY  gibt es nicht -> 0 (open() auf einen Ordner scheitert unter Windows; ordner_haltbar tut dann nichts)
 * Textmodus: der Pruefstand setzt _fmode auf binaer (pruefstand_main.c), sonst schriebe open() CRLF. */
#ifndef R35_KOMPAT_WIN_H
#define R35_KOMPAT_WIN_H
#if defined(_WIN32)
#  include <io.h>
#  include <direct.h>
#  include <fcntl.h>
#  include <stdio.h>
#  include <sys/stat.h>
#  include <sys/types.h>
#  include <unistd.h>
int  r35_mkdir(const char *p, int mode);
int  r35_rename(const char *a, const char *b);
int  r35_fsync(int fd);
void r35_sync(void);
#  define mkdir(p, m)  r35_mkdir((p), (m))
#  define rename(a, b) r35_rename((a), (b))
#  define fsync(fd)    r35_fsync(fd)
#  define sync()       r35_sync()
#  ifndef O_CLOEXEC
#    define O_CLOEXEC 0
#  endif
#  ifndef O_DIRECTORY
#    define O_DIRECTORY 0
#  endif
#endif
#endif
