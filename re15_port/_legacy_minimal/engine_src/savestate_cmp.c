/**
 * @file savestate_cmp.c
 * @brief Savestate-Vergleichslogik für Headless-Validierung
 *
 * Implementiert:
 * - JSON-Savestate laden und parsen
 * - Spielzustand-Vergleich mit konfigurierbarer Toleranz
 * - Ergebnis-Ausgabe als JSON auf stdout
 *
 * Validates: Requirements 12.3
 */

#include "re15_savestate.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ============================================================================
 * Interner Mini-JSON-Parser
 *
 * Minimalistischer Parser für das definierte Savestate-Format.
 * Unterstützt: Objekte, Arrays von Integers, Integer-Werte.
 * Kein externer JSON-Bibliotheks-Dependency.
 * ========================================================================= */

/** Überspringt Whitespace im Puffer */
static const char* skip_ws(const char* p)
{
    while (*p == ' ' || *p == '\t' || *p == '\n' || *p == '\r') {
        p++;
    }
    return p;
}

/** Parst einen Integer (optional mit Vorzeichen) */
static const char* parse_int(const char* p, int32_t* out)
{
    int32_t sign = 1;
    int32_t val = 0;

    p = skip_ws(p);
    if (*p == '-') {
        sign = -1;
        p++;
    } else if (*p == '+') {
        p++;
    }

    if (*p < '0' || *p > '9') {
        return NULL; /* Kein gültiger Integer */
    }

    while (*p >= '0' && *p <= '9') {
        val = val * 10 + (*p - '0');
        p++;
    }

    *out = val * sign;
    return p;
}

/** Sucht nach einem JSON-Key im Puffer (naiv aber ausreichend) */
static const char* find_key(const char* json, const char* key)
{
    char search[128];
    const char* pos;

    snprintf(search, sizeof(search), "\"%s\"", key);
    pos = strstr(json, search);
    if (!pos) return NULL;

    pos += strlen(search);
    pos = skip_ws(pos);
    if (*pos != ':') return NULL;
    pos++;
    pos = skip_ws(pos);
    return pos;
}

/** Parst ein Integer-Array: [1, 2, 3, ...] */
static const char* parse_int_array(const char* p, int* out, int max_count, int* count)
{
    *count = 0;
    p = skip_ws(p);
    if (*p != '[') return NULL;
    p++;
    p = skip_ws(p);

    /* Leeres Array */
    if (*p == ']') {
        return p + 1;
    }

    while (*count < max_count) {
        int32_t val;
        p = parse_int(p, &val);
        if (!p) return NULL;
        out[*count] = (int)val;
        (*count)++;

        p = skip_ws(p);
        if (*p == ']') {
            return p + 1;
        }
        if (*p != ',') return NULL;
        p++;
    }

    return NULL; /* Overflow */
}

/** Parst ein uint8_t-Array: [0, 1, 0, ...] */
static const char* parse_uint8_array(const char* p, uint8_t* out, int max_count, int* count)
{
    *count = 0;
    p = skip_ws(p);
    if (*p != '[') return NULL;
    p++;
    p = skip_ws(p);

    /* Leeres Array */
    if (*p == ']') {
        return p + 1;
    }

    while (*count < max_count) {
        int32_t val;
        p = parse_int(p, &val);
        if (!p) return NULL;
        out[*count] = (uint8_t)val;
        (*count)++;

        p = skip_ws(p);
        if (*p == ']') {
            return p + 1;
        }
        if (*p != ',') return NULL;
        p++;
    }

    return NULL; /* Overflow */
}

/* ============================================================================
 * Öffentliche API
 * ========================================================================= */

int re15_savestate_load(const char* path, re15_savestate_t* out)
{
    FILE*   f;
    long    file_size;
    char*   buf = NULL;
    const char* pos;

    if (!path || !out) return -1;

    memset(out, 0, sizeof(re15_savestate_t));

    /* Datei öffnen und lesen */
    f = fopen(path, "rb");
    if (!f) {
        fprintf(stderr, "[RE15 ERROR] Savestate: Datei nicht lesbar: %s\n", path);
        return -1;
    }

    fseek(f, 0, SEEK_END);
    file_size = ftell(f);
    fseek(f, 0, SEEK_SET);

    if (file_size <= 0 || file_size > 1024 * 1024) {
        fprintf(stderr, "[RE15 ERROR] Savestate: Ungültige Dateigröße: %ld\n", file_size);
        fclose(f);
        return -1;
    }

    buf = (char*)malloc((size_t)file_size + 1);
    if (!buf) {
        fprintf(stderr, "[RE15 ERROR] Savestate: Speicherallokation fehlgeschlagen\n");
        fclose(f);
        return -1;
    }

    if (fread(buf, 1, (size_t)file_size, f) != (size_t)file_size) {
        fprintf(stderr, "[RE15 ERROR] Savestate: Lesefehler: %s\n", path);
        free(buf);
        fclose(f);
        return -1;
    }
    buf[file_size] = '\0';
    fclose(f);

    /* --- player_pos parsen --- */
    pos = find_key(buf, "player_pos");
    if (pos) {
        const char* x_pos;
        const char* y_pos;
        const char* z_pos;

        /* Objekt: {"x": N, "y": N, "z": N} */
        if (*pos != '{') {
            free(buf);
            return -1;
        }

        x_pos = find_key(pos, "x");
        y_pos = find_key(pos, "y");
        z_pos = find_key(pos, "z");

        if (x_pos && parse_int(x_pos, &out->player_x)) {
            out->has_position = 1;
        }
        if (y_pos) parse_int(y_pos, &out->player_y);
        if (z_pos) parse_int(z_pos, &out->player_z);
    }

    /* --- yaw parsen --- */
    pos = find_key(buf, "yaw");
    if (pos) {
        int32_t yaw_val;
        if (parse_int(pos, &yaw_val)) {
            out->yaw = (int16_t)yaw_val;
            out->has_yaw = 1;
        }
    }

    /* --- flags parsen --- */
    pos = find_key(buf, "flags");
    if (pos) {
        int count = 0;
        if (parse_uint8_array(pos, out->flags, RE15_SAVESTATE_MAX_FLAGS, &count)) {
            out->flags_count = count;
            out->has_flags = 1;
        }
    }

    /* --- inventory parsen --- */
    pos = find_key(buf, "inventory");
    if (pos) {
        int count = 0;
        if (parse_int_array(pos, out->inventory, RE15_SAVESTATE_MAX_INVENTORY, &count)) {
            out->inventory_count = count;
            out->has_inventory = 1;
        }
    }

    free(buf);
    return 0;
}

int re15_savestate_pos_match(int32_t expected, int32_t actual)
{
    /* Use int64_t to avoid overflow when computing the difference */
    int64_t diff = (int64_t)expected - (int64_t)actual;
    if (diff < 0) diff = -diff;
    return diff <= RE15_SAVESTATE_POS_TOLERANCE;
}

void re15_savestate_compare(
    const re15_savestate_t* expected,
    int32_t actual_x, int32_t actual_y, int32_t actual_z,
    int16_t actual_yaw,
    const uint8_t* actual_flags, int actual_flags_count,
    const int* actual_inventory, int actual_inventory_count,
    re15_cmp_result_t* out)
{
    if (!expected || !out) return;

    memset(out, 0, sizeof(re15_cmp_result_t));
    out->result = RE15_CMP_MATCH;

    /* --- Positions-Vergleich (Toleranz ±1) --- */
    if (expected->has_position) {
        if (!re15_savestate_pos_match(expected->player_x, actual_x)) {
            if (out->diff_count < RE15_CMP_MAX_DIFFS) {
                out->diffs[out->diff_count].field = "player_pos.x";
                out->diffs[out->diff_count].expected = expected->player_x;
                out->diffs[out->diff_count].actual = actual_x;
                out->diff_count++;
            }
            out->result = RE15_CMP_MISMATCH;
        }
        if (!re15_savestate_pos_match(expected->player_y, actual_y)) {
            if (out->diff_count < RE15_CMP_MAX_DIFFS) {
                out->diffs[out->diff_count].field = "player_pos.y";
                out->diffs[out->diff_count].expected = expected->player_y;
                out->diffs[out->diff_count].actual = actual_y;
                out->diff_count++;
            }
            out->result = RE15_CMP_MISMATCH;
        }
        if (!re15_savestate_pos_match(expected->player_z, actual_z)) {
            if (out->diff_count < RE15_CMP_MAX_DIFFS) {
                out->diffs[out->diff_count].field = "player_pos.z";
                out->diffs[out->diff_count].expected = expected->player_z;
                out->diffs[out->diff_count].actual = actual_z;
                out->diff_count++;
            }
            out->result = RE15_CMP_MISMATCH;
        }
    }

    /* --- Yaw-Vergleich (exakt) --- */
    if (expected->has_yaw) {
        if (expected->yaw != actual_yaw) {
            if (out->diff_count < RE15_CMP_MAX_DIFFS) {
                out->diffs[out->diff_count].field = "yaw";
                out->diffs[out->diff_count].expected = (int32_t)expected->yaw;
                out->diffs[out->diff_count].actual = (int32_t)actual_yaw;
                out->diff_count++;
            }
            out->result = RE15_CMP_MISMATCH;
        }
    }

    /* --- Flags-Vergleich (exakt) --- */
    if (expected->has_flags) {
        int i;
        int max_flags = expected->flags_count;

        /* Längen-Mismatch */
        if (expected->flags_count != actual_flags_count) {
            if (out->diff_count < RE15_CMP_MAX_DIFFS) {
                out->diffs[out->diff_count].field = "flags.length";
                out->diffs[out->diff_count].expected = expected->flags_count;
                out->diffs[out->diff_count].actual = actual_flags_count;
                out->diff_count++;
            }
            out->result = RE15_CMP_MISMATCH;
            /* Vergleiche nur bis zum kürzeren Array */
            if (actual_flags_count < max_flags) {
                max_flags = actual_flags_count;
            }
        }

        /* Element-Vergleich */
        for (i = 0; i < max_flags && out->diff_count < RE15_CMP_MAX_DIFFS; i++) {
            if (expected->flags[i] != actual_flags[i]) {
                char* field_buf = NULL;
                /* Statischer Puffer für Feldnamen (Thread-unsafe, aber ok für CLI) */
                static char flag_fields[RE15_CMP_MAX_DIFFS][32];
                field_buf = flag_fields[out->diff_count];
                snprintf(field_buf, 32, "flags[%d]", i);

                out->diffs[out->diff_count].field = field_buf;
                out->diffs[out->diff_count].expected = (int32_t)expected->flags[i];
                out->diffs[out->diff_count].actual = (int32_t)actual_flags[i];
                out->diff_count++;
                out->result = RE15_CMP_MISMATCH;
            }
        }
    }

    /* --- Inventar-Vergleich (exakt) --- */
    if (expected->has_inventory) {
        int i;
        int max_inv = expected->inventory_count;

        /* Längen-Mismatch */
        if (expected->inventory_count != actual_inventory_count) {
            if (out->diff_count < RE15_CMP_MAX_DIFFS) {
                out->diffs[out->diff_count].field = "inventory.length";
                out->diffs[out->diff_count].expected = expected->inventory_count;
                out->diffs[out->diff_count].actual = actual_inventory_count;
                out->diff_count++;
            }
            out->result = RE15_CMP_MISMATCH;
            if (actual_inventory_count < max_inv) {
                max_inv = actual_inventory_count;
            }
        }

        /* Element-Vergleich */
        for (i = 0; i < max_inv && out->diff_count < RE15_CMP_MAX_DIFFS; i++) {
            if (expected->inventory[i] != actual_inventory[i]) {
                static char inv_fields[RE15_CMP_MAX_DIFFS][32];
                char* field_buf = inv_fields[out->diff_count];
                snprintf(field_buf, 32, "inventory[%d]", i);

                out->diffs[out->diff_count].field = field_buf;
                out->diffs[out->diff_count].expected = (int32_t)expected->inventory[i];
                out->diffs[out->diff_count].actual = (int32_t)actual_inventory[i];
                out->diff_count++;
                out->result = RE15_CMP_MISMATCH;
            }
        }
    }
}

void re15_savestate_print_result(const re15_cmp_result_t* result)
{
    int i;

    if (!result) return;

    if (result->result == RE15_CMP_ERROR) {
        printf("{\"result\":\"error\",\"message\":\"%s\"}\n", result->error_msg);
        return;
    }

    if (result->result == RE15_CMP_MATCH) {
        printf("{\"result\":\"match\"}\n");
        return;
    }

    /* Mismatch — alle Abweichungen ausgeben */
    printf("{\"result\":\"mismatch\",\"mismatches\":[");
    for (i = 0; i < result->diff_count; i++) {
        if (i > 0) printf(",");
        printf("{\"field\":\"%s\",\"expected\":%d,\"actual\":%d}",
               result->diffs[i].field,
               (int)result->diffs[i].expected,
               (int)result->diffs[i].actual);
    }
    printf("]}\n");
}
