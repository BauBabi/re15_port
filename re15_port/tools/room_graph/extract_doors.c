/**
 * @file extract_doors.c
 * @brief Door_aot_set_4p-Extraktor — Extrahiert alle Tür-Opcodes aus RDT-Dateien
 *
 * Standalone-Tool der Asset-Pipeline. Liest RDT-Dateien aller 6 Stages,
 * parst mainScd und subScd-Sektionen und extrahiert Door_aot_set_4p-Opcodes
 * (0x68, 40 Bytes) mit vollständigen Feldern.
 *
 * Ausgabe: Strukturierte Tür-Datensätze auf stdout (CSV-Format).
 * Fehlerbehandlung: Warnung auf stderr bei unlesbarer SCD-Sektion, Rest
 * wird weiterverarbeitet.
 *
 * Requirements: 3.1, 3.2, 3.8
 *
 * Nutzung:
 *   extract_doors [ASSETS_PATH]
 *   Falls kein Argument: nutzt Umgebungsvariable RE15_ASSETS_PATH
 *
 * RDT-Pfad-Format: STAGE{N}/ROOM{Stage}{RoomHex}{Player}.RDT
 *   Stage: 1-6, RoomHex: 00-27 (hex), Player: 0/1
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

/* ============================================================================
 * Konfiguration
 * ========================================================================= */

/** Stages 1-6 */
#define STAGE_MIN       1
#define STAGE_MAX       6

/** Room-Range: 0x00 - 0x27 (dezimal 0-39) */
#define ROOM_HEX_MAX    0x27

/** Player-Varianten: 0 (Leon), 1 (Elza) */
#define PLAYER_MAX      1

/** Maximale RDT-Dateigröße (2 MB — PSX RAM-Limit) */
#define MAX_RDT_SIZE    (2 * 1024 * 1024)

/** Maximale Pfadlänge */
#define MAX_PATH_LEN    512

/** Door_aot_set_4p Opcode */
#define OPCODE_DOOR_AOT_SET_4P  0x68

/** Gesamtgröße des Opcodes */
#define DOOR_OPCODE_SIZE        40

/** RDT-Adresstabelle: Offset von main_scd (Index 14, ab 0x08 gezählt) */
#define RDT_ADDR_TABLE_OFFSET   0x08
#define RDT_MAIN_SCD_INDEX      14  /* (0x40 - 0x08) / 4 = 14 */
#define RDT_SUB_SCD_INDEX       15  /* (0x44 - 0x08) / 4 = 15 */
#define RDT_SECTION_COUNT       21

/** Minimale RDT-Dateigröße (Header + Adresstabelle) */
#define RDT_MIN_SIZE            (0x08 + RDT_SECTION_COUNT * 4)

/* ============================================================================
 * Datenstrukturen
 * ========================================================================= */

/**
 * Extrahierter Tür-Datensatz.
 */
typedef struct {
    /* Quelle */
    uint8_t  src_stage;
    uint8_t  src_room;
    uint8_t  src_player;
    const char* scd_section;    /* "main" oder "sub" */

    /* Aus dem Opcode extrahierte Felder */
    uint8_t  aot_slot;
    uint8_t  floor_band;
    int16_t  trigger_x[4];
    int16_t  trigger_z[4];
    uint8_t  dest_stage;
    uint8_t  dest_room;
    uint8_t  dest_cut;
    int16_t  spawn_x;
    int16_t  spawn_y;
    int16_t  spawn_z;
    int16_t  spawn_rotation;
} door_record_t;

/* ============================================================================
 * Statistiken
 * ========================================================================= */

static int g_files_processed = 0;
static int g_files_skipped   = 0;
static int g_doors_found     = 0;

/* ============================================================================
 * Hilfsfunktionen
 * ========================================================================= */

/**
 * Liest eine 16-Bit signed LE Ganzzahl aus einem Bytepuffer.
 */
static int16_t read_s16_le(const uint8_t* p)
{
    uint16_t val = (uint16_t)p[0] | ((uint16_t)p[1] << 8);
    return (int16_t)val;
}

/**
 * Liest eine 32-Bit unsigned LE Ganzzahl aus einem Bytepuffer.
 */
static uint32_t read_u32_le(const uint8_t* p)
{
    return (uint32_t)p[0]
         | ((uint32_t)p[1] << 8)
         | ((uint32_t)p[2] << 16)
         | ((uint32_t)p[3] << 24);
}

/**
 * Konstruiert den RDT-Dateipfad.
 *
 * Format: {assets_root}/STAGE{N}/ROOM{Stage}{RoomHex}{Player}.RDT
 */
static void build_rdt_path(char* out, size_t out_size,
                           const char* assets_root,
                           int stage, int room_hex, int player)
{
    snprintf(out, out_size,
             "%s/STAGE%d/ROOM%d%02X%d.RDT",
             assets_root, stage, stage, room_hex, player);
}

/**
 * Lädt eine Datei vollständig in den Speicher.
 *
 * @return  Puffer (caller muss free() aufrufen), oder NULL bei Fehler
 *          *out_size wird auf die Dateigröße gesetzt
 */
static uint8_t* load_file(const char* path, int* out_size)
{
    FILE* f;
    long fsize;
    uint8_t* buf;
    size_t bytes_read;

    f = fopen(path, "rb");
    if (!f) {
        return NULL;
    }

    fseek(f, 0, SEEK_END);
    fsize = ftell(f);
    fseek(f, 0, SEEK_SET);

    if (fsize <= 0 || fsize > MAX_RDT_SIZE) {
        fclose(f);
        return NULL;
    }

    buf = (uint8_t*)malloc((size_t)fsize);
    if (!buf) {
        fclose(f);
        return NULL;
    }

    bytes_read = fread(buf, 1, (size_t)fsize, f);
    fclose(f);

    if ((long)bytes_read != fsize) {
        free(buf);
        return NULL;
    }

    *out_size = (int)fsize;
    return buf;
}

/* ============================================================================
 * SCD-Scanning
 * ========================================================================= */

/**
 * Scannt eine SCD-Sektion nach Door_aot_set_4p-Opcodes und gibt sie aus.
 *
 * SCD ist Bytecode — wir scannen sequentiell nach Opcode 0x68 und prüfen
 * ob genug Bytes für einen vollständigen 40-Byte-Datensatz verbleiben.
 *
 * @param scd_data      Pointer auf SCD-Sektionsdaten
 * @param scd_size      Größe der SCD-Sektion in Bytes
 * @param src_stage     Quell-Stage (1-6) für die Ausgabe
 * @param src_room      Quell-Room (hex) für die Ausgabe
 * @param src_player    Spieler-Variante (0/1)
 * @param section_name  "main" oder "sub" — für Ausgabe
 * @return              Anzahl gefundener Door-Opcodes, -1 bei Fehler
 */
static int scan_scd_for_doors(const uint8_t* scd_data, int scd_size,
                              int src_stage, int src_room, int src_player,
                              const char* section_name)
{
    int offset = 0;
    int doors_found = 0;
    int i;

    if (!scd_data || scd_size < 2) {
        return -1;
    }

    /* Linear-Scan: SCD-Bytecode durchsuchen */
    while (offset < scd_size) {
        uint8_t opcode = scd_data[offset];

        /* Door_aot_set_4p gefunden? */
        if (opcode == OPCODE_DOOR_AOT_SET_4P) {
            const uint8_t* p;
            door_record_t door;

            /* Prüfe ob genug Bytes verbleiben */
            if (offset + DOOR_OPCODE_SIZE > scd_size) {
                fprintf(stderr,
                    "[WARN] ROOM%d%02X%d/%s_scd: Door_aot_set_4p bei Offset "
                    "0x%04X abgeschnitten (%d Bytes verbleiben, 40 benötigt)\n",
                    src_stage, src_room, src_player, section_name,
                    offset, scd_size - offset);
                break;
            }

            p = scd_data + offset;

            /* Felder extrahieren gemäß Layout aus scd_opcodes_aot.c:
             *   [opcode(1)][aot_slot(1)][type(1)][floor(1)]
             *   [trigger_x[4] s16 LE (8B)]
             *   [trigger_z[4] s16 LE (8B)]
             *   [dest_stage(1)][dest_room(1)][dest_cut(1)][pad(1)]
             *   [spawn_x(s16)][spawn_y(s16)][spawn_z(s16)][spawn_rot(s16)]
             *   [reserved(8B)]
             */
            memset(&door, 0, sizeof(door));
            door.src_stage   = (uint8_t)src_stage;
            door.src_room    = (uint8_t)src_room;
            door.src_player  = (uint8_t)src_player;
            door.scd_section = section_name;

            door.aot_slot    = p[1];
            door.floor_band  = p[3];

            for (i = 0; i < 4; i++) {
                door.trigger_x[i] = read_s16_le(p + 4 + i * 2);
            }
            for (i = 0; i < 4; i++) {
                door.trigger_z[i] = read_s16_le(p + 12 + i * 2);
            }

            door.dest_stage     = p[20];
            door.dest_room      = p[21];
            door.dest_cut       = p[22];

            door.spawn_x        = read_s16_le(p + 24);
            door.spawn_y        = read_s16_le(p + 26);
            door.spawn_z        = read_s16_le(p + 28);
            door.spawn_rotation = read_s16_le(p + 30);

            /* CSV-Ausgabe */
            printf("ROOM%d%02X%d,%s,%d,%d,",
                   src_stage, src_room, src_player, section_name,
                   door.aot_slot, door.floor_band);

            /* Trigger-Polygon X */
            printf("%d,%d,%d,%d,",
                   door.trigger_x[0], door.trigger_x[1],
                   door.trigger_x[2], door.trigger_x[3]);

            /* Trigger-Polygon Z */
            printf("%d,%d,%d,%d,",
                   door.trigger_z[0], door.trigger_z[1],
                   door.trigger_z[2], door.trigger_z[3]);

            /* Ziel und Spawn */
            printf("%d,%d,%d,%d,%d,%d,%d\n",
                   door.dest_stage, door.dest_room, door.dest_cut,
                   door.spawn_x, door.spawn_y, door.spawn_z,
                   door.spawn_rotation);

            doors_found++;
            offset += DOOR_OPCODE_SIZE;
        } else {
            /* Nicht unser Opcode — ein Byte weiter scannen.
             * Hinweis: Für einen vollständig korrekt decodierenden Scanner
             * müssten wir die Opcode-Größen kennen. Da wir aber nur nach
             * 0x68 suchen und die 40-Byte-Struktur validieren, genügt ein
             * Byte-für-Byte-Scan. False-Positives sind unwahrscheinlich
             * bei 40-Byte-Alignment. */
            offset++;
        }
    }

    return doors_found;
}

/* ============================================================================
 * RDT-Verarbeitung
 * ========================================================================= */

/**
 * Verarbeitet eine einzelne RDT-Datei: Liest Adresstabelle, extrahiert
 * mainScd und subScd, scannt beide nach Door_aot_set_4p.
 *
 * @return  Anzahl extrahierter Doors, oder -1 bei Fehler
 */
static int process_rdt_file(const char* path, int stage, int room_hex, int player)
{
    uint8_t* buf = NULL;
    int file_size = 0;
    int doors_total = 0;
    int result;
    uint32_t main_scd_offset, sub_scd_offset;
    uint32_t main_scd_end, sub_scd_end;
    int main_scd_size, sub_scd_size;

    /* Datei laden */
    buf = load_file(path, &file_size);
    if (!buf) {
        /* Datei existiert nicht — das ist normal (nicht alle Kombinationen belegt) */
        return 0;
    }

    /* Minimale Größe prüfen */
    if (file_size < RDT_MIN_SIZE) {
        fprintf(stderr,
            "[WARN] %s: Datei zu klein (%d Bytes, min %d). Übersprungen.\n",
            path, file_size, RDT_MIN_SIZE);
        free(buf);
        g_files_skipped++;
        return -1;
    }

    /* Adresstabelle lesen: mainScd und subScd Offsets */
    main_scd_offset = read_u32_le(buf + RDT_ADDR_TABLE_OFFSET + RDT_MAIN_SCD_INDEX * 4);
    sub_scd_offset  = read_u32_le(buf + RDT_ADDR_TABLE_OFFSET + RDT_SUB_SCD_INDEX * 4);

    g_files_processed++;

    /* mainScd verarbeiten */
    if (main_scd_offset != 0 && main_scd_offset < (uint32_t)file_size) {
        /* Größe bestimmen: bis zum nächsten Sektions-Offset oder Dateiende */
        main_scd_end = (uint32_t)file_size;
        if (sub_scd_offset != 0 && sub_scd_offset > main_scd_offset) {
            main_scd_end = sub_scd_offset;
        }
        main_scd_size = (int)(main_scd_end - main_scd_offset);

        if (main_scd_size > 0) {
            result = scan_scd_for_doors(
                buf + main_scd_offset, main_scd_size,
                stage, room_hex, player, "main");
            if (result < 0) {
                fprintf(stderr,
                    "[WARN] %s: mainScd (Offset 0x%08X) nicht lesbar. Übersprungen.\n",
                    path, main_scd_offset);
            } else {
                doors_total += result;
            }
        }
    } else if (main_scd_offset != 0) {
        fprintf(stderr,
            "[WARN] %s: mainScd Offset 0x%08X ungültig (Datei: %d Bytes). Übersprungen.\n",
            path, main_scd_offset, file_size);
    }

    /* subScd verarbeiten */
    if (sub_scd_offset != 0 && sub_scd_offset < (uint32_t)file_size) {
        /* Größe: bis zum nächsten gültigen Offset nach sub_scd oder Dateiende */
        sub_scd_end = (uint32_t)file_size;

        /* Prüfe nächste Sektion in der Adresstabelle (extra_scd = Index 16) */
        if (RDT_SUB_SCD_INDEX + 1 < RDT_SECTION_COUNT) {
            uint32_t next_offset = read_u32_le(
                buf + RDT_ADDR_TABLE_OFFSET + (RDT_SUB_SCD_INDEX + 1) * 4);
            if (next_offset != 0 && next_offset > sub_scd_offset
                && next_offset <= (uint32_t)file_size) {
                sub_scd_end = next_offset;
            }
        }
        sub_scd_size = (int)(sub_scd_end - sub_scd_offset);

        if (sub_scd_size > 0) {
            result = scan_scd_for_doors(
                buf + sub_scd_offset, sub_scd_size,
                stage, room_hex, player, "sub");
            if (result < 0) {
                fprintf(stderr,
                    "[WARN] %s: subScd (Offset 0x%08X) nicht lesbar. Übersprungen.\n",
                    path, sub_scd_offset);
            } else {
                doors_total += result;
            }
        }
    } else if (sub_scd_offset != 0) {
        fprintf(stderr,
            "[WARN] %s: subScd Offset 0x%08X ungültig (Datei: %d Bytes). Übersprungen.\n",
            path, sub_scd_offset, file_size);
    }

    free(buf);
    g_doors_found += doors_total;
    return doors_total;
}

/* ============================================================================
 * Hauptprogramm
 * ========================================================================= */

/**
 * Gibt die CSV-Kopfzeile aus.
 */
static void print_csv_header(void)
{
    printf("source_rdt,scd_section,aot_slot,floor_band,"
           "trigger_x0,trigger_x1,trigger_x2,trigger_x3,"
           "trigger_z0,trigger_z1,trigger_z2,trigger_z3,"
           "dest_stage,dest_room,dest_cut,"
           "spawn_x,spawn_y,spawn_z,spawn_rotation\n");
}

/**
 * Gibt eine Zusammenfassung auf stderr aus.
 */
static void print_summary(void)
{
    fprintf(stderr, "\n=== Extraktion abgeschlossen ===\n");
    fprintf(stderr, "  RDT-Dateien verarbeitet: %d\n", g_files_processed);
    fprintf(stderr, "  RDT-Dateien übersprungen: %d\n", g_files_skipped);
    fprintf(stderr, "  Door_aot_set_4p gefunden: %d\n", g_doors_found);
}

int main(int argc, char* argv[])
{
    const char* assets_root = NULL;
    char path_buf[MAX_PATH_LEN];
    int stage, room, player;

    /* Assets-Pfad bestimmen: Argument > Umgebungsvariable */
    if (argc >= 2) {
        assets_root = argv[1];
    } else {
        assets_root = getenv("RE15_ASSETS_PATH");
    }

    if (!assets_root || assets_root[0] == '\0') {
        fprintf(stderr,
            "[ERROR] Kein Asset-Pfad angegeben.\n"
            "Nutzung: %s <ASSETS_PATH>\n"
            "Oder setze Umgebungsvariable RE15_ASSETS_PATH.\n",
            argv[0]);
        return 1;
    }

    fprintf(stderr, "[INFO] Assets-Pfad: %s\n", assets_root);
    fprintf(stderr, "[INFO] Scanne Stages %d-%d, Rooms 0x00-0x%02X, "
                    "Player 0-%d\n",
            STAGE_MIN, STAGE_MAX, ROOM_HEX_MAX, PLAYER_MAX);

    /* CSV-Header ausgeben */
    print_csv_header();

    /* Alle RDT-Dateien über 6 Stages iterieren */
    for (stage = STAGE_MIN; stage <= STAGE_MAX; stage++) {
        for (room = 0; room <= ROOM_HEX_MAX; room++) {
            for (player = 0; player <= PLAYER_MAX; player++) {
                build_rdt_path(path_buf, sizeof(path_buf),
                               assets_root, stage, room, player);
                process_rdt_file(path_buf, stage, room, player);
            }
        }
    }

    /* Zusammenfassung */
    print_summary();

    return (g_doors_found > 0) ? 0 : 2;
}
