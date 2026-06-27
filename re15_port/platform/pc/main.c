/* =============================================================================
 * RE 1.5 Port — PC Main (SDL2)
 * =============================================================================
 * Game-Loop mit 30fps Timer und optionalem Headless-Modus.
 *
 * Normaler Modus:
 *   Erstellt ein 320x240-Fenster (skaliert) und führt den Hauptloop aus.
 *   Subsystem-Reihenfolge pro Frame:
 *     Input → Player-Update → Kollision → AOT-Check → Kamera → SCD-Tick → Render
 *
 * Headless-Modus (--headless):
 *   Lädt einen Raum, führt N SCD-Ticks ohne Rendering aus, gibt Ergebnis
 *   als JSON auf stdout aus. Kein SDL-Fenster, kein Renderer.
 *
 * CLI-Flags:
 *   --headless          Headless-Modus aktivieren (kein Fenster/Rendering)
 *   --ticks N           Anzahl SCD-Ticks (default: 60 = 2 Sekunden bei 30Hz)
 *   --stage N           Ziel-Stage (1-6)
 *   --room M            Ziel-Room-ID (0x00-0x27, dezimal oder hex mit 0x-Prefix)
 *   --compare PATH      Vergleiche Spielzustand gegen Savestate-JSON
 *
 * JSON-Ausgabe (stdout, ohne --compare):
 *   { "player_pos": {"x": N, "y": N, "z": N}, "yaw": N,
 *     "stage": N, "room": N, "flags": [...], "errors": [] }
 *
 * JSON-Ausgabe (stdout, mit --compare, bei Mismatch):
 *   {"result":"mismatch","mismatches":[{"field":"player_pos.x","expected":100,"actual":105}]}
 *
 * Exit-Codes:
 *   0 = Erfolg / Übereinstimmung
 *   1 = Fehler oder Abweichung (Mismatch)
 *
 * Validates: Requirements 1.5, 5.6, 12.2, 12.3
 * =============================================================================
 */

#include <SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "re15_audio.h"
#include "re15_camera.h"
#include "re15_collision.h"
#include "debug_overlay.h"
#include "re15_engine.h"
#include "re15_rdt.h"
#include "re15_input.h"
#include "re15_io.h"
#include "re15_io_common.h"
#include "re15_player.h"
#include "re15_render.h"
#include "re15_room.h"
#include "re15_aot.h"
#include "re15_savestate.h"
#include "re15_scd.h"
#include "re15_error.h"

/* Ziel-Framerate: 30 FPS → 33 ms pro Frame */
#define RE15_TARGET_FPS     30
#define RE15_FRAME_TIME_MS  (1000 / RE15_TARGET_FPS)  /* ~33 ms */

/* Headless-Modus Default-Ticks (60 = 2 Sekunden bei 30Hz Tick-Rate) */
#define RE15_HEADLESS_DEFAULT_TICKS  60

/* Maximale Fehler im Headless-Modus */
#define RE15_HEADLESS_MAX_ERRORS     16

/* Startknoten: ROOM1240 (Stage 1, Room 0x24) */
#define RE15_START_STAGE    1
#define RE15_START_ROOM     0x24

/* Extern: PC I/O-Backend */
extern const re15_io_backend_t g_io_pc_backend;

/* Extern: PC Render-Backend */
extern const render_backend_t render_pc_backend;

/* Extern: Render-Backend SDL_Renderer-Zugriff (für Debug-Overlays) */
extern SDL_Renderer* render_pc_get_renderer(void);

/* ============================================================================
 * Headless-Modus Konfiguration
 * ========================================================================= */

typedef struct {
    int  enabled;                /**< 1 wenn --headless gesetzt         */
    int  ticks;                  /**< Anzahl SCD-Ticks                  */
    int  room_specified;         /**< 1 wenn --room angegeben           */
    int  stage;                  /**< Ziel-Stage (1-6)                  */
    int  room_id;                /**< Ziel-Room-ID (0x00-0x27)          */
    int  compare_enabled;        /**< 1 wenn --compare angegeben        */
    char compare_path[RE15_SAVESTATE_MAX_PATH]; /**< Pfad zur Savestate-JSON */
} headless_config_t;

/* ============================================================================
 * CLI-Parsing
 * ========================================================================= */

/**
 * Parst die Kommandozeilenargumente für Headless-Modus.
 *
 * @param argc       Argumentanzahl
 * @param argv       Argumentvektor
 * @param out_cfg    Ausgabe: Headless-Konfiguration
 * @return           0 bei Erfolg, -1 bei Parse-Fehler
 */
static int parse_cli_args(int argc, char* argv[], headless_config_t* out_cfg)
{
    int i;

    memset(out_cfg, 0, sizeof(headless_config_t));
    out_cfg->ticks = RE15_HEADLESS_DEFAULT_TICKS;

    for (i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--headless") == 0) {
            out_cfg->enabled = 1;
        }
        else if (strcmp(argv[i], "--ticks") == 0) {
            if (i + 1 >= argc) {
                fprintf(stderr, "[RE15 ERROR] CLI: --ticks erwartet eine Zahl\n");
                return -1;
            }
            i++;
            out_cfg->ticks = atoi(argv[i]);
            if (out_cfg->ticks <= 0) {
                fprintf(stderr, "[RE15 ERROR] CLI: --ticks muss > 0 sein (erhalten: %s)\n",
                        argv[i]);
                return -1;
            }
        }
        else if (strcmp(argv[i], "--stage") == 0) {
            if (i + 1 >= argc) {
                fprintf(stderr, "[RE15 ERROR] CLI: --stage erwartet eine Zahl (1-6)\n");
                return -1;
            }
            i++;
            out_cfg->stage = atoi(argv[i]);
            out_cfg->room_specified = 1;

            if (out_cfg->stage < 1 || out_cfg->stage > 6) {
                fprintf(stderr,
                    "[RE15 ERROR] CLI: Stage muss 1-6 sein (erhalten: %d)\n",
                    out_cfg->stage);
                return -1;
            }
        }
        else if (strcmp(argv[i], "--room") == 0) {
            if (i + 1 >= argc) {
                fprintf(stderr, "[RE15 ERROR] CLI: --room erwartet ROOM-ID (0x00-0x27)\n");
                return -1;
            }
            i++;
            /* Room-ID kann dezimal oder hex (0x..) angegeben werden */
            out_cfg->room_id = (int)strtol(argv[i], NULL, 0);
            out_cfg->room_specified = 1;

            if (out_cfg->room_id < 0 || out_cfg->room_id > 0x27) {
                fprintf(stderr,
                    "[RE15 ERROR] CLI: Room muss 0x00-0x27 sein (erhalten: 0x%02X)\n",
                    out_cfg->room_id);
                return -1;
            }
        }
        else if (strcmp(argv[i], "--compare") == 0) {
            if (i + 1 >= argc) {
                fprintf(stderr, "[RE15 ERROR] CLI: --compare erwartet <savestate_path>\n");
                return -1;
            }
            i++;
            out_cfg->compare_enabled = 1;
            strncpy(out_cfg->compare_path, argv[i], RE15_SAVESTATE_MAX_PATH - 1);
            out_cfg->compare_path[RE15_SAVESTATE_MAX_PATH - 1] = '\0';
        }
    }

    return 0;
}

/* ============================================================================
 * Headless-Modus JSON-Ausgabe
 * ========================================================================= */

/**
 * Gibt den Headless-Ergebniszustand als JSON auf stdout aus.
 *
 * Format:
 * { "player_pos": {"x": N, "y": N, "z": N}, "yaw": N,
 *   "stage": N, "room": N, "flags": [...], "errors": [] }
 *
 * @param player       Spieler-Zustand nach SCD-Ausführung
 * @param vm           SCD-VM-Zustand (für game_flags)
 * @param stage        Stage-Nummer (1-6)
 * @param room_id     Room-ID (0x00-0x27)
 * @param errors       Array von Fehlermeldungen (NULL-terminated strings)
 * @param error_count  Anzahl der Fehler
 */
static void headless_output_json(const re15_player_t* player,
                                 const scd_vm_t* vm,
                                 int stage,
                                 int room_id,
                                 const char* errors[],
                                 int error_count)
{
    int i;
    int first_flag;

    printf("{\n");

    /* Player Position */
    printf("  \"player_pos\": {\"x\": %d, \"y\": %d, \"z\": %d},\n",
           (int)player->x, (int)player->y, (int)player->z);

    /* Yaw */
    printf("  \"yaw\": %d,\n", (int)player->yaw);

    /* Stage und Room */
    printf("  \"stage\": %d,\n", stage);
    printf("  \"room\": %d,\n", room_id);

    /* Flags — nur gesetzte Flags ausgeben (Index : Wert) */
    printf("  \"flags\": [");
    first_flag = 1;
    for (i = 0; i < 256; i++) {
        if (vm->game_flags[i] != 0) {
            if (!first_flag) printf(", ");
            printf("%d", i);
            first_flag = 0;
        }
    }
    printf("],\n");

    /* Errors */
    printf("  \"errors\": [");
    for (i = 0; i < error_count; i++) {
        if (i > 0) printf(", ");
        printf("\"%s\"", errors[i]);
    }
    printf("]\n");

    printf("}\n");
}

/* ============================================================================
 * Headless-Modus Ausführung
 * ========================================================================= */

/**
 * Führt den Headless-Modus aus.
 *
 * 1. I/O-Backend initialisieren (kein SDL nötig)
 * 2. Raum laden
 * 3. N SCD-Ticks ausführen (ohne Rendering)
 * 4. JSON-Ergebnis auf stdout ausgeben ODER Savestate-Vergleich durchführen
 *
 * Wenn --compare angegeben:
 *   - Lädt den Savestate aus der JSON-Datei
 *   - Vergleicht nach N Ticks: Position (±1 Toleranz), Yaw (exakt),
 *     Flags (exakt), Inventar (exakt)
 *   - Gibt bei Mismatch JSON mit abweichenden Feldern aus
 *   - Exit 0 = match, 1 = mismatch/error
 *
 * @param cfg  Headless-Konfiguration
 * @return     EXIT_SUCCESS (0) bei Erfolg/Match, EXIT_FAILURE (1) bei Fehler/Mismatch
 */
static int run_headless(const headless_config_t* cfg)
{
    re15_player_t player;
    re15_rdt_t    rdt;
    const char*   errors[RE15_HEADLESS_MAX_ERRORS];
    int           error_count = 0;
    int           rc;
    int           tick;
    int           stage;
    int           room_id;

    /* --- Subsysteme initialisieren (kein SDL/Rendering nötig) --- */
    g_io = &g_io_pc_backend;

    memset(&player, 0, sizeof(player));
    memset(&rdt, 0, sizeof(rdt));

    /* g_game initialisieren */
    memset(&g_game, 0, sizeof(g_game));
    g_game.player = &player;
    g_game.current_rdt = &rdt;
    g_game.scd = &g_scd;

    /* SCD-VM initialisieren */
    scd_vm_init();

    /* --- Raum-Parameter bestimmen --- */
    if (!cfg->room_specified) {
        /* Fallback: ROOM1240 (Startknoten) */
        fprintf(stderr, "[RE15 INFO]  HEADLESS: Kein --stage/--room angegeben, nutze ROOM1240 (Stage 1, Room 0x24)\n");
        stage   = 1;
        room_id = 0x24;
    } else {
        stage   = cfg->stage   ? cfg->stage   : 1;
        room_id = cfg->room_id;
    }

    /* --- Raum laden --- */
    rc = re15_room_load((uint8_t)stage, (uint8_t)room_id, 0);

    if (rc != 0) {
        if (error_count < RE15_HEADLESS_MAX_ERRORS) {
            errors[error_count++] = "room_load_failed";
        }

        if (cfg->compare_enabled) {
            /* Bei Vergleichsmodus: Fehler-JSON ausgeben */
            re15_cmp_result_t cmp_result;
            memset(&cmp_result, 0, sizeof(cmp_result));
            cmp_result.result = RE15_CMP_ERROR;
            snprintf(cmp_result.error_msg, sizeof(cmp_result.error_msg),
                     "room_load_failed: stage=%d room=0x%02X", stage, room_id);
            re15_savestate_print_result(&cmp_result);
        } else {
            headless_output_json(&player, &g_scd, stage, room_id, errors, error_count);
        }
        return EXIT_FAILURE;
    }

    /* --- SCD-Ticks ausführen (ohne Rendering) --- */
    for (tick = 0; tick < cfg->ticks; tick++) {
        scd_vm_tick();
    }

    /* --- Vergleichsmodus (--compare) --- */
    if (cfg->compare_enabled) {
        re15_savestate_t    savestate;
        re15_cmp_result_t   cmp_result;
        int                 inv_array[RE15_SAVESTATE_MAX_INVENTORY];
        int                 inv_count = 0;

        /* Savestate laden */
        if (re15_savestate_load(cfg->compare_path, &savestate) != 0) {
            re15_cmp_result_t err_result;
            memset(&err_result, 0, sizeof(err_result));
            err_result.result = RE15_CMP_ERROR;
            snprintf(err_result.error_msg, sizeof(err_result.error_msg),
                     "savestate_load_failed: %s", cfg->compare_path);
            re15_savestate_print_result(&err_result);
            re15_room_unload();
            return EXIT_FAILURE;
        }

        /* Aktuelles Inventar sammeln (derzeit leer — Platzhalter) */
        /* TODO: Wenn Inventar-System aktiv, hier g_inv auslesen */
        inv_count = 0;

        /* Vergleich durchführen */
        re15_savestate_compare(
            &savestate,
            player.x, player.y, player.z,
            player.yaw,
            g_scd.game_flags, 256,
            inv_array, inv_count,
            &cmp_result
        );

        /* Ergebnis-JSON ausgeben */
        re15_savestate_print_result(&cmp_result);

        /* Aufräumen */
        re15_room_unload();

        /* Exit-Code: 0 = match, 1 = mismatch oder error */
        return (cmp_result.result == RE15_CMP_MATCH) ? EXIT_SUCCESS : EXIT_FAILURE;
    }

    /* --- Normaler Headless-Modus: JSON-Ergebnis auf stdout ausgeben --- */
    headless_output_json(&player, &g_scd, stage, room_id, errors, error_count);

    /* --- Aufräumen --- */
    re15_room_unload();

    return EXIT_SUCCESS;
}

/* ============================================================================
 * main — Einstiegspunkt
 * ========================================================================= */

int main(int argc, char* argv[])
{
    headless_config_t headless_cfg;
    re15_player_t     player;
    re15_rdt_t        rdt;
    re15_sca_data_t   sca_data;
    re15_rvd_data_t   rvd_data;
    re15_input_state_t input;
    SDL_Event         event;
    int               running = 1;
    int               rc;
    uint8_t           new_cam;

    /* --------------------------------------------------------------------- */
    /* CLI-Argumente parsen                                                    */
    /* --------------------------------------------------------------------- */
    if (parse_cli_args(argc, argv, &headless_cfg) != 0) {
        return EXIT_FAILURE;
    }

    /* --compare impliziert --headless (Vergleich braucht kein Fenster) */
    if (headless_cfg.compare_enabled) {
        headless_cfg.enabled = 1;
    }

    /* --------------------------------------------------------------------- */
    /* Headless-Modus: Kein SDL-Fenster, kein Rendering                       */
    /* --------------------------------------------------------------------- */
    if (headless_cfg.enabled) {
        return run_headless(&headless_cfg);
    }

    /* --------------------------------------------------------------------- */
    /* SDL Basis-Subsysteme initialisieren                                     */
    /* --------------------------------------------------------------------- */
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_GAMECONTROLLER) != 0) {
        fprintf(stderr, "[RE15 ERROR] SDL: SDL_Init fehlgeschlagen: %s\n", SDL_GetError());
        return EXIT_FAILURE;
    }

    /* --------------------------------------------------------------------- */
    /* Spieler-Entität initialisieren                                          */
    /* --------------------------------------------------------------------- */
    memset(&player, 0, sizeof(player));
    player.radius = 200;  /* Standard-Kollisionsradius */
    memset(&rdt, 0, sizeof(rdt));
    memset(&sca_data, 0, sizeof(sca_data));
    memset(&rvd_data, 0, sizeof(rvd_data));

    /* --------------------------------------------------------------------- */
    /* Globale Engine-Struktur initialisieren                                  */
    /* --------------------------------------------------------------------- */
    memset(&g_game, 0, sizeof(g_game));
    g_game.player = &player;
    g_game.current_rdt = &rdt;
    g_game.scd = &g_scd;

    /* --------------------------------------------------------------------- */
    /* I/O-Backend initialisieren                                              */
    /* --------------------------------------------------------------------- */
    g_io = &g_io_pc_backend;

    /* --------------------------------------------------------------------- */
    /* Audio-Backend initialisieren                                            */
    /* --------------------------------------------------------------------- */
    g_audio = &audio_pc_backend;
    g_audio->init();

    /* --------------------------------------------------------------------- */
    /* Render-Backend initialisieren (erstellt Fenster + GL-Kontext)           */
    /* --------------------------------------------------------------------- */
    g_render = &render_pc_backend;
    g_render->init();

    /* --------------------------------------------------------------------- */
    /* Input-System initialisieren                                             */
    /* --------------------------------------------------------------------- */
    re15_input_init();

    /* --------------------------------------------------------------------- */
    /* SCD-VM initialisieren                                                   */
    /* --------------------------------------------------------------------- */
    scd_vm_init();

    /* --------------------------------------------------------------------- */
    /* Debug-Overlay-System initialisieren                                     */
    /* --------------------------------------------------------------------- */
    debug_overlay_init();

    /* --------------------------------------------------------------------- */
    /* Initialen Raum laden: ROOM1240 (Startknoten)                           */
    /* --------------------------------------------------------------------- */
    rc = re15_room_load(RE15_START_STAGE, RE15_START_ROOM, 0);
    if (rc != 0) {
        fprintf(stderr, "[RE15 ERROR] Initialer Raum ROOM1240 konnte nicht geladen werden.\n");
        /* Graceful: Weiter laufen lassen, evtl. schwarzer Bildschirm */
    } else {
        /* SCA-Kollisionsdaten parsen (wenn vorhanden) */
        if (g_game.current_rdt && g_game.current_rdt->collision) {
            re15_sca_parse(g_game.current_rdt->collision,
                           g_game.rdt_buffer_size, &sca_data);
        }
        /* RVD-Kamerazonen parsen (wenn vorhanden) */
        if (g_game.current_rdt && g_game.current_rdt->zone) {
            re15_rvd_parse(g_game.current_rdt->zone,
                           g_game.rdt_buffer_size, &rvd_data);
        }
    }

    fprintf(stdout, "[RE15 INFO]  Game-Loop gestartet (30 FPS)\n");

    /* ===================================================================== */
    /* Hauptloop (30 FPS)                                                      */
    /*                                                                         */
    /* Subsystem-Reihenfolge pro Frame:                                        */
    /*   1. Input-Tick (SDL-Events + Pad-Polling)                              */
    /*   2. Player-Update (Tank-Controls)                                      */
    /*   3. Kollisionsprüfung (SCA)                                            */
    /*   4. AOT-Check (Trigger-Zonen)                                          */
    /*   5. Kamerazone-Check (RVD)                                             */
    /*   6. SCD-VM-Tick (Skript-Ausführung)                                    */
    /*   7. Render (begin_frame → submit → end_frame)                          */
    /* ===================================================================== */
    while (running) {

        /* --- 1. Event-Verarbeitung + Input-Tick --- */
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = 0;
            }
            if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE) {
                running = 0;
            }
            /* Debug-Overlay Tastenkürzel (F1-F5) */
            if (event.type == SDL_KEYDOWN && !event.key.repeat) {
                debug_overlay_toggle(event.key.keysym.sym);
            }
        }

        if (!running) break;

        /* Input-Subsystem aktualisieren (Keyboard/Gamepad → Button-State) */
        re15_input_tick();
        input = re15_input_get();

        /* --- 2. Player-Update (Tank-Controls: Rotation + Bewegung) --- */
        re15_player_update(&player, input);

        /* --- 3. Kollisionsprüfung gegen SCA-Daten --- */
        if (sca_data.count > 0) {
            re15_collision_check(&player, &sca_data);
        }

        /* --- 4. AOT-Check (Trigger-Zonen: Türen, Items, Events) --- */
        re15_aot_check((int16_t)(player.x), (int16_t)(player.z),
                       player.floor_band);

        /* --- 5. Kamerazone-Check (RVD: Kamerawechsel bei Zonenübertritt) --- */
        if (rvd_data.count > 0) {
            new_cam = g_game.current_cut;
            re15_camera_check_zones(
                (int16_t)(player.x), (int16_t)(player.z),
                &rvd_data, g_game.current_cut, &new_cam);

            if (new_cam != g_game.current_cut) {
                g_game.current_cut = new_cam;
                /* BSS-Hintergrund für neue Kamera laden */
                if (g_render->show_background) {
                    g_render->show_background(g_game.bss_chunk, new_cam);
                }
            }
        }

        /* --- 6. SCD-VM-Tick (Skript-Ausführung, 30 Hz) --- */
        scd_vm_tick();

        /* Kamerawechsel durch SCD-Opcode (Cut_chg) verarbeiten */
        if (g_scd.cam_change_pending) {
            g_game.current_cut = g_scd.cam_id;
            g_scd.cam_change_pending = 0;
            if (g_render->show_background) {
                g_render->show_background(g_game.bss_chunk, g_scd.cam_id);
            }
        }

        /* --- 7. Rendering --- */
        g_render->begin_frame();

        /* BSS-Hintergrund anzeigen (wird vom Backend als Hintergrund gezeichnet) */
        /* (Bereits beim Kamerawechsel geladen — end_frame zeichnet automatisch) */

        /* 3D-Modelle und Sprites werden von Subsystemen via submit_tri/quad
         * eingereicht (Entity-Rendering wird in späteren Tasks implementiert) */

        /* Debug-Overlays rendern (nach Spielinhalt, vor end_frame) */
        {
            SDL_Renderer* dbg_renderer = render_pc_get_renderer();
            if (dbg_renderer) {
                debug_overlay_render(dbg_renderer);
            }
        }

        g_render->end_frame();
    }

    /* --------------------------------------------------------------------- */
    /* Aufräumen                                                               */
    /* --------------------------------------------------------------------- */
    re15_room_unload();

    if (g_audio && g_audio->shutdown) {
        g_audio->shutdown();
    }

    if (g_render && g_render->shutdown) {
        g_render->shutdown();
    }

    SDL_Quit();

    return EXIT_SUCCESS;
}
