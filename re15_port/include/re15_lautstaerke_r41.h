/* re15_lautstaerke_r41.h — Runde 41 Spur D "kleinkram_a": Lautstaerke-Option.
 * Dossier: analysis/befunde_runde41/D_kleinkram_a.md §1. Code: engine/src/lautstaerke_r41.c (Zustand, Datei),
 * platform/pc/src/lautstaerke_r41_pc.c (Zeile im SOUND-Schirm, Eingabe), Endstufe platform/pc/src/audio_pc.c.
 *
 * NUTZER-VORGABE (AUFTRAG.md 2026-10-23): "Mir fehlt in den Optionen eine Lautstaerke Option bei der ich alles
 * lauter oder leiser machen kann."
 *
 * ORIGINAL (vollstaendig gelesen, ghidra1_V2.txt:122560-122800): der SOUND-Schirm ist der Sub-Treiber FUN_8002f0d4
 * (Handler-Tabelle @0x80073d9c[5]) und kennt nur Stereo/Mono. Einzige Pad-Abfrage `lhu v1,DAT_800ac762` @0x8002f268:
 *   `andi v0,v1,0x40`   @0x8002f270  Cross         -> zurueck OHNE Speichern (@0x8002f27c-8c)
 *   `andi v0,v1,0xa0`   @0x8002f278  Kreis/Quadrat -> Arbeitskopie S[7] -> DAT_800b0fce (`sh v0` @0x8002f2c0),
 *                                     Anwenden `jal FUN_80043c00` @0x8002f2c8
 *   `andi v0,v1,0x5000` @0x8002f294  UP|DOWN       -> S[7] ^= 1 (`xori v0,v0,0x1` @0x8002f2e8)
 * LEFT (0x8000) und RIGHT (0x2000) werden im ganzen Treiber NICHT gelesen -> frei fuer die Port-Zeile. Beim Eintritt
 * (Phase 0) laedt der Treiber die Arbeitskopie aus dem gespeicherten Wert (`lbu v0,DAT_800b0fce` / `sb v0,7(v1)`
 * @0x8002f244-4c). Kacheln: w=0x58 h=0x12 x=0x74 y=0x33 u=0 v=0xe8 (@0x8002f11c-30), zweite y=0x46 (@0x8002f17c).
 * RE2 PSX (Leon) hat ebenso keine Lautstaerke (kein "VOLUME"-Text in ghidra_re2_Leon.txt; Options = Stereo/Monaural).
 *
 * => PORT-WAHL, FORM aus dem Original:
 *   - dieselbe CONFIG.TIM-Kachel uv(0,232) 88x18 bei x 116 wie STEREO/MONO, Beschriftung "VOLUME" in der Spielschrift
 *     ab x 119 (wie STEREO), die Zahl rechtsbuendig bis x 201 (3 px Rand wie links). Vorschuebe BIN/DEBUG.BIN@0x4416:
 *     "VOLUME" 52 px, "100" 23 px. LAGE: im freien Streifen unter dem byte-true Kasten (Kastenende y 220 -> Kachel
 *     y 221..238; Vorbild = die AI-Zeile des Ports, CFG_AI_Y 225 in main.c). Die Raster-Lage y 89 (51/70 -> 89) wurde
 *     am Shot GEMESSEN und verworfen: dort liegen die Original-Beschriftungskaesten DAT_80073d2c[2] = (46,91) und
 *     [8] = (203,91), die Kachel 116..204 verdeckte "Forward" ("Forwar") und den Rand des rechten Kastens.
 *   - Stufen 0..100 in Zehnerschritten, LEFT = leiser, RIGHT = lauter (sofort hoerbar = Arbeitskopie), Kreis/
 *     Quadrat speichert (wie @0x8002f2c0), Cross verwirft (wie @0x8002f27c), Eintritt laedt (wie @0x8002f244-4c).
 *   - WIRKUNG: EINE Endstufe am Ende der Mischkette audio_callback (audio_pc.c) nach der Mono-Faltung — dort laufen
 *     SE-Samples, XA-Stimmen, FMV-Ton, BGM und Ambience zusammen; RE15_AUDIO_CAP_SYNC rendert durch denselben
 *     Rueckruf. Linear wie die SPU-Hauptlautstaerke: Q15, 100 % = 0x8000 = unveraendert, 50 % = 0x4000 = halbe
 *     Amplitude (RMS halbiert).
 *   - PERSISTENZ: der Port speicherte den Sound-Modus bisher gar nicht (s_config_sound = static in main.c). Textdatei
 *     RE15_LAUT_R41_DATEI am Ort der Speicherkarte re15_card.mcr (Arbeitsverzeichnis, main.c RE15_CARD_PATH),
 *     Zeilen `volume=<0..100>` und `sound=stereo|mono`; Umgebungs-Override RE15_OPTIONS_FILE (Messung/Tests). */
#ifndef RE15_LAUTSTAERKE_R41_H
#define RE15_LAUTSTAERKE_R41_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

#define RE15_LAUT_R41_DATEI    "re15_options.cfg"   /* neben re15_card.mcr (main.c RE15_CARD_PATH, CWD-relativ) */
#define RE15_LAUT_R41_ENV      "RE15_OPTIONS_FILE"  /* Override des Pfads (Messschiene/Tests)                  */
#define RE15_LAUT_R41_MAX      100                  /* Prozent                                                   */
#define RE15_LAUT_R41_SCHRITT  10                   /* je LEFT/RIGHT                                             */
#define RE15_LAUT_R41_Q15_VOLL 0x8000               /* 100 % = unveraendert (SPU-Hauptlautstaerke linear)        */

/* Datei lesen (fehlt sie: 100 %, Stereo). Setzt gespeicherten Wert UND Arbeitskopie. */
void        re15_lautstaerke_r41_laden(void);
/* gespeicherten Stand schreiben; 0 = ok, -1 = Datei nicht schreibbar. */
int         re15_lautstaerke_r41_speichern(void);
const char *re15_lautstaerke_r41_pfad(void);

/* Arbeitskopie (wird sofort angewendet = hoerbar). */
int         re15_lautstaerke_r41_prozent(void);
/* Q15-Faktor der Endstufe aus der Arbeitskopie: prozent * 0x8000 / 100. */
int         re15_lautstaerke_r41_q15(void);
/* Arbeitskopie um einen Schritt (richtung < 0 leiser, > 0 lauter), geklemmt 0..100. */
void        re15_lautstaerke_r41_schritt(int richtung);
/* Arbeitskopie direkt setzen (geklemmt). */
void        re15_lautstaerke_r41_setzen(int prozent);

/* Gespeicherter Stand. */
int         re15_lautstaerke_r41_gespeichert(void);
int         re15_lautstaerke_r41_stereo(void);          /* 1 = Stereo, 0 = Mono */

/* Schirm-Eintritt (@0x8002f244-4c): Arbeitskopie := gespeichert; liefert den gespeicherten Stereo-Stand. */
int         re15_lautstaerke_r41_eintritt(void);
/* Bestaetigen (@0x8002f2c0): gespeichert := Arbeitskopie, Stereo-Stand mit; schreibt die Datei (Rueckgabe wie
 * re15_lautstaerke_r41_speichern). */
int         re15_lautstaerke_r41_bestaetigen(int stereo);
/* Cross (@0x8002f27c): Arbeitskopie := gespeichert. */
void        re15_lautstaerke_r41_verwerfen(void);

/* Reiner Parser/Formatierer der Datei (Tests): Rueckgabe = Zahl erkannter Schluessel (volume, sound). */
int         re15_lautstaerke_r41_parse(const char *text, int *prozent, int *stereo);
int         re15_lautstaerke_r41_format(char *buf, size_t n, int prozent, int stereo);
/* Test: Zustand auf die Voreinstellung (100, Stereo) ohne Datei. */
void        re15_lautstaerke_r41_reset(void);

#ifdef __cplusplus
}
#endif
#endif /* RE15_LAUTSTAERKE_R41_H */
