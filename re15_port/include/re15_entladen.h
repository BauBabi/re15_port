/*
 * re15_entladen.h — Runde 35 Spur I: ALLE Raum-Assets entladen (Raumwechsel, Spielstart, Tod).
 *
 * NUTZER (2026-10-03, AUFTRAG.md Z. 24): "Nachdem ich gestorben bin und new game mache habe ich
 * teilweise noch PRIs von meinen Spielstand davor ... Wenn man tot ist, aber auch wenn man den
 * Raum wechselt sollen saemtliche Assets von den Raeumen davor entladen sein."
 *
 * ORIGINAL (Belege: analysis/befunde_runde35/I_entladen.md, R1-R4):
 *   - Der Raumlader FUN_800396fc setzt die Raum-Arena auf ihre Basis zurueck
 *     (`lw a0,-0x3880` @0x80039704 = *0x800ac780, `sw a0` -> 0x800ac77c @0x80039738,
 *     -> 0x800ac778 @0x80039740, -> 0x800bbeb0 @0x80039748) und laedt die neue RDT AB der Basis
 *     (`jal 0x80013b60` @0x800397e8). Die Masken-Tabelle wird in derselben Arena neu angelegt
 *     (`jal 0x80039270` @0x800399cc: DAT_800b2584 = Arena-Kopf).
 *   - Er hat genau ZWEI Aufrufer (jal-Wort 0x0C00E5BF, eigener Voll-Scan): @0x8001d988 (Tuer,
 *     FUN_8001d600) und @0x8001d5ac (Spielmodul-Init FUN_8001d22c, die zusaetzlich selbst
 *     @0x8001d590-a0 die Arena zuruecksetzt). Spielstart (NEW GAME / LOAD) = Raumwechsel.
 *   - Die Masken werden NUR im Spielmodul gezeichnet: einziger Aufrufer des Zeichners
 *     FUN_80039590 ist `jal` @0x8001ce54 in der Spielmodul-Schleife FUN_8001c958; nach deren
 *     Ende (@0x8001d1f8 / @0x8001d200) zeichnet niemand mehr eine Raum-Maske.
 *
 * PORT: Der PC haelt dieselben Daten in Caches mit Prozess-Lebensdauer. Diese Schnittstelle ist
 * das Gegenstueck zum Arena-Reset: EIN Aufruf, an den drei Grenzen des Originals:
 *   "raum"       re15_room_load (room_pc.c) nach erfolgreichem Lesen, VOR der Installation der
 *                neuen RDT  (== @0x80039738 vor @0x800397e8)
 *   "spielstart" Boot-Block in main.c, vor dem Boot-RDT (== @0x8001d590-a0 vor @0x8001d5ac)
 *   "tod"        Uebergabe YOU-DIED -> Titel in main.c (== Modul-Ende @0x8001d200)
 *
 * GENERATION: jedes Ereignis zaehlt g_re15_entladen_gen hoch. Jeder Cache vermerkt beim FUELLEN
 * die laufende Generation; ein belegter Eintrag mit aelterer Generation ist FREMD (stammt aus
 * einem Raum/Spielstand davor). Die Zaehlung (re15_entladen_zensus) ist die Messschiene des Pins.
 */
#ifndef RE15_ENTLADEN_H
#define RE15_ENTLADEN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Laufende Generation (startet bei 1; 0 = "nie gefuellt"). */
extern unsigned g_re15_entladen_gen;

/* Gegner-Baenke: Generation beim Anlegen (re15_enemy_alloc). Index = Bank 0..RE15_ENEMY_MAX-1. */
void     re15_entladen_gegner_merken(int bank);
unsigned re15_entladen_gegner_gen(int bank);

/* Zaehlt die Generation hoch (ohne zu entladen) — nur fuer Plattformen ohne eigenen Teardown. */
void     re15_entladen_gen_weiter(void);

/* ---- PC-Seite (platform/pc/src/entladen_pc.c) ------------------------------------------- */

/* Die Faecher, die der Zensus zaehlt. */
enum {
    RE15_FACH_PRI_MASKEN = 0,  /* render_pc: aktive Maskenliste (sprite.pri)              */
    RE15_FACH_PRI_ATLAS,       /* render_pc: Vordergrund-Atlas-Textur                       */
    RE15_FACH_SLD,             /* bg_pc: SLD-Atlas-Auszug aus dem BSS-Chunk                 */
    RE15_FACH_MSK,             /* nachgezeichnete Masken (MASKS/ROOMxxxx.MSK)               */
    RE15_FACH_TIM,             /* Raum-TIM-Slots (Props, Gegner, Raum-ESP, Gore, Tuer)       */
    RE15_FACH_GEGNER,          /* g_enemy-Baenke                                            */
    RE15_FACH_ESP_BANK,        /* Raum-Effektbank (RDT-ESP)                                 */
    RE15_FACH_ESP_FX,          /* laufende Effekt-Instanzen (re15_esp_fx)                   */
    RE15_FACH_ESP_POOL,        /* ESP-Row-Pool                                              */
    RE15_FACH_RE2FX,           /* RE2-FX-Plaetze                                            */
    RE15_FACH_RDT,             /* residente Raum-RDT-Bytes (room_pc)                         */
    RE15_FACH_ANZAHL
};

typedef struct {
    int belegt[RE15_FACH_ANZAHL];   /* belegte Eintraege je Fach                               */
    int fremd[RE15_FACH_ANZAHL];    /* davon mit aelterer Generation (aus einem Raum davor)   */
    int belegt_summe, fremd_summe;
} re15_entladen_zensus_t;

extern const char *const re15_entladen_fachname[RE15_FACH_ANZAHL];

/* Grenze ueberschritten: Generation hoch, alle Raum-Assets entladen, Zensus protokollieren. */
void re15_entladen_ereignis(const char *anlass);

/* Zaehlt die Belegung aller Faecher. */
void re15_entladen_zensus(re15_entladen_zensus_t *z);

/* Messschiene (RE15_ENTLADEN_LOG=<datei>): einmal je Bild aufgerufen (render_pc end_frame) mit
 * der Zahl der in diesem Bild GEZEICHNETEN Masken und deren Generation. Schreibt eine Zeile,
 * sobald fremd belegt oder fremd gezeichnet wird, plus eine Zusammenfassung je Ereignis. */
void re15_entladen_bild(int masken_gezeichnet, unsigned masken_gen);

/* Nachgezeichnete Masken (R15M-Container) des Raums — ersetzt den alten main.c-Cache, damit das
 * Entladen ihn erreicht. Liefert NULL, wenn der Raum keine Datei hat. */
const unsigned char *re15_entladen_msk(unsigned room_id, int *out_size);

#ifdef __cplusplus
}
#endif

#endif /* RE15_ENTLADEN_H */
