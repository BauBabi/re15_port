/* FUN_80069eec @ 0x80069eec  (Ghidra headless, raw MIPS overlay) */

void FUN_80069eec(int param_1)

{
  RECT local_10;
  
  local_10.x = 0x1c0;
  local_10.y = 0x100;
  local_10.w = 0x40;
  local_10.h = 0x100;
  if (param_1 == 0) {
    StoreImage(&local_10,(u_long *)&DAT_801b5000);
  }
  else {
    LoadImage(&local_10,(u_long *)&DAT_801b5000);
  }
  DrawSync(0);
  return;
}


