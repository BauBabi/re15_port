/* FUN_80021970 @ 0x80021970  (Ghidra headless, raw MIPS overlay) */

void FUN_80021970(void)

{
  int iVar1;
  int iVar2;
  
  *(undefined1 *)(DAT_800dcbd0 + 2) = 0x40;
  iVar1 = DAT_800dcbd0;
  *(undefined1 *)(DAT_800dcbd0 + 8) = 0;
  *(undefined1 *)(iVar1 + 3) = 0;
  iVar2 = DAT_800dcbd0;
  *(undefined2 *)(iVar1 + 0xc) = 0;
  *(ushort *)(iVar2 + 0x12) = *(ushort *)(iVar2 + 0x12) & 0xfffd;
  return;
}


