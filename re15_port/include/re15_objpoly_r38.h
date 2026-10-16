/*
 * RE1.5 Rebuilt — Runde 38 Spur W "rolltor20a0": die Polygon-Schranken des ORIGINAL-Objektzeichners fuer die
 * Raum-Objekte (Obj_model_set) im PC-Zeichenweg.
 *
 * NUTZER-BEFUND (2026-10-16): "in ROOM 20A0 the shutter that you have to open to push the box down is somehow
 * DIRECTLY in the camera. I cannot see anything... this is not like this in the original."
 *
 * MESSUNG (Dossier analysis/befunde_runde38/W_rolltor20a0.md): der Rolladen ist obj 1 (Obj_model_set @0x01932,
 * Typ 0, Attribut 0x10, 48 Vierecke). Offen (y -10800, sub07 @0x01E40) steht er unmittelbar um das Objektiv der
 * Kamera von Cut 11 (von (9648,-12691,-20700)). Der Port zeichnete jedes Dreieck mit allen Scheiteln vor der
 * Kamera ungesaettigt und ohne Rueckseiten-Test -> das ganze Bild voller Lamellen. Das Original (DuckStation-GDB,
 * Haltepunkte im Zeichner) verwirft 20 Vierecke an NCLIP, 16 an OTZ < 64, die eingehaengten Reste sind fuer die GPU
 * zu gross oder liegen ausserhalb des Bilds -> Cut 11 zeigt die Plattform ohne Rolladen.
 *
 * DER ORIGINAL-WEG (FUN_8002c18c -> vier Zeichner, alle mit denselben zwei Schranken, PSX.EXE disassembliert):
 *   Flach-Dreieck FUN_80022f0c:  RTPT @0x80022fb0, NCLIP @0x80022fec, `bltz v0` @0x80023010 (OPZ < 0 -> weg),
 *                                AVSZ3 @0x80023020, `srl v0,v1,6` @0x80023044 + `beq v0,zero` @0x80023048
 *   Flach-Viereck FUN_80023098:  RTPT @0x80023148, NCLIP @0x80023168, `bgez v0` @0x80023190 (OPZ >= 0 -> weiter),
 *                                RTPS @0x800231f8, AVSZ4 @0x8002320c, `srl v0,v1,6` @0x80023224 + `beq` @0x80023228
 *   Licht-Dreieck FUN_800254a0:  RTPT @0x8002554c, NCLIP @0x80025580, `bgez v0` @0x800255a8,
 *                                AVSZ3 @0x80025640, `sra v0,v1,6` @0x80025654 + `beq` @0x80025658
 *   Licht-Viereck FUN_800256b0:  RTPT @0x80025764, NCLIP @0x80025798, `bgez v0` @0x800257c0,
 *                                RTPS @0x80025878, AVSZ4 @0x8002589c, `sra v0,v0,6` @0x800258b0 + `beq` @0x800258b4
 *   OTZ >> 6 == 0 heisst OTZ < 64 -> nicht in die OT gehaengt (RE15_PRI_OTZ_NEAR_DROP, re15_pri.h).
 *   GTE (InitGeom): ZSF3 = 0x155 = 341 (@0x80066c70 `li t0,0x155` / @0x80066c74 gte_ldZSF3), ZSF4 = 0x100 = 256
 *   (@0x80066c7c `li t0,0x100` / @0x80066c80 gte_ldZSF4); Bildmitte OFX/OFY = SetGeomOffset(0xa0,0x78)
 *   @0x8001643c/@0x80016440.
 *   GTE-Arithmetik (psx-spx geometrytransformationenginegte.md, RTPS/NCLIP/AVSZ):
 *     IR = MAC gesaettigt auf -8000h..7FFFh (lm = 0 in RTPT 0x4a280030 / RTPS 0x4a180001)
 *     SZ3 = MAC3 gesaettigt 0..FFFFh ("negative Z coordinates are saturated to SZ3=0")
 *     SX2 = (n*IR1 + OFX) / 10000h gesaettigt -400h..+3FFh, SY2 ebenso (n = re15_gte_divide, max 1FFFFh)
 *     NCLIP: MAC0 = SX0*SY1 + SX1*SY2 + SX2*SY0 - SX0*SY2 - SX1*SY0 - SX2*SY1
 *     AVSZ3: OTZ = ZSF3*(SZ1+SZ2+SZ3) / 1000h, AVSZ4: OTZ = ZSF4*(SZ0+SZ1+SZ2+SZ3) / 1000h (gesaettigt 0..FFFFh)
 *   GPU (psx-spx graphicsprocessingunitgpu.md "Vertex"): "Size Restriction: The maximum distance between two
 *     vertices is 1023 horizontally, and 511 vertically. Polygons and lines that are exceeding that dimensions are
 *     NOT rendered." Ein Viereck rastert die GPU als zwei Dreiecke (0,1,2) und (1,2,3); jedes wird fuer sich
 *     geprueft (Haltepunkt-Messung Q07: ein Dreieck zu gross, das andere nicht).
 *
 * Diese Datei rechnet genau das; der PC-Zeichenweg (platform/pc/main.c, Prop-Schleife) ruft sie statt seines
 * bisherigen Scheitel-Ausschnitts `_vz < 64` (Port-Erfindung "H28", ohne Original-Gegenstueck). Die Scheitel kommen
 * als Sichtraum-Koordinaten MAC1..3 (Kamera x Objektmatrix, wie bisher in main.c gerechnet).
 */
#ifndef RE15_OBJPOLY_R38_H
#define RE15_OBJPOLY_R38_H

#include <stdint.h>
#include "re15_md1.h"

#define RE15_OBJPOLY_ZSF3        341    /* 0x155 @0x80066c70 / gte_ldZSF3 @0x80066c74 */
#define RE15_OBJPOLY_ZSF4        256    /* 0x100 @0x80066c7c / gte_ldZSF4 @0x80066c80 */
#define RE15_OBJPOLY_OTZ_MIN     64     /* OTZ >> 6 != 0: @0x80023224/28, @0x80023044/48, @0x80025654/58, @0x800258b0/b4 */
#define RE15_OBJPOLY_SXY_MIN     (-0x400)  /* psx-spx RTPS "ScrX FIFO -400h..+3FFh" */
#define RE15_OBJPOLY_SXY_MAX     0x3FF
#define RE15_OBJPOLY_GPU_DX_MAX  1023   /* psx-spx GPU "Size Restriction": 1023 horizontal */
#define RE15_OBJPOLY_GPU_DY_MAX  511    /*                                  511 vertikal    */

/* Ein Objekt-Polygon nach dem Original-Weg. */
typedef struct {
    int16_t  sx[4], sy[4];   /* gesaettigte Bildkoordinaten (GTE SX2/SY2) */
    uint16_t sz[4];          /* SZ3 je Scheitel (0..FFFFh) */
    int32_t  opz;            /* NCLIP MAC0 (Scheitel 0,1,2) */
    int32_t  otz;            /* AVSZ3 / AVSZ4 */
    uint8_t  tri_ok[2];      /* GPU-Groessengrenze: [0] = Dreieck (0,1,2), [1] = Dreieck (1,2,3) (nur Viereck) */
} re15_objpoly_t;

/* RTPS eines Scheitels aus seinen Sichtraum-Koordinaten (MAC1..3). h = GTE H (fov >> 7), ofx/ofy = Bildmitte. */
void re15_objpoly_r38_rtps(int32_t mac1, int32_t mac2, int32_t mac3, int h, int ofx, int ofy,
                           int16_t *sx, int16_t *sy, uint16_t *sz);

/* 1 = das Dreieck (x0..y2) wird von der GPU gerastert (psx-spx Size Restriction), 0 = verworfen. */
int re15_objpoly_r38_gpu_ok(int x0, int y0, int x1, int y1, int x2, int y2);

/* nclip: 1 = Rueckseiten-Test wie das Original (RDT-Modelle). 0 = PORT-WAHL fuer eingebackene Port-Modelle (nicht aus
 * der RDT): die haben keine verbindliche Windung (Zensus unit_r38_rolltor20a0_windung: Feuerloescher R37 112 von 148
 * Polygonen verkehrt herum) und bleiben zweiseitig wie bisher; OTZ-Schranke, Saettigung und GPU-Grenze gelten auch
 * fuer sie (Engine/Hardware, unabhaengig von der Windung). */

/* Dreieck (FUN_80022f0c / FUN_800254a0). mac[v] = Sichtraum (x,y,z) der Scheitel 0..2.
 * Rueckgabe 1 = in die OT (NCLIP >= 0 und OTZ >= 64) UND von der GPU gerastert; out wird immer gefuellt. */
int re15_objpoly_r38_tri(const int32_t mac[3][3], int h, int ofx, int ofy, int nclip, re15_objpoly_t *out);

/* Viereck (FUN_80023098 / FUN_800256b0). mac[v] = Sichtraum der Scheitel 0..3 (MD1-Reihenfolge = Original).
 * Rueckgabe 1 = in die OT (NCLIP >= 0 und OTZ >= 64); welche der beiden GPU-Dreiecke (0,1,2)/(1,2,3) gerastert
 * werden, steht in out->tri_ok[]. 0 = verworfen. */
int re15_objpoly_r38_quad(const int32_t mac[4][3], int h, int ofx, int ofy, int nclip, re15_objpoly_t *out);

/* Zeichenweg-Helfer fuer die Prop-Schleife (platform/pc/main.c): Scheitel (MD1, Objektraum) mit der Kamera x Objekt-
 * Matrix bm (Q12) + bt in den Sichtraum (MAC1..3 = ((R*V) >> 12) + T, wie bisher in main.c), dann die Schranken.
 * ax/ay = gesaettigte Bildkoordinaten, *wz = Mittel der SZ (Sortierschluessel, wie bisher der Mittelwert der
 * Sicht-z; fuer jedes bisher gezeichnete Dreieck identisch). Rueckgabe wie re15_objpoly_r38_tri/_quad;
 * beim Viereck tri_ok[0]/[1] = GPU-Dreieck (0,1,2)/(1,2,3). */
int re15_objpoly_r38_tri_mesh(const re15_md1_vertex_t *const v[3], const int32_t bm[9], const int32_t bt[3],
                              int h, int ofx, int ofy, int nclip, int ax[3], int ay[3], int *wz);
int re15_objpoly_r38_quad_mesh(const re15_md1_vertex_t *const v[4], const int32_t bm[9], const int32_t bt[3],
                               int h, int ofx, int ofy, int nclip, int ax[4], int ay[4], int *wz, uint8_t tri_ok[2]);

/* NUR PC (platform/pc/src/objpoly_r38_pc.c): Viereck, von dem genau ein GPU-Dreieck (0,1,2)/(1,2,3) besteht ->
 * nur dieses zeichnen. rgb = je Scheitel 0..3 die Farbe (r,g,b). */
void re15_objpoly_r38_halbviereck_pc(const int ax[4], const int ay[4], const re15_md1_quad_uv_t *uv, int page_off,
                                     int wz, const uint8_t gpu_ok[2], const uint8_t rgb[12]);

/* Messschiene RE15_OBJPOLY_LOG=<datei> (Diagnose, kein Verhalten): der Zeichenweg meldet den Beginn jedes Objekts;
 * je Bild und Objekt schreibt die Datei eine Zeile "F<bild> room=<raum> cut=<cut> obj=<id> gezeichnet=<n>
 * nclip=<n> otz=<n> gpu=<n>" (Polygone nach Grund; ein Viereck zaehlt einmal). */
void re15_objpoly_r38_objekt(int obj_id, int cut, unsigned room);

#endif /* RE15_OBJPOLY_R38_H */
