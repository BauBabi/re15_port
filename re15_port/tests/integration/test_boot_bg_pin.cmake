# =============================================================================
# BOOT-BG-PIN (Nutzer-Report v0.1.1, gefixt 2026-08-10): zwischen Select-Exit
# und den ROOM1240-Intro-Stills darf KEIN Frame einen Fremd-Raum-BG zeigen.
#
# Gemessener Defekt: der Boot-BG-Preload lief VOR `g_current_room_id = boot_room`
# und cachte ROOM1170#00 (g_current_room_id-Default, room_common.c:34); Game-
# Frame 0 blittete den Cache VOR dem Cut-Wechsel-Block -> 1 ungedeckter Frame
# ROOM1170-Helipad. Original-SOLL: Raumlader FUN_8001d600 haelt Modus-2-SCHWARZ
# (FUN_80021634(2,0) @0x8001d620-28 Boot / @0x8001d830-34 Tuer) bis Raum+Entry-
# Cut-BG geladen sind (FUN_80013c50 @0x8001da80, Waits @0x8001d850-868 /
# @0x8001dabc-d4), Freigabe erst @0x8001dadc-ec (FUN_80021634(0,0) +
# DAT_800b5457=1) -> erster sichtbarer Frame = BG des GELADENEN Raums.
#
# Pin: re15_pc.exe faehrt den echten Boot (Title-Auto-Advance -> NEW GAME ->
# ROOM1240, Exit bei Game-Frame 34 via RE15_INV_SHOT) mit RE15_FADE_LOG-
# Instrumentierung; debug.log MUSS als erste Blit-Zeile
#   [bg-log] F0 blit room1240#00 (room=1240)
# enthalten und darf im gesamten Fenster keinen room1170-Load/Blit zeigen.
#
# Aufruf: cmake -DRE15_PC_EXE=<exe> -DWORKDIR=<dir> -P test_boot_bg_pin.cmake
# =============================================================================

if(NOT RE15_PC_EXE OR NOT EXISTS "${RE15_PC_EXE}")
    message(FATAL_ERROR "boot_bg_pin: RE15_PC_EXE fehlt/existiert nicht: '${RE15_PC_EXE}'")
endif()
if(NOT WORKDIR)
    message(FATAL_ERROR "boot_bg_pin: WORKDIR fehlt")
endif()

file(MAKE_DIRECTORY "${WORKDIR}")
file(REMOVE "${WORKDIR}/debug.log")

execute_process(
    COMMAND "${CMAKE_COMMAND}" -E env
            RE15_NO_INTRO=1              # Boot-FMV aus (irrelevant fuer den Pin, spart ~20s)
            RE15_TITLE_SHOT=title.bmp    # Title-Auto-Advance -> re15_gameflow_new_game(0)
            RE15_FADE_LOG=1              # [bg-log]/[fade-log]-Instrumentierung an
            RE15_INV_SHOT=exit34.bmp     # sauberer exit(0) bei Game-Frame 34
            "${RE15_PC_EXE}"
    WORKING_DIRECTORY "${WORKDIR}"
    TIMEOUT 100
    RESULT_VARIABLE rv
)
if(NOT rv EQUAL 0)
    message(FATAL_ERROR "boot_bg_pin: re15_pc.exe exit=${rv} (erwartet 0)")
endif()

if(NOT EXISTS "${WORKDIR}/debug.log")
    message(FATAL_ERROR "boot_bg_pin: kein debug.log in ${WORKDIR}")
endif()
file(STRINGS "${WORKDIR}/debug.log" bg_lines REGEX "\\[bg-log\\]")

# 1) Kein room1170 im gesamten Boot-Fenster (Load ODER Blit).
foreach(line IN LISTS bg_lines)
    if(line MATCHES "room1170")
        message(FATAL_ERROR "boot_bg_pin: ROOM1170-BG im Boot-Fenster geleakt: '${line}'")
    endif()
endforeach()

# 2) Erste Blit-Zeile MUSS Game-Frame 0 mit dem Boot-Raum-Entry-Cut sein.
set(first_blit "")
foreach(line IN LISTS bg_lines)
    if(line MATCHES " blit ")
        set(first_blit "${line}")
        break()
    endif()
endforeach()
if(NOT first_blit MATCHES "F0 blit room1240#00 \\(room=1240\\)")
    message(FATAL_ERROR "boot_bg_pin: erster Blit ist nicht ROOM1240 cut 0: '${first_blit}'")
endif()

message(STATUS "boot_bg_pin OK: '${first_blit}', kein room1170 im Boot-Fenster")
