/* sdl_blend_probe.c — Belegt, warum die Deck-Startskripte SDL_RENDER_DRIVER=opengles2 setzen
 * und warum das Windows-Startskript KEIN Backend erzwingt.
 *
 * Bauen (SDL2 aus dem FetchContent-Baum eines vorhandenen Builds):
 *   Linux:
 *     gcc -O1 -I<build>/_deps/sdl2-src/include -o sdl_blend_probe re15_port/tools/sdl_blend_probe.c \
 *         <build>/_deps/sdl2-build/libSDL2.a -lm -ldl -lpthread -lrt
 *   Windows (mingw64) — SDL_MAIN_HANDLED steht unten im Code, deshalb OHNE libSDL2main:
 *     gcc -O1 -I<build>/_deps/sdl2-src/include -o sdl_blend_probe.exe re15_port/tools/sdl_blend_probe.c \
 *         <build>/_deps/sdl2-build/libSDL2.a -lm -ldinput8 -ldxguid -luser32 -lgdi32 -lwinmm \
 *         -limm32 -lole32 -loleaut32 -lshell32 -lsetupapi -lversion -luuid -ladvapi32 -lkernel32
 *
 * Messung auf Steam Deck / SteamOS holo, Mesa, 2026-08-11:
 *   opengl     -> aktiv=opengl     REV_SUBTRACT rc=-1  "That operation is not supported"
 *   opengles2  -> aktiv=opengles2  REV_SUBTRACT rc=0   OK
 *   opengles   -> aktiv=opengl     REV_SUBTRACT rc=-1
 *   vulkan     -> aktiv=opengl     REV_SUBTRACT rc=-1
 *   software   -> aktiv=software   REV_SUBTRACT rc=-1
 * Nur opengles2 nimmt den Blend an; SDLs Default (opengl) lehnt ihn ab -> der vorherige
 * Blend bleibt aktiv und Fades/Balken/Schatten werden DECKEND (weiss) gezeichnet.
 * Deshalb setzt re15-deck.sh / pkg_files/linux/run.sh SDL_RENDER_DRIVER=opengles2.
 *
 * Messung auf Windows 11 x64, mingw64 GCC 15.2, 2026-08-11:
 *   (default)  -> aktiv=direct3d   REV_SUBTRACT rc=0   OK
 *   direct3d   -> aktiv=direct3d   REV_SUBTRACT rc=0   OK
 *   direct3d11 -> aktiv=direct3d11 REV_SUBTRACT rc=0   OK
 *   direct3d12 -> aktiv=direct3d12 REV_SUBTRACT rc=0   OK
 *   opengl     -> aktiv=opengl     REV_SUBTRACT rc=-1  "That operation is not supported"
 *   opengles2  -> aktiv=opengles2  REV_SUBTRACT rc=0   OK
 *   software   -> aktiv=software   REV_SUBTRACT rc=-1  "That operation is not supported"
 * Windows waehlt per Default direct3d, und das kann den Blend -> der Deck-Fix
 * (SDL_RENDER_DRIVER erzwingen) wird dort NICHT gebraucht; Start_RE15_Port.bat setzt
 * bewusst kein Backend. Der Defekt existiert auf Windows nur, wenn jemand von aussen
 * SDL_RENDER_DRIVER=opengl (oder software) setzt.
 *
 * Misst auf DIESER Hardware, welches SDL-Render-Backend den subtraktiven
 * Custom-Blend (SDL_BLENDOPERATION_REV_SUBTRACT) wirklich annimmt — genau der
 * Blend, mit dem der Port Fades-to-black, Cutscene-Balken und Schatten zeichnet
 * (render_pc.c:517/703/763/864). Backend per SDL_RENDER_DRIVER, Ergebnis:
 * Rueckgabewert von SDL_SetRenderDrawBlendMode(). != 0 -> das Backend kann es
 * NICHT, der vorherige Blend bleibt stehen -> die Quelle wird deckend (weiss)
 * gezeichnet. */
#define SDL_MAIN_HANDLED   /* eigenes main(): SDL soll main nicht auf SDL_main umbiegen
                            * (sonst verlangt der mingw-Link libSDL2main + WinMain) */
#include <SDL.h>
#include <stdio.h>

/* Prueft den aktuell per SDL_RENDER_DRIVER eingestellten Zustand. Bewusst mit
 * frischem Init/Quit je Aufruf: SDL liest den Treiber-Hint beim Renderer-Bau. */
static void probe_current(const char *label)
{
    if (SDL_Init(SDL_INIT_VIDEO) != 0) { printf("%-11s INIT-FAIL %s\n", label, SDL_GetError()); return; }
    SDL_Window *w = SDL_CreateWindow("probe", 0, 0, 64, 64, SDL_WINDOW_HIDDEN);
    SDL_Renderer *r = w ? SDL_CreateRenderer(w, -1, 0) : NULL;
    if (!r) { printf("%-11s KEIN RENDERER (%s)\n", label, SDL_GetError()); }
    else {
        SDL_RendererInfo info; SDL_GetRendererInfo(r, &info);
        SDL_BlendMode sub = SDL_ComposeCustomBlendMode(
            SDL_BLENDFACTOR_ONE, SDL_BLENDFACTOR_ONE, SDL_BLENDOPERATION_REV_SUBTRACT,
            SDL_BLENDFACTOR_ZERO, SDL_BLENDFACTOR_ONE, SDL_BLENDOPERATION_ADD);
        int rc = SDL_SetRenderDrawBlendMode(r, sub);
        printf("%-11s -> aktiv=%-11s REV_SUBTRACT rc=%d %s%s\n", label, info.name, rc,
               rc == 0 ? "OK" : "ABGELEHNT: ", rc == 0 ? "" : SDL_GetError());
        SDL_DestroyRenderer(r);
    }
    if (w) SDL_DestroyWindow(w);
    SDL_Quit();
}

int main(void)
{
    SDL_SetMainReady();   /* Pflicht bei SDL_MAIN_HANDLED */

    /* ZUERST der Default-Pfad — noch ohne je SDL_RENDER_DRIVER gesetzt zu haben,
     * also genau das Backend, das re15_pc ohne Startskript waehlt. Das ist die
     * eigentliche Frage: muss die Plattform ein Backend erzwingen oder nicht? */
    probe_current("(default)");

    /* Danach jedes Backend, das DIESE SDL-Build wirklich anbietet. Die frueher hier
     * fest verdrahtete Liste war Deck-geformt und enthielt kein direct3d* — auf
     * Windows blieb damit ausgerechnet das Default-Backend ungemessen. */
    char names[16][32]; int cnt = 0;
    if (SDL_Init(SDL_INIT_VIDEO) != 0) { printf("INIT-FAIL %s\n", SDL_GetError()); return 1; }
    int n = SDL_GetNumRenderDrivers();
    for (int i = 0; i < n && cnt < 16; i++) {
        SDL_RendererInfo ri;
        if (SDL_GetRenderDriverInfo(i, &ri) == 0) { snprintf(names[cnt], sizeof names[0], "%s", ri.name); cnt++; }
    }
    SDL_Quit();

    for (int i = 0; i < cnt; i++) {
        SDL_setenv("SDL_RENDER_DRIVER", names[i], 1);
        probe_current(names[i]);
    }
    return 0;
}
