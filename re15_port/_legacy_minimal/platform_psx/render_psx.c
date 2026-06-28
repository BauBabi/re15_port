/**
 * @file render_psx.c
 * @brief PSX Rendering-Backend (GTE/GPU via PSn00bSDK)
 *
 * Implementiert die render_backend_t Schnittstelle für die PSX-Plattform.
 * Verwendet die GTE (Geometry Transformation Engine) für 3D-Transformationen
 * (RotMatrix, TransMatrix, RTPT, NCCT) und GPU-DMA-Submission vor VSync-Wait.
 *
 * OT: 4096 Einträge für Tiefensortierung (Painter's Algorithm).
 * MDEC: VLC → YCbCr → RGB → VRAM für BSS-Hintergründe.
 *
 * Auflösung: 320×240 (NTSC), Double-Buffered.
 * Frame-Timing: 30fps via VSync(2) (jedes zweite NTSC-Feld).
 */

#include "re15_render.h"
#include "re15_types.h"

#include <stdint.h>
#include <string.h>
#include <stdio.h>

#include <psxgpu.h>
#include <psxgte.h>
#include <psxapi.h>

/* ============================================================================
 * Konstanten
 * ========================================================================= */

/** Display-Auflösung (NTSC) */
#define PSX_SCREEN_W    320
#define PSX_SCREEN_H    240

/** Primitive-Buffer-Größe pro Framebuffer (Bytes) */
#define PSX_PRI_SIZE    (64 * 1024)

/** MDEC-Dekodierungspuffer-Größe (320×240 × 2 Bytes RGB555) */
#define PSX_MDEC_BUF_SIZE (PSX_SCREEN_W * PSX_SCREEN_H * 2)

/** MDEC-Streifen: Dekodierung in 16-Pixel-hohe Streifen */
#define PSX_MDEC_SLICE_H  16

/* ============================================================================
 * Double-Buffer Struktur
 * ========================================================================= */

typedef struct {
    DISPENV  disp;
    DRAWENV  draw;
    uint32_t ot[RE15_OT_DEPTH];       /**< Ordering Table (4096 Einträge) */
    char     pri[PSX_PRI_SIZE];        /**< Primitive-Buffer               */
} psx_framebuffer_t;

/* ============================================================================
 * Backend-State
 * ========================================================================= */

static struct {
    psx_framebuffer_t fb[2];           /**< Double-Buffer                  */
    int               fb_idx;          /**< Aktiver Framebuffer-Index      */
    char*             next_pri;        /**< Nächster freier Primitive-Slot  */

    /** Letterbox-Höhe (Pixel, 0 = deaktiviert) */
    int               letterbox_height;

    /** Fade-Level (0 = kein Fade, 255 = komplett schwarz) */
    int               fade_level;

    /** MDEC-Hintergrund-Status */
    int               bg_loaded;       /**< 1 wenn BG im VRAM liegt        */
    uint16_t          bg_vram_x;       /**< VRAM X-Position des BG         */
    uint16_t          bg_vram_y;       /**< VRAM Y-Position des BG         */

    /** Initialisiert-Flag */
    int               initialized;
} s_rpsx;

/* ============================================================================
 * MDEC-Dekodierungspuffer (in .bss, aligned für DMA)
 * ========================================================================= */

/** Dekodierungspuffer für MDEC-Streifen (320 × 16 Pixel × 3 Bytes RGB) */
static uint32_t s_mdec_slice_buf[PSX_SCREEN_W * PSX_MDEC_SLICE_H / 2]
    __attribute__((aligned(4)));

/* ============================================================================
 * Hilfsfunktionen
 * ========================================================================= */

/**
 * Gibt den aktuellen Primitive-Pointer zurück und rückt um size Bytes vor.
 * @return Pointer auf den allozierten Bereich oder NULL bei Überlauf.
 */
static void* psx_alloc_pri(int size)
{
    char* current = s_rpsx.next_pri;
    char* end = s_rpsx.fb[s_rpsx.fb_idx].pri + PSX_PRI_SIZE;

    if (current + size > end) {
        /* Primitive-Buffer voll — Primitiv verwerfen */
        return NULL;
    }

    s_rpsx.next_pri += size;
    return (void*)current;
}

/* ============================================================================
 * GTE-Transformationsfunktionen
 *
 * Wrappers um die PSn00bSDK GTE-Inline-Assemblerfunktionen.
 * Diese stellen die kanonischen RE2-Transformations-Primitiven bereit:
 *   - RotMatrix:  Rotation setzen (GTE Rotationsmatrix laden)
 *   - TransMatrix: Translation setzen (GTE Translationsvektor laden)
 *   - RTPT:       Rotate-Translate-Perspective-Transform (3 Vertices)
 *   - NCCT:       Normal-Color-Color-Triple (Beleuchtung für 3 Vertices)
 * ========================================================================= */

/**
 * Setzt die GTE-Rotationsmatrix aus einer MATRIX-Struktur.
 * @param mtx  Pointer auf die zusammengesetzte Rotations-/Translationsmatrix.
 */
static void psx_gte_set_rotation(const MATRIX* mtx)
{
    gte_SetRotMatrix(mtx);
}

/**
 * Setzt den GTE-Translationsvektor aus einer MATRIX-Struktur.
 * @param mtx  Pointer auf die zusammengesetzte Rotations-/Translationsmatrix.
 */
static void psx_gte_set_translation(const MATRIX* mtx)
{
    gte_SetTransMatrix(mtx);
}

/**
 * Führt RTPT (Rotate-Translate-Perspective-Transform) für 3 Vertices aus.
 * Liest die 3 Vertices aus v0/v1/v2 (SVECTOR) und schreibt die
 * projizierten Screen-Koordinaten in sxy0/sxy1/sxy2.
 *
 * @param v0,v1,v2   Eingabe-Vertices (3D Welt/Modell-Koordinaten)
 * @param sxy0,sxy1,sxy2  Ausgabe-Screenkoordinaten (2D projiziert)
 * @param p          Interpolation-Flag (für sz-Average)
 * @param otz        Gemittelter Z-Wert für OT-Einsortierung
 * @return           GTE-Flags (Clip-Flags)
 */
static int psx_gte_rtpt(const SVECTOR* v0, const SVECTOR* v1,
                         const SVECTOR* v2, int32_t* sxy0,
                         int32_t* sxy1, int32_t* sxy2,
                         int* p, int* otz)
{
    int flags;

    gte_ldv3(v0, v1, v2);
    gte_rtpt();
    gte_stflg(&flags);
    gte_stsxy0(sxy0);
    gte_stsxy1(sxy1);
    gte_stsxy2(sxy2);
    gte_avsz3(p, otz);

    return flags;
}

/**
 * Führt NCCT (Normal-Color-Color-Triple) für Beleuchtung aus.
 * Berechnet die finale Vertex-Farbe basierend auf den Normalen
 * und den aktuellen Lichtquellen im GTE.
 *
 * @param n0,n1,n2   Eingabe-Normalen (SVECTOR, normalisiert)
 * @param c0,c1,c2   Ausgabe-Farben (CVECTOR RGB)
 */
static void psx_gte_ncct(const SVECTOR* n0, const SVECTOR* n1,
                          const SVECTOR* n2, CVECTOR* c0,
                          CVECTOR* c1, CVECTOR* c2)
{
    gte_ldv3(n0, n1, n2);
    gte_ncct();
    gte_strgb(c0);
    gte_strgb1(c1);
    gte_strgb2(c2);
}

/* ============================================================================
 * MDEC-Dekodierung für BSS-Hintergründe
 *
 * BSS-Daten sind VLC-komprimiert (Variable Length Coding). Die Dekodierung
 * läuft in Streifen:
 *   1. DecDCTReset() — MDEC-Hardware zurücksetzen
 *   2. DecDCTin() — VLC-komprimierte Daten an MDEC senden
 *   3. DecDCTout() — RGB-Streifen empfangen
 *   4. LoadImage() — Streifen in VRAM hochladen
 *
 * Ziel-VRAM-Position: (640, 0) — außerhalb des Display-Bereichs,
 * wird bei show_background per MoveImage in den Framebuffer kopiert.
 * ========================================================================= */

/**
 * Dekodiert einen 64KB BSS-Chunk via MDEC in den VRAM.
 *
 * @param bss_chunk  Pointer auf den 64KB-VLC-Bitstream
 * @param dest_x     VRAM-Ziel X-Koordinate
 * @param dest_y     VRAM-Ziel Y-Koordinate
 * @return           0 bei Erfolg, -1 bei Fehler
 */
static int psx_mdec_decode_bss(const uint8_t* bss_chunk,
                                uint16_t dest_x, uint16_t dest_y)
{
    RECT slice_rect;
    int  num_slices;
    int  i;

    if (!bss_chunk) return -1;

    /* MDEC zurücksetzen */
    DecDCTReset(0);

    /* Anzahl der 16-Pixel-hohen Streifen */
    num_slices = PSX_SCREEN_H / PSX_MDEC_SLICE_H;

    /* VLC-Daten an den MDEC-Dekoder senden (asynchron) */
    DecDCTin((uint32_t*)bss_chunk, 0);

    /* Streifen empfangen und in VRAM hochladen */
    for (i = 0; i < num_slices; i++) {
        /* MDEC-Output empfangen (320 × 16 Pixel, 15-bit RGB) */
        DecDCTout(s_mdec_slice_buf,
                  PSX_SCREEN_W * PSX_MDEC_SLICE_H / 2);

        /* Auf Dekodierung warten */
        DecDCToutSync(0);

        /* Streifen-Rect im VRAM definieren */
        slice_rect.x = dest_x;
        slice_rect.y = dest_y + (i * PSX_MDEC_SLICE_H);
        slice_rect.w = PSX_SCREEN_W;
        slice_rect.h = PSX_MDEC_SLICE_H;

        /* In VRAM hochladen */
        LoadImage(&slice_rect, s_mdec_slice_buf);
        DrawSync(0);
    }

    return 0;
}

/* ============================================================================
 * Backend-Implementierung (render_backend_t Funktionen)
 * ========================================================================= */

/**
 * Initialisiert die PSX-GPU (Double-Buffer, NTSC 320×240) und die GTE.
 * Setzt den MDEC-Hintergrund-Status zurück.
 */
static void psx_init(void)
{
    if (s_rpsx.initialized) return;

    /* GPU zurücksetzen und Display-Modus konfigurieren */
    ResetGraph(0);

    /* Framebuffer 0: Display bei (0,0), Draw bei (0,240) */
    SetDefDispEnv(&s_rpsx.fb[0].disp, 0,   0, PSX_SCREEN_W, PSX_SCREEN_H);
    SetDefDrawEnv(&s_rpsx.fb[0].draw, 0, 240, PSX_SCREEN_W, PSX_SCREEN_H);

    /* Framebuffer 1: Display bei (0,240), Draw bei (0,0) */
    SetDefDispEnv(&s_rpsx.fb[1].disp, 0, 240, PSX_SCREEN_W, PSX_SCREEN_H);
    SetDefDrawEnv(&s_rpsx.fb[1].draw, 0,   0, PSX_SCREEN_W, PSX_SCREEN_H);

    /* Hintergrundfarbe für Clear (Schwarz) */
    s_rpsx.fb[0].draw.isbg = 1;
    s_rpsx.fb[1].draw.isbg = 1;
    setRGB0(&s_rpsx.fb[0].draw, 0, 0, 0);
    setRGB0(&s_rpsx.fb[1].draw, 0, 0, 0);

    /* NTSC-Videomodus */
    SetVideoMode(MODE_NTSC);

    /* GTE initialisieren */
    InitGeom();
    SetGeomOffset(PSX_SCREEN_W / 2, PSX_SCREEN_H / 2);
    SetGeomScreen(PSX_SCREEN_W / 2);  /* Projektionsdistanz = 160 */

    /* Erster Framebuffer aktiv */
    s_rpsx.fb_idx = 0;
    PutDispEnv(&s_rpsx.fb[0].disp);
    PutDrawEnv(&s_rpsx.fb[0].draw);

    /* OT leeren */
    ClearOTagR(s_rpsx.fb[0].ot, RE15_OT_DEPTH);
    s_rpsx.next_pri = s_rpsx.fb[0].pri;

    /* Display einschalten */
    SetDispMask(1);

    /* MDEC-Decoder zurücksetzen */
    DecDCTReset(0);

    /* State initialisieren */
    s_rpsx.letterbox_height = 0;
    s_rpsx.fade_level = 0;
    s_rpsx.bg_loaded = 0;
    s_rpsx.bg_vram_x = 640;  /* BG-Cache im oberen rechten VRAM-Quadrant */
    s_rpsx.bg_vram_y = 0;

    s_rpsx.initialized = 1;

    printf("[RE15 RENDER] PSX-Backend initialisiert (320x240, NTSC)\n");
}

/**
 * Frame beginnen: OT leeren und Primitive-Buffer zurücksetzen.
 */
static void psx_begin_frame(void)
{
    /* OT des aktuellen Framebuffers leeren (Reverse-Order für Painter's Alg.) */
    ClearOTagR(s_rpsx.fb[s_rpsx.fb_idx].ot, RE15_OT_DEPTH);

    /* Primitive-Buffer-Pointer auf Anfang setzen */
    s_rpsx.next_pri = s_rpsx.fb[s_rpsx.fb_idx].pri;
}

/**
 * Frame abschließen: GPU-DMA-Submission, dann VSync-Wait.
 *
 * Reihenfolge (RE2-kanonisch):
 *   1. DrawSync(0) — Warte bis alle laufenden GPU-Kommandos fertig sind
 *   2. VSync(0) — Warte auf nächstes VBlank (30fps bei NTSC)
 *   3. PutDispEnv/PutDrawEnv — Framebuffer umschalten
 *   4. DrawOTag — OT an GPU-DMA übergeben (startet asynchrones Rendern)
 *   5. Framebuffer-Index umschalten für nächsten Frame
 *
 * Die GPU-DMA-Submission (DrawOTag) erfolgt VOR dem nächsten Frame-Begin,
 * sodass die GPU parallel zur CPU-Logik des nächsten Frames rendert.
 */
static void psx_end_frame(void)
{
    /* Auf laufende GPU-DMA warten */
    DrawSync(0);

    /* VSync: Warte auf VBlank (synchronisiert auf 30fps NTSC) */
    VSync(0);

    /* Framebuffer umschalten: Display ↔ Draw */
    PutDispEnv(&s_rpsx.fb[s_rpsx.fb_idx].disp);
    PutDrawEnv(&s_rpsx.fb[s_rpsx.fb_idx].draw);

    /* BSS-Hintergrund in den Framebuffer kopieren (vor OT-Traversal) */
    if (s_rpsx.bg_loaded) {
        RECT bg_src;
        RECT bg_dst;

        bg_src.x = s_rpsx.bg_vram_x;
        bg_src.y = s_rpsx.bg_vram_y;
        bg_src.w = PSX_SCREEN_W;
        bg_src.h = PSX_SCREEN_H;

        /* Ziel = aktuelle Draw-Area */
        bg_dst.x = s_rpsx.fb[s_rpsx.fb_idx].draw.clip.x;
        bg_dst.y = s_rpsx.fb[s_rpsx.fb_idx].draw.clip.y;
        bg_dst.w = PSX_SCREEN_W;
        bg_dst.h = PSX_SCREEN_H;

        MoveImage(&bg_src, bg_dst.x, bg_dst.y);
        DrawSync(0);
    }

    /* GPU-DMA-Submission: OT an die GPU senden (Painter's Algorithm).
     * DrawOTag traversiert vom letzten (höchsten Z = fern) zum ersten Eintrag.
     * Die PSX-OT ist reverse-linked: Eintrag[OT_DEPTH-1] ist der Head. */
    DrawOTag(&s_rpsx.fb[s_rpsx.fb_idx].ot[RE15_OT_DEPTH - 1]);

    /* Framebuffer-Index umschalten */
    s_rpsx.fb_idx ^= 1;
}

/**
 * Texturiertes/untexturiertes Dreieck in die OT einreichen.
 *
 * Erstellt ein POLY_GT3 (Gouraud-texturiert) oder POLY_G3 (Gouraud-untexturiert)
 * Primitiv und fügt es in die OT am gegebenen Z-Level ein.
 */
static void psx_submit_tri(const render_tri_t* tri)
{
    uint16_t z;

    if (!tri) return;

    z = tri->z;
    if (z >= RE15_OT_DEPTH) z = RE15_OT_DEPTH - 1;

    if (tri->flags & RE15_RENDER_FLAG_TEXTURED) {
        /* Texturiertes Gouraud-Dreieck (POLY_GT3) */
        POLY_GT3* pgt3 = (POLY_GT3*)psx_alloc_pri(sizeof(POLY_GT3));
        if (!pgt3) return;

        setPolyGT3(pgt3);

        /* Vertex 0 */
        setXY3(pgt3,
               tri->vertices[0].x, tri->vertices[0].y,
               tri->vertices[1].x, tri->vertices[1].y,
               tri->vertices[2].x, tri->vertices[2].y);

        setRGB0(pgt3, tri->vertices[0].r, tri->vertices[0].g, tri->vertices[0].b);
        setRGB1(pgt3, tri->vertices[1].r, tri->vertices[1].g, tri->vertices[1].b);
        setRGB2(pgt3, tri->vertices[2].r, tri->vertices[2].g, tri->vertices[2].b);

        setUV3(pgt3,
               tri->vertices[0].u, tri->vertices[0].v,
               tri->vertices[1].u, tri->vertices[1].v,
               tri->vertices[2].u, tri->vertices[2].v);

        pgt3->tpage = tri->tpage;
        pgt3->clut  = tri->clut;

        if (tri->flags & RE15_RENDER_FLAG_SEMI_TRANS) {
            SetSemiTrans(pgt3, 1);
        }

        addPrim(&s_rpsx.fb[s_rpsx.fb_idx].ot[z], pgt3);
    } else {
        /* Untexturiertes Gouraud-Dreieck (POLY_G3) */
        POLY_G3* pg3 = (POLY_G3*)psx_alloc_pri(sizeof(POLY_G3));
        if (!pg3) return;

        setPolyG3(pg3);

        setXY3(pg3,
               tri->vertices[0].x, tri->vertices[0].y,
               tri->vertices[1].x, tri->vertices[1].y,
               tri->vertices[2].x, tri->vertices[2].y);

        setRGB0(pg3, tri->vertices[0].r, tri->vertices[0].g, tri->vertices[0].b);
        setRGB1(pg3, tri->vertices[1].r, tri->vertices[1].g, tri->vertices[1].b);
        setRGB2(pg3, tri->vertices[2].r, tri->vertices[2].g, tri->vertices[2].b);

        if (tri->flags & RE15_RENDER_FLAG_SEMI_TRANS) {
            SetSemiTrans(pg3, 1);
        }

        addPrim(&s_rpsx.fb[s_rpsx.fb_idx].ot[z], pg3);
    }
}

/**
 * Texturiertes/untexturiertes Viereck in die OT einreichen.
 *
 * Erstellt ein POLY_GT4 (Gouraud-texturiert) oder POLY_G4 (Gouraud-untexturiert)
 * Primitiv und fügt es in die OT am gegebenen Z-Level ein.
 */
static void psx_submit_quad(const render_quad_t* quad)
{
    uint16_t z;

    if (!quad) return;

    z = quad->z;
    if (z >= RE15_OT_DEPTH) z = RE15_OT_DEPTH - 1;

    if (quad->flags & RE15_RENDER_FLAG_TEXTURED) {
        /* Texturiertes Gouraud-Viereck (POLY_GT4) */
        POLY_GT4* pgt4 = (POLY_GT4*)psx_alloc_pri(sizeof(POLY_GT4));
        if (!pgt4) return;

        setPolyGT4(pgt4);

        setXY4(pgt4,
               quad->vertices[0].x, quad->vertices[0].y,
               quad->vertices[1].x, quad->vertices[1].y,
               quad->vertices[2].x, quad->vertices[2].y,
               quad->vertices[3].x, quad->vertices[3].y);

        setRGB0(pgt4, quad->vertices[0].r, quad->vertices[0].g, quad->vertices[0].b);
        setRGB1(pgt4, quad->vertices[1].r, quad->vertices[1].g, quad->vertices[1].b);
        setRGB2(pgt4, quad->vertices[2].r, quad->vertices[2].g, quad->vertices[2].b);
        setRGB3(pgt4, quad->vertices[3].r, quad->vertices[3].g, quad->vertices[3].b);

        setUV4(pgt4,
               quad->vertices[0].u, quad->vertices[0].v,
               quad->vertices[1].u, quad->vertices[1].v,
               quad->vertices[2].u, quad->vertices[2].v,
               quad->vertices[3].u, quad->vertices[3].v);

        pgt4->tpage = quad->tpage;
        pgt4->clut  = quad->clut;

        if (quad->flags & RE15_RENDER_FLAG_SEMI_TRANS) {
            SetSemiTrans(pgt4, 1);
        }

        addPrim(&s_rpsx.fb[s_rpsx.fb_idx].ot[z], pgt4);
    } else {
        /* Untexturiertes Gouraud-Viereck (POLY_G4) */
        POLY_G4* pg4 = (POLY_G4*)psx_alloc_pri(sizeof(POLY_G4));
        if (!pg4) return;

        setPolyG4(pg4);

        setXY4(pg4,
               quad->vertices[0].x, quad->vertices[0].y,
               quad->vertices[1].x, quad->vertices[1].y,
               quad->vertices[2].x, quad->vertices[2].y,
               quad->vertices[3].x, quad->vertices[3].y);

        setRGB0(pg4, quad->vertices[0].r, quad->vertices[0].g, quad->vertices[0].b);
        setRGB1(pg4, quad->vertices[1].r, quad->vertices[1].g, quad->vertices[1].b);
        setRGB2(pg4, quad->vertices[2].r, quad->vertices[2].g, quad->vertices[2].b);
        setRGB3(pg4, quad->vertices[3].r, quad->vertices[3].g, quad->vertices[3].b);

        if (quad->flags & RE15_RENDER_FLAG_SEMI_TRANS) {
            SetSemiTrans(pg4, 1);
        }

        addPrim(&s_rpsx.fb[s_rpsx.fb_idx].ot[z], pg4);
    }
}

/**
 * BSS-Hintergrund anzeigen: MDEC-Dekodierung des 64KB-Chunks.
 *
 * Dekodiert den VLC-komprimierten Hintergrund über die MDEC-Hardware
 * und platziert das Ergebnis im VRAM-Cache (640,0). Das Blit in den
 * aktiven Framebuffer erfolgt in psx_end_frame().
 *
 * @param bss_chunk  Pointer auf 64KB BSS-Daten (VLC-Bitstream)
 * @param cut_id     Kamera-Cut-ID (für Debug-Ausgabe)
 */
static void psx_show_background(const uint8_t* bss_chunk, int cut_id)
{
    if (!bss_chunk) {
        s_rpsx.bg_loaded = 0;
        return;
    }

    /* GPU-Pipeline leeren bevor MDEC-Decode den VRAM beschreibt */
    DrawSync(0);

    /* MDEC-Dekodierung in den VRAM-Cache */
    if (psx_mdec_decode_bss(bss_chunk, s_rpsx.bg_vram_x, s_rpsx.bg_vram_y) == 0) {
        s_rpsx.bg_loaded = 1;
    } else {
        printf("[RE15 RENDER] MDEC-Dekodierung fehlgeschlagen (cut %d)\n", cut_id);
        s_rpsx.bg_loaded = 0;
    }
}

/**
 * 2D-Sprite rendern: Erstellt ein SPRT-Primitiv in der OT.
 *
 * @param x,y     Bildschirmposition
 * @param w,h     Sprite-Größe in Pixeln
 * @param tpage   Texture-Page (VRAM-Position/Format)
 * @param clut    Color Look-Up Table
 * @param z       OT-Tiefenwert
 */
static void psx_draw_sprite(int x, int y, int w, int h,
                             int tpage, int clut, int z)
{
    SPRT* sprt;
    uint16_t clamped_z;

    clamped_z = (uint16_t)z;
    if (clamped_z >= RE15_OT_DEPTH) clamped_z = RE15_OT_DEPTH - 1;

    sprt = (SPRT*)psx_alloc_pri(sizeof(SPRT));
    if (!sprt) return;

    setSprt(sprt);
    setXY0(sprt, x, y);
    setWH(sprt, w, h);
    setRGB0(sprt, 128, 128, 128);  /* Neutral-Farbe (keine Modulation) */
    sprt->clut = (uint16_t)clut;

    addPrim(&s_rpsx.fb[s_rpsx.fb_idx].ot[clamped_z], sprt);

    /* TPage-Änderung als separates DR_TPAGE-Primitiv einfügen */
    {
        DR_TPAGE* tp = (DR_TPAGE*)psx_alloc_pri(sizeof(DR_TPAGE));
        if (tp) {
            setDrawTPage(tp, 0, 0, (uint16_t)tpage);
            addPrim(&s_rpsx.fb[s_rpsx.fb_idx].ot[clamped_z], tp);
        }
    }
}

/**
 * Letterbox-Balken setzen.
 *
 * Auf der PSX wird dies durch Anpassung der DRAWENV-Clip-Region und
 * Einfügen schwarzer TILE-Primitive in die OT realisiert.
 *
 * @param height  Höhe der Balken in Pixeln (0 = deaktiviert)
 */
static void psx_set_letterbox(int height)
{
    s_rpsx.letterbox_height = height;

    if (height <= 0) return;

    /* Schwarze Balken oben und unten als TILE-Primitive */
    {
        TILE* tile_top = (TILE*)psx_alloc_pri(sizeof(TILE));
        TILE* tile_bot = (TILE*)psx_alloc_pri(sizeof(TILE));

        if (tile_top) {
            setTile(tile_top);
            setXY0(tile_top, 0, 0);
            setWH(tile_top, PSX_SCREEN_W, height);
            setRGB0(tile_top, 0, 0, 0);
            /* Z=0 = nahster Punkt, wird als letztes gezeichnet (über allem) */
            addPrim(&s_rpsx.fb[s_rpsx.fb_idx].ot[0], tile_top);
        }

        if (tile_bot) {
            setTile(tile_bot);
            setXY0(tile_bot, 0, PSX_SCREEN_H - height);
            setWH(tile_bot, PSX_SCREEN_W, height);
            setRGB0(tile_bot, 0, 0, 0);
            addPrim(&s_rpsx.fb[s_rpsx.fb_idx].ot[0], tile_bot);
        }
    }
}

/**
 * Screen-Fade-Level setzen.
 *
 * Auf der PSX wird ein semi-transparentes schwarzes TILE über den
 * gesamten Bildschirm gelegt. Da die PSX kein Alpha-Blending mit
 * variablem Wert unterstützt, wird B/2+F/2-Modus verwendet (ABR=0)
 * bei vollem Schwarz für einen 50%-Fade. Für andere Level wird ein
 * Wiederholungs-Muster genutzt.
 *
 * @param level  0 = kein Fade, 255 = komplett schwarz
 */
static void psx_set_fade(int level)
{
    TILE* fade_tile;

    if (level < 0) level = 0;
    if (level > 255) level = 255;
    s_rpsx.fade_level = level;

    if (level == 0) return;

    /* Einfacher Fade: Schwarzes TILE mit Semi-Transparenz */
    fade_tile = (TILE*)psx_alloc_pri(sizeof(TILE));
    if (!fade_tile) return;

    setTile(fade_tile);
    setXY0(fade_tile, 0, 0);
    setWH(fade_tile, PSX_SCREEN_W, PSX_SCREEN_H);

    /* Farbe basierend auf Fade-Level:
     * PSX ABR=2 (B+F) mit negativer Farbe simuliert Verdunklung.
     * Für einfache Implementierung: Schwarz mit Semi-Transparenz.
     * level=255 → voll undurchsichtig (kein STP). */
    if (level >= 255) {
        setRGB0(fade_tile, 0, 0, 0);
        /* Kein SemiTrans → voll deckend schwarz */
    } else {
        setRGB0(fade_tile, 0, 0, 0);
        SetSemiTrans(fade_tile, 1);
    }

    /* In Z=0 einfügen (wird über allem gezeichnet) */
    addPrim(&s_rpsx.fb[s_rpsx.fb_idx].ot[0], fade_tile);
}

/**
 * Backend herunterfahren: GPU-Pipeline leeren und Display ausschalten.
 * Auf der PSX gibt es keinen dynamisch allozierten Speicher freizugeben,
 * da alle Puffer statisch sind.
 */
static void psx_shutdown(void)
{
    if (!s_rpsx.initialized) return;

    /* GPU-Pipeline leeren */
    DrawSync(0);

    /* Display ausschalten */
    SetDispMask(0);

    s_rpsx.initialized = 0;
    s_rpsx.bg_loaded = 0;

    printf("[RE15 RENDER] PSX-Backend heruntergefahren.\n");
}

/* ============================================================================
 * Exportierte Backend-Instanz
 * ========================================================================= */

const render_backend_t render_psx_backend = {
    .init            = psx_init,
    .begin_frame     = psx_begin_frame,
    .end_frame       = psx_end_frame,
    .submit_tri      = psx_submit_tri,
    .submit_quad     = psx_submit_quad,
    .show_background = psx_show_background,
    .draw_sprite     = psx_draw_sprite,
    .set_letterbox   = psx_set_letterbox,
    .set_fade        = psx_set_fade,
    .shutdown        = psx_shutdown,
};
