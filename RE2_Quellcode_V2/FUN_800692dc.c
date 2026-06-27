/* FUN_800692dc @ 0x800692dc  (Ghidra headless, raw MIPS overlay) */

uint FUN_800692dc(char param_1)

{
  char *pcVar1;
  uint uVar2;
  
  uVar2 = 0;
  pcVar1 = &DAT_800d4b68;
  do {
    if (*pcVar1 == -1) {
      *pcVar1 = param_1;
      return uVar2;
    }
    uVar2 = uVar2 + 1;
    pcVar1 = pcVar1 + 1;
  } while (uVar2 < 0x18);
  return 0;
}


