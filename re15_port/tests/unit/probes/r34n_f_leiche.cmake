# Runde 34 Nacht, Spur F (Leichen ROOM1110 / ROOM1230): MESS-SONDE, kein Pin.
# Misst die zwei Original-Leichen-Ereignisse (ROOM1110 Slot 5 -> sub02, ROOM1230 Slot 18 -> sub21)
# im echten Bild-Ablauf des Ports: Nachricht, Seiten, Freeze, Faden-Programmzaehler, Item-Modal.
# Dossier: analysis/befunde_runde34_nacht/F_leichen.md (Abschnitte 2 und 4).
add_executable(probe_r34n_f_leiche ${CMAKE_CURRENT_SOURCE_DIR}/probe_r34n_f_leiche.c)
target_link_libraries(probe_r34n_f_leiche PRIVATE re15_engine re15_test_support)
target_include_directories(probe_r34n_f_leiche PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(probe_r34n_f_leiche PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX")
