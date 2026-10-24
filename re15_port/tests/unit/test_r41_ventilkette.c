/*
 * unit_r41_ventilkette — Runde 41 Spur C "ventilkette": die STAGE2-Kette, Glied fuer Glied gemessen.
 * Herleitung: include/re15_ventilkette_r41.h, Dossier analysis/befunde_runde41/C_ventilkette.md.
 *
 * ECHTER WEG je Raum (Leon + Elza): RDT laden, Raum wie im Spiel hochfahren (scd_room_reenter = Installer-Block),
 * Spieler vor die Stelle, Blick hin, Aktion -> re15_aot_scan (Vorwaertspunkt 620) -> scd_event_fire -> Programm;
 * je Bild scd_vm_tick + re15_ventilkette_r41_tick (der Spiel-Takt-Haken). Geprueft:
 *   A  Original-Scheiben Byte fuer Byte gegen die RDTs; Port-Texte dekodiert; 2080-Programm = sub02 + Schwanz
 *   B  2000/2001: Leiter Slot 4 = Sperrtext 40 (Aktion zeigt ihn), mit Bit 100 = Tuer
 *   C  2030/2031: Bruecke oben + Zelle 12 zu; Pult "kein Druck"; Ventil 13 (Frage, Ja, Bit 101, Klick-Zustand);
 *      Pult "Lower the bridge?" -> Fahrt 30 x 120 unter Cut 4 -> y -1800, Bit 102, Zelle 12 offen; mit 102 unten
 *   D  Ventile: Frage, Warnung, Zaehler (5,0x1D) -> Ost-Tuer -> 20B0, Wasser geparkt; 1800 Takte -> Flut (5,0x1E),
 *      Wasser steigt 50/Takt, Spieler ertrinkt (hp -1, Blende), Zeit abgelaufen = tot
 *   E  Verlassen waehrend des Zaehlers -> Bits 103 + 100; 20B0: Leiter = Tuer, Strahlen unterdrueckt (0 statt 3);
 *      Tueren nach 2000 -> 20B0 mit Cut-Zuordnung; 2030 nach der Flut unter Wasser, Lift-Rufzone = msg 1; mit 107 leer,
 *      Ventile danach = Zustandssatz 49 (Nachbesserung 1, M8)
 *   F  2050/2051: Hebel ohne Strom = msg 40, (3,104) bleibt; Pult: Slot 8 + Cursor, Frage, Cut 15, Cursor faehrt
 *      (Original sub07), SQUARE kippt Hebel (Zonen), 30+40+10 = 80 -> 30 Ruhebilder -> Bit 106, "Power supply OK.",
 *      Ausstieg sub10; danach Hebel = Original sub04 ((3,104)=1); alle 16 Masken: nur 0xD = 80;
 *      Kisten im Becken (sub01 @0x0201A -> Ereignis 2 = sub02 @0x0220E): ohne Bit 106 Sperrsatz 40, Wasser bleibt;
 *      mit Bit 106 Original sub02 (Wasser steigt) (Nachbesserung 1, M1)
 *   G  2080/2081: ohne Flut Original; mit Flut: sub02 + Set(9,107) + msg 40, Wasser obj 0 -> -3420; war der Rechner
 *      schon gelaufen: Slot 4 erneut, nur Frage + Bit 107
 *   H  fremde Raeume: der Installer laesst (5,0x1D)/(5,0x1E) stehen (ROOM1171 sub02 @0x013B2 `22 05 1e 01` im
 *      Init-Takt bleibt 1), nur 2030/2031 loescht er (Nachbesserung 1, M3)
 *   I  Ring-Zugaenge des gefluteten 2030 (Orchestrator-Entscheid C-E2, Abnahme 0 M5): 20B0/20B1 Slot 3 @0x019C2 und
 *      2070/2071 Slot 1 @0x00A5A / Slot 2 @0x00A7A sind mit Bit 103 ohne 107 Sperrtexte (41 bzw. 40, Aktion zeigt ihn,
 *      kein Raumwechsel), mit 107 wieder Tueren nach 2030; die Tuerebenen-Tueren (2080 Slot 0 @0x00BA2) bleiben offen
 *
 * Werkzeug (Riegel integration_r41_ventilkette C, Nachbesserung 1 M4): `test_r41_ventilkette ppm <datei.ppm> x0 y0 x1 y1`
 * gibt "MITTEL r g b" der Flaeche (P6-Framedump, x1/y1 ausschliesslich) aus.
 */
#include "re15_rdt.h"
#include "re15_scd.h"
#include "re15_actor.h"
#include "re15_aot.h"
#include "re15_room.h"
#include "re15_msg.h"
#include "re15_inventory.h"
#include "re15_esp.h"
#include "re15_fade.h"
#include "re15_collision.h"
#include "re15_ventilkette_r41.h"
#include "gen/ventilkette_r41_original.inc"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void scd_register_current_rdt(const re15_rdt_t *rdt);
extern uint16_t g_scd_pad_edge;
extern uint16_t g_scd_pad_held;

static int s_fail = 0, s_pruef = 0;
#define PRUEFE(cond, ...) do { s_pruef++; if (!(cond)) { printf("FAIL: " __VA_ARGS__); printf("\n"); s_fail = 1; } } while (0)

static uint8_t *slurp(const char *path, long *out_sz)
{
    FILE *f = fopen(path, "rb");
    if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    uint8_t *d = (uint8_t *)malloc((size_t)sz);
    if (!d || fread(d, 1, (size_t)sz, f) != (size_t)sz) { fclose(f); free(d); return NULL; }
    fclose(f);
    *out_sz = sz;
    return d;
}

static uint8_t *s_daten = NULL;
static re15_rdt_t s_rdt;
static re15_esp_t s_esp;

/* Spielbits, die VOR dem Raumstart gelten sollen: scd_vm_init() loescht alle Flags (re15_game_state_init,
 * scd_vm.c:566), deshalb werden sie nach den Inits und vor scd_room_reenter gesetzt (wie ein Spielstand). */
static struct { uint8_t zone, idx; } s_vf[16];
static int s_nvf = 0;
static void vorflag(uint8_t zone, uint8_t idx) { if (s_nvf < 16) { s_vf[s_nvf].zone = zone; s_vf[s_nvf].idx = idx; s_nvf++; } }

static uint8_t *rdt_lesen(uint16_t raum, long *sz)
{
    char pfad[256];
    snprintf(pfad, sizeof pfad, RE15_ASSET_PSX_DIR "/STAGE%d/ROOM%04X.RDT", (raum >> 12), raum);
    return slurp(pfad, sz);
}

static void band_aus_y(int32_t y) { re15_collision_set_band(re15_collision_band_from_y(y)); }

static int raum_hoch(uint16_t raum, int32_t x, int32_t y, int32_t z)
{
    long sz = 0;
    free(s_daten);
    s_daten = rdt_lesen(raum, &sz);
    if (!s_daten || re15_rdt_parse(s_daten, (size_t)sz, &s_rdt) != 0) return -1;
    re15_actor_init(); re15_aot_init();
    scd_vm_init(); re15_inv_init();
    for (int i = 0; i < s_nvf; i++) re15_game_flag_set(s_vf[i].zone, s_vf[i].idx, 1);
    memset(&g_room_change, 0, sizeof g_room_change);
    g_current_room_id = raum;
    scd_register_current_rdt(&s_rdt);
    re15_msg_clear_room_block();
    re15_msg_load_room_block(s_rdt.messages, s_rdt.messages_size);
    /* Effekt-Bank des Raums wie main.c (RDT+0x4C/0x50/0x54/0x58), damit Sce_espr_on Strahlen anlegt. */
    { uint32_t idh = 0, pe = 0, tb = 0, te = 0;
      memcpy(&idh, s_daten + 0x4C, 4); memcpy(&pe, s_daten + 0x50, 4); memcpy(&tb, s_daten + 0x54, 4); memcpy(&te, s_daten + 0x58, 4);
      if (re15_esp_parse(s_daten, (size_t)sz, idh, pe, tb, te, &s_esp) == 0) re15_esp_set_room_bank(&s_esp); else re15_esp_set_room_bank(NULL); }
    re15_actor_t *p = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    p->active = 1; p->type = 0; p->hp = 100;
    p->x = x; p->y = y; p->z = z; p->rot_y = 0; p->state = 1;
    re15_collision_reset_band(); band_aus_y(y);
    g_scd.player_mode = 0;
    scd_register_room_events(&s_rdt);
    scd_room_reenter(&s_rdt, p->x, p->z, 0);
    for (int i = 0; i < 60; i++) { scd_vm_tick(); re15_ventilkette_r41_tick(); re15_aot_scan(p->x, p->z, 0xFF); (void)re15_cam_present_tick(); }
    { scd_audio_event_t ev; while (scd_audio_queue_pop(&ev)) {} }
    return 0;
}

static void takt(void)
{
    re15_actor_t *p = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    re15_msg_tick(0, 0, 0);
    scd_vm_tick();
    re15_ventilkette_r41_tick();
    re15_aot_scan(p->x, p->z, 0xFF);
    (void)re15_cam_present_tick();
}

typedef struct {
    int msgs[8], n_msgs;
    int cut_bei[8];
    int cuts[16], n_cuts;
    int cut_vorher;
    int takte;
} lauf_t;

/* Spieler an (x,z,rot), Aktion, dann Bilder bis kein Faden mehr laeuft (max_takte). Bei einer Frage: nein = einmal
 * umschalten (0x1000) vor dem Bestaetigen. Rot: 0 = +x, 1024 = -z, 2048 = -x, 3072 = +z (Vorwaertspunkt 620). */
static lauf_t druecken(int32_t x, int32_t z, int rot, int nein, int max_takte)
{
    lauf_t L; memset(&L, 0, sizeof L);
    re15_actor_t *p = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    p->x = x; p->z = z; p->rot_y = (int16_t)rot;
    band_aus_y(p->y);   /* Band-Gate der Aktion (FUN_80042cac): Spielerband = Band der Stelle */
    L.cut_vorher = g_scd.work_vars[0x0A];
    L.cuts[L.n_cuts++] = L.cut_vorher;
    g_aot.fired_event_id_this_frame = 0;
    g_aot_action_pressed = 1;
    re15_aot_scan(p->x, p->z, 0xFF);
    g_aot_action_pressed = 0;
    if (g_aot.fired_event_id_this_frame) (void)scd_event_fire(g_aot.fired_event_id_this_frame);
    int offen = -1, umgeschaltet = 0;
    for (int i = 0; i < max_takte; i++) {
        g_scd_pad_edge = 0; g_scd_pad_held = 0;
        if (g_scd.message_active) {
            if (g_scd.message_id != offen && L.n_msgs < 8) {
                offen = g_scd.message_id;
                L.cut_bei[L.n_msgs] = g_scd.work_vars[0x0A];
                L.msgs[L.n_msgs++] = offen;
            }
            if ((i & 7) == 7 && (g_scd.message_fsm == 1 || g_scd.message_fsm == 3 ||
                                 g_scd.message_fsm == 4 || g_scd.message_fsm == 7)) {
                /* "Nein" nur in der Auswahl (fsm 3 = SELECT); bei zweiseitigen Fragen kommt davor PAGE_WAIT (1) */
                if (nein && !umgeschaltet && g_scd.message_fsm == 3) { g_scd_pad_edge = 0x1000; umgeschaltet = 1; }
                else g_scd_pad_edge = 0x4000;
            }
        } else offen = -1;
        takt();
        L.takte = i + 1;
        g_scd_pad_edge = 0;
        if (L.n_cuts < 16 && g_scd.work_vars[0x0A] != L.cuts[L.n_cuts - 1]) L.cuts[L.n_cuts++] = g_scd.work_vars[0x0A];
        int laeuft = g_scd.message_active;
        for (int k = SCD_EVENT_SLOT_FIRST; k <= SCD_EVENT_SLOT_LAST; k++)
            if (g_scd.threads[k].active) laeuft = 1;
        if (!laeuft && i > 30) break;
    }
    { scd_audio_event_t ev; while (scd_audio_queue_pop(&ev)) {} }
    return L;
}

static void zeige(const char *was, uint16_t raum, const lauf_t *L)
{
    printf("  ROOM%04X %-10s msgs:", raum, was);
    for (int i = 0; i < L->n_msgs; i++) printf(" %d@Cut%d", L->msgs[i], L->cut_bei[i]);
    printf("  Cuts:");
    for (int i = 0; i < L->n_cuts; i++) printf(" %d", L->cuts[i]);
    printf("  Takte %d Ende cam=%d (2,7)=%d\n", L->takte, g_scd.cam_id, re15_game_flag_get(2, 7));
}

static int bit(int b) { return re15_game_flag_get(RE15_R41C_BANK, (uint8_t)b) ? 1 : 0; }
static void bits_leeren(void) { re15_game_state_init(); re15_ventilkette_r41_reset(); s_nvf = 0; }

static int prop_von_obj(int obj)
{
    for (int k = 0; k < (int)g_scd.prop_count; k++)
        if (g_scd.props[k].active && g_scd.props[k].obj_id == obj) return k;
    return -1;
}

static int zelle12_u0(int region)
{
    int base = 0;
    for (int r = 0; r < region; r++) base += s_rdt.sca_rgn[r];
    return s_rdt.sca[base + RE15_R41C_STEG_ZELLE].u0;
}

static int text_ist(uint16_t raum, int id, const char *soll)
{
    char out[256];
    int n = 0;
    const uint8_t *b = re15_ventilkette_r41_text(raum, (uint8_t)id, &n);
    if (!b) return 0;
    re15_msg_decode_text(b, (size_t)n, out, sizeof out);
    if (strcmp(out, soll) != 0) printf("    Text %d: \"%s\" != \"%s\"\n", id, out, soll);
    return strcmp(out, soll) == 0;
}

static int strahlen_zaehlen(void)
{
    int n = 0;
    for (int i = 0; i < RE15_ESP_FX_MAX; i++) {
        const re15_esp_fx_t *f = re15_esp_fx_get(i);
        if (f && f->active && f->effect_id == 0x0b) n++;
    }
    return n;
}

/* ---- A: Bytes und Texte ----------------------------------------------------------------------------------- */
static void fall_a(void)
{
    struct { uint16_t raum; uint32_t off; const unsigned char *b; int n; const char *was; } k[] = {
        { 0x2080, RE15_R41C_ORG_2080_SUB02_OFF, re15_r41c_org_2080_sub02, (int)sizeof re15_r41c_org_2080_sub02, "2080 sub02" },
        { 0x2050, RE15_R41C_ORG_2050_SUB09_OFF, re15_r41c_org_2050_sub09, (int)sizeof re15_r41c_org_2050_sub09, "2050 sub09" },
        { 0x2050, RE15_R41C_ORG_2050_SLOT8_OFF, re15_r41c_org_2050_slot8, (int)sizeof re15_r41c_org_2050_slot8, "2050 Slot 8" },
        { 0x2050, RE15_R41C_ORG_2050_OBJ8_OFF,  re15_r41c_org_2050_obj8,  (int)sizeof re15_r41c_org_2050_obj8,  "2050 obj 8" },
        { 0x2050, RE15_R41C_ORG_2050_KISTEN_OFF, re15_r41c_org_2050_kisten, (int)sizeof re15_r41c_org_2050_kisten, "2050 sub01 Kisten -> Ereignis 2" },
        { 0x2000, RE15_R41C_ORG_2000_SLOT4_OFF, re15_r41c_org_2000_slot4, (int)sizeof re15_r41c_org_2000_slot4, "2000 Slot 4" },
        { 0x2030, RE15_R41C_ORG_2030_OBJ0_OFF,  re15_r41c_org_2030_obj0,  (int)sizeof re15_r41c_org_2030_obj0,  "2030 obj 0" },
        { 0x2080, RE15_R41C_ORG_2080_SLOT4_OFF, re15_r41c_org_2080_slot4, (int)sizeof re15_r41c_org_2080_slot4, "2080 Slot 4" },
        { 0x11F0, RE15_R41C_ORG_11F0_MSG1_OFF,  re15_r41c_org_11f0_msg1,  (int)sizeof re15_r41c_org_11f0_msg1,  "11F0 msg 1" },
        { 0x11F0, RE15_R41C_ORG_11F0_MSG2_OFF,  re15_r41c_org_11f0_msg2,  (int)sizeof re15_r41c_org_11f0_msg2,  "11F0 msg 2" },
    };
    for (unsigned i = 0; i < sizeof k / sizeof k[0]; i++) {
        for (int v = 0; v < 2; v++) {
            long sz = 0;
            uint8_t *d = rdt_lesen((uint16_t)(k[i].raum + v), &sz);
            PRUEFE(d && (long)k[i].off + k[i].n <= sz && memcmp(d + k[i].off, k[i].b, (size_t)k[i].n) == 0,
                   "ROOM%04X %s @0x%05X: Scheibe != RDT", k[i].raum + v, k[i].was, k[i].off);
            free(d);
        }
    }
    /* 2080-Programm = sub02 Byte fuer Byte mit 10-Byte-Schwanz im Ja-Zweig */
    int n = 0;
    const uint8_t *p = re15_ventilkette_r41_programm("2080_rechner_erw", &n);
    PRUEFE(p && n == 118 && memcmp(p, re15_r41c_org_2080_sub02, 6) == 0 &&
           memcmp(p + 10, re15_r41c_org_2080_sub02 + 10, 0x5E) == 0 && p[8] == 0x6A &&
           memcmp(p + 0x68, "\x22\x09\x6b\x01\x2b\x28\xff\xff\x02\x00\x08\x00\x01\x00", 14) == 0,
           "2080_rechner_erw: nicht sub02 @0x00C86 + Schwanz (n=%d, Ifel %02x)", n, p ? p[8] : 0);
    /* Texte (Port-Ids, Glyphen des Ports) */
    PRUEFE(text_ist(0x2000, 40, "I cannot climb up the ladder because of the water pressure."), "Text 2000/40");
    PRUEFE(text_ist(0x20B1, 40, "I cannot climb up the ladder because of the water pressure."), "Text 20B1/40 (= 2000)");
    PRUEFE(text_ist(0x2030, 43, "The pressure gauge reads 80. Lower the bridge?"), "Text 2030/43");
    PRUEFE(text_ist(0x2030, 46, "WARNING: The water will rise in one minute. Leave the room!"), "Text 2030/46");
    PRUEFE(text_ist(0x2050, 42, "Power supply OK."), "Text 2050/42 = 11F0 msg 2");
    PRUEFE(text_ist(0x2050, 41, "Will you push them?"), "Text 2050/41 = 11F0 msg 1");
    PRUEFE(text_ist(0x2080, 40, "The treatment pool is being drained as well."), "Text 2080/40");
    PRUEFE(text_ist(0x2031, 49, "The valves are open. The pool has been drained."), "Text 2031/49 (Nachbesserung 1, M8)");
    PRUEFE(text_ist(0x20B1, 41, "I cannot open the door because of the water pressure."), "Text 20B1/41 (C-E2)");
    PRUEFE(text_ist(0x2071, 40, "I cannot open the door because of the water pressure."), "Text 2071/40 (C-E2)");
    PRUEFE(re15_ventilkette_r41_text(0x2000, 41, NULL) == NULL && text_ist(0x20B0, 40, "I cannot climb up the ladder because of the water pressure."),
           "Id 41 nur in 20B0 (VERTRAG §1.2), 20B0 traegt weiter den Leiter-Satz 40");
    /* Gewichte: genau eine der 16 Masken ergibt 80 */
    int treffer = 0, welche = -1;
    for (unsigned m = 0; m < 16; m++)
        if (re15_ventilkette_r41_spannung_aus_maske(m) == RE15_R41C_PANEL_ZIEL) { treffer++; welche = (int)m; }
    PRUEFE(treffer == 1 && welche == (int)RE15_R41C_PANEL_LOESUNG, "Gewichte: %d Masken ergeben 80 (0x%X)", treffer, welche);
    /* Cut-Zuordnung 2000 -> 20B0 */
    PRUEFE(re15_ventilkette_r41_cut_20b0(14) == 10 && re15_ventilkette_r41_cut_20b0(10) == 9 &&
           re15_ventilkette_r41_cut_20b0(0) == 0 && re15_ventilkette_r41_cut_20b0(4) == 4 && re15_ventilkette_r41_cut_20b0(13) == 7,
           "Cut-Zuordnung 2000 -> 20B0");
}

/* ---- B: Leiter ---------------------------------------------------------------------------------------------- */
static void fall_b(uint16_t r2000, uint16_t r20b0)
{
    bits_leeren();
    if (raum_hoch(r2000, -25950, -1800, 10300) != 0) { PRUEFE(0, "ROOM%04X Parse", r2000); return; }
    const re15_aot_t *a = &g_aot.slots[RE15_R41C_SLOT_2000_LEITER];
    PRUEFE(a->active && a->type == RE15_AOT_TYPE_MESSAGE && a->event_id == RE15_R41C_MSG_LEITER,
           "ROOM%04X: Leiter Slot 4 aktiv=%d Typ=%d msg=%d (erwartet Text 40)", r2000, a->active, a->type, a->event_id);
    /* Leiter-Rechteck x -27600..-26600 (Slot 4 @0x0169E, Mitte -27100 +- 500), Spieler bei x -26000 Blick -x:
     * Vorwaertspunkt -26620 */
    lauf_t L = druecken(-26000, 10300, 2048, 0, 400); zeige("leiter", r2000, &L);
    PRUEFE(L.n_msgs >= 1 && L.msgs[0] == RE15_R41C_MSG_LEITER, "ROOM%04X: Aktion an der Leiter zeigt Text 40", r2000);
    PRUEFE(!g_room_change.pending, "ROOM%04X: kein Raumwechsel ausgeloest", r2000);
    /* mit Bit 100: Tuer */
    bits_leeren(); vorflag(RE15_R41C_BANK, RE15_R41C_BIT_LEITER_FREI);
    if (raum_hoch(r2000, -25950, -1800, 10300) != 0) { PRUEFE(0, "Parse"); return; }
    PRUEFE(g_aot.slots[4].type == RE15_AOT_TYPE_DOOR && g_aot.door_params[4].dest_room == 5,
           "ROOM%04X mit Bit 100: Leiter ist Tuer nach ROOM2050 (Typ %d, Ziel %d)", r2000, g_aot.slots[4].type, g_aot.door_params[4].dest_room);
    /* 20B0 ohne Bit 100 (Debug-Sprung): ebenfalls Sperrtext; mit (3,235)=1 die drei Strahlen (Original) */
    bits_leeren(); vorflag(3, 235);
    if (raum_hoch(r20b0, -25950, -1800, 10300) != 0) { PRUEFE(0, "ROOM%04X Parse", r20b0); return; }
    PRUEFE(g_aot.slots[4].type == RE15_AOT_TYPE_MESSAGE && g_aot.slots[4].event_id == RE15_R41C_MSG_LEITER,
           "ROOM%04X ohne Bit 100: Leiter = Text 40", r20b0);
    int n_ohne = strahlen_zaehlen();
    /* drei Sce_espr_on @0x01E96/@0x01EA6/@0x01EB6 zu je 6 Stroemen (re15_esp.c, Effekt 0x0b) = 18 Effektplaetze */
    PRUEFE(n_ohne == 18, "ROOM%04X (3,235)=1 ohne Bit 100: %d Strahlen-Plaetze (Original 3 x 6)", r20b0, n_ohne);
}

/* ---- C: Bruecke, Pult, Ventil 13 --------------------------------------------------------------------------- */
static void fall_c(uint16_t r)
{
    bits_leeren();
    if (raum_hoch(r, 350, -5400, -9500) != 0) { PRUEFE(0, "ROOM%04X Parse", r); return; }
    int pb = prop_von_obj(RE15_R41C_BRUECKE_OBJ);
    PRUEFE(pb >= 0 && g_scd.props[pb].y == RE15_R41C_BRUECKE_Y_OBEN, "ROOM%04X: Bruecke oben (y %d)", r, pb >= 0 ? (int)g_scd.props[pb].y : 0);
    for (int rg = 0; rg < 5; rg++) PRUEFE(zelle12_u0(rg) == 0xFF, "ROOM%04X: Zelle 12 Region %d gesperrt (%02x)", r, rg, zelle12_u0(rg));
    const int slots[3] = { RE15_R41C_SLOT_PULT, RE15_R41C_SLOT_VENTIL13, RE15_R41C_SLOT_VENTILE };
    const int evs[3] = { RE15_R41C_EV_PULT, RE15_R41C_EV_VENTIL13, RE15_R41C_EV_VENTILE };
    for (int i = 0; i < 3; i++) {
        const re15_aot_t *a = &g_aot.slots[slots[i]];
        PRUEFE(a->active && a->type == RE15_AOT_TYPE_GENERIC && a->event_id == evs[i] && (a->sce_flags & 0x7f) == 0x31,
               "ROOM%04X: Platz %d aktiv=%d Typ=%d Ereignis=%d sat=%02x", r, slots[i], a->active, a->type, a->event_id, a->sce_flags);
    }
    /* Pult ohne Druck (Nordstreifen, Blick -z) */
    lauf_t L = druecken(350, -9500, 1024, 0, 400); zeige("pult", r, &L);
    PRUEFE(L.n_msgs == 1 && L.msgs[0] == RE15_R41C_MSG_PULT_OHNE, "ROOM%04X Pult ohne Druck: msg 40", r);
    PRUEFE(prop_von_obj(0) >= 0 && g_scd.props[prop_von_obj(0)].y == RE15_R41C_BRUECKE_Y_OBEN, "ROOM%04X: Bruecke bleibt oben", r);
    /* Ventil 13 (Landeplatz Band 1, Blick -x): Nein, dann Ja */
    g_actors[RE15_ACTOR_SLOT_PLAYER].y = -1800;
    L = druecken(-1500, -6400, 2048, 1, 400); zeige("v13 nein", r, &L);
    PRUEFE(L.n_msgs == 1 && L.msgs[0] == RE15_R41C_MSG_V13_FRAGE && bit(RE15_R41C_BIT_VENTIL13) == 0, "ROOM%04X Ventil 13 Nein: nur Frage, Bit 101 = 0", r);
    L = druecken(-1500, -6400, 2048, 0, 400); zeige("v13 ja", r, &L);
    PRUEFE(L.n_msgs == 2 && L.msgs[0] == RE15_R41C_MSG_V13_FRAGE && L.msgs[1] == RE15_R41C_MSG_V13_OFFEN && bit(RE15_R41C_BIT_VENTIL13) == 1,
           "ROOM%04X Ventil 13 Ja: Frage + \"opened\", Bit 101 = 1 (msgs %d)", r, L.n_msgs);
    L = druecken(-1500, -6400, 2048, 0, 400); zeige("v13 zust", r, &L);
    PRUEFE(L.n_msgs == 1 && L.msgs[0] == RE15_R41C_MSG_V13_ZUSTAND, "ROOM%04X Ventil 13 danach: Zustandssatz 47", r);
    /* Pult: Lower the bridge? Nein -> nichts; Ja -> Fahrt */
    g_actors[RE15_ACTOR_SLOT_PLAYER].y = -5400;
    L = druecken(350, -9500, 1024, 1, 400); zeige("pult nein", r, &L);
    PRUEFE(L.n_msgs == 1 && L.msgs[0] == RE15_R41C_MSG_BRUECKE_FRAGE && bit(RE15_R41C_BIT_BRUECKE_UNTEN) == 0 &&
           g_scd.props[pb].y == RE15_R41C_BRUECKE_Y_OBEN, "ROOM%04X Pult Nein: Frage 43, Bruecke bleibt oben", r);
    L = druecken(350, -9500, 1024, 0, 600); zeige("pult ja", r, &L);
    int cut4 = 0; for (int i = 0; i < L.n_cuts; i++) if (L.cuts[i] == RE15_R41C_FAHRT_CUT) cut4 = 1;
    PRUEFE(L.n_msgs == 1 && L.msgs[0] == RE15_R41C_MSG_BRUECKE_FRAGE, "ROOM%04X Pult Ja: Frage 43", r);
    PRUEFE(g_scd.props[pb].y == RE15_R41C_BRUECKE_Y_UNTEN, "ROOM%04X Pult Ja: Bruecke unten y %d (erwartet -1800)", r, (int)g_scd.props[pb].y);
    PRUEFE(bit(RE15_R41C_BIT_BRUECKE_UNTEN) == 1, "ROOM%04X Pult Ja: Bit 102", r);
    PRUEFE(cut4, "ROOM%04X Pult Ja: Fahrt unter Cut 4 (Draufsicht)", r);
    for (int rg = 0; rg < 5; rg++) PRUEFE(zelle12_u0(rg) == 0x00, "ROOM%04X: Zelle 12 Region %d offen (%02x)", r, rg, zelle12_u0(rg));
    PRUEFE(g_scd.cam_id == L.cut_vorher && g_scd.cut_auto_enabled == 1 && re15_game_flag_get(2, 7) == 0,
           "ROOM%04X Pult Ja: zurueck in Cut %d (cam %d), auto %d, (2,7)=%d", r, L.cut_vorher, g_scd.cam_id, g_scd.cut_auto_enabled, re15_game_flag_get(2, 7));
    PRUEFE(L.takte >= 60 && L.takte <= 200, "ROOM%04X Pult Ja: Fahrt %d Takte (30 Schritte x 2 Evt_next + Sleep 20)", r, L.takte);
    L = druecken(350, -9500, 1024, 0, 400); zeige("pult unten", r, &L);
    PRUEFE(L.n_msgs == 1 && L.msgs[0] == RE15_R41C_MSG_BRUECKE_UNTEN, "ROOM%04X Pult danach: Zustandssatz 44", r);
    /* Neustart des Raums mit Bit 102 (wie ein Spielstand): unten, Zelle offen */
    vorflag(RE15_R41C_BANK, RE15_R41C_BIT_VENTIL13); vorflag(RE15_R41C_BANK, RE15_R41C_BIT_BRUECKE_UNTEN);
    if (raum_hoch(r, 350, -5400, -9500) != 0) { PRUEFE(0, "Parse"); return; }
    pb = prop_von_obj(0);
    PRUEFE(pb >= 0 && g_scd.props[pb].y == RE15_R41C_BRUECKE_Y_UNTEN && zelle12_u0(0) == 0x00 && zelle12_u0(4) == 0x00,
           "ROOM%04X mit Bit 102: Bruecke unten, Zelle 12 offen", r);
}

/* ---- D: Ventile, Zaehler, Flut, Ertrinken ------------------------------------------------------------------ */
static void fall_d(uint16_t r)
{
    bits_leeren();
    vorflag(RE15_R41C_BANK, RE15_R41C_BIT_VENTIL13);
    vorflag(RE15_R41C_BANK, RE15_R41C_BIT_BRUECKE_UNTEN);
    if (raum_hoch(r, 6500, -5400, -200) != 0) { PRUEFE(0, "ROOM%04X Parse", r); return; }
    lauf_t L = druecken(6500, -200, 2048, 1, 400); zeige("ventile nein", r, &L);
    PRUEFE(L.n_msgs == 1 && L.msgs[0] == RE15_R41C_MSG_VENTILE_FRAGE && !re15_ventilkette_r41_timer_laeuft() && !g_scd.message_active,
           "ROOM%04X Ventile Nein: nur Frage, kein Zaehler (msgs %d, timer %d)", r, L.n_msgs, re15_ventilkette_r41_timer_laeuft());
    /* Ja: Frage + Warnung, dann schlaeft der Faden (druecken bricht nach 600 Takten ab) */
    L = druecken(6500, -200, 2048, 0, 600); zeige("ventile ja", r, &L);
    PRUEFE(L.n_msgs == 2 && L.msgs[0] == RE15_R41C_MSG_VENTILE_FRAGE && L.msgs[1] == RE15_R41C_MSG_WARNUNG,
           "ROOM%04X Ventile Ja: Frage 45 + Warnung 46 (msgs %d)", r, L.n_msgs);
    PRUEFE(re15_ventilkette_r41_timer_laeuft() && re15_game_flag_get(5, RE15_R41C_FLAG_TIMER) == 1, "ROOM%04X: Zaehler laeuft ((5,0x3D)=1)", r);
    PRUEFE(re15_game_flag_get(2, 7) == 0 && g_scd.message_active == 0, "ROOM%04X: Spieler frei waehrend des Zaehlers", r);
    const re15_aot_door_params_t *d = &g_aot.door_params[RE15_R41C_SLOT_2030_OSTTUER];
    PRUEFE(g_aot.slots[0].type == RE15_AOT_TYPE_DOOR && d->dest_room == 0x0B && d->dest_stage == 1 && d->target_cut == 11 &&
           d->spawn_x == 5950 && d->spawn_z == -6400, "ROOM%04X: Ost-Tuer -> ROOM20B0 Cut 11 (Ziel %d, Cut %d)", r, d->dest_room, d->target_cut);
    int pa = re15_ventilkette_r41_wasser_prop(0), pw = re15_ventilkette_r41_wasser_prop(1);
    PRUEFE(pa >= 0 && pw >= 0 && g_scd.props[pa].z == RE15_R41C_WASSER_PARK_Z && g_scd.props[pa].obj_type == 3,
           "ROOM%04X: Wasserflaechen angelegt und geparkt (z %d)", r, pa >= 0 ? (int)g_scd.props[pa].z : 0);
    PRUEFE(re15_ventilkette_r41_programm("2030_ventile", NULL) != NULL, "Programm 2030_ventile");
    /* Zaehler bis zum Ende: 1800 Takte Sleep (600 liefen schon in druecken) */
    int takte = 0, flut_bei = -1, tot_bei = -1, spritz_bei = -1;
    int32_t wy_vorher = 0, wy_tot = 0, wy_spritz = 0;
    PRUEFE(re15_ventilkette_r41_spritzer() == 0, "ROOM%04X: vor der Flut kein Spritzer", r);
    for (int i = 0; i < 2800; i++) {
        g_scd_pad_edge = 0; g_scd_pad_held = 0;
        if (g_scd.message_active && (i & 7) == 7) g_scd_pad_edge = 0x4000;
        takt(); takte++;
        g_scd_pad_edge = 0;
        if (flut_bei < 0 && re15_ventilkette_r41_flut_laeuft()) { flut_bei = takte; wy_vorher = g_scd.props[pa].y; }
        if (tot_bei < 0 && g_actors[RE15_ACTOR_SLOT_PLAYER].hp < 0) { tot_bei = takte; wy_tot = g_scd.props[pa].y; }
        if (spritz_bei < 0 && re15_ventilkette_r41_spritzer() > 0) { spritz_bei = takte; wy_spritz = g_scd.props[pa].y; }
        if (flut_bei > 0 && takte == flut_bei + 10) {
            PRUEFE(g_scd.props[pa].y == wy_vorher - 10 * RE15_R41C_WASSER_SCHRITT || g_scd.props[pa].y < wy_vorher,
                   "ROOM%04X: Wasser steigt 50/Takt (y %d -> %d nach 10 Takten)", r, (int)wy_vorher, (int)g_scd.props[pa].y);
        }
        if (tot_bei > 0 && takte > tot_bei + 400) break;
    }
    printf("  ROOM%04X Flut bei Takt %d (nach druecken +600), tot bei %d, Wasser y %d, Spieler y %d\n", r, flut_bei, tot_bei,
           pa >= 0 ? (int)g_scd.props[pa].y : 0, (int)g_actors[RE15_ACTOR_SLOT_PLAYER].y);
    /* 1800 Takte Sleep ab dem Schliessen der Warnung; davor Frage (2 Seiten) + Warnung im druecken-Lauf (je 8 Takte
     * Tastentakt + Schreibmaschine), gemessen ~300 Takte -> Flut zwischen 1800 und 1800 + 450 nach der Aktion */
    PRUEFE(flut_bei > 0 && flut_bei + 600 >= RE15_R41C_ZAEHLER_BILDER && flut_bei + 600 <= RE15_R41C_ZAEHLER_BILDER + 450,
           "ROOM%04X: Flut nach 1800 Takten Sleep (gemessen %d + 600 nach der Aktion)", r, flut_bei);
    PRUEFE(re15_game_flag_get(2, 7) == 1 || tot_bei > 0, "ROOM%04X: Pad-Sperre waehrend der Flut", r);
    PRUEFE(g_scd.cam_id == RE15_R41C_FAHRT_CUT, "ROOM%04X: Flut unter Cut 4 (cam %d)", r, g_scd.cam_id);
    PRUEFE(tot_bei > 0 && re15_ventilkette_r41_ertrunken() && g_actors[RE15_ACTOR_SLOT_PLAYER].hp < 0,
           "ROOM%04X: Zeit abgelaufen und noch im Raum -> tot (hp %d)", r, g_actors[RE15_ACTOR_SLOT_PLAYER].hp);
    PRUEFE(tot_bei > 0 && wy_tot <= -5400 - RE15_R41C_ERTRINK_TIEFE && wy_tot > -5400 - RE15_R41C_ERTRINK_TIEFE - RE15_R41C_WASSER_SCHRITT,
           "ROOM%04X: Tod, als das Wasser 900 ueber den Fuessen stand (Wasser y %d beim Tod, Fuesse -5400)", r, (int)wy_tot);
    /* Spritzer = EIN Ereignis, wenn die Flaeche die Fuesse erreicht (Original nur je Fuss-Aufsetzer @0x8003253c ->
     * FUN_80045630 @0x800456b8; keine Rate) — nicht davor, nicht wiederholt bis zum Tod. */
    PRUEFE(spritz_bei > 0 && wy_spritz == -5400 && spritz_bei < tot_bei && re15_ventilkette_r41_spritzer() == 1,
           "ROOM%04X: genau ein Spritzer, als die Flaeche die Fuesse erreichte (Takt %d, Wasser y %d, Tod %d, Zahl %d)", r,
           spritz_bei, (int)wy_spritz, tot_bei, re15_ventilkette_r41_spritzer());
    PRUEFE(g_scd.props[pa].y == RE15_R41C_WASSER_Y_ENDE, "ROOM%04X: Wasser am Ende auf der Tuerebene y %d (ist %d)", r,
           RE15_R41C_WASSER_Y_ENDE, (int)g_scd.props[pa].y);
    /* Blau bleibt: ABR 2 (B-F) zieht nur Rot + Gruen ab (Masken r/g 0xff, b 0x00); gemessen 2026-10-23 22:05, dass
     * "ABR 0 + Maske Blau" im PC-Renderer Blau WEGzog (Bildmitte (29,36,29) -> (30,37,0)). */
    PRUEFE(!(g_fade_ch[RE15_R41C_FADE_KANAL].level & 0x8000) && g_fade_ch[RE15_R41C_FADE_KANAL].abr == 2 &&
           g_fade_ch[RE15_R41C_FADE_KANAL].mask_r == 0xff && g_fade_ch[RE15_R41C_FADE_KANAL].mask_g == 0xff &&
           g_fade_ch[RE15_R41C_FADE_KANAL].mask_b == 0 && (g_fade_ch[RE15_R41C_FADE_KANAL].level >> 7) == 160,
           "ROOM%04X: blaue Blende aktiv = subtraktiv Rot+Gruen, Blau bleibt (level %04x abr %d r %02x g %02x b %02x)", r,
           g_fade_ch[RE15_R41C_FADE_KANAL].level, g_fade_ch[RE15_R41C_FADE_KANAL].abr, g_fade_ch[RE15_R41C_FADE_KANAL].mask_r,
           g_fade_ch[RE15_R41C_FADE_KANAL].mask_g, g_fade_ch[RE15_R41C_FADE_KANAL].mask_b);
    PRUEFE(!re15_ventilkette_r41_timer_laeuft(), "ROOM%04X: nach dem Tod kein Zaehler mehr (keine Flut-Bits beim Neustart)", r);
    PRUEFE(bit(RE15_R41C_BIT_FLUT_GELAUFEN) == 0, "ROOM%04X: Bit 103 nach dem Tod nicht gesetzt", r);
    /* Neustart (CONTINUE) nach dem Tod: Blende weg, Zaehler weg, Ventile wieder fragbar */
    if (raum_hoch(r, 6500, -5400, -200) != 0) { PRUEFE(0, "Parse"); return; }
    PRUEFE((g_fade_ch[RE15_R41C_FADE_KANAL].level & 0x8000) != 0, "ROOM%04X Neustart: Blende aus", r);
    L = druecken(6500, -200, 2048, 1, 300);
    PRUEFE(L.n_msgs == 1 && L.msgs[0] == RE15_R41C_MSG_VENTILE_FRAGE, "ROOM%04X Neustart: Ventile wieder fragbar", r);
}

/* ---- D2: Spieler auf der Tuerebene (Band 9, y -16200): stirbt spaetestens mit der Endlage des Wassers ------ */
static void fall_d2(uint16_t r)
{
    bits_leeren();
    vorflag(RE15_R41C_BANK, RE15_R41C_BIT_VENTIL13);
    vorflag(RE15_R41C_BANK, RE15_R41C_BIT_BRUECKE_UNTEN);
    if (raum_hoch(r, 6500, -5400, -200) != 0) { PRUEFE(0, "ROOM%04X Parse", r); return; }
    lauf_t L = druecken(6500, -200, 2048, 0, 600);
    PRUEFE(L.n_msgs == 2 && re15_ventilkette_r41_timer_laeuft(), "ROOM%04X D2: Zaehler laeuft", r);
    /* dann hinauf zur Tuerebene (im Spiel per Lift B); hier gesetzt */
    g_actors[RE15_ACTOR_SLOT_PLAYER].y = -16200; g_actors[RE15_ACTOR_SLOT_PLAYER].x = -8000; g_actors[RE15_ACTOR_SLOT_PLAYER].z = -5000;
    band_aus_y(-16200);
    int takte = 0, flut_bei = -1, tot_bei = -1; int32_t wy_tot = 0;
    int pa = re15_ventilkette_r41_wasser_prop(0);
    for (int i = 0; i < 2800; i++) {
        g_scd_pad_edge = 0; g_scd_pad_held = 0;
        if (g_scd.message_active && (i & 7) == 7) g_scd_pad_edge = 0x4000;
        takt(); takte++; g_scd_pad_edge = 0;
        if (flut_bei < 0 && re15_ventilkette_r41_flut_laeuft()) flut_bei = takte;
        if (tot_bei < 0 && g_actors[RE15_ACTOR_SLOT_PLAYER].hp < 0) { tot_bei = takte; wy_tot = pa >= 0 ? g_scd.props[pa].y : 0; }
        if (tot_bei > 0 && takte > tot_bei + 60) break;
    }
    printf("  ROOM%04X D2 Tuerebene: Flut bei %d, tot bei %d, Wasser y %d\n", r, flut_bei, tot_bei, (int)wy_tot);
    PRUEFE(tot_bei > 0 && wy_tot == RE15_R41C_WASSER_Y_ENDE, "ROOM%04X D2: Tod auf der Tuerebene mit der Endlage -16200 (Wasser %d)", r, (int)wy_tot);
    /* 324 Schritte, davon der ERSTE im selben VM-Takt wie der Marker Set(5,0x1E) (der For-Koerper laeuft bis zum ersten
     * Evt_next, scd_vm.c op_for; derselbe Takt sieht in re15_ventilkette_r41_tick schon s_flut = 1) -> Endlage -16200
     * und Tod im Takt flut_bei + 323. Gemessen 323 (Leon + Elza), Lauf 2026-10-23 21:50. */
    PRUEFE(flut_bei > 0 && tot_bei - flut_bei >= RE15_R41C_WASSER_SCHRITTE - 1 && tot_bei - flut_bei <= RE15_R41C_WASSER_SCHRITTE + 4,
           "ROOM%04X D2: Tod %d Takte nach Flutbeginn (324 Schritte, der erste im Marker-Takt)", r, tot_bei - flut_bei);
    /* Die Flaeche erreicht die Fuesse (-16200) erst mit der Endlage: ein Spritzer im Todestakt, keiner davor. */
    PRUEFE(re15_ventilkette_r41_spritzer() == 1, "ROOM%04X D2: genau ein Spritzer (Fuesse auf der Tuerebene, Zahl %d)", r,
           re15_ventilkette_r41_spritzer());
}

/* ---- E: Verlassen -> Flut gelaufen; 20B0; Umlenkung; 2030 unter Wasser ------------------------------------ */
static void fall_e(uint16_t r2030, uint16_t r20b0, uint16_t r2000, uint16_t r2010, uint16_t r2050, uint16_t r2070, uint16_t r1260)
{
    bits_leeren();
    vorflag(RE15_R41C_BANK, RE15_R41C_BIT_VENTIL13);
    vorflag(RE15_R41C_BANK, RE15_R41C_BIT_BRUECKE_UNTEN);
    if (raum_hoch(r2030, 6500, -5400, -200) != 0) { PRUEFE(0, "Parse"); return; }
    lauf_t L = druecken(6500, -200, 2048, 0, 600);
    PRUEFE(re15_ventilkette_r41_timer_laeuft(), "ROOM%04X: Zaehler laeuft", r2030);
    /* Raumwechsel waehrend des Zaehlers (= Tuer): Installer des naechsten Raums setzt 103 + 100 */
    vorflag(3, 235);   /* 20B0 nach der Auswasch-Szene (sub05 @0x01FBC), damit der Else-Zweig laeuft */
    if (raum_hoch(r20b0, -25950, -1800, 10300) != 0) { PRUEFE(0, "ROOM%04X Parse", r20b0); return; }
    PRUEFE(bit(RE15_R41C_BIT_FLUT_GELAUFEN) == 1 && bit(RE15_R41C_BIT_LEITER_FREI) == 1, "Verlassen -> Bits 103 + 100");
    PRUEFE(!re15_ventilkette_r41_timer_laeuft(), "Zaehler nach dem Raumwechsel aus");
    /* ab hier gelten 103 + 100 wie in einem Spielstand (jeder Raumstart setzt sie neu, s. raum_hoch); der naechste
     * Eintritt in 20B0 (Else-Zweig @0x01E0A, (3,235)=1 nach der Auswasch-Szene) laeuft mit Bit 100 */
    vorflag(RE15_R41C_BANK, RE15_R41C_BIT_FLUT_GELAUFEN); vorflag(RE15_R41C_BANK, RE15_R41C_BIT_LEITER_FREI);
    if (raum_hoch(r20b0, -25950, -1800, 10300) != 0) { PRUEFE(0, "ROOM%04X Parse", r20b0); return; }
    PRUEFE(g_aot.slots[4].type == RE15_AOT_TYPE_DOOR && g_aot.door_params[4].dest_room == 5, "ROOM%04X: Leiter ist Tuer nach 2050", r20b0);
    int n_mit = strahlen_zaehlen();
    PRUEFE(n_mit == 0, "ROOM%04X nach der Flut: %d Strahlen (erwartet 0, unterdrueckt @0x01E96/@0x01EA6/@0x01EB6)", r20b0, n_mit);
    PRUEFE(re15_ventilkette_r41_espr_unterdrueckt(r20b0, s_daten + 0x01E96, s_daten) == 1 &&
           re15_ventilkette_r41_espr_unterdrueckt(r20b0, s_daten + 0x01E86, s_daten) == 0, "Unterdrueckung nur an den drei Offsets");
    /* Tueren nach 2000 -> 20B0 */
    struct { uint16_t raum; int slot; int cut; } t[] = { { r2010, 0, 10 }, { r2050, 0, 9 }, { r2070, 0, 7 }, { r2070, 3, 7 }, { r1260, 1, 0 }, { r1260, 4, 4 } };
    for (unsigned i = 0; i < sizeof t / sizeof t[0]; i++) {
        if (raum_hoch(t[i].raum, 0, 0, 0) != 0) { PRUEFE(0, "ROOM%04X Parse", t[i].raum); continue; }
        const re15_aot_door_params_t *d = &g_aot.door_params[t[i].slot];
        PRUEFE(g_aot.slots[t[i].slot].type == RE15_AOT_TYPE_DOOR && d->dest_stage == 1 && d->dest_room == 0x0B && d->target_cut == t[i].cut,
               "ROOM%04X Slot %d: -> ROOM20B0 Cut %d (ist Raum %d Cut %d)", t[i].raum, t[i].slot, t[i].cut, d->dest_room, d->target_cut);
    }
    /* 2030 nach der Flut: Wasser y -7200, Ost-Tuer -> 20B0, Lift-A-Rufzone oben (mit (3,222)=1) = msg 1 */
    vorflag(3, 222);
    if (raum_hoch(r2030, 0, -16200, -3000) != 0) { PRUEFE(0, "Parse"); return; }
    int pa = re15_ventilkette_r41_wasser_prop(0), pw = re15_ventilkette_r41_wasser_prop(1);
    PRUEFE(pa >= 0 && pw >= 0 && g_scd.props[pa].y == RE15_R41C_WASSER_Y_RUHE && g_scd.props[pw].y == RE15_R41C_WASSER_Y_RUHE &&
           g_scd.props[pa].z == RE15_R41C_WASSER_ZA && g_scd.props[pw].z == RE15_R41C_WASSER_ZB,
           "ROOM%04X nach der Flut: zwei Wasserflaechen bei y -7200", r2030);
    PRUEFE(g_aot.door_params[0].dest_room == 0x0B && g_aot.door_params[0].target_cut == 11, "ROOM%04X nach der Flut: Ost-Tuer -> 20B0", r2030);
    PRUEFE(g_aot.slots[4].type == RE15_AOT_TYPE_MESSAGE && g_aot.slots[4].event_id == RE15_R41C_MSG_LIFT_ORIGINAL,
           "ROOM%04X nach der Flut: Lift-A-Rufzone Slot 4 = msg 1 (Typ %d msg %d)", r2030, g_aot.slots[4].type, g_aot.slots[4].event_id);
    g_actors[RE15_ACTOR_SLOT_PLAYER].y = -5400;   /* Band 3 fuer das Band-Gate der Stelle (im Spiel unter Wasser, hier nur der Satz) */
    L = druecken(6500, -200, 2048, 0, 300);
    PRUEFE(L.n_msgs == 1 && L.msgs[0] == RE15_R41C_MSG_VENTILE_ZUSTAND, "ROOM%04X nach der Flut: Ventile = Zustandssatz 48", r2030);
    /* mit Bit 107: leer */
    vorflag(RE15_R41C_BANK, RE15_R41C_BIT_2030_LEER);
    if (raum_hoch(r2030, 0, -16200, -3000) != 0) { PRUEFE(0, "Parse"); return; }
    PRUEFE(prop_von_obj(RE15_R41C_WASSER_OBJ_A) < 0 && prop_von_obj(RE15_R41C_WASSER_OBJ_B) < 0, "ROOM%04X mit Bit 107: kein Wasser", r2030);
    PRUEFE(g_aot.slots[4].type != RE15_AOT_TYPE_MESSAGE, "ROOM%04X mit Bit 107: Lift-Rufzone wieder Original", r2030);
    PRUEFE(g_aot.door_params[0].dest_room == 0x0B, "ROOM%04X mit Bit 107: Ost-Tuer bleibt -> 20B0 (Bit 103)", r2030);
    /* Nachbesserung 1 (M8): nach dem Entleeren sagen die Ventile nicht mehr "flooded", sondern Zustandssatz 49 */
    g_actors[RE15_ACTOR_SLOT_PLAYER].y = -5400;
    L = druecken(6500, -200, 2048, 0, 300);
    PRUEFE(L.n_msgs == 1 && L.msgs[0] == RE15_R41C_MSG_VENTILE_LEER, "ROOM%04X mit Bit 107: Ventile = Zustandssatz 49 (msgs %d, erste %d)",
           r2030, L.n_msgs, L.n_msgs ? L.msgs[0] : -1);
}

/* ---- F: 2050 Hebel + Pult ----------------------------------------------------------------------------------- */
static void cursor_setzen(int32_t x, int32_t z)
{
    int c = prop_von_obj(RE15_R41C_CURSOR_OBJ);
    if (c >= 0) { g_scd.props[c].x = x; g_scd.props[c].z = z; }
}

static void square(void)
{
    g_scd_pad_edge = 0x0040; g_scd_pad_held = 0x0040;
    takt();
    g_scd_pad_edge = 0; g_scd_pad_held = 0;
    for (int i = 0; i < 3; i++) takt();
}

/* Nachbesserung 1 (M1): die drei Kisten liegen im Becken = die Bits, die sub01 beim Einschieben setzt ((5,0) @0x0203C/
 * @0x0205A/@0x02078 nach Member_cmp(0x0F == 3/4/5) fuer obj 1, (5,1) @0x02096.. obj 2, (5,2) @0x020F0.. obj 3; Messmittel
 * wie RE15_SET_FLAG_AT=5:0,5:1,5:2 der Abnahme). Danach legt sub01 @0x0201A `46 06 03 31 ff 00 18 02` den Hebel-Platz
 * Slot 6 auf Ereignis 2 (sub01 laeuft je Bild, scd_vm_tick FUN_8003f038 @0x8003f064-84). */
static void kisten_ins_becken(void)
{
    re15_game_flag_set(5, 0, 1); re15_game_flag_set(5, 1, 1); re15_game_flag_set(5, 2, 1);
    for (int i = 0; i < 4; i++) takt();
}

static void fall_f_kisten(uint16_t r)
{
    /* ohne Bit 106: Ereignis 2 = Sperrsatz 40; (3,109) bleibt 0; Wasser obj 0 bleibt "ausser Sicht" y -16200 (@0x01D92) */
    bits_leeren();
    if (raum_hoch(r, 5100, -5400, -10500) != 0) { PRUEFE(0, "ROOM%04X Parse", r); return; }
    const int w = prop_von_obj(0);
    const int32_t wy0 = (w >= 0) ? g_scd.props[w].y : 0;
    PRUEFE(g_aot.slots[6].type == RE15_AOT_TYPE_GENERIC && g_aot.slots[6].event_id == RE15_R41C_EV_2050_HEBEL,
           "ROOM%04X: ohne Kisten Slot 6 = Ereignis 4 (ist %d)", r, g_aot.slots[6].event_id);
    kisten_ins_becken();
    PRUEFE(g_aot.slots[6].active && g_aot.slots[6].type == RE15_AOT_TYPE_GENERIC && g_aot.slots[6].event_id == RE15_R41C_EV_2050_KISTEN,
           "ROOM%04X Kisten im Becken: Slot 6 = Ereignis 2 (sub01 @0x0201A; Typ %d ev %d)", r, g_aot.slots[6].type, g_aot.slots[6].event_id);
    lauf_t L = druecken(5100, -10500, 1024, 0, 600); zeige("kisten ohne", r, &L);
    int cut4 = 0; for (int i = 0; i < L.n_cuts; i++) if (L.cuts[i] == 4) cut4 = 1;
    PRUEFE(L.n_msgs == 1 && L.msgs[0] == RE15_R41C_MSG_HEBEL_OHNE && re15_game_flag_get(3, 109) == 0 && !cut4 &&
           w >= 0 && wy0 == -16200 && g_scd.props[w].y == wy0,
           "ROOM%04X Kisten ohne Strom: Sperrsatz 40, (3,109)=%d, kein Cut 4 (%d), Wasser y %d -> %d (erwartet -16200)",
           r, re15_game_flag_get(3, 109), cut4, (int)wy0, w >= 0 ? (int)g_scd.props[w].y : 0);
    /* Gegenprobe mit Bit 106: Original sub02 @0x0220E -> Frage msg 0 -> Ja -> Set(3,109) @0x0221C, Wasser y 0 @0x02268,
     * Speed -20 For 180 @0x0227C/@0x02280 -> y -3600 */
    bits_leeren(); vorflag(RE15_R41C_BANK, RE15_R41C_BIT_PULT2050);
    if (raum_hoch(r, 5100, -5400, -10500) != 0) { PRUEFE(0, "ROOM%04X Parse", r); return; }
    kisten_ins_becken();
    L = druecken(5100, -10500, 1024, 0, 900); zeige("kisten 106", r, &L);
    const int w2 = prop_von_obj(0);
    PRUEFE(L.n_msgs >= 1 && L.msgs[0] == 0 && re15_game_flag_get(3, 109) == 1 && w2 >= 0 && g_scd.props[w2].y == -3600,
           "ROOM%04X Kisten mit Strom: Original sub02 (msg %d, (3,109)=%d, Wasser y %d, erwartet -3600)",
           r, L.n_msgs ? L.msgs[0] : -1, re15_game_flag_get(3, 109), w2 >= 0 ? (int)g_scd.props[w2].y : 0);
}

static void fall_f(uint16_t r)
{
    bits_leeren();
    if (raum_hoch(r, 5100, -5400, -10500) != 0) { PRUEFE(0, "ROOM%04X Parse", r); return; }
    const re15_aot_t *a = &g_aot.slots[RE15_R41C_SLOT_2050_PULT];
    PRUEFE(a->active && a->type == RE15_AOT_TYPE_GENERIC && a->event_id == RE15_R41C_EV_2050_PULT && a->band == 3,
           "ROOM%04X: Pult Slot 8 Ereignis 9 Band 3 (aktiv %d Typ %d ev %d)", r, a->active, a->type, a->event_id);
    int c = prop_von_obj(RE15_R41C_CURSOR_OBJ);
    PRUEFE(c >= 0 && g_scd.props[c].x == -19554 && g_scd.props[c].z == 22684 && g_scd.props[c].obj_type == 4,
           "ROOM%04X: Cursor obj 8 (-19554,0,22684) Typ 4", r);
    /* Hebel ohne Strom: Platz 6 (4700,-11550,800,800) Band 3, Blick -z */
    lauf_t L = druecken(5100, -10500, 1024, 0, 400); zeige("hebel ohne", r, &L);
    PRUEFE(L.n_msgs == 1 && L.msgs[0] == RE15_R41C_MSG_HEBEL_OHNE && re15_game_flag_get(3, 104) == 0,
           "ROOM%04X Hebel ohne Strom: Sperrsatz 40, (3,104) bleibt 0", r);
    /* Pult: Platz 8 (4000,-8450,1800,800), Blick -z; Nein -> nichts */
    /* Platz 8 (4000,-8450,1800,800) = z -8450..-7650; Spieler z -7100 Blick -z: Vorwaertspunkt -7720 */
    L = druecken(4900, -7100, 1024, 1, 400); zeige("pult nein", r, &L);
    PRUEFE(L.n_msgs == 2 && L.msgs[0] == RE15_R41C_MSG_PULT_BESCHR && L.msgs[1] == RE15_R41C_MSG_PULT_FRAGE && re15_game_flag_get(5, 4) == 0 && g_scd.cam_id != 15,
           "ROOM%04X Pult Nein: Beschreibung 43 + Frage 41, kein Pult", r);
    /* Ja -> Cut 15, (5,4)=1, (5,6)=1, (5,5)=0, Slot 8 aus */
    L = druecken(4900, -7100, 1024, 0,200); zeige("pult ja", r, &L);
    PRUEFE(g_scd.cam_id == 15 && re15_game_flag_get(5, 4) == 1 && re15_game_flag_get(5, 6) == 1 && re15_game_flag_get(5, 5) == 0,
           "ROOM%04X Pult Ja: Cut 15, (5,4)=%d (5,6)=%d (5,5)=%d", r, re15_game_flag_get(5, 4), re15_game_flag_get(5, 6), re15_game_flag_get(5, 5));
    PRUEFE(re15_ventilkette_r41_panel_aktiv() == 1 && g_aot.slots[8].type != RE15_AOT_TYPE_GENERIC, "ROOM%04X Pult aktiv, Slot 8 aus", r);
    /* Original-Cursorfahrt: RIGHT gehalten -> sub07 Speed_set(0,+200) + Add_speed */
    c = prop_von_obj(RE15_R41C_CURSOR_OBJ);
    int32_t x0 = g_scd.props[c].x;
    g_scd_pad_held = 0x0002; takt(); takt(); g_scd_pad_held = 0;
    PRUEFE(g_scd.props[c].x > x0, "ROOM%04X: RIGHT bewegt den Cursor (x %d -> %d, Original sub07 @0x023DA)", r, (int)x0, (int)g_scd.props[c].x);
    /* Zonen: Hebel 1 kippen, dann 3, dann klein -> 80; nebenan nichts */
    const int32_t zx[4] = { -25350, -21772, -18193, -13422 };
    cursor_setzen(-23500, 22215); square();
    PRUEFE(re15_ventilkette_r41_schalter() == 0, "ROOM%04X: SQUARE zwischen den Hebeln kippt nichts", r);
    cursor_setzen(zx[0], 22215); square();
    PRUEFE(re15_ventilkette_r41_schalter() == 0x1 && re15_ventilkette_r41_spannung() == 30, "ROOM%04X: Hebel 1 EIN -> 30 V (0x%X)", r, re15_ventilkette_r41_schalter());
    cursor_setzen(zx[1], 22215); square();
    PRUEFE(re15_ventilkette_r41_schalter() == 0x3 && re15_ventilkette_r41_spannung() == 50, "ROOM%04X: Hebel 2 EIN -> 50 V", r);
    cursor_setzen(zx[1], 22215); square();
    PRUEFE(re15_ventilkette_r41_schalter() == 0x1, "ROOM%04X: Hebel 2 wieder AUS", r);
    cursor_setzen(zx[2], 22215); square();
    cursor_setzen(zx[3], 22215); square();
    PRUEFE(re15_ventilkette_r41_schalter() == 0xD && re15_ventilkette_r41_spannung() == 80 && bit(RE15_R41C_BIT_PULT2050) == 0,
           "ROOM%04X: 30+40+10 = 80 V, Bit 106 noch nicht (Ruhe)", r);
    int x_zeiger = 0, y_zeiger = 0, w = 0, h = 0;
    PRUEFE(re15_ventilkette_r41_sicht(0, &x_zeiger, &y_zeiger, &w, &h) == 1 && x_zeiger + 1 == re15_ventilkette_r41_zeiger_x(80) &&
           re15_ventilkette_r41_zeiger_x(80) == 238 && re15_ventilkette_r41_zeiger_x(0) == 166, "Zeiger: x(80) = 238, x(0) = 166 (ist %d/%d)",
           re15_ventilkette_r41_zeiger_x(80), re15_ventilkette_r41_zeiger_x(0));
    PRUEFE(re15_ventilkette_r41_sicht(1, NULL, NULL, NULL, NULL) == 1 && re15_ventilkette_r41_sicht(2, NULL, NULL, NULL, NULL) == 0, "Marken: Hebel 1 an, 2 aus");
    /* 30 Ruhebilder -> Bit 106, Satz 42, danach Ausstieg (sub10): (5,4)=0, Slot 8 wieder Ereignis 9, Cut zurueck */
    int geloest_bei = -1;
    for (int i = 0; i < 400; i++) {
        g_scd_pad_edge = 0; g_scd_pad_held = 0;
        if (g_scd.message_active && (i & 7) == 7) g_scd_pad_edge = 0x4000;
        takt(); g_scd_pad_edge = 0;
        if (geloest_bei < 0 && bit(RE15_R41C_BIT_PULT2050)) geloest_bei = i + 1;
        if (geloest_bei > 0 && re15_game_flag_get(5, 4) == 0 && !g_scd.message_active && i > geloest_bei + 20) break;
    }
    PRUEFE(geloest_bei > 0 && geloest_bei <= RE15_R41C_PANEL_RUHE + 2, "ROOM%04X: Bit 106 nach %d Ruhebildern (erwartet <= 32)", r, geloest_bei);
    PRUEFE(re15_game_flag_get(5, 4) == 0 && g_scd.cam_id != 15 && g_aot.slots[8].type == RE15_AOT_TYPE_GENERIC && g_aot.slots[8].event_id == 9,
           "ROOM%04X: Ausstieg sub10 ((5,4)=%d cam %d Slot 8 Typ %d ev %d)", r, re15_game_flag_get(5, 4), g_scd.cam_id, g_aot.slots[8].type, g_aot.slots[8].event_id);
    c = prop_von_obj(RE15_R41C_CURSOR_OBJ);
    PRUEFE(c >= 0 && g_scd.props[c].x == -19554 && g_scd.props[c].z == 22684, "ROOM%04X: Cursor geparkt (sub10 Pos_set @0x02414)", r);
    /* danach: Hebel = Original sub04 -> (3,104)=1; Pult = Zustandssatz */
    L = druecken(5100, -10500, 1024, 0, 600); zeige("hebel orig", r, &L);
    PRUEFE(L.n_msgs == 1 && L.msgs[0] == 0 && re15_game_flag_get(3, 104) == 1, "ROOM%04X Hebel mit Strom: Original msg 0, (3,104)=1", r);
    L = druecken(4900, -7100, 1024, 0,300); zeige("pult zust", r, &L);
    PRUEFE(L.n_msgs == 1 && L.msgs[0] == RE15_R41C_MSG_PULT_ZUSTAND, "ROOM%04X Pult danach: Zustandssatz 44", r);
    /* Abbruch mit CROSS: neuer Raum ohne Bit 106 */
    bits_leeren();
    if (raum_hoch(r, 4900, -5400, -7100) != 0) { PRUEFE(0, "Parse"); return; }
    L = druecken(4900, -7100, 1024, 0,200);
    PRUEFE(re15_game_flag_get(5, 4) == 1, "ROOM%04X: Pult offen", r);
    g_scd_pad_edge = 0x8000; takt(); g_scd_pad_edge = 0;
    for (int i = 0; i < 40; i++) takt();
    PRUEFE(re15_game_flag_get(5, 4) == 0 && g_scd.cam_id != 15 && bit(RE15_R41C_BIT_PULT2050) == 0, "ROOM%04X: CROSS = Abbruch (sub10), Bit 106 bleibt 0", r);
    fall_f_kisten(r);
}

/* ---- G: 2080 Rechner ---------------------------------------------------------------------------------------- */
static void fall_g(uint16_t r)
{
    /* ohne Flut: Original */
    bits_leeren();
    if (raum_hoch(r, -11550, -14400, -6000) != 0) { PRUEFE(0, "ROOM%04X Parse", r); return; }
    lauf_t L = druecken(-11550, -6000, 1024, 0, 900); zeige("rechner", r, &L);
    int p0 = prop_von_obj(0);
    /* Wasser obj 0: -10800 (@0x00C2E) + 148 x 50 (@0x00CC8/@0x00CCC) = -3400 */
    PRUEFE(L.n_msgs == 1 && L.msgs[0] == 0 && re15_game_flag_get(3, 245) == 1 && bit(RE15_R41C_BIT_2030_LEER) == 0 && p0 >= 0 && g_scd.props[p0].y == -3400,
           "ROOM%04X ohne Flut: Original sub02 ((3,245)=%d, Bit 107=%d, Wasser y %d)", r, re15_game_flag_get(3, 245), bit(107), p0 >= 0 ? (int)g_scd.props[p0].y : 0);
    /* mit Flut, Rechner noch nicht gelaufen: sub02 + Schwanz */
    bits_leeren(); vorflag(RE15_R41C_BANK, RE15_R41C_BIT_FLUT_GELAUFEN);
    if (raum_hoch(r, -11550, -14400, -6000) != 0) { PRUEFE(0, "Parse"); return; }
    L = druecken(-11550, -6000, 1024, 0, 900); zeige("rechner+2030", r, &L);
    p0 = prop_von_obj(0);
    PRUEFE(L.n_msgs == 2 && L.msgs[0] == 0 && L.msgs[1] == RE15_R41C_MSG_RECHNER_2030 && re15_game_flag_get(3, 245) == 1 &&
           bit(RE15_R41C_BIT_2030_LEER) == 1 && p0 >= 0 && g_scd.props[p0].y == -3400,
           "ROOM%04X mit Flut: msg 0 + 40, (3,245)=1, Bit 107, Wasser -3400 (msgs %d, y %d)", r, L.n_msgs, p0 >= 0 ? (int)g_scd.props[p0].y : 0);
    PRUEFE(g_scd.cam_id == 3 && re15_game_flag_get(2, 7) == 0, "ROOM%04X: Ende wie Original (Cut 3, frei)", r);
    /* mit Flut, Rechner schon gelaufen ((3,245)=1): Slot 4 erneut, nur Frage + Bit 107 */
    bits_leeren(); vorflag(RE15_R41C_BANK, RE15_R41C_BIT_FLUT_GELAUFEN); vorflag(3, 245);
    if (raum_hoch(r, -11550, -14400, -6000) != 0) { PRUEFE(0, "Parse"); return; }
    PRUEFE(g_aot.slots[4].active && g_aot.slots[4].type == RE15_AOT_TYPE_GENERIC && g_aot.slots[4].event_id == 2,
           "ROOM%04X Rechner gelaufen + Flut: Slot 4 erneut (Typ %d ev %d)", r, g_aot.slots[4].type, g_aot.slots[4].event_id);
    L = druecken(-11550, -6000, 1024, 0, 400); zeige("nur 2030", r, &L);
    PRUEFE(L.n_msgs == 2 && L.msgs[0] == 0 && L.msgs[1] == RE15_R41C_MSG_RECHNER_2030 && bit(RE15_R41C_BIT_2030_LEER) == 1 &&
           g_aot.slots[4].type != RE15_AOT_TYPE_GENERIC, "ROOM%04X nur 2030: Frage + Satz, Bit 107, Slot 4 aus", r);
    /* mit Bit 107: keine Stelle mehr */
    vorflag(RE15_R41C_BANK, RE15_R41C_BIT_2030_LEER);
    if (raum_hoch(r, -11550, -14400, -6000) != 0) { PRUEFE(0, "Parse"); return; }
    PRUEFE(!g_aot.slots[4].active || g_aot.slots[4].type != RE15_AOT_TYPE_GENERIC, "ROOM%04X mit Bit 107: Slot 4 bleibt aus (Typ %d ev %d, (3,245)=%d, Bit 103=%d 107=%d)", r,
           g_aot.slots[4].type, g_aot.slots[4].event_id, re15_game_flag_get(3, 245), bit(RE15_R41C_BIT_FLUT_GELAUFEN), bit(RE15_R41C_BIT_2030_LEER));
}

/* ---- I: Ring-Zugaenge des gefluteten 2030 (Orchestrator-Entscheid C-E2, Abnahme 0 M5) ---------------------------- */
static void ring_pruefen(uint16_t raum, int slot, int msg, int32_t x, int32_t y, int32_t z, int rot, int band)
{
    /* geflutet: Bit 103 ohne 107 -> Sperrtext, Aktion zeigt ihn, kein Raumwechsel */
    bits_leeren();
    vorflag(3, 235); vorflag(RE15_R41C_BANK, RE15_R41C_BIT_FLUT_GELAUFEN); vorflag(RE15_R41C_BANK, RE15_R41C_BIT_LEITER_FREI);
    if (raum_hoch(raum, x, y, z) != 0) { PRUEFE(0, "ROOM%04X Parse", raum); return; }
    const re15_aot_t *a = &g_aot.slots[slot];
    PRUEFE(a->active && a->type == RE15_AOT_TYPE_MESSAGE && a->event_id == msg && a->band == band,
           "ROOM%04X Slot %d geflutet: Sperrtext %d Band %d (Typ %d ev %d Band %d)", raum, slot, msg, band, a->type, a->event_id, a->band);
    memset(&g_room_change, 0, sizeof g_room_change);
    lauf_t L = druecken(x, z, rot, 0, 300); zeige("ring zu", raum, &L);
    PRUEFE(L.n_msgs == 1 && L.msgs[0] == msg && !g_room_change.pending,
           "ROOM%04X Slot %d geflutet: Aktion zeigt nur Text %d, kein Raumwechsel (msgs %d, erste %d, pending %d)",
           raum, slot, msg, L.n_msgs, L.n_msgs ? L.msgs[0] : -1, g_room_change.pending);
    /* entleert: Bit 107 -> wieder die Original-Tuer nach 2030 (Stage 1 Raum 3) */
    vorflag(RE15_R41C_BANK, RE15_R41C_BIT_2030_LEER);
    if (raum_hoch(raum, x, y, z) != 0) { PRUEFE(0, "ROOM%04X Parse", raum); return; }
    PRUEFE(g_aot.slots[slot].type == RE15_AOT_TYPE_DOOR && g_aot.door_params[slot].dest_stage == 1 &&
           g_aot.door_params[slot].dest_room == RE15_R41C_RAUM_2030_ID,
           "ROOM%04X Slot %d mit Bit 107: Tuer nach 2030 (Typ %d Raum %d)", raum, slot, g_aot.slots[slot].type, g_aot.door_params[slot].dest_room);
}

static void fall_i(uint16_t r20b0, uint16_t r2070, uint16_t r2080)
{
    /* 20B0 Slot 3 @0x019C2 Rechteck (5450,-7900,1000,3000) Band 1: Spieler (4900,-6400) Blick +x (Punkt 620 -> x 5520) */
    ring_pruefen(r20b0, RE15_R41C_SLOT_20B0_RING, RE15_R41C_MSG_RING_20B0, 4900, -1800, -6400, 0, 1);
    /* 2070 Slot 1 @0x00A5A Rechteck (5000,-7900,1000,3000) Band 0: Spieler (4400,-6400) Blick +x */
    ring_pruefen(r2070, RE15_R41C_SLOT_2070_RING_A, RE15_R41C_MSG_RING_2070, 4400, 0, -6400, 0, 0);
    /* 2070 Slot 2 @0x00A7A Rechteck (-19950,4900,1000,3000) Band 0: Struktur (gleiches Ziel) */
    bits_leeren(); vorflag(RE15_R41C_BANK, RE15_R41C_BIT_FLUT_GELAUFEN);
    if (raum_hoch(r2070, 0, 0, 0) == 0)
        PRUEFE(g_aot.slots[RE15_R41C_SLOT_2070_RING_B].type == RE15_AOT_TYPE_MESSAGE &&
               g_aot.slots[RE15_R41C_SLOT_2070_RING_B].event_id == RE15_R41C_MSG_RING_2070 &&
               g_aot.slots[0].type == RE15_AOT_TYPE_DOOR && g_aot.door_params[0].dest_room == 0x0B,
               "ROOM%04X geflutet: Slot 2 Sperrtext 40, Slot 0 bleibt Tuer (-> 20B0)", r2070);
    /* Tuerebene bleibt offen: 2080 Slot 0 @0x00BA2 -> 2030 (-6950,-16200,6450) nfloor 9 */
    bits_leeren(); vorflag(RE15_R41C_BANK, RE15_R41C_BIT_FLUT_GELAUFEN);
    if (raum_hoch(r2080, -11550, -14400, -6000) == 0)
        PRUEFE(g_aot.slots[0].type == RE15_AOT_TYPE_DOOR && g_aot.door_params[0].dest_room == RE15_R41C_RAUM_2030_ID,
               "ROOM%04X geflutet: Tuerebenen-Tuer Slot 0 nach 2030 bleibt offen (Typ %d)", r2080, g_aot.slots[0].type);
}

/* ---- H: Bank-5-Marker nur in 2030 loeschen (Nachbesserung 1, M3) ------------------------------------------------ */
static void fall_h(void)
{
    /* Direkt am Installer: fremder Raum laesst beide Bits stehen, 2030 loescht sie. */
    bits_leeren();
    re15_game_flag_set(5, RE15_R41C_FLAG_TIMER, 1); re15_game_flag_set(5, RE15_R41C_FLAG_FLUT, 1);
    re15_ventilkette_r41_install(0x1171, NULL);
    PRUEFE(re15_game_flag_get(5, RE15_R41C_FLAG_TIMER) == 1 && re15_game_flag_get(5, RE15_R41C_FLAG_FLUT) == 1,
           "ROOM1171: Installer laesst (5,0x1D)/(5,0x1E) stehen");
    re15_ventilkette_r41_install(0x2031, NULL);
    PRUEFE(re15_game_flag_get(5, RE15_R41C_FLAG_TIMER) == 0 && re15_game_flag_get(5, RE15_R41C_FLAG_FLUT) == 0,
           "ROOM2031: Installer loescht die Marker");
    /* Echter Raumstart ROOM1171 mit (3,194) (Kraehenszene): sub00 Switch -> Gosub sub07 -> Evt_exec sub02, das im
     * Init-Takt @0x013B2 `22 05 1e 01` setzt (erst @0x014A4 `22 05 1e 00` loescht es wieder, nach dem Warten auf
     * (5,28) @0x013F4). Vor der Nachbesserung loeschte der Installer es direkt danach. */
    bits_leeren(); vorflag(3, 194);
    if (raum_hoch(0x1171, -1500, 0, -8000) != 0) { PRUEFE(0, "ROOM1171 Parse"); return; }
    PRUEFE(re15_game_flag_get(3, 194) == 0 && re15_game_flag_get(5, 0x1E) == 1,
           "ROOM1171 (3,194): sub02 lief ((3,194)=%d) und (5,0x1E) = %d nach dem Installer (erwartet 1, @0x013B2)",
           re15_game_flag_get(3, 194), re15_game_flag_get(5, 0x1E));
}

/* Werkzeug fuer den exe-Riegel C (M4): Mittel einer Flaeche eines P6-Framedumps. */
static int ppm_mittel(const char *pfad, int x0, int y0, int x1, int y1)
{
    long sz = 0;
    uint8_t *d = slurp(pfad, &sz);
    if (!d) { printf("FEHLER: %s nicht lesbar\n", pfad); return 2; }
    int w = 0, h = 0, mx = 0, n = 0;
    if (sscanf((const char *)d, "P6 %d %d %d%n", &w, &h, &mx, &n) != 3 || w <= 0 || h <= 0 || mx != 255) {
        printf("FEHLER: kein P6/255: %s\n", pfad); free(d); return 2; }
    const long kopf = n + 1;   /* genau ein Trennzeichen nach maxval */
    if (x0 < 0 || y0 < 0 || x1 > w || y1 > h || x0 >= x1 || y0 >= y1 || kopf + (long)w * h * 3 > sz) {
        printf("FEHLER: Flaeche/Groesse (%dx%d, %ld B)\n", w, h, sz); free(d); return 2; }
    long s[3] = { 0, 0, 0 }, cnt = 0;
    for (int y = y0; y < y1; y++)
        for (int x = x0; x < x1; x++) {
            const uint8_t *px = d + kopf + ((long)y * w + x) * 3;
            s[0] += px[0]; s[1] += px[1]; s[2] += px[2]; cnt++;
        }
    printf("MITTEL %ld %ld %ld (%dx%d, Flaeche %d,%d..%d,%d)\n", s[0] / cnt, s[1] / cnt, s[2] / cnt, w, h, x0, y0, x1, y1);
    free(d);
    return 0;
}

int main(int argc, char **argv)
{
    if (argc == 7 && strcmp(argv[1], "ppm") == 0)
        return ppm_mittel(argv[2], atoi(argv[3]), atoi(argv[4]), atoi(argv[5]), atoi(argv[6]));
    fall_a();
    for (int v = 0; v < 2; v++) {
        printf("== %s\n", v ? "Elza" : "Leon");
        fall_b((uint16_t)(0x2000 + v), (uint16_t)(0x20B0 + v));
        fall_c((uint16_t)(0x2030 + v));
        fall_d((uint16_t)(0x2030 + v));
        fall_d2((uint16_t)(0x2030 + v));
        fall_e((uint16_t)(0x2030 + v), (uint16_t)(0x20B0 + v), (uint16_t)(0x2000 + v), (uint16_t)(0x2010 + v),
               (uint16_t)(0x2050 + v), (uint16_t)(0x2070 + v), (uint16_t)(0x1260 + v));
        fall_f((uint16_t)(0x2050 + v));
        fall_g((uint16_t)(0x2080 + v));
        fall_i((uint16_t)(0x20B0 + v), (uint16_t)(0x2070 + v), (uint16_t)(0x2080 + v));
    }
    /* fremd: ROOM2060 ohne Plaetze 12..14 / Wasser */
    bits_leeren();
    if (raum_hoch(0x2060, 0, 0, 0) == 0) {
        int n = 0;
        for (int s = 0; s < RE15_AOT_MAX; s++)
            if (g_aot.slots[s].active && g_aot.slots[s].type == RE15_AOT_TYPE_GENERIC &&
                (g_aot.slots[s].event_id == RE15_R41C_EV_PULT || g_aot.slots[s].event_id == RE15_R41C_EV_VENTIL13 || g_aot.slots[s].event_id == RE15_R41C_EV_VENTILE)) n++;
        PRUEFE(n == 0 && re15_ventilkette_r41_wasser_prop(0) < 0, "ROOM2060: %d fremde Plaetze (obj 4 ist dort das Sicherungs-Panel)", n);
    }
    fall_h();
    printf("%s (%d Pruefungen)\n", s_fail ? "FAIL" : "PASS", s_pruef);
    free(s_daten);
    return s_fail;
}
