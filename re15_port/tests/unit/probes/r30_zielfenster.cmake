# --- Runde 30 (2026-09-27), Thema "das fuenfte Tor von FUN_800470C0 — das senkrechte
#     Zielfenster". WELLE 1 ist reine MESSUNG: die Sonde baut das Tor NICHT ein, sie rechnet
#     es nur durch und misst, was der Port an seinen drei Eingaengen heute traegt.
#       Tor:    @0x8004716C-A4  (lh +0x98 / lw +0x3C / lhu +0x9E / lw 4(s4) / sltu / beq)
#       Ziel:   s4+4 = MATRIX.t[1] der Waffen-Bone-Kette (@0x80042E60-94, @0x80042F8C)
#       Hitbox: FUN_80104088 @0x80104088-D8 (Hund), Overlay-INITs je Typ (s. Sondenkopf)
#     Kein add_test: es gibt in dieser Welle nichts zu verriegeln — das Tor ist nicht gebaut.
add_executable(probe_r30_zielfenster probe_r30_zielfenster.c)
target_link_libraries(probe_r30_zielfenster PRIVATE re15_engine re15_test_support)
target_include_directories(probe_r30_zielfenster PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(probe_r30_zielfenster PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX")
