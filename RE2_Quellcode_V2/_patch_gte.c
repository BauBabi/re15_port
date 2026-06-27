/* _patch_gte @ 0x8008f49c  (Ghidra headless, raw MIPS overlay) */

void _patch_gte(void)

{
  int iVar1;
  undefined4 *puVar2;
  undefined4 uVar3;
  code *pcVar4;
  undefined4 unaff_retaddr;
  
  DAT_800c3a4c = unaff_retaddr;
  FUN_800957a4();
  iVar1 = (*(code *)&SUB_000000b0)();
  pcVar4 = (code *)&PATCHGTE_OBJ_68;
  puVar2 = *(undefined4 **)(iVar1 + 0x18);
  do {
    uVar3 = *(undefined4 *)pcVar4;
    pcVar4 = pcVar4 + 4;
    *puVar2 = uVar3;
    puVar2 = puVar2 + 1;
  } while (pcVar4 != LoadTPage);
  FlushCache();
  FUN_800957b4();
  return;
}


