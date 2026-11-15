/*
 * RE1.5 Rebuilt — Runde 41 Spur "Monitor": die Hangar-Szene (Birkin wirft den Spider-Man, birkin_formen.c Kopf "Spur H")
 * laeuft als UEBERWACHUNGSBILD im Monitor Room ROOM5030 (Leon) / ROOM5031 (Elza), nicht mehr als Zwischensequenz im
 * Hangar. Nutzer 2026-11-15: "diese Szene wo der Birkin den Spiderman schmeisst, ist 100%ig kein Boss Kampf ... Ich wuerde
 * eher sagen, das Leon oder Elza Birkin auf dem Monitor sehen mit der Szene, wo auch immer er danach hin laeuft."
 *
 * ⛔ PORT-SZENE, KEIN AUSLIEFERUNGSSTAND. Original sind nur die Bausteine, je mit @0x belegt (monitor_r41.c):
 *   - der Monitor-Bild-Effekt: Zone-1-Bit 0x1E (DAT_800aca3c & 2) -> FUN_80020610 (@0x8001d1d8 je Bild) -> FUN_80020794(0,0):
 *     240 Zeilen-Kopien mit Versatz Sinus/64 (Tabelle @0x80072d70). Im ganzen Spiel setzt ihn genau EIN Skript: ROOM5031
 *     sub03 @0x011A8 Set(1,0x1E)=1 / @0x011E6 Set(1,0x1E)=0 um das Sherry-Monitorbild (Cut 11) - dieselbe Form hier;
 *   - die Kamerafuehrung aus ROOM5031 sub03: Cut_chg 7 (Nahaufnahme Monitor) @0x01194 + Sleep 20 @0x01196, Monitorbild,
 *     Cut_chg 9 (Monitorwand) @0x011EA + Sleep 50 @0x011F8, Cut_chg zum Spieler + Cut_auto 1 @0x01208/@0x0120A,
 *     Riegel auf @0x0120C.
 * Das Monitorbild selbst ist die unveraenderte Szene in ROOM50A0/50A1 Cut 12: der Port wechselt dafuer ohne Spieler in den
 * Hangar (Raumwechsel wie eine Tuer, Spieler unsichtbar ausserhalb des Ankers von Cut 12) und danach zurueck an die Stelle
 * des Spielers. Ausserhalb dieses Monitorbilds gibt es im Hangar keinen Birkin mehr.
 */
#ifndef RE15_MONITOR_R41_H
#define RE15_MONITOR_R41_H

#include <stdint.h>

/* PORT-WAHL (Zensus Bank 9, VERTRAG Runde 41 1.1 / Runde 42 1.1): Bit 38 ist im Original (bank9_zensus.py: frei 23..51),
 * im Port und in r42/story (23..37) frei. "Monitorbild gesehen" - je Spielstand genau einmal (Leon UND Elza). */
#define RE15_MON41_BANK  9
#define RE15_MON41_BIT   38

/* Phasen (re15_monitor_r41_phase). */
enum {
    RE15_MON41_AUS = 0,     /* wartet (oder schon gesehen) */
    RE15_MON41_VOR,         /* 5030/5031: Cut 7, Nahaufnahme Monitor */
    RE15_MON41_HIN,         /* Raumwechsel in den Hangar angefordert */
    RE15_MON41_BILD,        /* 50A0/50A1: Monitorbild, die Hangar-Szene laeuft */
    RE15_MON41_ZURUECK,     /* Raumwechsel zurueck angefordert */
    RE15_MON41_NACH,        /* 5030/5031: Cut 9, Monitorwand, dann zurueck zum Spieler */
    RE15_MON41_ENDE         /* Runde 2: 50A0/50A1 - Szene beendet, ihr letztes Bild (h135) wird noch gezeigt, Rueckweg im naechsten Takt */
};

/* Runde 2 (Pruefer SICHT R1/R2): Darstellung des Monitorbilds im PC-Zeichner (render_pc.c pc_monitor_look), je Bild.
 * PORT-WAHL, Herleitung monitor_r41.c Kopf Punkt 5. */
enum {
    RE15_MON41_LOOK_AUS = 0,    /* normales Bild */
    RE15_MON41_LOOK_MERKEN,     /* 5030/5031 Cut 7: das fertige Bild (vor Letterbox/Text) als Standbild des Monitors merken */
    RE15_MON41_LOOK_STANDBILD,  /* Hangar geladen, Szene noch ohne Figuren: das gemerkte Standbild statt des leeren Hangars */
    RE15_MON41_LOOK_MONITOR,    /* Szene laeuft: Standbild + das Szenenbild verkleinert im Bildschirm des Monitors */
    RE15_MON41_LOOK_HALTEN      /* zurueck im Monitor Room, Bild des Ladetakts: das zuletzt zusammengesetzte Bild halten */
};
int  re15_monitor_r41_look(void);
/* 1 = der eben ausgefuehrte Raumwechsel ist ein Monitor-Schnitt (harter Schnitt wie Cut 7 -> Cut 11 / Cut 11 -> Cut 9 in
 * ROOM5031 sub03 @0x011A6/@0x011EA, ohne Schwarz + Einblende); setzt die Marke zurueck (room_common.c
 * re15_room_transition_present). */
int  re15_monitor_r41_harter_schnitt(void);
/* MESSHAKEN RE15_MONITOR_OHNE_LOOK=1 (Alias RE15_MONITOR_OHNE_ZEILEN): kein Zeileneffekt, kein Monitor-Rahmen, keine
 * Letterbox vor der Szene - die Bilder der Szene wie im abgenommenen Stand (Beweis bitgleich). Kein Spielverhalten. */
int  re15_monitor_r41_ohne_look(void);
/* Bildschirm des Monitors in Cut 7 (logische 320x240-Koordinaten, Ecken oben links, oben rechts, unten rechts, unten links),
 * fuer den Monitor Room raum (5030/5031). 0 = keine Flaeche bekannt. */
int  re15_monitor_r41_schirm(unsigned raum, float ecken[8]);
unsigned re15_monitor_r41_raum(void);
/* Messzeile [monitor-bild] je Bild im Hangar waehrend des Monitorbilds (RE15_HANGAR_LOG). */
void re15_monitor_r41_bild_log(unsigned bild, int modus);

/* Je KI-Tick (aus re15_bf_raumtick, vor der Hangar-Szene). */
void re15_monitor_r41_tick(void);
/* Ende des Raumwechsels (room_common.c re15_room_apply_pending, nach dem SCD-Eintritt): Zonen der Szene halten. */
void re15_monitor_r41_raum_geladen(void);
/* 1 = der aktuelle Raum ist der Hangar und zeigt gerade das Monitorbild (re15_bf_hangar_aktiv). */
int  re15_monitor_r41_bild(void);
/* 1 = der Spieler ist nur "unterwegs" im Hangar (Karte: kein Besuch, keine Zone). */
int  re15_monitor_r41_unterwegs(void);
/* Ende der Hangar-Szene im Monitorbild (birkin_formen.c bf_hangar_beenden): Rueckweg anfordern. */
void re15_monitor_r41_szene_ende(void);
int  re15_monitor_r41_phase(void);
void re15_monitor_r41_reset(void);       /* Testhaken / neues Spiel: Ablauf (nicht das Bit) zuruecksetzen */
/* Gespeicherte Stelle des Spielers beim Ausloesen (Testhaken). */
void re15_monitor_r41_testhaken_hin(unsigned raum_mon, int32_t x, int32_t y, int32_t z, int16_t gier, int cut);   /* Testhaken */
void re15_monitor_r41_stelle(int32_t *x, int32_t *y, int32_t *z, int16_t *gier, int *cut, unsigned *raum);

/* Monitor-Bild-Effekt (FUN_80020610 dritter Zweig -> FUN_80020794(0,0), PC-Zeichner render_pc.c): 1 = an in diesem Bild.
 * Rechnet dann die Zeilenzuordnung des Bilds: quelle[y] = Zeile, deren Inhalt nach den 240 Kopien in Zeile y steht
 * (Reihenfolge der Kopien wie in der OT, s. monitor_r41.c), und schaltet die Phase DAT_800b5576 um 2 weiter. */
int  re15_monitor_r41_zeilen(uint8_t quelle[240]);
/* Dasselbe ohne Flag-Abfrage und mit fester Phase (Testhaken). */
void re15_monitor_r41_zeilen_phase(uint8_t phase, uint8_t quelle[240]);

#endif
