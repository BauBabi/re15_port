/* FUN_8001d07c @ 0x8001d07c  (Ghidra headless, raw MIPS overlay) */

void FUN_8001d07c(void)

{
  undefined2 *puVar1;
  undefined1 *puVar2;
  
  puVar2 = &UNK_800d8c74;
  puVar1 = (undefined2 *)&UNK_800d8c8c;
  do {
    puVar2 = puVar2 + 0x7c;
    *(undefined1 *)((int)puVar1 + 0x65) = 0;
    *puVar2 = 0;
    puVar1[0x3e] = 0;
    puVar1 = puVar1 + 0x3e;
  } while (puVar2 != &DAT_800dbaf4);
  return;
}


