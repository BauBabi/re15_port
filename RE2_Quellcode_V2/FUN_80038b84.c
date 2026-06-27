/* FUN_80038b84 @ 0x80038b84  (Ghidra headless, raw MIPS overlay) */

/* WARNING: Type propagation algorithm not settling */

void FUN_80038b84(void)

{
  uint uVar1;
  uint *puVar2;
  uint uStack_15;
  uint auStack_11 [4];
  
  uVar1 = (uint)&uStack_15 & 3;
  puVar2 = (uint *)((int)&uStack_15 - uVar1);
  *puVar2 = *puVar2 & -1 << (uVar1 + 1) * 8 | DAT_80010b9c >> (3 - uVar1) * 8;
  uVar1 = (uint)auStack_11 & 3;
  puVar2 = (uint *)((int)auStack_11 - uVar1);
  *puVar2 = *puVar2 & -1 << (uVar1 + 1) * 8 | DAT_80010ba0 >> (3 - uVar1) * 8;
  return;
}


