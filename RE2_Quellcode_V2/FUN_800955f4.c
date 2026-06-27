/* FUN_800955f4 @ 0x800955f4  (Ghidra headless, raw MIPS overlay) */

void FUN_800955f4(void)

{
  int iVar1;
  undefined4 uVar2;
  undefined4 *puVar3;
  undefined4 *puVar4;
  undefined4 unaff_retaddr;
  
  DAT_800c3a6c = unaff_retaddr;
  FUN_800957a4();
  iVar1 = (*(code *)&SUB_000000b0)();
  iVar1 = *(int *)(iVar1 + 0x16c);
  puVar3 = &DAT_800955c4;
  do {
    uVar2 = *puVar3;
    puVar4 = puVar3 + 1;
    *puVar3 = *(undefined4 *)(iVar1 + 0x9c8);
    *(undefined4 *)(iVar1 + 0x9c8) = uVar2;
    iVar1 = iVar1 + 4;
    puVar3 = puVar4;
  } while (puVar4 != (undefined4 *)&UNK_800955d8);
  FlushCache();
  return;
}


