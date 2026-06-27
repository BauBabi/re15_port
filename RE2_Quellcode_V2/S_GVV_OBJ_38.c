/* S_GVV_OBJ_38 @ 0x8008174c  (Ghidra headless, raw MIPS overlay) */

void S_GVV_OBJ_38(ushort param_1,undefined4 param_2,ushort *param_3)

{
  if (0x3fff < param_1) {
    *param_3 = param_1 + 0x8000;
    S_GVV_OBJ_58();
    return;
  }
  *param_3 = param_1;
  return;
}


