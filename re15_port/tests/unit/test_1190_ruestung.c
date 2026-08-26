/* test_1190_ruestung.c — PIN: die R.P.D.-Ruestung in ROOM1190
 *
 * NUTZER-BEFUND (2026-08-26): "Nach anziehen der Weste im ROOM 1190 aendert sich Leons
 * outfit nicht und wahrscheinlich hat er auch die Vorteile der Weste nicht."
 *
 * ── DASS ES DIESE MECHANIK GIBT, STEHT IM RAUM SELBST ────────────────────────────────
 * Die Message-Tabelle von ROOM1190.RDT (RDT+0x3C), mit dem Dekoder DES PORTS gelesen:
 *     msg 0x04: "There is one R.P.D. armor that should fit you.Will you equip it?"
 *     msg 0x05: "Will you unequip the armor?"
 *
 * ── DIE KETTE, GANZ GEMESSEN ─────────────────────────────────────────────────────────
 *  1. Prop 16 = die Weste auf einem Staender: Obj_model_set @Datei 0x002D4C (sub14),
 *     pos (20480,-1500,-21168). Sie fehlte bis zum Kapazitaets-Fix (s. test_1190_weste).
 *  2. Aot_set @Datei 0x002D90 (sub14): slot 16, sce 3, Rechteck (19700,-22000)+(1000,1500)
 *     — enthaelt die Weste. Nutzlast pc[14..19] = ff 00 18 0f 00 00 -> (u16@+2)>>8 = 15.
 *     sce-3-Handler @0x800430F0: `lhu a0,0(rec+0xC); lbu a1,3(rec+0xC); jal FUN_8003ee3c`
 *     = "fuehre sub15 aus".
 *  3. sub15 @Datei 0x002DA6 = UMSCHALTER auf Flag(3,0x75):
 *        21 03 75 00  Flag(3,0x75) pruefen
 *        2b 04 80 ff  Meldung 0x04 (equip?)   /  2b 05 80 ff  Meldung 0x05 (unequip?)
 *        22 03 75 01  Flag setzen             /  22 03 75 00  Flag loeschen
 *        24 10 01 00  work_vars[0x10] = 1     /  24 10 00 00  work_vars[0x10] = 0
 *        47 0f        Aot_on(15)
 *  4. AOT-Slot 15 = Door_aot_set @Datei 0x002D70, Rechteck (0,0,0,0) = reiner Skript-
 *     Ausloeser; next_pos (19100,0,-21250), Zielraum 0x19 = ROOM1190 SELBST, Cut 13.
 *  5. Raumlader FUN_800396fc @0x80039760-8c:
 *        80039760  lbu  a0,0x0(s0)=>DAT_800aca5c   ; Modell/Waffen-Byte
 *        80039768  lh   v1,DAT_800b0ff0            ; work_vars[0x10]
 *        8003976c  andi v0,a0,0xf                  ; untere Nibble = MODELL-Index
 *        80039770  beq  v0,v1,LAB_80039790         ; unveraendert -> nichts tun
 *        80039788  jal  FUN_800314b0               ; sonst Spielermodell NEU LADEN
 *     FUN_800314b0 @0x800314d4-1c: `lbu v0,DAT_800aca5c; sll v0,v0,1;
 *     lhu a0,DAT_80073f70[v0]` — Tabelle @0x80073f70 = 16 u16 0x3C..0x4B, also
 *     Datei-Id = 0x3C + Index = PL0<Index>.PLD. Index 1 = PL01 = Leon mit roter
 *     "POLICE"-Panzerweste (Textur dekodiert und angesehen).
 *
 * ── KEINE SCHADENSMINDERUNG IM ORIGINAL (bewusst NICHT erfunden) ─────────────────────
 * Flag(3,0x75) hat game-weit 14 Fundstellen: gesetzt/geloescht NUR in ROOM1190/1191
 * sub15, gelesen in deren sub14 und in ROOM3060 sub03, ROOM3091 sub07, ROOM40A0/40A1
 * sub06. Die EXE liest das Flag-Wort (Bank 3 = DAT_800b0ff8, Bit 117 -> 0x800b1004)
 * NIRGENDS — keine Code-Referenz im Ghidra-Dump. Der Bit-0x4-Test im Spieler-Schadenspfad
 * (@0x80012088 `andi v1,v1,0x4`) trifft die Modell-Nibbles 4..7 = die ZWEITE Figur, nicht
 * die Ruestung (Modell 1). Der Nutzen der Weste ist Skript-seitig.
 *
 * ── WAS DIESER PIN PRUEFT ────────────────────────────────────────────────────────────
 *   P1  sub14 legt Prop 16 an der belegten Stelle an und installiert AOT 16 als
 *       Ereignis-AOT mit event_id 15.
 *   P2  AOT 16 ausloesen setzt work_vars[0x10] auf 1 UND Flag(3,0x75); ein zweites Mal
 *       setzt beides zurueck (der Umschalter, nicht bloss ein Einschalter).
 *   P3  Die Meldungstexte des Raums sind woertlich die Ruestungs-Abfragen.
 *   P4  PLD-DATENLAGE, auf der der Modelltausch beruht: PL00 und PL01 haben BYTE-GLEICHE
 *       EDD+EMR und UNTERSCHIEDLICHE MD1+TIM — es wechseln also nur Mesh und Textur.
 *       Der Schnitt wird im Test gegen die separat ausgelieferten PL00.* gegengeprueft,
 *       damit die Regel hier nicht bloss behauptet ist.
 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "re15_rdt.h"
#include "re15_scd.h"
#include "re15_aot.h"
#include "re15_msg.h"
#include "re15_md1.h"   /* re15_pld_part = die AUSGELIEFERTE Schnitt-Regel */


extern void scd_vm_init(void);
extern void scd_vm_tick(void);
extern int  scd_thread_start(int slot, const uint8_t *pc);
extern void scd_register_room_events(const re15_rdt_t *rdt);
extern void scd_vm_set_room_init(int on);

static int g_fail = 0;
#define CHECK(cond, msg) do {                                            \
        if (!(cond)) { printf("  FAIL: %s\n", (msg)); g_fail++; }         \
        else         { printf("  ok  : %s\n", (msg)); }                   \
    } while (0)

static uint32_t fnv1a(const uint8_t *d, long n)
{
    uint32_t h = 0x811c9dc5u;
    for (long i = 0; i < n; i++) { h ^= d[i]; h *= 0x01000193u; }
    return h;
}

static uint8_t *slurp(const char *rel, long *out)
{
    char p[512];
    snprintf(p, sizeof p, "%s/%s", RE15_ASSET_PSX_DIR, rel);
    FILE *f = fopen(p, "rb");
    if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    uint8_t *b = (uint8_t *)malloc((size_t)sz);
    if (!b || fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); fclose(f); return NULL; }
    fclose(f); *out = sz; return b;
}

/* ⛔ KEINE TEST-EIGENE SCHNITT-REGEL. Der Pin ruft dieselbe Engine-Funktion, die auch
 * pc_sync_player_model in main.c benutzt (re15_pld_part, re15_md1.h) — sonst pruefte P4f
 * eine Kopie gegen sich selbst und sagte nichts ueber den ausgelieferten Code. */
static int pld_part(const uint8_t *b, long sz, int idx, uint32_t *off, uint32_t *len)
{
    unsigned long o = 0, l = 0;
    if (!re15_pld_part(b, sz, idx, &o, &l)) return 0;
    *off = (uint32_t)o; *len = (uint32_t)l; return 1;
}

/* Zaehler fuer den Spielermodell-Rueckruf: der Pin registriert ihn selbst, damit er den
 * AUSGELIEFERTEN Ladeweg misst und nicht bloss die Skript-Variable. */
static int s_sync_calls = 0;
static void zaehl_sync(void) { s_sync_calls++; }

static re15_rdt_t s_rdt;

int main(void)
{
    printf("== test_1190_ruestung ==\n");
    long sz = 0;
    uint8_t *b = slurp("STAGE1/ROOM1190.RDT", &sz);
    if (!b) { printf("  FAIL: ROOM1190.RDT nicht lesbar\n"); return 1; }
    if (re15_rdt_parse(b, (size_t)sz, &s_rdt) != 0) { printf("  FAIL: Parse\n"); return 1; }

    /* ---- P1 ------------------------------------------------------------------------ */
    scd_vm_init();
    scd_register_room_events(&s_rdt);
    if (s_rdt.main_scd)    scd_thread_start(0, s_rdt.main_scd);
    if (s_rdt.sub_scd[14]) scd_thread_start(1, s_rdt.sub_scd[14]);
    scd_vm_set_room_init(1);
    for (int t = 0; t < 4; t++)  scd_vm_tick();
    scd_vm_set_room_init(0);
    for (int t = 0; t < 60; t++) scd_vm_tick();

    int vest = -1;
    for (int i = 0; i < (int)g_scd.prop_count; i++)
        if (g_scd.props[i].obj_id == 16) vest = i;
    CHECK(vest >= 0, "P1a sub14 legt Prop 16 (die Weste) an");
    if (vest >= 0) {
        printf("  Prop16: pos=(%d,%d,%d) aktiv=%u\n", g_scd.props[vest].x,
               g_scd.props[vest].y, g_scd.props[vest].z,
               (unsigned)g_scd.props[vest].active);
        CHECK(g_scd.props[vest].x == 20480 && g_scd.props[vest].y == -1500 &&
              g_scd.props[vest].z == -21168,
              "P1b an der belegten Stelle (20480,-1500,-21168) @Datei 0x002D4C");
        CHECK(g_scd.props[vest].active == 1, "P1c und sichtbar");
    }
    {
        const re15_aot_t *a = &g_aot.slots[16];
        printf("  AOT16: typ=%d aktiv=%d event_id=%u rect=(%d,%d) +-(%d,%d)\n",
               (int)a->type, (int)a->active, (unsigned)a->event_id,
               a->x, a->z, a->half_w, a->half_h);
        CHECK(a->active, "P1d AOT-Slot 16 ist installiert");
        CHECK(a->event_id == 15, "P1e und feuert sub15 (Nutzlast ff 00 18 0f -> 0x0f18>>8)");
        CHECK(a->x - a->half_w <= 20480 && 20480 <= a->x + a->half_w &&
              a->z - a->half_h <= -21168 && -21168 <= a->z + a->half_h,
              "P1f sein Rechteck enthaelt die Weste");
    }

    /* ---- P2: der UMSCHALTER -------------------------------------------------------- */
    printf("  work_vars[0x10]=%d Flag(3,0x75)=%d  (Ausgangslage)\n",
           (int)g_scd.work_vars[0x10], re15_game_flag_get(3, 0x75) ? 1 : 0);
    CHECK(g_scd.work_vars[0x10] == 0, "P2a vorher kein Ruestungs-Modell");

    re15_aot_fire_slot(16);
    for (int t = 0; t < 90; t++) scd_vm_tick();
    printf("  nach 1. Ausloesen: work_vars[0x10]=%d Flag(3,0x75)=%d\n",
           (int)g_scd.work_vars[0x10], re15_game_flag_get(3, 0x75) ? 1 : 0);
    CHECK(g_scd.work_vars[0x10] == 1,
          "P2b ANLEGEN setzt work_vars[0x10] = 1 = Modell-Index PL01");
    CHECK(re15_game_flag_get(3, 0x75) != 0, "P2c und Flag(3,0x75)");

    re15_aot_fire_slot(16);
    for (int t = 0; t < 90; t++) scd_vm_tick();
    printf("  nach 2. Ausloesen: work_vars[0x10]=%d Flag(3,0x75)=%d\n",
           (int)g_scd.work_vars[0x10], re15_game_flag_get(3, 0x75) ? 1 : 0);
    CHECK(g_scd.work_vars[0x10] == 0,
          "P2d ABLEGEN setzt work_vars[0x10] zurueck auf 0 = PL00 "
          "(ein Umschalter, kein Einschalter)");
    CHECK(re15_game_flag_get(3, 0x75) == 0, "P2e und loescht Flag(3,0x75)");

    /* ---- P2f/P2g: ⛔ FEUERT DER LADEWEG UEBERHAUPT? --------------------------------
     * Hier ist die Ruestung schon einmal GESCHEITERT, obwohl P2b/P2d gruen waren: das
     * Anlegen loest eine SELBST-TUER aus (Record @Datei 0x2d70 zielt auf ROOM1190), und
     * der Port gatete seinen Ladeweg mit `if (dest_id != g_current_room_id)`. Damit lief
     * kein Modellwechsel — die Skript-Variable stimmte, das Bild nicht.
     * Das Original kennt diesen Kurzschluss nicht; sein Tuer-Warp vergleicht NUR die
     * Stage und ruft den Raumlader unbedingt:
     *     8001d960  lbu  v0,8(a0)            ; Ziel-STAGE
     *     8001d968  beq  v1,v0,0x8001d988    ; gleiche Stage -> Stage-Lader ueberspringen
     *     8001d980  jal  0x80039a30          ; (nur bei Stage-Wechsel)
     *     8001d988  jal  0x800396fc          ; RAUMLADER — UNBEDINGT
     * Und der Raumlader traegt den Modell-Test (@0x80039760-88).
     *
     * Zweitens: work_vars[0x10] MUSS den Raum-Reset ueberleben. Das Original wischt bei
     * der Raum-Init punktuell (FUN_8003ecec: 0x800b0ff4, 0x800b1028, 0x800b0ff2,
     * 0x800aca50, 0x800b281e) — 0x800b0ff0 ist NICHT dabei. Der pauschale memset des
     * Ports haette Leon die Weste beim naechsten Raum wieder ausgezogen. */
    {
        g_scd.work_vars[0x10] = 1;              /* Ruestung an */
        s_sync_calls = 0;
        re15_scd_set_player_model_sync(zaehl_sync);
        scd_room_reenter(&s_rdt, 0, 0, 0);      /* = der Selbst-Raum-Ladeweg */
        re15_scd_set_player_model_sync(NULL);
        printf("  nach scd_room_reenter: Rueckrufe=%d work_vars[0x10]=%d\n",
               s_sync_calls, (int)g_scd.work_vars[0x10]);
        CHECK(s_sync_calls == 1,
              "P2f der Raumlade-Weg ruft den Modell-Rueckruf GENAU EINMAL "
              "(sonst feuert der Wechsel im Spiel nie)");
        CHECK(g_scd.work_vars[0x10] == 1,
              "P2g work_vars[0x10] ueberlebt den Raum-Reset (0x800b0ff0 wird im "
              "Original nicht gewischt)");
    }

    /* ---- P3: die Texte des Raums --------------------------------------------------- */
    re15_msg_load_room_block(s_rdt.messages, s_rdt.messages_size);
    {
        const char *m4 = re15_msg_get_text(4);
        const char *m5 = re15_msg_get_text(5);
        printf("  msg 0x04: \"%s\"\n", m4 ? m4 : "(fehlt)");
        printf("  msg 0x05: \"%s\"\n", m5 ? m5 : "(fehlt)");
        CHECK(m4 && strstr(m4, "R.P.D. armor") && strstr(m4, "equip"),
              "P3a Meldung 0x04 ist die ANLEGEN-Abfrage");
        CHECK(m5 && strstr(m5, "unequip") && strstr(m5, "armor"),
              "P3b Meldung 0x05 ist die ABLEGEN-Abfrage");
    }
    free(b);

    /* ---- P4: die PLD-Datenlage ------------------------------------------------------ */
    {
        long s0 = 0, s1 = 0;
        uint8_t *p0 = slurp("PLD/PL00.PLD", &s0);
        uint8_t *p1 = slurp("PLD/PL01.PLD", &s1);
        CHECK(p0 && p1, "P4a PL00.PLD und PL01.PLD sind da");
        if (p0 && p1) {
            static const char *NM[4] = { "EDD (Anim)", "EMR (Skelett)",
                                         "MD1 (Mesh)", "TIM (Textur)" };
            uint32_t o0, l0, o1, l1, h0[4], h1[4], L0[4], L1[4];
            for (int i = 0; i < 4; i++) {
                if (!pld_part(p0, s0, i, &o0, &l0) || !pld_part(p1, s1, i, &o1, &l1)) {
                    printf("  FAIL: PLD-Schnitt %d\n", i); g_fail++; continue;
                }
                h0[i] = fnv1a(p0 + o0, l0); L0[i] = l0;
                h1[i] = fnv1a(p1 + o1, l1); L1[i] = l1;
                printf("  %-14s PL00 %6u B FNV=0x%08X | PL01 %6u B FNV=0x%08X  -> %s\n",
                       NM[i], L0[i], h0[i], L1[i], h1[i],
                       h0[i] == h1[i] ? "GLEICH" : "VERSCHIEDEN");
            }
            CHECK(L0[0] == 3160 && h0[0] == 0xC334FD9Bu && h1[0] == h0[0],
                  "P4b EDD (Animation) ist zwischen PL00 und PL01 BYTE-GLEICH");
            CHECK(L0[1] == 57136 && h0[1] == 0xA05ADF2Bu && h1[1] == h0[1],
                  "P4c EMR (Skelett) ist BYTE-GLEICH — der Wechsel kann keine "
                  "Animation kaputtmachen");
            CHECK(L0[2] == 28916 && L1[2] == 30708 && h0[2] != h1[2],
                  "P4d MD1 unterscheidet sich (die Weste bringt Geometrie mit)");
            CHECK(L0[3] == 99872 && L1[3] == 99872 && h0[3] != h1[3],
                  "P4e TIM unterscheidet sich (rote POLICE-Weste statt Uniform)");

            /* GEGENPROBE der Schnitt-Regel gegen die separat ausgelieferten Dateien —
             * sonst waere P4b..e nur intern konsistent, ohne Bezug zur Wirklichkeit. */
            static const char *FN[4] = { "PLD/PL00.EDD", "PLD/PL00.EMR",
                                         "PLD/PL00.MD1", "PLD/PL00.TIM" };
            int allsame = 1;
            for (int i = 0; i < 4; i++) {
                long fs = 0; uint8_t *fb = slurp(FN[i], &fs);
                if (!fb) { allsame = 0; continue; }
                if (fs != (long)L0[i] || fnv1a(fb, fs) != h0[i]) allsame = 0;
                free(fb);
            }
            CHECK(allsame,
                  "P4f GEGENPROBE: die vier aus PL00.PLD geschnittenen Bloecke sind "
                  "byte-gleich mit PL00.EDD/.EMR/.MD1/.TIM — die Schnitt-Regel stimmt");
        }
        free(p0); free(p1);
    }

    printf(g_fail ? "== test_1190_ruestung: %d FEHLER ==\n"
                  : "== test_1190_ruestung: OK ==\n", g_fail);
    return g_fail ? 1 : 0;
}
