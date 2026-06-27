/* FUN_8009553c @ 0x8009553c  (Ghidra headless, raw MIPS overlay) */

void FUN_8009553c(void)

{
  int iVar1;
  undefined4 *puVar2;
  undefined4 uVar3;
  undefined4 *puVar4;
  undefined4 unaff_retaddr;
  
  DAT_800c3a6c = unaff_retaddr;
  FUN_800957a4();
  iVar1 = (*(code *)&SUB_000000b0)();
  puVar4 = &DAT_800954e4;
  puVar2 = (undefined4 *)
           (*(int *)(*(int *)(iVar1 + 0x18) + 0x70) * 0x10000 +
            (*(uint *)(*(int *)(iVar1 + 0x18) + 0x74) & 0xffff) + 0x28);
  do {
    uVar3 = *puVar4;
    puVar4 = puVar4 + 1;
    DAT_800c3a70 = puVar2 + 1;
    *puVar2 = uVar3;
    puVar2 = DAT_800c3a70;
  } while (puVar4 != (undefined4 *)&UNK_800954f8);
  FlushCache();
  return;
}


