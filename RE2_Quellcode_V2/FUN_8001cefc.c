/* FUN_8001cefc @ 0x8001cefc  (Ghidra headless, raw MIPS overlay) */

void FUN_8001cefc(char param_1,char param_2,int param_3)

{
  undefined1 *puVar1;
  uint uVar2;
  
  uVar2 = 0;
  if (param_3 == 0) {
    puVar1 = &DAT_800d8cf0;
    do {
      if ((puVar1[0x1c] == param_1) && (puVar1[0x1e] == param_2)) {
        puVar1[1] = 0;
        *puVar1 = 0;
        *(undefined2 *)(puVar1 + 0x18) = 0;
      }
      uVar2 = uVar2 + 1;
      puVar1 = puVar1 + 0x7c;
    } while (uVar2 < 0x60);
  }
  else {
    puVar1 = &DAT_800d8cf0;
    do {
      if (((puVar1[0x1c] == param_1) && (puVar1[0x1e] == param_2)) &&
         (*(int *)(puVar1 + 0x6c) == param_3)) {
        puVar1[1] = 0;
        *puVar1 = 0;
        *(undefined2 *)(puVar1 + 0x18) = 0;
      }
      uVar2 = uVar2 + 1;
      puVar1 = puVar1 + 0x7c;
    } while (uVar2 < 0x60);
  }
  return;
}


