/*
 * unit_r41_ventilkette — Runde 41 Spur C "ventilkette": die STAGE2-Kette, Glied fuer Glied gemessen.
 * Herleitung: include/re15_ventilkette_r41.h, Dossier analysis/befunde_runde41/C_ventilkette.md.
 *
 * ECHTER WEG je Raum (Leon + Elza): RDT laden, Raum wie im Spiel hochfahren (scd_room_reenter = Installer-Block),
 * Spieler vor die Stelle, Blick hin, Aktion -> re15_aot_scan (Vorwaertspunkt 620) -> scd_event_fire -> Programm;
 * je Bild scd_vm_tick + re15_ventilkette_r41_tick (der Spiel-Takt-Haken). Geprueft:
 *   A  Original-Scheiben Byte fuer Byte gegen die RDTs; Port-Texte dekodiert; 2080 ohne Port-Programme/-Texte
 *      (Nachtrag 7); Netz der Relinghoehe (Nachtrag 8) = MD1-Form von ROOM2080 Prop 0 ueber der Schachtoeffnung
 *   B  2000/2001: Leiter Slot 4 = Sperrtext 40 (Aktion zeigt ihn), mit Bit 100 = Tuer
 *   C  2030/2031: Bruecke oben + Zelle 12 zu; Pult "kein Druck"; Ventil 13 (Frage, Ja, Bit 101, Klick-Zustand);
 *      Pult "Lower the bridge?" -> Fahrt 30 x 120 unter Cut 4 -> y -1800, Bit 102, Zelle 12 offen; mit 102 unten
 *   D  Ventile (Nachbesserung 4: sechs Raeder, Folge 09/07/02/01; Bits 101/102 + Sicherung (3,108)): Fragen 50..55, nach der
 *      vierten Drehung Warnung, Zaehler (5,0x1D) -> Ost-Tuer -> 20B0,
 *      Wasser geparkt; waehrend des Zaehlers zeigen die Ventile die Warnung 46 erneut; 1800 Takte -> Flut (5,0x1E),
 *      Wasser steigt 50/Takt, Spieler (zurueck am Ring) ertrinkt (hp -1, Blende), Zeit abgelaufen = tot
 *   E  Verlassen waehrend des Zaehlers -> Bits 103 + 100; 20B0: Leiter = Tuer, Strahlen unterdrueckt (0 statt 3);
 *      Tueren nach 2000 -> 20B0 mit Cut-Zuordnung; 2030 nach der Flut ohne Strom unter Wasser (y -7200), Lift-Rufzone =
 *      msg 1, kein Ventil-Platz mehr; MIT Strom (3,126) (Nachtraege 7/8/12): eine Flaeche y -5350 (Schachtoeffnung), Treppe
 *      Slot 8 = Sperrtext 48 im Band 3 (Aktion vom NW-Balkon zeigt ihn), Lifte unten = Original-Ereignisse 2/4
 *   F  2050/2051: Hebel ohne Strom = msg 40, (3,104) bleibt; Pult: Slot 8 + Cursor, Frage, Cut 15, Cursor faehrt
 *      (Original sub07), SQUARE kippt Hebel (Zonen), 30+40+10 = 80 -> 30 Ruhebilder -> Bit 106, "Power supply OK.",
 *      Ausstieg sub10; danach Hebel = Original sub04 ((3,104)=1); alle 16 Masken: nur 0xD = 80;
 *      Kisten im Becken (sub01 @0x0201A -> Ereignis 2 = sub02 @0x0220E): ohne Bit 106 Sperrsatz 40, Wasser bleibt;
 *      mit Bit 106 Original sub02 (Wasser steigt) (Nachbesserung 1, M1)
 *   G  2080/2081: Rechner = nur Original (Nachtrag 7) in JEDEM Zustand (ohne Flut, mit Flut, mit Flut + Strom):
 *      kein Port-Programm, msg 0, (3,245)=1, Wasser obj 0 -> -3400, kein Bit 107, kein Port-Text; danach Slot 4 aus
 *   H  fremde Raeume: der Installer laesst (5,0x1D)/(5,0x1E) stehen (ROOM1171 sub02 @0x013B2 `22 05 1e 01` im
 *      Init-Takt bleibt 1), nur 2030/2031 loescht er (Nachbesserung 1, M3)
 *   I  Ring-Zugaenge des gefluteten 2030 (Orchestrator-Entscheid C-E2, Abnahme 0 M5): 20B0/20B1 Slot 3 @0x019C2 und
 *      2070/2071 Slot 1 @0x00A5A / Slot 2 @0x00A7A sind mit Bit 103 ohne Strom Sperrtexte (41 bzw. 40, Aktion zeigt ihn,
 *      kein Raumwechsel), mit Strom (3,126) (Nachtrag 7) wieder Tueren nach 2030; die Tuerebenen-Tueren bleiben offen
 *   J  Zugaenge 2080/2090/20A0 (Nachbesserung 2; seit Nachtrag 6 bleibt die Original-Sperre): J1 Zugangs-Zensus aller
 *      RDTs (Tueren nach 2080/2090/20A0 und Setzer von (3,206) genau wie der Tuergraph), J2 die ORIGINAL-Sperre
 *      ROOM11A0/11A1 Slot 4 (@0x00FEA Ck(3,206)=0 -> @0x00FEE msg 2) an der Engine: Aktion = nur msg 2, kein
 *      Raumwechsel; mit (3,206)=1 Tuer nach 20A0 — der Weg zu 2080 fuehrt seit Nachtrag 7 ueber den Strom (K/L)
 *   K  Ventilraetsel erst mit Bruecke UND Sicherung im Ventilkasten 2010 (Nachbesserung 5, Nachtraege 9/16): die sechs
 *      Plaetze liegen an den Raedern (Band 1, Nord- und Sued-Landeplatz), am alten Ring-Platz feuert nichts; ohne Bit 102
 *      oder ohne Bit (9,125) Satz 49 = Original "It's locked." (ROOM20A0 msg 1 @0x203C) und kein Zaehler - auch mit der
 *      aufgehobenen Sicherung (3,108) allein; mit Bit 125 die Ventil-Frage (50..55)
 *   N  Ventil-Sicherungskasten ROOM2010/2011 (Nachbesserung 5, Nachtrag 16): Platz 8 / Ereignis 20 an der Ostwand; ohne
 *      Sicherung nur Text 40; mit (3,108) Text 40 + Frage 41 -> Ja -> Bit 125, Standbild 2030 Cut 7 + Aufschliess-Ton,
 *      Bestaetigen schliesst, Text 42; Nein laesst 125 aus; steckt sie: Text 43 (Frage) -> Ja -> 125 aus, Text 44; im
 *      Generator ((3,144)) nur Text 40
 *   O  ROOM2060/2061: steckt die Sicherung im Ventilkasten (Bit 125), zeigt der Generator-Kasten Slot 9 Text 3 (Else-Zweig
 *      @0x010A6) statt Ereignis 18; ohne Bit 125 das Original (Ereignis 18), mit (3,144) Text 6
 *   P  Abnahme 4 Mangel 2: waehrend des Zaehlers oeffnet das Plakat nicht; laeuft die Flut, nimmt das Schliessen der
 *      Nahaufnahme die Pad-Sperre der Flut (2,7)/(2,2) nicht zurueck
 *   Q  Ventil-Dreh-Animation (Nachbesserung 5, Nachtrag 18 Punkt 3) = RE2 ROOM1080 sub08: Spieler auf die Radmitte, 816
 *      vor die Wand, Blick zur Wand; Clips 15 -> 16 -> 17 (5 Takte) -> 16/15 rueckwaerts (Flags 0xC0); 6 Ventil-Toene;
 *      erst danach ist die Drehung vermerkt
 *   M  Nachbesserung 4 (Nachtrag 13): falsche Folge (die Plakat-Folge woertlich 09/01/02/07) -> nach der vierten Drehung
 *      (5,0x10..0x13) := 0 + Text 45, kein Zaehler; Nein vermerkt nichts; richtige Folge -> 46 + Zaehler; Dokument = Item-
 *      Zone Slot 20 (0x4E, Bit 107, obj 6) -> Leser angefordert; mit Bit 107 weder Zone noch Blatt; Plakat Slot 21 ->
 *      Nahaufnahme offen (Pad-Sperre, Bild), Bestaetigen schliesst sie; nach der Flut weder Ventile noch Dokument/Plakat
 *   L  Strom-Zensus + Zustand (Nachtrag 7): (3,126) nur 2060/2061 sub01 @0x012AE gesetzt, nie geloescht; (3,108) nur
 *      2030/2031 sub06 @0x01FF2; SCA-Belege: Band-1-Reling fl 0x11, Ring-Boden fl 0x23, Absperrung 19-22/31/32 fl 0x33 =
 *      Rand der Schachtoeffnung (Nachbesserung 4: Pegel -5350 = Ringboden -5400 + 50)
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
#include "re15_room_list.h"
#include "re15_md1.h"
#include "re15_files.h"      /* Nachbesserung 4: Dokument 6 */
#include "re15_menu.h"       /* re15_menu_doc_active */
#include "re15_bss.h"        /* Nachbesserung 5: Werkzeug "bss" (Standbild 2030 Cut 7 im Riegel k1) */

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

/* Nachbesserung 4: Ventil k (0..5 = 01/02/03/07/08/09) von vorn druecken - Nordwand (z -8500): Spieler z -7900 Blick -z
 * (1024), Suedwand (z 7900): Spieler z 7300 Blick +z (3072); Vorwaertspunkt 620 -> in den Platz Wand +- 500. */
static const int32_t k_vx[RE15_R41C_VENTILE_N] = RE15_R41C_VENTIL_X;
static const int32_t k_vz[RE15_R41C_VENTILE_N] = RE15_R41C_VENTIL_WAND_Z;
static const int     k_los[RE15_R41C_DREHUNGEN] = RE15_R41C_LOESUNG;
static lauf_t ventil(int k, int nein, int max_takte)
{
    const int nord = k_vz[k] < 0;
    if (!nein && max_takte < 900 && !re15_ventilkette_r41_timer_laeuft())
        max_takte = 900;   /* Nachbesserung 5: Ja = RE2-Dreh-Animation (345 Takte, RE2 ROOM1080 sub08); waehrend des Zaehlers nur 46 */
    return druecken(k_vx[k], nord ? k_vz[k] + 600 : k_vz[k] - 600, nord ? 1024 : 3072, nein, max_takte);
}
/* die vier Drehungen der richtigen Folge; Rueckgabe = Lauf der LETZTEN (Frage + Warnung) */
static lauf_t ventile_richtig(int max_letzte)
{
    lauf_t L;
    memset(&L, 0, sizeof L);
    for (int i = 0; i < RE15_R41C_DREHUNGEN; i++) L = ventil(k_los[i], 0, i + 1 < RE15_R41C_DREHUNGEN ? 300 : max_letzte);
    return L;
}
static int ventil_platz_weg(void)
{
    for (int k = 0; k < RE15_R41C_VENTILE_N; k++)
        if (g_aot.slots[RE15_R41C_SLOT_VENTIL0 + k].active && g_aot.slots[RE15_R41C_SLOT_VENTIL0 + k].type != RE15_AOT_TYPE_NONE) return 0;
    return 1;
}

static int bit(int b) { return re15_game_flag_get(RE15_R41C_BANK, (uint8_t)b) ? 1 : 0; }
static int sicherung_bit(void) { return re15_game_flag_get(RE15_R41C_SICHERUNG_BANK, RE15_R41C_SICHERUNG_BIT) ? 1 : 0; }
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
        { 0x2030, RE15_R41C_ORG_2030_SLOT8_OFF, re15_r41c_org_2030_slot8, (int)sizeof re15_r41c_org_2030_slot8, "2030 Slot 8 Treppe" },
        { 0x2030, RE15_R41C_ORG_2030_UNTEN_OFF, re15_r41c_org_2030_unten, (int)sizeof re15_r41c_org_2030_unten, "2030 Sicherung + Shotgun Shells" },
        { 0x2030, RE15_R41C_ORG_2030_SUB01_OFF, re15_r41c_org_2030_sub01, (int)sizeof re15_r41c_org_2030_sub01, "2030 sub01 Strom -> Lifte" },
        { 0x2030, RE15_R41C_ORG_2030_SUB06_OFF, re15_r41c_org_2030_sub06, (int)sizeof re15_r41c_org_2030_sub06, "2030 sub06 Sicherung (3,108)" },
        { 0x2060, RE15_R41C_ORG_2060_STROM_OFF, re15_r41c_org_2060_strom, (int)sizeof re15_r41c_org_2060_strom, "2060 sub01 Loesung + Set(3,126)" },
        { 0x2060, RE15_R41C_ORG_2060_MSG4_OFF,  re15_r41c_org_2060_msg4,  (int)sizeof re15_r41c_org_2060_msg4,  "2060 msg 4 (2010 Id 41)" },
        { 0x2060, RE15_R41C_ORG_2060_MSG5_OFF,  re15_r41c_org_2060_msg5,  (int)sizeof re15_r41c_org_2060_msg5,  "2060 msg 5 (2010 Id 42)" },
        { 0x2030, RE15_R41C_ORG_2030_MSG3_OFF,  re15_r41c_org_2030_msg3,  (int)sizeof re15_r41c_org_2030_msg3,  "2030 msg 3 (2010 Id 44)" },
        { 0x20A0, RE15_R41C_ORG_20A0_MSG1_OFF,  re15_r41c_org_20a0_msg1,  (int)sizeof re15_r41c_org_20a0_msg1,  "20A0 msg 1 (2030 Id 49)" },
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
    /* Nachtrag 7: 2080 nur Original — keine Port-Programme, kein Port-Text 2080 Id 40/41 (VERTRAG 1.2) */
    PRUEFE(re15_ventilkette_r41_programm("2080_rechner_erw", NULL) == NULL && re15_ventilkette_r41_programm("2080_rechner_nur2030", NULL) == NULL,
           "2080: keine Port-Programme mehr (Nachtrag 7)");
    PRUEFE(re15_ventilkette_r41_text(0x2080, 40, NULL) == NULL && re15_ventilkette_r41_text(0x2081, 41, NULL) == NULL,
           "2080/2081: keine Port-Texte 40/41 (VERTRAG 1.2: entfallen)");
    PRUEFE(re15_ventilkette_r41_programm("2030_ventile_leer", NULL) == NULL && re15_ventilkette_r41_programm("2030_ventile_zustand", NULL) == NULL &&
           re15_ventilkette_r41_programm("2030_ventile_laeuft", NULL) != NULL && re15_ventilkette_r41_programm("2030_ventile_vorher", NULL) != NULL,
           "2030: Ventil-Programme laeuft/vorher statt zustand/leer (Nachbesserung 3)");
    /* Nachbesserung 4: der Platzhalter (EIN Platz, Ja/Nein-Frage 45) ist weg; sechs Ventil-Programme, Flut/Falsch/Plakat */
    PRUEFE(re15_ventilkette_r41_programm("2030_ventile", NULL) == NULL, "2030: kein Platzhalter-Programm 2030_ventile mehr (Nachtrag 13)");
    { static const char *const nm[6] = { "2030_ventil_01", "2030_ventil_02", "2030_ventil_03", "2030_ventil_07", "2030_ventil_08", "2030_ventil_09" };
      for (int k = 0; k < 6; k++) {
          int n = 0; const uint8_t *p = re15_ventilkette_r41_programm(nm[k], &n);
          int marker = 0;
          for (int i = 0; p && i + 3 < n; i++) if (p[i] == 0x22 && p[i + 1] == 5 && p[i + 2] == RE15_R41C_FLAG_GEDREHT && p[i + 3] == 1) marker = 1;
          PRUEFE(p && p[0] == 0x2b && p[1] == RE15_R41C_MSG_VENTIL0 + k && marker,
                 "%s: Message_on %d + Ja -> Set(5,0x1B)=1", nm[k], RE15_R41C_MSG_VENTIL0 + k);
          /* Nachbesserung 5 (Nachtrag 18 Punkt 3): RE2 ROOM1080 sub08 - Plc_motion 15, 16, 5 x 17, 16/15 rueckwaerts (0xC0),
           * sechs Ton-Marker (5,0x1A) statt se_on(2,5), Plc_ret; die Drehung wird erst danach vermerkt. */
          int folge[16], nf = 0, ton = 0, ret = -1, mark = -1;
          for (int i = 0; p && i + 3 < n; i++) {
              if (p[i] == 0x3f && p[i + 1] == 0 && nf < 16) { folge[nf++] = p[i + 2] | (p[i + 3] << 8); i += 3; continue; }
              if (p[i] == 0x22 && p[i + 1] == 5 && p[i + 2] == RE15_R41C_FLAG_TON && p[i + 3] == 1) { ton++; i += 3; continue; }
              if (p[i] == 0x42 && p[i + 1] == 0 && ret < 0 && nf > 0) ret = i;
              if (p[i] == 0x22 && p[i + 1] == 5 && p[i + 2] == RE15_R41C_FLAG_GEDREHT && p[i + 3] == 1) mark = i;
          }
          static const int soll[9] = { 15, 16, 17, 17, 17, 17, 17, 16 | 0xC000, 15 | 0xC000 };
          int ok = nf == 9;
          for (int i = 0; ok && i < 9; i++) if (folge[i] != soll[i]) ok = 0;
          PRUEFE(ok && ton == 6 && ret > 0 && mark > ret, "%s: RE2-Dreh-Animation (9 Plc_motion %d, 6 Toene %d, Plc_ret vor dem Marker)",
                 nm[k], nf, ton);
      } }
    /* Nachbesserung 5 (Nachtrag 16): Programme des Ventil-Sicherungskastens 2010 */
    { int n = 0; const uint8_t *p = re15_ventilkette_r41_programm("2010_kasten_frage", &n);
      int setz = 0, still = 0, wart = 0;
      for (int i = 0; p && i + 3 < n; i++) {
          if (p[i] == 0x22 && p[i + 1] == 9 && p[i + 2] == RE15_R41C_BIT_KASTEN && p[i + 3] == 1) setz = 1;
          if (p[i] == 0x22 && p[i + 1] == 5 && p[i + 2] == RE15_R41C_FLAG_STANDBILD && p[i + 3] == 1) still = 1;
          if (p[i] == 0x0f && p[i + 1] == 4 && p[i + 4] == 0x21 && p[i + 5] == 5 && p[i + 6] == RE15_R41C_FLAG_STANDBILD) wart = 1;
      }
      PRUEFE(p && p[0] == 0x2b && p[1] == RE15_R41C_MSG_KASTEN_LEER && setz && still && wart,
             "2010_kasten_frage: Text 40, Frage 41, Ja -> Set(9,125), Standbild-Marker, While bis zu (Form ROOM2000 @0x01AD4)"); }
    { int n = 0; const uint8_t *p = re15_ventilkette_r41_programm("2010_kasten_drin", &n); int loesch = 0;
      for (int i = 0; p && i + 3 < n; i++) if (p[i] == 0x22 && p[i + 1] == 9 && p[i + 2] == RE15_R41C_BIT_KASTEN && p[i + 3] == 0) loesch = 1;
      PRUEFE(p && p[1] == RE15_R41C_MSG_KASTEN_DRIN && loesch, "2010_kasten_drin: Text 43 (Frage) -> Ja -> Set(9,125)=0");
      p = re15_ventilkette_r41_programm("2010_kasten_leer", &n);
      PRUEFE(p && p[0] == 0x2b && p[1] == RE15_R41C_MSG_KASTEN_LEER && p[4] == 0x02, "2010_kasten_leer: nur Text 40"); }
    { int n = 0; const uint8_t *p = re15_ventilkette_r41_programm("2030_ventile_falsch", &n);
      static const uint8_t soll[16] = { 0x22,5,0x10,0, 0x22,5,0x11,0, 0x22,5,0x12,0, 0x22,5,0x13,0 };
      PRUEFE(p && n > 20 && memcmp(p, soll, 16) == 0 && p[16] == 0x2b && p[17] == RE15_R41C_MSG_VENTILE_ZURUECK,
             "2030_ventile_falsch: (5,0x10..0x13) := 0 (Form ROOM11F0 sub17 @0x016C0) + Text 45"); }
    { int n = 0; const uint8_t *p = re15_ventilkette_r41_programm("2030_ventile_flut", &n);
      PRUEFE(p && p[0] == 0x2b && p[1] == RE15_R41C_MSG_WARNUNG, "2030_ventile_flut: beginnt mit der Warnung 46"); }
    { int n = 0; const uint8_t *p = re15_ventilkette_r41_programm("2030_plakat", &n);
      static const uint8_t soll[12] = { 0x22,2,7,1, 0x22,2,2,1, 0x22,5,RE15_R41C_FLAG_PLAKAT,1 };
      PRUEFE(p && n >= 14 && memcmp(p, soll, 12) == 0 && p[12] == 0x01, "2030_plakat: Pad-Sperre, KI steht, Marker (5,0x1C), Evt_end"); }
    /* Dokument 6 (Nachtrag 13 Folge 2): Item 0x4E, Bildsatz 31, max_page 2, H 176 (Satz FILE31) */
    { const re15_file_doc_t *d = re15_files_doc(RE15_R41C_DOK_NR);
      PRUEFE(d && d->item_id == RE15_R41C_DOK_ITEM && d->bildsatz == 31 && d->max_page == 2 && d->page_h == 176 &&
             re15_files_doc_from_item(RE15_R41C_DOK_ITEM) == RE15_R41C_DOK_NR,
             "re15_files: Dokument 6 = Item 0x4E, Bildsatz 31, max_page 2, H 176"); }
    { int n = 0; const uint8_t *p = re15_ventilkette_r41_programm("2030_ventile_laeuft", &n);
      PRUEFE(p && n >= 4 && p[0] == 0x2b && p[1] == RE15_R41C_MSG_WARNUNG, "2030_ventile_laeuft: Message_on 46 (die Warnung)"); }
    /* Netz der Relinghoehe: MD1-Form von ROOM2080 Prop 0, nur ueber der Schachtoeffnung (Nachbesserung 3, N3) */
    { int n = 0; const uint8_t *m = re15_ventilkette_r41_md1_bytes_fuer(1, &n); int nv = 0;
      if (m && n > 0) {
          re15_md1_t md; memset(&md, 0, sizeof md);
          int ok = re15_md1_parse(m, n, &md) == 0 && md.mesh_count == 1 && md.meshes[0].quad_count > 0;
          PRUEFE(ok, "Reling-Netz: MD1 parsebar (1 Netz, Vierecke)");
          if (ok) {
              static const int32_t rr[RE15_R41C_RELING_N][4] = RE15_R41C_RELING_RECHTECKE;
              long long flaeche_soll = 0, flaeche_ist = 0; int alle_drin = 1, uv_gleich = 1;
              for (int i = 0; i < RE15_R41C_RELING_N; i++) flaeche_soll += (long long)(rr[i][1] - rr[i][0]) * (rr[i][3] - rr[i][2]);
              const re15_md1_mesh_t *ms = &md.meshes[0];
              nv = ms->quad_vertex_count;
              int ow = 0; const uint8_t *org = re15_ventilkette_r41_md1_bytes_fuer(0, &ow);
              re15_md1_t mo; memset(&mo, 0, sizeof mo);
              (void)re15_md1_parse(org, ow, &mo);
              for (int q = 0; q < ms->quad_count; q++) {
                  const re15_md1_quad_t *f = &ms->quads[q];
                  const re15_md1_vertex_t *a = &ms->quad_vertices[f->v0], *d = &ms->quad_vertices[f->v3];
                  if (a->y != 0 || d->y != 0) alle_drin = 0;
                  int drin = 0;
                  for (int i = 0; i < RE15_R41C_RELING_N; i++)
                      if (a->x >= rr[i][0] && d->x <= rr[i][1] && a->z >= rr[i][2] && d->z <= rr[i][3]) drin = 1;
                  if (!drin || d->x - a->x > 1800 || d->z - a->z > 1800) alle_drin = 0;
                  flaeche_ist += (long long)(d->x - a->x) * (d->z - a->z);
                  if (memcmp(&ms->quad_uvs[q], &mo.meshes[0].quad_uvs[0], sizeof ms->quad_uvs[q]) != 0) uv_gleich = 0;
              }
              PRUEFE(alle_drin && flaeche_ist == flaeche_soll, "Reling-Netz: %d Vierecke <= 1800 in den Rechtecken, Flaeche %lld == %lld",
                     ms->quad_count, flaeche_ist, flaeche_soll);
              /* Nachbesserung 4: der Treppenschacht (x -5400..-4200 z -8500..-6900) liegt unter dem Pegel -5350 -> Flaeche */
              int schacht = 0;
              for (int q = 0; q < ms->quad_count; q++) {
                  const re15_md1_vertex_t *a = &ms->quad_vertices[ms->quads[q].v0], *d = &ms->quad_vertices[ms->quads[q].v3];
                  const int ox0 = a->x > -5400 ? a->x : -5400, ox1 = d->x < -4200 ? d->x : -4200;   /* Schnitt mit dem Schacht */
                  const int oz0 = a->z > -8500 ? a->z : -8500, oz1 = d->z < -6900 ? d->z : -6900;
                  if (ox1 > ox0 && oz1 > oz0) schacht += (ox1 - ox0) * (oz1 - oz0);
              }
              PRUEFE(schacht == 1200 * 1600, "Reling-Netz: Treppenschacht x -5400..-4200 z -8500..-6900 bedeckt (Flaeche %d)", schacht);
              PRUEFE(uv_gleich && ms->quad_normal_count == 1 && ms->quad_normals[0].y == -4096,
                     "Reling-Netz: UV-Satz und Normale = ROOM2080 Prop 0 (@0x000D98)");
              printf("  Reling-Netz: %d B, %d Ecken, %d Vierecke, Flaeche %lld\n", n, nv, ms->quad_count, flaeche_ist);
          }
      } else PRUEFE(0, "Reling-Netz fehlt (nur PC-Ziel)"); }
    /* Texte (Port-Ids, Glyphen des Ports) */
    PRUEFE(text_ist(0x2000, 40, "I cannot climb up the ladder because of the water pressure."), "Text 2000/40");
    PRUEFE(text_ist(0x20B1, 40, "I cannot climb up the ladder because of the water pressure."), "Text 20B1/40 (= 2000)");
    PRUEFE(text_ist(0x2030, 43, "The pressure gauge reads 80. Lower the bridge?"), "Text 2030/43");
    PRUEFE(text_ist(0x2030, 46, "WARNING: The water will rise in one minute. Please leave the room immediately!") &&
           text_ist(0x2031, 46, "WARNING: The water will rise in one minute. Please leave the room immediately!"),
           "Text 2030/2031 46 = Wortlaut Nachtrag 19");
    PRUEFE(text_ist(0x2050, 42, "Power supply OK."), "Text 2050/42 = 11F0 msg 2");
    PRUEFE(text_ist(0x2050, 41, "Will you push them?"), "Text 2050/41 = 11F0 msg 1");
    PRUEFE(text_ist(0x2031, 48, "I cannot go down the stairs because of the water."), "Text 2031/48 (Treppe, Nachtrag 8)");
    PRUEFE(text_ist(0x2030, 49, "It's locked.") && text_ist(0x2031, 49, "It's locked."),
           "Text 2030/2031 49 = Original ROOM20A0 msg 1 @0x203C (Nachtrag 16 Folge 4)");
    PRUEFE(text_ist(0x2010, 40, "A fuse box for the valve controls.One of the fuses is missing...") &&
           text_ist(0x2011, 41, "Will you use the Fuse?") && text_ist(0x2010, 42, "You've used the Fuse.") &&
           text_ist(0x2011, 43, "The fuse is in place.Will you take the Fuse?") && text_ist(0x2010, 44, "You've taken the Fuse."),
           "Texte 2010/2011 40..44 (Ventilkasten; 41/42 = ROOM2060 msg 4/5, 43 = 2060 msg 6 + 2030 msg 2, 44 = 2030 msg 3)");
    PRUEFE(re15_ventilkette_r41_text(0x2010, 45, NULL) == NULL && re15_ventilkette_r41_text(0x2010, 39, NULL) == NULL,
           "2010: nur Ids 40..44 (VERTRAG 1.2)");
    PRUEFE(text_ist(0x2031, 45, "The valves turned back to their original position."), "Text 2031/45 (falsche Folge, Nachtrag 13)");
    PRUEFE(text_ist(0x2030, 50, "Valve 01, black wheel. Will you turn it?") && text_ist(0x2030, 51, "Valve 02, green wheel. Will you turn it?") &&
           text_ist(0x2030, 52, "Valve 03, red wheel. Will you turn it?") && text_ist(0x2031, 53, "Valve 07, black wheel. Will you turn it?") &&
           text_ist(0x2031, 54, "Valve 08, green wheel. Will you turn it?") && text_ist(0x2031, 55, "Valve 09, red wheel. Will you turn it?"),
           "Texte 2030/2031 50..55 (Ventil-Fragen, Nummer + Farbe)");
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
    const int slots[4] = { RE15_R41C_SLOT_PULT, RE15_R41C_SLOT_VENTIL13, RE15_R41C_SLOT_VENTIL0, RE15_R41C_SLOT_PLAKAT };
    const int evs[4] = { RE15_R41C_EV_PULT, RE15_R41C_EV_VENTIL13, RE15_R41C_EV_VENTIL0, RE15_R41C_EV_PLAKAT };
    for (int i = 0; i < 4; i++) {
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
/* Raumstart am Sued-Landeplatz vor Ventil 09 (Nachbesserung 4: sechs Plaetze an den Raedern, Kopf "Ventil-RAEDER"). */
#define VX (-2650)
#define VZ 7300
#define VROT 3072
static void fall_d(uint16_t r)
{
    bits_leeren();
    vorflag(RE15_R41C_BANK, RE15_R41C_BIT_VENTIL13);
    vorflag(RE15_R41C_BANK, RE15_R41C_BIT_BRUECKE_UNTEN);
    vorflag(RE15_R41C_SICHERUNG_BANK, RE15_R41C_SICHERUNG_BIT);
    vorflag(RE15_R41C_BANK, RE15_R41C_BIT_KASTEN);   /* Nachbesserung 5 (Nachtrag 16): Sicherung im Ventilkasten 2010 */
    if (raum_hoch(r, VX, -1800, VZ) != 0) { PRUEFE(0, "ROOM%04X Parse", r); return; }
    lauf_t L = ventil(5, 1, 400); zeige("09 nein", r, &L);
    PRUEFE(L.n_msgs == 1 && L.msgs[0] == RE15_R41C_MSG_VENTIL0 + 5 && !re15_ventilkette_r41_timer_laeuft() && !g_scd.message_active &&
           re15_ventilkette_r41_drehungen() == 0,
           "ROOM%04X Ventil 09 Nein: nur Frage 55, keine Drehung (msgs %d, Drehungen %d)", r, L.n_msgs, re15_ventilkette_r41_drehungen());
    /* richtige Folge 09, 07, 02, 01: nach der vierten Drehung Warnung, dann schlaeft der Faden (Abbruch nach 900 Takten) */
    L = ventile_richtig(900); zeige("folge ja", r, &L);
    PRUEFE(L.n_msgs == 2 && L.msgs[0] == RE15_R41C_MSG_VENTIL0 + 0 && L.msgs[1] == RE15_R41C_MSG_WARNUNG,
           "ROOM%04X richtige Folge: vierte Frage (01 = 50) + Warnung 46 (msgs %d)", r, L.n_msgs);
    PRUEFE(re15_ventilkette_r41_timer_laeuft() && re15_game_flag_get(5, RE15_R41C_FLAG_TIMER) == 1, "ROOM%04X: Zaehler laeuft ((5,0x3D)=1)", r);
    PRUEFE(re15_game_flag_get(2, 7) == 0 && g_scd.message_active == 0, "ROOM%04X: Spieler frei waehrend des Zaehlers", r);
    const re15_aot_door_params_t *d = &g_aot.door_params[RE15_R41C_SLOT_2030_OSTTUER];
    PRUEFE(g_aot.slots[0].type == RE15_AOT_TYPE_DOOR && d->dest_room == 0x0B && d->dest_stage == 1 && d->target_cut == 11 &&
           d->spawn_x == 5950 && d->spawn_z == -6400, "ROOM%04X: Ost-Tuer -> ROOM20B0 Cut 11 (Ziel %d, Cut %d)", r, d->dest_room, d->target_cut);
    int pa = re15_ventilkette_r41_wasser_prop(0), pw = re15_ventilkette_r41_wasser_prop(1);
    PRUEFE(pa >= 0 && pw >= 0 && g_scd.props[pa].z == RE15_R41C_WASSER_PARK_Z && g_scd.props[pa].obj_type == 3,
           "ROOM%04X: Wasserflaechen angelegt und geparkt (z %d)", r, pa >= 0 ? (int)g_scd.props[pa].z : 0);
    /* waehrend des Zaehlers: die Ventile zeigen die Warnung erneut (Programm 2030_ventile_laeuft), kein zweiter Zaehler */
    L = ventil(0, 0, 300); zeige("ventile zust", r, &L);
    PRUEFE(L.n_msgs == 1 && L.msgs[0] == RE15_R41C_MSG_WARNUNG && re15_ventilkette_r41_timer_laeuft(),
           "ROOM%04X: Ventile waehrend des Zaehlers = Warnung 46 (msgs %d, erste %d)", r, L.n_msgs, L.n_msgs ? L.msgs[0] : -1);
    /* zurueck zum Ring (im Spiel: Steg -> Treppe -> Ring), dort laeuft die Flut ab */
    g_actors[RE15_ACTOR_SLOT_PLAYER].x = 6500; g_actors[RE15_ACTOR_SLOT_PLAYER].y = -5400; g_actors[RE15_ACTOR_SLOT_PLAYER].z = -200;
    band_aus_y(-5400);
    /* Zaehler bis zum Ende: 1800 Takte Sleep (1200 liefen schon in den druecken-Laeufen: 900 + 300) */
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
    printf("  ROOM%04X Flut bei Takt %d (nach den druecken-Laeufen +1200), tot bei %d, Wasser y %d, Spieler y %d\n", r, flut_bei, tot_bei,
           pa >= 0 ? (int)g_scd.props[pa].y : 0, (int)g_actors[RE15_ACTOR_SLOT_PLAYER].y);
    /* 1800 Takte Sleep ab dem Schliessen der Warnung; davor Frage (2 Seiten) + Warnung im druecken-Lauf (je 8 Takte
     * Tastentakt + Schreibmaschine), gemessen ~300 Takte, seit Nachbesserung 5 dazu die Dreh-Animation der vierten Drehung
     * (345 Takte = Summe der Sleeps von RE2 ROOM1080 sub08 @0x02130..@0x021FA) -> Flut zwischen 1800 und 1800 + 450 + 345 */
    PRUEFE(flut_bei > 0 && flut_bei + 1200 >= RE15_R41C_ZAEHLER_BILDER && flut_bei + 1200 <= RE15_R41C_ZAEHLER_BILDER + 450 + 345,
           "ROOM%04X: Flut nach 1800 Takten Sleep (gemessen %d + 1200 nach der Aktion)", r, flut_bei);
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
    if (raum_hoch(r, VX, -1800, VZ) != 0) { PRUEFE(0, "Parse"); return; }
    PRUEFE((g_fade_ch[RE15_R41C_FADE_KANAL].level & 0x8000) != 0, "ROOM%04X Neustart: Blende aus", r);
    L = ventil(5, 1, 300);
    PRUEFE(L.n_msgs == 1 && L.msgs[0] == RE15_R41C_MSG_VENTIL0 + 5 && re15_ventilkette_r41_drehungen() == 0,
           "ROOM%04X Neustart: Ventile wieder fragbar, keine Drehung vermerkt", r);
}

/* ---- D2: Spieler auf der Tuerebene (Band 9, y -16200): stirbt spaetestens mit der Endlage des Wassers ------ */
static void fall_d2(uint16_t r)
{
    bits_leeren();
    vorflag(RE15_R41C_BANK, RE15_R41C_BIT_VENTIL13);
    vorflag(RE15_R41C_BANK, RE15_R41C_BIT_BRUECKE_UNTEN);
    vorflag(RE15_R41C_SICHERUNG_BANK, RE15_R41C_SICHERUNG_BIT);
    vorflag(RE15_R41C_BANK, RE15_R41C_BIT_KASTEN);   /* Nachbesserung 5 (Nachtrag 16): Sicherung im Ventilkasten 2010 */
    if (raum_hoch(r, VX, -1800, VZ) != 0) { PRUEFE(0, "ROOM%04X Parse", r); return; }
    lauf_t L = ventile_richtig(600);
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
    vorflag(RE15_R41C_SICHERUNG_BANK, RE15_R41C_SICHERUNG_BIT);
    vorflag(RE15_R41C_BANK, RE15_R41C_BIT_KASTEN);   /* Nachbesserung 5 (Nachtrag 16) */
    if (raum_hoch(r2030, VX, -1800, VZ) != 0) { PRUEFE(0, "Parse"); return; }
    lauf_t L = ventile_richtig(600);
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
    PRUEFE(re15_ventilkette_r41_zustand_2030() == RE15_R41C_2030_GEFLUTET, "ROOM%04X nach der Flut ohne Strom: Zustand geflutet", r2030);
    PRUEFE(ventil_platz_weg() && !g_aot.slots[RE15_R41C_SLOT_DOKUMENT].active && !g_aot.slots[RE15_R41C_SLOT_PLAKAT].active &&
           prop_von_obj(RE15_R41C_DOK_OBJ) < 0 && prop_von_obj(RE15_R41C_PLAKAT_OBJ) < 0,
           "ROOM%04X nach der Flut: weder Ventile noch Dokument/Plakat (unter Wasser, Nachtrag 9)", r2030);
    PRUEFE(re15_ventilkette_r41_obj_id(r2030, 1) == RE15_R41C_WASSER_OBJ_B, "ROOM%04X geflutet: zwei Flaechen (obj 4/5)", r2030);
    /* MIT Strom (3,126) (Nachtraege 7/8): Relinghoehe — eine Flaeche ueber der Schachtoeffnung, Treppe gesperrt */
    vorflag(RE15_R41C_STROM_BANK, RE15_R41C_STROM_BIT);
    if (raum_hoch(r2030, -6600, -5400, -7700) != 0) { PRUEFE(0, "Parse"); return; }
    pa = prop_von_obj(RE15_R41C_WASSER_OBJ_A);
    PRUEFE(re15_ventilkette_r41_zustand_2030() == RE15_R41C_2030_RELING, "ROOM%04X mit Strom: Zustand Relinghoehe", r2030);
    PRUEFE(pa >= 0 && g_scd.props[pa].y == RE15_R41C_WASSER_Y_RELING && g_scd.props[pa].x == 0 && g_scd.props[pa].z == 0 &&
           prop_von_obj(RE15_R41C_WASSER_OBJ_B) < 0,
           "ROOM%04X mit Strom: EINE Flaeche obj 4 bei (0,-5350,0), keine obj 5 (y %d)", r2030, pa >= 0 ? (int)g_scd.props[pa].y : 0);
    /* Nachtrag 12 (k): knapp UNTER der Absperrung = unter dem Ringboden -5400 (PSX-y: groesser = tiefer), Abstand 50 */
    PRUEFE(RE15_R41C_WASSER_Y_RELING == -5350 && RE15_R41C_WASSER_Y_RELING > RE15_R41C_RINGBODEN_Y &&
           RE15_R41C_WASSER_Y_RELING - RE15_R41C_RINGBODEN_Y == RE15_R41C_WASSER_SCHRITT,
           "Pegel mit Strom -5350 = Ringboden -5400 + ein Wasser-Schritt 50 (@0x00CC8)");
    PRUEFE(re15_ventilkette_r41_obj_id(r2030, 0) == RE15_R41C_WASSER_OBJ_A && re15_ventilkette_r41_obj_id(r2030, 1) == -1,
           "ROOM%04X mit Strom: Modell nur fuer obj 4", r2030);
    { int n = 0, m = 0; const uint8_t *b = re15_ventilkette_r41_md1_bytes(&n); const uint8_t *br = re15_ventilkette_r41_md1_bytes_fuer(1, &m);
      PRUEFE(b && b == br && n == m, "ROOM%04X mit Strom: main.c laedt das Netz der Relinghoehe (%d B)", r2030, n); }
    const re15_aot_t *tr = &g_aot.slots[RE15_R41C_SLOT_2030_TREPPE];
    PRUEFE(tr->active && tr->type == RE15_AOT_TYPE_MESSAGE && tr->event_id == RE15_R41C_MSG_TREPPE_FLUT && tr->band == RE15_R41C_TREPPE_BAND,
           "ROOM%04X mit Strom: Treppe Slot 8 = Sperrtext 48 Band 3 (Typ %d ev %d Band %d)", r2030, tr->type, tr->event_id, tr->band);
    PRUEFE(g_aot.slots[4].type != RE15_AOT_TYPE_MESSAGE, "ROOM%04X mit Strom: Lift-Rufzone oben wieder Original", r2030);
    PRUEFE(g_aot.slots[7].type == RE15_AOT_TYPE_GENERIC && g_aot.slots[7].event_id == 4,
           "ROOM%04X mit Strom: Lift B unten Slot 7 = Ereignis 4 (sub00 @0x01D88, sub01 @0x01E14 mit Strom ohne Reset; Typ %d ev %d)",
           r2030, g_aot.slots[7].type, g_aot.slots[7].event_id);
    PRUEFE(g_aot.door_params[0].dest_room == 0x0B, "ROOM%04X mit Strom: Ost-Tuer bleibt -> 20B0 (Bit 103)", r2030);
    PRUEFE(ventil_platz_weg() && !g_aot.slots[RE15_R41C_SLOT_DOKUMENT].active && !g_aot.slots[RE15_R41C_SLOT_PLAKAT].active,
           "ROOM%04X mit Strom: weder Ventile noch Dokument/Plakat (unten geflutet)", r2030);
    /* Aktion vom NW-Balkon (Band 3) Richtung Treppe (Blick +x, Punkt 620 -> x -5980 im Treppen-Rechteck x -6050..-2350) */
    memset(&g_room_change, 0, sizeof g_room_change);
    L = druecken(-6600, -7700, 0, 0, 300); zeige("treppe zu", r2030, &L);
    PRUEFE(L.n_msgs == 1 && L.msgs[0] == RE15_R41C_MSG_TREPPE_FLUT && g_actors[RE15_ACTOR_SLOT_PLAYER].y == -5400,
           "ROOM%04X mit Strom: Aktion an der Treppe = Text 48, Spieler bleibt am Ring (msgs %d, erste %d, y %d)", r2030,
           L.n_msgs, L.n_msgs ? L.msgs[0] : -1, (int)g_actors[RE15_ACTOR_SLOT_PLAYER].y);
    /* Gegenprobe ohne Flut: die Treppe ist die Original-Treppe */
    bits_leeren();
    if (raum_hoch(r2030, -6600, -5400, -7700) != 0) { PRUEFE(0, "Parse"); return; }
    PRUEFE(g_aot.slots[RE15_R41C_SLOT_2030_TREPPE].type == RE15_AOT_TYPE_STAIR, "ROOM%04X ohne Flut: Treppe Slot 8 bleibt Treppe", r2030);
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
    /* Nachbesserung 5 (Nachtrag 18 Punkt 2): der umgelegte Hebel laeuft je Bild einen Schritt nach unten, 16 Schritte wie
     * ROOM11F0 sub06 (For 16 @0x01336); square() zaehlt 4 Takte (Kippen + 3), der erste Schritt faellt in das Bild NACH dem
     * Kippen (der Schritt steht in panel_tick vor der SQUARE-Auswertung) -> nach square() Stellung 3. */
    { const int t0 = re15_ventilkette_r41_hebel_stellung(0); int n = 0;
      while (re15_ventilkette_r41_hebel_stellung(0) < RE15_R41C_HEBEL_TAKTE && n < 40) { takt(); n++; }
      PRUEFE(t0 == 3 && t0 + n == RE15_R41C_HEBEL_TAKTE && re15_ventilkette_r41_hebel_stellung(1) == 0,
             "ROOM%04X: Hebel 1 laeuft in 16 Bildern nach unten (nach dem Kippen %d, danach %d Bilder), Hebel 2 bleibt oben", r, t0, n); }
    cursor_setzen(zx[1], 22215); square();
    PRUEFE(re15_ventilkette_r41_schalter() == 0x3 && re15_ventilkette_r41_spannung() == 50, "ROOM%04X: Hebel 2 EIN -> 50 V", r);
    cursor_setzen(zx[1], 22215); square();
    PRUEFE(re15_ventilkette_r41_schalter() == 0x1, "ROOM%04X: Hebel 2 wieder AUS", r);
    { int n = 0; while (re15_ventilkette_r41_hebel_stellung(1) > 0 && n < 40) { takt(); n++; }
      PRUEFE(re15_ventilkette_r41_hebel_stellung(1) == 0 && re15_ventilkette_r41_hebel_stellung(0) == RE15_R41C_HEBEL_TAKTE,
             "ROOM%04X: Hebel 2 laeuft wieder nach oben, Hebel 1 bleibt unten", r); }
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

/* ---- G: 2080 Rechner = nur Original (Nachtrag 7) ---------------------------------------------------------------- */
static void fall_g(uint16_t r)
{
    /* drei Zustaende: ohne Flut / Flut ohne Strom / Flut mit Strom — ueberall dasselbe Original sub02 @0x00C86 */
    for (int z = 0; z < 3; z++) {
        bits_leeren();
        if (z >= 1) vorflag(RE15_R41C_BANK, RE15_R41C_BIT_FLUT_GELAUFEN);
        if (z >= 2) vorflag(RE15_R41C_STROM_BANK, RE15_R41C_STROM_BIT);
        if (raum_hoch(r, -11550, -14400, -6000) != 0) { PRUEFE(0, "ROOM%04X Parse", r); return; }
        PRUEFE(re15_ventilkette_r41_ereignis(r, 2) == NULL, "ROOM%04X Zustand %d: Ereignis 2 = kein Port-Programm", r, z);
        PRUEFE(g_aot.slots[4].type == RE15_AOT_TYPE_GENERIC && g_aot.slots[4].event_id == 2,
               "ROOM%04X Zustand %d: Rechner Slot 4 = Original sub00 @0x00C1A", r, z);
        lauf_t L = druecken(-11550, -6000, 1024, 0, 900); zeige("rechner", r, &L);
        int p0 = prop_von_obj(0);
        /* Wasser obj 0: -10800 (@0x00C2E) + 148 x 50 (@0x00CC8/@0x00CCC) = -3400 */
        PRUEFE(L.n_msgs == 1 && L.msgs[0] == 0 && re15_game_flag_get(3, 245) == 1 && bit(107) == 0 && p0 >= 0 && g_scd.props[p0].y == -3400,
               "ROOM%04X Zustand %d: Original sub02 (msgs %d, (3,245)=%d, Bit 107=%d, Wasser y %d)", r, z, L.n_msgs,
               re15_game_flag_get(3, 245), bit(107), p0 >= 0 ? (int)g_scd.props[p0].y : 0);
        PRUEFE(g_scd.cam_id == 3 && re15_game_flag_get(2, 7) == 0, "ROOM%04X Zustand %d: Ende wie Original (Cut 3, frei)", r, z);
        PRUEFE(g_aot.slots[1].type == RE15_AOT_TYPE_DOOR, "ROOM%04X Zustand %d: Tuer Slot 1 nach 2090 an (@0x00CAC)", r, z);
    }
    /* Wiedereintritt mit (3,245)=1: Slot 4 bleibt aus (Original sub00 Else @0x00C5E), auch nach der Flut */
    bits_leeren(); vorflag(RE15_R41C_BANK, RE15_R41C_BIT_FLUT_GELAUFEN); vorflag(3, 245);
    if (raum_hoch(r, -11550, -14400, -6000) != 0) { PRUEFE(0, "Parse"); return; }
    PRUEFE(!g_aot.slots[4].active || g_aot.slots[4].type != RE15_AOT_TYPE_GENERIC, "ROOM%04X (3,245)=1 + Flut: Slot 4 aus (Typ %d)", r,
           g_aot.slots[4].type);
}

/* ---- I: Ring-Zugaenge des gefluteten 2030 (Orchestrator-Entscheid C-E2, Abnahme 0 M5) ---------------------------- */
static void ring_pruefen(uint16_t raum, int slot, int msg, int32_t x, int32_t y, int32_t z, int rot, int band)
{
    /* geflutet: Bit 103 ohne Strom -> Sperrtext, Aktion zeigt ihn, kein Raumwechsel */
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
    /* Strom (3,126) (Nachtrag 7: "Nach der Aktivierung des Stroms ist 2030 leer gelaufen") -> wieder die Original-Tuer
     * nach 2030 (Stage 1 Raum 3); Aktion -> Raumwechsel nach 2030 */
    vorflag(RE15_R41C_STROM_BANK, RE15_R41C_STROM_BIT);
    if (raum_hoch(raum, x, y, z) != 0) { PRUEFE(0, "ROOM%04X Parse", raum); return; }
    PRUEFE(g_aot.slots[slot].type == RE15_AOT_TYPE_DOOR && g_aot.door_params[slot].dest_stage == 1 &&
           g_aot.door_params[slot].dest_room == RE15_R41C_RAUM_2030_ID,
           "ROOM%04X Slot %d mit Strom: Tuer nach 2030 (Typ %d Raum %d)", raum, slot, g_aot.slots[slot].type, g_aot.door_params[slot].dest_room);
    memset(&g_room_change, 0, sizeof g_room_change);
    L = druecken(x, z, rot, 0, 60);
    PRUEFE(g_room_change.pending && (g_room_change.room_id & 0xFFFEu) == 0x2030u,
           "ROOM%04X Slot %d mit Strom: Aktion -> Raumwechsel nach 2030 (pending %d Raum %04X)", raum, slot, g_room_change.pending,
           g_room_change.room_id);
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

/* ---- J: Weg zum Rechner 2080 nach der Flut (Nachbesserung 2, Mangel N1 / Orchestrator-Entscheid C-E2b) ------------
 * J1 Zugangs-Zensus ueber ALLE RDTs (re15_room_ids): SCD-Regionen von main (RDT+0x40) und sub (RDT+0x44) wie
 *    tools/scd_dump_room.py (Zeigertabelle, Regionsende = naechster Zeiger bzw. kleinster Kopf-Zeiger hinter der
 *    Tabelle), Laengen aus scd_opcode_size_at (= VM-Tabelle s_opcode_sizes). Door_aot_set (0x3B) mit Ziel
 *    Stage-Index 1 / Raum 8 (2080), 9 (2090), 0x0A (20A0) und Set(3,206)=1 `22 03 ce 01` muessen GENAU an den Stellen
 *    des Tuergraphen stehen (tools/r41_ventilkette/tuergraph_nach_flut.py, C_belege/nb2_tuergraph_nach_flut.txt).
 *    Dazu die Bytes der Sperre ROOM11A0/11A1 main00 @0x00FEA `21 03 ce 00` + @0x00FEE (Text-Platz slot 4 msg 2).
 * J2 die Original-Sperre an der Engine (Leon 11A0, Elza 11A1): Zustand nach der Flut (Bits 103+100) mit (3,151) aus
 *    2070: Slot 4 = Text msg 2 im Band 1, Aktion vom Steg (28457,-1800,1319) Blick +x zeigt nur msg 2 ("It's locked
 *    from the other side.", ROOM11A0 msg @0x142C), kein Raumwechsel, (3,206) bleibt 0; Gegenprobe mit (3,206)=1: Slot 4 =
 *    Tuer nach Stage-Index 1 Raum 0x0A, Aktion -> Raumwechsel nach 20A0. */
static uint32_t le32(const uint8_t *p) { return (uint32_t)p[0] | ((uint32_t)p[1] << 8) | ((uint32_t)p[2] << 16) | ((uint32_t)p[3] << 24); }
static unsigned le16u(const uint8_t *p) { return (unsigned)p[0] | ((unsigned)p[1] << 8); }

typedef struct { uint16_t raum; uint32_t off; uint8_t art; } fund_t;   /* art: 8/9/10 = Tuer-Ziel, 206 = Set(3,206) */
static fund_t s_fund[64];
static int s_nfund = 0;

static uint32_t abschnitt_ende(const uint8_t *d, uint32_t n, uint32_t s)
{
    uint32_t boden = s;
    unsigned first = le16u(d + s);
    if (first >= 2 && !(first & 1) && s + first <= n) {
        unsigned mx = 0;
        for (unsigned i = 0; i < first / 2; i++) {
            unsigned o = le16u(d + s + 2 * i);
            if (o && s + o < n && o > mx) mx = o;
        }
        boden = s + mx;
    }
    uint32_t best = n;
    for (int o = 0x40; o < 0x60; o += 4) {
        uint32_t v = le32(d + o);
        if (v > boden && v <= n && v < best) best = v;
    }
    return best;
}

static void abschnitt_laufen(uint16_t raum, const uint8_t *d, uint32_t n, uint32_t s)
{
    if (s == 0 || s + 2 > n) return;
    unsigned first = le16u(d + s);
    if (first < 2 || (first & 1) || s + first > n) return;
    uint32_t se = abschnitt_ende(d, n, s);
    unsigned offs[64]; int k = 0;
    for (unsigned i = 0; i < first / 2 && k < 64; i++) {
        unsigned o = le16u(d + s + 2 * i);
        if (!o || s + o >= n) continue;
        int dup = 0;
        for (int j = 0; j < k; j++) if (offs[j] == o) dup = 1;
        if (!dup) offs[k++] = o;
    }
    for (int i = 0; i < k; i++)                       /* aufsteigend wie section_regions */
        for (int j = i + 1; j < k; j++) if (offs[j] < offs[i]) { unsigned t = offs[i]; offs[i] = offs[j]; offs[j] = t; }
    for (int i = 0; i < k; i++) {
        uint32_t a = s + offs[i], e = (i + 1 < k) ? s + offs[i + 1] : se;
        if (e > se) e = se;
        for (uint32_t pc = a; pc < e; ) {
            int sz = scd_opcode_size_at(d + pc);
            if (sz <= 0 || pc + (uint32_t)sz > e) break;
            uint8_t art = 0;
            if (d[pc] == 0x3B && sz >= 26 && d[pc + 22] == 1 && (d[pc + 23] == 8 || d[pc + 23] == 9 || d[pc + 23] == 0x0A))
                art = d[pc + 23];
            if (d[pc] == 0x22 && d[pc + 1] == 3 && d[pc + 2] == 206 && d[pc + 3] == 1) art = 206;
            if (art && s_nfund < 64) { s_fund[s_nfund].raum = raum; s_fund[s_nfund].off = pc; s_fund[s_nfund].art = art; s_nfund++; }
            pc += (uint32_t)sz;
        }
    }
}

static void fall_j1(void)
{
    static const fund_t soll[] = {
        { 0x2030, 0x01AEA, 8 }, { 0x2031, 0x01AEA, 8 }, { 0x2090, 0x00996, 8 }, { 0x2091, 0x00996, 8 },
        { 0x2080, 0x00BC2, 9 }, { 0x2081, 0x00BC2, 9 }, { 0x20A0, 0x01746, 9 }, { 0x20A1, 0x01746, 9 },
        { 0x11A0, 0x01006, 10 }, { 0x11A1, 0x01006, 10 }, { 0x2090, 0x009B6, 10 }, { 0x2091, 0x009B6, 10 },
        { 0x20A0, 0x01766, 10 }, { 0x20A1, 0x01766, 10 },
        { 0x20A0, 0x01FF0, 206 }, { 0x20A1, 0x01FF0, 206 },
    };
    const int nsoll = (int)(sizeof soll / sizeof soll[0]);
    s_nfund = 0;
    int raeume = 0;
    for (int r = 0; r < RE15_ROOM_COUNT; r++) {
        long sz = 0;
        uint8_t *d = rdt_lesen((uint16_t)re15_room_ids[r], &sz);
        if (!d) continue;
        if (sz >= 0x60) {
            raeume++;
            abschnitt_laufen((uint16_t)re15_room_ids[r], d, (uint32_t)sz, le32(d + 0x40));
            abschnitt_laufen((uint16_t)re15_room_ids[r], d, (uint32_t)sz, le32(d + 0x44));
        }
        free(d);
    }
    int alle = 1, fremd = 0;
    for (int i = 0; i < nsoll; i++) {
        int da = 0;
        for (int j = 0; j < s_nfund; j++)
            if (s_fund[j].raum == soll[i].raum && s_fund[j].off == soll[i].off && s_fund[j].art == soll[i].art) da = 1;
        if (!da) { alle = 0; printf("    fehlt: ROOM%04X @0x%05X art %d\n", soll[i].raum, (unsigned)soll[i].off, soll[i].art); }
    }
    for (int j = 0; j < s_nfund; j++) {
        int bek = 0;
        for (int i = 0; i < nsoll; i++)
            if (s_fund[j].raum == soll[i].raum && s_fund[j].off == soll[i].off && s_fund[j].art == soll[i].art) bek = 1;
        if (!bek) { fremd++; printf("    fremd: ROOM%04X @0x%05X art %d\n", s_fund[j].raum, (unsigned)s_fund[j].off, s_fund[j].art); }
    }
    printf("  J1 Zensus: %d Raeume, %d Funde (Tueren nach 2080/2090/20A0 + Set(3,206)=1)\n", raeume, s_nfund);
    PRUEFE(raeume >= 200, "J1: RDTs gelesen (%d)", raeume);
    PRUEFE(alle && fremd == 0, "J1: Zugaenge 2080/2090/20A0 und Setzer von (3,206) genau wie der Tuergraph (%d fremd)", fremd);
    /* Bytes der Sperre (beide Szenarien gleich) */
    static const uint8_t ck[4]  = { 0x21, 0x03, 0xce, 0x00 };
    static const uint8_t txt[20] = { 0x2c, 0x04, 0x01, 0x31, 0x01, 0x00, 0x54, 0x6f, 0xc8, 0x00, 0xe8, 0x03, 0x98, 0x08,
                                     0x02, 0x00, 0xff, 0xff, 0x00, 0x00 };
    for (int v = 0; v < 2; v++) {
        long sz = 0;
        uint8_t *d = rdt_lesen((uint16_t)(0x11A0 + v), &sz);
        PRUEFE(d && sz > 0x1026 && memcmp(d + 0x00FEA, ck, 4) == 0 && memcmp(d + 0x00FEE, txt, 20) == 0 && d[0x01006] == 0x3B,
               "ROOM%04X main00: @0x00FEA Ck(3,206)=0, @0x00FEE Text slot 4 msg 2, @0x01006 Tuer (Sonst-Zweig)", 0x11A0 + v);
        free(d);
    }
}

static void fall_j2(uint16_t r11a0)
{
    /* Spieler auf dem Steg (Band 1, y -1800) vor der Ost-Tuer: Rechteck x 28500..29500 z 200..2400, Punkt 620 voraus */
    bits_leeren();
    vorflag(RE15_R41C_BANK, RE15_R41C_BIT_FLUT_GELAUFEN); vorflag(RE15_R41C_BANK, RE15_R41C_BIT_LEITER_FREI); vorflag(3, 151);
    if (raum_hoch(r11a0, 28457, -1800, 1319) != 0) { PRUEFE(0, "ROOM%04X Parse", r11a0); return; }
    const re15_aot_t *a = &g_aot.slots[4];
    PRUEFE(a->active && a->type == RE15_AOT_TYPE_MESSAGE && a->event_id == 2 && a->band == 1,
           "ROOM%04X nach der Flut: Slot 4 = Text msg 2 Band 1 (Typ %d ev %d Band %d)", r11a0, a->type, a->event_id, a->band);
    memset(&g_room_change, 0, sizeof g_room_change);
    lauf_t L = druecken(28457, 1319, 0, 0, 300); zeige("11A0 zu", r11a0, &L);
    PRUEFE(L.n_msgs == 1 && L.msgs[0] == 2 && !g_room_change.pending && re15_game_flag_get(3, 206) == 0,
           "ROOM%04X: Aktion zeigt nur msg 2 (locked from the other side), kein Raumwechsel, (3,206)=0 (msgs %d erste %d pending %d)",
           r11a0, L.n_msgs, L.n_msgs ? L.msgs[0] : -1, g_room_change.pending);
    /* Gegenprobe: mit (3,206)=1 ist Slot 4 die Tuer nach 20A0 */
    vorflag(3, 206);
    if (raum_hoch(r11a0, 28457, -1800, 1319) != 0) { PRUEFE(0, "ROOM%04X Parse", r11a0); return; }
    PRUEFE(g_aot.slots[4].type == RE15_AOT_TYPE_DOOR && g_aot.door_params[4].dest_stage == 1 && g_aot.door_params[4].dest_room == 0x0A,
           "ROOM%04X mit (3,206): Slot 4 Tuer nach 20A0 (Typ %d Stage %d Raum 0x%02X)", r11a0, g_aot.slots[4].type,
           g_aot.door_params[4].dest_stage, g_aot.door_params[4].dest_room);
    memset(&g_room_change, 0, sizeof g_room_change);
    L = druecken(28457, 1319, 0, 0, 60);
    PRUEFE(g_room_change.pending && (g_room_change.room_id & 0xFFF0u) == 0x20A0u,
           "ROOM%04X mit (3,206): Aktion -> Raumwechsel nach 20A0 (pending %d Raum %04X)", r11a0, g_room_change.pending,
           g_room_change.room_id);
}

/* ---- K: Ventilraetsel erst mit Bruecke UND Sicherung im Ventilkasten 2010 (Nachbesserung 5, Nachtraege 9/16) -------------- */
static void fall_k(uint16_t r)
{
    /* Bruecke oben, keine Sicherung: der Platz liegt am Sued-Landeplatz (Band 1), nicht mehr am Ring */
    bits_leeren(); vorflag(RE15_R41C_BANK, RE15_R41C_BIT_VENTIL13);
    if (raum_hoch(r, VX, -1800, VZ) != 0) { PRUEFE(0, "ROOM%04X Parse", r); return; }
    for (int k = 0; k < RE15_R41C_VENTILE_N; k++) {
        const re15_aot_t *a = &g_aot.slots[RE15_R41C_SLOT_VENTIL0 + k];
        PRUEFE(a->active && a->type == RE15_AOT_TYPE_GENERIC && a->event_id == RE15_R41C_EV_VENTIL0 + k && a->band == RE15_R41C_VENTILE_BAND &&
               a->x == k_vx[k] && a->z == k_vz[k] && a->half_w == 500 && a->half_h == 500,
               "ROOM%04X: Ventil %02d Slot %d Mitte (%d,%d) Band 1 (ist (%d,%d) Band %d Ereignis %d)", r,
               re15_ventilkette_r41_ventil_nummer(k), RE15_R41C_SLOT_VENTIL0 + k, (int)k_vx[k], (int)k_vz[k], (int)a->x, (int)a->z,
               a->band, a->event_id);
    }
    lauf_t L = ventil(5, 0, 300); zeige("ohne 102", r, &L);
    PRUEFE(L.n_msgs == 1 && L.msgs[0] == RE15_R41C_MSG_VENTILE_VORHER && !re15_ventilkette_r41_timer_laeuft(),
           "ROOM%04X ohne Bit 102 und ohne Kasten: Satz 49, kein Zaehler (msgs %d, erste %d)", r, L.n_msgs, L.n_msgs ? L.msgs[0] : -1);
    /* der alte Ring-Platz (Ost-Streifen (6500,-200) Blick -x, Band 3) loest nichts mehr aus */
    g_actors[RE15_ACTOR_SLOT_PLAYER].y = -5400;
    L = druecken(6500, -200, 2048, 0, 120);
    PRUEFE(L.n_msgs == 0 && !re15_ventilkette_r41_timer_laeuft(), "ROOM%04X: am alten Ring-Platz kein Ventilraetsel (msgs %d)", r, L.n_msgs);
    g_actors[RE15_ACTOR_SLOT_PLAYER].y = -1800;
    /* Bruecke unten, Sicherung nur aufgehoben (Nachtrag 10 entfaellt mit Nachtrag 16): weiter Satz 49 */
    bits_leeren(); vorflag(RE15_R41C_BANK, RE15_R41C_BIT_VENTIL13); vorflag(RE15_R41C_BANK, RE15_R41C_BIT_BRUECKE_UNTEN);
    if (raum_hoch(r, VX, -1800, VZ) != 0) { PRUEFE(0, "Parse"); return; }
    L = ventil(0, 0, 300); zeige("ohne 108", r, &L);
    PRUEFE(L.n_msgs == 1 && L.msgs[0] == RE15_R41C_MSG_VENTILE_VORHER && !re15_ventilkette_r41_timer_laeuft(),
           "ROOM%04X Bit 102 ohne Sicherung: Satz 49, kein Zaehler", r);
    /* Sicherung aufheben = Original Slot 9 (@0x01BAA, Ereignis 6) -> sub06: msg 2 (Frage), Ja -> Set(3,108) @0x01FF2, msg 3.
     * Rechteck x -5900..-5100 z 6650..7650 Band 1; Spieler (-5350,6400) Blick +z -> Punkt (-5350,7020). */
    PRUEFE(g_aot.slots[9].active && g_aot.slots[9].type == RE15_AOT_TYPE_GENERIC && g_aot.slots[9].event_id == 6,
           "ROOM%04X: Sicherung Slot 9 = Ereignis 6 (Typ %d ev %d)", r, g_aot.slots[9].type, g_aot.slots[9].event_id);
    L = druecken(-5350, 6400, 3072, 0, 400); zeige("sicherung", r, &L);
    PRUEFE(L.n_msgs == 2 && L.msgs[0] == 2 && L.msgs[1] == 3 && sicherung_bit() == 1,
           "ROOM%04X: Sicherung aufgehoben (msg 2 + 3, (3,108)=%d)", r, sicherung_bit());
    /* Nachtrag 16: die aufgehobene Sicherung allein schaltet die Ventile nicht frei -> weiter "It's locked." */
    L = ventil(5, 0, 300); zeige("nur 108", r, &L);
    PRUEFE(L.n_msgs == 1 && L.msgs[0] == RE15_R41C_MSG_VENTILE_VORHER && re15_ventilkette_r41_drehungen() == 0,
           "ROOM%04X Bit 102 + (3,108) ohne Ventilkasten: Satz 49 (msgs %d, erste %d)", r, L.n_msgs, L.n_msgs ? L.msgs[0] : -1);
    /* mit der Sicherung im Ventilkasten 2010 (Bit 125): die Ventil-Fragen (Nein -> nichts) */
    bits_leeren(); vorflag(RE15_R41C_BANK, RE15_R41C_BIT_VENTIL13); vorflag(RE15_R41C_BANK, RE15_R41C_BIT_BRUECKE_UNTEN);
    vorflag(RE15_R41C_SICHERUNG_BANK, RE15_R41C_SICHERUNG_BIT); vorflag(RE15_R41C_BANK, RE15_R41C_BIT_KASTEN);
    if (raum_hoch(r, VX, -1800, VZ) != 0) { PRUEFE(0, "Parse"); return; }
    L = ventil(5, 1, 300); zeige("mit 125", r, &L);
    PRUEFE(L.n_msgs == 1 && L.msgs[0] == RE15_R41C_MSG_VENTIL0 + 5 && !re15_ventilkette_r41_timer_laeuft(),
           "ROOM%04X Bit 102 + Bit 125: Frage 55 an Ventil 09 (msgs %d, erste %d)", r, L.n_msgs, L.n_msgs ? L.msgs[0] : -1);
    L = ventil(1, 1, 300);
    PRUEFE(L.n_msgs == 1 && L.msgs[0] == RE15_R41C_MSG_VENTIL0 + 1, "ROOM%04X: Frage 51 an Ventil 02 (Nordwand)", r);
    /* Kasten-Sicherung ohne Bruecke (nicht erreichbar, aber eindeutig): Satz 49 (Nachtrag 9 bleibt) */
    bits_leeren(); vorflag(RE15_R41C_SICHERUNG_BANK, RE15_R41C_SICHERUNG_BIT); vorflag(RE15_R41C_BANK, RE15_R41C_BIT_KASTEN);
    if (raum_hoch(r, VX, -1800, VZ) != 0) { PRUEFE(0, "Parse"); return; }
    L = ventil(2, 0, 300);
    PRUEFE(L.n_msgs == 1 && L.msgs[0] == RE15_R41C_MSG_VENTILE_VORHER, "ROOM%04X Bit 125 ohne Bit 102: Satz 49 (Ventil 03)", r);
}

/* ---- N: Ventil-Sicherungskasten ROOM2010/2011 (Nachbesserung 5, Nachtrag 16) ------------------------------------------ */
#define KX 4950
#define KZ (-2770)
static void fall_n(uint16_t r)
{
    bits_leeren();
    if (raum_hoch(r, KX, 0, KZ) != 0) { PRUEFE(0, "ROOM%04X Parse", r); return; }
    const re15_aot_t *a = &g_aot.slots[RE15_R41C_SLOT_KASTEN];
    PRUEFE(a->active && a->type == RE15_AOT_TYPE_GENERIC && a->event_id == RE15_R41C_EV_KASTEN && a->band == RE15_R41C_KASTEN_BAND &&
           a->x == 5400 && a->z == -2770 && a->half_w == 500 && a->half_h == 700,
           "ROOM%04X: Ventilkasten Slot 8 = Ereignis 20, Mitte (5400,-2770) 1000 x 1400 Band 0 (ist (%d,%d) %d x %d ev %d)", r,
           (int)a->x, (int)a->z, 2 * (int)a->half_w, 2 * (int)a->half_h, a->event_id);
    /* ohne aufgehobene Sicherung: nur Text 40 (2 Seiten), kein Bit */
    lauf_t L = druecken(KX, KZ, 0, 0, 300); zeige("leer", r, &L);
    PRUEFE(L.n_msgs == 1 && L.msgs[0] == RE15_R41C_MSG_KASTEN_LEER && bit(RE15_R41C_BIT_KASTEN) == 0 &&
           !re15_ventilkette_r41_standbild(NULL, NULL), "ROOM%04X ohne (3,108): nur Text 40 (msgs %d)", r, L.n_msgs);
    /* mit (3,108): Nein -> nichts */
    bits_leeren(); vorflag(RE15_R41C_SICHERUNG_BANK, RE15_R41C_SICHERUNG_BIT);
    if (raum_hoch(r, KX, 0, KZ) != 0) { PRUEFE(0, "Parse"); return; }
    L = druecken(KX, KZ, 0, 1, 300); zeige("nein", r, &L);
    PRUEFE(L.n_msgs == 2 && L.msgs[0] == RE15_R41C_MSG_KASTEN_LEER && L.msgs[1] == RE15_R41C_MSG_KASTEN_FRAGE && bit(RE15_R41C_BIT_KASTEN) == 0,
           "ROOM%04X Nein: Text 40 + Frage 41, Bit 125 bleibt 0", r);
    /* Ja: Bit 125, Standbild der Ventile (2030 Cut 7) + Aufschliess-Ton; das Programm wartet, bis bestaetigt wird */
    const unsigned auf0 = re15_ventilkette_r41_ton_zaehler(RE15_R41C_TON_AUF);
    L = druecken(KX, KZ, 0, 0, 200); zeige("ja", r, &L);
    unsigned raum = 0; int cut = -1;
    PRUEFE(L.n_msgs == 2 && L.msgs[1] == RE15_R41C_MSG_KASTEN_FRAGE && bit(RE15_R41C_BIT_KASTEN) == 1 &&
           re15_ventilkette_r41_standbild(&raum, &cut) == 1 && raum == RE15_R41C_STANDBILD_RAUM && cut == RE15_R41C_STANDBILD_CUT &&
           re15_ventilkette_r41_ton_zaehler(RE15_R41C_TON_AUF) == auf0 + 1 && re15_game_flag_get(2, 7) == 1 && !g_scd.message_active,
           "ROOM%04X Ja: Bit 125, Standbild ROOM%04X Cut %d offen, 1 Aufschliess-Ton, Pad-Sperre, noch kein Text 42", r, raum, cut);
    for (int i = 0; i < 20; i++) { g_scd_pad_edge = 0; takt(); }
    PRUEFE(re15_ventilkette_r41_standbild(NULL, NULL) == 1, "ROOM%04X: Standbild bleibt ohne Taste offen", r);
    g_scd_pad_edge = 0x4000; takt(); g_scd_pad_edge = 0;
    int msg42 = 0, frei = -1;
    for (int i = 0; i < 400; i++) {
        if (g_scd.message_active && g_scd.message_id == RE15_R41C_MSG_KASTEN_BENUTZT) msg42 = 1;
        g_scd_pad_edge = (g_scd.message_active && (i & 7) == 7) ? 0x4000 : 0;
        takt(); g_scd_pad_edge = 0;
        if (msg42 && !g_scd.message_active && re15_game_flag_get(2, 7) == 0) { frei = i; break; }
    }
    PRUEFE(re15_ventilkette_r41_standbild(NULL, NULL) == 0 && msg42 && frei >= 0 && re15_game_flag_get(5, RE15_R41C_FLAG_STANDBILD) == 0,
           "ROOM%04X: Bestaetigen schliesst das Standbild, danach Text 42, Spieler frei (Takt %d)", r, frei);
    /* steckt sie (Bit 125): Text 43 (Frage) -> Ja -> Bit 125 = 0, Text 44 */
    bits_leeren(); vorflag(RE15_R41C_SICHERUNG_BANK, RE15_R41C_SICHERUNG_BIT); vorflag(RE15_R41C_BANK, RE15_R41C_BIT_KASTEN);
    if (raum_hoch(r, KX, 0, KZ) != 0) { PRUEFE(0, "Parse"); return; }
    L = druecken(KX, KZ, 0, 0, 400); zeige("raus", r, &L);
    PRUEFE(L.n_msgs == 2 && L.msgs[0] == RE15_R41C_MSG_KASTEN_DRIN && L.msgs[1] == RE15_R41C_MSG_KASTEN_GENOMMEN && bit(RE15_R41C_BIT_KASTEN) == 0,
           "ROOM%04X: Sicherung herausnehmen (43 -> Ja -> 44), Bit 125 = 0 (msgs %d)", r, L.n_msgs);
    /* im Generator 2060 ((3,144)): nur Text 40 */
    bits_leeren(); vorflag(RE15_R41C_SICHERUNG_BANK, RE15_R41C_SICHERUNG_BIT); vorflag(RE15_R41C_SICHERUNG_BANK, RE15_R41C_GENERATOR_BIT);
    if (raum_hoch(r, KX, 0, KZ) != 0) { PRUEFE(0, "Parse"); return; }
    L = druecken(KX, KZ, 0, 0, 300);
    PRUEFE(L.n_msgs == 1 && L.msgs[0] == RE15_R41C_MSG_KASTEN_LEER && bit(RE15_R41C_BIT_KASTEN) == 0,
           "ROOM%04X: Sicherung im Generator (3,144) -> nur Text 40", r);
    /* nach der Flut (Bits 103 + 100) ist der Kasten weiter da (Nachtrag 16 Folge 3: Herausnehmen jederzeit) */
    bits_leeren(); vorflag(RE15_R41C_BANK, RE15_R41C_BIT_FLUT_GELAUFEN); vorflag(RE15_R41C_BANK, RE15_R41C_BIT_LEITER_FREI);
    vorflag(RE15_R41C_SICHERUNG_BANK, RE15_R41C_SICHERUNG_BIT); vorflag(RE15_R41C_BANK, RE15_R41C_BIT_KASTEN);
    if (raum_hoch(r, KX, 0, KZ) != 0) { PRUEFE(0, "Parse"); return; }
    L = druecken(KX, KZ, 0, 0, 400);
    PRUEFE(L.n_msgs == 2 && L.msgs[1] == RE15_R41C_MSG_KASTEN_GENOMMEN && bit(RE15_R41C_BIT_KASTEN) == 0 &&
           g_aot.door_params[0].dest_room == 0x0B, "ROOM%04X nach der Flut: Sicherung heraus (Bit 125 = 0), Tuer -> 20B0", r);
}

/* ---- O: Generator-Kasten 2060/2061 mit der Sicherung im Ventilkasten (Nachbesserung 5, Nachtrag 16 Folge 3) -------- */
static void fall_o(uint16_t r)
{
    bits_leeren(); vorflag(RE15_R41C_SICHERUNG_BANK, RE15_R41C_SICHERUNG_BIT); vorflag(RE15_R41C_BANK, RE15_R41C_BIT_KASTEN);
    if (raum_hoch(r, 0, 0, 0) != 0) { PRUEFE(0, "ROOM%04X Parse", r); return; }
    PRUEFE(g_aot.slots[RE15_R41C_SLOT_2060_KASTEN].type == RE15_AOT_TYPE_MESSAGE &&
           g_aot.slots[RE15_R41C_SLOT_2060_KASTEN].event_id == RE15_R41C_MSG_2060_FEHLT,
           "ROOM%04X Bit 125: Generator-Kasten Slot 9 = Text 3 (Typ %d ev %d)", r, g_aot.slots[9].type, g_aot.slots[9].event_id);
    bits_leeren(); vorflag(RE15_R41C_SICHERUNG_BANK, RE15_R41C_SICHERUNG_BIT);
    if (raum_hoch(r, 0, 0, 0) != 0) { PRUEFE(0, "Parse"); return; }
    PRUEFE(g_aot.slots[9].type == RE15_AOT_TYPE_GENERIC && g_aot.slots[9].event_id == 0x12,
           "ROOM%04X ohne Bit 125: Original Ereignis 18 (@0x010C6; Typ %d ev %d)", r, g_aot.slots[9].type, g_aot.slots[9].event_id);
    bits_leeren(); vorflag(RE15_R41C_SICHERUNG_BANK, RE15_R41C_SICHERUNG_BIT); vorflag(RE15_R41C_SICHERUNG_BANK, RE15_R41C_GENERATOR_BIT);
    if (raum_hoch(r, 0, 0, 0) != 0) { PRUEFE(0, "Parse"); return; }
    PRUEFE(g_aot.slots[9].type == RE15_AOT_TYPE_MESSAGE && g_aot.slots[9].event_id == 6,
           "ROOM%04X mit (3,144): Original Text 6 (@0x010DE)", r);
}

/* ---- P: Plakat und Flut-Sperre (Abnahme 4 Mangel 2) --------------------------------------------------------------- */
static void fall_p(uint16_t r)
{
    bits_leeren();
    vorflag(RE15_R41C_BANK, RE15_R41C_BIT_VENTIL13); vorflag(RE15_R41C_BANK, RE15_R41C_BIT_BRUECKE_UNTEN);
    vorflag(RE15_R41C_SICHERUNG_BANK, RE15_R41C_SICHERUNG_BIT); vorflag(RE15_R41C_BANK, RE15_R41C_BIT_KASTEN);
    if (raum_hoch(r, VX, -1800, VZ) != 0) { PRUEFE(0, "ROOM%04X Parse", r); return; }
    /* Plakat offen, dann setzt das Flut-Programm seine Sperre (Form ROOM2030 sub02 @0x01E30) und den Marker (5,0x1E) */
    lauf_t L = druecken(-3650, 7280, 3072, 0, 20);
    PRUEFE(re15_ventilkette_r41_plakat_offen() == 1, "ROOM%04X: Plakat offen", r);
    re15_game_flag_set(2, 7, 1); re15_game_flag_set(2, 2, 1); re15_game_flag_set(5, RE15_R41C_FLAG_FLUT, 1);
    for (int i = 0; i < 5; i++) { g_scd_pad_edge = 0; takt(); }
    PRUEFE(re15_ventilkette_r41_flut_laeuft() == 1, "ROOM%04X: Flut laeuft", r);
    g_scd_pad_edge = 0x4000; takt(); g_scd_pad_edge = 0; takt();
    PRUEFE(re15_ventilkette_r41_plakat_offen() == 0 && re15_game_flag_get(2, 7) == 1 && re15_game_flag_get(2, 2) == 1,
           "ROOM%04X: Plakat zu in der Flut -> Pad-Sperre (2,7)/(2,2) der Flut bleibt ((2,7)=%d)", r, re15_game_flag_get(2, 7));
    /* waehrend des Zaehlers oeffnet das Plakat nicht (der Spieler soll fliehen) */
    bits_leeren();
    vorflag(RE15_R41C_BANK, RE15_R41C_BIT_VENTIL13); vorflag(RE15_R41C_BANK, RE15_R41C_BIT_BRUECKE_UNTEN);
    vorflag(RE15_R41C_SICHERUNG_BANK, RE15_R41C_SICHERUNG_BIT); vorflag(RE15_R41C_BANK, RE15_R41C_BIT_KASTEN);
    if (raum_hoch(r, VX, -1800, VZ) != 0) { PRUEFE(0, "Parse"); return; }
    L = ventile_richtig(600);
    PRUEFE(re15_ventilkette_r41_timer_laeuft(), "ROOM%04X: Zaehler laeuft", r);
    L = druecken(-3650, 7280, 3072, 0, 40);
    PRUEFE(re15_ventilkette_r41_plakat_offen() == 0 && re15_game_flag_get(2, 7) == 0 && re15_game_flag_get(5, RE15_R41C_FLAG_PLAKAT) == 0,
           "ROOM%04X: waehrend des Zaehlers kein Plakat (offen %d)", r, re15_ventilkette_r41_plakat_offen());
    (void)L;
}

/* ---- Q: Ventil-Dreh-Animation = RE2 ROOM1080 sub08 (Nachbesserung 5, Nachtrag 18 Punkt 3) -------------------------------- */
static void fall_q(uint16_t r)
{
    bits_leeren();
    vorflag(RE15_R41C_BANK, RE15_R41C_BIT_VENTIL13); vorflag(RE15_R41C_BANK, RE15_R41C_BIT_BRUECKE_UNTEN);
    vorflag(RE15_R41C_SICHERUNG_BANK, RE15_R41C_SICHERUNG_BIT); vorflag(RE15_R41C_BANK, RE15_R41C_BIT_KASTEN);
    for (int k = 0; k < 2; k++) {               /* Suedwand 09 (k 5) und Nordwand 01 (k 0) */
        const int vk = k ? 0 : 5;
        const int nord = k_vz[vk] < 0;
        if (raum_hoch(r, k_vx[vk], -1800, nord ? k_vz[vk] + 600 : k_vz[vk] - 600) != 0) { PRUEFE(0, "Parse"); return; }
        const unsigned ton0 = re15_ventilkette_r41_ton_zaehler(RE15_R41C_TON_VENTIL);
        re15_actor_t *p = &g_actors[RE15_ACTOR_SLOT_PLAYER];
        p->x = k_vx[vk] + 300; p->z = nord ? k_vz[vk] + 600 : k_vz[vk] - 600; p->rot_y = (int16_t)(nord ? 1024 : 3072);
        g_aot.fired_event_id_this_frame = 0; g_aot_action_pressed = 1;
        re15_aot_scan(p->x, p->z, 0xFF); g_aot_action_pressed = 0;
        if (g_aot.fired_event_id_this_frame) (void)scd_event_fire(g_aot.fired_event_id_this_frame);
        int folge[16], nf = 0, ende = -1, letzte = -1;
        for (int i = 0; i < 900; i++) {
            g_scd_pad_edge = (g_scd.message_active && (i & 7) == 7) ? 0x4000 : 0;
            takt(); g_scd_pad_edge = 0;
            const int m = p->motion | ((p->anim_flags & 0xC0) << 8);
            if (p->state == 4 && m != letzte && nf < 16) { folge[nf++] = m; letzte = m; }
            if (re15_ventilkette_r41_drehungen() == 1) { ende = i; break; }
        }
        static const int soll[5] = { 15, 16, 17, 16 | 0xC000, 15 | 0xC000 };
        int ok = nf == 5;
        for (int i = 0; ok && i < 5; i++) if (folge[i] != soll[i]) ok = 0;
        printf("  ROOM%04X Q Ventil %02d: Clips", r, re15_ventilkette_r41_ventil_nummer(vk));
        for (int i = 0; i < nf; i++) printf(" %d%s", folge[i] & 0xFF, (folge[i] & 0xC000) ? "r" : "");
        printf("  Ende Takt %d  Lage (%d,%d,%d) rot %d  Toene %u\n", ende, (int)p->x, (int)p->y, (int)p->z, (int)p->rot_y,
               re15_ventilkette_r41_ton_zaehler(RE15_R41C_TON_VENTIL) - ton0);
        PRUEFE(ok, "ROOM%04X Ventil %02d: Clips 15 -> 16 -> 17 -> 16r -> 15r (RE2 ROOM1080 sub08)", r, re15_ventilkette_r41_ventil_nummer(vk));
        PRUEFE(p->x == k_vx[vk] && p->z == (nord ? k_vz[vk] + RE15_R41C_DREH_ABSTAND : k_vz[vk] - RE15_R41C_DREH_ABSTAND) &&
               p->y == -1800 && p->rot_y == (nord ? 1024 : 3072),
               "ROOM%04X Ventil %02d: Spieler auf der Radmitte, 816 vor der Wand, Blick zur Wand", r, re15_ventilkette_r41_ventil_nummer(vk));
        PRUEFE(re15_ventilkette_r41_ton_zaehler(RE15_R41C_TON_VENTIL) - ton0 == 6 && ende > 300 && re15_game_flag_get(2, 7) == 0,
               "ROOM%04X Ventil %02d: 6 Ventil-Toene, Drehung erst nach der Animation vermerkt (Takt %d), Spieler frei", r,
               re15_ventilkette_r41_ventil_nummer(vk), ende);
    }
}

/* ---- M: Nachbesserung 4 (Nachtrag 13): Reihenfolge, Zuruecksetzen, Dokument, Plakat --------------------------------- */
static void fall_m(uint16_t r)
{
    bits_leeren();
    vorflag(RE15_R41C_BANK, RE15_R41C_BIT_VENTIL13); vorflag(RE15_R41C_BANK, RE15_R41C_BIT_BRUECKE_UNTEN);
    vorflag(RE15_R41C_SICHERUNG_BANK, RE15_R41C_SICHERUNG_BIT);
    vorflag(RE15_R41C_BANK, RE15_R41C_BIT_KASTEN);   /* Nachbesserung 5 (Nachtrag 16) */
    if (raum_hoch(r, VX, -1800, VZ) != 0) { PRUEFE(0, "ROOM%04X Parse", r); return; }
    /* die Folge, wie das Plakat sie druckt (09, 01, 02, 07) = falsch: keine Rueckmeldung je Schritt, nach der vierten zurueck */
    static const int plakat[4] = { 5, 0, 1, 3 };
    lauf_t L;
    for (int i = 0; i < 3; i++) {
        L = ventil(plakat[i], 0, 300);
        PRUEFE(L.n_msgs == 1 && L.msgs[0] == RE15_R41C_MSG_VENTIL0 + plakat[i] && re15_ventilkette_r41_drehungen() == i + 1 &&
               re15_game_flag_get(5, (uint8_t)(RE15_R41C_FLAG_DREHUNG0 + i)) == 1 && !re15_ventilkette_r41_timer_laeuft(),
               "ROOM%04X falsche Folge Schritt %d: nur Frage, Drehung %d vermerkt (Drehungen %d)", r, i + 1, i + 1, re15_ventilkette_r41_drehungen());
    }
    L = ventil(plakat[3], 0, 400); zeige("falsch 4", r, &L);
    PRUEFE(L.n_msgs == 2 && L.msgs[0] == RE15_R41C_MSG_VENTIL0 + 3 && L.msgs[1] == RE15_R41C_MSG_VENTILE_ZURUECK,
           "ROOM%04X falsche Folge 09/01/02/07: vierte Frage + Text 45 (msgs %d, zweite %d)", r, L.n_msgs, L.n_msgs > 1 ? L.msgs[1] : -1);
    PRUEFE(re15_ventilkette_r41_drehungen() == 0 && re15_game_flag_get(5, 0x10) == 0 && re15_game_flag_get(5, 0x13) == 0 &&
           !re15_ventilkette_r41_timer_laeuft() && re15_game_flag_get(5, RE15_R41C_FLAG_TIMER) == 0 && bit(RE15_R41C_BIT_FLUT_GELAUFEN) == 0,
           "ROOM%04X falsche Folge: (5,0x10..0x13) zurueck, kein Zaehler, keine Flut", r);
    /* ein falsches Ventil mitten in der richtigen Folge: zurueck erst nach vier */
    L = ventil(5, 0, 300); L = ventil(3, 0, 300); L = ventil(4, 0, 300);
    PRUEFE(re15_ventilkette_r41_drehungen() == 3, "ROOM%04X: 09, 07, 08 = drei Drehungen vermerkt", r);
    L = ventil(0, 0, 400);
    PRUEFE(L.n_msgs == 2 && L.msgs[1] == RE15_R41C_MSG_VENTILE_ZURUECK && re15_ventilkette_r41_drehungen() == 0 && !re15_ventilkette_r41_timer_laeuft(),
           "ROOM%04X: 09/07/08/01 falsch -> Text 45, zurueck", r);
    /* Nein vermerkt nichts */
    L = ventil(5, 1, 300);
    PRUEFE(re15_ventilkette_r41_drehungen() == 0, "ROOM%04X: Nein vermerkt keine Drehung", r);
    /* richtige Folge -> 46 + Zaehler; die Folge im Protokoll */
    L = ventile_richtig(400); zeige("richtig", r, &L);
    PRUEFE(L.n_msgs == 2 && L.msgs[1] == RE15_R41C_MSG_WARNUNG && re15_ventilkette_r41_timer_laeuft() &&
           re15_ventilkette_r41_folge(0) == 5 && re15_ventilkette_r41_folge(1) == 3 && re15_ventilkette_r41_folge(2) == 1 &&
           re15_ventilkette_r41_folge(3) == 0,
           "ROOM%04X richtige Folge 09/07/02/01 -> Warnung 46 + Zaehler", r);
    /* Dokument: Item-Zone Slot 20 (Item 0x4E, Bit 107, Weltmodell obj 6) neben der Sicherung -> der Leser wird angefordert */
    bits_leeren();
    vorflag(RE15_R41C_BANK, RE15_R41C_BIT_VENTIL13); vorflag(RE15_R41C_BANK, RE15_R41C_BIT_BRUECKE_UNTEN);
    if (raum_hoch(r, -4850, -1800, 7280) != 0) { PRUEFE(0, "Parse"); return; }
    const re15_aot_t *dz = &g_aot.slots[RE15_R41C_SLOT_DOKUMENT];
    const re15_aot_item_params_t *ip = &g_aot.item_params[RE15_R41C_SLOT_DOKUMENT];
    int pd = prop_von_obj(RE15_R41C_DOK_OBJ);
    PRUEFE(dz->active && dz->type == RE15_AOT_TYPE_ITEM && ip->item_type == RE15_R41C_DOK_ITEM && ip->taken_bit == RE15_R41C_BIT_DOKUMENT &&
           ip->taken_prop == RE15_R41C_DOK_OBJ && dz->x == -4850 && dz->z == 7900 && dz->band == 1 &&
           pd >= 0 && g_scd.props[pd].x == RE15_R41C_DOK_X && g_scd.props[pd].y == RE15_R41C_DOK_Y && g_scd.props[pd].z == RE15_R41C_DOK_Z,
           "ROOM%04X: Dokument-Zone Slot 20 (Item 0x%02X Bit %d obj %d, Mitte (%d,%d)) + Blatt obj 6", r, ip->item_type, ip->taken_bit,
           ip->taken_prop, (int)dz->x, (int)dz->z);
    if ((r & 1) == 0) {   /* der Menue-Lauf bleibt ohne Menue-Takt angefordert -> nur im ersten (Leon-)Durchgang messbar */
        const int vorher = re15_menu_doc_active();
        L = druecken(-4850, 7280, 3072, 0, 30);   /* Nachbesserung 5: Zettel an der Suedwand, Blick +z */
        PRUEFE(vorher == 0 && re15_menu_doc_active() == 1,
               "ROOM%04X: Aktion am Blatt fordert den Dokument-Leser an (Item 0x4E -> Dokument 6; vorher %d)", r, vorher);
    }
    PRUEFE(re15_ventilkette_r41_obj_id(r, 2) == RE15_R41C_DOK_OBJ && re15_ventilkette_r41_obj_id(r, 3) == RE15_R41C_PLAKAT_OBJ,
           "ROOM%04X: main.c laedt Blatt (obj 6) und Aushang (obj 7)", r);
    { int n = 0, t = 0;
      const uint8_t *m = re15_ventilkette_r41_md1_bytes_k(2, &n); const uint8_t *tb = re15_ventilkette_r41_tim_bytes_k(3, &t);
      re15_md1_t md; memset(&md, 0, sizeof md);
      PRUEFE(m && tb && t > 20 && re15_md1_parse(m, n, &md) == 0 && md.mesh_count == 1 && md.meshes[0].quad_count == 1 &&
             md.meshes[0].quad_normals[0].z == -4096,
             "Dokument-Zettel (Nachbesserung 5): ein aufrechtes Viereck, Normale (0,0,-4096) zur Kamera + Aushang-TIM");
      m = re15_ventilkette_r41_md1_bytes_k(3, &n); memset(&md, 0, sizeof md);
      PRUEFE(m && re15_md1_parse(m, n, &md) == 0 && md.mesh_count == 1 && md.meshes[0].quad_count == 1 && md.meshes[0].quad_normals[0].z == -4096,
             "Aushang-MD1: ein Viereck, Normale (0,0,-4096)"); }
    /* mit Bit 107 (genommen): weder Zone noch Blatt */
    vorflag(RE15_R41C_BANK, RE15_R41C_BIT_DOKUMENT);
    if (raum_hoch(r, -4850, -1800, 7280) != 0) { PRUEFE(0, "Parse"); return; }
    PRUEFE(!g_aot.slots[RE15_R41C_SLOT_DOKUMENT].active && prop_von_obj(RE15_R41C_DOK_OBJ) < 0, "ROOM%04X mit Bit 107: kein Dokument mehr", r);
    /* Plakat: Slot 21 vor dem Aushang (Suedwand) -> Nahaufnahme offen, Pad-Sperre + KI steht; Bestaetigen schliesst */
    int pp = prop_von_obj(RE15_R41C_PLAKAT_OBJ);
    PRUEFE(pp >= 0 && g_scd.props[pp].x == RE15_R41C_PLAKAT_X && g_scd.props[pp].y == RE15_R41C_PLAKAT_Y && g_scd.props[pp].z == RE15_R41C_PLAKAT_Z,
           "ROOM%04X: Aushang obj 7 an der Suedwand (-3650,-3900,7880)", r);
    /* Kletter-Sonde (@0x8002d370 Objekt-Band == Spielerband) und Standtest (@0x80031cfc Band + 1 == Spielerband) duerfen die
     * Blaetter am Landeplatz (Spielerband 1) nicht greifen: Band 2 (gemessen: mit Band 1 stieg Leon auf das Plakat). */
    PRUEFE(pp >= 0 && g_scd.props[pp].band == RE15_R41C_BLATT_BAND && RE15_R41C_BLATT_BAND != 1 && RE15_R41C_BLATT_BAND + 1 != 1,
           "ROOM%04X: Aushang Band %d (nicht kletterbar vom Landeplatz)", r, pp >= 0 ? (int)g_scd.props[pp].band : -1);
    L = druecken(-3650, 7280, 3072, 0, 20);
    PRUEFE(re15_ventilkette_r41_plakat_offen() == 1 && re15_game_flag_get(2, 7) == 1 && re15_game_flag_get(2, 2) == 1,
           "ROOM%04X: Plakat-Nahaufnahme offen, Pad-Sperre (2,7) + KI (2,2)", r);
    { const uint32_t *b = re15_ventilkette_r41_plakat_rgba();
      PRUEFE(b != NULL && b[120 * 320 + 160] != 0 && (b[0] & 0xFFu) == 0xFFu, "ROOM%04X: Nahaufnahme-Bild 320 x 240 liegt vor", r); }
    for (int i = 0; i < 10; i++) { g_scd_pad_edge = 0; takt(); }
    PRUEFE(re15_ventilkette_r41_plakat_offen() == 1, "ROOM%04X: Plakat bleibt offen ohne Taste", r);
    g_scd_pad_edge = 0x4000; takt(); g_scd_pad_edge = 0; takt();
    PRUEFE(re15_ventilkette_r41_plakat_offen() == 0 && re15_game_flag_get(2, 7) == 0 && re15_game_flag_get(2, 2) == 0 &&
           re15_game_flag_get(5, RE15_R41C_FLAG_PLAKAT) == 0 && re15_ventilkette_r41_plakat_rgba() == NULL,
           "ROOM%04X: Bestaetigen schliesst die Nahaufnahme, Pad frei", r);
}

/* ---- L: Zensus der Original-Bits (3,126) Strom und (3,108) Sicherung + SCA-Belege der Relinghoehe -------------------- */
typedef struct { uint16_t raum; uint32_t off; uint8_t op, bit, wert; } bitfund_t;
static bitfund_t s_bf[64];
static int s_nbf = 0;
static void bits_laufen(uint16_t raum, const uint8_t *d, uint32_t n, uint32_t s0)
{
    if (s0 == 0 || s0 + 2 > n) return;
    unsigned first = le16u(d + s0);
    if (first < 2 || (first & 1) || s0 + first > n) return;
    uint32_t se = abschnitt_ende(d, n, s0);
    unsigned offs[64]; int k = 0;
    for (unsigned i = 0; i < first / 2 && k < 64; i++) {
        unsigned o = le16u(d + s0 + 2 * i);
        if (!o || s0 + o >= n) continue;
        int dup = 0;
        for (int j = 0; j < k; j++) if (offs[j] == o) dup = 1;
        if (!dup) offs[k++] = o;
    }
    for (int i = 0; i < k; i++)
        for (int j = i + 1; j < k; j++) if (offs[j] < offs[i]) { unsigned t = offs[i]; offs[i] = offs[j]; offs[j] = t; }
    for (int i = 0; i < k; i++) {
        uint32_t a = s0 + offs[i], e = (i + 1 < k) ? s0 + offs[i + 1] : se;
        if (e > se) e = se;
        for (uint32_t pc = a; pc < e; ) {
            int sz = scd_opcode_size_at(d + pc);
            if (sz <= 0 || pc + (uint32_t)sz > e) break;
            if ((d[pc] == 0x21 || d[pc] == 0x22) && d[pc + 1] == 3 && (d[pc + 2] == 126 || d[pc + 2] == 108) && s_nbf < 64) {
                s_bf[s_nbf].raum = raum; s_bf[s_nbf].off = pc; s_bf[s_nbf].op = d[pc]; s_bf[s_nbf].bit = d[pc + 2]; s_bf[s_nbf].wert = d[pc + 3];
                s_nbf++;
            }
            pc += (uint32_t)sz;
        }
    }
}

static void fall_l(void)
{
    s_nbf = 0;
    for (int r = 0; r < RE15_ROOM_COUNT; r++) {
        long sz = 0;
        uint8_t *d = rdt_lesen((uint16_t)re15_room_ids[r], &sz);
        if (!d) continue;
        if (sz >= 0x60) {
            bits_laufen((uint16_t)re15_room_ids[r], d, (uint32_t)sz, le32(d + 0x40));
            bits_laufen((uint16_t)re15_room_ids[r], d, (uint32_t)sz, le32(d + 0x44));
        }
        free(d);
    }
    int set126 = 0, loesch126 = 0, set108 = 0, loesch108 = 0, ort126 = 1, ort108 = 1;
    for (int i = 0; i < s_nbf; i++) {
        if (s_bf[i].op != 0x22) continue;
        if (s_bf[i].bit == 126) {
            if (s_bf[i].wert == 1) { set126++; if ((s_bf[i].raum & 0xFFFEu) != 0x2060u || s_bf[i].off != 0x012AE) ort126 = 0; }
            else loesch126++;
        } else {
            if (s_bf[i].wert == 1) { set108++; if ((s_bf[i].raum & 0xFFFEu) != 0x2030u || s_bf[i].off != 0x01FF2) ort108 = 0; }
            else loesch108++;
        }
    }
    printf("  L Zensus: %d Stellen (3,126)/(3,108); Set(3,126) %d (geloescht %d), Set(3,108) %d (geloescht %d)\n",
           s_nbf, set126, loesch126, set108, loesch108);
    PRUEFE(set126 == 2 && ort126 && loesch126 == 0, "L: (3,126) nur ROOM2060/2061 sub01 @0x012AE gesetzt, nie geloescht");
    PRUEFE(set108 == 2 && ort108 && loesch108 == 0, "L: (3,108) nur ROOM2030/2031 sub06 @0x01FF2 gesetzt, nie geloescht");
    /* SCA-Belege der Relinghoehe (Kopf): untere Reling = Band-1-Wandzellen fl 0x11, Ring-Boden = fl 0x23 (Band 2..3) */
    bits_leeren();
    if (raum_hoch(0x2030, 0, -5400, 0) != 0) { PRUEFE(0, "ROOM2030 Parse"); return; }
    static const int k_reling[5] = { 3, 4, 5, 10, 11 };
    for (int i = 0; i < 5; i++)
        PRUEFE(s_rdt.sca[k_reling[i]].floor == 0x11 && s_rdt.sca[k_reling[i]].u1 == 0,
               "SCA 2030 Zelle %d: untere Reling Band 1 (fl %02x)", k_reling[i], s_rdt.sca[k_reling[i]].floor);
    for (int c = 36; c <= 41; c++)
        PRUEFE(s_rdt.sca[c].floor == 0x23 && s_rdt.sca[c].u1 == 1, "SCA 2030 Zelle %d: Ring-Boden fl 0x23 (fl %02x)", c, s_rdt.sca[c].floor);
    static const int k_ring[4][2] = { { 19, 5400 }, { 20, -8500 }, { 21, 8150 }, { 22, -5850 } };
    PRUEFE(s_rdt.sca[19].x == 5400 && s_rdt.sca[20].z + (int)s_rdt.sca[20].density == -8500 && s_rdt.sca[21].z == 8150 &&
           s_rdt.sca[22].x + (int)s_rdt.sca[22].width == -5850 && s_rdt.sca[19].floor == 0x33,
           "SCA 2030: Innenkanten der Ring-Reling = Rand der Schachtoeffnung (19 x %d, 20 z %d, 21 z %d, 22 x %d)",
           s_rdt.sca[19].x, s_rdt.sca[20].z + (int)s_rdt.sca[20].density, s_rdt.sca[21].z, s_rdt.sca[22].x + (int)s_rdt.sca[22].width);
    (void)k_ring;
    /* Nachbesserung 4 (Nachtrag 12 (k)): Absperrung = Band-3-Wandzellen fl 0x33 am inneren Ringrand, NW-Balkon 31/32 */
    PRUEFE(s_rdt.sca[31].floor == 0x33 && s_rdt.sca[32].floor == 0x33 && s_rdt.sca[31].z + (int)s_rdt.sca[31].density == -6400 &&
           s_rdt.sca[32].x + (int)s_rdt.sca[32].width == -5400 && s_rdt.sca[20].floor == 0x33 && s_rdt.sca[21].floor == 0x33 &&
           s_rdt.sca[22].floor == 0x33, "SCA 2030: Absperrung 19-22/31/32 fl 0x33 (Balkon z %d, x %d)",
           s_rdt.sca[31].z + (int)s_rdt.sca[31].density, s_rdt.sca[32].x + (int)s_rdt.sca[32].width);
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

/* Werkzeug "bss"/"bssdiff" (Nachbesserung 5, Riegel integration_r41_ventilkette_weg k1): die erste Kamera-Scheibe (0x10000 B)
 * einer BSS-Datei, dekodiert mit derselben Kette wie das Standbild der Ventile (ventilkette_r41_pc.c standbild_laden:
 * re15_bss_parse_chunk -> re15_bss_vlc_decode -> re15_bss_mdec_decode, 320 x 240 RGB). "bss" gibt das Flaechenmittel im
 * Format von "ppm" aus; "bssdiff" vergleicht einen Framedump Pixel fuer Pixel mit dem Bild: ABWEICHUNG = mittlere
 * Abweichung je Pixel ((|dr| + |dg| + |db|) / 3), GLEICH = Anteil der Pixel mit Abweichung <= 4 in Prozent. */
static uint8_t *bss_rgb(const char *pfad)
{
    long sz = 0;
    uint8_t *d = slurp(pfad, &sz);
    if (!d || sz < 0x10000) { free(d); return NULL; }
    re15_bss_chunk_t ch;
    uint8_t *aus = NULL;
    if (re15_bss_parse_chunk(d, 0x10000, &ch) && re15_bss_chunk_has_video(&ch)) {
        size_t cap = ((size_t)ch.run_length_words + 2) * 8 + 4096;
        int16_t *co = (int16_t *)malloc(cap * sizeof(int16_t));
        uint8_t *rgb = (uint8_t *)malloc((size_t)RE15_BSS_FRAME_WIDTH * RE15_BSS_FRAME_HEIGHT * 3);
        if (co && rgb) {
            int k = re15_bss_vlc_decode(ch.vlc_payload, ch.vlc_payload_size, ch.run_length_words, ch.quant, ch.version, co, cap);
            if (k >= 0 && re15_bss_mdec_decode(co, (size_t)k, RE15_BSS_FRAME_WIDTH, RE15_BSS_FRAME_HEIGHT, rgb) == 0) {
                aus = rgb; rgb = NULL;
            }
        }
        free(co); free(rgb);
    }
    free(d);
    return aus;
}

static int flaeche_ok(int x0, int y0, int x1, int y1)
{
    return !(x0 < 0 || y0 < 0 || x1 > RE15_BSS_FRAME_WIDTH || y1 > RE15_BSS_FRAME_HEIGHT || x0 >= x1 || y0 >= y1);
}

static int bss_mittel(const char *pfad, int x0, int y0, int x1, int y1)
{
    if (!flaeche_ok(x0, y0, x1, y1)) { printf("FEHLER: Flaeche\n"); return 2; }
    uint8_t *rgb = bss_rgb(pfad);
    if (!rgb) { printf("FEHLER: %s nicht lesbar/dekodierbar\n", pfad); return 2; }
    long su[3] = { 0, 0, 0 }, cnt = 0;
    for (int y = y0; y < y1; y++)
        for (int x = x0; x < x1; x++) {
            const uint8_t *px = rgb + ((long)y * RE15_BSS_FRAME_WIDTH + x) * 3;
            su[0] += px[0]; su[1] += px[1]; su[2] += px[2]; cnt++;
        }
    printf("MITTEL %ld %ld %ld (BSS %dx%d, Flaeche %d,%d..%d,%d)\n", su[0] / cnt, su[1] / cnt, su[2] / cnt,
           RE15_BSS_FRAME_WIDTH, RE15_BSS_FRAME_HEIGHT, x0, y0, x1, y1);
    free(rgb);
    return 0;
}

static int bss_diff(const char *bss, const char *ppm, int x0, int y0, int x1, int y1)
{
    if (!flaeche_ok(x0, y0, x1, y1)) { printf("FEHLER: Flaeche\n"); return 2; }
    uint8_t *rgb = bss_rgb(bss);
    if (!rgb) { printf("FEHLER: %s nicht lesbar/dekodierbar\n", bss); return 2; }
    long sz = 0;
    uint8_t *d = slurp(ppm, &sz);
    int w = 0, h = 0, mx = 0, n = 0;
    if (!d || sscanf((const char *)d, "P6 %d %d %d%n", &w, &h, &mx, &n) != 3 || w != RE15_BSS_FRAME_WIDTH ||
        h != RE15_BSS_FRAME_HEIGHT || mx != 255 || (long)n + 1 + (long)w * h * 3 > sz) {
        printf("FEHLER: %s kein P6 320x240/255\n", ppm); free(d); free(rgb); return 2; }
    const uint8_t *bild = d + n + 1;
    long summe = 0, gleich = 0, cnt = 0;
    for (int y = y0; y < y1; y++)
        for (int x = x0; x < x1; x++) {
            const uint8_t *a = rgb + ((long)y * w + x) * 3, *b = bild + ((long)y * w + x) * 3;
            const int dd = (abs(a[0] - b[0]) + abs(a[1] - b[1]) + abs(a[2] - b[2])) / 3;
            summe += dd; if (dd <= 4) gleich++; cnt++;
        }
    printf("ABWEICHUNG %ld GLEICH %ld (Pixel %ld, Flaeche %d,%d..%d,%d)\n", summe / cnt, (gleich * 100) / cnt, cnt,
           x0, y0, x1, y1);
    free(d); free(rgb);
    return 0;
}

int main(int argc, char **argv)
{
    if (argc == 7 && strcmp(argv[1], "ppm") == 0)
        return ppm_mittel(argv[2], atoi(argv[3]), atoi(argv[4]), atoi(argv[5]), atoi(argv[6]));
    if (argc == 7 && strcmp(argv[1], "bss") == 0)
        return bss_mittel(argv[2], atoi(argv[3]), atoi(argv[4]), atoi(argv[5]), atoi(argv[6]));
    if (argc == 8 && strcmp(argv[1], "bssdiff") == 0)
        return bss_diff(argv[2], argv[3], atoi(argv[4]), atoi(argv[5]), atoi(argv[6]), atoi(argv[7]));
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
        fall_j2((uint16_t)(0x11A0 + v));
        fall_k((uint16_t)(0x2030 + v));
        fall_m((uint16_t)(0x2030 + v));
        fall_n((uint16_t)(0x2010 + v));   /* Nachbesserung 5 */
        fall_o((uint16_t)(0x2060 + v));
        fall_p((uint16_t)(0x2030 + v));
        fall_q((uint16_t)(0x2030 + v));
    }
    /* fremd: ROOM2060 ohne Plaetze 12..14 / Wasser */
    bits_leeren();
    if (raum_hoch(0x2060, 0, 0, 0) == 0) {
        int n = 0;
        for (int s = 0; s < RE15_AOT_MAX; s++)
            if (g_aot.slots[s].active && g_aot.slots[s].type == RE15_AOT_TYPE_GENERIC &&
                (g_aot.slots[s].event_id == RE15_R41C_EV_PULT || g_aot.slots[s].event_id == RE15_R41C_EV_VENTIL13 ||
                 (g_aot.slots[s].event_id >= RE15_R41C_EV_VENTIL0 && g_aot.slots[s].event_id <= RE15_R41C_EV_PLAKAT))) n++;
        PRUEFE(n == 0 && re15_ventilkette_r41_wasser_prop(0) < 0, "ROOM2060: %d fremde Plaetze (obj 4 ist dort das Sicherungs-Panel)", n);
    }
    fall_h();
    fall_j1();
    fall_l();
    printf("%s (%d Pruefungen)\n", s_fail ? "FAIL" : "PASS", s_pruef);
    free(s_daten);
    return s_fail;
}
