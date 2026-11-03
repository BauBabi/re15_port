/*
 * test_r41_nachtrag30.c — Runde 41 Spur N "nachtrag30": Mechanik-Riegel der Nutzer-Punkte (echte RDTs, Leon + Elza, Raumaufbau wie test_r41_ventilkette.c).
 * Dossier analysis/befunde_runde41/N_nachtrag30.md.
 *   A  Panel 2030 (Nachtrag 37): Regel je Zug, Vollsuche aller Zustaende: 16 Staende, keine Sackgasse, kuerzeste Loesung A->B, B->C, C->A = 80
 *   B  Panel im Raum: Pult -> Frage 40 -> Ja -> Panel offen, Text 41; Zuege ueber Cursor + SQUARE; EXIT/CROSS setzen zurueck; 80 -> Bit 101 -> Frage 43 -> Ja -> Bruecke;
 *      die RE2-Toene der Bruecke (Satz 0x0F vor der Fahrt, 0x10 am Ende) kommen in dieser Reihenfolge; Ventil 13 gibt es nicht mehr
 *   C  Schrank 2010/2011: erster Klick klappt auf (16 Bilder, rot_y 1024), danach Text; zweiter Klick ohne Fahrt; nur unter Cut 3 gezeichnet; Ja-Zweig wie ROOM2060: Cut 8 + Cursor ohne Zeitgrenze,
 *      SQUARE neben der Zone nichts, Steuerkreuz +-200 je Bild mit Rahmen, Zone 7 + SQUARE -> Cut 9 + Bit 125 + Text 42, dann Standbild; START bleibt gesperrt; Raumwechsel schliesst
 *   C2 USE der Sicherung am geschlossenen Schrank (Integration N x O, N_abnahme_1.md Mangel 1): der USE-Lauf haelt ueber die 16 Bilder Aufklappen, kein Text, direkt der Cursor-Schritt ->
 *      Bit 125 -> Text 42 -> Standbild; zweite Sicherung aus ROOM1150 bei steckender Sicherung / bestuecktem Generator: USE gilt nicht ("You can't use it here.", RE2 @0x8006b248), Tuer zu und offen, Bit 125 unberuehrt (Z1, Dossier N OFFEN 6); die Aktionstaste zeigt Text 43 / 40
 *   D  Wasserlauf ROOM2000/2001: Strahlen + Lauf ueber den Weg (Effekt 0x0F wandert 100 je Bild +x bis vor die Kante, faellt dahinter) + Einschlaege, unter einem Text steht der Lauf,
 *      Leiter bleibt gesperrt; mit Bit 100 oder 103 nichts; 20B0 nichts; ROOM2010 hat keine Leiter
 *   E  Sirene: an mit dem Zaehler (nach der Ansage), aus mit der Flut, dem Tod und dem Verlassen des Raums
 *   F  Notiz + Plakat: FILE31 (FNV der Seiten), Loesung {5,1,0,4} = 09/02/01/08
 *   G  (Spur PF) Beschriftung Panel 2030 = 8x8-Spielschrift aus TEX.TIM Texel fuer Texel (FUN_80029214), Gegenprobe um 1 Bildpunkt versetzt
 *   Q  (Spur PF) Ja/Nein-Auswahl: keine Glyphe einer Port-Frage auf Cursor/"Yes"/"No" (Zeile 0xb4 + 0x10), Gegenprobe alte Id 40
 * Werkzeug: "bild <ppm> <x0> <y0> <x1> <y1>" = mittlere Helligkeit, "diff <a.ppm> <b.ppm> <x0> <y0> <x1> <y1>" = mittlere Abweichung, "endstufe <cap_a> <cap_b> <q>" = Endstufe bitgleich,
 * "fluss <ordner> <von> <bis> <x0> <y0> <x1> <y1>" = Bewegung in Bildern f_<n>.ppm: je Abstand 1..3 Bilder die beste Verschiebung in x des Bildes ohne sein Zeitmittel, Korrelation dort und bei 0, dazu Bildpunkte, die sich je 6 Bilder aendern,
 * "luecke <cap_a> <cap_b> <an> <aus> <schwelle>" = laengste Folge leiser Takte, (Spur PF) "etikett <ppm>" = Beschriftung im Framedump gegen die Spielschrift,
 * "kasten <ppm> <innen|tuer>" = Seitenverhaeltnis und Scherung des Schranks 2010 Cut 3 gegen ROOM2060 Cut 6, "karte <datei> <raum-hex> <x> <y> <z> <gier> [bank:bit ...]" = Spielstand fuer den Lade-Weg (RE15_CONTINUE_TEST=1 RE15_CARD_AUTO=1 RE15_CARD_SLOT=0; Muster probe_r36_elza_karte.c, mit y:
 * das Band folgt beim Laden aus y, re15_savedata.c @re15_collision_band_from_y; der Ring von 2030 liegt auf y -5400).
 */
#include "re15_rdt.h"
#include "re15_scd.h"
#include "re15_actor.h"
#include "re15_aot.h"
#include "re15_room.h"
#include "re15_msg.h"
#include "re15_msg_select.h"
#include "re15_inventory.h"
#include "re15_esp.h"
#include "re15_fade.h"
#include "re15_collision.h"
#include "re15_ventilkette_r41.h"
#include "re15_keyitems_r41.h"
#include "re15_panel2030_r41.h"
#include "re15_schrank2010_r41.h"
#include "re15_wasserlauf_r41.h"
#include "re15_files.h"
#include "re15_savedata.h"
#include "re15_memcard.h"
#include "re15_gameflow.h"

#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void scd_register_current_rdt(const re15_rdt_t *rdt);
void re15_game_state_init(void);
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
static struct { uint8_t zone, idx; } s_vf[16];
static int s_nvf = 0;
static void vorflag(uint8_t zone, uint8_t idx) { if (s_nvf < 16) { s_vf[s_nvf].zone = zone; s_vf[s_nvf].idx = idx; s_nvf++; } }
static void band_aus_y(int32_t y) { re15_collision_set_band(re15_collision_band_from_y(y)); }
static int bit(int b) { return re15_game_flag_get(RE15_R41C_BANK, (uint8_t)b) ? 1 : 0; }
static void bits_leeren(void) { re15_game_state_init(); re15_ventilkette_r41_reset(); s_nvf = 0; }

static int raum_hoch(uint16_t raum, int32_t x, int32_t y, int32_t z)
{
    long sz = 0;
    char pfad[256];
    snprintf(pfad, sizeof pfad, RE15_ASSET_PSX_DIR "/STAGE%d/ROOM%04X.RDT", (raum >> 12), raum);
    free(s_daten);
    s_daten = slurp(pfad, &sz);
    if (!s_daten || re15_rdt_parse(s_daten, (size_t)sz, &s_rdt) != 0) return -1;
    re15_actor_init(); re15_aot_init();
    scd_vm_init(); re15_inv_init();
    for (int i = 0; i < s_nvf; i++) re15_game_flag_set(s_vf[i].zone, s_vf[i].idx, 1);
    memset(&g_room_change, 0, sizeof g_room_change);
    g_current_room_id = raum;
    scd_register_current_rdt(&s_rdt);
    re15_msg_clear_room_block();
    re15_msg_load_room_block(s_rdt.messages, s_rdt.messages_size);
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

typedef struct { int msgs[8], n_msgs, takte; } lauf_t;

/* Spieler an (x,z,rot), Aktion, dann Bilder bis kein Faden mehr laeuft. nein = 1: in der Auswahl einmal umschalten. */
static lauf_t druecken(int32_t x, int32_t z, int rot, int nein, int max_takte)
{
    lauf_t L; memset(&L, 0, sizeof L);
    re15_actor_t *p = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    p->x = x; p->z = z; p->rot_y = (int16_t)rot;
    band_aus_y(p->y);
    g_aot.fired_event_id_this_frame = 0;
    g_aot_action_pressed = 1;
    re15_aot_scan(p->x, p->z, 0xFF);
    g_aot_action_pressed = 0;
    if (g_aot.fired_event_id_this_frame) (void)scd_event_fire(g_aot.fired_event_id_this_frame);
    int offen = -1, umgeschaltet = 0;
    for (int i = 0; i < max_takte; i++) {
        g_scd_pad_edge = 0; g_scd_pad_held = 0;
        if (g_scd.message_active) {
            if (g_scd.message_id != offen && L.n_msgs < 8) { offen = g_scd.message_id; L.msgs[L.n_msgs++] = offen; }
            if ((i & 7) == 7 && (g_scd.message_fsm == 1 || g_scd.message_fsm == 3 || g_scd.message_fsm == 4 || g_scd.message_fsm == 7)) {
                if (nein && !umgeschaltet && g_scd.message_fsm == 3) { g_scd_pad_edge = 0x1000; umgeschaltet = 1; }
                else g_scd_pad_edge = 0x4000;
            }
        } else offen = -1;
        takt();
        L.takte = i + 1;
        g_scd_pad_edge = 0;
        int laeuft = g_scd.message_active;
        for (int k = SCD_EVENT_SLOT_FIRST; k <= SCD_EVENT_SLOT_LAST; k++) if (g_scd.threads[k].active) laeuft = 1;
        if (!laeuft && i > 30) break;
    }
    { scd_audio_event_t ev; while (scd_audio_queue_pop(&ev)) {} }
    return L;
}

/* Nachricht wegdruecken, bis keine mehr steht (Panel offen: das Spiel laeuft weiter). */
static void nachricht_weg(void)
{
    for (int i = 0; i < 400 && (g_scd.message_active || g_scd.message_display_frames > 0); i++) {
        g_scd_pad_edge = (i & 7) == 7 ? 0x4000 : 0;
        g_scd_pad_held = 0;
        takt();
        g_scd_pad_edge = 0;
    }
}

/* Cursor per Steuerkreuz zur Stelle (x,y) fahren und SQUARE druecken (virtuelle Bits 1 hoch, 2 rechts, 4 runter, 8 links, 0x40 SQUARE). */
static void cursor_fahren(int zx, int zy)
{
    for (int i = 0; i < 400; i++) {
        int x = 0, y = 0;
        re15_panel2030_r41_cursor(&x, &y);
        uint16_t h = 0;
        if (x < zx - 2) h |= 2; else if (x > zx + 2) h |= 8;
        if (y < zy - 2) h |= 4; else if (y > zy + 2) h |= 1;
        if (!h) break;
        g_scd_pad_held = h; g_scd_pad_edge = 0;
        takt();
    }
    g_scd_pad_held = 0;
}
static void druck(void) { g_scd_pad_edge = 0x40; g_scd_pad_held = 0; takt(); g_scd_pad_edge = 0; for (int i = 0; i < 20; i++) takt(); }
static void zug(int von_x, int nach_x) { cursor_fahren(von_x, 90); druck(); cursor_fahren(nach_x, 90); druck(); }
#define X_A 71
#define X_B 147
#define X_C 191

/* ---- A: Regel + Vollsuche ---------------------------------------------------------------------------------------- */
static void fall_a(void)
{
    re15_panel2030_r41_oeffnen();
    PRUEFE(re15_panel2030_r41_stand(0) == 100 && re15_panel2030_r41_stand(1) == 0 && re15_panel2030_r41_stand(2) == 0, "Start 100/0/0");
    PRUEFE(re15_panel2030_r41_aktion(1) == 4 && re15_panel2030_r41_quelle() == -1, "leere Quelle B: Rueckgabe 4, keine Wahl");
    PRUEFE(re15_panel2030_r41_aktion(0) == 1 && re15_panel2030_r41_quelle() == 0, "Quelle A gewaehlt");
    PRUEFE(re15_panel2030_r41_aktion(0) == 2 && re15_panel2030_r41_quelle() == -1, "A nochmal: Wahl zurueck");
    (void)re15_panel2030_r41_aktion(0);
    PRUEFE(re15_panel2030_r41_aktion(1) == 3 && re15_panel2030_r41_stand(0) == 50 && re15_panel2030_r41_stand(1) == 50 && re15_panel2030_r41_stand(2) == 0,
           "A nach B: bis das Ziel voll ist: 50/50/0");
    (void)re15_panel2030_r41_aktion(1);
    PRUEFE(re15_panel2030_r41_aktion(2) == 3 && re15_panel2030_r41_stand(0) == 50 && re15_panel2030_r41_stand(1) == 20 && re15_panel2030_r41_stand(2) == 30,
           "B nach C: 50/20/30");
    (void)re15_panel2030_r41_aktion(2);
    PRUEFE(re15_panel2030_r41_aktion(0) == 3 && re15_panel2030_r41_stand(0) == 80 && re15_panel2030_r41_stand(1) == 20 && re15_panel2030_r41_stand(2) == 0,
           "C nach A: 80/20/0 = Ziel");
    (void)re15_panel2030_r41_aktion(1);
    (void)re15_panel2030_r41_aktion(0);   /* B nach A: bis A voll */
    PRUEFE(re15_panel2030_r41_stand(0) == 100 && re15_panel2030_r41_stand(1) == 0, "B nach A: 100/0/0");
    (void)re15_panel2030_r41_aktion(0);
    (void)re15_panel2030_r41_aktion(1);   /* 50/50/0 */
    (void)re15_panel2030_r41_aktion(0);
    PRUEFE(re15_panel2030_r41_aktion(1) == 5 && re15_panel2030_r41_quelle() == 0, "Ziel voll: Rueckgabe 5, Quelle bleibt");
    /* Vollsuche ueber die Regel selbst: von jedem erreichbaren Stand ein Weg zu 80, kuerzester Weg 3 Zuege */
    enum { MAXS = 64 };
    int st[MAXS][3], dist[MAXS], n = 1, kopf = 0, ziel_d = -1;
    st[0][0] = 100; st[0][1] = 0; st[0][2] = 0; dist[0] = 0;
    static const int kap[3] = RE15_R41N_KAPAZITAET;
    int kante[MAXS][6];
    while (kopf < n) {
        const int *s = st[kopf];
        if (s[0] == RE15_R41N_ZIEL && ziel_d < 0) ziel_d = dist[kopf];
        int e = 0;
        for (int q = 0; q < 3; q++) for (int t = 0; t < 3; t++) {
            if (q == t) continue;
            int d[3] = { s[0], s[1], s[2] };
            int menge = d[q] < kap[t] - d[t] ? d[q] : kap[t] - d[t];
            d[q] -= menge; d[t] += menge;
            int k = -1;
            for (int i = 0; i < n; i++) if (st[i][0] == d[0] && st[i][1] == d[1] && st[i][2] == d[2]) k = i;
            if (k < 0 && n < MAXS) { k = n++; memcpy(st[k], d, sizeof d); dist[k] = dist[kopf] + 1; }
            kante[kopf][e++] = k;
        }
        kopf++;
    }
    PRUEFE(n == 16, "16 erreichbare Staende (%d)", n);
    PRUEFE(ziel_d == 3, "kuerzeste Loesung 3 Zuege (%d)", ziel_d);
    /* keine Sackgasse: von jedem Stand ist ein Stand mit A = 80 erreichbar */
    int gut[MAXS]; memset(gut, 0, sizeof gut);
    for (int i = 0; i < n; i++) gut[i] = st[i][0] == RE15_R41N_ZIEL;
    for (int r = 0; r < n; r++) for (int i = 0; i < n; i++) for (int e = 0; e < 6; e++) if (gut[kante[i][e]]) gut[i] = 1;
    int alle = 1; for (int i = 0; i < n; i++) if (!gut[i]) alle = 0;
    PRUEFE(alle, "keine Sackgasse");
    re15_panel2030_r41_reset();
}

/* ---- B: Panel im Raum ----------------------------------------------------------------------------------------------- */
static void fall_b(uint16_t r)
{
    bits_leeren();
    if (raum_hoch(r, 350, -5400, -9500) != 0) { PRUEFE(0, "ROOM%04X Parse", r); return; }
    int vs = 0;
    for (int s = 0; s < RE15_AOT_MAX; s++) if (g_aot.slots[s].active && g_aot.slots[s].event_id == 25 && g_aot.slots[s].type == RE15_AOT_TYPE_GENERIC) vs++;
    PRUEFE(vs == 0 && re15_ventilkette_r41_programm("2030_v13_frage", NULL) == NULL && re15_ventilkette_r41_programm("2030_pult_ohne", NULL) == NULL,
           "ROOM%04X: Ventil 13 entfaellt (Plaetze %d)", r, vs);
    lauf_t L = druecken(350, -9500, 1024, 1, 300);
    PRUEFE(L.n_msgs == 1 && L.msgs[0] == RE15_R41C_MSG_PANEL_FRAGE && !re15_panel2030_r41_offen() && re15_game_flag_get(5, RE15_R41C_FLAG_PANEL) == 0,
           "ROOM%04X Nein: nur Frage 40, kein Panel", r);
    L = druecken(350, -9500, 1024, 0, 400);
    PRUEFE(L.n_msgs == 2 && L.msgs[0] == RE15_R41C_MSG_PANEL_FRAGE && L.msgs[1] == RE15_R41C_MSG_PANEL_START && re15_panel2030_r41_offen(), "ROOM%04X Ja: Panel offen, Text 41 (msgs %d)", r, L.n_msgs);
    PRUEFE(re15_game_flag_get(2, 0) == 1 && re15_game_flag_get(2, 2) == 1 && re15_game_flag_get(2, 7) == 0, "ROOM%04X: Spieler + KI stehen, keine Pad-Sperre", r);
    PRUEFE(re15_panel2030_r41_rgba() != NULL, "ROOM%04X: Vollbild steht", r);
    { const uint32_t *b = re15_panel2030_r41_rgba(); int bunt = 0; for (int i = 0; i < 320 * 240; i += 97) if ((b[i] >> 8) != 0) bunt++;
      PRUEFE(bunt > 400, "ROOM%04X: Vollbild traegt Bildinhalt (%d)", r, bunt); }
    nachricht_weg();
    /* EXIT verlaesst, naechstes Oeffnen startet bei 100/0/0 */
    zug(X_A, X_B);
    PRUEFE(re15_panel2030_r41_stand(1) == 50, "ROOM%04X: A nach B per Cursor: B %d", r, re15_panel2030_r41_stand(1));
    cursor_fahren(254, 195); g_scd_pad_edge = 0x40; takt(); g_scd_pad_edge = 0;
    PRUEFE(!re15_panel2030_r41_offen() && re15_game_flag_get(2, 0) == 0 && re15_game_flag_get(2, 2) == 0 && bit(RE15_R41C_BIT_DRUCK80) == 0, "ROOM%04X: EXIT verlaesst, Bit 101 = 0", r);
    L = druecken(350, -9500, 1024, 0, 400);
    PRUEFE(re15_panel2030_r41_offen() && re15_panel2030_r41_stand(0) == 100 && re15_panel2030_r41_stand(1) == 0, "ROOM%04X: neu offen = Start", r);
    PRUEFE(re15_ventilkette_r41_vollbild_sperrt(), "ROOM%04X: START bei offenem Panel gesperrt (Abnahme 0, Mangel 3)", r);
    nachricht_weg();
    /* volle Aufgabe: leere Quelle -> Text 42 + Fehlerton, volles Ziel -> Text 47; CROSS verlaesst */
    cursor_fahren(X_B, 90); g_scd_pad_edge = 0x40; takt(); g_scd_pad_edge = 0; takt();
    PRUEFE(g_scd.message_active && g_scd.message_id == RE15_R41C_MSG_PANEL_LEER, "ROOM%04X: leere Quelle: Text 42 (ist %d)", r, g_scd.message_id);
    nachricht_weg();
    g_scd_pad_edge = 0x8000; takt(); g_scd_pad_edge = 0;
    PRUEFE(!re15_panel2030_r41_offen() && re15_game_flag_get(2, 0) == 0, "ROOM%04X: CROSS verlaesst", r);
    PRUEFE(!re15_ventilkette_r41_vollbild_sperrt(), "ROOM%04X: nach dem Panel keine START-Sperre", r);
    /* Loesung */
    L = druecken(350, -9500, 1024, 0, 400);
    nachricht_weg();
    zug(X_A, X_B); zug(X_B, X_C); zug(X_C, X_A);
    PRUEFE(re15_panel2030_r41_stand(0) == 80 && re15_panel2030_r41_stand(1) == 20 && re15_panel2030_r41_stand(2) == 0, "ROOM%04X: Loesung 80/20/0 (A %d)", r, re15_panel2030_r41_stand(0));
    PRUEFE(re15_panel2030_r41_ziel_erreicht(), "ROOM%04X: Ziel erreicht", r);
    int frage = 0;
    for (int i = 0; i < 80 && !frage; i++) { takt(); if (g_scd.message_active && g_scd.message_id == RE15_R41C_MSG_BRUECKE_FRAGE) frage = 1; }
    PRUEFE(frage && !re15_panel2030_r41_offen() && bit(RE15_R41C_BIT_DRUCK80) == 1, "ROOM%04X: bei 80 folgt Text 43, Panel zu, Bit 101", r);
    /* Ja: Fahrt mit den RE2-Toenen, Satz 0x0F vor der Fahrt, 0x10 am Ende (ROOM4080 sub13 @0x031F2/@0x03314) */
    const unsigned f0 = re15_ventilkette_r41_ton_zaehler(RE15_R41C_TON_BRUECKE_FAHRT), h0 = re15_ventilkette_r41_ton_zaehler(RE15_R41C_TON_BRUECKE_HALT);
    int fahrt_y = 0, halt_bei = -1, fahrt_bei = -1, y_fahrt = 0, y_halt = 0;
    int bruecke = -1;
    for (int k = 0; k < (int)g_scd.prop_count; k++) if (g_scd.props[k].active && g_scd.props[k].obj_id == RE15_R41C_BRUECKE_OBJ) bruecke = k;
    for (int i = 0; i < 400; i++) {
        g_scd_pad_edge = (g_scd.message_active && (i & 7) == 7) ? 0x4000 : 0;
        takt();
        g_scd_pad_edge = 0;
        if (fahrt_bei < 0 && re15_ventilkette_r41_ton_zaehler(RE15_R41C_TON_BRUECKE_FAHRT) > f0) { fahrt_bei = i; y_fahrt = g_scd.props[bruecke].y; }
        if (halt_bei < 0 && re15_ventilkette_r41_ton_zaehler(RE15_R41C_TON_BRUECKE_HALT) > h0) { halt_bei = i; y_halt = g_scd.props[bruecke].y; }
        fahrt_y = g_scd.props[bruecke].y;
    }
    PRUEFE(fahrt_y == RE15_R41C_BRUECKE_Y_UNTEN && bit(RE15_R41C_BIT_BRUECKE_UNTEN) == 1, "ROOM%04X: Bruecke unten (y %d)", r, fahrt_y);
    PRUEFE(fahrt_bei >= 0 && halt_bei > fahrt_bei && y_fahrt <= RE15_R41C_BRUECKE_Y_OBEN + 2 * RE15_R41C_BRUECKE_SCHRITT && y_halt == RE15_R41C_BRUECKE_Y_UNTEN,
           "ROOM%04X: Motor-Ton am Fahrtbeginn (Takt %d, y %d), Halt am Ende (Takt %d, y %d)", r, fahrt_bei, y_fahrt, halt_bei, y_halt);
    PRUEFE(halt_bei - fahrt_bei >= 2 * RE15_R41C_BRUECKE_SCHRITTE - 2 && halt_bei - fahrt_bei <= 2 * RE15_R41C_BRUECKE_SCHRITTE + 2,
           "ROOM%04X: Abstand Motor -> Halt = Fahrt 60 Bilder (%d)", r, halt_bei - fahrt_bei);
    PRUEFE(re15_ventilkette_r41_ton_zaehler(RE15_R41C_TON_BRUECKE_FAHRT) == f0 + 1 && re15_ventilkette_r41_ton_zaehler(RE15_R41C_TON_BRUECKE_HALT) == h0 + 1,
           "ROOM%04X: je ein Bruecken-Ton", r);
}

/* ---- C: Schrank -------------------------------------------------------------------------------------------------------- */
static void fall_c(uint16_t r)
{
    enum { KX = 4300, KZ = -2770 };
    bits_leeren();
    if (raum_hoch(r, KX, 0, KZ) != 0) { PRUEFE(0, "ROOM%04X Parse", r); return; }
    PRUEFE(re15_schrank2010_r41_prop(RE15_R41N_SCHRANK_OBJ_AUSSEN) < 0 && !re15_schrank2010_r41_offen(), "ROOM%04X: geschlossen kein Modell im Raum", r);
    PRUEFE(re15_ventilkette_r41_obj_id(r, 5) == RE15_R41N_SCHRANK_OBJ_INNEN && re15_ventilkette_r41_obj_id(r, 7) == RE15_R41N_SCHRANK_OBJ_TUER, "ROOM%04X: Modelle gebunden", r);
    /* Klick: Fahrt zaehlen */
    g_actors[RE15_ACTOR_SLOT_PLAYER].rot_y = 0;
    band_aus_y(0);
    g_aot.fired_event_id_this_frame = 0;
    g_aot_action_pressed = 1; re15_aot_scan(KX, KZ, 0xFF); g_aot_action_pressed = 0;
    PRUEFE(g_aot.fired_event_id_this_frame == RE15_R41C_EV_KASTEN, "ROOM%04X: Aktion am Schrank = Ereignis 20", r);
    (void)scd_event_fire(g_aot.fired_event_id_this_frame);
    int bilder = 0, winkel_alt = 0, stetig = 1, sperre = 0;
    for (int i = 0; i < 60 && !re15_schrank2010_r41_offen(); i++) {
        takt();
        const int w = re15_schrank2010_r41_winkel();
        if (w != winkel_alt) { bilder++; if (w - winkel_alt != RE15_R41N_AUF_SCHRITT) stetig = 0; winkel_alt = w; }
        if (re15_game_flag_get(2, 7)) sperre = 1;
    }
    PRUEFE(re15_schrank2010_r41_offen() && bilder == RE15_R41N_AUF_TAKTE && stetig && re15_schrank2010_r41_winkel() == 1024 && sperre,
           "ROOM%04X: klappt in 16 Bildern um 90 Grad auf (Bilder %d, Winkel %d, Pad-Sperre %d)", r, bilder, re15_schrank2010_r41_winkel(), sperre);
    const int a = re15_schrank2010_r41_prop(RE15_R41N_SCHRANK_OBJ_AUSSEN), b = re15_schrank2010_r41_prop(RE15_R41N_SCHRANK_OBJ_TUER), c = re15_schrank2010_r41_prop(RE15_R41N_SCHRANK_OBJ_INNEN);
    PRUEFE(a >= 0 && b >= 0 && c >= 0 && g_scd.props[a].rot_y == 1024 && g_scd.props[b].rot_y == 1024 && g_scd.props[a].x == RE15_R41N_ANGEL_X && g_scd.props[a].z == RE15_R41N_ANGEL_Z,
           "ROOM%04X: Tuer steht in der Angel offen", r);
    int md = 0, ok = 0;
    const uint8_t *m = re15_ventilkette_r41_md1_bytes_k(6, &md);
    ok = m && md > 0 && re15_ventilkette_r41_tim_bytes_k(5, &md) && md > 0;
    PRUEFE(ok, "ROOM%04X: Modelle eingebacken", r);
    /* Text 40 danach */
    int text = 0;
    for (int i = 0; i < 400; i++) {
        if (g_scd.message_active && g_scd.message_id == RE15_R41C_MSG_KASTEN_LEER) text = 1;
        g_scd_pad_edge = (g_scd.message_active && (i & 7) == 7) ? 0x4000 : 0;
        takt(); g_scd_pad_edge = 0;
        if (text && !g_scd.message_active) break;
    }
    PRUEFE(text && re15_game_flag_get(2, 7) == 0, "ROOM%04X: nach dem Aufklappen Text 40, Spieler frei", r);
    /* zweiter Klick: keine zweite Fahrt */
    lauf_t L = druecken(KX, KZ, 0, 0, 300);
    PRUEFE(L.n_msgs == 1 && L.msgs[0] == RE15_R41C_MSG_KASTEN_LEER && re15_schrank2010_r41_winkel() == 1024, "ROOM%04X: zweiter Klick nur Text, Tuer bleibt auf", r);
    /* Nur unter Cut 3 (Abnahme 0, Mangel 4) */
    for (int c = 0; c < 6; c++)
        for (int o = RE15_R41N_SCHRANK_OBJ_INNEN; o <= RE15_R41N_SCHRANK_OBJ_TUER; o++)
            PRUEFE(re15_schrank2010_r41_verdeckt(r, c, o) == (c != 3), "ROOM%04X: Cut %d obj %d", r, c, o);
    PRUEFE(!re15_schrank2010_r41_verdeckt(r, 4, 4) && !re15_schrank2010_r41_verdeckt(0x2030, 4, 6), "ROOM%04X: nur die drei Modelle, nur 2010/2011", r);
    /* Ja-Zweig wie ROOM2060 (Abnahme 0, Mangel 1) */
    vorflag(RE15_R41C_SICHERUNG_BANK, RE15_R41C_SICHERUNG_BIT);
    if (raum_hoch(r, KX, 0, KZ) != 0) { PRUEFE(0, "Parse"); return; }
    PRUEFE(!re15_schrank2010_r41_offen() && re15_schrank2010_r41_winkel() == 0, "ROOM%04X: Raumstart: Schrank wieder zu", r);
    g_actors[RE15_ACTOR_SLOT_PLAYER].rot_y = 0; band_aus_y(0);
    g_aot.fired_event_id_this_frame = 0;
    g_aot_action_pressed = 1; re15_aot_scan(KX, KZ, 0xFF); g_aot_action_pressed = 0;
    (void)scd_event_fire(g_aot.fired_event_id_this_frame);
    int cn = 0;
    for (int i = 0; i < 700 && !re15_schrank2010_r41_nahbild(&cn); i++) {
        g_scd_pad_edge = (g_scd.message_active && (i & 7) == 7) ? 0x4000 : 0;
        takt(); g_scd_pad_edge = 0;
    }
    unsigned sr = 0; int sc = -1; int32_t cx = 0, cz = 0;
    re15_schrank2010_r41_cursor(&cx, &cz);
    PRUEFE(cn == 8 && re15_ventilkette_r41_standbild(&sr, &sc) == 1 && sr == 0x2060 && sc == 8, "ROOM%04X Ja: Vollbild ROOM2060 Cut 8", r);
    PRUEFE(cx == 19571 && cz == -22542, "ROOM%04X: Cursor startet an der Lage obj 4 @0x00F12 (%d,%d)", r, (int)cx, (int)cz);
    PRUEFE(re15_game_flag_get(2, 0) && re15_game_flag_get(2, 2) && !re15_game_flag_get(2, 7), "ROOM%04X: (2,0)/(2,2) wie sub01 @0x01288, keine Pad-Sperre", r);
    PRUEFE(re15_ventilkette_r41_vollbild_sperrt(), "ROOM%04X: START im Cursor-Schritt gesperrt", r);
    /* wartet ohne Zeitgrenze, ohne Taste geht nichts weiter */
    g_scd_pad_held = 0; g_scd_pad_edge = 0;
    for (int i = 0; i < 600; i++) takt();
    PRUEFE(re15_schrank2010_r41_nahbild(&cn) && cn == 8 && bit(RE15_R41C_BIT_KASTEN) == 0, "ROOM%04X: 600 Bilder ohne Taste: weiter Cut 8, Bit 125 = 0", r);
    /* SQUARE neben der Zone: Start x 19571 < 19700 @0x0100C */
    PRUEFE(!re15_schrank2010_r41_auf_zone(), "ROOM%04X: Start liegt neben Zone 7", r);
    g_scd_pad_held = 0x40;
    for (int i = 0; i < 10; i++) takt();
    g_scd_pad_held = 0;
    PRUEFE(re15_schrank2010_r41_nahbild(&cn) && cn == 8 && bit(RE15_R41C_BIT_KASTEN) == 0, "ROOM%04X: SQUARE neben der Zone tut nichts", r);
    /* Steuerkreuz: je Bild 200 (sub14..17) */
    g_scd_pad_held = 1; takt(); g_scd_pad_held = 0;
    re15_schrank2010_r41_cursor(&cx, &cz);
    PRUEFE(cx == 19571 && cz == -22342, "ROOM%04X: oben +200 z (%d,%d)", r, (int)cx, (int)cz);
    g_scd_pad_held = 4; takt(); g_scd_pad_held = 8; takt(); g_scd_pad_held = 0;
    re15_schrank2010_r41_cursor(&cx, &cz);
    PRUEFE(cx == 19371 && cz == -22542, "ROOM%04X: unten -200 z, links -200 x (%d,%d)", r, (int)cx, (int)cz);
    int bx = 0, by = 0;
    re15_schrank2010_r41_cursor_bild(&bx, &by);
    PRUEFE(bx == 156 && by == 119, "ROOM%04X: Bild 2,667 Bildpunkte je 200 (%d,%d)", r, bx, by);
    /* Rahmen: Original G2_belege/orig_zensus/log.txt */
    g_scd_pad_held = 1; for (int i = 0; i < 60; i++) takt();
    re15_schrank2010_r41_cursor(&cx, &cz);
    PRUEFE(cz == -15968, "ROOM%04X: Rahmen oben z -15968 (%d)", r, (int)cz);
    g_scd_pad_held = 8; for (int i = 0; i < 80; i++) takt();
    re15_schrank2010_r41_cursor(&cx, &cz);
    PRUEFE(cx == 9418, "ROOM%04X: Rahmen links x 9418 (%d)", r, (int)cx);
    g_scd_pad_held = 4; for (int i = 0; i < 90; i++) takt();
    re15_schrank2010_r41_cursor(&cx, &cz);
    PRUEFE(cz == -29132, "ROOM%04X: Rahmen unten z -29132 (%d)", r, (int)cz);
    g_scd_pad_held = 2; for (int i = 0; i < 120; i++) takt();
    re15_schrank2010_r41_cursor(&cx, &cz);
    PRUEFE(cx == 29532, "ROOM%04X: Rahmen rechts x 29532 (%d)", r, (int)cx);
    /* In die Zone fahren, bis der Cursor drinsteht, und erst dann bestaetigen */
    int auf_zone = 0;
    for (int i = 0; i < 200 && !auf_zone; i++) {
        re15_schrank2010_r41_cursor(&cx, &cz);
        g_scd_pad_held = (uint16_t)((cx > 20725 ? 8 : 0) | (cz < -23275 ? 1 : 0));
        takt();
        auf_zone = re15_schrank2010_r41_auf_zone();
    }
    g_scd_pad_held = 0; takt();
    PRUEFE(auf_zone && re15_schrank2010_r41_nahbild(&cn) && cn == 8, "ROOM%04X: Cursor in Zone 7, ohne Taste weiter Cut 8", r);
    g_scd_pad_held = 0x40; takt(); g_scd_pad_held = 0;
    PRUEFE(re15_schrank2010_r41_nahbild(&cn) && cn == 9 && re15_ventilkette_r41_standbild(&sr, &sc) == 1 && sr == 0x2060 && sc == 9, "ROOM%04X: Zone 7 + SQUARE -> Cut 9", r);
    PRUEFE(re15_ventilkette_r41_vollbild_sperrt(), "ROOM%04X: START auf Cut 9 gesperrt", r);
    for (int i = 0; i < 8; i++) takt();
    PRUEFE(bit(RE15_R41C_BIT_KASTEN) == 1, "ROOM%04X: Bit 125 hinter dem Cursor-Schritt", r);
    int text42 = 0, standbild = 0, cut9_bei_text = 0;
    for (int i = 0; i < 400 && !standbild; i++) {
        if (g_scd.message_active && g_scd.message_id == RE15_R41C_MSG_KASTEN_BENUTZT) { text42 = 1; if (re15_schrank2010_r41_nahbild(&cn) && cn == 9) cut9_bei_text = 1; }
        g_scd_pad_edge = (g_scd.message_active && (i & 7) == 7) ? 0x4000 : 0;
        takt(); g_scd_pad_edge = 0;
        if (re15_ventilkette_r41_standbild(&sr, &sc) == 1 && sr == RE15_R41C_STANDBILD_RAUM) standbild = 1;
    }
    PRUEFE(text42 && cut9_bei_text && standbild && !re15_schrank2010_r41_nahbild(NULL), "ROOM%04X: Text 42 auf Cut 9, dann Standbild der Ventile", r);
    PRUEFE(!re15_ventilkette_r41_vollbild_sperrt(), "ROOM%04X: die Sperre haengt nur am Vollbild ohne (2,7)", r);
}

/* ---- C2: USE am geschlossenen Schrank (Integration N x O) ------------------------------------------------------------------
 * Reihenfolge wie im Spiel: main.c scd_vm_tick, dann re15_game_step: re15_keyitems_r41_tick (game_step_common.c:1364) VOR re15_ventilkette_r41_tick (:1365). Die Sicherung wird
 * Eintrag ueber den Besitz-Takt ((3,108) @0x01FF2), USE laeuft ueber dieselbe Stellen-Pruefung wie das Inventar (Vorwaertspunkt 620 @0x80042bd0). */
static void takt_use(void)
{
    re15_actor_t *p = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    re15_msg_tick(0, 0, 0);
    scd_vm_tick();
    re15_keyitems_r41_tick();
    re15_ventilkette_r41_tick();
    re15_aot_scan(p->x, p->z, 0xFF);
    (void)re15_cam_present_tick();
}

static void fall_c_use(uint16_t r)
{
    enum { KX = 4300, KZ = -2770, SICHERUNG = 0x40 };
    bits_leeren();
    vorflag(RE15_R41C_SICHERUNG_BANK, RE15_R41C_SICHERUNG_BIT);
    if (raum_hoch(r, KX, 0, KZ) != 0) { PRUEFE(0, "ROOM%04X Parse", r); return; }
    re15_actor_t *p = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    p->rot_y = 0; band_aus_y(0);
    re15_keyitems_r41_tick();
    PRUEFE(re15_inv_find_item(SICHERUNG) >= 0 && re15_game_flag_get(RE15_R41C_SICHERUNG_BANK, RE15_R41C_SICHERUNG_BIT), "ROOM%04X USE: Sicherung im Inventar", r);
    PRUEFE(!re15_schrank2010_r41_offen() && re15_ventilkette_r41_kasten_fragt(), "ROOM%04X USE: Tuer zu, Kasten fragt", r);
    PRUEFE(re15_keyitems_r41_use(SICHERUNG) == 1 && re15_keyitems_r41_schliessen() == 1, "ROOM%04X USE: trifft den Kasten", r);
    /* Mangel 1: der Lauf endete 2 Takte nach dem Start, die Tuer braucht 16 (Vorfix: "USE-Lauf ohne Frage beendet", dann Text 40) */
    int cn = 0, text = 0, lauf_hielt = 1, bilder = 0;
    for (int i = 0; i < 40 && !re15_schrank2010_r41_nahbild(&cn); i++) {
        takt_use();
        bilder++;
        if (g_scd.message_active) text = 1;
        if (!re15_schrank2010_r41_offen() && !re15_keyitems_r41_lauf_aktiv()) lauf_hielt = 0;
    }
    PRUEFE(lauf_hielt, "ROOM%04X USE: Lauf haelt bis die Tuer offen ist", r);
    PRUEFE(cn == 8 && re15_schrank2010_r41_offen() && re15_schrank2010_r41_winkel() == 1024 && bilder >= RE15_R41N_AUF_TAKTE, "ROOM%04X USE: Tuer ganz auf, dann Cut 8 + Cursor (Bilder %d, Cut %d)", r, bilder, cn);
    PRUEFE(!text, "ROOM%04X USE: kein Text bis zum Cursor (RE2 ROOM1110 sub01 @0x02336: Ereignis ohne Text und Frage)", r);
    PRUEFE(!re15_keyitems_r41_lauf_aktiv(), "ROOM%04X USE: Lauf mit der Frage beendet", r);
    PRUEFE(bit(RE15_R41C_BIT_KASTEN) == 0 && re15_inv_find_item(SICHERUNG) >= 0, "ROOM%04X USE: Sicherung noch nicht drin", r);
    /* Zone 7: x 19571 + 200 >= 19700 @0x0100C, SQUARE @0x0132E */
    g_scd_pad_held = 2; takt_use(); g_scd_pad_held = 0x40; takt_use(); g_scd_pad_held = 0;
    PRUEFE(re15_schrank2010_r41_nahbild(&cn) && cn == 9, "ROOM%04X USE: Zone 7 + SQUARE -> Cut 9 (%d)", r, cn);
    int text42 = 0, standbild = 0; unsigned sr = 0; int sc = -1;
    for (int i = 0; i < 400 && !standbild; i++) {
        if (g_scd.message_active && g_scd.message_id == RE15_R41C_MSG_KASTEN_BENUTZT) text42 = 1;
        g_scd_pad_edge = (g_scd.message_active && (i & 7) == 7) ? 0x4000 : 0;
        takt_use(); g_scd_pad_edge = 0;
        if (re15_ventilkette_r41_standbild(&sr, &sc) == 1 && sr == RE15_R41C_STANDBILD_RAUM) standbild = 1;
    }
    PRUEFE(bit(RE15_R41C_BIT_KASTEN) == 1 && text42 && standbild, "ROOM%04X USE: Bit 125 -> Text 42 -> Standbild", r);
    PRUEFE(re15_inv_find_item(SICHERUNG) < 0, "ROOM%04X USE: Sicherung verlaesst das Inventar", r);

    /* Z1 (Dossier N OFFEN 6, Entscheid des Orchestrators): zweite Sicherung (Eintrag aus ROOM1150), der Kasten fragt nicht: Sicherung steckt (Fall 0) oder sitzt im Generator (Fall 1). USE gilt
     * dann nicht, RE2 @0x8006b228 -> Text 7 @0x8006b248: kein Lauf, kein Programm, Bit 125 unberuehrt, bei geschlossener UND bei offener Tuer (offen nahm der Lauf die Sicherung stumm heraus,
     * gemessen Laeufe z1/a3, z1/a3e; geschlossen zeigte er Text 43 / 40 hinter der Tuer). Die Aktionstaste bleibt: Tuer auf, Text 43 / 40. */
    for (int fall = 0; fall < 2; fall++) {
        bits_leeren();
        vorflag(RE15_R41C_SICHERUNG_BANK, RE15_R41C_SICHERUNG_BIT);
        if (fall == 0) vorflag(RE15_R41C_BANK, RE15_R41C_BIT_KASTEN);
        else           vorflag(RE15_R41C_SICHERUNG_BANK, RE15_R41C_GENERATOR_BIT);
        if (raum_hoch(r, KX, 0, KZ) != 0) { PRUEFE(0, "ROOM%04X Parse", r); return; }
        g_actors[RE15_ACTOR_SLOT_PLAYER].rot_y = 0; band_aus_y(0);
        PRUEFE(!re15_ventilkette_r41_kasten_fragt(), "ROOM%04X Fall %d: Kasten fragt nicht", r, fall);
        (void)re15_inv_grant(SICHERUNG, 1);
        const int soll = fall == 0 ? RE15_R41C_MSG_KASTEN_DRIN : RE15_R41C_MSG_KASTEN_LEER;
        for (int offen = 0; offen < 2; offen++) {
            PRUEFE(re15_schrank2010_r41_offen() == offen, "ROOM%04X Fall %d: Tuer %s", r, fall, offen ? "offen" : "zu");
            PRUEFE(re15_keyitems_r41_use(SICHERUNG) == 0 && re15_keyitems_r41_schliessen() == 0 && !re15_keyitems_r41_lauf_aktiv(),
                   "ROOM%04X Fall %d Tuer %d: USE gilt nicht (You can't use it here.)", r, fall, offen);
            int text = 0;
            for (int i = 0; i < 40; i++) { takt_use(); if (g_scd.message_active) text = 1; }
            PRUEFE(!text && re15_schrank2010_r41_offen() == offen, "ROOM%04X Fall %d Tuer %d: kein Text, Tuer unveraendert", r, fall, offen);
            PRUEFE(bit(RE15_R41C_BIT_KASTEN) == (fall == 0), "ROOM%04X Fall %d Tuer %d: Bit 125 unberuehrt", r, fall, offen);
            PRUEFE(re15_inv_find_item(SICHERUNG) >= 0, "ROOM%04X Fall %d Tuer %d: die zweite Sicherung bleibt im Inventar", r, fall, offen);
            if (offen) break;
            /* Aktionstaste: klappt auf, Text 43 / 40 wie vorher; Nein, damit Bit 125 stehen bleibt */
            g_aot.fired_event_id_this_frame = 0;
            g_aot_action_pressed = 1; re15_aot_scan(KX, KZ, 0xFF); g_aot_action_pressed = 0;
            (void)scd_event_fire(g_aot.fired_event_id_this_frame);
            int gesehen = 0;
            for (int i = 0; i < 60 && !gesehen; i++) { takt_use(); if (g_scd.message_active && g_scd.message_id == soll) gesehen = 1; }
            PRUEFE(gesehen && re15_schrank2010_r41_offen(), "ROOM%04X Fall %d: Aktionstaste: Text %d hinter der Tuer", r, fall, soll);
            for (int i = 0; i < 300 && (g_scd.message_active || re15_game_flag_get(2, 7)); i++) {
                g_scd.message_choice = 1;
                g_scd_pad_edge = (i & 7) == 7 ? 0x4000 : 0; takt_use(); g_scd_pad_edge = 0;
            }
        }
    }
}

/* ---- D: Wasserlauf --------------------------------------------------------------------------------------------------- */
static int esp_0b(void)
{
    int n = 0;
    for (int i = 0; i < 96; i++) { const re15_esp_fx_t *f = re15_esp_fx_get(i); if (f && f->effect_id == 0x0b) n++; }
    return n;
}
static void fall_d(uint16_t r)
{
    bits_leeren();
    if (raum_hoch(r, -26300, -1800, 9000) != 0) { PRUEFE(0, "ROOM%04X Parse", r); return; }
    PRUEFE(re15_wasserlauf_r41_aktiv() && re15_wasserlauf_r41_strahlen() == 6, "ROOM%04X: Wasserlauf an, 6 Strahlen (%d)", r, re15_wasserlauf_r41_strahlen());
    /* Strahlen liegen an der Leiter, Anker von oben bis Fuss - 3300 (Kopf "LAGE") */
    for (int i = 0; i < 6; i++) {
        const int32_t *s = re15_wasserlauf_r41_strahl(i);
        PRUEFE(s && s[0] >= -26800 && s[0] <= -26700 && s[2] >= 9034 && s[2] <= 9945 && s[1] <= -5100, "ROOM%04X: Strahl %d an der Leiter", r, i);
    }
    const int n3 = esp_0b();
    PRUEFE(n3 >= 18 + 6 * 6 - 6, "ROOM%04X: Original 3 x 6 + Wasserlauf 6 x 6 Plaetze (%d)", r, n3);
    const unsigned s0 = re15_wasserlauf_r41_spritzer(), l0 = re15_wasserlauf_r41_laeufer();
    /* Lauf: jeder Spritzer der flachen Punkte wandert 100 je Bild nach +x, keiner geht ueber die Kante hinaus; die fallenden sinken hinter der Kante */
    re15_esp_fx_reset();      /* der Raumaufbau oben taktet den Pool nicht */
    int32_t alt_x[96], flach = 0, schritt_ok = 1, ueber_kante = 0, gefallen = 0, max_0f = 0;
    for (int i = 0; i < 96; i++) alt_x[i] = INT32_MIN;
    for (int i = 0; i < 90; i++) {
        takt();
        re15_esp_fx_tick(re15_esp_room_bank());
        int n0f = 0;
        for (int k = 0; k < 96; k++) {
            const re15_esp_fx_t *f = re15_esp_fx_get(k);
            if (!f || f->effect_id != RE15_R41N_ESP_LAUF) { alt_x[k] = INT32_MIN; continue; }
            n0f++;
            if (f->sub_index == RE15_R41N_LAUF_SUB_FLACH) {
                if (alt_x[k] != INT32_MIN) { flach++; if (f->wpos[0] - alt_x[k] != RE15_R41N_LAUF_VEL) schritt_ok = 0; }
                alt_x[k] = f->wpos[0];
                if (f->wpos[0] > RE15_R41N_LAUF_KANTE) ueber_kante++;
            } else if (f->wpos[0] > RE15_R41N_LAUF_KANTE && f->wpos[1] > RE15_R41N_LAUF_Y) gefallen = 1;
        }
        if (n0f > max_0f) max_0f = n0f;
    }
    PRUEFE(flach > 100 && schritt_ok, "ROOM%04X: flache Spritzer wandern 100 je Bild (%d Schritte)", r, flach);
    PRUEFE(ueber_kante == 0, "ROOM%04X: kein flacher Spritzer ueber der Kante (%d)", r, ueber_kante);
    PRUEFE(gefallen, "ROOM%04X: hinter der Kante faellt das Wasser unter den Weg", r);
    PRUEFE(max_0f <= 20, "ROOM%04X: Lauf belegt hoechstens 20 Plaetze (%d)", r, max_0f);
    PRUEFE(re15_wasserlauf_r41_laeufer() >= l0 + 80 && re15_wasserlauf_r41_spritzer() >= s0 + 7, "ROOM%04X: 5 Punkte, Abstand im Mittel 5 Bilder: 90 Spawns in 90 Bildern (%u -> %u), Einschlaege (%u)", r, l0, re15_wasserlauf_r41_laeufer(), re15_wasserlauf_r41_spritzer());
    /* unter einem Text steht der Lauf mit den Effekten (@0x80019e28), der Pool laeuft nicht voll */
    { const unsigned l1 = re15_wasserlauf_r41_laeufer();
      g_re15_pauseflags |= RE15_PAUSE_ACTION;
      for (int i = 0; i < 40; i++) takt();
      g_re15_pauseflags &= ~RE15_PAUSE_ACTION;
      PRUEFE(re15_wasserlauf_r41_laeufer() == l1, "ROOM%04X: unter einem Text kein Spawn", r); }
    PRUEFE(g_aot.slots[RE15_R41C_SLOT_2000_LEITER].type == RE15_AOT_TYPE_MESSAGE, "ROOM%04X: Leiter bleibt Sperrtext", r);
    /* bis zur Flut: mit Bit 100 oder 103 nichts */
    for (int b = 0; b < 2; b++) {
        bits_leeren(); vorflag(RE15_R41C_BANK, b ? RE15_R41C_BIT_FLUT_GELAUFEN : RE15_R41C_BIT_LEITER_FREI);
        if (raum_hoch(r, -26300, -1800, 9000) != 0) { PRUEFE(0, "Parse"); return; }
        PRUEFE(!re15_wasserlauf_r41_aktiv() && re15_wasserlauf_r41_laeufer() == 0, "ROOM%04X: Bit %d: kein Wasserlauf", r, b ? 103 : 100);
    }
    const uint16_t r20b0 = (uint16_t)(0x20B0 + (r & 1));
    bits_leeren(); vorflag(3, 235);
    if (raum_hoch(r20b0, -25950, -1800, 10300) != 0) { PRUEFE(0, "Parse"); return; }
    PRUEFE(!re15_wasserlauf_r41_aktiv(), "ROOM%04X: kein Wasserlauf", r20b0);
}

/* ---- E: Sirene ------------------------------------------------------------------------------------------------------- */
static void fall_e(uint16_t r)
{
    bits_leeren();
    vorflag(RE15_R41C_BANK, RE15_R41C_BIT_DRUCK80); vorflag(RE15_R41C_BANK, RE15_R41C_BIT_BRUECKE_UNTEN);
    if (raum_hoch(r, 6500, -5400, -200) != 0) { PRUEFE(0, "ROOM%04X Parse", r); return; }
    PRUEFE(!re15_ventilkette_r41_sirene(), "ROOM%04X: ohne Zaehler keine Sirene", r);
    re15_game_flag_set(5, RE15_R41C_FLAG_TIMER, 1);       /* Programm 2030_ventile_flut hinter dem Warntext */
    takt();
    PRUEFE(re15_ventilkette_r41_timer_laeuft() && re15_ventilkette_r41_sirene(), "ROOM%04X: Sirene an mit dem Zaehler (nach der Ansage)", r);
    re15_game_flag_set(5, RE15_R41C_FLAG_FLUT, 1);
    takt();
    PRUEFE(re15_ventilkette_r41_flut_laeuft() && !re15_ventilkette_r41_sirene(), "ROOM%04X: Flut: Sirene aus", r);
    /* Verlassen des Raums */
    bits_leeren();
    if (raum_hoch(r, 6500, -5400, -200) != 0) { PRUEFE(0, "Parse"); return; }
    re15_game_flag_set(5, RE15_R41C_FLAG_TIMER, 1); takt();
    g_current_room_id = 0x20B0; re15_ventilkette_r41_install(0x20B0, NULL);
    PRUEFE(!re15_ventilkette_r41_sirene() && bit(RE15_R41C_BIT_FLUT_GELAUFEN) == 1, "ROOM%04X: Raum verlassen: Sirene aus, Flut gelaufen", r);
    /* Tod */
    bits_leeren();
    if (raum_hoch(r, 6500, -5400, -200) != 0) { PRUEFE(0, "Parse"); return; }
    re15_game_flag_set(5, RE15_R41C_FLAG_TIMER, 1); takt();
    g_actors[RE15_ACTOR_SLOT_PLAYER].hp = -1;
    PRUEFE(!re15_ventilkette_r41_sirene(), "ROOM%04X: tot: Sirene aus", r);
}

/* ---- F: Notiz + Plakat ------------------------------------------------------------------------------------------------- */
static uint32_t fnv(const uint8_t *d, long n) { uint32_t h = 2166136261u; for (long i = 0; i < n; i++) h = (h ^ d[i]) * 16777619u; return h; }
static void fall_f(void)
{
    static const int l[4] = RE15_R41C_LOESUNG;
    static const int nr[6] = RE15_R41C_VENTIL_NUMMERN, farbe[6] = RE15_R41C_VENTIL_FARBE;
    PRUEFE(nr[l[0]] == 9 && nr[l[1]] == 2 && nr[l[2]] == 1 && nr[l[3]] == 8, "Loesung 09, 02, 01, 08 (Nachtrag 30d)");
    PRUEFE(farbe[l[0]] == 0 && farbe[l[1]] == 1 && farbe[l[2]] == 2 && farbe[l[3]] == 1, "Farben rot, gruen, schwarz, gruen = die echten Raeder");
    long n = 0;
    uint8_t *d = slurp(RE15_ASSET_RE2_DIR "/FILES/FILE31_p01_page.TIM", &n);
    PRUEFE(d && fnv(d, n) == RE15_N30_FILE31_P01, "FILE31 Seite 1 = Satz Nachtrag 30b (FNV %08X)", d ? fnv(d, n) : 0u);
    free(d);
    d = slurp(RE15_ASSET_RE2_DIR "/FILES/FILE31_p02_page.TIM", &n);
    PRUEFE(d && fnv(d, n) == RE15_N30_FILE31_P02, "FILE31 Seite 2 = Satz Nachtrag 30b (FNV %08X)", d ? fnv(d, n) : 0u);
    free(d);
}

/* ---- PF: Spur "panelfix" (Runde 41, Dossier analysis/befunde_runde41/PF_panelfix.md) -------------------------------------------
 * G  Beschriftung Panel 2030: jede Glyphe im Vollbild ist Texel fuer Texel die 8x8-Spielschrift aus TEX.TIM (FUN_80029214 @0x80029214: u = ((c-0x20) & 0x1f) * 8,
 *    v = ((c-0x20) & 0xe0) >> 5 * 8 ab Texel x 256) in ihrer CLUT; Gegenprobe: um einen Bildpunkt versetzt stimmt weniger als die Haelfte.
 * Q  Ja/Nein-Auswahl: auf der Frageseite JEDER Port-Frage (2000/2010/2030/2050/2070/20B0) beruehrt keine Glyphe den Cursor (0xa0 @0x80028650 / +0x46) oder
 *    "Yes"/"No" (0xae @0x80028680) in Zeile 0xb4 + 0x10 (@0x80027f14 / @0x80028674); Gegenprobe: die alte Id 40 (zweite Zeile bis x 218) ueberdeckt. */
static uint8_t s_tex_font[32][256];    /* TEX.TIM Texel x 256..511, Zeilen 0..31: 4-bit-Index */
static int     s_tex_font_ok = -1;
static uint8_t s_breite[256];          /* DEBUG.BIN @0x4416 = RAM 0x800c4416 (Vorschub je Glyphe, render_pc.c s_msgfont_w) */
static int     s_breite_ok = -1;

static int tex_font_laden(void)
{
    if (s_tex_font_ok >= 0) return s_tex_font_ok;
    long n = 0;
    uint8_t *d = slurp(RE15_ASSET_PSX_DIR "/DATA/TEX.TIM", &n);
    s_tex_font_ok = 0;
    if (d && n > 20 && (d[4] & 8)) {
        const uint32_t clen = (uint32_t)d[8] | ((uint32_t)d[9] << 8) | ((uint32_t)d[10] << 16) | ((uint32_t)d[11] << 24);
        const uint8_t *im = d + 8 + clen;
        const int iw = im[8] | (im[9] << 8), ih = im[10] | (im[11] << 8);
        if ((long)(8 + clen + 12) + (long)iw * 2 * ih <= n && iw * 4 >= 512 && ih >= 32) {
            for (int y = 0; y < 32; y++)
                for (int x = 0; x < 256; x++) {
                    const int t = 256 + x;
                    const uint8_t b = im[12 + y * iw * 2 + t / 2];
                    s_tex_font[y][x] = (uint8_t)((t & 1) ? (b >> 4) : (b & 15));
                }
            s_tex_font_ok = 1;
        }
    }
    free(d);
    return s_tex_font_ok;
}

static uint32_t farbe15(uint16_t v)    /* wie panel2030_r41.c farbe(): 5 -> 8 Bit mit voller Spanne, RGBA8888 */
{
    const uint32_t r = v & 31u, g = (v >> 5) & 31u, b = (v >> 10) & 31u;
    return (((r << 3) | (r >> 2)) << 24) | (((g << 3) | (g >> 2)) << 16) | (((b << 3) | (b >> 2)) << 8) | 0xFFu;
}

/* Glyphen der Beschriftung gegen ein Bild (rgba, 320 x 240): gesetzte Texel gesamt / gleich / gleich bei Versatz dx. */
static void etiketten_vergleich(const uint32_t *rgba, int dx, long *gesamt, long *gleich)
{
    *gesamt = 0; *gleich = 0;
    const int n = re15_panel2030_r41_glyphe(-1, NULL, NULL, NULL, NULL);
    for (int i = 0; i < n; i++) {
        int code = 0, x = 0, y = 0;
        const uint16_t *clut = NULL;
        re15_panel2030_r41_glyphe(i, &code, &x, &y, &clut);
        const int g = code - 0x20, u = (g & 0x1f) * 8, v = ((g & 0xe0) >> 5) * 8;   /* FUN_80029214 */
        for (int gy = 0; gy < 8; gy++)
            for (int gx = 0; gx < 8; gx++) {
                const uint8_t k = s_tex_font[v + gy][u + gx];
                if (!k || !clut[k]) continue;                                       /* Index 0 / CLUT 0 = durchsichtig */
                const int px = x + gx + dx, py = y + gy;
                if ((unsigned)px >= 320u || (unsigned)py >= 240u) continue;
                (*gesamt)++;
                if ((rgba[py * 320 + px] & 0xFFFFFF00u) == (farbe15(clut[k]) & 0xFFFFFF00u)) (*gleich)++;
            }
    }
}

static void fall_g(void)
{
    PRUEFE(tex_font_laden() == 1, "TEX.TIM: 8x8-Schrift geladen");
    if (s_tex_font_ok != 1) return;
    const int n = re15_panel2030_r41_glyphe(-1, NULL, NULL, NULL, NULL);
    PRUEFE(n == 42, "Beschriftung: 42 Glyphen (A 100, B 50, C 30, 80, BRIDGE, SET 80, CAUTION, HIGH PRESSURE) (%d)", n);
    re15_panel2030_r41_oeffnen();
    re15_panel2030_r41_cursor_setzen(300, 200);           /* Cursor weg von den Schildern */
    const uint32_t *b = re15_panel2030_r41_rgba();
    PRUEFE(b != NULL, "Panel-Bild steht");
    if (!b) return;
    long ges = 0, gl = 0, ges1 = 0, gl1 = 0;
    etiketten_vergleich(b, 0, &ges, &gl);
    etiketten_vergleich(b, 1, &ges1, &gl1);
    PRUEFE(ges > 600 && gl == ges, "Beschriftung = Spielschrift Texel fuer Texel: %ld von %ld gleich", gl, ges);
    PRUEFE(gl1 * 2 < ges1, "Gegenprobe um 1 Bildpunkt versetzt: %ld von %ld gleich (< Haelfte)", gl1, ges1);
    printf("G Beschriftung: %d Glyphen, %ld von %ld Texeln gleich der Spielschrift, um 1 Bildpunkt versetzt %ld von %ld\n", n, gl, ges, gl1, ges1);
    re15_panel2030_r41_reset();
}

/* Spur PF Nachbesserung 1 (Zusage an den Nutzer: Drehscheiben und Teilstriche auf ganze Bildpunkte). S1: jeder Stempel-Texel des Generators (Teilstriche,
 * rotes Band, Querbalken) steht unveraendert im Panel-Bild (Gegenprobe um (2,2) versetzt). S2: der Zeiger kennt keine Zwischentoene: jeder Texel des
 * Instruments ist Grund, Zeigerfarbe (16,15,15), Nabe (96,96,96) oder Grund mit Schatten 87/256 - in jedem Bild der Loesungsfahrt A 100 -> 50 -> 80. */
static int zeiger_texel_pruefen(int *zeiger_px, int *zwischen)
{
    const uint32_t *b = re15_panel2030_r41_rgba();
    if (!b) return -1;
    *zeiger_px = 0; *zwischen = 0;
    for (int y = RE15_R41N_ZEIGER_Y - 15; y <= RE15_R41N_ZEIGER_Y + 15; y++)
        for (int x = RE15_R41N_ZEIGER_X - 15; x <= RE15_R41N_ZEIGER_X + 15; x++) {
            const double dx = x + .5 - (RE15_R41N_ZEIGER_X + .5), dy = y + .5 - (RE15_R41N_ZEIGER_Y + .5);
            if (dx * dx + dy * dy > 13.9 * 13.9) continue;                                   /* Blatt des Instruments */
            const uint32_t g = re15_panel2030_r41_grund_rgba(x, y), v = b[y * 320 + x];
            if (v == g) continue;
            const uint32_t r0 = g >> 24, g0 = (g >> 16) & 255, b0 = (g >> 8) & 255;
            const uint32_t schatten = (((r0 * 169) >> 8) << 24) | (((g0 * 169) >> 8) << 16) | (((b0 * 169) >> 8) << 8) | 0xFFu;
            if (v == ((16u << 24) | (15u << 16) | (15u << 8) | 0xFFu) || v == ((96u << 24) | (96u << 16) | (96u << 8) | 0xFFu)) (*zeiger_px)++;
            else if (v != schatten) (*zwischen)++;
        }
    return 0;
}

static void fall_s(void)
{
    const int n = re15_panel2030_r41_stempel(-1, NULL, NULL, NULL);
    PRUEFE(n > 200, "Scheiben: Stempel-Texel vorhanden (%d)", n);
    re15_panel2030_r41_oeffnen();
    re15_panel2030_r41_cursor_setzen(300, 200);
    const uint32_t *b = re15_panel2030_r41_rgba();
    PRUEFE(b != NULL, "Panel-Bild steht");
    if (!b || n <= 0) return;
    const double zw = (-135.0 + 270.0 * re15_panel2030_r41_anzeige_zehntel() / 1000.0);
    long ges = 0, gl = 0, ges1 = 0, gl1 = 0;
    for (int i = 0; i < n; i++) {
        int x = 0, y = 0;
        uint16_t f = 0;
        re15_panel2030_r41_stempel(i, &x, &y, &f);
        const double dx = x + .5 - (RE15_R41N_ZEIGER_X + .5), dy = y + .5 - (RE15_R41N_ZEIGER_Y + .5);
        if (dx * dx + dy * dy < 16 * 16 && fabs(atan2(dx, -dy) * 180 / 3.14159265358979 - zw) < 15) continue;    /* unter dem Zeiger */
        ges++;
        if ((b[y * 320 + x] & 0xFFFFFF00u) == (farbe15(f) & 0xFFFFFF00u)) gl++;
        ges1++;
        if ((b[(y + 2) * 320 + x + 2] & 0xFFFFFF00u) == (farbe15(f) & 0xFFFFFF00u)) gl1++;    /* quer zu Balken und Band */
    }
    PRUEFE(ges > 200 && gl == ges, "Teilstriche/Band/Balken Texel fuer Texel im Bild: %ld von %ld gleich", gl, ges);
    PRUEFE(gl1 * 4 < ges1, "Gegenprobe um (2,2) versetzt: %ld von %ld gleich (< Viertel)", gl1, ges1);
    /* S2: Zeiger in jedem Bild der Fahrt */
    int bilder = 0, min_px = 1 << 30, max_zw = 0, z_px = 0, z_zw = 0;
    zeiger_texel_pruefen(&z_px, &z_zw);
    bilder++; if (z_px < min_px) min_px = z_px; if (z_zw > max_zw) max_zw = z_zw;
    static const int zuege[3][2] = { { 0, 1 }, { 1, 2 }, { 2, 0 } };    /* Loesung: A -> B (A 50), B -> C, C -> A (A 80) */
    for (int z = 0; z < 3; z++) {
        re15_panel2030_r41_aktion(zuege[z][0]);
        re15_panel2030_r41_aktion(zuege[z][1]);
        for (int t = 0; t < RE15_R41N_UMFUELL_TAKTE + 2; t++) {
            re15_panel2030_r41_takt(0, 0, 0);
            if (zeiger_texel_pruefen(&z_px, &z_zw) != 0) break;
            bilder++; if (z_px < min_px) min_px = z_px; if (z_zw > max_zw) max_zw = z_zw;
        }
    }
    PRUEFE(bilder > 20 && min_px >= 12 && max_zw == 0, "Zeiger auf ganzen Texeln: %d Bilder, je mindestens %d Zeiger-Texel, hoechstens %d Zwischentoene", bilder, min_px, max_zw);
    printf("S Scheiben: %d Stempel-Texel, %ld von %ld gleich, um (2,2) versetzt %ld von %ld; Zeiger %d Bilder (A %d), Zwischentoene max %d, Zeiger-Texel min %d\n",
           n, gl, ges, gl1, ges1, bilder, re15_panel2030_r41_stand(0), max_zw, min_px);
    re15_panel2030_r41_reset();
}

static int debug_breiten(void)
{
    if (s_breite_ok >= 0) return s_breite_ok;
    long n = 0;
    uint8_t *d = slurp(RE15_ASSET_PSX_DIR "/BIN/DEBUG.BIN", &n);
    s_breite_ok = (d && n >= 0x4416 + 256) ? 1 : 0;
    if (s_breite_ok) memcpy(s_breite, d + 0x4416, 256);
    free(d);
    return s_breite_ok;
}

typedef struct { int n, ueber; int x0, x1, y; } frage_lage_t;
static frage_lage_t *s_fl;
static int sel_x0[3], sel_x1[3], sel_y;
static int breite_cb(unsigned char g, void *ctx) { (void)ctx; return s_breite[g]; }
static void glyphe_cb(int penx, int peny, unsigned char g, int attr, void *ctx)
{
    (void)attr; (void)ctx;
    const int x0 = penx, x1 = penx + s_breite[g];
    s_fl->n++;
    for (int k = 0; k < 3; k++)
        if (peny < sel_y + 16 && peny + 16 > sel_y && x0 < sel_x1[k] && x1 > sel_x0[k]) { s_fl->ueber++; break; }
}

/* Anzahl der Glyphen der Frageseite, die Cursor / "Yes" / "No" beruehren (beide Cursor-Zellen). */
static int frage_ueberdeckt(const uint8_t *b, int n, int *glyphen)
{
    int start = 0;                                     /* letzte Seite: hinter dem letzten `02 xx` (FSM message_page) */
    for (int i = 0; i < n && b[i] != 0x01 && b[i] != 0x03; ) {
        const uint8_t c = b[i];
        if (c == 0x02) { start = i + 2; i += 2; continue; }
        if (c == 0x04 || c == 0x05 || c == 0x06 || c == 0x09 || c == 0x0A || c == 0x0B) { i += 2; continue; }
        i++;
    }
    re15_msg_select_t s0, s1;
    re15_msg_select_layout(0, 0x18, &s0);
    re15_msg_select_layout(1, 0x18, &s1);
    sel_y = s0.cursor_y;
    int w = 0;
    for (int k = 0; k < s0.opt[0].len; k++) w += s_breite[s0.opt[0].glyphs[k]];
    sel_x0[0] = s0.opt[0].x; sel_x1[0] = s0.opt[0].x + w;
    w = 0;
    for (int k = 0; k < s0.opt[1].len; k++) w += s_breite[s0.opt[1].glyphs[k]];
    sel_x0[1] = s0.opt[1].x; sel_x1[1] = s0.opt[1].x + w;
    sel_x0[2] = s0.cursor_x; sel_x1[2] = s1.cursor_x + s_breite[0x02];   /* Cursor-Glyphe 0x02 (render_pc.c re15_render_pc_cursor) an 160 und 230 */
    frage_lage_t fl = { 0, 0, 0, 0, 0 };
    s_fl = &fl;
    re15_msg_layout(0x22, 0xb4, b + start, n - start, glyphe_cb, breite_cb, NULL);   /* Box-Ursprung @0x80027eec / @0x80027f14 */
    if (glyphen) *glyphen = fl.n;
    return fl.ueber;
}

static void fall_q(void)
{
    PRUEFE(debug_breiten() == 1, "DEBUG.BIN: Breiten @0x4416");
    if (s_breite_ok != 1) return;
    static const uint16_t raeume[] = { 0x2000, 0x2010, 0x2030, 0x2050, 0x2070, 0x20B0 };
    int fragen = 0, schlecht = 0;
    for (int r = 0; r < (int)(sizeof raeume / sizeof raeume[0]); r++)
        for (int id = 0; id < 64; id++) {
            int n = 0;
            const uint8_t *t = re15_ventilkette_r41_text(raeume[r], (uint8_t)id, &n);
            if (!t || n <= 0 || !memchr(t, 0x03, (size_t)n)) continue;
            int gl = 0;
            const int u = frage_ueberdeckt(t, n, &gl);
            fragen++;
            if (u) { schlecht++; printf("   ROOM%04X Id %d: %d von %d Glyphen auf der Auswahl\n", raeume[r], id, u, gl); }
        }
    PRUEFE(fragen >= 10 && schlecht == 0, "Port-Fragen: %d gefunden, %d ueberdecken die Ja/Nein-Auswahl", fragen, schlecht);
    int n40 = 0, gl = 0;
    const uint8_t *t40 = re15_ventilkette_r41_text(0x2030, RE15_R41C_MSG_PANEL_FRAGE, &n40);
    PRUEFE(t40 && frage_ueberdeckt(t40, n40, &gl) == 0 && gl > 20, "Id 40: Frageseite frei (%d Glyphen)", gl);
    /* Gegenprobe: Id 40 vor Spur PF ("... the bridge." 0x08 "Will you operate the panel?" auf EINER Seite) */
    static const uint8_t alt40[65] = {
        0x04,0x02,0x1d,0x00,0x3f,0x4b,0x4a,0x50,0x4e,0x4b,0x48,0x00,0x4c,0x3d,0x4a,0x41,0x48,0x00,0x42,0x4b,0x4e,0x00,0x50,0x44,
        0x41,0x00,0x3e,0x4e,0x45,0x40,0x43,0x41,0x57,0x08,0x33,0x45,0x48,0x48,0x00,0x55,0x4b,0x51,0x00,0x4b,0x4c,0x41,0x4e,0x3d,
        0x50,0x41,0x00,0x50,0x44,0x41,0x00,0x4c,0x3d,0x4a,0x41,0x48,0x1b,0x03,0x02,0x01,0x00 };
    const int ua = frage_ueberdeckt(alt40, (int)sizeof alt40, &gl);
    PRUEFE(ua >= 4, "Gegenprobe alte Id 40: %d Glyphen auf der Auswahl", ua);
    printf("Q Ja/Nein: %d Port-Fragen, %d ueberdecken; Id 40 neu frei, alte Id 40 %d Glyphen auf der Auswahl\n", fragen, schlecht, ua);
}

/* Werkzeug "etikett <ppm>" (Riegel integration_r41_nachtrag30_panel_*): Beschriftung im Framedump gegen die Spielschrift. */
static int etikett_werkzeug(const char *pfad)
{
    if (tex_font_laden() != 1) { printf("FEHLER: TEX.TIM\n"); return 2; }
    long n = 0;
    uint8_t *d = slurp(pfad, &n);
    if (!d || n < 230415) { printf("FEHLER: %s\n", pfad); free(d); return 2; }
    static uint32_t rgba[320 * 240];
    for (int i = 0; i < 320 * 240; i++)
        rgba[i] = ((uint32_t)d[15 + 3 * i] << 24) | ((uint32_t)d[16 + 3 * i] << 16) | ((uint32_t)d[17 + 3 * i] << 8) | 0xFFu;
    free(d);
    long ges = 0, gl = 0, ges1 = 0, gl1 = 0;
    etiketten_vergleich(rgba, 0, &ges, &gl);
    etiketten_vergleich(rgba, 1, &ges1, &gl1);
    printf("ETIKETT %ld %ld VERSETZT %ld %ld\n", gl, ges, gl1, ges1);
    return 0;
}

/* Werkzeug "kasten <ppm> <innen|tuer>" (Riegel integration_r41_nachtrag30_schrank_*): Seitenverhaeltnis und Scherung des Schranks im Framedump (Cut 3 ROOM2010)
 * gegen ROOM2060 Cut 6 (BG06, fuse_box.bmp). Beide Flaechen werden auf gleichmaessige Weltkoordinaten entzerrt (20 Einheiten je Bildpunkt): die Port-Flaeche ueber die
 * Kamera ROOM2010 Cut 3 @0xC0, die Vorlage (Spur PF Nachbesserung 1, wie schrank_bild.py) ueber die Kamera ROOM2060 Cut 6 @0x120 (Projektion H = fov >> 7, Achsen wie
 * camera_common.c / FUN_80053ca4): Rueckwand Rechteck auf z 4500, x -4830..-3250, y -3545..0; Fluegel senkrechte Ebene durch (-4830,0,4500), Richtung aus dem
 * Fluchtpunkt der Schildkanten x 130,8 auf dem Horizont, s 290..1538 (Scharnierfuge .. Bildrand), y -3198..15 (schrank_bild.py FLUEGEL, fluchtpunkt()).
 * 1. Massstab (sx, sy) mit der besten Deckung der Kanten (NCC); sx/sy = 1 heisst unverzerrt. GEGENPROBE: Port-Flaeche waagrecht auf 80 % gestaucht.
 * 2. Scherung (Abnahme 1 M1): Port-Flaeche mit geneigten Achsen abgetastet (Waagrechte um w Grad, Senkrechte um s Grad, -8..+8 in 0,25), NCC gegen die Vorlage im
 *    besten Massstab; der beste Winkel ist die Neigung des Inhalts gegen die Kastenkanten (0 = nicht geschert). GEGENPROBE: die Port-Flaeche zusaetzlich um 5 Grad
 *    geschert abgetastet (innen die Waagrechten, Fluegel die Senkrechten) muss um 5 Grad versetzt gemessen werden. Ausgabe in Hundertstel Grad. */
#include "re15_bss.h"
static uint8_t *bss_rgb_pf(const char *pfad)
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
            if (k >= 0 && re15_bss_mdec_decode(co, (size_t)k, RE15_BSS_FRAME_WIDTH, RE15_BSS_FRAME_HEIGHT, rgb) == 0) { aus = rgb; rgb = NULL; }
        }
        free(co); free(rgb);
    }
    free(d);
    return aus;
}

static float grau_bilinear(const uint8_t *rgb, float x, float y)   /* Bildpunkt-Mitten bei .5 */
{
    x -= 0.5f; y -= 0.5f;
    int x0 = (int)floorf(x), y0 = (int)floorf(y);
    const float fx = x - (float)x0, fy = y - (float)y0;
    float s = 0;
    for (int j = 0; j < 2; j++)
        for (int i = 0; i < 2; i++) {
            int xx = x0 + i, yy = y0 + j;
            if (xx < 0) xx = 0; if (xx > 319) xx = 319; if (yy < 0) yy = 0; if (yy > 239) yy = 239;
            const uint8_t *p = rgb + 3 * (yy * 320 + xx);
            const float g = 0.299f * p[0] + 0.587f * p[1] + 0.114f * p[2];
            s += g * (i ? fx : 1 - fx) * (j ? fy : 1 - fy);
        }
    return s;
}

static void sobel(const float *g, int w, int h, float *e)
{
    for (int y = 0; y < h; y++)
        for (int x = 0; x < w; x++) {
            if (x == 0 || y == 0 || x == w - 1 || y == h - 1) { e[y * w + x] = 0; continue; }
#define G(i, j) g[(y + (j)) * w + x + (i)]
            const float gx = G(1, -1) + 2 * G(1, 0) + G(1, 1) - G(-1, -1) - 2 * G(-1, 0) - G(-1, 1);
            const float gy = G(-1, 1) + 2 * G(0, 1) + G(1, 1) - G(-1, -1) - 2 * G(0, -1) - G(1, -1);
#undef G
            e[y * w + x] = sqrtf(gx * gx + gy * gy);
        }
}

/* Vorlage tg (tw x th) auf (sx, sy) skaliert (Flaechenmittel), Kanten (Sobel) in te; Groesse in *w_, *h_. */
static void vorlage_skaliert(const float *tg, int tw, int th, double sx, double sy, float *t, float *te, int *w_, int *h_)
{
    const int w = (int)(tw * sx), h = (int)(th * sy);
    for (int y = 0; y < h; y++)
        for (int x = 0; x < w; x++) {
            const double fx0 = x / sx, fx1 = (x + 1) / sx, fy0 = y / sy, fy1 = (y + 1) / sy;
            double s = 0, a = 0;
            for (int yy = (int)fy0; yy < (int)ceil(fy1) && yy < th; yy++)
                for (int xx = (int)fx0; xx < (int)ceil(fx1) && xx < tw; xx++) {
                    const double ox = fmin(fx1, xx + 1) - fmax(fx0, xx), oy = fmin(fy1, yy + 1) - fmax(fy0, yy);
                    if (ox > 0 && oy > 0) { s += tg[yy * tw + xx] * ox * oy; a += ox * oy; }
                }
            t[y * w + x] = (float)(a > 0 ? s / a : 0);
        }
    sobel(t, w, h, te);
    *w_ = w; *h_ = h;
}

/* NCC zwischen dem Ausschnitt (x0, y0, nw, nh) der skalierten Vorlagenkanten te (Breite w) und pe (Breite pw) an der Lage (ox, oy). */
static double ncc_an(const float *pe, int pw, const float *te, int w, int x0, int y0, int nw, int nh, int ox, int oy)
{
    double ms = 0, mq = 0, sp = 0, sq = 0, sk = 0;
    for (int y = 0; y < nh; y++)
        for (int x = 0; x < nw; x++) {
            const double v = te[(y0 + y) * w + x0 + x], p = pe[(oy + y) * pw + ox + x];
            ms += v; mq += v * v; sp += p; sq += p * p; sk += p * v;
        }
    const double nn = (double)nw * nh, mt = ms / nn, st = sqrt(fmax(mq / nn - mt * mt, 1e-9)), mp = sp / nn, spp = sqrt(fmax(sq / nn - mp * mp, 1e-9));
    return (sk / nn - mp * mt) / (spp * st);
}

typedef struct { double ncc, sx, sy; int ox, oy; } kasten_treffer_t;

/* Bestes (NCC, sx, sy, Lage): Vorlage tg (tw x th) skaliert, mittig auf die Port-Groesse beschnitten, ueber alle Lagen in pe (pw x ph). */
static void suche(const float *pe, int pw, int ph, const float *tg, int tw, int th, double *best, double *bsx, double *bsy, kasten_treffer_t *tr)
{
    *best = -2; *bsx = 0; *bsy = 0;
    float *t = (float *)malloc(sizeof(float) * (size_t)(tw * 2) * (size_t)(th * 2)), *te = (float *)malloc(sizeof(float) * (size_t)(tw * 2) * (size_t)(th * 2));
    for (int iy = 0; iy <= 18; iy++)
        for (int ix = 0; ix <= 40; ix++) {
            const double sy = 0.84 + 0.02 * iy, sx = 0.50 + 0.02 * ix;
            const int w = (int)(tw * sx), h = (int)(th * sy);
            for (int y = 0; y < h; y++)                                   /* Flaechenmittel je Zielpunkt (Verkleinerung) bzw. bilinear */
                for (int x = 0; x < w; x++) {
                    const double fx0 = x / sx, fx1 = (x + 1) / sx, fy0 = y / sy, fy1 = (y + 1) / sy;
                    double s = 0, a = 0;
                    for (int yy = (int)fy0; yy < (int)ceil(fy1) && yy < th; yy++)
                        for (int xx = (int)fx0; xx < (int)ceil(fx1) && xx < tw; xx++) {
                            const double ox = fmin(fx1, xx + 1) - fmax(fx0, xx), oy = fmin(fy1, yy + 1) - fmax(fy0, yy);
                            if (ox > 0 && oy > 0) { s += tg[yy * tw + xx] * ox * oy; a += ox * oy; }
                        }
                    t[y * w + x] = (float)(a > 0 ? s / a : 0);
                }
            sobel(t, w, h, te);
            const int cw = w < pw - 4 ? w : pw - 4, ch = h < ph - 4 ? h : ph - 4;
            const int x0 = (w - cw) / 2 + 2, y0 = (h - ch) / 2 + 2, nw = cw - 4, nh = ch - 4;
            if (nw < 20 || nh < 20) continue;
            double ms = 0, mq = 0;
            for (int y = 0; y < nh; y++) for (int x = 0; x < nw; x++) { const double v = te[(y0 + y) * w + x0 + x]; ms += v; mq += v * v; }
            const double nn = (double)nw * nh, mt = ms / nn, st = sqrt(fmax(mq / nn - mt * mt, 1e-9));
            const int oxm = (pw - nw) / 2, oym = (ph - nh) / 2;            /* Lage nahe der Mitte (+-6), beide Flaechen sind mittig ausgerichtet */
            for (int oy = oym - 6 < 0 ? 0 : oym - 6; oy <= oym + 6 && oy + nh <= ph; oy++)
                for (int ox = oxm - 6 < 0 ? 0 : oxm - 6; ox <= oxm + 6 && ox + nw <= pw; ox++) {
                    double sp = 0, sq = 0, sk = 0;
                    for (int y = 0; y < nh; y++)
                        for (int x = 0; x < nw; x++) {
                            const double p = pe[(oy + y) * pw + ox + x], v = te[(y0 + y) * w + x0 + x];
                            sp += p; sq += p * p; sk += p * v;
                        }
                    const double mp = sp / nn, spp = sqrt(fmax(sq / nn - mp * mp, 1e-9));
                    const double ncc = (sk / nn - mp * mt) / (spp * st);
                    if (ncc > *best) { *best = ncc; *bsx = sx; *bsy = sy; if (tr) { tr->ncc = ncc; tr->sx = sx; tr->sy = sy; tr->ox = ox; tr->oy = oy; } }
                }
        }
    free(t); free(te);
}

typedef struct { double p[3], x[3], y[3], z[3], H; } kam_t;

/* RDT-Kamera Satz cut: Kopf +0x24 -> Kameratabelle, je 0x20 B: fov (u16 @+2), Auge @+4, Ziel @+16; Achsen wie camera_common.c (FUN_80053ca4). */
static void kam_lesen(const uint8_t *rdt, int cut, kam_t *k)
{
    const uint32_t kt = (uint32_t)rdt[0x24] | ((uint32_t)rdt[0x25] << 8) | ((uint32_t)rdt[0x26] << 16) | ((uint32_t)rdt[0x27] << 24);
    const uint8_t *s = rdt + kt + (uint32_t)cut * 0x20;
    int32_t w6[6];
    memcpy(w6, s + 4, sizeof w6);
    k->H = (double)((s[2] | (s[3] << 8)) >> 7);
    for (int i = 0; i < 3; i++) { k->p[i] = w6[i]; k->z[i] = w6[3 + i] - w6[i]; }
    double l = sqrt(k->z[0] * k->z[0] + k->z[1] * k->z[1] + k->z[2] * k->z[2]);
    for (int i = 0; i < 3; i++) k->z[i] /= l;
    k->x[0] = k->z[2]; k->x[1] = 0; k->x[2] = -k->z[0];
    l = sqrt(k->x[0] * k->x[0] + k->x[2] * k->x[2]); k->x[0] /= l; k->x[2] /= l;
    k->y[0] = k->z[1] * k->x[2] - k->z[2] * k->x[1]; k->y[1] = k->z[2] * k->x[0] - k->z[0] * k->x[2]; k->y[2] = k->z[0] * k->x[1] - k->z[1] * k->x[0];
}

static void kam_proj(const kam_t *k, const double w[3], double *sx, double *sy)
{
    const double d[3] = { w[0] - k->p[0], w[1] - k->p[1], w[2] - k->p[2] };
    const double cz = d[0] * k->z[0] + d[1] * k->z[1] + d[2] * k->z[2];
    *sx = 160 + k->H * (d[0] * k->x[0] + d[1] * k->x[1] + d[2] * k->x[2]) / cz;
    *sy = 120 + k->H * (d[0] * k->y[0] + d[1] * k->y[1] + d[2] * k->y[2]) / cz;
}

enum { KPW = 65, KPH = 180, KE = 20 };    /* Port-Flaeche 1300 x 3600 Welt, 20 je Bildpunkt */

/* Port-Flaeche aus dem Framedump (Cut 3): Waagrechte um w Grad, Senkrechte um s Grad geneigt abgetastet (Drehpunkt Flaechenmitte). */
static void port_flaeche(const uint8_t *rgb, const kam_t *k, int innen, double w_grad, double s_grad, float *out)
{
    const double tw = tan(w_grad * 3.14159265358979 / 180), ts = tan(s_grad * 3.14159265358979 / 180);
    for (int v = 0; v < KPH; v++)
        for (int u = 0; u < KPW; u++) {
            const double cu = u + .5 - KPW / 2.0, cv = v + .5 - KPH / 2.0;
            const double fu = KPW / 2.0 + cu + ts * cv, fv = KPH / 2.0 + cv + tw * cu;
            double w[3], sx, sy;
            if (innen) { w[0] = 5400; w[1] = -3650 + fv * KE; w[2] = -2000 - fu * KE; }    /* links Angel z -2000 .. rechts z -3300 */
            else       { w[0] = 4100 + fu * KE; w[1] = -3650 + fv * KE; w[2] = -2000; }    /* offene Tuer: freier Rand x 4100 links .. Angel rechts */
            kam_proj(k, w, &sx, &sy);
            out[v * KPW + u] = grau_bilinear(rgb, (float)sx, (float)sy);
        }
}

/* Neigung des Inhalts (Grad): Winkel der Waagrechten (senkrecht = 0) bzw. Senkrechten (senkrecht = 1) mit der besten NCC gegen die Vorlage im besten Massstab,
 * Lage +-3 um die beste; vorab = zusaetzliche Scherung der Abtastung (Gegenprobe). */
static double kasten_neigung(const uint8_t *rgb, const kam_t *k, int innen, int senkrecht, double vorab, const float *te, int w, int x0, int y0, int nw, int nh,
                             const kasten_treffer_t *tr)
{
    static float pg[KPW * KPH], pe[KPW * KPH];
    double best = -2, wbest = 0;
    for (int i = -32; i <= 32; i++) {
        const double a = i * 0.25;
        port_flaeche(rgb, k, innen, senkrecht ? 0 : vorab + a, senkrecht ? vorab + a : 0, pg);
        sobel(pg, KPW, KPH, pe);
        for (int oy = tr->oy - 3; oy <= tr->oy + 3; oy++)
            for (int ox = tr->ox - 3; ox <= tr->ox + 3; ox++) {
                if (ox < 0 || oy < 0 || ox + nw > KPW || oy + nh > KPH) continue;
                const double n = ncc_an(pe, KPW, te, w, x0, y0, nw, nh, ox, oy);
                if (n > best) { best = n; wbest = a; }
            }
    }
    return wbest;
}

static int kasten_werkzeug(const char *ppm, const char *teil)
{
    long n = 0;
    uint8_t *d = slurp(ppm, &n);
    uint8_t *bg = bss_rgb_pf(RE15_ASSET_PSX_DIR "/BSS/ROOM2060/BG06.BSS");
    uint8_t *rdt = slurp(RE15_ASSET_PSX_DIR "/STAGE2/ROOM2010.RDT", &n);
    uint8_t *rdt6 = slurp(RE15_ASSET_PSX_DIR "/STAGE2/ROOM2060.RDT", &n);
    if (!d || !bg || !rdt || !rdt6) { printf("FEHLER: Eingabe\n"); return 2; }
    const int innen = strcmp(teil, "tuer") != 0;
    kam_t k3, k6;
    kam_lesen(rdt, 3, &k3);          /* ROOM2010 Cut 3 @0xC0 */
    kam_lesen(rdt6, 6, &k6);         /* ROOM2060 Cut 6 @0x120: Auge (-5146,-2700,-442), H 208 */
    static float pg[KPW * KPH], pe[KPW * KPH], ps[KPW * KPH], pse[KPW * KPH];
    port_flaeche(d + 15, &k3, innen, 0, 0, pg);
    sobel(pg, KPW, KPH, pe);
    /* Gegenprobe Massstab: waagrecht auf 80 % gestaucht (Mitte bleibt), Rand = Randwert */
    for (int v = 0; v < KPH; v++)
        for (int u = 0; u < KPW; u++) {
            float s = (float)((u + 0.5 - KPW / 2.0) / 0.8 + KPW / 2.0 - 0.5);
            if (s < 0) s = 0; if (s > KPW - 1) s = KPW - 1;
            const int s0 = (int)s; const float f = s - (float)s0;
            ps[v * KPW + u] = pg[v * KPW + s0] * (1 - f) + pg[v * KPW + (s0 + 1 < KPW ? s0 + 1 : s0)] * f;
        }
    sobel(ps, KPW, KPH, pse);
    /* Vorlage: ROOM2060 Cut 6 ueber die Kamera (schrank_bild.py RUECKWAND / FLUEGEL / fluchtpunkt) */
    int tw, th;
    double dfl[3] = { 0, 0, 0 };
    if (innen) { tw = (int)lround((-3250.0 + 4830.0) / KE); th = (int)lround(3545.0 / KE); }
    else {
        tw = (int)lround((1538.0 - 290.0) / KE); th = (int)lround((15.0 + 3198.0) / KE);
        const double c = (130.8 - 160) / k6.H;                                  /* Fluchtpunkt x 130,8 auf dem Horizont */
        const double t = atan2(k6.x[0] - c * k6.z[0], -(k6.x[2] - c * k6.z[2]));
        dfl[0] = cos(t); dfl[2] = sin(t);
        if (dfl[0] * k6.z[0] + dfl[2] * k6.z[2] < 0) { dfl[0] = -dfl[0]; dfl[2] = -dfl[2]; }
    }
    float *tg = (float *)malloc(sizeof(float) * (size_t)(tw * th));
    for (int v = 0; v < th; v++)
        for (int u = 0; u < tw; u++) {
            double w[3], sx, sy;
            if (innen) { w[0] = -4830 + (u + .5) * KE; w[1] = -3545 + (v + .5) * KE; w[2] = 4500; }
            else {
                const double s_ = 1538 - (u + .5) * KE;                         /* links freier Rand, rechts Scharnier */
                w[0] = -4830 - s_ * dfl[0]; w[1] = -3198 + (v + .5) * KE; w[2] = 4500 - s_ * dfl[2];
            }
            kam_proj(&k6, w, &sx, &sy);
            tg[v * tw + u] = grau_bilinear(bg, (float)sx, (float)sy);
        }
    double ncc, sx, sy, ncc2, sx2, sy2;
    kasten_treffer_t tr = { 0, 0, 0, 0, 0 };
    suche(pe, KPW, KPH, tg, tw, th, &ncc, &sx, &sy, &tr);
    suche(pse, KPW, KPH, tg, tw, th, &ncc2, &sx2, &sy2, NULL);
    /* Scherung im besten Massstab */
    float *t = (float *)malloc(sizeof(float) * (size_t)(tw * 2) * (size_t)(th * 2)), *te = (float *)malloc(sizeof(float) * (size_t)(tw * 2) * (size_t)(th * 2));
    int w = 0, h = 0;
    vorlage_skaliert(tg, tw, th, tr.sx, tr.sy, t, te, &w, &h);
    const int cw = w < KPW - 4 ? w : KPW - 4, ch = h < KPH - 4 ? h : KPH - 4;
    const int x0 = (w - cw) / 2 + 2, y0 = (h - ch) / 2 + 2, nw = cw - 4, nh = ch - 4;
    const double nw_w = kasten_neigung(d + 15, &k3, innen, 0, 0, te, w, x0, y0, nw, nh, &tr);
    const double nw_s = kasten_neigung(d + 15, &k3, innen, 1, 0, te, w, x0, y0, nw, nh, &tr);
    /* Gegenprobe Scherung: zusaetzlich um 5 Grad geschert abgetastet (innen die Waagrechten, Fluegel die Senkrechten) */
    const double gp = kasten_neigung(d + 15, &k3, innen, !innen, 5.0, te, w, x0, y0, nw, nh, &tr);
    printf("KASTEN %s NCC %d SX %d SY %d VERHAELTNIS %d GEGENPROBE %d SCHERUNG_W %d SCHERUNG_S %d GEGENPROBE_SCHERUNG %d\n", innen ? "innen" : "tuer",
           (int)lround(ncc * 1000), (int)lround(sx * 100), (int)lround(sy * 100), (int)lround(sx / sy * 1000), (int)lround(sx2 / sy2 * 1000),
           (int)lround(nw_w * 100), (int)lround(nw_s * 100), (int)lround(gp * 100));
    free(t); free(te); free(tg); free(d); free(bg); free(rdt); free(rdt6);
    return 0;
}

/* ---- Werkzeuge -------------------------------------------------------------------------------------------------------- */
static uint8_t *ppm_lesen(const char *p)
{
    long n = 0;
    uint8_t *d = slurp(p, &n);
    if (!d || n < 230415) { free(d); return NULL; }
    return d;
}
static const uint8_t *ppm_px(const uint8_t *d) { return d + 15; }   /* "P6\n320 240\n255\n" */

/* Werkzeug "fluss" (Abnahme 0, Mangel 2: die Riegel mussten messen, dass es LAEUFT, nicht nur, dass es heller ist). Das Zeitmittel je Bildpunkt faellt weg: eine stehende Flaeche bleibt 0, ein
 * Muster, das sich mit festem Takt wiederholt, zeigt seinen Takt, ein wanderndes eine mit dem Abstand wachsende Verschiebung. */
static int fluss(char **argv)
{
    const int von = atoi(argv[3]), bis = atoi(argv[4]), x0 = atoi(argv[5]), y0 = atoi(argv[6]), x1 = atoi(argv[7]), y1 = atoi(argv[8]);
    const int n = bis - von + 1, w = x1 - x0, h = y1 - y0;
    if (n < 8 || w <= 20 || h <= 0) return 2;
    float *b = (float *)calloc((size_t)(n * w * h), sizeof(float)), *m = (float *)calloc((size_t)(w * h), sizeof(float));
    if (!b || !m) return 2;
    for (int t = 0; t < n; t++) {
        char pfad[512];
        snprintf(pfad, sizeof pfad, "%s/f_%06d.ppm", argv[2], von + t);
        uint8_t *d = ppm_lesen(pfad);
        if (!d) { printf("FEHLT %s\n", pfad); return 2; }
        const uint8_t *q = ppm_px(d);
        for (int y = 0; y < h; y++)
            for (int x = 0; x < w; x++) {
                const uint8_t *c = q + 3 * ((y0 + y) * 320 + x0 + x);
                const float g = ((float)c[0] + (float)c[1] + (float)c[2]) / 3.0f;
                b[(t * h + y) * w + x] = g;
                m[y * w + x] += g / (float)n;
            }
        free(d);
    }
    long aend = 0, proben = 0;
    for (int t = 0; t + 6 < n; t += 3, proben++)
        for (int i = 0; i < w * h; i++)
            if (fabsf(b[(size_t)(t + 6) * (size_t)(w * h) + (size_t)i] - b[(size_t)t * (size_t)(w * h) + (size_t)i]) > 16.0f) aend++;
    double null = 0;
    for (int t = 0; t < n; t++)
        for (int i = 0; i < w * h; i++) {
            float *r = &b[(size_t)t * (size_t)(w * h) + (size_t)i];
            *r -= m[i];
            null += (double)*r * (double)*r;
        }
    null /= (double)(n * w * h);
    printf("FLUSS");
    for (int lag = 1; lag <= 3; lag++) {
        int beste = 0; double k_beste = -1e30, k_null = 0;
        for (int d = -10; d <= 10; d++) {
            double sum = 0; long zahl = 0;
            for (int t = 0; t + lag < n; t++)
                for (int y = 0; y < h; y++)
                    for (int x = (d < 0 ? -d : 0); x < w - (d > 0 ? d : 0); x++) {
                        sum += (double)b[(size_t)(t * h + y) * (size_t)w + (size_t)x] * (double)b[(size_t)((t + lag) * h + y) * (size_t)w + (size_t)(x + d)];
                        zahl++;
                    }
            sum /= (double)zahl;
            if (d == 0) k_null = sum;
            if (sum > k_beste) { k_beste = sum; beste = d; }
        }
        printf(" %d %d %d", beste, (int)(100.0 * k_beste / (null > 0 ? null : 1)), (int)(100.0 * k_null / (null > 0 ? null : 1)));
    }
    printf(" AENDERUNG %ld VON %d\n", proben ? aend / proben : 0, w * h);
    free(b); free(m);
    return 0;
}

/* Werkzeug "karte": Figur aus der RDT-Variante (xxx1 = Elza, Charakter-Byte 4, Kopf probe_r36_elza_karte.c). */
static int karte(int argc, char **argv)
{
    const unsigned room = (unsigned)strtoul(argv[3], NULL, 16);
    scd_vm_init();
    re15_actor_init();
    re15_aot_init();
    re15_game_state_init();
    g_current_room_id = (int)room;
    g_gameflow.character = (room & 1u) ? 4 : 0;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100;
    pl->x = atoi(argv[4]); pl->y = atoi(argv[5]); pl->z = atoi(argv[6]); pl->rot_y = (int16_t)atoi(argv[7]);
    for (int a = 8; a < argc; a++) {
        char *c = strchr(argv[a], ':');
        if (!c) { printf("FAIL: '%s'\n", argv[a]); return 2; }
        re15_game_flag_set((uint8_t)strtol(argv[a], NULL, 0), (uint8_t)strtol(c + 1, NULL, 0), 1);
    }
    re15_savedata_t sd, back;
    uint16_t rr = 0;
    re15_savedata_capture(&sd, 0, 1);
    if (re15_memcard_save(argv[2], 0, &sd, "NACHTRAG30  ") != 0) { printf("FAIL: Karte %s\n", argv[2]); return 1; }
    if (re15_memcard_load(argv[2], 0, &back) != 0) { printf("FAIL: Ruecklesen\n"); return 1; }
    re15_game_state_init();
    if (re15_savedata_restore(&back, &rr) != 0) { printf("FAIL: Restore\n"); return 1; }
    printf("Karte %s: Raum 0x%04X, Figur %d, Lage (%d,%d,%d)\n", argv[2], (unsigned)rr, (int)g_gameflow.character, (int)pl->x, (int)pl->y, (int)pl->z);
    return rr == (uint16_t)room ? 0 : 1;
}

int main(int argc, char **argv)
{
    if (argc >= 8 && !strcmp(argv[1], "karte")) return karte(argc, argv);
    if (argc >= 9 && !strcmp(argv[1], "fluss")) return fluss(argv);
    if (argc >= 3 && !strcmp(argv[1], "etikett")) return etikett_werkzeug(argv[2]);      /* Spur PF */
    if (argc >= 4 && !strcmp(argv[1], "kasten")) return kasten_werkzeug(argv[2], argv[3]); /* Spur PF */
    if (argc >= 7 && !strcmp(argv[1], "bild")) {
        uint8_t *d = ppm_lesen(argv[2]);
        if (!d) { printf("kein Bild\n"); return 2; }
        const int x0 = atoi(argv[3]), y0 = atoi(argv[4]), x1 = atoi(argv[5]), y1 = atoi(argv[6]);
        long s = 0, n = 0;
        for (int y = y0; y < y1; y++) for (int x = x0; x < x1; x++) { const uint8_t *q = ppm_px(d) + 3 * (y * 320 + x); s += q[0] + q[1] + q[2]; n += 3; }
        printf("BILD %ld\n", n ? s / n : 0);
        return 0;
    }
    if (argc >= 8 && !strcmp(argv[1], "diff")) {
        uint8_t *a = ppm_lesen(argv[2]), *b = ppm_lesen(argv[3]);
        if (!a || !b) { printf("kein Bild\n"); return 2; }
        const int x0 = atoi(argv[4]), y0 = atoi(argv[5]), x1 = atoi(argv[6]), y1 = atoi(argv[7]);
        long s = 0, n = 0;
        for (int y = y0; y < y1; y++) for (int x = x0; x < x1; x++) for (int c = 0; c < 3; c++) {
            const int d = (int)ppm_px(a)[3 * (y * 320 + x) + c] - (int)ppm_px(b)[3 * (y * 320 + x) + c];
            s += d < 0 ? -d : d; n++;
        }
        printf("DIFF %ld\n", n ? s * 10 / n : 0);    /* Zehntel Graustufen */
        return 0;
    }
    if (argc >= 5 && !strcmp(argv[1], "endstufe")) {
        /* volle Mischung a (100 %) gegen b: b == (a * q) >> 15 fuer JEDEN Abtastwert (Tuersequenz, Spritzer, alles) */
        long na = 0, nb = 0;
        uint8_t *a = slurp(argv[2], &na), *b = slurp(argv[3], &nb);
        const int q = (int)strtol(argv[4], NULL, 0);
        if (!a || !b || na != nb) { printf("Laenge %ld %ld\n", na, nb); return 2; }
        long falsch = 0, laut = 0; int spitze = 0;
        for (long i = 0; i + 1 < na; i += 2) {
            const int16_t va = (int16_t)(uint16_t)(a[i] | (a[i + 1] << 8)), vb = (int16_t)(uint16_t)(b[i] | (b[i + 1] << 8));
            if ((int16_t)(((int32_t)va * q) >> 15) != vb) falsch++;
            if (va > 2000 || va < -2000) laut++;
            if (abs(va) > spitze) spitze = abs(va);
        }
        printf("ENDSTUFE falsch %ld von %ld, laut %ld, Spitze %d\n", falsch, na / 2, laut, spitze);
        return falsch ? 1 : 0;
    }
    if (argc >= 7 && !strcmp(argv[1], "luecke")) {
        /* laengste Folge von Spieltakten (1470 Stereo-Bilder) in an..aus, in denen RMS(a-b) unter der Schwelle liegt: "durchgehend" (Nachtrag 36) */
        long na = 0, nb = 0;
        uint8_t *a = slurp(argv[2], &na), *b = slurp(argv[3], &nb);
        const long an = atol(argv[4]), aus = atol(argv[5]), schwelle = atol(argv[6]), takt = 1470L * 2 * 2;
        if (!a || !b || na != nb) { printf("Laenge %ld %ld\n", na, nb); return 2; }
        long laengste = 0, folge = 0, leiseste = 0x7fffffffL;
        for (long t = an; t < aus && (t + 1) * takt <= na; t++) {
            double s = 0;
            for (long i = t * takt; i + 1 < (t + 1) * takt; i += 2) {
                const int d = (int)(int16_t)(uint16_t)(a[i] | (a[i + 1] << 8)) - (int)(int16_t)(uint16_t)(b[i] | (b[i + 1] << 8));
                s += (double)d * (double)d;
            }
            const long rms = lround(sqrt(s / (double)(takt / 2)));
            if (rms < leiseste) leiseste = rms;
            if (rms < schwelle) { if (++folge > laengste) laengste = folge; } else folge = 0;
        }
        printf("LUECKE %ld TAKTE, leisester Takt RMS %ld\n", laengste, leiseste);
        return 0;
    }
    /* Mechanik */
    fall_a();
    fall_f();
    fall_g();   /* Spur PF */
    fall_q();   /* Spur PF */
    fall_s();   /* Spur PF Nachbesserung 1 */
    for (int v = 0; v < 2; v++) {
        fall_b((uint16_t)(0x2030 + v));
        fall_c((uint16_t)(0x2010 + v));
        fall_c_use((uint16_t)(0x2010 + v));
        fall_d((uint16_t)(0x2000 + v));
        fall_e((uint16_t)(0x2030 + v));
    }
    printf("%s (%d Pruefungen)\n", s_fail ? "FAIL" : "PASS", s_pruef);
    free(s_daten);
    return s_fail;
}
