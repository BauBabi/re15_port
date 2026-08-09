/*
 * RE1.5 Rebuilt — Zustand des ORIGINAL-Debug-Menues ("UTILITY MENU", PSX.EXE @0x80014444).
 *
 * Die Felder tragen absichtlich dieselbe Bedeutung wie die drei Original-Bytes, damit ein
 * RAM-Vergleich Port gegen Original moeglich bleibt:
 *     row       <- 0x800BBE5D          Auswahlzeile, 0..2 (@0x80014604/18, Grenze @0x80014644)
 *     stage     <- 0x800BBE5E          Stage-Index (@0x800146E4)
 *     room_idx  <- 0x800BBE5F + stage  Raumindex je Stage, Wrap bei 0x31 (@0x80014734)
 *
 * Zweck im Port: Original und Port muessen einen Raum ueber DENSELBEN Weg betreten, sonst vergleicht
 * der Paritaets-Harness zwei verschiedene Situationen (genau daher kamen die Positions- und
 * Yaw-Differenzen in ROOM1140, die ich faelschlich dem Port zugeschrieben hatte).
 */
#ifndef RE15_DEBUG_MENU_H
#define RE15_DEBUG_MENU_H

#include <stdint.h>

#define RE15_DBG_STAGES 8

/* Lade-Flanke im remappten Pad-Wort (Quadrat, @0x80014A38) — exportiert, damit ein
 * automatisierter Messlauf DENSELBEN Tick-Pfad ausloest wie ein Tastendruck. */
#define RE15_DBG_EDGE_LOAD 0x0080

typedef struct {
    uint8_t open;                          /* Menue sichtbar (Original: eigener Pause-Zustand)   */
    uint8_t row;                           /* 0x800BBE5D — 0..2                                  */
    uint8_t stage;                         /* 0x800BBE5E                                         */
    uint8_t room_idx[RE15_DBG_STAGES];     /* 0x800BBE5F + stage, Wrap bei 0x31                  */
    uint8_t want_load;                     /* 1 = Raum laden (Square in der JUMP-Zeile)          */
    uint16_t load_room;                    /* der aufzuloesende Raum                             */
} re15_debug_menu_t;

void re15_debug_menu_reset(void);
void re15_debug_menu_toggle(void);
int  re15_debug_menu_open(void);
/* JUMP-Tabelle zur LAUFZEIT aus DEBUG.BIN (RAW-Abbild, laedt im Original nach 0x800c0000):
 * Satz-Halbwort @0x263a+2*(637*stage+13*idx) (0 = leer @0x800147B0, 2 in Slot 0 = Stage
 * uebersprungen: "ori a0,zero,0x2" @0x800148C8, "lhu" @0x80014924, "beq v0,a0" @0x8001492C),
 * Name @+8 (Anzeige-Zeiger -> &0x800c2642 + 0x4fa*stage + 0x1a*idx). Ohne Aufruf (oder wenn die Datei
 * fehlt/zu klein ist) faellt das Modul auf die generierte Tabelle debug_jump_table.h zurueck. */
void re15_debug_menu_set_bin(const uint8_t *debug_bin, unsigned size);
/* Beim Oeffnen: Cursor auf den aktuellen Raum stellen (Original @0x800144B0/@0x800144E0 kopiert
 * DAT_800b0fe0/DAT_800b0fe2 in die Menue-Bytes). room_id = volle Port-ID, z.B. 0x1140. */
void re15_debug_menu_sync_cursor(unsigned room_id);
/* held = Halbwort 0x800AC760, edge = Halbwort 0x800AC762 (Schreiber @0x80030564/@0x800305A0).
 * D-Pad liegt auf den Bits 12-15 (die Maske `andi 0xf000` @0x80030514 gruppiert genau diese
 * vier), die Face-Tasten auf 4-7. Rueckgabe 1 = Raum laden, Ziel in ->load_room. */
int  re15_debug_menu_tick(uint16_t held, uint16_t edge);
/* Stellt den Menue-Cursor fuer einen automatisierten Messlauf auf room_id (volle ID, 0x1140) und
 * oeffnet das Menue in der JUMP-Zeile. Loest NICHTS aus — den Sprung macht danach der normale
 * re15_debug_menu_tick() mit der Lade-Flanke, also derselbe Pfad wie ein Quadrat-Druck.
 * Rueckgabe 1 = Raum in der Tabelle gefunden. */
int  re15_debug_menu_point_at(unsigned room_id);
const char *re15_debug_menu_room_name(void);
uint16_t re15_debug_menu_pad(uint16_t phys);
const re15_debug_menu_t *re15_debug_menu_state(void);

/* Halbwort-Offset in die JUMP-Tabelle (Original @0x800C263A): 637 je Stage, 13 je Eintrag,
 * erstes Halbwort = Raum-ID (@0x8001476C-0x800147A8). */
unsigned re15_debug_menu_table_offset(unsigned stage, unsigned idx);

#endif /* RE15_DEBUG_MENU_H */
