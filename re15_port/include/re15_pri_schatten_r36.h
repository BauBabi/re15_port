/*
 * RE1.5 Rebuilt — Figurenschatten in der Zeichenreihenfolge der Vordergrundmasken (Runde 36 Spur O).
 *
 * NUTZER-BEFUND (AUFTRAG.md Z.58): "ROOM 1100: Leon wird mit einer Halbtransparenz ueberdeckt mit
 * Boden." Gemessen (Dossier analysis/befunde_runde36/O_pri.md §2): der KOERPER wird von den ROOM1100-
 * Wandmasken an keinem begehbaren Standplatz verdeckt (Modell 0 von 561, drei A/B-Durchlaeufe mit
 * RE15_NO_PRI); was im Port ueberdeckt wird, ist der halbtransparente SCHATTEN (ABR2-Subtraktiv-Quad,
 * FUN_8001af5c SetSemiTrans @0x8001afb4) — die Masken am Wandfuss (ROOM1100.MSK Cut 1/2, Zellen-
 * polster 85..135 Einheiten = Bodenpixel) blitten Boden ueber ihn, weil der PC-Renderer ALLE Schatten
 * vor allen Masken und Dreiecken malte (render_pc.c "Step 1.5"), das Original den Schatten aber als
 * Prim in die Ordnungstabelle haengt.
 *
 * ORIGINAL (PSX.EXE, ghidra1_V2.txt):
 *   FUN_8001b064 (Spieler-Schatten, einziger Aufrufer FUN_80031c44 @0x80031dcc):
 *     @0x8001b2c8  jal RotAverage4          -> OTZ = AVSZ4 ueber die vier Ecken
 *     @0x8001b2d8  srl v0,v0,0x4            -> OT-Index = otz >> 4   (wie die Figur @0x8002565c)
 *     @0x8001b2e0  sll v0,v0,0x2            -> *4 = Wortoffset
 *     @0x8001b30c-0x8001b318  addPrim in DAT_800aa6d8[index] (Kopf-Einhaengung)
 *   ZSF4 = 256: @0x80066c7c li t0,0x100 / @0x80066c80 gte_ldZSF4  -> otz = (256*(sz0+sz1+sz2+sz3))>>12
 *     = (sz0+..+sz3)>>4, mit sz = Kamera-z der Ecke -> OT-Index = (Summe der vier Kamera-z) >> 8.
 *   Masken: FUN_80039590, OT-Index = depth (@0x80039658), addPrim @0x80039660.
 *   Reihenfolge im Bildaufbau (eine Funktion, Jump-Kette LAB_8001cb78..LAB_8001d158):
 *     @0x8001ce0c jal FUN_80031c44   Spieler (am Ende @0x80031dcc der Schatten)
 *     @0x8001ce54 jal FUN_80039590   Masken
 *     @0x8001d09c jal FUN_8001e8c8   Spieler-Polygone
 *     @0x8001d0e8..0x8001d164        Figuren-Schleife (NPC-Polygone, @0x8001d108)
 *   ClearOTagR + Kopf-Einhaengung (AddPrim): im SELBEN Bucket wird das ZULETZT eingehaengte Prim
 *   ZUERST gezeichnet. Spieler-Schatten (vor den Masken eingehaengt) liegt im gleichen Bucket also
 *   OBEN auf der Maske; ein Prim, das NACH den Masken eingehaengt wird, liegt darunter.
 *
 * PORT-REGEL (dieselbe Ordnung, Buckets statt OT):
 *   schatten_bucket = (vz0+vz1+vz2+vz3) >> 8
 *   Maske ueber Schatten  <=>  depth < schatten_bucket, bei Gleichstand nur fuer NICHT-Spieler-Schatten.
 *   Dreieck ueber Schatten <=> bucket(tri) < schatten_bucket, bei Gleichstand nur fuer Nicht-Spieler
 *   (bucket(tri) = (1023*vz)>>16, re15_pri_bucket_of_vz; der Spieler-Schatten wird vor den Spieler-
 *   Polygonen @0x8001d09c eingehaengt und liegt im gleichen Bucket oben).
 *   NPC-Schatten: der Port zeichnet sie ueber denselben Weg (platform/pc/main.c, Figuren-Schleife); ihr
 *   Original-Emitter ist nicht belegt (FUN_8001b064 hat nur den einen Aufrufer) — fuer sie gilt bei
 *   Gleichstand "unten" = das bisherige Port-Verhalten.
 */
#ifndef RE15_PRI_SCHATTEN_R36_H
#define RE15_PRI_SCHATTEN_R36_H

#include <stdint.h>
#include "re15_pri.h"

/* OT-Index des Schattens aus der Summe der vier Ecken-Kamera-z (AVSZ4, ZSF4=256 @0x80066c7c/80,
 * srl 4 @0x8001b2d8). Negative Summe = kein Schatten (nie der Fall nach der Nahklemme vz >= 64). */
static inline int re15_schatten_bucket(int32_t vz_summe4)
{
    if (vz_summe4 < 0) return -1;
    return (int)(vz_summe4 >> 8);
}

/* Liegt die Maske `depth` UEBER dem Schatten (wird nach ihm gezeichnet)? */
static inline int re15_schatten_maske_darueber(int depth, int schatten_bucket, int ist_spieler)
{
    if (depth < schatten_bucket) return 1;
    if (depth == schatten_bucket && !ist_spieler) return 1;
    return 0;
}

/* Liegt ein Figur-Dreieck der Kamera-z `vz` UEBER dem Schatten? */
static inline int re15_schatten_dreieck_darueber(long tri_vz, int schatten_bucket, int ist_spieler)
{
    int tb = re15_pri_bucket_of_vz(tri_vz);
    if (tb < schatten_bucket) return 1;
    if (tb == schatten_bucket && !ist_spieler) return 1;
    return 0;
}

/* PC-Renderer: Tiefe fuer das NAECHSTE re15_render_shadow_quad(_c) setzen (verbraucht sich).
 * vz_summe4 < 0 = untagged -> alter Weg (vor allen Dreiecken und Masken). */
void re15_render_pc_shadow_tiefe(int32_t vz_summe4, int ist_spieler);

#endif /* RE15_PRI_SCHATTEN_R36_H */
