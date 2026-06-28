/**
 * @file re15_render.h
 * @brief Rendering-Abstraktionsschicht — Interface für PSX-GTE/GPU und PC-SDL2/OpenGL
 *
 * Definiert die gemeinsame Rendering-Schnittstelle (render_backend_t) sowie
 * die Polygon-Strukturen (render_tri_t, render_quad_t) für texturierte und
 * untexturierte Primitive. Beide Backends implementieren dieselbe
 * Funktionssignatur-Tabelle.
 *
 * Basiert auf dem RE2-Ordering-Table-Konzept mit 4096 Tiefenstufen.
 */
#ifndef RE15_RENDER_H
#define RE15_RENDER_H

#include "re15_types.h"

/* ============================================================================
 * Konstanten
 * ========================================================================= */

/** Ordering-Table-Tiefe: 4096 Einträge für Z-Sortierung */
#define RE15_OT_DEPTH 4096

/* ============================================================================
 * Render-Flags (Bitfeld für Polygon-Attribute)
 * ========================================================================= */

/** Polygon ist texturiert */
#define RE15_RENDER_FLAG_TEXTURED       (1 << 0)

/** Polygon ist beleuchtet (Gouraud-Shading oder Flat pro Vertex-Farbe) */
#define RE15_RENDER_FLAG_LIT            (1 << 1)

/** Polygon ist semi-transparent (Alpha-Blending) */
#define RE15_RENDER_FLAG_SEMI_TRANS     (1 << 2)

/* ============================================================================
 * Vertex-Strukturen
 * ========================================================================= */

/**
 * 3D-Vertex mit Position, UV-Koordinaten und Farbe.
 * Zusammengefasst pro Vertex für Cache-freundlichen Zugriff.
 */
typedef struct {
    int16_t  x, y, z;      /**< Position (Welt-/Modell-Koordinaten)           */
    uint8_t  u, v;          /**< Textur-Koordinaten (0-255 innerhalb TPage)   */
    uint8_t  r, g, b;      /**< Vertex-Farbe (Gouraud) oder Flat-Farbe       */
    uint8_t  pad;           /**< Padding auf gerade Byte-Grenze               */
} render_vertex_t;

/* ============================================================================
 * Polygon-Strukturen
 * ========================================================================= */

/**
 * Dreieck-Primitiv (3 Vertices).
 * Wird über submit_tri() an das Backend übergeben.
 */
typedef struct {
    render_vertex_t vertices[3]; /**< Drei Eckpunkte                          */
    uint16_t tpage;              /**< Texture-Page (VRAM-Position/Format)      */
    uint16_t clut;               /**< Color Look-Up Table ID                   */
    uint16_t z;                  /**< OT-Tiefenwert (0 = nah, 4095 = fern)    */
    uint16_t flags;              /**< Kombination aus RE15_RENDER_FLAG_*       */
} render_tri_t;

/**
 * Viereck-Primitiv (4 Vertices).
 * Wird über submit_quad() an das Backend übergeben.
 */
typedef struct {
    render_vertex_t vertices[4]; /**< Vier Eckpunkte                          */
    uint16_t tpage;              /**< Texture-Page (VRAM-Position/Format)      */
    uint16_t clut;               /**< Color Look-Up Table ID                   */
    uint16_t z;                  /**< OT-Tiefenwert (0 = nah, 4095 = fern)    */
    uint16_t flags;              /**< Kombination aus RE15_RENDER_FLAG_*       */
} render_quad_t;

/* ============================================================================
 * Render-Backend-Interface (Funktionszeiger-Tabelle)
 *
 * Identische Signatur für PSX (GTE/GPU) und PC (SDL2/OpenGL).
 * Das aktive Backend wird zur Linkzeit oder Laufzeit über g_render gesetzt.
 * ========================================================================= */

typedef struct {
    /** Hardware/Library initialisieren (Fenster, GPU, VRAM) */
    void (*init)(void);

    /** Frame-Rendering beginnen (OT leeren, Framebuffer vorbereiten) */
    void (*begin_frame)(void);

    /** Frame-Rendering abschliessen (OT traversieren, Flip/VSync) */
    void (*end_frame)(void);

    /** Texturiertes/untexturiertes Dreieck einreichen */
    void (*submit_tri)(const render_tri_t* tri);

    /** Texturiertes/untexturiertes Viereck einreichen */
    void (*submit_quad)(const render_quad_t* quad);

    /** MDEC-dekodierten BSS-Hintergrund (64KB Chunk) anzeigen */
    void (*show_background)(const uint8_t* bss_chunk, int cut_id);

    /** 2D-Sprite rendern (für UI, Items, Effekte) */
    void (*draw_sprite)(int x, int y, int w, int h, int tpage, int clut, int z);

    /** Letterbox-Balken setzen (Höhe in Pixeln, 0 = kein Letterbox) */
    void (*set_letterbox)(int height);

    /** Screen-Fade-Level setzen (0 = kein Fade, 255 = komplett schwarz) */
    void (*set_fade)(int level);

    /** Backend herunterfahren (Ressourcen freigeben, Fenster schliessen) */
    void (*shutdown)(void);
} render_backend_t;

/* ============================================================================
 * Globales aktives Render-Backend
 *
 * Wird beim Start auf das plattformspezifische Backend gesetzt:
 * - PSX: &render_psx_backend
 * - PC:  &render_pc_backend
 * ========================================================================= */

extern const render_backend_t* g_render;

#endif /* RE15_RENDER_H */
