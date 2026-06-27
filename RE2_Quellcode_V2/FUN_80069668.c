/* FUN_80069668 @ 0x80069668  (Ghidra headless, raw MIPS overlay) */

uint FUN_80069668(int param_1)

{
  uint uVar1;
  int iVar2;
  uint uVar3;
  
  uVar1 = 0;
  uVar3 = 0;
  iVar2 = 0;
  while (((&DAT_800d4a3c)[iVar2] != '\0' || (uVar3 = uVar3 + 1, param_1 != 0))) {
    uVar1 = uVar1 + 1;
    iVar2 = iVar2 + 4;
    if (DAT_800d46ac <= uVar1) {
      uVar1 = 0xffffffff;
      if (param_1 != 0) {
        uVar1 = uVar3;
      }
      return uVar1;
    }
  }
  return uVar1;
}


