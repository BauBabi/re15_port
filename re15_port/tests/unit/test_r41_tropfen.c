/*
 * unit_r41_tropfen — Runde 41 Spur W, OFFEN 4 (Variante B, wie die 40%-Disc): In ROOM2000 (Leon) und ROOM2001 (Elza) startet die
 * Deckentropfen-Schleife nicht. Herleitung und Bytes: include/re15_tropfen_r41.h, Dossier analysis/befunde_runde41/W_wasserblink.md OFFEN 4.
 *
 * Echte RDTs (shared_assets/PSX/STAGE2), Raum wie im Spiel hochgefahren (scd_room_reenter = main00 + sub00 + Installer), dann 700 Takte
 * scd_vm_tick + re15_esp_fx_tick. Ein Umlauf der Schleife dauert 585 Bilder, ihre Punkte starten bei t = 0/180/360/540. Geprueft wird:
 *   1  der Haken greift beim Raumstart genau einmal;
 *   2  in keinem Takt ist ein Effekt 6 Sub 0 im Pool (vorher: 3 im ersten Takt, 8 nach t = 540);
 *   3  kein SCD-Faden steht je in sub07 @0x01B06..@0x01BB6 (2000) bzw. sub08 @0x01D32..@0x01DE2 (2001);
 *   4  sonst bleibt alles: die drei Gitterstrahlen 0x0B aus main00 @0x0174C/@0x0175C/@0x0176C (x -23900/-23300/-22700, y -5300,
 *      z 11200) stehen im Pool;
 *   5  Eingrenzung: der Haken antwortet nur auf (Raum, main00, @0x0177C, Bytes, Ziel). Falsch sind: anderer Raum (20B0, 2010, 1140),
 *      2001 mit den 2000-Daten (sub07 statt sub08), andere Stellen (die drei 0x0B, die uebrigen Evt_exec @0x0179C/@0x01A32 bzw.
 *      @0x017C6/@0x0181E/@0x018DA), eine Kopie mit `04 ff 18 06` @0x0177C, eine Kopie mit Effekt 7 statt 6 im Ziel @0x01B08, NULL.
 * Gegenprobe (ohne die Aenderung rot): mit scd_vm.c ohne den Haken-Aufruf schlaegt 1, 2 und 3 fehl (Dossier OFFEN 4, Messung).
 */
#include "re15_rdt.h"
#include "re15_scd.h"
#include "re15_actor.h"
#include "re15_aot.h"
#include "re15_room.h"
#include "re15_msg.h"
#include "re15_inventory.h"
#include "re15_esp.h"
#include "re15_collision.h"
#include "re15_tropfen_r41.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void scd_register_current_rdt(const re15_rdt_t *rdt);

static int s_fail = 0, s_pruef = 0;
#define PRUEFE(cond, ...) do { s_pruef++; if (!(cond)) { printf("FAIL: " __VA_ARGS__); printf("\n"); s_fail = 1; } } while (0)

static uint8_t *rdt_lesen(uint16_t raum, long *sz)
{
    char pfad[256];
    snprintf(pfad, sizeof pfad, RE15_ASSET_PSX_DIR "/STAGE%d/ROOM%04X.RDT", (raum >> 12), raum);
    FILE *f = fopen(pfad, "rb");
    if (!f) return NULL;
    fseek(f, 0, SEEK_END); long n = ftell(f); fseek(f, 0, SEEK_SET);
    uint8_t *d = (uint8_t *)malloc((size_t)n);
    if (!d || fread(d, 1, (size_t)n, f) != (size_t)n) { fclose(f); free(d); return NULL; }
    fclose(f);
    *sz = n;
    return d;
}

static uint8_t   *s_daten = NULL;
static long       s_groesse = 0;
static re15_rdt_t s_rdt;
static re15_esp_t s_esp;

/* Raumstart wie test_r41_ventilkette.c raum_hoch (Tuer-Weg scd_room_reenter), Spieler auf dem Steg bei (-7000,-1800,-25659). */
static int raum_hoch(uint16_t raum)
{
    free(s_daten);
    s_daten = rdt_lesen(raum, &s_groesse);
    if (!s_daten || re15_rdt_parse(s_daten, (size_t)s_groesse, &s_rdt) != 0) return -1;
    re15_actor_init(); re15_aot_init();
    scd_vm_init(); re15_inv_init();
    re15_esp_fx_reset();
    re15_tropfen_r41_reset();
    g_current_room_id = raum;
    scd_register_current_rdt(&s_rdt);
    re15_msg_clear_room_block();
    re15_msg_load_room_block(s_rdt.messages, s_rdt.messages_size);
    { uint32_t idh = 0, pe = 0, tb = 0, te = 0;
      memcpy(&idh, s_daten + 0x4C, 4); memcpy(&pe, s_daten + 0x50, 4); memcpy(&tb, s_daten + 0x54, 4); memcpy(&te, s_daten + 0x58, 4);
      if (re15_esp_parse(s_daten, (size_t)s_groesse, idh, pe, tb, te, &s_esp) == 0) re15_esp_set_room_bank(&s_esp); else re15_esp_set_room_bank(NULL); }
    re15_actor_t *p = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    p->active = 1; p->type = 0; p->hp = 100;
    p->x = -7000; p->y = -1800; p->z = -25659; p->rot_y = 0; p->state = 1;
    re15_collision_reset_band(); re15_collision_set_band(re15_collision_band_from_y(p->y));
    g_scd.player_mode = 0;
    scd_register_room_events(&s_rdt);
    scd_room_reenter(&s_rdt, p->x, p->z, 0);
    return 0;
}

static int tropfen_im_pool(void)
{
    int n = 0;
    for (int i = 0; i < RE15_ESP_FX_MAX; i++) {
        const re15_esp_fx_t *f = re15_esp_fx_get(i);
        if (f && f->active && f->effect_id == 0x06 && f->sub_index == 0) n++;
    }
    return n;
}

static int faden_in(uint32_t von, uint32_t bis)
{
    for (int s = 0; s < SCD_THREAD_COUNT; s++) {
        const scd_thread_t *t = &g_scd.threads[s];
        if (!t->active || !t->pc) continue;
        if (t->pc >= s_daten + von && t->pc < s_daten + bis) return s;
    }
    return -1;
}

static int gitter_strahlen(void)
{
    static const int32_t gx[3] = { -23900, -23300, -22700 };   /* main00 @0x0174C/@0x0175C/@0x0176C: a4 a2 / fc a4 / 54 a7 */
    int gefunden = 0;
    for (int k = 0; k < 3; k++)
        for (int i = 0; i < RE15_ESP_FX_MAX; i++) {
            const re15_esp_fx_t *f = re15_esp_fx_get(i);
            if (f && f->active && f->effect_id == 0x0B && f->x == gx[k] && f->y == -5300 && f->z == 11200) { gefunden++; break; }
        }
    return gefunden;
}

static void raum(uint16_t r, const char *figur, uint32_t sub_von, uint32_t sub_bis)
{
    if (raum_hoch(r) != 0) { PRUEFE(0, "ROOM%04X (%s): RDT nicht geladen", r, figur); return; }
    unsigned z = re15_tropfen_r41_zaehler();
    int max_tropfen = tropfen_im_pool(), faden = faden_in(sub_von, sub_bis), faden_takt = faden >= 0 ? 0 : -1;
    for (int takt = 1; takt <= 700; takt++) {
        scd_vm_tick();
        re15_esp_fx_tick(re15_esp_room_bank());
        int n = tropfen_im_pool();
        if (n > max_tropfen) max_tropfen = n;
        if (faden_takt < 0) { faden = faden_in(sub_von, sub_bis); if (faden >= 0) faden_takt = takt; }
    }
    int gitter = gitter_strahlen();
    PRUEFE(z == 1, "ROOM%04X (%s): Haken beim Raumstart %u-mal (erwartet 1, main00 @0x0177C)", r, figur, z);
    PRUEFE(max_tropfen == 0, "ROOM%04X (%s): Effekt 6 Sub 0 im Pool, hoechstens %d zugleich (erwartet 0)", r, figur, max_tropfen);
    PRUEFE(faden_takt < 0, "ROOM%04X (%s): Faden %d steht ab Takt %d in der Tropfen-Schleife @0x%05X..@0x%05X", r, figur, faden, faden_takt,
           (unsigned)sub_von, (unsigned)sub_bis);
    PRUEFE(gitter == 3, "ROOM%04X (%s): %d von 3 Gitterstrahlen 0x0B (main00 @0x0174C/@0x0175C/@0x0176C)", r, figur, gitter);
    printf("ROOM%04X (%s): Haken %u, Effekt 6 Sub 0 hoechstens %d, Faden in der Schleife %s, Gitterstrahlen %d/3\n",
           r, figur, z, max_tropfen, faden_takt < 0 ? "nie" : "ja", gitter);
}

static void eingrenzung(void)
{
    if (raum_hoch(RE15_R41W_RAUM_2000) != 0) { PRUEFE(0, "ROOM2000: RDT nicht geladen"); return; }
    const uint8_t *d = s_daten;
    re15_tropfen_r41_reset();
    PRUEFE(re15_tropfen_r41_evt_exec_gestrichen(0x2000u, d + 0x0177C, &s_rdt) == 1, "2000 @0x0177C greift nicht");
    PRUEFE(re15_tropfen_r41_evt_exec_gestrichen(0x2001u, d + 0x0177C, &s_rdt) == 0, "2001 mit den 2000-Daten greift (sub07 statt sub08)");
    static const uint16_t fremd[] = { 0x20B0u, 0x20B1u, 0x2010u, 0x1140u, 0x2002u };
    for (unsigned i = 0; i < sizeof fremd / sizeof fremd[0]; i++)
        PRUEFE(re15_tropfen_r41_evt_exec_gestrichen(fremd[i], d + 0x0177C, &s_rdt) == 0, "ROOM%04X greift", fremd[i]);
    static const uint32_t andere[] = { 0x0174C, 0x0175C, 0x0176C, 0x01780, 0x0179C, 0x01A32, 0x0177B, 0x0177D };
    for (unsigned i = 0; i < sizeof andere / sizeof andere[0]; i++)
        PRUEFE(re15_tropfen_r41_evt_exec_gestrichen(0x2000u, d + andere[i], &s_rdt) == 0, "2000 @0x%05X greift", (unsigned)andere[i]);
    PRUEFE(re15_tropfen_r41_evt_exec_gestrichen(0x2000u, NULL, &s_rdt) == 0 && re15_tropfen_r41_evt_exec_gestrichen(0x2000u, d + 0x0177C, NULL) == 0,
           "NULL greift");
    /* Kopien mit einem geaenderten Byte: Sub-Nummer @0x0177F 07 -> 06, Effekt im Ziel @0x01B08 06 -> 07. */
    uint8_t *k = (uint8_t *)malloc((size_t)s_groesse);
    re15_rdt_t kr;
    if (k) {
        memcpy(k, d, (size_t)s_groesse); k[0x0177F] = 0x06;
        PRUEFE(re15_rdt_parse(k, (size_t)s_groesse, &kr) == 0 && re15_tropfen_r41_evt_exec_gestrichen(0x2000u, k + 0x0177C, &kr) == 0,
               "Kopie mit `04 ff 18 06` greift");
        memcpy(k, d, (size_t)s_groesse); k[0x01B08] = 0x07;
        PRUEFE(re15_rdt_parse(k, (size_t)s_groesse, &kr) == 0 && re15_tropfen_r41_evt_exec_gestrichen(0x2000u, k + 0x0177C, &kr) == 0,
               "Kopie mit Effekt 7 im Ziel greift");
        memcpy(k, d, (size_t)s_groesse);
        PRUEFE(re15_rdt_parse(k, (size_t)s_groesse, &kr) == 0 && re15_tropfen_r41_evt_exec_gestrichen(0x2000u, k + 0x0177C, &kr) == 1,
               "unveraenderte Kopie greift nicht");
        free(k);
    }
    if (raum_hoch(RE15_R41W_RAUM_2001) != 0) { PRUEFE(0, "ROOM2001: RDT nicht geladen"); return; }
    d = s_daten;
    PRUEFE(re15_tropfen_r41_evt_exec_gestrichen(0x2001u, d + 0x0177C, &s_rdt) == 1, "2001 @0x0177C greift nicht");
    PRUEFE(re15_tropfen_r41_evt_exec_gestrichen(0x2000u, d + 0x0177C, &s_rdt) == 0, "2000 mit den 2001-Daten greift (sub08 statt sub07)");
    static const uint32_t andere1[] = { 0x0174C, 0x017C6, 0x0181E, 0x018DA };
    for (unsigned i = 0; i < sizeof andere1 / sizeof andere1[0]; i++)
        PRUEFE(re15_tropfen_r41_evt_exec_gestrichen(0x2001u, d + andere1[i], &s_rdt) == 0, "2001 @0x%05X greift", (unsigned)andere1[i]);
    printf("Eingrenzung: nur (2000|2001, main00 @0x0177C, `04 ff 18 07|08`, Ziel `3a 00 06 00 00 00 00 10`)\n");
}

int main(void)
{
    raum(RE15_R41W_RAUM_2000, "Leon", RE15_R41W_SUB_OFF_2000, 0x01BB6u);   /* sub07 @0x01B06..@0x01BB4 `01 00` */
    raum(RE15_R41W_RAUM_2001, "Elza", RE15_R41W_SUB_OFF_2001, 0x01DE2u);   /* sub08 @0x01D32, 0xB0 Bytes wie sub07 */
    eingrenzung();
    printf("unit_r41_tropfen: %d Pruefungen, %s\n", s_pruef, s_fail ? "FEHLER" : "OK");
    free(s_daten);
    return s_fail;
}
