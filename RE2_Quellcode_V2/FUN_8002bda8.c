/* FUN_8002bda8 @ 0x8002bda8  (Ghidra headless, raw MIPS overlay) */

void FUN_8002bda8(int param_1,undefined4 param_2)

{
  undefined *puVar1;
  uint uVar2;
  
  DAT_800dfd54 = (undefined1)param_1;
  if (param_1 == 2) {
    uVar2 = 0;
    puVar1 = &DAT_800dfae0;
    do {
      *(undefined4 *)(puVar1 + 0x8c) = param_2;
      *(undefined4 *)(puVar1 + 0x90) = 0;
      *(undefined4 *)(puVar1 + 0x94) = 0xf00140;
      uVar2 = uVar2 + 1;
      puVar1 = puVar1 + 0x98;
    } while (uVar2 < 2);
  }
  return;
}


