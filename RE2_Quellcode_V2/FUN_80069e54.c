/* FUN_80069e54 @ 0x80069e54  (Ghidra headless, raw MIPS overlay) */

void FUN_80069e54(uint param_1)

{
  RECT local_10;
  
  local_10.x = 0x2a8;
  local_10.y = 0x100;
  local_10.w = 0x14;
  local_10.h = 0x1e;
  MoveImage(&local_10,((param_1 % 3) * 0x28 - 0x80 >> 1) + 0x2c0,(param_1 / 3) * 0x1e + 0x100);
  DrawSync(0);
  return;
}


