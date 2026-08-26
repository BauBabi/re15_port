/* probe_1190_texte.c — MESSUNG: die Meldungstexte von ROOM1190.
 * Zweck: sub15 (der sce-3-Auslöser an der Weste, Aot_set @Datei 0x002D90) zeigt je nach
 * Flag(3,0x75) Meldung 0x04 oder 0x05. Diese Sonde liest sie mit dem DEKODER DES PORTS
 * (re15_msg_load_room_block + re15_msg_get_text), damit hier nichts nachgebaut wird.
 * KEIN add_test. */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "re15_rdt.h"
#include "re15_msg.h"

static re15_rdt_t s_rdt;

int main(void)
{
    char path[512];
    snprintf(path, sizeof path, "%s/STAGE1/ROOM1190.RDT", RE15_ASSET_PSX_DIR);
    FILE *f = fopen(path, "rb");
    if (!f) { printf("FAIL: %s\n", path); return 1; }
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    uint8_t *b = (uint8_t *)malloc((size_t)sz);
    if (fread(b, 1, (size_t)sz, f) != (size_t)sz) return 1;
    fclose(f);
    if (re15_rdt_parse(b, (size_t)sz, &s_rdt) != 0) { printf("FAIL: Parse\n"); return 1; }
    printf("Message-Block: %d B\n", s_rdt.messages_size);
    re15_msg_load_room_block(s_rdt.messages, s_rdt.messages_size);
    for (int i = 0; i < 24; i++) {
        const char *t = re15_msg_get_text(i);
        if (t && t[0]) printf("  msg 0x%02X: \"%s\"\n", i, t);
    }
    free(b);
    return 0;
}
