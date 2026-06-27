/* setIntr @ 0x800864e0  (Ghidra headless, raw MIPS overlay) */

int setIntr(uint param_1,int param_2)

{
  ushort uVar1;
  ushort uVar2;
  int *piVar3;
  int iVar4;
  
  piVar3 = &DAT_800abc48 + param_1;
  iVar4 = *piVar3;
  if ((param_2 != iVar4) && (DAT_800abc44 != 0)) {
    uVar1 = *(ushort *)PTR_I_MASK_800accd4;
    *(undefined2 *)PTR_I_MASK_800accd4 = 0;
    if (param_2 != 0) {
      *piVar3 = param_2;
      DAT_800abc74 = DAT_800abc74 | (ushort)(1 << (param_1 & 0x1f));
      iVar4 = INTR_OBJ_514();
      return iVar4;
    }
    uVar2 = ~(ushort)(1 << (param_1 & 0x1f));
    *piVar3 = 0;
    DAT_800abc74 = DAT_800abc74 & uVar2;
    if (param_1 == 0) {
      ChangeClearPAD(1);
      ChangeClearRCnt(3,1);
    }
    if (param_1 == 4) {
      ChangeClearRCnt(0,1);
    }
    if (param_1 == 5) {
      ChangeClearRCnt(1,1);
    }
    if (param_1 == 6) {
      ChangeClearRCnt(2,1);
    }
    *(ushort *)PTR_I_MASK_800accd4 = uVar1 & uVar2;
  }
  return iVar4;
}


