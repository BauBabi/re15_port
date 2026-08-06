/* probe_1030_spawncap.c — ROOM1030 darf nur SECHS Zombies instanziieren.
 *
 * NUTZER-REPORT (2026-08-06): "Im Original sind nur 6 Zombies sichtbar, bei uns Dutzende."
 *
 * MECHANISMUS (byte-true, selbst disassembliert): ROOM1030 setzt direkt VOR seine 20
 * Sce_em_set-Records ein Gleichzeitig-Limit — RDT-Datei-Offset 0x1de2 = `24 12 06 00` =
 * Save(0x12, 6), unmittelbar gefolgt vom ersten `44 00 16 0d`. Sce_em_set prueft:
 *   @0x80042214  lh v0,0(a1)          Lebend-Zaehler  (0x800b0ff2 = work_vars[0x11])
 *   @0x8004221c  lh v1,4084(v1)       Maximum         (0x800b0ff4 = work_vars[0x12])
 *   @0x80042224  slt v0,v0,v1         Zaehler < Maximum ?
 *   @0x80042228  bne v0,zero,...      ja  -> Zaehler++ @0x8004223c
 *   @0x80042230  ori v0,zero,0x8000   nein -> Record verworfen (@0x80042238 sw v0,0(s0))
 * Vorgabe 0xFF / Zaehler 0 aus der Raum-SCD-Init FUN_8003ecec @0x8003ed58/60 bzw. @0x8003ed7c.
 * Game-weit setzen den Opcode nur ROOM1030/1031 (6) und ROOM1040/1041 (5).
 *
 * Das Kill-Flag-Gate (@0x80042128-38) laeuft VOR der Zaehler-Pruefung und verbraucht keinen
 * Zaehler — ein bereits erledigter Record macht Platz fuer den naechsten aus der Warteschlange.
 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "re15_rdt.h"
#include "re15_scd.h"
#include "re15_actor.h"
#include "re15_room.h"

#define RE15_STR(x)  #x
#define RE15_XSTR(x) RE15_STR(x)

static uint8_t *read_file(const char *path, size_t *out)
{
    FILE *f = fopen(path, "rb");
    if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    if (sz <= 0) { fclose(f); return NULL; }
    uint8_t *b = (uint8_t *)malloc((size_t)sz);
    size_t rd = b ? fread(b, 1, (size_t)sz, f) : 0;
    fclose(f);
    if (!b || rd != (size_t)sz) { free(b); return NULL; }
    *out = (size_t)sz; return b;
}

static int count_zombies(void)
{
    int n = 0;
    for (int i = 1; i < RE15_ACTOR_MAX; i++)
        if (g_actors[i].active && g_actors[i].type == 0x16) n++;
    return n;
}

/* min_n/max_n = -1 heisst "egal". Fuer die Kontrollraeume wird bewusst NICHT auf eine exakte
 * Spawn-Zahl geprueft (die haengt am Skript-Verlauf und ist nicht Gegenstand dieses Tests) —
 * geprueft wird das LIMIT und dass der Raum nicht stillgelegt wird. */
static int run_room(const char *base, const char *name, unsigned room,
                    int min_n, int max_n, int cap_erwartet)
{
    char path[600]; size_t sz = 0;
    snprintf(path, sizeof path, "%s/STAGE%u/ROOM%04X.RDT", base, (room >> 12) & 0xF, room);
    uint8_t *buf = read_file(path, &sz);
    if (!buf) { printf("  %-10s RDT nicht lesbar\n", name); return 1; }
    static re15_rdt_t rdt;
    if (re15_rdt_parse(buf, sz, &rdt) != 0) { printf("  %-10s parse fail\n", name); return 1; }

    re15_actor_init();
    scd_vm_init();
    g_current_room_id = room;
    scd_register_room_events(&rdt);
    scd_room_reenter(&rdt, 0, 0, 0);
    for (int f = 0; f < 8; f++) scd_vm_tick();

    int n   = count_zombies();
    int cap = g_scd.work_vars[0x12];
    int ok  = (cap == cap_erwartet)
              && (min_n < 0 || n >= min_n)
              && (max_n < 0 || n <= max_n);
    printf("  %-10s Zombies=%2d   Limit work_vars[0x12]=%3d (erwartet %3d)  %s\n",
           name, n, cap, cap_erwartet, ok ? "OK" : "  <== FEHLER");
    return ok ? 0 : 1;
}

int main(void)
{
    const char *base = RE15_XSTR(RE15_ASSETS_PATH);
    printf("=== Gleichzeitig-Limit der Gegner-Spawns ===\n");
    int bad = 0;
    /* DER GEMELDETE FEHLER: ROOM1030 hat 20 Sce_em_set-Records, darf aber GENAU 6 instanziieren. */
    bad += run_room(base, "ROOM1030", 0x1030,  6,  6,   6);
    /* ROOM1040 setzt Save(0x12,5) — der einzige weitere Raum mit einem echten Limit.
     * Geprueft wird das Limit und dass nie mehr als 5 entstehen. */
    bad += run_room(base, "ROOM1040", 0x1040, -1,  5,   5);
    /* GEGENPROBE gegen den gefaehrlichsten Nebeneffekt: ein Raum OHNE den Opcode muss die
     * Vorgabe 0xFF behalten UND weiter spawnen. Stuende dort 0 (der memset der Raum-Init nullt
     * work_vars), wuerde das Gate JEDEN Spawn im ganzen Spiel verwerfen. */
    bad += run_room(base, "ROOM1140", 0x1140,  1, -1, 255);

    printf("\n%s\n", bad ? "ERGEBNIS: Spawn-Limit FALSCH." : "ERGEBNIS: Spawn-Limit byte-true.");
    return bad ? 1 : 0;
}
