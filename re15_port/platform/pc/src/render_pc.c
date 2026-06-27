/**
 * @file render_pc.c
 * @brief PC Rendering-Backend (SDL2 Renderer API)
 *
 * Implementiert die render_backend_t Schnittstelle für die PC-Plattform.
 * Verwendet SDL_RenderGeometry (SDL 2.0.18+) für texturierte/untexturierte
 * Primitive und eine Ordering Table (4096 Einträge) für Z-Sortierung
 * nach dem Painter's Algorithm (höchster Z = fern = wird zuerst gezeichnet).
 *
 * Auflösung: 320×240 (logisch), skaliert auf Fenstergröße.
 * Frame-Timing: 30fps via SDL_Delay.
 */

#include "re15_render.h"
#include "re15_types.h"

#include <SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ============================================================================
 * Konstanten
 * ========================================================================= */

/** Maximale Anzahl Primitive pro Frame */
#define RENDER_PC_MAX_PRIMS     4096

/** Fenster-Dimensionen (PSX-native Auflösung) */
#define RENDER_PC_WIDTH         320
#define RENDER_PC_HEIGHT        240

/** Integer-Skalierungsfaktor für Fenster */
#define RENDER_PC_SCALE         3

/** Ziel-Framerate und Frame-Zeit */
#define RENDER_PC_TARGET_FPS    30
#define RENDER_PC_FRAME_MS      (1000 / RENDER_PC_TARGET_FPS)

/* ============================================================================
 * Primitive-Pool (OT-Einträge)
 * ========================================================================= */

/** Typ eines Primitiv-Eintrags */
typedef enum {
    PRIM_TYPE_TRI,
    PRIM_TYPE_QUAD,
    PRIM_TYPE_SPRITE
} prim_type_t;

/** Sprite-Daten (für draw_sprite) */
typedef struct {
    int x, y, w, h;
    int tpage, clut;
} sprite_data_t;

/** Ein Primitiv im Pool */
typedef struct prim_node {
    prim_type_t type;
    struct prim_node* next;    /**< Nächster Eintrag in derselben OT-Stufe */
    union {
        render_tri_t  tri;
        render_quad_t quad;
        sprite_data_t sprite;
    } data;
} prim_node_t;

/* ============================================================================
 * Backend-State
 * ========================================================================= */

static struct {
    SDL_Window*   window;
    SDL_Renderer* renderer;

    /* Ordering Table: Array von Linked-List-Köpfen (Index = Z-Wert) */
    prim_node_t*  ot[RE15_OT_DEPTH];

    /* Primitiv-Pool (statisch alloziert, kein malloc pro Frame) */
    prim_node_t   prim_pool[RENDER_PC_MAX_PRIMS];
    int           prim_count;

    /* BSS-Hintergrund-Textur */
    SDL_Texture*  bg_texture;
    int           bg_valid;

    /* Letterbox-Höhe (in Pixeln, 0 = deaktiviert) */
    int           letterbox_height;

    /* Fade-Level (0 = kein Fade, 255 = komplett schwarz) */
    int           fade_level;

    /* Frame-Timing */
    Uint32        frame_start;

    /* Initialisiert-Flag */
    int           initialized;
} s_rpc;

/* ============================================================================
 * Hilfsfunktionen
 * ========================================================================= */

/**
 * Alloziert einen Primitiv-Knoten aus dem Pool.
 * @return Pointer auf freien Knoten oder NULL wenn Pool voll.
 */
static prim_node_t* alloc_prim(void)
{
    if (s_rpc.prim_count >= RENDER_PC_MAX_PRIMS) {
        return NULL;
    }
    prim_node_t* node = &s_rpc.prim_pool[s_rpc.prim_count++];
    node->next = NULL;
    return node;
}

/**
 * Fügt einen Primitiv-Knoten in die OT am gegebenen Z-Level ein.
 * Prepend (Insertion am Kopf der Liste).
 */
static void ot_insert(prim_node_t* node, uint16_t z)
{
    uint16_t clamped_z = z;
    if (clamped_z >= RE15_OT_DEPTH) {
        clamped_z = RE15_OT_DEPTH - 1;
    }
    node->next = s_rpc.ot[clamped_z];
    s_rpc.ot[clamped_z] = node;
}

/**
 * Rendert ein einzelnes Dreieck über SDL_RenderGeometry.
 */
static void render_tri_prim(const render_tri_t* tri)
{
    SDL_Vertex verts[3];
    int i;

    for (i = 0; i < 3; i++) {
        verts[i].position.x = (float)tri->vertices[i].x;
        verts[i].position.y = (float)tri->vertices[i].y;
        verts[i].color.r = tri->vertices[i].r;
        verts[i].color.g = tri->vertices[i].g;
        verts[i].color.b = tri->vertices[i].b;
        verts[i].color.a = (tri->flags & RE15_RENDER_FLAG_SEMI_TRANS) ? 128 : 255;
        /* UV-Koordinaten normalisiert auf 0..1 (SDL erwartet Textur-Coords) */
        verts[i].tex_coord.x = (float)tri->vertices[i].u / 255.0f;
        verts[i].tex_coord.y = (float)tri->vertices[i].v / 255.0f;
    }

    /* Untexturiert: texture=NULL, SDL nutzt Vertex-Farben */
    SDL_RenderGeometry(s_rpc.renderer, NULL, verts, 3, NULL, 0);
}

/**
 * Rendert ein Quad als zwei Dreiecke über SDL_RenderGeometry.
 * Vertex-Reihenfolge: 0-1-2, 1-2-3 (Dreiecks-Fan-Aufteilung).
 */
static void render_quad_prim(const render_quad_t* quad)
{
    SDL_Vertex verts[4];
    int indices[6] = { 0, 1, 2, 1, 2, 3 };
    int i;

    for (i = 0; i < 4; i++) {
        verts[i].position.x = (float)quad->vertices[i].x;
        verts[i].position.y = (float)quad->vertices[i].y;
        verts[i].color.r = quad->vertices[i].r;
        verts[i].color.g = quad->vertices[i].g;
        verts[i].color.b = quad->vertices[i].b;
        verts[i].color.a = (quad->flags & RE15_RENDER_FLAG_SEMI_TRANS) ? 128 : 255;
        verts[i].tex_coord.x = (float)quad->vertices[i].u / 255.0f;
        verts[i].tex_coord.y = (float)quad->vertices[i].v / 255.0f;
    }

    SDL_RenderGeometry(s_rpc.renderer, NULL, verts, 4, indices, 6);
}

/**
 * Rendert ein Sprite als farbiges Rechteck (Platzhalter ohne Textur-Lookup).
 */
static void render_sprite_prim(const sprite_data_t* spr)
{
    SDL_Rect dst;
    dst.x = spr->x;
    dst.y = spr->y;
    dst.w = spr->w;
    dst.h = spr->h;

    /* Sprite als weißes Rect rendern (Textur-Lookup TBD) */
    SDL_SetRenderDrawColor(s_rpc.renderer, 255, 255, 255, 255);
    SDL_RenderFillRect(s_rpc.renderer, &dst);
}

/**
 * Zeichnet Letterbox-Balken (schwarze Streifen oben/unten).
 */
static void apply_letterbox(void)
{
    SDL_Rect top, bottom;

    if (s_rpc.letterbox_height <= 0) return;

    SDL_SetRenderDrawColor(s_rpc.renderer, 0, 0, 0, 255);

    top.x = 0;
    top.y = 0;
    top.w = RENDER_PC_WIDTH;
    top.h = s_rpc.letterbox_height;
    SDL_RenderFillRect(s_rpc.renderer, &top);

    bottom.x = 0;
    bottom.y = RENDER_PC_HEIGHT - s_rpc.letterbox_height;
    bottom.w = RENDER_PC_WIDTH;
    bottom.h = s_rpc.letterbox_height;
    SDL_RenderFillRect(s_rpc.renderer, &bottom);
}

/**
 * Wendet Screen-Fade an (schwarzes Semi-Transparent-Overlay).
 */
static void apply_fade(void)
{
    SDL_Rect full;

    if (s_rpc.fade_level <= 0) return;

    full.x = 0;
    full.y = 0;
    full.w = RENDER_PC_WIDTH;
    full.h = RENDER_PC_HEIGHT;

    SDL_SetRenderDrawBlendMode(s_rpc.renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(s_rpc.renderer, 0, 0, 0, (Uint8)s_rpc.fade_level);
    SDL_RenderFillRect(s_rpc.renderer, &full);
    SDL_SetRenderDrawBlendMode(s_rpc.renderer, SDL_BLENDMODE_NONE);
}

/* ============================================================================
 * Backend-Implementierung (render_backend_t Funktionen)
 * ========================================================================= */

/**
 * Initialisiert SDL2-Fenster (320×240 skaliert) und Renderer.
 * Bei Fehler: Fehlermeldung auf stderr + Programmabbruch.
 */
static void pc_init(void)
{
    if (s_rpc.initialized) return;

    /* SDL Video-Subsystem (falls noch nicht initialisiert) */
    if (!SDL_WasInit(SDL_INIT_VIDEO)) {
        if (SDL_Init(SDL_INIT_VIDEO) != 0) {
            fprintf(stderr, "[RE15 RENDER] SDL_Init(VIDEO) fehlgeschlagen: %s\n",
                    SDL_GetError());
            exit(EXIT_FAILURE);
        }
    }

    /* Fenster erstellen */
    s_rpc.window = SDL_CreateWindow(
        "RE 1.5 Port (PC)",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        RENDER_PC_WIDTH * RENDER_PC_SCALE,
        RENDER_PC_HEIGHT * RENDER_PC_SCALE,
        SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE
    );

    if (!s_rpc.window) {
        fprintf(stderr, "[RE15 RENDER] SDL_CreateWindow fehlgeschlagen: %s\n",
                SDL_GetError());
        exit(EXIT_FAILURE);
    }

    /* Renderer erstellen (Hardware-beschleunigt) */
    s_rpc.renderer = SDL_CreateRenderer(
        s_rpc.window, -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
    );

    if (!s_rpc.renderer) {
        fprintf(stderr, "[RE15 RENDER] SDL_CreateRenderer fehlgeschlagen: %s\n",
                SDL_GetError());
        SDL_DestroyWindow(s_rpc.window);
        exit(EXIT_FAILURE);
    }

    /* Logische Auflösung = PSX-nativ (automatische Skalierung) */
    SDL_RenderSetLogicalSize(s_rpc.renderer, RENDER_PC_WIDTH, RENDER_PC_HEIGHT);

    /* Blending-Modus für Semi-Transparenz */
    SDL_SetRenderDrawBlendMode(s_rpc.renderer, SDL_BLENDMODE_BLEND);

    /* State initialisieren */
    s_rpc.bg_texture = NULL;
    s_rpc.bg_valid = 0;
    s_rpc.letterbox_height = 0;
    s_rpc.fade_level = 0;
    s_rpc.prim_count = 0;
    memset(s_rpc.ot, 0, sizeof(s_rpc.ot));

    s_rpc.initialized = 1;

    fprintf(stdout, "[RE15 RENDER] PC-Backend initialisiert (%dx%d, %dx Skalierung)\n",
            RENDER_PC_WIDTH, RENDER_PC_HEIGHT, RENDER_PC_SCALE);
}

/**
 * Frame beginnen: OT leeren, Framebuffer clearen.
 */
static void pc_begin_frame(void)
{
    /* OT-Köpfe zurücksetzen */
    memset(s_rpc.ot, 0, sizeof(s_rpc.ot));
    s_rpc.prim_count = 0;

    /* Frame-Start für Timing */
    s_rpc.frame_start = SDL_GetTicks();

    /* Framebuffer schwarz löschen */
    SDL_SetRenderDrawColor(s_rpc.renderer, 0, 0, 0, 255);
    SDL_RenderClear(s_rpc.renderer);
}

/**
 * Frame abschliessen: OT von höchstem Z (fern) zu niedrigstem Z (nah)
 * traversieren und alle Primitive rendern. Dann Letterbox, Fade,
 * SDL_RenderPresent und Frame-Timing anwenden.
 */
static void pc_end_frame(void)
{
    int z;
    prim_node_t* node;
    Uint32 elapsed;

    /* BSS-Hintergrund zuerst (liegt immer hinter allem) */
    if (s_rpc.bg_valid && s_rpc.bg_texture) {
        SDL_RenderCopy(s_rpc.renderer, s_rpc.bg_texture, NULL, NULL);
    }

    /* OT traversieren: Höchster Z (fern) → Niedrigster Z (nah) = Painter's Algorithm */
    for (z = RE15_OT_DEPTH - 1; z >= 0; z--) {
        node = s_rpc.ot[z];
        while (node) {
            switch (node->type) {
                case PRIM_TYPE_TRI:
                    render_tri_prim(&node->data.tri);
                    break;
                case PRIM_TYPE_QUAD:
                    render_quad_prim(&node->data.quad);
                    break;
                case PRIM_TYPE_SPRITE:
                    render_sprite_prim(&node->data.sprite);
                    break;
            }
            node = node->next;
        }
    }

    /* Post-Processing: Letterbox und Fade */
    apply_letterbox();
    apply_fade();

    /* Framebuffer präsentieren */
    SDL_RenderPresent(s_rpc.renderer);

    /* 30fps Frame-Timing */
    elapsed = SDL_GetTicks() - s_rpc.frame_start;
    if (elapsed < RENDER_PC_FRAME_MS) {
        SDL_Delay(RENDER_PC_FRAME_MS - elapsed);
    }
}

/**
 * Dreieck in OT einreichen.
 */
static void pc_submit_tri(const render_tri_t* tri)
{
    prim_node_t* node;

    if (!tri) return;

    node = alloc_prim();
    if (!node) {
        /* Pool voll — Primitiv wird verworfen (Frame-Limit erreicht) */
        return;
    }

    node->type = PRIM_TYPE_TRI;
    node->data.tri = *tri;
    ot_insert(node, tri->z);
}

/**
 * Viereck in OT einreichen.
 */
static void pc_submit_quad(const render_quad_t* quad)
{
    prim_node_t* node;

    if (!quad) return;

    node = alloc_prim();
    if (!node) {
        return;
    }

    node->type = PRIM_TYPE_QUAD;
    node->data.quad = *quad;
    ot_insert(node, quad->z);
}

/**
 * BSS-Hintergrund anzeigen: MDEC-dekodierten 64KB-Chunk als Textur hochladen.
 * Der Chunk wird als 320×240 16-bit RGB565 interpretiert (PSX MDEC-Output-Format).
 * Wird bei z=4095 (fernster Punkt) in die Szene eingebunden.
 */
static void pc_show_background(const uint8_t* bss_chunk, int cut_id)
{
    (void)cut_id;

    if (!bss_chunk) {
        s_rpc.bg_valid = 0;
        return;
    }

    /* Alte Textur freigeben */
    if (s_rpc.bg_texture) {
        SDL_DestroyTexture(s_rpc.bg_texture);
        s_rpc.bg_texture = NULL;
    }

    /* Neue Textur erstellen (320×240, RGB565 wie PSX-MDEC-Output) */
    s_rpc.bg_texture = SDL_CreateTexture(
        s_rpc.renderer,
        SDL_PIXELFORMAT_RGB565,
        SDL_TEXTUREACCESS_STATIC,
        RENDER_PC_WIDTH, RENDER_PC_HEIGHT
    );

    if (!s_rpc.bg_texture) {
        fprintf(stderr, "[RE15 RENDER] BSS-Textur erstellen fehlgeschlagen: %s\n",
                SDL_GetError());
        s_rpc.bg_valid = 0;
        return;
    }

    /* Pixel-Daten hochladen (320 * 2 Bytes pro Zeile = 640 Bytes Pitch) */
    if (SDL_UpdateTexture(s_rpc.bg_texture, NULL, bss_chunk,
                          RENDER_PC_WIDTH * 2) != 0) {
        fprintf(stderr, "[RE15 RENDER] BSS-Textur-Update fehlgeschlagen: %s\n",
                SDL_GetError());
        SDL_DestroyTexture(s_rpc.bg_texture);
        s_rpc.bg_texture = NULL;
        s_rpc.bg_valid = 0;
        return;
    }

    s_rpc.bg_valid = 1;
}

/**
 * 2D-Sprite rendern: In OT am gegebenen Z-Level einfügen.
 */
static void pc_draw_sprite(int x, int y, int w, int h, int tpage, int clut, int z)
{
    prim_node_t* node = alloc_prim();

    if (!node) return;

    node->type = PRIM_TYPE_SPRITE;
    node->data.sprite.x = x;
    node->data.sprite.y = y;
    node->data.sprite.w = w;
    node->data.sprite.h = h;
    node->data.sprite.tpage = tpage;
    node->data.sprite.clut = clut;
    ot_insert(node, (uint16_t)z);
}

/**
 * Letterbox-Balken setzen.
 * @param height Höhe der Balken in Pixeln (0 = deaktiviert).
 */
static void pc_set_letterbox(int height)
{
    s_rpc.letterbox_height = height;
}

/**
 * Screen-Fade-Level setzen.
 * @param level 0 = kein Fade, 255 = komplett schwarz.
 */
static void pc_set_fade(int level)
{
    if (level < 0) level = 0;
    if (level > 255) level = 255;
    s_rpc.fade_level = level;
}

/**
 * Backend herunterfahren: Ressourcen freigeben.
 */
static void pc_shutdown(void)
{
    if (!s_rpc.initialized) return;

    if (s_rpc.bg_texture) {
        SDL_DestroyTexture(s_rpc.bg_texture);
        s_rpc.bg_texture = NULL;
    }

    if (s_rpc.renderer) {
        SDL_DestroyRenderer(s_rpc.renderer);
        s_rpc.renderer = NULL;
    }

    if (s_rpc.window) {
        SDL_DestroyWindow(s_rpc.window);
        s_rpc.window = NULL;
    }

    s_rpc.initialized = 0;
    s_rpc.bg_valid = 0;

    fprintf(stdout, "[RE15 RENDER] PC-Backend heruntergefahren.\n");
}

/* ============================================================================
 * Exportierte Backend-Instanz
 * ========================================================================= */

const render_backend_t render_pc_backend = {
    .init            = pc_init,
    .begin_frame     = pc_begin_frame,
    .end_frame       = pc_end_frame,
    .submit_tri      = pc_submit_tri,
    .submit_quad     = pc_submit_quad,
    .show_background = pc_show_background,
    .draw_sprite     = pc_draw_sprite,
    .set_letterbox   = pc_set_letterbox,
    .set_fade        = pc_set_fade,
    .shutdown        = pc_shutdown,
};

/* Definition des in re15_render.h deklarierten globalen Backend-Pointers.
 * Standardmäßig auf das PC-Backend gesetzt; main.c setzt ihn bei der Init erneut. */
const render_backend_t* g_render = &render_pc_backend;

/* ============================================================================
 * Hilfsfunktionen für plattformspezifischen Zugriff
 * ========================================================================= */

/**
 * Gibt den internen SDL_Renderer zurück (für Debug-Overlays und andere
 * PC-spezifische Rendering-Aufgaben die direkten Zugriff benötigen).
 *
 * @return SDL_Renderer* oder NULL wenn Backend nicht initialisiert
 */
SDL_Renderer* render_pc_get_renderer(void)
{
    return s_rpc.renderer;
}
