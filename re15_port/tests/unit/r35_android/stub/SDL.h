/* Runde 35 Spur N: SDL-Attrappe fuer den Pruefstand des ECHTEN android_glue.c (nur was es benutzt).
 * Vorbild: Gegenpruefung R4-2 (analysis/befunde_runde34_android/pruefer_umgehung_r4_2_belege/pruefstand/stub). */
#ifndef R35_STUB_SDL_H
#define R35_STUB_SDL_H
#include <stdint.h>
typedef uint32_t Uint32;
typedef uint8_t Uint8;
typedef struct SDL_Renderer SDL_Renderer;
typedef struct { int x, y, w, h; } SDL_Rect;
typedef struct { Uint32 type; } SDL_Event;
enum { SDL_QUIT = 0x100, SDL_APP_TERMINATING = 0x101 };
enum { SDL_BLENDMODE_NONE = 0, SDL_BLENDMODE_BLEND = 1 };
#define SDL_ANDROID_EXTERNAL_STORAGE_READ 1
#define SDL_ANDROID_EXTERNAL_STORAGE_WRITE 2
Uint32 SDL_GetTicks(void);
void SDL_Delay(Uint32 ms);
int SDL_PollEvent(SDL_Event *e);
void SDL_PumpEvents(void);
int SDL_GetRendererOutputSize(SDL_Renderer *r, int *w, int *h);
void SDL_RenderGetLogicalSize(SDL_Renderer *r, int *w, int *h);
int SDL_RenderSetLogicalSize(SDL_Renderer *r, int w, int h);
int SDL_SetRenderDrawBlendMode(SDL_Renderer *r, int m);
int SDL_SetRenderDrawColor(SDL_Renderer *r, Uint8 a, Uint8 b, Uint8 c, Uint8 d);
int SDL_RenderClear(SDL_Renderer *r);
int SDL_RenderFillRect(SDL_Renderer *r, const SDL_Rect *rc);
void SDL_RenderPresent(SDL_Renderer *r);
int SDL_AndroidGetExternalStorageState(void);
const char *SDL_AndroidGetExternalStoragePath(void);
const char *SDL_AndroidGetInternalStoragePath(void);
void *SDL_AndroidGetJNIEnv(void);
void *SDL_AndroidGetActivity(void);
#endif
