/*
 * room_pc.c — PC ARCH half of the per-room RDT loader.
 *
 * Mirror of the PSX re15_room.c (CD loader), but reads the room container from
 * a FILE (assets/room####.rdt) via the PC asset backend instead of CdRead. It
 * fills the SHARED g_room_rdt (room_common.c); the shared transition
 * (re15_room_apply_pending) calls it through the ctx->load_rdt callback.
 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "re15_room.h"
#include "re15_rdt.h"
#include "re15_msg.h"     /* re15_msg_clear_room_block  — Teardown (3) */
#include "re15_light.h"   /* g_re15_room_lights_ok      — Teardown (2) */
#include "re15_esp.h"     /* re15_esp_fx_reset / _set_room_bank — Teardown (4) */
#include "asset_root_pc.h"   /* gemeinsame Asset-Wurzel-Aufloesung (exe-relativ) */

/* Overdraw-Layer des PC-Renderers (definiert in render_pc.c) — Teardown (1). */
extern void re15_render_pc_set_pri_rects(const int *src_x, const int *src_y,
                                         const int *dst_x, const int *dst_y,
                                         const int *w, const int *h,
                                         const int *depth, int count);
extern void re15_render_pc_set_pri_atlas(const uint32_t *rgba, int w, int h);
extern void re15_render_pc_invalidate_tim_slot(int slot);   /* Teardown (5) */

extern uint8_t *re15_asset_read_file(const char *path, int *out_size);

/* Keeps the resident room's RDT bytes alive (g_room_rdt points into it). */
static unsigned char *s_pc_room_buf = NULL;
static int            s_pc_room_size = 0;

int re15_room_load(unsigned room_id)
{
    /* Asset-Pfad-Konsolidierung (2026-07-02): der Raum-RDT kommt aus der EINEN Asset-
     * Wurzel shared_assets/PSX, CD-Layout STAGE{N}/ROOM%04X.RDT. Die erste Hex-Ziffer der
     * Room-ID = Stage (verifiziert: 80/32/32/32/48/16 = 240 RDTs unter STAGE1..6). Die alten
     * assets_shared/RDT-Flach- + re15_reborn-Ketten sind entfernt — es gibt nur noch diese
     * eine Wurzel (env RE15_ASSET_ROOT / RE15_CD_ROOT übersteuern; sonst der Compile-Default). */
    char path[300];
    uint8_t *buf = NULL;
    int size = 0;
    unsigned stage = (room_id >> 12) & 0xF;

    /* 2026-08-24: eigene Wurzelliste ersetzt durch die gemeinsame (asset_root_pc.c) — die kennt
     * zusaetzlich das VERZEICHNIS DER LAUFENDEN EXE, damit findet ein Paket seine RDTs auch,
     * wenn die exe direkt und aus einem beliebigen Arbeitsverzeichnis gestartet wird. */
    if (stage >= 1 && stage <= 6) {
        snprintf(path, sizeof path, "STAGE%u/ROOM%04X.RDT", stage, room_id);
        buf = re15_pc_read_cd(path, &size);
    }
    if (!buf) {
        fprintf(stderr, "[room] PC load FAILED: room%04x.rdt not found\n", room_id);
        g_room_rdt_ok = 0;
        return -1;
    }
    if (re15_rdt_parse(buf, (size_t)size, &g_room_rdt) != 0) {
        free(buf);
        g_room_rdt_ok = 0;
        return -1;
    }
    if (s_pc_room_buf) free(s_pc_room_buf);   /* free the previous room's bytes */
    s_pc_room_buf     = buf;
    s_pc_room_size    = size;
    g_current_room_id = room_id;
    g_room_rdt_ok     = 1;
    fprintf(stderr, "[room] PC loaded room%04x.rdt (%d bytes)\n", room_id, size);
    return 0;
}

/* The RAW bytes of the room currently resident in g_room_rdt. main.c keeps its own
 * rdt_buf/rdt_size for the consumers that parse sections straight out of the file
 * (sprite.pri, camera FX, ESP); on a cross-room transition those MUST be re-pointed
 * here, or they keep reading the BOOT room's bytes with the NEW room's offsets. */
const unsigned char *re15_room_pc_bytes(int *size)
{
    if (size) *size = s_pc_room_size;
    return s_pc_room_buf;
}

/* PC-PER-RAUM-TEARDOWN (der ctx-Callback reset_render von re15_room_apply_pending).
 *
 * WAR VORHER EIN LEERER RUMPF — mit der Begruendung, der PC lade jeden Cut ohnehin neu und habe
 * keinen Bump-Allokator zum Zuruecksetzen. Das war der Kern der Architektur-Abweichung, die der
 * Nutzer aufgedeckt hat ("dann haben wir unseren Port ja NICHT ans Original angelehnt"): weil auf
 * PSX genau dieser Hook den Per-Raum-Reset ausloest, lief auf PC gar keiner — und jeder Zustand,
 * den niemand einzeln aufraeumte, wanderte in den naechsten Raum.
 *
 * DAS ORIGINAL-MODELL (audit wf_64fa9e73) hat ZWEI Mechanismen, nicht einen:
 *   (a) ARENA: alles Variable liegt in einer Arena, die EIN Store freigibt — FUN_800396FC setzt den
 *       Bump-Pointer 0x800AC77C auf die Arena-Basis 0x800AC780 (@0x80039738) und laedt die neue RDT
 *       ab genau dieser Adresse (@0x80039740/@0x800397E8), also UEBER die alten Raumdaten.
 *   (b) POOLS FESTER GROESSE: die werden nicht freigegeben, sondern es wird nur ihr
 *       Gueltigkeits-Flagwort genullt — Entity +0x00 (@0x8001A4E8), Objekt +0x00 (@0x8003EABC),
 *       Effekt +0x00 (@0x80019378), SCD-Thread +0x01/+0x02/+0x04/+0x08 (@0x8003ED0C-3D).
 * Der Port braucht deshalb KEINEN Arena-Allokator — er braucht einen Teardown mit derselben
 * Wirkung, an derselben Stelle der Reihenfolge.
 *
 * Diese Fassung macht die Loeschungen, die eigenstaendig belegt und ohne Speicher-Umlagerung
 * machbar sind. NOCH NICHT drin (bewusst, als eigener Schritt): das free der RDT-Bytes wandert vom
 * Loader hierher. Solange das nicht passiert ist, zeigen alte Aliasse auf noch gueltige Bytes —
 * unschoen, aber harmlos; nach der Umlagerung waeren sie freigegeben, und dann muss die Liste der
 * Besitzer vollstaendig sein (ASAN-Sweep ueber alle 240 Raeume). */
void re15_room_reset_render_pc(void)
{
    /* (1) SPRITE.PRI-MASKEN + Vordergrund-Atlas verwerfen. Im Original zeigt der Masken-Zeiger jedes
     * Cut-Eintrags (+0x1C) nach dem Laden in die NEUE RDT: FUN_800396FC reloziert ihn fuer alle
     * RDT[+0x01] Cuts (@0x80039854-0x80039898). Die Arbeitspuffer der alten Masken lagen in der bei
     * @0x80039738 freigegebenen Arena. Ohne diesen Clear verdecken im Zielraum die Occluder des
     * Vorraums, bis der erste Cut-Wechsel sie ersetzt. */
    re15_render_pc_set_pri_rects(NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0);
    re15_render_pc_set_pri_atlas(NULL, 0, 0);

    /* (2) LICHTSET ungueltig machen. Der Licht-Zeiger RDT+0x2C wird ebenfalls aus der NEUEN RDT
     * reloziert (@0x800397FC-0x80039834); das alte Set lag in der freigegebenen Arena. Der
     * Installations-Block in room_common.c hat keinen else-Zweig — schlaegt das Parsen fehl, stand
     * bisher das Lichtset des Vorraums weiter. */
    g_re15_room_lights_ok = 0;

    /* (3) NACHRICHTENTABELLE verwerfen — Begruendung siehe re15_msg_clear_room_block (der Loader
     * ueberschreibt nur so viele IDs, wie der neue Raum mitbringt). */
    re15_msg_clear_room_block();

    /* (4) EFFEKT-BANK (ESP) verwerfen. Der Clear stand bisher IM Parser pc_load_room_esp und lief
     * damit nur, wenn ueberhaupt ein RDT-Puffer vorlag — ein Raum ohne Effekt-Sektion behielt die
     * Bank des Vorraums (17 von 206 RDTs haben keine). Im Original sind beide Schritte getrennt:
     * FUN_80019354 nullt erst 96 Effekt-Slots ab 0x800A7424, Stride 0x84, Feld +0x00 (@0x80019378)
     * und setzt die ID-Maps 0x800B2248/0x800B22D4 auf -1 (@0x80019388-E4) — erst danach wird die
     * Sektion des neuen Raumes geparst. */
    re15_esp_fx_reset();
    re15_esp_set_room_bank(NULL);

    /* (5) PROP-TEXTUR-SLOTS 4..9 ungueltig machen. pc_load_room_prop_set laedt Slot 4+op nur fuer
     * die Props, die der neue Raum HAT; ein Raum mit weniger Props behielt die Textur des Vorraums
     * im ueberzaehligen Slot. Im Original nullt FUN_8003EA7C das Flagwort ALLER 32 Objekt-Slots ab
     * 0x800B3F98 (@0x8003EAB0-ACC), bevor die TPage/CLUT-Bytes der vorhandenen Props gesetzt werden
     * (@0x8003EB04-18) — es faellt also nur die GUELTIGKEIT, nicht der Speicher. */
    for (int slot = 4; slot <= 9; slot++)
        re15_render_pc_invalidate_tim_slot(slot);
}
