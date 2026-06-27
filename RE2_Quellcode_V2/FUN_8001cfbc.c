/* FUN_8001cfbc @ 0x8001cfbc  (Ghidra headless, raw MIPS overlay) */

void FUN_8001cfbc(uint param_1)

{
  undefined2 *puVar1;
  undefined1 *puVar2;
  
  puVar2 = &UNK_800d8c74;
  puVar1 = &DAT_800d8d08;
  do {
    puVar2 = puVar2 + 0x7c;
    if (*(byte *)((int)puVar1 + 5) == param_1) {
      *(undefined1 *)((int)puVar1 + -0x17) = 0;
      *puVar2 = 0;
      *puVar1 = 0;
    }
    puVar1 = puVar1 + 0x3e;
  } while (puVar2 != &DAT_800dbaf4);
  return;
}


