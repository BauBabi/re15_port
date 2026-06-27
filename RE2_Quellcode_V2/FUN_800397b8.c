/* FUN_800397b8 @ 0x800397b8  (Ghidra headless, raw MIPS overlay) */

char * FUN_800397b8(char *param_1)

{
  uint uVar1;
  
  uVar1 = 0;
  do {
    if (*param_1 == '\0') {
      return param_1;
    }
    uVar1 = uVar1 + 1;
    param_1 = param_1 + 10;
  } while (uVar1 < 0x20);
  return param_1;
}


