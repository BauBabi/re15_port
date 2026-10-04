/*
 * test_r35_cut11c0_fenster.c — Runde 35 Spur M: Riegel des Fenster-Ereignisses ROOM1120.
 * Dossier analysis/befunde_runde35/M_cut11c0_fenster.md, Code engine/src/fenster_1120.c,
 * engine/src/re2_fx.c (Raum-ESP, Ops 5/16/39/84), engine/src/enemy_ai_re2_crow.c (State 4 Sub 2).
 *
 *   glas      RE2-Raum-ESP room1090 registrieren (Ids 29 10 11 12 13 14 0C 19 hinter dem Kern), einen
 *             Splitter (Bank 0x10 Sub 2) spawnen und Bild fuer Bild gegen eine UNABHAENGIGE Rechnung der
 *             Schritte pruefen: Op 1 -> Op 16 (Boden gemerkt) -> Op 5 (Fall) -> Landung im erwarteten
 *             Bild -> Op 39 (Op A 84, Glitzern Bank 0x14) -> Op 84 (Platz frei, 2. Glitzern) -> Glitzern
 *             nach 6 Anim-Bildern frei; Flugrichtung -z bei Gier 0x400. Keine unbekannten Ops.
 *   kraehe    RE2-Kraehe +0x10E = 0x4002 -> INIT -> State 4 Sub 2: versteckt, Wandpass aus, wartet;
 *             +0x1D4 = 4 -> sichtbar, Clip 4, 7 Bewegungsbilder 300..240 (1890), dann ACTIVE Sub 4.
 *   ereignis  install/Hook/VM/AOT/Zeitlinie: Slot 4 AUTO Ereignis 24 auf dem Band, Spawn-Programm
 *             spawnt die Kraehe ueber die echte VM, Ausloeser ueber den AOT-Scan, T+0 Befehl, T+2 13
 *             Splitter, T+5/T+10 Knall, T+23 fertig, Schaden ab T+2 nur in Cut 1; Tor (9,73) und
 *             Einmaligkeit (9,79) als Gegenproben.
 *   knall     RE2-Raumbank room1090 (GLAS1090.EDT/.VH/.VB): Satz 0x21 = `00 00 7c 60` -> Prog 0 Ton 7,
 *             3 Zusatzlagen (Toene 7..10, alle VAG 5), Wellen im VB vorhanden.
 *   karte     Messwerkzeug (kein Riegel): Speicherkarte mit einem Stand in ROOM1120, (9,73)=1, Spieler
 *             im Gang suedlich des Bands (fuer den exe-Haken).
 */
#include "re15_actor.h"
#include "re15_scd.h"
#include "re15_room.h"
#include "re15_aot.h"
#include "re15_ai_flavor.h"
#include "re15_fenster1120.h"
#include "re15_savedata.h"
#include "re15_memcard.h"
#include "re2_fx.h"
#include "re15_vab.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define RE15_XSTR_(x) #x
#define RE15_XSTR(x)  RE15_XSTR_(x)

extern int re15_re2crow_tick(int slot);
extern void scd_register_current_rdt(const re15_rdt_t *rdt);

static int fail(int code, const char *m) { printf("FAIL %d: %s\n", code, m); return 1; }
static uint16_t u16(const uint8_t *b, int o) { return (uint16_t)(b[o] | (b[o + 1] << 8)); }
static int16_t  s16(const uint8_t *b, int o) { return (int16_t)u16(b, o); }

static uint8_t *datei(const char *name, size_t *n)
{
    char p[1024];
    snprintf(p, sizeof p, "%s/../RE2/%s", RE15_XSTR(RE15_ASSETS_PATH), name);
    FILE *f = fopen(p, "rb");
    if (!f) { printf("kann %s nicht oeffnen\n", p); return NULL; }
    fseek(f, 0, SEEK_END); long l = ftell(f); fseek(f, 0, SEEK_SET);
    uint8_t *b = (uint8_t *)malloc((size_t)l);
    if (b && fread(b, 1, (size_t)l, f) != (size_t)l) { free(b); b = NULL; }
    fclose(f);
    if (b && n) *n = (size_t)l;
    return b;
}

static uint8_t *s_core, *s_raum;
static size_t   s_core_n, s_raum_n;
static int registrieren(void)
{
    if (!s_core) s_core = datei("CORE00.ESP", &s_core_n);
    if (!s_raum) s_raum = datei("GLAS1090.ESP", &s_raum_n);
    if (!s_core || !s_raum) return -1;
    if (re2fx_register_core(s_core, s_core_n) != 0) return -2;
    return re2fx_register_raum(s_raum, s_raum_n);
}

/* flacher Boden y 0, Kontakt = P.y > 0 (RE2 FUN_8004fba0 Grundwerte @0x8004fc3c / @0x8004fc48-58) */
static int32_t boden_flach(const int32_t p[3], int r, uint32_t mask, int a3, int *kontakt)
{ (void)r; (void)mask; (void)a3; *kontakt = p[1] > 0; return 0; }
static int32_t kein_wasser(int32_t x, int32_t z) { (void)x; (void)z; return 0; }

static int lebende_bank(unsigned bank)
{
    int n = 0;
    for (int i = 0; i < RE2FX_PLAETZE; i++) {
        const uint8_t *b = re2fx_platz(i);
        if (u16(b, 0x18) != 0 && b[0x1C] == bank) n++;
    }
    return n;
}

/* ============================================================================================ */
static int pruef_glas(void)
{
    int rc = registrieren();
    if (rc != 0) { printf("registrieren rc=%d\n", rc); return fail(1, "Registrierung Kern+Raum"); }
    for (unsigned b = 0x10; b <= 0x14; b++)
        if (!re2fx_bank_registriert(b)) return fail(2, "Raum-Bank 0x10..0x14 fehlt");
    if (!re2fx_bank_registriert(0x29) || !re2fx_bank_registriert(0x19) || !re2fx_bank_registriert(0x0C))
        return fail(3, "Raum-Ids 29/19/0C fehlen");
    if (!re2fx_bank_registriert(5)) return fail(4, "Kern-Bank 5 verloren");
    /* Gegenprobe: Kern neu -> Raum weg (re2fx_register_core loescht die Raum-ESP mit). */
    re2fx_register_core(s_core, s_core_n);
    if (re2fx_bank_registriert(0x10)) return fail(5, "Raum-Bank ueberlebt Kern-Neuregistrierung");
    if (re2fx_register_raum(s_raum, s_raum_n) != 0) return fail(6, "Raum erneut");

    re2fx_reset();
    re2fx_boden_hook = boden_flach;
    re2fx_wasser_hook = kein_wasser;
    /* sub15 @0x0054-Form: Bank 0x10 Sub 2 Skala 0x2800, Gier 0x400 (Port), Einheitsmatrix, Lage. */
    const int16_t ofs[4] = { 4700, -2200, 10200, 0 };
    int i = re2fx_spawn_sofort(0x10022800u, 0x0400, re2fx_einheitsmatrix, ofs);
    if (i < 0 || i >= RE2FX_PLAETZE) return fail(10, "Spawn");
    const uint8_t *b = re2fx_platz(i);
    if (u16(b, 0x18) != 0xA003 || b[0] != 1 || b[1] != 0) return fail(11, "Schritt 0 (Op A 1, Status 0xA003)");
    if (u16(b, 0x3A) != 0x2800 || b[0x1C] != 0x10 || b[0x1E] != 2) return fail(12, "Skala/Bank/Sub");

    /* UNABHAENGIGE Erwartung aus den Datei-Bytes (Schritt 1 Sub 2: v = (64,-60,0), a = (0,12,0)):
     * Bild 1: Op 1 laedt Schritt 1, Op 16 im selben Bild; ab Bild 2 Op 5 mit Lage y = -2200 + Y(t-1),
     * Y(k) = sum_{j<k} (-60 + 12 j). Landung = erstes t >= 2 mit -2200 + Y(t-1) > 0. */
    int t_erwartet = -1;
    for (int t = 2; t < 200; t++) {
        int k = t - 1; int y = -2200 + (-60 * k + 6 * k * (k - 1));
        if (y > 0) { t_erwartet = t; break; }
    }
    int t_land = -1, t_frei = -1;
    for (int t = 1; t <= 60; t++) {
        unsigned vor39 = re2fx_op_zaehler(39);
        re2fx_tick();
        if (t == 1) {
            if (re2fx_op_zaehler(1) < 1 || re2fx_op_zaehler(16) != 1) return fail(13, "Bild 1: Op 1 + Op 16");
            if (b[0] != 0 || b[1] != 5) return fail(14, "nach Op 16: Op A = +0x0B (0), Op B 5");
            if (u16(b, 0x18) != 0xB003) return fail(15, "Status 0xB003 (Schritt 0 +0x12)");
            if (s16(b, 0x0E) != -48) return fail(16, "vy nach Bild 1 = -60 + 12");
        }
        if (t_land < 0 && re2fx_op_zaehler(39) > vor39) {
            t_land = t;
            if (b[0] != 84 || b[1] != 0) return fail(17, "Op 39: Op A 84 / Op B 0");
            if (s16(b, 0x38) >= 10200) return fail(18, "Splitter flog nicht nach -z (Gier 0x400)");
        }
        if (t_land > 0 && t_frei < 0 && t == t_land + 1) {
            /* Op 84 (Update-Pass) gibt den Platz frei (`sh zero,24` @0x80025378) und sein Spawn
             * 0x14000000 (@0x80025384) nimmt den ersten freien Platz von 95 abwaerts (@0x8001cc44-6c)
             * = denselben: er traegt jetzt Bank 0x14. */
            if (re2fx_op_zaehler(84) == 1 && b[0x1C] == 0x14) t_frei = t;
        }
    }
    printf("Landung Bild %d (erwartet %d), Op84/Platz neu Bild %d, Op5 %u Op16 %u Op39 %u Op84 %u unbekannt %u\n",
           t_land, t_erwartet, t_frei, re2fx_op_zaehler(5), re2fx_op_zaehler(16), re2fx_op_zaehler(39),
           re2fx_op_zaehler(84), re2fx_op_unbekannt());
    if (t_land != t_erwartet) return fail(19, "Landebild weicht von der unabhaengigen Rechnung ab");
    if (t_frei != t_land + 1) return fail(20, "Op 84 gibt den Platz nicht im Folgebild frei (Glitzern uebernimmt ihn)");
    if (re2fx_op_unbekannt() != 0) return fail(21, "unbekannte Ops / Lesefehler");
    if (lebende_bank(0x14) != 0) return fail(22, "Glitzern Bank 0x14 nicht nach 6 Anim-Bildern frei");
    /* Glitzern zaehlen: direkt nach der Landung zwei Plaetze Bank 0x14 (Op 39 + Op 84). */
    re2fx_reset();
    i = re2fx_spawn_sofort(0x10022800u, 0x0400, re2fx_einheitsmatrix, ofs);
    for (int t = 1; t <= t_erwartet + 1; t++) re2fx_tick();
    int glitzern = lebende_bank(0x14);
    if (glitzern != 2) { printf("Glitzern %d\n", glitzern); return fail(23, "zwei Glitzer-Plaetze (Op 39 + Op 84)"); }
    re2fx_boden_hook = NULL; re2fx_wasser_hook = NULL;
    printf("OK glas\n");
    return 0;
}

/* ============================================================================================ */
static int pruef_kraehe(void)
{
    re15_actor_init();
    re15_ai_flavor_set(RE15_AI_FLAVOR_RE2);
    int slot = RE15_FENSTER_KRAEHE_EM + 1;
    re15_actor_t *k = &g_actors[slot];
    k->active = 1; k->type = 0x21; k->state = 0; k->flags = 0x01;
    k->x = 5400; k->y = -2500; k->z = 12100; k->rot_y = 0x0418;
    k->re2z_f10e = RE15_FENSTER_KRAEHE_F10E;
    re15_re2crow_zwang(slot, 1);
    g_actors[RE15_ACTOR_SLOT_PLAYER].active = 1;
    g_actors[RE15_ACTOR_SLOT_PLAYER].x = 5000; g_actors[RE15_ACTOR_SLOT_PLAYER].z = 3000;
    re15_re2crow_tick(slot);
    if (k->state != 4 || k->sub_state_1 != 2 || k->sub_state_2 != 1) return fail(30, "INIT -> State 4 Sub 2 Phase 1");
    if (!k->crow_hide || !(k->flags & 0x8u)) return fail(31, "P0: versteckt + Wandpass aus");
    if (!(k->re2z_f10e & 0x4000u) || !(k->re2c_flags22a & 1u)) return fail(32, "+0x10E|0x4000 / +0x22A|1");
    for (int t = 0; t < 10; t++) re15_re2crow_tick(slot);
    if (k->x != 5400 || k->z != 12100 || !k->crow_hide) return fail(33, "P1 ohne Befehl: steht versteckt");
    re15_re2crow_befehl(slot, 4);
    re15_re2crow_tick(slot);
    if (k->sub_state_2 != 2 || k->crow_hide || k->speed_h != 300 || k->motion != 4)
        return fail(34, "P1 mit +0x1D4&4: sichtbar, Tempo 300, Clip 4");
    if (re15_re2crow_befehl_lesen(slot) != 0) return fail(35, "+0x1D4 := 0");
    int32_t z0 = k->z;
    int bilder = 0;
    while (k->state == 4 && bilder < 20) { re15_re2crow_tick(slot); bilder++; }
    int32_t weg = z0 - k->z;
    printf("Durchflug %d Bilder, z %d -> %d (%d), Zustand %d/%d, flags 0x%02x\n",
           bilder, (int)z0, (int)k->z, (int)weg, k->state, k->sub_state_1, k->flags);
    if (bilder != 7) return fail(36, "7 Bewegungsbilder (Zaehler 6..0)");
    /* 300+290+...+240 = 1890 entlang dir 0x418: z -= sin(0x418)*v >> 12 je Bild (~0.9993) */
    if (weg < 1880 || weg > 1890) return fail(37, "Weg ~1890 nach -z");
    if (k->state != 1 || k->sub_state_1 != 4) return fail(38, "danach ACTIVE Sub 4 (0x80104078(e,1,4))");
    if (k->flags & 0x8u) return fail(39, "Wandpass wieder an");
    if (re15_re2crow_zwang_ist(slot)) return fail(40, "Zwang endet nach dem Skript");
    /* RE1.5-Flavor: Uebergabe an das RE1.5-Hirn ab INIT. */
    re15_ai_flavor_set(RE15_AI_FLAVOR_RE15);
    k->state = 0; k->re2z_f10e = RE15_FENSTER_KRAEHE_F10E; re15_re2crow_zwang(slot, 1);
    re15_re2crow_tick(slot); re15_re2crow_befehl(slot, 4);
    for (int t = 0; t < 12 && k->state == 4; t++) re15_re2crow_tick(slot);
    if (k->state != 0) return fail(41, "RE1.5-Flavor: nach dem Durchflug State 0 (RE1.5-INIT)");
    re15_ai_flavor_set(RE15_AI_FLAVOR_RE2);
    printf("OK kraehe\n");
    return 0;
}

/* ============================================================================================ */
static int s_knall_n = 0, s_knall_satz = -1;
static void knall_spion(int satz) { s_knall_n++; s_knall_satz = satz; }

static int pruef_ereignis(void)
{
    if (registrieren() != 0) return fail(50, "Registrierung");
    re2fx_reset();
    scd_vm_init();
    re15_actor_init();
    re15_aot_init();
    re15_ai_flavor_set(RE15_AI_FLAVOR_RE2);
    static re15_rdt_t dummy;
    memset(&dummy, 0, sizeof dummy);
    scd_register_current_rdt(&dummy);
    g_current_room_id = RE15_FENSTER_RAUM;
    re15_fenster1120_se_hook = knall_spion;

    /* Gegenprobe Tor: (9,73)=0 -> nichts */
    re15_game_flag_set(9, RE15_FENSTER_TOR_BIT, 0);
    re15_fenster1120_install(RE15_FENSTER_RAUM);
    if (re15_fenster1120_phase() != RE15_FENSTER_AUS || g_aot.slots[RE15_FENSTER_SLOT].active)
        return fail(51, "ohne (9,73) kein Ereignis");
    /* Gegenprobe Raum */
    re15_game_flag_set(9, RE15_FENSTER_TOR_BIT, 1);
    g_current_room_id = 0x1121;
    re15_fenster1120_install(0x1121);
    if (re15_fenster1120_phase() != RE15_FENSTER_AUS) return fail(52, "ROOM1121 bleibt ohne Ereignis");
    g_current_room_id = RE15_FENSTER_RAUM;

    re15_fenster1120_install(RE15_FENSTER_RAUM);
    const re15_aot_t *a = &g_aot.slots[RE15_FENSTER_SLOT];
    if (re15_fenster1120_phase() != RE15_FENSTER_SCHARF) return fail(53, "scharf");
    if (!a->active || a->type != RE15_AOT_TYPE_AUTO_EVENT || a->event_id != RE15_FENSTER_EREIGNIS ||
        a->sce_flags != RE15_FENSTER_SAT)
        return fail(54, "Slot 4 = AUTO-Ereignis 24, sat 0x41");
    if (a->x - a->half_w != RE15_FENSTER_ZONE_X0 || a->x + a->half_w != RE15_FENSTER_ZONE_X1 ||
        a->z - a->half_h != RE15_FENSTER_ZONE_Z0 || a->z + a->half_h != RE15_FENSTER_ZONE_Z1)
        return fail(55, "Band x 3500..6650 z 4300..6400");
    /* Das Spawn-Programm laeuft ueber die echte VM (install hat scd_event_fire(24) gerufen). */
    scd_vm_tick();
    int ks = RE15_FENSTER_KRAEHE_EM + 1;
    re15_actor_t *k = &g_actors[ks];
    printf("Kraehe Slot %d: aktiv %d Typ 0x%02x Lage (%d,%d,%d) rot 0x%x\n", ks, k->active, k->type,
           (int)k->x, (int)k->y, (int)k->z, (unsigned)k->rot_y);
    if (!k->active || k->type != 0x21 || k->x != 5400 || k->y != -2500 || k->z != 12100 || k->rot_y != 0x0418)
        return fail(56, "Spawn-Programm: Kraehe (5400,-2500,12100) dir 0x418");
    re15_fenster1120_tick();
    if (re15_fenster1120_kraehe_slot() != ks || k->re2z_f10e != RE15_FENSTER_KRAEHE_F10E || !re15_re2crow_zwang_ist(ks))
        return fail(57, "Ausstattung +0x10E 0x4002 + Zwang vor der KI");
    re15_re2crow_tick(ks);
    if (k->state != 4 || !k->crow_hide) return fail(58, "Kraehe versteckt in State 4");
    if (re15_fenster1120_schaden_sichtbar(1)) return fail(59, "vor dem Ausloesen kein Schaden");

    /* Ausloeser ueber den AOT-Scan: Spieler ins Band. */
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->x = 5000; pl->z = 5000; pl->floor = 0;
    re15_aot_scan(pl->x, pl->z, 1);
    /* AUTO-Ereignis: der Scan merkt die Nummer (aot_common.c default-Zweig), der Spielschritt
     * feuert sie (game_step_common.c `scd_event_fire(g_aot.fired_event_id_this_frame)`). */
    if (g_aot.fired_event_id_this_frame != RE15_FENSTER_EREIGNIS) return fail(59, "AOT-Scan meldet Ereignis 24 nicht");
    scd_event_fire(g_aot.fired_event_id_this_frame);
    scd_vm_tick();
    if (!re15_game_flag_get(9, RE15_FENSTER_BIT)) return fail(60, "Ausloeser-Programm setzt (9,79)");
    if (g_aot.slots[RE15_FENSTER_SLOT].type != RE15_AOT_TYPE_NONE) return fail(61, "Aot_reset(4) -> sce 0");
    int sp_t = -1, k1 = -1, k2 = -1, fertig = -1, schaden_t = -1;
    for (int f = 0; f < 30; f++) {
        int vor_sp = re15_fenster1120_splitter_gespawnt(), vor_k = s_knall_n;
        re15_fenster1120_tick();
        int t = re15_fenster1120_takt();
        if (t == 0 && re15_re2crow_befehl_lesen(ks) != 4) return fail(62, "T+0: +0x1D4 = 4");
        if (re15_fenster1120_splitter_gespawnt() > vor_sp && sp_t < 0) sp_t = t;
        if (s_knall_n > vor_k) { if (k1 < 0) k1 = t; else k2 = t; }
        if (schaden_t < 0 && re15_fenster1120_schaden_sichtbar(1)) schaden_t = t;
        if (fertig < 0 && re15_fenster1120_phase() == RE15_FENSTER_FERTIG) fertig = t;
        re2fx_tick();
    }
    printf("Splitter T+%d (%d), Knall T+%d/T+%d Satz 0x%02x, Schaden ab T+%d, fertig T+%d\n", sp_t,
           re15_fenster1120_splitter_gespawnt(), k1, k2, s_knall_satz, schaden_t, fertig);
    if (sp_t != RE15_FENSTER_T_SPLITTER || re15_fenster1120_splitter_gespawnt() != RE15_FENSTER_SPLITTER_N)
        return fail(63, "T+2: 13 Splitter");
    if (k1 != RE15_FENSTER_T_KNALL1 || k2 != RE15_FENSTER_T_KNALL2 || s_knall_satz != RE15_FENSTER_SE_SATZ)
        return fail(64, "Knall T+5 und T+10, Satz 0x21");
    if (schaden_t != RE15_FENSTER_T_SPLITTER) return fail(65, "Schaden ab T+2");
    if (fertig != RE15_FENSTER_T_ENDE) return fail(66, "fertig T+23");
    if (re15_fenster1120_schaden_sichtbar(0) || re15_fenster1120_schaden_sichtbar(2)) return fail(67, "Schaden nur Cut 1");
    /* Splitter-Lagen = Abbildung der RE2-Saetze (Stichprobe @0x0054 -> (4700,-2200,10200)). */
    int32_t p[3];
    re15_fenster1120_abbilden(-2200, -2200, -13600, p);
    if (p[0] != 4700 || p[1] != -2200 || p[2] != 10200) return fail(68, "Abbildung RE2->RE1.5");
    /* Einmaligkeit: Wiedereintritt mit (9,79)=1 -> kein Slot, kein Spawn, Schaden steht. */
    re15_aot_init();
    re15_fenster1120_install(RE15_FENSTER_RAUM);
    if (re15_fenster1120_phase() != RE15_FENSTER_AUS || g_aot.slots[RE15_FENSTER_SLOT].active)
        return fail(69, "Wiedereintritt: kein zweites Ereignis");
    if (!re15_fenster1120_schaden_sichtbar(1)) return fail(70, "Wiedereintritt: Fenster bleibt beschaedigt");
    re15_fenster1120_se_hook = NULL;
    printf("OK ereignis\n");
    return 0;
}


/* ============================================================================================ */
static int pruef_knall(void)
{
    size_t ne = 0, nv = 0, nb = 0;
    uint8_t *edt = datei("GLAS1090.EDT", &ne), *vh = datei("GLAS1090.VH", &nv), *vb = datei("GLAS1090.VB", &nb);
    if (!edt || !vh || !vb) return fail(90, "GLAS1090.EDT/.VH/.VB fehlen");
    if (ne != 192 || nv != 3616 || nb != 119904) return fail(91, "Groessen = RDT-Schnitt (192/3616/119904)");
    const uint8_t *w = edt + RE15_FENSTER_SE_SATZ * 4;
    if (w[0] != 0x00 || w[1] != 0x00 || w[2] != 0x7c || w[3] != 0x60) return fail(92, "EDT @0x84 = 00 00 7c 60");
    re15_edt_rec_t r;
    if (re15_edt_decode(edt, RE15_FENSTER_SE_SATZ, &r) != 0 || r.empty) return fail(93, "Satz 0x21 leer");
    if (r.prog != 0 || r.tone != 7 || r.extra != 3 || r.prio_nib != 0xC) return fail(94, "Prog 0 Ton 7, 3 Zusatzlagen, Prio 0xC");
    static re15_vab_t vab;
    if (re15_vab_parse(vh, nv, &vab) != 0) return fail(95, "VH");
    int vags[8], toene[8];
    int n = re15_edt_resolve_layers_ex(edt, &vab, RE15_FENSTER_SE_SATZ, vags, toene, 8);
    printf("Satz 0x21: %d Lagen:", n);
    for (int k = 0; k < n; k++) printf(" Ton %d VAG %d (vol %d mitte %d)", toene[k], vags[k],
                                        vab.tones[toene[k]].vol, vab.tones[toene[k]].center_note);
    printf("\n");
    if (n != 4) return fail(96, "4 Lagen (Ton 7 + 3)");
    for (int k = 0; k < n; k++) {
        if (toene[k] != 7 + k) return fail(97, "Toene 7..10");
        if (vags[k] != vags[0] || vags[k] < 0 || vags[k] >= vab.vag_count) return fail(98, "alle Lagen dieselbe Welle (VAG 5)");
        if (vab.samples[vags[k]].offset + vab.samples[vags[k]].size > nb) return fail(99, "Welle im VB");
    }
    printf("OK knall\n");
    return 0;
}

/* ============================================================================================ */
static int karte(const char *path, int gebrochen)
{
    scd_vm_init();
    re15_actor_init();
    re15_aot_init();
    g_current_room_id = RE15_FENSTER_RAUM;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100;
    /* Gang Cut 1 (RVD @0x128: x 1000..8500, z -2000..13000), suedlich des Bands, Blick nach Norden. */
    pl->x = 5000; pl->y = 0; pl->z = 2600; pl->rot_y = 0xC00;   /* 0xC00 = Blick +z (Norden) */
    g_scd.cam_id = 1;   /* Stand mit Cut 1 (savedata camera_cut v3) */
    re15_game_flag_set(9, RE15_FENSTER_TOR_BIT, 1);
    if (gebrochen) re15_game_flag_set(9, RE15_FENSTER_BIT, 1);
    re15_savedata_t sd;
    re15_savedata_capture(&sd, 0, 1);
    if (re15_memcard_save(path, 0, &sd, "LEON  1120") != 0) return fail(80, "Karte schreiben");
    re15_savedata_t back; uint16_t rr = 0;
    if (re15_memcard_load(path, 0, &back) != 0 || re15_savedata_restore(&back, &rr) != 0) return fail(81, "Ruecklesen");
    printf("Karte %s: Raum 0x%04X, (9,73)=%d (9,79)=%d\n", path, (unsigned)rr,
           re15_game_flag_get(9, RE15_FENSTER_TOR_BIT), re15_game_flag_get(9, RE15_FENSTER_BIT));
    return rr == RE15_FENSTER_RAUM ? 0 : fail(82, "Raum");
}

/* ============================================================================================ */
/* Nachbesserung 1: Satzform p0/p1 des Ereignis-Platzes gegen die ausgelieferten Bytes.
 *   ROOM1050.RDT @0x0C22 `2c 07 03 31 ... ff 00 18 02 00 00` (p0 @0x0C30, p1 @0x0C32)
 *   ROOM1020.RDT @0x1E18 `2c 06 03 41 ... ff 00 18 03 00 00` (AUTO-Form sat 0x41 wie Slot 4)
 * und die Bedeutung von p0 = 0x00FF: 0x8003ee3c nimmt bei a0 >= 0xa (`sltiu v0,a2,0xa` @0x8003ee54)
 * den ERSTEN FREIEN Ereignis-Faden — im Port scd_event_fire (erster freier Faden ab SCD_EVENT_SLOT_FIRST). */
static uint8_t *psx_datei(const char *rel, size_t *n)
{
    char p[1024];
    snprintf(p, sizeof p, "%s/%s", RE15_XSTR(RE15_ASSETS_PATH), rel);
    FILE *f = fopen(p, "rb");
    if (!f) { printf("kann %s nicht oeffnen\n", p); return NULL; }
    fseek(f, 0, SEEK_END); long l = ftell(f); fseek(f, 0, SEEK_SET);
    uint8_t *b = (uint8_t *)malloc((size_t)l);
    if (b && fread(b, 1, (size_t)l, f) != (size_t)l) { free(b); b = NULL; }
    fclose(f);
    if (b && n) *n = (size_t)l;
    return b;
}

static int satz_pruefen(const char *rel, size_t off, uint8_t sat_soll)
{
    size_t n = 0;
    uint8_t *b = psx_datei(rel, &n);
    if (!b || off + 20 > n) { free(b); return fail(90, "RDT fehlt/zu kurz"); }
    const uint8_t *r = b + off;
    uint16_t p0 = u16(r, 14), p1 = u16(r, 16);
    printf("%s @0x%04zx: op 0x%02x sce %u sat 0x%02x p0 0x%04x p1 0x%04x\n", rel, off, r[0], r[2], r[3], p0, p1);
    int ok = r[0] == 0x2c && r[2] == RE15_FENSTER_SCE && r[3] == sat_soll &&
             p0 == RE15_FENSTER_P0 && (p1 & 0xff) == (RE15_FENSTER_P1 & 0xff);
    free(b);
    return ok ? 0 : fail(91, "Satzform weicht von RE15_FENSTER_P0/P1 ab");
}

static int pruef_satzform(void)
{
    if (satz_pruefen("STAGE1/ROOM1050.RDT", 0x0C22, 0x31)) return 1;
    if (satz_pruefen("STAGE1/ROOM1020.RDT", 0x1E18, RE15_FENSTER_SAT)) return 1;
    if ((RE15_FENSTER_P1 >> 8) != RE15_FENSTER_EREIGNIS) return fail(92, "p1 Oberbyte = Ereignis (@0x80043100)");
    if (RE15_FENSTER_P0 < 0xa) return fail(93, "p0 >= 0xa = freier Faden (@0x8003ee54)");

    /* Freier Faden statt festem Platz: Platz FIRST belegt -> das naechste Ereignis laeuft in FIRST+1. */
    static const uint8_t prog[] = { 0x01, 0x00 };          /* Evt_end */
    static re15_rdt_t rdt;
    memset(&rdt, 0, sizeof rdt);
    rdt.sub_scd[RE15_FENSTER_EREIGNIS] = prog;
    scd_vm_init();
    scd_register_current_rdt(&rdt);
    g_current_room_id = 0x1000;                            /* kein Port-Haken greift */
    re15_fenster1120_install(0x1000);
    int s1 = scd_event_fire(RE15_FENSTER_EREIGNIS);
    int s2 = scd_event_fire(RE15_FENSTER_EREIGNIS);
    printf("Ereignis-Faeden: %d dann %d (erster freier ab %d)\n", s1, s2, SCD_EVENT_SLOT_FIRST);
    if (s1 != SCD_EVENT_SLOT_FIRST || s2 != SCD_EVENT_SLOT_FIRST + 1)
        return fail(94, "p0 0x00FF: erster freier Ereignis-Faden");
    return 0;
}

int main(int argc, char **argv)
{
    const char *was = argc > 1 ? argv[1] : "glas";
    if (!strcmp(was, "satzform")) return pruef_satzform();
    if (!strcmp(was, "glas"))     return pruef_glas();
    if (!strcmp(was, "kraehe"))   return pruef_kraehe();
    if (!strcmp(was, "ereignis")) return pruef_ereignis();
    if (!strcmp(was, "knall"))    return pruef_knall();
    if (!strcmp(was, "karte"))    return karte(argc > 2 ? argv[2] : "re15_card.mcr", argc > 3 && !strcmp(argv[3], "gebrochen"));
    return fail(99, "unbekannter Teil");
}
