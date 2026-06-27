/* FUN_8002c7dc @ 0x8002c7dc  (Ghidra headless, raw MIPS overlay) */

int FUN_8002c7dc(uint param_1)

{
  byte bVar1;
  int iVar2;
  
  bVar1 = *(byte *)(*(int *)(DAT_800ce324 + 0x28) + 2);
  iVar2 = *(int *)(DAT_800ce324 + 0x28);
  while (bVar1 != param_1) {
    bVar1 = *(byte *)(iVar2 + 0x16);
    iVar2 = iVar2 + 0x14;
  }
  return iVar2;
}


