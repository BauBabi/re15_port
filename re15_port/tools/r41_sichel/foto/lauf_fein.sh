#!/bin/bash
# Feinlauf beider Fotos (Startliste aus grob_<b>.npy, Sohlen-Anker an); Ausgabe pose_<b>.json, Protokoll pose_<b>b.log
S=C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/sfv4/foto
cd "$(dirname "$0")"
rm -f "$S/pose_1.json" "$S/pose_2.json"
python f_pose.py 1 8 nurfein > "$S/pose_1b.log" 2>&1 &
python f_pose.py 2 8 nurfein > "$S/pose_2b.log" 2>&1 &
wait
date
cut -c1-900 "$S/pose_1b.log" "$S/pose_2b.log"
