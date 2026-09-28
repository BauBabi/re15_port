# Runde 30 (2026-09-27), Thema D "titel-blinken": das Titelmenue blinkt zu schnell.
# Dossier: analysis/befunde_runde30/titel-blinken.md
# Reine Messsonde (kein add_test): fuehrt den ORIGINAL-Puls-Handler FUN_801028ec aus den
# ausgelieferten Bytes von BIN/TITLE.BIN aus und prueft jedes im Dossier zitierte
# Befehlswort in TITLE.BIN und PSX.EXE. Sie haengt NICHT an der Engine.
add_executable(probe_r30_titel_blinken_puls "probe_r30_titel-blinken_puls.c")
target_compile_definitions(probe_r30_titel_blinken_puls PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX"
    RE15_R30_PSX_EXE="${CMAKE_SOURCE_DIR}/../info/Re1.5/PSX.EXE")
