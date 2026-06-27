/* FUN_80036c5c @ 0x80036c5c  (Ghidra headless, raw MIPS overlay) */

void FUN_80036c5c(int param_1)

{
  int iVar1;
  int iVar2;
  
  iVar2 = (*(int *)(param_1 + 0x38) - DAT_800cfc30) * 0x10000;
  iVar1 = iVar2 >> 0x10;
  if (iVar1 < 0) {
    iVar1 = -iVar1;
  }
  if (iVar1 < 900) {
    DAT_800dcba2 = (ushort)((*(int *)(param_1 + 0x40) - DAT_800cfc38) * 0x10000 >> 0x1f) | 1;
    DAT_800dcb98 = 0;
  }
  else {
    DAT_800dcb98 = (ushort)(iVar2 >> 0x1f) | 1;
    DAT_800dcba2 = 0;
  }
  DAT_800d7638 = *(undefined2 *)(param_1 + 0x74);
  DAT_800d7642 = *(undefined2 *)(param_1 + 0x78);
  return;
}


