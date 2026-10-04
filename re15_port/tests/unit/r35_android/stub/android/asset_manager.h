/* Runde 35 Spur N: AAssetManager-Attrappe - liest "APK-Assets" aus dem Ordner $H_APK (Vorbild R4-2) */
#ifndef R35_STUB_AM_H
#define R35_STUB_AM_H
#include <stddef.h>
#include <sys/types.h>
typedef struct AAssetManager AAssetManager;
typedef struct AAsset AAsset;
enum { AASSET_MODE_STREAMING = 2 };
AAsset *AAssetManager_open(AAssetManager *m, const char *name, int mode);
off64_t AAsset_getLength64(AAsset *a);
int AAsset_read(AAsset *a, void *buf, size_t n);
void AAsset_close(AAsset *a);
#endif
