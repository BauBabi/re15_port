/* granate_r36.c — Runde 36 Spur D: Handgranate. Belege und Einordnung: include/re15_granate_r36.h,
 * Dossier analysis/befunde_runde36/D_granate.md. */
#include "re15_granate_r36.h"
#include "re15_esp.h"
#include "re15_inventory.h"   /* re15_ammo_consume = FUN_8004eae4 */

static unsigned s_russ_unterdrueckt = 0;
static unsigned s_platz_geraeumt    = 0;
static unsigned s_verbraucht        = 0;

/* Punkt 1 — die Flammen des Spreng-Russ FUN_8010640C (@0x80106418-44 Part 0 / @0x80106450-74 Part 3).
 * RE2 Retail spawnt sie fuer die Zeilen 9 (HE-Granate 0x09, GL-Explosivrunde) und 17 (Rakete) — beide
 * sind Spreng-, keine Brandwaffen. NUTZER-VORGABE (AUFTRAG.md Runde 36, Punkt 1): kein Feuer-Effekt
 * bei normalen Granaten, Feuer nur bei der Brandgranate (deren Verkohlung FUN_80106128 laeuft ueber
 * Zeile 10/16 und ruft diese Funktion nie). Deshalb: fuer JEDE Zeile, die den Spreng-Russ erreicht,
 * keine Flammen. Die Russ-Tinten (@0x80106478-f8) setzt der Aufrufer weiterhin. */
int re15_granate_r36_russ_flammen(unsigned zeile)
{
    (void)zeile;                       /* 9 oder 17 (`beq v1,9` @0x80108cc8 / `bne v1,17` @0x80108cd0) */
    s_russ_unterdrueckt++;
    return 0;
}

unsigned re15_granate_r36_russ_unterdrueckt(void) { return s_russ_unterdrueckt; }
unsigned re15_granate_r36_platz_geraeumt(void)    { return s_platz_geraeumt; }
unsigned re15_granate_r36_verbraucht(void)        { return s_verbraucht; }
void     re15_granate_r36_zaehler_reset(void)     { s_russ_unterdrueckt = 0; s_platz_geraeumt = 0; s_verbraucht = 0; }

/* Nachbesserung 1 (M1/M2) — Verbrauch = Abgang (Herleitung re15_granate_r36.h (2c), Dossier §6).
 * Original: Abzug im Rueckstossbild 0 (Entlade-Handler 0x80033B38/58/78, `jal 0x8004eae4` @0x80033b40/60/80),
 * Abgang erst im Bild 19/22/24; ein Griff dazwischen (@0x80113b00/@0x80113b30) kostet die Granate ohne Wurf.
 * ORCHESTRATOR-ENTSCHEIDUNG ("wird nicht geworfen"): fuer die Wurf-Ids faellt der Abzug in das Spawnbild. */
int re15_granate_r36_verbrauch_im_spawnbild(int id)
{
    return id == 9 || id == 10 || id == 11;        /* Entlade-Tabelle @0x80074100 [9..11] = 0x80033B38/58/78 */
}

/* Spawnbild, Wurfkoerper hat seinen Platz (`jal 0x80019700` @0x800336ec/@0x80033748/@0x800337a4 erfolgreich):
 * derselbe FUN_8004eae4 wie im Entlade-Handler (eine Granate des ausgeruesteten Platzes). */
int re15_granate_r36_geworfen(void)
{
    int r = re15_ammo_consume();
    if (r) s_verbraucht++;
    return r;
}

/* Punkt 2b — voller Pool im Spawnbild. FUN_80019700 sucht 0..95 (`sltiu` @0x8001978c) und gibt bei
 * vollem Pool 0xFF zurueck (`ori v0,zero,0xff` @0x800197c8-cc): die Granate ist dann weg (Abzug im
 * Rueckstossbild 0), ohne Wurf. Gemessen (Dossier §3.1): nach einer HE-Explosion an zwei Zombies 95 von
 * 96 Plaetzen belegt (F120..F130), drei Zombies fuellen ihn — der RE2-Gore (Blut 0/0, Brocken 5/7) auf
 * dem RE1.5-Pool ist eine Port-Konstruktion, das Original kennt diese Last nicht. PORT-WAHL: der
 * Wurfkoerper (Spielobjekt) verdraengt das Blut-Teilchen mit dem kleinsten Index (Effekt-Id 0, die
 * Mehrzahl des Gore-Schubs; der Spawner fuellt ab Index 0, also das aelteste). Keine Granate und kein
 * Nicht-Blut-Effekt wird verdraengt; ohne Blut bleibt das Original-Verhalten (kein Platz). */
int re15_granate_r36_platz_freimachen(void)
{
    for (int i = 0; i < RE15_ESP_FX_MAX; i++) {
        const re15_esp_fx_t *f = re15_esp_fx_get(i);
        if (!f || f->granate_art || f->effect_id != 0) continue;
        re15_esp_fx_free_slot(i);
        s_platz_geraeumt++;
        return i;
    }
    return -1;
}

/* Punkt 2 — Spawnbild des Wurfkoerpers 0x040D1000 je Zielhoehe (Substate 2 @0x80033680-7a8):
 *   HOCH  `ori v0,zero,0x13` @0x80033690 + `andi v0,v0,0x8000` @0x800336b4 -> Cursor 19
 *   MITTE `ori v0,zero,0x16` @0x800336a4 + `andi v0,v0,0x4000` @0x80033714 -> Cursor 22
 *   TIEF  `ori v0,zero,0x18` @0x80033758 + `andi v0,v0,0x2000` @0x80033770 -> Cursor 24
 * (game_step_common.c spawnt bei genau diesen Bildern; die Zielhoehe ist im Rueckstoss eingefroren —
 * der Wechsel laeuft nur in Substate 1 @0x80033228-c8.) */
int re15_granate_r36_spawnbild(int elev)
{
    return (elev > 0) ? 19 : (elev < 0) ? 24 : 22;
}

/* Punkt 2 — Abbruch-Schwelle: statt Byte 2 = 10 (@0x800740ba) das Spawnbild der Zielhoehe. Die Pruefung
 * des Aufrufers bleibt `Cursor > Schwelle` (`sltu v0,v0,a0` @0x8003363c): LOWER fruehestens im Bild nach
 * dem Abgang der Granate. NUTZER-VORGABE "Granate wird manchmal nicht geworfen" / Beta -> Retail
 * (RE2-Granatwerfer: Abgang im Rueckstossbild 1, werfer_r35.c). */
int re15_granate_r36_recoil_break(int id, int elev)
{
    if (id == 9 || id == 10 || id == 11)           /* Wurf-Ids: Spawn-Gate `id == 9` @0x8003368c + E1 */
        return re15_granate_r36_spawnbild(elev);
    return -1;                                     /* kein Eingriff */
}
