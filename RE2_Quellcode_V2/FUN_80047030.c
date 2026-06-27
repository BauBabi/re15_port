/* FUN_80047030 @ 0x80047030  (Ghidra headless, raw MIPS overlay) */

uint FUN_80047030(int param_1,int param_2)

{
  int iVar1;
  int iVar2;
  uint uVar3;
  short sVar4;
  
  sVar4 = 0x400;
  if (0 < param_2) {
    sVar4 = 0xc00;
  }
  if (param_1 == 0) {
    uVar3 = (uint)sVar4;
  }
  else {
    if (param_1 == 0) {
      trap(0x1c00);
    }
    if ((param_1 == -1) && (param_2 << 0xc == -0x80000000)) {
      trap(0x1800);
    }
    iVar1 = catan((param_2 << 0xc) / param_1);
    iVar2 = 0;
    if (param_1 < 0) {
      iVar2 = 0x800;
    }
    uVar3 = iVar2 - iVar1 & 0xfff;
  }
  return uVar3;
}


