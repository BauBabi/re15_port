/**
 * @file probe_r17_waffen_loop.c
 * @brief MESSSONDE + PIN (Runde 17, 2026-09-20, Thema waffen-loop).
 *
 * Nutzer: "nehme ich die mehrfach schuss Pistole mit in stage 1, bleibt dort die Kugel
 * Animation und Sound im loop haengen. Ausserdem bleibt genau in diesem Raum der Sound
 * haengen."
 *
 * Die "Mehrfachschuss-Pistole" ist die Beretta M93R (Inventar-Id 5) bzw. die Glock 18
 * (Id 6) — die beiden EINZIGEN Waffen, deren Entlade-Handler 0x800338A8 die Effekte mit
 * FX-SUB 2 spawnt (@0x800338ac/f4/28; Tabellenspiegel game_step_common.c ENT[5]/ENT[6]).
 * Genau diese drei Row-Ketten in CORE00.ESP — id2 sub2 @0x1188, id3 sub2 @0x44c,
 * id4 sub2 @0x1988 — bestehen aus je DREI Rows mit Routine-A 15 (@0x80017ac8), und sie
 * sind die einzigen Routine-15-Ketten der ganzen Datei (Scan ueber alle Ids/Subs). Jede
 * Row spawnt EIN Kind (Muendungsfeuer / Rauch / Huelse, jeweils sub 0) — das ist der
 * 3-Schuss-Burst.
 *
 * Original-Mechanismus (selbst disassembliert, info/Re1.5/PSX.EXE):
 *   Routine 15 @0x80017ac8:
 *     80017adc  sb   v1,108(v0)          ; Slot-Flags := 0x65
 *     80017af4  row[0x0e] != 0 -> 80017b04 addiu v0,v0,-1 ; 80017b0c sh -> RETURN
 *     80017b38  jal  0x800199d4          ; Kind spawnen (Code aus row[0x17]/row[0x16])
 *     80017b54  row[0x26] != 0 -> 80017b5c jal 0x800174e4 (Row-Advance)
 *     80017b6c  sb   zero,108(v1)        ; SONST: Flags-Byte := 0
 *   Und das Flags-Byte slot+0x6c IST die Belegung:
 *     Spawner   800197a8 lbu v0,108(t0) / 800197b0 beq v0,zero -> Platz wird NEU vergeben
 *     Tick 1    80019e70 lbu v0,108(v1) / 80019e78 andi v0,v0,0x1 / 80019e7c beq v0,zero
 *               -> Routine-A-Dispatch uebersprungen
 *     Tick-Rumpf 80019f44 lbu v1,108(a3) / 80019f4c andi 0x1 / 80019f50 beq v0,zero,0x8001a480
 * Der Port fuehrt die Belegung in einem EIGENEN Feld (`active`); `f->flags = 0` liess den
 * Platz deshalb ewig im Dispatch stehen, row[0x0e] blieb 0 und Routine 15 spawnte in JEDEM
 * Bild ein neues Kind — bis der 96er-Pool voll war und dort blieb. Jedes Muendungs-Kind
 * traegt in seiner Row-Kette Routine 9 = den positionsgebundenen KNALL (ARMS-Record 0);
 * Effekt und Schuss-Sound liefen damit endlos weiter. Der Pool wird nur beim RAUMLADEN
 * gewischt (scd_room_setup.c:195) — deshalb "genau in diesem Raum".
 *
 * Aufruf: probe_r17_waffen_loop [pin]
 *   pin -> ctest unit_r17_waffen_loop_pin:
 *          (1) jede der drei sub-2-Ketten spawnt GENAU 3 Kinder,
 *          (2) ihr eigener Platz ist nach der Kette frei,
 *          (3) 300 Bilder nach dem letzten Schuss ist der Effekt-Pool LEER.
 */
#include "re15_esp.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef RE15_ASSET_PSX_DIR
#define RE15_ASSET_PSX_DIR "shared_assets/PSX"
#endif

static uint8_t *lade(const char *rel, int *out_size)
{
    char pfad[512];
    snprintf(pfad, sizeof pfad, "%s/%s", RE15_ASSET_PSX_DIR, rel);
    FILE *f = fopen(pfad, "rb");
    if (!f) { fprintf(stderr, "FEHLER: %s nicht lesbar\n", pfad); return NULL; }
    fseek(f, 0, SEEK_END); long n = ftell(f); fseek(f, 0, SEEK_SET);
    uint8_t *b = (uint8_t *)malloc((size_t)n);
    if (!b || fread(b, 1, (size_t)n, f) != (size_t)n) { fclose(f); free(b); return NULL; }
    fclose(f);
    *out_size = (int)n;
    return b;
}

/* Die drei Entlade-Effekte der Burst-Pistolen, exakt wie ENT[5]/ENT[6] in
 * game_step_common.c (Belege @0x800338ac/f4/28). */
static const struct { uint8_t id, sub; uint16_t scale; const char *name; } BURST[3] = {
    { 2, 2, 0x0800, "Muendungsfeuer" },
    { 3, 2, 0x0c00, "Rauch"          },
    { 4, 2, 0x0800, "Huelse"         },
};

int main(int argc, char **argv)
{
    int pin = (argc > 1 && strcmp(argv[1], "pin") == 0);

    int csz = 0;
    uint8_t *core = lade("DATA/CORE00.ESP", &csz);
    if (!core) return 2;
    static re15_esp_t bank;
    if (re15_esp_parse_global(core, (size_t)csz, &bank) != 0) {
        fprintf(stderr, "FEHLER: CORE00.ESP nicht parsebar\n");
        return 2;
    }
    re15_esp_set_global_bank(&bank);

    int fehler = 0;

    printf("== Runde 17 / waffen-loop: Routine-15-Ketten der Burst-Pistolen (Item 5/6) ==\n");
    printf("CORE00.ESP %d Bytes, %d Effekt-Ids\n\n", csz, bank.id_count);

    /* --- Teil 1: jede Kette einzeln. Erwartung = 3 Kinder, danach Platz frei. --------- */
    for (int k = 0; k < 3; k++) {
        re15_esp_fx_reset();
        int n = re15_esp_fx_spawn_rows(&bank, BURST[k].id, BURST[k].sub, BURST[k].scale,
                                       0, 0, 0, 0, 0);
        if (n != 1) { printf("  ! id%u sub%u: %d Stroeme statt 1\n",
                             BURST[k].id, BURST[k].sub, n); fehler++; continue; }

        int kinder = 0, eltern_tot_bei = -1;
        int vorher = re15_esp_fx_count();
        /* Kinder ZAEHLEN heisst: Plaetze, die in diesem Tick NEU belegt wurden und die
         * Kind-Signatur (dieselbe Effekt-Id, sub 0) tragen — der Netto-Poolstand taugt
         * nicht, weil Kinder selbst Enkel spawnen und zwischendurch sterben. */
        uint8_t belegt[RE15_ESP_FX_MAX];
        for (int i = 0; i < RE15_ESP_FX_MAX; i++) belegt[i] = (re15_esp_fx_get(i) != NULL);
        for (int t = 0; t < 60; t++) {
            re15_esp_fx_tick(&bank);
            for (int i = 0; i < RE15_ESP_FX_MAX; i++) {
                const re15_esp_fx_t *fi = re15_esp_fx_get(i);
                if (fi && !belegt[i] && fi->effect_id == BURST[k].id && fi->sub_index == 0)
                    kinder++;
                belegt[i] = (fi != NULL);
            }
            if (eltern_tot_bei < 0 && re15_esp_fx_get(0) == NULL) eltern_tot_bei = t;
        }
        /* Ein Kind-Spawn belegt EINEN Platz JE STROM des Kind-Effekts (Muendungsfeuer
         * id2 sub0 fuehrt 2 Stroeme, Rauch/Huelse je 1) — SOLL = 3 Spawns x Stroeme. */
        int kei = re15_esp_find_id(&bank, BURST[k].id);
        int kstr = (kei >= 0) ? re15_esp_row_streams(&bank, kei, 0) : 1;
        if (kstr <= 0) kstr = 1;
        int soll = 3 * kstr;
        printf("  id%u sub%u (%s): Start-Plaetze %d, Kind-Plaetze %d (SOLL %d = 3 x %d Stroeme), "
               "Eltern tot ab Tick %d, Pool nach 60 Ticks %d\n",
               BURST[k].id, BURST[k].sub, BURST[k].name, vorher, kinder, soll, kstr,
               eltern_tot_bei, re15_esp_fx_count());
        if (kinder != soll) {
            printf("    ! SOLL %d, IST %d (drei Routine-15-Rows = drei Schuss des Bursts)\n",
                   soll, kinder);
            fehler++;
        }
        if (eltern_tot_bei < 0) {
            printf("    ! Eltern-Platz wurde NIE frei (Original: 80017b6c sb zero,108(v1))\n");
            fehler++;
        }
    }

    /* --- Teil 2: ein voller Schuss (alle drei Ketten) und lange Nachlaufzeit. --------- */
    re15_esp_fx_reset();
    for (int k = 0; k < 3; k++)
        re15_esp_fx_spawn_rows(&bank, BURST[k].id, BURST[k].sub, BURST[k].scale, 0, 0, 0, 0, 0);
    int spitze = 0;
    for (int t = 0; t < 300; t++) {
        re15_esp_fx_tick(&bank);
        int c = re15_esp_fx_count();
        if (c > spitze) spitze = c;
    }
    int rest = re15_esp_fx_count();
    printf("\n  Ein Schuss (3 Ketten), 300 Bilder Nachlauf: Spitze %d Plaetze, Rest %d\n",
           spitze, rest);
    if (rest != 0) {
        printf("    ! SOLL 0 belegte Plaetze, IST %d — der Effekt-/Knall-Loop lebt noch\n", rest);
        fehler++;
    }
    if (spitze >= RE15_ESP_FX_MAX) {
        printf("    ! Pool lief voll (%d von %d) — das ist der gemeldete Dauer-Loop\n",
               spitze, RE15_ESP_FX_MAX);
        fehler++;
    }

    printf("\n%s (%d Befunde)\n", fehler ? "FEHLER" : "OK", fehler);
    free(core);
    if (pin) return fehler ? 1 : 0;
    return 0;
}
