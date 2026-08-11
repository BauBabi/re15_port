/* sdl_blend_probe.c — Belegt, warum die Deck-Startskripte SDL_RENDER_DRIVER=opengles2 setzen.
 *
 * Bauen (Beispiel: SDL2 aus dem FetchContent-Baum eines vorhandenen Builds):
 *   gcc -O1 -I<build>/_deps/sdl2-src/include -o sdl_blend_probe re15_port/tools/sdl_blend_probe.c \
 *       <build>/_deps/sdl2-build/libSDL2.a -lm -ldl -lpthread -lrt
 *
 * Messung auf Steam Deck / SteamOS holo, Mesa, 2026-08-11:
 *   opengl     -> aktiv=opengl     REV_SUBTRACT rc=-1  "That operation is not supported"
 *   opengles2  -> aktiv=opengles2  REV_SUBTRACT rc=0   OK
 *   opengles   -> aktiv=opengl     REV_SUBTRACT rc=-1
 *   vulkan     -> aktiv=opengl     REV_SUBTRACT rc=-1
 *   software   -> aktiv=software   REV_SUBTRACT rc=-1
 * Nur opengles2 nimmt den Blend an; SDLs Default (opengl) lehnt ihn ab -> der vorherige
 * Blend bleibt aktiv und Fades/Balken/Schatten werden DECKEND (weiss) gezeichnet.
 *
 * Misst auf DIESER Hardware, welches SDL-Render-Backend den subtraktiven
 * Custom-Blend (SDL_BLENDOPERATION_REV_SUBTRACT) wirklich annimmt — genau der
 * Blend, mit dem der Port Fades-to-black, Cutscene-Balken und Schatten zeichnet
 * (render_pc.c:517/703/763/864). Backend per SDL_RENDER_DRIVER, Ergebnis:
 * Rueckgabewert von SDL_SetRenderDrawBlendMode(). != 0 -> das Backend kann es
 * NICHT, der vorherige Blend bleibt stehen -> die Quelle wird deckend (weiss)
 * gezeichnet. */
#include <SDL.h>
#include <stdio.h>

int main(void)
{
    static const char *drv[] = { "opengl", "opengles2", "opengles", "vulkan", "software", NULL };
    for (int i = 0; drv[i]; i++) {
        SDL_setenv("SDL_RENDER_DRIVER", drv[i], 1);
        if (SDL_Init(SDL_INIT_VIDEO) != 0) { printf("%-10s INIT-FAIL %s\n", drv[i], SDL_GetError()); continue; }
        SDL_Window *w = SDL_CreateWindow("probe", 0, 0, 64, 64, SDL_WINDOW_HIDDEN);
        SDL_Renderer *r = w ? SDL_CreateRenderer(w, -1, 0) : NULL;
        if (!r) { printf("%-10s KEIN RENDERER (%s)\n", drv[i], SDL_GetError()); }
        else {
            SDL_RendererInfo info; SDL_GetRendererInfo(r, &info);
            SDL_BlendMode sub = SDL_ComposeCustomBlendMode(
                SDL_BLENDFACTOR_ONE, SDL_BLENDFACTOR_ONE, SDL_BLENDOPERATION_REV_SUBTRACT,
                SDL_BLENDFACTOR_ZERO, SDL_BLENDFACTOR_ONE, SDL_BLENDOPERATION_ADD);
            int rc = SDL_SetRenderDrawBlendMode(r, sub);
            printf("%-10s -> aktiv=%-10s REV_SUBTRACT rc=%d %s%s\n", drv[i], info.name, rc,
                   rc == 0 ? "OK" : "ABGELEHNT: ", rc == 0 ? "" : SDL_GetError());
            SDL_DestroyRenderer(r);
        }
        if (w) SDL_DestroyWindow(w);
        SDL_Quit();
    }
    return 0;
}
