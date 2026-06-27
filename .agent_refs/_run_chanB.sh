#!/bin/bash
# Render channel-B (dir[5]/dir[6]) clip CLIP at several frames, SOLO isolated zombie.
# Usage: _run_chanB.sh <clip>
set -e
CLIP=$1
EXE="c:/workspace/git/reAi/psx_dev/re15_reborn_pc/build/Debug/re15_reborn_pc.exe"
WD="c:/workspace/git/reAi/psx_dev/re15_reborn_pc"
OUT="c:/workspace/git/reAi/.agent_refs/chanB/clip$CLIP"
rm -rf "$OUT"; mkdir -p "$OUT"
cd "$WD"
rm -rf shots; mkdir -p shots
# self-advance: anim_frame = (frame_count/2)&0x3f, so frames 64..128 step 8 cover frames 0..32 of clip (each clip <=32 stored)
RE15_START_ROOM=1140 RE15_FORCE_CUT=1 RE15_FPS=30 RE15_PLAYER_POS=6000,-9000,1024 \
RE15_ZB_POOL=1 RE15_ZB_CLIP=$CLIP RE15_ZB_SOLO=1 RE15_AUTOSHOT=1 \
RE15_AUTOSHOT_SERIES=84,140,8 \
timeout 60 "$EXE" 2>/dev/null || true
for b in shots/series_*.bmp; do
  [ -e "$b" ] || continue
  n=$(basename "$b" .bmp)
  python -c "
from PIL import Image, ImageEnhance
im=Image.open('$b')
crop=im.crop((780,400,1160,960))
crop=ImageEnhance.Brightness(crop).enhance(2.2)
crop.save('$OUT/${n}.png')
"
done
ls "$OUT"
