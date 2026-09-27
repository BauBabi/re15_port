/* test_elza_zweig.c — Riegel fuer den Elza-Zweig (Runde 35).
 *
 * Bis Runde 34 war die Charakterwahl folgenlos: g_gameflow.character trug 0/1,
 * gelesen wurde es nur in zwei `& 4`-Tests, die mit 0/1 nie zuschlagen konnten,
 * und der Startraum stand unbedingt auf 0x1240. Zwei Vollstarts erzeugten
 * bitgleiche Bilder (0 von 691200 Pixeln Unterschied, elza-portzustand.md).
 *
 * Dieser Haken nagelt die fuenf Stellen fest, an denen das jetzt anders ist —
 * und zwar jede gegen ihren Original-Beleg:
 *
 *  A  KODIERUNG. DAT_800ACA5C ist der PLD-INDEX, 0 = Leon / 4 = Elza. TITLE.BIN
 *     schreibt ihn zweimal: `sll v0,v0,2` @0x801016a4 + `sb` @0x801016ac
 *     (Cursor << 2) und `sb zero` @0x801024c0 / `ori v0,zero,0x4` @0x801024cc +
 *     `sb` @0x801024d4. FUN_800314b0 indiziert damit die PLD-Dateitabelle
 *     0x80073f70 (`lbu` @0x800314d4, `sll v0,v0,1`): 0 -> CD 60 = PL00.PLD,
 *     4 -> CD 64 = PL04.PLD.
 *
 *  B  DER &4-TEST IST ERREICHBAR. Das ist der Diskriminator des Originals —
 *     STAGE1.BIN `lbu` @0x80104000 / `andi v0,v0,0x4` @0x80104008 / `bne`
 *     @0x8010400c (gleichlautend STAGE2 @0x80103e9c, STAGE3 @0x801040ec,
 *     STAGE4 @0x80103fb4, STAGE5 @0x80104134). Mit der alten 0/1-Kodierung war
 *     er tot; hier wird gemessen, dass er fuer Elza zuschlaegt und fuer Leon nicht.
 *
 *  C  RAUMVARIANTE. FUN_800396fc holt den CD-Basisindex aus der Stage-Tabelle
 *     (Zeigertabelle 0x8007438c @0x800397cc, `lhu v0,0(v1)` @0x800397e0) und
 *     addiert das Elza-Bit: `srl a0,a0,31` @0x800397e4, `addu a0,a0,v0`
 *     @0x800397ec. Die Stage-0-Tabelle @0x8007429c laeuft in Schritten von 3
 *     (681, 684, 687, …) = drei CD-Dateien je Raum, die Elza-RDT ist die
 *     Leon-RDT + 1. Im Port ist der Dateiindex die Raum-Id, die Variante also
 *     deren niedrigste Hex-Ziffer.
 *
 *  D  DER STARTRAUM STEHT IN DEN AUSGELIEFERTEN BYTES. Beide Vorspann-RDT haben
 *     je genau EINEN Door_aot_set in main00 @Datei-0x051A und unterscheiden sich
 *     in EINEM Byte — dem Zielraum-Index bei +23 = Datei-0x0531:
 *         ROOM1240.RDT @0x0531 = 0x17  -> Raum 0x17 = ROOM1170 "HELIPORT"
 *         ROOM1241.RDT @0x0531 = 0x03  -> Raum 0x03 = ROOM1031 "LOBBY"
 *     Das sind Zeichen fuer Zeichen die Raumindizes des Original-Einstiegs
 *     FUN_8001d22c (`bltz` @0x8001d2a4 auf DAT_800ACA3C; Leon `ori v0,zero,0x17`
 *     @0x8001d2a8 + `sh` @0x8001d2b0, Elza `ori v0,zero,0x3` @0x8001d324 + `sh`
 *     @0x8001d32c). Der Haken liest diese beiden Bytes aus der echten RDT —
 *     faellt die Belegstelle weg, faellt der Haken.
 *
 *  E  TUEREN UND SPEICHERSTAND TRAGEN DIE VARIANTE MIT. Die Tuer-Zielformel
 *     (aot_common.c: dest = ((stage+1)<<12) | (raum<<4) | (aktuell & 0xF))
 *     bleibt in der Variante, und ein Save/Load-Rundlauf bringt Elza samt
 *     angefordertem Modell-Index zurueck (0x800b0fbe <-> 0x800aca5c,
 *     `lbu` @0x80026f4c / `sb` @0x80026f6c und zurueck `lbu` @0x8001d4c8 /
 *     `sb` @0x8001d4f4; work_vars[0x10] = DAT_800B0FF0, gefuellt @0x8001d558).
 */
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include "re15_gameflow.h"
#include "re15_savedata.h"
#include "re15_memcard.h"
#include "re15_actor.h"
#include "re15_room.h"
#include "re15_scd.h"

#ifndef RE15_ASSET_PSX_DIR
#define RE15_ASSET_PSX_DIR "shared_assets/PSX"
#endif

static int lies_byte(const char *rel, long off, int *out)
{
    char p[512];
    snprintf(p, sizeof p, "%s/%s", RE15_ASSET_PSX_DIR, rel);
    FILE *f = fopen(p, "rb");
    if (!f) { fprintf(stderr, "FAIL: %s nicht lesbar\n", p); return -1; }
    if (fseek(f, off, SEEK_SET) != 0) { fclose(f); return -1; }
    int c = fgetc(f);
    fclose(f);
    if (c < 0) return -1;
    *out = c;
    return 0;
}

int main(void)
{
    int fail = 0;
    printf("=== Elza-Zweig: Kodierung, Variante, Startraum, Rundlauf ===\n");

    /* ---- A  Kodierung: der Cursor wird zum PLD-Index (@0x801016a4 sll 2) ---- */
    re15_gameflow_init();
    re15_gameflow_new_game(0);
    int leon_ch = g_gameflow.character;
    unsigned leon_room = g_gameflow.start_room;
    int leon_var = re15_char_variant();
    re15_gameflow_new_game(1);
    int elza_ch = g_gameflow.character;
    unsigned elza_room = g_gameflow.start_room;
    int elza_var = re15_char_variant();

    printf("A  Leon: character=%d variante=%d start=%04X\n", leon_ch, leon_var, leon_room);
    printf("A  Elza: character=%d variante=%d start=%04X\n", elza_ch, elza_var, elza_room);
    if (leon_ch != 0) { fprintf(stderr, "FAIL(A-leon): character=%d, erwartet 0 (@0x801024c0)\n", leon_ch); fail = 1; }
    if (elza_ch != 4) { fprintf(stderr, "FAIL(A-elza): character=%d, erwartet 4 (@0x801024cc/d4)\n", elza_ch); fail = 1; }

    /* ---- B  der &4-Test des Originals schlaegt jetzt an (@0x80104008) ---- */
    g_gameflow.character = 0;
    int leon_gate = ((g_gameflow.character & 4) == 0);   /* == enemy_ai_common.c:4230 */
    g_gameflow.character = 4;
    int elza_gate = ((g_gameflow.character & 4) == 0);
    printf("B  &4-Tor: Leon nimmt Zweig-A=%d, Elza nimmt Zweig-A=%d\n", leon_gate, elza_gate);
    if (!leon_gate) { fprintf(stderr, "FAIL(B-leon): Leon muss den ERSTEN Zweig nehmen\n"); fail = 1; }
    if (elza_gate)  { fprintf(stderr, "FAIL(B-elza): Elza muss den ZWEITEN Zweig nehmen — mit 0/1 war er unerreichbar\n"); fail = 1; }
    /* Gegenprobe, dass das Tor ueberhaupt etwas misst: der ALTE Port-Wert 1 faellt
     * auf Leons Seite. Genau das war der Defekt, nicht eine Meinung darueber. */
    g_gameflow.character = 1;
    if ((g_gameflow.character & 4) != 0) {
        fprintf(stderr, "FAIL(B-probe): Wert 1 darf NICHT im Elza-Zweig landen\n"); fail = 1; }

    /* ---- C  Raumvariante: Basis unveraendert, niederste Ziffer = Spieler ---- */
    g_gameflow.character = 0;
    unsigned l1170 = re15_room_for_char(0x1170), l1031 = re15_room_for_char(0x1030);
    g_gameflow.character = 4;
    unsigned e1170 = re15_room_for_char(0x1170), e1031 = re15_room_for_char(0x1030);
    printf("C  0x1170 -> Leon %04X / Elza %04X ; 0x1030 -> Leon %04X / Elza %04X\n",
           l1170, e1170, l1031, e1031);
    if (l1170 != 0x1170 || e1170 != 0x1171 || l1031 != 0x1030 || e1031 != 0x1031) {
        fprintf(stderr, "FAIL(C): Variantenregel (@0x800397e4/@0x800397ec) stimmt nicht\n"); fail = 1; }
    if (RE15_ROOM_BASE(0x1241) != 0x1240 || RE15_ROOM_BASE(0x1240) != 0x1240) {
        fprintf(stderr, "FAIL(C-basis): RE15_ROOM_BASE\n"); fail = 1; }

    /* ---- D  der Startraum gegen die ausgelieferten RDT-Bytes ---- */
    int ziel_leon = -1, ziel_elza = -1;
    if (lies_byte("STAGE1/ROOM1240.RDT", 0x0531, &ziel_leon) != 0 ||
        lies_byte("STAGE1/ROOM1241.RDT", 0x0531, &ziel_elza) != 0) {
        fprintf(stderr, "FAIL(D): Vorspann-RDT nicht lesbar\n"); fail = 1;
    } else {
        printf("D  ROOM1240@0x0531=0x%02X (Raum 0x%02X)  ROOM1241@0x0531=0x%02X (Raum 0x%02X)\n",
               ziel_leon, ziel_leon, ziel_elza, ziel_elza);
        if (ziel_leon != 0x17) { fprintf(stderr, "FAIL(D-leon): erwartet 0x17 = ROOM1170 (@0x8001d2a8)\n"); fail = 1; }
        if (ziel_elza != 0x03) { fprintf(stderr, "FAIL(D-elza): erwartet 0x03 = ROOM1031 (@0x8001d324)\n"); fail = 1; }
    }
    if (leon_room != 0x1240) { fprintf(stderr, "FAIL(D-start-leon): %04X\n", leon_room); fail = 1; }
    if (elza_room != 0x1241) { fprintf(stderr, "FAIL(D-start-elza): %04X\n", elza_room); fail = 1; }
    if (leon_var != 0 || elza_var != 1) { fprintf(stderr, "FAIL(D-bit)\n"); fail = 1; }

    /* ---- E1  Tuer-Zielformel bleibt in der Variante ----
     * Nachgerechnet wie aot_common.c: dest = ((stage+1)<<12) | (raum<<4) | (aktuell & 0xF).
     * Beispiel: aus Elzas Lobby ROOM1031 zu Stage-Index 0 / Raum 0x17. */
    {
        unsigned aktuell = 0x1031, dest_stage = 0, dest_raum = 0x17;
        unsigned dest = ((dest_stage + 1u) << 12) | (dest_raum << 4) | (aktuell & 0x000Fu);
        printf("E1 Tuer aus %04X nach Stage %u Raum %02X -> %04X\n", aktuell, dest_stage, dest_raum, dest);
        if (dest != 0x1171) { fprintf(stderr, "FAIL(E1): %04X statt 1171 — die Variante faellt weg\n", dest); fail = 1; }
        aktuell = 0x1030;
        dest = ((dest_stage + 1u) << 12) | (dest_raum << 4) | (aktuell & 0x000Fu);
        if (dest != 0x1170) { fprintf(stderr, "FAIL(E1-leon): %04X statt 1170\n", dest); fail = 1; }
    }

    /* ---- E2  Save/Load bringt Elza samt Modell-Index zurueck ---- */
    {
        const char *mcr = "test_elza_card.mcr";
        remove(mcr);
        memset(&g_actors[0], 0, sizeof g_actors[0]);
        g_actors[0].hp = 100;
        g_current_room_id = 0x1031;
        g_gameflow.character = 4;
        g_scd.work_vars[0x10] = 4;
        memset(&g_game, 0, sizeof g_game);

        re15_savedata_t sd;
        re15_savedata_capture(&sd, 1234, 1);
        if (sd.character != 4) {
            fprintf(stderr, "FAIL(E2-capture): character=%u statt 4 (@0x80026f6c)\n", sd.character); fail = 1; }
        if (re15_memcard_save(mcr, 0, &sd, "BIOHAZARD 1.5") != 0) {
            fprintf(stderr, "FAIL(E2-save)\n"); fail = 1; }

        /* Zustand verwuesten — danach muss der Ladeweg alles zurueckholen. */
        g_gameflow.character = 0;
        g_scd.work_vars[0x10] = 0;
        g_current_room_id = 0x1240;

        re15_savedata_t ld; uint16_t room = 0;
        if (re15_memcard_load(mcr, 0, &ld) != 0 || re15_savedata_restore(&ld, &room) != 0) {
            fprintf(stderr, "FAIL(E2-load)\n"); fail = 1;
        } else {
            printf("E2 geladen: character=%d room=%04X work_vars[0x10]=%d\n",
                   g_gameflow.character, room, (int)g_scd.work_vars[0x10]);
            if (g_gameflow.character != 4) {
                fprintf(stderr, "FAIL(E2-char): %d statt 4 (@0x8001d4f4)\n", g_gameflow.character); fail = 1; }
            if (room != 0x1031) {
                fprintf(stderr, "FAIL(E2-room): %04X statt 1031\n", room); fail = 1; }
            /* ⛔ Ohne diesen Wert taeuscht der Load: das Spiel haette Elza geladen,
             * aber der erste Raumwechsel danach haette sie gegen PL00 getauscht
             * (@0x8003976c andi 0xf / @0x80039770 beq / @0x80039788 jal). */
            if (g_scd.work_vars[0x10] != 4) {
                fprintf(stderr, "FAIL(E2-modell): work_vars[0x10]=%d statt 4 — der naechste "
                                "Raumwechsel wuerde PL00 nachladen\n", (int)g_scd.work_vars[0x10]);
                fail = 1; }
        }
        remove(mcr);
    }

    printf(fail ? "=== FEHLGESCHLAGEN ===\n" : "=== OK ===\n");
    return fail;
}
