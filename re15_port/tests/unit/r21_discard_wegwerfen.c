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
 *  TEIL I — WORAUS DIE PORT-WARTEZEIT BESTAND, und dass sie es nicht mehr tut:
 *      je Benutzungsstelle wird die Spanne "vorgemerkt" in ihre Anteile zerlegt
 *      (Nachricht / Szene / Faden-Ende) und gegen die Spanne "die Nachricht haelt
 *      den Spieler" gestellt. Verlangt wird GLEICHHEIT — RE2s Regel (@0x800517f0
 *      `andi v0,v0,0x80` / @0x800517f4). Der frueher gemessene UEBERHANG wird im
 *      selben Lauf mit protokolliert; waere er 0, wuerde der Riegel nichts messen.
 *
 *  TEIL J — WAS DER SPIELER SPUERT: das px/pz-Delta hinter dem ECHTEN
 *      re15_game_step, nicht ein Praedikat gegen sich selbst. Drei Spalten im
 *      selben Lauf: JETZT (muss 0 sein), GEGENPROBE ueber die Ueberhang-Spanne des
 *      Runde-22-Modells (muss > 0 sein) und STUB ohne re15_game_step (muss 0
 *      sein — so ist belegt, dass eine 0 nicht schon aus dem Messaufbau folgt).
 *      Je Stelle zusaetzlich: die Abfrage geht wirklich auf, und nach der Antwort
 *      laeuft der Spieler wieder.
 *
 *  TEIL K — der SICHTBARE Prompt friert das Skript ein (byte-true @0x80051844/
 *      @0x80051850 mit 0xFF000000, SCD-Bit 0x02000000) und es laeuft nach der
 *      Antwort WEITER. ROOM1090 sub03 ist der Fall, an dem Runde 21 den
 *      Gegenbeweis zu sehen glaubte — dort wurde die Frage nie beantwortet.
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
 *      C6  die Schreiber von flag(3,32) (ROOM4000s Tor) per Set (0x22): 1 / 0.
 *      C7  und dieselbe Frage fuer die ANDEREN zwei Flag-Opcodes, damit C2/C6 nicht
 *          nur ueber Set gemessen sind: 0x59 (Flag-Set2, der zweite Schreiber) und
 *          0x58 (Flag-Ck2, nur Leser).
 *
 *  TEIL O — DIE FAIL-CLOSED-KOPPLUNG, die Hauptbehauptung der Runde 24: ein
 *      Direktaufruf von re15_discard_notice_message OHNE vorher gefallenen
 *      Vorentscheid darf NICHTS vormerken (RE2s Nicht-Treffer-Zweig haengt nichts ein,
 *      @0x800516C0). Mit POSITIVFALL, sonst waere der Riegel durch Nichtstun gruen.
 *
 *  TEIL P — re15_discard_restore: ein Laden belebt NIE eine Abfrage, und es laesst
 *      keinen Platz mit Anzahl 0 zurueck. Mit Gegenprobe (ohne restore steht sie).
 *
 *  TEIL Q — der JA-Zweig, wenn der gemerkte Platz den Gegenstand nicht mehr traegt:
 *      die Anzahl muss zurueck (@0x800517C4), sonst bleibt ein Platz mit Anzahl 0.
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
#include "re15_msg_select.h"
#include "re15_game_step.h"     /* der ECHTE Spielschritt — TEIL J misst px/pz dahinter */
#include "re15_player.h"        /* RE15_PAD_BIT_UP                                     */
#include "re15_collision.h"     /* Boden-Band aus dem Spawn-Y                          */
#include "re15_enemy_ai.h"      /* re15_player_cmd_reset                               */
#include "re15_camera.h"
#include "re15_room_spawns.h"   /* der Tuer-Spawn je Raum (begehbarer Punkt)           */
#include "re15_room_list.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef RE15_ASSET_PSX_DIR
#define RE15_ASSET_PSX_DIR "shared_assets/PSX"
#endif

extern scd_vm_t g_scd;
extern uint32_t g_re15_pauseflags;
void scd_register_current_rdt(const re15_rdt_t *rdt);

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
    re15_game_state_init();       /* u.a. flag(1,27)/flag(2,7) = "keine Szene laeuft" */
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
    long set332_setzt, set332_loescht;           /* C6: die Schreiber von flag(3,32) */
    long op59, op59_bank3, op59_bank9, op58;     /* C7: die ANDEREN Flag-Opcodes */
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
        /* C7 — DIE ANDEREN ZWEI FLAG-OPCODES. C2/C6 zaehlen nur Set (0x22); RE1.5 hat
         * aber noch 0x59 (Flag-Set2, der indizierte SCHREIBER, LAB_8003fe90 ->
         * @0x8003ff3c or / @0x8003ff24 nor+and / @0x8003ff50 xor) und 0x58 (Flag-Ck2,
         * das LESENDE Praedikat, LAB_8003fd54 -> nur `and`, kein Store). Ohne sie waere
         * "flag(3,32) hat genau einen Schreiber" nur ueber EINEN Opcode gemessen.
         * Die BANK von 0x59 steht statisch in pc[1] (`lw` ueber die Zeigertabelle
         * @0x80074664, pc[1]*4 @0x8003fed0); der INDEX kommt erst zur Laufzeit aus
         * work_vars[pc[2]] (`lhu` @0x8003feb8, DAT_800b0fd0) und ist statisch NICHT
         * bestimmbar. Darum wird die Bank gezaehlt: trifft kein einziges 0x59 die
         * Bank 3 bzw. 9, kann auch kein Index darin etwas schreiben. */
        if (op == 0x59) {
            z->op59++;
            if (d[pc + 1] == 3) z->op59_bank3++;
            if (d[pc + 1] == 9) z->op59_bank9++;
        }
        if (op == 0x58) z->op58++;
        if (op == 0x22 && d[pc + 1] == 3 && d[pc + 2] == 32) {    /* C6 */
            if (d[pc + 3] == 1)                     z->set332_setzt++;
            else if (d[pc + 3] == 0 || d[pc + 3] == 7) z->set332_loescht++;
        }
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
    /* C6 — DIE EINE STELLE, DIE EIN TOR ENDGUELTIG SCHLIESST.
     * ROOM4000 sub02 setzt mit `Set(3,32,1)` @0x01450 das Tor `Ck(3,32,0)` @0x0142A,
     * durch das seine Benutzungsstelle (Message_on 2) allein erreichbar ist. Gemessen
     * wird hier, dass dieses Flag im GANZEN ausgelieferten Bestand genau EINEN Schreiber
     * hat und NIE geloescht oder umgeschaltet wird — die Stelle ist also einmalig, und
     * zwar endgueltig. Das ist keine Port-Eigenschaft, sondern die der Daten; ausgewiesen
     * wird sie, damit niemand sie fuer einen Port-Defekt haelt.
     * Vollstaendige Liste aller 16 Stellen: tools/discard_verlustwege.py. */
    printf("  C6 Schreiber von flag(3,32) per Set (0x22): %ld x setzen,"
           " %ld x loeschen/umschalten\n", z.set332_setzt, z.set332_loescht);
    PRUEFE(z.set332_setzt == 1 && z.set332_loescht == 0,
           "flag(3,32): %ld Setzer / %ld Loescher — erwartet 1 / 0",
           z.set332_setzt, z.set332_loescht);

    /* C7 — UND DIE ANDEREN ZWEI FLAG-OPCODES, damit "genau EIN Schreiber" nicht nur
     * fuer Set (0x22) gilt. Gemessen ueber denselben Vollzensus:
     *   0x59 (Flag-Set2, der zweite SCHREIBER): 2 Vorkommen im ganzen Bestand, BEIDE
     *        auf Bank 5 — 0 auf Bank 3 (C6) und 0 auf Bank 9 (C2). Die 2 ist zugleich
     *        die GEGENPROBE: der Zaehler sieht diesen Opcode ueberhaupt.
     *   0x58 (Flag-Ck2): 0 Vorkommen, und er ist ohnehin ein LESER — op_flag_ck2
     *        (scd_vm.c) ruft nur re15_game_flag_get, LAB_8003fd54 hat keinen Store.
     * Damit gilt C2 und C6 ueber ALLE Flag-Opcodes der Maschine, nicht nur ueber Set. */
    printf("  C7 Flag-Set2 (0x59): %ld Vorkommen, davon Bank 3: %ld, Bank 9: %ld"
           " | Flag-Ck2 (0x58, nur LESER): %ld\n",
           z.op59, z.op59_bank3, z.op59_bank9, z.op58);
    PRUEFE(z.op59 == 2, "C7: %ld x 0x59 statt 2 — der Zaehler sieht den Opcode nicht"
           " (dann sind die zwei Nullen darunter wertlos)", z.op59);
    PRUEFE(z.op59_bank3 == 0,
           "C7: %ld x 0x59 auf Bank 3 — flag(3,32) haette einen zweiten Schreiber",
           z.op59_bank3);
    PRUEFE(z.op59_bank9 == 0,
           "C7: %ld x 0x59 auf Bank 9 — ein zone-9-Bit koennte doch geloescht werden",
           z.op59_bank9);
    PRUEFE(z.op58 == 0, "C7: %ld x 0x58 — erwartet 0", z.op58);
}

/* =========================================================================
 * TEIL D — die Ja/Nein-Auswahl steht auf den ORIGINAL-Zahlen
 *
 * Gepinnt werden die fuenf Zahlen des einzigen Ja/Nein-Zeichners des Spiels
 * (LAB_80028564, Zustand 4 der Nachrichten-FSM) und das Blink-Gatter:
 *     Yes-Text  174   @0x80028680  ori a0,a0,0xae
 *     No-Text   244   = 174 + 0x46 (Schrittweite @0x8002863c-@0x8002864c)
 *     Zeile     196   @0x80027f14 (0xb4) + @0x80028674 (+0x10)
 *     Cursor    160   @0x80028650  addiu v0,v0,0xa0            (Wahl = Yes)
 *               230   = 160 + 0x46                             (Wahl = No)
 *     Blinken   Maske 0x18 @0x80028600, Zaehler -1/Bild @0x800285f0,
 *               beim Umschalten erst 0 (@0x800285d4), dann -1 -> 0xFF.
 *
 * GEGENPROBE: die fuenf ZUVOR im Port stehenden GERATENEN Zahlen (Yes 190 / No 234 /
 * Zeile 202 / Cursor 180/224 bei Zeile 203) muessen alle verfehlt werden — sonst
 * stuende der Riegel auch auf dem alten Stand gruen.
 * ========================================================================= */
static void teil_d(void)
{
    printf("\n=== TEIL D: Ja/Nein-Auswahl auf den Original-Zahlen (LAB_80028564) ===\n");
    re15_msg_select_t ja, nein;
    re15_msg_select_layout(0, 0xff, &ja);     /* Wahl Yes, Blink offen */
    re15_msg_select_layout(1, 0xff, &nein);   /* Wahl No,  Blink offen */

    PRUEFE(ja.opt[0].x == 174, "Yes-Spalte %d statt 174 (@0x80028680)", ja.opt[0].x);
    PRUEFE(ja.opt[1].x == 244, "No-Spalte %d statt 244 (174 + 0x46 @0x8002864c)", ja.opt[1].x);
    PRUEFE(ja.opt[0].y == 196 && ja.opt[1].y == 196,
           "Zeile %d/%d statt 196 (@0x80027f14 + @0x80028674)", ja.opt[0].y, ja.opt[1].y);
    PRUEFE(ja.cursor_x == 160, "Cursor bei Yes %d statt 160 (@0x80028650)", ja.cursor_x);
    PRUEFE(nein.cursor_x == 230, "Cursor bei No %d statt 230 (160 + 0x46)", nein.cursor_x);
    PRUEFE(ja.cursor_y == 196 && nein.cursor_y == 196,
           "Cursor-Zeile %d/%d statt 196 (@0x80028630)", ja.cursor_y, nein.cursor_y);
    /* Die Glyphen sind die der .msg-Schrift: "Yes" / "No". */
    PRUEFE(ja.opt[0].len == 3 && ja.opt[0].glyphs[0] == 0x35
           && ja.opt[0].glyphs[1] == 0x41 && ja.opt[0].glyphs[2] == 0x4f, "\"Yes\"-Glyphen falsch");
    PRUEFE(ja.opt[1].len == 2 && ja.opt[1].glyphs[0] == 0x2a
           && ja.opt[1].glyphs[1] == 0x4b, "\"No\"-Glyphen falsch");

    /* GEGENPROBE gegen die frueheren geratenen Zahlen. */
    PRUEFE(ja.opt[0].x != 190 && ja.opt[1].x != 234 && ja.opt[0].y != 202
           && ja.cursor_x != 180 && nein.cursor_x != 224 && ja.cursor_y != 203,
           "GEGENPROBE: eine der geratenen Zahlen (190/234/202/180/224/203) steht noch");

    /* --- Blink-Gatter: Maske 0x18 @0x80028600 -------------------------------------- */
    int sichtbar = 0, unsichtbar = 0;
    for (int z = 0; z < 256; z++) {
        re15_msg_select_t s; re15_msg_select_layout(0, (uint8_t)z, &s);
        int erwartet = (z & 0x18) ? 1 : 0;
        PRUEFE(s.cursor_visible == erwartet,
               "Blink-Gatter bei Zaehler 0x%02X: %d statt %d (Maske 0x18 @0x80028600)",
               z, s.cursor_visible, erwartet);
        if (erwartet) sichtbar++; else unsichtbar++;
    }
    printf("  Blink-Gatter ueber alle 256 Zaehlerstaende: %d sichtbar / %d aus\n",
           sichtbar, unsichtbar);
    /* Maske 0x18 = Bits 3 und 4. Verdeckt ist der Cursor nur, wenn BEIDE klar sind —
     * das ist 1 von 4 Bitpaaren, also 64 von 256 Zaehlerstaenden. Ueber 32 aufeinander
     * folgende Bilder: 8 aus, 24 an. (Der erste Ansatz dieses Riegels stand hier auf 96
     * und war schlicht falsch gerechnet; der Riegel hat es gefangen.) */
    PRUEFE(sichtbar == 192 && unsichtbar == 64,
           "Maske 0x18 muesste 192/256 Bilder zeigen, gemessen %d", sichtbar);
    /* GEGENPROBE: ein Cursor OHNE Gatter stuende in allen 256 Bildern. */
    PRUEFE(sichtbar != 256, "GEGENPROBE: der Cursor blinkt gar nicht");

    /* --- Zaehler-Takt: -1 je Bild, beim Umschalten erst 0 dann -1 (= 0xFF) ---------- */
    PRUEFE(re15_msg_select_blink_tick(0x10, 0) == 0x0f,
           "Zaehler laeuft nicht um 1 herunter (@0x800285f0)");
    PRUEFE(re15_msg_select_blink_tick(0x00, 0) == 0xff,
           "Zaehler laeuft bei 0 nicht auf 0xFF ueber (@0x800285f0)");
    PRUEFE(re15_msg_select_blink_tick(0x07, 1) == 0xff,
           "Umschalten muss den Zaehler auf 0 setzen (@0x800285d4) und DANN -1 (@0x800285f0)");
    { re15_msg_select_t s; re15_msg_select_layout(1, re15_msg_select_blink_tick(0x07, 1), &s);
      PRUEFE(s.cursor_visible, "Cursor nach dem Umschalten nicht sofort sichtbar"); }
    printf("  Zahlen und Gatter stimmen mit LAB_80028564 ueberein.\n");
}

/* =========================================================================
 * TEIL E — DIE NACHRICHT haelt die Abfrage zurueck, NICHT die Szene.
 *
 * ⛔ DIESER TEIL PRUEFT SEIT RUNDE 23 DAS GEGENTEIL VON VORHER, und zwar mit
 * Beleg: RE2s Fortsetzung kennt GENAU EINE Schranke, das Belegt-Bit des
 * Nachrichtensystems —
 *     800517e0  lbu  v0,-0x78c4(v0)        DAT_800e873c
 *     800517f0  andi v0,v0,0x80
 *     800517f4  bne  v0,zero,LAB_80051870
 * — und ein Szenen-Fenster gibt es dort nicht. Der Riegel von Runde 22 hielt die
 * Abfrage zusaetzlich waehrend flag(2,7)/flag(1,27) zurueck; GENAU DAS hat die
 * Spanne "vorgemerkt" ueber den Nachrichten-Freeze hinaus verlaengert und das
 * Loch aufgerissen (TEIL I zerlegt es). Gefragt wird jetzt also:
 *   (1) offene Nachricht -> Abfrage bleibt zu, EGAL ob eine Szene laeuft;
 *   (2) Nachricht ausgeredet -> Abfrage geht auf, EGAL ob eine Szene laeuft.
 * Der Pad ist in beiden Faellen gesperrt (die Nachricht hat Maske 0xFFFF0000, die
 * Szene setzt 0x01000000 selbst) — es entsteht also kein Loch.
 * ========================================================================= */
static void teil_e(void)
{
    extern uint16_t g_scd_pad_edge, g_scd_pad_held;
    printf("\n=== TEIL E: die NACHRICHT haelt zurueck, nicht die Szene ===\n");
    static const struct { unsigned room; uint8_t msg, item; const char *was; } f[] = {
        { 0x1090,  9, 0x31, "Fire Extinguisher"      },
        { 0x11E0, 12, 0x30, "Pliers"                 },
        { 0x1100,  4, 0x44, "Minidisc Player w/ Disc" },
    };
    int gefahren = 0;
    for (unsigned i = 0; i < sizeof f / sizeof f[0]; i++) {
        re15_rdt_t rdt; size_t n = 0;
        uint8_t *raw = raum_laden(f[i].room, &rdt, &n);
        if (!raw) { printf("  SKIP ROOM%04X\n", f[i].room); continue; }

        /* --- (1) Nachricht OFFEN (und Szene laeuft): Abfrage bleibt zu -------------- */
        grundzustand();
        re15_inv_grant(f[i].item, 1);
        re15_game_flag_set(2, 7, 1);
        PRUEFE(re15_cine_active(), "flag(2,7)=1 macht re15_cine_active() nicht wahr");
        g_current_room_id = f[i].room;
        re15_msg_load_room_block(rdt.messages, rdt.messages_size);
        s_prog[1] = f[i].msg;
        scd_thread_start(0, s_prog);
        g_scd_pad_edge = 0; g_scd_pad_held = 0;
        scd_vm_tick();                                   /* Message_on oeffnet          */
        { const unsigned char *r; int l, id; re15_msg_tick(&r, &l, &id); }
        PRUEFE(re15_discard_active(), "ROOM%04X: die Abfrage wird nicht vorgemerkt", f[i].room);
        PRUEFE(re15_pauseflags_belegt(),
               "ROOM%04X: die Nachricht setzt das Belegt-Bit nicht", f[i].room);
        for (int k = 0; k < 20; k++) re15_discard_tick(0, 0);
        PRUEFE(re15_discard_prompt(NULL, NULL) == 0,
               "ROOM%04X (%s): die Abfrage ging auf, waehrend die Nachricht noch stand",
               f[i].room, f[i].was);
        { int slot = re15_inv_find_item(f[i].item);
          PRUEFE(slot >= 0 && g_inv.slots[slot].qty == 1,
                 "ROOM%04X: die Anzahl faellt schon vor der Frage (%d statt 1)",
                 f[i].room, slot >= 0 ? g_inv.slots[slot].qty : -1); }
        PRUEFE((g_re15_pauseflags & 0x01000000u) != 0,
               "ROOM%04X: waehrend der Nachricht ist der Pad frei", f[i].room);

        /* --- (2) Nachricht ausgeredet, Szene laeuft WEITER: Abfrage geht auf -------- */
        for (int k = 0; k < 4000 && re15_pauseflags_belegt(); k++) {
            const unsigned char *r; int l, id;
            g_scd_pad_edge = ((k % 4) == 0) ? 0x4000u : 0u;
            if (!re15_discard_frozen()) scd_vm_tick();
            re15_msg_tick(&r, &l, &id);
            g_scd_pad_edge = 0;
            if (re15_discard_active()) re15_discard_tick(0, 0);
        }
        PRUEFE(re15_cine_active(), "ROOM%04X: die Szene ist zwischendurch ausgegangen",
               f[i].room);
        int bild = -1;
        for (int k = 0; k < 8; k++) {
            if (re15_discard_prompt(NULL, NULL)) { bild = k; break; }
            re15_discard_tick(0, 0);
        }
        PRUEFE(bild >= 0, "ROOM%04X (%s): die Abfrage kommt nach der Nachricht nicht",
               f[i].room, f[i].was);
        { uint8_t it = 0; re15_discard_prompt(&it, NULL);
          PRUEFE(it == f[i].item, "ROOM%04X: falscher Gegenstand", f[i].room); }
        { int slot = re15_inv_find_item(f[i].item);
          PRUEFE(slot >= 0 && g_inv.slots[slot].qty == 0,
                 "ROOM%04X: die Anzahl faellt nicht im Bild der Frage (@0x80051810)",
                 f[i].room); }
        printf("  %-24s ROOM%04X msg %2u: waehrend der Nachricht zu, %d Bild(er) nach"
               " dem Belegt-Bit offen — MIT laufender Szene\n",
               f[i].was, f[i].room, f[i].msg, bild);

        /* --- GEGENPROBE: derselbe Ablauf OHNE Szene -> genauso ---------------------- */
        grundzustand();
        re15_inv_grant(f[i].item, 1);
        nachricht_spielen(&rdt, f[i].room, f[i].msg);
        PRUEFE(re15_discard_prompt(NULL, NULL) != 0,
               "GEGENPROBE ROOM%04X: ohne Szene kommt die Abfrage gar nicht", f[i].room);
        PRUEFE(!re15_cine_active(), "GEGENPROBE ROOM%04X: die Szene laeuft doch", f[i].room);

        gefahren++;
        free(raw);
    }
    printf("  ABDECKUNG: %d von 3 Faellen gefahren, je 3 Pruefungen (Nachricht haelt /"
           " danach offen TROTZ Szene / Gegenprobe ohne Szene)\n", gefahren);
    PRUEFE(gefahren == 3, "nicht alle drei Faelle gefahren");
}

/* =========================================================================
 * Werkzeuge fuer TEIL I/J/K: das ausgelieferte Unterprogramm finden und im
 * echten VM fahren.
 * ========================================================================= */
/* Das Unterprogramm suchen, das `Message_on <msg_id>` enthaelt. Die Region wird durch den
 * NAECHSTEN Unterprogramm-Zeiger begrenzt, nicht durch das erste Evt_end — ein Evt_end in
 * einem Zweig (z.B. ROOM4001) beendet die Region nicht. */
static int sub_mit_nachricht(re15_rdt_t *rdt, uint8_t msg_id)
{
    for (int s = 0; s < rdt->sub_scd_count; s++) {
        const uint8_t *p = rdt->sub_scd[s];
        if (!p) continue;
        long grenze = 4096;
        if (s + 1 < rdt->sub_scd_count && rdt->sub_scd[s+1] > p)
            grenze = rdt->sub_scd[s+1] - p;
        for (long o = 0; o < grenze; ) {
            int sz = scd_opcode_size_at(p + o);
            if (sz <= 0) break;
            if (p[o] == 0x2B && p[o+1] == msg_id) return s;   /* Message_on <id> */
            o += sz;
        }
    }
    return -1;
}

/* Den Tuer-Spawn des Raums holen (re15_room_spawns[] parallel zu re15_room_ids[]) —
 * ein Punkt, an dem der Spieler im Spiel wirklich steht und laufen kann. */
static const re15_room_spawn_t *spawn_von(unsigned room)
{
    for (int i = 0; i < RE15_ROOM_COUNT; i++)
        if (re15_room_ids[i] == room) return &re15_room_spawns[i];
    return NULL;
}

/* Raum wie im Spiel aufbauen, damit re15_game_step echte Wege rechnet. */
static int raum_booten(re15_rdt_t *rdt, unsigned room, re15_game_ctx_t *ctx,
                       re15_camera_view_t *cam)
{
    const re15_room_spawn_t *sp = spawn_von(room);
    if (!sp || (sp->x == 0 && sp->z == 0)) return 0;
    grundzustand();
    re15_player_cmd_reset();
    memset(cam, 0, sizeof *cam);
    memset(ctx, 0, sizeof *ctx);
    ctx->rdt = rdt; ctx->rdt_ok = 1; ctx->cam_view = cam; ctx->active_cut = sp->cut;
    g_current_room_id = room; g_room_change.pending = 0;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100;
    pl->x = sp->x; pl->y = sp->y; pl->z = sp->z; pl->rot_y = sp->yaw;
    re15_collision_set_band(re15_collision_band_from_y(pl->y));
    re15_msg_load_room_block(rdt->messages, rdt->messages_size);
    scd_register_current_rdt(rdt);
    scd_register_room_events(rdt);
    scd_room_reenter(rdt, pl->x, pl->z, sp->cut);
    g_scd.cut_auto_enabled = 1;
    for (int f = 0; f < 20; f++) {                 /* Raum einschwingen lassen          */
        const unsigned char *r; int l, id;
        scd_vm_tick(); re15_msg_tick(&r, &l, &id);
        ctx->pad_current = 0; ctx->pad_pressed = 0;
        re15_game_step(ctx);
    }
    return 1;
}

/* =========================================================================
 * TEIL I — WORAUS DIE PORT-WARTEZEIT BESTAND, Anteil fuer Anteil.
 *
 * Gemessen wird ZWEIMAL dasselbe Unterprogramm aus der RDT, im echten VM, mit
 * registriertem RDT (ohne das sind Gosub/Evt_exec stille No-Ops):
 *
 *  Lauf 1 — MIT dem Gegenstand. Die Abfrage wird vorgemerkt; gemessen wird, in
 *      welchem Bild das Belegt-Bit des Nachrichtensystems frei wird
 *      (re15_pauseflags_belegt, = RE2 @0x800517f0 `andi v0,v0,0x80`) und in
 *      welchem Bild die Abfrage aufgeht. VERLANGT WIRD: dasselbe Bild.
 *      Danach friert der sichtbare Prompt das Skript ein (byte-true
 *      @0x80051850 mit 0xFF000000, SCD-Bit 0x02000000 @0x8003f044), der Lauf
 *      kann also nicht weiterlaufen — deshalb der zweite Lauf.
 *
 *  Lauf 2 — OHNE den Gegenstand. Nichts wird vorgemerkt, das Unterprogramm
 *      laeuft unbehindert durch, und die drei Schranken des Runde-22-Modells
 *      werden Bild fuer Bild mitgeschrieben:
 *          NACHRICHT  re15_pauseflags_belegt()        (die EINE echte Schranke)
 *          SZENE      re15_cine_active()              (Port-Zutat, Runde 21)
 *          FADEN      g_scd.threads[0].active         (Port-Zutat, Runde 21)
 *      Der UEBERHANG ist die Spanne vom Ende der Nachricht bis zu dem Bild, in
 *      dem alle drei frei sind — genau die Spanne, in der der Spieler frei lief,
 *      waehrend die Abfrage armiert und unsichtbar war. Je Anteil wird die
 *      Bildzahl ausgewiesen; die Bilder mit FREIEM Pad sind das Loch.
 *
 * GEGENPROBE IM SELBEN LAUF: waere der Ueberhang 0, wuerde dieser Riegel nichts
 * messen. Zwei PRUEFE erzwingen, dass er da ist (Summe > 0, groesster >= 20).
 * ========================================================================= */
typedef struct {
    long b_msgende;      /* erstes Bild mit freiem Belegt-Bit                      */
    long b_alt;          /* erstes Bild, in dem ALLE DREI Schranken frei sind      */
    long n_nachricht;    /* Bilder mit gehaltenem Belegt-Bit (Anteil NACHRICHT)    */
    long n_szene;        /* Ueberhang-Bilder, die die SZENE hielt                  */
    long n_faden;        /* Ueberhang-Bilder, die nur der FADEN hielt              */
    long n_msg2;         /* Ueberhang-Bilder, die eine FOLGE-Nachricht hielt       */
    long loch;           /* Ueberhang-Bilder mit FREIEM Pad                        */
} spanne_t;

/* Lauf 2: das Unterprogramm OHNE Vormerkung fahren und die Anteile zaehlen. */
static int spanne_messen(re15_rdt_t *rdt, unsigned room, uint8_t msg, int sub, spanne_t *out)
{
    extern uint16_t g_scd_pad_edge, g_scd_pad_held;
    memset(out, 0, sizeof *out);
    out->b_msgende = out->b_alt = -1;
    grundzustand();
    g_current_room_id = room;
    re15_msg_load_room_block(rdt->messages, rdt->messages_size);
    scd_register_current_rdt(rdt);
    scd_thread_start(0, rdt->sub_scd[sub]);
    long b_start = -1;
    for (long fr = 0; fr < 20000; fr++) {
        const unsigned char *r; int l, id;
        g_scd_pad_edge = (fr > 2 && (fr % 4) == 0) ? 0x4000u : 0u;
        g_scd_pad_held = 0;
        scd_vm_tick();
        re15_msg_tick(&r, &l, &id);
        g_scd_pad_edge = 0;
        int belegt = re15_pauseflags_belegt();
        /* Startmarke: DIE Nachricht dieser Benutzungsstelle steht. */
        if (b_start < 0 && belegt && g_scd.message_id == msg) b_start = fr;
        if (b_start < 0) continue;
        if (out->b_msgende < 0) {
            if (belegt) { out->n_nachricht++; continue; }
            out->b_msgende = fr;
        }
        int szene = re15_cine_active();
        int faden = g_scd.threads[0].active;
        if (!belegt && !szene && !faden) { out->b_alt = fr; break; }
        if (belegt)      out->n_msg2++;
        else if (szene)  out->n_szene++;
        else             out->n_faden++;
        if (!(g_re15_pauseflags & 0x01000000u)) out->loch++;
    }
    scd_register_current_rdt(NULL);
    return b_start >= 0;
}

static spanne_t s_spanne[RE15_DISCARD_SITE_COUNT];
static int      s_spanne_ok[RE15_DISCARD_SITE_COUNT];
static int      s_sub_idx[RE15_DISCARD_SITE_COUNT];

static void teil_i(void)
{
    extern uint16_t g_scd_pad_edge, g_scd_pad_held;
    printf("\n=== TEIL I: die Spanne \"vorgemerkt\", Anteil fuer Anteil ===\n");
    printf("  %-22s %8s | %9s %6s %6s %6s | %6s %7s\n",
           "Stelle", "Nachr.", "UEBERH.", "Szene", "Faden", "Msg2", "Loch", "Frage");
    int gefahren = 0, ausgelassen = 0, bruecke_stellen = 0;
    long ueberhang_summe = 0, loch_summe = 0, groesster = 0, bruecke_summe = 0;
    unsigned gr_raum = 0; uint8_t gr_msg = 0;

    for (int i = 0; i < RE15_DISCARD_SITE_COUNT; i++) {
        unsigned room = re15_discard_sites[i].room;
        uint8_t  msg  = re15_discard_sites[i].msg;
        uint8_t  item = re15_discard_sites[i].item;
        s_spanne_ok[i] = 0; s_sub_idx[i] = -1;

        re15_rdt_t rdt; size_t n = 0;
        uint8_t *raw = raum_laden(room, &rdt, &n);
        if (!raw) { printf("  SKIP ROOM%04X\n", room); continue; }
        int sub = sub_mit_nachricht(&rdt, msg);
        if (sub < 0) {
            printf("  AUSGELASSEN ROOM%04X msg %2u: kein Message_on %u in einer sub-Region\n",
                   room, (unsigned)msg, (unsigned)msg);
            ausgelassen++; free(raw); continue;
        }
        s_sub_idx[i] = sub;

        /* ---------- Lauf 1: MIT Gegenstand — wann geht die Abfrage auf? ------------ */
        grundzustand();
        g_current_room_id = room;
        re15_msg_load_room_block(rdt.messages, rdt.messages_size);
        scd_register_current_rdt(&rdt);
        re15_inv_grant(item, 1);
        scd_thread_start(0, rdt.sub_scd[sub]);
        long b_vor = -1, b_frei = -1, b_frage = -1;
        long ohne_pad = 0, ohne_scd = 0, spanne = 0, ohne_bruecke = 0;
        for (long fr = 0; fr < 20000; fr++) {
            const unsigned char *r; int l, id;
            g_scd_pad_edge = (fr > 2 && (fr % 4) == 0) ? 0x4000u : 0u;
            g_scd_pad_held = 0;
            if (!re15_discard_frozen()) scd_vm_tick();
            re15_msg_tick(&r, &l, &id);
            g_scd_pad_edge = 0;
            if (b_vor < 0 && re15_discard_active()) b_vor = fr;
            if (b_vor >= 0 && b_frei < 0 && !re15_pauseflags_belegt()) b_frei = fr;
            /* ⛔ DIE EIGENSCHAFT, AUF DER DER GANZE FIX RUHT: solange vorgemerkt und
             * unsichtbar, haelt die ausloesende Nachricht BEIDE Pause-Bits — Pad
             * 0x01000000 (@0x800304f4-@0x8003051c, Eingabe auf 0xf000) UND Skript
             * 0x02000000 (@0x8003f044 `lui v1,0x200` / @0x8003f04c). Damit kann in der
             * Spanne weder der Spieler noch das Skript etwas tun; das eine SCHLIESS-Bild,
             * in dem die Maske schon weg ist, deckt re15_discard_frozen() ab (RE2 legt sie
             * im selben Aufruf wieder an, @0x80051844/@0x80051850).
             * Zensus der Daten dahinter: tools/discard_maskenzensus.py — 16 von 16
             * Benutzungsstellen oeffnen mit 0xFFFF0000. */
            if (re15_discard_active() && !re15_discard_prompt(NULL, NULL)) {
                spanne++;
                /* ⛔ DER ZEUGE IST DAS BIT, DAS DEN SPIELER WIRKLICH HAELT:
                 * RE15_PAUSE_PLAYER 0x80000000 = FUN_80031c44s Vorzeichen-Gate
                 * (@0x80031c54 `lw a0,g_pauseflags` / @0x80031c78 `bltz a0,0x80031da8`),
                 * das den GANZEN Spieler-Dispatch ueberspringt.
                 * HIER STAND BIS RUNDE 23 RE15_PAUSE_PAD 0x01000000 — der FALSCHE Zeuge:
                 * der maskiert nur die SCD-Pad-Woerter (game_step_common.c:1061-65,
                 * @0x800304f4-@0x8003051c) und wird ZUSAETZLICH vom Szenen-Fenster
                 * flag(1,27)/flag(2,7) gehalten. An den Stellen mit laufender Szene war er
                 * gesetzt, obwohl der Nachrichten-Freeze schon weg war — genau deshalb
                 * meldete der Riegel 4 Bruecken-Bilder statt eines je Stelle. */
                if (!(g_re15_pauseflags & RE15_PAUSE_PLAYER) && !re15_discard_frozen()) ohne_pad++;
                if (!(g_re15_pauseflags & RE15_PAUSE_SCD)    && !re15_discard_frozen()) ohne_scd++;
                /* GEGENPROBE zur Bruecke: dieselbe Zaehlung OHNE den frozen()-Ausweg.
                 * Sie MUSS Bilder finden — sonst waere die Zeile
                 * `s_zustand == D_WARTET && !re15_pauseflags_belegt()` in
                 * re15_discard_frozen() wirkungslos und die Masken-Pruefung darueber
                 * eine Tautologie. */
                if (!(g_re15_pauseflags & RE15_PAUSE_PLAYER)) ohne_bruecke++;
            }
            if (re15_discard_active()) re15_discard_tick(0, 0);
            if (b_frage < 0 && re15_discard_prompt(NULL, NULL)) { b_frage = fr; break; }
        }
        scd_register_current_rdt(NULL);
        if (b_vor < 0) {
            /* ⛔ UNTERSCHEIDUNG, NICHT NACHSICHT: die Kartenleser (ROOM10D0/10D1/1230/
             * 1231, ROOM11E0/11E1 msg 9) haengen ihre Benutzungs-Nachricht hinter eine
             * Verzweigung auf die Ja/Nein-Antwort davor (ROOM10D0 sub20 @0x19B8 `Ifel_ck`
             * / @0x19BC `Ck(12,31,0)`). Ohne Spieler-Antwort geht der Zweig nach Else, die
             * Abfrage wird also gar nicht erst vorgemerkt — die Stelle ist NICHT ERREICHT,
             * nicht "gruen". Im Spiel belegt der Live-Abzug, dass sie dort aufgeht. */
            printf("  AUSGELASSEN ROOM%04X sub%02d msg %2u: nicht erreicht (Verzweigung auf"
                   " die Ja/Nein-Antwort)\n", room, sub, (unsigned)msg);
            ausgelassen++; free(raw); continue;
        }
        /* ⛔ EIN FALL MIT sichtbar=0 DARF NICHT ALS ERFOLG ZAEHLEN. */
        PRUEFE(b_frage >= 0, "ROOM%04X msg %2u: die Abfrage geht GAR NICHT auf",
               room, (unsigned)msg);
        PRUEFE(b_frei >= 0 && b_frage == b_frei,
               "ROOM%04X msg %2u: Belegt-Bit frei in Bild %ld, Abfrage erst in Bild %ld"
               " (RE2: dasselbe Bild, @0x800517f4)", room, (unsigned)msg, b_frei, b_frage);
        PRUEFE(spanne > 0, "ROOM%04X msg %2u: die Spanne \"vorgemerkt+unsichtbar\" ist LEER"
               " — dann prueft die Masken-Pruefung nichts", room, (unsigned)msg);
        PRUEFE(ohne_pad == 0, "ROOM%04X msg %2u: %ld von %ld Spannen-Bildern OHNE"
               " Spieler-Bit 0x80000000 (@0x80031c78)",
               room, (unsigned)msg, ohne_pad, spanne);
        PRUEFE(ohne_scd == 0, "ROOM%04X msg %2u: %ld von %ld Spannen-Bildern OHNE Skript-Bit"
               " 0x02000000", room, (unsigned)msg, ohne_scd, spanne);
        /* ⛔ JE STELLE GENAU EIN BILD. Das ist das SCHLIESS-Bild, in dem der Port den
         * Nachrichten-Freeze schon geloest und die Abfrage noch nicht aufgemacht hat;
         * re15_discard_frozen() traegt es. RE2 hat dieses Bild NICHT — dort liegen
         * Dekrement (@0x80051810), Frage (@0x80051834) und neuer Freeze (@0x80051844
         * `lui v1,0xff00` / @0x80051850 `sw v0,DAT_800cfbdc`) in EINEM Aufruf, also
         * 0 Bilder. Eine andere Zahl als 1 heisst: die Bruecke deckt nicht genau das
         * eine Bild, sondern mehr oder gar nichts. */
        PRUEFE(ohne_bruecke == 1,
               "ROOM%04X msg %2u: %ld Bruecken-Bilder statt genau 1 (RE2 hat 0,"
               " @0x80051844 friert im selben Aufruf wieder ein)",
               room, (unsigned)msg, ohne_bruecke);
        bruecke_summe += ohne_bruecke;
        bruecke_stellen++;

        /* ---------- Lauf 2: OHNE Gegenstand — die Anteile des Ueberhangs ----------- */
        spanne_t sp;
        int ok = spanne_messen(&rdt, room, msg, sub, &sp);
        PRUEFE(ok, "ROOM%04X msg %2u: Lauf 2 findet die Nachricht nicht", room, (unsigned)msg);
        if (ok) {
            s_spanne[i] = sp; s_spanne_ok[i] = 1;
            long ueberhang = (sp.b_alt >= 0 && sp.b_msgende >= 0)
                           ? sp.b_alt - sp.b_msgende : -1;
            char nam[24], ue[12];
            snprintf(nam, sizeof nam, "ROOM%04X sub%02d m%u", room, sub, (unsigned)msg);
            /* ⛔ Ueberhang "NIE" = das Runde-22-Modell haette hier ueberhaupt nicht
             * gefragt: der Faden endet in diesem Pruefstand nicht (ROOM1090 sub03 laeuft
             * @0x261C `Plc_dest(mode 9, flag 33)` + @0x2624 `Gosub 5` in eine
             * Do/Edwhile-Schleife auf das Ankunftsflag des Spieler-Weges), und das
             * Szenen-Fenster bleibt offen. Genau das war Runde 22s eigener Befund. */
            if (ueberhang > 0) snprintf(ue, sizeof ue, "%ld", ueberhang);
            else               snprintf(ue, sizeof ue, "NIE");
            printf("  %-22s %8ld | %9s %6ld %6ld %6ld | %6ld %7ld\n",
                   nam, sp.n_nachricht, ue, sp.n_szene, sp.n_faden, sp.n_msg2,
                   sp.loch, b_frage - b_vor);
            if (ueberhang > 0) {
                ueberhang_summe += ueberhang;
                loch_summe      += sp.loch;
                if (sp.loch > groesster) { groesster = sp.loch; gr_raum = room; gr_msg = msg; }
            }
        }
        gefahren++;
        free(raw);
    }
    printf("  ABDECKUNG: %d von %d Benutzungsstellen gefahren, %d mit Grund ausgelassen\n",
           gefahren, RE15_DISCARD_SITE_COUNT, ausgelassen);
    printf("  Die Spanne \"vorgemerkt\" endet JETZT an allen %d gefahrenen Stellen im\n"
           "  Bild, in dem das Belegt-Bit frei wird (RE2 @0x800517f4).\n", gefahren);
    printf("  Der UEBERHANG des Runde-22-Modells betrug zusammen %ld Bilder, davon\n"
           "  %ld mit FREIEM Pad; groesstes Loch ROOM%04X msg %u mit %ld Bildern.\n",
           ueberhang_summe, loch_summe, gr_raum, (unsigned)gr_msg, groesster);
    PRUEFE(gefahren + ausgelassen == RE15_DISCARD_SITE_COUNT,
           "%d Stellen weder gefahren noch mit Grund ausgelassen",
           RE15_DISCARD_SITE_COUNT - gefahren - ausgelassen);
    /* GEGENPROBE: der Ueberhang MUSS messbar gewesen sein, sonst misst der Riegel nichts. */
    PRUEFE(loch_summe > 0,
           "GEGENPROBE: das Runde-22-Modell hat gar kein Loch hinterlassen — dann prueft"
           " dieser Riegel nichts");
    PRUEFE(groesster >= 20, "GEGENPROBE: groesstes Loch nur %ld Bilder", groesster);
    printf("  Das SCHLIESS-Bild: ohne den frozen()-Ausweg waeren %ld Spannen-Bilder ohne\n"
           "  das Spieler-Bit 0x80000000 (@0x80031c78) — 1 Bild an %d von %d Stellen.\n"
           "  RE2 hat 0 solche Bilder (@0x80051810 dekrementiert, @0x80051844 friert im\n"
           "  SELBEN Aufruf wieder ein). Das Fenster ist damit ABGESICHERT, nicht weg:\n"
           "  aus 161 Bildern Ueberhang (ROOM1100/1101, Runde-22-Modell) ist 1 Bild je\n"
           "  Stelle geworden, und das haelt die Port-Zeile re15_discard_frozen().\n",
           bruecke_summe, bruecke_stellen, gefahren);
    PRUEFE(bruecke_summe > 0,
           "GEGENPROBE: ohne die Bruecke gibt es kein Bild ohne Spieler-Bit — dann"
           " prueft die Masken-Pruefung sich selbst");
    PRUEFE(bruecke_stellen == gefahren && bruecke_summe == gefahren,
           "Bruecken-Bilder: %ld an %d Stellen, erwartet je 1 an allen %d gefahrenen",
           bruecke_summe, bruecke_stellen, gefahren);
}

/* =========================================================================
 * TEIL J — WAS DER SPIELER SPUERT: px/pz hinter dem ECHTEN re15_game_step.
 *
 * ⛔ WARUM NICHT MEHR UEBER EIN PRAEDIKAT: der Riegel von Runde 22 prueft
 * `vorgemerkt && !sichtbar && (pad_alt && !pad_locked)`, und
 * re15_discard_pad_locked() gab WOERTLICH dasselbe zurueck wie
 * re15_discard_active() (beide `s_zustand != D_AUS`). Die Bedingung war damit
 * `x && !x` = konstant 0 — gruen auch ohne jede Aenderung. Hier wird stattdessen
 * gemessen, was der Spieler spuert: die Summe |dx|+|dz| des Spielers hinter dem
 * echten re15_game_step, in genau den Bildern, in denen die Abfrage vorgemerkt
 * und unsichtbar ist. Gefahren wird mit gehaltenem VORWAERTS.
 *
 * DREI SPALTEN IM SELBEN LAUF:
 *   JETZT       — der ausgelieferte Stand. Muss 0 sein.
 *   GEGENPROBE  — dieselbe Strecke, aber ueber die UEBERHANG-Spanne des
 *                 Runde-22-Modells (aus TEIL I), gefahren ohne Vormerkung, damit
 *                 das Skript nicht einfriert. Muss > 0 sein: das ist das Loch in
 *                 Einheiten, die der Spieler spuert.
 *   STUB        — dieselbe Schleife, aber OHNE re15_game_step. Muss 0 sein — so
 *                 ist belegt, dass eine 0 aus dem Stillstand kommt und nicht
 *                 schon aus dem Messaufbau.
 * Und je Stelle: die Abfrage geht auf, und nach der Antwort laeuft der Spieler
 * wieder (der Riegel geht also wieder auf).
 * ========================================================================= */
static long j_weg_ueberhang(re15_rdt_t *rdt, unsigned room, int sub,
                            long b_von, long b_bis, int mit_step)
{
    extern uint16_t g_scd_pad_edge, g_scd_pad_held;
    re15_game_ctx_t ctx; re15_camera_view_t cam;
    if (!raum_booten(rdt, room, &ctx, &cam)) return -1;
    scd_thread_start(0, rdt->sub_scd[sub]);
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    long weg = 0, b_start = -1;
    for (long fr = 0; fr < 20000; fr++) {
        const unsigned char *r; int l, id;
        g_scd_pad_edge = (fr > 2 && (fr % 4) == 0) ? 0x4000u : 0u;
        g_scd_pad_held = 0;
        scd_vm_tick();
        re15_msg_tick(&r, &l, &id);
        g_scd_pad_edge = 0;
        if (b_start < 0 && re15_pauseflags_belegt()) b_start = fr;
        int32_t x0 = pl->x, z0 = pl->z;
        ctx.pad_current = RE15_PAD_BIT_UP;
        ctx.pad_pressed = (fr == 0) ? RE15_PAD_BIT_UP : 0;
        if (mit_step) re15_game_step(&ctx);
        if (b_start >= 0) {
            long rel = fr - b_start;
            if (rel >= b_von && rel < b_bis) {
                long dx = (long)pl->x - x0, dz = (long)pl->z - z0;
                weg += (dx < 0 ? -dx : dx) + (dz < 0 ? -dz : dz);
            }
            if (rel >= b_bis) break;
        }
    }
    scd_register_current_rdt(NULL);
    return weg;
}

static void teil_j(void)
{
    extern uint16_t g_scd_pad_edge, g_scd_pad_held;
    printf("\n=== TEIL J: das px/pz-Delta hinter dem ECHTEN re15_game_step ===\n");
    printf("  %-22s %8s %10s %12s %8s %8s\n",
           "Stelle", "JETZT", "RUECKBAU", "GEGENPROBE", "STUB", "wieder");
    int gefahren = 0, mit_bewegung = 0, wieder_frei = 0, ohne_ueberhang = 0, rueck_stellen = 0;
    long jetzt_summe = 0, gegen_summe = 0, stub_summe = 0, rueck_summe = 0, rueck_ohne_span = 0;

    for (int i = 0; i < RE15_DISCARD_SITE_COUNT; i++) {
        if (!s_spanne_ok[i] || s_sub_idx[i] < 0) continue;
        unsigned room = re15_discard_sites[i].room;
        uint8_t  msg  = re15_discard_sites[i].msg;
        uint8_t  item = re15_discard_sites[i].item;
        int      sub  = s_sub_idx[i];
        spanne_t sp   = s_spanne[i];
        /* ⛔ KEIN STILLES UEBERSPRINGEN MEHR. Bis Runde 23 fiel hier jede Stelle
         * ohne b_alt per `continue` heraus und wurde damit als Erfolg GEZAEHLT, obwohl sie
         * gar nicht gefahren war — ROOM1090 ist genau so eine: sein Faden endet im
         * Pruefstand NICHT, weil sub03 `Gosub 5` macht und sub05 eine Warteschleife auf das
         * Ankunftsflag des Spieler-Weges ist:
         *     0x026F4  Do  11 00 08 00
         *     0x026F8  Evt_next
         *     0x026FC  Ck  21 05 21 00      ; Ck(5,33,0) — Edwhile-Bedingung
         * Ohne einen laufenden Plc_dest-Weg wird flag(5,33) nie gesetzt, die Schleife
         * laeuft ewig, und die UEBERHANG-Spanne (b_alt) existiert nicht. Die Stelle wird
         * deshalb mit GRUND ausgelassen — der RUECKBAU wird trotzdem gemessen, der
         * braucht nur b_msgende. */
        if (sp.b_msgende < 0) continue;

        re15_rdt_t rdt; size_t n = 0;
        uint8_t *raw = raum_laden(room, &rdt, &n);
        if (!raw) continue;

        re15_game_ctx_t ctx; re15_camera_view_t cam;
        if (!raum_booten(&rdt, room, &ctx, &cam)) {
            printf("  AUSGELASSEN ROOM%04X: kein Tuer-Spawn in re15_room_spawns[]\n", room);
            free(raw); continue;
        }
        /* ---------- JETZT: vorgemerkt + unsichtbar, VORWAERTS gehalten ------------- */
        re15_inv_grant(item, 1);
        scd_thread_start(0, rdt.sub_scd[sub]);
        re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
        long weg_jetzt = 0, gesperrt = 0, b_frage = -1;
        for (long fr = 0; fr < 20000; fr++) {
            const unsigned char *r; int l, id;
            g_scd_pad_edge = (fr > 2 && (fr % 4) == 0) ? 0x4000u : 0u;
            g_scd_pad_held = 0;
            if (!re15_discard_frozen()) scd_vm_tick();
            re15_msg_tick(&r, &l, &id);
            g_scd_pad_edge = 0;
            int vorgemerkt = re15_discard_active();
            int sichtbar   = re15_discard_prompt(NULL, NULL) != 0;
            int32_t x0 = pl->x, z0 = pl->z;
            ctx.pad_current = RE15_PAD_BIT_UP;
            ctx.pad_pressed = (fr == 0) ? RE15_PAD_BIT_UP : 0;
            re15_game_step(&ctx);
            if (vorgemerkt && !sichtbar) {
                long dx = (long)pl->x - x0, dz = (long)pl->z - z0;
                weg_jetzt += (dx < 0 ? -dx : dx) + (dz < 0 ? -dz : dz);
                gesperrt++;
            }
            /* Die Abfrage wird bewusst NICHT bedient (0 statt der Pad-Woerter), damit
             * die gesperrte Spanne ueberhaupt messbar bleibt. */
            if (re15_discard_active()) re15_discard_tick(0, 0);
            if (re15_discard_prompt(NULL, NULL)) { b_frage = fr; break; }
        }
        PRUEFE(b_frage >= 0, "ROOM%04X msg %2u: die Abfrage geht im echten Raum nicht auf",
               room, (unsigned)msg);
        PRUEFE(gesperrt > 0, "ROOM%04X msg %2u: es gab gar kein Bild mit vorgemerkter,"
               " unsichtbarer Abfrage — dann misst TEIL J hier nichts",
               room, (unsigned)msg);
        PRUEFE(weg_jetzt == 0,
               "ROOM%04X msg %2u: der Spieler legt %ld Einheiten zurueck, waehrend die"
               " Abfrage vorgemerkt und unsichtbar ist", room, (unsigned)msg, weg_jetzt);

        /* ---------- WIEDER AUF: antworten, dann laeuft der Spieler --------------- */
        for (long f = 0; f < 4000 && !re15_discard_ready(); f++) re15_discard_tick(0, 0);
        re15_discard_tick(0x4000, 0);                    /* SQUARE = Ja               */
        PRUEFE(!re15_discard_active(), "ROOM%04X: Abfrage nach der Antwort nicht beendet",
               room);
        long weg_wieder = 0;
        for (int f = 0; f < 30; f++) {
            const unsigned char *r; int l, id;
            if (!re15_discard_frozen()) scd_vm_tick();
            re15_msg_tick(&r, &l, &id);
            int32_t x0 = pl->x, z0 = pl->z;
            ctx.pad_current = RE15_PAD_BIT_UP;
            ctx.pad_pressed = (f == 0) ? RE15_PAD_BIT_UP : 0;
            re15_game_step(&ctx);
            long dx = (long)pl->x - x0, dz = (long)pl->z - z0;
            weg_wieder += (dx < 0 ? -dx : dx) + (dz < 0 ? -dz : dz);
        }
        /* ⛔ "DER RIEGEL GEHT WIEDER AUF" heisst NICHT "der Spieler laeuft immer
         * wieder": an manchen Stellen haelt ihn danach das SKRIPT (Szenen-Fenster
         * flag(2,7)/flag(1,27) = Pad-Bit 0x01000000 @0x800304f4) oder eine
         * FOLGE-Nachricht (Belegt-Bit). Verlangt wird deshalb: entweder er laeuft, oder
         * es ist eine der beiden ANDEREN Schranken, die ihn haelt — nie die Abfrage. */
        int gehalten_von_szene = re15_cine_active();
        int gehalten_von_msg   = re15_pauseflags_belegt();
        PRUEFE(weg_wieder > 0 || gehalten_von_szene || gehalten_von_msg,
               "ROOM%04X msg %2u: nach der Antwort laeuft der Spieler nicht, und weder"
               " Szene noch Nachricht halten ihn", room, (unsigned)msg);
        if (weg_wieder > 0) wieder_frei++;
        scd_register_current_rdt(NULL);

        /* ---------- RUECKBAU: das EINE Bruecken-Bild ohne die Bruecke -------------- */
        /* ⛔ DAS IST DER RUECKBAU ZUR ZAHL AUS TEIL I. Dort steht: ohne
         * re15_discard_frozen() waere an jeder Stelle GENAU EIN Bild offen — das
         * Bild, in dem der Nachrichten-Freeze schon geloest und die Abfrage noch nicht
         * aufgemacht ist (RE2 hat es nicht, @0x80051844 friert im selben Aufruf wieder
         * ein). Was dieses eine Bild WERT ist, wird hier in Einheiten gemessen: derselbe
         * Raum, derselbe Weg, gehaltenes VORWAERTS, aber im Lauf OHNE Vormerkung —
         * dort ist der Spieler in genau diesem Bild frei. Eine Zahl > 0 belegt, dass die
         * Bruecke etwas traegt; die 0 in der Spalte JETZT waere sonst wertlos. */
        long weg_rueck = j_weg_ueberhang(&rdt, room, sub,
                                         sp.b_msgende, sp.b_msgende + 1, 1);
        long rueck_stub = j_weg_ueberhang(&rdt, room, sub,
                                          sp.b_msgende, sp.b_msgende + 1, 0);
        PRUEFE(rueck_stub == 0, "ROOM%04X: der RUECKBAU-STUB (ohne re15_game_step) bewegt"
               " den Spieler um %ld", room, rueck_stub);
        if (weg_rueck > 0) { rueck_summe += weg_rueck; rueck_stellen++; }

        /* ---------- GEGENPROBE + STUB ueber die Ueberhang-Spanne ------------------ */
        long weg_gegen = -1, weg_stub = -1;
        if (sp.b_alt >= 0) {
            weg_gegen = j_weg_ueberhang(&rdt, room, sub, sp.b_msgende, sp.b_alt, 1);
            weg_stub  = j_weg_ueberhang(&rdt, room, sub, sp.b_msgende, sp.b_alt, 0);
            PRUEFE(weg_stub == 0, "ROOM%04X: der STUB-Lauf (ohne re15_game_step) bewegt den"
                   " Spieler um %ld — dann misst der Aufbau sich selbst", room, weg_stub);
            if (weg_gegen > 0) mit_bewegung++;
            if (weg_gegen > 0) gegen_summe += weg_gegen;
            if (weg_stub  > 0) stub_summe  += weg_stub;
        } else {
            ohne_ueberhang++;
            rueck_ohne_span += (weg_rueck > 0) ? weg_rueck : 0;
        }

        char nam[24];
        snprintf(nam, sizeof nam, "ROOM%04X sub%02d m%u", room, sub, (unsigned)msg);
        if (sp.b_alt >= 0)
            printf("  %-22s %8ld %10ld %12ld %8ld %8ld\n",
                   nam, weg_jetzt, weg_rueck, weg_gegen, weg_stub, weg_wieder);
        else
            printf("  %-22s %8ld %10ld %12s %8s %8ld   (UEBERHANG AUSGELASSEN:"
                   " Faden endet nicht, Warteschleife Ck(5,33,0) @0x26FC)\n",
                   nam, weg_jetzt, weg_rueck, "-", "-", weg_wieder);
        jetzt_summe += weg_jetzt;
        gefahren++;
        free(raw);
    }
    printf("  ABDECKUNG: %d Stellen mit echtem Tuer-Spawn und echtem re15_game_step,\n"
           "             davon %d ohne UEBERHANG-Spanne (mit Grund ausgelassen)\n",
           gefahren, ohne_ueberhang);
    printf("  RUECKBAU des SCHLIESS-Bildes: %ld Einheiten an %d von %d Stellen — so viel\n"
           "  Weg traegt die eine Zeile re15_discard_frozen() je Stelle in EINEM Bild.\n"
           "  Davon %ld Einheiten an den %d Stellen MIT Ueberhang-Spanne (die Zahl, die ohne\n"
           "  das jetzt ausgewiesene ROOM1090 herauskommt) und %ld allein in ROOM1090.\n",
           rueck_summe, rueck_stellen, gefahren,
           rueck_summe - rueck_ohne_span, gefahren - ohne_ueberhang, rueck_ohne_span);
    printf("  Summe JETZT %ld Einheiten / GEGENPROBE %ld / STUB %ld;\n"
           "  in %d Stellen bewegte sich der Spieler im Ueberhang des Runde-22-Modells,\n"
           "  in %d Stellen laeuft er nach der Antwort sofort wieder (sonst haelt ihn\n"
           "  Szene oder Folge-Nachricht, je Stelle geprueft).\n",
           jetzt_summe, gegen_summe, stub_summe, mit_bewegung, wieder_frei);
    PRUEFE(gefahren > 0, "TEIL J hat keine einzige Stelle gefahren");
    PRUEFE(jetzt_summe == 0, "TEIL J: %ld Einheiten Bewegung im gesperrten Fenster",
           jetzt_summe);
    /* ⛔ DIE GEGENPROBE IST DER KERN: ohne sie waere die 0 oben wertlos. */
    PRUEFE(mit_bewegung > 0,
           "GEGENPROBE: in KEINER Stelle bewegte sich der Spieler im Ueberhang — dann"
           " kann dieser Riegel eine Bewegung gar nicht sehen");
    PRUEFE(gegen_summe > stub_summe,
           "GEGENPROBE: die Gegenprobe (%ld) liegt nicht ueber dem Stub (%ld)",
           gegen_summe, stub_summe);
    PRUEFE(wieder_frei > 0,
           "in KEINER Stelle laeuft der Spieler nach der Antwort wieder — dann ist nicht"
           " gezeigt, dass der Riegel wieder aufgeht");
    /* ⛔ DER RUECKBAU MUSS ETWAS WERT SEIN. Waere er 0, haette das eine
     * Bruecken-Bild aus TEIL I keine Wirkung, und die Zeile re15_discard_frozen()
     * waere Zierde. */
    PRUEFE(rueck_summe > 0,
           "RUECKBAU: das Bruecken-Bild ist 0 Einheiten wert — dann traegt"
           " re15_discard_frozen() nichts");
}

/* =========================================================================
 * TEIL L — DAS BESITZ-GATE, AN DEN AUSGELIEFERTEN UNTERPROGRAMMEN.
 *
 * Gefragt wird: erscheint die Wegwerf-Abfrage je Benutzungsstelle auch dann, wenn der
 * Spieler den Gegenstand GAR NICHT traegt? Gefahren wird NICHT die 6-Byte-Ersatzfolge aus
 * TEIL A, sondern das AUSGELIEFERTE Unterprogramm des Raums, mit einem Spieler, der jede
 * Ja/Nein-Frage des Skripts mit JA beantwortet (virtuelles Bestaetigen-Bit 0x4000). Denn
 * genau so kommt man an die Stelle: ROOM4000 sub02 fragt zuerst "Will you use the Blue
 * Master Keycard?" (Message_on 1 @0x01440), latcht die Antwort nach flag(12,31) und gibt
 * erst dahinter "You've used the Blue Master Keycard." aus:
 *     0x0142A  Ck  21 03 20 00     ; Ck(3,32,0)   Tuer noch verschlossen
 *     0x01440  Message_on 1        ; "Will you use the Blue Master Keycard?"  (0x03)
 *     0x01446  Ck  21 0c 1f 00     ; Ck(12,31,0)  Antwort JA
 *     0x0144A  Message_on 2        ; "You've used the Blue Master Keycard."  <== die Stelle
 *     0x01450  Set 22 03 20 01     ; Set(3,32,1)  Tuer ab jetzt benutzt
 *
 * DIE REGEL, GEGEN DIE GEPRUEFT WIRD, IST RE2s TUER-HANDLER:
 *     80051628  jal   FUN_800696cc      ; Inventarplatz suchen
 *     80051630  move  s1,v0
 *     80051634  bltz  s1,LAB_800516a0   ; kein Treffer -> anderer Zweig
 *     8005164C  jal   FUN_8002fe38      ; ERST DANN die Nachricht
 *     80051670  sw    LAB_80051718,...  ; und DANACH die Fortsetzung einhaengen
 *     800516B4/B8/C0 : der Nicht-Treffer-Zweig oeffnet eine andere Nachricht und
 *                      haengt NICHTS ein — also nie eine Abfrage.
 *
 * ZWEI LAEUFE JE STELLE, EIN EINZIGER UNTERSCHIED (der Besitz):
 *   OHNE  — Inventar leer.       VERLANGT: kein Prompt, kein Vorentscheid mit Treffer.
 *   MIT   — Gegenstand, Anzahl 1. VERLANGT: Prompt.
 * Der MIT-Lauf IST die Gegenprobe: ohne ihn stuende dieser Riegel auch dann gruen, wenn
 * die Abfrage nie kommt (etwa weil der Pruefstand die Stelle nicht erreicht).
 * Zusaetzlich wird der Zaehler des Gates selbst ausgewertet
 * (re15_discard_vorentscheide / _mit_besitz): so ist belegt, dass das Gate GEFRAGT wurde
 * und nicht bloss nichts passierte.
 * ========================================================================= */
static int l_lauf(re15_rdt_t *rdt, unsigned room, uint8_t msg, int sub,
                  int mit_gegenstand, uint8_t item, int *out_erreicht)
{
    extern uint16_t g_scd_pad_edge, g_scd_pad_held;
    grundzustand();
    g_current_room_id = room;
    re15_msg_load_room_block(rdt->messages, rdt->messages_size);
    scd_register_current_rdt(rdt);
    if (mit_gegenstand) re15_inv_grant(item, 1);
    scd_thread_start(0, rdt->sub_scd[sub]);
    int erreicht = 0, abfrage = 0;
    for (long fr = 0; fr < 6000; fr++) {
        const unsigned char *r; int l, id;
        g_scd_pad_edge = (fr > 2 && (fr % 4) == 0) ? 0x4000u : 0u;
        g_scd_pad_held = 0;
        if (!re15_discard_frozen()) scd_vm_tick();
        re15_msg_tick(&r, &l, &id);
        g_scd_pad_edge = 0;
        if (re15_pauseflags_belegt() && g_scd.message_id == msg) erreicht = 1;
        if (re15_discard_active()) { abfrage = 1; re15_discard_tick(0, 0); }
        if (re15_discard_prompt(NULL, NULL)) break;
    }
    scd_register_current_rdt(NULL);
    if (out_erreicht) *out_erreicht = erreicht;
    return abfrage;
}

static void teil_l(void)
{
    printf("\n=== TEIL L: Besitz-Gate an den ausgelieferten Unterprogrammen ===\n");
    printf("  %-22s %6s | %-22s | %-14s\n",
           "Stelle", "item", "OHNE Besitz", "MIT Besitz");
    int gefahren = 0, unerreicht = 0, ohne_abfrage = 0, mit_abfrage = 0;
    int v0 = re15_discard_vorentscheide(), vb0 = re15_discard_vorentscheide_mit_besitz();
    int vz0 = re15_discard_vorentscheid_belegt();

    for (int i = 0; i < RE15_DISCARD_SITE_COUNT; i++) {
        unsigned room = re15_discard_sites[i].room;
        uint8_t  msg  = re15_discard_sites[i].msg;
        uint8_t  item = re15_discard_sites[i].item;
        re15_rdt_t rdt; size_t n = 0;
        uint8_t *raw = raum_laden(room, &rdt, &n);
        if (!raw) { printf("  SKIP ROOM%04X (RDT fehlt)\n", room); continue; }
        int sub = sub_mit_nachricht(&rdt, msg);
        if (sub < 0) {
            printf("  AUSGELASSEN ROOM%04X msg %2u: kein Message_on %u in einer sub-Region\n",
                   room, (unsigned)msg, (unsigned)msg);
            free(raw); continue;
        }
        int e_ohne = 0, e_mit = 0;
        int a_ohne = l_lauf(&rdt, room, msg, sub, 0, item, &e_ohne);
        int a_mit  = l_lauf(&rdt, room, msg, sub, 1, item, &e_mit);
        char nam[24];
        snprintf(nam, sizeof nam, "ROOM%04X sub%02d m%u", room, sub, (unsigned)msg);
        printf("  %-22s   0x%02X | erreicht=%d abfrage=%-4s | abfrage=%-4s%s\n",
               nam, item, e_ohne, a_ohne ? "JA" : "nein", a_mit ? "JA" : "nein",
               e_ohne ? "" : "   (Stelle im Pruefstand nicht erreicht)");
        /* ⛔ HART: an einer ERREICHTEN Stelle darf ohne Besitz NICHTS kommen. */
        PRUEFE(!a_ohne, "%s: Abfrage OHNE Besitz — das Gate @0x80051634 greift nicht", nam);
        if (e_ohne) { gefahren++; if (a_ohne) ohne_abfrage++; } else unerreicht++;
        if (a_mit)  mit_abfrage++;
        /* Erreicht + Besitz muss die Abfrage bringen — sonst misst der OHNE-Lauf nichts. */
        if (e_mit) PRUEFE(a_mit, "%s: MIT Besitz kommt die Abfrage NICHT — dann ist die 0"
                          " im OHNE-Lauf wertlos", nam);
        free(raw);
    }
    int v = re15_discard_vorentscheide() - v0;
    int vb = re15_discard_vorentscheide_mit_besitz() - vb0;
    printf("  ABDECKUNG: %d von %d Benutzungsstellen im Pruefstand erreicht"
           " (%d nicht erreicht)\n", gefahren, RE15_DISCARD_SITE_COUNT, unerreicht);
    printf("  OHNE BESITZ eine Abfrage: %d von %d erreichten Stellen\n",
           ohne_abfrage, gefahren);
    printf("  MIT  BESITZ eine Abfrage: %d von %d Stellen (GEGENPROBE)\n",
           mit_abfrage, RE15_DISCARD_SITE_COUNT);
    printf("  Das Gate selbst: %d Vorentscheide, davon %d mit Besitz"
           " (@0x80051628 gesucht, @0x80051634 verzweigt)\n", v, vb);
    /* KETTENPOSITION, gemessen statt behauptet: RE2 entscheidet die Besitzfrage VOR
     * @0x8005164C, die Nachricht ist dann noch nicht offen. Jeder Vorentscheid, der das
     * Belegt-Bit schon gesetzt findet (@0x800517f0 `andi v0,v0,0x80`), saesse HINTER dem
     * Oeffnen. Vor Runde 24 war das der Fall: die Pruefung stand in
     * re15_discard_notice_message, also hinter re15_dialog_open_mask. */
    printf("  KETTENPOSITION: %d von %d Vorentscheiden fielen, waehrend das"
           " Nachrichtensystem\n  schon BELEGT war (verlangt 0 - RE2 entscheidet VOR"
           " @0x8005164C).\n", re15_discard_vorentscheid_belegt() - vz0, v);
    PRUEFE(re15_discard_vorentscheid_belegt() - vz0 == 0,
           "%d Vorentscheide sitzen HINTER dem Oeffnen der Nachricht - falsche Stelle"
           " der Kette", re15_discard_vorentscheid_belegt() - vz0);
    /* ⛔ ABDECKUNGS-SCHRANKE. Ohne sie zaehlt ein Lauf, der KEINE Stelle erreicht, als
     * Erfolg: ohne_abfrage waere 0, und "0 von 0" liest sich wie "0 von 10". Und die
     * Gleichheit mit_abfrage == gefahren faengt das NICHT — sie bleibt wahr, wenn beide
     * Zahlen zusammen fallen (Rueckbau R-D1: 1 von 16 erreicht, alter Riegel gruen).
     * Gemessen werden 10 von 16; die 6 anderen haengen an AOTs/Kartenlesern, die der
     * Pruefstand nicht betritt, und stehen mit Grund in der Tabelle oben. */
    PRUEFE(gefahren >= 10,
           "ABDECKUNG: nur %d von %d Stellen erreicht (erwartet >= 10) — dann messen die"
           " Nullen darunter nichts", gefahren, RE15_DISCARD_SITE_COUNT);
    PRUEFE(ohne_abfrage == 0, "%d erreichte Stellen fragen OHNE Besitz", ohne_abfrage);
    PRUEFE(mit_abfrage > 0, "GEGENPROBE: MIT Besitz kommt NIRGENDS eine Abfrage — dann"
           " kann dieser Riegel eine Abfrage gar nicht sehen");
    PRUEFE(mit_abfrage == gefahren,
           "GEGENPROBE: %d Stellen fragen mit Besitz, aber %d sind erreichbar",
           mit_abfrage, gefahren);
    /* ⛔ UND DER ZAEHLER: das Gate MUSS gefragt worden sein, und zwar OEFTER als es
     * Treffer gab — sonst waere "keine Abfrage ohne Besitz" nur die Abwesenheit von
     * Ereignissen und kein Beweis fuer eine wirkende Pruefung. */
    PRUEFE(v > vb && vb > 0,
           "Gate-Zaehler unplausibel: %d Vorentscheide, %d mit Besitz", v, vb);
}

/* =========================================================================
 * TEIL M — DER NACHHALL-UEBERLAPP: steht der Untertitel der ausloesenden Zeile
 *          noch, waehrend die Wegwerf-Abfrage schon offen ist?
 *
 * RE2 kennt den Zustand "Text steht, Freeze geloest" NICHT. Belegt an zwei Stellen:
 *   (a) das Belegt-Bit loeschen und den Pause-Schnappschuss zuruecklegen ist EIN Paar im
 *       selben Block — @0x800307e8 `andi v0,v0,0x7f` / @0x800307f4
 *       `sw v1,DAT_800cfbdc`, keine Bildgrenze dazwischen;
 *   (b) die Abfrage wird mit DERSELBEN Nachrichten-Routine geoeffnet wie die Zeile davor
 *       — @0x80051834 `jal FUN_8002fe38` (a1=0x100 @0x8005182C, a2=9 @0x80051830)
 *       gegen @0x8005164C `jal FUN_8002fe38` (a1=0x100, a2=5 @0x80051640). EIN Kanal:
 *       die neue Zeile legt sich auf die alte, die alte kann nicht daneben stehenbleiben.
 *
 * Der Port hat einen solchen Zustand: den UNTERTITEL-NACHHALL (msg-FSM Zustand 7,
 * ausdrueckliche Nutzer-Entscheidung 2026-09-20, msg_common.c). Er BLEIBT — er ist die
 * Nutzer-Entscheidung und haelt den Spieler nicht auf. Aber er darf die Abfrage nicht
 * ueberlappen; dafuer ruft re15_discard_tick beim Aufgehen re15_msg_nachhall_beenden().
 *
 * GEMESSEN WIRD MIT ERZWUNGENEM NACHHALL: der Pruefstand hat keine Tonausgabe, also
 * werden die beiden Plattform-Globalen der Aufnahme (audio_pc.c stempelt sie je Bild)
 * von Hand gesetzt. Zwei Zahlen im selben Lauf:
 *   UEBERLAPP  Bilder mit stehendem Text UND sichtbarer Abfrage. Verlangt: 0.
 *   NACHHALL   Bilder im Zustand 7 im Lauf OHNE Abfrage. Muss > 0 sein — sonst
 *              hat der Aufbau gar keinen Nachhall erzeugt und die 0 oben waere wertlos.
 * ========================================================================= */
/* Ein Lauf von TEIL M. `mit_besitz` ist der EINZIGE Unterschied zwischen den beiden
 * Laeufen: mit Besitz geht die Abfrage auf (und beendet den Nachhall), ohne Besitz
 * nicht — dann laeuft der Nachhall ungestoert und ist ZAEHLBAR. */
static int m_lauf(re15_rdt_t *rdt, unsigned room, uint8_t msg, int sub, uint8_t item,
                  int mit_besitz, long *out_prompt, long *out_nachhall, long *out_ueberlapp)
{
    extern uint16_t g_scd_pad_edge, g_scd_pad_held;
    extern int g_re15_voice_laeuft, g_re15_voice_restbilder;
    long ueberlapp = 0, nachhall_bilder = 0, prompt_bilder = 0, nach_zeile = 0;
    grundzustand();
    g_current_room_id = room;
    re15_msg_load_room_block(rdt->messages, rdt->messages_size);
    scd_register_current_rdt(rdt);
    if (mit_besitz) re15_inv_grant(item, 1);
    scd_thread_start(0, rdt->sub_scd[sub]);
    g_re15_voice_laeuft = 0; g_re15_voice_restbilder = 0;
    int scharf = 0;
    for (long fr = 0; fr < 4000; fr++) {
        const unsigned char *r; int l, id;
        g_scd_pad_edge = (fr > 2 && (fr % 4) == 0) ? 0x4000u : 0u;
        g_scd_pad_held = 0;
        if (!re15_discard_frozen()) scd_vm_tick();
        re15_msg_tick(&r, &l, &id);
        g_scd_pad_edge = 0;
        /* ⛔ DIE AUFNAHME WIRD ERST SCHARF, WENN DIE ZEILE SCHON STEHT.
         * Wuerde g_re15_voice_laeuft von Anfang an 1 sein, parkte der Riegel gegen die
         * Aufnahme in op_message_on (scd_vm.c, RE15_VOICE_NOTBREMSE = 300 Bilder) und
         * die Stelle waere nie erreicht — gemessen: 0 Bilder Prompt, also gar kein
         * Fall. Hier laeuft die Aufnahme ab dem Bild, in dem die Benutzungs-Nachricht
         * steht; ihr regulaeres Ende geht dann in den Nachhall
         * (msg_common.c: `if (re15_stimme_rest() > 0) fsm = 7`). */
        if (!scharf && re15_pauseflags_belegt() && g_scd.message_id == msg) {
            scharf = 1; g_re15_voice_laeuft = 1; g_re15_voice_restbilder = 600;
        }
        if (re15_discard_active()) re15_discard_tick(0, 0);
        int prompt = re15_discard_prompt(NULL, NULL) != 0;
        if (g_scd.message_fsm == 7 && g_scd.message_active) nachhall_bilder++;
        if (prompt) prompt_bilder++;
        if (prompt && g_scd.message_active) ueberlapp++;
        if (prompt && prompt_bilder > 120) break;
        /* Der Lauf OHNE Besitz hat keinen Prompt, der ihn beendet: er laeuft 200 Bilder
         * ueber das Ende der ausloesenden Zeile hinaus, damit der Nachhall gezaehlt
         * werden kann. */
        if (!mit_besitz && scharf && !re15_pauseflags_belegt() && ++nach_zeile > 200) break;
    }
    g_re15_voice_laeuft = 0; g_re15_voice_restbilder = 0;
    scd_register_current_rdt(NULL);
    if (out_prompt)    *out_prompt    = prompt_bilder;
    if (out_nachhall)  *out_nachhall  = nachhall_bilder;
    if (out_ueberlapp) *out_ueberlapp = ueberlapp;
    return scharf;
}

static void teil_m(void)
{
    printf("\n=== TEIL M: Nachhall-Ueberlapp mit der Abfrage ===\n");

    const unsigned room = 0x4000; const uint8_t msg = 2, item = 0x47;
    re15_rdt_t rdt; size_t n = 0;
    uint8_t *raw = raum_laden(room, &rdt, &n);
    if (!raw) { printf("  SKIP ROOM%04X\n", room); return; }
    int sub = sub_mit_nachricht(&rdt, msg);
    if (sub < 0) { printf("  SKIP: kein Message_on %u\n", (unsigned)msg); free(raw); return; }

    /* LAUF 1 — MIT Besitz: die Abfrage geht auf. Gemessen wird der UEBERLAPP. */
    long p1 = 0, nh1 = 0, ue1 = 0;
    int scharf1 = m_lauf(&rdt, room, msg, sub, item, 1, &p1, &nh1, &ue1);
    /* LAUF 2 — OHNE Besitz: KEINE Abfrage, also beendet auch nichts den Nachhall.
     * ⛔ DAS IST DIE GEGENPROBE, DIE BIS RUNDE 24 FEHLTE. Der Lauf mit Besitz meldet
     * "Nachhall-Bilder 0" — genau den Zustand, den der Kommentar dieses Teils selbst
     * wertlos nennt, denn eine 0 kann zwei Ursachen haben: der Nachhall wurde beendet
     * (das WILL der Riegel zeigen) ODER er ist im Aufbau nie entstanden (dann zeigt er
     * nichts). Erst dieser zweite Lauf trennt die beiden: derselbe Raum, dieselbe Zeile,
     * dieselbe erzwungene Aufnahme, EIN Unterschied (der Besitz). Hier MUSS der Nachhall
     * entstehen. */
    long p2 = 0, nh2 = 0, ue2 = 0;
    int scharf2 = m_lauf(&rdt, room, msg, sub, item, 0, &p2, &nh2, &ue2);
    (void)ue2;

    printf("  LAUF 1 MIT  Besitz: Prompt-Bilder %ld, Nachhall-Bilder %ld, UEBERLAPP %ld\n",
           p1, nh1, ue1);
    printf("  LAUF 2 OHNE Besitz: Prompt-Bilder %ld, Nachhall-Bilder %ld"
           "  (GEGENPROBE: hier MUSS Nachhall entstehen)\n", p2, nh2);
    PRUEFE(p1 > 0,
           "GEGENPROBE: die Abfrage geht in diesem Aufbau gar nicht auf — dann prueft"
           " der Ueberlapp nichts");
    PRUEFE(scharf1, "GEGENPROBE: die Benutzungs-Nachricht stand im Lauf 1 nie");
    PRUEFE(scharf2, "GEGENPROBE: die Benutzungs-Nachricht stand im Lauf 2 nie");
    PRUEFE(p2 == 0, "LAUF 2: %ld Prompt-Bilder OHNE Besitz — dann ist der Lauf kein"
           " Vergleichsfall", p2);
    PRUEFE(nh2 > 0,
           "GEGENPROBE: der Aufbau erzeugt GAR KEINEN Nachhall (%ld Bilder im Lauf ohne"
           " Abfrage) — dann ist die 0 aus Lauf 1 wertlos", nh2);
    PRUEFE(ue1 == 0,
           "%ld Bilder Nachhall-Ueberlapp — RE2 hat diesen Zustand nicht"
           " (@0x800307e8/@0x800307f4 sind ein Paar, @0x80051834 = @0x8005164C)",
           ue1);
    free(raw);
}

/* =========================================================================
 * TEIL N — DIE ZWEI FAELLE VON ROOM4000 UND ROOM1090, GEFAHREN.
 *
 * ROOM4000 sub02 ist der Fall, an dem die Runde-23-Beurteilung "ein Ja kann den
 * Spielstand toeten" haengt. Das ausgelieferte Unterprogramm (Datei-Offsets, mit dem
 * Laengen-Vorschub des Motors ausgelesen):
 *     0x01426  Ifel_ck      06 00 2c 00
 *     0x0142A  Ck           21 03 20 00     ; Ck(3,32,0)  Tuer noch unbenutzt
 *     0x0142E  Se_on        36 02 0f 00 ...
 *     0x0143A  Message_on   2b 00 ff ff     ; "The door is locked ... An ID card is required"
 *     0x01440  Message_on   2b 01 ff ff     ; "Will you use the Blue Master Keycard?"  (0x03)
 *     0x01446  Ck           21 0c 1f 00     ; Ck(12,31,0)  Antwort JA
 *     0x0144A  Message_on   2b 02 ff ff     ; "You've used the Blue Master Keycard."  <== Stelle
 *     0x01450  Set          22 03 20 01     ; Set(3,32,1)  Tuer ab jetzt benutzt
 *     0x0145A  Ck           21 03 20 01     ; und der Zweig dahinter oeffnet sie
 *
 * ⛔ ZWEI BERICHTIGUNGEN ZUR BEURTEILUNG, beide am Byte gemessen:
 *   (1) `Ck(3,32,0)` ist NICHT das einzige Tor. Zwischen der Frage und der
 *       Benutzungs-Nachricht steht `Ck(12,31,0)` @0x01446 — die JA-ANTWORT des
 *       Spielers auf Message_on 1. Und `Set(3,32,1)` @0x01450 liegt HINTER diesem Ck,
 *       faellt also nur bei JA (op_ck gibt bei falsch SCD_R_IF_FALSE zurueck und der
 *       Dispatcher springt auf das block_end des umgebenden Ifel_ck, scd_vm.c:1992-2011).
 *   (2) Die Forderung "ohne Besitz faellt Set(3,32,1) nicht" ist im Auslieferungsstand
 *       NICHT erfuellbar, ohne dem Skript eine Besitzpruefung anzudichten: sub02 hat
 *       keine. Und das ist KEIN Versehen — der Blue Master Keycard 0x47 wird im ganzen
 *       Spiel genau EINMAL ausgegeben, und zwar in ROOM4010, also HINTER dieser Tuer
 *       (Item_aot_set 0x50, tools/discard_verlustwege.py). Wer die Tuer ohne Karte nicht
 *       oeffnen koennte, kaeme nie an die Karte. Der Port darf das also nicht "reparieren".
 *   Was der Port verhindern MUSS und hier verhindert: die WEGWERF-ABFRAGE ohne Besitz.
 *   Ein Ja kann damit nichts wegwerfen, was der Spieler nicht hat.
 *
 * GEFAHREN werden je Raum zwei Faelle im ECHTEN VM:
 *   (a) OHNE Besitz: keine Abfrage. Das Tuer-Flag des Skripts wird MITGEMESSEN und
 *       ausgewiesen — nicht behauptet.
 *   (b) MIT Besitz: die Abfrage kommt, zeigt GENAU diesen Gegenstand, und ein Ja nimmt
 *       ihn aus dem Inventar.
 * ========================================================================= */
static void teil_n_fall(unsigned room, uint8_t msg, uint8_t item,
                        uint8_t fz, uint8_t fi, const char *fname)
{
    extern uint16_t g_scd_pad_edge, g_scd_pad_held;
    re15_rdt_t rdt; size_t n = 0;
    uint8_t *raw = raum_laden(room, &rdt, &n);
    if (!raw) { printf("  SKIP ROOM%04X\n", room); return; }
    int sub = sub_mit_nachricht(&rdt, msg);
    if (sub < 0) { printf("  SKIP ROOM%04X: kein Message_on %u\n", room, (unsigned)msg);
                   free(raw); return; }

    for (int mit = 0; mit < 2; mit++) {
        grundzustand();
        g_current_room_id = room;
        re15_msg_load_room_block(rdt.messages, rdt.messages_size);
        scd_register_current_rdt(&rdt);
        if (mit) re15_inv_grant(item, 1);
        int f_vor = re15_game_flag_get(fz, fi);
        scd_thread_start(0, rdt.sub_scd[sub]);
        int erreicht = 0;
        for (long fr = 0; fr < 6000; fr++) {
            const unsigned char *r; int l, id;
            g_scd_pad_edge = (fr > 2 && (fr % 4) == 0) ? 0x4000u : 0u;
            g_scd_pad_held = 0;
            if (!re15_discard_frozen()) scd_vm_tick();
            re15_msg_tick(&r, &l, &id);
            g_scd_pad_edge = 0;
            if (re15_pauseflags_belegt() && g_scd.message_id == msg) erreicht = 1;
            if (re15_discard_active()) re15_discard_tick(0, 0);
            if (re15_discard_prompt(NULL, NULL)) break;
        }
        uint8_t pitem = 0; int pchoice = -1;
        int prompt = re15_discard_prompt(&pitem, &pchoice);
        int f_nach = re15_game_flag_get(fz, fi);
        if (!mit) {
            printf("  (a) ROOM%04X sub%02d m%-2u OHNE Besitz : Stelle erreicht=%d,"
                   " Abfrage=%s, %s %d->%d\n",
                   room, sub, (unsigned)msg, erreicht, prompt ? "JA" : "nein",
                   fname, f_vor, f_nach);
            /* ⛔ ABDECKUNGS-SCHRANKE: "keine Abfrage" ist nur dann ein Befund, wenn die
             * Stelle ueberhaupt erreicht wurde. Ohne diese Zeile stuende (a) auch dann
             * gruen, wenn das Unterprogramm gar nicht bis zur Benutzungs-Nachricht kam
             * (Rueckbau R-D2). */
            PRUEFE(erreicht, "ROOM%04X: die Benutzungsstelle wurde im OHNE-Lauf gar nicht"
                   " erreicht — dann prueft (a) nichts", room);
            PRUEFE(!prompt, "ROOM%04X: Abfrage OHNE Besitz", room);
            PRUEFE(re15_inv_find_item(item) < 0,
                   "ROOM%04X: 0x%02X liegt im Inventar, obwohl nichts gewaehrt wurde",
                   room, item);
        } else {
            printf("  (b) ROOM%04X sub%02d m%-2u MIT  Besitz : Abfrage=%s auf 0x%02X"
                   " (Auswahl %d), %s %d->%d",
                   room, sub, (unsigned)msg, prompt ? "JA" : "nein", pitem, pchoice,
                   fname, f_vor, f_nach);
            PRUEFE(prompt == 8, "ROOM%04X: MIT Besitz keine Abfrage", room);
            PRUEFE(pitem == item, "ROOM%04X: Abfrage zeigt 0x%02X statt 0x%02X",
                   room, pitem, item);
            if (prompt) {
                antworten(0x4000);                     /* SQUARE = Ja, Auswahl steht auf Yes */
                printf(", Ja -> Gegenstand %s\n",
                       re15_inv_find_item(item) < 0 ? "WEG" : "NOCH DA");
                PRUEFE(re15_inv_find_item(item) < 0,
                       "ROOM%04X: 0x%02X liegt nach JA immer noch im Inventar", room, item);
            } else {
                printf("\n");
            }
        }
        scd_register_current_rdt(NULL);
    }
    free(raw);
}

static void teil_n(void)
{
    printf("\n=== TEIL N: ROOM4000 und ROOM1090, beide Faelle im echten VM ===\n");
    /* ROOM4000: das Tuer-Flag ist flag(3,32) (`Set(3,32,1)` @0x01450). */
    teil_n_fall(0x4000, 2, 0x47, 3, 32, "flag(3,32)");
    /* ROOM1090: das Tor von sub03 ist flag(3,132) (`Ck(3,132,1)` @0x022A6 im Aufrufer
     * sub00, von sub03 selbst geloescht: `Set(3,132,0)` @0x024CE). */
    teil_n_fall(0x1090, 9, 0x31, 3, 132, "flag(3,132)");
    printf("  Die Tuer-/Tor-Flags des SKRIPTS werden ausgewiesen, nicht verlangt: sub02 bzw.\n"
           "  sub03 haben keine Besitzpruefung, und die Karte 0x47 liegt HINTER ihrer Tuer\n"
           "  (ROOM4010). Verlangt wird nur, was der Port verantwortet: keine Abfrage ohne\n"
           "  Besitz, und mit Besitz die richtige.\n");
}

/* =========================================================================
 * TEIL K — der SICHTBARE Prompt friert das Skript ein, und es laeuft NACH der
 *          Antwort WEITER.
 *
 * Das ist byte-true: RE2 legt beim Fragen sofort wieder die ganze Maske an
 *     80051844  lui v1,0xff00
 *     80051850  sw  v0,DAT_800cfbdc
 * und 0xFF000000 enthaelt das Skript-Bit 0x02000000, dessen Gate am Kopf des
 * SCD-Runners sitzt (@0x8003f044 `lui v1,0x200` / @0x8003f04c).
 *
 * ⛔ UND ES IST DER FALL, AN DEM RUNDE 21 SICH GEIRRT HAT. Dort wurde
 * gemessen: "die Abfrage erwischte das Bild zwischen Nachricht und Szene, und
 * weil ein sichtbarer Prompt den SCD-Takt anhaelt, kam die Szene danach gar
 * nicht mehr in Gang — sub03 stand auf @0x2508 fest". Das war ein Befund des
 * PRUEFSTANDS, nicht des Spiels: dort wurde die Frage NIE beantwortet (getickt
 * wurde mit pad = 0). Hier wird sie beantwortet, und das Unterprogramm laeuft
 * weiter — gemessen am Programmzeiger und am Szenen-Fenster @0x2508.
 * ========================================================================= */
static void teil_k(void)
{
    extern uint16_t g_scd_pad_edge, g_scd_pad_held;
    printf("\n=== TEIL K: sichtbarer Prompt friert das Skript ein — und es laeuft weiter ===\n");
    const unsigned room = 0x1090; const uint8_t msg = 9, item = 0x31;
    re15_rdt_t rdt; size_t n = 0;
    uint8_t *raw = raum_laden(room, &rdt, &n);
    if (!raw) { printf("  SKIP ROOM%04X\n", room); return; }
    int sub = sub_mit_nachricht(&rdt, msg);
    PRUEFE(sub >= 0, "ROOM%04X: Message_on %u in keiner sub-Region", room, (unsigned)msg);
    if (sub < 0) { free(raw); return; }

    grundzustand();
    g_current_room_id = room;
    re15_msg_load_room_block(rdt.messages, rdt.messages_size);
    scd_register_current_rdt(&rdt);
    re15_inv_grant(item, 1);
    scd_thread_start(0, rdt.sub_scd[sub]);

    long b_frage = -1;
    for (long fr = 0; fr < 20000; fr++) {
        const unsigned char *r; int l, id;
        g_scd_pad_edge = (fr > 2 && (fr % 4) == 0) ? 0x4000u : 0u;
        g_scd_pad_held = 0;
        if (!re15_discard_frozen()) scd_vm_tick();
        re15_msg_tick(&r, &l, &id);
        g_scd_pad_edge = 0;
        if (re15_discard_active()) re15_discard_tick(0, 0);
        if (re15_discard_prompt(NULL, NULL)) { b_frage = fr; break; }
    }
    PRUEFE(b_frage >= 0, "ROOM%04X: die Abfrage geht nicht auf", room);
    long pc_bei_frage = g_scd.threads[0].active
                      ? (long)(g_scd.threads[0].pc - raw) : -1;
    PRUEFE(!re15_cine_active(),
           "ROOM%04X: das Szenen-Fenster @0x2508 steht schon, bevor die Abfrage aufgeht",
           room);

    /* EINGEFROREN: 200 Bilder ohne Antwort — der Programmzeiger darf sich NICHT ruehren. */
    for (int f = 0; f < 200; f++) {
        const unsigned char *r; int l, id;
        if (!re15_discard_frozen()) scd_vm_tick();
        re15_msg_tick(&r, &l, &id);
    }
    long pc_eingefroren = g_scd.threads[0].active
                        ? (long)(g_scd.threads[0].pc - raw) : -1;
    PRUEFE(pc_eingefroren == pc_bei_frage,
           "ROOM%04X: der Faden laeuft unter der stehenden Abfrage weiter"
           " (0x%04lX -> 0x%04lX)", room, pc_bei_frage, pc_eingefroren);

    /* ANTWORTEN — und danach MUSS das Unterprogramm weiterlaufen. */
    for (long f = 0; f < 4000 && !re15_discard_ready(); f++) re15_discard_tick(0, 0);
    re15_discard_tick(0x4000, 0);
    PRUEFE(!re15_discard_active(), "ROOM%04X: Abfrage nach JA nicht beendet", room);
    int szene = 0; long pc_danach = pc_bei_frage;
    for (int f = 0; f < 120; f++) {
        const unsigned char *r; int l, id;
        if (!re15_discard_frozen()) scd_vm_tick();
        re15_msg_tick(&r, &l, &id);
        if (re15_cine_active()) szene = 1;
        if (g_scd.threads[0].active) pc_danach = (long)(g_scd.threads[0].pc - raw);
    }
    PRUEFE(pc_danach != pc_bei_frage,
           "ROOM%04X: das Unterprogramm laeuft nach der Antwort NICHT weiter"
           " (steht weiter auf 0x%04lX)", room, pc_bei_frage);
    PRUEFE(szene, "ROOM%04X: das Szenen-Fenster @0x2508 geht nach der Antwort nicht auf",
           room);
    printf("  ROOM%04X sub%02d msg %u: Abfrage in Bild %ld, Faden parkt 200 Bilder auf"
           " 0x%04lX,\n           nach der Antwort laeuft er bis 0x%04lX und das"
           " Szenen-Fenster @0x2508 geht auf.\n",
           room, sub, (unsigned)msg, b_frage, pc_bei_frage, pc_danach);
    scd_register_current_rdt(NULL);
    free(raw);
}

/* =========================================================================
 * TEIL O — DIE FAIL-CLOSED-KOPPLUNG, gefahren statt behauptet.
 *
 * Die Hauptbehauptung der Runde 24 lautet: re15_discard_notice_message merkt NUR vor,
 * wenn fuer GENAU DIESELBE (Raum, Nachricht) vorher ein Vorentscheid MIT TREFFER
 * gefallen ist. Bis Runde 25 stand diese Behauptung in KEINEM Riegel — sie war
 * vollstaendig zurueckbaubar, ohne dass etwas rot wurde (Rueckbau R6 des Skeptikers:
 * exit 0). Genau das schliesst dieser Teil.
 *
 * DAS VORBILD IST RE2s NICHT-TREFFER-ZWEIG. Dort wird die Fortsetzung NUR im
 * Treffer-Zweig eingehaengt:
 *     80051628  jal   FUN_800696cc      ; Platz suchen
 *     80051634  bltz  s1,LAB_800516a0   ; kein Treffer -> anderer Zweig
 *     80051670  sw    v0=>LAB_80051718,-0x7d50(at)   ; NUR hier eingehaengt
 *   Nicht-Treffer-Zweig:
 *     800516B4  addiu a2,s0,-0x4c       ; andere Nachrichten-Id
 *     800516B8  jal   FUN_8002fe38      ; andere Zeile oeffnen
 *     800516C0  j     LAB_800516f8      ; return — NICHTS eingehaengt
 * Ein Oeffnungsweg, der an @0x80051628 vorbeikommt, kann in RE2 also keine Abfrage
 * armieren. Im Port heisst das: ohne gueltigen Vorentscheid passiert nichts.
 *
 * SECHS FAELLE JE STELLE, alle ueber die Kopplung selbst (keine RDT noetig — genau das
 * ist der Punkt: ein FREMDER Oeffnungsweg ruft eben nicht op_message_on):
 *   O1 DIREKT        Gegenstand da, notice() OHNE jeden Vorentscheid      -> nichts
 *   O2 FREMDE STELLE Vorentscheid mit Treffer fuer eine ANDERE Stelle     -> nichts
 *   O3 FREMDE MSG    Vorentscheid fuer (Raum, andere Nachricht)           -> nichts
 *   O4 OHNE BESITZ   Vorentscheid fuer die richtige Stelle, Inventar leer -> nichts
 *   O5 POSITIV       Vorentscheid mit Treffer + notice() derselben Stelle -> VORGEMERKT
 *   O6 VERBRAUCHT    nach O5 ein zweites notice() ohne neuen Vorentscheid -> nichts
 * O5 ist die Gegenprobe: ohne ihn stuende dieser Riegel auch dann gruen, wenn
 * re15_discard_notice_message NIE etwas vormerkt (also das Merkmal ganz ausgebaut ist).
 * ========================================================================= */
static void teil_o(void)
{
    printf("\n=== TEIL O: fail-closed — ohne Vorentscheid wird NICHTS vorgemerkt ===\n");
    int o1 = 0, o2 = 0, o3 = 0, o4 = 0, o5 = 0, o6 = 0, gefahren = 0;

    for (int i = 0; i < RE15_DISCARD_SITE_COUNT; i++) {
        unsigned room = re15_discard_sites[i].room;
        uint8_t  msg  = re15_discard_sites[i].msg;
        uint8_t  item = re15_discard_sites[i].item;

        /* Eine ANDERE Benutzungsstelle (fuer O2) und eine Nachrichten-Id desselben
         * Raums, die KEINE Benutzungsstelle ist (fuer O3). */
        int j = -1;
        for (int k = 1; k < RE15_DISCARD_SITE_COUNT; k++) {
            int c = (i + k) % RE15_DISCARD_SITE_COUNT;
            if (re15_discard_sites[c].room != room || re15_discard_sites[c].msg != msg) {
                j = c; break;
            }
        }
        uint8_t fremd_msg = 0xFF;
        for (int k = 0; k < 64; k++) {
            int belegt = 0;
            for (int c = 0; c < RE15_DISCARD_SITE_COUNT; c++)
                if (re15_discard_sites[c].room == room
                    && re15_discard_sites[c].msg == (uint8_t)k) belegt = 1;
            if (!belegt) { fremd_msg = (uint8_t)k; break; }
        }

        /* --- O1: der Direktaufruf. KEIN Vorentscheid, Gegenstand liegt im Inventar. --- */
        grundzustand();
        re15_inv_grant(item, 1);
        re15_discard_notice_message(room, msg);
        PRUEFE(re15_discard_pending_item() == 0 && !re15_discard_active(),
               "O1 ROOM%04X msg %u: der DIREKTAUFRUFER hat vorgemerkt (0x%02X) —"
               " die fail-closed-Kopplung greift nicht (Vorbild @0x800516C0)",
               room, (unsigned)msg, re15_discard_pending_item());
        if (re15_discard_pending_item() == 0 && !re15_discard_active()) o1++;
        { int slot = re15_inv_find_item(item);
          PRUEFE(slot >= 0 && g_inv.slots[slot].qty == 1,
                 "O1 ROOM%04X: der Direktaufruf hat die Anzahl angefasst", room); }

        /* --- O2: Vorentscheid mit Treffer, aber fuer eine ANDERE Stelle. ------------ */
        if (j >= 0) {
            grundzustand();
            re15_inv_grant(item, 1);
            if (re15_discard_sites[j].item != item)
                re15_inv_grant(re15_discard_sites[j].item, 1);
            int v = re15_discard_besitz_vor_nachricht(re15_discard_sites[j].room,
                                                      re15_discard_sites[j].msg);
            PRUEFE(v == 1, "O2 ROOM%04X: der Vorentscheid der FREMDEN Stelle ROOM%04X"
                   " msg %u traf nicht — dann ist O2 kein Fall", room,
                   re15_discard_sites[j].room, (unsigned)re15_discard_sites[j].msg);
            re15_discard_notice_message(room, msg);
            PRUEFE(re15_discard_pending_item() == 0 && !re15_discard_active(),
                   "O2 ROOM%04X msg %u: ein Vorentscheid fuer ROOM%04X msg %u hat DIESE"
                   " Stelle armiert", room, (unsigned)msg,
                   re15_discard_sites[j].room, (unsigned)re15_discard_sites[j].msg);
            if (v == 1 && re15_discard_pending_item() == 0 && !re15_discard_active()) o2++;
        }

        /* --- O3: Vorentscheid fuer (derselbe Raum, FREMDE Nachricht). --------------- */
        if (fremd_msg != 0xFF) {
            grundzustand();
            re15_inv_grant(item, 1);
            (void)re15_discard_besitz_vor_nachricht(room, fremd_msg);
            re15_discard_notice_message(room, msg);
            PRUEFE(re15_discard_pending_item() == 0 && !re15_discard_active(),
                   "O3 ROOM%04X: ein Vorentscheid fuer msg %u hat msg %u armiert",
                   room, (unsigned)fremd_msg, (unsigned)msg);
            if (re15_discard_pending_item() == 0 && !re15_discard_active()) o3++;
        }

        /* --- O4: richtige Stelle, aber der Gegenstand fehlt (@0x80051634). ---------- */
        grundzustand();
        int v4 = re15_discard_besitz_vor_nachricht(room, msg);
        PRUEFE(v4 == 0, "O4 ROOM%04X msg %u: der Vorentscheid traf OHNE Gegenstand",
               room, (unsigned)msg);
        re15_discard_notice_message(room, msg);
        PRUEFE(re15_discard_pending_item() == 0 && !re15_discard_active(),
               "O4 ROOM%04X msg %u: vorgemerkt ohne Besitz", room, (unsigned)msg);
        if (v4 == 0 && re15_discard_pending_item() == 0 && !re15_discard_active()) o4++;

        /* --- O5: DER POSITIVFALL. Ohne ihn ist der ganze Teil durch Nichtstun gruen. */
        grundzustand();
        re15_inv_grant(item, 1);
        int v5 = re15_discard_besitz_vor_nachricht(room, msg);
        PRUEFE(v5 == 1, "O5 ROOM%04X msg %u: der Vorentscheid traf MIT Gegenstand nicht",
               room, (unsigned)msg);
        re15_discard_notice_message(room, msg);
        PRUEFE(re15_discard_pending_item() == item && re15_discard_active(),
               "O5 ROOM%04X msg %u: MIT Vorentscheid wurde NICHT vorgemerkt"
               " (pending 0x%02X, erwartet 0x%02X) — dann messen O1..O4 nichts",
               room, (unsigned)msg, re15_discard_pending_item(), item);
        if (v5 == 1 && re15_discard_pending_item() == item && re15_discard_active()) o5++;
        /* Und die Anzahl darf beim Einhaengen NICHT gefallen sein (@0x800517f4 verlaesst
         * die Routine vor dem Dekrement @0x80051810). */
        { int slot = re15_inv_find_item(item);
          PRUEFE(slot >= 0 && g_inv.slots[slot].qty == 1,
                 "O5 ROOM%04X: die Anzahl fiel schon beim Einhaengen", room); }

        /* --- O6: der Vorentscheid ist VERBRAUCHT. Zweites notice() ohne neuen. ------ */
        re15_discard_reset();
        re15_discard_notice_message(room, msg);
        PRUEFE(re15_discard_pending_item() == 0 && !re15_discard_active(),
               "O6 ROOM%04X msg %u: der verbrauchte Vorentscheid hat ein zweites Mal"
               " armiert", room, (unsigned)msg);
        if (re15_discard_pending_item() == 0 && !re15_discard_active()) o6++;
        gefahren++;
    }
    re15_discard_reset();

    printf("  ABDECKUNG: %d von %d Benutzungsstellen, je 6 Faelle\n",
           gefahren, RE15_DISCARD_SITE_COUNT);
    printf("  O1 Direktaufruf %d | O2 fremde Stelle %d | O3 fremde Nachricht %d |"
           " O4 ohne Besitz %d\n", o1, o2, o3, o4);
    printf("  O5 POSITIV (vorgemerkt) %d | O6 verbrauchter Vorentscheid %d\n", o5, o6);
    PRUEFE(gefahren == RE15_DISCARD_SITE_COUNT,
           "ABDECKUNG: nur %d von %d Stellen gefahren", gefahren, RE15_DISCARD_SITE_COUNT);
    PRUEFE(gefahren >= 10, "ABDECKUNG: nur %d Stellen (erwartet >= 10)", gefahren);
    PRUEFE(o1 == gefahren, "O1: nur %d von %d Stellen halten den Direktaufruf",
           o1, gefahren);
    PRUEFE(o2 == gefahren, "O2: nur %d von %d", o2, gefahren);
    PRUEFE(o3 == gefahren, "O3: nur %d von %d", o3, gefahren);
    PRUEFE(o4 == gefahren, "O4: nur %d von %d", o4, gefahren);
    PRUEFE(o5 == gefahren,
           "O5 (GEGENPROBE): nur %d von %d Stellen merken MIT Vorentscheid vor — ohne"
           " diese Zahl waere der ganze Teil durch Nichtstun gruen", o5, gefahren);
    PRUEFE(o6 == gefahren, "O6: nur %d von %d", o6, gefahren);
}

/* =========================================================================
 * TEIL P — re15_discard_restore: EIN LADEN BELEBT NIE EINE ABFRAGE.
 *
 * Pflicht-Korrektur 5 der Vorrunde: die Funktion hatte gar keinen Riegel. Bis Runde 23
 * stellte sie bei vorhandenem Gegenstand wieder auf D_WARTET — ein Wiederbeleben ohne
 * Beleg. Sie ist seither ein HARTER Reset, und das ist hier gemessen:
 *
 *   P1 aus D_WARTET  : restore(item) -> nichts aktiv, Anzahl unberuehrt (1)
 *   P2 aus D_FRAGT   : restore(item) -> nichts aktiv, die schon gefallene Anzahl
 *                      wieder 1 (@0x800517C4 `sb v1,count`)
 *   P3 aus D_AUS     : restore(item) mit einem Gegenstand im Inventar darf NICHTS
 *                      erzeugen — das ist der Wiederbelebungs-Fall von Runde 23
 *   P4 GEGENPROBE    : dieselbe Vorgeschichte OHNE restore -> die Abfrage STEHT.
 *                      Ohne P4 waere P1/P2 auch dann gruen, wenn nie etwas vorgemerkt
 *                      wuerde.
 * Herleitung, warum ein harter Reset byte-true ist: RE2s Fortsetzungs-Zeiger
 * DAT_800D4498 (@0x80051670 `sw v0,-0x7d50(at)`) liegt im RAM und in KEINEM
 * Speicherformat; der Auslieferungsstand von RE1.5 kann ohnehin nicht speichern
 * (21 RDTs sagen woertlich "Save is not available in this preview").
 * ========================================================================= */

/* Bis D_FRAGT fahren, ohne RDT: Vorentscheid + Einhaengen + ein freier Takt.
 * Das ist DIESELBE Reihenfolge, die op_message_on nimmt (scd_vm.c), nur ohne den Raum. */
static int bis_frage(unsigned room, uint8_t msg, uint8_t item)
{
    grundzustand();
    re15_inv_grant(item, 1);
    if (re15_discard_besitz_vor_nachricht(room, msg) != 1) return 0;
    re15_discard_notice_message(room, msg);
    if (!re15_discard_active()) return 0;
    for (int f = 0; f < 8 && !re15_discard_prompt(NULL, NULL); f++) re15_discard_tick(0, 0);
    return re15_discard_prompt(NULL, NULL) != 0;
}

static void teil_p(void)
{
    printf("\n=== TEIL P: ein Laden belebt NIE eine Abfrage (re15_discard_restore) ===\n");
    int p1 = 0, p2 = 0, p3 = 0, p4 = 0, gefahren = 0, erreicht_frage = 0;

    for (int i = 0; i < RE15_DISCARD_SITE_COUNT; i++) {
        unsigned room = re15_discard_sites[i].room;
        uint8_t  msg  = re15_discard_sites[i].msg;
        uint8_t  item = re15_discard_sites[i].item;

        /* --- P1: aus D_WARTET (vorgemerkt, Anzahl noch 1). ------------------------- */
        grundzustand();
        re15_inv_grant(item, 1);
        if (re15_discard_besitz_vor_nachricht(room, msg) == 1)
            re15_discard_notice_message(room, msg);
        int war_aktiv = re15_discard_active();
        uint8_t gemerkt = re15_discard_pending_item();
        PRUEFE(war_aktiv && gemerkt == item,
               "P1 ROOM%04X msg %u: die Vorgeschichte steht nicht (aktiv=%d,"
               " gemerkt=0x%02X)", room, (unsigned)msg, war_aktiv, gemerkt);
        re15_discard_restore(gemerkt);
        PRUEFE(!re15_discard_active() && re15_discard_pending_item() == 0,
               "P1 ROOM%04X msg %u: das Laden hat die Abfrage BELEBT", room, (unsigned)msg);
        { int slot = re15_inv_find_item(item);
          PRUEFE(slot >= 0 && g_inv.slots[slot].qty == 1,
                 "P1 ROOM%04X: Anzahl nach dem Laden %d statt 1",
                 room, slot >= 0 ? g_inv.slots[slot].qty : -1); }
        if (war_aktiv && !re15_discard_active() && re15_discard_pending_item() == 0) p1++;

        /* --- P2: aus D_FRAGT (sichtbarer Prompt, Anzahl schon auf 0 @0x80051810). --- */
        if (bis_frage(room, msg, item)) {
            erreicht_frage++;
            int slot0 = re15_inv_find_item(item);
            PRUEFE(slot0 >= 0 && g_inv.slots[slot0].qty == 0,
                   "P2 ROOM%04X: die Anzahl ist beim offenen Prompt %d statt 0"
                   " (@0x80051810)", room, slot0 >= 0 ? g_inv.slots[slot0].qty : -1);
            re15_discard_restore(re15_discard_pending_item());
            PRUEFE(!re15_discard_active() && re15_discard_prompt(NULL, NULL) == 0,
                   "P2 ROOM%04X: der Prompt steht nach dem Laden weiter", room);
            int slot = re15_inv_find_item(item);
            PRUEFE(slot >= 0 && g_inv.slots[slot].qty == 1,
                   "P2 ROOM%04X: Anzahl nach dem Laden %d statt 1 (@0x800517C4)",
                   room, slot >= 0 ? g_inv.slots[slot].qty : -1);
            if (!re15_discard_active() && slot >= 0 && g_inv.slots[slot].qty == 1) p2++;
        }

        /* --- P3: aus D_AUS. restore darf aus einem Spielstand-Feld NICHTS bauen. ---- */
        grundzustand();
        re15_inv_grant(item, 1);
        re15_discard_restore(item);
        PRUEFE(!re15_discard_active() && re15_discard_pending_item() == 0,
               "P3 ROOM%04X: restore(0x%02X) hat aus dem Nichts eine Abfrage erzeugt"
               " — genau das Wiederbeleben von Runde 23", room, item);
        if (!re15_discard_active() && re15_discard_pending_item() == 0) p3++;

        /* --- P4: GEGENPROBE. Dieselbe Vorgeschichte, KEIN restore -> sie steht. ----- */
        if (bis_frage(room, msg, item)) {
            PRUEFE(re15_discard_active() && re15_discard_prompt(NULL, NULL) == 8,
                   "P4 ROOM%04X: die Gegenprobe zeigt keine stehende Abfrage", room);
            if (re15_discard_active() && re15_discard_prompt(NULL, NULL) == 8) p4++;
        }
        gefahren++;
    }
    re15_discard_reset();

    printf("  ABDECKUNG: %d von %d Stellen | P1 aus D_WARTET %d | P2 aus D_FRAGT %d"
           " | P3 aus D_AUS %d\n", gefahren, RE15_DISCARD_SITE_COUNT, p1, p2, p3);
    printf("  P4 GEGENPROBE (ohne restore steht die Abfrage): %d von %d\n",
           p4, erreicht_frage);
    PRUEFE(gefahren >= 10, "ABDECKUNG: nur %d Stellen (erwartet >= 10)", gefahren);
    PRUEFE(erreicht_frage >= 10,
           "ABDECKUNG: nur %d Stellen erreichen den offenen Prompt (erwartet >= 10) —"
           " dann messen P2/P4 nichts", erreicht_frage);
    PRUEFE(p1 == gefahren, "P1: nur %d von %d", p1, gefahren);
    PRUEFE(p2 == erreicht_frage, "P2: nur %d von %d", p2, erreicht_frage);
    PRUEFE(p3 == gefahren, "P3: nur %d von %d", p3, gefahren);
    PRUEFE(p4 == erreicht_frage,
           "P4 (GEGENPROBE): nur %d von %d — ohne eine stehende Abfrage koennte P1/P2"
           " gar kein Beleben sehen", p4, erreicht_frage);
}

/* =========================================================================
 * TEIL Q — DER JA-ZWEIG, WENN DER GEMERKTE PLATZ DEN GEGENSTAND NICHT MEHR TRAEGT.
 *
 * Beim Uebergang auf die Abfrage faellt die Anzahl auf 0 (@0x80051810 `addiu v0,v0,-1`).
 * Weggeworfen wird im JA-Zweig aber nur, wenn der gemerkte PLATZ den Gegenstand noch
 * traegt — und re15_inv_remove_slot kompaktiert das Inventar (FUN_8004dadc), ein Index
 * kann danach auf einen fremden Platz zeigen. Vor Runde 25 fiel dieser Zweig ohne
 * anzahl_zurueck() durch: der Gegenstand blieb mit ANZAHL 0 im Inventar liegen, sichtbar
 * und unbenutzbar. RE2 schreibt die 1 in JEDEM Zweig zurueck, der nicht wegwirft
 * (@0x800517C4 `sb v1,count` mit v1 = 1).
 *
 * GEFAHREN wird die Verschiebung: zwei Plaetze, der Gegenstand auf dem hinteren; nach dem
 * Aufgehen der Abfrage wird der VORDERE Platz geleert, die Kompaktierung zieht den
 * Gegenstand nach vorn, der gemerkte Index zeigt ins Leere. Dann JA.
 *   Q1 VERLANGT   : kein Platz traegt den Gegenstand mit Anzahl 0.
 *   Q2 GEGENPROBE : derselbe Ablauf OHNE die Verschiebung wirft wirklich weg — sonst
 *                   waere "kein Platz mit Anzahl 0" auch dann wahr, wenn die Abfrage
 *                   ueberhaupt nichts tut.
 * ========================================================================= */
static void teil_q(void)
{
    printf("\n=== TEIL Q: JA-Zweig nach Inventar-Verschiebung (@0x800517C4) ===\n");
    const uint8_t fremd = 0x34;      /* ein anderer Gegenstand fuer den vorderen Platz */
    int gefahren = 0, ok_null = 0, weg_gegenprobe = 0;

    for (int i = 0; i < RE15_DISCARD_SITE_COUNT; i++) {
        unsigned room = re15_discard_sites[i].room;
        uint8_t  msg  = re15_discard_sites[i].msg;
        uint8_t  item = re15_discard_sites[i].item;
        if (item == fremd) continue;

        /* --- Q1: die Verschiebung. ------------------------------------------------- */
        grundzustand();
        PRUEFE(re15_inv_grant(fremd, 1) == 0, "Q1: Platzhalter 0x%02X nicht gewaehrt",
               fremd);
        re15_inv_grant(item, 1);
        if (re15_discard_besitz_vor_nachricht(room, msg) != 1) continue;
        re15_discard_notice_message(room, msg);
        for (int f = 0; f < 8 && !re15_discard_prompt(NULL, NULL); f++)
            re15_discard_tick(0, 0);
        if (!re15_discard_prompt(NULL, NULL)) continue;
        int slot_item  = re15_inv_find_item(item);
        int slot_fremd = re15_inv_find_item(fremd);
        PRUEFE(slot_item > slot_fremd,
               "Q1 ROOM%04X: der Gegenstand liegt nicht HINTER dem Platzhalter"
               " (%d vs %d) — dann verschiebt die Kompaktierung nichts",
               room, slot_item, slot_fremd);
        if (slot_fremd >= 0) re15_inv_remove_slot(slot_fremd);  /* kompaktiert FUN_8004dadc */
        int nach = re15_inv_find_item(item);
        PRUEFE(nach >= 0 && nach != slot_item,
               "Q1 ROOM%04X: der Gegenstand ist nicht nachgerueckt (%d -> %d)",
               room, slot_item, nach);
        /* Und JETZT die Antwort JA. */
        for (int f = 0; f < 4000 && !re15_discard_ready(); f++) re15_discard_tick(0, 0);
        re15_discard_tick(0x4000, 0);
        int rest = re15_inv_find_item(item);
        int qty  = rest >= 0 ? g_inv.slots[rest].qty : -1;
        PRUEFE(!(rest >= 0 && qty == 0),
               "Q1 ROOM%04X msg %u: 0x%02X liegt auf Platz %d mit ANZAHL 0 — ein"
               " Gegenstand, den der Spieler sieht und nicht benutzen kann"
               " (@0x800517C4 schreibt die 1 zurueck)", room, (unsigned)msg, item, rest);
        PRUEFE(!re15_discard_active(), "Q1 ROOM%04X: Abfrage nach JA nicht beendet", room);
        if (!(rest >= 0 && qty == 0) && !re15_discard_active()) ok_null++;

        /* --- Q2 GEGENPROBE: OHNE Verschiebung wirft JA wirklich weg. --------------- */
        grundzustand();
        re15_inv_grant(fremd, 1);
        re15_inv_grant(item, 1);
        if (re15_discard_besitz_vor_nachricht(room, msg) == 1)
            re15_discard_notice_message(room, msg);
        for (int f = 0; f < 8 && !re15_discard_prompt(NULL, NULL); f++)
            re15_discard_tick(0, 0);
        for (int f = 0; f < 4000 && !re15_discard_ready(); f++) re15_discard_tick(0, 0);
        re15_discard_tick(0x4000, 0);
        PRUEFE(re15_inv_find_item(item) < 0,
               "Q2 ROOM%04X: 0x%02X liegt nach JA immer noch im Inventar", room, item);
        if (re15_inv_find_item(item) < 0) weg_gegenprobe++;
        gefahren++;
    }
    re15_discard_reset();

    printf("  ABDECKUNG: %d Stellen gefahren | Q1 ohne Platz mit Anzahl 0: %d"
           " | Q2 GEGENPROBE weggeworfen: %d\n", gefahren, ok_null, weg_gegenprobe);
    PRUEFE(gefahren >= 10, "ABDECKUNG: nur %d Stellen (erwartet >= 10)", gefahren);
    PRUEFE(ok_null == gefahren, "Q1: nur %d von %d", ok_null, gefahren);
    PRUEFE(weg_gegenprobe == gefahren,
           "Q2 (GEGENPROBE): nur %d von %d Stellen werfen ohne Verschiebung wirklich weg"
           " — dann ist Q1 kein Befund", weg_gegenprobe, gefahren);
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
    teil_d();
    teil_e();
    teil_o();      /* die fail-closed-Kopplung — ohne Vorentscheid nichts, mit: alles */
    teil_p();      /* re15_discard_restore belebt nie eine Abfrage                    */
    teil_q();      /* JA-Zweig nach Inventar-Verschiebung (@0x800517C4)               */
    teil_l();
    teil_n();
    teil_m();
    teil_i();
    teil_j();
    teil_k();

    if (g_fehler) { printf("\nFEHLGESCHLAGEN: %d Pruefungen\n", g_fehler); return 1; }
    printf("\nOK — Abfrage, Ja/Nein und der Sackgassen-Riegel halten.\n");
    return 0;
}
