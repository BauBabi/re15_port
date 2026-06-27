undefined4 SpuVmVSetUp(ushort param_1,short param_2)

{
  undefined4 uVar1;
  int iVar2;
  
  if (param_1 < 0x10) {
    uVar1 = 0xffffffff;
    if ((&DAT_800b5334)[(short)param_1] == '\x01') {
      iVar2 = (short)param_1 * 4;
      if ((int)DAT_800b284c <= (int)param_2) goto VM_VSU_OBJ_50;
      DAT_800b2b34 = *(undefined4 *)(&DAT_800b2500 + iVar2);
      DAT_800b2b28 = *(int *)(&DAT_800b24b8 + iVar2);
      DAT_800b2b3c = *(undefined4 *)(&DAT_800b2544 + iVar2);
      DAT_800b5315 = (undefined1)param_1;
      DAT_800b531a = (undefined1)param_2;
      DAT_800b531b = *(undefined1 *)(param_2 * 0x10 + DAT_800b2b28 + 8);
      uVar1 = 0;
    }
    return uVar1;
  }
VM_VSU_OBJ_50:
  uVar1 = VM_VSU_OBJ_BC();
  return uVar1;
}
