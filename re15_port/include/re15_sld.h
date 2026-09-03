/*
 * RE1.5 Rebuilt — SLD-Vordergrundatlas (sprite.pri-Textur) aus dem BSS-Chunk.
 *
 * WOZU: Jeder Kamera-Cut kann hinter dem MDEC-Hintergrund einen SLD-komprimierten
 * 8-bpp-Sony-TIM tragen — die Textur, aus der die sprite.pri-Masken sampeln
 * (Gelaender, Tische, Saeulen, die die Figur verdecken). Ohne diese Textur zeichnet
 * der Port fuer den ganzen Cut nichts, egal wie viele Maskenrecords im RDT stehen.
 *
 * ⛔ DAS ORIGINAL SUCHT DEN BLOCK NICHT. Es liest ihn ueber einen Trailer, dessen
 * Position aus einer Tabelle im STAGE-Overlay kommt. Der frueher benutzte
 * Heuristik-Scan ("irgendein u32 in [0x2000,0x40000], das sich zu einem TIM
 * entpacken laesst") ist NICHT byte-true — er ueberdetektiert Altdatenreste
 * jenseits der geladenen Laenge (Beleg: ROOM305 Cut 14).
 *
 * DIE KETTE, Instruktion fuer Instruktion (FUN_80021bbc = Kamera-Cut anwenden,
 * der einzige Aufrufer des Dekompressors):
 *
 *   ; L = u16 tabelle[raum*0x20 + cut*2]      (Tabellenzeiger = DAT_800b52c8)
 *   80021d44: lh    v1,4068(v1)        ; Cut-Index   (DAT_800b0fe4)
 *   80021d48: lh    v0,0(s0)           ; Raum-Index  (DAT_800b0fe2)
 *   80021d4c: sll   v1,v1,1            ; cut*2       -> 16 Spalten je Zeile
 *   80021d50: sll   v0,v0,5            ; raum*0x20   -> Zeilen-Stride 0x20
 *   80021d58: addu  v1,v1,v0
 *   80021d5c: lhu   v0,0(v1)           ; L = Byte-Laenge der geladenen Cut-Daten
 *   80021d68: addu  at,v0,at           ; at = 0x80190000 + L   (BSS-Cut-Puffer)
 *   80021d6c: lw    v0,-4(at)          ; PRESENT-Flag = u32 chunk[L-4]
 *   80021d74: beq   v0,zero,0x80021df8 ; == 0 -> dieser Cut hat KEINEN Vordergrund
 *   80021da4: lw    s0,-8(at)          ; SLD-OFFSET  = u32 chunk[L-8]
 *   80021db8: jal   FUN_800c47e8       ; dekomprimieren
 *
 * Die Tabellenzeiger setzt das jeweilige STAGE-Overlay (Ladebasis 0x80100000,
 * KEIN 0x800-Header) — disasm-belegt je Stage, Datei-Offset in BIN/STAGE<n>.BIN:
 *   S1 @0x8011e97c sw v0,0x52c8(at), v0=0x8011EAE4 -> 0x1EAE4
 *   S2 @0x80116fd4                    0x8011713C -> 0x1713C
 *   S3 @0x8011cfa0                    0x8011D0F8 -> 0x1D0F8
 *   S4 @0x80118458                    0x80118590 -> 0x18590
 *   S5 @0x8011ddc8                    0x8011DF18 -> 0x1DF18
 *   S6 @0x80101e0c                    0x80101E3C -> 0x01E3C
 * Gegenprobe im Decompilat: RE_15_Quellcode_Overlays/STAGE1/FUN_8011e064.c
 * enthaelt woertlich `_DAT_800b52c8 = &DAT_8011eae4;`.
 *
 * GEMESSEN ueber alle 6 Stages / 120 BSS-Dateien / 1119 Chunks: 359 Cuts tragen
 * einen Vordergrund, 735 nicht, 0 Fehlschlaege — jeder der 359 Bloecke entpackt
 * zu einem gueltigen Sony-TIM (Magic 10 00 00 00). Punktprobe ROOM1170 Cut 1:
 * L=0x5EE4, present=1, sld_offset=0x4FCC.
 *
 * Ziel im VRAM (nur zur Einordnung, der PC-Pfad blittet stattdessen):
 * Bild -> (320,256) = TPage 0x95 (`ori a3,zero,0x95` @0x80039630),
 * CLUT -> (0,480)   = Clut-Id 0x7800 (`ori v0,zero,0x7800` @0x80039498).
 */
#ifndef RE15_SLD_H
#define RE15_SLD_H

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Groesster entpackter Atlas ueber alle 6 Stages (Vollzaehlung der 359 Bloecke:
 * 0x10220 x320, 0xF220 x38, 0xE420 x1). 0x10220 = 66080 B = 256x256 8bpp + CLUT. */
#define RE15_SLD_MAX_UNPACKED   0x10220

/* Zeilen-Stride der Stage-Cut-Tabelle: `sll v0,v0,5` @0x80021d50 = raum*0x20. */
#define RE15_SLD_TBL_ROW_STRIDE 0x20
/* Spalten je Zeile: `sll v1,v1,1` @0x80021d4c, 0x20/2 = 16 Cuts. */
#define RE15_SLD_TBL_COLS       16

/* Rueckgabecodes. 0 und 1 sind KEIN Fehler. */
enum {
    RE15_SLD_OK            =  0, /* Atlas entpackt, *out_len gesetzt              */
    RE15_SLD_NO_FOREGROUND =  1, /* chunk[L-4] == 0 -> Original zeichnet nichts   */
    RE15_SLD_E_ARG         = -1, /* NULL-Zeiger / negative Groesse                */
    RE15_SLD_E_USEDLEN     = -2, /* L < 8 oder L > chunk_size                     */
    RE15_SLD_E_OFFSET      = -3, /* sld_offset+4 liegt nicht mehr im Chunk        */
    RE15_SLD_E_SIZE        = -4, /* entpackte Groesse <= 0 oder > out_cap         */
    RE15_SLD_E_SRC_EOF     = -5, /* Stream laeuft ueber das Chunk-Ende hinaus     */
    RE15_SLD_E_BACKREF     = -6, /* Rueckverweis vor den Pufferanfang             */
    RE15_SLD_E_DST_OVF     = -7, /* Schreibueberlauf ueber die entpackte Groesse  */
    RE15_SLD_E_NOTABLE     = -8  /* Stage-Tabellenzeile nicht lesbar              */
};

/* Datei-Offset der Cut-Tabelle in BIN/STAGE<stage>.BIN (stage 1..6, sonst 0). */
uint32_t re15_sld_table_file_offset(int stage);

/* L = u16 tabelle[room_index*0x20 + cut*2] aus dem geladenen STAGE<n>.BIN lesen.
 * room_index = (room_id >> 4) & 0xFF (die Raumnummer OHNE die Varianten-Ziffer —
 * ROOM1210 und ROOM1211 teilen sich Zeile 0x21 und die Datei ROOM121.BSS).
 * Rueckgabe RE15_SLD_OK oder RE15_SLD_E_*. */
int re15_sld_used_len(const uint8_t *stage_bin, int stage_bin_size, int stage,
                      int room_index, int cut, uint16_t *out_len);

/* 1:1-Port von FUN_800c47e8 (BIN/DEBUG.BIN +0x47E8, geladen nach 0x800C0000 —
 * deshalb steht der Code NICHT in ghidra1_V2.txt, dessen Text bei 0x800BF000 endet).
 * src/src_size = der GANZE Chunk (fuer die Bereichswachen), src_pos = erstes
 * Stream-Byte (= sld_offset+4). Schreibt genau dst_size Bytes.
 * Rueckgabe >= 0: Index des ersten NICHT gelesenen Quellbytes; < 0: Fehlercode. */
int re15_sld_decode(const uint8_t *src, int src_size, int src_pos,
                    uint8_t *dst, int dst_size);

/* Trailer auswerten und entpacken. `used_len` = L aus re15_sld_used_len().
 * RE15_SLD_OK           -> *out_len = entpackte Groesse, `out` = fertiger Sony-TIM.
 * RE15_SLD_NO_FOREGROUND-> *out_len = 0, `out` unveraendert (kein Fehler!). */
int re15_sld_atlas_from_chunk(const uint8_t *chunk, int chunk_size,
                              uint16_t used_len,
                              uint8_t *out, int out_cap, int *out_len);

#ifdef __cplusplus
}
#endif

#endif /* RE15_SLD_H */
