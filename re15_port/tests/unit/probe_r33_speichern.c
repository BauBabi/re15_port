/*
 * probe_r33_speichern.c — Runde 33, Thema S: Speichern nur mit Memory Card (RE2-Farbband-Ablauf).
 * Dossier: analysis/befunde_runde33/speichern_memory_card.md
 *
 * RIEGEL (Engine-Ebene, echte RDT-Daten, echter Examine-Pfad):
 *   (A) Text-Bau an ALLEN 16 Speicherstellen: Anfang = die ausgelieferte RE1.5-Meldung bis zum
 *       1./2. Seitenwechsel (Bytes aus der RDT), danach RE2s Schlussseite mit "Memory Card"
 *       (RE2 @0x8009EFE6 / @0x8009F064, Name DEBUG.BIN @0x800C4BEA). Dekodiert == Sollsatz.
 *   (B) ROOM1150 OHNE Karte: Untersuchen des Telefons (AOT-Slot 3 -> sub06 -> Message_on(1))
 *       zeigt den Hinweis "If I had a Memory Card, I could save my progress...", keine Frage,
 *       und nach dem Wegdruecken KEIN Speicherbildschirm (RE2 @0x80051b68-b98).
 *   (C) Karte NUR in der Item-Kiste -> wie ohne Karte (RE2 FUN_800696CC liest nur das Inventar
 *       @0x800D4A3C, die Kiste @0x800D4A68 nicht).
 *   (D) MIT Karte, Antwort JA: Frage "Will you use the Memory Card?" + Ja/Nein, poll -> 1 genau
 *       einmal (RE2 @0x80051bc4-dc), die Karte liegt danach unveraendert im Inventar.
 *   (E) MIT Karte, Antwort NEIN: poll -> 0, kein Speicherbildschirm (RE2 @0x80051be8-f4).
 *   (F) Waehrend der Text steht, steht das Skript (Maske 0xffff0000 aus `2b 01 ff ff`): der
 *       Nahaufnahme-Cut 6 von sub06 bleibt bis zum Schliessen stehen.
 * Exit 0 = alle Sollwerte getroffen, sonst 1.
 */
#include "re15_rdt.h"
#include "re15_scd.h"
#include "re15_actor.h"
#include "re15_aot.h"
#include "re15_room.h"
#include "re15_inventory.h"
#include "re15_savepoint.h"
#include "re15_itembox.h"
#include "re15_msg.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef RE15_ASSET_PSX_DIR
#define RE15_ASSET_PSX_DIR "shared_assets/PSX"
#endif

void scd_register_current_rdt(const re15_rdt_t *rdt);
extern uint16_t g_scd_pad_edge;
extern uint16_t g_scd_pad_held;
extern uint8_t  g_aot_action_pressed;

static int s_fail = 0;
#define CHECK(c, ...) do { if (!(c)) { fprintf(stderr, "FAIL: " __VA_ARGS__); fputc('\n', stderr); s_fail = 1; } } while (0)

static uint8_t *slurp(const char *path, long *out_sz)
{
    FILE *f = fopen(path, "rb");
    if (!f) return NULL;
    fseek(f, 0, SEEK_END);
    long sz = ftell(f);
    fseek(f, 0, SEEK_SET);
    if (sz <= 0) { fclose(f); return NULL; }
    uint8_t *b = (uint8_t *)malloc((size_t)sz);
    if (b && fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); b = NULL; }
    fclose(f);
    if (b) *out_sz = sz;
    return b;
}

/* RDT-Nachricht id aus dem Nachrichtenblock (off[0] = n*2, Rumpf bis zum eigenen Ende). */
static const uint8_t *rdt_msg(const re15_rdt_t *r, int id, int *len)
{
    if (!r->messages || r->messages_size < 2) return NULL;
    int n = (r->messages[0] | (r->messages[1] << 8)) / 2;
    if (id >= n) return NULL;
    int o = r->messages[id * 2] | (r->messages[id * 2 + 1] << 8);
    int e = r->messages_size;
    if (id + 1 < n) { int o2 = r->messages[id * 2 + 2] | (r->messages[id * 2 + 3] << 8); if (o2 > o) e = o2; }
    *len = e - o;
    return r->messages + o;
}

/* Sollsaetze (re15_msg_decode_text: 02 xx ohne Zwischenraum, 08 -> ' ', Stopp an 0x03). */
static const char *k_frage_rest   = "Will you use the Memory Card?";
static const char *k_hinweis_rest = "If I had a Memory Card, I could save my progress...";

/* ---- (A) alle 16 Speicherstellen ---------------------------------------------------- */
static void teil_a(void)
{
    static const struct { const char *pfad; unsigned room; int msg; const char *satz1; } k[] = {
        { "STAGE1/ROOM1070.RDT", 0x1070, 0x14, "It's an phone." },   /* Tippfehler des Originals */
        { "STAGE1/ROOM1071.RDT", 0x1071, 0x14, "It's an phone." },
        { "STAGE1/ROOM1120.RDT", 0x1120, 0x06, "It's a phone." },
        { "STAGE1/ROOM1121.RDT", 0x1121, 0x06, "It's a phone." },
        { "STAGE1/ROOM1150.RDT", 0x1150, 0x01, "It's a phone." },
        { "STAGE1/ROOM1151.RDT", 0x1151, 0x01, "It's a phone." },
        { "STAGE2/ROOM2010.RDT", 0x2010, 0x03, "It's a computer." },
        { "STAGE2/ROOM2011.RDT", 0x2011, 0x03, "It's a computer." },
        { "STAGE3/ROOM30A0.RDT", 0x30A0, 0x01, "It's a computer." },
        { "STAGE3/ROOM30A1.RDT", 0x30A1, 0x01, "It's a computer." },
        { "STAGE3/ROOM30B0.RDT", 0x30B0, 0x01, "It's a computer." },
        { "STAGE3/ROOM30B1.RDT", 0x30B1, 0x01, "It's a computer." },
        { "STAGE4/ROOM4010.RDT", 0x4010, 0x2B, "It's a computer." },
        { "STAGE4/ROOM4011.RDT", 0x4011, 0x07, "It's a computer." },
        { "STAGE5/ROOM5010.RDT", 0x5010, 0x06, "It's a computer." },
        { "STAGE5/ROOM5011.RDT", 0x5011, 0x2D, "It's a computer." },
    };
    int ok = 0;
    for (unsigned i = 0; i < sizeof k / sizeof k[0]; i++) {
        char pfad[256]; snprintf(pfad, sizeof pfad, "%s/%s", RE15_ASSET_PSX_DIR, k[i].pfad);
        long sz = 0; uint8_t *buf = slurp(pfad, &sz);
        if (!buf) { CHECK(0, "(A) %s nicht lesbar", pfad); continue; }
        re15_rdt_t r;
        if (re15_rdt_parse(buf, (size_t)sz, &r) != 0) { CHECK(0, "(A) %s: RDT-Parse", pfad); free(buf); continue; }
        CHECK(re15_savepoint_is(k[i].room, (uint8_t)k[i].msg), "(A) %04X/%d ist keine Speicherstelle", k[i].room, k[i].msg);
        int olen = 0; const uint8_t *orig = rdt_msg(&r, k[i].msg, &olen);
        if (!orig) { CHECK(0, "(A) %04X: msg %d fehlt", k[i].room, k[i].msg); free(buf); continue; }
        for (int with = 0; with <= 1; with++) {
            uint8_t out[160]; char txt[200], soll[200];
            int n = re15_savepoint_build_text(orig, olen, with, out, sizeof out);
            /* Anfang byte-gleich zur RDT: bis 1. bzw. 2. `02 00` */
            int want = with ? 2 : 1, seen = 0, cut = 0;
            for (int j = 0; j + 1 < olen; ) {
                if (orig[j] == 0x02) { if (++seen == want) { cut = j + 2; break; } j += 2; continue; }
                if (orig[j] == 0x04 || orig[j] == 0x05) { j += 2; continue; }
                j++;
            }
            CHECK(cut > 0 && n > cut && memcmp(out, orig, (size_t)cut) == 0,
                  "(A) %04X with=%d: Anfang nicht byte-gleich zur RDT (cut=%d n=%d)", k[i].room, with, cut, n);
            re15_msg_decode_text(out, (size_t)n, txt, sizeof txt);
            if (with) snprintf(soll, sizeof soll, "%sYou can save your progress with this.%s", k[i].satz1, k_frage_rest);
            else      snprintf(soll, sizeof soll, "%s%s", k[i].satz1, k_hinweis_rest);
            CHECK(strcmp(txt, soll) == 0, "(A) %04X with=%d:\n   ist  \"%s\"\n   soll \"%s\"", k[i].room, with, txt, soll);
            /* Endform: mit Karte `1b 03 02 01 00` (Ja/Nein wie alle 90 RE1.5-Auswahltexte),
             * ohne Karte `57 57 57 01 00` ("..." + Ende, wartet auf Taste). */
            if (with) CHECK(n >= 5 && memcmp(out + n - 5, "\x1b\x03\x02\x01\x00", 5) == 0, "(A) %04X: Frage-Ende", k[i].room);
            else      CHECK(n >= 5 && memcmp(out + n - 5, "\x57\x57\x57\x01\x00", 5) == 0, "(A) %04X: Hinweis-Ende", k[i].room);
        }
        if (!s_fail) ok++;
        free(buf);
    }
    printf("  (A) 16 Speicherstellen: %d/16 Texte == Soll (RE1.5-Anfang aus der RDT + RE2-Schluss)\n", ok);
}

/* ---- ROOM1150 hochfahren --------------------------------------------------------------- */
static re15_rdt_t s_rdt;
static uint8_t   *s_buf;

static void raum_1150(void)
{
    re15_actor_init();
    re15_aot_init();
    scd_vm_init();
    re15_inv_init();
    re15_itembox_init();
    re15_savepoint_reset();
    re15_itembox_reset();
    re15_pauseflags_clear();
    memset(&g_room_change, 0, sizeof g_room_change);
    g_current_room_id = 0x1150;
    re15_msg_clear_room_block();
    re15_msg_load_room_block(s_rdt.messages, s_rdt.messages_size);
    scd_register_current_rdt(&s_rdt);
    scd_thread_start(0, s_rdt.main_scd);
    scd_thread_start(1, s_rdt.sub_scd[0]);
    for (int i = 0; i < 120; i++) scd_vm_tick();
}

/* Telefon untersuchen wie die Spielschleife: Aktionsflanke -> re15_aot_scan -> Event 6.
 * Slot 3 = `2c 03 03 31 .. 40 a2 c0 ae e8 03 e8 03 ff 00 18 06` @ROOM1150 0x0DA6:
 * Ecke (-24000,-20800), Ausdehnung 1000x1000 -> Mitte (-23500,-20300), Event 6. */
static int telefon(int *cut_waehrend)
{
    extern void re15_collision_set_band(int band);
    re15_collision_set_band(0);
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->x = -23500 - 620; pl->z = -20300; pl->rot_y = 0;
    g_aot.fired_event_id_this_frame = 0;
    g_aot_action_pressed = 1;
    re15_aot_scan(pl->x, pl->z, 0xFF);
    g_aot_action_pressed = 0;
    if (g_aot.fired_event_id_this_frame != 6) return -1;
    if (scd_event_fire(6) < 0) return -2;
    for (int i = 0; i < 12; i++) scd_vm_tick();          /* Cut_chg(6) + Message_on(1) */
    if (cut_waehrend) *cut_waehrend = g_scd.cam_id;
    return 0;
}

/* Den Text Bild fuer Bild treiben (Bestaetigung als Flanke) bis zur Ja/Nein-Auswahl oder
 * bis er zu ist. Liefert 1, wenn die Auswahl auf dem Schirm steht. */
static int bis_auswahl(void)
{
    for (int i = 0; i < 900 && g_scd.message_active; i++) {
        if (g_scd.message_select) return 1;
        g_scd_pad_held = 0x4000; g_scd_pad_edge = (i & 1) ? 0x4000 : 0;
        re15_msg_tick(0, 0, 0);
        (void)re15_savepoint_poll();
    }
    g_scd_pad_held = 0; g_scd_pad_edge = 0;
    return g_scd.message_select ? 1 : 0;
}

static int text_ist(const char *soll)
{
    int len = 0; char txt[200];
    const unsigned char *raw = re15_msg_get_raw(RE15_SAVEPOINT_MSG_ID, &len);
    if (!raw) return 0;
    re15_msg_decode_text(raw, (size_t)len, txt, sizeof txt);
    if (strcmp(txt, soll) != 0) { fprintf(stderr, "   Text ist \"%s\"\n   soll    \"%s\"\n", txt, soll); return 0; }
    return 1;
}

int main(void)
{
    printf("=== Runde 33 S: Speichern nur mit Memory Card (RE2-Farbband-Ablauf) ===\n");
    teil_a();

    long sz = 0;
    s_buf = slurp(RE15_ASSET_PSX_DIR "/STAGE1/ROOM1150.RDT", &sz);
    if (!s_buf || re15_rdt_parse(s_buf, (size_t)sz, &s_rdt) != 0 || !s_rdt.sub_scd[6]) {
        fprintf(stderr, "FAIL: ROOM1150.RDT\n"); return 1;
    }

    /* ---- (B) ohne Karte ---- */
    {
        raum_1150();
        int cut = -1, r = telefon(&cut);
        CHECK(r == 0, "(B) Telefon-Examine rc=%d", r);
        CHECK(g_scd.message_active && g_scd.message_id == RE15_SAVEPOINT_MSG_ID, "(B) kein Port-Text offen (id %d)", g_scd.message_id);
        CHECK(text_ist("It's a phone.If I had a Memory Card, I could save my progress..."), "(B) Hinweistext");
        CHECK(!re15_savepoint_asking(), "(B) ohne Karte darf keine Frage offen sein");
        CHECK(!re15_msg_is_choice(RE15_SAVEPOINT_MSG_ID), "(B) Hinweis darf keine Ja/Nein-Auswahl tragen");
        /* (F) Maske 0xffff0000 -> Skript steht: Cut 6 bleibt waehrend des Textes */
        CHECK(cut == 6, "(F) Nahaufnahme-Cut waehrend des Textes = %d, Soll 6 (sub06 Cut_chg(6))", cut);
        CHECK((g_re15_pauseflags & 0xffff0000u) == 0xffff0000u, "(F) Pausenmaske 0x%08x, Soll 0xffff0000", (unsigned)g_re15_pauseflags);
        int auswahl = bis_auswahl();
        CHECK(!auswahl, "(B) ohne Karte erschien eine Ja/Nein-Auswahl");
        int ja = 0;
        for (int i = 0; i < 5; i++) ja |= re15_savepoint_poll();
        CHECK(!g_scd.message_active, "(B) Hinweis nicht geschlossen");
        CHECK(!ja && !re15_savepoint_pending(), "(B) ohne Karte wurde der Speicherbildschirm verlangt");
        for (int i = 0; i < 8; i++) scd_vm_tick();
        CHECK(g_scd.cam_id != 6, "(F) nach dem Schliessen laeuft sub06 weiter (Cut_old), cam=%d", g_scd.cam_id);
        if (!s_fail) printf("  (B) ohne Karte: Hinweis, keine Frage, kein Speichern; (F) Cut 6 + Maske 0xffff0000 waehrend des Textes\n");
    }

    /* ---- (C) Karte nur in der Kiste ---- */
    {
        raum_1150();
        g_itembox.slots[0].id = RE15_SAVEPOINT_CARD_ITEM; g_itembox.slots[0].qty = 1;
        int r = telefon(NULL);
        CHECK(r == 0, "(C) Telefon-Examine rc=%d", r);
        CHECK(!re15_savepoint_asking(), "(C) Karte in der Kiste zaehlte als Besitz");
        CHECK(text_ist("It's a phone.If I had a Memory Card, I could save my progress..."), "(C) Hinweistext");
        bis_auswahl();
        if (!s_fail) printf("  (C) Karte nur in der Item-Kiste -> Hinweis (zaehlt nur das Inventar)\n");
    }

    /* ---- (D) mit Karte, JA ---- */
    {
        raum_1150();
        g_inv.slots[4].id = RE15_SAVEPOINT_CARD_ITEM; g_inv.slots[4].qty = 1;
        re15_inventory_t inv_vorher = g_inv;
        int r = telefon(NULL);
        CHECK(r == 0, "(D) Telefon-Examine rc=%d", r);
        CHECK(re15_savepoint_asking(), "(D) mit Karte keine Frage offen");
        CHECK(text_ist("It's a phone.You can save your progress with this.Will you use the Memory Card?"), "(D) Fragetext");
        CHECK(re15_msg_is_choice(RE15_SAVEPOINT_MSG_ID), "(D) Frage traegt keine Ja/Nein-Auswahl");
        int auswahl = bis_auswahl();
        CHECK(auswahl, "(D) Ja/Nein-Auswahl erschien nicht");
        CHECK(g_scd.message_choice == 0, "(D) Vorbelegung muss JA sein (RE2 @0x8002FE88), ist %d", g_scd.message_choice);
        int n_ja = 0;
        n_ja += re15_savepoint_poll();                         /* Auswahl steht, noch nichts */
        g_scd_pad_edge = 0x4000; re15_msg_tick(0, 0, 0); g_scd_pad_edge = 0;   /* JA bestaetigen */
        for (int i = 0; i < 5; i++) n_ja += re15_savepoint_poll();
        CHECK(n_ja == 1, "(D) poll meldete JA %d-mal, Soll genau 1", n_ja);
        CHECK(!re15_savepoint_asking(), "(D) Frage nach der Antwort noch offen");
        CHECK(memcmp(&inv_vorher, &g_inv, sizeof g_inv) == 0, "(D) Inventar veraendert (Karte muss bleiben)");
        CHECK(g_inv.slots[4].id == RE15_SAVEPOINT_CARD_ITEM && g_inv.slots[4].qty == 1, "(D) Memory Card weg");
        if (!s_fail) printf("  (D) mit Karte + JA: Frage, Ja vorbelegt, poll==1 genau einmal, Karte bleibt\n");
    }

    /* ---- (E) mit Karte, NEIN ---- */
    {
        raum_1150();
        g_inv.slots[4].id = RE15_SAVEPOINT_CARD_ITEM; g_inv.slots[4].qty = 1;
        int r = telefon(NULL);
        CHECK(r == 0, "(E) Telefon-Examine rc=%d", r);
        int auswahl = bis_auswahl();
        CHECK(auswahl, "(E) Ja/Nein-Auswahl erschien nicht");
        g_scd_pad_edge = 0x1000; re15_msg_tick(0, 0, 0); g_scd_pad_edge = 0;   /* auf NEIN */
        (void)re15_savepoint_poll();
        CHECK(g_scd.message_choice == 1, "(E) Cursor steht nicht auf NEIN");
        g_scd_pad_edge = 0x4000; re15_msg_tick(0, 0, 0); g_scd_pad_edge = 0;   /* bestaetigen */
        int ja = 0;
        for (int i = 0; i < 5; i++) ja |= re15_savepoint_poll();
        CHECK(!ja, "(E) NEIN fuehrte zum Speicherbildschirm");
        CHECK(!re15_savepoint_asking() && !g_scd.message_active, "(E) Frage/Text nach NEIN noch offen");
        CHECK(g_inv.slots[4].id == RE15_SAVEPOINT_CARD_ITEM, "(E) Memory Card weg");
        if (!s_fail) printf("  (E) mit Karte + NEIN: kein Speichern, zurueck ins Spiel\n");
    }

    free(s_buf);
    printf(s_fail ? "=== ROT ===\n" : "=== GRUEN ===\n");
    return s_fail ? 1 : 0;
}
