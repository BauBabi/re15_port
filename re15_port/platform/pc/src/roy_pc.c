/* =============================================================================================
 * roy_pc.c — Runde 37 Spur K "roy": Elzas Roy (Typ 0x47) aus dem ELZA-Archiv laden.
 *
 * ORIGINAL: FUN_80022300 liest bei gesetztem Elza-Bit (bltz @0x80022334) die Datei 0x27 =
 * EMD/CDEMD1.EMS (`ori v0,zero,0x27` @0x800223ac), Sektor/Laenge aus der TOC @0x80073178/@0x8007317c
 * (Typ 0x47: Sektor 1639, 142272 B). Belege: re15_roy_r37.h, Dossier K_roy.md §2.
 *
 * PORT: main.c pc_enemy_read_re15_emd fragt hier zuerst (ein Haken, "Runde 37 Spur K"); fuer jeden
 * Typ ausser Elzas 0x47 kommt NULL zurueck und der bisherige CDEMD0-Weg bleibt unveraendert. Gelesen
 * wird nur der eine Blob (fseek auf Sektor*2048), nicht das ganze 4,7-MB-Archiv. Der Aufrufer
 * bekommt einen eigenen malloc-Puffer (Bank-Besitz wie beim CDEMD0-Weg, eb->buf).
 * ============================================================================================= */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "re15_roy_r37.h"
#include "asset_root_pc.h"

#define RE15_EMS_SEKTOR 2048u          /* CD-Sektor; TOC-Werte sind Sektoren im Archiv */

uint8_t *re15_roy_pc_emd(uint8_t type, size_t *out_len)
{
    if (!re15_roy_aus_elza_archiv(type)) return NULL;
    uint32_t sektor = 0, laenge = 0;
    if (!re15_ems_toc(RE15_EMS_ARCHIV_ELZA, type, &sektor, &laenge)) return NULL;
    const char *rel = re15_ems_archiv_datei(RE15_EMS_ARCHIV_ELZA);
    char pfad[1024];
    for (int i = 0; re15_pc_cd_path(i, rel, pfad, (unsigned)sizeof pfad); i++) {
        FILE *f = fopen(pfad, "rb");
        if (!f) continue;
        uint8_t *buf = (uint8_t *)malloc(laenge);
        int ok = buf && fseek(f, (long)(sektor * RE15_EMS_SEKTOR), SEEK_SET) == 0 &&
                 fread(buf, 1, laenge, f) == laenge;
        fclose(f);
        if (ok) {
            if (out_len) *out_len = laenge;
            fprintf(stderr, "[roy] Typ 0x%02X aus %s Sektor %u (%u B) — Elza-Archiv (@0x800223ac)\n",
                    (unsigned)type, rel, (unsigned)sektor, (unsigned)laenge);
            return buf;
        }
        free(buf);
    }
    fprintf(stderr, "[roy] %s nicht lesbar — Typ 0x%02X faellt auf das Leon-Archiv zurueck\n",
            rel, (unsigned)type);
    return NULL;
}
