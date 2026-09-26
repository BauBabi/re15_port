#ifndef RE15_ELEV_SE_H
#define RE15_ELEV_SE_H

/* ============================================================================
 * FAHRSTUHL-FAHRTON — ⛔ RE2-ERGAENZUNG, KEIN RE1.5-ORIGINAL
 * ============================================================================
 * Begruendung und Belege stehen vollstaendig im Kopf von engine/src/scd_elev_se.c.
 * Kurz: RE1.5 faehrt in ROOM1080/1081/4020/4021 exakt dasselbe Fahrskript wie RE2
 * in ROOM21B0/ROOMB1B0 (32 bitgleiche Bytes), aber ohne die zwei Se_on(bank2,
 * id 0x11 / 0x12), die RE2 unmittelbar vor die beiden bit28-Pulse setzt
 * (ROOM21B0.RDT @0x2756 / @0x2784, selbst gelesen).
 * ==========================================================================*/

/* Anzahl der Anker im gerade registrierten Raum. In 236 von 240 RE1.5-Raeumen 0
 * — der Haken im SCD-Dispatcher prueft NUR diese Zahl und tut sonst nichts. */
extern int g_re15_elev_anchor_n;

/* Beim Raumwechsel: den ROHEN RDT-Puffer nach der 32-Byte-Fahrt-Signatur
 * absuchen und die Anker (Zeiger IN diesen Puffer) merken. raw == NULL loescht. */
void re15_elev_se_room_scan(const unsigned char *raw, int raw_size);

/* Im SCD-Dispatcher vor jedem Opcode: steht der Programmzeiger auf einem Anker,
 * wird der zugehoerige RE2-SE ausgeloest. Sonst passiert nichts. */
void re15_elev_se_pc(const unsigned char *pc);

/* Mess-Schiene (Riegel): die Folge der ausgeloesten SE-Ids seit dem letzten
 * Ruecksetzen. Gibt die Anzahl zurueck, *out zeigt auf den internen Puffer. */
int  re15_elev_se_fired(const unsigned char **out);
void re15_elev_se_reset_log(void);

/* Die GEMESSENEN Fundstellen der Signatur (Raum-Id + Datei-Offset), wie sie
 * gen/re15_elev_se.inc vom Generator uebernommen hat — nur fuer den Riegel. */
int  re15_elev_se_hit_count(void);
int  re15_elev_se_hit(int i, unsigned *out_room, unsigned *out_off);

/* Satz-TOC der Mini-Bank shared_assets/RE2/ELEVSE.VBS — dasselbe Muster wie
 * re2_enemse_toc_entry (re2_ems.h): der Satz steht in der Datei, seine Groessen im
 * Code (gen/re2_elev_bank.inc, erzeugt von tools/re2_elevator_cut.py). */
typedef struct {
    unsigned edt_off, edt_size;   /* SE-Map @+0, VH @u32[edt_size-8] */
    unsigned vbd_off, vbd_size;   /* VB-Rumpf                        */
    unsigned vag1_size, vag2_size;/* 16400 / 6336 (Erwartung)        */
    int      se_ride, se_arrive;  /* 0x11 / 0x12                     */
} re15_elev_bank_rec_t;
void re15_elev_bank_rec(re15_elev_bank_rec_t *out);

#endif /* RE15_ELEV_SE_H */
