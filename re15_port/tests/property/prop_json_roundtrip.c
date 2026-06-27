/**
 * @file prop_json_roundtrip.c
 * @brief Property-Test: Strukturdaten JSON-Roundtrip (Raum-Graph)
 *
 * **Property 4: Strukturdaten JSON-Roundtrip**
 *
 * Für jeden gültigen Raum-Graphen soll die JSON-Serialisierung gefolgt von
 * Deserialisierung einen semantisch identischen Datensatz erzeugen
 * (alle Felder mit identischen Werten).
 *
 * Test-Strategie:
 *   1. Generiere zufällige Raum-Graph-Datensätze (Knoten + Kanten)
 *   2. Serialisiere in JSON-String (In-Memory-Buffer)
 *   3. Parse JSON-String zurück in Strukturen
 *   4. Vergleiche alle Felder auf semantische Identität
 *
 * **Validates: Requirements 3.4, 8.5**
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <time.h>

/* =========================================================================
 * Konfiguration
 * ========================================================================= */

#define NUM_ITERATIONS   200
#define MAX_TEST_NODES   8
#define MAX_TEST_EDGES   8
#define ROOM_ID_LEN      16
#define JSON_BUF_SIZE    (64 * 1024)  /* 64 KB buffer for JSON output */

/* =========================================================================
 * Test-Framework
 * ========================================================================= */

static int g_passed = 0;
static int g_failed = 0;

#define PROP_ASSERT(cond, fmt, ...)                                          \
    do {                                                                      \
        if (!(cond)) {                                                        \
            printf("  FAIL: " fmt "\n", ##__VA_ARGS__);                       \
            printf("        at %s:%d\n", __FILE__, __LINE__);                 \
            g_failed++;                                                        \
            return 1;                                                          \
        }                                                                      \
    } while (0)

/* =========================================================================
 * Datenstrukturen (identisch zum Raum-Graph-Format aus generate_graph.c)
 * ========================================================================= */

typedef struct {
    char     dest_room_id[ROOM_ID_LEN]; /* z.B. "ROOM1140" */
    uint8_t  dest_stage;
    int16_t  spawn_x;
    int16_t  spawn_y;
    int16_t  spawn_z;
    int16_t  spawn_angle;
} test_graph_edge_t;

typedef struct {
    char     id[ROOM_ID_LEN];   /* z.B. "ROOM1170" */
    uint8_t  stage;
    uint8_t  room_hex;
    int      dead_end;          /* 0 oder 1 */
    int      start_node;        /* 0 oder 1 */
    int      edge_count;
    test_graph_edge_t edges[MAX_TEST_EDGES];
} test_graph_node_t;

typedef struct {
    char     start_node[ROOM_ID_LEN];
    int      node_count;
    test_graph_node_t nodes[MAX_TEST_NODES];
} test_graph_t;

/* =========================================================================
 * PRNG: xorshift32 (reproduzierbar)
 * ========================================================================= */

static uint32_t xorshift_state;

static uint32_t xorshift32(void)
{
    uint32_t x = xorshift_state;
    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;
    xorshift_state = x;
    return x;
}

static int16_t rand_i16(void) { return (int16_t)(xorshift32() & 0xFFFF); }

/* =========================================================================
 * Zufalls-Graph-Generierung
 * ========================================================================= */

static void make_room_id(char* out, int stage, int room_hex)
{
    snprintf(out, ROOM_ID_LEN, "ROOM%d%02X0", stage, room_hex);
}

static void generate_random_graph(test_graph_t* graph)
{
    int i, j;

    memset(graph, 0, sizeof(test_graph_t));

    /* 1-8 Knoten */
    graph->node_count = 1 + (int)(xorshift32() % MAX_TEST_NODES);

    /* Start-Node ID */
    int start_stage = 1 + (int)(xorshift32() % 6);
    int start_room  = (int)(xorshift32() % 0x28);  /* 0x00-0x27 */
    make_room_id(graph->start_node, start_stage, start_room);

    for (i = 0; i < graph->node_count; i++) {
        test_graph_node_t* node = &graph->nodes[i];

        /* Stage 1-6, Room 0x00-0x27 */
        node->stage    = (uint8_t)(1 + xorshift32() % 6);
        node->room_hex = (uint8_t)(xorshift32() % 0x28);
        make_room_id(node->id, node->stage, node->room_hex);

        node->dead_end   = (xorshift32() & 1) ? 1 : 0;
        node->start_node = (i == 0) ? 1 : 0;

        /* 0-8 Kanten */
        node->edge_count = (int)(xorshift32() % (MAX_TEST_EDGES + 1));
        if (node->edge_count > 0) {
            node->dead_end = 0; /* Hat Kanten → nicht dead_end */
        }

        for (j = 0; j < node->edge_count; j++) {
            test_graph_edge_t* edge = &node->edges[j];

            edge->dest_stage = (uint8_t)(1 + xorshift32() % 6);
            int dest_room_hex = (int)(xorshift32() % 0x28);
            make_room_id(edge->dest_room_id, edge->dest_stage, dest_room_hex);

            edge->spawn_x     = rand_i16();
            edge->spawn_y     = rand_i16();
            edge->spawn_z     = rand_i16();
            edge->spawn_angle = rand_i16();
        }
    }
}

/* =========================================================================
 * JSON-Serialisierung (in-memory buffer)
 *
 * Format identisch zu generate_graph.c write_json():
 * {
 *   "start_node": "ROOM1240",
 *   "nodes": [
 *     {
 *       "id": "ROOM1170",
 *       "stage": 1,
 *       "room_hex": "17",
 *       "dead_end": false,
 *       "start_node": false,
 *       "edges": [
 *         {
 *           "dest_room": "ROOM1140",
 *           "dest_stage": 1,
 *           "spawn_x": -1200,
 *           "spawn_y": 0,
 *           "spawn_z": 3400,
 *           "spawn_angle": 2048
 *         }
 *       ]
 *     }
 *   ]
 * }
 * ========================================================================= */

static int json_serialize(const test_graph_t* graph, char* buf, int buf_size)
{
    int pos = 0;
    int i, j;

#define EMIT(fmt, ...) do {                                             \
    int n = snprintf(buf + pos, buf_size - pos, fmt, ##__VA_ARGS__);   \
    if (n < 0 || pos + n >= buf_size) return -1;                       \
    pos += n;                                                           \
} while(0)

    EMIT("{\n");
    EMIT("  \"start_node\": \"%s\",\n", graph->start_node);
    EMIT("  \"nodes\": [\n");

    for (i = 0; i < graph->node_count; i++) {
        const test_graph_node_t* node = &graph->nodes[i];

        EMIT("    {\n");
        EMIT("      \"id\": \"%s\",\n", node->id);
        EMIT("      \"stage\": %d,\n", node->stage);
        EMIT("      \"room_hex\": \"%02X\",\n", node->room_hex);
        EMIT("      \"dead_end\": %s,\n", node->dead_end ? "true" : "false");
        EMIT("      \"start_node\": %s,\n", node->start_node ? "true" : "false");
        EMIT("      \"edges\": [\n");

        for (j = 0; j < node->edge_count; j++) {
            const test_graph_edge_t* edge = &node->edges[j];

            EMIT("        {\n");
            EMIT("          \"dest_room\": \"%s\",\n", edge->dest_room_id);
            EMIT("          \"dest_stage\": %d,\n", edge->dest_stage);
            EMIT("          \"spawn_x\": %d,\n", edge->spawn_x);
            EMIT("          \"spawn_y\": %d,\n", edge->spawn_y);
            EMIT("          \"spawn_z\": %d,\n", edge->spawn_z);
            EMIT("          \"spawn_angle\": %d\n", edge->spawn_angle);
            EMIT("        }%s\n", (j < node->edge_count - 1) ? "," : "");
        }

        EMIT("      ]\n");
        EMIT("    }%s\n", (i < graph->node_count - 1) ? "," : "");
    }

    EMIT("  ]\n");
    EMIT("}\n");

#undef EMIT

    return pos;
}

/* =========================================================================
 * Minimaler JSON-Parser (handgeschrieben, nur für bekanntes Schema)
 *
 * Strategien:
 *   - Keine allgemeine JSON-Lib, sondern schemabasiertes Parsen
 *   - Whitespace überspringen
 *   - Strings in Anführungszeichen lesen
 *   - Integers parsen (mit Vorzeichen)
 *   - Booleans als true/false
 * ========================================================================= */

static const char* skip_ws(const char* p)
{
    while (*p == ' ' || *p == '\t' || *p == '\n' || *p == '\r') p++;
    return p;
}

static const char* expect_char(const char* p, char c)
{
    p = skip_ws(p);
    if (*p != c) return NULL;
    return p + 1;
}

/**
 * Parse a JSON string value. Copies content into out (without quotes).
 * Returns pointer past closing quote, or NULL on error.
 */
static const char* parse_string(const char* p, char* out, int max_len)
{
    int i = 0;
    p = skip_ws(p);
    if (*p != '"') return NULL;
    p++;
    while (*p && *p != '"') {
        if (i < max_len - 1) out[i++] = *p;
        p++;
    }
    out[i] = '\0';
    if (*p != '"') return NULL;
    return p + 1;
}

/**
 * Parse a JSON integer (with optional negative sign).
 */
static const char* parse_int(const char* p, int* out)
{
    int sign = 1;
    int val = 0;
    p = skip_ws(p);
    if (*p == '-') { sign = -1; p++; }
    if (*p < '0' || *p > '9') return NULL;
    while (*p >= '0' && *p <= '9') {
        val = val * 10 + (*p - '0');
        p++;
    }
    *out = val * sign;
    return p;
}

/**
 * Parse a JSON boolean (true/false).
 */
static const char* parse_bool(const char* p, int* out)
{
    p = skip_ws(p);
    if (strncmp(p, "true", 4) == 0) {
        *out = 1;
        return p + 4;
    } else if (strncmp(p, "false", 5) == 0) {
        *out = 0;
        return p + 5;
    }
    return NULL;
}

/**
 * Skip a JSON key: expects "key":
 */
static const char* skip_key(const char* p, const char* expected_key)
{
    char key[64];
    p = parse_string(p, key, 64);
    if (!p) return NULL;
    if (strcmp(key, expected_key) != 0) return NULL;
    p = expect_char(p, ':');
    return p;
}

/**
 * Skip optional comma.
 */
static const char* skip_comma(const char* p)
{
    p = skip_ws(p);
    if (*p == ',') return p + 1;
    return p;
}

/**
 * Parse a single edge object.
 */
static const char* parse_edge(const char* p, test_graph_edge_t* edge)
{
    int val;
    memset(edge, 0, sizeof(test_graph_edge_t));

    p = expect_char(p, '{');
    if (!p) return NULL;

    /* "dest_room": "..." */
    p = skip_key(p, "dest_room");
    if (!p) return NULL;
    p = parse_string(p, edge->dest_room_id, ROOM_ID_LEN);
    if (!p) return NULL;
    p = skip_comma(p);

    /* "dest_stage": N */
    p = skip_key(p, "dest_stage");
    if (!p) return NULL;
    p = parse_int(p, &val);
    if (!p) return NULL;
    edge->dest_stage = (uint8_t)val;
    p = skip_comma(p);

    /* "spawn_x": N */
    p = skip_key(p, "spawn_x");
    if (!p) return NULL;
    p = parse_int(p, &val);
    if (!p) return NULL;
    edge->spawn_x = (int16_t)val;
    p = skip_comma(p);

    /* "spawn_y": N */
    p = skip_key(p, "spawn_y");
    if (!p) return NULL;
    p = parse_int(p, &val);
    if (!p) return NULL;
    edge->spawn_y = (int16_t)val;
    p = skip_comma(p);

    /* "spawn_z": N */
    p = skip_key(p, "spawn_z");
    if (!p) return NULL;
    p = parse_int(p, &val);
    if (!p) return NULL;
    edge->spawn_z = (int16_t)val;
    p = skip_comma(p);

    /* "spawn_angle": N */
    p = skip_key(p, "spawn_angle");
    if (!p) return NULL;
    p = parse_int(p, &val);
    if (!p) return NULL;
    edge->spawn_angle = (int16_t)val;

    p = skip_ws(p);
    p = expect_char(p, '}');
    return p;
}

/**
 * Parse a single node object.
 */
static const char* parse_node(const char* p, test_graph_node_t* node)
{
    int val;
    char hex_str[8];
    memset(node, 0, sizeof(test_graph_node_t));

    p = expect_char(p, '{');
    if (!p) return NULL;

    /* "id": "..." */
    p = skip_key(p, "id");
    if (!p) return NULL;
    p = parse_string(p, node->id, ROOM_ID_LEN);
    if (!p) return NULL;
    p = skip_comma(p);

    /* "stage": N */
    p = skip_key(p, "stage");
    if (!p) return NULL;
    p = parse_int(p, &val);
    if (!p) return NULL;
    node->stage = (uint8_t)val;
    p = skip_comma(p);

    /* "room_hex": "XX" */
    p = skip_key(p, "room_hex");
    if (!p) return NULL;
    p = parse_string(p, hex_str, 8);
    if (!p) return NULL;
    node->room_hex = (uint8_t)strtol(hex_str, NULL, 16);
    p = skip_comma(p);

    /* "dead_end": true/false */
    p = skip_key(p, "dead_end");
    if (!p) return NULL;
    p = parse_bool(p, &node->dead_end);
    if (!p) return NULL;
    p = skip_comma(p);

    /* "start_node": true/false */
    p = skip_key(p, "start_node");
    if (!p) return NULL;
    p = parse_bool(p, &node->start_node);
    if (!p) return NULL;
    p = skip_comma(p);

    /* "edges": [...] */
    p = skip_key(p, "edges");
    if (!p) return NULL;
    p = expect_char(p, '[');
    if (!p) return NULL;

    /* Parse edges array */
    node->edge_count = 0;
    p = skip_ws(p);
    while (*p != ']') {
        if (node->edge_count >= MAX_TEST_EDGES) return NULL;
        p = parse_edge(p, &node->edges[node->edge_count]);
        if (!p) return NULL;
        node->edge_count++;
        p = skip_comma(p);
        p = skip_ws(p);
    }
    p = expect_char(p, ']');  /* consume ']' */
    if (!p) return NULL;

    p = skip_ws(p);
    p = expect_char(p, '}');
    return p;
}

/**
 * Parse the full graph JSON.
 */
static int json_deserialize(const char* json, test_graph_t* graph)
{
    const char* p = json;
    memset(graph, 0, sizeof(test_graph_t));

    p = expect_char(p, '{');
    if (!p) return -1;

    /* "start_node": "..." */
    p = skip_key(p, "start_node");
    if (!p) return -1;
    p = parse_string(p, graph->start_node, ROOM_ID_LEN);
    if (!p) return -1;
    p = skip_comma(p);

    /* "nodes": [...] */
    p = skip_key(p, "nodes");
    if (!p) return -1;
    p = expect_char(p, '[');
    if (!p) return -1;

    graph->node_count = 0;
    p = skip_ws(p);
    while (*p != ']') {
        if (graph->node_count >= MAX_TEST_NODES) return -1;
        p = parse_node(p, &graph->nodes[graph->node_count]);
        if (!p) return -1;
        graph->node_count++;
        p = skip_comma(p);
        p = skip_ws(p);
    }
    p = expect_char(p, ']');
    if (!p) return -1;

    p = skip_ws(p);
    p = expect_char(p, '}');
    if (!p) return -1;

    return 0;
}

/* =========================================================================
 * Vergleichsfunktion: Semantische Identität
 * ========================================================================= */

static int compare_edges(const test_graph_edge_t* a, const test_graph_edge_t* b,
                         int node_idx, int edge_idx)
{
    if (strcmp(a->dest_room_id, b->dest_room_id) != 0) {
        printf("    Node[%d].Edge[%d].dest_room: '%s' vs '%s'\n",
               node_idx, edge_idx, a->dest_room_id, b->dest_room_id);
        return 1;
    }
    if (a->dest_stage != b->dest_stage) {
        printf("    Node[%d].Edge[%d].dest_stage: %d vs %d\n",
               node_idx, edge_idx, a->dest_stage, b->dest_stage);
        return 1;
    }
    if (a->spawn_x != b->spawn_x) {
        printf("    Node[%d].Edge[%d].spawn_x: %d vs %d\n",
               node_idx, edge_idx, a->spawn_x, b->spawn_x);
        return 1;
    }
    if (a->spawn_y != b->spawn_y) {
        printf("    Node[%d].Edge[%d].spawn_y: %d vs %d\n",
               node_idx, edge_idx, a->spawn_y, b->spawn_y);
        return 1;
    }
    if (a->spawn_z != b->spawn_z) {
        printf("    Node[%d].Edge[%d].spawn_z: %d vs %d\n",
               node_idx, edge_idx, a->spawn_z, b->spawn_z);
        return 1;
    }
    if (a->spawn_angle != b->spawn_angle) {
        printf("    Node[%d].Edge[%d].spawn_angle: %d vs %d\n",
               node_idx, edge_idx, a->spawn_angle, b->spawn_angle);
        return 1;
    }
    return 0;
}

static int compare_nodes(const test_graph_node_t* a, const test_graph_node_t* b,
                         int node_idx)
{
    int j;

    if (strcmp(a->id, b->id) != 0) {
        printf("    Node[%d].id: '%s' vs '%s'\n", node_idx, a->id, b->id);
        return 1;
    }
    if (a->stage != b->stage) {
        printf("    Node[%d].stage: %d vs %d\n", node_idx, a->stage, b->stage);
        return 1;
    }
    if (a->room_hex != b->room_hex) {
        printf("    Node[%d].room_hex: 0x%02X vs 0x%02X\n",
               node_idx, a->room_hex, b->room_hex);
        return 1;
    }
    if (a->dead_end != b->dead_end) {
        printf("    Node[%d].dead_end: %d vs %d\n",
               node_idx, a->dead_end, b->dead_end);
        return 1;
    }
    if (a->start_node != b->start_node) {
        printf("    Node[%d].start_node: %d vs %d\n",
               node_idx, a->start_node, b->start_node);
        return 1;
    }
    if (a->edge_count != b->edge_count) {
        printf("    Node[%d].edge_count: %d vs %d\n",
               node_idx, a->edge_count, b->edge_count);
        return 1;
    }
    for (j = 0; j < a->edge_count; j++) {
        if (compare_edges(&a->edges[j], &b->edges[j], node_idx, j) != 0) {
            return 1;
        }
    }
    return 0;
}

static int compare_graphs(const test_graph_t* a, const test_graph_t* b, int iter)
{
    int i;

    if (strcmp(a->start_node, b->start_node) != 0) {
        printf("  Iteration %d: start_node mismatch: '%s' vs '%s'\n",
               iter, a->start_node, b->start_node);
        return 1;
    }
    if (a->node_count != b->node_count) {
        printf("  Iteration %d: node_count mismatch: %d vs %d\n",
               iter, a->node_count, b->node_count);
        return 1;
    }
    for (i = 0; i < a->node_count; i++) {
        if (compare_nodes(&a->nodes[i], &b->nodes[i], i) != 0) {
            printf("  Iteration %d: mismatch at node %d\n", iter, i);
            return 1;
        }
    }
    return 0;
}

/* =========================================================================
 * Property-Test 1: Serialize → Deserialize → Identisch
 * ========================================================================= */

static int prop_json_roundtrip(void)
{
    static char json_buf[JSON_BUF_SIZE];
    test_graph_t original;
    test_graph_t parsed;
    int i;

    for (i = 0; i < NUM_ITERATIONS; i++) {
        generate_random_graph(&original);

        /* Serialize */
        int json_len = json_serialize(&original, json_buf, JSON_BUF_SIZE);
        PROP_ASSERT(json_len > 0,
            "Iteration %d: Serialisierung fehlgeschlagen (Buffer zu klein?)", i);

        /* Deserialize */
        int parse_result = json_deserialize(json_buf, &parsed);
        PROP_ASSERT(parse_result == 0,
            "Iteration %d: Deserialisierung fehlgeschlagen", i);

        /* Compare */
        int cmp = compare_graphs(&original, &parsed, i);
        PROP_ASSERT(cmp == 0,
            "Iteration %d: Roundtrip-Vergleich fehlgeschlagen", i);
    }

    g_passed++;
    return 0;
}

/* =========================================================================
 * Property-Test 2: Deserialisierung erzeugt immer gültige Wertebereiche
 *
 * Invarianten nach dem Roundtrip:
 *   - stage ist im Bereich [1, 6]
 *   - room_hex ist im Bereich [0x00, 0x27]
 *   - dead_end ist 0 oder 1
 *   - start_node ist 0 oder 1
 *   - edge_count >= 0 und <= MAX_TEST_EDGES
 *   - dest_stage ist im Bereich [1, 6]
 * ========================================================================= */

static int prop_value_ranges(void)
{
    static char json_buf[JSON_BUF_SIZE];
    test_graph_t original;
    test_graph_t parsed;
    int i, j, k;

    for (i = 0; i < NUM_ITERATIONS; i++) {
        generate_random_graph(&original);

        int json_len = json_serialize(&original, json_buf, JSON_BUF_SIZE);
        PROP_ASSERT(json_len > 0,
            "Iteration %d: Serialisierung fehlgeschlagen", i);

        int parse_result = json_deserialize(json_buf, &parsed);
        PROP_ASSERT(parse_result == 0,
            "Iteration %d: Deserialisierung fehlgeschlagen", i);

        PROP_ASSERT(parsed.node_count >= 1 && parsed.node_count <= MAX_TEST_NODES,
            "Iteration %d: node_count=%d out of range", i, parsed.node_count);

        for (j = 0; j < parsed.node_count; j++) {
            const test_graph_node_t* node = &parsed.nodes[j];

            PROP_ASSERT(node->stage >= 1 && node->stage <= 6,
                "Iteration %d, Node %d: stage=%d out of range",
                i, j, node->stage);

            PROP_ASSERT(node->room_hex <= 0x27,
                "Iteration %d, Node %d: room_hex=0x%02X out of range",
                i, j, node->room_hex);

            PROP_ASSERT(node->dead_end == 0 || node->dead_end == 1,
                "Iteration %d, Node %d: dead_end=%d invalid",
                i, j, node->dead_end);

            PROP_ASSERT(node->start_node == 0 || node->start_node == 1,
                "Iteration %d, Node %d: start_node=%d invalid",
                i, j, node->start_node);

            PROP_ASSERT(node->edge_count >= 0 && node->edge_count <= MAX_TEST_EDGES,
                "Iteration %d, Node %d: edge_count=%d out of range",
                i, j, node->edge_count);

            for (k = 0; k < node->edge_count; k++) {
                const test_graph_edge_t* edge = &node->edges[k];

                PROP_ASSERT(edge->dest_stage >= 1 && edge->dest_stage <= 6,
                    "Iteration %d, Node %d, Edge %d: dest_stage=%d out of range",
                    i, j, k, edge->dest_stage);

                /* dest_room_id muss mit "ROOM" beginnen */
                PROP_ASSERT(strncmp(edge->dest_room_id, "ROOM", 4) == 0,
                    "Iteration %d, Node %d, Edge %d: dest_room_id='%s' invalid prefix",
                    i, j, k, edge->dest_room_id);
            }
        }
    }

    g_passed++;
    return 0;
}

/* =========================================================================
 * Main — Test-Runner
 * ========================================================================= */

int main(int argc, char** argv)
{
    (void)argc;
    (void)argv;

    /* Seed: Nutze Zeit für Varianz, drucke Seed für Reproduzierbarkeit */
    xorshift_state = (uint32_t)time(NULL);
    if (xorshift_state == 0) xorshift_state = 1; /* xorshift darf nicht 0 sein */

    printf("=== Property-Test: JSON-Roundtrip (Raum-Graph) ===\n");
    printf("    Validates: Requirements 3.4, 8.5\n");
    printf("    PRNG Seed: %u\n", xorshift_state);
    printf("    Iterationen: %d\n\n", NUM_ITERATIONS);

    /* Test 1: Roundtrip-Identität */
    printf("[1/2] prop_json_roundtrip (%d Iterationen) ... ", NUM_ITERATIONS);
    if (prop_json_roundtrip() == 0) {
        printf("PASS\n");
    } else {
        printf("FAIL\n");
    }

    /* Test 2: Wertebereiche nach Roundtrip */
    printf("[2/2] prop_value_ranges (%d Iterationen) ... ", NUM_ITERATIONS);
    if (prop_value_ranges() == 0) {
        printf("PASS\n");
    } else {
        printf("FAIL\n");
    }

    /* Summary */
    printf("\n=== Ergebnis: %d bestanden, %d fehlgeschlagen ===\n",
           g_passed, g_failed);

    return (g_failed > 0) ? 1 : 0;
}
