/* FUN_8002eeb4 @ 0x8002eeb4  (Ghidra headless, raw MIPS overlay) */

void FUN_8002eeb4(int param_1,int param_2)

{
  ushort *puVar1;
  int iVar2;
  int iVar3;
  
  iVar3 = *(int *)(param_1 + 0x10);
  iVar2 = *(int *)(*(int *)(param_1 + 4) + 0x14) * 2;
  if (iVar2 != 0) {
    puVar1 = (ushort *)(*(int *)(param_1 + 8) + 0x1a);
    do {
      iVar2 = iVar2 + -1;
      *puVar1 = *puVar1 & 0xff9f | (ushort)(param_2 << 5);
      puVar1 = puVar1 + 0x14;
    } while (iVar2 != 0);
  }
  iVar2 = *(int *)(*(int *)(param_1 + 0xc) + 0x14) * 2;
  if (iVar2 != 0) {
    puVar1 = (ushort *)(iVar3 + 0x1a);
    do {
      iVar2 = iVar2 + -1;
      *puVar1 = *puVar1 & 0xff9f | (ushort)(param_2 << 5);
      puVar1 = puVar1 + 0x1a;
    } while (iVar2 != 0);
  }
  return;
}


