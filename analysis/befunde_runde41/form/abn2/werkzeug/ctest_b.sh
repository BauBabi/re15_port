#!/usr/bin/env bash
W=/c/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/w2abn
export PATH=/c/msys64/mingw64/bin:$PATH
export SDL_ASSERT=always_ignore
B=/c/workspace/git/reAi_v2/.claude/worktrees/r41_form/re15_port/build
ctest --test-dir "$B" -R "^(unit_birkin_ai|unit_g5_tentakel|unit_g5_morph|unit_g5_boss|probe_p2_birkin_g5|probe_p3_birkin_rest|unit_r17_birkin_1zu1_pin|unit_r18_birkin_restluecken_pin|unit_r20_birkin_push_pin|unit_r30_birkin_frost|unit_r30_n_room5080_birkin_wand|unit_r34_plattform|unit_r34_plattform_ton|integration_r35_zgirl_5090|integration_r37_elzaems_leon5080|unit_birkin_formen|unit_r41_birkin_wand_rein|unit_r41_birkin_wand_g5|unit_r41_birkin_wand_griff|unit_n2_griff_start|unit_birkin_szenen|integration_r41_birkin_pistole|integration_r41_g5_westen|integration_r41_g5_tod|unit_birkin_opfer)$" -j 3 --timeout 900 --output-on-failure > "$W/ctest/set_b.log" 2>&1
echo "ctest rc=$?" >> "$W/ctest/set_b.log"
