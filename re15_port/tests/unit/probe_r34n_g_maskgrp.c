/* probe_r34n_g_maskgrp.c — Spur G2 (Runde 34 Nacht), PIN unit_r34n_g_maskgrp.
 *
 * Haelt die Masken-Sichtbarkeit je Gruppe fest (engine/src/masken_gruppen.c,
 * include/re15_masken_gruppen.h), mit ECHTEN RDT-Bytes und der ECHTEN SCD-VM:
 *
 *   A  ROOM1150 Cut 2 (sprite.pri @0x0066C, Kopf 0B 00 36 00): Aufbau -> Zahl 54, alle Byte0 == 1,
 *      Byte1 = Gruppe+1 (12/9/10/10/7/1/1/1/1/1/1 Masken; Records 48..53 = Gruppen 6..11 =
 *      die Buchstaben H E A V E N). Opcode-Bytes 45 05 00 45 06 02 01 00 ueber die VM in EINEM
 *      Takt: Record 48 Byte0 = 0, Record 49 Byte0 = 2 (ganzes Byte, @0x800396e0) und damit
 *      unsichtbar (Bit 0, @0x800395f0), 47/50 unveraendert. 45 05 01 -> wieder an. Neuaufbau ->
 *      alles an. Cut 0 -> Zahl 2, 0x45 Gruppe 6 trifft nichts. NULL-Cut 5 -> Zahl 0, sichtbar.
 *   B  TAKT: echter Raumstart ROOM1150 und ROOM1151 (scd_room_reenter wie der Tuerweg), Eintritts-
 *      Cut 0 aufgebaut wie room_common.c Schritt 9, dann Cut 2 angefordert und je Takt wie die
 *      Hauptschleife: re15_cam_present_tick() -> Aufbau, scd_vm_tick(). 200 Takte: Record 48 ist
 *      ab dem Aufbau an, danach Wechsel EXAKT alle 20 Takte (sub05: Sleep 20 @0x010C8/@0x010DE),
 *      alle sechs Buchstaben gleich, die anderen 48 Masken immer an.
 *   C  CUT-WECHSEL mitten in einer AUS-Phase (Cut 2 -> 0 -> 2): sofort wieder AN (Aufbau
 *      FUN_800392d4 @0x80021c28), das naechste AUS erst beim naechsten `:= 0` von sub05 — 20
 *      Takte nach dem ausgelassenen `:= 1`.
 *
 * Mutationsprobe (Dossier §9.3): ohne `s_op_table[0x45] = op_col_chg_set` faellt A und B.
 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "re15_masken_gruppen.h"
#include "re15_rdt.h"
#include "re15_scd.h"
#include "re15_actor.h"
#include "re15_room.h"
#include "re15_aot.h"
#include "re15_msg.h"
#include "re15_inventory.h"
#include "re15_files.h"

#ifndef RE15_ASSET_PSX_DIR
#define RE15_ASSET_PSX_DIR "shared_assets/PSX"
#endif

static int fails = 0;
#define CHECK(c, ...) do { if (!(c)) { printf("FAIL: " __VA_ARGS__); printf("\n"); fails++; } \
                           else { printf("  PASS: " __VA_ARGS__); printf("\n"); } } while (0)

static uint8_t *datei(const char *pfad, size_t *n)
{
    FILE *f = fopen(pfad, "rb");
    if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    uint8_t *b = (uint8_t *)malloc((size_t)sz);
    if (b && fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); b = NULL; }
    fclose(f);
    if (b) *n = (size_t)sz;
    return b;
}

typedef struct { uint8_t *buf; size_t n; re15_rdt_t rdt; } raum_t;

static int raum_laden(raum_t *r, const char *name)
{
    char p[600];
    snprintf(p, sizeof p, "%s/STAGE1/ROOM%s.RDT", RE15_ASSET_PSX_DIR, name);
    r->buf = datei(p, &r->n);
    if (!r->buf) { printf("FAIL: RDT nicht lesbar: %s\n", p); return -1; }
    if (re15_rdt_parse(r->buf, r->n, &r->rdt) != 0) { printf("FAIL: parse %s\n", p); return -1; }
    return 0;
}

/* Gruppe+1 je Record in ROOM1150/1151 Cut 2 (Gruppenkoepfe @0x00670: 12/9/10/10/7/1/1/1/1/1/1). */
static int soll_gruppe1(int i)
{
    static const int n[11] = { 12, 9, 10, 10, 7, 1, 1, 1, 1, 1, 1 };
    int k = 0;
    for (int g = 0; g < 11; g++) { if (i < k + n[g]) return g + 1; k += n[g]; }
    return -1;
}

/* ------------------------------------------------------------------ A */
static void teil_a(raum_t *r)
{
    printf("== A: Aufbau + Opcode 0x45 ueber die echte VM (ROOM1150)\n");
    scd_vm_init();
    re15_mg_aufbauen(&r->rdt, 2, "pin");
    CHECK(re15_mg_zahl() == 54, "Cut 2: Zahl %d == 54 (Kopf 0B 00 36 00 @0x0066C)", re15_mg_zahl());
    int alle_an = 1, gruppen_ok = 1;
    for (int i = 0; i < 54; i++) {
        if (re15_mg_byte0(i) != 1) alle_an = 0;
        if (re15_mg_byte1(i) != soll_gruppe1(i)) gruppen_ok = 0;
    }
    CHECK(alle_an, "nach dem Aufbau Byte0 == 1 fuer alle 54 Records (@0x800393e0 ori 0x1)");
    CHECK(gruppen_ok, "Byte1 = Gruppe+1 in Bau-Reihenfolge (@0x800393e8 addiu v0,a3,1)");
    CHECK(re15_mg_byte1(48) == 6 && re15_mg_byte1(53) == 11, "Records 48..53 = Gruppen 6..11");

    static const uint8_t prog1[] = { 0x45, 0x05, 0x00, 0x45, 0x06, 0x02, 0x01, 0x00 };
    scd_thread_start(1, prog1);
    scd_vm_tick();
    CHECK(re15_mg_byte0(48) == 0 && !re15_mg_sichtbar(48), "45 05 00: Record 48 aus");
    CHECK(re15_mg_byte0(49) == 2 && !re15_mg_sichtbar(49),
          "45 06 02 im SELBEN Takt (Rueckgabe 1 @0x80042900, pc+3 @0x80042904): Record 49 "
          "Byte0 = 2 (ganzes Byte @0x800396e0) -> unsichtbar (Bit 0 @0x800395f0)");
    CHECK(re15_mg_byte0(47) == 1 && re15_mg_byte0(50) == 1 && re15_mg_sichtbar(47) &&
          re15_mg_sichtbar(50), "Nachbarn 47 und 50 unveraendert");

    static const uint8_t prog2[] = { 0x45, 0x05, 0x01, 0x01, 0x00 };
    scd_thread_start(1, prog2);
    scd_vm_tick();
    CHECK(re15_mg_byte0(48) == 1 && re15_mg_sichtbar(48), "45 05 01: Record 48 wieder an");

    re15_mg_aufbauen(&r->rdt, 2, "pin");
    CHECK(re15_mg_byte0(49) == 1 && re15_mg_sichtbar(49), "Neuaufbau: Record 49 wieder an");

    re15_mg_aufbauen(&r->rdt, 0, "pin");
    CHECK(re15_mg_zahl() == 2, "Cut 0: Zahl %d == 2", re15_mg_zahl());
    static const uint8_t prog3[] = { 0x45, 0x05, 0x00, 0x01, 0x00 };
    scd_thread_start(1, prog3);
    scd_vm_tick();
    CHECK(re15_mg_byte0(0) == 1 && re15_mg_byte0(1) == 1,
          "Cut 0: 0x45 Gruppe 6 trifft keinen der 2 Records (Byte1 == 1)");

    re15_mg_aufbauen(&r->rdt, 5, "pin");
    CHECK(re15_mg_zahl() == 0 && re15_mg_sichtbar(0),
          "NULL-Cut 5 (FF FF FF FF): Zahl 0 (@0x80039338), Index 0 sichtbar (Port-Konstruktion)");
    re15_mg_aufbauen(NULL, 0, "pin");
    CHECK(re15_mg_zahl() == 0, "ohne RDT: Zahl 0");
}

/* ------------------------------------------------------------------ B/C */
static void hochfahren(raum_t *r, uint16_t room_id)
{
    re15_game_state_init();
    re15_inv_init();
    re15_files_reset();
    re15_aot_init();
    scd_vm_init(); re15_actor_init();
    g_current_room_id = room_id;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100;
    pl->x = -22250; pl->y = 0; pl->z = -18500; pl->rot_y = 2048; pl->state = 1;
    g_scd.player_mode = 0;
    re15_msg_load_room_block(r->rdt.messages, r->rdt.messages_size);
    scd_register_room_events(&r->rdt);
    scd_room_reenter(&r->rdt, pl->x, pl->z, 0);
    re15_mg_aufbauen(&r->rdt, 0, "raum");              /* room_common.c Schritt 9 */
}

/* Ein Takt wie die Hauptschleife: Praesentations-Apply (Aufbau) VOR dem SCD-Takt. */
static void takt(raum_t *r)
{
    if (re15_cam_present_tick()) re15_mg_aufbauen(&r->rdt, (int)g_scd.cam_id, "cut");
    scd_vm_tick();
}

#define TAKTE 200

static void teil_b(raum_t *r, uint16_t room_id, int *wechsel, int *n_wechsel)
{
    printf("== B: Takt ROOM%04X Cut 2 (%d Takte)\n", room_id, TAKTE);
    hochfahren(r, room_id);
    g_scd.cam_id = 2;                                   /* Zonenwechsel nach Cut 2 */
    int vorher = -1, ok_sechs = 1, ok_rest = 1, erster_aufbau_an = -1;
    *n_wechsel = 0;
    for (int t = 0; t < TAKTE; t++) {
        takt(r);
        if (re15_mg_zahl() != 54) { printf("  Takt %d: Zahl %d\n", t, re15_mg_zahl()); ok_rest = 0; continue; }
        int s = re15_mg_sichtbar(48);
        for (int i = 49; i <= 53; i++) if (re15_mg_sichtbar(i) != s) ok_sechs = 0;
        for (int i = 0; i < 48; i++) if (!re15_mg_sichtbar(i)) ok_rest = 0;
        if (t == 0) erster_aufbau_an = s;
        if (vorher >= 0 && s != vorher && *n_wechsel < 32) wechsel[(*n_wechsel)++] = t;
        vorher = s;
    }
    CHECK(erster_aufbau_an == 1, "Takt 0 (Aufbau Cut 2): Buchstaben AN");
    CHECK(ok_sechs, "alle sechs Buchstaben-Records (48..53) schalten gemeinsam");
    CHECK(ok_rest, "Records 0..47 (Gruppen 1..5) bleiben immer an, Zahl bleibt 54");
    printf("  Wechsel bei Takt:");
    for (int k = 0; k < *n_wechsel; k++) printf(" %d", wechsel[k]);
    printf("\n");
    CHECK(*n_wechsel >= 8, "mindestens 8 Wechsel in %d Takten (gemessen %d)", TAKTE, *n_wechsel);
    int abstand_ok = 1;
    for (int k = 1; k < *n_wechsel; k++) if (wechsel[k] - wechsel[k - 1] != 20) abstand_ok = 0;
    CHECK(abstand_ok, "Abstand zwischen zwei Wechseln EXAKT 20 Takte (Sleep 20 @0x010C8/@0x010DE)");
    CHECK(*n_wechsel > 0 && wechsel[0] <= 40, "erstes AUS spaetestens 40 Takte nach dem Aufbau (%d)",
          *n_wechsel ? wechsel[0] : -1);
}

static void teil_c(raum_t *r, uint16_t room_id, const int *wechsel, int n_wechsel)
{
    printf("== C: Cut-Wechsel 2 -> 0 -> 2 in einer AUS-Phase (ROOM%04X)\n", room_id);
    if (n_wechsel < 6) { CHECK(0, "Teil B lieferte zu wenige Wechsel"); return; }
    /* AUS-Phasen beginnen bei wechsel[0], [2], [4] ... (erster Wechsel = an -> aus). */
    int t_aus = wechsel[2], t_an = wechsel[3], t_aus2 = wechsel[4];
    int x = t_aus + 5;                                  /* mitten in der AUS-Phase */
    hochfahren(r, room_id);
    g_scd.cam_id = 2;
    int fehler = 0, s_nach = -1;
    for (int t = 0; t < TAKTE; t++) {
        if (t == x) g_scd.cam_id = 0;                   /* Cut 2 verlassen ... */
        if (t == x + 1) g_scd.cam_id = 2;               /* ... und zurueck */
        takt(r);
        if (t < x + 1 || t >= t_aus2 + 1) continue;
        int s = re15_mg_sichtbar(48);
        if (t == x + 1) s_nach = s;
        int soll = (t < t_aus2) ? 1 : 0;
        if (s != soll) { if (fehler < 5) printf("  Takt %d: sichtbar %d, soll %d\n", t, s, soll); fehler++; }
    }
    CHECK(s_nach == 1, "Takt %d (Aufbau Cut 2 nach dem Rueckwechsel): Buchstaben sofort AN", x + 1);
    CHECK(fehler == 0, "AN von Takt %d bis %d (das `:= 1` bei Takt %d aendert nichts), AUS erst bei "
          "Takt %d (naechstes `:= 0` von sub05)", x + 1, t_aus2 - 1, t_an, t_aus2);
}

int main(void)
{
    raum_t r1150, r1151;
    if (raum_laden(&r1150, "1150") || raum_laden(&r1151, "1151")) return 1;
    teil_a(&r1150);
    int w[32], nw = 0;
    teil_b(&r1150, 0x1150, w, &nw);
    teil_c(&r1150, 0x1150, w, nw);
    int w1[32], nw1 = 0;
    teil_b(&r1151, 0x1151, w1, &nw1);
    teil_c(&r1151, 0x1151, w1, nw1);
    CHECK(nw == nw1 && (nw == 0 || w[0] == w1[0]), "ROOM1151 (Elza) im selben Takt wie ROOM1150");
    if (fails) { printf("\nunit_r34n_g_maskgrp: %d FEHLER\n", fails); return 1; }
    printf("\nunit_r34n_g_maskgrp: OK\n");
    return 0;
}
