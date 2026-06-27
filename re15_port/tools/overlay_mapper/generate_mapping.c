/**
 * generate_mapping.c — Mapping-Tabelle generieren (JSON + Markdown)
 *
 * Post-processes the JSON output from overlay_mapper (Task 19.1):
 *   1. Cross-references against RE15_FUN_CATALOG.md (address matching)
 *   2. Generates proposed names based on jump-table index → room-ID
 *   3. Assigns confidence levels (high/medium/low)
 *   4. Outputs overlay_mapping.json and overlay_mapping.md
 *
 * Usage:
 *   overlay_mapper <dir> | generate_mapping [-c catalog.md] [-o output_dir]
 *
 * Confidence levels:
 *   high:   Jump-table index match + RE15_FUN_CATALOG.md entry
 *   medium: Jump-table index present, no catalog entry
 *   low:    Address in overlay, but no jump-table reference
 *
 * Requirements: 8.2, 8.3, 8.4, 8.5, 8.6
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <ctype.h>

/* ======================================================================
 * Configuration
 * ====================================================================== */

#define MAX_LINE_LEN       8192
#define MAX_FUNCTIONS      2048
#define MAX_CATALOG        512
#define MAX_STAGES         6
#define MAX_PATH_LEN       512
#define MAX_NAME_LEN       128
#define MAX_JT_ENTRIES     64
#define MAX_JUMP_TABLES    256

/* ======================================================================
 * Data Structures
 * ====================================================================== */

/** Confidence level for a mapping entry */
typedef enum {
    CONF_HIGH   = 0,  /* Jump-table + catalog match */
    CONF_MEDIUM = 1,  /* Jump-table index, no catalog */
    CONF_LOW    = 2   /* In overlay, no jump-table ref */
} confidence_t;

/** A single mapping entry (final output) */
typedef struct {
    uint32_t address;
    int      stage;
    char     room_id[16];           /* Hex room ID or "unresolved" */
    char     proposed_name[MAX_NAME_LEN];
    char     catalog_name[MAX_NAME_LEN];
    confidence_t confidence;
    int      jt_index;              /* Jump-table index (-1 if none) */
} mapping_entry_t;

/** A catalog entry from RE15_FUN_CATALOG.md */
typedef struct {
    uint32_t address;
    char     purpose[256];
    char     subsystem[64];
    char     confidence_str[16];
} catalog_entry_t;

/** Jump-table entry parsed from overlay_mapper JSON */
typedef struct {
    int      room_index;
    uint32_t pointer_location;
} jt_parsed_entry_t;

/** Jump table parsed from overlay_mapper JSON */
typedef struct {
    uint32_t base_address;
    int      max_index;
    int      index_offset;
    char     containing_func[64];
    int      entry_count;
    jt_parsed_entry_t entries[MAX_JT_ENTRIES];
} jt_parsed_t;

/** Function parsed from overlay_mapper JSON */
typedef struct {
    uint32_t address;
    char     name[64];
} func_parsed_t;

/** Per-stage data parsed from overlay_mapper JSON */
typedef struct {
    int stage;
    int func_count;
    func_parsed_t functions[MAX_FUNCTIONS];
    int jt_count;
    jt_parsed_t jump_tables[MAX_JUMP_TABLES];
} stage_parsed_t;

/** Global state */
static stage_parsed_t g_stages[MAX_STAGES];
static int g_stage_count = 0;

static catalog_entry_t g_catalog[MAX_CATALOG];
static int g_catalog_count = 0;

static mapping_entry_t g_mappings[MAX_FUNCTIONS * MAX_STAGES];
static int g_mapping_count = 0;

/* ======================================================================
 * Utility
 * ====================================================================== */

static uint32_t parse_hex_string(const char *s)
{
    uint32_t val = 0;
    if (s[0] == '0' && (s[1] == 'x' || s[1] == 'X')) s += 2;
    while (isxdigit((unsigned char)*s)) {
        char c = *s;
        uint32_t nibble;
        if (c >= '0' && c <= '9')      nibble = c - '0';
        else if (c >= 'a' && c <= 'f') nibble = c - 'a' + 10;
        else                            nibble = c - 'A' + 10;
        val = (val << 4) | nibble;
        s++;
    }
    return val;
}

/** Extract a JSON string value for a given key from a line */
static int json_extract_string(const char *line, const char *key, char *out, int max_len)
{
    char pattern[128];
    snprintf(pattern, sizeof(pattern), "\"%s\":", key);
    const char *pos = strstr(line, pattern);
    if (!pos) return 0;
    pos += strlen(pattern);
    while (*pos == ' ') pos++;
    if (*pos != '"') return 0;
    pos++; /* skip opening quote */
    int i = 0;
    while (*pos && *pos != '"' && i < max_len - 1) {
        out[i++] = *pos++;
    }
    out[i] = '\0';
    return 1;
}

/** Extract a JSON integer value for a given key from a line */
static int json_extract_int(const char *line, const char *key, int *out)
{
    char pattern[128];
    snprintf(pattern, sizeof(pattern), "\"%s\":", key);
    const char *pos = strstr(line, pattern);
    if (!pos) return 0;
    pos += strlen(pattern);
    while (*pos == ' ') pos++;
    if (*pos == '-' || isdigit((unsigned char)*pos)) {
        *out = atoi(pos);
        return 1;
    }
    return 0;
}

/* ======================================================================
 * Catalog Parser (RE15_FUN_CATALOG.md)
 * ====================================================================== */

/**
 * Parse RE15_FUN_CATALOG.md to extract address → purpose mappings.
 * Format: Markdown tables with | Addr | Purpose | Evidence | Conf |
 * Addresses can be: FUN_80xxxxxx, DAT_80xxxxxx, LAB_80xxxxxx, 0x80xxxxxx
 */
static int load_catalog(const char *filepath)
{
    FILE *fp = fopen(filepath, "r");
    if (!fp) {
        fprintf(stderr, "[RE15 WARN] generate_mapping: Cannot open catalog '%s'\n", filepath);
        return -1;
    }

    char line[MAX_LINE_LEN];
    char current_section[128] = "";
    g_catalog_count = 0;

    while (fgets(line, sizeof(line), fp) != NULL) {
        /* Track section headers */
        if (line[0] == '#' && line[1] == '#' && line[2] == ' ') {
            const char *sec = line + 3;
            int len = (int)strlen(sec);
            if (len > 0 && sec[len - 1] == '\n') len--;
            if (len >= (int)sizeof(current_section)) len = (int)sizeof(current_section) - 1;
            memcpy(current_section, sec, len);
            current_section[len] = '\0';
            continue;
        }

        /* Skip non-table lines */
        if (line[0] != '|') continue;
        /* Skip header/separator rows */
        if (strstr(line, "----") || strstr(line, "Addr")) continue;

        /* Parse table row: | Addr | Purpose | Evidence | Conf | */
        const char *col1 = strchr(line + 1, '|');  /* end of addr col */
        if (!col1) continue;

        /* Extract address from first column */
        char addr_str[64] = "";
        const char *a = line + 1;
        while (*a == ' ') a++;
        int ai = 0;
        while (a < col1 && ai < 63) {
            if (*a != ' ' && *a != '*') addr_str[ai++] = *a;
            a++;
        }
        addr_str[ai] = '\0';

        /* Parse the hex address */
        uint32_t addr = 0;
        if (strstr(addr_str, "FUN_") || strstr(addr_str, "DAT_") ||
            strstr(addr_str, "LAB_")) {
            const char *hex = strchr(addr_str, '_');
            if (hex) addr = parse_hex_string(hex + 1);
        } else if (addr_str[0] == '0' && addr_str[1] == 'x') {
            addr = parse_hex_string(addr_str);
        } else {
            /* Try as plain hex */
            int all_hex = 1;
            for (int i = 0; addr_str[i]; i++) {
                if (!isxdigit((unsigned char)addr_str[i])) { all_hex = 0; break; }
            }
            if (all_hex && ai >= 7) addr = parse_hex_string(addr_str);
        }

        if (addr == 0) continue;  /* Skip unparseable */

        /* Only care about overlay range (0x80100000+) for this tool */
        /* But also store main-exe addresses for completeness */

        /* Extract purpose from second column */
        const char *col2 = strchr(col1 + 1, '|');
        if (!col2) continue;

        char purpose[256] = "";
        const char *p = col1 + 1;
        while (*p == ' ') p++;
        int pi = 0;
        while (p < col2 && pi < 255) {
            purpose[pi++] = *p++;
        }
        purpose[pi] = '\0';
        /* Trim trailing spaces */
        while (pi > 0 && purpose[pi - 1] == ' ') purpose[--pi] = '\0';

        /* Extract confidence from last column */
        char conf[16] = "med";
        const char *last_pipe = strrchr(col2 + 1, '|');
        if (last_pipe) {
            /* Second-to-last pipe before the trailing one */
            const char *conf_start = NULL;
            const char *scan = col2 + 1;
            const char *prev_pipe = NULL;
            while (scan < last_pipe) {
                if (*scan == '|') prev_pipe = scan;
                scan++;
            }
            if (prev_pipe) conf_start = prev_pipe + 1;
            else conf_start = col2 + 1;
            while (*conf_start == ' ') conf_start++;
            int ci = 0;
            while (conf_start < last_pipe && ci < 15 && *conf_start != ' ') {
                conf[ci++] = *conf_start++;
            }
            conf[ci] = '\0';
        }

        if (g_catalog_count < MAX_CATALOG) {
            catalog_entry_t *e = &g_catalog[g_catalog_count];
            e->address = addr;
            strncpy(e->purpose, purpose, sizeof(e->purpose) - 1);
            e->purpose[sizeof(e->purpose) - 1] = '\0';
            strncpy(e->subsystem, current_section, sizeof(e->subsystem) - 1);
            e->subsystem[sizeof(e->subsystem) - 1] = '\0';
            strncpy(e->confidence_str, conf, sizeof(e->confidence_str) - 1);
            e->confidence_str[sizeof(e->confidence_str) - 1] = '\0';
            g_catalog_count++;
        }
    }

    fclose(fp);
    fprintf(stderr, "[RE15 INFO] generate_mapping: Loaded %d catalog entries\n", g_catalog_count);
    return 0;
}

/** Look up an address in the catalog. Returns pointer or NULL. */
static const catalog_entry_t *catalog_lookup(uint32_t addr)
{
    for (int i = 0; i < g_catalog_count; i++) {
        if (g_catalog[i].address == addr) return &g_catalog[i];
    }
    return NULL;
}

/* ======================================================================
 * JSON Input Parser (overlay_mapper output)
 * ====================================================================== */

/**
 * Minimal JSON parser for the overlay_mapper output.
 * Reads the entire stdin into memory and parses the stage array.
 */
static char *read_all_stdin(int *out_len)
{
    int capacity = 65536;
    char *buf = (char *)malloc(capacity);
    if (!buf) return NULL;
    int len = 0;

    while (!feof(stdin)) {
        int chunk = (int)fread(buf + len, 1, capacity - len - 1, stdin);
        if (chunk <= 0) break;
        len += chunk;
        if (len >= capacity - 1) {
            capacity *= 2;
            char *new_buf = (char *)realloc(buf, capacity);
            if (!new_buf) { free(buf); return NULL; }
            buf = new_buf;
        }
    }
    buf[len] = '\0';
    *out_len = len;
    return buf;
}

/**
 * Simple state-machine parser for overlay_mapper JSON output.
 * The format is well-known: array of stage objects.
 */
static int parse_overlay_json(const char *json)
{
    /* Process line by line */
    const char *p = json;
    char line[MAX_LINE_LEN];
    int current_stage_idx = -1;
    int in_functions = 0;
    int in_jump_tables = 0;
    int in_entries = 0;
    int current_jt_idx = -1;

    g_stage_count = 0;

    while (*p) {
        /* Read a line */
        int li = 0;
        while (*p && *p != '\n' && li < MAX_LINE_LEN - 1) {
            line[li++] = *p++;
        }
        line[li] = '\0';
        if (*p == '\n') p++;

        /* Detect stage object start */
        int stage_num;
        if (json_extract_int(line, "stage", &stage_num)) {
            if (current_stage_idx < MAX_STAGES - 1) {
                current_stage_idx = g_stage_count++;
                g_stages[current_stage_idx].stage = stage_num;
                g_stages[current_stage_idx].func_count = 0;
                g_stages[current_stage_idx].jt_count = 0;
            }
            in_functions = 0;
            in_jump_tables = 0;
            in_entries = 0;
            continue;
        }

        /* Detect arrays */
        if (strstr(line, "\"functions\":")) { in_functions = 1; in_jump_tables = 0; in_entries = 0; continue; }
        if (strstr(line, "\"jump_tables\":")) { in_jump_tables = 1; in_functions = 0; in_entries = 0; continue; }
        if (strstr(line, "\"entries\":")) { in_entries = 1; continue; }

        /* Detect array end */
        if (strstr(line, "]") && !strstr(line, "[")) {
            if (in_entries) { in_entries = 0; continue; }
            if (in_functions) { in_functions = 0; continue; }
            if (in_jump_tables) { in_jump_tables = 0; continue; }
        }

        if (current_stage_idx < 0) continue;
        stage_parsed_t *st = &g_stages[current_stage_idx];

        /* Parse function entries */
        if (in_functions && strstr(line, "\"address\"")) {
            char addr_s[32] = "";
            char name_s[64] = "";
            json_extract_string(line, "address", addr_s, sizeof(addr_s));
            json_extract_string(line, "name", name_s, sizeof(name_s));
            if (addr_s[0] && st->func_count < MAX_FUNCTIONS) {
                func_parsed_t *f = &st->functions[st->func_count];
                f->address = parse_hex_string(addr_s);
                strncpy(f->name, name_s, sizeof(f->name) - 1);
                f->name[sizeof(f->name) - 1] = '\0';
                st->func_count++;
            }
            continue;
        }

        /* Parse jump table header fields */
        if (in_jump_tables && !in_entries) {
            if (strstr(line, "\"base_address\"")) {
                char ba[32] = "";
                json_extract_string(line, "base_address", ba, sizeof(ba));
                if (ba[0] && st->jt_count < MAX_JUMP_TABLES) {
                    current_jt_idx = st->jt_count;
                    jt_parsed_t *jt = &st->jump_tables[current_jt_idx];
                    jt->base_address = parse_hex_string(ba);
                    jt->entry_count = 0;
                    jt->max_index = -1;
                    jt->index_offset = -1;
                    jt->containing_func[0] = '\0';
                    st->jt_count++;
                }
            }
            int val;
            if (json_extract_int(line, "max_index", &val) && current_jt_idx >= 0) {
                st->jump_tables[current_jt_idx].max_index = val;
            }
            if (json_extract_int(line, "index_offset_byte", &val) && current_jt_idx >= 0) {
                st->jump_tables[current_jt_idx].index_offset = val;
            }
            char cf[64] = "";
            if (json_extract_string(line, "containing_function", cf, sizeof(cf)) && current_jt_idx >= 0) {
                strncpy(st->jump_tables[current_jt_idx].containing_func, cf, 63);
            }
            continue;
        }

        /* Parse jump table entries */
        if (in_entries && current_jt_idx >= 0 && strstr(line, "\"room_index\"")) {
            jt_parsed_t *jt = &st->jump_tables[current_jt_idx];
            if (jt->entry_count < MAX_JT_ENTRIES) {
                int ri = 0;
                json_extract_int(line, "room_index", &ri);
                char pl[32] = "";
                json_extract_string(line, "pointer_location", pl, sizeof(pl));
                jt_parsed_entry_t *e = &jt->entries[jt->entry_count];
                e->room_index = ri;
                e->pointer_location = parse_hex_string(pl);
                jt->entry_count++;
            }
            continue;
        }
    }

    fprintf(stderr, "[RE15 INFO] generate_mapping: Parsed %d stages from overlay_mapper JSON\n",
            g_stage_count);
    return 0;
}

/* ======================================================================
 * Mapping Generation
 * ====================================================================== */

/**
 * Determine if a function address appears in any jump table for its stage.
 * Checks both direct pointer_location match AND address-proximity heuristic.
 *
 * Heuristic: If a jump table has max_index entries, the functions listed
 * immediately after the dispatch function in address order are likely targets.
 * We also check if the function address is referenced by jump-table pointers.
 *
 * Returns the room index if found, or -1.
 */
static int find_jt_room_index(const stage_parsed_t *st, uint32_t func_addr)
{
    /* Direct match against pointer locations (overlay_mapper resolved entries) */
    for (int j = 0; j < st->jt_count; j++) {
        const jt_parsed_t *jt = &st->jump_tables[j];
        for (int e = 0; e < jt->entry_count; e++) {
            if (jt->entries[e].pointer_location == func_addr) {
                return jt->entries[e].room_index;
            }
        }
    }
    return -1;
}

/**
 * Address-proximity-based room assignment.
 *
 * For jump tables with known max_index, find the dispatch function in the
 * function list, then assign the NEXT max_index functions as room init/update
 * targets (they follow the dispatcher in address order).
 *
 * This populates a lookup table: func_addr → room_index.
 */
#define MAX_ADDR_MAP 4096

typedef struct {
    uint32_t addr;
    int      room_index;
} addr_room_map_t;

static addr_room_map_t g_addr_map[MAX_ADDR_MAP];
static int g_addr_map_count = 0;

static void build_address_proximity_map(void)
{
    g_addr_map_count = 0;

    for (int s = 0; s < g_stage_count; s++) {
        const stage_parsed_t *st = &g_stages[s];

        for (int j = 0; j < st->jt_count; j++) {
            const jt_parsed_t *jt = &st->jump_tables[j];
            if (jt->max_index <= 0) continue;
            if (jt->containing_func[0] == '\0') continue;

            /* Find the dispatch function's index in the function list */
            int disp_idx = -1;
            for (int f = 0; f < st->func_count; f++) {
                if (strcmp(st->functions[f].name, jt->containing_func) == 0) {
                    disp_idx = f;
                    break;
                }
            }
            if (disp_idx < 0) continue;

            /* Assign subsequent functions as room targets.
             * Each room typically gets 2 functions (init + update),
             * so we assign pairs if available. */
            int assigned = 0;
            int room = 0;
            for (int f = disp_idx + 1; f < st->func_count && room < jt->max_index; f++) {
                if (g_addr_map_count >= MAX_ADDR_MAP) break;
                g_addr_map[g_addr_map_count].addr = st->functions[f].address;
                g_addr_map[g_addr_map_count].room_index = room;
                g_addr_map_count++;
                assigned++;
                /* Heuristic: alternate init/update per room
                 * (every 2 functions = one room) */
                if (assigned % 2 == 0) room++;
            }
        }
    }
}

/**
 * Look up a function address in the proximity map.
 * Returns room index or -1.
 */
static int find_proximity_room_index(uint32_t func_addr)
{
    for (int i = 0; i < g_addr_map_count; i++) {
        if (g_addr_map[i].addr == func_addr) {
            return g_addr_map[i].room_index;
        }
    }
    return -1;
}

/**
 * Generate a proposed function name from stage and room index.
 * Format: room{Stage}{RoomHex}_init or room{Stage}{RoomHex}_update
 *
 * Heuristic: the first function for a given room is _init,
 * subsequent ones are _update.
 */
static void generate_proposed_name(int stage, int room_index, int func_ordinal,
                                   char *out, int max_len)
{
    const char *suffix = (func_ordinal == 0) ? "init" : "update";
    snprintf(out, max_len, "room%d%02x_%s", stage, room_index, suffix);
}

/**
 * Build the complete mapping table from parsed data.
 */
static void build_mapping(void)
{
    g_mapping_count = 0;

    /* Build the address-proximity map for room assignment */
    build_address_proximity_map();

    for (int s = 0; s < g_stage_count; s++) {
        const stage_parsed_t *st = &g_stages[s];

        /* Track how many functions we've seen per room (for init/update naming) */
        int room_func_count[256];
        memset(room_func_count, 0, sizeof(room_func_count));

        for (int f = 0; f < st->func_count; f++) {
            const func_parsed_t *fn = &st->functions[f];
            mapping_entry_t *m = &g_mappings[g_mapping_count];

            m->address = fn->address;
            m->stage = st->stage;
            m->jt_index = -1;
            m->proposed_name[0] = '\0';
            m->catalog_name[0] = '\0';
            m->confidence = CONF_LOW;
            strcpy(m->room_id, "unresolved");

            /* Try to find this function in a jump table (direct match) */
            int room_idx = find_jt_room_index(st, fn->address);

            /* If no direct match, try proximity heuristic */
            if (room_idx < 0) {
                room_idx = find_proximity_room_index(fn->address);
            }

            if (room_idx >= 0) {
                m->jt_index = room_idx;
                snprintf(m->room_id, sizeof(m->room_id), "%02x", room_idx);

                /* Generate proposed name */
                int ordinal = room_func_count[room_idx]++;
                generate_proposed_name(st->stage, room_idx, ordinal,
                                       m->proposed_name, sizeof(m->proposed_name));
                m->confidence = CONF_MEDIUM;
            }

            /* Cross-reference with catalog */
            const catalog_entry_t *cat = catalog_lookup(fn->address);
            if (cat) {
                strncpy(m->catalog_name, cat->purpose, sizeof(m->catalog_name) - 1);
                m->catalog_name[sizeof(m->catalog_name) - 1] = '\0';
                /* If we also have a jump-table match, confidence is high */
                if (m->jt_index >= 0) {
                    m->confidence = CONF_HIGH;
                } else {
                    /* Catalog match but no JT → medium (known function but unsure of room) */
                    m->confidence = CONF_MEDIUM;
                }
            }

            g_mapping_count++;
        }
    }

    fprintf(stderr, "[RE15 INFO] generate_mapping: Generated %d mapping entries\n",
            g_mapping_count);
}

/* ======================================================================
 * Output: JSON
 * ====================================================================== */

static const char *confidence_str(confidence_t c)
{
    switch (c) {
        case CONF_HIGH:   return "high";
        case CONF_MEDIUM: return "medium";
        case CONF_LOW:    return "low";
    }
    return "low";
}

static int write_json(const char *filepath)
{
    FILE *fp = fopen(filepath, "w");
    if (!fp) {
        fprintf(stderr, "[RE15 ERROR] generate_mapping: Cannot write '%s'\n", filepath);
        return -1;
    }

    fprintf(fp, "[\n");
    for (int i = 0; i < g_mapping_count; i++) {
        const mapping_entry_t *m = &g_mappings[i];
        fprintf(fp, "  {\n");
        fprintf(fp, "    \"address\": \"FUN_%08x\",\n", m->address);
        fprintf(fp, "    \"stage\": %d,\n", m->stage);
        fprintf(fp, "    \"room_id\": \"%s\",\n", m->room_id);
        fprintf(fp, "    \"proposed_name\": \"%s\",\n", m->proposed_name);
        fprintf(fp, "    \"catalog_name\": \"%s\",\n", m->catalog_name);
        fprintf(fp, "    \"confidence\": \"%s\"\n", confidence_str(m->confidence));
        fprintf(fp, "  }%s\n", (i < g_mapping_count - 1) ? "," : "");
    }
    fprintf(fp, "]\n");

    fclose(fp);
    fprintf(stderr, "[RE15 INFO] generate_mapping: Wrote %s (%d entries)\n",
            filepath, g_mapping_count);
    return 0;
}

/* ======================================================================
 * Output: Markdown
 * ====================================================================== */

static int write_markdown(const char *filepath)
{
    FILE *fp = fopen(filepath, "w");
    if (!fp) {
        fprintf(stderr, "[RE15 ERROR] generate_mapping: Cannot write '%s'\n", filepath);
        return -1;
    }

    fprintf(fp, "# Overlay Function Mapping\n\n");
    fprintf(fp, "Generated by `generate_mapping` — cross-references Ghidra overlay functions\n");
    fprintf(fp, "against jump-table dispatch patterns and RE15_FUN_CATALOG.md.\n\n");

    /* Per-stage tables */
    for (int s = 0; s < g_stage_count; s++) {
        int stage_num = g_stages[s].stage;
        fprintf(fp, "## Stage %d\n\n", stage_num);
        fprintf(fp, "| Address | Room ID | Proposed Name | Catalog Name | Confidence |\n");
        fprintf(fp, "|---------|---------|---------------|--------------|------------|\n");

        int stage_count = 0;
        int unresolved_count = 0;

        for (int i = 0; i < g_mapping_count; i++) {
            const mapping_entry_t *m = &g_mappings[i];
            if (m->stage != stage_num) continue;
            stage_count++;

            fprintf(fp, "| FUN_%08x | %s | %s | %s | %s |\n",
                    m->address,
                    m->room_id,
                    m->proposed_name[0] ? m->proposed_name : "—",
                    m->catalog_name[0] ? m->catalog_name : "—",
                    confidence_str(m->confidence));

            if (strcmp(m->room_id, "unresolved") == 0) {
                unresolved_count++;
            }
        }

        fprintf(fp, "\n**Summary Stage %d:** %d functions total, %d unresolved\n\n",
                stage_num, stage_count, unresolved_count);
    }

    /* Overall summary */
    fprintf(fp, "---\n\n## Summary\n\n");

    int total = 0, total_high = 0, total_med = 0, total_low = 0, total_unresolved = 0;
    for (int i = 0; i < g_mapping_count; i++) {
        total++;
        switch (g_mappings[i].confidence) {
            case CONF_HIGH:   total_high++; break;
            case CONF_MEDIUM: total_med++;  break;
            case CONF_LOW:    total_low++;  break;
        }
        if (strcmp(g_mappings[i].room_id, "unresolved") == 0) total_unresolved++;
    }

    fprintf(fp, "| Stage | Functions | Unresolved |\n");
    fprintf(fp, "|-------|-----------|------------|\n");
    for (int s = 0; s < g_stage_count; s++) {
        int stage_num = g_stages[s].stage;
        int sc = 0, su = 0;
        for (int i = 0; i < g_mapping_count; i++) {
            if (g_mappings[i].stage != stage_num) continue;
            sc++;
            if (strcmp(g_mappings[i].room_id, "unresolved") == 0) su++;
        }
        fprintf(fp, "| %d | %d | %d |\n", stage_num, sc, su);
    }
    fprintf(fp, "| **Total** | **%d** | **%d** |\n\n", total, total_unresolved);

    fprintf(fp, "**Confidence breakdown:** high=%d, medium=%d, low=%d\n",
            total_high, total_med, total_low);

    fclose(fp);
    fprintf(stderr, "[RE15 INFO] generate_mapping: Wrote %s\n", filepath);
    return 0;
}

/* ======================================================================
 * Main
 * ====================================================================== */

static void print_usage(const char *prog)
{
    fprintf(stderr,
        "Usage: %s [-c catalog.md] [-o output_dir]\n"
        "\n"
        "  Reads overlay_mapper JSON from stdin and generates:\n"
        "    overlay_mapping.json  — machine-readable mapping\n"
        "    overlay_mapping.md   — human-readable Markdown table\n"
        "\n"
        "  Options:\n"
        "    -c <path>   Path to RE15_FUN_CATALOG.md for address matching\n"
        "    -o <dir>    Output directory (default: current directory)\n"
        "\n"
        "  Example:\n"
        "    overlay_mapper ./overlays | %s -c ../RE15_FUN_CATALOG.md -o ./output\n",
        prog, prog);
}

int main(int argc, char *argv[])
{
    const char *catalog_path = NULL;
    const char *output_dir = ".";

    /* Parse command-line arguments */
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-c") == 0 && i + 1 < argc) {
            catalog_path = argv[++i];
        } else if (strcmp(argv[i], "-o") == 0 && i + 1 < argc) {
            output_dir = argv[++i];
        } else if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
            print_usage(argv[0]);
            return 0;
        } else {
            fprintf(stderr, "[RE15 WARN] generate_mapping: Unknown option '%s'\n", argv[i]);
        }
    }

    /* Load catalog if specified */
    if (catalog_path) {
        load_catalog(catalog_path);
    }

    /* Read overlay_mapper JSON from stdin */
    int json_len = 0;
    char *json = read_all_stdin(&json_len);
    if (!json || json_len == 0) {
        fprintf(stderr, "[RE15 ERROR] generate_mapping: No input on stdin\n");
        fprintf(stderr, "             Pipe overlay_mapper output: overlay_mapper <dir> | generate_mapping\n");
        free(json);
        return 1;
    }

    /* Parse the JSON */
    if (parse_overlay_json(json) != 0) {
        fprintf(stderr, "[RE15 ERROR] generate_mapping: Failed to parse JSON input\n");
        free(json);
        return 1;
    }
    free(json);

    if (g_stage_count == 0) {
        fprintf(stderr, "[RE15 ERROR] generate_mapping: No stages found in input\n");
        return 1;
    }

    /* Build the mapping */
    build_mapping();

    /* Write outputs */
    char json_path[MAX_PATH_LEN];
    char md_path[MAX_PATH_LEN];
    snprintf(json_path, sizeof(json_path), "%s/overlay_mapping.json", output_dir);
    snprintf(md_path, sizeof(md_path), "%s/overlay_mapping.md", output_dir);

    int ret = 0;
    if (write_json(json_path) != 0) ret = 1;
    if (write_markdown(md_path) != 0) ret = 1;

    if (ret == 0) {
        fprintf(stderr, "\n=== Mapping Generation Complete ===\n");
        fprintf(stderr, "  Total entries: %d\n", g_mapping_count);
        fprintf(stderr, "  Output: %s, %s\n", json_path, md_path);
        fprintf(stderr, "===================================\n");
    }

    return ret;
}
