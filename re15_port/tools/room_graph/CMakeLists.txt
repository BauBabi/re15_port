# =============================================================================
# Room-Graph Tools (Asset-Pipeline)
# =============================================================================

# Door_aot_set_4p-Extraktor — standalone, keine Engine-Abhängigkeit
add_executable(extract_doors extract_doors.c)
target_compile_options(extract_doors PRIVATE
    $<$<C_COMPILER_ID:GNU>:-Wall -Wextra -pedantic>
    $<$<C_COMPILER_ID:MSVC>:/W4>
)

# Raum-Graph-Generator — liest CSV von extract_doors, erzeugt JSON + Markdown
add_executable(generate_graph generate_graph.c)
target_compile_options(generate_graph PRIVATE
    $<$<C_COMPILER_ID:GNU>:-Wall -Wextra -pedantic>
    $<$<C_COMPILER_ID:MSVC>:/W4>
)
