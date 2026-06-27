/* _SsVmVSetUp @ 0x80084da8  (Ghidra headless, raw MIPS overlay) */

undefined4 _SsVmVSetUp(ushort param_1,short param_2)

{
  undefined4 uVar1;
  int iVar2;
  
  if (param_1 < 0x10) {
    uVar1 = 0xffffffff;
    if ((&DAT_800dcc68)[(short)param_1] == '\x01') {
      iVar2 = (short)param_1 * 4;
      if ((int)DAT_800d7664 <= (int)param_2) goto VM_VSU_OBJ_50;
      DAT_800d7848 = *(undefined4 *)(&DAT_800d5270 + iVar2);
      DAT_800d7838 = *(int *)(&DAT_800d5228 + iVar2);
      DAT_800d784c = *(undefined4 *)(&DAT_800d52b8 + iVar2);
      DAT_800dcc31 = (undefined1)param_1;
      DAT_800dcc36 = (undefined1)param_2;
      uVar1 = 0;
      DAT_800dcc37 = (undefined1)*(undefined4 *)(param_2 * 0x10 + DAT_800d7838 + 8);
    }
    return uVar1;
  }
VM_VSU_OBJ_50:
  uVar1 = VM_VSU_OBJ_BC();
  return uVar1;
}


