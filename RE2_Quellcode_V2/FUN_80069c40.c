/* FUN_80069c40 @ 0x80069c40  (Ghidra headless, raw MIPS overlay) */

void FUN_80069c40(uint param_1,int param_2,u_long *param_3)

{
  RECT local_20;
  
  local_20.w = 0x14;
  local_20.h = 0x1e;
  local_20.x = (short)((param_1 % 3) * 0x28 - 0x80 >> 1) + 0x200;
  local_20.y = (short)(param_1 / 3) * 0x1e + 0x100;
  LoadImage(&local_20,param_3);
  DrawSync(0);
  if ((param_2 != 0x13) && (param_2 - 0xeU < 6)) {
    local_20.x = (short)(((param_1 + 1) % 3) * 0x28 - 0x80 >> 1) + 0x200;
    local_20.y = (short)((param_1 + 1) / 3) * 0x1e + 0x100;
    LoadImage(&local_20,param_3);
    DrawSync(0);
    LoadImage(&local_20,param_3 + 0x64c8);
    DrawSync(0);
  }
  return;
}


