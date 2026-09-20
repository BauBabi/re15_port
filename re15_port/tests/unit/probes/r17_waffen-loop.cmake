# Runde 17 (2026-09-20) — Thema "waffen-loop": die Routine-15-Ketten der Burst-Pistolen
# (Beretta M93R = Item 5, Glock 18 = Item 6; FX-SUB 2 aus Entlade-Handler 0x800338A8).
# Pin: jede Kette spawnt genau DREI Kinder, gibt danach ihren Platz frei (Original
# @0x80017b6c sb zero,108(v1)) und der Effekt-Pool ist 300 Bilder spaeter leer.
# Dossier: analysis/befunde_2026-09-20/waffen-loop.md
add_executable(probe_r17_waffen_loop probe_r17_waffen_loop.c)
target_link_libraries(probe_r17_waffen_loop PRIVATE re15_engine re15_test_support)
target_include_directories(probe_r17_waffen_loop PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(probe_r17_waffen_loop PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX")
add_test(NAME unit_r17_waffen_loop_pin COMMAND probe_r17_waffen_loop pin)
set_tests_properties(unit_r17_waffen_loop_pin PROPERTIES TIMEOUT 60)
