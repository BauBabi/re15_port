#ifndef RE15_MAP_HINT_H
#define RE15_MAP_HINT_H

/* ============================================================================
 * KARTENHINWEIS NACH DER IRONS-SZENE — ⛔ RE2-ERGAENZUNG, KEIN RE1.5-ORIGINAL
 * ============================================================================
 * Nutzer-Auftrag Runde 30, Thema B (analysis/befunde_runde30/AUFTRAG.md): "In ROOM 3010
 * in Resident Evil 2 gibt es nach der Cutscene eine Stelle wo die Map aufgeht und zeigt,
 * wo der Spieler hin gehen muss ... So einen aehnlichen Mechanismus moechte ich bitte
 * nach der Irons Cutscene in Irons Office im ROOM 1150 ebenfalls haben, der mir dann den
 * communication ROOM markiert."
 *
 * WARUM RE2 UND NICHT RE1.5: RE1.5 hat diesen Mechanismus nicht. Seine SCD-Tabelle endet
 * bei Opcode 0x5E (95 Eintraege), RE2s Hinweis-Opcode ist 0x84 (Handler @0x800591C4, aus
 * der Tabelle 0x800A74C8 + 0x84*4 = @0x800A76D8). Nach der Regel Beta->Retail ist RE2 das
 * Vorbild; jede Konstante unten traegt deshalb eine RE2-Adresse (info/re2leon/PSX.EXE,
 * t_addr 0x80010000, Kopf 0x800). Dossier: analysis/befunde_runde30/karte-3010.md.
 *
 * DER RE2-MECHANISMUS IN EINEM SATZ: Opcode 0x84 setzt den Statusschirm-Modus 4
 * (`sb v0,[0x800D5C00]` @0x800591DC, v0 = 4) samt Anforderung (Phase 1 @0x800591E8,
 * Bit 0x8000 @0x800591EC) und die Hinweis-Nummer (@0x80059210); Modus 4 blendet ein
 * FESTES Kartenblatt ein (Init @0x8006F6A8), laesst den Zielraum zwischen CLUT-Zeile 502
 * und 498 wechseln (Zeichner FUN_8006F1C4) und schliesst nur auf Status/Abbruch
 * (Maske 0x6000 @0x8006F884). Nach dem Schliessen bleibt NICHTS zurueck: kein Flag, keine
 * Marke, kein Besucht-Bit (alle jal im Zeichner-Bereich: 0 x Bit-Setzer 0x8007730C).
 * ==========================================================================*/

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ---- Blinkzaehler (RE2 FUN_8006F1C4 @0x8006F20C-0x8006F284, Init @0x8006F6A8) ------- */

/* Startwert des Zaehlers [0x800D5C18]: `addiu v0,zero,10` @0x8006F6DC, `sb` @0x8006F6F4. */
#define RE15_HINT_ZAEHLER_START   10
/* Startwert der Richtung [0x800D5C19]: `addiu v1,zero,1` @0x8006F6B4, `sb` @0x8006F6C4. */
#define RE15_HINT_RICHTUNG_START  1
/* Richtung != 0: Zaehler < 10 -> Ton + Richtung := 0: `sltiu v0,v0,0xa` @0x8006F22C. */
#define RE15_HINT_ZAEHLER_UNTEN   10
/* Richtung == 0: Zaehler >= 0x51 -> Richtung := 1: `sltiu v0,v1,0x51` @0x8006F264. */
#define RE15_HINT_ZAEHLER_OBEN    0x51
/* Schrittweite: `addiu v0,v0,-2` @0x8006F254, `addiu v0,v1,2` @0x8006F26C/@0x8006F27C. */
#define RE15_HINT_SCHRITT         2

/* ---- Takt: ein Zaehlschritt je Durchgang des RE2-Statusschirms ----------------------
 * Der Status-Task setzt den VSync-Teiler [0x800DFC1A] = 0 (`sb zero,-998(at)`
 * @0x80068A1C); die Hauptschleife liest ihn als Argument von VSync (`lbu a0,[0x800DFC1A]`
 * @0x8002B994, `jal 0x80085EA0` @0x8002B998). VSync(0) = warten auf den NAECHSTEN VBlank
 * (@0x80085EF0-0x80085EFC: a0 != 1, a0 <= 0 -> Warte-Zweig). Also genau ein Durchgang =
 * ein Zeichner-Aufruf = ein Zaehlschritt je VBlank. */
#define RE15_HINT_VBLANKS_JE_SCHRITT 1
/* VBlank-Rate in Millihertz: NTSC nicht interlaced 59,826 Hz — psx-spx
 * graphicsprocessingunitgpu.md "Vertical Refresh Rates"; dieselbe Zahl wie die
 * Titel-Uhr der Titel-Spur (re15_title_pulse.h, RE15_TITLE_VBLANK_MILLIHZ, dort am
 * DISPENV.isinter = 0 der Titel-Savestates belegt). Dass RE2s STATUSSCHIRM nicht
 * interlaced laeuft, ist NICHT eigens nachgewiesen; bei 59,94 Hz (interlaced) wiche
 * eine Phase von 39 Schritten um 1,2 ms ab. */
#define RE15_HINT_VBLANK_MILLIHZ     59826

/* ---- Wanduhr des Hosts ---------------------------------------------------------------
 * Der Blinker laeuft auf der ZEIT, nicht auf der Bildrate der Anzeige (dieselbe
 * Fehlerklasse liess in Runde 30 das Titelmenue 4,8-mal zu schnell laufen). Die Plattform
 * setzt die Uhr einmal je Bild VOR re15_game_step (PC: SDL-Performance-Zaehler in
 * platform/pc/main.c); die Tests setzen sie selbst. Einheit Mikrosekunden, monoton. */
void     re15_host_clock_set_us(uint64_t now_us);
uint64_t re15_host_clock_us(void);

/* Volle VBlanks in elapsed_us:  floor(elapsed_us * 59826 / 10^9)  (ganzzahlig, 64 Bit). */
uint64_t re15_map_hint_vblanks(uint64_t elapsed_us);

/* ---- Ausloeser im Skript (Port-Gegenstueck zum Opcode 0x84) --------------------------
 * RE1.5s Skript hat keinen Opcode 0x84; der Hinweis haengt deshalb an einer Stelle im
 * GELADENEN RDT-Puffer (Muster scd_elev_se.c). Tabelle und Belege in map_hint_common.c. */
extern int g_re15_map_hint_anchor_n;   /* Anker im registrierten Raum (sonst 0) */

/* Beim Registrieren eines Raums: Signatur im rohen Puffer suchen, NUR im Quellraum der
 * Tabelle. Loescht eine noch stehende Anforderung. raw == NULL loescht alles. */
void re15_map_hint_room_scan(const unsigned char *raw, int raw_size, unsigned room_id);
/* Im SCD-Verteiler vor jedem Opcode: pc == Anker -> Anforderung merken. */
void re15_map_hint_pc(const unsigned char *pc);
/* Datei-Offset des Ankers im registrierten Puffer (-1 = keiner) — fuer den Riegel. */
long re15_map_hint_anchor_off(void);

/* Anforderung: -1 = keine, sonst die Hinweis-Nummer (Index der Tabelle). Sie bleibt
 * stehen, bis re15_map_hint_take sie verbraucht (RE2: Bit 0x8000 [0x800CFBD8] bleibt
 * gesetzt, bis der Status-Task startet, @0x80025BA8-E0). */
int  re15_map_hint_pending(void);
void re15_map_hint_take(void);

/* Ziel des Hinweises nr: Blatt und Rechteck der HAUPTZEILE (etage == 0) des Zielraums in
 * der Zonen-Tabelle. 1 = gefunden. */
int  re15_map_hint_ziel(int nr, int *page, int *rect);

/* ---- Blinker ------------------------------------------------------------------------ */
/* Init: Zaehler 10, Richtung 1, Nullpunkt der Uhr = jetzt (RE2 @0x8006F6B4-0x8006F6F4). */
void re15_map_hint_begin(void);
/* Je Menue-Bild: alle seit dem Nullpunkt faelligen Zaehlschritte nachholen (je VBlank
 * einer). Mehr als ein Ton je Aufruf wird nicht ausgegeben. */
void re15_map_hint_tick(void);
/* EIN Zaehlschritt, Befehl fuer Befehl nach @0x8006F20C-0x8006F284 (samt Ton). */
void re15_map_hint_step(void);
/* 1 = rote Phase (Richtung == 0 -> CLUT 501+1 = 502, `addiu s2,s2,1` @0x8006F514),
 * 0 = Umriss-Phase (Richtung != 0 -> CLUT 498, `addiu s2,zero,498` @0x8006F5DC). */
int  re15_map_hint_rot(void);
int  re15_map_hint_zaehler(void);
int  re15_map_hint_richtung(void);
/* Ausgefuehrte Zaehlschritte seit re15_map_hint_begin (Messschiene). */
uint64_t re15_map_hint_schritte(void);
/* Periode der Zaehlfolge in Schritten — aus den Konstanten oben durch Nachspielen
 * bestimmt, nicht hineingeschrieben (Erwartung laut Dossier 3.5 a: 78). */
int  re15_map_hint_periode(void);

/* ---- Satz-TOC der Mini-Bank shared_assets/RE2/HINTSE.VBS ---------------------------
 * Muster re15_elev_bank_rec (gen/re2_hint_bank.inc, erzeugt von tools/re2_hint_cut.py). */
typedef struct {
    unsigned edt_off, edt_size;   /* SE-Map @+0, VH @u32[edt_size-8] */
    unsigned vbd_off, vbd_size;   /* VB-Rumpf                        */
    unsigned vag1_size;           /* 4480 (Erwartung)                */
    int      se_hint;             /* 0x2B                            */
} re15_map_hint_bank_rec_t;
void re15_map_hint_bank_rec(re15_map_hint_bank_rec_t *out);

#ifdef __cplusplus
}
#endif

#endif /* RE15_MAP_HINT_H */
