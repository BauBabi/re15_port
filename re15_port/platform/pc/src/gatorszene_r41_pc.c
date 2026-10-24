/*
 * RE1.5 Rebuilt — Runde 41 Spur F "gatorszene", PC-Teil: Ton-Anbindung der 2090-Ankunftsszene.
 * Herleitung: include/re15_gatorszene_r41.h, Dossier analysis/befunde_runde41/F_gatorszene.md.
 *
 *  - Gator-SEs (ENEMSE Bank 17, Paar-Tabelle @0x800A7400) ueber denselben Weg wie der Boss (main.c pc_gator_se_re15 +
 *    re15_audio_re2_enemy_bank, re15_gator_audio_hook): Schrei der Grossreaktion SE 0 (`addu a0,zero,zero` @0x80102c0c +
 *    `jal 0x8005bd6c` @0x80102c10).
 *  - Metalltuer der 2080-Montage: der Door_exit-Satz (Tonkopf-Eintrag 1, re15_audio.h re15_audio_re2_tuer_se) des RE2-
 *    Tuerarchivs, das der Port fuer GENAU diese Tuer spielt - ROOM2080 Slot 0 (-> 2030) = DOOR06 (gen/tuer_zuordnung.inc
 *    `S161 T075 ROOM2080 -> ROOM2030 | ROOM2080.RDT@0xBA2 main00 Slot 0 | DOOR06 V0`), Tonteil-Groesse aus der Archiv-
 *    tabelle @0x8009A568 (`98 4c 8c 90 0a ..` = 0x4C98 B, re15_door_seq_re2_archiv). Form: cut_10f0.c Tuerknall (Door_exit
 *    des Archivs der Tuer, durch die die Figur kommt).
 */
#include "re15_gatorszene_r41.h"
#include "re15_audio.h"
#include "re15_door_seq.h"
#include "asset_root_pc.h"

#include <stdio.h>
#include <stdlib.h>

extern void pc_gator_se_re15(int se_id, int flag2000);   /* main.c: ENEMSE-Mapper des Boss-Alligators */

#define GSZ_PC_TUER_ARCHIV  0x06   /* DOOR06 (gen/tuer_zuordnung.inc S161) */
#define GSZ_PC_DOOR_EXIT    1      /* Tonkopf-Eintrag 1 = Door_exit (re15_audio.h) */

static void tuerton_2080(void)
{
    int ton = 0, modell = 0, sektor = 0, datei = 0, n = 0;
    if (re15_door_seq_re2_archiv(GSZ_PC_TUER_ARCHIV, &ton, &modell, &sektor, &datei) != 0) return;
    uint8_t *d = re15_pc_read_re2("DOOR/DOOR06.DO2", &n);
    if (!d || n < ton) {
        fprintf(stderr, "[gatorszene] DOOR06.DO2 fehlt/zu kurz (%d < %d) -> Metalltuer stumm\n", n, ton);
        free(d);
        return;
    }
    int ok = re15_audio_re2_tuer_laden(d, ton);
    if (ok) re15_audio_re2_tuer_se(GSZ_PC_DOOR_EXIT);
    fprintf(stderr, "[gatorszene] Metalltuer 2080: DOOR06 Tonteil %d B %s, Door_exit (Satz %d)\n", ton,
            ok ? "geladen" : "NICHT geladen", GSZ_PC_DOOR_EXIT);
    free(d);
}

void re15_pc_gatorszene_r41_init(void)
{
    re15_gatorszene_r41_audio_hook(pc_gator_se_re15, re15_audio_re2_enemy_bank);
    re15_gatorszene_r41_tuer_hook(tuerton_2080);
}
