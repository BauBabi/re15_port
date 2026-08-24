/*
 * RE1.5 Rebuilt — PC asset loader (Phase 4.3, 2026-05-18).
 *
 * Reads files from disk (extracted RE1.5 assets in ../assets/).
 * TIM blit into the software framebuffer.
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>   /* strlen — Asset-Wurzel-Fallback */
#include "re15_engine.h"
#include "re15_tim.h"
#include "asset_root_pc.h"

/* Defined in render_pc.c — exposed via extern for now (Phase 4.4 will refactor) */
extern uint32_t *re15_pc_framebuffer(void);
extern void      re15_pc_put_pixel(int x, int y, uint32_t rgba);

/* Read entire file into a malloc'd buffer. Returns NULL on failure. */
uint8_t *re15_asset_read_file(const char *path, int *out_size)
{
    /* ⛔ REIHENFOLGE (korrigiert 2026-08-24, Skeptiker-Befund): frueher lief ZUERST das blanke
     * fopen(path) — bei einem RELATIVEN Pfad also gegen das ARBEITSVERZEICHNIS — und die
     * verankerten Wurzeln kamen erst danach. Gemessen: startet man die exe per Verknuepfung mit
     * einem fremden Arbeitsverzeichnis, das SELBST einen shared_assets-Baum traegt, gewann
     * dieser fremde Baum gegen das Paket neben der exe (nachgewiesen an DATA/ITEMALL.PIX und
     * ITEM/ITPS.ITP) — eine stille Vermischung zweier Asset-Baeume, die niemand bemerkt.
     * Jetzt gilt fuer relative Pfade: erst die Wurzelkette (exe-Verzeichnis zuerst), das
     * Arbeitsverzeichnis nur noch als letzte Zuflucht. Absolute Pfade bleiben unberuehrt. */
    if (path && path[0] && path[1] != ':' && path[0] != '/' && path[0] != '\\') {
        /* CWD-UNABHAENGIGKEIT (2026-08-01, Nutzer-Report "BGM und Soundeffekte funktionieren nicht,
         * lediglich die Voiceover"). Diese Funktion war ein blankes fopen — relative Pfade wurden
         * also gegen das ARBEITSVERZEICHNIS aufgeloest, nicht gegen die Asset-Wurzel. Der Rest des
         * Ports liest ueber pc_read_shared() (main.c), das RE15_ASSET_ROOT bzw. den einkompilierten
         * Default voranstellt; das AUDIO-Backend umgeht das und benutzt fest verdrahtete relative
         * Verzeichnislisten ("shared_assets/PSX/SOUND/", "SOUND/", "PSX/SOUND/", ...). Je nachdem,
         * von wo die .exe gestartet wird, findet es seine Baenke damit nicht — waehrend alles, was
         * ueber pc_read_shared laeuft, tadellos funktioniert. Genau dieses Muster hat der Nutzer
         * beschrieben.
         *
         * Fix: schlaegt der literale Pfad fehl, ein zweiter Versuch unter der Asset-Wurzel. Der
         * erste Versuch bleibt unveraendert, es kann also nichts kaputtgehen, was heute geht. */
        /* 2026-08-24: der Fallback ging frueher NUR gegen RE15_ASSET_ROOT bzw. den
         * EINKOMPILIERTEN Default. Im ausgelieferten Paket zeigt dieser Default auf einen
         * Pfad, den es auf der Nutzer-Maschine nicht gibt (Docker: "/src/re15_port/..."),
         * also blieb als einzige Chance der literale cwd-Versuch oben — und "SOUND/MAIN28.BGM"
         * gibt es cwd-relativ nicht (die Datei liegt unter shared_assets/PSX/SOUND/). Jetzt
         * uebernimmt der gemeinsame Wurzel-Helfer: er kennt das exe-Verzeichnis, deckt CD-,
         * Shared- und Basis-Wurzel ab und faengt damit ALLE relativen Pfad-Bauarten
         * ("DATA/…", "extracted_fx/…", "shared_assets/PSX/…", "synchro/…") in einem Aufruf. */
        int rsz = 0;
        uint8_t *rb = re15_pc_read_any(path, &rsz);
        if (rb) {
            if (out_size) *out_size = rsz;
            return rb;
        }
    }
    FILE *f = fopen(path, "rb");
    if (!f) {
        fprintf(stderr, "[asset] cannot open %s\n", path);
        return NULL;
    }
    fseek(f, 0, SEEK_END);
    long sz = ftell(f);
    fseek(f, 0, SEEK_SET);
    if (sz <= 0) {
        fclose(f);
        return NULL;
    }
    uint8_t *buf = (uint8_t *) malloc((size_t)sz);
    if (!buf) {
        fclose(f);
        return NULL;
    }
    fread(buf, 1, (size_t)sz, f);
    fclose(f);
    if (out_size) *out_size = (int)sz;
    return buf;
}

/* Ein Texel dieses 2D-Blits -> RGBA8888 des Software-Framebuffers.
 *
 * DIE ENTSCHEIDUNG "zeichnen oder nicht" kommt aus der EINEN Regel in re15_tim.h
 * (`re15_tim_texel_argb`, ARGB; hier nur die Kanal-Reihenfolge gedreht) — dieser Blit
 * hatte bis 2026-08-21 eine EIGENE, ANDERE Regel: er verwarf Texel nach dem PALETTEN-
 * INDEX (`if (idx == 0) continue`). Das ist genau der Mechanismus, der Modelltexturen
 * Loecher reisst, denn Index 0 ist dort eine echte, sichtbare Farbe (gemessen mit
 * tests/unit/probe_texel_key_census: EM45 3635, EM42 1787, Elliot/EM47 478 gesampelte
 * Index-0-Texel mit CLUT-Wert != 0). Die Hardware keyt am aufgeloesten WERT
 * (psx-spx: Color 0000h = Fully-transparent), nicht am Index. */
static uint32_t tim_texel_rgba(uint16_t c)
{
    uint32_t argb = re15_tim_texel_argb(c);
    if (argb == 0u) return 0x00000000u;                 /* nicht zeichnen */
    return ((argb & 0x00FF0000u) << 8)                  /* R */
         | ((argb & 0x0000FF00u) << 8)                  /* G */
         | ((argb & 0x000000FFu) << 8)                  /* B */
         | 0xFFu;                                       /* A */
}

void re15_tim_blit_pc(const re15_tim_t *tim, int dst_x, int dst_y)
{
    if (!tim) return;

    if (tim->bpp == 8 && tim->has_clut) {
        /* 8bpp: 1 byte per pixel, indexed into CLUT */
        const uint8_t *src = (const uint8_t *) tim->pixels;
        for (int y = 0; y < tim->height; y++) {
            for (int x = 0; x < tim->width; x++) {
                uint32_t px = tim_texel_rgba(tim->clut[src[y * tim->width + x]]);
                if (px == 0u) continue;
                re15_pc_put_pixel(dst_x + x, dst_y + y, px);
            }
        }
    } else if (tim->bpp == 4 && tim->has_clut) {
        /* 4bpp: 2 pixels per byte (low nibble first) */
        const uint8_t *src = (const uint8_t *) tim->pixels;
        for (int y = 0; y < tim->height; y++) {
            for (int x = 0; x < tim->width; x++) {
                int byte_idx = (y * tim->width + x) / 2;
                int nibble = (x & 1) ? (src[byte_idx] >> 4) : (src[byte_idx] & 0xF);
                uint32_t px = tim_texel_rgba(tim->clut[nibble]);
                if (px == 0u) continue;
                re15_pc_put_pixel(dst_x + x, dst_y + y, px);
            }
        }
    } else if (tim->bpp == 16) {
        /* 16bpp direct color RGB555 */
        const uint16_t *src = tim->pixels;
        for (int y = 0; y < tim->height; y++) {
            for (int x = 0; x < tim->width; x++) {
                uint32_t px = tim_texel_rgba(src[y * tim->width + x]);
                if (px == 0u) continue;
                re15_pc_put_pixel(dst_x + x, dst_y + y, px);
            }
        }
    } else {
        fprintf(stderr, "[tim] unsupported bpp=%d clut=%d\n", tim->bpp, tim->has_clut);
    }
}
