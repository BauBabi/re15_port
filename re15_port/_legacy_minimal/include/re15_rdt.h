/**
 * @file re15_rdt.h
 * @brief RDT (Room Definition Table) Datenstrukturen
 *
 * Definiert die Struktur für geparste RDT-Dateien. Eine RDT-Datei ist ein
 * Container pro Raum mit Header, Adresstabelle (21 Einträge, Offset 0x08-0x5C)
 * und allen raumspezifischen Daten (Kollision, Kamera, Skripte, Modelle, etc.).
 *
 * Parsing-Strategie: In-Place — der Parser liefert Pointer in den geladenen
 * Puffer (kein Kopieren). Ein Offset von 0x00000000 in der Adresstabelle
 * bedeutet: Sektion nicht vorhanden → Pointer = NULL.
 */
#ifndef RE15_RDT_H
#define RE15_RDT_H

#include "re15_types.h"

/* ============================================================================
 * RDT-Adresstabelle (21 Sektionen, Offset 0x08-0x5C)
 *
 * Jeder Eintrag in der originalen RDT-Datei ist ein 32-bit LE Offset relativ
 * zum Dateianfang. Nach dem Parsen werden diese zu absoluten Pointern in den
 * geladenen Puffer konvertiert. Null-Offset → NULL-Pointer.
 * ========================================================================= */

/** Anzahl der Sektionen in der RDT-Adresstabelle */
#define RE15_RDT_SECTION_COUNT 21

/**
 * Geparste RDT-Struktur — enthält Pointer auf alle Sektionen im Puffer.
 *
 * Die Pointer zeigen direkt in den geladenen RDT-Puffer (In-Place-Parsing).
 * NULL bedeutet: Sektion nicht vorhanden (Offset war 0x00000000).
 */
typedef struct re15_rdt_s {
    uint8_t* snd0_edt;     /**< 0x08 — Sound-Bank 0 EDT                    */
    uint8_t* snd0_vh;      /**< 0x0C — Sound-Bank 0 VH (VAG Header)        */
    uint8_t* snd0_vb;      /**< 0x10 — Sound-Bank 0 VB (VAG Body)          */
    uint8_t* snd1_edt;     /**< 0x14 — Sound-Bank 1 EDT                    */
    uint8_t* snd1_vh;      /**< 0x18 — Sound-Bank 1 VH                     */
    uint8_t* snd1_vb;      /**< 0x1C — Sound-Bank 1 VB                     */
    uint8_t* collision;    /**< 0x20 — SCA-Kollisionsdaten                  */
    uint8_t* camera;       /**< 0x24 — RID-Kameradefinitionen               */
    uint8_t* zone;         /**< 0x28 — RVD-Trigger-Zonen                    */
    uint8_t* light;        /**< 0x2C — Beleuchtungsdaten                    */
    uint8_t* md1_ptr;      /**< 0x30 — Modell-Pointer-Tabelle               */
    uint8_t* floor;        /**< 0x34 — Bodensound-Regionen                  */
    uint8_t* block;        /**< 0x38 — (Unused — nicht parsen)              */
    uint8_t* message;      /**< 0x3C — MSG-Texttabelle                      */
    uint8_t* main_scd;     /**< 0x40 — Haupt-SCD-Skript                    */
    uint8_t* sub_scd;      /**< 0x44 — Sub-SCD-Skripte                     */
    uint8_t* extra_scd;    /**< 0x48 — Extra-SCD (unused in RE1.5)          */
    uint8_t* effect;       /**< 0x4C — Effekt-Sprites (ESP)                 */
    uint8_t* esp_tim;      /**< 0x54 — ESP-Texturen                         */
    uint8_t* model_tim;    /**< 0x58 — Modell-Texturen                      */
    uint8_t* animation;    /**< 0x5C — Animationsdaten                      */
} re15_rdt_t;

/* ============================================================================
 * RDT-Parser API
 * ========================================================================= */

/**
 * Parst eine geladene RDT-Datei (In-Place).
 *
 * Liest die 21-Einträge-Adresstabelle ab Offset 0x08 und konvertiert jeden
 * Offset zu einem Pointer in den Puffer. Null-Offsets ergeben NULL-Pointer.
 *
 * @param buffer   Zeiger auf den geladenen RDT-Puffer (muss gültig bleiben)
 * @param size     Größe des Puffers in Bytes
 * @param out_rdt  Ausgabe: Befüllte RDT-Struktur mit Sektions-Pointern
 * @return         0 bei Erfolg, negativer Fehlercode bei ungültigem Input
 */
int re15_rdt_parse(uint8_t* buffer, int size, re15_rdt_t* out_rdt);

#endif /* RE15_RDT_H */
