# Sonde: die ABTASTPHASE der texturierten Dreiecke, geeicht an einem echten PSX-Bild.
#
# Anlass: der Nutzer meldet ZUM ZWEITEN MAL "der Cursor hat einen schraeg versetzten
# Schatten" (ROOM11F0, Cut 10). Der Schatten selbst ist Original-Kunst (in die Textur
# gebacken, ROOM11F0.RDT @Datei 0x018DAC, +5u/+3v) — falsch war, dass unser Kreuz
# ueberhaupt nicht HELL gezeichnet wurde.
#
# Grundwahrheit: DuckStation-Lauf, analysis/grundwahrheit/psx_room11f0_cut10.png/.sav.
# Herleitung und die komplette Phasen-Fahrt: re15_port/include/re15_abtastphase.h.
#
# GEGENPROBE nachgefahren (RE15_UV_ABTASTPHASE in include/re15_abtastphase.h getauscht,
# neu gebaut, `ctest -R unit_abtastphase_11f0`):
#   -0.5f (der Wert aus v0.8.13)  -> EXIT 1, 4 von 4 Pruefungen ROT
#                                    (x=160 -> u=67 = Index 2 Schatten; x=161 -> u=73 = Index 0;
#                                     y=118 -> v=42 = Index 2;          y=119 -> v=46 = Index 0)
#    0.0f (der Stand davor)       -> EXIT 1, 2 von 4 Pruefungen ROT
#                                    (x=160 -> u=64 = Index 2 Schatten; x=161 -> u=70 = Index 0)
#   +0.375f (gemessenes Optimum)  -> EXIT 0, GRUEN
#                                    (x=160 -> u=61 = Index 4 hell; x=161 -> u=67 = Index 2 Schatten;
#                                     y=118 -> v=38 = Index 4;      y=119 -> v=42 = Index 2)
# Der Riegel misst also wirklich diesen Defekt und nicht bloss sich selbst.
add_executable(probe_abtastphase_11f0 probe_abtastphase_11f0.c)
target_include_directories(probe_abtastphase_11f0 PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(probe_abtastphase_11f0 PRIVATE
    RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX")

add_test(NAME unit_abtastphase_11f0 COMMAND probe_abtastphase_11f0)
