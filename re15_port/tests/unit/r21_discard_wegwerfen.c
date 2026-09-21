/* r21_discard_wegwerfen.c — RIEGEL fuer "You don't need this key any more. Discard it?"
 *
 * Nutzer-Auftrag 2026-09-21. Herleitung, Adressen und Sackgassen-Argument:
 * include/re15_item_discard.h.  Dieser Riegel prueft DREI Dinge, jedes mit
 * Abdeckungszahl UND Gegenprobe:
 *
 *  TEIL A — je betroffenem Gegenstand: die Abfrage kommt NACH dem Gebrauch und VORHER NICHT.
 *      Gefahren wird durch den ECHTEN Motorpfad: eine 6-Byte-SCD-Folge
 *      `2B <msg> FF FF / 01 00` (Message_on + Evt_end) laeuft im ECHTEN VM im ECHTEN Raum,
 *      also durch op_message_on und damit durch re15_discard_notice_message.
 *      GEGENPROBE je Fall: dieselbe Folge mit einer ANDEREN Nachrichten-Id desselben Raums
 *      darf NICHTS ausloesen. Ohne sie stuende der Riegel auch dann gruen, wenn die
 *      Abfrage bei JEDER Nachricht kaeme.
 *
 *  TEIL B — Ja entfernt genau diesen Gegenstand, Nein behaelt ihn MIT Anzahl 1.
 *      Die Anzahl ist der Punkt: RE2 zaehlt VOR der Frage herunter (@0x80051810) und
 *      schreibt im Nein-Zweig die 1 zurueck (@0x800517C4) — sonst liefe sie beim naechsten
 *      Gebrauch auf 0xFF unter und es wuerde nie wieder gefragt.
 *
 *  TEIL C — SACKGASSEN-RIEGEL: ein weggeworfener Gegenstand kann keinen Skript-Zweig
 *      veraendern. Vollzensus ueber ALLE RDTs mit dem Laengen-Vorschub des Motors selbst
 *      (scd_opcode_size_at):
 *        C1  Keep_Item_ck (0x5E) — der EINZIGE RE1.5-Opcode, dessen Handler den
 *            Inventar-Zeiger anfasst (LAB_80042b04 -> FUN_80013278, liest 0x800ac99c) —
 *            muss 0 MAL vorkommen.
 *        C2  KEIN Set (0x22) auf Flag-Zone 9 mit op 0 (loeschen) oder 7 (umschalten):
 *            das "genommen"-Bit, an dem die Schluesseltore haengen, wird nie geloescht.
 *        C3  Jeder aufgenommene Gegenstand hat mindestens EIN Item_aot_set mit Anzahl 1.
 *      GEGENPROBE zu C1/C2 (sonst waere ein Walker, der sofort stehenbleibt, gruen):
 *        C4  der Walk MUSS die drei bekannten Kartenleser-Tore wirklich sehen —
 *            Ck(9,52) in ROOM10D0, Ck(9,136) in ROOM1230, Ck(9,138) in ROOM11E0 —
 *            und die 164 ausgelieferten Item_aot_set-Records zaehlen.
 *      C5  Und der Beweis am lebenden Objekt: Gegenstand ins Inventar, "genommen"-Bit
 *          setzen (wie der Aufnahme-Modal, item_modal_common.c:289), Gegenstand
 *          wegwerfen — das Tor-Bit steht danach immer noch.
 */
#include "re15_rdt.h"
#include "re15_scd.h"
#include "re15_actor.h"
#include "re15_aot.h"
#include "re15_room.h"
#include "re15_inventory.h"
#include "re15_msg.h"
#include "re15_item_discard.h"
#include "re15_item_prompt.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef RE15_ASSET_PSX_DIR
#define RE15_ASSET_PSX_DIR "shared_assets/PSX"
#endif

extern scd_vm_t g_scd;

/* Die aufgenommenen Benutzungsstellen — DIESELBE erzeugte Tabelle, die der Motor benutzt.
 * (Der Riegel prueft damit genau das ausgelieferte Verhalten, nicht eine Zweitfassung.) */
#include "gen/discard_sites.inc"

static int g_fehler = 0;
#define PRUEFE(bed, ...) do { if (!(bed)) { printf("  FEHLER: "); printf(__VA_ARGS__); \
                              printf("\n"); g_fehler++; } } while (0)

static uint8_t *slurp(const char *p, size_t *n)
{
    FILE *f = fopen(p, "rb"); if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    if (sz <= 0) { fclose(f); return NULL; }
    uint8_t *b = (uint8_t *)malloc((size_t)sz);
    if (b && fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); b = NULL; }
    fclose(f); if (b) *n = (size_t)sz; return b;
}

static const char *stage_dir(unsigned room) { return
    (room >> 12) == 1 ? "STAGE1" : (room >> 12) == 2 ? "STAGE2" :
    (room >> 12) == 3 ? "STAGE3" : (room >> 12) == 4 ? "STAGE4" :
    (room >> 12) == 5 ? "STAGE5" : "STAGE6"; }

static uint8_t *raum_laden(unsigned room, re15_rdt_t *rdt, size_t *n)
{
    char p[600];
    snprintf(p, sizeof p, "%s/%s/ROOM%04X.RDT", RE15_ASSET_PSX_DIR, stage_dir(room), room);
    uint8_t *raw = slurp(p, n);
    if (!raw) return NULL;
    if (re15_rdt_parse(raw, *n, rdt) < 0) { free(raw); return NULL; }
    return raw;
}

/* =========================================================================
 * TEIL A + B — der Mechanismus, durch den echten VM
 * ========================================================================= */

/* 6 Byte: Message_on <id> (Maske 0xFFFF wie die Raeume selbst) + Evt_end. */
static uint8_t s_prog[6] = { 0x2B, 0x00, 0xFF, 0xFF, 0x01, 0x00 };

/* Eine Nachricht des Raums im ECHTEN VM abspielen und wie ein Spieler wegdruecken.
 * Der Dialog liest seine Knoepfe aus g_scd_pad_edge/held (msg_common.c:439-471,
 * virtuelles Bit 0x4000 = BESTAETIGEN, roh SQUARE) — genau die werden hier gepulst.
 * Die Abfrage-FSM bekommt davon NICHTS ab (sie wird mit 0 getickt): so ist belegt, dass
 * sie nicht etwa vom Wegdruecken der Nachricht mitbestaetigt wird. */
static void nachricht_spielen(re15_rdt_t *rdt, unsigned room, uint8_t msg_id)
{
    extern uint16_t g_scd_pad_edge, g_scd_pad_held;
    g_current_room_id = room;
    re15_msg_load_room_block(rdt->messages, rdt->messages_size);
    s_prog[1] = msg_id;
    scd_thread_start(0, s_prog);
    for (int f = 0; f < 4000; f++) {
        const unsigned char *raw; int len, id;
        g_scd_pad_edge = (f > 2 && (f % 4) == 0) ? 0x4000u : 0u;   /* Seite/Ende wegdruecken */
        g_scd_pad_held = 0;
        scd_vm_tick();
        re15_msg_tick(&raw, &len, &id);
        g_scd_pad_edge = 0;
        if (re15_discard_active()) re15_discard_tick(0, 0);
        if (!g_scd.message_active && !g_scd.message_fsm_active) break;
    }
    /* Danach der Warte->Frage-Uebergang (D_WARTET braucht einen freien Takt). */
    for (int f = 0; f < 8 && re15_discard_active() && !re15_discard_prompt(NULL, NULL); f++)
        re15_discard_tick(0, 0);
}

/* Den Prompt-Text austippen lassen und dann `edge` einmal anlegen. */
static void antworten(uint16_t edge)
{
    for (int f = 0; f < 4000 && !re15_discard_ready(); f++) re15_discard_tick(0, 0);
    PRUEFE(re15_discard_ready(), "Prompt wurde nicht fertig getippt");
    re15_discard_tick(edge, 0);
}

static void grundzustand(void)
{
    scd_vm_init();
    re15_actor_init();
    re15_aot_init();
    re15_inv_init();
    re15_discard_reset();
    memset(&g_scd, 0, sizeof g_scd);
    g_scd.work_slot = -1;
}

static int teil_a_b(void)
{
    printf("\n=== TEIL A+B: je Gegenstand — Abfrage nach dem Gebrauch, vorher nicht ===\n");
    int faelle = 0, mit_gegenprobe = 0;
    uint8_t gesehen[256]; memset(gesehen, 0, sizeof gesehen);

    for (int i = 0; i < RE15_DISCARD_SITE_COUNT; i++) {
        unsigned room = re15_discard_sites[i].room;
        uint8_t  msg  = re15_discard_sites[i].msg;
        uint8_t  item = re15_discard_sites[i].item;
        re15_rdt_t rdt; size_t n = 0;
        uint8_t *raw = raum_laden(room, &rdt, &n);
        if (!raw) { printf("  SKIP ROOM%04X (RDT fehlt)\n", room); continue; }
        gesehen[item] = 1;

        /* --- GEGENPROBE: irgendeine ANDERE Nachricht desselben Raums loest nichts aus. --- */
        uint8_t andere = (uint8_t)(msg == 0 ? 1 : 0);
        int andere_ok = 0;
        for (int k = 0; k < 24; k++) {
            uint8_t cand = (uint8_t)k;
            if (cand == msg) continue;
            int belegt = 0;
            for (int j = 0; j < RE15_DISCARD_SITE_COUNT; j++)
                if (re15_discard_sites[j].room == room && re15_discard_sites[j].msg == cand)
                    belegt = 1;
            if (belegt) continue;
            andere = cand; andere_ok = 1; break;
        }
        grundzustand();
        PRUEFE(re15_inv_grant(item, 1) == 0, "ROOM%04X: Gegenstand 0x%02X nicht gewaehrt", room, item);
        if (andere_ok) {
            nachricht_spielen(&rdt, room, andere);
            PRUEFE(!re15_discard_active(),
                   "ROOM%04X: Nachricht %u (NICHT die Benutzung) hat die Abfrage ausgeloest",
                   room, (unsigned)andere);
            PRUEFE(re15_inv_find_item(item) >= 0 &&
                   g_inv.slots[re15_inv_find_item(item)].qty == 1,
                   "ROOM%04X: Anzahl nach der Gegenprobe-Nachricht nicht mehr 1", room);
            mit_gegenprobe++;
        }

        /* --- FALL 1: Gebrauch -> Abfrage -> JA -> Gegenstand weg -------------------- */
        grundzustand();
        re15_inv_grant(item, 1);
        int vor = re15_discard_gefragt();
        nachricht_spielen(&rdt, room, msg);
        uint8_t pitem = 0; int pchoice = -1;
        int prompt = re15_discard_prompt(&pitem, &pchoice);
        PRUEFE(prompt == 8, "ROOM%04X msg %u: kein Prompt (Skript-Schluessel %d, erwartet 8)",
               room, (unsigned)msg, prompt);
        PRUEFE(pitem == item, "ROOM%04X: Prompt zeigt 0x%02X statt 0x%02X", room, pitem, item);
        PRUEFE(pchoice == 0, "ROOM%04X: Yes ist nicht vorbelegt (choice=%d)", room, pchoice);
        PRUEFE(re15_discard_gefragt() == vor + 1, "ROOM%04X: Frage-Zaehler stimmt nicht", room);
        /* Der Text ist das RE1.5-Skript [6] — gleiche Glyphenzahl wie der Walker meldet. */
        PRUEFE(re15_discard_reveal_total() == re15_item_prompt_walk(8, item, 0, 0, 0)
               && re15_discard_reveal_total() > 30,
               "ROOM%04X: Prompt-Textlaenge %d unplausibel", room, re15_discard_reveal_total());
        antworten(0x4000);                       /* SQUARE = bestaetigen, Auswahl Yes */
        PRUEFE(re15_inv_find_item(item) < 0,
               "ROOM%04X: 0x%02X liegt nach JA immer noch im Inventar", room, item);
        PRUEFE(!re15_discard_active(), "ROOM%04X: Abfrage nach JA nicht beendet", room);

        /* --- FALL 2: Gebrauch -> Abfrage -> NEIN -> Gegenstand bleibt, Anzahl 1 ----- */
        grundzustand();
        re15_inv_grant(item, 1);
        nachricht_spielen(&rdt, room, msg);
        PRUEFE(re15_discard_prompt(NULL, NULL) == 8, "ROOM%04X: kein Prompt im NEIN-Fall", room);
        for (int f = 0; f < 4000 && !re15_discard_ready(); f++) re15_discard_tick(0, 0);
        re15_discard_tick(0x1000, 0);            /* Menue-rechts: Auswahl auf No        */
        { uint8_t it2; int ch2 = -1; re15_discard_prompt(&it2, &ch2);
          PRUEFE(ch2 == 1, "ROOM%04X: Umschalten auf No wirkte nicht (choice=%d)", room, ch2); }
        re15_discard_tick(0x4000, 0);            /* bestaetigen                          */
        int slot = re15_inv_find_item(item);
        PRUEFE(slot >= 0, "ROOM%04X: 0x%02X nach NEIN verschwunden", room, item);
        PRUEFE(slot >= 0 && g_inv.slots[slot].qty == 1,
               "ROOM%04X: Anzahl nach NEIN ist %d statt 1 (@0x800517C4)",
               room, slot >= 0 ? g_inv.slots[slot].qty : -1);
        PRUEFE(!re15_discard_active(), "ROOM%04X: Abfrage nach NEIN nicht beendet", room);

        /* --- FALL 3: der Gegenstand ist GAR NICHT da -> keine Abfrage ---------------- */
        grundzustand();
        nachricht_spielen(&rdt, room, msg);
        PRUEFE(!re15_discard_active(),
               "ROOM%04X: Abfrage obwohl der Gegenstand nicht getragen wird", room);

        faelle++;
        free(raw);
    }
    int items = 0;
    for (int i = 0; i < 256; i++) if (gesehen[i]) items++;
    printf("  ABDECKUNG: %d von %d Benutzungsstellen gefahren, %d verschiedene Gegenstaende,\n"
           "             je 3 Faelle (Ja / Nein / nicht getragen) + %d Gegenproben\n",
           faelle, RE15_DISCARD_SITE_COUNT, items, mit_gegenprobe);
    PRUEFE(faelle == RE15_DISCARD_SITE_COUNT, "nicht alle Benutzungsstellen gefahren");
    PRUEFE(items == 9, "erwartet 9 Gegenstaende, gezaehlt %d", items);
    PRUEFE(mit_gegenprobe == faelle, "nicht jeder Fall hatte eine Gegenprobe");
    return faelle;
}

/* =========================================================================
 * TEIL C — Sackgassen-Riegel: Vollzensus ueber alle RDTs
 * ========================================================================= */

typedef struct {
    long rdts, stummel, opcodes, desync;
    long keep_item_ck;          /* C1 */
    long zone9_clear;           /* C2 */
    long item_aot_set;          /* C4 */
    long ck_10d0_52, ck_1230_136, ck_11e0_138;   /* C4 */
    uint8_t menge1[256];        /* C3 */
} zensus_t;

static uint16_t le16(const uint8_t *p) { return (uint16_t)(p[0] | (p[1] << 8)); }
static uint32_t le32(const uint8_t *p) { return (uint32_t)p[0] | ((uint32_t)p[1] << 8)
                                              | ((uint32_t)p[2] << 16) | ((uint32_t)p[3] << 24); }

/* Sektionsende wie tools/discard_zensus.py (= aus rdt_common.c abgeleitet): Untergrenze ist
 * die Ausdehnung der eigenen Offsettabelle, Obergrenze der naechste Sektionszeiger. */
static uint32_t sektion_ende(const uint8_t *d, size_t n, uint32_t s)
{
    uint32_t boden = s;
    if (s && s + 2 <= n) {
        uint16_t first = le16(d + s);
        if (first >= 2 && !(first & 1) && s + first <= n) {
            uint32_t mx = 0;
            for (uint32_t i = 0; i < first / 2u; i++) {
                uint16_t o = le16(d + s + 2 * i);
                if (!o || s + o >= n) continue;
                if (o > mx) mx = o;
            }
            boden = s + mx;
        }
    }
    uint32_t best = (uint32_t)n;
    for (uint32_t o = 0x40; o < 0x60; o += 4) {
        if (o + 4 > n) break;
        uint32_t v = le32(d + o);
        if (v > boden && v <= n && v < best) best = v;
    }
    return best;
}

static void walk(zensus_t *z, const uint8_t *d, size_t n, uint32_t start, uint32_t end,
                 unsigned room)
{
    uint32_t pc = start, maxf = start;
    while (pc < end) {
        int sz = scd_opcode_size_at(d + pc);
        if (sz <= 0) { z->desync++; return; }
        if (pc + (uint32_t)sz > end) return;
        z->opcodes++;
        uint8_t op = d[pc];
        if (op == 0x5E) z->keep_item_ck++;                        /* C1 */
        else if (op == 0x22 && d[pc + 1] == 9 && (d[pc + 3] == 0 || d[pc + 3] == 7))
            z->zone9_clear++;                                     /* C2 */
        else if (op == 0x50) {                                    /* C3 + C4 */
            int lf = (d[pc + 3] & 0x80) != 0;
            uint8_t t = lf ? d[pc + 22] : d[pc + 14];
            uint8_t a = lf ? d[pc + 24] : d[pc + 16];
            z->item_aot_set++;
            if (a == 1) z->menge1[t] = 1;
        } else if (op == 0x21 && d[pc + 1] == 9) {                /* C4 */
            if (room == 0x10D0 && d[pc + 2] == 52)  z->ck_10d0_52++;
            if (room == 0x1230 && d[pc + 2] == 136) z->ck_1230_136++;
            if (room == 0x11E0 && d[pc + 2] == 138) z->ck_11e0_138++;
        }
        /* Vorwaerts-Sprungziele merken: ein Evt_end VOR dem weitesten Ziel beendet die
         * Region nicht (gleiche Regel wie tools/scd_dump_room.py). */
        uint32_t t = 0;
        if (op == 0x06) t = pc + 4 + le16(d + pc + 2);
        else if (op == 0x14) t = pc + 6 + le16(d + pc + 2);
        if (t > maxf) maxf = t;
        if (op == 0x01 && pc + 2 > maxf) return;
        pc += (uint32_t)sz;
    }
}

static void zensus_raum(zensus_t *z, const char *pfad, unsigned room)
{
    size_t n = 0; uint8_t *d = slurp(pfad, &n);
    if (!d) return;
    if (n < 0x48) { z->stummel++; free(d); return; }
    z->rdts++;
    uint32_t sec[2] = { le32(d + 0x40), le32(d + 0x44) };
    for (int k = 0; k < 2; k++) {
        uint32_t s = sec[k];
        if (!s || s >= n || s + 2 > n) continue;
        uint32_t se = sektion_ende(d, n, s);
        uint16_t first = le16(d + s);
        if (first < 2 || (first & 1) || s + first > n) continue;
        uint32_t offs[512]; int cnt = 0;
        for (uint32_t i = 0; i < first / 2u && cnt < 512; i++) {
            uint16_t o = le16(d + s + 2 * i);
            if (!o || s + o >= n) continue;
            int dup = 0; for (int j = 0; j < cnt; j++) if (offs[j] == o) dup = 1;
            if (!dup) offs[cnt++] = o;
        }
        for (int i = 0; i < cnt; i++)                      /* aufsteigend sortieren */
            for (int j = i + 1; j < cnt; j++)
                if (offs[j] < offs[i]) { uint32_t t = offs[i]; offs[i] = offs[j]; offs[j] = t; }
        for (int i = 0; i < cnt; i++) {
            uint32_t a = s + offs[i];
            uint32_t b = (i + 1 < cnt) ? s + offs[i + 1] : se;
            if (b > se) b = se;
            if (b > a) walk(z, d, n, a, b, room);
        }
    }
    free(d);
}

static void teil_c(void)
{
    printf("\n=== TEIL C: Sackgassen-Riegel (Vollzensus ueber alle RDTs) ===\n");
    zensus_t z; memset(&z, 0, sizeof z);
    static const char *stages[6] = { "STAGE1","STAGE2","STAGE3","STAGE4","STAGE5","STAGE6" };
    for (int s = 0; s < 6; s++) {
        /* Raumnummern gehen bis ROOM##F# (z.B. ROOM1230) — die mittleren zwei Nibbles
         * muessen deshalb voll durchlaufen werden, nicht nur 0x00..0x1F. */
        for (unsigned hi = 0; hi < 0x100; hi++) {
            for (unsigned lo = 0; lo < 2; lo++) {
                unsigned room = (unsigned)((s + 1) << 12) | (hi << 4) | lo;
                char p[600];
                snprintf(p, sizeof p, "%s/%s/ROOM%04X.RDT", RE15_ASSET_PSX_DIR, stages[s], room);
                zensus_raum(&z, p, room);
            }
        }
    }
    printf("  ABDECKUNG: %ld RDTs mit Header (+%ld Stummel), %ld Opcodes, %ld Desync-Stopps\n",
           z.rdts, z.stummel, z.opcodes, z.desync);
    printf("  C1 Keep_Item_ck (0x5E, einziger Inventar-Leser): %ld\n", z.keep_item_ck);
    printf("  C2 Set auf Zone 9 mit loeschen/umschalten:       %ld\n", z.zone9_clear);
    printf("  C4 Item_aot_set-Records: %ld | Kartenleser-Tore 10D0/1230/11E0: %ld/%ld/%ld\n",
           z.item_aot_set, z.ck_10d0_52, z.ck_1230_136, z.ck_11e0_138);

    PRUEFE(z.rdts == 206, "erwartet 206 RDTs mit Header, gezaehlt %ld", z.rdts);
    PRUEFE(z.desync == 0, "%ld Desync-Stopps — der Zensus ist nicht belastbar", z.desync);
    PRUEFE(z.keep_item_ck == 0,
           "C1: %ld x Keep_Item_ck — ein Skript koennte das Inventar lesen", z.keep_item_ck);
    PRUEFE(z.zone9_clear == 0,
           "C2: %ld x Zone-9-Bit geloescht — ein Tor koennte sich wieder schliessen",
           z.zone9_clear);
    /* GEGENPROBE: der Walk hat wirklich etwas gesehen. Ein Walker, der sofort stehenbliebe,
     * meldete C1/C2 ebenfalls 0 — deshalb muessen die drei bekannten Tore und die
     * unabhaengig gemessene 164 (scd_vm.c:3862-3869) auftauchen. */
    PRUEFE(z.item_aot_set == 164, "C4: %ld Item_aot_set statt 164", z.item_aot_set);
    PRUEFE(z.ck_10d0_52 == 1,  "C4: Blaue-Karte-Tor Ck(9,52) in ROOM10D0 nicht gesehen");
    PRUEFE(z.ck_1230_136 == 1, "C4: Rote-Karte-Tor Ck(9,136) in ROOM1230 nicht gesehen");
    PRUEFE(z.ck_11e0_138 == 1, "C4: Gelbe-Karte-Tor Ck(9,138) in ROOM11E0 nicht gesehen");
    /* C3: jeder aufgenommene Gegenstand wird mit Anzahl 1 ausgegeben. */
    for (int i = 0; i < RE15_DISCARD_SITE_COUNT; i++)
        PRUEFE(z.menge1[re15_discard_sites[i].item],
               "C3: 0x%02X hat kein Item_aot_set mit Anzahl 1",
               re15_discard_sites[i].item);

    /* C5 — am lebenden Objekt: wegwerfen laesst das Tor-Bit stehen. */
    struct { uint8_t item, tk_bit; unsigned tor_raum; } k[3] = {
        { 0x38, 52,  0x10D0 },   /* Blaue Keycard,  Tor ROOM10D0 sub20 @0x19C0 */
        { 0x37, 136, 0x1230 },   /* Rote Keycard,   Tor ROOM1230 sub20 @0x1488 */
        { 0x39, 138, 0x11E0 },   /* Gelbe Keycard,  Tor ROOM11E0 sub20 @0x1FA8 */
    };
    for (int i = 0; i < 3; i++) {
        grundzustand();
        re15_game_flag_set(9, k[i].tk_bit, 0);
        re15_inv_grant(k[i].item, 1);
        re15_game_flag_set(9, k[i].tk_bit, 1);           /* item_modal_common.c:289 */
        int slot = re15_inv_find_item(k[i].item);
        PRUEFE(slot >= 0, "C5: 0x%02X nicht im Inventar", k[i].item);
        if (slot >= 0) re15_inv_remove_slot(slot);
        PRUEFE(re15_inv_find_item(k[i].item) < 0, "C5: 0x%02X nicht entfernt", k[i].item);
        PRUEFE(re15_game_flag_get(9, k[i].tk_bit) == 1,
               "C5: ROOM%04X-Tor (Zone 9 Bit %u) ist nach dem Wegwerfen ZU — SACKGASSE",
               k[i].tor_raum, (unsigned)k[i].tk_bit);
    }
    printf("  C5: 3 von 3 Kartenleser-Toren bleiben nach dem Wegwerfen offen\n");
}

int main(void)
{
    printf("=== r21_discard_wegwerfen — \"You don't need this key any more. Discard it?\"\n");
    printf("    Wortlaut: DEBUG.BIN @0x800C508B, Prompt-Skript [6] der Tabelle @0x800C4FC6\n");
    printf("    Regel:    RE2 LAB_80051718 (Zaehler @0x80051810, Frage @0x80051834)\n");

    /* Der Prompt-Text muss ueberhaupt da sein, sonst misst Teil A nur Nullen. */
    int glyphen = re15_item_prompt_walk(8, 0x38, 0, 0, 0);
    printf("    Prompt-Skript [6] + Name \"Blue Keycard\" = %d Glyphen\n", glyphen);
    PRUEFE(glyphen > 30, "Prompt-Skript [6] liefert nur %d Glyphen", glyphen);

    if (teil_a_b() == 0) {
        printf("SKIP: keine RDTs gefunden (%s)\n", RE15_ASSET_PSX_DIR);
        return 77;
    }
    teil_c();

    if (g_fehler) { printf("\nFEHLGESCHLAGEN: %d Pruefungen\n", g_fehler); return 1; }
    printf("\nOK — Abfrage, Ja/Nein und der Sackgassen-Riegel halten.\n");
    return 0;
}
