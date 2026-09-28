/* re15_files.h — die FILE-LISTE (aufgehobene Dokumente) und die Dokument-Tabelle.
 *
 * ⛔ WARUM RE2 UND NICHT RE1.5
 * RE1.5 hat KEIN Dokument-System, nur dessen Schirm: die FILE-Liste ist dort eine
 * STATISCHE Tabelle (Maske u16[3] @0x800c6c98 = `01 00 ff ff ff ff`, DEBUG.BIN), mit genau
 * EINEM Leser (`lhu s3,0(t1)` @0x800c72f0) und KEINEM Schreiber (Adress-Suche ueber
 * PSX.EXE, DEBUG.BIN, STAGE1-6.BIN, TITLE.BIN: 1 Treffer). Der Leser zeigt fuer JEDE
 * Zeile denselben Blob "Operation Report" (`addiu t1,t1,-13004` @0x800c7614 =
 * 0x800ccd34). Einen Dokument-Zweig im Aufnahme-Pfad gibt es nicht: das einzige
 * `sltiu ...,0x48` der EXE ist die Klemme des Status-Schirms @0x8004a350. Das System ist
 * in RE1.5 also unfertig; massgeblich ist RE2 Retail (info/re2leon/PSX.EXE).
 *
 * RE2 (selbst disassembliert, Datei-Offset = RAM - 0x8000F800):
 *   Liste fuellen beim Spielstart
 *     800682dc  addiu a1,zero,24        ; 24 Plaetze
 *     800682e0  addiu v0,zero,255       ; leer = 0xFF
 *     800682f0  sb    v0,19304(at)      ; 0x800d4b68 + a1
 *   Anhaengen FUN_800692dc(a0 = Dokument-Nr), Rueckgabe v0 = Platz
 *     800692e0  addiu a2,zero,255
 *     800692e8  addiu v1,v1,19304       ; 0x800d4b68
 *     800692ec  lbu   v0,0(v1)
 *     800692f4  bne   v0,a2,0x80069304  ; belegt -> naechster Platz
 *     800692f8  addu  v0,a1,zero        ; Rueckgabe = Platz
 *     80069300  sb    a0,0(v1)          ; erster freier Platz bekommt die Dokument-Nr
 *     80069308  sltiu v0,a1,0x18        ; 24 Plaetze
 *   Dokument-Nr aus der Item-Id
 *     80071bbc  sltiu v0,a3,0x68        ; Id >= 0x68 -> Dokument
 *     80071d04  addiu a0,a3,-104        ; Dokument-Nr = Id - 0x68
 *
 * DIE ERSTE DOKUMENT-ID IST IN RE1.5 0x48, NICHT 0x68 — das ist RE1.5s eigene Zahl:
 * die Item-Ids enden bei 0x47 (Klemme `sltiu v0,v0,0x48` @0x8004a350 / `ori v0,zero,0x47`
 * @0x8004a358; DATA/ITEMALL.PIX 86400 B = 72 Kacheln; ITEM/ITPS.ITP 884736 B = 72
 * Bilder), und die erste FILE-Zeile traegt die Basis-Id 0x48 (u8 @0x800c7370, DEBUG.BIN
 * Datei 0x07370 = `48 52 5c`).
 */
#ifndef RE15_FILES_H
#define RE15_FILES_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define RE15_FILES_SLOTS          24     /* `sltiu v0,a1,0x18` @0x80069308 (RE2)           */
#define RE15_FILES_EMPTY          0xFF   /* `addiu a2,zero,255` @0x800692e0 (RE2)          */
#define RE15_FILES_FIRST_ITEM_ID  0x48   /* RE1.5: u8 @0x800c7370; Klemme @0x8004a350-5c   */

/* Ein Eintrag der Dokument-Tabelle. */
typedef struct {
    uint8_t        item_id;    /* Item-Id der Aufhebe-Zone (RE15_FILES_FIRST_ITEM_ID + Nr) */
    uint8_t        bildsatz;   /* FILE%02d_*.TIM unter shared_assets/RE2/FILES             */
    uint8_t        max_page;   /* letzte Seite; der Leser blaettert 0..max_page            */
    uint8_t        page_h;     /* Seitenhoehe H (256 - y_off)                              */
    const uint8_t *name;       /* Listenname in der RE1.5-Glyphenkodierung, Ende 0x07      */
} re15_file_doc_t;

/* Wie viele Dokumente die Tabelle fuehrt. */
int re15_files_doc_count(void);

/* Tabelleneintrag eines Dokuments; NULL, wenn es das Dokument nicht gibt. */
const re15_file_doc_t *re15_files_doc(int doc);

/* Dokument-Nr zu einer Item-Id (Id - 0x48, RE2 `addiu a0,a3,-104` @0x80071d04);
 * -1, wenn die Id kein Dokument der Tabelle ist. */
int re15_files_doc_from_item(int item_id);

/* ANSEHHILFE (nur Debug): letzte Seite eines Bild-Satzes; 0..24 = RE2s Dokumente
 * (u16 @0x800AA144 + doc*4), sonst die Saetze der Dokument-Tabelle. */
int re15_files_bildsatz_max_page(int bildsatz);

/* Liste leeren: alle 24 Plaetze 0xFF (RE2 Spielstart @0x800682dc-f8). */
void re15_files_reset(void);

/* Dokument an den ersten freien Platz haengen; Rueckgabe = Platz.
 * Byte-treu FUN_800692dc: KEINE Pruefung auf Doppelte, und ist kein Platz frei, wird
 * nichts geschrieben und 0 zurueckgegeben (v0 traegt dann das Ergebnis des letzten
 * `sltiu` @0x80069308). */
int re15_files_add(int doc);

/* Dokument-Nr auf einem Platz; RE15_FILES_EMPTY fuer leer oder ausserhalb 0..23. */
int re15_files_get(int slot);

/* Wie viele Plaetze belegt sind. */
int re15_files_count(void);

/* Speicherstand: die 24 Byte roh. RE2 speichert die Liste mit — sie liegt bei Offset
 * 0x6C4 im Block 0x800D44A4 (0x800D4B68 - 0x800D44A4), geschrieben 0x800 Byte
 * (`addiu v0,zero,2048` @0x801c0c70, MEM_CARD.BIN laedt @0x801BFA18), zurueckgeladen
 * 0x798 Byte (`addiu a2,zero,1944` @0x801c0dfc). */
void re15_files_export(uint8_t out[RE15_FILES_SLOTS]);
void re15_files_import(const uint8_t in[RE15_FILES_SLOTS]);

#ifdef __cplusplus
}
#endif

#endif /* RE15_FILES_H */
