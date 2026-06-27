/**
 * stub.c — Platzhalter-Datei für die Engine-Library.
 *
 * Diese Datei stellt sicher, dass der GLOB-Mechanismus in engine/CMakeLists.txt
 * mindestens eine Quelldatei findet und die STATIC Library korrekt erstellt
 * werden kann. Kann entfernt werden, sobald echte Engine-Quelldateien vorhanden
 * sind.
 *
 * Enthält außerdem globale Extern-Definitionen, die von mehreren Subsystemen
 * referenziert werden und deren eigentliche Initialisierung in den
 * plattformspezifischen Backends erfolgt.
 */

#include "re15_audio.h"

/* Audio-Backend-Zeiger — wird von platform/psx oder platform/pc gesetzt */
const audio_backend_t* g_audio = NULL;

