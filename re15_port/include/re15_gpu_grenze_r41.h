#ifndef RE15_GPU_GRENZE_R41_H
#define RE15_GPU_GRENZE_R41_H

/* Runde 41 Spur Q (UEBERGABE 3b, Dossier analysis/befunde_runde41/Q_restpunkte.md Punkt 3): Schutz des CPU-Rasterers von SDL vor riesigen Dreiecken, nach dem Vorbild dessen, was die
 * PSX-Hardware mit grossen Dreiecken tut. Er gilt NUR fuer SDLs CPU-Rasterer (RE15_SOFTWARE_RENDER oder Rueckfall ohne GPU): im beschleunigten Renderer des Spielers tritt der Fehler nicht
 * auf, und Port-eigene grosse Flaechen (Wasserflaechen als Prop) sind nicht gegen diese Hardware-Grenze gebaut.
 *
 * GTE (psx-spx geometrytransformationenginegte.md Z. 418/419): "ScrX FIFO -400h..+3FFh", "ScrY FIFO -400h..+3FFh" = Bildkoordinaten saettigen. Der Modell-Zeichner des Originals
 * prueft dafuer kein FLAG (@0x80025640 AVSZ3, @0x80025658 beq nur auf otz>>6 == 0), die Dreiecke gehen so an die GPU.
 * GPU (psx-spx graphicsprocessingunitgpu.md, "Size Restriction"): "The maximum distance between two vertices is 1023 horizontally, and 511 vertically. Polygons and lines that are
 * exceeding that dimensions are NOT rendered." Ein Vierer-Polygon besteht auf der GPU aus zwei Dreiecken, die Pruefung gilt je Dreieck.
 *
 * Warum das im Port wichtig ist: ohne beides gingen Effekt-Quader kurz vor der Kamera mit Koordinaten bis 10 250 an den Renderer (ROOM20B0 F755..F767, ESP 0x0f, Dreiecksflaeche bis
 * 1,1e9). Der CPU-Rasterer von SDL 2.28.5 rechnet `s2_x_area.x = s2->x * area` (SDL_triangle.c:618) in 32 Bit, oberhalb 2^31/255 laufen die Texturkoordinaten aus der Textur:
 * SDL_BlitTriangle_Slow las dann fremden Speicher, das waren die stillen Enden von integration_r41_ventilkette_kette. Saettigung + Grenze halten die Flaeche bei hoechstens
 * 4 * 1023 * 511, unter dem Ueberlauf.
 *
 * Effekt-Quader (Integration Q+R, Z2: Dossier Q OFFEN 1, Entscheid des Orchestrators): fuer die ESP-Quader gilt die GPU-Grenze in BEIDEN Renderern, denn das ist Hardwareverhalten und kein Schutz vor
 * dem CPU-Rasterer. Ihre Ecken sind keine GTE-Ausgaben, darum nicht re15_gpu_dreieck_verwerfen: FUN_800534c4 holt nur die Mitte aus der GTE (RTPS @0x80053534, SXY2 @0x8005354c, gesaettigt) und
 * rechnet die Ecken in der CPU ohne Klemme (@0x800536b0 `addu a0,s6,a0`, @0x800536e0 `addu t1,s0,a0`, je `srl ..,16` @0x800536e4/e8, Paket @0x80053718-28). Die einzige Sperre davor ist SZ3 < 4
 * @0x8005357c. Das Paket traegt 16 Bit, die GPU liest davon 11: psx-spx graphicsprocessingunitgpu.md "Vertex": "0-10 X-coordinate (signed, -1024..+1023)", "11-15 Not used (usually sign-extension,
 * but ignored by hardware)", Y ebenso. Die Size Restriction oben gilt fuer diese Werte. */
#include <stdio.h>

#define RE15_GPU_MAX_DX 1023   /* psx-spx: horizontal */
#define RE15_GPU_MAX_DY 511    /* psx-spx: vertikal   */
#define RE15_GTE_SXY_MIN (-0x400)   /* psx-spx: ScrX/ScrY FIFO */
#define RE15_GTE_SXY_MAX 0x3FF

static inline int re15_gte_saettigen(int v) { return v < RE15_GTE_SXY_MIN ? RE15_GTE_SXY_MIN : (v > RE15_GTE_SXY_MAX ? RE15_GTE_SXY_MAX : v); }

/* Bildkoordinate, wie die GPU sie aus dem Paket liest (psx-spx "Vertex", Bits 0-10 mit Vorzeichen). */
static inline int re15_gpu_vertex(int v) { return ((v & 0x7FF) ^ 0x400) - 0x400; }

static inline int re15_gpu_dreieck_zu_gross(int x0, int y0, int x1, int y1, int x2, int y2)
{
    const int xa = x0 < x1 ? (x0 < x2 ? x0 : x2) : (x1 < x2 ? x1 : x2);
    const int xb = x0 > x1 ? (x0 > x2 ? x0 : x2) : (x1 > x2 ? x1 : x2);
    const int ya = y0 < y1 ? (y0 < y2 ? y0 : y2) : (y1 < y2 ? y1 : y2);
    const int yb = y0 > y1 ? (y0 > y2 ? y0 : y2) : (y1 > y2 ? y1 : y2);
    return (xb - xa) > RE15_GPU_MAX_DX || (yb - ya) > RE15_GPU_MAX_DY;
}

/* ESP-Quader mit CPU-Ecken: Ecken wie die GPU sie liest, 1 = verwerfen. Das Original hat keine Klemme, ein Quader jenseits von 11 Bit kommt gewickelt an (auch gespiegelt, x1 < x0) und wird in dieser
 * Lage gezeichnet, wenn er unter der Grenze liegt. Ein Vierer-Polygon sind zwei Dreiecke, bei diesem achsparallelen Viereck haben beide dieselbe Ausdehnung. */
static inline int re15_gpu_quad_verwerfen(int *x0, int *y0, int *x1, int *y1)
{
    *x0 = re15_gpu_vertex(*x0); *y0 = re15_gpu_vertex(*y0);
    *x1 = re15_gpu_vertex(*x1); *y1 = re15_gpu_vertex(*y1);
    return re15_gpu_dreieck_zu_gross(*x0, *y0, *x1, *y0, *x0, *y1);
}

/* Fuer den Renderer: saettigt die Ecken wie die GTE, 1 = verwerfen wie die GPU; die ersten 16 je Prozess stehen im Log. */
static inline int re15_gpu_dreieck_verwerfen(unsigned bild, int slot, int *x0, int *y0, int *x1, int *y1, int *x2, int *y2)
{
    static int s_n = 0;
    *x0 = re15_gte_saettigen(*x0); *y0 = re15_gte_saettigen(*y0);
    *x1 = re15_gte_saettigen(*x1); *y1 = re15_gte_saettigen(*y1);
    *x2 = re15_gte_saettigen(*x2); *y2 = re15_gte_saettigen(*y2);
    if (!re15_gpu_dreieck_zu_gross(*x0, *y0, *x1, *y1, *x2, *y2)) return 0;
    if (s_n < 16) { s_n++; fprintf(stderr, "[gpu-grenze] F%u Slot %d Dreieck (%d,%d) (%d,%d) (%d,%d) verworfen\n", bild, slot, *x0, *y0, *x1, *y1, *x2, *y2); }
    return 1;
}

#endif
