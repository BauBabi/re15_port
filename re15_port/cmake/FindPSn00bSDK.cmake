# =============================================================================
# FindPSn00bSDK.cmake — SDK-Detektion für PSn00bSDK
# =============================================================================
# Sucht PSn00bSDK am Pfad PSN00BSDK_PATH und validiert die Installation.
#
# Ergebnis-Variablen:
#   PSn00bSDK_FOUND        - TRUE wenn SDK gefunden und valide
#   PSn00bSDK_INCLUDE_DIR  - Include-Verzeichnis
#   PSn00bSDK_LIBRARIES    - Library-Verzeichnis
#   PSn00bSDK_VERSION      - SDK-Version (falls ermittelbar)
# =============================================================================

# Sicherstellen dass PSN00BSDK_PATH definiert ist
if(NOT DEFINED PSN00BSDK_PATH)
    if(DEFINED ENV{PSN00BSDK_PATH})
        set(PSN00BSDK_PATH "$ENV{PSN00BSDK_PATH}")
    endif()
endif()

if(NOT DEFINED PSN00BSDK_PATH)
    set(PSn00bSDK_FOUND FALSE)
    if(PSn00bSDK_FIND_REQUIRED)
        message(FATAL_ERROR
            "PSn00bSDK nicht gefunden: PSN00BSDK_PATH ist nicht gesetzt.\n"
            "Bitte setze PSN00BSDK_PATH auf das PSn00bSDK-Installationsverzeichnis.\n"
            "Installationsanleitung: https://github.com/Lameguy64/PSn00bSDK"
        )
    endif()
    return()
endif()

# Prüfe ob der angegebene Pfad existiert
if(NOT EXISTS "${PSN00BSDK_PATH}")
    set(PSn00bSDK_FOUND FALSE)
    if(PSn00bSDK_FIND_REQUIRED)
        message(FATAL_ERROR
            "PSn00bSDK nicht gefunden: Pfad existiert nicht: ${PSN00BSDK_PATH}\n"
            "Bitte prüfe den PSN00BSDK_PATH.\n"
            "Installationsanleitung: https://github.com/Lameguy64/PSn00bSDK"
        )
    endif()
    return()
endif()

# Prüfe ob wichtige SDK-Komponenten vorhanden sind
find_path(PSn00bSDK_INCLUDE_DIR
    NAMES "psxgpu.h" "psxgte.h" "psxspu.h"
    PATHS "${PSN00BSDK_PATH}/include"
    NO_DEFAULT_PATH
)

find_path(PSn00bSDK_LIBRARIES
    NAMES "libpsxgpu.a" "libpsxgte.a"
    PATHS "${PSN00BSDK_PATH}/lib"
    NO_DEFAULT_PATH
)

# Ergebnis bestimmen
if(PSn00bSDK_INCLUDE_DIR AND PSn00bSDK_LIBRARIES)
    set(PSn00bSDK_FOUND TRUE)
    message(STATUS "PSn00bSDK gefunden: ${PSN00BSDK_PATH}")
    message(STATUS "  Include: ${PSn00bSDK_INCLUDE_DIR}")
    message(STATUS "  Libraries: ${PSn00bSDK_LIBRARIES}")
else()
    set(PSn00bSDK_FOUND FALSE)
    if(PSn00bSDK_FIND_REQUIRED)
        message(FATAL_ERROR
            "PSn00bSDK Installation unvollständig unter: ${PSN00BSDK_PATH}\n"
            "Fehlende Komponenten: "
            "${PSn00bSDK_INCLUDE_DIR_MISSING}${PSn00bSDK_LIBRARIES_MISSING}\n"
            "Bitte installiere PSn00bSDK erneut.\n"
            "Installationsanleitung: https://github.com/Lameguy64/PSn00bSDK"
        )
    endif()
endif()

# Standard-CMake FindPackageHandleStandardArgs Kompatibilität
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(PSn00bSDK
    REQUIRED_VARS PSN00BSDK_PATH PSn00bSDK_INCLUDE_DIR PSn00bSDK_LIBRARIES
)

mark_as_advanced(PSn00bSDK_INCLUDE_DIR PSn00bSDK_LIBRARIES)
