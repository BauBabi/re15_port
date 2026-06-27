/* FUN_80069d88 @ 0x80069d88  (Ghidra headless, raw MIPS overlay) */

void FUN_80069d88(uint param_1,uint param_2)

{
  RECT local_10;
  
  local_10.w = 0x14;
  local_10.h = 0x1e;
  local_10.x = (short)((param_1 % 3) * 0x28 - 0x80 >> 1) + 0x2c0;
  local_10.y = (short)(param_1 / 3) * 0x1e + 0x100;
  MoveImage(&local_10,((param_2 % 3) * 0x28 - 0x80 >> 1) + 0x2c0,(param_2 / 3) * 0x1e + 0x100);
  DrawSync(0);
  return;
}


