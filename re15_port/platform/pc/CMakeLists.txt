# =============================================================================
# PC-Target — SDL2/OpenGL via FetchContent
# =============================================================================
# SDL2 wird automatisch via FetchContent heruntergeladen (keine manuelle
# Installation auf Windows nötig). Auf Linux: libSDL2-dev, libGL, libX11
# müssen installiert sein.
# =============================================================================

include(FetchContent)

# -----------------------------------------------------------------------------
# SDL2 via FetchContent (Version 2.28.5) — STATISCH gebaut.
# So läuft die exe ohne separate SDL2.dll (selbst-enthalten, doppelklickbar).
# KEIN FIND_PACKAGE_ARGS: immer aus Quelle bauen — sonst zieht ein dynamisches
# System-SDL2 die DLL-Abhängigkeit zurück.
# -----------------------------------------------------------------------------
set(SDL_SHARED OFF CACHE BOOL "" FORCE)
set(SDL_STATIC ON  CACHE BOOL "" FORCE)
set(SDL_TEST   OFF CACHE BOOL "" FORCE)

FetchContent_Declare(
    SDL2
    URL https://github.com/libsdl-org/SDL/releases/download/release-2.28.5/SDL2-2.28.5.tar.gz
    URL_HASH SHA256=332cb37d0be20cb9541739c61f79bae5a477427d79ae85e352089afdaf6666e4
)

FetchContent_MakeAvailable(SDL2)

# Prüfe ob das statische SDL2-Target verfügbar ist
if(NOT TARGET SDL2::SDL2-static)
    message(FATAL_ERROR
        "SDL2-static konnte nicht gebaut werden (FetchContent).\n"
        "Mögliche Ursachen:\n"
        "  - Keine Netzwerkverbindung (FetchContent benötigt Download)\n"
        "  - GitHub nicht erreichbar\n"
        "Abhängigkeit: SDL2 2.28.5 (statisch)"
    )
endif()

# -----------------------------------------------------------------------------
# Plattform-spezifische Quellen (src/)
# -----------------------------------------------------------------------------
file(GLOB PC_PLATFORM_SOURCES "src/*.c")

# -----------------------------------------------------------------------------
# PC-Executable
# -----------------------------------------------------------------------------
add_executable(re15_pc main.c ${PC_PLATFORM_SOURCES})

target_compile_definitions(re15_pc PRIVATE RE15_PLATFORM_PC)

# EINZIGE Asset-Wurzel (2026-07-02): der in-repo Baum shared_assets/PSX enthält jetzt
# ALLES — den rohen CD-Baum (RDT unter STAGE{N}/, DATA, gepackte Container) PLUS die
# vor-extrahierten Assets (PLD-Split TIM/MD1/EDD/EMR, RBJ/, per-cut BSS/ROOM####/…).
# Die alte psx_dev/assets_shared-Abhängigkeit ist damit entfernt. Beide Defaults zeigen
# auf denselben Baum; RE15_ASSET_ROOT / RE15_CD_ROOT (env) übersteuern zur Laufzeit.
target_compile_definitions(re15_pc PRIVATE
    RE15_ASSET_ROOT_DEFAULT="${CMAKE_SOURCE_DIR}/shared_assets/PSX"
    RE15_CD_ROOT_DEFAULT="${CMAKE_SOURCE_DIR}/shared_assets/PSX"
)

target_include_directories(re15_pc PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)

target_link_libraries(re15_pc
    PRIVATE
        re15_engine
        SDL2::SDL2-static
)

# MinGW nutzt ein reguläres C-main() (psx_dev main.c inkludiert nur <SDL_timer.h>,
# nicht <SDL.h> → kein SDL_main-Makro). SDL2main dort zu linken zieht ein
# unauflösbares SDL_main / WinMain. Daher nur auf Nicht-MinGW linken.
if(NOT MINGW)
    target_link_libraries(re15_pc PRIVATE SDL2::SDL2main)
endif()

# Mathe-Lib auf GCC/Clang explizit (auf MSVC automatisch).
if(NOT MSVC)
    target_link_libraries(re15_pc PRIVATE m)
endif()

# MinGW: gcc-Runtime statisch linken → exe ohne libgcc_s_/libwinpthread-DLLs.
# Zusammen mit dem statischen SDL2 ist die exe damit voll selbst-enthalten.
if(MINGW)
    target_link_options(re15_pc PRIVATE -static)
endif()

# -----------------------------------------------------------------------------
# GUI-SUBSYSTEM (2026-08-17) — Fix fuer "startet nicht, kein Fenster"
# -----------------------------------------------------------------------------
# Die exe war bis hierher eine KONSOLEN-Anwendung (PE-Subsystem 3 "Windows CUI").
# Beim Start OHNE vorhandene Konsole (Doppelklick, `start`, Start-Process) muss der
# Windows-Loader dann erst eine Konsole erzeugen — und genau dort blieb der Prozess
# auf betroffenen Systemen haengen, NOCH BEVOR main() lief. Per gdb an den haengenden
# Prozess (2026-08-17, Nutzer-Rechner UND hier reproduziert):
#     #0  ntdll!ZwCreateFile
#     #1  KERNELBASE!AttachConsole
#     ...
#     #14 ntdll!LdrLoadDll
#     #19 ntdll!LdrInitializeThunk          <- noch im Loader, vor main()
# Symptom: Prozess lebt (1 Thread, 0% CPU), KEIN Fenster, nicht einmal debug.log
# (die legt die erste Zeile von main() an). Aus einer BEREITS OFFENEN Konsole
# gestartet lief dieselbe exe normal durch — sie erbt dort die Konsole.
#
# Ein SDL-Spiel ist eine GUI-Anwendung; Subsystem 2 ("Windows GUI") braucht gar keine
# Konsole und umgeht den Loader-Pfad komplett. Bei MinGW wird NUR das Subsystem
# umgestellt, der Entry-Point bleibt mainCRTStartup — sonst verlangte der Linker ein
# WinMain(), und main.c hat ein regulaeres C-main().
# Ausgaben gehen dadurch nicht verloren: stderr landet ohnehin per freopen in
# debug.log, und main() haengt sich beim Start an eine vorhandene Eltern-Konsole an
# (AttachConsole(ATTACH_PARENT_PROCESS)), so dass Shell-Laeufe und CTest wie bisher
# stdout sehen.
if(WIN32)
    if(MINGW)
        target_link_options(re15_pc PRIVATE -Wl,--subsystem,windows -Wl,-e,mainCRTStartup)
    else()
        set_target_properties(re15_pc PROPERTIES WIN32_EXECUTABLE TRUE)
    endif()
endif()
