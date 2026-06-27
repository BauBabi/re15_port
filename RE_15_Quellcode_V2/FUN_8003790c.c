void FUN_8003790c(int *param_1,int *param_2,int param_3)

{
  long lVar1;
  int iVar2;
  uint uVar3;
  int iVar4;
  uint uVar5;
  
  iVar4 = *param_2 - *param_1;
  uVar3 = param_2[2] - param_1[2];
  uVar5 = param_1[1] - param_2[1];
  lVar1 = SquareRoot0(iVar4 * iVar4 + uVar3 * uVar3);
  if (uVar3 != 0) {
    *(ushort *)(param_3 + 2) = (ushort)((uVar3 & 0x80000000) == 0) * 0x800 + 0x400;
  }
  if (iVar4 != 0) {
    if (iVar4 == 0) {
      trap(0x1c00);
    }
    if ((iVar4 == -1) && (uVar3 * 0x1000 == -0x80000000)) {
      trap(0x1800);
    }
    iVar2 = catan((int)(uVar3 * 0x1000) / iVar4);
    *(ushort *)(param_3 + 2) =
         (0x1000 - (short)iVar2) - ((ushort)((uint)iVar4 >> 0x14) & 0x800) & 0xfff;
  }
  if (uVar5 != 0) {
    *(ushort *)(param_3 + 4) = (ushort)((uVar5 & 0x80000000) == 0) * 0x800 + 0xc00;
  }
  if (lVar1 != 0) {
    if (lVar1 == 0) {
      trap(0x1c00);
    }
    if ((lVar1 == -1) && (uVar5 * 0x1000 == -0x80000000)) {
      trap(0x1800);
    }
    iVar4 = catan((int)(uVar5 * 0x1000) / lVar1);
    *(ushort *)(param_3 + 4) = 0x1000U - (short)iVar4 & 0xfff;
  }
  return;
}
