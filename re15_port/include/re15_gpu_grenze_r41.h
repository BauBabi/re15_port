#ifndef RE15_GPU_GRENZE_R41_H
#define RE15_GPU_GRENZE_R41_H

/* Runde 41 Spur Q (UEBERGABE 3b, Dossier analysis/befunde_runde41/Q_restpunkte.md Punkt 3): Groessengrenze der PSX-GPU fuer Dreiecke.
 *
 * Hardware (psx-spx graphicsprocessingunitgpu.md, "Size Restriction"): "The maximum distance between two vertices is 1023 horizontally, and 511 vertically.
 * Polygons and lines that are exceeding that dimensions are NOT rendered." Ein Vierer-Polygon besteht auf der GPU aus zwei Dreiecken, die Pruefung gilt je Dreieck.
 *
 * Warum das im Port wichtig ist: ohne die Grenze gingen Effekt-Quader kurz vor der Kamera mit Koordinaten bis 10 250 an den Renderer (ROOM20B0 F755..F767, ESP 0x0f,
 * Dreiecksflaeche bis 1,1e9). Der CPU-Rasterer von SDL 2.28.5 rechnet `s2_x_area.x = s2->x * area` (SDL_triangle.c:618) in 32 Bit, oberhalb 2^31/255 laufen die
 * Texturkoordinaten aus der Textur: SDL_BlitTriangle_Slow las dann fremden Speicher, das waren die stillen Enden von integration_r41_ventilkette_kette. Die GPU-Grenze
 * haelt die Flaeche bei hoechstens 4 * 1023 * 511, unter dem Ueberlauf. Die Saettigung der GTE (SX/SY auf -1024..1023) bildet der Port nicht nach: hier zaehlt der
 * Abstand der ungesaettigten Koordinaten; eine Ecke jenseits der Saettigung faellt damit weg, wo die Hardware ein verzerrtes Dreieck zeichnen koennte. */
#define RE15_GPU_MAX_DX 1023   /* psx-spx: horizontal */
#define RE15_GPU_MAX_DY 511    /* psx-spx: vertikal   */

static inline int re15_gpu_dreieck_zu_gross(int x0, int y0, int x1, int y1, int x2, int y2)
{
    const int xa = x0 < x1 ? (x0 < x2 ? x0 : x2) : (x1 < x2 ? x1 : x2);
    const int xb = x0 > x1 ? (x0 > x2 ? x0 : x2) : (x1 > x2 ? x1 : x2);
    const int ya = y0 < y1 ? (y0 < y2 ? y0 : y2) : (y1 < y2 ? y1 : y2);
    const int yb = y0 > y1 ? (y0 > y2 ? y0 : y2) : (y1 > y2 ? y1 : y2);
    return (xb - xa) > RE15_GPU_MAX_DX || (yb - ya) > RE15_GPU_MAX_DY;
}

#endif
