# --- Runde 30 (2026-09-27), Thema "das fuenfte Tor von FUN_800470C0 — das senkrechte
#     Zielfenster". WELLE 1 ist reine MESSUNG: die Sonde baut das Tor NICHT ein, sie rechnet
#     es nur durch und misst, was der Port an seinen drei Eingaengen heute traegt.
#       Tor:    @0x8004716C-A4  (lh +0x98 / lw +0x3C / lhu +0x9E / lw 4(s4) / sltu / beq)
#       Ziel:   s4+4 = MATRIX.t[1] der Waffen-Bone-Kette (@0x80042E60-94, @0x80042F8C)
#       Hitbox: FUN_80104088 @0x80104088-D8 (Hund), Overlay-INITs je Typ (s. Sondenkopf)
#     RIEGEL (Argument "riegel"): NICHT das Tor — das ist nicht gebaut, ihm fehlt die
#     Muendungshoehe. Verriegelt wird nur die HITBOX-STAUCHUNG des Hundes FUN_80104088:
#       vor dem Treffer -1000/1000 (INIT @0x8010028C-9C)
#       waehrend HURT    -500/500  (FUN_80104088(0) @0x80104098-A8, gerufen @0x80103458/@0x8010352C)
#       nach der Kette  -1000/1000 (FUN_80104088(1) @0x801040B8-C4, gerufen @0x801036F0/@0x801037C0)
#     Gegen-Riegel: die HURT-Kette MUSS enden und die Box MUSS wieder aufgehen (sonst waere
#     die Stauchung die Hitbox-Variante der Runde-13/14-Dauersperre).
add_executable(probe_r30_zielfenster probe_r30_zielfenster.c)
target_link_libraries(probe_r30_zielfenster PRIVATE re15_engine re15_test_support)
target_include_directories(probe_r30_zielfenster PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(probe_r30_zielfenster PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX")
add_test(NAME r30_hund_hitbox_stauchung COMMAND probe_r30_zielfenster riegel)
set_tests_properties(r30_hund_hitbox_stauchung PROPERTIES TIMEOUT 240)
