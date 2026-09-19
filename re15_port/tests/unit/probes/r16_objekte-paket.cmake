# Sonden-Registrierung Runde 16 (2026-09-19) — Thema "objekte-paket": Props (Leiche 1050,
# Kisten 1090, Schalter 11F0) fehlen, sobald ein Raum mit bereits genommenem Item durchlaufen
# wurde (s_prop_taken_hidden in scd_vm.c ueberlebt den Raumwechsel). Dossier:
# analysis/befunde_2026-09-19/objekte-paket.md. Reine Messsonde; Rueckgabe != 0 solange der
# Defekt besteht. NACH dem Fix als add_test registrieren (dann ist sie der Pin):
#   add_test(NAME probe_r16_objekte_paket COMMAND probe_r16_objekte_paket)
add_executable(probe_r16_objekte_paket probe_r16_objekte_paket.c)
target_link_libraries(probe_r16_objekte_paket PRIVATE re15_engine re15_test_support)
target_include_directories(probe_r16_objekte_paket PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(probe_r16_objekte_paket PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX"
    RE15_ASSET_RE2_DIR="${CMAKE_SOURCE_DIR}/shared_assets/RE2")
# Phase 2 (2026-09-19): nach dem Fix (scd_prop_taken_mask_reset in scd_room_reenter, Verstecken per
# obj_id) ist die Sonde der Pin — A (frisch), B/C/D (Folgeraum sichtbar), E (gleicher Raum versteckt).
add_test(NAME probe_r16_objekte_paket COMMAND probe_r16_objekte_paket)
set_tests_properties(probe_r16_objekte_paket PROPERTIES TIMEOUT 60)
