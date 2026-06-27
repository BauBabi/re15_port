/* =============================================================================
 * RE 1.5 Port — PSX Main (Game Loop)
 * =============================================================================
 * Initialisiert die PSX-Hardware und verdrahtet alle Subsysteme im
 * Haupt-Game-Loop bei 30 fps (VSync-synchronisiert via render_psx end_frame).
 *
 * Subsystem-Pipeline pro Frame:
 *   Pad-Input → Player-Update → Kollision → AOT-Check → Kamera → SCD → Render
 *
 * Hardware-Initialisierung:
 *   1. ResetGraph(0) — GPU reset (via render_psx init)
 *   2. SpuInit()     — SPU (via audio_psx init)
 *   3. CdInit()      — CD-ROM Dateisystem
 *   4. PadInit(0)    — Controller (via input_psx init)
 *
 * Validates: Requirements 1.5, 5.6
 * ========================================================================== */

#include <stdint.h>
#include <string.h>
#include <stdio.h>

#include <psxgpu.h>
#include <psxgte.h>
#include <psxpad.h>
#include <psxcd.h>
#include <psxapi.h>

#include "re15_types.h"
#include "re15_engine.h"
#include "re15_render.h"
#include "re15_audio.h"
#include "re15_input.h"
#include "re15_io.h"
#include "re15_player.h"
#include "re15_scd.h"
#include "re15_room.h"
#include "re15_collision.h"
#include "re15_aot.h"
#include "re15_camera.h"
#include "re15_door.h"
#include "re15_rdt.h"
#include "re15_error.h"

/* ============================================================================
 * Extern: PSX-Backend-Instanzen
 * ========================================================================= */

/** I/O-Backend (platform/psx/src/io_psx.c) */
extern const re15_io_backend_t g_io_psx_backend;

/** Audio-Backend (platform/psx/src/audio_psx.c) */
extern const audio_backend_t* g_audio_psx;

/** Render-Backend (platform/psx/src/render_psx.c) */
extern const render_backend_t render_psx_backend;

/* ============================================================================
 * Pad-Buffer (34 Bytes pro Pad, DMA-aligned)
 *
 * Wird von input_psx.c über extern referenziert.
 * ========================================================================= */

uint8_t pad_buf[2][34] __attribute__((aligned(4)));

/* ============================================================================
 * Lokaler Spieler- und Raum-Zustand
 * ========================================================================= */

/** Statische Spieler-Instanz */
static re15_player_t s_player;

/** Statische RDT-Instanz (In-Place-Zeiger in den RDT-Puffer) */
static re15_rdt_t s_rdt;

/* ============================================================================
 * Hardware-Initialisierung
 *
 * Die Reihenfolge folgt dem PSX-Boot-Standard:
 *   1. GPU (via render_psx init — ruft ResetGraph(0) auf)
 *   2. SPU (via audio_psx init — ruft SpuInit() auf)
 *   3. CD-ROM (CdInit direkt — nicht in einem Backend gekapselt)
 *   4. Controller (InitPAD/StartPAD — direkt hier, input_psx liest nur)
 * ========================================================================= */

/**
 * Initialisiert das CD-ROM-Dateisystem.
 * Muss vor dem ersten I/O-Zugriff aufgerufen werden.
 */
static void psx_cd_init(void)
{
    CdInit();
}

/**
 * Initialisiert die Controller-Hardware (Pad 0 + Pad 1).
 * Danach liest input_psx.c den pad_buf pro Frame aus.
 */
static void psx_pad_init(void)
{
    InitPAD(pad_buf[0], 34, pad_buf[1], 34);
    StartPAD();
    ChangeClearPAD(0);
}

/* ============================================================================
 * Backend-Zuweisung
 *
 * Setzt die globalen Backend-Zeiger auf die PSX-Implementierungen.
 * ========================================================================= */

static void psx_backends_init(void)
{
    /* I/O-Backend: CD-Dateisystem */
    g_io = &g_io_psx_backend;

    /* Audio-Backend: SPU/libsnd */
    g_audio = g_audio_psx;

    /* Render-Backend: GTE/GPU */
    g_render = &render_psx_backend;
}

/* ============================================================================
 * Engine-Zustand initialisieren
 *
 * Verbindet die globale Engine-Instanz (g_game) mit den lokalen Strukturen
 * und initialisiert alle Subsysteme.
 * ========================================================================= */

static void psx_engine_init(void)
{
    /* Spieler-Instanz initialisieren */
    memset(&s_player, 0, sizeof(s_player));
    s_player.radius = 20;       /* Standard-Kollisionsradius */
    s_player.floor_band = 0;

    /* RDT-Instanz initialisieren */
    memset(&s_rdt, 0, sizeof(s_rdt));

    /* Globale Engine-Struktur verdrahten */
    memset(&g_game, 0, sizeof(g_game));
    g_game.player      = &s_player;
    g_game.current_rdt = &s_rdt;
    g_game.scd         = &g_scd;

    /* SCD-VM initialisieren */
    scd_vm_init();

    /* AOT-System initialisieren */
    re15_aot_init();

    /* Input-Subsystem initialisieren */
    re15_input_init();
}

/* ============================================================================
 * Hauptprogramm
 * ========================================================================= */

int main(void)
{
    re15_input_state_t input;
    int rc;

    /* =================================================================
     * Phase 1: Hardware initialisieren
     *
     * Reihenfolge:
     *   1. Render (GPU + GTE) — ResetGraph(0), InitGeom, Display-Setup
     *   2. Audio (SPU)        — SpuInit, Master-Volume
     *   3. CD-ROM             — CdInit (Dateisystem)
     *   4. Controller         — InitPAD, StartPAD
     * ================================================================= */

    /* Backend-Zeiger setzen (vor init-Aufrufen!) */
    psx_backends_init();

    /* GPU/GTE initialisieren (Double-Buffer, NTSC 320×240, OT) */
    g_render->init();

    /* SPU initialisieren (Sound-Hardware, VAB-Upload-Infrastruktur) */
    g_audio->init();

    /* CD-ROM Dateisystem initialisieren */
    psx_cd_init();

    /* Controller-Hardware initialisieren */
    psx_pad_init();

    /* =================================================================
     * Phase 2: Engine-Zustand aufbauen
     * ================================================================= */

    psx_engine_init();

    /* =================================================================
     * Phase 3: Initialen Raum laden (ROOM1240 = Startknoten)
     *
     * Stage 1, Room 0x24, Player 0 (Leon)
     * ================================================================= */

    rc = re15_room_load(1, 0x24, 0);
    if (rc != 0) {
        printf("[RE15 ERROR] PSX: Initialer Raum ROOM1240 konnte nicht "
               "geladen werden (rc=%d)\n", rc);
        /* Fallback: Loop trotzdem starten — leerer Raum */
    }

    /* =================================================================
     * Phase 4: Haupt-Game-Loop (30 fps, VSync-synchronisiert)
     *
     * Die VSync-Synchronisation erfolgt in g_render->end_frame():
     *   DrawSync(0) + VSync(0) + Framebuffer-Swap + DrawOTag
     *
     * Subsystem-Pipeline pro Frame:
     *   1. Input: Pad-Status auslesen (Edge-Detection)
     *   2. Player: Tank-Controls anwenden (Position + Rotation)
     *   3. Kollision: SCA-Prüfung, Position korrigieren
     *   4. AOT: Trigger-Zonen prüfen (Tür, Item, Event)
     *   5. Kamera: RVD-Zonen prüfen, Cut wechseln bei Bedarf
     *   6. SCD: VM-Tick (Skript-Logik ausführen)
     *   7. Render: Frame aufbauen und an GPU senden
     * ================================================================= */

    while (1) {
        /* --- 1. Input: Pad-Status auslesen --- */
        re15_input_tick();
        input = re15_input_get();

        /* --- 2. Player: Tank-Controls Bewegungsupdate --- */
        re15_player_update(&s_player, input);

        /* --- 3. Kollision: Position gegen SCA-Daten prüfen --- */
        if (g_game.current_rdt && g_game.current_rdt->collision) {
            re15_collision_check(&s_player,
                (const re15_sca_data_t*)g_game.current_rdt->collision);
        }

        /* --- 4. AOT: Tür-Trigger prüfen (Aktionstaste + Polygon) --- */
        re15_door_trigger_check(&s_player, input);

        /* --- 5. Kamera: RVD-Zonen prüfen, Kamera-ID aktualisieren --- */
        if (g_game.current_rdt && g_game.current_rdt->zone) {
            uint8_t new_cam = g_game.current_cut;
            re15_camera_check_zones(
                (int16_t)Q12_TO_INT(s_player.x),
                (int16_t)Q12_TO_INT(s_player.z),
                (const re15_rvd_data_t*)g_game.current_rdt->zone,
                g_game.current_cut,
                &new_cam
            );

            /* Kamerawechsel: BSS-Hintergrund für neuen Cut laden */
            if (new_cam != g_game.current_cut) {
                g_game.current_cut = new_cam;
                /* BSS-Chunk für neuen Cut laden und dekodieren */
                /* Hinweis: BSS-Laden wird vom Raum-System verwaltet.
                 * Hier nur den Cut-Wechsel signalisieren. */
            }
        }

        /* --- 6. SCD: Skript-VM einen Tick ausführen --- */
        scd_vm_tick();

        /* --- 7. Render: Frame aufbauen und an GPU senden --- */
        g_render->begin_frame();

        /* BSS-Hintergrund anzeigen (MDEC-dekodiert im VRAM) */
        if (g_game.bss_chunk[0] != 0 || g_game.current_cut == 0) {
            g_render->show_background(g_game.bss_chunk, g_game.current_cut);
        }

        /* TODO: 3D-Modelle (Spieler, Feinde) in OT einreichen */
        /* TODO: Sprites (Items, Effekte) in OT einreichen */

        /* Frame abschließen: DrawSync → VSync → Framebuffer-Swap → DrawOTag
         * Dies synchronisiert auf 30fps (NTSC VBlank). */
        g_render->end_frame();
    }

    /* Unreachable — PSX-Programme laufen endlos */
    return 0;
}
