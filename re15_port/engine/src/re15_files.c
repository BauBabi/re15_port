/* re15_files.c — FILE-Liste und Dokument-Tabelle. Beleg-Block: include/re15_files.h.
 *
 * Reine Engine (plattformunabhaengig). Die Liste ist RE2s 24-Byte-Feld @0x800D4B68,
 * die Tabelle ist das Gegenstueck zu RE2s drei EXE-Tabellen (erster Slot @0x800A9AD0,
 * max_page/y_off @0x800AA144, Namen in der Item-Namensbank ab Id 0x68).
 */
#include "re15_files.h"

#include <stddef.h>
#include <string.h>

#include "gen/re2_files_toc.inc"   /* RE2s Dokument-Records @0x800AA144 (nur Ansehhilfe) */

/* ---- Dokument-Tabelle ------------------------------------------------------------
 * Eintrag 0 = "Irons Diary" (Nutzer-Auftrag Runde 30, Abschnitt E; Text seit Nachtrag J
 * ENGLISCH, analysis/befunde_runde30/irons_diary_en.txt).
 *
 *   Item-Id 0x48   erste Dokument-Id von RE1.5 (u8 @0x800c7370), Dokument-Nr 0.
 *   Bild-Satz 25   Port-Wahl, keine Original-Adresse: der erste freie Satz hinter RE2s 25
 *                  Dokumenten (RE2_FILES_DOC_COUNT 25, gen/re2_files_toc.inc, Tabelle
 *                  @0x800A9AD0 fuehrt die Saetze 0..24). Die Dateien FILE25_*.TIM erzeugt
 *                  re15_port/tools/re2_doc_satz.py aus dem Nutzertext.
 *   max_page 15    Port-Wahl, keine Original-Adresse - gemessen am Satz: letzte Seite des
 *                  gesetzten ENGLISCHEN Nutzertexts (Nachtrag J; 9 Zeilen je Seite bei
 *                  H 144, re2_doc_satz.py -> FILE25_satz.txt "max_page ... = 15";
 *                  FILE25_p15_page.TIM ist vorhanden, FILE25_p16 nicht; Riegel
 *                  probe_r30_irons_diary_dokument Teil C). Die deutsche Fassung (Abschnitt E)
 *                  hatte 17.
 *                  Gegenstueck zu RE2s u16 @0x800AA144 + doc*4, gelesen
 *                  `lhu a0,-24252(at)` @0x800727c8. max_page steht AUSDRUECKLICH hier
 *                  und wird nicht aus der Zahl der Dateien gezaehlt: bei RE2s Dokumenten
 *                  9, 23 und 24 liegen mehr Dateien vor, als max_page erlaubt.
 *   H 144          aus dem TIM-Kopf von FILE25_title_page.TIM; gleich der Vorlage FILE08
 *                  (RE2 @0x800AA144 + 8*4 = `04 00 70 00`: y_off 0x70 = 112, H = 256-112).
 *   Name           "Irons Diary" in RE1.5s Glyphenkodierung: Code = ASCII - 0x24,
 *                  Leerzeichen 0x00, Ende 0x07. Belegt an "Chris' Diary" @0x800c4e04
 *                  (DEBUG.BIN Datei 0x04e04) = `1f 44 4e 45 4f 3a 00 20 45 3d 4e 55 07`:
 *                  die Glyphen " Diary" (00 20 45 3d 4e 55) sind von dort uebernommen,
 *                  I/r/o/n/s folgen derselben Formel (r = 0x4e und s = 0x4f stehen im
 *                  selben Namen).
 *                  Schreibung: Listenname gemischt "Irons Diary" (RE2s Listennamen sind
 *                  gemischt, "Chief's diary" @0x8009EAB1), Titelseite in Versalien wie
 *                  alle 25 RE2-Titelseiten — festgelegt im Auftrag der Runde 30. */
static const uint8_t s_name_irons_diary[] = {
    0x25, 0x4e, 0x4b, 0x4a, 0x4f, 0x00, 0x20, 0x45, 0x3d, 0x4e, 0x55, 0x07
};

/* RUNDE 34 NACHT, SPUR E — Dokumente 1..4 (Welt, Aufhebe-Zone: include/re15_dokumente.h;
 * Dossier analysis/befunde_runde34_nacht/E_dokumente.md 5.2 / 9.0). Je Eintrag:
 *   Item-Id        0x48 + Nr (VERTRAG 1.4; RE2 `addiu a0,a3,-104` @0x80071d04).
 *   Bild-Satz      26..29: PORT-WAHL, die naechsten freien Saetze hinter FILE25 (VERTRAG 1.4).
 *                  Seiten gesetzt von tools/r34n_e/doc_satz_brief.py aus dem Nutzertext
 *                  (AUFTRAG.md, woertlich), reproduzierbar mit tools/r34n_e/satz_bauen.sh.
 *   max_page       PORT-WAHL, gemessen am Satz: FILEnn_p<max> vorhanden, p<max+1> nicht
 *                  (selbstpruefung.py) — 3 / 4 / 2 / 2.
 *   H              TIM-Kopf der gesetzten Titelseite (Bildhoehe) = Vorlage: 144 (FILE00/FILE08)
 *                  bzw. 176 (FILE02/FILE06).
 *   Name           NUTZER-VORGABE (AUFTRAG.md Z. 22/29/41/55, gemischte Schreibung wie
 *                  RE2s Listennamen, Runde-30-Regel), kodiert wie "Chris' Diary" @0x800c4e04
 *                  (DEBUG.BIN 0x04e04 `1f 44 4e 45 4f 3a 00 20 45 3d 4e 55 07`): Code =
 *                  ASCII - 0x24, Leerzeichen 0x00, Apostroph 0x3A, Ende 0x07. */
static const uint8_t s_name_dok1[] = {     /* "Police Officer's Final Diary Entry" */
    0x2c, 0x4b, 0x48, 0x45, 0x3f, 0x41, 0x00, 0x2b, 0x42, 0x42, 0x45, 0x3f, 0x41, 0x4e,
    0x3a, 0x4f, 0x00, 0x22, 0x45, 0x4a, 0x3d, 0x48, 0x00, 0x20, 0x45, 0x3d, 0x4e, 0x55,
    0x00, 0x21, 0x4a, 0x50, 0x4e, 0x55, 0x07
};
static const uint8_t s_name_dok2[] = {     /* "Elliot's Diary" */
    0x21, 0x48, 0x48, 0x45, 0x4b, 0x50, 0x3a, 0x4f, 0x00, 0x20, 0x45, 0x3d, 0x4e, 0x55, 0x07
};
static const uint8_t s_name_dok3[] = {     /* "Marvin's Notes" */
    0x29, 0x3d, 0x4e, 0x52, 0x45, 0x4a, 0x3a, 0x4f, 0x00, 0x2a, 0x4b, 0x50, 0x41, 0x4f, 0x07
};
static const uint8_t s_name_dok4[] = {     /* "Armory Notice" */
    0x1d, 0x4e, 0x49, 0x4b, 0x4e, 0x55, 0x00, 0x2a, 0x4b, 0x50, 0x45, 0x3f, 0x41, 0x07
};

static const re15_file_doc_t s_docs[] = {
    { RE15_FILES_FIRST_ITEM_ID + 0, 25, 15, 144, s_name_irons_diary },
    { RE15_FILES_FIRST_ITEM_ID + 1, 26,  3, 144, s_name_dok1 },   /* Runde 34 Nacht, Spur E */
    { RE15_FILES_FIRST_ITEM_ID + 2, 27,  4, 144, s_name_dok2 },   /* Runde 34 Nacht, Spur E */
    { RE15_FILES_FIRST_ITEM_ID + 3, 28,  2, 176, s_name_dok3 },   /* Runde 34 Nacht, Spur E */
    { RE15_FILES_FIRST_ITEM_ID + 4, 29,  2, 176, s_name_dok4 },   /* Runde 34 Nacht, Spur E */
};
#define DOC_COUNT ((int)(sizeof s_docs / sizeof s_docs[0]))

/* ---- die Liste: RE2 @0x800D4B68, 24 Byte ------------------------------------------ */
static uint8_t s_list[RE15_FILES_SLOTS] = {
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
};

int re15_files_doc_count(void) { return DOC_COUNT; }

const re15_file_doc_t *re15_files_doc(int doc)
{
    if (doc < 0 || doc >= DOC_COUNT) return NULL;
    return &s_docs[doc];
}

int re15_files_doc_from_item(int item_id)
{
    /* `sltiu v0,a3,0x68` @0x80071bbc / `addiu a0,a3,-104` @0x80071d04, mit RE1.5s
     * erster Dokument-Id 0x48 (u8 @0x800c7370) an der Stelle von RE2s 0x68. */
    int doc = item_id - RE15_FILES_FIRST_ITEM_ID;
    if (item_id < RE15_FILES_FIRST_ITEM_ID) return -1;
    if (doc >= DOC_COUNT) return -1;          /* Port-Schranke: die Tabelle ist ein C-Feld */
    return doc;
}

/* ANSEHHILFE (Umgebungsvariable RE15_DOC, nur Debug): letzte Seite eines Bild-Satzes.
 * Saetze der Dokument-Tabelle -> deren max_page; RE2s Saetze 0..24 -> max_page aus
 * RE2s Dokument-Record (u16 @0x800AA144 + doc*4, gen/re2_files_toc.inc). Sonst 0. */
int re15_files_bildsatz_max_page(int bildsatz)
{
    int i;
    (void)re2_files_first_slot; (void)re2_files_slot;   /* Tabellen des Generators, hier ungenutzt */
    for (i = 0; i < DOC_COUNT; i++)
        if (s_docs[i].bildsatz == bildsatz) return s_docs[i].max_page;
    if (bildsatz >= 0 && bildsatz < RE2_FILES_DOC_COUNT)
        return (int)re2_files_doc[bildsatz].max_page;
    return 0;
}

void re15_files_reset(void)
{
    /* @0x800682dc-f8: 24 x `sb v0(=255),19304(at)` */
    memset(s_list, RE15_FILES_EMPTY, sizeof s_list);
}

int re15_files_add(int doc)
{
    /* FUN_800692dc @0x800692dc-0x80069314 */
    int platz;
    for (platz = 0; platz < RE15_FILES_SLOTS; platz++) {      /* sltiu 0x18 @0x80069308 */
        if (s_list[platz] == RE15_FILES_EMPTY) {              /* bne v0,a2 @0x800692f4  */
            s_list[platz] = (uint8_t)doc;                     /* sb a0,0(v1) @0x80069300 */
            return platz;                                     /* addu v0,a1,zero @0x800692f8 */
        }
    }
    return 0;                 /* v0 = Ergebnis des letzten sltiu @0x80069308 = 0 */
}

int re15_files_get(int slot)
{
    if (slot < 0 || slot >= RE15_FILES_SLOTS) return RE15_FILES_EMPTY;
    return s_list[slot];
}

int re15_files_count(void)
{
    int n = 0, i;
    for (i = 0; i < RE15_FILES_SLOTS; i++)
        if (s_list[i] != RE15_FILES_EMPTY) n++;
    return n;
}

void re15_files_export(uint8_t out[RE15_FILES_SLOTS])
{
    if (out) memcpy(out, s_list, sizeof s_list);
}

void re15_files_import(const uint8_t in[RE15_FILES_SLOTS])
{
    if (in) memcpy(s_list, in, sizeof s_list);
}
