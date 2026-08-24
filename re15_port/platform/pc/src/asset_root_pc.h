/* =============================================================================================
 * RE1.5 Rebuilt — EINE Asset-Wurzel-Aufloesung fuer das PC-Target (2026-08-24).
 *
 * WARUM ES DAS GIBT (Nutzer-Report 0.3.19: "Es fehlen die Optionen in config, die subtitles usw."):
 * Bis hierher hatte JEDER Lade-Pfad im PC-Target seine EIGENE Wurzelliste — main.c pc_read_shared,
 * main.c pc_re2_cdemd, asset_pc.c, audio_pc.c (ARMS/CORE/BGM/VOICE/ENEMSE), bg_pc.c, room_pc.c,
 * inv_render_pc.c, render_pc.c (msgfont). Acht Listen, jede anders, die meisten am EINKOMPILIERTEN
 * Pfad (RE15_ASSET_ROOT_DEFAULT) verankert. Auf der Entwickler-Maschine zeigt der ins Repo, also
 * hat das Paket in Wahrheit aus dem Repo gelesen; im Docker-Cross-Build ist der Default der
 * Container-Pfad "/src/re15_port/shared_assets/PSX", der beim Nutzer nicht existiert. Ergebnis:
 * DATA/TEX.TIM (die Spielschrift) wurde nicht gefunden -> keine CONFIG-Labels, keine Untertitel,
 * keine Dialoge, keine Item-Namen; dazu stumme BGM und fehlende extracted_fx-Effekte.
 *
 * KEINE dieser Listen hat je gefragt: "wo liegt eigentlich MEINE exe?". Genau das tut dieses Modul.
 *
 * DREI WURZEL-ARTEN (jede eine geordnete Liste, hoechste Prioritaet zuerst):
 *   CD-Wurzel      = ein Verzeichnis, das DATA/, BIN/, STAGE1/, SOUND/, BSS/, EMD/, ITEM/, PLD/,
 *                    RBJ/ enthaelt (im Baum: shared_assets/PSX).
 *   Shared-Wurzel  = dessen Elternverzeichnis (enthaelt PSX/, RE2/, extracted_fx/).
 *   Basis-Wurzel   = das Paket-/Projekt-Verzeichnis (enthaelt shared_assets/ und/oder synchro/).
 *
 * PRIORITAETENKETTE (in dieser Reihenfolge, erster Treffer gewinnt):
 *   1. env RE15_CD_ROOT / RE15_ASSET_ROOT / RE15_RE2_ASSET_ROOT  (Skripte + Tests haengen daran)
 *   2. das Verzeichnis der laufenden exe und bis zu 4 Elternverzeichnisse
 *   3. das Arbeitsverzeichnis und bis zu 4 Elternverzeichnisse
 *   4. der einkompilierte Default (RE15_ASSET_ROOT_DEFAULT / RE15_CD_ROOT_DEFAULT) — ZULETZT,
 *      damit ein ausgeliefertes Paket seine eigenen Assets nimmt und nicht heimlich ein Repo.
 * Kandidaten aus (2)/(3) werden gefiltert: nur Verzeichnisse, die tatsaechlich shared_assets/
 * oder synchro/ enthalten, kommen in die Liste (haelt sie kurz — typisch 1-3 Eintraege).
 * ============================================================================================= */
#ifndef RE15_ASSET_ROOT_PC_H
#define RE15_ASSET_ROOT_PC_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Verzeichnis der laufenden exe, ohne abschliessenden Trenner ("" wenn nicht ermittelbar). */
const char *re15_pc_exe_dir(void);

/* Geordnete Wurzel-Listen. i ab 0; NULL hinter dem letzten Eintrag. Pfade OHNE End-Trenner. */
const char *re15_pc_cd_root(int i);      /* .../shared_assets/PSX          */
const char *re15_pc_shared_root(int i);  /* .../shared_assets              */
const char *re15_pc_base_root(int i);    /* Paketwurzel (shared_assets/, synchro/) */

/* Vollen Pfad aus der i-ten Wurzel bauen. Rueckgabe 1 = gebaut, 0 = keine i-te Wurzel. */
int re15_pc_cd_path(int i, const char *rel, char *buf, unsigned bufsz);
int re15_pc_base_path(int i, const char *rel, char *buf, unsigned bufsz);

/* Datei relativ zu der jeweiligen Wurzelliste lesen; malloc'd Puffer (Aufrufer gibt frei),
 * *size gesetzt, NULL wenn in KEINER Wurzel gefunden. */
uint8_t *re15_pc_read_cd(const char *rel, int *size);      /* <cd>/<rel>      z.B. "DATA/TEX.TIM"     */
uint8_t *re15_pc_read_shared(const char *rel, int *size);  /* <shared>/<rel>  z.B. "extracted_fx/x.tim"*/
uint8_t *re15_pc_read_base(const char *rel, int *size);    /* <base>/<rel>    z.B. "synchro/..."       */
uint8_t *re15_pc_read_re2(const char *rel, int *size);     /* env RE15_RE2_ASSET_ROOT, sonst <shared>/RE2/<rel> */

/* Universeller Fallback fuer einen RELATIVEN Pfad unbekannter Bauart: CD -> Shared -> Basis.
 * Deckt in EINEM Aufruf "DATA/TEX.TIM", "extracted_fx/x.tim", "shared_assets/PSX/SOUND/x" und
 * "synchro/..." ab. Das ist der Netzboden unter re15_asset_read_file(). */
uint8_t *re15_pc_read_any(const char *rel, int *size);

/* Einmal eine Zeile mit der aufgeloesten CD-Wurzel nach stderr (Diagnose in Nutzer-Logs). */
void re15_pc_asset_roots_report(void);

#ifdef __cplusplus
}
#endif
#endif /* RE15_ASSET_ROOT_PC_H */
