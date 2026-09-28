/*
 * RE1.5 Rebuilt — Item-Bild und Inventar-Icon der SICHERUNG (Item 0x40 "Fuse"),
 * eingesetzt im GELADENEN Puffer (Runde 30, Thema H "andere Sicherung").
 *
 * Herleitung und Belege: include/re15_sicherung.h, Dossier
 * analysis/befunde_runde30/sicherung.md §3.5/§5.4.
 * Bytes: tools/sicherung_itembild.py -> gen/sicherung_itembild.inc (gerendert aus dem
 * Welt-Modell gen/sicherung_prop.inc). KEIN Asset-Patch: ITEM/ITPS.ITP und
 * DATA/ITEMALL.PIX auf der Platte bleiben byte-true.
 *
 * ⛔ EIGENE DATEI, NICHT sicherung_1150.c: die Mess-Variante der Ermittlung
 * (tests/unit/probe_r30_sicherung_variante.c) ersetzt die vier Symbole aus
 * sicherung_1150.c per Objekt-vor-Archiv. Stuenden die Einsetz-Funktionen dort, zoege
 * der Linker das Archivmitglied doch und die Variante haette Doppelsymbole.
 *
 * WER RUFT: alle SECHS Ladestellen der beiden Dateien — sonst zeigen zwei Leser zwei
 * Gegenstaende:
 *   platform/pc/main.c            ITEMALL.PIX vor re15_itemall_set_pix
 *   platform/pc/main.c            ITPS.ITP    vor re15_itps_set_data
 *   platform/pc/src/inv_render_pc.c   s_itemall, s_itps (Statusschirm)
 *   engine/src/item_icon_common.c fauler Lader re15_itemall_load
 *   engine/src/itps_common.c      fauler Lader re15_itps_load
 */
#include "re15_sicherung.h"

#include <stddef.h>
#include <string.h>

#include "gen/sicherung_itembild.inc"

/* Blockgroesse des Item-Bilds. Lader LAB_8001e404 (PSX.EXE):
 *     8001e414: ori  v0,zero,0x3000     ; Leselaenge
 *     8001e450: sll  v0,a0,1
 *     8001e454: addu v0,v0,a0
 *     8001e458: sll  v0,v0,1            ; id * 6 Sektoren = id * 0x3000 Bytes
 * Block 0x40 liegt also bei 0x40 * 0x3000 = 0xC0000. */
#define SICHERUNG_ITPS_BLOCK   0x3000

/* Groesse eines Icon-Tiles: 40 x 30 Bildpunkte, 1 Byte je Punkt, Tile = Item-Id
 * (item_icon_common.c ITEMALL_TILE_BYTES; ITEMALL.PIX 86400 B = 72 Tiles a 1200 B).
 * Tile 0x40 liegt bei 0x40 * 1200 = 0x12C00. */
#define SICHERUNG_ICON_TILE    1200

/* Uebersetzungszeit-Riegel: die eingebackenen Felder muessen GENAU ein Block / ein Tile
 * sein, sonst schriebe das Einsetzen in den Nachbarn (Item 0x41 "Spark Plug"). */
typedef char sicherung_block_groesse_pruefen[
    (sizeof(re15_sicherung_itps_block) == SICHERUNG_ITPS_BLOCK) ? 1 : -1];
typedef char sicherung_tile_groesse_pruefen[
    (sizeof(re15_sicherung_icon_tile) == SICHERUNG_ICON_TILE) ? 1 : -1];

const uint8_t *re15_sicherung_itps_block_bytes(int *out_size)
{
    if (out_size) *out_size = (int)sizeof(re15_sicherung_itps_block);
    return re15_sicherung_itps_block;
}

const uint8_t *re15_sicherung_icon_tile_bytes(int *out_size)
{
    if (out_size) *out_size = (int)sizeof(re15_sicherung_icon_tile);
    return re15_sicherung_icon_tile;
}

void re15_sicherung_bild_einsetzen(uint8_t *itps, int size)
{
    size_t ab = (size_t)RE15_SICHERUNG_ITEM * SICHERUNG_ITPS_BLOCK;     /* 0xC0000 */
    if (!itps || size < 0) return;
    if ((size_t)size < ab + SICHERUNG_ITPS_BLOCK) return;   /* zu kurz: nichts anfassen */
    memcpy(itps + ab, re15_sicherung_itps_block, SICHERUNG_ITPS_BLOCK);
}

void re15_sicherung_icon_einsetzen(uint8_t *itemall, int size)
{
    size_t ab = (size_t)RE15_SICHERUNG_ITEM * SICHERUNG_ICON_TILE;      /* 0x12C00 */
    if (!itemall || size < 0) return;
    if ((size_t)size < ab + SICHERUNG_ICON_TILE) return;    /* zu kurz: nichts anfassen */
    memcpy(itemall + ab, re15_sicherung_icon_tile, SICHERUNG_ICON_TILE);
}
