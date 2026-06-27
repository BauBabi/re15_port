/* FUN_800769c4 @ 0x800769c4  (Ghidra headless, raw MIPS overlay) */

void FUN_800769c4(short param_1,short param_2,short param_3,short param_4)

{
  RECT local_10;
  
  local_10.x = param_1;
  local_10.y = param_2;
  local_10.w = param_3;
  local_10.h = param_4;
  ClearImage(&local_10,'\0','\0','\0');
  return;
}


