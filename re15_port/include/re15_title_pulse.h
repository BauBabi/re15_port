/* re15_title_pulse.h — Titelmenue: Helligkeitspuls der aktiven Zeile + Titel-Tick.
 *
 * Runde 30 / Thema D (Nutzer 2026-09-27): "Im Titelbild bei der Auswahl New Game, Load Game,
 * Option blinkt der ausgewaehlte Bereich in der Frequenz im Vergleich zum Original zu schnell."
 * Dossier: analysis/befunde_runde30/titel-blinken.md
 *
 * GEMESSEN (vor dem Umbau, Anzeige 144 Hz): ein Pulsschritt je Bild der Anzeige = 6,94 ms,
 * Periode 416 ms. ORIGINAL: ein Pulsschritt je Durchgang der Hauptschleife = VSync(2) =
 * 33,43 ms, Periode 60 x 2 VBlanks = 2006 ms.
 *
 * Die Pulsfolge selbst war im Port richtig; falsch war, dass der Schritt im ZEICHNEN sass und
 * damit an der Bildrate der Anzeige hing. Dieses Modul trennt beides:
 *   - der PULS (Rumpf von FUN_801028ec, TITLE.BIN) ist ein reiner Zustand ohne SDL,
 *   - der TITEL-TICK rechnet aus vergangener ZEIT, wie viele Durchgaenge der Original-
 *     Hauptschleife faellig sind.
 * Beides liegt in engine/, damit es ohne Fenster pruefbar ist
 * (tests/unit/test_r30_titel_blinken.c).
 *
 * Quellen: info/Re1.5/PSX/BIN/TITLE.BIN (laedt @0x80100000 ohne Kopf, Datei-Offset =
 * Adresse - 0x80100000) und info/Re1.5/PSX.EXE (Datei-Offset = 0x800 + Adresse - 0x80010000).
 * RE2 wird NICHT herangezogen: RE1.5 hat dieses System vollstaendig.
 */
#ifndef RE15_TITLE_PULSE_H
#define RE15_TITLE_PULSE_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ---- Puls: FUN_801028ec (TITLE.BIN 0x801028ec-0x80102940) -------------------------------- */

/* Startwert des Pulswerts: Datenwort @0x80102944 = TITLE.BIN Datei 0x2944 = 0x0080;
 * derselbe Wert ist der Ruecksetzwert `ori a0,zero,0x80` @0x80102928. */
#define RE15_TITLE_PULSE_START    0x80
/* Schwelle "aufwaerts": `sltiu v0,v1,0x1f` @0x801028fc — Zaehler < 31, Vergleich VOR dem
 * Inkrement (@0x80102914). */
#define RE15_TITLE_PULSE_UP_BELOW 0x1f
/* Schrittweite: `addiu a0,a0,2` @0x8010290c / `addiu a0,a0,-2` @0x80102910. */
#define RE15_TITLE_PULSE_DELTA    2
/* Periode in Aufrufen: `ori v0,zero,0x3c` @0x80102918, `bne v1,v0` @0x8010291c. */
#define RE15_TITLE_PULSE_PERIOD   0x3c

/* Zaehler := 0 (Datei 0x2946 / `ori v1,zero,0` @0x80102924), Wert := 0x80 (Datei 0x2944 /
 * @0x80102928). Aufzurufen bei JEDEM Eintritt in den Titel: beide Wege dorthin laufen ueber
 * FUN_80029a28(0) (`jal 0x80029a28` @0x80021318 Boot-Task und @0x8001d208 Ende der Spiel-Task,
 * beide mit `addu a0,zero,zero` im Verzoegerungsplatz), und FUN_80029a28 laedt die Datei
 * UNBEDINGT neu (`jal 0x80013b60` @0x80029a64, Datei-Id aus Tabelle @0x80073bdc[0] = 6) —
 * die beiden Datenworte kommen also jedes Mal frisch von der CD. */
void re15_title_pulse_reset(void);

/* Ein Aufruf von FUN_801028ec. */
void re15_title_pulse_step(void);

/* n Aufrufe. Die Folge ist 0x3c-periodisch (@0x80102918), also genuegen n mod 0x3c Schritte —
 * der Zustand danach ist derselbe wie nach n Schritten. Damit bleibt die Arbeit je
 * Schleifendurchgang auch nach einem Stillstand (Fenster gezogen, Haltepunkt) unter 60
 * Schritten, OHNE dass der Puls von der Zeit abweicht. */
void re15_title_pulse_advance(uint64_t n);

/* Pulswert @0x80102944 (0x80 ... 0xBE) = Farbbyte R=G=B des modulierten Rechtecks der aktiven
 * Zeile (`lhu t0,0x2944(t0)` @0x80102848, `sb` @0x80102850/54/58). */
int re15_title_pulse_value(void);

/* Zaehler @0x80102946 (0 ... 59). */
int re15_title_pulse_counter(void);

/* ---- Titel-Tick: ein Durchgang der Original-Hauptschleife -------------------------------- */

/* VBlanks je Durchgang im Titel: die Boot-Task setzt DAT_800b5456 := 2 unmittelbar vor dem
 * Start von TITLE.BIN (`ori v0,zero,2` @0x8002130c, `sb v0,0x5456(at)` @0x80021314); der Flip
 * FUN_8002137c liest den Wert (`lbu a0,0(s0)` @0x8002147c) und ruft VSync(a0)
 * (`jal 0x80061fc0` @0x80021480). VSync(n>1) kehrt n VBlanks nach dem vorigen Ruecksprung
 * zurueck (Ziel = letzter - 1 + n @0x80062028-38, danach noch ein VBlank @0x8006206c-78).
 * Der Titel-Task gibt je Durchgang genau einmal ab (`ori a0,zero,1` @0x80101fc0 ->
 * FUN_80029ac8 @0x80101fbc) und ruft im Menue genau einen Pulsschritt
 * (`jal 0x801028ec` @0x80102ba0). */
#define RE15_TITLE_VBLANKS_PER_TICK 2
/* VBlank-Rate in Millihertz: NTSC non-interlaced 59,826 Hz — psx-spx
 * graphicsprocessingunitgpu.md "Vertical Refresh Rates" (Zeile 1261-1264). Der Titel laeuft
 * NICHT in 480i: DISPENV.isinter @0x800b5438 = 0 in den Titel-Savestates (boot_40.sav,
 * mzd_title.sav). Gegenmessung DuckStation-Aufnahme: 59,8173 Bilder/s, jede Pulsstufe haelt
 * genau 2 Bilder (Dossier §3.6 c). */
#define RE15_TITLE_VBLANK_MILLIHZ   59826

/* Zahl der vollen Durchgaenge in elapsed_us Mikrosekunden:
 *   floor(elapsed_us * 59826 / (2 * 10^9))          (T_TICK = 2 / 59,826 s = 33430,28 us)
 * Ganzzahlig, ohne Rundung der Tickdauer. */
uint64_t re15_title_tick_count(uint64_t elapsed_us);

/* Uhr der Titel-Schleife. `start` setzt den Nullpunkt auf den Beginn eines Durchgangs (Eintritt
 * in den Titel und Rueckkehr aus Bestaetigungs-Fade/Unterbildschirm); `poll` liefert, wie viele
 * WEITERE Durchgaenge seitdem faellig geworden sind. Waehrend des Fades und der
 * Unterbildschirme wird sie nicht abgefragt — dort ruht im Original auch der Puls: FUN_80102a10
 * (Neuzeichner des Fades, `jal 0x80102a10` @0x80102d10/@0x80102d60) ruft FUN_801028ec nicht,
 * und ausser FUN_801028ec schreibt niemand die beiden Worte. */
typedef struct re15_title_clock {
    uint64_t start_us;  /* Beginn von Durchgang 0                         */
    uint64_t ticks;     /* bereits ausgegebene Durchgaenge (ohne den 0.)  */
} re15_title_clock_t;

void     re15_title_clock_start(re15_title_clock_t *c, uint64_t now_us);
/* neue Durchgaenge seit dem letzten Aufruf */
uint64_t re15_title_clock_poll(re15_title_clock_t *c, uint64_t now_us);

/* ---- Titel-Einblende ------------------------------------------------------------------ */

/* Blauwert (= R = G) des subtraktiven Vollbild-Rechtecks der Titel-Einblende im Durchgang
 * `tick` (0 = erster). Fade-Engine der EXE, Kanal 0:
 *   Aufbau  FUN_800217b0(0x200, 0xfc00, 7, 3): `ori a1,zero,0xfc00` @0x80102058,
 *           `jal 0x800217b0` @0x80102060 -> Schritt -0x400 (`sh a1,2(s0)` @0x800217e4);
 *   Anstoss FUN_800216ec (`jal 0x800216ec` @0x80102078): Schritt != 0 und < 1
 *           (`slti v0,v0,1` @0x80021710) -> Pegel := 0x7fff (`subu v0,zero,v0` @0x80021714,
 *           `andi v0,v0,0x7fff` @0x80021718, `sh v0,0(v1)` @0x80021720);
 *   Takt    FUN_80021880, einmal je Durchgang der Hauptschleife (`jal 0x80021880`
 *           @0x80020f44): Farbe = Pegel >> 7 (`sll v0,v0,16` @0x800218c8, `sra a0,v0,23`
 *           @0x800218d0) VOR der Integration (`addu v0,v0,v1` @0x80021928); fertig, sobald
 *           der Pegel negativ ist (`bltz v0` @0x800218cc).
 * Ergibt 255, 247, ... 7 fuer tick 0 ... 31 und 0 (nichts gezeichnet) ab tick 32. */
int re15_title_fadein_level(uint64_t tick);

#ifdef __cplusplus
}
#endif

#endif /* RE15_TITLE_PULSE_H */
