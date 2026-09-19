# Phase 2 (2026-09-19), Thema "objekte-und-tuer": Pins zu den Runde-16-Dossiers
#   analysis/befunde_2026-09-19/objekte-paket.md      (add_test in r16_objekte-paket.cmake)
#   analysis/befunde_2026-09-19/tuer-animation-1040.md (Pins hier)
# probe_r16_tuer1040 (Ziel in r16_tuer-animation-1040.cmake) prueft den Raumeintritt
# ROOM1030 -> ROOM1040 (Block @0x5C) und zurueck: cmd-0-Endzustand @0x80031c10/c18/c20 =
# W-Bank Clip 1 Bild 0 hart waehrend der Blende (t=0..4), Freigabe t=5 (Idle case 0
# @0x80032088/@0x8003209c), Render-Pose t=12 == W Clip 3 Bild 0. Bankabhaengig: A=W01 (Messer),
# B=W03 (Handfeuerwaffe) — c1f0==c3f0, keine sichtbare Bewegung; F=W0F (Item 15) — c1f0!=c3f0,
# 7-Bild-Blend wie im Original; N = Renderer ohne W-Bank (PSX-Rueckfall auf PL00 Clip 6).
add_test(NAME probe_r16_tuer1040_A COMMAND probe_r16_tuer1040 A)
add_test(NAME probe_r16_tuer1040_B COMMAND probe_r16_tuer1040 B)
add_test(NAME probe_r16_tuer1040_F COMMAND probe_r16_tuer1040 F)
add_test(NAME probe_r16_tuer1040_N COMMAND probe_r16_tuer1040 N)
set_tests_properties(probe_r16_tuer1040_A probe_r16_tuer1040_B probe_r16_tuer1040_F
                     probe_r16_tuer1040_N PROPERTIES TIMEOUT 60)
