/* FUN_80018d64 @ 0x80018d64  (Ghidra headless, raw MIPS overlay) */

int FUN_80018d64(short param_1,short param_2)

{
  ushort uVar1;
  int iVar2;
  int iVar3;
  
  uVar1 = (ushort)(((int)param_1 & 0x80000000U) == 0) << 0xb | 0x400;
  iVar2 = -(int)param_2;
  if (iVar2 != 0) {
    iVar3 = (int)param_1 << 0xc;
    if (iVar2 == 0) {
      trap(0x1c00);
    }
    if ((iVar2 == -1) && (iVar3 == -0x80000000)) {
      trap(0x1800);
    }
    iVar2 = catan(iVar3 / iVar2);
    uVar1 = (ushort)iVar2;
    if (0 < param_2) {
      uVar1 = -uVar1;
    }
  }
  return (int)(short)uVar1;
}


