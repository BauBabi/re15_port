#!/usr/bin/env bash
W=/c/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/w2abn
export PATH=/c/msys64/mingw64/bin:$PATH
export SDL_ASSERT=always_ignore
B=/c/workspace/git/reAi_v2/.claude/worktrees/r41_form/re15_port/build
ctest --test-dir "$B" -R "^(integration_r41_form_wagenweg|unit_r41_form_emd|unit_birkin_bahnsteig|integration_r34_plattform_takt|integration_r34_plattform_esp_eintritt)$" -j 3 --timeout 900 --output-on-failure -V > "$W/ctest/set_a.log" 2>&1
echo "ctest rc=$?" >> "$W/ctest/set_a.log"
