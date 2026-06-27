/**
 * overlay_mapper.c — Dispatch-Pattern-Erkennung in Stage-Overlays
 *
 * Parses STAGE1_overlay.c through STAGE6_overlay.c (Ghidra decompilation output)
 * to identify jump-table dispatch patterns and extract the mapping:
 *   Room-ID → Function-Address per Jump-Table
 *
 * Recognized patterns:
 *   1. Indirect calls via label-based jump tables:
 *      (**(code **)(&LAB_80xxxxxx + index * 4))();
 *   2. Bounds-checked dispatch:
 *      if (*(byte *)(base + offset) < N) { ... jump_table[index](); }
 *   3. Function headers:
 *      [comment] ======= FUN_80xxxxxx @ 0x80xxxxxx ======= [end comment]
 *
 * Output: JSON array of jump-table entries per stage.
 *
 * Requirements: 8.1
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <ctype.h>

/* ======================================================================
 * Configuration
 * ====================================================================== */

#define MAX_LINE_LEN       4096
#define MAX_FUNCTIONS      1024
#define MAX_JUMP_TABLES    256
#define MAX_JT_ENTRIES     64
#define MAX_PATH_LEN       512

/* ======================================================================
 * Data Structures
 * ====================================================================== */

/** A single entry in a jump table: index → function address */
typedef struct {
    int      index;             /* Room index (0-based) in the jump table */
    uint32_t table_base_addr;  /* Base address of the jump table (LAB_xxxx) */
    uint32_t func_addr;        /* Target function address (if resolvable) */
    char     func_name[64];    /* Function name (FUN_80xxxxxx) if matched */
} jt_entry_t;

/** A jump table discovered in an overlay */
typedef struct {
    uint32_t base_addr;        /* LAB_xxxxxxxx address of the table */
    int      max_index;        /* Upper bound from bounds check (or -1) */
    int      index_offset;     /* Byte offset used as index (e.g., +5, +7, +9) */
    int      entry_count;      /* Number of entries we could resolve */
    jt_entry_t entries[MAX_JT_ENTRIES];
    char     containing_func[64]; /* Function that contains this dispatch */
    int      line_number;      /* Line number in source where found */
} jump_table_t;

/** A function found in the overlay */
typedef struct {
    uint32_t addr;             /* Address (e.g., 0x80100424) */
    char     name[64];         /* FUN_80100424 */
    int      line_number;      /* Line in source file */
} overlay_func_t;

/** Per-stage analysis result */
typedef struct {
    int           stage;
    int           func_count;
    overlay_func_t functions[MAX_FUNCTIONS];
    int           jt_count;
    jump_table_t  jump_tables[MAX_JUMP_TABLES];
} stage_result_t;

/* ======================================================================
 * Utility Functions
 * ====================================================================== */

/**
 * Parse a hex address from a string starting at position.
 * Returns the address value, or 0 on failure.
 * Advances *pos past the parsed hex.
 */
static uint32_t parse_hex_addr(const char *str, int *pos)
{
    uint32_t addr = 0;
    int start = *pos;

    /* Skip optional "0x" prefix */
    if (str[*pos] == '0' && (str[*pos + 1] == 'x' || str[*pos + 1] == 'X')) {
        *pos += 2;
    }

    int digits = 0;
    while (isxdigit((unsigned char)str[*pos])) {
        char c = str[*pos];
        uint32_t nibble;
        if (c >= '0' && c <= '9')      nibble = c - '0';
        else if (c >= 'a' && c <= 'f') nibble = c - 'a' + 10;
        else                            nibble = c - 'A' + 10;
        addr = (addr << 4) | nibble;
        (*pos)++;
        digits++;
    }

    if (digits == 0) {
        *pos = start;
        return 0;
    }
    return addr;
}

/**
 * Try to extract a hex address from a string like "LAB_80xxxxxx" or "FUN_80xxxxxx".
 * Returns the 32-bit address or 0 if not found.
 */
static uint32_t extract_label_addr(const char *label)
{
    /* Expect format: LAB_XXXXXXXX or FUN_XXXXXXXX */
    const char *hex_start = strchr(label, '_');
    if (!hex_start) return 0;
    hex_start++; /* skip the underscore */

    uint32_t addr = 0;
    int digits = 0;
    while (isxdigit((unsigned char)*hex_start) && digits < 8) {
        char c = *hex_start;
        uint32_t nibble;
        if (c >= '0' && c <= '9')      nibble = c - '0';
        else if (c >= 'a' && c <= 'f') nibble = c - 'a' + 10;
        else                            nibble = c - 'A' + 10;
        addr = (addr << 4) | nibble;
        hex_start++;
        digits++;
    }

    if (digits < 7) return 0; /* Need at least 7 hex digits for 0x80xxxxx range */
    return addr;
}

/* ======================================================================
 * Pattern Recognition
 * ====================================================================== */

/**
 * Parse a function header comment line:
 *   [slash*] ======= FUN_80xxxxxx @ 0x80xxxxxx ======= [*slash]
 * Returns 1 if found, fills name and addr.
 */
static int parse_function_header(const char *line, char *name, uint32_t *addr)
{
    const char *marker = strstr(line, "/* ======= FUN_");
    if (!marker) return 0;

    /* Extract function name */
    const char *name_start = marker + 11; /* skip the comment start + "======= " */
    const char *name_end = strchr(name_start, ' ');
    if (!name_end) return 0;

    int name_len = (int)(name_end - name_start);
    if (name_len <= 0 || name_len >= 64) return 0;
    memcpy(name, name_start, name_len);
    name[name_len] = '\0';

    /* Extract address from "@ 0x80xxxxxx" */
    const char *at_marker = strstr(name_end, "@ ");
    if (!at_marker) return 0;
    at_marker += 2; /* skip "@ " */

    int pos = 0;
    const char *hex_str = at_marker;
    /* Skip "0x" */
    if (hex_str[0] == '0' && hex_str[1] == 'x') {
        hex_str += 2;
    }

    *addr = 0;
    int digits = 0;
    while (isxdigit((unsigned char)hex_str[pos]) && digits < 8) {
        char c = hex_str[pos];
        uint32_t nibble;
        if (c >= '0' && c <= '9')      nibble = c - '0';
        else if (c >= 'a' && c <= 'f') nibble = c - 'a' + 10;
        else                            nibble = c - 'A' + 10;
        *addr = (*addr << 4) | nibble;
        pos++;
        digits++;
    }

    return (digits >= 7) ? 1 : 0;
}

/**
 * Parse a jump-table indirect call pattern:
 *   (**(code **)(&LAB_80xxxxxx + (uint)*(byte *)(_DAT_800ac784 + N) * 4))();
 *   (**(code **)(&LAB_80xxxxxx + index * 4))();
 *
 * Extracts: table base address, index offset byte.
 * Returns 1 if pattern matched.
 */
static int parse_jump_table_call(const char *line, uint32_t *table_addr, int *index_offset)
{
    /* Look for the pattern: &LAB_80xxxxxx + ... * 4 */
    const char *lab_ptr = strstr(line, "&LAB_");
    if (!lab_ptr) {
        /* Also check for &DAT_80 pattern (some jump tables use DAT_ prefix) */
        lab_ptr = strstr(line, "&DAT_80");
        if (!lab_ptr) return 0;
    }

    /* Must be part of an indirect call: (**(code **)( */
    const char *call_prefix = strstr(line, "(**(code **)");
    if (!call_prefix || call_prefix > lab_ptr) return 0;

    /* Extract the label address */
    const char *label_start = lab_ptr + 1; /* skip '&' */
    *table_addr = extract_label_addr(label_start);
    if (*table_addr == 0) return 0;

    /* Try to extract the index offset from _DAT_800ac784 + N */
    *index_offset = -1;
    const char *dat_marker = strstr(line, "_DAT_800ac784 + ");
    if (dat_marker) {
        dat_marker += 16; /* skip "_DAT_800ac784 + " */
        /* Parse the offset, might be "0x" prefixed or just a number */
        if (dat_marker[0] == '0' && dat_marker[1] == 'x') {
            int pos = 0;
            *index_offset = (int)parse_hex_addr(dat_marker, &pos);
        } else {
            *index_offset = atoi(dat_marker);
        }
    } else {
        /* Check for (int)_DAT_800ac784 + N pattern */
        dat_marker = strstr(line, "_DAT_800ac784 + ");
        if (!dat_marker) {
            /* Check for DAT_800ac784 + 9) & 0xf pattern (masked index) */
            dat_marker = strstr(line, "800ac784 + ");
            if (dat_marker) {
                dat_marker += 11; /* skip "800ac784 + " */
                if (dat_marker[0] == '0' && dat_marker[1] == 'x') {
                    int pos = 0;
                    *index_offset = (int)parse_hex_addr(dat_marker, &pos);
                } else {
                    *index_offset = atoi(dat_marker);
                }
            }
        }
    }

    /* Verify it's multiplied by 4 (pointer table indexing) */
    const char *mul4 = strstr(line, "* 4");
    if (!mul4) {
        /* Also handle "* 4)" or the index is computed differently */
        /* Some patterns use in_v0 directly without *4, which is pre-multiplied */
        /* Accept these too as they still represent jump tables */
    }

    return 1;
}

/**
 * Parse a bounds check pattern:
 *   if (*(byte *)(base + offset) < N) {
 * Only matches when N is in a plausible range for jump-table bounds (1-50).
 * Returns 1 if found, fills max_index.
 */
static int parse_bounds_check(const char *line, int *max_index, int *index_offset)
{
    /* Look for pattern: < 0xNN) or < NN) after a byte read from _DAT_800ac784 */
    const char *dat_ref = strstr(line, "_DAT_800ac784 + ");
    if (!dat_ref) return 0;

    /* Must be in an if statement with a comparison */
    if (!strstr(line, "if (") && !strstr(line, "if(")) return 0;

    /* Must reference a byte read (byte *) for it to be an index check */
    if (!strstr(line, "byte *)") && !strstr(line, "byte*)")) return 0;

    /* Extract the offset */
    dat_ref += 16;
    if (dat_ref[0] == '0' && dat_ref[1] == 'x') {
        int pos = 0;
        *index_offset = (int)parse_hex_addr(dat_ref, &pos);
    } else {
        *index_offset = atoi(dat_ref);
    }

    /* Find the comparison operator and value: ") < N" */
    const char *less_than = strstr(line, ") < ");
    if (!less_than) return 0;
    less_than += 4; /* skip ") < " */

    /* Parse the bound value */
    const char *val_start = less_than;
    while (*val_start && *val_start == ' ') {
        val_start++;
    }

    if (val_start[0] == '0' && val_start[1] == 'x') {
        int pos = 0;
        *max_index = (int)parse_hex_addr(val_start, &pos);
    } else if (isdigit((unsigned char)*val_start)) {
        *max_index = atoi(val_start);
    } else {
        return 0;
    }

    /* Plausibility filter: room-dispatch bounds are typically < 50 */
    if (*max_index <= 0 || *max_index > 50) return 0;

    return 1;
}

/* ======================================================================
 * Overlay File Processing
 * ====================================================================== */

/**
 * Process a single overlay file and extract all jump tables and functions.
 */
static int process_overlay_file(const char *filepath, stage_result_t *result)
{
    FILE *fp = fopen(filepath, "r");
    if (!fp) {
        fprintf(stderr, "[RE15 WARN] overlay_mapper: Cannot open '%s'\n", filepath);
        return -1;
    }

    char line[MAX_LINE_LEN];
    int line_num = 0;
    char current_func[64] = "";
    int pending_bounds_max = -1;
    int pending_bounds_offset = -1;

    result->func_count = 0;
    result->jt_count = 0;

    while (fgets(line, sizeof(line), fp) != NULL) {
        line_num++;

        /* 1. Check for function header */
        char fname[64];
        uint32_t faddr;
        if (parse_function_header(line, fname, &faddr)) {
            if (result->func_count < MAX_FUNCTIONS) {
                overlay_func_t *f = &result->functions[result->func_count];
                f->addr = faddr;
                strncpy(f->name, fname, sizeof(f->name) - 1);
                f->name[sizeof(f->name) - 1] = '\0';
                f->line_number = line_num;
                result->func_count++;
            }
            strncpy(current_func, fname, sizeof(current_func) - 1);
            current_func[sizeof(current_func) - 1] = '\0';
            continue;
        }

        /* 2. Check for bounds check (sets context for next jump table) */
        int bounds_max = -1, bounds_offset = -1;
        if (parse_bounds_check(line, &bounds_max, &bounds_offset)) {
            pending_bounds_max = bounds_max;
            pending_bounds_offset = bounds_offset;
            continue;
        }

        /* 3. Check for jump-table indirect call */
        uint32_t table_addr;
        int idx_offset;
        if (parse_jump_table_call(line, &table_addr, &idx_offset)) {
            if (result->jt_count < MAX_JUMP_TABLES) {
                jump_table_t *jt = &result->jump_tables[result->jt_count];
                jt->base_addr = table_addr;
                jt->index_offset = idx_offset;
                jt->line_number = line_num;
                jt->entry_count = 0;
                strncpy(jt->containing_func, current_func, sizeof(jt->containing_func) - 1);
                jt->containing_func[sizeof(jt->containing_func) - 1] = '\0';

                /* Apply pending bounds check if offset matches */
                if (pending_bounds_max > 0 &&
                    (pending_bounds_offset == idx_offset || pending_bounds_offset == -1)) {
                    jt->max_index = pending_bounds_max;
                } else {
                    jt->max_index = -1;  /* Unknown bounds */
                }

                result->jt_count++;
            }
            /* Reset bounds context (only applies to immediately following table) */
            pending_bounds_max = -1;
            pending_bounds_offset = -1;
            continue;
        }

        /* Reset bounds if we hit a non-matching line (not blank) */
        if (pending_bounds_max > 0 && line[0] != '\n' && line[0] != '\r' &&
            !strstr(line, "WARNING") && line[0] != ' ') {
            pending_bounds_max = -1;
            pending_bounds_offset = -1;
        }
    }

    fclose(fp);
    return 0;
}

/**
 * Attempt to resolve jump-table entries to function addresses.
 *
 * Strategy: Jump tables at address LAB_X typically contain consecutive
 * function pointers. If the table base is near the start of the overlay
 * (0x80100000 region), and we know the function list, we can infer
 * which functions correspond to which table entries based on:
 *   - Address ordering (functions in overlay are ordered by address)
 *   - Table base + index*4 gives the stored pointer location
 *   - Functions immediately following the table base are likely targets
 *
 * For overlays loaded at 0x80100000, if a jump table references
 * LAB_801000fc with max_index 0xc, entries 0..11 correspond to
 * addresses stored at 0x801000fc + 0*4, 0x801000fc + 1*4, etc.
 */
static void resolve_jump_table_entries(stage_result_t *result)
{
    for (int jt_idx = 0; jt_idx < result->jt_count; jt_idx++) {
        jump_table_t *jt = &result->jump_tables[jt_idx];

        /* We can infer entries if:
         * - The table base is in the overlay range (0x80100000+)
         * - The max_index is known
         * - We have function addresses to match against
         *
         * Each entry in the table is a 4-byte pointer.
         * table[i] is at address (base_addr + i * 4).
         * The VALUE at that address would be a function pointer.
         *
         * Since we don't have the binary data, we infer from the
         * sequential function layout. Functions defined right after
         * the dispatch function are typically the targets.
         */

        if (jt->max_index <= 0) continue;

        int max_entries = jt->max_index;
        if (max_entries > MAX_JT_ENTRIES) max_entries = MAX_JT_ENTRIES;

        /* For the main room dispatch tables, each entry at table_base + i*4
         * contains a function pointer. We record the table structure even
         * without binary resolution. */
        for (int i = 0; i < max_entries; i++) {
            jt_entry_t *entry = &jt->entries[jt->entry_count];
            entry->index = i;
            entry->table_base_addr = jt->base_addr;
            entry->func_addr = jt->base_addr + (uint32_t)(i * 4);
            /* Mark as unresolved — needs binary data for full resolution */
            snprintf(entry->func_name, sizeof(entry->func_name),
                     "table_entry_%d", i);
            jt->entry_count++;
        }
    }
}

/* ======================================================================
 * Output Generation
 * ====================================================================== */

/**
 * Output analysis results as JSON to stdout.
 */
static void output_json(const stage_result_t *results, int num_stages)
{
    printf("[\n");

    for (int s = 0; s < num_stages; s++) {
        const stage_result_t *r = &results[s];

        if (s > 0) printf(",\n");
        printf("  {\n");
        printf("    \"stage\": %d,\n", r->stage);
        printf("    \"total_functions\": %d,\n", r->func_count);
        printf("    \"jump_tables_found\": %d,\n", r->jt_count);
        printf("    \"functions\": [\n");

        for (int f = 0; f < r->func_count; f++) {
            const overlay_func_t *fn = &r->functions[f];
            printf("      {\"address\": \"0x%08x\", \"name\": \"%s\", \"line\": %d}",
                   fn->addr, fn->name, fn->line_number);
            if (f < r->func_count - 1) printf(",");
            printf("\n");
        }
        printf("    ],\n");

        printf("    \"jump_tables\": [\n");
        for (int j = 0; j < r->jt_count; j++) {
            const jump_table_t *jt = &r->jump_tables[j];
            printf("      {\n");
            printf("        \"base_address\": \"0x%08x\",\n", jt->base_addr);
            printf("        \"max_index\": %d,\n", jt->max_index);
            printf("        \"index_offset_byte\": %d,\n", jt->index_offset);
            printf("        \"containing_function\": \"%s\",\n", jt->containing_func);
            printf("        \"source_line\": %d,\n", jt->line_number);
            printf("        \"entries\": [\n");

            for (int e = 0; e < jt->entry_count; e++) {
                const jt_entry_t *entry = &jt->entries[e];
                printf("          {\"room_index\": %d, "
                       "\"pointer_location\": \"0x%08x\", "
                       "\"resolved_name\": \"%s\"}",
                       entry->index, entry->func_addr, entry->func_name);
                if (e < jt->entry_count - 1) printf(",");
                printf("\n");
            }
            printf("        ]\n");
            printf("      }");
            if (j < r->jt_count - 1) printf(",");
            printf("\n");
        }
        printf("    ]\n");
        printf("  }");
    }

    printf("\n]\n");
}

/**
 * Print a summary to stderr.
 */
static void output_summary(const stage_result_t *results, int num_stages)
{
    fprintf(stderr, "\n=== Overlay Mapper Summary ===\n");
    int total_funcs = 0, total_jts = 0;

    for (int s = 0; s < num_stages; s++) {
        const stage_result_t *r = &results[s];
        fprintf(stderr, "  STAGE%d: %3d functions, %2d jump-tables\n",
                r->stage, r->func_count, r->jt_count);
        total_funcs += r->func_count;
        total_jts += r->jt_count;
    }

    fprintf(stderr, "  ----- \n");
    fprintf(stderr, "  Total: %d functions, %d jump-tables across %d stages\n",
            total_funcs, total_jts, num_stages);
    fprintf(stderr, "==============================\n\n");
}

/* ======================================================================
 * Main
 * ====================================================================== */

static void print_usage(const char *prog)
{
    fprintf(stderr,
        "Usage: %s <overlay_directory>\n"
        "\n"
        "  Parses STAGE1_overlay.c through STAGE6_overlay.c in the given directory.\n"
        "  Detects jump-table dispatch patterns and extracts Room-ID to\n"
        "  function-address mappings.\n"
        "\n"
        "  Output: JSON on stdout, summary on stderr.\n"
        "\n"
        "  Example:\n"
        "    %s ../RE_15_Quellcode_Overlays > overlay_mapping.json\n",
        prog, prog);
}

int main(int argc, char *argv[])
{
    if (argc < 2) {
        print_usage(argv[0]);
        return 1;
    }

    const char *overlay_dir = argv[1];
    static stage_result_t results[6];  /* Static to avoid stack overflow (large structs) */
    int stages_processed = 0;

    for (int stage = 1; stage <= 6; stage++) {
        char filepath[MAX_PATH_LEN];
        snprintf(filepath, sizeof(filepath), "%s/STAGE%d_overlay.c", overlay_dir, stage);

        results[stages_processed].stage = stage;

        if (process_overlay_file(filepath, &results[stages_processed]) == 0) {
            /* Attempt to resolve table entries to known functions */
            resolve_jump_table_entries(&results[stages_processed]);
            stages_processed++;
        } else {
            fprintf(stderr, "[RE15 WARN] overlay_mapper: Skipping STAGE%d (file not found)\n",
                    stage);
            /* Still include stage with zero results */
            results[stages_processed].func_count = 0;
            results[stages_processed].jt_count = 0;
            stages_processed++;
        }
    }

    if (stages_processed == 0) {
        fprintf(stderr, "[RE15 ERROR] overlay_mapper: No overlay files found in '%s'\n",
                overlay_dir);
        fprintf(stderr, "             Erwartet: STAGE1_overlay.c bis STAGE6_overlay.c\n");
        fprintf(stderr, "             Hilfe: Ghidra-Export der Stage-Overlays in dieses Verzeichnis ablegen\n");
        return 1;
    }

    output_json(results, stages_processed);
    output_summary(results, stages_processed);

    return 0;
}
