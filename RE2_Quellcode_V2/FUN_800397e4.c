/* FUN_800397e4 @ 0x800397e4  (Ghidra headless, raw MIPS overlay) */

int FUN_800397e4(char *param_1)

{
  uint uVar1;
  int iVar2;
  
  iVar2 = 0;
  uVar1 = 0;
  do {
    if (*param_1 != '\0') {
      iVar2 = iVar2 + 1;
    }
    uVar1 = uVar1 + 1;
    param_1 = param_1 + 10;
  } while (uVar1 < 0x20);
  return iVar2;
}


