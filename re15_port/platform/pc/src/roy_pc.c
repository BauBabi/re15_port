/* =============================================================================================
 * roy_pc.c — Runde 37 Spur K "roy" / Spur N "elzaems": den RE1.5-Gegnerblob aus dem Archiv laden, aus dem
 * das Original ihn holt.
 *
 * ORIGINAL: FUN_80022300 (einziger Aufrufer Sce_em_set `jal 0x80022300` @0x80042328) waehlt das Archiv fuer
 * JEDEN Typ nach DAT_800aca3c Bit 31 (`bltz` @0x80022334): 0 -> Datei 0x26 = EMD/CDEMD0.EMS (@0x8002233c,
 * TOC @0x80072f38), 1 -> Datei 0x27 = EMD/CDEMD1.EMS (`ori v0,zero,0x27` @0x800223ac, TOC @0x80073178);
 * Sektor/Laenge aus der TOC (Index Typ-0x10 @0x80022338). Das Bit ist Elzas Figurenbit UND SCD-Flag (1,0),
 * das vier Raumskripte fuer einen Birkin-Spawn umschalten (re15_elzaems_r37.h). Belege: Dossiers K_roy.md §2
 * (Typ 0x47) und N_elzaems.md §1/§2 (alle Typen, Blob im Original-RAM == TOC-Blob).
 *
 * PORT: main.c pc_enemy_read_re15_emd fragt hier zuerst (Haken der Spur K, "Runde 37 Spur K"). Name der
 * Funktion aus Spur K; seit Spur N gilt sie fuer JEDEN Typ:
 *   - Archiv des Typs (re15_ems_archiv_fuer_typ: beim Sce_em_set gemerkt) == Elza-Archiv -> den einen Blob aus
 *     EMD/CDEMD1.EMS lesen (fseek Sektor*2048, TOC-Laenge) — nicht ueber die Blobkette: in CDEMD1 liegt der
 *     Birkin 0x30 NICHT auf Kettenplatz 16, sondern ans Archivende angehaengt (TOC-Sektor 2015, n1_zensus).
 *   - sonst NULL -> der bisherige CDEMD0-Weg (Port-Kette == TOC-Blob fuer alle 25 Typen, n1_zensus).
 * Jeder Elza-Archiv-Blob schreibt eine debug.log-Zeile mit Sektor, Laenge und CRC32 (Riegel r37_elzaems);
 * "[roy]" bleibt fuer Typ 0x47 (Riegel der Spur K). Die Leon-Archiv-Zeile erscheint nur, wenn ein Elza-Skript
 * das Bit fuer den Spawn auf 0 gestellt hat (ROOM3071/50F1).
 * ============================================================================================= */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "re15_roy_r37.h"
#include "re15_elzaems_r37.h"
#include "re15_gameflow.h"
#include "asset_root_pc.h"

#define RE15_EMS_SEKTOR 2048u          /* CD-Sektor; TOC-Werte sind Sektoren im Archiv */

/* CRC32 (IEEE, reflektiert) — nur fuer die Messzeile; dieselbe Pruefsumme rechnet der Riegel aus der Datei. */
uint32_t re15_elzaems_crc32(const uint8_t *p, size_t n)
{
    uint32_t c = 0xFFFFFFFFu;
    for (size_t i = 0; i < n; i++) {
        c ^= p[i];
        for (int k = 0; k < 8; k++) c = (c >> 1) ^ (0xEDB88320u & (0u - (c & 1u)));
    }
    return ~c;
}

uint8_t *re15_roy_pc_emd(uint8_t type, size_t *out_len)
{
    uint32_t sektor = 0, laenge = 0;
    if (re15_ems_archiv_fuer_typ(type) != RE15_EMS_ARCHIV_ELZA) {
        if (re15_char_variant() && re15_ems_toc(RE15_EMS_ARCHIV_LEON, type, &sektor, &laenge))
            fprintf(stderr, "[elzaems] Typ 0x%02X aus EMD/CDEMD0.EMS Sektor %u (%u B) - Elza-Bit 0 beim "
                            "Sce_em_set (Skript-Flag (1,0), @0x8002233c)\n",
                    (unsigned)type, (unsigned)sektor, (unsigned)laenge);
        return NULL;
    }
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
            fprintf(stderr, "[%s] Typ 0x%02X aus %s Sektor %u (%u B) crc %08X - Elza-Archiv (@0x800223ac)\n",
                    type == RE15_ROY_TYP ? "roy" : "elzaems", (unsigned)type, rel, (unsigned)sektor,
                    (unsigned)laenge, (unsigned)re15_elzaems_crc32(buf, laenge));
            return buf;
        }
        free(buf);
    }
    fprintf(stderr, "[elzaems] %s nicht lesbar - Typ 0x%02X faellt auf das Leon-Archiv zurueck\n",
            rel, (unsigned)type);
    return NULL;
}
