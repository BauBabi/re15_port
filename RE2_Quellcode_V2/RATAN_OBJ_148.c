/* RATAN_OBJ_148 @ 0x8008f464  (Ghidra headless, raw MIPS overlay) */

int RATAN_OBJ_148(undefined4 param_1,undefined4 param_2,int param_3,int param_4)

{
  int in_v0;
  int iVar1;
  
  iVar1 = 0x400 - *(short *)(&DAT_800b1eac + in_v0);
  if (param_3 != 0) {
    iVar1 = 0x800 - iVar1;
  }
  if (param_4 != 0) {
    iVar1 = -iVar1;
  }
  return iVar1;
}


