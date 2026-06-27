/**
 * @file debug_overlay.c
 * @brief Debug-Overlays für PC-Build — Implementierung
 *
 * Rendert visuelle Debug-Informationen als Overlays auf dem Spielbild:
 *   F1: SCA-Kollisionsboxen als Drahtgitter (weiß=Rect, cyan=Kreis, gelb=Treppe)
 *   F2: AOT-Zonen farbcodiert (blau=Tür, grün=Item, gelb=Event)
 *   F3: RVD-Kamerazonen mit Kamera-ID-Text (magenta)
 *   F4: SCD-Trace — letzte 10 ausgeführte Opcodes als Text
 *   F5: RAM-Belegung als horizontaler Balken
 *
 * Alle Overlays rendern nach dem Spielinhalt (Z=0 / on top).
 * Guarded durch RE15_PLATFORM_PC — kompiliert nur im PC-Build.
 *
 * Validates: Requirements 12.6
 */

#ifdef RE15_PLATFORM_PC

#include <SDL.h>
#include <stdio.h>
#include <string.h>

#include "debug_overlay.h"
#include "re15_engine.h"
#include "re15_collision.h"
#include "re15_aot.h"
#include "re15_camera.h"
#include "re15_scd.h"

/* ============================================================================
 * Interner Zustand
 * ========================================================================= */

/** Bitmaske der aktiven Overlays */
static uint32_t s_active_overlays = 0;

/** SCD-Trace Ringpuffer */
static debug_scd_trace_entry_t s_scd_trace[DEBUG_SCD_TRACE_SIZE];
static int s_scd_trace_write_idx = 0;
static int s_scd_trace_count = 0;

/* ============================================================================
 * Interne Hilfsfunktionen: Minimaler Bitmap-Zeichensatz
 *
 * Ein einfacher 4x6-Pixel Zeichensatz für Debug-Text. Rendert Zeichen
 * pixelweise über SDL_RenderDrawPoint. Unterstützt 0-9, A-F, Leerzeichen,
 * und einige Sonderzeichen für Hex-Ausgabe.
 * ========================================================================= */

/* 4x6 Bitmap-Glyphen für '0'-'9', 'A'-'F', ':', ' ', '[', ']', 'x', '-' */
static const uint8_t s_font_glyphs[][6] = {
    /* 0 */ {0x6, 0x9, 0xB, 0xD, 0x9, 0x6},
    /* 1 */ {0x2, 0x6, 0x2, 0x2, 0x2, 0x7},
    /* 2 */ {0x6, 0x9, 0x2, 0x4, 0x8, 0xF},
    /* 3 */ {0xF, 0x2, 0x6, 0x1, 0x9, 0x6},
    /* 4 */ {0x2, 0x6, 0xA, 0xF, 0x2, 0x2},
    /* 5 */ {0xF, 0x8, 0xE, 0x1, 0x9, 0x6},
    /* 6 */ {0x6, 0x8, 0xE, 0x9, 0x9, 0x6},
    /* 7 */ {0xF, 0x1, 0x2, 0x4, 0x4, 0x4},
    /* 8 */ {0x6, 0x9, 0x6, 0x9, 0x9, 0x6},
    /* 9 */ {0x6, 0x9, 0x9, 0x7, 0x1, 0x6},
    /* A */ {0x6, 0x9, 0x9, 0xF, 0x9, 0x9},
    /* B */ {0xE, 0x9, 0xE, 0x9, 0x9, 0xE},
    /* C */ {0x6, 0x9, 0x8, 0x8, 0x9, 0x6},
    /* D */ {0xE, 0x9, 0x9, 0x9, 0x9, 0xE},
    /* E */ {0xF, 0x8, 0xE, 0x8, 0x8, 0xF},
    /* F */ {0xF, 0x8, 0xE, 0x8, 0x8, 0x8},
    /* : */ {0x0, 0x4, 0x0, 0x0, 0x4, 0x0},
    /*   */ {0x0, 0x0, 0x0, 0x0, 0x0, 0x0},
    /* [ */ {0x6, 0x4, 0x4, 0x4, 0x4, 0x6},
    /* ] */ {0x6, 0x2, 0x2, 0x2, 0x2, 0x6},
    /* x */ {0x0, 0x0, 0x9, 0x6, 0x6, 0x9},
    /* - */ {0x0, 0x0, 0x0, 0xF, 0x0, 0x0},
    /* N */ {0x9, 0xD, 0xD, 0xB, 0xB, 0x9},
    /* o */ {0x0, 0x0, 0x6, 0x9, 0x9, 0x6},
    /* p */ {0x0, 0x0, 0xE, 0x9, 0xE, 0x8},
    /* S */ {0x6, 0x8, 0x6, 0x1, 0x9, 0x6},
    /* L */ {0x8, 0x8, 0x8, 0x8, 0x8, 0xF},
    /* P */ {0xE, 0x9, 0x9, 0xE, 0x8, 0x8},
    /* / */ {0x1, 0x1, 0x2, 0x4, 0x8, 0x8},
    /* M */ {0x9, 0xF, 0xF, 0x9, 0x9, 0x9},
    /* R */ {0xE, 0x9, 0x9, 0xE, 0xA, 0x9},
    /* K */ {0x9, 0xA, 0xC, 0xC, 0xA, 0x9},
    /* T */ {0xF, 0x4, 0x4, 0x4, 0x4, 0x4},
    /* I */ {0xE, 0x4, 0x4, 0x4, 0x4, 0xE},
    /* % */ {0x9, 0x1, 0x2, 0x4, 0x8, 0x9},
};

/**
 * Mappt ein Zeichen auf den Index in der Glyph-Tabelle.
 * Gibt -1 zurück wenn das Zeichen nicht unterstützt wird.
 */
static int font_char_index(char c)
{
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'A' && c <= 'F') return 10 + (c - 'A');
    if (c >= 'a' && c <= 'f') return 10 + (c - 'a');
    switch (c) {
        case ':': return 16;
        case ' ': return 17;
        case '[': return 18;
        case ']': return 19;
        case 'x': return 20;
        case '-': return 21;
        case 'N': case 'n': return 22;
        case 'o': case 'O': return 23;
        case 'p': return 24;
        case 'S': case 's': return 25;
        case 'L': case 'l': return 26;
        case 'P': return 27;
        case '/': return 28;
        case 'M': case 'm': return 29;
        case 'R': case 'r': return 30;
        case 'K': case 'k': return 31;
        case 'T': case 't': return 32;
        case 'I': case 'i': return 33;
        case '%': return 34;
        default:  return 17; /* Leerzeichen als Fallback */
    }
}

/**
 * Zeichnet einen einzelnen Buchstaben an (x, y).
 * Glyph-Größe: 4×6 Pixel.
 */
static void draw_char(SDL_Renderer* renderer, int x, int y, char c)
{
    int idx = font_char_index(c);
    int row, col;

    if (idx < 0 || idx >= (int)(sizeof(s_font_glyphs) / sizeof(s_font_glyphs[0]))) {
        return;
    }

    for (row = 0; row < 6; row++) {
        uint8_t bits = s_font_glyphs[idx][row];
        for (col = 0; col < 4; col++) {
            if (bits & (0x8 >> col)) {
                SDL_RenderDrawPoint(renderer, x + col, y + row);
            }
        }
    }
}

/**
 * Zeichnet einen Null-terminierten String an (x, y).
 * Zeichenabstand: 5 Pixel (4 + 1 Lücke).
 */
static void draw_text(SDL_Renderer* renderer, int x, int y, const char* text)
{
    while (*text) {
        draw_char(renderer, x, y, *text);
        x += 5;
        text++;
    }
}

/* ============================================================================
 * Interne Render-Funktionen pro Overlay
 * ========================================================================= */

/* Speicherbudget-Konstante (gesamt verfügbar für RDT auf PSX-Äquivalent) */
#define DEBUG_MEM_BUDGET  (1536 * 1024)  /* 1,5 MB */

/**
 * Extern referenzierte Datenquellen. Wenn die Subsysteme noch nicht
 * initialisiert sind (NULL-Pointer), überspringt das jeweilige Overlay.
 */
extern re15_sca_data_t  g_current_sca;
extern re15_rvd_data_t  g_current_rvd;

/* Weak-Symbol Fallback: Falls die Globals noch nicht in einer
 * anderen TU definiert sind, liefern wir leere Defaults. */
#if defined(__GNUC__) || defined(__clang__)
  __attribute__((weak)) re15_sca_data_t g_current_sca = {0, 0, NULL};
  __attribute__((weak)) re15_rvd_data_t g_current_rvd = {0, {{{0}}}};
#endif

/* --------------------------------------------------------------------------
 * F1: Kollisionsboxen (SCA als Drahtgitter)
 * -------------------------------------------------------------------------- */

static void render_overlay_sca(SDL_Renderer* renderer)
{
    int i;
    const re15_sca_data_t* sca = &g_current_sca;

    if (sca->entries == NULL || sca->count == 0) {
        /* Keine Kollisionsdaten geladen */
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 200);
        draw_text(renderer, 4, 4, "F1: SCA - No data");
        return;
    }

    for (i = 0; i < sca->count; i++) {
        const re15_sca_entry_t* e = &sca->entries[i];

        /* Vereinfachte Weltkoordinaten → Bildschirmkoordinaten Projektion.
         * Debug-Overlays nutzen eine Top-Down-Projektion (X/Z → Screen X/Y)
         * skaliert auf das 320×240 Fenster. Die Mitte des Fensters entspricht
         * (0, 0) in Weltkoordinaten. Skalierung: 1 Welt-Einheit = 1/32 Pixel. */
        int cx = 160 + (e->corner_x / 32);
        int cz = 120 + (e->corner_z / 32);

        switch (e->type) {
            case RE15_SCA_TYPE_RECTANGLE: {
                int w = e->width / 32;
                int d = e->depth / 32;
                SDL_Rect rect = {cx, cz, w > 0 ? w : 1, d > 0 ? d : 1};
                SDL_SetRenderDrawColor(renderer, 255, 255, 255, 180);
                SDL_RenderDrawRect(renderer, &rect);
                break;
            }
            case RE15_SCA_TYPE_CIRCLE: {
                /* Approximiere Kreis als Octagon */
                int r = e->width / 32;
                int j;
                SDL_Point pts[9];
                if (r < 1) r = 1;
                for (j = 0; j < 8; j++) {
                    /* Einfache 8-Punkt-Approximation */
                    static const int dx8[] = {100, 71, 0, -71, -100, -71, 0, 71};
                    static const int dz8[] = {0, 71, 100, 71, 0, -71, -100, -71};
                    pts[j].x = cx + (r * dx8[j]) / 100;
                    pts[j].y = cz + (r * dz8[j]) / 100;
                }
                pts[8] = pts[0]; /* Polygon schließen */
                SDL_SetRenderDrawColor(renderer, 0, 255, 255, 180);
                SDL_RenderDrawLines(renderer, pts, 9);
                break;
            }
            case RE15_SCA_TYPE_STAIR_UP:
            case RE15_SCA_TYPE_STAIR_DOWN: {
                int w = e->width / 32;
                int d = e->depth / 32;
                SDL_Rect rect = {cx, cz, w > 0 ? w : 1, d > 0 ? d : 1};
                SDL_SetRenderDrawColor(renderer, 255, 255, 0, 180);
                SDL_RenderDrawRect(renderer, &rect);
                /* Diagonale als Treppe-Indikator */
                SDL_RenderDrawLine(renderer, rect.x, rect.y,
                                   rect.x + rect.w, rect.y + rect.h);
                break;
            }
            default:
                break;
        }
    }

    /* Label */
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    draw_text(renderer, 4, 4, "F1: SCA");
}

/* --------------------------------------------------------------------------
 * F2: AOT-Zonen (Tür/Item/Event farbcodiert)
 * -------------------------------------------------------------------------- */

static void render_overlay_aot(SDL_Renderer* renderer)
{
    int i;
    int drawn = 0;

    for (i = 0; i < RE15_AOT_MAX_SLOTS; i++) {
        const re15_aot_slot_t* slot = re15_aot_get_slot(i);
        int j;
        SDL_Point pts[5];

        if (slot == NULL || !slot->active) {
            continue;
        }

        /* Farbcodierung nach Typ */
        switch (slot->type) {
            case AOT_TYPE_DOOR:
                SDL_SetRenderDrawColor(renderer, 80, 80, 255, 200); /* Blau */
                break;
            case AOT_TYPE_ITEM:
                SDL_SetRenderDrawColor(renderer, 80, 255, 80, 200); /* Grün */
                break;
            case AOT_TYPE_GENERIC:
                SDL_SetRenderDrawColor(renderer, 255, 255, 80, 200); /* Gelb */
                break;
            case AOT_TYPE_CAM_SWITCH:
                SDL_SetRenderDrawColor(renderer, 200, 80, 200, 200); /* Magenta */
                break;
            default:
                SDL_SetRenderDrawColor(renderer, 128, 128, 128, 200); /* Grau */
                break;
        }

        /* 4-Punkt-Polygon zeichnen (Top-Down-Projektion) */
        for (j = 0; j < 4; j++) {
            pts[j].x = 160 + (slot->trigger_x[j] / 32);
            pts[j].y = 120 + (slot->trigger_z[j] / 32);
        }
        pts[4] = pts[0]; /* Polygon schließen */
        SDL_RenderDrawLines(renderer, pts, 5);
        drawn++;
    }

    /* Label */
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    {
        char buf[32];
        snprintf(buf, sizeof(buf), "F2: AOT %d", drawn);
        draw_text(renderer, 4, 12, buf);
    }
}

/* --------------------------------------------------------------------------
 * F3: Kamerazonen (RVD-Polygone mit Kamera-ID)
 * -------------------------------------------------------------------------- */

static void render_overlay_rvd(SDL_Renderer* renderer)
{
    int i;
    const re15_rvd_data_t* rvd = &g_current_rvd;

    if (rvd->count == 0) {
        SDL_SetRenderDrawColor(renderer, 200, 80, 200, 255);
        draw_text(renderer, 4, 20, "F3: RVD - No data");
        return;
    }

    for (i = 0; i < rvd->count; i++) {
        const re15_rvd_zone_t* z = &rvd->zones[i];
        int j;
        SDL_Point pts[5];
        char id_buf[8];
        int cx, cy;

        /* Magenta-Ton für Kamerazonen */
        SDL_SetRenderDrawColor(renderer, 200, 80, 200, 160);

        for (j = 0; j < 4; j++) {
            pts[j].x = 160 + (z->trigger_x[j] / 32);
            pts[j].y = 120 + (z->trigger_z[j] / 32);
        }
        pts[4] = pts[0];
        SDL_RenderDrawLines(renderer, pts, 5);

        /* Kamera-ID als Text in der Mitte des Polygons */
        cx = (pts[0].x + pts[1].x + pts[2].x + pts[3].x) / 4;
        cy = (pts[0].y + pts[1].y + pts[2].y + pts[3].y) / 4;
        snprintf(id_buf, sizeof(id_buf), "%d", z->cam_to);
        SDL_SetRenderDrawColor(renderer, 255, 200, 255, 255);
        draw_text(renderer, cx - 2, cy - 3, id_buf);
    }

    /* Label */
    SDL_SetRenderDrawColor(renderer, 200, 80, 200, 255);
    draw_text(renderer, 4, 20, "F3: RVD");
}

/* --------------------------------------------------------------------------
 * F4: SCD-Trace (letzte 10 Opcodes)
 * -------------------------------------------------------------------------- */

/** Opcode-Kurznamen für die Anzeige (ein paar bekannte) */
static const char* scd_opcode_name(uint8_t op)
{
    switch (op) {
        case 0x00: return "Nop";
        case 0x01: return "End";
        case 0x04: return "Exec";
        case 0x06: return "Ifel";
        case 0x07: return "Else";
        case 0x08: return "Endi";
        case 0x09: return "Slep";
        case 0x0A: return "Slpg";
        case 0x0D: return "For";
        case 0x0E: return "Next";
        case 0x21: return "Ck";
        case 0x22: return "Set";
        case 0x29: return "CCut";
        case 0x2C: return "AotS";
        case 0x2D: return "ObjM";
        case 0x2E: return "WrkS";
        case 0x2F: return "SpdS";
        case 0x30: return "AddS";
        case 0x36: return "SeOn";
        case 0x44: return "EmSt";
        case 0x4E: return "ItmA";
        case 0x68: return "Door";
        default:   return "??";
    }
}

static void render_overlay_scd_trace(SDL_Renderer* renderer)
{
    int i;
    int y_start = 30;
    int count;
    int read_idx;

    SDL_SetRenderDrawColor(renderer, 255, 200, 100, 255);
    draw_text(renderer, 4, y_start, "F4: SCD Trace");

    count = s_scd_trace_count < DEBUG_SCD_TRACE_SIZE
          ? s_scd_trace_count : DEBUG_SCD_TRACE_SIZE;

    if (count == 0) {
        draw_text(renderer, 4, y_start + 8, "No opcodes");
        return;
    }

    /* Von ältestem zum neuesten lesen */
    if (s_scd_trace_count >= DEBUG_SCD_TRACE_SIZE) {
        read_idx = s_scd_trace_write_idx; /* Ältester Eintrag */
    } else {
        read_idx = 0;
    }

    SDL_SetRenderDrawColor(renderer, 255, 255, 200, 255);
    for (i = 0; i < count; i++) {
        int idx = (read_idx + i) % DEBUG_SCD_TRACE_SIZE;
        char line[32];
        snprintf(line, sizeof(line), "%04X %02X %s",
                 s_scd_trace[idx].offset,
                 s_scd_trace[idx].opcode,
                 scd_opcode_name(s_scd_trace[idx].opcode));
        draw_text(renderer, 8, y_start + 8 + i * 8, line);
    }
}

/* --------------------------------------------------------------------------
 * F5: Speicher-Belegung (RAM-Balken)
 * -------------------------------------------------------------------------- */

static void render_overlay_memory(SDL_Renderer* renderer)
{
    int bar_x = 4;
    int bar_y = 228;
    int bar_w = 100;
    int bar_h = 8;
    int used_w;
    int pct;
    char buf[24];

    int rdt_size = g_game.rdt_buffer_size;
    if (rdt_size < 0) rdt_size = 0;

    pct = (rdt_size * 100) / DEBUG_MEM_BUDGET;
    if (pct > 100) pct = 100;
    used_w = (bar_w * pct) / 100;

    /* Hintergrund (dunkelgrau) */
    {
        SDL_Rect bg = {bar_x, bar_y, bar_w, bar_h};
        SDL_SetRenderDrawColor(renderer, 40, 40, 40, 200);
        SDL_RenderFillRect(renderer, &bg);
    }

    /* Belegter Bereich (grün → gelb → rot je nach Nutzung) */
    {
        SDL_Rect used = {bar_x, bar_y, used_w, bar_h};
        if (pct < 60) {
            SDL_SetRenderDrawColor(renderer, 80, 200, 80, 220);
        } else if (pct < 85) {
            SDL_SetRenderDrawColor(renderer, 220, 200, 50, 220);
        } else {
            SDL_SetRenderDrawColor(renderer, 220, 60, 60, 220);
        }
        SDL_RenderFillRect(renderer, &used);
    }

    /* Rahmen */
    {
        SDL_Rect frame = {bar_x, bar_y, bar_w, bar_h};
        SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255);
        SDL_RenderDrawRect(renderer, &frame);
    }

    /* Text-Label */
    snprintf(buf, sizeof(buf), "RAM %dK/%dK",
             rdt_size / 1024, DEBUG_MEM_BUDGET / 1024);
    SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255);
    draw_text(renderer, bar_x + bar_w + 4, bar_y + 1, buf);
}

/* ============================================================================
 * Exportierte API
 * ========================================================================= */

void debug_overlay_init(void)
{
    s_active_overlays = 0;
    s_scd_trace_write_idx = 0;
    s_scd_trace_count = 0;
    memset(s_scd_trace, 0, sizeof(s_scd_trace));
}

int debug_overlay_toggle(SDL_Keycode key)
{
    switch (key) {
        case SDLK_F1:
            s_active_overlays ^= DEBUG_OVERLAY_SCA;
            return 1;
        case SDLK_F2:
            s_active_overlays ^= DEBUG_OVERLAY_AOT;
            return 1;
        case SDLK_F3:
            s_active_overlays ^= DEBUG_OVERLAY_RVD;
            return 1;
        case SDLK_F4:
            s_active_overlays ^= DEBUG_OVERLAY_SCD_TRACE;
            return 1;
        case SDLK_F5:
            s_active_overlays ^= DEBUG_OVERLAY_MEMORY;
            return 1;
        default:
            return 0;
    }
}

void debug_overlay_render(SDL_Renderer* renderer)
{
    if (s_active_overlays == 0) {
        return;
    }

    /* Alpha-Blending aktivieren für halbtransparente Overlays */
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

    if (s_active_overlays & DEBUG_OVERLAY_SCA) {
        render_overlay_sca(renderer);
    }
    if (s_active_overlays & DEBUG_OVERLAY_AOT) {
        render_overlay_aot(renderer);
    }
    if (s_active_overlays & DEBUG_OVERLAY_RVD) {
        render_overlay_rvd(renderer);
    }
    if (s_active_overlays & DEBUG_OVERLAY_SCD_TRACE) {
        render_overlay_scd_trace(renderer);
    }
    if (s_active_overlays & DEBUG_OVERLAY_MEMORY) {
        render_overlay_memory(renderer);
    }

    /* Blendmode zurücksetzen */
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
}

void debug_overlay_scd_record(uint32_t offset, uint8_t opcode)
{
    s_scd_trace[s_scd_trace_write_idx].offset = offset;
    s_scd_trace[s_scd_trace_write_idx].opcode = opcode;
    s_scd_trace_write_idx = (s_scd_trace_write_idx + 1) % DEBUG_SCD_TRACE_SIZE;
    if (s_scd_trace_count < DEBUG_SCD_TRACE_SIZE) {
        s_scd_trace_count++;
    }
}

uint32_t debug_overlay_get_active(void)
{
    return s_active_overlays;
}

#endif /* RE15_PLATFORM_PC */
