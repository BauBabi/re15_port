/* FUN_8001d0ac @ 0x8001d0ac  (Ghidra headless, raw MIPS overlay) */

void FUN_8001d0ac(char param_1,char param_2,undefined1 param_3,int param_4)

{
  undefined1 *puVar1;
  uint uVar2;
  
  uVar2 = 0;
  if (param_4 == 0) {
    puVar1 = &DAT_800d8cf0;
    do {
      if ((puVar1[0x1c] == param_1) && (puVar1[0x1e] == param_2)) {
        *puVar1 = param_3;
      }
      uVar2 = uVar2 + 1;
      puVar1 = puVar1 + 0x7c;
    } while (uVar2 < 0x60);
  }
  else {
    puVar1 = &DAT_800d8cf0;
    do {
      if (((puVar1[0x1c] == param_1) && (puVar1[0x1e] == param_2)) &&
         (*(int *)(puVar1 + 0x6c) == param_4)) {
        *puVar1 = param_3;
      }
      uVar2 = uVar2 + 1;
      puVar1 = puVar1 + 0x7c;
    } while (uVar2 < 0x60);
  }
  return;
}


