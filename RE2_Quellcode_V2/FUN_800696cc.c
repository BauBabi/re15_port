/* FUN_800696cc @ 0x800696cc  (Ghidra headless, raw MIPS overlay) */

uint FUN_800696cc(uint param_1)

{
  uint uVar1;
  int iVar2;
  
  uVar1 = 0;
  iVar2 = 0;
  do {
    if ((byte)(&DAT_800d4a3c)[iVar2] == param_1) {
      return uVar1;
    }
    uVar1 = uVar1 + 1;
    iVar2 = iVar2 + 4;
  } while (uVar1 < DAT_800d46ac);
  return 0xffffffff;
}


