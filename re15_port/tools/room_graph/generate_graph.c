/**
 * @file generate_graph.c
 * @brief Raum-Graph-Generator — Erzeugt Adjazenzliste aus Door-Extraktion
 *
 * Standalone-Tool der Asset-Pipeline. Liest die CSV-Ausgabe von extract_doors
 * (stdin oder Datei), baut einen gerichteten Raum-Graphen und gibt diesen als
 * room_graph.json und room_graph.md aus.
 *
 * Pro Door-Opcode entsteht eine gerichtete Kante mit:
 *   - Ziel-Room (ROOM{Stage}{RoomHex})
 *   - Ziel-Stage
 *   - Spawn-Position (X/Y/Z)
 *   - Blickwinkel (spawn_angle)
 *
 * Besondere Kennzeichnungen:
 *   - ROOM1240: start_node = true (Intro-Raum)
 *   - Räume ohne Door-Opcodes: dead_end = true
 *
 * Requirements: 3.3, 3.4, 3.5, 3.7
 *
 * Nutzung:
 *   extract_doors <ASSETS_PATH> | generate_graph [OUTPUT_DIR]
 *   generate_graph [OUTPUT_DIR] < doors.csv
 *
 *   OUTPUT_DIR: Verzeichnis für room_graph.json + room_graph.md
 *              (Standard: aktuelles Verzeichnis)
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

/* ============================================================================
 * Konfiguration
 * ========================================================================= */

/** Maximale Anzahl Knoten (6 Stages × 40 Rooms × 2 Player = 480, aber
 *  wir deduplizieren auf Room-Ebene ohne Player → max 240) */
#define MAX_NODES       256

/** Maximale Kanten pro Knoten (großzügig: Byte-Scanner kann viele Treffer
 *  pro Room liefern, besonders bei falsch-positiven 0x68-Bytes) */
#define MAX_EDGES       64

/** Maximale Zeilenlänge für CSV-Eingabe */
#define MAX_LINE_LEN    1024

/** Maximale Pfadlänge */
#define MAX_PATH_LEN    512

/** Room-ID-String-Länge ("ROOM" + Stage + 2hex = max 8 Zeichen) */
#define ROOM_ID_LEN     16

/** Start-Knoten: ROOM1240 (Stage 1, Room 0x24) */
#define START_STAGE     1
#define START_ROOM_HEX  0x24

/* ============================================================================
 * Datenstrukturen
 * ========================================================================= */

typedef struct {
    char     dest_room_id[ROOM_ID_LEN]; /* z.B. "ROOM1140" */
    uint8_t  dest_stage;
    int16_t  spawn_x;
    int16_t  spawn_y;
    int16_t  spawn_z;
    int16_t  spawn_angle;
} graph_edge_t;

typedef struct {
    char     id[ROOM_ID_LEN];   /* z.B. "ROOM1170" */
    uint8_t  stage;
    uint8_t  room_hex;
    int      dead_end;          /* 1 wenn keine ausgehenden Kanten */
    int      start_node;        /* 1 für ROOM1240 */
    int      edge_count;
    graph_edge_t edges[MAX_EDGES];
} graph_node_t;

/* ============================================================================
 * Globaler Zustand
 * ========================================================================= */

static graph_node_t g_nodes[MAX_NODES];
static int g_node_count = 0;

/* ============================================================================
 * Hilfsfunktionen
 * ========================================================================= */

/**
 * Erzeugt eine Room-ID-Zeichenkette im Format "ROOM{Stage}{RoomHex:02X}0".
 * Wir verwenden Player 0 als kanonische ID (Räume sind spielerunabhängig).
 */
static void make_room_id(char* out, int stage, int room_hex)
{
    snprintf(out, ROOM_ID_LEN, "ROOM%d%02X0", stage, room_hex);
}

/**
 * Sucht oder erstellt einen Knoten für die gegebene Stage/Room-Kombination.
 * Gibt den Index zurück, oder -1 wenn MAX_NODES erreicht.
 */
static int find_or_create_node(int stage, int room_hex)
{
    char id[ROOM_ID_LEN];
    int i;

    make_room_id(id, stage, room_hex);

    /* Existierenden Knoten suchen */
    for (i = 0; i < g_node_count; i++) {
        if (strcmp(g_nodes[i].id, id) == 0) {
            return i;
        }
    }

    /* Neuen Knoten erstellen */
    if (g_node_count >= MAX_NODES) {
        fprintf(stderr, "[WARN] Maximale Knotenanzahl (%d) erreicht.\n", MAX_NODES);
        return -1;
    }

    memset(&g_nodes[g_node_count], 0, sizeof(graph_node_t));
    strncpy(g_nodes[g_node_count].id, id, ROOM_ID_LEN - 1);
    g_nodes[g_node_count].stage    = (uint8_t)stage;
    g_nodes[g_node_count].room_hex = (uint8_t)room_hex;
    g_nodes[g_node_count].dead_end = 1; /* Wird auf 0 gesetzt wenn Kante hinzukommt */

    /* Start-Knoten prüfen */
    if (stage == START_STAGE && room_hex == START_ROOM_HEX) {
        g_nodes[g_node_count].start_node = 1;
    }

    return g_node_count++;
}

/**
 * Fügt eine Kante zu einem Knoten hinzu. Duplikate werden vermieden
 * (gleiche dest_room + spawn_position = selbe Tür).
 */
static int add_edge(int node_idx, int dest_stage, int dest_room_hex,
                    int16_t spawn_x, int16_t spawn_y, int16_t spawn_z,
                    int16_t spawn_angle)
{
    graph_node_t* node = &g_nodes[node_idx];
    char dest_id[ROOM_ID_LEN];
    int i;

    make_room_id(dest_id, dest_stage, dest_room_hex);

    /* Duplikat-Check: gleiche Ziel-Room + gleiche Spawn-Position */
    for (i = 0; i < node->edge_count; i++) {
        if (strcmp(node->edges[i].dest_room_id, dest_id) == 0 &&
            node->edges[i].spawn_x == spawn_x &&
            node->edges[i].spawn_y == spawn_y &&
            node->edges[i].spawn_z == spawn_z &&
            node->edges[i].spawn_angle == spawn_angle) {
            return 0; /* Bereits vorhanden */
        }
    }

    if (node->edge_count >= MAX_EDGES) {
        fprintf(stderr, "[WARN] Knoten %s: Maximale Kantenanzahl (%d) erreicht.\n",
                node->id, MAX_EDGES);
        return -1;
    }

    /* Kante hinzufügen */
    graph_edge_t* edge = &node->edges[node->edge_count];
    strncpy(edge->dest_room_id, dest_id, ROOM_ID_LEN - 1);
    edge->dest_stage  = (uint8_t)dest_stage;
    edge->spawn_x     = spawn_x;
    edge->spawn_y     = spawn_y;
    edge->spawn_z     = spawn_z;
    edge->spawn_angle = spawn_angle;

    node->edge_count++;
    node->dead_end = 0; /* Hat mindestens eine Kante */

    return 1;
}

/* ============================================================================
 * CSV-Parsing
 * ========================================================================= */

/**
 * Parst die CSV-Ausgabe von extract_doors von einem FILE-Stream.
 *
 * CSV-Spalten:
 *   source_rdt, scd_section, aot_slot, floor_band,
 *   trigger_x0..x3, trigger_z0..z3,
 *   dest_stage, dest_room, dest_cut,
 *   spawn_x, spawn_y, spawn_z, spawn_rotation
 *
 * Wir benötigen: source_rdt (zum Ermitteln von src_stage/src_room),
 * dest_stage, dest_room, spawn_x, spawn_y, spawn_z, spawn_rotation
 */
static int parse_csv(FILE* input)
{
    char line[MAX_LINE_LEN];
    int line_num = 0;
    int doors_parsed = 0;

    while (fgets(line, MAX_LINE_LEN, input)) {
        line_num++;

        /* Header-Zeile überspringen */
        if (line_num == 1 && strncmp(line, "source_rdt", 10) == 0) {
            continue;
        }

        /* Leere Zeilen überspringen */
        if (line[0] == '\n' || line[0] == '\r' || line[0] == '\0') {
            continue;
        }

        /* CSV-Felder parsen */
        char source_rdt[32];
        char scd_section[8];
        int aot_slot, floor_band;
        int trigger_x[4], trigger_z[4];
        int dest_stage, dest_room, dest_cut;
        int spawn_x, spawn_y, spawn_z, spawn_rotation;

        int fields = sscanf(line,
            "%31[^,],%7[^,],%d,%d,"
            "%d,%d,%d,%d,"
            "%d,%d,%d,%d,"
            "%d,%d,%d,"
            "%d,%d,%d,%d",
            source_rdt, scd_section, &aot_slot, &floor_band,
            &trigger_x[0], &trigger_x[1], &trigger_x[2], &trigger_x[3],
            &trigger_z[0], &trigger_z[1], &trigger_z[2], &trigger_z[3],
            &dest_stage, &dest_room, &dest_cut,
            &spawn_x, &spawn_y, &spawn_z, &spawn_rotation);

        if (fields < 19) {
            fprintf(stderr, "[WARN] Zeile %d: Nur %d Felder geparst (19 erwartet). "
                            "Übersprungen.\n", line_num, fields);
            continue;
        }

        /* Quell-Room parsen: Format "ROOM{Stage}{RoomHex:2}{Player}" */
        int src_stage = 0, src_room_hex = 0;
        if (strlen(source_rdt) >= 7 && strncmp(source_rdt, "ROOM", 4) == 0) {
            src_stage = source_rdt[4] - '0';
            /* RoomHex: 2 Zeichen ab Position 5 */
            char hex_buf[3] = { source_rdt[5], source_rdt[6], '\0' };
            src_room_hex = (int)strtol(hex_buf, NULL, 16);
        } else {
            fprintf(stderr, "[WARN] Zeile %d: Ungültiges source_rdt '%s'. "
                            "Übersprungen.\n", line_num, source_rdt);
            continue;
        }

        /* Quell-Knoten finden/erstellen */
        int src_node_idx = find_or_create_node(src_stage, src_room_hex);
        if (src_node_idx < 0) {
            continue;
        }

        /* Validierung: dest_stage muss im gültigen Bereich liegen (0-5 für Stages 1-6).
         * Werte außerhalb sind falsch-positive Treffer des Byte-Scanners. */
        if (dest_stage < 0 || dest_stage > 5) {
            /* Ungültiger dest_stage — wahrscheinlich kein echter Door-Opcode */
            continue;
        }

        /* dest_room muss ebenfalls im gültigen Bereich liegen (0x00-0x27) */
        if (dest_room < 0 || dest_room > 0x27) {
            continue;
        }

        /* Ziel-Knoten erstellen (damit er im Graph erscheint, auch ohne Kanten) */
        /* dest_stage aus CSV ist 0-basiert laut Opcode-Parsing, aber im
         * Room-Graphen nutzen wir 1-basierte Stage-Nummern */
        int actual_dest_stage = dest_stage + 1;
        find_or_create_node(actual_dest_stage, dest_room);

        /* Kante hinzufügen */
        add_edge(src_node_idx, actual_dest_stage, dest_room,
                 (int16_t)spawn_x, (int16_t)spawn_y,
                 (int16_t)spawn_z, (int16_t)spawn_rotation);

        doors_parsed++;
    }

    return doors_parsed;
}

/* ============================================================================
 * JSON-Ausgabe
 * ========================================================================= */

/**
 * Gibt eine JSON-Escape-Version eines Strings aus (einfach für Room-IDs).
 */
static void write_json_string(FILE* out, const char* str)
{
    fprintf(out, "\"%s\"", str);
}

/**
 * Schreibt den Raum-Graphen als JSON-Datei.
 */
static int write_json(const char* path)
{
    FILE* f;
    int i, j;

    f = fopen(path, "w");
    if (!f) {
        fprintf(stderr, "[ERROR] Kann '%s' nicht schreiben.\n", path);
        return -1;
    }

    fprintf(f, "{\n");

    /* Start-Knoten */
    fprintf(f, "  \"start_node\": \"ROOM%d%02X0\",\n", START_STAGE, START_ROOM_HEX);

    /* Knoten-Array */
    fprintf(f, "  \"nodes\": [\n");

    for (i = 0; i < g_node_count; i++) {
        const graph_node_t* node = &g_nodes[i];

        fprintf(f, "    {\n");
        fprintf(f, "      \"id\": ");
        write_json_string(f, node->id);
        fprintf(f, ",\n");
        fprintf(f, "      \"stage\": %d,\n", node->stage);
        fprintf(f, "      \"room_hex\": \"%02X\",\n", node->room_hex);
        fprintf(f, "      \"dead_end\": %s,\n", node->dead_end ? "true" : "false");
        fprintf(f, "      \"start_node\": %s,\n", node->start_node ? "true" : "false");
        fprintf(f, "      \"edges\": [\n");

        for (j = 0; j < node->edge_count; j++) {
            const graph_edge_t* edge = &node->edges[j];

            fprintf(f, "        {\n");
            fprintf(f, "          \"dest_room\": ");
            write_json_string(f, edge->dest_room_id);
            fprintf(f, ",\n");
            fprintf(f, "          \"dest_stage\": %d,\n", edge->dest_stage);
            fprintf(f, "          \"spawn_x\": %d,\n", edge->spawn_x);
            fprintf(f, "          \"spawn_y\": %d,\n", edge->spawn_y);
            fprintf(f, "          \"spawn_z\": %d,\n", edge->spawn_z);
            fprintf(f, "          \"spawn_angle\": %d\n", edge->spawn_angle);
            fprintf(f, "        }%s\n", (j < node->edge_count - 1) ? "," : "");
        }

        fprintf(f, "      ]\n");
        fprintf(f, "    }%s\n", (i < g_node_count - 1) ? "," : "");
    }

    fprintf(f, "  ]\n");
    fprintf(f, "}\n");

    fclose(f);
    return 0;
}

/* ============================================================================
 * Markdown-Ausgabe
 * ========================================================================= */

/**
 * Schreibt den Raum-Graphen als Markdown-Tabelle.
 */
static int write_markdown(const char* path)
{
    FILE* f;
    int i, j;
    int total_edges = 0;
    int dead_ends = 0;

    f = fopen(path, "w");
    if (!f) {
        fprintf(stderr, "[ERROR] Kann '%s' nicht schreiben.\n", path);
        return -1;
    }

    /* Header */
    fprintf(f, "# RE 1.5 Raum-Graph\n\n");
    fprintf(f, "Automatisch generiert aus Door_aot_set_4p-Opcodes aller RDT-Dateien.\n\n");

    /* Zusammenfassung */
    for (i = 0; i < g_node_count; i++) {
        total_edges += g_nodes[i].edge_count;
        if (g_nodes[i].dead_end) dead_ends++;
    }

    fprintf(f, "## Zusammenfassung\n\n");
    fprintf(f, "| Eigenschaft | Wert |\n");
    fprintf(f, "|-------------|------|\n");
    fprintf(f, "| Gesamtanzahl Räume | %d |\n", g_node_count);
    fprintf(f, "| Gesamtanzahl Kanten | %d |\n", total_edges);
    fprintf(f, "| Sackgassen (dead_end) | %d |\n", dead_ends);
    fprintf(f, "| Startknoten | ROOM%d%02X0 |\n\n", START_STAGE, START_ROOM_HEX);

    /* Tabelle pro Stage */
    for (int stage = 1; stage <= 6; stage++) {
        int has_nodes = 0;

        /* Prüfe ob Stage Knoten hat */
        for (i = 0; i < g_node_count; i++) {
            if (g_nodes[i].stage == stage) {
                has_nodes = 1;
                break;
            }
        }

        if (!has_nodes) continue;

        fprintf(f, "## Stage %d\n\n", stage);
        fprintf(f, "| Raum | Flags | Ziel-Room | Stage | Spawn X | Spawn Y | Spawn Z | Winkel |\n");
        fprintf(f, "|------|-------|-----------|-------|---------|---------|---------|--------|\n");

        for (i = 0; i < g_node_count; i++) {
            const graph_node_t* node = &g_nodes[i];
            if (node->stage != stage) continue;

            /* Flags-String */
            char flags[32] = "";
            if (node->start_node) strcat(flags, "START ");
            if (node->dead_end)   strcat(flags, "DEAD_END ");

            if (node->edge_count == 0) {
                /* Raum ohne Kanten */
                fprintf(f, "| %s | %s | — | — | — | — | — | — |\n",
                        node->id, flags);
            } else {
                for (j = 0; j < node->edge_count; j++) {
                    const graph_edge_t* edge = &node->edges[j];

                    if (j == 0) {
                        fprintf(f, "| %s | %s | %s | %d | %d | %d | %d | %d |\n",
                                node->id, flags,
                                edge->dest_room_id, edge->dest_stage,
                                edge->spawn_x, edge->spawn_y,
                                edge->spawn_z, edge->spawn_angle);
                    } else {
                        fprintf(f, "| ↳ | | %s | %d | %d | %d | %d | %d |\n",
                                edge->dest_room_id, edge->dest_stage,
                                edge->spawn_x, edge->spawn_y,
                                edge->spawn_z, edge->spawn_angle);
                    }
                }
            }
        }

        fprintf(f, "\n");
    }

    fclose(f);
    return 0;
}

/* ============================================================================
 * Hauptprogramm
 * ========================================================================= */

static void print_usage(const char* prog)
{
    fprintf(stderr,
        "Nutzung: %s [OPTIONS] [OUTPUT_DIR]\n"
        "\n"
        "Liest Door-CSV von stdin und erzeugt room_graph.json + room_graph.md.\n"
        "\n"
        "Argumente:\n"
        "  OUTPUT_DIR    Ausgabeverzeichnis (Standard: aktuelles Verzeichnis)\n"
        "\n"
        "Optionen:\n"
        "  -h, --help    Diese Hilfe anzeigen\n"
        "\n"
        "Beispiel:\n"
        "  extract_doors /path/to/assets | %s output/\n",
        prog, prog);
}

int main(int argc, char* argv[])
{
    const char* output_dir = ".";
    char json_path[MAX_PATH_LEN];
    char md_path[MAX_PATH_LEN];
    int doors_parsed;

    /* Argumente verarbeiten */
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
            print_usage(argv[0]);
            return 0;
        } else {
            output_dir = argv[i];
        }
    }

    fprintf(stderr, "[INFO] Ausgabeverzeichnis: %s\n", output_dir);
    fprintf(stderr, "[INFO] Lese CSV-Daten von stdin...\n");

    /* CSV von stdin parsen */
    doors_parsed = parse_csv(stdin);

    fprintf(stderr, "[INFO] %d Tür-Datensätze verarbeitet.\n", doors_parsed);
    fprintf(stderr, "[INFO] %d Knoten im Graphen.\n", g_node_count);

    if (g_node_count == 0) {
        fprintf(stderr, "[WARN] Keine Knoten erzeugt. Prüfe die CSV-Eingabe.\n");
        return 2;
    }

    /* Sicherstellen dass der Start-Knoten existiert */
    find_or_create_node(START_STAGE, START_ROOM_HEX);

    /* Ausgabepfade konstruieren */
    snprintf(json_path, MAX_PATH_LEN, "%s/room_graph.json", output_dir);
    snprintf(md_path, MAX_PATH_LEN, "%s/room_graph.md", output_dir);

    /* JSON schreiben */
    fprintf(stderr, "[INFO] Schreibe %s...\n", json_path);
    if (write_json(json_path) != 0) {
        return 1;
    }

    /* Markdown schreiben */
    fprintf(stderr, "[INFO] Schreibe %s...\n", md_path);
    if (write_markdown(md_path) != 0) {
        return 1;
    }

    /* Zusammenfassung */
    int total_edges = 0;
    int dead_ends = 0;
    for (int i = 0; i < g_node_count; i++) {
        total_edges += g_nodes[i].edge_count;
        if (g_nodes[i].dead_end) dead_ends++;
    }

    fprintf(stderr, "\n=== Graph-Generierung abgeschlossen ===\n");
    fprintf(stderr, "  Knoten (Räume):   %d\n", g_node_count);
    fprintf(stderr, "  Kanten (Türen):   %d\n", total_edges);
    fprintf(stderr, "  Sackgassen:       %d\n", dead_ends);
    fprintf(stderr, "  Startknoten:      ROOM%d%02X0\n", START_STAGE, START_ROOM_HEX);
    fprintf(stderr, "  Ausgabe:          %s, %s\n", json_path, md_path);

    return 0;
}
